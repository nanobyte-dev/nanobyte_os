#pragma once

#include "FileSystem.hpp"

class VirtualFileSystem
{
public:
    VirtualFileSystem();
    void Mount(FileSystem* fs, const char* mountPoint);
    void Unmount(const char* mountPoint);
    FileSystem* Lookup(const char* path);


private:
    FileSystem* m_Root;
    
};