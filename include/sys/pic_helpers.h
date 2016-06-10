#ifndef _PIC_H
#define _PIC_H

#include <sys/defs.h>


#define PIC1			0x20
#define PIC2			0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI			0x20
#define ICW1_ICW4		0x01
#define ICW1_INIT		0x10
#define ICW4_8086		0x01
#define PIC_MASTER_OFFSET 0x20
#define PIC_SLAVE_OFFSET 0x28

#define TMR_CMD			0x43
#define TMR_INIT		0x43
#define TMR_DATA		0x43
// 1193180/32 = 37286
#define TMR_FRQ_H		0x91
#define TMR_FRQ_L		0xa6

static inline void outb(uint16_t port, uint8_t val)
{
	__asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	return ret;
}
static inline void io_wait(void)
{
	__asm__ volatile ( "outb %%al, $0x80" : : "a"(0) );
} 
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

void timer_setup(void){
	outb(TMR_CMD, TMR_INIT);
	io_wait();
	outb(TMR_DATA, TMR_FRQ_L);
	io_wait();
	outb(TMR_DATA, TMR_FRQ_H);
	io_wait();
}

///////// 
void PIC_setmasks(char master_masks, char slave_masks){
	outb(PIC1_DATA, master_masks);
	outb(PIC2_DATA, slave_masks);
}
void PIC_setup(void){
	PIC_remap(PIC_MASTER_OFFSET, PIC_SLAVE_OFFSET);
	// PIC_setmasks(0,0); // clear all masks
	PIC_setmasks(1,0); // clear all masks
	// PIC_setmasks(0xfd,0xff); // KEYBOARD ONLY
	// PIC_setmasks(0xf8,0xff); // KEYBOARD AND TIMER
}


#endif