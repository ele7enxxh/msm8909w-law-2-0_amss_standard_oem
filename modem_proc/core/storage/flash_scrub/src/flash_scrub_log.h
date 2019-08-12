/***********************************************************************
 * flash_scrub_log.h
 *
 * Flash Scrub Log module.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Flash Scrub Log module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_log.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-28   vm    Use correct MSG macros to save RAM.
2013-12-09   wek   Add macro to print F3 messages and log to efs file.
2013-09-04   rp    Cleanup the Scrub Log Module.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-04   rp    Create

===========================================================================*/

#ifndef __FLASH_SCRUB_LOG_H__
#define __FLASH_SCRUB_LOG_H__

#include "flash_scrub_config_i.h"
#include "comdef.h"


void flash_scrub_log_init (void);

void flash_scrub_log_err (const char *fmt, uint32 arg1, uint32 arg2,
                          uint32 arg3);

void flash_scrub_log_msg (const char *fmt, uint32 arg1, uint32 arg2,
                          uint32 arg3);

#define FLASH_SCRUB_LOG_ERR_0(fmt)                  \
  do{                                               \
    FLASH_SCRUB_MSG_ERR_0 (fmt);                    \
    flash_scrub_log_err (fmt, 0, 0, 0);             \
  } while (0)

#define FLASH_SCRUB_LOG_ERR_1(fmt, arg1)            \
  do{                                               \
    FLASH_SCRUB_MSG_ERR_1 (fmt, arg1);              \
    flash_scrub_log_err (fmt, arg1, 0, 0);          \
  } while (0)

#define FLASH_SCRUB_LOG_ERR_2(fmt, arg1, arg2)       \
  do{                                                \
    FLASH_SCRUB_MSG_ERR_2 (fmt, arg1, arg2);         \
    flash_scrub_log_err (fmt, arg1, arg2, 0);        \
  } while (0)

#define FLASH_SCRUB_LOG_ERR_3(fmt, arg1, arg2, arg3) \
  do{                                                \
    FLASH_SCRUB_MSG_ERR_3 (fmt, arg1, arg2, arg3);   \
    flash_scrub_log_err (fmt, arg1, arg2, arg3);     \
  } while (0)


#define FLASH_SCRUB_LOG_MSG_0(fmt)                  \
  do{                                               \
    FLASH_SCRUB_MSG_HIGH_0 (fmt);                   \
    flash_scrub_log_msg (fmt, 0, 0, 0);             \
  } while (0)

#define FLASH_SCRUB_LOG_MSG_1(fmt, arg1)            \
  do{                                               \
    FLASH_SCRUB_MSG_HIGH_1 (fmt, arg1);             \
    flash_scrub_log_msg (fmt, arg1, 0, 0);          \
  } while (0)

#define FLASH_SCRUB_LOG_MSG_2(fmt, arg1, arg2)      \
  do{                                               \
    FLASH_SCRUB_MSG_HIGH_2 (fmt, arg1, arg2);       \
    flash_scrub_log_msg (fmt, arg1, arg2, 0);       \
  } while (0)

#define FLASH_SCRUB_LOG_MSG_3(fmt, arg1, arg2, arg3) \
  do{                                               \
    FLASH_SCRUB_MSG_HIGH_3 (fmt, arg1, arg2, arg3);   \
    flash_scrub_log_msg (fmt, arg1, arg2, arg3);    \
  } while (0)

#endif /* not __FLASH_SCRUB_LOG_H__ */
