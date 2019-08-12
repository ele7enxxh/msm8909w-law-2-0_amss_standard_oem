/*=======================================================================
 * FILE:        nand_sim.c
 *
 * SERVICES:    Exports the APIs for NAND programmer.
 *
 * GENERAL DESCRIPTION
 *   Exports the APIs for NAND programmer.
 *
 * Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_sim.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 05/07/09     eo      Clean up gcc related compiler warnings
 * 02/27/09     jz      Updated to support SB2.0
 * 01/10/09     mm      Cleanup of IOVEC updates
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "nand_core.h"
#include "flash_prog.h"
#include "nand_param.h"
#include "nand_wrapper.h"
#include "nand_sim.h"


/* Allocate one extra block beyond what is required for essential MIBIB blocks
 * to represent rest of the device. This is required for the the init routines
 * that prep the main image area by erasing the first good block in it. */
unsigned char ram_blocks[NAND_RAM_DEV_SIZE][MAX_BLOCK_SIZE][TOTAL_PAGE_SIZE];


/*===========================================================================

  This function copy the source char string to destination string using nand_memcpy

===========================================================================*/
void flash_sim_strcpy(char *dir_name, char *filepath)
{
    int len = strlen(filepath);
    nand_memcpy(dir_name, filepath, len);
    dir_name[len] ='\0';
    //printf ("\n %s, %s \n", dir_name, filepath);
}


/*===========================================================================

  This function make a full file path name based on file path and file name

===========================================================================*/
void flash_sim_makepath(char *dir_name, char *filepath, char *filename)
{
    int len;
    int len1;

    len= strlen(filepath);
    nand_memcpy(dir_name, filepath, len);
    dir_name[len] = '/';
    len1 = strlen(filename);
    nand_memcpy(&dir_name[len+1], filename, len1);
    dir_name[len+len1+1]='\0';
    //printf ("\n%s, %s, %s \n", dir_name, filepath, filename);
}


/*===========================================================================

  This function checks blocks in simulated flash devices to be bad. Depending
  on the device context it gets called in it reports the blocks to be good if
  they are in the correct range.

  MIBIB device descriptor has access to the block from 0 to MI_BLOCK_LAST,
  where as IMAGE device descriptor has access only to (MI_BLOCK_LAST + 1)
  that represents rest of the device.

===========================================================================*/

int flash_block_get_state(flash_handle_t self,
  uint32 block, enum flash_block_state *block_state)
{
  uint32 threshold;

  if (block >= (MI_BLOCK_LAST))
  {
    return FLASH_DEVICE_FAIL;
  }

  if (self == mibib.flash_handle)
  {
    threshold = 0;
  }
  else if (self == image.flash_handle)
  {
    threshold = MI_BLOCK_LAST;
  }
  else
  {
    //printf("\njdie: Unknown device context 0x%x\n", (uint32) self);
    return FLASH_DEVICE_FAIL;
  }

  //printf("\njdbbc: Checking blk %d thold %d\n", (int) block, (int) threshold);
  //printf("\njdbbc: Checking bad_blk flag in RAM flash blk %d\n", (int) block);

  if (block >= threshold)
  {
    return FLASH_DEVICE_DONE;
  }

  return FLASH_BLOCK_BAD;
}


/*===========================================================================

  This function erases a block on the simulated flash device in RAM.

===========================================================================*/
int flash_erase_blocks(flash_handle_t handle,
   struct flash_block_vector *blk_vectors, uint32 uBlkCount)
{

  uint32 total_page_size = mibib.flash_info.partition.total_page_size_bytes;
  uint32 pages_in_blk = mibib.flash_info.partition.pages_per_block;
  uint32 i;

  if (blk_vectors->start_block == 0)
  {
    printf(" ********Oth block Erased*******");

  }

  if (blk_vectors->start_block >= NAND_RAM_DEV_SIZE)
  {
    return FLASH_DEVICE_FAIL;
  }
  else
  {
    printf("\njdeb: Erasing simulated flash block %d\n", (int) blk_vectors->start_block);

    for (i = 0; i < pages_in_blk; i++)
    {
      memset(&ram_blocks[blk_vectors->start_block][i][0], 0xFF, total_page_size);
    }
  }

  return FLASH_DEVICE_DONE;
}


/*===========================================================================

  This function reads a page from simulated flash block in RAM.

===========================================================================*/

int flash_prog_read_page(flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 read_type, void *buffer)
{
  unsigned char *page_start;
  uint32 page_size = mibib.flash_info.partition.page_size_bytes;
  uint32 pages_in_blk = mibib.flash_info.partition.pages_per_block;
  uint32 page_offset_in_blk;
  uint32 blk_offset;
  uint32 total_page_size = mibib.flash_info.partition.total_page_size_bytes;
  uint32 i;

  blk_offset = page / pages_in_blk;
  page_offset_in_blk = page - pages_in_blk * blk_offset;

  if ( (page_offset_in_blk >= MAX_BLOCK_SIZE) ||
       (blk_offset >= NAND_RAM_DEV_SIZE) )
  {
    return FLASH_DEVICE_FAIL;
  }
  else
  {
    //printf("\njdrp: Reading page %d from RAM flash device.\n", (int) page);

    page_start = &(ram_blocks[blk_offset][page_offset_in_blk][0]);
    for (i = 0; i < (total_page_size/sizeof(uint32)); i++)
    {
      if (page_start[i] != 0xFF)
      {
        //printf("\njdie: Page %d is not erased.\n", (int) page);
        nand_memcpy((unsigned char *)buffer, page_start, page_size);
        return FLASH_DEVICE_DONE;
      }
    }
    return FLASH_DEVICE_FAIL_PAGE_ERASED;
  }
}


/*===========================================================================

  This function writes a page to simulated flash block in RAM.
===========================================================================*/

int flash_prog_write_page(flash_handle_t handle, struct flash_info *dev_info,
   uint32 page, uint32 write_type, void *buffer)
{
  unsigned char *page_start;
  uint32 page_size = mibib.flash_info.partition.page_size_bytes;
  uint32 pages_in_blk = mibib.flash_info.partition.pages_per_block;
  uint32 page_offset_in_blk;
  uint32 blk_offset;

  blk_offset = page / pages_in_blk;
  page_offset_in_blk = page - pages_in_blk * blk_offset;


  if( (blk_offset == 2) && (page_offset_in_blk == 4))
  {
    printf(" ********Oth block Written*******");

  }
  if ( (page_offset_in_blk >= MAX_BLOCK_SIZE) ||
       (blk_offset >= NAND_RAM_DEV_SIZE) )
  {
    return FLASH_DEVICE_FAIL;
  }
  else
  {
    //printf("\njdwp: Writing page %d to RAM flash device.\n", (int) page);
    page_start = &(ram_blocks[blk_offset][page_offset_in_blk][0]);

    nand_memcpy(page_start, buffer, page_size);

    /* Set the flag in spare area to indicate that this page is written */
    page_start[page_size] = (char) NAND_MIBIB_MBN_PAGE_WRITTEN_FLAG;

    return FLASH_DEVICE_DONE;
  }

  return FLASH_DEVICE_DONE;
}

int flash_prog_write_page_and_udata (flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 write_type, void *buffer, void *spare_udata,
  uint32 spare_udata_blen)
{
  return FLASH_DEVICE_DONE;
}

int flash_prog_read_page_and_udata (flash_handle_t handle, struct flash_info *dev_info,
  uint32 page, uint32 read_type, void *buffer, void *spare_udata,
  uint32 spare_udata_blen)
{
  return FLASH_DEVICE_DONE;
}
/*===========================================================================

  This function returns block size of simulated flash device in number of pages.

===========================================================================*/
int flash_get_info(flash_handle_t handle,
  enum flash_info_type info_type, void *pinfo)
{
  struct flash_info *info = (struct flash_info *) pinfo;

  info->mibib_info.new_mibib_block = -1;
  info->mibib_info.old_mibib_block = -1;
  info->partition.page_size_bytes =
    mibib.flash_info.partition.page_size_bytes;
  info->partition.total_page_size_bytes =
    mibib.flash_info.partition.total_page_size_bytes;
  info->partition.pages_per_block =
    mibib.flash_info.partition.pages_per_block;
  info->partition.block_count =
    mibib.flash_info.partition.block_count;

  return DAL_SUCCESS;
}


int flash_open_partition(flash_handle_t handle,
  const unsigned char *partition_name)
{
  return DAL_SUCCESS;
}


int
DAL_DeviceAttach(DALDEVICEID DeviceId,
                 DalDeviceHandle **phDevice)
{
  *phDevice = (DalDeviceHandle *)&ram_blocks;
  return DAL_SUCCESS;
}

int
DAL_DeviceDetach(DalDeviceHandle *hDevice)
{
  return DAL_SUCCESS;
}

void
DALSYS_InitMod(DALSYSConfig * pCfg)
{
}

void term_put(char c)
{
  printf("%c", c);
}

/*
 * Wrapper for memcpy API. Introduced as a
 * Workaround for unaligned memcpy failure with RVCT 4.0
 */
void* nand_memcpy(void *out, const void *in, uint32 n)
{
  memcpy(out, in, n);
  return out;
}

