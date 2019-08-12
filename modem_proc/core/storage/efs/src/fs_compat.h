#ifndef __FS_COMPAT_H__
#define __FS_COMPAT_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  E M B E D D E D   F I L E   S Y S T E M

GENERAL DESCRIPTION
  Compatibility interface for EFS2. Users of EFS2 should use the POSIX
  interface as much as possible because this interface is deprecated.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  fs_compat_init should be called before any calls to the API are made.
  FEATURE_EFS_ACCESS_METHODS needs to be turned on.

Copyright (C) 2006, 2009-2010, 2012 by Qualcomm Technologies Inc. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_compat.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
03/20/12  wek     Cleanup customer header file inclusion.
04/28/10   vr     Relocate fs_task from EFS1 to EFS2
06/12/09  ebb     Moved rex_xxx calls to fs_osal.h abstraction.
09/08/06   yg     Fixed compilation error in tmc.c
08/31/06  dlb     Fixed garbled RCS header.
08/30/06   yg     Created new file

===========================================================================*/

#include "fs_config_i.h"

#include "fsi.h"
#include "fs_osal.h"

extern fs_os_tcb_type fs_compat_tcb;

void fs_compat_init (void);
void fs_compat_ops_cmd (fs_cmd_type *cmd_ptr);

#endif /* __FS_COMPAT_H__ */
