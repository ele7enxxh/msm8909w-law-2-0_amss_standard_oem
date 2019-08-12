/*=======================================================================
* FILE:        nor_core_v1.c
*
* SERVICES:    Flash Programming Plug-In support
*
* DESCRIPTION :
*   This file has the functions used for programming the flash 
*   
*
* Copyright (c) 2010,2012 Qualcomm Technologies Incorporated.
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
*   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/config_v1/nor_core_v1.c#1 $ 
*   $DateTime: 2015/09/01 00:30:35 $ 
*   $Author: pwbldsvc $
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 02/28/12     sv      Fix Klocwork error
* 09/10/10     sv      Initial version
*==================================================================*/
#include "nor_core.h"

static unsigned char parti_name[FLASH_NOR_PARTI_NAME_LENGTH];
/*
 * This function performs the necessary initialization to program the 
 * device for different Images 
 */
uint32 flash_prog_init_image(void)
{
  flash_prog_img_type_t image_id = (flash_prog_img_type_t)prog_param.reserved;
  uint8 *data = (uint8 *)prog_param.data;

  /* Currently only Single(Unified) image programming is supported. This can 
   * be enhanced to program individual images.
   */
   KICK_WATCHDOG();
   switch(image_id)
  {
    case FLASH_PROG_SINGLE_IMG:
      FLASHLOG (3,("\n\n------ Initialize for Single Image ------\n"));
      memcpy(parti_name, FLASH_DEVICE_ALL_PARTI_NAME, 
	    sizeof(FLASH_DEVICE_ALL_PARTI_NAME));
      break;

    case FLASH_PROG_CUSTOM_IMG:
      FLASHLOG (3,("\n\n------ Initialize for Custom Image ------\n"));
      memcpy(parti_name, data, FLASH_NOR_PARTI_NAME_LENGTH);
      break;

    default:
      FLASHLOG (3,("\n\n------ Not supported ------\n"));
      SET_ERR (ERR_UNKNOWN_IMG);
      return FLASH_PROG_FAIL;
  }

  /* Open the partition to use for this image */
  if (flash_prog_init_for_partition_image (parti_name) != FLASH_PROG_SUCCESS)
  {
    SET_ERR (ERR_NO_MIBIB_BLOCK);
    return FLASH_PROG_FAIL;
  }
  
  KICK_WATCHDOG();

  FLASHLOG (3,("===========  Done ===========\n\n"));
 
  return FLASH_PROG_SUCCESS;
} /* flash_prog_init_image */
