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
08/04/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_REQUEST_FACTORY_H
#define DS_HTTP_REQUEST_FACTORY_H

#include "ds_http_types.h"

class ds_http_request;


/* HTTP Request Methods */
/* Only GET and POST is currently supported. Future support for PUT? */
enum ds_http_method_e_type
{
  DS_HTTP_METHOD_GET,
  DS_HTTP_METHOD_PUT,
  DS_HTTP_METHOD_POST,
  DS_HTTP_METHOD_POST_CHUNKED,
  DS_HTTP_METHOD_HEAD,
  DS_HTTP_METHOD_DELETE,
  DS_HTTP_METHOD_CONNECT,
  DS_HTTP_METHOD_OPTIONS,
  DS_HTTP_METHOD_TRACE
};




struct ds_http_supp_header_info
{
  ds_http_content_info_s_type*         post_info;
  ds_http_header_condition_s_type*     condition_info;
  ds_http_chunked_content_info_s_type* chunked_content_info;

  ds_http_supp_header_info()
  : post_info(NULL), condition_info(NULL), chunked_content_info(NULL)
  { }
};




/*============================================================================
FUNCTION ds_http_request_factory

DESCRIPTION
  Creates a new ds_http_request object

PARAMETERS
  [In]  http_method       - Request method
  [In]  session_id        - Client session ID
  [In]  request_id        - Request ID
  [In]  uri_str           - URI to send request to
  [In]  request_header    - Request header information
  [In]  supp_info         - Supplementary information for the request
  [In]  reserved_src_port - The port for the socket to bind
  [Out] request_errno     - Error status

RETURN VALUE
  A new ds_http_request object

DEPENDENCIES
  None
============================================================================*/
ds_http_request* ds_http_request_factory(
                                         const ds_http_method_e_type     http_method,
                                         const uint32                    session_id,
                                         const uint32                    request_id,
                                         const char*                     uri_str,
                                         const ds_http_header_s_type*    request_header,
                                         const ds_http_supp_header_info* supp_info,
                                         uint16                          reserved_src_port,
                                         sint15*                         request_errno
                                         );


#endif /* DS_HTTP_REQUEST_FACTORY_H */

