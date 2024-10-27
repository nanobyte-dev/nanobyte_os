#include "DeviceFSEntry.hpp"
#include "DeviceFileSystem.hpp"

DeviceFSEntry::DeviceFSEntry(DeviceFileSystem* fs, int handle, const etl::string_view& name, const FileType& type)
    : m_FS(fs), m_Handle(handle), m_Name(name), m_Type(type)
{
}

etl::string_view DeviceFSEntry::Name()
{
    return m_Name;
}

const FileType DeviceFSEntry::Type()
{
    return m_Type;
}

etl::expected<File*, Result> DeviceFSEntry::Open(FileOpenMode mode)
{
    return etl::unexpected(Result::NotImplemented);
}

etl::expected<FSEntry*, Result> DeviceFSEntry::FindChild(const etl::string_view& childName)
{
    return m_FS->FindChild(m_Handle, childName);
}

etl::expected<FSEntry*, Result> DeviceFSEntry::FirstChild()
{
    return m_FS->FirstChild(m_Handle);
}

etl::expected<FSEntry*, Result> DeviceFSEntry::NextSibling()
{
    return m_FS->NextSibling(m_Handle);
}

etl::expected<FSEntry*, Result> DeviceFSEntry::Parent()
{
    return m_FS->Parent(m_Handle);
}
