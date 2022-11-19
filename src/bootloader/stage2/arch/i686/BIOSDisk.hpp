#include <core/dev/BlockDevice.hpp>


class BIOSDisk : public BlockDevice
{
public:
    BIOSDisk(uint8_t id);
    bool Initialize();
    virtual size_t Read(uint8_t* data, size_t size) override;
    virtual size_t Write(const uint8_t* data, size_t size) override;
    virtual bool Seek(SeekPos pos, int rel) override;
    virtual size_t Size() override;
    virtual size_t Position() override;

private:
    bool ReadNextSector();
    void LBA2CHS(uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut);

    static inline const constexpr int SectorSize = 512;

    uint8_t m_Id;
    bool m_HaveExtensions;
    uint16_t m_Cylinders;
    uint16_t m_Sectors;
    uint16_t m_Heads;

    uint8_t m_Buffer[SectorSize];
    uint64_t m_Pos;
    uint64_t m_Size;
};