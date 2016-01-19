#ifndef _SYS_MEM_H
#define _SYS_MEM_H

#include <sys/defs.h>

// 4 K
#define PAGESIZE 4096
#define KERNEL_MAPPING 0xffffffff80000000

typedef struct memory_page_linked_list
{
	uint64_t base;
	uint64_t next;
}mem_page;


void * init_pages(void* physfree);

void setup_paging();

void * make_pages(uint64_t base, uint64_t length, void * physfree);

void* kmalloc(uint64_t no_bytes);
mem_page* get_new_page_table();

#endif
