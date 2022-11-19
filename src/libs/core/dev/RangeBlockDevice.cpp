#include "RangeBlockDevice.hpp"
#include <core/cpp/Algorithm.hpp>

RangeBlockDevice::RangeBlockDevice()
    : m_Device(nullptr),
      m_RangeBegin(0),
      m_RangeSize(0)
{
}

void RangeBlockDevice::Initialize(BlockDevice* device, size_t rangeBegin, size_t rangeSize)
{
    m_Device = device;
    m_RangeBegin = rangeBegin;
    m_RangeSize = rangeSize;
    m_Device->Seek(SeekPos::Set, rangeBegin);
}

size_t RangeBlockDevice::Read(uint8_t* data, size_t size)
{
    if (m_Device == nullptr)
        return 0;

    size = Min(size, Size() - Position());
    return m_Device->Read(data, size);
}

size_t RangeBlockDevice::Write(const uint8_t* data, size_t size)
{
    if (m_Device == nullptr)
        return 0;

    size = Min(size, Size() - Position());
    return m_Device->Write(data, size);
}

bool RangeBlockDevice::Seek(SeekPos pos, int rel)
{
    if (m_Device == nullptr)
        return false;
        
    switch (pos)
    {
    case SeekPos::Set:
        return m_Device->Seek(SeekPos::Set, m_RangeBegin + rel);
    
    case SeekPos::Current:
        return m_Device->Seek(SeekPos::Current, rel);

    case SeekPos::End:
        return m_Device->Seek(SeekPos::Set, m_RangeBegin + m_RangeSize);

    default:
        return false;
    }
}

size_t RangeBlockDevice::Size()
{
    return m_RangeSize;
}

size_t RangeBlockDevice::Position()
{
    return m_Device->Position() - m_RangeBegin;
}