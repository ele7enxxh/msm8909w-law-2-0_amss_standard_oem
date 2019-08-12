/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M _ A U G M E N T . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

The module contains helper functions to augment DSM memory pool at run time.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2013 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/src/dsm_augment.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/13/13    sh     Implemented FEATURE_DSM_AUGMENT
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "core_variation.h"
#include "comdef.h"
#include "dsm_pool.h"
#include "dsm_item.h"
#include "dsmi.h"
#include "dsm_lock.h"
#include "modem_mem.h"
#include "msg.h"
#include "err.h"


/* If using heap as external memory */
/* start */
#include "memheap.h"
extern mem_heap_type modem_mem_heap;
/* end */


#ifdef FEATURE_DSM_AUGMENT

#ifdef FEATURE_DSM_NATIVE_ASSERT
#define ASSERT( cond )      assert( cond )
#endif

/*===========================================================================

                EXTERNAL  DEFINITIONS AND DECLARATIONS FOR MODULE

 ==========================================================================*/

/* All DSM augment definitions in dsmi.h */


/*===========================================================================

                INTERNAL  DEFINITIONS AND DECLARATIONS FOR MODULE

==========================================================================*/

static uint32 dsma_aug_bytes;
uint32 dsma_ext_memory_check_lower_limit;
uint32 dsma_ext_memory_alloc_upper_limit;
uint32 dsma_ext_memory_upper_limit;


static int16 dsma_augpool_cnt;
dsm_augment_pool_data_type dsma_aug_pools[DSMI_MAX_AUG_POOLS] __attribute__((aligned(8)));

/* DSM Augment statistics */
#define DSM_EVENTS_BUFFER_CNT 20
typedef enum
{
  DSM_MEM_EVENTS_NONE = 0,
  DSM_MAXALLOC_LIMIT_REACHED,
  DSM_EXTTMEM_LIMIT_REACHED,
  DSM_EXTMEM_ALLOC_FAILED,
  DSM_POOL_CNT_LIMIT_REACHED
} dsm_mem_alloc_event_type;

typedef struct
{
  dsm_mempool_id_type pool_id;
  uint32 pool_item_cnt;
  uint32 pool_free_cnt;
  uint32 dbuffer_cnt;
} dsm_mem_pool_statistics_type;

typedef struct
{
  dsm_mem_alloc_event_type mem_alloc_event;
  uint32 ext_mem_dsm_allocated_bytes;
  uint32 ext_mem_current_used_bytes;
  dsm_mem_pool_statistics_type pool_stats;
} dsm_stats_entry_type;

typedef struct
{
  uint32 stats_extmem_alloc_failed_cnt; /* Cumulative count of DSM_EXTMEM_ALLOC_FAILED event*/
  uint32 stats_dsm_maxalloc_reached_cnt; /* Cumulative count of DSM_MAXALLOC_LIMIT_REACHED event*/
  uint32 stats_extmem_limit_reached_cnt; /* Cumulative count of DSM_EXTTMEM_LIMIT_REACHED event*/
  uint32 stats_events_next_entry;           /* Next entry in stats event buffer */
  dsm_stats_entry_type dsm_stats_events_buffer[DSM_EVENTS_BUFFER_CNT]; /* Circular buffer of event statistics */
} dsm_statistics_data_type;

#ifdef FEATURE_DSM_MEM_CHK
static dsm_statistics_data_type dsm_event_stats;

#endif /* #ifdef FEATURE_DSM_MEM_CHK*/

/*-----------------------------------------------------------------
 * External memory  allocation/free
 * ------------------------------------------------------------------*/

static inline void dsma_free(void* ptr)
{
  modem_mem_free(ptr,4);
}

static inline void * dsma_alloc(uint32 size)
{
  return modem_mem_alloc(size,4);
}
uint32 dsm_get_external_memory_usage(void)
{
 return modem_mem_heap.used_bytes;
}

void dsm_get_external_memory_max_alloc(void)
{ /* Consider fragmentation and other usage */
 dsma_ext_memory_upper_limit = modem_mem_heap.total_bytes - DSMI_AUGMENT_EXT_MEM_BACK_OFF;
 dsma_ext_memory_alloc_upper_limit = dsma_ext_memory_upper_limit - DSMI_AUGMENT_EXT_MEM_ALLOC_BACK_OFF;
 
}


/*========================================================================
 *  Functions invoked from DSM Library
 * ========================================================================*/

/*========================================================================
FUNCTION dsmi_augment_check_flow_control_levels()

DESCRIPTION
 Check whether the flow control levels matches the current dsm augment
   configuration

DEPENDENCIES
 None

PARAMETERS
  table - pool
  available_item_count - Flow control level

RETURN VALUE
  Return whether the available item count matches the current dsm augment
  configuration

SIDE EFFECTS None
=========================================================================*/
boolean dsmi_augment_check_flow_control_levels(dsm_pool_mgmt_table_type* table,
          uint32 available_item_count)
{
  dsm_augment_pool_data_type *aug_data = table->augment_data;
  if (aug_data)
  {
    if (available_item_count > ( aug_data->sbuffer_cnt + (aug_data->augment_cnt * 3)))
    {
        return FALSE;
    }
  }
  return TRUE;
}

/*========================================================================
FUNCTION dsmi_augment_get_dbuffer_cnt()

DESCRIPTION
 Get number of dynamic buffers required for the augment items

DEPENDENCIES
 None

PARAMETERS
 augment_items - Total DSM items from augment pool
 augment_count - Number of items in dbuffer

RETURN VALUE
  Number of dynamic buffers required

SIDE EFFECTS None
=========================================================================*/
inline static uint32 dsmi_augment_get_dbuffer_cnt(
        uint32 augment_count,
        uint32 augment_items)
{
  /* Make it multiple of 4 for static buffer calculation items */
  return ((augment_items + augment_count -1)/augment_count);
}

/*========================================================================

FUNCTION dsmi_augment_init_new_buffer()

DESCRIPTION
 Initialize DSM item headers

DEPENDENCIES
 Pool is initialized.

PARAMETERS
  aug_buffer_index - Augment Buffer ID to fill in DSM item header
  buffer - Memory start address
  count - Number of dsm items
  item_size - Size of DSM item

RETURN VALUE
  Top free item in initialized buffer

SIDE EFFECTS
  This function is NOT threadsafe.
  Except for init time, the function is called only from DSM Task Context
=========================================================================*/
static inline dsm_item_type* dsmi_augment_init_new_buffer(
        uint8 aug_buffer_index,
        uint8* buffer,
        uint32 count,
        uint32 item_size)
{
  uint32 i;               /* Array iterator */
  dsm_item_type *item_ptr;     /* pointer to a dsm item  */
  void* top_free_item = NULL;

  /* Make a link of all items
   * Items are initialized at the time of allocation */
  item_ptr = (dsm_item_type *)buffer;
  for (i=0; i < count; i++)
  {
    item_ptr->pool_id = 0;
    item_ptr->pool_id_ext = aug_buffer_index;
    item_ptr->app_ptr = top_free_item;
    top_free_item = item_ptr;
    item_ptr = (dsm_item_type *)((uint8*)item_ptr +
            DSMI_POOL_OBJECT_SIZE_OPT(item_size));
  }
  return top_free_item;
}

/*===========================================================================
FUNCTION DSM_AUGMENT_EXTERNAL_MEM_INIT()

DESCRIPTION
  This function will initialize the Data Service Memory Pool, if augmented from 
  external memory
  This function is called 
   - if Heap memory is not available when DSM is being initialized.
   - if heap is not initialized
   - if static DSM memory is only needed.
DEPENDENCIES
  None
PARAMETERS
  None
RETURN VALUE
  None
SIDE EFFECTS
  Should be called after external memory and dsm pool is initialized
===========================================================================*/
void dsm_augment_external_mem_init(void)
{
  uint32 i;
  
  /* Allow heap allocation by disabling lower limit check */
  dsma_ext_memory_check_lower_limit = 0;  
  dsm_get_external_memory_max_alloc();
  
  dsm_augment_pools();
  for (i = 0; i < dsma_augpool_cnt; i++)
  {
    if ((!dsma_aug_pools[i].recycle_top_free) &&
        (dsma_aug_pools[i].dynamic_top_free))
    {
      dsma_aug_pools[i].recycle_top_free = dsma_aug_pools[i].dynamic_top_free; 
      dsma_aug_pools[i].dynamic_top_free = NULL;
      dsma_aug_pools[i].dsm_task_alloc = TRUE;      
    }
  }
  dsm_augment_pools();  
}

/*========================================================================
FUNCTION dsmi_augment_init_pool()

DESCRIPTION
 Initialize DSM augment data structures for the pool.

DEPENDENCIES
 Pool is initialized.
 External memory manager is initialized.

PARAMETERS
  table - DSM pool to augment
  augment_count - Augment item count for the pool
  augment_items - Total DSM items to be allocated from augment pool
  static_augment_items - number of static augment items
  static_augment_item_array - Address of static pool, from which Static Augment buffer can be allocated.

RETURN VALUE
  None

SIDE EFFECTS
  This function is NOT threadsafe.  Initializes static augment buffers.
=========================================================================*/
void dsmi_augment_init_pool(dsm_pool_mgmt_table_type* table,
        uint32 augment_count,
        uint32 augment_items,
        uint32 static_augment_items,
        uint8* static_augment_item_array)
{

  uint8 *table_end_address;
  dsm_augment_pool_data_type *aug_data;

  ASSERT(dsma_augpool_cnt < DSMI_MAX_AUG_POOLS) 

#ifdef FEATURE_DSM_MEM_CHK
  dsm_event_stats.stats_events_next_entry = 0;
#endif
  /* Disable heap allocation till heap is initialized 
     by setting lower limit */
  dsma_ext_memory_check_lower_limit = 1;

  if (augment_items !=0)
  {
    ASSERT (augment_count > 49 );

    aug_data = &dsma_aug_pools[dsma_augpool_cnt];

    memset(aug_data,0,
            sizeof(dsm_augment_pool_data_type));

    aug_data->pool = table;

    aug_data->augment_cnt = augment_count;

    /* This change is only fr JO.1.2 . Refer comments in dsm_init.c */
	/* Start */
    aug_data->recycle_count = DSMI_AUGMENT_RECYCLE_MIN_COUNT_JO12;

	if (table->pool_item_size == 128)
	{
	  aug_data->recycle_count = DSMI_AUGMENT_RECYCLE_MIN_COUNT_JO12-20;
	}
	if (table->pool_item_size == 0)
	{
	  aug_data->recycle_count = DSMI_AUGMENT_RECYCLE_MIN_COUNT_JO12+50;
	}
    /* End */
    aug_data->sbuffer_cnt = static_augment_items;

    aug_data->static_pool_level_to_reuse_cnt = augment_count + (augment_count >>1);
    
    aug_data->free_dbuffer_level = augment_count + 
        (augment_count * DSM_AUGMENT_REUSE_TRIGGER_FACTOR/100);

    if (aug_data->free_dbuffer_level > DSM_AUGMENT_REUSE_TRIGGER_FACTOR_MAX)
    {
      aug_data->free_dbuffer_level = DSM_AUGMENT_REUSE_TRIGGER_FACTOR_MAX;
    }

    aug_data->dbuffer_limit = dsmi_augment_get_dbuffer_cnt(augment_count,augment_items);

    aug_data->dbuffer_limit += 3; /*To allow for alloc/dynamic/recycled buffer*/

    table_end_address = table->item_array + table->item_count_static *
            (DSMI_POOL_OBJECT_SIZE_OPT(table->pool_item_size));
    ASSERT(aug_data->dbuffer_limit < DSMI_AUGMENT_TOTAL_BLOCKS)

    /* These values are static and will not change */
    aug_data->static_top_free =
         dsmi_augment_init_new_buffer(DSMI_AUGMENT_TOTAL_BLOCKS,static_augment_item_array,
                                     aug_data->sbuffer_cnt,table->pool_item_size);
    static_augment_item_array +=
              (DSMI_POOL_OBJECT_SIZE_OPT(table->pool_item_size))*aug_data->sbuffer_cnt;
    table->free_count += aug_data->sbuffer_cnt;
    table->item_count += aug_data->sbuffer_cnt;

    ASSERT((static_augment_item_array-1) < table_end_address)
    aug_data->recycle_top_free = 0;
    aug_data->dynamic_top_free = 0;

    table->augment_data = (void *)aug_data;

    dsma_augpool_cnt++;
    /* Allocate the dynamic heap */
    /* Heap may be initialized with this call
         * dsm_init depends on heap initialization */
         
    aug_data->dsm_task_alloc = TRUE;
    aug_data->dsm_task_free = FALSE;    
#ifndef FEATURE_DSM_TRIGGER_EXTERNAL_MEMORY_INIT
    dsm_augment_external_mem_init();        
#endif /* ifndef FEATURE_DSM_TRIGGER_EXTERNAL_MEMORY_INIT */
 }

}

/*========================================================================
 *  Functions invoked from HIGH PRIO DSM Task AND DSM_INIT()
 *  Deals only with dynamic buffers
 ========================================================================= */

/*========================================================================
FUNCTION dsma_update_statistics()

DESCRIPTION
  Update statistics entries for major external memory allocation events

DEPENDENCIES

PARAMETERS
  table - Pool
  event  - Event that happened

RETURN VALUE
  None

SIDE EFFECTS
  None. Can be commented out if it affects performance.
=========================================================================*/
static void dsma_update_statistics(
        dsm_mem_alloc_event_type event,
         dsm_pool_mgmt_table_type *table)
{
#ifdef FEATURE_DSM_MEM_CHK
    dsm_stats_entry_type* entry;
    dsm_augment_pool_data_type *aug_data = (dsm_augment_pool_data_type *)table->augment_data;
    entry = &(dsm_event_stats.dsm_stats_events_buffer[dsm_event_stats.stats_events_next_entry]);

    switch (event) {
      case DSM_MAXALLOC_LIMIT_REACHED:
        dsm_event_stats.stats_dsm_maxalloc_reached_cnt++;
        if (table->pool_stats)
        {
          table->pool_stats->stats_ext_mem_alloc_fail_count++;
        }
        break;
      case DSM_EXTTMEM_LIMIT_REACHED:
        dsm_event_stats.stats_extmem_limit_reached_cnt++;
        break;
      case DSM_EXTMEM_ALLOC_FAILED:
        dsm_event_stats.stats_extmem_alloc_failed_cnt++;    
        if (table->pool_stats)
        {
          table->pool_stats->stats_ext_mem_alloc_fail_count++;
        }
        break;
     default:;
    }

    entry->mem_alloc_event = event;
    entry->ext_mem_dsm_allocated_bytes = dsma_aug_bytes;
    entry->ext_mem_current_used_bytes = dsm_get_external_memory_usage();

    /* Fill only entries of the current pool */
    entry->pool_stats.pool_id = (dsm_mempool_id_type)aug_data->pool;
    entry->pool_stats.pool_item_cnt = aug_data->pool->item_count;
    entry->pool_stats.pool_free_cnt = aug_data->pool->free_count;
    entry->pool_stats.dbuffer_cnt = aug_data->dbuffer_count;

    dsm_event_stats.stats_events_next_entry++;
    if (dsm_event_stats.stats_events_next_entry >= DSM_EVENTS_BUFFER_CNT)
    {
      dsm_event_stats.stats_events_next_entry = 0;
    }
#endif /* FEATURE_DSM_MEM_CHK */
}

/*========================================================================
FUNCTION dsma_check_ext_memory_limits()

DESCRIPTION
  Check the configured limits on external memory 
   against current usage of external memory

  Threshold is defined to reenable DSMA allocation after external memory limit is reached, 
  initialized in   dsma_ext_memory_check_lower_limit

DEPENDENCIES

PARAMETERS

RETURN VALUE
 Current heap allocation ,If Limit is not reached, 0 -  If external memory limits are reached.

SIDE EFFECTS
  This function is called from a single thread and is NOT ThreadSafe
=========================================================================*/
static inline uint32 dsma_check_ext_memory_limits(void)
{
    uint32 lheap_alloc;
    
    lheap_alloc = dsm_get_external_memory_usage();
    
    if (!lheap_alloc)
    {
      lheap_alloc = 1;
    }
    
    if (DSM_UNLIKELY(lheap_alloc > dsma_ext_memory_upper_limit))
    {
       dsma_ext_memory_check_lower_limit = 
            lheap_alloc - (DSMI_AUGMENT_MAX_ALLOC>>1);
    
       /* Wait till external memory is reduced to dsma_ext_memory_check_lower_limit for allocation */
       MSG_HIGH("DSMA: External Memory Upper Limit %d reached \n \n",lheap_alloc,0,0);
       lheap_alloc = 0;
    }
    else if (DSM_UNLIKELY(dsma_ext_memory_check_lower_limit))
    {
      if (lheap_alloc<dsma_ext_memory_check_lower_limit)
      {
        /* Continue with DSM allocation */
        MSG_HIGH(" DSMA: External Memory Lower Limit %d reached \n \n",dsma_ext_memory_check_lower_limit,0,0);
        dsma_ext_memory_check_lower_limit = 0;        
      }
      else
      {
        lheap_alloc = 0;
      }
    }
    return lheap_alloc;
}

/*========================================================================
FUNCTION dsma_get_buffer_head()

DESCRIPTION
  Traverse through the array to find a non used buffer location

DEPENDENCIES

PARAMETERS
  aug_data - Augment data of the pool
  new_pool - Buffer head

RETURN VALUE
  Index of empty buffer head or DSMI_AUGMENT_TOTAL_BLOCKS, if none available

SIDE EFFECTS
  This function is called from a single thread and is NOT ThreadSafe
=========================================================================*/
static inline uint32 dsma_get_buffer_head(
        dsm_augment_pool_data_type *aug_data,
        dsm_augment_buffer_type** new_pool)
{
    uint32 i = 0;
    *new_pool = NULL;

    while(i < DSMI_AUGMENT_TOTAL_BLOCKS)
    {
      if (!aug_data->buffer[i].dsma_state)
      {
        *new_pool =  &aug_data->buffer[i];
        return i;
      }
      i++;
    }
    return DSMI_AUGMENT_TOTAL_BLOCKS;
}
/*========================================================================
FUNCTION DSM_AUGMENT_POOLS()

DESCRIPTION
  This API is called from High Priority DSM task to augment pool memory from external memory.
  Also called from dsm_init()

DEPENDENCIES

PARAMETERS

RETURN VALUE  None

SIDE EFFECTS
  This function is called from a single thread and is NOT ThreadSafe.
  Pool's item  count and free count are modified.
=========================================================================*/

void dsm_augment_pools(void)
{
  uint32 i,size,heap_alloc;
  void* buffer;
  uint8* item_array_start;
  uint32 pool_id;
  dsm_augment_buffer_type* new_pool;
  dsm_augment_pool_data_type *aug_data;
  dsm_pool_mgmt_table_type *table;

  dsm_get_external_memory_max_alloc();  
  heap_alloc = dsma_check_ext_memory_limits();
  
  for (i = 0; i < dsma_augpool_cnt; i++)
  {
    if (dsma_aug_pools[i].dsm_task_alloc)
    {
      aug_data = &dsma_aug_pools[i];
      table = aug_data->pool;

      if (!heap_alloc)
      {
        atomic_set(&aug_data->reuse_trigger_cnt,0);
        aug_data->dsm_task_alloc = FALSE;
        dsma_update_statistics(DSM_EXTTMEM_LIMIT_REACHED,table);
        /* Cannot allocate from External memory, Upper limit reached */
        continue;
      }

      if (aug_data->reuse_buffer_cnt)
      {
        /* Try to reuse free buffer */
        dsma_alloc_dynamic_reuse_recycle_buffer(aug_data);
      }
      
      if(!aug_data->dynamic_top_free)
      {
        if (aug_data->dbuffer_count >= aug_data->dbuffer_limit)
        {
            dsma_update_statistics(DSM_POOL_CNT_LIMIT_REACHED,table);
            MSG_HIGH("DSMA: Cannot allocate from External Memory.DBuffer limit %d reached for pool %x",
              aug_data->dbuffer_limit,
              aug_data->pool,0);
            aug_data->dsm_task_alloc = FALSE;
            continue;
        }
        pool_id = dsma_get_buffer_head(aug_data,&new_pool);        
        if (new_pool != NULL)
        {
          /* This will return a memory aligned to 32 bytes */          
          size = DSMA_GET_AUGMENT_BUFFER_SIZE(table->pool_item_size,
                aug_data->augment_cnt);

          if ((dsma_aug_bytes + size > DSMI_AUGMENT_MAX_ALLOC ) || 
              (heap_alloc + size > dsma_ext_memory_alloc_upper_limit))
          {
              if (dsma_aug_bytes + size > DSMI_AUGMENT_MAX_ALLOC)
              {
                MSG_HIGH("DSMA: Exceeded total bytes that can be allocated from ext memory pool %x size %d ",table,dsma_aug_bytes + size,0);
                dsma_update_statistics(DSM_MAXALLOC_LIMIT_REACHED,table);
              }
              else
              {
                MSG_HIGH("DSM Library Config Warning: Low item count in Pool 0x%x used %d. Contact DSM team",table,dsma_aug_bytes + size,0);
                dsma_update_statistics(DSM_EXTTMEM_LIMIT_REACHED,table);
              }
              aug_data->dsm_task_alloc = FALSE;
              return;
          }           
          buffer = dsma_alloc(size);
          if (buffer != NULL)
          {
              MSG_HIGH_DSM_CONFIG(" DSMA alloc_new_buffer id %d dbuffer_cnt %d dbuffer_limit %d",
                   pool_id,aug_data->dbuffer_count,aug_data->dbuffer_limit);
              new_pool->item_array = buffer; /* The item array pointer is only used to delete */
              /* Align the item_array so that item data lands on a 32 byte boundary */
              /*
       
                   +---------------+---------------+---------------+---------------+
                   |T|  pad     | Header         |H|Data                           |T|
                   +---------------+---------------+---------------+---------------+
  
                   +     - indicate 32 byte boundaries.
                   T     - is the tail cookie from the previous buffer.
                   pad   - is padding so that things line up correctly.
                   H     - is the head cookie for this buffer
                   Data  - is the data for this buffer. data may be any multiple of
                     32 bytes.
   
                    */
    
            item_array_start = (uint8*)(((uint32)buffer + 31 ) & ~31 );
            item_array_start += DSM_TAIL_COOKIE_LEN  + table->pad_size;
  
            /* This function should be similar to dsmi_init_pool_data, but optimized for run time
                      initialization */
            new_pool->dsma_free_list.top_item =
                    dsmi_augment_init_new_buffer(pool_id+1,
                              item_array_start,aug_data->augment_cnt,
                              table->pool_item_size);
            
            /* Assert below has to be true, but need not be checked at run time*/
            /* ASSERT((uint8 *)(buffer + size) <=
                           (uint8 *)((uint8*)(new_pool->dsma_free_list.top_item)
                           + DSMI_POOL_OBJECT_SIZE_OPT(dsma_aug_pools[i].pool->pool_item_size)));
                       */
                   
            new_pool->dsma_state = DSMA_ALLOC;
            aug_data->dynamic_top_free = new_pool->dsma_free_list.top_item;
            new_pool->free_list = 0;
            table->item_count+=aug_data->augment_cnt;
            atomic_add(&table->free_count,aug_data->augment_cnt);
            aug_data->dbuffer_count++;
            dsma_aug_bytes += size;
            if (table->pool_stats)
            {
              if (table->pool_stats->stats_max_item_count < table->item_count)
              {
                table->pool_stats->stats_max_item_count = table->item_count;
              }
            }
            /* Collect statistics */
            if (aug_data->dbuffer_count >
                                      aug_data->max_dbuffer_count_interval)
            {
              aug_data->max_dbuffer_count_interval = aug_data->dbuffer_count;
            }
            
            if (aug_data->dbuffer_count >
              aug_data->stats_max_dbuffer_used)
            {
              aug_data->stats_max_dbuffer_used = aug_data->dbuffer_count;
            }     
          }
          else
          {
            dsma_update_statistics(DSM_EXTMEM_ALLOC_FAILED,table);
            MSG_HIGH("DSMA: Could not allocate from Heap %d, %d ",table,aug_data->dbuffer_count,0);
          }
        }
      }
  
          MSG_HIGH_DSM_CONFIG(" DSMA Augent alloc %d dynamic%d, recycle3 %d ",
                          DSMA_GET_TOP_FREE_BUFFER_INDEX(aug_data->alloc_top_free),
                  DSMA_GET_TOP_FREE_BUFFER_INDEX(aug_data->dynamic_top_free),
                  DSMA_GET_TOP_FREE_BUFFER_INDEX(aug_data->recycle_top_free));
  
          MSG_HIGH_DSM_CONFIG(" DSMA Augment pool %x item_cnt %d free_cnt %d",
                table,
                table->item_count,
                table->free_count);
                
      aug_data->dsm_task_alloc = FALSE;
    }
  } 
}

/*========================================================================

FUNCTION DSM_AUGMENT_FREE_BUFFER()

DESCRIPTION
    This API is called from High Priority DSM task after augment pool memory
    to perform necessary cleanup of freed buffers.

DEPENDENCIES

PARAMETERS

RETURN VALUE  
  boolean - False, indicates, there are more reuse buffers to be freed,
                 True , indicates all reuse buffers are freed.

SIDE EFFECTS
  This function is called from a single thread and is NOT ThreadSafe.
  Pool's item  count and free count are modified.
=========================================================================*/
boolean dsm_augment_free_buffer(void)
{
  uint32 i,j,total,size,heap_alloc;
  dsm_augment_buffer_type* free_buffer;
  dsm_augment_pool_data_type *aug_data;
  boolean lcompleted = TRUE;
  int32 delete_block_cnt;
  

  heap_alloc = dsma_check_ext_memory_limits();
  
  for (i = 0; i < dsma_augpool_cnt; i++)
  {    
    if (!heap_alloc)
    {
      atomic_set(&dsma_aug_pools[i].reuse_trigger_cnt,0);
      dsma_aug_pools[i].dsm_task_free = TRUE;
    }

    if(!dsma_aug_pools[i].dsm_task_free)
    {
      continue;
    }
    
    aug_data = &dsma_aug_pools[i];

    if (heap_alloc &&
    ((!aug_data->dynamic_top_free) || (!aug_data->recycle_top_free)))
    {
      /* Try to reuse freed buffers */
      dsma_alloc_dynamic_reuse_recycle_buffer(aug_data);
      MSG_HIGH_DSM_CONFIG(" DSMA allocated in free alloc %d dynamic%d, recycle3 %d ",
           DSMA_GET_TOP_FREE_BUFFER_INDEX(aug_data->alloc_top_free),
           DSMA_GET_TOP_FREE_BUFFER_INDEX(aug_data->dynamic_top_free),
           DSMA_GET_TOP_FREE_BUFFER_INDEX(aug_data->recycle_top_free));
    }

    /* Release only 1 buffer in the function */
    if(aug_data->reuse_trigger_cnt)
    {
      /* Keep only DSMI_AUGMENT_TOTAL_REUSE_BUFFERS reuse buffers circulating */
      delete_block_cnt =
       (aug_data->reuse_buffer_cnt <= DSMI_AUGMENT_TOTAL_REUSE_BUFFERS) ? \
       0:aug_data->reuse_buffer_cnt-DSMI_AUGMENT_TOTAL_REUSE_BUFFERS+1;
    }
    else
    {
      delete_block_cnt = aug_data->reuse_buffer_cnt;
      aug_data->max_dbuffer_count_interval = 0;
    }
    /* If not timed right, the value delete_block_cnt, could be negative */
    if( delete_block_cnt > 0)
    {    
      size = DSMA_GET_AUGMENT_BUFFER_SIZE(aug_data->pool->pool_item_size,
          aug_data->augment_cnt);
      if(aug_data->pool->free_count > (aug_data->pool->item_count_static + 3*aug_data->augment_cnt))
      {
        ERR_FATAL("DSM Library Config Error 1 : Low item count in Pool 0x%x Contact DSM team",aug_data->pool,0,0);
      }
      j = DSMI_AUGMENT_TOTAL_BLOCKS-1;
      total = aug_data->dbuffer_count;
      while (total >0)
      {
        if (aug_data->buffer[j].dsma_state)
        {
          total--;
          if (aug_data->buffer[j].dsma_state == DSMA_REUSE)
          {
           free_buffer = &aug_data->buffer[j];
           delete_block_cnt--;
           if (atomic_compare_and_set(&free_buffer->dsma_state, DSMA_REUSE,DSMA_FREE))
           {
             /* The value could be modified in DSM library */
             MSG_HIGH_DSM_CONFIG("DSMA Freed buffer %d in pool 0x%x delete_buffer_cnt %d",j, aug_data->pool,delete_block_cnt);
             MSG_HIGH_DSM_CONFIG("DSMA Freed buffer pool free_cnt %d, dbuffer_cnt %d, reuse_buffer_cnt %d",
                                 aug_data->pool->free_count,
                                 aug_data->dbuffer_count,
                                 aug_data->reuse_buffer_cnt);
             aug_data->pool->item_count-=aug_data->augment_cnt;
             dsma_free(free_buffer->item_array);
             memset(free_buffer,0, sizeof(dsm_augment_buffer_type));
             aug_data->dbuffer_count --;
             atomic_dec(&aug_data->reuse_buffer_cnt);
             dsma_aug_bytes -= size;
             break;
           }
         }
        }
        j--;
      }      
    }
    if (delete_block_cnt > 0)
    {
      /* Keep it running in a loop based on timer */
      lcompleted  = FALSE;
    }
    else
    {
      lcompleted  = TRUE;
      aug_data->dsm_task_free = FALSE;
    }
  }
  return lcompleted;
}

#endif /* FEATURE_DSM_AUGMENT */

