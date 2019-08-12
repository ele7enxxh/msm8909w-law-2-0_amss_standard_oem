/*===========================================================================

                        Boot Rollback Version Image Set

GENERAL DESCRIPTION
  This header file contains the definition of
  the rollback version image set table.

Copyright 2013 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/15   jz      Added missing SECBOOT_FLASH_PRG_SW_TYPE 
02/28/14   jz      Updated for 9x45
04/16/13   kedara  Added MPSS and MBA image type to image set
01/31/13   kedara  Added Apps image type to image set
01/07/13   kedara  Initial creation

============================================================================*/
/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_rollback_version.h"
#include "boot_rollback_qfprom.h"
#include "boot_msm.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

struct boot_version_rollback_img_set boot_rollback_version_img_set_table[] =
{
  /* image set 0, including SBL1 image */
  {
    /* images in this set*/
    {
      SECBOOT_SBL1_SW_TYPE, 
      SECBOOT_MAX_SW_TYPE, 
      SECBOOT_FLASH_PRG_SW_TYPE 
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_ADDR,
    
    /* feature_enable_mask */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_BOOT_ANTI_ROLLBACK_EN_BMSK,
    
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

  /* image set 1, including TZBSP */
  {
    /* images in this set*/
    {
      SECBOOT_TZ_KERNEL_SW_TYPE, 
      SECBOOT_MAX_SW_TYPE, 
      SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_ADDR,
    
    /* feature_enable_mask */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_BOOT_ANTI_ROLLBACK_EN_BMSK,
    
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
  
  /* image set 2, including RPM */
  {
    /* images in this set*/
    {
      SECBOOT_RPM_FW_SW_TYPE, 
      SECBOOT_MAX_SW_TYPE, 
      SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_ADDR,
    
    /* feature_enable_mask */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_BOOT_ANTI_ROLLBACK_EN_BMSK,
    
    /* version_lsb_base_addr*/
    NULL,
    /* version_lsb_mask*/
    0x0,
    /* version_lsb_shift */
    0x0,
    
    /* version_msb_base_addr */
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,
    
     /* version_msb_mask */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_BMSK,
    
    /* version_msb_shift*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_SHFT,
     /* current_set_version_num*/
    0,
  },  
  
  /* image set 3, including APPSBL */
  {
    /* images in this set*/
    {
      SECBOOT_APPSBL_SW_TYPE, 
      SECBOOT_APPS_HASH_TABLE_TYPE, 
      SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_ADDR,
    
    /* feature_enable_mask */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_BOOT_ANTI_ROLLBACK_EN_BMSK,
    
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

  /* image set 5, including MBA */
  {
    /* images in this set*/
    {
      SECBOOT_MBA_SW_TYPE,
      SECBOOT_MAX_SW_TYPE,
      SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_ADDR,
    
    /* feature_enable_mask */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_MSA_ANTI_ROLLBACK_EN_BMSK,
    
    /* version_lsb_base_addr*/
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,
    
    /* version_lsb_mask*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MBA_BMSK,
    
    /* version_lsb_shift */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MBA_SHFT,
    
    /* version_msb_base_addr */
    NULL,
     /* version_msb_mask */
    0x0,
    /* version_msb_shift*/
    0x0,
     /* current_set_version_num*/
    0,
  },

  /* image set 6, including MPSS */
  {
    /* images in this set*/
    {
      SECBOOT_AMSS_HASH_TABLE_SW_TYPE,
      SECBOOT_MAX_SW_TYPE,
      SECBOOT_MAX_SW_TYPE
    },
    
    /* feature_enable_base_addr */
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_ADDR,
    
    /* feature_enable_mask */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_FEATURE_EN_MSA_ANTI_ROLLBACK_EN_BMSK,
    
    /* version_lsb_base_addr*/
    (uint32 *)HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,
    
    /* version_lsb_mask*/
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MSS_BMSK,
    
    /* version_lsb_shift */
    HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_MSS_SHFT,
    
    /* version_msb_base_addr */
    NULL,
     /* version_msb_mask */
    0x0,
    /* version_msb_shift*/
    0x0,
     /* current_set_version_num*/
    0,
  }  
};

uint32 boot_rollback_version_img_set_num = sizeof(boot_rollback_version_img_set_table)/sizeof(struct boot_version_rollback_img_set);




