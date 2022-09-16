#pragma once
#include <dev/BlockDevice.hpp>
#include "File.hpp"
#include "FileEntry.hpp"

enum FileOpenMode
{
    Read,
    Write,
    Append
};

class FileSystem
{
public:
    virtual bool Initialize(BlockDevice* device) = 0;
    virtual File* Open(FileEntry* parent, FileOpenMode mode) = 0;
    virtual File* RootDirectory() = 0;
};