#config = 'release'
#arch = 'i686'
imageType = 'disk'
imageFS = 'fat32'
imageSize = '250m'
toolchain = '../.toolchains'

# Mount methods:
# guestfs - uses libguestfs, doesn't require admin rights but may not work on some systems
# mount - uses mount, requires sudo
mountMethod = 'mount'