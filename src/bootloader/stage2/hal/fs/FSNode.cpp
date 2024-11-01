#include "FSNode.hpp"
#include "FileSystem.hpp"
#include "PathIterator.hpp"

namespace hal::fs {

FSNode::FSNode(FileSystem *fs)
    : m_FS(fs)
{
}

FSNode* FSNode::LocateChild(const etl::string_view &path, ErrorChain& err)
{
    if (err.Failed()) return nullptr;

    // Sanity checks
    if (path.empty())
    {
        err.Fail(ResultCode::InvalidArgument, "Empty path");
        return nullptr;
    }

    PathIterator pathIterator(path);

    FSNode* current = this;
    auto nextName = pathIterator.Next();
    for (; nextName && current != nullptr; nextName = pathIterator.Next())
    {
        if (nextName.value() == ".")
            continue;
        else if (nextName.value() == "..")
            current = current->Parent(err);
        else
            current = current->FindChild(nextName.value(), err);
    }

    if (current == nullptr)
        err.Fail(ResultCode::NotFound, "Path does not exist");

    return current;
}

}