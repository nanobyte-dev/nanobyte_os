#include "DeviceFileSystem.hpp"

constexpr int RootDirectoryHandle = -1;

DeviceFileSystem::DeviceFileSystem()
    : m_RootDirectory("", FileType::Directory, RootDirectoryHandle)
{
}

etl::expected<FSEntry*, Result> DeviceFileSystem::RootDirectory()
{
    return &m_RootDirectory;
}

etl::expected<FSEntry*, Result> DeviceFileSystem::FindChild(int parentHandle, const etl::string_view& childName)
{
    if (parentHandle != RootDirectoryHandle)
        return etl::unexpected(Result::NotFound);

    auto it = m_DevicesByName.find(childName);
    if (it == m_DevicesByName.end())
        return etl::unexpected(Result::NotFound);

    int childHandle = it->second;
    return new DeviceFSEntry(this, childHandle, it->first, FileType::File);
}

etl::expected<FSEntry*, Result> DeviceFileSystem::FirstChild(int parentHandle)
{
    if (parentHandle != RootDirectoryHandle)
        return etl::unexpected(Result::DirectoryExpected);

    if (m_Devices.empty())
        return nullptr;

    return new DeviceFSEntry(this, 0, m_Devices[0].Name, FileType::File);
}

etl::expected<FSEntry*, Result> DeviceFileSystem::NextSibling(int childHandle)
{
    if (childHandle == RootDirectoryHandle)
        return nullptr;

    int nextHandle = childHandle + 1;
    if (nextHandle >= m_Devices.size())
        return nullptr;

    return new DeviceFSEntry(this, 0, m_Devices[nextHandle].Name, FileType::File);
}

etl::expected<FSEntry*, Result> DeviceFileSystem::Parent(int childHandle)
{
    if (childHandle == RootDirectoryHandle)
        return nullptr;

    return &m_RootDirectory;
}

Result DeviceFileSystem::RegisterDevice(const etl::string_view& name, BlockDevice* device)
{
    if (m_Devices.size() >= DEVICE_FS_MAX_DEVICES)
        return Result::OutOfDevices;

    m_Devices.push_back({ name, device });
    return Result::Ok;
}