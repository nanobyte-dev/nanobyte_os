#pragma once
#include "../dev/BlockDevice.hpp"

class FileEntry;

class File : public BlockDevice 
{
public:
    virtual FileEntry* ReadFileEntry() = 0;
    virtual void Release() = 0;
};