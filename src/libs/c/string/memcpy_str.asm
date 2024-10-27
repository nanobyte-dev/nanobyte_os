bits 32

global memcpy_str
memcpy_str:

    ; make new call frame
    push ebp              ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov edi, [ebp + 8]    ; dest
    mov esi, [ebp + 12]   ; src
    mov ecx, [ebp + 16]   ; count

    cmp ecx, 4
    jb .byte_loop

    ; Align destination to 4-byte boundary
    mov edx, edi
    and edx, 3
    jz .aligned
    neg edx
    add edx, 4
    sub ecx, edx
    rep movsb

.aligned:
    mov edx, ecx
    shr ecx, 2
    rep movsd

    ; Handle remaining bytes
    mov ecx, edx
    and ecx, 3
    rep movsb
    jmp .done

.byte_loop:
    rep movsb

.done:
    ; Return the original pointer
    mov eax, [ebp + 8]

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

