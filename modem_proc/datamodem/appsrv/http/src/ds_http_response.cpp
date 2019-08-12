/*==============================================================================

                              ds_http_response.cpp

GENERAL DESCRIPTION
  HTTP response information object

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/16    ml     Session option to handle response size > 100k
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/30/15    ml     Fix to not fail request if cookie parsing fails
06/11/15    ml     Cookie support
05/20/15    ml     Use memory from modem heap
04/16/15    ml     Response authentication support.
01/26/15    ml     Fix for receive state changing to complete state too early.
01/07/15    ml     Fixed bug for persistent timeout being 0 when it should be default.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_response.h"

#include "ds_http_data_mgr.h" // ds_http_notify_session_cb, ds_http_notify_auth_cb
#include "ds_http_types.h"

#include "ds_ASList.h"
#include "ds_ASQueue.h"
#include "ds_ASString.h"
#include "ds_ASStringStream.h"
#include "ds_ASVector.h"
#include "ds_ASUtility.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"
#include <cstdlib>


const char DS_HTTP_VERSION_11[]                         = "http/1.1";
const char DS_HTTP_HEADER_NAME_CONTENT_LENGTH[]         = "content-length";
const char DS_HTTP_HEADER_NAME_CONNECTION[]             = "connection";
const char DS_HTTP_HEADER_NAME_CONNECTION_KEEP_ALIVE[]  = "keep-alive";
const char DS_HTTP_HEADER_NAME_TRANSFER_ENCODING[]      = "transfer-encoding";
const char DS_HTTP_HEADER_NAME_SET_COOKIE[]             = "set-cookie";
const char DS_HTTP_HEADER_VALUE_CONNECTION_KEEP_ALIVE[] = "keep-alive";
const char DS_HTTP_HEADER_VALUE_CHUNKED[]               = "chunked";

const char DS_HTTP_DELIMITER[]        = "\r\n";
const char DS_HTTP_HEADER_DELIMITER[] = "\r\n\r\n";
const char DS_HTTP_EMPTY_HEADER_STR[] = "";

const uint32 DS_HTTP_HEX_STR_MAX_SIZE          = 8;
static const uint32 DEFAULT_CONNECTION_TIMEOUT = 10;  // Default 10 seconds timeout for persistent connection
static const uint32 MAX_CONNECTION_TIMEOUT     = 300; // 5 min


ds_http_response::ds_http_response(const uint32 sid, const uint32 rid)
: session_id(sid),
  request_id(rid),
  keep_alive(true),
  http_status(0),
  content_size(0),
  timeout(DEFAULT_CONNECTION_TIMEOUT),
  receive_status(DS_HTTP_RECEIVE_HEADER),
  cancelled(false),
  header_info(NULL),
  cookie_info(NULL),
  content_received(0),
  forward_oversize(false)
{
  ds_http_session_config_value_u_type config_info;
  memset(&config_info, 0, sizeof(ds_http_session_config_value_u_type));

  if( ds_http_get_session_config(sid, DS_HTTP_SESSION_CONFIG_FORWARD_OVERSIZE_RESPONSE, &config_info) )
  {
    forward_oversize = config_info.forward_oversize_response;
  }
}



ds_http_response::~ds_http_response()
{
  if(NULL != header_info)
  {
    ds_appsrv_free(header_info);
  }
  if(NULL != cookie_info)
  {
    ds_appsrv_free(cookie_info);
  }
}



bool ds_http_response::append_response(const uint8* input, const uint32 size)
{
  ASString err_info("");

  switch(receive_status)
  {
    case DS_HTTP_RECEIVE_HEADER:
    {
      // returns true when all header content is received
      if(!header_append(input, size))
        break;

      // failed to parse header
      if(!decode_header())
      {
        err_info = "Failed to parse header";
        break;
      }
    }
    break;

    case DS_HTTP_RECEIVE_RAW:
    {
      if(!content_append(input, size))
      {
        err_info = "Content append failed";
        break;
      }
    }
    break;

    case DS_HTTP_RECEIVE_CHUNK:
    {
      if(!chunk_append(input, size))
      {
        err_info = "Chunk append failed";
        break;
      }
    }
    break;

    case DS_HTTP_RECEIVE_COMPLETE:
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "append_response - Appending to a complete response");
    }
    break;

    default:
      break;
  }

  if( !err_info.empty() )
  {
    DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_ERROR, "append_response - %s", err_info.c_str());
    return false;
  }
  return true;
}



bool ds_http_response::header_append(const uint8* input, const uint32 size)
{
  response_buffer.append(input, size);

  const char* header_end = strstr(response_buffer.c_str(), DS_HTTP_HEADER_DELIMITER);

  if(NULL != header_end)
    return true;
  else
    return false;
}



bool ds_http_response::content_append(const uint8* input, const uint32 size)
{
  if( DS_HTTP_CONTENT_MAX_SIZE < (response_content.size() + size) )
  {
    notify_cb(DS_HTTP_NOTIFY_PARTIAL_BODY, response_content.content(), response_content.size() );
    response_content.clear();
  }

  response_content.append(input, size);
  content_received += size;
  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "content_append - %d / %d complete", content_received, content_size);

  if(content_received == content_size)
    receive_status = DS_HTTP_RECEIVE_COMPLETE;

  if(content_received > content_size)
    return false;
  else
    return true;
}



bool ds_http_response::chunk_append(const uint8* input, const uint32 size)
{
  response_buffer.append(input, size);

  char         hex_str[DS_HTTP_HEX_STR_MAX_SIZE+1] = {0};
  const char*  iss                                 = response_buffer.c_str();
  const char*  token                               = NULL;
  uint32       remaining                           = response_buffer.size();
  uint32       hex_value                           = 0;


  // Loop <chunk_hex>\r\n<content>\r\n
  while(NULL != (token = strstr((const char*)iss, DS_HTTP_DELIMITER)))
  {
    if(DS_HTTP_HEX_STR_MAX_SIZE < (token - iss))
    {
      DATA_APPSRV_MSG_SPRINTF_2(MSG_LEGACY_ERROR, "chunk_append - Hex string too long len:%d cont:%s", (token - iss), iss);
      return false;
    }

    memset(hex_str, 0, DS_HTTP_HEX_STR_MAX_SIZE+1);
    memscpy(hex_str, DS_HTTP_HEX_STR_MAX_SIZE, iss, (token - iss));
    hex_value = strtoul(hex_str, NULL, 16);

    // offset = hex str len + chunk size + CRLF*2
    uint32 offset = ((token - iss) + hex_value + 4);
    if(hex_value == 0 && remaining == offset)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "chunk_append - Chunked transfer complete");
      receive_status = DS_HTTP_RECEIVE_COMPLETE;
      return true;
    }

    if(remaining < offset)
      break;


    token += 2; // skip CRLF
    while(0 != hex_value)
    {
      uint32 read_size = Appsrv::Utils::get_min<uint32>(hex_value, DS_HTTP_CONTENT_MAX_SIZE);

      // response size exceeds content size limit
      if(DS_HTTP_CONTENT_MAX_SIZE < response_content.size() + read_size)
      {
        if(forward_oversize)
        {
          notify_cb(DS_HTTP_NOTIFY_PARTIAL_BODY, response_content.content(), response_content.size() );
          response_content.clear();
        }
        else
        {
          DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "chunk_append - Content size too large!");
          return false;
        }
      }

      response_content.append(token, read_size);
      hex_value -= read_size;
      token     += read_size;
    }

    iss = token + 2;
    remaining -= offset;
  }

  DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "chunk_append - Chunked transfer incomplete, waiting for more data");
  if(0 != remaining)
  {
    ASBuffer temp((const uint8*)iss, remaining); // copy the buffer since memscpy will overlap otherwise
    response_buffer = temp;
  }
  else
  {
    response_buffer.clear();
  }

  return true;
}



bool ds_http_response::decode_header()
{
  ASStringStream iss(response_buffer.c_str());
  ASString       getter;
  receive_status = DS_HTTP_RECEIVE_COMPLETE;

  DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "decode_header - Parsing response header. Buffer size:%d", response_buffer.size());

  iss.get_next(getter, ' ');
  if(DS_HTTP_VERSION_11 != getter)
  {
    DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_ERROR, "decode_header - Not http 1.1! HTTP version: %s", getter.c_str());
    return false;
  }

  iss.get_next(getter, ' ');
  http_status = strtoul(getter.c_str(), NULL, 10);
  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "decode_header - HTTP status: %d", http_status);

  iss.get_next(getter, '\n'); // skip readable status.

  while(iss.good())
  {
    getter = "";
    iss.get_next(getter, DS_HTTP_DELIMITER);

    if(getter.empty())
    {
      DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "decode_header - Found empty string after %d headers", response_header.size());
      break;
    }

    ASStringStream hss(getter);
    ASString header_name, header_value;
    hss.get_next(header_name, ": ");
    if(hss.eof())
    {
      DATA_APPSRV_MSG_SPRINTF_1(
                                MSG_LEGACY_ERROR,
                                "decode_header - Invalid header format: %s",
                                getter.c_str()
                                );
      return false;
    }
    hss.get_next(header_value, '\0'); // get remaining as value.

    // Add header to header list
    if(!response_header.push_back(ds_http_header_pair(header_name, header_value)))
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "decode_header - Failed to allocate memory for vector");
      return false;
    }

    // Additional header parsing for some headers
    if(DS_HTTP_HEADER_NAME_CONTENT_LENGTH == header_name)
    {
      content_size = strtoul(header_value.c_str(), NULL, 10);
      receive_status = DS_HTTP_RECEIVE_RAW;
      DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "decode_header - Content size %d", content_size);
    }
    else if(DS_HTTP_HEADER_NAME_CONNECTION == header_name)
    {
      // check keep-alive
      if(DS_HTTP_HEADER_VALUE_CONNECTION_KEEP_ALIVE == header_value)
        keep_alive = true;
      else
        keep_alive = false;
    }
    else if(DS_HTTP_HEADER_NAME_CONNECTION_KEEP_ALIVE == header_name)
    {
      ASStringStream kss(header_value);
      ASString tok;

      while(!kss.get_next(tok, '='))
      {
        if("timeout" == tok)
        {
          kss.get_next(tok, ',');
          timeout = strtoul(tok.c_str(), NULL, 10);

          if(timeout > MAX_CONNECTION_TIMEOUT)
            timeout = MAX_CONNECTION_TIMEOUT;

          DATA_APPSRV_MSG1(MSG_LEGACY_LOW, "decode_header - Timeout %d", timeout);
        }
        else
        {
          kss.get_next(tok, ',');
        }
        kss.next_ptr(); // skip whitespace
      }
    }
    else if(DS_HTTP_HEADER_NAME_TRANSFER_ENCODING == header_name)
    {
      if(DS_HTTP_HEADER_VALUE_CHUNKED == header_value)
        receive_status = DS_HTTP_RECEIVE_CHUNK;
    }
    else if(DS_HTTP_HEADER_NAME_SET_COOKIE == header_name)
    {
      parse_cookie(header_value);
    }
  }

  if(DS_HTTP_RECEIVE_RAW == receive_status)
  {
    if(DS_HTTP_CONTENT_MAX_SIZE < content_size && !forward_oversize)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "decode_header - Content size too large");
      return false;
    }

    if(0 == content_size)
    {
      // Empty content; no need to receive more data.
      DATA_APPSRV_MSG0(MSG_LEGACY_MED, "decode_header - Empty content");
      receive_status = DS_HTTP_RECEIVE_COMPLETE;
      return true;
    }
  }
  else if(DS_HTTP_RECEIVE_COMPLETE == receive_status)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_MED, "decode_header - No content data info in header");
    return true;
  }

  const char* header_end = strstr(response_buffer.c_str(), DS_HTTP_HEADER_DELIMITER);
  header_end += 4;
  uint32 header_size = header_end - response_buffer.c_str();

  // Remove response headers from response buffer
  if(response_buffer.size() != header_size)
  {
    ASBuffer remaining_buf((const uint8*)header_end, response_buffer.size() - header_size);
    response_buffer.clear();
    append_response(remaining_buf.content(), remaining_buf.size());
  }
  else
  {
    response_buffer.clear();
  }

  return true;
}


bool ds_http_response::parse_cookie(const ASString& cookie_header)
{
  bool                 error_flag = false;
  ds_http_cookie_info* new_cookie = new(ds_appsrv_alloc<ds_http_cookie_info>(), APPSRV_MEM) ds_http_cookie_info(cookie_header, error_flag);

  if(NULL == new_cookie)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "parse_cookie - Failed to allocate memory for cookie_info");
    return false;
  }

  if(error_flag || !cookies.push_back(new_cookie))
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "parse_cookie - Parsing failed %d or failed to add to cookie list", error_flag);
    ds_appsrv_free(new_cookie);
    return false;
  }

  return true;
}


uint32 ds_http_response::validate_response(
                                           const ds_http_credential_s_type* credential,
                                           const char*                      uri_path,
                                           const ds_http_auth_generator*    request_auth_info
                                           )
{
  const char* auth_info = get_header_value("Authentication-Info");

  // Check if Authentication-Info header exists
  if(NULL == auth_info || 0 == strlen(auth_info))
    return DS_HTTP_STATUS_OK;

  // Parse Authentication-Info header data
  if(!auth_validator.parse_auth_header(auth_info))
    return DS_HTTP_ERROR_HTTP_HEADER;

  auth_validator.realm     = request_auth_info->realm;
  auth_validator.nonce     = request_auth_info->nonce;
  auth_validator.algorithm = request_auth_info->algorithm;

  // Validate response
  if(auth_validator.validate_response(
                                      credential,
                                      uri_path,
                                      response_content.non_const_content(),
                                      response_content.size()
                                      ))
  {
    return DS_HTTP_STATUS_OK;
  }
  else
  {
    return DS_HTTP_ERROR_RSPAUTH_MISMATCH;
  }
}



const char* ds_http_response::get_header_value(const char* header_name) const
{
  ASString hname(header_name);
  uint32 size = response_header.size();

  for(uint32 i=0; i<size; ++i)
  {
    if(hname == response_header[i].name)
      return response_header[i].value.c_str();
  }

  return DS_HTTP_EMPTY_HEADER_STR;
}


void ds_http_response::notify_success()
{
  notify_cb(DS_HTTP_STATUS_OK, response_content.content(), response_content.size());
}



void ds_http_response::notify_cb(uint32 error_status)
{
  notify_cb(error_status, response_content.content(), response_content.size());
}



void ds_http_response::notify_cb(sint15 error_status, const uint8* content, uint32 content_size)
{
  if(cancelled)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_MED, "notify_cb - Request is cancelled; not notifying client");
    return;
  }

  if( error_status != DS_HTTP_STATUS_OK              &&
      error_status != DS_HTTP_ERROR_HTTP_STATUS      &&
      error_status != DS_HTTP_ERROR_RSPAUTH_MISMATCH &&
      error_status != DS_HTTP_NOTIFY_PARTIAL_BODY )
  {
    ds_http_notify_session_cb(
                              session_id,
                              request_id,
                              error_status,
                              NULL
                              );
    return;
  }

  uint32 num_headers = response_header.size();
  uint16 num_cookies = cookies.size();

  DATA_APPSRV_MSG1(MSG_LEGACY_MED, "notify_cb - HTTP request complete with err:%d", error_status);


  // Create header info
  if(NULL == header_info && 0 != num_headers)
  {
    header_info = (ds_http_response_header*)ds_appsrv_alloc(sizeof(ds_http_response_header), num_headers);

    if(NULL == header_info)
      num_headers = 0;

    for(uint32 i = 0; i < num_headers; ++i)
    {
      memscpy(
              header_info[i].name,
              DS_HTTP_HEADER_NAME_MAX_LEN,
              response_header[i].name.c_str(),
              response_header[i].name.size()
              );
      memscpy(
              header_info[i].value,
              DS_HTTP_HEADER_VALUE_MAX_LEN,
              response_header[i].value.c_str(),
              response_header[i].value.size()
              );
    }
  }


  // Create cookie info
  if(NULL == cookie_info && 0 != num_cookies)
  {
    cookie_info = (ds_http_cookie_info_s_type*)ds_appsrv_alloc(sizeof(ds_http_cookie_info_s_type), (uint32)num_cookies);
    get_cookie_info(cookie_info, num_cookies);
  }


  ds_http_response_info_s_type resp_info;
  memset(&resp_info, 0, sizeof(ds_http_response_info_s_type));
  resp_info.http_status  = http_status;
  resp_info.content      = content;
  resp_info.content_size = content_size;
  resp_info.header_info  = header_info;
  resp_info.num_headers  = num_headers;
  resp_info.cookie_info  = cookie_info;
  resp_info.num_cookies  = num_cookies;

  ds_http_notify_session_cb(
                            session_id,
                            request_id,
                            error_status,
                            &resp_info
                            );
}



void ds_http_response::notify_auth_cb(ds_http_block_auth_info_type* auth_info) const
{
  auth_info->cookie_info = NULL;
  auth_info->num_cookies = cookies.size();

  if(cancelled)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_MED, "notify_auth_cb - Request is cancelled; not notifying client");
    return;
  }

  if(0 != auth_info->num_cookies)
  {
    auth_info->cookie_info = (ds_http_cookie_info_s_type*)ds_appsrv_alloc(
                                                                          sizeof(ds_http_cookie_info_s_type),
                                                                          (uint32)auth_info->num_cookies
                                                                          );
    get_cookie_info(auth_info->cookie_info, auth_info->num_cookies);
  }

  ds_http_notify_auth_cb(session_id, request_id, auth_info);

  // cleanup
  if(NULL != auth_info->cookie_info)
  {
    ds_appsrv_free(auth_info->cookie_info);
    auth_info->cookie_info = NULL;
    auth_info->num_cookies = 0;
  }
}



void ds_http_response::cancel_request()
{
  cancelled = true;
}



bool ds_http_response::is_cancelled() const
{
  return cancelled;
}



// Assumes caller to allocate enough memory for cookie_info.
void ds_http_response::get_cookie_info(ds_http_cookie_info_s_type* cookie_info, uint16& num_cookies) const
{
  if(NULL == cookie_info || (uint32)num_cookies != cookies.size())
    num_cookies = 0;

  for(uint16 i = 0; i < num_cookies; ++i)
  {
    cookies[i]->get_cookie_info(cookie_info[i]);
  }
}



bool ds_http_response::receive_complete() const
{
  return (DS_HTTP_RECEIVE_COMPLETE == receive_status);
}


