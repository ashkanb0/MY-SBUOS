#ifndef _TARFS_H
#define _TARFS_H

#include <sys/defs.h>
#include <sys/process_helpers.h>
#include <sys/sbunix.h>

#define NUM_FILES 100
#define NAME_MAX 255

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


struct dirent
{
  long d_ino;
  off_t d_off;
  unsigned short d_reclen;
	// unsigned char d_type;
  char d_name [NAME_MAX+1];
};


typedef struct DIR
{
	uint64_t fd;
	struct dirent* dents;
	uint64_t offset;
}DIR;

enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };


void init_tarfs(void* tar_start, void* tar_end);

int search_for_dir(char* path);
uint64_t tar_size( char* s);
uint64_t tar_size_roundup( uint64_t);
uint64_t map_file(char* path, int pid);


int search_file_for_exec(char* path);
int search_file_open(char* abspath);

uint64_t fill_dents(uint64_t file_start, struct dirent * buffer, uint64_t size);


#endif
