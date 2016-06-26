#include <sys/tarfs.h>
#include <sys/sysutil.h>
#include <sys/sbunix.h>
#include <sys/exec.h>


uint64_t _tar_start, _tar_end;

uint64_t tar_size(char* s){
	uint64_t res = 0;
	for(int i=0; i<11; i++, s++){
		res = (res* 8) + ((uint64_t)*s - 0x30);
	}
	return res;
}

uint64_t tar_size_roundup( uint64_t size){
	if (size == 0) return 0; //TODO: size % 512 == 0?
	return 512 - (size%512);
}


void init_tarfs(void* tar_start, void* tar_end){
	_tar_start = (uint64_t) tar_start;
	_tar_end = (uint64_t) tar_end;
}

uint64_t map_file(char* path, int pid){
	uint64_t offset = 0;

	for (; _tar_start + offset<_tar_end ;)
	{
		tarfs_header* p = (tarfs_header *) (_tar_start+offset);
		if (kstrlen(p->name)==0)return 0;
		uint64_t size = tar_size(p->size);
		// printf("%s==%s?\n",p->name, path );
		if(kstrcmp(p->name, path+1)==0){

			if(check_elf((elf_header*)(p+1))){
				return map_elf((elf_header*)(p+1));
			}else{
				return 0;
			}
		}
		offset += size + sizeof(tarfs_header) + tar_size_roundup(size);
	}
	return 0;
}

int search_file_for_exec(char* path){
	uint64_t offset = 0;

	for (; _tar_start + offset<_tar_end ;)
	{
		tarfs_header* p = (tarfs_header *) (_tar_start+offset);
		if (kstrlen(p->name)==0)return 0;
		uint64_t size = tar_size(p->size);
		// printf("size : >%x<, p->size >%s< p->name >%s<\n", size, p->size, p->name);
		// printf("%s==%s?\n",p->name, path );
		if(kstrcmp(p->name, path+1)==0){

			if(check_elf((elf_header*)(p+1))){
				return 1;
			}else{
				return 0;
			}
		}
		offset += size + sizeof(tarfs_header) + tar_size_roundup(size);
		// printf("size : >%x<, offset >%x<\n", size, offset);
	}
	return 0;
}

