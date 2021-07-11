#pragma once

#include <stdint.h>
#include <stdio.h>

typedef int bool;
#define true    1
#define false   0
#define far

typedef struct {
    FILE* File;
} DISK;

bool DISK_Initialize(DISK* disk, const char* fileName);
bool DISK_ReadSectors(DISK* disk, uint32_t lba, uint8_t sectors, void far* dataOut);
