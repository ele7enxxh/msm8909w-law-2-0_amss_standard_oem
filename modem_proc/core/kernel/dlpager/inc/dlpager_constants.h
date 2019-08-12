#ifndef __DLPAGER_CONSTANTS_H__
#define __DLPAGER_CONSTANTS_H__

/*===========================================================================
 * FILE:         dlpager_constants.h
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  Constant values used within dlpager
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/inc/dlpager_constants.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/15   ao      Initial revision
===========================================================================*/

#ifndef assert
#define assert ASSERT
#endif

//#define DLPAGER_PRINT_DEBUG
#ifdef DLPAGER_PRINT_DEBUG
#include <stdio.h>
#define DL_DEBUG printf
#else
#define DL_DEBUG(...)
#endif

#include "core_variation.h"

#define DLPAGER_QURT_SUPPORTS_SSR
#define DLPAGER_CBSP_SUPPORTS_HEAP_LITE
//#define DLPAGER_WRITE_DETECT_ENABLED

#define PAGE_SIZE                 4096
#define DLPAGER_PAGE_NUM_SHIFT    12
#define CACHE_LINE_SHIFT          (5)
#define CACHE_LINE_SZ             (1 << CACHE_LINE_SHIFT)

#define CAUSE_ILLEGAL_INSTRUCTION 0x1D /* Packet with multiple writes to the same destination register */
#define CAUSE_PRIVILEGE_EXCEPTION 0x23 /* Privilege violation */
#define CAUSE_MISS_EXECUTE        0x60 /* Due to missing Fetch address on PC-page */
#define CAUSE_MISS_EXECUTE_BORDER 0x61 /* Due to missing Fetch on second page from packet that spans pages */
#define CAUSE_MISS_READ           0x70 /* Due to memory read */
#define CAUSE_MISS_WRITE          0x71 /* Due to memory write */
#define CAUSE_ANY_TLB_MISS        0xFF /* Used within dlpager when QuRT doesn't support passing SSR */

/* size is a power of 2 */
#define DLPAGER_ALIGN_DOWN(addr, size) (addr & ~(size - 1))
#define DLPAGER_ALIGN_UP(addr, size) ((addr + size - 1) & ~(size - 1))

#endif
