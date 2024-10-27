bits 32

global memcmp_str
memcmp_str:

    ; make new call frame
    push ebp              ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov edi, [ebp + 8]    ; dest
    mov esi, [ebp + 12]   ; src
    mov ecx, [ebp + 16]   ; count

    ; Use REPE CMPSB to compare memory
    xor eax, eax          ; Clear EAX for the result
    repe cmpsb            ; Compare bytes while equal
    jz .equal             ; If zero flag is set, strings are equal

    ; If not equal, set the result based on the last comparison
    mov al, byte [edi - 1]
    sub al, byte [esi - 1]
    movsx eax, al         ; Sign-extend AL to EAX

    jmp .done

.equal:
    xor eax, eax          ; Set result to 0 if strings are equal

.done:

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret