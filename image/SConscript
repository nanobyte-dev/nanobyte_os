import os
import re
import time
from decimal import Decimal
from io import SEEK_CUR, SEEK_SET
from pathlib import Path
from shutil import copy2
import parted
import sh

from SCons.Action import Action
from SCons.Builder import Builder
from SCons.Environment import Environment

from build_scripts.utility import FindIndex, GlobRecursive, IsFileName

Import('stage1')
Import('stage2')
Import('kernel')

Import('TARGET_ENVIRONMENT')
TARGET_ENVIRONMENT: Environment
env:Environment = TARGET_ENVIRONMENT

SECTOR_SIZE = 512

def generate_image_file(target: str, size_sectors: int):
    with open(target, 'wb') as fout:
        fout.write(bytes(size_sectors * SECTOR_SIZE))
        fout.close()

def create_filesystem(target: str, filesystem, reserved_sectors=0, offset=0):
    if filesystem in ['fat12', 'fat16', 'fat32']:
        reserved_sectors += 1
        if filesystem == 'fat32':
            reserved_sectors += 1

        mkfs_fat = sh.Command('mkfs.fat')
        mkfs_fat(target,
                 F=filesystem[3:],            # fat size
                 n='NBOS',                    # label
                 R=reserved_sectors,      # reserved sectors
                 offset=offset
        )
    elif filesystem == 'ext2':
        mkfs_ext2 = sh.Command('mkfs.ext2')
        mkfs_ext2(target,
                  L='NBOS',                   # label
                  E=f'offset={offset * SECTOR_SIZE}'
        )
    else:
        raise ValueError('Unsupported filesystem ' + filesystem)


def find_symbol_in_map_file(map_file: str, symbol: str):
    with map_file.open('r') as fmap:
        for line in fmap:
            if symbol in line:
                match = re.search('0x([0-9a-fA-F]+)', line)
                if match is not None:
                    return int(match.group(1), base=16)
    return None


def install_stage1(target: str, stage1: str, stage2_offset, stage2_size, offset=0):
    # find stage1 map file
    map_file = Path(stage1).with_suffix('.map')
    if not map_file.exists():
        raise ValueError("Can't find " + map_file)
    
    entry_offset = find_symbol_in_map_file(map_file, '__entry_start')
    if entry_offset is None:
        raise ValueError("Can't find __entry_start symbol in map file " + str(map_file))
    entry_offset -= 0x7c00

    stage2_start = find_symbol_in_map_file(map_file, 'stage2_location')
    if stage2_start is None:
        raise ValueError("Can't find stage2_location symbol in map file " + str(map_file))
    stage2_start -= 0x7c00

    with open(stage1, 'rb') as fstage1:
        with os.fdopen(os.open(target, os.O_WRONLY | os.O_CREAT), 'wb+') as ftarget:
            ftarget.seek(offset * SECTOR_SIZE, SEEK_SET)

            # write first 3 bytes jump instruction
            ftarget.write(fstage1.read(3))

            # write starting at entry_offset (end of header)
            fstage1.seek(entry_offset - 3, SEEK_CUR)
            ftarget.seek(entry_offset - 3, SEEK_CUR)
            ftarget.write(fstage1.read())

            # write location of stage2
            ftarget.seek(offset * SECTOR_SIZE + stage2_start, SEEK_SET)
            ftarget.write(stage2_offset.to_bytes(4, byteorder='little'))
            ftarget.write(stage2_size.to_bytes(1, byteorder='little'))

def install_stage2(target: str, stage2: str, offset=0, limit=None):
    with open(stage2, 'rb') as fstage2:
        with os.fdopen(os.open(target, os.O_WRONLY | os.O_CREAT), 'wb+') as ftarget:
            ftarget.seek(offset * SECTOR_SIZE, SEEK_SET)
            ftarget.write(fstage2.read())

def build_floppy(image, stage1, stage2, kernel, files, env):
    size_sectors = 2880
    stage2_size = os.stat(stage2).st_size
    stage2_sectors = (stage2_size + SECTOR_SIZE - 1) // SECTOR_SIZE
    
    generate_image_file(image, size_sectors)

    print(f"> formatting file using fat12...")
    create_filesystem(image, 'fat12', stage2_sectors)

    print(f"> installing stage1...")
    install_stage1(image, stage1, stage2_offset=1, stage2_size=stage2_sectors)

    print(f"> installing stage2...")
    install_stage2(image, stage2, offset=1)

    print(f"> copying files...")
    print('    ... copying', kernel)
    sh.mmd('-i', image, "::boot")
    sh.mcopy('-i', image, kernel, "::boot/")

    # copy rest of files
    src_root = env['BASEDIR']
    for file in files:
        file_src = file.srcnode().path
        file_rel = os.path.relpath(file_src, src_root)
        file_dst = '::' + file_rel

        if os.path.isdir(file_src):
            print('    ... creating directory', file_rel)
            sh.mmd('-i', image, file_dst)
        else:
            print('    ... copying', file_rel)
            sh.mcopy('-i', image, file_src, file_dst)


def create_partition_table(target: str, align_start: int):
    device = parted.getDevice(target)
    disk = parted.freshDisk(device, 'msdos')
    freeSpace = disk.getFreeSpaceRegions()
    partitionGeometry = parted.Geometry(device, align_start, end=freeSpace[-1].end)
    partition = parted.Partition(disk=disk, type=parted.PARTITION_NORMAL, geometry=partitionGeometry)
    partition.setFlag(parted.PARTITION_BOOT)
    disk.addPartition(partition, constraint=device.optimalAlignedConstraint)
    disk.commit()


def mount_fs(image: str, mount_dir: str):
    sh.guestmount(mount_dir, add=image, mount='/dev/sda1')

def unmount_fs(mount_dir: str):
    time.sleep(3)
    sh.fusermount(u=mount_dir)


def build_disk(image, stage1, stage2, kernel, files, env):
    size_sectors = (env['imageSize'] + SECTOR_SIZE - 1) // SECTOR_SIZE
    file_system = env['imageFS']
    partition_offset = 2048

    stage1_size = os.stat(stage1).st_size
    stage2_size = os.stat(stage2).st_size
    stage2_sectors = (stage2_size + SECTOR_SIZE - 1) // SECTOR_SIZE

    generate_image_file(image, size_sectors)

    # create partition table
    print(f"> creating partition table...")
    create_partition_table(image, partition_offset)

    # create file system
    print(f"> formatting file using {file_system}...")
    create_filesystem(image, file_system, offset=partition_offset)

    # install stage1
    print(f"> installing stage1...")
    install_stage1(image, stage1, offset=partition_offset, stage2_offset=1, stage2_size=stage2_sectors)
 
    # install stage2
    print(f"> installing stage2...")
    install_stage2(image, stage2, offset=1, limit=partition_offset)

    tempdir_name = 'tmp_mount_{0}'.format(int(time.time()))
    tempdir = os.path.join(os.path.dirname(image), tempdir_name)
    try:
        # mount
        os.mkdir(tempdir)

        print(f"> mounting image to {tempdir}...")
        mount_fs(image, tempdir)

        # copy kernel
        print(f"> copying kernel...")
        bootdir = os.path.join(tempdir, 'boot')
        os.makedirs(bootdir)
        copy2(kernel, bootdir)

        # copy rest of files
        src_root = env['BASEDIR']
        print(f"> copying files...")
        for file in files:
            file_src = file.srcnode().path
            file_rel = os.path.relpath(file_src, src_root)
            file_dst = os.path.join(tempdir, file_rel)

            if os.path.isdir(file_src):
                print('    ... creating directory', file_rel)
                os.makedirs(file_dst)
            else:
                print('    ... copying', file_rel)
                copy2(file_src, file_dst)

    finally:
        print("> cleaning up...")
        try:
            unmount_fs(tempdir)
        except:
            pass
        os.rmdir(tempdir)


def build_image(target, source, env):
    stage1 = str(source[0])
    stage2 = str(source[1])
    kernel = str(source[2])
    files = source[3:]

    image = str(target[0])
    if env['imageType'] == 'floppy':
        build_floppy(image, stage1, stage2, kernel, files, env)
    elif env['imageType'] == 'disk':
        build_disk(image, stage1, stage2, kernel, files, env)
    else:
        raise ValueError('Unknown image type ' + env['imageType'])


# Setup image target
root = env.Dir('root')
root_content = GlobRecursive(env, '*', root)
inputs = [stage1, stage2, kernel] + root_content

output_fmt = 'img'
# if env['imageType'] == 'qcow3':
#     output_fmt = 'qcow3'

output = f'image.{output_fmt}'

image = env.Command(output, inputs,
                    action=Action(build_image, 'Creating disk image...'), 
                    BASEDIR=root.srcnode().path)
env.Depends(image, inputs)

Export('image')