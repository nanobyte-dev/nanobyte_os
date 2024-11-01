#pragma once

#include "FSNode.hpp"
#include <etl/string.h>

namespace hal::fs
{

class MemoryFileSystem;

class MemoryFSNode : public FSNode
{
public:
    MemoryFSNode(MemoryFileSystem* fs, MemoryFSNode* parent, etl::string_view name, FileType type);

    etl::string_view Name() override;
    const FileType Type() override;

    // File operations
    void Delete(ErrorChain& err) override;
    void Rename(const etl::string_view& newName, ErrorChain& err) override;
    void Move(FSNode* newParent, ErrorChain& err) override;

    // Navigation
    FSNode* FindChild(const etl::string_view& childName, ErrorChain& err) override;
    FSNode* FirstChild(ErrorChain& err) override;
    FSNode* NextSibling(ErrorChain& err) override;
    FSNode* Parent(ErrorChain& err) override;

    // Child manipulation
    void UnlinkChild(FSNode* child, ErrorChain& err) override;
    void AddChild(FSNode* child, ErrorChain& err) override;

private:
    void AddChildInternal(MemoryFSNode* child);
    void RemoveChildInternal(MemoryFSNode* child);

    MemoryFileSystem* m_MemoryFS;
    MemoryFSNode* m_Parent;
    MemoryFSNode* m_Next;
    MemoryFSNode* m_Prev;
    MemoryFSNode* m_FirstChild;
    MemoryFSNode* m_LastChild;
    etl::string<256> m_Name;
    FileType m_Type;

    friend class MemoryFileSystem;
};

}