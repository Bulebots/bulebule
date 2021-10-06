"""
This script defines the mount for the wheels and motor.
"""
from math import asin
from math import cos
from math import sin
from math import radians

import cadquery


def circle_points(number, circle, shift=0):
    radius = circle / 2.0
    step = 360 / number
    points = [
        (
            radius * sin(radians(i * step + shift)),
            radius * cos(radians(i * step + shift)),
        )
        for i in range(number)
    ]
    return points


# Gears
MODULE = 0.3
Z_PINION = 15
Z_GEAR = 60

# Motor
MOTOR_SHIFT = 1.5
MOTOR_DIAMETER = 15
MOTOR_HOLE_DIAMETER = 6.5
MOTOR_MOUNT_THICK = 1.5
MOUNT_MINIHOLES_CIRCLE = 10
MOUNT_MINIHOLES_DIAMETER = 1.7

# Mount
MOUNT_THICK = 6
MOUNT_HEIGHT = MOTOR_DIAMETER
MOUNT_WIDTH = 40
MOUNT_FILLET = 1

# Holes
SCREW_SPACE = 34
SCREW_DIAMETER = 2
AXIS_DIAMETER = 3


# Basic mount structure
mount = cadquery.Workplane("XY").box(MOUNT_WIDTH, MOUNT_HEIGHT, MOUNT_THICK)

# Base screws
mount = (
    mount.faces("<Y")
    .workplane()
    .pushPoints([(-SCREW_SPACE / 2.0, 0), (SCREW_SPACE / 2.0, 0)])
    .hole(SCREW_DIAMETER)
)

# Motor holes
miniholes = circle_points(number=6, circle=MOUNT_MINIHOLES_CIRCLE)
mount = (
    mount.faces("<Z")
    .workplane(centerOption="CenterOfMass")
    .pushPoints(miniholes)
    .hole(diameter=MOUNT_MINIHOLES_DIAMETER)
)
mount = (
    mount.faces(">Z")
    .workplane()
    .cboreHole(
        MOTOR_HOLE_DIAMETER,
        MOTOR_DIAMETER + 1,
        MOUNT_THICK - MOTOR_MOUNT_THICK,
    )
)

# Axis
axis_shift = (MODULE * (Z_PINION + Z_GEAR)) / 2.0
axis_shift *= cos(asin(MOTOR_SHIFT / axis_shift))
mount = (
    mount.faces("<Z")
    .workplane()
    .pushPoints([(axis_shift, -MOTOR_SHIFT), (-axis_shift, -MOTOR_SHIFT)])
    .hole(AXIS_DIAMETER)
)

# Fillet
mount = mount.edges("|Z").fillet(MOUNT_FILLET)
