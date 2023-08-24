# Nanobyte OS bootloader: Stage 1

The stage1 works in the following way:

* it relocates itself to the address set in `config.inc` (0000:0700)

* reads sector 1, which contains the boot table that describes how stage2 should be loaded. The boot table has the following format:

|--------|------|------|
| offset | size | what |
|--------|------|------|
| 0 | 2 | entry point offset |
| 2 | 2 | entry point segment |
| 4 | 10 | boot table entry 1 |
| 14 | 10 | boot table entry 2 |
| ... | ... | ... |
| ?? | 10 | null table entry |

* parses the boot table entries, which have the following format, loading each section in the process

|--------|------|------|-------------|
| offset | size | what | description |
|--------|------|------|-------------|
| 0 | 4 | lba | LBA (logical block address) of sector to load |
| 4 | 2 | load offset | offset of address where the data will be loaded |
| 6 | 2 | load segment | segment of address where the data will be loaded
| 8 | 2 | count | number of sectors to load |

* null table entry contains all zeros

* performs a far jump to the entry point, passing the following arguments:

    * **DL** contains the boot disk number
    * **ES:DI** contains a pointer to the MBR entry of the partition