#ifndef __HOSTDL_HWIO_H__
#define __HOSTDL_HWIO_H__

/*=============================================================================
 *
 * FILE:      hostdl_hwio.h
 *
 * DESCRIPTION: HWIO Interface definitions for Hostdl.  This file contains 
 *              HWIO register definitions for the following modules:
 *              SECURITY_CONTROL_CORE
 *              MPM2_PSHOLD
 *              PERIPH_SS_QPIC_EBI2CR
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_hwio.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/30/14      sb      Add TLMM EBI2 EMMC select reg address
 * 05/03/13      eo      Add boot partition select reg address 
 * 04/01/13      eo      Add pmic support for warm reset 
 * 09/17/12      eo      Ehostdl/hostdl support for 9x25
 * 012/06/11     sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "msmhwiobase.h"
#include "msmhwio.h"

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/
 
#define SECURITY_CONTROL_CORE_REG_BASE             (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_PHYS        (SECURITY_CONTROL_BASE_PHYS + 0x00000000)

#define HWIO_BOOT_PARTITION_SELECT_ADDR            (SECURITY_CONTROL_CORE_REG_BASE + 0x000060f0)
#define HWIO_BOOT_PARTITION_SELECT_PHYS            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060f0)
#define HWIO_BOOT_PARTITION_SELECT_RMSK            0xffffffff
#define HWIO_BOOT_PARTITION_SELECT_IN          \
        in_dword_masked(HWIO_BOOT_PARTITION_SELECT_ADDR, HWIO_BOOT_PARTITION_SELECT_RMSK)
#define HWIO_BOOT_PARTITION_SELECT_INM(m)      \
        in_dword_masked(HWIO_BOOT_PARTITION_SELECT_ADDR, m)
#define HWIO_BOOT_PARTITION_SELECT_OUT(v)      \
        out_dword(HWIO_BOOT_PARTITION_SELECT_ADDR,v)
#define HWIO_BOOT_PARTITION_SELECT_OUTM(m,v)   \
        out_dword_masked_ns(HWIO_BOOT_PARTITION_SELECT_ADDR,m,v,HWIO_BOOT_PARTITION_SELECT_IN)
#define HWIO_BOOT_PARTITION_SELECT_PARTITION_BMSK  0xffffffff
#define HWIO_BOOT_PARTITION_SELECT_PARTITION_SHFT  0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                       (MPM2_MPM_BASE + 0x0000b000)

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                 (MPM2_PSHOLD_REG_BASE + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_RMSK                 0x1
#define HWIO_MPM2_MPM_PS_HOLD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, HWIO_MPM2_MPM_PS_HOLD_RMSK)
#define HWIO_MPM2_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM2_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM2_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM2_MPM_PS_HOLD_IN)
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK          0x1
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_SHFT          0x0

/*----------------------------------------------------------------------------
 * TLMM
 *--------------------------------------------------------------------------*/
#define TLMM_CSR_REG_BASE                                                                   (TLMM_BASE      + 0x00000000)
#define TLMM_CSR_REG_BASE_PHYS                                                              (TLMM_BASE_PHYS + 0x00000000)
#define TLMM_CSR_REG_BASE_OFFS                                                              0x00000000

#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_PHYS                                                   (TLMM_CSR_REG_BASE_PHYS + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_RMSK                                                          0x7
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_IN          \
        in_dword_masked(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR, HWIO_TLMM_EBI2_EMMC_GPIO_CFG_RMSK)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_INM(m)      \
        in_dword_masked(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR, m)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUT(v)      \
        out_dword(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,v)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,m,v,HWIO_TLMM_EBI2_EMMC_GPIO_CFG_IN)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EMMC_BOOT_SELECT_BMSK                                         0x4
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EMMC_BOOT_SELECT_SHFT                                         0x2
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BOOT_SELECT_BMSK                                         0x2
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BOOT_SELECT_SHFT                                         0x1
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_PBL_DEBUG_BMSK                                                0x1
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_PBL_DEBUG_SHFT                                                0x0

#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR                                                   (TLMM_CSR_REG_BASE      + 0x0010a000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OFFS                                                   (TLMM_CSR_REG_BASE_OFFS + 0x0010a000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK                                                       0x7fff
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_BMSK                                         0x6000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_SHFT                                            0xd
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_BMSK                                         0x1800
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_SHFT                                            0xb
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_BMSK                                         0x600
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_SHFT                                           0x9
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_BMSK                                          0x1c0
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_SHFT                                            0x6
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_BMSK                                           0x38
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_SHFT                                            0x3
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_BMSK                                           0x7
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_SHFT                                           0x0

/*------------------------------------------------------------------------------
 * MODULE: PERIPH_SS_QPIC_EBI2CR
 *------------------------------------------------------------------------------*/
 
#define EBI2CR_REG_BASE                                                       (QPIC_QPIC_BASE + 0x00000000)
#define EBI2CR_REG_BASE_PHYS                                                  (QPIC_QPIC_BASE_PHYS + 0x00000000)

#define HWIO_EBI2_CHIP_SELECT_CFG0_ADDR                                       (EBI2CR_REG_BASE      + 00000000)
#define HWIO_EBI2_CHIP_SELECT_CFG0_PHYS                                       (EBI2CR_REG_BASE_PHYS + 00000000)
#define HWIO_EBI2_CHIP_SELECT_CFG0_RMSK                                       0xffffffff
#define HWIO_EBI2_CHIP_SELECT_CFG0_IN                                         \
        in_dword_masked(HWIO_EBI2_CHIP_SELECT_CFG0_ADDR, HWIO_EBI2_CHIP_SELECT_CFG0_RMSK)
#define HWIO_EBI2_CHIP_SELECT_CFG0_INM(m)                                     \
        in_dword_masked(HWIO_EBI2_CHIP_SELECT_CFG0_ADDR, m)
#define HWIO_EBI2_CHIP_SELECT_CFG0_OUT(v)                                     \
        out_dword(HWIO_EBI2_CHIP_SELECT_CFG0_ADDR,v)
#define HWIO_EBI2_CHIP_SELECT_CFG0_OUTM(m,v)                                  \
        out_dword_masked_ns(HWIO_EBI2_CHIP_SELECT_CFG0_ADDR,m,v,HWIO_EBI2_CHIP_SELECT_CFG0_IN)
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS7_CFG_BMSK                                   0x1000
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS7_CFG_SHFT                                      0xc
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS7_CFG_DISABLE_FVAL                                0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS7_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS6_CFG_BMSK                                    0x800
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS6_CFG_SHFT                                      0xb
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS6_CFG_DISABLE_FVAL                                0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS6_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_ETM_CS_CFG_BMSK                                 0x400
#define HWIO_EBI2_CHIP_SELECT_CFG0_ETM_CS_CFG_SHFT                                   0xa
#define HWIO_EBI2_CHIP_SELECT_CFG0_ETM_CS_CFG_DISABLE_FVAL                             0
#define HWIO_EBI2_CHIP_SELECT_CFG0_ETM_CS_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS5_CFG_BMSK                                    0x300
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS5_CFG_SHFT                                      0x8
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS5_CFG_DISABLE_FVAL                                0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS5_CFG_LCD_DEVICE_CONNECTED_FVAL                 0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS5_CFG_LCD_DEVICE_CHIP_ENABLE_FVAL               0x2
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS5_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x3
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS4_CFG_BMSK                                     0xc0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS4_CFG_SHFT                                      0x6
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS4_CFG_DISABLE_FVAL                                0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS4_CFG_LCD_DEVICE_CONNECTED_FVAL                 0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS4_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x3
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS3_CFG_BMSK                                     0x20
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS3_CFG_SHFT                                      0x5
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS3_CFG_DISABLE_FVAL                                0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS3_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS2_CFG_BMSK                                     0x10
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS2_CFG_SHFT                                      0x4
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS2_CFG_DISABLE_FVAL                                0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS2_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS1_CFG_BMSK                                      0xc
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS1_CFG_SHFT                                      0x2
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS1_CFG_DISABLE_FVAL                                0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS1_CFG_SERIAL_FLASH_DEVICE_FVAL                  0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS1_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x2
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS0_CFG_BMSK                                      0x3
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS0_CFG_SHFT                                        0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS0_CFG_DISABLE_FVAL                                0
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS0_CFG_SERIAL_FLASH_DEVICE_FVAL                  0x1
#define HWIO_EBI2_CHIP_SELECT_CFG0_CS0_CFG_GENERAL_SRAM_MEMORY_INTERFACE_FVAL        0x2

#define HWIO_EBI2_CFG_ADDR                                                    (EBI2CR_REG_BASE      + 0x00000004)
#define HWIO_EBI2_CFG_PHYS                                                    (EBI2CR_REG_BASE_PHYS + 0x00000004)
#define HWIO_EBI2_CFG_RMSK                                                    0xffffffff
#define HWIO_EBI2_CFG_IN                                                      \
        in_dword_masked(HWIO_EBI2_CFG_ADDR, HWIO_EBI2_CFG_RMSK)
#define HWIO_EBI2_CFG_INM(m)                                                  \
        in_dword_masked(HWIO_EBI2_CFG_ADDR, m)
#define HWIO_EBI2_CFG_OUT(v)                                                  \
        out_dword(HWIO_EBI2_CFG_ADDR,v)
#define HWIO_EBI2_CFG_OUTM(m,v)                                               \
        out_dword_masked_ns(HWIO_EBI2_CFG_ADDR,m,v,HWIO_EBI2_CFG_IN)
#define HWIO_EBI2_CFG_DISABLE_SECOND_NAND_CONTROLER_BMSK                       0x8000000
#define HWIO_EBI2_CFG_DISABLE_SECOND_NAND_CONTROLER_SHFT                            0x1b
#define HWIO_EBI2_CFG_DRIVE_OE_EN_BMSK                                         0x4000000
#define HWIO_EBI2_CFG_DRIVE_OE_EN_SHFT                                              0x1a
#define HWIO_EBI2_CFG_DRIVE_OE_EN_OUTPUT_ENABLE_ASSERTED_ONLY_DURING_WRITES_FVAL          0
#define HWIO_EBI2_CFG_DRIVE_OE_EN_OUTPUT_ENABLE_ASSERTED_ALL_THE_TIME_EXCEPT_DURING_READ_DATA_TRANSFER_FVAL        0x1
#define HWIO_EBI2_CFG_WAIT_N_PGM_DLY_ENA_BMSK                                   0x800000
#define HWIO_EBI2_CFG_WAIT_N_PGM_DLY_ENA_SHFT                                       0x17
#define HWIO_EBI2_CFG_WAIT_N_PGM_DLY_SEL_BMSK                                   0x7c0000
#define HWIO_EBI2_CFG_WAIT_N_PGM_DLY_SEL_SHFT                                       0x12
#define HWIO_EBI2_CFG_EBI2_XMEMC_RECOVERY_BMSK                                   0x30000
#define HWIO_EBI2_CFG_EBI2_XMEMC_RECOVERY_SHFT                                      0x10
#define HWIO_EBI2_CFG_FORCE_IE_HIGH_BMSK                                          0x8000
#define HWIO_EBI2_CFG_FORCE_IE_HIGH_SHFT                                             0xf
#define HWIO_EBI2_CFG_FBCLK_PAD_FB_ENA_BMSK                                       0x4000
#define HWIO_EBI2_CFG_FBCLK_PAD_FB_ENA_SHFT                                          0xe
#define HWIO_EBI2_CFG_FBCLK_PGM_DLY_ENA_BMSK                                      0x2000
#define HWIO_EBI2_CFG_FBCLK_PGM_DLY_ENA_SHFT                                         0xd
#define HWIO_EBI2_CFG_FBCLK_PGM_DLY_SEL_BMSK                                      0x1f00
#define HWIO_EBI2_CFG_FBCLK_PGM_DLY_SEL_SHFT                                         0x8
#define HWIO_EBI2_CFG_BUSY_01_SEL_BMSK                                              0x80
#define HWIO_EBI2_CFG_BUSY_01_SEL_SHFT                                               0x7
#define HWIO_EBI2_CFG_PGM_DLY_ENA_BMSK                                              0x20
#define HWIO_EBI2_CFG_PGM_DLY_ENA_SHFT                                               0x5
#define HWIO_EBI2_CFG_PGM_DLY_SEL_BMSK                                              0x1f
#define HWIO_EBI2_CFG_PGM_DLY_SEL_SHFT                                                 0

#define HWIO_EBI2_HW_INFO_ADDR                                                (EBI2CR_REG_BASE      + 0x00000008)
#define HWIO_EBI2_HW_INFO_PHYS                                                (EBI2CR_REG_BASE_PHYS + 0x00000008)
#define HWIO_EBI2_HW_INFO_RMSK                                                0xffffffff
#define HWIO_EBI2_HW_INFO_IN                                                  \
        in_dword_masked(HWIO_EBI2_HW_INFO_ADDR, HWIO_EBI2_HW_INFO_RMSK)
#define HWIO_EBI2_HW_INFO_INM(m)                                              \
        in_dword_masked(HWIO_EBI2_HW_INFO_ADDR, m)
#define HWIO_EBI2_HW_INFO_EBI2_CORE_ID_BMSK                                      0xf0000
#define HWIO_EBI2_HW_INFO_EBI2_CORE_ID_SHFT                                         0x10
#define HWIO_EBI2_HW_INFO_EBI2_CORE_ID_VITUS_FVAL                                      0
#define HWIO_EBI2_HW_INFO_EBI2_MAJOR_REV_BMSK                                     0xf000
#define HWIO_EBI2_HW_INFO_EBI2_MAJOR_REV_SHFT                                        0xc
#define HWIO_EBI2_HW_INFO_EBI2_MINOR_REV_BMSK                                      0xff0
#define HWIO_EBI2_HW_INFO_EBI2_MINOR_REV_SHFT                                        0x4
#define HWIO_EBI2_HW_INFO_EBI2_SUBMINOR_REV_BMSK                                     0xf
#define HWIO_EBI2_HW_INFO_EBI2_SUBMINOR_REV_SHFT                                       0

#define HWIO_EBI2_LCD_CFG0_ADDR                                               (EBI2CR_REG_BASE      + 0x00000020)
#define HWIO_EBI2_LCD_CFG0_PHYS                                               (EBI2CR_REG_BASE_PHYS + 0x00000020)
#define HWIO_EBI2_LCD_CFG0_RMSK                                               0xffffffff
#define HWIO_EBI2_LCD_CFG0_IN                                                 \
        in_dword_masked(HWIO_EBI2_LCD_CFG0_ADDR, HWIO_EBI2_LCD_CFG0_RMSK)
#define HWIO_EBI2_LCD_CFG0_INM(m)                                             \
        in_dword_masked(HWIO_EBI2_LCD_CFG0_ADDR, m)
#define HWIO_EBI2_LCD_CFG0_OUT(v)                                             \
        out_dword(HWIO_EBI2_LCD_CFG0_ADDR,v)
#define HWIO_EBI2_LCD_CFG0_OUTM(m,v)                                          \
        out_dword_masked_ns(HWIO_EBI2_LCD_CFG0_ADDR,m,v,HWIO_EBI2_LCD_CFG0_IN)
#define HWIO_EBI2_LCD_CFG0_ADDR_CS_SETUP_BMSK                                 0x3e000000
#define HWIO_EBI2_LCD_CFG0_ADDR_CS_SETUP_SHFT                                       0x19
#define HWIO_EBI2_LCD_CFG0_WR_ACTIVE_BMSK                                      0x1f00000
#define HWIO_EBI2_LCD_CFG0_WR_ACTIVE_SHFT                                           0x14
#define HWIO_EBI2_LCD_CFG0_WR_CS_HOLD_BMSK                                       0xf8000
#define HWIO_EBI2_LCD_CFG0_WR_CS_HOLD_SHFT                                           0xf
#define HWIO_EBI2_LCD_CFG0_CS_WR_RD_SETUP_BMSK                                    0x7c00
#define HWIO_EBI2_LCD_CFG0_CS_WR_RD_SETUP_SHFT                                       0xa
#define HWIO_EBI2_LCD_CFG0_RD_ACTIVE_BMSK                                          0x3e0
#define HWIO_EBI2_LCD_CFG0_RD_ACTIVE_SHFT                                            0x5
#define HWIO_EBI2_LCD_CFG0_RD_CS_HOLD_BMSK                                          0x1f
#define HWIO_EBI2_LCD_CFG0_RD_CS_HOLD_SHFT                                             0

#define HWIO_EBI2_LCD_CFG1_ADDR                                               (EBI2CR_REG_BASE      + 0x00000024)
#define HWIO_EBI2_LCD_CFG1_PHYS                                               (EBI2CR_REG_BASE_PHYS + 0x00000024)
#define HWIO_EBI2_LCD_CFG1_RMSK                                               0xffffffff
#define HWIO_EBI2_LCD_CFG1_IN                                                 \
        in_dword_masked(HWIO_EBI2_LCD_CFG1_ADDR, HWIO_EBI2_LCD_CFG1_RMSK)
#define HWIO_EBI2_LCD_CFG1_INM(m)                                             \
        in_dword_masked(HWIO_EBI2_LCD_CFG1_ADDR, m)
#define HWIO_EBI2_LCD_CFG1_OUT(v)                                             \
        out_dword(HWIO_EBI2_LCD_CFG1_ADDR,v)
#define HWIO_EBI2_LCD_CFG1_OUTM(m,v)                                          \
        out_dword_masked_ns(HWIO_EBI2_LCD_CFG1_ADDR,m,v,HWIO_EBI2_LCD_CFG1_IN)
#define HWIO_EBI2_LCD_CFG1_LCD_RECOV_CYCLES_BMSK                              0xf8000000
#define HWIO_EBI2_LCD_CFG1_LCD_RECOV_CYCLES_SHFT                                    0x1b
#define HWIO_EBI2_LCD_CFG1_LCD_16BIT_ADDR_MODE_BMSK                            0x6000000
#define HWIO_EBI2_LCD_CFG1_LCD_16BIT_ADDR_MODE_SHFT                                 0x19
#define HWIO_EBI2_LCD_CFG1_LCD_16BIT_ADDR_MODE_DO_NOT_CHANGE_THE_ADDRESS_FVAL          0
#define HWIO_EBI2_LCD_CFG1_LCD_16BIT_ADDR_MODE_INCREMENT_ADDRESS_BY_1_FVAL           0x1
#define HWIO_EBI2_LCD_CFG1_LCD_16BIT_ADDR_MODE_DECREMENT_ADDRESS_BY_1_FVAL           0x2
#define HWIO_EBI2_LCD_CFG1_LCD_IF_MODE0_BMSK                                   0x1000000
#define HWIO_EBI2_LCD_CFG1_LCD_IF_MODE0_SHFT                                        0x18
#define HWIO_EBI2_LCD_CFG1_LCD_IF_MODE0_WIDTH1_FVAL                                    0
#define HWIO_EBI2_LCD_CFG1_LCD_IF_MODE0_WIDTH2_FVAL                                  0x1
#define HWIO_EBI2_LCD_CFG1_LCD_ENBL_POLARITY_BMSK                               0x800000
#define HWIO_EBI2_LCD_CFG1_LCD_ENBL_POLARITY_SHFT                                   0x17
#define HWIO_EBI2_LCD_CFG1_LCD_ENBL_POLARITY_ACTIVE_HIGH_FVAL                        0x1
#define HWIO_EBI2_LCD_CFG1_LCD_ENBL_POLARITY_ACTIVE_LOW_FVAL                           0
#define HWIO_EBI2_LCD_CFG1_LCD_IF_ENABLE_BMSK                                   0x400000
#define HWIO_EBI2_LCD_CFG1_LCD_IF_ENABLE_SHFT                                       0x16
#define HWIO_EBI2_LCD_CFG1_LCD_IF_ENABLE_EBI2_CS5_ACTS_AS_ENABLE_FVAL                0x1
#define HWIO_EBI2_LCD_CFG1_LCD_IF_ENABLE_EBI2_CS5_IS_CHIP_SELECT_FVAL                  0
#define HWIO_EBI2_LCD_CFG1_RDENBL_ACTIVE_BMSK                                   0x3f0000
#define HWIO_EBI2_LCD_CFG1_RDENBL_ACTIVE_SHFT                                       0x10
#define HWIO_EBI2_LCD_CFG1_CS_RDENBL_SETUP_BMSK                                   0xf800
#define HWIO_EBI2_LCD_CFG1_CS_RDENBL_SETUP_SHFT                                      0xb
#define HWIO_EBI2_LCD_CFG1_LCD_IF_MODE1_BMSK                                       0x400
#define HWIO_EBI2_LCD_CFG1_LCD_IF_MODE1_SHFT                                         0xa
#define HWIO_EBI2_LCD_CFG1_LCD_IF_MODE1_MEMORY_MAP_FVAL                                0
#define HWIO_EBI2_LCD_CFG1_LCD_IF_MODE1_PORT_MAP_FVAL                                0x1
#define HWIO_EBI2_LCD_CFG1_WRENBL_ACTIVE_BMSK                                      0x3e0
#define HWIO_EBI2_LCD_CFG1_WRENBL_ACTIVE_SHFT                                        0x5
#define HWIO_EBI2_LCD_CFG1_CS_WRENBL_SETUP_BMSK                                     0x1f
#define HWIO_EBI2_LCD_CFG1_CS_WRENBL_SETUP_SHFT                                        0

#define HWIO_EBI2_ARBITER_CFG_ADDR                                            (EBI2CR_REG_BASE      + 0x00000030)
#define HWIO_EBI2_ARBITER_CFG_PHYS                                            (EBI2CR_REG_BASE_PHYS + 0x00000030)
#define HWIO_EBI2_ARBITER_CFG_RMSK                                            0xffffffff
#define HWIO_EBI2_ARBITER_CFG_IN                                              \
        in_dword_masked(HWIO_EBI2_ARBITER_CFG_ADDR, HWIO_EBI2_ARBITER_CFG_RMSK)
#define HWIO_EBI2_ARBITER_CFG_INM(m)                                          \
        in_dword_masked(HWIO_EBI2_ARBITER_CFG_ADDR, m)
#define HWIO_EBI2_ARBITER_CFG_OUT(v)                                          \
        out_dword(HWIO_EBI2_ARBITER_CFG_ADDR,v)
#define HWIO_EBI2_ARBITER_CFG_OUTM(m,v)                                       \
        out_dword_masked_ns(HWIO_EBI2_ARBITER_CFG_ADDR,m,v,HWIO_EBI2_ARBITER_CFG_IN)
#define HWIO_EBI2_ARBITER_CFG_NANDC_CFG_PRIORITY_BMSK                               0xc0
#define HWIO_EBI2_ARBITER_CFG_NANDC_CFG_PRIORITY_SHFT                                0x6
#define HWIO_EBI2_ARBITER_CFG_NANDC_CFG_PRIORITY_LOWEST_PRIORITY_FVAL                  0
#define HWIO_EBI2_ARBITER_CFG_NANDC_CFG_PRIORITY_HIGHEST_PRIORITY_FVAL               0x1
#define HWIO_EBI2_ARBITER_CFG_NANDC_CFG_PRIORITY_SECOND_HIGHEST_PRIORITY_FVAL        0x2
#define HWIO_EBI2_ARBITER_CFG_NANDC_CFG_PRIORITY_THIRD_HIGHEST_PRIORITY_FVAL         0x3
#define HWIO_EBI2_ARBITER_CFG_XMEMC_PRIORITY_BMSK                                   0x30
#define HWIO_EBI2_ARBITER_CFG_XMEMC_PRIORITY_SHFT                                    0x4
#define HWIO_EBI2_ARBITER_CFG_XMEMC_PRIORITY_LOWEST_PRIORITY_FVAL                      0
#define HWIO_EBI2_ARBITER_CFG_XMEMC_PRIORITY_HIGHEST_PRIORITY_FVAL                   0x1
#define HWIO_EBI2_ARBITER_CFG_XMEMC_PRIORITY_SECOND_HIGHEST_PRIORITY_FVAL            0x2
#define HWIO_EBI2_ARBITER_CFG_XMEMC_PRIORITY_THIRD_HIGHEST_PRIORITY_FVAL             0x3
#define HWIO_EBI2_ARBITER_CFG_NANDC_PRIORITY_BMSK                                    0xc
#define HWIO_EBI2_ARBITER_CFG_NANDC_PRIORITY_SHFT                                    0x2
#define HWIO_EBI2_ARBITER_CFG_NANDC_PRIORITY_LOWEST_PRIORITY_FVAL                      0
#define HWIO_EBI2_ARBITER_CFG_NANDC_PRIORITY_HIGHEST_PRIORITY_FVAL                   0x1
#define HWIO_EBI2_ARBITER_CFG_NANDC_PRIORITY_SECOND_HIGHEST_PRIORITY_FVAL            0x2
#define HWIO_EBI2_ARBITER_CFG_NANDC_PRIORITY_THIRD_HIGHEST_PRIORITY_FVAL             0x3
#define HWIO_EBI2_ARBITER_CFG_LCDC_PRIORITY_BMSK                                     0x3
#define HWIO_EBI2_ARBITER_CFG_LCDC_PRIORITY_SHFT                                       0
#define HWIO_EBI2_ARBITER_CFG_LCDC_PRIORITY_LOWEST_PRIORITY_FVAL                       0
#define HWIO_EBI2_ARBITER_CFG_LCDC_PRIORITY_HIGHEST_PRIORITY_FVAL                    0x1
#define HWIO_EBI2_ARBITER_CFG_LCDC_PRIORITY_SECOND_HIGHEST_PRIORITY_FVAL             0x2
#define HWIO_EBI2_ARBITER_CFG_LCDC_PRIORITY_THIRD_HIGHEST_PRIORITY_FVAL              0x3

#define HWIO_EBI2_DEBUG_SEL_ADDR                                              (EBI2CR_REG_BASE      + 0x00000050)
#define HWIO_EBI2_DEBUG_SEL_PHYS                                              (EBI2CR_REG_BASE_PHYS + 0x00000050)
#define HWIO_EBI2_DEBUG_SEL_RMSK                                              0xffffffff
#define HWIO_EBI2_DEBUG_SEL_IN                                                \
        in_dword_masked(HWIO_EBI2_DEBUG_SEL_ADDR, HWIO_EBI2_DEBUG_SEL_RMSK)
#define HWIO_EBI2_DEBUG_SEL_INM(m)                                            \
        in_dword_masked(HWIO_EBI2_DEBUG_SEL_ADDR, m)
#define HWIO_EBI2_DEBUG_SEL_OUT(v)                                            \
        out_dword(HWIO_EBI2_DEBUG_SEL_ADDR,v)
#define HWIO_EBI2_DEBUG_SEL_OUTM(m,v)                                         \
        out_dword_masked_ns(HWIO_EBI2_DEBUG_SEL_ADDR,m,v,HWIO_EBI2_DEBUG_SEL_IN)
#define HWIO_EBI2_DEBUG_SEL_DEBUG_BUS_EN_BMSK                                      0x200
#define HWIO_EBI2_DEBUG_SEL_DEBUG_BUS_EN_SHFT                                        0x9
#define HWIO_EBI2_DEBUG_SEL_DEBUG_BUS_EN_DEBUG_BUS_DISABLED_FVAL                       0
#define HWIO_EBI2_DEBUG_SEL_DEBUG_BUS_EN_DEBUG_BUS_ENABLED_FVAL                      0x1
#define HWIO_EBI2_DEBUG_SEL_DEBUG_BUS_UPPER_SEL_BMSK                               0x100
#define HWIO_EBI2_DEBUG_SEL_DEBUG_BUS_UPPER_SEL_SHFT                                 0x8
#define HWIO_EBI2_DEBUG_SEL_DEBUG_BUS_UPPER_SEL_SELECT_EBI2_DEBUG_BUS_LOWER_32_BIT_31_0_FVAL          0
#define HWIO_EBI2_DEBUG_SEL_DEBUG_BUS_UPPER_SEL_SELECT_EBI2_DEBUG_BUS_UPPER_32_BIT_63_32_FVAL        0x1
#define HWIO_EBI2_DEBUG_SEL_MISR_SEL_BMSK                                           0xf0
#define HWIO_EBI2_DEBUG_SEL_MISR_SEL_SHFT                                            0x4
#define HWIO_EBI2_DEBUG_SEL_DEBUG_SEL_BMSK                                           0xf
#define HWIO_EBI2_DEBUG_SEL_DEBUG_SEL_SHFT                                             0

#define HWIO_EBI2_CRC_CFG_ADDR                                                (EBI2CR_REG_BASE      + 0x00000054)
#define HWIO_EBI2_CRC_CFG_PHYS                                                (EBI2CR_REG_BASE_PHYS + 0x00000054)
#define HWIO_EBI2_CRC_CFG_RMSK                                                0xffffffff
#define HWIO_EBI2_CRC_CFG_IN                                                  \
        in_dword_masked(HWIO_EBI2_CRC_CFG_ADDR, HWIO_EBI2_CRC_CFG_RMSK)
#define HWIO_EBI2_CRC_CFG_INM(m)                                              \
        in_dword_masked(HWIO_EBI2_CRC_CFG_ADDR, m)
#define HWIO_EBI2_CRC_CFG_OUT(v)                                              \
        out_dword(HWIO_EBI2_CRC_CFG_ADDR,v)
#define HWIO_EBI2_CRC_CFG_OUTM(m,v)                                           \
        out_dword_masked_ns(HWIO_EBI2_CRC_CFG_ADDR,m,v,HWIO_EBI2_CRC_CFG_IN)
#define HWIO_EBI2_CRC_CFG_EBI2_CRC_MODE_BMSK                                     0x20000
#define HWIO_EBI2_CRC_CFG_EBI2_CRC_MODE_SHFT                                        0x11
#define HWIO_EBI2_CRC_CFG_EBI2_LD_SEED_BMSK                                      0x10000
#define HWIO_EBI2_CRC_CFG_EBI2_LD_SEED_SHFT                                         0x10
#define HWIO_EBI2_CRC_CFG_EBI2_RANDM_SEED_BMSK                                    0xffff
#define HWIO_EBI2_CRC_CFG_EBI2_RANDM_SEED_SHFT                                         0

#define HWIO_EBI2_CRC_REMINDER_CFG_ADDR                                       (EBI2CR_REG_BASE      + 0x00000058)
#define HWIO_EBI2_CRC_REMINDER_CFG_PHYS                                       (EBI2CR_REG_BASE_PHYS + 0x00000058)
#define HWIO_EBI2_CRC_REMINDER_CFG_RMSK                                       0xffffffff
#define HWIO_EBI2_CRC_REMINDER_CFG_IN                                         \
        in_dword_masked(HWIO_EBI2_CRC_REMINDER_CFG_ADDR, HWIO_EBI2_CRC_REMINDER_CFG_RMSK)
#define HWIO_EBI2_CRC_REMINDER_CFG_INM(m)                                     \
        in_dword_masked(HWIO_EBI2_CRC_REMINDER_CFG_ADDR, m)
#define HWIO_EBI2_CRC_REMINDER_CFG_EBI2_CRC_REMINDER_BMSK                         0xffff
#define HWIO_EBI2_CRC_REMINDER_CFG_EBI2_CRC_REMINDER_SHFT                              0

#define HWIO_EBI2_NAND_ADM_MUX_ADDR                                           (EBI2CR_REG_BASE      + 0x0000005c)
#define HWIO_EBI2_NAND_ADM_MUX_PHYS                                           (EBI2CR_REG_BASE_PHYS + 0x0000005c)
#define HWIO_EBI2_NAND_ADM_MUX_RMSK                                           0xffffffff
#define HWIO_EBI2_NAND_ADM_MUX_IN                                             \
        in_dword_masked(HWIO_EBI2_NAND_ADM_MUX_ADDR, HWIO_EBI2_NAND_ADM_MUX_RMSK)
#define HWIO_EBI2_NAND_ADM_MUX_INM(m)                                         \
        in_dword_masked(HWIO_EBI2_NAND_ADM_MUX_ADDR, m)
#define HWIO_EBI2_NAND_ADM_MUX_OUT(v)                                         \
        out_dword(HWIO_EBI2_NAND_ADM_MUX_ADDR,v)
#define HWIO_EBI2_NAND_ADM_MUX_OUTM(m,v)                                      \
        out_dword_masked_ns(HWIO_EBI2_NAND_ADM_MUX_ADDR,m,v,HWIO_EBI2_NAND_ADM_MUX_IN)
#define HWIO_EBI2_NAND_ADM_MUX_CH1_CMD_ACK_MASK_BMSK                               0x800
#define HWIO_EBI2_NAND_ADM_MUX_CH1_CMD_ACK_MASK_SHFT                                 0xb
#define HWIO_EBI2_NAND_ADM_MUX_CH1_DATA_ACK_MASK_BMSK                              0x400
#define HWIO_EBI2_NAND_ADM_MUX_CH1_DATA_ACK_MASK_SHFT                                0xa
#define HWIO_EBI2_NAND_ADM_MUX_CH1_CMD_REQ_MASK_BMSK                               0x200
#define HWIO_EBI2_NAND_ADM_MUX_CH1_CMD_REQ_MASK_SHFT                                 0x9
#define HWIO_EBI2_NAND_ADM_MUX_CH1_DATA_REQ_MASK_BMSK                              0x100
#define HWIO_EBI2_NAND_ADM_MUX_CH1_DATA_REQ_MASK_SHFT                                0x8
#define HWIO_EBI2_NAND_ADM_MUX_CH1_OP_DONE_IRQ_MASK_BMSK                            0x80
#define HWIO_EBI2_NAND_ADM_MUX_CH1_OP_DONE_IRQ_MASK_SHFT                             0x7
#define HWIO_EBI2_NAND_ADM_MUX_CH1_WR_ER_DONE_IRQ_MASK_BMSK                         0x40
#define HWIO_EBI2_NAND_ADM_MUX_CH1_WR_ER_DONE_IRQ_MASK_SHFT                          0x6
#define HWIO_EBI2_NAND_ADM_MUX_CH0_CMD_ACK_MASK_BMSK                                0x20
#define HWIO_EBI2_NAND_ADM_MUX_CH0_CMD_ACK_MASK_SHFT                                 0x5
#define HWIO_EBI2_NAND_ADM_MUX_CH0_DATA_ACK_MASK_BMSK                               0x10
#define HWIO_EBI2_NAND_ADM_MUX_CH0_DATA_ACK_MASK_SHFT                                0x4
#define HWIO_EBI2_NAND_ADM_MUX_CH0_CMD_REQ_MASK_BMSK                                 0x8
#define HWIO_EBI2_NAND_ADM_MUX_CH0_CMD_REQ_MASK_SHFT                                 0x3
#define HWIO_EBI2_NAND_ADM_MUX_CH0_DATA_REQ_MASK_BMSK                                0x4
#define HWIO_EBI2_NAND_ADM_MUX_CH0_DATA_REQ_MASK_SHFT                                0x2
#define HWIO_EBI2_NAND_ADM_MUX_CH0_OP_DONE_IRQ_MASK_BMSK                             0x2
#define HWIO_EBI2_NAND_ADM_MUX_CH0_OP_DONE_IRQ_MASK_SHFT                             0x1
#define HWIO_EBI2_NAND_ADM_MUX_CH0_WR_ER_DONE_IRQ_MASK_BMSK                          0x1
#define HWIO_EBI2_NAND_ADM_MUX_CH0_WR_ER_DONE_IRQ_MASK_SHFT                            0

#define HWIO_EBI2_MUTEX_ADDR_OFFSET_ADDR                                      (EBI2CR_REG_BASE      + 0x00000060)
#define HWIO_EBI2_MUTEX_ADDR_OFFSET_PHYS                                      (EBI2CR_REG_BASE_PHYS + 0x00000060)
#define HWIO_EBI2_MUTEX_ADDR_OFFSET_RMSK                                         0xffc00
#define HWIO_EBI2_MUTEX_ADDR_OFFSET_IN                                        \
        in_dword_masked(HWIO_EBI2_MUTEX_ADDR_OFFSET_ADDR, HWIO_EBI2_MUTEX_ADDR_OFFSET_RMSK)
#define HWIO_EBI2_MUTEX_ADDR_OFFSET_INM(m)                                    \
        in_dword_masked(HWIO_EBI2_MUTEX_ADDR_OFFSET_ADDR, m)
#define HWIO_EBI2_MUTEX_ADDR_OFFSET_OUT(v)                                    \
        out_dword(HWIO_EBI2_MUTEX_ADDR_OFFSET_ADDR,v)
#define HWIO_EBI2_MUTEX_ADDR_OFFSET_OUTM(m,v)                                 \
        out_dword_masked_ns(HWIO_EBI2_MUTEX_ADDR_OFFSET_ADDR,m,v,HWIO_EBI2_MUTEX_ADDR_OFFSET_IN)
#define HWIO_EBI2_MUTEX_ADDR_OFFSET_MUTEX_ADDR_OFFSET_BMSK                       0xffc00
#define HWIO_EBI2_MUTEX_ADDR_OFFSET_MUTEX_ADDR_OFFSET_SHFT                           0xa

#define HWIO_EBI2_MISR_VALUE_ADDR                                             (EBI2CR_REG_BASE      + 0x00000064)
#define HWIO_EBI2_MISR_VALUE_PHYS                                             (EBI2CR_REG_BASE_PHYS + 0x00000064)
#define HWIO_EBI2_MISR_VALUE_RMSK                                             0xffffffff
#define HWIO_EBI2_MISR_VALUE_IN                                               \
        in_dword_masked(HWIO_EBI2_MISR_VALUE_ADDR, HWIO_EBI2_MISR_VALUE_RMSK)
#define HWIO_EBI2_MISR_VALUE_INM(m)                                           \
        in_dword_masked(HWIO_EBI2_MISR_VALUE_ADDR, m)
#define HWIO_EBI2_MISR_VALUE_MISR_VAL_BMSK                                    0xffffffff
#define HWIO_EBI2_MISR_VALUE_MISR_VAL_SHFT                                             0

#define HWIO_EBI2_CLKON_CFG_ADDR                                              (EBI2CR_REG_BASE      + 0x00000068)
#define HWIO_EBI2_CLKON_CFG_PHYS                                              (EBI2CR_REG_BASE_PHYS + 0x00000068)
#define HWIO_EBI2_CLKON_CFG_RMSK                                              0xffffffff
#define HWIO_EBI2_CLKON_CFG_IN                                                \
        in_dword_masked(HWIO_EBI2_CLKON_CFG_ADDR, HWIO_EBI2_CLKON_CFG_RMSK)
#define HWIO_EBI2_CLKON_CFG_INM(m)                                            \
        in_dword_masked(HWIO_EBI2_CLKON_CFG_ADDR, m)
#define HWIO_EBI2_CLKON_CFG_OUT(v)                                            \
        out_dword(HWIO_EBI2_CLKON_CFG_ADDR,v)
#define HWIO_EBI2_CLKON_CFG_OUTM(m,v)                                         \
        out_dword_masked_ns(HWIO_EBI2_CLKON_CFG_ADDR,m,v,HWIO_EBI2_CLKON_CFG_IN)
#define HWIO_EBI2_CLKON_CFG_GATE_ENA_BMSK                                     0x80000000
#define HWIO_EBI2_CLKON_CFG_GATE_ENA_SHFT                                           0x1f
#define HWIO_EBI2_CLKON_CFG_GATED_CLK_SEL_BMSK                                0x40000000
#define HWIO_EBI2_CLKON_CFG_GATED_CLK_SEL_SHFT                                      0x1e
#define HWIO_EBI2_CLKON_CFG_PIPE_AHB_ACC_BMSK                                 0x20000000
#define HWIO_EBI2_CLKON_CFG_PIPE_AHB_ACC_SHFT                                       0x1d
#define HWIO_EBI2_CLKON_CFG_IGN_AHB_HRDYIN_BMSK                               0x10000000
#define HWIO_EBI2_CLKON_CFG_IGN_AHB_HRDYIN_SHFT                                     0x1c
#define HWIO_EBI2_CLKON_CFG_UNUSED_27_24_BMSK                                  0xf000000
#define HWIO_EBI2_CLKON_CFG_UNUSED_27_24_SHFT                                       0x18
#define HWIO_EBI2_CLKON_CFG_CD_IVAL_BMSK                                        0xff0000
#define HWIO_EBI2_CLKON_CFG_CD_IVAL_SHFT                                            0x10
#define HWIO_EBI2_CLKON_CFG_CH_IVAL_BMSK                                          0xffff
#define HWIO_EBI2_CLKON_CFG_CH_IVAL_SHFT                                               0

#define HWIO_EBI2_CORE_CLKON_CFG_ADDR                                         (EBI2CR_REG_BASE      + 0x0000006c)
#define HWIO_EBI2_CORE_CLKON_CFG_PHYS                                         (EBI2CR_REG_BASE_PHYS + 0x0000006c)
#define HWIO_EBI2_CORE_CLKON_CFG_RMSK                                         0xffffffff
#define HWIO_EBI2_CORE_CLKON_CFG_IN                                           \
        in_dword_masked(HWIO_EBI2_CORE_CLKON_CFG_ADDR, HWIO_EBI2_CORE_CLKON_CFG_RMSK)
#define HWIO_EBI2_CORE_CLKON_CFG_INM(m)                                       \
        in_dword_masked(HWIO_EBI2_CORE_CLKON_CFG_ADDR, m)
#define HWIO_EBI2_CORE_CLKON_CFG_OUT(v)                                       \
        out_dword(HWIO_EBI2_CORE_CLKON_CFG_ADDR,v)
#define HWIO_EBI2_CORE_CLKON_CFG_OUTM(m,v)                                    \
        out_dword_masked_ns(HWIO_EBI2_CORE_CLKON_CFG_ADDR,m,v,HWIO_EBI2_CORE_CLKON_CFG_IN)
#define HWIO_EBI2_CORE_CLKON_CFG_GATE_NAND_ENA_BMSK                           0x80000000
#define HWIO_EBI2_CORE_CLKON_CFG_GATE_NAND_ENA_SHFT                                 0x1f
#define HWIO_EBI2_CORE_CLKON_CFG_GATE_XMEM_ENA_BMSK                           0x40000000
#define HWIO_EBI2_CORE_CLKON_CFG_GATE_XMEM_ENA_SHFT                                 0x1e
#define HWIO_EBI2_CORE_CLKON_CFG_GATE_LCD_ENA_BMSK                            0x20000000
#define HWIO_EBI2_CORE_CLKON_CFG_GATE_LCD_ENA_SHFT                                  0x1d
#define HWIO_EBI2_CORE_CLKON_CFG_UNUSED_28_12_BMSK                            0x1ffff000
#define HWIO_EBI2_CORE_CLKON_CFG_UNUSED_28_12_SHFT                                   0xc
#define HWIO_EBI2_CORE_CLKON_CFG_NAND_CH_IVAL_BMSK                                 0xf00
#define HWIO_EBI2_CORE_CLKON_CFG_NAND_CH_IVAL_SHFT                                   0x8
#define HWIO_EBI2_CORE_CLKON_CFG_XMEM_CH_IVAL_BMSK                                  0xf0
#define HWIO_EBI2_CORE_CLKON_CFG_XMEM_CH_IVAL_SHFT                                   0x4
#define HWIO_EBI2_CORE_CLKON_CFG_LCD_CH_IVAL_BMSK                                    0xf
#define HWIO_EBI2_CORE_CLKON_CFG_LCD_CH_IVAL_SHFT                                      0

//newly added
#define CLK_CTL_REG_BASE                                                         (CLK_CTL_BASE      + 0x00000000)
#define CLK_CTL_REG_BASE_PHYS                                                    (CLK_CTL_BASE_PHYS + 0x00000000)


#define HWIO_SFAB_AHB_S3_FCLK_CTL_ADDR                                           (CLK_CTL_REG_BASE      + 0x0000216c)
#define HWIO_SFAB_AHB_S3_FCLK_CTL_PHYS                                           (CLK_CTL_REG_BASE_PHYS + 0x0000216c)
#define HWIO_SFAB_AHB_S3_FCLK_CTL_RMSK                                           0xffffffff
#define HWIO_SFAB_AHB_S3_FCLK_CTL_IN          \
        in_dword_masked(HWIO_SFAB_AHB_S3_FCLK_CTL_ADDR, HWIO_SFAB_AHB_S3_FCLK_CTL_RMSK)
#define HWIO_SFAB_AHB_S3_FCLK_CTL_INM(m)      \
        in_dword_masked(HWIO_SFAB_AHB_S3_FCLK_CTL_ADDR, m)
#define HWIO_SFAB_AHB_S3_FCLK_CTL_OUT(v)      \
        out_dword(HWIO_SFAB_AHB_S3_FCLK_CTL_ADDR,v)
#define HWIO_SFAB_AHB_S3_FCLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SFAB_AHB_S3_FCLK_CTL_ADDR,m,v,HWIO_SFAB_AHB_S3_FCLK_CTL_IN)
#define HWIO_SFAB_AHB_S3_FCLK_CTL_FABRIC_CLK_GATE_ENA_BMSK                             0x40
#define HWIO_SFAB_AHB_S3_FCLK_CTL_FABRIC_CLK_GATE_ENA_SHFT                              0x6
#define HWIO_SFAB_AHB_S3_FCLK_CTL_CLK_INV_BMSK                                         0x20
#define HWIO_SFAB_AHB_S3_FCLK_CTL_CLK_INV_SHFT                                          0x5
#define HWIO_SFAB_AHB_S3_FCLK_CTL_CLK_BRANCH_ENA_BMSK                                  0x10
#define HWIO_SFAB_AHB_S3_FCLK_CTL_CLK_BRANCH_ENA_SHFT                                   0x4


#define HWIO_CFPB_CLK_NS_ADDR                                                    (CLK_CTL_REG_BASE      + 0x0000264c)
#define HWIO_CFPB_CLK_NS_PHYS                                                    (CLK_CTL_REG_BASE_PHYS + 0x0000264c)
#define HWIO_CFPB_CLK_NS_RMSK                                                    0xffffffff
#define HWIO_CFPB_CLK_NS_IN          \
        in_dword_masked(HWIO_CFPB_CLK_NS_ADDR, HWIO_CFPB_CLK_NS_RMSK)
#define HWIO_CFPB_CLK_NS_INM(m)      \
        in_dword_masked(HWIO_CFPB_CLK_NS_ADDR, m)
#define HWIO_CFPB_CLK_NS_OUT(v)      \
        out_dword(HWIO_CFPB_CLK_NS_ADDR,v)
#define HWIO_CFPB_CLK_NS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CFPB_CLK_NS_ADDR,m,v,HWIO_CFPB_CLK_NS_IN)
#define HWIO_CFPB_CLK_NS_CLK_DIV_BMSK                                                   0x3
#define HWIO_CFPB_CLK_NS_CLK_DIV_SHFT                                                   0x0

#define HWIO_SFAB_CFPB_S_HCLK_CTL_ADDR                                           (CLK_CTL_REG_BASE      + 0x000026c0)
#define HWIO_SFAB_CFPB_S_HCLK_CTL_PHYS                                           (CLK_CTL_REG_BASE_PHYS + 0x000026c0)
#define HWIO_SFAB_CFPB_S_HCLK_CTL_RMSK                                           0xffffffff
#define HWIO_SFAB_CFPB_S_HCLK_CTL_IN          \
        in_dword_masked(HWIO_SFAB_CFPB_S_HCLK_CTL_ADDR, HWIO_SFAB_CFPB_S_HCLK_CTL_RMSK)
#define HWIO_SFAB_CFPB_S_HCLK_CTL_INM(m)      \
        in_dword_masked(HWIO_SFAB_CFPB_S_HCLK_CTL_ADDR, m)
#define HWIO_SFAB_CFPB_S_HCLK_CTL_OUT(v)      \
        out_dword(HWIO_SFAB_CFPB_S_HCLK_CTL_ADDR,v)
#define HWIO_SFAB_CFPB_S_HCLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SFAB_CFPB_S_HCLK_CTL_ADDR,m,v,HWIO_SFAB_CFPB_S_HCLK_CTL_IN)
#define HWIO_SFAB_CFPB_S_HCLK_CTL_ASYNC_RESET_BMSK                                     0x80
#define HWIO_SFAB_CFPB_S_HCLK_CTL_ASYNC_RESET_SHFT                                      0x7
#define HWIO_SFAB_CFPB_S_HCLK_CTL_FABRIC_CLK_GATE_ENA_BMSK                             0x40
#define HWIO_SFAB_CFPB_S_HCLK_CTL_FABRIC_CLK_GATE_ENA_SHFT                              0x6
#define HWIO_SFAB_CFPB_S_HCLK_CTL_CLK_INV_BMSK                                         0x20
#define HWIO_SFAB_CFPB_S_HCLK_CTL_CLK_INV_SHFT                                          0x5
#define HWIO_SFAB_CFPB_S_HCLK_CTL_CLK_BRANCH_ENA_BMSK                                  0x10
#define HWIO_SFAB_CFPB_S_HCLK_CTL_CLK_BRANCH_ENA_SHFT                                   0x4

#define HWIO_CFPB_SPLITTER_HCLK_CTL_ADDR                                         (CLK_CTL_REG_BASE      + 0x000026e0)
#define HWIO_CFPB_SPLITTER_HCLK_CTL_PHYS                                         (CLK_CTL_REG_BASE_PHYS + 0x000026e0)
#define HWIO_CFPB_SPLITTER_HCLK_CTL_RMSK                                         0xffffffff
#define HWIO_CFPB_SPLITTER_HCLK_CTL_IN          \
        in_dword_masked(HWIO_CFPB_SPLITTER_HCLK_CTL_ADDR, HWIO_CFPB_SPLITTER_HCLK_CTL_RMSK)
#define HWIO_CFPB_SPLITTER_HCLK_CTL_INM(m)      \
        in_dword_masked(HWIO_CFPB_SPLITTER_HCLK_CTL_ADDR, m)
#define HWIO_CFPB_SPLITTER_HCLK_CTL_OUT(v)      \
        out_dword(HWIO_CFPB_SPLITTER_HCLK_CTL_ADDR,v)
#define HWIO_CFPB_SPLITTER_HCLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CFPB_SPLITTER_HCLK_CTL_ADDR,m,v,HWIO_CFPB_SPLITTER_HCLK_CTL_IN)
#define HWIO_CFPB_SPLITTER_HCLK_CTL_FABRIC_CLK_GATE_ENA_BMSK                           0x40
#define HWIO_CFPB_SPLITTER_HCLK_CTL_FABRIC_CLK_GATE_ENA_SHFT                            0x6
#define HWIO_CFPB_SPLITTER_HCLK_CTL_CLK_INV_BMSK                                       0x20
#define HWIO_CFPB_SPLITTER_HCLK_CTL_CLK_INV_SHFT                                        0x5
#define HWIO_CFPB_SPLITTER_HCLK_CTL_CLK_BRANCH_ENA_BMSK                                0x10
#define HWIO_CFPB_SPLITTER_HCLK_CTL_CLK_BRANCH_ENA_SHFT                                 0x4

#define HWIO_CFPB0_HCLK_CTL_ADDR                                                 (CLK_CTL_REG_BASE      + 0x00002650)
#define HWIO_CFPB0_HCLK_CTL_PHYS                                                 (CLK_CTL_REG_BASE_PHYS + 0x00002650)
#define HWIO_CFPB0_HCLK_CTL_RMSK                                                 0xffffffff
#define HWIO_CFPB0_HCLK_CTL_IN          \
        in_dword_masked(HWIO_CFPB0_HCLK_CTL_ADDR, HWIO_CFPB0_HCLK_CTL_RMSK)
#define HWIO_CFPB0_HCLK_CTL_INM(m)      \
        in_dword_masked(HWIO_CFPB0_HCLK_CTL_ADDR, m)
#define HWIO_CFPB0_HCLK_CTL_OUT(v)      \
        out_dword(HWIO_CFPB0_HCLK_CTL_ADDR,v)
#define HWIO_CFPB0_HCLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CFPB0_HCLK_CTL_ADDR,m,v,HWIO_CFPB0_HCLK_CTL_IN)
#define HWIO_CFPB0_HCLK_CTL_BUS_CLK_GATE_ENA_BMSK                                      0x40
#define HWIO_CFPB0_HCLK_CTL_BUS_CLK_GATE_ENA_SHFT                                       0x6
#define HWIO_CFPB0_HCLK_CTL_CLK_INV_BMSK                                               0x20
#define HWIO_CFPB0_HCLK_CTL_CLK_INV_SHFT                                                0x5
#define HWIO_CFPB0_HCLK_CTL_CLK_BRANCH_ENA_BMSK                                        0x10
#define HWIO_CFPB0_HCLK_CTL_CLK_BRANCH_ENA_SHFT                                         0x4

#define HWIO_EBI2_CLK_CTL_ADDR                                                   (CLK_CTL_REG_BASE      + 0x00002664)
#define HWIO_EBI2_CLK_CTL_PHYS                                                   (CLK_CTL_REG_BASE_PHYS + 0x00002664)
#define HWIO_EBI2_CLK_CTL_RMSK                                                   0xffffffff
#define HWIO_EBI2_CLK_CTL_IN          \
        in_dword_masked(HWIO_EBI2_CLK_CTL_ADDR, HWIO_EBI2_CLK_CTL_RMSK)
#define HWIO_EBI2_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_EBI2_CLK_CTL_ADDR, m)
#define HWIO_EBI2_CLK_CTL_OUT(v)      \
        out_dword(HWIO_EBI2_CLK_CTL_ADDR,v)
#define HWIO_EBI2_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_EBI2_CLK_CTL_ADDR,m,v,HWIO_EBI2_CLK_CTL_IN)
#define HWIO_EBI2_CLK_CTL_ALWAYS_ON_CLK_INV_BMSK                                      0x200
#define HWIO_EBI2_CLK_CTL_ALWAYS_ON_CLK_INV_SHFT                                        0x9
#define HWIO_EBI2_CLK_CTL_ALWAYS_ON_CLK_BRANCH_ENA_BMSK                               0x100
#define HWIO_EBI2_CLK_CTL_ALWAYS_ON_CLK_BRANCH_ENA_SHFT                                 0x8
#define HWIO_EBI2_CLK_CTL_ASYNC_RESET_BMSK                                             0x80
#define HWIO_EBI2_CLK_CTL_ASYNC_RESET_SHFT                                              0x7
#define HWIO_EBI2_CLK_CTL_BUS_CLK_GATE_ENA_BMSK                                        0x40
#define HWIO_EBI2_CLK_CTL_BUS_CLK_GATE_ENA_SHFT                                         0x6
#define HWIO_EBI2_CLK_CTL_CLK_INV_BMSK                                                 0x20
#define HWIO_EBI2_CLK_CTL_CLK_INV_SHFT                                                  0x5
#define HWIO_EBI2_CLK_CTL_CLK_BRANCH_ENA_BMSK                                          0x10
#define HWIO_EBI2_CLK_CTL_CLK_BRANCH_ENA_SHFT                                           0x4

#endif /* __HOSTDL_HWIO_H__ */




