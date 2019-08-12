#ifndef MSG_WINCE_H
#define MSG_WINCE_H
/*!
  @ingroup diag_message_service
  @file msg_wince.h 
  @brief
  Diagnostic Message macro re-definitions for some Windows phone-specific 
  clients that run in WinCE OAL and BOOTLOADER.
  
  @details
  This file contains necessary re-definitions to achieve commonality of 
  Diagnostic Debug message service API header in Windows phones. Diagnostic
  Message macros are re-defined to native windows definitions, for clients to
  continue having common files on Windows and non-Windows systems.
  @note
  Please do not include this file directly. Include msg.h to use Diagnostic 
  debug message service.
*/
/*
Copyright (c) 2008-2010 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*/
/*===========================================================================
                         Edit History
  $Header: //components/rel/core.mpss/3.5.c12.3/api/services/msg_wince.h#1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2010-05-26    mad     Doxygenated
2010-03-02    mad     Removed uldr feature
2010-01-20    sg      Ported Qshrink support to WM7
2009-10-29    mad     Removed inclusion of bsp.h. Added relevant headers for
                      definitions of OALMSG, ERRORMSG, WCE_MSG etc. 
                      Included windows.h.
2009-03-16    mad     Removed inclusion of customer.h
2008-10-22    mad     Created from diag_wince_msg.h. Provides redirection 
                      of MSG macros for WINCE OAL, BOOTLOADER and uldr.
===========================================================================*/
#include <windows.h>


#if defined(FEATURE_WINCE_BOOTLOADER) || defined (FEATURE_WINCE_OAL)
#include "oal_log.h" /* For OALMSG, OAL_INFO etc */
#include "dbgapi.h" /* For ERRORMSG and RETAILMSG */

/*!
@name Legacy MSG macro re-definitions for OAL and Bootloader
@ingroup diag_message_service
These macros provide native windows re-definitions for legacy debug message
macros (a.k.a MSG 1.0)
*/
/*@{*/ /* start group Legacy MSG macro re-definitions for OAL and Bootloader */
   #define MSG_HIGH(str, a, b, c)  OALMSG(OAL_INFO, (TEXT("HIGH: ") TEXT(str), a, b, c))
   #define MSG_MED(str, a, b, c)   OALMSG(OAL_INFO, (TEXT("MED:  ") TEXT(str), a, b, c))
   #define MSG_LOW(str, a, b, c)   OALMSG(0, (TEXT("LOW:  ") TEXT(str), a, b, c))
   #define MSG_ERROR(str, a, b, c) ERRORMSG(1, (L"ERR: " TEXT(str) L"\r\n", a, b, c))
   #define MSG_FATAL(str, a, b, c) ERRORMSG(1, (L"FATAL: " TEXT(str) L"\r\n", a, b, c))
/*@}*/ /* End group Legacy MSG macro re-definitions for OAL and Bootloader */

/*! 
@name Re-definitions for QSHRINK MSG 1.0 macros
@ingroup diag_message_service
These macros provide re-definitions for QSHRINK MSG 1.0 macros
@note
These macros (QSR_MSG*) should not be called directly. MSG* macros are 
converted to QSR_MSG* with a text replacement before build.
*/
/*@{*/ /* start group Re-definitions for QSHRINK MSG 1.0 macros */
   #define QSR_MSG_HIGH(hash, str, a, b, c)  OALMSG(OAL_INFO, (TEXT("HIGH: ") TEXT(str), a, b, c))
   #define QSR_MSG_MED(hash, str, a, b, c)   OALMSG(OAL_INFO, (TEXT("MED:  ") TEXT(str), a, b, c))
   #define QSR_MSG_LOW(hash, str, a, b, c)   OALMSG(0, (TEXT("LOW:  ") TEXT(str), a, b, c))
   #define QSR_MSG_ERROR(hash, str, a, b, c) ERRORMSG(1, (L"ERR: " TEXT(str) L"\r\n", a, b, c))
   #define QSR_MSG_FATAL(hash, str, a, b, c) ERRORMSG(1, (L"FATAL: " TEXT(str) L"\r\n", a, b, c))
   #define MSG_DIAG OALMSG
/*@}*/ /* end group Re-definitions for QSHRINK MSG 1.0 macros */  

#endif /* #if defined(FEATURE_WINCE_BOOTLOADER) || defined (FEATURE_WINCE_OAL) */


/*!
@name Re-definitions for MSG2.0 macros for some windows-specific clients
@ingroup diag_message_service
These macros provide re-definitions for MSG 2.0 macros, for a few windows-specific
clients that run in OAL and Bootloader. For these clients, MSG_DIAG is defined as OALMSG.
*/
/*!
Macro for messages with no parameters.
*/
#define MSG(ssid, ssmask, str) \
	MSG_DIAG(1, (TEXT("SSID:%d LVL:%d "), ssid, ssmask))
/*!
Macro for messages with 1 parameter.
*/
#define MSG_1(ssid, ssmask, str, arg1) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1))
/*!
Macro for messages with 2 parameters.
*/
#define MSG_2(ssid, ssmask, str, arg1, arg2) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
				arg2))
/*!
Macro for messages with 3 parameters.
*/
#define MSG_3(ssid, ssmask, str, arg1, arg2, arg3) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
				arg2, arg3))
/*!
Macro for messages with 4 parameters.
*/
#define MSG_4(ssid, ssmask, str, arg1, arg2, arg3, arg4) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4))
/*!
Macro for messages with 5 parameters.
*/
#define MSG_5(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4, arg5))
/*!
Macro for messages with 6 parameters.
*/			
#define MSG_6(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5, arg6) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4, arg5, arg6))
/*!
Macro for messages with 7 parameters.
*/
#define MSG_7(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4, arg5, arg6, arg7))
/*!
Macro for messages with 8 parameters.
*/
#define MSG_8(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4, arg5, arg6, arg7, arg8))
/*!
Macro for messages with 9 parameters.
*/
#define MSG_9(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, arg2,\
				arg3, arg4, arg5, arg6, arg7, arg8, arg9))

#endif /* MSG_WINCE_H */


