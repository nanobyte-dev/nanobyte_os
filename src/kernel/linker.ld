ENTRY(start)
OUTPUT_FORMAT("elf32-i386")
phys = 0x00100000;

SECTIONS
{
    . = phys;
    
    .text               : { __text_start = .;       *(.text)    }
    .data               : { __data_start = .;       *(.data)    }
    .rodata             : { __rodata_start = .;     *(.rodata)  }
    .bss                : { __bss_start = .;        *(.bss)     }
    
    __end = .;
}
