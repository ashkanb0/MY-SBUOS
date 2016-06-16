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

pcb* get_active_pcb(){
	return _active_pcb;
}

void notify_stdin(){
	
	for(uint32_t ptr = processq.head; ptr!= processq.tail; ptr = (ptr+1)%PROCESS_QUEUE_SIZE){
		pcb* proc = processq.list[ptr];
		if (proc->status==WAITING && proc-> waiting_on_stdin){
			proc -> waiting_on_stdin = 0;
			// TODO : if not waiting on pid;
			proc -> status = RUNNING;
		}
	}
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

	_active_pcb = res;
	if (res->status!=RUNNING && res->status!=READY)
		res = get_next_context();
	return res;

	_set_cr3(_active_pcb -> pml4);
	tss.rsp0 = (uint64_t) (_active_pcb -> kernel_stack + PAGESIZE - 16);

}

pcb* _get_new_pcb(){
	pcb* res = kmalloc(sizeof(pcb));
	_prev_pid ++;
	res -> pid = _prev_pid;

	res -> kernel_stack = get_new_page_v(res->pid);
	res -> kernel_sp = (uint64_t*)(res->kernel_stack+PAGESIZE);
	res->kernel_sp --;
	*(res->kernel_sp) = 0;

	res -> ppid = 0;
	res -> waiting_on_stdin = 0;
	res -> waiting_on_pid = 0;

	return res;
}

pcb* get_forked_pcb(pcb* parent){
	pcb* child = _get_new_pcb();
	mark_COW();
	child -> ppid = parent -> pid;
	child -> pml4 = get_new_page_table();
	child -> status = RUNNING;
	child -> user_brk_point = parent -> user_brk_point;
	child -> user_sp = parent -> user_sp;
	
	kstrcpy (child-> pname, parent-> pname, 50);
	kstrcpy (child-> wd, parent-> wd, 50);

	return child;
}



void prepare_user_memory(pcb* process){
	process -> pml4 = get_new_page_table();
	_set_cr3(process -> pml4);

	process -> ip = map_file(process->pname, process->pid);

	process -> user_brk_point = 0x0000700000000000;
	process -> user_sp = 0x00006FFFFFFFFFF0;
	*((uint64_t*)(process-> user_sp)-1) = 0;

	process -> status = RUNNING;
}

void _sys_idle(){
	while(1){
		schedule();
	}
}

void _switch_to_ring_3(){
	// http://wiki.osdev.org/Getting_to_Ring_3

	_set_cr3(_active_pcb -> pml4);
	tss.rsp0 = (uint64_t) (_active_pcb -> kernel_stack + PAGESIZE - 16);
	__asm__ volatile(
		// "cli\n\t"
		// "mov $0x23, %%ax\n\t"
		// "mov %%ax, %%ds\n\t"
		// "mov %%ax, %%es\n\t"
		// "mov %%ax, %%fs\n\t"
		// "mov %%ax, %%gs\n\t"


		"pushq $0x23\n\t"
		"pushq %0\n\t"
		"pushf \n\t"
		// "pushq $0x200\n\t"
		"pushq $0x1b\n\t"
		"pushq %1\n\t"
		:: "r"(_active_pcb->user_sp),
		   "r"(_active_pcb->ip)
		  :"%eax"
	);
	__asm__ volatile("iretq");
}

void k_thread_kernel(){
	
	if (_active_pcb -> status == READY){
		prepare_user_memory(_active_pcb);
	}
	//just an extra check! It should always be true!
	if (_active_pcb -> status == RUNNING){
		_switch_to_ring_3();
	}
	printf("[k_thread_kernel]: Shouldn't have gotten here!! PANIC!!!!\n");
	while(1);
}

void init(){
	pcb* sys_idle = _get_new_pcb();

	kstrcpy(sys_idle -> pname, "system_idle_process", 50);
	sys_idle->status = RUNNING;
	sys_idle->kernel_sp --;
	*(sys_idle->kernel_sp) = (uint64_t)(_sys_idle);

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

void process_run(pcb* proc){
	enqueue_process(&processq, proc);
}
