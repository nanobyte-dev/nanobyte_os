#include "DeviceTree.hpp"
#include <etl/to_string.h>

namespace hal
{

DeviceTree::DeviceTree()
    : m_Root(this, nullptr, "", fs::FileType::Directory)
{
}

fs::FSNode* DeviceTree::AttachDevice(dev::BlockDevice *device, etl::string_view kind, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    auto node = CreateFSNode(device, "", err);
    err.FailOnError(ResultCode::IOFailed, "Failed to attach device");
    if (err.Failed()) return nullptr;
    
    AttachDeviceInternal(node, kind, err);
    if (err.Failed())
    {
        DestroyFSNode(node);
        return nullptr;
    }

    return node;
}

fs::FSNode* DeviceTree::AttachDevice(dev::CharacterDevice *device, etl::string_view kind, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    auto node = CreateFSNode(device, "", err);
    err.FailOnError(ResultCode::IOFailed, "Failed to attach device");
    if (err.Failed()) return nullptr;

    AttachDeviceInternal(node, kind, err);
    if (err.Failed())
    {
        DestroyFSNode(node);
        return nullptr;
    }

    return node;
}

fs::FSNode* DeviceTree::AttachDeviceInternal(DeviceFSNode *node, etl::string_view kind, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    // Create device
    int index = 0;
    etl::string<256> name;

    do
    {
        name.assign(kind.begin(), kind.end());
        etl::to_string(index, name, true);
        
        node->Rename(name, err);
        m_Root.AddChild(node, err);

        if (!err.Failed())
            break;
        if (err.LastError() == ResultCode::AlreadyExists)
            err.Clear();
        err.FailOnError(ResultCode::IOFailed, "Failed to attach device");

        index += 1;
    } while (!err.Failed());

    return node;
}

fs::File* DeviceTree::OpenDevice(DeviceFSNode* node, dev::BlockDevice *device, fs::FileOpenMode mode, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    if (m_BlockDevPool.size() == m_BlockDevPool.max_size())
    {
        err.Fail(ResultCode::ContainerFull, "Block device file pool full");
        return nullptr;
    }

    auto file = &m_BlockDevPool.emplace_back();
    file->Initialize(node, device, mode, err);
    if (err.Failed())
    {
        m_BlockDevPool.erase(file);
        return nullptr;
    }
    
    return file;
}

fs::File* DeviceTree::OpenDevice(DeviceFSNode* node, dev::CharacterDevice *device, fs::FileOpenMode mode, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    if (m_CharDevPool.size() == m_CharDevPool.max_size())
    {
        err.Fail(ResultCode::ContainerFull, "Character device file pool full");
        return nullptr;
    }

    auto file = &m_CharDevPool.emplace_back();
    file->Initialize(node, device, mode, err);
    if (err.Failed())
    {
        m_CharDevPool.erase(file);
        return nullptr;
    }
    
    return file;
}

DeviceFSNode*  DeviceTree::CreateFSNode(dev::BlockDevice *device, const etl::string_view &name, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    if (m_NodePool.size() == m_NodePool.max_size())
    {
        err.Fail(ResultCode::ContainerFull, "Device node pool full");
        return nullptr;
    }

    return &m_NodePool.emplace_back(this, &m_Root, name, device);
}

DeviceFSNode *  DeviceTree::CreateFSNode(dev::CharacterDevice *device, const etl::string_view &name, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    if (m_NodePool.size() == m_NodePool.max_size())
    {
        err.Fail(ResultCode::ContainerFull, "Device node pool full");
        return nullptr;
    }
    return &m_NodePool.emplace_back(this, &m_Root, name, device);
}

void DeviceTree::DestroyFSNode(fs::MemoryFSNode *entry)
{
    m_NodePool.erase((DeviceFSNode*)entry);
}

}