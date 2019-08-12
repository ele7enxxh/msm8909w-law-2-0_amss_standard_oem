#ifndef FLASH_HAL_MAIN_H
#define FLASH_HAL_MAIN_H
/*=======================================================================
 * FILE:        flash_hal_main.h
 *
 * SERVICES:    Header for Common APIs used across all controllers
 *
 * DESCRIPTION: Header for Common APIs used across all controllers
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2008-2009,2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/hal/flash_hal_main.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/13/12     sv      Add declation for hal_nandc_deinit
 * 05/20/11     bb      Flash hal Optimization - Remove Onenand codes
 * 07/27/09     jz      Added XMEMC based OneNAND support
 * 06/03/09     mh      Clean-ups.
 * 05/29/09     mh      Rename nandc, sflashc init API headers
 * 03/12/09     eo      Add comments
 * 03/30/08     BB      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flashi_hal.h"

/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

/*---------------------------------------------------------------------
 *             Function Declarations
 * ------------------------------------------------------------------*/

/* Initialization for the regular NAND controller */
void hal_nandc_init(const struct flash_hal_dev_config *dev_cfg,
  flash_hal_dev_info *dev_info, enum flash_hal_status *status);

/* De-initialization for the regular NAND controller */
void hal_nandc_deinit(flash_hal_dev_info *dev_info);

/* Flash HAL current sequence */
extern struct flash_hal_cmd_seq flash_hal_op_seq;

#endif /* FLASH_HAL_MAIN_H */
