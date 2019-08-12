/****************************************************************************
 * flash_scrub_master.h
 *
 * Scrub Master keeps track of progress of the scrub operation.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * The Scrub Master module is a state machine that keeps tracks of the
 * progress of the scrub operation. This file contains the function
 * declarations to be called by the scrub task to handle the scrub master
 * events.
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_master.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-01   rp    Scrub-Master module cleanup.
2013-07-22   wek   Add a separate sequence number for QMI scrubbing.
2013-07-12   wek   Add APIs for QMI Scrubbing trigger.
2013-06-05   wek   Create.

===========================================================================*/


#ifndef __FLASH_SCRUB_MASTER_H__
#define __FLASH_SCRUB_MASTER_H__

#include "flash_scrub_config_i.h"
#include "comdef.h"


/**----------------------------------------------------------------------------
 * Test simulator callback function to register for knowing scrub-master
 * state changes.
----------------------------------------------------------------------------**/
extern void (*scrub_master_sim_tests_callback_func) (uint32 seq, uint32 state);


#endif /* not __FLASH_SCRUB_MASTER_H__ */
