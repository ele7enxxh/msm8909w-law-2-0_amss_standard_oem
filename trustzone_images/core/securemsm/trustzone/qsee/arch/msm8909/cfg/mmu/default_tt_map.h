#ifndef DEFAULT_TT_MAP_H
#define DEFUALT_TT_MAP_H

/*=============================================================================

                          TZBSP MMU Table

GENERAL DESCRIPTION
    This module contains memory map for EBI and SMI regions.
    This memory map is used for Scorpion MSM8660 mmu table.

Copyright 2011-2012 by Qualcomm Technologies Incorporated. All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/default_tt_map.h#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/26/11   jct     Factored out MobiCore code
01/26/11   pre     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tzbsp_target.h"
#include "tt_tz.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_memory_map.h"
#include "tzbsp_mem.h"

#define TZ_DEFAULT_MEM_CFG (TZ_READ_WRITE_PL1 | TZ_MEM_DEVICE |   \
                            TZ_NON_EXECUTABLE | TZ_INR_SHAREABLE)

/*=============================================================================
Memory Map Structure

  The memory map structure is an array of mem blocks defined below.
  They are used by the function mmu_create_tt to create the mmu
  translation tables.  Each entry represents a memory block and
  consists of a base address, size in kilobytes, and memory
  attributes.

  Initially, we mark all of the region to be device shared as most of
  the regions besides the ROM, IMEM and RAM are device peripherals.

  Memory regions greater than or equal to 1MB are placed in the Level
  1 translation tables.  The rest are placed in Level 2 translation
  tables.
=============================================================================*/

/* These memory structures are placed into the translation tables
   sequentially from top to bottom.  The strategy is as follows:

   1) Map the entire virtual address range as DEVICE memory with a 1:1
      relation to physical memory.  This is the "default" memory property
      for all 4 GB of addressable memory by TZBSP.

   2) Map the various statically available regions of IMEM in L2
      translation tables for IMEM.
 */
#define VENUS_XPU_START_ADDR_PHYS 0x01DF0000
#define VENUS_XPU_START_ADDR     (VENUS_XPU_START_ADDR_PHYS + \
									  TZBSP_SANDBOX_RELOCATE_OFFSET)
#define VENUS_XPU_END_ADDR_PHYS   0x01E00000

mem_block_type default_memory_map[] =
{
  /* Mark the entire virtual address space as device memory by default */
  { TZ_PAS_BASE,                                 /* Physical address start */
    TZ_VAS_BASE,                                 /* Virtual address start */
    (TZ_VAS_SIZE >> 10),                         /* Size of mapping area */
    TZ_DEFAULT_MEM_CFG },                        /* Bitfield of config mem
                                                    type and permissions. */
#ifdef TZBSP_WITH_TBASE
  /* Remap 0-32 MB to an unused memory area to allow tbase to use
     0-32MB and allow TZBSP to access peripherals in that area */
  { 0x0, TZBSP_SANDBOX_RELOCATE_OFFSET, (TZBSP_SANDBOX_END >> 10),
    (TZ_READ_WRITE_PL1 | TZ_MEM_DEVICE | TZ_NON_EXECUTABLE) },

  /* Mark 0-32MB as zeros to create a page fault if TZBSP still tries
     to access this area */
  { 0x0, 0x0, (TZBSP_SANDBOX_END >> 10), TZ_TTE_INVALID },

#else
  /* Marking the first 4k as invalid	   | L2 PT 3 */
  { 0x0, 0x0, TZBSP_PAGE_4K>>10, TZ_TTE_INVALID },
#endif

  /* 
   * There is a HW bug which causes back to back writes to venus xpu space to
   * get corrupted or return a noc error. The workaroudn is to make venus
   * xpu space as strongly ordered
   */
  { VENUS_XPU_START_ADDR_PHYS, VENUS_XPU_START_ADDR, (VENUS_XPU_END_ADDR_PHYS - VENUS_XPU_START_ADDR_PHYS)>>10,
    (TZ_READ_WRITE_PL1 | TZ_MEM_STRONGLY_ORDERED | TZ_INR_SHAREABLE |
     TZ_NON_EXECUTABLE)},

  /* TZBSP Image IMEM Default                   | L2 PT 0 */
  { TZBSP_CODE_BASE, TZBSP_CODE_BASE, TZBSP_MAX_IMAGE_SIZE>>10,
    (TZ_READ_WRITE_PL1 | TZ_MEM_WRITE_BACK_WA | TZ_INR_SHAREABLE |
     TZ_NON_EXECUTABLE)},

  /* TZBSP Heap must be non-cacheable, non-exec | L2 PT 0 */
  { TZBSP_HEAP_BASE, TZBSP_HEAP_BASE, TZBSP_HEAP_SIZE>>10,
    (TZ_READ_WRITE_PL1 | TZ_MEM_NON_CACHEABLE | TZ_INR_SHAREABLE |
     TZ_NON_EXECUTABLE) },

  /* TZBSP non-cached region is non-exec        | L2 PT 0 */
  { TZBSP_UNCACHED_ZI_BASE, TZBSP_UNCACHED_ZI_BASE, TZBSP_UNCACHED_ZI_SIZE>>10,
    (TZ_READ_WRITE_PL1 | TZ_MEM_NON_CACHEABLE | TZ_INR_SHAREABLE |
     TZ_NON_EXECUTABLE) },

  /* TZBSP Diag must be non-cacheable, non-exec | L2 PT 0 */
  { TZBSP_DIAG_BASE, TZBSP_DIAG_BASE, TZBSP_DIAG_SIZE>>10,
    (TZ_READ_WRITE_PL1 | TZ_MEM_NON_CACHEABLE | TZ_INR_SHAREABLE |
     TZ_NON_EXECUTABLE) },

  /* Apps Bootrom (for secboot), executable     | L2 PT 1 */
  { TZBSP_BOOT_ROM_APPS_BASE, TZBSP_BOOT_ROM_APPS_BASE,
    TZBSP_ROUNDUP(uint32, TZBSP_BOOT_ROM_APPS_SIZE, MIN_MAP_SIZE)>>10,
    (TZ_READ_ONLY_PL1 | TZ_MEM_WRITE_BACK_WA | TZ_INR_SHAREABLE |
     TZ_EXECUTABLE) },

#ifdef TZBSP_WITH_TBASE
  /* External ELF L1 translation tables         | L2 PT 2 */
  { TZBSP_EXT_ELF_TT_BASE, TZBSP_EXT_ELF_TT_BASE, TZBSP_L1_TT_SIZE>>10,
    (TZ_READ_WRITE_PL1 | TZ_MEM_NON_CACHEABLE | TZ_INR_SHAREABLE |
     TZ_NON_EXECUTABLE) },
#endif

  /* Last Region Marker */
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },

  /* --------------------------------------------- */
  /* Placeholders for initial mapping information: */

  /* TZBSP Code                                 | L2 PT 0 */
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },

  /* TZBSP RO                                   | L2 PT 0 */
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },

  /* DDR Invalidation */
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },

  /* 4 DDR memory regions */
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },

  /* Secure application region */
  { TZ_VAS_LAST_ADDRESS, TZ_VAS_LAST_ADDRESS },

};

#endif  /* TZBSP_MMU_PT_DEF_H */
