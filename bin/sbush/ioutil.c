#include <stdlib.h>
#include <sbush/util.h>
// #include <sbush/strutil.h>

int readline(char* buffer, char* prompt){
	write(1,prompt, strlen(prompt));	
	return read(0, buffer, BUFFERSIZE);
}

