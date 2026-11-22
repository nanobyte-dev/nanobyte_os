# nanobyte_os
This repository contains the code from the ["Building an OS"](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN) series on the ["Nanobyte"](https://www.youtube.com/channel/UCSPIuWADJIMIf9Erf--XAsA) YouTube channel.


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

1. Install dependencies by running `scripts/install_deps.sh`. It detects your distro (Debian/Ubuntu, Fedora, Arch, openSUSE) and installs the packages listed in `scripts/deps/<distro>.txt`. To install them yourself instead, use that list with your package manager, for example:

```sh
sudo apt install $(sed 's/#.*//' scripts/deps/debian.txt)
```

2. The Python package `sh` is in the lists above. If you use a virtual environment or a distro without them, install them with `python3 -m pip install -r requirements.txt`.

3. Check the `build_scripts/config.py` configuration file. The `toolchain` variable specifies where the toolchain will be downloaded and installed. The default path is `../.toolchains`, which is in the parent directory to where you cloned the repo.

Coming from Part 10, you might want to reuse the old toolchain you have already built. Simply change the path `toolchain` variable to point to the correct directory.

Another important variable is `mountMethod`. See the section below for details.

4. Run `scons toolchain`, this should download and build the required tools (binutils and GCC). If you encounter errors during this step, you might have to modify `scripts/setup_toolchain.sh` and try a different version of **binutils** and **gcc**. Using the same version as the one bundled with your distribution is your best bet.

5. Run `scons`. If you are using the `mount` method, type your sudo password when prompted, this is required for running `losetup`, `mount`, `umount`.

### Building with docker

If you'd rather not install everything on your machine, `./build-docker.sh` runs the build in a container, so you only need `docker` or `podman`. Any arguments are passed to `scons`, for example `./build-docker.sh imageType=floppy`.

The first run builds a local `nanobyte-build` image with the dependencies and the cross compiler, which takes a while. Later runs reuse it. When the files it's built from change (`scripts/docker/Dockerfile`, `scripts/install_deps.sh`, `scripts/deps/debian.txt`, `scripts/setup_toolchain.sh`), the script builds a new image and lists the old ones, which you can delete.

The container always uses the `mtools` mount method. To run the OS, use `scripts/run.sh` on your machine (it needs `qemu-system-x86`), for example `scripts/run.sh disk build/i686_debug/image.img`. `scons run` would rebuild everything with your local toolchain.

### Mount methods

The `mountMethod` variable in `build_scripts/config.py` sets how files are copied into the disk image:

* `mtools` (default) writes the file system directly with `mtools`, without mounting anything. No sudo, works in WSL and containers. FAT only.

* `guestfs` mounts the image with `guestmount` (libguestfs), without sudo. Use it if your distro has no `mtools`; install libguestfs yourself (`libguestfs-tools` on Debian/Ubuntu, `guestfs-tools` on Fedora and openSUSE, `libguestfs` on Arch). It doesn't work properly on some systems, most notably WSL.

* `mount` uses `losetup` and `mount`. The disadvantage is that it requires typing your password every time you rebuild the disk image.

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
