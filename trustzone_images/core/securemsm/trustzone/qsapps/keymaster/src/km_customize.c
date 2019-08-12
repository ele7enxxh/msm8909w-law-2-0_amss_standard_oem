/* =================================================================================
 *  Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 * =================================================================================
 */

/*
 * Included Files
 */
#include "libstd/stringl/stringl.h"
#include <comdef.h>

/* Customize the fingerprint TA name. Default value is defined, if it needs to be changed then the 
 * default name assigned shall be modified as per the need. 
 */
int32 get_fpta_name(char *fpta_name_str, uint32 fpta_name_len)
{
   const char* default_fpta_name = "fpctzappfingerprint";

   if (fpta_name_str && (strlen(default_fpta_name) <= fpta_name_len))
   {
      strlcpy(fpta_name_str, default_fpta_name, fpta_name_len);
      return 0;
   }
   else
   {
	   return -1;
   }
}



/* OEM specific requirement for GK Retry Timeout, default is 0.
 */
uint32 get_gk_timeout_config(void)
{
  const uint32 gatekeeper_timeout_config = 0;
  return gatekeeper_timeout_config;
}


