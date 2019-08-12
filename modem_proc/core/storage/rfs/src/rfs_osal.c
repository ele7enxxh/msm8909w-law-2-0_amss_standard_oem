/***********************************************************************
 * rfs_osal.c
 *
 * OS abstraction layer implemenation.
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Using this file allows us to compile RFS API code without any sort of
 * dependency on underlying operating system.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/rfs/src/rfs_osal.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-09-19   dks   Add hooks to extract performance numbers.
2014-08-04   rp    Debug checks to ensure mutex calls succeed.
2014-01-28   rp    Move from rex to posix.
2014-01-27   dks   Create

===========================================================================*/

#include "rfs_config_i.h"
#include "rfs_osal.h"
#include "rfs_assert.h"

#include <pthread.h>

void
rfs_osal_mutex_init (rfs_osal_mutex_handle *lock_handle)
{
  pthread_mutexattr_t lock_attrb;
  int result;

  result = pthread_mutexattr_init (&lock_attrb);
  RFS_ASSERT (result == 0);

  result = pthread_mutexattr_settype (&lock_attrb, PTHREAD_MUTEX_RECURSIVE);
  RFS_ASSERT (result == 0);

  result = pthread_mutex_init (lock_handle, &lock_attrb);
  RFS_ASSERT (result == 0);

  result = pthread_mutexattr_destroy (&lock_attrb);
  RFS_ASSERT (result == 0);
}

void
rfs_osal_mutex_lock (rfs_osal_mutex_handle *lock_handle)
{
  int result;
  result = pthread_mutex_lock (lock_handle);
  RFS_ASSERT (result == 0);
}

void
rfs_osal_mutex_unlock (rfs_osal_mutex_handle *lock_handle)
{
  int result;
  result = pthread_mutex_unlock (lock_handle);
  RFS_ASSERT (result == 0);
}

