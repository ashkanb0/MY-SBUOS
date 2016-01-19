#include <sys/sbunix.h>
#include <sys/process_helpers.h>
#include <sys/memory_helpers.h>
#include <sys/sysutil.h>

uint64_t _prev_pid ;

pcb_list runableq;
pcb_list waitlist;

void process_init(){
	_prev_pid = 0;
	runableq.head = runableq.tail = 0;
	waitlist.head = waitlist.tail = 0;

	for (int ind = 0; ind < PROCESS_QUEUE_SIZE; ++ind)
	{
		runableq.list[ind] = NULL;
		waitlist.list[ind] = NULL;
	}
	// TODO: create PCB for kernel!!!
}

void enqueue_process(pcb_list* list, pcb* process){
	//TODO : 
}

void kexecve(char* path, int argc, char* argv[], char* envp[]){
	pcb* prog = kmalloc(sizeof(pcb));

	_prev_pid ++;
	prog->pid = _prev_pid;
	strcpy(prog->command, path);
	prog -> pml4 = get_new_page_table()->base;

	enqueue_process(&runableq, prog);
}

void exec_empty(char* command){
	// kexecve(command, 1, &command, )
}

void schedule(){
 // TODO: 
}

