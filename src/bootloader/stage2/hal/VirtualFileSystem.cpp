#include "VirtualFileSystem.hpp"
#include <core/Memory.hpp>
#include <hal/fs/PathIterator.hpp>
#include <Debug.hpp>

#define MODULE "VirtualFileSystem"

namespace hal {

VirtualFileSystem::VirtualFileSystem()
    : m_Root(this)
{
}

void VirtualFileSystem::RegisterFileSystem(fs::FileSystemFactory *factory, ErrorChain& err)
{
    if (err.Failed()) return;

    if (m_FSFactories.size() >= VFS_MAX_FILE_SYSTEM_FACTORIES)
    {
        err.Fail(ResultCode::ContainerFull, "System factory container full");
        return;
    }

    m_FSFactories.push_back(factory);
}

void VirtualFileSystem::Mount(const etl::string_view& mountPoint, fs::FileSystem* fs, ErrorChain& err)
{
    if (err.Failed()) return;

    if (m_MountPoints.size() >= VFS_MAX_MOUNTPOINTS)
    {
        err.Fail(ResultCode::ContainerFull, "Out of mount points");
        return;
    }

    if (m_MountPoints.find(mountPoint) != m_MountPoints.end())
    {
        err.Fail(ResultCode::MountpointInUse, "Mountpoint in use");
        return;
    }

    auto str = m_Names.emplace_back(mountPoint);
    m_MountPoints[str] = fs;
    fs->RootDirectory(err)->Rename(mountPoint, err);
}

void VirtualFileSystem::Mount(const etl::string_view& mountPoint, fs::File* device, const etl::string_view& options, ErrorChain& err)
{
    if (err.Failed()) return;

    for (auto& factory : m_FSFactories)
    {
        if (factory->Probe(device, options, err))
        {
            auto fs = factory->Create(device, options, err);
            Mount(mountPoint, fs, err);
        }
        else if (err.Failed())
        {
            Debug::Debug(MODULE, "Failed to probe %s:", mountPoint.data());
            Debug::Debug(MODULE, "  %s", err.Message().data());
            err.Clear();
        }
    }

    err.Fail(ResultCode::UnknownFileSystem, "Unknown file system");
}

void VirtualFileSystem::Unmount(const etl::string_view& mountPoint, ErrorChain& err)
{
    auto it = m_MountPoints.find(mountPoint);
    if (it == m_MountPoints.end())
    {
        err.Fail(ResultCode::MountpointNotFound, "Mountpoint not found");
        return;
    }

    m_MountPoints.erase(it);

    // Remove the mountPoint from m_Names
    for (auto nameIt = m_Names.begin(); nameIt != m_Names.end(); ++nameIt)
    {
        if (*nameIt == mountPoint)
        {
            m_Names.erase(nameIt);
            break;
        }
    }
}

fs::FSNode* VirtualFileSystem::Locate(const etl::string_view &path, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    // Sanity checks
    if (path.empty())
    {
        err.Fail(ResultCode::InvalidPath, "Empty path");
        return nullptr;
    }

    if (path[0] != '/')
    {
        err.Fail(ResultCode::AbsolutePathRequired, "Absolute path required");
        return nullptr;
    }

    err.Fail(ResultCode::NotImplemented, "Not implemented");
    return nullptr;
    // Find the appropriate file system
    // fs::FileSystem* fs = nullptr;
    // fs::PathIterator pathIterator(path);
    
    
    // auto it = m_MountPoints.find(mountPoint);
    // if (it == m_MountPoints.end())
    //     return err.Fail(ResultCode::MountpointNotFound);

    // fs = it->second;
    // remainingPath = path.substr(firstSlash);

    // // Get the root directory
    // auto rootDirectory = fs->RootDirectory();
    // if (!rootDirectory)
    //     return err.Fail(rootDirectory.error());

    // // Iterate through path components
    // fs::FSNode* currentEntry = rootDirectory.value();
    // while (remainingPath.length() > 0)
    // {
    //     size_t slash_pos = remainingPath.find('/');
    //     etl::string_view component = remainingPath.substr(0, slash_pos);
        
    //     if (!component.empty() && component != ".")
    //     {
    //         if (component == "..")
    //         {
    //             auto parentResult = fs->Parent(currentEntry);
    //             if (!parentResult)
    //                 return err.Fail(parentResult.error());
                
    //             currentEntry = parentResult.value();
    //         }
            
    //         else
    //         {
    //             auto childResult = fs->FindChild(currentEntry, component);
    //             if (!childResult)
    //                 return err.Fail(childResult.error());
                
    //             currentEntry = childResult.value();
    //         }
    //     }

    //     remainingPath = remainingPath.substr(slash_pos + 1);
    // }

    // return currentEntry;
}

fs::FSNode *VirtualFileSystem::RootDirectory(ErrorChain &err)
{
    return &m_Root;
}
}