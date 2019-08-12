#ifndef BOOT_SBL_QSEE_INTERFACE_H
#define BOOT_SBL_QSEE_INTERFACE_H

/*===========================================================================

                          Boot SBL QSEE Interface

GENERAL DESCRIPTION
 Contains common defintion used by SBL and QSEE.

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/14   ck      Added boot_sbl_qsee_interface_get_secboot_info
04/02/14   ck      Updated boot_sbl_qsee_interface to Bear 2.0 BAS spec
04/01/14   ck      Updated pbl_secboot_verified_info_type to secboot_verified_info_type
                   as Bear PBL now uses common secboot lib.
03/21/14   ck      Added boot_sbl_qsee_interface_get_eident and
                   boot_sbl_qsee_interface_get_entry_point functions
12/02/13   ck      Initial creation
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include BOOT_PBL_H
#include "miprogressive.h"
#include "miheader.h"
#include "secboot.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* SBL - QSEE Magic Number 1 */
#define SBL_QSEE_MAGIC_NUMBER_1  0xCE473D94

/* SBL - QSEE Magic Number 2 */
#define SBL_QSEE_MAGIC_NUMBER_2  0x3CB237BA


/*---------------------------------------------------------------------------
  SBL Boot Images Entry Structure
  
  For each image loaded by SBL, there is a matching image entry in the
  boot_sbl_qsee_interface structure that is passed to QSEE at the end of 
  SBL execution.  The boot_images_entry structure is populated by the logic
  that loads the image.
---------------------------------------------------------------------------*/

typedef struct boot_images_entry
{
  secboot_sw_type image_id;
  uint32 e_ident;
  uint64 entry_point;
  secboot_verified_info_type image_verified_info;
  uint32 reserved_1;
  uint32 reserved_2;
  uint32 reserved_3;
  uint32 reserved_4;

} boot_images_entry;


/*---------------------------------------------------------------------------
  SBL - QSEE Interface Structure
  
  The following is needed by QSEE and is populated during SBL execution.
---------------------------------------------------------------------------*/

typedef struct boot_sbl_qsee_interface
{  
  uint32 magic_1;
  uint32 magic_2;
  uint32 version;
  uint32 number_images;
  uint32 reserved_1;
  boot_images_entry boot_image_entry[BOOT_IMAGES_NUM_ENTRIES];

} boot_sbl_qsee_interface;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
                                   secboot_sw_type image_id);


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
                                        secboot_sw_type image_id);


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
                                         secboot_sw_type image_id);

#endif /* BOOT_SBL_QSEE_INTERFACE_H */
