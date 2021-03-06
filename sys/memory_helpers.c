#include <sys/sbunix.h>
#include <sys/memory_helpers.h>
#include <sys/process_helpers.h>
#include <sys/vma.h>

// LINKED LIST OF FREE PAGES
mem_page* _page_list = NULL;


uint64_t _free_page_list_head = 0;
uint64_t _free_page_list_tail = 0;

uint64_t _used_page_list_head = 0;
uint64_t _used_page_list_tail = 0;

uint64_t _available_virt_mem;

char* _kmalloc_page = NULL;
uint64_t _kmalloc_index = 0;

void copy_kernel_pml4(uint64_t prog_page_table){
	uint64_t* kernel_pml4 = (uint64_t*) (0xffffff7fbfdfe000);
	uint64_t* prog_pml4 = (uint64_t*) (prog_page_table);

	for (int i = 0; i < 512; ++i)
		prog_pml4[i] = kernel_pml4[i];

}

uint64_t get_active_pml4(){
	uint64_t* active_pml4 = (uint64_t*) (0xffffff7fbfdfe000);
	return active_pml4[510];
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


void zero_out(uint64_t address){
	char* bt = (char*) address;
	for (int i = 0; i < PAGESIZE; ++i)
		bt[i] = 0;
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
// uint64_t* get_vitrual_address(uint64_t virt){
// 	uint64_t mask = 0xffffff0000000000;
// 	virt =  (virt  >>  9) & 0x0000007ffffff000 ;
// 	uint64_t* page_table = (uint64_t) virt | mask;
// 	uint64_t index = (0x01ff & (virt>> 12 )); 
	
// 	return (uint64_t*)(page_table[index]);
// }

uint64_t map_page (uint64_t phys, uint64_t virt, uint64_t flags, uint64_t pid){
	uint64_t* table = (uint64_t*) (0xffffff7fbfdfe000);
	uint64_t index;
	int new_page_flag = 0;
	for (int lvl = 4; lvl>1; lvl--){
		index = (0x01ff & (virt>> (12 + (lvl-1)*9)));
	 	if(new_page_flag) zero_out((uint64_t)table);
	 	new_page_flag = 0;
	 	if(table[index] == 0){
			// create it!
			mem_page* next_lvl_page = get_free_page();
			vma_register_page(next_lvl_page, pid);
			table[index] = ((uint64_t)next_lvl_page->base|PRESENT|READ_WRITE|USER_ACCESSIBLE); 
			new_page_flag = 1;
		}
		table = (uint64_t*)((((uint64_t)(table))|0xffffff8000000000| (index<<3))<<9);
	}
	index = (0x01ff & (virt >> 12 ));
	table [index] = (phys & 0xffffffffff000)|flags;

	return virt+ PAGESIZE;
}

void add_physical_page_in(uint64_t virt){
	mem_page* pg = get_free_page();
	int pid = get_active_pid();
	vma_register_page(pg, pid);
	uint64_t flags = PRESENT|READ_WRITE|USER_ACCESSIBLE;
	map_page(pg->base, virt& 0xfffffffffffff000, flags, pid);
}


uint64_t get_new_page_v(uint64_t pid){
	mem_page* mal = get_free_page();
	vma_register_page(mal, pid);
	uint64_t temp = _available_virt_mem;
	_available_virt_mem = map_page(mal->base, _available_virt_mem, PRESENT|READ_WRITE|USER_ACCESSIBLE, pid);
	return temp;
}

void * kmalloc(uint64_t no_bytes){
	if (no_bytes > PAGESIZE) return NULL;
	if (_kmalloc_page == NULL || _kmalloc_index + no_bytes >= PAGESIZE){
		mem_page* mal = get_free_page();
		_kmalloc_page = (char*)_available_virt_mem;
		_available_virt_mem = map_page(mal->base, _available_virt_mem, PRESENT|READ_WRITE, 0);
		_kmalloc_index = 0;
	}

	char* res = _kmalloc_page + _kmalloc_index;
	_kmalloc_index += no_bytes; 
	return (void* ) res;
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



void k_map_v(uint64_t phys, uint64_t virt, uint64_t* table, int lvl){
	uint64_t index = (0x01ff & (virt>> (12 + (lvl-1)*9))); 
	
	if (lvl == 1){
		table[index] = (phys & 0xffffffffff000)|PRESENT|READ_WRITE;
		// table[index] = (phys & 0xffffffffff000);
		return;
	}

	if(table[index] == 0){
		// create it!
		mem_page* next_lvl_page = get_free_page();
		zero_out(next_lvl_page->base);
		table[index] = ((uint64_t)next_lvl_page->base|PRESENT|READ_WRITE|USER_ACCESSIBLE); 
	}
	k_map_v(phys, virt, (uint64_t*)(table[index] & 0xffffffffff000), lvl - 1);

}

uint64_t k_mem_map_v(uint64_t base, uint64_t end, uint64_t vrtlmm, uint64_t table){
	uint64_t res = vrtlmm;
	for(; base<end; base+=PAGESIZE){
		k_map_v(base, res, (uint64_t*)table, 4);
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
	__asm__ volatile("movq %0, %%cr3"::"r"(table): );
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


uint64_t get_new_page_table(int pid){
	mem_page* page = get_free_page();
	vma_register_page(page, pid);
	uint64_t temp = _available_virt_mem;
	_available_virt_mem = map_page(page->base, _available_virt_mem, PRESENT|READ_WRITE|USER_ACCESSIBLE, pid);


	zero_out(temp);
	copy_kernel_pml4(temp);

	// self referencing entry
	uint64_t* table = (uint64_t*) temp;
	table[510] = (page-> base) | PRESENT| READ_WRITE|USER_ACCESSIBLE;

	return page->base;
}

void setup_paging(
	void* physbase, void* physfree, 
	uint64_t displaybase, uint64_t displayfree, 
	void* kernel_virtual){
	
	mem_page* kernel_pml4 = NULL;
	

	filter_out_pages((uint64_t)physbase, (uint64_t)physfree); // kernel
	filter_out_pages(0xb8000 - PAGESIZE, 0xbb200); // mem-mapped display // TODO: is this correct?
	

	uint64_t kernel_vrt = (uint64_t)kernel_virtual;

	kernel_pml4 = get_free_page();
	zero_out(kernel_pml4->base);

	((uint64_t*)(kernel_pml4->base))[0x1fe] = kernel_pml4->base|PRESENT|READ_WRITE|USER_ACCESSIBLE;

	kernel_vrt = k_mem_map_v((uint64_t)physbase, (uint64_t)physfree, kernel_vrt, kernel_pml4->base);
	set_display_address(kernel_vrt| KERNEL_MAPPING);
	kernel_vrt = k_mem_map_v(displaybase, displayfree, kernel_vrt, kernel_pml4->base);


	_page_list = (mem_page *)(((uint64_t)_page_list)|KERNEL_MAPPING);
	_available_virt_mem = kernel_vrt;

	// CHANEG PAGING
	_set_cr3((uint64_t)kernel_pml4->base);

	printf("HELLO PAGED WORLD!\n");

}

void _do_mark_COW(uint64_t* table_p, uint64_t* table_c, int pid, int lvl){
	if (lvl==1){
		for (int i = 0; i < 512; ++i)
		{
			if(table_p[i]== 0)continue;
			table_p[i] &= (~READ_WRITE);
			table_p[i] |= COW;
			table_c[i] = table_p[i];
		}
		return;
	}

	int count = (lvl==4)?509:512;

	for (int i = 0; i < count; ++i)
	{
		if(table_p[i]== 0)continue;
		mem_page* next_lvl_page = get_free_page();
		vma_register_page(next_lvl_page, pid);
		table_c[i] = (next_lvl_page->base|PRESENT|READ_WRITE|USER_ACCESSIBLE);
		_do_mark_COW(
			(uint64_t*)((((uint64_t)(table_p))|0xffffff8000000000| (i<<3))<<9),
			(uint64_t*)((((uint64_t)(table_c))|0xffffff8000000000| (i<<3))<<9),
			pid, lvl - 1);
	}
}

void mark_COW(int pid){
	_do_mark_COW((uint64_t*)(0xffffff7fbfdfe000), (uint64_t*)(0xffffff7fbfdfd000), pid, 4);
	return;
}

void cross_off_COW(uint64_t virt){
	uint64_t temp;
	uint64_t* ptr;

	ptr = (uint64_t*)(0xffffff7fbfdfe000);
	if (ptr[509]==0)return;
	

	virt &= 0xfffffffffffff000;
	virt |= 0xffff000000000000;
	virt &= 0xfffdffffffffffff;

	ptr = (uint64_t*)( (virt>>9)| 0xffff000000000000);

	temp = *ptr;
	temp &= (~COW);
	temp |= READ_WRITE;
	*ptr = temp;
}

char temp_page [PAGESIZE];

void map_page_COW (uint64_t phys, uint64_t v_addr, uint64_t flags, uint64_t current_table){
	uint64_t virt = v_addr;

	virt &= 0xfffffffffffff000;

	char* temp_ptr = (char*)(virt);
	for (int i = 0; i < PAGESIZE; ++i)
		temp_page[i] = temp_ptr[i];

	virt |= 0xffff000000000000;
	virt &= 0xfffeffffffffffff;
	uint64_t* ptr = (uint64_t*)( (virt>>9)| 0xffff000000000000);
	*ptr = phys| flags;

	_set_cr3(current_table);

	virt = v_addr & 0xfffffffffffff000;
	temp_ptr = (char*)(virt);
	for (int i = 0; i < PAGESIZE; ++i)
		temp_ptr[i] = temp_page[i];

}

void do_COW(pcb* proc, uint64_t v_addr){
	mem_page* pg = get_free_page();
	vma_register_page(pg, proc->pid);
	map_page_COW (pg->base, v_addr, USER_ACCESSIBLE|READ_WRITE|PRESENT, proc->pml4);
}

void handle_COW(uint64_t address){
	pcb* proc = get_active_pcb();
	if (proc-> ppid == 0){	
		// TODO : what about multiple forks?
		pcb* child;
		child = find_pcb_by_ppid(proc->pid);
		if (child == NULL){
			cross_off_COW(address);
			return;
		}
	}
	else{
		pcb* parent;
		parent = find_pcb_by_pid(proc->ppid);
		if (parent == NULL){
			cross_off_COW(address);
			return;
		}
	}
	do_COW(proc, address);
	cross_off_COW(address);
}


void mark_fork_cross_entry(uint64_t parent_pml4, uint64_t child_pml4){
	uint64_t* table = (uint64_t*)(0xffffff7fbfdfe000);
	table[509] =  child_pml4 | READ_WRITE| PRESENT| USER_ACCESSIBLE;
	table = (uint64_t*)(0xffffff7fbfdfd000);
	table[509] = parent_pml4 | READ_WRITE| PRESENT| USER_ACCESSIBLE;
}
