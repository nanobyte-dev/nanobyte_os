# nanobyte_os
This repo contains the code from the ["Building an OS"](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN) tutorial on the ["Nanobyte"](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA) YouTube channel.

This branch contains the code as written in [Part 7 - Finishing the bootloader, migrating to C](https://youtu.be/qr8d7bZLPHg), with some minor bug fixes:

* fixed bochs configuration
* fixed toolchain building script

## Prerequisites

The project requires a Unix-like environment. If you are using Windows, there are various ways of setting one up (WSL, a Linux virtual machine, Cygwin, MSYS2). I recommend using WSL, which is the [easiest to setup](https://learn.microsoft.com/en-us/windows/wsl/install).

For Part 7, you need the following tools:

* `make`
* `nasm`
* `mtools`
* dependencies to build a GCC cross compiler (see the section below)
* `qemu-system-x86` for testing
* `bochs-x bochsbios vgabios` for debugging
* your preferred text editor

## Building

1. Install the following dependencies:

```
# Ubuntu, Debian:
$ sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo \
                   nasm mtools qemu-system-x86
           
# Fedora:
$ sudo dnf install gcc gcc-c++ make bison flex gmp-devel libmpc-devel mpfr-devel texinfo \
                   nasm mtools qemu-system-x86
```

2. Run `make toolchain`, this should download and build the required tools (binutils and GCC). If you encounter errors during this step, you might have to modify `build_scripts/config.mk` and try a different version of **binutils** and **gcc**. Using the same versions as in your distribution's repositories is your best bet. Run `make clean-toolchain-all` to delete all toolchain related files and start over. 

3. Run `make`.

## Running with qemu

* run `./run.sh`

## Debugging with bochs

* run `./debug.sh`

**Troubleshooting**: Bochs has proven to be pretty unreliable. Check [this article](https://github.com/nanobyte-dev/nanobyte_os/wiki/Frequent-issues#bochs-doesnt-work) for some troubleshooting tips.

## Links

* [YouTube](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA)
* [Discord channel](https://discord.gg/RgHc5XrCEw)
* [Patreon](https://www.patreon.com/nanobyte)
