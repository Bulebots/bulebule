from io import StringIO

import numpy

from mazes import load_maze


def test_maze_oshwdem():
    maze = """OSHWDEM Maze Generator v1.2 R42263
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
    result = load_maze(StringIO(maze))
    assert (result == numpy.array([
        [28, 12, 10, 10, 24],
        [20, 20, 12, 24, 20],
        [4, 16,  6, 16, 22],
        [20,  4, 26,  6, 24],
        [22,  6, 10, 10, 18],
    ])).all()
