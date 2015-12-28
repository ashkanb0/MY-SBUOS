#ifndef _IDT_H
#define _IDT_H

#include <sys/defs.h>
#include <sys/pic_helpers.h>
#include <sys/sbunix.h>

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
	printf("INT ???\n");
	// char l = inb(0x60);
	// printf("hit: %x\n", l);
}


// TODO : OFF WITH THESE!!!!
void dummy_interrupt_handler_0(void);
void dummy_interrupt_handler_1(void);

void int_dmmy_srv_0(){
	printf("Unimplemented Interrupt 0!\n");
	outb(0x20,0x20);
}
void int_dmmy_srv_1(){
	printf("Unimplemented Interrupt 1!\n");
	outb(0x20,0x20); outb(0xa0,0x20);
}

IDTDescriptor idt [IDT_SIZE];


void idts_setup(){
	
	
	struct
	{
		uint16_t length;
		uint64_t base;
	} __attribute__((packed)) IDTR;

	IDTR.base = (uint64_t)(&idt); IDTR.length = sizeof(IDTDescriptor)*IDT_SIZE;

	printf("idtent  SIZE [%d]\n", sizeof(IDTDescriptor));
	printf("IDTR.base [%x] , IDTR.length [%d]\n", IDTR.base, IDTR.length);


	set_isr(idt, 1, (uint64_t)(&dummy_interrupt_handler_2));



	IDTDescriptor* l = (IDTDescriptor*)(IDTR.base);

	printf("entry[0].offset [%x|%x|%x]\n", l[0].offset_high, l[0].offset_mid, l[0].offset_low );

	__asm__ ( "lidt (%0)" : : "r"(&IDTR) );
	
}





#endif
