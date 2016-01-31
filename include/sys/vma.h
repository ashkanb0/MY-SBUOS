#ifndef _SYS_VMA_H
#define _SYS_VMA_H

#include <sys/defs.h>
#include <sys/memory_helpers.h>



void vma_register_page(mem_page* page, uint64_t pid);
void mem_map_file(uint64_t paddr, uint64_t vaddr, uint64_t memsz, uint64_t filesz);


#endif
