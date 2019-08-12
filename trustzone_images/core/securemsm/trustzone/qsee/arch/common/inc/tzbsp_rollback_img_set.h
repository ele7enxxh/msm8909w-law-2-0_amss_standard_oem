#ifndef ROLLBACK_VERSION_IMG_SET_H
#define ROLLBACK_VERSION_IMG_SET_H
/*===========================================================================

                        Boot Rollback Version Image Set

GENERAL DESCRIPTION
  This header file contains the definition of
  the rollback version image set table.

Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/14   hw      Initial creation

============================================================================*/
/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "tzbsp_rollback_version_fuseprov.h"
#include "tzbsp_hwio.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* RAW region for write and CORR region for read */
#define ROLLBACK_FUSE_RAW_REGION_MIN (HWIO_QFPROM_RAW_JTAG_ID_ADDR)
#define ROLLBACK_FUSE_RAW_REGION_MAX (HWIO_ACC_IR_ADDR)
#define ROLLBACK_FUSE_CORR_REGION_MIN (HWIO_QFPROM_CORR_JTAG_ID_ADDR)
#define ROLLBACK_FUSE_CORR_REGION_MAX (HWIO_SEC_CTRL_HW_VERSION_ADDR)                     

struct rollback_version_img_set rollback_version_img_set_table[] =
{
  /* image set 0  SBL Image */
  {
    /* images in this set*/
    {
      RBV_SECBOOT_SBL1_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR,
    
    /* feature_enable_mask */
    0x80000,
    
    /* version_lsb_base_addr*/
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,
    
    /* version_lsb_mask*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_SBL1_BMSK,
    
    /* version_lsb_shift */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_SBL1_SHFT,
    
    /* version_msb_base_addr */
    NULL,
     /* version_msb_mask */
    0x0,
    /* version_msb_shift*/
    0x0,
     /* current_set_version_num*/
    0,
  },


  /* image set 1  QSEE Image */
  {
    /* images in this set */
    {
      RBV_SECBOOT_QSEE_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR,
    
    /* feature_enable_mask */
    0x80000,
    
    /* version_lsb_base_addr*/
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,
    
    /* version_lsb_mask*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_TZ_BMSK,
    
    /* version_lsb_shift */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_TZ_SHFT,
    
    /* version_msb_base_addr */
    NULL,
     /* version_msb_mask */
    0x0,
    /* version_msb_shift*/
    0x0,
     /* current_set_version_num*/
    0,
  },
  

  /* image set 2,  QHEE Image */
  {
    /* images in this set*/
    {
      RBV_SECBOOT_QHEE_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR,
    
    /* feature_enable_mask */
    0x80000,
    
    /* version_lsb_base_addr*/
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,
    
    /* version_lsb_mask*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_HYPERVISOR_BMSK,
    
    /* version_lsb_shift */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_HYPERVISOR_SHFT,
    
    /* version_msb_base_addr */
    NULL,
     /* version_msb_mask */
    0x0,
    /* version_msb_shift*/
    0x0,
     /* current_set_version_num*/
    0,
  },  
  

  /* image set 3,  RPM Image */
  {
    /* images in this set*/
    {
      RBV_SECBOOT_RPM_FW_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR,
    
    /* feature_enable_mask */
    0x80000,
    
    /* version_lsb_base_addr*/
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,
    
    /* version_lsb_mask*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_BMSK,
    
    /* version_lsb_shift */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_SHFT,
    
    /* version_msb_base_addr */
    NULL,
    
    /* version_msb_mask */
    0x0,
    
    /* version_msb_shift*/
    0x0,

    /* current_set_version_num*/
    0,
  },


  /* image set 4,  APPSBL Image */
  {
    /* images in this set*/
    {
      RBV_SECBOOT_APPSBL_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE, 
      RBV_SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR,
    
    /* feature_enable_mask */
    0x80000,
    
    /* version_lsb_base_addr*/
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,
    
    /* version_lsb_mask*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_APPSBL0_BMSK,
    
    /* version_lsb_shift */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_APPSBL0_SHFT,
    
    /* version_msb_base_addr */
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,

     /* version_msb_mask */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_APPSBL2_BMSK,

    /* version_msb_shift*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_APPSBL2_SHFT,

    /* current_set_version_num*/
    0,
  }, 

};


#endif /* ROLLBACK_VERSION_IMG_SET_H */

