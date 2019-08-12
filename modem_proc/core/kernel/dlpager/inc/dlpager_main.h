#ifndef DLPAGER_MAIN_H
#define DLPAGER_MAIN_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               DL PAGER TOP LEVEL HEADER FILE

GENERAL DESCRIPTION

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

            EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/inc/dlpager_main.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/10   op     Header file for demand paging
===========================================================================*/

#include <stdio.h>
#include <qurt.h>
#include <stdlib.h>
#include "dlpager_constants.h"

#define DLPAGER_MAIN_STACK_SIZE   (4*1024) /**< Stack size of DL pager thread */
#define DLPAGER_MAIN_PRIO         103 /**< DL pager's QURT thread priority */
#define DLPAGER_MAX_SWAP_POOLS    1 /**< No. of swap pools allowed */

/* DL PAGER version */
#define DLPAGER_VERSION "02.01.09" 

/** Data structure of the DL pager attributes that can be configured.
 */
typedef struct dlpager_attr
{
    int num_swap_pools; /**< No. of swap pools, <= DLPAGER_MAX_SWAP_POOLS */
    struct {
        unsigned int size; /**< Total size of the physical pool */
        unsigned int page_size; /**< Page size of the pool */
    } swap_pool [DLPAGER_MAX_SWAP_POOLS]; /**< Array of swap pools */
} dlpager_attr_t ;

/*===========================================================================

             FUNCTION DEFINITIONS

===========================================================================*/

/** dlpager_init
    Initialization function to start pager thread.

    @param[out]  None

    @return
    Registry status: \n
    0 -- Success \n
    -1 -- Failure \n

    @dependencies
    __swapped_segments_start__ and __swapped_segments_end__ definitions must be available in the build. \n
*/
extern int dlpager_init (void);

/** dlpager_get_attr
    DL pager calls this function to get configuration information such as swap pool size, page size etc.

    @param[out]  pAttr   Pointer to the pager attributes required for pager operation.

    @return
    Registry status: \n
    0 -- Success \n
    -1 -- Failure \n
*/
extern int dlpager_get_attr (dlpager_attr_t *pAttr);

/** dlpager_load_virtual_page
    Pager calls this function to load a page. Pager allocates a page and provides the virtual address to copy the contents.

    @param[in]  src_addr    start address of the page which caused page fault. This page need to be loaed to physical memory.
    @param[in]  dest_addr   The contents of the faulted page needs to be copied to this address. This is the start address of the physical page allocated to load the page.
    @param[in]  page_size   size of the page. This is also the max. size of the buffer starting at "dest_addr".

    @param[out] None

    @return
    Registry status: \n
    None \n

*/


/** dlpager_store_virtual_page
    Pager calls this function to load a page. Pager allocates a page and provides the virtual address to copy the contents.

    @param[in]  src_addr    start address of the page in swap pool that needs to be stored away
    @param[in]  orig_fault_addr   The contents of the swap pool page needs to be copied to this address. This is the start address of the program page that caused the contents to be loaded to src_addr in swap pool.
    @param[in]  page_size   size of the page. This is also the max. size of the buffer starting at "dest_addr".

    @param[out] None

    @return
    Registry status: \n
    None \n

*/

extern void dlpager_store_virtual_page(unsigned int src_addr, unsigned int orig_fault_addr);

extern unsigned int dlpager_is_addr_in_rx_range(unsigned int addr);
extern unsigned int dlpager_is_addr_in_rw_range(unsigned int addr);

extern unsigned int dlpager_is_addr_in_rodata_range(unsigned int addr);
extern unsigned int dlpager_is_addr_in_text_range(unsigned int addr);
extern unsigned int dlpager_is_addr_in_rwdata_range(unsigned int addr);
extern unsigned int dlpager_is_addr_in_bss_range(unsigned int addr);
extern void dlpager_handle_brkpts_on_eviction(unsigned int addr,unsigned int page_size);
#endif /* DLPAGER_MAIN_H */


