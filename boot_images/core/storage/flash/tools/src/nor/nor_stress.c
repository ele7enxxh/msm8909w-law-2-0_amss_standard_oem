/*=======================================================================
 * FILE:        nor_stress.c
 *
 * SERVICES:    Stress test APIs for flash driver
 *
 * GENERAL DESCRIPTION
 *   Stress test APIs for flash driver
 *
 * Copyright(c) 2010-2013 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_stress.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 12/17/13     whc     Add RF EEPROM I2C support
 * 03/19/12     sv      Add power mode api tests
 * 07/20/11     sv      Use compile flag to decide SPI/I2C NOR tools
 * 09/10/10     sv      Initial version
 *==================================================================*/

#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include <stdio.h>
#include "nor_config.h"
#include "flash_log.h"
#include "nor_wrapper.h"

#define NOR_WAIT_COUNT 0xFFFFF

#define STRESS_TEST_MAX_PAGES         10
#define STRESS_TEST_MAX_PAGE_SIZE     512
#define STRESS_TEST_MAX_TEST_BLOCKS   64

static int result_vec[STRESS_TEST_MAX_TEST_BLOCKS];
static struct flash_block_vector block_vector[STRESS_TEST_MAX_TEST_BLOCKS];

uint8 page_write_raw_buffer[STRESS_TEST_MAX_PAGES * STRESS_TEST_MAX_PAGE_SIZE];
uint8 page_read_raw_buffer[STRESS_TEST_MAX_PAGES * STRESS_TEST_MAX_PAGE_SIZE];

uint32 enable_efs2_parti_test = FALSE;
uint32 enable_erase_test = TRUE;
uint32 enable_read_write_test = TRUE;
uint32 enable_partial_write_test = TRUE;
uint32 enable_suspend_resume_test = TRUE;
uint32 enable_power_mode_test = TRUE;

int flash_test_erase_api(flash_handle_t handle_flash, struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE;
  uint32 block_count, block, start_block;
  int test_result = FLASH_DEVICE_DONE;

  FLASHLOG(3, ("----------------------------------------\n"));
  FLASHLOG(3, ("Testing Block Erase APIs\n"));

  /* Erase ALL Blocks individually, skipping any bad blocks */
  block_count = info->partition.block_count;
  
  block_vector[0].block_count = 1;
  block_vector[0].result_vector = &result_vec[0];

  start_block = 0;
  block = start_block;
  while (block < block_count)
  {
    KICK_WATCHDOG();

    block_vector[0].start_block = block++;
    result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
    if (DAL_SUCCESS != result) 
    {
      FLASHLOG(3, ("flash_erase_blocks failed on blk %d\n", \
        block_vector[0].start_block));
      test_result = FLASH_DEVICE_FAIL;
      break;
    }
    else
    {
      FLASHLOG(3,(" ."));  
      if (((block - start_block) % 20) == 0)
      {
        FLASHLOG(3, ("\n"));
      }    
    }        
  }

  FLASHLOG(3, ("\n"));

  if (DAL_SUCCESS == test_result)
  {
    FLASHLOG(3, ("- Single Erase of %d Blocks PASSED\n", block_count));
  }

  block_vector[0].start_block = 0;
  block_vector[0].block_count = 
   (block_count > STRESS_TEST_MAX_TEST_BLOCKS)? STRESS_TEST_MAX_TEST_BLOCKS :
   block_count;
  result = flash_erase_blocks(handle_flash, &block_vector[0], 1);

  if (DAL_SUCCESS != result) 
  {
    FLASHLOG(3, ("- Vector Erase of %d Blocks FAILED\n", block_count));
    test_result = FLASH_DEVICE_FAIL;
  }
  else
  {
    FLASHLOG(3, ("- Vector Erase of %d Blocks PASSED\n", block_count));
  }      

  FLASHLOG(3, ("----------------------------------------\n"));

  return test_result;
}


int flash_test_read_write_api(flash_handle_t handle_flash, 
  struct flash_info *info, 
  struct flash_nor_erase_region_info *erase_region_info)
{
  int result = FLASH_DEVICE_DONE;
  uint32 block, index, count, block_size = 0, region_start_blk;
  int test_result = FLASH_DEVICE_DONE;
  uint16 pages_per_block, max_pages_to_test;
  uint32 start_page = 0;
  uint32 page_size_bytes = 0;
  uint32 logical_start_addr = 0;
  uint32 phy_start_block, region_blk_size, region_blk_cnt;

  FLASHLOG(3, ("----------------------------------------\n"));
  FLASHLOG(3, ("Testing Page Read/Page Write APIs\n"));

  block = 0;  /* Change this value to perform the test on a different block */
  block_vector[0].start_block = block;
  block_vector[0].result_vector = &result_vec[0];
  block_vector[0].block_count = 1;

  page_size_bytes = info->partition.page_size_bytes;

  region_start_blk = 0;
  phy_start_block = info->partition.physical_start_block;

  for(index = 0; index < erase_region_info->erase_region_count; index++)
  {
    region_blk_size = 
      erase_region_info->erase_region[index].block_size_in_bytes;
    region_blk_cnt = erase_region_info->erase_region[index].block_count;

    if (phy_start_block > (region_start_blk + region_blk_cnt))
    {    
      continue;
    }
    else if ((phy_start_block + block) < (region_start_blk + region_blk_cnt))
    {
      if(phy_start_block < region_start_blk)
      {
        logical_start_addr += 
          (phy_start_block + block - region_start_blk) * region_blk_size;
      }
      else
      {
        logical_start_addr += block * region_blk_size;
      }
      block_size = region_blk_size;
      break;
    }
    else if(phy_start_block < (region_start_blk + region_blk_cnt))
    {
      logical_start_addr += 
        (region_start_blk + region_blk_cnt - phy_start_block) * region_blk_size;
    }  

    region_start_blk += region_blk_cnt;
  }
  if (block_size == 0)
  {
    FLASHLOG(3, (" Invalid Block\n"));
    return (-1);
  }

  start_page = logical_start_addr / page_size_bytes; 
  /* Write n pages sequentially and read n pages sequentially
   * to confirm the computed data */
       
  count = 0;

  FLASHLOG(3, ("  Testing Write and Read of n pages \n "));
   
  pages_per_block = block_size / page_size_bytes;
  /* loop for the number of pages in the blocks */
  max_pages_to_test = (pages_per_block < STRESS_TEST_MAX_PAGES) 
    ? pages_per_block : STRESS_TEST_MAX_PAGES;

  while (++count <= max_pages_to_test)
  {
    KICK_WATCHDOG();
    FLASHLOG(3, ("     n = %d\n", count));
    /* start with an erased block */
    result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
    if (DAL_SUCCESS != result) 
    {
      FLASHLOG(3, ("flash_erase_blocks failed on blk %d\n", \
        block_vector[0].start_block));
      result = FLASH_DEVICE_FAIL;
      test_result = FLASH_DEVICE_FAIL;
      break;
    }
      
    /* Compute and init write data for the count pages */
    for (index = 0; index < count; index++)
    {
      DALSYS_memset(&page_write_raw_buffer[index * page_size_bytes], 
        (count + index), page_size_bytes);
    }

    KICK_WATCHDOG();  
    result = flash_prog_write_pages(handle_flash, info, start_page, count, 
      &page_write_raw_buffer[0]);
    KICK_WATCHDOG();
    if (DAL_SUCCESS != result)
    {
      test_result = FLASH_DEVICE_FAIL;
      FLASHLOG(3, ("     flash_prog_write_pages of %d pages failed\n", count));
      break;
    }
    else
    {
      FLASHLOG(3, ("     flash_prog_write_pages of %d pages passed\n", count));        
    }

    /* init read data for the count pages */
    DALSYS_memset(&page_read_raw_buffer[0], 0x0, count * page_size_bytes);

    result = flash_prog_read_pages(handle_flash, info, start_page, count, 
      &page_read_raw_buffer[0]);

    if (DAL_SUCCESS != result)
    {
      FLASHLOG(3, ("     flash_prog_read_pages of %d pages failed\n", count));
      test_result = FLASH_DEVICE_FAIL;
      break;
    }
    KICK_WATCHDOG();
    for (index = 0; index < count; index++)
    {
      if (memcmp(&page_read_raw_buffer[index * page_size_bytes],
                 &page_write_raw_buffer[index * page_size_bytes],
                 page_size_bytes) != 0)
      {
        FLASHLOG(3, ("     Data compare failed count = %d, index = %d\n", \
          count, index));
        test_result = FLASH_DEVICE_FAIL;
      }
    }
  }

  FLASHLOG(3, ("----------------------------------------\n"));
  if (test_result == FLASH_DEVICE_FAIL)
  {
    FLASHLOG(3, ("page write/page read api test Failed\n"));
  }
  else
  {
    FLASHLOG(3, ("page write/page read api test succeeded\n"));
  }
  FLASHLOG(3, ("----------------------------------------\n"));

  return test_result;
}


int flash_test_read_partial_write_api(flash_handle_t handle_flash, 
  struct flash_info *info, struct flash_nor_erase_region_info *nor_region_info)
{
  int result = FLASH_DEVICE_DONE;
  uint32 block, index, word_count, block_size = 0, region_start_blk;
  int test_result = FLASH_DEVICE_DONE;
  uint32 region_blk_size, region_blk_cnt, phy_start_block, start_page;
  uint32 logical_start_addr = 0;
  void *buffer = NULL;
  uint32 byte_offset, write_size_bytes;
  uint32 pages_per_block, max_pages_to_test, page_index;
  uint32 page_size_bytes;

  FLASHLOG(3, ("----------------------------------------\n"));
  FLASHLOG(3, ("Testing Page Read/Partial Page Write APIs\n"));

  block = 1; /* Change this value to perform test in a different block */

  block_vector[0].start_block = block;
  block_vector[0].block_count = 1;
  block_vector[0].result_vector = &result_vec[0];

  page_size_bytes = info->partition.page_size_bytes;
       
  region_start_blk = 0;
  phy_start_block = info->partition.physical_start_block;

  for(index = 0; index < nor_region_info->erase_region_count; index++)
  {
    region_blk_size = nor_region_info->erase_region[index].block_size_in_bytes;
    region_blk_cnt = nor_region_info->erase_region[index].block_count;

    if (phy_start_block > (region_start_blk + region_blk_cnt))
    {    
      continue;
    }
    else if ((phy_start_block + block) < (region_start_blk + region_blk_cnt))
    {
      if(phy_start_block < region_start_blk)
      {
        logical_start_addr += 
          (phy_start_block + block - region_start_blk) * region_blk_size;
      }
      else
      {
        logical_start_addr += block * region_blk_size;
      } 
      block_size = region_blk_size;
      break;
    }
    else if(phy_start_block < (region_start_blk + region_blk_cnt))
    {
      logical_start_addr += (region_start_blk + region_blk_cnt - phy_start_block) * region_blk_size;
    }      

    region_start_blk += region_blk_cnt;
  }

  if (block_size == 0)
  {
    FLASHLOG(3, (" Invalid Block\n"));
    return (-1);
  }

  start_page = logical_start_addr / page_size_bytes; 

  /* Write n pages sequentially and read n pages sequentially
     to confirm the computed data */
  FLASHLOG(3, (" - Testing Partial Page Write + Read Page\n"));
  
  /* start with an erased block */
  result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
  if (DAL_SUCCESS != result) 
  {
    FLASHLOG(3, ("flash_erase_blocks failed on blk %d\n", \
      block_vector[0].start_block));
    test_result = FLASH_DEVICE_FAIL;
  }
  else
  {
    FLASHLOG(3,(" Erase Completed Successfully\n"));  
    pages_per_block = block_size / page_size_bytes;
    /* loop for the number of pages in the blocks */
    max_pages_to_test = (pages_per_block < STRESS_TEST_MAX_PAGES) 
      ? pages_per_block : STRESS_TEST_MAX_PAGES;

    for(page_index = 0; page_index < max_pages_to_test; page_index++)
    {
      /* Compute and init write data for the count pages */
      word_count = 1;
      for (index = 0; index < page_size_bytes >> 1; index++)
      {
        DALSYS_memset(&page_write_raw_buffer[index << word_count], index, 
          word_count << 1);
        byte_offset = (index << 1) * word_count;     
        buffer = &page_write_raw_buffer[index << word_count]; 
        write_size_bytes = word_count << 1;  
     
        KICK_WATCHDOG(); 
        result = flash_prog_write_partial_page(handle_flash, info, start_page,
          byte_offset, write_size_bytes, buffer);

        if (DAL_SUCCESS != result)
        {
          result = FLASH_DEVICE_FAIL;
          FLASHLOG(3, ("flash_write_partial_page of %d bytes failed\n", \
            write_size_bytes));
          break;
        }
      }

      DALSYS_memset(&page_read_raw_buffer[0], 0x0, page_size_bytes);

      result = flash_prog_read_pages(handle_flash, info, start_page, 1, 
        &page_read_raw_buffer[0]);
      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_prog_read_pages of 1 page failed\n" ));
        test_result = FLASH_DEVICE_FAIL;     
      }

      for (index = 0; index < page_size_bytes ; index++)
      {
        if (page_read_raw_buffer[index] != page_read_raw_buffer[index])
        {
          FLASHLOG(3, (" Data compare failed index = %d\n", index));
          result = FLASH_DEVICE_FAIL;
          break;
        }
      }
    }
    start_page++;
  }
  FLASHLOG(3, ("-----------------------------------------------\n"));
  if (result == FLASH_DEVICE_DONE)
  {
    FLASHLOG(3, ("Page Read/Partial Page Write APIs Test Passed\n"));
  }
  else
  {
    FLASHLOG(3, ("Page Read/Partial Page Write APIs Test Failed\n"));
  }
  FLASHLOG(3, ("-----------------------------------------------\n"));
  return test_result;
}



int flash_test_erase_suspend_api(flash_handle_t handle_flash, 
  struct flash_info *info, struct flash_nor_erase_region_info *nor_region_info)
{
  int result = FLASH_DEVICE_DONE;
  uint32 block, index, count, wait_cnt;
  int test_result = FLASH_DEVICE_DONE;
  uint32  region_start_blk, phy_start_block, region_blk_size, region_blk_cnt;
  uint32  logical_start_addr = 0, start_page;
  uint32 page_size_bytes;

  FLASHLOG(3, ("----------------------------------------\n"));
  FLASHLOG(3, ("Testing Erase Suspend APIs\n"));

  (void)count;

  block = 10; /* Change this value to perform the test on a different block*/

  page_size_bytes = info->partition.page_size_bytes;

  region_start_blk = 0;
  phy_start_block = info->partition.physical_start_block;

  for(index = 0; index < nor_region_info->erase_region_count; index++)
  {
    region_blk_size = nor_region_info->erase_region[index].block_size_in_bytes;
    region_blk_cnt = nor_region_info->erase_region[index].block_count;

    if ((phy_start_block + block) < (region_start_blk + 
         nor_region_info->erase_region[index].block_count))
    {    
      logical_start_addr += (phy_start_block + block - region_start_blk) * 
        region_blk_size; 
      break;
    }
    if (phy_start_block < (region_start_blk + 
         nor_region_info->erase_region[index].block_count ))
    {
      if(phy_start_block > region_start_blk)
      {
        logical_start_addr += 
          (region_start_blk + region_blk_cnt - phy_start_block) * 
          region_blk_size;
      }
      else
      {
        logical_start_addr += region_blk_cnt *
          region_blk_size;
      }
    }
    region_start_blk += region_blk_cnt;
  }


  start_page = logical_start_addr / page_size_bytes;
  
  /* Write n pages sequentially and read n pages sequentially
     to confirm the computed data */
  FLASHLOG(3, (" - Testing Write + Read\n"));

  count = 0;

  FLASHLOG(3, ("   - Write and Read of n pages \n "));
   
  
  KICK_WATCHDOG();
  /* start with an erased block */
  result = flash_begin_erase_block(handle_flash, block);
  if (DAL_SUCCESS != result) 
  {
    FLASHLOG(3, ("flash_begin_erase_block failed on blk %d\n", block));
    result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }
      
  wait_cnt = NOR_WAIT_COUNT;
  while(1)
  {
    result = flash_get_status(handle_flash);
    if ((--wait_cnt == 0) || (result != FLASH_DEVICE_OP_INPROGRESS))
    {
      break;
    } 
    KICK_WATCHDOG(); 
  }      

  if (FLASH_DEVICE_OP_INPROGRESS == result)
  {
    FLASHLOG(3, (" Erase Time Out\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }
  else  if (FLASH_DEVICE_FAIL == result)
  {
    FLASHLOG(3, (" Erase Failed\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }
  
  /* Compute and init write data for the count pages */
  for (index = 0; index < page_size_bytes ; index++)
  {
    page_write_raw_buffer[index] = index;
  }  

  result = flash_prog_write_pages(handle_flash, info, start_page, 1, 
    &page_write_raw_buffer[0]);
  if (DAL_SUCCESS != result) 
  {
    test_result = FLASH_DEVICE_FAIL;
    FLASHLOG(3, ("     flash_prog_write_pages of %d pages failed\n", count));
    goto suspend_test_end;
  }

  /* start with an erased block */
  result = flash_begin_erase_block(handle_flash, block + 1);
  if (DAL_SUCCESS != result) 
  {
    FLASHLOG(3, ("flash_begin_erase_block failed on blk %d\n", \
                block_vector[0].start_block));
    result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }

  result = flash_erase_suspend(handle_flash);
  
  if (FLASH_DEVICE_FAIL == result) 
  {
    FLASHLOG(3, ("flash_erase_suspend failed on blk %d\n", \
                block_vector[0].start_block));
    test_result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }
 
  if (FLASH_DEVICE_DONE == result) 
  {
    FLASHLOG(3, ("erase block completed before suspending\n", \
                 block_vector[0].start_block));
    test_result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }

  if (FLASH_DEVICE_NOT_SUPPORTED == result) 
  {
    FLASHLOG(3, (" **Erase Suspend Not Supported by this device**\n", \
                 block_vector[0].start_block));
    test_result = FLASH_DEVICE_DONE;
    goto suspend_test_end;
  }

  /* init read data for the count pages */
  DALSYS_memset(&page_read_raw_buffer[0], 0x0, page_size_bytes);
  
  result = flash_prog_read_pages(handle_flash, info, start_page, 1, 
    &page_read_raw_buffer[0]);
  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, ("     flash_prog_read_pages of %d pages failed\n", count));
    test_result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }

  if (memcmp(&page_read_raw_buffer[0],
             &page_write_raw_buffer[0],
             page_size_bytes) != 0)
  {
    FLASHLOG(3, ("     Data compare failed count = %d, index = %d\n", \
      count, index));
    test_result = FLASH_DEVICE_FAIL;
  }
  
  result = flash_erase_resume(handle_flash);
  if (FLASH_DEVICE_DONE == result) 
  {
    FLASHLOG(3, ("erase resume done successfully\n", \
      block_vector[0].start_block));
  }

  wait_cnt = NOR_WAIT_COUNT;
  while(1)
  {
    result = flash_get_status(handle_flash);
    if ((--wait_cnt == 0) || (result != FLASH_DEVICE_OP_INPROGRESS))
    {
      break;
    } 
    KICK_WATCHDOG(); 
  }      

  if (FLASH_DEVICE_DONE == result)
  {
    FLASHLOG(3, ("Resumed Erase completed Successfully\n"));
  }
  else if (FLASH_DEVICE_OP_INPROGRESS == result)
  {
    FLASHLOG(3, (" Resumed Erase Time Out\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }
  else if (FLASH_DEVICE_FAIL == result)
  {
    FLASHLOG(3, ("Resumed Erase Failed\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto suspend_test_end;
  }

  FLASHLOG(3, ("----------------------------------------\n"));

suspend_test_end:

  return test_result;
}


int flash_test_power_mode_api(flash_handle_t handle_flash,
  struct flash_info *info,
  struct flash_nor_erase_region_info *erase_region_info)
{
  int result = FLASH_DEVICE_DONE;
  uint32 block, index;
  int test_result = FLASH_DEVICE_DONE;
  uint32 start_page = 0;
  uint32 page_size_bytes = 0;

  FLASHLOG(3, ("----------------------------------------\n"));
  FLASHLOG(3, ("Testing Power Mode APIs\n"));

  block = 0;
  block_vector[0].start_block = block;
  block_vector[0].result_vector = &result_vec[0];
  block_vector[0].block_count = 1;

  page_size_bytes = info->partition.page_size_bytes;
  
  /* start with an erased block */
  result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
  if (DAL_SUCCESS != result) 
  {
    FLASHLOG(3, ("flash_erase_blocks failed on blk %d\n", \
      block_vector[0].start_block));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }
      
  /* Compute and init write data */
  for (index = 0; index < page_size_bytes; index++)
  {
    page_write_raw_buffer[index] = index;
  }

  KICK_WATCHDOG();  

  /* Write a Page with some known data */
  result = flash_prog_write_pages(handle_flash, info, start_page, 1, 
    &page_write_raw_buffer[0]);

  KICK_WATCHDOG();
  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, (" flash_prog_write_pages failed\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }

  /* init read data buffer */
  DALSYS_memset(&page_read_raw_buffer[0], 0x0, page_size_bytes);

  /* Readback the data written and compare */
  result = flash_prog_read_pages(handle_flash, info, start_page, 1, 
    &page_read_raw_buffer[0]);

  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, (" flash_prog_read_pages b4 DPD failed\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }

  KICK_WATCHDOG();

  if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0], 
    page_size_bytes) != 0)
  {
    FLASHLOG(3, ("  Data compare failed b4 DPD test \n"));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }
  
  /* Set the flash device to LOW POWER mode */
  result =  flash_set_power_mode(handle_flash, FLASH_POWER_LOW_PWR_MODE);
  if (result != FLASH_DEVICE_DONE)
  {	  
    FLASHLOG(3, (" Set Power mode to DPD failed\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }

  /* init read data buffer */
  DALSYS_memset(&page_read_raw_buffer[0], 0x0, page_size_bytes);

  /* Readback the same data programmed earlier */
  result = flash_prog_read_pages(handle_flash, info, start_page, 1, 
    &page_read_raw_buffer[0]);

  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, (" flash_prog_read_pages during DPD failed\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }

  /* Compare the data written and readback. It should not match */
  if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0], 
    page_size_bytes) == 0)
  {
    FLASHLOG(3, (" Failed - Data comparison in DPD mode matched \n"));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }

  /* Set back the flash device to NORMAL MODE */
  result =  flash_set_power_mode(handle_flash, FLASH_POWER_NORMAL_MODE);
  if (result != FLASH_DEVICE_DONE)
  {	  
    FLASHLOG(3, (" Set Power mode to Stand-by Mode failed\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }

  /* init read data buffer */
  DALSYS_memset(&page_read_raw_buffer[0], 0x0, page_size_bytes);

  /* Readback the page again */
  result = flash_prog_read_pages(handle_flash, info, start_page, 1, 
    &page_read_raw_buffer[0]);

  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, (" flash_prog_read_pages after release from DPD failed\n"));
    test_result = FLASH_DEVICE_FAIL;
    goto power_mode_test_end;
  }

  /* Data raad should match the data written earlier */
  if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0], 
    page_size_bytes) != 0)
  {
    FLASHLOG(3, (" Data comparison in Stand-by mode failed \n"));
    test_result = FLASH_DEVICE_FAIL;
  }

power_mode_test_end:

  if (test_result == FLASH_DEVICE_FAIL)
  {
    FLASHLOG(3, ("power mode api test Failed\n"));
  }
  else
  {
    FLASHLOG(3, ("power mode api test succeeded\n"));
  }

  FLASHLOG(3, ("----------------------------------------\n"));

  return test_result;
}


int flash_test_get_info_api(flash_handle_t handle_flash, 
  struct flash_info *info, 
  struct flash_nor_erase_region_info *erase_region_info)
{
  int status;
  int index = 0;
  status = flash_get_info(handle_flash, FLASH_DEVICE_INFO, info);

  FLASHLOG(3, ("----------------------------------------\n"));
  FLASHLOG(3, ("Testing GetInfo API\n"));

  if(DAL_SUCCESS == status)
  {
    FLASHLOG(3, ("- dal_flash_GetInfo(FLASH_DEVICE_INFO) successful\n"));

    FLASHLOG(3, ("  Device Name = %s \n",info->device_name));
    FLASHLOG(3, ("  Device Type = 0x%x \n",info->type));
    FLASHLOG(3, ("  Device Bits Per Cell = 0x%x \n",info->tech));
    FLASHLOG(3, ("  Device Width = 0x%x \n",info->width));
    FLASHLOG(3, ("  Device Write Style = 0x%x \n",info->write_style));

    FLASHLOG(3, ("  Device Maker ID = 0x%x \n",info->maker_id));
    FLASHLOG(3, ("  Device ID = 0x%x \n",info->device_id));
    FLASHLOG(3, ("  Device Version ID = 0x%x \n",info->version_id));

    FLASHLOG(3, ("  Partition ECC state = 0x%x \n", \
      info->partition.ecc_state));
    FLASHLOG(3, ("  Partition Total Blocks = %d \n", \
      info->partition.block_count));
    FLASHLOG(3, ("  Partition Page per Block count = %d \n", \
      info->partition.pages_per_block));
    FLASHLOG(3, ("  Partition Page Size = %d bytes \n", \
      info->partition.page_size_bytes));
    FLASHLOG(3, ("  Partition Total Page Size = %d bytes \n", \
      info->partition.total_page_size_bytes));
    FLASHLOG(3, ("  Partition Max Spare User Data bytes = %d bytes \n", \
      info->partition.max_spare_udata_bytes));
    FLASHLOG(3, ("  Partition Physical Start Block = 0x%x \n", \
      info->partition.physical_start_block));
    FLASHLOG(3, ("  Partition Attributes = 0x%x \n", \
      info->partition.attributes));
  }
  else
  {
    FLASHLOG(3, ("- dal_flash_GetInfo(FLASH_DEVICE_INFO) error\n"));
  }

  status = flash_get_info(handle_flash, FLASH_NOR_ERASE_REGION_INFO, \
    erase_region_info);
  if(DAL_SUCCESS == status)
  {
    FLASHLOG(3, ("- dal_flash_GetInfo(FLASH_NOR_ERASE_REGION_INFO) successful\n"));

    FLASHLOG(3, ("  Erase Region Count = %d \n", 
       erase_region_info->erase_region_count));
    
    for(index = 0; index < erase_region_info->erase_region_count; index++)
    {
      FLASHLOG(3, ("  Region %d block count = %d \n", index, \
                   erase_region_info->erase_region[index].block_count));
      FLASHLOG(3, ("  Region %d block size = %d \n", index, \
                   erase_region_info->erase_region[index].block_size_in_bytes));
    }
  }
  else
  {
    FLASHLOG(3, ("- dal_flash_GetInfo(FLASH_NOR_ERASE_REGION_INFO) error\n"));
  }

/* TBD - Erase CFG info */

  FLASHLOG(3, ("----------------------------------------\n"));
  return status;
}


int run_flash_api_tests (flash_handle_t handle_flash)
{
   int status;
   struct flash_info info;
   struct flash_nor_erase_region_info erase_region_info;

   status = flash_test_get_info_api(handle_flash, &info, &erase_region_info);
 
  if (enable_erase_test)
  {      
    if (DAL_SUCCESS == status)
    {
      status = flash_test_erase_api(handle_flash, &info);
    }
  }

  if (enable_read_write_test)
  {      
    if (DAL_SUCCESS == status)
    {
      status = flash_test_read_write_api(handle_flash, &info, 
        &erase_region_info);
    }
  }

  if (enable_partial_write_test)
  {      
    if (DAL_SUCCESS == status)
    {
      status = flash_test_read_partial_write_api(handle_flash, &info, 
        &erase_region_info);
    }
  }

  if (enable_suspend_resume_test)
  {      
    if (DAL_SUCCESS == status)
    {
      status = flash_test_erase_suspend_api(handle_flash, &info, 
        &erase_region_info);
    }
  }

  if (enable_power_mode_test)
  {      
    if (DAL_SUCCESS == status)
    {
      status = flash_test_power_mode_api(handle_flash, &info, 
        &erase_region_info);
    }
  }

  return status;
}

int flash_dal_stress_test ()
{
  uint32 iteration_count = 0;
  flash_handle_t handle_flash1;
  flash_handle_t handle_flash2;

  if(DAL_SUCCESS != flash_device_attach(dev_id, 
    &handle_flash1))                 
  {
    FLASHLOG(3, ("Error attaching to device driver \n"));
    return -1;
  }

  FLASHLOG(3, ("Device+Client1 Attached!\n"));

  //Open 'test' device...
  if(DAL_SUCCESS == flash_open_partition(handle_flash1, 
    (unsigned char*)"0:ALL"))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    return -1;
  }

  if(enable_efs2_parti_test)
  {    
    if(DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1, 
      &handle_flash2))                 
    {
      FLASHLOG(3, ("Error attaching to device driver \n"));
      return -1;
    }

    FLASHLOG(3, ("Device+Client2 Attached!\n"));

    //Open 'test' device...
    if(DAL_SUCCESS == flash_open_partition(handle_flash2, "0:EFS2"))
    {
      FLASHLOG(3, ("flash_open_partition successful\n"));
    }
    else
    {
      FLASHLOG(3, ("flash_open_partition error\n"));
      return -1;
    }
  } /* if(enable_efs2_parti_test) */


  while (iteration_count++ < 5)
  {       
    FLASHLOG(3, ("Running Tests on handle_flash1\n"));
    if (DAL_SUCCESS == run_flash_api_tests(handle_flash1))
    {
      FLASHLOG(3, ("All API tests PASSED on handle_flash1\n"));
      FLASHLOG(3, ("-------------------------------\n"));
      if(enable_efs2_parti_test)
      {      
        FLASHLOG(3, ("Running Tests on handle_flash2\n"));
        if (DAL_SUCCESS == run_flash_api_tests(handle_flash2))
        {
          FLASHLOG(3, ("All API tests PASSED on handle_flash2\n"));
        }
        else
        {
          FLASHLOG(3, ("API tests FAILED on handle_flash2\n"));
        }
        FLASHLOG(3, ("-------------------------------\n"));
      }
    }
    else
    {
      FLASHLOG(3, ("API tests FAILED on handle_flash1\n"));
    }
  }  

  DalDevice_Close((DalDeviceHandle*)handle_flash1);
  if(enable_efs2_parti_test)
  {      
    DalDevice_Close((DalDeviceHandle*)handle_flash2);

    DAL_DeviceDetach((DalDeviceHandle*)handle_flash2);

    FLASHLOG(3, ("Device+Client2 Detached!\n"));
  } /*   if(enable_efs2_parti_test) */
  DAL_DeviceDetach((DalDeviceHandle*)handle_flash1);

  FLASHLOG(3, ("Device+Client1 Detached!\n"));

  return 0;
}

