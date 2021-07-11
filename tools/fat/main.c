#include "stdint.h"
#include "stdio.h"
#include "disk.h"
#include "fat.h"

int main(int argc, const char** argv)
{
    if (argc < 3)
    {
        printf("Syntax: %s <image> <file_path>", argv[0]);
    }

    DISK disk;
    if (!DISK_Initialize(&disk, argv[1]))
    {
        printf("Disk init error\n");
        return -1;
    }

    if (!FAT_Initialize(&disk))
    {
        printf("FAT init error\n");
        return -1;
    }

    // browse files in root
    FAT_File far* fd = FAT_Open(&disk, argv[2]);

    if (fd->IsDirectory)
    {
        FAT_DirectoryEntry entry;
        int i = 0;
        while (FAT_ReadEntry(&disk, fd, &entry) && i++ < 10)
        {
            printf("  ");
            for (int i = 0; i < 11; i++)
                putc(entry.Name[i], stdout);
            printf("\r\n");
        }
    }
    else
    {
        uint32_t read = 0;
        char buffer[100];
        while ((read = FAT_Read(&disk, fd, sizeof(buffer), buffer)))
        {
            // for (uint32_t i = 0; i < read; i++)
            //     putc(buffer[i], stdout);
            // fflush(stdout);
        }
    }
    
    FAT_Close(fd);

    return 0;
}
