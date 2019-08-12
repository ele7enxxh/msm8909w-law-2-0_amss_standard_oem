/*=======================================================================
 * FILE:        nor_core.c
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach FLASH_PROG/ICD TRACE32
 *
 * DESCRIPTION
 *   This file has the main functions used for programming the flash
 *
 * Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
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
*   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/nor_entry.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 11/01/12     sv      Initialize the DAL Heap
* 04/06/11     jz      Added support to read platform ID from EEPROM
* 09/10/10     sv      Initial version
*==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "nor_core.h"
#include "nor_utils.h"
#include "nor_param.h"
#include "DALSys.h"
#include "DALHeap.h"

extern int flash_dal_stress_test(void);

uint8 flash_tools_heap[FLASH_TOOLS_HEAP_SIZE];

/*
 * This function calls the appropriate routine based on what TRACE32/Other External 
 * clients told us.  
 */
void main_c(void)
{
  uint32 ret_code = 0;
  static uint32 flash_tools_init = 0;
  if (flash_tools_init == 0)
  {
    DALSYS_InitMod(NULL);
    DALSYS_HeapInit(flash_tools_heap, FLASH_TOOLS_HEAP_SIZE, 1);
    flash_tools_init = 1;
  }

  switch ( prog_param.status )
  {
    case FLASH_PROG_INITIALIZE:
      ret_code = flash_prog_init_image();
      break;

    case FLASH_PROG_PROGRAM:
      ret_code = flash_prog_program_img();
      break;

    case FLASH_PROG_ERASE_CHIP:
      ret_code = flash_prog_erase_chip();
      break;

    case FLASH_PROG_STRESS_TEST:
      ret_code = flash_dal_stress_test();
      break;

    case FLASH_PROG_ERASE_BLOCK:
      ret_code = flash_utils_erase_block(prog_param.addr);
      break;

    case FLASH_PROG_READ_PAGE:
      ret_code = flash_utils_read_page();
      break;
    
    case FLASH_PROG_FILL_PAGE:
      ret_code = flash_utils_fill_page();
      break;

    case FLASH_PROG_DUMP_INIT:
      ret_code = flash_prog_init_parti_dump();
      break;  

    case FLASH_PROG_PARTI_DUMP:
      ret_code = flash_prog_dump_parti();
      break;  
    
    case FLASH_PROG_READ_PLATFORM_ID:
      ret_code = flash_utils_read_platform_id();
      break;
    
    default:
      ret_code = FLASH_PROG_FAIL;
      SET_ERR (ERR_BAD_CMD);
      break;
    }
  /* Return error code back to TRACE32/other external clients, 0 = success, 
   * 0x100 = fail
   */
  prog_param.status = ret_code;

} /* main_c */
