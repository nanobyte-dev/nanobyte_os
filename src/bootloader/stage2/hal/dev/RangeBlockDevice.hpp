#pragma once

#include "BlockDevice.hpp"

namespace hal::dev
{

class RangeBlockDevice : public BlockDevice
{
public:
    RangeBlockDevice();
    void Initialize(BlockDevice* device, size_t rangeBegin, size_t rangeSize, ErrorChain& err);
    virtual size_t Read(size_t position, uint8_t* data, size_t size, ErrorChain& err) override;
    virtual size_t Write(size_t position, const uint8_t* data, size_t size, ErrorChain& err) override;
    virtual size_t Size() override;

private:
    BlockDevice* m_Device;
    size_t m_RangeBegin;
    size_t m_RangeSize;
};

}