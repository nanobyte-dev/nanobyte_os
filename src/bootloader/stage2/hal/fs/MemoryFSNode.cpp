#include "MemoryFSNode.hpp"
#include "MemoryFileSystem.hpp"
#include <core/Assert.hpp>

namespace hal::fs
{

MemoryFSNode::MemoryFSNode(MemoryFileSystem *fs, MemoryFSNode* parent, etl::string_view name, FileType type)
    : FSNode(fs), m_MemoryFS(fs), m_Parent(parent), m_Name(name), m_Type(type)
{
}

etl::string_view MemoryFSNode::Name()
{
    return m_Name;
}

const FileType MemoryFSNode::Type()
{
    return m_Type;
}

void MemoryFSNode::Delete(ErrorChain& err)
{
    if (err.Failed()) return;
    Assert(m_MemoryFS != nullptr);

    // unparent self
    if (m_Parent)
    {
        m_Parent->UnlinkChild(this, err);
        err.FailOnError(ResultCode::IOFailed, "Failed to delete (unlink failed)");
        if (err.Failed()) return;
    }

    // destroy self
    m_MemoryFS->DestroyFSNode(this);
}

void MemoryFSNode::Rename(const etl::string_view &newName, ErrorChain& err)
{
    if (err.Failed()) return;

    // Invalid name
    if (newName.empty())
    {   
        err.Fail(ResultCode::InvalidArgument, "Name empty");
        return;
    }
    if (newName.size() > m_Name.max_size())
    {   
        err.Fail(ResultCode::ContainerFull, "Name too long");
        return;
    }

    // Ensure name is unique
    if (m_Parent != nullptr)
    {
        FSNode* otherChild = m_Parent->FindChild(newName, err);
        if (err.Failed() && err.LastError() != ResultCode::NotFound)
        {
            err.Fail(ResultCode::IOFailed, "Rename failed");
            return;
        }
        err.Clear();

        if (otherChild != nullptr)
        {
            err.Fail(ResultCode::AlreadyExists, "Name conflict");
            return;
        }
    }

    // Rename
    m_Name.assign(newName.begin(), newName.end());
}

void MemoryFSNode::Move(FSNode *newParent, ErrorChain& err)
{
    if (err.Failed()) return;

    return newParent->AddChild(this, err);
}

FSNode* MemoryFSNode::FindChild(const etl::string_view &childName, ErrorChain& err)
{
    if (err.Failed()) return nullptr;
    Assert(m_MemoryFS != nullptr);

    for (auto child = m_FirstChild; child != nullptr; child = child->m_Next)
    {
        if (child->m_Name == childName)
            return child;
    }

    err.Fail(ResultCode::NotFound, "File not found");
    return nullptr;
}

FSNode* MemoryFSNode::FirstChild(ErrorChain& err)
{
    if (err.Failed()) return nullptr;
    return m_FirstChild;
}

FSNode* MemoryFSNode::NextSibling(ErrorChain& err)
{
    if (err.Failed()) return nullptr;
    return m_Next;
}

FSNode* MemoryFSNode::Parent(ErrorChain& err)
{
    if (err.Failed()) return nullptr;
    return m_Parent;
}

void MemoryFSNode::UnlinkChild(FSNode *child, ErrorChain& err)
{
    if (err.Failed()) return;

    if (child == nullptr)
    {
        err.Fail(ResultCode::ArgumentNull, "Child is null");
        return;
    }
    if (child->FS() != m_FS)
    {
        err.Fail(ResultCode::InvalidArgument, "Child is from another filesystem");
        return;
    }

    MemoryFSNode* mfsChild = (MemoryFSNode*)child;
    if (mfsChild->m_Parent != this)
    {
        err.Fail(ResultCode::InvalidArgument, "Not a child");
        return;
    }

    RemoveChildInternal(mfsChild);
}

void MemoryFSNode::AddChild(FSNode *child, ErrorChain& err)
{
    if (err.Failed()) return;

    if (child == nullptr)
    {
        err.Fail(ResultCode::ArgumentNull, "Child is null");
        return;
    }
    if (m_Type != FileType::Directory)
    {
        err.Fail(ResultCode::InvalidOperation, "Not a directory");
        return;
    }
    if (child->FS() != m_FS)
    {
        err.Fail(ResultCode::InvalidArgument, "Child is from another filesystem");
        return;
    }

    // Ensure name is unique
    FSNode* otherChild = FindChild(child->Name(), err);
    if (err.Failed() && err.LastError() != ResultCode::NotFound)
    {
        err.Fail(ResultCode::IOFailed, "Add failed");
        return;
    }
    err.Clear();

    if (otherChild != nullptr)
    {
        err.Fail(ResultCode::AlreadyExists, "Name conflict");
        return;
    }
    
    AddChildInternal((MemoryFSNode*)child);
}

void MemoryFSNode::AddChildInternal(MemoryFSNode *child)
{
    child->m_Parent = this;
    child->m_Next = nullptr;
    child->m_Prev = m_LastChild;

    // first child
    if (m_FirstChild == nullptr)
    {
        m_FirstChild = child;
        m_LastChild = child;
    }
    else
    {
        m_LastChild->m_Next = child;
        m_LastChild = child;
    }
}

void MemoryFSNode::RemoveChildInternal(MemoryFSNode *child)
{
    if (child == m_FirstChild)
        m_FirstChild = child->m_Next;
    if (child == m_LastChild)
        m_LastChild = child->m_Prev;

    if (child->m_Prev != nullptr)
        child->m_Prev->m_Next = child->m_Next;
    child->m_Prev = nullptr;

    if (child->m_Next != nullptr)
        child->m_Next->m_Prev = child->m_Prev;
    child->m_Next = nullptr;

    child->m_Parent = nullptr;
}

}