.text

.globl interrupt_0_handler
.align 4
 
interrupt_0_handler:
	# pushad
	# TODO : save rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11
	# push %ax
	# push %bx
	# push %cx
	# push %dx
	# pushf
	# cld
	call int_0_srv
	# popad
	# pop %dx
	# pop %cx
	# pop %bx
	# pop %ax
	# popf
	iretq

.globl keyboard_interrupt_handler
.align 4
keyboard_interrupt_handler:
	# pushad
	# pushf
	# cld
	# push %ax
	# push %bx
	# push %cx
	# push %dx
	call int_kbd_srv
	# TODO : send EOI
	# pop %dx
	# pop %cx
	# pop %bx
	# pop %ax
	# popf
	# popad

	iretq
