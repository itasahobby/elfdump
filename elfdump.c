#include <elf.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define eprintf(...) fprintf (stderr, __VA_ARGS__)

void print_help(char **argv) {
    printf("Usage: %s [-e|--exclude] file section\n", argv[0]);
}

int main(int argc, char **argv) {
    
    // argument parsing
    int c;
    int flag_exclude = 0;

    while((c = getopt(argc, argv, "pe")) != -1) {
        switch(c) {
            case 'e':
                flag_exclude = 1;
                break;
            case '?':
                print_help(argv);
                return 1;
            default:
                print_help(argv);
                return 1;
        }
    }

    if( (argc - optind) < 2) {
        print_help(argv);
        return 1;
    }

    // dump section contents
    char *buf;
    char *name;
    int fd;
    size_t shrnum;
    size_t shstrndx;
    Elf *elf;
    Elf_Data *data = NULL;
    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;

    // initialize libelf and exit if any error

    if(elf_version(EV_CURRENT) == EV_NONE){
        eprintf("Error while initializing libelf\n");
        return 1;
    }

    if((fd = open(argv[optind], O_RDONLY, 0)) < 0){
        perror("Error");
        return 1;
    }

    if((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL){
        eprintf("Error while loading elf\n");
        return 1;
    }

    if(elf_getshdrstrndx(elf, &shstrndx) != 0){
        eprintf("Failed retrieving section index of section header string table\n");
        return 1;
    }

    elf_getshdrnum(elf, &shrnum);

    // keep section order specified in argv
    for(int i = optind + 1; i < argc; i++) {
        while((scn = elf_nextscn(elf, scn)) != NULL) {
            
            gelf_getshdr(scn, &shdr);
            name = elf_strptr(elf, shstrndx, shdr.sh_name);
            
            if(! (strcmp(name, argv[i])) ^ flag_exclude) {
                
                buf = calloc(shdr.sh_size, sizeof(char));
                if(shdr.sh_type != SHT_NOBITS) {
                    lseek(fd, shdr.sh_offset, SEEK_SET);
                    read(fd, buf, shdr.sh_size);
                }

                write(1, buf, shdr.sh_size);
                free(buf);
            }
        }
    }

    elf_end(elf);

    return 0;
}