#ifndef __TMR_HLP_H
#define __TMR_HLP_H

#include <sys/defs.h>

#define TICK_DIVIDE 	18

/***
*
*  char _time_buffer [] = "0:00:00";
*
***/

int tick_count(char* buff);

uint64_t timer_get_clock();

#endif

