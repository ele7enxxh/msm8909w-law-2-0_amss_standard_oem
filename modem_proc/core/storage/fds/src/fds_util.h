/***********************************************************************
 * fds_util.h
 *
 * Utility functions for Flash Driver Service
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/src/fds_util.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-03   dks   Add commands for Fota Update and Event registration.
2013-07-17   dks   Create

===========================================================================*/

#ifndef __FDS_UTIL_H__
#define __FDS_UTIL_H__

#include "fds_i.h"

int fds_map_scrub_error_to_fds_error (int scrub_error);

fds_scrub_status fds_map_scrub_status_to_fds_scrub_status (int scrub_status);

#endif /* not __FDS_UTIL_H__ */
