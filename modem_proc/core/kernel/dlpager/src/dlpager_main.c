/*==========================================================================
 * FILE:         dlpager_main.c
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  This file provides the implementation of pager service
 *               initilaiztion and creation of pager thread. It servers as an
 *               interface to Kernel.
 *
 * Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
=============================================================================*/


/*===========================================================================

            EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/src/dlpager_main.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/13   bc      Re-write dlpager_posix_main.c
===========================================================================*/
#include <stdlib.h>
#include <stringl.h>
#include <assert.h>
#include <qurt.h>

#include "dlpager_main.h"
#include "memload_handler.h"
#include "dlpager_log.h"
#include "dlpager_aux.h"


#define PAGE_SIZE_4K     (4*1024)      /* 4K page, the smallest pages supported */
#define PAGE_SIZE_4M     (4*1024*1024) /* 4M page, the largest page supported */

/* dlpager Thread ID */
qurt_thread_t dlpager_tid;

/* dlpager attributes set externally */
dlpager_attr_t  dlpager_attr;

struct dlpager_mem_regions {
    unsigned int seg_base; /* Demand Loaded segment base address */
    unsigned int seg_size; /* segment size */
    qurt_mem_region_t region; /* QuRT region of swap pool */
    qurt_mem_region_t region_rx; /* QuRT region of allocated virtual memory */
    unsigned int region_size; /* size of the allocated swap pool */
    unsigned int p_base; /* swap pool physical base address */
    unsigned int v_rw_base; /* swap pool virtual address with RW permissions */
    unsigned int v_rx_base; /* swap pool virtual address with RX permission */
} dlpager_mem_regions[DLPAGER_MAX_SWAP_POOLS];

extern unsigned int __attribute__ ((weak)) __swapped_segments_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_end__;

unsigned int __attribute__((section (".data"))) start_va_uncompressed_text = 0;
unsigned int __attribute__((section (".data"))) end_va_uncompressed_text = 0;
unsigned int __attribute__((section (".data"))) start_va_compressed_text = 0;
unsigned int __attribute__((section (".data"))) end_va_compressed_text = 0;
unsigned int __attribute__((section (".data"))) start_va_uncompressed_rw = 0;
unsigned int __attribute__((section (".data"))) end_va_uncompressed_rw = 0;
unsigned int __attribute__((section (".data"))) start_va_compressed_rw = 0;
unsigned int __attribute__((section (".data"))) end_va_compressed_rw = 0;

/* DL PAGER Versioning*/
static volatile char dlpager_version[] = DLPAGER_VERSION;


/*
 * dlpager_main
 *
 * DL pager thread. It creates two regions to create a shadow mapping to the
 * swap pool (physical) for maintenance. One regions is RW to help loading the
 * page and the other regions is to help invalidate I-cache.
 */
void dlpager_main (void *pArg)
{
    int i, ret;
    qurt_mem_region_attr_t attr;
    qurt_mem_region_t region;
    qurt_sysevent_pagefault_t pf_data;
    qurt_mem_pool_t default_pool;

    qurt_mem_pool_attach ("DEFAULT_PHYSPOOL", &default_pool);

    for (i = 0; i < dlpager_attr.num_swap_pools; i++) {
        unsigned int pool_size = dlpager_attr.swap_pool[i].size;
        unsigned int page_size = dlpager_attr.swap_pool[i].page_size;
        unsigned int p_addr, v_rw_addr, v_rx_addr, r_size;
        unsigned int seg_base, seg_size;

        /* Segment (not loaded) base address and size */
        seg_base = DLPAGER_ALIGN_DOWN ((unsigned int)&__swapped_segments_start__, page_size);
        seg_size = DLPAGER_ALIGN_UP (((unsigned int)&__swapped_segments_end__ - seg_base), page_size);

        /* swap pool don't need to be bigger than the segment */
        if (pool_size > seg_size) {
            pool_size = seg_size;
        }

        /* Create memory region for swap pool */
        qurt_mem_region_attr_init (&attr);
        qurt_mem_region_attr_set_mapping (&attr, QURT_MEM_MAPPING_VIRTUAL_RANDOM);
        ret = qurt_mem_region_create (&region, pool_size, default_pool, &attr);
        assert (ret == QURT_EOK);
        qurt_mem_region_attr_get (region, &attr);

        qurt_mem_region_attr_get_physaddr (&attr, &p_addr);
        qurt_mem_region_attr_get_virtaddr (&attr, &v_rw_addr);
        qurt_mem_region_attr_get_size (&attr, &r_size);

        /* Initialize dlpager_mem_regions */
        dlpager_mem_regions[i].seg_base = seg_base;
        dlpager_mem_regions[i].seg_size = seg_size;
        dlpager_mem_regions[i].region = region;
        dlpager_mem_regions[i].region_size = r_size;
        dlpager_mem_regions[i].p_base = p_addr;
        dlpager_mem_regions[i].v_rw_base = v_rw_addr;

        /* Reserve virtual memory for RX operations */
        qurt_mem_region_attr_init (&attr);
        qurt_mem_region_attr_set_mapping (&attr, QURT_MEM_MAPPING_NONE);


        /* Map vitual memory for RX operations */
        ret = qurt_mem_region_create (&region, r_size, default_pool, &attr);
        assert (ret == QURT_EOK);
        qurt_mem_region_attr_get (region, &attr);
        qurt_mem_region_attr_get_virtaddr (&attr, &v_rx_addr);
        dlpager_mem_regions[i].v_rx_base = v_rx_addr;
        qurt_mapping_create (v_rx_addr, p_addr, r_size,
                             QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ | QURT_PERM_EXECUTE);

        /* Initialize swap pool for paging */
        dlpager_memload_init_active_page_pool (i, p_addr, v_rw_addr, v_rx_addr, r_size,
                                               page_size, seg_base, seg_size);

        /* Removing the RW mappings as the swap pools have been configured */
        qurt_mapping_remove(v_rw_addr, p_addr,r_size );
    }

    while (1) {
        unsigned int ssr_cause = 0;
        pf_data.thread_id = 0;
        pf_data.fault_addr = 0;

        ret = qurt_exception_wait_pagefault (&pf_data);

        assert(ret == QURT_EOK &&
               (unsigned)&__swapped_segments_start__ <= pf_data.fault_addr &&
               pf_data.fault_addr < (unsigned)&__swapped_segments_end__);

#ifdef DLPAGER_QURT_SUPPORTS_SSR
        ssr_cause = pf_data.ssr_cause;
#else
        ssr_cause = CAUSE_ANY_TLB_MISS;
#endif

        DL_DEBUG("receive page fault info: threadID=%x, faulting address=%x, cause=%x\n",
                 pf_data.thread_id, pf_data.fault_addr, ssr_cause);

        dlpager_memload_fault_handler(pf_data.fault_addr, pf_data.thread_id, ssr_cause);

        /* resume the faultiing thread */
        ret = qurt_thread_resume(pf_data.thread_id);
        assert(ret == QURT_EOK);

        dlpager_memload_postfault_handler(pf_data.fault_addr, pf_data.thread_id, ssr_cause);
    }
}

/*
 * dlpager_init
 *
 * Initialization function to start pager thread. Input parameters decides
 * the configuration of swap pool size, page size etc.
 */
int dlpager_init (void)
{
    qurt_thread_attr_t thread_attr;
    int i, rc, status;
    void *pStack;
    unsigned int page_size;

    /* No need to start pager, if swap segments are not available */
    if (&__swapped_segments_start__ == &__swapped_segments_end__) {
        return 0;
    }

    i = strlen( DLPAGER_VERSION );
    memscpy( (void*)&dlpager_version, i, DLPAGER_VERSION, i );
	
    dlpager_log_init();
    dlpager_aux_init();

    rc = dlpager_get_attr (&dlpager_attr);

    assert (rc == 0);

    assert (dlpager_attr.num_swap_pools <= DLPAGER_MAX_SWAP_POOLS);

    /* If there are no swap pools, there is no point in starting DL Pager */
    if (dlpager_attr.num_swap_pools == 0) {
        DL_DEBUG("No need to start DL Pager\n");
        return -1;
    }

    for (i = 0; i < dlpager_attr.num_swap_pools; i++) {
        for (page_size = PAGE_SIZE_4K; page_size < PAGE_SIZE_4M; page_size *= 4) {
            if (dlpager_attr.swap_pool[i].page_size <= page_size) {
                break;
            }
        }
        if (dlpager_attr.swap_pool[i].page_size != page_size) {
            /* Not a valid page size of Hexagon processor */
            return -1;
        }
        dlpager_attr.swap_pool[i].size = DLPAGER_ALIGN_UP (dlpager_attr.swap_pool[i].size, page_size);
        DL_DEBUG("swap pool %d size = 0x%0x, page size = 0x%x\n", i, dlpager_attr.swap_pool[i].size, dlpager_attr.swap_pool[i].page_size);

    }

    /* Demand paging Thread */
    qurt_thread_attr_init (&thread_attr);
    pStack = malloc (DLPAGER_MAIN_STACK_SIZE);
    if (pStack != NULL)
    {
        qurt_thread_attr_set_name (&thread_attr, "DLPager_main");
        qurt_thread_attr_set_stack_addr (&thread_attr, pStack);
        qurt_thread_attr_set_stack_size (&thread_attr, DLPAGER_MAIN_STACK_SIZE);
        qurt_thread_attr_set_priority (&thread_attr, DLPAGER_MAIN_PRIO);
        status = qurt_thread_create (&dlpager_tid, &thread_attr, dlpager_main, (void *)0);
        assert(status == 0);
        DL_DEBUG(" Starting DL main Thread , id=%d \n", dlpager_tid);
    }
    else
    {
       assert(0);
    }

    return 0;
}
