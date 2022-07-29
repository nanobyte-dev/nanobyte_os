b *0x7c00
layout asm
target remote | qemu-system-i386 -S -gdb stdio -m 16 -boot c -hda build/main_disk.raw
