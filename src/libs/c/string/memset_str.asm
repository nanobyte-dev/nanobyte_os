bits 32

global memset_str
memset_str:
    ; make new call frame
    push ebp              ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov edi, [ebp + 8]    ; ptr
    mov eax, [ebp + 12]   ; value
    mov ecx, [ebp + 16]   ; num

    ; Spread the byte value to all bytes of EAX
    mov ah, al
    mov edx, eax
    shl edx, 16
    or eax, edx

    ; For small counts, byte-by-byte is faster
    cmp ecx, 4
    jb .byte_loop

    ; Set as many dwords as possible
    mov edx, ecx
    shr ecx, 2
    rep stosd

    ; Set remaining bytes
    mov ecx, edx
    and ecx, 3
    rep stosb

    jmp .done

.byte_loop:
    rep stosb

.done:
    ; Return the original pointer
    mov eax, [ebp + 8]

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret
