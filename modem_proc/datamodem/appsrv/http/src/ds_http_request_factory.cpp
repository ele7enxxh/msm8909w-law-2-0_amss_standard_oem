/*==============================================================================

                              ds_http_request_factory.h

GENERAL DESCRIPTION
  Provides a common function to create a ds_http_request object with error
  handling without needing to know the derived classes of ds_http_request.

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/02/15    ml     HTTP Post chunked support
05/20/15    ml     Use memory from modem heap
08/04/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_request_factory.h"
#include "ds_http_request_derived.h"
#include "ds_http_types.h"
#include "ds_appsrv_mem.h"
#include "data_msg.h"


ds_http_request* ds_http_request_factory(
                                         const ds_http_method_e_type     http_method,
                                         const uint32                    session_id,
                                         const uint32                    request_id,
                                         const char*                     uri_str,
                                         const ds_http_header_s_type*    request_header,
                                         const ds_http_supp_header_info* supp_info,
                                         uint16                          reserved_src_port,
                                         sint15*                         request_errno
                                         )
{
  ds_http_request* request = NULL;

  *request_errno = DS_HTTP_ERROR_NONE;
  switch(http_method)
  {
    case DS_HTTP_METHOD_GET:
    {
      request = new(ds_appsrv_alloc<ds_http_get_request>(), APPSRV_MEM) ds_http_get_request(
                                                                                            session_id,
                                                                                            request_id,
                                                                                            uri_str,
                                                                                            request_header,
                                                                                            reserved_src_port,
                                                                                            request_errno
                                                                                            );
    }
    break;

    case DS_HTTP_METHOD_POST:
    {
      if(NULL == supp_info || NULL == supp_info->post_info)
      {
        *request_errno = DS_HTTP_ERROR_INVALID_PARAM;
        return NULL;
      }

      request = new(ds_appsrv_alloc<ds_http_post_request>(), APPSRV_MEM) ds_http_post_request(
                                                                                              session_id,
                                                                                              request_id,
                                                                                              uri_str,
                                                                                              request_header,
                                                                                              supp_info->post_info->content,
                                                                                              supp_info->post_info->content_size,
                                                                                              reserved_src_port,
                                                                                              request_errno
                                                                                              );
    }
    break;

    case DS_HTTP_METHOD_POST_CHUNKED:
    {
      if(NULL == supp_info || NULL == supp_info->chunked_content_info)
      {
        *request_errno = DS_HTTP_ERROR_INVALID_PARAM;
        return NULL;
      }

      request
        = new(ds_appsrv_alloc<ds_http_post_chunked_request>(), APPSRV_MEM) ds_http_post_chunked_request(
                                                                                                        session_id,
                                                                                                        request_id,
                                                                                                        uri_str,
                                                                                                        request_header,
                                                                                                        supp_info->chunked_content_info,
                                                                                                        reserved_src_port,
                                                                                                        request_errno
                                                                                                        );
    }
    break;

    case DS_HTTP_METHOD_PUT:     // fallthrough
    case DS_HTTP_METHOD_HEAD:    // fallthrough
    case DS_HTTP_METHOD_DELETE:  // fallthrough
    case DS_HTTP_METHOD_CONNECT: // fallthrough
    case DS_HTTP_METHOD_OPTIONS: // fallthrough
    case DS_HTTP_METHOD_TRACE:   // fallthrough
    default:
    {
      *request_errno = DS_HTTP_ERROR_UNSUPPORTED_METHOD;
      return NULL;
    }
  }

  if(NULL == request)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_request_factory - Failed to allocate memory for request");
    *request_errno = DS_HTTP_ERROR_MEMALLOC;
  }
  else if(*request_errno != DS_HTTP_ERROR_NONE)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_request_factory - request error %d", *request_errno);
    ds_appsrv_free(request);
    request = NULL;
  }
  else
  {
    // Log for request tracking.
    DATA_APPSRV_MSG_SPRINTF_3(
                              MSG_LEGACY_HIGH,
                              "ds_http_request_factory - rid:%d method:%d url:%s",
                              request_id,
                              http_method,
                              uri_str
                              );
  }

  return request;
}


