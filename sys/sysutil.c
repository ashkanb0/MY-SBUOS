#include <sys/sysutil.h>

void kstrcpy(char* dest, char* src, int size){
	int i;
	for(i=0; src[i]; i++){
		dest[i] = src[i];
		if (i>=size)return;
	}
	dest[i]=0; 
}



