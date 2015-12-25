#ifndef _IDT_H
#define _IDT_H

#include <sys/defs.h>
#include <sys/pic_helpers.h>
#include <sys/sbunix.h>


typedef struct IDTDescriptor{
	uint16_t offset_low;
	uint16_t selector;
	// uint8_t zero;
	// uint8_t type_attr; 
	uint16_t type_attr; 
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t zero2;
}IDTDescriptor;


void set_isr(uint32_t base, int int_num, uint64_t handler){
	IDTDescriptor* idt = (IDTDescriptor*)((uint64_t)base);
	idt[int_num].offset_low = (uint16_t)(handler & 0x00ffff);
	idt[int_num].offset_mid = (uint16_t)((handler>>16) & 0x00ffff);
	idt[int_num].offset_high= (uint32_t)((handler>>32) & 0x00ffffffff);
}


// void keyboard_interrupt_handler(void);





void keyboard_interrupt_handler(void){
	char l = inb(0x60);
	printf("hit: %x\n", l);
	outb(0x20, 0x20);
}

void interrupt_handler(void){
	int jafar = 0;
	jafar = 50/10;
	if(jafar<5){
		jafar = 5;
	}
	printf("HEREEEEEEEEEEEEEE\n");
}


void idts_setup(){
	
	
	struct
	{
		uint16_t length;
		uint64_t base;
	} __attribute__((packed)) IDTR;

	IDTR.base = 0; IDTR.length = 0;
	__asm__ ( "sidt (%0)" : : "r"(&IDTR) );

	printf("IDTR.base [%x]\n", IDTR.base);

	set_isr((uint64_t)IDTR.base, 0, (uint64_t)(&interrupt_handler));
	set_isr((uint64_t)IDTR.base, 1, (uint64_t)(&keyboard_interrupt_handler));

	IDTDescriptor* l = (IDTDescriptor*)(IDTR.base);

	printf("entry[0].offset [%x|%x|%x]\n", l[0].offset_high, l[0].offset_mid, l[0].offset_low );

	
}





#endif
