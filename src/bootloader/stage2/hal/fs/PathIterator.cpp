#include "PathIterator.hpp"

hal::fs::PathIterator::PathIterator(const etl::string_view &path)
    : m_Path(path), m_Pos(0)
{
}

bool hal::fs::PathIterator::IsPathRooted() const
{
    return m_Path[0] == '/';
}

etl::optional<etl::string_view> hal::fs::PathIterator::Next()
{
    if (m_Pos >= m_Path.size())
        return {};

    // skip root slash
    if (m_Pos == 0 && m_Path[0] == '/')
        m_Pos++;

    size_t nextSlash = m_Path.find('/', m_Pos);
    size_t begin = m_Pos;

    if (nextSlash == etl::string_view::npos)
    {
        m_Pos = etl::string_view::npos;    
        return m_Path.substr(begin);
    }
    else
    {
        m_Pos = nextSlash + 1;   
        return m_Path.substr(begin, nextSlash - m_Pos);
    }
}

etl::string_view hal::fs::PathIterator::Rest() const
{
    return m_Path.substr(m_Pos);
}
