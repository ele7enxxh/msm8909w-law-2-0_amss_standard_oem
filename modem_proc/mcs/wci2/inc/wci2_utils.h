#ifndef WCI2_UTILS_H
#define WCI2_UTILS_H
/*!
  @file wci2_utils.h

  @brief
   Provides a number of basic utilities to the MWCI-2 Layer

*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/inc/wci2_utils.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
           ckk     Initial Checkin

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <msgcfg.h>
#include <msg.h>
#include <err.h>
#include <amssassert.h>
#include <comdef.h>
#include <rex.h>
#include "modem_mem.h"
#include <IxErrno.h>

/*=============================================================================

                   INTERNAL DEFINITIONS AND TYPES

=============================================================================*/
#define WCI2_ERR_FATAL(format, exp1, exp2, exp3) \
          ERR_FATAL(format, exp1, exp2, exp3)

/*=============================================================================

  MACRO: WCI2_ASSERT

=============================================================================*/
/*!
    @brief
     Given a boolean expression, verify that the input expression is TRUE.
     If the input expression is FALSE, flags an error and resets SW stack.

    @detail
     Use this in all places where you are NOT handling or otherwise checking 
     for this error gracefully. This can be used for  out of range
     function parameters, NULL pointers and unintentional fall to the default
     case of a switch statement. The assumption is that no additional error
     recovery mechanism is present in the code at the time these macros are
     placed. If and when error recovery code is developed, default class 
     macros are expected to be changed to WCI2_ERROR macro.

    @note
     This is not good for error validation at a module's top level fns which 
     are probably seeing the input arguents from a different module. There 
     proper error checks are recommended and a grecful recovery is desirable.
     There use WCI2_ERROR macros to catch error right where it is seen.

*/
/*===========================================================================*/
#define WCI2_ASSERT(exp)  \
  if( !(exp) ) \
  { \
     WCI2_ERR_FATAL( "MCS_WCI2 assertion " #exp " failed", 0, 0, 0 ); \
  }

/*=============================================================================
                         Message Macros
=============================================================================*/

#define WCI2_MSG_0(LVL, TEXT) \
  MSG( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT );

#define WCI2_MSG_1(LVL, TEXT, A) \
  MSG_1( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT, A );

#define WCI2_MSG_2(LVL, TEXT, A, B) \
  MSG_2( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT, A, B );

#define WCI2_MSG_3(LVL, TEXT, A, B, C) \
  MSG_3( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT, A, B, C );

#define WCI2_MSG_4(LVL, TEXT, A, B, C, D) \
  MSG_4( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT, A, B, C, D );

#define WCI2_MSG_5(LVL, TEXT, A, B, C, D, E) \
  MSG_5( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT, A, B, C, D, E );

#define WCI2_MSG_6(LVL, TEXT, A, B, C, D, E, F ) \
  MSG_6( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT, A, B, C, D, E, F );

#define WCI2_MSG_7(LVL, TEXT, A, B, C, D, E, F, G ) \
  MSG_7( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT, A, B, C, D, E, F, G );

#define WCI2_MSG_8(LVL, TEXT, A, B, C, D, E, F, G, H ) \
  MSG_8( MSG_SSID_WCI2, MSG_LEGACY_ ## LVL, "WCI2: " TEXT, A, B, C, D, E, F, G, H );


#define WCI2_MSG_SPRINTF_1(msg, a1 ) \
  MSG_SPRINTF_1( MSG_SSID_WCI2, MSG_LEGACY_HIGH, "WCI2: " msg, (a1))

#define WCI2_MSG_SPRINTF_2(msg, a1, a2) \
  MSG_SPRINTF_2( MSG_SSID_WCI2, MSG_LEGACY_HIGH, "WCI2: " msg, (a1), (a2))

#define WCI2_MSG_SPRINTF_3(msg, a1, a2, a3 ) \
  MSG_SPRINTF_3( MSG_SSID_WCI2, MSG_LEGACY_HIGH, "WCI2: " msg, (a1), (a2), (a3))

#define WCI2_MSG_SPRINTF_4(msg, a1, a2, a3, a4 ) \
  MSG_SPRINTF_4( MSG_SSID_WCI2, MSG_LEGACY_HIGH, "WCI2: " msg, (a1), (a2), (a3), (a4))

/*=============================================================================
                         MEM alloc/free Macros
=============================================================================*/
#define WCI2_MEM_ALLOC( size ) modem_mem_alloc( size, MODEM_MEM_CLIENT_MCS )
#define WCI2_MEM_CALLOC( cnt, size ) modem_mem_calloc( cnt, size, MODEM_MEM_CLIENT_MCS )
#define WCI2_MEM_FREE( ptr ) modem_mem_free( ptr, MODEM_MEM_CLIENT_MCS )

#endif /* WCI2_UTILS_H */
