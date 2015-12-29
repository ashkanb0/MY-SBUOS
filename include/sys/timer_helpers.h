#ifndef __TMR_HLP_H
#define __TMR_HLP_H

#include <sys/defs.h>

#define TICK_DIVIDE 	24

/***
*
*  char _time_buffer [] = "0:00:00";
*
***/
void print_clk(int h, int m,int s, char* buff){
	buff[0] = '0'+h;
	
	buff[1] = ':';
	
	buff[2] = '0'+(m/10);
	buff[3] = '0'+(m%10);
	
	buff[4] = ':';

	buff[5] = '0'+(s/10);
	buff[6] = '0'+(s%10);
}


uint64_t clk = 0;
uint16_t ticks = 0;
int tick_count(char* buff){
	ticks++;
	if( ticks >= TICK_DIVIDE){
		clk ++;
		print_clk(clk/3600, (clk%3600)/60, clk%60, buff);
		ticks = 0;
		return 1;
	}
	return 0;
}

#endif

