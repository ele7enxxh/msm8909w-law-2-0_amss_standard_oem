/*=======================================================================
 * FILE:        nand_utils.c
 *
 * SERVICES:    Various utility functions used in NAND.
 *
 * GENERAL DESCRIPTION
 *    Various utility functions used in NAND.
 *
 * Copyright (c) 2008-2010,2012-2013 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_utils.c#1 $
 *   $DateTime: 2015/09/01 00:30:35 $
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/29/13     sv      Fix KW issues
 * 03/01/12     sv      Fix potential invalid memory access issue
 * 11/18/10     jz      Fix KW warnings
 * 10/01/10     sb      Set the error as ERR_BAD_BLOCK when block is bad
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 04/06/10     sb      Fix for compiler warning
 * 03/17/10     sb      Set the block's state with flash_block_set_state API
 * 01/08/10     op      Support WM7 boot config read/write
 * 07/08/09     mh      Embed codeword number in buffer before writing a page
 * 05/07/09     eo      Fix compiler warning
 * 02/26/09     bb      Fix for write spare alone failure
 * 02/20/09     bb      Removed banned APIs
 * 02/20/09     jz      Further cleanup
 * 11/25/08     mm      Removed unused function
 * 11/20/08     mm      Throw error if user attempts to write a non-erased page
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALDeviceId.h"
#include "nand_wrapper.h"
#include "flash_prog.h"
#include "nand_param.h"
#include "nand_config.h"


/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                           LOCAL DATA

===========================================================================*/

/* the size of UUID data structure */
#define UUID_SIZE             16     /* sizeof(GUID) */
#define USER_DATA_BYTES         0x08     /* User meta data bytes/sector */
#define MAX_SPARE_DATA_BYTES    0x10     /* Total User metadata bytes/sector*/

/* FMD OEM reserved area bitfield. */
#define OEM_BLOCK_RESERVED    0x01
#define OEM_BLOCK_READONLY    0x02

typedef struct _SectorInfo
{
  uint32 dwReserved1;              // Reserved - used by FAL
  uint8  bOEMReserved;             // For use by OEM
  uint8  bBadBlock;                // Indicates if block is BAD
  uint16 wReserved2;               // Reserved - used by FAL
}SectorInfo, *PSectorInfo;


/* NAND device structure */
static flash_handle_t handle_flash;

static struct flash_info dev_info;
enum flash_block_state block_state;
static struct flash_block_vector block_vector;
static uint32 read_type;
static uint32 write_type;

static uint8 *page_buffer;
static uint8 *page_readback_buffer;

/* NAND device parameters */
static uint32 block_count;        /* number of erasable units in flash */
static uint32 pages_per_block;    /* number of pages in block          */
static uint32 page_size;          /* number of bytes in a page         */
static uint32 total_pages;        /* number of pages in the device     */
//static uint32 max_spare_udata_bytes;
static char device_name[32];
static uint32 total_page_size_bytes; /* total number of bytes in a page  */
static DALBOOL dev_is_initialized = FALSE;
static DALBOOL is_BldrInitialized = FALSE;
static unsigned char partName[32];
static int erase_result = 0;

/*
 * Safe string copy function for flash utils
 */
static void nand_utils_strcpy(uint8 *dest, const uint8 *src, uint32 len)
{
  nand_memcpy(dest, src, len);
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

  if (dev_is_initialized)
  {
    return FLASH_PROG_SUCCESS;
  }

  if (is_BldrInitialized)
  {
    nand_utils_strcpy((uint8 *)&partName, "0:APPS",
      sizeof("0:APPS"));
  }
  else
  {
    nand_utils_strcpy((uint8 *)&partName, "0:ALL",
    sizeof("0:ALL"));
  }

  if(DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1, //Device Id
    &handle_flash))                 //Handle
  {
    FLASHLOG(3, ("Error attaching to device driver \n"));
    return FLASH_PROG_FAIL;
  }

  FLASHLOG(3, ("Device+Client Attached!\n"));

  //Open 'test' device...
  if(DAL_SUCCESS == flash_open_partition(handle_flash, partName))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    result = FLASH_PROG_FAIL;
    goto detach;
  }

   status = flash_get_info(handle_flash, FLASH_DEVICE_INFO, &dev_info);

  if(DAL_SUCCESS != status)
  {
    FLASHLOG(3, ("- flash_get_info error\n"));
    result = FLASH_PROG_FAIL;
    goto close_and_detach;
  }

  /* Get some information about the device */
  block_count = dev_info.partition.block_count;
  pages_per_block = dev_info.partition.pages_per_block;
  page_size = dev_info.partition.page_size_bytes;
  total_page_size_bytes = dev_info.partition.total_page_size_bytes;
  //max_spare_udata_bytes = dev_info.partition.max_spare_udata_bytes;

  nand_utils_strcpy((uint8 *)&device_name, dev_info.device_name,
    sizeof(device_name));

  total_pages = block_count * pages_per_block;

  FLASHLOG(1, ("Found %s\n", device_name));
  FLASHLOG(1, ("Block count is %d \n", block_count));
  FLASHLOG(1, ("Pages/Block is %d \n", pages_per_block));
  FLASHLOG(1, ("Page size  is %d \n", page_size));
  FLASHLOG(1, ("Pages Avail is %d \n", total_pages));

  if (DAL_SUCCESS != DALSYS_Malloc(total_page_size_bytes,
    (void **)&page_buffer))
  {
    goto close_and_detach;
  }

  if (DAL_SUCCESS != DALSYS_Malloc(total_page_size_bytes,
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

  This function lists bad blocks found in the flash

===========================================================================*/
uint32 flash_utils_list_bad_blocks (void)
{
  uint32 result = FLASH_PROG_SUCCESS;
  uint32 block;
  int total_bad_blocks = 0;

  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Listing All Bad Blocks:\n"));

    for (block = 0; block < block_count; block++)
    {
      KICK_WATCHDOG();

      if (DAL_SUCCESS != flash_block_get_state(handle_flash, block, &block_state))
      {
        result = FLASH_PROG_FAIL;
        break;
      }

      if (FLASH_BLOCK_BAD == block_state)
      {
        FLASHLOG(1, ("- Bad block 0x%x\n", block));
        ++total_bad_blocks;
      }
    }

    if (FLASH_PROG_FAIL == result)
    {
      flash_utils_deinit();
    }
    else
    {
      FLASHLOG(1, ("Found 0x%x bad blocks\n", total_bad_blocks));
      FLASHLOG(1, ("========= Done ======\n"));
    }
  }

  return result;
}


/*===========================================================================

  This function invalidates a block by calling flash driver function

===========================================================================*/
uint32 flash_utils_invalidate_block(uint32 block)
{
  uint32 result = FLASH_PROG_SUCCESS;

  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Marking Block 0x%x Bad\n", block));

    /* Ensure block is in range */
    if (block >= block_count)
    {
      FLASHLOG(1, ("- Block 0x%x out of range \n", block));
      return FLASH_PROG_FAIL;
    }

    result = flash_block_set_state(handle_flash, block, FLASH_BLOCK_BAD);

    if (DAL_SUCCESS == result)
    {
         if (DAL_SUCCESS == flash_block_get_state(handle_flash, block, &block_state))
      {
            if (FLASH_BLOCK_BAD == block_state)
        {
          FLASHLOG(3, ("- Set Block to BAD PASSED\n"));
        }
        else
        {
          FLASHLOG(3, ("- Set Block to BAD Failed. Block is Still Good!\n"));
          result = FLASH_PROG_FAIL;
        }
      }
      else
      {
        FLASHLOG(3, ("- Read of Block State Failed!\n"));
        result = FLASH_PROG_FAIL;
      }
    }
    else
    {
      FLASHLOG(3, ("- Set Block to BAD Failed!\n"));
      result = FLASH_PROG_FAIL;
    }

    if (FLASH_PROG_FAIL == result)
    {
     FLASHLOG(1, ("- Block 0x%x invalidation failed\n", block));
      flash_utils_deinit();
    }
    else
    {
     FLASHLOG(1, ("Block 0x%x successfully invalidated\n", block));
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }

  return result;
} // flash_utils_invalidate_block

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
    if (block >= block_count)
    {
      FLASHLOG(1, ("- Block 0x%x out of range \n", block));
      return FLASH_PROG_FAIL;
    }

    if (DAL_SUCCESS == flash_block_get_state(handle_flash, block, &block_state))
    {
      if (FLASH_BLOCK_BAD == block_state)
      {
        FLASHLOG(3, ("- Block is Bad. No Erase Done\n"));
        SET_ERR (ERR_BAD_BLOCK);
        result = FLASH_PROG_FAIL;
      }
      else
      {
        block_vector.start_block = block;
        if (DAL_SUCCESS != flash_erase_blocks(handle_flash, &block_vector, 1))
        {
          FLASHLOG(3, ("- Erase Failed\n"));
          SET_ERR (ERR_ERASE_FAIL);
          result = FLASH_PROG_FAIL;
        }
      }
    }
    else
    {
      FLASHLOG(3, ("- Read of Block State Failed!\n"));
      result = FLASH_PROG_FAIL;
    }

    if (FLASH_PROG_FAIL == result)
    {
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
  This function erases a block, thereby making it appear valid once again.
===========================================================================*/
uint32 flash_utils_make_block_good(uint32 block)
{
  uint32 result = FLASH_PROG_SUCCESS;

  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Marking Block 0x%x Good\n", block));

    /* Ensure block is in range */
    if (block >= block_count)
    {
      FLASHLOG(1, ("- Block 0x%x out of range \n", block));
      return FLASH_PROG_FAIL;
    }

    /* Set the Block's state with flash_block_set_state() instead of erasing
     the block because flash_erase_blocks() won't update the bad block status
     table */
    result = flash_block_set_state(handle_flash, block, FLASH_BLOCK_OK);
    if (DAL_SUCCESS == result)
    {
         if (DAL_SUCCESS == flash_block_get_state(handle_flash, block, &block_state))
      {
            if (FLASH_BLOCK_OK == block_state)
        {
          FLASHLOG(3, ("- Set Block to Good PASSED\n"));
        }
        else
        {
          FLASHLOG(3, ("- Set Block to Good Failed!\n"));
          result = FLASH_PROG_FAIL;
        }
      }
      else
      {
        FLASHLOG(3, ("- Read of Block State Failed!\n"));
        result = FLASH_PROG_FAIL;
      }
    }
    else
    {
      FLASHLOG(3, ("- Set Block to Good Failed!\n"));
      result = FLASH_PROG_FAIL;
    }

    if (FLASH_PROG_FAIL == result)
    {
     FLASHLOG(1, ("- Making Block 0x%x good failed\n", block));
      flash_utils_deinit();
    }
    else
    {
     FLASHLOG(1, ("Block 0x%x successfully marked good\n", block));
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }

  return result;
}

/*===========================================================================
  This function will read a page from the NAND flash
===========================================================================*/
uint32 flash_utils_read_page(void)
{
  uint32 page;
  uint32 result = FLASH_PROG_SUCCESS;
  uint32 spare_udata_len = 0;
  int op_result = FLASH_DEVICE_DONE;

  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    page = prog_param.reserved;
    read_type = (int)prog_param.size;

    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Reading Logical Page 0x%x ", page));

    memset(page_buffer, 0x00, total_page_size_bytes);

    switch (read_type)
    {
      case FLASH_READ_MAIN:
        FLASHLOG(1, ("in Main only mode\n")); break;
      case FLASH_READ_SPARE:
        FLASHLOG(1, ("in Spare Udata only mode\n")); break;
      case FLASH_READ_MAIN_SPARE:
        FLASHLOG(1, ("in Main + Spare Udata mode\n")); break;
      case FLASH_READ_RAW:
        FLASHLOG(1, ("in RAW(Main+SpareUdata+ECC) mode\n")); break;
      default:
        FLASHLOG(1, ("with Unknown ECC state! Abort!\n"));
        return FLASH_PROG_FAIL;
    };

    switch (read_type)
    {
      case FLASH_READ_MAIN:
      case FLASH_READ_RAW:
        op_result = flash_prog_read_page(handle_flash, &dev_info,
          page, read_type, page_buffer);
        break;
      case FLASH_READ_SPARE:
      case FLASH_READ_MAIN_SPARE:
        spare_udata_len = (dev_info.partition.max_spare_udata_bytes == 0) ?
          16 : dev_info.partition.max_spare_udata_bytes;
        op_result = flash_prog_read_page_and_udata(handle_flash, &dev_info,
          page, read_type, page_buffer, page_buffer+page_size,
          spare_udata_len);
        break;
    }

    if (FLASH_DEVICE_DONE != op_result)
    {
       if (FLASH_DEVICE_FAIL_PAGE_ERASED == op_result)
      {
        FLASHLOG(3, ("Page 0x%x is Erased\n", page));
        memset(page_buffer, 0xFF, total_page_size_bytes);
      }
      else
      {
        FLASHLOG(3, ("Read Page failed with status %d\n", op_result));
        result = FLASH_PROG_FAIL;
      }
    }

    if (FLASH_PROG_FAIL == result)
    {
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
  This function will fill a page of the NAND flash with a 32-bit value
===========================================================================*/
uint32 flash_utils_fill_page(void)
{
  uint32 page;
  uint32 fill_value;
  int i, j;
  uint32 *fill_ptr;
  uint32 result = FLASH_PROG_SUCCESS;
  uint32 spare_udata_len = 0, cw_count = 0;
  int op_result = FLASH_DEVICE_DONE;

  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    page = prog_param.reserved;
    fill_value = prog_param.size;
    write_type = (int)prog_param.width;

    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Writing to Logical Page 0x%x ", page));

    switch (write_type)
    {
      case FLASH_WRITE_MAIN:
        FLASHLOG(1, ("in Main only mode\n")); break;
      case FLASH_WRITE_SPARE:
        FLASHLOG(1, ("in Spare Udata only mode\n")); break;
      case FLASH_WRITE_MAIN_SPARE:
        FLASHLOG(1, ("in Main + Spare Udata mode\n")); break;
      case FLASH_WRITE_RAW:
        FLASHLOG(1, ("in RAW(Main+SpareUdata+ECC) mode\n")); break;
      default:
        FLASHLOG(1, ("with Unknown ECC state! Abort!\n"));
        return FLASH_PROG_FAIL;
    };

    /* Now we can fill the page buffer */
    fill_ptr = (uint32 *)page_buffer;
    for (i = 0, j = 1; i < total_page_size_bytes / (sizeof(uint32)); i++)
    {
      *fill_ptr = fill_value;
      if (i >= (page_size /(sizeof(uint32))))
      {
       *fill_ptr = j | ((j+1)<<8) | ((j+2) << 16) | ((j+3) << 24);
       j += 4;
      }
      fill_ptr++;
    }

    /* Now insert the codeword count number in the first dword
       of each codeword of the page */
    fill_ptr = (uint32 *)page_buffer;
    cw_count = page_size/512;
    for (i =0; i < cw_count; i++)
    {
      *fill_ptr = i;
      fill_ptr += 512/(sizeof(uint32));
    }

    switch (write_type)
    {
      case FLASH_WRITE_MAIN:
      case FLASH_WRITE_RAW:
        op_result = flash_prog_write_page(handle_flash, &dev_info,
          page, write_type, page_buffer);
        break;
      case FLASH_WRITE_SPARE:
      case FLASH_WRITE_MAIN_SPARE:
        spare_udata_len = (dev_info.partition.max_spare_udata_bytes == 0) ?
          16 : dev_info.partition.max_spare_udata_bytes;
        op_result = flash_prog_write_page_and_udata(handle_flash, &dev_info,
          page, write_type, page_buffer, page_buffer+page_size,
          spare_udata_len);
        break;
    }

    if (FLASH_DEVICE_DONE != op_result)
    {
      FLASHLOG(3, ("Write Page failed with status %d\n", op_result));
      result = FLASH_PROG_FAIL;
    }

    if (FLASH_PROG_FAIL == result)
    {
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

  This function calls the flash probe function and sets up everything to be
  ready to erase and program the device.  Then it sees if the boot block
  is good.
===========================================================================*/

uint32 flash_utils_validate_boot_block(void)
{
  uint32 result;

  FLASHLOG(1,("\n\n------ Validate Boot Blcok -----------------\n"));

  FLASHLOG(1,("Probing flash device:  "));

  /*
  * Probe will fill values in structure if it succeeds.
  * Currently, which_flash parameter is hard coded to 0, but in
  * future this will be dynamic.
  */
  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Validating Boot Block:\n"));

    /* check boot block */
    if (DAL_SUCCESS != flash_block_get_state(handle_flash, 0, &block_state))
    {
      result = FLASH_PROG_FAIL;
    }

    if (FLASH_BLOCK_BAD == block_state)
    {
     FLASHLOG(1,("fvbb: Boot block is bad\n"));
     SET_ERR (ERR_BAD_BOOT);
     result = FLASH_PROG_FAIL;
    }

    if (FLASH_PROG_FAIL == result)
    {
      flash_utils_deinit();
    }
    else
    {
      FLASHLOG(1,("fvbb: Boot block is good\n"));
      SET_ERR (ERR_GOOD_BOOT);
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }

  return result;
} // flash_utils_validate_boot_block

/*
 * This function reads the uuid
 */
uint32 flash_utils_read_uuid (void)
{
  uint32 result = FLASH_PROG_SUCCESS;

  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
  FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Reading UUID:\n"));

    if (DAL_SUCCESS != flash_get_uuid(handle_flash, &prog_param.data,
      FLASH_PROG_BUFFER_SIZE, &prog_param.size))
    {
      result = FLASH_PROG_FAIL;
    }

    if (FLASH_PROG_FAIL == result)
    {
      FLASHLOG(3, ("- Get UUID Failed!\n"));
      flash_utils_deinit();
    }
    else
    {
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }

   return result;
 }

/*===========================================================================
  This function read a block status
===========================================================================*/
uint32
nand_bldr_get_block_status(uint32 blockId)
{

  enum flash_block_state block_state;
  int result = FLASH_DEVICE_DONE;

  flash_block_get_state(handle_flash, blockId, &block_state);
  result = (int)block_state;
  return result;
}

/*----------------------------------------------------------------------------
 * Function:  nand_erase_block
 *
 *  Erase the given block allowing for a number of retries
 *  The erase block routine may fail due to timing or
 *  other issues.  If it fails for four consecutive attempts
 *  then we should assume there is a problem!
 *---------------------------------------------------------------------------*/

uint32 nand_erase_bldr_cfg_block(uint32 block)
{
  uint32 rc = TRUE;

  struct flash_block_vector block_vector;

  /*  Erase block */
  block_vector.start_block = block;
  block_vector.block_count = 1;
  block_vector.result_vector = &erase_result;
  FLASHLOG(3, ("neb: Erasing boot config block 0x%x\n", block));

  if (flash_erase_blocks(handle_flash, &block_vector, 1)
                     != FLASH_DEVICE_DONE)
  {
   FLASHLOG(3, ("neb:Fail to erase block 0x%x \r\n", block ));
   rc = FALSE;
  }
  return rc;
}


/*===========================================================================
  This function will read a boot config table in 0:APPS
===========================================================================*/
uint32
flash_utils_bldr_read_boot_config(void)
{
  uint32 page;
  uint32 block;
  int result = FLASH_PROG_SUCCESS;
  uint32 spare_udata_len = 0;

  is_BldrInitialized = TRUE;
  result = flash_utils_init();

  if (FLASH_PROG_SUCCESS == result)
  {
    /* Configuration is located in first block of APPS partition */
    block  = 0;
  // skip bad blocks
    while (nand_bldr_get_block_status(block) == FLASH_BLOCK_BAD)
    {
      FLASHLOG(3,("bwc: find bad block block %d  \n",block));
      block++;
    }

    page = pages_per_block * block;;
    read_type = FLASH_READ_MAIN_SPARE;

    FLASHLOG(1, ("=======================\n"));
    FLASHLOG(1, ("Reading Logical Page 0x%x ", page));

    memset(page_buffer, 0x00, total_page_size_bytes);

    spare_udata_len = (dev_info.partition.max_spare_udata_bytes == 0) ?
      16 : dev_info.partition.max_spare_udata_bytes;
    result = flash_prog_read_page_and_udata(handle_flash, &dev_info,
      page, read_type, page_buffer, page_buffer+page_size,
      spare_udata_len);

    if (FLASH_DEVICE_DONE != result)
    {
      if (FLASH_DEVICE_FAIL_PAGE_ERASED == result)
      {
        FLASHLOG(3, ("Page 0x%x is Erased\n", page));
        memset(page_buffer, 0xFF, total_page_size_bytes);
      }
      else
      {
        FLASHLOG(3, ("Read Page failed with status %d\n", result));
        result = FLASH_PROG_FAIL;
      }
    }

    if (FLASH_PROG_FAIL == result)
    {
      flash_utils_deinit();
    }
    else
    {
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }

  return result;
}

/*===========================================================================
  This function will write a boot config table in 0:APPS
===========================================================================*/
uint32
flash_utils_bldr_write_boot_Config(void)
{
  SectorInfo sectorInfo, *pSectorInfo;
  uint32 page;
  int result = FLASH_PROG_SUCCESS;
  uint32 spare_udata_len = 0;
  uint8 *spare_udata;
  uint32 block;

  is_BldrInitialized = TRUE;

  result = flash_utils_init();

  if (result!=FLASH_PROG_FAIL)
  {
    /* Configuration is located in first block of APPS partition */
    block  = 0;
    while (nand_bldr_get_block_status(block) == FLASH_BLOCK_BAD)
    {
        FLASHLOG(3,("bwc: find bad block block %d  \n",block));
        block++;
    }
    page = pages_per_block * block;

    /* Erase block */
    if (!nand_erase_bldr_cfg_block(block))
    {
      FLASHLOG(3, ("Flash block %d erase failed\r\n", block ));
      return FLASH_PROG_FAIL;
    }

    /* Write boot configuration */
    /* Assume only one page for boot config */
    /* copy data to buffer */
    /* Prepare page spare info */
    pSectorInfo = &sectorInfo;
    memset(&sectorInfo, 0xFF, sizeof(sectorInfo));
    sectorInfo.bOEMReserved &= ~(OEM_BLOCK_READONLY|OEM_BLOCK_RESERVED);
    sectorInfo.dwReserved1 = 0;
    sectorInfo.wReserved2 = 0;
    spare_udata = &page_buffer[page_size];

    memset((byte *)(&spare_udata[0]), 0xff, MAX_SPARE_DATA_BYTES);
    /* Read the rest of SectInfo from spare_udata for 2048 byte devices */
    nand_memcpy ((byte *)(&spare_udata[0]), (byte *)pSectorInfo, USER_DATA_BYTES);
    if (!(*((byte *)pSectorInfo + 6) & 1))
    {
      /* Fill unique dirty bit position for the page in spare area
        if dirty bit is set to 0*/
      *(byte *)((byte *)(&spare_udata[0]) + USER_DATA_BYTES + \
         ((page & 0x3F)>>3)) &= ~((byte)(1<<(page & 0x07)));
    } /* if (!(*((byte *)pSectorInfo + 6) & 1))*/

    spare_udata_len = (dev_info.partition.max_spare_udata_bytes == 0) ?
     16 : dev_info.partition.max_spare_udata_bytes;
    write_type = FLASH_WRITE_MAIN_SPARE;

    /* Write sector  */
    result = flash_prog_write_page_and_udata(handle_flash, &dev_info,
      page, write_type, page_buffer, page_buffer+page_size,
      spare_udata_len);

    if (FLASH_DEVICE_DONE != result)
    {
      FLASHLOG(3, ("Write Page failed with status %d\n", result));
      result = FLASH_PROG_FAIL;
    }

    if (FLASH_PROG_FAIL == result)
    {
      flash_utils_deinit();
    }
    else
    {
      FLASHLOG(1, ("============= Done =============\n"));
    }
  }
  return result;
}

