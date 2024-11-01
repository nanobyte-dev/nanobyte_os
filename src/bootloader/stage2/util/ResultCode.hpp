#pragma once

enum class ResultCode
{
    Ok,
    NoMoreItems,

    // Generic errors    
    NotImplemented,
    NotSupported,
    NotInitialized,
    NotFound,
    ArgumentNull,
    ArgumentOutOfRange,
    InvalidArgument,
    InvalidOperation,
    NullDereference,
    ContainerFull,
    AlreadyExists,
    
    // IO
    IOFailed,

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
