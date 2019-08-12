/*======================================================================

               PDCOMM local API declarations header file

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_tcp/inc/pd_comms_tcp_local_api.h#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
 02/24/11   ss   Adding function to handle APN profile change.
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#ifndef PD_COMMS_TCP_LOCAL_API_H
#define PD_COMMS_TCP_LOCAL_API_H

#include "comdef.h"
#include "pd_comms_api.h"
#include "sys.h"

/****************** Function Declarations**************************************/
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
);


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
);


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
);

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
);


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
);

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
);


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
);

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
); 

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
void pd_comms_client_srv_system_change_notification_handle(sys_sys_mode_e_type srv_system);

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
void pd_comms_client_tcp_set_data_enable_cb(boolean data_enable);

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
void pd_comms_client_apn_profile_change_notification_handle(uint32 apn_profile_type);


#endif /* PD_COMMS_TCP_LOCAL_API_H */
