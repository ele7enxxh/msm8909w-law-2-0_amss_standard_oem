/*==============================================================================

                              ds_http_response.h

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
06/11/15    ml     Cookie support
04/16/15    ml     Response authentication support.
01/07/15    ml     Fixed bug for persistent timeout being 0 when it should be default.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_RESPONSE_H
#define DS_HTTP_RESPONSE_H

#include "ds_http_types.h"
#include "ds_http_auth_validator.h"
#include "ds_http_auth_generator.h"
#include "ds_http_cookie_info.h"

#include "ds_ASString.h"
#include "ds_ASBuffer.h"
#include "ds_ASVector.h"

const uint32 DS_HTTP_DEFAULT_HEADER_CAPACITY = 32;


class ds_http_response
{
public:
  ds_http_response(const uint32 sid, const uint32 rid);
  ~ds_http_response();

  const char* get_header_value(const char* header_name) const;

  bool append_response(const uint8* input, const uint32 size);
  bool receive_complete() const;
  uint32 validate_response(
                           const ds_http_credential_s_type* credential,
                           const char*                      uri_path,
                           const ds_http_auth_generator*    request_auth_info
                           );
  void notify_success();
  void notify_cb(uint32 error_status);
  void notify_cb(sint15 error_status, const uint8* content, uint32 content_size);
  void notify_auth_cb(ds_http_block_auth_info_type* auth_event_info) const;

  void cancel_request();
  bool is_cancelled() const;

private:
  // returns true if header end delimeter (crlf) is found, else false
  bool header_append(const uint8* input, const uint32 size);
  // Returns false if append fails
  bool content_append(const uint8* input, const uint32 size);
  // Returns false if append fails
  bool chunk_append(const uint8* input, const uint32 size);
  bool decode_header();
  bool parse_cookie(const ASString& cookie_header);
  void get_cookie_info(ds_http_cookie_info_s_type* cookie_info, uint16& num_cookies) const;

private:
  // Internal types
  enum ds_http_receive_status_e_type
  {
    DS_HTTP_RECEIVE_HEADER,
    DS_HTTP_RECEIVE_RAW,
    DS_HTTP_RECEIVE_CHUNK,
    DS_HTTP_RECEIVE_COMPLETE
  };

  struct ds_http_header_pair
  {
    ASString name;
    ASString value;

    ds_http_header_pair() { }
    ds_http_header_pair(const ASString& n, const ASString& v)
    : name(n), value(v) { }
  };


/* Class members */
public:
  const uint32 session_id; // Constructor req
  const uint32 request_id; // Constructor req

  // Response header data. Only valid if receive_complete() returns true
  bool   keep_alive;
  uint32 http_status;
  uint32 content_size;
  // Persistent connection timeout in seconds.
  // If the connection timeout isn't specified in the response, it will use DEFAULT_CONNECTION_TIMEOUT
  uint32 timeout;


private:
  ds_http_receive_status_e_type  receive_status;
  ASBuffer                       response_buffer;
  ASBuffer                       response_content;
  ASVector<ds_http_header_pair>  response_header;
  ds_http_auth_validator         auth_validator;
  ASVector<ds_http_cookie_info*> cookies;
  bool                           cancelled;

  ds_http_response_header*       header_info;
  ds_http_cookie_info_s_type*    cookie_info;
  uint32                         content_received;
  bool                           forward_oversize;
};


#endif /* DS_HTTP_RESPONSE_H */
