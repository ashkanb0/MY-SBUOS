#ifndef __TMR_HLP_H
#define __TMR_HLP_H

#include <sys/defs.h>

#define PRINT_TICK_DIVIDE 	10
#define ALARM_TICK_DIVIDE 	3

/***
*
*  char _time_buffer [] = "0:00:00";
*
***/

int tick_count(char* buff);

uint64_t timer_get_clock();

#endif

