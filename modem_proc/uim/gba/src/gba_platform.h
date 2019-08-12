#ifndef GBA_PLATFORM_H
#define GBA_PLATFORM_H
/*===========================================================================


            G B A   P L A T F O R M   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_msg.h"
#include "modem_mem.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define GBA_MALLOC(size)  modem_mem_calloc(1, size, MODEM_MEM_CLIENT_UIM)

#define GBA_FREE(p_mem)                                            \
  if (p_mem != NULL)                                               \
  {                                                                \
    modem_mem_free(p_mem, MODEM_MEM_CLIENT_UIM);                   \
    p_mem = NULL;                                                  \
  }

#define GBA_MSG_HIGH_0(xx_fmt) \
  UIM_MSG_HIGH_0(xx_fmt)

#define GBA_MSG_HIGH_1(xx_fmt, xx_arg1) \
  UIM_MSG_HIGH_1(xx_fmt, xx_arg1)

#define GBA_MSG_HIGH_2(xx_fmt, xx_arg1, xx_arg2) \
  UIM_MSG_HIGH_2(xx_fmt, xx_arg1, xx_arg2)

#define GBA_MSG_HIGH_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  UIM_MSG_HIGH_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define GBA_MSG_HIGH_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  UIM_MSG_HIGH_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define GBA_MSG_HIGH_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  UIM_MSG_HIGH_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define GBA_MSG_HIGH_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  UIM_MSG_HIGH_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define GBA_MSG_MED_0(xx_fmt) \
  UIM_MSG_MED_0(xx_fmt)

#define GBA_MSG_MED_1(xx_fmt, xx_arg1) \
  UIM_MSG_MED_1(xx_fmt, xx_arg1)

#define GBA_MSG_MED_2(xx_fmt, xx_arg1, xx_arg2) \
  UIM_MSG_MED_2(xx_fmt, xx_arg1, xx_arg2)

#define GBA_MSG_MED_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  UIM_MSG_MED_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define GBA_MSG_MED_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  UIM_MSG_MED_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define GBA_MSG_MED_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  UIM_MSG_MED_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define GBA_MSG_MED_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  UIM_MSG_MED_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define GBA_MSG_LOW_0(xx_fmt) \
  UIM_MSG_LOW_0(xx_fmt)

#define GBA_MSG_LOW_1(xx_fmt, xx_arg1) \
  UIM_MSG_LOW_1(xx_fmt, xx_arg1)

#define GBA_MSG_LOW_2(xx_fmt, xx_arg1, xx_arg2) \
  UIM_MSG_LOW_2(xx_fmt, xx_arg1, xx_arg2)

#define GBA_MSG_LOW_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  UIM_MSG_LOW_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define GBA_MSG_LOW_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  UIM_MSG_LOW_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define GBA_MSG_LOW_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  UIM_MSG_LOW_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define GBA_MSG_LOW_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  UIM_MSG_LOW_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define GBA_MSG_ERR_0(xx_fmt) \
  UIM_MSG_ERR_0(xx_fmt)

#define GBA_MSG_ERR_1(xx_fmt, xx_arg1) \
  UIM_MSG_ERR_1(xx_fmt, xx_arg1)

#define GBA_MSG_ERR_2(xx_fmt, xx_arg1, xx_arg2) \
  UIM_MSG_ERR_2(xx_fmt, xx_arg1, xx_arg2)

#define GBA_MSG_ERR_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  UIM_MSG_ERR_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define GBA_MSG_ERR_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  UIM_MSG_ERR_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters
---------------------------------------------------------------------------*/
#define GBA_MSG_SPRINTF_1(xx_fmt, xx_arg1) \
  MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters
---------------------------------------------------------------------------*/
#define GBA_MSG_SPRINTF_2(xx_fmt, xx_arg1, xx_arg2) \
  MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2)

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters
---------------------------------------------------------------------------*/
#define GBA_MSG_SPRINTF_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_SPRINTF_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#endif /* GBA_PLATFORM_H */

