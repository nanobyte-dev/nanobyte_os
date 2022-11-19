#include "BIOSDisk.hpp"
#include "RealMemory.hpp"
#include <core/Defs.hpp>
#include <core/Assert.hpp>
#include <core/Memory.hpp>
#include <core/cpp/Algorithm.hpp>

EXPORT bool ASMCALL i686_Disk_GetDriveParams(uint8_t drive,
                                             uint8_t* driveTypeOut,
                                             uint16_t* cylindersOut,
                                             uint16_t* sectorsOut,
                                             uint16_t* headsOut);

EXPORT bool ASMCALL i686_Disk_Reset(uint8_t drive);

EXPORT bool ASMCALL i686_Disk_Read(uint8_t drive,
                                   uint16_t cylinder,
                                   uint16_t sector,
                                   uint16_t head,
                                   uint8_t count,
                                   void* lowerDataOut);

EXPORT bool ASMCALL i686_Disk_ExtensionsPresent(uint8_t drive);

struct ExtendedReadParameters
{
    uint8_t ParamsSize;
    uint8_t Reserved;
    uint16_t Count;
    uint32_t Buffer;
    uint64_t LBA;
} __attribute__((packed));

EXPORT bool ASMCALL i686_Disk_ExtendedRead(uint8_t drive,
                                           ExtendedReadParameters* params);

struct ExtendedDriveParameters
{
    uint16_t ParamsSize;
    uint16_t Flags;
    uint32_t Cylinders;
    uint32_t Heads;
    uint32_t SectorsPerTrack;
    uint64_t Sectors;
    uint16_t BytesPerSector;
} __attribute__((packed));

EXPORT bool ASMCALL i686_Disk_ExtendedGetDriveParams(uint8_t drive,
                                                     ExtendedDriveParameters* params);

BIOSDisk::BIOSDisk(uint8_t id)
    : m_Id(id),
      m_Pos(-1),
      m_Size(0)
{
}

bool BIOSDisk::Initialize()
{
    m_HaveExtensions = i686_Disk_ExtensionsPresent(m_Id);

    if (m_HaveExtensions)
    {
        ExtendedDriveParameters params;
        params.ParamsSize = sizeof(ExtendedDriveParameters);
        if (!i686_Disk_ExtendedGetDriveParams(m_Id, &params))
            return false;

        Assert(params.BytesPerSector == SectorSize);
        m_Size = SectorSize * params.Sectors;
    }
    else
    {
        uint8_t driveType;
        if (!i686_Disk_GetDriveParams(m_Id, &driveType, &m_Cylinders, &m_Sectors, &m_Heads))
            return false;
    }

    return true;
}

size_t BIOSDisk::Read(uint8_t* data, size_t size)
{
    size_t initialPos = m_Pos;
    if (m_Pos == -1)
    {
        ReadNextSector();
        m_Pos = 0;
    }
    if (m_Pos >= m_Size)
    {
        return 0;
    }

    while (size > 0)
    {
        size_t bufferPos = m_Pos % SectorSize;
        size_t canRead = Min(size, SectorSize - bufferPos);
        Memory::Copy(data, m_Buffer + bufferPos, canRead);

        size -= canRead;
        data += canRead;
        m_Pos += canRead;

        if (size > 0)
        {
            ReadNextSector();
        }
    }

    return m_Pos - initialPos;
}

bool BIOSDisk::ReadNextSector()
{
    uint64_t lba = m_Pos / SectorSize;
    bool ok = false;

    if (m_HaveExtensions)
    {
        ExtendedReadParameters params;
        params.ParamsSize = sizeof(ExtendedDriveParameters);
        params.Reserved = 0;
        params.Count = 1;
        params.Buffer = ToSegOffset(m_Buffer);
        params.LBA = lba;

        for (int i = 0; i < 3 && !ok; i++) 
        {
            ok = i686_Disk_ExtendedRead(m_Id, &params);
            if (!ok)
                i686_Disk_Reset(m_Id);
        }
    }
    else
    {
        uint16_t cylinder, sector, head;
        LBA2CHS(lba, &cylinder, &sector, &head);

        for (int i = 0; i < 3 && !ok; i++) 
        {
            ok = i686_Disk_Read(m_Id, cylinder, sector, head, 1, m_Buffer);
            if (!ok)
                i686_Disk_Reset(m_Id);
        }
    }

    return ok;
}

size_t BIOSDisk::Write(const uint8_t* data, size_t size)
{
    return 0;
}

void BIOSDisk::LBA2CHS(uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut)
{
    // sector = (LBA % sectors per track + 1)
    *sectorOut = lba % m_Sectors + 1;

    // cylinder = (LBA / sectors per track) / heads
    *cylinderOut = (lba / m_Sectors) / m_Heads;

    // head = (LBA / sectors per track) % heads
    *headOut = (lba / m_Sectors) % m_Heads;

}

bool BIOSDisk::Seek(SeekPos pos, int rel)
{
    switch (pos)
    {
        case SeekPos::Set:
            m_Pos = -1;
            return true;

        case SeekPos::Current:
            m_Pos += rel;
            ReadNextSector();
            return true;

        case SeekPos::End:
            m_Pos = m_Size;
            return true;
    }

    return false;
}

size_t BIOSDisk::Size()
{
    return m_Size;
}

size_t BIOSDisk::Position()
{
    return m_Pos;
}
