#pragma once

#include <core/dev/BlockDevice.hpp>
#include <hal/fs/FileSystem.hpp>
#include <etl/string_view.h>
#include <etl/expected.h>

class FileSystemFactory
{
public:
    virtual etl::expected<FileSystem*, Result> Create(BlockDevice* device, const etl::string_view& options) = 0;
    virtual bool Probe(BlockDevice* device, const etl::string_view& options) = 0;
};