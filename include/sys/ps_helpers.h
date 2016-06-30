#ifndef _SYS_PS_H
#define _SYS_PS_H


typedef struct process_control_block_report
{
	int pid;
	int ppid;
	char pname [50];
	
}pspcb;


#endif
