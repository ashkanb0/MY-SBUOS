.globl fork_return_from_child
.align 4
fork_return_from_child:
	movq $0, %rax
	iretq
