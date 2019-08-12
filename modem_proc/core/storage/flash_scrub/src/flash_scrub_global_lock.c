/***********************************************************************
 * flash_scrub_global_lock.c
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_global_lock.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-12   rp    Create

===========================================================================*/

#include "flash_scrub_config_i.h"
#include "flash_scrub_global_lock.h"
#include "rex.h"

#include <string.h>

static rex_crit_sect_type flash_scrub_crit_sect;

void
flash_scrub_global_lock_init (void)
{
  memset (&flash_scrub_crit_sect, 0, sizeof (flash_scrub_crit_sect));
  rex_init_crit_sect (&flash_scrub_crit_sect);
}

void
flash_scrub_global_lock (void)
{
  rex_enter_crit_sect (&flash_scrub_crit_sect);
}

void
flash_scrub_global_unlock (void)
{
  rex_leave_crit_sect (&flash_scrub_crit_sect);
}

