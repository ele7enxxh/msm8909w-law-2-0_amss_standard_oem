/***********************************************************************
 * fs_rmts_cookie_golden_copy_handler.h
 *
 * Provides interface function to check if a golden copy can be created
 * in boot.
 *
 * Copyright (C) 2014 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_cookie_golden_copy_handler.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-05-12   vm    Create

===========================================================================*/

#ifndef __FS_RMTS_COOKIE_GOLDEN_COPY_HANDLER_H__
#define __FS_RMTS_COOKIE_GOLDEN_COPY_HANDLER_H__

#include "fs_hotplug.h"


/* Check if FSG has already been created. Returns 1 if created and 0 if not
   and -1 on error to read from device. */
int
fs_rmts_cookie_is_golden_copy_present (struct hotplug_device *hdev,
                                       uint8 *copy_buffer, uint32 buf_size);
#ifdef FS_UNIT_TEST
  /* Callback function used by tests */
  int (*sim_test_boot_create_golden_copy_callback) ();
#endif

#endif /* not __FS_RMTS_COOKIE_GOLDEN_COPY_HANDLER_H__ */
