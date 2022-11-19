#include "FATFileEntry.hpp"
#include <fs/fat/FATFile.hpp>
#include <fs/FATFileSystem.hpp>

FATFileEntry::FATFileEntry()
    : m_FS(),
      m_DirEntry()
{
}

void FATFileEntry::Release()
{
    m_FS->ReleaseFileEntry(this);
}

void FATFileEntry::Initialize(FATFileSystem* fs, const FATDirectoryEntry& dirEntry)
{
    m_FS = fs;
    m_DirEntry = dirEntry;
}

const char* FATFileEntry::Name()
{
    return reinterpret_cast<const char*>(m_DirEntry.Name);
}

const FileType FATFileEntry::Type()
{
    if (m_DirEntry.Attributes & FAT_ATTRIBUTE_DIRECTORY)
        return FileType::Directory;

    return FileType::File;
}

File* FATFileEntry::Open(FileOpenMode mode)
{
    FATFile* file = m_FS->AllocateFile();
    if (file == nullptr)
    {
        // todo: print error
        return nullptr;
    }

    // setup vars
    uint32_t size = m_DirEntry.Size;
    uint32_t firstCluster = m_DirEntry.FirstClusterLow + ((uint32_t)m_DirEntry.FirstClusterHigh << 16);

    if (!file->Open(m_FS, firstCluster, Name(), size, m_DirEntry.Attributes & FAT_ATTRIBUTE_DIRECTORY))
    {
        m_FS->ReleaseFile(file);
        return nullptr;
    }

    return file;
}
