# Build environment for nanobyte_os: the packages from scripts/deps/debian.txt and the cross compiler.
# Used by build-docker.sh, which builds this image locally the first time it runs.
# The build context is the scripts directory.

FROM debian:trixie

ENV DEBIAN_FRONTEND=noninteractive

# dependencies, installed the same way as on a regular machine
COPY install_deps.sh /tmp/nanobyte/
COPY deps/debian.txt /tmp/nanobyte/deps/
RUN /tmp/nanobyte/install_deps.sh -y \
    && rm -rf /var/lib/apt/lists/*

# cross compiler; only the installed toolchain is kept, not the sources and build directories
COPY setup_toolchain.sh /tmp/nanobyte/
RUN /tmp/nanobyte/setup_toolchain.sh /opt/toolchains \
    && cd /opt/toolchains \
    && rm -rf binutils-* gcc-*
