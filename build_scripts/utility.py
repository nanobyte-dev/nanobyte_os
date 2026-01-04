from decimal import Decimal
import re
import os

from SCons.Node.FS import Dir, File, Entry
from SCons.Environment import Environment


def ParseSize(size: str):
    size_match = re.match(r'([0-9\.]+)([kmg]?)', size, re.IGNORECASE)
    if size_match is None:
        raise ValueError(f'Error: Invalid size {size}')

    result = Decimal(size_match.group(1))
    multiplier = size_match.group(2).lower()

    if multiplier == 'k':
        result *= 1024
    if multiplier == 'm':
        result *= 1024 ** 2
    if multiplier == 'g':
        result *= 1024 ** 3

    return int(result)


def GlobRecursive(env: Environment, pattern, node='.'):
    src = str(env.Dir(node).srcnode())
    cwd = str(env.Dir('.').srcnode())

    dir_list = [src]
    for root, directories, _ in os.walk(src):
        for d in directories:
            dir_list.append(os.path.join(root, d))

    globs = []
    for d in dir_list:
        glob = env.Glob(os.path.join(os.path.relpath(d, cwd), pattern))
        globs.append(glob)

    return globs


def FindIndex(the_list, predicate):
    for i in range(len(the_list)):
        if predicate(the_list[i]):
            return i
    return None


def IsFileName(obj, name):
    if isinstance(obj, str):
        return name in obj
    elif isinstance(obj, File) or isinstance(obj, Dir) or isinstance(obj, Entry):
        return obj.name == name
    return False


def RemoveSuffix(str, suffix):
    if str.endswith(suffix):
        return str[:-len(suffix)]
    return str