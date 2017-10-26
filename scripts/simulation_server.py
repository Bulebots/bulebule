from itertools import product
from pathlib import Path
import time
import sys

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
from mazes import EAST_BIT
from mazes import SOUTH_BIT
from mazes import WEST_BIT
from mazes import NORTH_BIT
from mazes import VISITED_BIT


CELL_WIDTH = 180
WALL_WIDTH = 12
MAZE_SIZE = 16

GRAY = (100, 100, 100)
WHITE = (255, 255, 255)
GREEN = (0, 255, 0)


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
    paint_walls(painter, walls, color=WHITE)
    for (x, y) in product(range(MAZE_SIZE), repeat=2):
        wall = walls[y][x]
        if wall & VISITED_BIT:
            painter.setPen(mkPen(color=GREEN))
        else:
            painter.setPen(mkPen(color=GRAY))
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


class MazeItem(GraphicsObject):
    def __init__(self, template):
        super().__init__()
        self.distances = None
        self.walls = None
        self.template = template

        self.picture = QtGui.QPicture()

        self.context = zmq.Context()
        self.puller = self.context.socket(zmq.PULL)
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

    def paint(self, p, *args):
        p.drawPicture(0, 0, self.template_picture)
        p.drawPicture(0, 0, self.picture)

    def boundingRect(self):
        return QtCore.QRectF(self.template_picture.boundingRect())

    def update(self):
        message = None
        while True:
            events = dict(self.poller.poll(0))
            if not events:
                break
            for socket in events:
                if events[socket] != zmq.POLLIN:
                    continue
                message = socket.recv()
        if not message:
            return
        print(time.time())

        order = message[0]
        distances = message[1:257]
        distances = numpy.frombuffer(distances, dtype='uint8')
        distances = distances.reshape(MAZE_SIZE, MAZE_SIZE).T
        if order == 'F':
            distances = distances.T
        print(distances)

        order = message[257]
        walls = message[258:]
        walls = numpy.frombuffer(walls, dtype='uint8')
        walls = walls.reshape(MAZE_SIZE, MAZE_SIZE)
        if order == 'F':
            walls = walls.T
        print(walls)

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
    timer.start(100)

    sys.exit(QtGui.QApplication.instance().exec_())


if __name__ == '__main__':
    run()
