#ifndef BOOT_CACHE_MMU_H
#define BOOT_CACHE_MMU_H

/*=============================================================================

                    Boot Cache MMU Header File

GENERAL DESCRIPTION
  This header file contains the public interface for the cache
  and mmu control functions.
          
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_cache_mmu.h#1 $    

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/14   ck      Added boot_l2_page_table_init
03/28/13   dh      Add mmu_get_page_table_base
03/13/13   kedara  Added mmu_set_dacr
03/01/13   jz      Expose mmu_flush_cache_and_disable
10/16/12   sl      Added boot_comdef.h
03/27/12   dh      Added boot_mmu_page_table_map_single_mem_block,
                   rename boot_mmu_populate_page_table_mem_map to
                   boot_mmu_page_table_map_mem_block_list
03/14/12   dh      Added boot_mmu_populate_page_table_mem_map,
                   boot_mmu_relocate_page_table and mmu_relocate_page_table_base
                   remove cache_mmu_re_enable
02/03/12   dh      Added mmu_invalidate_tlb
09/13/11   dh      Added dcache_flush_region
08/09/10   aus     Created.
===========================================================================*/



/*---------------------------------------------------------------------------
  Define DACR config value
---------------------------------------------------------------------------*/
/*Configure all domains as clients. If the domain remains as manager the
  TLB entry domain configurations are ignored by cpu during accesses and hence
  the permissions we set might not have any effect */
#define DACR_ALL_DOMAIN_CLIENTS        0x55555555


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/

/* Forward declaration */ 
struct mem_block;


/*===========================================================================

**  Function :  boot_mmu_page_table_map_mem_block_list

** ==========================================================================
*/
/*!
* 
* @brief
*   Populates a pagetable with the content of a memory map.
* 
* @par Dependencies
*   None
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
*
* @param[in] map Pointer to the memory map
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
boot_boolean boot_mmu_page_table_map_mem_block_list
(
  uint32 *mmu_l1_pagetable,
  const struct mem_block  *map 
);


/*===========================================================================

**  Function :  boot_mmu_page_table_map_single_mem_block

** ==========================================================================
*/
/*!
* 
* @brief
*   Populates a pagetable with the content of a memory map block.
* 
* @par Dependencies
*   None
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
*
* @param[in] map Pointer to the memory map block
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
boot_boolean boot_mmu_page_table_map_single_mem_block
(
  uint32 *mmu_l1_pagetable,
  const struct mem_block  *map_ptr
);


/*===========================================================================

**  Function :  boot_mmu_copy_page_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Copy the source L1 and L2 page table to destination L1 and L2 page table.
*   It will NOT relocate the page table base register(TTBR0) to the 
*   destination L1 page table. 
*   This API will assume the destination L1 and L2 page tables are 
*   already invalidated
*
* @param[in] src_l1_pagetable Pointer to the base of source L1 page table
*
* @param[in] dst_l1_pagetable Pointer to the base of destination L1 page table
*
* @param[in] dst_l2_pagetable Pointer to the base of destination L2 page table
*
* @param[in] dst_l2_pagetable_num total number of available destination L2 page table
*
* @par Dependencies
*   destination L1 and L2 page tables must be allocated and invalidated, destination L2
*   page tables must reside in a continuous chunk of memory
*
* @retval
*   True if successful, 
*   False if there is not enough space to copy existing L2 page table
* 
* @par Side Effects
*   None
* 
*/
boot_boolean boot_mmu_copy_page_table
(
  const uint32 *src_l1_pagetable,
  uint32 *dst_l1_pagetable,
  uint32 *dst_l2_pagetable,
  uint32 dst_l2_pagetable_num
);


/*===========================================================================

**  Function :  mmu_flush_cache

** ==========================================================================
*/
/*!
* 
* @brief
*   Flush the entire cache system. The data cache flush is achieved using 
*   atomic clean / invalidates working outwards from L1 cache. This is done 
*   using Set/Way based cache maintainance instructions.
*   The instruction cache can still be invalidated back to the point of
*   unification in a single instruction.  
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void mmu_flush_cache
( 
  void 
);

/*===========================================================================

**  Function :  cache_mmu_disable

** ==========================================================================
*/
/*!
* 
* @brief
*   Disable the MMU and caches.  
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void cache_mmu_disable
( 
  void 
);

/*============================================================================
* mmu_flush_cache_and_disable
*
** ==========================================================================
*/
/*!
* 
* @brief
*  Flush and disable the i and d cache
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/  
void mmu_flush_cache_and_disable
( 
  void 
);
  
/*===========================================================================

**  Function :  mmu_enable_instruction_cache

** ==========================================================================
*/
/*!
* 
* @brief
*   Enables instruction cache.  
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/  
void mmu_enable_instruction_cache
( 
  void 
);

/*===========================================================================

**  Function :  cache_set_memory_barrier_location

** ==========================================================================
*/
/*!
* 
* @brief
*   Sets the location of the actual memory barrier in SMEM  
* 
* @param[in] memory_barrier_ptr Pointer to a non-buffered or non-cached memory
*
* @par Dependencies
*   cache_set_memory_barrier_location() must be called with a pointer to
*   non-buffered or non-cached memory before any call to memory_barrier(),
*   for the instruction and data streams to be syncronized by
*   the memory_barrier() operation.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/  
void cache_set_memory_barrier_location
(
   void *memory_barrier_ptr
);


/*===========================================================================

**  Function :  dcache_flush_region

** ==========================================================================
*/
/*!
*
* @brief
*   Cleans and invalidates a memory region in the cache. Please note that the
*   data in the cache would be written back to main memory if dirty and the 
*   region invalidated. So any further access to the data would result in a 
*   cache-miss.
*   @param[in] addr   Start address of memory region
*   @param[in] length Length of memory region
*
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void dcache_flush_region(void *addr, unsigned int length);

/*===========================================================================

**  Function :  memory_barrier

** ==========================================================================
*/
/*!
* 
* @brief
*   Creates a memory barrier  
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/  
void memory_barrier
(
  void
);


/*===========================================================================

**  Function :  mmu_invalidate_tlb

** ==========================================================================
*/
/*!
* 
* @brief
*   Invalidates the entire Translation Look-aside Buffer (TLB) as a unified
*   operation (Data and Instruction). Invalidates all unlocked entries in the
*   TLB. Causes the prefetch buffer to be flushed. All following instructions
*   are fetched after the TLB invalidation.
*   We should do this after any change of page table  
* 
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/  
void  mmu_invalidate_tlb(void);


/*===========================================================================

**  Function :  mmu_relocate_page_table_base

** ==========================================================================
*/
/*!
* 
* @brief
*  Relocate the base register(TTBR0) of page table to the given address, 
*  all attributes of TTBR0 will be perserved
* 
* @par Dependencies
*   None
*
* @param[in] dst_l1_pagetable address of the destination page table base.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/  
void mmu_relocate_page_table_base(uint32 *dst_l1_pagetable);

/* SBL L1 and L2 page table defines */
extern uint32 mmu_l1_pagetable[];
extern uint32 mmu_l2_pagetable[];

/*===========================================================================

**  Function :  mmu_set_dacr

** ==========================================================================
*/
/*!
* 
* @brief
*  Configure DACR, Domain Access Control Register.
* 
* @param[in] 
*  uint32 dacr_value:   Value to be written to DACR.
* 
* @par Dependencies
*  None
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void mmu_set_dacr( uint32 dacr_value);


/*===========================================================================

**  Function :  mmu_get_page_table_base

** ==========================================================================
*/
/*!
* 
* @brief
*  Return the page table base address in TTBR0. Attributes bits are cleared
* 
* @par Dependencies
*  None
* 
* @retval
*  page table base address in TTBR0
* 
* @par Side Effects
*  None
* 
*/
uint32 *mmu_get_page_table_base(void);


/*===========================================================================
**  Function :  boot_l2_page_table_init
** ==========================================================================
*/
/*!
* 
* @brief
*   Calculate the number of L2 page table entries available and set L2
    page table pointer.
* 
* @param[in]
*   uint32 * mmu_l1_pagetable_base
*   uint32   mmu_pagetable_size
*   
* @par Dependencies
*   None
*   
* @retval
*   None                           
* 
* @par Side Effects
*   None
* 
*/
void boot_l2_page_table_init(uint32 * mmu_l1_pagetable_base,
                             uint32 mmu_pagetable_size);


#endif /* BOOT_CACHE_MMU_H */
