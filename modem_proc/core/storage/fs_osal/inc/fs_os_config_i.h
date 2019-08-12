/***********************************************************************
 * fs_os_config_i.h
 *
 * OS abstraction Config file.
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_osal/inc/fs_os_config_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-02-06   dks   Create

===========================================================================*/

#ifndef __FS_OS_CONFIG_I_H__
#define __FS_OS_CONFIG_I_H__

#include "customer.h"

#if !defined (FEATURE_FS_OS_FOR_QURT)  && \
    !defined (FEATURE_FS_OS_FOR_POSIX)
    #error "Configure build for some OS. Neither of Qurt or Posix defined"
#endif

#endif /* __FS_OS_CONFIG_I_H__ */

