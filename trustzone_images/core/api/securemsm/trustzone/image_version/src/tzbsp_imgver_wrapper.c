/*===========================================================================
   Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/trustzone/image_version/src/tzbsp_imgver_wrapper.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/27/13   ah      Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "tzbsp_errno.h"
#include "tzbsp_image_version.h"

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
int tzbsp_image_version_populate_version(tzbsp_image_index_type image_index)
{
  uint32 ret=0;

  ret = image_version_populate_version((image_index_type) image_index);

  return ((ret == IMG_VER_ERROR_NONE)?E_SUCCESS:-E_FAILURE);
}

