#include "disk.h"
#include <stdio.h>

#define SECTOR_SIZE 512

bool DISK_Initialize(DISK* disk, const char* input_file)
{
    disk->File = fopen(input_file, "rb");
    return (disk->File != NULL);
}

bool DISK_ReadSectors(DISK* disk, uint32_t lba, uint8_t sectors, void far* dataOut)
{
    fseek(disk->File, lba * SECTOR_SIZE, SEEK_SET);
    return fread(dataOut, SECTOR_SIZE, sectors, disk->File) > 0;
}
