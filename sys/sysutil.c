#include <sys/sysutil.h>

int kstrlen(char* s){
	int i;
	for(i=0; s[i]; i++);
	return i;
}

void kstrcpy(char* dest, char* src, int size){
	int i;
	for(i=0; src[i]; i++){
		dest[i] = src[i];
		if (i>=size)return;
	}
	dest[i]=0; 
}

int kstrcmp(char* s1, char* s2){
	int l1 = kstrlen(s1);
	int l2 = kstrlen(s2);
	if (l1!=l2)return s1[l1]-s2[l1];
	for(int i=0; s1[i]; i++){
		if(s1[i]!=s2[i]) return s1[i]-s2[i];
	}
	return 0; 
}



