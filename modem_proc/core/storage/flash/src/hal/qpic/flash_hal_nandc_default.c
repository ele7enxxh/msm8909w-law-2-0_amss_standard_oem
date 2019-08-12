/*====================================================================
 *
 * FILE:        flash_hal_nandc_cfg_default.c
 *
 * SERVICES:    config file for default(full mode) NAND driver
 *
 * DESCRIPTION: This file helps configure the inclusion of desired 
 *              NAND driver functionality for full mode NAND driver.
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/qpic/flash_hal_nandc_default.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/04/12     sv      Initial Revision
 *===================================================================*/

#include "flashi_hal_nand.h"
#include "flashi_hal_nandc.h"
#include "flashi_hal_common.h"

/* Initialize command pointers for desired operations in reduced mode. */
enum flash_hal_status hal_nandc_predef_cmd_ptrs_init(
 struct flash_hal_ctlr_data *priv_data,
  struct flash_hal_predef_init_data *init_data,
  struct flash_hal_mem_data *buff_data, uint32 *used_buff_size)
{
  return hal_nandc_predef_cmd_ptrs_init_default(priv_data, init_data, 
    buff_data, used_buff_size);
}

/*
 * Init pre-defined sequence function pointers. Call the function to 
 * initialize the default(full mode) function pointers.
 */
void hal_nandc_fptrs_init(struct flash_hal_ctlr_ops *ctlr_ops)
{
  hal_nandc_fptrs_init_default(ctlr_ops);
}
