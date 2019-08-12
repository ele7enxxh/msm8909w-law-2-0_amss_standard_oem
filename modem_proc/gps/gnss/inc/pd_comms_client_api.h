/*======================================================================

                         PDCOMM APP API header file

 GENERAL DESCRIPTION
  This file contains API declarations by which PDCOMM Application can 
  communicate to it's clients (e.g Transport Manager). The file also 
  contains the HTORPC meta-comments to export these APIs using RPC.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/inc/pd_comms_client_api.h#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#ifndef PD_COMMS_CLIENT_API_H
#define PD_COMMS_CLIENT_API_H

#include "gps_variation.h"
#include "comdef.h"
#include "pd_comms_api.h"

#ifdef FEATURE_CGPS_LBS_TASK
/* A PD Comms client can register a callback function to receive
   PD Comms Client API notifications (instead of via os_IpcSend()).
   This functionality was added for remote clients i.e. on app processor 
   since os_IpcSend() does not work across processor boundaries. */
#define MAX_IPC_CB_TABLE_SIZE  12
typedef struct
{
  boolean              occupied;
  pd_comms_handle_type z_handle;
  pd_comms_app_ipc_send_type *ipc_send_func;
}ipc_func_table_entry;

extern ipc_func_table_entry ipc_func_table[MAX_IPC_CB_TABLE_SIZE];
#endif

/****************** Function Declarations**************************************/

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
  const pd_comms_handle_type z_handle,
  const uint32 q_dest_thread_id,
  const uint32 q_base_msg_id,
  const uint32 q_transaction_id,
  const pd_comms_app_notification_type e_notification,
  const pd_comms_notification_params_type * const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_notify_client*/


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
  const pd_comms_handle_type z_handle,
  const uint32 q_dest_thread_id,
  const uint32 q_base_msg_id,
  const uint32 q_transaction_id
);
/*~ FUNCTION pd_comms_send_handle_to_client*/


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
  const pd_comms_handle_type z_handle,
  const uint32 q_dest_thread_id,
  const uint32 q_base_msg_id,
  const uint32 q_transaction_id,
  const pd_comms_read_data_params_type * const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_senddata_to_client*/

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
  const pd_comms_handle_type z_handle,
  const uint32 q_dest_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_write_ack_params_type * const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_send_write_ack_to_client*/

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
  const pd_comms_handle_type z_handle,
  const uint32 q_dest_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_sms_ack_report_params_type * const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_send_sms_ack_report_to_client*/

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
  const pd_comms_handle_type z_handle,
  const uint32 q_dest_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_ioctl_ack_params_type * const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_send_ioctl_ack_to_client*/

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
  const uint32 q_dest_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_generate_hash_response_params_type * const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_send_hash_resp_to_client*/

#endif /* PD_COMMS_CLIENT_API_H */
