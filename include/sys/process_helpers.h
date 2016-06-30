#ifndef _SYS_PROC_H
#define _SYS_PROC_H

#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/ps_helpers.h>

#define PROCESS_QUEUE_SIZE 100
#define HEAP_TOP 0x0000770000000000

enum process_status{READY = 0, RUNNING = 1, WAITING = 2, FINISHED = 3};


typedef struct file_descriptor
{
	uint64_t file_start_address;
	uint64_t file_size;
	uint64_t file_offset;
}fd_t;


typedef struct process_control_block
{
	int pid;
	int ppid;
	uint64_t pml4;
	uint64_t user_sp;
	uint64_t user_brk_point;
	uint64_t* kernel_sp;
	uint64_t kernel_stack;
	uint64_t ip;
	char pname [50];
	char wd [50];

	char waiting_on_stdin;
	int waiting_on_pid;
	uint64_t waiting_on_clock;

	uint64_t exit_notify_status;
	uint64_t status_return;

	enum process_status status;

	fd_t fd_table [15];
	int next_fd;
}pcb;


typedef struct process_control_block_list
{
	pcb* list [PROCESS_QUEUE_SIZE];
	uint32_t head ;
	uint32_t tail ;
}pcb_list;

void notify_stdin();
void notify_exit_pid(int pid, uint64_t status);
void notify_alarm(uint64_t clock);

void process_init();

void init();

void schedule();

void copy_kernel_pml4(uint64_t prog_page_table);

void k_process_exit();

int get_active_pid();
pcb* get_active_pcb();
pcb* get_forked_pcb(pcb* parent);
void process_run(pcb* proc);

pcb* find_pcb_by_ppid(int ppid);
pcb* find_pcb_by_pid(int pid);

int process_exec(pcb* proc, char* abspath, char *argv[], char* envp[]);

int do_open_file (pcb* proc, char* abspath);
int do_open_dir  (pcb* proc, char* abspath);
int _get_process_info(pspcb* buffer,int size);

int kill_pid (pid);

#endif
