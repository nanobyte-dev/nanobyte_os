bits 32

global memmove_str
memmove_str:
    push ebp
    mov ebp, esp

    mov edi, [ebp + 8]    ; destination
    mov esi, [ebp + 12]   ; source
    mov ecx, [ebp + 16]   ; count

    ; Check if source and destination overlap
    cmp edi, esi
    je .done          ; If source and destination are the same, we're done
    jb .forward_copy  ; If destination < source, we can copy forward

    ; Backward copy (destination > source)
    add esi, ecx
    add edi, ecx
    dec esi
    dec edi
    std               ; Set direction flag for backward copy
    rep movsb
    cld               ; Clear direction flag
    jmp .done

.forward_copy:
    rep movsb         ; Copy bytes from source to destination

.done:
    mov eax, [ebp + 8] ; Return destination pointer

    ; Restore stack frame
    mov esp, ebp
    pop ebp
    ret

