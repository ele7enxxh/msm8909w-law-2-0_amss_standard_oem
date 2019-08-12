/***********************************************************************
 * rfs_config.h
 *
 * Remote File System Public Configuration file.
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Public configuration file for the Remote File System module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/rfs_config.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-11-14   rp    Create

===========================================================================*/

#ifndef __RFS_CONFIG_H__
#define __RFS_CONFIG_H__

#include "customer.h"

/**
 Maximum length of a full pathname, not including a trailing '\\0' character.
*/
#ifndef RFS_PATH_MAX
  #define RFS_PATH_MAX 256
#endif


#endif /* not __RFS_CONFIG_H__ */
