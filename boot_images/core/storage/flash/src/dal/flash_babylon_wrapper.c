/**********************************************************************
 * flash_babylon_wrapper.c
 *
 *  This file implements wrappers for Babylon APIs. This file is
 *  currently used for non-WM platforms.
 *
 * Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_babylon_wrapper.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     --------------------------------------------------------
05/21/11     bb      create common babylon wrapper file
08/11/10     nr      Added support for Flash profiling framework
02/12/10     wt      Modified Babylon Marker for 2X support
05/24/09     bb      Add support for babylon
===========================================================================*/

#include "flash_stats_framework.h"

#include "bab_public.h"

/* Babylon port init */
void flash_bab_init(void)
{
  bab_port_init();
}

/* Marks the start of a marker in driver */
void flash_bab_start(uint8 id , uint8 npages)
{
  bab_marker_start (BAB_STREAM_FLASH, id, npages);
}

/* Marks the end of a marker in driver */
void flash_bab_end(uint8 id)
{
  bab_marker_end (BAB_STREAM_FLASH, id, 0xff);
}

