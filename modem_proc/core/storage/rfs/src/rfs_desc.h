/***********************************************************************
 * rfs_desc.h
 *
 * File descriptor management for rfs
 *
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * File descriptor management for rfs
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/rfs/src/rfs_desc.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-10-15   dks   Create

===========================================================================*/

#ifndef __RFS_DESC_H__
#define __RFS_DESC_H__

#include "rfs_config_i.h"

void rfs_desc_init (void);

int rfs_desc_allocate_fd (const char *path, int oflag);

int rfs_desc_free_fd (int fd);

int rfs_desc_lookup_fd (int fd, char *fname, uint32 fname_len,
                        int *oflag, uint32 *offset);

int rfs_desc_update_offset (int fd, uint32 offset);

#endif /* not __RFS_DESC_H__ */
