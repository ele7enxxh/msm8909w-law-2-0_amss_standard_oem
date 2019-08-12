/*==============================================================================

                              ds_http_socket.cpp

GENERAL DESCRIPTION
  HTTP socket manager

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/02/15    ml     Permanent redirect
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/22/15    ml     IPv6 support
06/11/15    ml     Cookie support
05/20/15    ml     Use memory from modem heap
04/16/15    ml     Response authentication support.
02/19/15    ml     Redirect requests to a new socket if current socket is in close wait status.
01/07/15    ml     Fixed bug for persistent timeout being 0 when it should be default.
01/02/15    ml     Added timeout to handle unresolved block status.
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_socket.h"

#include "ds_http_event_handler.h" // ds_http_timeout_timer_cb
#include "ds_http_msg.h"
#include "ds_http_request.h"
#include "ds_http_response.h"
#include "ds_http_utility.h"
#include "ds_appsrv_mem.h"

#include "dssinternal.h"
#include "data_msg.h"
#include "rex.h"
#include <stringl/stringl.h>

#if defined(TEST_FRAMEWORK) && defined(T_WINNT)
  #error code not present
#endif

// 10 seconds
#define DS_HTTP_AUTH_CLIENT_WAIT_TIME 10000

// 5 seconds
#define DS_HTTP_CHUNK_CLIENT_WAIT_TIME 5000

/* HTTP request cancelled */
#define DS_HTTP_ERROR_REQUEST_CANCELLED -400


static const uint32 DS_HTTP_HEADER_BUFFER_SIZE = 2000;

static uint16 socket_id_counter = 0;

typedef ASList<ds_http_request*>::iterator       request_iterator;
typedef ASList<ds_http_request*>::const_iterator request_const_iterator;

/*==============================================================================
                              ds_http_socket
==============================================================================*/
ds_http_socket::ds_http_socket(
                               ds_http_request*            request,
                               const sint15                dsnet_handle,
                               const dss_iface_id_type     iface,
                               const ds_http_ip_addr_info& addr_info,
                               sint15*                     err
                               )
: socket_id(socket_id_counter++), // overflow reset to 0 is ok
  sockfd(-1),
  port(request->get_port()),
  socket_status(SOCKET_STATUS_SOCKET_CONNECT),
  active_request(request),
  active_response(NULL),
  timeout_length(0),
  dsnet_handle_id(dsnet_handle),
  iface_id(iface),
  ip_addr_info(addr_info),
  persistent(true),
  filter_set(false),
  waiting_ds_close_event(false)
{
  socket_init();
}


ds_http_socket::ds_http_socket(
                               ds_http_socket* prev_socket,
                               sint15*         err
                               )
: socket_id(socket_id_counter++), // overflow reset to 0 is ok
  sockfd(-1),
  port(prev_socket->port),
  socket_status(SOCKET_STATUS_SOCKET_CONNECT),
  active_request(NULL),
  active_response(NULL),
  timeout_length(0),
  dsnet_handle_id(prev_socket->dsnet_handle_id),
  iface_id(prev_socket->iface_id),
  ip_addr_info(prev_socket->ip_addr_info),
  persistent(true),
  filter_set(false),
  waiting_ds_close_event(false)
{
  DATA_APPSRV_MSG1(MSG_LEGACY_HIGH, "ds_http_socket - Copying socket id %d", prev_socket->socket_id);
  // move the requests from the old socket to the new one
  if(NULL != prev_socket->active_request)
  {
    active_request = prev_socket->active_request;
    prev_socket->active_request = NULL;
  }
  else if(!prev_socket->request_queue.empty())
  {
    active_request = prev_socket->request_queue.front();
    prev_socket->request_queue.pop_front();
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_socket copy constructor - No request to handle in previous socket");
    *err = DS_HTTP_ERROR_SOCKET;
    return;
  }

  while(!prev_socket->request_queue.empty())
  {
    request_queue.push_back(prev_socket->request_queue.front());
    prev_socket->request_queue.pop_front();
  }

  socket_init();
  prev_socket->clear_connection_info();
}

void ds_http_socket::clear_connection_info()
{
  DATA_APPSRV_MSG1(MSG_LEGACY_HIGH, "clear_connection_info - socket id %d", socket_id);
  // remove server and port information from the old socket so that the new
  // socket gets new requests instead of the old socket
  port = 0;
  ip_addr_info.clear(); // 0.0.0.0
}


ds_http_socket::~ds_http_socket()
{
  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "~ds_http_socket - Socket %d: destructor", socket_id);
  if(!request_queue.empty())
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "~ds_http_socket - request queue not empty");

  if(NULL != active_request)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "~ds_http_socket - socket is active");
    active_request->notify_error(DS_HTTP_ERROR_SOCKET);
    ds_appsrv_free(active_request);
  }

  if(NULL != active_response)
    ds_appsrv_free(active_response);

  if(0 != rex_get_timer(&timeout_timer))
    rex_clr_timer(&timeout_timer);
}


void ds_http_socket::socket_init()
{
  uint32 var = ((dsnet_handle_id << 16) | socket_id); // 32 bit representing 'this' socket for timer_cb

  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "socket_init - HTTP_SOCKET id:%d port:%d", socket_id, port);

  rex_def_timer_ex(
                   &timeout_timer,
                   &ds_http_timeout_timer_cb,
                   var
                   );

  http_create_socket();
  http_connect();
}

/*==============================================================================
                          ds_http_socket - Utility
==============================================================================*/
bool ds_http_socket::socket_closing() const
{
  return SOCKET_STATUS_CLOSE == socket_status;
}


bool ds_http_socket::socket_closed() const
{
  return SOCKET_STATUS_CLOSED == socket_status;
}


bool ds_http_socket::is_active() const
{
  return (NULL != active_request || !request_queue.empty());
}


bool ds_http_socket::is_ssl_protocol() const
{
  if(NULL != active_request)
    return active_request->is_ssl_protocol();

  else if(!request_queue.empty())
    return request_queue.front()->is_ssl_protocol();

  return false;
}


const ds_http_ip_addr_info& ds_http_socket::get_ip_addr_info() const
{
  return ip_addr_info;
}


uint32 ds_http_socket::get_port() const
{
  return port;
}


sint15 ds_http_socket::get_nethandle() const
{
  return dsnet_handle_id;
}


uint32 ds_http_socket::get_active_request_id() const
{
  if(NULL == active_request)
    return DS_HTTP_ERROR;
  return active_request->request_id;
}



/*==============================================================================
                        ds_http_socket - Event Handlers
==============================================================================*/
void ds_http_socket::timeout_event()
{
  DATA_APPSRV_MSG2(
                   MSG_LEGACY_MED,
                   "ds_http_socket::timeout_event - Socket %d: timeout triggered in state:%d",
                   socket_id,
                   socket_status
                   );

  switch(socket_status)
  {
    case SOCKET_STATUS_SEND_WAIT: // fallthrough
    case SOCKET_STATUS_AUTH:
    {
      socket_error(DS_HTTP_ERROR_TIMEOUT);
    }
    break;

    case SOCKET_STATUS_CONNECTED:
    {
      socket_status = SOCKET_STATUS_CLOSE;
      http_close();
    }
    break;

    case SOCKET_STATUS_SOCKET_CONNECT: // fallthrough
    case SOCKET_STATUS_SEND:           // fallthrough
    case SOCKET_STATUS_RECEIVE:        // fallthrough
    case SOCKET_STATUS_CLOSE:          // fallthrough
    case SOCKET_STATUS_CLOSED:         // fallthrough
    default:
    {
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_socket::timeout_event - timeout triggered in invalid state %d", socket_status);
      socket_error(DS_HTTP_ERROR_TIMEOUT);
    }
    break;
  }
}



void ds_http_socket::net_down_event()
{
  while (!request_queue.empty())
  {
    ds_http_request* request = request_queue.front();
    request->notify_error(DS_HTTP_ERROR_NETWORK_DOWN);
    ds_appsrv_free(request);
    request_queue.pop_front();
  }

  socket_error(DS_HTTP_ERROR_NETWORK_DOWN);

  if(SOCKET_STATUS_CLOSE != socket_status && SOCKET_STATUS_CLOSED != socket_status)
  {
    socket_status = SOCKET_STATUS_CLOSE;
    http_close();
  }
}



void ds_http_socket::socket_event(const uint32 event_mask)
{
  socket_status_e_type prev_socket_status = socket_status;

  if(0 != rex_get_timer(&timeout_timer))
    rex_clr_timer(&timeout_timer);

  switch(socket_status)
  {
    case SOCKET_STATUS_SOCKET_CONNECT:
    {
      if((event_mask & DS_WRITE_EVENT) != 0)
      {
        http_connect();
        break;
      }
    }
    break;

    case SOCKET_STATUS_SEND:
    {
      if((event_mask & DS_WRITE_EVENT) != 0)
      {
        http_send();
        break;
      }
    }
    break;

    case SOCKET_STATUS_RECEIVE:
    {
      if((event_mask & DS_READ_EVENT) != 0)
      {
        http_receive();
        break;
      }
    }
    break;

    case SOCKET_STATUS_CLOSE:
    {
      if((event_mask & DS_CLOSE_EVENT) != 0)
      {
        http_close();
        break;
      }
    }
    break;

    case SOCKET_STATUS_SEND_WAIT: // fallthrough
    case SOCKET_STATUS_CLOSED:    // fallthrough
    default:
      break;
  }

  DATA_APPSRV_MSG4(
                   MSG_LEGACY_HIGH,
                   "ds_http_socket::socket_event - socket:%d status:%d -> %d event_mask:%d",
                   socket_id,
                   prev_socket_status,
                   socket_status,
                   event_mask
                   );
}




/*==============================================================================
                      ds_http_socket - Utility/Virtual
==============================================================================*/
void ds_http_socket::init_values()
{
  sockfd = DSS_ERROR;
}



void ds_http_socket::add_request(ds_http_request* request)
{
  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "ds_http_socket::add_request - Socket %d: Adding request %d to queue", socket_id, request->request_id);
  request_queue.push_back(request);

  if(NULL == active_request && SOCKET_STATUS_CONNECTED == socket_status)
    http_connected();
}



bool ds_http_socket::remove_request(const uint32 request_id)
{
  if(NULL != active_request && request_id == active_request->request_id)
  {
    // Request in progress; request must be complete before removing.
    active_request->cancel_request();

    if(NULL != active_response)
    {
      active_response->cancel_request();
    }
    else if(active_request->is_chunked_transfer())
    {
      // If chunked, terminate tcp connection with server to cancel request.
      // Exception for receive state; let the transfer complete and continue
      // using the socket for other requests in queue.
      socket_error(DS_HTTP_ERROR_REQUEST_CANCELLED);
    }

    return true;
  }

  request_iterator request_it = request_queue.begin();
  while(request_queue.end() != request_it)
  {
    if(request_id == (*request_it)->request_id)
    {
      // No need to notify client
      ds_appsrv_free(*request_it);
      request_queue.erase(request_it);
      return true;
    }

    // else
    ++request_it;
  }

  return false;
}



void ds_http_socket::remove_requests_for_session(const uint32 session_id)
{
  if(NULL != active_request && session_id == active_request->session_id)
  {
    // Request in progress; request must be complete before removing.
    active_request->cancel_request();

    if(NULL != active_response)
    {
      active_response->cancel_request();
    }
    else if(active_request->is_chunked_transfer())
    {
      // If chunked, terminate tcp connection with server to cancel request.
      // Exception for receive state; let the transfer complete and continue
      // using the socket for other requests in queue.
      socket_error(DS_HTTP_ERROR_REQUEST_CANCELLED);
    }
  }

  request_iterator request_it = request_queue.begin();
  while(request_queue.end() != request_it)
  {
    if(session_id == (*request_it)->session_id)
    {
      // No need to notify client
      ds_appsrv_free(*request_it);
      request_it = request_queue.erase(request_it);
    }
    else
    {
      ++request_it;
    }
  }
}



void ds_http_socket::resolve_auth(const ds_http_auth_info_s_type* auth_info)
{
  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "resolve_auth - status %d pers:%d", socket_status, persistent);
  if(SOCKET_STATUS_AUTH != socket_status)
    return;

  if(0 != rex_get_timer(&timeout_timer))
    rex_clr_timer(&timeout_timer);

  if(NULL == active_response)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "resolve_auth - Unexpected NULL response object");
    status_error(DS_HTTP_ERROR_HTTP_STATUS, 401);
    return;
  }

  if( !active_request->create_auth_header_str(auth_info) )
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "resolve_auth - Failed to create auth header %d", auth_info);
    status_error(DS_HTTP_ERROR_HTTP_STATUS, active_response->http_status);
    return;
  }

  ds_appsrv_free(active_response);
  active_response = NULL;

  if(persistent)
  {
    socket_status = SOCKET_STATUS_SEND;
    active_request->reset_send_status();
    http_send();
  }
  else
  {
    socket_status = SOCKET_STATUS_CLOSE;
    http_close();
  }
}



void ds_http_socket::resolve_ssl_suspension(bool cont)
{
  // do nothing
}



sint15 ds_http_socket::http_write(const sint15 sockfd, const uint8* buffer, const uint32 buffer_size, sint15* err)
{
  return dss_write(sockfd, buffer, buffer_size, err);
}



sint15 ds_http_socket::http_read(const sint15 sockfd, uint8* buffer, const uint32 buffer_size, sint15* err)
{
  return dss_read(sockfd, buffer, buffer_size, err);
}



bool ds_http_socket::check_socket_error(
                                        const sint15 retval,
                                        const sint15 sock_err,
                                        const sint31 event_mask,
                                        const sint15 internal_err
                                        )
{
  sint15 err = 0;

  if(DSS_ERROR == retval)
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_LOW, "socket error: %d, internal error: %d", sock_err, internal_err);
    if(DS_EWOULDBLOCK == sock_err)
    {
      dss_async_select(sockfd, event_mask, &err);
    }
    else
    {
      socket_error(internal_err);
    }
    return true;
  }

  if(DS_READ_EVENT == event_mask && 0 == retval)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "check_socket_error - Unexpected eof");
    socket_error(DS_HTTP_ERROR_SOCKET);
    return true;
  }

  return false;
}




/*==============================================================================
                          ds_http_socket - Flow
==============================================================================*/
void ds_http_socket::http_create_socket()
{
  sint15 err = 0;
  init_values();

  int    val              = 1;
  uint32 val_len          = sizeof(int);

  //create socket
  if (ip_addr_info.ip_addr_type == DS_HTTP_IPV4)
  {
    sockfd = dss_socket(dsnet_handle_id, DSS_AF_INET, DSS_SOCK_STREAM, IPPROTO_TCP, &err);
  }
  else
  {
    sockfd = dss_socket(dsnet_handle_id, DSS_AF_INET6, DSS_SOCK_STREAM, IPPROTO_TCP, &err);
  }

  if(DSS_ERROR == sockfd)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "Failed to create socket. Error - %d", err);
    return;
  }

  if(0 != active_request->reserved_src_port)
  {
    sint15 retval = DSS_ERROR;
    DATA_APPSRV_MSG1(
                     MSG_LEGACY_LOW,
                     "http_create_socket: client reserved src_port:%d",
                     active_request->reserved_src_port
                     );

    if (ip_addr_info.ip_addr_type == DS_HTTP_IPV4)
    {
      struct ps_sockaddr_in dss_sockaddr;

      dss_sockaddr.ps_sin_family = DSS_AF_INET;
      dss_sockaddr.ps_sin_port = dss_htons(active_request->reserved_src_port);
      dss_sockaddr.ps_sin_addr.ps_s_addr = 0;

      retval = dss_bind(sockfd,
                         (struct ps_sockaddr*)&dss_sockaddr,
                         sizeof(struct ps_sockaddr_in),
                         &err);
    }
    else
    {
      struct ps_sockaddr_in6 dss_sockaddr;
      memset(&dss_sockaddr, 0, sizeof(ps_sockaddr_in6));

      dss_sockaddr.ps_sin6_family = DSS_AF_INET6;
      dss_sockaddr.ps_sin6_port = dss_htons(active_request->reserved_src_port);

      retval = dss_bind(sockfd,
                        (struct ps_sockaddr*)&dss_sockaddr,
                        sizeof(struct ps_sockaddr_in),
                        &err);
    }

    if(DSS_SUCCESS != retval)
    {
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR,  "http_create_socket: Failed to bind socket: %d", err);
      dss_close(sockfd, &err);
      sockfd = DSS_ERROR;
      return;
    }
  }



  DATA_APPSRV_MSG2(MSG_LEGACY_LOW, "http_create_socket - Socket created sockfd:%d, port:%d", sockfd, port);

  if(DSS_SUCCESS != dss_setsockopt(
                                   sockfd,
                                   (int32)DSS_SOCK,
                                   (int32)DSS_SO_SYS_SOCK,
                                   &val,
                                   &val_len,
                                   &err
                                   ))
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "http_create_socket: unable to set socket to system %d %d", err);
    dss_close(sockfd, &err);
    sockfd = DSS_ERROR;
    return;
  }

  DATA_APPSRV_MSG3(MSG_LEGACY_LOW, "http_create_socket - registering filter for id:%d socket:%d iface:%d", socket_id, sockfd, iface_id);
  ip_filter_type filter;
  memset(&filter, 0, sizeof(ip_filter_type));

  if (ip_addr_info.ip_addr_type == DS_HTTP_IPV4)
  {
    filter.ip_vsn                       = IP_V4;
    filter.ip_hdr.v4.field_mask         = IPFLTR_MASK_IP4_NEXT_HDR_PROT;
    filter.ip_hdr.v4.next_hdr_prot      = PS_IPPROTO_TCP;
  }
  else
  {
    filter.ip_vsn                       = IP_V6;
    filter.ip_hdr.v6.field_mask         = IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    filter.ip_hdr.v6.next_hdr_prot      = PS_IPPROTO_TCP;
  }

  if(0 == active_request->reserved_src_port)
  {
    filter.next_prot_hdr.tcp.field_mask = IPFLTR_MASK_TCP_SRC_PORT;
    filter.next_prot_hdr.tcp.src.port   = dss_htons(port);
    filter.next_prot_hdr.tcp.src.range  = 0;
  }
  else
  {
    filter.next_prot_hdr.tcp.field_mask = IPFLTR_MASK_TCP_DST_PORT;
    filter.next_prot_hdr.tcp.dst.port   = dss_htons(active_request->reserved_src_port);
    filter.next_prot_hdr.tcp.dst.range  = 0;
  }

  if(DSS_ERROR == dss_reg_ip_filter(
                                    sockfd,
                                    iface_id,
                                    IPFLTR_DEFAULT_TYPE,
                                    1,
                                    &filter,
                                    &err
                                    ))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "http_create_socket, failed to set filter");
    dss_close(sockfd, &err);
    sockfd = DSS_ERROR;
    return;
  }
  filter_set = true;
}



void ds_http_socket::http_connect()
{
  sint15 err = 0;
  sint15 retval = 0;

  if(NULL == active_request)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "http_connect - Unexpected NULL request");
    http_close_complete();
    return;
  }

  DATA_APPSRV_MSG1(MSG_LEGACY_HIGH, "http_connect - %d", active_request->request_id);

  if(DSS_ERROR == sockfd)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "http_connect - Failed to obtain socket fd");
    active_request->notify_error(DS_HTTP_ERROR_SOCKET);
    ds_appsrv_free(active_request);
    active_request = NULL;
    http_close_complete();
    return;
  }

  struct ps_sockaddr_in  sock_addr;
  struct ps_sockaddr_in6 sock_addr6;
  struct ps_sockaddr*    ps_sock_addr = (struct ps_sockaddr*)&sock_addr;
  uint32                 sockaddr_size = 0;

  if(DS_HTTP_IPV4 == ip_addr_info.ip_addr_type)
  {
    memset(&sock_addr, 0, sizeof(struct ps_sockaddr_in));
    sock_addr.ps_sin_addr.ps_s_addr = ip_addr_info.ip_addr;
    sock_addr.ps_sin_family         = DSS_AF_INET;
    sock_addr.ps_sin_port           = dss_htons(port);

    ps_sock_addr  = (struct ps_sockaddr*)&sock_addr;
    sockaddr_size = sizeof(struct ps_sockaddr_in);
  }
  else // if(DS_HTTP_IPV6 == ip_addr_info.ip_addr_type)
  {
    memset(&sock_addr6, 0, sizeof(struct ps_sockaddr_in6));
    memscpy(sock_addr6.ps_sin6_addr.in6_u.u6_addr32, DS_HTTP_IPV6_SIZE, ip_addr_info.ip_addr6, DS_HTTP_IPV6_SIZE);
    sock_addr6.ps_sin6_family = DSS_AF_INET6;
    sock_addr6.ps_sin6_port   = dss_htons(port);

    ps_sock_addr  = (struct ps_sockaddr*)&sock_addr6;
    sockaddr_size = sizeof(struct ps_sockaddr_in6);
  }

  retval = dss_connect(sockfd, ps_sock_addr, sockaddr_size, &err);
  waiting_ds_close_event = false;

  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "http_connect - ret:%d err:%d", retval, err);

  if(DSS_SUCCESS == retval)
    err = DS_EISCONN;

  switch(err)
  {
    case DS_EWOULDBLOCK:
    case DS_EINPROGRESS:
    {
      dss_async_select(sockfd, DS_WRITE_EVENT, &err);
    }
    break;

    case DS_EISCONN:
    {
      socket_connected();
    }
    break;

    default:
    {
      socket_error(DS_HTTP_ERROR_SOCKET);
    }
    break;
  }
}



void ds_http_socket::http_send()
{
  sint15 err           = 0;
  sint31 bytes_written = 0;

  if(0 != rex_get_timer(&timeout_timer))
    rex_clr_timer(&timeout_timer);

  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "http_send - Socket %d request %d", socket_id, active_request->request_id);
  while(!active_request->send_complete())
  {
    bytes_written = http_write(
                               sockfd,
                               active_request->get_send_buffer_content(),
                               active_request->get_send_buffer_size(),
                               &err
                               );

    if(check_socket_error(bytes_written, err, DS_WRITE_EVENT, DS_HTTP_ERROR_SOCKET))
      return;

    active_request->update_send_status(bytes_written);

    if(active_request->is_chunked_transfer() && active_request->is_waiting_chunk())
    {
      DATA_APPSRV_MSG1(
                       MSG_LEGACY_MED,
                       "http_send - Waiting client for more chunks to send, %d seconds",
                       DS_HTTP_CHUNK_CLIENT_WAIT_TIME/1000
                       );
      socket_status = SOCKET_STATUS_SEND_WAIT;
      rex_set_timer(&timeout_timer, DS_HTTP_CHUNK_CLIENT_WAIT_TIME);
      return;
    }
  }

  if(active_request->is_chunked_transfer() && !active_request->chunked_transfer_complete())
  {
    return;
  }

  DATA_APPSRV_MSG0(MSG_LEGACY_MED, "http_send - Finished sending request");
  socket_status   = SOCKET_STATUS_RECEIVE;
  active_response = new(ds_appsrv_alloc<ds_http_response>(), APPSRV_MEM) ds_http_response(
                                                                                          active_request->session_id,
                                                                                          active_request->request_id
                                                                                          );

  if(NULL == active_response)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "http_send - memory allocation for response object failed");
    socket_error(DS_HTTP_ERROR_MEMALLOC);
  }
  else
  {
    dss_async_select(sockfd, DS_READ_EVENT, &err);
  }
}


bool ds_http_socket::append_chunk_to_request(
                                             const uint32                   request_id,
                                             const ds_http_chunked_content& chunked_content
                                             )
{
  if(NULL != active_request && active_request->request_id == request_id)
  {
    active_request->append_chunked_content(chunked_content);

    if(SOCKET_STATUS_SEND_WAIT == socket_status)
    {
      socket_status = SOCKET_STATUS_SEND;

      if(0 != rex_get_timer(&timeout_timer))
        rex_clr_timer(&timeout_timer);

      // ???:
      // sint15 err = 0;
      // dss_async_select(sockfd, DS_WRITE_EVENT, &err);
      http_send();
    }

    return true;
  }

  // else
  for(request_iterator request_it = request_queue.begin(); request_queue.end() != request_it; ++request_it)
  {
    if(request_id == (*request_it)->request_id)
    {
      (*request_it)->append_chunked_content(chunked_content);
      return true;
    }
  }

  return false;
}


void ds_http_socket::http_receive()
{
  sint15 err        = 0;
  sint15 bytes_read = 0;
  uint8* buffer     = (uint8*)ds_appsrv_alloc(sizeof(uint8), DS_HTTP_HEADER_BUFFER_SIZE+1);

  if(0 != rex_get_timer(&timeout_timer))
    rex_clr_timer(&timeout_timer);

  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "http_receive - Socket %d", socket_id);
  if(NULL == buffer)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "http_receive - memory allocation failed");
    socket_error(DS_HTTP_ERROR_SOCKET);
    return;
  }

  do
  {
    memset(buffer, 0, DS_HTTP_HEADER_BUFFER_SIZE+1);
    bytes_read = http_read(sockfd, buffer, DS_HTTP_HEADER_BUFFER_SIZE, &err);
    DATA_APPSRV_MSG2(MSG_LEGACY_MED, "http_receive - bytes_read:%d , error:%d", bytes_read, err);

    if(check_socket_error(bytes_read, err, DS_READ_EVENT, DS_HTTP_ERROR_SOCKET))
      break;

    if(!active_response->append_response(buffer, bytes_read))
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "http_receive - Error in response header/content");
      socket_error(DS_HTTP_ERROR_HTTP_HEADER);
      break;
    }

    if(active_response->receive_complete())
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_MED, "http_receive - Finished receiving content");
      handle_response_status();
      break;
    }
  } while(true);

  ds_appsrv_free(buffer);
}



void ds_http_socket::http_connected()
{
  DATA_APPSRV_MSG3(MSG_LEGACY_LOW, "http_connected - %d %p %p", socket_id, active_request, active_response);
  if(NULL != active_request)
  {
    ds_appsrv_free(active_request);
    active_request = NULL;
  }
  if(NULL != active_response)
  {
    ds_appsrv_free(active_response);
    active_response = NULL;
  }

  if(persistent && 0 != timeout_length)
  {
    if(request_queue.empty())
    {
      DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "http_connected - Setting timeout timer for %d ms", timeout_length);
      rex_set_timer(&timeout_timer, timeout_length);
    }
    else
    {
      if(0 != rex_get_timer(&timeout_timer))
        rex_clr_timer(&timeout_timer);

      active_request = request_queue.front();
      request_queue.pop_front();

      socket_status = SOCKET_STATUS_SEND;
      active_request->reset_send_status();
      http_send();
    }
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "http_connected - server is not keep-alive");
    socket_status = SOCKET_STATUS_CLOSE;
    http_close();
  }
}



void ds_http_socket::http_close()
{
  DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "http_close - %d", socket_id);
  if(close_socket())
    http_close_complete();
}



bool ds_http_socket::close_socket()
{
  sint15 err = 0;
  sint15 result = 0;

  if(filter_set && waiting_ds_close_event)
  {
    DATA_APPSRV_MSG3(MSG_LEGACY_MED, "ds_http_socket::close_socket - deregistering filter for id:%d socket:%d iface:%d", socket_id, sockfd, iface_id);
    if(DSS_ERROR == dss_dereg_ip_filter(sockfd, iface_id, &err))
    {
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "close_socket - ip filter dereg failed %d", err);
    }
    filter_set = false;
  }

  result = dss_close(sockfd, &err);
  DATA_APPSRV_MSG4(MSG_LEGACY_MED, "ds_http_socket::close_socket - Socket:%d fd:%d result:%d err:%d", socket_id, sockfd, result, err);

  if(DSS_ERROR == result)
  {
    if(DS_EWOULDBLOCK == err)
    {
      waiting_ds_close_event = true;
      dss_async_select(sockfd, DS_CLOSE_EVENT, &err);
      return false;
    }
    else
    {
      // ???: what to do? handle unexpected error?
    }
  }

  sockfd = DSS_ERROR;
  return true;
}



void ds_http_socket::http_close_complete()
{
  socket_status = SOCKET_STATUS_CLOSED;
  sockfd = DSS_ERROR;
  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "http_close_complete - Socket %d", socket_id);

  if(request_queue.empty() && NULL == active_request)
    return;

  if(NULL == active_request)
  {
    active_request = request_queue.front();
    request_queue.pop_front();
  }

  socket_status = SOCKET_STATUS_SOCKET_CONNECT;
  http_create_socket();
  http_connect();
}




void ds_http_socket::socket_connected()
{
  socket_status = SOCKET_STATUS_SEND;
  http_send();
}




/*==============================================================================
                        ds_http_socket - Status Handler
==============================================================================*/
void ds_http_socket::handle_response_status()
{
  timeout_length = active_response->timeout * 1000; // s to ms
  persistent     = active_response->keep_alive;

  DATA_APPSRV_MSG3(MSG_LEGACY_MED, "handle_response_status - Socket %d Keep alive %d, timeout %d", socket_id, persistent, timeout_length);

  if(active_request->is_cancelled())
  {
    ds_appsrv_free(active_request);
    ds_appsrv_free(active_response);
    active_request = NULL;
    active_response = NULL;

    socket_status = SOCKET_STATUS_CONNECTED;
    http_connected();
    return;
  }

  switch(active_response->http_status)
  {
    case 200: // Success
    {
      status_success();
    }
    break;

    case 301: /* Permanent redirect */
    case 308: /* Permanent redirect */
    case 300: /* Multiple choices - Choose first one; treat it as temp redirect */
    case 302: /* temp redirect */
    case 303: /* temp redirect */
    case 307: /* temp redirect */
    {
      status_redirect();
    }
    break;

    case 401: // fallthrough
    case 407: // Authenticate
    {
      status_authenticate(active_response->http_status);
    }
    break;

    default:
    {
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "handle_response_status - HTTP Error Status %d", active_response->http_status);
      status_error(DS_HTTP_ERROR_HTTP_STATUS, active_response->http_status);
    }
    break;
  }
}



void ds_http_socket::status_success()
{
  uint32 error_status = active_response->validate_response(
                                                           active_request->get_credential(),
                                                           active_request->get_uri_path(),
                                                           active_request->get_request_auth_info()
                                                           );

  DATA_APPSRV_MSG3(
                   MSG_LEGACY_HIGH,
                   "status_success - HTTP Request success %d : %d auth:%d",
                   socket_id,
                   active_response->request_id,
                   error_status
                   );

  active_response->notify_cb(error_status);
  ds_appsrv_free(active_request);
  ds_appsrv_free(active_response);
  active_request = NULL;
  active_response = NULL;

  socket_status = SOCKET_STATUS_CONNECTED;
  http_connected();
}


void ds_http_socket::update_permanent_redirect(const ds_http_request_uri& redirect_uri)
{
  bool             expiration_set = false;
  const char*      cache_str      = active_response->get_header_value("Cache-Control");
  const char*      expires_str    = active_response->get_header_value("Expires");
  time_julian_type expiration_date;

  memset(&expiration_date, 0, sizeof(time_julian_type));
  time_jul_from_secs( time_get_secs(), &expiration_date );

  // Cache-Control takes precedence over Expires header
  if(NULL != cache_str && 0 != strlen(cache_str))
  {
    ds_http_cache_control_info cache_control(cache_str);

    if( !cache_control.no_cache &&
        !cache_control.no_store &&
        cache_control.max_age.is_set() )
    {
      time_jul_from_secs( time_get_secs() + cache_control.max_age.get(), &expiration_date );
      expiration_set = true;
    }
  }
  else if(NULL != expires_str && 0 != strlen(expires_str))
  {
    expiration_set = parse_http_expires_date( ASString(expires_str), expiration_date );
  }

  add_redirect_cache(active_request->get_uri_info(), redirect_uri, expiration_date, expiration_set);
}



void ds_http_socket::status_redirect()
{
  const char* redirect_uri_str = active_response->get_header_value("Location");

  ds_http_request_uri redirect_uri;

  if(NULL == redirect_uri_str || !parse_uri(redirect_uri_str, redirect_uri))
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "status_redirect - uri parsing failed %d", redirect_uri_str);
    active_response->notify_cb(DS_HTTP_ERROR_HTTP_STATUS);
    ds_appsrv_free(active_request);
  }
  else
  {
    // Cache permanent redirect URI
    if(301 == active_response->http_status || 308 == active_response->http_status)
    {
      update_permanent_redirect(redirect_uri);
    }

    if( !active_request->update_redirect(redirect_uri) || !ds_http_add_new_request(active_request) )
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "status_redirect - Redirect URI failed");
      active_response->notify_cb(DS_HTTP_ERROR_HTTP_STATUS);
      ds_appsrv_free(active_request);
    }
  }

  ds_appsrv_free(active_response);
  active_response = NULL;
  active_request  = NULL;
  socket_status   = SOCKET_STATUS_CONNECTED;
  http_connected();
}



void ds_http_socket::status_authenticate(uint32 status)
{
  const char* auth_str = NULL;

  if(401 == status)
    auth_str = active_response->get_header_value("WWW-Authenticate");
  else if(407 == status)
    auth_str = active_response->get_header_value("Proxy-Authenticate");

  if( NULL == auth_str || !active_request->decode_auth_header(auth_str, status) )
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "status_authenticate - Auth header parsing failed %d", auth_str);
    status_error(DS_HTTP_ERROR_HTTP_HEADER, active_response->http_status);
    return;
  }

  socket_status = SOCKET_STATUS_AUTH;

  // Socket will wait 10 seconds for the client to respond with a credential.
  DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "status_authenticate - Setting timeout timer for %d seconds", DS_HTTP_AUTH_CLIENT_WAIT_TIME/1000);
  rex_set_timer(&timeout_timer, DS_HTTP_AUTH_CLIENT_WAIT_TIME);

  ds_http_block_auth_info_type auth_info;
  memset(&auth_info, 0, sizeof(ds_http_block_auth_info_type));
  active_request->get_auth_info(&auth_info);
  auth_info.auth_response_header = auth_str;
  active_response->notify_auth_cb(&auth_info);
}



/*==============================================================================
                        ds_http_socket - Error Handler
==============================================================================*/
void ds_http_socket::notify_error(const sint15 error, const sint15 http_status)
{
  DATA_APPSRV_MSG6(
                   MSG_LEGACY_HIGH,
                   "http_error - socket id:%d fd:%d err:%d status:%d req:%d resp:%d",
                   socket_id,
                   sockfd,
                   error,
                   http_status,
                   active_request,
                   active_response
                   );

  if(NULL != active_response)
  {
    active_response->notify_cb(error);
    ds_appsrv_free(active_request);
    ds_appsrv_free(active_response);
    active_request = NULL;
    active_response = NULL;
  }
  else if(NULL != active_request)
  {
    active_request->notify_error(error);
    ds_appsrv_free(active_request);
    active_request = NULL;
  }
}



void ds_http_socket::socket_error(const sint15 error)
{
  notify_error(error, 0);

  socket_status = SOCKET_STATUS_CLOSE;
  http_close();
}



void ds_http_socket::status_error(const sint15 error, const sint15 http_status)
{
  notify_error(error, http_status);

  socket_status = SOCKET_STATUS_CONNECTED;
  http_connected();
}


