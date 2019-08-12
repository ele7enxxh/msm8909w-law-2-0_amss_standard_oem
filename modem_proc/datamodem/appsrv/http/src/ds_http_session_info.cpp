/*==============================================================================

                            ds_http_session_info.cpp

GENERAL DESCRIPTION
  DS HTTP Session information handler

  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/20/16    ml     Session option to handle response size > 100k
02/20/16    ml     Multiple SSL version support
06/22/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_session_info.h"
#include "ds_http_dsnet.h"


uint32 ds_http_session_info::session_counter = 0;


ds_http_session_info::ds_http_session_info()
: session_id(DS_HTTP_ERROR),
  http_dsnet(NULL),
  http_cb_fcn(NULL),
  http_cb_fcn_ex(NULL),
  block_cb_fcn(NULL)
{ }



ds_http_session_info::ds_http_session_info(
                                           ds_http_cb_fcn             http_cb,
                                           ds_http_cb_fcn_ex          http_cb_ex,
                                           ds_http_block_event_cb_fcn block_cb,
                                           ds_http_dsnet*             dsnet
                                           )
: session_id(session_counter++),
  http_dsnet(dsnet),
  http_cb_fcn(http_cb),
  http_cb_fcn_ex(http_cb_ex),
  block_cb_fcn(block_cb),
  config_locked(false)
{
  if(NULL != http_dsnet)
  {
    ++http_dsnet->reference_counter;
  }

  if(DS_HTTP_ERROR == session_counter)
  {
    session_counter = 0;
  }
}


ds_http_session_info::ds_http_session_info(const ds_http_session_info* session_info)
: session_id(DS_HTTP_ERROR),
  http_dsnet(NULL),
  http_cb_fcn(NULL),
  http_cb_fcn_ex(NULL),
  block_cb_fcn(NULL),
  config_locked(true)
{
  if(NULL == session_info)
    return;

  session_id     = session_info->session_id;
  http_cb_fcn    = session_info->http_cb_fcn;
  http_cb_fcn_ex = session_info->http_cb_fcn_ex;
  block_cb_fcn   = session_info->block_cb_fcn;
}


ds_http_session_info::~ds_http_session_info()
{
  if(NULL != http_dsnet && 0 != http_dsnet->reference_counter)
  {
    --http_dsnet->reference_counter;
  }
}


uint32 ds_http_session_info::get_session_id() const
{
  return session_id;
}


ds_http_dsnet* ds_http_session_info::get_dsnet() const
{
  return http_dsnet;
}


void ds_http_session_info::notify_http_cb(
                                          uint32                              request_id,
                                          sint15                              error,
                                          const ds_http_response_info_s_type* response_info
                                          ) const
{
  if(NULL != http_cb_fcn_ex)
  {
    (http_cb_fcn_ex)(session_id, request_id, error, response_info);
  }
  else if(NULL != http_cb_fcn)
  {
    if(NULL == response_info)
    {
      (http_cb_fcn)(session_id, request_id, error, 0, NULL, 0, NULL, 0);
    }
    else
    {
      (http_cb_fcn)(
                    session_id,
                    request_id,
                    error,
                    response_info->http_status,
                    response_info->content,
                    response_info->content_size,
                    response_info->header_info,
                    response_info->num_headers
                    );
    }
  }
}


void ds_http_session_info::notify_block_cb(
                                           uint32                         request_id,
                                           ds_http_block_event_type       event,
                                           ds_http_block_event_info_type* event_info
                                           ) const
{
  if(NULL != block_cb_fcn)
  {
    (block_cb_fcn)(session_id, request_id, event, event_info);
  }
}



void ds_http_session_info::clear_cb()
{
  http_cb_fcn    = NULL;
  http_cb_fcn_ex = NULL;
  block_cb_fcn   = NULL;
}


void ds_http_session_info::lock_config()
{
  config_locked = true;
}


bool ds_http_session_info::update_config(
                                         ds_http_session_config_option_e_type config_type,
                                         ds_http_session_config_value_u_type* config_info
                                         )
{
  if(config_locked)
    return false;

  return session_config_info.update_config(config_type, config_info);
}



void ds_http_session_info::get_config(
                                      ds_http_session_config_option_e_type config_type,
                                      ds_http_session_config_value_u_type* config_info
                                      ) const
{
  session_config_info.get_config(config_type, config_info);
}

