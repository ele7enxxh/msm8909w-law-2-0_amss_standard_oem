#ifndef FLASH_BOOT_OPS_SETUP_H
#define FLASH_BOOT_OPS_SETUP_H

/**********************************************************************
 * flash_boot_ops_setup.h
 *
 * SERVICES:    Header file for flash operations used by Bootloader
 *
 * DESCRIPTION: Header file for flash operations used by Bootloader
 * 
 * Copyright (c) 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/inc/flash_boot_ops_setup.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
07/01/11     eo      Initial Revision
===========================================================================*/
#include "comdef.h"

/* Flash boot operations to update SMEM AARM with MIBIB partition table */
void flash_boot_ops_get_parti_tbl(uint32 *buffer, uint32 *size_in_bytes);

#endif /* FLASH_BOOT_OPS_SETUP_H */
