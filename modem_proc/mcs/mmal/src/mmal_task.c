/*!
  @file mmal_task.c

  @brief
  This file implements the main functionality of the Modem Metrics Adaptation 
  Layer task.

  The functions in this file perform the mmal task functionality at a high
  level.  

  The main-loop of the task is mmal_task_main() .

  @ingroup per_implementation
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_task.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/15/11   rj      Added SMD Implementation for SlaveBridge
05/22/11   sk      Call the mmal_ciq_fend_shutdown on task_deinit
05/03/11   ars     Fix for CR 286318. Moved IQBackend_Initialize to porting 
                   layer
04/26/11   ars     Changed mmal_msg_send to return boolean
04/24/11   ars     Incorporated code review comments
04/20/11   ars     If IQ init does not succeed then dont call IQ
                   Execute.
04/12/11   ars     Moved extern of mmal_tcb from .h to .c
03/24/11   ars     Initial Checkin

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "mcs_variation.h"
#include <mmal_cfg.h>
#include <tmc.h>
#include <task.h>
#include <mutils_circ_q.h>
#include <mmal_util.h>
#include <mmal_msg_i.h>
#include <mmal_task.h>
#include <dsm.h>
#ifdef FEATURE_MMAL_CIQ
#include <mmal_ciq_pl.h>
#include <mmal_ciq_fend.h>
#include <IQPortingCommon.h>
#include <IQPortingBackend.h>
#endif /* FEATURE_MMAL_CIQ */
#include "mmal_msgr.h"


/*=============================================================================

                   INTERNAL DEFINITIONS AND TYPES

=============================================================================*/
/*! @brief MMAL task rex control block
 * On target these declarations reside in service/task/task.c */
extern rex_tcb_type mmal_tcb;
/*! @brief A union of all the messages that can be sent to the mmal task.
 */

/*! @brief A union of all the messages that can be sent to the mmal task. */
typedef union
{
  mmal_msg_hdr_s                        hdr;
  mmal_log_listener_req_s               mmal_log_listener;
#ifdef FEATURE_MMAL_CIQ
  mmal_pl_sio_rx_cb_s                   mmal_pl_sio_rx_cb_info;
  mmal_pl_smd_rx_cb_s                   mmal_pl_smd_rx_cb_info;
  mmal_time_zone_info_s                 mmal_time_zone_info;
#endif /* FEATURE_MMAL_CIQ */
#ifdef FEATURE_WCDMA
  mmal_wrrc_log_msg_cb_s                mmal_wrrc_cb_info;
#endif
#ifdef FEATURE_LTE
  mmal_lrrc_log_msg_cb_s                mmal_lrrc_cb_info;
#endif
} mmal_msg_u;

typedef struct
{
#ifdef FEATURE_MMAL_CIQ
  uint32           ciq_error_cnt;
#endif /* FEATURE_MMAL_CIQ */
  uint32           message_post_fail_cnt;
}mmal_task_stat_s;

/*! @brief This structure defines the variables that are used within the
    mmal task
  */
typedef struct
{

  /*! The data buffer for the maml task message q */
  mmal_msg_u      msg_q_data[ MMAL_MAX_MSG ];
  /*! The circular mmal message queue */
  mutils_circ_q_s msg_q;
  /*! Critical section to protect the message queue */
  mmal_crit_sect_s msg_crit_sect;
  /*! Signal to notify whenever data is placed in the mmal message queue. */
  mmal_signal_s task_msg_signal;
  /*! Signal to notify whenever MMAL's MSGR REX client & associated queue */
  mmal_signal_s task_msgr_q_signal;
  /*! Signal to notify the mmal task to stop and shutdown */
  mmal_signal_s task_stop_signal;
  /*! Error stats */
  mmal_task_stat_s    task_stat;
} mmal_task_s;


/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/


/*! @brief This structure defines the variables that are used within the
    mmal task
  */
mmal_task_s  mmal_task_info;


/*=============================================================================

                                FUNCTIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  mmal_msg_send

=============================================================================*/
/*!
  @brief
  Sends a message to the MMAL.

  Returns TRUE if the message was put in the circ_q
          FALSE otherwise

*/
/*===========================================================================*/
boolean mmal_msg_send
(
  /*! The message to be sent */
  mmal_msg_hdr_s* msg
)
{
  boolean ret_val;

  mmal_crit_sect_enter( &mmal_task_info.msg_crit_sect );

  ret_val = mutils_circ_q_put_item (
              &mmal_task_info.msg_q,
              (uint8*) msg
              );

  mmal_crit_sect_leave( &mmal_task_info.msg_crit_sect );

  mmal_signal_set( &mmal_task_info.task_msg_signal );

  if(ret_val == FALSE)
  {
    /* Increment stat counter */
    mmal_task_info.task_stat.message_post_fail_cnt++;
  }

  return ret_val;

}

/*=============================================================================

  FUNCTION:  mmal_task_init

=============================================================================*/
/*!
    @brief
    call inits of general modules needed for mmal task and also sets the
    cache line registers

*/
/*===========================================================================*/
void mmal_task_init(void)
{
  errno_enum_type estatus;

  (void)memset(&mmal_task_info, 0, sizeof(mmal_task_s));

  mmal_crit_sect_init( &mmal_task_info.msg_crit_sect );

  mmal_signal_init( &mmal_task_info.task_msg_signal, &mmal_tcb, 
                                        (uint32)MMAL_TASK_MSG_SIG );
  mmal_signal_init( &mmal_task_info.task_msgr_q_signal, &mmal_tcb, 
                                        (uint32)MMAL_MSGR_Q_SIG );
  mmal_signal_init( &mmal_task_info.task_stop_signal, &mmal_tcb, 
                                                    TASK_STOP_SIG );

  mutils_circ_q_init (
      &mmal_task_info.msg_q,
      (uint8*)mmal_task_info.msg_q_data,
      sizeof (mmal_task_info.msg_q_data),
      sizeof ( mmal_msg_u )
      );

  /* Initialize MMAL's MSGR client & associated data */
  estatus = mmal_msgr_init();
  MMAL_ASSERT( E_SUCCESS == estatus );
}

/*=============================================================================

  FUNCTION:  mmal_task_deinit

=============================================================================*/
/*!
    @brief
    Tears down the MMAL thread.

    @return
    None
*/
/*===========================================================================*/
static void mmal_task_deinit(void)
{
  errno_enum_type estatus;
#ifdef FEATURE_MMAL_CIQ
  iq_porting_status_t  ciq_bkend_shutdown_result;
#endif /* FEATURE_MMAL_CIQ */

  /* De-register for all the UMIDs MMAL is interested in */
  mmal_msgr_deregister_msgs();

  /* De-initialize MMAL's MSGR client & associated data */
  estatus = mmal_msgr_deinit();
  MMAL_ASSERT( E_SUCCESS == estatus );

  /* destroy the msg critical section mutex */
  mmal_crit_sect_deinit(&mmal_task_info.msg_crit_sect);

#ifdef FEATURE_MMAL_CIQ
  /*---------------------------------------------------------------------------
    Shutdown IQ Backend
  ---------------------------------------------------------------------------*/
  mmal_ciq_fend_shutdown();

  ciq_bkend_shutdown_result = IQBackend_Shutdown();

  if( ciq_bkend_shutdown_result != IQ_PORTING_OK )
  {
    MMAL_MSG_SPRINTF_1("IQBackend Shutdown failed. %d", 
                                       ciq_bkend_shutdown_result);

    mmal_task_info.task_stat.ciq_error_cnt++;
  }

#endif /* FEATURE_MMAL_CIQ */
}

/*=============================================================================

  FUNCTION:  mmal_task_process_message

=============================================================================*/
/*!
    @brief
    This function processes messages sent to the MMAL task.

*/
/*===========================================================================*/
static void mmal_task_process_message(void)
{
  mmal_msg_u msg;
  boolean processing = TRUE;

  while ( processing )
  {

    /* Dequeue the message, exit the loop if not available */
    if ( !mutils_circ_q_get_item( &mmal_task_info.msg_q, (uint8*) &msg ) )
    {
      break;
    }

    switch ( msg.hdr.id )
    {
      case MMAL_LOG_LISTENER_REQ:
        //placeholder
        mmal_ciq_fend_process_log_listener_req( msg.mmal_log_listener.log_id, 
                                                msg.mmal_log_listener.payload_dsm_ptr );
        break;
#ifdef FEATURE_MMAL_CIQ
      case MMAL_CIQ_BKEND_EXECUTE_TIMER_EXP_REQ:
        mmal_ciq_pl_process_bkend_timer_exp_req();
        break;
      case MMAL_CIQ_PL_SIO_RX_CB_REQ:
        mmal_ciq_pl_process_sio_rx_cb_req( msg.mmal_pl_sio_rx_cb_info.dsm_item_ptr );
        break;
      case MMAL_CIQ_PL_SMD_RX_CB_REQ:
        mmal_ciq_pl_process_smd_rx_cb_req( msg.mmal_pl_smd_rx_cb_info.port,
                                           msg.mmal_pl_smd_rx_cb_info.event,
                                           msg.mmal_pl_smd_rx_cb_info.data );
        break;
      case MMAL_CIQ_PL_TIME_ZONE_CHG_REQ:
        mmal_ciq_pl_process_time_zone_chg_req( msg.mmal_time_zone_info.time_zone,
                                               msg.mmal_time_zone_info.daylight_saving_adj_avail,
                                               msg.mmal_time_zone_info.daylight_saving_adj );
        break;
#endif /* FEATURE_MMAL_CIQ */
#ifdef FEATURE_WCDMA
      case MMAL_CIQ_FEND_WRRC_LOG_MSG_CB_REQ:
        mmal_ciq_wcdma_metric_pack_rrc_msg_info( msg.mmal_wrrc_cb_info.log_msg_ptr, 
                                                 msg.mmal_wrrc_cb_info.dsm_item_ptr );
        break;
#endif
#ifdef FEATURE_LTE
      case MMAL_CIQ_FEND_LRRC_LOG_MSG_CB_REQ:
        mmal_ciq_lte_metric_pack_lt01( msg.mmal_lrrc_cb_info.pdu_num,
                                       msg.mmal_lrrc_cb_info.encoded_msg_len,
                                       msg.mmal_lrrc_cb_info.msg_type, 
                                       msg.mmal_lrrc_cb_info.lc_type,
                                       msg.mmal_lrrc_cb_info.dsm_item_ptr );
        break;
      case MMAL_CIQ_METRICS_LT03_REQ:
        mmal_ciq_lte_metric_pack_and_submit_lt03();
        break;
      case MMAL_CIQ_METRICS_LT04_REQ:
        mmal_ciq_lte_metric_pack_and_submit_lt04();
        break;
#endif
      default:
        MMAL_ASSERT(0);
    }
  }
}

/*=============================================================================

  FUNCTION:  mmal_task_main

=============================================================================*/
/*!
    @brief
    Rex main function for mmal task

*/
/*===========================================================================*/
void mmal_task
(
  dword dummy
)
{
  static mmal_signal_mask_s     sigs;
  mmal_signal_result_t          set_signals;

  (void)dummy;

  mmal_task_init();

  /* MMAL task initializes all modules */

  mmal_signal_mask_init( &sigs );

  /* Synchronize thread with tmc startup */
  tmc_task_start();

  mmal_signal_mask_add( &sigs, &mmal_task_info.task_msg_signal );
  mmal_signal_mask_add( &sigs, &mmal_task_info.task_msgr_q_signal );
  mmal_signal_mask_add( &sigs, &mmal_task_info.task_stop_signal );

#ifdef FEATURE_MMAL_CIQ
  /* MMAL init routine for CIQ Porting Layer */
  if(mmal_ciq_pl_init() == FALSE)
  {
    MMAL_MSG_ERROR("MMAL CIQ Porting Layer Init failed");
    MMAL_ASSERT(0);
  }
  if (mmal_ciq_pl_get_iq_agent_init_flag() == TRUE) 
  {
    /*---------------------------------------------------------------------------
    Invoke IQBackend_Execute to give the IQ Agent a chance to run
    When finished running, a timer value will be provided and the IQ Agent 
    expects another call to IQBackend_Execute upon timer expiry.
    ---------------------------------------------------------------------------*/
    mmal_ciq_pl_invoke_bkend_exec();
  }
  else
  {
    MMAL_MSG_ERROR("Fail to invoke IQ Execute as IQ init was not successful");
    mmal_task_info.task_stat.ciq_error_cnt++;
  }

#endif /* FEATURE_MMAL_CIQ */

  for (;;)
  {
    set_signals = mmal_signal_mask_wait( &sigs );

    if ( mmal_signal_is_set( &set_signals, &mmal_task_info.task_msg_signal ) )
    {
      mmal_signal_clear( &mmal_task_info.task_msg_signal );
      mmal_task_process_message();
    }
    if ( mmal_signal_is_set( &set_signals, &mmal_task_info.task_msgr_q_signal ) )
    {
      mmal_signal_clear( &mmal_task_info.task_msgr_q_signal );
      mmal_msgr_process_rcvd_signal();
    }
    if ( mmal_signal_is_set( &set_signals, &mmal_task_info.task_stop_signal ) )
    {
      mmal_signal_clear( &mmal_task_info.task_stop_signal );
      mmal_task_deinit();
      tmc_task_stop();
      return;
    }
  }

}
