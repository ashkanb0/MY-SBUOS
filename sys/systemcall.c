#include <sys/systemcall.h>
#include <sys/syscall.h>
#include <sys/sysutil.h>
#include <sys/tarfs.h>
#include <sys/vma.h>
#include <sys/process_helpers.h>
#include <sys/timer_helpers.h>
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
	pcb* proc = get_active_pcb();
	if (fd== 0 && proc->fd_table[0].file_start_address==0){
		while( !buffer_is_ready()){
			proc -> status = WAITING;
			proc -> waiting_on_stdin = -1;
			schedule();
		}
		return copy_input((char*)buffer, size);
	}
	return read_to_buffer(&(proc->fd_table[fd]), (char*)buffer , size);
}

// uint64_t _return_from_fork_child(){
// 	__asm__ volatile(
// 		"movq $0, %%rax\n\t"
// 		"iretq\n\t"
// 		);
// 	return 0;
// }

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
	
	child->kernel_sp += 8;
	child->kernel_sp --;
	*(child->kernel_sp) = (uint64_t)(fork_return_from_child);
	child->kernel_sp --;
	*(child->kernel_sp) = 0;

	process_run(child);

	// 0xffffff37fffff000
	// handle_COW(child -> user_sp);
	_set_cr3(proc -> pml4);
	return child->pid;
}



uint64_t do_waitpid (uint64_t pid, uint64_t status_return, uint64_t options){
	pcb* proc = get_active_pcb();
	proc -> waiting_on_pid = pid;
	proc -> status = WAITING;
	proc -> status_return = status_return;
	schedule();
	*((int*)(proc -> status_return)) = proc -> exit_notify_status;
	return 0;
}

uint64_t do_alarm (uint64_t seconds){
	pcb* proc = get_active_pcb();
	uint64_t clock = timer_get_clock();
	proc -> waiting_on_clock = clock+(PRINT_TICK_DIVIDE*seconds);
	proc -> status = WAITING;
	schedule();
	return 0;
}

char abspath[100];
uint64_t do_execve (uint64_t filename, uint64_t argv, uint64_t envp){
	pcb* proc = get_active_pcb();
	path_merge(proc->wd, (char*)filename, abspath, 100);


	if (search_file_for_exec(abspath)){
		uint64_t* ptr = (uint64_t*)(0xffffff7fbfdfe000);
		ptr[0] = 0;
		return process_exec(proc, abspath, (char**)argv, (char**)envp );
	}
	return 1;
}

uint64_t do_chdir (uint64_t dirname){
	pcb* proc = get_active_pcb();
	path_merge(proc->wd, (char*)dirname, abspath, 100);
	int l = kstrlen(abspath);
	abspath[l] = '/';
	abspath[l+1] = '\0';

	if (kstrcmp(abspath, "/")==0 || search_for_dir(abspath)){		
		kstrcpy(proc->wd, abspath, 100);
		return 0;
	}
	return 1;
}

uint64_t do_exit(uint64_t status){
	pcb* proc = get_active_pcb();
	proc -> status = FINISHED;
	notify_exit_pid (proc-> pid, status);
	vma_free_pages (proc -> pid);
	schedule();
	return -1;
}

uint64_t do_open(uint64_t path, uint64_t mode){
	pcb* proc = get_active_pcb();
	path_merge(proc->wd, (char*)path, abspath, 100);
	if( mode == O_RDONLY){
		if (search_file_open(abspath)){
			return do_open_file(proc, abspath);
		}
	}
	else if( mode == O_DIRECTORY){
		int l = kstrlen(abspath);
		if (abspath[l-1]!= '/'){
			abspath[l] = '/';
			abspath[l+1] = '\0';
		}

		if (kstrcmp(abspath, "/")==0 || search_for_dir(abspath)){		
			return do_open_dir(proc, abspath);
		}			
	}
	else{
		printf("OPEN MODE NOT SUPPORTED: %x\n", mode);
		while(1);
	}
	return -1;
}

uint64_t do_getdents(uint64_t fd, uint64_t buffer, uint64_t size){
	pcb* proc = get_active_pcb();
	return fill_dents(proc->fd_table[fd].file_start_address, (struct dirent *) buffer, size);
}

uint64_t do_close(uint64_t fd){
	pcb* proc = get_active_pcb();
	proc->fd_table[fd].file_start_address = 0;
	proc->fd_table[fd].file_size = 0;
	proc->fd_table[fd].file_offset = 0;
	return 0;
}

uint64_t do_getps(uint64_t buffer, uint64_t size){
	return _get_process_info((pspcb*)buffer, size);
}

uint64_t do_kill(uint64_t pid, uint64_t sig){
	if(kill_pid (pid)!=0)
		return -1;
	notify_exit_pid (pid, sig);
	vma_free_pages (pid);
	return 0;
}

uint64_t do_yield(){
	schedule();
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
		case SYS_execve : res = do_execve(arg1, arg2, arg3);
						break;
		case SYS_exit : res = do_exit(arg1);
						break;
		case SYS_chdir : res = do_chdir(arg1);
						break;
		case SYS_open : res = do_open(arg1, arg2);
						break;
		case SYS_getdents : res = do_getdents(arg1, arg2, arg2);
						break;
		case SYS_close : res = do_close(arg1);
						break;
		case SYS_alarm : res = do_alarm(arg1);
						break;
		case SYS_getps : res = do_getps(arg1, arg2);
						break;
		case SYS_kill : res = do_kill(arg1, arg2);
						break;
		case SYS_yield : res = do_yield();
						break;
		default : printf("SYSCALL NOT IMPLEMENTED: %d, 0x%x\n (0x%x, 0x%x, 0x%x)",
						 syscall_code, syscall_code, arg1, arg2, arg3);
						while(1);
						break;
	}


	return res;


}



