#pragma once

#include <stddef.h>
#include <stdint.h>
#include <etl/expected.h>
#include <util/ErrorChain.hpp>

namespace hal::fs
{

enum class SeekPos {
    Set,
    Current,
    End,
};

enum FileOpenMode
{
    Read,
    Write,
    Append
};

class FSNode;

class File
{
public:
    virtual FSNode* Entry() { return m_FSNode; }
    
    virtual size_t Read(uint8_t* data, size_t size, ErrorChain& err) = 0;
    virtual size_t Write(const uint8_t* data, size_t size, ErrorChain& err) = 0;
    virtual void Seek(SeekPos pos, int rel, ErrorChain& err) = 0;
    virtual size_t Size() = 0;
    virtual size_t Position() = 0;

    virtual size_t ReadAbs(size_t position, uint8_t* data, size_t size, ErrorChain& err);

protected:
    virtual void Initialize(FSNode* FSNode, FileOpenMode mode, ErrorChain& err);

    FSNode* m_FSNode;
    FileOpenMode m_Mode;
};

}