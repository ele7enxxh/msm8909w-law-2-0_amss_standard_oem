/*======================================================================

               PDCOMM common declarations and API header file

 GENERAL DESCRIPTION
  This file contains constants and data-structures commmon to various
  files of PDCOMM. This includes PDCOMM API return values and function
  parameter types. 

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/inc/pd_comms_api.h#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 01/27/14   mj  Modified pd_comms_sms_parse_msg_cb_type with new params
 09/03/12   rh   Added inject msg PDCOMM API
 11/14/06   mr   Added HTORPC meta-comments  
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#ifndef PD_COMMS_API_H
#define PD_COMMS_API_H

#include "comdef.h"
#include "wms.h"
#include "sys.h"
#include "pdapi.h"
#include "ps_in.h"

/*APN Profile Type*/
#define PDCOMM_APN_PROFILE_TYPE_LBS   			0
#define PDCOMM_APN_PROFILE_TYPE_XTRA  			1
#define PDCOMM_APN_PROFILE_TYPE_LBS_EMERGENCY 	2
#define PDCOMM_APN_PROFILE_TYPE_MAX   			3


/****************** Constants/Enums *******************************************/

/*Immediate return values all the API calls can have*/
/*PDCOMM_RESULT_OK and PDCOMM_RESULT_WAIT can also
  come as asynchronously delivered return values  */
typedef enum
{
  PDCOMM_RESULT_OK = 0,
  PDCOMM_RESULT_WAIT,
  PDCOMM_RESULT_ERROR_UNKNOWN, 
  PDCOMM_RESULT_ERROR_IPC_CREATE_FAILURE,
  PDCOMM_RESULT_ERROR_IPC_SEND_FAILURE,
  PDCOMM_RESULT_ERROR_RPC_FAILURE,
  PDCOMM_RESULT_ERROR_BAD_BUFFER,
  PDCOMM_RESULT_ERROR_SERVICE_NOT_REGISTERED,
  PDCOMM_RESULT_ERROR_NO_MORE_HANDLES,  
  PDCOMM_RESULT_ERROR_DATA_DISABLED,
  PDCOMM_RETURN_TYPE_END /*add any new return values before this*/
} pd_comms_return_type;       

typedef enum
{
  PDCOMM_CONNECT_RESULT_TYPE_BEGIN = PDCOMM_RETURN_TYPE_END +1,

  PDCOMM_CONNECT_ERROR_SOCKET_CONNECTION_FAILED, 
  PDCOMM_CONNECT_ERROR_INVALID_PARAMETERS,
  PDCOMM_CONNECT_ERROR_PPP_OPEN_FAILURE,
  PDCOMM_CONNECT_ERROR_HOST_UNREACHABLE,
  PDCOMM_CONNECT_ERROR_HTTP_GATEWAY_UNREACHABLE,
  PDCOMM_CONNECT_ERROR_HTTP_NOT_FOUND,
  PDCOMM_CONNECT_ERROR_HTTP_TIMEOUT,
  PDCOMM_CONNECT_ERROR_SECURITY_FAILURE,
  PDCOMM_CONNECT_ERROR_CONNECTION_REFUSED,
  PDCOMM_CONNECT_ERROR_CONNECTION_TIMED_OUT,
  PDCOMM_CONNECT_ERROR_CONNECT_NETWORK_DOWN,
  PDCOMM_CONNECT_ERROR_CONNECT_ADDR_IN_USE,
  PDCOMM_CONNECT_ERROR_DNS_LOOKUP_FAILED,
  PDCOMM_CONNECT_ERROR_APP_NOT_INITIALIZED,
  PDCOMM_CONNECT_ERROR_NETWORK_LIB_INIT_FAILURE,
  PDCOMM_CONNECT_ERROR_TCP_CONNECTION_RESET,
  PDCOMM_CONNECT_ERROR_UNSUPPORTED_OPERATION,
  PDCOMM_CONNECT_ERROR_CONNECTION_FAILED_INTERNAL_ERROR,
  PDCOMM_CONNECT_ERROR_TCP_CONNECTION_REFUSED,
  PDCOMM_CONNECT_ERROR_PROTOCOL_VER_NOTSUPPORTED,
  PDCOMM_CONNECT_ERROR_UNKNOWN,
  PDCOMM_CONNECT_ERROR_PREVIOUS_CONNECTION_ACTIVE,
  PDCOMM_CONNECT_ERROR_NO_MORE_HANDLES,

  PDCOMM_CONNECT_RESULT_TYPE_END /*add any new return values before this*/
} pd_comms_connect_result_type;

typedef enum
{ 
  PDCOMM_DISCONNECT_RESULT_TYPE_BEGIN = PDCOMM_CONNECT_RESULT_TYPE_END + 1,

  PDCOMM_DISCONNECT_ERROR_INVALID_HANDLE,
  PDCOMM_DISCONNECT_ERROR_UNKNOWN,
  PDCOMM_DISCONNECT_ERROR_INVALID_PARAMETERS,

  PDCOMM_DISCONNECT_RESULT_TYPE_END /*add any new return values before this*/
} pd_comms_disconnect_result_type;

typedef enum
{
  PDCOMM_WRITE_RESULT_TYPE_BEGIN = PDCOMM_DISCONNECT_RESULT_TYPE_END + 1,

  PDCOMM_WRITE_ERROR_INVALID_HANDLE,
  PDCOMM_WRITE_ERROR_INVALID_PARAMETERS,
  PDCOMM_WRITE_ERROR_LINK_NOT_CONNECTED,
  PDCOMM_WRITE_ERROR_PREVIOUS_WRITE_PENDING,
  PDCOMM_WRITE_ERROR_WRITE_INTERNAL_ERROR,
  PDCOMM_WRITE_ERROR_WRITE_TCP_CONNECTION_LOST,
  PDCOMM_WRITE_ERROR_WRITE_PPP_CONNECTION_LOST,
  PDCOMM_WRITE_ERROR_BAD_BUFFER,
  PDCOMM_WRITE_ERROR_CONNECTION_CLOSED_BY_PEER,
  PDCOMM_WRITE_ERROR_CONNECTION_ABORTED,
  PDCOMM_WRITE_ERROR_NETWORK_DOWN,
  PDCOMM_WRITE_ERROR_WRITE_CONNECTION_LOST,
  PDCOMM_WRITE_ERROR_UNKNOWN,

  PDCOMM_WRITE_RESULT_TYPE_END /*add any new return values before this*/
} pd_comms_write_result_type;

typedef enum
{
  PDCOMM_HASH_RESULT_TYPE_BEGIN = PDCOMM_WRITE_RESULT_TYPE_END + 1,

  PDCOMM_HASH_ERROR_INVALID_PARAMETERS,
  PDCOMM_HASH_ERROR_UNKNOWN,

  PDCOMM_HASH_RESULT_TYPE_END /*add any new return values before this*/
} pd_comms_hash_result_type;

typedef enum
{
  PDCOMM_IOCTL_RESULT_TYPE_BEGIN = PDCOMM_HASH_RESULT_TYPE_END + 1,

  PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS,
  PDCOMM_IOCTL_ERROR_LINK_NOT_CONNECTED,
  PDCOMM_IOCTL_ERROR_BAD_BUFFER,
  PDCOMM_IOCTL_ERROR_CONNECTION_ABORTED,
  PDCOMM_IOCTL_ERROR_UNKNOWN,
  PDCOMM_IOCTL_ERROR_FORCE_DORMANCY_FAILED,  
  PDCOMM_IOCTL_ERROR_UNFORCE_DORMANCY_FAILED,    
  PDCOMM_IOCTL_ERROR_GET_IP_ADDR_FAILED,	

  PDCOMM_IOCTL_RESULT_TYPE_END /*add any new return values before this*/
} pd_comms_ioctl_result_type;

typedef enum 
{
  PDCOMM_READ_RESULT_TYPE_BEGIN = PDCOMM_IOCTL_RESULT_TYPE_END,

  PDCOMM_READ_ERROR_INTERNAL,
  PDCOMM_READ_ERROR_TCP_CONNECTION_LOST,
  PDCOMM_READ_ERROR_PPP_CONNECTION_LOST,
  PDCOMM_READ_ERROR_CONNECTION_LOST,
  PDCOMM_READ_ERROR_PREVIOUS_READ_PENDING,
  PDCOMM_READ_ERROR_UNKNOWN,

  PDCOMM_READ_RESULT_TYPE_END
} pd_comms_read_result_type;

typedef enum
{
  LINK_HTTP = 0,
  LINK_TCP,
  LINK_SMS,
  LINK_UDP,
  LINK_TYPE_MAX
} pd_comms_link_type;

typedef enum
{
  PD_COMMS_IP_ADDR,
  PD_COMMS_HOST_NAME,
  PD_COMMS_HOST_NAME_AND_IP_ADDR
} pd_comms_tcp_params_host_type;

typedef enum
{
  HTTP_GET,
  HTTP_POST
} pd_comms_http_req_type;

typedef enum
{
  SECURITY_NONE,
  SECURITY_SUPL /*use security required as per SUPL specs*/
} pd_comms_security_type;

typedef enum
{
  SSL_VER_TLS_1_0,
  SSL_VER_TLS_1_1 /*use security required as per SUPL specs*/
} pd_comms_tls_ver_type;


typedef enum
{
  STATE_CHANGE,
  ERROR_RPT,
  READ_DATA  
} pd_comms_app_notification_type;

typedef enum
{
  SHA1,
  SHA256
} pd_comms_hash_type;

typedef enum
{
  PDCOMM_STATE_DISCONNECTED,
  PDCOMM_STATE_OPENING,    /*optional state. This is an internal state for
                             PDCOMM APP about which client is not notified*/
  PDCOMM_STATE_CONNECTING, /*optional state*/
  PDCOMM_STATE_CONNECTED,
  PDCOMM_STATE_DISCONNECTING /*optional state*/
} pd_comms_state_type;

typedef enum
{
  PDCOMM_IOCTL_CMD_MIN,  
  PDCOMM_IOCTL_CMD_FORCE_DORMANCY,
  PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY,  
  PDCOMM_IOCTL_CMD_SET_SSL_CERT,  
  PDCOMM_IOCTL_SMS_SEND_MSG_ACK,
  PDCOMM_IOCTL_SMS_DC_CONNECT,
  PDCOMM_IOCTL_SMS_DC_DISCONNECT,    
  PDCOMM_IOCTL_CMD_GET_IP_ADDR,  
  PDCOMM_IOCTL_CMD_SET_SSL_CERT_DIR,  
  PDCOMM_IOCTL_CMD_MAX  
} pd_comms_ioctl_cmd_type;

typedef enum
{
   PD_COMMS_IP_ADDR_UNSPEC,
   PD_COMMS_IP_ADDR_IPV4,
   PD_COMMS_IP_ADDR_IPV6
} pd_comms_tcp_ip_addr_enum_type;

/****************** Data Structures*********************************************/

typedef struct
{
  pd_comms_tcp_params_host_type e_type;
  char   p_host_name[256]; /*~ FIELD pd_comms_tcp_connect_params_type.p_host_name STRING 256*/
  union
  {
    uint32 q_ip_addr_v4;
    struct ps_in6_addr q_ip_addr_v6;
  }q_ip_addr;
  pd_comms_tcp_ip_addr_enum_type ip_addr_type;  
  uint16 w_ip_port;
  uint32 profile_type;
} pd_comms_tcp_connect_params_type;

typedef struct
{
  uint16 w_ip_port;
} pd_comms_tcp_bind_params_type;

typedef struct
{
  pd_comms_http_req_type e_http_req;
  char p_url[256]; /*~ FIELD  pd_comms_http_connect_params_type.p_url STRING 256*/
} pd_comms_http_connect_params_type;

typedef boolean (*pd_comms_sms_parse_msg_cb_type)(
   const wms_client_message_s_type *p_data,
   /*~ PARAM p_data POINTER */
   wms_client_ts_data_s_type *p_ts_data,
   /*~ PARAM p_ts_data POINTER */
   uint8 *p_MesgContent,
   /*~ PARAM p_MesgContent VARRAY LENGTH u_MesgContentMaxLen */   
   uint16 *p_MesgLen,
   /*~ PARAM p_MesgLen POINTER */
   uint16 w_MesgContentMaxLen,
   boolean u_isWapPush
   );

typedef boolean (*pd_comms_sms_read_msg_cb_type)(
   wms_mt_message_info_s_type *p_data,
   /*~ PARAM p_data POINTER */
   wms_client_ts_data_s_type *p_ts_data,
   /*~ PARAM p_ts_data POINTER */
   uint8 *p_MesgContent,
   /*~ PARAM p_MesgContent VARRAY LENGTH u_MesgContentMaxLen */   
   uint16 *p_MesgLen,
   /*~ PARAM p_MesgLen POINTER */
   uint16 w_MesgContentMaxLen,
   boolean u_isWapPush
   );

typedef struct
{
  pd_comms_sms_parse_msg_cb_type p_parse_cb_func;
  pd_comms_sms_read_msg_cb_type p_read_msg_cb_func;
  boolean u_connectDcOnLargeSendFailure;
} pd_comms_sms_connect_params_type;

typedef union
{
  /*~ PARTIAL pd_comms_link_connect_params_type*/  
  pd_comms_http_connect_params_type z_http_params;
  pd_comms_tcp_connect_params_type z_tcp_params;
  pd_comms_sms_connect_params_type z_sms_params;
} pd_comms_link_connect_params_type;

typedef union
{
   /*~ PARTIAL pd_comms_sock_bind_params_type*/ 
   pd_comms_tcp_bind_params_type z_tcp_params;
}pd_comms_link_bind_params_type;

/*~ IF (_DISC_ == LINK_HTTP)                         pd_comms_link_connect_params_type.z_http_params */ 
/*~ IF ((_DISC_ == LINK_TCP) || (_DISC_ == LINK_UDP)) pd_comms_link_connect_params_type.z_tcp_params */

typedef struct
{
  uint32 q_base_msg_id;
  pd_comms_link_type e_link_type;  
} pd_comms_get_handle_params_type;

typedef struct
{
  pd_comms_link_type e_link_type;
  pd_comms_link_connect_params_type z_link_params; /*~ FIELD pd_comms_connect_params_type.z_link_params DISC pd_comms_connect_params_type.e_link_type */
  pd_comms_security_type e_use_security; /*specifies which security algorithm to use*/
  pd_comms_tls_ver_type  e_ssl_ver_tls;
  uint32 q_transaction_id; 
} pd_comms_connect_params_type;

typedef struct
{
  pd_comms_link_type e_link_type;
  pd_comms_link_bind_params_type z_link_params; /*~ FIELD pd_comms_connect_params_type.z_link_params DISC pd_comms_connect_params_type.e_link_type */
  uint32 q_transaction_id; 
} pd_comms_bind_params_type;


typedef struct
{
  wms_address_s_type           z_address;
  wms_teleservice_e_type       e_teleservice;
} pd_comms_sms_write_params_type;

/*Transport layer specific write params*/
typedef struct
{
  pd_comms_sms_write_params_type z_sms_write_params;
} pd_comms_tl_specific_write_params_type;

typedef struct
{
  uint16 w_data_len;
  uint8 *p_data; /*~ FIELD  pd_comms_write_params_type.p_data 
                   VARRAY LENGTH  pd_comms_write_params_type.w_data_len*/
  uint32 q_transaction_id; 
  pd_comms_tl_specific_write_params_type z_tl_write_params;
} pd_comms_write_params_type;

typedef struct
{
  uint16 w_data_len;
  boolean u_wait_for_all; 
  uint32 q_transaction_id; 
} pd_comms_read_params_type;

typedef struct
{
  uint16 w_data_len;
  uint8 *p_data; /*~ FIELD  pd_comms_read_data_params_type.p_data 
                   VARRAY LENGTH  pd_comms_read_data_params_type.w_data_len*/
} pd_comms_read_data_params_type;

typedef struct
{
  uint32 q_handle_id;
  pd_comms_link_type e_link_type;
} pd_comms_handle_type;


typedef struct
{
  pd_comms_ioctl_cmd_type q_ioctl_request; /*ioctl request/command specific to the transport type*/
  uint8* p_argument; /*~ FIELD  pd_comms_ioctl_params_type.p_argument 
                       VARRAY LENGTH  pd_comms_ioctl_params_type.q_argument_len*/
  uint32 q_argument_len;
  uint32 q_transaction_id;
} pd_comms_ioctl_params_type;

typedef struct
{
  pd_comms_ioctl_cmd_type q_ioctl_request;
  uint8* p_argument; /*~ FIELD  pd_comms_ioctl_ack_params_type.p_argument 
                       VARRAY LENGTH  pd_comms_ioctl_ack_params_type.q_argument_len*/
  uint32 q_argument_len;
  uint32 q_transaction_id;
} pd_comms_ioctl_ack_params_type;

typedef struct
{
  pd_comms_hash_type e_hash_type; 
  uint8 *p_msg; /*~ FIELD  pd_comms_generate_hash_params_type.p_msg 
                  VARRAY LENGTH  pd_comms_generate_hash_params_type.q_msg_len*/
  uint32 q_msg_len;
  uint8* p_key_ptr; /*~ FIELD  pd_comms_generate_hash_params_type.p_key_ptr
                      VARRAY LENGTH  pd_comms_generate_hash_params_type.w_key_len*/
  uint16 w_key_len;
  uint32 q_transaction_id;
} pd_comms_generate_hash_params_type;

typedef struct
{
  uint8* p_hash_output; /*~ FIELD  pd_comms_generate_hash_response_params_type.p_hash_output 
                          VARRAY LENGTH  pd_comms_generate_hash_response_params_type.q_output_len*/
  uint32 q_output_len;
  uint8* p_msg;  /*~ FIELD  pd_comms_generate_hash_response_params_type.p_msg 
                    VARRAY LENGTH  pd_comms_generate_hash_response_params_type.q_msg_len*/
  uint32 q_msg_len;
  uint32 q_transaction_id;
} pd_comms_generate_hash_response_params_type;

typedef struct
{
  pd_comms_state_type e_new_state;
  uint32 q_result;
  /*uint32 q_transaction_id;*/
} pd_comms_notification_params_type;

typedef struct
{
  uint32 q_bytes_written;
  uint32 q_transaction_id; 
} pd_comms_write_ack_params_type;

typedef struct
{
  boolean u_success;  
  uint32 q_transaction_id; 
} pd_comms_sms_ack_report_params_type;

typedef struct
{
  pd_comms_link_type e_link_type;
  uint8* p_msg;  /*~ FIELD  pd_comms_inject_msg_params_type.p_msg 
                    VARRAY LENGTH  pd_comms_inject_msg_params_type.q_msg_len*/
  uint32 q_msg_len;
  uint32 q_transaction_id; 
} pd_comms_inject_msg_params_type;


/***************IPC Headers***************************************************/

/****************** Constants/Enums ******************************************/

/*A base index will be provided by the client with the connect call. This will
 *be added to this enumeration to get the final message ID. This will give the 
 *flexibility to each individual client to choose it's own range of message IDs*/
typedef enum
{
  C_PD_COMMS_CLIENT_MSG_WRITE_ACK,
  C_PD_COMMS_CLIENT_MSG_READ_DATA,
  C_PD_COMMS_CLIENT_MSG_NOTIFY,
  C_PD_COMMS_CLIENT_MSG_IOCTL_ACK,
  C_PD_COMMS_CLIENT_MSG_HASH_RESP,
  C_PD_COMMS_CLIENT_MSG_HANDLE,
  C_PD_COMMS_CLIENT_MSG_SMS_ACK_REPORT,  
  C_PD_COMMS_CLIENT_MSG_IPC_END /*add any new message type before this*/
} pd_comms_client_ipc_message_type;

typedef enum
{
  PD_COMMS_FUNC_GET_HANDLE = 0x01,
  PD_COMMS_FUNC_CONNECT = 0x02,
  PD_COMMS_FUNC_DISCONNECT = 0x04,
  PD_COMMS_FUNC_WRITE = 0x08,
  PD_COMMS_FUNC_GENERATE_HASH = 0x10,
  PD_COMMS_FUNC_IOCTL = 0x20,
  PD_COMMS_FUNC_READ = 0x40,
  PD_COMMS_FUNC_RELEASE_HANDLE = 0x80,
  PD_COMMS_FUNC_SRV_SYSTEM_NOTIFICATION = 0x100,
  PD_COMMS_FUNC_BIND = 0x0200,  
  PD_COMMS_FUNC_SET_DATA_ENABLE = 0x0400,
  PD_COMMS_FUNC_APN_PROFILE_CHANGE_NOTIFICATION = 0x800,
  PD_COMMS_FUNC_INJECT_MSG = 0x1000 
} pd_comms_function_mask;


/****************** Data Structures*********************************************/

typedef struct
{
  uint32 q_transaction_id;
  pd_comms_handle_type z_handle;
} pd_comms_ipc_handle_hdr_type;



typedef struct
{
  uint32 q_transaction_id; 
  pd_comms_handle_type z_handle;
  uint32 q_bytes_written;
} pd_comms_ipc_write_ack_hdr_type;

typedef struct
{
  uint32 q_transaction_id;
  pd_comms_handle_type z_handle;
  uint16 w_data_len;
  char p_data[1]; /*data read starts here */
} pd_comms_ipc_read_data_hdr_type;

typedef struct
{
  uint32 q_transaction_id;
  pd_comms_handle_type z_handle;
  pd_comms_app_notification_type e_type;
  pd_comms_notification_params_type z_params;
} pd_comms_ipc_notification_hdr_type;

typedef struct
{
  uint32 q_transaction_id;
  pd_comms_handle_type z_handle;
  uint32 q_ioctl_request;
  uint32 q_argument_len;
  char p_data[1]; /*ioctl result argument of size argument_len starts here*/
} pd_comms_ipc_ioctl_ack_hdr_type;

typedef struct
{
  uint32 q_transaction_id;
  uint32 q_output_len;
  uint32 q_msg_len;
  char p_data[1]; /* first q_msg_len contains original message and
                     the next output_len bytes contain the hash output*/
} pd_comms_ipc_hash_resp_hdr_type;

typedef struct
{
  uint32 q_transaction_id; 
  boolean u_success;
  pd_comms_handle_type z_handle;
} pd_comms_ipc_sms_ack_report_hdr_type;

typedef pd_comms_return_type (pd_comms_app_get_handle_type)(
  const pd_comms_get_handle_params_type* const p_params,
  /*~ PARAM p_params POINTER */
  pd_comms_handle_type* p_handle
  /*~ PARAM OUT p_handle POINTER */  
  );
/*~ CALLBACK pd_comms_app_get_handle_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_release_handle_type)(
  const pd_comms_handle_type* p_handle
  /*~ PARAM p_handle POINTER */  
  );
/*~ CALLBACK pd_comms_app_release_handle_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_connect_type)( 
  const pd_comms_handle_type z_handle,                                                       
  const uint32 q_source_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_connect_params_type * const p_params
  /*~ PARAM p_params POINTER */
  );
/*~ CALLBACK pd_comms_app_connect_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_disconnect_type)(
  const pd_comms_handle_type z_handle,
  const uint32 q_source_thread_id,
  const uint32 q_base_msg_id,
  const uint32 q_transaction_id,
  const boolean abort);
/*~ CALLBACK pd_comms_app_disconnect_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_bind_type)( 
  const pd_comms_handle_type z_handle,                                                       
  const uint32 q_source_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_bind_params_type * const p_params
  /*~ PARAM p_params POINTER */
  );
/*~ CALLBACK pd_comms_app_connect_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_write_type)( 
  const pd_comms_handle_type z_handle,
  const uint32 q_source_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_write_params_type * const p_params
  /*~ PARAM p_params POINTER */
  );
/*~ CALLBACK pd_comms_app_write_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_read_type)( 
  const pd_comms_handle_type z_handle,
  const uint32 q_source_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_read_params_type * const p_params
  /*~ PARAM p_params POINTER */
  );
/*~ CALLBACK pd_comms_app_read_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_generate_hash_type)(
  const uint32 q_source_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_generate_hash_params_type* const p_hash_params
  /*~ PARAM p_hash_params POINTER */
  );
/*~ CALLBACK pd_comms_app_generate_hash_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_ioctl_type)(
  const pd_comms_handle_type z_handle,
  const uint32 q_source_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_ioctl_params_type *const p_params
  /*~ PARAM p_params POINTER */
  );
/*~ CALLBACK pd_comms_app_ioctl_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef pd_comms_return_type (pd_comms_app_ipc_send_type)(
  uint32       msg_id,
  uint16       len,
  uint8*       msg
  /*~ PARAM msg VARRAY LENGTH len */
);
/*~ CALLBACK pd_comms_app_ipc_send_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef void (pd_comms_app_srv_sys_change_handle_type) (
  sys_sys_mode_e_type srv_system
);
typedef void (pd_comms_app_apn_profile_change_handle_type) (
  uint32 apn_profile_type
);

typedef void (pd_comms_app_set_data_enable_type)(
  boolean  data_enable
 );
/*~ CALLBACK pd_comms_app_set_data_enable_type
    ONERROR  return void
*/

typedef pd_comms_return_type (pd_comms_app_inject_msg_type)( 
  const pd_comms_handle_type z_handle,                                                       
  const uint32 q_source_thread_id,
  const uint32 q_base_msg_id,
  const pd_comms_inject_msg_params_type * const p_params
  /*~ PARAM p_params POINTER */
  );
/*~ CALLBACK pd_comms_app_inject_msg_type
    ONERROR  return PDCOMM_RESULT_ERROR_RPC_FAILURE
*/

typedef struct
{
  pd_comms_app_get_handle_type *get_handle_func;  
  pd_comms_app_connect_type *connect_func; 
  pd_comms_app_disconnect_type *disconnect_func;
  pd_comms_app_bind_type *bind_func;
  pd_comms_app_write_type *write_func;
  pd_comms_app_ioctl_type *ioctl_func;
  pd_comms_app_read_type *read_func;
  pd_comms_app_release_handle_type *release_handle_func; 
  pd_comms_app_srv_sys_change_handle_type *srv_system_change_notification_handle_func;
  pd_comms_app_set_data_enable_type *set_data_enable_func;  
  pd_comms_app_apn_profile_change_handle_type *p_apn_profile_change_notification_handle_func;
  pd_comms_app_inject_msg_type *inject_msg_func;
} pd_comms_register_functions_type;


typedef struct
{
  pd_comms_link_type e_link_type;
  uint32 q_function_mask; /*flag values defined in pd_comms_function_mask*/
} pd_comms_register_service_type;


/****************** Function Declarations**************************************/


/*===========================================================================
FUNCTION pd_comms_app_get_handle

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
pd_comms_return_type pd_comms_app_get_handle(
  /*get_handle parameters*/  
  const pd_comms_get_handle_params_type* const p_params,
  /*~ PARAM   p_params POINTER */
  /*contains the handle allocated by pdcomm-task */
  pd_comms_handle_type* p_handle
  /*~ PARAM   OUT p_handle POINTER */
);
/*~ FUNCTION pd_comms_app_get_handle */

/*===========================================================================
FUNCTION pd_comms_app_connect

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
pd_comms_return_type pd_comms_app_connect( 
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
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_app_connect */


/*===========================================================================
FUNCTION pd_comms_app_disconnect

DESCRIPTION
  This function closes the connection associated with the given handle. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_disconnect(
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
  const boolean abort
);
/*~ FUNCTION pd_comms_app_disconnect */

/*===========================================================================
FUNCTION pd_comms_app_bind

DESCRIPTION
  This function binds to a socket for a given handle 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_bind(
  /*handle corresponding to the connection to be closed*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*Pointer to structure containing connect parameters*/
  const pd_comms_bind_params_type * const p_params
);
/*~ FUNCTION pd_comms_app_bind */


/*===========================================================================
FUNCTION pd_comms_app_write

DESCRIPTION
  This function writes data to the specified connection. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_write( 
  /*handle corresponding to the connection to be written to.*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*write parameters corresponding to the link type*/
  const pd_comms_write_params_type * const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_app_write */


/*===========================================================================
FUNCTION pd_comms_app_read

DESCRIPTION
  This function reads data from a specified connection. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_read( 
  /*handle corresponding to the connection to be read from*/  
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*read parameters corresponding to the link type*/
  const pd_comms_read_params_type * const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_app_read */


/*===========================================================================
FUNCTION pd_comms_app_generate_hash

DESCRIPTION
  This function generates a hash based upon a given data payload. 
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_generate_hash(
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
  const pd_comms_generate_hash_params_type* const p_hash_params
  /*~ PARAM   p_hash_params POINTER */
);
/*~ FUNCTION pd_comms_app_generate_hash */


/*===========================================================================
FUNCTION pd_comms_app_ioctl

DESCRIPTION
  This function provides get/set I/O parameter functionality. The exact I/O option
  depends upon the specific kind of transport/data service being used.
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_ioctl(
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
  const pd_comms_ioctl_params_type *const p_params
  /*~ PARAM   p_params POINTER */
);
/*~ FUNCTION pd_comms_app_ioctl */

/*===========================================================================
FUNCTION pd_comms_app_release_handle

DESCRIPTION
  This function releases a pdcomm handle previously allocated by calling
  pd_comms_app_get_handle(). If the connection is in CONNECTED state, this call
  will also abort the connection before releasing handle.
  
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_release_handle(
  /*contains the handle allocated by pdcomm-task */
  const pd_comms_handle_type* p_handle
  /*~ PARAM   p_handle POINTER */
);
/*~ FUNCTION pd_comms_app_release_handle */

/*===========================================================================
FUNCTION pd_comms_app_inject_msg

DESCRIPTION
  This function is called by PDCOMM client to inject message into PDCOMM's
  read buffer. It's currently used by TM Core's SUPL module to inject external SMS to
  WMS Task, this is mainly for SUPL INIT NI SMS received from HLOS
 
DEPENDENCIES
  None

RETURN VALUE
 pdcomm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_inject_msg(
  /*contains the handle allocated by pdcomm-task */
  const pd_comms_handle_type z_handle,
  /*thread id of the client*/
  const uint32 q_source_thread_id,
  /*the message ID to be used as the base messageID when PDCOMM
  responds back to client. This base_msg_id will be added to messages declared
  in pd_comms_client_ipc_message_type to get the final message_id when 
  PDCOMM APP responds back to the client*/
  const uint32 q_base_msg_id,
  /*Pointer to structure containing connect parameters*/
  const pd_comms_inject_msg_params_type * const p_params
);
/*~ FUNCTION pd_comms_app_inject_msg */

/*===========================================================================
FUNCTION pd_comms_app_srv_sys_change_notification

DESCRIPTION
  This function handles the serving system change notification
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_app_srv_sys_change_notification(sys_sys_mode_e_type srv_system);

/*===========================================================================
FUNCTION pd_comms_app_set_data_enable

DESCRIPTION
  This function enables/disables data
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_app_set_data_enable(boolean data_enable);


/*===========================================================================
FUNCTION pd_comms_app_apn_profile_change_notification

DESCRIPTION
  This function handles the apn profile change notification
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_app_apn_profile_change_notification(uint32 apn_profile_type);


/*===========================================================================
FUNCTION pd_comms_set_apn_profiles

DESCRIPTION
  This function shall be used to set the apn profiles. NV shall be updated
  to reflect the change.
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_set_apn_profiles(pdsm_client_id_type client_id, uint32 apn_profile_type, pdsm_pa_apn_profiles_type *apn_profile_p);

/*===========================================================================
FUNCTION pd_comms_get_apn_profiles

DESCRIPTION
  This function shall be used to get the apn profiles. 
  
DEPENDENCIES
  None

RETURN VALUE
 void

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_get_apn_profiles(uint32 apn_profile_type, pdsm_pa_apn_profiles_type *apn_profile_p);

/*===========================================================================
FUNCTION pd_comms_app_reg_ipc_cb

DESCRIPTION
  This function is called by PDCOMM APP to register a callback to be used by
  PDCOMMS to inform the app of PDCOMM events. 
  
DEPENDENCIES
  None

RETURN VALUE
 pd_comms_return_type 

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_app_reg_ipc_cb(
  const pd_comms_handle_type *p_handle,
  /*~ PARAM   p_handle POINTER */
  pd_comms_app_ipc_send_type *ipc_send_func
  /*~ PARAM   ipc_send_func POINTER */
);
/*~ FUNCTION pd_comms_app_reg_ipc_cb */

/*===========================================================================
FUNCTION pd_comms_register_service

DESCRIPTION
  This function is called by PDCOMM APP to register/de-register communication 
  services it offers for a particular transport type. If a functions mask is set
  and the corresponding function pointer is passed as NULL, the function will
  be de-registered from PDCOMM layer.
  
DEPENDENCIES
  None

RETURN VALUE
 boolean - Registration successful or not

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_register_service(
  const pd_comms_register_service_type *p_reg,
  /*~ PARAM   p_reg POINTER */
  pd_comms_app_get_handle_type *p_get_handle_func, 
  /*~ PARAM   p_get_handle_func POINTER */
  pd_comms_app_connect_type *p_connect_func, 
  /*~ PARAM   p_connect_func POINTER */
  pd_comms_app_disconnect_type *p_disconnect_func,
  /*~ PARAM   p_bind_func POINTER */
  pd_comms_app_bind_type *p_bind_func,
  /*~ PARAM   p_disconnect_func POINTER */
  pd_comms_app_write_type *p_write_func,
  /*~ PARAM   p_write_func POINTER */
  pd_comms_app_generate_hash_type *p_generate_hash_func,
  /*~ PARAM   p_generate_hash_func POINTER */
  pd_comms_app_ioctl_type *p_ioctl_func,
  /*~ PARAM   p_ioctl_func POINTER */
  pd_comms_app_read_type *p_read_func,
  /*~ PARAM   p_read_func POINTER */
  pd_comms_app_release_handle_type *p_release_handle_func,
  /*~ PARAM   p_release_handle_func POINTER */
  pd_comms_app_srv_sys_change_handle_type *p_srv_system_change_notification_handle_func,
 /*~ PARAM serving system change notification handle*/
  pd_comms_app_set_data_enable_type *p_set_data_enable_func,
  /*~ PARAM pdcomm set data enable handle*/  
  pd_comms_app_apn_profile_change_handle_type *p_apn_profile_change_notification_handle_func,
  /*~ PARAM apn profile change notification handle*/
  pd_comms_app_inject_msg_type *p_inject_msg_func
  /*~ PARAM   p_inject_msg_func POINTER */
);
/*~ FUNCTION pd_comms_register_service*/


#endif /* PD_COMMS_API_H */
