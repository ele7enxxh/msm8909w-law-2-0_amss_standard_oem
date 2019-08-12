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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_wms/src/pd_comms_wms_local_api.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
 09/03/12   rh   Added inject msg API
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#include <string.h>
#include <stdlib.h>

#include "pd_comms_wms_local_api.h"
#include "pd_comms_app_api.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "pd_comms_wms_data.h"


/****************** Function Definitions**************************************/

/*===========================================================================
FUNCTION pd_comms_client_wms_get_handle_cb

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
pd_comms_return_type pd_comms_client_wms_get_handle_cb(
  /*get_handle parameters*/    
  const pd_comms_get_handle_params_type* const p_params,
  /*assigned handle is returned in this param*/
  pd_comms_handle_type* p_handle
)
{
  return pd_comms_wms_task_handle_get_handle(p_params,p_handle);
}/*pd_comms_client_wms_get_handle_cb*/


/*===========================================================================
FUNCTION pd_comms_client_wms_connect_cb

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
pd_comms_return_type pd_comms_client_wms_connect_cb( 
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
                              IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_WMS); 
                

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_connect_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

      p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_CONNECT;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_WMS;
      p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_connect_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->z_params           = *p_params;
         

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_WMS))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/
  

  return e_result;

}/*pd_comms_client_wms_connect_cb*/

/*===========================================================================
FUNCTION pd_comms_client_wms_disconnect_cb

DESCRIPTION
  This function closes the connection associated with the given handle. 

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_wms_disconnect_cb(
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
                         IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_WMS);
          
  if(p_ipc_ptr == NULL)
  {
    e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
  }
  else
  {
    /*Pack the message in IPC HDR*/
	p_hdr_ptr = (pd_comms_ipc_disconnect_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

	p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_DISCONNECT;
	p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_WMS;
	p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_disconnect_hdr_type);

    p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
	p_hdr_ptr->q_transaction_id = q_transaction_id;
    p_hdr_ptr->z_handle           = z_handle;
	p_hdr_ptr->abort            = u_abort;


    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_WMS))
    {
       e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
	   (void)os_IpcDelete(p_ipc_ptr);
    }

  }


  return e_result;

}/*pd_comms_client_wms_disconnect_cb*/


/*===========================================================================
FUNCTION pd_comms_client_wms_write_cb

DESCRIPTION
  This function writes data to the specified connection. 

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_wms_write_cb( 
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
                              IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_WMS); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_write_hdr_type*) p_ipc_ptr->p_Data;  /*lint !e826*/

	  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_WRITE;
	  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_WMS;
	  p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_write_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->q_transaction_id   = p_params->q_transaction_id;
      p_hdr_ptr->z_tl_write_params  = p_params->z_tl_write_params;
      p_hdr_ptr->w_data_len         = p_params->w_data_len;
      memscpy(p_hdr_ptr->p_data, p_params->w_data_len, p_params->p_data, p_params->w_data_len);


      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_WMS))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/


  return e_result;
}/*pd_comms_client_wms_write_cb*/


/*===========================================================================
FUNCTION pd_comms_client_wms_read_cb

DESCRIPTION
  This function reads data from a specified connection. 

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_wms_read_cb( 
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
                              IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_WMS); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_read_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

	  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_READ;
	  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_WMS;
	  p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_read_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->p_params           = *p_params;
      
      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_WMS))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/


  return e_result;
}/*pd_comms_client_wms_read_cb*/


/*===========================================================================
FUNCTION pd_comms_client_generate_hash_cb

DESCRIPTION
  This function generates a hash based upon a given data payload. 

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_generate_hash_cb(
  /*thread id of the client*/  
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*parameters for the message on which hash function is to be applied
    The following fields are included in the hash_params structure: 
     hash_type - Hash algorith to use
     msg - Pointer to data payload upon which the hash shall be generated
     msg_len - length in bytes of the data payload
     hash_output - Pointer to data buffer in which to store the generated hash*/
  const pd_comms_generate_hash_params_type* const p_params
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;
  pd_comms_ipc_hash_req_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_WAIT;  

  if(p_params == NULL)
  {
      e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr =  os_IpcCreate
		       (
		       sizeof(pd_comms_ipc_hash_req_hdr_type)+p_params->q_msg_len+p_params->w_key_len-1,
               IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_WMS
			   ); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_hash_req_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

	  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_HASH_REQ;
	  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_WMS;
	  p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_hash_req_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->q_transaction_id   = p_params->q_transaction_id;
      p_hdr_ptr->e_hash_type        = p_params->e_hash_type;
      p_hdr_ptr->q_msg_len          = p_params->q_msg_len;
      p_hdr_ptr->w_key_len          = p_params->w_key_len;
      memscpy(p_hdr_ptr->p_data, p_params->q_msg_len, p_params->p_msg, p_params->q_msg_len);
      memscpy(p_hdr_ptr->p_data + p_params->q_msg_len, p_params->w_key_len, p_params->p_key_ptr, p_params->w_key_len);


      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_WMS))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/


  return e_result;
}/*pd_comms_client_generate_hash_cb*/

/*===========================================================================
FUNCTION pd_comms_client_wms_ioctl_cb

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
pd_comms_return_type pd_comms_client_wms_ioctl_cb(
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
                           IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_WMS); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
	  p_hdr_ptr = (pd_comms_ipc_ioctl_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

	  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_IOCTL;
	  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_WMS;
	  p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_ioctl_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->q_transaction_id   = p_params->q_transaction_id;
      p_hdr_ptr->q_ioctl_request    = p_params->q_ioctl_request;
      p_hdr_ptr->q_argument_len     = p_params->q_argument_len;
      memscpy(p_hdr_ptr->p_data, p_params->q_argument_len, p_params->p_argument, p_params->q_argument_len);


      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_WMS))
      {
         e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		 (void)os_IpcDelete(p_ipc_ptr);
      }

    }


  }/*if(p_params ==  NULL) else {}*/


  return e_result;
}/*pd_comms_client_wms_ioctl_cb*/


/*===========================================================================
FUNCTION pd_comms_client_wms_release_handle_cb

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
pd_comms_return_type pd_comms_client_wms_release_handle_cb(
  /*contains the handle allocated by pdcomm-task */    
  const pd_comms_handle_type* p_handle
)
{
  return pd_comms_wms_task_handle_release_handle(p_handle);  
}/*pd_comms_client_wms_release_handle_cb*/

/*===========================================================================
FUNCTION pd_comms_client_wms_inject_msg_cb

DESCRIPTION
  This function injects a msg into PDCOMM WMS's read buffer

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_handle_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_client_wms_inject_msg_cb(
  /*contains the handle allocated by pdcomm-task */    
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client.*/
  const uint32 q_base_msg_id,
  /*read parameters corresponding to the link type*/
  const pd_comms_inject_msg_params_type * const p_params
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;
  pd_comms_ipc_inject_msg_hdr_type* p_hdr_ptr = NULL;
  pd_comms_return_type e_result = PDCOMM_RESULT_WAIT;  

  if(p_params == NULL)
  {
      e_result = PDCOMM_RESULT_ERROR_BAD_BUFFER;
  }
  else
  {
    p_ipc_ptr =   os_IpcCreate(sizeof(pd_comms_ipc_inject_msg_hdr_type),
                              IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_WMS); 
                 

    if(p_ipc_ptr == NULL)
    {
      e_result = PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
  	  p_hdr_ptr = (pd_comms_ipc_inject_msg_hdr_type*) p_ipc_ptr->p_Data; /*lint !e826*/

	  p_ipc_ptr->q_MsgId          = (uint32)C_PD_COMMS_APP_MSG_INJECT_MSG;
	  p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_PDCOMM_WMS;
	  p_ipc_ptr->q_Size           = sizeof(pd_comms_ipc_inject_msg_hdr_type);

      p_hdr_ptr->q_source_thread_id = q_source_thread_id;
	  p_hdr_ptr->q_base_msg_id      = q_base_msg_id;
      p_hdr_ptr->z_handle           = z_handle;
      p_hdr_ptr->z_params           = *p_params;
      
      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_WMS))
      {
        e_result = PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE;
		    (void)os_IpcDelete(p_ipc_ptr);
      }
    }

  }/*if(p_params ==  NULL) else {}*/

  return e_result;

}/*pd_comms_client_wms_inject_msg_cb*/

