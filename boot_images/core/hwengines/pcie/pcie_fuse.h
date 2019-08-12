#ifndef __PCIE_FUSE_H__
#define __PCIE_FUSE_H__
/*
===========================================================================
*/
/**
  @file pcie_fuse.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

  'Include' filters applied: FEATURE_CONFIG1[SECURITY_CONTROL_CORE] 
  'Exclude' filters applied: RESERVED DUMMY 
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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/pcie/pcie_fuse.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                                             (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                                        0x00000000

#define HWIO_FEATURE_CONFIG1_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_VENDOR_LOCK_BMSK                                                                      0xf0000000
#define HWIO_FEATURE_CONFIG1_VENDOR_LOCK_SHFT                                                                            0x1c
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                                       0x8000000
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                                            0x1b
#define HWIO_FEATURE_CONFIG1_MODEM_DSDA_DISABLE_BMSK                                                                0x4000000
#define HWIO_FEATURE_CONFIG1_MODEM_DSDA_DISABLE_SHFT                                                                     0x1a
#define HWIO_FEATURE_CONFIG1_MODEM_DSDA_DISABLE_ENABLE_FVAL                                                               0x0
#define HWIO_FEATURE_CONFIG1_MODEM_DSDA_DISABLE_DISABLE_FVAL                                                              0x1
#define HWIO_FEATURE_CONFIG1_MODEM_1X_DISABLE_EXTRA_BMSK                                                            0x2000000
#define HWIO_FEATURE_CONFIG1_MODEM_1X_DISABLE_EXTRA_SHFT                                                                 0x19
#define HWIO_FEATURE_CONFIG1_MODEM_1X_DISABLE_EXTRA_ENABLE_FVAL                                                           0x0
#define HWIO_FEATURE_CONFIG1_MODEM_1X_DISABLE_EXTRA_DISABLE_FVAL                                                          0x1
#define HWIO_FEATURE_CONFIG1_MODEM_DO_DISABLE_EXTRA_BMSK                                                            0x1000000
#define HWIO_FEATURE_CONFIG1_MODEM_DO_DISABLE_EXTRA_SHFT                                                                 0x18
#define HWIO_FEATURE_CONFIG1_MODEM_DO_DISABLE_EXTRA_ENABLE_FVAL                                                           0x0
#define HWIO_FEATURE_CONFIG1_MODEM_DO_DISABLE_EXTRA_DISABLE_FVAL                                                          0x1
#define HWIO_FEATURE_CONFIG1_MODEM_WCDMA_DISABLE_EXTRA_BMSK                                                          0x800000
#define HWIO_FEATURE_CONFIG1_MODEM_WCDMA_DISABLE_EXTRA_SHFT                                                              0x17
#define HWIO_FEATURE_CONFIG1_MODEM_WCDMA_DISABLE_EXTRA_ENABLE_FVAL                                                        0x0
#define HWIO_FEATURE_CONFIG1_MODEM_WCDMA_DISABLE_EXTRA_DISABLE_FVAL                                                       0x1
#define HWIO_FEATURE_CONFIG1_MODEM_LTE_DISABLE_EXTRA_BMSK                                                            0x400000
#define HWIO_FEATURE_CONFIG1_MODEM_LTE_DISABLE_EXTRA_SHFT                                                                0x16
#define HWIO_FEATURE_CONFIG1_MODEM_LTE_DISABLE_EXTRA_ENABLE_FVAL                                                          0x0
#define HWIO_FEATURE_CONFIG1_MODEM_LTE_DISABLE_EXTRA_DISABLE_FVAL                                                         0x1
#define HWIO_FEATURE_CONFIG1_MODEM_HSPA_DISABLE_EXTRA_BMSK                                                           0x200000
#define HWIO_FEATURE_CONFIG1_MODEM_HSPA_DISABLE_EXTRA_SHFT                                                               0x15
#define HWIO_FEATURE_CONFIG1_MODEM_HSPA_DISABLE_EXTRA_ENABLE_FVAL                                                         0x0
#define HWIO_FEATURE_CONFIG1_MODEM_HSPA_DISABLE_EXTRA_DISABLE_FVAL                                                        0x1
#define HWIO_FEATURE_CONFIG1_MODEM_TDSCDMA_DISABLE_EXTRA_BMSK                                                        0x100000
#define HWIO_FEATURE_CONFIG1_MODEM_TDSCDMA_DISABLE_EXTRA_SHFT                                                            0x14
#define HWIO_FEATURE_CONFIG1_MODEM_TDSCDMA_DISABLE_EXTRA_ENABLE_FVAL                                                      0x0
#define HWIO_FEATURE_CONFIG1_MODEM_TDSCDMA_DISABLE_EXTRA_DISABLE_FVAL                                                     0x1
#define HWIO_FEATURE_CONFIG1_MODEM_LTE_ABOVE_CAT1_DISABLE_EXTRA_BMSK                                                  0x80000
#define HWIO_FEATURE_CONFIG1_MODEM_LTE_ABOVE_CAT1_DISABLE_EXTRA_SHFT                                                     0x13
#define HWIO_FEATURE_CONFIG1_MODEM_LTE_ABOVE_CAT1_DISABLE_EXTRA_ENABLE_FVAL                                               0x0
#define HWIO_FEATURE_CONFIG1_MODEM_LTE_ABOVE_CAT1_DISABLE_EXTRA_DISABLE_FVAL                                              0x1
#define HWIO_FEATURE_CONFIG1_SPARE0_BMSK                                                                              0x70000
#define HWIO_FEATURE_CONFIG1_SPARE0_SHFT                                                                                 0x10
#define HWIO_FEATURE_CONFIG1_SLIMBUS_GPIO_PD_DISABLE_BMSK                                                              0x8000
#define HWIO_FEATURE_CONFIG1_SLIMBUS_GPIO_PD_DISABLE_SHFT                                                                 0xf
#define HWIO_FEATURE_CONFIG1_SLIMBUS_GPIO_PD_DISABLE_ENABLE_FVAL                                                          0x0
#define HWIO_FEATURE_CONFIG1_SLIMBUS_GPIO_PD_DISABLE_DISABLE_FVAL                                                         0x1
#define HWIO_FEATURE_CONFIG1_MULTIMEDIA_SPARE_DISABLE_BMSK                                                             0x7800
#define HWIO_FEATURE_CONFIG1_MULTIMEDIA_SPARE_DISABLE_SHFT                                                                0xb
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_BMSK                                                                          0x400
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_SHFT                                                                            0xa
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_ENABLE_FVAL                                                                     0x0
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_DISABLE_FVAL                                                                    0x1
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_BMSK                                                                          0x200
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_SHFT                                                                            0x9
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_ENABLE_FVAL                                                                     0x0
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_DISABLE_FVAL                                                                    0x1
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_BMSK                                                                           0x100
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_SHFT                                                                             0x8
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_BMSK                                                                            0x80
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_SHFT                                                                             0x7
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_BMSK                                                                            0x40
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_SHFT                                                                             0x6
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_BMSK                                                                   0x20
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_SHFT                                                                    0x5
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_BMSK                                                                   0x10
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_SHFT                                                                    0x4
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_BMSK                                                                    0x8
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_SHFT                                                                    0x3
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_BMSK                                                                   0x4
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_SHFT                                                                   0x2
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_ENABLE_FVAL                                                            0x0
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_DISABLE_FVAL                                                           0x1
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_BMSK                                                                     0x2
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_SHFT                                                                     0x1
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_ENABLE_FVAL                                                              0x0
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_DISABLE_FVAL                                                             0x1
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_BMSK                                                                            0x1
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_SHFT                                                                            0x0
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_ENABLE_FVAL                                                                     0x0
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_DISABLE_FVAL                                                                    0x1


#endif /* __PCIE_FUSE_H__ */
