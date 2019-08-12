/*!
  @file
  cxm_msgr.c

  @brief
  Implementation of CXM's MSGR APIs

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

===========================================================================*/
/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/cxm_msgr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/15   ckk     Update type7[rrc_c] using RAT layer 3 states
01/14/15   ckk     Add support for Tx antenna via WCI2 MWS_Connected Type 7
07/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
01/10/13   tak     Added CA and TDS support for WWAN update and sleep indication 
11/05/12   ckk     Add CXM's event trace logging
11/02/12   btl     Fix condition fail for WCN Prio
10/21/12   btl     Added MSGR msgs to support coex power limiting
09/29/12   ag      Added mcs_variation for features
08/14/12   btl     Picked up missed coex msgr handlers
07/24/12   btl     Removed coex_msgr_process_msg()
04/08/12   ckk     Initial implementation

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
/* Common */
#include "mcs_variation.h"
#include <IxErrno.h>

#include <msgr_rex.h>
#include "cxm_msgr.h"
#include "wcn_coex_mgr.h"
#include "cxm_utils.h"
#include "coex_interface.h"
#include "cxm_trace.h"
#include "cxm_smem.h"
/*===========================================================================

                   INTERNAL TYPES

===========================================================================*/

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
/* Buffer for the received messages */
STATIC cxm_msgr_rcv_queue_t cxm_msgr_rcv_q;

/* CXM MSGR client id */
STATIC msgr_client_t        cxm_msgr_client_id;

/* CXM MSGR queue id */
STATIC msgr_id_t            cxm_msgr_queue_id;

/*behavior of the system*/
cxm_sys_bhvr_t coex_sys_behavior_mask;

/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_msgr_register_msg

===========================================================================*/
/*!
  @brief
    To register a single msg/umid for CXM MSGR's client

  @return
    None
*/
/*=========================================================================*/

STATIC void cxm_msgr_register_msg (
  msgr_umid_type umid /*!< UMID of the message to deregister */
)
{
  errno_enum_type  estatus;
  /*-----------------------------------------------------------------------*/
  estatus = msgr_register( MSGR_MCS_CXM,
                           &cxm_msgr_client_id,
                           MSGR_ID_REX,
                           umid );
  CXM_ASSERT( E_SUCCESS == estatus);

  return;
} /* cxm_msgr_register_msg */


/*===========================================================================

  FUNCTION:  cxm_msgr_deregister_msg

===========================================================================*/
/*!
  @brief
    To deregister a single msg/umid for CXM MSGR's client

  @return
    None
*/
/*=========================================================================*/
STATIC void cxm_msgr_deregister_msg (
  msgr_umid_type umid /*!< UMID of the message to deregister */
)
{
  errno_enum_type  estatus;
  /*-----------------------------------------------------------------------*/
  estatus = msgr_deregister( MSGR_MCS_CXM,
                             &cxm_msgr_client_id,
                             umid );
  CXM_ASSERT( E_SUCCESS == estatus);

  return;
} /* cxm_msgr_deregister_msg */

/*===========================================================================

  FUNCTION:  cxm_msgr_process_rcvd_msg

===========================================================================*/
/*!
  @brief
    Process CXM's received MSGR message 

  @return
    None
*/
/*=========================================================================*/
STATIC void cxm_msgr_process_rcvd_msg (
  void *rcv_msgr_msg_ptr /*!< Pointer to the rcvd msgr msg */
)
{
  msgr_hdr_struct_type *msg_ptr;
  /*-----------------------------------------------------------------------*/
  CXM_ASSERT( NULL != rcv_msgr_msg_ptr );
  msg_ptr = (msgr_hdr_struct_type*) rcv_msgr_msg_ptr;

  switch ( msg_ptr->id )
  {

    /* Req for boot params */
    case MCS_CXM_COEX_BOOT_PARAMS_REQ:
      (void) coex_handle_params_req( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_COEX_TECH_TX_ADV_NTC_IND:
      (void) coex_handle_tx_adv_ntc_ind( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_COEX_METRICS_LTE_BLER_IND:
      (void) coex_handle_bler_ind( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_COEX_METRICS_LTE_SINR_RSP:
      (void) coex_handle_lte_sinr_read_cnf( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_COEX_TX_PWR_LMT_LTE_CNDTNS_RSP:
      (void) coex_handle_lte_tx_power_limit_rsp( rcv_msgr_msg_ptr );
      break;

    /* Indication when RRC procedure starts/ends */
    case MCS_CXM_COEX_TX_PWR_LMT_LTE_CNDTNS_IND:
      (void) coex_handle_lte_tx_power_limit_ind( rcv_msgr_msg_ptr );
      break;

    /* WCN Prio condition fail (too many subframe denials) */
    case MCS_CXM_COEX_WCN_TXFRMDNL_REPORT_IND:
      (void) coex_handle_wcn_txfrndnl_report_ind( rcv_msgr_msg_ptr );
      break;

    /* WCN Prio condition fail (too many subframe denials) tech message */
    case MCS_CXM_COEX_TECH_TX_FRM_DNL_REPORT_IND:
      (void) coex_handle_tech_wcn_txfrndnl_report_ind( rcv_msgr_msg_ptr );
      break;

    /* Indication when a tech sleeps or wakes up */
    case MCS_CXM_COEX_TECH_SLEEP_WAKEUP_IND:
      (void) coex_handle_sleep_wakeup_ind( rcv_msgr_msg_ptr );  
      break;

    /* Indication when a tech sends state */
    case MCS_CXM_WWAN_TECH_STATE_IND:
      (void) coex_update_wwan_state_info_ind( rcv_msgr_msg_ptr );  
      break;

    /* Indication when a tech sends frame timing info */
    case MCS_CXM_COEX_TECH_FRAME_TIMING_IND:
      (void) cxm_smem_handle_frame_timing_ind( rcv_msgr_msg_ptr );  
      break;

    /* Indication for tech to inform of high priority events */
    case MCS_CXM_COEX_HIGH_PRIORITY_IND:
      (void) coex_handle_high_priority_ind( rcv_msgr_msg_ptr );  
      break;

    /* Indication for tech to share Tx power report */
    case MCS_CXM_COEX_TECH_TX_PWR_REPORT_IND:
      (void) coex_handle_tech_tx_pwr_report_ind( rcv_msgr_msg_ptr );  
      break;

    /* Indication when a tech activity info in advance */
    case MCS_CXM_COEX_TECH_ACTIVITY_ADV_NTC_IND:
      (void) coex_handle_activity_adv_ntc( rcv_msgr_msg_ptr );  
      break;

    /* Indication when a tech sends metrics */
    case MCS_CXM_COEX_TECH_METRICS_RSP:
      (void) coex_handle_metrics_read_rsp( rcv_msgr_msg_ptr );  
      break;
    
    /* Indication when a tech sends activity timeline */
    case MCS_CXM_SET_SLOT_ACTIVITY_TL_IND:
      (void) coex_handle_activity_timeline_ind( rcv_msgr_msg_ptr );  
      break;

    /* Indication of logging to be done */
    case MCS_CXM_COEX_LOG_ASYNC_IND:
      (void) coex_handle_log_async_ind( rcv_msgr_msg_ptr );
      break;

    /* Indication sent to set Tx antenna in use */
    case MCS_CXM_COEX_TX_ANT_SEL_IND:
      (void) coex_handle_tx_ant_sel_ind( rcv_msgr_msg_ptr );
      break;

    /* Indication to report RAT's Layer 3 states */
    case MCS_CXM_COEX_TECH_L3_STATE_IND:
      (void) coex_handle_tech_l3_state_ind( rcv_msgr_msg_ptr );
      break;

    default:
      break;
  } /* end switch */

  return;
} /* cxm_msgr_process_rcvd_msg */



/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_msgr_init

===========================================================================*/
/*!
  @brief
    To initialize MSGR client & setup associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_msgr_init (
  void
)
{
  errno_enum_type retval = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  /* Initialize the queues */
  (void) q_init( &cxm_msgr_rcv_q );

  /* Create the MSGR client */
  retval = msgr_client_create( &cxm_msgr_client_id );

  if( E_SUCCESS == retval )
  {
    /* Add the CXM REX Q with the MSGR client */
    retval = msgr_client_add_rex_q ( &cxm_msgr_client_id,
                                     &cxm_tcb,
                                     CXM_MSGR_Q_SIG,
                                     &cxm_msgr_rcv_q,
                                     MSGR_NO_QUEUE,
                                     (uint16)sizeof( q_link_type ),
                                     MSGR_NO_OFFSET,
                                     (uint16)sizeof( cxm_msgr_rcv_msg_t ),
                                     &cxm_msgr_queue_id );
  }

  /* Register for all necessary umids */
  if ( E_SUCCESS == retval )
  {
    /* Register for all necessary umids */
    cxm_msgr_register_msgs(); 
  }

  return retval;
} /* cxm_msgr_init */


/*===========================================================================

  FUNCTION:  cxm_msgr_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize MSGR client & associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_msgr_deinit (
  void
)
{
  errno_enum_type  retval;
  /*-----------------------------------------------------------------------*/
  /* Deregister for all umids */
  cxm_msgr_deregister_msgs();

  /* Clear REX signals */
  (void) rex_clr_sigs( &cxm_tcb, CXM_MSGR_Q_SIG );

  /* Delete client with the MSGR */
  retval = msgr_client_delete( &cxm_msgr_client_id );

  return retval;
} /* cxm_msgr_deinit */


/*===========================================================================

  FUNCTION:  cxm_msgr_register_msgs

===========================================================================*/
/*!
  @brief
    To register for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void cxm_msgr_register_msgs (
  void
)
{
  /*-----------------------------------------------------------------------*/
  /* LTE request for boot params */
  cxm_msgr_register_msg( MCS_CXM_COEX_BOOT_PARAMS_REQ );
  cxm_msgr_register_msg( MCS_CXM_COEX_TECH_TX_ADV_NTC_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_METRICS_LTE_BLER_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_METRICS_LTE_SINR_RSP );
  cxm_msgr_register_msg( MCS_CXM_COEX_TX_PWR_LMT_LTE_CNDTNS_RSP );
  cxm_msgr_register_msg( MCS_CXM_COEX_TX_PWR_LMT_LTE_CNDTNS_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_WCN_TXFRMDNL_REPORT_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_TECH_TX_FRM_DNL_REPORT_IND );
  cxm_msgr_register_msg( MCS_CXM_WWAN_TECH_STATE_IND ); 
  cxm_msgr_register_msg( MCS_CXM_COEX_TECH_SLEEP_WAKEUP_IND );  
  cxm_msgr_register_msg( MCS_CXM_COEX_TECH_FRAME_TIMING_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_HIGH_PRIORITY_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_TECH_TX_PWR_REPORT_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_TECH_ACTIVITY_ADV_NTC_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_TECH_METRICS_RSP );  
  cxm_msgr_register_msg( MCS_CXM_SET_SLOT_ACTIVITY_TL_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_LOG_ASYNC_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_TX_ANT_SEL_IND );
  cxm_msgr_register_msg( MCS_CXM_COEX_TECH_L3_STATE_IND );

  return;
} /* cxm_msgr_register_msgs */


/*===========================================================================

  FUNCTION:  cxm_msgr_deregister_msgs

===========================================================================*/
/*!
  @brief
    To deregister for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void cxm_msgr_deregister_msgs (
  void
)
{
  /*-----------------------------------------------------------------------*/
  /* LTE request for boot params */
  cxm_msgr_deregister_msg( MCS_CXM_COEX_BOOT_PARAMS_REQ );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TECH_TX_ADV_NTC_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_METRICS_LTE_BLER_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_METRICS_LTE_SINR_RSP );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TX_PWR_LMT_LTE_CNDTNS_RSP );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TX_PWR_LMT_LTE_CNDTNS_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_WCN_TXFRMDNL_REPORT_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TECH_TX_FRM_DNL_REPORT_IND );
  cxm_msgr_deregister_msg( MCS_CXM_WWAN_TECH_STATE_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TECH_SLEEP_WAKEUP_IND ); 
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TECH_FRAME_TIMING_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_HIGH_PRIORITY_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TECH_TX_PWR_REPORT_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TECH_ACTIVITY_ADV_NTC_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TECH_METRICS_RSP );  
  cxm_msgr_deregister_msg( MCS_CXM_SET_SLOT_ACTIVITY_TL_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_LOG_ASYNC_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TX_ANT_SEL_IND );
  cxm_msgr_deregister_msg( MCS_CXM_COEX_TECH_L3_STATE_IND );

  return;
} /* cxm_msgr_deregister_msgs */


/*===========================================================================

  FUNCTION:  cxm_msgr_send_msg

===========================================================================*/
/*!
  @brief
    Send a message via MSGR

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type cxm_msgr_send_msg (
  msgr_hdr_struct_type *msg_hdr_ptr,  /*!< Pointer to MSG Header  */
  msgr_umid_type        umid,         /*!< UMID of the message    */
  uint32                size          /*!< Size of the message    */
)
{
  errno_enum_type  retval;
  /*-----------------------------------------------------------------------*/
  /* Initialize the message header */
  msgr_init_hdr( msg_hdr_ptr, MSGR_MCS_CXM, umid );

  /* Send the message */
  retval = msgr_send( msg_hdr_ptr, size );

  CXM_TRACE( CXM_TRC_MSGR_SND_MSG, 0, 0, umid, CXM_TRC_EN );

  return retval;
} /* cxm_msgr_send_msg */


/*===========================================================================

  FUNCTION:  cxm_msgr_process_sig

===========================================================================*/
/*!
  @brief
    Process the CXM's MSGR REX signal & associated queue

  @return
    None
*/
/*=========================================================================*/
void cxm_msgr_process_sig (
  void
)
{
  cxm_msgr_rcv_msg_t *rcv_msg_ptr;
  /*-----------------------------------------------------------------------*/
  /* There is a chance of getting more than one message on the
     same signal. Hence do a while loop to process all messages */

  /* Get the received message off the queue and process it */
  while ( ( rcv_msg_ptr = (cxm_msgr_rcv_msg_t*)q_get(&cxm_msgr_rcv_q) ) != NULL )
  {
    CXM_TRACE( CXM_TRC_MSGR_RCV_MSG, 0, 0, rcv_msg_ptr->msg.hdr.id, CXM_TRC_EN );

    /* Now process the received message */
    cxm_msgr_process_rcvd_msg( (void*) &(rcv_msg_ptr->msg) );

    /* Once the processing is over, free the buffer */
    msgr_rex_free_msg_buf( &(rcv_msg_ptr->qlink) );
  } /* End while loop */

  return;
} /* cxm_msgr_process_sig */

