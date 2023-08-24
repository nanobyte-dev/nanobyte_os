bits 16

%include "config.inc"

%define ENDL 0x0D, 0x0A

%define fat12 1
%define fat16 2
%define fat32 3
%define ext2  4


;
; FAT12 header
; 

section .fsjump

    jmp short start
    nop

section .fsheaders

%if (FILESYSTEM == fat12) || (FILESYSTEM == fat16) || (FILESYSTEM == fat32)

    bdb_oem:                    db "abcdefgh"           ; 8 bytes
    bdb_bytes_per_sector:       dw 512
    bdb_sectors_per_cluster:    db 1
    bdb_reserved_sectors:       dw 1
    bdb_fat_count:              db 2
    bdb_dir_entries_count:      dw 0E0h
    bdb_total_sectors:          dw 2880                 ; 2880 * 512 = 1.44MB
    bdb_media_descriptor_type:  db 0F0h                 ; F0 = 3.5" floppy disk
    bdb_sectors_per_fat:        dw 9                    ; 9 sectors/fat
    bdb_sectors_per_track:      dw 18
    bdb_heads:                  dw 2
    bdb_hidden_sectors:         dd 0
    bdb_large_sector_count:     dd 0

    %if (FILESYSTEM == fat32)
        fat32_sectors_per_fat:      dd 0
        fat32_flags:                dw 0
        fat32_fat_version_number:   dw 0
        fat32_rootdir_cluster:      dd 0
        fat32_fsinfo_sector:        dw 0
        fat32_backup_boot_sector:   dw 0
        fat32_reserved:             times 12 db 0
    %endif

    ; extended boot record
    ebr_drive_number:           db 0                    ; 0x00 floppy, 0x80 hdd, useless
                                db 0                    ; reserved
    ebr_signature:              db 29h
    ebr_volume_id:              db 12h, 34h, 56h, 78h   ; serial number, value doesn't matter
    ebr_volume_label:           db 'NANOBYTE OS'        ; 11 bytes, padded with spaces
    ebr_system_id:              db 'FAT12   '           ; 8 bytes

%endif

;
; Code goes here
;
section .entry
    global start

    ; the MBR will pass us the following information:
    ; DL = drive number
    ; DS:SI = partition table entry that we booted from

    start:
        ;
        ; copy partition entry to somewhere safe
        ;
        mov ax, STAGE1_SEGMENT
        mov es, ax
        mov di, partition_entry
        mov cx, 16
        rep movsb
        
        ;
        ; relocate self
        ;
        mov ax, 0
        mov ds, ax
        mov si, 0x7C00
        ; mov ax, STAGE1_SEGMENT
        ; mov es, ax
        mov di, STAGE1_OFFSET
        mov cx, 512
        rep movsb

        jmp STAGE1_SEGMENT:.relocated           ; far jump to relocated code

    .relocated:

        ;
        ; initialization
        ;

        ; setup data segments
        mov ax, STAGE1_SEGMENT                  ; can't set ds/es directly
        mov ds, ax
        ; mov es, ax
        
        ; setup stack
        mov ss, ax
        mov sp, STAGE1_OFFSET                   ; stack grows downwards from where we are loaded in memory

        ; save drive number from DL
        mov [ebr_drive_number], dl

        ; check extensions present
        mov ah, 0x41
        mov bx, 0x55AA
        stc
        int 13h

        jc .no_disk_extensions
        cmp bx, 0xAA55
        jne .no_disk_extensions

        ; extensions are present
        mov byte [have_extensions], 1
        jmp .after_disk_extensions_check

    .no_disk_extensions:
        mov byte [have_extensions], 0

    .after_disk_extensions_check:
        ;
        ; load boot_table_lba which contains the boot table
        ;
        mov eax, [boot_table_lba]
        mov cx, 1
        mov bx, boot_table
        call disk_read

        ;
        ; parse boot table and load
        ;
        mov si, boot_table

        ; parse entry point
        mov eax, [si]
        mov [entry_point], eax
        add si, 4

    .readloop:
        ; read until lba=0
        cmp dword [si + boot_table_entry.lba], 0
        je .endreadloop

        mov eax, [si + boot_table_entry.lba]
        mov bx, [si + boot_table_entry.load_seg]
        mov es, bx
        mov bx, [si + boot_table_entry.load_off]
        mov cx, [si + boot_table_entry.count]
        call disk_read

        ; go to next entry
        add si, boot_table_entry_size
        jmp .readloop

    .endreadloop:
       
        ; jump to our kernel
        mov dl, [ebr_drive_number]          ; boot device in dl
        mov di, partition_entry             ; es:di points to partition entry
    
        mov ax, [entry_point.seg]           ; set segment registers
        mov ds, ax

        ; do far jump
        push ax
        push word [entry_point.off]
        retf

    .never:
        jmp wait_key_and_reboot             ; should never happen
        cli                                 ; disable interrupts, this way CPU can't get out of "halt" state
        hlt


section .text

    ;
    ; Error handlers
    ;

    floppy_error:
        mov si, msg_read_failed
        call puts
        jmp wait_key_and_reboot

    wait_key_and_reboot:
        mov ah, 0
        int 16h                     ; wait for keypress
        jmp 0FFFFh:0                ; jump to beginning of BIOS, should reboot

    .halt:
        cli                         ; disable interrupts, this way CPU can't get out of "halt" state
        hlt


    ;
    ; Prints a string to the screen
    ; Params:
    ;   - ds:si points to string
    ;
    puts:
        ; save registers we will modify
        push si
        push ax
        push bx

    .loop:
        lodsb               ; loads next character in al
        or al, al           ; verify if next character is null?
        jz .done

        mov ah, 0x0E        ; call bios interrupt
        mov bh, 0           ; set page number to 0
        int 0x10

        jmp .loop

    .done:
        pop bx
        pop ax
        pop si    
        ret

    ;
    ; Disk routines
    ;

    ;
    ; Converts an LBA address to a CHS address
    ; Parameters:
    ;   - ax: LBA address
    ; Returns:
    ;   - cx [bits 0-5]: sector number
    ;   - cx [bits 6-15]: cylinder
    ;   - dh: head
    ;

    lba_to_chs:

        push ax
        push dx

        xor dx, dx                          ; dx = 0
        div word [bdb_sectors_per_track]    ; ax = LBA / SectorsPerTrack
                                            ; dx = LBA % SectorsPerTrack

        inc dx                              ; dx = (LBA % SectorsPerTrack + 1) = sector
        mov cx, dx                          ; cx = sector

        xor dx, dx                          ; dx = 0
        div word [bdb_heads]                ; ax = (LBA / SectorsPerTrack) / Heads = cylinder
                                            ; dx = (LBA / SectorsPerTrack) % Heads = head
        mov dh, dl                          ; dh = head
        mov ch, al                          ; ch = cylinder (lower 8 bits)
        shl ah, 6
        or cl, ah                           ; put upper 2 bits of cylinder in CL

        pop ax
        mov dl, al                          ; restore DL
        pop ax
        ret


    ;
    ; Reads sectors from a disk
    ; Parameters:
    ;   - eax: LBA address
    ;   - cl: number of sectors to read (up to 128)
    ;   - dl: drive number
    ;   - es:bx: memory address where to store read data
    ;
    disk_read:

        push eax                            ; save registers we will modify
        push bx
        push cx
        push dx
        push si
        push di

        cmp byte [have_extensions], 1
        jne .no_disk_extensions

        ; with extensions
        mov [extensions_dap.lba], eax
        mov [extensions_dap.segment], es
        mov [extensions_dap.offset], bx
        mov [extensions_dap.count], cx

        mov ah, 0x42
        mov si, extensions_dap
        mov di, 3                           ; retry count

        .retry:
            pusha                           ; save all registers, we don't know what bios modifies
            stc                             ; set carry flag, some BIOS'es don't set it
            int 13h                         ; carry flag cleared = success
            jnc .done                       ; jump if carry not set

            ; read failed
            popa
            call disk_reset

            dec di
            test di, di
            jnz .retry

        .fail:
            ; all attempts are exhausted
            jmp floppy_error

        .done:
            popa
            jmp .function_end

    .no_disk_extensions:
        mov esi, eax                            ; save lba to esi
        mov di, cx                              ; save number of sectors to di

        .outer_loop:
            mov eax, esi
            call lba_to_chs                     ; compute CHS
            mov al, 1                           ; read 1 sector

            push di
            mov di, 3                           ; retry count
            mov ah, 02h

            .inner_retry:
                pusha                           ; save all registers, we don't know what bios modifies
                stc                             ; set carry flag, some BIOS'es don't set it
                int 13h                         ; carry flag cleared = success
                jnc .inner_done                 ; jump if carry not set

                ; read failed
                popa
                call .inner_retry

                dec di
                test di, di
                jnz .retry

            .inner_fail:
                ; all attempts are exhausted
                jmp floppy_error

            .inner_done:

            popa
            pop di

            cmp di, 0                   ; exit condition - have we read all the sectors?
            je .function_end

            inc esi                     ; increment lba we want to read
            dec di                      ; decrement number of sectors
            
            mov ax, es
            add ax, 512 / 16            ; increment destination address (use segment to avoid segment boundary trouble)
            mov es, ax
            jmp .outer_loop


    .function_end:
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop eax                            ; restore registers modified
        ret


    ;
    ; Resets disk controller
    ; Parameters:
    ;   dl: drive number
    ;
    disk_reset:
        pusha
        mov ah, 0
        stc
        int 13h
        jc floppy_error
        popa
        ret

section .rodata

    msg_read_failed:        db 'Read failed!', ENDL, 0

section .data

    extensions_dap:
        .size:              db 10h
                            db 0
        .count:             dw 0
        .offset:            dw 0
        .segment:           dw 0
        .lba:               dq 0

    struc boot_table_entry
        .lba                resd 1
        .load_off           resw 1
        .load_seg           resw 1
        .count              resw 1
    endstruc

section .data
    global boot_table_lba
    boot_table_lba:         dd 0

section .bss
    have_extensions:        resb 1
    buffer:                 resb 512
    partition_entry:        resb 16
    boot_table:             resb 512
    entry_point:
        .off                resw 1
        .seg                resw 1