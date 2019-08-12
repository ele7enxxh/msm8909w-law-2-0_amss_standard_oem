/*======================================================================

   PDCOMM Application task implementation file (BIT APIs)

 GENERAL DESCRIPTION
  This file contains the implementation of PDCOMM Application task.
  This task handles requests from various clients like TM, calls 
  appropriate BIT APIs and sends back results to the client.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_tcp/src/pd_comms_tcp_task_bit.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 04/26/11   gk   resolved compiler errors related to changes for APN
 03/09/11   rh   Added TLS/SSL support for BIT API
 03/08/11   rh   Fixed compiler errors on ULP for new PDCOMM header
 11/11/10   rh   Fixed BIT compilation issue with new PDCOMM API header
 04/30/10   rh   Initial version
======================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_CGPS_PDCOMM

#ifdef FEATURE_GNSS_BIT_API
#include "msg.h"
#include "secapi.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "pd_comms_api.h"
#include "pd_comms_client_api.h"
#include "pd_comms_app_api.h"
#include "pd_comms_tcp_local_api.h"
#include "pd_comms_tcp_data.h"

#include "gpsone_bit_api.h"  
#include "gpsone_bit_local_api.h"  


/***************************** Variables *************************************/
extern pd_comms_tcp_task_handle_data_type z_handle_table[];

/**************************** Data Structures *******************************/

/*The BIT event callback functions will send an internal
 *message to PDCOMM_APP_TASK to notify PDCOMM_APP_TASK regarding BIT EVENT*/
typedef struct
{
  uint32                            handle_id;
  gpsone_bit_session_handle_type    session_handle; 
  uint32                            transaction_id;
  gpsone_bit_event_payload_type     event_payload;
} pd_comms_tcp_task_bit_event_cb_ipc_hdr_type;


/*****************************  Function Definitions ***************************/

// functions implemented in tcp_task_common.c
extern void pd_comms_tcp_task_update_state (pd_comms_handle_type z_handle, pd_comms_state_type z_new_state,
                                            boolean notify_client, uint32 q_result);

extern void pd_comms_tcp_task_reset_inactivity_count (uint32 q_handle_id);

extern void pd_comms_tcp_task_open_ssl (pd_comms_handle_type z_handle);

extern void pd_comms_tcp_task_ssl_forward_event(sint15 sockfd);

extern void pd_comms_tcp_task_secure_tcp_disconnect (pd_comms_handle_type z_handle, boolean b_abort, boolean u_notifyClient);

// forward local function declarations
static void pd_comms_tcp_task_handle_write_result(uint32 q_handle_id, uint32 bytes_written);
static void pd_comms_tcp_task_handle_read_result(uint32 q_handle_id, uint32 bytes_ready);
static void pd_comms_tcp_task_handle_ioctl_result(uint32 q_handle_id, uint32 transaction_id, gpsone_bit_event_payload_type *event);


/*===========================================================================
FUNCTION pd_comms_tcp_task_register_callbacks

DESCRIPTION
  Registers callabcks with PDCOMM layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_register_callbacks(void)
{
  pd_comms_register_service_type z_reg_params;
  boolean u_result;

  /*Register TCP transport functions*/
  z_reg_params.e_link_type = LINK_TCP;
  z_reg_params.q_function_mask = (uint32)PD_COMMS_FUNC_CONNECT | (uint32)PD_COMMS_FUNC_DISCONNECT |
                                 (uint32)PD_COMMS_FUNC_WRITE |(uint32)PD_COMMS_FUNC_IOCTL | 
                                 (uint32)PD_COMMS_FUNC_GET_HANDLE |(uint32)PD_COMMS_FUNC_READ |
                                 (uint32)PD_COMMS_FUNC_RELEASE_HANDLE | (uint32) PD_COMMS_FUNC_SRV_SYSTEM_NOTIFICATION;

  u_result = pd_comms_register_service(&z_reg_params,pd_comms_client_tcp_get_handle_cb,
                                       pd_comms_client_tcp_connect_cb,
                                       pd_comms_client_tcp_disconnect_cb, NULL, pd_comms_client_tcp_write_cb,
                                       NULL,pd_comms_client_tcp_ioctl_cb,
                                       pd_comms_client_tcp_read_cb, pd_comms_client_tcp_release_handle_cb,
                                       pd_comms_client_srv_system_change_notification_handle,
                                       NULL, NULL, NULL);
 
  if(u_result == FALSE)
  {
    PDCOMM_APP_MSG_ERROR("Failed registering TCP services with PDCOMM layer",0,0,0);
    return;
  }

 PDCOMM_APP_MSG_MED("Successfully registered TCP callbacks",0,0,0);

  /*Register UDP transport functions*/
  z_reg_params.e_link_type = LINK_UDP;
  z_reg_params.q_function_mask = (uint32)PD_COMMS_FUNC_CONNECT | (uint32)PD_COMMS_FUNC_DISCONNECT |
                                 (uint32)PD_COMMS_FUNC_WRITE | 
                                 (uint32)PD_COMMS_FUNC_GET_HANDLE |(uint32)PD_COMMS_FUNC_READ |
                                 (uint32)PD_COMMS_FUNC_RELEASE_HANDLE | (uint32)PD_COMMS_FUNC_BIND;

  u_result = pd_comms_register_service(&z_reg_params,
                                       pd_comms_client_tcp_get_handle_cb,
                                       pd_comms_client_tcp_connect_cb, pd_comms_client_tcp_disconnect_cb,
                                       pd_comms_client_tcp_bind_cb,
                                       pd_comms_client_tcp_write_cb,
                                       NULL,NULL,
                                       pd_comms_client_tcp_read_cb, pd_comms_client_tcp_release_handle_cb,
                                       NULL, NULL, NULL, NULL);
 
  if(u_result == FALSE)
  {
    PDCOMM_APP_MSG_ERROR("Failed registering UDP services with PDCOMM layer",0,0,0);
    return;
  }
  
  PDCOMM_APP_MSG_MED("Successfully registered UDP callbacks",0,0,0);

}/*pd_comms_tcp_task_register_callbacks*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_report_error

DESCRIPTION
  This function handles sending error to PDCOMM client
  (for read/write/ioctl/abort etc)

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_report_error(
  uint32 q_handle_id, 
  uint32 transaction_id,
  pd_comms_state_type e_new_state,
  int32  q_result)
{
  PDCOMM_APP_MSG_ERROR("ERROR_RPT being sent (handle_id=%d error=%d transid=0x%x)", 
                       q_handle_id, q_result, transaction_id);

  if (q_handle_id >= PDCOMM_MAX_HANDLES || z_handle_table[q_handle_id].in_use != TRUE)
  {
    PDCOMM_APP_MSG_ERROR("Invalid connection handle(%d)!", q_handle_id, 0, 0);
  }
  else
  {
    pd_comms_notification_params_type  z_params;   
    pd_comms_return_type               e_result;

    z_params.e_new_state = e_new_state;
    z_params.q_result = q_result;
    e_result = pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                           z_handle_table[q_handle_id].q_source_thread_id,
                           z_handle_table[q_handle_id].q_base_msg_id,
                           transaction_id,
                           ERROR_RPT,&z_params);
    if (e_result != PDCOMM_RESULT_OK)
    {
      PDCOMM_APP_MSG_ERROR("IPC error %d for handle_id [%d]", (uint32)e_result,q_handle_id,0);
    }    
  }
}


/*===========================================================================
FUNCTION pd_comms_tcp_task_abort_connection

DESCRIPTION
  This function will abort a particular connection. The function doesn't wait 
  for any callbacks and immediately frees up the handles after calling all 
  the close routines. This is used to free up connections immediately for 
  cases like E911 calls and error scenarios

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_abort_connection(
  /*handle_id corresponding to the connection for which connection is to be aborted*/
  uint32 q_handle_id )
{
  gpsone_bit_status_e_type                 status;
  gpsone_bit_disconnect_params_type        disconnect_param;

  disconnect_param.force_disconnect = TRUE;

  // use z_handle.q_handle_id as the transaction_id so we can look it up when notify() send back RESULT
  status = gpsone_bit_disconnect(z_handle_table[q_handle_id].session_handle, q_handle_id, 
                                 &disconnect_param);  // call wrapper function to disconnect
    /* clear app_id */
  z_handle_table[q_handle_id].session_handle = GPSONE_BIT_INVALID_SESSION_HANDLE;

  PDCOMM_APP_MSG_MED("Abort connection(handle=%d disconnect_status=%d)", q_handle_id, status, 0);

} /* pd_comms_tcp_task_abort_connection*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_tcp_connect

DESCRIPTION
 This function handles a TCP/IP connect request by calling BIT API

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_tcp_task_operation_status_type pd_comms_tcp_task_tcp_connect(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  pd_comms_tcp_task_operation_status_type   e_retval = FAILED;
  pd_comms_connect_params_type              *p_param;
  gpsone_bit_status_e_type                  status = GPSONE_BIT_STATUS_SUCCESS;
  gpsone_bit_connect_params_type            connect_param;

  PDCOMM_APP_MSG_MED("Calling BIT connect (handle = %d, link_status = %d)",
           z_handle.q_handle_id, z_handle_table[z_handle.q_handle_id].e_link_status , 0 );   

  p_param = &z_handle_table[z_handle.q_handle_id].z_connect_params;  // connect param passed into PDCOMM

  memset((void *)&connect_param, 0, sizeof(connect_param));

  /*If we reach here that means netlib and PPP are successfully opened so start socket connection now*/
  pd_comms_tcp_task_update_state(z_handle, PDCOMM_STATE_CONNECTING, TRUE,(uint32)PDCOMM_RESULT_WAIT);    

  // TODO: need to touch PDCOMM layer to actually pass in the protocol
  if (p_param->z_link_params.z_tcp_params.profile_type == PDCOMM_APN_PROFILE_TYPE_LBS)
  {
    // Vx, IS801, SUPL will fill in PDCOMM_APN_PROFILE_TYPE_LBS
    if (p_param->e_use_security == SECURITY_NONE)
      connect_param.protocol_type = GPSONE_BIT_PROTOCOL_TYPE_IS801; // FIXME: non-secure can be other protocol?
    else connect_param.protocol_type = GPSONE_BIT_PROTOCOL_TYPE_SUPL;  // will map to SUPL APN by GPS daemon
  }
  else
  {
    // XTRA/XTRA-T will fill in PDCOMM_APN_PROFILE_TYPE_XTRA
    connect_param.protocol_type = GPSONE_BIT_PROTOCOL_TYPE_ANY;  // will map to Default Internet APN by GPS daemon
  }

  connect_param.iface_param.iface_name = GPSONE_BIT_IFACE_ANY;
  connect_param.iface_param.iface_family = GPSONE_BIT_IP_V4;
  //connect_param.iface_param.apn_name[0] = '\0';  // not used for now

  connect_param.link_type = (gpsone_bit_link_e_type)p_param->e_link_type;  // TCP, UDP or HTTP, etc
  connect_param.ip_port = p_param->z_link_params.z_tcp_params.w_ip_port; // port

  switch(p_param->z_link_params.z_tcp_params.e_type)
  {
  case PD_COMMS_IP_ADDR:
    connect_param.adr_type = GPSONE_BIT_IP_ADDR; 
    break;

  case PD_COMMS_HOST_NAME:
    connect_param.adr_type = GPSONE_BIT_HOST_NAME;
    /* length must match!! 256 bytes */
    (void) GNSS_STRLCPY(connect_param.host_name, p_param->z_link_params.z_tcp_params.p_host_name , sizeof(connect_param.host_name));
    break;

  case PD_COMMS_HOST_NAME_AND_IP_ADDR:
    connect_param.adr_type = GPSONE_BIT_HOST_NAME_AND_IP_ADDR;
    (void) GNSS_STRLCPY(connect_param.host_name, p_param->z_link_params.z_tcp_params.p_host_name , sizeof(connect_param.host_name));
    break;

  default:
    PDCOMM_APP_MSG_ERROR("Unknown tcp connect parameter 0x%x (handle = %d)",
             (uint32)p_param->z_link_params.z_tcp_params.e_type, z_handle.q_handle_id,  0 );   
    status = GPSONE_BIT_STATUS_INVALID_PARAM;
    break;
  }

  // If hostname only, no IP address is required
  if( status == GPSONE_BIT_STATUS_SUCCESS && connect_param.adr_type != GPSONE_BIT_HOST_NAME)
  {
    // pd_comms_api.h with IPv6 support
    switch(p_param->z_link_params.z_tcp_params.ip_addr_type)
    {
    case PD_COMMS_IP_ADDR_IPV6:
      connect_param.ip_addr.type = GPSONE_BIT_IP_V6;
      connect_param.iface_param.iface_family = GPSONE_BIT_IP_V6;
      memscpy((void *)&(connect_param.ip_addr.addr.v6_addr), sizeof(connect_param.ip_addr.addr.v6_addr),
	  	    (void *)&(p_param->z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v6), 
            sizeof(p_param->z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v6));  // 16 bytes for IPv6 address
      break;

    case PD_COMMS_IP_ADDR_IPV4:
      connect_param.ip_addr.type = GPSONE_BIT_IP_V4;
      connect_param.iface_param.iface_family = GPSONE_BIT_IP_V4;
      connect_param.ip_addr.addr.v4_addr = p_param->z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4;
      break;

    default:
      PDCOMM_APP_MSG_ERROR("Unknown IP address format 0x%x(Not V4 or V6!) (handle = %d)",
               (uint32)p_param->z_link_params.z_tcp_params.ip_addr_type, z_handle.q_handle_id,  0 );   
      status = GPSONE_BIT_STATUS_INVALID_PARAM;
      break;
    }
  }

  if( status == GPSONE_BIT_STATUS_SUCCESS )
  {
    // use q_handle_id as transaction_id so notify() will know where to send back the CONNECT result
    status = gpsone_bit_connect(z_handle.q_handle_id,  &connect_param);  // call wrapper function 
  }

  if( status == GPSONE_BIT_STATUS_SUCCESS )
  {
    /*Goto CONNECTED state and notify client only if this is a NON-SSL
      link since for SSL we still need to open a SSL session on this connection */
    if(p_param->e_use_security == SECURITY_NONE)
    {
      pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                     PDCOMM_STATE_CONNECTED, TRUE,(uint32)PDCOMM_RESULT_OK);
    }

    e_retval = SUCCEEDED;

  }
  else if( status == GPSONE_BIT_STATUS_WAIT )
  {
    /* Notify client that the socket connection has started */
    if (z_handle_table[z_handle.q_handle_id].e_link_status != PDCOMM_STATE_CONNECTING)
    {
      pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                   PDCOMM_STATE_CONNECTING, TRUE,(uint32)PDCOMM_RESULT_WAIT);
    }
    e_retval = PENDING;
  }
  else  // failure
  {
    pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                     PDCOMM_STATE_DISCONNECTED, TRUE,(uint32) PDCOMM_RESULT_OK);
    e_retval = FAILED;
  }

  return e_retval;  

} /* pd_comms_tcp_task_tcp_connect */


/*===========================================================================
FUNCTION pd_comms_tcp_task_tcp_disconnect

DESCRIPTION
  This function handles a TCP link disconnect request. If abort option is set
  to true, the connection will be immediately closed and handle released. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_tcp_disconnect(
  /*handle corresponding to the connection for which connection is to be disconnected*/
  pd_comms_handle_type z_handle,
  /*to abort or gracefully close a connection*/
  boolean b_abort,
  /*in case of abort, whether to notify the client or not*/
  boolean u_notifyClient)
{
  pd_comms_disconnect_result_type          e_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;
  gpsone_bit_status_e_type                 status;
  gpsone_bit_disconnect_params_type        disconnect_param;

  disconnect_param.force_disconnect = b_abort;

  if (b_abort)
  {
    PDCOMM_APP_MSG_MED("TCP connection abort received for handle_id[%d]",z_handle.q_handle_id,0,0);  
    pd_comms_tcp_task_abort_connection( z_handle.q_handle_id ); 
    pd_comms_tcp_task_update_state(z_handle, PDCOMM_STATE_DISCONNECTED, u_notifyClient,(uint32) PDCOMM_RESULT_OK);

    return;
  }

  switch(z_handle_table[z_handle.q_handle_id].e_link_status)
  {
    case PDCOMM_STATE_DISCONNECTED:   // already disconnected, do nothing
    {
      break;
    }

    case PDCOMM_STATE_OPENING:
    {
      // state not used by BIT API  (opening PPP)
      break;
    }

    case PDCOMM_STATE_DISCONNECTING:
    {
      PDCOMM_APP_MSG_MED("Disconnect request recevied while in DISCONNECTING state for handle_id [%d]",
                         z_handle.q_handle_id,0,0);
      /*This is to mark the condition where client calls multiple disconnects
       *for the same handle or client sends a disconnect when disconnect process
       *is already going on becuase of some error*/
      z_handle_table[z_handle.q_handle_id].client_disconnect_recvd = TRUE;
      break;
    }

    case PDCOMM_STATE_CONNECTED:
    case PDCOMM_STATE_CONNECTING:
    {
      // use z_handle.q_handle_id as the transaction_id so we can look it up when notify() send back RESULT
      status = gpsone_bit_disconnect(z_handle_table[z_handle.q_handle_id].session_handle, z_handle.q_handle_id, 
                                     &disconnect_param);  // call wrapper function to disconnect

      if (status == GPSONE_BIT_STATUS_SUCCESS)
      {
        pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32) PDCOMM_RESULT_OK);
      }
      else if (status == GPSONE_BIT_STATUS_WAIT)
      {
        pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                     TRUE,(uint32) PDCOMM_RESULT_WAIT);
      }
      else // failure? mark it as DISCONNECTED anyway 
      { 
        pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED, 
                                     TRUE,(uint32) e_disc_error);
      }

      break;
    }

  } /* switch(link_status) ends*/

} /*pd_comms_tcp_task_tcp_disconnect() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_disconnect_after_error

DESCRIPTION
  This function handles a TCP link disconnect request. It can handle both
  secured and unsecured link by calling correct function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_tcp_disconnect_after_error(
  /*handle corresponding to the connection for which connection is to be disconnected*/
  pd_comms_handle_type z_handle,
  /*to abort or gracefully close a connection*/
  boolean b_abort,
  /*in case of abort, whether to notify the client or not*/
  boolean u_notifyClient)
{
   if (z_handle_table[z_handle.q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
   {
     /*Abort the connection but do not notify client*/     
     pd_comms_tcp_task_tcp_disconnect(z_handle, b_abort, u_notifyClient); 
   }
   else
   {
     /*Abort the connection but do not notify client*/   
     pd_comms_tcp_task_secure_tcp_disconnect(z_handle, b_abort, u_notifyClient); 
   }
}

/*===========================================================================
FUNCTION pd_comms_tcp_task_udp_bind

DESCRIPTION
  This function is called when a UDP socket needs to be opened and bind

DEPENDENCIES
  None

RETURN VALUE
  boolean - status of bind

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_tcp_task_udp_bind(
  /*handle_id corresponding to the connection for which bind is needed*/
  pd_comms_handle_type z_handle)
{
  
  pd_comms_tcp_task_operation_status_type   e_retval = FAILED;
  pd_comms_connect_params_type              *p_param;
  
  
  PDCOMM_APP_MSG_MED("UDP Bind. Calling BIT connect (handle = %d, link_status = %d)",
           z_handle.q_handle_id, z_handle_table[z_handle.q_handle_id].e_link_status , 0 );   

  p_param = &z_handle_table[z_handle.q_handle_id].z_connect_params;  // connect param passed into PDCOMM
  p_param->e_link_type = LINK_UDP;

  /* call the tcp_connect with a fake IP address and the UDP port number.
     This will inform the GPS deamon that a listening UDP socket is to be
     created */
  p_param->z_link_params.z_tcp_params.e_type = PD_COMMS_IP_ADDR;
  p_param->z_link_params.z_tcp_params.w_ip_port = z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port;
  p_param->z_link_params.z_tcp_params.ip_addr_type = PD_COMMS_IP_ADDR_IPV4;
  z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4  = PDCOMM_UDP_SERVER_DUMMY_IP;
  p_param->z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4 = PDCOMM_UDP_SERVER_DUMMY_IP;
  z_handle_table[z_handle.q_handle_id].z_connect_params.e_use_security = SECURITY_NONE;
  
  PDCOMM_APP_MSG_MED("UDP Bind. Calling TCP connect (handle = %d, link_status = %d, port_id = %d)",
           z_handle.q_handle_id, z_handle_table[z_handle.q_handle_id].e_link_status , 
    p_param->z_link_params.z_tcp_params.w_ip_port  );   
  
  e_retval = pd_comms_tcp_task_tcp_connect(z_handle);
  /* not sure if need to handle any other error cases here */
  
  PDCOMM_APP_MSG_MED("UDP Bind. TCP connect retval (handle = %d, retval = %d)", 
      z_handle.q_handle_id, e_retval, 0);
  
  return e_retval;
}

/*===========================================================================
FUNCTION pd_comms_tcp_task_tcp_read

DESCRIPTION
  This function is called when a DATA_READY BIT event occurs which 
  indicates presence of incoming data to be read. It tries to call BIT recv()
  untill all data present in BIT daemon buffer is completely read. For
  each PDCOMM_READ_DATA_BUF_SIZE bytes of data read, this data will be sent
  to the client in a IPC message. 

DEPENDENCIES
  None

RETURN VALUE
  boolean - status of data read

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_tcp_task_tcp_read(
  /*handle_id corresponding to the connection for which data is to be read*/
  uint32 q_handle_id,
  /*wait for all requested data before sending it to client or not*/
  boolean u_wait_for_all) 
{
  uint8 *p_read_data;
  uint32 length, x_bytes_read = 0, x_bytes_leftover = 0;
  boolean result = TRUE;                        
  uint16 *p_num_bytes_to_read;
  uint16 *p_num_bytes_already_read;
  gpsone_bit_status_e_type          status;

  p_read_data = &z_handle_table[q_handle_id].p_read_buff[0];
  p_num_bytes_to_read = &z_handle_table[q_handle_id].w_num_bytes_to_read;
  p_num_bytes_already_read = &z_handle_table[q_handle_id].w_num_bytes_already_read;

  do
  {
    if ( z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_CONNECTED)
    {
      // link disconnected
      result = FALSE;
      break;
    }

    length = (uint32)(*p_num_bytes_to_read - *p_num_bytes_already_read);
    /* Retrieve up to max_bytes from the socket */
    status = gpsone_bit_recv(z_handle_table[q_handle_id].session_handle, &p_read_data[*p_num_bytes_already_read], 
                             length, &x_bytes_read, &x_bytes_leftover);

    if (status == GPSONE_BIT_STATUS_NO_MORE_DATA)
    {
      // no more data available, not fatal error!
      x_bytes_read = x_bytes_leftover = 0;
    }
    else if ( status != GPSONE_BIT_STATUS_SUCCESS ) // failure case, no GPSONE_BIT_STATUS_WAIT for gpsone_bit_recv()!
    {
      // pd_comms_tcp_task_handle_read_result(z_handle.q_handle_id, 0); // 0 means error
      // initiate disconnect
      result = FALSE; 
      break;
    }

    if (x_bytes_read > length)
    {
      PDCOMM_APP_MSG_ERROR("Recevied %d bytes for handle_id [%d] (requested %d bytes)",x_bytes_read,q_handle_id,length);  
      // error condition, got more than we asked for !
      result = FALSE;
      break;
    }

    if (x_bytes_read == 0)  // no data but status is SUCCESS, not fatal error (maybe multiple reads)
    {
      if (x_bytes_leftover > 0) // bytes available, but got 0 byte ?!
      {
        PDCOMM_APP_MSG_ERROR("Recevied 0 bytes for handle_id [%d] while %d bytes available", q_handle_id,x_bytes_leftover,0);  
        result = FALSE;
      }
      // if x_bytes_leftover is 0, then no more data available, need to wait for DATA_READY event
      break;
    }

    // reached here, we got some bytes
    *p_num_bytes_already_read += (uint16)x_bytes_read;
    PDCOMM_APP_MSG_MED("Recevied %d bytes for handle_id [%d]",x_bytes_read,q_handle_id,0);  

    if ( (*p_num_bytes_already_read == *p_num_bytes_to_read) || (FALSE == u_wait_for_all) )
    {
      pd_comms_read_data_params_type z_params; 
      z_params.w_data_len = *p_num_bytes_already_read;
      z_params.p_data = p_read_data;

      PDCOMM_APP_MSG_MED("Sending %d bytes for handle_id [%d] to client",
                           *p_num_bytes_already_read,q_handle_id,0);   
      (void)pd_comms_senddata_to_client(z_handle_table[q_handle_id].z_handle, 
                                    z_handle_table[q_handle_id].q_source_thread_id,
                                    z_handle_table[q_handle_id].q_base_msg_id,
                                    z_handle_table[q_handle_id].q_read_transaction_id,
                                    &z_params);

      *p_num_bytes_already_read = *p_num_bytes_to_read = 0;
      z_handle_table[q_handle_id].requested_read = FALSE;  // no more pending READ, can send more READ_DATA
      result = TRUE;
      break;
    }

  } while ( (*p_num_bytes_to_read > *p_num_bytes_already_read) && (x_bytes_leftover > 0) );
  // continue only if a) u_wait_for_all = TRUE  b) x_bytes_leftover > 0 c) still bytes remaining

  // read done, handle failure and success cases
  if (result == FALSE)
  {
    *p_num_bytes_already_read = *p_num_bytes_to_read = 0;

    /* An error has occurred. Set link to disconnecting state */
    pd_comms_tcp_task_tcp_disconnect_after_error(z_handle_table[q_handle_id].z_handle, FALSE, TRUE); // abort = FALSE
  }
  else
  {
    if (x_bytes_leftover > 0)
    {
      pd_comms_tcp_task_handle_read_result(q_handle_id, x_bytes_leftover);
      // got maximum requested bytes, send READ_DATA event to PDCOMM client
    }
  }

  return( result );  

} /*pd_comms_tcp_task_tcp_read() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_read_result

DESCRIPTION
  This function handles operations after read

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_read_result(
  uint32 q_handle_id, 
  uint32 bytes_ready)
{
  if (bytes_ready > 0)
  {
    if ( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED )
    {
      if (z_handle_table[q_handle_id].w_num_bytes_already_read < z_handle_table[q_handle_id].w_num_bytes_to_read)
      {
        /* If a previous buffered read is pending, continue with that */   
        (void)pd_comms_tcp_task_tcp_read(q_handle_id, TRUE);  // u_wait_for_all=TRUE
      }
      else if (z_handle_table[q_handle_id].requested_read == FALSE)  
      {
        pd_comms_notification_params_type z_params;
        pd_comms_return_type e_result;

        /*Notify client about the read data availability*/  
        z_params.e_new_state = z_handle_table[q_handle_id].e_link_status;
        e_result = pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                   z_handle_table[q_handle_id].q_source_thread_id,
                   z_handle_table[q_handle_id].q_base_msg_id,
                   z_handle_table[q_handle_id].z_connect_params.q_transaction_id,
                   READ_DATA,&z_params);

        if (e_result != PDCOMM_RESULT_OK)
        {
          PDCOMM_APP_MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]", (uint32)e_result,q_handle_id,0);
        }    
        else 
        {
          // successfully sent READ_DATA event to PDCOMM client to request read
          z_handle_table[q_handle_id].requested_read = TRUE;   
        }
      }

    }
    else  // state is not CONNECTED
    {
      PDCOMM_APP_MSG_ERROR("PDCOMM APP: Data Ready while link is DISCONNECTED for handle_id [%d]", q_handle_id,0,0);
    }
  }
} /* pd_comms_tcp_task_handle_read_result */


/*===========================================================================
FUNCTION pd_comms_tcp_task_tcp_write

DESCRIPTION
  This function handles a write request for TCP connection. It also notifies 
  the client about the result of the write operation

DEPENDENCIES
  None

RETURN VALUE
  boolean - write operation successfull or not 

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_tcp_task_tcp_write(
  /*handle corresponding to the connection for which data is to be written*/
  pd_comms_handle_type z_handle)
{
  uint16 *w_num_bytes_written = &z_handle_table[z_handle.q_handle_id].w_num_bytes_written;
  gpsone_bit_status_e_type status;
  uint32 length;
  boolean result = TRUE;                        

  length = z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write - *w_num_bytes_written;

  // (for now, write is sequential! ie. no 2nd write until 1st write's result comes back)
  status = gpsone_bit_send(z_handle_table[z_handle.q_handle_id].session_handle, z_handle.q_handle_id,
                           &z_handle_table[z_handle.q_handle_id].p_write_buff[*w_num_bytes_written], length);

  if( status == GPSONE_BIT_STATUS_SUCCESS )
  {
    // all length bytes have been written successfully, no notify() event needed
    pd_comms_tcp_task_handle_write_result(z_handle.q_handle_id, length);
  }
  else if (status == GPSONE_BIT_STATUS_WAIT)  
  {
    // write pending, wait for notify() event
  }
  else  // failure case
  {
    pd_comms_tcp_task_handle_write_result(z_handle.q_handle_id, 0); // 0 means error
    result = FALSE;
  }

  return( result );  
} /*pd_comms_tcp_task_tcp_write() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_write_result

DESCRIPTION
  This function handles WRITE_RESULT event sent back from BIT daemon

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_write_result(
  uint32 q_handle_id, 
  uint32 bytes_written)
{
  boolean write_failure = FALSE;
  uint16 *w_num_bytes_written = &z_handle_table[q_handle_id].w_num_bytes_written;

  if (bytes_written == 0) // failure case
  {
    PDCOMM_APP_MSG_ERROR("Failed to write! bytes_written=%d for handle_id [%d]", bytes_written,q_handle_id,0);
    write_failure = TRUE;
  }
  else // some bytes written
  {
    *w_num_bytes_written += (uint16)bytes_written;

    if(*w_num_bytes_written < z_handle_table[q_handle_id].w_num_bytes_to_write) // not done yet
    {
      (void)pd_comms_tcp_task_tcp_write(z_handle_table[q_handle_id].z_handle);
      // it can create a recursive call diagram if BIT daemon return SUCCESS immediately
    }
    else if (*w_num_bytes_written == z_handle_table[q_handle_id].w_num_bytes_to_write)
    {
      // entire write transaction is done, notify client
      pd_comms_write_ack_params_type z_params;
      z_params.q_bytes_written = z_handle_table[q_handle_id].w_num_bytes_to_write;
      z_params.q_transaction_id = z_handle_table[q_handle_id].q_write_transaction_id;

      /* Acknowledge the write */
      (void)pd_comms_send_write_ack_to_client(z_handle_table[q_handle_id].z_handle,
                                        z_handle_table[q_handle_id].q_source_thread_id,
                                        z_handle_table[q_handle_id].q_base_msg_id,
                                        &z_params);
      *w_num_bytes_written = 0;
      z_handle_table[q_handle_id].w_num_bytes_to_write = 0;

    }
    else
    {
      write_failure = TRUE; // total bytes written more than w_num_bytes_to_write ?!
    }
  }

  if (write_failure) 
  {
    pd_comms_write_result_type        e_write_error = PDCOMM_WRITE_ERROR_UNKNOWN;

    // reset the write state
    *w_num_bytes_written = 0;
    z_handle_table[q_handle_id].w_num_bytes_to_write = 0;

    /*z_params.q_transaction_id = z_handle_table[z_handle.q_handle_id].q_write_transaction_id;*/
    pd_comms_tcp_task_report_error(q_handle_id, z_handle_table[q_handle_id].q_write_transaction_id,
      z_handle_table[q_handle_id].e_link_status, (uint32) e_write_error);

    pd_comms_tcp_task_tcp_disconnect_after_error(z_handle_table[q_handle_id].z_handle, FALSE, TRUE); // abort = FALSE
    // tcp_task_disconnect_handle() will reset the read/write state
  }
} /* pd_comms_tcp_task_handle_read_result */


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_ioctl_force_dormancy

DESCRIPTION
  This function handles IOCTL command to force dormancy in a TCP connection

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_ioctl_force_dormancy(
  /*ioctl command header*/
  pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)
{
  pd_comms_handle_type z_handle;
  
  if(p_hdr_ptr == NULL)
  {
    return;
  }

  z_handle = p_hdr_ptr->z_handle;
  
  if (z_handle.q_handle_id < PDCOMM_MAX_HANDLES)
  {
    gpsone_bit_status_e_type        status;
    gpsone_bit_ioctl_params_type    ioctl_param;

    ioctl_param.arg = ioctl_param.reserved = 0; // dummy param

    // use q_ioctl_transaction_id as transaction_id so we can match up 
    status = gpsone_bit_ioctl(z_handle_table[z_handle.q_handle_id].session_handle,
                              GPSONE_BIT_MAKE_TRANSID(z_handle.q_handle_id, GPSONE_BIT_IOCTL_FORCE_DORMANCY),
                              GPSONE_BIT_IOCTL_FORCE_DORMANCY, &ioctl_param);

    if ( status == GPSONE_BIT_STATUS_SUCCESS )
    {
      pd_comms_ioctl_ack_params_type z_ioctl_ack;

      PDCOMM_APP_MSG_HIGH("Force dormancy successfull for handle_id[%d]",z_handle.q_handle_id,0,0);
      /* Force dormancy succeeded, inform PDSM, no more notify(IOCTL_RESULT) */

      z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_FORCE_DORMANCY;
      z_ioctl_ack.q_transaction_id = p_hdr_ptr->q_transaction_id;
      z_ioctl_ack.q_argument_len = 0;
      (void)pd_comms_send_ioctl_ack_to_client(z_handle,
                                              z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                              z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                              &z_ioctl_ack);  
    }
    else if ( status == GPSONE_BIT_STATUS_WAIT )
    {
      PDCOMM_APP_MSG_HIGH("Force dormancy for handle_id[%d] pending", z_handle.q_handle_id, 0, 0);
      // need to wait for notify(IOCTL_RESULT)
    }
    else    
    {
      PDCOMM_APP_MSG_HIGH(" FORCE_DORMANCY ioctl failed for handle_id [%d]",
                            z_handle.q_handle_id,0,0);
      pd_comms_tcp_task_report_error(z_handle.q_handle_id, p_hdr_ptr->q_transaction_id,
        z_handle_table[z_handle.q_handle_id].e_link_status, (uint32)PDCOMM_IOCTL_ERROR_FORCE_DORMANCY_FAILED);

    }
  }/*if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    pd_comms_notification_params_type z_params;

    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;

    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in FORCE_DORMANCY ioctl ",
                          z_handle.q_handle_id,0,0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->q_transaction_id,
                           ERROR_RPT,&z_params);

  }
} /*lint !e818 */ /*pd_comms_tcp_task_handle_ioctl_force_dormancy*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_ioctl_unforce_dormancy

DESCRIPTION
  This function handles IOCTL command to unforce dormancy in a TCP connection

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_ioctl_unforce_dormancy(
  /*ioctl command header*/
  pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)
{
  pd_comms_handle_type z_handle;

  if(p_hdr_ptr == NULL)
  {
    return;
  }

  z_handle = p_hdr_ptr->z_handle;
  
  if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)
  {
    gpsone_bit_status_e_type        status;
    gpsone_bit_ioctl_params_type    ioctl_param;

    ioctl_param.arg = ioctl_param.reserved = 0; // dummy param

    // use q_ioctl_transaction_id as transaction_id so we can match up 
    status = gpsone_bit_ioctl(z_handle_table[z_handle.q_handle_id].session_handle,
                              GPSONE_BIT_MAKE_TRANSID(z_handle.q_handle_id, GPSONE_BIT_IOCTL_UNFORCE_DORMANCY),
                              GPSONE_BIT_IOCTL_UNFORCE_DORMANCY, &ioctl_param);

    if ( status == GPSONE_BIT_STATUS_SUCCESS )
    {
      pd_comms_ioctl_ack_params_type z_ioctl_ack;

      PDCOMM_APP_MSG_HIGH("unforce dormancy successfull for handle_id[%d]",z_handle.q_handle_id,0,0);
      /* Unforce dormancy succeeded, inform PDSM, no more notify(IOCTL_RESULT) */

      z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY;
      z_ioctl_ack.q_transaction_id = p_hdr_ptr->q_transaction_id;
      z_ioctl_ack.q_argument_len = 0;
      (void)pd_comms_send_ioctl_ack_to_client(z_handle,
                                              z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                              z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                              &z_ioctl_ack);  
    }
    else if ( status == GPSONE_BIT_STATUS_WAIT )
    {
      PDCOMM_APP_MSG_HIGH("Unforce dormancy for handle_id[%d] pending", z_handle.q_handle_id, 0, 0);
      return;  // need to wait for notify(IOCTL_RESULT)
    }
    else
    {
      /* Unforce dormancy failed, inform PDSM */
      PDCOMM_APP_MSG_HIGH(" UNFORCE_DORMANCY ioctl failed for handle_id [%d]",
                            z_handle.q_handle_id,0,0);
      pd_comms_tcp_task_report_error(z_handle.q_handle_id, p_hdr_ptr->q_transaction_id,
        z_handle_table[z_handle.q_handle_id].e_link_status, (uint32)PDCOMM_IOCTL_ERROR_UNFORCE_DORMANCY_FAILED);

    }
  }/*if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    pd_comms_notification_params_type z_params;

    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;
    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in UNFORCE_DORMANCY ioctl ",
                          z_handle.q_handle_id,0,0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->q_transaction_id,
                           ERROR_RPT,&z_params);

  }
} /*lint !e818 */ /*pd_comms_tcp_task_handle_ioctl_unforce_dormancy*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_ioctl_get_ip_addr

DESCRIPTION
  This function handles IOCTL command to get the current IP address

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_ioctl_get_ip_addr(
  /*ioctl command header*/
  const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)  
{
  pd_comms_handle_type z_handle;
    
  if(p_hdr_ptr == NULL)
  {
    return;
  }
  
  z_handle = p_hdr_ptr->z_handle;  

  if (z_handle.q_handle_id < PDCOMM_MAX_HANDLES)
  {
    gpsone_bit_status_e_type        status;
    gpsone_bit_ioctl_params_type    ioctl_param;

    ioctl_param.arg = ioctl_param.reserved = 0; // dummy param

    // use q_ioctl_transaction_id as transaction_id so we can match up 
    status = gpsone_bit_ioctl(z_handle_table[z_handle.q_handle_id].session_handle,
                              GPSONE_BIT_MAKE_TRANSID(z_handle.q_handle_id, GPSONE_BIT_IOCTL_GET_LOCAL_IP_ADDR),
                              GPSONE_BIT_IOCTL_GET_LOCAL_IP_ADDR, &ioctl_param);

    if ( status == GPSONE_BIT_STATUS_SUCCESS )
    {
      PDCOMM_APP_MSG_ERROR("Getting local IP address for handle_id[%d], return SUCCESS instead of WAIT!.", z_handle.q_handle_id, 0, 0);
      return;  // AGPS daemon should not return SUCCESS!!
    }
    else if ( status == GPSONE_BIT_STATUS_WAIT )
    {
      PDCOMM_APP_MSG_HIGH("Getting local IP address for handle_id[%d].", z_handle.q_handle_id, 0, 0);
      return;  // need to wait for notify(IOCTL_RESULT)
    }
    else
    {
      PDCOMM_APP_MSG_HIGH(" Ioctl call failed in GET_IP_ADDR ioctl for handle[%d]",
                            z_handle.q_handle_id,0,0);

      pd_comms_tcp_task_report_error(z_handle.q_handle_id, p_hdr_ptr->q_transaction_id,
        z_handle_table[z_handle.q_handle_id].e_link_status, (uint32)PDCOMM_IOCTL_ERROR_GET_IP_ADDR_FAILED);

    }
  }/*if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    pd_comms_notification_params_type z_params;   

    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;
    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in GET_IP_ADDR ioctl ",
                          z_handle.q_handle_id,0,0);

    /*Send the appropriate error to client*/ 
    (void)pd_comms_notify_client(z_handle,
                         p_hdr_ptr->q_source_thread_id,
                         p_hdr_ptr->q_base_msg_id,
                         p_hdr_ptr->q_transaction_id,
                         ERROR_RPT,&z_params);
  }
} /*pd_comms_tcp_task_handle_ioctl_get_ip_addr*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_ioctl_result

DESCRIPTION
  This function handles IOCTL_RESULT event sent back from BIT daemon

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_ioctl_result(
  uint32 q_handle_id, 
  uint32 transaction_id, 
  gpsone_bit_event_payload_type *event)
{
  uint32 opcode = GPSONE_BIT_TRANSID_OPCODE(transaction_id);  // higher 16 bits
  boolean ioctl_error = FALSE;
  pd_comms_ioctl_ack_params_type z_ioctl_ack;

  if (event->result == GPSONE_BIT_STATUS_SUCCESS)
  {
    z_ioctl_ack.q_transaction_id = z_handle_table[q_handle_id].q_ioctl_transaction_id;
    z_ioctl_ack.q_argument_len = 0;

    switch(opcode)
    {
    case GPSONE_BIT_IOCTL_FORCE_DORMANCY:
      z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_FORCE_DORMANCY; 
      break;

    case GPSONE_BIT_IOCTL_UNFORCE_DORMANCY:
      z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY; 
      break;

    case GPSONE_BIT_IOCTL_GET_LOCAL_IP_ADDR:
      z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_GET_IP_ADDR; 
      if (event->arg.ipaddr.type != GPSONE_BIT_IP_V4)
      {
        // FIXME: GET_LOCAL_IP_ADDR ioctl will be called from KDDI/JGPS only, currently
        //  only v4 is supported in JGPS protocol code
        PDCOMM_APP_MSG_ERROR("Got non-IPv4 address(type=%d) in IOCTL_RESULT event (handle_id %d)",
                              event->arg.ipaddr.type, q_handle_id, 0);
        ioctl_error = TRUE;
      }
      else
      {
        z_ioctl_ack.q_argument_len = sizeof(event->arg.ipaddr.addr.v4_addr);
        z_ioctl_ack.p_argument = (uint8*)&(event->arg.ipaddr.addr.v4_addr);
      }
      break;

    default:
      PDCOMM_APP_MSG_ERROR(" Received INVALID ioctl opcode %d for IOCTL_RESULT event (handle_id %d)",
                            opcode, q_handle_id,0);
      ioctl_error = TRUE;
      break;
    }
  }
  else
  {
    ioctl_error = TRUE;
  }

  if (ioctl_error == FALSE) // success case
  {
    (void)pd_comms_send_ioctl_ack_to_client(z_handle_table[q_handle_id].z_handle,
                                              z_handle_table[q_handle_id].q_source_thread_id,
                                              z_handle_table[q_handle_id].q_base_msg_id,
                                              &z_ioctl_ack);
  }
  else
  {
    pd_comms_tcp_task_report_error(q_handle_id, z_handle_table[q_handle_id].q_ioctl_transaction_id,
      z_handle_table[q_handle_id].e_link_status, (uint32)PDCOMM_IOCTL_ERROR_UNKNOWN);  // TODO: any better error code?

  }
}




/*===========================================================================
FUNCTION pd_comms_tcp_task_abort_all_connections

DESCRIPTION
  This function handles aborting all connections

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_abort_all_connections(
  int32  q_result)
{
  int i;

  PDCOMM_APP_MSG_ERROR("Abort all connections(error=%d)", q_result, 0 , 0);

  for(i = 0; i < PDCOMM_MAX_HANDLES; i++)
  {
    if (z_handle_table[i].in_use == TRUE && z_handle_table[i].e_link_status != PDCOMM_STATE_DISCONNECTED)
    {
      /* use 0 as dummy transaction id, will send ERROR_RPT to PDCOMM client */
      pd_comms_tcp_task_report_error(i, z_handle_table[i].q_disconnect_transaction_id, 
        z_handle_table[i].e_link_status, q_result);  

      if (z_handle_table[i].e_link_status == PDCOMM_STATE_CONNECTED)
      {
        pd_comms_tcp_task_tcp_disconnect_after_error(z_handle_table[i].z_handle, FALSE, TRUE); // abort = FALSE
        // tcp_task_disconnect_handle() will reset the read/write state
      }
    }
  }
}


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_bit_event_cb

DESCRIPTION
  Whenever there is a BIT event callback, an IPC message is sent to PDCOMM APP.
  This IPC message is handled in this function. 

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_bit_event_cb(
  /*Pointer to IPC message containing the BIT event callback information*/
  const os_IpcMsgType* p_msg )
{
  uint32                             q_handle_id;
  pd_comms_tcp_task_bit_event_cb_ipc_hdr_type *p_hdr_ptr;
  gpsone_bit_event_e_type            event;
  gpsone_bit_event_payload_type      *p_event_payload;
  boolean                            u_forward_event = FALSE;
  uint32                             l_result = 0;

  if (p_msg == NULL || p_msg->p_Data == NULL)
  {
    return;
  }

  p_hdr_ptr = (pd_comms_tcp_task_bit_event_cb_ipc_hdr_type *)p_msg->p_Data;
  q_handle_id = p_hdr_ptr->handle_id;
  p_event_payload = &p_hdr_ptr->event_payload;
  event = p_event_payload->event;

  if (event == GPSONE_BIT_EVENT_OPEN_RESULT || event == GPSONE_BIT_EVENT_CLOSE_RESULT)
  {
    gpsone_bit_process_event(p_event_payload); // run event processing in PDCOMM TCP Task context
    return;
  }

  if ( q_handle_id < PDCOMM_MAX_HANDLES &&  z_handle_table[q_handle_id].in_use == TRUE)
  {
    pd_comms_tcp_task_reset_inactivity_count(q_handle_id);  
    PDCOMM_APP_MSG_LOW("BIT event 0x%x callback received for handle_id[%d]",(uint32)event,q_handle_id,0);
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("BIT event callback: handle_id %d out of range", q_handle_id, 0, 0);
    return;
  }

  // We don't handle OPEN_RESULT/CLOSE_RESULT here, they are handled in gpsone_bit_api.c
  switch(event)
  {
  case GPSONE_BIT_EVENT_CONNECT_RESULT:    /* event to report result for connect */
    PDCOMM_APP_MSG_LOW("Got CONNECT_RESULT event(session_handle=0x%x handle_id=%d result=%d)", 
                       (uint32)p_hdr_ptr->session_handle, q_handle_id, p_event_payload->result);
    if (p_event_payload->result == GPSONE_BIT_STATUS_SUCCESS)
    {
      if (p_hdr_ptr->session_handle != GPSONE_BIT_INVALID_SESSION_HANDLE)
      {
        z_handle_table[q_handle_id].session_handle = p_hdr_ptr->session_handle;
        PDCOMM_APP_MSG_MED("Connect successful (session_handle=0x%x) for handle_id[%d]", (uint32)p_hdr_ptr->session_handle, q_handle_id,0);
      }
      else
      {
        PDCOMM_APP_MSG_ERROR("Invalid session handle received for successful CONNECT_RESULT (handle_id[%d])",q_handle_id,0,0);
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32)PDCOMM_CONNECT_ERROR_UNKNOWN);
      }

      /*If this is a NON-SSL link, notify client that the link is connected */
      if(z_handle_table[q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
      {
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, PDCOMM_STATE_CONNECTED,
                                       TRUE,(uint32)PDCOMM_RESULT_OK);
      }
      else
      {
        /*Continue with opening SSL on this socket connection*/  
        pd_comms_tcp_task_open_ssl(z_handle_table[q_handle_id].z_handle); 
      }
    }
    else
    {
      pd_comms_connect_result_type    e_conn_error = PDCOMM_CONNECT_ERROR_UNKNOWN;
      pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32)e_conn_error);
    }
    break;

  case GPSONE_BIT_EVENT_DISCONNECT_RESULT: /* event to report result for disconnect */
    PDCOMM_APP_MSG_LOW("Got DISCONNECT_RESULT event(session_handle=0x%x handle_id=%d result=%d)", 
                       (uint32)p_hdr_ptr->session_handle, q_handle_id, p_event_payload->result);
    if (p_event_payload->result == GPSONE_BIT_STATUS_SUCCESS)
    {
      /* For both SSL & non_SSL link, notify client that the link is connected */
      //if(z_handle_table[q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
      {
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32)PDCOMM_RESULT_OK);
      }
    }
    else
    {
      pd_comms_disconnect_result_type    e_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;
      pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32)e_disc_error);
    }
    break;

  case GPSONE_BIT_EVENT_SEND_RESULT:       /* event to report result for send */
     PDCOMM_APP_MSG_LOW("Got SEND_RESULT event(handle_id=%d result=%d bytes=%d)", 
                        q_handle_id, p_event_payload->result, p_event_payload->arg.bytes_sent);
     /*If this is a SSL link, simply forward the WRITE_EVENT to SSL layer*/
    if (z_handle_table[q_handle_id].z_connect_params.e_use_security != SECURITY_NONE)
    {
      if (z_handle_table[q_handle_id].ssl_to_write_bytes == 0)
      {
        // We did not send any data to BIT daemon, should ignore the event
        PDCOMM_APP_MSG_ERROR("Recevied SEND_RESULT event(%d bytes) with no pending SSL send (handle_id[%d])!", 
                             p_event_payload->arg.bytes_sent, q_handle_id,0);
      }
      else 
      {
        if (p_event_payload->result == GPSONE_BIT_STATUS_SUCCESS)
        {
          z_handle_table[q_handle_id].ssl_written_bytes += p_event_payload->arg.bytes_sent;
          if (z_handle_table[q_handle_id].ssl_written_bytes >= z_handle_table[q_handle_id].ssl_to_write_bytes)
          {
            // if we have not got all the SEND_RESULT events yet, wait till we got all
            z_handle_table[q_handle_id].ssl_to_write_bytes = 0;
            u_forward_event = TRUE; 
            l_result = DS_WRITE_EVENT; 
          }
        }
        else
        {
          // write failure, need to close socket, DS_xxx_EVENT can't specify success/failure, 
          // so pass in DS_CLOSE_EVENT for any read/write error
          u_forward_event = TRUE; 
          l_result = DS_CLOSE_EVENT; // TODO: do we need to tear down connnection?
        }
      }
    }
    else 
    {
      if (z_handle_table[q_handle_id].w_num_bytes_to_write == 0) 
      {
        // We did not send any data to BIT daemon, should ignore the event
        PDCOMM_APP_MSG_ERROR("Recevied SEND_RESULT event with no pending send (handle_id[%d])!", q_handle_id,0,0);
      }
      else 
      {
        if (p_event_payload->result == GPSONE_BIT_STATUS_SUCCESS)
        {
          //if ( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED )
          pd_comms_tcp_task_handle_write_result(q_handle_id, p_event_payload->arg.bytes_sent);
        }
        else  // failed to send, notify client about write error
        { 
          pd_comms_tcp_task_handle_write_result(q_handle_id, 0); // 0 means error
        }
      }
    }
    break;

  case GPSONE_BIT_EVENT_DATA_READY:        /* event to notify PDSM of new data ready */
    PDCOMM_APP_MSG_LOW("Got DATA_READY event(handle_id=%d result=%d bytes=%d)", 
                        q_handle_id, p_event_payload->result, p_event_payload->arg.bytes_available);
    /*If this is a SSL link, simply forward the READ_EVENT to SSL layer*/
    if (z_handle_table[q_handle_id].z_connect_params.e_use_security != SECURITY_NONE)
    {
      if (p_event_payload->result == GPSONE_BIT_STATUS_SUCCESS)
      {
        u_forward_event = TRUE; 
        l_result = DS_READ_EVENT;
      }
      else
      {
        PDCOMM_APP_MSG_ERROR("SSL connection got DATA_READY event with error 0x%x (handle_id[%d])!", p_event_payload->result, q_handle_id,0);
        u_forward_event = TRUE; 
        l_result = DS_CLOSE_EVENT;
      }
    }
    else
    {
      if (p_event_payload->result == GPSONE_BIT_STATUS_SUCCESS)
      {
        pd_comms_tcp_task_handle_read_result(q_handle_id, p_event_payload->arg.bytes_available);
      }
      else
      {
        // something is wrong, ignore this event, DATA_READY event has a failure result?
        PDCOMM_APP_MSG_ERROR("Recevied DATA_READY event with error 0x%x (handle_id[%d])!", p_event_payload->result, q_handle_id,0);
        // FIXME: should tear down connection
      }
    }
    break;

  case GPSONE_BIT_EVENT_IOCTL_RESULT:      /* event to report result for ioctl */  
    PDCOMM_APP_MSG_LOW("Got IOCTL_RESULT event(handle_id=%d result=%d transid=0x%x)", 
                       q_handle_id, p_event_payload->result, p_hdr_ptr->transaction_id);
    pd_comms_tcp_task_handle_ioctl_result(q_handle_id, p_hdr_ptr->transaction_id, p_event_payload);
    break;

  case GPSONE_BIT_EVENT_NETWORK_STATUS:    /* event to notify PDSM of network status change */  
    /* NOT IMPLEMENTED */
    break;

  case GPSONE_BIT_EVENT_ABORT_CONNNECTION:  /* event to notify PDSM to abort this connection */
    if (z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED)
    {
      /* TODO: any better error code ? */
      pd_comms_tcp_task_report_error(q_handle_id, z_handle_table[q_handle_id].q_disconnect_transaction_id, 
        z_handle_table[q_handle_id].e_link_status, (int32)PDCOMM_RESULT_ERROR_UNKNOWN);

      pd_comms_tcp_task_tcp_disconnect_after_error(z_handle_table[q_handle_id].z_handle, FALSE, TRUE); // abort = FALSE
      // tcp_task_disconnect_handle() will reset the read/write state
    }
    break;

  case GPSONE_BIT_EVENT_ABORT_ALL:  /* event to notify PDSM to abort all connections */
    /* TODO: any better error code ? */
    pd_comms_tcp_task_abort_all_connections((int32)PDCOMM_RESULT_ERROR_UNKNOWN);  
    break;

  default:
    PDCOMM_APP_MSG_ERROR("Unknown BIT event 0x%x received in PDCOMM TCP Task", (uint32)event,0,0);
    break;
  }

  /*
  *  Forward events meant for SSL layer
  */
  if (u_forward_event &&
      (z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTED) &&
      (z_handle_table[q_handle_id].z_connect_params.e_use_security != SECURITY_NONE) &&
      (z_handle_table[q_handle_id].z_ssl_handle != NULL) )
  {
    PDCOMM_APP_MSG_MED("Calling ds2_default_socket_event_handler for event[%ld] handle[%d]",
                             l_result,q_handle_id,0);

    z_handle_table[q_handle_id].ssl_events |= l_result;  // save all events

    pd_comms_tcp_task_ssl_forward_event((sint15)q_handle_id);
  }

}/*pd_comms_tcp_task_handle_bit_event_cb*/


/*===========================================================================
FUNCTION gpsone_bit_post_event

DESCRIPTION
  This function is called from the context of BIT daemon when a BIT
  event occurs(gpsone_bit_notify being called). PDCOMM TCP task needs to supply
  this function. The function simply creates an IPC message and sends it to 
  PDCOMM APP so that it can process the BIT event.

  This is the only function calling from BIT daemon context into PDCOMM
  TCP task context.

DEPENDENCIES
  None

RETURN VALUE
  Returns SUCCESS if successfully sent IPC to PDCOMM task, otherwise FAILURE

SIDE EFFECTS
  None

===========================================================================*/
gpsone_bit_status_e_type gpsone_bit_post_event
(
  gpsone_bit_session_handle_type         session_handle, 
  uint32                                 transaction_id,
  const gpsone_bit_event_payload_type    *p_event_payload)
{
  pd_comms_tcp_task_bit_event_cb_ipc_hdr_type* p_hdr_ptr; 
  os_IpcMsgType* p_ipc_ptr;
  uint32 q_handle_id = 0;
  gpsone_bit_status_e_type status = GPSONE_BIT_STATUS_SUCCESS;

  if (p_event_payload->event == GPSONE_BIT_EVENT_DATA_READY ||
      p_event_payload->event == GPSONE_BIT_EVENT_ABORT_CONNNECTION) // GPS daemon-initiated message, no q_handle_id
  {
    int i;
    for(i = 0; i < PDCOMM_MAX_HANDLES; i++)
    {
      if (z_handle_table[i].in_use == TRUE && z_handle_table[i].session_handle == session_handle)
      {
        q_handle_id = i;  // using session_handle to look up q_handle_id
        break;
      }
    }

    if (i >= PDCOMM_MAX_HANDLES)  // not found!
    {
      BIT_MSG_ERROR("DATA_READY event's session handle 0x%x not found (trans_id=0x%x)!", session_handle,transaction_id,0);
    }
  }
  else
  {
    q_handle_id = GPSONE_BIT_TRANSID_HANDLE(transaction_id); // q_handle_id is already encoded in trans_id
  }

  p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_bit_event_cb_ipc_hdr_type),
                         IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);
                         
  if (p_ipc_ptr == NULL)
  {
    BIT_MSG_HIGH("IPC Create failed during BIT event callback. Handle Id: %d", q_handle_id,0,0);
    status = GPSONE_BIT_STATUS_IPC_FAILURE;
  }
  else
  {
    /*lint -e826 */  
    p_hdr_ptr = (pd_comms_tcp_task_bit_event_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
    /*lint +e826 */
    p_ipc_ptr->q_MsgId = (uint32)PDCOMM_APP_TASK_BIT_EVENT_CB;
    p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

    p_hdr_ptr->handle_id = q_handle_id;
    p_hdr_ptr->session_handle = session_handle;
    p_hdr_ptr->transaction_id = transaction_id;  // save the entire id
    p_hdr_ptr->event_payload = *p_event_payload;

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
    {
      BIT_MSG_ERROR("IPC Send failed during BIT event callback. Handle Id: %d", q_handle_id,0,0);
      (void)os_IpcDelete(p_ipc_ptr);
      status = GPSONE_BIT_STATUS_IPC_FAILURE;
    }
  }

  return status;
} /*lint !e715 !e818 */ 
/*gpsone_bit_post_event*/


#endif /* FEATURE_GNSS_BIT_API */

#endif /* FEATURE_CGPS_PDCOMM */
