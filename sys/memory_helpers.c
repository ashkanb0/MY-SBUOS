#include <sys/sbunix.h>
#include <sys/memory_helpers.h>


// LINKED LIST OF FREE PAGES
mem_page* _free_page_list_head = NULL;
mem_page* _free_page_list_tail = NULL;

// mem_page* _filtered_page_list_head = NULL;
// mem_page* _filtered_page_list_tail = NULL;

mem_page* _used_page_list_head = NULL;
mem_page* _used_page_list_tail = NULL;

mem_page* kernel_pml4 = NULL;

void * init_pages(void* physfree){
	mem_page* dummy_hdr = (mem_page*) physfree;

	dummy_hdr[0].base = 0;
	dummy_hdr[0].next = NULL;

	_free_page_list_head = _free_page_list_tail = dummy_hdr;

	// dummy_hdr[1].base = 0;
	// dummy_hdr[1].next = NULL;

	// _filtered_page_list_head = _filtered_page_list_tail = dummy_hdr + 1;
	
	return dummy_hdr + 1;// number of allocated
	// return dummy_hdr + 2;// number of allocated

}


void zero_out(mem_page* page){
	char* bt = (char*) page-> base;
	for (int i = 0; i < PAGESIZE; ++i)
		bt[i] = 0;
}


mem_page* dequeue_page(mem_page** list_head, mem_page** list_tail){
	if (*list_head == *list_tail)
		return NULL;
	mem_page* res = (*list_head) -> next;
	(*list_head) -> next = res -> next;
	if (res -> next == NULL)
		*list_tail = *list_head;
	return res;
	
}

void add_page(mem_page* page, mem_page** list_tail){
	page -> next = NULL;
	(*list_tail) -> next = page;
	*list_tail = page;
}

mem_page* get_free_page(){
	mem_page* res = dequeue_page(&_free_page_list_head, &_free_page_list_tail);
	if (res != NULL){
		add_page(res, &_used_page_list_tail);
	}
	printf("page being taken: %x\n", res->base);
	return res;
}

void map_v(uint64_t phys, uint64_t virt, uint64_t* table, int lvl){
	uint64_t index = (0x01ff & (virt>> (12 + (lvl-1)*9))); 
	
	if (lvl == 1){
		table[index] = (phys & 0xffffffffff000) | 3;
		return;
	}

	if(table[index] == 0){
		// create it!
		mem_page* next_lvl_page = get_free_page();
		zero_out(next_lvl_page);
		table[index] = ((uint64_t)next_lvl_page->base| 7); 
	}
	map_v(phys, virt, (uint64_t*)table[index], lvl - 1);
	
}

uint64_t mem_map_v(uint64_t base, uint64_t end, uint64_t vrtlmm, uint64_t table){
	for(; base<end; base+=PAGESIZE){
		map_v(base, vrtlmm, (uint64_t*)table, 4);
		vrtlmm+=PAGESIZE;
	}
	return vrtlmm;
}



void filter_out_pages(uint64_t base, uint64_t top){
	mem_page * p;

	for(mem_page* curr = _free_page_list_head -> next; curr; curr= curr->next){
		p = curr->next;
		while( p-> base >= base && p-> base < top){
			curr -> next = p-> next;
			p-> next = NULL;
			// add_page(p, &_filtered_page_list_tail);
			p = curr->next;

		}
	}
}

void * make_pages(uint64_t base, uint64_t length, void * physfree){
	// JK :))))
	// if(base==0)return physfree;
	uint64_t free_int = (uint64_t) physfree;
	free_int = free_int%PAGESIZE == 0? free_int : free_int + PAGESIZE - (free_int% PAGESIZE);
	// I'll throw this out! 

	mem_page* page = (mem_page*) free_int;
	for(uint64_t index = base; index<base+length; index+= PAGESIZE){
		page -> base = index;
		page -> next = NULL;
		add_page(page, &_free_page_list_tail);
		page ++;
	}
	return page;
}



static inline uint64_t _read_cr0(void)
{
    uint64_t val;
    __asm__ volatile ( "movq %%cr0, %0" : "=r"(val) );
    return val;
}

static inline void _set_cr3(uint64_t table){
	__asm__ volatile("movq %0, %%cr3"::"r"(table):);
}
static inline void _set_cr0(uint64_t table){
	__asm__ volatile("movq %0, %%cr0"::"g"(table):);
}

void setup_paging(
	void* physbase, void* physfree, 
	uint64_t displaybase, uint64_t displayfree, 
	void* kernel_virtual){
	
	uint64_t kernel_vrt = (uint64_t)kernel_virtual;

	kernel_pml4 = get_free_page();
	zero_out(kernel_pml4);

	// self referencing entry
	uint64_t* table = (uint64_t*) kernel_pml4-> base;
	table[511] = kernel_pml4-> base;


	kernel_vrt =  mem_map_v((uint64_t)physbase, (uint64_t)physfree, kernel_vrt, kernel_pml4->base);
	mem_map_v((uint64_t)displaybase, (uint64_t)displayfree, kernel_vrt, kernel_pml4->base);

	set_display_address(kernel_vrt| KERNEL_MAPPING);


	_free_page_list_head = (mem_page*)((uint64_t)_free_page_list_head|KERNEL_MAPPING);
	_free_page_list_tail = (mem_page*)((uint64_t)_free_page_list_tail|KERNEL_MAPPING);
	_used_page_list_head = (mem_page*)((uint64_t)_used_page_list_head|KERNEL_MAPPING);
	_used_page_list_tail = (mem_page*)((uint64_t)_used_page_list_tail|KERNEL_MAPPING);

	_set_cr3((uint64_t)kernel_pml4->base);

	// enabling paging
	uint64_t cr0= _read_cr0();
	cr0 |= 0x080000000;
	_set_cr0(cr0);

}

