#include <sys/systemcall.h>
#include <sys/syscall.h>
#include <sys/process_helpers.h>

uint64_t do_break (uint64_t size){
	pcb* proc = get_active_pcb();
	if (proc-> user_brk_point + size >= HEAP_TOP)
		return -1;
	proc -> user_brk_point += size;
	return proc-> user_brk_point;	
}

uint64_t do_system_call(uint64_t syscall_code, uint64_t arg1, uint64_t arg2, uint64_t arg3){
	
	switch(syscall_code)
	{
		case SYS_exit : printf("SYS_exit NOT IMPLEMENTED\n");
						while(1);
						break;
		case SYS_brk : return do_break(arg1);
						break;
		default : printf("SYSCALL NOT IMPLEMENTED: %d, 0x%x\n", syscall_code, syscall_code);
						while(1);
						break;

	}


}



