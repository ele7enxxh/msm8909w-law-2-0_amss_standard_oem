#ifndef FLASH_DAL_CONFIG_H
#define FLASH_DAL_CONFIG_H

/**********************************************************************
 * flash_dal_config.h
 *
 * SERVICES:    Header file for flash DAL driver configuration
 *
 * DESCRIPTION: Header file for flash DAL driver configuration
 * 
 * Copyright (c) 2013 QUALCOMM Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Technologies Proprietary and Confidential.
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/dal/flash_dal_config.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
01/18/13     eo      Initial Revision
===========================================================================*/

#include "flash_miparti.h"

/* Flash driver configuration states */
enum flash_dal_config_state 
{
  FLASH_DAL_CONFIG_NAND_BOOT,
  FLASH_DAL_CONFIG_FLASHLESS_BOOT,
  FLASH_DAL_CONFIG_NAND,
  FLASH_DAL_CONFIG_NOR,
  FLASH_DAL_CONFIG_UNKNOWN = 0x7FFFFFFF /* Force 32-bit */
};

/* Flash driver configuration */
struct flash_dal_drv_config
{
  enum flash_dal_config_state config;
  flash_partable_t parti_tbl;
};

#if defined(FLASH_ENABLE_DAL_CONFIG)
/* Holds the flash driver configuration */
extern struct flash_dal_drv_config flash_dal_config;

/* Flash driver configuration */
enum flash_dal_config_state flash_dal_drv_get_config(void);
#else
/* Flash driver default configuration */
static inline enum flash_dal_config_state flash_dal_drv_get_config(void)
{
  return FLASH_DAL_CONFIG_NAND;
};
#endif

#endif /* FLASH_DAL_CONFIG_H */
