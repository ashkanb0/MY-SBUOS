#include <sys/timer_helpers.h>
#include <sys/process_helpers.h>

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
uint64_t ticks = 0;
// uint16_t alarm_ticks = 0;
int tick_count(char* buff){
	ticks++;
	uint16_t print_ticks = ticks % PRINT_TICK_DIVIDE; 
	
	int print_time = 0;

	if( print_ticks == 1){
		clk ++;
		print_clk(clk/3600, (clk%3600)/60, clk%60, buff);
		print_time = 1;
	}
	uint16_t alarm_ticks = ticks % ALARM_TICK_DIVIDE; 
	if( alarm_ticks == 1){
		notify_alarm(ticks);
	}
	return print_time;
}

uint64_t timer_get_clock(){
	return ticks;
}


