#ifndef _SYS_PROC_H
#define _SYS_PROC_H

#include <sys/defs.h>

#define PROCESS_QUEUE_SIZE 100


enum{RUNNING = 0, WAITING = 1, FINISHED = 2};

typedef struct process_control_block
{
	uint64_t pid;
	uint64_t pml4;
	char command [50];
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

#endif
