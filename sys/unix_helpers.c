#include <sys/sbunix.h>

long row = 0; long col = 0;
void do_newline(){
	// TODO : clear rest of the line :))
	col = 0;
	row ++;
	// TODO: rollup;	
	if (row>=22) row =0;
}
void write_k(char v){
	if (v=='\n'){
		do_newline();
		return;
	}
	char* position = (char*)(0xb8000 + 160*row+ 2*col);
	*position = v;
	position++;
	*position = 0;
	col ++;
	if (col>=80){
		do_newline();
	}
}


char _glyph_buffer [4] = {' ', ' ', ' ', ' '};

void put_pressed_key(unsigned char key){
	if(keyboard_feed(key, _glyph_buffer)){
		char* position = (char*)(0xb8000 + 160*21+ 2*48);
		*position = _glyph_buffer[0];
		position++;
		*position = 0x00;
		position++;
		*position = _glyph_buffer[1];
		position++;
		*position = 0x00;
		position++;
		*position = _glyph_buffer[2];
		position++;
		*position = 0x00;
		position++;
		*position = _glyph_buffer[3];
		position++;
		*position = 0x00;
		position++;
	}
}


char _time_buffer [] = "0:00:00";

void put_time_since_boot(void){
	if (tick_count(_time_buffer)){
		char* position = (char*)(0xb8000 + 160*21+ 2*56);
		for(int i=0; _time_buffer[i]; ++i){
			*position = _time_buffer[i];
			position++;
			*position = 0x00;
			position++;		
		}
	} 

}



int itoh_printf(char* buf, uint64_t n){
	if (n==0){
		buf[0]='0';
		buf[1]=0;
		return 0;
	}
	uint64_t temp = n;
	int i=0;
	while(temp){
		i++;
		temp/=16;
	}
	buf[i] = '\0';
	for(i--;i>=0;i--){
		temp = n%16;
		if (temp>9)
			buf[i]='A'+(temp-10);
		else
			buf[i]='0'+temp;
		n/=16;
	}
	return 0;
}
int itoa_printf(char* buf, uint64_t n){
	if (n==0){
		buf[0]='0';
		buf[1]=0;
		return 0;
	}
	int temp = n;
	int i=0;
	while(temp){
		i++;
		temp/=10;
	}
	buf[i] = '\0';
	for(i--;i>=0;i--){
		buf[i]='0'+(n%10);
		n/=10;
	}
	return 0;
}

char _buffer [30];


void printf(const char *fmt, ...){
	va_list val;
	
	va_start(val, fmt);

	// char *_buffer = _buffer; 

	while(*fmt) {
		if(*fmt == '%') {
			++fmt;
			if(*fmt == 'c') {
				_buffer[0] = va_arg(val, int);
				_buffer[1] = 0;  
				write_k(_buffer[0]);
			}
			else if(*fmt == 'd') {
				int number = va_arg(val, int);
				if (number<0){
					_buffer[0] = '-';
					itoa_printf(_buffer+1, -number); 
				}else
					itoa_printf(_buffer, number); 
				printf(_buffer);

			}
			else if(*fmt == 'x' || *fmt == 'p') {//assuming p is the same as x for now!
				int number = va_arg(val, int);
				itoh_printf(_buffer, number); 
				printf(_buffer);
			}
			else if(*fmt == 's') {
				char *strTemp;
				strTemp = va_arg(val, char*);
				_buffer[1] = 0;  
				while(*strTemp != '\0')
				{
					_buffer[0] = *strTemp;
					write_k(_buffer[0]);
					strTemp++;
				}
			}
		}
		else{
			
			write_k(*fmt);
		}
		++fmt;
	}

	return;

}


