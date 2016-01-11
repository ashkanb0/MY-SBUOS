.text
# .global _set_cr3

# _set_cr3:
# 	movq %rdi, %rax
# 	movq %rax, %cr3
# 	retq

.global _enable_paging
_enable_paging:
	push %ax
	movq %cr0, %rax
	or $0x80000000, %eax
	movq %rax, %cr0
	pop %ax


.global _disable_paging
_disable_paging:
	push %ax
	movq %cr0, %rax
	and $0x7fffffff, %eax
	movq %rax, %cr0
	pop %ax
