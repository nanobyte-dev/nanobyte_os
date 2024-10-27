#include "Hal.hpp"
#include <hal/fs/VirtualFileSystem.hpp>

VirtualFileSystem g_VFS;

namespace Hal
{
    void Initialize()
    {
        //g_VFS.Mount("/", new FATFileSystem(g_SDHC));
    }
}