#include <sys/sbunix.h>

#include <sys/gdt.h>
#include <sys/tarfs.h>

char hexnum(int i, int sh){
	i = (i>>sh)&0x000f;
	return (i<10)? '0'+i: 'A'+(i-10);
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
	int i = 0;
	register char *s, *v;
	char c;
	v = (char*)0xb8000; c=  'A';
	for (i=0;i<500;i++){
		*v=c;
		v+=2;
		c+=1;
		if (c>'Z') c= 'A';
	}
	i=0;
	while(1){
		i++;
		s = "we counted [  ] ";
		s[12] = hexnum(i, 20);
		s[13] = hexnum(i, 16);
		for(v = (char*)0xb8000; *s; ++s, v += 2) *v = *s;
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
