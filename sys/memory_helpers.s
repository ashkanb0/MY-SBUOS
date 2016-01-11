.text
# .global _set_cr3

# _set_cr3:
# 	movq %rdi, %rax
# 	movq %rax, %cr3
# 	retq

# .global _enable_paging
# _enable_paging:
# 	push eax
# 	mov eax, cr0
# 	or eax, 1 << 31
# 	or eax, 1 << 0
# 	mov cr0, eax
# 	pop eax
# 	ret

