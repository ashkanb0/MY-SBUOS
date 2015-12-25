#include <sys/sbunix.h>

#include <sys/gdt.h>
#include <sys/tarfs.h>

void interrupt_handler(void){

}


void start(uint32_t* modulep, void* physbase, void* physfree)
{
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
			printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
		}
	}
	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	// kernel starts here
	struct
    {
        uint16_t length;
        uint32_t base;
    } __attribute__((packed)) IDTR;
 
    IDTR.length = (uint16_t) 5;
    IDTR.base   = (uint32_t) 0;
	printf("testing - before load [%p]\n", &IDTR);
    __asm__ ( "sidt (%0)" : : "r"(&IDTR) );

	printf("testing -after load [%p]\n", &IDTR);
	printf("testing -after load [%x][%x]\n", IDTR.length, IDTR.base);

	// for(long l = IDTR.base; l<IDTR.base+IDTR.length;l++){
	// for(long l = IDTR.base; l<IDTR.base+10;l++){
	// 	printf("%p -> [%x]\n", l, *((long*)(l)));
		
	// }

	// void (*l) (void);
	// l = &interrupt_handler;

	// long* place = (long*)((long)IDTR.base+2);

	// *place = (long)(l);

	// long l = IDTR.base+2;
	// (void (void) *)(l) = &interrupt_handler;

	__asm__ volatile("int $0");
	// __asm__ volatile("sti");
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
