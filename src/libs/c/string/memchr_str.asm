bits 32

global memchr_str
memchr_str:
    push ebp
    mov ebp, esp

    mov edi, [ebp + 8]
    mov al, [ebp + 12]
    mov ecx, [ebp + 16]

    ; Use REPNE SCASB to search for the character
    repne scasb
    je .found
    
    ; If not found, return NULL
    xor eax, eax
    jmp .done
    
.found:
    ; If found, return the address (EDI - 1)
    mov eax, edi
    dec eax
    
.done:
    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret
