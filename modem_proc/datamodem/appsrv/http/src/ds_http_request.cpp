/*==============================================================================

                              ds_http_request.cpp

GENERAL DESCRIPTION
  HTTP request information object

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/16    ml     Session option to handle response size > 100k
02/20/16    ml     Multiple SSL version support
11/02/15    ml     Permanent redirect
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/29/15    ml     Resolve profile id if it is default before attempting bringup
09/29/15    ml     IPv6 URI support
06/11/15    ml     Cookie support
04/16/15    ml     Response authentication support.
01/06/15    ml     HTTP Digest auth-int support.
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_request.h"

#include "ds_http_data_mgr.h" // ds_http_notify_session_cb
#include "ds_http_utility.h"

#include "ds_ASStringStream.h"
#include "ds_ASUtility.h"

#include <cctype>
#include <cstdlib>
#include <stringl/stringl.h>

#include "data_msg.h"
extern "C"
{
  #include "md5.h"
}

using namespace Appsrv::Utils;

/*==============================================================================
                              Static variables
==============================================================================*/

static const uint8  DS_HTTP_HEADER_DELIMITER[]   = "\r\n\r\n";
static const uint32 DS_HTTP_HEADER_DELIMITER_LEN = 4;
static const uint32 MAX_REDIRECT_COUNT           = 5;



/*==============================================================================
                            ds_http_chunked_content
==============================================================================*/
ds_http_chunked_content::ds_http_chunked_content(const ds_http_chunked_content_info_s_type* chunked_content_info)
: is_last_chunk(false)
{
  if(NULL == chunked_content_info)
    return;

  is_last_chunk = (TRUE == chunked_content_info->is_last_chunk) ? true : false;

  if(NULL != chunked_content_info->content_info.content &&
     0    != chunked_content_info->content_info.content_size)
  {
    char hex_str[5];
    memset(hex_str, 0, 5);
    uint16 hex_str_len = ultohex(chunked_content_info->content_info.content_size, hex_str);

    chunked_content.append(hex_str, hex_str_len);
    chunked_content.append("\r\n", 2);
    chunked_content.append(chunked_content_info->content_info.content, chunked_content_info->content_info.content_size);
    chunked_content.append("\r\n", 2);
  }

  if(is_last_chunk)
  {
    // Append terminating chunk
    chunked_content.append("0\r\n\r\n", 5);
  }
}




/*==============================================================================
                              ds_http_request
==============================================================================*/
ds_http_request::ds_http_request(
                                 const uint32                 sid,
                                 const uint32                 rid,
                                 const char*                  uri_str,
                                 const ds_http_header_s_type* header,
                                 uint16                       rsv_src_port,
                                 bool                         is_chunked,
                                 sint15*                      request_errno
                                 )
: session_id(sid),
  request_id(rid),
  reserved_src_port(rsv_src_port),
  chunked_transfer(is_chunked),
  send_status(REQUEST_SEND_REQUEST),
  redirect_counter(0),
  bytes_written(0),
  cancelled(false),
  last_chunk_received(false)
{
  if(!parse_uri(uri_str, uri_info))
  {
    *request_errno = DS_HTTP_ERROR_INVALID_URI;
    return;
  }
  memset(&credential, 0, sizeof(ds_http_credential_s_type));
  memset(&ssl_config_info, 0, sizeof(ds_http_ssl_config_s_type));

  // Get SSL info if HTTPS
  if( is_ssl_protocol() )
  {
    ds_http_session_config_value_u_type session_config;
    memset(&session_config, 0, sizeof(ds_http_session_config_value_u_type));

    if( !ds_http_get_session_config(session_id, DS_HTTP_SESSION_CONFIG_SSL, &session_config) )
    {
      *request_errno = DS_HTTP_ERROR_CONFIG;
      return;
    }

    memscpy(&ssl_config_info, sizeof(ds_http_ssl_config_s_type), &(session_config.ssl_config), sizeof(ds_http_ssl_config_s_type));
  }

  // Check if the request URI is permanent redirect
  check_and_update_redirect_uri(uri_info);

  // Create request header
  create_header_str(header);
}



ds_http_request::~ds_http_request()
{ }



/*==============================================================================
                      ds_http_request - Utility
==============================================================================*/
const char* ds_http_request::get_hostname() const
{
  return uri_info.hostname;
}



sint15 ds_http_request::get_port() const
{
  return uri_info.port;
}



bool ds_http_request::is_ssl_protocol() const
{
  return (DS_HTTP_PROTOCOL_HTTPS == uri_info.protocol);
}


bool ds_http_request::is_cancelled() const
{
  return cancelled;
}


void ds_http_request::get_auth_info(ds_http_block_auth_info_type* auth_data) const
{
  auth_data->realm = auth_generator.realm.c_str();
  auth_data->nonce = auth_generator.nonce.c_str();
}


const ds_http_credential_s_type* ds_http_request::get_credential() const
{
  return &credential;
}


const ds_http_auth_generator* ds_http_request::get_request_auth_info() const
{
  return &auth_generator;
}


const char* ds_http_request::get_uri_path() const
{
  return uri_info.path;
}


void ds_http_request::create_request_str(const char* request_method)
{
  request_str.clear();
  request_str.append(request_method);
  request_str.append(uri_info.path);
  request_str.append(" HTTP/1.1\r\nHost: "); // host not in header_str since it could change by redirect
  request_str.append(uri_info.hostname);

  // Append port to Host header if not default port
  if( ( uri_info.protocol == DS_HTTP_PROTOCOL_HTTP  && uri_info.port != DS_HTTP_DEFAULT_PORT  ) ||
      ( uri_info.protocol == DS_HTTP_PROTOCOL_HTTPS && uri_info.port != DS_HTTPS_DEFAULT_PORT ) )
  {
    char port_str[DS_UTILS_SINT31_MAX_LEN+1];
    memset(port_str, 0, DS_UTILS_SINT31_MAX_LEN+1);
    uint16 len = sltostr(uri_info.port, port_str);

    request_str.append(":");
    request_str.append(port_str, len);
  }
}



void ds_http_request::create_header_str(const ds_http_header_s_type* header_info)
{
  append_header("Connection", "Keep-Alive");

  if(chunked_transfer)
  {
    append_header("Transfer-Encoding", "chunked");
  }

  if(NULL == header_info)
    return;

  append_header("Accept", header_info->accept);
  append_header("Accept-Charset", header_info->accept_charset);
  append_header("Accept-Encoding", header_info->accept_encoding);
  append_header("Accept-Language", header_info->accept_language);
  append_header("User-Agent", header_info->user_agent);
  append_header("UAProf", header_info->ua_profile);
  append_header("Referer", header_info->referer);
  append_header("Content-Type", header_info->content_type);

  for(uint32 i = 0; i < (uint32)header_info->num_cust_headers; ++i)
  {
    append_header(header_info->cust_header_list[i].name, header_info->cust_header_list[i].value);
  }

  uint32 auth_len = strlen(header_info->authorization);
  if(0 != auth_len)
  {
    auth_header_str.append("\r\nAuthorization: ");
    auth_header_str.append(header_info->authorization);
  }

  create_cookie_header_str(header_info->cookie, header_info->num_cookies);
}


void ds_http_request::create_cookie_header_str(const ds_http_cookie_name_s_type cookie[], uint16 num_cookies)
{
  if(0 == num_cookies)
    return;
  else if(num_cookies >= DS_HTTP_COOKIE_MAX)
    num_cookies = DS_HTTP_COOKIE_MAX;

  cookie_header_str.clear();
  cookie_header_str.append("\r\nCookie: ");

  cookie_header_str.append(cookie[0].name);
  cookie_header_str.append("=");
  cookie_header_str.append(cookie[0].value);

  for(uint16 i = 1; i < num_cookies; ++i)
  {
    cookie_header_str.append("; ");
    cookie_header_str.append(cookie[i].name);
    cookie_header_str.append("=");
    cookie_header_str.append(cookie[i].value);
  }
}


void ds_http_request::append_header(const char* header_name, const char* header_value)
{
  if(NULL == header_name || NULL == header_value)
    return;

  uint32 nsize = strlen(header_name);
  uint32 vsize = strlen(header_value);

  if(
     0 < nsize &&
     0 < vsize &&
     !has_carriage_return(header_name, nsize) &&
     !has_carriage_return(header_value, vsize)
     )
  {
    header_str.append("\r\n");
    header_str.append(header_name, nsize);
    header_str.append(": ");
    header_str.append(header_value, vsize);
  }
}



void ds_http_request::append_content(const uint8* content_data, const uint32 content_size)
{
  char buffer[11] = {0};
  append_header("Content-Length", uitoa(content_size, buffer));
  if(NULL != content_data && 0 != content_size)
  {
    content.append(content_data, content_size);
  }
}



void ds_http_request::notify_error(const sint15 http_error) const
{
  if(cancelled)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_MED, "notify_error - Request is cancelled; not notifying client");
    return;
  }

  ds_http_notify_session_cb(session_id, request_id, http_error, NULL);
}


void ds_http_request::notify_ssl_alert(uint8 alert_level, uint32 alert_mask)
{
  if( NULL == ssl_config_info.ssl_alert_cb_fcn ||
      0    == (ssl_config_info.ssl_alert_notification_mask & alert_level) )
  {
    return;
  }

  ds_http_ssl_alert_info_s_type alert_info;
  memset(&alert_info, 0, sizeof(ds_http_ssl_alert_info_s_type));
  alert_info.alert_level = alert_level;
  alert_info.alert_mask  = alert_mask;

  ssl_config_info.ssl_alert_cb_fcn(session_id, request_id, &alert_info);
}



bool ds_http_request::check_buffer_error() const
{
  return (header_str.error()      ||
          content.error()         ||
          request_str.error()     ||
          auth_header_str.error() ||
          cookie_header_str.error());
}


void ds_http_request::cancel_request()
{
  cancelled = true;
}


/*==============================================================================
                ds_http_request - buffer & status management
==============================================================================*/
const uint8* ds_http_request::get_send_buffer_content() const
{
  switch(send_status)
  {
    case REQUEST_SEND_REQUEST:
      return request_str.content() + bytes_written;

    case REQUEST_SEND_HEADER:
      return header_str.content() + bytes_written;

    case REQUEST_SEND_COOKIE_HEADER:
      return cookie_header_str.content() + bytes_written;

    case REQUEST_SEND_AUTH_HEADER:
      return auth_header_str.content() + bytes_written;

    case REQUEST_SEND_HEADER_DELIM:
      return DS_HTTP_HEADER_DELIMITER + bytes_written;

    case REQUEST_SEND_CONTENT:
      return content.content() + bytes_written;

    case REQUEST_SEND_CONTENT_WAIT: // fallthrough
    case REQUEST_SEND_COMPLETE:     // fallthrough
    default:
      return NULL;
  }
}



uint32 ds_http_request::get_send_buffer_size() const
{
  switch(send_status)
  {
    case REQUEST_SEND_REQUEST:
      return request_str.size() - bytes_written;

    case REQUEST_SEND_HEADER:
      return header_str.size() - bytes_written;

    case REQUEST_SEND_COOKIE_HEADER:
      return cookie_header_str.size() - bytes_written;

    case REQUEST_SEND_AUTH_HEADER:
      return auth_header_str.size() - bytes_written;

    case REQUEST_SEND_HEADER_DELIM:
      return DS_HTTP_HEADER_DELIMITER_LEN - bytes_written;

    case REQUEST_SEND_CONTENT:
      return content.size() - bytes_written;

    case REQUEST_SEND_CONTENT_WAIT: // fallthrough
    case REQUEST_SEND_COMPLETE:     // fallthrough
    default:
      return 0;
  }
}


// Returns true if no error
void ds_http_request::reset_send_status()
{
  send_status   = REQUEST_SEND_REQUEST;
  bytes_written = 0;
}



void ds_http_request::update_send_status(const uint32 written)
{
  bytes_written += written;

  ds_http_send_status prev_send_status = send_status;
  uint32              prev_written     = bytes_written;

  switch(send_status)
  {
    case REQUEST_SEND_REQUEST:
    {
      if(request_str.size() <= bytes_written)
      {
        send_status   = REQUEST_SEND_HEADER;
        bytes_written = 0;
      }
      else
        break;
    } // fallthrough

    case REQUEST_SEND_HEADER:
    {
      if(header_str.size() <= bytes_written)
      {
        send_status   = REQUEST_SEND_COOKIE_HEADER;
        bytes_written = 0;
      }
      else
        break;
    } // fallthrough

    case REQUEST_SEND_COOKIE_HEADER:
    {
      if(cookie_header_str.size() <= bytes_written)
      {
        send_status   = REQUEST_SEND_AUTH_HEADER;
        bytes_written = 0;
      }
      else
        break;
    } // fallthrough

    case REQUEST_SEND_AUTH_HEADER:
    {
      if(auth_header_str.size() <= bytes_written)
      {
        send_status   = REQUEST_SEND_HEADER_DELIM;
        bytes_written = 0;
      }
      else
        break;
    } // fallthrough

    case REQUEST_SEND_HEADER_DELIM:
    {
      if(DS_HTTP_HEADER_DELIMITER_LEN <= bytes_written)
      {
        send_status   = REQUEST_SEND_CONTENT;
        bytes_written = 0;
      }
      else
        break;
    } // fallthrough

    case REQUEST_SEND_CONTENT:
    case REQUEST_SEND_CONTENT_WAIT:
    {
      if(content.size() <= bytes_written)
      {
        if(!is_chunked_transfer() || last_chunk_received)
        {
          DATA_APPSRV_MSG1(MSG_LEGACY_HIGH, "update_send_status - content send complete, total size:%d", content.size());
          send_status = REQUEST_SEND_COMPLETE;
        }
        else
        {
          send_status = REQUEST_SEND_CONTENT_WAIT;
        }
      }
    }
    break;

    case REQUEST_SEND_COMPLETE: // fallthrough
    default:
      break;
  }

  DATA_APPSRV_MSG4(
                   MSG_LEGACY_MED,
                   "update_status - status:%d written:%d -> status:%d written:%d",
                   prev_send_status,
                   prev_written,
                   send_status,
                   bytes_written
                   );
}




bool ds_http_request::send_complete() const
{
  return (REQUEST_SEND_COMPLETE == send_status);
}



bool ds_http_request::update_redirect(const ds_http_request_uri& redirect_uri)
{
  if(MAX_REDIRECT_COUNT <= ++redirect_counter)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "status_redirect - Redirect loop");
    return false;
  }

  // ds_http_request_uri redirect_uri_info;
  // if(!ds_http_parse_uri(redirect_uri, redirect_uri_info))
  // {
  //   DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "status_redirect - Redirect URI decode failed");
  //   *err = DS_HTTP_ERROR_HTTP_HEADER;
  //   return false;
  // }

  // Cache permanent redirect URI
  // if(301 == http_status || 308 == http_status)
  // {
  //   add_redirect_cache(uri_info, redirect_uri_info);
  // }

  uri_info = redirect_uri;

  auth_header_str.clear(); // clear authentication info (no change if none)
  create_request_str();

  if(header_str.error())
    return false;

  return true;
}



/*==============================================================================
                    ds_http_request - HTTP Authentication
==============================================================================*/
bool ds_http_request::create_auth_header_str(const ds_http_auth_info_s_type* client_auth_info)
{
  ASString err_info("");

  do
  {
    if(NULL == client_auth_info || 0 == client_auth_info->credential.username_len)
    {
      err_info = "NULL or invalid param";
      break;
    }

    memset(&credential, 0, sizeof(ds_http_credential_s_type));
    memscpy(
            &credential,
            sizeof(ds_http_credential_s_type),
            &client_auth_info->credential,
            sizeof(ds_http_credential_s_type)
            );

    if(!auth_generator.generate_response(
                                         &(client_auth_info->credential),
                                         get_request_method_str(),
                                         uri_info.path,
                                         content.non_const_content(),
                                         content.size(),
                                         auth_header_str
                                         ))
    {
      err_info = "Failed to generate auth header string";
      break;
    }

    if(!add_cust_auth_param(client_auth_info))
    {
      err_info = "Failed to append custom params";
      break;
    }

    if(auth_header_str.error())
    {
      err_info = "auth_header buffer error";
      break;
    }

    create_cookie_header_str(client_auth_info->cookie, client_auth_info->num_cookies);
    if(cookie_header_str.error())
    {
      err_info = "cookie_header buffer error";
      break;
    }
  } while(0);

  if( !err_info.empty() )
  {
    DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_ERROR, "create_auth_header_str - %s", err_info.c_str());
    return false;
  }
  return true;
}


bool ds_http_request::decode_auth_header(const char* auth_str, uint32 http_status)
{
  return auth_generator.parse_auth_header(auth_str, http_status);
}



/* Note: Assumes caller checked that client_auth_info is not NULL */
bool ds_http_request::add_cust_auth_param(const ds_http_auth_info_s_type* client_auth_info)
{
  if(0 == client_auth_info->num_cust_auth_param)
    return true;

  if(DS_HTTP_CUST_PARAM_MAX_SIZE <= client_auth_info->num_cust_auth_param)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "add_cust_auth_param - Invalid additional auth param size");
    return false;
  }

  for(uint32 i = 0; i < client_auth_info->num_cust_auth_param; ++i)
  {
    uint32 name_len  = strlen(client_auth_info->cust_auth_param[i].name);
    uint32 value_len = strlen(client_auth_info->cust_auth_param[i].value);

    if(0 == name_len || 0 == value_len)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "add_cust_auth_param - Invalid auth param value");
      return false;
    }

    auth_header_str.append(",");
    auth_header_str.append(client_auth_info->cust_auth_param[i].name, name_len);
    auth_header_str.append("=");
    auth_header_str.append(client_auth_info->cust_auth_param[i].value, value_len);
  }
  return true;
}



void ds_http_request::append_chunked_content(const ds_http_chunked_content& chunked_content_info)
{
  DATA_APPSRV_MSG2(
                   MSG_LEGACY_HIGH,
                   "append_chunked_content - chunk received size:%d last:%d",
                   chunked_content_info.chunked_content.size(),
                   chunked_content_info.is_last_chunk
                   );

  if(!chunked_transfer || last_chunk_received || cancelled)
  {
    DATA_APPSRV_MSG3(
                     MSG_LEGACY_ERROR,
                     "append_chunked_content - Request invalid chunk:%d last:%d cancel:%d",
                     chunked_transfer,
                     last_chunk_received,
                     cancelled
                     );
    return;
  }

  if(chunked_content_info.is_last_chunk)
    last_chunk_received = true;

  if(REQUEST_SEND_CONTENT_WAIT == send_status)
    send_status = REQUEST_SEND_CONTENT;

  content.append(chunked_content_info.chunked_content);
}



bool ds_http_request::is_chunked_transfer() const
{
  return chunked_transfer;
}


bool ds_http_request::is_waiting_chunk() const
{
  return REQUEST_SEND_CONTENT_WAIT == send_status;
}


bool ds_http_request::chunked_transfer_complete() const
{
  return last_chunk_received;
}





