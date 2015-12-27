.text

.globl interrupt_0_handler
.align 4
 
interrupt_0_handler:
	# pushad
	# TODO : save rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11
	push %ax
	push %di
	push %si
	push %dx
	push %cx
	push %r8
	push %r9
	push %r10
	push %r11
	# pushf
	# cld
	call int_0_srv
	# popad
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %cx
	pop %dx
	pop %si
	pop %di
	pop %ax
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
