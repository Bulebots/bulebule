"""
Common functions for testing C modules.
"""
from pathlib import Path
import sys
from tempfile import TemporaryDirectory

from cffi import FFI


def clean_header(path):
    """
    Clean C header file to make it CFFI-friendly.
    """
    text = path.read_text()
    lines = text.splitlines()
    return '\n'.join(l for l in lines if not l.startswith('#'))


def stringify_enums(enums, ffi, cast):
    """
    Convert an array of enumerated movements into an array of readable strings.
    """
    return [ffi.string(ffi.cast(cast, x)) for x in enums]


def yield_cffi(name):
    """
    Yield a C Foreign Function Interface to test with Python.
    """
    with TemporaryDirectory() as tmpdir:
        name = Path(name).resolve()
        builder = FFI()
        builder.cdef(clean_header(name.with_suffix('.h')))
        builder.set_source(
            'ffimodule',
            '#include "%s"' % name.with_suffix('.h'),
            sources=[name.with_suffix('.c')])
        builder.compile(tmpdir=tmpdir)
        sys.path.insert(0, tmpdir)
        from ffimodule import ffi
        from ffimodule import lib
        yield ffi, lib
