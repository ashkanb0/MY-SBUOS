#include <sys/tarfs.h>

uint64_t _tar_start, _tar_end;

uint64_t atoi(int l, char* s){
	uint64_t res = 0;
	for(int i=0; i<l-1; i++, s++){
		res = (res* 10) + ((uint64_t)*s - 0x30);
	}
	return res;
}

void init_tarfs(void* tar_start, void* tar_end){
	_tar_start = (uint64_t) tar_start;
	_tar_end = (uint64_t) tar_end;
}

