#pragma once
#include "../dev/BlockDevice.hpp"
#include "File.hpp"
#include "FileEntry.hpp"

#define MAX_PATH_SIZE 256


class FileSystem
{
public:
    virtual bool Initialize(BlockDevice* device) = 0;
    virtual File* RootDirectory() = 0;

    virtual File* Open(const char* path, FileOpenMode mode);

private:
    virtual FileEntry* FindFile(File* parentDirectory, const char* name);
};