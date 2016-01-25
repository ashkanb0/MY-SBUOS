#ifndef _SYS_VMA_H
#define _SYS_VMA_H

#include <sys/defs.h>
#include <sys/memory_helpers.h>



void vma_register_page(mem_page* page, uint64_t pid);


#endif
