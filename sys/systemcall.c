#include <sys/systemcall.h>
#include <sys/syscall.h>

uint64_t do_system_call(uint64_t syscall_code, uint64_t arg1, uint64_t arg2, uint64_t arg3){
	printf("syscall_code: %x, arg1, %x, arg2, %x, arg3, %x\n",syscall_code, arg1, arg2, arg3);
	
	switch(syscall_code)
	{
		case SYS_exit : printf("SYS_exit NOT IMPLEMENTED\n");
						while(1);
						break;
		case SYS_brk : printf("SYS_brk NOT IMPLEMENTED\n");
						while(1);
						break;
		default : printf("SYSCALL NOT IMPLEMENTED: %d, 0x%x\n", syscall_code, syscall_code);
						while(1);
						break;

	}


}



