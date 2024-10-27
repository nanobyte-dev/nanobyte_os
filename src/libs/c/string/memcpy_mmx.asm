bits 32

global memcpy_mmx
memcpy_mmx:
    ; make new call frame
    push ebp              ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov edi, [ebp + 8]    ; dest
    mov esi, [ebp + 12]   ; src
    mov ecx, [ebp + 16]   ; count

    cmp ecx, 8
    jb .small_copy

    ; Align destination to 8-byte boundary
    mov edx, edi
    and edx, 7
    jz .aligned
    neg edx
    add edx, 8
    sub ecx, edx
    rep movsb

.aligned:
    mov edx, ecx
    shr ecx, 6            ; 64 bytes per iteration
    jz .handle_remainder

.mmx_loop:
    movq mm0, [esi]
    movq mm1, [esi + 8]
    movq mm2, [esi + 16]
    movq mm3, [esi + 24]
    movq mm4, [esi + 32]
    movq mm5, [esi + 40]
    movq mm6, [esi + 48]
    movq mm7, [esi + 56]
    movq [edi], mm0
    movq [edi + 8], mm1
    movq [edi + 16], mm2
    movq [edi + 24], mm3
    movq [edi + 32], mm4
    movq [edi + 40], mm5
    movq [edi + 48], mm6
    movq [edi + 56], mm7
    add esi, 64
    add edi, 64
    dec ecx
    jnz .mmx_loop

.handle_remainder:
    mov ecx, edx
    and ecx, 63
    shr ecx, 3
    jz .final_bytes
    
.mmx_remainder:
    movq mm0, [esi]
    movq [edi], mm0
    add esi, 8
    add edi, 8
    dec ecx
    jnz .mmx_remainder

.final_bytes:
    mov ecx, edx
    and ecx, 7
    rep movsb

    emms                  ; Empty MMX state
    jmp .done

.small_copy:
    rep movsb

.done:
    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

global memset_mmx
memset_mmx:
    ; make new call frame
    push ebp              ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov edi, [ebp + 8]    ; ptr
    mov eax, [ebp + 12]   ; value
    mov ecx, [ebp + 16]   ; num

    ; Spread the byte value to all bytes of MM0
    movd mm0, eax
    punpcklbw mm0, mm0
    punpcklwd mm0, mm0
    punpckldq mm0, mm0

    cmp ecx, 8
    jb .small_set

    ; Align destination to 8-byte boundary
    mov edx, edi
    and edx, 7
    jz .aligned
    neg edx
    add edx, 8
    sub ecx, edx
    rep stosb

.aligned:
    mov edx, ecx
    shr ecx, 6            ; 64 bytes per iteration
    jz .handle_remainder

.mmx_loop:
    movq [edi], mm0
    movq [edi + 8], mm0
    movq [edi + 16], mm0
    movq [edi + 24], mm0
    movq [edi + 32], mm0
    movq [edi + 40], mm0
    movq [edi + 48], mm0
    movq [edi + 56], mm0
    add edi, 64
    dec ecx
    jnz .mmx_loop

.handle_remainder:
    mov ecx, edx
    and ecx, 63
    shr ecx, 3
    jz .final_bytes

.mmx_remainder:
    movq [edi], mm0
    add edi, 8
    dec ecx
    jnz .mmx_remainder

.final_bytes:
    mov ecx, edx
    and ecx, 7
    rep stosb

    emms                  ; Empty MMX state
    jmp .done

.small_set:
    rep stosb

.done:
    ; Return the original pointer
    mov eax, [ebp + 8]

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret
