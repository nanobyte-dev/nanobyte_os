#pragma once

#include <etl/string_view.h>
#include <etl/expected.h>
#include <hal/fs/File.hpp>
#include <hal/fs/FileSystem.hpp>

namespace hal::fs
{

class FileSystemFactory
{
public:
    virtual FileSystem* Create(File* device, const etl::string_view& options, ErrorChain& err) = 0;
    virtual bool Probe(File* device, const etl::string_view& options, ErrorChain& err) = 0;
};

}