#include <sys/sbunix.h>
#include <sys/process_helpers.h>
#include <sys/memory_helpers.h>
#include <sys/sysutil.h>
#include <sys/tarfs.h>
#include <sys/gdt.h>


int _prev_pid ;

pcb_list processq;

pcb* _active_pcb;
pcb* kernel_pcb;


int get_active_pid(){
	return _active_pcb -> pid;
}



void process_init(){
	_prev_pid = 0;
	processq.head = processq.tail = 0;

	for (int ind = 0; ind < PROCESS_QUEUE_SIZE; ++ind)
	{
		processq.list[ind] = NULL;
	}
}

void enqueue_process(pcb_list* list, pcb* process){
	list->list[list->tail] = process;
	list->tail++;
	list->tail %= PROCESS_QUEUE_SIZE;
}

pcb* get_next_context(){
	enqueue_process(&processq, _active_pcb);

	pcb* res = processq.list[processq.head];
	processq.head++;
	processq.head %= PROCESS_QUEUE_SIZE;
	return res;
}

pcb* _get_new_pcb(){
	pcb* res = kmalloc(sizeof(pcb));
	_prev_pid ++;
	res -> pid = _prev_pid;

	res -> kernel_stack = get_new_page_v(res->pid);
	res -> kernel_sp = (uint64_t*)(res->kernel_stack+PAGESIZE);
	res->kernel_sp --;
	*(res->kernel_sp) = 0;


	return res;
}

void prepare_user_memory(pcb* process){
	process -> pml4 = get_new_page_table();
	_set_cr3(process -> pml4);

	process -> ip = map_file(process->pname, process->pid);

	//TODO: any better way?
	process -> user_sp = process->ip - (40*PAGESIZE); 

	process -> status = RUNNING;
}

void k_thread_0(){
	while(1){
		schedule();
	}
}

void k_thread_kernel(){
	
	if (_active_pcb -> status == READY){
		prepare_user_memory(_active_pcb);
	}
	//just an extra check! It should always be true!
	if (_active_pcb -> status == RUNNING){
		// http://wiki.osdev.org/Getting_to_Ring_3
		__asm__ volatile(
			"push 0x23\n\t"
			"push %0\n\t"
			"pushf\n\t"
			"push 0x1b\n\t"
			"push %1\n\t"
			"iretq\n\t"
			:
			: "r"(_active_pcb->ip), "r"(_active_pcb->user_sp)
			);
	}
	printf("[k_thread_kernel]: Shouldn't have gotten here!! PANIC!!!!\n");
	while(1);
}

void init(){
	pcb* sys_idle = _get_new_pcb();

	kstrcpy(sys_idle -> pname, "system_idle_process", 50);
	sys_idle->status = RUNNING;
	sys_idle->kernel_sp --;
	*(sys_idle->kernel_sp) = (uint64_t)(k_thread_0);

	pcb* shell = _get_new_pcb();

	kstrcpy(shell -> pname, "bin/sbush", 50);
	kstrcpy(shell -> wd, "/", 50);
	shell->status = READY;
	shell->kernel_sp --;
	*(shell->kernel_sp) = (uint64_t)(k_thread_kernel);
	shell->kernel_sp --;
	*(shell->kernel_sp) = 0;

	enqueue_process(&processq, sys_idle);
	enqueue_process(&processq, shell);

	_active_pcb = sys_idle;
	processq.head++;
	__asm__ volatile("movq %0, %%rsp"::"r"(sys_idle->kernel_sp):);
	__asm__ volatile("retq":);

}

void schedule(){
	__asm__ volatile("movq %%rsp, %0":"=r"(_active_pcb -> kernel_sp):);
	_active_pcb = get_next_context();
	__asm__ volatile("movq %0, %%rsp"::"r"(_active_pcb -> kernel_sp):);

}

