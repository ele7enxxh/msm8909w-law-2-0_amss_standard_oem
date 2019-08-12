/*=============================================================================

                       Boot Page Table

GENERAL DESCRIPTION
  This file contains definitions of functions for setting up a page table in SBL

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_page_table.c#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/08/14   ck      Added boot_l2_page_table_init
03/26/13   dh      in both L1 and L2 mapping, round up to page size alignment
10/17/12   dh      Fix the bug that when relocating L2 page tables L1 descriptor
		           is not updated to point to the new L2 table.
05/24/12   dh      Change boot_mmu_relocate_page_table to boot_mmu_copy_page_table
03/27/12   dh      Added boot_mmu_page_table_map_single_mem_block,
                   rename boot_mmu_populate_page_table_mem_map to
                   boot_mmu_page_table_map_mem_block_list
03/14/12   dh      Add boot_mmu_relocate_page_table for relocating page table.
                   Pass l1 page table base as an argument for all the page table APIs
                   
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_page_table_armv7.h"
#include "boot_cache_mmu.h"
#include "boot_util.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* SBL L1 and L2 page table */
#pragma arm section zidata = "BOOT_PAGE_TABLE_ZONE"
__align(16*1024) uint32 mmu_l1_pagetable[MMU_L1_PT_SIZE];
__align(1024) uint32 mmu_l2_pagetable[MMU_L2_PT_SIZE*MMU_NUM_L2_PT];
#pragma arm section zidata 

/* Pointer to the next empty L2 page table we are going to use */
static uint32 *empty_l2_pagetable_ptr = NULL;

/* Number of empty L2 page table left */
static uint32 empty_l2_pagetable_num = 0;


/*===========================================================================

**  Function :  mmu_map_block_as_l1_sections

** ==========================================================================
*/
/*!
* 
* @brief
*   Maps a block descriptor into the page table as l1 sections
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
* 
* @par Dependencies
*   mmu_l1_pagetable must be allocated
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
static void mmu_map_block_as_l1_sections
(
  uint32 *mmu_l1_pagetable, 
  const struct mem_block *block
)
{
  uint32 address_base_index;
  uint32 limit;
  uint32 p_addr;

  /* Convert our base address into an index into the page table */
  address_base_index = block->v_base >> 20;

  /* only map 1MB size sections ,  round up to 1MB alignment*/
  limit = address_base_index + ((block->size_in_kbytes +1023) >> 10);
  
  /* high 12 bits provide the physical base address of the section */
  p_addr = block->p_base & 0xFFF00000; 


  for( ; address_base_index < limit; address_base_index++)
  {
    /*
    _________________________________________________________________________________
   |                 |     |  |  |    |     |      | |      |   |         |          |
   |31-------------20|19|18|17|16|-15-|14-12|11--10|9|8----5|-4-|3-------2|1------- 0|
   |Phys Address base|nS|0 |nG| S| APX| TEX |Access|P|DOMAIN| XN|Cach/Buff|Descriptor|
   |_________________|_____|__|__|____|_____|______|_|______|___|_________|__________|

    */
    
      mmu_l1_pagetable[address_base_index] =  (p_addr)                   |
                                              (block->access)            |
                                              (block->cachetype)         |
                                              MMU_PAGETABLE_ECC_DISABLED |
                                              (block->domain)            |
                                              (block->executable)        |
                                              MMU_PAGETABLE_SECTION_DESCRIPTOR;
      
      /* If this L1 section is NS then update the NS bit of L1 section pagetable entry*/
      if(block->memory_mapping == MMU_L1_NS_SECTION_MAPPING)
      {
        mmu_l1_pagetable[address_base_index] |= MMU_L1_PAGETABLE_SECTION_NS;
      }
      
      p_addr += 0x100000;
  }
   
} /* mmu_map_block_as_l1_sections */

 
/*===========================================================================

**  Function :  mmu_map_block_as_l2_pages

** ==========================================================================
*/
/*!
* 
* @brief
*   Maps a block descriptor into the page table as l2 pages
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
* 
* @par Dependencies
*   mmu_l1_pagetable must be allocated
*   There must be empty l2 page table left if new L2 page table is required
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
static void mmu_map_block_as_l2_pages
(
  uint32 *mmu_l1_pagetable,
  const struct mem_block *block 
)
{
  uint32 *l2_page_table;
  uint32  address_base_index, i;
  uint32  limit;
  uint32  p_addr;
  boot_boolean  l2_pt_exist = TRUE;
  
  /* First initialize the first level descriptor for each 1 MB
     High 22 bits provide the physical base address of the level 2 page table

     ________________________________________________________________________
    |                                                                       |
    |31----------------------------------------------10|9|8------5|4|3 |2|1 0|
    |         level 2 PT Address base                  |0| DOMAIN |0|nS|0|0 1|
    |_______________________________________________________________________|
        
  */
  
  /* Convert the virtua address[31:20] into an index of the L1 page table */
  address_base_index = block->v_base >> 20;  
  
  /* Check if this 1MB entry has L2 page table mapped already */
  
  do
  {
    /* If L1 entry hasn't mapped any L2 page table, allocate a L2 page table for it */
    if((mmu_l1_pagetable[address_base_index] & 0x1) == 0)
    {
      /* First check if we have any empty L2 page table left */
      if(empty_l2_pagetable_num <= 0)
      {
        break;
      }
      
      /* We will now use the next empty l2 page table for this mapping
         Point L1 entry to the empty L2 page table*/
      l2_page_table = empty_l2_pagetable_ptr;
      mmu_l1_pagetable[address_base_index] = ((uint32)l2_page_table & 0xFFFFFC00)  |
                                             (block->domain)                       |             
                                             MMU_PAGETABLE_SECONDARY_PAGE_DESCRIPTOR;
      
      /* If this L2 section is NS then update the NS bit of L1 coarse pagetable entry */
      if(block->memory_mapping == MMU_L2_NS_SECTION_MAPPING)
      {
        mmu_l1_pagetable[address_base_index] |= MMU_L1_PAGETABLE_COARSE_NS;
      }
      
      /* Advance pointer to next empty l2 page table */
      l2_pt_exist = FALSE;
      empty_l2_pagetable_ptr += MMU_L2_PT_SIZE;
      empty_l2_pagetable_num--;
    }
    
    /* Entry has L2 page table mapped already, so just get the existing L2 page table address */
    else
    {
      l2_page_table = (uint32*)(mmu_l1_pagetable[address_base_index] & 0xFFFFFC00);  
    }
    
    /* The start physical address for the 1MB section placed inside the L2 page table */
    p_addr = block->p_base & 0xFFF00000;   
    
    /* Start index into the L2 page table for this section using the virtual address[19:12]*/
    address_base_index  = (block->v_base & 0x000FFFFF) >> 12;
    
    /* The end index for the given section. This includes the total number of 4KB segments 
      in the given section, round up to 4K alignment */
    limit = address_base_index + ((block->size_in_kbytes + 3) >> 2);
    
    
    /*
    ______________________________________________________________________
    |                        |  |  |   |     |       |        |    |
    |31---------------------------12|11|10| 9 |8 7 6|5 --- 4|3-------2|1  0|
    |     Phys Address base         |nG|S |APX| TEX | Access|Cach/Buff|1 XN| 
    |_______________________________|__|__|___|_____|_______|_________|____|
    
    */  
    
    /* Map all the 4KB segments in the 1MB section */
    for (i = 0; i < MMU_L2_PT_SIZE; i++)
    {
      /* If the index lies between the base index and the limit, then this is the given section 
      we need to map */
      if ((i >= address_base_index)  && (i < limit))
      {
        l2_page_table[i] =  (p_addr)                 |
                            (block->access)          |
                            (block->cachetype)       |
                            (block->executable)      |
                            MMU_PAGETABLE_SECTION_DESCRIPTOR;
      }
      /* If the 1MB was previously not mapped, 
        then put the remaining sections to default attributes: rw, nonexe, device*/
      else if(!l2_pt_exist)
      {
        l2_page_table[i] = (p_addr)                                 |
                           MMU_L2_PAGETABLE_MEM_READ_WRITE          |
                           MMU_L2_PAGETABLE_DEVICE_SHARED           |
                           MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION   |
                           MMU_PAGETABLE_SECTION_DESCRIPTOR;
      }
      
      /* Increment to the next 4KB segment in current L2 page table*/
      p_addr += 0x1000;
    }
    
  }while(0);
  
} /* mmu_map_block_as_l2_sections */


/*===========================================================================

**  Function :  boot_mmu_page_table_map_mem_block_list

** ==========================================================================
*/
/*!
* 
* @brief
*   Populates a pagetable with the content of a memory map list.
* 
* @par Dependencies
*   None
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
*
* @param[in] map Pointer to start of the memory map list
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
)
{
  boot_boolean result = TRUE;
  uint32 empty_l2_pt_needed = 0;
  const struct mem_block  *map_ptr;
  
  /*First caclulate the number of empty L2 page tables needed*/
  for(map_ptr = map; map_ptr->v_base != MMU_LAST_ADDRESS; map_ptr++)
  {
    if(map_ptr->memory_mapping == MMU_L2_SECTION_MAPPING ||
       map_ptr->memory_mapping == MMU_L2_NS_SECTION_MAPPING)
      {
        if((mmu_l1_pagetable[map_ptr->v_base >> 20] & 0x1) == 0)
        {
          empty_l2_pt_needed++;
        }
      }
  }
  
  /* If we have enough L2 page tables to use then do the mapping*/
  if(empty_l2_pt_needed <= empty_l2_pagetable_num)
  {
    for (map_ptr = map; map_ptr->v_base != MMU_LAST_ADDRESS; map_ptr++)
    {
      if(map_ptr->memory_mapping == MMU_L1_SECTION_MAPPING ||
         map_ptr->memory_mapping == MMU_L1_NS_SECTION_MAPPING)
      {
        mmu_map_block_as_l1_sections(mmu_l1_pagetable, map_ptr);
      }
      else
      {
        mmu_map_block_as_l2_pages(mmu_l1_pagetable, map_ptr);
      }
    }
    /* Invalidate the TLB after map is updated */
    mmu_invalidate_tlb();
  }
  else
  {
    result = FALSE;
  }

  return result;
} /* boot_mmu_page_table_map_mem_block_list */


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
)
{
  uint32 i;
  uint32 src_l2_pagetable_num = 0;
  boot_boolean result = TRUE;
  
  do
  {
    /* First we need to count the number of L2 page tables the source
       L1 page table is using */
    for(i = 0; i< MMU_L1_PT_SIZE; i++)
    {
      if((src_l1_pagetable[i] & 0x1) != 0)
      {
        src_l2_pagetable_num++;
      }
    }
    
    /* Apprearently if the source has more L2 table than the destination,
       we can not copy*/
    if(src_l2_pagetable_num > dst_l2_pagetable_num)
    {
      result =  FALSE;
      break;
    }
    
    /* Set the number of emtpy L2 page table we can use*/
    empty_l2_pagetable_ptr = dst_l2_pagetable;
    empty_l2_pagetable_num = dst_l2_pagetable_num;
    
    /* Now copy the entire page table */
    for(i = 0; i< MMU_L1_PT_SIZE; i++)
    {
      /* First copy the L1 page table entry */
      dst_l1_pagetable[i] = src_l1_pagetable[i];
      
	  /*If current L1 page table entry points to a L2 page table*/
      if((src_l1_pagetable[i] & 0x01) != 0)
      {        
        /* Copy over the source L2 page table and 
           decrement the number of empty L2 page table*/
        qmemcpy(empty_l2_pagetable_ptr, 
                (void *)(src_l1_pagetable[i] & 0xFFFFFC00), 
                MMU_L2_PT_SIZE * sizeof(uint32));
        
		/* Update the L1 descriptor in dst page table to point
		   to the new dst L2 page table, keep the attributes the same*/
		dst_l1_pagetable[i] = ((uint32)empty_l2_pagetable_ptr & 0xFFFFFC00)|
		                      (src_l1_pagetable[i] & 0x000003FF);
		
        empty_l2_pagetable_ptr += MMU_L2_PT_SIZE;
        empty_l2_pagetable_num--;
      }
    }
  }while(0);
    
  return result;
} /* boot_mmu_relocate_page_table */



/*===========================================================================

**  Function :  boot_mmu_page_table_map_single_mem_block

** ==========================================================================
*/
/*!
* 
* @brief
*   Populates a pagetable with the content of a single memory map block.
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
)
{
  uint32 empty_l2_pt_needed = 0;
  boot_boolean result = TRUE;
  
  do
  {
    if(map_ptr->memory_mapping == MMU_L1_SECTION_MAPPING ||
      map_ptr->memory_mapping == MMU_L1_NS_SECTION_MAPPING)
    {
      mmu_map_block_as_l1_sections(mmu_l1_pagetable, map_ptr);
    }
    else
    {
      if((mmu_l1_pagetable[map_ptr->v_base >> 20] & 0x1) == 0)
      {
        empty_l2_pt_needed++;
      }
        /* If we have enough L2 page tables to use then do the mapping*/
      if(empty_l2_pt_needed <= empty_l2_pagetable_num)
      {
        mmu_map_block_as_l2_pages(mmu_l1_pagetable, map_ptr);
      }
      else
      {
        result = FALSE;
        break;
      }
    }
    
    /* Invalidate the TLB after map is updated */
    mmu_invalidate_tlb();
  
  }while(0);
  
  return result;
}


/*===========================================================================
**  Function :  boot_l2_page_table_init
** ==========================================================================
*/
/*!
* 
* @brief
*   Calculate the number of L2 page table entries available and set l2
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
                             uint32 mmu_pagetable_size)
{
  uint32 * current_l1_entry = NULL;
  uint32 current_l2_entry = NULL;

  uint32 max_l1_pagetable_size = MMU_L1_PT_SIZE * sizeof(uint32);
  uint32 mmu_l2_pagetable_base = 0;
  uint32 mmu_l2_pagetable_size = 0;
  

  /* Make sure there is room for L2 page tables. If not set the l2 pointer and num properly and exit. */
  if (mmu_pagetable_size <= max_l1_pagetable_size)
  {
    empty_l2_pagetable_ptr = NULL;
    empty_l2_pagetable_num = 0;

    return;
  }


  /* Calculate the l2 page table base and size. */
  mmu_l2_pagetable_base = (uint32)mmu_l1_pagetable_base + max_l1_pagetable_size;
  mmu_l2_pagetable_size = mmu_pagetable_size - max_l1_pagetable_size;
   

  /* Calculate maximum number of L2 page tables available.  This will not be
     the final number as some could be in use already by PBL. */
  empty_l2_pagetable_num = mmu_l2_pagetable_size / (MMU_L2_PT_SIZE * sizeof(uint32));
 

  /* Set l2 pointer to first unused entry. If no entries are available
     then set l2 pointer and num appropriately and exit. */
  empty_l2_pagetable_ptr = (uint32 *)mmu_l2_pagetable_base;
  while (1)
  {
    if (*empty_l2_pagetable_ptr == NULL)
    {
      break;
    }

    /* Bump the l2 pointer to next possible L2 entry. */
    empty_l2_pagetable_ptr += MMU_L2_PT_SIZE;

    /* Make sure pointer is within L2 page table area.  If it isn't then
       all L2 entries have been exhausted. */
    if ((uint32)empty_l2_pagetable_ptr >= mmu_l2_pagetable_base +
                                          mmu_l2_pagetable_size)
    {
      empty_l2_pagetable_ptr = NULL;
      empty_l2_pagetable_num = 0;
      return;
    }
  }

  
  /* L2 page table entries from PBL could be scattered and not consecutive.
     SBL needs the entries to be in order as it uses a walking pointer.
     Go through the l2 table and reorder the entries (if necessary), decrementing
     the available L2 page tables at the same time. */
  for (current_l1_entry = mmu_l1_pagetable_base;
       (uint32)current_l1_entry < mmu_l2_pagetable_base;
       current_l1_entry++)
  {
    if (*current_l1_entry & MMU_PAGETABLE_SECONDARY_PAGE_DESCRIPTOR)
    {
      current_l2_entry = *current_l1_entry & 
        ~MMU_L1_PAGETABLE_L2_PT_ADDRESS_BASE_MASK;


      /* L2 exists so subtract from overall available.  Before doing so make sure
         value isn't 0 as this would mean the calculations done earlier are wrong. */
      if (empty_l2_pagetable_num == 0)
      {
        empty_l2_pagetable_ptr = NULL;
        empty_l2_pagetable_num = 0;
        return;
      }
      empty_l2_pagetable_num--;


      /* If l2 entry address is greater than empty l2 pointer then it's not consecutive. */
      if (current_l2_entry > (uint32)empty_l2_pagetable_ptr)
      {
        /* Move l2 entry, zero old entry, and correct l1 entry */
        qmemcpy(empty_l2_pagetable_ptr,
                (const void *)current_l2_entry,
                MMU_L2_PT_SIZE * sizeof(uint32));

        qmemset((void *)current_l2_entry,
                MMU_L2_PT_SIZE * sizeof(uint32),
                0);

        /* Keep previous L2 attributes.  Only update L2 PT base address. */
        *current_l1_entry &= ~MMU_L1_PAGETABLE_L2_PT_ADDRESS_BASE_MASK;
        *current_l1_entry |= ((uint32)(empty_l2_pagetable_ptr) &
                              MMU_L1_PAGETABLE_L2_PT_ADDRESS_BASE_MASK);


        /* Find the next free L2 entry. */
        while (1)
        {
          if (*empty_l2_pagetable_ptr == NULL)
          {
            break;
          }
          
          /* Bump the _l2_entry pointer to next possible L2 entry. */
          empty_l2_pagetable_ptr += MMU_L2_PT_SIZE;
          
          /* Make sure pointer is within L2 page table area.  If it isn't then
             all L2 entries have been exhausted. */
          if ((uint32)empty_l2_pagetable_ptr >= mmu_l2_pagetable_base +
                                                mmu_l2_pagetable_size)
          {
            empty_l2_pagetable_ptr = NULL;
            empty_l2_pagetable_num = 0;
            return;
          }
        }
      }                
    } /* if (*current_l1_entry & MMU_PAGETABLE_SECONDARY_PAGE_DESCRIPTOR) */
  }


  /* Invalidate the table since we modified it. */
  mmu_invalidate_tlb();
}
