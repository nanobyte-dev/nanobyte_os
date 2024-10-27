#pragma once

#include <hal/fs/FileSystem.hpp>
#include "DeviceFSEntry.hpp"

#include <etl/map.h>
#include <etl/string.h>
#include <etl/string_view.h>
#include <etl/vector.h>
#include <etl/variant.h>

#define DEVICE_FS_MAX_DEVICES 64
#define DEVICE_FS_MAX_DEVICE_NAME_LENGTH 64

class DeviceFileSystem : public FileSystem
{
private:
    struct RegisteredDevice
    {
        etl::string<DEVICE_FS_MAX_DEVICE_NAME_LENGTH> Name;
        etl::variant<BlockDevice*, CharacterDevice*> Device;
    };

public:
    DeviceFileSystem();

    etl::expected<FSEntry*, Result> RootDirectory() override;

    Result RegisterDevice(const etl::string_view& name, BlockDevice* device);
    Result RegisterDevice(const etl::string_view& name, CharacterDevice* device);

private:
    etl::expected<FSEntry*, Result> FindChild(int parentHandle, const etl::string_view& childName);
    etl::expected<FSEntry*, Result> FirstChild(int parentHandle);
    etl::expected<FSEntry*, Result> NextSibling(int childHandle);
    etl::expected<FSEntry*, Result> Parent(int childHandle);

    DeviceFSEntry m_RootDirectory;

    etl::vector<RegisteredDevice, DEVICE_FS_MAX_DEVICES> m_Devices;
    etl::map<etl::string_view, int, DEVICE_FS_MAX_DEVICES> m_DevicesByName;

    friend class DeviceFSEntry;
};