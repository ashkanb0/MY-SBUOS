#ifndef _SYS_MEM_H
#define _SYS_MEM_H

#include <sys/defs.h>

// 4 K
#define PAGESIZE 4096
#define KERNEL_MAPPING 0xffffffff80000000

typedef struct memory_page_linked_list
{
	uint64_t base;
	struct memory_page_linked_list* next;
}mem_page;


void * init_pages(void* physfree);
void self_map_filtered_out_pages(void);
void setup_paging();
void filter_out_pages(uint64_t base, uint64_t top);


void * make_pages(uint64_t base, uint64_t length, void * physfree);

#endif