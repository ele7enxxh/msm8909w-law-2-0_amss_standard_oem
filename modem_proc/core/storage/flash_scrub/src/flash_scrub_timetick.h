/***********************************************************************
 * flash_scrub_timetick.h
 *
 * Flash-Scrub TimeTick module.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Flash-Scrub TimeTick module used for time-profiling.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_timetick.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-08-21   rp    Create

===========================================================================*/

#ifndef __FLASH_SCRUB_TIMETICK_H__
#define __FLASH_SCRUB_TIMETICK_H__


#include "comdef.h"

/* Initialize TimeTick module. */
void flash_scrub_timetick_init (void);

/* Returns current timetick in microseconds, or 0 in case of any error. */
uint64 flash_scrub_timetick_get_timetick (void);

/* Convert timetick to microseconds, or 0 in case of any error. */
uint64 flash_scrub_timetick_convert_to_us (uint64 timetick);

#endif /* not __FLASH_SCRUB_TIMETICK_H__ */
