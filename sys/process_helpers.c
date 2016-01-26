#include <sys/sbunix.h>
#include <sys/process_helpers.h>
#include <sys/memory_helpers.h>
#include <sys/sysutil.h>


uint64_t _prev_pid ;
uint64_t _active_pid ;

pcb_list runableq;
pcb_list waitlist;

pcb* kernel_pcb;


void switch_to_ring_3(){
	// TODO : why not 0x28 ?
		// "movq , %%lax\n\t"
	uint32_t tss= 0x2b; 
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

void exec_empty(char* path){
	pcb* prog = kmalloc(sizeof(pcb));
	_prev_pid ++;
	prog -> pid = _prev_pid;
	kstrcpy(prog -> wd, "/", 50);
	prog -> pml4 = get_new_page_table(prog->pid)->base;

	copy_kernel_pml4(prog->pml4);

	kstrcpy(prog -> pname, path, 50);
	prog -> rsp = get_new_page_v(prog->pid)+ PAGESIZE - 1; // BOTTOM OF PAGE!
	
	prog -> status = READY;


	switch_to_ring_3();
	printf("Hello, User World!\n");
	// _active_pid = prog -> pid;
	// kexecve(command, 1, &command, )
}

void schedule(){
	// TODO :
}

