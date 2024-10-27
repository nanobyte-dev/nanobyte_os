#pragma once

#include "FSEntry.hpp"
#include <util/Result.hpp>

#include <etl/expected.h>
#include <etl/string_view.h>
#include <etl/vector.h>

class FileSystem
{
public:
    virtual etl::expected<FSEntry*, Result> RootDirectory() = 0;
    virtual etl::expected<FSEntry*, Result> FindChild(FSEntry* parent, const etl::string_view& childName) = 0;
    virtual etl::expected<FSEntry*, Result> FirstChild(FSEntry* parent) = 0;
    virtual etl::expected<FSEntry*, Result> NextChild(FSEntry* child) = 0;
    virtual etl::expected<FSEntry*, Result> Parent(FSEntry* child) = 0;
};