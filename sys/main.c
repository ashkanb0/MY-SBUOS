#include <sys/sbunix.h>

#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/tarfs.h>

void interrupt_handler(void){
	int jafar = 0;
	jafar = 50/10;
	if(jafar<5){
		jafar = 5;
	}
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

	IDTDescriptor* idt = (IDTDescriptor*)((uint64_t)IDTR.base);

	printf("test for idts : [%x]\n", ((uint64_t)idt[0].offset_low)|
										(((uint64_t)idt[0].offset_mid)<<16)|
										(((uint64_t)idt[0].offset_high)<<32));


	printf("address: [%x]\n", (uint64_t)(&interrupt_handler));
	set_isr((uint64_t)IDTR.base, 0, (uint64_t)(&interrupt_handler));
	
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
