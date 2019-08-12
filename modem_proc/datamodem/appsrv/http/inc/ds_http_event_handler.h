/*==============================================================================

                              ds_http_event_handler.h

GENERAL DESCRIPTION
  HTTP event handler

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/15/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_EVENT_HANDLER_H
#define DS_HTTP_EVENT_HANDLER_H


extern "C"
{
  #include "dssdns.h"
}
#include "dssocket.h"

class ds_http_request;

void ds_http_init_event_handler();


/*==============================================================================
FUNCTION ds_http_add_new_request

DESCRIPTION
  Adds the new HTTP request to dstask which eventually will go to http_dsnet

PARAMETERS
  [In] request - ds_http_request object

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
bool ds_http_add_new_request(ds_http_request* request);



// used by socket timeout timer
void ds_http_timeout_timer_cb(uint32 http_socket_id);


void ds_http_net_cb(
                    sint15            nethandle,
                    dss_iface_id_type iface_id,
                    sint15            err,
                    void*             net_cb_user_data
                    );
void ds_http_sock_cb(
                     sint15 nethandle,
                     sint15 sockfd,
                     uint32 event_mask,
                     void*  sock_cb_user_data
                     );
void ds_http_dns_cb(
                    dss_dns_session_mgr_handle_type session_handle,
                    dss_dns_query_handle_type       query_handle,
                    dss_dns_api_type_enum_type      api_type,
                    uint16                          num_records,
                    void*                           user_data_ptr,
                    int16                           dss_errno
                    );



#endif /* DS_HTTP_EVENT_HANDLER_H */
