/***********************************************************************
 * flash_scrub_boot_code.h
 *
 * Function declarations to scrub boot code.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Boot loaders code is handled differently than other code partitions.
 * This is because it is by the Boot ROM, while other code partitions are
 * loaded by boot loaders. This file declares the functions to refresh
 * the boot image so PBL can still read it and start the system.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_boot_code.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-09-01   wek   Add MD5 verification for SBL partition.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Include "scrub_config_i.h" before any other file.
2013-07-16   wek   Create

===========================================================================*/

#ifndef __FLASH_SCRUB_BOOT_CODE_H__
#define __FLASH_SCRUB_BOOT_CODE_H__

#include "flash_scrub_config_i.h"
#include "comdef.h"

typedef struct flash_scrub_boot_handle FLASH_SCRUB_BOOT_HANDLE;

/* Initializes the scrub boot module. */
FLASH_SCRUB_BOOT_HANDLE* flash_scrub_boot_init (uint32 seq_num);

/* Start/Resume scrub operation. */
int flash_scrub_boot (FLASH_SCRUB_BOOT_HANDLE *handle, uint32 seq_num);

/* Updates the value of the MD5 hash for boot code. It also creates the MD5
 * hash if it does not exist. */
int flash_scrub_boot_refresh_md5 (FLASH_SCRUB_BOOT_HANDLE *handle);

#endif /* not __FLASH_SCRUB_BOOT_CODE_H__ */
