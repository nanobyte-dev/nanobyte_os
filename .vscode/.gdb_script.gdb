    symbol-file /home/tibi/Nanobyte/nanobyte_os_live3/build/i686_debug/kernel/kernel.elf
    set disassembly-flavor intel
    target remote | qemu-system-i386 -S -gdb stdio -m 32 -hda /home/tibi/Nanobyte/nanobyte_os_live3/build/i686_debug/image.img
