/*=======================================================================
 * FILE:        nand_entry.c
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * DESCRIPTION
 *  This file has the main functions used for programming the flash.
 *
 * Copyright (c) 2008-2012, 2014 Qualcomm Technologies Incorporated.
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
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nand/nand_entry.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/14/14     sb      Remove stress test to reduce tool size
 * 02/15/12     sv      Adding Erase NAND capability in EDL/QPST download
 * 10/30/12     sv      Initialize the DAL Heap
 * 09/13/12     sv      Call bam_drv_init to initialize bam driver
 * 04/13/11     jz      Pass on return result
 * 03/18/11     jz      Pass over partition table size
 * 08/12/10     nr      Moved nand_log.h reference to nand_wrapper.h
 * 01/01/10     op      Added support Read/Write WM7 Boot config 
 * 12/04/09     wt      Added the support for ROFS Partition Dump
 * 12/03/09     wt      Fix to Remove the warnings  
 * 11/03/09     wt      Added the support for XSR USER DATA Partition Dump
 * 02/20/09     jz      Renamed flash_ with flash_prog_
 * 10/29/08     mm      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "nand_core.h"
#include "nand_param.h"
#include "nand_param.h"
#include "nand_utils.h"
#include "nand_wrapper.h"
#include "DALSys.h"
#include "DALHeap.h"
#include "bam_drv.h"

#pragma arm section zidata = "FLASH_TOOLS_HEAP_ZONE"
uint8 flash_tools_heap[FLASH_TOOLS_HEAP_SIZE];
#pragma arm section zidata
/*
 * Entry to flash programmer.
 */
#ifdef BUILD_NAND
void main_c(void)
#else
void nand_dispatch(void)
#endif
{
  uint32 ret_code = 0;
  static uint32 flash_tools_init = 0;

  if (flash_tools_init == 0)
  {
    DALSYS_InitMod(NULL);
    DALSYS_HeapInit(flash_tools_heap, FLASH_TOOLS_HEAP_SIZE, 1);
    (void)bam_drv_init();
    flash_tools_init = 1;
  }

  switch ( prog_param.status )
  {

    case FLASH_PROG_SECURITY_MODE:
    {
      ret_code = flash_prog_set_security_mode();
      break;
    }

    case FLASH_PROG_USR_PARTI_TBL:
    {
      ret_code = flash_prog_init_parti(prog_param.size);
      break;
    }

    case FLASH_PROG_INITIALIZE:
    {
      ret_code = flash_prog_image_init();
      break;
    }

    case FLASH_PROG_PROGRAM:
    {
      if (current_mode == MODE_WM_APPS)
      {
        ret_code = flash_prog_program_flash_bin(&image);
      }
      else if (current_mode == MODE_FACTORY_IMG)
      {
        ret_code = flash_prog_program_factory_img(&image);
      }
      else
      {
        ret_code = flash_prog_program_img(&image);
      }
      break;
    }

    case FLASH_PROG_FINALIZE:
    {
      ret_code = flash_prog_image_finalize ();
      break;
    }

    case FLASH_PROG_GET_FLASH_PARAM:
    {
      ret_code = flash_prog_get_flash_info();
      break;
    }

    case FLASH_PROG_ERASE_CHIP:
    {
      ret_code = flash_prog_erase_chip();
      break;
    }

#if defined(BUILD_NAND) && !defined(BUILD_FOR_X86)

    case FLASH_PROG_STRESS_TEST:
    {
      /* This function never returns unless something is very wrong */
      {
//        extern int flash_dal_stress_test();
//        ret_code = flash_dal_stress_test();
      }
      break;
    }

    case FLASH_PROG_REPORT_BAD:
    {
      ret_code = flash_utils_list_bad_blocks();
      break;
    }

    case FLASH_PROG_MARK_BLOCK_BAD:
    {
      ret_code = flash_utils_invalidate_block(prog_param.addr);
      break;
    }

    case FLASH_PROG_MARK_BLOCK_GOOD:
    {
      ret_code = flash_utils_make_block_good(prog_param.addr);
      break;
    }

    case FLASH_PROG_ERASE_BLOCK:
    {
      ret_code = flash_utils_erase_block(prog_param.addr);
      break;
    }

    case FLASH_PROG_READ_PAGE:
    {
      ret_code = flash_utils_read_page();
      break;
    }

    case FLASH_PROG_FILL_PAGE:
    {
      ret_code = flash_utils_fill_page();
      break;
    }

    case FLASH_PROG_RETRIEVE_ID :
    {
      ret_code = flash_utils_read_id();
      break;
    }

    case FLASH_PROG_DUMP_INIT:
    {
      ret_code = flash_prog_init_parti_dump();
      break;
    }

    case FLASH_PROG_PARTI_DUMP:
    {
      ret_code = flash_prog_dump_parti();
      break;
    }

    case FLASH_PROG_PRINT_MIBIB:
    {
      ret_code = flash_prog_print_mibib();
      break;
    }

    case FLASH_PROG_PRINT_LAYOUT:
    {
      ret_code = flash_prog_print_layout();
      break;
    }

    case FLASH_PROG_VALID_BOOT_BLOCK:
    {
      ret_code = flash_utils_validate_boot_block();
      break;
    }

    case FLASH_PROG_DUMP_MIBIB_PARTI_BLOCK:
    {
      ret_code = flash_prog_dump_mibib_parti_block();
      break;
    }

    case FLASH_PROG_READ_UUID:
    {
      ret_code = flash_utils_read_uuid();
      break;
    }
    
    case FLASH_DISP_XSR_ROFS_PARTITIONS:
    {
      ret_code = flash_display_rofs_partitions();      
      break;
    }

    case FLASH_PROG_DUMP_INIT_XSR_USER_DATA:
    {
      ret_code = flash_prog_init_parti_xsr_userdata_dump();      
      break;
    }

    case FLASH_PROG_PARTI_DUMP_XSR_USER_DATA:
    {
      ret_code = flash_prog_dump_xsr_userdata_parti();
      break;
    }

    case FLASH_PROG_DUMP_INIT_XSR_ROFS:
    {
     ret_code = flash_prog_init_parti_xsr_rofs_dump();

      break;
    }

    case FLASH_PROG_PARTI_DUMP_XSR_ROFS:
    {
      ret_code = flash_prog_dump_xsr_rofs_parti();
      break;
    }

    case FLASH_PROG_READ_BCONFIG:
    {
      ret_code = flash_utils_bldr_read_boot_config();
      break;
    }

    case FLASH_PROG_WRITE_BCONFIG:
    {
      ret_code = flash_utils_bldr_write_boot_Config();
      break;
    }

#endif
    default:
    {
      ret_code = FLASH_PROG_FAIL;
      break;
    }
  }

  /* Return error code back to TRACE32, 0 = success, 0x100 = fail
   * We have also stuffed our own error code in param.reserved
   * so that it is easier to debug what exactly went wrong */
  prog_param.status = ret_code;

}
