#include <sys/systemcall.h>
#include <sys/syscall.h>
#include <sys/sysutil.h>
#include <sys/process_helpers.h>

uint64_t do_break (uint64_t arg1){
	pcb* proc = get_active_pcb();
	if (arg1 == 0)
		return proc -> user_brk_point;
	if (arg1 >= HEAP_TOP)
		return -1;
	proc -> user_brk_point = arg1;
	return proc-> user_brk_point;	
}

uint64_t do_getcwd (uint64_t buffer, uint64_t size){
	pcb* proc = get_active_pcb();
	int i = kstrcpy((char*)buffer, proc-> wd, size);
	if (i == -1) return (uint64_t)NULL;
	return buffer;
}

uint64_t do_write (uint64_t fd, uint64_t buffer, uint64_t size){
	if (fd==1){
		char* str = (char*)buffer;
		int i;
		for(i = 0; i<size && str[i]!='\0'; i++){
			write_k(str[i]);
		}
		return (uint64_t)i;
	}
	return (uint64_t)NULL;
}

uint64_t do_read (uint64_t fd, uint64_t buffer, uint64_t size){
	if (fd== 0){
		if buffer_is_ready(){
			copy_input(buffer, size);
			return kstrlen(buffer);
		}
		pcb* proc = get_active_pcb();
		proc -> status = WAITING;
		proc -> waing_on_stdin = -1;
		schedule();
	}
	return (uint64_t)NULL;
}

uint64_t do_system_call(uint64_t syscall_code, uint64_t arg1, uint64_t arg2, uint64_t arg3){
	
	switch(syscall_code)
	{
		case SYS_exit : printf("SYS_exit NOT IMPLEMENTED\n");
						while(1);
						break;
		case SYS_brk : return do_break(arg1);
						break;
		case SYS_getcwd : return do_getcwd(arg1, arg2);
						break;
		case SYS_write : return do_write(arg1, arg2, arg3);
						break;
		case SYS_read : return do_read(arg1, arg2, arg3);
						break;
		default : printf("SYSCALL NOT IMPLEMENTED: %d, 0x%x\n (0x%x, 0x%x, 0x%x)",
						 syscall_code, syscall_code, arg1, arg2, arg3);
						while(1);
						break;

	}


}



