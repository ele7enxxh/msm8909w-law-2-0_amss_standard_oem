#ifndef MSG_WINCE_H
#define MSG_WINCE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              DIAGNOSTIC MESSAGE SERVICE HEADER FILE FOR WINCE

General Description
  Redirection of Diag Message macros, for WinCE OAL and BOOTLOADER.

Copyright (c) 2008-2010 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

 /*===========================================================================
                         Edit History
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/msg_wince.h#1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
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

   #define MSG_HIGH(str, a, b, c)  OALMSG(OAL_INFO, (TEXT("HIGH: ") TEXT(str), a, b, c))
   #define MSG_MED(str, a, b, c)   OALMSG(OAL_INFO, (TEXT("MED:  ") TEXT(str), a, b, c))
   #define MSG_LOW(str, a, b, c)   OALMSG(0, (TEXT("LOW:  ") TEXT(str), a, b, c))
   #define MSG_ERROR(str, a, b, c) ERRORMSG(1, (L"ERR: " TEXT(str) L"\r\n", a, b, c))
   #define MSG_FATAL(str, a, b, c) ERRORMSG(1, (L"FATAL: " TEXT(str) L"\r\n", a, b, c))

   /* Re-direction for QSR_MSG* macros */
   /* Note: These macros (QSR_MSG*) should not be called directly.
   MSG* macros are converted to QSR_MSG* with a text replacement before build.*/

   #define QSR_MSG_HIGH(hash, str, a, b, c)  OALMSG(OAL_INFO, (TEXT("HIGH: ") TEXT(str), a, b, c))
   #define QSR_MSG_MED(hash, str, a, b, c)   OALMSG(OAL_INFO, (TEXT("MED:  ") TEXT(str), a, b, c))
   #define QSR_MSG_LOW(hash, str, a, b, c)   OALMSG(0, (TEXT("LOW:  ") TEXT(str), a, b, c))
   #define QSR_MSG_ERROR(hash, str, a, b, c) ERRORMSG(1, (L"ERR: " TEXT(str) L"\r\n", a, b, c))
   #define QSR_MSG_FATAL(hash, str, a, b, c) ERRORMSG(1, (L"FATAL: " TEXT(str) L"\r\n", a, b, c))
   #define MSG_DIAG OALMSG
   

#endif


/*---------------------------------------------------------------------------
  Macro for messages with 0 to 9 parameters.
---------------------------------------------------------------------------*/
#define MSG(ssid, ssmask, str) \
	MSG_DIAG(1, (TEXT("SSID:%d LVL:%d "), ssid, ssmask))

#define MSG_1(ssid, ssmask, str, arg1) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1))

#define MSG_2(ssid, ssmask, str, arg1, arg2) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
				arg2))

#define MSG_3(ssid, ssmask, str, arg1, arg2, arg3) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
				arg2, arg3))

#define MSG_4(ssid, ssmask, str, arg1, arg2, arg3, arg4) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4))

#define MSG_5(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4, arg5))
				
#define MSG_6(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5, arg6) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4, arg5, arg6))


#define MSG_7(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4, arg5, arg6, arg7))

#define MSG_8(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, \
			arg2, arg3,	arg4, arg5, arg6, arg7, arg8))


#define MSG_9(ssid, ssmask, str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
			MSG_DIAG(1, (TEXT("SSID:%d LVL:%d ") TEXT(str), ssid, ssmask, arg1, arg2,\
				arg3, arg4, arg5, arg6, arg7, arg8, arg9))

#endif /* MSG_WINCE_H */


