from io import StringIO

import numpy
import pytest

from mazes import load_maze
from mazes import read_walls

from mazes import EAST_DIRECTION
from mazes import SOUTH_DIRECTION
from mazes import WEST_DIRECTION
from mazes import NORTH_DIRECTION


MAZE_OSHWDEM_00 = """OSHWDEM Maze Generator v1.2 R42263
+---+---+---+---+---+
| A |               |
+   +   +---+---+   +
|   |   |       |   |
+   +   +   B   +   +
|       |       |   |
+   +   +---+   +---+
|   |       |       |
+   +   +---+---+   +
|   |               |
+---+---+---+---+---+
"""


def test_load_maze_oshwdem():
    result = load_maze(StringIO(MAZE_OSHWDEM_00))
    assert (result == numpy.array([
        [28, 12, 10, 10, 24],
        [20, 20, 12, 24, 20],
        [4, 16,  6, 16, 22],
        [20,  4, 26,  6, 24],
        [22,  6, 10, 10, 18],
    ])).all()


@pytest.mark.parametrize('x,y,direction,walls', [
    (0, 0, EAST_DIRECTION, (True, False, True)),
    (1, 0, EAST_DIRECTION, (True, False, True)),
    (2, 0, EAST_DIRECTION, (False, False, True)),
    (0, 0, NORTH_DIRECTION, (True, True, False)),
    (0, 1, NORTH_DIRECTION, (True, False, False)),
    (3, 2, NORTH_DIRECTION, (True, True, True)),
    (3, 3, SOUTH_DIRECTION, (True, True, False)),
    (4, 4, WEST_DIRECTION, (False, False, True)),
])
def test_read_walls(x, y, direction, walls):
    maze = load_maze(StringIO(MAZE_OSHWDEM_00))
    assert read_walls(maze, x, y, direction) == walls
