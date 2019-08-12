/*=======================================================================
 * FILE:        nor_dump.c
 *
 * SERVICES:    functions to dump partition contents
 *
 * DESCRIPTION
 *   This file has the main functions used to dump images from flash
 *
 * Copyright (c) 2010-2011, 2013 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_dump.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 12/17/13     whc     Add RF EEPROM I2C support
 * 07/20/11     sv      Use compile flag to decide SPI/I2C NOR tools
 * 09/09/10     sv      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_prog.h"
#include "nor_config.h"
#include "nor_param.h"

#include "DALDeviceId.h"

#include "nor_wrapper.h"
#include "flash_log.h"
#include "nor_core.h"
#include "nor_dump.h"

/* TODO: Utility routine includes */
#include <string.h>  /* for memcpy() */
#include "stdio.h"

/*===========================================================================
                           LOCAL DATA
===========================================================================*/

struct dump_device_info dump_dev_info_param;

struct dump_device_info *dump_dev = &dump_dev_info_param;

/***********************************************************************
This function initializes the structure for the partition dump .

**********************************************************************/

uint32 flash_prog_init_parti_dump(void)
{
  KICK_WATCHDOG();

  FLASHLOG(1, ("\n\n------ Initialize for partition dump ---------------\n"));

  FLASHLOG(1, ("Probing flash device:  "));

  /*
   * Probe will fill values in structure if it succeeds.
   * Currently, which_flash parameter is hard coded to 0, but in
   * future this will be dynamic.
   */
  KICK_WATCHDOG();

  /* Memset the partition name */
  DALSYS_memset(dump_dev->parti_name, 0, FLASH_NOR_PARTI_NAME_LENGTH);

  /* Copy the partition name */
  DALSYS_memcpy(dump_dev->parti_name, prog_param.data,
    FLASH_NOR_PARTI_NAME_LENGTH);
  FLASHLOG(3, ("Dumping contents of partition %s\n", dump_dev->parti_name));

  /* Open the partition.  The code which does this is now
   * actually reading the partition information from the flash.
   */

  KICK_WATCHDOG();
  if (NULL == dump_dev->flash_handle)
  {
    /* Probe the device */
    if (DAL_SUCCESS != flash_device_attach(dev_id, 
      &(dump_dev->flash_handle)))                  
    {
      FLASHLOG(3, ("Error attaching to device driver \n"));
      SET_ERR (ERR_DEV_MISSING);
      return FLASH_PROG_FAIL;
    }
  }
  FLASHLOG(3, ("Device+Client Attached!\n"));

  if (DAL_SUCCESS == flash_open_partition(dump_dev->flash_handle,
    (const unsigned char *)dump_dev->parti_name))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  if (DAL_SUCCESS == flash_get_info(dump_dev->flash_handle,
    FLASH_DEVICE_INFO, &dump_dev->flash_info))
  {
    FLASHLOG(3, ("- flash_get_info successful\n"));
  }
  else
  {
    FLASHLOG(3, ("- flash_get_info error\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  /* TBD: Rewrite this for regular NOR devices having different block sizes
   * within a device.
   */

  /* Determine the block size depending on the options selected */
  dump_dev->read_stride = dump_dev->flash_info.partition.total_page_size_bytes;

  dump_dev->read_bsize_per_blk =
    dump_dev->flash_info.partition.pages_per_block * dump_dev->read_stride;

  FLASHLOG(3,("dump_init:  blocksize    0x%x\n",
    dump_dev->read_bsize_per_blk));
  FLASHLOG(3,("dump_init:  block start  0x%x\n",
    dump_dev->flash_info.partition.physical_start_block));
  FLASHLOG(3,("dump_init:  num blocks   0x%x\n",
    dump_dev->flash_info.partition.block_count));

  /* Return the number/size of blocks in the partition to the T32 script.*/
  prog_param.size = dump_dev->flash_info.partition.physical_start_block;
  prog_param.width = dump_dev->flash_info.partition.physical_start_block +
    dump_dev->flash_info.partition.block_count;
  prog_param.addr = dump_dev->flash_info.partition.block_count;
  prog_param.offset = dump_dev->read_bsize_per_blk;

  return FLASH_PROG_SUCCESS;
}


/***********************************************************************
FUNCTION      flash_prog_dump_parti

DESCRIPTION   Dump the pages in the partition.


RETURN VALUE  FLASH_PROG_SUCCESS if successful else
              FLASH_PROG_FAIL
**********************************************************************/

uint32 flash_prog_dump_parti(void)
{
  uint32 block = 0;
  uint32 current_page;
  uint32 pages_per_block = 0;
  char *data;
  int result = FLASH_DEVICE_DONE;

  if (NULL == dump_dev->flash_handle)
  {
    FLASHLOG(1, ("Error: Init function not called.\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  data = (char *)&prog_param.data;
  block = prog_param.addr;
  pages_per_block = dump_dev->flash_info.partition.pages_per_block;

  FLASHLOG(3,("dump_block:  block 0x%x\n", block));

  /* Calculate the current page number */
  current_page = block * pages_per_block;

  result = flash_prog_read_pages(dump_dev->flash_handle, 
    &dump_dev->flash_info, current_page, pages_per_block, data);

  /* return the Block read */
  prog_param.addr = block;

  /* Return status */
  prog_param.width = 1;

  return result;
}
