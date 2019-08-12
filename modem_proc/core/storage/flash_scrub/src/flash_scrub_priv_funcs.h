/***********************************************************************
 * flash_scrub_priv_funcs.h
 *
 * Flash-Scrub module private functions.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Flash-scrub module private functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_priv_funcs.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-04   dks   Handle Fota update message from apps.
2013-09-05   dks   Fix accuracy of scrub timer when no system time available.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-01   rp    Create

===========================================================================*/

#ifndef __FLASH_SCRUB_PRIV_FUNCS_H__
#define __FLASH_SCRUB_PRIV_FUNCS_H__

#include "flash_scrub_config_i.h"


/*---------------------------------------------------------------------------
 * Scrub-Task signals
---------------------------------------------------------------------------*/
#define FLASH_SCRUB_SYSTEM_TIMER_EXPIRED_SIGNAL            0x01
#define FLASH_SCRUB_SCRUB_TIMER_EXPIRED_SIGNAL             0x02
#define FLASH_SCRUB_START_SCRUB_SIGNAL                     0x04
#define FLASH_SCRUB_SAVE_TIME_IN_EFS_SIGNAL                0x08
#define FLASH_SCRUB_FOTA_NOTIFICATION_SIGNAL               0x10

/*---------------------------------------------------------------------------
 * 12,049-Days : Midnight between Jan-6-1980 to Jan-1-2013.
---------------------------------------------------------------------------*/
#define FLASH_SCRUB_MASTER_JAN_1_2013_TIME_VAL_IN_SECS (1041033600)



/**----------------------------------------------------------------------------
 * Initialize the scrub module.
----------------------------------------------------------------------------**/
void flash_scrub_init (void);


/**----------------------------------------------------------------------------
 * Function called by the scrub-task when the System-Timer signal expires.
----------------------------------------------------------------------------**/
void flash_scrub_system_timer_signal_received (void);

/**----------------------------------------------------------------------------
 * Function called by the scrub-task when the Scrub-Timer signal expires.
----------------------------------------------------------------------------**/
void flash_scrub_scrub_timer_signal_received (void);


/**----------------------------------------------------------------------------
 * Function called by the scrub-task when the Scrub-signal is received.
----------------------------------------------------------------------------**/
void flash_scrub_start_scrub_signal_received (void);


/**----------------------------------------------------------------------------
 * Function called by the scrub-task when the update_time_since_last_scrub
 * signal is received.
----------------------------------------------------------------------------**/
void flash_scrub_save_time_in_efs_signal_received (void);


/**----------------------------------------------------------------------------
 * Function called by the scrub-task when the FOTA update signal is received.
----------------------------------------------------------------------------**/
void flash_scrub_fota_notification_signal_received (void);


/**----------------------------------------------------------------------------
 * Do cleanup activity required during to gracefully shutdown.
----------------------------------------------------------------------------**/
void flash_scrub_rcinit_shutdown_callback (void);

#endif /* not __FLASH_SCRUB_PRIV_FUNCS_H__ */
