#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H

#include <sys/defs.h>


uint64_t do_system_call(syscall_code, arg1, arg2, agr3);

#endif