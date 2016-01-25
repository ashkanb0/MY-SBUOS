#ifndef _SYS_PROC_H
#define _SYS_PROC_H

#include <sys/defs.h>

#define PROCESS_QUEUE_SIZE 100


enum{READY = 0, RUNNING = 1, WAITING = 2, FINISHED = 3};

typedef struct process_control_block
{
	uint64_t pid;
	uint64_t pml4;
	uint16_t rsp;
	char pname [50];
	char wd [50];

	uint16_t status;
	// TODO : complete pcb as it goes;
}pcb;

typedef struct process_control_block_list
{
	pcb* list [PROCESS_QUEUE_SIZE];
	uint32_t head ;
	uint32_t tail ;
}pcb_list;



void process_init();

void exec_empty(char* path);

void schedule();

void copy_kernel_pml4(uint64_t prog_page_table);


#endif
