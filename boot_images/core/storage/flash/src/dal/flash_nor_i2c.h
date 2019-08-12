#ifndef FLASH_NOR_I2C_H
#define FLASH_NOR_I2C_H

/*==========================================================================
 *
 * FILE:      flash_nor_i2c.h
 *
 * DESCRIPTION: Function and data structure declarations for I2C flash driver
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 * This section contains comments describing changes made to the
 * module. Notice that changes are listed in reverse chronological
 * order.
 *
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/dal/flash_nor_i2c.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 10/11/10     bn      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "flash_nor_entry.h"
#include "flash_extdrv_i2c.h"

#define GET_NOR_PDATA(client_ctxt)   ((struct nor_i2c_dev_pdata *) \
  client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data)
  
/* I2C NOR specific/private/probe data that is shared across clients and is
   malloced once-only at probe time */
struct nor_i2c_dev_pdata
{
  struct flash_nor_params *config_data;  /* Place to hold device details */
};

/* I2C NOR probe configuration */
struct nor_i2c_probe_cfgs
{
  uint32 base_address;
  uint32 chip_sel;
  enum flash_type dev_type;
};

/* Probe NOR I2C device and do all one time initialization for I2C driver */
int nor_i2c_probe(flash_client_ctxt *);

/* Wrapper function for flash I2C read operation */
int nor_i2c_read_op(struct nor_i2c_xfer_buff_info *i2c_read_buf_info);

/* Wrapper function for flash I2C write operation */
int nor_i2c_write_op(struct nor_i2c_xfer_buff_info *i2c_write_buf_info);

/* Init/Configure the I2C wrapper APIs */
void flash_i2c_configure(void *);

#endif /* FLASH_NOR_I2C_H */
