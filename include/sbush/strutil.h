#ifndef _SB_STRUTIL_H
#define _SB_STRUTIL_H

#define MAX_STR_LEN 2040


int strlen(char* s);
int strsearch(char* s, char c);
int strcpy(char* s, char* t);
int strcpy_from(char* s, char* t, int start);
int strprefix(char* pref, char* str);
int strcomp(char* str1, char* str2);
char** strsplit(char* str, char delim);
int itoa(char* buf, int i);
int strrvrs(char* buf);
int str_free_splitted_list(char** list);
int strcat(char* prefix, char* suffix, char* buffer, int buffer_size);
int str_split_line(char* line, int pos, char* target[]);
int strcount(char*str, char q);
int strremove(char* str, char q);


#endif
