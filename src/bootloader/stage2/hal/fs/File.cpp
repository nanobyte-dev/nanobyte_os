#include "File.hpp"

namespace hal::fs
{

void File::Initialize(FSNode *fsnode, FileOpenMode mode, ErrorChain& err)
{
    if (err.Failed())
        return;

    if (fsnode == nullptr)
    {
        err.Fail(ResultCode::ArgumentNull, "fsnode is null");
        return;
    }

    m_FSNode = fsnode;
    m_Mode = mode;
}

size_t File::ReadAbs(size_t position, uint8_t *data, size_t size, ErrorChain& err)
{
    if (err.Failed())
        return 0;

    Seek(SeekPos::Set, position, err);
    err.FailOnError(ResultCode::IOFailed, "Read failed");
    
    return Read(data, size, err);
}

}