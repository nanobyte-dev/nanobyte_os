#pragma once
#include "core/dev/BlockDevice.hpp"

class FSEntry;

class File : public BlockDevice 
{
public:
    virtual FSEntry* ReadFileEntry() = 0;
    virtual void Release() = 0;
};