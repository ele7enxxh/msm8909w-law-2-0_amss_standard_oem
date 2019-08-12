/*=============================================================================
 *
 * FILE:      nor_wrapper.h
 *
 * DESCRIPTION: This file implements wrapper functions for
 *              read/write APIs in flash driver.
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010, 2013 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_wrapper.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 12/17/13     whc     Add RF EEPROM I2C support
 * 09/02/10     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALFramework.h"
#include "flash.h"

int flash_prog_read_pages(flash_handle_t handle, struct flash_info *dev_info, 
  uint32 start_page, uint32 num_pages, void *buffer);

int flash_prog_write_pages(flash_handle_t handle, struct flash_info *dev_info, 
  uint32 start_page, uint32 num_pages, void *buffer);

int flash_prog_write_partial_page(flash_handle_t handle, 
  struct flash_info *dev_info, uint32 page, uint32 offset, uint32 num_bytes, 
  void *buffer);

extern DALDEVICEID dev_id;

