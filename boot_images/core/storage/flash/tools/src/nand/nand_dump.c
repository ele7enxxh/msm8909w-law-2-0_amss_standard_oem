/*=======================================================================
 * FILE:        nand_dump.c
 *
 * SERVICES:    Functions to dump file system area
 *
 * DESCRIPTION
 *  These functions dump the named partition and copy the data back to the
 *  T32 debugger. There is also support to dump a MIBIB image.
 *
 * Copyright (c) 2008-2010, 2012, 2014 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_dump.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/14/14     sb      Move page buffers to Code RAM to support non DDR based tools
 * 09/11/12     eo      Remove miboot.h
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 11/03/09     wt      Included header file for the definition of dump_device_info
 * 09/18/09     op      Make DM buffer 32byte alignment to avoid any misaligned
 * 06/18/09     bb      Include bad block data during partition dump in RAW mode
 * 04/17/09     eo      Fix partition block limit check in dump partition.
 * 02/25/09     eo      Cleanup mibib_page_buffer initialization.
 * 02/20/09     jz      Use FLASH_PROG_SUCCESS/FAIL return codes for nand_tools
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "DALDeviceId.h"
#include "flash_miparti.h"

#include "flash_prog.h"
#include "nand_config.h"
#include "nand_param.h"
#include "nand_wrapper.h"
#include "nand_core.h"
#include "nand_tools.h"
#include "nand_dump.h"

/* TODO: Utility routine includes */
#include <string.h>
#include "stdio.h"

/*===========================================================================
                           LOCAL DATA
===========================================================================*/

struct dump_device_info dump_dev_info_param;

struct dump_device_info *dump_dev = &dump_dev_info_param;

/* If flash tools get compiles as part of bootloaders, where the data caches is
 * setup, the 32 bytes aligment changes of source buffers here would make them
 * more cache line friendly and avoid any misaligned boot data structure that
 * gets invalidated
 */
/* Buffer used in generating the MIBIB image */
extern unsigned char mibib_pg_buf[MAX_PAGE_SIZE];

/* Security mode required in generation of the MIBIB signature */
static uint32 sec_mode = 0;

/***********************************************************************
This function initializes the structure for the partition dump .

**********************************************************************/

uint32 flash_prog_init_parti_dump(void)
{
  KICK_WATCHDOG();

  sec_mode = prog_param.size;

  FLASHLOG(1, ("\n\n------ Initialize for partition dump ---------------\n"));

  FLASHLOG(1, ("Probing flash device:  "));

  /*
   * Probe will fill values in structure if it succeeds.
   * Currently, which_flash parameter is hard coded to 0, but in
   * future this will be dynamic.
   */
  KICK_WATCHDOG();

  /* Memset the partition name */
  DALSYS_memset(dump_dev->parti_name, 0, FLASH_PART_NAME_LENGTH);

  /* Copy the partition name */
  nand_memcpy(dump_dev->parti_name, prog_param.data,
    FLASH_PART_NAME_LENGTH);
  FLASHLOG(3, ("Dumping contents of partition %s\n", dump_dev->parti_name));

  /* Open the partition.  The code which does this is now
   * actually reading the partition information from the flash.
   */

  KICK_WATCHDOG();
  if (NULL == dump_dev->flash_handle)
  {
    /* Probe the device */
    if (DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1,  //Device Id
      &(dump_dev->flash_handle)))                  //Handle
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

  /* Determine the block size depending on the options selected */
  dump_dev->read_stride = dump_dev->flash_info.partition.total_page_size_bytes;

  dump_dev->read_bsize_per_blk =
    dump_dev->flash_info.partition.pages_per_block * dump_dev->read_stride;

  FLASHLOG(3,("dump_init:  page size    0x%x\n",
    dump_dev->flash_info.partition.total_page_size_bytes));
  FLASHLOG(3,("dump_init:  page start  0x%x\n",
    dump_dev->flash_info.partition.physical_start_block * 
	dump_dev->flash_info.partition.pages_per_block));
  FLASHLOG(3,("dump_init:  num pages   0x%x\n",
    dump_dev->flash_info.partition.block_count * 
	dump_dev->flash_info.partition.pages_per_block));

  /* Return the number/size of blocks in the partition to the T32 script.*/
  prog_param.size = dump_dev->flash_info.partition.physical_start_block;
  prog_param.width = dump_dev->flash_info.partition.physical_start_block +
    dump_dev->flash_info.partition.block_count;
  prog_param.addr = dump_dev->flash_info.partition.block_count * 
    dump_dev->flash_info.partition.pages_per_block;
  prog_param.offset = dump_dev->flash_info.partition.total_page_size_bytes;

  return FLASH_PROG_SUCCESS;
}


/***********************************************************************
FUNCTION      flash_prog_dump_parti

DESCRIPTION   Dump the pages in the partition.


RETURN VALUE  FLASH_PROG_SUCCESS if successful else
              FLASH_PROG_FAIL
**********************************************************************/

uint32 flash_prog_dump_parti()
{
  uint32 current_page;
  char *data;
  int result = FLASH_DEVICE_DONE;

  if (NULL == dump_dev->flash_handle)
  {
    FLASHLOG(1, ("Error: Init function not called.\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  data = (char *)&prog_param.data;
  current_page = prog_param.addr;
  
  FLASHLOG(3,("dump_page:  page 0x%x\n", current_page));

  result = flash_prog_read_page(dump_dev->flash_handle, &dump_dev->flash_info,
      current_page, FLASH_READ_RAW, mibib_pg_buf);

    if (DAL_SUCCESS != result)
    {
      /* if device is OneNAND, ignore page erased failure */
      if (!((FLASH_DEVICE_FAIL_PAGE_ERASED == result) &&
           (FLASH_ONENAND == dump_dev->flash_info.type)))
      {
        FLASHLOG(1, ("Error: read and udata failed\n"));
        FLASHLOG(3,("Page number 0x%x\n", current_page));
        SET_ERR (ERR_READ_FAIL);
        return FLASH_PROG_FAIL;
      }
    }

  nand_memcpy(data, &mibib_pg_buf[0],
    dump_dev->flash_info.partition.total_page_size_bytes);

  /* return the Block read */
  prog_param.addr = current_page;

  /* Return status */
  prog_param.width = 1;

  return FLASH_PROG_SUCCESS;
}


/*===========================================================================

  This function return a size in log base 2

===========================================================================*/
static uint8 flash_prog_get_log_base2(uint32 size)
{
  uint8 log_2_size=1;
  while (size > 2)
  {
    log_2_size++;
    size = size >> 1;
  }
  return log_2_size;
}

/*===========================================================================

  This function read a mibib block to a passed buffer

===========================================================================*/
uint32 flash_prog_dump_mibib_parti_block(void)
{
  int block;
  int result = FLASH_DEVICE_DONE;
  int current_page;
  char *data;
  int i;
  static int decode_key_stamped = 0;

  block = prog_param.addr;

  current_page = block * dump_dev->flash_info.partition.pages_per_block;

  data = (char *)&prog_param.data;

  //FLASHLOG (3,("fdmpb:  block 0x%x\n", block));

  for (i=0; i<dump_dev->flash_info.partition.pages_per_block; i++)
  {
    result = flash_prog_read_page(dump_dev->flash_handle, &dump_dev->flash_info,
      current_page, FLASH_READ_MAIN, mibib_pg_buf);

    if (DAL_SUCCESS != result)
    {
      if (FLASH_DEVICE_FAIL_PAGE_ERASED == result)
      {
        DALSYS_memset(&mibib_pg_buf[0], 0xFF,
          dump_dev->flash_info.partition.total_page_size_bytes);
      }
      else
      {
        FLASHLOG(1, ("Error: read and udata failed\n"));
        FLASHLOG(3,("Page number 0x%x\n", current_page));
        SET_ERR (ERR_READ_FAIL);
        return FLASH_PROG_FAIL;
      }
    }
    else
    {
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes] =
        NAND_MIBIB_MBN_PAGE_WRITTEN_FLAG;
      DALSYS_memset(&mibib_pg_buf[
        dump_dev->flash_info.partition.page_size_bytes + 1], 0xFF,
        (dump_dev->flash_info.partition.total_page_size_bytes -
        dump_dev->flash_info.partition.page_size_bytes - 1));
    }

        /* Generates the mibib signature key. This signature is used by QFIT
         * tool to validate if the mibib.mbn generated is compatible to the
         * device the factory image is generated for. Thus we add this to the
         * dump so that the generated images and dumped images will match.*/
    if (decode_key_stamped == 0)
    {
      /* Decode key start flag */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+1] =
        NAND_MIBIB_MBN_DECODE_KEY_BEGIN;
      /* version */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+2] =
        NAND_MIBIB_MBN_DECODE_KEY_VERSION;
      /* page size in bytes log2*/
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+3] =
        flash_prog_get_log_base2(dump_dev->flash_info.partition.page_size_bytes);
      /* block size in page log2*/
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+4] =
        flash_prog_get_log_base2(dump_dev->flash_info.partition.pages_per_block);
      /* device size in blocks */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+5] =
        flash_prog_get_log_base2(dump_dev->flash_info.partition.block_count);
      /* Mibib size MIBIB dump size in blocks  */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+6] =
        (sec_mode) ? FLASH_MI_BLOCK_LAST - 1 : FLASH_MI_BLOCK_LAST;
      /* Decode key end flag */
      mibib_pg_buf[dump_dev->flash_info.partition.page_size_bytes+7] =
        NAND_MIBIB_MBN_DECODE_KEY_END;

      decode_key_stamped = 1;
    }
    nand_memcpy(data, &mibib_pg_buf[0],
      dump_dev->flash_info.partition.total_page_size_bytes);
    current_page++;
    data += dump_dev->flash_info.partition.total_page_size_bytes;
  }
  return FLASH_PROG_SUCCESS;
}

