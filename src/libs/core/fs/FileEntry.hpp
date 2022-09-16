#pragma once

enum class FileType
{
    File,
    Directory
};

struct FileEntry
{
    char Name[256];
    uint8_t FSData[256];
    FileType Type;
};