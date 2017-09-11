"""
This script defines the rim body.
"""
from math import cos
from math import sin
from math import radians

import cadquery
from Helpers import show


REEL_D0 = 19.
REEL_H0 = 5.

REEL_D1 = 14.8
REEL_H1 = 3.

BEARING_D = 6.2

MINIHOLES = 9
MINIHOLE_D = 2.5


# Rim body
rim = cadquery.Workplane('XY')\
    .circle(radius=REEL_D0/2.).extrude(distance=REEL_H0)\
    .faces('>Z').workplane()\
    .circle(radius=REEL_D1/2.).extrude(distance=REEL_H1)\
    .faces('>Z').workplane()\
    .hole(diameter=BEARING_D)

# Mini-holes
minihole_position = (REEL_D1 + BEARING_D) / 4.
miniholes = [(minihole_position * sin(radians(i * 360. / MINIHOLES)),
              minihole_position * cos(radians(i * 360. / MINIHOLES)))
             for i in range(MINIHOLES)]
rim = rim.faces('>Z').workplane()\
    .pushPoints(miniholes).hole(diameter=MINIHOLE_D)

show(rim)
