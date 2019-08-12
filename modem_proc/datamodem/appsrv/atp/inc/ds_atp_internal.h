/******************************************************************************
  @file    ds_atp_internal.h
  @brief   Internal header file for ATP(Application Traffic Paring)

  DESCRIPTION
  Internal header file for ATP(Application Traffic Paring)

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/atp/inc/ds_atp_internal.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/14   jz      Created
===========================================================================*/
#ifndef DS_ATP_INTERNAL_H
#define DS_ATP_INTERNAL_H

#include "datamodem_variation.h"

#ifdef FEATURE_DATA_ATP

#include "comdef.h"
#include "dssocket.h"
#include "ds_atp_api.h"
#include "ps_in.h"
#include "ps_sys_event.h"
#include "ps_sys.h"


#define DS_ATP_CFG_MAX_URL_LEN 255
#define DS_ATP_PCO_STRING_MAX_LEN 254
#define DS_ATP_MAX_IPV4_STRING_LEN  15
#define DS_ATP_HTTP_URL_PREFIX "http://"
#define DS_ATP_HTTP_URL_PREFIX_LEN 7
#define DS_ATP_HTTPS_URL_PREFIX "https://"
#define DS_ATP_HTTPS_URL_PREFIX_LEN 8
#define DS_ATP_DNS_MAX_RESULT 10
#define DS_ATP_MAX_PCO_CONTENT_LEN 252


typedef enum
{
  DS_ATP_MSG_SOCK_PROTOCOL_UDP,
  DS_ATP_MSG_SOCK_PROTOCOL_TCP,
  DS_ATP_MSG_SOCK_PROTOCOL_MAX
}ds_atp_sock_protocol_enum_type;

typedef enum
{
  DS_ATP_MSG_ID_MIN,
  DS_ATP_POLICY_PULLED_IND = DS_ATP_MSG_ID_MIN,
  DS_ATP_POLICY_REFRESH_TIMER_EXPIRES,
  DS_ATP_POLICY_RETRY_TIMER_EXPIRES,
  DS_ATP_POLICY_PUSH_IND,
  DS_ATP_3GPP_FULL_SRV_IND,
  DS_ATP_3GPP_OOS_IND,
  DS_ATP_ACTIVATION_IND,
  DS_ATP_DEACTIVATION_IND,
  DS_ATP_OP_PCO_SET_IND,
  DS_ATP_ROAMING_IND,
  DS_ATP_EXIT_ROAMING_IND,

  DS_ATP_NET_IF_UP_IND,
  DS_ATP_NET_IF_DOWN_IND,
  DS_ATP_DNS_RESULT_IND,

  /*MSGs for filter report*/
  DS_ATP_FILTER_COUNT_TIMER_EXPIRE_IND,
  DS_ATP_FILTER_REPORT_REQ,
  DS_ATP_FILTER_REPORT_SERVER_UPDATED_IND,

  /*MSGs for Net Interface*/
  DS_ATP_PS_SYS_CB_IND,
  DS_ATP_DSS_NET_CB_IND,
  DS_ATP_DSS_SOCK_CB_IND,

  DS_ATP_MSG_ID_MAX
}ds_atp_msg_id_enum_type;

typedef enum
{
  DS_ATP_CFG_TOKEN_ID_MIN,
  ATP_ACTIVATED = DS_ATP_CFG_TOKEN_ID_MIN,
  APN_NAME,
  PDN_PROFILE_ID,
  ATP_PCO_CONTAINER_ID,
  ATP_PCO_STRING,
  POLICY_SERVER_DEFAULT_URL,
  POLICY_DEFAULT_SERVER_FQDN,
  POLICY_SERVER_FQDN,
  POLICY_PULL_SOURCE_PORT,
  POLICY_PULL_DEST_PORT,
  POLICY_PUSH_SOURCE_PORT,
  POLICY_PUSH_DEST_PORT,
  POLICY_MAX_RETRY,
  POLICY_RETRY_TIME_INTERVAL,
  POLICY_REFRESH_TIME_INTERVAL,
  POLICY_DEFAULT_REFRESH_TIME_INTERVAL,
  FILTER_REPORT_SERVER_FQDN,
  FILTER_REPORT_SERVER_IP,
  FILTER_REPORT_SOURCE_PORT,
  FILTER_REPORT_DEST_PORT,
  FILTER_REPORT_TIMEOUT_VAL,
  FILTER_REPORT_MAX_NUM_RETX,
  ATP_SMS_DEST_PORT,
  DS_ATP_CFG_TOKEN_ID_MAX
}ds_atp_cfg_token_id_enum_type;

typedef struct
{
  uint8      atp_activated;
  char       apn[DS_ATP_MAX_APN_NAME_LEN+1];
  uint16     pdn_profile_id;
  uint16     atp_pco_container_id;
  char       atp_pco_string[DS_ATP_PCO_STRING_MAX_LEN+1];
  char       policy_server_default_url[DS_ATP_CFG_MAX_URL_LEN+1];
  char       policy_default_fqdn[DS_ATP_CFG_MAX_URL_LEN+1];
  char       policy_fqdn[DS_ATP_CFG_MAX_URL_LEN+1];
  uint16     policy_pull_src_port;
  uint16     policy_pull_dest_port;
  uint16     policy_push_src_port;
  uint16     policy_push_dest_port;
  uint8      policy_pull_max_retry;
  uint32     policy_pull_retry_time_interval;//in second
  uint32     policy_refresh_time_interval; //in sencond
  uint32     policy_default_fresh_time_interval; //in second
  char       filter_report_server_fqdn[DS_ATP_CFG_MAX_URL_LEN+1];
  char       filter_report_server_ip[DS_ATP_MAX_IPV4_STRING_LEN+1];
  uint16     filter_report_src_port;
  uint16     filter_report_dest_port;
  uint32     filter_report_timeout; //in milisecond
  uint8      filter_report_max_retx;
  uint16     atp_sms_dest_port;
}ds_atp_cfg_param_type;


typedef struct
{
  ps_sys_tech_enum_type     tech_type;
  ps_sys_event_enum_type    event_name;
  void                      *event_info_ptr;
}ds_atp_ps_sys_cb_event_type;


typedef struct
{
  sint15            nethandle;
  sint15            err;
  dss_iface_id_type iface_fd;
}ds_atp_dss_net_cb_event_type;

typedef struct
{
  sint15 nethandle;
  sint15 sockfd;
  uint32 event_mask;
}ds_atp_sock_event_type;

typedef struct
{
  boolean is_succ;
  uint8   *content_ptr;
  uint32  content_len;
}ds_atp_policy_pulled_event_type;

typedef struct
{
  ds_atp_policy_info_type policy_info;
  char policy_server_fqdn[DS_ATP_CFG_MAX_URL_LEN+1];
  char report_server_fqdn[DS_ATP_CFG_MAX_URL_LEN+1];
  uint32 policy_refresh_time;
}ds_atp_policy_info_internal_type;

typedef struct
{
  char   host_name[DS_ATP_MAX_IPV4_STRING_LEN+1];
  uint32 num_addr;
  struct ps_in_addr ip_addr[DS_ATP_DNS_MAX_RESULT];
  void   *user_data_ptr;
}ds_atp_dns_result_type;



typedef struct
{
   uint16  pco_container_id;
   uint8   pco_container_content_len;
   char    pco_container_content[DS_ATP_MAX_PCO_CONTENT_LEN];
   uint8   policy_url_len;
   char    policy_url[DS_ATP_CFG_MAX_URL_LEN+1];
}ds_atp_activation_info_type;

typedef uint8 ds_atp_policy_push_duration_type;

typedef struct
{
  ds_atp_msg_id_enum_type atp_msg_id;
  union
  {
    ds_atp_ps_sys_cb_event_type  ps_sys_cb_content;
    ds_atp_dss_net_cb_event_type net_cb_content;
    ds_atp_sock_event_type  sock_event;
    ds_atp_filter_report_type filter;
    ds_atp_policy_pulled_event_type policy_pulled_event;
    ds_atp_dns_result_type dns_result;
    ds_atp_activation_info_type  activation_info;
    ds_atp_policy_push_duration_type push_duration;
  }msg_content;

}ds_atp_msg_type;

typedef enum
{
  DS_ATP_NET_IF_STATE_NONE,
  DS_ATP_NET_IF_STATE_DOWN = DS_ATP_NET_IF_STATE_NONE,
  DS_ATP_NET_IF_STATE_UP,
  DS_ATP_NET_IF_STATE_MAX
}ds_atp_net_if_state_enum_type;

typedef void (*ds_atp_net_sock_evt_hdlr)(ds_atp_sock_event_type *sock_read_evt_ptr);


#ifdef __cplusplus
extern "C"
{
#endif


void ds_atp_event_notification(ds_atp_proxy_event_enum_type  event, void *event_info_ptr);

void ds_atp_sms_init(void);

/*===========================================================================
FUNCTION DS_ATP_POLICY_MGR_INIT()

DESCRIPTION
To initiate ATP policy manager

PARAMETERS

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_atp_policy_mgr_init(void);

/*===========================================================================
FUNCTION DS_ATP_NET_IF_BRINGUP()

DESCRIPTION
To initiate ATP net interface

PARAMETERS

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_atp_net_if_bringup(void);
ds_atp_net_if_state_enum_type ds_atp_net_get_netif_state(void);

/*===========================================================================
FUNCTION DS_ATP_COMMON_POST_MSG()

DESCRIPTION
POST ATP message to DS task

PARAMETERS

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_atp_msg_post(ds_atp_msg_id_enum_type msg_id, void *msg_content_ptr);

/*===========================================================================
FUNCTION DS_ATP_POLICY_MSG_HDLR

DESCRIPTION
ATP Policy manager message handler
PARAMETERS

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_atp_policy_msg_hdlr(ds_atp_msg_type  *msg_ptr);
uint32 ds_atp_policy_mgr_get_curr_fsm_state(void);
boolean ds_atp_policy_get_policy_info(ds_atp_policy_info_type *policy_ptr);
uint16 ds_atp_policy_get_matched_app_by_name(char *app_name_ptr);

/*===========================================================================
FUNCTION DS_ATP_NET_IF_INIT()

DESCRIPTION
To initiate ATP net interface

PARAMETERS

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_atp_net_if_init(void);

/*===========================================================================
FUNCTION DS_ATP_NET_CREATE_SOCKET()

DESCRIPTION
To create a socket

PARAMETERS
protocol - UDP/TCP
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 ds_atp_net_create_socket
(
  ds_atp_sock_protocol_enum_type protocol,
  ds_atp_net_sock_evt_hdlr sock_evt_hdlr
);
/*===========================================================================
FUNCTION DS_ATP_NET_CLOSE_SOCKET()

DESCRIPTION
To close a socket

PARAMETERS
protocol - UDP/TCP
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_atp_net_close_socket(sint15 sock_fd);
/*===========================================================================
FUNCTION DS_ATP_NET_SOCK_BIND()

DESCRIPTION
To bind a socket to specified port

PARAMETERS
sock_fd - socket handle
port - port number
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_atp_net_sock_bind(sint15 sock_fd, uint16 port);

/*===========================================================================
FUNCTION DS_ATP_NET_ASYNC_SELECT()

DESCRIPTION
to enable events to be notified asynchronously.

PARAMETERS
sock_fd - socket handle
event_mask - event mask
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_atp_net_async_select(sint15 sock_fd, sint31 event_mask, uint16 dest_port);
/*===========================================================================
FUNCTION DS_ATP_FILTER_INIT()

DESCRIPTION
Initialization for filter report manager

PARAMETERS

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_atp_filter_init(void);

void ds_atp_filter_msg_hdlr(ds_atp_msg_type  *msg_ptr);
void ds_atp_filter_send_report(ds_atp_filter_report_type *filter_ptr);

void ds_atp_net_msg_hdlr(ds_atp_msg_type  *msg_ptr);

boolean ds_atp_net_sendto(
                          sint15             sock_fd,
                          struct ps_sockaddr *dest_ptr,
                          uint8              *pkt_ptr,
                          uint16             pkt_len
                          );
uint32 ds_atp_net_recvfrom(
                           sint15                sock_fd,
                           uint8                 *buf_ptr,
                           uint16                buf_len,
                           struct ps_sockaddr_in *dest_ptr
                           );
boolean ds_atp_net_get_ip_by_hostname(char *host_name_ptr, void *user_data_ptr);
ps_sys_rat_ex_enum_type ds_atp_net_get_rat(void);
uint64 ds_atp_net_get_so(void);

void ds_atp_cfg_init(void);
void ds_atp_cfg_update_efs_from_cache(void);
void ds_atp_cfg_read_from_efs(void);

boolean ds_atp_cfg_set_param(
                             ds_atp_cfg_token_id_enum_type token_id,
                             void                          *param_ptr,
                             uint32                        param_len
                             );
char* ds_atp_cfg_get_report_server_fqdn(void);
char* ds_atp_cfg_get_filter_server_ip(void);
uint16 ds_atp_cfg_get_filter_server_port(void);
uint16 ds_atp_cfg_get_filter_source_port(void);
uint32 ds_atp_cfg_get_filter_report_timeout(void);
uint8 ds_atp_cfg_get_filter_max_retx(void);
char* ds_atp_cfg_get_policy_server_default_fqdn(void);
char* ds_atp_cfg_get_policy_server_fqdn(void);
uint16 ds_atp_cfg_get_policy_pull_src_port(void);
uint16 ds_atp_cfg_get_policy_pull_dest_port(void);
uint16 ds_atp_cfg_get_policy_push_src_port(void);
uint16 ds_atp_cfg_get_policy_push_dest_port(void);
uint8 ds_atp_cfg_get_policy_max_retry(void);
uint32 ds_atp_cfg_get_policy_retry_interv(void);
uint32 ds_atp_cfg_get_policy_refresh_interv(void);
uint32 ds_atp_cfg_get_policy_default_refresh_interv(void);
char* ds_atp_cfg_get_apn_name(void);
uint16 ds_atp_cfg_get_pdn_profile_id(void);
uint8 ds_atp_cfg_get_atp_activation_status(void);
uint16 ds_atp_cfg_get_atp_sms_dest_port(void);
void ds_atp_set_ext_activation_status(uint8 activation_status);
uint16 ds_atp_cfg_get_atp_pco_id(void);
char* ds_atp_cfg_get_atp_pco_string(void);

boolean ds_atp_is_in_roaming(void);


void ds_atp_msg_init(void);


boolean ds_atp_set_uplink_filter(uint16 src_port);


#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_ATP */
#endif /* DS_ATP_INTERNAL_H */

