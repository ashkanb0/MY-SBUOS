#ifndef _SYS_MEM_H
#define _SYS_MEM_H

#include <sys/defs.h>

// 4 K
#define PAGESIZE 4096
#define KERNEL_MAPPING 0xffffffff80000000

#define PRESENT 1
#define READ_WRITE 2
#define USER_ACCESSIBLE 4


typedef struct memory_page_linked_list
{
	uint64_t base;
	// TODO : ref_count?
	uint64_t next;
}mem_page;


void * init_pages(void* physfree);

void setup_paging();

void add_physical_page_in(uint64_t virt);

void * make_pages(uint64_t base, uint64_t length, void * physfree);

void* kmalloc(uint64_t no_bytes);
uint64_t get_new_page_table();
uint64_t get_new_page_v();

void _set_cr3(uint64_t table);
uint64_t get_active_pml4();
#endif
