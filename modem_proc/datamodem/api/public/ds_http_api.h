/*==============================================================================

                              ds_http_api.h

GENERAL DESCRIPTION
  API header for Data Service HTTP request

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds_http_init()
    - required initialization
    - ps_sys and ds_ssl must be initialized before

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/21/16    ml     Query API for PDN bringup failure reason
02/20/16    ml     Session option to handle response size > 100k
02/20/16    ml     Multiple SSL version support
11/02/15    ml     Cancel request
11/02/15    ml     HTTP Post chunked support
09/11/15    ml     DSDS support
06/11/15    ml     Cookie support
04/20/15    ml     Session deregistration
03/10/15    ml     IWLAN / WLAN LB Support
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_API_H
#define DS_HTTP_API_H

#include "ds_http_types.h"
#include "dssocket.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*==============================================================================
FUNCTION ds_http_open_session

DESCRIPTION
  Creates a new session for ds http.

PARAMETERS
  [In] http_cb_fcn  - Callback function to register
  [In] block_cb_fcn - Callback function to register
  [In] iface_info   - Iface information

RETURN VALUE
  On success, returns a session ID.
  On fail, returns DS_HTTP_ERROR.

DEPENDENCIES
  None
==============================================================================*/
uint32 ds_http_open_session(
                            ds_http_cb_fcn             http_cb_fcn,
                            ds_http_block_event_cb_fcn block_cb_fcn,
                            ds_http_iface_info_s_type* iface_info
                            );


/*==============================================================================
FUNCTION ds_http_open_session_ex

DESCRIPTION
  Creates a new session for ds http. This function registers an improved
    callback for http response which provides more details about the response
    headers.

PARAMETERS
  [In] http_cb_fcn  - Callback function to register
  [In] block_cb_fcn - Callback function to register
  [In] iface_info   - Iface information

RETURN VALUE
  On success, returns a session ID.
  On fail, returns DS_HTTP_ERROR.

DEPENDENCIES
  None
==============================================================================*/
uint32 ds_http_open_session_ex(
                               ds_http_cb_fcn_ex          http_cb_fcn,
                               ds_http_block_event_cb_fcn block_cb_fcn,
                               ds_http_iface_info_s_type* iface_info
                               );



/*==============================================================================
FUNCTION ds_http_set_session_config

DESCRIPTION
  Modifies session configuration settings.
  Session configurations are locked when first request is received.

PARAMETERS
  [In] session_id  - Session ID to change config
  [In] config_type - Config type to modify
  [In] config_info - Config value to set

RETURN VALUE
  Returns TRUE on success, FALSE on fail

DEPENDENCIES
  None
==============================================================================*/
boolean ds_http_set_session_config(
                                   const uint32                         session_id,
                                   ds_http_session_config_option_e_type config_type,
                                   ds_http_session_config_value_u_type* config_info
                                   );






/*==============================================================================
FUNCTION ds_http_close_session

DESCRIPTION
  Closes the session

PARAMETERS
  [In] session_id - The session to close

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_close_session(const uint32 session_id);


/*==============================================================================
FUNCTION ds_http_create_get_request

DESCRIPTION
  Creates a HTTP GET request.

PARAMETERS
  [In]  session_id     - The session ID obtained from ds_http_open_session
  [In]  uri            - Request uri string
  [In]  request_header - Header information for the request. NULL ok
  [Out] request_errno  - Error code

RETURN VALUE
  On success, a request identifier.
  On fail, returns DS_HTTP_ERROR and request_errno will be set.

  request_errno values
  --------------------
  DS_HTTP_ERROR_NO_URI_INFO
  DS_HTTP_ERROR_UNSUPPORTED_METHOD
  DS_HTTP_ERROR_INVALID_SESSION
  DS_HTTP_ERROR_NETDOWN
  DS_HTTP_ERROR_MEMALLOC

DEPENDENCIES
  ds_http_open_session must be called to create a session.
==============================================================================*/
uint32 ds_http_create_get_request(
                                  const uint32           session_id,
                                  const char*            uri,
                                  ds_http_header_s_type* request_header,
                                  uint16                 reserved_src_port,
                                  sint15*                request_errno
                                  );




/*==============================================================================
FUNCTION ds_http_create_post_request

DESCRIPTION
  Creates a HTTP POST request.

PARAMETERS
  [In]  session_id     - The session ID obtained from ds_http_open_session
  [In]  uri            - Request uri string
  [In]  request_header - Header information for the request. Can't be NULL and
                         content_type must not be an empty string
  [In]  content        - Content to post. Can't be NULL.
  [Out] request_errno  - Error code

RETURN VALUE
  On success, a request identifier.
  On fail, returns DS_HTTP_ERROR and request_errno will be set.

  request_errno values
  --------------------
  DS_HTTP_ERROR_NO_URI_INFO
  DS_HTTP_ERROR_UNSUPPORTED_METHOD
  DS_HTTP_ERROR_INVALID_SESSION
  DS_HTTP_ERROR_INVALID_PARAM
  DS_HTTP_ERROR_NETDOWN
  DS_HTTP_ERROR_MEMALLOC

DEPENDENCIES
  ds_http_open_session must be called to create a session.
==============================================================================*/
uint32 ds_http_create_post_request(
                                   const uint32                 session_id,
                                   const char*                  uri,
                                   ds_http_header_s_type*       request_header,
                                   ds_http_content_info_s_type* content_info,
                                   uint16                       reserved_src_port,
                                   sint15*                      request_errno
                                   );



/*==============================================================================
FUNCTION ds_http_create_post_request_chunked

DESCRIPTION
  Creates a HTTP POST request.
  Request body will be sent using chunked transfer encoding.

PARAMETERS
  [In]  session_id     - The session ID obtained from ds_http_open_session
  [In]  uri            - Request uri string
  [In]  request_header - Header information for the request. Can't be NULL and
                         content_type must not be an empty string
  [In]  content_data   - Content to post. Can't be NULL.
  [Out] request_errno  - Error code

RETURN VALUE
  On success, a request identifier.
  On fail, returns DS_HTTP_ERROR and request_errno will be set.

  request_errno values
  --------------------
  DS_HTTP_ERROR_NO_URI_INFO
  DS_HTTP_ERROR_UNSUPPORTED_METHOD
  DS_HTTP_ERROR_INVALID_SESSION
  DS_HTTP_ERROR_INVALID_PARAM
  DS_HTTP_ERROR_NETDOWN
  DS_HTTP_ERROR_MEMALLOC

DEPENDENCIES
  ds_http_open_session must be called to create a session.
==============================================================================*/
uint32 ds_http_create_post_request_chunked(
                                           const uint32                         session_id,
                                           const char*                          uri,
                                           ds_http_header_s_type*               request_header,
                                           ds_http_chunked_content_info_s_type* content_data,
                                           uint16                               reserved_src_port,
                                           sint15*                              request_errno
                                           );



/*==============================================================================
FUNCTION ds_http_cancel_request

DESCRIPTION
  Cancel the request

PARAMETERS
  [In] session_id - The session used to create the request
  [In] request_id - ID of the request

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_cancel_request(const uint32 session_id, const uint32 request_id);


/*==============================================================================
FUNCTION ds_http_append_chunked_fragment

DESCRIPTION
  Append a fragment of the content to the request. Only valid for chunked
    transfer requests that has not received its last fragment.

PARAMETERS
  [In] session_id   - The session used to create the request
  [In] request_id   - ID of the request
  [In] content_data - Fragment to add to the request

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
sint15 ds_http_append_chunked_fragment(
                                       const uint32                         session_id,
                                       const uint32                         request_id,
                                       ds_http_chunked_content_info_s_type* content_data
                                       );



/*==============================================================================
FUNCTION ds_http_resolve_auth_block

DESCRIPTION
  Provide authentication information to the request with 401/407 error.
  Providing NULL or empty credentials will fail the request and notify a 401/407
  error in the http callback function.

  This function will have no effect if
    - Session/request ID is invalid
    - Authentication block event didn't occur for the request
    - Socket times out before client provides the credentials

PARAMETERS
  [In] session_id - Session ID given by ds_http_open_session
  [In] request_id - Request ID given by ds_http_create_xxx_request
  [In] auth_info  - Information needed to resolve authentication status error,
                    such as username and password to authenticate.
                    Giving a NULL pointer will fail the request and will notify
                    the failure through the http callback fcn

RETURN VALUE
  Success
    DS_HTTP_ERROR_NONE
  Fail
    DS_HTTP_ERROR_INVALID_SESSION

DEPENDENCIES
  None
==============================================================================*/
void ds_http_resolve_auth_block(
                                uint32                    session_id,
                                uint32                    request_id,
                                ds_http_auth_info_s_type* auth_info
                                );



/*==============================================================================
FUNCTION ds_http_resolve_ssl_cert_block

DESCRIPTION
  Updates the request to handle SSL block event.

  This function will have no effect if
    - Session/request ID is invalid
    - SSL block event didn't occur for the request
    - Socket times out before client provides the credentials

PARAMETERS
  [In] session_id - Session ID given by ds_http_open_session
  [In] request_id - Request ID given by ds_http_create_xxx_request
  [In] cont       - DS_HTTP_SSL_CONTINUE to ignore SSL alert and continue.
                    DS_HTTP_SSL_ABORT to fail the request. There will be a
                    request failure notification in the callback.

RETURN VALUE
  Success - DS_HTTP_ERROR_NONE
  Fail    - DS_HTTP_ERROR_INVALID_SESSION

DEPENDENCIES
  None
==============================================================================*/
void ds_http_resolve_ssl_cert_block(
                                    uint32  session_id,
                                    uint32  request_id,
                                    boolean cont
                                    );


/*==============================================================================
FUNCTION ds_http_is_nw_srv_avail

DESCRIPTION
  Query for network service status.

PARAMETERS
  [In] iface - The iface to query network status for

RETURN VALUE
  TRUE if network service is available, else FALSE

DEPENDENCIES
  None
==============================================================================*/
boolean ds_http_is_nw_srv_avail(ds_http_iface_e_type iface);
boolean ds_http_is_nw_srv_avail_for_subs_id(
                                            ds_http_iface_e_type        iface,
                                            ds_http_subscription_e_type subs_id
                                            );



/*==============================================================================
FUNCTION ds_http_last_netdownreason

DESCRIPTION
  Retrieve reason for network iface going down. Values only valid if callback
    notifies DS_HTTP_ERROR_DSNET or DS_HTTP_ERROR_NETWORK_DOWN errors.

PARAMETERS
  [In]  session_id - Session ID given by ds_http_open_session
  [Out] reason     - Reason for network iface going down

RETURN VALUE
  DS_HTTP_ERROR_NONE if retrieval is successful
  DS_HTTP_ERROR      if retrieval fails

DEPENDENCIES
  None
==============================================================================*/
uint32 ds_http_last_netdownreason(uint32 session_id, dss_net_down_reason_type *reason);



#ifdef __cplusplus
}
#endif

#endif /* DS_HTTP_API_H */

