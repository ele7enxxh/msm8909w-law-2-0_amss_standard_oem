/*============================================================================
  FILE:         le_msg.h

  OVERVIEW:     Defines MSG_LOW_n to MSG_ERROR_n to use MGPPE SSID & MSG_n
                for realtime & PE Playback tool

  DEPENDENCIES: NA
                
                Copyright (c) 2015 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/le/src/le_msg.h#1 $ 
   $DateTime: 2016/12/13 07:59:45 $ 
   $Author: mplcsds1 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-06  wtr  new file for LE module message tracing

============================================================================*/

#ifndef _LE_MSG_H
#define _LE_MSG_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "msg.h" /* for MSG_* */

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

// F3 MSG Logging MACROS (for use by LE)

// added _0 after the macro that doesn't take any additional parameters other 
// than the format string to remind the user that he/she shall change to 
// _1/_2/_3 if there are more parameters to print

#ifndef FEATURE_CGPS_PLAYBACK

// TODO: Consider requesting a new SSID for LE, seperate from legacy MGP PE
// (NF & CD)  - legacy MGP PE can remain for CD, and remnants of NF
#define LE_MSG_0(xx_ss_mask, fmt) \
          MSG(MSG_SSID_MGPPE, xx_ss_mask, fmt)

#define LE_MSG_1(xx_ss_mask, fmt, arg1) \
          MSG_1(MSG_SSID_MGPPE, xx_ss_mask, fmt, arg1)

#define LE_MSG_2(xx_ss_mask, fmt, arg1, arg2) \
          MSG_2(MSG_SSID_MGPPE, xx_ss_mask, fmt, arg1, arg2)

#define LE_MSG_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
          MSG_3(MSG_SSID_MGPPE, xx_ss_mask, fmt, arg1, arg2, arg3)

#else /* FEATURE_CGPS_PLAYBACK is defined */

#define LE_MSG_0(xx_ss_mask, fmt)          PB_MSG_0("LE:", xx_ss_mask, fmt)
#define LE_MSG_1(xx_ss_mask, fmt, a)       PB_MSG_1("LE:", xx_ss_mask, fmt, a)
#define LE_MSG_2(xx_ss_mask, fmt, a, b)    PB_MSG_2("LE:", xx_ss_mask, fmt, a, b)
#define LE_MSG_3(xx_ss_mask, fmt, a, b, c) \
  PB_MSG_3("LE:", xx_ss_mask, fmt, a, b, c)

#endif /* FEATURE_CGPS_PLAYBACK */
                   
// added _0 after the macro that doesn't take any additional parameters other 
// than the format string
// this is to remind the user that he/she shall change to _1/_2/_3 if there are 
// more to print
#define LE_MSG_LOW_0(str)         LE_MSG_0(MSG_LEGACY_LOW, str)
#define LE_MSG_LOW_1(str,a)       LE_MSG_1(MSG_LEGACY_LOW, str, a)
#define LE_MSG_LOW_2(str,a,b)     LE_MSG_2(MSG_LEGACY_LOW, str, a, b)
#define LE_MSG_LOW_3(str,a,b,c)   LE_MSG_3(MSG_LEGACY_LOW, str, a, b, c)

#define LE_MSG_MED_0(str)         LE_MSG_0(MSG_LEGACY_MED, str)
#define LE_MSG_MED_1(str,a)       LE_MSG_1(MSG_LEGACY_MED, str, a)
#define LE_MSG_MED_2(str,a,b)     LE_MSG_2(MSG_LEGACY_MED, str, a, b)
#define LE_MSG_MED_3(str,a,b,c)   LE_MSG_3(MSG_LEGACY_MED, str, a, b, c)

#define LE_MSG_HIGH_0(str)        LE_MSG_0(MSG_LEGACY_HIGH, str)
#define LE_MSG_HIGH_1(str,a)      LE_MSG_1(MSG_LEGACY_HIGH, str, a)
#define LE_MSG_HIGH_2(str,a,b)    LE_MSG_2(MSG_LEGACY_HIGH, str, a, b)
#define LE_MSG_HIGH_3(str,a,b,c)  LE_MSG_3(MSG_LEGACY_HIGH, str, a, b, c)

#define LE_MSG_ERROR_0(str)       LE_MSG_0(MSG_LEGACY_ERROR, str)
#define LE_MSG_ERROR_1(str,a)     LE_MSG_1(MSG_LEGACY_ERROR, str, a)
#define LE_MSG_ERROR_2(str,a,b)   LE_MSG_2(MSG_LEGACY_ERROR, str, a, b)
#define LE_MSG_ERROR_3(str,a,b,c) LE_MSG_3(MSG_LEGACY_ERROR, str, a, b, c)

/* Avoid using a message w/unknown SSID in files containing le_msg.h */
#undef MSG_ERROR
#undef MSG_HIGH
#undef MSG_MED
#undef MSG_LOW

#endif  /* _LE_MSG_H */
