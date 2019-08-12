/*===========================================================================

                       QFPROM  Driver Source  Code

DESCRIPTION
 Contains target specific defintions and APIs to be used to read and write
 qfprom values for sec ctrl 3.0.2 (used by 8x10).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright  2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/13   lm      Updated the qfprom region index based on the fusemap
07/01/13   lm      Ported and updated for 8x10 fuse map

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include QFPROM_HWIO_REG_INCLUDE_H
#include QFPROM_TARGET_INCLUDE_H

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/*
**  Array containing QFPROM data items that can be read and associated
**  registers, mask and shift values for the same.
*/
const QFPROM_REGION_INFO qfprom_region_data[] =
{
    {
        QFPROM_JTAG_ID_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR,
        HWIO_QFPROM_CORR_JTAG_ID_LSB_ADDR,
		HWIO_QFPROM_CORR_RD_PERM_LSB_JTAG_ID_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_JTAG_ID_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        2
    },
    
    {
        QFPROM_READ_PERMISSION_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        4
    },
    
	{
        QFPROM_WRITE_PERMISSION_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR,
        HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        6
    },
    {
        QFPROM_PTE_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_PTE_LSB_ADDR,
        HWIO_QFPROM_CORR_PTE_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        8
    },
    
    {
        QFPROM_AP_ANTI_ROLLBACK_REGION,
        2,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR ,
        HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR ,
        HWIO_QFPROM_CORR_RD_PERM_LSB_AP_ANTI_ROLLBACK_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_AP_ANTI_ROLLBACK_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        10
    },
    
    {
        QFPROM_MSA_ANTI_ROLLBACK_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR,
        HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_ADDR ,
        HWIO_QFPROM_CORR_RD_PERM_LSB_MSA_ANTI_ROLLBACK_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_MSA_ANTI_ROLLBACK_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        12
    },
    {
        QFPROM_SPARE7_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_SPARE_REG7_LSB_ADDR,
        HWIO_QFPROM_CORR_SPARE_REG7_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_SPARE7_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_SPARE7_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        14
    },
	{
        QFPROM_SPARE8_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_SPARE_REG8_LSB_ADDR,
        HWIO_QFPROM_CORR_SPARE_REG8_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_SPARE8_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_SPARE8_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        16
    },
	{
        QFPROM_SPARE9_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_SPARE_REG9_LSB_ADDR,
        HWIO_QFPROM_CORR_SPARE_REG9_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_SPARE9_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_SPARE9_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        18
    },
    {
        QFPROM_OEM_CONFIG_REGION,
        2,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        20
    },
    
    {
        QFPROM_FEATURE_CONFIG_REGION,
        4,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_BMSK ,
        QFPROM_ROW_LSB,
        TRUE,
        22
    },
    
    {
        QFPROM_DEBUG_KEY_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR,
        HWIO_QFPROM_CORR_DEBUG_KEY_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_LSB_DEBUG_KEY_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_LSB_DEBUG_KEY_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        26
    },
      
    {
        QFPROM_FEC_EN_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,
        HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_MSB_FEC_EN_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_FEC_EN_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        32
    },
    
    {
        QFPROM_INTAGLIO_GLUEBIT_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR,
        HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_MSB_INTAGLIO_GLUEBIT_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_INTAGLIO_GLUEBIT_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        34
    },
    
    {
        QFPROM_CUST_KEY_REGION,
        3,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(0),
        HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_ADDR(0),
        HWIO_QFPROM_CORR_RD_PERM_MSB_CUST_KEY_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_CUST_KEY_BMSK,
        QFPROM_ROW_LSB,
        FALSE,
        36
    },
    
    {
        QFPROM_SERIAL_NUM_REGION,
        1,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR,
        HWIO_QFPROM_CORR_SERIAL_NUM_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_MSB_SERIAL_NUM_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_SERIAL_NUM_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        38
    },
    
    {
        QFPROM_SPARE20_REGION,
        2,
        QFPROM_FEC_NONE,
        HWIO_QFPROM_RAW_SPARE_REG20_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_SPARE_REG20_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_MSB_SPARE20_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_SPARE20_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        40
    },
    {
        QFPROM_SEC_KEY_DERIVATION_KEY_REGION,
        5,
        QFPROM_FEC_63_56,
        HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(0) ,
        HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(0) ,
        HWIO_QFPROM_CORR_RD_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        46
    },    
    {
        QFPROM_OEM_PK_HASH_REGION,
        5,
        QFPROM_FEC_63_56,
        HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(0) ,
        HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_ADDR(0) ,
        HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_PK_HASH_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_PK_HASH_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        48
    },
    
    {
        QFPROM_OEM_SEC_BOOT_REGION,
        1,
        QFPROM_FEC_63_56,
        HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_MSB_OEM_SEC_BOOT_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_OEM_SEC_BOOT_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        52
    },
    
    {
        QFPROM_QC_SEC_BOOT_REGION,
        1,
        QFPROM_FEC_63_56,
        HWIO_QFPROM_RAW_QC_SEC_BOOT_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_QC_SEC_BOOT_ROW0_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_MSB_QC_SEC_BOOT_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_QC_SEC_BOOT_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        50
    },
    
    {
        QFPROM_USB_VID_PID_REGION,
        1,
        QFPROM_FEC_63_56,
        HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR,
        HWIO_QFPROM_CORR_USB_VID_PID_LSB_ADDR,
        HWIO_QFPROM_CORR_RD_PERM_MSB_USB_VID_PID_BMSK,
        HWIO_QFPROM_CORR_WR_PERM_MSB_USB_VID_PID_BMSK,
        QFPROM_ROW_LSB,
        TRUE,
        54
    },
        
    /* Add above this entry */
    {
        QFPROM_LAST_REGION_DUMMY,
        0,
        QFPROM_FEC_NONE,
        0,
        0,
        0,
        0,
        QFPROM_ROW_LSB
    }
};
