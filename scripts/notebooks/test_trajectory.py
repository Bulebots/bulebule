from numpy import pi
import pytest
from pytest import approx

from trajectory import Line
from trajectory import lines_intersection


@pytest.mark.parametrize('l0,l1,intersection', [
    (Line(x=0., y=-0.09, angle=0.), Line(x=0.09, y=0., angle=pi / 2),
        (0.09, -0.09)),
    (Line(x=0., y=-0.09, angle=0.), Line(x=0.18, y=0.09, angle=pi / 4),
        (0., -0.09)),
    (Line(x=0., y=-0.09, angle=0.), Line(x=0., y=0.09, angle=3 * pi / 4),
        (0.18, -0.09)),
], ids=[
    '90-degree turn',
    '45-degree turn',
    '135-degree turn',
])
def test_lines_intersection(l0, l1, intersection):
    assert intersection == approx(lines_intersection(l0, l1))


@pytest.mark.parametrize('l0,l1', [
    (Line(x=0., y=-0.09, angle=0.), Line(x=0., y=0.09, angle=pi)),
], ids=[
    '180-degree turn',
])
def test_lines_intersection_parallel(l0, l1):
    with pytest.raises(ValueError):
        lines_intersection(l0, l1)
