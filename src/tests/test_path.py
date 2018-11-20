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


@pytest.mark.parametrize('language', ['PATH_SAFE', 'PATH_DIAGONALS'])
@pytest.mark.parametrize('sharp,smooth', [
    ('F', ['FRONT']),
    ('FF', ['FRONT', 'FRONT']),
    ('F' * 12, ['FRONT'] * 12),
    ('FS', ['FRONT', 'STOP']),
    ('FLF', ['FRONT', 'LEFT_90', 'FRONT']),
    ('FRF', ['FRONT', 'RIGHT_90', 'FRONT']),
    ('FLFS', ['FRONT', 'LEFT_90', 'FRONT', 'STOP']),
    ('FRFS', ['FRONT', 'RIGHT_90', 'FRONT', 'STOP']),
    ('FLLF', ['FRONT', 'LEFT_180', 'FRONT']),
    ('FRRF', ['FRONT', 'RIGHT_180', 'FRONT']),
    ('FLLFS', ['FRONT', 'LEFT_180', 'FRONT', 'STOP']),
    ('FRRFS', ['FRONT', 'RIGHT_180', 'FRONT', 'STOP']),
    ('BS', ['START', 'STOP']),
    ('BFS', ['START', 'FRONT', 'STOP']),
    ('BFFS', ['START', 'FRONT', 'FRONT', 'STOP']),
    ('BFLF', ['START', 'FRONT', 'LEFT_90', 'FRONT']),
    ('BFRF', ['START', 'FRONT', 'RIGHT_90', 'FRONT']),
    ('BFLLF', ['START', 'FRONT', 'LEFT_180', 'FRONT']),
    ('BFRRF', ['START', 'FRONT', 'RIGHT_180', 'FRONT']),
    ('BFRFLLFS',
     ['START', 'FRONT', 'RIGHT_90', 'FRONT', 'LEFT_180', 'FRONT', 'STOP']),
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
    'Start and stop',
    'Start, front once and stop',
    'Start, front twice and stop',
    'Start-to-straight 90-degrees left turn',
    'Start-to-straight 90-degrees right turn',
    'Start-to-straight 180-degrees left turn',
    'Start-to-straight 180-degrees right turn',
    'Start-to-straight 90-degrees right turn, then 180-degrees left and stop',
])
def test_path_smoother_all(interface, sharp, smooth, language):
    """
    Test correct path smoothing for all path languages.
    """
    ffi, lib = interface
    result = ffi.new('enum movement destination[30]')
    assert len(smooth) + 1 <= len(result)

    language = getattr(lib, language)
    lib.make_smooth_path(sharp.encode('ascii'), result, language)
    result = stringify_enums(result, ffi, 'enum movement')
    result = [x[5:] for x in result]
    assert result[:len(smooth)] == smooth
    assert result[len(smooth)] == 'END'


@pytest.mark.parametrize('sharp,smooth', [
    ('FLRF', ['FRONT', 'LEFT', 'RIGHT', 'FRONT']),
    ('FRLF', ['FRONT', 'RIGHT', 'LEFT', 'FRONT']),
    ('FLRFS', ['FRONT', 'LEFT', 'RIGHT', 'FRONT', 'STOP']),
    ('FRLFS', ['FRONT', 'RIGHT', 'LEFT', 'FRONT', 'STOP']),
    ('FRLLF', ['FRONT', 'RIGHT', 'LEFT', 'LEFT', 'FRONT']),
], ids=[
    'Left-right zig-zag',
    'Right-left zig-zag',
    'Left-right zig-zag with stop',
    'Right-left zig-zag with stop',
    'Once we start zig-zag, we keep 90-degrees turns',
])
def test_path_smoother_safe(interface, sharp, smooth):
    """
    Test correct path smoothing with the safe language.
    """
    ffi, lib = interface
    result = ffi.new('enum movement destination[30]')
    assert len(smooth) + 1 <= len(result)

    lib.make_smooth_path(sharp.encode('ascii'), result, lib.PATH_SAFE)
    result = stringify_enums(result, ffi, 'enum movement')
    result = [x[5:] for x in result]
    assert result[:len(smooth)] == smooth
    assert result[len(smooth)] == 'END'


@pytest.mark.parametrize('sharp,smooth', [
    ('FLRF', ['FRONT', 'LEFT_TO_45', 'RIGHT_FROM_45', 'FRONT']),
    ('FRLF', ['FRONT', 'RIGHT_TO_45', 'LEFT_FROM_45', 'FRONT']),
    ('FLRLF', ['FRONT', 'LEFT_TO_45', 'DIAGONAL', 'LEFT_FROM_45', 'FRONT']),
    ('FRLRF', ['FRONT', 'RIGHT_TO_45', 'DIAGONAL', 'RIGHT_FROM_45', 'FRONT']),
    ('FLLRRF', ['FRONT', 'LEFT_TO_135', 'RIGHT_FROM_135', 'FRONT']),
    ('FRRLLF', ['FRONT', 'RIGHT_TO_135', 'LEFT_FROM_135', 'FRONT']),
    ('FLLRLLF',
     ['FRONT', 'LEFT_TO_135', 'DIAGONAL', 'LEFT_FROM_135', 'FRONT']),
    ('FRRLRRF',
     ['FRONT', 'RIGHT_TO_135', 'DIAGONAL', 'RIGHT_FROM_135', 'FRONT']),
    ('FLRRF', ['FRONT', 'LEFT_TO_45', 'RIGHT_FROM_135', 'FRONT']),
    ('FRLLF', ['FRONT', 'RIGHT_TO_45', 'LEFT_FROM_135', 'FRONT']),
    ('FLRLLF', ['FRONT', 'LEFT_TO_45', 'DIAGONAL', 'LEFT_FROM_135', 'FRONT']),
    ('FRLRRF',
     ['FRONT', 'RIGHT_TO_45', 'DIAGONAL', 'RIGHT_FROM_135', 'FRONT']),
    ('FLLRF', ['FRONT', 'LEFT_TO_135', 'RIGHT_FROM_45', 'FRONT']),
    ('FRRLF', ['FRONT', 'RIGHT_TO_135', 'LEFT_FROM_45', 'FRONT']),
    ('FLLRLF', ['FRONT', 'LEFT_TO_135', 'DIAGONAL', 'LEFT_FROM_45', 'FRONT']),
    ('FRRLRF',
     ['FRONT', 'RIGHT_TO_135', 'DIAGONAL', 'RIGHT_FROM_45', 'FRONT']),
    ('FRLLRF',
     ['FRONT', 'RIGHT_TO_45', 'LEFT_DIAGONAL', 'RIGHT_FROM_45', 'FRONT']),
    ('FLRRLF',
     ['FRONT', 'LEFT_TO_45', 'RIGHT_DIAGONAL', 'LEFT_FROM_45', 'FRONT']),
    ('FFRLRLRRFRLLRRFRFFLRRLRRFRLRLFF',
     ['FRONT', 'FRONT', 'RIGHT_TO_45', 'DIAGONAL', 'DIAGONAL', 'DIAGONAL',
      'RIGHT_FROM_135', 'FRONT', 'RIGHT_TO_45', 'LEFT_DIAGONAL',
      'RIGHT_FROM_135', 'FRONT', 'RIGHT_90', 'FRONT', 'FRONT', 'LEFT_TO_45',
      'RIGHT_DIAGONAL', 'DIAGONAL', 'RIGHT_FROM_135', 'FRONT', 'RIGHT_TO_45',
      'DIAGONAL', 'DIAGONAL', 'LEFT_FROM_45', 'FRONT', 'FRONT']),
], ids=[
    '45-degrees left in and right out',
    '45-degrees right in and left out',
    '45-degrees left in and left out (one diagonal)',
    '45-degrees right in and left out (one diagonal)',
    '135-degrees left in and right out',
    '135-degrees right in and left out',
    '135-degrees left in and left out (one diagonal)',
    '135-degrees right in and right out (one diagonal)',
    '45-degrees left in and 135-degrees right out',
    '45-degrees right in and 135-degrees left out',
    '45-degrees left in and 135-degrees right out (one diagonal)',
    '45-degrees right in and 135-degrees left out (one diagonal)',
    '135-degrees left in and 45-degrees right out',
    '135-degrees right in and 45-degrees left out',
    '135-degrees left in and 45-degrees right out (one diagonal)',
    '135-degrees right in and 45-degrees left out (one diagonal)',
    'Left V-turn',
    'Right V-turn',
    'Challenge 0',
])
def test_path_smoother_diagonals(interface, sharp, smooth):
    """
    Test correct path smoothing with the diagonals language.
    """
    ffi, lib = interface
    result = ffi.new('enum movement destination[30]')
    assert len(smooth) + 1 <= len(result)

    lib.make_smooth_path(sharp.encode('ascii'), result, lib.PATH_DIAGONALS)
    result = stringify_enums(result, ffi, 'enum movement')
    result = [x[5:] for x in result]
    assert result[:len(smooth)] == smooth
    assert result[len(smooth)] == 'END'
