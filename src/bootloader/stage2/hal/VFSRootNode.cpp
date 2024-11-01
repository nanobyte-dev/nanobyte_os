#include "VFSRootNode.hpp"
#include "VirtualFileSystem.hpp"

namespace hal {
    
VFSRootNode::VFSRootNode(VirtualFileSystem *fs)
    : fs::FSNode(fs), m_VFS(fs)
{
}

fs::File *VFSRootNode::Open(fs::FileOpenMode mode, ErrorChain &err)
{
    if (err.Failed()) return nullptr;
    err.Fail(ResultCode::InvalidOperation, "Cannot open root");
    return nullptr;
}

void VFSRootNode::Delete(ErrorChain &err)
{
    if (err.Failed()) return;
    err.Fail(ResultCode::InvalidOperation, "Cannot delete root");
}

void VFSRootNode::Rename(const etl::string_view &newName, ErrorChain &err)
{
    if (err.Failed()) return;
    err.Fail(ResultCode::InvalidOperation, "Cannot rename root");
}

void VFSRootNode::Move(FSNode *newParent, ErrorChain &err)
{
    if (err.Failed()) return;
    err.Fail(ResultCode::InvalidOperation, "Cannot move root");
}

fs::FSNode *VFSRootNode::FirstChild(ErrorChain &err)
{
    if (err.Failed()) return nullptr;
    return m_VFS->m_MountPoints.begin()->second->RootDirectory(err);
}

fs::FSNode *VFSRootNode::FindChild(const etl::string_view &childName, ErrorChain &err)
{
    if (err.Failed()) return nullptr;
    
    auto it = m_VFS->m_MountPoints.find(childName);
    if (it == m_VFS->m_MountPoints.end())
    {
        err.Fail(ResultCode::NotFound, "File not found");
        return nullptr;
    }

    return it->second->RootDirectory(err);
}

fs::FSNode *VFSRootNode::LocateChild(const etl::string_view &path, ErrorChain &err)
{
    // todo
    return nullptr;
}

void VFSRootNode::UnlinkChild(FSNode *child, ErrorChain &err)
{
    if (err.Failed()) return;
    err.Fail(ResultCode::InvalidOperation, "Use unmount");
}

void VFSRootNode::AddChild(FSNode *child, ErrorChain &err)
{
    if (err.Failed()) return;
    err.Fail(ResultCode::InvalidOperation, "Use mount");
}

}