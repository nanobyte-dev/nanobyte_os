bits 32

global memcpy
memcpy:

    ; make new call frame
    push ebp              ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov edi, [ebp + 8]    ; dest
    mov esi, [ebp + 12]   ; src
    mov ecx, [ebp + 16]   ; count

    rep movsb

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret