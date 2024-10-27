#pragma once

#include "File.hpp"
#include <util/Result.hpp>
#include <etl/expected.h>
#include <etl/string_view.h>

enum class FileType
{
    File,
    Directory
};

enum FileOpenMode
{
    Read,
    Write,
    Append
};

class FSEntry
{
public:
    virtual etl::string_view Name() = 0;
    virtual const FileType Type() = 0;

    virtual etl::expected<File*, Result> Open(FileOpenMode mode) = 0;

    virtual etl::expected<FSEntry*, Result> FindChild(const etl::string_view& childName) = 0;
    virtual etl::expected<FSEntry*, Result> FirstChild() = 0;
    virtual etl::expected<FSEntry*, Result> NextSibling() = 0;
    virtual etl::expected<FSEntry*, Result> Parent() = 0;
};