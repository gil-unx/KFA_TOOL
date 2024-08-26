.PSP

.open "EBOOT.BIN","EBOOT_PATCHED.BIN",0x8804000 -0xA0

//force use decrypted main.dat
.org 0x08857a04
	nop//jal        memcpy
.org 0x08857a08
	nop//or         a2,s0,zero
.org 0x08857c54
	nop//jal        memcpy
.org 0x08857c58
	nop//or         a2,s0,zero

.close

