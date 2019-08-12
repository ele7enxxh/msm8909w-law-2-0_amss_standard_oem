/***********************************************************************
 * fs_os_qurt.h
 *
 * Filesystem qurt abstraction
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_osal/inc/fs_os_qurt.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-02-06   dks   Create

===========================================================================*/

#ifndef __FS_OS_QURT_H__
#define __FS_OS_QURT_H__

#include "fs_os_config_i.h"

#ifdef FEATURE_FS_OS_FOR_QURT

#include "qurt_rmutex.h"
#include "qurt_thread.h"
#include "qurt_error.h"

typedef qurt_thread_t fs_os_thread_t;
typedef qurt_mutex_t fs_os_mutex_t;

#ifdef FS_OS_SIMULATOR_BUILD
  typedef void* fs_os_thread_return_type;
  #define FS_OS_THREAD_DEFAULT_STACK_SIZE SIM_QURT_THREAD_DEFAULT_STACK_SIZE
#else
  typedef void fs_os_thread_return_type;
  #define FS_OS_THREAD_DEFAULT_STACK_SIZE (4*1024)
#endif

#ifndef FS_OS_QURT_DEFAULT_PRIORITY
  #define FS_OS_QURT_DEFAULT_PRIORITY   85
#endif


#define fs_os_thread_exit()   \
  do                          \
  {                           \
    qurt_thread_exit(0);      \
    return NULL;              \
  } while (0)

#endif /* not FEATURE_FS_OS_FOR_QURT */

#endif /* not __FS_OS_QURT_H__ */


