/*==============================================================================

                              ds_http_data_manager.cpp

GENERAL DESCRIPTION
  Internal data manager for session and dsnet

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/21/16    ml     Query API for PDN bringup failure reason
02/20/16    ml     Session option to handle response size > 100k
02/20/16    ml     Multiple SSL version support
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/29/15    ml     Resolve profile id if it is default before attempting bringup
09/11/15    ml     DSDS support
06/11/15    ml     Cookie support
05/20/15    ml     Use memory from modem heap
04/20/15    ml     Session deregistration
04/10/15    ml     Iface any support
03/10/15    ml     IWLAN / WLAN LB Support
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_data_mgr.h"

#include "ds_http_dsnet.h"
#include "ds_http_request.h"
#include "ds_http_session_info.h"
/* Init */
#include "ds_http_msg.h"
#include "ds_ssl_socket.h"
#include "ds_http_event_handler.h"

#include "ds_ASMutex.h"
#include "ds_ASVector.h"
#include "ds_ASList.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"
#include "rex.h"

using Appsrv::Utils::ASMutex;
using Appsrv::Utils::ASLockGuard;



/*==============================================================================
                            Static variables
==============================================================================*/
/*  Note: If both session and dsnet must be locked, lock order should ALWAYS be
    dsnet->session.

    Cases for when to lock both:
      session creation
      dsnet management through session
*/
static ASMutex dsnet_crit_sect;
static ASMutex session_crit_sect;
static ASMutex net_status_crit_sect;

static ASList<ds_http_session_info*>  session_list;
static ASList<ds_http_dsnet*>         dsnet_list;

static bool network_status[DS_HTTP_SUBS_MAX][DS_HTTP_IFACE_MAX];

typedef ASList<ds_http_session_info*>::iterator       session_iterator;
typedef ASList<ds_http_session_info*>::const_iterator session_const_iterator;
typedef ASList<ds_http_dsnet*>::iterator              dsnet_iterator;
typedef ASList<ds_http_dsnet*>::const_iterator        dsnet_const_iterator;


/*==============================================================================
                            Static functions
==============================================================================*/
static void ds_http_notify_block_cb(
                                    const uint32                   session_id,
                                    const uint32                   request_id,
                                    const ds_http_block_event_type event,
                                    ds_http_block_event_info_type* event_info
                                    );

static void ds_http_event_remove_dsnet();
static void ds_http_remove_dsnet(const ds_http_dsnet* dsnet);
static void ds_http_remove_dsnet_event_handler(uint32* msg);
static void ds_http_session_cleanup_handler(uint32* data_ptr);

static ds_http_session_info* get_session_info(const uint32 session_id);
static ds_http_dsnet* ds_http_get_dsnet_by_session_id(const uint32 session_id);
static ds_http_dsnet* ds_http_get_dsnet_by_dsnet_handle(const sint15 dsnet_handle_id);

static void ds_http_lock_session_config(uint32 session_id);


/*==============================================================================
                                HTTP Init
==============================================================================*/
void ds_http_init()
{
  ds_http_init_msg();
  ds_http_init_data_mgr();
  ds_http_init_event_handler();
  ds_http_init_ssl_session();
}


void ds_http_init_data_mgr()
{
  for(uint32 i = 0; i < DS_HTTP_SUBS_MAX; ++i)
  {
    for(uint32 j = 0; j < DS_HTTP_IFACE_MAX; ++j)
    {
      network_status[i][j] = false;
    }
  }
}



/*==============================================================================
                              Network status mgr
==============================================================================*/
void ds_http_set_network_status(
                                ds_http_iface_e_type        iface,
                                ds_http_subscription_e_type subs_id,
                                bool                        net_status
                                )
{
  ASLockGuard lock(net_status_crit_sect);
  network_status[subs_id][iface] = net_status;
}


bool ds_http_get_network_status(
                                ds_http_iface_e_type        iface,
                                ds_http_subscription_e_type subs_id
                                )
{
  ASLockGuard lock(net_status_crit_sect);
  return network_status[subs_id][iface];
}




/*==============================================================================
                              Sessions mgr
==============================================================================*/
uint32 ds_http_create_session(
                              ds_http_cb_fcn              http_cb_fcn,
                              ds_http_cb_fcn_ex           http_cb_fcn_ex,
                              ds_http_block_event_cb_fcn  block_cb_fcn,
                              ds_http_dsnet_iface_info&   iface_info
                              )
{
  if( DS_HTTP_IFACE_MAX == iface_info.iface_type ||
      DS_HTTP_SUBS_MAX  <= iface_info.subs_id    ||
      NULL  == block_cb_fcn                      ||
      (NULL == http_cb_fcn && NULL == http_cb_fcn_ex) )
  {
    DATA_APPSRV_MSG5(
                     MSG_LEGACY_ERROR,
                     "ds_http_create_session - Invalid iface or fcn parameters iface:%d subs:%d fcn:(%x %x %x)",
                     iface_info.iface_type,
                     iface_info.subs_id,
                     http_cb_fcn,
                     http_cb_fcn_ex,
                     block_cb_fcn
                     );
    return DS_HTTP_ERROR;
  }

  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = NULL;

  for(dsnet_iterator it = dsnet_list.begin(); it != dsnet_list.end(); ++it)
  {
    if((*it)->iface_info == iface_info)
    {
      http_dsnet = *it;
    }
  }


  if(NULL == http_dsnet)
  {
    http_dsnet = new(ds_appsrv_alloc<ds_http_dsnet>(), APPSRV_MEM) ds_http_dsnet(iface_info);

    if(NULL == http_dsnet)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_create_session - Failed to allocate memory for http dsnet");
      return DS_HTTP_ERROR;
    }
    else if(!dsnet_list.push_back(http_dsnet))
    {
      ds_appsrv_free(http_dsnet);
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_create_session - Failed to allocate memory for dsnet list");
      return DS_HTTP_ERROR;
    }

    DATA_APPSRV_MSG6(
                     MSG_LEGACY_HIGH,
                     "ds_http_create_session - [Input iface:%d profile:%d subs:%d] [Session iface:%d profile:%d subs:%d]",
                     iface_info.iface_type,
                     iface_info.profile_id,
                     iface_info.subs_id,
                     http_dsnet->iface_info.iface_type,
                     http_dsnet->iface_info.profile_id,
                     http_dsnet->iface_info.subs_id
                     );
  }


  { // Scoping to guarantee session_crit_sect to be released first
    ASLockGuard session_lock(session_crit_sect);

    ds_http_session_info* new_session
      = new(ds_appsrv_alloc<ds_http_session_info>(), APPSRV_MEM) ds_http_session_info(
                                                                                      http_cb_fcn,
                                                                                      http_cb_fcn_ex,
                                                                                      block_cb_fcn,
                                                                                      http_dsnet
                                                                                      );

    if(NULL == new_session)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_create_session - Failed to allocate memory for session");
      return DS_HTTP_ERROR;
    }

    // add to session list
    DATA_APPSRV_MSG0(MSG_LEGACY_MED, "ds_http_create_session - Created session");

    if(!session_list.push_back(new_session))
    {
      ds_appsrv_free(new_session);

      if(http_dsnet->dsnet_inactive())
      {
        ds_http_event_remove_dsnet();
      }

      return DS_HTTP_ERROR;
    }

    return new_session->get_session_id();
  }
}



void ds_http_remove_session(const uint32 session_id)
{
  DATA_APPSRV_MSG1(MSG_LEGACY_HIGH, "ds_http_remove_session - Closing session %d",session_id);
  ASLockGuard session_lock(session_crit_sect);

  ds_http_session_info* session_info = get_session_info(session_id);

  if(NULL == session_info)
    return;

  // prevent cb fcns from being called
  session_info->clear_cb();


  // Add to ds_task for session cleanup
  uint32* session_id_msg = ds_appsrv_alloc<uint32>();
  if(NULL == session_id_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_remove_session - failed to alloc memory");
    return;
  }

  *session_id_msg = session_id;
  if(!ds_http_msg_post(&ds_http_session_cleanup_handler, (uint32*)session_id_msg))
  {
    ds_appsrv_free(session_id_msg);
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_remove_session - ds_task failed");
  }
}



void ds_http_lock_session_config(uint32 session_id)
{
  ASLockGuard lock(session_crit_sect);
  ds_http_session_info* session_info = get_session_info(session_id);

  if(NULL != session_info)
  {
    session_info->lock_config();
  }
}



bool ds_http_update_session_config(
                                   const uint32                         session_id,
                                   ds_http_session_config_option_e_type config_type,
                                   ds_http_session_config_value_u_type* config_info
                                   )
{
  if(NULL == config_info)
    return false;

  ASLockGuard session_lock(session_crit_sect);

  ds_http_session_info* session_info = get_session_info(session_id);

  if(NULL == session_info)
    return false;

  return session_info->update_config(config_type, config_info);
}



bool ds_http_get_session_config(
                                const uint32                         session_id,
                                ds_http_session_config_option_e_type config_type,
                                ds_http_session_config_value_u_type* config_info
                                )
{
  if(NULL == config_info)
    return false;

  ASLockGuard session_lock(session_crit_sect);

  ds_http_session_info* session_info = get_session_info(session_id);

  if(NULL == session_info)
    return false;

  session_info->get_config(config_type, config_info);
  return true;
}



void ds_http_session_cleanup_handler(uint32* data_ptr)
{
  if(NULL == data_ptr)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_session_cleanup_handler - NULL remove_req_msg");
    return;
  }

  uint32                session_id   = *data_ptr;
  ds_http_session_info* session_info = NULL;

  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "ds_http_session_cleanup_handler - Cleaning up session %d",session_id);
  session_crit_sect.lock();

  for(session_iterator it = session_list.begin(); it != session_list.end(); ++it)
  {
    if(session_id == (*it)->get_session_id())
    {
      session_info = *it;
      *it = NULL;
      session_list.erase(it);
      break;
    }
  }

  session_crit_sect.unlock();

  if(NULL == session_info)
    return;

  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = session_info->get_dsnet();
  ds_appsrv_free(session_info);

  if(NULL != http_dsnet && http_dsnet->dsnet_inactive())
  {
    ds_http_event_remove_dsnet();
  }

  ds_appsrv_free(data_ptr);
}



/* IMPORTANT!!!: This function should ONLY be called after entering critical section for session_list */
ds_http_session_info* get_session_info(const uint32 session_id)
{
  for(session_iterator it = session_list.begin(); it != session_list.end(); ++it)
  {
    if(session_id == (*it)->get_session_id())
    {
      return *it;
    }
  }

  return NULL;
}



// true if valid session, else false
bool ds_http_check_valid_session(const uint32 session_id)
{
  ASLockGuard lock(session_crit_sect);

  for(session_iterator it = session_list.begin(); it != session_list.end(); ++it)
  {
    if(session_id == (*it)->get_session_id())
    {
      return true;
    }
  }

  return false;
}



void ds_http_notify_http_ready(ds_http_iface_e_type iface, ds_http_subscription_e_type subs_id)
{
  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "ds_http_notify_http_ready - for iface:%d subs:%d", iface, subs_id);
  ASVector<ds_http_session_info> cb_fcn_list;

  // Get list of fcn to notify
  dsnet_crit_sect.lock();
  session_crit_sect.lock();
  for(session_iterator it = session_list.begin(); it != session_list.end(); ++it)
  {
    ds_http_session_info* session_info = *it;
    ds_http_dsnet*        dsnet_info   = session_info->get_dsnet();

    if(iface   == dsnet_info->iface_info.iface_type &&
       subs_id == dsnet_info->iface_info.subs_id    &&
       dsnet_info->net_down())
    {
      cb_fcn_list.push_back(ds_http_session_info(session_info));
    }
  }
  session_crit_sect.unlock();
  dsnet_crit_sect.unlock();

  for(uint32 i = 0; i < cb_fcn_list.size(); ++i)
  {
    cb_fcn_list[i].notify_http_cb(0, DS_HTTP_READY, NULL);
  }
}



void ds_http_notify_http_ready(const uint32 session_id)
{
  ds_http_notify_session_cb(session_id, 0, DS_HTTP_READY, NULL);
}


void ds_http_notify_session_cb(
                               const uint32                        session_id,
                               const uint32                        request_id,
                               const sint15                        error,
                               const ds_http_response_info_s_type* resp_info
                               )
{
  DATA_APPSRV_MSG3(
                   MSG_LEGACY_MED,
                   "ds_http_notify_session_cb - HTTP callback function triggered sess:%d req:%d err:%d",
                   session_id,
                   request_id,
                   error
                   );

  session_crit_sect.lock();
  ds_http_session_info session_info(get_session_info(session_id));
  session_crit_sect.unlock();

  session_info.notify_http_cb(request_id, error, resp_info);
}



void ds_http_notify_auth_cb(
                            const uint32                  session_id,
                            const uint32                  request_id,
                            ds_http_block_auth_info_type* auth_info
                            )
{
  ds_http_block_event_info_type event_info;
  event_info.auth_event_info = *auth_info;
  ds_http_notify_block_cb(session_id, request_id, DS_HTTP_BLOCK_AUTHENTICATION, &event_info);
}



void ds_http_notify_ssl_alert_cb(
                                 const uint32                  session_id,
                                 const uint32                  request_id,
                                 ds_http_ssl_cert_alert_e_type ssl_alert_info
                                 )
{
  ds_http_block_event_info_type event_info;
  event_info.ssl_event_info = ssl_alert_info;
  ds_http_notify_block_cb(session_id, request_id, DS_HTTP_BLOCK_SSL_CERTIFICATE_ALERT, &event_info);
}


void ds_http_notify_block_cb(
                             const uint32                   session_id,
                             const uint32                   request_id,
                             const ds_http_block_event_type event,
                             ds_http_block_event_info_type* event_info
                             )
{
  DATA_APPSRV_MSG3(
                   MSG_LEGACY_MED,
                   "ds_http_notify_block_cb - block callback function triggered for sess:%d req:%d event:%d",
                   session_id,
                   request_id,
                   event
                   );

  session_crit_sect.lock();
  ds_http_session_info session_info(get_session_info(session_id));
  session_crit_sect.unlock();

  session_info.notify_block_cb(request_id, event, event_info);
}




/*==============================================================================
                          ds_http_dsnet mgr
==============================================================================*/
// Caller must lock dsnet
void ds_http_remove_dsnet(const ds_http_dsnet* http_dsnet)
{
  for(dsnet_iterator it = dsnet_list.begin(); it != dsnet_list.end(); ++it)
  {
    if(http_dsnet == *it)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_HIGH, "ds_http_remove_dsnet - Removing dsnet");
      ds_appsrv_free(*it);
      dsnet_list.erase(it);
      return;
    }
  }
}


void ds_http_event_remove_dsnet()
{
  if( !ds_http_msg_post(&ds_http_remove_dsnet_event_handler, NULL) )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_event_remove_dsnet - ds_task failed");
  }
}



void ds_http_remove_dsnet_event_handler(uint32* msg)
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  for(dsnet_iterator it = dsnet_list.begin(); it != dsnet_list.end(); ++it)
  {
    if( (*it)->dsnet_inactive() )
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_HIGH, "ds_http_remove_dsnet_event_handler - Removing dsnet");
      ds_appsrv_free(*it);
      dsnet_list.erase(it);
      return;
    }
  }
}


ds_http_dsnet* ds_http_get_dsnet_by_session_id(const uint32 session_id)
{
  ASLockGuard lock(session_crit_sect);
  ds_http_session_info* session_info = get_session_info(session_id);
  if(NULL != session_info)
    return session_info->get_dsnet();

  return NULL;
}


// Caller must lock dsnet
ds_http_dsnet* ds_http_get_dsnet_by_dsnet_handle(const sint15 dsnet_handle_id)
{
  for(dsnet_iterator it = dsnet_list.begin(); it != dsnet_list.end(); ++it)
  {
    if((*it)->dsnet_handle_id == dsnet_handle_id)
    {
      return *it;
    }
  }

  return NULL;
}



bool ds_http_check_dsnet_status(uint32 session_id, sint15* request_errno)
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_session_id(session_id);

  if(NULL == http_dsnet)
  {
    *request_errno = DS_HTTP_ERROR_DSNET;
    return false;
  }

  if (!http_dsnet->get_sys_status() && http_dsnet->net_down())
  {
    *request_errno = DS_HTTP_ERROR_NETDOWN;
    return false;
  }
  return true;
}



void ds_http_add_request_to_dsnet(ds_http_request* request)
{
  // Disallow further changes to session configuration
  ds_http_lock_session_config(request->session_id);

  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_session_id(request->session_id);

  if(NULL == http_dsnet)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_add_request_to_dsnet - dsnet not found %d", request->session_id);
    request->notify_error(DS_HTTP_ERROR_DSNET);
    ds_appsrv_free(request);
    return;
  }

  http_dsnet->add_request(request);
}



void ds_http_remove_request(const uint32 session_id, const uint32 request_id)
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_session_id(session_id);

  if(NULL == http_dsnet)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_remove_request - dsnet not found %d", session_id);
    return;
  }

  DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "ds_http_remove_request - removing request %d", request_id);
  http_dsnet->remove_request(request_id);
}



void ds_http_resolve_auth(uint32 session_id, uint32 request_id, ds_http_auth_info_s_type* auth_info)
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_session_id(session_id);

  if(NULL == http_dsnet)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_resolve_auth - dsnet not found %d", session_id);
    return;
  }

  http_dsnet->resolve_auth(request_id, auth_info);

  if(http_dsnet->dsnet_inactive())
  {
    ds_http_event_remove_dsnet();
  }
}



void ds_http_resolve_ssl_suspension(uint32 session_id, uint32 request_id, bool ssl_cont)
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_session_id(session_id);

  if(NULL == http_dsnet)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_resolve_ssl_suspension - dsnet not found %d", session_id);
    return;
  }

  http_dsnet->resolve_ssl_suspension(request_id, ssl_cont);

  if(http_dsnet->dsnet_inactive())
  {
    ds_http_event_remove_dsnet();
  }
}



void ds_http_append_chunk_to_request(
                                     const uint32                   session_id,
                                     const uint32                   request_id,
                                     const ds_http_chunked_content& chunked_content
                                     )
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_session_id(session_id);

  if(NULL == http_dsnet)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_append_chunk_to_request - dsnet not found %d", session_id);
    return;
  }

  http_dsnet->append_chunk_to_request(request_id, chunked_content);

  if(http_dsnet->dsnet_inactive())
  {
    ds_http_event_remove_dsnet();
  }
}



void ds_http_handle_dsnet_event(
                                const sint15            nethandle,
                                const dss_iface_id_type iface_id,
                                const sint15            error
                                )
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_dsnet_handle(nethandle);
  if (NULL == http_dsnet)
    return;

  http_dsnet->handle_dsnet_event(iface_id, error);

  if(http_dsnet->dsnet_inactive())
  {
    ds_http_remove_dsnet(http_dsnet);
  }
}


void ds_http_handle_socket_event(
                                 const sint15 nethandle,
                                 const sint15 sockfd,
                                 const uint32 event_mask
                                 )
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_dsnet_handle(nethandle);
  if (NULL == http_dsnet)
    return;

  http_dsnet->handle_socket_event(sockfd, event_mask);

  if(http_dsnet->dsnet_inactive())
  {
    ds_http_remove_dsnet(http_dsnet);
  }
}


void ds_http_handle_dns_event(
                              const dss_dns_session_mgr_handle_type dns_session_handle,
                              const dss_dns_query_handle_type       query_handle,
                              const uint16                          num_records,
                              const int16                           dss_errno
                              )
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = NULL;

  for(dsnet_iterator it = dsnet_list.begin(); it != dsnet_list.end(); ++it)
  {
    if(dns_session_handle == (*it)->dns_session_handle)
    {
      http_dsnet = *it;
      break;
    }
  }

  if(NULL == http_dsnet)
    return;

  http_dsnet->handle_dns_event(query_handle, num_records, dss_errno);

  if(http_dsnet->dsnet_inactive())
  {
    ds_http_remove_dsnet(http_dsnet);
  }
}


void ds_http_handle_timeout_event(const sint15 nethandle, const uint16 socket_id)
{
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_dsnet_handle(nethandle);
  if(NULL == http_dsnet)
    return;

  http_dsnet->handle_timeout_event(socket_id);

  if(http_dsnet->dsnet_inactive())
  {
    ds_http_remove_dsnet(http_dsnet);
  }
}


uint32 ds_http_get_last_netdownreason(uint32 session_id, dss_net_down_reason_type *reason)
{
  if(NULL == reason)
    return DS_HTTP_ERROR;

  sint15      dss_errno = 0;
  ASLockGuard dsnet_lock(dsnet_crit_sect);

  ds_http_dsnet* http_dsnet = ds_http_get_dsnet_by_session_id(session_id);

  if( NULL == http_dsnet || DSS_ERROR == dss_last_netdownreason(http_dsnet->dsnet_handle_id, reason, &dss_errno) )
  {
    return DS_HTTP_ERROR;
  }
  return DS_HTTP_ERROR_NONE;
}
