#include <err.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <unistd.h>

void dump_hex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

typedef struct { 
  unsigned char* data;
  unsigned int size;
  unsigned int addr;
} section;

void free_section(section *s) {
    free(s->data);
    free(s);
}

section* read_section(int fd, Elf_Scn* scn) {
    section *s;
    GElf_Shdr shdr;

    if(gelf_getshdr(scn , &shdr) != &shdr) {
        err(EX_DATAERR, "Could not get section data");
    }

    unsigned char* section_data = calloc(1, shdr.sh_size);

    if (shdr.sh_type == SHT_NOBITS)
        memset(section_data, 1, shdr.sh_size);
    else {
        if (lseek(fd, shdr.sh_offset, SEEK_SET) == -1) {
            free(section_data);
            err(EX_SOFTWARE, "Could not seek");
        }

        read(fd, section_data, shdr.sh_size);
    }

    s = malloc(sizeof(section*));
    s->data = section_data;
    s->size = shdr.sh_size;
    s->addr = shdr.sh_offset;

    return s;
}

section* read_section_by_name(Elf* elf, int fd, const char* section_name) {

    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;

    // retrieve index of section name string table
    size_t shstrndx;
    if (elf_getshdrstrndx(elf, &shstrndx) != 0) {
        err(EX_DATAERR, "Could not find section name section");
    }


    // iterate over sections searching by name
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        if (gelf_getshdr(scn , &shdr) != &shdr) {
            err(EX_DATAERR, "Could not get section data");
        }
        const char* scn_name = elf_strptr(elf, shstrndx , shdr.sh_name);
        if (scn_name == NULL) {
            err(EX_DATAERR, "Could not get section name");
        }
        if (strcmp(scn_name, section_name) == 0)
            return read_section(fd, scn);
    }

    err(EX_SOFTWARE, "Section %s not found", section_name);
}

int main(int argc, char **argv) {
    int fd;
    Elf *e;

    if (argc != 3)
        errx(EX_USAGE, "usage: %s file section", argv[0]);

    if (elf_version(EV_CURRENT) == EV_NONE)
        errx(EX_SOFTWARE, "ELF library initialization failed : %s ", elf_errmsg(-1));

    if ((fd = open(argv[1], O_RDONLY, 0)) < 0)
        err(EX_NOINPUT , "open %s failed", argv[1]);

    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
        errx(EX_SOFTWARE , "elf_begin() failed: %s", elf_errmsg(-1));

    section *s = read_section_by_name(e, fd, argv[2]);
    
    dump_hex(s->data, s->size);

    free_section(s);

    return EXIT_SUCCESS;
}