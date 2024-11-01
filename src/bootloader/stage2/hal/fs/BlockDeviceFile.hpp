#pragma once

#include "File.hpp"
#include <hal/dev/BlockDevice.hpp>

namespace hal::fs
{

class BlockDeviceFile : public File
{
public:
    BlockDeviceFile();
    void Initialize(FSNode* node, hal::dev::BlockDevice* device, FileOpenMode mode, ErrorChain& err);
    size_t Read(uint8_t* data, size_t size, ErrorChain& err) override;
    size_t Write(const uint8_t* data, size_t size, ErrorChain& err) override;
    void Seek(SeekPos pos, int rel, ErrorChain& err) override;
    size_t Size() override;
    size_t Position() override;

private:
    hal::dev::BlockDevice* m_Device;
    size_t m_Pos;
};

}