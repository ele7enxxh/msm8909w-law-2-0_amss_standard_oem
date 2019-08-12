/*======================================================================

               PDCOMM local API definitions file

 GENERAL DESCRIPTION
   Local refers to APIs which are supposed to be called from the same
   proccessor as the PDCOMM Application and hence are not supposed to
   be exported using RPC. The developers of the PDCOMM Application or
   equivalent application have to implement these functions. It is 
   recommended that these functions don't actually do any processing 
   and simply send request messages to PDCOMM Application and return
   immediately so that the caller doesn't get blocked.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_tcp/src/pd_comms_tcp_local_api.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include <string.h>
#include <stdlib.h>

#include "customer.h"
#include "msg.h"

#ifdef   FEATURE_CGPS_PDCOMM 

#include "pd_comms_tcp_local_api.h"
#include "pd_comms_app_api.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "pd_comms_tcp_data.h"
#include "pdapi.h"


/****************** Function Definitions**************************************/

/*===========================================================================
FUNCTION pd_comms_client_tcp_get_handle_cb

DESCRIPTION
  This function returns a new pdcomm handle. A handle will be allocated by the 
  pd comms app task. This handle must be used in all other API functions to 
  identify the connection.

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_handle_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_tcp_get_handle_cb(
  /*get_handle parameters*/  
  const pd_comms_get_handle_params_type* const p_params,
  /*assigned handle is returned in this param*/
  pd_comms_handle_type* p_handle
)
{
  return pd_comms_tcp_task_handle_get_handle(p_params,p_handle);
} /*pd_comms_client_tcp_get_handle_cb*/


/*===========================================================================
FUNCTION pd_comms_client_tcp_connect_cb

DESCRIPTION
  This function opens a new connection. A handle will be allocated by the 
  application layer. This handle must be used in all other API functions 
  to identify the connection.

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_tcp_connect_cb( 
  /*handle corresponding to the connection to be opened*/  
  const pd_comms_handle_type z_handle,  
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
    responds back to client. This base_msg_id will be added to messages declared
    in pd_comms_client_ipc_message_type to get the final message_id when 
    PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*Pointer to structure containing connect parameters*/
  const pd_comms_connect_params_type * const p_params
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;
  pd_comms_ipc_connect_hdr_type* p_hdr_ptr = NULL;

  pd_comms_return_type e_result = PDCOMM_RESULT_WAIT;  

  if(p_params == NULL)
  {
      e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr =  os_IpcCreate(sizeof(pd_comms_ipc_connect_hdr_type),
                              IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP); 
                

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_connect_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

      p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_CONNECT;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_TCP;
      p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_connect_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->z_params           = *p_params;
         

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/
  

  return e_result;

}/*pd_comms_client_tcp_connect_cb*/


/*===========================================================================
FUNCTION pd_comms_client_tcp_disconnect_cb

DESCRIPTION
  This function closes the connection associated with the given handle. 

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_tcp_disconnect_cb(
  /*handle corresponding to the connection to be closed*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*transaction ID*/
  const uint32 q_transaction_id,
  /*to abort or gracefully close*/
  const boolean u_abort
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;
  pd_comms_ipc_disconnect_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_WAIT;  

  
  
  
  p_ipc_ptr =  os_IpcCreate(sizeof(pd_comms_ipc_disconnect_hdr_type),
                         IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);
          
  if(p_ipc_ptr == NULL)
  {
    e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
  }
  else
  {
    /*Pack the message in IPC HDR*/
	p_hdr_ptr = (pd_comms_ipc_disconnect_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

	p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_DISCONNECT;
	p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_TCP;
	p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_disconnect_hdr_type);

    p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
	p_hdr_ptr->q_transaction_id = q_transaction_id;
    p_hdr_ptr->z_handle           = z_handle;
	p_hdr_ptr->abort            = u_abort;


    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
    {
       e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
	   (void)os_IpcDelete(p_ipc_ptr);
    }

  }


  return e_result;

}/*pd_comms_client_tcp_disconnect_cb*/

/*===========================================================================
FUNCTION pd_comms_client_tcp_bind_cb

DESCRIPTION
  This function binds to the socket.
  
DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_tcp_bind_cb(
  /*handle corresponding to the connection to be bound*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*Pointer to structure containing bind parameters*/
  const pd_comms_bind_params_type* const p_params
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;
  pd_comms_ipc_bind_hdr_type* p_hdr_ptr = NULL;

  pd_comms_return_type e_result = PDCOMM_RESULT_WAIT;  

  if(p_params == NULL)
  {
     e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
   p_ipc_ptr =  os_IpcCreate(sizeof(pd_comms_ipc_connect_hdr_type),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP); 
               

   if(p_ipc_ptr == NULL)
   {
     e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
   }
   else
   {
     /*Pack the message in IPC HDR*/
     p_hdr_ptr = (pd_comms_ipc_bind_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

     p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_BIND;
     p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_TCP;
     p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_bind_hdr_type);

     p_hdr_ptr->q_source_thread_id = q_source_thread_id;
     p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
     p_hdr_ptr->z_handle           = z_handle;
     p_hdr_ptr->z_params           = *p_params;
        

     if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
     {
        e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
    (void)os_IpcDelete(p_ipc_ptr);
     }

   }


  }/*if(p_params ==  NULL) else {}*/


  return e_result;

}


/*===========================================================================
FUNCTION pd_comms_client_tcp_write_cb

DESCRIPTION
  This function writes data to the specified connection. 

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_tcp_write_cb( 
  /*handle corresponding to the connection to be written to.*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client.*/
  const uint32 q_base_msg_id,
  /*write parameters corresponding to the link type*/
  const pd_comms_write_params_type * const p_params
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;
  pd_comms_ipc_write_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_WAIT;  

  if(p_params == NULL)
  {
      e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr =   os_IpcCreate(sizeof(pd_comms_ipc_write_hdr_type)+p_params->w_data_len-1,
                              IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_write_hdr_type*) p_ipc_ptr->p_Data;  /*lint !e826*/

	  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_WRITE;
	  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_TCP;
	  p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_write_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->q_transaction_id   = p_params->q_transaction_id;
      p_hdr_ptr->z_tl_write_params  = p_params->z_tl_write_params;
      p_hdr_ptr->w_data_len         = p_params->w_data_len;
      memscpy(p_hdr_ptr->p_data, p_params->w_data_len, p_params->p_data, p_params->w_data_len);


      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/


  return e_result;
}/*pd_comms_client_tcp_write_cb*/


/*===========================================================================
FUNCTION pd_comms_client_tcp_read_cb

DESCRIPTION
  This function reads data from a specified connection. 

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_tcp_read_cb( 
  /*handle corresponding to the connection to be read from*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client.*/
  const uint32 q_base_msg_id,
  /*read parameters corresponding to the link type*/
  const pd_comms_read_params_type * const p_params
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;
  pd_comms_ipc_read_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_WAIT;  

  if(p_params == NULL)
  {
      e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr =   os_IpcCreate(sizeof(pd_comms_ipc_read_hdr_type),
                              IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_read_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

	  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_READ;
	  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_TCP;
	  p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_read_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->p_params           = *p_params;
      
      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/


  return e_result;
}/*pd_comms_client_tcp_read_cb*/


/*===========================================================================
FUNCTION pd_comms_client_tcp_ioctl_cb

DESCRIPTION
  This function provides get/set I/O parameter functionality. The exact I/O 
  option depends upon the specific kind of transport/data service being used.

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_tcp_ioctl_cb(
  /*handle corresponding to the connection for which ioctl is needed*/
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*ioctl parameters*/
  const pd_comms_ioctl_params_type * const p_params
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;
  pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_WAIT;  

  if(p_params == NULL)
  {
      e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_ipc_ioctl_hdr_type)+p_params->q_argument_len-1,
                           IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_ioctl_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

	  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_IOCTL;
	  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_TCP;
	  p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_ioctl_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->q_transaction_id   = p_params->q_transaction_id;
      p_hdr_ptr->q_ioctl_request    = p_params->q_ioctl_request;
      p_hdr_ptr->q_argument_len     = p_params->q_argument_len;
      memscpy(p_hdr_ptr->p_data, p_params->q_argument_len, p_params->p_argument, p_params->q_argument_len);


      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/


  return e_result;
}/*pd_comms_client_tcp_ioctl_cb*/


/*===========================================================================
FUNCTION pd_comms_client_tcp_release_handle_cb

DESCRIPTION
  This function releases a pdcomm handle previously allocated by calling
  pd_comms_app_get_handle(). If the connection is in CONNECTED state, this call
  will also abort the connection before releasing handle.

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_handle_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_tcp_release_handle_cb(
  /*contains the handle allocated by pdcomm-task */  
  const pd_comms_handle_type* p_handle
)
{
  return pd_comms_tcp_task_handle_release_handle(p_handle);  
}/*pd_comms_client_tcp_release_handle_cb*/


/*===========================================================================
FUNCTION pd_comms_client_srv_system_change_notification_handle

DESCRIPTION
  This function handles the serving system change notification.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_client_srv_system_change_notification_handle(sys_sys_mode_e_type srv_system)
{
  
  os_IpcMsgType* p_ipc_ptr = NULL;
  uint32* data_p = NULL;
  
  p_ipc_ptr = os_IpcCreate(sizeof(uint32),
                         IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP); 
               
  
  if(p_ipc_ptr == NULL)
  {
    MSG_ERROR("Failure during Ipc creation",0,0,0);
    return;
  }

  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_SRV_SYS_CHANGE_NOTIFICATION;
  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_TCP;
  p_ipc_ptr->q_Size           = sizeof(uint32);

  data_p = (uint32*)p_ipc_ptr->p_Data;

  *(data_p) = (uint32)srv_system;
  
  if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
  {    
    MSG_ERROR("Failure during Ipc Send",0,0,0);
   (void)os_IpcDelete(p_ipc_ptr);
  }
  
}

/*===========================================================================
FUNCTION pd_comms_client_apn_profile_change_notification_handle

DESCRIPTION
  This function handles the apn profile change notification.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_client_apn_profile_change_notification_handle(uint32 apn_profile_type)
{
#ifndef FEATURE_GNSS_BIT_API
  cgps_nv_efs_reg_item_type item = CGPS_NV_EFS_MAX_NUM_ITEMS;
  uint32 profile_type = PDCOMM_APN_PROFILE_TYPE_MAX;

  
  if(apn_profile_type == PDSM_PA_LBS_APN_PROFILE)
  {
    item = CGPS_NV_EFS_PDCOMM_LBS_APN_PROFILES;
    profile_type = PDCOMM_APN_PROFILE_TYPE_LBS;
  }
  else if( apn_profile_type == PDSM_PA_XTRA_APN_PROFILE)
  {
    item = CGPS_NV_EFS_PDCOMM_XTRA_APN_PROFILES;
    profile_type = PDCOMM_APN_PROFILE_TYPE_XTRA;
  }
  else if( apn_profile_type == PDSM_PA_EMERGENCY_LBS_APN_PROFILE)
  {
    item = CGPS_NV_EFS_PDCOMM_EMERGENCY_LBS_APN_PROFILES;
    profile_type = PDCOMM_APN_PROFILE_TYPE_LBS_EMERGENCY;
  }  
  else
  {
    MSG_ERROR("Apn profile type %d not supported",apn_profile_type,0,0);
  }

  (void)pd_comms_tcp_task_read_apn_profiles_from_nv(item,profile_type, FALSE);
#else
  MSG_ERROR("APN profiles NV are not supported when BIT API is enabled",0,0,0);    
#endif /* FEATURE_GNSS_BIT_API */
}

/*===========================================================================
FUNCTION pd_comms_client_tcp_set_data_enable_cb

DESCRIPTION
  This function enables/disables data

DEPENDENCIES
  None

RETURN VALUE
  void
SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_client_tcp_set_data_enable_cb(boolean data_enable)
{
#ifndef FEATURE_GNSS_BIT_API
  pd_comms_tcp_task_handle_set_data_enable(data_enable);
#else
    MSG_ERROR("Data Enable can not be set when BIT API is enabled",0,0,0);  
#endif /* FEATURE_GNSS_BIT_API */
}



#endif /* FEATURE_CGPS_PDCOMM */
