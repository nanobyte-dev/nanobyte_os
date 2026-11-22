#!/usr/bin/env bash
# Builds nanobyte_os inside a container, so you only need docker (or podman) installed.
# All arguments are passed to scons, for example:
#
#   ./build-docker.sh
#   ./build-docker.sh imageType=floppy
#
# The first run builds a local image with all the dependencies and the cross compiler, which takes
# a while. The image is tagged with a hash of the files it's built from, so when one of them
# changes, the image is built again. Set DOCKER=podman to use podman when both are installed.

set -e
cd "$(dirname "$(readlink -f "$0")")"

DOCKER=${DOCKER:-$(command -v docker || command -v podman || true)}
if [ -z "$DOCKER" ]; then
    echo "docker or podman is required"
    exit 1
fi

# the files the image is built from
INPUTS=(scripts/docker/Dockerfile scripts/install_deps.sh scripts/deps/debian.txt scripts/setup_toolchain.sh)
if command -v sha256sum >/dev/null; then
    SHA256=(sha256sum)
else
    SHA256=(shasum -a 256)          # macOS
fi
HASH=$(cat "${INPUTS[@]}" | "${SHA256[@]}" | cut -c1-12)
IMAGE=nanobyte-build:$HASH

if ! "$DOCKER" image inspect "$IMAGE" >/dev/null 2>&1; then
    OLD_IMAGES=$("$DOCKER" image ls --format '{{.Repository}}:{{.Tag}}' | grep '^\(localhost/\)\?nanobyte-build:' || true)
    if [ -n "$OLD_IMAGES" ]; then
        echo "The build environment changed (${INPUTS[*]}), building $IMAGE..."
    else
        echo "Building the build environment image $IMAGE. This includes building the cross compiler, so it takes a while..."
    fi

    "$DOCKER" build -t "$IMAGE" -f scripts/docker/Dockerfile scripts

    if [ -n "$OLD_IMAGES" ]; then
        echo
        echo "Older build environment images, which you can delete with '$(basename "$DOCKER") image rm <image>':"
        echo "$OLD_IMAGES"
        echo
    fi
fi

# Run as the current user, so the files in build/ belong to you and not to root. Rootless podman
# maps root in the container to your user already, and needs SELinux labeling off to use the repo.
if [ "$(basename "$DOCKER")" = "podman" ]; then
    RUN_OPTS=(--userns=keep-id --security-opt label=disable)
else
    RUN_OPTS=(--user "$(id -u):$(id -g)")
fi

# interactive terminal only if we have one (not in CI, or when the output is piped)
if [ -t 0 ] && [ -t 1 ]; then
    RUN_OPTS+=(-it)
fi

# Mount the repo at the same path as on the host, so paths in error messages match.
# The disk image can only be built with mtools here, mounting doesn't work in a container.
exec "$DOCKER" run --rm "${RUN_OPTS[@]}" \
    -e HOME=/tmp \
    -v "$PWD:$PWD" -w "$PWD" \
    "$IMAGE" \
    scons toolchain=/opt/toolchains mountMethod=mtools "$@"
