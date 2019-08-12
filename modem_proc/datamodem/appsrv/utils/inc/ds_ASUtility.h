/*==============================================================================

                        ds_ASUtility.h

GENERAL DESCRIPTION
  Utility functions for appsrv

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/09/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_AS_UTILITY_H
#define DS_AS_UTILITY_H

#include "comdef.h"
#include "ds_ASAssignType.h"

#define DS_APPSRV_UTIL_WLAN_MAC_ADDR_LEN 6

class ASString;
class ASBuffer;

namespace Appsrv
{
namespace Utils
{

const uint16 DS_UTILS_SINT31_MAX_LEN = 12;
const uint16 DS_UTILS_UINT32_MAX_LEN = 11;
const uint16 DS_UTILS_UINT32_HEX_LEN = 5;

bool mac_addr_to_bin(const ASString& mac_addr_str, uint8 mac_addr[DS_APPSRV_UTIL_WLAN_MAC_ADDR_LEN]);

bool get_file_content(const char* file_path, ASBuffer& buffer);
bool copy_file(const char* orig_file_path, const char* copy_file_path);

// Signed / Unsigned int to string. Returns resulting stringlen
uint16 sltostr(sint31 value, char dst[DS_UTILS_SINT31_MAX_LEN]);
uint16 ultostr(uint32 value, char dst[DS_UTILS_UINT32_MAX_LEN]);

uint16 ultohex(uint32 value, char dst[DS_UTILS_UINT32_HEX_LEN]);


template <typename T>
const T& get_min(const T& lhs, const T& rhs);

template <typename T>
const T& get_max(const T& lhs, const T& rhs);

template<typename T, typename Y>
bool validator_low(const Utils::ASAssignType<T>& value, const Y compare, bool avail);

template<typename T, typename Y>
bool validator_high(const Utils::ASAssignType<T>& value, const Y compare, bool avail);








template <typename T>
const T& get_min(const T& lhs, const T& rhs)
{
  return (lhs < rhs) ? lhs : rhs;
}


template <typename T>
const T& get_max(const T& lhs, const T& rhs)
{
  return (lhs > rhs) ? lhs : rhs;
}


template<typename T, typename Y>
bool validator_low(const Utils::ASAssignType<T>& value, const Y compare, bool avail)
{
  return (avail && (value.get() > compare));
}


template<typename T, typename Y>
bool validator_high(const Utils::ASAssignType<T>& value, const Y compare, bool avail)
{
  return (avail && (value.get() <= compare));
}



} /* namespace Utils */
} /* namespace Appsrv */

#endif /* DS_AS_UTILITY_H */
