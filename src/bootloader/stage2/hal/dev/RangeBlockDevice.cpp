#include "RangeBlockDevice.hpp"
#include <core/cpp/Algorithm.hpp>

namespace hal::dev
{

RangeBlockDevice::RangeBlockDevice()
    : m_Device(nullptr),
      m_RangeBegin(0),
      m_RangeSize(0)
{
}

void RangeBlockDevice::Initialize(BlockDevice* device, size_t rangeBegin, size_t rangeSize, ErrorChain& err)
{
    if (err.Failed())
        return;

    m_Device = device;
    m_RangeBegin = rangeBegin;
    m_RangeSize = rangeSize;
}

size_t RangeBlockDevice::Read(size_t position, uint8_t *data, size_t size, ErrorChain& err)
{
    if (err.Failed())
        return 0;

    if (m_Device == nullptr)
    {
        err.Fail(ResultCode::NotInitialized, "Device not initialized");
        return 0;
    }

    if (position > m_RangeSize)
        return 0;

    size = Min(size, Size() - position);
    return m_Device->Read(m_RangeBegin + position, data, size, err);
}

size_t RangeBlockDevice::Write(size_t position, const uint8_t *data, size_t size, ErrorChain& err)
{
    if (err.Failed())
        return 0;

    if (m_Device == nullptr)
    {
        err.Fail(ResultCode::NotInitialized, "Device not initialized");
        return 0;
    }

    if (position > m_RangeSize)
        return 0;

    size = Min(size, Size() - position);
    return m_Device->Write(m_RangeBegin + position, data, size, err);
}

size_t RangeBlockDevice::Size()
{
    return m_RangeSize;
}

}