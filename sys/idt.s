.text

# .globl dummy_interrupt_handler_0
# .align 4
 
# dummy_interrupt_handler_0:
# 	# pushad
# 	# TODO : save rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11
# 	cli
# 	push %ax
# 	push %di
# 	push %si
# 	push %dx
# 	push %cx
# 	push %r8
# 	push %r9
# 	push %r10
# 	push %r11
# 	# pushf
# 	# cld
# 	call int_dmmy_srv_0
# 	# popad
# 	pop %r11
# 	pop %r10
# 	pop %r9
# 	pop %r8
# 	pop %cx
# 	pop %dx
# 	pop %si
# 	pop %di
# 	pop %ax
# 	# TODO : iretq ?
# 	sti
# 	iretq

# .globl dummy_interrupt_handler_1
# .align 4
 
# dummy_interrupt_handler_1:
# 	# pushad
# 	# TODO : save rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11
# 	cli
# 	push %ax
# 	push %di
# 	push %si
# 	push %dx
# 	push %cx
# 	push %r8
# 	push %r9
# 	push %r10
# 	push %r11
# 	# pushf
# 	# cld
# 	call int_dmmy_srv_1
# 	# popad
# 	pop %r11
# 	pop %r10
# 	pop %r9
# 	pop %r8
# 	pop %cx
# 	pop %dx
# 	pop %si
# 	pop %di
# 	pop %ax
# 	# TODO : iretq ?
# 	sti
# 	iretq

.globl keyboard_interrupt_handler
.align 4
keyboard_interrupt_handler:
	# pushad
	# pushf
	cli
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
	call int_kbd_srv
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
	
	sti
	iretq
