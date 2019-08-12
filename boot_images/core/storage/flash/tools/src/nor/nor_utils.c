/*=======================================================================
 * FILE:        nor_utils.c
 *
 * SERVICES:    Various utility functions used in NOR.
 *
 * GENERAL DESCRIPTION
 *    Various utility functions used in NOR.
 *
 * Copyright (c) 2010-2013, 2015 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_utils.c#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ 
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/28/15     sb      Remove KW Banned Functions
 * 12/17/13     whc     Add RF EEPROM I2C support
 * 01/29/13     sv      Fix Klocwork error
 * 02/28/12     sv      Fix Klocwork error
 * 07/20/11     sv      Use compile flag to decide SPI/I2C NOR tools
 * 04/06/11     jz      Added support for reading platform ID from EEPROM
 * 08/17/10     sv      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALDeviceId.h"
#include "nor_wrapper.h"
#include "flash_prog.h"
#include "flash_log.h"
#include "nor_param.h"
#include "nor_config.h"
#include "stringl/stringl.h"

/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                           LOCAL DATA

===========================================================================*/

/* Index to the Platform ID info in CDT (Config Data Table) */
#define FLASH_CDT_PLATFORM_ID_IDX  0xE
/* Magic number for identifying CDT programmed in EEPROM */
#define FLASH_CDT_MAGIC_NUMBER     0x00544443

/* NOR device structure */
static flash_handle_t handle_flash;

static struct flash_info dev_info;
enum flash_block_state block_state;
static struct flash_block_vector block_vector;

static uint8 *page_buffer;
static uint8 *page_readback_buffer;

/* NOR device parameters */
static uint32 total_block_count;  /* number of erasable units in flash */
static uint32 page_size_bytes;    /* number of bytes in a page         */
static uint32 total_pages;        /* number of pages in the device     */
static char device_name[32];
static DALBOOL dev_is_initialized = FALSE;
static unsigned char parti_name[20];
static int erase_result = 0;
DALDEVICEID dev_id = FLASH_NOR_DEVICE_ID;

/*
 * Safe string copy function for flash utils
 */
static void nor_utils_strcpy(uint8 *dest, const uint8 *src, uint32 len)
{
  memscpy(dest, len, src, len);
  dest[len-1] ='\0';
}

static void flash_utils_deinit(void)
{
  if (dev_is_initialized )
  {
    DALSYS_Free(page_buffer);
    DALSYS_Free(page_readback_buffer);
    flash_device_close((DalDeviceHandle*)handle_flash);
    flash_device_detach((DalDeviceHandle*)handle_flash);
    dev_is_initialized = FALSE;
  }
}

static uint32 flash_utils_init(void)
{
  int status;
  uint32 result = FLASH_PROG_SUCCESS;
  struct flash_nor_erase_region_info erase_region_info;
  uint32 block_size_bytes;
  uint32 pages_per_block;  
  uint32 block_count;  
  uint32 index;

  if (dev_is_initialized)
  {
    return FLASH_PROG_SUCCESS;
  }

  nor_utils_strcpy((uint8 *)&parti_name, "0:ALL", 
	  sizeof("0:ALL"));

  if(DAL_SUCCESS != flash_device_attach(dev_id,
    &handle_flash))                 
  {
    FLASHLOG(3, ("Error attaching to device driver \n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  FLASHLOG(3, ("Device+Client Attached!\n"));

  if(DAL_SUCCESS == flash_open_partition(handle_flash, parti_name))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    SET_ERR (ERR_UNKNWN_PARTI);
    result = FLASH_PROG_FAIL;
    goto detach;
  }

   status = flash_get_info(handle_flash, FLASH_DEVICE_INFO, &dev_info);

  if(DAL_SUCCESS != status)
  {
    FLASHLOG(3, ("- flash_get_info error\n"));
    SET_ERR (ERR_DEV_MISSING);
    result = FLASH_PROG_FAIL;
    goto close_and_detach;
  }

  /* Get some information about the device */
  total_block_count = dev_info.partition.block_count;
  page_size_bytes = dev_info.partition.page_size_bytes;

  nor_utils_strcpy((uint8 *)&device_name, dev_info.device_name, 
    sizeof(device_name));

  FLASHLOG(1, ("Found %s\n", device_name));

  total_pages = 0;

  status = flash_get_info(handle_flash, FLASH_NOR_ERASE_REGION_INFO, 
    &erase_region_info);
  
  if(DAL_SUCCESS == status)
  {
    FLASHLOG(3, ("Erase Region Count = %d \n", \
      erase_region_info.erase_region_count));
    
    for(index = 0; index < erase_region_info.erase_region_count; index++)
    {
      block_size_bytes = 
        erase_region_info.erase_region[index].block_size_in_bytes;
      block_count = 
        erase_region_info.erase_region[index].block_count;
      pages_per_block =  block_size_bytes / page_size_bytes;

      FLASHLOG(3, ("Region %d block count = %d \n", index, block_count));
      FLASHLOG(3, ("Region %d block size = %d \n", index, block_size_bytes));
      FLASHLOG(3, ("Region %d pages per block = %d \n", index, 
        pages_per_block));
      total_pages += pages_per_block * block_count;
    }
  }
  else
  {
    FLASHLOG(3, ("- dal_flash_GetInfo(FLASH_NOR_ERASE_REGION_INFO) error\n"));
    SET_ERR (ERR_DEV_MISSING);
    result = FLASH_PROG_FAIL;
    goto close_and_detach;
  }

  FLASHLOG(1, ("Total Block count is %d \n", total_block_count));
  FLASHLOG(1, ("Page size  is %d \n", page_size_bytes));
  FLASHLOG(1, ("Total Pages Avail is %d \n", total_pages));


  if (DAL_SUCCESS != DALSYS_Malloc(page_size_bytes,
    (void **)&page_buffer))
  {
    goto close_and_detach;
  }

  if (DAL_SUCCESS != DALSYS_Malloc(page_size_bytes,
    (void **)&page_readback_buffer))
  {
    DALSYS_Free(page_buffer);
    goto close_and_detach;
  }

  dev_is_initialized = TRUE;

  return result;

close_and_detach:
   flash_device_close((DalDeviceHandle*)handle_flash);

detach:
   flash_device_detach((DalDeviceHandle*)handle_flash);
   return result;
}


/*===========================================================================
  This function erases a block by calling flash driver function
===========================================================================*/

uint32 flash_utils_erase_block(uint32 block)
{
  uint32 result = FLASH_PROG_SUCCESS;

  result = flash_utils_init();

  block_vector.block_count = 1;
  block_vector.result_vector = &erase_result;

  if (FLASH_PROG_SUCCESS == result)
  {
    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Erasing Block 0x%x\n", block));

    /* Ensure block is in range */
    if (block >= total_block_count)
    {
      FLASHLOG(1, ("- Block 0x%x out of range \n", block));
      SET_ERR(ERR_PAST_BLOCK_END);
      return FLASH_PROG_FAIL;
    }

    block_vector.start_block = block;
    
    if (FLASH_DEVICE_DONE != flash_erase_blocks(handle_flash, &block_vector, 1))
    {
      FLASHLOG(3, ("- Erase Failed\n"));
      SET_ERR(ERR_ERASE_FAIL);
      result = FLASH_PROG_FAIL;
      flash_utils_deinit();
    }
    else
    {
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }

  return result;
} // flash_utils_erase_block



/*===========================================================================
  This function will read a page from the NOR flash
===========================================================================*/
uint32 flash_utils_read_page(void)
{
  uint32 page;
  uint32 result = FLASH_PROG_SUCCESS;
  int op_result = FLASH_DEVICE_DONE;

  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    page = prog_param.reserved;

    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Reading Logical Page 0x%x \n", page));

    memset(page_buffer, 0x00, page_size_bytes);

    op_result = flash_prog_read_pages(handle_flash, &dev_info,
      page, 1, page_buffer);

    if ((FLASH_DEVICE_DONE != op_result) &&
        (FLASH_DEVICE_FAIL_PAGE_ERASED != op_result))
    {
      FLASHLOG(3, ("Read Page failed with status %d\n", op_result));
      result = FLASH_PROG_FAIL;
      SET_ERR(ERR_READ_FAIL);
      flash_utils_deinit();
    }
    else
    {
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }

  return result;
}  // flash_utils_read_page


/*===========================================================================
  This function will fill a page of the NOR flash with a 32-bit value
===========================================================================*/
uint32 flash_utils_fill_page(void)
{
  uint32 page;
  uint32 fill_value;
  int i, j;
  uint32 *fill_ptr;
  uint32 result = FLASH_PROG_SUCCESS;
  int op_result = FLASH_DEVICE_DONE;

  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    page = prog_param.reserved;
    fill_value = prog_param.size;

    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Writing to Logical Page 0x%x\n", page));

    /* Now we can fill the page buffer */
    fill_ptr = (uint32 *)page_buffer;
    for (i = 0, j = 1; i < page_size_bytes / (sizeof(uint32)); i++)
    {
      *fill_ptr = fill_value;
      if (i >= (page_size_bytes /(sizeof(uint32))))
      {
       *fill_ptr = j | ((j+1)<<8) | ((j+2) << 16) | ((j+3) << 24);
       j += 4;
      }
      fill_ptr++;
    }

    op_result = flash_prog_write_pages(handle_flash, &dev_info,
      page, 1, page_buffer);

    if (FLASH_DEVICE_DONE != op_result)
    {
      FLASHLOG(3, ("Write Page failed with status %d\n", op_result));
      SET_ERR(ERR_WRITE_FAIL);
      result = FLASH_PROG_FAIL;
      flash_utils_deinit();
    }
    else
    {
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }

  return result;
}  // flash_utils_fill_page


/*===========================================================================
  This function retrieves the raw ID of the flash.
===========================================================================*/

uint32 flash_utils_read_id(void)
{
  uint32 id;
  uint32 result;

  FLASHLOG(1,("\n\n------ Read ID---------------\n"));

  /* Probe will fill values in structure if it succeeds.
  * Currently, which_flash parameter is hard coded to 0, but in
  * future this will be dynamic.
  */
  result = flash_utils_init();

  FLASHLOG(1, ("=======================\n"));

  if (FLASH_PROG_SUCCESS == result)
  {
    FLASHLOG(1, ("Flash Device Detected \n"));
  }
  else
  {
    FLASHLOG(1, ("Flash Device NOT Detected \n"));
    return FLASH_PROG_FAIL;
  }

  id = (uint32)(dev_info.device_id | (dev_info.maker_id << 8));

  FLASHLOG(1,("Raw flash ID is 0x%x\n", id));

  KICK_WATCHDOG();

  prog_param.reserved = id;
  FLASHLOG(1, ("============= Done =============\n"));

  return result;
} // flash_utils_read_id


/*===========================================================================
  This function reads the platform ID from CDT (Config Data Table) in EEPROM.
  
  Please refer to the following document (Agile: 80-N3411-1) for details:
  "APPLICATION NOTE: EEPROM Software Configuration Data Table (CDT)"
===========================================================================*/
uint32 flash_utils_read_platform_id(void)
{
  uint32 result = FLASH_PROG_SUCCESS;
  int op_result = FLASH_DEVICE_DONE;
  uint16 pid_offset;
  uint32 magic_number;
  uint8 pid_platform = 0, pid_version, pid_hw_major, pid_hw_minor, pid_subtype;
  
  result = flash_utils_init();

  (void)pid_hw_major;
  (void)pid_hw_minor;
  (void)pid_subtype;

  if (FLASH_PROG_SUCCESS == result)
  {
    memset(page_buffer, 0x00, page_size_bytes);

    /* CDT header is in first page (page 0), which contains the platform ID */
    op_result = flash_prog_read_pages(handle_flash, &dev_info,
      0, 1, page_buffer);

    if (FLASH_DEVICE_DONE != op_result)
    {
      result = FLASH_PROG_FAIL;
      SET_ERR(ERR_READ_FAIL);
      FLASHLOG(1, ("Failed to read CDT from this device.\n"));
      flash_utils_deinit();
    }
    else
    {
      /* Check the magic number existence */
      magic_number = (page_buffer[0] | page_buffer[1] << 8 |
        page_buffer[2] << 16 | page_buffer[3] << 24);
      if (magic_number == FLASH_CDT_MAGIC_NUMBER)
      {
        /* Get the offset of pid info block */
        pid_offset = (page_buffer[FLASH_CDT_PLATFORM_ID_IDX] |
          page_buffer[FLASH_CDT_PLATFORM_ID_IDX+1] << 8);
        
        /* Get pid info */
        pid_version = page_buffer[pid_offset];
        pid_platform = page_buffer[pid_offset+1];
        pid_hw_major = page_buffer[pid_offset+2];
        pid_hw_minor = page_buffer[pid_offset+3];
        pid_subtype = page_buffer[pid_offset+4];

        FLASHLOG(1, ("Platform ID is: %d\n", pid_platform));
        FLASHLOG(1, ("Hardware Major Version is: %d\n", pid_hw_major));
        FLASHLOG(1, ("Hardware Minor Version is: %d\n", pid_hw_minor));
        if (pid_version == 0x2)
        {
          FLASHLOG(1, ("Platform Subtype is: %d\n", pid_subtype));
        }
      }
    }
  }

  prog_param.reserved = pid_platform;
  return result;
}
