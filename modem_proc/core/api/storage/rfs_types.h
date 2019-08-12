/***********************************************************************
 * rfs_types.h
 *
 * RFS Type defines
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Type defines for Remote File System Module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/rfs_types.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-20   dks   Allow compilation of client on HLOS builds.
2013-11-07   dks   Create

===========================================================================*/

#ifndef __RFS_TYPES_H__
#define __RFS_TYPES_H__

#include "comdef.h"

struct rfs_stat_buf
{
  uint32  st_size;      /**< File size in bytes.          */
  uint16  st_mode;      /**< File mode. Is not implemented and will be filled
                             with 0 value until specified in documentation */
};

#endif /* not __RFS_TYPES_H__ */
