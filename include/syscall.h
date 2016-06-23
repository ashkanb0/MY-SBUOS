#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <sys/syscall.h>

struct pt_regs {
	unsigned long bx;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long bp;
	unsigned long ax;
	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;
	unsigned long orig_ax;
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
};

static __inline int64_t syscall_0(uint64_t n) {
	int64_t res;
	__asm volatile ("int $0x80"
		 : "=a" (res)
		 : "a" (n)
		 );
	return res;
}

static __inline int64_t syscall_1(uint64_t n, uint64_t a1) {
	int64_t res;
	__asm volatile ("int $0x80"
		 : "=a" (res)
		 : "a" (n), "D" (a1)
		 );
	return res;
}

static __inline int64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
	int64_t res;
	__asm volatile ("int $0x80"
		 : "=a" (res)
		 : "a" (n), "D" (a1), "S" (a2)
		 );
	return res;
}

static __inline int64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	// return 0;
	int64_t res;
	__asm volatile ("int $0x80"
		 : "=a" (res)
		 : "a" (n), "D" (a1), "S" (a2), "d" (a3)
		 );
	return res;
}

#endif
