#include <sys/sbunix.h>
#include <sys/process_helpers.h>
#include <sys/memory_helpers.h>
#include <sys/sysutil.h>
#include <sys/tarfs.h>

int _prev_pid ;
int _active_pid ;

pcb_list runableq;
pcb_list waitlist;

pcb* kernel_pcb;


int get_active_pid(){
	return _active_pid;
}


void switch_to_ring_3(){
	// TODO : why not 0x28 ? why not 0x2B ?
	uint32_t tss= 0x28; 
	// uint32_t tss= 0x2b; 
	__asm__ volatile("ltr (%0)" : : "r"(&tss) );

}


void process_init(){
	_prev_pid = 0;
	runableq.head = runableq.tail = 0;
	waitlist.head = waitlist.tail = 0;

	for (int ind = 0; ind < PROCESS_QUEUE_SIZE; ++ind)
	{
		runableq.list[ind] = NULL;
		waitlist.list[ind] = NULL;
	}
	// create PCB for kernel!!!
	kernel_pcb = kmalloc(sizeof(pcb));
	kernel_pcb -> pid = 0;
	kernel_pcb -> pml4 = get_active_pml4();
	_active_pid = 0;
}

void enqueue_process(pcb_list* list, pcb* process){
	//TODO : 
}

pcb* dequeue_process(pcb_list* list){
	// TODO :
	return NULL;
}

// void kexecve(pcb* prog, char* path, int argc, char* argv[], char* envp[]){

// 	strcpy(prog->command, path);

// 	enqueue_process(&runableq, prog);
// }

void init(){
	pcb* prog = kmalloc(sizeof(pcb));
	_prev_pid ++;
	prog -> pid = _prev_pid;
	kstrcpy(prog -> wd, "/", 50);

	prog -> pml4 = get_new_page_table(prog->pid);

	kstrcpy(prog -> pname, "/bin/init", 50);

	_set_cr3(prog->pml4);

	// TODO : do all this in prog -> pml4! right?
	prog -> sp = get_new_page_v(prog->pid)+ PAGESIZE - 1; // BOTTOM OF PAGE!
	
	prog -> status = READY;

	_active_pid = prog->pid;
	prog -> ip = map_file("bin/init",prog->pid);

	runableq.list[1] = prog;

	_set_cr3(kernel_pcb->pml4);
	// switch_to_ring_3();
	// printf("Hello, User World!\n");
}

void k_process_exit(){
	// TODO : free vma pages

	runableq.list[_active_pid] -> status = FINISHED; 
	schedule();
}


void schedule(){
	// TODO :

	pcb* prog = runableq.list[1];

	_set_cr3(prog->pml4);
	switch_to_ring_3();


	printf("ip: %x, sp:%x \n", prog->ip, prog->sp);
	uint64_t tem = 0x28; 
	__asm__ volatile("mov %0,%%rax;"::"r"(tem));
	__asm__ volatile("ltr %ax");

	__asm__ volatile(
		"push $0x23\n\t"
		"push %0\n\t"
		"push $0x1b\n\t"
		"push %1\n\t"
		"iretq\n\t"
		:
		: "r"(prog->ip), "r"(prog->sp)
		);
		// "pushf\n\t"
		// : "%rax"

	while(1);

}

