/***********************************************************************
 * fds.h
 *
 * Public APIs for FDS
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Public APIs supported by Flash Driver Service module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/inc/fds.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-08   dks   Create

===========================================================================*/

#ifndef __FDS_H__
#define __FDS_H__

#include "comdef.h"

/**************************************************************************//**
 * @par Name:
 *  fds_scrub_send_apps_scrub_notification - API that scrub task can call to
 *   send indication to Applications Processor to start doing Apps filesystem
 *   scrubbiing
 *
 * @par Synopsis:
 *   @code
 *     #include "fds_scrub.h"
 *     result = fds_scrub_send_apps_scrub_notification (scrub_token);
 *   @endcode
 *
 * @par scrub_token:
 *   The current Scrub sequence number.
 *
 * @par Description:
 *   Sends the apps_start_scrub indication message to all registered clients
 *
 * @return:
 *   0 if ind send was successful and -1 otherwise.
 *****************************************************************************/
int fds_scrub_send_apps_scrub_notification (uint32 scrub_token);


#endif /* not __FDS_H__ */
