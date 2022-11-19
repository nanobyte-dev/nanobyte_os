#pragma once

#include "../FileEntry.hpp"
#include "../File.hpp"
#include "../fat/FATHeaders.hpp"
class FATFileSystem;

class FATFileEntry : public FileEntry
{
public:
    FATFileEntry();
    virtual void Release() override;
    
    void Initialize(FATFileSystem* fs, const FATDirectoryEntry& dirEntry);
    virtual const char* Name() override;
    virtual const FileType Type() override;

    virtual File* Open(FileOpenMode mode) override;

private:
    FATFileSystem* m_FS;
    FATDirectoryEntry m_DirEntry;
};