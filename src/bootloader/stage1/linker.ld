ENTRY(start)
OUTPUT_FORMAT("binary")
phys = 0x7C00;

SECTIONS
{
    . = phys;

    .fsjump             : { *(.fsjump)           }
    .fsheaders          : { *(.fsheaders)       }
    .entry              : { __entry_start = .;
                            *(.entry)   
                          }
    .text               : { *(.text)            }
    .data               : { *(.data)            }
    .rodata             : { *(.rodata)          }
    .bios_footer 0x7DFE : { SHORT(0xAA55)       }
    .bss                : { *(.bss)             }
}