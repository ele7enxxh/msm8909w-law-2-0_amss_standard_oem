/* Pager source code */

#include <stdio.h>
#include <stdlib.h>
#include <qube.h>
#include <blast.h>
#include "memload_handler.h"


static int demand_load_debug_enabled = 1;
static int load_log_num_entries = 4096;
static int duplicate_pf_count;

int start_counter = 0;
long long prev_pcycles, cur_pcycles;

/* starting address of flash data to load... only for simulation of load_page without actual flash */
extern unsigned int logical_start;
//extern int image_vstart;

/* swap pools */
struct swap_page_pool swap_pools[NUM_SWAP_POOLS];

//size is in PAGES, not bytes! 
void init_active_page_pool (int pool_index, unsigned int pool_base_addr, int size)
{
  //assert(pool_ptr);
  assert(pool_index >= 0 && pool_index < NUM_SWAP_POOLS);
  assert(size >= 1);

  //obtain pool to init:
  struct swap_page_pool * pool = &swap_pools[pool_index];
  pool->active_page_pool = pool_base_addr;
  //pool->active_page_pool = (struct active_page*)pool_ptr; //will be pointing to first page in the pool
  pool->active_page_pool_size = size;

  pool->active_page_list = (struct active_page_list_entry*)malloc( sizeof(struct active_page_list_entry) * 
                                                                   pool->active_page_pool_size );
  assert(pool->active_page_list);
  int i ;
  for( i=0; i < pool->active_page_pool_size; i++) {
        pool->active_page_list[i].addr = 0UL;
        pool->active_page_list[i].paddr = 0UL;
  }
  pool->active_page_list_count = 0;
  pool->active_page_list_index = 0;

  /* Demand Loading Debug */
  pool->page_replacement_count = 0;

  /* Initialize the Load/Evict Log */
  pool->load_log_list_count = 0;
  pool->load_log_list_index = 0;
  if ( demand_load_debug_enabled ) 
  {
      pool->load_log_list =
          (struct load_log_list_entry*)malloc(sizeof(struct load_log_list_entry) * load_log_num_entries );
      assert(pool->load_log_list);

      for(i=0; i < load_log_num_entries; i++) 
      {
          pool->load_log_list[i].loaded_vaddr = 0UL;
          pool->load_log_list[i].evicted_vaddr = 0UL;
          pool->load_log_list[i].thread = 0;//or something qurt specific?
      }
  }

}/* init_active_pool */

static mapping_table_entry_t *mapping_table;
void dl_segment_init (swap_pools_t *dl_pool_configs)
{
  //int mapping_table_size = (dl_pool_configs->swap_pool0_size + 
  //                           dl_pool_configs->swap_pool1_size )/SIZE_4K; //at least semantically
   duplicate_pf_count = 0;
   mapping_table = (mapping_table_entry_t *) malloc ((sizeof (struct mapping_table_entry)) * MAPPING_TABLE_SIZE);
   //zero out mapping table:
   int i = 0;
   for (i = 0; i < MAPPING_TABLE_SIZE; i++)
   {
       mapping_table[i].physAddr = 0UL;
       mapping_table[i].virtAddr = 0UL;
   }
}/* init_dl_segment*/


/* This function inserts a page entry into the active list
   Returns:  non-zero if there is room on the list
             zero if list is full and there was no room
             output param dest_addr containing address to load inserted page into */
int insert_active_page(int pool_index, uintptr_t addr, unsigned int * dest_addr, unsigned int caller_thread)
{
    /* DEBUG: Assert state of list and params */
    //assert(pool_index >= 0 && pool_index < NUM_SWAP_POOLS);
    //assert(dest_addr);

    /* Obtain the pool structure from the array */
    struct swap_page_pool* pool = &swap_pools[pool_index];

    //assert(pool->active_page_list_count >= 0);
    //assert(pool->active_page_list_count <= pool->active_page_pool_size);
    //assert(pool->active_page_list_index >= 0);
    //assert(pool->active_page_list_index <= pool->active_page_pool_size);

    /* Increment the page count unless the list is full */
    if( pool->active_page_list_count < pool->active_page_pool_size ) {
        /* DEBUG: This will only happen as we're filling, so ensure index=count */
        assert(pool->active_page_list_index == pool->active_page_list_count);

        pool->active_page_list_count++;
    }

    /* Loop around circular buffer if necessary */
    if( pool->active_page_list_index >= pool->active_page_pool_size ) {
        pool->active_page_list_index = 0;
    }

    /* Calculate the address to load this page into, based on the list index */
    //*dest_addr = (uintptr_t)&pool->active_page_pool[pool->active_page_list_index];
    *dest_addr = pool->active_page_pool + SIZE_4K * pool->active_page_list_index;

    /* Place the page in the list */
    pool->active_page_list[pool->active_page_list_index].addr = addr;
    pool->active_page_list[pool->active_page_list_index].paddr = *dest_addr;

    /* Move the index ahead */
    pool->active_page_list_index++;

    /* Demand Loading Debug */
    if ( demand_load_debug_enabled ) {
        /* Update the load log with load info */
        pool->load_log_list[pool->load_log_list_index].loaded_vaddr = addr;
        pool->load_log_list[pool->load_log_list_index].thread = caller_thread;
        pool->load_log_list_index++;
        pool->load_log_list_count++;
        /* Loop around circular buffer if necessary */
        if( pool->load_log_list_index >= load_log_num_entries ) {
            pool->load_log_list_index = 0;
        }
    }

    //printf("Inserting Page: addr=%p paddr=%p count=%d index=%d pool_num=%d\n",
    // addr, *dest_addr, pool->memsection_active_page_list_count,
    // pool->memsection_active_page_list_index, pool_index);

    return 1;
}/* insert_active_page */



/* This function removes a page entry from the active list only if the list is currently full
   Returns:  non-zero if list was full and entry was removed
             zero if list is not full */
int remove_active_page(int pool_index, uintptr_t* addr, uintptr_t* paddr)
{
    int victim_index;

    /* DEBUG: Assert state of list and params */
    //assert(pool_index >= 0 && pool_index < NUM_SWAP_POOLS);
    //assert(addr);
    //assert(paddr);

    /* Obtain the pool structure from the array */
    struct swap_page_pool* pool = &swap_pools[pool_index];

    //assert(pool->active_page_list_count >= 0);
    //assert(pool->active_page_list_count <= pool->active_page_pool_size);
    //assert(pool->active_page_list_index >= 0);
    //assert(pool->active_page_list_index <= pool->active_page_pool_size);

    /* Return if we're not full */
    if( pool->active_page_list_count < pool->active_page_pool_size ) {
        *addr = 0UL;
        *paddr = 0UL;
        return 0;
    }

    /* Calculate the victim index */
    victim_index = pool->active_page_list_index;
    if( victim_index >= pool->active_page_pool_size ) {
        victim_index = 0;
    }

    /* Set the victim addr and paddr output params */
    *addr = pool->active_page_list[victim_index].addr;
    *paddr = pool->active_page_list[victim_index].paddr;


    if ( demand_load_debug_enabled ) {
        /* Update the load log with eviction info */
        pool->load_log_list[pool->load_log_list_index].evicted_vaddr = *addr;
        pool->page_replacement_count++; //this may not be needed
//    printf("Replacing Page: addr=%p paddr=%p count=%d index=%d vic_index=%d pool_num=%d\n",
//            *addr, *paddr, pool->memsection_active_page_list_count,
//            pool->memsection_active_page_list_index, victim_index, pool_index );
    }

    return 1;
} /* remove_active_page */


void simulate_load_page (unsigned int fault_addr, unsigned int dest_addr, unsigned int logical_start)
{
  //calculate actual virt addr intermedially mapped:
  unsigned int virt_offset = fault_addr - (unsigned int)&__swapped_segments_start__;
  unsigned int logical_addr = logical_start + virt_offset;
  memcpyK ((void *)dest_addr, (void *)logical_addr, SIZE_4K);
  //printf ("logical_addr to load from: ptr-%x, actual-%x\n", (void *)logical_addr, logical_addr);

} /* simulate_load_page */


int memload_fault_handler (pagefault_t *event_data)
{
    int r; //used for return results
    unsigned int addr = event_data->faulting_addr;
    //swap_region_t *swap_region = (swap_region_t *)malloc (sizeof (swap_region_t));//structure "comparable" to memsection
    //assert(swap_region != NULL);
    unsigned int flags = 0;
    //swap_region->flags = flags;
    //swap_region->pool_index = flags >> MI_PBT_FLAG_POOL_INDEX_SHIFT; 
    int pool_index = flags >> MI_PBT_FLAG_POOL_INDEX_SHIFT;

    //know for sure it is from a swapped segment: kernel verified this, remove the check      
    /* Check if this is a reloadable memsection, we may need to unload a page to make room */
    uintptr_t victim_paddr;
    uintptr_t victim_addr; //this is vadd
    uintptr_t victim_page;
    unsigned int dest_addr;
    uintptr_t page; 
    int removed = 0;
    uintptr_t unmap_page;

    /* check mapping_table for a duplicate page fault */
     page = (addr >> PAGE_SHIFT_BITS << PAGE_SHIFT_BITS);
     //page = page - (uintptr_t)image_vstart;
     page = page - (unsigned int) &__swapped_segments_start__;
     page = page >> PAGE_SHIFT_BITS;
     if ((mapping_table[page].physAddr != 0) && (mapping_table[page].physAddr != 0x1f1f1f1f))
     {
        duplicate_pf_count++;
        printf ("duplicate page fault... mapping already exists \n");
        return EOK;//still return ok, no problem
     }

    /* Obtain the Victim page and remove it from the active list (see implementation for algorithm)
     * This function returns non-zero if the active list is at capacity, and a page was removed
     * This function returns zero if the active list is not yet full, and no page was removed  */
     if(remove_active_page(pool_index, &victim_addr, &victim_paddr)) 
     {
        unmap_page = victim_addr >> PAGE_SHIFT_BITS << PAGE_SHIFT_BITS;
        removed = 1;
        start_counter = 1;
        cur_pcycles = qurt_get_core_pcycles();
        printf("memload+remove_active_page: %d  \n",(unsigned int)(cur_pcycles-prev_pcycles));
        prev_pcycles = qurt_get_core_pcycles();;
        /* unmap page, do not free range: defer this till later */
        //qurt_remove_mapping(unmap_page, 0, SIZE_4K);              
        
        //victim_page = ((pool->active_page_list[index].addr) - pool->base) / l4e_min_pagesize();
        //victim_page = unmap_page - image_vstart; // extern vimage_start
        victim_page = unmap_page - (unsigned int)&__swapped_segments_start__;
        victim_page = victim_page >> PAGE_SHIFT_BITS; //should end up with 12 bits for index to the mapping table
        mapping_table[victim_page].physAddr = 0UL;
     }

     /* insert active page: will return destination address */
     r = insert_active_page(pool_index, addr, &dest_addr, event_data->thread_id);            
    if (start_counter) {            
        cur_pcycles = qurt_get_core_pcycles();
        printf("insert_active_page: %d  \n",(unsigned int)(cur_pcycles-prev_pcycles));
        prev_pcycles = qurt_get_core_pcycles();
    }
     //FIXME: make sure to test this as well, but need flash
     //load page: bsp_qurt_load_page(addr, dest_addr);         
     //dl_pager_load_virtual_page (addr, dest_addr);
     //simulated FLASH load_page, extern logical_start
     //logical_start set manually, of same size as swap_segm 
     simulate_load_page (addr, dest_addr, logical_start);
 
    if (start_counter) {            
        cur_pcycles = qurt_get_core_pcycles();
        printf("simulate_load_page: %d  \n",(unsigned int)(cur_pcycles-prev_pcycles));
        prev_pcycles = qurt_get_core_pcycles();
    }
     /* flush Dcache on every load */
     qurt_mem_dcache_clean((const void *) dest_addr, SIZE_4K);

     /* if eviction happened, flush icache as well */
     //if ((removed) && (addr >= (unsigned int)&__swapped_segments_text_start__) &&
     //    (addr < (unsigned int) &__swapped_segments_end__))
     //{
     //   qmem_qurt_inv_icache((const void *)dest_addr, SIZE_4K);
       // //qmem_qurt_cleaninv_dcache((const void *) dest_addr, SIZE_4K);
     //}

     /* remove mapping after flush, otherwise cannot flush by address */
     if (removed)
     {
        qurt_mapping_remove(unmap_page, 0, SIZE_4K);
        //printf ("unmpping this page: %x\n", unmap_page);
     } 
        
    if (start_counter) {            
        cur_pcycles = qurt_get_core_pcycles();
        printf("remove_mapping+$: %d\n ",(unsigned int)(cur_pcycles-prev_pcycles));
        prev_pcycles = qurt_get_core_pcycles();
    }
     /* map page: hardcode size, CCC, perms */
     //QMEM_PERM_READ: 4, QMEM_PERM_EXECUTE: 1, or of these:
     //FIXME: permissions are WRONG, this is just for testing because George is writing to memory which 
     //should not be allowed 
     qurt_mapping_create(addr, dest_addr, SIZE_4K, QMEM_CACHE_WRITEBACK, 0x7); 

    if (start_counter) {            
        cur_pcycles = qurt_get_core_pcycles();
        printf("create_mapping: %d  \n\n===========================================================\n",(unsigned int)(cur_pcycles-prev_pcycles));
    }
#if 0
     /* update mapping table */        
     page = (addr >> PAGE_SHIFT_BITS << PAGE_SHIFT_BITS);

     //page = page - (uintptr_t)image_vstart;
     page = page - (uintptr_t)&__swapped_segments_start__;
     page = page >> PAGE_SHIFT_BITS; 
#endif

     mapping_table[page].physAddr = dest_addr;
     mapping_table[page].virtAddr = addr;

     return EOK;
}/* memload_handler */

void pager(void * name)
{
int ret;
    pagefault_t pf_data;
    swap_pools_t pools_configs;
    //qmem_region_attr_t attr;
    //qmem_region_t region;

    //ret = qurt_get_env_object(ENV_OBJ_SWAP_POOLS, (unsigned int *)&pools_configs);
		ret = qurt_sysenv_get_swap_spool0((qurt_sysenv_swap_pools_t *)&pools_configs);
    assert (ret == EOK);
    printf("Pager: swap pool0 pools_configs address=%x \n",(unsigned int)&pools_configs);
		
		ret = qurt_sysenv_get_swap_spool1((qurt_sysenv_swap_pools_t *)(&pools_configs.swap_pool1_addr));
    assert (ret == EOK);
    printf("Pager: swap pool1 pools_configs address=%x \n",(unsigned int)&pools_configs.swap_pool1_addr);
    printf("Pager: swap pool0 address=%x, size = %x; swap pool1 address=%x, size=%x\n",
            pools_configs.swap_pool0_addr, pools_configs.swap_pool0_size,
            pools_configs.swap_pool1_addr, pools_configs.swap_pool1_size);

      
    //init swap pools here;
      init_active_page_pool(0, pools_configs.swap_pool0_addr, pools_configs.swap_pool0_size/SIZE_4K);
      init_active_page_pool(1, pools_configs.swap_pool1_addr, pools_configs.swap_pool1_size/SIZE_4K);
      dl_segment_init(&pools_configs);
    

    while (1) {
        // receive page fault event with fautling address in payload
        ret = qurt_receive_system_event (EVENT_PAGEFAULT, (unsigned int *)&pf_data);
        assert(ret == EOK);
        printf ("pf_data->addr: %x\n", pf_data.faulting_addr);
        //printf("receive page fault info: threadID=%x, faulting address=%x\n", pf_data.thread_id, pf_data.faulting_addr);
            
        unsigned int f1 = pf_data.faulting_addr - (unsigned int)&__swapped_segments_start__;
        unsigned int f2 = (unsigned int)&__swapped_segments_end__ - (unsigned int)&__swapped_segments_start__;
        if (f1 >= f2){
             printf ("out of range!!!, f1: %x, f2: %x \n", f1, f2);
             //break;
        }
        //else     //call mem_load_fault_handler:
        if (start_counter)
            prev_pcycles = qurt_get_core_pcycles();
        memload_fault_handler (&pf_data);

        // resume the faultiing thread
        ret = qurt_thread_resume(pf_data.thread_id);
        assert( ret == EOK );
    }
    qurt_thread_exit(0);

}/* pager */


void dump_mapping_table(void)
{
  int i = 0;
  unsigned int j = 0;
  for (i = 0; i < MAPPING_TABLE_SIZE; i++)
  {
     j = (unsigned int)i;
     if ((mapping_table[i].physAddr != 0) && (mapping_table[i].physAddr != 0x1f1f1f1f))
        printf ("m_tbl[%x]: %x -- %x \n", i, mapping_table[i].virtAddr, 
                                             mapping_table[i].physAddr);
  }
 
}/* dump_mapping_table */


void dump_pool (int pool_index)
{
    struct swap_page_pool* pool = &swap_pools[pool_index]; 
    int i = 0;
    printf ("    pool_index: %d\n", pool_index);
    printf ("    base_address: %x \n", pool->active_page_pool);
    printf ("    active_page_pool_size in 4K: %d \n", pool->active_page_pool_size);
    printf ("    active_page_list_count: %d\n", pool->active_page_list_count);
    printf ("    list of active mappings for this pool.....\n");
    for (i = 0 ; i < pool->active_page_list_count; i++)
    {
         if ((pool->active_page_list[i].paddr != 0) && (pool->active_page_list[i].paddr != 0x1f1f1f1f))
         printf ("virt: %x -- phys: %x\n", pool->active_page_list[i].addr,
                                           pool->active_page_list[i].paddr);
    }
    printf ("    total replacements: %d \n", pool->page_replacement_count);
    if (demand_load_debug_enabled)
    {
      //dump load _log:
      int j = 0;
      for (j = 0; j < load_log_num_entries; j++)
      {
         if ((pool->load_log_list[j].evicted_vaddr != 0)&&
             (pool->load_log_list[j].evicted_vaddr != 0x1f1f1f1f))
             printf ("victim: %x, replacement: %x \n", pool->load_log_list[j].evicted_vaddr,
                                                       pool->load_log_list[j].loaded_vaddr); 
      }
    }
    printf ("duplicate pagefaults total: %d\n", duplicate_pf_count);

}/* dump_pool */


