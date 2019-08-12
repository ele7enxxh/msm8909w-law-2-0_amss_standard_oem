#ifndef QSEE_MEM_H
#include "lktypes.h"

#define QSEE_MEM_H

/*Allocate pages from both the PHYSICAL and VIRTUAL pools, map it accordingly.
  Uses Read/Write and non cacheable for the attributes.   
  Only maps for non-user mode. 
  Returns the virtual address.
  Maps as 1:1. 
  Assumes the phyiscal and virtual masks contain exactly the same information 
  (ie, only 1:1 mappings have been made this far). 
*/
unsigned int qsee_allocate_and_map_pages(unsigned int len, unsigned int alg_type);

/*Free pages from both the PHYSICAL and VIRTUAL pools, unmap them.*/
int qsee_free_and_unmap_pages(unsigned int vaddr, unsigned int len);


//These should not be called unless you know what you're doing (need to alloc both for VA and PA then map them, similar for free)
//unsigned int qsee_allocate_pages_aligned(unsigned int len, unsigned int mask_type, unsigned int alg_type);
//int qsee_free_pages_aligned(unsigned int addr, unsigned int len, unsigned int mask_type);

#define BLOCK_SIZE 0x1000  //4K block size 

#define MAX_REGION_SIZE_physical 0x0D00000  // 13 MB
#define MAX_REGION_SIZE MAX_REGION_SIZE_physical
#define BIT_MASK_LEN_physical ((MAX_REGION_SIZE_physical/BLOCK_SIZE)/32)

#define MAX_REGION_SIZE_virtual 0x8000000  //128MB, the virtual mask will take up exactly one page. 
#define BIT_MASK_LEN_virtual ((MAX_REGION_SIZE_virtual/BLOCK_SIZE)/32)

/* For calling into the alloc and free APIs */
#define PHYSICAL 0
#define VIRTUAL 1

#define BESTFIT 0
#define RANDOM 1

#endif
