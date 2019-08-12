#ifndef NOR_DUMP_H
#define NOR_DUMP_H

/*=======================================================================
 * FILE:        nor_dump.h
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * DESCRIPTION
 *   This file has the NOR device dump structure
 *
 * Copyright (c) 2010 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/03/10     sv      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_prog.h"
#include "flash.h"

/* NOR device dump structure */
/* Holds the device structure, the partition info */
struct dump_device_info
{
  /* NOR device structure */
  flash_handle_t flash_handle;
  struct flash_info flash_info;

  uint32 read_bsize_per_blk; /* Number of bytes to be read per block */

  /* Partition to be read */
  char parti_name[FLASH_NOR_PARTI_NAME_LENGTH];

  uint32 current_block;

  /* Actual size of data read/page */
  uint32 read_stride;

};

extern struct dump_device_info dump_dev_info_param;

extern struct dump_device_info *dump_dev ;

#endif /* NOR_DUMP_H */
