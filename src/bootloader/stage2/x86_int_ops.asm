bits 16

section _TEXT class=CODE

;
;  Performs unsigned 4 byte division
;  Supported on 386 and later
;
;  Params:
;    - DX:AX dividend
;    - CX:BX divisor
;  Returns:
;    - DX:AX quotient
;    - CX:BX remainder
;
global __U4D
__U4D:

    ; put dividend in edx:eax
    shl edx, 16
    mov dx, ax
    mov eax, edx
    xor edx, edx

    ; divisor in ecx
    shl ecx, 16
    mov cx, bx

    ; divide
    div ecx

    ; remainder from edx to cx:bx
    mov bx, dx
    shr edx, 16
    mov cx, dx

    ; quotient from eax to dx:ax
    mov edx, eax
    shr edx, 16

    ret