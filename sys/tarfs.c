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

	// logging
	// uint64_t _tar_start = (uint64_t) &_binary_tarfs_start;
	// // uint64_t _tar_end = (uint64_t) &_binary_tarfs_end;
	// uint64_t offset = 0;

	// for (int i = 0; i < 10; ++i)
	// {
	// 	tarfs_header* p = (tarfs_header *) (_tar_start+offset);
	// 	uint64_t size = tar_size(p->size);
	// 	printf("name: %s, size: %s = %d\n", p->name, p->size, size);
	// 	offset += size + sizeof(tarfs_header) + tar_size_roundup(size);
	// }
	// kernel starts here
	
	
}

