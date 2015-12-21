#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sbush/strutil.h>

#define MAXSTRLEN 256

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
int printf(const char *format, ...) {
	va_list val;
	int printed = 0;

	va_start(val, format);

	char *strArray = _buffer; 

	while(*format) {
		if(*format == '%') {
			++format;
			if(*format == 'c') {
				_buffer[0] = va_arg(val, int);
				_buffer[1] = 0;  
				write(1, _buffer, 1);
				printed++;
			}
			else if(*format == 'd') {
				int number = va_arg(val, int);
				if (number<0){
					strArray[0] = '-';
					itoa_printf(strArray+1, -number); 
				}else
					itoa_printf(strArray, number); 
				while(*strArray)
				{
					write(1, strArray, 1);
					strArray++;
					printed++;
				}

			}
			else if(*format == 'x') {
				int number = va_arg(val, int);
				itoh_printf(strArray, number); 
				while(*strArray)
				{
					write(1, strArray, 1);
					strArray++;
					printed++;
				}

			}
			else if(*format == 's') {
				char *strTemp;
				strTemp = va_arg(val, char*);
				_buffer[1] = 0;  
				while(*strTemp != '\0')
				{
					_buffer[0] = *strTemp;
					write(1, _buffer, 1);
					printed++;
					strTemp++;
				}
			}
		}
		else{
			write(1, format, 1);
			++printed;
		}
		++format;
	}

	return printed;
}
