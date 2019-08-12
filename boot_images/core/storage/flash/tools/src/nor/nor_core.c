/*=======================================================================
* FILE:        nor_core.c
*
* SERVICES:    Flash Programming Plug-In support
*
* DESCRIPTION :
*   This file has the main functions used for programming the flash
*
* Copyright (c) 2010-2011, 2013, 2015 Qualcomm Technologies Incorporated.
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
*   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_core.c#1 $ 
*   $DateTime: 2015/09/01 00:30:35 $ 
*   $Author: pwbldsvc $
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 01/28/15     sb      Remove KW Banned Functions
* 12/17/13     whc     Add RF EEPROM I2C support
* 07/20/11     sv      Use compile flag to decide SPI/I2C NOR tools
* 07/07/11     jz      Fixed NOR tools programming issues
* 04/06/11     jz      Cleaned up flash_prog_write_data
* 03/25/11     jz      Added busywait/clk_pause in NOR tools
* 10/22/10     bn      Added check for flash unsupported API
* 09/10/10     sv      Initial version
*==================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "nor_core.h"
#include "nor_wrapper.h"
#include "stringl/stringl.h"

struct nor_program_info  nor_prog_info;
struct flash_block_vector block_vector;
/*===========================================================================

                           DEFINITIONS

===========================================================================*/

/* This structure is used to communicate between on target programmer and
 * TRACE32. */
extern struct flash_prog_param prog_param;

#if defined(BUILD_NOR_TOOLS)
  /* Main stack */
  uint32 svc_stack[0x1000/sizeof(uint32)];
#endif

static int erase_result_data;

ALIGN(ALIGN_VAL) unsigned char page_buf[MAX_PAGE_SIZE]; 

/*
 * This function is a wrapper for the real flash write routine. It extracts
 * the required parameters from TRACE32.
 */
uint32 flash_prog_program_img(void)
{
  uint8 *data;
  uint32 addr, size;
  uint32 start_blk = 0, end_blk = 0;
  uint32 roof_blk = nor_prog_info.nor_info.partition.block_count;
  uint32 tmp_addr;
  int32 index = 0;
  struct flash_block_vector block_vector;
  int ret_val = FLASH_PROG_SUCCESS;
  int status = FLASH_DEVICE_FAIL;
 

  data = prog_param.data + prog_param.offset;
  addr = prog_param.addr + prog_param.offset - \
         prog_param.flashaddr;
  size = prog_param.size;
   
  FLASHLOG (4, ("fpgm: jata 0x%x jaddr 0x%x joff 0x%x\n", prog_param.data,
                prog_param.addr, prog_param.offset));
  FLASHLOG (3, ("fpgm: data 0x%x addr 0x%x len 0x%x\n", data, addr,
                 size));

  KICK_WATCHDOG();
  CHECK_FOR_DATA();     /* Just to be sure */
 
  /* bail out if zero length write */
  if (size == 0)
  {
    FLASHLOG (3, ("fpgm: Invalid size '0' provided by T32 \n"));
    ret_val = FLASH_PROG_FAIL;   
    goto exitfunction;
  }

  if (addr >= nor_prog_info.partition_size)
  {
    FLASHLOG (3, ("fpgm: Addr provided by T32 is beyond device limit\n"));
    ret_val = FLASH_PROG_FAIL;
    goto exitfunction;
  }

  /* Determine which sector this write is supposed to be in.
   * It may begin in a sector which is already erased, in which
   * case, we want to bump to the next block.
   */
  tmp_addr = 0;
  while (start_blk < roof_blk)
  {
    tmp_addr += nor_prog_info.parti_block_info[start_blk].len;
    if (tmp_addr > addr)
      break;
    start_blk++;
  }
 
  if (start_blk == roof_blk)
  {
    FLASHLOG (3, ("fpgm: Start addr provided by T32 is beyond device limit\n"));
    ret_val = FLASH_PROG_FAIL;
    goto exitfunction;
  }

  FLASHLOG (3, ("fpgm: block st 0x%x\n", start_blk));
 
  end_blk = start_blk;
  while (end_blk < roof_blk)
  {
    if ((addr + size - 1) < tmp_addr)
      break;
    tmp_addr += nor_prog_info.parti_block_info[end_blk].len;
    end_blk++;
  }
 
  KICK_WATCHDOG();
  CHECK_FOR_DATA ();
 
  if (end_blk == roof_blk)
  {
    FLASHLOG (3, ("fpgm: End addr provided by T32 is beyond device limit\n"));
    ret_val = FLASH_PROG_FAIL;
    goto exitfunction;
  }

  FLASHLOG (3, ("fpgm: block end 0x%x\n", end_blk));

  /* Call erase function on blocks to erase */
  for (index = start_blk; index <= end_blk; index++)
  {
    if (nor_prog_info.parti_block_info[index].is_erased == FALSE)
    {
      FLASHLOG (5, ("fpgm: Erasing block 0x%x\n", nor_prog_info.parti_block_info[index].start));
      block_vector.start_block = index; 
      block_vector.block_count = 1;
      block_vector.result_vector = &erase_result_data;
      status = flash_erase_blocks(nor_prog_info.handle, &block_vector, 1);

      if ((status != FLASH_DEVICE_DONE) && (status != FLASH_DEVICE_NOT_SUPPORTED))
      {
        SET_ERR (ERR_ERASE_FAIL);
        ret_val = FLASH_PROG_FAIL;
        goto exitfunction;
      }
      else
      {
        nor_prog_info.parti_block_info[index].is_erased = TRUE;
      }
    }
  }

  ret_val = flash_prog_write_data(addr, size, data);

exitfunction:
  return ret_val;

} /* flash_program_img */

/*
 * This function fills flash device geometry data in a static structure
 * which is used by the other routines.
 */
uint32 flash_prog_update_device_info(void)
{
  uint32 ret_val = FLASH_PROG_SUCCESS;
  uint16 region_index;
  uint32 block_index, curraddr = 0;
  int32 block_cnt = 0;
  uint32 parti_info_indx  = 0;
  uint32 partition_block_cnt = 0;
  struct flash_nor_erase_region_info *nor_region_info;

  if (nor_prog_info.handle != NULL)
  {
    if (DAL_SUCCESS != flash_get_info(nor_prog_info.handle, FLASH_DEVICE_INFO, 
         &nor_prog_info.nor_info))
    {
      ret_val = FLASH_PROG_FAIL;
    }
    else if (DAL_SUCCESS != flash_get_info(nor_prog_info.handle, 
              FLASH_NOR_ERASE_REGION_INFO, &nor_prog_info.erase_region_info))
    {
       ret_val = FLASH_PROG_FAIL;
    }
    else if (DAL_SUCCESS != flash_get_info(nor_prog_info.handle, 
              FLASH_NOR_BLOCK_ERASE_CFG_INFO, &nor_prog_info.erase_cfg_info))
    {
       ret_val = FLASH_PROG_FAIL;
    }    
  }  
  else
  {   
    ret_val = FLASH_PROG_FAIL;
  }

  /* 
   * Iterate through the geometry structure to fill the partition block info
   * structure.  Outer loop index keeps track of erase block regions 
   * and the inner loop index keeps track of the number of blocks in 
   * each region.
   */ 
  nor_region_info =  &nor_prog_info.erase_region_info;
  nor_prog_info.device_size = 0;
  partition_block_cnt = 0;
  for (region_index = 0; 
       region_index < nor_region_info->erase_region_count;
       region_index++)
  {
    nor_prog_info.device_size +=  
      nor_region_info->erase_region[region_index].block_count *
      nor_region_info->erase_region[region_index].block_size_in_bytes;

    for (block_index = 0; block_index < 
         nor_region_info->erase_region[region_index].block_count; 
         block_index++, block_cnt++)
    {

      if (block_cnt < nor_prog_info.nor_info.partition.physical_start_block)
      {
        continue;
      }   
      if (block_cnt >= (nor_prog_info.nor_info.partition.physical_start_block + 
          nor_prog_info.nor_info.partition.block_count))
      {
        break;
      }
      partition_block_cnt++;
      nor_prog_info.parti_block_info[parti_info_indx].start = curraddr;
      nor_prog_info.parti_block_info[parti_info_indx].len = 
        nor_region_info->erase_region[region_index].block_size_in_bytes;
      nor_prog_info.parti_block_info[parti_info_indx].is_erased = FALSE;             
      curraddr += nor_prog_info.parti_block_info[parti_info_indx].len;
      nor_prog_info.partition_size += 
        nor_prog_info.parti_block_info[parti_info_indx].len; 
      parti_info_indx++;
      KICK_WATCHDOG();
      CHECK_FOR_DATA ();

    }        
  } /* for (region_index = 0 ... */
  
  if( nor_prog_info.nor_info.partition.block_count != partition_block_cnt)
  {
    FLASHLOG(3, ("Error in block info filling\n"));
    ret_val = FLASH_PROG_FAIL;
  }
 
  return ret_val;
}  /* flash_prog_update_device_info */

/*
 * This function opens the device, then the named partition and sets up
 * device structure to use that partition.
 */
uint32 flash_prog_init_for_partition_image (unsigned char *pname)
{
  KICK_WATCHDOG();
  FLASHLOG(3,("ifpi:  trying to find partition %s\n", pname));

  KICK_WATCHDOG();

  if (nor_prog_info.handle == NULL)
  {
     if(DAL_SUCCESS != flash_device_attach(dev_id, 
       &nor_prog_info.handle))                 
     {
        FLASHLOG(3, ("Error attaching to device driver \n"));
        SET_ERR (ERR_DEV_MISSING); 
        return FLASH_PROG_FAIL;
     }
  }

  if (FLASH_DEVICE_DONE == flash_open_partition (nor_prog_info.handle, pname))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
     FLASHLOG(3, ("flash_open_partition error\n"));
     SET_ERR (ERR_WRITE_FAIL);
     return FLASH_PROG_FAIL;
  }

  if(flash_prog_update_device_info() == FLASH_PROG_FAIL)
  {
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;  
  }  
  
  FLASHLOG(3,("Found partition %s\n", pname));

  return FLASH_PROG_SUCCESS;

} /* flash_prog_init_for_partition_image */


/*
 * This function is a wrapper for the real flash erase chip routine. It extracts
 * the required parameters from TRACE32 and passes it onto the real thing.
 */
uint32 flash_prog_erase_chip(void)
{
  uint16 index;
  int ret_val = FLASH_PROG_SUCCESS;
  int status = FLASH_DEVICE_DONE;

  if (flash_prog_init_for_partition_image((unsigned char*)FLASH_DEVICE_ALL_PARTI_NAME) !=
    FLASH_PROG_SUCCESS)
  {
    FLASHLOG (3,("Error: no nor_device found \n"));
    SET_ERR (ERR_DEV_MISSING);
    ret_val = FLASH_PROG_FAIL;
  }
  else
  {
    /* Erase every block in the flash, one at a time */
    for (index = 0; index < nor_prog_info.nor_info.partition.block_count; 
         index++)
    {
      KICK_WATCHDOG();
 
      FLASHLOG (3, ("jfec: block 0x%x\n", index));

      block_vector.start_block = index; 
      block_vector.block_count = 1;
      block_vector.result_vector = &erase_result_data;
      status = flash_erase_blocks(nor_prog_info.handle, &block_vector, 1);

      if (status != FLASH_DEVICE_DONE)
      {
        SET_ERR (ERR_ERASE_FAIL);
        ret_val = FLASH_PROG_FAIL;
        break;
      }
    }  
  }
  return ret_val;
}  /* flash_prog_erase_chip */

/*
 * This function programs the data to the NOR flash 
 * */
uint32 flash_prog_write_data(uint32 addr, uint32 size, uint8 *data_buf)
{
  uint32 ret_val = FLASH_PROG_SUCCESS;
  uint32 num_pages = 0;
  uint32 bytes_written = 0, offset;
  uint32 page, num_bytes_to_write, start_page;
  uint32 page_size = 0;
  int status;

  /* Compute and init write data for the count pages */
  num_pages = size / nor_prog_info.nor_info.partition.page_size_bytes;
  start_page = addr / nor_prog_info.nor_info.partition.page_size_bytes;
  page_size = nor_prog_info.nor_info.partition.page_size_bytes;
    offset = addr & (nor_prog_info.nor_info.partition.page_size_bytes - 1);
    
  if (size >= page_size)
  {
    status = flash_prog_write_pages(nor_prog_info.handle, 
      &nor_prog_info.nor_info, start_page, num_pages, data_buf);
 
    if (FLASH_DEVICE_DONE != status)
    {
      SET_ERR (ERR_WRITE_FAIL);
      ret_val = FLASH_PROG_FAIL;
      FLASHLOG(3, ("flash_prog_write_pages failed for page start %d \n", \
        start_page));
      return ret_val;
    }
    bytes_written = num_pages * page_size;
  }
  
  if (size > bytes_written)
  {
    page = start_page + num_pages;

    num_bytes_to_write = size - bytes_written;  

    /* Note: when the image size is not a multiple of 4 bytes,
     * T32 might call the programming function the second time
     * for the last few bytes (< 4 bytes)
     */
    if (offset != 0)
    {
      /* We're called the second time, need to handle
       * partial page programming by reading back the page last programmed
       */
      status = flash_prog_read_pages(nor_prog_info.handle,
        &nor_prog_info.nor_info, page, 1, page_buf);

      if (FLASH_DEVICE_DONE != status)
      {
        SET_ERR (ERR_WRITE_FAIL);
        ret_val = FLASH_PROG_FAIL;
        FLASHLOG(3, ("flash_prog_read_pages failed for page %d \n", page));
        return ret_val;
      }
    }
    else
    {
      DALSYS_memset(page_buf, 0xFF, page_size);
    }

    /* Copy/Append data to the right location of the page_buf */
    DALSYS_memcpy(&page_buf[offset], &data_buf[bytes_written],
      num_bytes_to_write);

    status = flash_prog_write_pages(nor_prog_info.handle, 
      &nor_prog_info.nor_info, page, 1, page_buf);

    if (FLASH_DEVICE_DONE != status)
    {
      SET_ERR (ERR_WRITE_FAIL);
      ret_val = FLASH_PROG_FAIL;
      FLASHLOG(3, ("flash_prog_write_pages failed for page %d \n", page));
    }
  }              
   
  return ret_val;
}

/*
 * This function provides the flash parameters for the NOR flash
 */
uint32 flash_prog_get_flash_info()
{
  uint32 ret_status = FLASH_PROG_SUCCESS;
  uint32 block_cnt, block_cnt_shift;
  static struct flash_parameter flash_param;
  uint32 status;
  /* Open the partition to use for this image */ 
  status = flash_prog_init_for_partition_image (
             (unsigned char *)FLASH_DEVICE_ALL_PARTI_NAME);

  KICK_WATCHDOG();

  if ((status == FLASH_PROG_SUCCESS) && (nor_prog_info.handle != NULL))
  {
    memscpy(flash_param.flash_name, FLASH_PROG_NAME_SIZE,
      (char *)nor_prog_info.nor_info.device_name, FLASH_PROG_NAME_SIZE); 
    block_cnt_shift = 0;
    block_cnt =  nor_prog_info.nor_info.partition.block_count;
    while(block_cnt > 1)
    {
      block_cnt >>= 1;
      block_cnt_shift++;
    } 
    block_cnt <<= block_cnt_shift;
    flash_param.num_blocks = block_cnt;
    flash_param.page_size = nor_prog_info.nor_info.partition.page_size_bytes;
    flash_param.pages_in_block =  
      (nor_prog_info.device_size / flash_param.num_blocks) / 
      (flash_param.page_size);
    prog_param.reserved = (uint32)&flash_param;
  }
  else
  {
    ret_status = FLASH_PROG_FAIL;
  }
  return ret_status;

} /* flash_get_flash_info */

/*
 * Delay in increments of this subroutine's execution time.
 */
void mdelay (uint32 delay)
{
  register uint32 delvar = delay;
  while (delvar)
  {
    delvar--;
    delvar++;
    delvar--;
    KICK_WATCHDOG();
  }
}
#ifndef BUILD_BOOT_CHAIN
void clk_pause (int microseconds)
{
  mdelay(100*microseconds);
}

void busywait (int microseconds)
{
  mdelay(100*microseconds);
}
#endif
