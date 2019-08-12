#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/*
 * Section name string table is a section has following format
 * table[0] has first char: '\0'
 * table[1-63] string table name
 * table[64*n-64*(n+1) - 1] is the nth section name
 * section name has fixed 64 char name for fast/easy processing
 *
 * string table section is located right after section headers
 * it's the last piece of data in output file
 */

typedef unsigned int Elf32_Word;
typedef unsigned int Elf32_Off;
typedef unsigned int Elf32_Addr;

typedef struct {
    Elf32_Word      p_type;
    Elf32_Off       p_offset;
    Elf32_Addr      p_vaddr;
    Elf32_Addr      p_paddr;
    Elf32_Word      p_filesz;
    Elf32_Word      p_memsz;
    Elf32_Word      p_flags;
    Elf32_Word      p_align;
} Elf32_Phdr;

typedef struct {
    Elf32_Word      sh_name;
    Elf32_Word      sh_type;
    Elf32_Word      sh_flags;
    Elf32_Addr      sh_addr;
    Elf32_Off       sh_offset;
    Elf32_Word      sh_size;
    Elf32_Word      sh_link;
    Elf32_Word      sh_info;
    Elf32_Word      sh_addralign;
    Elf32_Word      sh_entsize;
} Elf32_Shdr;

typedef struct {
    unsigned int entry_offset;  // file offset of entry point of the ELF 
    unsigned int vstart;
    unsigned int pstart;
    unsigned int vend;
    char name[32];
} Elf_Info_Patch;

#define ELF_ENTRY_START 24
#define ELF_PHPOS_START 28
#define ELF_PHNUM_START 44
#define ELF_SHPOS_START 32
#define ELF_SHNUM_START 48
#define ELF_SHSTRNDX_START  50
#define ELF_PHENT_SIZE  0x20
#define ELF_SHENT_SIZE  0x28
#define MAX_ELFS        3
#define MAX_SECTIONS    32
#define MAX_SEGMENTS    8
#define SHT_STRTAB      3
#define MAX_SEC_NAME_LEN 64
#define EIP_RECORD_LEN  48

Elf32_Phdr ph[MAX_ELFS][MAX_SEGMENTS];     
Elf32_Shdr sh[MAX_ELFS * MAX_SECTIONS] = { {0} };     
char data_buf[0x1000000];
char strtab[MAX_ELFS*MAX_SECTIONS * MAX_SEC_NAME_LEN] = {0};    //ASSUME section name is less than 64 chars
int cur_sh_index=1;
char str_buf[MAX_SECTIONS * MAX_SEC_NAME_LEN];  //ASSUME section name is less than 64 chars
Elf_Info_Patch eip[MAX_ELFS];
unsigned int entry_addr = 0;

unsigned int debug_enabled = 0;

void my_printf(const char *fmt, ...) {
    if (debug_enabled != 0) {
        char buf[1024];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf) -1, fmt, ap);
        va_end(ap);
        printf("%s\n",buf);
    }
}

void dump_shdrs(void) {
    int i;
    for (i = 0; i < cur_sh_index; i++) 
        my_printf("sh_name=%x, sh_type=%x, sh_flags=%x, sh_addr=%x, sh_offset=%x, sh_size=%x sh_link=%x, sh_info=%x, sh_addralign=%x, sh_entsize=%x\n", 
                    sh[i].sh_name, sh[i].sh_type, sh[i].sh_flags, sh[i].sh_addr, sh[i].sh_offset,
                    sh[i].sh_size, sh[i].sh_link, sh[i].sh_info, 
                    sh[i].sh_addralign, sh[i].sh_entsize);
    
}

void extract_name(char *name, const char *input) {
    int i, j=0;
    memset(name, 0, 32);
    for (i = 0; i < strlen(input); i++) {
        if (input[i] == 0x2f || input[i] == 0x5c) {
            j = 0;
            memset(name, 0, 32);
            continue;
        }
        if (j < 31)
            name[j++] = input[i];
    }
    name[31] = 0;
}

int my_atoi(char *in) {
    char output[16]={0}, c;
    int i=strlen(in), j=0, ret=0;
    
    if (*in !='0')
        printf("Error: incorrect physical address given to qurt-image-build.py. Please provide hexadecimal value!!\n");

    while (i) 
        output[j++] = in[--i];
    
    j = 0;

    while(c = output[j]) {
        if ('0'<= c && c<='9')
            ret += (c-0x30)<<(4*j);
        if ('a'<= c && c<='f')
            ret += ((c-0x61) + 10)<<(4*j);
        if ('A'<= c && c<='F')
            ret += ((c-0x41) + 10)<<(4*j);
        j++;
    }

    return ret;
}

int main(int argc, char *argv[]) {
    FILE *in[MAX_ELFS], *out;
    int i, j, phdrpos, flag = 0, cur_offset = 0, next_fstart = 0, next_phstart = 0, padding_size, delta_paddr = 0, next_paddr=0, tmp;
    int input_elfs = 0;
    int shdrpos, start_sh, cur_strtab_index=0;
    size_t n;
    unsigned short phnum, shnum;
    char section_name[64];

    if (argc <=1 || argc > 9) {
        printf("usage: qdsp6-image-build qurt.elf app1.elf [appN.elf] -p paddr -o bootimg.pbn\n");
        printf("Note:  The first input has to be guest OS image, e.g. qurt.elf.\n");
        exit(1);
    }
        
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o")) {
            out = fopen(argv[i+1], "wb+");
            flag = 1;
            if (input_elfs == 0) {
                input_elfs = i - 1;
                if (input_elfs > MAX_ELFS) { 
                    printf("qdsp6-image-build only supports build <= %d ELFs\n", MAX_ELFS);
                    exit(1);
                }
            }
            break;
        }
        if (!strcmp(argv[i], "-p")) {
            if (input_elfs == 0) {
                input_elfs = i - 1;
                if (input_elfs > MAX_ELFS) { 
                    printf("qdsp6-image-build only supports build <= %d ELFs\n", MAX_ELFS);
                    exit(1);
                }
            }
            next_paddr = my_atoi(argv[i+1]);
            printf(">>>>>output=%x\n",next_paddr);
            entry_addr = next_paddr;
            i++;
            continue;
        }
            
        if ((in[i-1] = fopen(argv[i], "rb")) == NULL) {
            printf("input file %s don't exist\n", argv[i]);
            exit(1);
        }
        extract_name(eip[i-1].name, argv[i]);
    }

    if (!flag) {
        printf("usage: qdsp6-image-build qurt.elf app1.elf [appN.elf] -p paddr -o bootimg.pbn\n");
        printf("Note:  The first input has to be guest OS image, e.g. qurt.elf.\n");
        exit(1);
    }
    
    if (!strcmp(argv[argc-1], "-d")) debug_enabled = 1;

    // copy elf hdr and elf phdrs to boot image file
    fseek(in[0], 0, SEEK_SET);
    n = fread((void *)&data_buf, 4096, 1, in[0]); 
    n = fwrite((void *)&data_buf, 4096, 1, out);
    my_printf("copied 4096 bytes of ELF header to output file\n");
    
    fseek(in[0], ELF_PHPOS_START, SEEK_SET);
    n = fread((void *)&phdrpos, 1, 4, in[0]);
    next_phstart = phdrpos; //start offset to append phdrs from input files

	//What needs to be changed/tracked is physical address of each segment/section
    //and their file offsets.
    for (i = 0; i < input_elfs; i++) {
        start_sh = cur_sh_index;
        fseek(in[i], ELF_PHPOS_START, SEEK_SET);
        n = fread((void *)&phdrpos, 1, 4, in[i]);
        fseek(in[i], ELF_PHNUM_START, SEEK_SET);
        n = fread((void *)&phnum, 1, 2, in[i]);

        my_printf("%s phdrpos=%x, phnum=%x next_phstart = %x shdrpos=%x, shnum=%x\n", eip[i].name, phdrpos, phnum, next_phstart, shdrpos, shnum);
        
        //add section dump
        fseek(in[i], ELF_SHPOS_START, SEEK_SET);
        n = fread((void *)&shdrpos, 1, 4, in[i]);
        fseek(in[i], ELF_SHNUM_START, SEEK_SET);
        n = fread((void *)&shnum, 1, 2, in[i]);

        for (j = 0; j < shnum; j++) {
            fseek(in[i], shdrpos+(j*ELF_SHENT_SIZE), SEEK_SET);
            n = fread((void *)&sh[cur_sh_index], 4, 10, in[i]);
            //update each section's file offset in boot image
            sh[cur_sh_index].sh_offset += next_fstart;
            if (sh[cur_sh_index].sh_type == 0)
                //it's NULL type section
                continue;

            //my_printf("%s sh_name=%x, sh_type=%x, sh_flags=%x, sh_addr=%x, sh_offset=%x, sh_size=%x sh_link=%x, sh_info=%x, sh_addralign=%x, sh_entsize=%x\n", eip.name[i],
            //            sh[cur_sh_index].sh_name, 
            //            sh[cur_sh_index].sh_type, sh[cur_sh_index].sh_flags, sh[cur_sh_index].sh_addr, sh[cur_sh_index].sh_offset,
            //            sh[cur_sh_index].sh_size, sh[cur_sh_index].sh_link, sh[cur_sh_index].sh_info, 
            //            sh[cur_sh_index].sh_addralign, sh[cur_sh_index].sh_entsize);
            if (sh[cur_sh_index].sh_type == SHT_STRTAB) {
                // read entire string table
                fseek(in[i], sh[cur_sh_index].sh_offset, SEEK_SET);
                fread((void *)str_buf, 1, sh[cur_sh_index].sh_size, in[i]);
                if (i == 0) 
                    //first ElF, copy over it's .strtab string to output string table
                    memcpy(strtab, str_buf, 64);
                continue;
            }
                
            cur_sh_index++;
        }
		         
       //update sh_names and populate output strtab. 
       for (j = start_sh; j < cur_sh_index; j++) {
            strcpy(section_name, &str_buf[sh[j].sh_name]);
            strcat(section_name, ".");
            strcat(section_name, eip[i].name);
            my_printf("section name is %s\n",section_name);
            //update section name with new string table offset
            sh[j].sh_name = 64 * j;
            memcpy(&strtab[64*j], section_name, 64);
       }
        
        //end of section dump
        fseek(in[i], phdrpos, SEEK_SET);
        for (j = 0; j < phnum; j++) {
            fseek(in[i], phdrpos+(j*ELF_PHENT_SIZE), SEEK_SET);
            // read Elf[i]'s program header j's information
            n = fread((void *)&ph[i][j], 4, 8, in[i]);
            my_printf("p_vaddr=%x, p_paddr=%x, p_offset=%x p_filesz=%x\n",ph[i][j].p_vaddr, ph[i][j].p_paddr, ph[i][j].p_offset, ph[i][j].p_filesz);

            if (!next_paddr) next_paddr = ph[i][j].p_paddr;
            if (j == 0) {
                //first segment, find the delta paddr: paddr difference shall be added to each segment's physical address 
                //of current ELF.
                delta_paddr = next_paddr - ph[i][j].p_paddr; 
                                                            
                eip[i].entry_offset = ph[i][j].p_offset + next_fstart; // program .start address at file offset
                eip[i].vstart = ph[i][j].p_vaddr;
                eip[i].pstart = next_paddr;
            }
                                   
            fseek(in[i], ph[i][j].p_offset, SEEK_SET);
            //load segment j's raw data into data_buf
            n = fread((void *)&data_buf, ph[i][j].p_filesz, 1, in[i]);
            my_printf("read %x bytes at offset=%x\n",ph[i][j].p_filesz, ph[i][j].p_offset);
            cur_offset = ph[i][j].p_offset + next_fstart; 

            // write to output image
            ph[i][j].p_offset = cur_offset;
            ph[i][j].p_paddr += delta_paddr;  
            fseek(out, next_phstart, SEEK_SET);
            n = fwrite((void *)&ph[i][j], 4, 8, out);
            next_phstart += ELF_PHENT_SIZE;

            //store segment j's raw data to output file
            fseek(out, cur_offset, SEEK_SET);
            n = fwrite((void *)&data_buf, ph[i][j].p_filesz, 1, out);
            my_printf("wrte %x bytes at offset=%x, \n",ph[i][j].p_filesz, cur_offset);
            
        }   

        eip[i].vend = ph[i][j-1].p_vaddr + ph[i][j-1].p_memsz;
        
        cur_offset += n;

        tmp = (cur_offset + 0xfff) & 0xfffff000;

        padding_size = tmp - cur_offset;

        n = fread((void *)&data_buf, 1, padding_size, in[i]);
        n = fwrite((void *)&data_buf, 1, n, out);
        my_printf("write padding bytes = %x\n", n);

        next_fstart = tmp;  // output starting file offset to hold next ELF's data: padding 4KB aligned

        next_paddr = (ph[i][j-1].p_paddr + 0x100000) & 0xfff00000;  //align starting paddr to 4MB for each ELF file
        
        //write vstart, pstart, vend into rtos_debug[] array at .start section fixed offset 0x20. 
        fclose(in[i]);
    }

    //update segment number in program headers
    fseek(out, ELF_PHPOS_START, SEEK_SET);
    n = fread((void *)&phdrpos, 1, 4, out);
    phnum = (unsigned short)((next_phstart - phdrpos)/ELF_PHENT_SIZE);
    my_printf("total phdrs = %d \n", phnum);
    fseek(out, ELF_PHNUM_START, SEEK_SET);
    n = fwrite((void *)&phnum, 1, 2, out);

    //create strtab section
    sh[cur_sh_index].sh_name = 1;
    sh[cur_sh_index].sh_size = 64*(cur_sh_index+1);
    sh[cur_sh_index].sh_flags = 0;
    sh[cur_sh_index].sh_link = 0;
    sh[cur_sh_index].sh_addralign = 1;
    sh[cur_sh_index].sh_addr = 0;
    sh[cur_sh_index].sh_info = 0;
    sh[cur_sh_index].sh_entsize = 0;
    sh[cur_sh_index].sh_type = SHT_STRTAB;
    sh[cur_sh_index].sh_offset = next_fstart + (cur_sh_index+1)*ELF_SHENT_SIZE;
    cur_sh_index++;

    my_printf("total sections is %d string table written to %x\n",cur_sh_index, sh[cur_sh_index-1].sh_offset);
    
    //write merged string table section.
    fseek(out, sh[cur_sh_index-1].sh_offset, SEEK_SET);
    fwrite((void *)strtab, 1, sh[cur_sh_index-1].sh_size, out);

    //update section headers, write section headers at next_fstart
    fseek(out, next_fstart, SEEK_SET);
    fwrite((void *)sh, 1, cur_sh_index*ELF_SHENT_SIZE, out);

    //update section header offset, number and section strtab index
    fseek(out, ELF_SHPOS_START, SEEK_SET);
    n = fwrite((void *)&next_fstart, 4, 1, out);
    fseek(out, ELF_SHNUM_START, SEEK_SET);
    n = fwrite((void *)&cur_sh_index, 2, 1, out);
    cur_sh_index--; //last entry is for section name strtab
    fseek(out, ELF_SHSTRNDX_START, SEEK_SET);
    n = fwrite((void *)&cur_sh_index, 2, 1, out);
 

    //patch the elf information into .start section of boot image. 
    //each process' elf information patch accounts for 48 bytes.
    //Information will be used by kernel load and create process.
    next_fstart = eip[0].entry_offset+0x40;
    fseek(out, next_fstart, SEEK_SET);
    my_printf("Writing at file offset %x of boot image\n", next_fstart);
    for (i = 0; i < input_elfs; i++) {
        //Save elf file name as process name
        fwrite((void *)&eip[i].entry_offset, 4, 1, out);
        fwrite((void *)&eip[i].vstart, 4, 1, out);
        fwrite((void *)&eip[i].pstart, 4, 1, out);
        fwrite((void *)&eip[i].vend, 4, 1, out);
        fwrite((void *)eip[i].name, 1, 32, out);
        my_printf("Writing entry offset %x, vstart = %x, pstart = %x, vend = %x of process %s\n", eip[i].entry_offset, eip[i].vstart, eip[i].pstart, eip[i].vend, eip[i].name);
        printf("%x %x\n",eip[i].pstart, eip[i].vstart);
    }
        
    
    //update ELF entry
    fseek(out, ELF_ENTRY_START, SEEK_SET);
    fwrite((void *)&entry_addr, 4, 1, out);

    fclose(out);
    return 0;
}
