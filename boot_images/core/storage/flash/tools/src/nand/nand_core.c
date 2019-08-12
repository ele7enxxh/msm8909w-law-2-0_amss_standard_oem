/*=======================================================================
 * FILE:        nand_core.c
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * DESCRIPTION
 *   This file has the main functions used for programming the flash
 *
 * Copyright (c) 2010-2015 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_core.c#1 $
 *   $DateTime: 2015/09/01 00:30:35 $
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/28/15     sb      Cleanup
 * 09/19/14     eo      Add bad block management support
 * 07/14/14     sb      Move page buffers to Code RAM to support non DDR based tools
 * 06/21/13     sb      Clean up as part of removing user partition table from MIBIB 
 * 02/15/13     sv      Clean up
 * 09/11/12     eo      Remove miboot.h
 * 09/05/12     sb      Fixing Klockwork errors
 * 08/16/11     jz      ehostdl image size reduction
 * 04/11/11     jz      Fix factory image programming issue due to array overflow
 * 03/18/11     jz      Check to make sure part tbl size not exceeding page size
 * 02/18/11     bb      Fix for flash.bin programming when ECC on main only
 * 10/09/10     bb      Fix for flash.bin programming when ECC on main and spare
 * 09/14/10     sv      Fix factory image programming issue
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 09/01/10     nr      Defer first block erase in tools till stream write
 * 08/26/10     sb      Add fill bytes to partial page
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 04/12/10     mh      Revert to the latest
 * 04/12/10     mh      Revert to #22 & add dual nand ionterleave mode support
 * 03/12/10     bn      undo changes from version #22 to #23.
 * 03/01/10     op      Magic numbers will be passed in the array params
 * 02/24/10     op      JTAG Downnload flash.bin error on dev with bad blocks
 * 10/14/09     mh      Add support for flashing bootloaders on dual nand devcies
 * 10/12/09     mm      Fix Bad Block Mode setting
 * 10/08/09     op      Make DM buffer 32byte alignment to avoid any misaligned
 * 10/02/09     mm      Added support for programming images in BML Format
 * 07/08/09     mm      Set Error in case of failure of extended partition
 * 07/08/09     mm      Support for programming extended partition table
 * 07/08/09     mm      Factory image programming use main and spare write
 * 05/20/09     jz      More clean up of gcc related compiler warnings
 * 05/07/09     sv      Tools Optimization changes
 * 05/07/09     eo      Clean up gcc related compiler warnings
 * 03/30/09     mh      Add support for 4K page and clean-ups
 * 03/24/09     bb      Removed banned API strncpy
 * 02/14/09     mm      Fix for APPSEFS2RAW programming
 * 02/09/09     jz      Added support for SB2.0
 * 11/20/08     mm      Fix for erase chip block count
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_mibib.h"
#include "flash_micrc.h"
#include "flash_mibbtb.h"

#include "nand_wrapper.h"
#include "nand_core.h"
#include "nand_param.h"
#include "nand_config.h"

/* Utility routine includes */
#include "comdef.h"  /* for ALIGN */


#ifdef BUILD_NAND
/* If building NANDPRG, ap_armprg.c contains the stack */
uint32 svc_stack[0x1000/4];             // main stack
#endif

enum page_write_opcode write_type = FLASH_WRITE_MAIN;
enum page_read_opcode read_type = FLASH_READ_MAIN;

/* Mode for bad block management
 * Skip for Qualcomm bad block management Algorithm
 * Replace for Unistore II Bad Block Algorithm used by Unistore II
 */
flash_prog_bad_block_mode_t current_bad_block_mode = FLASH_PROG_BAD_BLOCK_SKIP;

/* Variable indicating security mode  */
static int security_mode = SEC_MODE_NOT_RCVD;

/* If flash tools get compiles as part of bootloaders, where the data caches is
 * setup, the 32 bytes aligment changes of source buffers here would make them
 * more cache line friendly and avoid any misaligned boot data structure that
 * gets invalidated
 */
#if !defined(BUILD_EHOSTDL) && !defined(BUILD_HOSTDL)
#pragma arm section zidata = "FLASH_TOOLS_HEAP_ZONE"
#endif
ALIGN(ALIGN_VAL) static uint32 magic_buffer[MAX_PAGE_SIZE/(sizeof(uint32))];
ALIGN(ALIGN_VAL)static unsigned char page_buffer[MAX_PAGE_SIZE];

ALIGN(ALIGN_VAL)static unsigned char readback_spare_buf[FLASH_NAND_SPARE_NUM_BYTES];
ALIGN(ALIGN_VAL)static unsigned char spare_buffer[FLASH_NAND_SPARE_NUM_BYTES];
ALIGN(ALIGN_VAL)static unsigned char temp_spare_buff[FLASH_NAND_SPARE_NUM_BYTES];

/* Partition tables from host, MIBIB1, and MIBIB2 and variables to
  * indicate which is in which block and whether we have found them yet.
  */
ALIGN(ALIGN_VAL) unsigned char oldest_parti_buffer[MAX_PAGE_SIZE];

/* Buffer for reading user partition tbl out from flash */
ALIGN(ALIGN_VAL) unsigned char oldest_usr_parti_buffer[MAX_PAGE_SIZE];
ALIGN(ALIGN_VAL) unsigned char partial_page_buffer[TOTAL_PAGE_SIZE];
ALIGN(ALIGN_VAL) unsigned char factory_img_buffer[TOTAL_PAGE_SIZE];
#if !defined(BUILD_EHOSTDL) && !defined(BUILD_HOSTDL)
#pragma arm section zidata
#endif

struct parti_ctl mibib;

int partition_rcvd = FALSE;

int oldest_mibib_block = -1;
int newest_mibib_block = -1;

/* Flag to enable/disable read verfiy during page writes
 * 1 - enable read verify
 * 0 - disable read verify
 * This is set to 0 by default(read verify disabled).
 * This can be set to 1 using Trace32 when read verify needs to be enabled
 * for debug purpose
 */
unsigned int enable_read_verify = 0;

#undef BAD_BLOCK_MGMNT_TEST

#ifdef BAD_BLOCK_MGMNT_TEST
#define BBM_TEST_MAX 4

struct bbm_node
{
  int32 block; /* block # failure is triggered */
  int32 page;  /* # of pages available */
};

struct bbm_node bbm_debug[BBM_TEST_MAX] = 
{
  {1,  0x3b},
  {5,  0x3f},
  {7,  0x40},
  {10, 0x1}
}; 
#endif

int flash_prog_write_current_page (parti_ctl_t pctl, unsigned char *src_main,
    unsigned char *src_spare);
static int flash_prog_prepare_next_block (parti_ctl_t pctl);
static int flash_prog_write_current_page_flash_bin (parti_ctl_t pctl,
  unsigned char *src, unsigned char *spare_bytes);
static int flash_prog_parse_factory_img_header (parti_ctl_t pctl);
static int flash_prog_parse_factory_img_string_table(parti_ctl_t pctl);
static int flash_prog_parse_factory_img_partition_table (parti_ctl_t pctl);
static int flash_prog_bad_block_manager (parti_ctl_t pctl,unsigned char *src_main,
  unsigned char *src_spare);

#ifdef BAD_BLOCK_MGMNT_TEST
static int flash_prog_bbm_test(parti_ctl_t pctl)
{
  int bbm_idx, result = FLASH_DEVICE_DONE;
  
  for (bbm_idx = 0; bbm_idx < BBM_TEST_MAX; bbm_idx++)
  {
    if ((pctl->current_block == bbm_debug[bbm_idx].block) && 
      (pctl->avail_pages_in_block == bbm_debug[bbm_idx].page))
    {
      FLASHLOG(1, ("bbt: pctl->current_block 0x%lx, pctl->current_page 0x%lx\n\n", pctl->current_block, 
        pctl->current_page));
      FLASHLOG(1, ("bbt: pctl->avail_pages_in_block 0x%lx\n", pctl->avail_pages_in_block));
      FLASHLOG(1, ("bbt: pctl->start_addr_loaded 0x%lx\n", pctl->start_addr_loaded));
      FLASHLOG(1, ("bbt: pctl->last_code_addr 0x%lx\n\n", pctl->last_code_addr));

      FLASHLOG(1, ("bbt: bbm_debug[blk] 0x%lx\n", (signed long int)bbm_debug[bbm_idx].block));
      FLASHLOG(1, ("bbt: bbm_debug[pg] 0x%lx\n", (signed long int)bbm_debug[bbm_idx].page));
      result = FLASH_DEVICE_FAIL;
      break;
    }
  }
  return result;
}
#endif

/*
 * Safe string copy function for nand core
 */
static void nand_core_strcpy(uint8 *dest, uint8 *src, uint32 len)
{
  nand_memcpy(dest, src, len);
  dest[len-1] ='\0';
}


/*
 *  This function will erase bad block table. It will find the right block, check
 * the magic numbers to make sure that the block found is indeed bad block table
 * and then erase the block.
 */
int flash_prog_erase_bad_block_table()
{
  int bbt_block;
  uint32 bbt_magic_page;
  typedef struct bad_block_table *bad_block_table_t;
  bad_block_table_t bbt_id;
  int result = FLASH_DEVICE_DONE;
  int erase_result_data;
  struct flash_block_vector block_vector;
  enum flash_block_state blk_state;

  KICK_WATCHDOG();

  /* Find the right block to look for using boot_layout structure */
  bbt_block = flash_prog_get_bbtb_block_from_mibib();
  FLASHLOG(3,("jebb: Finding 0x%xth good block for BBT\n", bbt_block));
  bbt_block =
    (uint32) flash_prog_find_nth_good_block(mibib.flash_handle, bbt_block);

  KICK_WATCHDOG();

  if (bbt_block == -1)
  {
    FLASHLOG(1, ("ebb: Failed to find bad block table Block\n"));
    SET_ERR (ERR_NO_BBT_BLOCK);
    return FLASH_PROG_FAIL;
  }

  /* Check to see if first page is written. If it is, read it into a buffer
   * and match the magic numbers and version number. This is to make sure that
   * we infact are going to erase the bad block table
   */

  bbt_magic_page = (mibib.flash_info.partition.pages_per_block * bbt_block);

  /* We only check for the first page here. If first page is erased, boot loader
   * will nitialize the bad block table anyways. We just return. If the page is
   * not erased, read it in and look for magic and version numbers. If there is
   * a match we erase the block and return.
   */

  result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
    bbt_magic_page, read_type, page_buffer);

  if (FLASH_DEVICE_DONE != result)
  {
    if (FLASH_DEVICE_FAIL_PAGE_ERASED == result)
    {
      FLASHLOG (3, ("ebb: Bad block tbl in block 0x%x is erased\n", bbt_block));
      FLASHLOG (3, ("ebb: Nothing to do\n"));
      return FLASH_PROG_SUCCESS;
    }
    else
    {
      FLASHLOG(1, ("ebb: Read of bad block magic page failed for page 0x%lx\n",
        (unsigned long int)bbt_magic_page));
      SET_ERR (ERR_READ_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  KICK_WATCHDOG();

  bbt_id = (bad_block_table_t) page_buffer;

  if (bbt_id->magic1 != BBTB_MAGIC1)
  {
    FLASHLOG(1, ("ebb: Magic #1 mismatch\n"));
    SET_ERR (ERR_BBT_MAGIC1_FAIL);
    return FLASH_PROG_FAIL;
  }
  else if (bbt_id->magic2 != BBTB_MAGIC2)
  {
    FLASHLOG(1,("ebb: Magic #2 mismatch\n"));
    SET_ERR (ERR_BBT_MAGIC2_FAIL);
    return FLASH_PROG_FAIL;
  }
  else
  {
    if (FLASH_DEVICE_DONE != flash_block_get_state(mibib.flash_handle,
      bbt_block, &blk_state))
    {
      FLASHLOG(1,("ebb: bbt block 0x%x is marked bad\n", bbt_block));
      SET_ERR (ERR_BAD_BLOCK);
      return FLASH_PROG_FAIL;
    }

    if (blk_state == FLASH_BLOCK_BAD)
    {
      return FLASH_PROG_FAIL;
    }

    block_vector.start_block = bbt_block;
    block_vector.block_count = 1;
    block_vector.result_vector = &erase_result_data;
    FLASHLOG(3, ("ebb: Erasing bad_block table in block 0x%x\n", bbt_block));

    if (DAL_SUCCESS != flash_erase_blocks(mibib.flash_handle, &block_vector, 1))
    {
      FLASHLOG(1,("Error: device failed during erase\n"));
      SET_ERR (ERR_ERASE_FAIL);
      return FLASH_PROG_FAIL;
    }

    return FLASH_PROG_SUCCESS;
  }
} /* flash_prog_erase_bad_block_table */


/*
 *
 * This function will write two magic numbers into the current page.
 *
 */
int flash_prog_write_magic_number_page (parti_ctl_t pctl, uint32 * magic_buf,
  int count)
{
  int i;
  /* Initialize the magic page buffer making it deterministic */
  DALSYS_memset((void *)magic_buffer, 0xFF, MAX_PAGE_SIZE);

  for (i=0; i< count; i++)
  {
    magic_buffer[i] = magic_buf[i];
  }

  FLASHLOG (6, ("wmnp:  writing page 0x%lx with magic numbers\n",
    (unsigned long int)pctl->current_page));

  KICK_WATCHDOG();

  return(flash_prog_write_current_page (pctl, (unsigned char *)magic_buffer, NULL));
} /* flash_prog_write_magic_number_page */

/*
 *
 * This function will erase the next available non-bad block in the flash
 * and update all global variables to point to it and its pages as the
 * next available for use.  It will also close the current span and open the
 * next available span if any bad block was encountered.
 */
static int flash_prog_prepare_next_block (parti_ctl_t pctl)
{
  /* Point to the next block */
  pctl->current_block++;

  FLASHLOG( 4, ("Block number %ld\n", (unsigned long int)pctl->current_block));

  /* Call _nand_prepare_this_block() to actually do the next good block
   * find and the erase when found.
   */
  return flash_prog_prepare_this_block(pctl);
} /* flash_prog_prepare_next_block */

/*
 *
 * This function will write the currently full partial page out and
 * update any housekeeping variables as necessary
 */
int flash_prog_write_partial_page_data (parti_ctl_t pctl)
{
  if (FLASH_PROG_SUCCESS != flash_prog_write_current_page (pctl,
    (unsigned char *)pctl->partial_page_buffer, NULL))
  {
    return FLASH_PROG_FAIL;
  }
  pctl->partial_page_index = 0;

  return FLASH_PROG_SUCCESS;
} /* flash_prog_write_partial_page_data */


/*
 *
 * This function will write the current page out and update any housekeeping
 * variables as necessary.
 *
 */
int flash_prog_write_current_page (parti_ctl_t pctl,
  unsigned char *src_main, unsigned char *src_spare )
{
  int result = FLASH_DEVICE_DONE;

  /* If the previous write used the last page in NAND, we should fail here.
   * However, this will probably be caught earlier, as once we write
   * the last page, we will then try to erase the next block of pages
   * and that should fail before we ever get here.  Just being extra safe.
   * If we have no pages available, erase the next block of pages,
   * skipping any bad blocks and failing if we have an erase failure
   * on a supposedly good block, or if we run out of blocks. For the first
   * page write for each image alone erase the current(block 0 or the first
   * non bad block) of the partition
   */
  if (pctl->avail_pages_in_block == 0)
  {
    if (pctl->partition_initial_blk_prepared == FALSE)
    {
      pctl->partition_initial_blk_prepared = TRUE;
      if (FLASH_PROG_SUCCESS != flash_prog_prepare_this_block (pctl))
      {
        pctl->partition_initial_blk_prepared = FALSE;
        return FLASH_PROG_FAIL;
      }
    }
    else
    {
      if (FLASH_PROG_SUCCESS != flash_prog_prepare_next_block (pctl))
      {
        return FLASH_PROG_FAIL;
      }
    }
  }

  FLASHLOG(6, ("wcp:  writing page 0x%lx\n", (unsigned long int)
    pctl->current_page));

  result = flash_prog_write_page_and_udata (pctl->flash_handle, &pctl->flash_info,
    pctl->current_page, write_type, src_main, src_spare,
    pctl->spare_bytes_len);

#ifdef BAD_BLOCK_MGMNT_TEST
  result = flash_prog_bbm_test(pctl);
#endif

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(1,("fmmv: Write of page failed at 0x%lx.\n",
      (unsigned long int)pctl->current_page));

    if (FLASH_PROG_FAIL == flash_prog_bad_block_manager(pctl, src_main, src_spare))
    {
      FLASHLOG(1,("wcp: Bad block management failed \n",
        (unsigned long int)pctl->current_page));
      SET_ERR (ERR_WRITE_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  if (enable_read_verify)
  {
    uint32 i;
    unsigned char *vsrc1, *vsrc2;
    unsigned char *origsrc_main = src_main;
    unsigned char *origsrc_spare = src_spare;
    unsigned int offset_inc = 0;
    uint32 main_bytes_to_compare = 0;

    /* zero out buffer for readback*/
    vsrc1 = page_buffer;
    for (i=0; i<pctl->page_size; i++)
    {
      *vsrc1++ = 0;
    }

    result = flash_prog_read_page_and_udata(pctl->flash_handle, &pctl->flash_info,
    pctl->current_page, read_type, page_buffer, readback_spare_buf,
    pctl->spare_bytes_len);

    if (FLASH_DEVICE_DONE != result)
    {
      if ((FLASH_ONENAND == pctl->flash_info.type) &&
        (FLASH_DEVICE_FAIL_PAGE_ERASED == result))
      {
        FLASHLOG(1,("1NAND: page 0x%lx erased result ignored.\n",
          (unsigned long int)pctl->current_page));
      }
      else
      {
        FLASHLOG(1,("Read page error \n"));
        SET_ERR (ERR_READ_FAIL);
        return FLASH_PROG_FAIL;
      }
    }

    main_bytes_to_compare = pctl->page_size;

    if ((FLASH_ONENAND == pctl->flash_info.type) &&
        ((src_spare != NULL)))
    {
      /* Since the OneNAND factory image has the ECC bytes filled with 0xFF's,
       * fill the ECC data with 0xFF in the readback_spare_buf before comparison
       * is performed.
       */
      for (i=0; i < image.ecc_num_cw; i++)
      {
        memset((void *)&readback_spare_buf[image.ecc_offset + offset_inc],
          0xFF, image.ecc_len);
        offset_inc += 16;
      }
      vsrc1 = origsrc_spare;
      vsrc2 = readback_spare_buf;

      for (i=0; i < pctl->spare_bytes_len; i++)
      {
        if (*vsrc1++ != *vsrc2++)
        {
          FLASHLOG(1,("Verify failed at index %ld\n", (unsigned long int)i));
          FLASHLOG(1,("src at 0x%08X  readback at 0x%08X\n",
            (unsigned int)src_spare, (unsigned int)readback_spare_buf));
          FLASHLOG(1,("Error: verify failure\n"));
          FLASHLOG(1,("fmmv: Page = %ld Block = %ld \n",
            (unsigned long int)pctl->current_page,
            (unsigned long int)pctl->current_block));
          SET_ERR (ERR_VERIFY_FAIL);
          return FLASH_PROG_FAIL;
        }
      }
      main_bytes_to_compare = pctl->flash_info.partition.page_size_bytes;
    }
    vsrc1 = origsrc_main;
    vsrc2 = page_buffer;
    for (i=0; i < main_bytes_to_compare; i++)
    {
      if (*vsrc1++ != *vsrc2++)
      {
        FLASHLOG(1,("Verify failed at index %ld\n", (unsigned long int)i));
        FLASHLOG(1,("src at 0x%08X  readback at 0x%08X\n",
          (unsigned int)src_main, (unsigned int)page_buffer));
        FLASHLOG(1,("Error: verify failure\n"));
        FLASHLOG(1,("fmmv: Page = %ld Block = %ld \n",
          (unsigned long int)pctl->current_page,
          (unsigned long int)pctl->current_block));
        SET_ERR (ERR_VERIFY_FAIL);
        return FLASH_PROG_FAIL;
      }
    }
  } /* if (enable_read_verify)*/

  pctl->current_page++;
  pctl->avail_pages_in_block--;

  return FLASH_PROG_SUCCESS;
} /* flash_prog_write_current_page */


/*===========================================================================

FUNCTION flash_prog_write_current_page_flash_bin

DESCRIPTION
  This function will write the current page and spare user bytes out
  and update any housekeeping variables as necessary.

DEPENDENCIES
  None

RETURN VALUE
  If success, return FLASH_PROG_SUCCESS else FLASH_PROG_FAIL

SIDE EFFECTS

===========================================================================*/
int flash_prog_write_current_page_flash_bin (parti_ctl_t pctl, unsigned char *src,
  unsigned char *spare_bytes)
{
  uint32 i;
  unsigned char *vsrc1, *vsrc2;
  unsigned char *origsrc = src;
//  unsigned char temp_spare_buff[FLASH_NAND_SPARE_NUM_BYTES];
  uint32 index;
  uint32 flash_data;
  int result = FLASH_DEVICE_DONE;

  /* If we have no pages available, erase the next block of pages,
   * skipping any bad blocks and failing if we have an erase failure
   * on a supposedly good block, or if we run out of blocks. For the
   * first page write for each image alone erase the current(block 0
   * or the first non bad block) of the partition
   */
  if (pctl->avail_pages_in_block == 0)
  {
    if (pctl->partition_initial_blk_prepared == FALSE)
    {
      pctl->partition_initial_blk_prepared = TRUE;
      if (FLASH_PROG_SUCCESS != flash_prog_prepare_this_block (pctl))
      {
        pctl->partition_initial_blk_prepared = FALSE;
        return FLASH_PROG_FAIL;
      }
    }
    else
    {
      if (FLASH_PROG_SUCCESS != flash_prog_prepare_next_block (pctl))
      {
        return FLASH_PROG_FAIL;
      }
    }
  }

  FLASHLOG (5, ("wcpfb:  writing page 0x%lx\n", (unsigned long int)
    pctl->current_page));

  /* Modify the spare buffer
   * WM requires only 4 byte/codeword
   * But the 8bit device allows 5 bytes, so re-order the bytes in
   * the spare buffer */
  memset(temp_spare_buff, 0xff, FLASH_NAND_SPARE_NUM_BYTES);
  nand_memcpy(temp_spare_buff, spare_bytes, 8);

  /* Check if the page data is erased */
  for (index = 0; index < pctl->page_size >> 2; index++)
  {
    flash_data = *((uint32 *)(src) + index);
    if (flash_data != 0xffffffff)
    {
      break;
    }
  }
  if (index == pctl->page_size >> 2)
  {
      /* Check if the spare is erased */
      for (index = 0; index < pctl->spare_bytes_len >> 2; index++)
      {
        flash_data = *((uint32 *)(spare_bytes) + index);
        if (flash_data != 0xffffffff)
        {
          break;
        }
      }

      if (index == pctl->spare_bytes_len >> 2)
      {
          /* Entire page is erased skip this page */
          /* Update the variables */
          pctl->current_page++;

          pctl->avail_pages_in_block--;

          /* Check if we have written the entire block */
          if (pctl->avail_pages_in_block == 0 )
          {
            pctl->record_header_parsed = 0;
          }

          /* Update the fact that we need to populate the next page spare buffer */
          pctl->write_spare = 1;

          return FLASH_PROG_SUCCESS;
      }
  }

  result = flash_prog_write_page_and_udata (pctl->flash_handle, &pctl->flash_info,
    pctl->current_page, FLASH_WRITE_MAIN_SPARE, src, temp_spare_buff,
    pctl->spare_bytes_len);

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG (1,("Error: device failed during write\n"));
    SET_ERR (ERR_WRITE_FAIL);
    return FLASH_PROG_FAIL;
  }

  /* zero out readback buffer */
  vsrc1 = page_buffer;
  for (i=0; i<pctl->page_size; i++)
  {
    *vsrc1++ = 0;
  }

  /* zero out spare buffer */
  vsrc1 = spare_buffer;
  for (i=0; i<64; i++)
  {
    *vsrc1++ = 0;
  }

  result = flash_prog_read_page_and_udata(pctl->flash_handle, &pctl->flash_info,
    pctl->current_page, FLASH_READ_MAIN_SPARE, page_buffer, spare_buffer,
    pctl->spare_bytes_len);

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(1,("Error: device failed during read back \n"));
    SET_ERR (ERR_READ_FAIL);
    return FLASH_PROG_FAIL;
  }

  vsrc1 = origsrc;
  vsrc2 = page_buffer;

  /* Verify the spare buffer*/
  for (i=0; i<pctl->page_size; i++)
  {
    if (*vsrc1++ != *vsrc2++)
    {
      FLASHLOG(1,("\nVerify failed at index %ld\n", (unsigned long int)i));
      FLASHLOG(1,("src at 0x%08lX  readback at 0x%08lX\n",
        (unsigned long int)src, (unsigned long int)page_buffer));
      FLASHLOG(1,("Error: verify failure\n"));
      SET_ERR (ERR_VERIFY_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  vsrc1 = temp_spare_buff;
  vsrc2 = spare_buffer;


  for (i=0; i<pctl->spare_bytes_len; i++)
  {
    if (*vsrc1++ != *vsrc2++)
    {
      FLASHLOG(1,("Spare Verify failed at index %ld\n", (unsigned long int)i));
      FLASHLOG(1,("src at 0x%08lX  spare buffer at 0x%08lX\n",
        (unsigned long int)temp_spare_buff, (unsigned long int)spare_buffer));
      FLASHLOG(1,("Error: Spare verify failure\n"));
      SET_ERR (ERR_VERIFY_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  pctl->current_page++;

  pctl->avail_pages_in_block--;

  /* Check if we have written the entire block */
  if (pctl->avail_pages_in_block == 0 )
  {
    pctl->record_header_parsed = 0;
  }

  /* Update the fact that we need to populate the next page spare buffer */
  pctl->write_spare = 1;

  return FLASH_PROG_SUCCESS;
  }

/*
 *
 * This function erases every good block in the partition
 *
 */
int flash_prog_erase_partition(parti_ctl_t pctl)
{
  int blockno;
  int erase_result_data;
  struct flash_block_vector block_vector;
  enum flash_block_state blk_state;

  KICK_WATCHDOG();
  FLASHLOG (3, ("Erasing partition\n"));

  for (blockno=0; blockno<(pctl->flash_info.partition.block_count); blockno++)
  {
    KICK_WATCHDOG();

    if (DAL_SUCCESS != flash_block_get_state(pctl->flash_handle,
      blockno, &blk_state))
    {
      return FLASH_PROG_FAIL;
    }

    /* Advance to next non-bad block */
    if (blk_state == FLASH_BLOCK_BAD)
    {
      FLASHLOG (6, ("\n--- Skipping:  bad block %d\n", blockno));
      continue;
    }
    else
    {
      FLASHLOG (6, ("Erasing good block %d\n", blockno));
    }

    /* erase the block */
    block_vector.start_block = blockno;
    block_vector.block_count = 1;
    block_vector.result_vector = &erase_result_data;

    if (DAL_SUCCESS != flash_erase_blocks(pctl->flash_handle, &block_vector, 1))
    {
      FLASHLOG (1,("Error: device failed during erase\n"));
      SET_ERR (ERR_ERASE_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  KICK_WATCHDOG();

  return FLASH_PROG_SUCCESS;
}  /* flash_prog_erase_partition */


/*
 *
 * This function will erase every block in the flash beginning from
 * some starting block.
 *
 */
uint32 flash_prog_erase_some_of_chip (uint32 start_block)
{
  int blockno;
  int erase_result_data;
  struct flash_block_vector block_vector;
  enum flash_block_state blk_state;

  KICK_WATCHDOG();
  FLASHLOG (1, ("\n\n------ Erase Chip ------\n"));

  /* Open the device if it has not already been opened */
  FLASHLOG (1, ("Probing flash device:  \n"));

  /* Probe will fill values in structure if it succeeds.
   * Currently, which_flash parameter is hard coded to 0, but in
   * future this will be dynamic.
   */
  flash_prog_init_for_partition_image(&mibib, MIBIB_DEVICE_ALL_PARTI_NAME);

  /* If possible use the values already populated in the image structure */
  KICK_WATCHDOG();

  for (blockno=start_block; blockno<mibib.flash_info.partition.block_count;
    blockno++)
  {
    KICK_WATCHDOG();

    if (DAL_SUCCESS != flash_block_get_state(mibib.flash_handle, blockno,
      &blk_state))
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

    if (DAL_SUCCESS != flash_erase_blocks(mibib.flash_handle, &block_vector, 1))
    {
      FLASHLOG (1, ("--- Error: device failed during erase of block 0x%x\n",
        blockno));

      /* Skip erase failures on block 0, which may be due to OTP protection */
      if (blockno == 0)
      {
        FLASHLOG (3, ("\nSkipping erase failure at block 0x%x\n", blockno));
        continue;
      }

      if ((blockno % 512) == 0)
        {
           block_vector.block_count = blockno % 512;
        }

      return FLASH_PROG_FAIL;
    }
  }

  return FLASH_PROG_SUCCESS;
} /* flash_prog_erase_some_of_chip */


/*
 *
 * This function will erase every block in the flash
 *
 */
uint32 flash_prog_erase_chip (void)
{
  return flash_prog_erase_some_of_chip (0);
} /* flash_prog_erase_chip */


/*
 *
 * This function finds which block to start a full flash erase with.  It takes
 * into account whether we have a SIM Secure partition and attempts to
 * preserve it if possible
 *
 */

int flash_prog_find_starting_erase_block (flash_partable_t ram_parti_ptr)
{
  int parti_page;
  flash_partable_t nand_parti_ptr;
  int starting_block;
  int result = FLASH_DEVICE_DONE;

  /* Read in the newest partition tbl from the flash.  We will use it
   * to make all our determinations against the RAM copy that has been
   * sent down to us.
   */
  parti_page = (newest_mibib_block *
    mibib.flash_info.partition.pages_per_block) +
    MIBIB_PAGE_PARTITION_TABLE;

  result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
    parti_page, read_type, page_buffer);

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(1,("Read page error on previously good partition tbl\n"));
    SET_ERR (ERR_READ_FAIL);
    return -1;
  }

  nand_parti_ptr = (flash_partable_t)page_buffer;

  /* First test for the most obvious case, that the MIBIB partition has
   * changed definition.  In this case, return 0 to erase the entire chip.
   * Things to test are that the name does not match, or the placement
   * or length has changed.
   */
  if (strncmp(nand_parti_ptr->part_entry[0].name,
        ram_parti_ptr->part_entry[0].name, FLASH_PART_NAME_LENGTH) )
  {
    FLASHLOG(1,("MIBIB partition name changed, erase whole flash\n"));
    return 0;
  }

  if (nand_parti_ptr->part_entry[0].offset !=
    ram_parti_ptr->part_entry[0].offset)
  {
    FLASHLOG(1,("MIBIB partition offset changed, erase whole flash\n"));
    return 0;
  }

  if (nand_parti_ptr->part_entry[0].length !=
    ram_parti_ptr->part_entry[0].length)
  {
    FLASHLOG(1,("MIBIB partition length changed, erase whole flash\n"));
    return 0;
  }

  /* Next, check to see if the second partition is the SIM Secure
   * partition and that it has not changed.  The SIM Secure partition
   * MUST be immediately following the MIBIB partition.
   *
   * We check against the RAM version of the partition tbl because when
   * we get here, the OVERRIDE flag is present.  We want to see if the new
   * flash layout will have the SIM Secure partition.  If not, we can erase
   * the entire flash.
   */
  if (strncmp(ram_parti_ptr->part_entry[1].name,
              MIBIB_SIMSECURE_PARTI_NAME, FLASH_PART_NAME_LENGTH))
  {
    FLASHLOG(1,("Second partition not SIM Secure, erase whole flash\n"));
    return 0;
  }

  if (nand_parti_ptr->part_entry[1].offset !=
    ram_parti_ptr->part_entry[1].offset)
  {
    FLASHLOG(1,("SIM Secure partition offset changed, erase whole flash\n"));
    return 0;
  }

  if (nand_parti_ptr->part_entry[1].length !=
    ram_parti_ptr->part_entry[1].length)
  {
    FLASHLOG(1,("SIM Secure partition length changed, erase whole flash\n"));
    return 0;
  }

  /* We finally are sure that we have an unchanged MIBIB and SIM Secure
   * partition, and we can now calculate what block is just beyond the
   * SIM Secure partition and return that value.
   */
  starting_block = ram_parti_ptr->part_entry[1].offset +
    ram_parti_ptr->part_entry[1].length;

  FLASHLOG (3, ("fseb: Starting erase block is 0x%x\n", starting_block));

  return starting_block;
} /* flash_prog_find_starting_erase_block */


/*
 * This function will call the MSM specific security mode function to set
 * some data values used in all subsequent actions
 */
uint32 flash_prog_set_security_mode (void)
{
  uint32 sec_mode = prog_param.reserved;
  uint32 result;

  KICK_WATCHDOG();
  FLASHLOG ( 1, ("\n\n------ Set Security Mode ------\n\n"));

  FLASHLOG ( 3, ("fssm:  Security mode is "));


  switch (sec_mode)
  {
    case FLASH_MI_BOOT_SEC_MODE_NON_TRUSTED:
    {
      FLASHLOG ( 3, ("NON-TRUSTED\n"));
      break;
    }

    case FLASH_MI_BOOT_SEC_MODE_TRUSTED:
    {
      FLASHLOG ( 3, ("TRUSTED\n"));
      break;
    }

    default:
    {
      FLASHLOG ( 3, ("UNKNOWN :: 0x%lx\n", (unsigned long int)sec_mode));
      SET_ERR(ERR_SECURE_BAD_MODE);
      return FLASH_PROG_FAIL;
    }
  }

  /* Cannot change security modes during a single invocation */
  if ((security_mode != SEC_MODE_NOT_RCVD) &&
    (security_mode != sec_mode))
  {
    FLASHLOG ( 1, ("Attempt to change security modes in single invocation\n"));
    SET_ERR(ERR_SECURE_MODE_CHG);
    return FLASH_PROG_FAIL;
  }

  /* No need to do twice if it has already been set */
  if ((security_mode != SEC_MODE_NOT_RCVD) &&
    (security_mode == sec_mode))
  {
    FLASHLOG ( 3, ("fssm:  Not setting security mode, already done\n"));
    return FLASH_PROG_SUCCESS;
  }

  /* If we get here, we need to call the MSM specific security mode function
   * to set the block layout data for us.
   */
  security_mode = sec_mode;

  result = flash_prog_set_block_offsets_in_mibib(security_mode);

  return result;

} /* flash_prog_set_security_mode */


/*
 *
 * This function receives the partition tbl to be used for subsequent
 * programming.  It validates that it is not different from the one stored
 * in the MIBIB.  It will fail if the two are different and the override
 * flag is not given in the command buffer.
 *  The device structure pointer that is used here to begin with, allows
 * the flash drivers to see the entire flash.  So, we temporarily stuff
 * the limits with a default value so that we do not go into the second
 * partition.
 */
uint32 flash_prog_init_parti(uint32 size)
{
  flash_usr_partable_t flash_prog_parti_ptr;
  flash_partable_t sys_parti_ptr;

  KICK_WATCHDOG();

  FLASHLOG ( 1, ("\n\n------ Receive User Partition Table ------\n"));

  /* Must have already received security mode */
  if (security_mode == SEC_MODE_NOT_RCVD)
  {
    FLASHLOG(1, ("Error:  Must send security mode before partition tbl\n"));
    SET_ERR(ERR_SEC_BAD_ORDER);
    return FLASH_PROG_FAIL;
  }

  /* Clear the oldest usr parti buffer */
  DALSYS_memset(oldest_usr_parti_buffer, 0xFF,
    mibib.flash_info.partition.page_size_bytes);

  flash_prog_parti_ptr = (flash_usr_partable_t)prog_param.data;
  /* Temporarily use oldest usr parti buffer to store ram based 
   * sys parti table. */
  sys_parti_ptr = (flash_partable_t)oldest_usr_parti_buffer;

  /* Sanity check that this is a valid user parition table */
  if ((flash_prog_parti_ptr->magic1 != FLASH_USR_PART_MAGIC1) ||
    (flash_prog_parti_ptr->magic2 != FLASH_USR_PART_MAGIC2) ||
    (flash_prog_parti_ptr->version != FLASH_PARTITION_VERSION) ||
    (flash_prog_parti_ptr->numparts > FLASH_NUM_PART_ENTRIES))
  {
    if ((flash_prog_parti_ptr->magic1 == FLASH_PART_MAGIC1) &&
      (flash_prog_parti_ptr->magic2 == FLASH_PART_MAGIC2))
    {
      SET_ERR(ERR_SYS_PARTI_RCVD);
    }
    else
    {
      SET_ERR(ERR_USR_PARTI_NOMATCH);
    }
    return FLASH_PROG_FAIL;
  }

  FLASHLOG(3,("jip: MIBIB partition not yet opened\n"));

  /* Open the device if it has not already been opened */
  /* Probe will fill values in structure if it succeeds.
   * Currently, which_flash parameter is hard coded to 0, but in
   * future this will be dynamic.
   */
  if (FLASH_PROG_SUCCESS !=
    flash_prog_init_for_partition_image(&mibib, MIBIB_DEVICE_ALL_PARTI_NAME))
  {
    FLASHLOG ( 1, ("Error1: flash_prog_init_parti no nand_device found \n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  /* Make sure usr partition table size doesn't exceed the page size */
  if (size > mibib.flash_info.partition.page_size_bytes)
  {
    FLASHLOG(1, ("Error:  Partition table too big - exceeds page size\n"));
    SET_ERR(ERR_PARTI_TOO_BIG);
    return FLASH_PROG_FAIL;
  }
  
  newest_mibib_block = mibib.flash_info.mibib_info.new_mibib_block;
  oldest_mibib_block = mibib.flash_info.mibib_info.old_mibib_block;

  KICK_WATCHDOG();

  FLASHLOG(3,("MIBIB device opened now\n"));

  KICK_WATCHDOG();

  if (FLASH_PROG_SUCCESS !=
    (flash_prog_convert_usr_parti_tbl_to_sys(mibib.flash_handle,
                  flash_prog_parti_ptr, sys_parti_ptr)))
  {
    FLASHLOG(1, ("Error:  Partition table conversion failed\n"));
    SET_ERR(ERR_PARTI_CNV_FAIL);
    return FLASH_PROG_FAIL;
  }

  KICK_WATCHDOG();

  /* Design Note:  There are only two conditions that are possible,
    * either we have both MIBIB blocks, or we have none.  This is
    * because of the fact that when we have none and we write one
    * for the first time, we also write a dummy "older" one.  This
    * greatly reduces the complexity of the task of finding the correct
    * MIBIB.
    *
    * Secure Boot 2.0 has only one MIBIB. There will either be one
    */

    /* Determine if we should accept this new partition tbl.  First find
     * the newest partition tbl and compare it, and if identical, accept
     * it.  If overrride command is given, allow a mismatch, otherwise,
     * indicate failure, and the host side can then send override.
     */
  if ((oldest_mibib_block == -1) && (newest_mibib_block == -1))
  {
    KICK_WATCHDOG();

    FLASHLOG(3,("fip:  no MIBIBs present\n"));

    if (FLASH_PROG_SUCCESS != flash_prog_setup_mibib_not_present ())
    {
      return FLASH_PROG_FAIL;
    }
  }
  else // if mibib was found
  {
    KICK_WATCHDOG();

    FLASHLOG(3,("flash_prog_init_parti:  some MIBIBs present\n"));

    if (FLASH_PROG_SUCCESS !=
      flash_prog_setup_mibib_present (flash_prog_parti_ptr, sys_parti_ptr))
    {
      return FLASH_PROG_FAIL;
    }
  } // if mibib was found

  KICK_WATCHDOG();

  DALSYS_memset(oldest_parti_buffer, 0xFF,
    mibib.flash_info.partition.page_size_bytes);

  FLASHLOG(3,("Copying system partition tbl from RAM to mibib\n"));

  nand_memcpy((void *)oldest_parti_buffer, (void *)sys_parti_ptr,
    sizeof (struct flash_partition_table));

  /* Now write the MIBIB block to flash. */
  FLASHLOG(3,("jip: Writing MIBIB to flash\n"));

  if (FLASH_PROG_SUCCESS != flash_prog_write_mibib())
  {
    FLASHLOG(1, ("jip: Error writing MIBIB to flash\n"));
    return FLASH_PROG_FAIL;
  }

  KICK_WATCHDOG();

  /* Indicate that we have a valid partition tbl. */
  partition_rcvd = TRUE;

  KICK_WATCHDOG();

  FLASHLOG(3,("\n\n"));
  FLASHLOG(3,("===========  Done ======MIBIB_PAGE_MAGIC_VERSION_PAGE=====\n\n"));

  KICK_WATCHDOG();

  /* Close the device */
  flash_device_close(mibib.flash_handle);
  if (DAL_SUCCESS != flash_device_detach(mibib.flash_handle))
  {
     FLASHLOG(3, ("Error Detaching to device driver \n"));
     SET_ERR (ERR_DEV_MISSING);
     return FLASH_PROG_FAIL;
  }

  mibib.flash_handle = NULL;

  if (FLASH_PROG_SUCCESS !=
    flash_prog_init_for_partition_image(&mibib, MIBIB_DEVICE_ALL_PARTI_NAME))
  {
    return FLASH_PROG_FAIL;
  }

  if (FLASH_PROG_SUCCESS != flash_prog_extended_parti())
  {
    SET_ERR (ERR_EXTENDED_PARTI_INVALID);
    return FLASH_PROG_FAIL;
  }

  return FLASH_PROG_SUCCESS;

} /* flash_prog_init_parti */

/*
 * Close mibib partition handle
 */
uint32 flash_prog_deinit_tools(void)
{
  if (mibib.flash_handle)
  {
    flash_device_close(mibib.flash_handle);
    if (DAL_SUCCESS != flash_device_detach(mibib.flash_handle))
    {
       FLASHLOG(3, ("Error Detaching to device driver \n"));
       SET_ERR (ERR_DEV_MISSING);
       return FLASH_PROG_FAIL;
    }

    mibib.flash_handle = NULL;
  }
  return FLASH_PROG_SUCCESS;
}


/*
 * This function will write the flash.  It gets invoked every time the
 * FLASH_PROG ICD has filled a buffer.  Those buffer fills may not have any
 * correspondence to the size of the pages we can write to the flash,
 * so we must be prepared to partially fill a block with data on one pass
 * through and then continue filling the block on the next pass through.
 * This means that state must be maintained in globals after this function
 * has finished with the current invocation.
 */

uint32 flash_prog_program_img(parti_ctl_t pctl)
{
  byte *data;
  dword addr;
  dword  size;
  unsigned int fill_remaining_in_page, num_fill_bytes = 0;

  unsigned int partial_bytes_avail;
  unsigned int new_last_code_addr;
  unsigned char *wbuf;

  extern mode_type current_mode;

  KICK_WATCHDOG();

  FLASHLOG (4, ("\n----- PROGRAM ------\n"));

  pctl->partial_page_buffer = &partial_page_buffer[0];
  data = prog_param.data + prog_param.offset;
  addr = prog_param.addr + prog_param.offset
    - prog_param.flashaddr;
  size = prog_param.size;

  FLASHLOG (4, ("prog:  addr 0x%lx  len  0x%x\n", (unsigned long int)addr,
    size));


  /* Save address of last code byte so we know where the next block of
   * code is in relation to it's predecessor.
   */
  new_last_code_addr = addr + size;

  /* If this is the first block of code, save the start address
   * of the code image..
   */
  if (pctl->start_addr_loaded == FALSE)
  {
    pctl->start_addr_loaded = TRUE;
    pctl->last_code_addr = addr;
  }

  /* If there is a gap between previous last image address and the
   * starting address of the current image data, fill with fill char
   * and update number of fill chars used.  Also, we are forced to
   * use the partial page buffer if we are doing a fill. Account for
   * gaps that stay within the partial page, as well as gaps that go
   * beyond the partial, and possibly gaps that are larger than the
   * the partial page.  However, we no longer have any spans to close out,
   * so we just fill with required number of fill bytes and write out
   * full pages as necessary, even if they are merely full of fill bytes. */
  KICK_WATCHDOG();
  if (pctl->last_code_addr != addr)
  {
    /* check to make sure we have not got a block with an address less
     * than we have had before.
     */
    if (addr < pctl->last_code_addr)
    {
      FLASHLOG (1,("Error: addr to program %ld < previous addr %ld \n",
        (unsigned long int)addr, (unsigned long int)pctl->last_code_addr));
      SET_ERR (ERR_ADDR_REVERSE);
      return FLASH_PROG_FAIL;
    }

    num_fill_bytes = addr - pctl->last_code_addr;

    while (num_fill_bytes > 0)
    {
      partial_bytes_avail = pctl->page_size - pctl->partial_page_index;
      fill_remaining_in_page = MIN (partial_bytes_avail, num_fill_bytes);

      while (fill_remaining_in_page > 0)
      {
        pctl->partial_page_buffer[pctl->partial_page_index++] =
          (unsigned char)NAND_FILL_CHAR;
        num_fill_bytes--;
        fill_remaining_in_page--;
      }

      if (pctl->partial_page_index >
          pctl->page_size)
      {
        SET_ERR (ERR_PPI_TOO_BIG);
        return FLASH_PROG_FAIL;
      }

      if (pctl->partial_page_index == pctl->page_size)
      {
        KICK_WATCHDOG();

        /* Based on the bad block mode, the appropriate write should be used.
         * Bad Block Replace method is used in BML functions
         * and bad block skip is used in the all other write functions
         */
        if (current_bad_block_mode == FLASH_PROG_BAD_BLOCK_REPLACE)
        {
          if (FLASH_PROG_SUCCESS != flash_prog_bml_write_partial_page_data (pctl))
          {
            return FLASH_PROG_FAIL;
          }
        }
        else
        {
          if (FLASH_PROG_SUCCESS != flash_prog_write_partial_page_data (pctl))
          {
            return FLASH_PROG_FAIL;
          }
        }
        pctl->partial_page_index = 0;
      } /* partial page is full */
    } /* while */
  } /* there is a gap in code addresses */

  KICK_WATCHDOG();

  /* Now write the data */
  while (size > 0)
  {
    partial_bytes_avail = pctl->page_size -
      pctl->partial_page_index;

    if (partial_bytes_avail > size)
    {
      /* fill partial page partially and we are done, do not write page,
       * no need to update data ptr as we are out of data */
      nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data, size);
      pctl->partial_page_index += size;
      break;
    }
    else if ((partial_bytes_avail == size) && (pctl->partial_page_index != 0))
    {
      /* fill partial page completely, ready for writing, none left over */
      nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data, size);
      wbuf = pctl->partial_page_buffer;
      data += partial_bytes_avail;
      size -= partial_bytes_avail;
      pctl->partial_page_index = 0;
    }
    else if ((partial_bytes_avail < size) && (pctl->partial_page_index != 0))
    {
      /* fill partial page completely, ready for writing, some left over */
      nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data,
             partial_bytes_avail);
      wbuf = pctl->partial_page_buffer;
      data += partial_bytes_avail;
      size -= partial_bytes_avail;
      pctl->partial_page_index = 0;
    }
    else /* partial_page_index == 0 && size >= page_size */
    {
      /* write a full page from where it is */
//      wbuf = data;
      nand_memcpy(&pctl->partial_page_buffer[0], data, pctl->page_size);
      wbuf = pctl->partial_page_buffer;
      data += pctl->page_size;
      size -= pctl->page_size;
    }

    KICK_WATCHDOG();


    /* Based on the bad block mode, the appropriate write should be used.
     * Bad Block Replace method is used in BML functions
     * and bad block skip is used in the all other write functions
     */
    if (current_bad_block_mode == FLASH_PROG_BAD_BLOCK_REPLACE)
    {
      /* Write a full page.  If we do not break out of the while early,
       * and we reach here, there is a full page to be written. */
      if (FLASH_PROG_SUCCESS != flash_prog_bml_write_current_page (pctl, wbuf, NULL))
      {
        return FLASH_PROG_FAIL;
      }
    }
    else
      {
        /* Write a full page.  If we do not break out of the while early,
         * and we reach here, there is a full page to be written. */
        if (FLASH_PROG_SUCCESS != flash_prog_write_current_page (pctl, wbuf, NULL))
        {
          return FLASH_PROG_FAIL;
        }
      }
    KICK_WATCHDOG();
    /* We only get here when we have a full page to write, so the
     * increment of the address in the code image is always by a page. */
    addr += pctl->page_size;
  }

  /* Save the RAM address of the last byte of the image written, so
   * that when we get called next, we can see if there are any gaps */
  pctl->last_code_addr = new_last_code_addr;

  if (pctl->partial_page_index > pctl->page_size)
  {
    return FLASH_PROG_FAIL;
  }

  return FLASH_PROG_SUCCESS;
} // flash_program



/*
  This function converts a string to a integer. Negative numbers are not
  handled.
*/
int flash_prog_str2int(char *str)
{
  int value = 0;
  while (*str != '\0')
  {
    int digit = 0;

    if ((*str >= '0') && (*str <= '9'))
    {
      /* Get the value of current digit character */
      digit = *str - '0';
    }
    else
    {
      /* Return 0 in case of failure */
      value = 0;

      break;
    }
    /* Point to the next character*/
    str++;

    /* Add the value of the digit to the result */
    value = 10 * value + digit;
  }
  return value;
}


/*

DESCRIPTION
  This function parses the factory image file header
  The file header consists of the  following fields:
   Partition table offset within file   : 4 bytes
   Binary image offset within file      : 4 bytes
   Signature                            : 24 bytes
   Version                              : 4 bytes

*/

static int flash_prog_parse_factory_img_header (parti_ctl_t pctl)
{
  int i;
  byte* data;
  uint32 version;
  char str[FLASH_FACTORY_IMAGE_SIG_LENGTH + 1];

  data = pctl->factory_img.buf;

  /* Store the partition offset */
  pctl->factory_img.parti_tbl_offset = 0;
  pctl->factory_img.parti_tbl_offset =
    (pctl->factory_img.parti_tbl_offset| (data[3] << 24));
  pctl->factory_img.parti_tbl_offset =
    (pctl->factory_img.parti_tbl_offset | (data[2] << 16));
  pctl->factory_img.parti_tbl_offset =
    (pctl->factory_img.parti_tbl_offset | (data[1] << 8));
  pctl->factory_img.parti_tbl_offset =
    (pctl->factory_img.parti_tbl_offset | data[0] );

  FLASHLOG (4, ("\n In Partition Offset 0x%lx\n",
    (unsigned long int)pctl->factory_img.parti_tbl_offset));

  /* Store the binary image offset */
  pctl->factory_img.bin_img_offset = 0;
  pctl->factory_img.bin_img_offset =
    (pctl->factory_img.bin_img_offset | (data[7] << 24));
  pctl->factory_img.bin_img_offset=
    (pctl->factory_img.bin_img_offset| (data[6] << 16));
  pctl->factory_img.bin_img_offset=
    (pctl->factory_img.bin_img_offset| (data[5] << 8));
  pctl->factory_img.bin_img_offset=
    (pctl->factory_img.bin_img_offset| data[4] );

  FLASHLOG (4, ("\n In Binary Image Offset 0x%lx\n",
    (unsigned long int)pctl->factory_img.bin_img_offset));

  data = data + sizeof(pctl->factory_img.parti_tbl_offset) +
    sizeof(pctl->factory_img.bin_img_offset);

  /* Verify signature string and version number */
  for ( i = 0; i < FLASH_FACTORY_IMAGE_SIG_LENGTH; i++)
  {
    str[i] = ((char)(*data));
    data++;
  }
  if (strcmp (str, "Image file with header") != 0)
  {
    SET_ERR (ERR_IMG_FMT_NOT_SUPPORTED);
    return FLASH_PROG_FAIL;
  }


  /* Store the factory programming specification version number. The 4th
   * byte of the version is always 0
   */
  version = 0;
  version |= (data[2] << 16);
  version |= (data[1] << 8);
  version |= data[0];

  /* Compare if this version is supported by the tool */
  if (version != FLASH_FACTORY_IMAGE_VERSION)
  {
    FLASHLOG ( 1,("\n Factory Image Version = 0x%lx\n",
      (unsigned long int)version));
    FLASHLOG ( 1,("\n Factory Image Version = %d%d.%d\n",
      data[2], data[1], data[0]));

    SET_ERR (ERR_IMG_VER_FAIL);
    return FLASH_PROG_FAIL;
  }

  return FLASH_PROG_SUCCESS;
}


/*
 * This function parses the factory image string table. The string table is a
 * variable length table with format "string = value". It also parses the
 * bad block byte address before the string table. This is done here as its
 * in the same format as the string table. The flash configurations is compared
 * which includes page size, device blocks, device width and pages in block.
 */

static int flash_prog_parse_factory_img_string_table(parti_ctl_t pctl)
{
  byte* data;
  uint32 size , i;
  uint32 hdr_parsed_total = FALSE;
  /* Buffers for Factory Image header information parsing */
  static char factory_img_str[FLASH_FACTORY_IMAGE_PARAM_LEN];
  static char factory_img_value[FLASH_FACTORY_IMAGE_PARAM_LEN];


  data = pctl->factory_img.buf;

  size = pctl->factory_img.buf_bytes;

  /* Print the variable data string */
  while ((size > 0) && (hdr_parsed_total == FALSE))
  {
    /* Name of the parameter */
    i=0;
    while ((((char)*data) != '=') && (size > 0))
    {
      if ((((char)*data) != '\0')  && ((((char)*data) != ' ') ))
      {
        factory_img_str[i++] = ((char)(*data));
      }
      data++;
      size--;
      factory_img_str[i] = '\0';
      if (strcmp (factory_img_str, "end_of_header") == 0)
      {
        /* Reached end of header data */
        hdr_parsed_total = TRUE;
        break;
      }
    }
    data++;
    size--;
    i=0;

    /* If end of header is encountered then exit */
    if (hdr_parsed_total == TRUE)
    {
      break;
    }

    while ((((char)*data) != '\0') && (size > 0) )
    {
      if ((((char)*data) != ' '))
      {
        factory_img_value[i++] = ((char)(*data));
      }
      data++;
      size--;
    }
    factory_img_value[i] = '\0';
    data++;
    size--;

    /* Check for the page size in image and device */
    if (strcmp(factory_img_str, "page_bytes_user") == 0)
    {
      int page_size_from_device = flash_prog_str2int(factory_img_value);
      if (pctl->flash_info.partition.page_size_bytes != page_size_from_device)
      {
        FLASHLOG ( 1,("\n Page size MISMATCH"));
        FLASHLOG ( 1,("\n Page size from Image %d\n", page_size_from_device));
        FLASHLOG ( 1,("\n Page size from target %ld\n",
          (unsigned long int)pctl->flash_info.partition.page_size_bytes));
        SET_ERR (ERR_IMG_CNF_NOMATCH);
        return FLASH_PROG_FAIL;

      }
    }

    /* Check for the pages in block in image and device */
    if (strcmp(factory_img_str, "block_pages") == 0)
    {
      uint32 pages_in_block = flash_prog_str2int(factory_img_value);

      if (pctl->flash_info.partition.pages_per_block != pages_in_block)
      {
        FLASHLOG ( 1,("\n Pages in block MISMATCH"));
        FLASHLOG ( 1,("\n Pages in block from Image %ld\n",
          (unsigned long int)pages_in_block));
        FLASHLOG ( 1,("\n Pages in block from target %ld\n",
          (unsigned long int)pctl->flash_info.partition.pages_per_block));
        SET_ERR (ERR_IMG_CNF_NOMATCH);
        return FLASH_PROG_FAIL;

      }
    }

    /* Check for the total blocks in image and device */
    if (strcmp(factory_img_str, "device_blocks") == 0)
    {
      uint32 device_blocks = flash_prog_str2int(factory_img_value);

      if (pctl->flash_info.partition.block_count < device_blocks)
      {
        FLASHLOG ( 1,("\n Total Blocks MISMATCH"));
        FLASHLOG ( 1,("\n Total Blocks from Image %ld\n",
          (unsigned long int)device_blocks));
        FLASHLOG ( 1,("\n Total Blocks from target %ld\n",
          (unsigned long int)pctl->flash_info.partition.block_count));

        SET_ERR (ERR_IMG_CNF_NOMATCH);
        return FLASH_PROG_FAIL;
      }
    }

    /* Check for the data width in image and device */
    if (strcmp(factory_img_str, "data_width") == 0)
    {
      int device_width = flash_prog_str2int(factory_img_value);

      if (((pctl->flash_info.width == FLASH_x8) &&
        (device_width != FLASH_FACTORY_IMAGE_8BIT_WIDTH)) ||
        ((pctl->flash_info.width == FLASH_x16) &&
        (device_width != FLASH_FACTORY_IMAGE_16BIT_WIDTH)))
      {
        FLASHLOG ( 1,("\n Device Width MISMATCH"));
        FLASHLOG ( 1,("\n Device width from Image %d\n", device_width));
        FLASHLOG ( 1,("\n Device width from target %d\n",
          pctl->flash_info.width));
        SET_ERR (ERR_IMG_CNF_NOMATCH);
        return FLASH_PROG_FAIL;
      }
    }
  }
  return FLASH_PROG_SUCCESS;
}


/*
 * This function parses the factory partition tbl.
 * Each Partition table record has the following fields:
 *  Partition Start Address  : 4 bytes
 *  Partition end Address    : 4 bytes
 *  Partition Actual blocks  : 4 bytes
 *  Partition Name           : 4 bytes
 */
static int flash_prog_parse_factory_img_partition_table (parti_ctl_t pctl)
{
  byte* data;

  uint32 size;
  uint32 i = 0, j = 0;
  uint32 partition_parsed = 0;

  uint32 total_image_partitions = 0;

  data = pctl->factory_img.buf;

  size = pctl->factory_img.buf_bytes;

  /* FACTORY Partition table
   * Start Address 4 bytes
   * End Address : 4 bytes
   * Actual data blocks : 4 bytes
   * Reserved : 4 bytes
   * End is marked with 0xFFFFFFFF
   */

  FLASHLOG (4, ("prog:  Parsing partition tbl "));

  /* Parse the partition tbl */
  while (partition_parsed == 0)
  {
    uint32 start_addr = 0;

    if ( size > FLASH_FACTORY_PARTITION_RECORD_SIZE )
    {
      /* Parse and check if the last entry is received. */
      start_addr = 0;
      start_addr = (start_addr | (data[3] << 24));
      start_addr = (start_addr| (data[2] << 16));
      start_addr = (start_addr| (data[1] << 8));
      start_addr = (start_addr| data[0] );

      data += sizeof(start_addr);
      size += sizeof(start_addr);

      if (start_addr != 0xFFFFFFFF)
      {
        /* Make sure total partitions not exceeding FLASH_TOTAL_NUM_PART_ENTRIES */
        if (total_image_partitions >= FLASH_TOTAL_NUM_PART_ENTRIES)
        {
          FLASHLOG (1,("\nERROR: Partition table entries exceeded %d supported!!!\n\n",
            FLASH_TOTAL_NUM_PART_ENTRIES));
          SET_ERR (ERR_IMG_PARTI_TOO_BIG);
          return FLASH_PROG_FAIL;
        }
        
        /* Set the partition index */
        i = total_image_partitions;

        total_image_partitions++;

        /* Store the partition start address */
        pctl->factory_img.parti_tbl[i].start_addr = start_addr;


        /* Store the partition end address */
        pctl->factory_img.parti_tbl[i].end_addr = 0;
        pctl->factory_img.parti_tbl[i].end_addr =
          (pctl->factory_img.parti_tbl[i].end_addr | (data[3] << 24));
        pctl->factory_img.parti_tbl[i].end_addr =
          (pctl->factory_img.parti_tbl[i].end_addr | (data[2] << 16));
        pctl->factory_img.parti_tbl[i].end_addr =
          (pctl->factory_img.parti_tbl[i].end_addr | (data[1] << 8));
        pctl->factory_img.parti_tbl[i].end_addr =
          (pctl->factory_img.parti_tbl[i].end_addr | data[0] );

        /* Increment the size of the end address  */
        data += sizeof(pctl->factory_img.parti_tbl[i].end_addr);
        size -=  sizeof(pctl->factory_img.parti_tbl[i].end_addr);

        /* Store the actual blocks of data for this partition */
        pctl->factory_img.parti_tbl[i].actual_blks = 0;
        pctl->factory_img.parti_tbl[i].actual_blks =
          (pctl->factory_img.parti_tbl[i].actual_blks | (data[ 3 ] << 24));
        pctl->factory_img.parti_tbl[i].actual_blks =
          (pctl->factory_img.parti_tbl[i].actual_blks | (data[2] << 16));
        pctl->factory_img.parti_tbl[i].actual_blks =
          (pctl->factory_img.parti_tbl[i].actual_blks | (data[1] << 8));
        pctl->factory_img.parti_tbl[i].actual_blks =
          (pctl->factory_img.parti_tbl[i].actual_blks | data[0] );

        /* Increment the size of the actual blocks  */
        data += sizeof(pctl->factory_img.parti_tbl[i].actual_blks);
        size -=  sizeof(pctl->factory_img.parti_tbl[i].actual_blks);

        /* Store the beginning 4 letters of the partition tbl stored in the
         * reserved field
         */
        for ( j=0; j < FLASH_FACTORY_IMAGE_PARTI_NAME; j++)
        {
          pctl->factory_img.parti_tbl[i].name[j] = *data;
        }

        /* Increment the size of the reserved field  */
        data += FLASH_FACTORY_IMAGE_PARTI_NAME;
        size -=  FLASH_FACTORY_IMAGE_PARTI_NAME;

      }
      else
      {
        /* Set the partition index */
        i = total_image_partitions;

        /* Set the last partition address */
	if (i < FLASH_TOTAL_NUM_PART_ENTRIES)
	{
          pctl->factory_img.parti_tbl[i].start_addr = start_addr;
	}  

        /* Partition table parsed */
        partition_parsed = 1;

        break;

      }
    }
    else
    {
      FLASHLOG ( 1,("\n\nPartition table not complete \n\n"));
      SET_ERR (ERR_IMG_PARTI_TOO_BIG);
      return FLASH_PROG_FAIL;
    }
  }
  return FLASH_PROG_SUCCESS;
}


/*
 * This function programs the factory image
 */
uint32 flash_prog_program_factory_img(parti_ctl_t pctl)
{
  byte *data;
  dword addr;
  word  size;
  int result = FLASH_PROG_SUCCESS;
  unsigned int partial_bytes_avail;
  unsigned int new_last_code_addr;
  //unsigned char *wbuf = pctl->partial_page_buffer;
  unsigned char *wbuf;
  uint32 blocks_programmed = 0, current_parti = 0;
  uint32 next_parti_start_addr = 0;
  uint32 new_partition_found = FALSE;

  KICK_WATCHDOG();

  FLASHLOG (4, ("\n----- PROGRAM  FactoryImage.mbn------\n"));

  pctl->partial_page_buffer = &partial_page_buffer[0];
  wbuf = pctl->partial_page_buffer;

  pctl->factory_img.buf = &factory_img_buffer[0];
  
  data = prog_param.data + prog_param.offset;
  addr = prog_param.addr + prog_param.offset
    - prog_param.flashaddr;
  size = prog_param.size;

  FLASHLOG (4, ("prog:  addr 0x%lx  len  0x%x\n", (unsigned long int)addr,
    size));

  KICK_WATCHDOG();

  /* Save address of last code byte so we know where the next block of
   * code is in relation to it's predecessor.
   */
  new_last_code_addr = addr + size;

  /* If this is the first block of code, save the start address
   * of the code image..
   */
  if (pctl->start_addr_loaded == FALSE)
  {
    pctl->start_addr_loaded = TRUE;
    pctl->last_code_addr = addr;
  }

  /* Now parse the factory image data */
  while (size > 0)
  {
    if (pctl->factory_img.parsing_state != FLASH_FACTORY_PARSER_STATE_DONE)
    {
      uint32 buffer_bytes = 0;
      uint32 bytes_needed = 0;
      switch (pctl->factory_img.parsing_state)
      {
        /* If image file header is not yet received */
        case FLASH_FACTORY_PARSER_STATE_HDR_PARTIAL:
          {
            bytes_needed = FLASH_FACTORY_FILE_HDR_SIZE;
            break;
          }

        /* Retrieve the bad block marker. Parse the variable string table after
         * the mandatory header
         */
        case FLASH_FACTORY_PARSER_STATE_HDR_COMPLETE:
          {
          /* Determine the size of the buffer to store the partition tbl data.
           * We store minimum of the actual partition tbl data and
           * the TOTAL_PAGE_SIZE
           */
            if ((pctl->factory_img.parti_tbl_offset -
              FLASH_FACTORY_FILE_HDR_SIZE) > TOTAL_PAGE_SIZE)
            {
            /* The factory header will not exceed TOTAL_PAGE_SIZE  */
              bytes_needed = TOTAL_PAGE_SIZE;
            }
            else
            {
            /* The total header data is less then the TOTAL_PAGE_SIZE. So buffer
             * we can buffer all data includind pad bytes
             */
              bytes_needed = pctl->factory_img.parti_tbl_offset -
                FLASH_FACTORY_FILE_HDR_SIZE;
            }
            break;
          }
        /* Parsing the partition tbl */
        case FLASH_FACTORY_PARSER_STATE_PARTITION:
          {
          /* Determine the size of the buffer to store the partition tbl data.
           * We store minimum of the actual partition tbl data and
           * the TOTAL_PAGE_SIZE
           */
            if ((pctl->factory_img.bin_img_offset -
              pctl->factory_img.parti_tbl_offset) > TOTAL_PAGE_SIZE)
            {
            /* Assumption: The partition tbl entries will not exceed the
             * TOTAL_PAGE_SIZE
             */
              bytes_needed = TOTAL_PAGE_SIZE;
            }
            else
            {
            /* The total partition data is less then the TOTAL_PAGE_SIZE. So
             * buffer we can buffer all data includind pad bytes
             */
              bytes_needed = pctl->factory_img.bin_img_offset -
                pctl->factory_img.parti_tbl_offset;
            }

          /* At this point the Factory Image  header is parsed and the
           * partition tbl offset is not reached. We can still receive some
           * padded data of the partition tbl. We skip this data till we
           * get the data from the partition tbl offset
           */
            if (( addr < pctl->factory_img.parti_tbl_offset))
            {
              if ( (pctl->factory_img.parti_tbl_offset - addr) > size)
              {
               /* All pad bytes in this chunk of data */
                size = 0;
              }
              else
              {
               /* This chunk of data consists of pad bytes and partition data */
                size -= pctl->factory_img.parti_tbl_offset - addr;
                data += (pctl->factory_img.parti_tbl_offset - addr);
                addr += (pctl->factory_img.parti_tbl_offset - addr);
              }
            }
            break;
          }
        default:
          break;
      }

      if ((bytes_needed - pctl->factory_img.buf_bytes) > size)
      {
        buffer_bytes = size;
      }
      else
      {
        buffer_bytes = bytes_needed - pctl->factory_img.buf_bytes;
      }
      nand_memcpy(&pctl->factory_img.buf[pctl->factory_img.buf_bytes],
        data, buffer_bytes);

      /* Increment the size of the fixed part of the file header */
      pctl->factory_img.buf_bytes += buffer_bytes;

      data += buffer_bytes;
      size -= buffer_bytes;
      addr += buffer_bytes;

      /* If we have buffered all the bytes we need for parsing */
      if ( pctl->factory_img.buf_bytes == bytes_needed )
      {
        switch ( pctl->factory_img.parsing_state )
        {
          case FLASH_FACTORY_PARSER_STATE_PARTITION:
          {
            /* Parse the partition tbl  */
            result = flash_prog_parse_factory_img_partition_table(pctl);
            /* Change state */
            pctl->factory_img.parsing_state =
              FLASH_FACTORY_PARSER_STATE_DONE;
            break;
          }

          case FLASH_FACTORY_PARSER_STATE_HDR_COMPLETE:
          {
            /* Parse the string table  */
            result = flash_prog_parse_factory_img_string_table(pctl);
            /* Change state */
            pctl->factory_img.parsing_state =
              FLASH_FACTORY_PARSER_STATE_PARTITION;
            break;
          }

          case FLASH_FACTORY_PARSER_STATE_HDR_PARTIAL:
          {
            /* Parse the fixed header */
            result = flash_prog_parse_factory_img_header(pctl);

            /* Change state */
            pctl->factory_img.parsing_state =
              FLASH_FACTORY_PARSER_STATE_HDR_COMPLETE;
            break;
          }

          default:
          {
            break;
          }
        }

        /* If the parsing function failed, then error */
        if (result != FLASH_PROG_SUCCESS)
        {
          return result;
        }

        /* Reset the partial file header bytes */
        pctl->factory_img.buf_bytes = 0;
        if (size == 0)
        {
          break;
        }
        else
        {
          if ( pctl->factory_img.parsing_state !=
            FLASH_FACTORY_PARSER_STATE_DONE)
          {
            /* Continue parsing */
            continue;
          }
        }
      }
    }

    /* At this point the Factory Image  header and the Partition Table are
     * parsed. We can still receive some padded data of the partition tbl.
     * We skip this data till we get the data from the binary image offset
     */
    if (( addr < pctl->factory_img.bin_img_offset))
    {
      if ( (pctl->factory_img.bin_img_offset - addr) > size)
      {
        /* All pad bytes in this chunk of data */
        size = 0;
        break;
      }
      else
      {
        /* This chunk of data consists of pad bytes and image data */
        size -= pctl->factory_img.bin_img_offset - addr;
        data += (pctl->factory_img.bin_img_offset - addr);
        addr += (pctl->factory_img.bin_img_offset - addr);

        /* Encountered first partition data */
        new_partition_found = TRUE;

        /* Set the partition to be programmed */
        pctl->factory_img.curr_parti = current_parti;

        /* Set the current block to the start address of this partition */
        pctl->current_block =
          pctl->factory_img.parti_tbl[pctl->factory_img.curr_parti].start_addr;

      }
    }

    current_parti = pctl->factory_img.curr_parti;

    /* Calculate the blocks of the partition that have been completely
     *  programmed
     */
    blocks_programmed = pctl->current_block -
      pctl->factory_img.parti_tbl[current_parti].start_addr -
      pctl->factory_img.bb_count[current_parti] + 1;


    /* If there are data bytes available and the actual blocks of the current
     * partition are programmed or the actual blocks of the current partition
     * equal to zero and the current data received is not equal to the next
     * partition then skip the data
     */
    if (((blocks_programmed >=
      pctl->factory_img.parti_tbl[current_parti].actual_blks) &&
      (pctl->avail_pages_in_block == 0)))
    {
      while (new_partition_found == FALSE)
      {
        if (pctl->factory_img.parti_tbl[current_parti+ 1].start_addr ==
          0xFFFFFFFF)
        {
          pctl->last_code_addr = new_last_code_addr;
          return FLASH_PROG_SUCCESS;
        }

        next_parti_start_addr =
          pctl->factory_img.parti_tbl[current_parti+ 1].start_addr *
          pctl->page_size * pctl->flash_info.partition.pages_per_block +
          pctl->factory_img.bin_img_offset;

        if ((next_parti_start_addr - addr) > size)
        {
          size = 0;

          /* Complete data is partition spare block data */
          break;
        }
        else
        {
          data += (next_parti_start_addr - addr);
          size -= (next_parti_start_addr - addr);
          addr += (next_parti_start_addr - addr);
        }
        if (addr == next_parti_start_addr)
        {
          /* Update the local variables */
          current_parti++;
          blocks_programmed = 0;

          /* Set the partition to be programmed */
          pctl->factory_img.curr_parti = current_parti;

          /* Set the current block to the start address of this partition */
          pctl->current_block =
            pctl->factory_img.parti_tbl[current_parti].start_addr;

          if (pctl->factory_img.parti_tbl[current_parti].actual_blks  != 0)
          {
            new_partition_found = TRUE;
          }

          if (pctl->factory_img.parti_tbl[current_parti].start_addr ==
            0xFFFFFFFF)
          {
            pctl->last_code_addr = new_last_code_addr;
            return FLASH_PROG_SUCCESS;
          }
        }
      }
    }

    if (new_partition_found == TRUE)
    {
      /* prepare the block for the new partition */
      pctl->partition_initial_blk_prepared = TRUE;
      if (flash_prog_prepare_this_block (pctl) == FLASH_PROG_FAIL)
      {
        return FLASH_PROG_FAIL;
      }

      new_partition_found = FALSE;
    }

    /* All data is skipped */
    if (size <= 0)
    {
      size = 0;
      break;
    }

    FLASHLOG (4, ("prog: partition 0x%lx block 0x%lx programmed\n",
      (unsigned long int)pctl->factory_img.curr_parti,
      (unsigned long int)pctl->current_block));

    partial_bytes_avail = pctl->page_size - pctl->partial_page_index;

    if (partial_bytes_avail > size)
    {
      /* fill partial page partially and we are done, do not write page,
       * no need to update data ptr as we are out of data */
      nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data, size);
      pctl->partial_page_index += size;
      break;
    }
    else if ((partial_bytes_avail == size) && (pctl->partial_page_index != 0))
    {
      /* fill partial page completely, ready for writing, none left over */
      nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data, size);
      wbuf = pctl->partial_page_buffer;
      data += partial_bytes_avail;
      size -= partial_bytes_avail;
      pctl->partial_page_index = 0;
      addr += partial_bytes_avail;
    }
    else if ((partial_bytes_avail < size) && (pctl->partial_page_index != 0))
    {
      /* fill partial page completely, ready for writing, some left over */
      nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data,
        partial_bytes_avail);
      wbuf = pctl->partial_page_buffer;
      data += partial_bytes_avail;
      size -= partial_bytes_avail;
      pctl->partial_page_index = 0;
      addr += partial_bytes_avail;
    }
    else
    { /* partial_page_index == 0 && size >= page_size */
      /* write a full page from where it is */
      wbuf = data;
      data += pctl->page_size;
      size -= pctl->page_size;
      addr += pctl->page_size;
    }

    KICK_WATCHDOG();

    FLASHLOG (4, ("prog:  page size4 0x%lx  len  0x%x\n",
      (unsigned long int)pctl->page_size, size));


    /* Write a full page.  If we do not break out of the while early,
     * and we reach here, there is a full page to be written. */
    if(pctl->flash_info.type == FLASH_ONENAND)
    {
      /* Split the full page into main and spare buffers */
      pctl->spare_bytes_len = pctl->flash_info.partition.total_page_size_bytes -
      pctl->flash_info.partition.page_size_bytes;
      if (FLASH_PROG_SUCCESS != flash_prog_write_current_page (pctl, wbuf,
        &wbuf[pctl->flash_info.partition.page_size_bytes]))
      {
        return FLASH_PROG_FAIL;
      }
    }
    else
    {
      if (FLASH_PROG_SUCCESS != flash_prog_write_current_page (pctl, wbuf,
        NULL))
      {
        return FLASH_PROG_FAIL;
      }
    }
    KICK_WATCHDOG();
  }

  FLASHLOG (4, ("prog:  page size5 0x%lx  len  0x%x\n",
    (unsigned long int)pctl->page_size, size));
  /* Save the RAM address of the last byte of the image written, so
   * that when we get called next, we can see if there are any gaps */
  pctl->last_code_addr = new_last_code_addr;

  if (pctl->partial_page_index > pctl->page_size)
  {
    return FLASH_PROG_FAIL;
  }

  return FLASH_PROG_SUCCESS;
} // flash_prog_program_factory_img


/*===========================================================================

FUNCTION flash_prog_program_flash_bin

DESCRIPTION
  This function will write the flash.bin image to the flash.
  It gets invoked every time the
  JTAG ICD has filled a buffer.  Those buffer fills may not have any
  correspondence to the size of the pages we can write to the flash,
  so we must be prepared to partially fill a block with data on one pass
  through and then continue filling the block on the next pass through.
  This means that state must be maintained in globals after this function
  has finished with the current invocation.
  In addition to the above this function also needs to parse the image,
  extract the image header , extract the record header for each of the blocks
  and enusure that any jumps in the record/block address are correctly
  reflected in the flash.

  The format of FLASH.bin is was follows
  IMAGE HEADER:
   Sync Record: 7 bytes (includes signature info to determine if WM6 or WM7 image)
   Address    : 4 bytes
   Length     : 4 bytes
  Each record/Block has the following header

  RECORD(BLOCK) HEADER:
   Address    : 4 bytes
   Length     : 4 bytes

  FOR WM6 images the record header has additional 4 bytes following the above
   Checksum   : 4 bytes

  The block headers are not written to the flash.
  The is only one Image header in flash.bin but, but multiple record/headers

  LOGIC for Bad block management.
  For WM6 image, if a jump is detected in the record address, it indicates end of
  a partition. For WM7  image a jump does not indicate end of partition but merley
  empty blocks.  In the case of WM6, check if the jump is less than the number of
  Bad blocks allowed in the partition. If lesser return error.
  Compensate for the bad block in the partition before determining how many
  earsed blocks to write
  Check for bad blocks in the padding blocks

DEPENDENCIES
  Assumes prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return FLASH_PROG_SUCCESS else FLASH_PROG_FAIL

SIDE EFFECTS

===========================================================================*/

uint32 flash_prog_program_flash_bin(parti_ctl_t pctl)
{
  byte *data;
  dword addr;
  word  size;
  int i;
  unsigned int partial_bytes_avail, partial_spare_bytes_avail;
  unsigned int new_last_code_addr;
//  unsigned char *wbuf = pctl->partial_page_buffer;
  unsigned char *wbuf;
  enum flash_block_state blk_state;

  KICK_WATCHDOG();

  FLASHLOG (4, ("\n----- PROGRAM  Flash.bin------\n"));

  pctl->partial_page_buffer = &partial_page_buffer[0];
  wbuf = pctl->partial_page_buffer;

  data = prog_param.data + prog_param.offset;
  addr = prog_param.addr + prog_param.offset
         - prog_param.flashaddr;
  size = prog_param.size;

  FLASHLOG (4, ("prog:  addr 0x%lx  len  0x%x\n", (unsigned long int)addr,
    size));

  /* Save address of last code byte so we know where the next block of
   * code is in relation to it's predecessor.
   */
  new_last_code_addr = addr + size;

 /* If this is the first block of code, save the start address
   * of the code image..
   */
  /* Also advance the image header for flash.bin*/
  if (pctl->start_addr_loaded == FALSE)
  {
    pctl->start_addr_loaded = TRUE;
    pctl->last_code_addr = addr;

    if (data[0] == FLASH_WM_VERSION_WM7_SIGNATURE)
    {
      pctl->wm_version = FLASH_WM_VERSION_WM7;
      pctl->flash_bin_record_header_size = 8;
      pctl->flash_bin_page_spare_size = 6;
    }
    else
    {
      pctl->wm_version = FLASH_WM_VERSION_WM6;
      pctl->flash_bin_record_header_size = 12;
      pctl->flash_bin_page_spare_size = 8;
    }

    /* Advance the image header for flash.bin*/
    data += FLASH_WM_FLASH_BIN_IMAGE_HDR_SIZE;
    size -= FLASH_WM_FLASH_BIN_IMAGE_HDR_SIZE;
    addr += FLASH_WM_FLASH_BIN_IMAGE_HDR_SIZE;
  }


  KICK_WATCHDOG();

  /* Now parse the record data for flash.bin */
  while (size > 0)
  {
    /* If it is a new block we need to parse the header*/
    if (pctl->record_header_parsed == 0)
    {
      /* While parsing the header the size should be atleast 12
       * Otherwise we need to store the record header till we recevied the
       * entire header */
      if (size < pctl->flash_bin_record_header_size)
      {
        pctl->record_header_parsed_partially = 1;
        pctl->len_record_header_parsed = size;

        /* Record address is complete so extract that*/
        if (size >= FLASH_WM_FLASH_BIN_RECORD_ADDR_SIZE)
        {
          /* Store previous address*/
          pctl->prev_record_addr  = pctl->curr_record_addr;

          /* Store the current address */
          pctl->curr_record_addr = 0;
          pctl->curr_record_addr = (pctl->curr_record_addr | (data[3] << 24));
          pctl->curr_record_addr = (pctl->curr_record_addr | (data[2] << 16));
          pctl->curr_record_addr = (pctl->curr_record_addr | (data[1] << 8));
          pctl->curr_record_addr = (pctl->curr_record_addr | data[0] );

          FLASHLOG (3, ("\n In Partial Curr Record addr 0x%lx\n",
            (unsigned long int)pctl->curr_record_addr ));


          /* Increment the size of the record header */
          data += size;
        }
        else /* Size < record address length */
        {
          /* Store the record header till we get complete information */
          for (i=0; i<size; i++)
          {
            pctl->temp_record_addr[i] = *data++;
          }

        }

        size -= size;
        addr += size;
        break;  /* break out of while loop */
      }/* if (size < FLASH_WM_FLASH_BIN_RECORD_HDR_SIZE) */

      /* If header partially parsed*/
      if ((pctl->record_header_parsed_partially == 1))
      {
        int rem_header_len =  pctl->flash_bin_record_header_size -
                             pctl->len_record_header_parsed;

        if (pctl->len_record_header_parsed <
            FLASH_WM_FLASH_BIN_RECORD_ADDR_SIZE)
        {
          pctl->prev_record_addr  = pctl->curr_record_addr;
          pctl->curr_record_addr = 0;

          /* Depending on the previous parse info */
          /* Calculate current address */
          switch (pctl->len_record_header_parsed)
          {
          case 3:
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (pctl->temp_record_addr[2] << 16));
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (pctl->temp_record_addr[1] << 8));
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      pctl->temp_record_addr[0] );
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (data[0] << 24));
            break;
          case 2:
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (pctl->temp_record_addr[1] << 8));
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      pctl->temp_record_addr[0] );
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (data[1] << 24));
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (data[0] << 16));
            break;
          case 1:
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      pctl->temp_record_addr[0] );
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (data[2] << 24) );
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (data[1] << 16));
            pctl->curr_record_addr = (pctl->curr_record_addr |
                                      (data[0] << 8));
            break;
          }
        }/* Complete the header parsing */

        /* Increment the size of the record header */
        data += rem_header_len;
        size -= rem_header_len;
        addr += rem_header_len;

        pctl->record_header_parsed_partially = 0;
        pctl->len_record_header_parsed = 0;
      }
      else
      {
        /* Header is completely parsed */

        /* Store the previous address*/
        pctl->prev_record_addr  = pctl->curr_record_addr;

        /* Store the current address*/
        pctl->curr_record_addr = 0;
        pctl->curr_record_addr = (pctl->curr_record_addr | (data[3] << 24));
        pctl->curr_record_addr = (pctl->curr_record_addr | (data[2] << 16));
        pctl->curr_record_addr = (pctl->curr_record_addr | (data[1] << 8));
        pctl->curr_record_addr = (pctl->curr_record_addr | data[0] );

        FLASHLOG (3, ("\n Curr Record addr 0x%lx\n",
          (unsigned long int)pctl->curr_record_addr ));

        /* Done only for the 1st record*/
        if (pctl->record_len == 0)
        {
          /* For the 1st record store the length */
          pctl->record_len = (pctl->record_len | (data[7] << 24));
          pctl->record_len = (pctl->record_len | (data[6] << 16));
          pctl->record_len = (pctl->record_len | (data[5] << 8));
          pctl->record_len = (pctl->record_len | data[4] );

          FLASHLOG (3, ("\n Curr Record len 0x%lx\n",
            (unsigned long int)pctl->record_len ));

          /* Verify whether Record len is as expected */
          if ((pctl->page_size == FLASH_NAND_PAGE_SIZE_2048)||
              (pctl->page_size == FLASH_NAND_PAGE_SIZE_4096))
          {
            pctl->flash_bin_record_data_size = 64 * (pctl->page_size +
              pctl->flash_bin_page_spare_size);
            if (pctl->record_len != pctl->flash_bin_record_data_size)
            {
              FLASHLOG(1, (" ERROR : Mismatch in record Len in flash.bin "));
              SET_ERR (ERR_FLSH_BIN_INV_LEN);
              return FLASH_PROG_FAIL;
            }
            /* Also set the spare byte length */
            pctl->spare_bytes_len = FLASH_WM_SPARE_BYTES_PER_2K_PAGE;
          }
          else
          {
            FLASHLOG(1,("Error: Page size not supported 0x%lx\n",
              (unsigned long int)pctl->page_size));
            SET_ERR (ERR_PG_SZ_NOT_SUPP);
            return FLASH_PROG_FAIL;
          }
        }
        /* Increment the size of the record header */
        data += pctl->flash_bin_record_header_size;
        size -= pctl->flash_bin_record_header_size;
        addr += pctl->flash_bin_record_header_size;
      }

      /* Set the flag that the record header is parsed */
      /* Reset it only after 64 pages */
      pctl->record_header_parsed = 1;

      /* Reset the flag to write the spare */
      pctl->write_spare = 1;

      /* Check if we have jumped blocks indicating end of a partition*/
      if (((pctl->curr_record_addr - pctl->prev_record_addr) !=
             pctl->record_len ) &&
            (pctl->curr_record_addr != 0))
      {
        /* Number of blocks jumped */
        uint32 blocks_jumped =
          ((pctl->curr_record_addr - pctl->prev_record_addr) /
            pctl->record_len) -1 ;
        int bb_count = 0, erase_blk_count = 0;
        int start_spare_block =0;
        int initial_bb_count =
          pctl->bad_block_count[pctl->flash_bin_partitions];

        FLASHLOG (3, ("\nDetected a jump of 0x%lx blocks",
          (unsigned long int)blocks_jumped));
        FLASHLOG (3, ("\nCurrent block: 0x%lx",
          (unsigned long int)pctl->current_block));
        FLASHLOG (3, ("\n Curr BB = 0x%lx ", (unsigned long int)
          pctl->bad_block_count[pctl->flash_bin_partitions]));

        if ((pctl->wm_version == FLASH_WM_VERSION_WM6) &&
            (blocks_jumped < pctl->bad_block_count[pctl->flash_bin_partitions]))
        {
          FLASHLOG(1,(" ERROR : Too many bad blocks in the flash "));
          FLASHLOG(1,(" ERROR : Buffering insufficient in flash.bin "));
          SET_ERR (ERR_FLSH_BIN_NO_BUFF);
          return FLASH_PROG_FAIL;
        }

        erase_blk_count = blocks_jumped;

        /* Start of the spare blocks */
        start_spare_block = pctl->current_block +1;

        for (i=0; i<erase_blk_count;)
        {
          KICK_WATCHDOG();

          if (DAL_SUCCESS != flash_block_get_state(pctl->flash_handle,
            pctl->current_block +1, &blk_state))
          {
            FLASHLOG(1,("ebb: bbt block 0x%lx is marked bad\n",
              (unsigned long int)pctl->current_block +1));
            SET_ERR (ERR_BAD_BLOCK);
            return FLASH_PROG_FAIL;
          }

          if (blk_state == FLASH_BLOCK_BAD)
          {
            FLASHLOG (3, ("\n\nSkip bad Blk at 0x%lx, i = 0x%x\n",
              (unsigned long int)(pctl->current_block + 1), i ));
            /* Increament the number of bad blocks */
            bb_count++;
            (pctl->current_block)++;
            continue;
          }

          KICK_WATCHDOG();
          FLASHLOG (3, ("\nErase buffer block 0x%lx, i = 0x%x",
            (unsigned long int)(pctl->current_block + 1), i ));
            (void )flash_prog_prepare_next_block(pctl);
          i++;

        }

        FLASHLOG (3, ("\n pctl->current_block 0x%lx ",
          (unsigned long int)pctl->current_block ));
        FLASHLOG (3, ("\n bb_count 0x%x", bb_count ));

        pctl->current_page = (start_spare_block + erase_blk_count + bb_count) \
                * pctl->flash_info.partition.pages_per_block;

        /* Correctly update the number of bad blocks in the padding blocks */
        if (bb_count)
        {
          if (pctl->bad_block_count[pctl->flash_bin_partitions] !=
              initial_bb_count + bb_count)
          {
            FLASHLOG (3, ("\n Correcting BB count " ));
            pctl->bad_block_count[pctl->flash_bin_partitions] =
              initial_bb_count + bb_count;
          }
        }

        FLASHLOG(1,(" \n pctl->current_page = 0x%lx  Calculated = 0x%lx ",
          (unsigned long int)(pctl->current_page -
                 (pctl->bad_block_count[pctl->flash_bin_partitions] * \
                  pctl->flash_info.partition.pages_per_block)),
           (unsigned long int)((pctl->curr_record_addr /pctl->record_len) * \
                  pctl->flash_info.partition.pages_per_block)));
        FLASHLOG(1,(" \n BB count = 0x%lx  spare_blk_count = 0x%x ",
          (unsigned long int)pctl->bad_block_count[pctl->flash_bin_partitions],
          erase_blk_count ));
        FLASHLOG(1,(" \n bb_count = 0x%x  ", bb_count ));

        /* Increment the number of partitions in flash.bin */
        pctl->flash_bin_partitions++;

        /* Calculate the current block */
        FLASHLOG(1,(" \n pctl->current_block to prepare = 0x%lx  ",
          (unsigned long int)pctl->current_block));
        (void )flash_prog_prepare_next_block(pctl);
      }

      if (size == 0)
      {
        break;
      }

    } /* if (pctl->record_header_parsed == 0) */

    /* Check if we have partially read the spare area */
    if (pctl->partial_spare_index == 0)
    {
      FLASHLOG (4, ("\n page parsing begun " ));
      partial_bytes_avail = pctl->page_size - pctl->partial_page_index;

      if (partial_bytes_avail > size)
      {
        FLASHLOG (4, ("\n partial_bytes_avail > size" ));
        FLASHLOG (4, ("\n number of pages available 0x%lx",
          (unsigned long int)pctl->avail_pages_in_block ));
        /* fill partial page partially and we are done, do not write page,
         * no need to update data ptr as we are out of data */
        nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index],
               data, size);
        pctl->partial_page_index += size;
        break;
      }
      else if ((partial_bytes_avail == size) && (pctl->partial_page_index != 0))
      {
        FLASHLOG (4,("\n partial_bytes_avail == size) && \
                  (pctl->partial_page_index != 0" ));

        /* fill partial page completely, ready for writing, none left over */
        nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data,
               size);
        wbuf = pctl->partial_page_buffer;
        data += partial_bytes_avail;
        size -= partial_bytes_avail;
        pctl->partial_page_index = 0;
      }
      else if ((partial_bytes_avail < size) && (pctl->partial_page_index != 0))
      {

        FLASHLOG (4, ("\n partial_bytes_avail < size) && \
                     (pctl->partial_page_index != 0" ));
        /* fill partial page completely, ready for writing, some left over */
        nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data,
               partial_bytes_avail);
        wbuf = pctl->partial_page_buffer;
        data += partial_bytes_avail;
        size -= partial_bytes_avail;
        pctl->partial_page_index = 0;
      }
      else /* partial_page_index == 0 && size >= page_size */
      {

        FLASHLOG (4, ("\n partial_page_index == 0 && size >= page_size" ));
        /* write a full page from where it is */
        //wbuf = data;
        nand_memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data,
               pctl->page_size);
        data += pctl->page_size;
        size -= pctl->page_size;
      }

      KICK_WATCHDOG();
      /* Populate the spare buffer */
      if (pctl->write_spare == 1)
      {
        /* Size is greater than the spare length */
        if (size >= pctl->flash_bin_page_spare_size)
        {
          /* fill partial spare page fully and we are ready to write the page*/
          nand_memcpy(&pctl->partial_spare_buffer[pctl->partial_spare_index],
                 data, pctl->flash_bin_page_spare_size);
          data += pctl->flash_bin_page_spare_size;
          size -= pctl->flash_bin_page_spare_size;
          pctl->write_spare = 0;
          pctl->partial_spare_index = 0;
        }
        /* Size is lesses than the spare length */
        else
        {
          /* fill spare buffer partially , cannot write till the spare
           * buffer is full */
          nand_memcpy(&pctl->partial_spare_buffer[pctl->partial_spare_index],
                 data, size);
          pctl->partial_spare_index += size;

          data += size;
          size -= size;
        }
      }
    }
    /* We have partially read the spare buffer */
    else
    {
      FLASHLOG (3, ("\n spare parsing begun " ));

      partial_spare_bytes_avail = pctl->flash_bin_page_spare_size -
                                  pctl->partial_spare_index;

      if (size >= partial_spare_bytes_avail)
      {
        /* fill partial spare page fully and we are ready to write the page*/
        nand_memcpy(&pctl->partial_spare_buffer[pctl->partial_spare_index],
               data, partial_spare_bytes_avail);
        data += partial_spare_bytes_avail;
        size -= partial_spare_bytes_avail;
        pctl->write_spare = 0;
        pctl->partial_spare_index = 0;
      }
      else
      {
        /* fill spare buffer partially , cannot write till the spare
         * buffer is full */
        nand_memcpy(&pctl->partial_spare_buffer[pctl->partial_spare_index],
               data, size);
        pctl->partial_spare_index += size;

        data += size;
        size -= size;
      }
    } /* if (pctl->partial_spare_index == 0) */

    /* Only if spare buffer is full can we write a page */
    if (pctl->write_spare == 0)
    {
      /* Write a full page with spare. If we do not break out of the while
       * early, and we reach here, there is a full page to be written.
       */

       /* For WM7 image, inject the Bad block byte at index 4 and
        * reserved1 bytes at index 5, and move reserved2 word to index 6
        */

      if (pctl->wm_version == FLASH_WM_VERSION_WM7)
      {
         pctl->partial_spare_buffer[6] =  pctl->partial_spare_buffer[4];
         pctl->partial_spare_buffer[7] =  pctl->partial_spare_buffer[5];
         pctl->partial_spare_buffer[4] = 0xFF;
         pctl->partial_spare_buffer[5] = 0xFF;
      }


      if (flash_prog_write_current_page_flash_bin (pctl,
        wbuf, pctl->partial_spare_buffer) == FLASH_PROG_FAIL)
      {
        return FLASH_PROG_FAIL;
      }


      /* We only get here when we have a full page to write, so the
       * increment of the address in the code image is always by a page
       * and spare length
       */
      addr += pctl->page_size + pctl->flash_bin_page_spare_size;

    }
    KICK_WATCHDOG();
  }

  /* Save the RAM address of the last byte of the image written, so
   * that when we get called next, we can see if there are any gaps */
  pctl->last_code_addr = new_last_code_addr;

  if (pctl->partial_page_index > pctl->page_size)
  {
    return FLASH_PROG_FAIL;
  }

  return FLASH_PROG_SUCCESS;

} /* flash_prog_program_flash_bin */

int flash_prog_get_flash_info(void)
{

  KICK_WATCHDOG();

  /* Open the device if it has not already been opened */
  /* Probe will fill values in structure if it succeeds.
   * Currently, which_flash parameter is hard coded to 0, but in
   * future this will be dynamic.
   */

  if (FLASH_PROG_SUCCESS !=
    flash_prog_init_for_partition_image(&mibib, MIBIB_DEVICE_ALL_PARTI_NAME))
  {
    FLASHLOG ( 1, ("Error1: flash_prog_get_flash_info: no nand_device found \n"));
    SET_ERR (ERR_DEV_MISSING);
    return FLASH_PROG_FAIL;
  }

  KICK_WATCHDOG();

  nand_core_strcpy((uint8 *)mibib.dev_params.flash_name,
    mibib.flash_info.device_name, FLASH_PROG_NAME_SIZE);

  mibib.dev_params.num_blocks = mibib.flash_info.partition.block_count;
  mibib.dev_params.page_size = mibib.flash_info.partition.page_size_bytes;
  mibib.dev_params.pages_in_block = mibib.flash_info.partition.pages_per_block;
  prog_param.reserved = (uint32)&mibib.dev_params;

 return FLASH_PROG_SUCCESS;
}/* flash_prog_get_flash_info */

/* Bad block manager */
static int flash_prog_bad_block_manager (parti_ctl_t pctl,unsigned char *src_main,
    unsigned char *src_spare)
{
  int result = FLASH_DEVICE_DONE;
  uint32 page_idx, failed_block, failed_page;
  enum flash_block_state bad_block = FLASH_BLOCK_BAD;
  
  KICK_WATCHDOG();

  /* Save new bad block info */
  //Block(bad) = Block(n)
  failed_block = pctl->current_block;
  failed_page = pctl->current_page;
  
  //Page(error) = Page(i % Block Size)
  /* Skip new bad block */
  //Block(n) = Block(n) + 1

next_good_block:
  /* Check if Block(n) is bad */
  //while( Block(n) == BLOCK_BAD )
  //   Block(n) = Block(n) + 1
  //   if( Block(n) == last block in partition )
  //      return ERROR
  if (FLASH_PROG_SUCCESS != flash_prog_prepare_next_block (pctl))
  {
    return FLASH_PROG_FAIL;
  }
  
  /* Found a good block. Now copy pages from  */
  /* new bad block to new good block */
  //for( Page(i)=0; Page(i)<=Page(error); Page(i)++ )
  //   page_src = Block(bad)+ Page(i)
  //   page_dst = Block(n) + Page(i)
  //   flash_read_pages(page_src, page_buf)
  //   flash_write_pages(page_dst, page_buf)
  page_idx=failed_block * pctl->flash_info.partition.pages_per_block;
  for (; page_idx <= failed_page; page_idx++)
  {
    FLASHLOG(1, ("bbm:  block(bad) 0x%lx, read page 0x%lx\n", (unsigned long int)failed_block,
      (unsigned long int)page_idx));

    if (page_idx < failed_page)
    {
      /* zero out buffer for readback*/
      memset(page_buffer, 0x0, pctl->page_size);

      result = flash_prog_read_page_and_udata(pctl->flash_handle, &pctl->flash_info,
        page_idx, read_type, page_buffer, readback_spare_buf,
        pctl->spare_bytes_len);

      if (FLASH_DEVICE_DONE != result)
      {
        FLASHLOG(1,("Read page error \n"));
        SET_ERR (ERR_READ_FAIL);
        break;
      }

      FLASHLOG(4, ("bbm:  block(good) 0x%lx, writing copy page 0x%lx\n", (unsigned long int)pctl->current_block,
        (unsigned long int)pctl->current_page));

      result = flash_prog_write_page_and_udata (pctl->flash_handle, &pctl->flash_info,
        pctl->current_page, write_type, page_buffer, readback_spare_buf,
        pctl->spare_bytes_len);

      /* Adjust page counters for alignment */
      pctl->current_page++;
      pctl->avail_pages_in_block--;
    }
    else
    {
      FLASHLOG(4, ("bbm:  block(good) 0x%lx, writing last copy page 0x%lx\n", (unsigned long int)pctl->current_block,
        (unsigned long int)pctl->current_page));

      result = flash_prog_write_page_and_udata (pctl->flash_handle, &pctl->flash_info,
        pctl->current_page, write_type, src_main, src_spare,
        pctl->spare_bytes_len);
    }

    if (FLASH_DEVICE_DONE != result)
    {
      FLASHLOG(1,("bbm: Write of page failed at 0x%lx.\n",
        (unsigned long int)pctl->current_page));
      SET_ERR (ERR_WRITE_FAIL);
      break;
    }
  }
  
  /* Check page copy is successful */
  //   if( flash_write_page == fail )
  //      mark Block(n) bad
  //      if( mark Block(n) bad == fail )
  //         return FLASH_PROG_FAIL;
  if (FLASH_DEVICE_DONE != result)
  {
    if (FLASH_DEVICE_DONE != flash_block_set_state(pctl->flash_handle,
      pctl->current_block, bad_block))
    {
      return FLASH_PROG_FAIL;
    }
    else
    {
      goto next_good_block;
    }
  }
  
  /* Now mark the Block(bad) as bad */
  //mark Block(bad) bad
  //if( mark Block(bad) bad == fail )
  //   return FLASH_PROG_FAIL
  if (FLASH_DEVICE_DONE == result)
  {
    if (FLASH_DEVICE_DONE != flash_block_set_state(pctl->flash_handle,
      failed_block, bad_block))
    {
      return FLASH_PROG_FAIL;
    }
  }
  
  FLASHLOG(4, ("bbm: Block marked BAD 0x%lx\n", (unsigned long int)
    failed_block));
  
  return FLASH_PROG_SUCCESS;
  
} /* flash_prog_bad_block_manager */

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
#endif
