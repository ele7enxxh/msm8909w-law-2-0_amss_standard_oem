/*===========================================================================

                            TM_LTE_MSGR_IFACE

DESCRIPTION


===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Qualcomm Proprietary

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

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_lte_nas_msgr_iface.c#1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
3/12/14      gk     Handle OTA reset when session is active. consider as end of the session
6/20/12      gk     Initial checkin
===========================================================================*/

/*=============================================================================
                                INCLUDE FILES
=============================================================================*/
#include "comdef.h"
 
#include "tm_lte_nas_msgr_iface.h"
#include "pdapi.h"
#include "sm_loc_lte_otdoa.h"


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Macros & Static

=============================================================================*/


#ifdef  FEATURE_CGPS_LTE_CELLDB
  /* LTE OTA reset message type identifier */
  #define MSG_IDEN_LTE_OTA_GPS_RESET 0
  #define MSG_IDEN_LTE_OTA_OTDOA_RESET 1

  static boolean tm_msgr_is_client_created = FALSE;
#endif /* FEATURE_CGPS_LTE_CELLDB */

/* Buffer to receive messages over MSGR */
static tm_msgr_rcv_buffer_type  tm_msgr_rcv;

/*=============================================================================

       Typedefs

=============================================================================*/


/*=============================================================================

       Function Declarations

=============================================================================*/

/*===========================================================================

FUNCTION       TM_MSGR_INIT_QUEUE

DESCRIPTION    This function initializes the receive queue

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static void tm_msgr_init_queue( void )
{
   uint8 i;
    /* Initialize the Command Queue, and the Command Free Queue */
  (void) q_init( &tm_msgr_rcv.msg_q );
  (void) q_init( &tm_msgr_rcv.free_q );

  /* Link the Command Queue Items onto the Command Free Queue */
  for (i=0; i<TM_MSGR_RCV_QUEUE_SIZE; i++)
  {
    q_put( &tm_msgr_rcv.free_q,
    q_link( &tm_msgr_rcv.q_items[i],
           &tm_msgr_rcv.q_items[i].msg_hdr.qlink ) );
  }/* init_queue */

}


/*===========================================================================

FUNCTION     TM_MSGR_GET_QUEUE_ID

DESCRIPTION    This function retuns the msgr queue id.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static msgr_id_t*  tm_msgr_get_queue_id( void )
{
  // MSGR Queue ID
  static msgr_id_t tm_msgr_queue_id;

  return &tm_msgr_queue_id;

} /*  tm_msgr_get_queue_id */


/*===========================================================================

FUNCTION tm_msgr_init

DESCRIPTION
  This function is to initialize MSGR interface .
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_msgr_init(void)
{

  static msgr_client_t tm_msgr_client_id = 0;
  errno_enum_type  estatus;
  tm_msgr_init_queue();


  estatus = msgr_client_create( &tm_msgr_client_id );

  if (estatus == E_SUCCESS)
  {
    estatus = msgr_client_add_rex_q ( &tm_msgr_client_id,
                                      rex_self(),  // will return own TCB
                                      C_OS_FLAG_MSGR_MSG,  // OSAPI will convert this to IPC
                                      &tm_msgr_rcv.msg_q,
                                      &tm_msgr_rcv.free_q,
                                      offsetof( tm_msgr_rcv_msg_type, msg ),
                                      MSGR_NO_OFFSET,
                                      (uint16)sizeof(tm_msgr_rcv_msg_type ),
                                      tm_msgr_get_queue_id() );
    
#ifdef  FEATURE_CGPS_LTE_CELLDB   
    if (estatus == E_SUCCESS)
    {
      tm_msgr_is_client_created = TRUE;
      estatus = msgr_register( MSGR_GPS_SM_TM, &tm_msgr_client_id, MSGR_ID_REX, (int)LTE_TLB_DL_INFO_IND);
    }
#endif
    if (estatus == E_SUCCESS)
    {
      estatus = msgr_register( MSGR_GPS_SM_TM, &tm_msgr_client_id, MSGR_ID_REX, (int)MM_CM_AC_EMERGENCY_ENTER_IND );
    }
    if (estatus == E_SUCCESS)
    {
      estatus = msgr_register( MSGR_GPS_SM_TM, &tm_msgr_client_id, MSGR_ID_REX, (int)MM_CM_AC_EMERGENCY_EXIT_IND);
    }
    if ( estatus != E_SUCCESS )
    {
      estatus = msgr_client_delete(&tm_msgr_client_id);
      MSG_ERROR("GNSS MSGR: Failed to register NAS DL message error=%d", (uint32)estatus, 0, 0);
      return ;
    }
  }
  else
  {
    MSG_ERROR("TM_LTE:MSGR_CREATE failed %d", estatus,0,0);
  }
}

/*===========================================================================

FUNCTION     tm_msgr_check_nas_ota_event

DESCRIPTION    This function checks if the LTE OTA message is a OTA RESET message

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tm_msgr_check_event(void)
{

  tm_msgr_rcv_msg_type* msg_ptr = NULL;

  // receive MSGR message from Queue
  msg_ptr = (tm_msgr_rcv_msg_type*)q_get(&tm_msgr_rcv.msg_q);
  if (msg_ptr == NULL)
  {
    MSG_ERROR("TM: No MSGR message",0,0,0);
    return;
  }

  /* check what kind of OTA test message this is */
  MSG_MED("MSGR message id %dpayload[1]=%d  received", 
          msg_ptr->msg.hdr.id,0,0);
  
  switch (msg_ptr->msg.hdr.id)
  {
    
#ifdef  FEATURE_CGPS_LTE_CELLDB   
    case LTE_TLB_DL_INFO_IND:
      MSG_MED("MSGR message id %dpayload[1]=%d  payload[2]=%d received", 
              msg_ptr->msg.hdr.id,
               msg_ptr->msg.dl_info.nas_msg_container.msg_payload[1],
               msg_ptr->msg.dl_info.nas_msg_container.msg_payload[2]);
      if (msg_ptr->msg.dl_info.nas_msg_container.msg_payload[1] == 0x88)
      {
        if (msg_ptr->msg.dl_info.nas_msg_container.msg_size -1 > 0)
        {
          if (msg_ptr->msg.dl_info.nas_msg_container.msg_payload[2] == MSG_IDEN_LTE_OTA_GPS_RESET)
          {
            tm_handle_lte_ota_gps_reset();
          }

          /* LTE OTA OTDOA reset ? */
          if ( msg_ptr->msg.dl_info.nas_msg_container.msg_payload[2] == MSG_IDEN_LTE_OTA_OTDOA_RESET )
          {
            /* send the LTE OTA OTDOA reset to the OTDOA engine */
            MSG_HIGH("OTDOA OTA received. Send to LOC_LTE ",0,0,0);
            if (FALSE == sm_loc_lte_ota_reset())
            {
              MSG_HIGH("OTDOA OTA reset failed to be sent to LOC_LTE ",0,0,0);
            }
            /* Send LTE OTA OTDOA reset to TM */
            if ( !tm_core_session_active() )
            {
              tm_core_lte_nas_ota_reset_notify_event(PRTL_EVENT_TYPE_LTE_OTA_OTDOA_RESET);
            }
            else
            {
              MSG_HIGH("TM_LTE_NAS: OTA OTDOA Reset while session is active. Drop it", 0, 0, 0);
            }
          }
        }
      }
      break;
#endif

    case MM_CM_AC_EMERGENCY_ENTER_IND: // need to change this to IND
      // call cm to set the state to emergency
      if (msg_ptr->msg.emergency_enter_info.reason == CM_AC_REASON_THIRD_PARTY_IMS_E911)
      {
        tm_cm_process_ext_emergency_enter(TM_CM_LTE_EMERGENCY_START);
      }
      break;
    case MM_CM_AC_EMERGENCY_EXIT_IND:
      if (msg_ptr->msg.emergency_exit_info.reason == CM_AC_REASON_THIRD_PARTY_IMS_E911) 
      {
        tm_cm_process_ext_emergency_enter(TM_CM_LTE_EMERGENCY_STOP);
      }
      if (msg_ptr->msg.emergency_exit_info.reason == CM_AC_REASON_THIRD_PARTY_IMS_E911_ECBM) 
      {
        tm_cm_process_ext_emergency_enter(TM_CM_LTE_EMERGENCY_ECBM);
      }
       break;
    default:
      MSG_MED("TM_CORE: UNhandled  MSGR message",0,0,0);
      break;
  }

  if (msg_ptr != NULL)
  {
    // return item to free queue
    q_put( &tm_msgr_rcv.free_q, &msg_ptr->msg_hdr.qlink );
  }
}

/*===========================================================================

FUNCTION     tm_handle_msgr_msg

DESCRIPTION    This function handles msgr messages to TM

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tm_handle_msgr_msg(void )
{

  uint32 msg_cnt = (uint32)q_cnt(&tm_msgr_rcv.msg_q);
  if (msg_cnt == 0)
  {
    return;
  }
  /* check the queue to see if this message is an OTA reset message */
  if (q_check(&tm_msgr_rcv.msg_q) != NULL)
  {
    tm_msgr_check_event();
  }
}
