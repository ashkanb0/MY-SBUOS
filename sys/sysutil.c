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

int kstrstartswith(char* target, char* prefix){
	for (int i = 0; prefix[i] ; ++i)
	{
		if(target[i] != prefix[i])return 0;
	}
	return 1;
}

int kstrcat(char* st1, char* st2, char* buffer, int size){
	int i;
	for (i = 0; st1[i]; ++i)
		buffer[i] = st1[i];
	for (int j = 0; st2[j]; ++i,++j)
		buffer[i] = st2[j];
	buffer[i] = '\0';
	return 0;
}

int cleanup_path(char* buffer, int size){
	// TODO:
	return 0;
}

int path_merge(char* base, char* relative, char* buffer, int size){
	if(kstrstartswith(relative, "/")){
		kstrcpy(buffer, relative, size);
		return cleanup_path(buffer, size);
	}
	if(kstrstartswith(relative, "./")){
		kstrcat(base, relative+2, buffer, size);
		return cleanup_path(buffer, size);
	}
	kstrcat(base, relative, buffer, size);
	return cleanup_path(buffer, size);
}

