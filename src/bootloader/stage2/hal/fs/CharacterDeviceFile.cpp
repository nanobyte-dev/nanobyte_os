#include <hal/dev/CharacterDevice.hpp>
#include "CharacterDeviceFile.hpp"

namespace hal::fs
{

CharacterDeviceFile::CharacterDeviceFile()
    : m_Device(nullptr)
{
}

void CharacterDeviceFile::Initialize(FSNode* node, hal::dev::CharacterDevice* device, FileOpenMode mode, ErrorChain& err)
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

size_t CharacterDeviceFile::Read(uint8_t *data, size_t size, ErrorChain& err)
{
    if (err.Failed()) return 0;

    if (m_Device == nullptr)
    {
        err.Fail(ResultCode::NotInitialized, "Not initialized");
        return 0;
    }

    return m_Device->Read(data, size, err);
}

size_t CharacterDeviceFile::Write(const uint8_t *data, size_t size, ErrorChain& err)
{
    if (err.Failed()) return 0;

    if (m_Device == nullptr)
    {
        err.Fail(ResultCode::NotInitialized, "Not initialized");
        return 0;
    }

    return m_Device->Write(data, size, err);
}

void CharacterDeviceFile::Seek(SeekPos pos, int rel, ErrorChain& err)
{
    if (err.Failed()) return;
    err.Fail(ResultCode::NotSupported, "Not supported");
}

size_t CharacterDeviceFile::Size()
{
    return 0;
}

size_t CharacterDeviceFile::Position()
{
    return 0;
}

}