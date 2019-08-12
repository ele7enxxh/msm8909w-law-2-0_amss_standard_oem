/***********************************************************************
 * flash_scrub_code_partitions.h
 *
 * This file contains the protypes of the APIs that will be called
 * in scrub master module.
 *
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_code_partitions.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-08   vm    Add support for retrieving scrub code result in simulator.
2013-08-29   rp    Recompute MD5 due to FOTA.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-04   rp    Scrub Code partition module cleanup.
2013-07-28   vm    Add init scrub info for first time bootup.
2013-05-24   vm    Create.

===========================================================================*/

#ifndef __FLASH_SCRUB_CODE_PARTITIONS_H__
#define __FLASH_SCRUB_CODE_PARTITIONS_H__

#include "flash_scrub_config_i.h"
#include "comdef.h"


/* This is the initialization function. It reads the partition table from MIBIB
 * partition into a global variable.
 */
int flash_scrub_code_initialize (void);

/* This function scrubs all the code partitions, updates the partition table
 * and updates the EFS file reflecting the completion of scrubbing action.
 */
int flash_scrub_all_code_partitions (uint32 sequence_num);

/* Compute MD5 for all code-partitions and update it in the EFS-files in
 * case of a mismatch.
 */
int flash_scrub_code_refresh_md5 (void);

#ifdef FLASH_SCRUB_UNIT_TEST
  /* Helps accessing the result, while testing in simulator */
  extern int scrub_code_parti_result;
#endif

#endif /* not __FLASH_SCRUB_CODE_PARTITIONS_H__ */
