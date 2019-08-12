/*==============================================================================

                              ds_http_request_derived.cpp

GENERAL DESCRIPTION
  Derived ds_http_request classes to create different headers for each HTTP
  methods

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/02/15    ml     HTTP Post chunked support
04/16/15    ml     Response authentication support
08/04/14    ml     Created file/Initial version
==============================================================================*/
#include "ds_http_request_derived.h"

#include "ds_http_types.h"
#include "ds_http_utility.h"

#include <stringl/stringl.h>


const char HTTP_METHOD_GET_STR[]    = "GET";
const char HTTP_METHOD_POST_STR[]   = "POST";
// const char HTTP_METHOD_PUT_STR[]    = "PUT";
// const char HTTP_METHOD_DELETE_STR[] = "DELETE";


/*==============================================================================
                            ds_http_get_request
==============================================================================*/
ds_http_get_request::ds_http_get_request(
                                         const uint32                 sid,
                                         const uint32                 rid,
                                         const char*                  uri_str,
                                         const ds_http_header_s_type* header,
                                         uint16                       reserved_src_port,
                                         sint15*                      request_errno
                                         )
: ds_http_request(sid, rid, uri_str, header, reserved_src_port, false, request_errno)
{
  if(DS_HTTP_ERROR_NONE != *request_errno)
    return;

  create_request_str();

  if(check_buffer_error())
    *request_errno = DS_HTTP_ERROR_MEMALLOC;
}


void ds_http_get_request::create_request_str()
{
  ds_http_request::create_request_str("GET ");
}


const char* ds_http_get_request::get_request_method_str()
{
  return HTTP_METHOD_GET_STR;
}





/*==============================================================================
                            ds_http_post_request
==============================================================================*/
ds_http_post_request::ds_http_post_request(
                                           const uint32                 sid,
                                           const uint32                 rid,
                                           const char*                  uri_str,
                                           const ds_http_header_s_type* header,
                                           const uint8*                 content_data,
                                           const uint32                 content_size,
                                           uint16                       reserved_src_port,
                                           sint15*                      request_errno
                                           )
: ds_http_request(sid, rid, uri_str, header, reserved_src_port, false, request_errno)
{
  if(DS_HTTP_ERROR_NONE != *request_errno)
    return;

  if(
     DS_HTTP_CONTENT_MAX_SIZE < content_size ||
     (NULL == content_data && 0 != content_size)
     )
  {
    *request_errno = DS_HTTP_ERROR_INVALID_PARAM;
    return;
  }

  create_request_str();
  append_content(content_data, content_size);

  if(check_buffer_error())
    *request_errno = DS_HTTP_ERROR_MEMALLOC;
}



void ds_http_post_request::create_request_str()
{
  ds_http_request::create_request_str("POST ");
}



const char* ds_http_post_request::get_request_method_str()
{
  return HTTP_METHOD_POST_STR;
}



/*==============================================================================
                            ds_http_post_chunked_request
==============================================================================*/
ds_http_post_chunked_request::ds_http_post_chunked_request(
                                                           const uint32                         sid,
                                                           const uint32                         rid,
                                                           const char*                          uri_str,
                                                           const ds_http_header_s_type*         header,
                                                           ds_http_chunked_content_info_s_type* chunked_content_info,
                                                           uint16                               reserved_src_port,
                                                           sint15*                              request_errno
                                                           )
: ds_http_request(sid, rid, uri_str, header, reserved_src_port, true, request_errno)
{
  if(DS_HTTP_ERROR_NONE != *request_errno)
    return;

  if(
     NULL == chunked_content_info                                  ||
     DS_HTTP_CONTENT_MAX_SIZE < chunked_content_info->content_info.content_size ||
     (NULL == chunked_content_info->content_info.content && 0 != chunked_content_info->content_info.content_size)
     )
  {
    *request_errno = DS_HTTP_ERROR_INVALID_PARAM;
    return;
  }

  ds_http_chunked_content chunked_content(chunked_content_info);
  create_request_str();
  append_chunked_content(chunked_content);

  if(check_buffer_error())
    *request_errno = DS_HTTP_ERROR_MEMALLOC;
}



void ds_http_post_chunked_request::create_request_str()
{
  ds_http_request::create_request_str("POST ");
}



const char* ds_http_post_chunked_request::get_request_method_str()
{
  return HTTP_METHOD_POST_STR;
}
