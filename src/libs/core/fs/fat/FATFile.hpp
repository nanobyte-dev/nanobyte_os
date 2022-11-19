#pragma once
#include "../File.hpp"
#include "../FileEntry.hpp"
#include "../fat/FATHeaders.hpp"
#include "../fat/FATFileEntry.hpp"

class FATFileSystem;

class FATFile : public File
{
public:
    FATFile();
    bool Open(FATFileSystem* fs, uint32_t firstCluster, const char* name, uint32_t size, bool isDirectory);
    bool OpenRootDirectory1216(FATFileSystem* fs, uint32_t rootDirLba, uint32_t rootDirSize);

    virtual void Release() override;

    bool IsOpened() const { return m_Opened; }
    bool ReadFileEntry(FATDirectoryEntry* dirEntry);

    virtual size_t Read(uint8_t* data, size_t size) override;
    virtual size_t Write(const uint8_t* data, size_t size) override;
    virtual bool Seek(SeekPos pos, int rel) override;
    virtual size_t Size() override { return m_Size; }
    virtual size_t Position() override { return m_Position; }
    
    virtual FileEntry* ReadFileEntry() override;
    

private:
    bool UpdateCurrentCluster();

    FATFileSystem* m_FS;
    uint8_t m_Buffer[SectorSize];
    bool m_Opened;
    bool m_IsRootDirectory;
    bool m_IsDirectory;
    uint32_t m_Position;
    uint32_t m_Size;
    uint32_t m_FirstCluster;
    uint32_t m_CurrentCluster;
    uint32_t m_CurrentClusterIndex;
    uint32_t m_CurrentSectorInCluster;
};