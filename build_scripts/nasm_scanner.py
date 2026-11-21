import re

import SCons.Tool
from SCons.Scanner import Scanner

# SCons scans C files for #include by itself, but has no scanner for NASM's %include.
# Without this one, editing an .inc file doesn't reassemble the files that include it.

INCLUDE_RE = re.compile(r'^\s*%include\s+["<]([^">]+)[">]', re.MULTILINE | re.IGNORECASE)


def _include_dirs(env, dir, target=None, source=None, argument=None):
    """NASM's include directories are the -I options in ASFLAGS."""
    flags = env.Flatten(env.get('ASFLAGS', []))
    dirs = []
    for flag, value in zip(flags, flags[1:]):
        if str(flag) == '-I':
            dirs.append(value if not isinstance(value, str) else env.Dir('#' + value))
    return tuple(dirs)


def _scan(node, env, path):
    found = []
    for name in INCLUDE_RE.findall(node.get_text_contents()):
        # like NASM, look next to the including file first, then in the -I directories
        file = env.FindFile(name, (node.srcnode().dir,) + path)
        if file is not None:
            found.append(file)
    return found


NasmScanner = Scanner(function=_scan, skeys=['.asm', '.inc'], path_function=_include_dirs,
                      recursive=True, name='NasmScanner')


def RegisterNasmScanner():
    # Object() picks its source scanner by file suffix from this table, not from env['SCANNERS']
    for suffix in ('.asm', '.inc'):
        SCons.Tool.SourceFileScanner.add_scanner(suffix, NasmScanner)
