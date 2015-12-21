#include <stdio.h>

#include <sbush/util.h>
#include <stdlib.h>

int strcpy(char* s, char* t){
	char* temp = s;
	int l = 0;
	do{
		*t = *s;
		t++;
		s++;
		l++;
	}while(*s && l<MAX_STR_LEN);
	s = temp;
	return l;
}


int strcpy_fr_to(char* s, char* t, int start, int end){
	int l = end-start;
	for(int i=0; i<l;i++)
		t[i]= s[i+start];
	t[l]=0;
	return l;
}

int strcpy_from(char* s, char* t, int start){
	int l = strlen(s);
	for(int i=0; i<=l;i++)
		t[start+i]= s[i];
	return start+l;
}

int strsearch(char* s, char c){
	int l=strlen(s);
	for (int i = 0; i<l ;i++)
		if (s[i] == c) return i;
	return -1;	
}

int strlen(char* s){
	for (int i = 0; i<MAX_STR_LEN;i++)
		if (s[i] == 0) return i;
	return -1;	
}

int strprefix(char* pref, char* str){
	int l = strlen(pref);
	for (int i = 0; i<l ;i++)
		if (pref[i]!= str[i]) return 0;
	return -1;
}

int strcomp(char* str1, char* str2){
	int l1 = strlen(str1);
	int l2 = strlen(str2);
	if (l1-l2) return l1-l2;
	for (int i = 0; i<l1 ;i++){	
		int d = str1[i] - str2[i];
		if (d) return d;
	}
	return 0;
}


char** strsplit(char* str, char delim){
	//count the delimeters!
	int delim_c = 0;
	for(int i=0; i<strlen(str); i++){
		if (str[i] == delim) delim_c++;
	}

	char** res = (char**) malloc(sizeof(char*)*(2+delim_c));
	// go over it once again! make it happen
	int l = -1;
	delim_c = 0;
	for(int i=0; i<=strlen(str); i++){
		if (str[i] == delim||str[i] == 0){
			char* arg = (char*)malloc(i-l+1);
			strcpy_fr_to(str, arg, l+1,i);
			res[delim_c] = arg;
			l = i;
			delim_c++;
		}
	}
	res[delim_c]=NULL;
	return res;
}

int strreverse(char* str){
	int l = strlen(str);
	for (int i = 0; i <= (l-1)/2; ++i){
		char temp = str[l-1-i];
		str[l-1-i] = str[i];
		str[i] = temp;
	}
	return 0;
}



int itoa_r(char* buf, int i){
	for(;i; i/=10){
		*buf = '0'+(i%10);
		buf++;
	}
	*buf = 0;
	return i;
}

int itoa(char* buf, int i){
	if (i==0){
		buf[0] = '0';
		buf[1] = 0;
		return 0;
	}

	itoa_r(buf, i);
	return strreverse(buf);
}

int str_free_splitted_list(char** list){
	for (int i=0; list[i]; i++){
		free(list[i]);
	}
	free(list);
	return 0;
}

int strcat(char* prefix, char* suffix, char* buffer, int buffer_size){
	if (suffix == buffer){
		printf(">%s< , >%s< , >%s<\n", prefix, suffix, buffer);
		printf("UNHANDLED SITUATION!!!! BUG!\n");
		return -1;
	}

	int pref_l = strlen(prefix);
	int suf_l = strlen(suffix);
	if (pref_l+suf_l+1 >= buffer_size) return -1;

	int i=0;

	for(;i<pref_l;i++){
		buffer[i] = prefix[i];
	}
	for(;i<pref_l+suf_l;i++){
		buffer[i] = suffix[i-pref_l];
	}
	buffer[i] = 0;
	return 0;

}

int str_split_line(char* line, int pos, char* target[]){
	char* first_half = malloc(pos+1);
	strcpy_fr_to(line, first_half, 0, pos);
	int l = strlen(line);
	char* second_half = malloc(l - pos);
	strcpy_fr_to(line, second_half, pos+1, l);
	target[0] = first_half;
	target[1] = second_half;
	return 0;
}

int strcount(char*str, char q){
	int count=0;
	for(char*t=str ; *t!='\0'; t++)
		if(*t==q)count++;
	return count;
}

int strremove(char* str, char q){
	int i=0;
	int j=0;
	while(str[j]){
		str[j]=str[i];
		if(str[j]!=q)j++;
		i++;
	}
	return j;
}



