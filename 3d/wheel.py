"""
This script defines the wheel body.
"""
from math import cos
from math import sin
from math import radians

import cadquery
from Helpers import show


TIRE_H0 = 6
TIRE_H1 = 10
TIRE_D0 = 16
TIRE_D1 = 18
BEARING_D = 6
MINIHOLES = 8
MINIHOLE_D = 2


# Wheel body
wheel = cadquery.Workplane('XY')\
    .circle(radius=TIRE_D1/2).extrude(distance=TIRE_H0)\
    .faces('>Z').workplane()\
    .circle(radius=TIRE_D0/2).extrude(distance=(TIRE_H1-TIRE_H0)/2.)\
    .faces('>Z').workplane()\
    .hole(diameter=BEARING_D)

# Mini-holes
minihole_position = (TIRE_D0 + BEARING_D) / 4.
miniholes = [(minihole_position * sin(radians(i * 360 / MINIHOLES)),
              minihole_position * cos(radians(i * 360 / MINIHOLES)))
             for i in range(MINIHOLES)]
wheel = wheel.faces('>Z').workplane()\
    .pushPoints(miniholes).hole(diameter=MINIHOLE_D)

show(wheel)
