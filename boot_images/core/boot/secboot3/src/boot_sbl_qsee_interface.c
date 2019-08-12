/*===========================================================================

                         Boot SBL QSEE Interface File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for sbl-qsee
  interface utilities.

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who         what, where, why
--------   ---         --------------------------------------------------
05/14/14   ck          Added boot_sbl_qsee_interface_get_secboot_info
03/21/14   ck          Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_sbl_qsee_interface.h"


/*=========================================================================
                            
                       FUNCTION DEFINITIONS

=========================================================================*/

/*===========================================================================

**  Function :  boot_sbl_qsee_interface_get_eident

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that retrieves the eident from the sbl qsee interface
*   for the given image type.
*
* @par Dependencies
*   None
*
* @retval
*   uint32 ELF ident of image type passed if found in sbl qsee interface.
           Returns 0 if not found.
* 
* @par Side Effects
*   None
* 
*/
uint32
boot_sbl_qsee_interface_get_eident(boot_sbl_qsee_interface * sbl_qsee_interface,
                                   secboot_sw_type image_id)
{
  uint32 image_count = 0;
  uint32 eident_value = 0;


  /* Search image entries in QSEE interface for image_id and get elf type. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id == image_id)
    {
      eident_value = sbl_qsee_interface->boot_image_entry[image_count].e_ident;
      break;
    }
  } 

  return eident_value;
}


/*===========================================================================

**  Function :  boot_sbl_qsee_interface_get_entry_point

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that retrieves the entry point from the sbl qsee interface
*   for the given image type.
*
* @par Dependencies
*   None
*
* @retval
*   uint32 Entry point of image type passed if found in sbl qsee interface.
           Returns 0 if not found.
* 
* @par Side Effects
*   None
* 
*/
uint32
boot_sbl_qsee_interface_get_entry_point(boot_sbl_qsee_interface * sbl_qsee_interface,
                                        secboot_sw_type image_id)
{
  uint32 image_count = 0;
  uint32 entry_point = 0;


  /* Search image entries in QSEE interface for image_id and get entry_point. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id == image_id)
    {
      entry_point =
        sbl_qsee_interface->boot_image_entry[image_count].entry_point;

      break;
    }
  } 

  return entry_point;
}


/*===========================================================================

**  Function :  boot_sbl_qsee_interface_get_secboot_info

** ==========================================================================
*/
/*!
* 
* @brief  
*   Utility function that retrieves the secboot_verified_info pointer
*   from the sbl qsee interface for the given image type.
*
* @par Dependencies
*   None
*
* @retval
*   secboot_verified_info_type * Pointer to the secboot verified info structure
*                                of image type passed if found in sbl qsee
*                                interface.  Returns NULL if not found.
* 
* @par Side Effects
*   None
* 
*/
secboot_verified_info_type *
boot_sbl_qsee_interface_get_secboot_info(boot_sbl_qsee_interface * sbl_qsee_interface,
                                         secboot_sw_type image_id)
{
  uint32 image_count = 0;
  secboot_verified_info_type * secboot_verified_info = NULL;


  /* Search image entries in QSEE interface for image_id and get entry_point. */
  for (image_count = 0;
       image_count < BOOT_IMAGES_NUM_ENTRIES;
       image_count++)
  {
    if (sbl_qsee_interface->boot_image_entry[image_count].image_id == image_id)
    {
      secboot_verified_info = (secboot_verified_info_type *)
        &(sbl_qsee_interface->boot_image_entry[image_count].image_verified_info);

      break;
    }
  } 

  return secboot_verified_info;
}
