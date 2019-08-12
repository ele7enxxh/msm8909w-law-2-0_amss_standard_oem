/***********************************************************************
 * flash_scrub_global_lock.h
 *
 * FLASH_SCRUB_GLOBAL_LOCK critical section
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * The FLASH_SCRUB_GLOBAL_LOCK is used to protect the scrub master system
 * data from concurrent access.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_global_lock.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-12   rp    Create

===========================================================================*/

#ifndef __FLASH_SCRUB_GLOBAL_LOCK_H__
#define __FLASH_SCRUB_GLOBAL_LOCK_H__


#include "flash_scrub_config_i.h"


void flash_scrub_global_lock_init (void);
void flash_scrub_global_lock (void);
void flash_scrub_global_unlock (void);

#define FLASH_SCRUB_GLOBAL_LOCK_INIT()    flash_scrub_global_lock_init()
#define FLASH_SCRUB_GLOBAL_LOCK()         flash_scrub_global_lock()
#define FLASH_SCRUB_GLOBAL_UNLOCK()       flash_scrub_global_unlock()



#endif /* not __FLASH_SCRUB_GLOBAL_LOCK_H__ */
