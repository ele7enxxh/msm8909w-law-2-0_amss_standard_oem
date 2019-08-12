/*==============================================================================

                          ds_http_request_derived.h

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
04/16/15    ml     Response authentication support.
08/04/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_REQUEST_DERIVED_H
#define DS_HTTP_REQUEST_DERIVED_H

#include "ds_http_request.h"


class ds_http_get_request : public ds_http_request
{
public:
  ds_http_get_request(
                      const uint32                 sid,
                      const uint32                 rid,
                      const char*                  uri_str,
                      const ds_http_header_s_type* header,
                      uint16                       reserved_src_port,
                      sint15*                      request_errno
                      );
  virtual ~ds_http_get_request()
  { }

protected:
  virtual void create_request_str();
  virtual const char* get_request_method_str();
};



class ds_http_post_request : public ds_http_request
{
public:
  ds_http_post_request(
                      const uint32                 sid,
                      const uint32                 rid,
                      const char*                  uri_str,
                      const ds_http_header_s_type* header,
                      const uint8*                 content_data,
                      const uint32                 content_size,
                      uint16                       reserved_src_port,
                      sint15*                      request_errno
                      );
  virtual ~ds_http_post_request()
  { }

protected:
  virtual void create_request_str();
  virtual const char* get_request_method_str();
};



class ds_http_post_chunked_request : public ds_http_request
{
public:
  ds_http_post_chunked_request(
                               const uint32                         sid,
                               const uint32                         rid,
                               const char*                          uri_str,
                               const ds_http_header_s_type*         header,
                               ds_http_chunked_content_info_s_type* chunked_content_info,
                               uint16                               reserved_src_port,
                               sint15*                              request_errno
                               );
  virtual ~ds_http_post_chunked_request()
  { }

protected:
  virtual void create_request_str();
  virtual const char* get_request_method_str();
};



#endif /* DS_HTTP_REQUEST_DERIVED_H */
