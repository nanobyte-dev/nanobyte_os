#include <hal/dev/BlockDevice.hpp>
#include "BlockDeviceFile.hpp"
#include <core/cpp/Algorithm.hpp>

namespace hal::fs
{

BlockDeviceFile::BlockDeviceFile()
    : m_Device(nullptr), m_Pos(0)
{
}

void BlockDeviceFile::Initialize(FSNode* node, hal::dev::BlockDevice *device, FileOpenMode mode, ErrorChain& err)
{
    if (err.Failed()) return;

    if (device == nullptr)
    {
        err.Fail(ResultCode::ArgumentNull, "Device is null");
        return;
    }
    m_Device = device;

    File::Initialize(node, mode, err);
}

size_t BlockDeviceFile::Read(uint8_t *data, size_t size, ErrorChain& err)
{
    if (err.Failed()) return 0;

    if (m_Device == nullptr)
    {
        err.Fail(ResultCode::NotInitialized, "Not initialized");
        return 0;
    }

    size_t count = m_Device->Read(m_Pos, data, size, err);
    m_Pos += count;
    return count;
}

size_t BlockDeviceFile::Write(const uint8_t *data, size_t size, ErrorChain& err)
{
    if (err.Failed()) return 0;

    if (m_Device == nullptr)
    {
        err.Fail(ResultCode::NotInitialized, "Not initialized");
        return 0;
    }

    size_t count = m_Device->Write(m_Pos, data, size, err);
    m_Pos += count;
    return count;
}

void BlockDeviceFile::Seek(SeekPos pos, int rel, ErrorChain& err)
{
    if (err.Failed()) return;

    if (m_Device == nullptr)
    {
        err.Fail(ResultCode::NotInitialized, "Not initialized");
        return;
    }

    switch (pos)
    {
        case SeekPos::Set:
            m_Pos = Min<size_t>(rel, m_Device->Size());
            break;

        case SeekPos::Current:
            // prevent underflow
            if (-rel > m_Pos)
                m_Pos = 0;
            else
                m_Pos = Min<size_t>(m_Pos + rel, m_Device->Size());
            break;;

        case SeekPos::End:
            m_Pos = m_Device->Size();
            break;

        default:
            err.Fail(ResultCode::InvalidArgument, "Invalid seek position");
            break;
    }
}

size_t BlockDeviceFile::Size()
{
    return m_Device->Size();
}

size_t BlockDeviceFile::Position()
{
    return m_Pos;
}

}