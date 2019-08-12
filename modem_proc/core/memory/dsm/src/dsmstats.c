/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M S T A T S . C

GENERAL DESCRIPTION
  DMSS Data Services statistics collection.

EXTERNALIZED FUNCTIONS

The module contains DSM Statistics function implementations

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/src/dsmstats.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/14    sh     Implemented statistics required by A2 team CR 749150, specific to Modem 
===========================================================================*/

#include "core_variation.h"
#include "comdef.h"

#include "dsm_pool.h"
#include "dsm_queue.h"
#include "dsm.h"
#include "dsmi.h"
#include "dsmstats.h"

/*******************************************************************
*        D S M     P O O L         S T A T I S T I C    F U N C T I O N S                   *
*                                                                                                              *
*        U S E D      O N L Y     F O R     D E B U G G I N G                                 *
*                                                                                                              *
*                                                                                                              *
* Statistics are approximate as requested by CR 749150 / A2 team and             *
* should not be used for logic based decisions.                                               *
* Please use dsm_pool_cb* functions for data consistency.                               *
* This function does not guarantee data consistency.                                       *
********************************************************************/


#define DSMI_STAT_POOLS_CNT 3
static uint32 dsm_stats_num_pools = 0;
dsm_pool_stats_entry dsm_pool_stats_array[DSMI_STAT_POOLS_CNT];


/*==================================================================

                     INTERNAL FUNCTIONS
===================================================================*/


/*========================================================================
FUNCTION dsm_pool_stats_get_recycle_min_count()

DESCRIPTION
   Returns recycle minimum count for a buffer

DEPENDENCIES

PARAMETERS
   Table -Pool Mgmt table
   
RETURN VALUE  
   Returns recycle minimum count for a buffer
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static uint32 dsm_pool_stats_get_recycle_min_count(
              dsm_pool_mgmt_table_type *table)
{
#ifdef FEATURE_DSM_AUGMENT
    uint32 item_count = DSM_MODEM_STATS_DEFAULT_RET_VALUE;
    
    if (table->augment_data !=NULL)
    {
     item_count = DSMI_AUGMENT_RECYCLE_MIN_COUNT_JO12;
    }   
    return item_count;
#else
    return DSM_MODEM_STATS_DEFAULT_RET_VALUE;
#endif
}
/*========================================================================
FUNCTION DSM_POOL_STATS_GET_EXT_MEM_START_ALLOC_FREE_ITEM_TRIGGER()

DESCRIPTION
   Returns free item count for DSM to start allocating from external memory

DEPENDENCIES

PARAMETERS
   Table -Pool Mgmt table
   
RETURN VALUE  
   Returns free item count for DSM to start allocating from external memory
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static uint32 dsm_pool_stats_get_ext_mem_start_alloc_free_item_trigger(
              dsm_pool_mgmt_table_type *table)
{
#ifdef FEATURE_DSM_AUGMENT
    uint32 item_count = DSM_MODEM_STATS_DEFAULT_RET_VALUE;
    dsm_augment_pool_data_type *aug_pool;
    
    if (table->augment_data !=NULL)
    {
      aug_pool =  (dsm_augment_pool_data_type *)table->augment_data;
      item_count = (aug_pool->augment_cnt*2) + aug_pool->sbuffer_cnt;
    }   
    return item_count;
#else
    return DSM_MODEM_STATS_DEFAULT_RET_VALUE;
#endif
}
/*========================================================================
FUNCTION DSM_POOL_STATS_GET_CONFIG_EXT_MEM_MAX_ALLOC_BYTES()

DESCRIPTION
   Returns configured maximum allocation bytes from external memory for the pool

DEPENDENCIES

PARAMETERS
   Table -Pool Mgmt table
   
RETURN VALUE  
   Returns configured maximum allocation bytes from external memory for the pool
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static uint32 dsm_pool_stats_get_config_ext_mem_max_alloc_bytes(
               dsm_pool_mgmt_table_type *table)
{
#ifdef FEATURE_DSM_AUGMENT
     uint32 bytes = 0;
     dsm_augment_pool_data_type *aug_pool;
      
     if (table->augment_data !=NULL)
     {
        aug_pool =  (dsm_augment_pool_data_type *)table->augment_data;
        bytes = aug_pool->dbuffer_limit*aug_pool->augment_cnt*
            DSMI_POOL_OBJECT_SIZE_OPT(table->pool_item_size);
      }   
      return bytes;
#else
     return 0;
#endif
}


/*========================================================================
FUNCTION DSM_POOL_STATS_GET_AUGMENT_COUNT()

DESCRIPTION
   Returns augment count for Pool

DEPENDENCIES

PARAMETERS
   Table -Pool Mgmt table
   
RETURN VALUE  
   Returns augment count for Pool
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static  uint32 dsm_pool_stats_get_augment_count(
              dsm_pool_mgmt_table_type *table)
{
#ifdef FEATURE_DSM_AUGMENT
         uint32 item_count = 0;
          
         if (table->augment_data !=NULL)
         {
            item_count =  ((dsm_augment_pool_data_type *)table->augment_data)->augment_cnt;
          }   
          return item_count;
#else
         return 0;
#endif
}

/*========================================================================
FUNCTION DSM_POOL_STATS_GET_MAX_ITEM_COUNT()

DESCRIPTION
   Returns maximum item count for the pool

DEPENDENCIES

PARAMETERS
   Table -Pool Mgmt table
   
RETURN VALUE  
   Returns maximum item count for the pool
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static uint32 dsm_pool_stats_get_max_item_count(
              dsm_pool_mgmt_table_type *table)
{
#ifdef FEATURE_DSM_AUGMENT
     uint32 item_count = 0;
     dsm_augment_pool_data_type *aug_pool;
    
     if (table->augment_data !=NULL)
     {
        aug_pool =  (dsm_augment_pool_data_type *)table->augment_data;
        item_count = table->item_count_static +
                aug_pool->dbuffer_limit*aug_pool->augment_cnt;
      }   
      else
      {
      item_count = table->item_count;
      }
      return item_count;
#else     
     return table->item_count;
#endif

}


/*========================================================================
FUNCTION DSM_POOL_STATS_GET_ITEMS_NOT_ALLOCATED()

DESCRIPTION
   Returns item count not allocated from pool

DEPENDENCIES

PARAMETERS
   Table -Pool Mgmt table
   
RETURN VALUE  
   Returns item count not allocated from pool
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static uint32 dsm_pool_stats_get_items_not_allocated(
              dsm_pool_mgmt_table_type *table)
{   
#ifdef FEATURE_DSM_AUGMENT
     uint32 free_cnt,i;         
     dsm_augment_pool_data_type *aug_pool;
     free_cnt = table->free_count;
     if (table->augment_data !=NULL)
     {
        aug_pool =  (dsm_augment_pool_data_type *)table->augment_data;
        for (i = 0; i < DSMI_AUGMENT_TOTAL_BLOCKS; i++)
        {
            if (aug_pool->buffer[i].dsma_state == DSMA_USED)
                {
                    free_cnt += aug_pool->buffer[i].dsma_free_list.cnt;
                }
        }           
      }   
      return free_cnt;
#else
     return table->free_count;
#endif

}

/*========================================================================
FUNCTION DSM_POOL_STATS_GET_CLIENT_ALLOC_BYTES()

DESCRIPTION
   Returns number of bytes allocated by clients in this pool

DEPENDENCIES

PARAMETERS
      Table -Pool Mgmt table
      
RETURN VALUE  
    Returns number of bytes allocated by clients in this pool
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static uint32 dsm_pool_stats_get_client_alloc_bytes(
              dsm_pool_mgmt_table_type *table)
{
     uint32 items_not_allocated = dsm_pool_stats_get_items_not_allocated(table);
     return (table->item_count-items_not_allocated)*table->pool_item_size;
}

/*========================================================================
FUNCTION DSM_POOL_STATS_GET_EXT_MEM_ALLOC_BYTES()

DESCRIPTION
   Returns number of bytes allocated from external memory by this pool
DEPENDENCIES

PARAMETERS
   Table -Pool Mgmt table
   
RETURN VALUE  
    Returns number of bytes allocated from external memory by this pool
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static uint32 dsm_pool_stats_get_ext_mem_alloc_bytes(
              dsm_pool_mgmt_table_type *table)
{
#ifdef FEATURE_DSM_AUGMENT
    uint32 bytes = 0;
    dsm_augment_pool_data_type *aug_pool;
          
    if (table->augment_data !=NULL)
    {
       aug_pool =  (dsm_augment_pool_data_type *)table->augment_data;
       bytes = aug_pool->dbuffer_count*aug_pool->augment_cnt*
                 DSMI_POOL_OBJECT_SIZE_OPT(table->pool_item_size);
    }   
    return bytes;
#else
    return 0;
#endif

}

/*========================================================================
FUNCTION DSM_POOL_STATS_GET_EXT_MEM_MAX_ALLOC_BYTES()

DESCRIPTION
   Returns maximum number of bytes allocated from external memory by this pool
DEPENDENCIES

PARAMETERS
      Table -Pool Mgmt table
      
RETURN VALUE  
    Returns maximum number of bytes allocated from external memory by this pool
   
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
=========================================================================*/
static uint32 dsm_pool_stats_get_ext_mem_max_alloc_bytes(
             dsm_pool_mgmt_table_type *table)
{
#ifdef FEATURE_DSM_AUGMENT
        uint32 bytes = 0;
        dsm_augment_pool_data_type *aug_pool;
              
        if (table->augment_data !=NULL)
        {
           aug_pool =  (dsm_augment_pool_data_type *)table->augment_data;
           bytes = aug_pool->stats_max_dbuffer_used*aug_pool->augment_cnt*
                     DSMI_POOL_OBJECT_SIZE_OPT(table->pool_item_size);
        }   
        return bytes;
#else
        return 0;
#endif
}

/*==================================================================

                     PUBLIC FUNCTIONS
===================================================================*/


/*===========================================================================

FUNCTION DSMI_MODEM GET_STATISTICS

DESCRIPTION
  Return statistics requested by Modem team - CR 749150
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None 
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
===========================================================================*/
void dsmi_modem_get_statistics( dsm_modem_stats_info_s  *dsm_modem_stats_info)
{
    
    /* Fill modem related dsm_modem_stats_info_s as indicated by CR 749150       */
    ASSERT(dsm_modem_stats_info !=NULL);

     /* This value can be interpreted by Modem teams */
     /* If segment loading is on, in some PLs, dsm will not be initialized in FTM mode */
     /* This is very modem specific information, so please check with modem teams */   
    dsm_modem_stats_info->config.common.is_initialized = dsm_init_check();
   
#ifdef FEATURE_DSM_AUGMENT
    dsm_modem_stats_info->config.common.max_heap_alloc_bytes = DSMI_AUGMENT_MAX_ALLOC;
#else
    dsm_modem_stats_info->config.common.max_heap_alloc_bytes = 0;
#endif
    dsm_modem_stats_info->config.dup_pool.augment_buffer_recycle_minimum_count =
                                   dsm_pool_stats_get_recycle_min_count(&dsm_dup_item_pool);
    dsm_modem_stats_info->config.dup_pool.free_item_cnt_heap_trigger =
                                   dsm_pool_stats_get_ext_mem_start_alloc_free_item_trigger(&dsm_dup_item_pool);
    dsm_modem_stats_info->config.dup_pool.max_heap_alloc_bytes =
                                   dsm_pool_stats_get_config_ext_mem_max_alloc_bytes(&dsm_dup_item_pool);
    dsm_modem_stats_info->config.dup_pool.num_items_per_heap_alloc =
                                   dsm_pool_stats_get_augment_count(&dsm_dup_item_pool);
    dsm_modem_stats_info->config.dup_pool.max_item_cnt =
                                   dsm_pool_stats_get_max_item_count(&dsm_dup_item_pool);

    dsm_modem_stats_info->config.large_pool.augment_buffer_recycle_minimum_count =
                                   dsm_pool_stats_get_recycle_min_count(&dsm_ds_large_item_pool);
    dsm_modem_stats_info->config.large_pool.free_item_cnt_heap_trigger =
                                   dsm_pool_stats_get_ext_mem_start_alloc_free_item_trigger(&dsm_ds_large_item_pool);
    dsm_modem_stats_info->config.large_pool.max_heap_alloc_bytes =
                                   dsm_pool_stats_get_config_ext_mem_max_alloc_bytes(&dsm_ds_large_item_pool);
    dsm_modem_stats_info->config.large_pool.num_items_per_heap_alloc =
                                   dsm_pool_stats_get_augment_count(&dsm_ds_large_item_pool);
    dsm_modem_stats_info->config.large_pool.max_item_cnt =
                                   dsm_pool_stats_get_max_item_count(&dsm_ds_large_item_pool);

    dsm_modem_stats_info->config.small_pool.augment_buffer_recycle_minimum_count =
                                   dsm_pool_stats_get_recycle_min_count(&dsm_ds_small_item_pool);
    dsm_modem_stats_info->config.small_pool.free_item_cnt_heap_trigger =
                                   dsm_pool_stats_get_ext_mem_start_alloc_free_item_trigger(&dsm_ds_small_item_pool);
    dsm_modem_stats_info->config.small_pool.max_heap_alloc_bytes =
                                   dsm_pool_stats_get_config_ext_mem_max_alloc_bytes(&dsm_ds_small_item_pool);
    dsm_modem_stats_info->config.small_pool.num_items_per_heap_alloc =
                                   dsm_pool_stats_get_augment_count(&dsm_ds_small_item_pool);
    dsm_modem_stats_info->config.small_pool.max_item_cnt =
                                   dsm_pool_stats_get_max_item_count(&dsm_ds_small_item_pool);

    dsm_modem_stats_info->stats.dup.items_not_allocated =
                                 dsm_pool_stats_get_items_not_allocated(&dsm_dup_item_pool);
    dsm_modem_stats_info->stats.dup.client_alloc_bytes =
                                dsm_pool_stats_get_client_alloc_bytes(&dsm_dup_item_pool);
    dsm_modem_stats_info->stats.dup.heap_alloc_bytes =
                                dsm_pool_stats_get_ext_mem_alloc_bytes(&dsm_dup_item_pool);
    dsm_modem_stats_info->stats.dup.max_heap_alloc_bytes =
                                dsm_pool_stats_get_ext_mem_max_alloc_bytes(&dsm_dup_item_pool);
    dsm_modem_stats_info->stats.dup.available_free_cnt = DSM_POOL_FREE_CNT(&dsm_dup_item_pool);

    dsm_modem_stats_info->stats.dup.item_alloc_fail_cnt = 0;
    dsm_modem_stats_info->stats.dup.heap_alloc_fail_cnt = 0;
    dsm_modem_stats_info->stats.dup.min_free_cnt_cb_based = DSM_MODEM_STATS_DEFAULT_RET_VALUE;
    dsm_modem_stats_info->stats.dup.total_item_cnt_at_min_free_level_cb_based = DSM_MODEM_STATS_DEFAULT_RET_VALUE;

     if (dsm_dup_item_pool.pool_stats)      
     {     
       dsm_modem_stats_info->stats.dup.item_alloc_fail_cnt =
                                 dsm_dup_item_pool.pool_stats->stats_num_out_of_memory;
#ifdef FEATURE_DSM_AUGMENT  
       dsm_modem_stats_info->stats.dup.heap_alloc_fail_cnt =
                       dsm_dup_item_pool.pool_stats->stats_ext_mem_alloc_fail_count;
       dsm_modem_stats_info->stats.dup.min_free_cnt_cb_based =
                        dsm_dup_item_pool.pool_stats->stats_free_count_at_lowest_cb_level;
       dsm_modem_stats_info->stats.dup.total_item_cnt_at_min_free_level_cb_based =
                        dsm_dup_item_pool.pool_stats->stats_item_count_at_lowest_cb_level;
#endif
     }
     dsm_modem_stats_info->stats.small.items_not_allocated =
                                      dsm_pool_stats_get_items_not_allocated(&dsm_ds_small_item_pool);
     dsm_modem_stats_info->stats.small.client_alloc_bytes =
                                     dsm_pool_stats_get_client_alloc_bytes(&dsm_ds_small_item_pool);
     dsm_modem_stats_info->stats.small.heap_alloc_bytes =
                                     dsm_pool_stats_get_ext_mem_alloc_bytes(&dsm_ds_small_item_pool);
     dsm_modem_stats_info->stats.small.max_heap_alloc_bytes =
                                     dsm_pool_stats_get_ext_mem_max_alloc_bytes(&dsm_ds_small_item_pool);
     dsm_modem_stats_info->stats.small.available_free_cnt = DSM_POOL_FREE_CNT(&dsm_ds_small_item_pool);
     
     dsm_modem_stats_info->stats.small.item_alloc_fail_cnt = 0;
     dsm_modem_stats_info->stats.small.heap_alloc_fail_cnt = 0;
     dsm_modem_stats_info->stats.small.min_free_cnt_cb_based = DSM_MODEM_STATS_DEFAULT_RET_VALUE;
     dsm_modem_stats_info->stats.small.total_item_cnt_at_min_free_level_cb_based = DSM_MODEM_STATS_DEFAULT_RET_VALUE;
     
      if (dsm_ds_small_item_pool.pool_stats)      
      {     
        dsm_modem_stats_info->stats.small.item_alloc_fail_cnt =
                                      dsm_ds_small_item_pool.pool_stats->stats_num_out_of_memory;
#ifdef FEATURE_DSM_AUGMENT  
        dsm_modem_stats_info->stats.small.heap_alloc_fail_cnt =
                            dsm_ds_small_item_pool.pool_stats->stats_ext_mem_alloc_fail_count;
        dsm_modem_stats_info->stats.small.min_free_cnt_cb_based =
                             dsm_ds_small_item_pool.pool_stats->stats_free_count_at_lowest_cb_level;
        dsm_modem_stats_info->stats.small.total_item_cnt_at_min_free_level_cb_based =
                             dsm_ds_small_item_pool.pool_stats->stats_item_count_at_lowest_cb_level;
#endif
      }
     

    dsm_modem_stats_info->stats.large.items_not_allocated =
                                 dsm_pool_stats_get_items_not_allocated(&dsm_ds_large_item_pool);
    dsm_modem_stats_info->stats.large.client_alloc_bytes =
                                dsm_pool_stats_get_client_alloc_bytes(&dsm_ds_large_item_pool);
    dsm_modem_stats_info->stats.large.heap_alloc_bytes =
                                dsm_pool_stats_get_ext_mem_alloc_bytes(&dsm_ds_large_item_pool);
    dsm_modem_stats_info->stats.large.max_heap_alloc_bytes =
                                dsm_pool_stats_get_ext_mem_max_alloc_bytes(&dsm_ds_large_item_pool);
    dsm_modem_stats_info->stats.large.available_free_cnt = DSM_POOL_FREE_CNT(&dsm_ds_large_item_pool);

    dsm_modem_stats_info->stats.large.item_alloc_fail_cnt = 0;
    dsm_modem_stats_info->stats.large.heap_alloc_fail_cnt = 0;
    dsm_modem_stats_info->stats.large.min_free_cnt_cb_based = DSM_MODEM_STATS_DEFAULT_RET_VALUE;
    dsm_modem_stats_info->stats.large.total_item_cnt_at_min_free_level_cb_based = DSM_MODEM_STATS_DEFAULT_RET_VALUE;

     if (dsm_ds_large_item_pool.pool_stats)      
     {     
       dsm_modem_stats_info->stats.large.item_alloc_fail_cnt =
                        dsm_ds_large_item_pool.pool_stats->stats_num_out_of_memory;
#ifdef FEATURE_DSM_AUGMENT  
       dsm_modem_stats_info->stats.large.heap_alloc_fail_cnt =
                       dsm_ds_large_item_pool.pool_stats->stats_ext_mem_alloc_fail_count;
       dsm_modem_stats_info->stats.large.min_free_cnt_cb_based =
                        dsm_ds_large_item_pool.pool_stats->stats_free_count_at_lowest_cb_level;
       dsm_modem_stats_info->stats.large.total_item_cnt_at_min_free_level_cb_based =
                        dsm_ds_large_item_pool.pool_stats->stats_item_count_at_lowest_cb_level;
#endif
     }
}

/*===========================================================================
FUNCTION DSMI_GET_WM_STATS

DESCRIPTION
   This function returns the min_cnt, max_cnt, HI, LO, DNE counts of a
   watermark queue. 
 
DEPENDENCIES
  If FEATURE_DSM_WM_TRACING is not defined, this function will 
  ERR_FATAL().

PARAMETERS
   wm_ptr - Pointer to the watermark queue.
   wm_cnts - Pointer to the memory location where this function will write
     the min_cnt, max_cnt, HI, LO, DNE counts.

RETURN VALUE
 None

SIDE EFFECTS
   None
===========================================================================*/
void dsmi_get_wm_stats
(
  dsm_watermark_type*     wm_ptr,
  dsm_wm_counts_type*     wm_cnts
)
{
  /* Check if the wm_cnts pointer is NULL */
  ASSERT(wm_cnts != NULL);
  ASSERT(wm_ptr  != NULL);

  DSMI_QUEUE_LOCK_WM(wm_ptr);

  /* Fill the watermark count structure with all the values */
  wm_cnts->current_cnt          =  wm_ptr->current_cnt;
  wm_cnts->highest_cnt          =  wm_ptr->highest_cnt;
  wm_cnts->total_rcvd_cnt       =  wm_ptr->total_rcvd_cnt;

  /* Check if the queue is empty */
  if(wm_ptr->q_ptr != NULL)
  {
    wm_cnts->q_cnt              =  wm_ptr->q_ptr->cnt;
  }
  else 
  {
    wm_cnts->q_cnt              =  0;
  }
  wm_cnts->dne_discard_pkt_cnt   =  wm_ptr->dne_discard_pkt_cnt;
  wm_cnts->dne_q_cnt_discard_pkt_cnt   =  wm_ptr->dne_q_cnt_discard_pkt_cnt;
  wm_cnts->dne_discard_bytes_cnt   =  wm_ptr->dne_discard_bytes_cnt;
  wm_cnts->dne_q_cnt_discard_bytes_cnt   =  wm_ptr->dne_q_cnt_discard_bytes_cnt;
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
} /* dsm_get_wm_stats */

/*===========================================================================
FUNCTION DSM_STATS_REGISTER_POOL

DESCRIPTION
  Register a pool for statistics
  
DEPENDENCIES
  None.
  
RETURN VALUE
  Boolean - Indicates whether the pool is registered for statistics

SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
===========================================================================*/
boolean dsmi_stats_register_pool
(
  dsm_mempool_id_type pool_id
)
{
   dsm_pool_mgmt_table_type *table; /* Temp table pointer */
   table = (dsm_pool_mgmt_table_type *)pool_id;

   if (dsm_stats_num_pools>= DSMI_STAT_POOLS_CNT)
   {
     return FALSE;
   }
   table->pool_stats = &dsm_pool_stats_array[dsm_stats_num_pools];
   dsm_stats_num_pools++;
   
   table->pool_stats->stats_num_out_of_memory = 0;
#ifdef FEATURE_DSM_AUGMENT   
   table->pool_stats->stats_max_item_count = table->item_count;  
   table->pool_stats->stats_item_count_at_out_of_memory =  0; 
   table->pool_stats->stats_free_count_at_lowest_cb_level = table->item_count;
   table->pool_stats->stats_item_count_at_lowest_cb_level = table->item_count;
   table->pool_stats->stats_ext_mem_alloc_fail_count = 0;
#endif
   return TRUE;   
}


