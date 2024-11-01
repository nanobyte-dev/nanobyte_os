    set disassembly-flavor intel
    target remote | qemu-system-i386 -S -gdb stdio -m 32 -hda /home/tibi/nanobyte_os/build/i686_debug/image.img
    set tdesc filename /home/tibi/nanobyte_os/scripts/target.xml
    symbol-file /home/tibi/nanobyte_os/build/i686_debug/stage2/stage2.elf
