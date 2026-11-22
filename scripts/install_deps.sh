#!/usr/bin/env bash
# Installs the packages needed to build and run nanobyte_os.
# The package lists are in scripts/deps/<distro>.txt.
#
# Usage: install_deps.sh [-y]
#   -y   don't ask for confirmation (for scripts and containers)

set -e

DEPS_DIR="$(dirname "$(readlink -f "$0")")/deps"

ASSUME_YES=
if [ "$1" = "-y" ]; then
    ASSUME_YES=1
fi

# Detect distro
PACKAGE_UPDATE=
if [ -x "$(command -v apk)" ]; then
    echo "Alpine not supported."
    exit 1
elif [ -x "$(command -v apt-get)" ]; then
    DISTRO='debian'
    PACKAGE_UPDATE='apt-get update'
    PACKAGE_INSTALL="apt-get install ${ASSUME_YES:+-y}"
elif [ -x "$(command -v dnf)" ]; then
    DISTRO='fedora'
    PACKAGE_INSTALL="dnf install ${ASSUME_YES:+-y}"
elif [ -x "$(command -v yum)" ]; then
    DISTRO='fedora'
    PACKAGE_INSTALL="yum install ${ASSUME_YES:+-y}"
elif [ -x "$(command -v zypper)" ]; then
    DISTRO='opensuse'
    PACKAGE_INSTALL="zypper ${ASSUME_YES:+--non-interactive} install"
elif [ -x "$(command -v pacman)" ]; then
    DISTRO='arch'
    # -Syu: installing without upgrading is a partial upgrade, which Arch doesn't support
    PACKAGE_INSTALL="pacman -Syu --needed ${ASSUME_YES:+--noconfirm}"
else
    echo "Unknown operating system!"
    exit 1
fi

# Read the package list, without comments and empty lines
DEPS=($(sed -e 's/#.*//' "$DEPS_DIR/$DISTRO.txt"))

# Package managers need root
SUDO=
if [ "$(id -u)" -ne 0 ]; then
    SUDO='sudo'
fi

echo ""
echo "Will install dependencies by running the following command."
echo ""
echo " \$ $SUDO $PACKAGE_INSTALL ${DEPS[*]}"
echo ""

if [ -z "$ASSUME_YES" ]; then
    read -p "Continue (y/n)?" choice
    case "$choice" in
      y|Y ) ;;
      * ) echo "Exiting..."
          exit 0
          ;;
    esac
fi

if [ -n "$PACKAGE_UPDATE" ]; then
    $SUDO $PACKAGE_UPDATE
fi
$SUDO $PACKAGE_INSTALL "${DEPS[@]}"
