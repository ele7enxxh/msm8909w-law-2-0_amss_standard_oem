/***********************************************************************
 * fs_diag_golden_copy_handler.h
 *
 * Provides interface function to check if a golden copy can be created.
 *
 * Copyright (C) 2014 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_diag_golden_copy_handler.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-05-05    vm   Create

===========================================================================*/

#ifndef __FS_DIAG_GOLDEN_COPY_HANDLER_H__
#define __FS_DIAG_GOLDEN_COPY_HANDLER_H__

/* This function can be called to check if golden copy can created. This
 * function when returns 1, lets EFS create the golden copy. When this
 * function returns 0, EFS doesn't create the golden copy.
 */
int fs_rmts_can_create_golden_copy (void);

#ifdef FS_UNIT_TEST
  /* Callback function used by tests */
  int (*sim_test_can_create_golden_copy_callback) (void);
#endif

#endif /* not __FS_DIAG_GOLDEN_COPY_HANDLER_H__ */
