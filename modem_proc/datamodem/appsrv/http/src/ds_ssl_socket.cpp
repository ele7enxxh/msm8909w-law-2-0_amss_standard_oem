/*==============================================================================

                              ds_ssl_socket.cpp

GENERAL DESCRIPTION
  HTTP ssl socket manager

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/16    ml     Multiple SSL version support
11/02/15    ml     HTTP Post chunked support
09/22/15    ml     IPv6 support
05/20/15    ml     Use memory from modem heap
02/19/15    ml     Redirect requests to a new socket if current socket is in close wait status.
01/02/15    ml     Added timeout to handle unresolved block status.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_ssl_socket.h"

#include "ds_http_msg.h"
#include "ds_http_request.h"
#include "ds_http_data_mgr.h"
#include "ds_http_types.h"
#include "ds_appsrv_mem.h"

extern "C"
{
  #include "ds_ssl_api.h"
}

#include "data_msg.h"
#include <stringl/stringl.h>

#include "ds_ASQueue.h"
#include "ds_ASList.h"

// 10 seconds
#define DS_HTTP_SSL_ALERT_CLIENT_WAIT_TIME 10000

/*==============================================================================
                                  Constants
==============================================================================*/
static const uint32 MAX_ACTIVE_SSL_SOCKET = 3;



/*==============================================================================
                              Static variables
==============================================================================*/
static sint15         ssl_client_id = -1;
static ds_ssl_socket* active_ssl_socket[MAX_ACTIVE_SSL_SOCKET];



/*==============================================================================
                              Static structures
==============================================================================*/
struct ds_ssl_msg
{
  const sint15                  ssl_session_id;
  const ds_ssl_status_enum_type status;
  ds_ssl_status_info_type       status_info;
  bool                          status_info_set;

  ds_ssl_msg(const sint15 session_id, const ds_ssl_status_enum_type stat, ds_ssl_status_info_type* stat_info)
  : ssl_session_id(session_id), status(stat), status_info_set(false)
  {
    if(NULL != stat_info)
    {
      memscpy(&status_info, sizeof(ds_ssl_status_info_type), stat_info, sizeof(ds_ssl_status_info_type));
      status_info_set = true;
    }
  }

  ds_ssl_status_info_type* get_status_info()
  {
    if(status_info_set)
      return &status_info;
    else
      return NULL;
  }
};



/*==============================================================================
                              Static functions
==============================================================================*/
static void ds_ssl_cb(
                      sint15                   client_id,
                      sint15                   ssl_session_id,
                      ds_ssl_status_enum_type  status,
                      ds_ssl_status_info_type* status_info,
                      void*                    user_data_ptr
                      );
static void ds_ssl_msg_handler(uint32* data_ptr);

static ds_ssl_socket* find_ssl_socket_by_ssl_session_id(const sint15 ssl_session_id);
static bool add_ssl_socket(ds_ssl_socket* ssl_socket);
static void remove_ssl_socket(ds_ssl_socket* ssl_socket);
static ds_ssl_ver_enum_type convert_ssl_ver_from_config(ds_http_ssl_version_e_type ssl_version);


/*==============================================================================
                            Function definitions
==============================================================================*/

/*==============================================================================
                              ds_ssl_socket
==============================================================================*/
ds_ssl_socket::ds_ssl_socket(
                             ds_http_request*            request,
                             const sint15                dsnet_handle,
                             const dss_iface_id_type     iface,
                             const ds_http_ip_addr_info& addr_info,
                             sint15*                     err
                             )
: ds_http_socket(request, dsnet_handle, iface, addr_info, err),
  ssl_session_id(-1),
  ssl_status(SSL_STATUS_DISABLED),
  ssl_version(DS_HTTP_SSL_VER_DEFAULT)
{
  if(DS_HTTP_ERROR_NONE != *err)
    return;

  ssl_version = request->ssl_config_info.ssl_version;
}

ds_ssl_socket::ds_ssl_socket(
                             ds_http_socket* prev_socket,
                             sint15*         err
                             )
: ds_http_socket(prev_socket, err),
  ssl_session_id(-1),
  ssl_status(SSL_STATUS_DISABLED),
  ssl_version(DS_HTTP_SSL_VER_DEFAULT)
{
  if(DS_HTTP_ERROR_NONE != *err || NULL == active_request)
    return;

  ssl_version = active_request->ssl_config_info.ssl_version;
}



ds_ssl_socket::~ds_ssl_socket()
{ }



sint15 ds_ssl_socket::get_ssl_session_id() const
{
  return ssl_session_id;
}


void ds_ssl_socket::add_request(ds_http_request* request)
{
  // Check if SSL version mismatch for same host
  if( convert_ssl_ver_from_config(request->ssl_config_info.ssl_version) != convert_ssl_ver_from_config(ssl_version) )
  {
    DATA_APPSRV_MSG3(
                     MSG_LEGACY_ERROR,
                     "ssl::add_request - req %d: get sess config failed or SSL version mismatch %d, %d",
                     request->request_id,
                     ssl_version,
                     request->ssl_config_info.ssl_version
                     );
    request->notify_error(DS_HTTP_ERROR_SSL);
    ds_appsrv_free(request);
    return;
  }

  ds_http_socket::add_request(request);
}



/*==============================================================================
                        ds_ssl_socket - Event handler
==============================================================================*/
void ds_ssl_socket::timeout_event()
{
  DATA_APPSRV_MSG3(
                   MSG_LEGACY_MED,
                   "ds_ssl_socket::timeout_event - Socket %d: timeout triggered in state:%d/%d",
                   socket_id,
                   socket_status,
                   ssl_status
                   );

  switch(socket_status)
  {
    case SOCKET_STATUS_SSL_CONNECT:
    {
      resolve_ssl_suspension(false);
    }
    break;

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
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_ssl_socket::timeout_event - timeout triggered in invalid state %d", socket_status);
      socket_error(DS_HTTP_ERROR_TIMEOUT);
    }
    break;
  }
}



void ds_ssl_socket::socket_event(const uint32 event_mask)
{
  socket_status_e_type prev_socket_status = socket_status;
  ssl_status_e_type    prev_ssl_status    = ssl_status;

  if(0 != rex_get_timer(&timeout_timer))
    rex_clr_timer(&timeout_timer);

  if(SSL_STATUS_DISABLED != ssl_status)
  {
    ds_ssl_foward_sockevent(sockfd, event_mask);
  }

  switch(socket_status)
  {
    case SOCKET_STATUS_SOCKET_CONNECT:
    {
      if ((event_mask & DS_WRITE_EVENT) != 0)
      {
        http_connect();
        break;
      }
    }
    break;

    case SOCKET_STATUS_SSL_CONNECT:
    {
      // Do nothing, wait for DS_SSL_STATUS_CONNECTED
      break;
    }
    break;

    case SOCKET_STATUS_SEND:
    {
      if ((event_mask & DS_WRITE_EVENT) != 0)
      {
        http_send();
        break;
      }
    }
    break;

    case SOCKET_STATUS_RECEIVE:
    {
      // Do nothing, wait for DS_SSL_STATUS_READ_EVENT
      break;
    }
    break;

    case SOCKET_STATUS_CLOSE:
    {
      if ((event_mask & DS_CLOSE_EVENT) != 0 && SSL_STATUS_DISABLED == ssl_status)
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

  DATA_APPSRV_MSG6(
                   MSG_LEGACY_HIGH,
                   "ds_ssl_socket::socket_event - socket:%d status:%d/%d -> %d/%d event_mask:%d",
                   socket_id,
                   prev_socket_status,
                   prev_ssl_status,
                   socket_status,
                   ssl_status,
                   event_mask
                   );
}



void ds_ssl_socket::ssl_event(const ds_ssl_status_enum_type status, void* status_info)
{
  socket_status_e_type prev_socket_status = socket_status;
  ssl_status_e_type    prev_ssl_status    = ssl_status;

  if(0 != rex_get_timer(&timeout_timer))
    rex_clr_timer(&timeout_timer);

  switch(status)
  {
    case DS_SSL_STATUS_CONNECTED:
    {
      ssl_status = SSL_STATUS_CONNECTED;
      socket_status = SOCKET_STATUS_SEND;
      active_request->reset_send_status();
      http_send();
    }
    break;

    case DS_SSL_STATUS_DISCONNECTED:
    {
      ssl_status     = SSL_STATUS_DISABLED;
      socket_status  = SOCKET_STATUS_CLOSE;
      ssl_session_id = -1;

      remove_ssl_socket(this);
      http_close();
    }
    break;

    case DS_SSL_STATUS_READ_EVENT:
    {
      if(SOCKET_STATUS_RECEIVE == socket_status)
        http_receive();
    }
    break;

    case DS_SSL_STATUS_ALERT_SUSPEND:
    {
      if(active_request->is_cancelled())
      {
        DATA_APPSRV_MSG0(MSG_LEGACY_MED, "ssl_event - Request is cancelled; aborting ssl block");
        resolve_ssl_suspension(false);
        break;
      }

      ds_http_ssl_cert_alert_e_type* cert_alert = (ds_http_ssl_cert_alert_e_type*)status_info;
      if(NULL == cert_alert)
      {
        DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ssl_event - Failed to cast certification alert");
        socket_error(DS_HTTP_ERROR_SSL);
        break;
      }

      // Socket will wait 10 seconds for the client to respond how to handle SSL alert.
      DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "ssl_event - Setting timeout timer for %d seconds", DS_HTTP_SSL_ALERT_CLIENT_WAIT_TIME/1000);
      rex_set_timer(&timeout_timer, DS_HTTP_SSL_ALERT_CLIENT_WAIT_TIME);
      ds_http_notify_ssl_alert_cb(active_request->session_id, active_request->request_id, *cert_alert);
    }
    break;

    case DS_SSL_STATUS_TCP_CLOSE:
    {
      if(SSL_STATUS_DISCONNECT_WAIT == ssl_status)
      {
        break;
      }
    }                               // else fallthrough
    case DS_SSL_STATUS_ALERT_FATAL: // fallthrough
    case DS_SSL_STATUS_FATAL_ERROR:
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ssl_event - SSL failure");
      socket_error(DS_HTTP_ERROR_SSL);
    }
    break;

    case DS_SSL_STATUS_ALERT_WARNING:
    {
      ds_ssl_status_info_type* alert_info = (ds_ssl_status_info_type*)status_info;

      if(NULL == alert_info || DS_SSL_ALERT_CLOSE_NOTIFY == alert_info->alert_mask)
      {
        DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ssl_event - NULL alert info or SSL closed by server");
        socket_error(DS_HTTP_ERROR_SSL);
      }
    }
    break;

    default:
      break;
  }

  DATA_APPSRV_MSG6(
                   MSG_LEGACY_HIGH,
                   "ssl_event - socket:%d status:%d/%d -> %d/%d ssl status:%d",
                   socket_id,
                   prev_socket_status,
                   prev_ssl_status,
                   socket_status,
                   ssl_status,
                   status
                   );
}



void ds_ssl_socket::resolve_ssl_suspension(const bool cont)
{
  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "resolve_ssl_suspension - action %d",cont);
  ds_ssl_control_type control = (cont ? DS_SSL_CONTROL_CONTINUE : DS_SSL_CONTROL_ABORT);

  if(0 != rex_get_timer(&timeout_timer))
  {
    rex_clr_timer(&timeout_timer);
  }

  if(DS_SSL_SUCCESS != ds_ssl_session_user_control(ssl_session_id, control))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "resolve_ssl_suspension - SSL control failed");
    socket_error(DS_HTTP_ERROR_SSL);
  }
}




/*==============================================================================
                          ds_ssl_socket - Utility
==============================================================================*/
void ds_ssl_socket::init_values()
{
  ds_http_socket::init_values();
  ssl_status = SSL_STATUS_DISABLED;
  ssl_session_id = -1;
}



sint15 ds_ssl_socket::http_write(const sint15 sockfd, const uint8* buffer, const uint32 buffer_size, sint15* err)
{
  return ds_ssl_session_write(ssl_session_id, buffer, buffer_size);
}



sint15 ds_ssl_socket::http_read(const sint15 sockfd, uint8* buffer, const uint32 buffer_size, sint15* err)
{
  return ds_ssl_session_read(ssl_session_id, buffer, buffer_size);
}



bool ds_ssl_socket::check_socket_error(
                                       const sint15 retval,
                                       const sint15 sock_err,
                                       const sint31 event_mask,
                                       const sint15 internal_err
                                       )
{
  switch(retval)
  {
    case DS_SSL_INVALID_ARG:    // fallthrough
    case DS_SSL_NOT_AVAILABLE:  // fallthrough
    case DS_SSL_ERROR_UNKNOW:
    {
      socket_error(DS_HTTP_ERROR_SSL);
    }                           // fallthrough
    case DS_SSL_WOULDBLOCK:
    {
      return true;
    }
    break;

    case DS_SSL_SUCCESS:        // fallthrough
    default:
    {
      return false;
    }
    break;
  }
}



/*==============================================================================
                              ds_ssl_socket - Flow
==============================================================================*/
void ds_ssl_socket::socket_connected()
{
  socket_status = SOCKET_STATUS_SSL_CONNECT;
  ssl_connect();
}



bool ds_ssl_socket::close_socket()
{
  DATA_APPSRV_MSG3(MSG_LEGACY_LOW, "ds_ssl_socket::close_socket - %d : socket %d ssl_status %d", socket_id, sockfd, ssl_status);

  switch(ssl_status)
  {
    case SSL_STATUS_DISABLED:
    {
      return ds_http_socket::close_socket();
    }
    break;

    case SSL_STATUS_ENABLED: // fallthrough
    case SSL_STATUS_CONNECTED:
    {
      ssl_close();
      ssl_status = SSL_STATUS_DISCONNECT_WAIT;
    }                        // fallthrough
    case SSL_STATUS_DISCONNECT_WAIT:
    {
      DATA_APPSRV_MSG2(MSG_LEGACY_LOW, "ds_ssl_socket::close_socket - waiting SSL close %d : socket %d", socket_id, sockfd);
      return false;
    }
    break;

    default:
      break;
  }
  return false;
}



void ds_ssl_socket::ssl_connect()
{
  ds_ssl_config_info_type ssl_config;

  if(SSL_STATUS_DISABLED != ssl_status)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "ssl_connect - SSL already requested");
    return;
  }

  memset(&ssl_config, 0, sizeof(ds_ssl_config_info_type));
  memscpy(
          ssl_config.server_name,
          DS_SSL_SERVER_NAME_MAX_SIZE,
          active_request->get_hostname(),
          DS_HTTP_HOSTNAME_MAX_LEN
          );

  ssl_config.ssl_ver     = convert_ssl_ver_from_config(ssl_version);
  ssl_config.server_port = port;

  DATA_APPSRV_MSG3(MSG_LEGACY_LOW, "ssl_connect - socket %d ssl %d port %d", sockfd, ssl_client_id, port);

  ssl_session_id = ds_ssl_session_open(ssl_client_id, sockfd, &ssl_config);
  if(0 > ssl_session_id)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ssl_connect - Received invalid SSL session ID %d", ssl_session_id);
    socket_error(DS_HTTP_ERROR_SSL);
    return;
  }

  ssl_status = SSL_STATUS_ENABLED;

  if(!add_ssl_socket(this))
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ssl_connect - Failed to add to ssl list %d", ssl_session_id);
    socket_error(DS_HTTP_ERROR_SSL);
    return;
  }
}



void ds_ssl_socket::ssl_close()
{
  DATA_APPSRV_MSG3(MSG_LEGACY_MED, "ssl_close - %d : socket %d ssl %d", socket_id, sockfd, ssl_client_id);
  ds_ssl_session_close(ssl_session_id);
}



/*==============================================================================
                                SSL Init
==============================================================================*/
void ds_http_init_ssl_session()
{
  ssl_client_id = ds_ssl_register_status_cback(&ds_ssl_cb, NULL);
  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "init_ssl_session - ssl client id %d", ssl_client_id);

  for(uint32 i = 0; i < MAX_ACTIVE_SSL_SOCKET; ++i)
  {
    active_ssl_socket[i] = NULL;
  }
}




/*==============================================================================
                              Static fcn
==============================================================================*/
void ds_ssl_cb(
               sint15                   client_id,
               sint15                   ssl_session_id,
               ds_ssl_status_enum_type  status,
               ds_ssl_status_info_type* status_info,
               void*                    user_data_ptr
               )
{
  // check nethandle
  DATA_APPSRV_MSG3(MSG_LEGACY_MED, "ds_ssl_cb - %d, %d, %d", client_id, ssl_session_id, status);
  if (client_id != ssl_client_id)
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "Invalid client id - %d, %d", client_id, ssl_client_id);
    return;
  }

  ds_ssl_msg* ssl_msg = new(ds_appsrv_alloc<ds_ssl_msg>(), APPSRV_MEM) ds_ssl_msg(ssl_session_id, status, status_info);
  if(NULL == ssl_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_ssl_cb - Failed to allocate memory for ssl_msg");
    return;
  }

  if(!ds_http_msg_post(&ds_ssl_msg_handler, (uint32*)ssl_msg))
  {
    ds_appsrv_free(ssl_msg);
  }
}



void ds_ssl_msg_handler(uint32* data_ptr)
{
  ds_ssl_msg* ssl_msg = (ds_ssl_msg*) data_ptr;
  if (NULL == ssl_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_ssl_msg_handler, illegal ssl_msg");
    return;
  }
  DATA_APPSRV_MSG2(MSG_LEGACY_LOW, "ds_ssl_msg_handler - %d, %d", ssl_msg->ssl_session_id, ssl_msg->status);

  ds_ssl_socket* ssl_socket = find_ssl_socket_by_ssl_session_id(ssl_msg->ssl_session_id);
  if (NULL != ssl_socket)
  {
    ssl_socket->ssl_event(ssl_msg->status, ssl_msg->get_status_info());

    // Clean up socket in data mgr if socket is closed
    if(ssl_socket->socket_closed())
    {
      ds_http_handle_socket_event(ssl_socket->get_nethandle(), ssl_socket->sockfd, 0);
      ssl_socket = NULL; // pointer will be deallocated elsewhere; dereference for possible error usage
    }
  }

  ds_appsrv_free(ssl_msg);
}



ds_ssl_socket* find_ssl_socket_by_ssl_session_id(const sint15 ssl_session_id)
{
  for(uint32 i = 0; i < MAX_ACTIVE_SSL_SOCKET; ++i)
  {
    if(NULL != active_ssl_socket[i] &&
       ssl_session_id == active_ssl_socket[i]->get_ssl_session_id())
    {
      return active_ssl_socket[i];
    }
  }

  return NULL;
}



bool add_ssl_socket(ds_ssl_socket* ssl_socket)
{
  for(uint32 i = 0; i < MAX_ACTIVE_SSL_SOCKET; ++i)
  {
    if(NULL == active_ssl_socket[i])
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "add_ssl_socket - adding ssl socket");
      active_ssl_socket[i] = ssl_socket;
      return true;
    }
  }

  DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "add_ssl_socket - failed to add ssl socket");
  return false;
}



void remove_ssl_socket(ds_ssl_socket* ssl_socket)
{
  for(uint32 i = 0; i < MAX_ACTIVE_SSL_SOCKET; ++i)
  {
    if(ssl_socket == active_ssl_socket[i])
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "remove_ssl_socket - removing active socket");
      active_ssl_socket[i] = NULL;
      return;
    }
  }
}



ds_ssl_ver_enum_type convert_ssl_ver_from_config(ds_http_ssl_version_e_type ssl_version)
{
  switch(ssl_version)
  {
    case DS_HTTP_SSL_VER_10:
      return DS_SSL_VER_10;

    case DS_HTTP_SSL_VER_20:
      return DS_SSL_VER_20;

    case DS_HTTP_SSL_VER_30:
      return DS_SSL_VER_30;

    case DS_HTTP_SSL_VER_TLS10:
      return DS_SSL_VER_TLS10;

    case DS_HTTP_SSL_VER_TLS11:
      return DS_SSL_VER_TLS11;

    case DS_HTTP_SSL_VER_TLS12:
      return DS_SSL_VER_TLS12;

    case DS_HTTP_SSL_VER_TLS13:
    // fallthrough
    //   return DS_SSL_VER_TLS13;

    case DS_HTTP_SSL_VER_DEFAULT: // fallthrough
    default:
      return DS_SSL_VER_DEFAULT; // SSL module to select it's highest supported version
  }
}

