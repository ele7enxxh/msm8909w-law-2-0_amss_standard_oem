/*===========================================================================

                  GM_Log Header File

DESCRIPTION
  This header file contains definitions of data structure and procedures necessary
  for Geofence logging

Copyright (c) 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.
Copyright (c) 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/15   ssu  Commericialising in memory logging.
12/05/11   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_MEM_LOG_H
#define GM_MEM_LOG_H

#include "comdef.h"

/*===========================================================================

FUNCTION gm_internal_in_mem_logging_start

DESCRIPTION
  This function is for starting in-mem logging.

DEPENDENCIES

RETURN VALUE
  TRUE/FAIL
SIDE EFFECTS
  None.

===========================================================================*/
boolean gm_internal_in_mem_logging_start(sm_gm_in_mem_log_config_type *p_log_config);


/*===========================================================================

FUNCTION gm_internal_in_mem_logging_stop

DESCRIPTION
  This function is for stopping in memory logging.

DEPENDENCIES

RETURN VALUE
  TRUE/FAIL
SIDE EFFECTS
  None.

===========================================================================*/
boolean gm_internal_in_mem_logging_stop(void);

/*===========================================================================

FUNCTION gm_in_mem_log_msg

DESCRIPTION
  This function is for logging a message

DEPENDENCIES

RETURN VALUE
  Void
SIDE EFFECTS
  None.

===========================================================================*/
void gm_in_mem_log_msg(
  const char* func,
  int line,
  uint32 level,
  uint8 num_args,
  const char *p_String,
  uint32 q_Data1,
  uint32 q_Data2,
  uint32 q_Data3,
  uint32 q_Data4,
  uint32 q_Data5,
  uint32 q_Data6,
  uint32 q_Data7,
  uint32 q_Data8,
  uint32 q_Data9);



#define GM_MSG_HIGH(str,a,b,c)  MSG_HIGH(str,a,b,c); gm_in_mem_log_msg(__FILE__,__LINE__,MSG_LEGACY_HIGH,3,str,a,b,c,0,0,0,0,0,0)
#define GM_MSG_MED(str,a,b,c)   MSG_MED(str,a,b,c); gm_in_mem_log_msg(__FILE__,__LINE__,MSG_LEGACY_MED,3,str,a,b,c,0,0,0,0,0,0)
#define GM_MSG_LOW(str,a,b,c)   MSG_LOW(str,a,b,c); gm_in_mem_log_msg(__FILE__,__LINE__,MSG_LEGACY_LOW,3,str,a,b,c,0,0,0,0,0,0)
#define GM_MSG_ERROR(str,a,b,c) MSG_ERROR(str,a,b,c); gm_in_mem_log_msg(__FILE__,__LINE__,MSG_LEGACY_ERROR,3,str,a,b,c,0,0,0,0,0,0)
#define GM_MSG_FATAL(str,a,b,c) MSG_FATAL(str,a,b,c); gm_in_mem_log_msg(__FILE__,__LINE__,MSG_LEGACY_FATAL,3,str,a,b,c,0,0,0,0,0,0)

#define GM_MSG_9(level,str,a,b,c,d,e,f,g,h,i) \
MSG_9(MSG_SSID_DFLT,level,str,a,b,c,d,e,f,g,h,i); \
gm_in_mem_log_msg(__FILE__,__LINE__,level,9,str,a,b,c,d,e,f,g,h,i)

#define GM_MSG_8(level,str,a,b,c,d,e,f,g,h)  \
MSG_8(MSG_SSID_DFLT,level,str,a,b,c,d,e,f,g,h); \
gm_in_mem_log_msg(__FILE__,__LINE__,level,8,str,a,b,c,d,e,f,g,h,0)

#define GM_MSG_7(level,str,a,b,c,d,e,f,g)  \
MSG_7(MSG_SSID_DFLT,level,str,a,b,c,d,e,f,g); \
gm_in_mem_log_msg(__FILE__,__LINE__,level,7,str,a,b,c,d,e,f,g,0,0)

#define GM_MSG_6(level,str,a,b,c,d,e,f)  \
MSG_6(MSG_SSID_DFLT,level,str,a,b,c,d,e,f); \
gm_in_mem_log_msg(__FILE__,__LINE__,level,6,str,a,b,c,d,e,f,0,0,0)

#define GM_MSG_5(level,str,a,b,c,d,e)  \
MSG_5(MSG_SSID_DFLT,level,str,a,b,c,d,e); \
gm_in_mem_log_msg(__FILE__,__LINE__,level,5,str,a,b,c,d,e,0,0,0,0)

#define GM_MSG_4(level,str,a,b,c,d)  \
MSG_4(MSG_SSID_DFLT,level,str,a,b,c,d); \
gm_in_mem_log_msg(__FILE__,__LINE__,level,4,str,a,b,c,d,0,0,0,0,0)

#endif /*GM_MEM_LOG_H*/

