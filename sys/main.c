#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/tarfs.h>
#include <sys/memory_helpers.h>

// #include <sys/pic_helpers.h>


void start(uint32_t* modulep, void* physbase, void* physfree)
{
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	
	physfree = init_pages(physfree);
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
			physfree = make_pages(smap->base, smap->length, physfree);
			printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
		}
	}

	// physfree should point to last used address in kernel by now,
	// update accordingly up until here
	filter_out_pages((uint64_t)physbase - PAGESIZE, (uint64_t)physfree); // kernel
	filter_out_pages(0xb8000 - PAGESIZE, 0xbb200); // mem-mapped display // TODO: is this correct?

	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	init_tarfs(&_binary_tarfs_start, &_binary_tarfs_end);

	// logging
	uint64_t _tar_start = (uint64_t) &_binary_tarfs_start;
	// uint64_t _tar_end = (uint64_t) &_binary_tarfs_end;
	uint64_t offset = 0;

	for (int i = 0; i < 10; ++i)
	{
		tarfs_header* p = (tarfs_header *) (_tar_start+offset);
		printf("name: %s, size: %s = %x\n", p->name, p->size, atoi(12, p->size));
		offset += atoi(12, p->size)+(sizeof(tarfs_header)==0?0:28754);
	}
	// kernel starts here
	idts_setup();
	PIC_setup();

	__asm__ volatile("sti");

	while(1){

	}
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	register char *s, *v;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	reload_gdt();
	setup_tss();
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	s = "!!!!! start() returned !!!!!";
	for(v = (char*)0xb8000; *s; ++s, v += 2) *v = *s;
	while(1);
}
