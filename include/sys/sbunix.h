#ifndef __SBUNIX_H
#define __SBUNIX_H

#include <sys/defs.h>
#include <stdarg.h>

long row = 0; long col = 0;

void write_k(char v){
	char* position = (char*)(0xb8000 + 160*row+ 2*col);
	*position = v;
	col ++;
	if (col>=80){
		col = 0;
		row ++;
		// TODO: rollup;
		if (row>=80) row =0;
	}
}


void printf(const char *fmt, ...){
	va_list val;
	
	va_start(val, fmt);

	// char *strArray = _buffer; 

	while(*fmt) {
		// if(*fmt == '%') {
		// 	++fmt;
		// 	if(*fmt == 'c') {
		// 		_buffer[0] = va_arg(val, int);
		// 		_buffer[1] = 0;  
		// 		write(1, _buffer, 1);
		// 		printed++;
		// 	}
		// 	else if(*fmt == 'd') {
		// 		int number = va_arg(val, int);
		// 		if (number<0){
		// 			strArray[0] = '-';
		// 			itoa_printf(strArray+1, -number); 
		// 		}else
		// 			itoa_printf(strArray, number); 
		// 		while(*strArray)
		// 		{
		// 			write(1, strArray, 1);
		// 			strArray++;
		// 			printed++;
		// 		}

		// 	}
		// 	else if(*fmt == 'x') {
		// 		int number = va_arg(val, int);
		// 		itoh_printf(strArray, number); 
		// 		while(*strArray)
		// 		{
		// 			write(1, strArray, 1);
		// 			strArray++;
		// 			printed++;
		// 		}

		// 	}
		// 	else if(*fmt == 's') {
		// 		char *strTemp;
		// 		strTemp = va_arg(val, char*);
		// 		_buffer[1] = 0;  
		// 		while(*strTemp != '\0')
		// 		{
		// 			_buffer[0] = *strTemp;
		// 			write(1, _buffer, 1);
		// 			printed++;
		// 			strTemp++;
		// 		}
		// 	}
		// }
		// else{
			
			write_k(*fmt);
		// }
		++fmt;
	}

	return;

}

#endif
