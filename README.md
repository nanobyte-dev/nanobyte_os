# nanobyte_os
This repo contains the code from the ["Building an OS"](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN) tutorial on the ["Nanobyte"](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA) YouTube channel.

This branch contains the code as written in [Part 1](https://youtu.be/9t-SPC7Tczc), with some minor bug fixes:

* main.asm: added missing colon on line 57
* Makefile: convert spaces to tabs to fix "missing separator" error
* Makefile: create build directory automatically to fix "unable to open output file" error

## Prerequisites

The project requires a Unix-like environment. If you are using Windows, there are various ways of setting one up (WSL, a Linux virtual machine, Cygwin, MSYS2). I recommend using WSL, which is the [easiest to setup](https://learn.microsoft.com/en-us/windows/wsl/install).

For Part 1, you need the following tools:

* make
* nasm
* qemu-system-x86 for testing
* your preferred text editor

## Build instructions

* run `make`

## Running with qemu

* run `qemu-system-i386 -fda build/main_floppy.img`

## Links

* [YouTube](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA)
* [Discord channel](https://discord.gg/RgHc5XrCEw)
* [Patreon](https://www.patreon.com/nanobyte)
