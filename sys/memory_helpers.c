#include <sys/memory_helpers.h>

// LINKED LIST OF FREE PAGES
mem_page* head = NULL;
mem_page* tail = NULL;



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

}

void filter_out_pages(uint64_t base, uint64_t top){
	mem_page * p;

	for(mem_page* curr = head -> next; curr; curr= curr->next){
		p = curr->next;
		while( p-> base > base && p-> base < top){
			curr -> next = p-> next;
			p = curr->next;

		}
	}
}

void * make_pages(uint64_t base, uint64_t length, void * physfree){
	mem_page* page = (mem_page*) physfree;
	for(uint64_t offset = 0; offset<length; offset+= PAGESIZE){
		add_page(base+offset, page);
		page ++;
	}
	return page;
}
