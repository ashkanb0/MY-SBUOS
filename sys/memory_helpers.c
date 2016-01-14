#include <sys/sbunix.h>
#include <sys/memory_helpers.h>


// LINKED LIST OF FREE PAGES
mem_page* _page_list = NULL;


uint64_t _free_page_list_head = 0;
uint64_t _free_page_list_tail = 0;

uint64_t _used_page_list_head = 0;
uint64_t _used_page_list_tail = 0;

mem_page* kernel_pml4 = NULL;


uint64_t get_page_table(uint64_t virt, uint64_t lvl){
	printf("TURNING VIRTUAL   >%x<  on level %d           \n",virt , lvl);
	uint64_t mask = 0x0000ffffffffffff >> ((lvl) * 9);
	if (lvl==1){
		printf("TURNING VIRTUAL   >%x<  on level %d           \n",virt , lvl);
		mask = 0x0000007fffffffff;
	}
	virt =  ((virt & 0x0000fffffffff000) >> (lvl * 9)) & 0xfffffffffffff000 ;
	printf("virt >%x<        mask >%x<lvl %d              \n",virt , mask, lvl);
	uint64_t res = (0xffffff7fbfdfe000 & (~mask))| (virt & mask);
	printf("DONE WITH VIRTUAL >%x<                        \n",res);
	return res;
}

void * init_pages(void* physfree){
	_page_list = (mem_page*) physfree;
	// _free_page_list = ((mem_page*) physfree)+1;
	// _used_page_list = ((mem_page*) physfree)+2;

	_page_list[0].base = 0;
	_page_list[0].next = 0;
	// _free_page_list[0].base = 0;
	// _free_page_list[0].next = 0;
	// _used_page_list[0].base = 0;
	// _used_page_list[0].next = 0;

	_free_page_list_head = 0;
	_free_page_list_tail = 0;
	_used_page_list_head = 0;
	_used_page_list_tail = 0;

	return _page_list + 1;
	// return _free_page_list + 3;
}


void zero_out(mem_page* page){
	char* bt = (char*) page-> base;
	for (int i = 0; i < PAGESIZE; ++i)
		bt[i] = 0;
}


mem_page* dequeue_page(uint64_t* list_head, uint64_t* list_tail){
	if(*list_head == 0 && list_tail==0)
		return NULL;

	mem_page* page = (_page_list+*list_head);

	(*list_head) = page -> next;
	if (page -> next == 0)
		*list_tail = 0 ;
	return page;
	
}

void queue_page(mem_page* page, uint64_t* list_head, uint64_t* list_tail, uint64_t index){
	if (*list_head == 0 && *list_tail == 0){
		*list_head = index;
	}else{
		(_page_list + (*list_tail))->next = index;
	}
	(_page_list + index)->next = 0;
	*list_tail = index;
}

mem_page* get_free_page(){
	uint64_t list_index = _free_page_list_head;
	mem_page* res = dequeue_page(&_free_page_list_head, &_free_page_list_tail);
	if (res != NULL){
		queue_page(res, &_used_page_list_head, &_used_page_list_tail, list_index);
	}
	// printf("page being taken: %x\n", res->base);
	return res;
}

void map_v(uint64_t phys, uint64_t virt, uint64_t* table, int lvl){
	uint64_t index = (0x01ff & (virt>> (12 + (lvl-1)*9))); 
	
	if (lvl == 1){
		table[index] = (phys & 0xffffffffff000)|3;
		// table[index] = (phys & 0xffffffffff000);
		return;
	}

	if(table[index] == 0){
		// create it!
		// printf("creating page for index %x out of virtual %x in level %d\n",index, virt, lvl );
		mem_page* next_lvl_page = get_free_page();
		zero_out(next_lvl_page);
		table[index] = ((uint64_t)next_lvl_page->base|3); 
		// table[index] = ((uint64_t)next_lvl_page->base); 
	}
	// TODO : use virtual memory of (table[index])
	// map_v(phys, virt, (uint64_t*)(virt_mem(table[index] & ~3, lvl)), lvl - 1);
	map_v(phys, virt, (uint64_t*)(get_page_table(virt, lvl - 1)), lvl - 1);
	
}

uint64_t mem_map_v(uint64_t base, uint64_t end, uint64_t vrtlmm, uint64_t table){
	uint64_t res = vrtlmm;
	for(; base<end; base+=PAGESIZE){
		map_v(base, res, (uint64_t*)table, 4);
		res+=PAGESIZE;
	}
	return res;
}



void filter_out_pages(uint64_t base, uint64_t top){
	mem_page * curr = (_page_list+ _free_page_list_head);
	while(curr->base >= base && curr->base < top ){
		_free_page_list_head = curr -> next;
		curr = (_page_list+ _free_page_list_head);
		if (_free_page_list_head == 0){
			// SHOULD NOT HAPPEN <<<<EVER>>>> !
			printf("<<PANIC!: 01>>\n");
			//TODO : shut_down();
			while(1);
		}
	}

	for(uint64_t index = _free_page_list_head ;index!=0 ; index = curr->next){
		curr = (_page_list+ index);
		uint64_t next_index = curr -> next;
		mem_page * next = (_page_list+ next_index);
		while( next-> base >= base && next-> base < top){
			next_index =  curr -> next = next -> next;
			next -> next = 0;
			next = (_page_list+ next_index);
		}
	}
}

void * make_pages(uint64_t base, uint64_t length, void * physfree){
	// JK :))))
	if(base==0)return physfree;
	
	mem_page* page = (mem_page*) physfree;

	uint64_t list_index = 1;
	for(uint64_t index = base; index<base+length; index+= PAGESIZE, list_index++){
		page -> base = index;
		queue_page(page, &_free_page_list_head, &_free_page_list_tail, list_index);
		page ++;
	}
	return page;
}



void _set_cr3(uint64_t table){
	__asm__ volatile("movq %0, %%cr3\n\r"::"r"(table): );
}

uint64_t _read_cr0(){
	uint64_t res;
	__asm__ volatile("movq %%cr0, %0":"=r"(res):);
	return res;
}
uint64_t _read_cr3(){
	uint64_t res;
	__asm__ volatile("movq %%cr3, %0":"=r"(res):);
	return res;
}
uint64_t _read_cr4(){
	uint64_t res;
	__asm__ volatile("movq %%cr4, %0":"=r"(res):);
	return res;
}




void setup_paging(
	void* physbase, void* physfree, 
	uint64_t displaybase, uint64_t displayfree, 
	void* kernel_virtual){
	

	printf("%x, %x, %x\n", 0xffffffffff000, 0x0000007fffffffff, 0x7357abcd98767357);

	filter_out_pages((uint64_t)physbase, (uint64_t)physfree); // kernel
	filter_out_pages(0xb8000 - PAGESIZE, 0xbb200); // mem-mapped display // TODO: is this correct?
	

	uint64_t kernel_vrt = (uint64_t)kernel_virtual;

	kernel_pml4 = get_free_page();
	zero_out(kernel_pml4);

	// self referencing entry
	uint64_t* table = (uint64_t*) kernel_pml4-> base;
	table[510] = kernel_pml4-> base; // 511 is being used by kernel memory


	kernel_vrt =  mem_map_v((uint64_t)physbase, (uint64_t)physfree, kernel_vrt, kernel_pml4->base);
	mem_map_v((uint64_t)displaybase, (uint64_t)displayfree, kernel_vrt, kernel_pml4->base);

	// DEBUGGING, MAKE SURE TO REMOVE IT!!
	printf("CR0 %x\n", _read_cr0());
	printf("CR3 %x\n", _read_cr3());
	printf("CR4 %x\n", _read_cr4());



	// _free_page_list_head = _free_page_list_head|KERNEL_MAPPING;
	// _free_page_list_tail = _free_page_list_tail|KERNEL_MAPPING;
	// _used_page_list_head = _used_page_list_head|KERNEL_MAPPING;
	// _used_page_list_tail = _used_page_list_tail|KERNEL_MAPPING;

	_page_list = (mem_page *)(((uint64_t)_page_list)|KERNEL_MAPPING);

	set_display_address(kernel_vrt| KERNEL_MAPPING);

	// CHANEG PAGING
	// _disable_paging();
	_set_cr3((uint64_t)kernel_pml4->base);
	// _enable_paging();

	printf("HELLO PAGED WORLD!\n");
	// DEBUGGING, MAKE SURE TO REMOVE IT!!
	// enabling paging
	// _enable_paging();

}

