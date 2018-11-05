"""
Test the path module.
"""
import pytest

from common import stringify_enums
from common import yield_cffi


@pytest.fixture(scope='module')
def interface():
    """
    Compile the `path.c` module and return the FFI and the smoother functions.
    """
    yield from yield_cffi('./src/path')


@pytest.mark.parametrize('sharp,smooth', [
    ('F', ['FRONT']),
    ('FF', ['FRONT', 'FRONT']),
    ('F' * 12, ['FRONT'] * 12),
    ('FS', ['FRONT', 'STOP']),
    ('FLF', ['FRONT', 'LEFT_90', 'FRONT']),
    ('FRF', ['FRONT', 'RIGHT_90', 'FRONT']),
    ('FLS', ['FRONT', 'LEFT', 'STOP']),
    ('FRS', ['FRONT', 'RIGHT', 'STOP']),
    ('FLLF', ['FRONT', 'LEFT_180', 'FRONT']),
    ('FRRF', ['FRONT', 'RIGHT_180', 'FRONT']),
    ('FLLS', ['FRONT', 'LEFT', 'LEFT', 'STOP']),
    ('FRRS', ['FRONT', 'RIGHT', 'RIGHT', 'STOP']),
    ('FLRF', ['FRONT', 'LEFT', 'RIGHT', 'FRONT']),
    ('FRLF', ['FRONT', 'RIGHT', 'LEFT', 'FRONT']),
    ('FLRS', ['FRONT', 'LEFT', 'RIGHT', 'STOP']),
    ('FRLS', ['FRONT', 'RIGHT', 'LEFT', 'STOP']),
    ('FRLLF', ['FRONT', 'RIGHT', 'LEFT', 'LEFT', 'FRONT']),
    ('BS', ['START', 'STOP']),
    ('BFS', ['START', 'FRONT', 'STOP']),
    ('BFFS', ['START', 'FRONT', 'FRONT', 'STOP']),
    ('BLF', ['START', 'LEFT', 'FRONT']),
    ('BRF', ['START', 'RIGHT', 'FRONT']),
    ('BLLF', ['START', 'LEFT', 'LEFT', 'FRONT']),
    ('BRRF', ['START', 'RIGHT', 'RIGHT', 'FRONT']),
    ('BRFLLFS', ['START', 'RIGHT', 'FRONT', 'LEFT_180', 'FRONT', 'STOP']),
], ids=[
    'Move front once',
    'Move front twice',
    'Move front many',
    'Move front once and stop',
    'Straight-to-straight 90-degrees left turn',
    'Straight-to-straight 90-degrees right turn',
    'Straight-to-stop 90-degrees left turn',
    'Straight-to-stop 90-degrees right turn',
    'Straight-to-straight 180-degrees left turn',
    'Straight-to-straight 180-degrees right turn',
    'Straight-to-stop 180-degrees left turn',
    'Straight-to-stop 180-degrees right turn',
    'Left-right zig-zag',
    'Right-left zig-zag',
    'Left-right zig-zag with stop',
    'Right-left zig-zag with stop',
    'Once we start zig-zag, we keep 90-degrees turns',
    'Start and stop',
    'Start, front once and stop',
    'Start, front twice and stop',
    'Start-to-straight 90-degrees left turn',
    'Start-to-straight 90-degrees right turn',
    'Start-to-straight 180-degrees left turn',
    'Start-to-straight 180-degrees right turn',
    'Start-to-straight 90-degrees right turn, then 180-degrees left and stop',
])
def test_path_smoother_no_diagonals(interface, sharp, smooth):
    """
    Test correct path smoothing when diagonals are disallowed.
    """
    ffi, lib = interface
    result = ffi.new('enum movement destination[20]')
    assert len(smooth) + 1 <= len(result)

    lib.make_smooth_path(sharp.encode('ascii'), result)
    result = stringify_enums(result, ffi, 'enum movement')
    result = [x[5:] for x in result]
    assert result[:len(smooth)] == smooth
    assert result[len(smooth)] == 'END'
