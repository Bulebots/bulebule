from itertools import product
from pathlib import Path

from pyqtgraph import (
    mkBrush,
    mkPen,
    GraphicsObject,
    GraphicsWindow,
    QtCore,
    QtGui,
    setConfigOptions,
)

from mazes import load_maze
from mazes import EAST_BIT
from mazes import SOUTH_BIT
from mazes import WEST_BIT
from mazes import NORTH_BIT
from mazes import VISITED_BIT


CELL_WIDTH = 180
WALL_WIDTH = 12
MAZE_SIZE = 16
MAZE_TARGET = (10, 8)

GRAY = (100, 100, 100)
WHITE = (255, 255, 255)
GREEN = (0, 255, 0)


def paint_walls(painter, walls, color):
    for (x, y) in product(range(MAZE_SIZE + 1), repeat=2):
        painter.setBrush(mkBrush(color))
        painter.setPen(mkPen(None))
        painter.drawRect(QtCore.QRectF(
            x * CELL_WIDTH - WALL_WIDTH / 2,
            -y * CELL_WIDTH + WALL_WIDTH / 2,
            WALL_WIDTH,
            WALL_WIDTH,
        ))
        if x >= MAZE_SIZE or y >= MAZE_SIZE:
            continue
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
            "%s" % distances[y][x])


class MazeItem(GraphicsObject):
    def __init__(self, distances, walls, template):
        super().__init__()
        self.distances = distances
        self.walls = walls
        self.template = template
        self.generatePicture()

    def generatePicture(self):
        self.picture = QtGui.QPicture()
        painter = QtGui.QPainter(self.picture)
        painter.setFont(QtGui.QFont('times', 50))
        painter.scale(1, -1)
        if self.template is not None:
            paint_walls(painter=painter, walls=template, color=GRAY)
        paint_discovered(painter, self.distances, self.walls)
        painter.end()

    def paint(self, p, *args):
        p.drawPicture(0, 0, self.picture)

    def boundingRect(self):
        return QtCore.QRectF(self.picture.boundingRect())


# Distances as received (byte array)
distances = bytes([abs(x - MAZE_TARGET[0]) + abs(y - MAZE_TARGET[1])
                  for y in range(MAZE_SIZE) for x in range(MAZE_SIZE)])

# Distances as stored internally
distances = list(distances)
distances = [distances[i*MAZE_SIZE:i*MAZE_SIZE+MAZE_SIZE]
             for i in range(MAZE_SIZE)]

# Template walls
template_file = Path('./mazes/00.txt')
template = load_maze(template_file)

setConfigOptions(antialias=True)
w = GraphicsWindow()
w.setWindowTitle('Maze viewer')
v = w.addViewBox()
v.setAspectLocked()

maze = MazeItem(distances=distances, walls=template, template=template)
v.addItem(maze)


if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()
