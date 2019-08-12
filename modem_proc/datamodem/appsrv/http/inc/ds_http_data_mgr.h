/*==============================================================================

                        ds_http_data_manager.h

GENERAL DESCRIPTION
  Internal data manager for session and dsnet

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
#ifndef DS_HTTP_DATA_MGR_H
#define DS_HTTP_DATA_MGR_H

#include "ds_http_types.h"
#include "ds_http_dsnet_iface_info.h"
#include "dssocket.h"

extern "C"
{
  #include "dssdns.h"
}


class  ds_http_dsnet;
class  ds_http_request;
struct ds_http_chunked_content;


/*==============================================================================
FUNCTION ds_http_init

DESCRIPTION
  ds_http initializer

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  Must be called ONLY ONCE
==============================================================================*/
void ds_http_init();



/*==============================================================================
FUNCTION ds_http_init_data_mgr

DESCRIPTION
  DS_HTTP data manager initializer

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_init_data_mgr();



/*==============================================================================
FUNCTION ds_http_create_session

DESCRIPTION
  Creates a new session and, if no dsnet exists for the pdn, a new dsnet object.

PARAMETERS
  [In] cb_fcn - Callback function to register
  [In] profile_id - The PDP ID to use for this session

RETURN VALUE
  On success, returns a session ID.
  On fail, returns DS_HTTP_ERROR.

DEPENDENCIES
  None
==============================================================================*/
uint32 ds_http_create_session(
                              ds_http_cb_fcn              http_cb_fcn,
                              ds_http_cb_fcn_ex           http_cb_fcn_ex,
                              ds_http_block_event_cb_fcn  block_cb_fcn,
                              ds_http_dsnet_iface_info&   iface_info
                              );



/*==============================================================================
FUNCTION ds_http_remove_session

DESCRIPTION
  Removes the specified session and does any cleanup necessary

PARAMETERS
  [In] session_id - ID of the session to close

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_remove_session(const uint32 session_id);


bool ds_http_get_session_config(
                                const uint32                         session_id,
                                ds_http_session_config_option_e_type config_type,
                                ds_http_session_config_value_u_type* config_info
                                );
bool ds_http_update_session_config(
                                   const uint32                         session_id,
                                   ds_http_session_config_option_e_type config_type,
                                   ds_http_session_config_value_u_type* config_info
                                   );


/*==============================================================================
FUNCTION ds_http_check_valid_session

DESCRIPTION
  Checks if the given session ID is valid

PARAMETERS
  [In] session_id - Session ID

RETURN VALUE
  Returns true if it is valid, else false

DEPENDENCIES
  None
==============================================================================*/
bool ds_http_check_valid_session(const uint32 session_id);



/*==============================================================================
FUNCTION ds_http_add_request_to_dsnet

DESCRIPTION
  Adds a request to the appropriate dsnet

PARAMETERS
  [In] request - request object

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_add_request_to_dsnet(ds_http_request* request);


/*==============================================================================
FUNCTION ds_http_remove_request

DESCRIPTION
  Adds a request to the appropriate dsnet

PARAMETERS
  [In] request - request object

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_remove_request(const uint32 session_id, const uint32 request_id);



/*==============================================================================
FUNCTION ds_http_notify_http_ready

DESCRIPTION
  Notifies the client that the system is up and ds_http is ready to receive requests

PARAMETERS
  session_id - The session ID to notify

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_notify_http_ready(ds_http_iface_e_type iface, ds_http_subscription_e_type subs_id);
void ds_http_notify_http_ready(const uint32 session_id);



/*==============================================================================
FUNCTION ds_http_notify_session_cb

DESCRIPTION
  Calls the registered http event callback function for the session with the
  given parameters.

PARAMETERS
  [In] session_id   - Session ID
  [In] request_id   - Request ID
  [In] error        - Error
  [In] resp_info    - Http response Information. NULL if it failed to receive response.

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_notify_session_cb(
                               const uint32                        session_id,
                               const uint32                        request_id,
                               const sint15                        error,
                               const ds_http_response_info_s_type* resp_info
                               );



/*==============================================================================
FUNCTION ds_http_notify_auth_cb

DESCRIPTION
  Notifies the client that an authentication block occured. Client must update
  the request by calling ds_http_resolve_auth_block().

PARAMETERS
  [In] session_id   - Session ID
  [In] request_id   - Request ID
  [In] realm        - Authentication realm that the server requested for

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_notify_auth_cb(
                            const uint32                  session_id,
                            const uint32                  request_id,
                            ds_http_block_auth_info_type* auth_info
                            );



/*==============================================================================
FUNCTION ds_http_notify_ssl_alert_cb

DESCRIPTION
  Notifies the client that a SSL block occured. Client must update the request
  by calling ds_http_resolve_ssl_cert_block().

PARAMETERS
  [In] session_id     - Session ID
  [In] request_id     - Request ID
  [In] ssl_alert_info - SSL certification alert error

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_http_notify_ssl_alert_cb(
                                 const uint32                  session_id,
                                 const uint32                  request_id,
                                 ds_http_ssl_cert_alert_e_type ssl_alert_info
                                 );


bool ds_http_check_dsnet_status(uint32 session_id, sint15* request_errno);
void ds_http_resolve_auth(uint32 session_id, uint32 request_id, ds_http_auth_info_s_type* auth_info);
void ds_http_resolve_ssl_suspension(uint32 session_id, uint32 request_id, bool ssl_cont);


void ds_http_append_chunk_to_request(
                                     const uint32                   session_id,
                                     const uint32                   request_id,
                                     const ds_http_chunked_content& chunk_content
                                     );


void ds_http_handle_dsnet_event(
                                const sint15            nethandle,
                                const dss_iface_id_type iface_id,
                                const sint15            error
                                );
void ds_http_handle_socket_event(
                                 const sint15 nethandle,
                                 const sint15 sockfd,
                                 const uint32 event_mask
                                 );
void ds_http_handle_dns_event(
                              const dss_dns_session_mgr_handle_type session_handle,
                              const dss_dns_query_handle_type       query_handle,
                              const uint16                          num_records,
                              const int16                           dss_errno
                              );
void ds_http_handle_timeout_event(const sint15 nethandle, const uint16 socket_id);



void ds_http_set_network_status(
                                ds_http_iface_e_type        iface,
                                ds_http_subscription_e_type subs_id,
                                bool                        net_status
                                );
bool ds_http_get_network_status(
                                ds_http_iface_e_type        iface,
                                ds_http_subscription_e_type subs_id
                                );

uint32 ds_http_get_last_netdownreason(uint32 session_id, dss_net_down_reason_type *reason);

#endif /* DS_HTTP_DATA_MGR_H */
