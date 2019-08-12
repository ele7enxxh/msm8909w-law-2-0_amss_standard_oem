/*============================================================================
  @file sns_memmgr.c

  @brief
  Implements SNS memory manager interfaces.

  <br><br>

  DEPENDENCIES:  Uses BM3 memory manager.

Copyright (c) 2010,2012-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/memmgr/src/sns_memmgr.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who    what, where, why
  ---------- --- -----------------------------------------------------------
  2015-08-13 bd  increase sensors heap by 512kB, and remove lowmem cb for sensors heap
  2015-01-12 ps  Merge updates for Memmgr
  2014-10-27 hw  Divide BigImage heap to two parts: default heap and memheap. Each
                 will have size of 512K. Also add lowmem callback function support
  2014-09-14 sc  Increased heap size to 1MB
  2014-08-15 asj Added spillover buffer tracking
  2013-08-22 pk  Increased SNS heap size for Hammerhead
  2013-06-12 agk  Do not use OI_Memmgr for adsp (Q6) baselines.
  2010-11-03 jtl  Implementing USE_NATIVE_MALLOC macro handling.
  2010-08-26 jtl  Initial version

  ============================================================================*/
/*=====================================================================
  INCLUDES
=======================================================================*/

#include "sns_common.h"
#include "sns_memmgr.h"
#include "sns_init.h"
#include "sns_queue.h"

#include "oi_status.h"
#include "oi_support_init.h"
#include "sns_uimg_utils.h"

/*============================================================================
  Global Data Definitions
  ===========================================================================*/

/** Queue of memory blocks that need to be freed before entering uImage */
sns_q_s sns_memmgr_spillover_q;
OS_EVENT *sns_memmgr_spillover_q_mutex;
static OS_EVENT *sns_memmgr_lowmem_cb_mutex;

/** An entry in the queue of memory blocks that need to be freed before entering
*  uImage
*/
typedef struct
{
  sns_q_link_s q_link;
  intptr_t  memPtr;
} sns_memmgr_spillover_q_item;

#if defined(QDSP6)
/* State of the heaps. */
mem_heap_type     sns_sensors_heap_state;
mem_heap_totals_type sns_sensors_heap_total;
#endif

/* Sensors heap of SNS_SENSORS_HEAP_SIZE bytes. Sensors' sam IO memory allocation are done here */
static char sns_sensors_heap[SNS_SENSORS_HEAP_SIZE] __attribute__((__section__(".bss.sns_sensors_heap")));

/*============================================================================
  Static function definitions
  ===========================================================================*/

/*===========================================================================

  FUNCTION:   sns_uheap_init
    		  Initializes the heap used for uImage dynamic memory operations

  ===========================================================================*/
void sns_uheap_init(void)
{
#ifdef SNS_USES_ISLAND
  int ret_val;

  sns_uheap_magic.magic_num_index_array = sns_uheap_magic_num_idx_array;
  sns_uheap_magic.magic_num             = sns_uheap_magic_num;
  sns_uheap_magic.magic_num_index       = 0;

  ret_val =  sns_mem_init_heap( &sns_uheap,
							&sns_uheap_magic,
							sns_uheap_mem,
							sizeof(sns_uheap_mem));

  MSG_3(MSG_SSID_SNS, DBG_MED_PRIO,
			  "Initialize micro Heap of size %d bytes @ %lu ret_val %de",
			  sizeof(sns_uheap_mem), &sns_uheap, ret_val);
  /* Note that the failure of uheap allocation is not catastrophic.
     The system should still continue to run using the regular heap memory
     This function thus does not return an error code.
  */
#endif /* SNS_USES_ISLAND */
}

/*===========================================================================

  FUNCTION:   sns_memmgr_init
  - Documented in sns_memmgr.h
  ===========================================================================*/
void sns_uheap_destroy(void)
{
#ifdef SNS_USES_ISLAND
  int ret_val;
  ret_val = sns_mem_deinit_heap(&sns_uheap, &sns_uheap_magic);
  MSG_2(MSG_SSID_SNS, DBG_MED_PRIO,
			  "Destroyed micro Heap  @ %lu ret_val %de",
			  &sns_uheap, ret_val);
#endif /* SNS_USES_ISLAND */
}

/*=====================================================================
  EXTERNAL FUNCTIONS
=======================================================================*/

/*===========================================================================

  FUNCTION:   sns_heap_init
  - Documented in sns_memmgr.h
  ===========================================================================*/
sns_err_code_e sns_heap_init(void)
{
   sns_err_code_e result;
   uint8_t sns_os_err, i;

   /* Create default sensors heap */
   result = sns_heap_create();
   if ( result != SNS_SUCCESS ) {
      MSG_1(MSG_SSID_SNS, DBG_ERROR_PRIO,
               "Failed to initialize default heap!! Error %d\n", result);
      return SNS_ERR_FAILED;
   }

#if defined(QDSP6)
   /* Initialize the heap using memheap for Sensors' sam IO memory allocation
    * Memheap can handle any alignment of heap_start_ptr
    * heap_size need not be a multiple of any power of 2.
    */
   mem_init_heap(&sns_sensors_heap_state,
                 sns_sensors_heap,
                 sizeof(sns_sensors_heap),
                 NULL);
#endif

   /* Initialize the micro Heap */
   sns_uheap_init();
   sns_q_init(&sns_memmgr_spillover_q);
   sns_memmgr_spillover_q_mutex = sns_os_mutex_create(SNS_MEMMGR_DSPS_MUTEX, &sns_os_err);
   sns_memmgr_lowmem_cb_mutex = sns_os_mutex_create(SNS_MEMMGR_DSPS_MUTEX, &sns_os_err);

   /* Initialize low_mem callback structure for both sensors heap and default heap */
   sns_default_heap_lowmem_cb.num_clients = 0;
   for( i = 0; i < SNS_MAX_NUM_MEMMGR_CLIENTS; i++ )
   {
     sns_default_heap_lowmem_cb.cb_func[i] = NULL;
   }

   return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_memmgr_init
  - Documented in sns_memmgr.h
  ===========================================================================*/
sns_err_code_e sns_heap_deinit(void)
{
   sns_uheap_destroy();
   sns_heap_destroy();

   mem_deinit_heap(&sns_sensors_heap_state);

   return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_memmgr_init
  - Documented in sns_memmgr.h
  ===========================================================================*/
sns_err_code_e
sns_memmgr_init( void )
{
#if !defined(USE_NATIVE_MALLOC) && !defined (QDSP6)
  OI_STATUS err;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MEMMGR INIT");

  err = OI_MEMMGR_Init( &oi_default_config_MEMMGR );

  sns_init_done();

  if( OI_OK == err ) {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MEMMGR INIT SUCCESS");
    return SNS_SUCCESS;
  } else {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MEMMGR INIT FAIL");
    return SNS_ERR_FAILED;
  }
#else
  sns_init_done();
  return SNS_SUCCESS;
#endif /* USE_NATIVE_MALLOC */

}

/*===========================================================================

  FUNCTION:   sns_memmgr_deinit
  - Documented in sns_memmgr.h
  ===========================================================================*/
sns_err_code_e
sns_memmgr_deinit( void )
{
#if !defined(USE_NATIVE_MALLOC) && !defined (QDSP6)
  OI_STATUS err;

  err = OI_MEMMGR_DeInit( );
#else
    //todo, find any/all memory used via native calls ?!?
#endif
    return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_memmgr_lowmem_cb_register

  ===========================================================================*/
sns_err_code_e
sns_memmgr_lowmem_cb_register( sns_memmgr_lowmem_cb_t cb )
{
  uint8_t os_err;
  sns_err_code_e err = SNS_ERR_NOTALLOWED;
  sns_os_mutex_pend(sns_memmgr_lowmem_cb_mutex, 0, &os_err);
  if( sns_default_heap_lowmem_cb.num_clients < SNS_MAX_NUM_MEMMGR_CLIENTS )
  {
    sns_default_heap_lowmem_cb.cb_func[sns_default_heap_lowmem_cb.num_clients] = cb;
    sns_default_heap_lowmem_cb.num_clients++;
    err = SNS_SUCCESS;
  }
  sns_os_mutex_post(sns_memmgr_lowmem_cb_mutex);
  return err;
}

#if defined(QDSP6)
/*===========================================================================
  FUNCTION:   sns_memmgr_sensor_heap_usage
  ===========================================================================*/
uint32_t
sns_memmgr_sensor_heap_usage( void )
{
  mem_heap_get_totals(&sns_sensors_heap_state, &sns_sensors_heap_total);
  return (sns_sensors_heap_total.used_bytes + sns_sensors_heap_total.wasted_bytes + sns_sensors_heap_total.header_bytes);
}
#endif

/*===========================================================================
  FUNCTION:   sns_memmgr_default_heap_usage
  ===========================================================================*/
uint32_t
sns_memmgr_default_heap_usage( void )
{
  mem_heap_get_totals(&sns_def_heap_state, &sns_def_heap_total);
  return (sns_def_heap_total.used_bytes + sns_def_heap_total.wasted_bytes + sns_def_heap_total.header_bytes);
}

/*===========================================================================
  FUNCTION:   sns_memmgr_find_spillover_buff
  ===========================================================================*/
static intptr_t sns_memmgr_find_spillover_buff( sns_q_s* memQ, intptr_t memPtr )
{
  sns_memmgr_spillover_q_item * itemPtr;
  itemPtr = (sns_memmgr_spillover_q_item *)sns_q_check(memQ);
  while(itemPtr != NULL)
  {
    if (itemPtr->memPtr == memPtr)
    {
      return ((intptr_t)itemPtr);
    }
    itemPtr = (sns_memmgr_spillover_q_item *)sns_q_next(memQ, &itemPtr->q_link);
  }
  return ((intptr_t)NULL);
}

#if defined(QDSP6)
/*===========================================================================
  FUNCTION:   sns_memmgr_free_spillover_buffer
  ===========================================================================*/
void sns_memmgr_free_spillover_buffer(void * ptr)
{
    uint8_t os_err;
    sns_heap_free(ptr);
    sns_os_mutex_pend(sns_memmgr_spillover_q_mutex, 0, &os_err);
    if(0 < sns_q_cnt(&sns_memmgr_spillover_q))
    {
      sns_memmgr_spillover_q_item *item_ptr = (sns_memmgr_spillover_q_item *)sns_memmgr_find_spillover_buff(
        &sns_memmgr_spillover_q, (intptr_t)ptr);
      if(item_ptr != NULL)
      {
        sns_q_delete(&item_ptr->q_link);
        SNS_OS_FREE(item_ptr);

        if(0 == sns_q_cnt(&sns_memmgr_spillover_q))
        {
          sns_utils_remove_uimage_hold(SNS_UIMG_MEMGR);
        }
      }
    }
    sns_os_mutex_post(sns_memmgr_spillover_q_mutex);
}
#endif

/*===========================================================================
  FUNCTION:   sns_memmgr_free_sensor_heap_spillover_buffer
  ===========================================================================*/
void sns_memmgr_free_sensor_heap_spillover_buffer(void * ptr)
{
    uint8_t os_err;
    mem_free(&sns_sensors_heap_state, ptr);
    sns_os_mutex_pend(sns_memmgr_spillover_q_mutex, 0, &os_err);
    if(0 < sns_q_cnt(&sns_memmgr_spillover_q))
    {
      sns_memmgr_spillover_q_item *item_ptr = (sns_memmgr_spillover_q_item *)sns_memmgr_find_spillover_buff(
        &sns_memmgr_spillover_q, (intptr_t)ptr);
      if(item_ptr != NULL)
      {
        sns_q_delete(&item_ptr->q_link);
        SNS_OS_FREE(item_ptr);

        if(0 == sns_q_cnt(&sns_memmgr_spillover_q))
        {
          sns_utils_remove_uimage_hold(SNS_UIMG_MEMGR);
        }
      }
    }
    sns_os_mutex_post(sns_memmgr_spillover_q_mutex);
}

/*===========================================================================
  FUNCTION:   sns_memmgr_add_to_spillover_list
  ===========================================================================*/
void sns_memmgr_add_to_spillover_list(void *memPtr)
{
    uint8_t os_err;
    sns_memmgr_spillover_q_item *item_ptr = (sns_memmgr_spillover_q_item *)
      SNS_OS_MALLOC(0, sizeof(sns_memmgr_spillover_q_item));
    SNS_ASSERT(NULL != item_ptr);
    item_ptr->memPtr = (intptr_t)memPtr;
    sns_q_link(item_ptr, &item_ptr->q_link);
    sns_os_mutex_pend(sns_memmgr_spillover_q_mutex, 0, &os_err);
    sns_q_put(&sns_memmgr_spillover_q, &item_ptr->q_link);
    sns_utils_place_uimge_hold(SNS_UIMG_MEMGR);
    sns_os_mutex_post(sns_memmgr_spillover_q_mutex);
}
