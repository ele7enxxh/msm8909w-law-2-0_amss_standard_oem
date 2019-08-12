/*==============================================================================

                              ds_http_dsnet.cpp

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
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/29/15    ml     Resolve profile id if it is default before attempting bringup
09/22/15    ml     IPv6 support
05/20/15    ml     Use memory from modem heap
04/20/15    ml     Session deregistration
04/10/15    ml     Iface any support
03/10/15    ml     WLAN LB Support
02/19/15    ml     Redirect requests to a new socket if current socket is in close wait status.
01/22/15    ml     Fixed infinite loop occuring due to dsnet bringup.
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_dsnet.h"

#include "ds_http_data_mgr.h" // ds_http_get_network_status
#include "ds_http_dns.h"
#include "ds_http_event_handler.h"
#include "ds_http_msg.h"
#include "ds_http_request.h"
#include "ds_http_socket.h"
#include "ds_ssl_socket.h"

#include "ds_ASQueue.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"
#include <stringl/stringl.h>

#include "dssocket_defs.h"

extern "C"
{
#include "ds_profile.h"
#include "ds_profile_3gpp.h"
}

typedef ASList<ds_http_dns_query*>::iterator       dns_iterator;
typedef ASList<ds_http_dns_query*>::const_iterator dns_const_iterator;
typedef ASList<ds_http_socket*>::iterator          socket_iterator;
typedef ASList<ds_http_socket*>::const_iterator    socket_const_iterator;
typedef ASList<ds_http_request*>::iterator         request_iterator;
typedef ASList<ds_http_request*>::const_iterator   request_const_iterator;


/*==============================================================================
                            DS_Task msg structs
==============================================================================*/
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
                            Static FCN Declaration
==============================================================================*/
static void ds_http_set_policy_info(
                                    dss_net_policy_info_type*       policy,
                                    const ds_http_dsnet_iface_info& iface_info
                                    );

static int ds_http_resolve_ip_type(const ds_http_dsnet_iface_info& iface_info);


/*==============================================================================
                              ds_http_dsnet
==============================================================================*/
ds_http_dsnet::ds_http_dsnet(const ds_http_dsnet_iface_info& iface)
: iface_info(iface),
  dsnet_handle_id(DSS_ERROR),
  dns_session_handle(DSS_DNS_SESSION_MGR_INVALID_HANDLE),
  dsnet_status(DS_HTTP_NET_STATUS_NOHANDLE),
  cache_counter(0),
  active_query(NULL),
  reference_counter(0),
  ip_family(DSS_AF_INET)
{ }



ds_http_dsnet::~ds_http_dsnet()
{
  if(DSS_ERROR != dsnet_handle_id)
  {
    sint15 err = 0;
    dsnet_release_handle(dsnet_handle_id, &err);
  }

  if(DSS_DNS_SESSION_MGR_INVALID_HANDLE != dns_session_handle)
  {
    ds_http_close_dns_session();
  }
}


bool ds_http_dsnet::dsnet_inactive() const
{
  return (0    == reference_counter   &&
          NULL == active_query        &&
          0    == socket_list.size()  &&
          dns_wait_queue_list.empty() &&
          dsnet_wait_queue.empty());
}



bool ds_http_dsnet::get_sys_status() const
{
  return ds_http_get_network_status(iface_info.iface_type, iface_info.subs_id);
}


bool ds_http_dsnet::net_down() const
{
  return dsnet_status != DS_HTTP_NET_STATUS_UP;
}



/*==============================================================================
                          DSNet request handlers
==============================================================================*/
void ds_http_dsnet::add_request(ds_http_request* request)
{
  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "add_request - %d, %d", request->request_id, dsnet_status);
  switch(dsnet_status)
  {
    case DS_HTTP_NET_STATUS_NOHANDLE:
    {
      if(!ds_http_init_dsnet())
      {
        request->notify_error(DS_HTTP_ERROR_DSNET);
        ds_appsrv_free(request);
        break;
      }
      DATA_APPSRV_MSG0(MSG_LEGACY_MED, "add_request - dsnet handle created");
    } // fallthrough

    case DS_HTTP_NET_STATUS_DOWN:
    {
      if(!ds_http_connect_dsnet())
      {
        request->notify_error(DS_HTTP_ERROR_DSNET);
        ds_appsrv_free(request);
        break;
      }
      // if(DS_HTTP_NET_STATUS_UP == dsnet_status)
      // {
      //   handle_request(request);
      //   break;
      // }
    } // else fallthrough

    case DS_HTTP_NET_STATUS_UP_IN_PROGRESS: // fallthrough
    case DS_HTTP_NET_STATUS_DOWN_IN_PROGRESS:
    {
      dsnet_wait_queue.push_back(request);
    }
    break;

    case DS_HTTP_NET_STATUS_UP:
    {
      handle_request(request);
    }
    break;

    default:
      break;
  }
}



void ds_http_dsnet::remove_request(const uint32 request_id)
{
  // Look for request in bringup wait staus if network is not up.
  if(DS_HTTP_NET_STATUS_UP != dsnet_status)
  {
    request_iterator request_it = dsnet_wait_queue.begin();
    while(dsnet_wait_queue.end() != request_it)
    {
      if(request_id == (*request_it)->request_id)
      {
      // No need to notify client
        ds_appsrv_free(*request_it);
        dsnet_wait_queue.erase(request_it);
        return;
      }

      // else
      ++request_it;
    }
  }

  // Look for request in DNS query. If so, remove and finish.
  if(NULL != active_query && active_query->remove_request(request_id))
    return;

  dns_iterator dns_it = dns_wait_queue_list.begin();
  while(dns_wait_queue_list.end() != dns_it)
  {
    if((*dns_it)->remove_request(request_id))
      return;

    ++dns_it;
  }


  // Look for request in Socket query.
  socket_iterator socket_it = socket_list.begin();
  while(socket_list.end() != socket_it)
  {
    if((*socket_it)->remove_request(request_id))
      return;

    ++socket_it;
  }
}



void ds_http_dsnet::remove_requests_for_session(const uint32 session_id)
{
  request_iterator request_it = dsnet_wait_queue.begin();
  while(dsnet_wait_queue.end() != request_it)
  {
    if(session_id == (*request_it)->session_id)
    {
      // No need to notify client
      ds_appsrv_free(*request_it);
      request_it = dsnet_wait_queue.erase(request_it);
    }
    else
    {
      ++request_it;
    }
  }

  // Look for session requests in DNS query.
  if(NULL != active_query)
  {
    active_query->remove_requests_for_session(session_id);
  }

  for(dns_iterator dns_it = dns_wait_queue_list.begin(); dns_it != dns_wait_queue_list.end(); ++dns_it)
  {
    (*dns_it)->remove_requests_for_session(session_id);
  }


  // Look for session request in Socket query.
  for(socket_iterator socket_it = socket_list.begin(); socket_it != socket_list.end(); ++socket_it)
  {
    (*socket_it)->remove_requests_for_session(session_id);
  }
}


void ds_http_dsnet::append_chunk_to_request(
                                            const uint32                   request_id,
                                            const ds_http_chunked_content& chunked_content
                                            )
{
  // Look for request in bringup wait queue, it's possible if net start syncronously succeeded
  request_iterator request_it = dsnet_wait_queue.begin();
  while(dsnet_wait_queue.end() != request_it)
  {
    if(request_id == (*request_it)->request_id)
    {
      (*request_it)->append_chunked_content(chunked_content);
      return;
    }

    // else
    ++request_it;
  }

  // Look for request in DNS query.
  if(NULL != active_query && active_query->append_chunk_to_request(request_id, chunked_content))
    return;

  dns_iterator dns_it = dns_wait_queue_list.begin();
  while(dns_wait_queue_list.end() != dns_it)
  {
    if((*dns_it)->append_chunk_to_request(request_id, chunked_content))
      return;

    ++dns_it;
  }


  // Look for request in Socket query.
  socket_iterator socket_it = socket_list.begin();
  while(socket_list.end() != socket_it)
  {
    if((*socket_it)->append_chunk_to_request(request_id, chunked_content))
      return;

    ++socket_it;
  }

  DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "append_chunk_to_request - Request %d not found", request_id);
}



void ds_http_dsnet::handle_request(ds_http_request* request)
{
  ds_http_ip_addr_info ip_addr_info;
  if(ds_http_dns_find_ip(request->get_hostname(), ip_addr_info))
  {
    ds_http_add_request_to_socket(request, ip_addr_info);
  }
  else
  {
    ds_http_add_request_to_dns(request);
  }
}



void ds_http_dsnet::dsnet_down_clear_all()
{
  DATA_APPSRV_MSG0(MSG_LEGACY_HIGH, "dsnet_down_clear_all - Failing all requests");
  while(!dsnet_wait_queue.empty())
  {
    ds_http_request* request = dsnet_wait_queue.front();
    request->notify_error(DS_HTTP_ERROR_NETWORK_DOWN);
    ds_appsrv_free(request);
    dsnet_wait_queue.pop_front();
  }

  if(NULL != active_query)
  {
    active_query->net_down_event();
    ds_appsrv_free(active_query);
    active_query = NULL;
  }

  dns_iterator dns_it = dns_wait_queue_list.begin();
  while(dns_wait_queue_list.end() != dns_it)
  {
    (*dns_it)->net_down_event();
    ds_appsrv_free(*dns_it);
    dns_it = dns_wait_queue_list.erase(dns_it);
  }


  socket_iterator sock_it = socket_list.begin();
  while(socket_list.end() != sock_it)
  {
    (*sock_it)->net_down_event();
    if((*sock_it)->socket_closed())
    {
      ds_appsrv_free(*sock_it);
      sock_it = socket_list.erase(sock_it);
    }
    else
      sock_it++;
  }
}



/*==============================================================================
                                  DSNet handlers
==============================================================================*/
bool ds_http_dsnet::ds_http_init_dsnet()
{
  dss_net_policy_info_type policy;
  sint15 err = 0;

  ds_http_set_policy_info(&policy, iface_info);

  ip_family = ds_http_resolve_ip_type(iface_info);
  policy.family = ip_family;
  DATA_APPSRV_MSG1(MSG_LEGACY_HIGH, "Setting policy IP family to %d", ip_family);

  dsnet_handle_id = dsnet_get_handle(
                                     &ds_http_net_cb,
                                     NULL,
                                     &ds_http_sock_cb,
                                     NULL,
                                     &policy,
                                     &err
                                     );

  DATA_APPSRV_MSG8(
                   MSG_LEGACY_MED,
                   "ds_http_init_dsnet - handle id:%d iface(iface:%d profile:%d subs:%d), policy(iface:%d profile:%d subs:%d ip:%d)",
                   dsnet_handle_id,
                   iface_info.iface_type,
                   iface_info.profile_id,
                   iface_info.subs_id,
                   policy.iface.info.name,
                   policy.umts.pdp_profile_num,
                   policy.subs_id,
                   policy.family
                   );

  if (DSS_ERROR == dsnet_handle_id)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "Session could not be created error %d", err);
    return false;
  }

  dsnet_status = DS_HTTP_NET_STATUS_DOWN;
  return true;
}



bool ds_http_dsnet::ds_http_connect_dsnet()
{
  sint15 err = 0;
  sint15 result = 0;


  if(!get_sys_status())
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_MED, "ds_http_connect_dsnet - system is down; will not attemt to start dsnet");
    dsnet_status = DS_HTTP_NET_STATUS_DOWN;
    return false;
  }

  result = dsnet_start(dsnet_handle_id, &err);
  DATA_APPSRV_MSG2(MSG_LEGACY_HIGH, "ds_http_connect_dsnet - ret:%d err:%d", result, err);

  if (DSS_SUCCESS == result)
  {
    dsnet_status = DS_HTTP_NET_STATUS_UP;
    return true;
  }

  switch(err)
  {
    case DS_EWOULDBLOCK:
    {
      dsnet_status = DS_HTTP_NET_STATUS_UP_IN_PROGRESS;
      return true;
    }
    break;

    case DS_EOPNOTSUPP:
    {
      // TODO: create timer to try again later?
    } // fallthrough

    default:
    {
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_connect_dsnet - dsnet failed err %d", err);
      dsnet_status = DS_HTTP_NET_STATUS_DOWN;
    }
    break;
  }
  return false;
}




/*==============================================================================
                                  DNS handlers
==============================================================================*/
bool ds_http_dsnet::ds_http_open_dns_session()
{
  sint15                   err = 0;
  dss_net_policy_info_type policy;

  dns_session_handle = dss_dns_create_session(ds_http_dns_cb, NULL, &err);

  if (DSS_DNS_SESSION_MGR_INVALID_HANDLE == dns_session_handle)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_open_dns_session - Session could not be created error %d", err);
    return false;
  }

  ds_http_set_policy_info(&policy, iface_info);
  policy.family = ip_family;

  if (DSS_ERROR == dss_dns_set_config_params(
                                             dns_session_handle,
                                             DSS_DNS_CONFIG_PARAMS_NET_POLICY,
                                             &policy,
                                             sizeof(dss_net_policy_info_type),
                                             &err
                                             ))
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_open_dns_session - Config params can not set, error %d", err);
    ds_http_close_dns_session();
    return false;
  }
  DATA_APPSRV_MSG0(MSG_LEGACY_MED, "ds_http_open_dns_session - Opened DNS Session");
  return true;
}



void ds_http_dsnet::ds_http_close_dns_session()
{
  if(DSS_DNS_SESSION_MGR_INVALID_HANDLE == dns_session_handle)
    return;

  sint15 err = 0;
  dss_dns_delete_session(dns_session_handle, &err);
  dns_session_handle = DSS_DNS_SESSION_MGR_INVALID_HANDLE;
  DATA_APPSRV_MSG0(MSG_LEGACY_MED, "ds_http_close_dns_session - Removed DNS Session");
}



void ds_http_dsnet::ds_http_add_request_to_dns(ds_http_request* request)
{
  DATA_APPSRV_MSG0(MSG_LEGACY_MED, "ds_http_add_request_to_dns - request -> dns");
  if(NULL != active_query &&
     0 == strcmp(
                 active_query->request_queue.front()->get_hostname(),
                 request->get_hostname()
                 ))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "ds_http_add_request_to_dns - Adding to active");
    active_query->add_request(request);
    return;
  }

  for(dns_iterator it = dns_wait_queue_list.begin(); it != dns_wait_queue_list.end(); it++)
  {
    if(0 == strcmp(
                   (*it)->request_queue.front()->get_hostname(),
                   request->get_hostname()
                   ))
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "ds_http_add_request_to_dns - Adding to queue");
      (*it)->add_request(request);
      return;
    }
  }
  ds_http_create_dns_query(request);
}



void ds_http_dsnet::ds_http_create_dns_query(ds_http_request* request)
{
  ds_http_dns_query* dns_query = new(ds_appsrv_alloc<ds_http_dns_query>(), APPSRV_MEM) ds_http_dns_query(request);

  DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "ds_http_create_dns_query - Creating dns query");
  if(NULL == dns_query)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_create_dns_query - memory allocation failed");
    request->notify_error(DS_HTTP_ERROR_DNS);
    ds_appsrv_free(request);
    return;
  }

  if(NULL == active_query)
  {
    if(DSS_DNS_SESSION_MGR_INVALID_HANDLE == dns_session_handle)
    {
      if(!ds_http_open_dns_session())
      {
        DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_create_dns_query - Failed to open dns session");
        ds_appsrv_free(dns_query);
        return;
      }
    }

    if(!dns_query->start_query(dns_session_handle, ip_family))
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_create_dns_query - dns query request failed");
      ds_http_close_dns_session();
      ds_appsrv_free(dns_query);
      return;
    }
    active_query = dns_query;
    return;
  }


  // Another DNS query is currently active; add new query to wait queue.
  if(!dns_wait_queue_list.push_back(dns_query))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_create_dns_query - Failed to allocate memory for wait queue");
    ds_appsrv_free(dns_query);
  }
}




/*==============================================================================
                                    DNS Cache
==============================================================================*/
void ds_http_dsnet::ds_http_cache_dns_info(const char* hostname, const ds_http_ip_addr_info& addr_info)
{
  if(DS_HTTP_DNS_CACHE_MAX_SIZE == cache_counter)
    cache_counter = 0;

  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "ds_http_cache_dns_info - DNS Info : %d", addr_info.ip_addr);
  dns_cache[cache_counter].hostname = hostname;
  dns_cache[cache_counter].ip_addr_info = addr_info;
  cache_counter++;
}


bool ds_http_dsnet::ds_http_dns_find_ip(const char* hostname, ds_http_ip_addr_info& addr_info) const
{
  for (uint32 i = 0; i < DS_HTTP_DNS_CACHE_MAX_SIZE; ++i)
  {
    if (dns_cache[i].hostname == hostname)
    {
      addr_info = dns_cache[i].ip_addr_info;
      return true;
    }
  }
  return false;
}




/*==============================================================================
                                Socket handlers
==============================================================================*/
void ds_http_dsnet::ds_http_add_request_to_socket(ds_http_request* request, const ds_http_ip_addr_info& addr_info)
{
  for(socket_iterator it = socket_list.begin(); it != socket_list.end(); it++)
  {
    if ((*it)->get_ip_addr_info() == addr_info &&
        (*it)->get_port() == request->get_port())
    {
      (*it)->add_request(request);
      return;
    }
  }

  // socket not found, create new
  create_http_socket(request, addr_info);
}



ds_http_socket* ds_http_dsnet::create_http_socket(ds_http_request* request, const ds_http_ip_addr_info& addr_info)
{
  sint15 err = DS_HTTP_ERROR_NONE;
  ds_http_socket* http_socket = NULL;
  if(request->is_ssl_protocol())
    http_socket = new(ds_appsrv_alloc<ds_ssl_socket>(), APPSRV_MEM) ds_ssl_socket(request, dsnet_handle_id, iface_id, addr_info, &err);
  else
    http_socket = new(ds_appsrv_alloc<ds_http_socket>(), APPSRV_MEM) ds_http_socket(request, dsnet_handle_id, iface_id, addr_info, &err);

  if(NULL == http_socket)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "create_http_socket - socket memory allocation failed");
    request->notify_error(DS_HTTP_ERROR_MEMALLOC);
    ds_appsrv_free(request);
  }
  // socket_closed() returns true if http_socket fails to get socket fd
  else if(
          DS_HTTP_ERROR_NONE != err    ||
          http_socket->socket_closed() ||
          !socket_list.push_back(http_socket)
          )
  {
    DATA_APPSRV_MSG2(
                     MSG_LEGACY_ERROR,
                     "create_http_socket - invalid socket status err:%d closed status:%d or failed to add to socket list",
                     err,
                     http_socket->socket_closed()
                     );
    ds_appsrv_free(http_socket);
    http_socket = NULL;
  }
  return http_socket;
}



ds_http_socket* ds_http_dsnet::find_http_socket_by_sockfd(const sint15 sockfd) const
{
  for(socket_const_iterator it = socket_list.begin(); it != socket_list.end(); it++)
  {
    if ((*it)->sockfd == sockfd)
      return *it;
  }

  return NULL;
}



ds_http_socket* ds_http_dsnet::find_http_socket_by_socket_id(const uint16 socket_id) const
{
  for(socket_const_iterator it = socket_list.begin(); it != socket_list.end(); it++)
  {
    if ((*it)->socket_id == socket_id)
      return *it;
  }

  return NULL;
}



ds_http_socket* ds_http_dsnet::find_http_socket_by_request_id(const uint32 request_id) const
{
  for(socket_const_iterator it = socket_list.begin(); it != socket_list.end(); it++)
  {
    if ((*it)->get_active_request_id() == request_id)
      return *it;
  }

  return NULL;
}



void ds_http_dsnet::check_and_handle_socket_status(ds_http_socket* http_socket)
{
  if(NULL == http_socket)
    return;

  // Socket is in close wait status.
  if(http_socket->socket_closing())
  {
    if(!http_socket->is_active())
    {
      http_socket->clear_connection_info();
      return;
    }

    // Socket still has requests to handle. Create a socket to prevent close wait time
    ds_http_socket* new_socket = NULL;
    sint15          err        = 0;

    if(http_socket->is_ssl_protocol())
      new_socket = new(ds_appsrv_alloc<ds_ssl_socket>(), APPSRV_MEM) ds_ssl_socket(http_socket, &err);
    else
      new_socket = new(ds_appsrv_alloc<ds_http_socket>(), APPSRV_MEM) ds_http_socket(http_socket, &err);


    if(NULL == new_socket)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "check_and_handle_socket_status - mem fail");
    }
    // socket_closed() returns true if http_socket fails to get socket fd
    else if( DS_HTTP_ERROR_NONE != err   ||
             new_socket->socket_closed() ||
             !socket_list.push_back(new_socket) )
    {
      DATA_APPSRV_MSG2(
                       MSG_LEGACY_ERROR,
                       "check_and_handle_socket_status - invalid socket status err:%d closed status:%d or failed to add to socket list",
                       err,
                       http_socket->socket_closed()
                       );
      ds_appsrv_free(new_socket);
      new_socket = NULL;
    }
  }
  else if (http_socket->socket_closed())
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "check_and_handle_socket_status - Socket closed, removing socket %d", http_socket->socket_id);
    remove_http_socket(http_socket);
  }
}



void ds_http_dsnet::remove_http_socket(const uint32 socket_id)
{
  DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "remove_http_socket - socket %d closed", socket_id);
  for(socket_iterator it = socket_list.begin(); it != socket_list.end(); it++)
  {
    if ((*it)->socket_id == socket_id)
    {
      ds_appsrv_free(*it);
      socket_list.erase(it);
      return;
    }
  }
}



void ds_http_dsnet::remove_http_socket(ds_http_socket* http_socket)
{
  remove_http_socket(http_socket->socket_id);
}




/*==============================================================================
                              Callback msg handlers
==============================================================================*/
void ds_http_dsnet::handle_dsnet_event(const dss_iface_id_type iface, const sint15 error)
{
  ds_http_dsnet_status old_dsnet_status = dsnet_status;

  switch (error)
  {
    case DS_ENETISCONN:
    {
      iface_id     = iface;
      dsnet_status = DS_HTTP_NET_STATUS_UP;

      while(!dsnet_wait_queue.empty())
      {
        handle_request(dsnet_wait_queue.front());
        dsnet_wait_queue.pop_front();
      }
    }
    break;

    case DS_ENETNONET:
    {
      if(DS_HTTP_NET_STATUS_UP_IN_PROGRESS == dsnet_status)
      {
        DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "handle_dsnet_event - Bringup failed");
        dsnet_down_clear_all();
        dsnet_status = DS_HTTP_NET_STATUS_DOWN;
        break;
      }

      dsnet_status = DS_HTTP_NET_STATUS_DOWN;
      if(!ds_http_connect_dsnet())
        dsnet_down_clear_all();

      if(DS_HTTP_NET_STATUS_UP == dsnet_status)
      {
        handle_dsnet_event(iface_id, DS_ENETISCONN); // is this needed here?
      }
    }
    break;

    case DS_ENETCLOSEINPROGRESS:
    {
      dsnet_status = DS_HTTP_NET_STATUS_DOWN_IN_PROGRESS;
    }
    break;

    case DS_ENETINPROGRESS: // do nothing. keep waiting for DS_ENETISCONN.
    default:
      break;
  }

  DATA_APPSRV_MSG4(
                   MSG_LEGACY_MED,
                   "handle_dsnet_event - iface:%d error:%d status:%d->%d",
                   iface,
                   error,
                   old_dsnet_status,
                   dsnet_status
                   );
}



void ds_http_dsnet::handle_socket_event(const sint15 sockfd, const sint15 event_mask)
{
  ds_http_socket* http_socket = find_http_socket_by_sockfd(sockfd);
  if(NULL == http_socket)
    return;

  http_socket->socket_event(event_mask);
  check_and_handle_socket_status(http_socket);
}



void ds_http_dsnet::handle_dns_event(
                                     const dss_dns_query_handle_type query_handle,
                                     const uint16                    num_records,
                                     const int16                     dss_errno
                                     )
{
  DATA_APPSRV_MSG3(MSG_LEGACY_MED, "handle_dns_event - %d, %d, %d", query_handle, num_records, dss_errno);
  int16             res            = 0;
  int16             err            = 0;
  dss_dns_addrinfo* result_buf_ptr = NULL;

  if(NULL == active_query || query_handle != active_query->dns_query_handle)
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "handle_dns_event - NULL active query %d or query handle mismatch %d", active_query, query_handle);
    return;
  }

  do
  {
    if(0 == num_records)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "handle_dns_event - DNS record not found");
      break;
    }

    result_buf_ptr = (dss_dns_addrinfo*)ds_appsrv_alloc(sizeof(dss_dns_addrinfo), num_records);
    if(NULL == result_buf_ptr)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "handle_dns_event - allocate mem for result failed");
      dss_dns_read_addrinfo(dns_session_handle, query_handle, NULL, 0, &err); // to free dns memory.
      break;
    }

    res = dss_dns_read_addrinfo(
                                dns_session_handle,
                                query_handle,
                                result_buf_ptr,
                                num_records,
                                &err
                                );
    if(DSS_ERROR == res)
    {
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "handle_dns_event - dss_dns_read_addrinfo failed with error: %d", err);
      if(DS_EWOULDBLOCK == err)
      {
        // DNS query incomplete; don't remove active query
        ds_appsrv_free(result_buf_ptr);
        return;
      }
      break;
    }

    ds_http_ip_addr_info addr_info;
    bool found = false;

    // Get DNS result.
    for(uint16 i = 0; i < num_records; i++)
    {
      if (result_buf_ptr[i].ai_family != ip_family)
      {
        /* Socket IP type needs to match dsnet IP type */
        DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "handle_dns_event - DNS family type mismatch %d - %d", result_buf_ptr[i].ai_family, ip_family);
        continue;
      }

      if (result_buf_ptr[i].ai_family == DSS_AF_INET)
      {
        struct ps_sockaddr_in* in_addr = (struct ps_sockaddr_in*)&(result_buf_ptr[i].ai_sockaddr);
        addr_info.ip_addr              = in_addr->ps_sin_addr.ps_s_addr;
        addr_info.ip_addr_type         = DS_HTTP_IPV4;
        found                          = true;
        break;
      }
      else if(result_buf_ptr[i].ai_family == DSS_AF_INET6)
      {
        struct ps_sockaddr_in6* in_addr = (struct ps_sockaddr_in6*)&(result_buf_ptr[i].ai_sockaddr);
        memscpy(addr_info.ip_addr6, DS_HTTP_IPV6_SIZE, in_addr->ps_sin6_addr.in6_u.u6_addr32, DS_HTTP_IPV6_SIZE);
        addr_info.ip_addr_type          = DS_HTTP_IPV6;
        found                           = true;
        break;
      }
    }

    if(!found)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "handle_dns_event - Valid address not found");
      break;
    }

    ds_http_cache_dns_info(active_query->request_queue.front()->get_hostname(), addr_info);

    while(!active_query->request_queue.empty())
    {
      ds_http_add_request_to_socket(active_query->request_queue.front(), addr_info);
      active_query->request_queue.pop_front();
    }
  } while(false);

  if(NULL != active_query)
  {
    ds_appsrv_free(active_query);
    active_query = NULL;
  }

  if(NULL != result_buf_ptr)
    ds_appsrv_free(result_buf_ptr);

  while(!dns_wait_queue_list.empty() && NULL == active_query)
  {
    active_query = dns_wait_queue_list.front();
    dns_wait_queue_list.pop_front();
    if(!active_query->start_query(dns_session_handle, ip_family))
    {
      ds_appsrv_free(active_query);
      active_query = NULL;
    }
  }

  if(dns_wait_queue_list.empty() && NULL == active_query)
  {
    ds_http_close_dns_session();
  }
}



void ds_http_dsnet::handle_timeout_event(const uint16 socket_id)
{
  ds_http_socket* http_socket = find_http_socket_by_socket_id(socket_id);
  if(NULL == http_socket)
    return;

  http_socket->timeout_event();
  check_and_handle_socket_status(http_socket);
}



void ds_http_dsnet::resolve_auth(const uint32 request_id, const ds_http_auth_info_s_type* auth_info)
{
  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "resolve_auth - updating auth info for request %d", request_id);
  ds_http_socket* http_socket = find_http_socket_by_request_id(request_id);
  if(NULL == http_socket)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "resolve_auth - socket to update credential not found");
    return;
  }

  http_socket->resolve_auth(auth_info);
  check_and_handle_socket_status(http_socket);
}



void ds_http_dsnet::resolve_ssl_suspension(const uint32 request_id, const bool cont)
{
  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "resolve_ssl_suspension - resolving ssl suspension for request %d", request_id);
  ds_http_socket* http_socket = find_http_socket_by_request_id(request_id);
  if(NULL == http_socket)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "resolve_ssl_suspension - socket not found");
    return;
  }

  http_socket->resolve_ssl_suspension(cont);
  check_and_handle_socket_status(http_socket);
}


int ds_http_resolve_ip_type(const ds_http_dsnet_iface_info& iface_info)
{
  ds_profile_status_etype res = DS_PROFILE_REG_RESULT_FAIL;
  ds_profile_hndl_type    profile_handle = NULL;
  ds_profile_info_type    profile_info   = {0,0};
  byte                    value[sizeof(uint32)];
  uint32                  profile_ip_type = 0;
  int                     ret_val         = DSS_AF_INET;

  /* set IP family based on profile IP type */
  if (iface_info.iface_type != DS_HTTP_IFACE_WLAN_LB)
  {
    do
    {
      res = ds_profile_begin_transaction(DS_PROFILE_TRN_READ,
                                         DS_PROFILE_TECH_3GPP,
                                         (ds_profile_num_type)iface_info.get_profile_id(),
                                         &profile_handle);

      if ( DS_PROFILE_REG_RESULT_SUCCESS != res ||
           profile_handle == NULL )
      {
        DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "Begin transaction failed %d", res);
        break;
      }

      memset((void*)value, 0, sizeof(value));
      profile_info.buf = (void*)value;
      profile_info.len = sizeof(value);

      res =  ds_profile_get_param (profile_handle,
                                   (ds_profile_identifier_type)DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
                                   &profile_info);

      if ( DS_PROFILE_REG_RESULT_SUCCESS != res || profile_info.len > sizeof(value) )
      {
        DATA_APPSRV_MSG3(MSG_LEGACY_ERROR, "Get profile param failed %d or invalid param len %d : %d", res, sizeof(value), profile_info.len);
        break;
      }

      profile_ip_type = *((uint32*)profile_info.buf);
      if (profile_ip_type == DS_PROFILE_3GPP_PDP_IPV6)
      {
        ret_val = DSS_AF_INET6;
      }
    } while (0);

    if (profile_handle != NULL )
    {
      res = ds_profile_end_transaction (profile_handle,DS_PROFILE_ACTION_COMMIT);
    }
  }

  return ret_val;
}


void ds_http_set_policy_info(
                             dss_net_policy_info_type*       policy,
                             const ds_http_dsnet_iface_info& iface_info
                             )
{
  memset(policy, 0, sizeof(dss_net_policy_info_type));
  dss_init_net_policy_info(policy);

  policy->iface.kind           = DSS_IFACE_NAME;
  policy->is_routeable         = TRUE;
  policy->umts.pdp_profile_num = iface_info.get_profile_id();
  policy->subs_id              = iface_info.get_subs_id();

  // Set Iface info
  switch(iface_info.iface_type)
  {
    case DS_HTTP_IFACE_3GPP:
    {
      policy->iface.info.name = DSS_IFACE_3GPP_ANY;
    }
    break;

    case DS_HTTP_IFACE_IWLAN:
    {
      policy->iface.info.name = DSS_IFACE_IWLAN_S2B;
    }
    break;

    case DS_HTTP_IFACE_WLAN_LB:
    {
      policy->iface.info.name = DSS_IFACE_WLAN;
    }
    break;

    case DS_HTTP_IFACE_ANY:
    {
      policy->iface.info.name = DSS_IFACE_EPC_ANY;
    }
    break;

    default:
      break;
  }
}

