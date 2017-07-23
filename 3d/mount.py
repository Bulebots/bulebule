"""
This script defines the mount for the wheels and motor.
"""
from math import cos
from math import sin
from math import radians

import cadquery
from Helpers import show


def circle_points(number, circle, shift=0):
    radius = circle / 2.
    step = 360 / number
    points = [(radius * sin(radians(i * step + shift)),
               radius * cos(radians(i * step + shift)))
              for i in range(number)]
    return points


# Gears
MODULE = 0.3
Z_PINION = 14
Z_GEAR = 60

# Motor
MOTOR_DIAMETER = 15
MOTOR_WHOLE_DIAMETER = 6.5
MOTOR_MOUNT_THICK = 1.5
MOUNT_MINIHOLES_CIRCLE = 10
MOUNT_MINIHOLES_DIAMETER = 1.7

# Mount
MOUNT_THICK = 8
MOUNT_TO_FLOOR = 5
WHEEL_DIAMETER = 20

# Axis
AXIS_INTERNAL_DIAMETER = 3
AXIS_EXTERNAL_DIAMETER = 8

# Base
BASE_SCREW_DIAMETER = 2
BASE_THICK = 2


# Distance between the two wheel axis
pinion_reference_diameter = Z_PINION * MODULE
gear_reference_diameter = Z_GEAR * MODULE
axis_to_axis = pinion_reference_diameter + gear_reference_diameter

# Basic mount structure
mount_width = axis_to_axis + AXIS_EXTERNAL_DIAMETER
mount_height = WHEEL_DIAMETER - MOUNT_TO_FLOOR * 2
mount = cadquery.Workplane('XY').box(mount_width, mount_height, MOUNT_THICK)

# Axis
mount = mount.faces('<Z').workplane()\
    .pushPoints([(axis_to_axis / 2, 0), (-axis_to_axis / 2, 0)])\
    .hole(AXIS_INTERNAL_DIAMETER)

# Motor holes
miniholes = circle_points(number=6, circle=MOUNT_MINIHOLES_CIRCLE)
miniholes = [miniholes[i] for i in (1, 2, 4, 5)]
mount = mount.faces('>Z').workplane()\
    .pushPoints(miniholes)\
    .hole(diameter=MOUNT_MINIHOLES_DIAMETER)
mount = mount.faces('<Z').workplane().cboreHole(
    MOTOR_WHOLE_DIAMETER,
    MOTOR_DIAMETER + 1,
    MOUNT_THICK - MOTOR_MOUNT_THICK)

# Base
mount = mount.faces('<X').workplane()\
    .center((mount_height - BASE_THICK) / 2, 0)\
    .rect(BASE_THICK, MOUNT_THICK).extrude(MOUNT_THICK)
mount = mount.faces('>X').workplane()\
    .center(-(mount_height - BASE_THICK) / 2, 0)\
    .rect(BASE_THICK, MOUNT_THICK).extrude(MOUNT_THICK)
screw_position = (mount_width + MOUNT_THICK) / 2
mount = mount.faces('<Y').workplane()\
    .pushPoints([(screw_position, 0), (-screw_position, 0)])\
    .hole(diameter=BASE_SCREW_DIAMETER)

# Fillet
mount = mount.faces('>Y').edges('|Z').fillet(3)

# Render the solid
show(mount)
