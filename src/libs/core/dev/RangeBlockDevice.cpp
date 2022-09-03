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

void RangeBlockDevice::Seek(SeekPos pos, int rel)
{
    if (m_Device == nullptr)
        return;
        
    switch (pos)
    {
    case SeekPos::Set:
        m_Device->Seek(SeekPos::Set, m_RangeBegin + rel);
        break;
    
    case SeekPos::Current:
        m_Device->Seek(SeekPos::Current, rel);
        break;

    case SeekPos::End:
        m_Device->Seek(SeekPos::Set, m_RangeBegin + m_RangeSize);
        break;
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