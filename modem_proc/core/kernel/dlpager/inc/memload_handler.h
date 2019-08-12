#ifndef  MEMLOAD_HANDLER_H
#define  MEMLOAD_HANDLER_H
/*==========================================================================
 * FILE:         memload_handler.h
 *
 * SERVICES:     DL PAGER TOP LEVEL HEADER FILE
 *
 * DESCRIPTION:  Header file that defines pager data structure and other
 *               definitions for pager operation.
 *
 * Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
=============================================================================*/


/*===========================================================================

            EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/inc/memload_handler.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/10   op     Header file for demand paging
07/01/13   bc     Re-write the header file
===========================================================================*/
#include <qurt.h>

/** Data structre that serves as the basic node/data holder for loaded_pages list
 */

typedef struct _dlpager_node {
	struct _dlpager_node *next;
	struct _dlpager_node *prev;
	unsigned int vaddr;
} dlpager_node_t;

/** Data structre that contains all the information for paging operation of a particular swap pool
 */
struct swap_page_pool{
    unsigned int seg_addr_base; /**< Base address of the segment used for loading */
    unsigned int p_addr_base; /**< Base physical address of the swap pool */ 
    unsigned int v_rw_addr_base; /**< Base virtual address of the swap pool for RW operation */
    unsigned int v_rx_addr_base; /**< Base virtual address of the swap pool for RX operations */
    unsigned short page_shift; /**< x, where 2 power x is the page size */
    unsigned int max_swap_pages; /* Max. number of swappable pages */
    unsigned int max_glob_pages; /* Max. number of pages in the segment */
    unsigned int* dirty_bits;
    unsigned int dirty_bits_array_sz; /* number of words used as dirty_bits*/
    dlpager_node_t* swap_list_head;  /*list of swap pages*/
    dlpager_node_t* loaded_pages; /* Table of virt addr of the loaded pages and swap nodes */
    unsigned int *global_pages; /* Mapping table of the overall segment */
};

/** Type definition of swap page pool
 */
typedef struct swap_page_pool swap_page_pool_t;

extern int dlpager_osam_debug_enabled;

/** init_active_page_pool
  Initialize swap_page_pool data structure for pager operation. This data structure contains all the information needed for pager operation for the particular swap pool. Segment with base address seg_base and seg_size needs to be copied to the physical memory (address: p_addr, size: pool_size) one page at a time based on the memory access. The goal is to reduce memory with reasonable hit to performance.
 
  @param[in] pool_index  swap page pool index
             p_addr      base address of the physical memory assigned for paging of the swap pool.
	     v_rw_addr   base address of the virtual address corresponding to p_addr for read write maintenance, e.g dcache opearations, copy.
             v_rx_addr   base address of the virtual address corresponding to p_addr for I-cache maintenance operations.
             pool_size   size of the swap pool. This size is aligned to the pages size.
             page_size   page size of the swap pool.
             seg_base    base address of the segmented which is going to be accessed by applications and needs to be paged.
             seg_size    Segment size

  @param[out] None

  @return
  Registry status: \n
  None \n
 */
extern void dlpager_memload_init_active_page_pool (int pool_index, unsigned int p_addr,
                                                   unsigned int v_rw_addr, unsigned int v_rx_addr,
                                                   unsigned int pool_size, unsigned int page_size,
                                                   unsigned int seg_base, unsigned int seg_size);

extern int  dlpager_memload_fault_handler (unsigned int fault_addr, qurt_thread_t thread_id, unsigned char cause);
extern void dlpager_memload_postfault_handler (unsigned int fault_addr, qurt_thread_t thread_id, unsigned char cause);
extern void dlpager_memload_cache_update(int delta, int size);
extern void dlpager_memload_loaded_pages_update(int pool_index);
extern void dlpager_memload_evict(void);

extern void dump_global_table (void);

extern void dump_swap_table (void);

extern void dump_log_table (void);

#endif /* MEMLOAD_HANDLER_H */
