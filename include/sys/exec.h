#ifndef _SYS_EXEC_H
#define _SYS_EXEC_H

#include <sys/defs.h>

typedef struct
{
	unsigned char e_ident[16]; /* ELF identification */
	uint16_t e_type; /* Object file type */
	uint16_t e_machine; /* Machine type */
	uint32_t e_version; /* Object file version */
	uint64_t e_entry; /* Entry point address */
	uint64_t e_phoff; /* Program header offset */
	uint64_t e_shoff; /* Section header offset */
	uint32_t e_flags; /* Processor-specific flags */
	uint16_t e_ehsize; /* ELF header size */
	uint16_t e_phentsize; /* Size of program header entry */
	uint16_t e_phnum; /* Number of program header entries */
	uint16_t e_shentsize; /* Size of section header entry */
	uint16_t e_shnum; /* Number of section header entries */
	uint16_t e_shstrndx; /* Section name string table index */
}elf_header;

typedef struct {
	uint32_t   p_type;
	uint32_t   p_flags;
	uint64_t   p_offset;
	uint64_t   p_vaddr;
	uint64_t   p_paddr;
	uint64_t   p_filesz;
	uint64_t   p_memsz;
	uint64_t   p_align;
}elf_pheader;

int check_elf(elf_header* header);
int map_elf(elf_header* header);

#endif
