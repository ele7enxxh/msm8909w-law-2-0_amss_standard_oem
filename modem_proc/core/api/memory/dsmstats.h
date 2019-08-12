#ifndef DSMSTATS_H 
#define DSMSTATS_H

/*===========================================================================

                                  D S M S T A T S . H

DESCRIPTION
  This file contains types and declarations associated with DSM Statistics

Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.mpss/3.5.c12.3/api/memory/dsmstats.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/14    sh     Implemented statistics required by A2 team CR 749150 , specific to Modem 
===========================================================================*/

/* Default return value of structure fields in case DSM doesn't have the info */
#define DSM_MODEM_STATS_DEFAULT_RET_VALUE 0xFFFFFFFF


typedef struct
{
  /* Whether DSM memory manager is initialized.
  Can be interpreted in segment loading configuration
  whether device is currently in FTM mode. 
  FALSE=FTM mode (dsm not initialized for data transfer)
  TRUE = not in FTM mode */
  boolean                 is_initialized;
  
  /* max heap memory that can be allocated across all DSM pools */
  uint32                  max_heap_alloc_bytes;  
} dsm_modem_config_info_common_s;


typedef struct
{
  /* max heap that can be allocated for this DSM pool */
  uint32  max_heap_alloc_bytes;
  
  /* free item count threshold to start allocating from heap */
  uint32  free_item_cnt_heap_trigger;
  
  /* Number of items added per heap allocation */
  uint32  num_items_per_heap_alloc;

  /* maximum DSM item count for this pool (static + dynamic)*/ 
  uint32  max_item_cnt;

  /* augment buffer recycle minimum count */
  uint32  augment_buffer_recycle_minimum_count;

} dsm_modem_config_info_per_pool_s;

typedef struct
{
  /* config common for all pools */
  dsm_modem_config_info_common_s      common;
  
  /* config specific to pools */
  dsm_modem_config_info_per_pool_s    large_pool;
  dsm_modem_config_info_per_pool_s    small_pool;
  dsm_modem_config_info_per_pool_s    dup_pool;
} dsm_modem_config_info_s;


typedef struct
{
  /* num dsm allocations failed due to out of memory condition */
  uint32  item_alloc_fail_cnt;
  
  /* num heap allocations failed */
  uint32  heap_alloc_fail_cnt;
  
   /* heap size in bytes allocated for this pool*/
  uint32  heap_alloc_bytes;

  /* bytes allocated by clients */
  /* NOTE: this will only be an approximate number */
  uint32  client_alloc_bytes;
  
  /* max heap bytes allocated by clients */
  /* NOTE: this will be an approximate number */
  uint32  max_heap_alloc_bytes;

  /* available free count visible to external clients for this pool */
  uint32 available_free_cnt; 

  /* total item count - currently allocated items */
  /* NOTE: this will be an approximate number */
  uint32  items_not_allocated; 

   /* minimum free level reached for this pool
  NOTE: call back triggers can be used to set this stat in order to avoid checks 
  for all allocations */
  uint32 min_free_cnt_cb_based;

  /* item cnt when min_free_level_cb_based has reached */
  uint32 total_item_cnt_at_min_free_level_cb_based;
  
} dsm_modem_stats_per_pool_s;

/*!
   @brief
   Top level DSM stats structure */
typedef struct
{ 
  /* status specific to pools */
  dsm_modem_stats_per_pool_s     large;
  dsm_modem_stats_per_pool_s     small;
  dsm_modem_stats_per_pool_s     dup;
} dsm_modem_stats_s;

/*!
   @brief
   Top level DSM Pool Statistics info structure  for Modem */
typedef struct
{
  dsm_modem_config_info_s config;
  dsm_modem_stats_s  stats;
} dsm_modem_stats_info_s;




/*---------------------------------------------------------------------------
   Watermark counts structure which will be used to store all the count
   values for watermark tracing. This structure will be populated each
   time a caller calls dsm_wm_get_all_queue_cnts() for a particular
   watermark. The caller is expected to allocate memory for holding
   this structure.
---------------------------------------------------------------------------*/

typedef struct dsm_wm_counts_s
{
  uint32  current_cnt;                 /* Current total byte count      */
  uint32  highest_cnt;                 /* largest count since powerupp  */
  uint64  total_rcvd_cnt;            /* Total RX bytes thru watermark */
  int     q_cnt;                                 /* Count of packets in the queue */

  /* Statistics */
  uint32  dne_discard_pkt_cnt;               /* Total pkts discarded when dne cnt is hit */
  uint32  dne_q_cnt_discard_pkt_cnt; /* Total pkts discarded when dne_q_cnt is hit */
  uint32  dne_discard_bytes_cnt;          /* Total byte cnt in dne_discard_pkt(s) */
  uint32  dne_q_cnt_discard_bytes_cnt;/* Total byte cnt when dne_q_cnt is hit   */
  
} dsm_wm_counts_type;


/********************************************************************
*        D S M     P O O L         S T A T I S T I C    F U N C T I O N S                   *                                        *
*                                                                                                              *
* IMPORTANT -                                                                                          *
* Statistics are approximate as requested by CR 749150 and should not be used *
* logic based decisions. Please use dsm_pool_cb* functions for data consistency.*
* This function does not guarantee data consistency.                                       *
********************************************************************/
/*===========================================================================

FUNCTION DSM_MODEM GET_STATISTICS

DESCRIPTION
  Return statistics requested by Modem team - CR 749150
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
===========================================================================*/
#define dsm_modem_get_statistics(dsm_modem_stats_info) \
      dsmi_modem_get_statistics(dsm_modem_stats_info)
void dsmi_modem_get_statistics( dsm_modem_stats_info_s  *dsm_modem_stats_info);

/*===========================================================================
FUNCTION DSM_GET_WM_STATS()

DESCRIPTION
   This function returns the min_cnt, max_cnt, HI, LO, DNE counts of a
   watermark queue. 
 
DEPENDENCIES

PARAMETERS
   wm_ptr - Pointer to the watermark queue.
   wm_cnts - Pointer to the memory location where this function will write
     the min_cnt, max_cnt, HI, LO, DNE counts.

RETURN VALUE
  None.
SIDE EFFECTS
   None
===========================================================================*/
#define dsm_get_wm_stats(wm_ptr,wm_cnts) \
          dsmi_get_wm_stats(wm_ptr,wm_cnts)
void dsmi_get_wm_stats
(
  dsm_watermark_type*     wm_ptr,
  dsm_wm_counts_type*     wm_cnts
);


/*===========================================================================
FUNCTION DSM_STAT_REGISTER_POOL

DESCRIPTION
  Register a pool for statistics
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE - Pool registered for updating statistics
  FALSE - Pool could not be registered due to memory limitation

SIDE EFFECTS
  Statistics are approximate as requested by CR 749150 and should not be used for
  logic based decisions. Please use dsm_pool_cb* functions for data consistency.
  This function does not guarantee data consistency.
===========================================================================*/
#define dsm_stats_register_pool(pool_id) \
      dsmi_stats_register_pool(pool_id)
boolean dsmi_stats_register_pool
(
  dsm_mempool_id_type pool_id
);


#endif /* DSMSTATS_H */
