#pragma once

#include <hal/fs/FileSystem.hpp>
#include <hal/fs/FileSystemFactory.hpp>
#include <util/ResultCode.hpp>
#include "VFSRootNode.hpp"

#include <etl/map.h>
#include <etl/string.h>
#include <etl/string_view.h>
#include <etl/vector.h>

#define VFS_MAX_MOUNTPOINTS 16
#define VFS_MAX_MOUNT_POINT_LENGTH 64
#define VFS_MAX_FILE_SYSTEM_FACTORIES 16

namespace hal {

class VirtualFileSystem : public fs::FileSystem
{
public:
    VirtualFileSystem();

    void RegisterFileSystem(fs::FileSystemFactory* factory, ErrorChain& err);

    void Mount(const etl::string_view& mountPoint, fs::FileSystem* fs, ErrorChain& err);
    void Mount(const etl::string_view& mountPoint, fs::File* device, const etl::string_view& options, ErrorChain& err);
    void Unmount(const etl::string_view& mountPoint, ErrorChain& err);
    
    fs::FSNode* Locate(const etl::string_view& path, ErrorChain& err);
    fs::FSNode* RootDirectory(ErrorChain& err) override;

private:
    VFSRootNode m_Root;
    etl::map<etl::string_view, fs::FileSystem*, VFS_MAX_MOUNTPOINTS> m_MountPoints;
    etl::vector<etl::string<VFS_MAX_MOUNT_POINT_LENGTH>, VFS_MAX_MOUNTPOINTS> m_Names;
    etl::vector<fs::FileSystemFactory*, VFS_MAX_FILE_SYSTEM_FACTORIES> m_FSFactories;

    friend class VFSRootNode;
};

}