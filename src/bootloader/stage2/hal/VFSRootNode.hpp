#pragma once

#include <hal/fs/FSNode.hpp>

namespace hal {

class VirtualFileSystem;

class VFSRootNode : public fs::FSNode
{
public:
    VFSRootNode(VirtualFileSystem* fs);
    etl::string_view Name() override { return "";}
    const fs::FileType Type() override { return fs::FileType::Directory; }

    // File operations
    fs::File* Open(fs::FileOpenMode mode, ErrorChain& err) override;
    void Delete(ErrorChain& err) override;
    void Rename(const etl::string_view& newName, ErrorChain& err) override;
    void Move(FSNode* newParent, ErrorChain& err) override;

    // Navigation
    FSNode* FirstChild(ErrorChain& err) override;
    FSNode* NextSibling(ErrorChain& err) override { return nullptr; }
    FSNode* Parent(ErrorChain& err) override { return nullptr; }
    FSNode* FindChild(const etl::string_view& childName, ErrorChain& err) override;

    FSNode* LocateChild(const etl::string_view& path, ErrorChain& err);

    // Child manipulation
    void UnlinkChild(FSNode* child, ErrorChain& err) override;
    void AddChild(FSNode* child, ErrorChain& err) override;

private:
    VirtualFileSystem* m_VFS;
};

}