# nanobyte_os
This repo contains the code from the ["Building an OS"](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN) tutorial on the ["Nanobyte"](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA) YouTube channel.

This branch contains the code as written in [Part 2](https://youtu.be/srbnMNk7K7k), with some minor bug fixes:

* src/kernel/main.asm: added missing colon on line 57

## Prerequisites

The project requires a Unix-like environment. If you are using Windows, there are various ways of setting one up (WSL, a Linux virtual machine, Cygwin, MSYS2). I recommend using WSL, which is the [easiest to setup](https://learn.microsoft.com/en-us/windows/wsl/install).

For Part 2, you need the following tools:

* `make`
* `nasm`
* `qemu-system-x86` for testing
* `bochs-x bochsbios vgabios` for debugging
* your preferred text editor

## Build instructions

* run `make`

## Running with qemu

* run `./run.sh`

## Debugging with bochs

* run `./debug.sh`
* navigate to *View* - *Disassemble* (or press Ctrl+D) and type `0x7C00` and press OK on both prompts
* double click on the first instruction to set a breakpoint. The line should turn red.
* press *Continue*
* if everything goes right, you will end up at address 0x7C00. If the code looks different than `boot.asm`, run the *Disassemble* command again.
* run step by step to see your code in action. If the code looks like garbage, you probably need to *Disassemble* again.
* when reaching the `hlt` instruction, look at the memory range `0x7E00-0x8000` (*View* - *Physical/Linear Mem Dump*). Open `main_floppy.img` in a hex editor of your choice, navigate to the second sector (offset `0x200` or `512`). The data should be the same.

**Troubleshooting**: Bochs has proven to be pretty unreliable. Check [this article](https://github.com/nanobyte-dev/nanobyte_os/wiki/Frequent-issues#bochs-doesnt-work) for some troubleshooting tips.

## Links

* [YouTube](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA)
* [Discord channel](https://discord.gg/RgHc5XrCEw)
* [Patreon](https://www.patreon.com/nanobyte)
