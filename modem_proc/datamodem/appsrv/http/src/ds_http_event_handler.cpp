/*==============================================================================

                              ds_http_event_handler.cpp

GENERAL DESCRIPTION
  HTTP event handler

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/20/15    ml     Use memory from modem heap
04/15/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_event_handler.h"

#include "ds_http_data_mgr.h"
#include "ds_http_msg.h"
#include "ds_http_dsnet_iface_info.h"
#include "ds_http_utility.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"
#include "ps_sys.h"
#include "ps_sys_event.h"

/*==============================================================================
 *                          DS_Task msg structs
 *============================================================================*/
struct ds_http_ps_sys_status
{
  bool                        service_status[DS_HTTP_IFACE_MAX];
  ds_http_subscription_e_type subs_id;

  ds_http_ps_sys_status(bool status_3gpp, bool status_iwlan, bool status_wlan_lb, ps_sys_subscription_enum_type ps_subs_id)
  {
    init();
    subs_id                               = ps_sys_subs_id_to_ds_http_subs_id(ps_subs_id);
    service_status[DS_HTTP_IFACE_3GPP]    = status_3gpp;
    service_status[DS_HTTP_IFACE_IWLAN]   = status_iwlan;
    service_status[DS_HTTP_IFACE_WLAN_LB] = status_wlan_lb;
    service_status[DS_HTTP_IFACE_ANY]     = (status_3gpp || status_iwlan);
  }

  void init()
  {
    for(uint32 i = 0; i < DS_HTTP_IFACE_MAX; ++i)
    {
      service_status[i] = false;
    }
  }
};


struct ds_http_dsnet_msg
{
  const sint15            nethandle;
  const dss_iface_id_type iface_id;
  const sint15            error;

  ds_http_dsnet_msg(sint15 nhandle, dss_iface_id_type iface, sint15 err)
  : nethandle(nhandle), iface_id(iface), error(err) { }
};


struct ds_http_socket_msg
{
  const sint15 nethandle;
  const sint15 sockfd;
  const uint32 event_mask;

  ds_http_socket_msg(sint15 nhandle, sint15 sfd, uint16 emask)
  : nethandle(nhandle), sockfd(sfd), event_mask(emask) { }
};


struct ds_http_dns_msg
{
  const dss_dns_session_mgr_handle_type session_handle;
  const dss_dns_query_handle_type       query_handle;
  const uint16                          num_records;
  const int16                           dss_errno;

  ds_http_dns_msg(
                  dss_dns_session_mgr_handle_type shandle,
                  dss_dns_query_handle_type       qhandle,
                  uint16                          num_rec,
                  int16                           err
                  )
  : session_handle(shandle), query_handle(qhandle), num_records(num_rec), dss_errno(err) { }
};



/*==============================================================================
 *                           Static msg handlers
 *============================================================================*/
static void ds_http_ps_sys_handler(uint32* data_ptr);
static void ds_http_ps_sys_handler_helper(ds_http_ps_sys_status* sys_status);
static void ds_dsnet_msg_handler(uint32* data_ptr);
static void ds_http_socket_msg_handler(uint32* data_ptr);
static void ds_dns_msg_handler(uint32* data_ptr);
static void ds_http_socket_timeout_msg_handler(uint32* data_ptr);
static void ds_http_request_msg_handler(uint32* data_ptr);

static void ds_http_ps_sys_cb(
                              ps_sys_tech_enum_type         tech_type,
                              ps_sys_event_enum_type        event_name,
                              ps_sys_subscription_enum_type subs_id,
                              void*                         event_info_ptr,
                              void*                         user_data_ptr
                              );




void ds_http_init_event_handler()
{
  int16 ps_errno = 0;

  ps_sys_event_reg_ex(
                      PS_SYS_TECH_ALL,
                      PS_SYS_EVENT_SYSTEM_STATUS_EX,
                      PS_SYS_PRIMARY_SUBS,
                      &ds_http_ps_sys_cb,
                      NULL,
                      &ps_errno
                      );
  ps_sys_event_reg_ex(
                      PS_SYS_TECH_ALL,
                      PS_SYS_EVENT_SYSTEM_STATUS_EX,
                      PS_SYS_SECONDARY_SUBS,
                      &ds_http_ps_sys_cb,
                      NULL,
                      &ps_errno
                      );
  ps_sys_event_reg_ex(
                      PS_SYS_TECH_ALL,
                      PS_SYS_EVENT_SYSTEM_STATUS_EX,
                      PS_SYS_TERTIARY_SUBS,
                      &ds_http_ps_sys_cb,
                      NULL,
                      &ps_errno
                      );
}


/*==============================================================================
                              System Status
==============================================================================*/
void ds_http_ps_sys_cb(
                       ps_sys_tech_enum_type         tech_type,
                       ps_sys_event_enum_type        event_name,
                       ps_sys_subscription_enum_type subs_id,
                       void*                         event_info_ptr,
                       void*                         user_data_ptr
                       )
{
  ps_sys_system_status_ex_type* sys_info          = NULL;
  ds_http_ps_sys_status*        sys_status        = NULL;
  ps_sys_rat_ex_enum_type       rat               = PS_SYS_RAT_EX_NULL_BEARER;
  uint64                        so                = PS_SYS_SO_EX_UNSPECIFIED;
  ps_sys_network_enum_type      preferred_network = PS_SYS_NETWORK_MAX;

  bool wwan_in_service    = false;
  bool iwlan_in_service   = false;
  bool wlan_lb_in_service = false;


  DATA_APPSRV_MSG2(MSG_LEGACY_LOW, "ds_http_ps_sys_cb - subs:%d event:%d", subs_id, event_name);

  if (event_info_ptr == NULL || PS_SYS_EVENT_SYSTEM_STATUS_EX != event_name)
  {
    DATA_APPSRV_MSG2(
                     MSG_LEGACY_ERROR,
                     "ds_http_ps_sys_cb - null event_info_ptr %d or unhandled event %d",
                     event_info_ptr,
                     event_name
                     );
    return;
  }

  sys_info = (ps_sys_system_status_ex_type*)event_info_ptr;

  DATA_APPSRV_MSG4(
                   MSG_LEGACY_LOW,
                   "ds_http_ps_sys_cb - subs:%d event:%d num_apns:%d num_sys:%d",
                   subs_id,
                   event_name,
                   sys_info->num_avail_apns,
                   sys_info->num_avail_sys
                   );

  for(uint32 i = 0; i < sys_info->num_avail_sys; ++i)
  {
    preferred_network = sys_info->avail_sys[i].technology;
    rat               = sys_info->avail_sys[i].rat_value;
    so                = sys_info->avail_sys[i].so_mask;

    DATA_APPSRV_MSG4(MSG_LEGACY_HIGH, "ds_http_ps_sys_cb - [%d] pref:%d rat:%d so:%d", i, preferred_network, rat, so);
    if(PS_SYS_NETWORK_3GPP == preferred_network)
    {
      if(rat == PS_SYS_RAT_EX_3GPP_WCDMA  ||
         rat == PS_SYS_RAT_EX_3GPP_GERAN  ||
         rat == PS_SYS_RAT_EX_3GPP_LTE    ||
         rat == PS_SYS_RAT_EX_3GPP_TDSCDMA)
      {
        wwan_in_service = true;
      }
      if(rat == PS_SYS_RAT_EX_3GPP_WLAN)
      {
        iwlan_in_service = true;
      }
    }
    else if(PS_SYS_NETWORK_WLAN == preferred_network)
    {
      if(rat == PS_SYS_RAT_EX_WLAN)
      {
        wlan_lb_in_service = true;
      }
    }
  }

  sys_status = new(ds_appsrv_alloc<ds_http_ps_sys_status>(), APPSRV_MEM) ds_http_ps_sys_status(
                                                                                               wwan_in_service,
                                                                                               iwlan_in_service,
                                                                                               wlan_lb_in_service,
                                                                                               subs_id
                                                                                               );

  if(NULL == sys_status)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_ps_sys_cb - Failed to allocate memory for ds_http_ps_sys_status");
    return;
  }

  if(!ds_http_msg_post(&ds_http_ps_sys_handler, (uint32*)sys_status))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_ps_sys_cb - ds_task failed");
    ds_appsrv_free(sys_status);
  }
}



/*==============================================================================
 *                            Callback functions
 *============================================================================*/
void ds_http_net_cb(
                    sint15            nethandle,
                    dss_iface_id_type iface_id,
                    sint15            err,
                    void*             net_cb_user_data
                    )
{
  ds_http_dsnet_msg* dsnet_msg
    = new(ds_appsrv_alloc<ds_http_dsnet_msg>(), APPSRV_MEM) ds_http_dsnet_msg(nethandle, iface_id, err);

  if(NULL == dsnet_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_net_cb - Failed to allocate memory for dsnet_msg");
    return;
  }

  if(!ds_http_msg_post(&ds_dsnet_msg_handler, (uint32*)dsnet_msg))
    ds_appsrv_free(dsnet_msg);
}


void ds_http_sock_cb(
                     sint15 nethandle,
                     sint15 sockfd,
                     uint32 event_mask,
                     void*  sock_cb_user_data
                     )
{
  ds_http_socket_msg* socket_msg
    = new(ds_appsrv_alloc<ds_http_socket_msg>(), APPSRV_MEM) ds_http_socket_msg(nethandle, sockfd, event_mask);

  if(NULL == socket_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_sock_cb - Failed to allocate memory for socket_msg");
    return;
  }

  if(!ds_http_msg_post(&ds_http_socket_msg_handler, (uint32*)socket_msg))
    ds_appsrv_free(socket_msg);
}


void ds_http_dns_cb(
                    dss_dns_session_mgr_handle_type session_handle,
                    dss_dns_query_handle_type       query_handle,
                    dss_dns_api_type_enum_type      api_type,
                    uint16                          num_records,
                    void*                           user_data_ptr,
                    int16                           dss_errno
                    )
{
  ds_http_dns_msg* dns_msg = new(ds_appsrv_alloc<ds_http_dns_msg>(), APPSRV_MEM) ds_http_dns_msg(
                                                                                                 session_handle,
                                                                                                 query_handle,
                                                                                                 num_records,
                                                                                                 dss_errno
                                                                                                 );

  if(NULL == dns_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_dns_cb - Failed to allocate memory for dns_msg");
    return;
  }

  if(!ds_http_msg_post(&ds_dns_msg_handler, (uint32*)dns_msg))
    ds_appsrv_free(dns_msg);
}


void ds_http_timeout_timer_cb(uint32 user_data)
{
  uint32* timeout_msg = ds_appsrv_alloc<uint32>();

  if(NULL == timeout_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_timeout_timer_cb - Failed to allocate memory for timeout_msg");
    return;
  }
  *timeout_msg = user_data;
  if(!ds_http_msg_post(&ds_http_socket_timeout_msg_handler, (uint32*)timeout_msg))
    ds_appsrv_free(timeout_msg);
}


bool ds_http_add_new_request(ds_http_request* request)
{
  if(!ds_http_msg_post(&ds_http_request_msg_handler, (uint32*)request))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_add_new_request - ds_task failed");
    return false;
  }

  return true;
}



/*==============================================================================
 *                            Callback msg handlers
 *============================================================================*/
void ds_http_ps_sys_handler(uint32* data_ptr)
{
  ds_http_ps_sys_status* sys_status = (ds_http_ps_sys_status*)data_ptr;

  if(NULL == sys_status || DS_HTTP_SUBS_MAX <= sys_status->subs_id)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_ps_sys_handler - NULL sys_status %d or invalid subs_id", sys_status);
    return;
  }

  ds_http_ps_sys_handler_helper(sys_status);
  if(ds_http_get_default_subscription() == sys_status->subs_id)
  {
    // Update and notify default subs
    sys_status->subs_id = DS_HTTP_DEFAULT_SUBS;
    ds_http_ps_sys_handler_helper(sys_status);
  }

  ds_appsrv_free(sys_status);
}


void ds_http_ps_sys_handler_helper(ds_http_ps_sys_status* sys_status)
{
  for(uint32 i = 0; i < DS_HTTP_IFACE_MAX; ++i)
  {
    ds_http_iface_e_type iface = static_cast<ds_http_iface_e_type>(i);

    if(sys_status->service_status[i] != ds_http_get_network_status(iface, sys_status->subs_id))
    {
      ds_http_set_network_status(iface, sys_status->subs_id, sys_status->service_status[i]);

      DATA_APPSRV_MSG3(
                       MSG_LEGACY_HIGH,
                       "ds_http_ps_sys_handler - iface %d subs_id %d service changed to %d",
                       iface,
                       sys_status->subs_id,
                       sys_status->service_status[i]
                       );

      // false -> true
      if(sys_status->service_status[i])
      {
        ds_http_notify_http_ready(iface, sys_status->subs_id);
      }
    }
  }
}


void ds_http_request_msg_handler(uint32* data_ptr)
{
  ds_http_request* request = (ds_http_request*) data_ptr;
  if(NULL == request)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_request_msg_handler - Invalid / NULL data_ptr");
    return;
  }

  ds_http_add_request_to_dsnet(request);
}


void ds_dsnet_msg_handler(uint32* data_ptr)
{
  ds_http_dsnet_msg* dsnet_msg = (ds_http_dsnet_msg*) data_ptr;
  if (NULL == dsnet_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_dsnet_msg_handler - illegal dsnet_msg");
    return;
  }

  ds_http_handle_dsnet_event(dsnet_msg->nethandle, dsnet_msg->iface_id, dsnet_msg->error);

  ds_appsrv_free(dsnet_msg);
}



void ds_http_socket_msg_handler(uint32* data_ptr)
{
  ds_http_socket_msg* socket_msg = (ds_http_socket_msg*) data_ptr;
  if (NULL == socket_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_socket_msg_handler - illegal socket_msg");
    return;
  }

  ds_http_handle_socket_event(socket_msg->nethandle, socket_msg->sockfd, socket_msg->event_mask);

  ds_appsrv_free(socket_msg);
}



void ds_dns_msg_handler(uint32* data_ptr)
{
  ds_http_dns_msg* dns_msg = (ds_http_dns_msg*) data_ptr;
  if (NULL == dns_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_dns_msg_handler, illegal dns_msg");
    return;
  }

  ds_http_handle_dns_event(
                           dns_msg->session_handle,
                           dns_msg->query_handle,
                           dns_msg->num_records,
                           dns_msg->dss_errno
                           );

  ds_appsrv_free(dns_msg);
}



void ds_http_socket_timeout_msg_handler(uint32* timeout_msg)
{
  if (NULL == timeout_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_socket_msg_handler, illegal timeout_msg");
    return;
  }

  sint15 nethandle = (0xFFFF & (*timeout_msg >> 16));
  uint16 socket_id = (0xFFFF & *timeout_msg);
  DATA_APPSRV_MSG2(MSG_LEGACY_HIGH, "ds_http_socket_timeout_msg_handler - %d, %d", nethandle, socket_id);

  ds_http_handle_timeout_event(nethandle, socket_id);

  ds_appsrv_free(timeout_msg);
}


