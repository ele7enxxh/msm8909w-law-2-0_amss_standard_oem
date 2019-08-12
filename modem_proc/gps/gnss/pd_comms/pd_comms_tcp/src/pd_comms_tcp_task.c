/*======================================================================

               PDCOMM Application task implementation file

 GENERAL DESCRIPTION
  This file contains the implementation of PDCOMM Application task.
  This task handles requests from various clients like TM, calls 
  appropriate data-service specific functions and sends back results to
  the client.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
 
  Copyright (c) 2011-2012, 2015-2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
 
 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_tcp/src/pd_comms_tcp_task.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

 when       who  what, where, why
 ----       ---- --- ---------------------------------------------------
 09/21/16   skm  forward WRITE event to SSL when in CONNECTING state and SSL Auth is in process
 06/01/16   skm  secssl_abort_open_session if TCP connection reset during SSL handshake/session.
 11/18/15   skm  PDCOMMS release handle crash fix 
 06/06/15   skm  SUPL call flow without ESLP in MTES causes a loop retrying SLP if configured in NV.
 05/26/15   skm  PD COMM fails to free DS List iterator object in Emergency profile look up.
 04/28/15   rh   Added MCFG refresh code to support Hot SIM swap
 11/19/14   skm  code review comment to move repeated code into local helper functions CR755132
 09/12/14   skm  Set SECSSL_SEC_CS_LIST explictly to avoid pickup up DH_ANON as part of Default Cipher Suite.
 08/26/14   skm  Remove Reserved port for TCP connections.
 07/19/14   skm  Fix for UDP socket closing on READ ERROR.
 07/08/14   sm   Changed LINGET time from 2secs to 50 MSecs on customer request.
 04/11/14   skm  Removed iface name mapping on ATL OPEN call's return
 01/23/14   skm  Support for UDP EPC_GROUP and removing Interface UP/DOWN event listeners
 12/13/13   sm   Remove default-listeners and add Ipv4v6 preference selection
 07/30/13   ssh  Passing correct iface family in case of Ipv6 connections
 06/11/13   ssh  Added Support for UDP to listen on all interfaces
05/14/13    ssh   send state change notification along with ERR RPT
 02/08/13   ssu  Move to the DOG Heartbeat mechanism on DIME and beyond.
 02/08/11   rc   removing MACRO definitions which conflict with Win32 Socket implementations
 01/05/11   gk   clean up ppp when abort_connection is called.
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
#ifndef __GRIFFON__

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_CGPS_PDCOMM 
#include "task.h"
#include "fs_public.h"
#include "dssocket.h"
#include "dssdns.h"
#include "dssinternal.h"
#include "dss_netpolicy.h"
#include "msg.h"
#include "secapi.h"
#include "secssl.h"
#include "secssldef.h"
#include "dss_iface_ioctl.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "pd_comms_api.h"
#include "pd_comms_client_api.h"
#include "pd_comms_app_api.h"
#include "pd_comms_tcp_local_api.h"
//#define extern
#include "pd_comms_tcp_data.h"
//#undef extern
#include "pd_comms_atl_i.h"
#include "sys.h"

#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_3gpp2.h"
#include "ps_iface.h"
#include "gps_common.h"
#include "event.h"
#include "sm_log.h"

#define PDCOMM_MIN_DH_KEY_SIZE                (64) 
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#define NUM_RX_FLTRS 1
#define NUM_ACTIVE_PDP_TYPES 2

#define LBS_APP_TYPE                    0x00000000 /* This value is specified in 
                                                     RUIM spec 80-W1429-1 */
#define PDCOMM_APN_PROFILE_SRV_SYS_CDMA_INDEX  0
#define PDCOMM_APN_PROFILE_SRV_SYS_HDR_INDEX   1
#define PDCOMM_APN_PROFILE_SRV_SYS_GSM_INDEX   2
#define PDCOMM_APN_PROFILE_SRV_SYS_WCDMA_INDEX 3
#define PDCOMM_APN_PROFILE_SRV_SYS_LTE_INDEX   4
#define PDCOMM_APN_PROFILE_SRV_SYS_TDS_INDEX   5
#define PDCOMM_APN_PROFILE_SRV_SYS_MAX_INDEX   6

/*Number of miliseconds the resolver will wait for an answer, from the moment the
  query was sent until a retry will be performed.*/
#define DNS_DEFAULT_FIRST_QUERY_TIMEOUT 3000
/*Number of miliseconds added to thewaiting time between two consecutive retries*/
#define DNS_DEFAULT_RETRIES_BACKOFF       0
/*Number of total retries.*/
#define DNS_DEFAULT_MAX_QUERY_RETRIES   3

/*Default socket linger time*/
#define PDCOMM_DEFAULT_SOCKET_LINGER_TIME_MSEC 50

/* 
 SSL session Parameter SECSSL_SEC_CS_LIST needs to be explicitly set to
 1) Avoid advertising Obsolete Cipher Suite in SSL connection
 2) Avoid using DH-ANON set of Cipher Suites which are vulnerable to Man in the
    middle attacks
IMP: The list of supported Cipher Suites given below are defined in "secssldef.h" 
 If new Cipher Suites are supported later on, the list should be updated. 
*/
#define PD_COMMS_SECSSL_CIPHER_LIST_SIZE_MAX   2

#define PD_COMMS_SECSSL_CIPHER_LIST  \
{                                    \
  TLS_RSA_WITH_AES_128_CBC_SHA     , \
  TLS_DHE_RSA_WITH_AES_128_CBC_SHA   \
}
/* Add new Cipher suites above.
** Update PD_COMMS_SECSSL_CIPHER_LIST_SIZE_MAX*/
#define PD_COMM_DHS_MIN_SIZE (64)
static uint16 pd_comm_sec_ssl_cs_list[PD_COMMS_SECSSL_CIPHER_LIST_SIZE_MAX] = PD_COMMS_SECSSL_CIPHER_LIST;

/****************** Variables ************************************************/
static pd_comms_tcp_task_handle_data_type z_handle_table[PDCOMM_MAX_HANDLES];

static os_TimerMsgType* p_inactivity_timer;
static secapi_handle_type z_hash_handle;
static os_MutexBlockType z_gethandle_mutex;
static boolean u_pd_comms_online;
static sys_sys_mode_e_type srv_sys_mode = SYS_SYS_MODE_NO_SRV;
static boolean u_pd_comms_data_enabled = TRUE;
/*
 *  ATL State
 */
static pdsm_atl_open_params_type z_atl_open_params;

/*Apn profile NV item*/
pdsm_pa_apn_profiles_type nv_apn_profiles[PDSM_PA_MAX_APN_PROFILES];

/*SSL certificate key length NV item*/
uint16 nv_ssl_certificate_key_length =CGPS_NV_EFS_PDCOMM_SSL_RSA_CERTIFICATE_KEY_LENGTH_DEFAULT;

typedef struct
{
  char apn_name[PDSM_PA_MAX_APN_NAME_LENGTH];
  uint32 pdp_type;
} apn_profiles_type;

/*APN Profile Table*/
static apn_profiles_type pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_MAX_INDEX][PDCOMM_APN_PROFILE_TYPE_MAX]; 

/****************** Function Definitions**************************************/

/* Initialize the GPS Diag Module */
extern void gpsdiag_init (void);
static void pd_comms_tcp_task_setsockopt(uint32 q_handle_id);
static void pd_comms_tcp_task_read_nvs(void);
#ifdef FEATURE_MODEM_CONFIG_REFRESH
static void pd_comms_tcp_task_register_mcfg_refresh(void);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

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

  memset((void*)&z_atl_open_params, 0, sizeof(pdsm_atl_open_params_type));
  z_atl_open_params.iface_name = PDSM_ATL_IFACE_NONE;
  

  /* Register GPS Diag with Diag services (now that it resides on Apps proc) */
  gpsdiag_init();

  /*Initialize pdcomm nv EFS*/  
  pd_comms_nv_efs_init();

  /*Read the NV items*/
  pd_comms_tcp_task_read_nvs();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  pd_comms_tcp_task_register_mcfg_refresh();
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

} /*pd_comms_tcp_task_startup() ends*/

/*Forward declaration*/
/*===========================================================================
FUNCTION pd_comms_tcp_task_dft_atl_open

DESCRIPTION
 This function handles a TCP/IP connect request.
  It attempts to do the following operations if they return immediately:
  1. open netlib
  2. open PPP
  3. open socket

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_tcp_task_dft_atl_open
(
  pd_comms_handle_type           z_handle,
  pd_comms_atl_open_params_type* z_open_params 
); 

/*===========================================================================
FUNCTION pd_comms_tcp_task_dft_atl_close

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_tcp_task_dft_atl_close
(
  pd_comms_handle_type       z_handle,
  boolean                    u_e911_close    
);

/*===========================================================================
FUNCTION pd_comms_tcp_task_dft_atl_dns_lookup

DESCRIPTION
  This function initates a DNS lookup. It is used by default when no ATL
  service has been registered.

DEPENDENCIES
  None

RETURN VALUE
  operation_status_type - status of the operation (FAILED, SUCCEDDED or 
  PENDING)

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type 
pd_comms_tcp_task_dft_atl_dns_lookup(
  /*handle_id corresponding to the connection for which DNS lookup is to be performed*/  
  uint32 q_handle_id,
  /*(pdcomm specific error is returned through this pointer*/
  pd_comms_connect_result_type* p_conn_error);

/*============================================================================
FUNCTION pd_comm_tcp_task_process_dns_lookup_results

DESCRIPTION
  A common function to process the results produced by both data service and 
  ATL-based lookup functions.
=============================================================================*/
static void pd_comm_tcp_task_process_dns_lookup_results(
  /*handle_id corresponding to the connection for which DNS lookup has been performed*/  
  uint32 q_handle_id, 
  pd_comms_tcp_task_operation_status_type e_status, 
  pd_comms_connect_result_type e_conn_error);

/*===========================================================================
FUNCTION pd_comms_tcp_get_sys_mode

DESCRIPTION
  This function returns the present system mode.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/

sys_sys_mode_e_type pd_comms_tcp_get_sys_mode(void);


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
static void pd_comms_tcp_task_register_callbacks(void)
{
  pd_comms_register_service_type z_reg_params;
  boolean u_result;

  /*Register TCP transport functions*/
  z_reg_params.e_link_type = LINK_TCP;
  z_reg_params.q_function_mask = (uint32)PD_COMMS_FUNC_CONNECT | (uint32)PD_COMMS_FUNC_DISCONNECT |
                                 (uint32)PD_COMMS_FUNC_WRITE |(uint32)PD_COMMS_FUNC_IOCTL | 
                                 (uint32)PD_COMMS_FUNC_GET_HANDLE |(uint32)PD_COMMS_FUNC_READ |
                                 (uint32)PD_COMMS_FUNC_RELEASE_HANDLE | (uint32) PD_COMMS_FUNC_SRV_SYSTEM_NOTIFICATION |
                                 (uint32)PD_COMMS_FUNC_SET_DATA_ENABLE |(uint32)PD_COMMS_FUNC_APN_PROFILE_CHANGE_NOTIFICATION;

  u_result = pd_comms_register_service(&z_reg_params,pd_comms_client_tcp_get_handle_cb,
                                       pd_comms_client_tcp_connect_cb,
                                       pd_comms_client_tcp_disconnect_cb,NULL,pd_comms_client_tcp_write_cb,
                                       NULL,pd_comms_client_tcp_ioctl_cb,
                                       pd_comms_client_tcp_read_cb, pd_comms_client_tcp_release_handle_cb,
                                       pd_comms_client_srv_system_change_notification_handle,
                                       pd_comms_client_tcp_set_data_enable_cb,
                                       pd_comms_client_apn_profile_change_notification_handle, NULL);
 
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
                                 (uint32)PD_COMMS_FUNC_RELEASE_HANDLE | (uint32)PD_COMMS_FUNC_BIND ;

  u_result = pd_comms_register_service(&z_reg_params,
                                       pd_comms_client_tcp_get_handle_cb,
                                       pd_comms_client_tcp_connect_cb, pd_comms_client_tcp_disconnect_cb,
                                       pd_comms_client_tcp_bind_cb,
                                       pd_comms_client_tcp_write_cb,
                                       NULL,NULL,
                                       pd_comms_client_tcp_read_cb, pd_comms_client_tcp_release_handle_cb,
                                       NULL,NULL,NULL,NULL);
 
  if(u_result == FALSE)
  {
    PDCOMM_APP_MSG_ERROR("Failed registering UDP services with PDCOMM layer",0,0,0);
    return;
  }
  
  PDCOMM_APP_MSG_MED("Successfully registered UDP callbacks",0,0,0);

  /*Register default ATL open/close callbacks*/

  if(pd_comms_atl_i_dflt_proxy_reg(PDSM_ATL_TYPE_IP,
                                   pd_comms_tcp_task_dft_atl_open,
                                   pd_comms_tcp_task_dft_atl_close,
                                   pd_comms_tcp_task_dft_atl_dns_lookup))
  {
    PDCOMM_APP_MSG_MED("Successfully registered default ATL open/close callbacks",0,0,0);  
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("Failed registering default ATL open/close callbacks",0,0,0);  
  }

}/*pd_comms_tcp_task_register_callbacks*/


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
  z_handle_table[q_handle_id].x_sockfd                    = DSS_ERROR;
  z_handle_table[q_handle_id].x_app_id                    = DSS_ERROR;
  z_handle_table[q_handle_id].x_prev_app_id               = DSS_ERROR;  
  z_handle_table[q_handle_id].x_force_dorm_app_id         = DSS_ERROR;
  z_handle_table[q_handle_id].client_disconnect_recvd     = FALSE;
  z_handle_table[q_handle_id].v_release_handle            = FALSE;
  z_handle_table[q_handle_id].l_inactivity_count          = 0;
  z_handle_table[q_handle_id].session_aborted             = FALSE; 
  z_handle_table[q_handle_id].u_connectPending            = FALSE;
  z_handle_table[q_handle_id].z_ssl_handle                = NULL;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle = 
                                                       DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
  z_handle_table[q_handle_id].u_use_laptop_call = FALSE;
  z_handle_table[q_handle_id].datacall_family = (uint16)DSS_AF_INET;
  z_handle_table[q_handle_id].z_datacall_family_fallback_state = FALLBACK_STATE_NONE;
  z_handle_table[q_handle_id].u_fallback_allowed = FALSE;
  
  z_handle_table[q_handle_id].requested_read              = FALSE;
  /* reset the ssl cert dir and the ssl cert */
  memset( z_handle_table[q_handle_id].p_ssl_cert_dir, 0, 
          sizeof(z_handle_table[q_handle_id].p_ssl_cert_dir) );
  z_handle_table[q_handle_id].w_ssl_cert_len              = 0;
  z_handle_table[q_handle_id].u_socket_retried_on_addr_change = FALSE;
  
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
  z_handle_table[q_handle_id].x_sockfd                    = DSS_ERROR;
  z_handle_table[q_handle_id].x_app_id                    = DSS_ERROR;
  z_handle_table[q_handle_id].x_prev_app_id               = DSS_ERROR;  
  z_handle_table[q_handle_id].x_force_dorm_app_id         = DSS_ERROR;
  z_handle_table[q_handle_id].client_disconnect_recvd     = FALSE;
  z_handle_table[q_handle_id].v_release_handle            = FALSE;
  z_handle_table[q_handle_id].l_inactivity_count          = 0;
  z_handle_table[q_handle_id].session_aborted             = FALSE; 
  z_handle_table[q_handle_id].p_read_ptr                  =
                             z_handle_table[q_handle_id].p_read_buff;
  z_handle_table[q_handle_id].w_num_bytes_already_read    = 0;
  z_handle_table[q_handle_id].w_num_bytes_in_buffer       = 0;
  z_handle_table[q_handle_id].w_num_bytes_to_read         = 0;
  z_handle_table[q_handle_id].w_num_bytes_to_write        = 0;
  z_handle_table[q_handle_id].w_num_bytes_written         = 0;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle = 
                                                       DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
  z_handle_table[q_handle_id].u_use_laptop_call = FALSE;
  
  z_handle_table[q_handle_id].datacall_family = (uint16)DSS_AF_INET;
  z_handle_table[q_handle_id].z_datacall_family_fallback_state = FALLBACK_STATE_NONE;
  z_handle_table[q_handle_id].u_fallback_allowed = FALSE;  
  
  z_handle_table[q_handle_id].requested_read              = FALSE;
  z_handle_table[q_handle_id].u_socket_retried_on_addr_change = FALSE;

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
    z_handle_table[l_i].x_sockfd                    = DSS_ERROR;
    z_handle_table[l_i].x_app_id                    = DSS_ERROR;
    z_handle_table[l_i].x_prev_app_id               = DSS_ERROR;  
    z_handle_table[l_i].x_force_dorm_app_id         = DSS_ERROR;
    z_handle_table[l_i].client_disconnect_recvd     = FALSE;
    z_handle_table[l_i].v_release_handle            = FALSE;
    z_handle_table[l_i].l_inactivity_count          = 0;
    z_handle_table[l_i].session_aborted             = FALSE;
    z_handle_table[l_i].p_read_ptr = z_handle_table[l_i].p_read_buff;
    z_handle_table[l_i].z_ssl_handle = NULL;
    z_handle_table[l_i].z_dns_cb_data.dns_session_handle = 
                                                 DSS_DNS_SESSION_MGR_INVALID_HANDLE;
    z_handle_table[l_i].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
    z_handle_table[l_i].u_use_laptop_call = FALSE;
    
    z_handle_table[l_i].datacall_family = (uint16)DSS_AF_INET;
    z_handle_table[l_i].z_datacall_family_fallback_state = FALLBACK_STATE_NONE;
    z_handle_table[l_i].u_fallback_allowed = FALSE;    
    z_handle_table[l_i].requested_read              = FALSE;

    /* reset the ssl cert dir and the ssl cert */
    memset( z_handle_table[l_i].p_ssl_cert_dir, 0, 
            sizeof(z_handle_table[l_i].p_ssl_cert_dir) );
    z_handle_table[l_i].w_ssl_cert_len              = 0;
    z_handle_table[l_i].u_socket_retried_on_addr_change = FALSE;   
  }

} /*pd_comms_tcp_task_init_handle_table() ends*/

/*===========================================================================
FUNCTION get_pdcomm_connect_error_from_dss_error

DESCRIPTION
  This function maps a dss error to a pdcomm_connect error so that the client
  can be notified about the error condition using this

DEPENDENCIES
  None

RETURN VALUE
  pdcomm_connect_result_type - contains the pdcomm_connect_error corresponding 
  to dss error

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_connect_result_type 
get_pdcomm_connect_error_from_dss_error(
  /*dss error which is to be mapped to pdcomm_connect_error*/
  sint15 x_error)
{
  switch(x_error)
  {
    case DSS_ERROR:

      return PDCOMM_CONNECT_ERROR_SOCKET_CONNECTION_FAILED;

    case DS_EBADAPP:
    case DS_ENETNONET:
    case DS_ENETCLOSEINPROGRESS:

      return PDCOMM_CONNECT_ERROR_PPP_OPEN_FAILURE;

    case DS_EBADF:
    case DS_EFAULT:

      return PDCOMM_CONNECT_ERROR_CONNECTION_FAILED_INTERNAL_ERROR;

    case DS_ETIMEDOUT:

      return PDCOMM_CONNECT_ERROR_CONNECTION_TIMED_OUT;

    case DS_ENETDOWN:

      return PDCOMM_CONNECT_ERROR_CONNECT_NETWORK_DOWN;

    case DS_EADDRREQ:

      return PDCOMM_CONNECT_ERROR_INVALID_PARAMETERS;

    case DS_EADDRINUSE:

      return PDCOMM_CONNECT_ERROR_CONNECT_ADDR_IN_USE;

    case DS_EIPADDRCHANGED:

      return PDCOMM_CONNECT_ERROR_TCP_CONNECTION_RESET;

    case DS_EOPNOTSUPP:

      return PDCOMM_CONNECT_ERROR_UNSUPPORTED_OPERATION;

    case DS_ECONNREFUSED:

      return PDCOMM_CONNECT_ERROR_TCP_CONNECTION_REFUSED;

    default:
      return PDCOMM_CONNECT_ERROR_UNKNOWN;

  } /*switch(x_error) ends*/

}/*get_pdcomm_connect_error_from_dss_error() ends*/


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
static void  pd_comms_tcp_task_reset_inactivity_count(
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
static void  pd_comms_tcp_task_handle_connect( 
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
static void pd_comms_tcp_task_update_state(
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

  if(z_handle_table[z_handle.q_handle_id].session_aborted)
  {
    /*If abort is in progress, the notificaiton will be sent to client directly
      and not from this function. No more notificaitons should reach the client*/
    notify_client = FALSE;
  }
  
  switch(z_new_state)
  {
    case PDCOMM_STATE_CONNECTED:
    {
      if(notify_client && z_old_state != PDCOMM_STATE_CONNECTED)
      {
        pd_comms_notification_params_type z_params;
        z_params.e_new_state = z_new_state;
        z_params.q_result = q_result;
        
        PDCOMM_APP_MSG_MED("Sending PDCOMM server connected event ",
                             0,0,0);
        // Log the connection status 
        pd_comms_log_connection_status_event(PD_EVENT_SERVER_CONNECTED);

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
       if (!z_handle_table[z_handle.q_handle_id].v_release_handle) 
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
               /*Send State Change also*/
               e_result = pd_comms_notify_client(z_handle,
                                    z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                    z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                    z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                                    STATE_CHANGE,&z_params);  
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
       }
       else
       {
          //safe to release handle here since the SSL and TCP connections are closed and we do not expect any callback's
          PDCOMM_APP_MSG_MED("PDCOMM APP: Releasing handle [%d]",z_handle.q_handle_id,0,0);
          pd_comms_tcp_task_release_handle(z_handle.q_handle_id);
       }           
      break;
    }/*case PDCOMM_STATE_DISCONNECTED*/
    
    case PDCOMM_STATE_CONNECTING:
    {
      if(notify_client && z_old_state != PDCOMM_STATE_CONNECTING)
      {
        pd_comms_notification_params_type z_params;
        z_params.e_new_state = z_new_state;
        z_params.q_result = q_result;

        PDCOMM_APP_MSG_MED("Sending PDCOMM server connecting event ",
                             0,0,0);
        // Log the connection status 
        pd_comms_log_connection_status_event(PD_EVENT_SERVER_CONNECTING);
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

          /*Send State Change also*/
          e_result = pd_comms_notify_client(z_handle,
                               z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                               z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                               z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                               STATE_CHANGE,&z_params);  
          
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
FUNCTION print_dss_err

DESCRIPTION
  Prints informative message for a particular DSS error

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void print_dss_err(
  /*dss error for which message is to be printed*/
  sint15 x_err_no)
{
  switch (x_err_no)
  {
    case DS_EMAPP:
      /* Too many apps */
      MSG_ERROR( "PDCOMM APP: Too many apps.", 0, 0, 0);
      break;
    case DS_EBADAPP:
      /* bad application ID */
      MSG_ERROR( "PDCOMM APP: Bad app id.", 0, 0, 0);
      break;
    case DS_ENETCLOSEINPROGRESS:
      /* net close in progress */
      PDCOMM_APP_MSG_HIGH( "PDCOMM APP: Close in progress.", 0, 0, 0);
      break;
    case DS_SOCKEXIST:
      /* socket exists */
      MSG_ERROR( "PDCOMM APP: Socket exists.", 0, 0, 0);
      break;
    case DS_ENETEXIST:
      /* Network exists */
      MSG_ERROR( "PDCOMM APP: Net exists.", 0, 0, 0);
      break;
    case DS_EAFNOSUPPORT:
      /* AF_INET parameter is bad */
      MSG_ERROR( "PDCOMM APP: Bad Addr Family.", 0, 0, 0);
      break;
    case DS_EPROTOTYPE:
      /* invalid protocol type */
      MSG_ERROR( "PDCOMM APP: Bad Protocol type.", 0, 0, 0);
      break;
    case DS_ESOCKNOSUPPORT:
      /* invalid or unsupported socket parameter specified */
      MSG_ERROR( "Bad socket param.", 0, 0, 0);
      break;
    case DS_EPROTONOSUPPORT:
      /*  specified protocol not supported */
      MSG_ERROR( "PDCOMM APP: Bad protocol not supported.", 0, 0, 0);
      break;
    case DS_EMFILE:
      /* too many descriptors open */
      MSG_ERROR( "PDCOMM APP: Too many descriptors open.", 0, 0, 0);
      break;
    case DS_EBADF:
      /* invalid socket descriptor is specfied */
      MSG_ERROR( "PDCOMM APP: Inv Socket Descriptor.", 0, 0, 0);
      break;
    case DS_EFAULT:
      /* addrlen parameter is invalid */
      MSG_ERROR( "PDCOMM APP: DS_EFAULT Inv Addr Len.", 0, 0, 0);
      break;
    case DS_EOPNOTSUPP:
      /* invalid server address specified */
      MSG_ERROR( "PDCOMM APP: Inv Serv Addr.", 0, 0, 0);
      break;
    case DS_EADDRREQ:
      /* destination address is required */
      MSG_ERROR( "PDCOMM APP: Dest Addr Req'd.", 0, 0, 0);
      break;
    case DS_ENETDOWN:
      /* network subsystem unavailable */
      MSG_ERROR( "PDCOMM APP: NW Subsys unavail.", 0, 0, 0);
      break;
    case DS_EISCONN:
      /* a socket descriptor is specified that is already connected */
      MSG_ERROR( "PDCOMM APP: Socket desc already conn.", 0, 0, 0);
      break;
    case DS_ECONNREFUSED:
      /* connection attempt refused */
      MSG_ERROR( "PDCOMM APP: Conn attmpt refused.", 0, 0, 0);
      break;
    case DS_ETIMEDOUT:
      /* connection attempt timed out */
      MSG_ERROR( "PDCOMM APP: Conn attmpt timeout.", 0, 0, 0);
      break;
    case DS_EIPADDRCHANGED:
      /* network level IP changed due to PPP resync */
      MSG_ERROR( "PDCOMM APP: IP Addr changed.", 0, 0, 0);
      break;
    case DS_EPIPE:
      /* broken pipe */
      MSG_ERROR( "PDCOMM APP: Broken Pipe.", 0, 0, 0);
      break;
    case DS_ENOTCONN:
      /* socket not connected */
      MSG_ERROR( "PDCOMM APP: Socket not connected.", 0, 0, 0);
      break;
    case DS_ECONNRESET:
      /* TCP connection reset by server */
      MSG_ERROR( "PDCOMM APP: TCP Conn. reset by server.", 0, 0, 0);
      break;
    case DS_ECONNABORTED:
      /* TCP connection aborted due to timeout or other failure */
      MSG_ERROR( "PDCOMM APP: TCP Connection aborted.", 0, 0, 0);
      break;
    case DS_EWOULDBLOCK:
      /* Operation would block */
      PDCOMM_APP_MSG_HIGH( "Operation would block.", 0, 0, 0);
      break;
    case DS_ENETNONET:
      /* network subsystem unavailable for some unknown reason */
      PDCOMM_APP_MSG_HIGH( "NW Subsys unavail unknown.", 0, 0, 0);
      break;
    case DS_ENETISCONN:
      /* network subsystem is connected and available */
      PDCOMM_APP_MSG_HIGH( "NW Subsys conn and avail.", 0, 0, 0);
      break;
    case DS_ENETINPROGRESS:
      /* network subsystem establishment currently in progress */
      PDCOMM_APP_MSG_HIGH( "NW Subsys create in progress.", 0, 0, 0);
      break;
    default:
      PDCOMM_APP_MSG_HIGH( "Unknown error %d.", x_err_no, 0, 0);
      break;
  } /* switch */
} /* print_dss_err() */

/*
******************************************************************************
 *  pdcomms_tcp_task_other_pdp_type
 *  Function description:
 *
 *  The two supported PDP types are PPP and IP. When passed one type, this 
 *  function returns the other
 *****************************************************************************
 */
pdsm_atl_pdp_type_e_type pdcomms_tcp_task_other_pdp_type(pdsm_atl_pdp_type_e_type input_pdp_type)
{
  if (input_pdp_type == PDSM_ATL_PDP_IP)
    return PDSM_ATL_PDP_PPP;

  if (input_pdp_type == PDSM_ATL_PDP_PPP)
    return PDSM_ATL_PDP_IP;

  /* if the input type matches neither of these, we should ideally return an error,
     but unforunately there is no err, so just return the IP pdp type
  */
  return PDSM_ATL_PDP_IP;
}


/*
 ******************************************************************************
 * pd_comms_tcp_task_find_matching_pdp_profile
 *
 * Function description:
 *
 *  Find a pdp profile in a specified pdp profile family.

 * Parameters: 
 *
 *  None
 *
 * Return value: 
\ *   SUCCEEDED : If required pdp profile structure was found
 *   FAILED    : Otherwise
 *
 *
 ******************************************************************************
*/
static boolean pd_comms_tcp_task_find_matching_pdp_profile(const char* apn_name,
                                                           ds_profile_tech_etype tech_type,
                                                           uint32 pdp_type,
                                                           int  *profile_num)
{
    ds_profile_list_type search_list_info; 
    ds_profile_list_info_type list_info;
    ds_profile_info_type profile_info;
    ds_profile_itr_type itr;
    uint8 value[128];
    boolean result = FAILED;
    
    if(apn_name == NULL)
    {
      MSG_HIGH("Apn name is NULL",0,0,0);
      return FAILED;
    }

    search_list_info.dfn = DS_PROFILE_LIST_SEARCH_PROFILES;

    search_list_info.ident = DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN;

    if(tech_type == DS_PROFILE_TECH_3GPP2)
    {
      search_list_info.ident = DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING;
    }

    search_list_info.info.len = strlen(apn_name);
    search_list_info.info.buf = (void*) apn_name;

    if(strlen(apn_name) >= 3)
    {
      MSG_HIGH("APN name is passed, printing first three charecters %c %c %c",apn_name[0],apn_name[1],apn_name[2]);
    }

    if( ds_profile_get_list_itr(tech_type, &search_list_info,&itr) == DS_PROFILE_REG_RESULT_SUCCESS)
    {
    
      memset(&list_info, 0, sizeof (ds_profile_list_info_type));
      memset(&profile_info, 0, sizeof (ds_profile_info_type));
      
      list_info.name = &profile_info;
      list_info.name->len  = sizeof(value);
      list_info.name->buf  = (void*)value;
      
      MSG_HIGH("Going through the list of profiles matched",0,0,0);
      while(ds_profile_get_info_by_itr(itr, &list_info) == DS_PROFILE_REG_RESULT_SUCCESS)
      {
        ds_profile_status_etype status = DS_PROFILE_REG_RESULT_FAIL;
        ds_profile_tech_etype   profile_type = tech_type;
        ds_profile_trn_etype    trn_type     = DS_PROFILE_TRN_READ;
        ds_profile_action_etype act          = DS_PROFILE_ACTION_CANCEL;
        ds_profile_num_type     prf_num  = list_info.num;
        
        ds_profile_hndl_type    profile_hndl = NULL;
        ds_profile_identifier_type ident;
        ds_profile_info_type       info;
        uint32 pdp_type_i = 0;

        MSG_HIGH("Profile Number: %d", list_info.num,0,0);

        ident = DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE;
    
        if(tech_type == DS_PROFILE_TECH_3GPP2)
        {
          ident = DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE;
        }

        status = ds_profile_begin_transaction( trn_type, 
                                               profile_type, 
                                               prf_num, 
                                               &profile_hndl );

        if( (status == DS_PROFILE_REG_RESULT_SUCCESS) &&
            (profile_hndl != NULL) 
          )
        {             
             info.buf = &(pdp_type_i);
             info.len = sizeof(uint32);
             MSG_HIGH("Getting the pdp type", 0,0,0);

             status = ds_profile_get_param( profile_hndl, 
                                   ident, 
                                   &info);
             MSG_HIGH("Got pdp type as %d", pdp_type_i,0,0);
    
             if(status == DS_PROFILE_REG_RESULT_SUCCESS &&
                pdp_type_i ==   pdp_type)
             {
               MSG_HIGH("Got the matching profile %d",list_info.num,0,0);
               *profile_num = list_info.num;
               (void)ds_profile_end_transaction( profile_hndl, act);
               profile_hndl = NULL;
               result = SUCCEEDED;
               break;
             }
        }

        if(profile_hndl != NULL)
        {          
          (void)ds_profile_end_transaction( profile_hndl, act);
           profile_hndl = NULL;
        }

        if(ds_profile_itr_next(itr) != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          break;
        }
        else
        {          
          memset(&list_info, 0, sizeof (ds_profile_list_info_type));
          memset(&profile_info, 0, sizeof (ds_profile_info_type));
          
          list_info.name = &profile_info;
          list_info.name->len  = sizeof(value);
          list_info.name->buf  = (void*)value;
        }
      }
      ds_profile_itr_destroy(itr);      
    }    
    return result;
}


/*
 ******************************************************************************
 * pd_comms_tcp_task_find_emergency_profile_by_tech
 *
 * Function description:
 *
 *  Find an emegency profile

 * Parameters: 
 *
 *  None
 *
 * Return value: 
\ *   SUCCEEDED : If emergency profile was found
 *   FAILED    : Otherwise
 *
 *
 ******************************************************************************
*/
static boolean pd_comms_tcp_task_find_emergency_profile_by_tech(ds_profile_tech_etype tech_type,
                                                           uint32 *pdp_type,
                                                           int  *profile_num)
{
  ds_profile_list_type search_list_info; 
  ds_profile_list_info_type list_info;
  ds_profile_info_type profile_info;
  ds_profile_itr_type itr;
  uint8 value[128];
  ds_profile_3gpp_emergency_calls_are_supported emergency_calls_supported= TRUE;
  boolean result = FAILED;

  if ( NULL == pdp_type || NULL == profile_num)
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL Parameter");
     return FAILED;
  }
    
  search_list_info.dfn = DS_PROFILE_LIST_SEARCH_PROFILES;

  search_list_info.ident =  DS_PROFILE_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED; /* identifies the field to search upon */

  if(tech_type == DS_PROFILE_TECH_3GPP2)
  {
    search_list_info.ident = DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING;
  }

  search_list_info.info.len = sizeof(ds_profile_3gpp_emergency_calls_are_supported);
  search_list_info.info.buf = &emergency_calls_supported; /* value of the field we are looking for in the profile */

  if( ds_profile_get_list_itr(tech_type, &search_list_info,&itr) == DS_PROFILE_REG_RESULT_SUCCESS)
  {
  
    memset(&list_info, 0, sizeof (ds_profile_list_info_type));
    memset(&profile_info, 0, sizeof (ds_profile_info_type));
    
    list_info.name = &profile_info;
    list_info.name->len  = sizeof(value);
    list_info.name->buf  = (void*)value;
    
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Going through the list of profiles matched");
    if(ds_profile_get_info_by_itr(itr, &list_info) == DS_PROFILE_REG_RESULT_SUCCESS)
    {
    
      ds_profile_status_etype status = DS_PROFILE_REG_RESULT_FAIL;
      ds_profile_tech_etype   profile_type = tech_type;
      ds_profile_trn_etype    trn_type     = DS_PROFILE_TRN_READ;
      ds_profile_action_etype act          = DS_PROFILE_ACTION_CANCEL;
      ds_profile_num_type     prf_num  = list_info.num;
      
      ds_profile_hndl_type    profile_hndl = NULL;
      ds_profile_identifier_type ident;
      ds_profile_info_type       info;
      uint32 pdp_type_i = 0;

      MSG_1(MSG_SSID_GPSSM,MSG_LEGACY_HIGH,"Profile Number: %d",list_info.num);        
      *profile_num = list_info.num;

      ident = DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE;

      if(tech_type == DS_PROFILE_TECH_3GPP2)
      {
        ident = DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE;
      }

      status = ds_profile_begin_transaction( trn_type, 
                                             profile_type, 
                                             prf_num, 
                                             &profile_hndl );

      if( (status == DS_PROFILE_REG_RESULT_SUCCESS) &&
          (profile_hndl != NULL) 
        )
      {             
           info.buf = &(pdp_type_i);
           info.len = sizeof(uint32);
           MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Getting the pdp type");

           status = ds_profile_get_param( profile_hndl, 
                                 ident, 
                                 &info);
           MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Got pdp type as %d", pdp_type_i);

           if(status == DS_PROFILE_REG_RESULT_SUCCESS)
           {
              *pdp_type = pdp_type_i;
           }
      }

      if(profile_hndl != NULL)
      {
        (void)ds_profile_end_transaction( profile_hndl, act);
      }      
      result = SUCCEEDED; /*Even if getting PDP type was not sucesfull, 
                                          We got the profile, so return SUCCESS*/
    }
    /*Free the DS List iterator object*/
    ds_profile_itr_destroy(itr);
  }
  return result;
}

uint32 pd_comms_tcp_task_xlate_3gpp_pdp_type_to_atl_pdp_type(ds_profile_3gpp_pdp_type_enum_type pdp_type)
{
  switch(pdp_type)
  {
    case DS_PROFILE_3GPP_PDP_IP: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4; 
    case DS_PROFILE_3GPP_PDP_IPV6: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6; 
    case DS_PROFILE_3GPP_PDP_IPV4V6: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4V6; 
    case DS_PROFILE_3GPP_PDP_PPP: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_PPP;
     default:
      return PDSM_PA_APN_PROFILE_PDN_TYPE_MAX;
  }
}


uint32 pd_comms_tcp_task_xlate_3gpp2_pdp_type_to_atl_pdp_type(ds_profile_3gpp2_pdn_type_enum_type pdp_type)
{
  switch(pdp_type)
  {
    case DS_PROFILE_3GPP2_PDN_TYPE_V4: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4; 
    case DS_PROFILE_3GPP2_PDN_TYPE_V6: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6; 
    case DS_PROFILE_3GPP2_PDN_TYPE_V4_V6: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4V6; 
    default:
      return PDSM_PA_APN_PROFILE_PDN_TYPE_MAX;
  }
}

/*===========================================================================

FUNCTION pd_comms_tcp_task_find_emergency_tech_pdp_profile

DESCRIPTION
  This function gets the Tech_type, PDP Type and Profile number needed for
  bringing up a Data call.
  Acts as a wrapper function for pd_comms_tcp_task_find_emergency_by_tech

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean pd_comms_tcp_task_find_emergency_profile_tech_pdp_type(ds_profile_tech_etype *tech_type,
                                                           uint32 *pdp_type,
                                                           int  *profile_num)
{
    boolean result = FAILED;
    ds_profile_tech_etype tech_type_i = DS_PROFILE_TECH_3GPP;

      if (tech_type == NULL || pdp_type == NULL || profile_num == NULL) 
      {
         MSG_ERROR("Emergency Profile Tech PDP: Invalid params, using default profile",0,0,0);
         return result;
      }      
      /* First check 3GPP Profiles, if we get the emergency profile set the tech_type and return Success
         PDP_type and profile Number are already set */
      if(pd_comms_tcp_task_find_emergency_profile_by_tech(tech_type_i, pdp_type, profile_num) == SUCCEEDED)
      {
        *tech_type = tech_type_i;
        *pdp_type = pd_comms_tcp_task_xlate_3gpp_pdp_type_to_atl_pdp_type((ds_profile_3gpp_pdp_type_enum_type)*pdp_type);
         MSG_LOW("Emergency profile: PDP:%d tech_type: %d profile: %d",(uint32)(*pdp_type),(int)(*tech_type),*profile_num);
        result = SUCCEEDED;
        return result; 
      }
      tech_type_i = (tech_type_i == DS_PROFILE_TECH_3GPP) ? DS_PROFILE_TECH_3GPP2 : DS_PROFILE_TECH_3GPP;

      /* First check 3GPP2 Profiles, if we get the emergency profile set the tech_type and return Success
         PDP_type and profile Number are already set */
      if(pd_comms_tcp_task_find_emergency_profile_by_tech(tech_type_i, pdp_type, profile_num) == SUCCEEDED)
      {
        *tech_type = tech_type_i;
        *pdp_type = pd_comms_tcp_task_xlate_3gpp2_pdp_type_to_atl_pdp_type((ds_profile_3gpp2_pdn_type_enum_type)*pdp_type);
        MSG_LOW("Emergency profile: PDP:%d tech_type: %d profile: %d",(uint32)(*pdp_type),(int)(*tech_type),*profile_num);
        result = SUCCEEDED;
        return result;
      }
      return result;
}

ds_profile_3gpp_pdp_type_enum_type pd_comms_tcp_task_xlate_to_3gpp_pdp_type(uint32 pdp_type)
{
  switch(pdp_type)
  {
    case PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4: 
      return DS_PROFILE_3GPP_PDP_IP; 
    case PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6: 
      return DS_PROFILE_3GPP_PDP_IPV6; 
    case PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4V6: 
      return DS_PROFILE_3GPP_PDP_IPV4V6; 
    case PDSM_PA_APN_PROFILE_PDN_TYPE_PPP: 
      return DS_PROFILE_3GPP_PDP_PPP;
     default:
      return DS_PROFILE_3GPP_PDP_MAX;
  }
}

ds_profile_3gpp2_pdn_type_enum_type pd_comms_tcp_task_xlate_to_3gpp2_pdp_type(uint32 pdp_type)
{
  switch(pdp_type)
  {
    case PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4: 
      return DS_PROFILE_3GPP2_PDN_TYPE_V4; 
    case PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6: 
      return DS_PROFILE_3GPP2_PDN_TYPE_V6; 
    case PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4V6: 
      return DS_PROFILE_3GPP2_PDN_TYPE_V4_V6; 
     default:
      return DS_PROFILE_3GPP2_PDN_TYPE_MAX;
  }
}

uint32 pd_comms_tcp_task_xlate_atl_pdp_type_to_pdp_type(pdsm_atl_pdp_type_e_type pdp_type)
{
  switch(pdp_type)
  {
    case PDSM_ATL_PDP_IP: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4; 
    case PDSM_ATL_PDP_IPV6: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6; 
    case PDSM_ATL_PDP_PPP: 
      return PDSM_PA_APN_PROFILE_PDN_TYPE_PPP; 
     default:
      return PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4V6;
  }
}


/*===========================================================================
FUNCTION pd_comms_tcp_get_apn_name

DESCRIPTION
  This function fetched the APN name to be used for a profile type.
  APN name to be used will depend of the profile type and the present serving system.
  If no APN name is configured in nv, then null will be returned.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/

const char* pd_comms_tcp_get_apn_name(uint32 profile_type, uint32 *pdp_type_p,
                                      ds_profile_tech_etype *tech_type)
{
  sys_sys_mode_e_type sys_mode = SYS_SYS_MODE_NONE;
  uint32 srv_sys_type = PDCOMM_APN_PROFILE_SRV_SYS_MAX_INDEX;
  sys_mode = pd_comms_tcp_get_sys_mode();

  switch(sys_mode)
  {  
    case SYS_SYS_MODE_CDMA:
      srv_sys_type = PDCOMM_APN_PROFILE_SRV_SYS_CDMA_INDEX;
      *tech_type = DS_PROFILE_TECH_3GPP2;
      break;
    case SYS_SYS_MODE_HDR:
      srv_sys_type = PDCOMM_APN_PROFILE_SRV_SYS_HDR_INDEX; 
      *tech_type = DS_PROFILE_TECH_3GPP2;
      break;
    case SYS_SYS_MODE_GSM:
      srv_sys_type = PDCOMM_APN_PROFILE_SRV_SYS_GSM_INDEX;       
      *tech_type = DS_PROFILE_TECH_3GPP;
      break;
    case SYS_SYS_MODE_WCDMA:
      srv_sys_type = PDCOMM_APN_PROFILE_SRV_SYS_WCDMA_INDEX;       
      *tech_type = DS_PROFILE_TECH_3GPP;
      break;
    case SYS_SYS_MODE_LTE:
      srv_sys_type = PDCOMM_APN_PROFILE_SRV_SYS_LTE_INDEX;       
      *tech_type = DS_PROFILE_TECH_3GPP;
      break;  
    case SYS_SYS_MODE_TDS:
      srv_sys_type = PDCOMM_APN_PROFILE_SRV_SYS_TDS_INDEX;       
      *tech_type = DS_PROFILE_TECH_3GPP;
      break;
    default:
      srv_sys_type = PDCOMM_APN_PROFILE_SRV_SYS_MAX_INDEX;
  }

  MSG_HIGH("Profile type %d Serving System %d",profile_type,srv_sys_type,0);

  if(srv_sys_type == PDCOMM_APN_PROFILE_SRV_SYS_MAX_INDEX)
  {
    MSG_HIGH("Returning Null Apn name",0,0,0);
    return NULL;
  }
  else
  {
    /*If ATL proxy is registered, use information passed from ATL proxy,
      else use the information from configured APN profile*/
    if(pd_comms_atl_i_ppp_proxy_exists())
    {    
      *pdp_type_p = pd_comms_tcp_task_xlate_atl_pdp_type_to_pdp_type(z_atl_open_params.pdp_type);
      return &z_atl_open_params.apn_name[0];
    }
    else
    {
      *pdp_type_p = pdcomm_tcp_apn_profiles[srv_sys_type][profile_type].pdp_type;
      return &pdcomm_tcp_apn_profiles[srv_sys_type][profile_type].apn_name[0];    
    }
  }
}

/*===========================================================================

FUNCTION pd_comms_tcp_task_dss_get_iface_ipfamily 

DESCRIPTION
  Get the current network interface ipfamily given a profile number

DEPENDENCIES
  none

RETURN VALUE
  PDP Type 

SIDE EFFECTS
  none

===========================================================================*/

static uint8 pd_comms_tcp_task_dss_get_iface_ipfamily(dss_net_policy_info_type* z_policy_info, boolean ipv6_preffered)
{
  dss_iface_id_type         z_iface_id = DSS_IFACE_INVALID_ID;
  sint15                    dss_err_no;
  uint8                     z_available_iface_family = PDSM_PA_APN_PROFILE_PDN_TYPE_MAX;

  /*Check if IPv6 interface is available on this profile*/
  z_policy_info->family = DSS_AF_INET6; 
  z_iface_id = dss_get_iface_id_by_policy(*z_policy_info,&dss_err_no );
  print_dss_err(dss_err_no);

  /*If Pref is TRUE we are looking for Iface Family for Emergency call. Prefer IPv6 if available*/
  if ((z_iface_id != DSS_IFACE_INVALID_ID)&&
      (dss_err_no != DS_ENOROUTE && dss_err_no != DS_EFAULT)
      )
  {
     if (ipv6_preffered == TRUE)
     {
        PDCOMM_APP_MSG_LOW("IPFAMILY: Found IPv6 interface on this Profile",0,0,0);
        return ((uint8)PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6);
     }
     else
     {
         z_available_iface_family = PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6;
     }
  }
 
  /*if we reach here. 
    1). No IPv6 iface associated with the Emergency Profile, if Preference is TRUE
    2). Or IPv6 iface was found and Preference was set to FALSE (not emergency) Prefer IPv4 if available. 
    */
  z_policy_info->family = DSS_AF_INET; 
  z_iface_id = dss_get_iface_id_by_policy(*z_policy_info,&dss_err_no );
  print_dss_err(dss_err_no);
  if ((z_iface_id != DSS_IFACE_INVALID_ID)&&
      (dss_err_no != DS_ENOROUTE && dss_err_no != DS_EFAULT)
     )
  {
        PDCOMM_APP_MSG_LOW("IPFAMILY: Found IPv4 interface on this Profile",0,0,0);
        return ((uint8)PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4);
  }
  else
  {
     /*If we reach here. 
       1). Preference for IPv4 iface, but only IPv6 iface was up.
       2). Preference for IPv6 iface, but only IPv4 was found.
       3). neither IPv6 / IPv4 interfaces are associated with this profile. Return IPv4 default*/
     if (z_available_iface_family == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6) 
     {
       PDCOMM_APP_MSG_LOW("IPFAMILY: Found IPv6 interface only. for NON Emergency PDN",0,0,0);
       return ((uint8)PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6);
     }
     else
     {
       PDCOMM_APP_MSG_LOW("IPFAMILY: Failed to find Iface Family of Interface on PDN",0,0,0);
       return ((uint8)PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4);
     }
  }
}

/*===========================================================================
FUNCTION pd_comms_tcp_get_pdp_type

DESCRIPTION
  This function is used to determine the PDP Type to bring up the data call for
  1)DNS resolution
  2)SLP connection.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/

static void pd_comms_tcp_get_pdp_type(uint32 q_handle_id, uint32 *pdp_type)
{
   ds_profile_tech_etype tech_type = DS_PROFILE_TECH_3GPP;
   const char *apn_name = NULL;
   int profile_num = 0;

   if (pdp_type == NULL)
   {
      PDCOMM_APP_MSG_MED("PDP Type get failed, using Default",0,0,0);
      return;
   }
	
  /*If connection request is 
    1) Regular(Internet), PDP Type and Tech Type are Configured in LBS NV.
    2) Emergency, LBS NV is NULL or Not configured. We need to Get the PDP Type and Tech Type from DS profile list
    */
  apn_name = pd_comms_tcp_get_apn_name(
      z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.profile_type,
      pdp_type, &tech_type);

  if ((apn_name == NULL || apn_name[0] == '\0')&&
      (z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.profile_type == PDCOMM_APN_PROFILE_TYPE_LBS_EMERGENCY)) 
  {
     if(pd_comms_tcp_task_find_emergency_profile_tech_pdp_type(&tech_type,pdp_type,&profile_num) == SUCCEEDED)
     {
        PDCOMM_APP_MSG_MED("EMERGENCY Profile: Tech: %d, PDP: %d \n",(int)tech_type,*pdp_type,0);
     }
     else
     {
        PDCOMM_APP_MSG_MED("EMERGENCY Profile get failed",0,0,0);
        *pdp_type = PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4V6;
     }
  }
  return;
}

/*===========================================================================
FUNCTION pd_comms_tcp_get_iface_ipfamily

DESCRIPTION
  This function is used to determine the IP Family Type to bring up the data call for
  1)DNS resolution
  2)SLP connection.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/

static void pd_comms_tcp_get_iface_ipfamily(dss_net_policy_info_type*  p_policy_info, boolean ipv6_preffered)
{
  dss_net_policy_info_type z_policy_info;
  uint8 ip_family = PDSM_PA_APN_PROFILE_PDN_TYPE_MAX;

  dss_init_net_policy_info(&z_policy_info);

  memscpy(&z_policy_info,sizeof(dss_net_policy_info_type),p_policy_info,sizeof(dss_net_policy_info_type));
  /*Copy the profile number to search the ipfamily of interface up on the profile*/
  ip_family = pd_comms_tcp_task_dss_get_iface_ipfamily(&z_policy_info, ipv6_preffered);
  if (ip_family == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6) 
  {
      p_policy_info->family = DSS_AF_INET6;
  }
  else if (ip_family == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4)
  {
      p_policy_info->family = DSS_AF_INET;
  }
  else
  {
      p_policy_info->family = DSS_AF_INET;
      PDCOMM_APP_MSG_HIGH("Could not determine Iface IP Family", 0,0,0);
  }
  return;
}

/*===========================================================================

  
FUNCTION pd_comms_tcp_task_dss_get_policy

DESCRIPTION
  Set the policy info used by DS. is_routable flag set for IP filters

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void pd_comms_tcp_task_dss_get_policy
(
  dss_net_policy_info_type*  p_policy_info,
  uint32 q_handle_id
)
{
  int profile_num;  
  const char *apn_name;
  uint32 pdp_type = 0, pdp_type_tech = 0;
  ds_profile_tech_etype tech_type = DS_PROFILE_TECH_3GPP;
  boolean profile_found = FALSE;

  if(p_policy_info == NULL)
  {
     MSG_ERROR("DSS_GET_POLICY: policy_info ptr is NULL !!", 0, 0, 0);
     return;
  }

  dss_init_net_policy_info(p_policy_info);

  apn_name = pd_comms_tcp_get_apn_name(
    z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.profile_type,
    &pdp_type, &tech_type);

  if(pd_comms_atl_i_ppp_proxy_exists())
  {
    p_policy_info->iface.kind       = DSS_IFACE_NAME;


    if(z_atl_open_params.iface_family == PDSM_ATL_IP_V6)
    {      
      p_policy_info->family           = DSS_AF_INET6;
    }
    else
    {
      p_policy_info->family           = DSS_AF_INET;
    }
    p_policy_info->policy_flag      = DSS_IFACE_POLICY_SPECIFIED;
    p_policy_info->is_routeable     = TRUE;  
    /*
     If a target doesn\92t have the code it probably doesn\92t have the 
     following app_identifier defined, likely they are all are pre-2H07 
     targets.So to take care of every situation,  featurize
     the following block.*/
#if ((defined FEATURE_DATA_PS_OMH) || (DSS_VERSION >= 1800))
    p_policy_info->app_identifier = LBS_APP_TYPE ;
#endif /* FEATURE_DATA_PS_OMH */
  }
  else
  {    
    p_policy_info->iface.kind       = DSS_IFACE_NAME;
    p_policy_info->policy_flag      = DSS_IFACE_POLICY_SPECIFIED;
    p_policy_info->is_routeable     = TRUE;   /* IP filter */
    p_policy_info->iface.info.name  = DSS_IFACE_ANY;

    if(z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
    {
      p_policy_info->family           = DSS_AF_INET;
    }
    else if(z_handle_table[q_handle_id].ip_addr_type  == PD_COMMS_IP_ADDR_IPV6)
    {
      p_policy_info->family           = DSS_AF_INET6;
    }
    else
    {
      p_policy_info->family           = DSS_AF_UNSPEC;
    }

#if ((defined FEATURE_DATA_PS_OMH) || (DSS_VERSION >= 1800))
    p_policy_info->app_identifier = LBS_APP_TYPE ;
#endif /* FEATURE_DATA_PS_OMH */
  }
  
  PDCOMM_APP_MSG_HIGH("Policy Family set to: %d", p_policy_info->family,0,0);
  /*If not APN name is configured, the use the default profile*/
  if(apn_name != NULL && apn_name[0] != '\0')
  {
    /*Convert the pdp type to Tech specific pdp type*/
    pdp_type_tech = (tech_type == DS_PROFILE_TECH_3GPP) ? (uint32)pd_comms_tcp_task_xlate_to_3gpp_pdp_type(pdp_type) :
                                                          (uint32)pd_comms_tcp_task_xlate_to_3gpp2_pdp_type(pdp_type);     
  
    /*First search is the prefered tech type*/
    if(pd_comms_tcp_task_find_matching_pdp_profile(
                          &apn_name[0],
                          tech_type,
                          pdp_type_tech,
                          &profile_num) == SUCCEEDED )
    {
      if(tech_type == DS_PROFILE_TECH_3GPP)
      {
        p_policy_info->umts.pdp_profile_num  = profile_num;
      }
      else
      {
        p_policy_info->cdma.data_session_profile_id  = profile_num;
      }

      profile_found = TRUE;
      MSG_HIGH("Profile Found = %d Tech Type = %d",profile_num,tech_type,0);
    }
  
    /*If we are here, then we did not find the profile in the prefered tech_type, 
      now try in the other tech_type*/
     tech_type = (tech_type == DS_PROFILE_TECH_3GPP) ? DS_PROFILE_TECH_3GPP2 : DS_PROFILE_TECH_3GPP;
  
    /*Convert the pdp type to Tech specific pdp type*/
    pdp_type_tech = (tech_type == DS_PROFILE_TECH_3GPP) ? (uint32)pd_comms_tcp_task_xlate_to_3gpp_pdp_type(pdp_type) :
                                                          (uint32)pd_comms_tcp_task_xlate_to_3gpp2_pdp_type(pdp_type);     
  
     
    if(pd_comms_tcp_task_find_matching_pdp_profile(
                          &apn_name[0],
                          tech_type,
                          pdp_type_tech,
                          &profile_num) == SUCCEEDED )
    {
      if(tech_type == DS_PROFILE_TECH_3GPP)
      {
        p_policy_info->umts.pdp_profile_num  = profile_num;
      }
      else
      {
        p_policy_info->cdma.data_session_profile_id  = profile_num;
      }

      profile_found = TRUE;
      MSG_HIGH("Profile Found = %d Tech Type = %d",profile_num,tech_type,0);
    }
    if (profile_found)
    {
       pd_comms_tcp_get_iface_ipfamily(p_policy_info, FALSE);
       return;
    }  
    /*If we are here, we did not find the profile in any tech type,
      pass the apn name instead*/
    p_policy_info->umts.apn.length = strlen(apn_name);
    PDCOMM_APP_MSG_HIGH("Could not find matching profile. Pass APN name of size %d to DS",
            p_policy_info->umts.apn.length,0,0);
  
    p_policy_info->umts.apn.name = (byte*)apn_name;
  } 
  else
  {  
     if(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.profile_type == PDCOMM_APN_PROFILE_TYPE_LBS_EMERGENCY)
     {
      if(pd_comms_tcp_task_find_emergency_profile_tech_pdp_type(&tech_type,&pdp_type,&profile_num) == SUCCEEDED)
      {
         if (tech_type == DS_PROFILE_TECH_3GPP)
         {
            p_policy_info->umts.pdp_profile_num  = profile_num;
         }
         else
         {
            p_policy_info->cdma.data_session_profile_id  = profile_num;
         }

         if (pdp_type == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6)
         {
            p_policy_info->family  = DSS_AF_INET6;
         }
         else if( pdp_type == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4) 
         {
            p_policy_info->family  = DSS_AF_INET;
         }
         else
         {
            pd_comms_tcp_get_iface_ipfamily(p_policy_info, TRUE);           
         }
         MSG_HIGH("Emergency Policy Family set to: %d",p_policy_info->family,0,0);
         return;
      }
     }
    MSG_HIGH("APN name is null and Session is non emergency, using default profile",0,0,0);
  }
} /* pd_comms_tcp_task_dss_get_policy */



/*===========================================================================

FUNCTION pd_comms_tcp_task_dss_get_iface_id 

DESCRIPTION
  Get the current network interface id on which the GPS data call is up.

DEPENDENCIES
  none

RETURN VALUE
  network interface id 

SIDE EFFECTS
  none

===========================================================================*/
static dss_iface_id_type pd_comms_tcp_task_dss_get_iface_id(uint32 q_handle_id)
{
  
  dss_net_policy_info_type  z_policy_info;
  dss_iface_id_type         z_iface_id;
  sint15                    dss_err_no;

  /* if a ppp proxy exists get interface using apn name(through policy) */
  if ((pd_comms_atl_i_ppp_proxy_exists()) || (z_handle_table[q_handle_id].x_app_id == DSS_ERROR))
  {
    if (z_handle_table[q_handle_id].x_app_id == DSS_ERROR) {
      MSG_MED("pd_comms_tcp_task_dss_get_iface_id called with NULL network interface id",0,0,0);
    }
    pd_comms_tcp_task_dss_get_policy(&z_policy_info, q_handle_id);
    z_iface_id = dss_get_iface_id_by_policy( z_policy_info,&dss_err_no );
    return z_iface_id;
  }
  /* otherwise get interface based on network handle */
  else
    {
      return dss_get_iface_id(z_handle_table[q_handle_id].x_app_id);
    } 
}



/*===========================================================================

FUNCTION pd_comms_tcp_task_dss_get_laptop_call_iface_id 

DESCRIPTION
  Get the current network interface id on which the laptop call is up.

DEPENDENCIES
  none

RETURN VALUE
  network interface id 

SIDE EFFECTS
  none

===========================================================================*/
/* -- no longer used since when laptop call is up DNS is performed based on policy --
static dss_iface_id_type pd_comms_tcp_task_dss_get_laptop_call_iface_id()
{
  dss_net_policy_info_type  z_policy_info;
  dss_iface_id_type         z_iface_id;
  sint15                    dss_err_no;

  pd_comms_tcp_task_dss_get_laptop_call_policy(&z_policy_info); 
  z_iface_id = dss_get_iface_id_by_policy(z_policy_info,&dss_err_no );
  return z_iface_id;
}
*/

/*===========================================================================

FUNCTION pd_comms_tcp_task_dss_add_filter

DESCRIPTION
  Add IP filter based on the source address of the network entity that is 
  connected.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - Successfully added the IP filter
  FALSE - Error

SIDE EFFECTS
  None

===========================================================================*/

boolean pd_comms_tcp_task_dss_add_filter(uint32 q_handle_id)
{
  ip_filter_type        rx_fltr[NUM_RX_FLTRS];
  dss_iface_id_type     iface_id;
  sint15                dss_err_no;
  boolean               retval;

  if(q_handle_id < PDCOMM_MAX_HANDLES)
  {

    /*-------------------------------------------------------------------------
      Get the iface pointer instance 0
    -------------------------------------------------------------------------*/  
    
    iface_id = pd_comms_tcp_task_dss_get_iface_id(q_handle_id);

    if( iface_id == DSS_IFACE_INVALID_ID )
    {
      PDCOMM_APP_MSG_ERROR("could not get iface id",0,0,0);
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Populate the Rx Filter Template
    -------------------------------------------------------------------------*/
    /*Check for Ipv4 address and port if Ipv4 address is provided or check for Ipv6 address and port
      if Ipv6 address is provided*/
    if (  (z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4 &&
           z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_addr.ps_s_addr &&
           z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_port
          )
          ||
          (z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV6 &&
           z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr.ps_s6_addr64 &&
           z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_port
          )
       )
    {    
      struct ps_sockaddr_storage    local_sockaddr;
      uint16                        local_addrlen;

      memset(rx_fltr, 0, sizeof(ip_filter_type)*NUM_RX_FLTRS);

      memset(&local_sockaddr, 0, sizeof(local_sockaddr));
      local_addrlen = sizeof(struct ps_sockaddr_in);
      
      if(dss_getsockname( z_handle_table[q_handle_id].x_sockfd,
                          (struct ps_sockaddr *) &local_sockaddr,
                           &local_addrlen,
                           &dss_err_no)== DSS_ERROR)
      {
        PDCOMM_APP_MSG_ERROR("Unable to get the local port",0,0,0);
        return FALSE;
      }
      
      
      PDCOMM_APP_MSG_HIGH("PDCOMM socket bound to port %d",
      ((struct ps_sockaddr_in *) &local_sockaddr)->ps_sin_port, 0, 0);
      

      /* Set the filter mask for 
      ** 1) srce IP Address: Network Entity IP address supplied by client
      ** 2) srce TCP port: Network Entity TCP port suppplied by client (i.e. 4911)
      ** 3) dst TCP port: Local TCP port of socket communication
      */

      if(z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_TCP)
      {
        
        if(z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
        {      
          rx_fltr[0].ip_vsn = IP_V4;
          rx_fltr[0].ip_hdr.v4.field_mask = IPFLTR_MASK_IP4_SRC_ADDR | IPFLTR_MASK_IP4_NEXT_HDR_PROT;         
          rx_fltr[0].ip_hdr.v4.next_hdr_prot      = PS_IPPROTO_TCP;

          /* Source IP address from network entity
          */
          rx_fltr[0].ip_hdr.v4.src.addr = z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_addr;
          rx_fltr[0].ip_hdr.v4.src.subnet_mask.ps_s_addr = dss_htonl(0xFFFFFFFF);

          /* Source TCP Port from network entity */
          rx_fltr[0].next_prot_hdr.tcp.field_mask = IPFLTR_MASK_TCP_SRC_PORT;
          rx_fltr[0].next_prot_hdr.tcp.src.port   = z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_port;

        
          /* Destination TCP Port*/
          rx_fltr[0].next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
          rx_fltr[0].next_prot_hdr.tcp.dst.port   =  ((struct ps_sockaddr_in *) &local_sockaddr)->ps_sin_port;
        }
        else
        {          
          rx_fltr[0].ip_vsn = IP_V6;
          rx_fltr[0].ip_hdr.v6.field_mask = IPFLTR_MASK_IP6_SRC_ADDR | IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          rx_fltr[0].ip_hdr.v6.next_hdr_prot      = PS_IPPROTO_TCP;          

          /* Source IP address from network entity
          */
          memscpy(&(rx_fltr[0].ip_hdr.v6.src.addr), sizeof(rx_fltr[0].ip_hdr.v6.src.addr),
                 &(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr),
                 sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr)
                );
          rx_fltr[0].ip_hdr.v6.src.prefix_len = 64;


          /* Source TCP Port from network entity */          
          rx_fltr[0].next_prot_hdr.tcp.field_mask = IPFLTR_MASK_TCP_SRC_PORT;
          rx_fltr[0].next_prot_hdr.tcp.src.port   = z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_port;
          
          /* Destination TCP Port*/
          rx_fltr[0].next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
          rx_fltr[0].next_prot_hdr.tcp.dst.port   =  ((struct ps_sockaddr_in6 *) &local_sockaddr)->ps_sin6_port;

        }
      }
      else
      {
        /* Source UDP Port from network entity */
        if(z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
        {        
          rx_fltr[0].ip_vsn = IP_V4;
          rx_fltr[0].ip_hdr.v4.field_mask = IPFLTR_MASK_IP4_SRC_ADDR | IPFLTR_MASK_IP4_NEXT_HDR_PROT;          
          rx_fltr[0].ip_hdr.v4.next_hdr_prot      = PS_IPPROTO_UDP;

          /* Source IP address from network entity
           */
          rx_fltr[0].ip_hdr.v4.src.addr = z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_addr;
          rx_fltr[0].ip_hdr.v4.src.subnet_mask.ps_s_addr = dss_htonl(0xFFFFFFFF);

          /* Source UDP Port from network entity */
          rx_fltr[0].next_prot_hdr.udp.field_mask = IPFLTR_MASK_UDP_SRC_PORT;
          rx_fltr[0].next_prot_hdr.udp.src.port   = z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_port;

          /* Destination UDP Port*/
          rx_fltr[0].next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
          rx_fltr[0].next_prot_hdr.udp.dst.port   =  ((struct ps_sockaddr_in *) &local_sockaddr)->ps_sin_port;

          
          
        }
        else
        {   
          rx_fltr[0].ip_vsn = IP_V6;
          rx_fltr[0].ip_hdr.v6.field_mask = IPFLTR_MASK_IP6_SRC_ADDR | IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          rx_fltr[0].ip_hdr.v6.next_hdr_prot      = PS_IPPROTO_UDP;

          /* Source IP address from network entity
          */
          memscpy(&(rx_fltr[0].ip_hdr.v6.src.addr), sizeof(rx_fltr[0].ip_hdr.v6.src.addr),
                 &(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr),
                 sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr)
                );
          rx_fltr[0].ip_hdr.v6.src.prefix_len = 64;
      
          /* Source UDP Port from network entity */
          rx_fltr[0].next_prot_hdr.udp.field_mask = IPFLTR_MASK_UDP_SRC_PORT;
          rx_fltr[0].next_prot_hdr.udp.src.port  = z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_port;

          /* Destination UDP Port*/
          rx_fltr[0].next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
          rx_fltr[0].next_prot_hdr.udp.dst.port   =  ((struct ps_sockaddr_in6 *) &local_sockaddr)->ps_sin6_port;      
        }
      }


      /* Local port assigned by DS
      */
      /*rx_fltr[0].next_prot_hdr.tcp.dst.port = local_addr.sin_port;*/
    }
    else
    {
      if(z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
      {
        PDCOMM_APP_MSG_ERROR("src IP addr: %d, src port: %d",
                  z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_addr.ps_s_addr,
                  z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_port,
                  0);
      }
      else
      {      
        PDCOMM_APP_MSG_ERROR("src IP addr: %d, src port: %d",
                  z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr.ps_s6_addr64,
                  z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_port,
                  0);
      }
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Request Rx Filter
    -------------------------------------------------------------------------*/
    if ( dss_reg_ip_filter( z_handle_table[q_handle_id].x_sockfd, 
                            iface_id, 
                            IPFLTR_DEFAULT_TYPE, 
                            NUM_RX_FLTRS,
                            (void *)rx_fltr,
                            &dss_err_no) == DSS_ERROR )
    {
      PDCOMM_APP_MSG_ERROR("error process dss_iface_ioctl request for Rx fltr",0,0,0);

      print_dss_err(dss_err_no);
      
      if( dss_err_no == DS_EFAULT || dss_err_no == DS_EINVAL )
      {
        PDCOMM_APP_MSG_HIGH("Unable to install Rx fltr",0,0,0);
      }

      retval = FALSE;
    }
    else
    {
      PDCOMM_APP_MSG_HIGH("Rx Filter installed",0,0,0);
      retval = TRUE;
    }
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("pd_comms_tcp_task_dss_add_filter: handle_id %d out of range", q_handle_id, 0, 0);
    retval = FALSE;
  }

  return retval;
} /* pd_comms_tcp_task_dss_add_filter */

/*===========================================================================

FUNCTION PDSM_COMM_DSS_DEL_FILTER

DESCRIPTION
  Remove the IP filter with Data Services.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - Successfully removed the IP filter
  FALSE - Error

SIDE EFFECTS
  None

===========================================================================*/

boolean pd_comms_app_task_dss_del_filter( uint32 q_handle_id )
{
  dss_iface_id_type iface_id;
  sint15        dss_err_no;
  boolean       retval;
  
  if(q_handle_id < PDCOMM_MAX_HANDLES)
  {
    /*-------------------------------------------------------------------------
      Get the CDMA iface pointer instance 0
    -------------------------------------------------------------------------*/
 
    iface_id = pd_comms_tcp_task_dss_get_iface_id(q_handle_id);

    if( iface_id == DSS_IFACE_INVALID_ID )
    {
      PDCOMM_APP_MSG_ERROR("could not get iface id",0,0,0);
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Remove the IP Filter registered for this interface
    -------------------------------------------------------------------------*/
    if ( dss_dereg_ip_filter( z_handle_table[q_handle_id].x_sockfd, 
                              iface_id, 
                              &dss_err_no) == DSS_ERROR )
    {
      PDCOMM_APP_MSG_ERROR("error process Rx fltr dereg", 0, 0, 0);

      print_dss_err(dss_err_no);
      
      if( dss_err_no == DS_EFAULT )
      {
        PDCOMM_APP_MSG_HIGH("Unable to dereg Rx fltr",0,0,0);
      }

      retval = FALSE;
    }
    else
    {
      PDCOMM_APP_MSG_HIGH("Rx Filter removed",0,0,0);
      retval = TRUE;
    }  
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("pd_comms_app_task_dss_del_filter: handle_id[%d] out of range", q_handle_id, 0, 0);
    retval = FALSE;
  }

  return retval;
} /* pd_comms_app_task_dss_del_filter */

/*===========================================================================
FUNCTION pd_comms_tcp_task_close_ppp

DESCRIPTION
  This function attempts to close PPP connection for a particular handle. 
  Incase the ppp closes immediately, this function will also close the netlib.

DEPENDENCIES
  None

RETURN VALUE
  operation_status_type - status of the operation i.e. FAILED, SUCCEEDED or 
  PENDING

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type 
pd_comms_tcp_task_close_ppp(
  /*handle corresponding to the connection for which PPP is to be closed*/
  uint32 q_handle_id,
  /*returns any pdcomm disconnect error returned on close_ppp operation*/
  pd_comms_disconnect_result_type* p_disc_error)
{
  pd_comms_return_type e_return;

  if(p_disc_error == NULL)
  {
    return FAILED;
  }
  
  /* Close PPP Connection */
  PDCOMM_APP_MSG_MED("Closing PPP for handle_id [%d]",q_handle_id,0,0);
  
  e_return = pd_comms_atl_i_close(z_handle_table[q_handle_id].z_handle,FALSE);

  if(e_return == PDCOMM_RESULT_OK)
  {
    PDCOMM_APP_MSG_MED("ATL close returned SUCCESS for handle_id [%d]",0, q_handle_id, 0);    
    return SUCCEEDED;  
  }
  else if (e_return == PDCOMM_RESULT_WAIT)
  {
    PDCOMM_APP_MSG_MED("ATL close returned WAIT for handle_id [%d]",0, q_handle_id, 0);    
    return PENDING;  
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("ATL close returned ERROR for handle_id [%d]",0, q_handle_id, 0);  
    *p_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;
    return FAILED ;
  }
} /*pd_comms_tcp_task_close_ppp() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_close_socket

DESCRIPTION
  This function attempts to close socket connection for a particular handle. 
  Incase the socket closes immediately, this function will also attempt to 
  close the PPP and netlib.

DEPENDENCIES
  None

RETURN VALUE
  operation_status_type - status of the operation i.e. FAILED, SUCCEEDED or 
  PENDING

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type 
pd_comms_tcp_task_close_socket
(
  /*handle corresponding to the connection for which socket is to be closed*/
  uint32 q_handle_id,
  /*returns any pdcomm disconnect error returned on socket_close operation*/
  pd_comms_disconnect_result_type* p_disc_error,
  /*Close the PPP connection?*/
  boolean u_close_ppp
)
{
  pd_comms_tcp_task_operation_status_type e_status = SUCCEEDED;
  sint15 x_err_no;
  sint31 x_retval;
  
  if(p_disc_error == NULL)
  {
    return FAILED;
  }
  
  /* If socket is currently open, attempt to close it. */
  if( z_handle_table[q_handle_id].x_sockfd != DSS_ERROR )
  {
    /*Set SO_LINGER time to 2 secs, to ensure TCP tear down doesnt take longer than 2 secs.*/
    pd_comms_tcp_task_setsockopt(q_handle_id); 
    /* Attempt to Close Socket */
    PDCOMM_APP_MSG_MED("Closing Socket for handle_id [%d]",q_handle_id,0,0);
    x_retval = dss_close( z_handle_table[q_handle_id].x_sockfd, &x_err_no );

    if( x_retval == DSS_SUCCESS )
    {
      /* Socket closed immediately, proceed with PPP teardown */
      PDCOMM_APP_MSG_MED("Socket Closed for handle_id [%d]",q_handle_id,0,0);

      /* Nullify Socket Fd */
      z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;
      
      if(u_close_ppp)
      {
        PDCOMM_APP_MSG_MED("Closing PPP for handle_id [%d]",
                            q_handle_id,0,0);  
        e_status = pd_comms_tcp_task_close_ppp(q_handle_id,p_disc_error);
      }
    }
    else
    {
      switch ( x_err_no )
      {
        case DS_EBADF:
        {
          MSG_ERROR("PDCOMM APP: Socket Close - Bad Socket Descriptor for handle_id [%d]",
                     q_handle_id,0,0);
          e_status = FAILED;
          *p_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;
          break;
        }

        case DS_EWOULDBLOCK:
        {
          /* Socket is closing down. Enable DS_CLOSE_EVENT. When this event occurs, 
             we will finish up the close */
          PDCOMM_APP_MSG_MED("Socket Closing for handle_id [%d]",q_handle_id,0,0);
          x_retval = dss_async_select( z_handle_table[q_handle_id].x_sockfd,
                                     DS_CLOSE_EVENT ,&x_err_no );

          if(DSS_ERROR == x_retval)
          {
            print_dss_err(x_err_no);
            *p_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;
            e_status = FAILED;
          }
          else
          {
            e_status = PENDING;
          }

          break;
        }

        default:
        {
          MSG_ERROR("PDCOMM APP unknown socket close error: %d for handle_id [%d]",
                    x_err_no, q_handle_id,0 );
          e_status = FAILED;
          *p_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;
          break;
        }
      } /*switch(err_no) ends*/
    } /*else for if( retval == DSS_SUCCESS ) ends*/
  } /*if( z_handle_table[handle_id].sockfd != DSS_ERROR ) ends*/
  else
  {
    if(u_close_ppp)
    {
    
      PDCOMM_APP_MSG_MED("Socket already closed. Closing PPP for handle_id [%d]",
                         q_handle_id,0,0);
      e_status = pd_comms_tcp_task_close_ppp(q_handle_id,p_disc_error);
    }
  }
  return ( e_status );  
  
} /*pd_comms_tcp_task_close_socket() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_net_callback

DESCRIPTION
  This function is called from the context of data services when a NETWORK 
  event occurs. The function simply creates an IPC message and sends it to
  PDCOMM APP so that it can process the network event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_net_callback
(
  /*application id for the connection*/  
  sint15            x_appid,                  
  /*dss interface id (not used here)*/             
  dss_iface_id_type z_iface_id,                    
  /*type of network error, ENETISCONN, ENETNONET. */
  sint15            x_errno, 
  /*callback user data. Should contain the handle_id*/
  void            * p_net_cb_user_data               
)
{
  pd_comms_tcp_task_tcp_cb_ipc_hdr_type* p_hdr_ptr; 
  os_IpcMsgType* p_ipc_ptr;
  
  if(p_net_cb_user_data  == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL user data in net callback",0,0,0);
    return;
  }

  
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_MED,"Net callback rcvd: appid[%d] iface_id[%d] handle_id[%d] error_num[%d]",
    x_appid,z_iface_id,*((uint32*)p_net_cb_user_data), x_errno);
  
  p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_tcp_cb_ipc_hdr_type),
                         IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);
                         
  if(p_ipc_ptr == NULL)
  {
    PDCOMM_APP_MSG_HIGH("IPC Create failed during network callback. Handle Id: %d",
                        *((uint32*)p_net_cb_user_data),0,0);
  }
  else
  {
    /*lint -e826 */  
    p_hdr_ptr = (pd_comms_tcp_task_tcp_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
    /*lint +e826 */

    p_ipc_ptr->q_MsgId =  (uint32)PDCOMM_APP_TASK_DSS_NETWORK_CB;
    p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

    p_hdr_ptr->event_mask = x_errno;
    p_hdr_ptr->handle.q_handle_id = *((uint32*)p_net_cb_user_data);
    p_hdr_ptr->x_appid = x_appid;

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
    {
      PDCOMM_APP_MSG_HIGH("IPC Send failed during network callback. Handle Id: %d",
                           *((uint32*)p_net_cb_user_data),0,0);
      (void)os_IpcDelete(p_ipc_ptr);
    }
  }


} /* pd_comms_app_net_callback() ends*/    /*lint !e715 !e818 */


/*===========================================================================
FUNCTION pd_comms_tcp_task_socket_callback

DESCRIPTION
  This function is called from the context of data services when a  SOCKET
  event occurs. The function simply creates an IPC message and sends it to
  PDCOMM APP so that it can process the socket event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_socket_callback
(
  /*application id for the connection*/  
  sint15 x_appid,            
  /*socket descriptor*/                              
  sint15 x_sockfd, 
  /*socket events that occured (DS_CLOSE_EVENT,DS_READ_EVENT,DS_WRITE_EVENT) */                               
  uint32 q_event_mask,                                     
  /*callback user data. Should contain the handle_id*/
  void * p_sock_cb_user_data       
)
{
  pd_comms_tcp_task_tcp_cb_ipc_hdr_type* p_hdr_ptr; 
  os_IpcMsgType* p_ipc_ptr;

  if(p_sock_cb_user_data  == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL user data in socket callback",0,0,0);
    return;
  }

  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_MED,"Socket callback rcvd: appid[%d] socket[%d] handle_id[%d] event_mask[%d]",
    x_appid,x_sockfd,*((uint32*)p_sock_cb_user_data), q_event_mask);

  p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_tcp_cb_ipc_hdr_type),
                         IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);
                         
  if(p_ipc_ptr == NULL)
  {
    PDCOMM_APP_MSG_HIGH("IPC Create failed during socket callback. Handle Id: %d",
                         *((uint32*)p_sock_cb_user_data),0,0);
  }
  else
  {
    /*lint -e826 */  
    p_hdr_ptr = (pd_comms_tcp_task_tcp_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
    /*lint +e826 */
    p_ipc_ptr->q_MsgId = (uint32)PDCOMM_APP_TASK_DSS_SOCKET_CB;
    p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

    p_hdr_ptr->handle.q_handle_id = *((uint32*)p_sock_cb_user_data);
    p_hdr_ptr->x_appid = x_appid;
    p_hdr_ptr->x_sockfd = x_sockfd;
    p_hdr_ptr->event_mask = q_event_mask;

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
    {
      PDCOMM_APP_MSG_HIGH("IPC Send failed during socket callback. Handle Id: %d",
                          *((uint32*)p_sock_cb_user_data),0,0);
      (void)os_IpcDelete(p_ipc_ptr);
    }
  }
} /*lint !e715 !e818 */ 
/*pd_comms_tcp_task_socket_callback*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_dss_dns_callback

DESCRIPTION
  This function defines the callback for the dss based dns lookup operations.
  It is called from the context of data services when a DNS 
  event occurs. The function simply creates an IPC message and sends it to
  PDCOMM APP so that it can process the DNS result

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_dss_dns_callback
(
  dss_dns_session_mgr_handle_type   dns_session_handle,
  dss_dns_query_handle_type         query_handle,
  dss_dns_api_type_enum_type        api_type,
  uint16                            w_num_records,
  void                            * p_user_data_ptr,
  int16                             x_dss_errno
)
{
  pd_comms_tcp_task_handle_data_type* p_handle_data_ptr;
  uint32 q_handle_id;

  if(p_user_data_ptr == NULL)
  {
     MSG_ERROR("pd_comms_tcp_task_dss_dns_callback: user_data_ptr is NULL!! Rejecting results", 0, 0, 0);
     return;
  }

  /* user_data_ptr is actually a pointer to the connection handle */
  p_handle_data_ptr = (pd_comms_tcp_task_handle_data_type *)p_user_data_ptr;

  q_handle_id = p_handle_data_ptr->z_handle.q_handle_id;

  if( q_handle_id < PDCOMM_MAX_HANDLES ) 
  {
    pd_comms_tcp_task_tcp_cb_ipc_hdr_type* p_hdr_ptr; 
    os_IpcMsgType* p_ipc_ptr;

    PDCOMM_APP_MSG_MED("DNS callback for handle_id [%d], x_dss_errno = %d",
                        q_handle_id, x_dss_errno, 0);
  
    /*Make sure if the DNS callback is not for an earlier query for some other session*/
    if( (z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_CONNECTING) ||
        (z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle != dns_session_handle)||
      (z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle != query_handle) )
    {
      PDCOMM_APP_MSG_MED("DNS callback for a stale session. DNS_Session:0x%x, DNS_query:0x%x",
                q_handle_id, dns_session_handle,query_handle);
      return;
    }
    
    /* Copy the dns handle and other information so that we can use it later in other functions */ 
    /*
      we have 3 pieces of info to pass back to pdcomms:
      1. dns handle
      2. query handle
      3. error information      
      we transfer this information by directly copying it to the handle table
    */

    z_handle_table[q_handle_id].z_dns_cb_data.x_dss_errno = x_dss_errno;

    /* this is executing on the PS context; so send an IPC message to PD_COMMS 
       the only information pass through the IPC is the handle number; everything
       else is assigned directly in the handle table (in the previous step)
    */
    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_tcp_cb_ipc_hdr_type),
                           IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);             

    if(p_ipc_ptr == NULL)
    {
      PDCOMM_APP_MSG_HIGH("IPC Create failed during DNS callback <unable to notify pdcomms from DS thread!>. Handle_id: %d",
                      q_handle_id,0,0);
    }
    else
    {
      /*lint -e826 */  
      p_hdr_ptr = (pd_comms_tcp_task_tcp_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
      /*lint +e826 */
      p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_DSS_DNS_CB;
      p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;
      p_hdr_ptr->handle.q_handle_id = q_handle_id;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
        PDCOMM_APP_MSG_HIGH("IPC Send failed during DNS callback. Handle Id: %d",
                            q_handle_id,0,0);
        (void)os_IpcDelete(p_ipc_ptr);
      }
    }
  }
  else
  {
    MSG_ERROR("Socket callback: handle_id %d out of range", q_handle_id, 0, 0);
  }
} /* pd_comms_tcp_task_dss_dns_callback ends */ /*lint !e818 */

/*===========================================================================
FUNCTION pd_comms_tcp_task_dns_lookup

DESCRIPTION
  This function initates a DNS lookup

DEPENDENCIES
  None

RETURN VALUE
  operation_status_type - status of the operation (FAILED, SUCCEDDED or 
  PENDING)

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type 
pd_comms_tcp_task_dns_lookup(
  /*handle_id corresponding to the connection for which DNS lookup is to be performed*/  
  uint32 q_handle_id,
  /*(pdcomm specific error is returned through this pointer*/
  pd_comms_connect_result_type* p_conn_error)
{
  pdsm_atl_dns_params_type  dns_lookup_params; /* dns params in one package */

  if(p_conn_error == NULL)
  {
    return FAILED;
  }

  PDCOMM_APP_MSG_MED( "DNS query for handle_id [%d]...", q_handle_id, 0, 0 );

  /* DNS Lookup, get IP address of host
  */

    /*
     *  Convert the IP Address into a "string" and store in hslp_address
     *  since there are places in the code that expect hslp_address to
     *  contain a valid string.
     */

  
  (void) GNSS_STRLCPY(dns_lookup_params.url, z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.p_host_name , sizeof(dns_lookup_params.url));

  return pd_comms_atl_i_dns_lookup(z_handle_table[q_handle_id].z_handle,  
               &dns_lookup_params, p_conn_error); 
}        /* pd_comms_tcp_task_dns_lookup() ends*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_dft_atl_dns_lookup

DESCRIPTION
  This function initates a DNS lookup. It is used by default when no ATL
  service has been registered.

DEPENDENCIES
  None

RETURN VALUE
  operation_status_type - status of the operation (FAILED, SUCCEDDED or 
  PENDING)

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type 
pd_comms_tcp_task_dft_atl_dns_lookup(
  /*handle_id corresponding to the connection for which DNS lookup is to be performed*/  
  uint32 q_handle_id,
  /*(pdcomm specific error is returned through this pointer*/
  pd_comms_connect_result_type* p_conn_error)
{
  dss_dns_session_mgr_handle_type  dns_session_handle;
  int16                            dss_errno = 0;
  /* dss_iface_id_type                z_iface_id; */  /* interface on which to perform DNS lookup
                                                      ommited since lookup is now based on policy */
  int16                            x_retval;
  dss_dns_query_handle_type        query_handle;
  dss_net_policy_info_type         z_policy_info; 
  dss_dns_linear_backoff_retry_info_type dns_linear_backoff_info;  
  static dss_dns_addrinfo          z_dss_dns_addrinfo;
  uint32 pdp_type = PDSM_PA_APN_PROFILE_PDN_TYPE_MAX;

  if(p_conn_error == NULL)
  {
    return FAILED;
  }

  PDCOMM_APP_MSG_MED( "Handling DNS query for handle_id [%d] within modem ...", q_handle_id, 0, 0 );

  /*  We create a session with pd_comms_tcp_task_dss_dns_callback as the callback. */
  dns_session_handle = dss_dns_create_session(pd_comms_tcp_task_dss_dns_callback, (void *) &z_handle_table[q_handle_id], &dss_errno );
  if(DSS_DNS_SESSION_MGR_INVALID_HANDLE == dns_session_handle )
  {
    MSG_ERROR("Session could not be created error %d", dss_errno, 0, 0);
    return FAILED;
  }

  /* Set the IFACE_ID config parameter for this session. */

  /*---------------------------------------------------------------------------------
  Get policy on which to perform DNS:
  The only time we 'force' a policy is when a laptop call is up and there is no ATL.
  This procedure for choosing the policy is identical to how it is done during socket 
  creation, as it should be!
  --------------------------------------------------------------------------------*/
  pd_comms_tcp_task_dss_get_policy(&z_policy_info, q_handle_id);
  PDCOMM_APP_MSG_MED("DNS on modem: using pd_comms_tcp_task_dss_get_policy to identify policy", 0,0,0);
  /* Now get the interface based on the policy */

  x_retval = dss_dns_set_config_params(dns_session_handle,
                                         DSS_DNS_CONFIG_PARAMS_NET_POLICY,
                                         (void *) &z_policy_info,
                                         sizeof(z_policy_info),
                                    &dss_errno );
  if( DSS_ERROR == x_retval )
  {
    MSG_ERROR("Set config param returned error %d", dss_errno, 0 ,0);

  /*Delete DNS session*/
  x_retval = dss_dns_delete_session(dns_session_handle, &dss_errno);
    if( DSS_ERROR == x_retval )
    {
      MSG_ERROR("DNS session %d could not be deleted", dns_session_handle,0,0);
    }
  
    return FAILED;
  }

  /* Set the ROUTEABLE_IFACE_MODE config parameter for this session. */
  /* 
     The following line is not reqd since while creating the policy info, we set:
     "p_policy_info->is_routeable = TRUE" 
  */
  /*
  x_retval = dss_dns_set_config_params(dns_session_handle,
                                    DSS_DNS_CONFIG_PARAMS_ROUTEABLE_IFACE_MODE,
                                    (void *) &b_routeable_iface_mode,
                                    sizeof(boolean),
                                    &dss_errno);
  */

  dns_linear_backoff_info.first_retry_time = DNS_DEFAULT_FIRST_QUERY_TIMEOUT;
  dns_linear_backoff_info.backoff = DNS_DEFAULT_RETRIES_BACKOFF;
  dns_linear_backoff_info.max_retries = DNS_DEFAULT_MAX_QUERY_RETRIES;
  
  x_retval = dss_dns_set_config_params(dns_session_handle,
                                  DSS_DNS_CONFIG_PARAMS_LINEAR_BACKOFF_RETRY_INFO,
                                  (void *) &dns_linear_backoff_info,
                                  sizeof(dss_dns_linear_backoff_retry_info_type),
                                  &dss_errno);

  if( DSS_ERROR == x_retval )
  {
    MSG_ERROR("Set config param DNS Linear Backoff returned error %d", dss_errno,0,0);
  }

  /*Get PDP Type to set the correct IP family for DNS query*/
  pd_comms_tcp_get_pdp_type(q_handle_id,&pdp_type);

  MSG_MED("PDP_TYPE DNS:%d ,Z_Policy IP Family:%d",pdp_type,z_policy_info.family,0); 
  
  if(pdp_type == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4)
  {
    z_dss_dns_addrinfo.ai_family = DSS_AF_INET;
  }
  else if(pdp_type == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6)
  {
    z_dss_dns_addrinfo.ai_family = DSS_AF_INET6;
  }
  else if(pdp_type == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4V6)
  {
    /*Iface Familiy to use is determined in pd_comms_tcp_dss_get_policy and saved in z_policy_info*/
    z_dss_dns_addrinfo.ai_family = z_policy_info.family;
  }
  else
  { 
    z_dss_dns_addrinfo.ai_family = DSS_AF_INET;
  }

  MSG_HIGH("DNS lookup request for Ip family %d pdp_type %d", z_dss_dns_addrinfo.ai_family, pdp_type,0);
  
  /* Invoke DNS resolver using dss_dns_get_addrinfo */
  query_handle = dss_dns_get_addrinfo( dns_session_handle,
                                     z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.p_host_name,
                                     NULL,
                                     &z_dss_dns_addrinfo,
                                     &dss_errno );
  if( DSS_DNS_QUERY_INVALID_HANDLE == query_handle )
  {
    MSG_ERROR("Get_addrinfo returned error %d", dss_errno,0,0);

  /*Delete DNS session*/
  x_retval = dss_dns_delete_session(dns_session_handle, &dss_errno);
    if( DSS_ERROR == x_retval )
    {
      MSG_ERROR("DNS session %d could not be deleted", dns_session_handle,0,0);
    }

    return FAILED;
  }
  
  /*Update the handle_table with DNS session info*/
  z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle = dns_session_handle;
  z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle = query_handle;
  
  /* If we have gotten here, then all went well. Results will be returned
     through the callback. For now just return PENDING */
  PDCOMM_APP_MSG_MED("Issued modem DNS lookup request: DNS_Session:0x%x, DNS_query:0x%x", dns_session_handle,query_handle,0);

  return PENDING;

} /* pd_comms_tcp_task_dft_atl_dns_lookup ends*/

/*===========================================================================
FUNCTION pd_comms_tcp_set_sys_sock_option

DESCRIPTION
  This function sets sys sock option. For binding to reserved ports, setting this 
  options is necessary. It is optional (not harm if done) to bind to non-reserved 
  ports

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_tcp_task_operation_status_type - status of the operation (FAILED, SUCCEDDED or 
  PENDING)

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type pd_comms_tcp_set_sys_sock_option(uint32 q_handle_id)
{
	sint31 x_retval;	 /* used to hold the return value from dss functions */
	sint15 x_err_no 	   = DS_ENOTCONN;

	uint32 sys_sock= 1;
	uint32 q_len = sizeof(uint32);

	if(z_handle_table[q_handle_id].x_sockfd == DSS_ERROR)
	{
		PDCOMM_APP_MSG_ERROR( "sockfd == DSS_ERROR while sys_sock", 0, 0, 0);
		return FAILED;
	}
	
	x_retval =	dss_setsockopt( z_handle_table[q_handle_id].x_sockfd,
								DSS_SOCK,
								DSS_SO_SYS_SOCK,
								&sys_sock,
								&q_len,
								&x_err_no );
	
	 if( x_retval == DSS_ERROR )
	 {
	   print_dss_err( x_err_no );
	   return FAILED;
	 }
	 else
	 {
	   PDCOMM_APP_MSG_MED( "DSS_SO_SYS_SOCK handle_id[%d] sockfd: %d",
				 q_handle_id, z_handle_table[q_handle_id].x_sockfd, 0);
	   return SUCCEEDED;
	 }

}

/*===========================================================================
FUNCTION pd_comms_tcp_bind_to_local_port

DESCRIPTION
  This function binds to local port.

DEPENDENCIES
  None

RETURN VALUE
  pd_comms_tcp_task_operation_status_type - status of the operation (FAILED, SUCCEDDED or 
  PENDING)

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type pd_comms_tcp_bind_to_local_port(uint32 q_handle_id)
{
	sint15 x_err_no 	   = DS_ENOTCONN;
        uint16 local_adr_size = 0;

	if(z_handle_table[q_handle_id].x_sockfd == DSS_ERROR)
	{
		PDCOMM_APP_MSG_ERROR( "sockfd == DSS_ERROR while binding to local port", 0, 0, 0);
		return FAILED;
	}

	if(z_handle_table[q_handle_id].ip_addr_type ==PD_COMMS_IP_ADDR_IPV4)
	{
		struct ps_sockaddr_in local_addr;					
		/*-------------------------------------------------------------------------
			Explicitly bind the local IP address and port number.
		-------------------------------------------------------------------------*/
		memset((void *)&(local_addr), 0, sizeof(local_addr));
		local_addr.ps_sin_family = DSS_AF_INET;
		local_addr.ps_sin_addr.ps_s_addr = PS_INADDR_ANY;		
                local_addr.ps_sin_port = 0; 

	        if ( DSS_ERROR == dss_bind( z_handle_table[q_handle_id].x_sockfd,
								(struct ps_sockaddr *)&local_addr,
								sizeof(local_addr),
								&x_err_no) )
		{
	           PDCOMM_APP_MSG_MED("Error: Bind Socket to local port for handle[%d]", q_handle_id, 0, 0);
		   print_dss_err(x_err_no);
	           return FAILED;
	        }
	        else
		{
                   memset((void *)&(local_addr), 0, sizeof(local_addr));
                   if(DSS_ERROR == dss_getsockname(z_handle_table[q_handle_id].x_sockfd,
                                      (struct ps_sockaddr *)&local_addr,
                                      &local_adr_size,
                                      &x_err_no))
                   {
                      PDCOMM_APP_MSG_MED("dss_getsocketname: Failed",0,0,0);
                      print_dss_err(x_err_no);
                   }         
                   else
                   {
                      PDCOMM_APP_MSG_HIGH("Socket succesfully bound to DS assigned port %d for handle[%d]",
                                          (((struct ps_sockaddr_in *) &local_addr)->ps_sin_port),q_handle_id , 0); 	
                   }
	        }
		
	}
	else
	{
		struct ps_sockaddr_in6		 local_addr;		
		struct ps_in6_addr ps_in6addr_any = PS_IN6ADDR_ANY_INIT;		
		/*-------------------------------------------------------------------------
		   Explicitly bind the local IP address and port number.
		-------------------------------------------------------------------------*/
		memset((void *)&(local_addr), 0, sizeof(local_addr));
		
		local_addr.ps_sin6_family = DSS_AF_INET6;
		local_addr.ps_sin6_addr = ps_in6addr_any;
                local_addr.ps_sin6_port = 0; 

                if ( DSS_ERROR == dss_bind( z_handle_table[q_handle_id].x_sockfd,
								(struct ps_sockaddr *)&local_addr,
								sizeof(local_addr),
								&x_err_no) )
		{
	           PDCOMM_APP_MSG_ERROR("Error: Bind Socket to local port for handle[%d]", q_handle_id, 0, 0);
	           print_dss_err(x_err_no);
		   return FAILED;
	        }
		else
		{
                   memset((void *)&(local_addr), 0, sizeof(local_addr));
                   if(DSS_ERROR == dss_getsockname(z_handle_table[q_handle_id].x_sockfd,
                                      (struct ps_sockaddr *)&local_addr,
                                      &local_adr_size,
                                      &x_err_no))
                   {
                      PDCOMM_APP_MSG_MED("dss_getsocketname: Failed",0,0,0);
                      print_dss_err(x_err_no);
                   }              
                   else
                   {
                      PDCOMM_APP_MSG_MED("Socket succesfully bound to DS assigned port %d for handle[%d]",
                                         (((struct ps_sockaddr_in6 *) &local_addr)->ps_sin6_port),q_handle_id, 0); 	
                   }
		}
	}/*If Ipv6*/
  	   
  return SUCCEEDED;
}

/*===========================================================================
FUNCTION pd_comms_tcp_task_connect_socket

DESCRIPTION
  This function attempts to create a new socket and calls connect on that
  socket

DEPENDENCIES
  None

RETURN VALUE
  operation_status_type - status of the operation (FAILED, SUCCEDDED or 
  PENDING)

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type 
pd_comms_tcp_task_connect_socket(
  /*handle_id corresponding to the connection for which socket connection is to be made*/  
  uint32 q_handle_id, 
  /*pdcomm specific error is returned through this pointer */
  pd_comms_connect_result_type* p_conn_error)
{
  sint31 x_retval;     /* used to hold the return value from dss functions */
  sint15 x_err_no        = DS_ENOTCONN;
  sint31 l_select_mask;    /* used to select events for async notification */
  uint8  p_ipaddr_str[16]; /*to get ip-address string*/
  dss_net_policy_info_type z_policy_info;
  char* tmp_addr_p = NULL;
  int32 af = DSS_AF_INET;
  struct ps_sockaddr *sock_addr = NULL;
  uint16 sock_addr_len =0;
  
  /* Create socket and Open connection to Host 
  */

  if(p_conn_error == NULL)
  {
    return FAILED;
  }
  
  if(DSS_ERROR == z_handle_table[q_handle_id].x_sockfd)
  {

    /*-------------------------------------------------------------------------
      Creates a TCP streaming socket.  If an error occured sockfd will be set
      to DSS_ERROR and we can determine what was wrong.
    -------------------------------------------------------------------------*/
    PDCOMM_APP_MSG_MED( "Creating socket for handle id [%d]", q_handle_id, 0, 0);
    
    /*z_handle_table[q_handle_id].x_sockfd 
              = dss_socket(z_handle_table[q_handle_id].x_app_id,
                           (byte)AF_INET,
                           SOCK_STREAM,
                           (byte)IPPROTO_TCP,
                           &x_err_no);       */

 
    /* Change made to use no DS API to set policy (i.e. use 1x CDMA only) */
      pd_comms_tcp_task_dss_get_policy(&z_policy_info, q_handle_id);

    if(z_handle_table[q_handle_id].ip_addr_type ==PD_COMMS_IP_ADDR_IPV4)
    {
      af = DSS_AF_INET;
    }
    else
    {
      af = DSS_AF_INET6;
    }
    

    if(z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_UDP)
    {
      z_handle_table[q_handle_id].x_sockfd 
                = dss_socket2( af,
                               DSS_SOCK_DGRAM,
                               PS_IPPROTO_UDP,
                               pd_comms_tcp_task_socket_callback,
                               (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id),
                               &z_policy_info,
                               &x_err_no);
    }
    else if(z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_TCP)
    {
      z_handle_table[q_handle_id].x_sockfd 
                = dss_socket2( af,
                               DSS_SOCK_STREAM,
                               PS_IPPROTO_TCP,
                               pd_comms_tcp_task_socket_callback,
                               (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id),
                               &z_policy_info,
                               &x_err_no);
    }
    else
    {
      z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;      
    }
    
    if(DSS_ERROR == z_handle_table[q_handle_id].x_sockfd)
    {
      print_dss_err(x_err_no);
      *p_conn_error = PDCOMM_CONNECT_ERROR_SOCKET_CONNECTION_FAILED;
      return FAILED;
    }
    else
    {      
      /*Set SysSocket option*/
      pd_comms_tcp_set_sys_sock_option(q_handle_id);

  		/*Bind to local port*/		
  		if(pd_comms_tcp_bind_to_local_port(q_handle_id) != SUCCEEDED)
  		{
  			return FAILED;
  		}
    }


    if(z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_TCP)
    {
      uint32 no_delay = 1;
      uint32 q_len = sizeof(uint32);
      
      x_retval =  dss_setsockopt( z_handle_table[q_handle_id].x_sockfd,
                                    (int)DSS_IPPROTO_TCP,
                                    (int)DSS_TCP_NODELAY,
                                    &no_delay,
                                    &q_len,
                                    &x_err_no );

      if(x_retval != DSS_ERROR)
      {
        MSG_HIGH("TCP NO DELAY set",0,0,0);
      }
      else
      {
        MSG_HIGH("TCP NO DELAY set failed",0,0,0);
      }
    }

    /*-------------------------------------------------------------------------
      Connect to the specified IP Address or Host.  
    -------------------------------------------------------------------------*/
    /*Check whether Ipv4 or Ipv6 is used and proceed accordingly*/
    if(z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
    {
      tmp_addr_p = (char*)&(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4);
      sock_addr = (struct ps_sockaddr *)&(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4);
      sock_addr_len = sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4);
    }
    else
    {    
      tmp_addr_p = (char*)&(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v6);
      sock_addr = (struct ps_sockaddr *)&(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6);
      sock_addr_len = sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6);
    }
    
    if( dss_inet_ntop((const char *)tmp_addr_p,af,p_ipaddr_str,sizeof(p_ipaddr_str),&x_err_no) == DSS_SUCCESS)
    {
      PDCOMM_APP_MSG_HIGH( "Connect to port = %d and IP:",
                           z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port,
                           0,0);
      if( msg_status( MSG_SSID_DFLT, MSG_LEGACY_HIGH ) )
      {
        MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "%s", p_ipaddr_str);
      }
    }
    else
    {
      PDCOMM_APP_MSG_HIGH( "Connect to IP = %x, port = %d", 
                           tmp_addr_p,
                           z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port, 0);
    }

    /*-------------------------------------------------------------------------
        Install the IP filter before connecting to network server
      -------------------------------------------------------------------------*/
    if(!pd_comms_tcp_task_dss_add_filter(q_handle_id))
    {
      PDCOMM_APP_MSG_ERROR("Failed installing IP filter for handle[%d]. Trying to connect",q_handle_id,0,0);
    }
    if(z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_TCP)
    {
      /*lint -e740*/
      x_retval = dss_connect( z_handle_table[q_handle_id].x_sockfd, 
                            sock_addr,
                            sock_addr_len,
                            &x_err_no );
    }
    else
    {
      /* UDP sockets dont need to connect */
      x_retval = DSS_SUCCESS;
    }

    /*lint +e740*/
    
    if((x_retval == DSS_SUCCESS) || ((x_retval == DSS_ERROR) && (x_err_no == DS_EISCONN))) 
    {
      PDCOMM_APP_MSG_MED("Socket connected without wait for handle_id [%d]",
                         q_handle_id,0,0);

      /*Socket successfully connected. Register to receive SOCKET READ and CLOSE events*/
      /*MR: need to add condition here for NO_SSL*/
      l_select_mask = DS_READ_EVENT | DS_CLOSE_EVENT;
      x_retval = dss_async_select(z_handle_table[q_handle_id].x_sockfd,
                                l_select_mask, &x_err_no);
      
      if(DSS_ERROR == x_retval)
      {
        print_dss_err(x_err_no);
        *p_conn_error = get_pdcomm_connect_error_from_dss_error(x_err_no);
        return FAILED;
      }
      else
      {
        return SUCCEEDED;
      }

    }/*if((x_retval == DSS_SUCCESS) || ((x_retval == DSS_ERROR) && (x_err_no == DS_EISCONN))) */
    else if((DSS_ERROR == x_retval) && (x_err_no != DS_EWOULDBLOCK) && (x_err_no != DS_EINPROGRESS))
    {
      print_dss_err(x_err_no);
      *p_conn_error = get_pdcomm_connect_error_from_dss_error(x_err_no);
      return FAILED;
    } /* end if retval */

  } /* end if sockfd */

  /*-------------------------------------------------------------------------
    The error is not an error that prevents going forward.  It is one of:
    DS_EWOULDBLOCK (async return) or DS_EINPROGRESS (connection establishment
    in progress).  Set signal pointer to wait for socket event.
  -------------------------------------------------------------------------*/
  l_select_mask = DS_WRITE_EVENT | DS_CLOSE_EVENT;
  x_retval = dss_async_select(z_handle_table[q_handle_id].x_sockfd, l_select_mask, &x_err_no);

  if(DSS_ERROR == x_retval)
  {
    print_dss_err(x_err_no);
    *p_conn_error = get_pdcomm_connect_error_from_dss_error(x_err_no);
    return FAILED;
  }

  return PENDING;
} /* end pd_comms_tcp_task_connect_socket()*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_open_socket

DESCRIPTION
  This function attempts to open a TCP socket. If IP address is not available 
  in connect params, it will initiate a DNS query first.

DEPENDENCIES
  None

RETURN VALUE
  operation_status_type - status of the operation (FAILED, SUCCEDDED or 
  PENDING)

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type 
pd_comms_tcp_task_open_socket(
  /*handle_id corresponding to the connection for which socket is to be opened*/  
  uint32 q_handle_id,
  /*a pdcomm_connect_result_type result/error is returned in this pointer*/
  pd_comms_connect_result_type* p_conn_error)
{
    
  if(p_conn_error == NULL)
  {
    return FAILED;
  }
  
  *p_conn_error = PDCOMM_CONNECT_ERROR_UNKNOWN; /*default error state*/

  if( q_handle_id < PDCOMM_MAX_HANDLES)
  {
    PDCOMM_APP_MSG_MED("Calling DSS Connect (handle_id = %d, link state = %d)", 
            q_handle_id, z_handle_table[q_handle_id].e_link_status, 0); 
   
    if((z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type == PD_COMMS_IP_ADDR) ||
     (z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type == PD_COMMS_HOST_NAME_AND_IP_ADDR))
    {
      z_handle_table[q_handle_id].ip_addr_type = z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.ip_addr_type ;
      
      if(z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
      {
        z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_family = (uint16)DSS_AF_INET;
        z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_addr.ps_s_addr
                = dss_htonl(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4);
        
        z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_port
                = dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);
        
        PDCOMM_APP_MSG_MED("PDCOMM_NO_DNS: HSLP Params = %d, %08X",
                    (int) z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type,
                    z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_addr.ps_s_addr,
                    0 );
      }
      else
      {
        int index = 0;

        z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_family = (uint16)DSS_AF_INET6;

        for(index = 0; index < 8 ; index ++)
        {
        z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr.ps_s6_addr16[index] =
          dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v6.ps_s6_addr16[index]);
        }
                 
        z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_port
                = dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);

        
        PDCOMM_APP_MSG_MED("PDCOMM_NO_DNS: HSLP Params = %d, %X",
                    (int) z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type,
                    z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr.ps_s6_addr64[0],
                    z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr.ps_s6_addr64[1] );
      }
      
    }
    else if(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type == PD_COMMS_HOST_NAME)
    {
      /*do a DNS lookup*/
      pd_comms_tcp_task_operation_status_type e_status;

#if 0
      z_handle_table[q_handle_id].z_peer_addr.ps_sin_family = (uint16)DSS_AF_INET;
      z_handle_table[q_handle_id].z_peer_addr.ps_sin_port
              = dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);
#endif

      PDCOMM_APP_MSG_MED("PDCOMM_DNS: HSLP Params = %d, %s",
                  (int) z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type,
                  z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.p_host_name,
                  0 );
      
      e_status = pd_comms_tcp_task_dns_lookup(q_handle_id,p_conn_error);

      if(e_status != SUCCEEDED)/*i.e if DNS lookup failed or is pending*/
      {
        return e_status;
      }

    }
    else
    {
      MSG_ERROR("Invalid TCP ADDR TYPE specified in connect request for handle_id [%d]",
                q_handle_id,0,0);
      *p_conn_error = PDCOMM_CONNECT_ERROR_INVALID_PARAMETERS;
      return FAILED;
    }

    return pd_comms_tcp_task_connect_socket(q_handle_id,p_conn_error);
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    MSG_ERROR("CONNECT: handle_id %d out of range", q_handle_id, 0, 0);
    return FAILED;
  } 

} /* end pd_comms_tcp_task_open_socket()*/ 


/*===========================================================================
FUNCTION pd_comms_tcp_task_get_app_id

DESCRIPTION
 Opens NETLIB and returns the DS app_id
DEPENDENCIES
  None

RETURN VALUE
  DS APP ID
SIDE EFFECTS
  None
===========================================================================*/
sint15 pd_comms_tcp_task_get_app_id(uint32           q_handle_id)
{
  sint15 x_err_no;            /* error value as returned from dss functions */
  sint15 x_retval;      /* used to hold the return value from dss functions */
  
  
  /*-------------------------------------------------------------------------
    Open up network library, setting up the network and socket level
    callback functions and obtain our app_id.  If an error occured app_id
    will be set to DSS_ERROR, check to see what the error was.
  -------------------------------------------------------------------------*/
    PDCOMM_APP_MSG_MED( "Opening Netlib.", 0, 0, 0);
    x_retval
        = dsnet_get_handle( pd_comms_tcp_task_net_callback, 
                            (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id),
                            pd_comms_tcp_task_socket_callback,
                            (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id),           
                            NULL,
                            &x_err_no );

    if(DSS_ERROR == x_retval)
    {
      print_dss_err(x_err_no);
      return DSS_ERROR;
    }
    else
    {
      PDCOMM_APP_MSG_MED("Netlib open with app_id: %d",x_retval,0,0);
      return x_retval;
    }
}/*pd_comms_tcp_task_get_app_id*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_release_app_id

DESCRIPTION
 Closes NETLIB and releases the app_id
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_release_app_id(sint15           x_app_id)
{
  sint15 x_err_no;            /* error value as returned from dss functions */
  sint15 x_retval;      /* used to hold the return value from dss functions */
  
  PDCOMM_APP_MSG_MED( "Closing Netlib.", 0, 0, 0);
  /* Close Network Library */
  x_retval = dss_close_netlib( x_app_id, &x_err_no );

  if(x_retval == DSS_ERROR )
  {
    MSG_ERROR("PDCOMM APP: Close Netlib: Error %d for app_id [%d]",
               x_err_no, x_app_id, 0);
  }
  else
  {
    PDCOMM_APP_MSG_MED("Netlib Closed for app_id [%d]", x_app_id,0,0);
  }
  
}/*pd_comms_tcp_task_release_app_id*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_dft_atl_open

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_tcp_task_dft_atl_open
(
  pd_comms_handle_type           z_handle,
  pd_comms_atl_open_params_type* z_open_params 
)
{
  sint15 x_err_no;            /* error value as returned from dss functions */
  sint15 x_retval;      /* used to hold the return value from dss functions */
  dss_net_policy_info_type  z_policy_info;
  dss_net_policy_info_type  p_policy_info;
  uint8 ip_family = PDSM_PA_APN_PROFILE_PDN_TYPE_MAX;
  const char *apn_name = NULL;
  int profile_num; 

  if(u_pd_comms_data_enabled == FALSE)
  {
    return PDCOMM_RESULT_ERROR_DATA_DISABLED;
  }
  
  /*-------------------------------------------------------------------------
    Open up network library, setting up the network and socket level
    callback functions and obtain our app_id.  If an error occured app_id
    will be set to DSS_ERROR, check to see what the error was.
  -------------------------------------------------------------------------*/
  /*If app_id is valid that means netlib is already open and PPP is connected*/
  if(DSS_ERROR == z_handle_table[z_handle.q_handle_id].x_app_id)
  {

    ds_profile_tech_etype tech_type;
    uint32 pdp_type = PDSM_PA_APN_PROFILE_PDN_TYPE_MAX;
    
    
    PDCOMM_APP_MSG_MED( "Opening Netlib.", 0, 0, 0);

    pd_comms_tcp_task_dss_get_policy(&z_policy_info, z_handle.q_handle_id);

    /*Get PDP Type to set the correct IP family for SLP connection*/
    pd_comms_tcp_get_pdp_type(z_handle.q_handle_id,&pdp_type);
    
    MSG_MED("PDP_TYPE SLP connection:%d ,Z_Policy IP Family:%d",pdp_type,z_policy_info.family,0);

    if((z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type == PD_COMMS_IP_ADDR) ||
     (z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type == PD_COMMS_HOST_NAME_AND_IP_ADDR))
    {      
      if(z_handle_table[z_handle.q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
      {
        /*Ipv4 address is provided, start with Ipv4 data family if the profile configured
          is not Ipv6*/
        if(pdp_type != PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6)
        {
          z_handle_table[z_handle.q_handle_id].datacall_family = DSS_AF_INET;
        }
        else
        {
          z_handle_table[z_handle.q_handle_id].datacall_family = DSS_AF_INET6;
        }
      }
      else
      {      
       /*Ipv6 address is provided, start with Ipv6 data family if the profile configured
        is not Ipv4*/
        if(pdp_type != PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4 )
        {
          z_handle_table[z_handle.q_handle_id].datacall_family = DSS_AF_INET6;
        }
        else
        {
          z_handle_table[z_handle.q_handle_id].datacall_family = DSS_AF_INET;
        }
      }
    }
    else /*Hostname is provided*/
    {
      /*Hostname is provided, start with Ipv4 data family if the profile configured is not Ipv6*/
      if(pdp_type == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6)
      {
        z_handle_table[z_handle.q_handle_id].datacall_family = DSS_AF_INET6;
      }
      else if (pdp_type == PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4) 
      {
        z_handle_table[z_handle.q_handle_id].datacall_family = DSS_AF_INET;
      }
      else
      {
        /*Iface Familiy to use is determined in pd_comms_tcp_dss_get_policy and saved in z_policy_info*/
        z_handle_table[z_handle.q_handle_id].datacall_family = z_policy_info.family;
      }

    }

    z_policy_info.family = z_handle_table[z_handle.q_handle_id].datacall_family;

    z_handle_table[z_handle.q_handle_id].x_app_id
        = dsnet_get_handle( pd_comms_tcp_task_net_callback, 
                            (void *)(&z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id),
                            pd_comms_tcp_task_socket_callback,
                            (void *)(&z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id),           
                            &z_policy_info,
                            &x_err_no );

    if(DSS_ERROR == z_handle_table[z_handle.q_handle_id].x_app_id)
    {
      print_dss_err(x_err_no);
      return PDCOMM_RESULT_ERROR_UNKNOWN;
    }

    /*Maintain internal state*/
    /*pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_OPENING,
                                 FALSE,(uint32)PDCOMM_RESULT_WAIT);*/

    /*-------------------------------------------------------------------------
      Starts up the network subsystem (CDMA data service and PPP).  If an
      error occurs, check if it is a DS_ENETCLOSEINPROGRESS, then try to open
      the network subsystem later.
    -------------------------------------------------------------------------*/
    /*MR: Might be required in future to select a specific iface(CDMA,UMTS etc.)
      when opening PPP*/

    PDCOMM_APP_MSG_MED( "Opening PPP.", 0, 0, 0);
    x_retval = dsnet_start(z_handle_table[z_handle.q_handle_id].x_app_id, &x_err_no);

    if(DSS_ERROR == x_retval)
    {
      if(DS_EWOULDBLOCK != x_err_no)
      {
      
        print_dss_err(x_err_no);
        MSG_MED("PPP Failed. Closing netlib",0,0,0);
        x_retval = dsnet_release_handle(z_handle_table[z_handle.q_handle_id].x_app_id, &x_err_no );

        if(DSS_ERROR == x_retval)
        {
          print_dss_err(x_err_no);
        } 
    
        /* Reset app_id on PPP fail 
        */
        z_handle_table[z_handle.q_handle_id].x_app_id = DSS_ERROR;
        
        return PDCOMM_RESULT_ERROR_UNKNOWN;
      }

      return PDCOMM_RESULT_WAIT;
    }/*if(DSS_ERROR == x_retval) */
    else
    {
      /* PPP Connection is Successful. Data services will immediately send a NET_CALLBACK, 
       indicating that the PPP connection is up. Proceed the callflow after getting the
     NET callback*/
      PDCOMM_APP_MSG_MED("Network Established for handle_id [%d] waiting for DS callback to proceed",
                          z_handle.q_handle_id,0,0);
      return PDCOMM_RESULT_WAIT;
    }
    
  }/*if(DSS_ERROR == z_handle_table[z_handle.q_handle_id].x_app_id)*/
  else
  {
    /* PPP Connection is Successful*/
    PDCOMM_APP_MSG_MED("Network already open for handle_id [%d]",
                       z_handle.q_handle_id,0,0);
    return PDCOMM_RESULT_OK;
  }
    
}/*pd_comms_tcp_task_dft_atl_open*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_dft_atl_close

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
pd_comms_return_type pd_comms_tcp_task_dft_atl_close
(
  pd_comms_handle_type       z_handle,
  boolean                    u_e911_close    
)
{
  sint15 x_err_no;
  sint15 x_retval;
  uint32 q_handle_id;

  q_handle_id = z_handle.q_handle_id;

  /* Close PPP Connection */
  PDCOMM_APP_MSG_MED("Closing PPP for handle_id [%d]",q_handle_id,0,0);
  x_retval = dss_pppclose( z_handle_table[q_handle_id].x_app_id, &x_err_no );  

  if( x_retval == DSS_SUCCESS )
  {
    /* PPP Connection closed without waiting. Switch to disconnected state */
    PDCOMM_APP_MSG_MED("PPP Closed for handle_id [%d]",q_handle_id,0,0);
    PDCOMM_APP_MSG_MED("Closing Netlib for handle_id [%d]",q_handle_id,0,0);

    /* Close Network Library */
    x_retval = dss_close_netlib( z_handle_table[q_handle_id].x_app_id, &x_err_no );

    if(x_retval == DSS_ERROR )
    {
      MSG_ERROR("PDCOMM APP: Close Netlib: Error %d for handle_id [%d]",
                 x_err_no, q_handle_id, 0);
      return PDCOMM_RESULT_ERROR_UNKNOWN;
    }
    else
    {
      PDCOMM_APP_MSG_MED("Netlib Closed for handle_id [%d]",q_handle_id,0,0);
      return PDCOMM_RESULT_OK;
    }

  }
  else
  {
    switch ( x_err_no )
    {
      case DS_EWOULDBLOCK:
      {
        /* PPP session is closing down. Netstatus callback will be called
           when session is torn down */
        PDCOMM_APP_MSG_MED("PPP close proceeding for handle_id [%d]",
                           q_handle_id,0,0);
        return PDCOMM_RESULT_WAIT;
      }

      case DS_EBADAPP:
      {
        MSG_ERROR("PDCOMM APP: PPP Close - Invalid App ID: %d for handle_id [%d]",
                  z_handle_table[q_handle_id].x_app_id ,q_handle_id,0);
        return PDCOMM_RESULT_ERROR_UNKNOWN; 
      }

      case DS_ENETCLOSEINPROGRESS:
      {
        PDCOMM_APP_MSG_MED("PPP Close Already in Progress for handle_id [%d]",
                           q_handle_id,0,0);
        return PDCOMM_RESULT_WAIT; 
      }

      default:
      {
        MSG_ERROR("PDCOMM APP: PPP Close Error: %d for handle_id [%d]",
                  x_err_no,q_handle_id,0);
        return PDCOMM_RESULT_ERROR_UNKNOWN;  
      }
    } /*switch (err_no) ends*/
  } /* else for if( retval == DSS_SUCCESS ) ends*/

}/*pd_comms_tcp_task_dft_atl_close*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_teardown_tcp

DESCRIPTION
 This function handles a TCP/IP connect request.
  It attempts to do the following operations if they return immediately:
  1. open netlib
  2. open PPP
  3. open socket

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type pd_comms_tcp_task_teardown_tcp(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  sint15 x_errno;
  pd_comms_tcp_task_operation_status_type e_status = FAILED;
  pd_comms_disconnect_result_type e_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;

  (void)dss_async_deselect(z_handle_table[z_handle.q_handle_id].x_sockfd,
                     DS_READ_EVENT | DS_WRITE_EVENT,
                     &x_errno);


  e_status = pd_comms_tcp_task_close_socket(z_handle.q_handle_id,&e_disc_error,TRUE);

  if(e_status == SUCCEEDED)
  {
    pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                 TRUE,(uint32) PDCOMM_RESULT_OK);
  }
  else if(e_status == PENDING)
  {
    pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                 TRUE,(uint32) PDCOMM_RESULT_WAIT);
  }
  else if(e_status == FAILED)
  { 
    pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                 TRUE,(uint32) e_disc_error);
    e_status = pd_comms_tcp_task_close_ppp(z_handle.q_handle_id, &e_disc_error);

    if(e_status == SUCCEEDED)
    {
      pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                   TRUE,(uint32) PDCOMM_RESULT_OK);
    }
    else if(e_status == PENDING)
    {
      pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                   TRUE,(uint32) PDCOMM_RESULT_WAIT);
    }
    else if(e_status == FAILED)
    {
      pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                   TRUE,(uint32) e_disc_error);
    }
  }

  return e_status;
  
}/*pd_comms_tcp_task_teardown_tcp*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_tcp_connect

DESCRIPTION
 This function handles a TCP/IP connect request.
  It attempts to do the following operations if they return immediately:
  1. open netlib
  2. open PPP
  3. open socket

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type pd_comms_tcp_task_tcp_connect(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  pd_comms_tcp_task_operation_status_type e_status = FAILED, e_retval = FAILED;
  pd_comms_connect_result_type z_conn_error;
  pd_comms_return_type e_return;
  pd_comms_atl_open_params_type z_open_params;
  sint15 x_err_no;            /* error value as returned from dss functions */
  sint31 x_retval;                                                               

  boolean u_close_ppp = TRUE;

  PDCOMM_APP_MSG_MED("Calling DSS Open (handle = %d, session state = %d)",
           z_handle.q_handle_id, z_handle_table[z_handle.q_handle_id].e_link_status , 0 );   


    if(pd_comms_atl_i_ppp_proxy_exists() && 
       z_handle_table[z_handle.q_handle_id].e_link_status == PDCOMM_STATE_DISCONNECTING)  
    {
      /* PPP Connection is already up from the previous session using ATL*/
      PDCOMM_APP_MSG_MED("Network already open through ATL for handle_id [%d]",
                         z_handle.q_handle_id,0,0);     
      z_handle_table[z_handle.q_handle_id].u_use_laptop_call = TRUE;
      u_close_ppp = FALSE;
    }
    else
    {
      pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_OPENING,
                                     FALSE,(uint32)PDCOMM_RESULT_WAIT);
    
      memset(&z_open_params,0,sizeof(pd_comms_atl_open_params_type));
      z_open_params.z_open_params.iface_family = PDSM_ATL_IP_V4;
      z_open_params.z_open_params.iface_name = PDSM_ATL_IFACE_ANY_DEFAULT;
      /* these are actually set by the remote side, but init them anyway */
      z_open_params.z_open_params.iface_ip_addr.type = PDSM_ATL_IP_V4;
      z_open_params.z_open_params.pdp_type = PDSM_ATL_PDP_IP;
      z_open_params.z_open_params.apn_name[0] = '\0';

      if(z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.profile_type == PDCOMM_APN_PROFILE_TYPE_LBS)
      {
         z_open_params.z_open_params.connection_type = PDSM_ATL_CONNECTION_LBS;
      }
      else if(z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.profile_type == PDCOMM_APN_PROFILE_TYPE_LBS_EMERGENCY)
      {
         z_open_params.z_open_params.connection_type = PDSM_ATL_CONNECTION_LBS_EMERGENCY;
      }
      else
      {
         z_open_params.z_open_params.connection_type = PDSM_ATL_CONNECTION_WWAN_INTERNET;
      }
       
      /*Call the ATL function provided by PPP proxy to open PPP link*/
      e_return = pd_comms_atl_i_open(z_handle,&z_open_params); 

      if(e_return == PDCOMM_RESULT_WAIT)
      {
        PDCOMM_APP_MSG_MED("ATL open returned WAIT for handle_id[%d]",z_handle.q_handle_id,0,0);
        return PENDING;
      }
      else if(e_return == PDCOMM_RESULT_OK)
      {
        PDCOMM_APP_MSG_MED("ATL open returned OK.PPP connected for handle_id[%d]",z_handle.q_handle_id,0,0);  
      }
      else if(e_return == PDCOMM_RESULT_ERROR_DATA_DISABLED)
      {
        PDCOMM_APP_MSG_MED("ATL open returned FAIL.Data is diabled handle_id[%d]",z_handle.q_handle_id,0,0);  
        pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                       PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32) PDCOMM_RESULT_ERROR_DATA_DISABLED);      
        return FAILED;
      }
      else
      {
        PDCOMM_APP_MSG_ERROR("ATL open returned ERROR.PPP not connected for handle_id[%d]",
                             z_handle.q_handle_id,0,0);     
        return FAILED;
      }
    }
  
  /*If we reach here that means netlib and PPP are successfully opened so start socket connection now*/
  pd_comms_tcp_task_update_state(z_handle, PDCOMM_STATE_CONNECTING,
                                 TRUE,(uint32)PDCOMM_RESULT_WAIT);    

  e_status = pd_comms_tcp_task_open_socket(z_handle.q_handle_id, &z_conn_error );

  if( e_status == SUCCEEDED )
  {
    /* Enable read data and close event */
    x_retval  = dss_async_select( z_handle_table[z_handle.q_handle_id].x_sockfd,
                                  DS_READ_EVENT | DS_CLOSE_EVENT,
                                  &x_err_no );

    if(DSS_ERROR == x_retval)
    {
      print_dss_err(x_err_no);
    }
        
    /*Goto CONNECTED state and notify client only if this is a NON-SSL
      link since for SSL we still need to open a SSL session on this connection */
    if(z_handle_table[z_handle.q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
    {
      pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                     PDCOMM_STATE_CONNECTED,
                                     TRUE,(uint32)PDCOMM_RESULT_OK);
    }

    e_retval = SUCCEEDED;

  }
  else if( e_status == FAILED )
  {
    pd_comms_disconnect_result_type z_disc_error;

    pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                   PDCOMM_STATE_DISCONNECTING,
                                   TRUE,(uint32)z_conn_error);

    /* Teardown socket connection */
    e_status = pd_comms_tcp_task_close_socket(z_handle.q_handle_id,&z_disc_error,u_close_ppp);

    if(e_status == SUCCEEDED)
    {
      pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                     PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32) PDCOMM_RESULT_OK);
      e_retval = FAILED;
    }
    else if(e_status == PENDING)
    {
      pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                     PDCOMM_STATE_DISCONNECTING,
                                     TRUE,(uint32) PDCOMM_RESULT_WAIT);
      e_retval = PENDING;
    }
    else if(e_status == FAILED)
    {
      pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                     PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32) z_disc_error);
      e_retval = FAILED;
    }

  }/*else if( e_status == FAILED )*/
  else if( e_status == PENDING )
  {
    /* Notify client that the socket connection has started */
    pd_comms_tcp_task_update_state(z_handle_table[z_handle.q_handle_id].z_handle,
                                   PDCOMM_STATE_CONNECTING,
                                   TRUE,(uint32)PDCOMM_RESULT_WAIT);
    e_retval = PENDING;
  }

  return e_retval;  

} /* pd_comms_tcp_task_tcp_connect */


/*===========================================================================
FUNCTION pd_comms_tcp_task_retrieve_root_certificate

DESCRIPTION
 This function retrieves a pointer to the NULL terminated H-SLP root certificate

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if succeeded, FALSE if failed

SIDE EFFECTS
  None
===========================================================================*/
static boolean pd_comms_tcp_task_retrieve_root_certificate(
  /*handle id*/  
  uint32 q_handle_id,
  /*Pointer to the root certificate*/
  uint8 **p_cert,
  /*length of the returned root certificate*/
  uint16 *p_length )
{
  if(p_cert ==NULL || p_length == NULL)
  {
    return FALSE;
  }
  
  *p_cert = z_handle_table[q_handle_id].p_ssl_cert;
  *p_length = z_handle_table[q_handle_id].w_ssl_cert_len;

  return TRUE;

}/*pd_comms_tcp_task_retrieve_root_certificate*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_close_callback

DESCRIPTION
  This function is called by the security library to provide status on a previously
  executed close command. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_close_callback(
  /*Pointer to optional user provided data*/
  void * p_udata,
  /*Error ( or success indication ) that occurred*/
  secerrno_enum_type e_error )
{
  uint32 q_handle_id;
  
  if(p_udata == NULL)
  {
    PDCOMM_APP_MSG_ERROR("Invalid user data in ssl_close callback",0,0,0);
    return;
  }                                        
  
  q_handle_id = *((uint32 *)p_udata);

  if( q_handle_id < PDCOMM_MAX_HANDLES ) 
  {
    pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type* p_hdr_ptr; 
    os_IpcMsgType* p_ipc_ptr;

    PDCOMM_APP_MSG_MED("SSL close callback for handle_id [%d], Error = %d",
                        q_handle_id, (uint32)e_error, 0);

    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type),
                           IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);             
    if(p_ipc_ptr == NULL)
    {
      PDCOMM_APP_MSG_HIGH("IPC Create failed during SSL close callback. Handle Id: %d",
                          q_handle_id,0,0);
    }
    else
    {
      /*lint -e826 */ 
      p_hdr_ptr = (pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
      /*lint +e826 */
      p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_SEC_CLOSE_CB;
      p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

      p_hdr_ptr->handle.q_handle_id = q_handle_id;
      p_hdr_ptr->e_error = e_error;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
        PDCOMM_APP_MSG_HIGH("IPC Send failed during SSL close callback. Handle Id: %d",
                            q_handle_id,0,0);
        (void)os_IpcDelete(p_ipc_ptr);
      }
    }
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES ) */
  else
  {
    MSG_ERROR( "Got unknown SSL Close callback (handle_id = %d)", q_handle_id, 0, 0);   
  }
} /*lint !e818 */
/*pd_comms_tcp_task_ssl_close_callback*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_alert_callback

DESCRIPTION
  This function is called by the security library to indicate a security related
  alert has occurred. If necessary this function will notify the application of the
  security alert. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_alert_callback(  
  /*Pointer to optional user provided data*/  
  void                              * p_udata,
  /*Type of alert being reported*/
  secssl_ses_alert_level_enum_type  alert_level,
  /*Bit mask containing the sum of all of the current alerts*/
  secssl_ses_alert_mask_enum_type   alert_mask
)
{

  boolean u_closeConnection = FALSE;
  uint32 q_handle_id;
  
  if(p_udata == NULL)
  {
    PDCOMM_APP_MSG_ERROR("Invalid user data in ssl_alert callback",0,0,0);
    return;
  }                                        
  
  q_handle_id = *((uint32*)p_udata);
   
  switch( alert_level )
  {
    case SECSSL_ALERT_LEVEL_WARN:
    {
      PDCOMM_APP_MSG_MED(" SSL Alert Received. Level: Warning",0,0,0);
      break;
    }

    case SECSSL_ALERT_LEVEL_INFO:
    {
      PDCOMM_APP_MSG_MED(" SSL Alert Received. Level: Info",0,0,0);
      break;
    }

    case SECSSL_ALERT_LEVEL_FATAL:
    {
      PDCOMM_APP_MSG_ERROR(" SSL Alert Received. Level: Fatal",0,0,0);
      u_closeConnection = TRUE;
      break;
    }

    case SECSSL_ALERT_LEVEL_SUSPEND:
    {
      PDCOMM_APP_MSG_ERROR(" SSL Alert Received. Level: Suspend",0,0,0);
      u_closeConnection = TRUE;
      break;
    }

    default:
      break;
  }/*switch( alert_level )*/

  PDCOMM_APP_MSG_MED(" Alerts Received:",0,0,0);
/*lint -e655 */
  if( alert_mask == SECSSL_ALERT_CLOSE_NOTIFY )
  {
    PDCOMM_APP_MSG_MED("          CLOSE NOTIFY",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_UNEXPECTED_MESSAGE )
  {
    PDCOMM_APP_MSG_MED("          UNEXPECTED MESSAGE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_BAD_RECORD_MAC )
  {
    PDCOMM_APP_MSG_MED("          BAD RECORD MAC",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_DECRYPTION_FAILED )
  {
    PDCOMM_APP_MSG_MED("          DECRYPTION FAILED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_RECORD_OVERFLOW )
  {
    PDCOMM_APP_MSG_MED("          RECORD OVERFLOW",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_DECOMPRESSION_FAILURE )
  {
    PDCOMM_APP_MSG_MED("          DECOMPRESSION FAILURE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_HANDSHAKE_FAILURE )
  {
    PDCOMM_APP_MSG_MED("          HANDSHAKE FAILURE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_NO_CERTIFICATE )
  {
    PDCOMM_APP_MSG_MED("          NO CERTIFICATE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_BAD_CERTIFICATE )
  {
    PDCOMM_APP_MSG_MED("          BAD CERTIFICATE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_BAD_CERTIFICATE_SIGN )
  {
    PDCOMM_APP_MSG_MED("          BAD CERTIFICATE SIGN",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_BAD_CERTIFICATE_ISSUER )
  {
    PDCOMM_APP_MSG_MED("          BAD CERTIFICATE ISSUER",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_UNSUPPORTED_CERTIFICATE )
  {
    PDCOMM_APP_MSG_MED("          UNSUPPORTED CERTIFICATE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_CERTIFICATE_REVOKED )
  {
    PDCOMM_APP_MSG_MED("          CERTIFICATE REVOKED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_CERTIFICATE_EXPIRED )
  {
    PDCOMM_APP_MSG_MED("          CERTIFICATE EXPIRED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_CERTIFICATE_UNKNOWN )
  {
    PDCOMM_APP_MSG_MED("          CERTIFICATE UNKNOWN",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_ILLEGAL_PARAMETER )
  {
    PDCOMM_APP_MSG_MED("          ILLEGAL PARAMETER",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_INVALID )
  {
    PDCOMM_APP_MSG_MED("          INVALID",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_UNKNOWN_CA )
  {
    PDCOMM_APP_MSG_MED("          UNKNOWN CA",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_ACCESS_DENIED )
  {
    PDCOMM_APP_MSG_MED("          ACCESS DENIED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_DECODE_ERROR )
  {
    PDCOMM_APP_MSG_MED("          DECODE ERROR",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_DECRYPT_ERROR )
  {
    PDCOMM_APP_MSG_MED("          DECRYPT ERROR",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_EXPORT_RESTRICTION )
  {
    PDCOMM_APP_MSG_MED("          EXPORT RESTRICTION",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_PROTOCOL_VERSION )
  {
    PDCOMM_APP_MSG_MED("          PROTOCOL VERSION",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_INSUFFICIENT_SECURITY )
  {
    PDCOMM_APP_MSG_MED("          INSUFFICIENT SECURITY",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_INTERNAL_ERROR )
  {
    PDCOMM_APP_MSG_MED("          INTERNAL ERROR",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_USER_CANCELED )
  {
    PDCOMM_APP_MSG_MED("          USER CANCELLED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_NO_RENEGOTIATION )
  {
    PDCOMM_APP_MSG_MED("          NO RENOGOTIATION",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_HOST_MISMATCH )
  {
    PDCOMM_APP_MSG_MED("          HOST MISMATCH",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_CERT_INFO )
  {
    PDCOMM_APP_MSG_MED("          CERT INFO",0,0,0);
  }

  if(u_closeConnection)
  {
    secerrno_enum_type e_error;
      
    PDCOMM_APP_MSG_MED("SSL Closing socket for handle_id [%d]",q_handle_id,0,0);

    if(z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTING )
    {
      pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                     PDCOMM_STATE_DISCONNECTING,
                                     TRUE,(uint32)PDCOMM_RESULT_WAIT);
    }

    /* Close SSL Session */
    e_error = secssl_close_session( z_handle_table[q_handle_id].z_ssl_handle,
                                    pd_comms_tcp_task_ssl_close_callback, 
                                    &z_handle_table[q_handle_id].z_handle.q_handle_id);
    if(e_error != E_SUCCESS )
    {
      if(e_error!= E_ALREADY_DONE)
      {
        MSG_ERROR("PDCOMM APP: Could not close SSL session. Error: %d for handle_id [%d]",
                  (uint32)e_error, q_handle_id, 0);    
      }
      
      (void)secssl_delete_session( z_handle_table[q_handle_id].z_ssl_handle );
      z_handle_table[q_handle_id].z_ssl_handle = NULL;
      /*disconnect external socket and bring down PPP*/
      (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[q_handle_id].z_handle); 
    }  
  }/*if(u_closeConnection)*/
/*lint +e655 */
} /*lint !e715 !e818 */
/*pd_comms_tcp_task_ssl_alert_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_event_callback

DESCRIPTION
  This function is called by the security library to indicate a security related
  event has occurred. If necessary this function will notify the application of the
  security event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_event_callback(
  /*Session handle for which the event occurred*/  
  secssl_handle_type                z_session_handle,
  /*Event that occurred*/
  secssl_ses_event_enum_type        e_event,
  /*Pointer to optional user provided data*/
  void                              *p_udata
)
{
  uint32 q_handle_id;
  
  if(p_udata == NULL)
  {
    PDCOMM_APP_MSG_ERROR("Invalid user data in ssl_event callback",0,0,0);
    return;
  }                                        
  
  q_handle_id = *((uint32*)p_udata);

  if( q_handle_id < PDCOMM_MAX_HANDLES ) 
  {
    pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type* p_hdr_ptr; 
    os_IpcMsgType* p_ipc_ptr;

    PDCOMM_APP_MSG_MED("SSL event callback for handle_id [%d], Event = %d",
                        q_handle_id, (uint32)e_event, 0);

    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type),
                           IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);             
    if(p_ipc_ptr == NULL)
    {
      PDCOMM_APP_MSG_HIGH("IPC Create failed during SSL event callback. Handle Id: %d",
                          q_handle_id,0,0);
    }
    else
    {
      /*lint -e826 */ 
      p_hdr_ptr = (pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
      /*lint +e826 */
      p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_SEC_EVENT_CB;
      p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

      p_hdr_ptr->handle.q_handle_id = q_handle_id;
      p_hdr_ptr->event_mask = e_event;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
        PDCOMM_APP_MSG_HIGH("IPC Send failed during SSL event callback. Handle Id: %d",
                            q_handle_id,0,0);
        (void)os_IpcDelete(p_ipc_ptr);
      }
    }
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES ) */
  else
  {
    MSG_ERROR( "Got unknown SSL event callback (handle_id = %d)", q_handle_id, 0, 0);   
  }

} /*lint !e715 !e818 */
/*pd_comms_tcp_task_ssl_event_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_errno_callback

DESCRIPTION
  This function is called by the security library to provide status on a previously
  executed command. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_errno_callback(
  /*Pointer to optional user provided data*/
  void *p_udata,
  /*Error ( or success indication ) that occurred*/
  secerrno_enum_type e_error )
{
  if( e_error == E_SUCCESS )
  {
    if (p_udata != NULL)
    {
      uint32 q_handle_id = *(uint32*)p_udata;
      PDCOMM_APP_MSG_MED("SSL Operation Successful, handle=%d", q_handle_id,0,0);

      if (q_handle_id < PDCOMM_MAX_HANDLES &&
          z_handle_table[q_handle_id].in_use == TRUE &&
          z_handle_table[q_handle_id].z_ssl_handle != NULL &&
          z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTED &&
          z_handle_table[q_handle_id].w_num_bytes_to_write > 0)
      {
        // there is a pending write, this is SSL's Write Ack (success)
        pd_comms_write_ack_params_type z_params;
    
        z_params.q_bytes_written = z_handle_table[q_handle_id].w_num_bytes_to_write;
        z_params.q_transaction_id = z_handle_table[q_handle_id].q_write_transaction_id;

        PDCOMM_APP_MSG_MED("Got SSL Write Ack for %d bytes (handle=%d)", 
                           z_handle_table[q_handle_id].w_num_bytes_to_write, q_handle_id,0);

        // reset the value, so PENDING write is done
        z_handle_table[q_handle_id].w_num_bytes_written = 0;
        z_handle_table[q_handle_id].w_num_bytes_to_write = 0;

        /* Acknowledge the write */
        (void)pd_comms_send_write_ack_to_client(z_handle_table[q_handle_id].z_handle,
                                      z_handle_table[q_handle_id].q_source_thread_id,
                                      z_handle_table[q_handle_id].q_base_msg_id,
                                      &z_params);
      }
    }
    else 
    {
      PDCOMM_APP_MSG_MED("SSL Operation Successful",0,0,0);
    }
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL Operation Failed. Error: %d", e_error, 0, 0 );
    
  }
} /*lint !e715 !e818 */
/*pd_comms_tcp_task_ssl_errno_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_open_callback

DESCRIPTION
  This function is called by the security library to provide status on a previously
  executed open command. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_open_callback(
  /*Pointer to optional user provided data*/
  void *p_udata,
  /*Error ( or success indication ) that occurred*/
  secerrno_enum_type e_error )
{
  uint32 q_handle_id;

  if( (e_error == E_SUCCESS) || (e_error == E_ALREADY_DONE) || 
      (e_error == E_WOULD_BLOCK) || (e_error == E_IN_PROGRESS) )
  {
    PDCOMM_APP_MSG_MED("SSL Open Request Successful",0,0,0);
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL Open Request Failed. Error: %d", e_error, 0, 0 );
    
    
    if(p_udata == NULL)
    {
      PDCOMM_APP_MSG_ERROR("Invalid user data in ssl_event callback",0,0,0);
      return;
    }                                        
    
    q_handle_id = *((uint32*)p_udata);

    if( q_handle_id < PDCOMM_MAX_HANDLES ) 
    {
      pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type* p_hdr_ptr; 
      os_IpcMsgType* p_ipc_ptr;

      PDCOMM_APP_MSG_MED("SSL open callback for handle_id [%d], Error = %d",
                          q_handle_id, (uint32)e_error, 0);

      p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);             
      if(p_ipc_ptr == NULL)
      {
        PDCOMM_APP_MSG_HIGH("IPC Create failed during SSL event callback. Handle Id: %d",
                            q_handle_id,0,0);
      }
      else
      {
        /*lint -e826 */  
        p_hdr_ptr = (pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
        /*lint +e826 */ 
        p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_SEC_OPEN_CB;
        p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

        p_hdr_ptr->handle.q_handle_id = q_handle_id;
        p_hdr_ptr->e_error = e_error;

        if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
        {
          PDCOMM_APP_MSG_HIGH("IPC Send failed during SSL event callback. Handle Id: %d",
                              q_handle_id,0,0);
          (void)os_IpcDelete(p_ipc_ptr);
        }
      }

    }
   
  } /*else for if( (e_error == E_SUCCESS) || (e_error == E_ALREADY_DONE) || 
      (e_error == E_WOULD_BLOCK) || (e_error == E_IN_PROGRESS) )*/
  
} /*lint !e818 */
/*pd_comms_tcp_task_ssl_open_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_secure_tcp_read

DESCRIPTION
  This function attempts to read one or more bytes from a secure TCP/IP socket 

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
static boolean pd_comms_tcp_task_secure_tcp_read(
  /*Handle of link to read from*/
  uint32 q_handle_id,
  /*wait for all requested data to arrive before sending it over to client or not*/ 
  boolean u_wait_for_all )
{
  uint16 w__bytes_read = PDCOMM_READ_DATA_BUF_SIZE;
  
  pd_comms_read_result_type e_read_error = PDCOMM_READ_ERROR_UNKNOWN;
  secerrno_enum_type e_error = E_SUCCESS;
  boolean result = TRUE;

 
  uint8 *p_read_data;
  uint16 *p_num_bytes_to_read;
  uint16 *p_num_bytes_already_read;
  

  p_read_data = &z_handle_table[q_handle_id].p_read_buff[0];
  p_num_bytes_to_read = &z_handle_table[q_handle_id].w_num_bytes_to_read;
  p_num_bytes_already_read = &z_handle_table[q_handle_id].w_num_bytes_already_read;

  /* Retrieve up to max_bytes from the socket */
  do
  {
    w__bytes_read = *p_num_bytes_to_read - *p_num_bytes_already_read;
    e_error = secssl_read( 
              z_handle_table[q_handle_id].z_ssl_handle,
              &p_read_data[*p_num_bytes_already_read],
              &w__bytes_read);
        

    if( e_error != E_SUCCESS )
    {
      switch (e_error)
      {
        case E_WOULD_BLOCK:
        {
          /* No more data available */
          if((*p_num_bytes_already_read == *p_num_bytes_to_read) || (u_wait_for_all == FALSE) )
          {
            pd_comms_read_data_params_type z_params;

            z_params.w_data_len = *p_num_bytes_already_read;
            z_params.p_data = p_read_data;
            PDCOMM_APP_MSG_MED("Sending %d bytes for handle_id [%d]",*p_num_bytes_already_read,q_handle_id,0); 
            (void)pd_comms_senddata_to_client(z_handle_table[q_handle_id].z_handle, 
                                        z_handle_table[q_handle_id].q_source_thread_id,
                                        z_handle_table[q_handle_id].q_base_msg_id,
                                        z_handle_table[q_handle_id].q_read_transaction_id,
                                        &z_params);

            
            *p_num_bytes_already_read = *p_num_bytes_to_read = 0;

            // no more pending READ, can send more READ_DATA event to PDCOMM client
            z_handle_table[q_handle_id].requested_read = FALSE;  
            
            
          } 
           
          result = TRUE;  
          break;
        }
        
        case E_IN_PROGRESS:
        case E_ALREADY_DONE:
        {
          /* No more data available */
          result = TRUE;
          break;
        }

        default:
        {
          MSG_ERROR("PDCOMM APP: Secure socket read - Unknown Error: %d for handle_id [%d]",
                    e_error,q_handle_id,0);
          e_read_error = PDCOMM_READ_ERROR_INTERNAL;
          result = FALSE;
          break;
        }
      }

    }/*if( e_error != E_SUCCESS )*/
    else
    {
      result = TRUE;
      
      if(w__bytes_read > 0)
      {
        *p_num_bytes_already_read += w__bytes_read;
        PDCOMM_APP_MSG_MED("Recevied %d bytes for handle_id [%d]",w__bytes_read,q_handle_id,0);  
       
        if((*p_num_bytes_already_read == *p_num_bytes_to_read) || (u_wait_for_all == FALSE) )
        {
            
          pd_comms_read_data_params_type z_params;

          z_params.w_data_len = *p_num_bytes_already_read;
          z_params.p_data = p_read_data;
          PDCOMM_APP_MSG_MED("Sending %d bytes for handle_id [%d]",*p_num_bytes_already_read,q_handle_id,0); 
          (void)pd_comms_senddata_to_client(z_handle_table[q_handle_id].z_handle, 
                                      z_handle_table[q_handle_id].q_source_thread_id,
                                      z_handle_table[q_handle_id].q_base_msg_id,
                                      z_handle_table[q_handle_id].q_read_transaction_id,
                                      &z_params);

          
          *p_num_bytes_already_read = *p_num_bytes_to_read = 0;

          // no more pending READ, can send more READ_DATA event to PDCOMM client
          z_handle_table[q_handle_id].requested_read = FALSE;  
          
          result = TRUE;
          e_error = E_FAILURE; /*to break from the do while() loop*/
          
        }
      }/*if(w__bytes_read > 0)*/
      else
      {
        MSG_ERROR("PDCOMM APP:Recevied %d bytes for handle_id [%d]",w__bytes_read,q_handle_id,0); 
      }
    }/*else for if( e_error != E_SUCCESS )*/
    
  }
  while( e_error == E_SUCCESS );

  if(result == FALSE)
  {
    pd_comms_notification_params_type z_params;
    z_params.e_new_state = z_handle_table[q_handle_id].e_link_status;

    z_params.q_result = (uint32) e_read_error;
    (void)pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                           z_handle_table[q_handle_id].q_source_thread_id,
                           z_handle_table[q_handle_id].q_base_msg_id,
                           z_handle_table[q_handle_id].z_connect_params.q_transaction_id,
                           ERROR_RPT,&z_params);
  }

  return result;

}/*pd_comms_tcp_task_secure_tcp_read*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sec_event_cb

DESCRIPTION
  Whenever there is a Security event callback, an IPC message is sent to 
  PDCOMM APP. This IPC message is handled in this function. Based upon the i
  recevied event_mask it takes the appropriate action 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_sec_event_cb(
  /*Pointer to IPC message containing the security callback information */
  const os_IpcMsgType* p_msg )
{
  secssl_ses_event_enum_type e_event;
  secerrno_enum_type e_error;
  uint32 q_handle_id;

  if(p_msg == NULL)
  {
    return;
  }
     
  /*lint -e826 */ 
  q_handle_id = ((pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  PDCOMM_APP_MSG_MED("Received SOCKET callback for handle_id [%d] with event: %d",
                      q_handle_id,
                     (uint32)((pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->event_mask,0);
  /*lint +e826 */ 

  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    pd_comms_tcp_task_reset_inactivity_count(q_handle_id);

    /*lint -e826 */ 
    e_event = ((pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->event_mask;
    /*lint +e826 */ 
    switch( e_event )
    {
      case SECSSL_SES_SOCK_READ_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Socket READ event for handle_id [%d]",q_handle_id,0,0);  
        if( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED )
        {
         
          if(z_handle_table[q_handle_id].w_num_bytes_already_read <
             z_handle_table[q_handle_id].w_num_bytes_to_read)
          {
           /*If a previous buffered read is pending, continue with that*/   
            (void) pd_comms_tcp_task_secure_tcp_read(q_handle_id, TRUE);
          }
          else
          {          
             // if we have not sent READ_DATA since last read operation, send it now
            if (z_handle_table[q_handle_id].requested_read == FALSE)  
            {
              /*Notify client about the read data availability*/  
              pd_comms_notification_params_type z_params;
              pd_comms_return_type e_result;
              
              z_params.e_new_state = z_handle_table[q_handle_id].e_link_status;
              e_result = pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                         z_handle_table[q_handle_id].q_source_thread_id,
                         z_handle_table[q_handle_id].q_base_msg_id,
                         z_handle_table[q_handle_id].z_connect_params.q_transaction_id, 
                         READ_DATA,&z_params);
              
              if(e_result != PDCOMM_RESULT_OK)
              {
                MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                          (uint32)e_result,q_handle_id,0);

              } 
              else
              {
                // successfully sent READ_DATA event to PDCOMM client to request read
                z_handle_table[q_handle_id].requested_read = TRUE;   
              }              
            }/*if (z_handle_table[q_handle_id].requested_read == FALSE)*/
          }
        }/*if( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED )*/
        else
        {
          MSG_ERROR("PDCOMM APP: SSL READ event while link is DISCONNECTED for handle_id [%d]",
                    q_handle_id,0,0);
        }
        break;
      }/*case SECSSL_SES_SOCK_READ_EVENT*/

      case SECSSL_SES_SOCK_CLOSE_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Socket Closed for handle_id [%d]",q_handle_id,0,0);

        if(z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTING )
        {
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)PDCOMM_RESULT_WAIT);
        }

        /* Close SSL Session */
        e_error = secssl_close_session( z_handle_table[q_handle_id].z_ssl_handle,
                                        pd_comms_tcp_task_ssl_close_callback, 
                                        &z_handle_table[q_handle_id].z_handle.q_handle_id);
        if(e_error != E_SUCCESS )
        {
          if(e_error!= E_ALREADY_DONE)
          {
            MSG_ERROR("PDCOMM APP: Could not close SSL session. Error: %d for handle_id [%d]",
                      (uint32)e_error, q_handle_id, 0);    
          }
          
          (void)secssl_delete_session( z_handle_table[q_handle_id].z_ssl_handle );
          z_handle_table[q_handle_id].z_ssl_handle = NULL;
          /*disconnect external socket and bring down PPP*/
          (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[q_handle_id].z_handle); 
        }

        break;
      }/*case SECSSL_SES_SOCK_CLOSE_EVENT*/

      case SECSSL_SES_DNS_START_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Attempting to resolve hostname for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_DNS_START_EVENT*/

      case SECSSL_SES_DNS_SUCCESS_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL hostname successfully resolved for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_DNS_SUCCESS_EVENT*/

      case SECSSL_SES_DNS_FAILURE_EVENT:
      {
        PDCOMM_APP_MSG_ERROR("SSL DNS Lookup Failed for handle_id [%d]",
                  q_handle_id,0,0);

        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)PDCOMM_CONNECT_ERROR_DNS_LOOKUP_FAILED);

        e_error = secssl_close_session( z_handle_table[q_handle_id].z_ssl_handle,
                                        pd_comms_tcp_task_ssl_close_callback, 
                                        &z_handle_table[q_handle_id].z_handle.q_handle_id);
        if(e_error != E_SUCCESS )
        {
          if(e_error!= E_ALREADY_DONE)
          {
          
            MSG_ERROR("PDCOMM APP: Could not close SSL session. Error: %d for handle_id [%d]",
                      (uint32)e_error, q_handle_id, 0);                                       
          }
          (void)secssl_delete_session( z_handle_table[q_handle_id].z_ssl_handle );
          z_handle_table[q_handle_id].z_ssl_handle = NULL;
          /*disconnect external socket and bring down PPP*/
          (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[q_handle_id].z_handle); 

        }
        break;
      }/*case SECSSL_SES_DNS_FAILURE_EVENT*/

      case SECSSL_SES_CONNECT_START_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Socket Connection Starting for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_CONNECT_START_EVENT*/

      case SECSSL_SES_CONNECT_SUCCESS_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Socket Connected for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_CONNECT_SUCCESS_EVENT*/

      case SECSSL_SES_CONNECT_FAILURE_EVENT:
      {
        PDCOMM_APP_MSG_ERROR("SSL Socket Connection Failed for handle_id [%d]",
                  q_handle_id,0,0);

        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)PDCOMM_CONNECT_ERROR_UNKNOWN);

        e_error = secssl_close_session( z_handle_table[q_handle_id].z_ssl_handle,
                                        pd_comms_tcp_task_ssl_close_callback, 
                                        &z_handle_table[q_handle_id].z_handle.q_handle_id);
        if(e_error != E_SUCCESS )
        {
          if(e_error!= E_ALREADY_DONE)
          {
          
            PDCOMM_APP_MSG_ERROR("Could not close SSL session. Error: %d for handle_id [%d]",
                                  (uint32)e_error, q_handle_id, 0);
          }
          (void)secssl_delete_session( z_handle_table[q_handle_id].z_ssl_handle );
          z_handle_table[q_handle_id].z_ssl_handle = NULL;
          /*disconnect external socket and bring down PPP*/
          (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[q_handle_id].z_handle); 

        }

        break;
      }/*case SECSSL_SES_CONNECT_FAILURE_EVENT*/

      case SECSSL_SES_AUTH_START_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Authentication Starting for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_AUTH_START_EVENT*/

      case SECSSL_SES_AUTH_SUCCESS_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Authentication Successful for handle_id [%d]",
                           q_handle_id,0,0);
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_CONNECTED,
                                         TRUE,(uint32)PDCOMM_RESULT_OK);
        break;
      }/*case SECSSL_SES_AUTH_SUCCESS_EVENT*/

      case SECSSL_SES_AUTH_FAILURE_EVENT:
      {
        /* Set the socket linger time to be 2 secs so that the TCP block does not remain in
           the TIME-WAIT state for 30 secs*/
        pd_comms_tcp_task_setsockopt(q_handle_id);

        PDCOMM_APP_MSG_ERROR("SSL Authentication Failure",0,0,0);

        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)PDCOMM_CONNECT_ERROR_SECURITY_FAILURE);

        e_error = secssl_close_session( z_handle_table[q_handle_id].z_ssl_handle,
                                        pd_comms_tcp_task_ssl_close_callback, 
                                        &z_handle_table[q_handle_id].z_handle.q_handle_id); 
        if(e_error != E_SUCCESS )
        {
          if(e_error!= E_ALREADY_DONE)
          {
          
            PDCOMM_APP_MSG_ERROR("Could not close SSL session. Error: %d for handle_id [%d]",
                                  (uint32)e_error, q_handle_id, 0);
          }
          (void)secssl_delete_session( z_handle_table[q_handle_id].z_ssl_handle );
          z_handle_table[q_handle_id].z_ssl_handle = NULL;
          /*disconnect external socket and bring down PPP*/
          (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[q_handle_id].z_handle); 

        }

        break;
      }/*case SECSSL_SES_AUTH_FAILURE_EVENT*/

      default:
      {
        PDCOMM_APP_MSG_ERROR("Unknown SSL Event: %d for handle_id [%d]",
                   e_event, q_handle_id, 0);
        break;
      }
    }/*switch( e_event )*/

  }/*if( q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL event callback: handle_id %d out of range",
               q_handle_id, 0, 0);
  }

}/*pd_comms_tcp_task_handle_sec_event_cb*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sec_open_cb

DESCRIPTION
  Whenever there is a Security open callback, an IPC message is sent to 
  PDCOMM APP. This IPC message is handled in this function. Based upon the i
  recevied event_mask it takes the appropriate action. An IPC message will be sent
  out for security open callback only if an open error has occured.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_sec_open_cb(
  /*Pointer to IPC message containing the security callback information */
  const os_IpcMsgType* p_msg )
{
  uint32 q_handle_id;
  pd_comms_connect_result_type z_connect_error;
  
  if(p_msg == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL IPC received in security_open callback",0,0,0);
    return;
  }
  
  /*lint -e826 */ 
  q_handle_id = ((pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  /*lint +e826 */ 

  PDCOMM_APP_MSG_MED("SEC_OPEN callback received for handle_id[%d]",q_handle_id,0,0);
  
  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    PDCOMM_APP_MSG_MED("Received security open callback for handle_id [%d] with error: %d link_state[%d]",
                        q_handle_id,
                        (uint32)((pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type*)p_msg->p_Data)->e_error,
                        z_handle_table[q_handle_id].e_link_status); /*lint !e826*/

    if((z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTING) &&
       (z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTED))
    {
      z_connect_error = PDCOMM_CONNECT_ERROR_NETWORK_LIB_INIT_FAILURE; 
      pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, PDCOMM_STATE_DISCONNECTING,
                                     TRUE,(uint32)z_connect_error);  
      (void)secssl_delete_session( z_handle_table[q_handle_id].z_ssl_handle);
      z_handle_table[q_handle_id].z_ssl_handle = NULL;
      /*disconnect external socket and bring down PPP*/
      (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[q_handle_id].z_handle); 
      
    }
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL open callback: handle_id %d out of range",
               q_handle_id, 0, 0);
  }
}/*pd_comms_tcp_task_handle_sec_open_cb*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sec_close_cb

DESCRIPTION
  Whenever there is a Security close callback, an IPC message is sent to 
  PDCOMM APP. This IPC message is handled in this function. Based upon the 
  recevied event_mask it takes the appropriate action. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_sec_close_cb(
  /*Pointer to IPC message containing the security callback information */
  const os_IpcMsgType* p_msg )
{
  secerrno_enum_type e_error;
  uint32 q_handle_id;
  pd_comms_disconnect_result_type e_disc_error;
  pd_comms_tcp_task_operation_status_type e_status = FAILED;
  sint15 x_errno;
  
  if(p_msg == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL IPC received in security_close callback",0,0,0);
    return;
  }
  
  /*lint -e826 */ 
  q_handle_id = ((pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  e_error = ((pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type*)p_msg->p_Data)->e_error;
  PDCOMM_APP_MSG_MED("Received security close callback for handle_id [%d] with error: %d",
                      q_handle_id,
                     (uint32)e_error,0);
  /*lint +e826 */ 

  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    if( e_error == E_SUCCESS )
    {
      PDCOMM_APP_MSG_MED("SSL Close Request Successful for handle_id [%d]",
                        q_handle_id,0,0);
    }
    else
    {
      PDCOMM_APP_MSG_ERROR(" SSL Close Request Failed. Error: %d for handle_id [%d]",
                e_error, q_handle_id, 0 );
    }

    /*In any case delete the existing SSL session*/
    (void)secssl_delete_session( z_handle_table[q_handle_id].z_ssl_handle);
    z_handle_table[q_handle_id].z_ssl_handle = NULL;

    
    /*If there was pending connection and SSL closed properly start processing that
     instead of going to DISCONNECTED*/  
    if( (z_handle_table[q_handle_id].u_connectPending) && (E_SUCCESS == e_error) )
    {
      /*Only close the socket. Do not bring down the PPP connection*/  
      (void)dss_async_deselect(z_handle_table[q_handle_id].x_sockfd,
                               DS_READ_EVENT | DS_WRITE_EVENT,
                               &x_errno);  
      
      e_status = pd_comms_tcp_task_close_socket(q_handle_id,&e_disc_error,FALSE);

      if(e_status != PENDING)
      {
      
       PDCOMM_APP_MSG_MED("Processing pending connection request for handle_id[%d]",
                           q_handle_id,0,0);
       z_handle_table[q_handle_id].u_connectPending = FALSE;
       pd_comms_tcp_task_handle_connect(&z_handle_table[q_handle_id].z_pendingConnectReq,TRUE);
      }
    }
    else
    {
      /*Bring down external socket and PPP connection*/  
      (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[q_handle_id].z_handle);
    }
      
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL close callback: handle_id %d out of range",
               q_handle_id, 0, 0);
  }
} /*pd_comms_tcp_task_handle_sec_close_cb*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sec_close_net_cb

DESCRIPTION
  Whenever there is a Security close network callback, an IPC message is sent to 
  PDCOMM APP. This IPC message is handled in this function. Based upon the 
  recevied event_mask it takes the appropriate action. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_sec_close_net_cb(
  /*Pointer to IPC message containing the security callback information */
  const os_IpcMsgType* p_msg )
{
  secerrno_enum_type e_error;
  uint32 q_handle_id;
  uint32 e_disc_error;
  
  if(p_msg == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL IPC received in security_close callback",0,0,0);
    return;
  }
  /*lint -e826 */    
  q_handle_id = ((pd_comms_tcp_task_secure_tcp_close_net_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  e_error = ((pd_comms_tcp_task_secure_tcp_close_net_cb_ipc_hdr_type*)p_msg->p_Data)->e_error;
  PDCOMM_APP_MSG_MED("Received security close network callback for handle_id [%d] with error: %d",
                      q_handle_id,
                     (uint32)e_error,0);
  /*lint +e826 */   
  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    if( e_error == E_SUCCESS )
    {
      PDCOMM_APP_MSG_MED("SSL Close network Request Successful for handle_id [%d]",
                        q_handle_id,0,0);
      e_disc_error = (uint32)PDCOMM_RESULT_OK;
    }
    else
    {
      PDCOMM_APP_MSG_ERROR(" SSL Close network Request Failed. Error: %d for handle_id [%d]",
                e_error, q_handle_id, 0 );
      e_disc_error = (uint32)PDCOMM_DISCONNECT_ERROR_UNKNOWN;
    }

    pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, 
                                   PDCOMM_STATE_DISCONNECTED,!z_handle_table[q_handle_id].session_aborted, /*lint !e730*/
                                   (uint32)e_disc_error);
    
      
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL close callback: handle_id %d out of range",
               q_handle_id, 0, 0);
  }
} /*pd_comms_tcp_task_handle_sec_close_net_cb*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_set_ssl_params

DESCRIPTION
  This function opens a SSL session for an existing socket connection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean pd_comms_tcp_task_set_ssl_params(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  secerrno_enum_type e_error;
  secssl_ses_parameter_pdata_union_type z_ssl_params;
  secssl_handle_type *p_ssl_handle = &z_handle_table[z_handle.q_handle_id].z_ssl_handle;
  uint8 i=0;

  if(z_handle_table[z_handle.q_handle_id].x_sockfd == DSS_ERROR)
  {
    PDCOMM_APP_MSG_ERROR("Invalid sockfd found while setting SSL params",0,0,0);  
    return FALSE;
  }
  
  /*Set the external socket for secure connections*/
  z_ssl_params.socket_fd_pdata = z_handle_table[z_handle.q_handle_id].x_sockfd;
  e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_SOCKET_FD, &z_ssl_params );
  
  if( e_error != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /*Ignore certificate expiration check*/
  z_ssl_params.ignore_expiration_pdata = TRUE;
  e_error = secssl_set_sesparam( *p_ssl_handle,  SECSSL_SES_IGNORE_EXPIRATION, &z_ssl_params );
  
  if( e_error != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter to ignore certificate expiration check", e_error, 0, 0);
    return FALSE;
  }

  /* Set up TLS parameters */

  /*Set server's HOST_NAME*/
  if(z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type == PD_COMMS_IP_ADDR)
  {
  PDCOMM_APP_MSG_ERROR("Unable to set SSL HOST_NAME. None provided",0,0,0);
  return FALSE;
  }
  
  z_ssl_params.server_pdata.host_type = SECSSL_HOST_NAME;
  z_ssl_params.server_pdata.addr.host_name = 
  z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.p_host_name;

  z_ssl_params.server_pdata.port = 
  dss_htons(z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);

  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_SERVER, &z_ssl_params ) )
       != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /* SSL Version */
  z_ssl_params.ssl_version_pdata = SECSSL_SSL_VER_TLS10;

  if(z_handle_table[z_handle.q_handle_id].z_connect_params.e_ssl_ver_tls == SSL_VER_TLS_1_1)
  {
    z_ssl_params.ssl_version_pdata = SECSSL_SSL_VER_TLS11;
  }  

  MSG_HIGH("TLS version being used is %d", z_ssl_params.ssl_version_pdata,0,0);
  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_SSL_VERSION, &z_ssl_params ) )
       != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  MSG_1( MSG_SSID_GPS, MSG_LEGACY_MED, "NV SSL Length = %d", nv_ssl_certificate_key_length);
  /*Set the SSL RSA key size is set to a non-default value */
  if( CGPS_NV_EFS_PDCOMM_SSL_RSA_CERTIFICATE_KEY_LENGTH_DEFAULT > nv_ssl_certificate_key_length)
  {
    z_ssl_params.rsa_min_key_size = nv_ssl_certificate_key_length; 
    e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_RSA_MIN_KEY_SIZE, &z_ssl_params );
	MSG_1( MSG_SSID_GPS, MSG_LEGACY_MED, "Set non-standard min cert key size = %d", nv_ssl_certificate_key_length);

    if( e_error != E_SUCCESS )
    {
	  MSG_1( MSG_SSID_GPS, MSG_LEGACY_ERROR, "PDCOMM APP: Error: %d setting SSL parameter", e_error);
      return FALSE;
    }


    z_ssl_params.dh_min_key_size  = PD_COMM_DHS_MIN_SIZE ;
    e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_DH_MIN_KEY_SIZE, &z_ssl_params );

    if( e_error != E_SUCCESS )
    {
	  MSG_1( MSG_SSID_GPS, MSG_LEGACY_ERROR, "PDCOMM APP: Error: %d setting SSL parameter", e_error);
      return FALSE;
    }
  }

  /* Inject Root Certificate info. Inject either a cert dir or a single cert */
  if( z_handle_table[z_handle.q_handle_id].p_ssl_cert_dir[0] == '\0' )
  {
    MSG_MED("PDCOMM APP: No Root Cert Dir", 0, 0, 0);
    /* If there is no string stored in the SSL cert dir array, check if a single
     * certificate can be injected */
    if( !pd_comms_tcp_task_retrieve_root_certificate( 
         z_handle.q_handle_id,
         (uint8 **)&z_ssl_params.ca_certificate_pdata.cert_data,
         &z_ssl_params.ca_certificate_pdata.cert_len ) )
    {
      MSG_ERROR("PDCOMM APP: Error Retrieving Root Certificate", 0, 0, 0);
      return FALSE;
    }
    if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_CA_CERTIFICATE, &z_ssl_params ) )
         != E_SUCCESS )
    {
      MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
      return FALSE;
    }
  }
  else
  {
    /* If there is a string stored in the SSL cert dir array, pass it to SSL */
    z_ssl_params.ca_cert_folder_pdata.session_ca_cert_folder = 
      z_handle_table[z_handle.q_handle_id].p_ssl_cert_dir;
  
    if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_CA_CERT_FOLDER, &z_ssl_params ) )
         != E_SUCCESS )
    {
      MSG_ERROR("PDCOMM APP: Error: %d setting SSL cert folder", e_error, 0, 0);
      return FALSE;
    }
    else
    {
      MSG_HIGH("PDCOMM APP: Root Cert Dir set", 0, 0, 0);
    }
  }
  
  /* Alert Callback */
  z_ssl_params.alert_cb_pdata.mode = SECSSL_SES_ALERT_MODE_AUTO;
  z_ssl_params.alert_cb_pdata.func_ptr = pd_comms_tcp_task_ssl_alert_callback;

  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_ALERT_CB, &z_ssl_params ) )
       != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /* Event Callback */
  z_ssl_params.event_cb_pdata = pd_comms_tcp_task_ssl_event_callback;
  
  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_EVENT_CB, &z_ssl_params ) )
       != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /* Errno Callback */
  z_ssl_params.errno_cb_pdata.func_ptr = pd_comms_tcp_task_ssl_errno_callback;
  z_ssl_params.errno_cb_pdata.udata_ptr = (void*)&z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id;
  
  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_ERRNO_CB, &z_ssl_params ) )
       != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /*Set the Cipher List to exclude DH_ANON, and Obsolete Cipher Suites*/
  
  for (i =0; i< PD_COMMS_SECSSL_CIPHER_LIST_SIZE_MAX; i++) 
  {
     z_ssl_params.cs_list_pdata.list[i] = pd_comm_sec_ssl_cs_list[i];
  }
  z_ssl_params.cs_list_pdata.size = PD_COMMS_SECSSL_CIPHER_LIST_SIZE_MAX;

  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_CS_LIST, &z_ssl_params ) )
       != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter SECSSL_SES_CS_LIST", e_error, 0, 0);
    return FALSE;
  }
  else
  {
     MSG_HIGH("PDCOM_APP: Supported Cipher Suite's set",0,0,0);
  }

  /* Data Services App ID */
  /*z_ssl_params.ds_app_id_pdata = z_handle_table[z_handle.q_handle_id].x_app_id;*/
  z_ssl_params.ds_app_id_pdata = 0;
  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_DS_APP_ID, &z_ssl_params ) )
       != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  return TRUE;
  
}/*pd_comms_tcp_task_set_ssl_params*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_open_ssl

DESCRIPTION
  This function opens a SSL session for an existing socket connection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_open_ssl(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  secerrno_enum_type e_error;
  secssl_handle_type *p_ssl_handle = &z_handle_table[z_handle.q_handle_id].z_ssl_handle;

  /* Create a new TLS session instance */
  if( ( e_error = secssl_new_session( p_ssl_handle ) ) != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("Could not create new SSL session. Error: %d", e_error, 0, 0);
    (void) pd_comms_tcp_task_teardown_tcp(z_handle);
    return;
  }

  /*Set all the parameters for this SSL session*/
  if(!pd_comms_tcp_task_set_ssl_params(z_handle))
  {
    PDCOMM_APP_MSG_ERROR("Error setting SSL params. Deleting SSL session..",0,0,0); 
    (void)secssl_delete_session( *p_ssl_handle );  
    z_handle_table[z_handle.q_handle_id].z_ssl_handle = NULL;
    (void)pd_comms_tcp_task_teardown_tcp(z_handle);
    
    return;
  }
  
  /* Attempt to open the connection */
  PDCOMM_APP_MSG_MED("Opening SSL Connection for handle_id [%d]",z_handle.q_handle_id,0,0);
  e_error = secssl_open_session( *p_ssl_handle, pd_comms_tcp_task_ssl_open_callback, 
                                 &z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id );

  switch ( e_error )
  {
    case E_ALREADY_DONE:
    {
      /* Opened Successfully */
      PDCOMM_APP_MSG_MED("SSL Connection Established for handle_id [%d]",
                         z_handle.q_handle_id,0,0);
      pd_comms_tcp_task_update_state(z_handle, PDCOMM_STATE_CONNECTED,
                                     TRUE,(uint32)PDCOMM_RESULT_OK);

      break;
    }

    case E_SUCCESS:
    case E_WOULD_BLOCK:
    case E_IN_PROGRESS:
    {
      /* Open Pending */
      PDCOMM_APP_MSG_MED("SSL Connection Establishment Pending for handle_id [%d]",
                     z_handle.q_handle_id,0,0);
      /*pd_comms_tcp_task_update_state(z_handle, PDCOMM_STATE_CONNECTING,
                                     TRUE,(uint32)PDCOMM_RESULT_OK);*/
      break;
    }

    default:
    {
      /* An error occurred */
      MSG_ERROR("PDCOMM APP: Error: %d opening SSL session for handle_id [%d]",
                e_error, z_handle.q_handle_id, 0);
      (void)secssl_delete_session( *p_ssl_handle );
      z_handle_table[z_handle.q_handle_id].z_ssl_handle = NULL; 
      (void)pd_comms_tcp_task_teardown_tcp(z_handle);
      
    }
  }/*switch ( e_error )*/
  
}/*pd_comms_tcp_task_open_ssl*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_secure_tcp_connect

DESCRIPTION
  This function handles a secure TCP connection request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_secure_tcp_connect(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  pd_comms_tcp_task_operation_status_type e_status = FAILED;
  /*Create the regular socket connection first and then OPEN a SSL
    session on that socket connection*/
  e_status  = pd_comms_tcp_task_tcp_connect(z_handle);

  if(e_status == SUCCEEDED)
  {
    pd_comms_tcp_task_open_ssl(z_handle);  
       
  }
   
}/*pd_comms_tcp_task_secure_tcp_connect*/

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
static void  pd_comms_tcp_task_handle_connect(
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

    if(z_handle_table[l_i].z_connect_params.z_link_params.z_tcp_params.ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
    {
      z_handle_table[l_i].ip_addr_type = PD_COMMS_IP_ADDR_IPV4;
    }
    else if(z_handle_table[l_i].z_connect_params.z_link_params.z_tcp_params.ip_addr_type == PD_COMMS_IP_ADDR_IPV6)
    {
      z_handle_table[l_i].ip_addr_type = PD_COMMS_IP_ADDR_IPV6;
    }
    else
    {
      z_handle_table[l_i].ip_addr_type = PD_COMMS_IP_ADDR_UNSPEC;
    }

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
    memscpy(&z_handle_table[l_i].z_pendingConnectReq, sizeof(z_handle_table[l_i].z_pendingConnectReq),
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
FUNCTION pd_comms_tcp_task_setsockopt

DESCRIPTION
  This function sets the DSS_SO_LINGER option to set linger timeout to 2 seconds.
  this is mainly used to close the socket connection immediately and not wait 
  for any callbacks.
  The socket will be kept in waiting state for only 2 seconds after RST is sent to 
  network. The 2 seconds linger time will make sure that the RST is succesfully sent
  to network from the lower layers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_setsockopt(
  /*handle_id corresponding to the connection for which socket 
    option is to be set*/  
  uint32 q_handle_id)
{
  dss_so_linger_type z_dss_so_linger;
  uint32 q_len = sizeof(z_dss_so_linger);
  sint15  x_err_no, x_dss_err_no;          /* err_no from dss_socket2() call */


  if(z_handle_table[q_handle_id].x_sockfd != DSS_ERROR)
  {
    z_dss_so_linger.l_onoff = (int)TRUE;

    /* Set timeout value of 2 seconds to abort the connection */
    z_dss_so_linger.l_linger = PDCOMM_DEFAULT_SOCKET_LINGER_TIME_MSEC;

    x_dss_err_no =  dss_setsockopt( z_handle_table[q_handle_id].x_sockfd,
                                  (int)DSS_SOCK,
                                  (int)DSS_SO_LINGER,
                                  &z_dss_so_linger,
                                  &q_len,
                                  &x_err_no );

    if( x_dss_err_no != DSS_SUCCESS )
    {
      print_dss_err( x_err_no );
    }
    else
    {
      PDCOMM_APP_MSG_HIGH( "Successfully set option handle_id: [%d], sockfd: %d", 
                           q_handle_id, z_handle_table[q_handle_id].x_sockfd, 0);
    }
  }/*if(z_handle_table[q_handle_id].x_sockfd != DSS_ERROR)*/
  else
  {
     MSG_ERROR("PDCOMM APP: Socket already closed before setsockopt for handle [%d]",
               q_handle_id,0,0);
  }

} /*pd_comms_tcp_task_setsockopt() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_dss_abort_connection

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
static void pd_comms_tcp_task_dss_abort_connection(
  /*handle_id corresponding to the connection for which connection is to be aborted*/
  uint32 q_handle_id ,  
  /*in case of abort, whether to notify the client or not*/
  boolean u_notifyClient)
  
{
  sint15 x_err_no;
  sint15 x_dss_retval;
  int16  x_retval;
  pd_comms_disconnect_result_type e_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;  
  pd_comms_notification_params_type z_params;
  pd_comms_return_type e_result;   

  /* Close Socket */
  if( z_handle_table[q_handle_id].x_sockfd != DSS_ERROR )
  {
    pd_comms_tcp_task_setsockopt(q_handle_id);


    /*Delete the IP filter installed previously*/
    if(!pd_comms_app_task_dss_del_filter(q_handle_id))
    {
      PDCOMM_APP_MSG_ERROR("Error deleting IP filter for handle_id[%d]",q_handle_id,0,0);
    }
    
    /*-------------------------------------------------------------------------
      Close the socket.  If an error occurs, check if it is a bad socket
      descriptor, otherwise it is a DS_EWOULDBLOCK so we can continue.
    -------------------------------------------------------------------------*/
    PDCOMM_APP_MSG_MED( "Closing socket for handle_id [%d]", q_handle_id, 0, 0);
    x_dss_retval = dss_close(z_handle_table[q_handle_id].x_sockfd, &x_err_no);

    if(DSS_ERROR == x_dss_retval)
    {
      print_dss_err(x_err_no);
    }
    
    z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;
         
  } /* if sock_fd */

  /*Cleanup any DNS queries/sessions*/
  if( (z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle != 
                                                           DSS_DNS_SESSION_MGR_INVALID_HANDLE)
                                    &&
    (z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle != DSS_DNS_QUERY_INVALID_HANDLE) )
  {
  
    x_retval = dss_dns_delete_session(z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle, 
                    &x_err_no);
    if( DSS_ERROR == x_retval )
    {
      MSG_ERROR("DNS session %d could not be deleted", 
        z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle,0,0);
    }
    /*Cleanup DNS related variables*/
    z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle = 
                                                           DSS_DNS_SESSION_MGR_INVALID_HANDLE;
    z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
  }
  
  /* Before doing anything else sendout a DISCONNECTED state to client
   * After that proceed silently (without updating client) with the disconnect
   * process*/ 
  if(u_notifyClient)
  {
    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_RESULT_OK;

    e_result = pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                         z_handle_table[q_handle_id].q_source_thread_id,
                         z_handle_table[q_handle_id].q_base_msg_id,
                         z_handle_table[q_handle_id].q_disconnect_transaction_id,
                         STATE_CHANGE,&z_params);  
    
    if(e_result != PDCOMM_RESULT_OK)
    {
      MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                (uint32)e_result,q_handle_id,0);
    
    }
  }
  
  /* Proceed with Closing PPP Connection */
  if(pd_comms_tcp_task_close_ppp(q_handle_id, &e_disc_error) == PENDING)
  {
    /*Change handle state to DISCONNECTING*/    
    pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTING,
                                   FALSE,(uint32)PDCOMM_RESULT_WAIT);
  }
  else
  {
    /*Change handle state to DISCONNECTED*/
    pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                   FALSE,(uint32)PDCOMM_RESULT_WAIT);    
  }
  
} /* pd_comms_tcp_task_dss_abort_connection*/


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
static void pd_comms_tcp_task_tcp_disconnect(
  /*handle corresponding to the connection for which connection is to be disconnected*/
  pd_comms_handle_type z_handle,
  /*to abort or gracefully close a connection*/
  boolean b_abort,
  /*in case of abort, whether to notify the client or not*/
  boolean u_notifyClient)
{
  pd_comms_disconnect_result_type e_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;
  pd_comms_tcp_task_operation_status_type e_status;
  int16                            x_retval;
  boolean                          u_close_ppp = TRUE;

  if(b_abort)
  {
    PDCOMM_APP_MSG_MED("TCP connection abort received for handle_id[%d]",z_handle.q_handle_id,0,0);  
    
    z_handle_table[z_handle.q_handle_id].session_aborted = TRUE;

    pd_comms_tcp_task_dss_abort_connection( z_handle.q_handle_id , u_notifyClient);

    return;
  }
  if(z_handle_table[z_handle.q_handle_id].u_use_laptop_call == TRUE)
  {                                                                
    u_close_ppp = FALSE;
  }

  switch(z_handle_table[z_handle.q_handle_id].e_link_status)
  {
    case PDCOMM_STATE_DISCONNECTED:
    {
      break;
    }

    case PDCOMM_STATE_OPENING:
    {
     if(u_close_ppp)
     {
       e_status = pd_comms_tcp_task_close_ppp(z_handle.q_handle_id, &e_disc_error);
     }
     else
     {
       e_status = SUCCEEDED;
     }

     if(e_status == SUCCEEDED)
     {
       pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                    TRUE,(uint32) PDCOMM_RESULT_OK);
     }
     else if(e_status == PENDING)
     {
       pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                    TRUE,(uint32) PDCOMM_RESULT_WAIT);
     }
     else if(e_status == FAILED)
     {
       pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                    TRUE,(uint32) e_disc_error);
     }

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
      sint15 x_errno;

      (void)dss_async_deselect(z_handle_table[z_handle.q_handle_id].x_sockfd,
                         DS_READ_EVENT | DS_WRITE_EVENT,
                         &x_errno);

      /*Cleanup any DNS queries/sessions*/
      if( (z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_session_handle != 
           DSS_DNS_SESSION_MGR_INVALID_HANDLE) && 
        (z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_query_handle != DSS_DNS_QUERY_INVALID_HANDLE) )
      {
      
        x_retval = dss_dns_delete_session(z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_session_handle, 
                                          &x_errno);
        if( DSS_ERROR == x_retval )
        {
          MSG_ERROR("DNS session %d could not be deleted", 
                    z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_session_handle,0,0);
        }
        /*Cleanup DNS related variables*/
        z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_session_handle = 
                                                               DSS_DNS_SESSION_MGR_INVALID_HANDLE;
        z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
      }


      e_status = pd_comms_tcp_task_close_socket(z_handle.q_handle_id,&e_disc_error,u_close_ppp);
      if(e_status == SUCCEEDED)
      {
        pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32) PDCOMM_RESULT_OK);
      }
      else if(e_status == PENDING)
      {
        pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                     TRUE,(uint32) PDCOMM_RESULT_WAIT);
      }
      else if(e_status == FAILED)
      { 
        pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                     TRUE,(uint32) e_disc_error);
        e_status = pd_comms_tcp_task_close_ppp(z_handle.q_handle_id, &e_disc_error);

        if(e_status == SUCCEEDED)
        {
          pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32) PDCOMM_RESULT_OK);
        }
        else if(e_status == PENDING)
        {
          pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                       TRUE,(uint32) PDCOMM_RESULT_WAIT);
        }
        else if(e_status == FAILED)
        {
          pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32) e_disc_error);
        }
      }

      break;
    }

  } /* switch(link_status) ends*/

} /*pd_comms_tcp_task_tcp_disconnect() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_secure_tcp_disconnect

DESCRIPTION
  This function handles a secure TCP link disconnect request. If abort option
  is set to true, the connection will be immediately closed and handle released. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_secure_tcp_disconnect(
  /*handle corresponding to the connection for which connection is to be disconnected*/
  pd_comms_handle_type z_handle, 
  /*to abort or gracefully close a connection*/
  boolean b_abort,
  /*to notify client about the aborted connection or not*/
  boolean u_notifyClient)
{
  pd_comms_disconnect_result_type e_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;
  sint15 x_err_no;
  int16  x_retval;
  boolean u_close_ppp = TRUE;
  pd_comms_tcp_task_operation_status_type e_status;
  
  if(z_handle_table[z_handle.q_handle_id].u_use_laptop_call == TRUE)
  {                                                                
    u_close_ppp = FALSE;
  }
  if(b_abort)
  {
    PDCOMM_APP_MSG_HIGH("Abort received for handle_id [%d]",
                        z_handle.q_handle_id,0,0); 
    z_handle_table[z_handle.q_handle_id].session_aborted = TRUE;

    if(u_notifyClient)
    {
      pd_comms_notification_params_type z_params;
      pd_comms_return_type e_result;   
      
      z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
      z_params.q_result = (uint32)PDCOMM_RESULT_OK;

      /* Before doing anything else sendout a DISCONNECTED state to client
       * After that proceed silently (without updating client) with the disconnect
       * process*/ 

      e_result = pd_comms_notify_client(z_handle,
                           z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                           z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                           z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                           STATE_CHANGE,&z_params);  

      if(e_result != PDCOMM_RESULT_OK)
      {
        MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                  (uint32)e_result,z_handle.q_handle_id,0);

      }
    }/*if(u_notifyClient)*/
  }/*if(b_abort)*/



  switch(z_handle_table[z_handle.q_handle_id].e_link_status)
  {
    case PDCOMM_STATE_DISCONNECTED:
    {
      break;
    }

    case PDCOMM_STATE_OPENING:
    {
      
     if(u_close_ppp)
     {
       e_status = pd_comms_tcp_task_close_ppp(z_handle.q_handle_id, &e_disc_error);
     }
     else
     {
       e_status = SUCCEEDED;
     }
     if(e_status == SUCCEEDED)
     {
       pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                    TRUE,(uint32) PDCOMM_RESULT_OK);
     }
     else if(e_status == PENDING)
     {
       pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                    TRUE,(uint32) PDCOMM_RESULT_WAIT);
     }
     else if(e_status == FAILED)
     {
       pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                    TRUE,(uint32) e_disc_error);
     }
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
      secerrno_enum_type e_error;

      /*Cleanup any DNS queries/sessions*/
      if( (z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_session_handle != 
                                                               DSS_DNS_SESSION_MGR_INVALID_HANDLE)
                                        &&
        (z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_query_handle != DSS_DNS_QUERY_INVALID_HANDLE) )
      {
      
        x_retval = dss_dns_delete_session(z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_session_handle, 
                        &x_err_no);
        if( DSS_ERROR == x_retval )
        {
          MSG_ERROR("DNS session %d could not be deleted", 
            z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_session_handle,0,0);
        }
        /*Cleanup DNS related variables*/
        z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_session_handle = 
                                                               DSS_DNS_SESSION_MGR_INVALID_HANDLE;
        z_handle_table[z_handle.q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
      } 
      
      if(z_handle_table[z_handle.q_handle_id].z_ssl_handle != NULL)
      {
      
        e_error = secssl_close_session( z_handle_table[z_handle.q_handle_id].z_ssl_handle,
                                        pd_comms_tcp_task_ssl_close_callback,
                                        &z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id);   

        if( e_error != E_SUCCESS )
        {
          MSG_ERROR("PDCOMM APP: Could not close SSL session. Error: %d", e_error, 0, 0);

          if(!b_abort)
          {
            pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                           TRUE,(uint32)e_disc_error);
          }
          else
          {
            /*update local state but donot inform client*/
            pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                           FALSE,(uint32)e_disc_error);
          }

          (void)secssl_delete_session(z_handle_table[z_handle.q_handle_id].z_ssl_handle);
          z_handle_table[z_handle.q_handle_id].z_ssl_handle = NULL;
          /*disconnect external socket and bring down PPP*/
          (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[z_handle.q_handle_id].z_handle); 
        }
        else
        {
          if(!b_abort)
          {
            pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                           TRUE,(uint32)PDCOMM_RESULT_WAIT);
          }
          else
          {
            /*update local state but donot inform client*/
            pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                           FALSE,(uint32)PDCOMM_RESULT_WAIT);
          }
        }
      }
      else
      {
        /*disconnect external socket and bring down PPP*/
        (void)pd_comms_tcp_task_teardown_tcp(z_handle_table[z_handle.q_handle_id].z_handle);   
      }

      break;
    }

  } /* switch(link_status) ends*/

} /*pd_comms_tcp_task_secure_tcp_disconnect() ends*/


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
FUNCTION pd_comms_tcp_task_tcp_read

DESCRIPTION
  This function is called when a DS_READ_EVENT socket event occurs which 
  indicates presence of incoming data to be read. It tries to call dss_read()
  untill all data present in data services buffer is completely read. For
  each PDCOMM_READ_DATA_BUF_SIZE bytes of data read, this data will be sent
  to the client in a IPC message. 

DEPENDENCIES
  None

RETURN VALUE
  boolean - status of data read

SIDE EFFECTS
  None
===========================================================================*/
static boolean pd_comms_tcp_task_tcp_read(
  /*handle_id corresponding to the connection for which data is to be read*/
  uint32 q_handle_id,
  /*wait for all requested data before sending it to client or not*/
  boolean u_wait_for_all) 
{
  sint15 x_err_no;
  uint8 *p_read_data;
  sint15 x_bytes_read;
  boolean result = TRUE;                        
  uint16 *p_num_bytes_to_read;
  uint16 *p_num_bytes_already_read;
  sint31 x_retval;
  pd_comms_read_result_type e_read_error = PDCOMM_READ_ERROR_UNKNOWN;  
  struct ps_sockaddr *sock_addr = NULL;
  uint16 sock_addr_len =0;
  boolean u_close_ppp = TRUE;

  p_read_data = &z_handle_table[q_handle_id].p_read_buff[0];
  p_num_bytes_to_read = &z_handle_table[q_handle_id].w_num_bytes_to_read;
  p_num_bytes_already_read = &z_handle_table[q_handle_id].w_num_bytes_already_read;
  
  /* Retrieve up to max_bytes from the socket */
  if(z_handle_table[q_handle_id].u_use_laptop_call == TRUE)
  {
    u_close_ppp = FALSE;
  }

  if(z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
  {
    sock_addr = (struct ps_sockaddr *)&(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4);
    sock_addr_len = sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4);
  }
  else
  {  
    sock_addr = (struct ps_sockaddr *)&(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6);
    sock_addr_len = sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6);
  }


  if(z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_TCP)
  {
    x_bytes_read = dss_read(z_handle_table[q_handle_id].x_sockfd,
                            &p_read_data[*p_num_bytes_already_read],
                            *p_num_bytes_to_read - *p_num_bytes_already_read,
                            &x_err_no );
  }
  else
  {
      x_bytes_read = dss_recvfrom( z_handle_table[q_handle_id].x_sockfd,            /* socket descriptor */
                       &p_read_data[*p_num_bytes_already_read],                     /* user buffer from which to copy the data */
                       *p_num_bytes_to_read - *p_num_bytes_already_read,            /* number of bytes to be written */
                       0,                                                           /* unused */
                       sock_addr,                                                   /* destination address */
                       &sock_addr_len,                                              /* address length */
                       &x_err_no );                                                 /* error condition value */
  }
  
  if(x_bytes_read >= 0)
  {
    if( (0 == x_bytes_read) && ( DS_EEOF == x_err_no) )
    {
      PDCOMM_APP_MSG_MED("Socket Read - Server closed connection",0,0,0);
      e_read_error = PDCOMM_READ_ERROR_INTERNAL;
      result = FALSE;
    }
    else
    {
      *p_num_bytes_already_read += (uint16)x_bytes_read;
      PDCOMM_APP_MSG_MED("Recevied %d bytes for handle_id [%d]",x_bytes_read,q_handle_id,0);  
       
      if((*p_num_bytes_already_read == *p_num_bytes_to_read) || (FALSE == u_wait_for_all))
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
      } /*if((*p_num_bytes_already_read == *p_num_bytes_to_read) || (FALSE == u_wait_for_all)) ends*/        
      
      /*re-enable the delivery of READ event*/
      x_retval = dss_async_select( z_handle_table[q_handle_id].x_sockfd, DS_READ_EVENT, &x_err_no );
      
      if(DSS_ERROR == x_retval)
      {
        print_dss_err(x_err_no);
        result = FALSE;
        *p_num_bytes_already_read = *p_num_bytes_to_read = 0;
      }

    } /*else for if( (0 == x_bytes_read) && ( DS_EEOF == x_err_no) ) ends*/
  } /*if(x_bytes_read >= 0) ends*/
  else if( x_bytes_read == DSS_ERROR )
  {
    switch (x_err_no)
    {
      case DS_EBADF:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - Invalid Socket Descriptor: %d for handle_id [%d]",
                  z_handle_table[q_handle_id].x_sockfd,q_handle_id,0);
        e_read_error = PDCOMM_READ_ERROR_INTERNAL;
        result = FALSE;
        break;
      }
  
      case DS_ENOTCONN:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - No TCP Connection for handle_id [%d]",
                   q_handle_id,0,0);
        e_read_error = PDCOMM_READ_ERROR_TCP_CONNECTION_LOST;
        result = FALSE;
        break;
      }

      case DS_ECONNRESET:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - TCP Connection Reset By Server for handle_id [%d]",
                  q_handle_id,0,0);
        e_read_error = PDCOMM_READ_ERROR_TCP_CONNECTION_LOST;
        result = FALSE;
        break;
      }

      case DS_ECONNABORTED:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - TCP Connection Aborted for handle_id [%d]",
                  q_handle_id,0,0);
        e_read_error = PDCOMM_READ_ERROR_TCP_CONNECTION_LOST;
        result = FALSE;
        break;
      }

      case DS_EIPADDRCHANGED:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - PPP Resync caused IP Address Change for handle_id [%d]",
                  q_handle_id,0,0);
        e_read_error = PDCOMM_READ_ERROR_TCP_CONNECTION_LOST;
        result = FALSE;
        break;
      }

      case DS_EPIPE:
      { 
        MSG_ERROR("PDCOMM APP: Socket Read - Peer closed TCP connection for handle_id [%d]",
                  q_handle_id,0,0);
        e_read_error = PDCOMM_READ_ERROR_TCP_CONNECTION_LOST;
        result = FALSE;
        break;
      }

      case DS_ENETDOWN:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - Network Subsystem Unavailable for handle_id [%d]",
                  q_handle_id,0,0);
        e_read_error = PDCOMM_READ_ERROR_PPP_CONNECTION_LOST;
        result = FALSE;
        break;
      }

      case DS_EFAULT:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - App Buff Not Valid Part Of Addr Space for handle_id [%d]",
                  q_handle_id,0,0);
        e_read_error = PDCOMM_READ_ERROR_INTERNAL;
        result = FALSE;
        break;
      }
  
      case DS_EWOULDBLOCK:
      {
        /* No more data available */
        if((*p_num_bytes_already_read == *p_num_bytes_to_read) || (FALSE == u_wait_for_all))
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
          result = TRUE;
        } 
        
        /*re-enable the delivery of READ event*/ 
        x_retval = dss_async_select( z_handle_table[q_handle_id].x_sockfd, DS_READ_EVENT, &x_err_no );
        
        if(DSS_ERROR == x_retval)
        {
          print_dss_err(x_err_no);
          result = FALSE;
          *p_num_bytes_already_read = *p_num_bytes_to_read = 0;
        }
        else
        {
          result = TRUE;
        }
        break;
      }
          
      case DS_EEOF:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - EOF recevied for handle_id [%d]",
                           q_handle_id,0,0);
        e_read_error = PDCOMM_READ_ERROR_INTERNAL;
        result = FALSE;
        break;
      }
  
      default:
      {
        MSG_ERROR("PDCOMM APP: Socket Read - Unknown Error: %d for handle_id [%d]",
                  x_err_no,q_handle_id,0);
        e_read_error = PDCOMM_READ_ERROR_UNKNOWN;
        result = FALSE;
        break;
      }
    }

  }/*if( bytes_read == DSS_ERROR ) ends*/


  if(result == FALSE)
  {
     if (z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_TCP) 
     {
       pd_comms_disconnect_result_type e_disc_error;
       pd_comms_tcp_task_operation_status_type e_status;
       sint15 x_errno;

       *p_num_bytes_already_read = *p_num_bytes_to_read = 0;
       (void)dss_async_deselect(z_handle_table[q_handle_id].x_sockfd,
                          DS_READ_EVENT | DS_WRITE_EVENT,&x_errno);
       
       /* An error has occurred. Set link to disconnecting state */
       pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                    PDCOMM_STATE_DISCONNECTING,
                                    TRUE,(uint32)e_read_error); 
       
       /* Close the TCP/IP connection */ 
       e_status = pd_comms_tcp_task_close_socket(q_handle_id,&e_disc_error,u_close_ppp);

       if( e_status == SUCCEEDED)
       {
         pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                      PDCOMM_STATE_DISCONNECTED,
                                      TRUE,(uint32) PDCOMM_RESULT_OK);
       }
       else if( e_status == FAILED )
       {
         /* If the socket close is not pending then either the socket is closed or an error
          * occurred. Either way teardown the PPP connection */
         e_status = pd_comms_tcp_task_close_ppp(q_handle_id,&e_disc_error);

         if(e_status == SUCCEEDED)
         {
           pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                        PDCOMM_STATE_DISCONNECTED,
                                        TRUE,(uint32) PDCOMM_RESULT_OK);
         }
         else if(e_status == PENDING)
         {
           pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTING,
                                        TRUE,(uint32) PDCOMM_RESULT_WAIT);
         }
         else if(e_status == FAILED)
         {
           pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32) e_disc_error);
         }

       }/* if( status != PENDING) ends */ 
     }
     else if (z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_UDP)
     {
        pd_comms_notification_params_type z_params;
        z_params.e_new_state = PDCOMM_STATE_CONNECTED;
        z_params.q_result = PDCOMM_READ_ERROR_INTERNAL;

        PDCOMM_APP_MSG_MED("Socket Read Error - Sending ERR report, keep port open",0,0,0);
        /*A READ error has occured for UDP socket. Do not close the socket*/
        (void) pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                             z_handle_table[q_handle_id].q_source_thread_id,
                             z_handle_table[q_handle_id].q_base_msg_id,
                             z_handle_table[q_handle_id].q_read_transaction_id,
                             ERROR_RPT,&z_params); 
        /*re-enable the delivery of READ event*/
        x_retval = dss_async_select( z_handle_table[q_handle_id].x_sockfd, DS_READ_EVENT, &x_err_no );
        *p_num_bytes_already_read = *p_num_bytes_to_read = 0;
        if(DSS_ERROR == x_retval)
        {
           print_dss_err(x_err_no);           
        }         
     }
     else
     {
        PDCOMM_APP_MSG_MED("Read Error - for non TCP/UDP connection not handled here",0,0,0);
     }

  }/*if(result == FALSE) ends*/

  return result;
} /*pd_comms_tcp_task_tcp_read() ends*/


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
FUNCTION pd_comms_tcp_task_create_policy_for_iface

DESCRIPTION
  This function creates a policy for a particular iface id

DEPENDENCIES
  None

RETURN VALUE
  uint8: Index of  UDP listener

SIDE EFFECTS
  None
===========================================================================
static void pd_comms_tcp_task_create_policy_for_iface(dss_net_policy_info_type *p_policy_info,
                                                      dss_iface_id_type iface_id)
{
  if(p_policy_info == NULL)
  {
    MSG_ERROR("Null policy info pointer",0,0,0);
    return;
  }
  
  dss_init_net_policy_info(p_policy_info);
  p_policy_info->iface.kind       = DSS_IFACE_ID;
  p_policy_info->policy_flag      = DSS_IFACE_POLICY_SPECIFIED;
  p_policy_info->is_routeable     = TRUE;   //IP filter 
  p_policy_info->iface.info.id  = iface_id;

  p_policy_info->family           = DSS_AF_UNSPEC;

#if ((defined FEATURE_DATA_PS_OMH) || (DSS_VERSION >= 1800))
  p_policy_info->app_identifier = LBS_APP_TYPE ;
#endif //FEATURE_DATA_PS_OMH 

  return;
}
*/
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
static boolean pd_comms_tcp_task_udp_bind(
  /*handle_id corresponding to the connection for which bind is needed*/
  uint32 q_handle_id)
{  
  sint15 x_err_no;
  dss_net_policy_info_type  z_policy_info; 
  struct ps_sockaddr_in6       local_addr;
  struct ps_in6_addr ps_in6addr_any = PS_IN6ADDR_ANY_INIT;
  ip_filter_type        rx_fltr[NUM_RX_FLTRS]; 

  /*Handle only UDP socket binds here*/
  if( q_handle_id < PDCOMM_MAX_HANDLES && 
    z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_UDP)
  {
    /*Removed default listener implementation from here*/
    /*Removed UDP listeners since we have a global filter now.*/
    PDCOMM_APP_MSG_MED( "x_sockfd: 0x%x", z_handle_table[q_handle_id].x_sockfd, 0, 0);
    /*Do socket operation only if not done before*/
    if(DSS_ERROR == z_handle_table[q_handle_id].x_sockfd)
    {    
      /*-------------------------------------------------------------------------
        Creates a UDP socket.  If an error occured sockfd will be set
        to DSS_ERROR and we can determine what was wrong.
      -------------------------------------------------------------------------*/
      PDCOMM_APP_MSG_MED( "Creating socket for handle id [%d]", q_handle_id, 0, 0);
      
      /* Change made to use no DS API to set policy (i.e. use 1x CDMA only) */
      pd_comms_tcp_task_dss_get_policy(&z_policy_info, q_handle_id);
      z_policy_info.family = DSS_AF_UNSPEC;
      z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_family = (uint16)DSS_AF_INET6;
      z_handle_table[q_handle_id].x_sockfd 
                = dss_socket2( DSS_AF_INET6,
                               DSS_SOCK_DGRAM,
                               PS_IPPROTO_UDP,
                               pd_comms_tcp_task_socket_callback,
                               (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id),
                               &z_policy_info,
                               &x_err_no);

      PDCOMM_APP_MSG_MED( "x_sockfd created: 0x%x", z_handle_table[q_handle_id].x_sockfd, 0, 0);

      if(DSS_ERROR == z_handle_table[q_handle_id].x_sockfd)
      {      
        pd_comms_notification_params_type z_params;
        
        z_params.e_new_state = z_handle_table[q_handle_id].e_link_status;
        z_params.q_result = (uint32)PDCOMM_CONNECT_ERROR_SOCKET_CONNECTION_FAILED;
                                       
        PDCOMM_APP_MSG_HIGH("Socket creation failed for handle_id[%d]",
                               q_handle_id,0,0);
        
        (void)pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                               z_handle_table[q_handle_id].q_source_thread_id,
                               z_handle_table[q_handle_id].q_base_msg_id,
                               z_handle_table[q_handle_id].z_connect_params.q_transaction_id,
                               ERROR_RPT,&z_params);   
        return FAILED;
      }

      /*Set socket option*/
      {
        sint31 x_retval;   /* used to hold the return value from dss functions */
        sint15 x_err_no      = DS_ENOTCONN;
        
        uint32 sys_sock= 1;
        uint32 reuse_addr = 1;
        uint32 skip_route = 1;
        uint32 q_len = sizeof(uint32);
              
        x_retval =  dss_setsockopt( z_handle_table[q_handle_id].x_sockfd,
                      DSS_SOCK,
                      DSS_SO_SYS_SOCK,
                      &sys_sock,
                      &q_len,
                      &x_err_no );
        
         if( x_retval == DSS_ERROR )
         {
            PDCOMM_APP_MSG_ERROR( "Set Socket option DSS_SO_SYS_SOCK failed", z_handle_table[q_handle_id].x_sockfd, x_err_no, 0 );
            (void)dss_close(z_handle_table[q_handle_id].x_sockfd, &x_err_no);
            z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;
            return FAILED;
         }
        
        x_retval =  dss_setsockopt( z_handle_table[q_handle_id].x_sockfd,
                      DSS_SOCK,
                      DSS_SO_REUSEADDR,
                      &reuse_addr,
                      &q_len,
                      &x_err_no );
        
         if( x_retval == DSS_ERROR )
         {
            PDCOMM_APP_MSG_ERROR( "Set Socket option DSS_SO_REUSEADDR failed", z_handle_table[q_handle_id].x_sockfd, x_err_no, 0 );
            (void)dss_close(z_handle_table[q_handle_id].x_sockfd, &x_err_no);
            z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;
            return FAILED;
         }

         x_retval =  dss_setsockopt( z_handle_table[q_handle_id].x_sockfd,
                      DSS_SOCK,
                      DSS_SO_SKIP_ROUTE_SCOPE_CHECK,
                      &skip_route,
                      &q_len,
                      &x_err_no );
         if( x_retval == DSS_ERROR )
         {
            PDCOMM_APP_MSG_ERROR( "Set Socket option DSS_SO_SKIP_ROUTE_SCOPE_CHECK failed", z_handle_table[q_handle_id].x_sockfd, x_err_no, 0 );
            (void)dss_close(z_handle_table[q_handle_id].x_sockfd, &x_err_no);
            z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;
            return FAILED;
         }
      }


      memset((char *)&(local_addr), 0, sizeof(local_addr));


      local_addr.ps_sin6_family = DSS_AF_INET6; //Since Ipv6 socket will support Ipv4 connections
      local_addr.ps_sin6_port = dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);
      local_addr.ps_sin6_addr = ps_in6addr_any;
      local_addr.ps_sin6_flowinfo = 0;

      if ( DSS_ERROR == dss_bind( z_handle_table[q_handle_id].x_sockfd,
                                  (struct ps_sockaddr*)&local_addr,
                                  sizeof(struct ps_sockaddr_in6),
                                  &x_err_no) )
      {
        pd_comms_notification_params_type z_params;
        
        z_params.e_new_state = z_handle_table[q_handle_id].e_link_status;
        z_params.q_result = (uint32)PDCOMM_CONNECT_ERROR_SOCKET_CONNECTION_FAILED;
                                       
        PDCOMM_APP_MSG_HIGH("Socket bind failed for handle_id[%d]",
                               q_handle_id,0,0);
        
        (void)pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                               z_handle_table[q_handle_id].q_source_thread_id,
                               z_handle_table[q_handle_id].q_base_msg_id,
                               z_handle_table[q_handle_id].z_connect_params.q_transaction_id,
                               ERROR_RPT,&z_params);

        (void)dss_close(z_handle_table[q_handle_id].x_sockfd, &x_err_no);
        z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;
        return FAILED;
      } 

      PDCOMM_APP_MSG_HIGH("Socket bind succeded for handle_id[%d]",
                             q_handle_id,0,0);
      pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, PDCOMM_STATE_CONNECTED,
                                     TRUE,(uint32)PDCOMM_RESULT_OK);


      if ( dss_async_select( z_handle_table[q_handle_id].x_sockfd , DS_READ_EVENT, &x_err_no ) != DSS_SUCCESS)
      {
        PDCOMM_APP_MSG_ERROR( "Failed async select READ on %x %d", z_handle_table[q_handle_id].x_sockfd, x_err_no, 0 );
        (void)dss_close(z_handle_table[q_handle_id].x_sockfd, &x_err_no);
        z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;        
        return FAILED;
      }

    memset(rx_fltr, 0, sizeof(ip_filter_type)*NUM_RX_FLTRS);
  
    rx_fltr[0].ip_vsn = IP_V4;
    rx_fltr[0].ip_hdr.v4.field_mask         = IPFLTR_MASK_IP4_NEXT_HDR_PROT;
    rx_fltr[0].ip_hdr.v4.next_hdr_prot      = PS_IPPROTO_UDP;
    rx_fltr[0].next_prot_hdr.udp.field_mask = IPFLTR_MASK_UDP_DST_PORT;
    rx_fltr[0].next_prot_hdr.udp.src.port   = 0;
    rx_fltr[0].next_prot_hdr.udp.dst.port   = dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);
    
    
    /*-------------------------------------------------------------------------
      Request Rx Filter for Ipv4
    -------------------------------------------------------------------------*/
    if ( dss_reg_ip_filter_by_group(z_handle_table[q_handle_id].x_sockfd, 
                            DSS_IFACE_EPC_ANY, 
                          IPFLTR_DEFAULT_TYPE, 
                          NUM_RX_FLTRS,
                          (void *)rx_fltr,
                          &x_err_no) == DSS_ERROR )
    {
      PDCOMM_APP_MSG_MED("error process dss_iface_ioctl request for Rx fltr",0,0,0);
    
      print_dss_err(x_err_no);
      
      if( x_err_no == DS_EFAULT || x_err_no == DS_EINVAL )
      {
        PDCOMM_APP_MSG_MED("Unable to install Rx fltr",0,0,0);
      }      
    }
    else
    {
      PDCOMM_APP_MSG_MED("Ipv4 Rx Filter installed",0,0,0);
    }


    
    memset(rx_fltr, 0, sizeof(ip_filter_type)*NUM_RX_FLTRS);
    
    rx_fltr[0].ip_vsn = IP_V6;
    rx_fltr[0].ip_hdr.v6.field_mask         =  IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    rx_fltr[0].ip_hdr.v6.next_hdr_prot      = PS_IPPROTO_UDP;      
    rx_fltr[0].next_prot_hdr.udp.field_mask = IPFLTR_MASK_UDP_DST_PORT;
    rx_fltr[0].next_prot_hdr.udp.src.port   = 0;
    rx_fltr[0].next_prot_hdr.udp.dst.port   = dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);
    
    /*-------------------------------------------------------------------------
      Request Rx Filter for Ipv6
    -------------------------------------------------------------------------*/
      if ( dss_reg_ip_filter_by_group( z_handle_table[q_handle_id].x_sockfd, 
                              DSS_IFACE_EPC_ANY, 
                            IPFLTR_DEFAULT_TYPE, 
                            NUM_RX_FLTRS,
                            (void *)rx_fltr,
                            &x_err_no) == DSS_ERROR )
    {
      PDCOMM_APP_MSG_MED("error process dss_iface_ioctl request for Rx fltr",0,0,0);
    
      print_dss_err(x_err_no);
      
      if( x_err_no == DS_EFAULT || x_err_no == DS_EINVAL )
      {
        PDCOMM_APP_MSG_MED("Unable to install Rx fltr",0,0,0);
      }      
    }
    else
    {
      PDCOMM_APP_MSG_MED("Ipv6 Rx Filter installed",0,0,0);
    }  

    return SUCCEEDED;
   } //making sure we dont install RX filters more than once.
   else
   {
      PDCOMM_APP_MSG_ERROR("Socket already created: x_sock_fd: [%d]",z_handle_table[q_handle_id].x_sockfd,0,0);
      PDCOMM_APP_MSG_ERROR("Ipv4/v6 Rx Filter should be installed dont install again",0,0,0);
      return SUCCEEDED;
   }
  }
  else
  {
    PDCOMM_APP_MSG_ERROR( "Wrong Handle Id or Link Type", 
        q_handle_id, z_handle_table[q_handle_id].z_connect_params.e_link_type, 0 );
    return FAILED;
  }
}

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
  boolean result = FAILED;
  
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
        result = pd_comms_tcp_task_udp_bind(z_handle.q_handle_id); 
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
    //Do not release yet, there could be socket call backs so for now mark the handle for release.
    z_handle_table[q_handle_id].v_release_handle = TRUE;    
  }/*else for if(z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_DISCONNECTED)*/
  
  os_MutexUnlock(&z_gethandle_mutex);
  MSG_MED("PDCOMM APP:GetHandle Mutex unlocked",0,0,0); 
  return PDCOMM_RESULT_OK;
} /*pd_comms_tcp_task_handle_release_handle*/


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
static boolean pd_comms_tcp_task_tcp_write(
  /*handle corresponding to the connection for which data is to be written*/
  pd_comms_handle_type z_handle)
{
  sint15 l_bytes_written_temp;
  sint15 x_err_no;
  uint16 *w_num_bytes_written = &z_handle_table[z_handle.q_handle_id].w_num_bytes_written;
  pd_comms_write_result_type e_write_error = PDCOMM_WRITE_ERROR_UNKNOWN;
  struct ps_sockaddr *sock_addr = NULL;
  uint16 sock_addr_len =0;
  boolean result;

  if(z_handle_table[z_handle.q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
  {
    sock_addr = (struct ps_sockaddr *)&(z_handle_table[z_handle.q_handle_id].z_peer_addr.z_peer_addr_v4);
    sock_addr_len = sizeof(z_handle_table[z_handle.q_handle_id].z_peer_addr.z_peer_addr_v4);
  }
  else
  {  
    sock_addr = (struct ps_sockaddr *)&(z_handle_table[z_handle.q_handle_id].z_peer_addr.z_peer_addr_v6);
    sock_addr_len = sizeof(z_handle_table[z_handle.q_handle_id].z_peer_addr.z_peer_addr_v6);
  }

  /* Attempt to write length bytes to socket. bytes_written will return how many
     bytes actually got sent. */ 
  if( z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type == LINK_TCP)
  {
    l_bytes_written_temp = dss_write( z_handle_table[z_handle.q_handle_id].x_sockfd,
                                      &z_handle_table[z_handle.q_handle_id].p_write_buff[*w_num_bytes_written],
                                      z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write - *w_num_bytes_written,
                                      &x_err_no );
  }
  else if(z_handle_table[z_handle.q_handle_id].z_connect_params.e_link_type == LINK_UDP)
  {
    pd_comms_tcp_task_handle_data_type *p_handle = &z_handle_table[z_handle.q_handle_id];

    l_bytes_written_temp = dss_sendto( p_handle->x_sockfd,
                             &p_handle->p_write_buff[*w_num_bytes_written],
                             p_handle->w_num_bytes_to_write - *w_num_bytes_written,
                             0,                          /* We dont intend to use Short Data Burst (SDB)   */
                             sock_addr,                  /* destination address */
                             sock_addr_len,             /* address length */
                             &x_err_no );
  }
  else
  {
    return FALSE;
  }
  
  if( l_bytes_written_temp >= 0 )
  {
    sint31 x_retval;

    *w_num_bytes_written += (uint16)l_bytes_written_temp;

    if(*w_num_bytes_written == z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write)
    {
      pd_comms_write_ack_params_type z_params;
      z_params.q_bytes_written = z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write;
      z_params.q_transaction_id = z_handle_table[z_handle.q_handle_id].q_write_transaction_id;

      /* Acknowledge the write */
      (void)pd_comms_send_write_ack_to_client(z_handle,
                                        z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                        z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                        &z_params);
      *w_num_bytes_written = 0;
      z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;

    }

    x_retval = dss_async_select( z_handle_table[z_handle.q_handle_id].x_sockfd, 
                               DS_WRITE_EVENT | DS_CLOSE_EVENT,
                               &x_err_no);
    if(DSS_ERROR == x_retval)
    {
      print_dss_err(x_err_no);
      e_write_error = PDCOMM_WRITE_ERROR_WRITE_INTERNAL_ERROR;
      result = FALSE;
      *w_num_bytes_written = 0;
      z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
    }
    else
    {
      result = TRUE;
    }
    
  }/*if( l_bytes_written_temp >= 0 )*/
  else
  {
    /* A write error has occurred */

    switch( x_err_no )
    {
      case DS_EBADF:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - Invalid Socket Descriptor: %d",
                  z_handle_table[z_handle.q_handle_id].x_sockfd,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_BAD_BUFFER;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

      case DS_ENOTCONN:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - No TCP Connection",0,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_WRITE_TCP_CONNECTION_LOST;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

      case DS_ECONNRESET:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - TCP Connection Reset By Server",0,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_WRITE_TCP_CONNECTION_LOST;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

      case DS_ECONNABORTED:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - TCP Connection Aborted",0,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_CONNECTION_ABORTED;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

      case DS_EIPADDRCHANGED:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - PPP Resync caused IP Address Change",0,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_WRITE_PPP_CONNECTION_LOST;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

      case DS_EPIPE:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - Peer closed TCP connection",0,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_CONNECTION_CLOSED_BY_PEER;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

      case DS_ENETDOWN:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - Network Subsystem Unavailable",0,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_WRITE_PPP_CONNECTION_LOST;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

      case DS_EFAULT:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - App Buff Not Valid Part Of Addr Space",0,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_WRITE_INTERNAL_ERROR;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

      case DS_EWOULDBLOCK:
      {
        sint31 retval;
        /* There are no buffers available right now or the call has gone dormant.
          Wait until we get a DS_WRITE_EVENT and try again. */
        MSG_MED("PDCOMM APP: Socket Write - No Free Buffers",0,0,0);
        retval = dss_async_select( z_handle_table[z_handle.q_handle_id].x_sockfd, 
                                   DS_WRITE_EVENT | DS_READ_EVENT | DS_CLOSE_EVENT,
                                   &x_err_no);
        if(DSS_ERROR == retval)
        {
          print_dss_err(x_err_no);
          e_write_error = PDCOMM_WRITE_ERROR_WRITE_INTERNAL_ERROR;
          result = FALSE;
        }
        else
        {
          result = TRUE;
        }
        break;
      }
      default:
      {
        MSG_ERROR("PDCOMM APP: Socket Write - unknown error %d for handle_id [%d]",
                  z_handle.q_handle_id,0,0);
        e_write_error = PDCOMM_WRITE_ERROR_WRITE_INTERNAL_ERROR;
        result = FALSE;
        *w_num_bytes_written = 0;
        z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
        break;
      }

    } /*switch( err_no ) ends*/
  } /*else for if( bytes_written_temp >= 0 ) ends*/

  if(result == FALSE)
  {
    pd_comms_notification_params_type z_params;
    z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;

    /*z_params.q_transaction_id = z_handle_table[z_handle.q_handle_id].q_write_transaction_id;*/
    z_params.q_result = (uint32) e_write_error;
    (void)pd_comms_notify_client(z_handle,
                           z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                           z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                           z_handle_table[z_handle.q_handle_id].q_write_transaction_id,
                           ERROR_RPT,&z_params);
  }

  return( result );  
} /*pd_comms_tcp_task_tcp_write() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_secure_tcp_write

DESCRIPTION
  This function handles a write request for  secure TCP connection. 
  It also notifies the client about the result of the write operation

DEPENDENCIES
  None

RETURN VALUE
  boolean - write operation successfull or not 

SIDE EFFECTS
  None
===========================================================================*/
static boolean pd_comms_tcp_task_secure_tcp_write(
  /*handle corresponding to the connection for which data is to be written*/
  pd_comms_handle_type z_handle)
{
  static uint16 l_bytes_written_temp;
  secerrno_enum_type e_error;
  pd_comms_write_result_type e_write_error = PDCOMM_WRITE_ERROR_UNKNOWN;
  boolean result;

  /* Attempt to write length bytes to socket. bytes_written will return how many
     bytes actually got sent. */ 

  l_bytes_written_temp =  z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write;

  e_error = secssl_write( z_handle_table[z_handle.q_handle_id].z_ssl_handle,
                        &z_handle_table[z_handle.q_handle_id].p_write_buff[0],
                        &l_bytes_written_temp );

  if( e_error == E_SUCCESS )
  {
#if 0
    pd_comms_write_ack_params_type z_params;
    
    z_params.q_bytes_written = l_bytes_written_temp;
    z_params.q_transaction_id = z_handle_table[z_handle.q_handle_id].q_write_transaction_id;

    /* Acknowledge the write */
    (void)pd_comms_send_write_ack_to_client(z_handle,
                                      z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                      z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                      &z_params);
    z_handle_table[z_handle.q_handle_id].w_num_bytes_written = 0;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
#endif
    result =  TRUE;
  }
  else
  {
    /* A write error has occurred */

    MSG_ERROR("PDCOMM APP: Socket Write - App Buff Not Valid Part Of Addr Space",0,0,0);
    e_write_error = PDCOMM_WRITE_ERROR_WRITE_INTERNAL_ERROR;
    result = FALSE;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_written  = 0;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;

  } /*else for if( bytes_written_temp >= 0 ) ends*/

  if(result == FALSE)
  {
    pd_comms_notification_params_type z_params;
    
    z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;
    z_params.q_result = (uint32) e_write_error;
    
    (void)pd_comms_notify_client(z_handle,
                           z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                           z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                           z_handle_table[z_handle.q_handle_id].q_write_transaction_id,
                           ERROR_RPT,&z_params);
  }

  return( result );  

} /*pd_comms_tcp_task_secure_tcp_write() ends*/


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

  PDCOMM_APP_MSG_MED("Write request received for handle_id [%d]",z_handle.q_handle_id,0,0);
  
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

  PDCOMM_APP_MSG_MED("Read request received for handle_id [%d]",z_handle.q_handle_id,0,0);
  
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
FUNCTION pd_comms_tcp_task_dss_force_dormancy

DESCRIPTION
  This function call appropriate DSS_IFACE calls to force dormancy for a 
  particular TCP link.

DEPENDENCIES
  None

RETURN VALUE
 boolean - force dormancy function was successfull or not

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_tcp_task_dss_force_dormancy( 
  /*handle id for the link for which force dormancy function has to be called*/
  uint32 q_handle_id )
{
  dss_iface_id_type q_iface_id;
  sint15        q_dss_err_no;
  boolean       flag = FALSE;

  if(q_handle_id < PDCOMM_MAX_HANDLES)
  {
    PDCOMM_APP_MSG_HIGH("Force DS socket into dormancy for handle_id[%d].", q_handle_id, 0, 0);

    /*-------------------------------------------------------------------------
      Get the CDMA iface pointer instance 0 and send the ioctl to go dormant
    -------------------------------------------------------------------------*/
    /* Change made to use no DS API to get interface ID */

    /* Change made to use no DS API to set policy (i.e. use 1x CDMA only) */
    /*dss_init_net_policy_info(&z_policy_info);

    z_policy_info.iface.kind                   = DSS_IFACE_NAME;
    z_policy_info.iface.info.name              = DSS_IFACE_CDMA_SN; 
    z_policy_info.policy_flag                  = DSS_IFACE_POLICY_SPECIFIED;
    z_policy_info.family                       = (int)AF_INET;  */
  
    q_iface_id = pd_comms_tcp_task_dss_get_iface_id(q_handle_id);

    if( q_iface_id == DSS_IFACE_INVALID_ID )
    {
      PDCOMM_APP_MSG_ERROR("could not get iface id",0,0,0);
      return FALSE;
    }

    /* Disable Dormant Timer. */
    if( dss_iface_ioctl( q_iface_id,
                     DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN,
                     &flag,
                     &q_dss_err_no ) == -1 )
    {
      if (q_dss_err_no == DS_EINVAL)   
      {
        /* invalid operation, iface maybe on non-1x, ignore error _707_ ioctl is for 1x interface ONLY */
        PDCOMM_APP_MSG_HIGH("pd_comms_tcp_task_dss_force_dormancy: INVALID err=%d ifaceid=%d",
                          q_dss_err_no, q_iface_id, 0);
        return TRUE;
      }
      else
      {
        PDCOMM_APP_MSG_ERROR("error process dss_iface_ioctl for holddown timer handle_id[%d]",q_handle_id,0,0);
        return FALSE;
      }
    }

    if( q_dss_err_no == DS_EFAULT )
    {
      PDCOMM_APP_MSG_HIGH("IOCTL cannot disable holddown timer for handle_id[%d]",q_handle_id,0,0);
      return FALSE;
    }

    if( dss_iface_ioctl( q_iface_id,
                     DSS_IFACE_IOCTL_GO_DORMANT,
                     NULL,
                     &q_dss_err_no ) == -1 )
    {
      PDCOMM_APP_MSG_ERROR("dss_iface_ioctl for force dormancy failed for handle_id[%d]",q_handle_id,0,0);
      return FALSE;
    }

    if( q_dss_err_no == DS_EFAULT )
    {
      PDCOMM_APP_MSG_HIGH("dss_iface_ioctl for force dormancy failed for handle_id[%d]",q_handle_id,0,0);
      return FALSE;
    }

    PDCOMM_APP_MSG_HIGH("Force Dormancy successfull for handle_id[%d]",q_handle_id,0,0);
    return TRUE;
  }/*if(q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    PDCOMM_APP_MSG_ERROR("pd_comms_tcp_task_dss_force_dormancy: handle id %d out of range",
                          q_handle_id, 0, 0);
    return FALSE;
  }
} /*pd_comms_tcp_task_dss_force_dormancy*/


/*===========================================================================
FUNCTION pdsm_comm_dss_unforce_dormancy

DESCRIPTION
  This function call appropriate DSS_IFACE calls to unforce dormancy for a 
  particular TCP link.

DEPENDENCIES
  None

RETURN VALUE
 boolean - unforce dormancy function was successfull or not

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_tcp_task_dss_unforce_dormancy( 
  /*handle id for the link for which unforce dormancy function has to be called.*/
  uint32 q_handle_id )
{
  dss_iface_id_type q_iface_id;
  sint15        x_dss_err_no;

  if(q_handle_id < PDCOMM_MAX_HANDLES)
  {
    PDCOMM_APP_MSG_HIGH("Unforce dormancy for handle_id[%d]", q_handle_id, 0, 0);

    /*-------------------------------------------------------------------------
      Get the CDMA iface pointer instance 0 and send the ioctl to go dormant
    -------------------------------------------------------------------------*/
    /* Change made to use no DS API to get interface ID */
    q_iface_id = pd_comms_tcp_task_dss_get_iface_id(q_handle_id);

    if( q_iface_id == DSS_IFACE_INVALID_ID )
    {
      PDCOMM_APP_MSG_ERROR("could not get iface id",0,0,0);
      return FALSE;
    }

    if( dss_iface_ioctl( q_iface_id,
                       DSS_IFACE_IOCTL_GO_ACTIVE,
                       NULL,
                       &x_dss_err_no ) == -1 )
    {
      PDCOMM_APP_MSG_ERROR("error process dss_iface_ioctl to unforce dormancy for handleid[%d]",
                           q_handle_id,0,0);
      return FALSE;
    }

    if( x_dss_err_no == DS_EFAULT )
    {
      PDCOMM_APP_MSG_HIGH("IOCTL cannot unforce Dormancy for handle_id[%d]",q_handle_id,0,0);
      return FALSE;
    }

    PDCOMM_APP_MSG_HIGH("unforce dormancy successfull for handle_id[%d]",q_handle_id,0,0);
    return TRUE;
  }/*if(q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    MSG_ERROR("pd_comms_tcp_task_dss_unforce_dormancy: handle_id %d out of range", q_handle_id, 0, 0);
    return FALSE;
  }
}/*pd_comms_tcp_task_dss_unforce_dormancy*/


/*===========================================================================

FUNCTION pd_comms_phy_link_down_event_callback

DESCRIPTION
  This function is used as a callback for the ioctl function event
  registration command

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.

===========================================================================*/
static void pd_comms_phy_link_down_event_callback (
               dss_iface_ioctl_event_enum_type          event,
               dss_iface_ioctl_event_info_union_type    event_info,
               void                                     *user_data,
               sint15                                   app_id,
               dss_iface_id_type                        name
               )
{
  uint32 q_handle_id;
  pd_comms_tcp_task_phy_link_down_cb_ipc_hdr_type* p_hdr_ptr; 
  os_IpcMsgType* p_ipc_ptr;  

  if(user_data == NULL)
  {
    PDCOMM_APP_MSG_ERROR("Null user_data received in phy_link_down callback",0,0,0);
    return;
  }

  q_handle_id = *((uint32 *)user_data);

  if( (q_handle_id < PDCOMM_MAX_HANDLES)
      && (event == DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV))
  {
    
    PDCOMM_APP_MSG_MED( "[phy_link_down] event for handle_id [%d]", 
                         q_handle_id,0,0);

    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_phy_link_down_cb_ipc_hdr_type),
                           IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);             
    if(p_ipc_ptr == NULL)
    {
      PDCOMM_APP_MSG_HIGH("IPC Create failed during physical link down callback. Handle Id: %d",
                          q_handle_id,0,0);
    }
    else
    {  /*lint -e826 */    
      p_hdr_ptr = (pd_comms_tcp_task_phy_link_down_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
      /*lint +e826 */   
      p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_DSS_PHY_LINK_DOWN_CB;
      p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

      p_hdr_ptr->handle.q_handle_id = q_handle_id;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
        PDCOMM_APP_MSG_HIGH("IPC Send failed during physical link down event callback. Handle Id: %d",
                            q_handle_id,0,0);
        (void)os_IpcDelete(p_ipc_ptr);
      }
    }

  }/*if( (q_handle_id < PDCOMM_MAX_HANDLES) && (event == DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV))*/
  else
  {
    PDCOMM_APP_MSG_ERROR( "Got unexpected phy_link_down event", 0, 0, 0);       
  }  
    
  return;
} /* pdsm_comm_phy_link_down_event_cb */ /*lint !e715 !e818 */


/*===========================================================================

 FUNCTION pd_comms_tcp_task_dss_reg_phy_link_down_event

 DESCRIPTION

 DEPENDENCIES
   none

 RETURN VALUE

 SIDE EFFECTS
   None

===========================================================================*/
boolean pd_comms_tcp_task_dss_reg_phy_link_down_event( uint32 q_handle_id )
{
  dss_iface_id_type q_iface_id;
  dss_iface_ioctl_ev_cb_type z_phy_link_down_ev_cb;
  sint15        x_dss_err_no;

  if(q_handle_id < PDCOMM_MAX_HANDLES)
  {
    /*-------------------------------------------------------------------------
      Register for phy link down event
    -------------------------------------------------------------------------*/
    PDCOMM_APP_MSG_HIGH("Wait for physical link down event", 0, 0, 0);

    q_iface_id = pd_comms_tcp_task_dss_get_iface_id(q_handle_id);

    if( q_iface_id == DSS_IFACE_INVALID_ID )
    {
      PDCOMM_APP_MSG_ERROR("could not get iface id for handle_id[%d]",q_handle_id,0,0);
      return FALSE;
    }

    /* In case of WM build, ARM11 is controlling the PPP open 
    ** PDCOMM_TCP needs to register with DS layer to get app_id
    */
    if (DSS_ERROR == z_handle_table[q_handle_id].x_app_id)
    {
      z_handle_table[q_handle_id].x_force_dorm_app_id = pd_comms_tcp_task_get_app_id (q_handle_id); 
      if( DSS_ERROR ==  z_handle_table[q_handle_id].x_force_dorm_app_id)
      {
        MSG_ERROR("Could not get App id",0,0,0);
        return FALSE;
      }
      z_phy_link_down_ev_cb.app_id = z_handle_table[q_handle_id].x_force_dorm_app_id;
    }
    else
    {
      z_phy_link_down_ev_cb.app_id = z_handle_table[q_handle_id].x_app_id;
    }
    
    /* Set to PDSM_COMM_DORMANCY_OPERATION_WAIT_PHY_LINK_DOWN before register callback,
       because if phy_link_link is already down, we will get a event callback right when we do registration
    */
    z_handle_table[q_handle_id].e_dormancy_state = PD_COMMS_DORMANCY_OPERATION_WAIT_PHY_LINK_DOWN;

    z_phy_link_down_ev_cb.event =  DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV;
    z_phy_link_down_ev_cb.event_cb = pd_comms_phy_link_down_event_callback;
    z_phy_link_down_ev_cb.user_data_ptr = (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id);

    if( dss_iface_ioctl( q_iface_id,
                     DSS_IFACE_IOCTL_REG_EVENT_CB,
                     &z_phy_link_down_ev_cb,
                     &x_dss_err_no ) == -1 )
    {
      PDCOMM_APP_MSG_ERROR("error process dss_iface_ioctl for handle_id[%d]",q_handle_id,0,0);
      z_handle_table[q_handle_id].e_dormancy_state = PD_COMMS_DORMANCY_OPERATION_INIT;
      return FALSE;
    }

    if( x_dss_err_no == DS_EFAULT )
    {
      PDCOMM_APP_MSG_HIGH("IOCTL cannot register event cbfor handle_id[%d]",q_handle_id,0,0);
      z_handle_table[q_handle_id].e_dormancy_state = PD_COMMS_DORMANCY_OPERATION_INIT;
      return FALSE;
    }

    return TRUE;
  }/*if(q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    PDCOMM_APP_MSG_ERROR("pd_comms_dss_reg_phy_link_down_event: handle_id %d out of range", q_handle_id, 0, 0);
    return FALSE;
  }
}/*pd_comms_tcp_task_dss_reg_phy_link_down_event*/


/*===========================================================================

 FUNCTION pd_comms_dss_dereg_phy_link_down_event

 DESCRIPTION

 DEPENDENCIES
   none

 RETURN VALUE

 SIDE EFFECTS
   None

===========================================================================*/
static boolean pd_comms_dss_dereg_phy_link_down_event( uint32 q_handle_id )
{
  dss_iface_id_type q_iface_id;
  sint15        x_dss_err_no;
  dss_iface_ioctl_ev_cb_type z_phy_link_down_ev_cb;
  boolean u_retval = TRUE;

  if(q_handle_id < PDCOMM_MAX_HANDLES)
  {
    PDCOMM_APP_MSG_HIGH("Deregister physical link down event for handle_id[%d]", q_handle_id, 0, 0);

    q_iface_id = pd_comms_tcp_task_dss_get_iface_id(q_handle_id);

    if( q_iface_id == DSS_IFACE_INVALID_ID )
    {
      PDCOMM_APP_MSG_ERROR("Could not get iface id",0,0,0);
      return FALSE;
    }

    if (z_handle_table[q_handle_id].x_force_dorm_app_id != DSS_ERROR)
    {
      z_phy_link_down_ev_cb.app_id = z_handle_table[q_handle_id].x_force_dorm_app_id;
    }
    else
    {
      z_phy_link_down_ev_cb.app_id = z_handle_table[q_handle_id].x_app_id;  
    }
    
    z_phy_link_down_ev_cb.event =  DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV;

    if( dss_iface_ioctl( q_iface_id,
                       DSS_IFACE_IOCTL_DEREG_EVENT_CB,
                       &z_phy_link_down_ev_cb,
                       &x_dss_err_no ) == -1 )
    {
      PDCOMM_APP_MSG_ERROR("ERROR deregistering physical link down callback for handle_id[%d]",
                           q_handle_id,0,0);
      u_retval =  FALSE;
    }

    if( x_dss_err_no == DS_EFAULT )
    {
      PDCOMM_APP_MSG_HIGH("IOCTL cannot deregister event cb for handle [%d]",q_handle_id,0,0);
      u_retval = FALSE;
    }

  /* Release the app_id if allocated when PPP proxy entity is active */
    if (z_handle_table[q_handle_id].x_force_dorm_app_id != DSS_ERROR)
    {
       /* Release the Data Services app_id */
       (void)pd_comms_tcp_task_release_app_id(z_handle_table[q_handle_id].x_force_dorm_app_id);
       z_handle_table[q_handle_id].x_force_dorm_app_id = DSS_ERROR;
    }

    return u_retval;
  }/*if(q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    PDCOMM_APP_MSG_ERROR("pd_comms_dss_dereg_phy_link_down_event: handle_id %d out of range",
                          q_handle_id, 0, 0);
    return FALSE;
  }
}/*pd_comms_dss_dereg_phy_link_down_event*/


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
static void pd_comms_tcp_task_handle_ioctl_force_dormancy(
  /*ioctl command header*/
  pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)
{
  pd_comms_handle_type z_handle;
  pd_comms_notification_params_type z_params;
  
  if(p_hdr_ptr == NULL)
  {
    return;
  }

  z_handle = p_hdr_ptr->z_handle;
  
  if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)
  {
    /* If force dormancy operation or reg_phy_link_event fails, inform TM NOW
       Others inform TM when physical link is down
    */
    if((pd_comms_tcp_task_dss_force_dormancy(z_handle.q_handle_id) == FALSE) ||
       (pd_comms_tcp_task_dss_reg_phy_link_down_event(z_handle.q_handle_id) == FALSE))
    {

      z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;
      z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_FORCE_DORMANCY_FAILED;
      PDCOMM_APP_MSG_HIGH(" FORCE_DORMANCY ioctl failed for handle_id [%d]",
                            z_handle.q_handle_id,0,0);
      (void)pd_comms_notify_client(z_handle,
                             p_hdr_ptr->q_source_thread_id,
                             p_hdr_ptr->q_base_msg_id,
                             p_hdr_ptr->q_transaction_id,
                             ERROR_RPT,&z_params);

    }
  }/*if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {

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
static void pd_comms_tcp_task_handle_ioctl_unforce_dormancy(
  /*ioctl command header*/
  pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)
{
  pd_comms_handle_type z_handle;
  pd_comms_ioctl_ack_params_type z_ioctl_ack;
  pd_comms_notification_params_type z_params;

  if(p_hdr_ptr == NULL)
  {
    return;
  }

  z_handle = p_hdr_ptr->z_handle;
  
  if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)
  {
    if(pd_comms_tcp_task_dss_unforce_dormancy(z_handle.q_handle_id) == FALSE)
    {
      /* Unforce dormancy failed, inform PDSM */
      z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;
      z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_UNFORCE_DORMANCY_FAILED;
      PDCOMM_APP_MSG_HIGH(" UNFORCE_DORMANCY ioctl failed for handle_id [%d]",
                            z_handle.q_handle_id,0,0);
      (void)pd_comms_notify_client(z_handle,
                             p_hdr_ptr->q_source_thread_id,
                             p_hdr_ptr->q_base_msg_id,
                             p_hdr_ptr->q_transaction_id,
                             ERROR_RPT,&z_params);  
    }
    else
    {
      /* Unforce dormancy succeeded, inform PDSM */
              
      z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_UNFORCE_DORMANCY;
      z_ioctl_ack.q_transaction_id = p_hdr_ptr->q_transaction_id;
      z_ioctl_ack.q_argument_len = 0;
      (void)pd_comms_send_ioctl_ack_to_client(z_handle,
                                              z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                              z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                              &z_ioctl_ack);  
    }
  }/*if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
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
FUNCTION pd_comms_tcp_task_handle_ioctl_set_ssl_cert

DESCRIPTION
  This function handles IOCTL command to set SSL certificate for a TCP/SSL connection

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_ioctl_set_ssl_cert(
  /*ioctl command header*/
  const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)  
{
  pd_comms_handle_type z_handle;
  pd_comms_ioctl_ack_params_type z_ioctl_ack;
    
  if(p_hdr_ptr == NULL)
  {
    return;
  }
  
  z_handle = p_hdr_ptr->z_handle;  

  if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)
  {
    if(p_hdr_ptr->q_argument_len > PDCOMM_MAX_SSL_CERT_SIZE)
    {
      PDCOMM_APP_MSG_ERROR("SSL certificate size[%d] greater than PDCOMM SSL buffer[%d]",
                            p_hdr_ptr->q_argument_len, PDCOMM_MAX_SSL_CERT_SIZE,0); 
    }

	z_handle_table[z_handle.q_handle_id].w_ssl_cert_len = 
		memscpy(z_handle_table[z_handle.q_handle_id].p_ssl_cert, PDCOMM_MAX_SSL_CERT_SIZE,
		       (void*)p_hdr_ptr->p_data, p_hdr_ptr->q_argument_len); /*lint !e420*/

    z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_SET_SSL_CERT;
    z_ioctl_ack.q_transaction_id = p_hdr_ptr->q_transaction_id;
    z_ioctl_ack.q_argument_len = 0;
    (void)pd_comms_send_ioctl_ack_to_client(z_handle,p_hdr_ptr->q_source_thread_id,
                                            p_hdr_ptr->q_base_msg_id,&z_ioctl_ack);
  }/*if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    pd_comms_notification_params_type z_params;   
        
    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;
    
    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in SET_SSL_CERT ioctl ",
                          z_handle.q_handle_id,0,0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->q_transaction_id,
                           ERROR_RPT,&z_params);

  }      
  
} /*pd_comms_tcp_task_handle_ioctl_set_ssl_cert*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_ioctl_set_ssl_cert_dir

DESCRIPTION
  This function handles IOCTL command to set SSL certificate directory for 
  a TCP/SSL connection

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_ioctl_set_ssl_cert_dir(
  /*ioctl command header*/
  const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)  
{
  pd_comms_handle_type              z_handle;
  pd_comms_ioctl_ack_params_type    z_ioctl_ack;
  pd_comms_notification_params_type z_params;   
  uint8                             error_code = 0;
    
  if(p_hdr_ptr == NULL)
  {
    return;
  }
  
  do
  {
    z_handle = p_hdr_ptr->z_handle;  

    if(z_handle.q_handle_id >= PDCOMM_MAX_HANDLES)
    {
      error_code = 1;
      break;
    }

    if(p_hdr_ptr->q_argument_len > PDCOMM_MAX_SSL_CERT_DIR_PATH_SIZE)
    {
      error_code = 2;
      break;
    }
    else
    {
      (void) GNSS_STRLCPY(z_handle_table[z_handle.q_handle_id].p_ssl_cert_dir,
                          (void*)p_hdr_ptr->p_data,
                          PDCOMM_MAX_SSL_CERT_DIR_PATH_SIZE); /*lint !e420*/
    }

    z_ioctl_ack.q_ioctl_request  = PDCOMM_IOCTL_CMD_SET_SSL_CERT_DIR;
    z_ioctl_ack.q_transaction_id = p_hdr_ptr->q_transaction_id;
    z_ioctl_ack.q_argument_len   = 0;
    (void)pd_comms_send_ioctl_ack_to_client(z_handle,p_hdr_ptr->q_source_thread_id,
                                              p_hdr_ptr->q_base_msg_id,&z_ioctl_ack);
  }while (0);

  if(error_code != 0)
  {
    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;
    
    PDCOMM_APP_MSG_HIGH(" Error code %d in SET_SSL_CERT_DIR ioctl ", error_code, 0, 0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->q_transaction_id,
                           ERROR_RPT,&z_params);

  }      
  
} /*pd_comms_tcp_task_handle_ioctl_set_ssl_cert_dir*/


/*===========================================================================
FUNCTION pd_comms_tcp_get_iface_ip_addr

DESCRIPTION
  This function retrieves current IP address from DSS

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/

boolean pd_comms_tcp_get_iface_ip_addr
(
  ip_addr_type      *p_ipaddr,
  uint32 q_handle_id
)
{
  dss_iface_id_type iface_id;
  sint15            x_err_no;
  dss_net_policy_info_type  z_policy_info;

  if(p_ipaddr == NULL)
  {
    return FALSE;
  }

  /*Get the default policy*/
  pd_comms_tcp_task_dss_get_policy(&z_policy_info, q_handle_id);
  
  iface_id = dss_get_iface_id_by_policy( z_policy_info,
                                         &x_err_no );

  if( iface_id == DSS_IFACE_INVALID_ID )
  {
    PDCOMM_APP_MSG_ERROR("Unable to get iface id.Error[%d]",x_err_no,0,0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
     Get the iface IP address
  -------------------------------------------------------------------------*/
  if( dss_iface_ioctl( iface_id,
                       DSS_IFACE_IOCTL_GET_IPV4_ADDR,
                       p_ipaddr,
                       &x_err_no ) == -1 )
  {
    PDCOMM_APP_MSG_ERROR("error processing dss_iface_ioctl for getting IP address.Error[%d]",x_err_no,0,0);
    return FALSE;
  }

  if( p_ipaddr->type ==  IPV4_ADDR  &&
      p_ipaddr->addr.v4 != 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* pd_comms_tcp_get_iface_ip_addr */

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
static void pd_comms_tcp_task_handle_ioctl_get_ip_addr(
  /*ioctl command header*/
  const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)  
{
  pd_comms_handle_type z_handle;
  pd_comms_ioctl_ack_params_type z_ioctl_ack;
  pd_comms_notification_params_type z_params;   
  ip_addr_type      z_ipaddr;
    
  if(p_hdr_ptr == NULL)
  {
    return;
  }
  
  z_handle = p_hdr_ptr->z_handle;  

  if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)
  {
    if(pd_comms_tcp_get_iface_ip_addr(&z_ipaddr,z_handle.q_handle_id ))
    {
      z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_GET_IP_ADDR;
      z_ioctl_ack.q_transaction_id = p_hdr_ptr->q_transaction_id;
      z_ioctl_ack.q_argument_len = sizeof(z_ipaddr.addr.v4);
      z_ioctl_ack.p_argument = (uint8*)&z_ipaddr.addr.v4;
      (void)pd_comms_send_ioctl_ack_to_client(z_handle,p_hdr_ptr->q_source_thread_id,
                                              p_hdr_ptr->q_base_msg_id,&z_ioctl_ack);
      return;
    }
    else
    {
      z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_GET_IP_ADDR_FAILED;
      z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;
      PDCOMM_APP_MSG_HIGH(" dss ioctl call failed in GET_IP_ADDR ioctl for handle[%d]",
                            z_handle.q_handle_id,0,0);
    }
    
  }/*if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;
    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in GET_IP_ADDR ioctl ",
                          z_handle.q_handle_id,0,0);
  }

  /*Send the appropriate error to client*/ 
  (void)pd_comms_notify_client(z_handle,
                         p_hdr_ptr->q_source_thread_id,
                         p_hdr_ptr->q_base_msg_id,
                         p_hdr_ptr->q_transaction_id,
                         ERROR_RPT,&z_params);

  
} /*pd_comms_tcp_task_handle_ioctl_get_ip_addr*/


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
FUNCTION pd_comms_tcp_task_stop_data_call

DESCRIPTION
  This function will stop the data call with app_id specified.

============================================================================*/

static pd_comms_tcp_task_operation_status_type
pd_comms_tcp_task_stop_data_call(sint15 x_app_id)
{
  sint15    x_errno = 0;
  
  /*Close the PPP connection*/  
  PDCOMM_APP_MSG_MED("Closing PPP for for app_id[%d] as part of fallback",x_app_id,0,0);
  if(dsnet_stop(x_app_id, &x_errno) == DSS_ERROR)
  {  
    if(DS_EWOULDBLOCK != x_errno)
    {      
      print_dss_err(x_errno);
      return FAILED;
    }
  }
  return SUCCEEDED;
}

/*===========================================================================
FUNCTION pd_comms_tcp_task_start_data_call

DESCRIPTION
  This function will start the data call with app_id specified.

============================================================================*/
static pd_comms_tcp_task_operation_status_type
pd_comms_tcp_task_start_data_call(sint15 x_app_id)
{
  sint15    x_errno = 0;

  PDCOMM_APP_MSG_MED("Opening PPP for for app_id[%d] as part of fallback",x_app_id,0,0);

  /*Open the PPP connection*/
  if(dsnet_start(x_app_id, &x_errno) == DSS_ERROR)
  {  
    if(DS_EWOULDBLOCK != x_errno)
    {      
      print_dss_err(x_errno);
      return FAILED;
    }
  }
 return SUCCEEDED;
}

/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_fallback

DESCRIPTION
  This function handles data family type fallback

============================================================================*/
static pd_comms_tcp_task_operation_status_type
pd_comms_tcp_task_handle_fallback(uint32 q_handle_id, uint16 family)
{
  dss_net_policy_info_type  z_policy_info;
  sint15 x_err_no;

  /*Change the data connections ip family*/
  
  z_handle_table[q_handle_id].datacall_family = family;
  z_handle_table[q_handle_id].z_datacall_family_fallback_state = FALLBACK_STATE_WAIT_OPEN;
  
   /*Change the policy ip family*/             
   (void)pd_comms_tcp_task_dss_get_policy(&z_policy_info, q_handle_id);
   z_policy_info.family = z_handle_table[q_handle_id].datacall_family;
   /*Cache the old app id*/
   z_handle_table[q_handle_id].x_prev_app_id = 
      z_handle_table[q_handle_id].x_app_id;
  /*Get the new app id*/
   z_handle_table[q_handle_id].x_app_id
      = dsnet_get_handle( pd_comms_tcp_task_net_callback,
                  (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id),
                  pd_comms_tcp_task_socket_callback,
                  (void *)(&z_handle_table[q_handle_id].z_handle.q_handle_id),
                  &z_policy_info,
                  &x_err_no );
  if(z_handle_table[q_handle_id].x_app_id != DSS_ERROR)
  {
    /*Start the new call*/
    if(pd_comms_tcp_task_start_data_call(z_handle_table[q_handle_id].x_app_id ) == SUCCEEDED)
    {
      return SUCCEEDED;
    }
    else
    {
      sint15 ds_err_no;
      dss_close_netlib( z_handle_table[q_handle_id].x_app_id, &ds_err_no );
      z_handle_table[q_handle_id].x_app_id = z_handle_table[q_handle_id].x_prev_app_id;
      z_handle_table[q_handle_id].x_prev_app_id = DSS_ERROR;

      
      if(pd_comms_tcp_task_stop_data_call(z_handle_table[q_handle_id].x_app_id ) != SUCCEEDED)
      {
        dss_close_netlib( z_handle_table[q_handle_id].x_app_id, &ds_err_no );
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                       TRUE,(uint32) PDCOMM_CONNECT_ERROR_PPP_OPEN_FAILURE);
      }
      return FAILED;
      
    }
    
  }
  else
  {
    sint15 ds_err_no;
    z_handle_table[q_handle_id].x_app_id = z_handle_table[q_handle_id].x_prev_app_id;
    z_handle_table[q_handle_id].x_prev_app_id = DSS_ERROR;
    
    if(pd_comms_tcp_task_stop_data_call(z_handle_table[q_handle_id].x_app_id ) != SUCCEEDED)
    {
      dss_close_netlib( z_handle_table[q_handle_id].x_app_id, &ds_err_no );
      pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32) PDCOMM_CONNECT_ERROR_PPP_OPEN_FAILURE);
    }    
    return FAILED;
  }

}

/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_net_cb

DESCRIPTION
  Whenever there is a NETWORK callback, an IPC message is sent to PDCOMM APP.
  This IPC message is handled in this function. It reads the network status
  by calling dss_netstatus() and takes the appropriate action based upon the
  recevied netstatus. 

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_net_cb(
  /*Pointer to IPC message containing the network callback information*/
  const os_IpcMsgType* p_msg )
{
  sint15 x_err_no;            /* error value as returned from dss functions */
  uint32 q_handle_id;
  sint15 x_appid;
  sint31 x_retval;
  boolean u_close_ppp = TRUE;
  secerrno_enum_type e_error;
  
  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */    
  q_handle_id = ((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  x_appid = ((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->x_appid;
  /*lint +e826 */

  PDCOMM_APP_MSG_MED("Net callback received for handle_id[%d]",q_handle_id,0,0);
  
  if( q_handle_id < PDCOMM_MAX_HANDLES )
  { 
    pd_comms_tcp_task_reset_inactivity_count(q_handle_id);

    /*if(z_handle_table[q_handle_id].x_app_id == DSS_ERROR)
    {
      return;
    } */
    
    /*(void)dss_netstatus(z_handle_table[q_handle_id].x_app_id, &x_err_no);*/
    x_err_no = ((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->event_mask;
    
    PDCOMM_APP_MSG_MED( "Network status [%d] for handle_id [%d], while link_state is [%d]", 
                        x_err_no, q_handle_id,z_handle_table[q_handle_id].e_link_status);
      
    print_dss_err(x_err_no);
    if(z_handle_table[q_handle_id].u_use_laptop_call == TRUE)
    {
      u_close_ppp = FALSE;
    }

    switch(x_err_no)
    {
      case DS_EBADAPP:
      {
        pd_comms_connect_result_type e_connect_err = 
                                     get_pdcomm_connect_error_from_dss_error(x_err_no);


        MSG_ERROR("PDCOMM APP: Invalid App Id: %d",z_handle_table[q_handle_id].x_app_id,0,0);

        /* Close Network Library */
        /*
        if( dss_close_netlib( z_handle_table[q_handle_id].x_app_id, &x_err_no ) == DSS_ERROR )
        {
          print_dss_err(x_err_no);
          MSG_ERROR("PDCOMM APP: Close Netlib: Error %d for handle_id [%d]", x_err_no, q_handle_id, 0);
        }
        else
        {
          PDCOMM_APP_MSG_MED("Netlib Closed for handle_id [%d]",q_handle_id,0,0);
        }
        */
        z_handle_table[q_handle_id].x_app_id = DSS_ERROR;
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32) e_connect_err);

        break;
      }

      case DS_ENETNONET:
      {
        pd_comms_connect_result_type e_connect_err = 
                                   get_pdcomm_connect_error_from_dss_error(x_err_no);


        PDCOMM_APP_MSG_MED("No Network for handle_id [%d]",q_handle_id,0,0);

        if(z_handle_table[q_handle_id].z_datacall_family_fallback_state == FALLBACK_STATE_WAIT_CLOSE &&
           x_appid == z_handle_table[q_handle_id].x_prev_app_id
          )
        {          
          dss_close_netlib( z_handle_table[q_handle_id].x_prev_app_id, &x_err_no );
          z_handle_table[q_handle_id].x_prev_app_id = DSS_ERROR;
          z_handle_table[q_handle_id].z_datacall_family_fallback_state = FALLBACK_STATE_NONE;
          return;
        }
        else if(z_handle_table[q_handle_id].z_datacall_family_fallback_state == FALLBACK_STATE_WAIT_OPEN)
        {
          /*The data call we were trying to bringup as part of fallback, did not come up*/
          dss_close_netlib( z_handle_table[q_handle_id].x_app_id, &x_err_no );
          z_handle_table[q_handle_id].x_app_id = z_handle_table[q_handle_id].x_prev_app_id ;
          z_handle_table[q_handle_id].x_prev_app_id = DSS_ERROR;
          z_handle_table[q_handle_id].z_datacall_family_fallback_state = FALLBACK_STATE_NONE;
          if(pd_comms_tcp_task_stop_data_call(z_handle_table[q_handle_id].x_app_id)==SUCCEEDED)
          {
            return;
          }
        }

        /*Clean up resources*/
        /*Cleanup any DNS queries/sessions*/
        if( (z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle != 
                                                                 DSS_DNS_SESSION_MGR_INVALID_HANDLE)
                                          &&
          (z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle != DSS_DNS_QUERY_INVALID_HANDLE) )
        {
        
          x_retval = dss_dns_delete_session(z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle, 
                          &x_err_no);
          if( DSS_ERROR == x_retval )
          {
            MSG_ERROR("DNS session %d could not be deleted", 
              z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle,0,0);
          }
          /*Cleanup DNS related variables*/
          z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle = 
                                                                 DSS_DNS_SESSION_MGR_INVALID_HANDLE;
          z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
        } 

        /*Clean up SSL*/
        if((z_handle_table[q_handle_id].z_connect_params.e_use_security !=
             SECURITY_NONE) &&
           (z_handle_table[q_handle_id].z_ssl_handle != NULL)
          )
        {
          e_error = secssl_abort_open_session(z_handle_table[q_handle_id].z_ssl_handle);
          PDCOMM_APP_MSG_ERROR("secssl_abort_open_session: [%d]",e_error,0,0);

          if (E_SUCCESS != e_error) 
          {
             e_error = secssl_close_session( z_handle_table[q_handle_id].z_ssl_handle,
                                             pd_comms_tcp_task_ssl_close_callback,
                                             &z_handle_table[q_handle_id].z_handle.q_handle_id);   

             if( e_error != E_SUCCESS )
             {
               (void)secssl_delete_session(z_handle_table[q_handle_id].z_ssl_handle);
               z_handle_table[q_handle_id].z_ssl_handle = NULL;
             }
          }
        }
        
        /* Clean up socket and filters if open */
        if( z_handle_table[q_handle_id].x_sockfd != DSS_ERROR )
        {
          
          pd_comms_tcp_task_setsockopt(q_handle_id);
          
          
          /*Delete the IP filter installed previously*/
          if(!pd_comms_app_task_dss_del_filter(q_handle_id))
          {
            PDCOMM_APP_MSG_ERROR("Error deleting IP filter for handle_id[%d]",q_handle_id,0,0);
          }
          
          PDCOMM_APP_MSG_MED( "Closing socket for handle_id [%d]", q_handle_id, 0, 0);
          x_retval = dss_close(z_handle_table[q_handle_id].x_sockfd, &x_err_no);
          
          if(DSS_ERROR == x_retval)
          {
            print_dss_err(x_err_no);
          }
          
          z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;
        }

        /* Close Network Library */
        if(z_handle_table[q_handle_id].x_app_id != DSS_ERROR)
        {
          if( dss_close_netlib( z_handle_table[q_handle_id].x_app_id, &x_err_no ) == DSS_ERROR )
          {
            print_dss_err(x_err_no);
            MSG_ERROR("PDCOMM APP: Close Netlib: Error %d for handle_id [%d]", x_err_no, q_handle_id, 0);
          }
          else
          {
            PDCOMM_APP_MSG_MED("Netlib Closed for handle_id [%d]",q_handle_id,0,0);
          }
        }

        if(z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_DISCONNECTING)
        {
          /*If we are disconnecting, we have already informed TM about any errors*/ 
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                         TRUE,(uint32) PDCOMM_RESULT_OK);
        }
        else
        {
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                         TRUE,(uint32) e_connect_err);    
        }
        break;
      }

      case DS_ENETISCONN:
      {
        pd_comms_tcp_task_operation_status_type e_status = FAILED;
        pd_comms_connect_result_type e_conn_error;

        /* PPP Connection is Successful. At this point, try to open up a socket */
        PDCOMM_APP_MSG_MED("Network Established for handle_id [%d]",q_handle_id,0,0);

        if(z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_DISCONNECTING)
        {
          PDCOMM_APP_MSG_MED("Link already DISCONNECTING. Not proceeding with connect for handle_id [%d]",
            q_handle_id,0,0);
          return;
        }          
        else if(z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_DISCONNECTED)
        {
          PDCOMM_APP_MSG_MED("Link already DISCONNECTED. Not proceeding with connect for handle_id [%d]",
            q_handle_id,0,0);
          return;
        }

        /* if open was performed through ATL, then store param information gotten 
               back at the completion of the open operation */
        if (pd_comms_atl_i_ppp_proxy_exists()) 
        {
          /*
            If the apn name is NULL, then prompt the user.
            In a 'real' network, this would never be the case.
           */
          memscpy(&z_atl_open_params, sizeof(z_atl_open_params), &((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->z_open_resp, sizeof(z_atl_open_params));
          
          if (((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->z_open_resp.apn_name[0] == 0)
          {
            PDCOMM_APP_MSG_MED("Warning: apn name is NULL",0, 0, 0);          
          }
          else
          {
            if(z_atl_open_params.pdp_type == PDSM_ATL_PDP_IPV6)
            {
              z_atl_open_params.iface_family = PDSM_ATL_IP_V6;
            }
            else
            {
              z_atl_open_params.iface_family = PDSM_ATL_IP_V4;
            }
          }
    
        }
        else
        {
          if(z_handle_table[q_handle_id].z_datacall_family_fallback_state == FALLBACK_STATE_WAIT_OPEN)
          {
          
            /*Close the previous data call*/
            pd_comms_tcp_task_stop_data_call(z_handle_table[q_handle_id].x_prev_app_id);
            z_handle_table[q_handle_id].z_datacall_family_fallback_state = FALLBACK_STATE_WAIT_CLOSE;
            /*Proceed with the TCP connection*/
            pd_comm_tcp_task_process_dns_lookup_results(q_handle_id, SUCCEEDED, PDCOMM_CONNECT_RESULT_TYPE_BEGIN);
            return;
          }
        }

        /* Notify client that the socket connection has started */
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_CONNECTING,
                                     TRUE,(uint32)PDCOMM_RESULT_WAIT);

        e_status = pd_comms_tcp_task_open_socket(q_handle_id, &e_conn_error );

        if( e_status == SUCCEEDED )
        {
          /* Enable read data and close event */
          x_retval  = dss_async_select( z_handle_table[q_handle_id].x_sockfd,
                                          DS_READ_EVENT | DS_CLOSE_EVENT,
                                          &x_err_no );

          if(DSS_ERROR == x_retval)
          {
            print_dss_err(x_err_no);
          }
            
          /*If this is a NON-SSL link, notify client that the link is connected */
          if(z_handle_table[q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_CONNECTED,
                                           TRUE,(uint32)PDCOMM_RESULT_OK);
          }
          else
          {
            /*Continue with opening SSL on this socket connection*/  
            pd_comms_tcp_task_open_ssl(z_handle_table[q_handle_id].z_handle);
          }

        }
        else if( e_status == FAILED )
        {
          pd_comms_disconnect_result_type e_disc_error;

          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTING,
                                          TRUE,(uint32)e_conn_error);

          /* Teardown socket connection */
          /* this used to be close ppp*/
          e_status = pd_comms_tcp_task_close_socket(q_handle_id,&e_disc_error,u_close_ppp);

          if(e_status == SUCCEEDED)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                         TRUE,(uint32) PDCOMM_RESULT_OK);
          }
          else if(e_status == PENDING)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32) PDCOMM_RESULT_WAIT);
          }
          else if(e_status == FAILED)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                         TRUE,(uint32) e_disc_error);
          }

        }/*else if( e_status == FAILED )*/

        break;
      }/*case DS_ENETISCONN*/

      case DS_ENETCLOSEINPROGRESS: /* DS_ENETCLOSEINPROGRESS should not happen for 6800 and 7500 onwards */
      {
        MSG_ERROR( "PDCOMM APP: DS_ENETCLOSEINPROGRESS ", 0, 0, 0);            
        break;
      }

      case DS_ENETINPROGRESS:
      {
        PDCOMM_APP_MSG_MED("Network Establishment In Progress for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }

      default:
      {
        MSG_ERROR("PDCOMM APP:Unknown Network Error [%d] for handle_id [%d]",
                  x_err_no,q_handle_id,0);
        break;
      }
    }/* switch(err_no) ends */

  }/*if( q_handle_id < PDCOMM_MAX_HANDLES )*/ 
  else
  {
    MSG_ERROR("CHECK NETWORK: handle_id %d out of range", q_handle_id, 0, 0);
  }
}/*pd_comms_tcp_task_handle_net_cb*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sock_cb

DESCRIPTION
  Whenever there is a SOCKET callback, an IPC message is sent to PDCOMM APP.
  This IPC message is handled in this function. Based upon the recevied 
  event_mask it takes the appropriate action 

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_sock_cb(
  /*Pointer to IPC message containing the socket callback information*/
  const os_IpcMsgType* p_msg )
{
  pd_comms_tcp_task_operation_status_type e_status = FAILED;
  uint32 l_result,l_result_tmp;
  sint15 x_err_no  = DS_ENOTCONN;
  sint15 x_retval  = DSS_ERROR;
  sint15 x_appid;
  sint15 sockfd;
  
  pd_comms_disconnect_result_type e_disc_error;
  /*lint -e826 */    
  uint32 q_handle_id;
  boolean result = TRUE;
  boolean u_forward_event = FALSE;
  boolean u_close_ppp = TRUE;
  
  if(p_msg == NULL)
  {
    return;
  }
  
  q_handle_id = ((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  x_appid = ((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->x_appid;
  sockfd =  ((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->x_sockfd;
  PDCOMM_APP_MSG_MED("Received SOCKET callback for handle_id [%d] with event: %d for app_id[%d]",
                      q_handle_id,((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->event_mask,x_appid);
  if(z_handle_table[q_handle_id].u_use_laptop_call == TRUE)
  {
    u_close_ppp = FALSE;
  }

  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    /*result = dss_getnextevent( z_handle_table[handle_id].app_id, 
                               &z_handle_table[handle_id].sockfd, &err_no );*/
    pd_comms_tcp_task_reset_inactivity_count(q_handle_id);

    l_result = ((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->event_mask;
    /*lint +e826 */    

    if( l_result & DS_CLOSE_EVENT )
    {
    
     if( (!z_handle_table[q_handle_id].u_socket_retried_on_addr_change) &&
         (z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_TCP) &&
         (z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTING) &&
          /*If connecting and it was not a secured conneciton*/
          (
            (z_handle_table[q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
            ||
            /*If connecting and it was a secured conneciton but SSL is not yet tried*/
            ( (z_handle_table[q_handle_id].z_connect_params.e_use_security != SECURITY_NONE) &&
             (z_handle_table[q_handle_id].z_ssl_handle == NULL)
            )
          )
        )
      {
          
        sint15 x_temp_err_no  = DS_ENOTCONN;            
        sint15 x_temp_retval  = DSS_ERROR;
        uint8 test_buffer[2];
        (void)dss_read(z_handle_table[q_handle_id].x_sockfd,
                        &test_buffer,
                        2,
                        &x_temp_err_no );

        if(x_temp_err_no == DS_EIPADDRCHANGED)
        {
          z_handle_table[q_handle_id].u_socket_retried_on_addr_change = TRUE;
          MSG_HIGH("Ip Address changed, retrying socket connection on new socket",0,0,0);
          MSG_HIGH("Close previous socket",0,0,0);

          (void)pd_comms_app_task_dss_del_filter(q_handle_id);
          /*Close this old socket. Assumig it will close immediately*/
          x_temp_retval = dss_close( z_handle_table[q_handle_id].x_sockfd, &x_temp_err_no );
          
          if( x_temp_retval == DSS_SUCCESS )
          {                        
            pd_comms_connect_result_type e_conn_error;
            MSG_HIGH("Connect on new socket",0,0,0);
            z_handle_table[q_handle_id].x_sockfd = DSS_ERROR;
            e_status = pd_comms_tcp_task_connect_socket(q_handle_id, &e_conn_error );
            
            if( e_status == SUCCEEDED )
            {
              MSG_MED("Socket conneciton succesfull for new socket",0,0,0);
              /* Enable read data and close event */
              x_temp_retval  = dss_async_select( z_handle_table[q_handle_id].x_sockfd,
                DS_READ_EVENT | DS_CLOSE_EVENT,
                &x_temp_err_no );
            
              if(DSS_ERROR == x_temp_err_no)
              {
                print_dss_err(x_err_no);
              }
                
              /*If this is a NON-SSL link, notify client that the link is connected */
              if(z_handle_table[q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
              {
                /* Notify client that the link is connected */
                pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                     PDCOMM_STATE_CONNECTED,
                     TRUE,(uint32)PDCOMM_RESULT_OK);
              }
              else
              {
                /*Continue with opening SSL on this socket connection*/  
                pd_comms_tcp_task_open_ssl(z_handle_table[q_handle_id].z_handle);
              }

              return;
            }
            else if( e_status == PENDING )
            {
              MSG_MED("Socket conneciton pending for new socket",0,0,0);
              return;
            }
            
            
          }
          else
          {
            MSG_ERROR("Error in closing previous socket for Ip addr change",0,0,0);              
          }            
        }
      }
     
      /* This event occurs after the first call to dss_close(). Or the external host
        closed the socket. According to the API documentation, this indicates that 
        "pre-close" actions have been finished, however another call to dss_close() 
        is supposedly necessary to finish the job */

      if( (z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTING) &&
          (z_handle_table[q_handle_id].z_connect_params.e_use_security != SECURITY_NONE))
      {
        /*If we were not in disconnecting state, that means this CLOSE_EVENT is because
          the peer closed the socket so the SSL session was never deleted*/
        pd_comms_tcp_task_secure_tcp_disconnect(z_handle_table[q_handle_id].z_handle,FALSE,TRUE);
      }
      else
      {
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTING,
                                       TRUE,(uint32)PDCOMM_RESULT_WAIT);
        
        /*Delete the IP filter installed previously*/
        if(z_handle_table[q_handle_id].x_sockfd != DSS_ERROR)
        {
          if(!pd_comms_app_task_dss_del_filter(q_handle_id))
          { 
            PDCOMM_APP_MSG_ERROR("Error deleting IP filter for handle_id[%d]",q_handle_id,0,0);
          }
        }
        
        /*If there is a connection request pending, donot close PPP since PPP can be reused*/
        if(z_handle_table[q_handle_id].u_connectPending)
        {
          e_status = pd_comms_tcp_task_close_socket(q_handle_id,&e_disc_error,FALSE);
        }
        else
        {
          e_status = pd_comms_tcp_task_close_socket(q_handle_id,&e_disc_error,u_close_ppp);
        }

        if(e_status == SUCCEEDED)
        {
          /*If there was pending connection start processing that instead of going to DISCONNECTED*/  
          if(z_handle_table[q_handle_id].u_connectPending)
          {
            PDCOMM_APP_MSG_MED("Processing pending connection request for handle_id[%d]",
                               q_handle_id,0,0);
            z_handle_table[q_handle_id].u_connectPending = FALSE;
            pd_comms_tcp_task_handle_connect(&z_handle_table[q_handle_id].z_pendingConnectReq,TRUE);
          }  
          else
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,PDCOMM_STATE_DISCONNECTED,
                                           TRUE,(uint32) PDCOMM_RESULT_OK);
          }
        }
        else if( e_status == FAILED )
        {
          if(z_handle_table[q_handle_id].u_use_laptop_call == FALSE)
          {
            /* An error occurred. Teardown the PPP connection */
            e_status = pd_comms_tcp_task_close_ppp(q_handle_id,&e_disc_error);
          }
          else
          {
            e_status = SUCCEEDED;
          }

          if(e_status == SUCCEEDED)
          {
             pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                            PDCOMM_STATE_DISCONNECTED,
                                             TRUE,(uint32) PDCOMM_RESULT_OK);
          }
          else if(e_status == PENDING)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                           PDCOMM_STATE_DISCONNECTING,
                                           TRUE,(uint32) PDCOMM_RESULT_WAIT);
          }
          else if(e_status == FAILED)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                           PDCOMM_STATE_DISCONNECTED,
                                           TRUE,(uint32) e_disc_error);
          }

        }/*else if( e_status == FAILED )*/
        else if( e_status == PENDING )
        {
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32) PDCOMM_RESULT_WAIT);

        }/* if( status == PENDING) ends */
      }
    }/*if( result & DS_CLOSE_EVENT ) ends*/

    if( l_result & DS_WRITE_EVENT )
    {
      /* If we are in the "connecting" state, this event indicates that a socket
         has been opened successfully. Otherwise this event indicates it is ok
         to send more data to DS */
      if( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTING )
      {        
        struct ps_sockaddr *sock_addr = NULL;
        uint16 sock_addr_len =0;

        PDCOMM_APP_MSG_MED("Socket became writable for handle_id [%d]",q_handle_id,0,0);
        PDCOMM_APP_MSG_MED("Attempting to Connect",0,0,0);

        if(z_handle_table[q_handle_id].ip_addr_type == PD_COMMS_IP_ADDR_IPV4)
        {
          sock_addr = (struct ps_sockaddr *)&(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4);
          sock_addr_len = sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4);
        }
        else
        {  
          sock_addr = (struct ps_sockaddr *)&(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6);
          sock_addr_len = sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6);
        }


        /*lint -e740*/
        if(z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_UDP)
        {
          /* This should never happen, as a data gram socket should never rcv a WRITE 
           * event while in STATE_CONNETING
           */
          PDCOMM_APP_MSG_ERROR("Dgram Socket got WRITE evt in CONNECTING State [%d]",q_handle_id,0,0);
          x_retval = DSS_SUCCESS;
        }
        else if ( ( SECURITY_NONE != z_handle_table[q_handle_id].z_connect_params.e_use_security ) &&
                  ( NULL != z_handle_table[q_handle_id].z_ssl_handle ) ) 
        {
           u_forward_event = TRUE;
        }
        else
        {
          x_retval = dss_connect( z_handle_table[q_handle_id].x_sockfd, 
                                  sock_addr,
                                  sock_addr_len,
                                  &x_err_no );
        }
         /*lint +e740*/
       if ( TRUE == u_forward_event)
       {
         /*In the case where Socket callback for write event occurs, when SSL session is already 
           initiated simply forward the write event to SSL. SSL would have received a E_WOULDBLOCK
           on the socket during the SSL authentication process*/
          PDCOMM_APP_MSG_ERROR(" WRITE evt in SSL Connecting State [%d]",q_handle_id,0,0);

        }/*if( TRUE == u_forward_event ) ends*/
        else if( (x_retval == DSS_SUCCESS) || (( x_retval == DSS_ERROR ) && ( x_err_no == DS_EISCONN )) )
        {
          PDCOMM_APP_MSG_MED("Socket Connection Established for handle_id %d",
                             q_handle_id,0,0);

          /* Enable read data and close event */
          x_retval  = dss_async_select( z_handle_table[q_handle_id].x_sockfd,
                                        DS_READ_EVENT | DS_CLOSE_EVENT,
                                        &x_err_no );

          if(DSS_ERROR == x_retval)
          {
            print_dss_err(x_err_no);
          }
         
          /*If this is a NON-SSL link, notify client that the link is connected */
          if(z_handle_table[q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                           PDCOMM_STATE_CONNECTED,
                                           TRUE,(uint32)PDCOMM_RESULT_OK);
          }
          else
          {
            /*Continue with opening SSL on this socket connection*/  
            pd_comms_tcp_task_open_ssl(z_handle_table[q_handle_id].z_handle);
            result = TRUE;
          }

        }/*if( (x_retval == DSS_SUCCESS) || (( x_retval == DSS_ERROR ) && ( x_err_no == DS_EISCONN )) ) ends*/
        else if(( x_retval == DSS_ERROR ) && ( x_err_no == DS_EWOULDBLOCK ))
        {
          /* Enable write data and close event */
          x_retval = dss_async_select( z_handle_table[q_handle_id].x_sockfd,
                                       DS_WRITE_EVENT | DS_CLOSE_EVENT,
                                       &x_err_no );
          if(DSS_ERROR == x_retval)
          {
            print_dss_err(x_err_no);
            result = FALSE;
          }
          else
          {
              result = TRUE;
          }
         
        }/*else if(( x_retval == DSS_ERROR ) && ( x_err_no == DS_EWOULDBLOCK )) ends*/
        else
        {
         result = FALSE;
        }
        
        if(FALSE == result)
        {
          pd_comms_connect_result_type e_conn_error;

          MSG_ERROR("PDCOMM APP: Socket Connection Failed for handle_id",
                     q_handle_id,0,0);

          print_dss_err(x_err_no);
          e_conn_error = get_pdcomm_connect_error_from_dss_error(x_err_no);

          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)e_conn_error);

          /* Abort socket connection */
          e_status = pd_comms_tcp_task_close_socket(q_handle_id, &e_disc_error,u_close_ppp);

          if(e_status == SUCCEEDED)
          {
              pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                             PDCOMM_STATE_DISCONNECTED,
                                             TRUE,(uint32)e_conn_error);
          }
          else if( e_status == FAILED )
          {
            /* If the socket close is not pending then either the socket is closed or an error
             occurred. Either way teardown the PPP connection */
            if(z_handle_table[q_handle_id].u_use_laptop_call == FALSE)
            {
              e_status = pd_comms_tcp_task_close_ppp(q_handle_id,&e_disc_error);
            }
            else
            {
              e_status = SUCCEEDED;
            }
            if(e_status == SUCCEEDED)
            {
              pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                             PDCOMM_STATE_DISCONNECTED,
                                             TRUE,(uint32) PDCOMM_RESULT_OK);
            }
            else if(e_status == PENDING)
            {
              pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                             PDCOMM_STATE_DISCONNECTING,
                                             TRUE,(uint32) PDCOMM_RESULT_WAIT);
            }
            else if(e_status == FAILED)
            {
              pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                           PDCOMM_STATE_DISCONNECTED,
                                           TRUE,(uint32) e_disc_error);
            }

          }
          else if( e_status == PENDING )
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                           PDCOMM_STATE_DISCONNECTING,
                                           TRUE,(uint32) PDCOMM_RESULT_WAIT);
          }/*if( status == PENDING ) ends*/

        }/* if(FALSE == result)*/ 

      }/*if( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTING )*/ 
      else if( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED )
      {
        /*If this is a SSL link, simply forward the WRITE_EVENT to SSL layer*/
        if(z_handle_table[q_handle_id].z_connect_params.e_use_security != SECURITY_NONE)
        {
          u_forward_event = TRUE; 
        }
        else  
        {
          if(z_handle_table[q_handle_id].w_num_bytes_written < 
                              z_handle_table[q_handle_id].w_num_bytes_to_write)
          {
            (void)pd_comms_tcp_task_tcp_write(z_handle_table[q_handle_id].z_handle);
          }
        }

      } /*else for if( z_handle_table[handle_id].link_status == PDCOMM_STATE_CONNECTING ) ends*/

    } /*if( result & DS_WRITE_EVENT ) ends*/

    if( l_result & DS_READ_EVENT )
    {
      /*If this is a SSL link, simply forward the READ_EVENT to SSL layer*/
      if(z_handle_table[q_handle_id].z_connect_params.e_use_security != SECURITY_NONE)
      {
        u_forward_event = TRUE; 
      }
      else
      {
        if( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED )
        {
          if(z_handle_table[q_handle_id].z_connect_params.e_link_type == LINK_UDP)
          {
             /*This is UDP bind and there are listeners which are listening to UDP packets*/
             /*Handles's sockfd shoudl be set so that UDP reads from the correct socket*/
             z_handle_table[q_handle_id].x_sockfd = sockfd;  
          }
          
          if(z_handle_table[q_handle_id].w_num_bytes_already_read <
             z_handle_table[q_handle_id].w_num_bytes_to_read)
          {
           /*If a previous buffered read is pending, continue with that*/   
            (void)pd_comms_tcp_task_tcp_read(q_handle_id,TRUE);
          }
          else
          {
            /*Notify client about the read data availability*/  
            pd_comms_notification_params_type z_params;
            pd_comms_return_type e_result;
            
            z_params.e_new_state = z_handle_table[q_handle_id].e_link_status;
            e_result = pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                       z_handle_table[q_handle_id].q_source_thread_id,
                       z_handle_table[q_handle_id].q_base_msg_id,
                       z_handle_table[q_handle_id].z_connect_params.q_transaction_id,
                       READ_DATA,&z_params);
            
            if(e_result != PDCOMM_RESULT_OK)
            {
              MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                        (uint32)e_result,q_handle_id,0);

            }    
          }
        }
        else
        {
          MSG_ERROR("PDCOMM APP: READ event while link is DISCONNECTED for handle_id [%d]",
                    q_handle_id,0,0);
        }
      }
    }/*if( l_result & DS_READ_EVENT )*/


   /*
    *  Forward events meant for SSL layer
    */
    if((z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTED) &&
       (z_handle_table[q_handle_id].z_connect_params.e_use_security != SECURITY_NONE))
    {
      l_result_tmp = l_result;  
      l_result_tmp &= ~ (DS_CLOSE_EVENT | DS_WRITE_EVENT | DS_READ_EVENT);
      
      u_forward_event = u_forward_event || (l_result_tmp != 0);

      if( u_forward_event )
      {
        if(z_handle_table[q_handle_id].z_ssl_handle != NULL)
        {
          PDCOMM_APP_MSG_MED("Calling ds2_default_socket_event_handler for event[%ld] handle[%d]",
                             l_result,q_handle_id,0);
          secssl_ds2_default_sock_event_handler(z_handle_table[q_handle_id].x_app_id,
                                                z_handle_table[q_handle_id].x_sockfd,
                                                l_result,
                                                z_handle_table[q_handle_id].z_ssl_handle);

          /*secssl_ds_default_event_handler(SECSSL_DS_SOCKET_EVENT);*/
        }
      }
    }
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    MSG_ERROR("Socket callback: handle_id %d out of range", q_handle_id, 0, 0);

  }
   
} /* pd_comms_tcp_task_handle_sock_cb() ends */


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_dss_dns_cb

DESCRIPTION
  Whenever there is a callback from data services regarding a DNS request, this
  function is called.

============================================================================*/
static void pd_comms_tcp_task_handle_dss_dns_cb(
  /*Pointer to IPC message containing the dns callback information*/
  const os_IpcMsgType* p_msg )
{
  uint32 q_handle_id;
  dss_dns_session_mgr_handle_type  dns_session_handle;
  dss_dns_query_handle_type        dns_query_handle;
  int16                            x_dss_errno = 0;
  /*  dss_dns_addrinfo                 z_result_buf[MAX_RECORDS];*/
  dss_dns_addrinfo                 z_result_buf[1];
  int16                            x_retval;
  
  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */    
  q_handle_id = ((pd_comms_tcp_task_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  /*lint +e826 */    

  PDCOMM_APP_MSG_MED("DNS callback received for handle_id[%d]",q_handle_id,0,0);
  
  if( q_handle_id < PDCOMM_MAX_HANDLES )
  { 
  
    /* check if the callback reported any error and handle it right away */
    x_dss_errno = z_handle_table[q_handle_id].z_dns_cb_data.x_dss_errno;
    dns_session_handle = z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle;
    dns_query_handle = z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle;
    if (x_dss_errno != 0) 
    {
      switch (x_dss_errno)
      {
        case DS_ENOMEM:
          PDCOMM_APP_MSG_MED("DNS lookup failed: Out of memory [DS_ENOMEM] returned by DNS callback routine", 0,0,0);
          break;
        case DS_EHOSTNOTFOUND:
          PDCOMM_APP_MSG_MED("DNS lookup failed: Host not found [DS_EHOSTNOTFOUND] returned by DNS callback routine", 0, 0 ,0);
          break;
        case DS_ENETNONET:
          PDCOMM_APP_MSG_MED("DNS lookup failed: Network not found [DS_ENETNONET] returned by DNS callback routine", 0, 0 ,0);
          break;
        case DS_ENORECOVERY:
          PDCOMM_APP_MSG_MED("DNS lookup failed: Irrecoverable error [DS_ENORECOVERY] returned by callback routine",0,0,0);
          break;
        default:
          PDCOMM_APP_MSG_MED("DNS lookup failed: Unknown error",0,0,0);
      };
      pd_comm_tcp_task_process_dns_lookup_results(q_handle_id, FAILED, PDCOMM_CONNECT_ERROR_DNS_LOOKUP_FAILED);
    }
    else     /* no callback error; retrieve and process results */
    {
      /* DSS_DNS_READ_ADDRINFO() retrieves results from an earlier dns request */
      /* what should we do with multiple records?  
       for now assume record '0' is what we should work with, so get just one result
       record */

      x_retval = dss_dns_read_addrinfo(dns_session_handle, 
                                       dns_query_handle,
                                       z_result_buf,
                                       1,
                                       &x_dss_errno);

      if (x_retval == DSS_ERROR)
      {
        /* error while retrieving results */
        switch (x_dss_errno)
        {
          case DS_EBADF:
            PDCOMM_APP_MSG_MED("DNS lookup failed: Bad file descriptor error returned by dss_dns_read_addrinfo",
                             0,0,0);
            break;
          
          case DS_EWOULDBLOCK:
            PDCOMM_APP_MSG_MED("DNS callback invoked without lookup being complete, assuming lookup failed",0,0,0);
            break;
        }
        pd_comm_tcp_task_process_dns_lookup_results(q_handle_id, FAILED, PDCOMM_CONNECT_ERROR_DNS_LOOKUP_FAILED);
      }
      else       /* all is well, simply extract and use results */
      {
        /*Appropriatly fill in the structure elements depending on the af family*/
        if (z_result_buf[0].ai_family == DSS_AF_INET)
        {
          z_handle_table[q_handle_id].ip_addr_type = PD_COMMS_IP_ADDR_IPV4;
          z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_family =  (uint16)DSS_AF_INET;
          z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4
            = ((struct ps_sockaddr_in *)(&z_result_buf[0].ai_sockaddr))->ps_sin_addr.ps_s_addr;

          z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_addr.ps_s_addr
            = ((struct ps_sockaddr_in *)(&z_result_buf[0].ai_sockaddr))->ps_sin_addr.ps_s_addr;

          
          z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_port
                  = dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);
          /*Do not have to change the data connection family policy, because by default
          when hostname is used, DSS_AF_INET family is used*/
          
        }
        else
        {
          z_handle_table[q_handle_id].ip_addr_type = PD_COMMS_IP_ADDR_IPV6;
          z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_family = (uint16)DSS_AF_INET6;
          memscpy(&(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v6),
                 sizeof(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v6),
                 &(((struct ps_sockaddr_in6 *)(&z_result_buf[0].ai_sockaddr))->ps_sin6_addr),
                 sizeof(((struct ps_sockaddr_in6 *)(&z_result_buf[0].ai_sockaddr))->ps_sin6_addr));

          memscpy(&(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr),
                 sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr),
                 &(((struct ps_sockaddr_in6 *)(&z_result_buf[0].ai_sockaddr))->ps_sin6_addr),
                 sizeof(((struct ps_sockaddr_in6 *)(&z_result_buf[0].ai_sockaddr))->ps_sin6_addr));

          
          z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_port
                  = dss_htons(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);
        }
        
        /* third param doesn't matter if dns lookup succeeded*/
        pd_comm_tcp_task_process_dns_lookup_results(q_handle_id, SUCCEEDED, PDCOMM_CONNECT_RESULT_TYPE_BEGIN);
      } /* finishes - all is well, simply extract ....  */
    } /* finishes - no callback error; retrieve and ... */
    /* delete the DNS session now that we are done */
    x_retval = dss_dns_delete_session(dns_session_handle, &x_dss_errno);
    if( DSS_ERROR == x_retval )
    {
      MSG_ERROR("DNS session %d could not be deleted", dns_session_handle,0,0);
    }
    /*Cleanup DNS related variables*/
    z_handle_table[q_handle_id].z_dns_cb_data.dns_session_handle = 
                                                         DSS_DNS_SESSION_MGR_INVALID_HANDLE;
    z_handle_table[q_handle_id].z_dns_cb_data.dns_query_handle = DSS_DNS_QUERY_INVALID_HANDLE;
    
  }
  else
  {
    MSG_ERROR("Socket callback: handle_id %d out of range", q_handle_id, 0, 0);
  }
}

/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_atl_dns_cb

DESCRIPTION
  Whenever there is a DNS callback from an external entity registered through ATL,
  an IPC message is sent to PDCOMM APP.
  This IPC message is handled in this function. If the DNS result returns a
  valid ipadddress, this function will continue with socket connection or else
  will notify the client with appropriate pdcomm error

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_atl_dns_cb(
  /*Pointer to IPC message containing the dns callback information*/
  const os_IpcMsgType* p_msg )
{
  /*handle_id corresponding to the connection for which DNS lookup has been performed*/  
  uint32 q_handle_id;
  pd_comms_tcp_task_operation_status_type dns_task_status;
  pd_comms_connect_result_type e_conn_error;                                  
  
  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */    
  q_handle_id = ((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->handle.q_handle_id;
  dns_task_status = ((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->status;
  e_conn_error = PDCOMM_CONNECT_ERROR_UNKNOWN; /* initialized to keep lint happy */
  /*lint +e826 */    

  PDCOMM_APP_MSG_MED("DNS callback received for handle_id[%d]",q_handle_id,0,0);
  
  if( q_handle_id < PDCOMM_MAX_HANDLES ) 
  {
    if (dns_task_status == SUCCEEDED)
    {  
      /*Appropriatly fill in the structure elements depending on the af family*/
      if(((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->ip_addr.type == PDSM_ATL_IP_V4)
      {
        z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v4
          = ((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->ip_addr.addr.v4_addr;
        z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v4.ps_sin_addr.ps_s_addr
          = ((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->ip_addr.addr.v4_addr;
      }
      else
      {
        
        memscpy(&(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v6.ps_s6_addr16[0]),
			   sizeof(z_handle_table[q_handle_id].z_connect_params.z_link_params.z_tcp_params.q_ip_addr.q_ip_addr_v6.ps_s6_addr16),
               &(((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->ip_addr.addr.v6_addr[0]),
               sizeof(((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->ip_addr.addr.v6_addr));
        
        memscpy(&(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr.ps_s6_addr16[0]),
			   sizeof(z_handle_table[q_handle_id].z_peer_addr.z_peer_addr_v6.ps_sin6_addr.ps_s6_addr16),
               &(((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->ip_addr.addr.v6_addr[0]),
               sizeof(((pd_comms_app_dns_cb_data_type*)p_msg->p_Data)->ip_addr.addr.v6_addr));
      }
    }
    else
    {
        e_conn_error = PDCOMM_CONNECT_ERROR_DNS_LOOKUP_FAILED; /* only set during error */
    }
    pd_comm_tcp_task_process_dns_lookup_results(q_handle_id, dns_task_status, e_conn_error);
  }
  else
  {
    MSG_ERROR("Socket callback: handle_id %d out of range", q_handle_id, 0, 0);

  }
}


/*============================================================================
FUNCTION pd_comm_tcp_task_process_dns_lookup_results

DESCRIPTION
  A common function to process the results produced by both data service and 
  remote ATL-based dns lookup functions.
=============================================================================*/
static void pd_comm_tcp_task_process_dns_lookup_results(
  /*handle_id corresponding to the connection for which DNS lookup has been performed*/  
  uint32 q_handle_id, 
  pd_comms_tcp_task_operation_status_type e_status, 
  pd_comms_connect_result_type e_conn_error)
{
  sint15 x_err_no;
  sint31 x_retval;
  boolean u_close_ppp = TRUE;
  if(z_handle_table[q_handle_id].u_use_laptop_call == TRUE)
  {
    u_close_ppp = FALSE;
  }

  switch(e_status)
    {
    case FAILED:
      { 
        pd_comms_disconnect_result_type e_disc_error;

        PDCOMM_APP_MSG_MED("DNS Lookup failed. Closing socket",0,0,0);

      
        e_status = pd_comms_tcp_task_close_socket(q_handle_id,&e_disc_error,u_close_ppp);
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
               PDCOMM_STATE_DISCONNECTING,
               TRUE,(uint32)e_conn_error);
    
        if(e_status == SUCCEEDED)
        {
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
             PDCOMM_STATE_DISCONNECTED,
             TRUE,(uint32) PDCOMM_RESULT_OK);
        }
        else if(e_status == PENDING)
        {
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
             PDCOMM_STATE_DISCONNECTING,
             TRUE,(uint32) PDCOMM_RESULT_WAIT);
        }
        else if(e_status == FAILED)
        {
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
             PDCOMM_STATE_DISCONNECTED,
             TRUE,(uint32) e_disc_error);
        }
        
        break;
      }/*case FAILED*/
      
    case SUCCEEDED:
      {
        e_status = pd_comms_tcp_task_connect_socket(q_handle_id, &e_conn_error );
  
        if( e_status == SUCCEEDED )
        {
          /* Enable read data and close event */
          x_retval  = dss_async_select( z_handle_table[q_handle_id].x_sockfd,
            DS_READ_EVENT | DS_CLOSE_EVENT,
            &x_err_no );
      
          if(DSS_ERROR == x_retval)
          {
            print_dss_err(x_err_no);
          }
            
          /*If this is a NON-SSL link, notify client that the link is connected */
          if(z_handle_table[q_handle_id].z_connect_params.e_use_security == SECURITY_NONE)
          {
            /* Notify client that the link is connected */
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                 PDCOMM_STATE_CONNECTED,
                 TRUE,(uint32)PDCOMM_RESULT_OK);
          }
          else
          {
            /*Continue with opening SSL on this socket connection*/  
            pd_comms_tcp_task_open_ssl(z_handle_table[q_handle_id].z_handle);
          }
        }
        else if( e_status == FAILED )
        {                                               
          pd_comms_disconnect_result_type e_disc_error;
      
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
             PDCOMM_STATE_DISCONNECTING,
             TRUE,(uint32)e_conn_error);
      
          /* Teardown socket connection */
          e_status = pd_comms_tcp_task_close_socket(q_handle_id,&e_disc_error,TRUE);
      
          if(e_status == SUCCEEDED)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                 PDCOMM_STATE_DISCONNECTED,
                 TRUE,(uint32) PDCOMM_RESULT_OK);
          }
          else if(e_status == PENDING)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                 PDCOMM_STATE_DISCONNECTING,
                 TRUE,(uint32) PDCOMM_RESULT_WAIT);
          }
          else if(e_status == FAILED)
          {
            pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                 PDCOMM_STATE_DISCONNECTED,
                 TRUE,(uint32) e_disc_error);
          }
        }
        else if( e_status == PENDING )
        {
      
          /* Notify client that the socket connection has started */
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
             PDCOMM_STATE_CONNECTING,
             TRUE,(uint32)PDCOMM_RESULT_WAIT);
        }
        break;
      }/*case SUCCEEDED*/
      
    case PENDING:
      {
        pd_comms_disconnect_result_type e_disc_error;
  
        /*Shouldn't normally reach here. Kept for safety*/
        MSG_ERROR("PDCOMM APP: DNS Lookup state still pending for handle id [%d]. DISCONNECTING",
                  q_handle_id,0,0);
        /*Treat this as an error for now. Since during unit testing 
       *it's seen that this case blocks for ever and we never get back a DNS
       *callback after this*/
        e_conn_error = PDCOMM_CONNECT_ERROR_DNS_LOOKUP_FAILED;
  
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
               PDCOMM_STATE_DISCONNECTING,
               TRUE,(uint32)e_conn_error);
  
        /* Teardown socket connection */
      e_status = pd_comms_tcp_task_close_socket(q_handle_id,&e_disc_error,u_close_ppp);

      if(e_status == SUCCEEDED)
      {
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
          PDCOMM_STATE_DISCONNECTED,
          TRUE,(uint32) PDCOMM_RESULT_OK);
      }
      else if(e_status == PENDING)
      {
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
          PDCOMM_STATE_DISCONNECTING,
          TRUE,(uint32) PDCOMM_RESULT_WAIT);
      }
      else if(e_status == FAILED)
      {
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
          PDCOMM_STATE_DISCONNECTED,
          TRUE,(uint32) e_disc_error);
      }
      break;
    }/*case PENDING*/

  }/*switch(status) ends*/ 
}


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_phy_link_down_cb

DESCRIPTION
  Whenever there is a NETWORK callback, an IPC message is sent to PDCOMM APP.
  This IPC message is handled in this function. It reads the network status
  by calling dss_netstatus() and takes the appropriate action based upon the
  recevied netstatus. 

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_handle_phy_link_down_cb(
  /*Pointer to IPC message containing the network callback information */
  const os_IpcMsgType* p_msg )
{
  uint32 q_handle_id;
  pd_comms_ioctl_ack_params_type z_ioctl_ack;

  if(p_msg == NULL)
  {
    return;
  }
  /*lint -e826 */    
  q_handle_id = ((pd_comms_tcp_task_phy_link_down_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
   /*lint +e826 */    

  PDCOMM_APP_MSG_MED("PHY_LINK_DOWN callback received for handle_id[%d]",q_handle_id,0,0);
  
  if( q_handle_id < PDCOMM_MAX_HANDLES )
  { 
    if(z_handle_table[q_handle_id].e_dormancy_state == 
       PD_COMMS_DORMANCY_OPERATION_WAIT_PHY_LINK_DOWN)
    {
      /* Set the socket signal */
      PDCOMM_APP_MSG_MED( "[phy_link_down] event for handle [%d], while link status is %d", 
                q_handle_id, (uint32)z_handle_table[q_handle_id].e_link_status, 0);

      z_handle_table[q_handle_id].e_dormancy_state = PD_COMMS_DORMANCY_OPERATION_INIT;    
      
      z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_FORCE_DORMANCY;
      z_ioctl_ack.q_transaction_id = z_handle_table[q_handle_id].q_ioctl_transaction_id;
      z_ioctl_ack.q_argument_len = 0;
      (void)pd_comms_send_ioctl_ack_to_client(z_handle_table[q_handle_id].z_handle,
                                              z_handle_table[q_handle_id].q_source_thread_id,
                                              z_handle_table[q_handle_id].q_base_msg_id,
                                              &z_ioctl_ack);

      /*De-register the physical link down event callback*/
      (void)pd_comms_dss_dereg_phy_link_down_event(q_handle_id);

    }
    else
    {
      PDCOMM_APP_MSG_ERROR( "Unexpected phy_link_down event", 0, 0, 0);       
    }
        
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    PDCOMM_APP_MSG_ERROR("phy_link_down callback: handle_id %d out of range", q_handle_id, 0, 0);    
  }
} /*pd_comms_tcp_task_handle_phy_link_down_cb*/


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
        MSG_ERROR("PDCOMM APP: Handle [%d] inactive for more than %d millisec. Aborting connection..",
                  l_i,PDCOMM_INACTIVITY_TIMER_PERIOD,0);

        if(z_handle_table[l_i].e_link_status == PDCOMM_STATE_DISCONNECTED)
        {
          MSG_ERROR("PDCOMM APP: Client already notified about aborted connection. Release Handle [%d]",
                    l_i,0,0);
        }
        else
        {
          if((z_handle_table[l_i].z_connect_params.e_link_type == LINK_TCP) ||
             (z_handle_table[l_i].z_connect_params.e_link_type == LINK_UDP))
          {
            /*abort the connection*/
            /*pdcomm_app_task_tcp_disconnect(z_handle_table[l_i].z_handle, TRUE);*/
            if(z_handle_table[l_i].z_connect_params.e_use_security == SECURITY_NONE)
            {
              z_handle_table[l_i].session_aborted = TRUE;
              pd_comms_tcp_task_dss_abort_connection( l_i, TRUE );
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
FUNCTION pd_comms_tcp_handle_srv_system_change_notification

DESCRIPTION
  This function catches the present system mode.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/

static void pd_comms_tcp_handle_srv_system_change_notification(const os_IpcMsgType* p_msg)
{
  uint32 *sys_mode;  
  if(p_msg == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL IPC received in srving system change callback",0,0,0);
    return;
  }

  sys_mode = (uint32*)(p_msg->p_Data);
  srv_sys_mode = (sys_sys_mode_e_type)(*sys_mode);
  MSG_HIGH("Pdcomm: Srving system changed to %d", srv_sys_mode, 0,0);
}

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

      case C_PD_COMMS_APP_SRV_SYS_CHANGE_NOTIFICATION:
      {
          pd_comms_tcp_handle_srv_system_change_notification(p_msg);
          break;
      }
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

  /* Create a new Hash function crypto instance */
  /*if( secapi_new( &z_hash_handle, SECAPI_SHA ) != E_SUCCESS )
  {  
    MSG_ERROR("PDCOMM APP: Could not create a hash function instance",0,0,0);
  } */

  /* Start the inactivity timer. */
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

/*===========================================================================
FUNCTION pd_comms_tcp_read_apn_profiles_from_nv

DESCRIPTION
  This function initialises the APN profile table by reading from nv

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_read_apn_profiles_from_nv(cgps_nv_efs_reg_item_type item, 
                                                                uint32   profile_type,
                                                                boolean force_read_from_hw)
{
  uint8 index=0;
  uint32 srv_sys_type = 0;
  uint32 pdp_type = 0;
  uint16 i=0;

  if(profile_type >= PDCOMM_APN_PROFILE_TYPE_MAX)
  {
    return;
  }

  memset(nv_apn_profiles, 0, sizeof(nv_apn_profiles));

  /*First clear away the data for this profile*/
  for(index = 0; index < PDCOMM_APN_PROFILE_SRV_SYS_MAX_INDEX; index++)
  {
    memset(&pdcomm_tcp_apn_profiles[index][profile_type], 0, sizeof(apn_profiles_type));
  }

  if ( pd_comms_nv_efs_reg_item_read(item, 
    sizeof(pdsm_pa_apn_profiles_type) * PDSM_PA_MAX_APN_PROFILES,
    &nv_apn_profiles, force_read_from_hw) == TRUE)
  {
    for(index=0; index <PDSM_PA_MAX_APN_PROFILES; index++)
    {
      srv_sys_type = nv_apn_profiles[index].srv_system_type;
      pdp_type = nv_apn_profiles[index].pdp_type;
      
      if( (nv_apn_profiles[index].apn_name[0] == 0) || 
          (srv_sys_type > PDSM_PA_APN_PROFILE_SRV_SYS_MAX) ||
          (pdp_type > PDSM_PA_APN_PROFILE_PDN_TYPE_MAX)
        )
      {
        continue;
      }

      i = 0;
      while ( ( nv_apn_profiles[index].apn_name[i] != 0 ) &&  
              ( i < ( PDSM_PA_MAX_APN_NAME_LENGTH -1 ) )
            )
      {
        if(srv_sys_type & PDSM_PA_APN_PROFILE_SRV_SYS_CDMA)
        {
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_CDMA_INDEX][profile_type].apn_name[i]= nv_apn_profiles[index].apn_name[i];
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_CDMA_INDEX][profile_type].pdp_type= pdp_type;
        }
        if(srv_sys_type & PDSM_PA_APN_PROFILE_SRV_SYS_HDR)
        {
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_HDR_INDEX][profile_type].apn_name[i] = nv_apn_profiles[index].apn_name[i];
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_HDR_INDEX][profile_type].pdp_type= pdp_type;
        }
        if(srv_sys_type & PDSM_PA_APN_PROFILE_SRV_SYS_GSM)
        {
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_GSM_INDEX][profile_type].apn_name[i] = nv_apn_profiles[index].apn_name[i];
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_GSM_INDEX][profile_type].pdp_type= pdp_type;
        }
        if(srv_sys_type & PDSM_PA_APN_PROFILE_SRV_SYS_WCDMA)
        {
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_WCDMA_INDEX][profile_type].apn_name[i] = nv_apn_profiles[index].apn_name[i];
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_WCDMA_INDEX][profile_type].pdp_type= pdp_type;
        }
        if(srv_sys_type & PDSM_PA_APN_PROFILE_SRV_SYS_LTE)
        {
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_LTE_INDEX][profile_type].apn_name[i] = nv_apn_profiles[index].apn_name[i];
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_LTE_INDEX][profile_type].pdp_type= pdp_type;
        }
        if(srv_sys_type & PDSM_PA_APN_PROFILE_SRV_SYS_TDS)
        {
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_TDS_INDEX][profile_type].apn_name[i] = nv_apn_profiles[index].apn_name[i];
          pdcomm_tcp_apn_profiles[PDCOMM_APN_PROFILE_SRV_SYS_TDS_INDEX][profile_type].pdp_type= pdp_type;
        }        
        
        i++;
      }   
    }
  }
}


/*===========================================================================
FUNCTION pd_comms_tcp_task_read_nvs

DESCRIPTION
  This function reads the nv items required by Pdcomms tcp task

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_read_nvs(void)
{
  
  /*Read LBS Apn profile*/
  pd_comms_tcp_task_read_apn_profiles_from_nv(CGPS_NV_EFS_PDCOMM_LBS_APN_PROFILES,
                                         PDCOMM_APN_PROFILE_TYPE_LBS, TRUE);

  /*Read XTRA Apn profile*/
  pd_comms_tcp_task_read_apn_profiles_from_nv(CGPS_NV_EFS_PDCOMM_XTRA_APN_PROFILES,
                                         PDCOMM_APN_PROFILE_TYPE_XTRA, TRUE);

  /*Read Emergency LBS Apn profile*/
  pd_comms_tcp_task_read_apn_profiles_from_nv(CGPS_NV_EFS_PDCOMM_EMERGENCY_LBS_APN_PROFILES,
                                         PDCOMM_APN_PROFILE_TYPE_LBS_EMERGENCY, TRUE);

  /*Read SSL certificate key length*/
  if (FALSE == pd_comms_nv_efs_reg_item_read(CGPS_NV_EFS_PDCOMM_SSL_CERTIFICATE_KEY_LENGTH,
                                    sizeof(nv_ssl_certificate_key_length),&nv_ssl_certificate_key_length,TRUE))
  {
    /* if read fails, set the key length to default */
    //nv_ssl_certificate_key_length = 64;
    nv_ssl_certificate_key_length = CGPS_NV_EFS_PDCOMM_SSL_RSA_CERTIFICATE_KEY_LENGTH_DEFAULT;
  }
}


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION  pd_comms_tcp_task__mcfg_refresh_cb

DESCRIPTION
  Client task is expected to copy over the p_info and put the refresh update into 
its own command queue 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE for success, FALSE for failure

SIDE EFFECTS
  None

===========================================================================*/
static boolean pd_comms_tcp_task_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info)
{
  if (p_info == NULL)
  {
    MSG( MSG_SSID_GPS, MSG_LEGACY_ERROR, "PDCOMM MCFG_REFRESH got NULL ptr!");
    return FALSE;
  }

  MSG_3( MSG_SSID_GPS, MSG_LEGACY_HIGH, "PDCOMM MCFG_REFRESH got type=%u index=%u slot=%u", 
          (uint32)p_info->type, (uint32)p_info->slot_index, (uint32)p_info->sub_index);

  /*Read LBS Apn profile from hardware for Hot SIM swap */
  pd_comms_tcp_task_read_apn_profiles_from_nv(CGPS_NV_EFS_PDCOMM_LBS_APN_PROFILES,
                                         PDCOMM_APN_PROFILE_TYPE_LBS, TRUE);

  /* Read Emergency LBS Apn profile from hardware for Hot SIM swap */
  pd_comms_tcp_task_read_apn_profiles_from_nv(CGPS_NV_EFS_PDCOMM_EMERGENCY_LBS_APN_PROFILES,
                                         PDCOMM_APN_PROFILE_TYPE_LBS_EMERGENCY, TRUE);

  /* This function is used to notify MCFG task that registered client has
  completed refresh operation. The client is expected to return the same
  reference id passed through registered callback */
  MSG_1( MSG_SSID_GPS, MSG_LEGACY_MED, "PDCOMM MCFG_REFRESH refresh done, RefID=%d", p_info->reference_id);
  (void)mcfg_refresh_done(p_info->reference_id);  

  return TRUE;
}

/*===========================================================================
FUNCTION pd_comms_tcp_task_register_mcfg_refresh

DESCRIPTION
  This function register MCFG refresh callback with MCFG

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_register_mcfg_refresh(void)
{
  mcfg_refresh_registration_s_type pdcomm_mcfg_refresh = 
  {
    MCFG_REFRESH_ORDER_50,        /**< Refresh order ID registed for refresh */
    MCFG_REFRESH_INDEX_MASK_ALL,  /**< Mask for slot(s) to recieve refresh update */
    MCFG_REFRESH_INDEX_MASK_ALL,  /**< Mask for sub(s) to recieve refresh update */
    0,                            /**< Reserved. */
    pd_comms_tcp_task_mcfg_refresh_cb  /**< Callback function to receive refresh update */
  };

  boolean status = mcfg_refresh_register(&pdcomm_mcfg_refresh);

  if (status == TRUE)
  {
    MSG( MSG_SSID_GPS, MSG_LEGACY_MED, "PDCOMM MCFG_REFRESH registration success");
  }
  else
  {
    MSG( MSG_SSID_GPS, MSG_LEGACY_ERROR, "PDCOMM MCFG_REFRESH registration fail!");
  }
}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */


/*===========================================================================
FUNCTION pd_comms_tcp_get_sys_mode

DESCRIPTION
  This function returns the present system mode.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/

sys_sys_mode_e_type pd_comms_tcp_get_sys_mode(void)
{
  return srv_sys_mode;
}


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_set_data_enable

DESCRIPTION
  This function enables/disables data

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_set_data_enable(boolean data_enable)
{
  u_pd_comms_data_enabled = data_enable;

  if(u_pd_comms_data_enabled)
  {
    MSG_HIGH("Data is Enabled",0,0,0);
  }
  else
  {
    MSG_HIGH("Data is Disabled",0,0,0);    
  }
} 




/*===========================================================================
FUNCTION pd_comms_log_connection_status_event

DESCRIPTION
  This function is used to log the different types of events from PDCOMMS into a log packet.

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_log_connection_status_event(pd_event_e_type event_type)
{
  LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type *p_LogPkt;
  uint8 i;
  MSG_LOW("Inside pd_comms_log_connection_status_event for %d",event_type,0,0);
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type*)log_alloc( 
             LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C,
             sizeof(LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type) );
  if (p_LogPkt == NULL) 
  {
    MSG_ERROR("PDCOMM : log_alloc error",0,0,0);
    return;
  }
  p_LogPkt->q_ClientId = 0;
  p_LogPkt->u_EventData = 0;

  switch(event_type)
  {
    case PD_EVENT_NW_CONNECTING:
      p_LogPkt->u_EventType = PD_EVENT_NW_CONNECTING;
      break;
    case PD_EVENT_NW_CONNECTED:
      p_LogPkt->u_EventType = PD_EVENT_NW_CONNECTED;
      break;
    case PD_EVENT_SERVER_CONNECTING:
      p_LogPkt->u_EventType = PD_EVENT_SERVER_CONNECTING;
      break;
    case PD_EVENT_SERVER_CONNECTED:
      p_LogPkt->u_EventType = PD_EVENT_SERVER_CONNECTED;
      break;
    default:
      MSG_HIGH("Unrecognised event to log",event_type,0,0);
      p_LogPkt->u_EventType = PD_EVENT_UNDEF_FOR_LOG;
      p_LogPkt->u_EventData = 0;
      break;
  }
    p_LogPkt->u_PacketVersion = GNSS_GEN8_LOG_VERSION; /* new log for Gen8 */
  for (i=0; i<7; i++)
    p_LogPkt->u_Reserved[i] =0;
  log_commit(p_LogPkt);
}

#endif

#endif /* FEATURE_CGPS_PDCOMM */

