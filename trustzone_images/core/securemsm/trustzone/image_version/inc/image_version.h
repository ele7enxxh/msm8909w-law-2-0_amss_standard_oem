#ifndef IMAGE_VERSION_H
#define IMAGE_VERSION_H

/*===========================================================================

GENERAL DESCRIPTION
  This file contains declaration of functions and other defines used for 
  image version reporting feature.

Copyright 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/image_version/inc/image_version.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/13   dhaval  declarations and defines for image version module
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

typedef enum
{
  IMG_VER_ERROR_NONE = 0,
  IMG_VER_ERROR_SMEM_ALLOC_FAILED,
  IMG_VER_ERROR_TABLE_NOT_INITIALIZED,
  IMG_VER_ERROR_TABLE_SIZE_WRONG,
  IMG_VER_ERROR_OUT_OF_BOUND_INDEX,
  IMG_VER_ERROR_ENUM_SIZE = 0x7FFFFFFF
} img_ver_error_type;

typedef enum
{
  IMAGE_INDEX_SBL 		= 0,
  IMAGE_INDEX_TZ		= 1,
  IMAGE_INDEX_TZSECAPP          = 2,
  IMAGE_INDEX_RPM 		= 3,
  IMAGE_INDEX_SDI 		= 4,
  IMAGE_INDEX_HYPERVISOR 	= 5,
  IMAGE_INDEX_RESERVED1 	= 6,
  IMAGE_INDEX_RESERVED2 	= 7,
  IMAGE_INDEX_RESERVED3 	= 8,
  IMAGE_INDEX_APPSBL 		= 9,
  IMAGE_INDEX_APPS 		= 10,
  IMAGE_INDEX_MPSS 		= 11,
  IMAGE_INDEX_ADSP 		= 12,
  IMAGE_INDEX_WCNS 		= 13,
  IMAGE_INDEX_VENUS             = 14,
  /* MAX 32 entries, each worth of 128bytes amounts to 4K version table total 
     so max usable index is 31 */
  IMAGE_INDEX_MAX               = 32,
  IMAGE_INDEX_ENUM_SIZE 	= 99
} image_index_type;

/*===========================================================================

**  Function :  image_version_populate_version

** ==========================================================================
*/
/*!
* 
* @brief
*   This function populates the image_version in image_versoin_table in smem   
*   based on image index/allocated-entry.
*  
* @param[in] image_index index in the smem_image_version_table
*
* @par Dependencies
*   smem must have been initialized.
*   
* @retval
*   returns status: 0 value represents no error, for other errors review
*   image_version_error_type                                                               
* 
* @par Side Effects
*   None
* 
*/
uint32 image_version_populate_version
(
  image_index_type image_index
);

/*===========================================================================

**  Function :  image_version_initialize_version_table

** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes the image_version_table in smem with spaces.
*  
* @param
*   None
*
* @par Dependencies
*   smem must have been initialized and smem element for image_version_table
*   allocated.
*   
* @retval
*   returns status: 0 value represents no error, for other errors review
*   image_version_error_type                                                               
* 
* @par Side Effects
*   Initializes whole SMEM_IMAGE_VERSION_TABLE with spaces
*   This API should only be called once right after smem_boot_init by the image
*   doing smem_boot_init.
* 
*/
uint32 image_version_initialize_version_table
(
  void
);

#endif  /* IMAGE_VERSION_H */

