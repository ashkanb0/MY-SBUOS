#include <sys/tarfs.h>

uint64_t _tar_start, _tar_end;

uint64_t atoi(int l, char* s){
	uint64_t res = 0;
	for(int i=0; i<l; i++){
		res = (res* 10) + (s[i] - '0');
	}
	return 0;
}

void init_tarfs(void* tar_start, void* tar_end){
	_tar_start = (uint64_t) tar_start;
	_tar_end = (uint64_t) tar_end;
}

