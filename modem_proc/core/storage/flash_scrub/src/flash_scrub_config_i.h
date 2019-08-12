/****************************************************************************
 * flash_scrub_config_i.h
 *
 * Defines the default configuration of Scrub module
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * The scrub module has several items it can configure, e.g.  Scrub interval
 * or scrub enabled/disabled. This file defines the default values.
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_config_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-12-12   rp    Allow disabling SCRUB feature at runtime.
2013-10-04   dks   Handle Fota update message from apps.
2013-10-03   vm    Remove err_fatal when partition table update fails.
2013-09-05   dks   Fix accuracy of scrub timer when no system time available.
2013-09-04   rp    Cleanup the Scrub Log Module.
2013-08-29   rp    Recompute MD5 due to FOTA.
2013-08-23   wek   Move the stored MD5 hashes to a safe directory.
2013-08-22   vm    Add retries when there is md5 failure.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Seperate out debug f3-messages and debug-err-fatal features.
2013-08-08   rp    Memory-Align flash-buffers.
2013-08-04   rp    Add debug err-fatal and debug f3-messages.
2013-08-04   rp    Add MD5-support to verify code-partition scrubbing.
2013-08-04   rp    Scrub Code partition module cleanup.
2013-08-04   rp    Add Scrub-Log module.
2013-08-01   rp    Scrub-Master module cleanup.
2013-07-28   vm    Add init scrub info for first time bootup.
2013-07-17   vm    Add code partition scrubbing.
2013-07-16   wek   Add Boot code scrubbing.
2013-07-09   wek   Add maximum page size supported by scrub module.
2013-06-05   wek   Create.

===========================================================================*/

#ifndef __FLASH_SCRUB_CONFIG_I_H__
#define __FLASH_SCRUB_CONFIG_I_H__

#include "flash_scrub_config.h"

/**----------------------------------------------------------------------------
 * Minimum number of seconds to enforce between 2 scrubs. Default is 2-months.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MINIMUM_TIME_IN_SECS_BETWEEN_2_SCRUBS
  #define FLASH_SCRUB_MINIMUM_TIME_IN_SECS_BETWEEN_2_SCRUBS (2 * 31 * 24 *3600)
#endif

/**----------------------------------------------------------------------------
 * Timer-value for the automatic periodic scrubs. A periodic timer will be
 * started with this timer-value and upon its expiry a scrub will be done.
 * A value of 0 will disable the automatic-scrub.
 * Default timer-value is 4-months.
 * A value of 0 will disable the automatic periodic scrubs.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_TIMER_VALUE_IN_SECS_FOR_AUTOMATIC_SCRUBS
  #define FLASH_SCRUB_TIMER_VALUE_IN_SECS_FOR_AUTOMATIC_SCRUBS \
                                                 (4 * 31 * 24 * 3600)
#endif

#if FLASH_SCRUB_MINIMUM_TIME_IN_SECS_BETWEEN_2_SCRUBS > \
    FLASH_SCRUB_TIMER_VALUE_IN_SECS_FOR_AUTOMATIC_SCRUBS
  #error "Min scrub interval larger than time triggered scrub"
#endif

/**----------------------------------------------------------------------------
 * Timer value in secs for triggering an update of the time since last scrub
 * value.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_TIMER_VALUE_IN_SECS_FOR_SAVING_TIME_IN_EFS
  #define FLASH_SCRUB_TIMER_VALUE_IN_SECS_FOR_SAVING_TIME_IN_EFS \
                                                 (7 * 24 * 60 * 60) /* 1week */
#endif

/**----------------------------------------------------------------------------
 * Enable QMI-commands to request scrub and scrub-status.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_DISABLE_QMI_COMMANDS_SUPPORT
  #define FLASH_SCRUB_QMI_COMMANDS_SUPPORT
#endif

/**----------------------------------------------------------------------------
 * File in EFS that holds the configuration file for scrub-master.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MASTER_CONFIG_FILEPATH
  #define FLASH_SCRUB_MASTER_CONFIG_FILEPATH "/safe/scrub/master_cfg.bin"
#endif

/**----------------------------------------------------------------------------
 * File in EFS that holds the configuration file for scrub-master.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MASTER_INFO_FILEPATH
  #define FLASH_SCRUB_MASTER_INFO_FILEPATH "/safe/scrub/master_info.bin"
#endif

/**----------------------------------------------------------------------------
 * File in EFS that holds debug info for scrub-master.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MASTER_DEBUG_FILEPATH
  #define FLASH_SCRUB_MASTER_DEBUG_FILEPATH "/safe/scrub/master_debug.bin"
#endif

/**----------------------------------------------------------------------------
* Location where to store the Boot scrub state file. This should not be
* accessible through DIAG.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_BOOT_STATE_FILE
  #define FLASH_SCRUB_BOOT_STATE_FILE "/safe/scrub/boot_state"
#endif

/**----------------------------------------------------------------------------
 * In Case of error, use this number as a exponentially growing back off
 * timer. For example, retry at 1 second, then 2, 4, 8...
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MASTER_SYSTEM_TIMER_RETRY_IN_MSEC
  #define FLASH_SCRUB_MASTER_SYSTEM_TIMER_RETRY_IN_MSEC   (1000)
#endif

/**----------------------------------------------------------------------------
 * Maximum time in milliseconds between retries, by default 1 day.
 * system will re-try when time hasn't been acquired or there is a problem
 * with the scrubbing attempt.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MASTER_SYSTEM_TIMER_RETRY_IN_MSEC_MAX
  #define FLASH_SCRUB_MASTER_SYSTEM_TIMER_RETRY_IN_MSEC_MAX (0x5265C00ULL)
#endif

/**----------------------------------------------------------------------------
 * Maximum NAND flash page size in bytes supported. This macro is used to
 * allocate the buffers for copying data.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MAX_PAGE_SIZE
  #define FLASH_SCRUB_MAX_PAGE_SIZE 4096
#endif

/**----------------------------------------------------------------------------
 * Location where to store the Scrub code config file. This should not be
 * accessible through DIAG.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_CODE_CONFIG_FILEPATH
  #define FLASH_SCRUB_CODE_CONFIG_FILEPATH "/safe/scrub/scrub_code_config.bin"
#endif

/**----------------------------------------------------------------------------
 * Location where to store the Scrub info file. This should not be
 * accessible through DIAG.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_CODE_INFO_FILEPATH
  #define FLASH_SCRUB_CODE_INFO_FILEPATH "/safe/scrub/scrub_code_info.bin"
#endif

/**----------------------------------------------------------------------------
 * Definitions used for scrubbing code partitions
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_PARTITION_NAME
  #define FLASH_SCRUB_PARTITION_NAME "0:SCRUB"
#endif

#ifndef FLASH_SCRUB_MIBIB_PARTITION_NAME
  #define FLASH_SCRUB_MIBIB_PARTITION_NAME "0:MIBIB"
#endif

#ifndef FLASH_SCRUB_MIBIB_UPDATE_MAX_RETRY_COUNT
  #define FLASH_SCRUB_MIBIB_UPDATE_MAX_RETRY_COUNT        3
#endif

/**----------------------------------------------------------------------------
 * Scrub module log file paths in EFS.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_LOG_ERR_LOGS_DIRECTORY_NAME
  #define FLASH_SCRUB_LOG_ERR_LOGS_DIRECTORY_NAME  "/scrub/logs/err"
#endif

#ifndef FLASH_SCRUB_LOG_MSG_LOGS_DIRECTORY_NAME
  #define FLASH_SCRUB_LOG_MSG_LOGS_DIRECTORY_NAME  "/scrub/logs/msg"
#endif

#ifndef FLASH_SCRUB_LOG_ERR_LOGS_LATEST_LOG_FILE
  #define FLASH_SCRUB_LOG_ERR_LOGS_LATEST_LOG_FILE "/scrub/logs/err/err_latest"
#endif

#ifndef FLASH_SCRUB_LOG_MSG_LOGS_LATEST_LOG_FILE
  #define FLASH_SCRUB_LOG_MSG_LOGS_LATEST_LOG_FILE "/scrub/logs/msg/msg_latest"
#endif

#ifndef FLASH_SCRUB_LOG_MAX_ERR_LOG_FILES_COUNT
  #define FLASH_SCRUB_LOG_MAX_ERR_LOG_FILES_COUNT    (20)
#endif

#ifndef FLASH_SCRUB_LOG_MAX_MSG_LOG_FILES_COUNT
  #define FLASH_SCRUB_LOG_MAX_MSG_LOG_FILES_COUNT    (30)
#endif

#ifndef FLASH_SCRUB_LOG_MAX_FILE_SIZE_IN_BYTES
  #define FLASH_SCRUB_LOG_MAX_FILE_SIZE_IN_BYTES     (1 * 1024)
#endif

#ifndef FLASH_SCRUB_LOG_MAX_LINE_BUFF_SIZE_IN_BYTES
  #define FLASH_SCRUB_LOG_MAX_LINE_BUFF_SIZE_IN_BYTES    (80)
#endif

#ifndef FLASH_SCRUB_LOG_MAX_FILE_PATH_SIZE_IN_BYTES
  #define FLASH_SCRUB_LOG_MAX_FILE_PATH_SIZE_IN_BYTES    (50)
#endif


/**----------------------------------------------------------------------------
 * MD5-Hash settings for SBL and Code-Partitions.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MD5_DIRECTORY
  #define FLASH_SCRUB_MD5_DIRECTORY  "/safe/scrub/md5"
#endif

#ifndef FLASH_SCRUB_MD5_MAX_RETRY_COUNT
  #define FLASH_SCRUB_MD5_MAX_RETRY_COUNT        3
#endif

/**----------------------------------------------------------------------------
 * Byte-Alignment requirements specified by the DMA. The buffers that we pass
 * to Flash-drivers if byte-aligned to these many bytes will optimize the data
 * transfer speed and avoid mallocs inside the flash-driver.
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_DM_ALIGN_BYTES
  #define FLASH_SCRUB_DM_ALIGN_BYTES        128
#endif

#if FLASH_SCRUB_DM_ALIGN_BYTES <= 0
  #error "Invalid memory alignment configuration"
#endif

/* Alignment keyword syntax varies with each compiler. */
#if   defined (__GNUC__)
  #define FLASH_SCRUB_ALIGN \
                __attribute__((__aligned__(FLASH_SCRUB_DM_ALIGN_BYTES)))
#elif defined (__arm)
  #define FLASH_SCRUB_ALIGN __align(FLASH_SCRUB_DM_ALIGN_BYTES)
#elif defined (_MSC_VER)
  #define FLASH_SCRUB_ALIGN __declspec(align(FLASH_SCRUB_DM_ALIGN_BYTES))
#elif defined (_lint)
  #define FLASH_SCRUB_ALIGN /* Nothing. */
#else
  #error "Unrecognized C compiler.  Need alignment syntax for this target."
#endif

/* Is this pointer 32-byte aligned?
 * Note that this assumes pointers are uint32-equivalent */
#define FLASH_SCRUB_OKAY_FOR_DM(x) \
              (((uint32)(x) & (FLASH_SCRUB_DM_ALIGN_BYTES-1)) == 0)

/**----------------------------------------------------------------------------
 * Enable more f3-messages for debug.
----------------------------------------------------------------------------**/
#undef FLASH_SCRUB_ENABLE_DEBUG_F3_MESSAGES

/**----------------------------------------------------------------------------
 * Enable stricter err-fatals for debug.
----------------------------------------------------------------------------**/
#undef FLASH_SCRUB_ENABLE_DEBUG_ERR_FATALS


/**----------------------------------------------------------------------------
 * The minimum size for the SCRUB-partition in the nand-card.
 * If the SCRUB-partition block-count in the nand is less than this number,
 * then it is a hint to disable the SCRUB feature altogether at runtime.
 *
 * Note : Configuring the SCRUB-partition size in the nand-card less than this
 *        number will disable the SCRUB feature altogether at runtime.
 *
----------------------------------------------------------------------------**/
#ifndef FLASH_SCRUB_MIN_SCRUB_PARTITION_BLOCK_COUNT
  #define FLASH_SCRUB_MIN_SCRUB_PARTITION_BLOCK_COUNT 5
#endif


#endif /* not __FLASH_SCRUB_CONFIG_I_H__ */
