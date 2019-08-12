/*=======================================================================
 * FILE:        nand_stress.c
 *
 * SERVICES:    Stress test APIs for flash driver
 *
 * GENERAL DESCRIPTION
 *   Stress test APIs for flash driver
 *
 * Copyright(c) 2008-2011,2012 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_stress.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/08/13     sv      Updated to support 2K + 128 byte Page NAND
 * 12/04/12     sv      Modified to Support 4K Page NAND / Cleanup
 * 09/12/12     sv      Adapt to Flash driver changes
 * 09/11/12     eo      Clean up
 * 07/10/12     eo      Skip uuid and write_spare api tests
 * 09/05/11     sv      Fix issue in erase api test
 * 03/02/11     sb      Adding 8bit ECC NAND support
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 08/03/10     jz      Added 2X support and cleaned up
 * 08/03/10     bn      Klocwork warning Fixes
 * 07/13/10     jz      Added KICK_WATCHDOG() in flash_test_read_write_pages_api
 * 06/07/10     sb      Set the Spare bytes as 16 for 4K NAND
 * 04/22/10     sb      No support for Get UUID on 512B NAND
 * 03/17/10     sb      Set the block's state with flash_block_set_state API
 * 02/05/10     mcg     Fix bug in UUID if FLASHLOG is not defined
 * 01/07/10     eo      Add support for 4K page OneNAND
 * 11/10/09     mh      Increse memory based on num of nand devices supported
 * 09/25/09     mh      Add support for 8K page
 * 08/11/09     bb      Support for stressing spare only write
 * 07/08/09     mh      Fill buffer with pattern to idenify codewords, clean-ups
 * 07/08/09     mm      Testing the Total Main and Spare writes for Onenand
 * 06/12/09     mh      Clean-ups and add more output comments
 * 03/22/09     bb      Added stress UUID check and DAL optimization changes
 * 04/28/09     bb      Header file case correction for Linux builds
 * 03/09/09     bb      Added stress for erase page check and iovec changes
 * 02/20/09     jz      Sync up changes related to prefix flash_prog_ for tools
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DALFramework.h"

#include <stdio.h>
#include <string.h>
#include "flash_mibib.h"
#include "flash_miparti.h"
#include "nand_param.h"
#include "flash_prog.h"
#include "flash.h"
#include "nand_core.h"
#include "nand_wrapper.h"
#include "nand_config.h"


#define MAX_PARTITION_SIZE_IN_BLOCKS  10

/* Check if the build/target  supports interleaving between 2 Nand Devices */
#if defined(FLASH_USES_DUAL_NAND_INTERLEAVE)
 #define FLASH_NUM_NAND_DEVICES        2
#else
 #define FLASH_NUM_NAND_DEVICES        1
#endif


#define STRESS_BUFFER_SIZE (128 * 4256 * FLASH_NUM_NAND_DEVICES)

/* Test partition definition used for stress testing the flash driver */
#define MIBIB_PARTI_SIZE                0x6
#define MIBIB_PARTI_PAD                 0x4
#define MIBIB_TEST1_PARTI_NAME           "0:ECCMainSpare1"
#define MIBIB_TEST1_PARTI_SIZE           2000
#define MIBIB_TEST1_PARTI_PAD            10

#define MIBIB_TEST2_PARTI_NAME           "0:ECCMainSpare2"
#define MIBIB_TEST2_PARTI_SIZE           2000
#define MIBIB_TEST2_PARTI_PAD            10

#define NAND_STRESS_UUID_BUFF_LEN        0x10

struct flash_usr_partition_table test_ram_table =
{
  FLASH_USR_PART_MAGIC1,
  FLASH_USR_PART_MAGIC2,
  FLASH_PARTITION_VERSION,

  3,  /* Number of partitions defined */

  {
    {
      MIBIB_PARTI_NAME,               /* Name of Partition */
      MIBIB_PARTI_SIZE,               /* Storage in blocks */
      MIBIB_PARTI_PAD,                /* Padding in blocks */
      0,                              /* Which flash  */
      0xFF,                           /* Reserved Flag 0 */
      0xFF,                           /* Reserved Flag 1 */
      0xFF,                           /* Reserved Flag 2 */
      0xFE,                           /* Reserved Flag 3 */
    },
    {
      MIBIB_TEST1_PARTI_NAME,              /* Name of Partition */
      MIBIB_TEST1_PARTI_SIZE,              /* Storage in blocks */
      MIBIB_TEST1_PARTI_PAD,               /* Padding in blocks */
      0,                                   /* Which flash  */
      0xFF,                                /* Reserved Flag 0 */
      FLASH_PARTITION_MAIN_AND_SPARE_ECC,  /* Reserved Flag 1 */
      0xFF,                                /* Reserved Flag 2 */
      0xFF,                                /* Reserved Flag 3 */
    },
    {
      MIBIB_TEST2_PARTI_NAME,              /* Name of Partition */
      MIBIB_TEST2_PARTI_SIZE,              /* Storage in blocks */
      MIBIB_TEST2_PARTI_PAD,               /* Padding in blocks */
      0,                                   /* Which flash  */
      0xFF,                                /* Reserved Flag 0 */
      FLASH_PARTITION_MAIN_AND_SPARE_ECC,  /* Reserved Flag 1 */
      0xFF,                                /* Reserved Flag 2 */
      0xFF,                                /* Reserved Flag 3 */
    },
  }
};

extern struct flash_prog_param prog_param;

static struct flash_block_vector block_vector[MAX_PARTITION_SIZE_IN_BLOCKS];
static struct flash_page_iovec page_data_iovec;

static uint8 page_write_raw_buffer[STRESS_BUFFER_SIZE];
static uint8 page_read_raw_buffer[STRESS_BUFFER_SIZE];
static int result_vec[64];

static uint32 stress_main_size_bytes = 0;
static uint32 stress_total_size_bytes = 0;
static uint32 stress_spare_index = 0;
static uint32 stress_partition_attributes = 0;
static uint32 stress_spare_size_bytes = 0;
static uint32 ecc_offset;
static uint32 ecc_len;
static uint32 ecc_num_cw;
static struct flash_info   Info;

/* Flag to enable 2X/dualplane tests */
static boolean run_2x_test = FALSE;

/* Flag to skip test */
static boolean skip_test = TRUE;

uint32 stress_test_cnt_max = 5;

extern void main_c(void);

/* DAL Memory descriptor for page operations */
static DALSYS_MEM_DESC_LIST_OBJECT(stress_buff_desc, 128);

/*
 * Initialize each codeword of page buffer with page number in dwrod 0
 * followed by the code word number.
 */
static void fill_buffer_with_patterns(uint32 page_count, uint32 start_page,
   enum page_write_opcode write_type)
{
  uint32 cw_count=0, cw_num = 0,  cw_size=0;
  uint32 page=0, blk_size=0, page_count_num = 0;
  uint8 * buffer;

  cw_count = Info.partition.page_size_bytes/512;
  buffer = (uint8 *)&page_write_raw_buffer[0];
  
  if(4256 == Info.partition.total_page_size_bytes)
  {
    start_page += Info.partition.physical_start_block << 7;
	blk_size = 128;
	cw_size = 532;
  } 	
  else
  {
    start_page += Info.partition.physical_start_block << 6;
	blk_size = 64;
	/* If total page size is 2128(2048 + 80)bytes, then the device is
	 * an 8 Bit ECC device and uses Code Word size 532 else its a 4 Bit
	 * ECC device which uses Code Word size 528.
	 */
	if (2128 == Info.partition.total_page_size_bytes)
	{
      cw_size = 532;
	}
	else
	{
      cw_size = 528;
    }
  }

  /* Get the signature that has the page count in  byte 3 and
     physical page num  in byte 0...2 */
  page_count_num  =  ((page_count << 24) | start_page);

  for(page = 0; page < blk_size; page++)
  {
     for(cw_num = 0; cw_num < cw_count; cw_num++)
     {
       *(uint32 *)buffer = page_count_num;
       memset (buffer+4, cw_num, 508);
       /* Point to the next codeword in the page */
       if (FLASH_WRITE_RAW == write_type)
       {
         buffer += cw_size;
       }
       else
       {
         buffer += 512;
       }
     }
    /* Update signature to next physical page */
     page_count_num++;
  }
}

/*
 * Initialize each codeword of 2x page buffer with page number in dword 0
 * followed by the code word number.
 */
void fill_2x_buffer_with_patterns(uint32 page_count, uint32 start_page,
   enum page_write_opcode write_type)
{
  uint32 cw_count=0, cw_num = 0,  page=0, page_count_num_0, page_count_num_1;
  uint8 *buffer_0, *buffer_1, pattern_0=0, pattern_1=0;
  uint32 start_page_0, start_page_1;

  cw_count = stress_main_size_bytes/512;

  buffer_0 = (uint8 *)&page_write_raw_buffer[0];
  if (FLASH_WRITE_RAW == write_type)
  {
    buffer_1 = (uint8 *)&page_write_raw_buffer[stress_total_size_bytes];
  }
  else
  {
    buffer_1 = (uint8 *)&page_write_raw_buffer[stress_main_size_bytes];
  }

  start_page_0 = start_page + Info.partition.physical_start_block << 6;
  start_page_1 = start_page_0 + 0x40;

  /* Get the signature that has the page count in  byte 3 and
     physical page num  in byte 0...2 */
  page_count_num_0  =  ((page_count << 24) | start_page_0);
  page_count_num_1  =  ((page_count << 24) | start_page_1);

  for(page = 0; page < 64; page++)
  {
     /* Fill buffer for both planes */
     for(cw_num = 0; cw_num < cw_count; cw_num++)
     {
       *(uint32 *)buffer_0 = page_count_num_0;
       *(uint32 *)buffer_1 = page_count_num_1;
       pattern_0 = ((page_count&0xF) << 4) | (cw_num+1);
       pattern_1 = ((page_count&0xF) << 4) | (cw_num+5);
       memset (buffer_0+4, pattern_0, 508);
       memset (buffer_1+4, pattern_1, 508);
       /* Point to the next codeword in the page */
       if (FLASH_WRITE_RAW == write_type)
       {
         buffer_0 += 528;
         buffer_1 += 528;
       }
       else
       {
         buffer_0 += 512;
         buffer_1 += 512;
       }
     }
     /* Update signature to next physical page */
     page_count_num_0++;
     page_count_num_1++;
     if (FLASH_WRITE_RAW == write_type)
     {
       buffer_0 += stress_total_size_bytes;
       buffer_1 += stress_total_size_bytes;
     }
     else
     {
       buffer_0 += stress_main_size_bytes;
       buffer_1 += stress_main_size_bytes;
     }
  }
}

/*
 * Stress test for flash erase API
 */
int flash_test_erase_api(flash_handle_t handle_flash, struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE;
  enum flash_block_state block_state = FLASH_BLOCK_OK;
  uint32 block_count = 0, block = 0, index = 0, prev_block = 0;
  int iovec_cnt = -1;
  DALBOOL test_passed;


  FLASHLOG(3, ("------------------------\n"));
  FLASHLOG(3, ("Testing block Erase APIs\n"));

  /* Erase ALL blocks individually, skipping any bad blocks */
  block_count = info->partition.block_count;

  block_vector[0].block_count = 1;
  block_vector[0].result_vector = &result_vec[0];

  /**-------------- TEST SINGLE ERASE BLOCK ---------------**/

  block = 0;
  while (block < block_count)
  {
    KICK_WATCHDOG();
    result = flash_block_get_state(handle_flash, block, &block_state);
    if (FLASH_DEVICE_DONE == result)
    {
    if (FLASH_BLOCK_OK != block_state)
    {
      FLASHLOG(3, ("Skipping Bad block 0x%x\n", block));
      block++;
      continue;
    }
    }
    else
    {
      block++;
      continue;
    }

    block_vector[0].start_block = block++;
    result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
    if (DAL_SUCCESS != result)
    {
      FLASHLOG(3, ("flash_erase_blocks failed\n"));
      break;
    }
  }

  if (FLASH_DEVICE_DONE == result)
  {
    FLASHLOG(3, ("- Single Erase of %d blocks PASSED\n", block_count));
  }

  /**-------------- TEST ERASE MULTI BLOCKS ---------------**/

  /* Erase 10 blocks in one shot with IOVEC mechanism */
  block = 0; index = 0;
  while ((block < block_count) && (index < 10))
  {
    KICK_WATCHDOG();
    result = flash_block_get_state(handle_flash, block, &block_state);
    if (FLASH_BLOCK_OK != block_state)
    {
      FLASHLOG(3, ("Skipping Bad block 0x%x\n", block));
      block++;
      continue;
    }

    if (prev_block == (block - 1))
    {
      if (iovec_cnt < 0)
      {
        iovec_cnt = 0;
        block_vector[iovec_cnt].start_block = block;
        block_vector[iovec_cnt].result_vector = &result_vec[index];
        block_vector[iovec_cnt].block_count = 1;
      }
      else
      {	      
        block_vector[iovec_cnt].block_count++;
      }
	}
    else
    {
      iovec_cnt++;
      block_vector[iovec_cnt].start_block = block;
      block_vector[iovec_cnt].block_count = 1;
      block_vector[iovec_cnt].result_vector = &result_vec[index];
    }

    prev_block = block;
    index++;
    block++;
  }

  result = flash_erase_blocks(handle_flash, &block_vector[0], (iovec_cnt + 1));

  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, ("flash_erase_blocks of %d blocks failed\n", index));
  }
  else
  {
    FLASHLOG(3, ("- Multi Erase of %d blocks PASSED\n", index));
  }

  /* Test Marking a block as BAD */
  test_passed = FALSE;
  result = flash_block_set_state(handle_flash,
    block_vector[0].start_block, FLASH_BLOCK_BAD);

  if (DAL_SUCCESS == result)
  {
    result = flash_block_get_state(handle_flash,
      block_vector[0].start_block, &block_state);

    if ((DAL_SUCCESS == result) && (FLASH_BLOCK_BAD == block_state))
    {
      /* Set the Block's state with flash_block_set_state() instead of erasing
       the block because flash_erase_blocks() won't update the bad block status
     table */
      result = flash_block_set_state(handle_flash,
        block_vector[0].start_block, FLASH_BLOCK_OK);

      if (DAL_SUCCESS == result)
      {
        result = flash_block_get_state(handle_flash,
          block_vector[0].start_block, &block_state);

        if ((DAL_SUCCESS == result) && (FLASH_BLOCK_OK == block_state))
        {
          FLASHLOG(3, ("- Set/Get block %d to BAD and GOOD PASSED\n",
            block_vector[0].start_block));
          test_passed = TRUE;
        }
      }
    }
  }

  if (FALSE == test_passed)
  {
    FLASHLOG(3, ("- Set/Get block %d to BAD and GOOD FAILED\n", block_vector[0].start_block));
    result = FLASH_DEVICE_FAIL;
  }

  FLASHLOG(3, ("------------------------\n"));

  return result;
}

/*
 *  This function tests the block Apis
 */
int flash_test_block_apis(flash_handle_t handle_flash, struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE;
  enum flash_block_state block_state;
  uint32 block;
  uint32 first_good_block = 0xFFFFFFFF;
  DALBOOL no_bad_blocks = TRUE;

  FLASHLOG(3, ("----------------------------\n"));
  FLASHLOG(3, ("Testing Block APIs\n"));

  FLASHLOG(3, ("\nPartition Start Block = 0x%x\n",
    info->partition.physical_start_block));
  FLASHLOG(3, ("Partition End Block = 0x%x\n\n",
    (info->partition.physical_start_block + info->partition.block_count)));

  FLASHLOG(3, ("(1) List All Bad Blocks\n"));

  block = 0;

  while (block < info->partition.block_count)
  {
    KICK_WATCHDOG();
    if (DAL_SUCCESS != flash_block_get_state(handle_flash, block,
      &block_state))
    {
      FLASHLOG(3, ("Read of Block State Failed!\n"));
      result = FLASH_DEVICE_FAIL;
      break;
    }

    if (FLASH_BLOCK_BAD == block_state)
    {
      FLASHLOG(3, ("Logical Bad Block = 0x%x\n", block));
      no_bad_blocks = FALSE;
    }
    else if (first_good_block == 0xFFFFFFFF)
    {
      first_good_block = block;
    }

    ++block;
  }

  if (FLASH_DEVICE_DONE == result)
  {
    if (TRUE == no_bad_blocks)
    {
      FLASHLOG(3, ("No Bad Blocks Detected\n"));
    }

    FLASHLOG(3, ("(2) Set a good block to be BAD\n"));
    result = flash_block_set_state(handle_flash, first_good_block,
      FLASH_BLOCK_BAD);

    if (FLASH_DEVICE_DONE == result)
    {
      if (DAL_SUCCESS == flash_block_get_state(handle_flash,
        first_good_block, &block_state))
      {
        if (FLASH_BLOCK_BAD == block_state)
        {
          FLASHLOG(3, ("Set Block to BAD PASSED\n"));
        }
        else
        {
          FLASHLOG(3, ("Set Block to BAD Failed. Block is Still Good!\n"));
          result = FLASH_DEVICE_FAIL;
        }
      }
      else
      {
        FLASHLOG(3, ("Read of Block State Failed!\n"));
        result = FLASH_DEVICE_FAIL;
      }
    }
    else
    {
      FLASHLOG(3, ("Set Block to BAD Failed!\n"));
      result = FLASH_DEVICE_FAIL;
    }
  }

  if (DAL_SUCCESS == result)
  {
    FLASHLOG(3, ("(3) Set marked block to good again\n"));
    /* Set the Block's state with flash_block_set_state() instead of erasing
     the block because flash_erase_blocks() won't update the bad block status
     table */
    result = flash_block_set_state(handle_flash, first_good_block,
      FLASH_BLOCK_OK);

    if (DAL_SUCCESS == result)
    {
      if (DAL_SUCCESS == flash_block_get_state(handle_flash,
        first_good_block, &block_state))
      {
        if (FLASH_BLOCK_OK == block_state)
        {
          FLASHLOG(3, ("Set Block to Good PASSED\n"));
        }
        else
        {
          FLASHLOG(3, ("Set Block to Good Failed. Block is Still Bad!\n"));
          result = FLASH_DEVICE_FAIL;
        }
      }
      else
      {
        FLASHLOG(3, ("Read of Block State Failed!\n"));
        result = FLASH_DEVICE_FAIL;
      }
    }
    else
    {
      FLASHLOG(3, ("Set Block to Good Failed!\n"));
      result = FLASH_DEVICE_FAIL;
    }
  }

  FLASHLOG(3, ("----------------------------\n"));

  return result;
}

/*
 *  This function tests the read and write apis with different page counts and iovec
 *  counts
 */
int flash_test_read_write_pages_api(flash_handle_t handle_flash,
 struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE, erase_status = 0;
  enum flash_block_state block_state;
  enum page_read_opcode read_type = FLASH_READ_MAIN_SPARE;
  enum page_write_opcode write_type = FLASH_WRITE_MAIN_SPARE;
  uint32 block, testcount, compare_count = 0;
  uint32 iovec_count = 0, page_count = 0;
  uint32 page_num = 0, page_num_shift = 0, main_buf_index = 0;
  uint32 spare_buf_index = 0;
  DALSysMemDescBuf *desc_buff;
  boolean seqential_test_mode = FALSE;
  unsigned int offset_inc = 0;
  uint32 cw_count = 0, num_tests;

  FLASHLOG(3, ("----------------------------\n"));
  FLASHLOG(3, ("Testing Page Read/Write APIs\n"));

  /* Find a good block */
  block = 0;
  while (block < info->partition.block_count)
  {
    KICK_WATCHDOG();

    result = flash_block_get_state(handle_flash, block, &block_state);

    if (FLASH_BLOCK_OK == block_state)
    {
      block_vector[0].start_block = block;
      block_vector[0].block_count = 1;
      block_vector[0].result_vector = &erase_status;
      break;
    }

    ++block;
  }

  if ((stress_partition_attributes & 0xFF) == 0x03)
  {
    /* ReadSpare is not supported for 2X mode yet*/
    num_tests = 3;
  }
  else
  {
    num_tests = 4;
  }

  testcount = 0;
  while (++testcount <= num_tests)
  {
    KICK_WATCHDOG();
    /* Write n pages sequentially and read n pages sequentially
      to confirm the computed data */
    switch (testcount)
    {
      case 1:
        FLASHLOG(3, (" - Testing WriteMain + ReadMain\n"));
        write_type = FLASH_WRITE_MAIN;
        read_type = FLASH_READ_MAIN;
        break;
      case 2:
        FLASHLOG(3, (" - Testing WriteMainSpare + ReadMainSpare\n"));
        write_type = FLASH_WRITE_MAIN_SPARE;
        read_type = FLASH_READ_MAIN_SPARE;
        break;
      case 3:
        FLASHLOG(3, (" - Testing WriteRaw + ReadRaw\n"));
        write_type = FLASH_WRITE_RAW;
        read_type = FLASH_READ_RAW;
        break;
      case 4:
        FLASHLOG(3, (" - Testing WriteMainSpare + ReadSpare\n"));
        write_type = FLASH_WRITE_MAIN_SPARE;
        read_type = FLASH_READ_SPARE;
        continue;
    }

    /* This will force v=1 always. To test with different values of V, 
	   set page_count to 1 here */
    page_count = info->partition.pages_per_block;

    FLASHLOG(3, ("  - Write+Read of v iovecs with p pages each,"
      "               "));

    while (page_count <= info->partition.pages_per_block)
    {
      if ((page_count == info->partition.pages_per_block) &&
        (!seqential_test_mode))
      {
        seqential_test_mode = TRUE;
        page_count = 1;
      }

      KICK_WATCHDOG();
      /* start with an erased block */
      result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_erase_blocks of 0x%x block FAILED\n",
          block_vector[0].start_block));
      }

      page_num_shift = block_vector[0].start_block *
        info->partition.pages_per_block;

      iovec_count = 0;
      page_num = page_num_shift;
      main_buf_index = 0;
      spare_buf_index = stress_spare_index;

      switch (write_type)
      {
        case FLASH_WRITE_MAIN_SPARE:
          page_data_iovec.start_page = page_num;
          page_data_iovec.main_size_bytes =
            stress_main_size_bytes;
          page_data_iovec.spare_size_bytes =
            stress_spare_size_bytes;
          break;

        case FLASH_WRITE_MAIN:
          page_data_iovec.start_page = page_num;
          page_data_iovec.main_size_bytes =
            stress_main_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;

        case FLASH_WRITE_RAW:
          page_data_iovec.start_page = page_num;
          page_data_iovec.main_size_bytes =
            info->partition.total_page_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;
      }

      if (seqential_test_mode)
      {
        page_data_iovec.total_page_count = page_count;
      }
      else
      {
        page_data_iovec.total_page_count = info->partition.pages_per_block;
      }

      memset (page_write_raw_buffer, page_count, STRESS_BUFFER_SIZE);

      if (!run_2x_test)
      {
        fill_buffer_with_patterns(page_count, page_data_iovec.start_page,
          write_type);
      }
      else
      {
        fill_2x_buffer_with_patterns(page_count, page_data_iovec.start_page,
          write_type);
      }

      stress_buff_desc.VirtualAddr = (uint32) &page_write_raw_buffer[0];

      desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *)
        &stress_buff_desc, 0);

      /* Compute and init write data for the count pages */
      while (page_num < (page_num_shift + info->partition.pages_per_block))
      {
        switch (write_type)
        {
          case FLASH_WRITE_MAIN_SPARE:

            desc_buff->VirtualAddr = (uint32)
              &page_write_raw_buffer[main_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = stress_main_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            desc_buff->VirtualAddr =
              (uint32) &page_write_raw_buffer[spare_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = stress_spare_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            main_buf_index  += stress_main_size_bytes * page_count;
            spare_buf_index += stress_spare_size_bytes * page_count;
            break;
          case FLASH_WRITE_MAIN:

            desc_buff->VirtualAddr = (uint32)
              &page_write_raw_buffer[main_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = stress_main_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            desc_buff->VirtualAddr = 0;
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = 0;
            desc_buff->user = page_count;

            desc_buff++;

            main_buf_index  += stress_main_size_bytes * page_count;
            break;
          case FLASH_WRITE_RAW:

            desc_buff->VirtualAddr =
              (uint32) &page_write_raw_buffer[main_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size =
              info->partition.total_page_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            desc_buff->VirtualAddr = 0;
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = 0;
            desc_buff->user = page_count;

            desc_buff++;

            main_buf_index  +=
              info->partition.total_page_size_bytes * page_count;
            break;
        }

        page_num += page_count;
        iovec_count++;
        if (seqential_test_mode)
        {
          break;
        }
      }/*  while (page_num < (page_num_shift + info->partition.pages_per_block))*/

      FLASHLOG(3, ("\b\b\b\b\b\b\b\b\b\b\b\b\bv = %2d p = %2d",
        iovec_count, page_count));

      KICK_WATCHDOG();

      stress_buff_desc.dwNumDescBufs = iovec_count * 2;

      result = flash_write_pages(handle_flash, write_type,
        (DALSysMemDescList *) &stress_buff_desc,
        &page_data_iovec);

      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_write_pages of %d pages failed\n", page_count));
        break;
      }

      iovec_count = 0;
      page_num = page_num_shift;
      main_buf_index = 0;
      spare_buf_index = stress_spare_index;
      desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *)
        &stress_buff_desc, 0);

      switch (read_type)
      {
        case FLASH_READ_MAIN_SPARE:
          page_data_iovec.start_page = page_num;
          page_data_iovec.main_size_bytes =
            stress_main_size_bytes;
          page_data_iovec.spare_size_bytes =
            stress_spare_size_bytes;
          break;

        case FLASH_READ_MAIN:
          page_data_iovec.start_page = page_num;
          page_data_iovec.main_size_bytes =
            stress_main_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;

        case FLASH_READ_SPARE:
          page_data_iovec.start_page = page_num;
          page_data_iovec.main_size_bytes = 0;
          page_data_iovec.spare_size_bytes =
            stress_spare_size_bytes;
          break;

        case FLASH_READ_RAW:
          page_data_iovec.start_page = page_num;
          page_data_iovec.main_size_bytes =
            info->partition.total_page_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;
      }

      if (seqential_test_mode)
      {
        page_data_iovec.total_page_count = page_count;
      }
      else
      {
        page_data_iovec.total_page_count = info->partition.pages_per_block;
      }

      memset (page_read_raw_buffer, 0, STRESS_BUFFER_SIZE);

      stress_buff_desc.VirtualAddr = (uint32) &page_read_raw_buffer[0];

      /* init read data for the count pages */
      while (page_num < (page_num_shift + info->partition.pages_per_block))
      {
        switch (read_type)
        {
          case FLASH_READ_MAIN_SPARE:
            desc_buff->VirtualAddr =
              (uint32) &page_read_raw_buffer[main_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = stress_main_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            desc_buff->VirtualAddr =
              (uint32) &page_read_raw_buffer[spare_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = stress_spare_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            main_buf_index  += stress_main_size_bytes * page_count;
            spare_buf_index += stress_spare_size_bytes * page_count;
            break;
          case FLASH_READ_MAIN:

            desc_buff->VirtualAddr =
              (uint32) &page_read_raw_buffer[main_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = stress_main_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            desc_buff->VirtualAddr = 0;
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = 0;
            desc_buff->user = page_count;

            desc_buff++;

            main_buf_index  += stress_main_size_bytes * page_count;
            break;
          case FLASH_READ_SPARE:

            desc_buff->VirtualAddr = 0;
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = 0;
            desc_buff->user = page_count;

            desc_buff++;

            desc_buff->VirtualAddr =
              (uint32) &page_read_raw_buffer[spare_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = stress_spare_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            spare_buf_index += stress_spare_size_bytes * page_count;
            break;
          case FLASH_READ_RAW:

            desc_buff->VirtualAddr =
              (uint32) &page_read_raw_buffer[main_buf_index];
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size =
              info->partition.total_page_size_bytes * page_count;
            desc_buff->user = page_count;

            desc_buff++;

            desc_buff->VirtualAddr = 0;
            desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
            desc_buff->size = 0;
            desc_buff->user = page_count;

            desc_buff++;

            main_buf_index  +=
              info->partition.total_page_size_bytes * page_count;
            break;
        }
        page_num += page_count;
        iovec_count++;
        if (seqential_test_mode)
        {
          break;
        }
      }/* while (page_num < (page_num_shift + info->partition.pages_per_block))*/

      KICK_WATCHDOG();

      stress_buff_desc.dwNumDescBufs = iovec_count * 2;
      result = flash_read_pages(handle_flash, read_type,
        (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_read_pages of %d pages failed\n", page_count));
        break;
      }

      compare_count = (seqential_test_mode) ? page_count :
        info->partition.pages_per_block;

      switch (read_type)
      {
        case FLASH_READ_MAIN_SPARE:
          /* Compare spare bytes */
         if (FLASH_ONENAND == Info.type)
         {
           if ( stress_spare_size_bytes ==
             (Info.partition.total_page_size_bytes -
             Info.partition.page_size_bytes))
           {
             if (FLASH_NAND_PAGE_SIZE_2048 < Info.partition.page_size_bytes)
             {
               ecc_num_cw = FLASH_ONENAND_4K_SPARE_NUM_CW;
               ecc_offset = stress_spare_index +
                 FLASH_ONENAND_4K_SPARE_ECCM_BOFF;
               ecc_len = FLASH_ONENAND_4K_SPARE_ECCM_BLEN;
             }
             else
             {
               ecc_num_cw = FLASH_ONENAND_2K_SPARE_NUM_CW;
               ecc_offset = stress_spare_index +
                 FLASH_ONENAND_2K_SPARE_ECCM_BOFF;
               ecc_len = FLASH_ONENAND_2K_SPARE_ECCM_BLEN +
                 FLASH_ONENAND_2K_SPARE_ECCS_BLEN;
             }

             /* Since the OneNAND write spare has the ECC bytes
              * filled with pattern, fill the ECC data with pattern in
              * the read buffer before comparison
              * is performed.
              */
              for (cw_count=0,offset_inc=0;
                cw_count < (compare_count*ecc_num_cw); cw_count++)
              {
                memset((void *)&page_read_raw_buffer[ecc_offset + offset_inc],
                  page_count, ecc_len);
                offset_inc += 16;
              }
            }
          }
          if (memcmp(&page_read_raw_buffer[stress_spare_index],
            &page_write_raw_buffer[stress_spare_index],
            (compare_count * stress_spare_size_bytes)) != 0)
          {
            FLASHLOG(3, ("Spare Data compare failed\n"));
            break;
          }
        case FLASH_READ_MAIN:
          /* Compare main bytes */
          if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0],
            (compare_count * stress_main_size_bytes)) != 0)
          {
            FLASHLOG(3, ("Main Data compare failed\n"));
          }
         break;
        case FLASH_READ_SPARE:
          /* Compare spare bytes */
          if (FLASH_ONENAND == Info.type)
          {
            if ( stress_spare_size_bytes ==
              (Info.partition.total_page_size_bytes -
              Info.partition.page_size_bytes))
            {
              if (FLASH_NAND_PAGE_SIZE_2048 < Info.partition.page_size_bytes)
              {
                ecc_num_cw = FLASH_ONENAND_4K_SPARE_NUM_CW;
                ecc_offset = stress_spare_index +
                  FLASH_ONENAND_4K_SPARE_ECCM_BOFF;
                  ecc_len = FLASH_ONENAND_4K_SPARE_ECCM_BLEN;
              }
              else
              {
                ecc_num_cw = FLASH_ONENAND_2K_SPARE_NUM_CW;
                ecc_offset = stress_spare_index +
                  FLASH_ONENAND_2K_SPARE_ECCM_BOFF;
                ecc_len = FLASH_ONENAND_2K_SPARE_ECCM_BLEN +
                  FLASH_ONENAND_2K_SPARE_ECCS_BLEN;
              }
              /* Since the OneNAND write spare has the ECC bytes
               * filled with pattern, fill the ECC data with pattern in
               * the read buffer before comparison
               * is performed.
               */
              for (cw_count=0,offset_inc=0;
                cw_count < (compare_count*ecc_num_cw); cw_count++)
              {
                memset((void *)&page_read_raw_buffer[ecc_offset + offset_inc],
                  page_count, ecc_len);
                offset_inc += 16;
              }
            }
          }

          /* Compare spare bytes */
          if (memcmp(&page_read_raw_buffer[stress_spare_index],
            &page_write_raw_buffer[stress_spare_index],
            (compare_count * stress_spare_size_bytes)) != 0)
          {
            FLASHLOG(3, ("Spare Data compare failed\n"));
          }
          break;
        case FLASH_READ_RAW:
          if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0],
            (compare_count * stress_main_size_bytes)) != 0)
          {
            FLASHLOG(3, ("Main Data compare failed\n"));
          }
          break;
      }/* switch (read_type) */

      if (!seqential_test_mode)
      {
        page_count = page_count << 1;
      }
      else
      {
        ++page_count;
        if (page_count > info->partition.pages_per_block)
        {
          seqential_test_mode = FALSE;
        }
      } /* else of if (!seqential_test_mode) */
    }/*  while (page_count <= info->partition.pages_per_block)*/

    FLASHLOG(3, ("  \n"));
  }/* while (++testcount <= 4)*/

  KICK_WATCHDOG();

  /* End with an erased block */
  result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, ("flash_erase_blocks of 0x%x block\n",
      block_vector[0].start_block));
  }

  FLASHLOG(3, ("----------------------------\n"));
  return result;
}

/*
 *  This function tests the read and write apis with different main and spare byte sizes
 */
int flash_test_read_write_bytes_api(flash_handle_t handle_flash, struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE, erase_status;
  enum flash_block_state block_state;
  enum page_read_opcode read_type = FLASH_READ_MAIN_SPARE;
  enum page_write_opcode write_type = FLASH_WRITE_MAIN_SPARE;
  uint32 block, testcount = 0, spare_len, num_tests;
  DALSysMemDescBuf *desc_buff;

  FLASHLOG(3, ("----------------------------\n"));
  FLASHLOG(3, ("Testing Page Read/Write APIs \n"
               "with Variable main and spare length\n"));

  stress_buff_desc.dwNumDescBufs = 2;

  /* Find a good block */
  block = 0;
  while (block < info->partition.block_count)
  {
    KICK_WATCHDOG();
    result = flash_block_get_state(handle_flash, block, &block_state);
    if (FLASH_BLOCK_OK == block_state)
    {
      block_vector[0].start_block = block;
      block_vector[0].block_count = 1;
      block_vector[0].result_vector = &erase_status;
      break;
    }
    ++block;
  }

  if ((stress_partition_attributes & 0xFF) == 0x03)
  {
    /* ReadSpare is not supported for 2X mode yet*/
    num_tests = 3;
  }
  else if (stress_partition_attributes == 0x00FF01FF)
  {
    /* No need to test case 5 : WriteSpare + ReadSpare when
       ECC is enabled on both Main and Spare */
    num_tests = 4;
  }
  else
  {
    num_tests = 5;
  }
  while (++testcount <= num_tests)
  {
    uint32 max_page_size_bytes = stress_main_size_bytes;

    KICK_WATCHDOG();
    /* Write n pages sequentially and read n pages sequentially
      to confirm the computed data */
    switch (testcount)
    {
      case 1:
        FLASHLOG(3, (" - Testing WriteMain + ReadMain\n"));
        write_type = FLASH_WRITE_MAIN;
        read_type = FLASH_READ_MAIN;
        break;
      case 2:
        FLASHLOG(3, (" - Testing WriteMainSpare + ReadMainSpare\n"));
        write_type = FLASH_WRITE_MAIN_SPARE;
        read_type = FLASH_READ_MAIN_SPARE;
        break;
      case 3:
        FLASHLOG(3, (" - Testing WriteRaw + ReadRaw\n"));
        write_type = FLASH_WRITE_RAW;
        read_type = FLASH_READ_RAW;
        max_page_size_bytes = stress_total_size_bytes;
        break;
      case 4:
        FLASHLOG(3, (" - Testing WriteMainSpare + ReadSpare\n"));
        write_type = FLASH_WRITE_MAIN_SPARE;
        read_type = FLASH_READ_SPARE;
        continue;
      case 5:
        FLASHLOG(3, (" - Testing WriteSpare + ReadSpare\n"));
        write_type = FLASH_WRITE_SPARE;
        read_type = FLASH_READ_SPARE;
		continue;
    }

    spare_len = 0;

    FLASHLOG(3, ("  - Write+Read of 1 iovec, m main + s spare bytes,"
      "                   "));

    page_data_iovec.start_page = block * info->partition.pages_per_block;
    page_data_iovec.total_page_count = 1;

    while (spare_len <= stress_spare_size_bytes)
    {
      KICK_WATCHDOG();

      /* start with an erased block */
      result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_erase_blocks of 0x%x block\n",
          block_vector[0].start_block));
      }

      /* Compute and init write data for the count pages */
      memset(&page_write_raw_buffer[0], spare_len+1, stress_total_size_bytes);
      stress_buff_desc.VirtualAddr = (uint32) &page_write_raw_buffer[0];

      desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

      switch (write_type)
      {
        case FLASH_WRITE_MAIN_SPARE:

          desc_buff->VirtualAddr = (uint32) &page_write_raw_buffer[0];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = max_page_size_bytes;
          desc_buff->user = 1;

          desc_buff++;

          desc_buff->VirtualAddr =
            (uint32) &page_write_raw_buffer[info->partition.page_size_bytes];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = spare_len;
          desc_buff->user = 1;

          page_data_iovec.main_size_bytes = max_page_size_bytes;
          page_data_iovec.spare_size_bytes = spare_len;
          break;
        case FLASH_WRITE_MAIN:

          desc_buff->VirtualAddr = (uint32) &page_write_raw_buffer[0];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = max_page_size_bytes;
          desc_buff->user = 1;

          desc_buff++;

          desc_buff->VirtualAddr = 0;
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = 0;
          desc_buff->user = 1;

          page_data_iovec.main_size_bytes = max_page_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;
        case FLASH_WRITE_RAW:

          desc_buff->VirtualAddr = (uint32) &page_write_raw_buffer[0];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = max_page_size_bytes;
          desc_buff->user = 1;

          desc_buff++;

          desc_buff->VirtualAddr = 0;
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = 0;
          desc_buff->user = 1;

          page_data_iovec.main_size_bytes = max_page_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;
        case FLASH_WRITE_SPARE:

          desc_buff->VirtualAddr = 0;
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = 0;
          desc_buff->user = 1;

          desc_buff++;

          desc_buff->VirtualAddr =
            (uint32) &page_write_raw_buffer[info->partition.page_size_bytes];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = spare_len;
          desc_buff->user = 1;

          page_data_iovec.main_size_bytes = 0;
          page_data_iovec.spare_size_bytes = spare_len;
          break;
      }

      FLASHLOG(3, ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bm = %4d s = %2d",
        page_data_iovec.main_size_bytes,
        page_data_iovec.spare_size_bytes));

      result = flash_write_pages(handle_flash, write_type,
        (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_write_pages with length %d failed\n", max_page_size_bytes));
        break;
      }

      /* init read data for the count pages */
      memset(&page_read_raw_buffer[0], 0x0, stress_total_size_bytes);
      stress_buff_desc.VirtualAddr = (uint32) &page_read_raw_buffer[0];
      desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

      switch (read_type)
      {
        case FLASH_READ_MAIN_SPARE:

          desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = max_page_size_bytes;
          desc_buff->user = 1;

          desc_buff++;

          desc_buff->VirtualAddr =
            (uint32) &page_read_raw_buffer[info->partition.page_size_bytes];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = spare_len;
          desc_buff->user = 1;

          page_data_iovec.main_size_bytes = max_page_size_bytes;
          page_data_iovec.spare_size_bytes = spare_len;
          break;
        case FLASH_READ_MAIN:

          desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = max_page_size_bytes;
          desc_buff->user = 1;

          desc_buff++;

          desc_buff->VirtualAddr = 0;
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = 0;
          desc_buff->user = 1;

          page_data_iovec.main_size_bytes = max_page_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;
        case FLASH_READ_SPARE:

          desc_buff->VirtualAddr = 0;
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = 0;
          desc_buff->user = 1;

          desc_buff++;

          desc_buff->VirtualAddr =
            (uint32) &page_read_raw_buffer[info->partition.page_size_bytes];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = spare_len;
          desc_buff->user = 1;

          page_data_iovec.main_size_bytes = 0;
          page_data_iovec.spare_size_bytes = spare_len;
          break;
        case FLASH_READ_RAW:

          desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = max_page_size_bytes;
          desc_buff->user = 1;

          desc_buff++;

          desc_buff->VirtualAddr = 0;
          desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
          desc_buff->size = 0;
          desc_buff->user = 1;

          page_data_iovec.main_size_bytes = max_page_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;
      }

      result = flash_read_pages(handle_flash, read_type,
        (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_read_pages with length %d failed\n", max_page_size_bytes));
        break;
      }

      switch (read_type)
      {
        case FLASH_READ_MAIN_SPARE:
          /* Spare Bytes */
          if (memcmp(&page_read_raw_buffer[info->partition.page_size_bytes],
              &page_write_raw_buffer[info->partition.page_size_bytes],
              spare_len) != 0)
          {
            FLASHLOG(3, ("Spare Data compare failed\n"));
            result = FLASH_DEVICE_FAIL;
            break;
          }
        case FLASH_READ_MAIN:
          /* Compare Main data */
          if (memcmp(&page_read_raw_buffer[0],
            &page_write_raw_buffer[0], max_page_size_bytes) != 0)
          {
            FLASHLOG(3, ("Main Data compare failed\n"));
            result = FLASH_DEVICE_FAIL;
            break;
          }
          break;
        case FLASH_READ_SPARE:
          /* Spare Bytes */
          if (memcmp(&page_read_raw_buffer[info->partition.page_size_bytes],
              &page_write_raw_buffer[info->partition.page_size_bytes],
              spare_len) != 0)
          {
            FLASHLOG(3, ("Spare Data compare failed\n"));
            result = FLASH_DEVICE_FAIL;
            break;
          }
          break;
        case FLASH_READ_RAW:
          /* Compare Main data */
          if (memcmp(&page_read_raw_buffer[0],
            &page_write_raw_buffer[0], max_page_size_bytes) != 0)
          {
            FLASHLOG(3, ("Main Data compare failed\n"));
            result = FLASH_DEVICE_FAIL;
            break;
          }
          break;
      }

      spare_len += 1;
    }

    FLASHLOG(3, ("  \n"));
  }/* while (++testcount <= 4) */

  KICK_WATCHDOG();

  /* End with an erased block */
  result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, ("flash_erase_blocks of 0x%x block\n",
      block_vector[0].start_block));
  }

  FLASHLOG(3, ("----------------------------\n"));
  return result;
}


/*
 *  This function tests the read and write apis with different main and spare byte sizes
 */
int flash_test_write_spare_bytes_api(flash_handle_t handle_flash, struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE, erase_status;
  enum flash_block_state block_state;
  uint32 block = 0, test_page = 0, spare_len = 0;
  uint32 main_size = 0, main_size_cmp = 0, curr_page = 0, main_idx = 0;
  DALSysMemDescBuf *desc_buff;


  FLASHLOG(3, ("----------------------------\n"));
  FLASHLOG(3, ("Testing Write and Read Spare APIs \n"));

  stress_buff_desc.dwNumDescBufs = 2;

  /* Find a good block */
  block = 0;
  while (block < info->partition.block_count)
  {
    KICK_WATCHDOG();
    result = flash_block_get_state(handle_flash, block, &block_state);
    if (FLASH_BLOCK_OK == block_state)
    {
      block_vector[0].start_block = block;
      block_vector[0].block_count = 1;
      block_vector[0].result_vector = &erase_status;
      break;
    }
    ++block;
  }

  if (stress_partition_attributes == 0x00FF01FF)
  {
    /* No need to test case 5 : WriteSpare + ReadSpare when
       ECC is enabled on both Main and Spare */
    FLASHLOG(3, ("Write Spare Test : Not Applicable for this partition!\n"));
    return result;
  }

  /* The test includes the below steps,
        - Write to a page with main and spare
        - Read the page with main and spare
        - Write to a different page with only spare
        - Read main and spare and compare.
        - Repeat with different spare length
   */

  /* start with an erased block */
  result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, ("Erasing block 0x%x : FAILED\n",
      block_vector[0].start_block));
  }

  test_page = block * info->partition.pages_per_block;
  curr_page = test_page + 1;

  page_data_iovec.start_page = test_page;
  page_data_iovec.total_page_count = 1;
  page_data_iovec.main_size_bytes = stress_main_size_bytes;
  page_data_iovec.spare_size_bytes = stress_spare_size_bytes;

  /* Compute and init write data for the count pages */
  memset(&page_write_raw_buffer[0], 0xAA, stress_total_size_bytes);
  stress_buff_desc.VirtualAddr = (uint32) &page_write_raw_buffer[0];

  desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

  desc_buff->VirtualAddr = (uint32) &page_write_raw_buffer[0];
  desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
  desc_buff->size = stress_main_size_bytes;
  desc_buff->user = 1;

  desc_buff++;

  desc_buff->VirtualAddr =
    (uint32) &page_write_raw_buffer[stress_main_size_bytes];
  desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
  desc_buff->size = stress_spare_size_bytes;
  desc_buff->user = 1;

  result = flash_write_pages(handle_flash, FLASH_WRITE_MAIN_SPARE,
    (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

  if (DAL_SUCCESS != result)
  {
    FLASHLOG(3, ("flash_write_pages on page %d \n", test_page));
  return result;
}

  memset(&page_write_raw_buffer[0], 0xFF, stress_main_size_bytes);

  FLASHLOG(3, ("  - Write+Read of 1 iovec, s spare bytes, s =    "));

  spare_len = 0;
  while (spare_len <= stress_spare_size_bytes)
  {
    KICK_WATCHDOG();

    FLASHLOG(3, ("\b\b%2d", spare_len));

    /*** Read the test page with main and spare ***/
    page_data_iovec.start_page = test_page;
    page_data_iovec.total_page_count = 1;
    page_data_iovec.main_size_bytes = stress_main_size_bytes;
    page_data_iovec.spare_size_bytes = stress_spare_size_bytes;

    stress_buff_desc.VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

    desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = stress_main_size_bytes;
    desc_buff->user = 1;

    desc_buff++;

    desc_buff->VirtualAddr =
      (uint32) &page_read_raw_buffer[stress_main_size_bytes];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = stress_spare_size_bytes;
    desc_buff->user = 1;

    result = flash_read_pages(handle_flash, FLASH_READ_MAIN_SPARE,
      (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

    if (DAL_SUCCESS != result)
    {
      FLASHLOG(3, ("flash_read_pages, page %d : Failed\n", test_page));
      break;
    }

    /* Write to a different page with only spare */
    memset(&page_write_raw_buffer[stress_main_size_bytes],
      (spare_len+1), stress_spare_size_bytes);

    desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

    desc_buff->VirtualAddr = 0;
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = 0;
    desc_buff->user = 1;

    desc_buff++;

    desc_buff->VirtualAddr =
      (uint32) &page_write_raw_buffer[info->partition.page_size_bytes];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = spare_len;
    desc_buff->user = 1;

    page_data_iovec.start_page = curr_page;
    page_data_iovec.total_page_count = 1;
    page_data_iovec.main_size_bytes = 0;
    page_data_iovec.spare_size_bytes = spare_len;

    result = flash_write_pages(handle_flash, FLASH_WRITE_SPARE,
      (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

    if (DAL_SUCCESS != result)
    {
      FLASHLOG(3, ("flash_write_pages on page %d \n", curr_page));
      return result;
    }

    /* Read back main and compare */
    memset(&page_read_raw_buffer[0], 0x0, stress_total_size_bytes);
    stress_buff_desc.VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

    desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = stress_total_size_bytes;
    desc_buff->user = 1;

    desc_buff++;

    desc_buff->VirtualAddr =
      (uint32) &page_read_raw_buffer[stress_main_size_bytes];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = 0;
    desc_buff->user = 1;

    page_data_iovec.main_size_bytes = stress_total_size_bytes;
    page_data_iovec.spare_size_bytes = 0;

    result = flash_read_pages(handle_flash, FLASH_READ_RAW,
      (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

    if (DAL_SUCCESS != result)
    {
      FLASHLOG(3, ("flash_read_pages, page %d : Failed\n", curr_page));
      break;
    }

    main_size = 0;
    
	if (FLASH_ONENAND == info->type)
	{
	  main_idx = 512;
	}
	else
	{
      main_idx =  (4256 == info->partition.total_page_size_bytes)? 532 : 528;
      main_size_cmp = (FLASH_ECC_MAIN_SPARE_ENABLED == info->partition.ecc_state)?
        516 : 512;
    }
	
    while (main_size < stress_main_size_bytes)
    {
      /* Compare main and spare bytes. All Main and all unwritten spare bytes
         should be 0xFF and written spare bytes should match (spare_len + 1) */
      if ((info->partition.total_page_size_bytes - main_size) == main_idx)
      {
        main_size_cmp = (info->partition.page_size_bytes == 4096)?
          484 : 500;
      }

      if (memcmp(&page_read_raw_buffer[main_size],
          &page_write_raw_buffer[0], main_size_cmp) != 0)
      {
        FLASHLOG(3, ("Main data compare : Failed\n"));
        result = FLASH_DEVICE_FAIL;
        break;
      }

      main_size += main_idx;
    }

    /* Read back spare and compare */
    memset(&page_read_raw_buffer[0], 0x0, stress_total_size_bytes);
    stress_buff_desc.VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

    desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = 0;
    desc_buff->user = 1;

    desc_buff++;

    desc_buff->VirtualAddr =
      (uint32) &page_read_raw_buffer[stress_main_size_bytes];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = stress_spare_size_bytes;
    desc_buff->user = 1;

    page_data_iovec.main_size_bytes = 0;
    page_data_iovec.spare_size_bytes = stress_spare_size_bytes;

    result = flash_read_pages(handle_flash, FLASH_READ_SPARE,
      (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

    if (DAL_SUCCESS != result)
    {
      FLASHLOG(3, ("flash_read_pages, page %d : Failed\n", curr_page));
      break;
    }

    if (memcmp(&page_read_raw_buffer[stress_main_size_bytes],
        &page_write_raw_buffer[stress_main_size_bytes], spare_len) != 0)
    {
      FLASHLOG(3, ("Spare data compare : Failed\n"));
      result = FLASH_DEVICE_FAIL;
      break;
    }

    if (spare_len < stress_spare_size_bytes)
    {
      if (memcmp(&page_read_raw_buffer[stress_main_size_bytes + spare_len],
          &page_write_raw_buffer[0],
          (stress_spare_size_bytes - spare_len)) != 0)
      {
        FLASHLOG(3, ("Spare data compare : Failed\n"));
        result = FLASH_DEVICE_FAIL;
        break;
      }
    }

    curr_page++;
    spare_len++;
  }

  FLASHLOG(3, ("\n----------------------------\n"));
  return result;
}

/*
 *  This function tests the copy api
 */

int flash_test_copy_page_api(flash_handle_t handle_flash,
  struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE, erase_status = 0;
  enum flash_block_state block_state;
  enum page_read_opcode read_type;
  enum page_write_opcode write_type;
  struct flash_copy_iovec copy_op;
  uint32 block, count = 0;
  uint32 good_block[3] = {0x0, 0x0, 0x0};
  uint32 good_block_flag, page0, page1, page2;
  DALSysMemDescBuf *desc_buff;

  FLASHLOG(3, ("----------------------------\n"));
  FLASHLOG(3, ("Testing Copy Page API(s) \n"));
  /* Basic idea of this test is to verify the copy page functionality
     of flash driver

     This test verifies 4 scenarios,

     1. Single page copy within same block
     2. Single page copy in two blocks
     3. Multiple page copy in two blocks in different plane
     4. Multiple page copy in two blocks in same plane

     In this code, block[0] and block[2] are same plane
     and block[1] is in different plane */

  /* Find 3 good blocks from partition */
  block = 0;
  good_block_flag = 0;

  while ((block < info->partition.block_count) &&
         (good_block_flag != 0x7))
  {
    while (block < info->partition.block_count)
    {
      KICK_WATCHDOG();
      result = flash_block_get_state(handle_flash, block, &block_state);
      if (FLASH_BLOCK_OK == block_state)
      {
        block_vector[0].start_block = block;
        block_vector[0].block_count = 1;
        block_vector[0].result_vector = &erase_status;

        if (DAL_SUCCESS != flash_erase_blocks(handle_flash,
          &block_vector[0], 1))
        {
          FLASHLOG(3, ("flash_erase_blocks of 0x%x block failed\n", block));
        }
        else
        {
          FLASHLOG(4, ("Erased block 0x%x\n", block));
          break;
        }
      }
      ++block;
    }

    /* Find two even blocks and one odd block */
    if (block & 0x1)
    {
      if (!(good_block_flag & 0x2))
      {
        good_block[1] = block;
        good_block_flag |= 0x2;
      }
    }
    else
    {
      if (!(good_block_flag & 0x1))
      {
        good_block[0] = block;
        good_block_flag |= 0x1;
      }
      else if (!(good_block_flag & 0x4))
      {
        good_block[2] = block;
        good_block_flag |= 0x4;
      }
    }

    ++block;
  }

  if(good_block_flag != 0x7)
  {
    result = FLASH_DEVICE_FAIL;
    return result;
  }

  page0 = good_block[0] * info->partition.pages_per_block;
  page1 = good_block[1] * info->partition.pages_per_block;
  page2 = good_block[2] * info->partition.pages_per_block;

  read_type = FLASH_READ_MAIN_SPARE;
  write_type = FLASH_WRITE_MAIN_SPARE;

  memset(&page_write_raw_buffer[0], 0xAB, STRESS_BUFFER_SIZE);

  stress_buff_desc.VirtualAddr = (uint32) &page_write_raw_buffer[0];
  desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

  desc_buff->VirtualAddr = (uint32) &page_write_raw_buffer[0];
  desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
  desc_buff->size = info->partition.page_size_bytes;
  desc_buff->user = 1;

  desc_buff++;

  desc_buff->VirtualAddr =
  (uint32) &page_write_raw_buffer[info->partition.page_size_bytes];
  desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
  desc_buff->size = stress_spare_size_bytes;
  desc_buff->user = 1;

  stress_buff_desc.dwNumDescBufs = 2;

  page_data_iovec.start_page = page0;
  page_data_iovec.main_size_bytes = info->partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = stress_spare_size_bytes;
  page_data_iovec.total_page_count = 1;

  result = flash_write_pages(handle_flash, write_type,
    (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

  if (DAL_SUCCESS == result)
  {
    uint32 offset = 0;
    count = info->partition.pages_per_block + 1;

    copy_op.src_page = page0;
    copy_op.dest_page = (page0 + offset);
    copy_op.copy_count = 1;

    while (--count)
    {
      KICK_WATCHDOG();
      copy_op.src_page = page0;
      copy_op.dest_page = (page0 + offset);
      copy_op.copy_count = 1;

      if(copy_op.src_page != copy_op.dest_page)
      {
        result = flash_copy_pages(handle_flash, &copy_op, 1);
      }

      if (DAL_SUCCESS == result)
      {
        copy_op.dest_page = (page1 + offset);
        result = flash_copy_pages(handle_flash, &copy_op, 1);
        if (DAL_SUCCESS != result)
        {
          FLASHLOG(3, ("flash_copy_pages src: 0x%x dest: 0x%x failed\n", page0, (page1 + offset)));
          break;
        }
      }
      else
      {
        FLASHLOG(3, ("flash_copy_pages src: 0x%x dest: 0x%x failed\n", page0, (page0 + offset)));
        break;
      }
      ++offset;
    }


    /* Verify single page copy in the same block */
    memset (page_read_raw_buffer, 0, STRESS_BUFFER_SIZE);

    stress_buff_desc.VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &stress_buff_desc, 0);

    desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = info->partition.page_size_bytes *
      info->partition.pages_per_block;
    desc_buff->user = info->partition.pages_per_block;

    desc_buff++;

    desc_buff->VirtualAddr =
      (uint32) &page_read_raw_buffer[stress_spare_index];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    desc_buff->size = stress_spare_size_bytes *
      info->partition.pages_per_block;
    desc_buff->user = info->partition.pages_per_block;

    stress_buff_desc.dwNumDescBufs = 2;

    page_data_iovec.start_page = page0;
    page_data_iovec.main_size_bytes = stress_main_size_bytes;
    page_data_iovec.spare_size_bytes = stress_spare_size_bytes;
    page_data_iovec.total_page_count = info->partition.pages_per_block;

    result = flash_read_pages(handle_flash, read_type,
      (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

    count = info->partition.pages_per_block;

    if (DAL_SUCCESS != result)
    {
      FLASHLOG(3, ("flash_read_pages of %d pages failed\n", count));
    }
    else
    {
      KICK_WATCHDOG();

      if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0],
        (info->partition.pages_per_block *
        info->partition.page_size_bytes)) != 0)
      {
        FLASHLOG(3, ("Main Data compare failed\n"));
        result = FLASH_DEVICE_FAIL;
      }

      KICK_WATCHDOG();
      if (memcmp(&page_read_raw_buffer[stress_spare_index],
        &page_write_raw_buffer[0], (info->partition.pages_per_block *
        stress_spare_size_bytes)) != 0)
      {
        FLASHLOG(3, ("Spare Data compare failed\n"));
        result = FLASH_DEVICE_FAIL;
      }
    }

    /* Verify single page copy in different blocks */
    KICK_WATCHDOG();
    memset (page_read_raw_buffer, 0, STRESS_BUFFER_SIZE);

    if (FLASH_DEVICE_DONE == result)
    {
      KICK_WATCHDOG();

      FLASHLOG(3, ("Single page copy within same block : PASSED\n"));
      page_data_iovec.start_page = page1;

      result = flash_read_pages(handle_flash, read_type,
        (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_read_pages of %d pages failed\n", count));
      }
      else
      {
        KICK_WATCHDOG();

        if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0],
          (info->partition.pages_per_block *
          info->partition.page_size_bytes)) != 0)
        {
          FLASHLOG(3, ("Main Data compare failed\n"));
          result = FLASH_DEVICE_FAIL;
        }

        KICK_WATCHDOG();
        if (memcmp(&page_read_raw_buffer[stress_spare_index],
          &page_write_raw_buffer[0], (info->partition.pages_per_block *
          stress_spare_size_bytes)) != 0)
        {
          FLASHLOG(3, ("Spare Data compare failed\n"));
          result = FLASH_DEVICE_FAIL;
        }

        KICK_WATCHDOG();
      }
    }

    /* Perform multiple copy */
    if (DAL_SUCCESS == result)
    {
      FLASHLOG(3, ("Single page copy in two blocks in different plane"
        ": PASSED\n", count));

      block_vector[0].start_block = good_block[1];
      block_vector[0].block_count = 1;
      block_vector[0].result_vector = &erase_status;

      result = flash_erase_blocks(handle_flash, &block_vector[0], 1);
      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_erase_blocks of 0x%x block failed\n", good_block[1]));
      }
      else
      {
        FLASHLOG(4, ("Erased block 0x%x\n", good_block[1]));
      }
    }

    /* Verify the multiple copy */
    if (DAL_SUCCESS == result)
    {
      copy_op.src_page = page0;
      copy_op.dest_page = page1;
      copy_op.copy_count = info->partition.pages_per_block;

      result = flash_copy_pages(handle_flash, &copy_op, 1);
      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_copy_pages multiple src: 0x%x dest: 0x%x failed\n",
          page0, page1));
      }
      else
      {
        /* Verify single page copy in different blocks */
        KICK_WATCHDOG();
        memset (page_read_raw_buffer, 0, STRESS_BUFFER_SIZE);

        result = flash_read_pages(handle_flash, read_type,
          (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

        if (DAL_SUCCESS != result)
        {
          FLASHLOG(3, ("flash_read_pages of %d pages failed\n", count));
        }
        else
        {
          KICK_WATCHDOG();

          if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0],
            (info->partition.pages_per_block *
            info->partition.page_size_bytes)) != 0)
          {
            FLASHLOG(3, ("Main Data compare failed\n"));
            result = FLASH_DEVICE_FAIL;
          }

          KICK_WATCHDOG();
          if (memcmp(&page_read_raw_buffer[stress_spare_index],
            &page_write_raw_buffer[0], (info->partition.pages_per_block *
            stress_spare_size_bytes)) != 0)
          {
            FLASHLOG(3, ("Spare Data compare failed\n"));
            result = FLASH_DEVICE_FAIL;
          }

          KICK_WATCHDOG();
        }
      }

      if (DAL_SUCCESS == result)
      {
        FLASHLOG(3, ("Multiple page copy in two blocks in different plane : PASSED\n"));
      }
    }

    /* Multi Copy pages within odd-odd blocks */
    if (DAL_SUCCESS == result)
    {
      copy_op.src_page = page0;
      copy_op.dest_page = page2;
      copy_op.copy_count = info->partition.pages_per_block;

      result = flash_copy_pages(handle_flash, &copy_op, 1);
      if (DAL_SUCCESS != result)
      {
        FLASHLOG(3, ("flash_copy_pages src: 0x%x dest: 0x%x failed\n",
          page0, page2));
      }
      else
      {
        KICK_WATCHDOG();
        memset (page_read_raw_buffer, 0, STRESS_BUFFER_SIZE);
        page_data_iovec.start_page = page2;
        page_data_iovec.total_page_count = info->partition.pages_per_block;

        result = flash_read_pages(handle_flash, read_type,
          (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

        if (DAL_SUCCESS != result)
        {
          FLASHLOG(3, ("flash_read_pages failed\n"));
        }
        else
        {
          KICK_WATCHDOG();

          if (memcmp(&page_read_raw_buffer[0], &page_write_raw_buffer[0],
            (info->partition.pages_per_block *
            info->partition.page_size_bytes)) != 0)
          {
            FLASHLOG(3, ("Main Data compare failed\n"));
            result = FLASH_DEVICE_FAIL;
          }

          KICK_WATCHDOG();
          if (memcmp(&page_read_raw_buffer[stress_spare_index],
            &page_write_raw_buffer[0],
            (info->partition.pages_per_block * stress_spare_size_bytes)) != 0)
          {
            FLASHLOG(3, ("Spare Data compare failed\n"));
            result = FLASH_DEVICE_FAIL;
          }

          KICK_WATCHDOG();
        }
      }
      if (DAL_SUCCESS == result)
      {
        FLASHLOG(3, ("Multiple page copy in two blocks in same plane : PASSED\n"));
      }
    }
  }
  else
  {
    FLASHLOG(3, ("flash_write_pages of 0x%x page failed\n", count));
  }


  FLASHLOG(3, ("----------------------------\n"));

  return result;
}


/*
 *  This function tests the different erased page status check scenarios
 */
int flash_test_erased_page(flash_handle_t handle_flash, struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE, erase_test_status = FLASH_DEVICE_DONE;
  int erase_status;
  enum flash_block_state block_state;
  enum page_read_opcode read_type = FLASH_READ_MAIN_SPARE;
  enum page_write_opcode write_type = FLASH_WRITE_MAIN_SPARE;
  uint32 read_type_count = 0, write_type_count = 0, test_page = 0;
  uint32 block = 0;
  struct flash_page_iovec read_data_iovec;
  DALSysMemDescBuf *desc_buff;

  uint8 read_type_str[][25] = {"'Read Main only'", "'Read Main and Spare'",
    "'Read Spare only'"};

  uint8 write_type_str[][25] = {"'Write Main only'", "'Write Main and Spare'",
                     "'Write Spare only'"};

  FLASHLOG(3, ("----------------------------\n"));
  FLASHLOG(3, ("Testing Erased page check.\n"));

  /* Find a good block */
  block = 0;
  while (block < info->partition.block_count)
  {
    KICK_WATCHDOG();

    result = flash_block_get_state(handle_flash, block, &block_state);

    if (FLASH_BLOCK_OK == block_state)
    {
      block_vector[0].start_block = block;
      block_vector[0].block_count = 1;
      block_vector[0].result_vector = &erase_status;
      break;
    }

    ++block;
  }

  /* Setup read and write iovec with stress_buff_desc. */
  memset (page_write_raw_buffer, 0x55, STRESS_BUFFER_SIZE);

  read_data_iovec.total_page_count = 1;
  page_data_iovec.total_page_count = 1;

  desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *)
    &stress_buff_desc, 0);

  desc_buff->user = 1;
  desc_buff->size = info->partition.total_page_size_bytes;
  (desc_buff + 1)->user = 1;
  (desc_buff + 1)->size = stress_spare_size_bytes;

  stress_buff_desc.dwNumDescBufs = 2;

  read_type_count = 0;

  /* For read types READ_MAIN, FLASH_READ_MAIN_SPARE */
  while (read_type_count < 3)
  {
    KICK_WATCHDOG();
    /* Write n pages sequentially and read n pages sequentially
      to confirm the computed data */
    switch (read_type_count)
    {
      case 0:
        read_type = FLASH_READ_MAIN;
        read_data_iovec.main_size_bytes = stress_main_size_bytes;
        read_data_iovec.spare_size_bytes = 0;
        break;
      case 1:
        read_type = FLASH_READ_MAIN_SPARE;
        read_data_iovec.main_size_bytes = stress_main_size_bytes;
        read_data_iovec.spare_size_bytes = stress_spare_size_bytes;
        break;
      case 3:
        read_type = FLASH_READ_SPARE;
        read_data_iovec.main_size_bytes = 0;
        read_data_iovec.spare_size_bytes = stress_spare_size_bytes;
        break;
    }

    /* start with an erased block */
    result = flash_erase_blocks(handle_flash, &block_vector[0], 1);

    if (DAL_SUCCESS != result)
    {
      FLASHLOG(3, ("flash_erase_blocks of 0x%x block - FAILED.\n",
        block_vector[0].start_block));
      erase_test_status = FLASH_DEVICE_FAIL;
      break;
    }

    FLASHLOG(3, ("\nTesting %s \n", read_type_str[read_type_count]));

    test_page = block_vector[0].start_block * info->partition.pages_per_block;

    memset (page_read_raw_buffer, 0, STRESS_BUFFER_SIZE);

    read_data_iovec.start_page = test_page;
    stress_buff_desc.VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
    desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
    (desc_buff + 1)->VirtualAddr = (uint32) &page_read_raw_buffer[stress_spare_index];
    (desc_buff + 1)->PhysicalAddr = (desc_buff + 1)->VirtualAddr;

    /* Read the erased page and verify */
    result = flash_read_pages(handle_flash, read_type,
      (DALSysMemDescList *) &stress_buff_desc, &read_data_iovec);

    if (result != FLASH_DEVICE_FAIL_PAGE_ERASED)
    {
      FLASHLOG(3, (" - Read an erased page but result does not show erased- FAILED\n"));
      erase_test_status = FLASH_DEVICE_FAIL;
    }
    else
    {
      FLASHLOG(3, (" - Read an erased page, result shows erased - PASSED\n"));
    }

    write_type_count = 0;

    while ((write_type_count < 3) &&
           (FLASH_DEVICE_DONE == erase_test_status))
    {
      KICK_WATCHDOG();
      /* Write n pages sequentially and read n pages sequentially
        to confirm the computed data */
      switch (write_type_count)
      {
        case 0:
          write_type = FLASH_WRITE_MAIN;
          page_data_iovec.main_size_bytes = stress_main_size_bytes;
          page_data_iovec.spare_size_bytes = 0;
          break;
        case 1:
          write_type = FLASH_WRITE_MAIN_SPARE;
          page_data_iovec.main_size_bytes = stress_main_size_bytes;
          page_data_iovec.spare_size_bytes = stress_spare_size_bytes;
          break;
        case 2:
          write_type = FLASH_WRITE_SPARE;
          page_data_iovec.main_size_bytes = 0;
          page_data_iovec.spare_size_bytes = stress_spare_size_bytes;
          break;
      }

      if ((FLASH_WRITE_SPARE == write_type) &&
         (FLASH_ECC_MAIN_SPARE_ENABLED == info->partition.ecc_state))
      {
        write_type_count++;
        continue;
      }

      FLASHLOG(3, ("  - Read after %s", write_type_str[write_type_count]));

      page_data_iovec.start_page = test_page;

      stress_buff_desc.VirtualAddr = (uint32) &page_write_raw_buffer[0];

      desc_buff->VirtualAddr = (uint32) &page_write_raw_buffer[0];
      desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
      (desc_buff + 1)->VirtualAddr = (uint32) &page_write_raw_buffer[stress_spare_index];
      (desc_buff + 1)->PhysicalAddr = (desc_buff + 1)->VirtualAddr;

      result = flash_write_pages(handle_flash, write_type,
        (DALSysMemDescList *) &stress_buff_desc, &page_data_iovec);

      if (FLASH_DEVICE_DONE != result)
      {
        FLASHLOG(3, (" : FAILED\n"));

        erase_test_status = FLASH_DEVICE_FAIL;
        break;
      }

      /* Readback the page and see that it is not erased */
      memset (page_read_raw_buffer, 0, STRESS_BUFFER_SIZE);

      read_data_iovec.start_page = test_page;

      stress_buff_desc.VirtualAddr = (uint32) &page_read_raw_buffer[0];

      desc_buff->VirtualAddr = (uint32) &page_read_raw_buffer[0];
      desc_buff->PhysicalAddr = desc_buff->VirtualAddr;
      (desc_buff + 1)->VirtualAddr = (uint32) &page_read_raw_buffer[stress_spare_index];
      (desc_buff + 1)->PhysicalAddr = (desc_buff + 1)->VirtualAddr;

      /* Read the erased page and verify */
      result = flash_read_pages(handle_flash, read_type,
        (DALSysMemDescList *) &stress_buff_desc, &read_data_iovec);

      if (result == FLASH_DEVICE_FAIL_PAGE_ERASED)
      {
        FLASHLOG(3, (" - FAILED. Page reported as ERASED\n"));
        erase_test_status = FLASH_DEVICE_FAIL;
        break;
      }
      else
      {
        FLASHLOG(3, (" - PASSED. Page reported as NOT ERASED\n"));
      }

      test_page++;
      write_type_count++;
    }
    read_type_count++;
  }

  if (FLASH_DEVICE_DONE == erase_test_status)
  {
     FLASHLOG(3, ("\nAll Flash Erased page check - PASSED\n\n"));
  }
  else
  {
    FLASHLOG(3, ("\nAll Flash Erased page check - FAILED\n"));
  }

  FLASHLOG(3, ("----------------------------\n"));
  return erase_test_status;
}


/*
 * Test for flash get UUID
 */
int flash_test_get_uuid(flash_handle_t handle_flash, struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE;
  uint8 *uuid_data;
  uint32 uuid_len = 0;

  FLASHLOG(3, ("----------------------------\n"));
  FLASHLOG(3, ("Testing Get UUID \n"));

  if (info->partition.page_size_bytes == 512)
  {
    FLASHLOG(3, ("\n No support for get UUID on 512B NAND\n"));
    FLASHLOG(3, ("\n----------------------------\n"));
    return result;
  }

  uuid_data = &page_read_raw_buffer[0];

  memset (&page_read_raw_buffer[stress_spare_index], 0xFF, NAND_STRESS_UUID_BUFF_LEN);

  result = flash_get_uuid(handle_flash, uuid_data,
    NAND_STRESS_UUID_BUFF_LEN, &uuid_len);

  if(FLASH_DEVICE_DONE == result)
  {
    if (memcmp(&page_read_raw_buffer[stress_spare_index],
      &page_write_raw_buffer[0], NAND_STRESS_UUID_BUFF_LEN ) == 0)
    {
      result = FLASH_DEVICE_FAIL;
    }

    if (uuid_len == 0)
    {
      result = FLASH_DEVICE_FAIL;
    }
  }

  if(FLASH_DEVICE_DONE == result)
  {
    int i = 0;
    FLASHLOG(3, ("\n Flash Get UUID - PASSED\n"));

    FLASHLOG(3, ("  UUID Length = %d\n", uuid_len));
    FLASHLOG(3, ("  UUID = "));

    while (i < uuid_len)
    {
      FLASHLOG(3, ("0x%-02x ", page_read_raw_buffer[i]));
      i++;
    }
  }
  else
  {
    FLASHLOG(3, ("\n Flash Get UUID - FAILED\n"));
  }

  FLASHLOG(3, ("\n----------------------------\n"));

  return result;
}

/*
 * Tests flash get info
 */
int flash_test_get_info_api(flash_handle_t handle_flash, struct flash_info *info)
{
  int result = FLASH_DEVICE_DONE;
  uint32 default_spare_size = 0;

  result = flash_get_info(handle_flash, FLASH_DEVICE_INFO, info);
  
  /* If the total page size is more than 4256(4096 + 160) bytes, set the 
   * total page size as 4256 as that will be the max size used as per our 
   * nand page layout. If the total page size is more than 2128(2048 + 80) bytes 
   * and less than 4224(4096 + 128) then the device is a 2K Page 8 Bit ECC device 
   * and the max page size we support as per our page layout is 2128(2048 + 80) bytes.
   */
  if(4256 < info->partition.total_page_size_bytes)
  {
    info->partition.total_page_size_bytes = 4256;
  } 	
  else if ((2128 < info->partition.total_page_size_bytes) && 
    (4224 > info->partition.total_page_size_bytes))
  {
    info->partition.total_page_size_bytes = 2128;
  }

  FLASHLOG(3, ("-------------------\n"));
  FLASHLOG(3, ("Testing GetInfo API\n"));

  if(DAL_SUCCESS == result)
  {
    FLASHLOG(3, ("- dal_flash_GetInfo successful\n"));

    FLASHLOG(3, ("  Device Name = %s \n",info->device_name));
    FLASHLOG(3, ("  Device Type = 0x%x \n",info->type));
    FLASHLOG(3, ("  Device Bits Per Cell = 0x%x \n",info->tech));
    FLASHLOG(3, ("  Device Width = 0x%x \n",info->width));
    FLASHLOG(3, ("  Device Write Style = 0x%x \n",info->write_style));

    FLASHLOG(3, ("  Device Maker ID = 0x%x \n",info->maker_id));
    FLASHLOG(3, ("  Device ID = 0x%x \n",info->device_id));
    FLASHLOG(3, ("  Device Version ID = 0x%x \n",info->version_id));

    FLASHLOG(3, ("  Partition ECC state = 0x%x \n",info->partition.ecc_state));
    FLASHLOG(3, ("  Partition Total Blocks = %d \n",
      info->partition.block_count));
    FLASHLOG(3, ("  Partition Page per Block count = %d \n",
      info->partition.pages_per_block));
    FLASHLOG(3, ("  Partition Page Size = %d bytes \n",
      info->partition.page_size_bytes));
    FLASHLOG(3, ("  Partition Total Page Size = %d bytes \n",
      info->partition.total_page_size_bytes));
    FLASHLOG(3, ("  Partition Max Spare User Data bytes = %d bytes \n",
       info->partition.max_spare_udata_bytes));
    FLASHLOG(3, ("  Partition Physical Start Block = 0x%x \n",
       info->partition.physical_start_block));
    FLASHLOG(3, ("  Partition Attributes = 0x%x \n",
       info->partition.attributes));

    stress_main_size_bytes = info->partition.page_size_bytes;
    stress_total_size_bytes = info->partition.total_page_size_bytes;
    stress_spare_index = info->partition.pages_per_block * stress_main_size_bytes;
    stress_partition_attributes = info->partition.attributes;

    default_spare_size = (stress_main_size_bytes / 512) * 4;

    /* The getinfo API returns the number of spare udata
       bytes covered by ECC. So for a client with ECC enabled
       on main only it returns zero. So for those clients
       defaulting the spare size bytes to 16 */
    stress_spare_size_bytes = (info->partition.max_spare_udata_bytes) ?
      info->partition.max_spare_udata_bytes : default_spare_size;

    if (FLASH_ONENAND == info->type)
    {
      struct onenand_reg_info onenand_info;

      stress_spare_size_bytes = 12;

      result = flash_get_info(handle_flash, FLASH_ONENAND_REG_INFO,
        (void *)&onenand_info);

      if (DAL_SUCCESS == result)
      {
        FLASHLOG(3, ("- dal_flash_GetInfo OneNAND successful\n"));

        FLASHLOG(3, ("  Block Address Reg1 = 0x%x \n",
          onenand_info.block_addr_reg1));
        FLASHLOG(3, ("  Block Address Reg2 = 0x%x \n",
          onenand_info.block_addr_reg2));
        FLASHLOG(3, ("  Protected Register Low Address Region = 0x%x \n",
          onenand_info.prot_reg_low));
        FLASHLOG(3, ("  Protected Register High Address Region = 0x%x \n",
          onenand_info.prot_reg_high));
        FLASHLOG(3, ("  Device Command Register = 0x%x \n",
          onenand_info.cmd_reg));
      }
      else
      {
        FLASHLOG(3, ("- dal_flash_GetInfo OneNAND error\n"));
      }
    }
  }
  else
  {
    FLASHLOG(3, ("- dal_flash_GetInfo error\n"));
  }

  FLASHLOG(3, ("-------------------\n"));

  return result;
}


/*
 *  This function initializes the flash with a test partition table.
 */

int flash_init_test_parti()
{
  char * buf_ptr = (char *) prog_param.data;

  /* Set security mode for the programmer */
  prog_param.status = FLASH_PROG_SECURITY_MODE;
  prog_param.reserved = FLASH_MI_BOOT_SEC_MODE_NON_TRUSTED;

  main_c();

  /* return error code back to TRACE32, 0 = success, 0x100 = fail */
  if (prog_param.status == FLASH_PROG_FAIL)
  {
    return prog_param.status;
  }

  /* Send partition table over to the flash programmer */
  prog_param.status = FLASH_PROG_USR_PARTI_TBL;
  nand_memcpy(buf_ptr, &test_ram_table, sizeof(test_ram_table));
  prog_param.size =  sizeof(test_ram_table);
  prog_param.width = FALSE;
  prog_param.reserved = TRUE;  /* Override */

  main_c();

  /* return error code back to TRACE32, 0 = success, 0x100 = fail */
  if (prog_param.status == FLASH_PROG_FAIL)
  {
    return prog_param.status;
  }

  return FLASH_PROG_SUCCESS;

}

/*
 * Highlevel function for stress test
 */
int flash_run_api_test (flash_handle_t handle_flash)
{
  int status;

  status = flash_test_get_info_api(handle_flash, &Info);

  if (DAL_SUCCESS == status)
  {
    status = flash_test_block_apis(handle_flash, &Info);
  }

  if (DAL_SUCCESS == status)
  {
    status = flash_test_erase_api(handle_flash, &Info);
  }

  /* Please do not alter the sequence of this API call
     as it is intended to call at this place :).
     Get UUID uses slow route in HAL. We need to make
     sure that this API call doesn't mess with the
     optimized route */
  if ((DAL_SUCCESS == status) && (skip_test != TRUE))
  {
    status = flash_test_get_uuid(handle_flash, &Info);
  }

  if (DAL_SUCCESS == status)
  {
    status = flash_test_read_write_pages_api(handle_flash, &Info);
  }

  if (DAL_SUCCESS == status)
  {
    /* This test is not valid when predefined support from
       flash HAL in use */
    status = flash_test_read_write_bytes_api(handle_flash, &Info);
  }

  if (DAL_SUCCESS == status)
  {
    if (FLASH_ONENAND == Info.type)
    {
      FLASHLOG(3, ("Starting Onenand Specific Tests \n"));
      FLASHLOG(3, (" Total Main and spare with ECC enabled \n"));
      /* Set the spare size bytes to total size */
      stress_spare_size_bytes = (Info.partition.total_page_size_bytes -
        Info.partition.page_size_bytes);

      status = flash_test_read_write_pages_api(handle_flash, &Info);

      /* Reset to user data spare write */
      stress_spare_size_bytes = 12;
    }
  }

  if ((DAL_SUCCESS == status) && (!run_2x_test) && (skip_test != TRUE))
  {
    status = flash_test_write_spare_bytes_api(handle_flash, &Info);
  }

  if (DAL_SUCCESS == status)
  {
    status = flash_test_copy_page_api(handle_flash, &Info);
  }

  if (DAL_SUCCESS == status)
  {
    status = flash_test_erased_page(handle_flash, &Info);
  }

  return status;
}

/*
 * Entry point for flash stress test
 */
int flash_dal_stress_test ()
{
  flash_handle_t handle_flash1;
  flash_handle_t handle_flash2;
  uint32 iteration_count = 1;

  DALSYS_InitMod(NULL);

  /* Initialize flash with test partition table*/
  flash_init_test_parti();

  if(DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1, //Device Id
                    &handle_flash1))            //Handle
  {
    FLASHLOG(3, ("Error attaching to device driver \n"));
    return -1;
  }

  FLASHLOG(3, ("Device+Client1 Attached!\n"));

   /*Open 'test' device...*/
  if (DAL_SUCCESS == flash_open_partition(handle_flash1,
    MIBIB_TEST1_PARTI_NAME))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    return -1;
  }

  if(DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1, //Device Id
                    &handle_flash2))            //Handle
  {
    FLASHLOG(3, ("Error attaching to device driver \n"));
    return -1;
  }

  FLASHLOG(3, ("Device+Client2 Attached!\n"));

   /*Open 'test' device...*/
  if (DAL_SUCCESS == flash_open_partition(handle_flash2,
    MIBIB_TEST2_PARTI_NAME))
  {
    FLASHLOG(3, ("flash_open_partition successful\n"));
  }
  else
  {
    FLASHLOG(3, ("flash_open_partition error\n"));
    return -1;
  }
  DALFW_MemDescInit(NULL, (DALSysMemDescList *) &stress_buff_desc, 128);

  while (iteration_count <= stress_test_cnt_max)
  {

    FLASHLOG(3, ("\n\n===================================================\n"));
    FLASHLOG(3, ("\n       TEST RUN: ITERATION 0x%x \n", iteration_count));
    FLASHLOG(3, ("\n===================================================\n\n"));

    if (DAL_SUCCESS == flash_run_api_test(handle_flash1))
    {
      FLASHLOG(3, ("\nAll API tests PASSED with ECC ON MAIN & SPARE "
        " partition 1!\n"));

      if (DAL_SUCCESS == flash_run_api_test(handle_flash2))
      {
        FLASHLOG(3, ("\nAll API tests PASSED with ECC ON MAIN AND SPARE "
          " partition 2!\n"));
      }
      else
      {
        FLASHLOG(3, ("\nAPI tests FAILED with ECC ON MAIN AND SPARE"
          " partition 2!\n"));
        break;
      }
    }
    else
    {
      FLASHLOG(3, ("\nAPI tests FAILED with ECC ON MAIN AND SPARE "
	    " partition 1!\n"));
      break;
    }

    iteration_count++;
  }
  
  flash_device_close((DalDeviceHandle*)handle_flash1);
  flash_device_close((DalDeviceHandle*)handle_flash2);

  flash_device_detach((DalDeviceHandle*)handle_flash2);

  FLASHLOG(3, ("Device+Client2 Detached!\n"));

  flash_device_detach((DalDeviceHandle*)handle_flash1);

  FLASHLOG(3, ("Device+Client1 Detached!\n"));

  flash_prog_deinit_tools();

  return 0;
}

