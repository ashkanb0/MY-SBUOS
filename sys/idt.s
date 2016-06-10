.text

.globl keyboard_interrupt_handler
.align 4
keyboard_interrupt_handler:
	# pushad
	# pushf
	cli
	push %rax
	push %rdi
	push %rsi
	push %rdx
	push %rcx
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
	pop %rcx
	pop %rdx
	pop %rsi
	pop %rdi
	pop %rax
	
	sti
	iretq


.globl timer_interrupt_handler
.align 4
timer_interrupt_handler:
	# pushad
	# pushf
	cli
	push %rax
	push %rdi
	push %rsi
	push %rdx
	push %rcx
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
	pop %rcx
	pop %rdx
	pop %rsi
	pop %rdi
	pop %rax
	
	sti
	iretq

.globl syscall_interrupt_handler
.align 4
syscall_interrupt_handler:
	# pushad
	# pushf
	cli
	push %rax
	push %rdi
	push %rsi
	push %rdx
	push %rcx
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
	pop %rcx
	pop %rdx
	pop %rsi
	pop %rdi
	pop %rax
	
	sti
	iretq

.globl protection_fault_interrupt_handler
.align 4
protection_fault_interrupt_handler:
	# pushad
	# pushf
	cli
	push %rax
	push %rdi
	push %rsi
	push %rdx
	push %rcx
	push %r8
	push %r9
	push %r10
	push %r11
	# pushf
	# cld
	call int_prtctn_srv
	# popad
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rcx
	pop %rdx
	pop %rsi
	pop %rdi
	pop %rax
	add $0x08, %rsp # popping error code,
	
	sti
	iretq


.globl pagefault_interrupt_handler
.align 4
pagefault_interrupt_handler:
	# pushad
	# pushf
	cli
	push %rax
	push %rdi
	push %rsi
	push %rdx
	push %rcx
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
	pop %rcx
	pop %rdx
	pop %rsi
	pop %rdi
	pop %rax
	add $0x08, %rsp # popping error code,
	sti
	iretq

.globl double_fault_interrupt_handler
.align 4
double_fault_interrupt_handler:
	# pushad
	# pushf
	cli
	push %rax
	push %rdi
	push %rsi
	push %rdx
	push %rcx
	push %r8
	push %r9
	push %r10
	push %r11
	# pushf
	# cld
	call int_dblflt_srv
	# popad
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rcx
	pop %rdx
	pop %rsi
	pop %rdi
	pop %rax
	add $0x08, %rsp # popping error code,
	sti
	iretq
