#include <sys/systemcall.h>

uint64_t do_system_call(syscall_code, arg1, arg2, agr3){
	printf("syscall_code: %x, arg1, %x, arg2, %x, arg3, %x\n",syscall_code, arg1, arg2, arg3);
	while(1);
	return 0;
}



