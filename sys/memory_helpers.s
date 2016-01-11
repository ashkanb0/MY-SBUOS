.text


# .global _enable_paging
# _enable_paging:
# 	push eax
# 	mov eax, cr0
# 	or eax, 1 << 31
# 	or eax, 1 << 0
# 	mov cr0, eax
# 	pop eax
# 	ret

