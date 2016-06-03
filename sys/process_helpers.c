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


void switch_to_ring_3(){
	// TODO : why not 0x28 ? why not 0x2B ?
	uint32_t tss= 0x28; 
	// uint32_t tss= 0x2b; 
	__asm__ volatile("ltr (%0)" : : "r"(&tss) );

}


void process_init(){
	_prev_pid = 0;
	processq.head = processq.tail = 0;

	for (int ind = 0; ind < PROCESS_QUEUE_SIZE; ++ind)
	{
		processq.list[ind] = NULL;
	}
	// create PCB for kernel!!!
	kernel_pcb = kmalloc(sizeof(pcb));
	kernel_pcb -> pid = 0;
	kernel_pcb -> pml4 = get_active_pml4();
}

void enqueue_process(pcb_list* list, pcb* process){
	list->list[list->tail] = process;
	list->tail++;
	list->tail %= PROCESS_QUEUE_SIZE;
}

pcb* dequeue_process(pcb_list* list){
	// TODO :
	return NULL;
}

pcb* get_next_context(){
	// TODO :
	pcb* res = processq.list[processq.head];
	processq.head++;
	processq.head %= PROCESS_QUEUE_SIZE;
	return res;
}

pcb* _get_new_pcb(){
	pcb* res = kmalloc(sizeof(pcb));
	_prev_pid ++;
	res -> pid = _prev_pid;
	
	// TODO: 
	res -> kernel_stack = get_new_page_v(res->pid);
	res -> kernel_sp = (uint64_t*)(res->kernel_stack+PAGESIZE);


	return res;
}

// void kexecve(pcb* prog, char* path, int argc, char* argv[], char* envp[]){

// 	strcpy(prog->command, path);

// 	enqueue_process(&runableq, prog);
// }

void k_thread_A(){
	while(1){
		printf("WOOHOO! I AAAAAM RUNNING!!!\n");
		schedule();
	}
}

void k_thread_B(){
	while(1){
		printf("WOOHOO! I BBBBBM RUNNING!!!\n");
		schedule();
	}
}

void init(){
	pcb* pcba = _get_new_pcb();

	kstrcpy(pcba -> pname, "A", 50);
	pcba->kernel_sp --;
	*(pcba->kernel_sp) = (uint64_t)(gdt);
	pcba->kernel_sp --;
	*(pcba->kernel_sp) = (uint64_t)(k_thread_A);

	pcb* pcbb = _get_new_pcb();
	// kstrcpy(prog -> wd, "/", 50);

	kstrcpy(pcbb -> pname, "B", 50);
	pcbb->kernel_sp --;
	*(pcbb->kernel_sp) = (uint64_t)(gdt);
	pcbb->kernel_sp --;
	*(pcbb->kernel_sp) = (uint64_t)(k_thread_B);

	enqueue_process(&processq, pcba);
	enqueue_process(&processq, pcbb);
	enqueue_process(&processq, pcba);

	_active_pcb = pcba;
	processq.head++;
	__asm__ volatile("movq %0, %%rsp"::"r"(pcba->kernel_sp):);
	__asm__ volatile("retq":);

}

// void k_process_exit(){
// 	// TODO : free vma pages

// 	runableq.list[_active_pid] -> status = FINISHED; 
// 	schedule();
// }


void schedule(){
	// TODO :

	enqueue_process(&processq, _active_pcb);

	__asm__ volatile("movq %%rsp, %0":"=r"(_active_pcb -> kernel_sp):);

	_active_pcb = get_next_context() ;
	
	__asm__ volatile("pop %%rax":);
	__asm__ volatile("movq %0, %%rsp"::"r"(_active_pcb -> kernel_sp):);
	__asm__ volatile("retq":);

	// _set_cr3(prog->pml4);
	// switch_to_ring_3();


	// printf("ip: %x, sp:%x \n", prog->ip, prog->sp);
	// uint64_t tem = 0x2B; 
	// __asm__ volatile("mov %0,%%rax;"::"r"(tem));
	// __asm__ volatile("ltr %ax");

	// __asm__ volatile(
	// 	"push %1\n\t"
	// 	"push %0\n\t"
	// 	// "iretq\n\t"
	// 	:
	// 	: "r"(prog->ip), "r"(prog->sp)
	// 	);
	// 	// "pushf\n\t"
	// 	// : "%rax"
	// __asm__ volatile(
	// 	"iretq\n\t"
	// 	);

	// while(1);

}

