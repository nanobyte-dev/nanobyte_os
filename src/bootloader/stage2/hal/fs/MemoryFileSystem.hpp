#pragma once
#include <stddef.h>
#include <etl/vector.h>
#include "FileSystem.hpp"
#include "MemoryFSNode.hpp"

namespace hal::fs
{

class MemoryFileSystem : public FileSystem
{
public:
    virtual void DestroyFSNode(MemoryFSNode* entry) = 0;
};

}