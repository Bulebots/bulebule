from numpy import array
from numpy import isclose
from numpy import tan


class Line:
    def __init__(self, x, y, angle):
        self.reference = array((x, y))
        self.angle = angle
        self.slope = tan(angle)
        self.intercept = y - x * self.slope


def lines_intersection(l0, l1):
    """Calculate the intersection of two lines."""
    if isclose(l0.slope, l1.slope):
        raise ValueError('Lines are parallel!')
    x = (l1.intercept - l0.intercept) / (l0.slope - l1.slope)
    y = l0.slope * x + l0.intercept
    return array((x, y))
