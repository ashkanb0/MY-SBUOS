#ifndef _TARFS_H
#define _TARFS_H

#include <sys/defs.h>
// #include <sys/sbunix.h>

#define NUM_FILES 100

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

typedef struct posix_header_ustar {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
}tarfs_header;


void init_tarfs(void* tar_start, void* tar_end);


uint64_t tar_size( char* s);
uint64_t tar_size_roundup( uint64_t);
uint64_t map_file(char* path, int pid);


#endif
