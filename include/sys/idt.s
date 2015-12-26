.globl   isr_wrapper
.align   4
 
isr_wrapper:
	# pushad
	# cld
	call interrupt_handler
	# popad
	iretq