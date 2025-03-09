# basic_os

My work in progress.  First step is getting the keyboard controller fully operational, then I can start working on parsing user input into something interesting.

---

# nanobyte_os
This repository contains the code from the ["Building an OS"](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN) series on the ["Nanobyte"](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA) YouTube channel.

## Building

First, install the following dependencies:

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

I had to run this to get guestmount to work without sudo:
```bash
sudo dpkg-statoverride --update --add root root 0644 /boot/vmlinuz-`uname -r`
```

Then you must run `python3 -m pip install -r requirements.txt`

Next, modify the configuration in `build_scripts/config.py`. The most important is the `toolchain='../.toolchains'` option which sets where the toolchain will be downloaded and built. The default option is in the directory above where the repo is cloned, in a .toolchains directory, but you will get an error if this directory doesn't exist.

After that, run `scons toolchain`, this should download and build the required tools (binutils and GCC). If you encounter errors during this step, you might have to modify `scripts/setup_toolchain.sh` and try a different version of **binutils** and **gcc**. Using the same version as the one bundled with your distribution is your best bet.

Finally, you should be able to run `scons`. Use `scons run` to test your OS using qemu.

## Links

* [Discord channel](https://discord.gg/RgHc5XrCEw)
* [Patreon](https://www.patreon.com/nanobyte)

