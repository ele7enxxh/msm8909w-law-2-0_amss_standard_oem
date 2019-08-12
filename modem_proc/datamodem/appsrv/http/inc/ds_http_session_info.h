/*==============================================================================

                            ds_http_session_info.h

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
#ifndef DS_HTTP_SESSION_INFO_H
#define DS_HTTP_SESSION_INFO_H

#include "ds_http_types.h"
#include "ds_http_config_info.h"

class ds_http_dsnet;



class ds_http_session_info
{
public:
  ds_http_session_info();
  ds_http_session_info(
                       ds_http_cb_fcn             http_cb,
                       ds_http_cb_fcn_ex          http_cb_ex,
                       ds_http_block_event_cb_fcn block_cb,
                       ds_http_dsnet*             dsnet
                       );
  ds_http_session_info(const ds_http_session_info* session_info);
  ~ds_http_session_info();

  uint32 get_session_id() const;
  ds_http_dsnet* get_dsnet() const;

  void notify_http_cb(
                      uint32                              request_id,
                      sint15                              error,
                      const ds_http_response_info_s_type* response_info
                      ) const;
  void notify_block_cb(
                       uint32                         request_id,
                       ds_http_block_event_type       event,
                       ds_http_block_event_info_type* event_info
                       ) const;

  void clear_cb();

  void lock_config();
  bool update_config(
                     ds_http_session_config_option_e_type config_type,
                     ds_http_session_config_value_u_type* config_info
                     );
  void get_config(
                  ds_http_session_config_option_e_type config_type,
                  ds_http_session_config_value_u_type* config_info
                  ) const;



private:
  uint32                     session_id;
  ds_http_dsnet*             http_dsnet;
  ds_http_cb_fcn             http_cb_fcn;
  ds_http_cb_fcn_ex          http_cb_fcn_ex;
  ds_http_block_event_cb_fcn block_cb_fcn;
  ds_http_config_info        session_config_info;
  bool                       config_locked;

  // Session counter for session ID management
  static uint32              session_counter;
};


#endif /* DS_HTTP_SESSION_INFO_H */


