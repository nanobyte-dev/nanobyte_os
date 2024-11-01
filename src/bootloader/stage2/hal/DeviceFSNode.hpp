#pragma once

#include <hal/fs/MemoryFSNode.hpp>
#include <hal/dev/BlockDevice.hpp>
#include <hal/dev/CharacterDevice.hpp>

namespace hal
{

class DeviceTree;

class DeviceFSNode : public fs::MemoryFSNode
{
public:
    DeviceFSNode(DeviceTree* fs, MemoryFSNode* parent, etl::string_view name, fs::FileType type);
    DeviceFSNode(DeviceTree* fs, MemoryFSNode* parent, etl::string_view name, dev::BlockDevice* device);
    DeviceFSNode(DeviceTree* fs, MemoryFSNode* parent, etl::string_view name, dev::CharacterDevice* device);

    fs::File* Open(fs::FileOpenMode mode, ErrorChain& err) override;

private:
    DeviceTree* m_DevTree;
    dev::BlockDevice* m_BlockDevice;
    dev::CharacterDevice* m_CharacterDevice;

};


}