#ifndef BOOT_MMU_TABLE_H
#define BOOT_MMU_TABLE_H

/*=============================================================================

                      MDM9x35 MMU Table

GENERAL DESCRIPTION
    This module contains memory map for MSM9x35 SBL.

Copyright 2010-2012 by Qualcomm Technologies Incorporated. All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/boot_mmu_table.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/04/14   jz      Update page table for SBL L2 region DEP at runtime
08/26/14   jz      Cleanup page table entries
07/23/14   jz      Updated TZ and SMEM regions, no need to align SMEM region
07/11/14   jz      Fixed to mark L2/TCM as non-cacheable
06/13/14   jz      Make SMEM entry 1MB aligned and round up to 1MB; fixed TZ entry
06/02/14   jz      Updated memory maps 
04/23/14   ck      Mark the Security Control Block region as read only
02/24/14   jz      Added QSEE/QHEE regions
09/04/13   kedara  Updated CACHE to INNER_WBWACACHE_OUTER_NONCACHE
06/19/13   kedara  Initial revision for 9x35

===========================================================================*/

#define SIZE_MASK_4KB 0xfffffffC

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_cache_mmu.h"
#include "boot_page_table_armv7.h"


/*=============================================================================
Memory Map Structure
  PBL only maps a limited number of memories: 
  IMEM, RPM CODE RAM, APPS PBL ROOM, L2 locked region.
  SBL1 is responsible to map the memory it will use such as DDR and ram dump regions.  

  Memory regions greater than or equal to 1MB are placed in the Level 1 pagetables. 
  The rest are placed in Level 2 pagetables. All regions are marked with the default 
  domain.
=============================================================================*/



/*=============================================================================
    SBL initial Memory Map  
    This memory map is populated before DDR is intialized.
    Regions that have XPU protection turned on should be marked as 
    DEVICE memory(strongly ordered) to eliminate the processor doing 
    speculative data access and NON-EXECUTABLE to eliminate 
    speculative instruction execution.
=============================================================================*/

const struct mem_block sbl_initial_memory_map[] =
{ 
  /* On Bear family PBL only supplies 4 L2 page table entries.
     Note: 1 L2 page table entry spans 1MB range. (256 x 4K)
     Three of them are used already by PBL but can be reused if memory
     region is the same. */

  /* OCIMEM */
  { SCL_IMEM_BASE, SCL_IMEM_BASE,
    SCL_IMEM_SIZE >> 10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_DEVICE_SHARED, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },
    
  /* RPM CODE RAM */
  { SCL_RPM_CODE_RAM_BASE, SCL_RPM_CODE_RAM_BASE, 
    SCL_RPM_CODE_RAM_SIZE>>10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_DEVICE_SHARED, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },
    
  /* RPM Data Ram */
  { SCL_RPM_DATA_RAM_BASE, SCL_RPM_DATA_RAM_BASE, 
    SCL_RPM_DATA_RAM_SIZE>>10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_DEVICE_SHARED, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  /* Map DDR as shared device before DDR is initialized,
     DDR driver may need to access DDR during size detection. 
     Here DDR must be mapped as shared device to prevent any speculative access
     since DDR is not available.
     we will add the actual DDR region as cacheable after DDR is initialized */
  { DDR_MEM_BASE, DDR_MEM_BASE,
    DDR_MEM_SIZE>>10,
    MMU_L1_SECTION_MAPPING, MMU_L1_PAGETABLE_MEM_READ_WRITE, 
    MMU_L1_PAGETABLE_DEVICE_SHARED, MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },
  
  /* Debug image & DDR driver region, must be marked as executable.
     Reusable L2. */
  { 
    SCL_SYSTEM_DEBUG_CO_RO_BASE,
    SCL_SYSTEM_DEBUG_CO_RO_BASE, 
    SCL_SYSTEM_DEBUG_CO_RO_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_ONLY,
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L2_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  /* Debug image & DDR driver region, must be marked as non-executable.
     Reusable L2. */
  { SCL_SYSTEM_DEBUG_DATA_BASE,
    SCL_SYSTEM_DEBUG_DATA_BASE, 
    SCL_SYSTEM_DEBUG_DATA_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  /* DO NOT add any addtional memory region as L2 section, 
     there is no more L2 page table available in IMEM page table*/
    
  /* Last Region Marker */
  { MMU_LAST_ADDRESS, MMU_LAST_ADDRESS }

};


/*=============================================================================
    SBL1 Addtional Memory Map  
    
    Addtional memory regions that need to be mapped in SBL1. 
    This memory map is populated after DDR is intialized.
    Regions that have XPU protection turned on should be marked as 
    DEVICE memory(strongly ordered) to eliminate the processor doing 
    speculative data access and NON-EXECUTABLE to eliminate 
    speculative instruction execution.
=============================================================================*/
const struct mem_block sbl1_addtional_memory_map[] =
{    
  /*SBL1 DDR, inner cacheable outter non-cacheable */  
  { SCL_SBL1_DDR_DATA_BASE, SCL_SBL1_DDR_DATA_BASE, 
    SCL_SBL1_DDR_DATA_SIZE >> 10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },   

  /*SBL1 DDR Page table, non-cacheable 
    Page table region must be mapped as non-cachable to be consistent with 
    the setting in TTBR0(we set page table walk as non-cachable)*/  
  { SCL_SBL1_DDR_PAGE_TABLE_BASE, SCL_SBL1_DDR_PAGE_TABLE_BASE, 
    SCL_SBL1_DDR_PAGE_TABLE_SIZE >> 10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },       
    
  /*SBL1 USB buffer, non-cacheable */  
  { SCL_DLOAD_BUFFERS_BASE, SCL_DLOAD_BUFFERS_BASE, 
    SCL_DLOAD_BUFFERS_SIZE >> 10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },     
    
  /* SMEM Region, Shared memory */ 
  { SCL_SHARED_RAM_BASE, SCL_SHARED_RAM_BASE, 
    SCL_SHARED_RAM_SIZE>>10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE_SHARED, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },
        
  /*QSEE, inner cacheable outter non-cacheable, non-executable */  
  { SCL_QSEE_CODE_BASE,
    SCL_QSEE_CODE_BASE, 
    SCL_QSEE_TOTAL_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_DEVICE_SHARED,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },   

  /* Security control region as read-only. */
  { SECURITY_CONTROL_BASE,
    SECURITY_CONTROL_BASE, 
    QFPROM_READONLY_BLOCK_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_ONLY, 
    MMU_L2_PAGETABLE_DEVICE_SHARED,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  
  /* Add addtional memory region here */
  
  /* Last Region Marker */
  { MMU_LAST_ADDRESS, MMU_LAST_ADDRESS }
};



#endif  /* BOOT_MMU_TABLE_H */
