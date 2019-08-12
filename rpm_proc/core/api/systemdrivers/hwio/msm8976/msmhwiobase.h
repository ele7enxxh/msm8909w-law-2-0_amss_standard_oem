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

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/hwio/msm8976/msmhwiobase.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * BASE: RPM_CODE_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_CODE_RAM_START_ADDRESS_BASE                             0x00000000
#define RPM_CODE_RAM_START_ADDRESS_BASE_SIZE                        0x00000000
#define RPM_CODE_RAM_START_ADDRESS_BASE_PHYS                        0x00000000

/*----------------------------------------------------------------------------
 * BASE: RPM
 *--------------------------------------------------------------------------*/

#define RPM_BASE                                                    0x00000000
#define RPM_BASE_SIZE                                               0x00089000
#define RPM_BASE_PHYS                                               0x00000000

/*----------------------------------------------------------------------------
 * BASE: PRNG_PRNG
 *--------------------------------------------------------------------------*/

#define PRNG_PRNG_BASE                                              0x60022000
#define PRNG_PRNG_BASE_SIZE                                         0x00001000
#define PRNG_PRNG_BASE_PHYS                                         0x60022000

/*----------------------------------------------------------------------------
 * BASE: SPDM_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define SPDM_WRAPPER_TOP_BASE                                       0x60040000
#define SPDM_WRAPPER_TOP_BASE_SIZE                                  0x00005000
#define SPDM_WRAPPER_TOP_BASE_PHYS                                  0x60040000

/*----------------------------------------------------------------------------
 * BASE: RPM_MSG_RAM
 *--------------------------------------------------------------------------*/

#define RPM_MSG_RAM_BASE                                            0x60060000
#define RPM_MSG_RAM_BASE_SIZE                                       0x00005000
#define RPM_MSG_RAM_BASE_PHYS                                       0x60060000

/*----------------------------------------------------------------------------
 * BASE: SECURITY_CONTROL
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_BASE                                       0x600a0000
#define SECURITY_CONTROL_BASE_SIZE                                  0x0000f000
#define SECURITY_CONTROL_BASE_PHYS                                  0x600a0000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_START_ADDRESS_BASE                                 0x60100000
#define BOOT_ROM_START_ADDRESS_BASE_SIZE                            0x00000000
#define BOOT_ROM_START_ADDRESS_BASE_PHYS                            0x60100000

/*----------------------------------------------------------------------------
 * BASE: BIMC
 *--------------------------------------------------------------------------*/

#define BIMC_BASE                                                   0x60400000
#define BIMC_BASE_SIZE                                              0x0006a000
#define BIMC_BASE_PHYS                                              0x60400000

/*----------------------------------------------------------------------------
 * BASE: MPM2_MPM
 *--------------------------------------------------------------------------*/

#define MPM2_MPM_BASE                                               0x604a0000
#define MPM2_MPM_BASE_SIZE                                          0x0000c000
#define MPM2_MPM_BASE_PHYS                                          0x604a0000

/*----------------------------------------------------------------------------
 * BASE: DEHR_BIMC_WRAPPER
 *--------------------------------------------------------------------------*/

#define DEHR_BIMC_WRAPPER_BASE                                      0x604b0000
#define DEHR_BIMC_WRAPPER_BASE_SIZE                                 0x00004000
#define DEHR_BIMC_WRAPPER_BASE_PHYS                                 0x604b0000

/*----------------------------------------------------------------------------
 * BASE: PC_NOC
 *--------------------------------------------------------------------------*/

#define PC_NOC_BASE                                                 0x60500000
#define PC_NOC_BASE_SIZE                                            0x00014000
#define PC_NOC_BASE_PHYS                                            0x60500000

/*----------------------------------------------------------------------------
 * BASE: SYSTEM_NOC
 *--------------------------------------------------------------------------*/

#define SYSTEM_NOC_BASE                                             0x60580000
#define SYSTEM_NOC_BASE_SIZE                                        0x0001b000
#define SYSTEM_NOC_BASE_PHYS                                        0x60580000

/*----------------------------------------------------------------------------
 * BASE: CRYPTO0_CRYPTO_TOP
 *--------------------------------------------------------------------------*/

#define CRYPTO0_CRYPTO_TOP_BASE                                     0x60700000
#define CRYPTO0_CRYPTO_TOP_BASE_SIZE                                0x00040000
#define CRYPTO0_CRYPTO_TOP_BASE_PHYS                                0x60700000

/*----------------------------------------------------------------------------
 * BASE: TLMM
 *--------------------------------------------------------------------------*/

#define TLMM_BASE                                                   0x61000000
#define TLMM_BASE_SIZE                                              0x00301000
#define TLMM_BASE_PHYS                                              0x61000000

/*----------------------------------------------------------------------------
 * BASE: CLK_CTL
 *--------------------------------------------------------------------------*/

#define CLK_CTL_BASE                                                0x61800000
#define CLK_CTL_BASE_SIZE                                           0x00082000
#define CLK_CTL_BASE_PHYS                                           0x61800000

/*----------------------------------------------------------------------------
 * BASE: CORE_TOP_CSR
 *--------------------------------------------------------------------------*/

#define CORE_TOP_CSR_BASE                                           0x61900000
#define CORE_TOP_CSR_BASE_SIZE                                      0x00058000
#define CORE_TOP_CSR_BASE_PHYS                                      0x61900000

/*----------------------------------------------------------------------------
 * BASE: PMIC_ARB
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_BASE                                               0x62000000
#define PMIC_ARB_BASE_SIZE                                          0x01907000
#define PMIC_ARB_BASE_PHYS                                          0x62000000

/*----------------------------------------------------------------------------
 * BASE: QDSS_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define QDSS_WRAPPER_TOP_BASE                                       0x66000000
#define QDSS_WRAPPER_TOP_BASE_SIZE                                  0x00099000
#define QDSS_WRAPPER_TOP_BASE_PHYS                                  0x66000000

/*----------------------------------------------------------------------------
 * BASE: BLSP1_BLSP
 *--------------------------------------------------------------------------*/

#define BLSP1_BLSP_BASE                                             0x67880000
#define BLSP1_BLSP_BASE_SIZE                                        0x0003b000
#define BLSP1_BLSP_BASE_PHYS                                        0x67880000


#endif /* __MSMHWIOBASE_H__ */
