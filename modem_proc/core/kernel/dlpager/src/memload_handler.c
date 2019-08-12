/*==========================================================================
 * FILE:         memload_handler.c
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  This file provides the implementation of pager replacement
 *               algorithm. Based on the fault address, missed page is
 *               identified and loaded to a swap page. swap page is identified
 *               using FIFO algorithm.
 *
 * Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
=============================================================================*/


/*===========================================================================

            EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/src/memload_handler.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/10   op     Header file for demand paging
07/01/13   bc     Re-write the header file
===========================================================================*/

#include <stdlib.h>
#include <assert.h>
#include <qurt.h>
#include <qurt_cycles.h>
#include <qurt_tlb.h>
#include <hexagon_protos.h>

#include "dlpager_main.h"
#include "memload_handler.h"
#include "dlpager_log.h"
#include "q6zip_clk.h"
#include "dlpager_cache.h"
#include "dlpager_q6zip.h"
#include "dlpager_aux.h"


#define DLPAGER_INVALID_VICTIM_IDX -1

/* swap pools */
struct swap_page_pool *swap_pools[DLPAGER_MAX_SWAP_POOLS];

typedef union {
 struct {
    unsigned S:1;
    unsigned PA:23;
    unsigned C:4;
    unsigned U:1;
    unsigned R:1;
    unsigned W:1;
    unsigned X:1;
    unsigned VA:20;
    unsigned ASID:7;
    unsigned A0:1;
    unsigned A1:1;
    unsigned EP:1;
    unsigned G:1;
    unsigned V:1;
 } s;
 unsigned long long v;
} tlb_entry_t;

unsigned dlpager_last_victim_idx         = DLPAGER_INVALID_VICTIM_IDX;
unsigned dlpager_last_page_removed;
unsigned dlpager_rw_mapping_id;

tlb_entry_t dlpager_rw_mapping_entry;

/* bit manipulation for the dirty_bits array */
#define set_bit(D, n)    ((D[(n/32)]) |= (1 << (n%32)))
#define clear_bit(D, n)  ((D[(n/32)]) &= ~(1 << (n%32)))
#define test_bit(D, n)   ((D[(n/32)]) & (1 << (n%32)))

static qurt_mutex_t dlpager_memload_plru_mutex;

void dlpager_remove_node(dlpager_node_t **ring, dlpager_node_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (*ring == node) {
        *ring = node->next;
    }
}

void dlpager_insert_head(dlpager_node_t **ring, dlpager_node_t *node)
{
    node->next = *ring;
    node->prev = (*ring)->prev;
    node->prev->next = node;
    node->next->prev = node;

    *ring = node;
}

void set_dirty_bit(int pool_index, unsigned int phys_addr)
{
    struct swap_page_pool *pool;
    unsigned int phys_page_idx;

    pool = swap_pools[pool_index];
    phys_page_idx = (phys_addr - pool->p_addr_base) >> pool->page_shift;
    set_bit(pool->dirty_bits, phys_page_idx);
}


void clear_dirty_bit(int pool_index, unsigned int phys_addr)
{
    struct swap_page_pool *pool;
    unsigned int phys_page_idx;

    pool = swap_pools[pool_index];
    phys_page_idx = (phys_addr - pool->p_addr_base) >> pool->page_shift;
    clear_bit(pool->dirty_bits, phys_page_idx);
}

/*
 * init_active_page_pool
 *
 * Initialize swap_page_pool data structure for pager operation. This data
 * structure contains all the information needed for pager operation for
 * the particular swap pool.
 *
 */
void dlpager_memload_init_active_page_pool (int pool_index, unsigned int p_addr,
                                            unsigned int v_rw_addr, unsigned int v_rx_addr,
                                            unsigned int pool_size, unsigned int page_size,
                                            unsigned int seg_base, unsigned int seg_size)
{
    unsigned int num_swap_pages, num_seg_pages;
    unsigned int shift, i;
    struct swap_page_pool *pool;
    unsigned int pool_mem_size;
    unsigned int dirty_bit_array_size;
    int ret_val;

    shift = (unsigned short)Q6_R_ct0_R (page_size);
    num_swap_pages = pool_size >> shift;
    num_seg_pages = seg_size >> shift;


    /* Each unsigned int can hold 32 dirty bits.
     * Computer number of unsigned ints required to hold
     * dirty bits for num_swap_pages, rounded to the next digit */
#define ROUNDIMG_NEXT_INT (31)
#define SHIFT_FOR_DIV_BY_32 (5)
    dirty_bit_array_size = (num_swap_pages + ROUNDIMG_NEXT_INT) >> SHIFT_FOR_DIV_BY_32;
#undef ROUNDIMG_NEXT_INT
#undef SHIFT_FOR_DIV_BY_32

    /* Allocate data structure for swap pool managemenet */
    pool_mem_size = sizeof( struct swap_page_pool) +
	            ( sizeof(unsigned int) * dirty_bit_array_size ) +
                    ( sizeof(unsigned int) * num_seg_pages ) +
                    ( sizeof(dlpager_node_t) * num_swap_pages);

    pool = (struct swap_page_pool *)malloc( pool_mem_size );
    if ( pool != NULL ) 
    {
       memset( pool, 0, pool_mem_size );

       swap_pools[pool_index] = pool;

       pool->seg_addr_base = seg_base;
       pool->p_addr_base = p_addr;
       pool->v_rw_addr_base = v_rw_addr;
       pool->v_rx_addr_base = v_rx_addr;
       pool->page_shift = shift;
       pool->max_swap_pages = num_swap_pages;
       pool->max_glob_pages = num_seg_pages;
       pool->dirty_bits_array_sz = dirty_bit_array_size;
       pool->dirty_bits = (unsigned int *)(pool + 1);
       pool->loaded_pages = (dlpager_node_t*)&(pool->dirty_bits[dirty_bit_array_size]) ;
       pool->global_pages = (unsigned int*)&(pool->loaded_pages[num_swap_pages]);

       /*
        * Initialize loadable pages to reflect no mapping.
        * This is a doubly linked list.  First and last nodes in
        * array need to be set to point to each other, respectively.
        * They are handled outside the for loop to avoid memory
        * corruption.
        */
       assert( num_swap_pages > 1 );
       pool->loaded_pages[0].next = &pool->loaded_pages[1];
       pool->loaded_pages[0].prev = &pool->loaded_pages[num_swap_pages-1];
       for( i = 1; i < (num_swap_pages - 1); i++ )
       {
           pool->loaded_pages[i].next = &pool->loaded_pages[i+1];
           pool->loaded_pages[i].prev = &pool->loaded_pages[i-1];		
       }
       pool->loaded_pages[num_swap_pages-1].next = &pool->loaded_pages[0];
       pool->loaded_pages[num_swap_pages-1].prev = &pool->loaded_pages[num_swap_pages-2];
    
       pool->swap_list_head = &pool->loaded_pages[0];

       qurt_mem_cache_clean ((qurt_addr_t)v_rx_addr, pool_size,
                          QURT_MEM_CACHE_INVALIDATE, QURT_MEM_ICACHE);
       qurt_mem_cache_clean ((qurt_addr_t)v_rw_addr, pool_size,
                          QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE);

       qurt_pimutex_init(&dlpager_memload_plru_mutex);

       ret_val = qurt_tlb_entry_create(&dlpager_rw_mapping_id, pool->v_rw_addr_base, pool->p_addr_base, PAGE_SIZE,
                                    QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ | QURT_PERM_WRITE, 0);
       assert( QURT_EOK == ret_val );
    
       ret_val = qurt_tlb_entry_get(dlpager_rw_mapping_id, &dlpager_rw_mapping_entry.v);
       assert( QURT_EOK == ret_val );

       dlpager_rw_mapping_entry.s.V = 0;
       ret_val = qurt_tlb_entry_set(dlpager_rw_mapping_id, dlpager_rw_mapping_entry.v);
       assert( QURT_EOK == ret_val );
 
    }
    else
    {
      assert(0);
    }
}

void dlpager_memload_loaded_pages_update(int pool_index)
{
    struct swap_page_pool *pool;
    unsigned int i,index;
    int num_active_pages;
    dlpager_node_t* update_node;
    unsigned int* dlpager_tlb_phys_addr;

    unsigned log_idx = dlpager_log_start(DLP_PLRU, 0, 0, 0);

    pool = swap_pools[pool_index];

    /*Request for Phys addrs of the Paged segments curretently in TLB*/
	num_active_pages = qurt_tlb_get_pager_physaddr(&dlpager_tlb_phys_addr);

    DL_DEBUG("the no of TLBentries:%d\n",num_active_pages);
    if(num_active_pages > 0){
        qurt_pimutex_lock(&dlpager_memload_plru_mutex);
        for (i = 0; i < num_active_pages; i++)
        {
            DL_DEBUG("phys addr in TLB:0x%x %d\n", (unsigned)dlpager_tlb_phys_addr[i], i);
            index = (dlpager_tlb_phys_addr[i] - pool->p_addr_base) >> pool->page_shift;

            /*Get the node which needs to be touched*/
            update_node = &pool->loaded_pages[index];
            dlpager_remove_node(&(pool->swap_list_head), update_node);
            dlpager_insert_head(&(pool->swap_list_head), update_node);
        }
        qurt_pimutex_unlock(&dlpager_memload_plru_mutex);
    }
    dlpager_log_end(DLP_PLRU, log_idx, 0, 0);
}

/*
 * insert_active_page
 *
 * Create mapping for the fault page and make it active. Set the next_victim
 * index appropriately.
 *
 * param[in]  pool_index   - swap pool index of the fault address
 *            fault_addr   - base address of the fault page
 *            phys_addr    - phys address where are the fault page is placed
 */
static int insert_active_page (int pool_index, unsigned int fault_addr,
                               unsigned int phys_addr, unsigned cause)
{
    struct swap_page_pool *pool = swap_pools[pool_index];
    unsigned short shift;
    unsigned short load_idx, global_idx;
    int rc = QURT_EFATAL;
    dlpager_node_t*  victim_node;

    clear_dirty_bit(pool_index, phys_addr);

    shift = pool->page_shift;
    /*Victim is always at the tail of the list
     * we removed the mapping of the tail node earlier */
    victim_node =  pool->loaded_pages + dlpager_last_victim_idx;
    load_idx = dlpager_last_victim_idx;

    DL_DEBUG("victim_node:0x%x, v_addr_mapped:0x%x  load_idx %d\n",(unsigned)victim_node,(unsigned)victim_node->vaddr, load_idx);
    assert (phys_addr == (pool->p_addr_base + (load_idx << shift)));

    if (dlpager_is_addr_in_rodata_range(fault_addr))
    {
        rc = qurt_mapping_create(fault_addr,
                                 phys_addr,
                                 1 << shift,
                                 QURT_MEM_CACHE_WRITEBACK,
                                 QURT_PERM_READ);
    }
    else if (dlpager_is_addr_in_text_range(fault_addr))
    {
        rc = qurt_mapping_create(fault_addr,
                                 phys_addr,
                                 1 << shift,
                                 QURT_MEM_CACHE_WRITEBACK,
                                 QURT_PERM_READ | QURT_PERM_EXECUTE);
    }
    else if (dlpager_is_addr_in_rw_range(fault_addr))
    {
#ifdef DLPAGER_QURT_SUPPORTS_SSR
#ifdef DLPAGER_WRITE_DETECT_ENABLED
        if( CAUSE_MISS_READ == cause )
        {
            rc = qurt_mapping_create(fault_addr,
                                     phys_addr,
                                     1 << shift,
                                     QURT_MEM_CACHE_WRITEBACK,
                                     QURT_PERM_READ);
        }
        else
#endif
#endif
        {
            rc = qurt_mapping_create(fault_addr,
                                     phys_addr,
                                     1 << shift,
                                     QURT_MEM_CACHE_WRITEBACK,
                                     QURT_PERM_READ | QURT_PERM_WRITE);
            set_dirty_bit(pool_index, phys_addr);
        }
    }

    DL_DEBUG ("Inserting Page: vaddr=0x%x paddr=0x%x index=%d\n", fault_addr, phys_addr, load_idx);

    /*Always insert at the head of the list*/
    qurt_pimutex_lock(&dlpager_memload_plru_mutex);
    dlpager_remove_node(&(pool->swap_list_head), victim_node);
    dlpager_insert_head(&(pool->swap_list_head), victim_node);
    pool->loaded_pages[load_idx].vaddr = fault_addr;
    qurt_pimutex_unlock(&dlpager_memload_plru_mutex);

    /* Add mapping to the global table */
    global_idx = (fault_addr - pool->seg_addr_base) >> shift;
    pool->global_pages[global_idx] = phys_addr;

    return rc;
}


/* This function removes a page entry from the active list only if the list is currently full
   Returns:  non-zero if list was full and entry was removed
   zero if list is not full */
/*
 * remove_active_page
 *
 * Identify the a victim page that can be used to map the fault page. To start
 * with none of the physical pages (swap pages) are mapped and they can be
 * used straight away. Once all the available swap pages are mapped we remove
 * existing mappngs using FIFO algorithm.
 * If an existing mapping is removed, we need to perform cache operations for
 * correct behavior.
 *
 * param[in]  pool_index   - swap pool index of the fault address
 *
 * param[out] virt_addr    - virtual address of the evicted page that can be
 *                           used to copy the fault page.
 *            phys_addr    - physical address of the evicted page
 *
 * return     0       - if no page from the segment is activley mapped.
 *            address - virtual addres  of the active page being evicted. This
 *            virtual address corresponds to the segment.
 */
static unsigned int remove_active_page (int pool_idx, unsigned int *virt_addr,
                                        unsigned int *phys_addr)
{
    struct swap_page_pool *pool = swap_pools[pool_idx];
    unsigned int victim_idx, global_idx, shift;
	int ret;
    unsigned int v_rw_addr, v_rx_addr, p_addr, v_addr_mapped;


    shift = pool->page_shift;
    victim_idx = pool->swap_list_head->prev - pool->loaded_pages;
    assert( victim_idx < pool->max_swap_pages );

    DL_DEBUG("Tail_node:0x%x, swap_head:0x%x,  victim_idx:0x%x\n",(unsigned)pool->swap_list_head->prev,
	          (unsigned)pool->swap_list_head, victim_idx);

    v_rw_addr = pool->v_rw_addr_base + (victim_idx << shift);
    v_rx_addr = pool->v_rx_addr_base + (victim_idx << shift);
    p_addr = pool->p_addr_base + (victim_idx << shift);
    v_addr_mapped = pool->loaded_pages[victim_idx].vaddr;
    pool->loaded_pages[victim_idx].vaddr = 0;

    *virt_addr = v_rw_addr;
    *phys_addr = p_addr;

    /* Check if we need to evict the page */
    if (v_addr_mapped) {
        unsigned log_idx = dlpager_log_start(DLP_EVICT, 0, 0, 0);
        dlpager_last_page_removed = v_addr_mapped;
        DL_DEBUG ("Removing Page: vaddr=0x%x paddr=0x%x index=%d\n", v_addr_mapped, p_addr, victim_idx);
        ret = qurt_mapping_remove (v_addr_mapped, p_addr, 1 << shift);
        assert( ret == QURT_EOK );

        /* Invalidate I-cache. We don't need to invalidate D-cache because
         * we will write to the page before we map and use */
        if (dlpager_is_addr_in_text_range(v_addr_mapped))
        {
            dlpager_icache_clean_128(v_rx_addr, PAGE_SIZE / 128);
        }

        /* Remove mapping from the global table */
        global_idx = (v_addr_mapped - pool->seg_addr_base) >> shift;
        pool->global_pages[global_idx] = 0;

        dlpager_log_end(DLP_EVICT, log_idx, 0, 0);
    }

    return v_addr_mapped;
}

static void dlpager_memload_write_detect_handler(unsigned fault_addr, qurt_thread_t thread_id, unsigned cause)
{
    unsigned int pool_index = 0;
    struct swap_page_pool *pool;
    unsigned int phys_addr;
    unsigned int page_idx;
    unsigned int fault_addr_page_align;
    unsigned int shift;
    int ret_val;

    unsigned log_idx = dlpager_log_start(DLP_WRITE_DETECT, (void*)fault_addr, thread_id, cause);
    pool = swap_pools[pool_index];
    shift = pool->page_shift;
    page_idx = (fault_addr - pool->seg_addr_base) >> shift;

    phys_addr = pool->global_pages[page_idx];

    if (phys_addr == 0)
    {
        DL_DEBUG ("page has been swapped out...\n");
        dlpager_log_end(DLP_WRITE_DETECT, log_idx, 0, 0);
        return;
    }

    fault_addr_page_align = DLPAGER_ALIGN_DOWN (fault_addr, (1 << shift));
    set_dirty_bit(pool_index, phys_addr);
    ret_val = qurt_mapping_remove (fault_addr_page_align, phys_addr, 1 << shift);
    assert( QURT_EOK == ret_val );
    ret_val = qurt_mapping_create (fault_addr_page_align, phys_addr, 1 << shift,
                                   QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ | QURT_PERM_WRITE);
    assert( QURT_EOK == ret_val );

    dlpager_log_end(DLP_WRITE_DETECT, log_idx, 0, 0);
    return;
}

static void dlpager_memload_secondary_handler(unsigned fault_addr, qurt_thread_t thread_id, unsigned cause)
{
    struct swap_page_pool *pool = swap_pools[0];
    unsigned page_idx = (fault_addr - pool->seg_addr_base) >> DLPAGER_PAGE_NUM_SHIFT;
    unsigned clk = q6zip_clk_query();
    unsigned fault_addr_align = DLPAGER_ALIGN_DOWN (fault_addr, PAGE_SIZE);
    unsigned log_idx = dlpager_log_start(DLP_FAULT_TEXT_SECONDARY, (void*)fault_addr, thread_id, cause);
	  int rc = QURT_EFATAL;

    if (pool->global_pages[page_idx] == 0) {
        dlpager_last_victim_idx = DLPAGER_INVALID_VICTIM_IDX;
        dlpager_log_end(DLP_FAULT_TEXT_SECONDARY, log_idx, -1, -1);
        return;
    }

    int victim_idx = (pool->global_pages[page_idx] - pool->p_addr_base) >> DLPAGER_PAGE_NUM_SHIFT;
    unsigned phys_addr = pool->p_addr_base + (victim_idx << DLPAGER_PAGE_NUM_SHIFT);
    unsigned v_rx_addr = pool->v_rx_addr_base + (victim_idx << DLPAGER_PAGE_NUM_SHIFT);
    unsigned load_addr = pool->v_rw_addr_base + (victim_idx << DLPAGER_PAGE_NUM_SHIFT);

    qurt_mapping_remove (fault_addr_align, phys_addr, PAGE_SIZE);

    dlpager_rw_mapping_entry.s.VA = load_addr >> DLPAGER_PAGE_NUM_SHIFT;
    dlpager_rw_mapping_entry.s.PA = phys_addr >> DLPAGER_PAGE_NUM_SHIFT;
    dlpager_rw_mapping_entry.s.V = 1;
    rc = qurt_tlb_entry_set(dlpager_rw_mapping_id, dlpager_rw_mapping_entry.v);
	  assert(rc == QURT_EOK);

    dlpager_last_victim_idx = victim_idx;

    unsigned log_idx2 = dlpager_log_start(DLP_LOAD_TEXT_SECONDARY, (void*)fault_addr, thread_id, cause);
    dlpager_load_secondary_page(fault_addr, load_addr);
    dlpager_log_end(DLP_LOAD_TEXT_SECONDARY, log_idx2, 0, clk);

    qurt_mapping_create (fault_addr_align, phys_addr, PAGE_SIZE,
                         QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ | QURT_PERM_EXECUTE);

    dlpager_rw_mapping_entry.s.V = 0;

    rc = qurt_tlb_entry_set(dlpager_rw_mapping_id, dlpager_rw_mapping_entry.v);
    assert(rc == QURT_EOK);


    dlpager_log_end(DLP_FAULT_TEXT_SECONDARY, log_idx, 0, clk);
}

static void dlpager_memload_primary_handler(unsigned fault_addr, qurt_thread_t thread_id, unsigned cause)
{
    struct swap_page_pool *pool = swap_pools[0];
    unsigned page_idx = (fault_addr - pool->seg_addr_base) >> DLPAGER_PAGE_NUM_SHIFT;
    unsigned clk = q6zip_clk_query();
    unsigned fault_addr_align = DLPAGER_ALIGN_DOWN (fault_addr, PAGE_SIZE);
    unsigned load_addr, victim_paddr;

    int event = -1;
    int event_load = -1;
    if      (dlpager_is_addr_in_rodata_range(fault_addr_align)) {
        event = DLP_FAULT_RODATA;
        event_load = DLP_LOAD_RODATA;
    } else if (dlpager_is_addr_in_text_range(fault_addr_align)) {
        event = DLP_FAULT_TEXT_PRIMARY;
        event_load = DLP_LOAD_TEXT_PRIMARY;
    } else if (dlpager_is_addr_in_rwdata_range(fault_addr_align)) {
        event = DLP_FAULT_RWDATA;
        event_load = DLP_LOAD_RWDATA;
    } else if (dlpager_is_addr_in_bss_range(fault_addr_align)) {
        event = DLP_FAULT_BSS;
        event_load = DLP_LOAD_BSS;
    }

    if (pool->global_pages[page_idx] != 0) {
        DL_DEBUG ("duplicate page fault... mapping already exists \n");
        unsigned log_idx3 = dlpager_log_start(DLP_FAULT_DUPLICATED, (void*)fault_addr, thread_id, cause);
        dlpager_last_victim_idx = DLPAGER_INVALID_VICTIM_IDX;
        dlpager_log_end(event, log_idx3, 0, clk);
        return;
    }

    unsigned log_idx = dlpager_log_start(event, (void*)fault_addr, thread_id, cause);
    unsigned evicted_addr = remove_active_page (0, &load_addr, &victim_paddr);
    unsigned evicted_page = (evicted_addr - pool->seg_addr_base) >> DLPAGER_PAGE_NUM_SHIFT;
	int rc = QURT_EFATAL;

    dlpager_rw_mapping_entry.s.VA = load_addr >> DLPAGER_PAGE_NUM_SHIFT;
    dlpager_rw_mapping_entry.s.PA = victim_paddr >> DLPAGER_PAGE_NUM_SHIFT;
    dlpager_rw_mapping_entry.s.V = 1;
    rc = qurt_tlb_entry_set(dlpager_rw_mapping_id, dlpager_rw_mapping_entry.v);
    assert(rc == QURT_EOK);

    dlpager_last_victim_idx = (victim_paddr - pool->p_addr_base) >> DLPAGER_PAGE_NUM_SHIFT;

    unsigned paddr_idx = (victim_paddr - pool->p_addr_base) >> DLPAGER_PAGE_NUM_SHIFT;
    if ((evicted_addr != 0)
        && (dlpager_is_addr_in_rw_range(evicted_addr))
        && (test_bit(pool->dirty_bits, paddr_idx) != 0))
    {
        unsigned log_idx2 = dlpager_log_start(DLP_STORE_RW, (void*)fault_addr, thread_id, cause);
        dlpager_store_virtual_page(load_addr, evicted_addr);
        dlpager_log_end(DLP_STORE_RW, log_idx2, evicted_page, clk);
    }
    unsigned log_idx2 = dlpager_log_start(event_load, (void*)fault_addr, thread_id, cause);
    dlpager_load_primary_page(fault_addr, load_addr);
    dlpager_log_end(event_load, log_idx2, evicted_page, clk);

    qurt_mem_cache_clean((qurt_addr_t)load_addr, PAGE_SIZE,
                         QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_DCACHE);

    dlpager_rw_mapping_entry.s.V = 0;
    rc = qurt_tlb_entry_set(dlpager_rw_mapping_id, dlpager_rw_mapping_entry.v);
    assert( rc == QURT_EOK);

    insert_active_page(0, fault_addr_align, victim_paddr, cause);

    dlpager_log_end(event, log_idx, evicted_page, clk);
}

/*
 * memload_fault_handler
 *
 * This function activates the fault page. If all physical pages are mapped, a
 * page is evicted.
 *
 * param[in]  faul_addr - Address that caused page fault
 *            thread_id - Thread ID that accessed the memory
 *            cause
 *
 * return - 0 for SUCCESS
 */
int dlpager_memload_fault_handler (unsigned fault_addr, qurt_thread_t thread_id, unsigned char cause)
{
    if (cause == CAUSE_PRIVILEGE_EXCEPTION)
    {
        dlpager_memload_write_detect_handler(fault_addr, thread_id, cause);
    }
    else if (cause == CAUSE_ILLEGAL_INSTRUCTION)
    {
        dlpager_memload_secondary_handler(fault_addr, thread_id, cause);
    }
    else if (cause == CAUSE_MISS_EXECUTE ||
             cause == CAUSE_MISS_EXECUTE_BORDER ||
             cause == CAUSE_MISS_READ ||
             cause == CAUSE_MISS_WRITE ||
             cause == CAUSE_ANY_TLB_MISS)
    {
        dlpager_memload_primary_handler(fault_addr, thread_id, cause);
    }
    else
    {
        assert(0);
    }
    return 0;
}

void dlpager_memload_cache_update(int delta, int size)
{
    struct swap_page_pool *pool = swap_pools[0];
    unsigned v_rw_addr = pool->v_rw_addr_base + (dlpager_last_victim_idx << DLPAGER_PAGE_NUM_SHIFT);
    unsigned v_rx_addr = pool->v_rx_addr_base + (dlpager_last_victim_idx << DLPAGER_PAGE_NUM_SHIFT);
    qurt_mem_cache_clean ((qurt_addr_t)(v_rx_addr + delta), size,
                          QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_ICACHE);
    qurt_mem_cache_clean ((qurt_addr_t)(v_rw_addr + delta), size,
                          QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_DCACHE);
}

void dlpager_memload_postfault_handler (unsigned int fault_addr, qurt_thread_t thread_id, unsigned char cause)
{
    struct swap_page_pool *pool = swap_pools[0];
    uintptr_t victim_paddr;
    unsigned int load_addr;

    if (cause == CAUSE_ILLEGAL_INSTRUCTION || cause == CAUSE_PRIVILEGE_EXCEPTION)
        return;

    unsigned log_idx = dlpager_log_start(DLP_POST_FAULT, (void*)fault_addr, thread_id, cause);

    unsigned evicted_addr = remove_active_page(0, &load_addr, &victim_paddr);
    unsigned evicted_page = (evicted_addr - pool->seg_addr_base) >> DLPAGER_PAGE_NUM_SHIFT;

    if ((evicted_addr != 0) && dlpager_is_addr_in_rw_range(evicted_addr))
    {
        int victim_idx = (load_addr - pool->v_rw_addr_base) >> DLPAGER_PAGE_NUM_SHIFT;
        unsigned store_addr = pool->v_rx_addr_base + (victim_idx << DLPAGER_PAGE_NUM_SHIFT);
        unsigned log_idx2 = dlpager_log_start(DLP_STORE_RW, (void*)evicted_addr, thread_id, cause);
        dlpager_store_virtual_page(store_addr, evicted_addr);
        dlpager_log_end(DLP_STORE_RW, log_idx2, evicted_page, 0);
    }
    /* Signals the aux task to do PLRU. */
    dlpager_aux_check_plru();
    dlpager_log_end(DLP_POST_FAULT, log_idx, evicted_page, 0);
}

/*
 * dump_global_table
 *
 * Dump the page mapping of the entire segment of each swap pool
 */
void dump_global_table (void)
{
    int i, j;
    unsigned int seg_addr;
    struct swap_page_pool *pool;

    for (i = 0; i < DLPAGER_MAX_SWAP_POOLS; i++) {
        pool = swap_pools[i];
        printf ("seg page    --- swap page\n");
        for (j = 0; j < pool->max_glob_pages; j++) {
            seg_addr = pool->seg_addr_base + (j << pool->page_shift);
            printf ("0x%x  ---  ", seg_addr);
            if (pool->global_pages[j]) {
                printf ("0x%x\n", pool->global_pages[j]);
            }
            else {
                printf ("------\n");
            }
        }
    }
}

/*
 * dump_swap_table
 *
 * Dump the page mapping of the swap pool. There should be one-to-one
 * correspondence between swap table and global table.
 */
void dump_swap_table (void)
{
    int i, j;
    unsigned int phy_addr;
    char empty_string[] = "             ";
    struct swap_page_pool *pool;

    for (i = 0; i < DLPAGER_MAX_SWAP_POOLS; i++) {
        pool = swap_pools[i];
        printf ("swap pool: %d swap page   --- seg page\n", i);
        for (j = 0; j < pool->max_swap_pages; j++) {
            phy_addr = pool->p_addr_base + (j << pool->page_shift);
            printf ("%s0x%x  ---  ", empty_string, phy_addr);
            if (pool->loaded_pages[j].vaddr) {
                printf ("0x%x\n", pool->loaded_pages[j].vaddr);
            }
            else {
                printf ("------\n");
            }
        }
    }
}
