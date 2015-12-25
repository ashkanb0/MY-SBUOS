#ifndef _IDT_H
#define _IDT_H

#include <sys/defs.h>


typedef struct IDTDescriptor{
	uint16_t offset_low;
	uint16_t selector;
	// uint8_t zero;
	// uint8_t type_attr; 
	uint16_t type_attr; 
	uint16_t offset_mid;
	uint32_t offset_high;
	char zero2;
}IDTDescriptor;


void set_isr(uint32_t base, int int_num, uint64_t handler){
	IDTDescriptor* idt = (IDTDescriptor*)((uint64_t)base);
	idt[int_num].offset_low = handler & 0xffff;
	idt[int_num].offset_mid = (handler>>16) & 0xffff;
	idt[int_num].offset_low = (handler>>32) & 0xffffffff;
}


#endif
