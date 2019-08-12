/*!
  @file
  lmtsmgr_msgr.c

  @brief
  Implementation of LMTSMGR's MSGR APIs

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/utils/src/lmtsmgr_msgr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/14   ag      Added call in distress indication
10/08/14   jm      FreqId generation support in sim mode
08/29/14   jm      Change SL TL sim to use FreqId Pointer
08/18/14   sg      Slot level timeline simulator support
08/12/14   jm      Slot level timeline support
06/23/14   sg      desense indication mechanism
04/28/14   jm      L+G Band Avoidance support
03/17/14   jm      WWAN coex based off of RF device assignment
04/05/13   rj      GPIO Based SAR Control added
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

#include "cxm.h"
#include "lmtsmgr_msgr.h"
#include "lmtsmgr_task.h"
#include "lmtsmgr_cmd.h"

#include "wwan_coex_mgr.h"
#include "sar_dsi.h"

#include "wwcoex_action_iface.h"




/*===========================================================================

                   INTERNAL TYPES

===========================================================================*/
/* LMTSMGR message type */
typedef struct
{
  /* qlink for the REX Q associated to this task */
  q_link_type            qlink;

  /* MSGR message within this packet */
  lmtsmgr_msgr_rcv_msg_u     msg;

} lmtsmgr_msgr_rcv_msg_t;

/* LMTSMGR receive queue type */
typedef q_type lmtsmgr_msgr_rcv_queue_t;


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
/* Buffer for the received messages */
static lmtsmgr_msgr_rcv_queue_t lmtsmgr_msgr_rcv_q;

/* LMTSMGR MSGR client id */
static msgr_client_t        lmtsmgr_msgr_client_id;

/* LMTSMGR MSGR queue id */
static msgr_id_t            lmtsmgr_msgr_queue_id;


/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_register_msg

===========================================================================*/
/*!
  @brief
    To register a single msg/umid for LMTSMGR MSGR's client

  @return
    None
*/
/*=========================================================================*/
static void lmtsmgr_msgr_register_msg (
  msgr_umid_type umid /*!< UMID of the message to deregister */
)
{
  errno_enum_type  estatus;

  /*-----------------------------------------------------------------------*/

  estatus = msgr_register( MSGR_MCS_CXM,
                           &lmtsmgr_msgr_client_id,
                           MSGR_ID_REX,
                           umid );
  LMTSMGR_ASSERT( E_FAILURE != estatus);

  /*-----------------------------------------------------------------------*/

} /* lmtsmgr_msgr_register_msg */


/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_deregister_msg

===========================================================================*/
/*!
  @brief
    To deregister a single msg/umid for LMTSMGR MSGR's client

  @return
    None
*/
/*=========================================================================*/
static void lmtsmgr_msgr_deregister_msg (
  msgr_umid_type umid /*!< UMID of the message to deregister */
)
{
  errno_enum_type  estatus;

  /*-----------------------------------------------------------------------*/

  estatus = msgr_deregister( MSGR_MCS_CXM,
                             &lmtsmgr_msgr_client_id,
                             umid );
  LMTSMGR_ASSERT( E_FAILURE != estatus);

  /*-----------------------------------------------------------------------*/

} /* lmtsmgr_msgr_deregister_msg */


/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_process_rcvd_msg

===========================================================================*/
/*!
  @brief
    Process LMTSMGR's received MSGR message 

  @return
    None
*/
/*=========================================================================*/
static void lmtsmgr_msgr_process_rcvd_msg (
  void *rcv_msgr_msg_ptr /*!< Pointer to the rcvd msgr msg */
)
{
  msgr_hdr_struct_type *msg_ptr;

  /*-----------------------------------------------------------------------*/

  LMTSMGR_ASSERT( NULL != rcv_msgr_msg_ptr );

  msg_ptr = (msgr_hdr_struct_type*) rcv_msgr_msg_ptr;

  switch ( msg_ptr->id )
  {
    case MCS_CXM_WWAN_TECH_STATE_IND:
      lmtsmgr_handle_tech_state_ind( rcv_msgr_msg_ptr, FALSE );
      break;

    case MCS_CXM_COEX_POWER_IND:
      lmtsmgr_handle_coex_power_update( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_SET_ACTIVITY_TIMELINE_IND:
      lmtsmgr_handle_activity_tl_update( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_INACTIVITY_CLIENT_REGISTRATION_IND:
      lmtsmgr_handle_inactivity_reg( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_STX_SET_POWER_IND:
      lmtsmgr_handle_tech_power_update( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_STX_SET_TX_STATE_IND:
      lmtsmgr_handle_tech_tx_on( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_STX_CLR_TX_STATE_IND:
      lmtsmgr_handle_tech_tx_off( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_STX_SET_TX_STATE_WITH_CHS_IND:
      lmtsmgr_handle_tech_tx_on_with_chs( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_SAR_SET_DSI_IND:
      sar_handle_set_dsi_ind( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_CHAIN_OWNER_UPDATE_IND:
      lmtsmgr_handle_chain_owner_upd_ind( rcv_msgr_msg_ptr );  
      break;

    case MCS_CXM_BAND_AVOID_BLIST_RSP:
      lmtsmgr_handle_band_avoidance_blist_rsp( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_BAND_AVOID_FREQ_IND:
      lmtsmgr_handle_band_avoidance_freq_ind( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_BAND_AVOID_PWR_IND:
      lmtsmgr_handle_band_avoidance_pwr_ind( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_DESENSE_IND_REQ:
      lmtsmgr_handle_desense_ind_req( rcv_msgr_msg_ptr );
      break;
 
    case MCS_CXM_TECH_DISTRESS_IND:
      lmtsmgr_handle_distress_ind( rcv_msgr_msg_ptr );
      break;

    case MCS_CXM_SET_SLOT_ACTIVITY_TL_IND:
      lmtsmgr_handle_slot_lvl_act_ind( rcv_msgr_msg_ptr );
      break;
 
    case MCS_CXM_SET_SIM_SLOT_ACTIVITY_TL_IND:
      lmtsmgr_handle_slot_lvl_act_sim_ind( rcv_msgr_msg_ptr );
      break;
      
    default:
      LMTSMGR_MSG_1(ERROR, "Received unknown ID: 0x%x", msg_ptr->id);
      break;
  } /* end switch */

  /*-----------------------------------------------------------------------*/
} /* lmtsmgr_msgr_process_rcvd_msg */



/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_init

===========================================================================*/
/*!
  @brief
    To initialize MSGR client & setup associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type lmtsmgr_msgr_init (
  void
)
{
  errno_enum_type retval = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  /* Initialize the queues */
  (void) q_init( &lmtsmgr_msgr_rcv_q );

  /* Create the MSGR client */
  retval = msgr_client_create( &lmtsmgr_msgr_client_id );

  if( E_SUCCESS == retval )
  {
    /* Add the LMTSMGR REX Q with the MSGR client */
    retval = msgr_client_add_rex_q ( &lmtsmgr_msgr_client_id,
                                     &lmtsmgr_tcb,
                                     LMTSMGR_MSGR_Q_SIG,
                                     &lmtsmgr_msgr_rcv_q,
                                     MSGR_NO_QUEUE,
                                     (uint16)sizeof( q_link_type ),
                                     MSGR_NO_OFFSET,
                                     (uint16)sizeof( lmtsmgr_msgr_rcv_msg_t ),
                                     &lmtsmgr_msgr_queue_id );
  }

  /* Register for all necessary umids */
  if ( E_SUCCESS == retval )
  {
    /* Register for all necessary umids */
    lmtsmgr_msgr_register_msgs(); 
  }

  /*-----------------------------------------------------------------------*/

  return retval;
} /* lmtsmgr_msgr_init */


/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize MSGR client & associated queue(s)

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type lmtsmgr_msgr_deinit (
  void
)
{
  errno_enum_type  retval;

  /*-----------------------------------------------------------------------*/

  /* Deregister for all umids */
  lmtsmgr_msgr_deregister_msgs();

  /* Clear REX signals */
  (void) rex_clr_sigs( &lmtsmgr_tcb, LMTSMGR_MSGR_Q_SIG );

  /* Delete client with the MSGR */
  retval = msgr_client_delete( &lmtsmgr_msgr_client_id );

  /*-----------------------------------------------------------------------*/

  return retval;
} /* lmtsmgr_msgr_deinit */


/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_register_msgs

===========================================================================*/
/*!
  @brief
    To register for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void lmtsmgr_msgr_register_msgs (
  void
)
{
  /*-----------------------------------------------------------------------*/
  lmtsmgr_msgr_register_msg( MCS_CXM_STX_SET_TX_STATE_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_STX_CLR_TX_STATE_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_STX_SET_POWER_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_STX_SET_TX_STATE_WITH_CHS_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_WWAN_TECH_STATE_IND );

  lmtsmgr_msgr_register_msg( MCS_CXM_COEX_POWER_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_SET_ACTIVITY_TIMELINE_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_INACTIVITY_CLIENT_REGISTRATION_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_SAR_SET_DSI_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_CHAIN_OWNER_UPDATE_IND );

  lmtsmgr_msgr_register_msg( MCS_CXM_BAND_AVOID_FREQ_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_BAND_AVOID_PWR_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_BAND_AVOID_BLIST_RSP );

  lmtsmgr_msgr_register_msg( MCS_CXM_DESENSE_IND_REQ );
  lmtsmgr_msgr_register_msg( MCS_CXM_TECH_DISTRESS_IND );

  lmtsmgr_msgr_register_msg( MCS_CXM_SET_SLOT_ACTIVITY_TL_IND );
  lmtsmgr_msgr_register_msg( MCS_CXM_SET_SIM_SLOT_ACTIVITY_TL_IND );

  /*-----------------------------------------------------------------------*/

  return;
} /* lmtsmgr_msgr_register_msgs */


/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_deregister_msgs

===========================================================================*/
/*!
  @brief
    To deregister for all related UMIDs

  @return
    None
*/
/*=========================================================================*/
void lmtsmgr_msgr_deregister_msgs (
  void
)
{
  /*-----------------------------------------------------------------------*/
  lmtsmgr_msgr_deregister_msg( MCS_CXM_STX_SET_TX_STATE_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_STX_CLR_TX_STATE_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_STX_SET_POWER_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_STX_SET_TX_STATE_WITH_CHS_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_WWAN_TECH_STATE_IND );

  lmtsmgr_msgr_deregister_msg( MCS_CXM_COEX_POWER_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_SET_ACTIVITY_TIMELINE_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_INACTIVITY_CLIENT_REGISTRATION_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_SAR_SET_DSI_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_CHAIN_OWNER_UPDATE_IND );

  lmtsmgr_msgr_deregister_msg( MCS_CXM_BAND_AVOID_FREQ_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_BAND_AVOID_PWR_IND );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_BAND_AVOID_BLIST_RSP );

  lmtsmgr_msgr_deregister_msg( MCS_CXM_DESENSE_IND_REQ );
  lmtsmgr_msgr_deregister_msg( MCS_CXM_TECH_DISTRESS_IND );
 
  lmtsmgr_msgr_deregister_msg (MCS_CXM_SET_SLOT_ACTIVITY_TL_IND); 
  lmtsmgr_msgr_deregister_msg (MCS_CXM_SET_SIM_SLOT_ACTIVITY_TL_IND); 
  
  /*-----------------------------------------------------------------------*/

  return;
} /* lmtsmgr_msgr_deregister_msgs */


/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_send_msg

===========================================================================*/
/*!
  @brief
    Send a message via MSGR

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type lmtsmgr_msgr_send_msg (
  msgr_hdr_struct_type *msg_hdr_ptr,  /*!< Pointer to MSG Header  */
  msgr_umid_type        umid,         /*!< UMID of the message    */
  int32                 size          /*!< Size of the message    */
)
{
  errno_enum_type  retval;

  /*-----------------------------------------------------------------------*/

  /* Initialize the message header */
  msgr_init_hdr( msg_hdr_ptr, MSGR_MCS_CXM, umid );

  /* Send the message */
  retval = msgr_send( msg_hdr_ptr, size );

  /*-----------------------------------------------------------------------*/

  return retval;
} /* lmtsmgr_msgr_send_msg */


/*===========================================================================

  FUNCTION:  lmtsmgr_msgr_process_sig

===========================================================================*/
/*!
  @brief
    Process the LMTSMGR's MSGR REX signal & associated queue

  @return
    None
*/
/*=========================================================================*/
void lmtsmgr_msgr_process_sig (
  void
)
{
  lmtsmgr_msgr_rcv_msg_t *rcv_msg_ptr;

  /*-----------------------------------------------------------------------*/

  /* There is a chance of getting more than one message on the
     same signal. Hence do a while loop to process all messages */

  /* Get the received message off the queue and process it */
  while ( ( rcv_msg_ptr = (lmtsmgr_msgr_rcv_msg_t*)q_get(&lmtsmgr_msgr_rcv_q) ) != NULL )
  {
    /* Check if we need to pet dog */
    lmtsmgr_task_check_dog();
    
    /* Now process the received message */
    lmtsmgr_msgr_process_rcvd_msg( (void*) &(rcv_msg_ptr->msg) );

    /* Once the processing is over, free the buffer */
    msgr_rex_free_msg_buf( &(rcv_msg_ptr->qlink) );
  } /* End while loop */

  /*-----------------------------------------------------------------------*/

  return;
} /* lmtsmgr_msgr_process_sig */

