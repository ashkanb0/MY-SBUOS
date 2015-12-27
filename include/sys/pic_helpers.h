#ifndef _PIC_H
#define _PIC_H

#include <sys/defs.h>


#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */


#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
// #define ICW1_SINGLE	0x02		/* Single (cascade) mode */
// #define ICW1_INTERVAL4	0x04		 /*Call address interval 4 (8)*/ 
// #define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
// #define ICW4_AUTO	0x02		/* Auto (normal) EOI */
// #define ICW4_BUF_SLAVE	0x08		 /*Buffered mode/slave*/ 
// #define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
// #define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC_MASTER_OFFSET 0x08
#define PIC_SLAVE_OFFSET 0x70

// #define PIC_MASTER_OFFSET 0x20
// #define PIC_SLAVE_OFFSET 0x28



static inline void outb(uint16_t port, uint8_t val)
{
	__asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
	/* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
	/* TODO: Should %1 be %w1? */
}
static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	/* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
	/* TODO: Should %1 be %w1? */
	return ret;
}

static inline void io_wait(void)
{
	/* Port 0x80 is used for 'checkpoints' during POST. */
	/* The Linux kernel seems to think it is free for use :-/ */
	__asm__ volatile ( "outb %%al, $0x80" : : "a"(0) );
	/* TODO: Is there any reason why al is forced? */
}
 
void PIC_sendEOI(unsigned char irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);// save masks
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);// ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);// ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);// ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);// ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

///////// 

void PIC_setmasks(char master_masks, char slave_masks){
	outb(PIC1_DATA, master_masks);
	outb(PIC2_DATA, slave_masks);
}


void PIC_setup(void){
	PIC_remap(PIC_MASTER_OFFSET, PIC_SLAVE_OFFSET);
	PIC_setmasks(0,0); // clear all masks
}



#endif



