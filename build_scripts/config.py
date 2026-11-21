#config = 'release'
#arch = 'i686'
imageType = 'disk'
imageFS = 'fat32'
imageSize = '250m'
toolchain = '../.toolchains'

# How files are copied into the disk image:
# mtools - writes the file system directly with mtools, no mounting (FAT only)
# guestfs - mounts the image with libguestfs (guestmount), doesn't require admin rights
# mount - mounts the image with losetup + mount, requires sudo
mountMethod = 'mtools'