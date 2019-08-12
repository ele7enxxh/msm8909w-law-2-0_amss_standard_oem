/*==============================================================================

                          ds_http_cookie_info.h

GENERAL DESCRIPTION
  Cookie header manager/parser for DS HTTP

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
#ifndef DS_HTTP_COOKIE_INFO_H
#define DS_HTTP_COOKIE_INFO_H

#include "ds_http_types.h"
#include "ds_ASString.h"
#include "ds_ASVector.h"

struct ds_http_cookie_pair
{
  ASString name;
  ASString value;

  ds_http_cookie_pair();
  ds_http_cookie_pair(const ASString& attribute_name, const ASString& attribute_value);
};

class ds_http_cookie_info
{
public:
  ds_http_cookie_info(const ASString& cookie_header_value, bool& error_flag);

  void get_cookie_info(ds_http_cookie_info_s_type& cookie_info) const;

public:
  /* Cookie name value pair */
  ds_http_cookie_pair name;
  /* Cookie attributes */
  ASString            domain;
  ASString            path;
  bool                expiration_set;
  time_julian_type    expiration;
  /* Cookie flags */
  bool                http_only;
  bool                security;

  ds_http_cookie_info() { }
};


#endif /* DS_HTTP_COOKIE_INFO_H */
