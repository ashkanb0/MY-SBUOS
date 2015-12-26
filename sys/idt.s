.text

.globl interrupt_0_handler
.align 4
 
interrupt_0_handler:
	# pushad
	# cld
	call int_0_srv
	# popad
	iretq

.globl keyboard_interrupt_handler
.align 4
keyboard_interrupt_handler:
	# pushad
	# cld
	push %ax
	call int_kbd_srv
	pop %ax
	# popad

	iretq
