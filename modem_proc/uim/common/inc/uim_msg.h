#ifndef UIM_MSG_H
#define UIM_MSG_H
/*===========================================================================


           U I M   L O G   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/common/inc/uim_msg.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/17/16   vr      Add MSG_7 to MSG_9 capabilities
05/12/16   sp      F3 improvements
01/29/14   sam     minor correction in macro
01/10/14   df      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "msg.h"

/*=============================================================================

                          UIM MSG MACROS
               MACRO Definitions used for logging throughout UIM

=============================================================================*/

#define UIM_MSG_HIGH_0(xx_fmt) \
  MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt)

#define UIM_MSG_HIGH_1(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1)

#define UIM_MSG_HIGH_2(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2)

#define UIM_MSG_HIGH_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define UIM_MSG_HIGH_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define UIM_MSG_HIGH_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define UIM_MSG_HIGH_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define UIM_MSG_HIGH_7(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7)

#define UIM_MSG_HIGH_8(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, \
        xx_arg8)

#define UIM_MSG_HIGH_9(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, \
        xx_arg8, xx_arg9)

#define UIM_MSG_MED_0(xx_fmt) \
  MSG(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt)

#define UIM_MSG_MED_1(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1)

#define UIM_MSG_MED_2(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2)

#define UIM_MSG_MED_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define UIM_MSG_MED_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define UIM_MSG_MED_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define UIM_MSG_MED_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define UIM_MSG_MED_7(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7)

#define UIM_MSG_MED_8(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, \
        xx_arg8)

#define UIM_MSG_MED_9(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_MED, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, \
        xx_arg8, xx_arg9)

#define UIM_MSG_LOW_0(xx_fmt) \
  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt)

#define UIM_MSG_LOW_1(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1)

#define UIM_MSG_LOW_2(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2)

#define UIM_MSG_LOW_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define UIM_MSG_LOW_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define UIM_MSG_LOW_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define UIM_MSG_LOW_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define UIM_MSG_LOW_7(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7)

#define UIM_MSG_LOW_8(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, \
        xx_arg8)

#define UIM_MSG_LOW_9(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_LOW, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, \
        xx_arg8, xx_arg9)

#define UIM_MSG_ERR_0(xx_fmt) \
  MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt)

#define UIM_MSG_ERR_1(xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1)

#define UIM_MSG_ERR_2(xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2)

#define UIM_MSG_ERR_3(xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define UIM_MSG_ERR_4(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define UIM_MSG_ERR_5(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define UIM_MSG_ERR_6(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define UIM_MSG_ERR_7(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  MSG_7(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7)

#define UIM_MSG_ERR_8(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  MSG_8(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, \
        xx_arg8)

#define UIM_MSG_ERR_9(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  MSG_9(MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, \
        xx_arg8, xx_arg9)

/* ************************************************************************
   To enforce use of the new macros for off-target compilation, each of the
   legacy macros are undef, and then defined as an error message. They will
   continue to work on target (with the penalty of wasted bytes for the
   excess parameters.

   core\api\services\msg_diag_service.h has the list of legacy macros.
   ************************************************************************/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/

#endif /* UIM_MSG_H */
