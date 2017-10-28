from itertools import product
from pathlib import Path
import time
import sys
import struct

import numpy
from pyqtgraph import (
    mkBrush,
    mkPen,
    GraphicsObject,
    GraphicsWindow,
    QtCore,
    QtGui,
    setConfigOptions,
)
import zmq

from mazes import load_maze
from mazes import read_walls

from mazes import EAST_BIT
from mazes import SOUTH_BIT
from mazes import WEST_BIT
from mazes import NORTH_BIT
from mazes import VISITED_BIT

from mazes import MAZE_SIZE


CELL_WIDTH = 180
WALL_WIDTH = 12

GRAY = (100, 100, 100)
GREEN = (0, 255, 0)
RED = (255, 0, 0)
WHITE = (255, 255, 255)


def paint_walls(painter, walls, color):
    painter.setBrush(mkBrush(color))
    painter.setPen(mkPen(None))
    for (x, y) in product(range(MAZE_SIZE), repeat=2):
        wall = walls[y][x]
        if wall & EAST_BIT:
            painter.drawRect(QtCore.QRectF(
                (y + 1) * CELL_WIDTH - WALL_WIDTH / 2,
                -(x + 1) * CELL_WIDTH + WALL_WIDTH / 2,
                WALL_WIDTH,
                CELL_WIDTH,
            ))
        if wall & SOUTH_BIT:
            painter.drawRect(QtCore.QRectF(
                y * CELL_WIDTH + WALL_WIDTH / 2,
                -x * CELL_WIDTH + WALL_WIDTH / 2,
                CELL_WIDTH,
                WALL_WIDTH,
            ))
        if wall & WEST_BIT:
            painter.drawRect(QtCore.QRectF(
                y * CELL_WIDTH - WALL_WIDTH / 2,
                -(x + 1) * CELL_WIDTH + WALL_WIDTH / 2,
                WALL_WIDTH,
                CELL_WIDTH,
            ))
        if wall & NORTH_BIT:
            painter.drawRect(QtCore.QRectF(
                y * CELL_WIDTH + WALL_WIDTH / 2,
                -(x + 1) * CELL_WIDTH + WALL_WIDTH / 2,
                CELL_WIDTH,
                WALL_WIDTH,
            ))


def paint_discovered(painter, distances, walls):
    if walls is not None:
        paint_walls(painter, walls, color=WHITE)
    for (x, y) in product(range(MAZE_SIZE), repeat=2):
        painter.setPen(mkPen(color=GRAY))
        if walls is not None:
            wall = walls[x][y]
            if wall & VISITED_BIT:
                painter.setPen(mkPen(color=GREEN))
        painter.drawText(QtCore.QRectF(
            (x + .5) * CELL_WIDTH + WALL_WIDTH / 2 - 50,
            -(y + .5) * CELL_WIDTH + WALL_WIDTH / 2 - 50,
            100, 100),
            QtCore.Qt.AlignCenter,
            "%s" % distances[x][y])


def paint_template(painter, walls):
    if walls is not None:
        paint_walls(painter=painter, walls=walls, color=GRAY)
    for (x, y) in product(range(MAZE_SIZE + 1), repeat=2):
        painter.setBrush(mkBrush(WHITE))
        painter.setPen(mkPen(None))
        painter.drawRect(QtCore.QRectF(
            x * CELL_WIDTH - WALL_WIDTH / 2,
            -y * CELL_WIDTH + WALL_WIDTH / 2,
            WALL_WIDTH,
            WALL_WIDTH,
        ))


def paint_position(painter, x, y, direction):
    painter.setBrush(mkBrush(RED))
    painter.setPen(mkPen(None))
    print(x, y, direction)
    painter.drawRect(QtCore.QRectF(
        x * CELL_WIDTH,
        -y * CELL_WIDTH - 100,
        50,
        100,
    ))


class MazeItem(GraphicsObject):
    def __init__(self, template):
        super().__init__()
        self.distances = None
        self.walls = None
        self.template = template
        self.x = 0
        self.y = 0
        self.direction = 0

        self.picture = QtGui.QPicture()
        self.position_picture = QtGui.QPicture()

        self.context = zmq.Context()
        self.puller = self.context.socket(zmq.REP)
        self.puller.bind('tcp://127.0.0.1:6574')
        self.poller = zmq.Poller()
        self.poller.register(self.puller, zmq.POLLIN)

        self.generateTemplate()

    def generateTemplate(self):
        self.template_picture = QtGui.QPicture()
        painter = QtGui.QPainter(self.template_picture)
        painter.scale(1, -1)
        paint_template(painter=painter, walls=self.template)
        painter.end()

    def generatePicture(self):
        self.picture = QtGui.QPicture()
        painter = QtGui.QPainter(self.picture)
        painter.setFont(QtGui.QFont('times', 50))
        painter.scale(1, -1)
        paint_discovered(painter, distances=self.distances, walls=self.walls)
        painter.end()

    def generatePosition(self):
        self.position_picture = QtGui.QPicture()
        painter = QtGui.QPainter(self.position_picture)
        painter.scale(1, -1)
        paint_position(painter, x=self.x, y=self.y, direction=self.direction)
        painter.end()

    def paint(self, p, *args):
        p.drawPicture(0, 0, self.template_picture)
        p.drawPicture(0, 0, self.position_picture)
        p.drawPicture(0, 0, self.picture)

    def boundingRect(self):
        return QtCore.QRectF(self.template_picture.boundingRect())

    def update(self):
        while True:
            events = dict(self.poller.poll(0))
            if not events:
                break
            for socket in events:
                if events[socket] != zmq.POLLIN:
                    continue
                self.process_socket(socket)

    def process_socket(self, socket):
        print(time.time())
        message = socket.recv()
        if message.startswith(b'D'):
            self.update_discovery(message.lstrip(b'D'))
            socket.send(b'ok')
        elif message.startswith(b'P'):
            walls = self.update_position(message.lstrip(b'P'))
            print('Walls: ', walls)
            socket.send(struct.pack('3B', *walls))

    def update_position(self, position):
        print('Received position: ', position)
        self.x, self.y, self.direction = struct.unpack('3B', position)
        self.generatePosition()
        self.informViewBoundsChanged()
        return read_walls(self.template, self.x, self.y, self.direction)

    def update_discovery(self, discovery):
        order = chr(discovery[0])
        distances = discovery[1:257]
        distances = numpy.frombuffer(distances, dtype='uint8')
        distances = distances.reshape(MAZE_SIZE, MAZE_SIZE).T
        if order == 'F':
            distances = distances.T

        order = chr(discovery[257])
        walls = discovery[258:]
        walls = numpy.frombuffer(walls, dtype='uint8')
        walls = walls.reshape(MAZE_SIZE, MAZE_SIZE).T
        if order == 'F':
            walls = walls.T

        self.walls = walls
        self.distances = distances
        self.generatePicture()
        self.informViewBoundsChanged()


# Template walls
template_file = Path('./mazes/00.txt')
template = load_maze(template_file)

setConfigOptions(antialias=True)


def run():
    window = GraphicsWindow()
    window.setWindowTitle('Maze viewer')
    view = window.addViewBox()
    view.setAspectLocked()

    maze = MazeItem(template=template)
    view.addItem(maze)

    timer = QtCore.QTimer()
    timer.timeout.connect(maze.update)
    timer.start(10)

    sys.exit(QtGui.QApplication.instance().exec_())


if __name__ == '__main__':
    run()
