#ifndef __QSEE_SERVICES_ELF_H
#define __QSEE_SERVICES_ELF_H

#include "lktypes.h"

#define ELFINFO_MAGIC_SIZE (16)

typedef struct
{
  unsigned char e_ident[ELFINFO_MAGIC_SIZE]; /** Magic number and other info   */
  uint16_t        e_type;                /** Object file type                    */
  uint16_t        e_machine;             /** Architecture                        */
  uint32_t        e_version;             /** Object file version                 */
  uint32_t        e_entry;               /** Entry point virtual address         */
  uint32_t        e_phoff;               /** Program header table file offset    */
  uint32_t        e_shoff;               /** Section header table file offset    */
  uint32_t        e_flags;               /** Processor-specific flags            */
  uint16_t        e_ehsize;              /** ELF header size in bytes            */
  uint16_t        e_phentsize;           /** Program header table entry size     */
  uint16_t        e_phnum;               /** Program header table entry count    */
  uint16_t        e_shentsize;           /** Section header table entry size     */
  uint16_t        e_shnum;               /** Section header table entry count    */
  uint16_t        e_shstrndx;            /** Section header string table index   */
} Elf32_Ehdr;


typedef struct
{
  uint32_t p_type;                   /** Segment type */
  uint32_t p_offset;                 /** Segment file offset */
  uint32_t p_vaddr;                  /** Segment virtual address */
  uint32_t p_paddr;                  /** Segment physical address */
  uint32_t p_filesz;                 /** Segment size in file */
  uint32_t p_memsz;                  /** Segment size in memory */
  uint32_t p_flags;                  /** Segment flags */
  uint32_t p_align;                  /** Segment alignment */
} Elf32_Phdr;

#define PT_NULL    0          /** NULL Segment */ 
#define PT_LOAD    1          /** Load Segment */ 
#define PT_DYNAMIC 2          /** Dynamic Segment */

#define PT_FLAG_HASH_TYPE_MASK 0x2000000
#define PT_FLAG_TYPE_MASK 0x7000000

int qsee_load_and_auth_elf_image(uint32_t pa, uint32_t mdt_len, uint32_t img_len, uint32_t privilege);
int qsee_load_and_auth_elf_image_reloc(uint32_t pa, uint32_t mdt_len, uint32_t img_len, uint32_t privilege, uint32_t reloc);

#endif
