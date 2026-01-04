DEPS_ALPINE=(
    build-base
    gmp-dev
    mpc
    mpfr-dev
    bison
    flex
    texinfo
    nasm
    mtools
    qemu-system-x86_64
    python3
    py3-pip
    scons
    py3-parted
    py3-sh
)

DEPS_DEBIAN=(
    build-essential
    bison 
    flex 
    libgmp3-dev 
    libmpc-dev 
    libmpfr-dev 
    texinfo 
    nasm 
    mtools 
    qemu-system-x86 
    python3-pip 
    python3-scons
)

DEPS_FEDORA=(
    gcc 
    gcc-c++ 
    make 
    bison 
    flex 
    gmp-devel 
    libmpc-devel 
    mpfr-devel 
    texinfo 
    nasm 
    mtools 
    qemu-system-x86 
    python3-pip 
    python3-scons
)

DEPS_ARCH=(
    gcc
    make
    bison
    flex
    libgmp-static
    libmpc
    mpfr
    texinfo
    nasm
    mtools
    qemu-system-x86
    python3-pip
    scons
)

DEPS_SUSE=()

OS=
PACKAGE_UPDATE=
PACKAGE_INSTALL=
DEPS=

# Detect distro
if [ -x "$(command -v apk)" ]; then
    OS='alpine'
    PACKAGE_UPDATE='apk update'
    PACKAGE_INSTALL='apk add --no-cache'
    echo "Alpine not supported."
    exit 1
elif [ -x "$(command -v apt-get)" ]; then
    OS='debian'
    PACKAGE_UPDATE='apt-get update'
    PACKAGE_INSTALL='apt-get install'
    DEPS="${DEPS_DEBIAN[@]}"
elif [ -x "$(command -v dnf)" ]; then
    OS='fedora'
    PACKAGE_INSTALL='dnf install'
    DEPS="${DEPS_FEDORA[@]}"
elif [ -x "$(command -v yum)" ]; then
    OS='fedora'
    PACKAGE_INSTALL='yum install'
    DEPS="${DEPS_FEDORA[@]}"
elif [ -x "$(command -v zypper)" ]; then
    OS='suse'
    PACKAGE_INSTALL='zypper install'
    DEPS="$DEPS_SUSE"
elif [ -x "$(command -v pacman)" ]; then
    OS='arch'
    PACKAGE_UPDATE='pacman -Syy'
    PACKAGE_INSTALL='pacman -S'
    DEPS="${DEPS_ARCH[@]}"
else
    echo "Unknown operating system!"; 
fi

# Install dependencies
echo ""
echo "Will install dependencies by running the following command."
echo ""
echo " $ $PACKAGE_INSTALL ${DEPS[@]}"
echo ""

read -p "Continue (y/n)?" choice
case "$choice" in 
  y|Y ) ;;
  * ) echo "Exiting..."
        exit 0
        ;;
esac

$PACKAGE_UPDATE
$PACKAGE_INSTALL ${DEPS[@]}
