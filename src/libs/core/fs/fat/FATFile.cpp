#include "FATFile.hpp"
#include <fs/FATFileSystem.hpp>
#include <core/Debug.hpp>
#include <core/cpp/Algorithm.hpp>
#include <core/Memory.hpp>

#define LOG_MODULE "FatFile"

FATFile::FATFile()
    : m_FS(nullptr),
      m_Opened(false),
      m_IsRootDirectory(false),
      m_Position(),
      m_Size(),
      m_FirstCluster(),
      m_CurrentCluster(),
      m_CurrentClusterIndex(0),
      m_CurrentSectorInCluster()
{
}

bool FATFile::Open(FATFileSystem* fs, uint32_t firstCluster, const char* name, uint32_t size, bool isDirectory)
{
    m_IsRootDirectory = false;
    m_Position = 0;
    m_Size = size;
    m_IsDirectory = isDirectory;
    m_FirstCluster = firstCluster;
    m_CurrentCluster = m_FirstCluster;
    m_CurrentClusterIndex = 0;
    m_CurrentSectorInCluster = 0;

    if (!m_FS->ReadSectorFromCluster(m_CurrentCluster, m_CurrentSectorInCluster, m_Buffer))
    {
        Debug::Error(LOG_MODULE, "FAT: open file %s failed - read error cluster=%u\n", name, m_CurrentCluster);
        return false;
    }

    m_Opened = true;
    return true;
}

bool FATFile::OpenRootDirectory1216(FATFileSystem* fs, uint32_t rootDirLba, uint32_t rootDirSize)
{
    m_IsRootDirectory = true;
    m_Position = 0;
    m_Size = rootDirSize;
    m_FirstCluster = rootDirLba;
    m_CurrentCluster = m_FirstCluster;
    m_CurrentClusterIndex = 0;
    m_CurrentSectorInCluster = 0;

    if (!m_FS->ReadSector(rootDirLba, m_Buffer))
    {
        Debug::Error(LOG_MODULE, "FAT: read root directory failed\r\n");
        return false;
    }

    return true;
}

void FATFile::Release()
{
    m_FS->ReleaseFile(this);
}

bool FATFile::ReadFileEntry(FATDirectoryEntry* dirEntry)
{
    return Read(reinterpret_cast<uint8_t*>(dirEntry), sizeof(FATDirectoryEntry)) == sizeof(FATDirectoryEntry);
}

size_t FATFile::Read(uint8_t* data, size_t byteCount)
{
    uint8_t* originalData = data;

    // don't read past the end of the file
    if (!m_IsDirectory || (m_IsDirectory && m_Size != 0))
        byteCount = Min(byteCount, m_Size - m_Position);

    while (byteCount > 0)
    {
        uint32_t leftInBuffer = SectorSize - (m_Position % SectorSize);
        uint32_t take = Min(byteCount, leftInBuffer);

        Memory::Copy(data, m_Buffer + m_Position % SectorSize, take);
        data += take;
        m_Position += take;
        byteCount -= take;

        // printf("leftInBuffer=%lu take=%lu\r\n", leftInBuffer, take);
        // See if we need to read more data
        if (leftInBuffer == take)
        {
            // Special handling for root directory
            if (m_IsRootDirectory)
            {
                ++m_CurrentCluster;

                // read next sector
                if (!m_FS->ReadSector(m_CurrentCluster, m_Buffer))
                {
                    Debug::Error(LOG_MODULE, "FAT: read error!\r\n");
                    break;
                }
            }
            else
            {
                // calculate next cluster & sector to read
                if (++m_CurrentSectorInCluster >= m_FS->Data().BS.BootSector.SectorsPerCluster)
                {
                    m_CurrentSectorInCluster = 0;
                    m_CurrentCluster = m_FS->GetNextCluster(m_CurrentCluster);
                    ++m_CurrentClusterIndex;
                }

                if (m_CurrentCluster >= 0xFFFFFFF8)
                {
                    // Mark end of file
                    m_Size = m_Position;
                    break;
                }

                // read next sector
                if (!m_FS->ReadSectorFromCluster(m_CurrentCluster, m_CurrentSectorInCluster, m_Buffer))
                {
                    Debug::Error(LOG_MODULE, "FAT: read error!");
                    break;
                }
            }
        }
    }

    return data - originalData;
}

size_t FATFile::Write(const uint8_t* data, size_t size)
{
    // not supported (yet)
    return 0;
}

bool FATFile::Seek(SeekPos pos, int rel)
{
    switch (pos)
    {
    case SeekPos::Set:
        m_Position = static_cast<uint32_t>(Max(0, rel));
        break;
    
    case SeekPos::Current:
        if (rel < 0 && m_Position < -rel)
            m_Position = 0;

        m_Position = Min(Size(), static_cast<uint32_t>(m_Position + rel));
        
    case SeekPos::End:
        if (rel < 0 && Size() < -rel)
            m_Position = 0;
        m_Position = Min(Size(), static_cast<uint32_t>(Size() + rel));

        break;
    }

    UpdateCurrentCluster();
    return true;
}

bool FATFile::UpdateCurrentCluster()
{
    uint32_t clusterSize = m_FS->Data().BS.BootSector.SectorsPerCluster * SectorSize;
    uint32_t desiredCluster = m_Position / clusterSize;
    uint32_t desiredSector = (m_Position % clusterSize) / SectorSize;
    
    if (desiredCluster == m_CurrentClusterIndex && desiredSector == m_CurrentSectorInCluster)
        return true;

    if (desiredCluster < m_CurrentClusterIndex)
    {
        m_CurrentClusterIndex = 0;
        m_CurrentCluster = m_FirstCluster;
    }

    while (desiredCluster > m_CurrentClusterIndex) 
    {
        m_CurrentCluster = m_FS->GetNextCluster(m_CurrentCluster);
        ++m_CurrentClusterIndex;
    }

    m_CurrentSectorInCluster = desiredSector;
    return m_FS->ReadSectorFromCluster(m_CurrentCluster, m_CurrentSectorInCluster, m_Buffer);
}

FileEntry* FATFile::ReadFileEntry()
{
    FATDirectoryEntry entry;
    if (ReadFileEntry(&entry))
    {
        FATFileEntry* fileEntry = m_FS->AllocateFileEntry();
        fileEntry->Initialize(m_FS, entry);
        return fileEntry;
    }

    return nullptr;
}