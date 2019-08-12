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
11/02/15    ml     Permanent redirect
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_HTTP_UTILITY_H
#define DS_HTTP_UTILITY_H

#include "comdef.h"

#include "ds_ASAssignType.h"
extern "C"
{
  #include "time_jul.h"
}

class ASString;


/*==============================================================================
FUNCTION is_numeric_host

DESCRIPTION
  Checks if the hostname is a numeric host

PARAMETERS
  [In] hostname - The hostname to check

RETURN VALUE
  True if hostname is a numeric host, else false.

DEPENDENCIES
  None
==============================================================================*/
bool is_numeric_host(const char* hostname);


/*==============================================================================
FUNCTION has_carriage_return

DESCRIPTION
  Checks if the string has a carriage return "\r\n"

PARAMETERS
  [In] str - The string to check
  [In] len - Length of the string

RETURN VALUE
  True if string has a carriage return, else false.

DEPENDENCIES
  None
==============================================================================*/
bool has_carriage_return(const char* str, uint32 len);


/*==============================================================================
FUNCTION hex_to_hex_str

DESCRIPTION
  Converts the given input's byte into to a hex string.

PARAMETERS
  [In]  hex - Hex value to convert to hex string
  [In]  chr - Char byte to convert to hex string
  [Out] str - String output

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void hex_to_hex_str(unsigned char hex[16], char str[32]);
void char_to_hex_str(char chr, char str[2]);


/*==============================================================================
FUNCTION uitoa

DESCRIPTION
  Converts unsigned int to string

PARAMETERS
  [In]  value - Int value to convert to string
  [Out] str   - String output

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
char* uitoa(uint32 value, char str[11]);



bool parse_http_expires_date(const ASString& expires_str, time_julian_type& expiration_date);

uint32 parse_http_max_age(const ASString& max_age_str, time_julian_type& expiration_date);


struct ds_http_cache_control_info
{
  // Note: Add other fields as necessary if cache-control header needs to be used
  bool no_cache;
  bool no_store;
  Appsrv::Utils::ASAssignType<uint32> max_age;

  ds_http_cache_control_info(const char* cache_control_str);
  ds_http_cache_control_info(const ASString& cache_control_str);

private:
  void init(const char* cache_control_str);
};

#endif /* DS_HTTP_UTILITY_H */
