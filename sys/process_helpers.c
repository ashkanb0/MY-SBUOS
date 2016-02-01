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

void init(){
	pcb* prog = kmalloc(sizeof(pcb));
	_prev_pid ++;
	prog -> pid = _prev_pid;
	kstrcpy(prog -> wd, "/", 50);

	prog -> pml4 = get_new_page_table(prog->pid);
	copy_kernel_pml4(prog->pml4);

	kstrcpy(prog -> pname, "/bin/init", 50);

	// TODO : map a page to a proper stack place, move this to schedule?
	prog -> sp = get_new_page_v(prog->pid)+ PAGESIZE - 1; // BOTTOM OF PAGE!
	
	prog -> status = READY;

	switch_to_ring_3();
	_active_pid = prog->pid;
	prog -> ip = map_file("bin/init",prog->pid);
	printf("Hello, User World!\n");
}

void k_process_exit(){
	// TODO : free vma pages

	runableq.list[_active_pid] -> status = FINISHED; 
	schedule();
}


void schedule(){
	// TODO :
	while(1);
}

