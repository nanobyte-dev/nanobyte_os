#pragma once

#include "FileSystem.hpp"
#include "FileSystemFactory.hpp"
#include <util/Result.hpp>

#include <etl/map.h>
#include <etl/string.h>
#include <etl/string_view.h>
#include <etl/vector.h>

#define VFS_MAX_MOUNTPOINTS 16
#define VFS_MAX_MOUNT_POINT_LENGTH 64
#define VFS_MAX_FILE_SYSTEM_FACTORIES 16

class VirtualFileSystem
{
public:
    VirtualFileSystem();

    Result RegisterFileSystem(FileSystemFactory* factory);

    Result Mount(const etl::string_view& mountPoint, FileSystem* fs);
    Result Mount(const etl::string_view& mountPoint, BlockDevice* device, const etl::string_view& options);
    Result Unmount(const etl::string_view& mountPoint);

    etl::expected<FSEntry*, Result> Locate(const etl::string_view& path);

private:
    etl::map<etl::string_view, FileSystem*, VFS_MAX_MOUNTPOINTS> m_MountPoints;
    etl::vector<etl::string<VFS_MAX_MOUNT_POINT_LENGTH>, VFS_MAX_MOUNTPOINTS> m_Names;

    etl::vector<FileSystemFactory*, VFS_MAX_FILE_SYSTEM_FACTORIES> m_FSFactories;
};