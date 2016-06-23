#include <sys/sbunix.h>
#include <sys/exec.h>
#include <sys/vma.h>


int check_elf(elf_header* hdr){
	if(!hdr) return 0;
	if(hdr->e_ident[0] != 0x7f||
		hdr->e_ident[1] != 'E'||
		hdr->e_ident[2] != 'L'||
		hdr->e_ident[3] != 'F'
	) {
		return 0;
	}
	return 1;
}

uint64_t map_elf(elf_header* header){
	uint64_t elf_base = (uint64_t)(header);
	uint64_t p_base = elf_base + header-> e_phoff;

	int seg_n = header-> e_phnum;

	printf("# of entries in the program header table : %d\n", seg_n);

	for (int seg = 0; seg < seg_n; ++seg)
	{
		elf_pheader* phdr= (elf_pheader*) p_base;

		// printf("p_memsz: %x, p_filesz: %x, p_vaddr: %x\n", phdr->p_memsz, phdr ->p_filesz, phdr->p_vaddr);
		mem_map_file(elf_base + (phdr->p_offset), phdr->p_vaddr, phdr->p_memsz, phdr ->p_filesz);

		p_base += header->e_phentsize;
	}

	return header -> e_entry;
}


