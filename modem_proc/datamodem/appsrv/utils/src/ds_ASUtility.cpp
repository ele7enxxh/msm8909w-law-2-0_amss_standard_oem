/*==============================================================================

                        ds_ASUtility.cpp

GENERAL DESCRIPTION
  Utility functions for AS decoding

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_ASUtility.h"
#include "ds_ASString.h"
#include "ds_ASBuffer.h"
#include "ds_ASFStream.h"

#include "ds_appsrv_mem.h"
#include "data_msg.h"
#include <cstdlib> // strtoul
#include <stringl/stringl.h>

extern "C"
{
  #include "fs_public.h"
}


namespace
{
const char DS_AS_NUMBERS[] = "0123456789";
const char DS_AS_HEX_NUMBERS[] = "0123456789ABCDEF";
} /* namespace */

bool Appsrv::Utils::mac_addr_to_bin(const ASString& mac_addr_str, uint8 mac_addr[DS_APPSRV_UTIL_WLAN_MAC_ADDR_LEN])
{
  // 00:18:4d:D0:9d:62
  // 5 + 2*6 = 17
  if(17 != mac_addr_str.size())
    return false;

  memset(mac_addr, 0, sizeof(uint8) * DS_APPSRV_UTIL_WLAN_MAC_ADDR_LEN);

  uint16 offset       = 0;
  const char* str_ptr = mac_addr_str.c_str();
  char hex_str[2]     = {0};

  do
  {
    // check 0~9 || A~F || a~f
    // if(0 == isdigit(*str_ptr))
    //   break;

    // if(0 == isdigit(*(str_ptr+1)))
    //   break;

    if(':' != str_ptr[2] && '\0' != str_ptr[2])
      break;

    hex_str[0] = str_ptr[0];
    hex_str[1] = str_ptr[1];
    mac_addr[offset++] = strtoul(hex_str, NULL, 16);

    if('\0' == str_ptr[2])
      return true;
    else
      str_ptr += 3;
  } while(true);

  DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "mac_addr_to_bin - Invalid mac address string");
  for(uint32 i = 0; i < DS_APPSRV_UTIL_WLAN_MAC_ADDR_LEN; ++i)
  {
    mac_addr[i] = 0;
  }
  return false;
}




// Assumes dst size is big enough for sint31, negative sign and NULL char. strlen(SINT31_MAX)+1+1 = 12
uint16 Appsrv::Utils::sltostr(sint31 value, char dst[DS_UTILS_SINT31_MAX_LEN])
{
  uint16 chrlen   = 0;
  sint31 tmpval   = value;
  bool   negative = false;

  // Check if value is negative
  if(0 > value)
  {
    negative = true;
    value *= -1;
    ++chrlen;
  }

  // Find # of chars needed
  do
  {
    tmpval /= 10;
    ++chrlen;
  } while( 0 != tmpval);


  for(uint16 i = 1; i <= chrlen; ++i)
  {
    tmpval = value % 10;
    dst[chrlen-i] = DS_AS_NUMBERS[tmpval];
    value /= 10;
  }

  dst[chrlen] = '\0';
  if(negative)
    dst[0] = '-';

  return chrlen;
}



// Assumes dst size is big enough for uint32 and NULL char. strlen(UINT32_MAX)+1 = 11
uint16 Appsrv::Utils::ultostr(uint32 value, char dst[DS_UTILS_UINT32_MAX_LEN])
{
  uint16 chrlen = 0;
  uint32 tmpval = value;

  // Find # of chars needed
  do {
    tmpval /= 10;
    ++chrlen;
  } while( 0 != tmpval);


  for(uint16 i = 1; i <= chrlen; ++i)
  {
    tmpval = value % 10;
    dst[chrlen-i] = DS_AS_NUMBERS[tmpval];
    value /= 10;
  }
  dst[chrlen] = '\0';

  return chrlen;
}



uint16 Appsrv::Utils::ultohex(uint32 value, char dst[DS_UTILS_UINT32_HEX_LEN])
{
  uint16 dst_loc = 0;

  for(uint16 i = 0; i < 4; ++i)
  {
    uint32 tmpval = value >> (4 * (3 - i));
    tmpval = tmpval & 0x0F;

    if(0 == tmpval && 0 == dst_loc)
      continue;

    dst[dst_loc++] = DS_AS_HEX_NUMBERS[tmpval];
  }
  dst[dst_loc] = '\0';

  return dst_loc;
}



bool Appsrv::Utils::get_file_content(const char* file_path, ASBuffer& buffer)
{
  int fd = -1;

  if( NULL == file_path || -1 == ( fd = efs_open(file_path, O_RDONLY) ) )
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "get_file_content - Failed to open file %d", file_path);
    return false;
  }

  uint32     buf_size        = 1000;
  fs_ssize_t read_size       = 0;
  char*      internal_buffer = (char*)ds_appsrv_alloc(sizeof(char), buf_size);

  if(NULL == internal_buffer)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "get_file_content - Failed to allocate memory for internal_buffer");
    efs_close(fd);
    return false;
  }

  // Loop until error or end of file
  do
  {
    read_size = efs_read(fd, internal_buffer, buf_size);
    if(0 < read_size)
      buffer.append(internal_buffer, read_size);
    else
      break;
  } while(true);

  ds_appsrv_free(internal_buffer);
  efs_close(fd);

  if(-1 == read_size)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "get_file_content - EFS read error");
    buffer.clear();
    return false;
  }
  return true;
}




bool Appsrv::Utils::copy_file(const char* orig_file_path, const char* copy_file_path)
{
  int fd = -1;

  if( NULL == orig_file_path ||
      NULL == copy_file_path ||
      -1   == ( fd = efs_open(orig_file_path, O_RDONLY) ) )
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "copy_file - NULL file path %d %d or failed to open file", orig_file_path, copy_file_path);
    return false;
  }

  ASOFStream fstream(copy_file_path, O_CREAT | O_TRUNC | O_WRONLY, DEFFILEMODE);

  if(!fstream.is_open())
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "copy_file - Failed to create/open copy file");
    efs_close(fd);
    return false;
  }


  uint32     buf_size        = 1000;
  char*      internal_buffer = (char*)ds_appsrv_alloc(sizeof(char), buf_size+1);
  fs_ssize_t read_size       = 0;

  if(NULL == internal_buffer)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "copy_file - Failed to allocate memory for internal_buffer");
    efs_close(fd);
    return false;
  }

  // Loop until error or end of file
  do
  {
    read_size = efs_read(fd, internal_buffer, buf_size);
    if(0 < read_size)
    {
      fstream.write(internal_buffer, read_size);
      if(!fstream.good())
      {
        break;
      }
    }
    else
    {
      break;
    }
  } while(true);

  ds_appsrv_free(internal_buffer);
  efs_close(fd);

  if( !fstream.good() )
    return false;

  fstream.flush();
  return true;
}
