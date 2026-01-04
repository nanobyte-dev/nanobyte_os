#pragma once
#include <stdint.h>
#include <stdbool.h>

#define ASMCALL __attribute__((cdecl))

void ASMCALL x86_outb(uint16_t port, uint8_t value);
uint8_t ASMCALL x86_inb(uint16_t port);

bool ASMCALL x86_Disk_GetDriveParams(uint8_t drive,
                                     uint8_t* driveTypeOut,
                                     uint16_t* cylindersOut,
                                     uint16_t* sectorsOut,
                                     uint16_t* headsOut);

bool ASMCALL x86_Disk_Reset(uint8_t drive);

bool ASMCALL x86_Disk_Read(uint8_t drive,
                           uint16_t cylinder,
                           uint16_t sector,
                           uint16_t head,
                           uint8_t count,
                           void* lowerDataOut);

typedef struct 
{
    uint64_t Base;
    uint64_t Length;
    uint32_t Type;
    uint32_t ACPI;

} E820MemoryBlock;

enum E820MemoryBlockType 
{
    E820_USABLE = 1,
    E820_RESERVED = 2,
    E820_ACPI_RECLAIMABLE = 3,
    E820_ACPI_NVS = 4,
    E820_BAD_MEMORY = 5,
};

int ASMCALL x86_E820GetNextBlock(E820MemoryBlock* block, uint32_t* continuationId);