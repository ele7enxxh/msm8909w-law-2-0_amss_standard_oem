#ifndef TZBSP_TARGET_MMU_MAIN_H
#define TZBSP_TARGET_MMU_MAIN_H

/*===========================================================================

                    T r u s t z o n e   T a r g e t
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the TZBSP
  that is target-specific

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/tzbsp_target.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/11   pre     Remove Mobicore and leave MMU changes
03/19/11   pre     MMU and MobiCore related changes
05/05/10   tk      DMOV related changes
04/22/10   ejt     removed comdef.h since it is not needed
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "../common/tzbsp_target.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define TZBSP_MAX_IMAGE_SIZE SCL_TZ_TOTAL_SIZE
/**
 * Defines a virtual address region for secure application use.
 * Values of zero indicate no need for a sandbox region.
 *
 * NOTE: The sandbox must start from address 0x00000000
 */
#define TZBSP_SANDBOX_END             0x02000000

/**
 * Defines the area of memory to relocate the memory map that will be
 * used for the sandbox.  A value of zero indicates no sandbox is
 * used.
 */
#ifdef TZBSP_WITH_TBASE
#define TZBSP_SANDBOX_RELOCATE_OFFSET 0x04800000
#else
#define TZBSP_SANDBOX_RELOCATE_OFFSET 0x0
#endif
/*---------------------------------------------------------------------------
  MMU Specific Definitions
---------------------------------------------------------------------------*/
/* TZ will map all 4 GB of addressable memory */
#define TZ_PAS_BASE                0x00000000
#define TZ_VAS_BASE                TZ_PAS_BASE
#define TZ_VAS_SIZE                TZBSP_4GB
#define TZ_VAS_LAST_ADDRESS       (TZ_VAS_BASE + TZ_VAS_SIZE - 1)

/*TZ code partition range in DDR */
#define TZBSP_TZ_REGION_SIZE           0x00080000
#define TZBSP_EBI1_TZ_CODE_BASE        SCL_TZ_CODE_BASE
#define TZBSP_EBI1_TZ_CODE_END         (SCL_TZ_CODE_BASE + TZBSP_TZ_REGION_SIZE)
#define TZBSP_EBI1_TZ_CODE_SIZE        (TZBSP_EBI1_TZ_CODE_END - \
                                       (TZBSP_EBI1_TZ_CODE_BASE))

/* Size of the entire L1 translation table, 16 KB */
#define TZBSP_L1_TT_SIZE             0x4000

/* Both L1 and L2 translation table entries are 4 bytes */
#define TZBSP_TTE_SIZE 4

/*---------------------------------------------------------------------------
  Define the base address for the L1/L2 PTs and stack for SCL file
---------------------------------------------------------------------------*/

#define TZBSP_L1_PT_BASE         ((TZBSP_CODE_BASE +        \
                                   TZBSP_EBI1_TZ_CODE_SIZE) -  \
                                  TZBSP_L1_TT_SIZE)

#define TZBSP_STACK_BASE         (TZBSP_L1_PT_BASE)

/* Define the heap base as relative to the stack.  Since the stack
 * base is the high end of the stack range and the heap base is the
 * low end of the heap range, the offset from the stack base is both
 * the stack size and the heap size
 */
#define TZBSP_HEAP_BASE          (TZBSP_STACK_BASE -        \
                                  (TZBSP_HEAP_SIZE +        \
                                   TZBSP_STACK_SIZE))

#define TZBSP_UNCACHED_ZI_BASE   (TZBSP_HEAP_BASE -         \
                                  TZBSP_UNCACHED_ZI_SIZE)


#define TZBSP_DIAG_BASE          (TZBSP_CODE_BASE + 0x15F000)


/*Carve out size in IMEM for demand paging*/
#define TZBSP_DP_IMEM_SIZE       0x0
#define TZBSP_DP_IMEM_RCLM_SIZE  0x0  /* No IMEM to be reclaimed after milestone */
#define TZBSP_DP_IMEM_RCLM_BASE  0x0

/* Additional definitions for demand paging */
#define TZBSP_DEMAND_PAGE_SIZE 0x1000  //4kb
#define TZBSP_DP_HASH_SIZE     32

/* Region in TZ DDR to store copy of amt table and its hash value */
#define TZBSP_AMT_DDR_BASE        (TZBSP_CODE_BASE + 0x0014D000)
#define TZBSP_AMT_DDR_SIZE        0x210

#endif /* TZBSP_TARGET_MMU_MAIN_H */
