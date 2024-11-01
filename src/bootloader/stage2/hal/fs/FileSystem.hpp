#pragma once

#include "FSNode.hpp"
#include <util/ErrorChain.hpp>

#include <etl/expected.h>
#include <etl/string_view.h>
#include <etl/vector.h>

namespace hal::fs
{

class FileSystem
{
public:
    virtual FSNode* RootDirectory(ErrorChain& err) = 0;
};

}