[bits 32]

global Out
Out:
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

global In
In:
    mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret