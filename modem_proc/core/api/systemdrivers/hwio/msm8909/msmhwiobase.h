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

  $Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/hwio/msm8909/msmhwiobase.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * BASE: PRNG_PRNG
 *--------------------------------------------------------------------------*/

#define PRNG_PRNG_BASE                                              0xe0022000
#define PRNG_PRNG_BASE_SIZE                                         0x00001000
#define PRNG_PRNG_BASE_PHYS                                         0x00022000

/*----------------------------------------------------------------------------
 * BASE: XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB
 *--------------------------------------------------------------------------*/

#define XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB_BASE                  0xe012d000
#define XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB_BASE_SIZE             0x00001000
#define XPU_CFG_SNOC_CFG_MPU1132_4_M19L12_AHB_BASE_PHYS             0x0002d000

/*----------------------------------------------------------------------------
 * BASE: SPDM_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define SPDM_WRAPPER_TOP_BASE                                       0xe0240000
#define SPDM_WRAPPER_TOP_BASE_SIZE                                  0x00005000
#define SPDM_WRAPPER_TOP_BASE_PHYS                                  0x00040000

/*----------------------------------------------------------------------------
 * BASE: SECURITY_CONTROL
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_BASE                                       0xe0358000
#define SECURITY_CONTROL_BASE_SIZE                                  0x00008000
#define SECURITY_CONTROL_BASE_PHYS                                  0x00058000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_START_ADDRESS_BASE                           0xe0460000
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE                      0x00005000
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS                      0x00060000

/*----------------------------------------------------------------------------
 * BASE: PDM_PERPH_WEB
 *--------------------------------------------------------------------------*/

#define PDM_PERPH_WEB_BASE                                          0xe0568000
#define PDM_PERPH_WEB_BASE_SIZE                                     0x00004000
#define PDM_PERPH_WEB_BASE_PHYS                                     0x00068000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_START_ADDRESS_BASE_PHYS                            0x00100000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_BASE                                               0xe0600000
#define BOOT_ROM_BASE_SIZE                                          0x00100000
#define BOOT_ROM_BASE_PHYS                                          0x00100000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_END_ADDRESS
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_END_ADDRESS_BASE_PHYS                              0x00123fff

/*----------------------------------------------------------------------------
 * BASE: RPM
 *--------------------------------------------------------------------------*/

#define RPM_BASE                                                    0xe0700000
#define RPM_BASE_SIZE                                               0x00089000
#define RPM_BASE_PHYS                                               0x00200000

/*----------------------------------------------------------------------------
 * BASE: BIMC
 *--------------------------------------------------------------------------*/

#define BIMC_BASE                                                   0xe0800000
#define BIMC_BASE_SIZE                                              0x0005a000
#define BIMC_BASE_PHYS                                              0x00400000

/*----------------------------------------------------------------------------
 * BASE: MPM2_MPM
 *--------------------------------------------------------------------------*/

#define MPM2_MPM_BASE                                               0xe09a0000
#define MPM2_MPM_BASE_SIZE                                          0x0000c000
#define MPM2_MPM_BASE_PHYS                                          0x004a0000

/*----------------------------------------------------------------------------
 * BASE: DEHR_BIMC_WRAPPER
 *--------------------------------------------------------------------------*/

#define DEHR_BIMC_WRAPPER_BASE                                      0xe0ab0000
#define DEHR_BIMC_WRAPPER_BASE_SIZE                                 0x00004000
#define DEHR_BIMC_WRAPPER_BASE_PHYS                                 0x004b0000

/*----------------------------------------------------------------------------
 * BASE: PC_NOC
 *--------------------------------------------------------------------------*/

#define PC_NOC_BASE                                                 0xe0b00000
#define PC_NOC_BASE_SIZE                                            0x00011000
#define PC_NOC_BASE_PHYS                                            0x00500000

/*----------------------------------------------------------------------------
 * BASE: SYSTEM_NOC
 *--------------------------------------------------------------------------*/

#define SYSTEM_NOC_BASE                                             0xe0c80000
#define SYSTEM_NOC_BASE_SIZE                                        0x00013000
#define SYSTEM_NOC_BASE_PHYS                                        0x00580000

/*----------------------------------------------------------------------------
 * BASE: CRYPTO0_CRYPTO_TOP
 *--------------------------------------------------------------------------*/

#define CRYPTO0_CRYPTO_TOP_BASE                                     0xe0d00000
#define CRYPTO0_CRYPTO_TOP_BASE_SIZE                                0x00040000
#define CRYPTO0_CRYPTO_TOP_BASE_PHYS                                0x00700000

/*----------------------------------------------------------------------------
 * BASE: QDSS_QDSS
 *--------------------------------------------------------------------------*/

#define QDSS_QDSS_BASE                                              0xe0e00000
#define QDSS_QDSS_BASE_SIZE                                         0x00099000
#define QDSS_QDSS_BASE_PHYS                                         0x00800000

/*----------------------------------------------------------------------------
 * BASE: TLMM
 *--------------------------------------------------------------------------*/

#define TLMM_BASE                                                   0xe1000000
#define TLMM_BASE_SIZE                                              0x00301000
#define TLMM_BASE_PHYS                                              0x01000000

/*----------------------------------------------------------------------------
 * BASE: CLK_CTL
 *--------------------------------------------------------------------------*/

#define CLK_CTL_BASE                                                0xe1400000
#define CLK_CTL_BASE_SIZE                                           0x00082000
#define CLK_CTL_BASE_PHYS                                           0x01800000

/*----------------------------------------------------------------------------
 * BASE: CORE_TOP_CSR
 *--------------------------------------------------------------------------*/

#define CORE_TOP_CSR_BASE                                           0xe1500000
#define CORE_TOP_CSR_BASE_SIZE                                      0x00058000
#define CORE_TOP_CSR_BASE_PHYS                                      0x01900000

/*----------------------------------------------------------------------------
 * BASE: PMIC_ARB
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_BASE                                               0xe2000000
#define PMIC_ARB_BASE_SIZE                                          0x01906000
#define PMIC_ARB_BASE_PHYS                                          0x02000000

/*----------------------------------------------------------------------------
 * BASE: MSS_TOP
 *--------------------------------------------------------------------------*/

#define MSS_TOP_BASE                                                0xec000000
#define MSS_TOP_BASE_SIZE                                           0x0038b000
#define MSS_TOP_BASE_PHYS                                           0x04000000

/*----------------------------------------------------------------------------
 * BASE: ULTAUDIO_CORE
 *--------------------------------------------------------------------------*/

#define ULTAUDIO_CORE_BASE                                          0xe4000000
#define ULTAUDIO_CORE_BASE_SIZE                                     0x0001d000
#define ULTAUDIO_CORE_BASE_PHYS                                     0x07700000

/*----------------------------------------------------------------------------
 * BASE: BLSP1_BLSP
 *--------------------------------------------------------------------------*/

#define BLSP1_BLSP_BASE                                             0xe4180000
#define BLSP1_BLSP_BASE_SIZE                                        0x0003b000
#define BLSP1_BLSP_BASE_PHYS                                        0x07880000

/*----------------------------------------------------------------------------
 * BASE: QPIC_QPIC
 *--------------------------------------------------------------------------*/

#define QPIC_QPIC_BASE                                              0xe4280000
#define QPIC_QPIC_BASE_SIZE                                         0x00032000
#define QPIC_QPIC_BASE_PHYS                                         0x07980000

/*----------------------------------------------------------------------------
 * BASE: WCSS_WCSS
 *--------------------------------------------------------------------------*/

#define WCSS_WCSS_BASE                                              0xe5000000
#define WCSS_WCSS_BASE_SIZE                                         0x004c1000
#define WCSS_WCSS_BASE_PHYS                                         0x0a000000


#endif /* __MSMHWIOBASE_H__ */
