/*======================================================================

               PDCOMM IPC structures header file

 GENERAL DESCRIPTION
  This file contains constants and data-structures used for packing 
  PDCOMM request/response params into IPC messages.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/inc/pd_comms_app_api.h#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#ifndef PD_COMMS_APP_API_H
#define PD_COMMS_APP_API_H

#include "comdef.h"
#include "pd_comms_api.h"
#include "aries_os_globals.h"


/****************** Constants/Enums *******************************************/

typedef enum
{
  C_PD_COMMS_APP_MSG_CONNECT = C_USR_MSG_ID_FIRST+1,
  C_PD_COMMS_APP_MSG_DISCONNECT,
  C_PD_COMMS_APP_MSG_WRITE,
  C_PD_COMMS_APP_MSG_IOCTL,
  C_PD_COMMS_APP_MSG_HASH_REQ,
  C_PD_COMMS_APP_MSG_READ,  
  C_PD_COMMS_APP_SRV_SYS_CHANGE_NOTIFICATION,
  C_PD_COMMS_APP_MSG_BIND,
  C_PD_COMMS_APP_MSG_INJECT_MSG,
  C_PD_COMMS_APP_MSG_IPC_END /*add any new message type before this*/
} pd_comms_app_ipc_message_type;

/****************** Data Structures*********************************************/

typedef struct
{
  pd_comms_handle_type z_handle;  
  uint32 q_source_thread_id;
  uint32 q_base_msg_id;
  pd_comms_connect_params_type z_params; /*params also contains the transaction-Id*/
} pd_comms_ipc_connect_hdr_type;

typedef struct
{
  uint32 q_source_thread_id;
  uint32 q_base_msg_id;
  uint32 q_transaction_id;
  pd_comms_handle_type z_handle;
  boolean abort;
} pd_comms_ipc_disconnect_hdr_type;

typedef struct
{
  pd_comms_handle_type z_handle;  
  uint32 q_source_thread_id;
  uint32 q_base_msg_id;
  pd_comms_bind_params_type z_params; /*params also contains the transaction-Id*/
} pd_comms_ipc_bind_hdr_type;


typedef struct
{
  uint32 q_source_thread_id;
  uint32 q_base_msg_id;
  pd_comms_handle_type z_handle;
  uint32 q_transaction_id;
  pd_comms_tl_specific_write_params_type z_tl_write_params;
  uint16 w_data_len;
  char p_data[1]; /*data to be written starts here */
} pd_comms_ipc_write_hdr_type;

typedef struct
{
  uint32 q_source_thread_id;
  uint32 q_base_msg_id;
  pd_comms_handle_type z_handle;
  pd_comms_read_params_type p_params; /*params also contains the transaction-Id*/ 
} pd_comms_ipc_read_hdr_type;

typedef struct
{
  uint32 q_source_thread_id;
  uint32 q_base_msg_id;
  pd_comms_handle_type z_handle;
  uint32 q_transaction_id;
  pd_comms_ioctl_cmd_type q_ioctl_request;
  uint32 q_argument_len;
  char p_data[1]; /*ioctl argument of size argument_len starts here*/
} pd_comms_ipc_ioctl_hdr_type;

typedef struct
{
  uint32 q_source_thread_id;
  uint32 q_base_msg_id;
  uint32 q_transaction_id;
  pd_comms_hash_type e_hash_type;
  uint32 q_msg_len;
  uint16 w_key_len;
  char p_data[1]; /*The first msg_len bytes of this buffer 
                 *contains the message to be hashed and
                 *the last key_len bytes contains the
                  hash key to be used                 */
} pd_comms_ipc_hash_req_hdr_type;

typedef struct
{
  pd_comms_handle_type z_handle;  
  uint32 q_source_thread_id;
  uint32 q_base_msg_id;
  pd_comms_inject_msg_params_type z_params; /*params also contains the transaction-Id*/
} pd_comms_ipc_inject_msg_hdr_type;

#endif /* PD_COMMS_APP_API_H */
