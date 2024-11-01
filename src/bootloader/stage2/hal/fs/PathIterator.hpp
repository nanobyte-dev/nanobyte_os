#pragma once

#include <etl/string_view.h>
#include <etl/optional.h>

namespace hal::fs
{

class PathIterator
{
public:
    PathIterator(const etl::string_view& path);
    bool IsPathRooted() const;
    etl::optional<etl::string_view> Next();
    etl::string_view Rest() const;

private:
    etl::string_view m_Path;
    size_t m_Pos;
};

}