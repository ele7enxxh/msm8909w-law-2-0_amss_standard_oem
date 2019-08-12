/*==============================================================================

                            ds_http_config_info.cpp

GENERAL DESCRIPTION
  DS HTTP configuration information management

  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/22/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_config_info.h"
#include "data_msg.h"
#include <stringl/stringl.h>




ds_http_config_info::ds_http_config_info()
: forward_oversize_response(FALSE)
{
  // Set default config values
  memset(&ssl_config_info, 0, sizeof(ds_http_ssl_config_s_type));
  ssl_config_info.ssl_version = DS_HTTP_SSL_VER_TLS11;
}



ds_http_config_info::ds_http_config_info(const ds_http_config_info& orig)
: forward_oversize_response(FALSE)
{
  copy(orig);
}



ds_http_config_info& ds_http_config_info::operator=(const ds_http_config_info& rhs)
{
  copy(rhs);
  return *this;
}



void ds_http_config_info::copy(const ds_http_config_info& rhs)
{
  // Copy rhs config info
  memscpy(
          &ssl_config_info,
          sizeof(ds_http_ssl_config_s_type),
          &(rhs.ssl_config_info),
          sizeof(ds_http_ssl_config_s_type)
          );

  forward_oversize_response = rhs.forward_oversize_response;
}



bool ds_http_config_info::update_config(
                                         ds_http_session_config_option_e_type       config_type,
                                         const ds_http_session_config_value_u_type* config_info
                                         )
{
  DATA_APPSRV_MSG2(MSG_LEGACY_MED, "Updating session config %d 0x%d", config_type, config_info);
  if(NULL == config_info)
    return false;

  switch(config_type)
  {
    case DS_HTTP_SESSION_CONFIG_SSL:
    {
      memscpy(
              &ssl_config_info,
              sizeof(ds_http_ssl_config_s_type),
              &(config_info->ssl_config),
              sizeof(ds_http_ssl_config_s_type)
              );

      // Unset mask if no cb function is set
      if(NULL == ssl_config_info.ssl_alert_cb_fcn)
      {
        ssl_config_info.ssl_alert_notification_mask = 0;
      }
    }
    break;

    case DS_HTTP_SESSION_CONFIG_FORWARD_OVERSIZE_RESPONSE:
    {
      forward_oversize_response = config_info->forward_oversize_response;
    }
    break;

    default:
      return false;
  }

  return true;
}



void ds_http_config_info::get_config(
                                     ds_http_session_config_option_e_type config_type,
                                     ds_http_session_config_value_u_type* config_info
                                     ) const
{
  if(NULL == config_info)
    return;

  switch(config_type)
  {
    case DS_HTTP_SESSION_CONFIG_SSL:
    {
      memscpy(
              &(config_info->ssl_config),
              sizeof(ds_http_ssl_config_s_type),
              &ssl_config_info,
              sizeof(ds_http_ssl_config_s_type)
              );
    }
    break;

    case DS_HTTP_SESSION_CONFIG_FORWARD_OVERSIZE_RESPONSE:
    {
      config_info->forward_oversize_response = forward_oversize_response;
    }
    break;

    default:
      break;
  }
}





