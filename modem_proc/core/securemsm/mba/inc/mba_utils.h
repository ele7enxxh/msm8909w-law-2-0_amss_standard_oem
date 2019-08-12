#ifndef MBA_UTILS_H
#define MBA_UTILS_H

/**
@file mba_utils.h
@brief MBA Utility Api's
*/

/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/mba_utils.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
12/20/11   mm       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <IxErrno.h> 
#include "qurt.h"
#include "mba_error_handler.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define FOUR_KB 0x1000
#define SIXTEEN_KB 0x4000
#define SIXTY_FOUR_KB 0x10000
#define TWO_FIFTY_SIX_KB 0x40000
#define ONE_MB  0x100000
#define FOUR_MB  0x400000
#define SIXTEEN_MB 0x1000000
#define PAGE_SIZES 7
#define SIXTY_FOUR_MB (0x4000000)
#define ONE_TWENTY_EIGHT_MB (0x8000000)
#define TWO_FIFTY_SIX_MB (0x10000000)
#define MBA_MAX_UINT32 (0xFFFFFFFF)

#define QURT_PAGE_SIZE_4K 4096

/* Clears the lowest 12 bits on xx. */
#define MBA_FLOOR_4K(xx)      ((xx) & ~( QURT_PAGE_SIZE_4K-1))

#define TARGET_FAMILY_MASK (0xFFFF0000)
#define TARGET_FAMILY_8937 (0x200A)
#define TARGET_FAMILY_8917 (0x200C)

/* Making a TLB entry dynamically */
int mba_qurt_map_memory_range(uint32 mba_region_paddr, uint32 mba_region_size, 
  qurt_mem_cache_mode_t cache_attribs, unsigned int *entry, uint8 remap, uint32 *vadd_out_param);
							  
/* Make multiple entries */
int mba_map_memory_range(uint32 mba_region_paddr, uint32 mba_region_size, 
  qurt_mem_cache_mode_t cache_attribs, unsigned int *entry, uint8 remap, uint32 *vadd_out_param);

int mba_check_memory_in_bank0(uint32 start_addr, uint32 size);

int mba_get_soc_hw_version_target_family(uint32 *target_family);

#endif /* MBA_UTILS_H */
  