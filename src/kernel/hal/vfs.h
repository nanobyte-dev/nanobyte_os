#pragma once
#include <stdint.h>
#include <stddef.h>

typedef int fd_t;

#define VFS_FD_STDIN    0
#define VFS_FD_STDOUT   1
#define VFS_FD_STDERR   2
#define VFS_FD_DEBUG    3

int VFS_Write(fd_t file, uint8_t* data, size_t size);