.globl   interrupt_handler
.align   4
 
interrupt_handler:
	# pushad
	# cld
	call do_something
	# popad
	iretq