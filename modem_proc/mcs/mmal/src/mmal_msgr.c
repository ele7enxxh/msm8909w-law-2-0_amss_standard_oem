/*!
  @file
  mmal_msgr.c

  @brief
  Implementation of MMAL's MSGR APIs

*/

/*===========================================================================

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

===========================================================================*/
/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_msgr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/11   ckk     Initial implementation

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "mcs_variation.h"
#include <IxErrno.h>

#include <msgr_rex.h>
#include "mmal_cfg.h"
#include "mmal_msgr.h"
#include "mmal_msg_i.h"
#include "mmal_util.h"
#include "mmal_lte_metrics.h"

#include <lte_mac.h>
#include <lte_cphy_msg.h>
#include <lte_mac_msg.h>
#include <lte_rlc_msg.h>
#include <lte_rrc_ext_msg.h>
#include <intf_ul.h>
#include <sm_cm_msgr_enum_msg.h>

/*===========================================================================

                   FORWARDED DECLARATIONS

===========================================================================*/
/*! @brief MMAL task rex control block
 * On target these declarations reside in service/task/task.c */
extern rex_tcb_type           mmal_tcb;

extern lte_mac_stats_data_s   lt07_mac_stats;

extern lte_rlcdl_stats_s      lt09_dl_stats;
extern lte_rlcul_stats_s      lt09_ul_stats;

/*===========================================================================

                   INTERNAL TYPES

===========================================================================*/

/* This is a union of all the messages that MMAL is interested in */
typedef union
{
  /* empty */
  msgr_hdr_struct_type                        hdr;
  /* GS19 */
  sm_pdp_context_request_log_info_ind_T       nas_sm_pdp_context_req_msg;
#if 0
  /* LT05 */
  lte_cphy_conn_meas_cfg_req_s                lte_cphy_conn_meas;
  lte_cphy_idle_meas_cfg_req_s                lte_cphy_idle_meas;
#endif
  /* LT07 */
  /* LT08 */
  lte_cphy_raa_ind_msg_s                      lte_mac_raa_msg;
  /* LT09 */
  /* LT12 */
  lte_LL1_ul_pusch_tx_status_ind_msg_struct   lte_ll1_ul_pusch_tx_stats;
  /* LT13 */
  lte_cphy_t310_status_ind_s                  lte_cphy_t310_msg;
} mmal_msgr_rcv_msg_u;

/* MMAL message type */
typedef struct
{
  q_link_type                                 qlink;
  mmal_msgr_rcv_msg_u                         msg;
} mmal_msgr_rcv_msg_t;

/* MMAL receive queue type */
typedef q_type mmal_msgr_rcv_queue_t;


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
/* Buffer for the received messages */
static mmal_msgr_rcv_queue_t  mmal_msgr_rcv_q;

/* MMAL MSGR client id */
static msgr_client_t          mmal_msgr_client_id;

/* MMAL MSGR queue id */
static msgr_id_t              mmal_msgr_queue_id;


/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  mmal_msgr_register_msg

===========================================================================*/
/*!
  @brief
    To register a single msg/umid for MMAL MSGR's client

  @return
    None
*/
/*=========================================================================*/
void mmal_msgr_register_msg
(
  msgr_umid_type umid /*!< UMID of the message to deregister */
)
{
  errno_enum_type  estatus;

  /*-----------------------------------------------------------------------*/

  estatus = msgr_register( MSGR_MCS_MMAL,
                           &mmal_msgr_client_id,
                           MSGR_ID_REX,
                           umid );
  MMAL_ASSERT( E_FAILURE != estatus);

  /*-----------------------------------------------------------------------*/

} /* mmal_msgr_register_msg */


/*===========================================================================

  FUNCTION:  mmal_msgr_deregister_msg

===========================================================================*/
/*!
  @brief
    To deregister a single msg/umid for MMAL MSGR's client

  @return
    None
*/
/*=========================================================================*/
void mmal_msgr_deregister_msg
(
  msgr_umid_type umid /*!< UMID of the message to deregister */
)
{
  errno_enum_type  estatus;

  /*-----------------------------------------------------------------------*/

  estatus = msgr_deregister( MSGR_MCS_MMAL,
                             &mmal_msgr_client_id,
                             umid );
  MMAL_ASSERT( E_FAILURE != estatus);

  /*-----------------------------------------------------------------------*/

} /* mmal_msgr_deregister_msg */


/*===========================================================================

  FUNCTION:  mmal_msgr_process_rcvd_msg

===========================================================================*/
/*!
  @brief
    Process MMAL's received MSGR message 

  @return
    None
*/
/*=========================================================================*/
static void mmal_msgr_process_rcvd_msg
(
  mmal_msgr_rcv_msg_u  *rcv_msgr_msg_ptr /*!< Pointer to the rcvd msgr msg */
)
{
  static boolean        process_lt07 = FALSE;

  MMAL_ASSERT( NULL != rcv_msgr_msg_ptr );

  /*-----------------------------------------------------------------------*/

  switch ( rcv_msgr_msg_ptr->hdr.id )
  {
    case NAS_SM_PDP_CONTEXT_REQUEST_LOG_INFO_IND:
      /* GS19 message on pdp context activation attempt */
      MMAL_MSG_SPRINTF_1("MMAL: Received NAS_SM_PDP_CONTEXT_REQUEST_LOG_INFO_IND - 0x%x\n", NAS_SM_PDP_CONTEXT_REQUEST_LOG_INFO_IND );
      if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_GS19_METRIC ) )
      {
        mmal_ciq_metric_pack_and_submit_msg_gs19( &rcv_msgr_msg_ptr->nas_sm_pdp_context_req_msg );
      }
      break;
#ifdef FEATURE_MMAL_LTE
#if 0
    case LTE_CPHY_CONN_MEAS_CFG_REQ:
      /* LT05 message while in connected mode */
      MMAL_MSG_SPRINTF_1("MMAL: Received LTE_CPHY_CONN_MEAS_CFG_REQ - 0x%x\n", LTE_CPHY_CONN_MEAS_CFG_REQ );
      if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_LT05_METRIC ) )
      {
        mmal_ciq_lte_metric_pack_and_submit_conn_msg_lt05( &rcv_msgr_msg_ptr->lte_cphy_conn_meas );
      }
      break;
    case LTE_CPHY_IDLE_MEAS_CFG_REQ:
      /* LT05 message while in idle mode */
      MMAL_MSG_SPRINTF_1("MMAL: Received LTE_CPHY_IDLE_MEAS_CFG_REQ - 0x%x\n", LTE_CPHY_IDLE_MEAS_CFG_REQ );
      if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_LT05_METRIC ) )
      {
        mmal_ciq_lte_metric_pack_and_submit_idle_msg_lt05( &rcv_msgr_msg_ptr->lte_cphy_idle_meas );
      }
      break;
#endif
    case LTE_RRC_CONN_EST_CNF:
      MMAL_MSG( "MMAL: RRC CONN EST, LT07 enabled" );
      process_lt07 = TRUE;
      break;
    case LTE_RRC_CONN_REL_IND:
      MMAL_MSG( "MMAL: RRC CONN REL, LT07 disabled" );
      process_lt07 = FALSE;
      break;
    case LTE_MAC_STATS_UPDATE_CNF:
      /* LT07 response to LTE_MAC_STATS_UPDATE_REQ */
      if ( process_lt07 )
      {
        MMAL_MSG_SPRINTF_1("MMAL: Received LTE_MAC_STATS_UPDATE_CNF - 0x%x\n", LTE_MAC_STATS_UPDATE_CNF );
        if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_LT07_METRIC ) )
        {
          mmal_ciq_lte_metric_pack_and_submit_msg_lt07( &lt07_mac_stats );
        }
      }
      break;
    case LTE_CPHY_MAC_RAA_IND:
      /* LT08 message */
      MMAL_MSG_SPRINTF_1("MMAL: Received LTE_CPHY_MAC_RAA_IND - 0x%x\n", LTE_CPHY_MAC_RAA_IND );
      if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_LT08_METRIC ) )
      {
        mmal_ciq_lte_metric_pack_and_submit_msg_lt08( &rcv_msgr_msg_ptr->lte_mac_raa_msg );
      }
      break;
    case LTE_RLCDL_STATS_CNF:
      /* LT09 response to LTE_RLCDL_STATS_REQ */
      MMAL_MSG_SPRINTF_1("MMAL: Received LTE_RLCDL_STATS_CNF - 0x%x\n", LTE_RLCDL_STATS_CNF );
      if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_LT09_METRIC ) )
      {
        mmal_ciq_lte_metric_pack_dl_msg_lt09( &lt09_dl_stats );
      }
      break;
    case LTE_RLCUL_STATS_CNF:
      /* LT09 response to LTE_RLCUL_STATS_REQ */
      MMAL_MSG_SPRINTF_1("MMAL: Received LTE_RLCUL_STATS_CNF - 0x%x\n", LTE_RLCUL_STATS_CNF );
      if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_LT09_METRIC ) )
      {
        mmal_ciq_lte_metric_pack_ul_and_submit_msg_lt09( &lt09_ul_stats );
      }
      break;
    case LTE_LL1_UL_PUSCH_TX_STATUS_IND:
      /* LT12 message */
      MMAL_MSG_SPRINTF_1("MMAL: Received LTE_LL1_UL_PUSCH_TX_STATUS_IND - 0x%x\n", LTE_LL1_UL_PUSCH_TX_STATUS_IND );
      if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_LT12_METRIC ) );
      {
        MMAL_MSG_SPRINTF_1("MMAL: Calling pack and submit LT12 - 0x%x\n", 0 );
        mmal_ciq_lte_metric_pack_and_submit_msg_lt12( &rcv_msgr_msg_ptr->lte_ll1_ul_pusch_tx_stats );
      }
      break;
    case LTE_CPHY_T310_STATUS_IND:
      /* LT13 message */
      MMAL_MSG_SPRINTF_1("MMAL: Received LTE_CPHY_T310_STATUS_IND - 0x%x\n", LTE_CPHY_T310_STATUS_IND );
      if ( mmal_ciq_fend_check_if_metric_enabled( MMAL_CIQ_LT13_METRIC ) )
      {
        mmal_ciq_lte_metric_pack_and_submit_msg_lt13( &rcv_msgr_msg_ptr->lte_cphy_t310_msg );
      }
      break;
#endif // FEATURE_MMAL_LTE
    default:
      MMAL_MSG_SPRINTF_1("MMAL: MSGR ERROR: UNANTICIPATED UMID - 0x%x\n", rcv_msgr_msg_ptr->hdr.id );
      break;
  } /* end switch */

  /*-----------------------------------------------------------------------*/
} /* mmal_msgr_process_rcvd_msg */



/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  mmal_msgr_init

===========================================================================*/
/*!
  @brief
    To initialize MSGR client & setup associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type mmal_msgr_init( void )
{
  errno_enum_type retval = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  /* Initialize the queues */
  (void) q_init( &mmal_msgr_rcv_q );

  /* Create the MSGR client */
  retval = msgr_client_create( &mmal_msgr_client_id );

  if( E_SUCCESS == retval )
  {
    /* Add the MMAL REX Q with the MSGR client */
    retval = msgr_client_add_rex_q ( &mmal_msgr_client_id,
                                     &mmal_tcb,
                                     MMAL_MSGR_Q_SIG,
                                     &mmal_msgr_rcv_q,
                                     MSGR_NO_QUEUE,
                                     (uint16)sizeof( q_link_type ),
                                     MSGR_NO_OFFSET,
                                     (uint16)sizeof( mmal_msgr_rcv_msg_t ),
                                     &mmal_msgr_queue_id );
  }

  if ( E_SUCCESS == retval )
  {
    mmal_msgr_register_msgs(); 
  }

  /*-----------------------------------------------------------------------*/

  return retval;
} /* mmal_msgr_init */


/*===========================================================================

  FUNCTION:  mmal_msgr_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize MSGR client & associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type mmal_msgr_deinit( void )
{
  errno_enum_type  retval;

  /*-----------------------------------------------------------------------*/

  /* Clear REX signals */
  (void) rex_clr_sigs( &mmal_tcb, MMAL_MSGR_Q_SIG );

  /* Deregister the client with the MSGR */
  retval = msgr_client_delete( &mmal_msgr_client_id );

  /*-----------------------------------------------------------------------*/

  return retval;
} /* mmal_msgr_deinit */


/*===========================================================================

  FUNCTION:  mmal_msgr_register_msgs

===========================================================================*/
/*!
  @brief
    To register for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void mmal_msgr_register_msgs( void )
{
  /*-----------------------------------------------------------------------*/

#ifndef FEATURE_MMAL_DISABLE_GS19
  /* GS19 */
  mmal_msgr_register_msg( NAS_SM_PDP_CONTEXT_REQUEST_LOG_INFO_IND );
#endif /* !FEATURE_MMAL_DISABLE_GS19 */

#if 0
  /* LT05 */
  mmal_msgr_register_msg( LTE_CPHY_CONN_MEAS_CFG_REQ );
  mmal_msgr_register_msg( LTE_CPHY_IDLE_MEAS_CFG_REQ );
#endif
  /* LT07 */
  mmal_msgr_register_msg( LTE_RRC_CONN_EST_CNF );
  mmal_msgr_register_msg( LTE_RRC_CONN_REL_IND );
  mmal_msgr_register_msg( LTE_MAC_STATS_UPDATE_CNF );
  /* LT08 */
  mmal_msgr_register_msg( LTE_CPHY_MAC_RAA_IND );
  /* LT09 */
  mmal_msgr_register_msg( LTE_RLCDL_STATS_CNF );
  mmal_msgr_register_msg( LTE_RLCUL_STATS_CNF );
  /* LT12 */
  mmal_msgr_register_msg( LTE_LL1_UL_PUSCH_TX_STATUS_IND );
  /* LT13 */
  mmal_msgr_register_msg( LTE_CPHY_T310_STATUS_IND );
} /* mmal_msgr_register_msgs */


/*===========================================================================

  FUNCTION:  mmal_msgr_deregister_msgs

===========================================================================*/
/*!
  @brief
    To deregister for all LTE related UMIDs

  @return
    None
*/
/*=========================================================================*/
void mmal_msgr_deregister_msgs( void )
{
  /*-----------------------------------------------------------------------*/

#ifndef FEATURE_MMAL_DISABLE_GS19
  /* GS19 */
  mmal_msgr_deregister_msg( NAS_SM_PDP_CONTEXT_REQUEST_LOG_INFO_IND );
#endif /* !FEATURE_MMAL_DISABLE_GS19 */

  /* LT07 */
  mmal_msgr_deregister_msg( LTE_RRC_CONN_EST_CNF );
  mmal_msgr_deregister_msg( LTE_RRC_CONN_REL_IND );
  mmal_msgr_deregister_msg( LTE_MAC_STATS_UPDATE_CNF );
  /* LT08 */
  mmal_msgr_deregister_msg( LTE_CPHY_MAC_RAA_IND );
  /* LT09 */
  mmal_msgr_deregister_msg( LTE_RLCDL_STATS_CNF );
  mmal_msgr_deregister_msg( LTE_RLCUL_STATS_CNF );
  /* LT12 */
  mmal_msgr_deregister_msg( LTE_LL1_UL_PUSCH_TX_STATUS_IND );
  /* LT13 */
  mmal_msgr_deregister_msg( LTE_CPHY_T310_STATUS_IND );
} /* mmal_msgr_deregister_msgs */


/*===========================================================================

  FUNCTION:  mmal_msgr_send_msg

===========================================================================*/
/*!
  @brief
    Send a message via MSGR

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type mmal_msgr_send_msg
(
  msgr_hdr_struct_type *msg_hdr_ptr,  /*!< Pointer to MSG Header  */
  msgr_umid_type        umid,         /*!< UMID of the message    */
  int32                 size          /*!< Size of the message    */
)
{
  errno_enum_type  retval;

  /*-----------------------------------------------------------------------*/

  /* Initialize the message header */
  msgr_init_hdr( msg_hdr_ptr, MSGR_MCS_MMAL, umid );

  /* Send the message */
  retval = msgr_send( msg_hdr_ptr, size );

  /*-----------------------------------------------------------------------*/

  return retval;
} /* mmal_msgr_send_msg */


/*===========================================================================

  FUNCTION:  mmal_msgr_process_rcv_signal

===========================================================================*/
/*!
  @brief
    Process the MMAL's MSGR REX signal & associated queue

  @return
    None
*/
/*=========================================================================*/
void mmal_msgr_process_rcvd_signal( void )
{
  mmal_msgr_rcv_msg_t *rcv_msg_ptr;

  /*-----------------------------------------------------------------------*/

  /* Clear the signal */
  (void) rex_clr_sigs ( &mmal_tcb, MMAL_MSGR_Q_SIG );

  /* There is a chance of getting more than one message on the
     same signal. Hence do a while loop to process all messages */

  /* Get the received message off the queue and process it */
  while ( ( rcv_msg_ptr = (mmal_msgr_rcv_msg_t*)q_get(&mmal_msgr_rcv_q) ) != NULL )
  {
    /* Now process the received message */
    mmal_msgr_process_rcvd_msg( &(rcv_msg_ptr->msg) );

    /* Once the processing is over, free the buffer */
    msgr_rex_free_msg_buf( &(rcv_msg_ptr->qlink) );

  } /* End while loop */

  /*-----------------------------------------------------------------------*/
} /* mmal_msgr_process_rcv_signal */
