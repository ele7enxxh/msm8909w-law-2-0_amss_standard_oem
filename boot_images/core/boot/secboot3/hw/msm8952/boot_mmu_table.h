#ifndef __BOOT_MMU_TABLE_H__
#define __BOOT_MMU_TABLE_H__

/*=============================================================================

                      MSM8936 MMU Table

GENERAL DESCRIPTION
    This module contains the memory map for MSM8916 SBL.

Copyright 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/22/15   sk  	   Code changes for merging firehose DeviceProgrammer into SBL
05/01/14   ck      Refactored stack region name
04/23/14   ck      Mark the Security Control Block region as read only
03/18/14   ck      Made SBL L2/TCM cacheable for speed increase and setup DEP
02/27/14   ck      Removing RPM Data RAM as A53 does not have visibility
02/25/14   ck      Changed sbl1_ddr_mem_block to Inner cacheable / Outer non-cacheable
02/22/14   dhaval  defined sys_debug code, data regions
02/10/14   ck      Updated QHEE and APPSBL page table entry to match QSEE and NON_EXECUTABLE
02/06/14   ck      Updated SDI region names
01/30/14   kpa     Mark DDR driver region executable
01/13/14   ck      Changed QSEE DDR region to be covered by a L1 page table and
                   be Inner cacheable / Outer non-cacheable to resolve transition issue.
10/17/13   ck      Modified initial memory to only include IMEM and DDR.
                   Everything else is mapped after DDR initialization.
09/09/13   ck      Initial revision for 8916

===========================================================================*/

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

 /* SBL1 Code and Vectors.  Reusable L2. */
  { SCL_SBL1_CODE_BASE,
    SCL_SBL1_CODE_BASE,
    SCL_SBL1_CODE_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_ONLY,
    MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE,
    MMU_L2_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  /* SBL1 Stack and Data (RW + ZI).  Reusable L2 */
  { SCL_SBL1_STACKS_BASE,
    SCL_SBL1_STACKS_BASE,
    (SCL_SBL1_STACKS_SIZE + SCL_SBL1_DATA_SIZE) >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE,
    MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  /* IMEM.  New L2. */
  { SCL_IMEM_BASE,
    SCL_IMEM_BASE,
    SCL_IMEM_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  /* DDR region, must be marked as shared device */
  { DDR_MEM_BASE,
    DDR_MEM_BASE, 
    DDR_MEM_SIZE >> 10,
    MMU_L1_SECTION_MAPPING,
    MMU_L1_PAGETABLE_MEM_READ_WRITE, 
    MMU_L1_PAGETABLE_DEVICE_SHARED,
    MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
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
  /*SBL1 DDR ZI, L2 cacheable */  
  { SCL_SBL1_DDR_DATA_BASE,
    SCL_SBL1_DDR_DATA_BASE, 
    SCL_SBL1_DDR_DATA_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },   

  /*SBL1 DDR Page table, non-cacheable 
    Page table region must be mapped as non-cachable to be consistent with 
    the setting in TTBR0(we set page table walk as non-cachable)*/  
  { SCL_SBL1_DDR_PAGE_TABLE_BASE,
    SCL_SBL1_DDR_PAGE_TABLE_BASE, 
    SCL_SBL1_DDR_PAGE_TABLE_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },       
    
  /*SBL1 USB buffer, non-cacheable */  
  { SCL_DLOAD_BUFFERS_BASE,
    SCL_DLOAD_BUFFERS_BASE, 
    (SCL_SBL1_DDR_DATA_SIZE - SCL_SBL1_DDR_PAGE_TABLE_SIZE - SCL_SBL1_DDR_ZI_SIZE)>> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  }, 
    
  /* SMEM Region, Shared memory */ 
  { SCL_SHARED_RAM_BASE,
    SCL_SHARED_RAM_BASE, 
    SCL_SHARED_RAM_SIZE >> 10,
    MMU_L1_SECTION_MAPPING,
    MMU_L1_PAGETABLE_MEM_READ_WRITE, 
    MMU_L1_PAGETABLE_MEM_SHARED,
    MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  }, 

  /*QSEE, L2 cacheable, Non-Executable,
    marked as Executable before jumping to it. */  
  { SCL_QSEE_CODE_BASE,
    SCL_QSEE_CODE_BASE, 
    SCL_QSEE_TOTAL_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },   

  /*QHEE, L2 cacheable, Executable */  
  { SCL_QHEE_CODE_BASE,
    SCL_QHEE_CODE_BASE, 
    SCL_QHEE_TOTAL_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },   

  /*RPM, L2 Non-Cacheable, Non-Executable */  
  { SCL_RPM_CODE_RAM_BASE,
    SCL_RPM_CODE_RAM_BASE, 
    SCL_RPM_CODE_RAM_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },   
  /*RPM Data RAM, L2 Non-Cacheable, Non-Executable */  
  { SCL_RPM_DATA_RAM_BASE,
    SCL_RPM_DATA_RAM_BASE, 
    SCL_RPM_DATA_RAM_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },   

  /*APPSBL, L2 cacheable, Executable */  
  { SCL_APPSBL_CODE_BASE,
    SCL_APPSBL_CODE_BASE, 
    SCL_APPSBL_TOTAL_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE,
    MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION, 
    MMU_PAGETABLE_DEF_DOMAIN,  },   

  /* Security control region as read-only.  New L2. */
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


/* Data Execution Prevention(DEP) for SBL RW region
   All SBL region except code should be marked as non executable */
struct mem_block sbl1_rw_dep_mem_block = 
{    
  /*All SBL1 rw region should be read/write and non-executable*/
  0, 0, 0,
  MMU_L2_NS_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
  MMU_L2_PAGETABLE_INNER_WT_OUTER_WB_CACHE, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
  MMU_PAGETABLE_DEF_DOMAIN
};


/* Data Execution Prevention(DEP) for SBL CODE region
   All SBL region except code should be marked as non executable */
struct mem_block sbl1_code_dep_mem_block = 
{    
  /*All SBL1 code region should be read only and non-executable*/
  0, 0, 0,
  MMU_L2_NS_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_ONLY, 
  MMU_L2_PAGETABLE_INNER_WT_OUTER_WB_CACHE, MMU_L2_PAGETABLE_EXECUTABLE_REGION,
  MMU_PAGETABLE_DEF_DOMAIN
};


/* Memory block to describe DDR memory */
struct mem_block sbl1_ddr_mem_block = 
{
  0, 0, 0,
  MMU_L1_SECTION_MAPPING, MMU_L1_PAGETABLE_MEM_READ_WRITE, 
  MMU_L1_PAGETABLE_INNER_WBWA_OUTER_NONCACHE, MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
  MMU_PAGETABLE_DEF_DOMAIN
};

#endif  /* __BOOT_MMU_TABLE_H__ */
