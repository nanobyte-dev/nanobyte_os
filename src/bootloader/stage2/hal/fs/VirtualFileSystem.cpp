#include "VirtualFileSystem.hpp"
#include <core/Memory.hpp>

VirtualFileSystem::VirtualFileSystem()
{
}

Result VirtualFileSystem::RegisterFileSystem(FileSystemFactory *factory)
{
    if (m_FSFactories.size() >= VFS_MAX_FILE_SYSTEM_FACTORIES)
        return Result::OutOfFileSystemFactories;

    m_FSFactories.push_back(factory);

    return Result::Ok;
}

Result VirtualFileSystem::Mount(const etl::string_view& mountPoint, FileSystem* fs)
{
    if (m_MountPoints.size() >= VFS_MAX_MOUNTPOINTS)
        return Result::OutOfMountpoints;

    if (m_MountPoints.find(mountPoint) != m_MountPoints.end())
        return Result::MountpointInUse;

    auto str = m_Names.emplace_back(mountPoint);
    m_MountPoints[str] = fs;

    return Result::Ok;
}

Result VirtualFileSystem::Mount(const etl::string_view& mountPoint, BlockDevice* device, const etl::string_view& options)
{
    for (auto& factory : m_FSFactories)
    {
        if (factory->Probe(device, options))
        {
            auto result = factory->Create(device, options);
            if (result)
                return Mount(mountPoint, result.value());

            return result.error();
        }
    }
    return Result::UnknownFileSystem;
}

Result VirtualFileSystem::Unmount(const etl::string_view& mountPoint)
{
    auto it = m_MountPoints.find(mountPoint);
    if (it == m_MountPoints.end())
        return Result::MountpointNotFound;

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

    return Result::Ok;
}

etl::expected<FSEntry *, Result> VirtualFileSystem::Locate(const etl::string_view &path)
{
    // Sanity checks
    if (path.empty())
        return etl::unexpected(Result::InvalidPath);

    if (path[0] != '/')
        return etl::unexpected(Result::AbsolutePathRequired);

    // Find the appropriate file system
    FileSystem* fs = nullptr;
    etl::string_view remainingPath;
    
    size_t firstSlash = path.find('/', 1);
    etl::string_view mountPoint = path.substr(1, firstSlash);

    auto it = m_MountPoints.find(mountPoint);
    if (it == m_MountPoints.end())
        return etl::unexpected(Result::MountpointNotFound);

    fs = it->second;
    remainingPath = path.substr(firstSlash);

    // Get the root directory
    auto rootDirectory = fs->RootDirectory();
    if (!rootDirectory)
        return etl::unexpected(rootDirectory.error());

    // Iterate through path components
    FSEntry* currentEntry = rootDirectory.value();
    while (remainingPath.length() > 0)
    {
        size_t slash_pos = remainingPath.find('/');
        etl::string_view component = remainingPath.substr(0, slash_pos);
        
        if (!component.empty() && component != ".")
        {
            if (component == "..")
            {
                auto parentResult = fs->Parent(currentEntry);
                if (!parentResult)
                    return etl::unexpected(parentResult.error());
                
                currentEntry = parentResult.value();
            }
            
            else
            {
                auto childResult = fs->FindChild(currentEntry, component);
                if (!childResult)
                    return etl::unexpected(childResult.error());
                
                currentEntry = childResult.value();
            }
        }

        remainingPath = remainingPath.substr(slash_pos + 1);
    }

    return currentEntry;
}
