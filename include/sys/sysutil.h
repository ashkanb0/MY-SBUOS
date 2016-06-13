#ifndef _SYS_SYSUTIL_H
#define _SYS_SYSUTIL_H

#include <sys/defs.h>

int kstrlen(char* dest);
int kstrcpy(char* dest, char* src, int);
int kstrcmp(char* s1, char* s2);

#endif
