#include "BIOSDisk.hpp"
#include <arch/i686/RealMemory.hpp>
#include <core/Defs.hpp>
#include <core/Assert.hpp>
#include <core/Memory.hpp>
#include <core/cpp/Algorithm.hpp>
#include <core/Debug.hpp>

#define MODULE "BIOSDisk"
#define UNDEFINED ((size_t)-1)

enum DiskResultCode
{
    Success                         = 0x00,
    InvalidFunctionOrParameter      = 0x01,
    AddressMarkNotFound             = 0x02,
    DiskWriteProtected              = 0x03,
    SectorNotFoundReadError         = 0x04,
    ResetFailed                     = 0x05,
    DataDidNotVerifyCorrectly       = 0x05,
    DiskChanged                     = 0x06,
    DriveParameterActivityFailed    = 0x07,
    DMAOverrun                      = 0x08,
    DataBoundaryError               = 0x09,
    BadSectorDetected               = 0x0A,
    BadTrackDetected                = 0x0B,
    UnsupportedTrackOrInvalidMedia  = 0x0C,
    InvalidNumberOfSectorsOnFormat  = 0x0D,
    ControlDataAddressMarkDetected  = 0x0E,
    DMAArbitrationLevelOutOfRange   = 0x0F,
    UncorrectableCRCErorrOnRead     = 0x10,
    DataECCEorrected                = 0x11,
    ControllerFailure               = 0x20,
    NoMediaInDrive                  = 0x31,
    IncorrectDriveTypeStoredInCMOS  = 0x32,
    SeekFailed                      = 0x40,
    Timeout                         = 0x80,
    DriveNotReady                   = 0xAA,
    VolumeNotLockedInDrive          = 0xB0,
    VolumeLockedInDrive             = 0xB1,
    VolumeNotRemovable              = 0xB2,
    VolumeInUse                     = 0xB3,
    LockCountExceeded               = 0xB4,
    ValidEjectRequestFailed         = 0xB5,
    VolumePresentButReadProtected   = 0xB6,
    UndefinedError                  = 0xBB,
    WriteFault                      = 0xCC,
    StatusRegisterError             = 0xE0,
    SenseOperationFailed            = 0xFF,
};


EXPORT DiskResultCode ASMCALL i686_Disk_GetDriveParams(uint8_t drive,
                                                       uint8_t* outDriveType,
                                                       uint16_t* outCylinders,
                                                       uint16_t* outSectors,
                                                       uint16_t* outHeads);

EXPORT DiskResultCode ASMCALL i686_Disk_Reset(uint8_t drive);

EXPORT DiskResultCode ASMCALL i686_Disk_Read(uint8_t drive,
                                             uint16_t cylinder,
                                             uint16_t sector,
                                             uint16_t head,
                                             uint8_t count,
                                             void* lowerDataPtr);

EXPORT bool ASMCALL i686_Disk_ExtensionsPresent(uint8_t drive);

struct ExtendedReadParameters
{
    uint8_t ParamsSize;
    uint8_t Reserved;
    uint16_t Count;
    uint32_t Buffer;
    uint64_t LBA;
} __attribute__((packed));

EXPORT DiskResultCode ASMCALL i686_Disk_ExtendedRead(uint8_t drive,
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

EXPORT DiskResultCode ASMCALL i686_Disk_ExtendedGetDriveParams(uint8_t drive,
                                                               ExtendedDriveParameters* params);



namespace arch::i686
{

BIOSDisk::BIOSDisk()
    : m_Id(-1),
      m_CurrentLba(UNDEFINED),
      m_Size(0)
{
}

void BIOSDisk::Initialize(uint8_t id, ErrorChain& err)
{
    if (err.Failed())
        return;

    m_Id = id;
    m_HaveExtensions = i686_Disk_ExtensionsPresent(m_Id);

    if (m_HaveExtensions)
    {
        ExtendedDriveParameters params;
        params.ParamsSize = sizeof(ExtendedDriveParameters);
        DiskResultCode result = i686_Disk_ExtendedGetDriveParams(m_Id, &params);
        if (result != DiskResultCode::Success)
        {
            Debug::Debug(MODULE, "Failed to get extended drive parameters (error code: %d)", result);
            err.Fail(ResultCode::IOFailed, "Failed to get extended drive parameters");
            return;
        }

        Assert(params.BytesPerSector == SectorSize);
        m_Size = SectorSize * params.Sectors;
    }
    else
    {
        uint8_t driveType;
        DiskResultCode result = i686_Disk_GetDriveParams(m_Id, &driveType, &m_Cylinders, &m_Sectors, &m_Heads);
        if (result != DiskResultCode::Success)
        {
            Debug::Debug(MODULE, "Failed to get drive parameters (error code: %d)", result);
            err.Fail(ResultCode::IOFailed, "Failed to get extended drive parameters");
            return;
        }
        m_Size = SectorSize * m_Cylinders * m_Sectors * m_Heads;
    }
}

size_t BIOSDisk::Read(size_t position, uint8_t* data, size_t size, ErrorChain& err)
{
    if (err.Failed())
        return 0;

    size_t initialPos = position;

    if (position >= m_Size)
        return 0;
    if (position + size >= m_Size)
        size = m_Size - position;

    while (size > 0)
    {
        size_t lba = position / SectorSize;
        ReadSector(lba, err); 
        if (err.Failed()) return 0;

        size_t bufferPos = position % SectorSize;
        size_t canRead = Min(size, SectorSize - bufferPos);
        Memory::Copy(data, m_Buffer + bufferPos, canRead);

        size -= canRead;
        data += canRead;
        position += canRead;
    }

    return position - initialPos;
}

void BIOSDisk::ReadSector(size_t lba, ErrorChain& err)
{
    if (err.Failed())
        return;

    if (lba == m_CurrentLba)
        return;
    
    m_HaveExtensions ? ReadSectorLBA(lba, err) : ReadSectorCHS(lba, err);
    if (!err.Failed())
        m_CurrentLba = lba;
}

void BIOSDisk::ReadSectorCHS(size_t lba, ErrorChain& err)
{
    if (err.Failed())
        return;

    DiskResultCode dr;
    uint16_t cylinder, sector, head;
    LBA2CHS(lba, &cylinder, &sector, &head);

    for (int i = 0; i < 3; i++) 
    {
        if ((dr = i686_Disk_Read(m_Id, cylinder, sector, head, 1, m_Buffer)) == DiskResultCode::Success)
            return;

        Debug::Debug(MODULE, "Attempt %d to read sector %ld failed (error code: %d)", i, lba, dr);
        
        if ((dr = i686_Disk_Reset(m_Id)) != DiskResultCode::Success)
        {
            Debug::Debug(MODULE, "Failed to reset disk controller (error code: %d)", dr);
            err.Fail(ResultCode::IOFailed, "Failed to reset disk controller");
            return;
        }
    }

    err.Fail(ResultCode::IOFailed, "All read attempts failed");
}

void BIOSDisk::ReadSectorLBA(size_t lba, ErrorChain& err)
{
    if (err.Failed())
        return;

    DiskResultCode dr;

    ExtendedReadParameters params;
    params.ParamsSize = sizeof(ExtendedDriveParameters);
    params.Reserved = 0;
    params.Count = 1;
    params.Buffer = ToSegOffset(m_Buffer);
    params.LBA = lba;

    for (int i = 0; i < 3; i++) 
    {
        if ((dr = i686_Disk_ExtendedRead(m_Id, &params)) == DiskResultCode::Success)
            return;

        Debug::Debug(MODULE, "Attempt %d to read sector %ld failed (error code: %d)", i, lba, dr);

        if ((dr = i686_Disk_Reset(m_Id)) != DiskResultCode::Success)
        {
            Debug::Debug(MODULE, "Failed to reset disk controller (error code: %d)", dr);
            err.Fail(ResultCode::IOFailed, "Failed to reset disk controller");
            return;
        }
    }

    err.Fail(ResultCode::IOFailed, "All read attempts failed");
}

size_t BIOSDisk::Write(size_t position, const uint8_t* data, size_t size, ErrorChain& err)
{
    if (err.Failed()) return 0;

    err.Fail(ResultCode::NotSupported, "Not supported");
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

size_t BIOSDisk::Size()
{
    return m_Size;
}

}