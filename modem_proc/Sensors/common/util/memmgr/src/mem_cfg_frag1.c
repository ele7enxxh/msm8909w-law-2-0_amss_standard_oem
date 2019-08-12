/*============================================================================
  @file mem_cfg_frag1.c

  @brief
  This contains the configuration of the heap memory fragmenation id #1 for the memory manager.

  DEPENDENCIES: None.

  Copyright (c) 2012, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header:
  $DateTime: 2016/12/13 08:00:23 $

  when       who    what, where, why
  ---------- --- -----------------------------------------------------------
  2015-03-26 ps  Updated license header
  2012-01-12 br  Initial creation

  ============================================================================*/


/*============================================================================

  INCLUDE FILES

  ============================================================================*/
#ifndef USE_NATIVE_MALLOC
#include "mem_cfg.h"
#include "oi_assert.h"

/** The heap is declared to be an array of elements of type OI_UINT32, with
 *  a size of @ref MEMMGR_TOTAL_HEAP_SIZE. OI_UINT32 elements ensure proper
 *  byte alignment on all platforms.
 */
OI_UINT32 OI_memmgrHeap_frag1[MEMMGR_TOTAL_HEAP_FRAG1_SIZE/sizeof(OI_UINT32)];
#endif /* USE_NATIVE_MALLOC */
