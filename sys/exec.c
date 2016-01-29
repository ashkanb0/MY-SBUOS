#include <sys/sbunix.h>
#include <sys/exec.h>


int check_elf(elf_header* header){
	return 1;
}

int map_elf(elf_header* header){
	return (uint64_t) header;
}


