#ifndef _MRE_DBG_H_
#define _MRE_DBG_H_

/**
  @file mre_dbg.c

  @brief Policy Manager debug macros.
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/mre_dbg.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include <sys/types.h>
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */
#include "mre_util.h"



#if defined(TEST_FRAMEWORK)
#error code not present
#else /* +TEST_FRAMEWORK- */

#define MRE_XML_MSG(str, a, b, c)

#endif /* -TEST_FRAMEWORK. */


/*---------------------------------------------------------------------------
      Compile PM with debug information, asserts and messages.
---------------------------------------------------------------------------*/

#define MRE_MSG_SEPARATOR(which)    mre_util_msg_separator(which)

// Prefix used for most MRE messages with subsId in them
#define SUBS_PREFIX "subs %d: "

// Standard line number string.
#define SRC_LINE    "{line %d} "

// Get the line number from the element
#define LINE_NUM(p) (p)->lineNum


/*---------------------------------------------------------------------------
                  MRE MSG macros
---------------------------------------------------------------------------*/

#if defined(TEST_FRAMEWORK)
#error code not present
#else /* +TEST_FRAMEWORK- */

// Leave message levels unchanged
#define MRE_MSG_LVL_LOW     MSG_LEGACY_LOW
#define MRE_MSG_LVL_MED     MSG_LEGACY_MED

// Disable extra messages on target
#define MRE_UT_MSG_0(str)
#define MRE_UT_MSG_1(str, a)
#define MRE_UT_MSG_2(str, a, b)
#define MRE_UT_MSG_3(str, a, b, c)
#define MRE_UT_MSG_4(str, a, b, c, d)
#define MRE_UT_MSG_5(str, a, b, c, d, e)
#define MRE_UT_MSG_6(str, a, b, c, d, e, f)

#define MRE_UT_MSG_S1(str, a)
#define MRE_UT_MSG_S2(str, a, b)
#define MRE_UT_MSG_S3(str, a, b, c)
#define MRE_UT_MSG_S4(str, a, b, c, d)
#define MRE_UT_MSG_S5(str, a, b, c, d, e)
#define MRE_UT_MSG_S6(str, a, b, c, d, e, f)

#endif /* -TEST_FRAMEWORK. */

#define MRE_MSG_LOW_0(str)                      MSG( MSG_SSID_MRE, MRE_MSG_LVL_LOW,  str)
#define MRE_MSG_MED_0(str)                      MSG( MSG_SSID_MRE, MRE_MSG_LVL_MED,  str)
#define MRE_MSG_HIGH_0(str)                     MSG( MSG_SSID_MRE, MSG_LEGACY_HIGH,  str)
#define MRE_MSG_ERROR_0(str)                    MSG( MSG_SSID_MRE, MSG_LEGACY_ERROR, str)

#define MRE_MSG_LOW_1(str, a)                   MSG_1( MSG_SSID_MRE, MRE_MSG_LVL_LOW,  str, a)
#define MRE_MSG_MED_1(str, a)                   MSG_1( MSG_SSID_MRE, MRE_MSG_LVL_MED,  str, a)
#define MRE_MSG_HIGH_1(str, a)                  MSG_1( MSG_SSID_MRE, MSG_LEGACY_HIGH,  str, a)
#define MRE_MSG_ERROR_1(str, a)                 MSG_1( MSG_SSID_MRE, MSG_LEGACY_ERROR, str, a)

#define MRE_MSG_LOW_2(str, a, b)                MSG_2( MSG_SSID_MRE, MRE_MSG_LVL_LOW,  str, a, b)
#define MRE_MSG_MED_2(str, a, b)                MSG_2( MSG_SSID_MRE, MRE_MSG_LVL_MED,  str, a, b)
#define MRE_MSG_HIGH_2(str, a, b)               MSG_2( MSG_SSID_MRE, MSG_LEGACY_HIGH,  str, a, b)
#define MRE_MSG_ERROR_2(str, a, b)              MSG_2( MSG_SSID_MRE, MSG_LEGACY_ERROR, str, a, b)

#define MRE_MSG_LOW_3(str, a, b, c)             MSG_3( MSG_SSID_MRE, MRE_MSG_LVL_LOW,  str, a, b, c)
#define MRE_MSG_MED_3(str, a, b, c)             MSG_3( MSG_SSID_MRE, MRE_MSG_LVL_MED,  str, a, b, c)
#define MRE_MSG_HIGH_3(str, a, b, c)            MSG_3( MSG_SSID_MRE, MSG_LEGACY_HIGH,  str, a, b, c)
#define MRE_MSG_ERROR_3(str, a, b, c)           MSG_3( MSG_SSID_MRE, MSG_LEGACY_ERROR, str, a, b, c)

#define MRE_MSG_LOW_4(str, a, b, c, d)          MSG_4( MSG_SSID_MRE, MRE_MSG_LVL_LOW,  str, a, b, c, d)
#define MRE_MSG_MED_4(str, a, b, c, d)          MSG_4( MSG_SSID_MRE, MRE_MSG_LVL_MED,  str, a, b, c, d)
#define MRE_MSG_HIGH_4(str, a, b, c, d)         MSG_4( MSG_SSID_MRE, MSG_LEGACY_HIGH,  str, a, b, c, d)
#define MRE_MSG_ERROR_4(str, a, b, c, d)        MSG_4( MSG_SSID_MRE, MSG_LEGACY_ERROR, str, a, b, c, d)

#define MRE_MSG_LOW_5(str, a, b, c, d, e)       MSG_5( MSG_SSID_MRE, MRE_MSG_LVL_LOW,  str, a, b, c, d, e)
#define MRE_MSG_MED_5(str, a, b, c, d, e)       MSG_5( MSG_SSID_MRE, MRE_MSG_LVL_MED,  str, a, b, c, d, e)
#define MRE_MSG_HIGH_5(str, a, b, c, d, e)      MSG_5( MSG_SSID_MRE, MSG_LEGACY_HIGH,  str, a, b, c, d, e)
#define MRE_MSG_ERROR_5(str, a, b, c, d, e)     MSG_5( MSG_SSID_MRE, MSG_LEGACY_ERROR, str, a, b, c, d, e)

#define MRE_MSG_LOW_6(str, a, b, c, d, e, f)    MSG_6( MSG_SSID_MRE, MRE_MSG_LVL_LOW,  str, a, b, c, d, e, f)
#define MRE_MSG_MED_6(str, a, b, c, d, e, f)    MSG_6( MSG_SSID_MRE, MRE_MSG_LVL_MED,  str, a, b, c, d, e, f)
#define MRE_MSG_HIGH_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_MRE, MSG_LEGACY_HIGH,  str, a, b, c, d, e, f)
#define MRE_MSG_ERROR_6(str, a, b, c, d, e, f)  MSG_6( MSG_SSID_MRE, MSG_LEGACY_ERROR, str, a, b, c, d, e, f)

#define MRE_MSG_HIGH(str, a, b, c)              MSG_3( MSG_SSID_MRE, MSG_LEGACY_HIGH,  str, a, b, c)

#define MRE_ERR_FATAL(str, a, b, c)             ERR_FATAL("=MRE= " str, a, b, c)

#define MRE_MSG_S1(str, a)                      MSG_SPRINTF_1( MSG_SSID_MRE, MSG_LEGACY_HIGH, str, a )
#define MRE_MSG_S2(str, a, b)                   MSG_SPRINTF_2( MSG_SSID_MRE, MSG_LEGACY_HIGH, str, a, b )
#define MRE_MSG_S3(str, a, b, c)                MSG_SPRINTF_3( MSG_SSID_MRE, MSG_LEGACY_HIGH, str, a, b, c )
#define MRE_MSG_S4(str, a, b, c, d)             MSG_SPRINTF_4( MSG_SSID_MRE, MSG_LEGACY_HIGH, str, a, b, c, d )
#define MRE_MSG_S5(str, a, b, c, d, e)          MSG_SPRINTF_5( MSG_SSID_MRE, MSG_LEGACY_HIGH, str, a, b, c, d, e )
#define MRE_MSG_S6(str, a, b, c, d, e, f)       MSG_SPRINTF_6( MSG_SSID_MRE, MSG_LEGACY_HIGH, str, a, b, c, d, e, f )

/*---------------------------------------------------------------------------
                  Asserts and verifications macros
---------------------------------------------------------------------------*/

#define MRE_ASSERT( exp )     \
  do                                \
  {                                 \
    if (!(exp))                     \
    {                               \
      MRE_ERR_FATAL("ASSERTION FAILED! " #exp, 0, 0, 0);  \
    }                               \
  } while (0)

#endif /* _MRE_DBG_H_ */
