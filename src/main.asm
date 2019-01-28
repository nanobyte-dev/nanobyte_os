org 0x7C00
bits 16


main:
	hlt
    
.halt:
	jmp .halt


times 510-($-$$) db 0
dw 0AA55h