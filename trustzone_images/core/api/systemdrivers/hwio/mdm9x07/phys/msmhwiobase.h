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

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/hwio/mdm9x07/phys/msmhwiobase.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * BASE: LPASS_LPM_SIZE
 *--------------------------------------------------------------------------*/

#define LPASS_LPM_SIZE_BASE                                         0x00001000
#define LPASS_LPM_SIZE_BASE_SIZE                                    0x00000000
#define LPASS_LPM_SIZE_BASE_PHYS                                    0x00001000

/*----------------------------------------------------------------------------
 * BASE: IMEM_SIZE
 *--------------------------------------------------------------------------*/

#define IMEM_SIZE_BASE                                              0x00004000
#define IMEM_SIZE_BASE_SIZE                                         0x00000000
#define IMEM_SIZE_BASE_PHYS                                         0x00004000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_SIZE
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_SIZE_BASE                                    0x00008000
#define RPM_SS_MSG_RAM_SIZE_BASE_SIZE                               0x00000000
#define RPM_SS_MSG_RAM_SIZE_BASE_PHYS                               0x00008000

/*----------------------------------------------------------------------------
 * BASE: PRNG_PRNG
 *--------------------------------------------------------------------------*/

#define PRNG_PRNG_BASE                                              0x00022000
#define PRNG_PRNG_BASE_SIZE                                         0x00001000
#define PRNG_PRNG_BASE_PHYS                                         0x00022000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_0_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_0_BUS_TIMEOUT_BASE                                    0x00023000
#define PCNOC_0_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_0_BUS_TIMEOUT_BASE_PHYS                               0x00023000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_SIZE
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_SIZE_BASE                                          0x00024000
#define BOOT_ROM_SIZE_BASE_SIZE                                     0x00000000
#define BOOT_ROM_SIZE_BASE_PHYS                                     0x00024000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_1_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_1_BUS_TIMEOUT_BASE                                    0x00024000
#define PCNOC_1_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_1_BUS_TIMEOUT_BASE_PHYS                               0x00024000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_2_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_2_BUS_TIMEOUT_BASE                                    0x00025000
#define PCNOC_2_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_2_BUS_TIMEOUT_BASE_PHYS                               0x00025000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_3_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_3_BUS_TIMEOUT_BASE                                    0x00026000
#define PCNOC_3_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_3_BUS_TIMEOUT_BASE_PHYS                               0x00026000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_5_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_5_BUS_TIMEOUT_BASE                                    0x00028000
#define PCNOC_5_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_5_BUS_TIMEOUT_BASE_PHYS                               0x00028000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_6_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_6_BUS_TIMEOUT_BASE                                    0x00029000
#define PCNOC_6_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_6_BUS_TIMEOUT_BASE_PHYS                               0x00029000

/*----------------------------------------------------------------------------
 * BASE: PCNOC_7_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_7_BUS_TIMEOUT_BASE                                    0x0002a000
#define PCNOC_7_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_7_BUS_TIMEOUT_BASE_PHYS                               0x0002a000

/*----------------------------------------------------------------------------
 * BASE: XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB
 *--------------------------------------------------------------------------*/

#define XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB_BASE                 0x0002e000
#define XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB_BASE_SIZE            0x00001000
#define XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB_BASE_PHYS            0x0002e000

/*----------------------------------------------------------------------------
 * BASE: XPU_CFG_PRNG_CFG_APU1132_1
 *--------------------------------------------------------------------------*/

#define XPU_CFG_PRNG_CFG_APU1132_1_BASE                             0x0002f000
#define XPU_CFG_PRNG_CFG_APU1132_1_BASE_SIZE                        0x00001000
#define XPU_CFG_PRNG_CFG_APU1132_1_BASE_PHYS                        0x0002f000

/*----------------------------------------------------------------------------
 * BASE: BIMC_MPU_MPU0032A_20_M31L10_AXI
 *--------------------------------------------------------------------------*/

#define BIMC_MPU_MPU0032A_20_M31L10_AXI_BASE                        0x00032000
#define BIMC_MPU_MPU0032A_20_M31L10_AXI_BASE_SIZE                   0x00001000
#define BIMC_MPU_MPU0032A_20_M31L10_AXI_BASE_PHYS                   0x00032000

/*----------------------------------------------------------------------------
 * BASE: XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB
 *--------------------------------------------------------------------------*/

#define XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB_BASE                   0x00033000
#define XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB_BASE_SIZE              0x00001000
#define XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB_BASE_PHYS              0x00033000

/*----------------------------------------------------------------------------
 * BASE: SMMU_500_MPU_WRAPPER
 *--------------------------------------------------------------------------*/

#define SMMU_500_MPU_WRAPPER_BASE                                   0x00034000
#define SMMU_500_MPU_WRAPPER_BASE_SIZE                              0x00004000
#define SMMU_500_MPU_WRAPPER_BASE_PHYS                              0x00034000

/*----------------------------------------------------------------------------
 * BASE: A7SS_L2_SIZE
 *--------------------------------------------------------------------------*/

#define A7SS_L2_SIZE_BASE                                           0x00040000
#define A7SS_L2_SIZE_BASE_SIZE                                      0x00000000
#define A7SS_L2_SIZE_BASE_PHYS                                      0x00040000

/*----------------------------------------------------------------------------
 * BASE: SPDM_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define SPDM_WRAPPER_TOP_BASE                                       0x00040000
#define SPDM_WRAPPER_TOP_BASE_SIZE                                  0x00005000
#define SPDM_WRAPPER_TOP_BASE_PHYS                                  0x00040000

/*----------------------------------------------------------------------------
 * BASE: RBCPR_WRAPPER
 *--------------------------------------------------------------------------*/

#define RBCPR_WRAPPER_BASE                                          0x00048000
#define RBCPR_WRAPPER_BASE_SIZE                                     0x00001000
#define RBCPR_WRAPPER_BASE_PHYS                                     0x00048000

/*----------------------------------------------------------------------------
 * BASE: OCIMEM_WRAPPER_CSR
 *--------------------------------------------------------------------------*/

#define OCIMEM_WRAPPER_CSR_BASE                                     0x00050000
#define OCIMEM_WRAPPER_CSR_BASE_SIZE                                0x00004000
#define OCIMEM_WRAPPER_CSR_BASE_PHYS                                0x00050000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_START_ADDRESS_BASE                           0x00060000
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE                      0x00000000
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS                      0x00060000

/*----------------------------------------------------------------------------
 * BASE: RPM_MSG_RAM
 *--------------------------------------------------------------------------*/

#define RPM_MSG_RAM_BASE                                            0x00060000
#define RPM_MSG_RAM_BASE_SIZE                                       0x00000000
#define RPM_MSG_RAM_BASE_PHYS                                       0x00060000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_END_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_END_ADDRESS_BASE                             0x00067fff
#define RPM_SS_MSG_RAM_END_ADDRESS_BASE_SIZE                        0x00000000
#define RPM_SS_MSG_RAM_END_ADDRESS_BASE_PHYS                        0x00067fff

/*----------------------------------------------------------------------------
 * BASE: PDM_PERPH_WEB
 *--------------------------------------------------------------------------*/

#define PDM_PERPH_WEB_BASE                                          0x00068000
#define PDM_PERPH_WEB_BASE_SIZE                                     0x00004000
#define PDM_PERPH_WEB_BASE_PHYS                                     0x00068000

/*----------------------------------------------------------------------------
 * BASE: USB2_PHY_CM_DWC_USB2_SW
 *--------------------------------------------------------------------------*/

#define USB2_PHY_CM_DWC_USB2_SW_BASE                                0x0006c000
#define USB2_PHY_CM_DWC_USB2_SW_BASE_SIZE                           0x00001000
#define USB2_PHY_CM_DWC_USB2_SW_BASE_PHYS                           0x0006c000

/*----------------------------------------------------------------------------
 * BASE: RPM_CODE_RAM_SIZE
 *--------------------------------------------------------------------------*/

#define RPM_CODE_RAM_SIZE_BASE                                      0x00080000
#define RPM_CODE_RAM_SIZE_BASE_SIZE                                 0x00000000
#define RPM_CODE_RAM_SIZE_BASE_PHYS                                 0x00080000

/*----------------------------------------------------------------------------
 * BASE: SECURITY_CONTROL
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_BASE                                       0x000a0000
#define SECURITY_CONTROL_BASE_SIZE                                  0x0000f000
#define SECURITY_CONTROL_BASE_PHYS                                  0x000a0000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_START_ADDRESS_BASE                                 0x00100000
#define BOOT_ROM_START_ADDRESS_BASE_SIZE                            0x00000000
#define BOOT_ROM_START_ADDRESS_BASE_PHYS                            0x00100000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_BASE                                               0x00100000
#define BOOT_ROM_BASE_SIZE                                          0x00100000
#define BOOT_ROM_BASE_PHYS                                          0x00100000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_END_ADDRESS
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_END_ADDRESS_BASE                                   0x00123fff
#define BOOT_ROM_END_ADDRESS_BASE_SIZE                              0x00000000
#define BOOT_ROM_END_ADDRESS_BASE_PHYS                              0x00123fff

/*----------------------------------------------------------------------------
 * BASE: RPM_CODE_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_CODE_RAM_START_ADDRESS_BASE                             0x00200000
#define RPM_CODE_RAM_START_ADDRESS_BASE_SIZE                        0x00000000
#define RPM_CODE_RAM_START_ADDRESS_BASE_PHYS                        0x00200000

/*----------------------------------------------------------------------------
 * BASE: RPM
 *--------------------------------------------------------------------------*/

#define RPM_BASE                                                    0x00200000
#define RPM_BASE_SIZE                                               0x00089000
#define RPM_BASE_PHYS                                               0x00200000

/*----------------------------------------------------------------------------
 * BASE: RPM_CODE_RAM_END_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_CODE_RAM_END_ADDRESS_BASE                               0x0027ffff
#define RPM_CODE_RAM_END_ADDRESS_BASE_SIZE                          0x00000000
#define RPM_CODE_RAM_END_ADDRESS_BASE_PHYS                          0x0027ffff

/*----------------------------------------------------------------------------
 * BASE: MSS_TCM_SIZE
 *--------------------------------------------------------------------------*/

#define MSS_TCM_SIZE_BASE                                           0x00400000
#define MSS_TCM_SIZE_BASE_SIZE                                      0x00000000
#define MSS_TCM_SIZE_BASE_PHYS                                      0x00400000

/*----------------------------------------------------------------------------
 * BASE: BIMC
 *--------------------------------------------------------------------------*/

#define BIMC_BASE                                                   0x00400000
#define BIMC_BASE_SIZE                                              0x00060000
#define BIMC_BASE_PHYS                                              0x00400000

/*----------------------------------------------------------------------------
 * BASE: EBI1_PHY_CFG
 *--------------------------------------------------------------------------*/

#define EBI1_PHY_CFG_BASE                                           0x00480000
#define EBI1_PHY_CFG_BASE_SIZE                                      0x00020000
#define EBI1_PHY_CFG_BASE_PHYS                                      0x00480000

/*----------------------------------------------------------------------------
 * BASE: MPM2_MPM
 *--------------------------------------------------------------------------*/

#define MPM2_MPM_BASE                                               0x004a0000
#define MPM2_MPM_BASE_SIZE                                          0x00010000
#define MPM2_MPM_BASE_PHYS                                          0x004a0000

/*----------------------------------------------------------------------------
 * BASE: DEHR_BIMC_WRAPPER
 *--------------------------------------------------------------------------*/

#define DEHR_BIMC_WRAPPER_BASE                                      0x004b0000
#define DEHR_BIMC_WRAPPER_BASE_SIZE                                 0x00004000
#define DEHR_BIMC_WRAPPER_BASE_PHYS                                 0x004b0000

/*----------------------------------------------------------------------------
 * BASE: PC_NOC
 *--------------------------------------------------------------------------*/

#define PC_NOC_BASE                                                 0x00500000
#define PC_NOC_BASE_SIZE                                            0x00016000
#define PC_NOC_BASE_PHYS                                            0x00500000

/*----------------------------------------------------------------------------
 * BASE: CRYPTO0_CRYPTO_TOP
 *--------------------------------------------------------------------------*/

#define CRYPTO0_CRYPTO_TOP_BASE                                     0x00700000
#define CRYPTO0_CRYPTO_TOP_BASE_SIZE                                0x00040000
#define CRYPTO0_CRYPTO_TOP_BASE_PHYS                                0x00700000

/*----------------------------------------------------------------------------
 * BASE: QDSS_STM_SIZE
 *--------------------------------------------------------------------------*/

#define QDSS_STM_SIZE_BASE                                          0x01000000
#define QDSS_STM_SIZE_BASE_SIZE                                     0x00000000
#define QDSS_STM_SIZE_BASE_PHYS                                     0x01000000

/*----------------------------------------------------------------------------
 * BASE: TLMM
 *--------------------------------------------------------------------------*/

#define TLMM_BASE                                                   0x01000000
#define TLMM_BASE_SIZE                                              0x00310000
#define TLMM_BASE_PHYS                                              0x01000000

/*----------------------------------------------------------------------------
 * BASE: CLK_CTL
 *--------------------------------------------------------------------------*/

#define CLK_CTL_BASE                                                0x01800000
#define CLK_CTL_BASE_SIZE                                           0x00082000
#define CLK_CTL_BASE_PHYS                                           0x01800000

/*----------------------------------------------------------------------------
 * BASE: CORE_TOP_CSR
 *--------------------------------------------------------------------------*/

#define CORE_TOP_CSR_BASE                                           0x01900000
#define CORE_TOP_CSR_BASE_SIZE                                      0x00058000
#define CORE_TOP_CSR_BASE_PHYS                                      0x01900000

/*----------------------------------------------------------------------------
 * BASE: SMMU_500_REG_WRAPPER
 *--------------------------------------------------------------------------*/

#define SMMU_500_REG_WRAPPER_BASE                                   0x01e00000
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
 * BASE: QDSS_QDSS_APB
 *--------------------------------------------------------------------------*/

#define QDSS_QDSS_APB_BASE                                          0x06000000
#define QDSS_QDSS_APB_BASE_SIZE                                     0x00027000
#define QDSS_QDSS_APB_BASE_PHYS                                     0x06000000

/*----------------------------------------------------------------------------
 * BASE: QDSS_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define QDSS_WRAPPER_TOP_BASE                                       0x06068000
#define QDSS_WRAPPER_TOP_BASE_SIZE                                  0x00006000
#define QDSS_WRAPPER_TOP_BASE_PHYS                                  0x06068000

/*----------------------------------------------------------------------------
 * BASE: QDSS_QDSS_AHB
 *--------------------------------------------------------------------------*/

#define QDSS_QDSS_AHB_BASE                                          0x06070000
#define QDSS_QDSS_AHB_BASE_SIZE                                     0x00029000
#define QDSS_QDSS_AHB_BASE_PHYS                                     0x06070000

/*----------------------------------------------------------------------------
 * BASE: ULTAUDIO_CORE
 *--------------------------------------------------------------------------*/

#define ULTAUDIO_CORE_BASE                                          0x07700000
#define ULTAUDIO_CORE_BASE_SIZE                                     0x00019000
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
 * BASE: USB_OTG_HS_EP16_PIPES2
 *--------------------------------------------------------------------------*/

#define USB_OTG_HS_EP16_PIPES2_BASE                                 0x078c0000
#define USB_OTG_HS_EP16_PIPES2_BASE_SIZE                            0x0001a000
#define USB_OTG_HS_EP16_PIPES2_BASE_PHYS                            0x078c0000

/*----------------------------------------------------------------------------
 * BASE: QPIC_QPIC
 *--------------------------------------------------------------------------*/

#define QPIC_QPIC_BASE                                              0x07980000
#define QPIC_QPIC_BASE_SIZE                                         0x00038000
#define QPIC_QPIC_BASE_PHYS                                         0x07980000

/*----------------------------------------------------------------------------
 * BASE: USB2_HSIC_USB_OTG_HS_HSIC_TOP_NO_SEC
 *--------------------------------------------------------------------------*/

#define USB2_HSIC_USB_OTG_HS_HSIC_TOP_NO_SEC_BASE                   0x07c00000
#define USB2_HSIC_USB_OTG_HS_HSIC_TOP_NO_SEC_BASE_SIZE              0x00001000
#define USB2_HSIC_USB_OTG_HS_HSIC_TOP_NO_SEC_BASE_PHYS              0x07c00000

/*----------------------------------------------------------------------------
 * BASE: EMAC_0_EMAC_SGMII_TOP_WRAPPER_LYKAN
 *--------------------------------------------------------------------------*/

#define EMAC_0_EMAC_SGMII_TOP_WRAPPER_LYKAN_BASE                    0x07c40000
#define EMAC_0_EMAC_SGMII_TOP_WRAPPER_LYKAN_BASE_SIZE               0x0001e000
#define EMAC_0_EMAC_SGMII_TOP_WRAPPER_LYKAN_BASE_PHYS               0x07c40000

/*----------------------------------------------------------------------------
 * BASE: A7SS_L2
 *--------------------------------------------------------------------------*/

#define A7SS_L2_BASE                                                0x08000000
#define A7SS_L2_BASE_SIZE                                           0x00000000
#define A7SS_L2_BASE_PHYS                                           0x08000000

/*----------------------------------------------------------------------------
 * BASE: A7SS_L2_END
 *--------------------------------------------------------------------------*/

#define A7SS_L2_END_BASE                                            0x0803ffff
#define A7SS_L2_END_BASE_SIZE                                       0x00000000
#define A7SS_L2_END_BASE_PHYS                                       0x0803ffff

/*----------------------------------------------------------------------------
 * BASE: IMEM
 *--------------------------------------------------------------------------*/

#define IMEM_BASE                                                   0x08600000
#define IMEM_BASE_SIZE                                              0x00000000
#define IMEM_BASE_PHYS                                              0x08600000

/*----------------------------------------------------------------------------
 * BASE: IMEM_END
 *--------------------------------------------------------------------------*/

#define IMEM_END_BASE                                               0x08603fff
#define IMEM_END_BASE_SIZE                                          0x00000000
#define IMEM_END_BASE_PHYS                                          0x08603fff

/*----------------------------------------------------------------------------
 * BASE: PCNOC_4_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_4_BUS_TIMEOUT_BASE                                    0x08801000
#define PCNOC_4_BUS_TIMEOUT_BASE_SIZE                               0x00001000
#define PCNOC_4_BUS_TIMEOUT_BASE_PHYS                               0x08801000

/*----------------------------------------------------------------------------
 * BASE: QDSS_STM
 *--------------------------------------------------------------------------*/

#define QDSS_STM_BASE                                               0x09000000
#define QDSS_STM_BASE_SIZE                                          0x00000000
#define QDSS_STM_BASE_PHYS                                          0x09000000

/*----------------------------------------------------------------------------
 * BASE: QDSS_STM_END
 *--------------------------------------------------------------------------*/

#define QDSS_STM_END_BASE                                           0x09ffffff
#define QDSS_STM_END_BASE_SIZE                                      0x00000000
#define QDSS_STM_END_BASE_PHYS                                      0x09ffffff

/*----------------------------------------------------------------------------
 * BASE: A7SS
 *--------------------------------------------------------------------------*/

#define A7SS_BASE                                                   0x0b000000
#define A7SS_BASE_SIZE                                              0x0002a000
#define A7SS_BASE_PHYS                                              0x0b000000

/*----------------------------------------------------------------------------
 * BASE: CATS_64BIT
 *--------------------------------------------------------------------------*/

#define CATS_64BIT_BASE                                             0x20000000
#define CATS_64BIT_BASE_SIZE                                        0x00000000
#define CATS_64BIT_BASE_PHYS                                        0x20000000

/*----------------------------------------------------------------------------
 * BASE: CATS_64BIT_SIZE
 *--------------------------------------------------------------------------*/

#define CATS_64BIT_SIZE_BASE                                        0x20000000
#define CATS_64BIT_SIZE_BASE_SIZE                                   0x00000000
#define CATS_64BIT_SIZE_BASE_PHYS                                   0x20000000

/*----------------------------------------------------------------------------
 * BASE: CATS_64BIT_END
 *--------------------------------------------------------------------------*/

#define CATS_64BIT_END_BASE                                         0x3fffffff
#define CATS_64BIT_END_BASE_SIZE                                    0x00000000
#define CATS_64BIT_END_BASE_PHYS                                    0x3fffffff

/*----------------------------------------------------------------------------
 * BASE: DDR
 *--------------------------------------------------------------------------*/

#define DDR_BASE                                                    0x40000000
#define DDR_BASE_SIZE                                               0x00000000
#define DDR_BASE_PHYS                                               0x40000000

/*----------------------------------------------------------------------------
 * BASE: DDR_SIZE
 *--------------------------------------------------------------------------*/

#define DDR_SIZE_BASE                                               0xc0000000
#define DDR_SIZE_BASE_SIZE                                          0x00000000
#define DDR_SIZE_BASE_PHYS                                          0xc0000000

/*----------------------------------------------------------------------------
 * BASE: DDR_END
 *--------------------------------------------------------------------------*/

#define DDR_END_BASE                                                0xffffffff
#define DDR_END_BASE_SIZE                                           0x00000000
#define DDR_END_BASE_PHYS                                           0xffffffff


#endif /* __MSMHWIOBASE_H__ */
