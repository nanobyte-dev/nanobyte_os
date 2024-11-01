#include "FATFileSystem.hpp"
#include <fs/fat/FATHeaders.hpp>
#include <fs/fat/FATFileEntry.hpp>
#include <Debug.hpp>

#define LOG_MODULE "FAT"

FATFileSystem::FATFileSystem() : 
    m_Device(),
    m_Data(new FATData()),
    m_DataSectionLba(),
    m_FatType(),
    m_TotalSectors(),
    m_SectorsPerFat()
{
}

bool FATFileSystem::Initialize(BlockDevice* device)
{
    m_Device = device;

    if (!ReadBootSector())
    {
        Debug::Error(LOG_MODULE, "FAT: read boot sector failed");
        return false;
    }

    m_Data->FatCachePosition = 0xFFFFFFFF;

    m_TotalSectors = m_Data->BS.BootSector.TotalSectors;
    if (m_TotalSectors == 0) {          // fat32
        m_TotalSectors = m_Data->BS.BootSector.LargeSectorCount;
    }

    bool isFat32 = false;
    m_SectorsPerFat = m_Data->BS.BootSector.SectorsPerFat;
    if (m_SectorsPerFat == 0) {         // fat32
        isFat32 = true;
        m_SectorsPerFat = m_Data->BS.BootSector.EBR32.SectorsPerFat;
    }
    
    // open root directory file
    uint32_t rootDirLba;
    uint32_t rootDirSize;
    if (isFat32) {
        m_DataSectionLba = m_Data->BS.BootSector.ReservedSectors + m_SectorsPerFat * m_Data->BS.BootSector.FatCount;
        if (!m_Data->RootDirectory.Open(this, m_Data->BS.BootSector.EBR32.RootDirectoryCluster, "", 0, true))
            return false;
    }
    else {
        rootDirLba = m_Data->BS.BootSector.ReservedSectors + m_SectorsPerFat * m_Data->BS.BootSector.FatCount;
        rootDirSize = sizeof(FATDirectoryEntry) * m_Data->BS.BootSector.DirEntryCount;
        uint32_t rootDirSectors = (rootDirSize + m_Data->BS.BootSector.BytesPerSector - 1) / m_Data->BS.BootSector.BytesPerSector;
        m_DataSectionLba = rootDirLba + rootDirSectors;

        if (!m_Data->RootDirectory.OpenRootDirectory1216(this, rootDirLba, rootDirSize))
            return false;
    }

    DetectFatType();

    m_Data->LFNCount = 0;

    return true;
}

FATFile* FATFileSystem::AllocateFile()
{
    return m_Data->OpenedFilePool.Allocate();
}

void FATFileSystem::ReleaseFile(FATFile* file)
{
    m_Data->OpenedFilePool.Free(file);
}

FATFileEntry* FATFileSystem::AllocateFileEntry()
{
    return m_Data->FileEntryPool.Allocate();
}

void FATFileSystem::ReleaseFileEntry(FATFileEntry* fileEntry)
{
    m_Data->FileEntryPool.Free(fileEntry);
}


File* FATFileSystem::RootDirectory()
{
    return &m_Data->RootDirectory;
}

bool FATFileSystem::ReadSector(uint32_t lba, uint8_t* buffer, size_t count)
{
    m_Device->Seek(SeekPos::Set, lba * SectorSize);
    return (m_Device->Read(buffer, count * SectorSize) == count * SectorSize);
}

bool FATFileSystem::ReadSectorFromCluster(uint32_t cluster, uint32_t sectorOffset, uint8_t* buffer)
{
    return ReadSector(ClusterToLba(cluster) + sectorOffset, buffer);
}

bool FATFileSystem::ReadBootSector()
{
    return ReadSector(0, m_Data->BS.BootSectorBytes);
}

uint32_t FATFileSystem::ClusterToLba(uint32_t cluster)
{
    return m_DataSectionLba + (cluster - 2) * m_Data->BS.BootSector.SectorsPerCluster;
}

void FATFileSystem::DetectFatType()
{
    uint32_t dataClusters = (m_TotalSectors - m_DataSectionLba) / m_Data->BS.BootSector.SectorsPerCluster;
    if (dataClusters < 0xFF5) 
        m_FatType = 12;
    else if (m_Data->BS.BootSector.SectorsPerFat != 0)
        m_FatType = 16;
    else m_FatType = 32;
}

uint32_t FATFileSystem::GetNextCluster(uint32_t currentCluster)
{    
    // Determine the byte offset of the entry we need to read
    uint32_t fatIndex;
    if (m_FatType == 12) {
        fatIndex = currentCluster * 3 / 2;
    }
    else if (m_FatType == 16) {
        fatIndex = currentCluster * 2;
    }
    else /*if (m_FatType == 32)*/ {
        fatIndex = currentCluster * 4;
    }

    // Make sure cache has the right number
    uint32_t fatIndexSector = fatIndex / SectorSize;
    if (fatIndexSector < m_Data->FatCachePosition 
        || fatIndexSector >= m_Data->FatCachePosition + FatCacheSize)
    {
        ReadFat(fatIndexSector);
        m_Data->FatCachePosition = fatIndexSector;
    }

    fatIndex -= (m_Data->FatCachePosition * SectorSize);

    uint32_t nextCluster;
    if (m_FatType == 12) {
        if (currentCluster % 2 == 0)
            nextCluster = (*(uint16_t*)(m_Data->FatCache + fatIndex)) & 0x0FFF;
        else
            nextCluster = (*(uint16_t*)(m_Data->FatCache + fatIndex)) >> 4;
        
        if (nextCluster >= 0xFF8) {
            nextCluster |= 0xFFFFF000;
        }
    }
    else if (m_FatType == 16) {
        nextCluster = *(uint16_t*)(m_Data->FatCache + fatIndex);
        if (nextCluster >= 0xFFF8) {
            nextCluster |= 0xFFFF0000;
        }
    }
    else /*if (m_FatType == 32)*/ {
        nextCluster = *(uint32_t*)(m_Data->FatCache + fatIndex);
    }

    return nextCluster;
}

bool FATFileSystem::ReadFat(uint32_t lbaOffset)
{
    return ReadSector(m_Data->BS.BootSector.ReservedSectors + lbaOffset, m_Data->FatCache, FatCacheSize);
}