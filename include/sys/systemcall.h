#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H

#include <sys/defs.h>
#include <sys/sbunix.h>

void fork_return_from_child(void);

uint64_t do_system_call(uint64_t syscall_code, uint64_t arg1, uint64_t arg2, uint64_t arg3);

#endif