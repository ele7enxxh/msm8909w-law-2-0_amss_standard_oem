/***********************************************************************
 * fs_diag_golden_copy_handler.c
 *
 * Copyright (C) 2014 QUALCOMM Technologies, Inc.
 *
 * This file is defined to support for customer's requirement to allow
 * creation of golden copy multiple times. The function
 * fs_rmts_can_create_golden_copy () can be re-defined as below to achieve
 * this objective:
 *
 *
 * int fs_rmts_can_create_golden_copy (void)
 * {
 *   return 1;
 * }
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_diag_golden_copy_handler.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-05-27    dks  Include fs_config_i for proper feature compilation.
2014-05-04    vm   Create

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS2

#include "fs_privatedir.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_diag_golden_copy_handler.h"

#ifdef FS_UNIT_TEST
  int (*sim_test_can_create_golden_copy_callback) (void) = NULL;
#endif

int fs_rmts_can_create_golden_copy (void)
{
  int result;
  uint8 item_val;
  int can_create_gc;

#ifdef FS_UNIT_TEST
  if (sim_test_can_create_golden_copy_callback != NULL)
  {
    return sim_test_can_create_golden_copy_callback ();
  }
#endif

  can_create_gc = 0;
  result = efs_get (MADE_GOLDEN_COPY_PATH, &item_val, sizeof (uint32));
  if (result == -1 && efs_errno == ENOENT)
  {
    /* If the item file does not exist then we can create the golden copy.
     * Any other error or if the file exist return diag error */
    item_val = 1;
    result = efs_put (MADE_GOLDEN_COPY_PATH, &item_val, sizeof (uint32),
                      O_CREAT|O_AUTODIR, 0444);
    can_create_gc = 1;
  }

  return can_create_gc;
}

#endif /* FEATURE_EFS2 */
