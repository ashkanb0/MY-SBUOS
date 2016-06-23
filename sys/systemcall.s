.globl fork_return_from_child
.align 4
fork_return_from_child:
	cli
	# pop %r11
	# pop %r10
	# pop %r9
	# pop %r8
	# pop %rcx
	popq %rdx
	popq %rsi
	popq %rdi
	popq %rax
	movq $0, %rax

	sti

	iretq
