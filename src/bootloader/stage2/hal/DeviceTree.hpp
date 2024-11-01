#pragma once
#include <hal/fs/MemoryFileSystem.hpp>
#include <hal/fs/BlockDeviceFile.hpp>
#include <hal/fs/CharacterDeviceFile.hpp>
#include <hal/dev/BlockDevice.hpp>
#include <hal/dev/CharacterDevice.hpp>
#include "DeviceFSNode.hpp"

#define DEVICE_TREE_FILE_POOL_CAPACITY 10

namespace hal
{

class DeviceTree : public fs::MemoryFileSystem
{
public:
    DeviceTree();
    fs::FSNode*  RootDirectory(ErrorChain& err) override { return &m_Root; }
    fs::FSNode *  AttachDevice(dev::BlockDevice* device, etl::string_view kind, ErrorChain& err);
    fs::FSNode *  AttachDevice(dev::CharacterDevice* device, etl::string_view kind, ErrorChain& err);

    void DestroyFSNode(fs::MemoryFSNode* entry) override;

private:
    fs::File*  OpenDevice(DeviceFSNode* node,dev::BlockDevice* device, fs::FileOpenMode mode, ErrorChain& err);
    fs::File*  OpenDevice(DeviceFSNode* node,dev::CharacterDevice* device, fs::FileOpenMode mode, ErrorChain& err);
    DeviceFSNode*  CreateFSNode(dev::BlockDevice* device, const etl::string_view& name, ErrorChain& err);
    DeviceFSNode*  CreateFSNode(dev::CharacterDevice* device, const etl::string_view& name, ErrorChain& err);

    fs::FSNode *  AttachDeviceInternal(DeviceFSNode* node, etl::string_view kind, ErrorChain& err);


    DeviceFSNode m_Root;
    etl::vector<fs::BlockDeviceFile, DEVICE_TREE_FILE_POOL_CAPACITY> m_BlockDevPool;
    etl::vector<fs::CharacterDeviceFile, DEVICE_TREE_FILE_POOL_CAPACITY> m_CharDevPool;
    etl::vector<DeviceFSNode, DEVICE_TREE_FILE_POOL_CAPACITY> m_NodePool;
    friend class DeviceFSNode;
};

}