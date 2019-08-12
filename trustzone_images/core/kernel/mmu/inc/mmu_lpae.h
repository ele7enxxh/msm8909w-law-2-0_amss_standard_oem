#ifndef MMU_LPAE_H
#define MMU_LPAE_H
/*=============================================================================

      LPAE (Large Physical Address Extention) Page Table Manager

FILE:      mmu_lpae.h

GENERAL DESCRIPTION
  This file contains data structures and exported API by SMMU LPAE translation.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2013
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/mmu/inc/mmu_lpae.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/06/13   sho     Initial version
=============================================================================*/

/* ----------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------- */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ----------------------------------------------------------------------------
** Definitions
** ------------------------------------------------------------------------- */
typedef void (*LPAE_tlb_flush_t)(uint64_t addr);
typedef void (*LPAE_cache_clean_t)(void * addr, unsigned int length);

/* ----------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------- */

// result codes
#define LPAE_OK               0
#define LPAE_ERR_GENERIC      -1
#define LPAE_ERR_BAD_PARAM    -2
#define LPAE_ERR_NOT_ALIGNED  -3
#define LPAE_ERR_ALLOC_FAILED -4

// page table upper block attributes
typedef union
{
   struct 
   {
      unsigned PXN:1;
      unsigned XN:1;
   } S1; // stage 1
   struct
   {
      unsigned rsv0:1;
      unsigned XN:1;
   } S2; // stage 2
   uint8_t raw;
}
LPAE_upper_block_attr_t;

// page table lower block attributes
typedef union
{
   struct 
   {
      unsigned AttrIndx:3;
      unsigned NS:1;
      unsigned AP:2;
      unsigned SH:2;
      unsigned AF:1;
      unsigned nG:1;
   } S1; // stage 1
   struct
   {
      unsigned MemAttr:4;
      unsigned HAP:2;
      unsigned SH:2;
      unsigned AF:1;
   } S2; // stage 2
   uint16_t raw;
}
LPAE_lower_block_attr_t;

// table attributes for page table
typedef union
{
   struct
   {
      unsigned PXN:1;
      unsigned XN:1;
      unsigned AP:1;
      unsigned NS:1;
   } S1; // stage 1
   // should be zero for stage 2
   uint8_t raw;
}
LPAE_table_attr_t;

// memory region attributes for mapping and unmapping
typedef struct
{
  uint64_t                 input_addr;
  uint64_t                 output_addr;
  uint64_t                 size;
  LPAE_lower_block_attr_t  lower_block_attr;
  LPAE_upper_block_attr_t  upper_block_attr;
  LPAE_table_attr_t        table_attr;
} 
LPAE_map_attr_t;

/**
@brief Initialize LPAE module

   The purpose of this function is to initialize the LPAE module.
   This should be called only once.

@param[in] pt_memory - start virtual address of memory used to allocate page tables; 
               this must be 4KB aligned
@param[in] pt_memory_size - size of above; this must be 4KB aligned
@param[in] pt_memory_phys_offset - difference of phys from virt address of the 
               the page table memory; for example if phys address is 0x300000
               and virt address is 0x200000, this offset is 0x100000

@return LPAE_OK if successful; error codes otherwise
*/
int LPAE_Init (  
   void                 * pt_memory,
   uint32_t             pt_memory_size,
   int64_t              pt_memory_phys_offset
   );

/**
@brief Map address range

   The purpose of this function is to map the address range.

@param[in] l1pt_base - L1 page table base
@param[in] tlb_flush - function to flush TLB for a single input address; 
           NULL if not required 
@param[in] cache_clean_fnc - function to clean cache for single line
           NULL if not required 
@param[in] p_attr - pointer to mapping attribute structure

@return LPAE_OK if successful; error codes otherwise
*/
int LPAE_Map (  
   void * l1pt_base,
   LPAE_tlb_flush_t tlb_flush,
   LPAE_cache_clean_t cache_clean,
   const LPAE_map_attr_t * p_attr
   );

/**
@brief Unmap address range

   The purpose of this function is to unmap the address range.

@param[in] l1pt_base - L1 page table base
@param[in] tlb_flush - function to flush TLB for a single input address; 
           NULL if not required 
@param[in] cache_clean_fnc - function to clean cache for single line
           NULL if not required 
@param[in] p_attr - pointer to mapping attribute structure; 
                    the physical address and block attributes are ignored 

@return LPAE_OK if successful; error codes otherwise
*/
int LPAE_Unmap (  
   void * l1pt_base,
   LPAE_tlb_flush_t tlb_flush,
   LPAE_cache_clean_t cache_clean,
   const LPAE_map_attr_t * p_attr
   );

#ifdef __cplusplus
}
#endif

#endif // MMU_LPAE_H
