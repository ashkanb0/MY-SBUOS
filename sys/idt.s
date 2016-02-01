.text

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


.globl timer_interrupt_handler
.align 4
timer_interrupt_handler:
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
	call int_tmr_srv
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

.globl syscall_interrupt_handler
.align 4
syscall_interrupt_handler:
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
	call int_syscall_srv
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

.globl d_interrupt_handler
.align 4
d_interrupt_handler:
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
	call int_d_srv
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
	add $0x04, %rsp # popping error code,
	
	sti
	iretq


.globl pagefault_interrupt_handler
.align 4
pagefault_interrupt_handler:
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
	call int_pgflt_srv
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
	add $0x04, %rsp # popping error code,
	sti
	iretq
