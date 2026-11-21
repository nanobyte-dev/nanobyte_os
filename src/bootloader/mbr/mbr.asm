bits 16

;
; Master boot record (MBR) boot code
;
; The BIOS loads the first sector of the disk at 0x7C00 and jumps to it. That sector holds the
; partition table, and in front of it, this code. All it does is find the active (bootable)
; partition, load the first sector of that partition (our stage1) at 0x7C00, and jump to it.
;
; Stage1 expects what any MBR passes along:
;   - dl: the drive number
;   - ds:si: the partition entry we booted from
;

%define ENDL 0x0D, 0x0A

MBR_LOAD_ADDRESS        equ 0x7C00      ; where the BIOS loads us
MBR_RELOCATE_ADDRESS    equ 0x0600      ; where we move ourselves (see linker.ld)
PARTITION_TABLE         equ MBR_RELOCATE_ADDRESS + 446
PARTITION_ENTRY_SIZE    equ 16
PARTITION_COUNT         equ 4
BOOT_SIGNATURE          equ 0xAA55


section .text
    global start

    start:
        cli
        xor ax, ax
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov sp, MBR_LOAD_ADDRESS    ; stack grows downwards from where we are loaded
        sti

        ; The partition's boot sector needs to be loaded at 0x7C00, where we are now, so move
        ; out of the way first. Until the far jump, we're running at a different address than
        ; the one we're linked at, so this part must not use any labels.
        cld
        mov si, MBR_LOAD_ADDRESS
        mov di, MBR_RELOCATE_ADDRESS
        mov cx, 256                 ; 512 bytes, as words
        rep movsw

        jmp 0:.relocated            ; also makes sure cs = 0

    .relocated:
        mov [drive_number], dl      ; BIOS should set DL to drive number

        ; find the active partition: bit 7 of the first byte of the entry is set
        mov si, PARTITION_TABLE
        mov cx, PARTITION_COUNT

    .find_active:
        test byte [si], 0x80
        jnz .found_active
        add si, PARTITION_ENTRY_SIZE
        loop .find_active

        mov si, msg_no_active_partition
        jmp error

    .found_active:
        ; check if the BIOS supports the disk extensions (LBA reads)
        mov ah, 0x41
        mov bx, 0x55AA
        mov dl, [drive_number]
        stc
        int 13h
        jc .read_chs
        cmp bx, 0xAA55
        jne .read_chs

        ; LBA read: the partition's start sector is at offset 8 in the entry
        mov eax, [si + 8]
        mov [dap.lba], eax

        push si
        mov si, dap
        mov ah, 0x42
        mov dl, [drive_number]
        int 13h
        pop si
        jc .read_error
        jmp .read_done

    .read_chs:
        ; no extensions, use the CHS address of the partition start (offset 1 in the entry):
        ;   - byte 1: head
        ;   - byte 2: sector (bits 0-5), bits 8-9 of the cylinder (bits 6-7)
        ;   - byte 3: bits 0-7 of the cylinder
        ; which is exactly the layout int 13h wants in dh and cx
        mov dh, [si + 1]
        mov cl, [si + 2]
        mov ch, [si + 3]
        mov dl, [drive_number]
        mov bx, MBR_LOAD_ADDRESS    ; es:bx = 0:7C00
        mov ax, 0x0201              ; ah = 2 (read sectors), al = 1 sector
        int 13h
        jc .read_error

    .read_done:
        cmp word [MBR_LOAD_ADDRESS + 510], BOOT_SIGNATURE
        jne .not_bootable

        ; jump to the partition's boot sector, with ds:si pointing at its partition entry
        mov dl, [drive_number]
        jmp 0:MBR_LOAD_ADDRESS

    .read_error:
        mov si, msg_read_failed
        jmp error

    .not_bootable:
        mov si, msg_not_bootable
        jmp error

    ;
    ; Prints the message at ds:si and halts
    ;
    error:
        lodsb
        or al, al
        jz .halt
        mov ah, 0x0E                ; int 10h, ah = 0E: print character in tty mode
        mov bh, 0                   ; page number
        int 10h
        jmp error

    .halt:
        cli
        hlt
        jmp .halt


section .data

    ; disk address packet, for the int 13h extended read (ah = 42h)
    dap:
        .size:      db 10h
                    db 0
        .count:     dw 1
        .offset:    dw MBR_LOAD_ADDRESS
        .segment:   dw 0
        .lba:       dq 0

    drive_number:   db 0


section .rodata

    msg_no_active_partition:    db 'MBR: no active partition', ENDL, 0
    msg_read_failed:            db 'MBR: read failed', ENDL, 0
    msg_not_bootable:           db 'MBR: partition not bootable', ENDL, 0
