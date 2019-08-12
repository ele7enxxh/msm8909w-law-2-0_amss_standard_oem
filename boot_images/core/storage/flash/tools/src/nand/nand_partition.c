/*=======================================================================
 * FILE:        nand_partition.c
 *
 * SERVICES:    Flash partition support
 *
 * GENERAL DESCRIPTION
 *
 *   This file implements functions and data used to access partitions
 *   for NAND flash parts.
 *
 * Copyright (c) 2008-2010, 2012-2014 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_partition.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/14/14     sb      Move page buffers to Code RAM to support non DDR based tools
 * 09/03/13     sb      Return length in available blocks instead GROW for last partition 
 * 06/21/13     sb      Clean up as part of removing user partition table from MIBIB 
 * 02/21/12     sb      Compare attributes also while comparing partition tables
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 09/01/10     nr      Defer first block erase in tools till stream write
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 07/20/10     jz      Added 2X support
 * 07/16/10     jz      Fixed for 8K page support
 * 01/07/10     eo      Add support for 4K page OneNAND
 * 10/12/09     mh      Add support for 8K
 * 09/18/09     op      Make DM buffer 32byte alignment to avoid any misaligned
 * 06/26/09     rk      Return when latest_mibib_block is not found
 * 07/08/09     mm      Support for total main and spare bytes
 * 05/07/09     eo      Clean up gcc related compiler warnings.
 * 05/06/09     sv      Add check for null handle from device attach
 * 04/28/09     bb      Fix for compilation errors during Linux build
 * 02/25/09     eo      Fix printing of remaining partition layouts.
 * 02/09/09     jz      Moved some functions here from nand_core.c,
 *                      also made the return types consistent.
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include <string.h>

#include "flash_miparti.h"
#include "flash_mibib.h"
#include "flash_micrc.h"
#include "crc.h"

#include "DALDeviceId.h"
#include "DALSys.h"

#include "nand_wrapper.h"
#include "nand_config.h"
#include "nand_param.h"
#include "nand_core.h"

/* If flash tools get compiles as part of bootloaders, where the data caches is
 * setup, the 32 bytes aligment changes of source buffers here would make them
 * more cache line friendly and avoid any misaligned boot data structure that
 * gets invalidated
 */
#if !defined(BUILD_EHOSTDL) && !defined(BUILD_HOSTDL)
#pragma arm section zidata = "FLASH_TOOLS_HEAP_ZONE"
#endif
static ALIGN(ALIGN_VAL) unsigned char mibib_pg_buf[MAX_PAGE_SIZE];
static ALIGN(ALIGN_VAL) unsigned char is_erased_pg_buf[MAX_PAGE_SIZE];
#if !defined(BUILD_EHOSTDL) && !defined(BUILD_HOSTDL)
#pragma arm section zidata
#endif

static enum flash_block_state blk_state;

static int latest_mibib_block = -1;

static uint32 flash_prog_print_mibib_pages(int mibib_block);


/*===========================================================================
  This function reads in both MIBIB blocks from th flash as present and
  prints out information from them.
===========================================================================*/

uint32 flash_prog_print_mibib(void)
{

  KICK_WATCHDOG();

  FLASHLOG(1,("\n\n------ Print MIBIB ------\n"));

  /* Probe will fill values in structure if it succeeds.
   * Currently, which_flash parameter is hard coded to 0, but in
   * future this will be dynamic.
   */
  if (FLASH_PROG_SUCCESS !=
    flash_prog_init_for_partition_image(&mibib, MIBIB_DEVICE_ALL_PARTI_NAME))
  {
    FLASHLOG(1,("Error: no nand_device found \n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }
  else
  {
    FLASHLOG(3,("\n MIBIB partition opened ptr = 0x%lx ::  \n",
        (unsigned long int)mibib.flash_handle));
  }

  KICK_WATCHDOG();

  latest_mibib_block = mibib.flash_info.mibib_info.new_mibib_block;

  KICK_WATCHDOG();

  if (latest_mibib_block == -1)
  {
    FLASHLOG ( 1,("\n\nMIBIB not present\n"));
  }
  else
  {
    FLASHLOG ( 1,("\n\nMIBIB present in block 0x%x\n", latest_mibib_block));
    flash_prog_print_mibib_pages(latest_mibib_block);
  }

  KICK_WATCHDOG();

  FLASHLOG ( 1,("===========  Done ===========\n\n"));

  return FLASH_PROG_SUCCESS;

} /* flash_prog_print_mibib */


/*
 * This function takes a valid MIBIB block number as an input and
 * prints out its contents from them. Assumes that MIBIB partition
 * has been opened by called.
 */

static uint32 flash_prog_print_mibib_pages(int mibib_block)
{
  int page;
  int result = FLASH_DEVICE_DONE;
  mi_boot_info_t mibib_magic;
  flash_partable_t parti_sys;

  KICK_WATCHDOG();

  FLASHLOG ( 1,("\n\nMIBIB Contents from block 0x%x are:\n", mibib_block));
  FLASHLOG ( 1,("====================================================\n"));

  /* If we here, we are sure that mibib magic and partition tbl pages
   * were present and vaid. We can report that here.
   */

  page = (mibib.flash_info.partition.pages_per_block * mibib_block);

  result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
    page, FLASH_READ_MAIN, mibib_pg_buf);

  if (FLASH_DEVICE_DONE != result)
  {
    if (FLASH_DEVICE_FAIL_PAGE_ERASED == result)
    {
      FLASHLOG(3, ("ebb: Nothing to do\n"));
      SET_ERR(ERR_READ_FAIL);
      return FLASH_PROG_SUCCESS;
    }
    else
    {
      FLASHLOG(1, ("ebb: fpmp: Page read of MIBIB version page failed 0x%x\n",
        page));
      SET_ERR (ERR_READ_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  mibib_magic = (mi_boot_info_t)mibib_pg_buf;

  FLASHLOG(1,("Page0  (MIBIB_HDR): Valid, Ver: 0x%lx, Age: 0x%lx\n",
    (unsigned long int)mibib_magic->version,
    (unsigned long int)mibib_magic->age));

  page = (mibib.flash_info.partition.pages_per_block * mibib_block) +
    MIBIB_PAGE_PARTITION_TABLE;

  result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
    page, FLASH_READ_MAIN, mibib_pg_buf);

  if (DAL_SUCCESS != result)
  {
    if (FLASH_DEVICE_FAIL_PAGE_ERASED == result)
    {
      FLASHLOG(3, ("ebb: Nothing to do\n"));
      SET_ERR(ERR_READ_FAIL);
      return FLASH_PROG_SUCCESS;
    }
    else
    {
      FLASHLOG(1, ("ebb: Page read of Sys partition tbl page failed 0x%x\n",
        page));
      SET_ERR (ERR_READ_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  parti_sys = (flash_partable_t)mibib_pg_buf;
  FLASHLOG(1,("Page1  (SYS_PARTI): Valid, Ver: 0x%lx\n",
    (unsigned long int)parti_sys->version));

  /* If we here, we are sure that mibib magic and partition tbl pages
   * were present and vaid. We can report that here.
   */

  page = (mibib.flash_info.partition.pages_per_block * mibib_block) +
    MIBIB_PAGE_MIBIB_BLK_CRC;

  result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
    page, FLASH_READ_MAIN, mibib_pg_buf);

  if (FLASH_DEVICE_DONE != result)
  {
    if (FLASH_DEVICE_FAIL_PAGE_ERASED == result)
    {
      FLASHLOG (3, ("ebb: Nothing to do\n"));
      SET_ERR(ERR_READ_FAIL);
      return FLASH_PROG_SUCCESS;
    }
    else
    {
      FLASHLOG(1, ("ebb: Page read of CRC checksum page faile e 0x%x\n",
        page));
      SET_ERR (ERR_READ_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  KICK_WATCHDOG();

  FLASHLOG ( 1,("====================================================\n"));
  FLASHLOG ( 1,("\n\n"));


  (void)mibib_magic;
  (void) parti_sys;

  return FLASH_PROG_SUCCESS;

} /* flash_prog_print_mibib_pages */


/*
 * This function prints out all the information about a flash layout
 */

uint32 flash_prog_print_layout(void)
{
  int i;
  int page, limit, offset;
  uint32 num_partitions;
  flash_partable_t parti_sys;
  int result = FLASH_DEVICE_DONE;

  KICK_WATCHDOG();


  FLASHLOG ( 1,("\n\n================ Flash Layout =================\n\n\n\n"));

  /* Probe will fill values in structure if it succeeds.
   * Currently, which_flash parameter is hard coded to 0, but in
   * future this will be dynamic.
   */
  if (FLASH_PROG_SUCCESS !=
    flash_prog_init_for_partition_image(&mibib, MIBIB_DEVICE_ALL_PARTI_NAME))
  {
    FLASHLOG(1,("Error: no nand_device found \n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }
  else
  {
    FLASHLOG(3,("\n MIBIB partition opened ptr = 0x%lx ::  \n",
      (unsigned long int)mibib.flash_handle));
  }

  KICK_WATCHDOG();

  latest_mibib_block = mibib.flash_info.mibib_info.new_mibib_block;

  if ( -1 == latest_mibib_block )
  {
    FLASHLOG(1,("Error: no mibib block available. Nothing to Print.\n"));
    return FLASH_PROG_SUCCESS;
  }

  page = (mibib.flash_info.partition.pages_per_block * latest_mibib_block) +
  MIBIB_PAGE_PARTITION_TABLE;

  result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
    page, FLASH_READ_MAIN, mibib_pg_buf);

  if (FLASH_DEVICE_DONE != result)
  {
    if(FLASH_DEVICE_FAIL_PAGE_ERASED == result)
    {
      FLASHLOG (3, ("ebbt: Nothing to do\n"));
      SET_ERR(ERR_READ_FAIL);
      return FLASH_PROG_SUCCESS;
    }
    else
    {
      FLASHLOG(1, ("ebbt: Page read of System Partition page failed 0x%x\n",
        page));
      SET_ERR (ERR_READ_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  parti_sys = (flash_partable_t)mibib_pg_buf;

  num_partitions = parti_sys->numparts;

  FLASHLOG ( 1,(" Partition\t    Start Block\t\tLimit\t\tPage 0\n"));
  FLASHLOG ( 1,("-----------\t   -------------\t------\t\t------\n"));

  for (i=0; i<num_partitions; i++)
  {
    FLASHLOG ( 1,("  %-6s", (char *)&parti_sys->part_entry[i].name));
    offset = parti_sys->part_entry[i].offset;
    FLASHLOG ( 1,("\t\t0x%x", offset));
    limit = parti_sys->part_entry[i].length;
    if (limit == -1)
    {
      limit = mibib.flash_info.partition.block_count;
      FLASHLOG ( 1,("\t\t 0x%x", limit));
    }
    else
    {
      FLASHLOG ( 1,("\t\t 0x%x", (offset + limit)));
    }

    page = mibib.flash_info.partition.pages_per_block * offset;

    result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
      page, FLASH_READ_MAIN, is_erased_pg_buf);

    if (FLASH_DEVICE_DONE != result)
    {
      if(FLASH_DEVICE_FAIL_PAGE_ERASED == result)
      {
        FLASHLOG ( 1,("\t\tEmpty\n"));
      }
      else
      {
        FLASHLOG(1, ("fpl: Page read of User Partition page failed e 0x%x\n",
          page));
      }
    }
    else
  {
      FLASHLOG ( 1,("\t\tWritten\n"));
    }
    KICK_WATCHDOG();
  }

  FLASHLOG ( 1,("\n\n=================  Done =================\n\n\n\n"));

  return FLASH_PROG_SUCCESS;

} /* nand_print_layout */

/*
 *
 * This function opens the device, then the named partition and sets up
 * device structure to use that partition.
 *
 */
int flash_prog_init_for_partition_image (parti_ctl_t pctl, char *pname)
{
  KICK_WATCHDOG();

  FLASHLOG(3,("ifpi:  trying to find partition %s\n", pname));

  if (pctl->flash_handle == NULL)
  {
    /* Probe the device */
    if ((FLASH_DEVICE_DONE != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1,
      &(pctl->flash_handle))) || (pctl->flash_handle == NULL))
    {
      FLASHLOG(3, ("Error attaching to device driver \n"));
      SET_ERR (ERR_DEV_MISSING);
      return FLASH_PROG_FAIL;
    }
  }

  FLASHLOG(3, ("Device+Client Attached!\n"));

  if (FLASH_DEVICE_DONE == flash_open_partition(pctl->flash_handle,
    (const unsigned char *)pname))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  if (FLASH_DEVICE_DONE == flash_get_info(pctl->flash_handle,
    FLASH_DEVICE_INFO, &pctl->flash_info))
  {
    FLASHLOG(3, ("Device Initialization: flash_get_info successful\n"));
  }
  else
  {
    FLASHLOG(3, ("- flash_get_info error\n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  KICK_WATCHDOG();

  FLASHLOG (3, ("\n\n"));
  FLASHLOG (3,("Pages/Block is %1d \n",
    (unsigned long int)pctl->flash_info.partition.pages_per_block));
  FLASHLOG (3,("Page size is 0x%x \n",
    (unsigned long int)pctl->flash_info.partition.page_size_bytes));
  FLASHLOG (3,("Block Count is 0x%x \n",
    (unsigned long int)pctl->flash_info.partition.block_count));
  FLASHLOG (3,("Attributes is 0x%x \n",
    (unsigned long int)pctl->flash_info.partition.attributes));

  /* Init some things that must be re-inited on every invocation. */
  pctl->start_addr_loaded = FALSE;
  pctl->partial_page_index = 0;
  pctl->page_size = pctl->flash_info.partition.page_size_bytes;
  pctl->write_ops = FLASH_WRITE_MAIN;
  pctl->current_block = 0;
  pctl->current_page = 0;
  pctl->avail_pages_in_block = 0;
  pctl->partition_initial_blk_prepared = FALSE;
  
  if (FLASH_ONENAND == pctl->flash_info.type)
  {
    if (FLASH_NAND_PAGE_SIZE_2048 < pctl->flash_info.partition.page_size_bytes)
    {
      pctl->ecc_num_cw = FLASH_ONENAND_4K_SPARE_NUM_CW;
      pctl->ecc_offset = FLASH_ONENAND_4K_SPARE_ECCM_BOFF;
      pctl->ecc_len = FLASH_ONENAND_4K_SPARE_ECCM_BLEN;
    }
    else
    {
      pctl->ecc_num_cw = FLASH_ONENAND_2K_SPARE_NUM_CW;
      pctl->ecc_offset = FLASH_ONENAND_2K_SPARE_ECCM_BOFF;
      pctl->ecc_len = FLASH_ONENAND_2K_SPARE_ECCM_BLEN +
        FLASH_ONENAND_2K_SPARE_ECCS_BLEN;
    }
    FLASHLOG(3,("\nOneNAND::  ecc_offset = 0x%x, ecc_len = %d\n",
      pctl->ecc_offset, pctl->ecc_len));
  }

  return FLASH_PROG_SUCCESS;
} /* flash_prog_init_for_partition_image */

/*
 *
 * This function uses the RAM version of the partition tbl and erases
 * the entire MIBIB partition.  It is used only when we would have
 * erased the entire chip, but want to avoid erasing the SIM Secure
 * partition.  We must do the erase in two portions like this to preserve the
 * SIM Secure data.
 *
 */
uint32 flash_prog_erase_MIBIB_partition(flash_partable_t parti_ptr)
{
  int blockno, start_block, blk_count, erase_result_data;
  struct flash_block_vector block_vector;

  start_block = parti_ptr->part_entry[0].offset;
  blk_count = parti_ptr->part_entry[0].length;
  FLASHLOG (3, ("femp: erasing from 0x%x for  0x%x blocks\n", start_block,
    blk_count));

  for (blockno=start_block; blockno<blk_count; blockno++)
  {
    KICK_WATCHDOG();

    if (FLASH_DEVICE_DONE != flash_block_get_state(mibib.flash_handle, blockno, &blk_state))
    {
      return FLASH_PROG_FAIL;
    }

    if (blk_state == FLASH_BLOCK_BAD)
    {
      FLASHLOG (6, ("\n--- Skipping:  bad block 0x%x\n", blockno));
      continue;
    }
    else
    {
      FLASHLOG (6, ("Erasing good block 0x%x\n", blockno));
    }

    /* erase the block */
    block_vector.start_block = blockno;
    block_vector.block_count = 1;
    block_vector.result_vector = &erase_result_data;
    if (FLASH_DEVICE_DONE != flash_erase_blocks(mibib.flash_handle, &block_vector, 1))
    {
      FLASHLOG(1,("--- Error: device failed during erase of block 0x%x\n",
        blockno));

      /* Skip erase failures on block 0, which may be due to OTP protection */
      if (blockno == 0)
      {
        FLASHLOG (3, ("\nSkipping erase failure at block 0x%x\n", blockno));
        continue;
      }

      return FLASH_PROG_FAIL;
    }
  }

  return FLASH_PROG_SUCCESS;
} /* flash_prog_erase_MIBIB_partition */

/***********************************************************************
This function compares a partition enty name to a string to find a match.

**********************************************************************/

static int flash_prog_partition_name_matches(flash_partentry_t partentry,
  char *name)
{
  if (strncmp(partentry->name, name,  FLASH_PART_NAME_LENGTH) == 0)
    return TRUE;
  else
    return FALSE;
}

/***********************************************************************
  Compares two partition tables and sees if they are equal

**********************************************************************/
int flash_prog_partition_tables_are_equal (flash_partable_t parti1,
                                      flash_partable_t parti2)
{
  uint16 i;

  if ((parti1->magic1 != parti2->magic1) ||
      (parti1->magic2 != parti2->magic2) ||
      (parti1->version != parti2->version) ||
      (parti1->numparts != parti2->numparts))
  {
    return FALSE;
  }

  /* We can use number of partitions from either, since they are equal */
  for (i=0; i<parti1->numparts; i++)
  {
    FLASHLOG(4,("fptae:  Comparing %s to %s \n", (char *)
      &parti1->part_entry[i].name, (char *)&parti2->part_entry[i].name));

    /* This call may look strange, but it is correct.  The function which
     * compares names, uses a partition entry record pointer and a name
     * as arguments.  Here we have to partition tables, so we create a
     * pointer to the name only for one of them.
     */
    if (!flash_prog_partition_name_matches(&parti1->part_entry[i],
                                      parti2->part_entry[i].name))
    {
      FLASHLOG(3,("fptae:  Names do not match\n"));
      return FALSE;
    }
    if ((parti1->part_entry[i].offset != parti2->part_entry[i].offset) ||
        (parti1->part_entry[i].length != parti2->part_entry[i].length) ||
        (parti1->part_entry[i].which_flash != parti2->part_entry[i].which_flash) ||
        (parti1->part_entry[i].attrib1 != parti2->part_entry[i].attrib1) ||
        (parti1->part_entry[i].attrib2 != parti2->part_entry[i].attrib2) ||
        (parti1->part_entry[i].attrib3 != parti2->part_entry[i].attrib3))
    {
      return FALSE;
    }
  }

  /* Every field matches exactly, indicate success. */
  return TRUE;
}


/*
 * This function starts at block 0 and will find the nth good block,
 * skipping bad blocks.
 */

int flash_prog_find_nth_good_block(flash_handle_t flash_handle, int which_block)
{
  int curblk = 0;
  int goodblks = -1;
  int block_found = FALSE;
  struct flash_info flash_info;
  enum flash_block_state blk_state;

  if(DAL_SUCCESS == flash_get_info(flash_handle, FLASH_DEVICE_INFO,
    &flash_info))
  {
    FLASHLOG(3, ("- flash_get_info successful\n"));
  }
  else
  {
     FLASHLOG(3, ("- flash_get_info error\n"));
     return -1;
  }

  /* First, test the simple case of a block out or range completely */
  if (which_block > flash_info.partition.block_count)
  {
    return -1;
  }

  while ((curblk < flash_info.partition.block_count) &&
         (block_found == FALSE) && (goodblks < which_block))
  {
    if (DAL_SUCCESS != flash_block_get_state(flash_handle, curblk, &blk_state))
    {
      return -1;
    }

    if (FLASH_BLOCK_BAD == blk_state)
    {
      curblk++;
    }
    else
    {
      goodblks++;
      if (goodblks == which_block)
      {
        block_found = TRUE;
      }
      else
      {
        curblk++;
      }
    }
  }


  /* Make sure we did not go past the end of the flash */
  if (curblk >= flash_info.partition.block_count)
  {
    return -1;
  }

  return curblk;
}


/***********************************************************************
 This function accepts user defined partition table as an input and generates
 a system partition table.

**********************************************************************/

int flash_prog_convert_usr_parti_tbl_to_sys (flash_handle_t flash_handle,
                                             flash_usr_partable_t parti1,
                                             flash_partable_t parti2)
{
  uint32 i;
  uint32 partition_size;
  uint16 partition_pad;
  uint32 partition_start = 0;
  uint32 partition_limit;
  uint32 *parti_buffer = (uint32 *) parti2;
  uint32 new_block_size = 0;
  uint32 block_size = 0;
  uint32 pages_in_block;
  uint32 page_length;
  uint32 total_blocks;
  uint32 sticky_flag = FALSE;
  int sim_secure_indx = -1;
  int mibib_indx = -1;
  int kilobyte_size_flag = -1;
  struct flash_info flash_info;

  /* Sanity check that this is a valid parition table. This is not absolutely
   * necessary since we have already checked things before. Because we are
   * writing the system partition table based on the user one, it is good to
   * do this sanity check.
   */
  if ( (parti1->magic1 != FLASH_USR_PART_MAGIC1) ||
       (parti1->magic2 != FLASH_USR_PART_MAGIC2) ||
       (parti1->version != FLASH_PARTITION_VERSION) ||
       (parti1->numparts > FLASH_NUM_PART_ENTRIES) )
  {
    return FLASH_PROG_FAIL;
  }

  /* Use public APIs to determine device geometry. Check function pointers to
   * be non-NULL before using them.
   */
  FLASHLOG(3,("\nfncuptts: Using public device APIs to retrieve geometry\n"));

  if(DAL_SUCCESS == flash_get_info(flash_handle,
    FLASH_DEVICE_INFO, &flash_info))
  {
    FLASHLOG(3, ("- flash_get_info successful\n"));
  }
  else
  {
     FLASHLOG(3, ("- flash_get_info error\n"));
     return FLASH_PROG_FAIL;
  }

  page_length = flash_info.partition.page_size_bytes;
  pages_in_block = flash_info.partition.pages_per_block;
  total_blocks = flash_info.partition.block_count;


  FLASHLOG(3,("\nfncuptts: %ld %ld %ld\n", (unsigned long int)page_length,
    (unsigned long int)pages_in_block, (unsigned long int)total_blocks));
  if ( (0 == page_length) || (0 == pages_in_block) || (0 == total_blocks) )
  {
    FLASHLOG(3,("\nfncuptts: Invalid device geometry infomation\n"));
    return FLASH_PROG_FAIL;
  }

  switch (page_length)
  {
    /* Since block_size in partition table is specified in KB, we may use a
     * simple calculation */
    case (512):
    {
      block_size = pages_in_block/2;
      break;
    }

    case (2048):
    {
      block_size = pages_in_block * 2;
      break;
    }

    case (4096):
    {
      block_size = pages_in_block * 4;
      break;
    }

    case (8192):
    {
      block_size = pages_in_block * 8;
      break;
    }

    default:
    {
      FLASHLOG(1,("fncuptts: Unsupported page size encountered\n"));
      return FLASH_PROG_FAIL;
    }
  }


  FLASHLOG (4, ("\n\n"));
  FLASHLOG (4, ("Pages/Block is %ld \n", (unsigned long int)pages_in_block));
  FLASHLOG (4, ("Page size   is %ld \n", (unsigned long int)page_length));
  FLASHLOG (4, ("Block size  is %ld KB \n", (unsigned long int)block_size));

  for (i=0; i<(page_length/sizeof(uint32)); i++)
  {
    *parti_buffer = 0xFFFFFFFF;
    parti_buffer++;
  }

  parti2->magic1 = FLASH_PART_MAGIC1;
  parti2->magic2 = FLASH_PART_MAGIC2;
  parti2->version = FLASH_PARTITION_VERSION;
  parti2->numparts = parti1->numparts;

  /* Use number of partitions from user partition table*/
  for (i=0; i<parti1->numparts; i++)
  {
    /* check for 0:All partitions */
    if (strncmp(parti1->part_entry[i].name, MIBIB_DEVICE_ALL_PARTI_NAME,
        FLASH_PART_NAME_LENGTH) != 0)
    {
      (void)nand_memcpy (parti2->part_entry[i].name, parti1->part_entry[i].name,
         FLASH_PART_NAME_LENGTH);

      parti2->part_entry[i].which_flash =
                   (uint8) parti1->part_entry[i].which_flash;

      parti2->part_entry[i].attrib1=
                   parti1->part_entry[i].reserved_flag1;

      parti2->part_entry[i].attrib2=
                   parti1->part_entry[i].reserved_flag2;

      parti2->part_entry[i].attrib3=
                   parti1->part_entry[i].reserved_flag3;
    } // if strcmp partition entry with :All partition
    /* can not find :All parition */
    else
    {
      FLASHLOG(1,("fncuptts: Rsrvd partition name found in usr partiton table!\n"));
      return FLASH_PROG_FAIL;
    } // else can not find :All parition

    new_block_size = block_size;

    /* If MIBIB partition has not been seen yet, check to see if this is MIBIB.
     * If this is MIBIB check to see if this is the first partition. If it is
     * not, fail.
     */
    if (mibib_indx == -1)
    {
      if ( !strncmp(parti2->part_entry[i].name, MIBIB_PARTI_NAME,
                   FLASH_PART_NAME_LENGTH) )
      {
        FLASHLOG(3,("MIBIB partition found at index %ld in partition table\n",
          (unsigned long int)i));
        mibib_indx = i;
      }
    }

    /* If SIM_SECURE partition has not been seen yet, check to see if this is
     * SIM_SECURE. If this is SIM_SECURE check to see if this is the one right
     * after the MIBIB partition. It is is not fail.
     */
    if (sim_secure_indx == -1)
    {
      if ( !strncmp(parti2->part_entry[i].name, MIBIB_SIMSECURE_PARTI_NAME,
                   FLASH_PART_NAME_LENGTH) )
      {
        FLASHLOG(3,
          ("Sim Secure partition found at index %ld in partition table\n",
          (unsigned long int)i));

        sim_secure_indx = i;
        if ((mibib_indx != -1) && (sim_secure_indx != (mibib_indx + 1)))
        {
          FLASHLOG(1,("fncuptts: SIM_SECURE is not next to MIBIB partition\n"));
          return FLASH_PROG_FAIL;
        }
      }
    }

    FLASHLOG(3,("fncuptts: Finding bounds for partition %s\n",
      (char *)&parti2->part_entry[i].name));

    partition_size = parti1->part_entry[i].img_size;
    partition_pad = parti1->part_entry[i].padding;

    if (partition_pad == 0)
    {
      FLASHLOG(1,("fncuptts: Zero padding defined for partition %s\n",
                                          parti1->part_entry[i].name));
      FLASHLOG(1,("        Padding must be defined for all the partitions!\n"));
      return FLASH_PROG_FAIL;
    }

    if (partition_size == 0)
    {
      FLASHLOG(1,("fncuptts: Partition %s is defined with zero size\n",
                                          parti1->part_entry[i].name));
      FLASHLOG(1,("          and non-zero padding. Not a valid definition!\n"));
      return FLASH_PROG_FAIL;
    }

    if ((uint32) partition_pad > partition_size)
    {
      FLASHLOG(1,("fncuptts: Partition %s is defined with padding larger\n",
                                          parti1->part_entry[i].name));
      FLASHLOG(1,("          than the partition itself. Not Allowed!\n"));
      return FLASH_PROG_FAIL;
    }

    /* Since there is no assigned image for some partitions, partition size
     * will not be know readily. To make it easier for the user, the first few
     * partitions can be specified in terms of number of blocks.
     */

    /* For now this is only allowed for MIBIB and SIM_SECURE partitions.*/
    if( !( parti1->part_entry[i].reserved_flag4 & ~(FLASH_PART_ATTR_BASE &
        FLASH_PART_HEX_SIZE) ) )
    {
      if (kilobyte_size_flag == -1)
      {
        parti2->part_entry[i].offset = partition_start;
        partition_limit = partition_size + partition_pad;
        parti2->part_entry[i].length = partition_limit;

        FLASHLOG(3,("fncuptts: Partition start at block %ld\n" ,
          (unsigned long int)parti2->part_entry[i].offset));
        FLASHLOG(3,("fncuptts: Partition length in blocks %ld\n" ,
          (unsigned long int)parti2->part_entry[i].length));

        partition_start += partition_limit;
        partition_limit = 0;
        continue;

      }
      else
      {
        FLASHLOG(1,("fncuptts: Hex size used after using KB sizes in table\n"));
        return FLASH_PROG_FAIL;
      }
    }

    kilobyte_size_flag = 0;

    FLASHLOG(3, ("fncuptts: Image Size is %ld KB\n",
      (unsigned long int)partition_size));
    FLASHLOG(3, ("fncuptts: Image Pad is %d KB\n", partition_pad));

    /* Check if 2X mode is enabled for this partition */
    if (FLASH_PART_ATTRIB(parti1->part_entry[i].reserved_flag1,
      FLASH_PART_ATTRIBUTE1) == (uint32)FLASH_PARTITION_SLC_READWRITE_2X)
    {
      /* Make sure the partition starts from an even block
       * so that the block_count passed for cefs generation
       * can be halfed for EFS2APPS partition
       */
      if (partition_start & 0x1)
      {
        partition_start++;
      }
    }

    if ( (partition_size == FLASH_PARTITION_GROW) &&
         (partition_pad == (uint16)FLASH_PARTITION_GROW) )
    {
      parti2->part_entry[i].offset = partition_start;
      parti2->part_entry[i].length = (total_blocks - partition_start);
      if (i != (parti1->numparts - 1))
      {
        FLASHLOG(1,("fncuptts: Non-terminal partition defined with GROW \n"));
        return FLASH_PROG_FAIL;
      }

      FLASHLOG(3,("fncuptts: Partition start at block %ld\n" ,
        (unsigned long int)parti2->part_entry[i].offset));
      FLASHLOG(3,("fncuptts: Partition length in blocks %ld\n" ,
        (unsigned long int)parti2->part_entry[i].length));

      return FLASH_PROG_SUCCESS;
    }

    partition_limit = 0;

    if (partition_size != 0)
    {
      partition_limit = partition_size / new_block_size;
      if (partition_size % new_block_size)
      {
        partition_limit++;
      }
    }
    FLASHLOG(4,("fncuptts: Partition lenght is %ld blocks\n",
      (unsigned long int)partition_limit));


    /*         ===============  DESIGN NOTE  ====================
     *
     * sticky flag is used here to bring a notion of either / or approach to
     * defining partition tables. If even a single partition in the partition
     * table is defined with non-zero pad size then all partitions will be laid
     * out statically without actually checking the flash device for existence
     * of bad blocks. As of now, all the partition with KB size have to be
     * defined with zero padding for run-time checking of bad blocks.
     */

    if ((partition_pad > 0) || (sticky_flag == TRUE))
    /* Static partition layout; no bad_block determintation */
    {
      partition_limit += partition_pad / new_block_size;
      if (partition_pad % new_block_size)
      {
        partition_limit++;
      }

      sticky_flag = TRUE;
    }
    else
    /* Runtime partition layout; conpensate for bad blocks */
    {
      FLASHLOG(4,("fncuptts: Running bad block check\n"));
      for (i=partition_start; i<partition_limit; i++)
      {
        if (DAL_SUCCESS != flash_block_get_state(flash_handle, i, &blk_state))
        {
          return FLASH_PROG_FAIL;
        }

        if(blk_state == FLASH_BLOCK_BAD)

        {
          partition_limit++;
        }
      }//for
    }//if

    FLASHLOG(4,("fncuptts: Partition start is %ld blocks\n",
      (unsigned long int)partition_start));
    FLASHLOG(4,("fncuptts: Partition lenght is %ld blocks\n",
      (unsigned long int)partition_limit));

    parti2->part_entry[i].offset = partition_start;
    parti2->part_entry[i].length = partition_limit;
    partition_start += partition_limit;


    FLASHLOG(3,("fncuptts: Partition start at block %ld\n" ,
      (unsigned long int)parti2->part_entry[i].offset));
    FLASHLOG(3,("fncuptts: Partition length in blocks %ld\n" ,
      (unsigned long int)parti2->part_entry[i].length));

  } //for

  return FLASH_PROG_SUCCESS;

}
