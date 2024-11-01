#pragma once

#include <etl/expected.h>
#include <etl/string_view.h>
#include <hal/fs/File.hpp>
#include <util/ResultCode.hpp>

namespace hal::fs
{

enum class FileType
{
    File,
    Directory
};


namespace FileCreateOptions_
{
    enum FileCreateOptions
    {
        None            = 0,
        Overwrite       = 1 << 0,
        ReturnExisting  = 1 << 1,
    };
}

using FileCreateOptions = FileCreateOptions_::FileCreateOptions;


class FileSystem;

class FSNode
{
public:
    FSNode(FileSystem* fs);
    virtual etl::string_view Name() = 0;
    virtual const FileType Type() = 0;
    virtual FileSystem* FS() { return m_FS; }

    // File operations
    virtual File* Open(FileOpenMode mode, ErrorChain& err) = 0;
    virtual void Delete(ErrorChain& err) = 0;
    virtual void Rename(const etl::string_view& newName, ErrorChain& err) = 0;
    virtual void Move(FSNode* newParent, ErrorChain& err) = 0;

    // Navigation
    virtual FSNode* FirstChild(ErrorChain& err) = 0;
    virtual FSNode* NextSibling(ErrorChain& err) = 0;
    virtual FSNode* Parent(ErrorChain& err) = 0;
    virtual FSNode* FindChild(const etl::string_view& childName, ErrorChain& err) = 0;

    virtual FSNode* LocateChild(const etl::string_view& path, ErrorChain& err);

    // Child manipulation
    virtual void UnlinkChild(FSNode* child, ErrorChain& err) = 0;
    virtual void AddChild(FSNode* child, ErrorChain& err) = 0;

protected:
    FileSystem* m_FS;
};

}