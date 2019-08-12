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
06/22/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_CONFIG_INFO_H
#define DS_HTTP_CONFIG_INFO_H

#include "ds_http_types.h"


class ds_http_config_info
{
public:
  ds_http_config_info();
  ds_http_config_info(const ds_http_config_info& rhs);
  ds_http_config_info& operator=(const ds_http_config_info& rhs);


  bool update_config(
                     ds_http_session_config_option_e_type       config_type,
                     const ds_http_session_config_value_u_type* config_info
                     );
  void get_config(
                  ds_http_session_config_option_e_type config_type,
                  ds_http_session_config_value_u_type* config_info
                  ) const;

private:
  void copy(const ds_http_config_info& rhs);

private:
  ds_http_ssl_config_s_type ssl_config_info;
  boolean                   forward_oversize_response;
};



#endif /* DS_HTTP_CONFIG_INFO_H */
