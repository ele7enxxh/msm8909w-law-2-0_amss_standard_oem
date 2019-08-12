/***********************************************************************
 * fds_util.c
 *
 * Utility functions for Flash Driver Service
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/src/fds_util.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-12-12   rp    Allow disabling SCRUB feature at runtime.
2013-10-03   dks   Add commands for Fota Update and Event registration.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-01   rp    Scrub-Master module cleanup.
2013-07-17   dks   Create

===========================================================================*/

#include "fds_util.h"
#include "fds_i.h"
#include "fds_err.h"
#include "flash_scrub_err.h"
#include "flash_scrub.h"

int
fds_map_scrub_error_to_fds_error (int scrub_error)
{
  switch (scrub_error)
  {
    case FLASH_SCRUB_EAGAIN:
      return FDS_ERR_TOO_SOON;

    case FLASH_SCRUB_EPERM:
      return FDS_ERR_NO_PERMISSION;

    case FLASH_SCRUB_ETIME:
      return FDS_ERR_TIME_NOT_ACQUIRED;

    case FLASH_SCRUB_EBUSY:
      return FDS_ERR_OP_IN_PROGRESS;

    case FLASH_SCRUB_EINVAL:
      return FDS_ERR_INVALID_HANDLE;

    case FLASH_SCRUB_DISABLED:
      return FDS_ERR_SCRUB_DISABLED;

    case FLASH_SCRUB_EPARAM:
      FDS_ERR_FATAL ("EPARAM returned from scrub master", 0, 0, 0);

    default:
      return FDS_ERR_INTERNAL;
  }
}

fds_scrub_status
fds_map_scrub_status_to_fds_scrub_status (int scrub_status)
{
  switch (scrub_status)
  {
    case FLASH_SCRUB_STATUS_PENDING:
      return FDS_SCRUB_PENDING;

    case FLASH_SCRUB_STATUS_COMPLETE:
      return FDS_SCRUB_COMPLETE;

    case FLASH_SCRUB_STATUS_UNKNOWN:
      return FDS_ERR_INVALID_HANDLE;

    default:
      FDS_ERR_FATAL ("%d, Unknown status returned from scrub master [status]",
                     scrub_status, 0, 0);
  }
}

