#ifndef __MSMHWIOBASE_H__
#define __MSMHWIOBASE_H__
/*
===========================================================================
*/
/**
  @file msmhwiobase.h
  @brief Auto-generated HWIO base include file.
*/
/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/hwio/msm8909/phys/msmhwiobase.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#ifdef TZBSP_WITH_TBASE
#define HWIO_SANDBOX_RELOCATE_OFFSET 0x04800000
#else
#define HWIO_SANDBOX_RELOCATE_OFFSET 0x0
#endif

/*----------------------------------------------------------------------------
 * BASE: LPASS_LPM_SIZE
 *--------------------------------------------------------------------------*/

#define LPASS_LPM_SIZE_BASE                                         (0x00001000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define LPASS_LPM_SIZE_BASE_SIZE                                    0x00000000
#define LPASS_LPM_SIZE_BASE_PHYS                                    0x00001000

/*----------------------------------------------------------------------------
 * BASE: PRONTO_CMEM_SIZE
 *--------------------------------------------------------------------------*/

#define PRONTO_CMEM_SIZE_BASE                                       (0x00008000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PRONTO_CMEM_SIZE_BASE_SIZE                                  0x00000000
#define PRONTO_CMEM_SIZE_BASE_PHYS                                  0x00008000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_SIZE
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_SIZE_BASE                                    (0x00008000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RPM_SS_MSG_RAM_SIZE_BASE_SIZE                               0x00000000
#define RPM_SS_MSG_RAM_SIZE_BASE_PHYS                               0x00008000

/*----------------------------------------------------------------------------
 * BASE: SYSTEM_IMEM_SIZE
 *--------------------------------------------------------------------------*/

#define SYSTEM_IMEM_SIZE_BASE                                       (0x00020000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define SYSTEM_IMEM_SIZE_BASE_SIZE                                  0x00000000
#define SYSTEM_IMEM_SIZE_BASE_PHYS                                  0x00020000

/*----------------------------------------------------------------------------
 * BASE: PRNG_PRNG
 *--------------------------------------------------------------------------*/

#define PRNG_PRNG_BASE                                              (0x00022000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PRNG_PRNG_BASE_SIZE                                         0x00000140
#define PRNG_PRNG_BASE_PHYS                                         0x00022000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_0_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_0_BUS_TIMEOUT_BASE                                    (0x00023000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PCNOC_0_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_0_BUS_TIMEOUT_BASE_PHYS                               0x00023000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_SIZE
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_SIZE_BASE                                          (0x00024000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define BOOT_ROM_SIZE_BASE_SIZE                                     0x00000000
#define BOOT_ROM_SIZE_BASE_PHYS                                     0x00024000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_1_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_1_BUS_TIMEOUT_BASE                                    (0x00024000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PCNOC_1_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_1_BUS_TIMEOUT_BASE_PHYS                               0x00024000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_2_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_2_BUS_TIMEOUT_BASE                                    (0x00025000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PCNOC_2_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_2_BUS_TIMEOUT_BASE_PHYS                               0x00025000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_3_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_3_BUS_TIMEOUT_BASE                                    (0x00026000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PCNOC_3_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_3_BUS_TIMEOUT_BASE_PHYS                               0x00026000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_4_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_4_BUS_TIMEOUT_BASE                                    (0x00027000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PCNOC_4_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_4_BUS_TIMEOUT_BASE_PHYS                               0x00027000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_5_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_5_BUS_TIMEOUT_BASE                                    (0x00028000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PCNOC_5_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_5_BUS_TIMEOUT_BASE_PHYS                               0x00028000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_6_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_6_BUS_TIMEOUT_BASE                                    (0x00029000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PCNOC_6_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_6_BUS_TIMEOUT_BASE_PHYS                               0x00029000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_7_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_7_BUS_TIMEOUT_BASE                                    (0x0002a000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PCNOC_7_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_7_BUS_TIMEOUT_BASE_PHYS                               0x0002a000

/*----------------------------------------------------------------------------
 * BASE: XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB
 *--------------------------------------------------------------------------*/

#define XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB_BASE                  (0x0002d000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB_BASE_SIZE             0x00001000
#define XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB_BASE_PHYS             0x0002d000

/*----------------------------------------------------------------------------
 * BASE: XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB
 *--------------------------------------------------------------------------*/

#define XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB_BASE                 (0x0002e000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB_BASE_SIZE            0x00001000
#define XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB_BASE_PHYS            0x0002e000

/*----------------------------------------------------------------------------
 * BASE: XPU_CFG_PRNG_CFG_APU1132_1
 *--------------------------------------------------------------------------*/

#define XPU_CFG_PRNG_CFG_APU1132_1_BASE                             (0x0002f000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define XPU_CFG_PRNG_CFG_APU1132_1_BASE_SIZE                        0x00001000
#define XPU_CFG_PRNG_CFG_APU1132_1_BASE_PHYS                        0x0002f000

/*----------------------------------------------------------------------------
 * BASE: XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB
 *--------------------------------------------------------------------------*/

#define XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB_BASE                   (0x00033000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB_BASE_SIZE              0x00001000
#define XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB_BASE_PHYS              0x00033000

/*----------------------------------------------------------------------------
 * BASE: SMMU_500_MPU_WRAPPER
 *--------------------------------------------------------------------------*/

#define SMMU_500_MPU_WRAPPER_BASE                                   (0x00034000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define SMMU_500_MPU_WRAPPER_BASE_SIZE                              0x00004000
#define SMMU_500_MPU_WRAPPER_BASE_PHYS                              0x00034000

/*----------------------------------------------------------------------------
 * BASE: SPDM_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define SPDM_WRAPPER_TOP_BASE                                       (0x00040000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define SPDM_WRAPPER_TOP_BASE_SIZE                                  0x00005000
#define SPDM_WRAPPER_TOP_BASE_PHYS                                  0x00040000

/*----------------------------------------------------------------------------
 * BASE: RBCPR_WRAPPER
 *--------------------------------------------------------------------------*/

#define RBCPR_WRAPPER_BASE                                          (0x00048000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RBCPR_WRAPPER_BASE_SIZE                                     0x00001000
#define RBCPR_WRAPPER_BASE_PHYS                                     0x00048000

/*----------------------------------------------------------------------------
 * BASE: OCIMEM_WRAPPER_CSR
 *--------------------------------------------------------------------------*/

#define OCIMEM_WRAPPER_CSR_BASE                                     (0x00050000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define OCIMEM_WRAPPER_CSR_BASE_SIZE                                0x00004000
#define OCIMEM_WRAPPER_CSR_BASE_PHYS                                0x00050000

/*----------------------------------------------------------------------------
 * BASE: SECURITY_CONTROL
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_BASE                                       (0x00058000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define SECURITY_CONTROL_BASE_SIZE                                  0x00008000
#define SECURITY_CONTROL_BASE_PHYS                                  0x00058000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_START_ADDRESS_BASE                           (0x00060000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE                      0x00060000
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS                      0x00060000

/*----------------------------------------------------------------------------
 * BASE: RPM_MSG_RAM
 *--------------------------------------------------------------------------*/

#define RPM_MSG_RAM_BASE                                            (0x00060000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RPM_MSG_RAM_BASE_SIZE                                       0x00005000
#define RPM_MSG_RAM_BASE_PHYS                                       0x00060000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_END_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_END_ADDRESS_BASE                             (0x00067fff + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RPM_SS_MSG_RAM_END_ADDRESS_BASE_SIZE                        0x00000000
#define RPM_SS_MSG_RAM_END_ADDRESS_BASE_PHYS                        0x00067fff

/*----------------------------------------------------------------------------
 * BASE: PDM_PERPH_WEB
 *--------------------------------------------------------------------------*/

#define PDM_PERPH_WEB_BASE                                          (0x00068000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PDM_PERPH_WEB_BASE_SIZE                                     0x00004000
#define PDM_PERPH_WEB_BASE_PHYS                                     0x00068000

/*----------------------------------------------------------------------------
 * BASE: USB2_PHY_CM_DWC_USB2_SW
 *--------------------------------------------------------------------------*/

#define USB2_PHY_CM_DWC_USB2_SW_BASE                                (0x0006c000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define USB2_PHY_CM_DWC_USB2_SW_BASE_SIZE                           0x00001000
#define USB2_PHY_CM_DWC_USB2_SW_BASE_PHYS                           0x0006c000

/*----------------------------------------------------------------------------
 * BASE: A7SS_L2_SIZE
 *--------------------------------------------------------------------------*/

#define A7SS_L2_SIZE_BASE                                           (0x00080000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define A7SS_L2_SIZE_BASE_SIZE                                      0x100000000
#define A7SS_L2_SIZE_BASE_PHYS                                      0x00080000

/*----------------------------------------------------------------------------
 * BASE: RPM_CODE_RAM_SIZE
 *--------------------------------------------------------------------------*/

#define RPM_CODE_RAM_SIZE_BASE                                      (0x00080000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RPM_CODE_RAM_SIZE_BASE_SIZE                                 0x00000000
#define RPM_CODE_RAM_SIZE_BASE_PHYS                                 0x00080000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_START_ADDRESS_BASE                                 (0x00100000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define BOOT_ROM_START_ADDRESS_BASE_SIZE                            0x00000000
#define BOOT_ROM_START_ADDRESS_BASE_PHYS                            0x00100000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_BASE                                               (0x00100000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define BOOT_ROM_BASE_SIZE                                          0x00100000
#define BOOT_ROM_BASE_PHYS                                          0x00100000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_END_ADDRESS
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_END_ADDRESS_BASE                                   (0x00123fff + HWIO_SANDBOX_RELOCATE_OFFSET)
#define BOOT_ROM_END_ADDRESS_BASE_SIZE                              0x00000000
#define BOOT_ROM_END_ADDRESS_BASE_PHYS                              0x00123fff

/*----------------------------------------------------------------------------
 * BASE: RPM_CODE_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_CODE_RAM_START_ADDRESS_BASE                             (0x00200000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RPM_CODE_RAM_START_ADDRESS_BASE_SIZE                        0x00000000
#define RPM_CODE_RAM_START_ADDRESS_BASE_PHYS                        0x00200000

/*----------------------------------------------------------------------------
 * BASE: RPM
 *--------------------------------------------------------------------------*/

#define RPM_BASE                                                    (0x00200000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RPM_BASE_SIZE                                               0x00089000
#define RPM_BASE_PHYS                                               0x00200000

/*----------------------------------------------------------------------------
 * BASE: RPM_CODE_RAM_END_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_CODE_RAM_END_ADDRESS_BASE                               (0x0027ffff + HWIO_SANDBOX_RELOCATE_OFFSET)
#define RPM_CODE_RAM_END_ADDRESS_BASE_SIZE                          0x00000000
#define RPM_CODE_RAM_END_ADDRESS_BASE_PHYS                          0x0027ffff

/*----------------------------------------------------------------------------
 * BASE: MSS_TCM_SIZE
 *--------------------------------------------------------------------------*/

#define MSS_TCM_SIZE_BASE                                           (0x00400000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define MSS_TCM_SIZE_BASE_SIZE                                      0x00000000
#define MSS_TCM_SIZE_BASE_PHYS                                      0x00400000

/*----------------------------------------------------------------------------
 * BASE: BIMC
 *--------------------------------------------------------------------------*/

#define BIMC_BASE                                                   (0x00400000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define BIMC_BASE_SIZE                                              0x00060000
#define BIMC_BASE_PHYS                                              0x00400000

/*----------------------------------------------------------------------------
 * BASE: EBI1_PHY_CFG
 *--------------------------------------------------------------------------*/

#define EBI1_PHY_CFG_BASE                                           (0x00480000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define EBI1_PHY_CFG_BASE_SIZE                                      0x00020000
#define EBI1_PHY_CFG_BASE_PHYS                                      0x00480000

/*----------------------------------------------------------------------------
 * BASE: MPM2_MPM
 *--------------------------------------------------------------------------*/

#define MPM2_MPM_BASE                                               (0x004a0000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define MPM2_MPM_BASE_SIZE                                          0x00010000
#define MPM2_MPM_BASE_PHYS                                          0x004a0000

/*----------------------------------------------------------------------------
 * BASE: DEHR_BIMC_WRAPPER
 *--------------------------------------------------------------------------*/

#define DEHR_BIMC_WRAPPER_BASE                                      (0x004b0000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define DEHR_BIMC_WRAPPER_BASE_SIZE                                 0x00004000
#define DEHR_BIMC_WRAPPER_BASE_PHYS                                 0x004b0000

/*----------------------------------------------------------------------------
 * BASE: PC_NOC
 *--------------------------------------------------------------------------*/

#define PC_NOC_BASE                                                 (0x00500000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define PC_NOC_BASE_SIZE                                            0x00011000
#define PC_NOC_BASE_PHYS                                            0x00500000

/*----------------------------------------------------------------------------
 * BASE: SYSTEM_NOC
 *--------------------------------------------------------------------------*/

#define SYSTEM_NOC_BASE                                             (0x00580000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define SYSTEM_NOC_BASE_SIZE                                        0x00013000
#define SYSTEM_NOC_BASE_PHYS                                        0x00580000

/*----------------------------------------------------------------------------
 * BASE: CRYPTO0_CRYPTO_TOP
 *--------------------------------------------------------------------------*/

#define CRYPTO0_CRYPTO_TOP_BASE                                     (0x00700000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define CRYPTO0_CRYPTO_TOP_BASE_SIZE                                0x00040000
#define CRYPTO0_CRYPTO_TOP_BASE_PHYS                                0x00700000

/*----------------------------------------------------------------------------
 * BASE: QDSS_QDSS
 *--------------------------------------------------------------------------*/

#define QDSS_QDSS_BASE                                              (0x00800000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define QDSS_QDSS_BASE_SIZE                                         0x00099000
#define QDSS_QDSS_BASE_PHYS                                         0x00800000

/*----------------------------------------------------------------------------
 * BASE: QDSS_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define QDSS_WRAPPER_TOP_BASE                                       (0x00868000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define QDSS_WRAPPER_TOP_BASE_SIZE                                  0x00006000
#define QDSS_WRAPPER_TOP_BASE_PHYS                                  0x00868000

/*----------------------------------------------------------------------------
 * BASE: TLMM
 *--------------------------------------------------------------------------*/

#define TLMM_BASE                                                   (0x01000000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define TLMM_BASE_SIZE                                              0x00301000
#define TLMM_BASE_PHYS                                              0x01000000

/*----------------------------------------------------------------------------
 * BASE: CLK_CTL
 *--------------------------------------------------------------------------*/

#define CLK_CTL_BASE                                                (0x01800000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define CLK_CTL_BASE_SIZE                                           0x00082000
#define CLK_CTL_BASE_PHYS                                           0x01800000

/*----------------------------------------------------------------------------
 * BASE: CORE_TOP_CSR
 *--------------------------------------------------------------------------*/

#define CORE_TOP_CSR_BASE                                           (0x01900000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define CORE_TOP_CSR_BASE_SIZE                                      0x00058000
#define CORE_TOP_CSR_BASE_PHYS                                      0x01900000

/*----------------------------------------------------------------------------
 * BASE: MDSS_MDSS_TOP
 *--------------------------------------------------------------------------*/

#define MDSS_MDSS_TOP_BASE                                          (0x01a00000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define MDSS_MDSS_TOP_BASE_SIZE                                     0x000f2000
#define MDSS_MDSS_TOP_BASE_PHYS                                     0x01a00000

/*----------------------------------------------------------------------------
 * BASE: CAMSS_CAMERA_SS
 *--------------------------------------------------------------------------*/

#define CAMSS_CAMERA_SS_BASE                                        (0x01b00000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define CAMSS_CAMERA_SS_BASE_SIZE                                   0x00043000
#define CAMSS_CAMERA_SS_BASE_PHYS                                   0x01b00000

/*----------------------------------------------------------------------------
 * BASE: OXILI_OXILI
 *--------------------------------------------------------------------------*/

#define OXILI_OXILI_BASE                                            (0x01c00000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define OXILI_OXILI_BASE_SIZE                                       0x0000f000
#define OXILI_OXILI_BASE_PHYS                                       0x01c00000

/*----------------------------------------------------------------------------
 * BASE: VENUS0_VENUS
 *--------------------------------------------------------------------------*/

#define VENUS0_VENUS_BASE                                           (0x01d00000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define VENUS0_VENUS_BASE_SIZE                                      0x000f1000
#define VENUS0_VENUS_BASE_PHYS                                      0x01d00000

/*----------------------------------------------------------------------------
 * BASE: SMMU_500_REG_WRAPPER
 *--------------------------------------------------------------------------*/

#define SMMU_500_REG_WRAPPER_BASE                                   (0x01e00000 + HWIO_SANDBOX_RELOCATE_OFFSET)
#define SMMU_500_REG_WRAPPER_BASE_SIZE                              0x0010c000
#define SMMU_500_REG_WRAPPER_BASE_PHYS                              0x01e00000

/*----------------------------------------------------------------------------
 * BASE: PMIC_ARB
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_BASE                                               0x02000000
#define PMIC_ARB_BASE_SIZE                                          0x01906000
#define PMIC_ARB_BASE_PHYS                                          0x02000000

/*----------------------------------------------------------------------------
 * BASE: MSS_TOP
 *--------------------------------------------------------------------------*/

#define MSS_TOP_BASE                                                0x04000000
#define MSS_TOP_BASE_SIZE                                           0x0038b000
#define MSS_TOP_BASE_PHYS                                           0x04000000

/*----------------------------------------------------------------------------
 * BASE: MSS_TCM
 *--------------------------------------------------------------------------*/

#define MSS_TCM_BASE                                                0x04400000
#define MSS_TCM_BASE_SIZE                                           0x00000000
#define MSS_TCM_BASE_PHYS                                           0x04400000

/*----------------------------------------------------------------------------
 * BASE: MSS_TCM_END
 *--------------------------------------------------------------------------*/

#define MSS_TCM_END_BASE                                            0x047fffff
#define MSS_TCM_END_BASE_SIZE                                       0x00000000
#define MSS_TCM_END_BASE_PHYS                                       0x047fffff

/*----------------------------------------------------------------------------
 * BASE: ULTAUDIO_CORE
 *--------------------------------------------------------------------------*/

#define ULTAUDIO_CORE_BASE                                          0x07700000
#define ULTAUDIO_CORE_BASE_SIZE                                     0x00020000
#define ULTAUDIO_CORE_BASE_PHYS                                     0x07700000

/*----------------------------------------------------------------------------
 * BASE: LPASS_LPM
 *--------------------------------------------------------------------------*/

#define LPASS_LPM_BASE                                              0x07718000
#define LPASS_LPM_BASE_SIZE                                         0x00000000
#define LPASS_LPM_BASE_PHYS                                         0x07718000

/*----------------------------------------------------------------------------
 * BASE: LPASS_LPM_END
 *--------------------------------------------------------------------------*/

#define LPASS_LPM_END_BASE                                          0x07718fff
#define LPASS_LPM_END_BASE_SIZE                                     0x00000000
#define LPASS_LPM_END_BASE_PHYS                                     0x07718fff

/*----------------------------------------------------------------------------
 * BASE: SDC1_SDCC5_TOP
 *--------------------------------------------------------------------------*/

#define SDC1_SDCC5_TOP_BASE                                         0x07800000
#define SDC1_SDCC5_TOP_BASE_SIZE                                    0x00025000
#define SDC1_SDCC5_TOP_BASE_PHYS                                    0x07800000

/*----------------------------------------------------------------------------
 * BASE: SDC2_SDCC5_TOP
 *--------------------------------------------------------------------------*/

#define SDC2_SDCC5_TOP_BASE                                         0x07840000
#define SDC2_SDCC5_TOP_BASE_SIZE                                    0x00025000
#define SDC2_SDCC5_TOP_BASE_PHYS                                    0x07840000

/*----------------------------------------------------------------------------
 * BASE: BLSP1_BLSP
 *--------------------------------------------------------------------------*/

#define BLSP1_BLSP_BASE                                             0x07880000
#define BLSP1_BLSP_BASE_SIZE                                        0x0003b000
#define BLSP1_BLSP_BASE_PHYS                                        0x07880000

/*----------------------------------------------------------------------------
 * BASE: USB2_HSIC_USB_OTG_HS_EP8_PIPES2_BAGHEERA
 *--------------------------------------------------------------------------*/

#define USB2_HSIC_USB_OTG_HS_EP8_PIPES2_BAGHEERA_BASE               0x078c0000
#define USB2_HSIC_USB_OTG_HS_EP8_PIPES2_BAGHEERA_BASE_SIZE          0x00020000
#define USB2_HSIC_USB_OTG_HS_EP8_PIPES2_BAGHEERA_BASE_PHYS          0x078c0000

/*----------------------------------------------------------------------------
 * BASE: QPIC_QPIC
 *--------------------------------------------------------------------------*/

#define QPIC_QPIC_BASE                                              0x07980000
#define QPIC_QPIC_BASE_SIZE                                         0x00032000
#define QPIC_QPIC_BASE_PHYS                                         0x07980000

/*----------------------------------------------------------------------------
 * BASE: A7SS_L2
 *--------------------------------------------------------------------------*/

#define A7SS_L2_BASE                                                0x08000000
#define A7SS_L2_BASE_SIZE                                           0x100000000
#define A7SS_L2_BASE_PHYS                                           0x08000000

/*----------------------------------------------------------------------------
 * BASE: A7SS_L2_END
 *--------------------------------------------------------------------------*/

#define A7SS_L2_END_BASE                                            0x0807ffff
#define A7SS_L2_END_BASE_SIZE                                       0x100000000
#define A7SS_L2_END_BASE_PHYS                                       0x0807ffff

/*----------------------------------------------------------------------------
 * BASE: SYSTEM_IMEM
 *--------------------------------------------------------------------------*/

#define SYSTEM_IMEM_BASE                                            0x08600000
#define SYSTEM_IMEM_BASE_SIZE                                       0x00000000
#define SYSTEM_IMEM_BASE_PHYS                                       0x08600000

/*----------------------------------------------------------------------------
 * BASE: SYSTEM_IMEM_END
 *--------------------------------------------------------------------------*/

#define SYSTEM_IMEM_END_BASE                                        0x0861ffff
#define SYSTEM_IMEM_END_BASE_SIZE                                   0x00000000
#define SYSTEM_IMEM_END_BASE_PHYS                                   0x0861ffff

/*----------------------------------------------------------------------------
 * BASE: SNOC_1_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define SNOC_1_BUS_TIMEOUT_BASE                                     0x08800000
#define SNOC_1_BUS_TIMEOUT_BASE_SIZE                                0x00001000
#define SNOC_1_BUS_TIMEOUT_BASE_PHYS                                0x08800000

/*----------------------------------------------------------------------------
 * BASE: SNOC_0_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define SNOC_0_BUS_TIMEOUT_BASE                                     0x08801000
#define SNOC_0_BUS_TIMEOUT_BASE_SIZE                                0x00001000
#define SNOC_0_BUS_TIMEOUT_BASE_PHYS                                0x08801000

/*----------------------------------------------------------------------------
 * BASE: WCSS_WCSS
 *--------------------------------------------------------------------------*/

#define WCSS_WCSS_BASE                                              0x0a000000
#define WCSS_WCSS_BASE_SIZE                                         0x004c1000
#define WCSS_WCSS_BASE_PHYS                                         0x0a000000

/*----------------------------------------------------------------------------
 * BASE: PRONTO_CMEM
 *--------------------------------------------------------------------------*/

#define PRONTO_CMEM_BASE                                            0x0a280000
#define PRONTO_CMEM_BASE_SIZE                                       0x00000000
#define PRONTO_CMEM_BASE_PHYS                                       0x0a280000

/*----------------------------------------------------------------------------
 * BASE: PRONTO_CMEM_END
 *--------------------------------------------------------------------------*/

#define PRONTO_CMEM_END_BASE                                        0x0a287fff
#define PRONTO_CMEM_END_BASE_SIZE                                   0x00000000
#define PRONTO_CMEM_END_BASE_PHYS                                   0x0a287fff

/*----------------------------------------------------------------------------
 * BASE: A7SS
 *--------------------------------------------------------------------------*/

#define A7SS_BASE                                                   0x0b000000
#define A7SS_BASE_SIZE                                              0x000ba000
#define A7SS_BASE_PHYS                                              0x0b000000

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM_RPM_REGION1_SIZE
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_RPM_REGION1_SIZE_BASE                              0x1ff00000
#define EBI1_MEM_RPM_REGION1_SIZE_BASE_SIZE                         0x00000000
#define EBI1_MEM_RPM_REGION1_SIZE_BASE_PHYS                         0x1ff00000

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM_RPM_REGION0
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_RPM_REGION0_BASE                                   0x20000000
#define EBI1_MEM_RPM_REGION0_BASE_SIZE                              0x00000000
#define EBI1_MEM_RPM_REGION0_BASE_PHYS                              0x20000000

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM_RPM_REGION0_SIZE
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_RPM_REGION0_SIZE_BASE                              0x40000000
#define EBI1_MEM_RPM_REGION0_SIZE_BASE_SIZE                         0x00000000
#define EBI1_MEM_RPM_REGION0_SIZE_BASE_PHYS                         0x40000000

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM_RPM_REGION0_END
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_RPM_REGION0_END_BASE                               0x5fffffff
#define EBI1_MEM_RPM_REGION0_END_BASE_SIZE                          0x00000000
#define EBI1_MEM_RPM_REGION0_END_BASE_PHYS                          0x5fffffff

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM_RPM_REGION1
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_RPM_REGION1_BASE                                   0x60100000
#define EBI1_MEM_RPM_REGION1_BASE_SIZE                              0x00000000
#define EBI1_MEM_RPM_REGION1_BASE_PHYS                              0x60100000

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM_RPM_REGION1_END
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_RPM_REGION1_END_BASE                               0x7fffffff
#define EBI1_MEM_RPM_REGION1_END_BASE_SIZE                          0x00000000
#define EBI1_MEM_RPM_REGION1_END_BASE_PHYS                          0x7fffffff

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_BASE                                               0x80000000
#define EBI1_MEM_BASE_SIZE                                          0x00000000
#define EBI1_MEM_BASE_PHYS                                          0x80000000

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM_SIZE
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_SIZE_BASE                                          0x80000000
#define EBI1_MEM_SIZE_BASE_SIZE                                     0x00000000
#define EBI1_MEM_SIZE_BASE_PHYS                                     0x80000000

/*----------------------------------------------------------------------------
 * BASE: EBI1_MEM_END
 *--------------------------------------------------------------------------*/

#define EBI1_MEM_END_BASE                                           0xffffffff
#define EBI1_MEM_END_BASE_SIZE                                      0x00000000
#define EBI1_MEM_END_BASE_PHYS                                      0xffffffff


#endif /* __MSMHWIOBASE_H__ */
