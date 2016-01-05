#include <sys/tarfs.h>

uint64_t _tar_start, _tar_end;

uint64_t tar_size(char* s){
	uint64_t res = 0;
	for(int i=0; i<11; i++, s++){
		res = (res* 8) + ((uint64_t)*s - 0x30);
	}
	return res;
}

uint64_t tar_size_roundup( uint64_t size){
	if (size == 0) return 0;
	return 512 - (size%512);
}


void init_tarfs(void* tar_start, void* tar_end){
	_tar_start = (uint64_t) tar_start;
	_tar_end = (uint64_t) tar_end;
}

