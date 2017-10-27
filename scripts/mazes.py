from pathlib import Path
from typing import IO

import numpy


EAST = 0
SOUTH = 1
WEST = 2
NORTH = 3

VISITED_BIT = 1
EAST_BIT = 2
SOUTH_BIT = 4
WEST_BIT = 8
NORTH_BIT = 16


def _read_maze_oshwdem(txt: str) -> numpy.ndarray:
    txt = '\n'.join(txt.splitlines()[1:])
    txt = [''.join(i) for i in zip(*txt.splitlines())]

    south = txt[::4][:-1]
    south = [[1 if wall == '|' else 0
              for wall in row.replace('B', '+').strip('+').split('+')]
             for row in south]
    south = numpy.array(south).astype('uint8').T
    north = numpy.roll(south, -1, axis=1)

    west = txt[1::4]
    west = [[1 if wall == '-' else 0
             for wall in column[::2][:-1]]
            for column in west]
    west = numpy.array(west).astype('uint8').T
    east = numpy.roll(west, -1, axis=0)

    east *= EAST_BIT
    south *= SOUTH_BIT
    west *= WEST_BIT
    north *= NORTH_BIT
    return east + south + west + north


def load_maze(data: IO) -> numpy.ndarray:
    if isinstance(data, Path):
        maze = data.read_text()
    else:
        maze = data.read()
    if maze.startswith('OSHWDEM'):
        return _read_maze_oshwdem(maze)
    raise NotImplementedError()
