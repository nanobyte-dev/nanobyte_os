#pragma once

#include <hal/fs/FSEntry.hpp>

class DeviceFileSystem;

class DeviceFSEntry : public FSEntry
{
public:
    DeviceFSEntry(DeviceFileSystem* fs, int handle, const etl::string_view& name, const FileType& type);
    
    etl::string_view Name() override;
    const FileType Type() override;

    etl::expected<File*, Result> Open(FileOpenMode mode) override;

    etl::expected<FSEntry*, Result> FindChild(const etl::string_view& childName) override;
    etl::expected<FSEntry*, Result> FirstChild() override;
    etl::expected<FSEntry*, Result> NextSibling() override;
    etl::expected<FSEntry*, Result> Parent() override;

private:
    DeviceFileSystem* m_FS;
    int m_Handle;
    etl::string_view m_Name;
    FileType m_Type;
};