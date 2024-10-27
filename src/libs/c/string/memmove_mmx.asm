bits 32

global memmove_mmx
memmove_mmx:
    push ebp
    mov ebp, esp

    mov edi, [ebp + 8]      ; destination
    mov esi, [ebp + 12]     ; source
    mov ecx, [ebp + 16]     ; count

    ; Check if source and destination overlap
    cmp edi, esi
    je .done                ; If source and destination are the same, we're done
    jb .forward_copy        ; If destination < source, we can copy forward

    ; Backward copy (destination > source)
    add esi, ecx
    add edi, ecx
    sub esi, 8
    sub edi, 8

    ; Align to 8-byte boundary for backward copy
    mov edx, edi
    and edx, 7
    sub ecx, edx
    rep movsb

    ; MMX backward copy
    shr ecx, 3
    jz .remainder_backward

.mmx_backward_loop:
    movq mm0, [esi]
    movq [edi], mm0
    sub esi, 8
    sub edi, 8
    dec ecx
    jnz .mmx_backward_loop

    jmp .remainder_backward

.forward_copy:
    ; Align to 8-byte boundary for forward copy
    mov edx, edi
    and edx, 7
    mov eax, 8
    sub eax, edx
    and eax, 7
    sub ecx, eax
    rep movsb

    ; MMX forward copy
    shr ecx, 3
    jz .remainder_forward

.mmx_forward_loop:
    movq mm0, [esi]
    movq [edi], mm0
    add esi, 8
    add edi, 8
    dec ecx
    jnz .mmx_forward_loop

.remainder_forward:
    mov ecx, [ebp + 16]
    and ecx, 7
    rep movsb
    jmp .done

.remainder_backward:
    mov ecx, [ebp + 16]
    and ecx, 7
    add esi, 7
    add edi, 7
    std
    rep movsb
    cld

.done:
    emms                ; Clear MMX state
    mov eax, [ebp + 8]  ; Return destination pointer

    ; Restore stack frame
    mov esp, ebp
    pop ebp
    ret
