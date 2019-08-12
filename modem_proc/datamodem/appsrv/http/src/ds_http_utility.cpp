/*==============================================================================

                              ds_http_utility.h

GENERAL DESCRIPTION
  Utility functions for ds_http

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/02/15    ml     HTTP Post chunked support
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_http_utility.h"

#include "ds_ASString.h"
#include "ds_ASStringStream.h"

#include "data_msg.h"
#include <stringl/stringl.h>
#include <cctype> // isdigit()
#include <cstdlib>

extern "C"
{
  #include "time_jul.h"
  #include "time_svc.h"
}

static const char hex_str[]      = "0123456789abcdef";
static const char digit[]        = "0123456789";

static const char month_list[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const char dow_list[7][4] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

uint16 parse_expires_value(const ASString& expires, uint32& offset, uint32 len);
bool parse_expires_delim_validator(const ASString& expires, uint32& offset, const char* expected);
bool parse_expires_date_delim_validator(const ASString& expires, uint32& offset);
uint16 parse_month(const ASString& expires, uint32& offset);
uint16 parse_day_of_week(const ASString& expires, uint32& offset);


bool is_numeric_host(const char* hostname)
{
  while('\0' != *hostname)
  {
    if(0 == isdigit(*hostname) && '.' != *hostname)
      return false;
    hostname++;
  }
  return true;
}



/* Returns true if the string contains \r\n */
bool has_carriage_return(const char* str, uint32 len)
{
  return (NULL != strstr(str, "\r\n"));
}



void hex_to_hex_str(unsigned char hex[16], char str[32])
{
  unsigned int a = 0, b = 0;

  for(int i=0; i < 16; ++i)
  {
    a = hex[i]>>4;
    a = a & 0x0f;
    b = hex[i] & 0x0f;

    str[i*2] = hex_str[a];
    str[i*2+1] = hex_str[b];
  }
}



void char_to_hex_str(char chr, char str[2])
{
  unsigned int a = 0, b = 0;

  a = chr>>4;
  a = a & 0x0f;
  b = chr & 0x0f;

  str[0] = hex_str[a];
  str[1] = hex_str[b];
}



char* uitoa(uint32 value, char str[11]){
  uint32 offset = 0;
  uint32 mod    = 0;

  if(0 == value)
  {
    str[0] = digit[0];
    return str;
  }

  // find offset
  mod = value/10;
  while(0 != mod)
  {
    offset++;
    mod /= 10;
  }

  // assign char values to string
  while(0 != value)
  {
    mod = value%10;
    str[offset] = digit[mod];
    value /= 10;
    --offset;
  }
  return str;
}




uint32 parse_http_max_age(const ASString& max_age_str, time_julian_type& expiration_date)
{
  // Add expire seconds to current time.
  uint32 expire_sec = strtoul(max_age_str.c_str(), NULL, 10);
  time_jul_from_secs( time_get_secs() + expire_sec, &expiration_date );
  return expire_sec;
}



bool parse_http_expires_date(const ASString& expires_str, time_julian_type& expiration_date)
{
  // Fixed format : Wdy, DD Mon YYYY HH:MM:SS GMT or Wdy, DD-Mon-YYYY HH:MM:SS GMT
  uint32 offset = 0;

  if( 29 != expires_str.length() )
  {
    DATA_APPSRV_MSG_SPRINTF_2(
                              MSG_LEGACY_ERROR,
                              "parse_http_expires_date - string too long len:%d value:%s",
                              expires_str.length(),
                              expires_str.c_str()
                              );
    return false;
  }

  // 'Wdy, '
  expiration_date.day_of_week = parse_day_of_week(expires_str, offset);
  if(!parse_expires_delim_validator(expires_str, offset, ", "))
    return false;
  // 'DD '
  expiration_date.day = parse_expires_value(expires_str, offset, 2);
  if(!parse_expires_date_delim_validator(expires_str, offset))
    return false;
  // 'Mon '
  expiration_date.month = parse_month(expires_str, offset);
  if(!parse_expires_date_delim_validator(expires_str, offset))
    return false;
  // 'YYYY '
  expiration_date.year = parse_expires_value(expires_str, offset, 4);
  if(!parse_expires_delim_validator(expires_str, offset, " "))
    return false;
  // 'HH:'
  expiration_date.hour = parse_expires_value(expires_str, offset, 2);
  if(!parse_expires_delim_validator(expires_str, offset, ":"))
    return false;
  // 'MM:'
  expiration_date.minute = parse_expires_value(expires_str, offset, 2);
  if(!parse_expires_delim_validator(expires_str, offset, ":"))
    return false;
  // 'ss '
  expiration_date.second = parse_expires_value(expires_str, offset, 2);
  if(!parse_expires_delim_validator(expires_str, offset, " "))
    return false;
  // 'GMT'
  if(!parse_expires_delim_validator(expires_str, offset, "GMT"))
    return false;

  // Validate Julian time values
  if(
     6    <  expiration_date.day_of_week ||
     0    == expiration_date.day         ||
     31   <  expiration_date.day         ||
     0    == expiration_date.month       ||
     12   <  expiration_date.month       ||
     1980 >  expiration_date.year        ||
     2100 <  expiration_date.year        ||
     24   <= expiration_date.hour        ||
     60   <= expiration_date.minute      ||
     60   <= expiration_date.second
     )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ds_http_cookie_info - Invalid Julian time value");
    return false;
  }

  return true;
}



uint16 parse_expires_value(const ASString& expires, uint32& offset, uint32 len)
{
  ASString value_str(expires.c_str()+offset, len);
  offset += len;
  return strtoul(value_str.c_str(), 0, 10);
}


bool parse_expires_delim_validator(const ASString& expires, uint32& offset, const char* expected)
{
  uint32   len = strlen(expected);
  ASString delim_str(expires.c_str()+offset, len);
  offset += len;
  return (delim_str == expected);
}


bool parse_expires_date_delim_validator(const ASString& expires, uint32& offset)
{
  char delim_chr = *(expires.c_str()+offset);
  offset += 1;
  return ('-' == delim_chr || ' ' == delim_chr);
}



uint16 parse_month(const ASString& expires, uint32& offset)
{
  ASString mon_str(expires.c_str()+offset, 3);
  offset += 3;

  for (uint32 i = 0; i < 12; ++i)
  {
    if(month_list[i] == mon_str)
    {
      return i+1;
    }
  }

  return 13;
}


uint16 parse_day_of_week(const ASString& expires, uint32& offset)
{
  ASString dow_str(expires.c_str()+offset, 3);
  offset += 3;

  for (uint32 i = 0; i < 7; ++i)
  {
    if(dow_list[i] == dow_str)
    {
      return i;
    }
  }

  return 7;
}








ds_http_cache_control_info::ds_http_cache_control_info(const char* cache_control_str)
: no_cache(false), no_store(false), max_age(0)
{
  init( cache_control_str );
}


ds_http_cache_control_info::ds_http_cache_control_info(const ASString& cache_control_str)
: no_cache(false), no_store(false), max_age(0)
{
  init( cache_control_str.c_str() );
}



void ds_http_cache_control_info::init(const char* cache_control_str)
{
  ASStringStream iss(cache_control_str, true);
  ASString       getter;

  // Attributes
  while(iss.good())
  {
    iss.get_next(getter, ',');

    if(getter.empty())
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_LOW, "ds_http_cache_control_info - Finished parsing cache-control");
      break;
    }

    ASStringStream hss(getter, true);
    ASString directive_name;
    hss.get_next(directive_name, "=");
    if(hss.eof())
    {
      if("no-cache" == directive_name)
      {
        no_cache = true;
      }
      else if("no-store" == directive_name)
      {
        no_store = true;
      }
      continue;
    }

    // else
    ASString directive_value;
    hss.get_next(directive_value, '\0');
    DATA_APPSRV_MSG_SPRINTF_1(MSG_LEGACY_MED, "ds_http_cache_control_info - attr %s", directive_value.c_str());

    if("Max-Age" == directive_name)
    {
      max_age = strtoul(directive_value.c_str(), NULL, 10);
    }
    // TODO: Support for no-cache on specific fields.
    // else if("no-cache" == directive_name)
    // {
    //   no_cache = true;
    // }
  }
}
