/*===========================================================================
             Public header just to include fs_diag_access.h

  Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_diag/inc/mcfg_diag.h#1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-25   sbt    created.
===========================================================================*/

#include "fs_diag_access.h"

/** Identifies the major version number of this API file. */
#define VER_MCFG_FS_DIAG_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_MCFG_FS_DIAG_H_MINOR     1

/*
 * Initialization function for this module. Registers the packet handlers
 * and sets up local data structures.
 */
void mcfg_diag_init(void);

