#ifndef BOOT_VERSION_ROLLBACK_H
#define BOOT_VERSION_ROLLBACK_H

/*===========================================================================

                        Boot Module Interface Header File

GENERAL DESCRIPTION
  This header file contains the definition of the public interface to
  the rollback version prevention feature.

Copyright 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/23/14   jz      Added back fuse blow support in SBL
12/09/13   ck      Removed functions for burning fuses as this is done post SBL in Bear family
10/25/12   dh      Initial creation for B family 

============================================================================*/
/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "miheader.h"
#include "boot_error_if.h"
#include "boot_sbl_shared.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

#define BOOT_VERSION_ROLLBACK_MAX_IMG_NUM_PER_SET 3

/*
  Data structure that represent a set of images that share the same anti rollback
  version fuses. 
  Each image set can contain a max of BOOT_VERSION_ROLLBACK_MAX_IMG_NUM_PER_SET images.
  Each image set can have up to two fuses to store the version: one is lsb the other is msb,
  which also means an image set's absolute max version number will be 32+32=64.
  Each image set has one fuse which indicates if rollback protection is enable for current image set or not
  Each image set also has one variable that store the current version(obtain from secboot cert after authentication)
*/
struct boot_version_rollback_img_set
{
  /* List of images in current image set*/
  const secboot_sw_type sec_sw_img_list[BOOT_VERSION_ROLLBACK_MAX_IMG_NUM_PER_SET];
  
  /* Base address of the fuse that defines anti rollback feature for current image set*/
  const uint32 * const feature_enable_base_addr;
  
  /* Bit Mask that should be applies to the fuse that defines anti rollback feature for current image set*/
  const uint32 feature_enable_mask;
  
  /* Base address of the fuse that defines the LSB of version number of current image set*/
  const uint32 * const version_lsb_base_addr;
  
  /* Bit Mask that should be applies to the LSB fuse to extract LSB of version number*/
  const uint32 version_lsb_mask;
  
  /* Bit Shift that should be applies to the LSB fuse to extract LSB of version number*/
  const uint32 version_lsb_shift;
  
  /* Base address of the fuse that defines the MSB of version number of current image set*/
  const uint32 * const version_msb_base_addr;  
  
  /* Bit Mask that should be applies to the MSB fuse to extract MSB of version number*/
  const uint32 version_msb_mask;
  
  /* Bit Shift that should be applies to the MSB fuse to extract MSB of version number*/
  const uint32 version_msb_shift;

  /* variable that store the current version(obtain from secboot cert after authentication) */
  uint32 current_set_version_num;
  
};

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

**  Function : boot_rollback_init_img_set_table

** ==========================================================================
*/
/*!
* 
* @brief
*    This function initializes the rollback version image set table.
*
* @param[in] img_set_table pointer to the table
*
* @param[in] img_set_num number of images sets the table contains
*
*
* @par Dependencies
*    None
* 
* @retval None                                 
* 
* 
*/
#ifdef FEATURE_BOOT_VERSION_ROLL_BACK
  void boot_rollback_init_img_set_table
  (
    bl_shared_data_type *bl_shared_data,
    struct boot_version_rollback_img_set *img_set_table,
    uint32 img_set_num
  );
#else
  static inline void boot_rollback_init_img_set_table
  (
    bl_shared_data_type *bl_shared_data,
    struct boot_version_rollback_img_set *img_set_table,
    uint32 img_set_num
  )
  {
  }
#endif



/*===========================================================================

**  Function : boot_rollback_get_fuse_version

** ==========================================================================
*/
/*!
* 
* @brief
*    This function returns the image version that's stored on the fuse given a
*    image type
*
* @param[in] img_sw_type secure image type
*
* @param[out] fuse_version pointer to the image version to be populated
*
*
* @par Dependencies
*    None
* 
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error                              
* 
* 
*/
#ifdef FEATURE_BOOT_VERSION_ROLL_BACK
  bl_error_type boot_rollback_get_fuse_version
  (
    secboot_sw_type img_sw_type,
    uint32 *fuse_version
  );
#else
  static inline bl_error_type boot_rollback_get_fuse_version
  (
    secboot_sw_type img_sw_type,
    uint32 *fuse_version
  )
  {
    *fuse_version = 0;
    return BL_ERR_NONE;
  }
#endif


/*===========================================================================

**  Function : boot_rollback_update_img_version

** ==========================================================================
*/
/*!
* 
* @brief
*    This function updates the current image version obtained from cert 
*    in the image set table given an image type
*
* @param[in] img_sw_type secure image type
*
* @param[out] img_version  the latest image versio obtained from cert
*
*
* @par Dependencies
*    None
* 
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error                              
* 
* 
*/
#ifdef FEATURE_BOOT_VERSION_ROLL_BACK
  bl_error_type boot_rollback_update_img_version
  (
    secboot_sw_type img_sw_type,
    uint32 img_version
  );
#else
  static inline bl_error_type boot_rollback_update_img_version
  (
    secboot_sw_type img_sw_type,
    uint32 img_version
  )
  {
    return BL_ERR_NONE;
  }
#endif

/*===========================================================================

**  Function : boot_rollback_update_fuse_version

** ==========================================================================
*/
/*!
* 
* @brief
*    Given an image type, this function updates the fuse image version accociated
*    with this image by blowing the fuse if the current version stored in the 
*    image set table is greater than the version stored in the fuse.
*
* @param[in] img_sw_type secure image type
*
*
* @par Dependencies
*    None
* 
*    bl_error_type Returns error code in case of error
*                  Returns BL_ERR_NONE in case of no error                              
* 
* 
*/
#ifdef FEATURE_BOOT_VERSION_ROLL_BACK
  bl_error_type boot_rollback_update_fuse_version
  (
    secboot_sw_type img_sw_type
  );
#else
  static inline bl_error_type boot_rollback_update_fuse_version
  (
    secboot_sw_type img_sw_type
  )
  {
    return BL_ERR_NONE;
  }
#endif

#endif /*BOOT_VERSION_ROLLBACK_H*/
