/*============================================================================

  @file
  sns_log_api_dsps_uimg.c

  @brief
  Contains implementation of Sensors Logging API's on the DSPS processor.

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


============================================================================*/
/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-07-13 hw   Set a max_size of log packet hold_queue
  2015-03-23 TC   Temporarily replaced the binary search method in
                  sns_logpkt_submit with a linear search method. The binary
                  search method did not work because sns_log_pkt_id_trans_tab
                  is not in ascending order.
  2015-01-16 VY   Moved uImage logging to debug module
  2014-12-10 MW   Eliminated compiler warning during HD22 generation
==============================================================================*/
/*=====================================================================
                 INCLUDE FILES
=======================================================================*/
#include "sns_debug_str.h"
#include "sns_log_api.h"
#include "sns_log_types.h"
#include "sns_memmgr.h"
#include "sns_rh_main.h"

#define NUM_MASK_BITS (sizeof(sns_log_mask_t) * 8)

#define NUM_MASK_ARRAY_SIZE (SNS_LOG_NUM_IDS/NUM_MASK_BITS+1)
#define NUM_MASK_ARRAY_IDX(x) (x/(sizeof(sns_log_mask_t) * 8))
/* log mask to filter log packets on DSPS */
//all logs disabled
static sns_log_mask_t sns_diag_dsps_log_mask     = 0xffffffffffffffffULL;
static sns_log_mask_t sns_diag_dsps_log_mask_ext = 0xffffffffffffffffULL;
/* log mask to filter log packets on DSPS */
sns_log_mask_t sns_diag_dsps_log_mask_arry[NUM_MASK_ARRAY_SIZE];
sns_log_mask_t * sns_diag_dsps_log_mask_arry_ptr = sns_diag_dsps_log_mask_arry;
static const uint32_t sns_log_pkt_id_trans_tab[] = SNS_LOG_QXDM_ID

/* to enable disable logging via diag input                            */
/* 0 means disabled, 1 menas enabled, it does not take any other value */
static uint32_t sns_log_disabled = 0;

#ifdef UIMG_LOG
sns_pm_handle_t        sns_debug_pm_client_handle;
logpkt_hold_buffer_s   sns_debug_logpkt_buff;
#define SNS_LOG_PKT_HOLD_QUEUE_MAX_SIZE 1000
#endif /* UIMG_LOG */

/*===========================================================================
                    STATIC FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION:   sns_log_malloc
===========================================================================*/
/*!
  @brief
  Allocates memory for the log packet.

  @param[i] pkt_size : Size of the log packet to allocate

  @return Log buffer, or NULL if not enough memory is available.
*/
/*=========================================================================*/
STATIC void *sns_log_malloc(uint32_t pkt_size)
{
  void *mod_log_pkt = NULL;

  /* Allocate extra memory in addition to what is asked by the caller to save pkt_size */
  mod_log_pkt = SNS_OS_ANY_MALLOC( SNS_DBG_MOD_DSPS_DIAG, pkt_size + sizeof(pkt_size) );
  if ( mod_log_pkt == NULL )
  {
    return NULL;
  }

  /* Save pkt_size at the top of the allocated memory, consuming the extra allocation */
  *(uint32_t *)mod_log_pkt = pkt_size;

  /* Provide the starting address of the remaining memory to the caller */
  /* The pkt_size can be obtained by the caller during the sns_log_commit by looking
     at the word just before the returned address */
  return  (uint8_t *)mod_log_pkt + sizeof(pkt_size);
}

/*===========================================================================
  FUNCTION:   sns_log_commit
===========================================================================*/
/*!
  @brief
  Commits the log packet to DIAG.

  @param[i] log_pkt_id    : Log Packet ID
  @param[i] log_pkt_size  : sizeof the log packet
  @param[i] log_pkt       : Log packet to commit

  @return
  sns_err_code_e: SNS_SUCCESS if the commit had no errors.
                  SNS_ERR_FAILED The commit process failed
*/
/*=========================================================================*/
STATIC sns_err_code_e sns_log_commit(uint32_t log_pkt_id,
                                     uint32_t log_pkt_size,
                                     void     *log_pkt)
{
#ifdef UIMG_LOG
  if( sns_debug_logpkt_commit_cntl( log_pkt, log_pkt_id, log_pkt_size ) != SNS_SUCCESS )
  {
    SNS_OS_ANY_FREE( (uint8_t *)log_pkt - sizeof(log_pkt_size) );
    return SNS_ERR_FAILED;
  }
  return SNS_SUCCESS;
#else
  sns_err_code_e status;
  log_set_length( log_pkt, log_pkt_size );
  log_set_code(   log_pkt, log_pkt_id );
  log_set_timestamp( log_pkt );
  status = log_submit( log_pkt ) ? SNS_SUCCESS : SNS_ERR_FAILED;
  SNS_OS_ANY_FREE( (uint8_t*)log_pkt - sizeof(log_pkt_size) );
  return status;
#endif /* UIMG_LOG */
}

/*===========================================================================
                    PUBLIC FUNCTIONS
===========================================================================*/

sns_err_code_e sns_logpkt_malloc(log_pkt_t log_pkt_type,
                                 uint32_t pkt_size,
                                 void** log_pkt_ptr)
{
  uint32_t  pkt_idx;
  log_pkt_t max_log_id;

  if( sns_log_disabled == 1 )
  {
    return SNS_ERR_NOTALLOWED;
  }

#ifdef SENSORS_DD_DEV_FLAG
  max_log_id = SNS_LOG_NUM_IDS_MAX;
#else
  max_log_id = SNS_LOG_NUM_IDS;
#endif

#ifndef UIMG_LOG
  if( sns_ddf_smgr_is_in_uimage() )
  {
    return SNS_ERR_NOTALLOWED;
  }
#endif /* UIMG_LOG */

  if ((pkt_size == 0) || (log_pkt_ptr == NULL) ||
      (log_pkt_type >= max_log_id))
  {
    return SNS_ERR_BAD_PARM;
  }
  *log_pkt_ptr = NULL;
  
  /* Filter log packets based on log mask */
  if ( (log_pkt_type < NUM_MASK_BITS) &&
       (((sns_log_mask_t)1 << log_pkt_type) & sns_diag_dsps_log_mask) )
  {
    return SNS_ERR_NOTALLOWED;
  }
  if ( (log_pkt_type >= NUM_MASK_BITS) &&
       (((sns_log_mask_t)1 << (log_pkt_type-NUM_MASK_BITS)) & sns_diag_dsps_log_mask_ext) )
  {
    return SNS_ERR_NOTALLOWED;
  }

  pkt_idx = (uint32_t)log_pkt_type;
  if ( pkt_idx >= ARR_SIZE(sns_log_pkt_id_trans_tab) )
  {
    return SNS_ERR_BAD_PARM;
  }

  *log_pkt_ptr = sns_log_malloc( pkt_size );

  return ( NULL == *log_pkt_ptr ) ? SNS_ERR_NOMEM : SNS_SUCCESS;
} // end of function sns_logpkt_malloc

void sns_diag_dsps_set_log_mask(sns_diag_set_log_mask_req_msg_v01* msg_ptr)
{
   if (msg_ptr != NULL)
   {
      sns_diag_dsps_log_mask = msg_ptr->log_mask.mask;
      if (msg_ptr->log_mask_ext_valid)
      {
         sns_diag_dsps_log_mask_ext = msg_ptr->log_mask_ext.mask;
      }
   }
}

sns_err_code_e sns_logpkt_commit(log_pkt_t log_pkt_idx,
                                 void      *log_pkt)
{
  uint32_t log_pkt_id = sns_log_pkt_id_trans_tab[ (uint32_t)log_pkt_idx ];
  /*Note: pkt_size must have same data type as used in sns_logpkt_malloc */
  uint32_t log_pkt_size;

  if( sns_log_disabled == 1 )
  {
    return SNS_ERR_NOTALLOWED;
  }
  
  if ( NULL == log_pkt )
  {
    return SNS_ERR_BAD_PARM;
  }

  /*Retrieve the pkt_size info stashed during sns_logpkt_malloc */
  log_pkt_size = *(uint32_t *)((uint8_t *)log_pkt - sizeof(log_pkt_size));

  return sns_log_commit( log_pkt_id, log_pkt_size, log_pkt );
}

sns_err_code_e sns_logpkt_submit(uint32_t     log_pkt_id,
                                 uint32_t     log_pkt_size,
                                 PACK(void*)  log_pkt)
{
  log_pkt_t log_pkt_type = SNS_LOG_NUM_IDS;
  void *log_pkt_copy = NULL;
  uint32_t low;
  
  if( sns_log_disabled == 1 )
  {
    return SNS_ERR_NOTALLOWED;
  }

// TODO: RE-ORDER THE LOG PACKETS IN sns_log_pkt_id_trans_tab IN ASCENDING ORDER
//       SUCH THAT WE CAN RE-IMPLEMENT THE BINARY SEARCHING BELOW
//  /* Filter log packets based on log mask */
//  low = 0;
//  high = SNS_LOG_NUM_IDS - 1;
//  SNS_SMGR_PRINTF3(HIGH, "sns_logpkt_submit low %d high %d log_pkt_id %d", low, high, log_pkt_id);
//  while( low <= high )
//  {
//    mid = (low + high) / 2;
//    SNS_SMGR_PRINTF1(HIGH, "sns_logpkt_submit mid %d", mid);
//    if( sns_log_pkt_id_trans_tab[mid] < log_pkt_id )
//    {
//      low = mid + 1;
//      SNS_SMGR_PRINTF2(HIGH, "sns_logpkt_submit new low %d  %d", low, sns_log_pkt_id_trans_tab[mid]);
//    }
//    else if( sns_log_pkt_id_trans_tab[mid] > log_pkt_id )
//    {
//      high = mid - 1;
//      SNS_SMGR_PRINTF2(HIGH, "sns_logpkt_submit new high %d  %d", high, sns_log_pkt_id_trans_tab[mid]);
//    }
//    else
//    {
//      log_pkt_type = mid;
//      SNS_SMGR_PRINTF1(HIGH, "sns_logpkt_submit found it mid %d", mid);
//      break;
//    }
//  }

  // TODO: REPLACE THIS LINEAR SEARCH METHOD WITH THE BINARY SEARCH METHOD (SEE ABOVE)
  for( low = 0; low < ARR_SIZE(sns_log_pkt_id_trans_tab); low++ )
  {
    if( sns_log_pkt_id_trans_tab[low] == log_pkt_id )
    {
      log_pkt_type = low;
      break;
    }
  }

  if (log_pkt_type == SNS_LOG_NUM_IDS)
  {
    return SNS_ERR_NOTALLOWED;
  }
  if ( (log_pkt_type < NUM_MASK_BITS) &&
       (((sns_log_mask_t)1 << log_pkt_type) & sns_diag_dsps_log_mask) )
  {
    return SNS_ERR_NOTALLOWED;
  }
  if ( (log_pkt_type >= NUM_MASK_BITS) &&
       (((sns_log_mask_t)1 << (log_pkt_type-NUM_MASK_BITS)) & sns_diag_dsps_log_mask_ext) )
  {
    return SNS_ERR_NOTALLOWED;
  }
  
  log_pkt_copy = sns_log_malloc( log_pkt_size );
  if( NULL == log_pkt_copy )
  {
    return SNS_ERR_NOMEM;
  }
  else
  {
    SNS_OS_MEMCOPY( log_pkt_copy, log_pkt, log_pkt_size );
    return sns_log_commit( log_pkt_id, log_pkt_size, log_pkt_copy );
  }
}

void sns_logging_status_override(bool log_override)
{
  sns_log_disabled = log_override ? 1 : 0;
}


#ifdef UIMG_LOG
/*===========================================================================

  FUNCTION:   sns_debug_logpkt_commit_cntl

===========================================================================*/
/*!
  @brief Commit log packet in a hold queue for tranmission in bigImage mode

  @detail

  @param   Ptr to log packet, packet type, and packet size
  @return  return SNS_SUCCESS if successfully added packet to hold queue,
           else returns error code
 */
/*=========================================================================*/
sns_err_code_e sns_debug_logpkt_commit_cntl(void *qmi_log_pkt_msg,
                                         uint32_t pkt_type,
                                         uint32_t pkt_size)
{
  uint8_t              os_err = OS_ERR_NONE;
  logpkt_hold_item_s  *logpkt_msg_q_ptr;

  if(sns_debug_logpkt_buff.hold_queue.cnt > SNS_LOG_PKT_HOLD_QUEUE_MAX_SIZE)
  {
     return SNS_ERR_FAILED;
  }

  logpkt_msg_q_ptr = SNS_OS_ANY_MALLOC(0, sizeof(logpkt_hold_item_s));

  if (logpkt_msg_q_ptr != NULL)
  {
    sns_q_link(logpkt_msg_q_ptr, &logpkt_msg_q_ptr->link);
    logpkt_msg_q_ptr->log_pkt_msg = qmi_log_pkt_msg;
    logpkt_msg_q_ptr->pkt_type = pkt_type;
    logpkt_msg_q_ptr->pkt_size = pkt_size;
    sns_os_mutex_pend(sns_debug_logpkt_buff.q_mutex_ptr, 0, &os_err );
    if (os_err != OS_ERR_NONE)
    {
      return SNS_ERR_FAILED;
    }
    sns_q_put(&sns_debug_logpkt_buff.hold_queue, &logpkt_msg_q_ptr->link);
    sns_os_mutex_post(sns_debug_logpkt_buff.q_mutex_ptr);
    if (!sns_ddf_smgr_is_in_uimage())
    {
      sns_debug_signal_me(SNS_DEBUG_LOGPKT_COMMIT_SIG);
    }
    return SNS_SUCCESS;
  }
  else
  {
    SNS_OS_ANY_FREE((uint8_t *)qmi_log_pkt_msg - sizeof(uint32_t));
    return SNS_ERR_NOMEM;
  }
}


/*===========================================================================

  FUNCTION:   sns_debug_logpkt_drop

===========================================================================*/
/*!
  @details To be called inside debug task loop to drop logpkts in bigImg mode.

  @param[i]  none
  @return  none
 */
/*=========================================================================*/
void sns_debug_logpkt_drop(void)
{
  uint8_t             os_err = OS_ERR_NONE;
  logpkt_hold_item_s *logpkt_msg_ptr = NULL;
  void               *log_pkt_ptr = NULL;

  SNS_SMGR_PRINTF1(HIGH, "Low memory - Dropping %d logpkts", sns_debug_logpkt_buff.hold_queue.cnt);

  while (sns_q_cnt(&sns_debug_logpkt_buff.hold_queue) > 0)
  {
    sns_os_mutex_pend(sns_debug_logpkt_buff.q_mutex_ptr, 0, &os_err);
    SNS_ASSERT(os_err == OS_ERR_NONE);
    logpkt_msg_ptr = (logpkt_hold_item_s *)sns_q_get(&sns_debug_logpkt_buff.hold_queue);
    sns_os_mutex_post(sns_debug_logpkt_buff.q_mutex_ptr);
    SNS_ASSERT(os_err == OS_ERR_NONE);

    if (logpkt_msg_ptr != NULL)
    {
      log_pkt_ptr = logpkt_msg_ptr->log_pkt_msg;
      SNS_OS_ANY_FREE((uint8_t *)log_pkt_ptr - sizeof(logpkt_msg_ptr->pkt_size));
      SNS_OS_ANY_FREE(logpkt_msg_ptr);
    }
    else
      break;
  }
  sns_debug_unsignal_me(SNS_DEBUG_LOGPKT_DROP_SIG);
}


/*===========================================================================

  FUNCTION:   sns_debug_logpkt_commit_send

===========================================================================*/
/*!
  @details To be called inside debug task loop to send out logpkts in bigImg mode.

  @param[i]  none
  @return  none
 */
/*=========================================================================*/
void sns_debug_logpkt_commit_send(void)
{
  uint8_t             os_err = OS_ERR_NONE;
  logpkt_hold_item_s *logpkt_msg_ptr = NULL;
  uint8_t             ret_val = 0;
  void               *log_pkt_ptr = NULL;

  while (sns_q_cnt(&sns_debug_logpkt_buff.hold_queue) > 0)
  {
    sns_os_mutex_pend(sns_debug_logpkt_buff.q_mutex_ptr, 0, &os_err);
    logpkt_msg_ptr = (logpkt_hold_item_s *)sns_q_get(&sns_debug_logpkt_buff.hold_queue);
    sns_os_mutex_post(sns_debug_logpkt_buff.q_mutex_ptr);

    if (logpkt_msg_ptr != NULL)
    {
      log_pkt_ptr = logpkt_msg_ptr->log_pkt_msg;
      log_set_length(log_pkt_ptr, logpkt_msg_ptr->pkt_size);
      log_set_code(log_pkt_ptr, logpkt_msg_ptr->pkt_type);
      log_set_timestamp(log_pkt_ptr);
      ret_val = log_submit(log_pkt_ptr);
      if (ret_val == 0)
        SNS_SMGR_PRINTF0(MED, "Fail to submit logpkt");
      /* Need to free memory including the "extra" allocation done during sns_logpkt_malloc */
      SNS_OS_ANY_FREE((uint8_t *)log_pkt_ptr - sizeof(logpkt_msg_ptr->pkt_size));
      SNS_OS_ANY_FREE(logpkt_msg_ptr);
    }
    else
      break;
  }
  sns_debug_unsignal_me(SNS_DEBUG_LOGPKT_COMMIT_SIG);
}
#endif /* UIMG_LOG */
