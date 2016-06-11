#include <sys/sysutil.h>

int kstrlen(char* s){
	int i;
	for(i=0; s[i]; i++);
	return i;
}

int kstrcpy(char* dest, char* src, int size){
	int i;
	for(i=0; src[i]; i++){
		dest[i] = src[i];
		if (i>=size) return -1;
	}
	dest[i]=0;
	return i; 
}

int kstrcmp(char* s1, char* s2){
	int l1 = kstrlen(s1);
	int l2 = kstrlen(s2);
	if (l1!=l2)return (l1<l2)?s1[l1]-s2[l1]:s1[l2]-s2[l2];
	for(int i=0; s1[i]; i++){
		if(s1[i]!=s2[i]) return s1[i]-s2[i];
	}
	return 0; 
}



