"""
Simple script to visualize emitters configuration.
"""
from itertools import product
from math import sin
from math import cos
from math import pi
from math import radians

import pyqtgraph as pg
from pyqtgraph import QtCore, QtGui


CELL_SIZE = 180
WALL_WIDTH = 12
WALL_LENGTH = CELL_SIZE - WALL_WIDTH


configuration = {
    'emitters': [
        # (x, y, rotation)
        (-34, 29, -8),
        (-4, 49.5, -60),
        (4, 49.5, 60),
        (34, 29, 8),
    ],
    'half_emission_angle': 3,
    'x': 0,
    'y': -CELL_SIZE / 2,
}


def post(x, y):
    return QtCore.QRectF(x, y, WALL_WIDTH, WALL_WIDTH)


def wall(x, y, vertical=False):
    if vertical:
        return QtCore.QRectF(x, y, WALL_WIDTH, WALL_LENGTH)
    return QtCore.QRectF(x, y, WALL_LENGTH, WALL_WIDTH)


class WorldItem(pg.GraphicsObject):
    def __init__(self):
        pg.GraphicsObject.__init__(self)
        self.generatePicture()

    def generatePicture(self):
        self.picture = QtGui.QPicture()
        p = QtGui.QPainter(self.picture)
        p.setPen(pg.mkPen('#555555'))
        p.setBrush(pg.mkBrush(None))
        # Posts
        for x, y in product((0, 1), (0, 1, 2)):
            p.drawRect(post(x * CELL_SIZE, y * CELL_SIZE))
        # Vertical walls
        for x, y in product((0, 1), (0, 1)):
            p.drawRect(wall(x * CELL_SIZE, y * CELL_SIZE + WALL_WIDTH, True))
        # Horizontal walls
        for i in range(3):
            p.drawRect(wall(WALL_WIDTH, i * CELL_SIZE))
        p.end()

    def paint(self, p, *args):
        p.drawPicture(0, 0, self.picture)

    def boundingRect(self):
        return QtCore.QRectF(self.picture.boundingRect())


class MouseItem(pg.GraphicsObject):
    def __init__(self, configuration):
        pg.GraphicsObject.__init__(self)
        self.configuration = configuration
        self.generatePicture()

    def generatePicture(self):
        self.picture = QtGui.QPicture()
        p = QtGui.QPainter(self.picture)
        p.setBrush(pg.mkBrush(None))
        # Mouse center
        p.setPen(pg.mkPen('b'))
        for i in range(4):
            p.drawLine(QtCore.QPointF(0, 0),
                       QtCore.QPointF(cos(i * pi / 2) * 30,
                                      sin(i * pi / 2) * 30))
        # Emitters
        p.setPen(pg.mkPen('#ffd70080'))
        for x, y, rotation in self.configuration['emitters']:
            for i in [1, 0, -1]:
                diff = i * self.configuration['half_emission_angle']
                angle = radians(rotation + diff)
                p.drawLine(QtCore.QPointF(x, y),
                           QtCore.QPointF(x + sin(angle) * 400,
                                          y + cos(angle) * 400))
        p.end()

    def paint(self, p, *args):
        mouse_x = WALL_WIDTH + WALL_LENGTH / 2 + self.configuration['x']
        mouse_y = CELL_SIZE + WALL_WIDTH / 2 + self.configuration['y']
        p.drawPicture(mouse_x, mouse_y, self.picture)

    def boundingRect(self):
        return QtCore.QRectF(self.picture.boundingRect())


world = WorldItem()
mouse = MouseItem(configuration)

plt = pg.plot()
plt.addItem(world)
plt.addItem(mouse)
plt.setWindowTitle('Micromouse emitter configuration')
plt.setAspectLocked()


if __name__ == '__main__':
    import sys
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()
