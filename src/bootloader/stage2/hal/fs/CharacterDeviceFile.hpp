#pragma once

#include "File.hpp"
#include <hal/dev/CharacterDevice.hpp>

namespace hal::fs
{

class CharacterDeviceFile : public File
{
public:
    CharacterDeviceFile();
    void Initialize(FSNode* node, hal::dev::CharacterDevice* device, FileOpenMode mode, ErrorChain& err);
    size_t Read(uint8_t* data, size_t size, ErrorChain& err) override;
    size_t Write(const uint8_t* data, size_t size, ErrorChain& err) override;
    void Seek(SeekPos pos, int rel, ErrorChain& err) override;
    size_t Size() override;
    size_t Position() override;

private:
    hal::dev::CharacterDevice* m_Device;
};

}