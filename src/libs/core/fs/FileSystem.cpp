#include "FileSystem.hpp"
#include <String.hpp>
#include <Memory.hpp>

File* FileSystem::Open(const char* path, FileOpenMode mode)
{
    char name[MAX_PATH_SIZE];

    // ignore leading slash
    if (path[0] == '/')
        path++;

    File* root = this->RootDirectory();

    while (*path) 
    {
        // extract next file name from path
        bool isLast = false;
        const char* delim = String::Find(path, '/');
        if (delim != NULL)
        {
            Memory::Copy(name, path, delim - path);
            name[delim - path] = '\0';
            path = delim + 1;
        }
        else
        {
            unsigned len = String::Length(path);
            Memory::Copy(name, path, len);
            name[len + 1] = '\0';
            path += len;
            isLast = true;
        }

        FileEntry* nextEntry = FindFile(root, name);
        if (nextEntry)
        {
            // release current
            root->Release();

            // check if directory
            if (!isLast && nextEntry->Type() != FileType::Directory)
            {
                // printf("FAT: %s not a directory\r\n", name);
                return nullptr;
            }

            // open new directory entry
            root = nextEntry->Open(isLast ? mode : FileOpenMode::Read);
            nextEntry->Release();
        }
        else
        {
            // release current
            root->Release();

            // printf("FAT: %s not found\r\n", name);
            return nullptr;
        }
    }

    return root;
}

FileEntry* FileSystem::FindFile(File* dir, const char* name)
{
    // find directory entry in current directory
    FileEntry* entry = dir->ReadFileEntry();
    while (entry != nullptr)
    {
        if (strcmp(entry->Name(), name) == 0)
        {
            // todo: close & release root
            return entry;
        }
       
        entry->Release();
        entry = dir->ReadFileEntry();
    }

    return nullptr;
}