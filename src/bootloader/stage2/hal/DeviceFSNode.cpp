#include "DeviceFSNode.hpp"
#include "DeviceTree.hpp"

namespace hal
{
    
DeviceFSNode::DeviceFSNode(DeviceTree *fs, MemoryFSNode *parent, etl::string_view name, fs::FileType type)
    : fs::MemoryFSNode(fs, parent, name, type), m_DevTree(fs), m_BlockDevice(nullptr), m_CharacterDevice(nullptr)
{
}

DeviceFSNode::DeviceFSNode(DeviceTree *fs, MemoryFSNode *parent, etl::string_view name, dev::BlockDevice *device)
    : fs::MemoryFSNode(fs, parent, name, fs::FileType::File), m_DevTree(fs), m_BlockDevice(device), m_CharacterDevice(nullptr)

{
}

DeviceFSNode::DeviceFSNode(DeviceTree *fs, MemoryFSNode *parent, etl::string_view name, dev::CharacterDevice *device)
    : fs::MemoryFSNode(fs, parent, name, fs::FileType::File), m_DevTree(fs), m_BlockDevice(nullptr), m_CharacterDevice(device)
{
}

fs::File* DeviceFSNode::Open(fs::FileOpenMode mode, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    if (Type() != fs::FileType::File)
    {
        err.Fail(ResultCode::InvalidOperation, "Cannot open directories.");
        return nullptr;
    }

    if (m_BlockDevice == nullptr && m_CharacterDevice == nullptr)
    {
        err.Fail(ResultCode::NotInitialized, "Node not properly initialized.");
        return nullptr;
    }

    return (m_BlockDevice != nullptr)
        ? m_DevTree->OpenDevice(this, m_BlockDevice, mode, err)
        : m_DevTree->OpenDevice(this, m_CharacterDevice, mode, err);
}

} // namespace hal