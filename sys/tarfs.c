#include <sys/tarfs.h>
#include <sys/sysutil.h>
#include <sys/sbunix.h>
#include <sys/exec.h>
#include <sys/process_helpers.h>


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
		uint64_t size = tar_size(p->size);
		// printf("size : >%x<, p->size >%s< p->name >%s<\n", size, p->size, p->name);
		if (kstrlen(p->name)==0)return 0;
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

int search_for_dir(char* path){
	uint64_t offset = 0;

	for (; _tar_start + offset<_tar_end ;)
	{
		tarfs_header* p = (tarfs_header *) (_tar_start+offset);
		uint64_t size = tar_size(p->size);
		if (kstrlen(p->name)==0)return 0;
		if(kstrcmp(p->name, path+1)==0){

			if(size==0){
				return 1;
			}else{
				return 0;
			}
		}
		offset += size + sizeof(tarfs_header) + tar_size_roundup(size);
	}
	return 0;
}

int search_file_open(char* abspath){
	uint64_t offset = 0;

	for (; _tar_start + offset<_tar_end ;)
	{
		tarfs_header* p = (tarfs_header *) (_tar_start+offset);
		uint64_t size = tar_size(p->size);
		if (kstrlen(p->name)==0)return 0;
		if(kstrcmp(p->name, abspath+1)==0){
			if(size==0){
				return 0;
			}else{
				return 1;
			}
		}
		offset += size + sizeof(tarfs_header) + tar_size_roundup(size);
	}
	return 0;
}

int do_open_file (pcb* proc, char* abspath){
	uint64_t offset = 0;

	for (; _tar_start + offset<_tar_end ;)
	{
		tarfs_header* p = (tarfs_header *) (_tar_start+offset);
		uint64_t size = tar_size(p->size);
		if (kstrlen(p->name)==0)return -1;
		if(kstrcmp(p->name, abspath+1)==0){
			int fd = proc-> next_fd;
			proc -> next_fd ++;
			(proc -> fd_table[fd]).file_start_address = (uint64_t)(p+1);
			(proc -> fd_table[fd]).file_size = size;
			(proc -> fd_table[fd]).file_offset = 0;
			return fd;
		}
		offset += size + sizeof(tarfs_header) + tar_size_roundup(size);
	}
	return -1;	
}

int do_open_dir  (pcb* proc, char* abspath){
	if (kstrcmp(abspath, "/")==0){
		int fd = proc-> next_fd;
		proc -> next_fd ++;
		(proc -> fd_table[fd]).file_start_address = _tar_end;
		(proc -> fd_table[fd]).file_size = 0;
		(proc -> fd_table[fd]).file_offset = 0;
		return fd;
	}
	
	uint64_t offset = 0;

	for (; _tar_start + offset<_tar_end ;)
	{
		tarfs_header* p = (tarfs_header *) (_tar_start+offset);
		uint64_t size = tar_size(p->size);
		if (kstrlen(p->name)==0)return -1;
		if(kstrcmp(p->name, abspath+1)==0){
			int fd = proc-> next_fd;
			proc -> next_fd ++;
			(proc -> fd_table[fd]).file_start_address = (uint64_t)(p);
			(proc -> fd_table[fd]).file_size = 0;
			(proc -> fd_table[fd]).file_offset = 0;
			return fd;
		}
		offset += size + sizeof(tarfs_header) + tar_size_roundup(size);
	}
	return -1;
}


uint64_t _fill_dents_by_path(char* path, struct dirent * buffer, uint64_t dir_size){
	uint64_t offset = 0;

	int ent_c = 0;

	int l = kstrlen(path);

	for (; _tar_start + offset<_tar_end ;)
	{
		tarfs_header* p = (tarfs_header *) (_tar_start+offset);
		uint64_t size = tar_size(p->size);
		int ll = kstrlen(p->name);
		if (ll==0)return 0;
		if(kstrstartswith(p->name, path)){
			int c = 0;
			for (int i = l; i < ll-1; ++i){
				if (p->name[i]=='/') c++;
			}
			if(c==0){
				buffer[ent_c].d_ino = ent_c+1;
				buffer[ent_c].d_reclen = sizeof(struct dirent);
				kstrcpy(buffer[ent_c].d_name, p->name+l, NAME_MAX);
				ent_c ++;
			}

		}
		offset += size + sizeof(tarfs_header) + tar_size_roundup(size);
	}
	for (int i = ent_c; i < dir_size; ++i)
	{
		buffer[i].d_ino = 0;
	}
	return 0;	
}

uint64_t fill_dents(uint64_t file_start, struct dirent * buffer, uint64_t size){
	if(file_start == _tar_end)
		return _fill_dents_by_path("", buffer, size);
	tarfs_header* p = (tarfs_header *) (file_start);
	return _fill_dents_by_path(p->name, buffer, size);
}

uint64_t read_to_buffer(fd_t* file_d, char* buffer ,uint64_t size){
	uint64_t size_read = 0;
	char* file = (char*) (file_d->file_start_address);
	for (;size_read < size -1 ; ++size_read){
		if (file_d->file_offset >= file_d->file_size){
			buffer[size_read] = '\0';
			return size_read +1;
		}
		buffer[size_read] = file[file_d->file_offset];
	}
	buffer[size_read] = '\0';
	return size_read +1;
}
