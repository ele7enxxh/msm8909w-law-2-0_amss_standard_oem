/*======================================================================

               PDCOMM Application task implementation file

 GENERAL DESCRIPTION
  This file contains the implementation of PDCOMM Application task.
  This task handles requests from various clients like TM, calls
  appropriate data-service specific functions and sends back results to
  the client.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_wms/src/pd_comms_wms_task.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 14/03/16   kpn  Modify calls to APIs to support Multi-SIM functionality
 09/04/15   mj   Add as_id as part of the WMS ack message
 10/24/14   mj   Add condition to check for potential integer overflow case
 01/27/14   mj   Redesigned ACKing mechanism for WMS msgs
 07/30/13   mj   Reverting changes to consume and ACK WMS messages
 06/18/13   mj   Workaround for compiler bug that allocates variables multiple times on stack.
 			    Replacing three declaration of wms_ack_info_s_type to one at top of function.
 02/08/13   ssu  Move to the DOG Heartbeat mechanism on DIME and beyond.
 01/22/13   mj   Consume SMS/WAP message when it contains SUPL INIT for GW
 12/09/07   rh   Added external SMS injection support
 11/16/06   mr   Added code to start connection closing on receiving SSL alert of level FATAL or SUSPEND
 11/16/06   mr   Changed write params to include SMS params like mt address and teleservice id.
 11/16/06   mr   Changed SMS read msg cb to include decoded ts data and WAP flag
 11/14/06   mr   Fix for processing pending connect requests.
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#ifndef __GRIFFON__
#include "comdef.h"
#include "task.h"
#include "fs_public.h"
#include "msg.h"
#include "wms.h"
#include "secapi.h"

#include "secssl.h"
#include "secssldef.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "pd_comms_api.h"
#include "pd_comms_client_api.h"
#include "pd_comms_app_api.h"
#include "pd_comms_wms_local_api.h"
//#define extern
#include "pd_comms_wms_data.h"
//#undef extern
#include "gps_common.h"

/****************** Variables ************************************************/

static pd_comms_wms_task_handle_data_type z_handle_table[PDCOMM_MAX_HANDLES];
static wms_client_id_type u_wms_client_id;
static os_TimerMsgType* p_inactivity_timer;
static secapi_handle_type z_hash_handle_sha, z_hash_handle_sha256;
static os_MutexBlockType z_gethandle_mutex;
static boolean u_pd_comms_online;

static wms_client_ts_data_s_type z_client_ts_data;
static uint16 parsedMesgLen = 0;
static uint32 pd_comms_msg_consumed_client_id = 0;

/****************** Function Definitions**************************************/

/*===========================================================================
FUNCTION pd_comms_wms_task_startup

DESCRIPTION
  Contains task initialization code.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_startup( void )
{
/*Call all the initialization code here*/
 boolean ret = os_IpcStart((uint32)THREAD_ID_SM_PDCOMM_WMS);

 if(ret == FALSE)
 {
   MSG_ERROR("PDCOMM APP: IPC Start Failed",0,0,0);
 }

 ret = os_MutexInit(&z_gethandle_mutex, MUTEX_DATA_ONLY_CONTEXT);

 if(ret == FALSE)
 {
   MSG_ERROR("PDCOMM APP: getHandle Mutex Init Failed",0,0,0);
 }

  /* Register for DOG heartbeat monitoring */
  os_DogHeartbeatRegister();

  p_inactivity_timer = os_TimerCreate((uint32)PDCOMM_APP_TASK_INACTIVITY_TIMER,(uint32)THREAD_ID_SM_PDCOMM_WMS);

 u_pd_comms_online = TRUE;

} /*pd_comms_wms_task_startup() ends*/


/*===========================================================================
FUNCTION pd_comms_wms_task_register_callbacks

DESCRIPTION
  Registers callabcks with PDCOMM layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_register_callbacks(void)
{
  pd_comms_register_service_type z_reg_params;
  boolean u_result;

  /*Register SMS transport functions*/
  z_reg_params.e_link_type = LINK_SMS;
  z_reg_params.q_function_mask = (uint32)PD_COMMS_FUNC_CONNECT | (uint32)PD_COMMS_FUNC_DISCONNECT |
                                 (uint32)PD_COMMS_FUNC_IOCTL | (uint32)PD_COMMS_FUNC_GET_HANDLE |
                                 (uint32)PD_COMMS_FUNC_READ  | (uint32)PD_COMMS_FUNC_RELEASE_HANDLE |
                                 (uint32)PD_COMMS_FUNC_WRITE | (uint32)PD_COMMS_FUNC_GENERATE_HASH |
                                 (uint32)PD_COMMS_FUNC_INJECT_MSG;

  u_result = pd_comms_register_service(&z_reg_params,pd_comms_client_wms_get_handle_cb,
                                      pd_comms_client_wms_connect_cb,
                                      pd_comms_client_wms_disconnect_cb,NULL,pd_comms_client_wms_write_cb,
                                      pd_comms_client_generate_hash_cb,pd_comms_client_wms_ioctl_cb,
                                      pd_comms_client_wms_read_cb, pd_comms_client_wms_release_handle_cb,
                                      NULL, NULL, NULL,
                                      pd_comms_client_wms_inject_msg_cb);

  if(u_result == FALSE)
  {
    PDCOMM_APP_MSG_ERROR("Failed registering SMS services with PDCOMM layer",0,0,0);
    return;
  }
} /*pd_comms_wms_task_register_callbacks*/

/*===========================================================================
FUNCTION pd_comms_wms_task_release_handle

DESCRIPTION
  This function sets the values of handle related data in handle_table to its
  defaults so that handle can be reused

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_release_handle(
  /*handle for the handle to be released.*/
  uint32 q_handle_id)
{
    PDCOMM_APP_MSG_MED("Releasing handle [%d]",q_handle_id,0,0);
    memset(&z_handle_table[q_handle_id],0,sizeof(pd_comms_wms_task_handle_data_type));
    z_handle_table[q_handle_id].in_use                      = FALSE;
    z_handle_table[q_handle_id].client_disconnect_recvd     = FALSE;
    z_handle_table[q_handle_id].l_inactivity_count          = 0;
    z_handle_table[q_handle_id].session_aborted             = FALSE;
    z_handle_table[q_handle_id].u_connectPending            = FALSE;
} /*pd_comms_wms_task_release_handle() ends*/


/*===========================================================================
FUNCTION pd_comms_wms_task_disconnect_handle

DESCRIPTION
  This function sets the values of handle related data in handle_table to its
  defaults so that handle can be reused

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_disconnect_handle(
  /*handle for the handle to be released.*/
  uint32 q_handle_id)
{
    PDCOMM_APP_MSG_MED("Disconnecting handle [%d]",q_handle_id,0,0);
    z_handle_table[q_handle_id].client_disconnect_recvd     = FALSE;
    z_handle_table[q_handle_id].l_inactivity_count          = 0;
    z_handle_table[q_handle_id].session_aborted             = FALSE;
    z_handle_table[q_handle_id].p_read_ptr                  =
                               z_handle_table[q_handle_id].p_read_buff;
    z_handle_table[q_handle_id].u_multiPartActive           = FALSE;
    z_handle_table[q_handle_id].w_num_bytes_already_read    = 0;
    z_handle_table[q_handle_id].w_num_bytes_in_buffer       = 0;
    z_handle_table[q_handle_id].w_num_bytes_to_read         = 0;
    z_handle_table[q_handle_id].w_num_bytes_to_write        = 0;
    z_handle_table[q_handle_id].w_num_bytes_written         = 0;

} /*pd_comms_wms_task_disconnect_handle() ends*/


/*===========================================================================
FUNCTION pd_comms_wms_task_init_handle_table

DESCRIPTION
  This function is called after task start to initalize the values in
  handle_table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_init_handle_table( void )
{
  int l_i;

  for (l_i=0;l_i<PDCOMM_MAX_HANDLES;l_i++)
  {
    memset(&z_handle_table[l_i],0,sizeof(pd_comms_wms_task_handle_data_type));
    z_handle_table[l_i].in_use                      = FALSE;
    z_handle_table[l_i].e_link_status               = PDCOMM_STATE_DISCONNECTED;
    z_handle_table[l_i].client_disconnect_recvd     = FALSE;
    z_handle_table[l_i].l_inactivity_count          = 0;
    z_handle_table[l_i].session_aborted             = FALSE;
    z_handle_table[l_i].p_read_ptr = z_handle_table[l_i].p_read_buff;
    z_handle_table[l_i].u_multiPartActive = FALSE;
  }

} /*pd_comms_wms_task_init_handle_table() ends*/

/*===========================================================================
FUNCTION pd_comms_wms_task_reset_inactivity_count

DESCRIPTION
  This function will reset inactivity timer for a particular connection to 0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_reset_inactivity_count(
  /*handle_id*/
  uint32 q_handle_id)
{
  uint32 q_timer_left=0;

  z_handle_table[q_handle_id].l_inactivity_count =0;
  q_timer_left = os_TimerCheck(p_inactivity_timer);
  z_handle_table[q_handle_id].l_inactivity_count -=
                                      (int)(PDCOMM_INACTIVITY_TIMER_PERIOD-q_timer_left);

} /*pd_comms_wms_task_reset_inactivity_count*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_connect

DESCRIPTION
  This function handles an incoming PDCOMM_CONNECT request. It assigns a free
  handle for the connection and calls the link_type(TCP,SMS,HTTP etc.)
  specific connection function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_handle_connect(
  /*Pointer to the received PDCOMM_CONNECT request message*/
  const pd_comms_ipc_connect_hdr_type* p_hdr_ptr );

/*===========================================================================
FUNCTION pd_comms_wms_task_update_state

DESCRIPTION
  Updates the state of a particular handle and notifies the client if required.
  If the new state is DISCONNECTED and a connect request is pending, this function
  will also process the pending connection request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_update_state(
  /*handle corresponding to the connection for which state is to be updated*/
  pd_comms_handle_type z_handle,
  /*the new state of the connection*/
  pd_comms_state_type z_new_state,
  /*if false only update the internal state to new state
  if true also notify the client about the state change */
  boolean notify_client,
  /*incase client is to be notified about the state update, this
  argument contains the pdcomm result code or error code corresponding to
  reason for state change.*/
  uint32 q_result)
{
  pd_comms_state_type z_old_state =
                     z_handle_table[z_handle.q_handle_id].e_link_status;
  pd_comms_return_type e_result;

  z_handle_table[z_handle.q_handle_id].e_link_status = z_new_state;

  PDCOMM_APP_MSG_HIGH("Updating client's state to %d from %d",z_new_state,z_old_state,0);

  /*Update the inactivity timer*/
  pd_comms_wms_task_reset_inactivity_count(z_handle.q_handle_id);

  switch(z_new_state)
  {
    case PDCOMM_STATE_CONNECTED:
    {
      if(notify_client && z_old_state != PDCOMM_STATE_CONNECTED)
      {
        pd_comms_notification_params_type z_params;
        z_params.e_new_state = z_new_state;
        z_params.q_result = q_result;

        e_result = pd_comms_notify_client(z_handle,
                   z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                   z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                   z_handle_table[z_handle.q_handle_id].z_connect_params.q_transaction_id,
                   STATE_CHANGE,&z_params);
        if(e_result != PDCOMM_RESULT_OK)
        {
          MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                    (uint32)e_result,z_handle.q_handle_id,0);

        }

      }
      break;
    }/*case PDCOMM_STATE_CONNECTED*/

    case PDCOMM_STATE_DISCONNECTED:
    {
      if(z_old_state != PDCOMM_STATE_DISCONNECTED)
      {
        if(notify_client)
        {
          pd_comms_notification_params_type z_params;
          z_params.e_new_state = z_new_state;
          z_params.q_result = q_result;

          if(q_result == (uint32)PDCOMM_RESULT_WAIT ||
                                         q_result == (uint32)PDCOMM_RESULT_OK)
          {
            e_result = pd_comms_notify_client(z_handle,
                                 z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                 z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                 z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                                 STATE_CHANGE,&z_params);
          }
          else
          {
            e_result = pd_comms_notify_client(z_handle,
                                 z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                 z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                 z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                                 ERROR_RPT,&z_params);
          }

          if(e_result != PDCOMM_RESULT_OK)
          {
            MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                      (uint32)e_result,z_handle.q_handle_id,0);

          }
        }/*if(notify_client)*/

        /*re-initialize the handle data*/
        pd_comms_wms_task_disconnect_handle(z_handle.q_handle_id);

        if(z_handle_table[z_handle.q_handle_id].u_connectPending)
        {
          PDCOMM_APP_MSG_MED("Processing pending connection request for handle_id[%d]",
                             z_handle.q_handle_id,0,0);
          z_handle_table[z_handle.q_handle_id].u_connectPending = FALSE;
          pd_comms_wms_task_handle_connect(&z_handle_table[z_handle.q_handle_id].z_pendingConnectReq);
        }

      }/*if(z_old_state != PDCOMM_STATE_DISCONNECTED)*/

      break;
    }/*case PDCOMM_STATE_DISCONNECTED*/

    case PDCOMM_STATE_CONNECTING:
    {
      if(notify_client && z_old_state != PDCOMM_STATE_CONNECTING)
      {
        pd_comms_notification_params_type z_params;
        z_params.e_new_state = z_new_state;
        z_params.q_result = q_result;

        e_result = pd_comms_notify_client(z_handle,
                   z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                   z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                   z_handle_table[z_handle.q_handle_id].z_connect_params.q_transaction_id,
                   STATE_CHANGE,&z_params);
        if(e_result != PDCOMM_RESULT_OK)
        {
          MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                    (uint32)e_result,z_handle.q_handle_id,0);

        }
      }
      break;
    }/*case PDCOMM_STATE_CONNECTING*/

    case PDCOMM_STATE_OPENING:
    {
      /*Never notify client about this state*/
      break;
    }/*case PDCOMM_STATE_OPENING*/

    case PDCOMM_STATE_DISCONNECTING:
    {
      if(notify_client && z_old_state != PDCOMM_STATE_DISCONNECTING)
      {
        pd_comms_notification_params_type z_params;
        z_params.e_new_state = z_new_state;
        z_params.q_result = q_result;

        if(q_result == (uint32)PDCOMM_RESULT_WAIT ||
                                       q_result == (uint32)PDCOMM_RESULT_OK)
        {
          e_result = pd_comms_notify_client(z_handle,
                     z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                     z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                     z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                     STATE_CHANGE,&z_params);
        }
        else
        {
          e_result = pd_comms_notify_client(z_handle,
                     z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                     z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                     z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                     ERROR_RPT,&z_params);
        }

        if(e_result != PDCOMM_RESULT_OK)
        {
          MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                    (uint32)e_result,z_handle.q_handle_id,0);

        }

      }/*if(notify_client && z_old_state != PDCOMM_STATE_DISCONNECTING)*/
      break;
    }/*case PDCOMM_STATE_DISCONNECTING*/

    default:
    {
      MSG_ERROR( "Invalid State %d recevied for update", z_new_state, 0, 0);
    }
  }/* switch(z_new_state) ends */

}/* pd_comms_wms_task_update_state() ends*/


/*===========================================================================
FUNCTION pd_comms_wms_task_sms_connect

DESCRIPTION
  This function handles a SMS connect request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_sms_connect(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  /*Update the link state to CONNECTED and notify the client*/
  pd_comms_wms_task_update_state(z_handle,PDCOMM_STATE_CONNECTED,
                                 TRUE,(uint32) PDCOMM_RESULT_OK);


}/*pd_comms_wms_task_sms_connect*/

#ifdef FEATURE_CGPS_CDMA_IF
/*===========================================================================
FUNCTION pd_comms_wms_task_set_default_sms_write_params

DESCRIPTION
  This function sets the default sms write parameters for sending out SMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_set_default_sms_write_params(
  /*pointer to the wms_client_message_s_type to be sent out*/
  wms_client_message_s_type *p_msg,
  /*pointer to the wms_client_bd_s_type to be sent out*/
  wms_client_bd_s_type      *p_bd)
{

  if(p_msg == NULL || p_bd == NULL)
  {
    return;
  }

  p_bd->mask = WMS_MASK_BD_USER_DATA | WMS_MASK_BD_MSG_ID ;
  p_bd->message_id.type       = WMS_BD_TYPE_SUBMIT;
  p_bd->message_id.id_number  = 0;
  p_bd->user_data.encoding    = WMS_ENCODING_OCTET;
  p_bd->reply_option.user_ack_requested     = FALSE;
  p_bd->reply_option.delivery_ack_requested = FALSE;
  p_bd->user_data.is91ep_type = WMS_IS91EP_SHORT_MESSAGE_FULL;

  /* TL fields */
  p_msg->msg_hdr.index =        WMS_DUMMY_MESSAGE_INDEX;
  p_msg->msg_hdr.mem_store =    WMS_MEMORY_STORE_NONE;
  p_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  p_msg->msg_hdr.tag =          WMS_TAG_MT_NOT_READ;

  p_msg->u.cdma_message.subaddress.number_of_digits = 0;
  p_msg->u.cdma_message.is_tl_ack_requested = FALSE;


}/*pd_comms_wms_task_set_default_sms_write_params*/
#endif /*FEATURE_CGPS_CDMA_IF*/

/*===========================================================================
FUNCTION pd_comms_wms_task_sms_write

DESCRIPTION
  This function handles a SMS write request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_sms_write(
  /*handle for which data has to be written*/
  pd_comms_handle_type z_handle,
  /*sms specific write parameters */
  const pd_comms_sms_write_params_type *p_sms_write_params)
{
#ifdef FEATURE_CGPS_CDMA_IF

  wms_status_e_type e_status;
  wms_client_message_s_type z_client_sms;
  wms_client_bd_s_type      z_bd;

  PDCOMM_APP_MSG_MED("Sending MO SMS with for handle_id[%d]",z_handle.q_handle_id,0,0);

  pd_comms_wms_task_set_default_sms_write_params(&z_client_sms,&z_bd);

  /*Copy MT address*/
  memscpy((void*)&z_client_sms.u.cdma_message.address,
         sizeof(z_client_sms.u.cdma_message.address),
         (void*)&p_sms_write_params->z_address,
         sizeof(p_sms_write_params->z_address));

  /*Don't write more than SMS user data buffer*/
  /*Copy user data*/
  z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write =
    memscpy((void*)z_bd.user_data.data, WMS_CDMA_USER_DATA_MAX,
         (void*)z_handle_table[z_handle.q_handle_id].p_write_buff,
         z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write);

  z_bd.user_data.data_len = z_bd.user_data.number_of_digits =
  (uint8)z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write;
  z_client_sms.u.cdma_message.teleservice = p_sms_write_params->e_teleservice;
  z_bd.user_data.padding_bits = 0;

  e_status = wms_ts_encode_bearer_data(&z_bd,&z_client_sms.u.cdma_message.raw_ts);

  if(e_status == WMS_OK_S)
  {
    /* Use multi-SIM API with the cached as_id. */
    e_status = wms_msg_ms_send( u_wms_client_id, z_handle_table[z_handle.q_handle_id].e_as_id, NULL,
                             &z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id,
                             WMS_SEND_MODE_CLIENT_MESSAGE,
                             &z_client_sms);
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("encode bearer data failed while sending out SMS",0,0,0);
  }

  if( e_status != WMS_OK_S )
  {
    pd_comms_notification_params_type z_params;

    z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write =
    z_handle_table[z_handle.q_handle_id].w_num_bytes_written  =  0;

    PDCOMM_APP_MSG_ERROR("MOSMS send for handle[%d] failed error=%d",
                          z_handle.q_handle_id,e_status,0);
    z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;
    z_params.q_result = (uint32) PDCOMM_WRITE_ERROR_UNKNOWN;
    (void)pd_comms_notify_client(z_handle,
                           z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                           z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                           z_handle_table[z_handle.q_handle_id].q_write_transaction_id,
                           ERROR_RPT,&z_params);
  }/*if( e_status != WMS_OK_S )*/
#else
  PDCOMM_APP_MSG_ERROR("PDCOMM SMS write not supported for UMTS only build",0,0,0);
#endif

}/*pd_comms_wms_task_sms_write*/

/*===========================================================================
FUNCTION pd_comms_wms_task_sms_read

DESCRIPTION
  This function attempts to read one or more bytes from a SMS link

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean pd_comms_wms_task_sms_read(
  /*Handle of link to read from*/
  uint32 q_handle_id,
  /*wait for all data requested before sending it to client or not*/
  boolean u_wait_for_all )
{
  pd_comms_read_data_params_type z_params;
  uint16 *p_num_bytes_to_read;
  uint16 *p_num_bytes_already_read;
  uint16 *p_num_bytes_in_buffer;


  p_num_bytes_to_read = &z_handle_table[q_handle_id].w_num_bytes_to_read;
  p_num_bytes_already_read = &z_handle_table[q_handle_id].w_num_bytes_already_read;
  p_num_bytes_in_buffer = &z_handle_table[q_handle_id].w_num_bytes_in_buffer;

  /* Retrieve up to max_bytes from the socket */

  if(*p_num_bytes_to_read > *p_num_bytes_in_buffer)
  {
    *p_num_bytes_to_read = *p_num_bytes_in_buffer;
  }


  z_params.w_data_len = *p_num_bytes_to_read;
  z_params.p_data = z_handle_table[q_handle_id].p_read_ptr;

  PDCOMM_APP_MSG_MED("Sending %d bytes for handle_id [%d] to client",
                     *p_num_bytes_to_read,q_handle_id,0);
  (void)pd_comms_senddata_to_client(z_handle_table[q_handle_id].z_handle,
                              z_handle_table[q_handle_id].q_source_thread_id,
                              z_handle_table[q_handle_id].q_base_msg_id,
                              z_handle_table[q_handle_id].q_read_transaction_id,
                              &z_params);

  *p_num_bytes_in_buffer -= *p_num_bytes_to_read;

  if(*p_num_bytes_in_buffer == 0)
  {
    z_handle_table[q_handle_id].p_read_ptr = z_handle_table[q_handle_id].p_read_buff;
  }
  else
  {
    pd_comms_notification_params_type z_notif_params;
    pd_comms_return_type e_result;

    z_handle_table[q_handle_id].p_read_ptr += *p_num_bytes_to_read;
    /*Clients always waits for a data availability notification
      from PDCOMM APP before issuing a read. For this reason if after read data
      is sent to client, there is still more data available in buffer, send
      another data availability notification to the client */


     z_notif_params.e_new_state = z_handle_table[q_handle_id].e_link_status;
     e_result = pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                                       z_handle_table[q_handle_id].q_source_thread_id,
                                       z_handle_table[q_handle_id].q_base_msg_id,
                                       z_handle_table[q_handle_id].z_connect_params.q_transaction_id,
                                       READ_DATA,&z_notif_params);

     if(e_result != PDCOMM_RESULT_OK)
     {
       MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d] while sending read_data notification",
                 (uint32)e_result,q_handle_id,0);

     }

  }/* else for   if(*p_num_bytes_in_buffer == 0)*/

  *p_num_bytes_already_read = *p_num_bytes_to_read = 0;

  return TRUE;

} /*lint !e715 */ /*pd_comms_wms_task_sms_read*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_sms_msg_cb

DESCRIPTION
  Whenever there is a SMS message callback with an event WMS_MSG_EVENT_RECEIVED_MESSAGE
  and the incoming message belongs to GPS, an IPC message is sent to PDCOMM APP.
  This IPC message is handled in this function.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
// static void pd_comms_wms_task_handle_sms_msg_cb(
//   /*Pointer to IPC message containing the SMS message callback information */
//   os_IpcMsgType* p_msg )
// {
//   uint8 u_mesgLen;
//   boolean u_finalSegment = TRUE;
//   boolean u_firstSegment = TRUE;
//   uint16 *w_bytes_already_read;
//   char *p_mesgContent;
//   uint32 i;
//
//   if(p_msg == NULL)
//   {
//     return;
//   }
//   /*lint -e826 */
//   i = ((pd_comms_wms_task_sms_msg_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
//   u_finalSegment = ((pd_comms_wms_task_sms_msg_cb_ipc_hdr_type*)p_msg->p_Data)->u_finalSegment;
//   u_firstSegment = ((pd_comms_wms_task_sms_msg_cb_ipc_hdr_type*)p_msg->p_Data)->u_firstSegment;
//   u_mesgLen = ((pd_comms_wms_task_sms_msg_cb_ipc_hdr_type*)p_msg->p_Data)->u_mesgLen;
//   p_mesgContent = ((pd_comms_wms_task_sms_msg_cb_ipc_hdr_type*)p_msg->p_Data)->p_data;
//   /*lint +e826 */
//
//   PDCOMM_APP_MSG_MED("Received SMS message callback for handle_id [%d], finalSegment=%d, newSegment=%d",
//                       i,u_finalSegment,u_firstSegment);
//
//   if( i < PDCOMM_MAX_HANDLES )
//   {
//     w_bytes_already_read = &z_handle_table[i].w_num_bytes_in_buffer;
//
//     if(u_mesgLen == 0)
//     {
//        PDCOMM_APP_MSG_ERROR("NULL message content received in SMS read msg cb for handle_id[%d]",
//                              i,0,0);
//
//        if((u_finalSegment == TRUE) && z_handle_table[i].u_multiPartActive)
//        {
//            /*This indicates that the current segments should be dropped*/
//           z_handle_table[i].p_read_ptr = z_handle_table[i].p_read_buff;
//           *w_bytes_already_read = 0;
//           z_handle_table[i].u_multiPartActive = FALSE;
//           PDCOMM_APP_MSG_ERROR("finalSegment indication received. Dropping previous segments",0,0,0);
//
//        }
//     }
//     else
//     {
//       if(u_firstSegment == TRUE)
//       {
//         /*If this is the first segment and there was a multipart message wait going on,
//           disacrd the previous segments and start with this new message*/
//         if(z_handle_table[i].u_multiPartActive)
//         {
//           z_handle_table[i].p_read_ptr = z_handle_table[i].p_read_buff;
//           *w_bytes_already_read = 0;
//           PDCOMM_APP_MSG_ERROR("firstSegment indication received. Dropping previous segments",0,0,0);
//         }
//
//         z_handle_table[i].u_multiPartActive = TRUE;
//
//       }
//       else if(!z_handle_table[i].u_multiPartActive)
//       {
//         /*If this is not a first segment and there was no multipart active,
//          drop this message*/
//         PDCOMM_APP_MSG_ERROR("Non-first segment received while multipart is not active. Dropping message",0,0,0);
//         return;
//       }
//
//       if( (z_handle_table[i].p_read_ptr + *w_bytes_already_read + u_mesgLen) >
//           (z_handle_table[i].p_read_buff + PDCOMM_READ_DATA_BUF_SIZE) )
//       {
//         PDCOMM_APP_MSG_ERROR("Buffer not available for incoming SMS. Dropping message handle_id[%d]",
//                               i,0,0);
//         if(u_firstSegment == TRUE)
//         {
//           /*Don't start the multipart message collection if this is the very first segment
//             and we don't have more space*/
//           z_handle_table[i].u_multiPartActive = FALSE;
//         }
//
//         if(z_handle_table[i].u_multiPartActive)
//         {
//           z_handle_table[i].p_read_ptr = z_handle_table[i].p_read_buff;
//           *w_bytes_already_read = 0;
//           PDCOMM_APP_MSG_ERROR("Buffer not available when multipart active. Dropping previous segments",0,0,0);
//         }
//
//         return;
//
//       }
//
//       memcpy((z_handle_table[i].p_read_ptr + *w_bytes_already_read),p_mesgContent,u_mesgLen);
//       *w_bytes_already_read += u_mesgLen;
//
//       if(u_finalSegment) /*If this is the final segment of a multipart SMS message inform TM*/
//       {
//         /*Notify client about the read data availability*/
//         pd_comms_notification_params_type z_params;
//         pd_comms_return_type e_result;
//
//
//         z_params.e_new_state = z_handle_table[i].e_link_status;
//         e_result = pd_comms_notify_client(z_handle_table[i].z_handle,
//                                           z_handle_table[i].q_source_thread_id,
//                                           z_handle_table[i].q_base_msg_id,
//                                           z_handle_table[i].z_connect_params.q_transaction_id,
//                                           READ_DATA,&z_params);
//
//         if(e_result != PDCOMM_RESULT_OK)
//         {
//           MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
//                     (uint32)e_result,i,0);
//
//         }
//         z_handle_table[i].u_multiPartActive = FALSE;
//
//       }
//     }
//
//   }
//   else
//   {
//     MSG_ERROR("SMS message callback: handle_id %d out of range", i, 0, 0);
//
//   }
//
// } /* pd_comms_wms_task_handle_sock_cb() ends */


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_sms_msg_cb

DESCRIPTION
  Whenever there is a SMS message callback with an event WMS_MSG_EVENT_RECEIVED_MESSAGE
  and the incoming message belongs to GPS, an IPC message is sent to PDCOMM APP.
  This IPC message is handled in this function.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_handle_sms_msg_cb(
  /*Pointer to IPC message containing the SMS message callback information */
  os_IpcMsgType* p_msg )
{
  uint16 w_mesgLen;
  uint32 i;

  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */
  i = ((pd_comms_wms_task_sms_msg_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  /*lint +e826 */

  PDCOMM_APP_MSG_MED("Received SMS message callback for handle_id [%d]",
                      i,0,0);

  if( i < PDCOMM_MAX_HANDLES )
  {
    w_mesgLen = z_handle_table[i].w_num_bytes_in_buffer;

    if(w_mesgLen == 0)
    {
       PDCOMM_APP_MSG_ERROR("NULL message content received in SMS read msg cb for handle_id[%d]",
                             i,0,0);
    }
    else
    {
      /*Notify client about the read data availability*/
      pd_comms_notification_params_type z_params;
      pd_comms_return_type e_result;

      z_params.e_new_state = z_handle_table[i].e_link_status;
      e_result = pd_comms_notify_client(z_handle_table[i].z_handle,
                                        z_handle_table[i].q_source_thread_id,
                                        z_handle_table[i].q_base_msg_id,
                                        z_handle_table[i].z_connect_params.q_transaction_id,
                                        READ_DATA,&z_params);

      if(e_result != PDCOMM_RESULT_OK)
      {
        MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                  (uint32)e_result,i,0);

      }
    }

  }/*if( i < PDCOMM_MAX_HANDLES )*/
  else
  {
    MSG_ERROR("SMS message callback: handle_id %d out of range", i, 0, 0);

  }

} /* pd_comms_wms_task_handle_sms_msg_cb() ends */


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_inject_msg

DESCRIPTION
  TM Task will send PDCOMM WMS task IPC to inject external SMS.
  This IPC message is handled in this function.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_handle_inject_msg(
  /*Pointer to IPC message containing the SMS message callback information */
  os_IpcMsgType* p_msg )
{
  pd_comms_ipc_inject_msg_hdr_type* pInjectMsgIpc;
  uint16 *pBufLen;
  uint32 msg_len, i;

  if(p_msg == NULL)
  {
    return;
  }

  /*lint -e826 */
  pInjectMsgIpc = (pd_comms_ipc_inject_msg_hdr_type*)p_msg->p_Data;
  i = pInjectMsgIpc->z_handle.q_handle_id;
  /*lint +e826 */

  msg_len = pInjectMsgIpc->z_params.q_msg_len;
  PDCOMM_APP_MSG_MED("Received Inject MSG(len=%d) callback for handle_id [%d]", msg_len,i,0);
  if (msg_len == 0 || msg_len > PDCOMM_READ_DATA_BUF_SIZE || pInjectMsgIpc->z_params.p_msg == NULL) // 1KB
  {
    PDCOMM_APP_MSG_ERROR("Received invalid injected msg(len=%d data=%x) for handle_id[%d]", 
                         msg_len, pInjectMsgIpc->z_params.p_msg, i);
    return;  // will not free the buffer since len/pointer is invalid! (something is wrong)
  }

  if( i < PDCOMM_MAX_HANDLES )
  {
    pBufLen = &(z_handle_table[i].w_num_bytes_in_buffer);

    // if over 1KB, ignore what's in WMS buffer now, otherwise, append at the end
	if ((*pBufLen + msg_len > PDCOMM_READ_DATA_BUF_SIZE) || (*pBufLen + msg_len < msg_len))	
    {
      PDCOMM_APP_MSG_ERROR("Current WMS buffer len=%d, will overflow after injection(max=%d)!", msg_len,PDCOMM_READ_DATA_BUF_SIZE,0);
      // we will ignore the injected MSG?
    }
    else
    {
      // copy the injected msg to PDCOMM WMS's read buffer
      memscpy((void *)&(z_handle_table[i].p_read_buff[*pBufLen]), msg_len, (void *)(pInjectMsgIpc->z_params.p_msg), msg_len);
      *pBufLen += msg_len; // won't be 0
  
      if (*pBufLen > 0)
      {
        /*Notify client about the read data availability*/
        pd_comms_notification_params_type z_params;
        pd_comms_return_type e_result;
  
        z_params.e_new_state = z_handle_table[i].e_link_status;
        e_result = pd_comms_notify_client(z_handle_table[i].z_handle,
                                          z_handle_table[i].q_source_thread_id,
                                          z_handle_table[i].q_base_msg_id,
                                          z_handle_table[i].z_connect_params.q_transaction_id,
                                          READ_DATA,&z_params);
  
        if(e_result != PDCOMM_RESULT_OK)
        {
          MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",  (uint32)e_result,i,0);
  
        }
      }
    }

  }/*if( i < PDCOMM_MAX_HANDLES )*/
  else
  {
    MSG_ERROR("Inject message callback: handle_id %d out of range", i, 0, 0);

  }

  if (pInjectMsgIpc->z_params.p_msg != NULL)
  {
    os_MemFree((void**)&pInjectMsgIpc->z_params.p_msg); // need to free it 
	pInjectMsgIpc->z_params.p_msg = NULL;
  }

} /* pd_comms_wms_task_handle_inject_msg() ends */


/*===========================================================================
FUNCTION pd_comms_wms_task_sms_parse_msg_callback

DESCRIPTION
  This function is called by WMS when an SMS message is received. This function checks the
  message parameters to determine if this message is for us. If so the message is consumed
  ( not shared with any other modules ) otherwise it is passed on to the next module in the
  priority list.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_sms_parse_msg_callback(
  /*Pointer to received message*/
  const wms_client_message_s_type *p_msg,
  /*Set to TRUE by this function if we are not going to consume the message otherwise
   it is set to FALSE*/
  boolean *shared )
{
  uint32 i;
  pd_comms_wms_task_sms_msg_cb_ipc_hdr_type* p_hdr_ptr;
  os_IpcMsgType* p_ipc_ptr;
  boolean u_isWapPush = FALSE;
  uint8* p_parsedMesgContent = NULL;

  if(p_msg == NULL || shared == NULL)
  {
    return;
  }
  *shared = TRUE;

  PDCOMM_APP_MSG_MED("SMS parse message callback received",0,0,0);

  /* Message is WAP Push or not*/
  u_isWapPush =  wms_cfg_check_wap_push_message(p_msg);
      
  memset (&z_client_ts_data,0,sizeof(z_client_ts_data));
      
  /* Retrieve User Data */
  if(p_msg->msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
  {
    if(wms_ts_decode(&p_msg->u.gw_message.raw_ts_data, &z_client_ts_data ) != WMS_OK_S)
    {
      PDCOMM_APP_MSG_ERROR("SMS parse callback:TS DECODE failed for GW" ,0,0,0);
      return;
    }
  }
  else if (p_msg->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA)
  {
    if(wms_ts_decode(&p_msg->u.cdma_message.raw_ts, &z_client_ts_data) != WMS_OK_S)
    {
      PDCOMM_APP_MSG_ERROR("SMS parse callback:TS DECODE failed for CDMA" ,0,0,0);
      return;
    }
  }
  else
  { 
    /* Not expected to happen */
    PDCOMM_APP_MSG_ERROR("Unsupported message mode of %d in header" ,(uint32)p_msg->msg_hdr.message_mode,0,0);
	return;
  }	

  for(i=0;i<PDCOMM_MAX_HANDLES;i++)
  {
    /*call the parse message callbacks for all the connected SMS handles. If any callback
      return true, the message belongs to us*/
    if( z_handle_table[i].in_use &&
       (z_handle_table[i].z_connect_params.e_link_type == LINK_SMS) &&
       (z_handle_table[i].e_link_status == PDCOMM_STATE_CONNECTED) &&
       (z_handle_table[i].z_connect_params.z_link_params.z_sms_params.p_parse_cb_func != NULL) )
    {
      p_parsedMesgContent = z_handle_table[i].p_read_buff;
	  
      if(z_handle_table[i].z_connect_params.z_link_params.z_sms_params.p_parse_cb_func(
                                                                           p_msg,
                                                                           &z_client_ts_data,
                                                                           p_parsedMesgContent,
                                                                           &parsedMesgLen,
                                                                           (uint16)sizeof(z_handle_table[i].p_read_buff),
                                                                           u_isWapPush))
      {
        PDCOMM_APP_MSG_MED("SMS message of size %d belongs to handle_id[%d]",parsedMesgLen,i,0);

        z_handle_table[i].w_num_bytes_in_buffer = parsedMesgLen;

		/* Save the pd_comms client index so that we can assign the transaction id in sms_msg_callback */
		pd_comms_msg_consumed_client_id = i;
		
		if(parsedMesgLen > 0)
        {
          p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_wms_task_sms_msg_cb_ipc_hdr_type),
                               IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_WMS);

          if(p_ipc_ptr == NULL)
          {
            PDCOMM_APP_MSG_HIGH("IPC Create failed during SMS mesg callback for size %d. Handle Id: %d",
                                p_parsedMesgContent,i,0);
          }
          else
          {
            /*lint -e826 */
            p_hdr_ptr = (pd_comms_wms_task_sms_msg_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
            /*lint +e826 */
            p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_SMS_MSG_CB;
            p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_WMS;
            p_hdr_ptr->handle.q_handle_id = i;
            p_hdr_ptr->handle.e_link_type = LINK_SMS;

            if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_WMS))
            {
              PDCOMM_APP_MSG_HIGH("IPC Send failed during SMS MSG callback. Handle Id: %d",
                                  i,0,0);
              (void)os_IpcDelete(p_ipc_ptr);
            }
          }
        }
		
		/* Consume the message*/
        *shared = FALSE;
		return;
      }
    }
  }/*for(i=0;i<PDCOMM_MAX_HANDLES;i++) ends*/
}/*pd_comms_wms_task_sms_parse_msg_callback*/


/*===========================================================================
FUNCTION pd_comms_wms_task_sms_msg_callback

DESCRIPTION
  This function is called by WMS to process a message that we indicated we wished to
  consume in the parse callback. Since we process the message in the parse callback this
  function is empty.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_sms_msg_callback(
  /*Message Event*/
  wms_msg_event_e_type e_event,
  /*Message Information*/
  wms_msg_event_info_s_type *p_info,
  /*Set to FALSE if we wish to consume the message*/
  boolean *shared )
{
  uint32 i;
  wms_ack_info_s_type z_ack_info;
  wms_status_e_type wms_ack_status;

  if(p_info == NULL)
  {
    return;
  }

  switch(e_event)
  {
    case WMS_MSG_EVENT_RECEIVED_MESSAGE:
    {
      PDCOMM_APP_MSG_MED("SMS message callback received with event WMS_MSG_EVENT_RECEIVED_MESSAGE",0,0,0);
	  /* Check if GPS is the client to parse this message */
	  if (p_info->mt_message_info.client_id != WMS_CLIENT_TYPE_GPS)
	  {
	    PDCOMM_APP_MSG_MED("SMS message does not belong to GPS client, but belongs to client = %d " ,(uint32)p_info->mt_message_info.client_id,0,0);
        return;
	  }

	  /* Memset WMS ACK stuct */
      memset (&z_ack_info,0,sizeof(z_ack_info));
		  
      if(parsedMesgLen == 0)
      {
        PDCOMM_APP_MSG_MED("NULL message content received in SMS read msg cb for handle_id[%d]",
                                 (uint32)pd_comms_msg_consumed_client_id,0,0);
      }	

	  /* We should ACK even though parsedMesgLen is 0 because this msg belongs to us. 
		 WMS will call this sms msg callback only if we set shared == FALSE at parse msg callback */

      if((p_info->mt_message_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) &&
         (p_info->mt_message_info.message.u.cdma_message.is_tl_ack_requested) )
      {
        z_handle_table[pd_comms_msg_consumed_client_id].q_sms_transaction_id = p_info->mt_message_info.transaction_id;

		/* Cache as_id so as to use it any time during the session. */
		z_handle_table[pd_comms_msg_consumed_client_id].e_as_id = p_info->mt_message_info.as_id;

		/* Check if an ACK needs to be sent by GPS client*/
		if (p_info->mt_message_info.route == WMS_ROUTE_TRANSFER_ONLY)
	  	{
	  	  PDCOMM_APP_MSG_MED("GPS client sending TL ACK with transaction_id [%d], as_id [%d]",
				p_info->mt_message_info.transaction_id, p_info->mt_message_info.as_id, 0);

          z_ack_info.message_mode = WMS_MESSAGE_MODE_CDMA;
          z_ack_info.transaction_id = p_info->mt_message_info.transaction_id;
          z_ack_info.u.cdma.error_class = WMS_ERROR_NONE;
		  z_ack_info.as_id = p_info->mt_message_info.as_id;

          wms_ack_status = wms_msg_ms_ack(u_wms_client_id, z_ack_info.as_id, NULL,
                           &z_handle_table[pd_comms_msg_consumed_client_id].z_handle.q_handle_id, &z_ack_info );

		  if (wms_ack_status != WMS_OK_S)
		  {
			PDCOMM_APP_MSG_ERROR("WMS ACK failed for CDMA with error code = %d",(uint32)wms_ack_status,0,0);
		  }  
		}  
		else
		{
		  PDCOMM_APP_MSG_MED("ACKing not required for CDMA. Message contains route info = %d",
						(uint32)p_info->mt_message_info.route,0,0);
		}  
      }
      else
      {
        if(p_info->mt_message_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_GW)
		{
		  z_handle_table[pd_comms_msg_consumed_client_id].q_sms_transaction_id = p_info->mt_message_info.transaction_id;

		  /* Cache as_id so as to use it any time during the session. */
		  z_handle_table[pd_comms_msg_consumed_client_id].e_as_id = p_info->mt_message_info.as_id;

		  /* Check if an ACK needs to be sent by GPS client*/
		  if (p_info->mt_message_info.route == WMS_ROUTE_TRANSFER_ONLY)
	  	  {
	  		PDCOMM_APP_MSG_MED("GPS client sending ACK with transaction_id [%d], as_id [%d]",
			    	p_info->mt_message_info.transaction_id, p_info->mt_message_info.as_id, 0);
  
    	 	z_ack_info.message_mode = WMS_MESSAGE_MODE_GW;
			z_ack_info.transaction_id = p_info->mt_message_info.transaction_id;
			z_ack_info.u.gw.success = TRUE;
			z_ack_info.u.gw.tpdu_type = WMS_TPDU_DELIVER_REPORT_ACK;
			z_ack_info.u.gw.u.ack.user_data_header_present = FALSE;
			z_ack_info.u.gw.u.ack.user_data.sm_len = 0;
			z_ack_info.as_id = p_info->mt_message_info.as_id;

			/* Use Multi-SIM API call with as_id. */
			wms_ack_status = wms_msg_ms_ack(u_wms_client_id, z_ack_info.as_id, NULL,
				      &z_handle_table[pd_comms_msg_consumed_client_id].z_handle.q_handle_id, &z_ack_info );
				
			if (wms_ack_status != WMS_OK_S)
			{
			  PDCOMM_APP_MSG_ERROR("WMS ACK failed with error code = %d",(uint32)wms_ack_status,0,0);
			}  
		  }  
		  else
		  {
			PDCOMM_APP_MSG_MED("ACKing not required. Message contains route info = %d",
			  			(uint32)p_info->mt_message_info.route,0,0);
		  }  
							
		}
      }	
	
      break;
    } /*case WMS_MSG_EVENT_RECEIVED_MESSAGE: ends*/

    case WMS_MSG_EVENT_SUBMIT_REPORT:
    {
      PDCOMM_APP_MSG_MED("SMS message callback received with event WMS_MSG_EVENT_SUBMIT_REPORT",0,0,0);

      if((int)p_info->submit_report_info.client_id == (int)u_wms_client_id)
      {
        if(p_info->submit_report_info.user_data == NULL)
        {
          PDCOMM_APP_MSG_ERROR("SMS message callback received with NULL user data",0,0,0);
          return;
        }

        i = *((uint32*)p_info->submit_report_info.user_data);

        if(i >= PDCOMM_MAX_HANDLES)
        {
          PDCOMM_APP_MSG_ERROR("SMS message callback received with invalid handle_id [%d]",i,0,0);
          return;
        }

        if( z_handle_table[i].in_use &&
           (z_handle_table[i].z_connect_params.e_link_type == LINK_SMS) &&
           (z_handle_table[i].e_link_status == PDCOMM_STATE_CONNECTED))
        {
          switch(p_info->submit_report_info.report_status)
          {
            case WMS_RPT_OK:
            {
              /*Send a write ACK to TM*/
              pd_comms_write_ack_params_type z_params;

              z_params.q_bytes_written = z_handle_table[i].w_num_bytes_to_write;
              z_params.q_transaction_id = z_handle_table[i].q_write_transaction_id;

              /* Acknowledge the write */
              (void)pd_comms_send_write_ack_to_client(z_handle_table[i].z_handle,
                                                z_handle_table[i].q_source_thread_id,
                                                z_handle_table[i].q_base_msg_id,
                                                &z_params);

              z_handle_table[i].w_num_bytes_to_write =
              z_handle_table[i].w_num_bytes_written  =  0;

              break;
            }

            default:
            {
              /*Send a error report to TM*/
              pd_comms_notification_params_type z_params;
              z_params.e_new_state = z_handle_table[i].e_link_status;

              z_params.q_result = (uint32) PDCOMM_WRITE_ERROR_UNKNOWN;
              (void)pd_comms_notify_client(z_handle_table[i].z_handle,
                                     z_handle_table[i].q_source_thread_id,
                                     z_handle_table[i].q_base_msg_id,
                                     z_handle_table[i].q_write_transaction_id,
                                     ERROR_RPT,&z_params);

              z_handle_table[i].w_num_bytes_to_write =
              z_handle_table[i].w_num_bytes_written  =  0;

              break;
            }

          } /*switch(report_status) ends*/
        }/*if( z_handle_table[i].in_use && (z_handle_table[i].z_connect_params.e_link_type == LINK_SMS) &&
           (z_handle_table[i].e_link_status == PDCOMM_STATE_CONNECTED))*/

      }/*if((int)p_info->submit_report_info.client_id == (int)u_wms_client_id)*/

      break;
    } /*case WMS_MSG_EVENT_SUBMIT_REPORT: ends*/

    case WMS_MSG_EVENT_ACK_REPORT:
    {
      pd_comms_sms_ack_report_params_type z_params;

      PDCOMM_APP_MSG_MED("SMS message callback received with event WMS_MSG_EVENT_ACK_REPORT",0,0,0);

      if(p_info->ack_report_info.client_id == u_wms_client_id)
      {

        if(p_info->ack_report_info.user_data == NULL)
        {
          PDCOMM_APP_MSG_ERROR("SMS message callback received with NULL user data",0,0,0);
          return;
        }

        i = *((uint32*)p_info->ack_report_info.user_data);

        if(i >= PDCOMM_MAX_HANDLES)
        {
          PDCOMM_APP_MSG_ERROR("SMS message callback received with invalid handle_id [%d]",i,0,0);
          return;
        }

        if(p_info->ack_report_info.transaction_id == z_handle_table[i].q_sms_transaction_id )
        {
          if(p_info->ack_report_info.is_success)
          {
            PDCOMM_APP_MSG_MED("TL ACK successfully sentout for handle_id[%d] and transaction_id[%d]",
                               i,p_info->ack_report_info.transaction_id,0);
          }
          else
          {
            PDCOMM_APP_MSG_MED("Error sending TL ACK for handle_id[%d] and transaction_id[%d]",
                               i,p_info->ack_report_info.transaction_id,0);
          }

           /*Send a SMS ACK REPORT to TM*/
          if( z_handle_table[i].in_use &&
             (z_handle_table[i].z_connect_params.e_link_type == LINK_SMS) &&
             (z_handle_table[i].e_link_status == PDCOMM_STATE_CONNECTED))
          {
            z_params.q_transaction_id = z_handle_table[i].q_read_transaction_id;
            z_params.u_success = p_info->ack_report_info.is_success;
            (void)pd_comms_send_sms_ack_report_to_client(z_handle_table[i].z_handle,
                                                  z_handle_table[i].q_source_thread_id,
                                                  z_handle_table[i].q_base_msg_id,
                                                  &z_params);
          }
		  /* Claim the report ACK report */
		  *shared = FALSE;

        }/*if(p_info->ack_report_info.transaction_id == z_handle_table[i].q_sms_transaction_id )*/
      }/*if(p_info->ack_report_info.client_id == u_wms_client_id)*/

      break;
    } /*case WMS_MSG_EVENT_ACK_REPORT: ends*/

    default:
    {
      PDCOMM_APP_MSG_HIGH("Event %d not handled for sms message callback",(uint32)e_event,0,0);
      break;
    }

  }/*switch(e_event) ends*/

} /*lint !e715 !e818 */ /*pd_comms_wms_task_sms_msg_callback*/


/*===========================================================================
FUNCTION pd_comms_wms_task_sms_init

DESCRIPTION
  This function registers with the WMS system inorder to receive SMS messages.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_sms_init(void)
{
  /* Register with WMS for reception of WAP Push messages */
  if( wms_client_init( WMS_CLIENT_TYPE_GPS, &u_wms_client_id ) != WMS_CLIENT_ERR_NONE )
  {
    PDCOMM_APP_MSG_ERROR("Error Initializing WMS API",0,0,0);
  }

  PDCOMM_APP_MSG_HIGH("Registered with WMS. wms_client_id [%d]",u_wms_client_id,0,0);

  if( wms_client_reg_parse_msg_cb( (int)WMS_CLIENT_TYPE_GPS, pd_comms_wms_task_sms_parse_msg_callback ) !=
       WMS_CLIENT_ERR_NONE )
  {
    PDCOMM_APP_MSG_ERROR("Could not register WMS parse msg callback",0,0,0);
  }

  if( wms_client_reg_msg_cb( (int)WMS_CLIENT_TYPE_GPS, pd_comms_wms_task_sms_msg_callback ) !=
       WMS_CLIENT_ERR_NONE )
  {
    PDCOMM_APP_MSG_ERROR("Could not register WMS msg callback",0,0,0);
  }

  if( wms_client_activate( (int)WMS_CLIENT_TYPE_GPS ) != WMS_CLIENT_ERR_NONE )
  {
    PDCOMM_APP_MSG_ERROR("Error Activating WMS Client",0,0,0);
  }

#if 0
  if(wms_msg_set_retry_period((int)WMS_CLIENT_TYPE_GPS,PDCOMM_MO_SMS_RETRY_PERIOD) != WMS_OK_S)
  {
    PDCOMM_APP_MSG_ERROR("Set WMS retry period failed",0,0,0);
  }

  if(wms_dbg_set_retry_interval((int)WMS_CLIENT_TYPE_GPS,NULL,NULL,PDCOMM_MO_SMS_RETRY_INTERVAL) != WMS_OK_S)
  {
    PDCOMM_APP_MSG_ERROR("Set WMS retry interval failed",0,0,0);
  }
#endif /*0*/

} /*pd_comms_wms_task_sms_init*/


/*===========================================================================
FUNCTION pd_comms_wms_task_http_connect

DESCRIPTION
  This function handles a HTTP connect request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_http_connect(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
   return;

} /*lint !e715 */ /*pd_comms_wms_task_http_connect*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_get_handle

DESCRIPTION
  This function handles an incoming get_handle request. It assigns an free
  handle for the connection

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type  pd_comms_wms_task_handle_get_handle(
  /*contains the get_handle params*/
  const pd_comms_get_handle_params_type* const p_params,
  /*the assigned handle is returned in this variable */
  pd_comms_handle_type* p_handle  )
{
  uint32 l_i;

  if(p_handle == NULL || p_params == NULL)
  {
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  /*Get the mutex lock since get_handle could be called simultaneously by
    two PDCOMM clients*/
  MSG_MED("PDCOMM APP:Trying to acquire GetHandle Mutex lock",0,0,0);
  os_MutexLock(&z_gethandle_mutex);
  MSG_MED("PDCOMM APP:GetHandle Mutex lock acquired",0,0,0);

  for(l_i=0; l_i<PDCOMM_MAX_HANDLES ;l_i++)
  {
    if(!z_handle_table[l_i].in_use)
    {
      break;
    }
  }

  if(l_i >= PDCOMM_MAX_HANDLES)
  {
    MSG_ERROR("************ No more handles available *************",0,0,0);
    os_MutexUnlock(&z_gethandle_mutex);
    MSG_MED("PDCOMM APP:GetHandle Mutex unlocked",0,0,0);
    return PDCOMM_RESULT_ERROR_NO_MORE_HANDLES;
  }

  /*Save all the general data for handle in handle_table*/
  z_handle_table[l_i].in_use = TRUE;
  p_handle->q_handle_id = l_i;
  p_handle->e_link_type = p_params->e_link_type;
  z_handle_table[l_i].z_handle = *p_handle;

  os_MutexUnlock(&z_gethandle_mutex);
  MSG_MED("PDCOMM APP:GetHandle Mutex unlocked",0,0,0);

  return PDCOMM_RESULT_OK;
}/*pd_comms_wms_task_handle_get_handle*/



/*===========================================================================
FUNCTION pd_comms_wms_task_handle_connect

DESCRIPTION
  This function handles an incoming PDCOMM_CONNECT request. It assigns a free
  handle for the connection and calls the link_type(TCP,SMS,HTTP etc.)
  specific connection function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_handle_connect(
  /*Pointer to the received PDCOMM_CONNECT request message*/
  const pd_comms_ipc_connect_hdr_type* p_hdr_ptr )
{
  uint32 l_i;
  pd_comms_handle_type z_handle;

  if(p_hdr_ptr ==  NULL)
  {
    return;
  }

  z_handle = p_hdr_ptr->z_handle;
  l_i = p_hdr_ptr->z_handle.q_handle_id;

  PDCOMM_APP_MSG_LOW("Connect request received for handle_id[%d]",l_i,0,0);

  if(l_i >= PDCOMM_MAX_HANDLES || z_handle_table[l_i].in_use == FALSE)
  {
    pd_comms_notification_params_type z_params;

    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_CONNECT_ERROR_INVALID_PARAMETERS;

    z_handle.q_handle_id = 0;
    z_handle.e_link_type = p_hdr_ptr->z_params.e_link_type;

    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->z_params.q_transaction_id,
                           ERROR_RPT,&z_params);
    MSG_ERROR("PDCOMM_TASK: ************INVALID handle received *************",0,0,0);

    return;
  }

  if(z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTED)
  {
    /*Save all the general data for handle in handle_table*/
    z_handle_table[l_i].z_connect_params = p_hdr_ptr->z_params;
    z_handle_table[l_i].q_source_thread_id = p_hdr_ptr->q_source_thread_id;
    z_handle_table[l_i].q_base_msg_id      = p_hdr_ptr->q_base_msg_id;
    z_handle_table[l_i].z_handle = z_handle;
    z_handle_table[l_i].u_connectPending  = FALSE;

    /*Set the disconnect_transaction_id to the transaction_id received
     *in connect request so that when pdcomm_app_task_update_state() is
     *called before client disconnect is recevied, (because of some error)
     *the same transaction id as the one used in connect request be used to
     *notify client*/

    z_handle_table[l_i].q_disconnect_transaction_id = p_hdr_ptr->z_params.q_transaction_id;

    switch(p_hdr_ptr->z_params.e_link_type)
    {
      case LINK_TCP:
      {
        PDCOMM_APP_MSG_ERROR("Connect request for TCP link received by WMS-PDCOMM task",0,0,0);
        break;
      }

      case LINK_SMS:
      {
        pd_comms_wms_task_sms_connect(z_handle);
        break;
      }

      case LINK_HTTP:
      {
        pd_comms_wms_task_http_connect(z_handle);
        break;
      }

      default:
      {
        PDCOMM_APP_MSG_ERROR(" Received INVALID link_type %d in connect ",
                               p_hdr_ptr->z_params.e_link_type,0,0);
      }
    }/*switch(p_hdr_ptr->z_params.e_link_type) ends*/
  }/*if(z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTED)*/
  else if(z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTING)
  {
    z_handle_table[l_i].u_connectPending = TRUE;

    /*Save the connect request to process later when the current connection is
      completely disconnected*/
    memscpy(&z_handle_table[l_i].z_pendingConnectReq, sizeof(z_handle_table[l_i].z_pendingConnectReq),
            p_hdr_ptr, sizeof(*p_hdr_ptr));
  }
  else  /*Refuse the new connect request if previous connection is still in CONNECTED/CONNECTING state*/
  {
    pd_comms_notification_params_type z_params;

    z_params.e_new_state = z_handle_table[l_i].e_link_status;
    z_params.q_result = (uint32)PDCOMM_CONNECT_ERROR_PREVIOUS_CONNECTION_ACTIVE;

    PDCOMM_APP_MSG_HIGH("Connection already in CONNECTED/CONNECTING state for handle_id[%d]",
                          l_i,0,0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->z_params.q_transaction_id,
                           ERROR_RPT,&z_params);
  }


} /*pd_comms_wms_task_handle_connect() ends*/


/*===========================================================================
FUNCTION pd_comms_wms_task_sms_disconnect

DESCRIPTION
  This function handles a SMS link disconnect request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_sms_disconnect(
  /*handle corresponding to the connection for which connection is to be disconnected*/
  pd_comms_handle_type z_handle,
  /*abort the connection*/
  boolean b_abort,
  /*send DISCONNCTED event back to client or not*/
  boolean u_notifyClient)
{
  if(b_abort)
  {
    PDCOMM_APP_MSG_HIGH("Abort received for handle_id [%d]",
                        z_handle.q_handle_id,0,0);
    z_handle_table[z_handle.q_handle_id].session_aborted = TRUE;

    if(u_notifyClient)
    {
      pd_comms_notification_params_type z_params;
      pd_comms_return_type e_result;

      z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
      z_params.q_result = (uint32)PDCOMM_RESULT_OK;

      /* Before doing anything else sendout a DISCONNECTED state to client
       * After that proceed silently (without updating client) with the disconnect
       * process*/

      e_result = pd_comms_notify_client(z_handle,
                           z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                           z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                           z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                           STATE_CHANGE,&z_params);

      if(e_result != PDCOMM_RESULT_OK)
      {
        MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                  (uint32)e_result,z_handle.q_handle_id,0);

      }
    }/*if(u_notifyClient)*/

  }/*if(b_abort)*/

  switch(z_handle_table[z_handle.q_handle_id].e_link_status)
  {
    case PDCOMM_STATE_DISCONNECTED:
    {
      /*No action required*/
      break;
    }

    case PDCOMM_STATE_OPENING:
    {
     /*This state doesn't apply to  SMS connection*/
     MSG_ERROR("PDCOMM APP:Illegal state PDCOMM_STATE_OPENING for a SMS connection. Handle_id [%d]",
               z_handle.q_handle_id,0,0);
     if(!b_abort)
     {
       pd_comms_wms_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32)PDCOMM_RESULT_WAIT);
     }
     else
     {
       /*update local state but donot inform client*/
       pd_comms_wms_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                      FALSE,(uint32)PDCOMM_RESULT_WAIT);
     }
     break;

    }

    case PDCOMM_STATE_DISCONNECTING:
    {
      PDCOMM_APP_MSG_MED("Illegal state PDCOMM_STATE_DISCONNECTING for a SMS connection. Handle_id [%d]",
                         z_handle.q_handle_id,0,0);

      z_handle_table[z_handle.q_handle_id].client_disconnect_recvd = TRUE;
      if(!b_abort)
      {
        pd_comms_wms_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32)PDCOMM_RESULT_WAIT);
      }
      else
      {
        /*update local state but donot inform client*/
        pd_comms_wms_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                       FALSE,(uint32)PDCOMM_RESULT_WAIT);
      }
      break;
    }

    case PDCOMM_STATE_CONNECTED:
    case PDCOMM_STATE_CONNECTING:
    {

      if(!b_abort)
      {
        pd_comms_wms_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32)PDCOMM_RESULT_WAIT);
      }
      else
      {
        /*update local state but donot inform client*/
        pd_comms_wms_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                       FALSE,(uint32)PDCOMM_RESULT_WAIT);
      }

      break;
    }

  } /* switch(link_status) ends*/
}

/*===========================================================================
FUNCTION pd_comms_wms_task_http_disconnect

DESCRIPTION
  This function handles a HTTP link disconnect request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_http_disconnect(
  /*handle corresponding to the connection for which connection is to be disconnected*/
  pd_comms_handle_type z_handle)
{
  return ;
} /*lint !e715 */ /*pd_comms_wms_task_http_disconnect*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_disconnect

DESCRIPTION
  This function handles an incoming PDCOMM_DISCONNECT request. It will call the
  the disconnect function specific to the link_type(TCP,HTTP,SMS etc.)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_handle_disconnect(
  /*Pointer to the incoming PDCOMM_DISCONNECT request IPC message*/
  os_IpcMsgType* p_msg )
{
  pd_comms_handle_type z_handle;
  pd_comms_ipc_disconnect_hdr_type* p_hdr_ptr;

  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */
  p_hdr_ptr  = (pd_comms_ipc_disconnect_hdr_type*)p_msg->p_Data;
  /*lint +e826 */
  z_handle = p_hdr_ptr->z_handle;

  PDCOMM_APP_MSG_LOW("Disconnect request received for handle_id[%d]",z_handle.q_handle_id,0,0);

  if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )
  {
    if(z_handle_table[z_handle.q_handle_id].in_use == TRUE)
    {
      z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id =
                                           p_hdr_ptr->q_transaction_id;
      z_handle_table[z_handle.q_handle_id].client_disconnect_recvd = TRUE;
    }

    /*drop the pending connect request since the client wants to disconnect*/
    z_handle_table[z_handle.q_handle_id].u_connectPending = FALSE;

    switch(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type)
    {
      case LINK_TCP:
      {
        PDCOMM_APP_MSG_ERROR("Disconnect request for TCP link received by WMS-PDCOMM task",0,0,0);
        break;
      }

      case LINK_SMS:
      {
        pd_comms_wms_task_sms_disconnect(z_handle, p_hdr_ptr->abort,TRUE);
        break;
      }

      case LINK_HTTP:
      {
        pd_comms_wms_task_http_disconnect(z_handle);
        break;
      }

      default:
      {
        PDCOMM_APP_MSG_HIGH("PDCOMM APP: Link already disconnected for handle id [%d]",
                  z_handle.q_handle_id,0,0);
        break;

      }

    }/*switch(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type)*/
  }/*if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    pd_comms_notification_params_type z_params;

    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;

    z_params.q_result = (uint32)PDCOMM_DISCONNECT_ERROR_INVALID_HANDLE;
    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in disconnect ",
                        z_handle.q_handle_id,0,0);
    (void)pd_comms_notify_client(z_handle,
                             p_hdr_ptr->q_source_thread_id,
                             p_hdr_ptr->q_base_msg_id,
                             p_hdr_ptr->q_transaction_id,
                             ERROR_RPT,&z_params);

  }

}/* pd_comms_wms_task_handle_disconnect() ends*/

/*===========================================================================
FUNCTION pd_comms_wms_task_handle_release_handle

DESCRIPTION
  This function handles an incoming release_handle request. It releases a previously
  assigned handle. If the link is CONNECTED it will abort the connection before
  releasing the handle.

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type  pd_comms_wms_task_handle_release_handle(
  /*the assigned handle is returned in this variable */
  const pd_comms_handle_type* p_handle)
{
  uint32 q_handle_id;

  if(p_handle == NULL)
  {
    MSG_ERROR("PDCOMM APP:NULL handle received in relase handle request",0,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  q_handle_id = p_handle->q_handle_id;

  if(q_handle_id >= PDCOMM_MAX_HANDLES)
  {
    MSG_ERROR("PDCOMM APP:release_handle request received with invalid handle[%d]",q_handle_id,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  /*Get the mutex lock since get_handle could be called simultaneously by
    two PDCOMM clients*/
  MSG_MED("PDCOMM APP:Trying to acquire GetHandle Mutex lock",0,0,0);
  os_MutexLock(&z_gethandle_mutex);
  MSG_MED("PDCOMM APP:GetHandle Mutex lock acquired",0,0,0);

  if(!z_handle_table[q_handle_id].in_use)
  {
    MSG_ERROR("PDCOMM APP:release_handle request received for free handle[%d]",q_handle_id,0,0);
    os_MutexUnlock(&z_gethandle_mutex);
    MSG_MED("PDCOMM APP:GetHandle Mutex unlocked",0,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  if(z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_DISCONNECTED)
  {
    pd_comms_wms_task_release_handle(q_handle_id);
  }
  else
  {
    /*if the link is not already disconnected, abort the connection before releasing
      the handle*/
    switch(z_handle_table[q_handle_id].z_connect_params.e_link_type)
    {
      case LINK_SMS:
      {
        pd_comms_wms_task_sms_disconnect(*p_handle, TRUE,FALSE);
        break;
      }
      case LINK_HTTP:
      {
        pd_comms_wms_task_http_disconnect(*p_handle);
        break;
      }
      default:
      {
        PDCOMM_APP_MSG_HIGH("PDCOMM APP: Invalid link type [%d] for handle id [%d]",
                            z_handle_table[q_handle_id].z_connect_params.e_link_type,
                            q_handle_id,0);

      }

    }/*end switch */

    /*release the handle*/
    pd_comms_wms_task_release_handle(q_handle_id);
  }

  os_MutexUnlock(&z_gethandle_mutex);
  MSG_MED("PDCOMM APP:GetHandle Mutex unlocked",0,0,0);
  return PDCOMM_RESULT_OK;
}

/*===========================================================================
FUNCTION pd_comms_wms_task_http_write

DESCRIPTION
  This function handles a write request for HTTP connection. It also notifies
  the client about the result of the write operation

DEPENDENCIES
  None

RETURN VALUE
  boolean - write operation successfull or not

SIDE EFFECTS
  None
===========================================================================*/
static boolean pd_comms_wms_task_http_write(
  /*handle corresponding to the connection for which data is to be written*/
  pd_comms_handle_type z_handle)
{
 return FALSE;
} /*lint !e715 */ /*pd_comms_wms_task_http_write*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_write

DESCRIPTION
  This function handles an incoming PDCOMM_WRITE request IPC message. It calls
  the write functio specific to the link_type (TCP,HTTP,SMS etc.)

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_handle_write(
  /*pointer to the PDCOMM_WRITE IPC message*/
  os_IpcMsgType* p_msg )
{
  pd_comms_handle_type z_handle;
  pd_comms_ipc_write_hdr_type* p_hdr_ptr;

  if(p_msg == NULL)
  {
    return;
  }
 /*lint -e826 */
  p_hdr_ptr = (pd_comms_ipc_write_hdr_type*)p_msg->p_Data;
  /*lint +e826 */
  z_handle = p_hdr_ptr->z_handle;

  PDCOMM_APP_MSG_LOW("Write request received for handle_id[%d]",z_handle.q_handle_id,0,0);

  if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )
  {
    pd_comms_wms_task_reset_inactivity_count(z_handle.q_handle_id);

    if(z_handle_table[z_handle.q_handle_id].w_num_bytes_written <
       z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write)
    {
      pd_comms_notification_params_type z_params;

      MSG_ERROR("Write called while previous write still pending for handle_id [%d]",
                z_handle.q_handle_id,0,0);

      z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;
      z_params.q_result = (uint32)PDCOMM_WRITE_ERROR_PREVIOUS_WRITE_PENDING;

      (void)pd_comms_notify_client(z_handle,
                             p_hdr_ptr->q_source_thread_id,
                             p_hdr_ptr->q_base_msg_id,
                             p_hdr_ptr->q_transaction_id,
                             ERROR_RPT,&z_params);

      return;
    }

    z_handle_table[z_handle.q_handle_id].q_write_transaction_id =
                                                     p_hdr_ptr->q_transaction_id;

    p_hdr_ptr->w_data_len = memscpy(z_handle_table[z_handle.q_handle_id].p_write_buff,
           PDCOMM_WRITE_DATA_BUF_SIZE, p_hdr_ptr->p_data, p_hdr_ptr->w_data_len);

    z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = p_hdr_ptr->w_data_len;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_written = 0;

    switch(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type)
    {
      case LINK_TCP:
      {
        PDCOMM_APP_MSG_ERROR("Write request for TCP link received by WMS-PDCOMM task",0,0,0);
        break;
      }

      case LINK_SMS:
      {
        pd_comms_wms_task_sms_write(z_handle,&p_hdr_ptr->z_tl_write_params.z_sms_write_params);
        break;
      }

      case LINK_HTTP:
      {
        (void)pd_comms_wms_task_http_write(z_handle);
        break;
      }

      default:
      {
          MSG_ERROR("PDCOMM APP: Invalid link type [%d] recevied in write request for handleid [%d]",
                     z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type,z_handle.q_handle_id,0);
      }

    }/*switch(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type)*/
  }/*if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    pd_comms_notification_params_type z_params;

    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_WRITE_ERROR_INVALID_HANDLE;

    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in write ",
                          z_handle.q_handle_id,0,0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->q_transaction_id,
                           ERROR_RPT,&z_params);

  }
} /*pd_comms_wms_task_handle_write() ends*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_read

DESCRIPTION
  This function handles an incoming PDCOMM_READ request IPC message. It calls
  the read function specific to the link_type (TCP,HTTP,SMS etc.)

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_handle_read(
  /*pointer to the PDCOMM_READ IPC message*/
  os_IpcMsgType* p_msg )
{
  pd_comms_handle_type z_handle;
  pd_comms_ipc_read_hdr_type* p_hdr_ptr;

  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */
  p_hdr_ptr  = (pd_comms_ipc_read_hdr_type*)p_msg->p_Data;
  /*lint +e826 */
  z_handle = p_hdr_ptr->z_handle;

  PDCOMM_APP_MSG_LOW("Read request received for handle_id[%d]",z_handle.q_handle_id,0,0);

  if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )
  {
    pd_comms_wms_task_reset_inactivity_count(z_handle.q_handle_id);

    if(z_handle_table[z_handle.q_handle_id].w_num_bytes_already_read <
       z_handle_table[z_handle.q_handle_id].w_num_bytes_to_read)
    {

      MSG_ERROR("Read called while previous read still pending for handle_id [%d]",
                z_handle.q_handle_id,0,0);

      /*MR: Will have to free the previous blocked read bufffer here
            when dynamic memory allocation is used*/

    }

    z_handle_table[z_handle.q_handle_id].q_read_transaction_id =
                                                     p_hdr_ptr->p_params.q_transaction_id;

    if(p_hdr_ptr->p_params.w_data_len > PDCOMM_READ_DATA_BUF_SIZE)
    {
      p_hdr_ptr->p_params.w_data_len = PDCOMM_READ_DATA_BUF_SIZE;
    }

    z_handle_table[z_handle.q_handle_id].w_num_bytes_to_read = p_hdr_ptr->p_params.w_data_len;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_already_read = 0;

    switch(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type)
    {
      case LINK_TCP:
      {
        PDCOMM_APP_MSG_ERROR("Write request for TCP link received by WMS-PDCOMM task",0,0,0);
        break;
      }

      case LINK_SMS:
      {
        (void)pd_comms_wms_task_sms_read(z_handle.q_handle_id,p_hdr_ptr->p_params.u_wait_for_all);
        break;
      }

      case LINK_HTTP:
      {
        /*pd_comms_wms_task_http_read(z_handle);*/
        break;
      }

      default:
      {
          MSG_ERROR("PDCOMM APP: Invalid link type [%d] recevied in write request for handleid [%d]",
                     z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type,z_handle.q_handle_id,0);
      }
    }/*switch(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type)*/
  }/*if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    pd_comms_notification_params_type z_params;
    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_WRITE_ERROR_INVALID_HANDLE;
    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in read ",
                          z_handle.q_handle_id,0,0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->p_params.q_transaction_id,
                           ERROR_RPT,&z_params);

  }
} /*pd_comms_wms_task_handle_read() ends*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_hash

DESCRIPTION
  This function handles an incoming PDCOMM_HASH request IPC message.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_handle_hash(
  /*pointer to the PDCOMM_HASH IPC message*/
  os_IpcMsgType* p_msg )
{

  uint8 p_hmac_buff[32]; /* SHA-256 outputs a 256 bit hash */
  secerrno_enum_type e_error;
  pd_comms_return_type e_result;
  secapi_hsh_param_data_type z_param_data;
  uint8* p_key;/*lint -e826 */
  pd_comms_ipc_hash_req_hdr_type* p_hdr_ptr;
  secapi_algo_enum_type e_hash_algo = SECAPI_SHA;
  secapi_handle_type z_hash_handle;

  if(p_msg == NULL)
  {
    return;
  }

  PDCOMM_APP_MSG_LOW("Hash request received",0,0,0);

  p_hdr_ptr  = (pd_comms_ipc_hash_req_hdr_type*)p_msg->p_Data;
  /*lint +e826 */
  if( ((p_hdr_ptr->e_hash_type != SHA1) && (p_hdr_ptr->e_hash_type != SHA256)) || (p_hdr_ptr->q_msg_len > 65535))
  {
    pd_comms_notification_params_type z_params;
    pd_comms_handle_type z_handle;

    MSG_ERROR("PDCOMM APP: Invalid parameters in hash request for transaction_id [%d]",
               p_hdr_ptr->q_transaction_id, 0, 0);

    /*Since there is no handle corresponding to a hash request,set
     *a big handle_id so that the notification doesn't get mistaken as the
     *one for a real handle*/
    z_handle.q_handle_id = 65535;
    z_handle.e_link_type = LINK_TYPE_MAX;

    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_HASH_ERROR_INVALID_PARAMETERS;

    e_result = pd_comms_notify_client(z_handle,
               p_hdr_ptr->q_source_thread_id,
               p_hdr_ptr->q_base_msg_id,
               p_hdr_ptr->q_transaction_id,
               ERROR_RPT,&z_params);
    if(e_result != PDCOMM_RESULT_OK)
    {
      MSG_ERROR("PDCOMM APP: IPC error %d for transaction_id [%d]",
                (uint32)e_result,p_hdr_ptr->q_transaction_id,0);
    }

    return;
  }/*if((p_hdr_ptr->e_hash_type != SHA1) || (p_hdr_ptr->q_msg_len > 65535))*/

  if(p_hdr_ptr->e_hash_type == SHA1)
  {
    e_hash_algo = SECAPI_SHA;
    z_hash_handle = z_hash_handle_sha;
    PDCOMM_APP_MSG_MED("PDCOMM APP: Using SHA1 as Hash Algo",0,0,0);
  }
  else
  {
    e_hash_algo = SECAPI_SHA256;
    z_hash_handle = z_hash_handle_sha256;
    PDCOMM_APP_MSG_MED("PDCOMM APP: Using SHA256 as Hash Algo",0,0,0);    
  }
  
  p_key = (uint8*) &p_hdr_ptr->p_data[p_hdr_ptr->q_msg_len];

  /* Process payload atomically */
  z_param_data.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  (void)secapi_hsh_set_param( z_hash_handle, e_hash_algo, SECAPI_HSH_INPUT_MODE, &z_param_data );

  /* Block until hash is completed */
  z_param_data.common.exec_mode = SECAPI_SYNC_EXEC;
  (void)secapi_hsh_set_param( z_hash_handle, e_hash_algo, SECAPI_EXEC_MODE, &z_param_data );

  /* Submit SUPL INIT to HMAC generator using the FQDN as the key */
  e_error = secapi_hsh_create_hmac( z_hash_handle, e_hash_algo, (uint8*)p_hdr_ptr->p_data,
                                   (uint16)p_hdr_ptr->q_msg_len,
                                   (uint8 *)p_key, (uint16)p_hdr_ptr->w_key_len, p_hmac_buff);

  if(e_error != E_SUCCESS)
  {
    pd_comms_notification_params_type z_params;
    pd_comms_handle_type z_handle;

    MSG_ERROR("PDCOMM APP: Error %d when generating HMAC for transaction_id [%d]",
               e_error, p_hdr_ptr->q_transaction_id, 0);
    /*Since there is no handle corresponding to a hash request,set
     *a big handle_id so that the notification doesn't get mistaken as the
     *one for a real handle*/
    z_handle.q_handle_id = 65535;
    z_handle.e_link_type = LINK_TYPE_MAX;

    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_HASH_ERROR_UNKNOWN;

    e_result = pd_comms_notify_client(z_handle,
               p_hdr_ptr->q_source_thread_id,
               p_hdr_ptr->q_base_msg_id,
               p_hdr_ptr->q_transaction_id,
               ERROR_RPT,&z_params);
    if(e_result != PDCOMM_RESULT_OK)
    {
      MSG_ERROR("PDCOMM APP: IPC error %d for transaction_id [%d]",
                (uint32)e_result,p_hdr_ptr->q_transaction_id,0);
    }

  }/*if(e_error != E_SUCCESS)*/
  else
  {
    pd_comms_generate_hash_response_params_type z_hash_resp_params;

    /* SUPL Uses HMAC-SHA1-64, so truncate HMAC by only sending first 8 bytes ( 64 bits ) */
    z_hash_resp_params.q_output_len  = 8;
    z_hash_resp_params.p_hash_output = p_hmac_buff;
    z_hash_resp_params.q_transaction_id = p_hdr_ptr->q_transaction_id;
    z_hash_resp_params.q_msg_len = p_hdr_ptr->q_msg_len;
    z_hash_resp_params.p_msg = (uint8*)p_hdr_ptr->p_data;

    (void)pd_comms_send_hash_resp_to_client(p_hdr_ptr->q_source_thread_id,p_hdr_ptr->q_base_msg_id,
                                      &z_hash_resp_params);

  }
} /*pd_comms_wms_task_handle_hash*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_ioctl

DESCRIPTION
  This function handles an incoming PDCOMM_IOCTL request IPC message.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_handle_ioctl(
  /*pointer to the PDCOMM_IOCTL IPC message*/
  os_IpcMsgType* p_msg )
{
  pd_comms_handle_type z_handle;
  pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr;

  if(p_msg == NULL)
  {
    MSG_ERROR("PDCOMM APP: NULL mesg received in ioctl call",0,0,0);
    return;
  }
 /*lint -e826 */
  p_hdr_ptr = (pd_comms_ipc_ioctl_hdr_type*)p_msg->p_Data;
  z_handle = p_hdr_ptr->z_handle;
  /*lint +e826 */

  PDCOMM_APP_MSG_LOW("Ioctl command [%d] received for handle_id[%d]",
                     p_hdr_ptr->q_ioctl_request,
                     z_handle.q_handle_id,0);

  if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )
  {
    z_handle_table[z_handle.q_handle_id].q_ioctl_transaction_id = p_hdr_ptr->q_transaction_id;

    switch(p_hdr_ptr->q_ioctl_request)
    {
      default:
      {
        pd_comms_notification_params_type z_params;

        z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;
        z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;\

        PDCOMM_APP_MSG_HIGH(" Received INVALID command %d in ioctl ",
                              p_hdr_ptr->q_ioctl_request,0,0);
        (void)pd_comms_notify_client(z_handle,
                               p_hdr_ptr->q_source_thread_id,
                               p_hdr_ptr->q_base_msg_id,
                               p_hdr_ptr->q_transaction_id,
                               ERROR_RPT,&z_params);

      }
    }
  }/*if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    pd_comms_notification_params_type z_params;

    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;

    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in ioctl ",
                          z_handle.q_handle_id,0,0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->q_transaction_id,
                           ERROR_RPT,&z_params);

  }

}/*pd_comms_wms_task_handle_ioctl*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_inactivity_timer

DESCRIPTION
  This function is called when a inactivity timer times out. It goes through
  all the in_use handles and finds out if there is a connection inactive for
  PDCOMM_INACTIVITY_TIMEOUT period. If it finds such a connection,
  it will abort that connection.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_wms_task_handle_inactivity_timer(void)
{
  uint32 l_i;

  for(l_i =0;l_i<PDCOMM_MAX_HANDLES;l_i++)
  {
    if(z_handle_table[l_i].in_use &&
       z_handle_table[l_i].z_connect_params.e_link_type != LINK_SMS)
    {
      z_handle_table[l_i].l_inactivity_count += PDCOMM_INACTIVITY_TIMER_PERIOD;

      if(z_handle_table[l_i].l_inactivity_count >= PDCOMM_INACTIVITY_TIMEOUT)
      {

        z_handle_table[l_i].l_inactivity_count = 0;
        MSG_ERROR("PDCOMM APP: Handle [%d] inactive for more than %d millisec. Aborting connection..",
                  l_i,PDCOMM_INACTIVITY_TIMER_PERIOD,0);

        if(z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTED)
        {
          MSG_ERROR("PDCOMM APP: Client already notified about aborted connection. Release Handle [%d]",
                    l_i,0,0);
        }
        else
        {
          if(z_handle_table[l_i].z_connect_params.e_link_type != LINK_SMS)
          {
            PDCOMM_APP_MSG_ERROR(" Invalid link type [%d] ",
                                 z_handle_table[l_i].z_connect_params.e_link_type,
                                 0,0);
          }
          else
          {
            pd_comms_wms_task_sms_disconnect(z_handle_table[l_i].z_handle,TRUE,TRUE);
          }

        }

      }
    }

  }/*for loop ends*/

  /* Re-arm the inactivity timer. */
  (void)os_TimerStart(p_inactivity_timer,PDCOMM_INACTIVITY_TIMER_PERIOD,0);

}/*pd_comms_wms_task_handle_inactivity_timer*/


/*===========================================================================
FUNCTION pd_comms_app_tasl_handle_stop

DESCRIPTION
  This function is used to handle Stop IPC

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_handle_stop( void )
{
  PDCOMM_APP_MSG_MED("STOP request received",0,0,0);

  /* Deregister with the Dog Heartbeat before stopping task */
  os_DogHeartbeatDeregister();

  os_TaskStopAck();
}/*pd_comms_wms_task_handle_stop*/


/*===========================================================================
FUNCTION pd_comms_wms_task_handle_offline

DESCRIPTION
  This function is used to handle TASK_OFFLINE IPC

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_handle_offline( void )
{
  u_pd_comms_online = FALSE;
  PDCOMM_APP_MSG_MED("OFFLINE request received",0,0,0);

}/*pd_comms_wms_task_handle_offline*/


/*===========================================================================
FUNCTION pd_comms_wms_task_proc

DESCRIPTION
  This function handles the all the incoming IPC messages. It will call the
  appropriate function based on the type of IPC message received.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_wms_task_proc(
  /*Pointer to received IPC message */
  os_IpcMsgType* p_msg )
{
/*pd_comms_wms_task calls this function whenever it recieves a
message*/

  if( p_msg == NULL )
  {
      PDCOMM_APP_MSG_ERROR(" Received NULL IPC Message ",0,0,0);
      return;
  }

  if (u_pd_comms_online)
  {
    switch(p_msg->q_MsgId)
    {
      case C_PD_COMMS_APP_MSG_CONNECT:
      {
        /*lint -e826 */
        pd_comms_wms_task_handle_connect((pd_comms_ipc_connect_hdr_type*)p_msg->p_Data);
        /*lint +e826 */
        break;
      }

      case C_PD_COMMS_APP_MSG_DISCONNECT:
      {
        pd_comms_wms_task_handle_disconnect(p_msg);
        break;
      }

      case C_PD_COMMS_APP_MSG_WRITE:
      {
        pd_comms_wms_task_handle_write(p_msg);
        break;
      }

      case C_PD_COMMS_APP_MSG_READ:
      {
        pd_comms_wms_task_handle_read(p_msg);
        break;
      }

      case C_PD_COMMS_APP_MSG_HASH_REQ:
      {
        pd_comms_wms_task_handle_hash(p_msg);
        break;
      }

      case C_PD_COMMS_APP_MSG_IOCTL:
      {
        pd_comms_wms_task_handle_ioctl(p_msg);
        break;
      }

      case PDCOMM_APP_TASK_SMS_MSG_CB:
      {
        pd_comms_wms_task_handle_sms_msg_cb(p_msg);
        break;
      }

      case C_PD_COMMS_APP_MSG_INJECT_MSG:
      {
        pd_comms_wms_task_handle_inject_msg(p_msg);
        break;
      }

      case C_OS_MSG_ID_TIMER_EXPIRY:
      {
        /*lint -e826 */
        if( ((os_TimerExpiryType*)p_msg->p_Data)->q_TimerId == (uint32)PDCOMM_APP_TASK_INACTIVITY_TIMER )
        {
          pd_comms_wms_task_handle_inactivity_timer();
        }
        else
        {
          MSG_ERROR("PDCOMM APP: Invalid timer id %d in timeout message",
                    ((os_TimerExpiryType*)p_msg->p_Data)->q_TimerId,0,0);
        }
        break;
        /*lint +e826 */
      }

      case C_OS_MSG_ID_OFFLINE:
      {
        pd_comms_wms_task_handle_offline();
        break;
      }

      case C_OS_MSG_ID_STOP:
      {
        pd_comms_wms_task_handle_stop();
        break;
      }
      case C_OS_MSG_ID_DOG_HB:
      {
        // Respond to the dog heart beat. Pat the dog. 
        os_DogHeartbeatReport();
        break;
      }
      default:
      {
        PDCOMM_APP_MSG_MED("Illegal IPC message type: %d received",
                           p_msg->q_MsgId,0,0);
        break;
      }

    }/* switch(p_msg->q_MsgId) ends */
  }/*else if (u_pd_comms_online)*/

}/*pd_comms_wms_task_proc() ends*/


/*===========================================================================
FUNCTION pd_comms_wms_task

DESCRIPTION
  This is the main entry function of the PDCOMM_APP task. It will initalize IPC
  and internal data structures and will also undergo the startup procedure with
  TMC. It then waits infintely for incoming IPC messages.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_wms_task(
  /*task input parameters(currently not used)*/
  dword q_dummy )
{
/*The entry point for the task and the infinite loop
  are present in this function */

  os_IpcMsgType* p_msg;

  pd_comms_wms_task_startup();

  pd_comms_wms_task_init_handle_table();

  os_TaskReadyAck();
  os_TaskStopRegister();

  pd_comms_wms_task_register_callbacks();
  

  pd_comms_wms_task_sms_init();

  /* Create a new Hash function crypto instance */
  if( secapi_new( &z_hash_handle_sha, SECAPI_SHA ) != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Could not create a hash function instance",0,0,0);
  }

  if( secapi_new( &z_hash_handle_sha256, SECAPI_SHA256 ) != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Could not create a hash function instance",0,0,0);//auto-gen, to change remove 'QSR_' and first param
  }
  /* Start the inactivity timer. */
  /*os_TimerStart(p_inactivity_timer,PDCOMM_INACTIVITY_TIMER_PERIOD,0);*/

  for(;;)
  {
    p_msg = os_IpcReceive();

    if(p_msg == NULL)
    {
      MSG_ERROR("PDCOMM APP: NULL message received",0,0,0);
    }
    else
    {
      pd_comms_wms_task_proc(p_msg);
      (void)os_IpcDelete(p_msg);
    }

  }/*for(;;) ends*/

} /*pd_comms_wms_task() ends*/ /*lint !e715 */
#endif
