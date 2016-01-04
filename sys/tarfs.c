#include <sys/tarfs.h>

uint64_t _tar_start, _tar_end;

uint64_t atoi(char* s){
	uint64_t res = 0;
	for(; *s; s++){
		res = (res* 10) + (*s - '0');
	}
	return res;
}

void init_tarfs(void* tar_start, void* tar_end){
	_tar_start = (uint64_t) tar_start;
	_tar_end = (uint64_t) tar_end;
}

