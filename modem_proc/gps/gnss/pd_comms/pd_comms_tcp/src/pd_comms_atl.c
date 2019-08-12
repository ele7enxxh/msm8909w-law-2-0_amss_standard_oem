/*======================================================================

               PDCOMM ATL API definitions file

 GENERAL DESCRIPTION
   This file contains ATL API implementations. ATL inteface is used
   by PD-COMM-TCP task to open and close physical link(e.g. PPP)
   connections. A physical link proxy registers open and close
   function implementations with the ATL layer. These functions are
   called when PD-COMM-TCP task requests a physical link open/close
   operation.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
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

  $$

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 06/02/08   kj   Compensate for de-featurization of pdsm_atl.h
 10/11/07   bp   Incorporated ATL based DNS lookup functionality
 -
 04/03/07   mr   Initial creation of file.
======================================================================*/
#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "pdsm_atl.h"
#ifndef __GRIFFON__
#ifdef FEATURE_PDSM_ATL
#include "msg.h"
#include "pd_comms_atl_i.h"
#include "pd_comms_tcp_data.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "dserrno.h"
#include "event.h"
#include "sm_log.h"

pd_comms_atl_func_table_struct_type p_atl_if_array[PD_COMMS_ATL_TYPE_NUM];
pd_comms_atl_dflt_func_table_struct_type p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_NUM];

pdsm_atl_dns_lookup_f_type *p_atl_dns_lookup_f;

static boolean pd_comms_atl_handle_atl_events(
                              pdsm_atl_session_handle_type    session_handle,
                              pdsm_atl_event_type             event_id,
                              const pdsm_atl_open_params_type       *open_params );

pd_comms_tcp_task_operation_status_type pd_comms_atl_shim_dns_lookup
(
  pdsm_atl_session_handle_type     session_handle,
  const pdsm_atl_dns_params_type   *p_dns_lookup_params,
  pd_comms_connect_result_type     *p_conn_error
);

#endif

/****************** Function Definitions**************************************/

/*===========================================================================

FUNCTION pdsm_atl_l2_proxy_reg

DESCRIPTION
  Proxy PPP clients (i.e. co-processor task) must call this function to
  register with PDSM-Core to forward all PPP connection request to a PPP
  proxy client.

DEPENDENCIES

RETURN VALUE
  TRUE - Successfully Registered with PDSM-Core
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_l2_proxy_reg
(
  pdsm_atl_type                                  q_transport_layer_type,
  pdsm_atl_open_f_type                           *p_open,
  pdsm_atl_close_f_type                          *p_close
)
{

#ifdef FEATURE_PDSM_ATL
  if(q_transport_layer_type & PDSM_ATL_TYPE_IP)
  {
    MSG_MED("PD COMMS IP transport layer proxy open/close registration", 0, 0, 0);

    /* Fill in the table with fucntions which transport layer proxy provides */
    p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_open_fnc = p_open;
    p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_close_fnc = p_close;

    return TRUE;
  }
#endif  /* FEATURE_PDSM_ATL */
  return TRUE;
}


/*===========================================================================

FUNCTION pdsm_atl_l2_proxy_dereg

DESCRIPTION
  Proxy PPP clients (i.e. co-processor task) must call this function to
  deregister with PDSM-Core indicating it no longer will proxy the connection.

DEPENDENCIES

RETURN VALUE
  TRUE - Successfully deregistered with ATL
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_l2_proxy_dereg
(
  pdsm_atl_type                                  q_transport_layer_type
)
{
#ifdef FEATURE_PDSM_ATL
  if(q_transport_layer_type & PDSM_ATL_TYPE_IP)
  {
    MSG_MED("PDCOMMS IP transport layer proxy open/close de-reg", 0, 0, 0);

    /* NULL out the callback functions */
    p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_open_fnc  = NULL;
    p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_close_fnc = NULL;

    return TRUE;
  }
#endif  /* FEATURE_PDSM_ATL */

  return TRUE;
}

/*===========================================================================
FUNCTION pdsm_atl_dns_proxy_reg

DESCRIPTION
  Proxy DNS service providers (eg: Serial PC-PDAPI s/w) call this function
  to register callbacks with the PDSM core, which may be invoked to
  resolve names to IP addresses.

  To account for the fact that 'default' dns lookup function needs to have the
  same type signature as the atl dns lookup function, both functions are universally
  defined to accept 3 parameters as defined in the type definition below:

  typedef pdsm_atl_operation_return_value_e_type (pdsm_atl_dns_lookup_f_type)
  (
    pdsm_atl_session_handle_type    session_handle,
    pdsm_atl_dns_params_type        dns_lookup_params,
    pd_comms_return_type *p_conn_error
  );

  The issue though is that atl dns function registered by an external entity
  (such as serial_pdapi) does NOT have the third parameter. To bridge this
  difference, a 'shim' function is added that accepts the three parameters and
  in turn calls the registered atl function with just the first two.
  Depending on the completion status of the function call, the p_conn_error
  pointer value is filled in.

DEPENDENCIES

RETURN VALUE
  TRUE - Successfully Registered with PDSM-Core
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_dns_proxy_reg
(
 pdsm_atl_type                                  transport_layer_type,
 pdsm_atl_dns_lookup_f_type                 *p_dns_lookup
)
{
#ifdef FEATURE_PDSM_ATL
  if(transport_layer_type & PDSM_ATL_TYPE_IP)
  {
    MSG_MED("PDSM IP transport layer DNS proxy registration", 0, 0, 0);
    p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_dns_lookup_fnc = pd_comms_atl_shim_dns_lookup;
    p_atl_dns_lookup_f = p_dns_lookup;
    return TRUE;
  }
#endif  /* FEATURE_PDSM_ATL */

  return FALSE;
}

/*===========================================================================

FUNCTION pdsm_atl_dns_proxy_dereg

DESCRIPTION
  The DNS Proxy can call this function to deregister with PDSM-Core

DEPENDENCIES

RETURN VALUE
  TRUE - Successfully Deregistered with PDSM-Core
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_dns_proxy_dereg
(
  pdsm_atl_type                                  transport_layer_type
)
{
#ifdef FEATURE_PDSM_ATL
  if(transport_layer_type & PDSM_ATL_TYPE_IP)
  {
    MSG_MED("PDSM IP transport layer DNS proxy deregistration", 0, 0, 0);

    /* Fill in the table with functions which transport layer proxy provides */
    p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_dns_lookup_fnc = NULL;
    p_atl_dns_lookup_f = NULL;

    return TRUE;
  }
#endif  /* FEATURE_PDSM_ATL */

  return FALSE;
}

/*===========================================================================

FUNCTION pdsm_atl_post_event

DESCRIPTION
  This function is called by transport layer proxy to inform PDSM network events

DEPENDENCIES

RETURN VALUE
  TURE: If PDSM abstract layer recognizes the event and is able to send event to upper
        layer.
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_post_event
(
  pdsm_atl_type                 transport_layer_type,
  pdsm_atl_session_handle_type  session_handle,
  pdsm_atl_event_type           event,
  pdsm_atl_open_params_type     open_param_info
)
{
#ifdef FEATURE_PDSM_ATL
  return pd_comms_atl_handle_atl_events(session_handle,event,&open_param_info);
#else
  return TRUE;
#endif
}

/*===========================================================================

FUNCTION pdsm_atl_dns_proxy_response

DESCRIPTION
  The DNS Proxy Clients must call this function to provide the DNS lookup
  results to PDSM-Core

DEPENDENCIES

RETURN VALUE
  TRUE - If PDSM ATL is able to accept the response and send it to upper layers
  FALSE - Otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_dns_proxy_response
(
  pdsm_atl_type                 transport_layer_type,
  pdsm_atl_session_handle_type  session_handle,
  pdsm_atl_event_type           event_type,
  pdsm_atl_dns_response_type    dns_results
)
{
#ifdef FEATURE_PDSM_ATL
  pd_comms_app_dns_cb_data_type *p_hdr_ptr;
  os_IpcMsgType* p_ipc_ptr;


  p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_app_dns_cb_data_type),
                         IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);

  if(p_ipc_ptr == NULL)
  {
    MSG_HIGH("IPC Create failed during pdsm_atl_post_event. Handle Id: %d",
                        (uint32)session_handle,0,0);
    return FALSE;
  }
  else
  {

    /*lint -e826 */
    p_hdr_ptr = (pd_comms_app_dns_cb_data_type*) p_ipc_ptr->p_Data;
    /*lint +e826 */

    p_ipc_ptr->q_MsgId =  (uint32)PDCOMM_APP_TASK_ATL_DNS_CB;
    p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

    /*
      we have 3 pieces of info to pass back to pdsm:
      1. handle
      2. actual ip address
      3. dns operation completion flag
     */

    p_hdr_ptr->handle.q_handle_id = (uint32)session_handle;

    /*
      pdsm_atl_dns_response_type has an ip_addr field of type pdsm_atl_ip_addr_type.
      Before the ip_addr information is passed to the PDSM core, is has to be converted
      to the type 'pdcomm_ip_addr_type', which is the format pd_comms stores ip_addresses.
      Since the two types are identical, conversion from one to another can be done
      by direct copying.
     */
    memscpy(&p_hdr_ptr->ip_addr, sizeof(p_hdr_ptr->ip_addr), &dns_results.ip_addr, sizeof(dns_results.ip_addr));
    if (event_type == PDSM_ATL_EVENT_DNS_LOOKUP_SUCCESS)
    {
      ((pd_comms_app_dns_cb_data_type*) p_ipc_ptr->p_Data)->status = SUCCEEDED;
    }
    else if (event_type == PDSM_ATL_EVENT_DNS_LOOKUP_FAIL)
    {
      ((pd_comms_app_dns_cb_data_type*) p_ipc_ptr->p_Data)->status = FAILED;
    }

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
    {
      MSG_HIGH("IPC Send failed during pdsm_atl_post_event. Handle Id: %d",
                           (uint32)session_handle,0,0);
      (void)os_IpcDelete(p_ipc_ptr);

      return FALSE;
    }
  }
  /*
   *  Tell the caller that we handled the event
   */
  return TRUE;
#else /*FEATURE_PDSM_ATL*/
  return FALSE;
#endif  /*FEATURE_PDSM_ATL*/
}

/*===========================================================================

FUNCTION pdsm_atl_reg

DESCRIPTION
  This function is called by transport layer proxy to inform PDSM that it is ready
  to serve.

DEPENDENCIES

RETURN VALUE
  TURE: If PDSM abstract layer allows registration
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_atl_reg
(
  pdsm_atl_type                                  transport_layer_type,
  pdsm_atl_session_handle_operation_f_type       *session_handle_operation_fp,
  pdsm_atl_open_f_type                           *open_fp,
  pdsm_atl_close_f_type                          *close_fp,
  pdsm_atl_connect_f_type                        *connect_fp,
  pdsm_atl_disconnect_f_type                     *disconnect_fp,
  pdsm_atl_send_f_type                           *send_fp,
  pdsm_atl_recv_f_type                           *recv_fp,
  pdsm_atl_dormancy_operation_f_type             *dormancy_fp
)
{
  return FALSE;
}


/*===========================================================================
    The remainder of this file contains functions which are
    only needed with FEATURE_PDSM_ATL is defined.  They need not be stubbed.
===========================================================================*/

#ifdef FEATURE_PDSM_ATL

/*
 ******************************************************************************
 * pd_comms_atl_handle_atl_events
 *
 * Function description:
 *
 * This function is called by PDSM ATL whenever an ATL event is posted.
 *
 * Parameters:
 *
 *  pdsm_atl_session_handle_type  - session handle (future)
 *  pdsm_atl_event_type           - Event ID
 *  pdsm_atl_open_params_type     - Open Parameters
 *
 * Return value:
 *  TRUE if successfully handled;
 *
 ******************************************************************************
*/
static boolean pd_comms_atl_handle_atl_events(
                              pdsm_atl_session_handle_type    session_handle,
                              pdsm_atl_event_type             event_id,
                              const pdsm_atl_open_params_type       *open_params )
{
#ifdef FEATURE_PDSM_ATL
  pd_comms_tcp_task_tcp_cb_ipc_hdr_type* p_hdr_ptr;
  os_IpcMsgType* p_ipc_ptr;
  uint32 q_ds_event_id;

  /*
   * Convert ATL events into DSS type events for PD-COMM-TCP task
   */
  switch( event_id )
  {
    case PDSM_ATL_EVENT_OPEN_SUCCESS:
      MSG_MED("ATL Open Successful",0,0,0);
      q_ds_event_id = DS_ENETISCONN;
      // Log the connection status 
      pd_comms_log_connection_status_event(PD_EVENT_NW_CONNECTED);
      break;

    case PDSM_ATL_EVENT_OPEN_FAIL:
      MSG_HIGH("ATL Open Failed",0,0,0);
      q_ds_event_id = DS_ENETNONET;
      break;

    case PDSM_ATL_EVENT_CLOSE_SUCCESS:
      MSG_MED("ATL Close Successful",0,0,0);
      q_ds_event_id = DS_ENETNONET;
      break;

    case PDSM_ATL_EVENT_CLOSE_FAIL:
      MSG_HIGH("ATL Close Failed",0,0,0);
      q_ds_event_id = DS_ENETNONET;
      break;

    /*
     *  The PDSM_ATL_EVENT_L2_PROXY_xxxxx events are ignored*/

    case PDSM_ATL_EVENT_L2_PROXY_OPEN_ACK:
      MSG_MED("ATL Open Ack Received",0,0,0);
      // Log the connection status 
      pd_comms_log_connection_status_event(PD_EVENT_NW_CONNECTING);
      return TRUE;

    case PDSM_ATL_EVENT_L2_PROXY_CLOSE_ACK:
      MSG_MED("ATL Close Ack Received",0,0,0);
      return TRUE;

    case PDSM_ATL_EVENT_DNS_LOOKUP_REQUEST_ACK:
      MSG_MED("ATL DNS lookup request Ack Received",0,0,0);
      return TRUE;

    default:
      MSG_ERROR("Invalid ATL Event Returned %d",event_id,0,0);
      return FALSE;
  }


  p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_tcp_cb_ipc_hdr_type),
                         IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);

  if(p_ipc_ptr == NULL)
  {
    MSG_HIGH("IPC Create failed during pdsm_atl_post_event. Handle Id: %d",
                        (uint32)session_handle,0,0);
    return FALSE;
  }
  else
  {
    /*lint -e826 */
    p_hdr_ptr = (pd_comms_tcp_task_tcp_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
    /*lint +e826 */

    p_ipc_ptr->q_MsgId =  (uint32)PDCOMM_APP_TASK_DSS_NETWORK_CB;
    p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

    p_hdr_ptr->handle.q_handle_id = (uint32)session_handle;
    p_hdr_ptr->handle.e_link_type = LINK_TCP;
    p_hdr_ptr->event_mask = q_ds_event_id;
    memscpy(&p_hdr_ptr->z_open_resp,sizeof(p_hdr_ptr->z_open_resp), open_params, sizeof(*open_params));

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
    {
      MSG_HIGH("IPC Send failed during pdsm_atl_post_event. Handle Id: %d",
                           (uint32)session_handle,0,0);
      (void)os_IpcDelete(p_ipc_ptr);

      return FALSE;
    }
  }

  /*
   *  Tell the caller that we handled the event
   */
  return TRUE;
#else /*FEATURE_PDSM_ATL*/
  return FALSE;
#endif /*FEATURE_PDSM_ATL*/
}

/*==========================================================================
FUNCTION pd_comms_atl_shim_dns_lookup

DESCRIPTION
  A 'bridge' between the dns loookup calls issued by PDCOMM and those
  registered by the ATL later (lookup description of pdsm_atl_dns_proxy_reg)
  for further information.
===========================================================================*/
pd_comms_tcp_task_operation_status_type pd_comms_atl_shim_dns_lookup
(
  pdsm_atl_session_handle_type     session_handle,
  const pdsm_atl_dns_params_type   *p_dns_lookup_params,
  pd_comms_connect_result_type     *p_conn_error
)
{
  pdsm_atl_operation_return_value_e_type e_pdsm_return;

  /* initialize error information to unknown since we don't know better better
     NOTE: the information is irrelevant if no error results
   */
  if (p_conn_error != NULL) {
    *p_conn_error = PDCOMM_CONNECT_ERROR_UNKNOWN;
  }

  /* if this is called, then atl_dns_lookup_f_ptr must not be NULL */
  if (p_atl_dns_lookup_f == NULL)
  {
    return FAILED;
  }

  e_pdsm_return = p_atl_dns_lookup_f(session_handle, *p_dns_lookup_params);

  if(e_pdsm_return == PDSM_ATL_OPERATION_SUCCESS)
  {
    return SUCCEEDED; /* should never happen as we always have to wait! */
  }
  else if (e_pdsm_return == PDSM_ATL_OPERATION_WAIT)
  {
    return PENDING;
  }

  /* if we got here, then something's wrong, and we don't know what it is
     so just return error.
  */
  return FAILED;

}

/*===========================================================================

FUNCTION pd_comms_atl_i_dflt_proxy_reg

DESCRIPTION
   This function is called by pd_comms_tcp task to register default PPP
   open/close functions with the ATL layer. If a PPP open/close function
   is not provided by the PPP open/close proxy, the default functions will
   be used.
DEPENDENCIES

RETURN VALUE
  TRUE - Successfully Registered default functions
  FALSE - Error

SIDE EFFECTS

===========================================================================*/
boolean pd_comms_atl_i_dflt_proxy_reg
(
  pdsm_atl_type                                  q_transport_layer_type,
  pd_comms_atl_i_open_f_type                     *p_open,
  pd_comms_atl_i_close_f_type                    *p_close,
  pd_comms_atl_i_dns_lookup_f_type               *p_dns_lookup
)
{
  if(q_transport_layer_type & PDSM_ATL_TYPE_IP)
  {
    MSG_MED("PD COMMS IP transport layer proxy default open/close registration", 0, 0, 0);

    /* Fill in the table with fucntions which transport layer proxy provides */
    p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_open_fnc = p_open;
    p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_close_fnc = p_close;
    p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_dns_lookup_fnc = p_dns_lookup;

    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION pd_comms_atl_i_open

DESCRIPTION
  This function is called by PD-COMMS-TCP task to establish a
  physical link.

DEPENDENCIES

RETURN VALUE
  PDCOMM_RESULT_OK: If physical link is established.
  PDCOMM_RESULT_ERROR_UNKNOWN:    If physical link can not be established.
  PDCOMM_RESULT_WAIT:    If physical link is in the process of being stablished.

SIDE EFFECTS

===========================================================================*/
pd_comms_return_type pd_comms_atl_i_open
(
  pd_comms_handle_type           z_session_handle,
  pd_comms_atl_open_params_type* z_open_params
)
{
  pdsm_atl_operation_return_value_e_type e_pdsm_return;

  if(z_open_params == NULL)
  {
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

#ifdef FEATURE_CGPS_LBS_TASK
  if((z_session_handle.e_link_type != LINK_TCP) && (z_session_handle.e_link_type != LINK_UDP))
#else
  if(z_session_handle.e_link_type != LINK_TCP)
#endif
  {
    MSG_ERROR("Invalid link type received in call to ATL open",0,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  if(z_session_handle.q_handle_id >= PDCOMM_MAX_HANDLES)
  {
    MSG_ERROR("Invalid session handle received in call to ATL open",0,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  if(p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_open_fnc != NULL)
  {
   e_pdsm_return = p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_open_fnc(
                                   (int)z_session_handle.q_handle_id,
                                   z_open_params->z_open_params);

   if(e_pdsm_return == PDSM_ATL_OPERATION_SUCCESS)
   {
     return PDCOMM_RESULT_OK;
   }
   else if (e_pdsm_return == PDSM_ATL_OPERATION_WAIT)
   {
     return PDCOMM_RESULT_WAIT;
   }
   else
   {
     return PDCOMM_RESULT_ERROR_UNKNOWN;
   }

  }
  else if (p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_open_fnc != NULL)
  {
     return p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_open_fnc(z_session_handle,
                                                                           z_open_params);
  }
  else
  {
    MSG_ERROR("No ATL open service registered",0,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }
}

/*===========================================================================

FUNCTION pd_comms_atl_i_close

DESCRIPTION
  This function is called by PD-COMMS-TCP task to tear down a
  physical link.

DEPENDENCIES

RETURN VALUE
  PDCOMM_RESULT_OK: If physical link is down.
  PDCOMM_RESULT_ERROR_UNKNOWN:    If physical link can not be torn down.
  PDCOMM_RESULT_WAIT:    If physical link is in the process of being torn down.

SIDE EFFECTS

===========================================================================*/
pd_comms_return_type pd_comms_atl_i_close
(
  pd_comms_handle_type       z_session_handle,
  boolean                    u_e911_close
)
{
  pdsm_atl_operation_return_value_e_type e_pdsm_return;

#ifdef FEATURE_CGPS_LBS_TASK
  if((z_session_handle.e_link_type != LINK_TCP) && (z_session_handle.e_link_type != LINK_UDP))
#else
  if(z_session_handle.e_link_type != LINK_TCP)
#endif
  {
    MSG_ERROR("Invalid link type received in call to ATL close",0,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  if(z_session_handle.q_handle_id >= PDCOMM_MAX_HANDLES)
  {
    MSG_ERROR("Invalid session handle received in call to ATL close",0,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }

  if(p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_close_fnc != NULL)
  {
    e_pdsm_return = p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_close_fnc(
                                            (int)z_session_handle.q_handle_id,
                                            u_e911_close);

    if(e_pdsm_return == PDSM_ATL_OPERATION_SUCCESS)
    {
      return PDCOMM_RESULT_OK;
    }
    else if (e_pdsm_return == PDSM_ATL_OPERATION_WAIT)
    {
      return PDCOMM_RESULT_WAIT;
    }
    else
    {
      return PDCOMM_RESULT_ERROR_UNKNOWN;
    }

  }
  else if (p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_close_fnc != NULL)
  {
    return p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_close_fnc(z_session_handle,
                                                                           u_e911_close);
  }
  else
  {
    MSG_ERROR("No ATL close service registered",0,0,0);
    return PDCOMM_RESULT_ERROR_UNKNOWN;
  }
}

/*==================================================================================

FUNCTION pd_comms_atl_i_dns_lookup

DESCRIPTION
  This function is called by PD-COMMS-TCP task to resolve a name to an IP address.
  The _i_ signifies that the function is internal to the PDSM module.

DEPENDENCIES

RETURN VALUE
  PDCOMM_RESULT_OK: If physical link is established.
  PDCOMM_RESULT_ERROR_UNKNOWN:    If physical link can not be established.
  PDCOMM_RESULT_WAIT:    If physical link is in the process of being stablished.

SIDE EFFECTS

NOTES:
The signature of this function is derived from the following function call from
the 'original' pd_comms_tcp_task.c
  p_hostent_ptr
       = dss_getipnodebyname(
           z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.p_host_name,
           (int32)AF_INET,0,
           (dss_dns_cb_f_type) pd_comms_tcp_task_dns_callback,
           (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id),
           &x_errno );


===========================================================================*/
pd_comms_tcp_task_operation_status_type pd_comms_atl_i_dns_lookup
(
  pd_comms_handle_type           z_session_handle,
  pdsm_atl_dns_params_type       *p_dns_lookup_params,
  pd_comms_connect_result_type*  p_conn_error
)
{

  /*
    simply call the dns resolve function that has been registered with us.
    if nothing's been registered, return call the default pdcomm
    note: return type of ATL functions (called across PDSM and passed through to
    a remote agent such as Serial PDAPI) is
        pdsm_atl_operation_return_value_e_type; however
          return type of functions interfacing pd_comms_atl.c and pd_comms_tcp_task.c
    is pd_comms_return_type
        so return values need to be converted from one form to another.
  */
  if (p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_dns_lookup_fnc != NULL)
  {
    return p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_dns_lookup_fnc(
                        (pdsm_atl_session_handle_type)z_session_handle.q_handle_id,
                        p_dns_lookup_params, p_conn_error);
  }

  else
  {
    if (p_atl_dflt_if_array[(int)PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_dns_lookup_fnc != NULL)
    {
      return p_atl_dflt_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_dns_lookup_fnc(
                             (int)z_session_handle.q_handle_id, p_conn_error);
    }
    else
    {
      MSG_ERROR("No ATL dns service registered",0,0,0);
      return FAILED;
    }
  }
}

/*===========================================================================

FUNCTION pd_comms_atl_i_ppp_proxy_exists

DESCRIPTION
  This function is called by PD-COMMS-TCP task to determine if an ATL based
  open function is registered

DEPENDENCIES

RETURN VALUE
  TRUE : if no ATL open function is registered
  FALSE : if one is registered
SIDE EFFECTS

===========================================================================*/
boolean pd_comms_atl_i_ppp_proxy_exists(void)
{
  if (p_atl_if_array[PD_COMMS_ATL_TYPE_IP_ARR_INDEX].p_open_fnc != NULL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif  /* FEATURE_PDSM_ATL */

#else
boolean pdsm_atl_post_event
(
  pdsm_atl_type                 transport_layer_type,
  pdsm_atl_session_handle_type  session_handle,
  pdsm_atl_event_type           event,
  pdsm_atl_open_params_type     open_param_info
)
{
  return TRUE;
}

boolean pdsm_atl_l2_proxy_reg
(
  pdsm_atl_type                                  q_transport_layer_type,
  pdsm_atl_open_f_type                           *p_open,
  pdsm_atl_close_f_type                          *p_close
)
{
  return TRUE;
}
#endif


