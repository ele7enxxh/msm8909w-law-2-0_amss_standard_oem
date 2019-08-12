/*==============================================================================

                            ds_http_cookie_info.cpp

GENERAL DESCRIPTION
  Cookie manager for DS HTTP

  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/30/15    ml     Fix parse issue on HTTP date conversion.
06/22/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_cookie_info.h"

#include "ds_http_utility.h"
#include "ds_ASStringStream.h"
#include "ds_appsrv_mem.h"
#include "data_msg.h"
extern "C"
{
  #include "time_svc.h"
}
#include <cstdlib>


ds_http_cookie_pair::ds_http_cookie_pair()
{ }

ds_http_cookie_pair::ds_http_cookie_pair(const ASString& attribute_name, const ASString& attribute_value)
: name(attribute_name), value(attribute_value)
{ }



ds_http_cookie_info::ds_http_cookie_info(const ASString& cookie_header_value, bool& error_flag)
: expiration_set(false), http_only(false), security(false)
{
  ASStringStream iss(cookie_header_value.c_str(), true);
  ASString       getter;
  error_flag = false;

  memset(&expiration, 0, sizeof(time_julian_type));

  // Name/value
  iss.get_next(getter, '=');
  name.name = getter;
  iss.get_next(getter, ';');
  name.value = getter;

  // Attributes
  while(iss.good())
  {
    iss.get_next(getter, ';');
    DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "ds_http_cookie_info - attr %s", getter.c_str());

    if(getter.empty())
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "ds_http_cookie_info - Finished parsing cookie attributes");
      break;
    }

    ASStringStream hss(getter, true);
    ASString attribute_name;
    hss.get_next(attribute_name, "=");
    if(hss.eof())
    {
      if("httponly" == attribute_name)
      {
        http_only = true;
      }
      else if("secure" == attribute_name)
      {
        security = true;
      }
      else
      {
        DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_cookie_info - Unknown cookie flag");
      }
      continue;
    }

    // else
    ASString attribute_value;
    hss.get_next(attribute_value, '\0');

    if("Domain" == attribute_name)
    {
      domain = attribute_value;
    }
    else if("Path" == attribute_name)
    {
      path = attribute_value;
    }
    else if("Expires" == attribute_name)
    {
      if( !parse_http_expires_date(attribute_value, expiration) )
      {
        error_flag = true;
        break;
      }

      expiration_set = true;
    }
    else if("Max-Age" == attribute_name)
    {
      parse_http_max_age(attribute_value, expiration);
      expiration_set = true;
    }
    else
    {
      DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_ERROR, "ds_http_cookie_info - %s is not a cookie attribute", attribute_name.c_str());
      error_flag = true;
      break;
    }
  }
}



void ds_http_cookie_info::get_cookie_info(ds_http_cookie_info_s_type& cookie_info) const
{
  // Copy cookie name and value
  memscpy(
          cookie_info.name_value.name,
          DS_HTTP_COOKIE_NAME_MAX_LEN,
          name.name.c_str(),
          name.name.length()
          );
  memscpy(
          cookie_info.name_value.value,
          DS_HTTP_COOKIE_VALUE_MAX_LEN,
          name.value.c_str(),
          name.value.length()
          );

  // populate attribute fields
  cookie_info.domain         = domain.c_str();
  cookie_info.path           = path.c_str();
  cookie_info.expiration     = expiration; // Value should be ignored if expiration_set is false.
  cookie_info.expiration_set = (expiration_set) ? TRUE : FALSE;
  cookie_info.http_only      = (http_only) ? TRUE : FALSE;
  cookie_info.security       = (security) ? TRUE : FALSE;
}

