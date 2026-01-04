ENTRY(start)
OUTPUT_FORMAT("binary")
phys = 0x7C00;                  /* BIOS (or MBR) puts us at 0x7C00 */

SECTIONS
{
    . = phys;

    /* Some filesystems (like FAT) have a header that begins with a 3 byte 
       jump instruction, which will end up in the .fsjump section.      
    */
    .fsjump             : { *(.fsjump)           }

    /* Section reserved for file system headers. */
    .fsheaders          : { *(.fsheaders)       }

    /* Entry section, which shouldn't be too far down the file (the near jump in
       .fsjump is limited to 127 bytes)
    */
    .entry              : { __entry_start = .;
                            *(.entry)   
                          }

    /* Code */
    .text               : { *(.text)            }

    /* Modifiable data */
    .data               : { *(.data)            }

    /* Read-only data */
    .rodata             : { *(.rodata)          }

    /* Required for the BIOS to boot our OS */
    .bios_footer 0x7DFE : { SHORT(0xAA55)       }

    /* Uninitialized memory (declared with resb/resw/resq). We can use memory
       beyond this binary for that. 
    */
    .bss                : { *(.bss)             }
}