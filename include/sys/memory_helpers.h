#ifndef _SYS_MEM_H
#define _SYS_MEM_H

// 4 K
#define PAGESIZE 4096

typedef struct memory_page_linked_list
{
	uint64_t base;
	struct memory_page_linked_list* next;
}mem_page;


// LINKED LIST OF FREE PAGES
mem_page* head = NULL;
mem_page* tail = NULL;

// NO REAL USE FOR THIS!!!
uint64_t page_count = 0;

void * init_pages(void* physfree){
	mem_page* p = (mem_page*) physfree;

	p -> base = 0;
	p -> next = NULL;

	head = tail = p;

	
	return p + 1;
}

void add_page(uint64_t base, mem_page* page){
	page -> base = base;
	page -> next = NULL;

	tail -> next = page;
	tail = page;

	page_count ++;
}

void * make_pages(uint64_t base, uint64_t length, void * physfree){
	mem_page* page = (mem_page*) physfree;
	for(uint64_t offset = 0; offset<length; offset+= PAGESIZE){
		add_page(base+offset, page);
		page ++;
	}
	return page;
}

#endif