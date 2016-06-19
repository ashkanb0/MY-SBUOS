#include <sys/systemcall.h>
#include <sys/syscall.h>
#include <sys/sysutil.h>
#include <sys/process_helpers.h>
#include <sys/memory_helpers.h>

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
		while( !buffer_is_ready()){
			pcb* proc = get_active_pcb();
			proc -> status = WAITING;
			proc -> waiting_on_stdin = -1;
			schedule();
		}
		return copy_input((char*)buffer, size);
	}
	return (uint64_t)NULL;
}

uint64_t _return_from_fork_child(){
	__asm__ volatile(
		"movq $0, %%eax\n\t"
		"iretq\n\t"
		);
	return 0;
}

uint64_t do_fork(){
	pcb* proc = get_active_pcb();
	pcb* child = get_forked_pcb(proc);

	char* to = (char*) child -> kernel_stack;
	char* from = (char*) proc -> kernel_stack;
	
	for (int i = 0; i < PAGESIZE; ++i)
	{
		to[i] = from [i];
	}


	uint64_t rsp = 0;
	__asm__ volatile("movq %%rsp, %0":"=r"(rsp):);
	
	child -> kernel_sp = (uint64_t*)(child -> kernel_stack + rsp - proc->kernel_stack);
	
	child->kernel_sp +=4;
	child->kernel_sp --;
	*(child->kernel_sp) = (uint64_t)(_return_from_fork_child);
	child->kernel_sp --;
	*(child->kernel_sp) = 0;

	process_run(child);

	handle_COW(child -> user_sp);
	return child->pid;
}

uint64_t do_waitpid (uint64_t pid, uint64_t status_return, uint64_t options){
	pcb* proc = get_active_pcb();
	proc -> waiting_on_pid = pid;
	proc -> status = WAITING;
	schedule();
	*((uint64_t*)status_return) = proc -> exit_notify_status;
	return 0;
}


uint64_t do_system_call(uint64_t syscall_code, uint64_t arg1, uint64_t arg2, uint64_t arg3){
	

	uint64_t res = 0;

	switch(syscall_code)
	{
		case SYS_brk : res = do_break(arg1);
						break;
		case SYS_getcwd : res = do_getcwd(arg1, arg2);
						break;
		case SYS_write : res = do_write(arg1, arg2, arg3);
						break;
		case SYS_read : res = do_read(arg1, arg2, arg3);
						break;
		case SYS_fork : res = do_fork();
						break;
		case SYS_wait4 : res = do_waitpid(arg1, arg2, arg3);
						break;
		default : printf("SYSCALL NOT IMPLEMENTED: %d, 0x%x\n (0x%x, 0x%x, 0x%x)",
						 syscall_code, syscall_code, arg1, arg2, arg3);
						while(1);
						break;
	}


	return res;


}



