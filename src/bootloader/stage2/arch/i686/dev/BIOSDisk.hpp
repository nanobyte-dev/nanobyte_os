#include <hal/dev/BlockDevice.hpp>

namespace arch::i686
{

class BIOSDisk : public hal::dev::BlockDevice
{
public:
    BIOSDisk();
    void Initialize(uint8_t id, ErrorChain& err);
    virtual size_t Read(size_t position, uint8_t* data, size_t size, ErrorChain& err) override;
    virtual size_t Write(size_t position, const uint8_t* data, size_t size, ErrorChain& err) override;
    virtual size_t Size() override;

private:
    void ReadSector(size_t lba, ErrorChain& err);
    void ReadSectorCHS(size_t lba, ErrorChain& err);
    void ReadSectorLBA(size_t lba, ErrorChain& err);

    void LBA2CHS(uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut);

    static inline const constexpr int SectorSize = 512;

    uint8_t m_Id;
    bool m_HaveExtensions;
    uint16_t m_Cylinders;
    uint16_t m_Sectors;
    uint16_t m_Heads;

    uint8_t m_Buffer[SectorSize];
    size_t m_CurrentLba;
    size_t m_Size;
};

}