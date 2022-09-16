#pragma once
#include <dev/BlockDevice.hpp>

class File : public BlockDevice 
{
public:
    virtual bool ReadFileEntry(FileEntry& entryOut) = 0;
};