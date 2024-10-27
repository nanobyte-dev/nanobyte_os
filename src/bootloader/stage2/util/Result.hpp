#pragma once

enum class Result
{
    Ok,

    // Generic errors    
    NotImplemented,
    ArgumentNull,
    ArgumentOutOfRange,
    InvalidArgument,
    NotFound,
    
    // Virtual file system errors
    OutOfMountpoints,
    MountpointInUse,
    MountpointNotFound,
    MountpointNameTooLong,

    OutOfFileSystemFactories,
    FileSystemCreateFailed,
    UnknownFileSystem,

    InvalidPath,
    AbsolutePathRequired,
    DirectoryExpected,
};