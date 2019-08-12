/*==============================================================================

                              ds_http_api.cpp

GENERAL DESCRIPTION
  API definition for Data Service HTTP request

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
05/20/15    ml     Use memory from modem heap
04/20/15    ml     Session deregistration
04/10/15    ml     Iface any support
03/10/15    ml     IWLAN / WLAN LB Support
12/02/14    ml     Added features for more detailed authentication requests.
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_api.h"

#include "ds_http_data_mgr.h"
#include "ds_http_event_handler.h"
#include "ds_http_dsnet_iface_info.h"
#include "ds_http_msg.h"
#include "ds_http_request.h"
#include "ds_http_request_factory.h"
#include "ds_ASMutex.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"
#include "dssocket.h"
#include "rex.h"
#include <stringl/stringl.h>

using Appsrv::Utils::ASMutex;
using Appsrv::Utils::ASLockGuard;

/*==============================================================================
                              Static structures
==============================================================================*/
struct ds_http_remove_request_msg
{
  const uint32 session_id;
  const uint32 request_id;

  ds_http_remove_request_msg(const uint32 sid, const uint32 rid)
  : session_id(sid), request_id(rid) { }
};



struct ds_http_append_chunk_msg
{
  const uint32            session_id;
  const uint32            request_id;
  ds_http_chunked_content chunked_content;

  ds_http_append_chunk_msg(
                           const uint32                         sid,
                           const uint32                         rid,
                           ds_http_chunked_content_info_s_type* chunked_content_info
                           )
  : session_id(sid), request_id(rid), chunked_content(chunked_content_info)
  { }
};



struct ds_http_block_msg
{
  const uint32                   session_id;
  const uint32                   request_id;
  const ds_http_block_event_type block_event;

  // DS_HTTP_BLOCK_AUTHENTICATION
  ds_http_auth_info_s_type auth_info;
  bool                     auth_info_set;
  // DS_HTTP_BLOCK_SSL_CERTIFICATE_ALERT
  bool                     ssl_cont;

  ds_http_block_msg(
                    const uint32                    sid,
                    const uint32                    rid,
                    const ds_http_auth_info_s_type* auth_info
                    );

  ds_http_block_msg(
                    const uint32 sid,
                    const uint32 rid,
                    bool         cont
                    );
};




/*==============================================================================
                              Static functions
==============================================================================*/
static uint32 ds_http_open_session_helper(
                                          ds_http_cb_fcn             http_cb_fcn,
                                          ds_http_cb_fcn_ex          http_cb_fcn_ex,
                                          ds_http_block_event_cb_fcn block_cb_fcn,
                                          ds_http_iface_info_s_type* iface_info
                                          );
static uint32 ds_http_create_request(
                                     const ds_http_method_e_type     http_method,
                                     const uint32                    session_id,
                                     const char*                     uri_str,
                                     const ds_http_header_s_type*    request_header,
                                     const ds_http_supp_header_info* other_info,
                                     uint16                          reserved_src_port,
                                     sint15*                         request_errno
                                     );

static uint32 ds_http_create_request_helper(
                                            const ds_http_method_e_type     http_method,
                                            const uint32                    session_id,
                                            const char*                     uri_str,
                                            const ds_http_header_s_type*    request_header_info,
                                            const ds_http_supp_header_info* other_info,
                                            uint16                          reserved_src_port,
                                            sint15*                         request_errno
                                            );

static void ds_http_block_msg_handler(uint32* data_ptr);
static void ds_http_remove_request_msg_handler(uint32* data_ptr);
static void ds_http_append_chunked_content_handler(uint32* data_ptr);

static void ds_http_notify_new_session_ready(uint32* data_ptr);


/*==============================================================================
                              Static variables
==============================================================================*/
static ASMutex ds_http_request_crit_sect;
static uint32 request_counter  = 0;



/*==============================================================================
                            Function definitions
==============================================================================*/

/*==============================================================================
                              HTTP Session
==============================================================================*/
uint32 ds_http_open_session(
                            ds_http_cb_fcn             http_cb_fcn,
                            ds_http_block_event_cb_fcn block_cb_fcn,
                            ds_http_iface_info_s_type* iface_info
                            )
{
  return ds_http_open_session_helper(http_cb_fcn, NULL, block_cb_fcn, iface_info);
}


uint32 ds_http_open_session_ex(
                               ds_http_cb_fcn_ex          http_cb_fcn,
                               ds_http_block_event_cb_fcn block_cb_fcn,
                               ds_http_iface_info_s_type* iface_info
                               )
{
  return ds_http_open_session_helper(NULL, http_cb_fcn, block_cb_fcn, iface_info);
}


uint32 ds_http_open_session_helper(
                                   ds_http_cb_fcn             http_cb_fcn,
                                   ds_http_cb_fcn_ex          http_cb_fcn_ex,
                                   ds_http_block_event_cb_fcn block_cb_fcn,
                                   ds_http_iface_info_s_type* iface
                                   )
{
  if(NULL == iface)
    return DS_HTTP_ERROR;

  DATA_APPSRV_MSG3(
                   MSG_LEGACY_MED,
                   "ds_http_open_session - Creating session; iface:%d pid:%d subs:%d",
                   iface->iface_type,
                   iface->profile_id,
                   iface->subs_id
                   );
  ds_http_dsnet_iface_info iface_info(iface);

  uint32 session = ds_http_create_session(http_cb_fcn, http_cb_fcn_ex, block_cb_fcn, iface_info);

  if(DS_HTTP_ERROR != session && ds_http_get_network_status(iface_info.iface_type, iface_info.subs_id))
  {
    uint32* sess_ptr = ds_appsrv_alloc<uint32>();

    if(NULL == sess_ptr)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_open_session - request alloc failed");
      return session;
    }

    *sess_ptr = session;
    if(!ds_http_msg_post(&ds_http_notify_new_session_ready, (uint32*)sess_ptr))
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_open_session - ds_task failed");
      ds_appsrv_free(sess_ptr);
    }
  }
  return session;
}


void ds_http_close_session(const uint32 session_id)
{
  ds_http_remove_session(session_id);
}



boolean ds_http_set_session_config(
                                   const uint32                         session_id,
                                   ds_http_session_config_option_e_type config_type,
                                   ds_http_session_config_value_u_type* config_info
                                   )
{
  return (ds_http_update_session_config(session_id, config_type, config_info)) ? TRUE : FALSE;
}





/*==============================================================================
                              HTTP Request
==============================================================================*/
uint32 ds_http_create_get_request(
                                  const uint32           session_id,
                                  const char*            uri,
                                  ds_http_header_s_type* request_header,
                                  uint16                 reserved_src_port,
                                  sint15*                request_errno
                                  )
{
  ds_http_supp_header_info sup_info;
  return ds_http_create_request(
                                DS_HTTP_METHOD_GET,
                                session_id,
                                uri,
                                request_header,
                                &sup_info,
                                reserved_src_port,
                                request_errno
                                );
}



uint32 ds_http_create_post_request(
                                   const uint32                 session_id,
                                   const char*                  uri,
                                   ds_http_header_s_type*       request_header,
                                   ds_http_content_info_s_type* content_info,
                                   uint16                       reserved_src_port,
                                   sint15*                      request_errno
                                   )
{
  if( NULL == content_info ||
      DS_HTTP_CONTENT_MAX_SIZE < content_info->content_size ||
      (NULL == content_info->content && 0 != content_info->content_size) )
  {
    *request_errno = DS_HTTP_ERROR_INVALID_PARAM;
    return DS_HTTP_ERROR;
  }

  ds_http_supp_header_info add_info;
  add_info.post_info = content_info;

  return ds_http_create_request(
                                DS_HTTP_METHOD_POST,
                                session_id,
                                uri,
                                request_header,
                                &add_info,
                                reserved_src_port,
                                request_errno
                                );
}


uint32 ds_http_create_post_request_chunked(
                                           const uint32                         session_id,
                                           const char*                          uri,
                                           ds_http_header_s_type*               request_header,
                                           ds_http_chunked_content_info_s_type* chunked_content_info,
                                           uint16                               reserved_src_port,
                                           sint15*                              request_errno
                                           )
{
  if( NULL == chunked_content_info ||
      DS_HTTP_CONTENT_MAX_SIZE < chunked_content_info->content_info.content_size ||
      (NULL == chunked_content_info->content_info.content && 0 != chunked_content_info->content_info.content_size) )
  {
    *request_errno = DS_HTTP_ERROR_INVALID_PARAM;
    return DS_HTTP_ERROR;
  }

  ds_http_supp_header_info add_info;
  add_info.chunked_content_info = chunked_content_info;

  return ds_http_create_request(
                                DS_HTTP_METHOD_POST_CHUNKED,
                                session_id,
                                uri,
                                request_header,
                                &add_info,
                                reserved_src_port,
                                request_errno
                                );
}




uint32 ds_http_create_request(
                              const ds_http_method_e_type     http_method,
                              const uint32                    session_id,
                              const char*                     uri_str,
                              const ds_http_header_s_type*    request_header_info,
                              const ds_http_supp_header_info* supp_info,
                              uint16                          reserved_src_port,
                              sint15*                         request_errno
                              )
{
  uint32 request_id = 0;

  if (!ds_http_check_valid_session(session_id))
  {
    *request_errno = DS_HTTP_ERROR_INVALID_SESSION;
    return DS_HTTP_ERROR;
  }

  if(!ds_http_check_dsnet_status(session_id, request_errno))
    return DS_HTTP_ERROR;

  if(NULL == uri_str || 0 == strlen(uri_str))
  {
    *request_errno = DS_HTTP_ERROR_NO_URI_INFO;
    return DS_HTTP_ERROR;
  }

  request_id = ds_http_create_request_helper(
                                             http_method,
                                             session_id,
                                             uri_str,
                                             request_header_info,
                                             supp_info,
                                             reserved_src_port,
                                             request_errno
                                             );

  return request_id;
}



uint32 ds_http_create_request_helper(
                                     const ds_http_method_e_type     http_method,
                                     const uint32                    session_id,
                                     const char*                     uri_str,
                                     const ds_http_header_s_type*    request_header_info,
                                     const ds_http_supp_header_info* supp_info,
                                     uint16                          reserved_src_port,
                                     sint15*                         request_errno
                                     )
{
  ASLockGuard lock(ds_http_request_crit_sect);

  if (DS_HTTP_ERROR == request_counter)
  {
    request_counter = 0; // reset to prevent overflow && prevent request_id==DS_HTTP_ERROR
  }

  uint32           request_id = request_counter;
  ds_http_request* request    = ds_http_request_factory(
                                                        http_method,
                                                        session_id,
                                                        request_id,
                                                        uri_str,
                                                        request_header_info,
                                                        supp_info,
                                                        reserved_src_port,
                                                        request_errno
                                                        );

  if(DS_HTTP_ERROR_NONE != *request_errno)
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_create_request - request error %d", *request_errno);
    return DS_HTTP_ERROR;
  }

  if(!ds_http_add_new_request(request))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_create_request - Failed to add request");
    *request_errno = DS_HTTP_ERROR_MEMALLOC;
    ds_appsrv_free(request);
    return DS_HTTP_ERROR;
  }

  request_counter++;
  return request_id;
}



void ds_http_cancel_request(const uint32 session_id, const uint32 request_id)
{
  ds_http_remove_request_msg* remove_msg = new(ds_appsrv_alloc<ds_http_remove_request_msg>(), APPSRV_MEM) ds_http_remove_request_msg(session_id, request_id);

  if(NULL == remove_msg || !ds_http_msg_post(&ds_http_remove_request_msg_handler, (uint32*)remove_msg))
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_remove_request_event - mem alloc or ds_task failed %d", remove_msg);
    ds_appsrv_free(remove_msg);
  }
}



sint15 ds_http_append_chunked_fragment(
                                     const uint32                         session_id,
                                     const uint32                         request_id,
                                     ds_http_chunked_content_info_s_type* chunked_content_info
                                     )
{
  if( NULL == chunked_content_info ||
      DS_HTTP_CONTENT_MAX_SIZE < chunked_content_info->content_info.content_size ||
      (NULL == chunked_content_info->content_info.content && 0 != chunked_content_info->content_info.content_size) )
  {
    return DS_HTTP_ERROR_INVALID_PARAM;
  }

  ds_http_append_chunk_msg* chunked_content
    = new(ds_appsrv_alloc<ds_http_append_chunk_msg>(), APPSRV_MEM) ds_http_append_chunk_msg(session_id, request_id, chunked_content_info);

  if(NULL == chunked_content || !ds_http_msg_post(&ds_http_append_chunked_content_handler, (uint32*)chunked_content))
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "ds_http_append_chunked_content - mem alloc or ds_task failed %d", chunked_content);
    ds_appsrv_free(chunked_content);
    return DS_HTTP_ERROR_MEMALLOC;
  }

  return DS_HTTP_ERROR_NONE;
}



/*==============================================================================
                          HTTP Block Handlers
==============================================================================*/
void ds_http_resolve_auth_block(
                                uint32                    session_id,
                                uint32                    request_id,
                                ds_http_auth_info_s_type* auth_info
                                )
{
  ds_http_block_msg* block_msg
    = new(ds_appsrv_alloc<ds_http_block_msg>(), APPSRV_MEM) ds_http_block_msg(session_id, request_id, auth_info);

  if(NULL == block_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_resolve_auth_block - Failed to allocate memory for block_msg");
    return;
  }

  if(!ds_http_msg_post(&ds_http_block_msg_handler, (uint32*)block_msg))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_resolve_auth_block - ds_task failed");
    ds_appsrv_free(block_msg);
  }
}



void ds_http_resolve_ssl_cert_block(uint32 session_id, uint32 request_id, boolean cont)
{
  ds_http_block_msg* block_msg
    = new(ds_appsrv_alloc<ds_http_block_msg>(), APPSRV_MEM) ds_http_block_msg(
                                                                              session_id,
                                                                              request_id,
                                                                              (cont == DS_HTTP_SSL_CONTINUE ? true : false)
                                                                              );

  if(NULL == block_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_resolve_ssl_block - Failed to allocate memory for block_msg");
    return;
  }

  if(!ds_http_msg_post(&ds_http_block_msg_handler, (uint32*)block_msg))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_resolve_ssl_block - ds_task failed");
    ds_appsrv_free(block_msg);
  }
}



boolean ds_http_is_nw_srv_avail(ds_http_iface_e_type iface)
{
  return ds_http_is_nw_srv_avail_for_subs_id(iface, DS_HTTP_DEFAULT_SUBS);
}



boolean ds_http_is_nw_srv_avail_for_subs_id(ds_http_iface_e_type iface, ds_http_subscription_e_type subs_id)
{
  return (ds_http_get_network_status(iface, subs_id)) ? TRUE : FALSE;
}



/*==============================================================================
                      ds_http_block_msg Constructors
==============================================================================*/
ds_http_block_msg::ds_http_block_msg(
                                     const uint32                    sid,
                                     const uint32                    rid,
                                     const ds_http_auth_info_s_type* auth
                                     )
: session_id(sid), request_id(rid), block_event(DS_HTTP_BLOCK_AUTHENTICATION), auth_info_set(false), ssl_cont(false)
{
  memset(&auth_info, 0, sizeof(ds_http_auth_info_s_type));

  if(NULL != auth)
  {
    auth_info_set = true;
    memscpy(&auth_info, sizeof(ds_http_auth_info_s_type), auth, sizeof(ds_http_auth_info_s_type));
  }
}

ds_http_block_msg::ds_http_block_msg(
                                     const uint32 sid,
                                     const uint32 rid,
                                     bool         cont
                                     )
: session_id(sid), request_id(rid), block_event(DS_HTTP_BLOCK_SSL_CERTIFICATE_ALERT), auth_info_set(false), ssl_cont(cont)
{
  memset(&auth_info, 0, sizeof(ds_http_auth_info_s_type));
}




/*==============================================================================
                              MSG Handlers
==============================================================================*/
void ds_http_remove_request_msg_handler(uint32* data_ptr)
{
  ds_http_remove_request_msg* remove_req_msg = (ds_http_remove_request_msg*) data_ptr;
  if(NULL == remove_req_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_remove_request_msg_handler - NULL remove_req_msg");
    return;
  }

  ds_http_remove_request(remove_req_msg->session_id, remove_req_msg->request_id);

  ds_appsrv_free(remove_req_msg);
}



void ds_http_append_chunked_content_handler(uint32* data_ptr)
{
  ds_http_append_chunk_msg* chunk_msg = (ds_http_append_chunk_msg*)data_ptr;

  if (NULL == chunk_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_append_chunked_content_handler - NULL chunk_msg");
    return;
  }

  ds_http_append_chunk_to_request(
                                  chunk_msg->session_id,
                                  chunk_msg->request_id,
                                  chunk_msg->chunked_content
                                  );

  ds_appsrv_free(chunk_msg);
}



void ds_http_block_msg_handler(uint32* data_ptr)
{
  ds_http_block_msg* block_msg = (ds_http_block_msg*) data_ptr;

  if (NULL == block_msg)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_block_msg_handler - NULL block_msg");
    return;
  }

  switch(block_msg->block_event)
  {
    case DS_HTTP_BLOCK_AUTHENTICATION:
    {
      ds_http_auth_info_s_type* auth_info = (block_msg->auth_info_set) ? &block_msg->auth_info : NULL;
      ds_http_resolve_auth(block_msg->session_id, block_msg->request_id, auth_info);
    }
    break;

    case DS_HTTP_BLOCK_SSL_CERTIFICATE_ALERT:
    {
      ds_http_resolve_ssl_suspension(block_msg->session_id, block_msg->request_id, block_msg->ssl_cont);
    }
    break;

    default:
      break;
  }

  ds_appsrv_free(block_msg);
}



void ds_http_notify_new_session_ready(uint32* data_ptr)
{
  if (NULL == data_ptr)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_notify_new_session_ready - NULL data_ptr");
    return;
  }

  ds_http_notify_http_ready(*data_ptr);
  ds_appsrv_free(data_ptr);
}


uint32 ds_http_last_netdownreason(uint32 session_id, dss_net_down_reason_type *reason)
{
  return ds_http_get_last_netdownreason(session_id, reason);
}


