# nanobyte_os
This repository contains the code from the ["Building an OS"](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN) series on the ["Nanobyte"](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA) YouTube channel.


This branch contains the code as written in [Part 11 - Memory detection](https://www.youtube.com/watch?v=xp-yB9WBadI), with some minor bug fixes:

* fixed bochs configuration
* fixed `missing ../.toolchains directory` error when building toolchain
* fixed error related to PREFIX when building toolchain
* added support for "mount" method (in addition to libguestfs)

## Prerequisites

The project requires a Unix-like environment. If you are using Windows, there are various ways of setting one up (WSL, a Linux virtual machine, Cygwin, MSYS2). I recommend using WSL, which is the [easiest to setup](https://learn.microsoft.com/en-us/windows/wsl/install).

For Part 11, you need the following tools:

* `scons`
* `nasm`
* `mtools`
* dependencies to build a GCC cross compiler (see the section below)
* `qemu-system-x86` for testing
* `bochs-x bochsbios vgabios` for debugging
* your preferred text editor

A major change in Part 11 was the move from `make` to `scons`. Most of the build scripts are now written in Python. Check the "Building" section if you are coming from Part 10, there are some new dependencies.

## Building

1. Install dependencies:

```sh
# Ubuntu, Debian:
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo wget \
                   nasm mtools python3 python3-pip python3-parted scons dosfstools libguestfs-tools qemu-system-x86

# Fedora:
sudo dnf install gcc gcc-c++ make bison flex gmp-devel libmpc-devel mpfr-devel texinfo wget \
                   nasm mtools python3 python3-pip python3-pyparted python3-scons dosfstools guestfs-tools qemu-system-x86

# Arch & Arch-based:
paru -S gcc make bison flex libgmp-static libmpc mpfr texinfo nasm mtools qemu-system-x86 python3 scons
```
NOTE: to install all the required packages on Arch, you need an [AUR helper](https://wiki.archlinux.org/title/AUR_helpers).

2. Install python packages: `python3 -m pip install -r requirements.txt`

3. Check the `build_scripts/config.py` configuration file. The `toolchain` variable specifies where the toolchain will be downloaded and installed. The default path is `../.toolchains`, which is in the parent directory to where you cloned the repo.

Coming from Part 10, you might want to reuse the old toolchain you have already built. Simply change the path `toolchain` variable to point to the correct directory.

Another important variable is `mountMethod`. See the section below for details.

4. Run `scons toolchain`, this should download and build the required tools (binutils and GCC). If you encounter errors during this step, you might have to modify `build_scripts/config.mk` and try a different version of **binutils** and **gcc**. Using the same version as the one bundled with your distribution is your best bet.

5. Run `scons`. Type your sudo password when prompted, this is required for running `losetup`, `mount`, `umount`.

### Mount methods

There are 2 supported mount methods that can be configured in `build_scripts/config.py` with the `mountMethod` variable:

* `guestfs` uses the `guestmount` command, and it doesn't require sudo access. This is the preferred method, however in some distributions (most notably WSL) it isn't working properly.

* `mount` is provided as a backup when `guestfs` fails. The disadvantage is that it requires typing your password every time you rebuild the disk image.

### Warning

** DO NOT use `sudo` to run the build**. This will mess up file permissions for all the files in the `build` directory, and has the potential of damaging your system. The build script will request elevated privileges only when performing the mount operations.

## Running

* run `scons run` to test your OS using qemu.

## Debugging with qemu and GDB

* run `scons debug`.

## Debugging with bochs

* run `scons bochs`.

**Troubleshooting**: Bochs has proven to be pretty unreliable. Check [this article](https://github.com/nanobyte-dev/nanobyte_os/wiki/Frequent-issues#bochs-doesnt-work) for some troubleshooting tips.


## Links

* [YouTube](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA)
* [Discord channel](https://discord.gg/RgHc5XrCEw)
* [Patreon](https://www.patreon.com/nanobyte)
