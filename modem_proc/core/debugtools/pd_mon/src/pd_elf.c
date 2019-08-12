/**
@file pd_elf.c
@brief This file contains the API details for the Protection Domain Monitor
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_elf.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_dump.h"
#include "pd_elf.h"
#include "pd_elf_hexagon.h"

struct regfile_hexagon regfile;

Elf32_Word memory[16384];

#define NT_QURT_PD_DUMP_NAME        0x20000000
#define NT_QURT_PD_DUMP_VERSION     0x20000001
#define NT_HEXAGON_REGFILE          0x60000000

static void regfile_init(void)
{
   int i;

   INTERNAL_MEMSET(&regfile, 0, sizeof(regfile));

   for (i = 0; i < sizeof(regfile) / sizeof(Elf32_Word); i++)
   {
      regfile.reg[i] = i << 24 + i << 16 + i << 8 + i;
   }
}

static void memory_init(void)
{
   Elf32_Word i;

   INTERNAL_MEMSET(memory, 0, sizeof(memory));

   for (i = 0; i < sizeof(memory) / sizeof(typeof(memory)); i++)
   {
      memory[i] = i;
   }
}

////////////////////////////////////////////////////////////////

Elf32_Word Elf32_Hash(Elf32_UChar const* name)
{
   Elf32_Word hash = 0;

   while (*name != '\0')
   {
      Elf32_Word temp;

      hash = (hash << 4) + *name;
      temp = hash & 0xf0000000;

      if (0 != temp)
      {
         hash ^= (temp >> 24);
      }

      hash &= ~temp;

      name++;
   }
   return hash;
}

static Elf32_Ehdr* pd_dump_header;

static inline PD_DUMP_STATUS pd_dump_header_init(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   pd_dump_header = INTERNAL_MALLOC(sizeof(Elf32_Ehdr));

   if (NULL != pd_dump_header)
   {
      INTERNAL_MEMSET(pd_dump_header, 0, sizeof(Elf32_Ehdr));

      pd_dump_header->e_ident[EI_MAG0]    = ELFMAG0;
      pd_dump_header->e_ident[EI_MAG1]    = ELFMAG1;
      pd_dump_header->e_ident[EI_MAG2]    = ELFMAG2;
      pd_dump_header->e_ident[EI_MAG3]    = ELFMAG3;
      pd_dump_header->e_ident[EI_CLASS]   = ELFCLASS32;
      pd_dump_header->e_ident[EI_DATA]    = ELFDATA2LSB;
      pd_dump_header->e_ident[EI_VERSION] = EV_CURRENT;

      pd_dump_header->e_ident[EI_OSABI]  = ELFOSABI_NONE;

      pd_dump_header->e_type             = ET_CORE;
      pd_dump_header->e_machine          = EM_HEXAGON;
      pd_dump_header->e_version          = EV_CURRENT;
      pd_dump_header->e_ehsize           = sizeof(Elf32_Ehdr);

      pd_dump_header->e_shentsize        = sizeof(Elf32_Shdr);
      pd_dump_header->e_phentsize        = sizeof(Elf32_Phdr);

      pd_dump_header->e_shstrndx         = SHN_UNDEF;

      rc = PD_DUMP_SUCCESS;
   }

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_header_prepare_offsets(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   rc = PD_DUMP_SUCCESS;

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_header_write(FILE* fp)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   if (1 == fwrite(pd_dump_header, pd_dump_header->e_ehsize, 1, fp))
   {
      rc = PD_DUMP_SUCCESS;
   }

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_header_free(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   if (NULL != pd_dump_header)
   {
      INTERNAL_FREE(pd_dump_header);

      rc = PD_DUMP_SUCCESS;
   }

   return rc;
}

////////////////////////////////////////////////////////////////

typedef struct pd_dump_prgtab
{
   char* name;
   void* addr;
   Elf32_Half size;

   Elf32_Phdr phdr;
   struct pd_dump_prgtab* next;

} pd_dump_prgtab_t, *pd_dump_prgtab_p;

static pd_dump_prgtab_p pd_dump_prgtab_head;

static inline PD_DUMP_STATUS pd_dump_prgtab_init(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   pd_dump_prgtab_head = NULL;

   rc = PD_DUMP_SUCCESS;

   return rc;
}

static inline Elf32_Half pd_dump_prgtab_count(void)
{
   pd_dump_prgtab_p pt_p;
   Elf32_Half count = 0;

   for (pt_p = pd_dump_prgtab_head; NULL != pt_p; pt_p = pt_p->next)
   {
      count++;
   }

   return count;
}

static inline Elf32_Off pd_dump_prgtab_size(void)
{
   pd_dump_prgtab_p pt_p;
   Elf32_Off size = 0;

   for (pt_p = pd_dump_prgtab_head; NULL != pt_p; pt_p = pt_p->next)
   {
      size += pt_p->phdr.p_filesz;
   }

   return size;
}

// INTERNAL API
PD_DUMP_STATUS pd_dump_prgtab_alloc(Elf32_Word type, char* name, Elf32_Word ntype, void* addr, Elf32_Half size)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;
   pd_dump_prgtab_p pt_p = INTERNAL_MALLOC(sizeof(pd_dump_prgtab_t));

   if (NULL != pt_p)
   {
      Elf32_Word name_len = strlen(name) + 1;
      Elf32_Word name_pad = 0;

      INTERNAL_MEMSET(pt_p, 0, sizeof(pd_dump_prgtab_t));

      pt_p->phdr.p_offset = 0;
      pt_p->phdr.p_type = type;
      pt_p->phdr.p_align = 0;

      switch (type)
      {
         case PT_LOAD:

            pt_p->size = size;
            pt_p->addr = addr;
            pt_p->name = name;

            pt_p->phdr.p_vaddr = (Elf32_Addr)addr;
            pt_p->phdr.p_filesz = size;
            pt_p->phdr.p_memsz = size;
            pt_p->phdr.p_flags = PF_R + PF_W;

            pt_p->next = pd_dump_prgtab_head;
            pd_dump_prgtab_head = pt_p;

            rc = PD_DUMP_SUCCESS;

            break;

         case PT_NOTE:

            if (0 != name_len % sizeof(Elf32_Word))
            {
               name_pad = sizeof(Elf32_Word) - name_len % sizeof(Elf32_Word);
            }

            pt_p->size = sizeof(Elf32_Nhdr) + name_len + name_pad;
            pt_p->size += size;

            pt_p->addr = INTERNAL_MALLOC(pt_p->size);

            if (NULL != pt_p->addr)
            {
               INTERNAL_MEMSET(pt_p->addr, 0, pt_p->size);

               ((Elf32_Nhdr*)pt_p->addr)->n_namesz = name_len;
               ((Elf32_Nhdr*)pt_p->addr)->n_descsz = size;
               ((Elf32_Nhdr*)pt_p->addr)->n_type = ntype;

               INTERNAL_MEMCPY(pt_p->addr + sizeof(Elf32_Nhdr), name, name_len);
               INTERNAL_MEMCPY(pt_p->addr + sizeof(Elf32_Nhdr) + name_len + name_pad, addr, size);

               pt_p->name = name;

               pt_p->phdr.p_vaddr = 0;
               pt_p->phdr.p_filesz = pt_p->size;
               pt_p->phdr.p_memsz = 0;
               pt_p->phdr.p_flags = 0;

               pt_p->next = pd_dump_prgtab_head;
               pd_dump_prgtab_head = pt_p;

               rc = PD_DUMP_SUCCESS;

               break;
            }

            else
            {
               INTERNAL_FREE(pt_p);
            }

            break;
      }
   }

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_prgtab_prepare_offsets(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;
   pd_dump_prgtab_p pt_p;
   Elf32_Off offset;

   pd_dump_header->e_phnum = pd_dump_prgtab_count();
   pd_dump_header->e_phoff = pd_dump_header->e_ehsize;

   offset = pd_dump_header->e_phoff + pd_dump_header->e_phentsize * pd_dump_header->e_phnum;

   for (pt_p = pd_dump_prgtab_head; NULL != pt_p; pt_p = pt_p->next)
   {
      if (0 != pt_p->phdr.p_filesz)
      {
         pt_p->phdr.p_offset = offset;

         offset += pt_p->phdr.p_filesz;
      }
   }

   rc = PD_DUMP_SUCCESS;

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_prgtab_write(FILE* fp)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;
   pd_dump_prgtab_p pt_p;

   // Write ALL Headers

   for (pt_p = pd_dump_prgtab_head; NULL != pt_p; pt_p = pt_p->next)
   {
      if (1 != fwrite((void*)&pt_p->phdr, sizeof(Elf32_Phdr), 1, fp))
      {
         //ERR("write error logic");
      }
   }

   // Write all Sections

   for (pt_p = pd_dump_prgtab_head; NULL != pt_p; pt_p = pt_p->next)
   {
      if (0 != pt_p->phdr.p_filesz)
      {
         if (1 != fwrite((void*)pt_p->addr, pt_p->phdr.p_filesz, 1, fp))
         {
            //ERR("write error logic");
         }
      }
   }

   rc = PD_DUMP_SUCCESS;

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_prgtab_free(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   while (NULL != pd_dump_prgtab_head)
   {
      pd_dump_prgtab_p pt_p = pd_dump_prgtab_head;
      pd_dump_prgtab_head = pt_p->next;

      switch (pt_p->phdr.p_type)
      {
         case PT_LOAD:
            break;

         case PT_NOTE:
            INTERNAL_FREE(pt_p->addr);
            break;
      }

      INTERNAL_FREE(pt_p);
   }

   rc = PD_DUMP_SUCCESS;

   return rc;
}

////////////////////////////////////////////////////////////////

typedef struct pd_dump_sectab
{
   Elf32_Shdr shdr;
   struct pd_dump_sectab* next;

} pd_dump_sectab_t, *pd_dump_sectab_p;

static pd_dump_sectab_p pd_dump_sectab_head;

static inline PD_DUMP_STATUS pd_dump_sectab_init(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   pd_dump_sectab_head = NULL;

   rc = PD_DUMP_SUCCESS;

   return rc;
}

static inline Elf32_Half pd_dump_sectab_count(void)
{
   pd_dump_sectab_p st_p;
   Elf32_Half count = 0;

   for (st_p = pd_dump_sectab_head; NULL != st_p; st_p = st_p->next)
   {
      count++;
   }

   return count;
}

static inline Elf32_Off pd_dump_sectab_size(void)
{
   pd_dump_sectab_p st_p;
   Elf32_Off size = 0;

   for (st_p = pd_dump_sectab_head; NULL != st_p; st_p = st_p->next)
   {
      size += st_p->shdr.sh_size;
   }

   return size;
}

// INTERNAL API
PD_DUMP_STATUS pd_dump_sectab_alloc(Elf32_Word type, Elf32_Word link)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;
   pd_dump_sectab_p st_p = INTERNAL_MALLOC(sizeof(pd_dump_sectab_t));

   if (NULL != st_p)
   {
      INTERNAL_MEMSET(st_p, 0, sizeof(pd_dump_sectab_t));

      st_p->shdr.sh_offset = 0;
      st_p->shdr.sh_type = type;
      st_p->shdr.sh_link = link;

      st_p->next = pd_dump_sectab_head;
      pd_dump_sectab_head = st_p;

      rc = PD_DUMP_SUCCESS;
   }

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_sectab_prepare_offsets(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;
   pd_dump_sectab_p st_p;
   Elf32_Off offset;

   pd_dump_header->e_shnum = pd_dump_sectab_count();
   pd_dump_header->e_shoff = pd_dump_header->e_phoff + pd_dump_header->e_phentsize * pd_dump_header->e_phnum + pd_dump_prgtab_size();

   offset = pd_dump_header->e_shoff + pd_dump_header->e_shentsize * pd_dump_header->e_shnum;

   for (st_p = pd_dump_sectab_head; NULL != st_p; st_p = st_p->next)
   {
      if (0 != st_p->shdr.sh_size)
      {
         st_p->shdr.sh_offset = offset;

         offset += st_p->shdr.sh_size;
      }
   }

   rc = PD_DUMP_SUCCESS;

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_sectab_write(FILE* fp)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;
   pd_dump_sectab_p st_p;

   for (st_p = pd_dump_sectab_head; NULL != st_p; st_p = st_p->next)
   {
      if (1 != fwrite((void*)&st_p->shdr, sizeof(Elf32_Shdr), 1, fp))
      {
         break;
      }
   }

   if (NULL == st_p)
   {
      for (st_p = pd_dump_sectab_head; NULL != st_p; st_p = st_p->next)
      {
         if (0 != st_p->shdr.sh_size)
         {
            if (1 != fwrite((void*)st_p->shdr.sh_addr, st_p->shdr.sh_size, 1, fp))
            {
               break;
            }
         }
      }

      if (NULL == st_p)
      {
         rc = PD_DUMP_SUCCESS;
      }
   }

   return rc;
}

static inline PD_DUMP_STATUS pd_dump_sectab_free(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   while (NULL != pd_dump_sectab_head)
   {
      pd_dump_sectab_p st_p = pd_dump_sectab_head;
      pd_dump_sectab_head = st_p->next;

      INTERNAL_FREE(st_p);
   }

   rc = PD_DUMP_SUCCESS;

   return rc;
}

////////////////////////////////////////////////////////////////

// API
PD_DUMP_STATUS pd_dump_init(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   if (PD_DUMP_SUCCESS == pd_dump_header_init() &&
       PD_DUMP_SUCCESS == pd_dump_prgtab_init() &&
       PD_DUMP_SUCCESS == pd_dump_sectab_init())
   {
      rc = PD_DUMP_SUCCESS;
   }

   return rc;
}

// API
PD_DUMP_STATUS pd_dump_prepare_offsets(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   if (PD_DUMP_SUCCESS == pd_dump_header_prepare_offsets() &&
       PD_DUMP_SUCCESS == pd_dump_prgtab_prepare_offsets() &&
       PD_DUMP_SUCCESS == pd_dump_sectab_prepare_offsets())
   {
      rc = PD_DUMP_SUCCESS;
   }

   return rc;
}

// API
PD_DUMP_STATUS pd_dump_write(FILE* fp)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   if (PD_DUMP_SUCCESS == pd_dump_header_write(fp) &&
       PD_DUMP_SUCCESS == pd_dump_prgtab_write(fp) &&
       PD_DUMP_SUCCESS == pd_dump_sectab_write(fp))
   {
      rc = PD_DUMP_SUCCESS;
   }

   return rc;
}

// API
PD_DUMP_STATUS pd_dump_term(void)
{
   PD_DUMP_STATUS rc = PD_DUMP_FAIL;

   if (PD_DUMP_SUCCESS == pd_dump_header_free() &&
       PD_DUMP_SUCCESS == pd_dump_prgtab_free() &&
       PD_DUMP_SUCCESS == pd_dump_sectab_free())
   {
      rc = PD_DUMP_SUCCESS;
   }

   return rc;
}

////////////////////////////////////////////////////////////////

main(int argc, char* argv)
{
   FILE* fp;

   memory_init();

   regfile_init();

   // pd_dump open

   if (PD_DUMP_SUCCESS != pd_dump_init())
   {
      printf("Failure to initialize\n");
      exit(PD_DUMP_FAIL);
   }

   // pd_dump note method

   if (PD_DUMP_SUCCESS != pd_dump_prgtab_alloc(PT_NOTE, ".qurt", NT_QURT_PD_DUMP_NAME, "SENSORS", strlen("SENSORS")+1))
   {
      printf("Failure to attach note\n");
      exit(PD_DUMP_FAIL);
   }

   if (PD_DUMP_SUCCESS != pd_dump_prgtab_alloc(PT_NOTE, ".qurt", NT_QURT_PD_DUMP_VERSION, "VERSION", strlen("VERSION")+1))
   {
      printf("Failure to attach note\n");
      exit(PD_DUMP_FAIL);
   }

   if (PD_DUMP_SUCCESS != pd_dump_prgtab_alloc(PT_NOTE, ".regs", NT_HEXAGON_REGFILE, &regfile, sizeof(regfile)))
   {
      printf("Failure to attach note\n");
      exit(PD_DUMP_FAIL);
   }

   if (PD_DUMP_SUCCESS != pd_dump_prgtab_alloc(PT_NOTE, ".regs/taska", NT_HEXAGON_REGFILE, &regfile, sizeof(regfile)))
   {
      printf("Failure to attach note\n");
      exit(PD_DUMP_FAIL);
   }

   if (PD_DUMP_SUCCESS != pd_dump_prgtab_alloc(PT_NOTE, ".regs/taskb", NT_HEXAGON_REGFILE, &regfile, sizeof(regfile)))
   {
      printf("Failure to attach note\n");
      exit(PD_DUMP_FAIL);
   }

   // pd_dump debug method

   if (PD_DUMP_SUCCESS != pd_dump_prgtab_alloc(PT_LOAD, ".data", NT_NONE, &memory[0], 1024))
   {
      printf("Failure to attach load region\n");
      exit(PD_DUMP_FAIL);
   }

   if (PD_DUMP_SUCCESS != pd_dump_prgtab_alloc(PT_LOAD, ".data", NT_NONE, &memory[1024], 1024))
   {
      printf("Failure to attach load region\n");
      exit(PD_DUMP_FAIL);
   }

   if (PD_DUMP_SUCCESS != pd_dump_prgtab_alloc(PT_LOAD, ".data", NT_NONE, &memory[8192], 2048))
   {
      printf("Failure to attach load region\n");
      exit(PD_DUMP_FAIL);
   }

   if (PD_DUMP_SUCCESS != pd_dump_sectab_alloc(SHT_NULL, SHN_UNDEF))
   {
      printf("Failure to attach required section header\n");
      exit(PD_DUMP_FAIL);
   }

   if (PD_DUMP_SUCCESS != pd_dump_prepare_offsets())
   {
      printf("Failure to prepare output offsets\n");
      exit(PD_DUMP_FAIL);
   }

   fp = fopen("test.lock", "r");

   if (NULL == fp)
   {
      fp = fopen("test.elf", "wb");

      if (NULL == fp)
      {
         printf("Failure to open output file\n");
         exit(PD_DUMP_FAIL);
      }

      if (PD_DUMP_SUCCESS != pd_dump_write(fp))
      {
         printf("Failure to write output file\n");
         exit(PD_DUMP_FAIL);
      }

      fclose(fp);
   }

   else
   {
      printf("Lock file present\n");
      fclose(fp);
   }

   if (PD_DUMP_SUCCESS != pd_dump_term())
   {
      printf("Failure to free memory\n");
      exit(PD_DUMP_FAIL);
   }

   exit(PD_DUMP_SUCCESS);
}
