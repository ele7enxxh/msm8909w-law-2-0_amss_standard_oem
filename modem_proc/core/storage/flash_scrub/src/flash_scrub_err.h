/****************************************************************************
 * flash_scrub_err.h
 *
 * Error macros for Flash scrub module.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 ***************************************************************************/

 /*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_err.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ------------------------------------------------------------
2014-04-28   vm    Define more MSG macros to save RAM.
2013-12-12   rp    Allow disabling SCRUB feature at runtime.
2013-10-03   vm    Remove err_fatal upon partition table update failure.
2013-09-27   wek   Update pre-amble when boot code is scrub.
2013-09-08   rp    Fix bad blocks handling in code-partitions.
2013-08-22   vm    Add retries when there is md5 failure.
2013-08-20   wek   Swap MSG_HIGH/MED/LOW macros for MSG_3 macro.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Seperate out debug f3-messages and debug-err-fatal features.
2013-08-04   rp    Add debug err-fatal and debug f3-messages.
2013-08-04   rp    Scrub Code partition module cleanup.
2013-07-17   vm    Add code partition scrubbing.
2013-07-16   wek   Add Boot code scrubbing.
2013-07-12   wek   Add APIs for QMI Scrubbing trigger.
2013-07-09   wek   Add error values for device read/write/erase errors.
2013-06-05   wek   Create.

===========================================================================*/

#ifndef __FLASH_SCRUB_ERR_H__
#define __FLASH_SCRUB_ERR_H__

#include "flash_scrub_config_i.h"
#include "comdef.h"
#include "msg.h"

/* Default debug message for Flash Scrubbing. */
#define FLASH_SCRUB_MSG_0(lvl,a)        MSG(MSG_SSID_FLASH_SCRUB,lvl,a)
#define FLASH_SCRUB_MSG_1(lvl,a,b)      MSG_1(MSG_SSID_FLASH_SCRUB,lvl,a,b)
#define FLASH_SCRUB_MSG_2(lvl,a,b,c)    MSG_2(MSG_SSID_FLASH_SCRUB,lvl,a,b,c)
#define FLASH_SCRUB_MSG_3(lvl,a,b,c,d)  MSG_3(MSG_SSID_FLASH_SCRUB,lvl,a,b,c,d)

/* Macros to send debug messages. */

#define FLASH_SCRUB_MSG_ERR_0(a)     FLASH_SCRUB_MSG_0(MSG_LEGACY_ERROR,a)
#define FLASH_SCRUB_MSG_ERR_1(a,b)   FLASH_SCRUB_MSG_1(MSG_LEGACY_ERROR,a,b)
#define FLASH_SCRUB_MSG_ERR_2(a,b,c) FLASH_SCRUB_MSG_2(MSG_LEGACY_ERROR,a,b,c)
#define FLASH_SCRUB_MSG_ERR_3(a,b,c,d) \
                                   FLASH_SCRUB_MSG_3(MSG_LEGACY_ERROR,a,b,c,d)

#define FLASH_SCRUB_MSG_HIGH_0(a)     FLASH_SCRUB_MSG_0(MSG_LEGACY_HIGH,a)
#define FLASH_SCRUB_MSG_HIGH_1(a,b)   FLASH_SCRUB_MSG_1(MSG_LEGACY_HIGH,a,b)
#define FLASH_SCRUB_MSG_HIGH_2(a,b,c) FLASH_SCRUB_MSG_2(MSG_LEGACY_HIGH,a,b,c)
#define FLASH_SCRUB_MSG_HIGH_3(a,b,c,d) \
                                   FLASH_SCRUB_MSG_3(MSG_LEGACY_HIGH,a,b,c,d)

#define FLASH_SCRUB_MSG_MED_0(a)       FLASH_SCRUB_MSG_0(MSG_LEGACY_MED,a)
#define FLASH_SCRUB_MSG_MED_1(a,b)     FLASH_SCRUB_MSG_1(MSG_LEGACY_MED,a,b)
#define FLASH_SCRUB_MSG_MED_2(a,b,c)   FLASH_SCRUB_MSG_2(MSG_LEGACY_MED,a,b,c)
#define FLASH_SCRUB_MSG_MED_3(a,b,c,d) \
                                   FLASH_SCRUB_MSG_3(MSG_LEGACY_MED,a,b,c,d)

#define FLASH_SCRUB_MSG_LOW_0(a)        FLASH_SCRUB_MSG_0(MSG_LEGACY_LOW,a)
#define FLASH_SCRUB_MSG_LOW_1(a,b)      FLASH_SCRUB_MSG_1(MSG_LEGACY_LOW,a,b)
#define FLASH_SCRUB_MSG_LOW_2(a,b,c)    FLASH_SCRUB_MSG_2(MSG_LEGACY_LOW,a,b,c)
#define FLASH_SCRUB_MSG_LOW_3(a,b,c,d) \
                                   FLASH_SCRUB_MSG_3(MSG_LEGACY_LOW,a,b,c,d)

#ifdef FLASH_SCRUB_ENABLE_DEBUG_F3_MESSAGES
  #define FLASH_SCRUB_MSG_DEBUG_HIGH_0(a) \
                    FLASH_SCRUB_MSG_0(MSG_LEGACY_HIGH,a)
  #define FLASH_SCRUB_MSG_DEBUG_HIGH_1(a,b) \
                    FLASH_SCRUB_MSG_1(MSG_LEGACY_HIGH,a,b)
  #define FLASH_SCRUB_MSG_DEBUG_HIGH_2(a,b,c) \
                    FLASH_SCRUB_MSG_2(MSG_LEGACY_HIGH,a,b,c)
  #define FLASH_SCRUB_MSG_DEBUG_HIGH_3(a,b,c,d) \
                    FLASH_SCRUB_MSG_3(MSG_LEGACY_HIGH,a,b,c,d)
#else
  #define FLASH_SCRUB_MSG_DEBUG_HIGH_0(a)
  #define FLASH_SCRUB_MSG_DEBUG_HIGH_1(a,b)
  #define FLASH_SCRUB_MSG_DEBUG_HIGH_2(a,b,c)
  #define FLASH_SCRUB_MSG_DEBUG_HIGH_3(a,b,c,d)
#endif

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
#define FLASH_SCRUB_ERR_FATAL(fmt,a,b,c)          \
  do {                                            \
    MSG_BASE ("FATAL", fmt, a, b, c, FS_MSG_RED); \
    err_fatal_raise ();                           \
  } while (0)

#else /* !FS_STANDALONE */
#include "err.h"

#define FLASH_SCRUB_ERR_FATAL(fmt,a,b,c)        \
  do {                                          \
    ERR_FATAL (fmt, a, b, c);                   \
  } while (0)

#endif


#ifdef FLASH_SCRUB_ENABLE_DEBUG_ERR_FATALS
  #define FLASH_SCRUB_DEBUG_ERR_FATAL(a,b,c,d)  FLASH_SCRUB_ERR_FATAL(a,b,c,d)
#else
  #define FLASH_SCRUB_DEBUG_ERR_FATAL(a,b,c,d)
#endif


/* List of error codes returned by SCRUB module. */
#define FLASH_SCRUB_EAGAIN        -1 /* Scrub too often, try again later. */
#define FLASH_SCRUB_EPERM         -2 /* Operation not permitted. */
#define FLASH_SCRUB_EEFS          -3 /* Error while writing state file to EFS*/
#define FLASH_SCRUB_EPARAM        -4 /* Invalid parameter. */
#define FLASH_SCRUB_ETIME         -5 /* System time not acquired. */
#define FLASH_SCRUB_EDEVREAD      -6 /* Error while reading from the device. */
#define FLASH_SCRUB_EDEVWRITE     -7 /* Error while writing to the device. */
#define FLASH_SCRUB_EDEVERASE     -8 /* Error while erasing a block in device*/
#define FLASH_SCRUB_EBUSY         -9 /* Scrub in progress. */
#define FLASH_SCRUB_EINVAL       -10 /* Invalid argument. */
#define FLASH_SCRUB_ENOSPC       -11 /* No space left in the device. */
#define FLASH_SCRUB_ENOENT       -12 /* No such partition in the table. */
#define FLASH_SCRUB_EINVALPARTI  -13 /* Invalid NAND-partition table. */
#define FLASH_SCRUB_EMD5         -14 /* MD5 mismatch. */
#define FLASH_SCRUB_EBADBLOCK    -15 /* Error while reading from the device. */
#define FLASH_SCRUB_EUPDATEPARTI -16 /* Error while updating parti table. */
#define FLASH_SCRUB_ECONFIG      -17 /* Build misconfiguration. */
#define FLASH_SCRUB_DISABLED     -18 /* SCRUB feature disabled. */

#endif /* End of __FLASH_SCRUB_ERR_H__ */

