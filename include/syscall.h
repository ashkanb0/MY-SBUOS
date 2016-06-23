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
	__asm volatile (
		"pushq %%rax\n\t"
		"pushq %%rdi\n\t"
		"pushq %%rsi\n\t"
		"pushq %%rdx\n\t"
		"movq %1, %%rax\n\t"
		"int $0x80\n\t"
		"movq %%rax, %0\n\t"
		"popq %%rdx\n\t"
		"popq %%rsi\n\t"
		"popq %%rdi\n\t"
		"popq %%rax\n\t"
		 : "=r" (res)
		 : "r" (n)
		 );
	return res;
}

static __inline int64_t syscall_1(uint64_t n, uint64_t a1) {
	int64_t res;
	__asm volatile (
		"pushq %%rax\n\t"
		"pushq %%rdi\n\t"
		"pushq %%rsi\n\t"
		"pushq %%rdx\n\t"
		"movq %1, %%rax\n\t"
		"movq %2, %%rdi\n\t"
		"int $0x80\n\t"
		"movq %%rax, %0\n\t"
		"popq %%rdx\n\t"
		"popq %%rsi\n\t"
		"popq %%rdi\n\t"
		"popq %%rax\n\t"
		 : "=r" (res)
		 : "r" (n), "r" (a1)
		 );
	return res;
}

static __inline int64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
	int64_t res;
	__asm volatile (
		"pushq %%rax\n\t"
		"pushq %%rdi\n\t"
		"pushq %%rsi\n\t"
		"pushq %%rdx\n\t"
		"movq %1, %%rax\n\t"
		"movq %2, %%rdi\n\t"
		"movq %3, %%rsi\n\t"
		"int $0x80\n\t"
		"movq %%rax, %0\n\t"
		"popq %%rdx\n\t"
		"popq %%rsi\n\t"
		"popq %%rdi\n\t"
		"popq %%rax\n\t"
		 : "=r" (res)
		 : "r" (n), "r" (a1), "r" (a2)
		 );
	return res;
}

static __inline int64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	int64_t res;
	__asm volatile (
		"pushq %%rax\n\t"
		"pushq %%rdi\n\t"
		"pushq %%rsi\n\t"
		"pushq %%rdx\n\t"
		"movq %1, %%rax\n\t"
		"movq %2, %%rdi\n\t"
		"movq %3, %%rsi\n\t"
		"movq %4, %%rdx\n\t"
		"int $0x80\n\t"
		"movq %%rax, %0\n\t"
		"popq %%rdx\n\t"
		"popq %%rsi\n\t"
		"popq %%rdi\n\t"
		"popq %%rax\n\t"
		 : "=r" (res)
		 : "r" (n), "r" (a1), "r" (a2), "r" (a3)
		 );
	return res;
}

#endif
