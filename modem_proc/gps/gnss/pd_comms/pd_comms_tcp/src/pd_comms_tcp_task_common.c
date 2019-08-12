/*======================================================================

      PDCOMM Application task implementation file (Common Portion)

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

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_tcp/src/pd_comms_tcp_task_common.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 02/08/13   ssu  Move to the DOG Heartbeat mechanism on DIME and beyond. 
 06/30/11   rh   Ignore C_PD_COMMS_APP_SRV_SYS_CHANGE_NOTIFICATION if BIT enabled
 03/09/11   rh   Added TLS/SSL support for BIT API
 03/08/11   rh   Fixed compiler errors on ULP for new PDCOMM header
 05/12/08   bp   Changed profile finding algo to quite if family is invalid
 03/05/08   bp   Change to specify air interface correctly
 02/14/08   bp   Added DNS lookup on modem feature
 11/06/07   bp   Added support for multiple PDP context 
 11/16/06   mr   Added code to start connection closing on receiving SSL alert of level FATAL or SUSPEND
 11/16/06   mr   Changed write params to include SMS params like mt address and teleservice id.
 11/16/06   mr   Changed SMS read msg cb to include decoded ts data and WAP flag
 11/14/06   mr   Fix for processing pending connect requests.
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_CGPS_PDCOMM 

#include "msg.h"
#include "secapi.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "pd_comms_api.h"
#include "pd_comms_client_api.h"
#include "pd_comms_app_api.h"
#include "pd_comms_tcp_local_api.h"
#include "pd_comms_tcp_data.h"

#ifdef FEATURE_GNSS_BIT_API
#include "gpsone_bit_api.h"
#include "qmi_bit.h"
#endif


/****************** Variables ************************************************/

pd_comms_tcp_task_handle_data_type z_handle_table[PDCOMM_MAX_HANDLES];  /* handle tables (6/4) */

static os_TimerMsgType* p_inactivity_timer;   /* timer for inactivity */
static secapi_handle_type z_hash_handle;      /* handle for secapi hash call */
static os_MutexBlockType z_gethandle_mutex;   /* mutex to guard get/release_handle */
static boolean u_pd_comms_online;             /* PDCOMM Task online/offline */


/****************** Function Definitions**************************************/

/* Initialize the GPS Diag Module */
extern void gpsdiag_init (void);  

// Functions implemented by BIT or DSS APIs
extern void pd_comms_tcp_task_register_callbacks (void);

extern pd_comms_tcp_task_operation_status_type pd_comms_tcp_task_tcp_connect (pd_comms_handle_type z_handle);

extern void pd_comms_tcp_task_tcp_disconnect (pd_comms_handle_type z_handle, boolean b_abort, boolean u_notifyClient);

extern boolean pd_comms_tcp_task_tcp_read (uint32 q_handle_id, boolean u_wait_for_all);

extern boolean pd_comms_tcp_task_tcp_write (pd_comms_handle_type z_handle);

extern void pd_comms_tcp_task_abort_connection (uint32 q_handle_id);

extern void pd_comms_tcp_task_handle_ioctl_force_dormancy (pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr);

extern void pd_comms_tcp_task_handle_ioctl_unforce_dormancy (pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr);

extern void pd_comms_tcp_task_handle_ioctl_get_ip_addr (const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr);

// TLS/SSL-related functions
extern void pd_comms_tcp_task_handle_ioctl_set_ssl_cert (const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr);

extern void pd_comms_tcp_task_handle_ioctl_set_ssl_cert_dir(const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr);

extern void pd_comms_tcp_task_secure_tcp_connect (pd_comms_handle_type z_handle);

extern void pd_comms_tcp_task_secure_tcp_disconnect (pd_comms_handle_type z_handle, boolean b_abort, boolean u_notifyClient);

extern boolean pd_comms_tcp_task_secure_tcp_read (uint32 q_handle_id, boolean u_wait_for_all);

extern boolean pd_comms_tcp_task_secure_tcp_write (pd_comms_handle_type z_handle);

extern void pd_comms_tcp_task_handle_sec_event_cb (const os_IpcMsgType* p_msg);

extern void pd_comms_tcp_task_handle_sec_open_cb (const os_IpcMsgType* p_msg);

extern void pd_comms_tcp_task_handle_sec_close_cb (const os_IpcMsgType* p_msg);

extern void pd_comms_tcp_task_handle_sec_close_net_cb (const os_IpcMsgType* p_msg);

extern boolean pd_comms_tcp_task_udp_bind(
  /*handle_id corresponding to the connection for which bind is needed*/
  pd_comms_handle_type z_handle);


#ifdef FEATURE_GNSS_BIT_API
extern void pd_comms_tcp_task_handle_bit_event_cb (const os_IpcMsgType* p_msg);
#endif /* FEATURE_GNSS_BIT_API */


/*===========================================================================
FUNCTION pd_comms_tcp_task_release_handle

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
static void pd_comms_tcp_task_release_handle(
  /*handle for the handle to be released.*/
  uint32 q_handle_id)
{
  PDCOMM_APP_MSG_MED("Releasing handle [%d]",q_handle_id,0,0);
  memset(&z_handle_table[q_handle_id],0,sizeof(pd_comms_tcp_task_handle_data_type));

  z_handle_table[q_handle_id].in_use                      = FALSE;
  z_handle_table[q_handle_id].client_disconnect_recvd     = FALSE;
  z_handle_table[q_handle_id].l_inactivity_count          = 0;
  z_handle_table[q_handle_id].session_aborted             = FALSE; 
  z_handle_table[q_handle_id].u_connectPending            = FALSE;
  z_handle_table[q_handle_id].z_ssl_handle                = NULL;
  z_handle_table[q_handle_id].u_use_laptop_call           = FALSE;

#ifdef FEATURE_GNSS_BIT_API
  z_handle_table[q_handle_id].session_handle              = GPSONE_BIT_INVALID_SESSION_HANDLE; 
  z_handle_table[q_handle_id].requested_read              = FALSE;
#else
  z_handle_table[q_handle_id].x_sockfd                    = DSS_ERROR;
  z_handle_table[q_handle_id].x_app_id                    = DSS_ERROR;
  z_handle_table[q_handle_id].x_force_dorm_app_id         = DSS_ERROR;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle = DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
#endif /* FEATURE_GNSS_BIT_API */

} /*pd_comms_tcp_task_release_handle() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_disconnect_handle

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
static void pd_comms_tcp_task_disconnect_handle(
  /*handle for the handle to be released.*/
  uint32 q_handle_id)
{
  PDCOMM_APP_MSG_MED("Disconnecting handle [%d]",q_handle_id,0,0);

  z_handle_table[q_handle_id].client_disconnect_recvd     = FALSE;
  z_handle_table[q_handle_id].l_inactivity_count          = 0;
  z_handle_table[q_handle_id].session_aborted             = FALSE; 
  z_handle_table[q_handle_id].p_read_ptr                  = z_handle_table[q_handle_id].p_read_buff;
  z_handle_table[q_handle_id].w_num_bytes_already_read    = 0;
  z_handle_table[q_handle_id].w_num_bytes_in_buffer       = 0;
  z_handle_table[q_handle_id].w_num_bytes_to_read         = 0;
  z_handle_table[q_handle_id].w_num_bytes_to_write        = 0;
  z_handle_table[q_handle_id].w_num_bytes_written         = 0;
  z_handle_table[q_handle_id].z_ssl_handle                = NULL;
  z_handle_table[q_handle_id].u_use_laptop_call           = FALSE;

#ifdef FEATURE_GNSS_BIT_API
  z_handle_table[q_handle_id].session_handle              = GPSONE_BIT_INVALID_SESSION_HANDLE;  
  z_handle_table[q_handle_id].requested_read              = FALSE;
  z_handle_table[q_handle_id].ssl_event_mask              = 0;
  z_handle_table[q_handle_id].ssl_events                  = 0;
  z_handle_table[q_handle_id].ssl_to_write_bytes          = 0;
  z_handle_table[q_handle_id].ssl_written_bytes           = 0;
#else
  z_handle_table[q_handle_id].x_sockfd                    = DSS_ERROR;
  z_handle_table[q_handle_id].x_app_id                    = DSS_ERROR;
  z_handle_table[q_handle_id].x_force_dorm_app_id         = DSS_ERROR;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle = DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
#endif /* FEATURE_GNSS_BIT_API */

} /*pd_comms_tcp_task_disconnect_handle() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_init_handle_table

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
static void pd_comms_tcp_task_init_handle_table( void )
{
  int l_i;

  for (l_i=0;l_i<PDCOMM_MAX_HANDLES;l_i++)
  {
    memset(&z_handle_table[l_i],0,sizeof(pd_comms_tcp_task_handle_data_type));

    z_handle_table[l_i].in_use                      = FALSE;
    z_handle_table[l_i].e_link_status               = PDCOMM_STATE_DISCONNECTED;
    z_handle_table[l_i].client_disconnect_recvd     = FALSE;
    z_handle_table[l_i].l_inactivity_count          = 0;
    z_handle_table[l_i].session_aborted             = FALSE;
    z_handle_table[l_i].p_read_ptr                  = z_handle_table[l_i].p_read_buff;
    z_handle_table[l_i].z_ssl_handle                = NULL;
    z_handle_table[l_i].u_use_laptop_call           = FALSE;

#ifdef FEATURE_GNSS_BIT_API
    z_handle_table[l_i].session_handle              = GPSONE_BIT_INVALID_SESSION_HANDLE;  
    z_handle_table[l_i].requested_read              = FALSE;
#else
    z_handle_table[l_i].x_sockfd                    = DSS_ERROR;
    z_handle_table[l_i].x_app_id                    = DSS_ERROR;
    z_handle_table[l_i].x_force_dorm_app_id         = DSS_ERROR;
    z_handle_table[l_i].z_dns_cb_data.dns_session_handle = DSS_DNS_SESSION_MGR_INVALID_HANDLE;
    z_handle_table[l_i].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
#endif /* FEATURE_GNSS_BIT_API */

  }

} /*pd_comms_tcp_task_init_handle_table() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_startup

DESCRIPTION
  Contains task initialization code.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_startup( void )
{
/*Call all the initialization code here*/
  boolean ret = os_IpcStart((uint32)THREAD_ID_SM_PDCOMM_TCP);

  if(ret == FALSE)
  {
    MSG_ERROR("PDCOMM APP: IPC Start Failed",0,0,0);
  }
  /*Since gethandle is blocking call, make it mutex protected*/
  ret = os_MutexInit(&z_gethandle_mutex, MUTEX_DATA_ONLY_CONTEXT); 

  if(ret == FALSE)
  {
    MSG_ERROR("PDCOMM APP: getHandle Mutex Init Failed",0,0,0);
  }
  /* Register for DOG heartbeat monitoring */
  os_DogHeartbeatRegister();

  p_inactivity_timer = os_TimerCreate((uint32)PDCOMM_APP_TASK_INACTIVITY_TIMER,(uint32)THREAD_ID_SM_PDCOMM_TCP);

  u_pd_comms_online = TRUE;

  /* Register GPS Diag with Diag services (now that it resides on Apps proc) */
  gpsdiag_init();  

} /*pd_comms_tcp_task_startup() ends*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_reset_inactivity_count

DESCRIPTION
  This function will reset inactivity timer for a particular connection to 0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  pd_comms_tcp_task_reset_inactivity_count(
  /*handle_id*/
  uint32 q_handle_id)
{
  uint32 q_timer_left=0;

  z_handle_table[q_handle_id].l_inactivity_count =0;
  q_timer_left = os_TimerCheck(p_inactivity_timer);
  z_handle_table[q_handle_id].l_inactivity_count -= 
                                      (int)(PDCOMM_INACTIVITY_TIMER_PERIOD-q_timer_left);

}/*pd_comms_tcp_task_reset_inactivity_count*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_connect

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
void  pd_comms_tcp_task_handle_connect( 
  /*Pointer to the received PDCOMM_CONNECT request message*/
  const pd_comms_ipc_connect_hdr_type* p_hdr_ptr,
  /*Are we handling a pending connect?*/
  boolean u_handle_pending_connect );


/*===========================================================================
FUNCTION pd_comms_tcp_task_update_state

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
void pd_comms_tcp_task_update_state(
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
  pd_comms_tcp_task_reset_inactivity_count(z_handle.q_handle_id);

  switch(z_new_state)
  {
    case PDCOMM_STATE_CONNECTED:
    {
      if(notify_client && z_old_state != PDCOMM_STATE_CONNECTED)
      {
        pd_comms_notification_params_type z_params;
        z_params.e_new_state = z_new_state;
        z_params.q_result = q_result;
        
        /*Notify the client about the state update*/
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
        
      }/*if(notify_client && z_old_state != PDCOMM_STATE_CONNECTED) ends*/
      break;
    } /*case PDCOMM_STATE_CONNECTED*/
    
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
            /*If no error, just send a STATE_CHANGE*/  
            e_result = pd_comms_notify_client(z_handle,
                                 z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                 z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                 z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                                 STATE_CHANGE,&z_params);  
          }
          else
          {
            /*If an error occured, send a ERROR_RPT along with the new state*/  
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
        }/*if(notify_client) ends*/
        
        /*re-initialize the handle data*/
        pd_comms_tcp_task_disconnect_handle(z_handle.q_handle_id);

        /*Start processing any pending connection*/
        if(z_handle_table[z_handle.q_handle_id].u_connectPending)
        {
          PDCOMM_APP_MSG_MED("Processing pending connection request for handle_id[%d]",
                             z_handle.q_handle_id,0,0);
          z_handle_table[z_handle.q_handle_id].u_connectPending = FALSE;
          pd_comms_tcp_task_handle_connect(&z_handle_table[z_handle.q_handle_id].z_pendingConnectReq,FALSE);
        }

      }/*if(z_old_state != PDCOMM_STATE_DISCONNECTED) ends*/
            
      break;
    }/*case PDCOMM_STATE_DISCONNECTED*/
    
    case PDCOMM_STATE_CONNECTING:
    {
      if(notify_client && z_old_state != PDCOMM_STATE_CONNECTING)
      {
        pd_comms_notification_params_type z_params;
        z_params.e_new_state = z_new_state;
        z_params.q_result = q_result;

        /*Notify the client about the state update*/ 
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
      }/*if(notify_client && z_old_state != PDCOMM_STATE_CONNECTING) ends*/
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
          /*If no error, just send a STATE_CHANGE*/    
          e_result = pd_comms_notify_client(z_handle,
                     z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                     z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                     z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                     STATE_CHANGE,&z_params);  
        }
        else
        {
          /*If an error occured, send a ERROR_RPT along with the new state*/    
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

      }/*if(notify_client && z_old_state != PDCOMM_STATE_DISCONNECTING) ends*/
      break;
    }/*case PDCOMM_STATE_DISCONNECTING*/
    
    default:
    {
      MSG_ERROR( "Invalid State %d recevied for update", z_new_state, 0, 0);
    }
  }/* switch(z_new_state) ends */

}/* pd_comms_tcp_task_update_state() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_http_connect

DESCRIPTION
  This function handles a HTTP connect request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_http_connect(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
   return;

} /*lint !e715 */ /*pd_comms_tcp_task_http_connect*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_get_handle

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
pd_comms_return_type  pd_comms_tcp_task_handle_get_handle( 
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
}/*pd_comms_tcp_task_handle_get_handle*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_connect

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
void  pd_comms_tcp_task_handle_connect(
  /*Pointer to the received PDCOMM_CONNECT request message*/
  const pd_comms_ipc_connect_hdr_type* p_hdr_ptr,
  /*Are we handling a pending connect?*/
  boolean u_handle_pending_connect )
{
  uint32 l_i;
  pd_comms_handle_type z_handle;
  
  if(p_hdr_ptr ==  NULL)
  {
    return;
  }
  
  z_handle = p_hdr_ptr->z_handle;
  l_i = p_hdr_ptr->z_handle.q_handle_id;

  PDCOMM_APP_MSG_MED("Connect request received for handle_id [%d]",l_i,0,0);
  
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

    /*MR: will have to teardown PPP connection if handling a pending connect and DISCONNECTING*/

    return;
  }/*if(l_i >= PDCOMM_MAX_HANDLES || z_handle_table[l_i].in_use == FALSE)*/
  
  if((z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTED) ||
     ((z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTING) && u_handle_pending_connect))
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
      case LINK_UDP:
      {
        if(p_hdr_ptr->z_params.e_use_security == SECURITY_NONE)
        {
          (void)pd_comms_tcp_task_tcp_connect(z_handle); 
        }
        else
        {
          pd_comms_tcp_task_secure_tcp_connect(z_handle);
        }
        break;
      }
      
      case LINK_SMS:
      {
        PDCOMM_APP_MSG_ERROR("SMS connect request received by TCP task",0,0,0);
        break;
      }
      
      case LINK_HTTP:
      {
        pd_comms_tcp_task_http_connect(z_handle);
        break;
      }
      
      default:
      {
        PDCOMM_APP_MSG_ERROR(" Received INVALID link_type %d in connect ",
                               p_hdr_ptr->z_params.e_link_type,0,0);
      }
    }/*switch(p_hdr_ptr->z_params.e_link_type) ends*/
  }/*if(z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTED) ends*/
  else if( (z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTING) &&
           !z_handle_table[l_i].u_connectPending)/*allow only one pending connection*/
  {
    z_handle_table[l_i].u_connectPending = TRUE;
    
    /*Save the connect request to process later when the current connection is 
      completely disconnected*/
    memscpy(&z_handle_table[l_i].z_pendingConnectReq,
            sizeof(z_handle_table[l_i].z_pendingConnectReq),
            p_hdr_ptr, sizeof(*p_hdr_ptr)); 
  }
  else  /*Refuse the new connect request if previous connection is still in CONNECTED/CONNECTING state
          or a connection is already pending*/
  {
    pd_comms_notification_params_type z_params;

    z_params.e_new_state = z_handle_table[l_i].e_link_status;
    z_params.q_result = (uint32)PDCOMM_CONNECT_ERROR_PREVIOUS_CONNECTION_ACTIVE;
                                   
    if(!z_handle_table[l_i].u_connectPending)                                   
    {
      PDCOMM_APP_MSG_HIGH("Connection already in CONNECTED/CONNECTING state for handle_id[%d]",
                           l_i,0,0);
    }
    else
    {
      PDCOMM_APP_MSG_HIGH("Previous connection PENDING for handle_id[%d]",
                           l_i,0,0);
    }
    
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->z_params.q_transaction_id,
                           ERROR_RPT,&z_params);   
  }


} /*pd_comms_tcp_task_handle_connect() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_http_disconnect

DESCRIPTION
  This function handles a HTTP link disconnect request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_http_disconnect(
  /*handle corresponding to the connection for which connection is to be disconnected*/
  pd_comms_handle_type z_handle)
{
  return ;
} /*lint !e715 */ /*pd_comms_tcp_task_http_disconnect*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_disconnect

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
static void  pd_comms_tcp_task_handle_disconnect( 
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

  PDCOMM_APP_MSG_MED("Disconnect request received for handle_id [%d]",z_handle.q_handle_id,0,0);
 
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
      case LINK_UDP:
      {
        if(z_handle_table[z_handle.q_handle_id].z_connect_params.e_use_security ==
             SECURITY_NONE)
        {
          pd_comms_tcp_task_tcp_disconnect(z_handle, p_hdr_ptr->abort,TRUE); 
        }
        else
        {
          pd_comms_tcp_task_secure_tcp_disconnect(z_handle, p_hdr_ptr->abort,TRUE); 
        }
        break;
      }
      
      case LINK_SMS:
      {
        PDCOMM_APP_MSG_ERROR("SMS disconnect request received by TCP task",0,0,0);
        break;
      }
      
      case LINK_HTTP:
      {
        pd_comms_tcp_task_http_disconnect(z_handle);
        break;
      }
      
      default:
      {
        PDCOMM_APP_MSG_HIGH("PDCOMM APP: Link already disconnected for handle id [%d]",
                  z_handle.q_handle_id,0,0);
        break;

      }

    }
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

}/* pd_comms_tcp_task_handle_disconnect() ends*/




/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_bind

DESCRIPTION
  This function handles an incoming PDCOMM_BIND request. It will call the
  the bind function specific to the link_type(TCP,HTTP,SMS etc.)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_tcp_task_handle_bind( 
  /*Pointer to the incoming PDCOMM_DISCONNECT request IPC message*/
  os_IpcMsgType* p_msg )
{
  uint32 l_i;
  pd_comms_handle_type z_handle;  
  pd_comms_ipc_bind_hdr_type* p_hdr_ptr; 
  
  if(p_msg ==  NULL)
  {
    return;
  }

  p_hdr_ptr = (pd_comms_ipc_bind_hdr_type*)p_msg->p_Data;
  z_handle = p_hdr_ptr->z_handle;
  l_i = p_hdr_ptr->z_handle.q_handle_id;

  PDCOMM_APP_MSG_MED("Connect request received for handle_id [%d]",l_i,0,0);
  
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
  }/*if(l_i >= PDCOMM_MAX_HANDLES || z_handle_table[l_i].in_use == FALSE)*/
  
  if((z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTED) || 
     (z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTING))
  {
    boolean result = FAILED;
    /*Save all the general data for handle in handle_table*/
    z_handle_table[l_i].z_connect_params.e_link_type = p_hdr_ptr->z_params.e_link_type;
    z_handle_table[l_i].z_connect_params.q_transaction_id = p_hdr_ptr->z_params.q_transaction_id;
    z_handle_table[l_i].z_connect_params.z_link_params.z_tcp_params.w_ip_port = p_hdr_ptr->z_params.z_link_params.z_tcp_params.w_ip_port;
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
      case LINK_UDP:
      {
          result = pd_comms_tcp_task_udp_bind(z_handle); 
        break;
      }
      case LINK_SMS:
      {
        PDCOMM_APP_MSG_ERROR("SMS connect request received by TCP task",0,0,0);
        break;
      }      
      default:
      {
        PDCOMM_APP_MSG_ERROR(" Received INVALID link_type %d in connect ",
                               p_hdr_ptr->z_params.e_link_type,0,0);
      }
    }/*switch(p_hdr_ptr->z_params.e_link_type) ends*/

    if(result == FAILED)
    {    
      pd_comms_notification_params_type z_params;
      
      z_params.e_new_state = z_handle_table[l_i].e_link_status;
      z_params.q_result = (uint32)PDCOMM_CONNECT_ERROR_SOCKET_CONNECTION_FAILED;
                                     
      PDCOMM_APP_MSG_HIGH("UDP Socket bind failed for handle_id[%d]",
                             l_i,0,0);
      
      (void)pd_comms_notify_client(z_handle,
                             p_hdr_ptr->q_source_thread_id,
                             p_hdr_ptr->q_base_msg_id,
                             p_hdr_ptr->z_params.q_transaction_id,
                             ERROR_RPT,&z_params);
    }
  }
  else  /*Refuse the new bind request if previous connection is still in CONNECTED/CONNECTING state*/
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

}


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_release_handle

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
pd_comms_return_type  pd_comms_tcp_task_handle_release_handle( 
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

  PDCOMM_APP_MSG_MED("Release handle request received for handle_id[%d]",q_handle_id,0,0);
  
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
    pd_comms_tcp_task_release_handle(q_handle_id);  
  }
  else
  {
    /*if the link is not already disconnected, abort the connection before releasing 
      the handle*/  
    switch(z_handle_table[q_handle_id].z_connect_params.e_link_type)
    {
      case LINK_TCP:
      case LINK_UDP:
      {
        if(z_handle_table[q_handle_id].z_connect_params.e_use_security ==
           SECURITY_NONE)
        {
          /*Abort the connection but do not notify client*/     
          pd_comms_tcp_task_tcp_disconnect(*p_handle, TRUE,FALSE); 
        }
        else
        {
          /*Abort the connection but do not notify client*/   
          pd_comms_tcp_task_secure_tcp_disconnect(*p_handle, TRUE,FALSE); 
        }
        break;
      }
      case LINK_SMS:
      {
        PDCOMM_APP_MSG_ERROR("SMS release handle request received by TCP task",0,0,0);
        break;
      }
      case LINK_HTTP:
      {
        pd_comms_tcp_task_http_disconnect(*p_handle);
        break;
      }
      default:
      {
        PDCOMM_APP_MSG_HIGH("PDCOMM APP: Link already disconnected for handle id [%d]",
                            q_handle_id,0,0);

      }

    }/*end switch */

    /*release the handle*/
    pd_comms_tcp_task_release_handle(q_handle_id);  
  }/*else for if(z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_DISCONNECTED)*/
  
  os_MutexUnlock(&z_gethandle_mutex);
  MSG_MED("PDCOMM APP:GetHandle Mutex unlocked",0,0,0); 
  return PDCOMM_RESULT_OK;
} /*pd_comms_tcp_task_handle_release_handle*/



/*===========================================================================
FUNCTION pd_comms_tcp_task_http_write

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
static boolean pd_comms_tcp_task_http_write(
  /*handle corresponding to the connection for which data is to be written*/
  pd_comms_handle_type z_handle)
{
 return FALSE;
} /*lint !e715 */ /*pd_comms_tcp_task_http_write*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_write

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
static void  pd_comms_tcp_task_handle_write(
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

  PDCOMM_APP_MSG_MED("Write request received for handle_id [%d] (%d bytes)",
                     z_handle.q_handle_id,p_hdr_ptr->w_data_len,0);
  
  if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )
  {
    pd_comms_tcp_task_reset_inactivity_count(z_handle.q_handle_id);

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

    if(p_hdr_ptr->w_data_len > PDCOMM_WRITE_DATA_BUF_SIZE)
    {
      PDCOMM_APP_MSG_ERROR("Requested Write data length[%d] greater than PDCOMM buffer size [%d]",
                            p_hdr_ptr->w_data_len,PDCOMM_WRITE_DATA_BUF_SIZE,0); 
    }
	
    p_hdr_ptr->w_data_len = memscpy(z_handle_table[z_handle.q_handle_id].p_write_buff,
		PDCOMM_WRITE_DATA_BUF_SIZE, p_hdr_ptr->p_data, p_hdr_ptr->w_data_len);

    z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = p_hdr_ptr->w_data_len;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_written = 0;

    switch(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type)
    {
      case LINK_TCP:
      case LINK_UDP:
      {
        if(z_handle_table[z_handle.q_handle_id].z_connect_params.e_use_security ==
           SECURITY_NONE)
        {
          (void)pd_comms_tcp_task_tcp_write(z_handle); 
        }
        else
        {
          (void)pd_comms_tcp_task_secure_tcp_write(z_handle); 
        }
        break;
      }           
      
      case LINK_SMS:
      {
        PDCOMM_APP_MSG_ERROR("SMS write request received by TCP task",0,0,0);
        break;
      }
      
      case LINK_HTTP:
      {
        (void)pd_comms_tcp_task_http_write(z_handle);
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
} /*pd_comms_tcp_task_handle_write() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_read

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
static void  pd_comms_tcp_task_handle_read(
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

  PDCOMM_APP_MSG_MED("Read request received for handle_id [%d] (%d bytes)",
                     z_handle.q_handle_id,p_hdr_ptr->p_params.w_data_len,0);
  
  if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )
  {
    pd_comms_tcp_task_reset_inactivity_count(z_handle.q_handle_id);

    if(z_handle_table[z_handle.q_handle_id].w_num_bytes_already_read < 
       z_handle_table[z_handle.q_handle_id].w_num_bytes_to_read)
    {

      MSG_ERROR("Read called while previous read still pending for handle_id [%d] read %d toread %d",
                z_handle.q_handle_id,
                z_handle_table[z_handle.q_handle_id].w_num_bytes_already_read,
                z_handle_table[z_handle.q_handle_id].w_num_bytes_to_read);

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
#ifndef FEATURE_CGPS_LBS_TASK
    z_handle_table[z_handle.q_handle_id].w_num_bytes_already_read = 0;
#endif    
    switch(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type)
    {
      case LINK_TCP:
      case LINK_UDP:
      {
        if(z_handle_table[z_handle.q_handle_id].z_connect_params.e_use_security ==
           SECURITY_NONE)
        {
          (void)pd_comms_tcp_task_tcp_read(z_handle.q_handle_id,p_hdr_ptr->p_params.u_wait_for_all); 
        }
        else
        {
          (void)pd_comms_tcp_task_secure_tcp_read(z_handle.q_handle_id,p_hdr_ptr->p_params.u_wait_for_all); 
        }
        break;
      }
      
      case LINK_SMS:
      {
        PDCOMM_APP_MSG_ERROR("SMS read request received by TCP task",0,0,0);
        break;
      }
      
      case LINK_HTTP:
      {
        /*pd_comms_tcp_task_http_read(z_handle);*/
        break;
      }
      
      default:
      {
          MSG_ERROR("PDCOMM APP: Invalid link type [%d] received in read request for handle id [%d]",
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
} /*pd_comms_tcp_task_handle_read() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_hash

DESCRIPTION
  This function handles an incoming PDCOMM_HASH request IPC message.

DEPENDENCIES
  None

RETURN VALUE
 None 

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_tcp_task_handle_hash( 
  /*pointer to the PDCOMM_HASH IPC message*/
  os_IpcMsgType* p_msg )
{
  uint8 p_hmac_buff[20]; /* SHA-1 outputs a 160 bit hash */
  secerrno_enum_type e_error;
  pd_comms_return_type e_result;
  secapi_hsh_param_data_type z_param_data;
  uint8* p_key;/*lint -e826 */   
  pd_comms_ipc_hash_req_hdr_type* p_hdr_ptr; 

  if(p_msg == NULL)
  {
    return;
  }

  PDCOMM_APP_MSG_MED("Hash request received",0,0,0); 
  
  p_hdr_ptr  = (pd_comms_ipc_hash_req_hdr_type*)p_msg->p_Data;
  /*lint +e826 */   
  if((p_hdr_ptr->e_hash_type != SHA1) || (p_hdr_ptr->q_msg_len > 65535))
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

  p_key = (uint8*) &p_hdr_ptr->p_data[p_hdr_ptr->q_msg_len];

  /* Process payload atomically */
  z_param_data.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  (void)secapi_hsh_set_param( z_hash_handle, SECAPI_SHA, SECAPI_HSH_INPUT_MODE, &z_param_data );

  /* Block until hash is completed */
  z_param_data.common.exec_mode = SECAPI_SYNC_EXEC;
  (void)secapi_hsh_set_param( z_hash_handle, SECAPI_SHA, SECAPI_EXEC_MODE, &z_param_data );

  /* Submit SUPL INIT to HMAC generator using the FQDN as the key */
  e_error = secapi_hsh_create_hmac( z_hash_handle, SECAPI_SHA, (uint8*)p_hdr_ptr->p_data, 
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

    (void)pd_comms_send_hash_resp_to_client(p_hdr_ptr->q_source_thread_id,p_hdr_ptr->q_base_msg_id,
                                      &z_hash_resp_params);

  }
} /*pd_comms_tcp_task_handle_hash*/



/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_ioctl

DESCRIPTION
  This function handles an incoming PDCOMM_IOCTL request IPC message.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void  pd_comms_tcp_task_handle_ioctl( 
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

  PDCOMM_APP_MSG_MED("Ioctl command [%d] received for handle_id[%d]",
                     p_hdr_ptr->q_ioctl_request,
                     z_handle.q_handle_id,0); 
  
  if( z_handle.q_handle_id < PDCOMM_MAX_HANDLES )
  {
    z_handle_table[z_handle.q_handle_id].q_ioctl_transaction_id = p_hdr_ptr->q_transaction_id;
      
    switch(p_hdr_ptr->q_ioctl_request)
    {
      case PDCOMM_IOCTL_CMD_FORCE_DORMANCY:
      {
        pd_comms_tcp_task_handle_ioctl_force_dormancy(p_hdr_ptr);  
        break;
      }
      case PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY:
      {
        pd_comms_tcp_task_handle_ioctl_unforce_dormancy(p_hdr_ptr); 
        break;
      }

      case PDCOMM_IOCTL_CMD_SET_SSL_CERT:
      {
        pd_comms_tcp_task_handle_ioctl_set_ssl_cert(p_hdr_ptr);  
        break;
      }

      case PDCOMM_IOCTL_CMD_GET_IP_ADDR:
      {
        pd_comms_tcp_task_handle_ioctl_get_ip_addr(p_hdr_ptr);  
        break;
      }

      case PDCOMM_IOCTL_CMD_SET_SSL_CERT_DIR:
      {
        pd_comms_tcp_task_handle_ioctl_set_ssl_cert_dir(p_hdr_ptr);  
        break;
      }
      
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
    }/*switch(p_hdr_ptr->q_ioctl_request)*/
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
  
}/*pd_comms_tcp_task_handle_ioctl*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_inactivity_timer

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
static void  pd_comms_tcp_task_handle_inactivity_timer(void)
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

        if(z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTED)
        {
          // connection already in DISCONNECTED state, no need to print out error message
          /* MSG_ERROR("PDCOMM APP: Client already notified about aborted connection. Release Handle [%d]", l_i,0,0); */
        }
        else
        {
          MSG_ERROR("PDCOMM APP: Handle [%d] inactive for more than %d millisec. Aborting connection..",
                    l_i,PDCOMM_INACTIVITY_TIMEOUT,0); // idle for 40 seconds while in CONNECTED state

          if((z_handle_table[l_i].z_connect_params.e_link_type == LINK_TCP) ||
             (z_handle_table[l_i].z_connect_params.e_link_type == LINK_UDP))
          {
            /*abort the connection*/
            /*pdcomm_app_task_tcp_disconnect(z_handle_table[l_i].z_handle, TRUE);*/
            if(z_handle_table[l_i].z_connect_params.e_use_security == SECURITY_NONE)
            {
              pd_comms_tcp_task_abort_connection( l_i ); 

              pd_comms_tcp_task_update_state(z_handle_table[l_i].z_handle,PDCOMM_STATE_DISCONNECTED,
                                             TRUE,(uint32) PDCOMM_RESULT_OK);
            }
            else
            {
              pd_comms_tcp_task_secure_tcp_disconnect(z_handle_table[l_i].z_handle,TRUE,TRUE); 
            }
          }
          else if(z_handle_table[l_i].z_connect_params.e_link_type == LINK_HTTP)
          {
            pd_comms_tcp_task_http_disconnect(z_handle_table[l_i].z_handle);
          }
          else if(z_handle_table[l_i].z_connect_params.e_link_type == LINK_SMS)
          {
            PDCOMM_APP_MSG_ERROR("SMS link type in TCP task",0,0,0);
          }

        }
        
      }
    }

  }/*for loop ends*/

  /* Re-arm the inactivity timer. */
  (void)os_TimerStart(p_inactivity_timer,PDCOMM_INACTIVITY_TIMER_PERIOD,0);

}/*pd_comms_tcp_task_handle_inactivity_timer*/


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
static void pd_comms_tcp_task_handle_stop( void )
{
  PDCOMM_APP_MSG_MED("STOP request received",0,0,0); 

  /* Deregister with the Dog Heartbeat before stopping task */
  os_DogHeartbeatDeregister();

  os_TaskStopAck();
} /*pd_comms_tcp_task_handle_stop*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_offline

DESCRIPTION
  This function is used to handle TASK_OFFLINE IPC

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_offline( void )
{
  u_pd_comms_online = FALSE;
  PDCOMM_APP_MSG_MED("OFFLINE request received",0,0,0);

}/*pd_comms_tcp_task_handle_offline*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_proc

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
static void pd_comms_tcp_task_proc(
  /*Pointer to received IPC message */
  os_IpcMsgType* p_msg )
{
  /*pd_comms_tcp_task calls this function whenever it recieves a message*/

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
        pd_comms_tcp_task_handle_connect((pd_comms_ipc_connect_hdr_type*)p_msg->p_Data,FALSE);
        /*lint +e826 */ 
        break;
      }
      
      case C_PD_COMMS_APP_MSG_DISCONNECT:
      {
        pd_comms_tcp_task_handle_disconnect(p_msg);
        break;
      }

      case C_PD_COMMS_APP_MSG_BIND:
      {
        pd_comms_tcp_task_handle_bind(p_msg);
        break;
      }

      case C_PD_COMMS_APP_MSG_WRITE:
      {
        pd_comms_tcp_task_handle_write(p_msg);
        break;
      }
      
      case C_PD_COMMS_APP_MSG_READ:
      {
        pd_comms_tcp_task_handle_read(p_msg);
        break;
      }
            
      case C_PD_COMMS_APP_MSG_HASH_REQ:
      {
        pd_comms_tcp_task_handle_hash(p_msg);
        break;
      }
      
      case C_PD_COMMS_APP_MSG_IOCTL:
      {
        pd_comms_tcp_task_handle_ioctl(p_msg);
        break;
      }

      case C_PD_COMMS_APP_SRV_SYS_CHANGE_NOTIFICATION:
      {
#ifdef FEATURE_GNSS_BIT_API
        // ignore this message if BIT is enabled, doesn't need to handle
#else
        // TODO, enable this when BIT & non-BIT PDCOMM is merged to common.c
        // pd_comms_tcp_handle_srv_system_change_notification(p_msg);
#endif
      }
      break;

#ifdef FEATURE_GNSS_BIT_API
      case PDCOMM_APP_TASK_BIT_EVENT_CB:
      {
        pd_comms_tcp_task_handle_bit_event_cb(p_msg);
        break;
      }

      // SSL callbacks
      case PDCOMM_APP_TASK_SEC_EVENT_CB:
      {
        pd_comms_tcp_task_handle_sec_event_cb(p_msg);
        break;
      }

      case PDCOMM_APP_TASK_SEC_OPEN_CB:
      {
        pd_comms_tcp_task_handle_sec_open_cb(p_msg);
        break;
      }

      case PDCOMM_APP_TASK_SEC_CLOSE_CB:
      {
        pd_comms_tcp_task_handle_sec_close_cb(p_msg);
        break;
      }

      case PDCOMM_APP_TASK_SEC_CLOSE_NET_CB:
      {
        pd_comms_tcp_task_handle_sec_close_net_cb(p_msg);
        break;
      }    
#else

      /*  // legacy callbacks from DSS/ATL, no longer supported
      case PDCOMM_APP_TASK_DSS_NETWORK_CB:
      {
        pd_comms_tcp_task_handle_net_cb(p_msg);
        break;
      }
      
      case PDCOMM_APP_TASK_ATL_DNS_CB:
      {
        pd_comms_tcp_task_handle_atl_dns_cb(p_msg);
        break;
      }

      case PDCOMM_APP_TASK_DSS_DNS_CB:
      {
        pd_comms_tcp_task_handle_dss_dns_cb(p_msg);
        break;
      }

      case PDCOMM_APP_TASK_DSS_SOCKET_CB:
      {
        pd_comms_tcp_task_handle_sock_cb(p_msg);
        break;
      }
      
      case PDCOMM_APP_TASK_DSS_PHY_LINK_DOWN_CB:
      {
        pd_comms_tcp_task_handle_phy_link_down_cb(p_msg);
        break;
      }
      */

#endif /* FEATURE_GNSS_BIT_API */ 
              
      case C_OS_MSG_ID_TIMER_EXPIRY:
      {       
        /*lint -e826 */               
        if( ((os_TimerExpiryType*)p_msg->p_Data)->q_TimerId == (uint32)PDCOMM_APP_TASK_INACTIVITY_TIMER )
        {
          pd_comms_tcp_task_handle_inactivity_timer();
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
        pd_comms_tcp_task_handle_offline();
        break;
      }
      
      case C_OS_MSG_ID_STOP: 
      {
        pd_comms_tcp_task_handle_stop();
        break;
      }

#ifdef FEATURE_GNSS_BIT_API  
      case C_OS_MSG_ID_QMI_EVENT:        
        qmiBitHandleEvent();
        break;
#endif
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
  
}/*pd_comms_tcp_task_proc() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task

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
void pd_comms_tcp_task(
  /*task input parameters(currently not used)*/
  dword q_dummy )
{
/*The entry point for the task and the infinite loop
  are present in this function */

  os_IpcMsgType* p_msg;

  pd_comms_tcp_task_startup();

  pd_comms_tcp_task_init_handle_table();

  os_TaskReadyAck();
  os_TaskStopRegister();
  
  pd_comms_tcp_task_register_callbacks();

#ifdef FEATURE_GNSS_BIT_API
  if ( FALSE ==  qmiBitInit() )
  {
    ASSERT(0);
  }
#endif


  
  /* Create a new Hash function crypto instance */
  /*if( secapi_new( &z_hash_handle, SECAPI_SHA ) != E_SUCCESS )
  {  
    MSG_ERROR("PDCOMM APP: Could not create a hash function instance",0,0,0);
  } */

  /* Start the inactivity timer. 5s timer, 40s timeout */
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
      pd_comms_tcp_task_proc(p_msg);
      (void)os_IpcDelete(p_msg);
    }

  }/*for(;;) ends*/

} /*pd_comms_tcp_task() ends*/ /*lint !e715 */

#endif /* FEATURE_CGPS_PDCOMM */
