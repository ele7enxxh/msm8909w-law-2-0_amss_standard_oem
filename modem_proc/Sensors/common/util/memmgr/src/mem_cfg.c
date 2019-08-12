
/*============================================================================
  @file mem_cfg.c

  @brief
  This contains the configuration for the memory manager.


  DEPENDENCIES: None.

  Copyright (c) 2010-2012, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
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
  2012-05-18 lka Added four 512B blocks in heap
  2012-01-16 br  Increased 16B memory pool in the heap memory 
  2012-01-12 br  Fragmented heap memory region into two regions 
  2011-09-02 sd  Recofigured the memory blocks to support multiple requests
  2011-07-26 br  Recofigured the memory blocks to support multiple requests
  2011-07-12 br  Shuffled the memory blocks to enable init to succeed
  2011-07-01 sj  Increase heap memory by 2kb as SMGR allocs were failing
  2011-06-11 sd  Increase Heap Memory for 2kb which is needed when power features are enabled
  2011-05-05 rk  Changed the max memory blocks for 128bytes to fit into 18KB
  2011-05-05 rk  reduced the heap memory size from 25KB to 18KB
  2011-04-27 br  increased heap memory size

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
OI_UINT32 OI_memmgrHeap_frag0[MEMMGR_TOTAL_HEAP_FRAG0_SIZE/sizeof(OI_UINT32)];

/**
 *  All memory allocated by the Memory Manager comes from a single heap. This includes
 *  statically allocated memory and the memory pools. This structure configures the heap,
 *  setting a pointer to the heap and establishing the heap size. This structure is
 *  eventually sent to OI_MEMMGR_Init() by way of OI_CONFIG_MEMMGR.
 */
const OI_MEMMGR_HEAP_CONFIG    heapConfig[] =
{
    {(void*)OI_memmgrHeap_frag0,      /**< pointer to the fraged heap */
    MEMMGR_TOTAL_HEAP_FRAG0_SIZE},    /**< size of the heap */
    {(void*)OI_memmgrHeap_frag1,      /**< pointer to the fraged heap */
    MEMMGR_TOTAL_HEAP_FRAG1_SIZE}     /**< size of the heap */
};

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
    - Also, the pool table must be ordered such that frag fields are in increasing order.
*/
#define NUM_16_BYTE_BLOCKS  80    /* 1KB + 256B */ 
#define NUM_24_BYTE_BLOCKS  32    /* 768B */
#define NUM_32_BYTE_BLOCKS  32    /* 1KB */
#define NUM_64_BYTE_BLOCKS  48    /* 3KB */
#define NUM_128_BYTE_BLOCKS 32    /* 4KB */
#define NUM_256_BYTE_BLOCKS 14    /* 3.5KB */
#define NUM_512_BYTE_BLOCKS 11    /* 5.5KB */
#define NUM_1024_BYTE_BLOCKS 5    /* 5KB */

const OI_MEMMGR_POOL_CONFIG memmgrPoolTable[] =
{
    {NUM_16_BYTE_BLOCKS,    16,   0 },
    {NUM_24_BYTE_BLOCKS,    24,   0 },
    {NUM_32_BYTE_BLOCKS,    32,   0 },
    {NUM_64_BYTE_BLOCKS,    64,   0 },
    {NUM_128_BYTE_BLOCKS,   128,  0 },
    {NUM_256_BYTE_BLOCKS,   256,  1 },
    {NUM_512_BYTE_BLOCKS,   512,  1 },
    {NUM_1024_BYTE_BLOCKS,  1024, 1 },
    {0,		0}
};

#ifdef SNS_DSPS_BUILD
// Compile time assertion for sufficient heap allocation for the required pools and overhead.
// "Incantation" below will produce a compiler error of the following form
// if MEMMGR_TOTAL_HEAP_SIZE is not sufficient :
// mem_cfg.c, line nn: Error:  #94: the size of an array must be greater than zero
C_ASSERT ( (MEMMGR_TOTAL_HEAP_FRAG0_SIZE) >= \
  ( ( NUM_16_BYTE_BLOCKS*16 +          \
      NUM_24_BYTE_BLOCKS*24 +          \
      NUM_32_BYTE_BLOCKS*32 +          \
      NUM_64_BYTE_BLOCKS*64 +          \
      NUM_1024_BYTE_BLOCKS*128) ) &&   \
  ( ( MEMMGR_TOTAL_HEAP_FRAG1_SIZE) >= \
    ( NUM_256_BYTE_BLOCKS*256 +        \
      NUM_512_BYTE_BLOCKS*512 +        \
      NUM_1024_BYTE_BLOCKS*1024) ) );
#endif

/*  This single structure is used for passing all memory configuration
 *  information to OI_MEMMGR_Init().
 */
const OI_CONFIG_MEMMGR  oi_default_config_MEMMGR =
{
    &memmgrPoolTable[0],
    (const OI_MEMMGR_HEAP_CONFIG*)&heapConfig[0],
};
#endif /* USE_NATIVE_MALLOC */
