#ifndef TZBSP_IMAGE_VERSION_H
#define TZBSP_IMAGE_VERSION_H

/*===========================================================================

GENERAL DESCRIPTION
  This file contains declaration of functions and other defines used for 
  TZBSP image version reporting feature.

Copyright 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/image_version/inc/tzbsp_image_version.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/13   ah      initial version
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "image_version.h"

typedef enum
{
  TZBSP_IMAGE_INDEX_TZ = IMAGE_INDEX_TZ,
  TZBSP_IMAGE_INDEX_TZSECAPP = IMAGE_INDEX_TZSECAPP,
} tzbsp_image_index_type;

/*===========================================================================

**  Function :  image_version_populate_version

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is a tzbsp wrapper for image_version_populate_version API   
*  
* @param[in] image_index index in the smem_image_version_table
*
* @par Dependencies
*   smem must have been initialized.
*   
* @retval
*   returns status: E_SUCCESS if successful, negative otherwise                       
* 
* @par Side Effects
*   None
* 
*/
int tzbsp_image_version_populate_version(tzbsp_image_index_type image_index);

#endif  /* IMAGE_VERSION_H */

