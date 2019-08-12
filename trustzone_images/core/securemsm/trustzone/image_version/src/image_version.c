/*===========================================================================

GENERAL DESCRIPTION
  This file contains functions and other defines used for 
  image version reporting feature.

Copyright 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/image_version/src/image_version.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/21/13   dhaval  Implements APIs to initialize SMEM_IMAGE_VERSION_TABLE and
                   to populate image_version_entry in SMEM_IMAGE_VERSION_TABLE
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "smem.h"
#include "string.h"
#include "image_version.h"
#include "image_version_oem_auto_update.h"
#include "image_version_qc_auto_update.h"

#define SMEM_IMAGE_VERSION_TABLE_SIZE		4096
/* below entry size is used in the code instead of sizeof(struct) to ensure
   that we never go over 128 bytes for image_version_entry population in smem */
#define SMEM_IMAGE_VERSION_ENTRY_SIZE		128

#define VERSION_TAG_LENGTH  			15

/* Image version entry lengths total = 128 */
#define IMAGE_INDEX_LENGTH			2
#define SEP_LENGTH				1
#define QC_IMAGE_VERSION_STRING_LENGTH		92
/* there is another colon seperatror worth **1 byte** between QC and OEM version */
#define OEM_IMAGE_VERSION_STRING_LENGTH         32

/* Image version entry strings */
#define IMAGE_VERSION_PREFIX_TAG		"IMAGE_VERSION: "
#define IMAGE_INDEX 				"  "
#define COLON_SEP 				":"
#define QC_IMAGE_VERSION_STRING 		QC_IMAGE_VERSION_STRING_AUTO_UPDATED
#define OEM_IMAGE_VERSION_STRING 		OEM_IMAGE_VERSION_STRING_AUTO_UPDATED

/* Image Version Entry structure: image-version-entry fields which gets 
   populated in image_version_table */
struct image_version_entry
{
  char image_index[IMAGE_INDEX_LENGTH];
  char colon_sep1[SEP_LENGTH];
  char qc_image_version_string[QC_IMAGE_VERSION_STRING_LENGTH];
  char colon_sep2[SEP_LENGTH];
  char oem_image_version_string[OEM_IMAGE_VERSION_STRING_LENGTH];
};

/* Image Version Info structure: image-version-info fields which gets
   embedded in image binary */
struct image_version_info
{
  char version_tag[VERSION_TAG_LENGTH];
  struct image_version_entry img_version_entry;
};

/* Image Version Info object which gets embedded in image binary's data 
   section */
static struct image_version_info img_version_info = 
{ 
  IMAGE_VERSION_PREFIX_TAG, 
  { 
    IMAGE_INDEX,
    COLON_SEP, 
    QC_IMAGE_VERSION_STRING, 
    COLON_SEP, 
    OEM_IMAGE_VERSION_STRING 
  } 
};

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

**  Function :  image_version_populate_index

** ==========================================================================
*/
/*!
* 
* @brief
*   This function populates the ascii image_index in image_version_entry 
*   struct.
*  
* @param[in] image_index index in the smem_image_version_table
*
* @par Dependencies
*   None
*   
* @retval
*   returns status: 0 value represents no error, for other errors review
*   image_version_error_type                                                               
* 
* @par Side Effects
*   None
* 
*/
static uint32 image_version_populate_index
(
  image_index_type image_index
)
{
  uint32 status = IMG_VER_ERROR_OUT_OF_BOUND_INDEX;
  uint32 img_index = (uint32) image_index;
  
  /* If image_index is valid then update image_index fields in 
     image_version_entry structure with ascii image_index */
  if (image_index < IMAGE_INDEX_MAX)
  {
    img_version_info.img_version_entry.image_index[1] = '0' + (img_index % 10);
    img_index = img_index/10;
    img_version_info.img_version_entry.image_index[0] = '0' + (img_index % 10);
    status = IMG_VER_ERROR_NONE;
  }
  return status;
} /* image_version_populate_index() */

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
)
{
  uint32 status = IMG_VER_ERROR_TABLE_NOT_INITIALIZED;
  uint32 image_version_table_size;
  struct image_version_entry *image_version_entry_ptr;
  
  /* get pointer to smem_version_table base */
  uint32 *image_version_table_ptr = (uint32*) smem_get_addr ( SMEM_IMAGE_VERSION_TABLE,
                                           &image_version_table_size );
  do 
  {
    if (image_version_table_ptr != NULL)
    {
      if (image_version_table_size != SMEM_IMAGE_VERSION_TABLE_SIZE)
      {
        status = IMG_VER_ERROR_TABLE_SIZE_WRONG;
        break;
      }
	  
      if (image_index >= IMAGE_INDEX_MAX)
      {
        status = IMG_VER_ERROR_OUT_OF_BOUND_INDEX;
	break;
      }

      /* based on passed image_index, populate ascii image_index in 
         image_version_entry */
      status = image_version_populate_index(image_index);
      if (status != IMG_VER_ERROR_NONE)
      {
        break;
      }

      /* locate image_version_entry in image_version table based on 
         assigned image_index */
      image_version_entry_ptr = (struct image_version_entry *) ( (uint32)image_version_table_ptr 
			        + (SMEM_IMAGE_VERSION_ENTRY_SIZE * (uint32)image_index) );

      /* populate image_version_entry in image_version_table at assigned index */
      *image_version_entry_ptr = img_version_info.img_version_entry;

      status = IMG_VER_ERROR_NONE;
    }
  }while(0);
  
  return status;
} /* image_version_populate_version() */

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
)
{
  uint32 status = IMG_VER_ERROR_SMEM_ALLOC_FAILED;

  /* allocate and retrieve pointer for SMEM_IMAGE_VERSION_TABLE */
  uint32 *image_version_table_ptr = smem_alloc( SMEM_IMAGE_VERSION_TABLE,
                                           SMEM_IMAGE_VERSION_TABLE_SIZE );
  
  if (image_version_table_ptr != NULL)
  {
    /* initialize image_version_table with ascii space(0x20) */
    memset((void*)image_version_table_ptr, 0x20, SMEM_IMAGE_VERSION_TABLE_SIZE); 
	status = IMG_VER_ERROR_NONE;
  }
  
  return status;
} /* image_version_populate_version() */
