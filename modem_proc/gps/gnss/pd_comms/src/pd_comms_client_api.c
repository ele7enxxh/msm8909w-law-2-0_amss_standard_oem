/*======================================================================

                         PDCOMM APP API definitions file

 GENERAL DESCRIPTION
  This file contains API definitions by which PDCOMM Application can 
  communicate to it's clients (e.g Transport Manager). 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/src/pd_comms_client_api.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 06/02/08   kj   Compensate for de-featurization of the Public API header file 
 04/04/06   mr   Initial creation of file.
======================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"
#include "pd_comms_client_api.h"
#include "aries_os_api.h"

#ifdef FEATURE_CGPS_PDCOMM_CLIENT
#include <string.h>
#include <stdlib.h>
#include "msg.h"
#endif

/****************** Function Definitions**************************************/
#ifdef FEATURE_CGPS_LBS_TASK
static pd_comms_return_type pd_comms_send_ipc_to_client(
  const pd_comms_handle_type z_handle,
  uint32 msg_id,
  int  len, 
  void *p_msg,
  os_IpcMsgType* p_ipc_ptr)
{
  int i;
  pd_comms_return_type retVal = PDCOMM_RESULT_OK;

  /*
  * Look through the ipc function table entries for any entry that is occupied
  * and that matches the handle and link type passed to us.
  */
  for(i = 0; i < MAX_IPC_CB_TABLE_SIZE; i++)
  {
    if( (ipc_func_table[i].occupied == TRUE) && 
      (ipc_func_table[i].z_handle.q_handle_id == z_handle.q_handle_id) &&
      (ipc_func_table[i].z_handle.e_link_type == z_handle.e_link_type) ) 
    {
      /*
      * If we have a ipc function registered for this entry, use that function
      * instead of os_IpcSend
      */
      if(ipc_func_table[i].ipc_send_func)
      {
        retVal = ipc_func_table[i].ipc_send_func( msg_id, len, (uint8*) p_msg);
        (void)os_IpcDelete(p_ipc_ptr);
        return retVal;
      }
    }
  }

  /* 
  * If we come here, there is no ipc function registered for this handle, use 
  * default os_IpcSend
  */
  if(!os_IpcSend(p_ipc_ptr, p_ipc_ptr->q_DestThreadId))
  {
    (void)os_IpcDelete(p_ipc_ptr);
    return PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
  }

  return retVal;
}
#endif

/*===========================================================================
FUNCTION pd_comms_notify_client

DESCRIPTION
  This function notifies TM about the status of a particular connection. This 
  can include state change or error.
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_notify_client(
  /*handle corresponding to the connection for which notification is
  being provided*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_dest_thread_id,
  /*base message ID to be added to the messageID when responding
  back to the client. The base messageID is provided in the connect call*/
  const uint32 q_base_msg_id,
  /*transaction id corresponding to the notification*/
  const uint32 q_transaction_id,
  /*Type of notification*/
  const pd_comms_app_notification_type e_notification,
  /*Pointer to structure containing notification parameters*/
  const pd_comms_notification_params_type * const p_params )
{
#ifdef FEATURE_CGPS_PDCOMM_CLIENT
  os_IpcMsgType* p_ipc_ptr = NULL;  
  pd_comms_ipc_notification_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_OK;  

  if(p_params == NULL)
  {
      e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_ipc_notification_hdr_type),
                           IPC_ALLOCATION_DYNAMIC,q_dest_thread_id); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (pd_comms_ipc_notification_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

      p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_CLIENT_MSG_NOTIFY + q_base_msg_id;
      p_ipc_ptr->q_DestThreadId   = q_dest_thread_id;
      p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_notification_hdr_type);
      
      p_hdr_ptr->q_transaction_id = q_transaction_id;
      p_hdr_ptr->z_handle = z_handle;
      p_hdr_ptr->e_type = e_notification;
      p_hdr_ptr->z_params = *p_params;
      if(p_params->e_new_state > PDCOMM_STATE_DISCONNECTING)
      {
        MSG_ERROR("PDCOMM_APP: Invalid state %d notified to client",
                   p_params->e_new_state,0,0);
      }

#ifdef FEATURE_CGPS_LBS_TASK
      e_result = pd_comms_send_ipc_to_client( 
        z_handle, 
        (uint32)C_PD_COMMS_CLIENT_MSG_NOTIFY + q_base_msg_id,
        sizeof(*p_hdr_ptr), 
        p_hdr_ptr,
        p_ipc_ptr);
#else
      if(!os_IpcSend(p_ipc_ptr,q_dest_thread_id))
      {
        e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
        (void)os_IpcDelete(p_ipc_ptr);
      }
#endif

    }


  }/*if(p_params ==  NULL) else {}*/
  

  return e_result;
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM_CLIENT */
}


/*===========================================================================
FUNCTION pd_comms_send_handle_to_client

DESCRIPTION
  This function sends the handle allocated by PD-COMM application in response
  to pdcomm_tm_connect() call to TM.
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_send_handle_to_client(
  /*handle to the link for which data arrived*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_dest_thread_id,
  /*base message ID to be added to the messageID when responding
  back to the client. The base messageID is provided in the connect call*/
  const uint32 q_base_msg_id,
  /*transaction-id received in PDCOMM_CONNECT mesg.*/
  const uint32 q_transaction_id
)
{
#ifdef FEATURE_CGPS_PDCOMM_CLIENT
  os_IpcMsgType* p_ipc_ptr = NULL;  
  pd_comms_ipc_handle_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_OK;  

  
  p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_ipc_handle_hdr_type),
                         IPC_ALLOCATION_DYNAMIC,q_dest_thread_id);
               

  if(p_ipc_ptr == NULL)
  {
    e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
  }
  else
  {
    /*Pack the message in IPC HDR*/
    p_hdr_ptr = (pd_comms_ipc_handle_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

    p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_CLIENT_MSG_HANDLE + q_base_msg_id;
    p_ipc_ptr->q_DestThreadId   = q_dest_thread_id;
    p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_handle_hdr_type);

    p_hdr_ptr->z_handle = z_handle;
    p_hdr_ptr->q_transaction_id = q_transaction_id;
         

#ifdef FEATURE_CGPS_LBS_TASK
    e_result = pd_comms_send_ipc_to_client( 
      z_handle, 
      (uint32)C_PD_COMMS_CLIENT_MSG_HANDLE + q_base_msg_id,
      sizeof(*p_hdr_ptr), 
      p_hdr_ptr,
      p_ipc_ptr);
#else
    if(!os_IpcSend(p_ipc_ptr,q_dest_thread_id))
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
      (void)os_IpcDelete(p_ipc_ptr);
    }
#endif
  }

  return e_result;
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM_CLIENT */
}


/*===========================================================================
FUNCTION pd_comms_senddata_to_client

DESCRIPTION
  This function sends the data received from data services to TM. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_senddata_to_client(
  /*handle to the link for which data arrived*/   
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_dest_thread_id,
  /*base message ID to be added to the messageID when responding
  back to the client. The base messageID is provided in the connect call*/
  const uint32 q_base_msg_id,
  /*transaction id corresponding to this read call*/
  const uint32 q_transaction_id,
  /*parameters of the status for which TM is to be notified*/
  const pd_comms_read_data_params_type * const p_params )
{
#ifdef FEATURE_CGPS_PDCOMM_CLIENT
  os_IpcMsgType* p_ipc_ptr = NULL;  
  pd_comms_ipc_read_data_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_OK;  

  if(p_params == NULL)
  {
    e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_ipc_read_data_hdr_type)+p_params->w_data_len-1,
                              IPC_ALLOCATION_DYNAMIC,q_dest_thread_id);
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (pd_comms_ipc_read_data_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

      p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_CLIENT_MSG_READ_DATA + q_base_msg_id;
      p_ipc_ptr->q_DestThreadId   = q_dest_thread_id;
      p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_read_data_hdr_type);

      p_hdr_ptr->q_transaction_id = q_transaction_id;
      p_hdr_ptr->z_handle = z_handle;
      p_hdr_ptr->w_data_len = p_params->w_data_len;
      memscpy(p_hdr_ptr->p_data, p_params->w_data_len, p_params->p_data, p_params->w_data_len);
         

#ifdef FEATURE_CGPS_LBS_TASK
      e_result = pd_comms_send_ipc_to_client( 
        z_handle, 
        (uint32)C_PD_COMMS_CLIENT_MSG_READ_DATA + q_base_msg_id,
        sizeof(*p_hdr_ptr) + p_params->w_data_len-1, 
        p_hdr_ptr,
        p_ipc_ptr);
#else
      if(!os_IpcSend(p_ipc_ptr,q_dest_thread_id))
      {
        e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
        (void)os_IpcDelete(p_ipc_ptr);
      }
#endif

    }


  }/*if(p_params ==  NULL) else {}*/

  return e_result;
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM_CLIENT */
}


/*===========================================================================
FUNCTION pd_comms_send_write_ack_to_client

DESCRIPTION
  This function sends the ack for a ioctl command to TM. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_send_write_ack_to_client(
  /*handle to the link for which write ack is being sent*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_dest_thread_id,
  /*base message ID to be added to the messageID when responding
  back to the client. The base messageID is provided in the connect/write call*/
  const uint32 q_base_msg_id,
  /*parameters of the write ack to be sent to TM*/
  const pd_comms_write_ack_params_type * const p_params )
{
#ifdef FEATURE_CGPS_PDCOMM_CLIENT
  os_IpcMsgType* p_ipc_ptr = NULL;  
  pd_comms_ipc_write_ack_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_OK;  

  if(p_params == NULL)
  {
    e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_ipc_write_ack_hdr_type),
                              IPC_ALLOCATION_DYNAMIC,q_dest_thread_id);
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (pd_comms_ipc_write_ack_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

      p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_CLIENT_MSG_WRITE_ACK + q_base_msg_id;
      p_ipc_ptr->q_DestThreadId   = q_dest_thread_id;
      p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_write_ack_hdr_type);

      p_hdr_ptr->z_handle = z_handle;
      p_hdr_ptr->q_bytes_written = p_params->q_bytes_written;
      p_hdr_ptr->q_transaction_id = p_params->q_transaction_id;
               

#ifdef FEATURE_CGPS_LBS_TASK
      e_result = pd_comms_send_ipc_to_client( 
        z_handle, 
        (uint32)C_PD_COMMS_CLIENT_MSG_WRITE_ACK + q_base_msg_id,
        sizeof(*p_hdr_ptr), 
        p_hdr_ptr,
        p_ipc_ptr);
#else
      if(!os_IpcSend(p_ipc_ptr,q_dest_thread_id))
      {
        e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
        (void)os_IpcDelete(p_ipc_ptr);
      }
#endif
    }


  }/*if(p_params ==  NULL) else {}*/

  return e_result;
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM_CLIENT */
}


/*===========================================================================
FUNCTION pd_comms_send_sms_ack_report_to_client

DESCRIPTION
  This function sends the ack report for a SMS ack sent out
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_send_sms_ack_report_to_client(
  /*handle to the link for which write ack is being sent*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_dest_thread_id,
  /*base message ID to be added to the messageID when responding
  back to the client. The base messageID is provided in the connect/write call*/
  const uint32 q_base_msg_id,
  /*parameters of the sms ack report to be sent to TM*/
  const pd_comms_sms_ack_report_params_type * const p_params )
{
#ifdef FEATURE_CGPS_PDCOMM_CLIENT
  os_IpcMsgType* p_ipc_ptr = NULL;  
  pd_comms_ipc_sms_ack_report_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_OK;  

  if(p_params == NULL)
  {
    e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_ipc_sms_ack_report_hdr_type),
                              IPC_ALLOCATION_DYNAMIC,q_dest_thread_id);
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (pd_comms_ipc_sms_ack_report_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

      p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_CLIENT_MSG_SMS_ACK_REPORT + q_base_msg_id;
      p_ipc_ptr->q_DestThreadId   = q_dest_thread_id;
      p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_sms_ack_report_hdr_type);

      p_hdr_ptr->z_handle = z_handle;
      p_hdr_ptr->q_transaction_id = p_params->q_transaction_id;
      p_hdr_ptr->u_success = p_params->u_success;
               

#ifdef FEATURE_CGPS_LBS_TASK
      e_result = pd_comms_send_ipc_to_client( 
        z_handle, 
        (uint32)C_PD_COMMS_CLIENT_MSG_SMS_ACK_REPORT + q_base_msg_id,
        sizeof(*p_hdr_ptr), 
        p_hdr_ptr,
        p_ipc_ptr);
#else
      if(!os_IpcSend(p_ipc_ptr,q_dest_thread_id))
      {
        e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
        (void)os_IpcDelete(p_ipc_ptr);
      }
#endif

    }


  }/*if(p_params ==  NULL) else {}*/

  return e_result;
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM_CLIENT */
}


/*===========================================================================
FUNCTION pd_comms_send_ioctl_ack_to_client

DESCRIPTION
  This function sends the result of a ioctl command to TM. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_send_ioctl_ack_to_client(
  /*handle to the link for which data arrived*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_dest_thread_id,
  /*base message ID to be added to the messageID when responding
  back to the client. The base messageID is provided in the connect/ioctl call*/
  const uint32 q_base_msg_id,
  /*parameters of the ioctl ack to be sent to TM*/
  const pd_comms_ioctl_ack_params_type * const p_params )
{
#ifdef FEATURE_CGPS_PDCOMM_CLIENT
  os_IpcMsgType* p_ipc_ptr = NULL;  
  pd_comms_ipc_ioctl_ack_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_OK;  

  if(p_params == NULL)
  {
    e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(
                          sizeof(pd_comms_ipc_ioctl_ack_hdr_type)+p_params->q_argument_len-1,
                          IPC_ALLOCATION_DYNAMIC,q_dest_thread_id
                          );
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (pd_comms_ipc_ioctl_ack_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

      p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_CLIENT_MSG_IOCTL_ACK + q_base_msg_id;
      p_ipc_ptr->q_DestThreadId   = q_dest_thread_id;
      p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_ioctl_ack_hdr_type);

      p_hdr_ptr->z_handle = z_handle;
      p_hdr_ptr->q_transaction_id = p_params->q_transaction_id;
      p_hdr_ptr->q_argument_len = p_params->q_argument_len;
      p_hdr_ptr->q_ioctl_request = (uint32)p_params->q_ioctl_request;
      memscpy(p_hdr_ptr->p_data, p_params->q_argument_len, p_params->p_argument, p_params->q_argument_len);
                     

#ifdef FEATURE_CGPS_LBS_TASK
      e_result = pd_comms_send_ipc_to_client( 
        z_handle, 
        (uint32)C_PD_COMMS_CLIENT_MSG_IOCTL_ACK + q_base_msg_id,
        sizeof(*p_hdr_ptr) + p_params->q_argument_len-1, 
        p_hdr_ptr,
        p_ipc_ptr);
#else
      if(!os_IpcSend(p_ipc_ptr,q_dest_thread_id))
      {
        e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
        (void)os_IpcDelete(p_ipc_ptr);
      }
#endif

    }


  }/*if(p_params ==  NULL) else {}*/

  return e_result;
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM_CLIENT */

}


/*===========================================================================
FUNCTION pd_comms_send_hash_resp_to_client

DESCRIPTION
  This function sends the hash message output to TM.
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_send_hash_resp_to_client(
  /*thread id of the client*/  
  const uint32 q_dest_thread_id,
  /*base message ID to be added to the messageID when responding
  back to the client. The base messageID is provided in the hash_req call*/
  const uint32 q_base_msg_id,
  /*parameters of the hash output to be sent to TM*/
  const pd_comms_generate_hash_response_params_type * const p_params )
{
#ifdef FEATURE_CGPS_PDCOMM_CLIENT
  os_IpcMsgType* p_ipc_ptr = NULL;  
  pd_comms_ipc_hash_resp_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_OK;  

  if(p_params == NULL)
  {
      e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_ipc_hash_resp_hdr_type)+p_params->q_output_len +p_params->q_msg_len -1,
                           IPC_ALLOCATION_DYNAMIC,q_dest_thread_id);   

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (pd_comms_ipc_hash_resp_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

      p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_CLIENT_MSG_HASH_RESP + q_base_msg_id;
      p_ipc_ptr->q_DestThreadId   = q_dest_thread_id;
      p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_hash_resp_hdr_type);

      p_hdr_ptr->q_transaction_id = p_params->q_transaction_id;
      p_hdr_ptr->q_output_len = p_params->q_output_len;
      p_hdr_ptr->q_msg_len    = p_params->q_msg_len;
      
      memscpy(p_hdr_ptr->p_data,p_params->q_msg_len,p_params->p_msg,p_params->q_msg_len);
      
      memscpy(p_hdr_ptr->p_data + p_params->q_msg_len,p_params->q_output_len,p_params->p_hash_output,p_params->q_output_len);
                     
#if 0
      /* this function is currently not used by a remote client, so it has been commented out for now */
      e_result = pd_comms_send_ipc_to_client( 
        z_handle, 
        (uint32)C_PD_COMMS_CLIENT_MSG_HASH_RESP + q_base_msg_id,
        sizeof(*p_hdr_ptr), 
        p_hdr_ptr,
        p_ipc_ptr);
#endif
      if(!os_IpcSend(p_ipc_ptr,q_dest_thread_id))
      {
          e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
          (void)os_IpcDelete(p_ipc_ptr);
      }
      
    }


  }/*if(p_params ==  NULL) else {}*/

  return e_result;
#else
  return PDCOMM_RESULT_ERROR_UNKNOWN;
#endif  /* FEATURE_CGPS_PDCOMM_CLIENT */


}

