#include <sys/vma.h>


void vma_register_page(mem_page* page, int pid){
	// TODO :
}

void vma_free_pages(int pid){
	// TODO :
}

void mem_map_file(uint64_t file, uint64_t vaddr, uint64_t memsz, uint64_t filesz){
	char* dst = (char*) vaddr;
	char* src = (char*) file;
	uint64_t i;
	for (i = 0; i < filesz; ++i)
	{
		dst[i] = src[i];
	}
	for (; i < memsz; ++i)
	{
		dst[i] = 0;
	}

}

