#pragma once

#include <core/dev/BlockDevice.hpp>

class FSEntry;

class File : public BlockDevice 
{
public:
    virtual FSEntry* Entry() = 0;
};