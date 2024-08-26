.PSP

.open "EBOOT.BIN","EBOOT_PATCHED.BIN",0x8804000 -0x80

//force use decrypted main.dat
.org 0x 08857ad8
	nop//jal   memcopy 
.org 0x08857adc
	nop//or a2,s0,zero
.org 0x08857d28
	nop//jal   memcopy 
.org 0x08857d2c
	nop//or a2,s0,zero

.close

