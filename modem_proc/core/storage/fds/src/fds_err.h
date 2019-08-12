/***********************************************************************
 * fds_err.h
 *
 * Error macros for FDS module.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/src/fds_err.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-28   vm    Define more MSG macros to save RAM.
2013-09-20   wek   Swap MSG_HIGH/MED/LOW macros for MSG_3 macro.
2013-07-17   dks   Create

===========================================================================*/

#ifndef __FDS_ERR_H__
#define __FDS_ERR_H__

#include "comdef.h"
#include "msg.h"

/* Default debug message for FDS. */
#define FDS_MSG_0(lvl,a)        MSG(MSG_SSID_FLASH_SCRUB,lvl,a)
#define FDS_MSG_1(lvl,a,b)      MSG_1(MSG_SSID_FLASH_SCRUB,lvl,a,b)
#define FDS_MSG_2(lvl,a,b,c)    MSG_2(MSG_SSID_FLASH_SCRUB,lvl,a,b,c)
#define FDS_MSG_3(lvl,a,b,c,d)  MSG_3(MSG_SSID_FLASH_SCRUB,lvl,a,b,c,d)

/* Macros to send debug messages. */
#define FDS_MSG_ERR_0(a)       FDS_MSG_0(MSG_LEGACY_ERROR,a)
#define FDS_MSG_ERR_1(a,b)     FDS_MSG_1(MSG_LEGACY_ERROR,a,b)
#define FDS_MSG_ERR_2(a,b,c)   FDS_MSG_2(MSG_LEGACY_ERROR,a,b,c)
#define FDS_MSG_ERR_3(a,b,c,d) FDS_MSG_3(MSG_LEGACY_ERROR,a,b,c,d)

#define FDS_MSG_HIGH_0(a)       FDS_MSG_0(MSG_LEGACY_HIGH,a)
#define FDS_MSG_HIGH_1(a,b)     FDS_MSG_1(MSG_LEGACY_HIGH,a,b)
#define FDS_MSG_HIGH_2(a,b,c)   FDS_MSG_2(MSG_LEGACY_HIGH,a,b,c)
#define FDS_MSG_HIGH_3(a,b,c,d) FDS_MSG_3(MSG_LEGACY_HIGH,a,b,c,d)

#define FDS_MSG_MED_0(a)        FDS_MSG_0(MSG_LEGACY_MED,a)
#define FDS_MSG_MED_1(a,b)      FDS_MSG_1(MSG_LEGACY_MED,a,b)
#define FDS_MSG_MED_2(a,b,c)    FDS_MSG_2(MSG_LEGACY_MED,a,b,c)
#define FDS_MSG_MED_3(a,b,c,d)  FDS_MSG_3(MSG_LEGACY_MED,a,b,c,d)

#define FDS_MSG_LOW_0(a)        FDS_MSG_0(MSG_LEGACY_LOW,a)
#define FDS_MSG_LOW_1(a,b)      FDS_MSG_1(MSG_LEGACY_LOW,a,b)
#define FDS_MSG_LOW_2(a,b,c)    FDS_MSG_2(MSG_LEGACY_LOW,a,b,c)
#define FDS_MSG_LOW_3(a,b,c,d)  FDS_MSG_3(MSG_LEGACY_LOW,a,b,c,d)


/* If compiling in simulator, use simulator err_fatals. */
#if defined (FS_STANDALONE)

#include <stdio.h>
#ifndef NORETURN
  #ifdef __GNUC__
    #define NORETURN   __attribute__ ((noreturn))
  #else
    #define NORETURN
  #endif
#endif

extern void err_fatal_raise (void) NORETURN;
#define FDS_ERR_FATAL(fmt,a,b,c) \
  do {                                            \
    MSG_BASE ("FATAL", fmt, a, b, c, FS_MSG_RED); \
    err_fatal_raise ();                           \
  } while (0)

#else /* !FS_STANDALONE */
#include "err.h"

#define FDS_ERR_FATAL(fmt,a,b,c)              \
  do {                                          \
    ERR_FATAL (fmt, a, b, c);                   \
  } while (0)

#endif
#endif /* not __FDS_ERR_H__ */
