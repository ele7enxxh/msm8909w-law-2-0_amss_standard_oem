/*======================================================================

               PDCOMM Application task internal header file

 GENERAL DESCRIPTION
  This file contains MACROS and data-structures internally used by 
  PDCOMM Application.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_tcp/src/pd_comms_tcp_data.h#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 11/18/15   skm  PDCOMMS release handle crash fix 
 01/23/14   skm  Support for UDP EPC_GROUP and removing Interface UP/DOWN event listeners
 12/13/13   sm   Remove default-listeners and add Ipv4v6 preference selection
 06/11/13   ssh  Added Support for UDP to listen on all interfaces
 03/07/11   rh   Added TLS/SSL support for BIT API
 06/03/10   rh   Added featurization for BIT API
 04/04/06   mr   Initial creation of header file.
======================================================================*/

#ifndef PDCOMM_APP_TCP_DATA_H
#define PDCOMM_APP_TCP_DATA_H

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_CGPS_PDCOMM 
#include "dssocket.h"
#include "dssdns.h"
#include "pd_comms_api.h"
#include "pd_comms_app_api.h"
#include "secssl.h"
#include "pdsm_atl.h"
#include "pd_comms_nv_efs.h"
#include "sm_log.h"

/****************** Constants/Enums *******************************************/

#define PDCOMM_APP_MSG_LOW( str, a, b, c )      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,  "=PDCOMM APPLICATION= " str, a, b, c )
#define PDCOMM_APP_MSG_MED( str, a, b, c )      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "=PDCOMM APPLICATION= " str, a, b, c )
#define PDCOMM_APP_MSG_ERROR( str, a, b, c )    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "=PDCOMM APPLICATION= " str, a, b, c )
#define PDCOMM_APP_MSG_HIGH( str, a, b, c )     MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "=PDCOMM APPLICATION= " str, a, b, c )
#define PDCOMM_APP_ERR_FATAL( str, a, b, c )    ERR_FATAL( "=PDCOMM APPLICATION= " str, a, b, c )

#ifdef FEATURE_CGPS_LBS_TASK
#define PDCOMM_MAX_HANDLES 6
#else
#define PDCOMM_MAX_HANDLES 4
#endif
#define PDCOMM_WRITE_DATA_BUF_SIZE 2500
#define PDCOMM_READ_DATA_BUF_SIZE 2000
#define PDCOMM_MAX_SSL_CERT_SIZE 2048
#define PDCOMM_MAX_SSL_CERT_DIR_PATH_SIZE 64
#define PDCOMM_INACTIVITY_TIMEOUT 40000 /*in millisecs*/
#define PDCOMM_INACTIVITY_TIMER_PERIOD  5000/*in millisecs*/
#define PDCOMM_UDP_SERVER_DUMMY_IP 0x7f000001

#define PDCOMM_UDP_MAX_LISTENERS  16
/*This enumeration defines all the internal IPC message IDs 
 *for PDCOMM_APP_TASK. PDCOMM_IPC_END is used to prevent 
 *overlap of these message IDs with the PDCOMM interface message
 *IDs*/
typedef enum
{
  PDCOMM_APP_TASK_DSS_NETWORK_CB = C_PD_COMMS_APP_MSG_IPC_END+1,
  PDCOMM_APP_TASK_DSS_SOCKET_CB,
  PDCOMM_APP_TASK_DSS_DNS_CB,
  PDCOMM_APP_TASK_DSS_PHY_LINK_DOWN_CB,  
  PDCOMM_APP_TASK_SEC_EVENT_CB,
  PDCOMM_APP_TASK_SEC_OPEN_CB,  
  PDCOMM_APP_TASK_SEC_CLOSE_CB,
  PDCOMM_APP_TASK_SEC_CLOSE_NET_CB,      
  PDCOMM_APP_TASK_DOG_TIMER,
  PDCOMM_APP_TASK_INACTIVITY_TIMER,
  PDCOMM_APP_TASK_ATL_DNS_CB,

#ifdef FEATURE_GNSS_BIT_API
  PDCOMM_APP_TASK_BIT_EVENT_CB,
#endif
} pd_comms_tcp_task_ipc_message_type;

/* Dormancy Operation State
*/
typedef enum
{
  PD_COMMS_DORMANCY_OPERATION_INIT,
  PD_COMMS_DORMANCY_OPERATION_WAIT_PHY_LINK_DOWN
} pd_comms_dormancy_state_type;

typedef enum
{
  SUCCEEDED,
  FAILED,
  PENDING
} pd_comms_tcp_task_operation_status_type;

/*Data connection family type fallback state*/
/*Fallback is done when a hostname is provided and 
  and PDCOMMS first tries a Ipv4 data call. If the DNS
  resolves to an Ipv6 address, then a Ipv6 data call is initiated,
  and the previous Ipv4 data call is torn down. 
  Below enum represents the states during such a fallback
*/
typedef enum
{
  FALLBACK_STATE_NONE,
  FALLBACK_STATE_WAIT_CLOSE,
  FALLBACK_STATE_WAIT_OPEN
} pd_comms_tcp_task_data_family_fallback_state;

/****************** Data Structures*********************************************/
typedef struct
{
  pdsm_atl_addr_family_e_type type; 

  union
  {
    uint32 v4_addr;   
    uint8 v6_addr[16];
  } addr; 
} pdcomm_ip_addr_type;


/* DNS Callback parameters */
typedef struct
{
  /*
    these fields are used in the 'default' dns case
   */
  dss_dns_session_mgr_handle_type   dns_session_handle;
  dss_dns_query_handle_type         dns_query_handle;
  int16                             x_dss_errno;
  /*  
    used when the functions are processed through registered ATL functions
   */
  pd_comms_handle_type handle;
  pdcomm_ip_addr_type  ip_addr;
  pd_comms_tcp_task_operation_status_type status;
} pd_comms_app_dns_cb_data_type;


/* PDCOMM APP state information */
typedef struct
{
  /*General PD-COMM state*/
  boolean                      in_use;      /*default value FALSE*/
  pd_comms_state_type          e_link_status;/*default value COMM_STATE_DISCONNECTED*/
  pd_comms_connect_params_type z_connect_params;
  uint32                       q_source_thread_id; /*thread_id of the client*/
  uint32                       q_base_msg_id; /*base message ID when responding back to client*/
  pd_comms_handle_type         z_handle;
  boolean                      client_disconnect_recvd; /*default FALSE*/
  boolean                      session_aborted; /*default FALSE*/

  pd_comms_ipc_connect_hdr_type z_pendingConnectReq; /*connect request received while the
                                                       previous connect is still in DISCONNECTING
                                                       state*/
  boolean                       u_connectPending;
  /*Inactivity Timer*/
  int                          l_inactivity_count; /*default value 0*/
                                                        
  sint15                       x_prev_app_id; /*Application id of previous data call during handover*/
  sint15                       x_sockfd;     /*default value DSS_ERROR*/
  union
  {
    struct                     ps_sockaddr_in   z_peer_addr_v4;
    struct                     ps_sockaddr_in6  z_peer_addr_v6;   
  } z_peer_addr;  
  pd_comms_tcp_ip_addr_enum_type ip_addr_type;  

  /*Write parameters*/
  uint16                       w_num_bytes_to_write;     /* Number of bytes in buffer to send */
  uint16                       w_num_bytes_written;      /* Number of bytes in buffer sent */
  /*MR: Make this dynamic allocation once DMA is implemented in PDCOMM*/
  uint8                        p_write_buff[PDCOMM_WRITE_DATA_BUF_SIZE];          /* data to send */

  /*Read parameters*/
  uint16                       w_num_bytes_to_read;      /* Number of bytes to be read */
  uint16                       w_num_bytes_already_read; /* Number of bytes already read */

  /*Read Buffer*/
  uint16                       w_num_bytes_in_buffer; /* Number of bytes in p_read_buff[]. Used only in SMS*/
  uint8*                       p_read_ptr; /*Used only in SMS*/
  /*MR: Make this dynamic allocation once DMA is implemented in PDCOMM*/
  uint8                        p_read_buff[PDCOMM_READ_DATA_BUF_SIZE];          /* data buffer */
  
  /* transaction IDs*/
  /*connect_transaction_id stored in connect_params*/
  uint32                      q_disconnect_transaction_id;
  uint32                      q_write_transaction_id;
  uint32                      q_read_transaction_id;
  uint32                      q_ioctl_transaction_id;

  boolean                      requested_read;  /* TRUE if READ_DATA has been sent */

#ifdef FEATURE_GNSS_BIT_API 
  uint16                       session_handle;  /* session handle for BIT API */
  uint32                       ssl_event_mask;  /* current DS event mask for SSL */
  uint32                       ssl_events;      /* current events for SSL */
  uint32                       ssl_to_write_bytes;  /* SSL bytes to write */
  uint32                       ssl_written_bytes;   /* SSL bytes written by GPS daemon */

#else  /* FEATURE_GNSS_BIT_API */

  /* TCP / IP */
  sint15                       x_app_id;     /* Application identifier Also used for SSL */ 
  pd_comms_dormancy_state_type e_dormancy_state; /*Dormancy state of the connection*/
  sint15                       x_force_dorm_app_id;/*App-ID used for forcing dormancy 
                                                    when PPP proxy is present*/
  /* DNS */
  pd_comms_app_dns_cb_data_type z_dns_cb_data; 

#endif /* FEATURE_GNSS_BIT_API */

  /* Security */
  secssl_handle_type z_ssl_handle;
  /*MR: Make this dynamic allocation once DMA is implemented in PDCOMM*/
  uint8 p_ssl_cert[PDCOMM_MAX_SSL_CERT_SIZE]; 
  uint16 w_ssl_cert_len;
  char  p_ssl_cert_dir[PDCOMM_MAX_SSL_CERT_DIR_PATH_SIZE];

  boolean u_use_laptop_call; /* flag indicating if the handle is using an existing laptop call */

  uint16 datacall_family; /*Present family type used for brining up the data call*/
  pd_comms_tcp_task_data_family_fallback_state z_datacall_family_fallback_state;
  boolean  u_fallback_allowed;

  boolean u_socket_retried_on_addr_change; /*Socket Retry on Ip Address Change*/
  boolean v_release_handle;
} pd_comms_tcp_task_handle_data_type;


/*The NETWORK/SOCKET callback functions will send an internal
 *message to PDCOMM_APP_TASK in pdcomm_app_task_cb_ipc_hdr_type
 *format to notify PDCOMM_APP_TASK regarding NETWORK/SOCKET EVENT*/
typedef struct
{
  pd_comms_handle_type handle;  
  sint15  x_appid;
  sint15 x_sockfd; 
  uint32 event_mask; 
  pdsm_atl_open_params_type z_open_resp;
} pd_comms_tcp_task_tcp_cb_ipc_hdr_type;

typedef struct
{
  pd_comms_handle_type handle;
} pd_comms_tcp_task_phy_link_down_cb_ipc_hdr_type;

typedef struct
{
  pd_comms_handle_type handle;
  secssl_ses_event_enum_type event_mask; 
} pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type;

typedef struct
{
  pd_comms_handle_type handle;
  secerrno_enum_type e_error; 
} pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type;

typedef struct
{
  pd_comms_handle_type handle;
  secerrno_enum_type e_error; 
} pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type;

typedef struct
{
  pd_comms_handle_type handle;
  secerrno_enum_type e_error; 
} pd_comms_tcp_task_secure_tcp_close_net_cb_ipc_hdr_type;


typedef struct
{
  pd_comms_handle_type handle;
  ps_iface_type       *ifacePtr;
  ps_iface_event_enum_type event;   
} pd_comms_tcp_task_iface_event_cb_ipc_hdr_type;


/******************************Function declarations**************************/

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
extern pd_comms_return_type  pd_comms_tcp_task_handle_get_handle( 
  /*contains the get_handle params*/  
  const pd_comms_get_handle_params_type* const p_params,
  /*the assigned handle is returned in this variable */
  pd_comms_handle_type* p_handle  );


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
extern pd_comms_return_type  pd_comms_tcp_task_handle_release_handle( 
  /*the assigned handle is returned in this variable */  
  const pd_comms_handle_type* p_handle);

/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_set_data_enable

DESCRIPTION
  This function enables/disables

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
extern void pd_comms_tcp_task_handle_set_data_enable(boolean data_enable);

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
extern void pd_comms_tcp_task_read_apn_profiles_from_nv(cgps_nv_efs_reg_item_type item, 
                                                        uint32   profile_type,                                                                      
                                                        boolean force_read_from_hw);


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
extern void pd_comms_log_connection_status_event(pd_event_e_type event_type);


#endif /* FEATURE_CGPS_PDCOMM */
#endif /* PDCOMM_APP_TCP_DATA_H */
