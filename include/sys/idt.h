#ifndef _IDT_H
#define _IDT_H

#include <sys/defs.h>
#include <sys/pic_helpers.h>
#include <sys/sbunix.h>
#include <sys/process_helpers.h>
#include <sys/memory_helpers.h>

#define IDT_SIZE 256

typedef struct IDTDescriptor{
	uint16_t offset_low;
	uint16_t selector;
	unsigned zero1:8;
	unsigned type_attr:8; 
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t zero2;
}IDTDescriptor;

typedef struct exception_pushed_stack_structure{
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t cx;
	uint64_t dx;
	uint64_t si;
	uint64_t di;
	uint64_t ax;
	uint64_t error;
}exception_stack;


void set_isr(IDTDescriptor* idt, int int_num, uint64_t handler){
	idt[int_num].offset_low = (uint16_t)(handler & 0x00ffff);
	idt[int_num].offset_mid = (uint16_t)((handler>>16) & 0x00ffff);
	idt[int_num].offset_high= (uint32_t)((handler>>32) & 0x00ffffffff);

	idt[int_num].zero1= 0;
	idt[int_num].zero2= 0;
	idt[int_num].selector= 0x0008;
	idt[int_num].type_attr= 0x8E;
}

void keyboard_interrupt_handler(void);
void int_kbd_srv(){
	unsigned char l = inb(0x60);
	put_pressed_key(l);
	// printf("%x\n", l);
	outb(0x20,0x20);
}

void timer_interrupt_handler(void);
void int_tmr_srv(){
	unsigned char l = inb(0x60);
	put_pressed_key(l);
	// printf("%x\n", l);
	outb(0x20,0x20);
}

void syscall_interrupt_handler(void);
void int_syscall_srv(){
	printf("SYSCALL INTERRUPT NOT IMPLEMENTED\n");
	while(1);
}

void protection_failure_interrupt_handler(void);
void int_prtctn_srv(exception_stack stack){
	uint64_t address = 0;
	uint32_t error = stack.error;

	__asm__ volatile("movq %%cr2, %0":"=r"(address):);

	printf("(general protection failure): %x: %x\n",address, error);
	while(1);
	// TODO: 
	// k_process_exit();
	return;	
}

void double_fault_interrupt_handler(void);
void int_dblflt_srv(exception_stack stack){
	uint64_t address = 0;
	uint32_t error = stack.error;

	__asm__ volatile("movq %%cr2, %0":"=r"(address):);

	printf("(double_fault):%x, %x\n", address, error);
	// while(1);
	// TODO: 
	// k_process_exit();
	// return;	
}

void pagefault_interrupt_handler(void);
void int_pgflt_srv(exception_stack stack){

	uint64_t address = 0;
	uint32_t error = stack.error;

	__asm__ volatile("movq %%cr2, %0":"=r"(address):);
	printf("(page fault):%x, %x\n", address, error);
	
	if(address== 0x00){
		printf("(segmantation fault)\n");
		while(1);
		// TODO : 
		// k_process_exit();
		// return;
	}
	// TODO : COW pages
	if((error&2)==2 || (error&5)==4) // PAGE NOT MAPPED
		add_physical_page_in(address);
	else{
		printf("(page fault - not handled):%x, %x\n", address, error);
		while(1);
	}
}


IDTDescriptor idt [IDT_SIZE];


void idts_setup(){
	
	
	struct
	{
		uint16_t length;
		uint64_t base;
	} __attribute__((packed)) IDTR;

	IDTR.base = (uint64_t)(&idt); IDTR.length = sizeof(IDTDescriptor)*IDT_SIZE;

	// printf("idtent  SIZE [%d]\n", sizeof(IDTDescriptor));
	printf("IDTR.base [%x] , IDTR.length [%d]\n", IDTR.base, IDTR.length);


	set_isr(idt, 0x08, (uint64_t)(&double_fault_interrupt_handler));
	set_isr(idt, 0x0d, (uint64_t)(&protection_failure_interrupt_handler));
	set_isr(idt, 0x0e, (uint64_t)(&pagefault_interrupt_handler));
	set_isr(idt, 0x20, (uint64_t)(&timer_interrupt_handler));
	set_isr(idt, 0x21, (uint64_t)(&keyboard_interrupt_handler));
	set_isr(idt, 0x80, (uint64_t)(&syscall_interrupt_handler));
	// set_isr(idt, 0x100, (uint64_t)(&syscall_interrupt_handler));


	IDTDescriptor* l = (IDTDescriptor*)(IDTR.base);

	printf("entry[0].offset [%x|%x|%x]\n", l[0].offset_high, l[0].offset_mid, l[0].offset_low );

	__asm__ ( "lidt (%0)" : : "r"(&IDTR) );
	
}





#endif
