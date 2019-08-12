/*=======================================================================
 * FILE:        nand_core_v2.c
 *
 * SERVICES:    Flash Programming Plug-In support for SB2.0 configuration
 *
 * DESCRIPTION
 *   This file has the functions used for programming the flash that are
 *   specific for SecureBoot 2.0 architecture
 *
 * Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/config_v2/nand_core_v2.c#1 $
 *   $DateTime: 2015/09/01 00:30:35 $
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/14/14     sb      Move page buffers to Code RAM to support non DDR based tools
 * 06/21/13     sb      Clean up as part of removing user partition table from MIBIB 
 * 02/15/13     sv      Clean up
 * 09/11/12     eo      Clean up
 * 07/25/12     eo      Remove call to BBT block erase for DSP/Apps images
 * 05/30/12     bb      Support to erase chip when partition table is empty
 * 03/01/12     sv      Fix potential invalid memory access issue
 * 09/22/11     sv      Support to program SBL1, DBL using custom partition
 * 08/16/11     jz      ehostdl image size reduction
 * 08/20/11     sv      DSP3 support for 9x15
 * 06/22/11     eo      SBLx and RPM support for 9x15
 * 04/11/11     jz      Reduce unnecessary outputs
 * 09/10/10     bb      Fix for unaligned memcpy failure with RVCT 4.0
 * 09/01/10     nr      Defer first block erase in tools till stream write
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 07/12/10     jz      Fixed QDSP image download in interleaved mode
 * 05/24/10     jz      Restore back the lines removed by last checkin
 * 04/12/10     mh      Revert to the latest
 * 04/12/10     mh      Revert to #6 & add dual nand ionterleave mode support
 * 02/02/10     op      Support OSBL, DSP, AMSS image update to 8650A target
 * 09/18/09     op      Make DM buffer 32byte alignment to avoid any misaligned
 * 07/08/09     mm      Factory Image Programming to use MAIN_SPARE
 * 06/03/09     mh      Clean ups
 * 05/20/09     jz      Clean up gcc related compiler warnings
 * 02/27/09     jz      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_micrc.h"
#include "flash_mibib.h"
#include "crc.h"

#include "nand_wrapper.h"
#include "nand_core.h"
#include "nand_core_v2.h"
#include "nand_param.h"
#include "nand_config.h"  /* for KICK_WATCHDOG() */

/* If flash tools get compiles as part of bootloaders, where the data caches is
 * setup, the 32 bytes aligment changes of source buffers here would make them
 * more cache line friendly and avoid any misaligned boot data structure that
 * gets invalidated
 */
#if !defined(BUILD_EHOSTDL) && !defined(BUILD_HOSTDL)
#pragma arm section zidata = "FLASH_TOOLS_HEAP_ZONE"
#endif
ALIGN(ALIGN_VAL) unsigned char mibib_pg_buf[MAX_PAGE_SIZE];
#if !defined(BUILD_EHOSTDL) && !defined(BUILD_HOSTDL)
#pragma arm section zidata
#endif

struct parti_ctl image;

struct boot_block_ctl boot_layout;

int flash_hw_present = TRUE;

/* Keep track of which mode we are in so finalize can do its various
 * different endings.
 */
mode_type current_mode = MODE_NONE;

static int flash_prog_make_mibib_valid(uint32 mibib_block);

/*
 * This function will erase the next available non-bad block in the flash
 * and update all global variables to point to it and its pages as the
 * next available for use.  If the current block is not bad it will be
 * prepared as the current block.  If the current block is bad, this
 * function will advance to the first non-bad block.
 * If the current mode is one which needs magic numbers, the first page
 * will be written with magic numbers and the page count will be
 * one less and the next available page will be page 1 in the block.
 */
int flash_prog_prepare_this_block (parti_ctl_t pctl)
{
  int erase_result_data;
  enum flash_block_state blk_state;
  struct flash_block_vector block_vector;
  uint32 magic_buf[6];

  FLASHLOG( 4, ("Block number = %ld", (unsigned long int)pctl->current_block));

  if (DAL_SUCCESS != flash_block_get_state(pctl->flash_handle,
    pctl->current_block, &blk_state))
  {
    return FLASH_PROG_FAIL;
  }

  /* Advance to next non-bad block */
  while (blk_state == FLASH_BLOCK_BAD)
  {
    FLASHLOG (2, ("ptb: skipping bad block 0x%lx   \n",
      (unsigned long int)pctl->current_block));
    pctl->current_block++;

    /* while programming the image keep track of the bad blocks */
    if (current_mode == MODE_FACTORY_IMG)
    {
      uint32 current_parti = pctl->factory_img.curr_parti;

      /* Increment the bad block count for this partition */
      pctl->factory_img.bb_count[current_parti]++;

      /* Make sure the partition data will still fit. Reject the flash
       * device if the number of bad blocks plus actual blocks
       * of data is greater then available blocks in the partition
       */

      if ((pctl->factory_img.parti_tbl[current_parti].end_addr -
        pctl->factory_img.parti_tbl[current_parti].start_addr + 1) <
        (pctl->factory_img.parti_tbl[current_parti].actual_blks +
        pctl->factory_img.bb_count[current_parti]) )
      {
        FLASHLOG (1,("\nprog: Rejecting Device !!!"));
        FLASHLOG (1,("\nprog: Too many bad blocks in this partition!!!"));
        FLASHLOG (1,("\nprog: Increase the spare blocks in this partition. "));
        SET_ERR(ERR_PAST_NAND_END);
        return FLASH_PROG_FAIL;
      }
    }
    if (DAL_SUCCESS != flash_block_get_state(pctl->flash_handle,
      pctl->current_block, &blk_state))
    {
      return FLASH_PROG_FAIL;
    }
  }

  /* Make sure we did not go past the end of the flash */
  if (pctl->current_block >= pctl->flash_info.partition.block_count)
  {
    FLASHLOG (1,("Error: Attempted to program beyond end of device\n"));
    SET_ERR (ERR_PAST_NAND_END);
    return FLASH_PROG_FAIL;
  }

  /* erase the block */
  block_vector.start_block = pctl->current_block;
  block_vector.block_count = 1;
  block_vector.result_vector = &erase_result_data;
  if (DAL_SUCCESS != flash_erase_blocks(pctl->flash_handle, &block_vector, 1))
  {
    FLASHLOG (1,("Error: device failed during erase\n"));
    SET_ERR (ERR_ERASE_FAIL);
    return FLASH_PROG_FAIL;
  }

  /* do housekeeping to set up to use block just erased */
  pctl->current_page =
    pctl->current_block * pctl->flash_info.partition.pages_per_block;
  pctl->avail_pages_in_block = pctl->flash_info.partition.pages_per_block;
  write_type = FLASH_WRITE_MAIN;
  read_type = FLASH_READ_MAIN;

  /* Write Magic Number page if necessary */
  switch (current_mode)
  {
    case MODE_FSBL:
    {
      magic_buf[0] = MI_FSBL_MAGIC1;
      magic_buf[1] = MI_FSBL_MAGIC2;
      if (FLASH_PROG_SUCCESS !=
        flash_prog_write_magic_number_page(pctl, magic_buf, 2))
      {
        FLASHLOG (3,("Error: failed to write magic number page\n"));
        SET_ERR (ERR_WRITE_FAIL);
        return FLASH_PROG_FAIL;
      }
      break;
    }

    case MODE_OSBL:
    {
      magic_buf[0] = MI_OSBL_MAGIC1;
      magic_buf[1] = MI_OSBL_MAGIC2;
      if (FLASH_PROG_SUCCESS !=
         flash_prog_write_magic_number_page(pctl, magic_buf, 2))
      {
        FLASHLOG (3,("Error: failed to write magic number page\n"));
        SET_ERR (ERR_WRITE_FAIL);
        return FLASH_PROG_FAIL;
      }
      break;
    }

    case MODE_OSBL_UPDATE:
    {
      magic_buf[0] = MI_OSBL_CHROME_MAGIC1;
      magic_buf[1] = MI_OSBL_CHROME_MAGIC2;
      magic_buf[2] = MI_OSBL_MAGIC1;
      magic_buf[3] = MI_OSBL_MAGIC2;
      if (FLASH_PROG_SUCCESS !=
        flash_prog_write_magic_number_page(pctl, magic_buf, 4))
      {
        FLASHLOG (3,("Error: failed to write magic number page\n"));
        SET_ERR (ERR_WRITE_FAIL);
        return FLASH_PROG_FAIL;
      }
      break;
    }

    case MODE_NONE:
    case MODE_DBL:
    case MODE_SBL:
    case MODE_RPM:
    case MODE_AMSS:
    case MODE_APPSBL:
    case MODE_APPS:
    case MODE_CEFS:
    case MODE_WM_APPS:
    case MODE_FOTAUI:
    case MODE_CEFS_APPS:
    case MODE_CUSTOM_IMG:
    case MODE_DSP1:
    case MODE_DSP2:
    case MODE_DSP3:
    case MODE_AMSS_UPDATE:
    case MODE_DSP_UPDATE:
    {
      write_type = FLASH_WRITE_MAIN;
      read_type = FLASH_READ_MAIN;
      break;
    }

    case MODE_RAW_IMG:
    {
      FLASHLOG (4,("Enter RAW image programming mode\n"));

      write_type = FLASH_WRITE_RAW;
      read_type = FLASH_READ_RAW;
      pctl->page_size =
        pctl->flash_info.partition.total_page_size_bytes;

      break;
    }

    case MODE_FACTORY_IMG:
    {
      FLASHLOG(4,("Enter Factory image programming mode\n"));
      if(pctl->flash_info.type == FLASH_ONENAND)
      {
        write_type = FLASH_WRITE_MAIN_SPARE;
        read_type = FLASH_READ_MAIN_SPARE;
      }
      else
      {
        write_type = FLASH_WRITE_RAW;
        read_type = FLASH_READ_RAW;
      }
      pctl->page_size =
        pctl->flash_info.partition.total_page_size_bytes;

      break;
    }
    default:
    {
      FLASHLOG (1,("Error: Programming mode 0x%x not valid\n", current_mode));
      return FLASH_PROG_FAIL;
    }
  }

  FLASHLOG (4, ("ptb:  new block at  0x%lx\n",
    (unsigned long int)pctl->current_block));

  return FLASH_PROG_SUCCESS;
} /* flash_prog_prepare_this_block */


/*
 *
 * This function calls the flash_open() function and sets up everything to be
 * ready to erase and program the device for image specified by T32 script.
 *
 */
int flash_prog_image_init(void)
{
  flash_prog_img_type_t image_id = (flash_prog_img_type_t) prog_param.reserved;
  char parti_name[FLASH_PART_NAME_LENGTH];

  KICK_WATCHDOG();

  if (FLASH_PROG_FACTORY_IMG != image_id)
  {
    /* Check for sending partition tbl cmd */
    if (!partition_rcvd)
    {
      FLASHLOG (1, ("Error: no partition table sent yet\n"));
      SET_ERR (ERR_PARTI_NOT_RCVD);
      return FLASH_PROG_FAIL;
    }
  }

  if (image_id >= FLASH_PROG_UNKNOWN_IMG)
  {
    FLASHLOG (1,("Error: Unknown image type encountered 0x%lx \n",
      (unsigned long int)image_id));
    SET_ERR (ERR_UNKNOWN_IMG);
    return FLASH_PROG_FAIL;
  }
  else
  {
    switch (image_id)
    {
      case FLASH_PROG_DBL_IMG:
      {
        nand_memcpy(parti_name, MIBIB_DEVICE_ALL_PARTI_NAME,
          sizeof(MIBIB_DEVICE_ALL_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for DBL ------\n"));
        current_mode = MODE_DBL;
        break;
      }

      case FLASH_PROG_FSBL_IMG:
      {
        nand_memcpy(parti_name, MIBIB_FSBL_PARTI_NAME,
          sizeof(MIBIB_FSBL_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for FSBL ------\n"));
        current_mode = MODE_FSBL;
        break;
      }

      case FLASH_PROG_OSBL_IMG:
      {
        nand_memcpy(parti_name, MIBIB_OSBL_PARTI_NAME,
          sizeof(MIBIB_OSBL_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for OSBL ------\n"));
        current_mode = MODE_OSBL;
        break;
      }

      case FLASH_PROG_SBL1_IMG:
      {
        nand_memcpy(parti_name, MIBIB_DEVICE_ALL_PARTI_NAME,
          sizeof(MIBIB_DEVICE_ALL_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for SBL1 ------\n"));
        current_mode = MODE_SBL;
        break;
      }

      case FLASH_PROG_SBL2_IMG:
      {
        nand_memcpy(parti_name, MIBIB_SBL2_PARTI_NAME,
          sizeof(MIBIB_SBL2_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for SBL2 ------\n"));
        current_mode = MODE_SBL;
        break;
      }

      case FLASH_PROG_RPM_IMG:
      {
        nand_memcpy(parti_name, MIBIB_RPM_PARTI_NAME,
          sizeof(MIBIB_RPM_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for RPM ------\n"));
        current_mode = MODE_RPM ;
        break;
      }

      case FLASH_PROG_AMSS_IMG:
      {
        nand_memcpy(parti_name, MIBIB_AMSS_PARTI_NAME,
          sizeof(MIBIB_AMSS_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for AMSS ------\n"));
        current_mode = MODE_AMSS;
        break;
      }

      case FLASH_PROG_APPS_BOOT_IMG:
      {
        nand_memcpy(parti_name, MIBIB_APPSBL_PARTI_NAME,
          sizeof(MIBIB_APPSBL_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for APPSBL ------\n"));
        current_mode = MODE_APPSBL;
        break;
      }

      case FLASH_PROG_APPS_IMG:
      {
        nand_memcpy(parti_name, MIBIB_APPS_PARTI_NAME,
          sizeof(MIBIB_APPS_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for APPS ------\n"));
        current_mode = MODE_APPS;
        break;
      }

      case FLASH_PROG_MODEM_CEFS_IMG:
      {
        nand_memcpy(parti_name, MIBIB_EFS2_MODEM_PARTI_NAME,
          sizeof(MIBIB_EFS2_MODEM_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for MODEM CEFS2 ------\n"));
        current_mode = MODE_CEFS;
        break;
      }

      case FLASH_PROG_APPS_CEFS_IMG:
      {
        nand_memcpy(parti_name, MIBIB_EFS2_APPS_PARTI_NAME,
          sizeof(MIBIB_EFS2_APPS_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for APPS CEFS2 ------\n"));
        current_mode = MODE_CEFS_APPS;
        break;
      }

      case FLASH_PROG_WM_IMG:
      {
        nand_memcpy(parti_name, MIBIB_EFS2_APPS_PARTI_NAME,
          sizeof(MIBIB_EFS2_APPS_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for Flash.bin ------\n"));
        current_mode = MODE_WM_APPS;
        break;
      }

      case FLASH_PROG_FOTAUI_IMG:
      {
        nand_memcpy(parti_name, MIBIB_FOTA_PARTI_NAME,
          sizeof(MIBIB_FOTA_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for FOTAUI ------\n"));
        current_mode = MODE_FOTAUI;
        break;
      }

      case FLASH_PROG_FACTORY_IMG:
      {
        nand_memcpy(parti_name, MIBIB_DEVICE_ALL_PARTI_NAME,
          sizeof(MIBIB_DEVICE_ALL_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for FACTORY IMAGE ------\n"));
        current_mode = MODE_FACTORY_IMG;
        break;
      }

      case FLASH_PROG_RAW_IMG:
      {
        nand_memcpy(parti_name, MIBIB_EFS2_APPS_PARTI_NAME,
          sizeof(MIBIB_EFS2_APPS_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for EFS2 APPS RAW IMAGE ------\n"));
        current_mode = MODE_RAW_IMG;
        break;
      }

      case FLASH_PROG_CUSTOM_IMG:
      {
        uint32 size_hd = prog_param.size;
        char * custom_parti_name = (char *) prog_param.data;
        if (size_hd <= 2)
        {
          /* Minimum partition name length has to be more than 2 bytes. If a
           * partition name is not available indicate failure.
           */

          FLASHLOG(3,("\nHeader size: %d\n", (unsigned int)size_hd));
          FLASHLOG (1,("Error: Partition name passed is too small\n"));
           return FLASH_PROG_FAIL;
        }
        else if(memcmp(custom_parti_name, NAND_TOOLS_SBL1_PARTI_NAME,
          sizeof(NAND_TOOLS_SBL1_PARTI_NAME)) == 0)
        {
          nand_memcpy(parti_name, MIBIB_DEVICE_ALL_PARTI_NAME, 
            sizeof(MIBIB_DEVICE_ALL_PARTI_NAME));		
          FLASHLOG(3,("\nInit for partition 0:SBL1\n"));
          current_mode = MODE_SBL;
	    }
        else if(memcmp(custom_parti_name, NAND_TOOLS_SBL1_PARTI_NAME,
          sizeof(NAND_TOOLS_DBL_PARTI_NAME)) == 0)
        {
          nand_memcpy(parti_name, MIBIB_DEVICE_ALL_PARTI_NAME, 
            sizeof(MIBIB_DEVICE_ALL_PARTI_NAME));		
          FLASHLOG(3,("\nInit for partition 0:DBL\n"));
          current_mode = MODE_DBL;
        }
        else
        {
          nand_memcpy(parti_name, custom_parti_name, sizeof(parti_name));
          FLASHLOG(3,("\nInit for partition \"%s\"\n", parti_name));
          current_mode = MODE_CUSTOM_IMG;
        }

        FLASHLOG (1,("\n\n------ Initialize for Custom Image ------\n"));
        KICK_WATCHDOG();
        break;
      }

      case FLASH_PROG_ADSP_IMG:
      {
        nand_memcpy(parti_name, MIBIB_ADSP_Q5_PARTI_NAME,
          sizeof(MIBIB_ADSP_Q5_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for ADSP_Q5 Image ------\n"));
        current_mode = MODE_CUSTOM_IMG;
        break;
      }

      case FLASH_PROG_DSP1_IMG:
      {
        nand_memcpy(parti_name, MIBIB_DSP1_PARTI_NAME,
          sizeof(MIBIB_DSP1_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for DSP1 Image ------\n"));
        current_mode = MODE_DSP1;
        break;
      }

      case FLASH_PROG_DSP2_IMG:
      {
        nand_memcpy(parti_name, MIBIB_DSP2_PARTI_NAME,
          sizeof(MIBIB_DSP2_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for DSP2 Image ------\n"));
        current_mode = MODE_DSP2;
        break;
      }

      case FLASH_PROG_DSP3_IMG:
      {
        nand_memcpy(parti_name, MIBIB_DSP3_PARTI_NAME,
          sizeof(MIBIB_DSP3_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for DSP3 ------\n"));
        current_mode = MODE_DSP3;
        break;
      }

      case FLASH_PROG_OSBL_UPDATE_IMG:
      {
        nand_memcpy(parti_name, MIBIB_OSBL_UPDATE_PARTI_NAME,
          sizeof(MIBIB_OSBL_UPDATE_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for MIBIB_OSBL_UPDATE_PARTI_NAME Image ------\n"));
        current_mode = MODE_OSBL_UPDATE;
        break;
      }

      case FLASH_PROG_AMSS_UPDATE_IMG:
      {
        nand_memcpy(parti_name, MIBIB_AMSS_UPDATE_PARTI_NAME,
          sizeof(MIBIB_AMSS_UPDATE_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for MIBIB_AMSS_UPDATE_PARTI_NAME Image ------\n"));
        current_mode = MODE_AMSS_UPDATE;
        break;
      }

      case FLASH_PROG_DSP_UPDATE_IMG:
      {
        nand_memcpy(parti_name, MIBIB_DSP_UPDATE_PARTI_NAME,
          sizeof(MIBIB_DSP_UPDATE_PARTI_NAME));
        FLASHLOG (1,("\n\n------ Initialize for MIBIB_DSP_UPDATE_PARTI_NAME Image ------\n"));
        current_mode = MODE_DSP_UPDATE;
        break;
      }

      default:
      {
        SET_ERR (ERR_UNKNOWN_IMG);
        return FLASH_PROG_FAIL;
      }
    } //switch
  } // else

  /* Open the partition to use for this image */
  if ((flash_prog_init_for_partition_image(&image, parti_name)) != FLASH_PROG_SUCCESS)
  {
    SET_ERR (ERR_PARTI_INVALID);
    return FLASH_PROG_FAIL;
  }

  FLASHLOG(3,("Found partition %s\n", parti_name));
  FLASHLOG(3,("\t%s:: blk_start 0x%lx\n", parti_name,
    (unsigned long int)image.flash_info.partition.physical_start_block));
  FLASHLOG(3,("\t%s:: blk limit 0x%lx\n", parti_name,
    ((unsigned long int)image.flash_info.partition.physical_start_block +
     (unsigned long int)image.flash_info.partition.block_count)));

  KICK_WATCHDOG();

  /* Do any mode specific processing after opening the device. */
  switch (current_mode)
  {
    case MODE_DBL:
    case MODE_FSBL:
    case MODE_OSBL:
    case MODE_SBL:
    case MODE_RPM:
    case MODE_AMSS:
    case MODE_DSP3:
    case MODE_APPS:
    case MODE_APPSBL:
    case MODE_DSP1:
    case MODE_DSP2:
    {
      break;
    }

    case MODE_CEFS:
    case MODE_CEFS_APPS:
    case MODE_WM_APPS:
    case MODE_RAW_IMG:
    case MODE_FACTORY_IMG:
    case MODE_FOTAUI:
    case MODE_OSBL_UPDATE:
    case MODE_AMSS_UPDATE:
    case MODE_DSP_UPDATE:
    {
      KICK_WATCHDOG();

      /* Erase the contents of the entire partition */
      if (FLASH_PROG_SUCCESS != flash_prog_erase_partition (&image))
      {
        return FLASH_PROG_FAIL;
      }
      break;
    }

    case MODE_CUSTOM_IMG:
    {
      uint32 attribute;

      /* Check to see if this partition can be upgraded using custom upgrade
       * mechanism.
       */
      attribute = (uint32) ((image.flash_info.partition.attributes &
        FLASH_PART_ATTRIBUTE3_BMSK) >> FLASH_PART_ATTRIBUTE3_SHFT);
      if ((attribute == FLASH_PART_DEF_ATTRB3) ||
        (attribute != FLASH_PART_ALLOW_NAMED_UPGRD))
      {
        SET_ERR (ERR_UNKNWN_PARTI);
        return FLASH_PROG_FAIL;
      }
      else
      {
        KICK_WATCHDOG();

        /* Erase the contents of the entire partition */
        if (FLASH_PROG_SUCCESS != flash_prog_erase_partition (&image))
        {
          return FLASH_PROG_FAIL;
        }
      }
      break;
    }

    case MODE_NONE:
    default:
    {
      FLASHLOG (1,("jii:  Unknown mode 0x%x\n ", current_mode));
      return FLASH_PROG_FAIL;
    }
  }

  KICK_WATCHDOG();

  FLASHLOG (1,("===========  Done ===========\n\n"));

  return FLASH_PROG_SUCCESS;
} /* flash_prog_image_init */


/*
 *
 * Set the data values for correct block numbers in the MIBIB partition
 * based on security mode.
 *
 */
uint32 flash_prog_set_block_offsets_in_mibib(int security_mode)
{
  switch (security_mode)
  {
    case FLASH_MI_BOOT_SEC_MODE_NON_TRUSTED:
    case FLASH_MI_BOOT_SEC_MODE_TRUSTED:
      boot_layout.dbl_block = FLASH_MI_BLOCK_DBL;
      boot_layout.bbtb_block = FLASH_MI_BLOCK_BBTB;
      boot_layout.mibib_block = FLASH_MI_BLOCK_MIBIB;
      break;

    default:
      SET_ERR(ERR_SECURE_BAD_MODE);
      return FLASH_PROG_FAIL;
  }

  FLASHLOG (3, ("sboim:  DBL     block is 0x%x\n", boot_layout.dbl_block));
  FLASHLOG (3, ("sboim:  BBTB    block is 0x%x\n", boot_layout.bbtb_block));
  FLASHLOG (3, ("sboim:  MIBIB  block is 0x%x\n", boot_layout.mibib_block));

  FLASHLOG (1, ("Max page size is: %d bytes\n\n", MAX_PAGE_SIZE));
  FLASHLOG (1, ("===========  Done ===========\n\n"));

  return FLASH_PROG_SUCCESS;
} /* flash_prog_set_block_offsets_in_mibib */


/* Get the bad block table block number from MIBIB */
int flash_prog_get_bbtb_block_from_mibib(void)
{
  return boot_layout.bbtb_block;
} /* flash_prog_get_bbtb_block_from_mibib */


/*
 * This function will make an MIBIB valid by computing crc32 checksum of mibib
 * contents and storing them in MIBIB CRC page. Before calculating crc it will
 * check for the block to be a valid MIBIB block with version page, system and
 * user partition tbls present in it.
 */
static int flash_prog_make_mibib_valid(uint32 mibib_block)
{
  uint32 temp_page;
  uint32 crc32 = 0x0UL;
  uint32 page_size_in_bits;
  mi_boot_info_t mibib_magic;
  flash_partable_t parti_sys;
  flash_micrc_data_t mibib_crc;
  uint8 fill_char = 0xFF;
  int result = FLASH_DEVICE_DONE;
  enum flash_block_state blk_state;

  /* If the block is bad, it cannot be a valid MIBIB */
  if (DAL_SUCCESS != flash_block_get_state(mibib.flash_handle, mibib_block,
    &blk_state))
  {
    return FLASH_PROG_FAIL;
  }

  if (blk_state == FLASH_BLOCK_BAD)
  {
    FLASHLOG(1,("fmmv: MIBIB block failed bad block check\n"));
    return FLASH_PROG_FAIL;
  }

  /* We have a good MIBIB block. Since the magic, system partition and user
   * partition pages are already read into buffers we can use these buffers
   * to compute crc on mibib contents. Note that we can do incremental
   * computation by passing the result of previous operation as seed for next
   * operation. This makes the sequence of invocation with respect to the page
   * buffers rigid.
   */

  temp_page = mibib_block * mibib.flash_info.partition.pages_per_block;
  page_size_in_bits = (uint32)mibib.flash_info.partition.page_size_bytes * 8;

  /* If the magic page does not contain valid ID, it cannot be a valid MIBIB */
  result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
    temp_page, read_type, mibib_pg_buf);

  FLASHLOG(1,("fmmv: Read of MIBIB page 0x%lx.\n",
    (unsigned long int)temp_page));

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(1,("fmmv: Read of magic page 0x%lx failed. Bailing out\n",
      (unsigned long int)temp_page));
    return FLASH_PROG_FAIL;
  }

  mibib_magic = (mi_boot_info_t)mibib_pg_buf;

  if ( (mibib_magic->magic1 != MIBIB_MAGIC1) ||
       (mibib_magic->magic2 != MIBIB_MAGIC2) ||
       (mibib_magic->version != MIBIB_VERSION) )
  {
    FLASHLOG (1, ("fmmv: MIBIB magic or version number mismatch\n"));
    return FLASH_PROG_FAIL;
  }

  crc32 = crc_32_calc( (uint8 *)mibib_pg_buf, page_size_in_bits, crc32);

  KICK_WATCHDOG();

  /* If the sys partiton page does not contain valid ID, it cannot be a valid
   * MIBIB
   */
  temp_page++;

  FLASHLOG(1,("fmmv: Read of MIBIB page 0x%lx.\n",
    (unsigned long int)temp_page));

  result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
    temp_page, read_type, mibib_pg_buf);

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(1,("fmmv: Read of sys parti page 0x%lx failed. Bailing out\n",
      (unsigned long int)temp_page));
    return FLASH_PROG_FAIL;
  }

  parti_sys = (flash_partable_t)mibib_pg_buf;

  if ( (parti_sys->magic1 != FLASH_PART_MAGIC1) ||
       (parti_sys->magic2 != FLASH_PART_MAGIC2) ||
       (parti_sys->version != FLASH_PARTITION_VERSION) )
  {
    FLASHLOG (1, ("fmmv: System partition magic or version number mismatch\n"));
    return FLASH_PROG_FAIL;
  }

  crc32 = crc_32_calc( (uint8 *)mibib_pg_buf, page_size_in_bits, crc32);

  KICK_WATCHDOG();

  /* We already know that MIBIB and partition table pages are there and are
   * good. We also have crc checksun for those pages. Prepare the buffer to
   * write the CRC page to validate MIBIB.
   */

  /* Use page buffer to construct the CRC page. Write this page to MIBIB block
   * in flash making it a valid MIBIB block */
  DALSYS_memset((void *)mibib_pg_buf, fill_char, mibib.flash_info.partition.page_size_bytes);

  /* If the CRC page does not contain valid ID and version number this MIBIB
   * block is corrupted.
   */
  mibib_crc = (flash_micrc_data_t)mibib_pg_buf;

  mibib_crc->magic1 = FLASH_MIBIB_CRC_MAGIC1;
  mibib_crc->magic2 = FLASH_MIBIB_CRC_MAGIC2;
  mibib_crc->version = FLASH_MIBIB_CRC_VERSION;
  mibib_crc->crc = crc32;

  FLASHLOG(3, ("fmmv: CRC for mibib block is 0x%lx\n",
    (unsigned long int)crc32));

  KICK_WATCHDOG();

  temp_page = temp_page + 2;

  FLASHLOG(4, ("fmmv: Writing CRC checksum to page 0x%lx\n",
    (unsigned long int)temp_page));

  result = flash_prog_write_page(mibib.flash_handle, &mibib.flash_info,
    temp_page, write_type, mibib_pg_buf);

  if (FLASH_DEVICE_DONE != result)
  {
    FLASHLOG(1,("fmmv: Write of MIBIB crc page failed at 0x%lx.\n",
      (unsigned long int)temp_page));
    return FLASH_PROG_FAIL;
  }

  FLASHLOG(3, ("fmmv: MIBIB successfully made valid\n"));

  return FLASH_PROG_SUCCESS;
} /* flash_prog_make_mibib_valid */


/*
 *
 * This function makes the new MIBIB valid by writing the first page.  If
 * we had no MIBIBs when we began programming, it will create a dummy MIBIB
 * so that later functions will not have a problem with an empty MIBIB.
 * The age is set to be older than the actual new one we are currently
 * writing.
 */
int flash_prog_write_mibib(void)
{
  uint32 temp_page = 0;
  int result = FLASH_DEVICE_DONE;
  struct flash_block_vector block_vector;
  int erase_result_data;
  flash_partable_t oldest_parti_ptr =
    (flash_partable_t)oldest_parti_buffer;
  mi_boot_info_t mibib_magic_ptr = (mi_boot_info_t)mibib_pg_buf;

  /* Now write the first page the MIBIB magic page. If we had a previous MIBIB,
   * read it in and increment the age and then write it out. Otherwise, compose
   * an MIBIB magic page with an age of 1 and write it out to the oldest
   * (before swap) block and initialize the newest as dummy block we have saved
   * away earlier. Compose an MIBIB magic page with an age of 0 and write it
   * out to newest along with system and user partition tbls.
   */
  FLASHLOG(3,("fuma:  On entry, newest MIBIB 0x%x oldest MIBIB 0x%x\n",
             newest_mibib_block, oldest_mibib_block));

  if (newest_mibib_block == -1)
  {
    FLASHLOG(3,("fuma:  Initializing first MIBIB\n"));

    DALSYS_memset(mibib_magic_ptr, 0xFF,
                  mibib.flash_info.partition.page_size_bytes);

    /* No previous MIBIB to copy from, initialize a new one */
    mibib_magic_ptr->magic1 = MIBIB_MAGIC1;
    mibib_magic_ptr->magic2 = MIBIB_MAGIC2;
    mibib_magic_ptr->version = MIBIB_VERSION;
    mibib_magic_ptr->age = 1;

    /* Erase the MIBIB block to initialize it */
    FLASHLOG (3, ("fwm:  erasing block 0x%x\n", oldest_mibib_block));

    /* erase the block */
    block_vector.start_block = oldest_mibib_block;
    block_vector.block_count = 1;
    block_vector.result_vector = &erase_result_data;
    if (DAL_SUCCESS != flash_erase_blocks(mibib.flash_handle, &block_vector, 1))
    {
      FLASHLOG (1, ("fwm: Error: device failed during erase\n"));
      SET_ERR (ERR_ERASE_FAIL);
      return FLASH_PROG_FAIL;
    }

    /* Now write the MIBIB magic page */
    temp_page = (oldest_mibib_block * mibib.flash_info.partition.pages_per_block) +
      MIBIB_PAGE_MAGIC_VERSION_PAGE;

    FLASHLOG(3,("fuma: Write new MIBIB magic page to page 0x%lx\n",
      (unsigned long int)temp_page));

    result = flash_prog_write_page(mibib.flash_handle, &mibib.flash_info,
      temp_page, write_type, mibib_magic_ptr);

    if (FLASH_DEVICE_DONE != result)
    {
      FLASHLOG(1,("fuma: Error: device failed during write\n"));
      SET_ERR (ERR_WRITE_FAIL);
      return FLASH_PROG_FAIL;
    }

    KICK_WATCHDOG();

    /* Now write the dummy sys partition table */
    temp_page++;
    FLASHLOG(3,("fwm:  Writing page 0x%lx with sys partition table\n",
      (unsigned long int)temp_page));

    result = flash_prog_write_page(mibib.flash_handle, &mibib.flash_info,
      temp_page, write_type, oldest_parti_ptr);

    if (FLASH_DEVICE_DONE != result)
    {
      FLASHLOG (1, ("fwm: Error: device failed during write\n"));
      SET_ERR (ERR_WRITE_FAIL);
      return FLASH_PROG_FAIL;
    }

    KICK_WATCHDOG();

    if (FLASH_PROG_SUCCESS != flash_prog_make_mibib_valid(oldest_mibib_block))
    {
      FLASHLOG (1, ("fwm: MIBIB validation failed\n"));
      SET_ERR (ERR_MIBIB_VALID_FAIL);
      return FLASH_PROG_FAIL;
    }
  }

  return FLASH_PROG_SUCCESS;
} /* flash_prog_write_mibib */

/*
 * This function sets for programming when MIBIB block was not found in
 * flash
 */
int flash_prog_setup_mibib_not_present (void)
{
  KICK_WATCHDOG();

  /* No MIBIB block present. Find a block to place MIBIB in.*/
  FLASHLOG(3,("Entry flash_prog_setup_mibib_not_present\n"));
  FLASHLOG(3,("smnp: No partition tbls found at all in NAND\n"));

  FLASHLOG(3,("smnp: Finding nth good block for block 0x%x for MIBIB\n",
             boot_layout.mibib_block));

  /* Unconditional chip erase - To address security concerns where
   * partition table area is erased by some malicious entity and which
   * can lead to a security vulnerability where modem data get
   * exposed to apps. To prevent that we are erasing the chip
   * unconditionally to wipe out all previous data contents.
   */

  FLASHLOG(3, ("Erasing entire chip\n"));

  if (flash_prog_erase_chip() == FLASH_PROG_FAIL)
  {
    FLASHLOG(1, ("Partition table override:  erase chip failed\n"));
    SET_ERR (ERR_ERASE_FAIL);
    return FLASH_PROG_FAIL;
  }
  else
  {
    FLASHLOG(3, ("Flash erased successfully\n"));
    KICK_WATCHDOG();
  }

  oldest_mibib_block = flash_prog_find_nth_good_block(mibib.flash_handle,
    boot_layout.mibib_block);

  if (oldest_mibib_block == -1)
  {
    FLASHLOG(1, ("smnp: Failed to find MIBIB Block\n"));
    SET_ERR (ERR_NO_MIBIB_BLOCK);
    return FLASH_PROG_FAIL;
  }
  else
  {
    /* Set up oldest MIBIB to be the next one we will write. */
    FLASHLOG(3,("smnp: Found MIBIB block at 0x%x to put oldest in\n",
               oldest_mibib_block));
  }

  KICK_WATCHDOG();

  return FLASH_PROG_SUCCESS;
} /* flash_prog_setup_mibib_not_present */


/*
 *
 * This function sets for programming when at least one MIBIB was
 * found in flash
 */
int flash_prog_setup_mibib_present (flash_usr_partable_t usr_parti_ptr,
                            flash_partable_t sys_parti_ptr)
{
  uint32 sys_parti_page;
  uint32 sys_parti_match_flag = 0;
  int starting_erase_block;
  int result = FLASH_DEVICE_DONE;
  flash_partable_t newest_parti_ptr = NULL;

  /* Determine if we should accept this new partition tbl.  First find
   * the newest partition tbl and compare it, and if identical, accept
   * it. If overrride command is given, allow a mismatch, otherwise,
   * indicate failure, and the host side can then send override.
   */

  /* We have at least one valid MIBIB.  Read in the partition tbl from
   * the newest MIBIB.  Compare the newest block's partition tbl to
   * the one which we were just given.
   */
  FLASHLOG(3,("smp: Entry setup_mibib_present\n"));
  FLASHLOG(3,("smp: Have at least one valid MIBIB block\n"));
  FLASHLOG(3,("smp: Newest MIBIB block at 0x%x\n", newest_mibib_block));
  FLASHLOG(3,("smp: Oldest MIBIB block at 0x%x\n", oldest_mibib_block));

  /* Compare the partition tables only if override flag (either conditional or
   * unconditional) is not present.
   */
  if (prog_param.reserved != FLASH_PROG_PARTI_OVERRRIDE)
  {
    /* If we only have one MIBIB block, it will be indicated as the newest. */
    sys_parti_page = (mibib.flash_info.partition.pages_per_block *
      newest_mibib_block) + MIBIB_PAGE_PARTITION_TABLE;

    result = flash_prog_read_page(mibib.flash_handle, &mibib.flash_info,
      sys_parti_page, read_type, mibib_pg_buf);

    if (FLASH_DEVICE_DONE != result)
    {
      FLASHLOG(1,("Page read of sys partition tbl failed for page 0x%lx\n",
        (unsigned long int)sys_parti_page));
      SET_ERR (ERR_READ_FAIL);
      return FLASH_PROG_FAIL;
    }

    FLASHLOG(3,("Read system partition table from newest MIBIB at block 0x%x\n",
               newest_mibib_block));

    newest_parti_ptr = (flash_partable_t)mibib_pg_buf;
    sys_parti_match_flag = flash_prog_partition_tables_are_equal (sys_parti_ptr,
                                               newest_parti_ptr);

    if (!sys_parti_match_flag)
    {
      FLASHLOG(1,("Derived sys partition tbl doesn't match with one in flash\n"));
    }
  }

  /* If partition tables are not equal, erase the entire flash. If tables are
   * equal, set up to use the correct one from flash.
   */
  if (!sys_parti_match_flag)
  {
    /* Please note that if partition override flash is set we are guramteed to
     * get here because system and user partition match flags are initialized
     * to be zero.
     */

    FLASHLOG(3,("Partition Table sent does not match what is in NAND\n"));
    if (prog_param.reserved == FLASH_PROG_PARTI_OVERRRIDE)
    {
      /* Use this partition table */
      FLASHLOG(3, ("Partition Table override in effect, erasing flash\n"));


      /* Call a function to determine where to start erasing.  If we
       * erase the entire part, we must set up conditions for no MIBIBs
       * present, otherwise, we set up for some present
       */
      starting_erase_block = flash_prog_find_starting_erase_block (sys_parti_ptr);

      if (starting_erase_block == -1)
      {
        FLASHLOG(1, ("Error finding starting erase block\n"));
        /* Error code set by called function */
        return FLASH_PROG_FAIL;
      }

      if (starting_erase_block == 0)
      {
        KICK_WATCHDOG();

        /* Set up conditions as if we have just returned from
         * flash_nand_find_mibib() and there were none present.  This
         * is because upon erasing, there are no MIBIB instances
         * present.  Then call flash_prog_setup_mibib_not_present() to do the
         * work of setup. This function will erase the chip.
         */
        oldest_mibib_block = -1;
        newest_mibib_block = -1;
        if (FLASH_PROG_SUCCESS != flash_prog_setup_mibib_not_present ())
        {
            return FLASH_PROG_FAIL;
        }
        else
        {
           /* Nothing to set up, call to flash_prog_setup_mibib_not_present() has
            * done it all for us.
           */
          return FLASH_PROG_SUCCESS;
        }
      }
      else
      { /* start erase past SIM_SECURE */
        FLASHLOG(3, ("Erasing some of chip from block 0x%x\n",
                    starting_erase_block));
        if (flash_prog_erase_some_of_chip(starting_erase_block) == FLASH_PROG_FAIL)
        {
          FLASHLOG(1, ("Partition table override:  erase partial chip failed\n"));
          SET_ERR (ERR_ERASE_FAIL);
          return FLASH_PROG_FAIL;
        }
        else
        {
          FLASHLOG(3, ("Flash erased successfully\n"));
          KICK_WATCHDOG();

          /* We now need to erase the MIBIB partition, leaving only
           * the SIM Secure partition untouched.
           */
          if (FLASH_PROG_SUCCESS != flash_prog_erase_MIBIB_partition(sys_parti_ptr))
          {
            FLASHLOG(1, ("Partition table override:  erase MIBIB failed\n"));
            SET_ERR (ERR_ERASE_FAIL);
            return FLASH_PROG_FAIL;
          }
          /* Call flash_prog_setup_mibib_not_present() even though we erased all
           * of the chip, because we also erased the MIBIB partition.
           */
          oldest_mibib_block = -1;
          newest_mibib_block = -1;
          if (FLASH_PROG_SUCCESS != flash_prog_setup_mibib_not_present ())
          {
            return FLASH_PROG_FAIL;
          }
          else
          {
            /* Nothing to set up, call to flash_prog_setup_mibib_not_present() has
             * done it all for us.
             */
            return FLASH_PROG_SUCCESS;
          }
        } /* FLASH_PROG_FAIL erasing some of chip */
      } /* Start erase at zero */
    } /* if override flag */
    else
    {
      /* Fail, user partition tables are not equal and no override */
      FLASHLOG(1, ("Partition Tables do not match\n"));
      SET_ERR (ERR_PARTI_NOMATCH);
      return FLASH_PROG_FAIL;
    }
  }
  else
  {
    /* Partition Tables match. Do structure assignment and continue. */
    FLASHLOG(3,("Partition Tables match\n"));
  }

  return FLASH_PROG_SUCCESS;

} /* flash_prog_setup_mibib_present */


/*
 *  This function will write any remaining data that is still buffered.
 *  It will then do any remaining tasks based on programming mode currently
 *  in force.
 *  Then just before exit, it will reset a bunch of variables.
 */
uint32 flash_prog_image_finalize(void)
{
  unsigned int num_fill_bytes;

  KICK_WATCHDOG();

  FLASHLOG (1,("\n----- FINALIZE ---------\n"));

  /* Maybe fill last page of code with fill character and write the
   * page.  Don't include these fill bytes in total fill bytes used
   * as that is keeping track only of wasted bytes internal
   * to the image.  */
  if (image.partial_page_index > 0)
  {
      FLASHLOG (3, ("flash_finalize:  Flushing last partial page\n"));
      num_fill_bytes = image.flash_info.partition.page_size_bytes -
        image.partial_page_index;

      while (num_fill_bytes > 0)
      {
        image.partial_page_buffer[image.partial_page_index++] =
        (unsigned char)NAND_FILL_CHAR;
        num_fill_bytes--;
      }

      if (FLASH_PROG_SUCCESS != flash_prog_write_partial_page_data (&image))
      {
        return FLASH_PROG_FAIL;
      }
  }

  KICK_WATCHDOG();

  switch (current_mode)
  {
    case MODE_DBL:
      FLASHLOG (3, ("DBL Image\n"));
      break;

    case MODE_FSBL:
      FLASHLOG (3, ("FSBL Image\n"));
      break;

    case MODE_OSBL:
      FLASHLOG (3, ("OSBL Image\n"));
      break;

    case MODE_SBL:
      FLASHLOG (3, ("SBLx Image\n"));
      break;
      
    case MODE_RPM:
      FLASHLOG (3, ("RPM Image\n"));
      break;

    case MODE_AMSS:
      FLASHLOG (3, ("AMSS Image\n"));
      /* Erase bad block table after AMSS update to force it being written
       * again.
       */
      if (FLASH_PROG_SUCCESS != flash_prog_erase_bad_block_table())
      {
         FLASHLOG (1, ("Error: Erase of bad block table failed\n"));
         return FLASH_PROG_FAIL;
      }
      break;

    case MODE_APPSBL:
      FLASHLOG (3, ("APPS BOOT Image\n"));
      break;

    case MODE_APPS:
      FLASHLOG (3, ("APPS Image\n"));
      break;

    case MODE_CEFS:
      FLASHLOG (3, ("Modem CEFS Image\n"));
      break;

    case MODE_FOTAUI:
      FLASHLOG (3, ("FOTA UI Image\n"));
      break;

    case MODE_CEFS_APPS:
      FLASHLOG (3, ("Apps CEFS Image\n"));
      break;

    case MODE_FACTORY_IMG:
      FLASHLOG (3, ("Factory Image\n"));
      break;
    case MODE_RAW_IMG:
      FLASHLOG (3, ("Raw Image\n"));
      break;

    case MODE_WM_APPS:
      FLASHLOG (3, ("Apps Flash.bin Image\n"));
      break;

    case MODE_CUSTOM_IMG:
      FLASHLOG (3, ("Custom Image\n"));
      break;

    case MODE_DSP1:
    case MODE_DSP2:
    case MODE_DSP3:
      FLASHLOG (3, ("DSP Image\n"));
      break;

    case MODE_NONE:
      FLASHLOG (1,("\flash_finalize:  Cannot finalize in MODE_NONE\n "));
      return FLASH_PROG_FAIL;

    case MODE_OSBL_UPDATE:
      FLASHLOG (3, ("OSBL Image UPDATE \n"));
      break;

    case MODE_AMSS_UPDATE:
      FLASHLOG (3, ("AMSS Image UPDATE \n"));
      break;

    case MODE_DSP_UPDATE:
      FLASHLOG (3, ("DSP Image UPDATE \n"));
      break;

    default:
      FLASHLOG (1,("\flash_finalize:  Unknown mode 0x%x\n ", current_mode));
      return FLASH_PROG_FAIL;
  }

  /* Set up variable to indicate nothing in progress. */
  current_mode = MODE_NONE;

  FLASHLOG (1,("===========  Done ===========\n\n"));

  return FLASH_PROG_SUCCESS;

} /* flash_prog_image_finalize */
