#ifndef MEMMGR_CONFIG
#define MEMMGR_CONFIG

/*============================================================================
  @file mem_cfg.h

  @brief
  This contains the configuration for the memory manager.

  <br><br>

  DEPENDENCIES: None.

  Copyright (c) 2010-2012, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/memmgr/inc/mem_cfg.h#1 $ 
  $DateTime: 2016/12/13 08:00:23 $

  when       who    what, where, why 
  ---------- --- -----------------------------------------------------------
  2015-03-26 ps  Updated license header
  2012-05-18 lka Added 2KB in heap
  2012-01-17 br  Increased by 1KB in a heap memory region
  2012-01-12 br  Fragmented heap memory region into two regions
  2011-07-01 sj  Increase heap memory by 2kb as SMGR allocs were failing  
  2011-06-11 sd  Increase Heap Memory for 2kb which is needed when power features are enabled
  2011-04-27 agk Included oi_string.h to remove a compiler warning.
  2011-04-27 br  increased heap memory size
  2011-02-28 jtl Added header.
  2011-05-05 rk  reduced the heap memory size from 25KB to 18KB to help fit into TCM

  ============================================================================*/

  
/*============================================================================

  INCLUDE FILES

  ============================================================================*/
#include "oi_stddefs.h"
#include "oi_support_init.h"
#include "oi_memmgr.h"
#include "oi_string.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*============================================================================
  Preprocessor Definitions and Constants
  ============================================================================*/

#ifndef USE_NATIVE_MALLOC

#define MEMMGR_HEAP_FRAG_CNT              2
#define MEMMGR_MAX_ALLOWED_POOL_CNT       10

#if defined(MEMMGR_DEBUG) || defined(OI_DEBUG)
    #define MEMMGR_TOTAL_HEAP_FRAG0_SIZE  ((10*1024)+(2*1024))
    #define MEMMGR_TOTAL_HEAP_FRAG1_SIZE  ((12*1024) + (2*1024))
#elif defined(SNS_MEMMGR_PROFILE_ON)
    #define MEMMGR_TOTAL_HEAP_FRAG0_SIZE  ((10*1024)+(3*1024))
    #define MEMMGR_TOTAL_HEAP_FRAG1_SIZE  ((12*1024) + (3*1024))
#else
    #define MEMMGR_TOTAL_HEAP_FRAG0_SIZE  (10*1024)
    #define MEMMGR_TOTAL_HEAP_FRAG1_SIZE  (14*1024)
#endif


/*============================================================================
 * Global Data Definitions
 ============================================================================*/

/**************************************************
  Memory Manager pool table

    This file defines settings for the Memory Manager heap and memory pool configuration:
        - Small platforms, typically embedded platforms with limited RAM

    These default settings may not be appropriate for a specific application. For those cases,
    the Memory Manager configuration can be defined somewhere else, in which case this file
    will not be linked in.

 **************************************************/

/** The heap is declared to be an array of elements of type OI_UINT32, with
 *  a size of @ref MEMMGR_TOTAL_HEAP_SIZE. OI_UINT32 elements ensure proper
 *  byte alignment on all platforms.
 */
extern OI_UINT32 OI_memmgrHeap_frag0[MEMMGR_TOTAL_HEAP_FRAG0_SIZE/sizeof(OI_UINT32)];
extern OI_UINT32 OI_memmgrHeap_frag1[MEMMGR_TOTAL_HEAP_FRAG1_SIZE/sizeof(OI_UINT32)];

/**
 *  All memory allocated by the Memory Manager comes from a single heap. This includes
 *  statically allocated memory and the memory pools. This structure configures the heap,
 *  setting a pointer to the heap and establishing the heap size. This structure is
 *  eventually sent to OI_MEMMGR_Init() by way of OI_CONFIG_MEMMGR.
 */
extern  const OI_MEMMGR_HEAP_CONFIG    heapConfig[];

/** This array is a table of pairs that indicate how the Memory Manager will partition
    the portion of the heap from which it dynamically allocates memory.
    A memory pool is a linked list of memory blocks of the same size. Each entry                                  
    in this table configures one memory pool: the first field indicates the number of blocks
    that will be allotted in a pool; the second field indicates the size of the blocks in this pool.
    For example, the entry {100, 64} indicates that a pool of 100 64-byte blocks will be set aside.

    Note:
    - The last entry must always be a pair of zeroes.
    - The block sizes must be multiples of 4.
    - The pool table must be ordered such that block sizes are in increasing order.
*/
extern const OI_MEMMGR_POOL_CONFIG memmgrPoolTable[];

/*  This single structure is used for passing all memory configuration
 *  information to OI_MEMMGR_Init().
 */
extern const OI_CONFIG_MEMMGR  oi_default_config_MEMMGR;

#endif /* USE_NATIVE_MALLOC */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
