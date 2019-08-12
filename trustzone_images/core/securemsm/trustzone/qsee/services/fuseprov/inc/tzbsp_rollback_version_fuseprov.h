#ifndef ROLLBACK_VERSION_FUSEPROV_H
#define ROLLBACK_VERSION_FUSEPROV_H

/**
@file tzbsp_rollback_version_fuseprov.h
@brief for tz Rollback Protection fuse provision functions
*/

/*===========================================================================
   Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/fuseprov/inc/tzbsp_rollback_version_fuseprov.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/23/14   hw       Initial version.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"


#define VERSION_ROLLBACK_MAX_IMG_NUM_PER_SET 3

/* image sw_id that the module only supports */
typedef enum
{
  RBV_SECBOOT_SBL1_SW_TYPE               = 0x0,
  RBV_SECBOOT_QSEE_SW_TYPE               = 0x7, /* TZ is now called QSEE */
  RBV_SECBOOT_APPSBL_SW_TYPE             = 0x9,
  RBV_SECBOOT_RPM_FW_SW_TYPE             = 0xA,
  RBV_SECBOOT_QHEE_SW_TYPE               = 0x15,
  RBV_SECBOOT_MAX_SW_TYPE                = 0x7FFFFFFF /* force to 32 bits*/
} secboot_sw_type;


/* Error codes */
typedef enum {
	RBV_E_SUCCESS               =  0x0,
	RBV_E_FAILURE               =  0x1,
 	RBV_E_BAD_FUSE_ADDRESS      =  0x2,
	RBV_E_NULL_POINTER, 
	RBV_E_QFPROM_READ_ROW_FAILURE, 
  RBV_E_QFPROM_WRITE_ROW_FAILURE, 
  RBV_E_QFPROM_FUSE_NOT_SUPPORTED, 
	RBV_E_NO_LSB_BASE_ADDR,
	RBV_E_NO_MSB_BASE_ADDR,
  RBV_E_NO_AVAIL_VER_BITS,
  RBV_E_UPDATE_FUSE_VER_FAIL,
	RBV_E_IMAGE_NOT_SUPPORTED,
} rollback_version_fuseprov_err_type;

/*
  Data structure that represent a set of images that share the same anti rollback
  version fuses. 
  Each image set can contain a max of BOOT_VERSION_ROLLBACK_MAX_IMG_NUM_PER_SET images.
  Each image set can have up to two fuses to store the version: one is lsb the other is msb,
  which also means an image set's absolute max version number will be 32+32=64.
  Each image set has one fuse which indicates if rollback protection is enable for current image set or not
  Each image set also has one variable that store the current version(obtain from secboot cert after authentication)
*/
struct rollback_version_img_set
{
  /* List of images in current image set*/
  const secboot_sw_type sec_sw_img_list[VERSION_ROLLBACK_MAX_IMG_NUM_PER_SET];
  
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

**  Function : rollback_init_img_set_table

** ==========================================================================
*/
/*!
* 
* @brief
*    This function initializes the rollback version image set table.
*
* @param[in] img_set_table pointer to the img_set table
*
* @param[in] img_set_num number of images sets the table contains
*
* @param[in] is_auth_enabled flag to check if authentication is enabled
*
* @par Dependencies
*    None
* 
* @retval
*    rollback_version_fuseprov_err_type Returns error code in case of error
*                                       Returns RBV_E_SUCCESS in case of no error                               
* 
* 
*/
rollback_version_fuseprov_err_type rollback_init_img_set_table
(
  struct rollback_version_img_set *img_set_table,
  uint32 img_set_num,
  int is_auth_enabled
);

/*===========================================================================

**  Function : rollback_update_version_fuse

** ==========================================================================
*/
/*!
* 
* @brief
*    This function update the version rollback fuse for images.
*
* @param[in] sw_id software id of images
*
* @param[in] version the version to update to the fuse
*
* @par Dependencies
*    None
* 
* @retval
*    rollback_version_fuseprov_err_type Returns error code in case of error
*                                       Returns RBV_E_SUCCESS in case of no error                                     
* 
* 
*/
rollback_version_fuseprov_err_type rollback_update_version_fuse
(
  uint32 sw_id,
  uint32 version
);

#endif /* ROLLBACK_VERSION_FUSEPROV_H */
