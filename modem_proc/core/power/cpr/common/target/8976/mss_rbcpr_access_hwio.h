#ifndef __MSS_RBCPR_ACCESS_HWIO_H__
#define __MSS_RBCPR_ACCESS_HWIO_H__
/*
===========================================================================
*/
/**
  @file mss_rbcpr_access_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8956/76 (Eldarion) [eldarion_v1.0_p3q3r41_MTO]
 
  This file contains HWIO register definitions for the following modules:
    MSS_PERPH

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/cpr/common/target/8976/mss_rbcpr_access_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"  

/*----------------------------------------------------------------------------
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                  (MSS_TOP_BASE      + 0x00180000)

#define HWIO_MSS_ENABLE_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00000000)
#define HWIO_MSS_ENABLE_RMSK                                                0x80000001
#define HWIO_MSS_ENABLE_IN          \
        in_dword_masked(HWIO_MSS_ENABLE_ADDR, HWIO_MSS_ENABLE_RMSK)
#define HWIO_MSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_MSS_ENABLE_ADDR, m)
#define HWIO_MSS_ENABLE_OUT(v)      \
        out_dword(HWIO_MSS_ENABLE_ADDR,v)
#define HWIO_MSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ENABLE_ADDR,m,v,HWIO_MSS_ENABLE_IN)
#define HWIO_MSS_ENABLE_MODEM_ARES_IN_BMSK                                  0x80000000
#define HWIO_MSS_ENABLE_MODEM_ARES_IN_SHFT                                        0x1f
#define HWIO_MSS_ENABLE_MODEM_BMSK                                                 0x1
#define HWIO_MSS_ENABLE_MODEM_SHFT                                                 0x0

#define HWIO_MSS_CLAMP_MEM_ADDR                                             (MSS_PERPH_REG_BASE      + 0x00000004)
#define HWIO_MSS_CLAMP_MEM_RMSK                                                    0x7
#define HWIO_MSS_CLAMP_MEM_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, HWIO_MSS_CLAMP_MEM_RMSK)
#define HWIO_MSS_CLAMP_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, m)
#define HWIO_MSS_CLAMP_MEM_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_MEM_ADDR,v)
#define HWIO_MSS_CLAMP_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_MEM_ADDR,m,v,HWIO_MSS_CLAMP_MEM_IN)
#define HWIO_MSS_CLAMP_MEM_SPARE_BMSK                                              0x4
#define HWIO_MSS_CLAMP_MEM_SPARE_SHFT                                              0x2
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_BMSK                                        0x2
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_SHFT                                        0x1
#define HWIO_MSS_CLAMP_MEM_HM_CLAMP_BMSK                                           0x1
#define HWIO_MSS_CLAMP_MEM_HM_CLAMP_SHFT                                           0x0

#define HWIO_MSS_CLAMP_IO_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00000008)
#define HWIO_MSS_CLAMP_IO_RMSK                                                    0xff
#define HWIO_MSS_CLAMP_IO_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, HWIO_MSS_CLAMP_IO_RMSK)
#define HWIO_MSS_CLAMP_IO_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, m)
#define HWIO_MSS_CLAMP_IO_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_IO_ADDR,v)
#define HWIO_MSS_CLAMP_IO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_IO_ADDR,m,v,HWIO_MSS_CLAMP_IO_IN)
#define HWIO_MSS_CLAMP_IO_SPARE_7_BMSK                                            0x80
#define HWIO_MSS_CLAMP_IO_SPARE_7_SHFT                                             0x7
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_BMSK                                        0x40
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_SHFT                                         0x6
#define HWIO_MSS_CLAMP_IO_RBCPR_BMSK                                              0x20
#define HWIO_MSS_CLAMP_IO_RBCPR_SHFT                                               0x5
#define HWIO_MSS_CLAMP_IO_BBRX_ADC_BMSK                                           0x10
#define HWIO_MSS_CLAMP_IO_BBRX_ADC_SHFT                                            0x4
#define HWIO_MSS_CLAMP_IO_GNSS_ADC_BMSK                                            0x8
#define HWIO_MSS_CLAMP_IO_GNSS_ADC_SHFT                                            0x3
#define HWIO_MSS_CLAMP_IO_COM_COMP_BMSK                                            0x4
#define HWIO_MSS_CLAMP_IO_COM_COMP_SHFT                                            0x2
#define HWIO_MSS_CLAMP_IO_NC_HM_BMSK                                               0x2
#define HWIO_MSS_CLAMP_IO_NC_HM_SHFT                                               0x1
#define HWIO_MSS_CLAMP_IO_MODEM_BMSK                                               0x1
#define HWIO_MSS_CLAMP_IO_MODEM_SHFT                                               0x0

#define HWIO_MSS_BUS_AHB2AHB_CFG_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000000c)
#define HWIO_MSS_BUS_AHB2AHB_CFG_RMSK                                              0x3
#define HWIO_MSS_BUS_AHB2AHB_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR, HWIO_MSS_BUS_AHB2AHB_CFG_RMSK)
#define HWIO_MSS_BUS_AHB2AHB_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR, m)
#define HWIO_MSS_BUS_AHB2AHB_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR,v)
#define HWIO_MSS_BUS_AHB2AHB_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_AHB2AHB_CFG_ADDR,m,v,HWIO_MSS_BUS_AHB2AHB_CFG_IN)
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_NAV_BMSK                          0x2
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_NAV_SHFT                          0x1
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_BMSK                              0x1
#define HWIO_MSS_BUS_AHB2AHB_CFG_POST_EN_AHB2AHB_SHFT                              0x0

#define HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00000010)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK                                            0xff
#define HWIO_MSS_BUS_MAXI2AXI_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, m)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,v)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,m,v,HWIO_MSS_BUS_MAXI2AXI_CFG_IN)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_Q6_CP_AREQPRIORITY_BMSK                   0xc0
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_Q6_CP_AREQPRIORITY_SHFT                    0x6
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_Q6_AREQPRIORITY_BMSK                      0x30
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_Q6_AREQPRIORITY_SHFT                       0x4
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_BMSK                      0xc
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_SHFT                      0x2
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_BMSK                   0x3
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_SHFT                   0x0

#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000014)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK                                         0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN          \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_INM(m)      \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, m)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUT(v)      \
        out_dword(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,v)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,m,v,HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_BMSK                                     0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_SHFT                                      0x0

#define HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00000018)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_RMSK                                           0x3
#define HWIO_MSS_ANALOG_IP_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR, HWIO_MSS_ANALOG_IP_TEST_CTL_RMSK)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR, m)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR,v)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ANALOG_IP_TEST_CTL_ADDR,m,v,HWIO_MSS_ANALOG_IP_TEST_CTL_IN)
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_IQDATA_EN_BMSK                        0x2
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_IQDATA_EN_SHFT                        0x1
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_Y1Y2_EN_BMSK                          0x1
#define HWIO_MSS_ANALOG_IP_TEST_CTL_EXTERNAL_Y1Y2_EN_SHFT                          0x0

#define HWIO_MSS_ATB_ID_ADDR                                                (MSS_PERPH_REG_BASE      + 0x0000001c)
#define HWIO_MSS_ATB_ID_RMSK                                                      0x7f
#define HWIO_MSS_ATB_ID_IN          \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, HWIO_MSS_ATB_ID_RMSK)
#define HWIO_MSS_ATB_ID_INM(m)      \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, m)
#define HWIO_MSS_ATB_ID_OUT(v)      \
        out_dword(HWIO_MSS_ATB_ID_ADDR,v)
#define HWIO_MSS_ATB_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ATB_ID_ADDR,m,v,HWIO_MSS_ATB_ID_IN)
#define HWIO_MSS_ATB_ID_ATB_ID_BMSK                                               0x7f
#define HWIO_MSS_ATB_ID_ATB_ID_SHFT                                                0x0

#define HWIO_MSS_DBG_BUS_CTL_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00000020)
#define HWIO_MSS_DBG_BUS_CTL_RMSK                                             0x7fffff
#define HWIO_MSS_DBG_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, HWIO_MSS_DBG_BUS_CTL_RMSK)
#define HWIO_MSS_DBG_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, m)
#define HWIO_MSS_DBG_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DBG_BUS_CTL_ADDR,v)
#define HWIO_MSS_DBG_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DBG_BUS_CTL_ADDR,m,v,HWIO_MSS_DBG_BUS_CTL_IN)
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_BIMC_SEL_BMSK                       0x780000
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_BIMC_SEL_SHFT                           0x13
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_SEL_BMSK                         0x70000
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_SEL_SHFT                            0x10
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_BMSK                               0xe000
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_SHFT                                  0xd
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_NAV_CONFIG_SEL_BMSK                        0x1800
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_NAV_CONFIG_SEL_SHFT                           0xb
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_BMSK                                    0x600
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_SHFT                                      0x9
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_BMSK                            0x1e0
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_SHFT                              0x5
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_NC_SEL_BMSK                              0x1c
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_NC_SEL_SHFT                               0x2
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_BMSK                             0x3
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_SHFT                             0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR                                 (MSS_PERPH_REG_BASE      + 0x00000024)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK                                        0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,m,v,HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_BMSK                                     0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_SHFT                                     0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR                             (MSS_PERPH_REG_BASE      + 0x00000028)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK                                    0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_BMSK                             0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_SHFT                             0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR                                (MSS_PERPH_REG_BASE      + 0x0000002c)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_RMSK                                       0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_BMSK                                   0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_SHFT                                   0x0

#define HWIO_MSS_BUS_CTL_CFG_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00000030)
#define HWIO_MSS_BUS_CTL_CFG_RMSK                                                  0x1
#define HWIO_MSS_BUS_CTL_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_CTL_CFG_ADDR, HWIO_MSS_BUS_CTL_CFG_RMSK)
#define HWIO_MSS_BUS_CTL_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CTL_CFG_ADDR, m)
#define HWIO_MSS_BUS_CTL_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CTL_CFG_ADDR,v)
#define HWIO_MSS_BUS_CTL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CTL_CFG_ADDR,m,v,HWIO_MSS_BUS_CTL_CFG_IN)
#define HWIO_MSS_BUS_CTL_CFG_Q6_FORCE_UNBUFFERED_BMSK                              0x1
#define HWIO_MSS_BUS_CTL_CFG_Q6_FORCE_UNBUFFERED_SHFT                              0x0

#define HWIO_MSS_MSA_ADDR                                                   (MSS_PERPH_REG_BASE      + 0x00000058)
#define HWIO_MSS_MSA_RMSK                                                          0x7
#define HWIO_MSS_MSA_IN          \
        in_dword_masked(HWIO_MSS_MSA_ADDR, HWIO_MSS_MSA_RMSK)
#define HWIO_MSS_MSA_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_ADDR, m)
#define HWIO_MSS_MSA_OUT(v)      \
        out_dword(HWIO_MSS_MSA_ADDR,v)
#define HWIO_MSS_MSA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_ADDR,m,v,HWIO_MSS_MSA_IN)
#define HWIO_MSS_MSA_FORCE_Q6_MSA_BMSK                                             0x4
#define HWIO_MSS_MSA_FORCE_Q6_MSA_SHFT                                             0x2
#define HWIO_MSS_MSA_MBA_OK_BMSK                                                   0x2
#define HWIO_MSS_MSA_MBA_OK_SHFT                                                   0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_BMSK                                              0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_SHFT                                              0x0

#define HWIO_MSS_HW_VERSION_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000005c)
#define HWIO_MSS_HW_VERSION_RMSK                                            0xffffffff
#define HWIO_MSS_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, HWIO_MSS_HW_VERSION_RMSK)
#define HWIO_MSS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, m)
#define HWIO_MSS_HW_VERSION_MAJOR_BMSK                                      0xf0000000
#define HWIO_MSS_HW_VERSION_MAJOR_SHFT                                            0x1c
#define HWIO_MSS_HW_VERSION_MINOR_BMSK                                       0xfff0000
#define HWIO_MSS_HW_VERSION_MINOR_SHFT                                            0x10
#define HWIO_MSS_HW_VERSION_STEP_BMSK                                           0xffff
#define HWIO_MSS_HW_VERSION_STEP_SHFT                                              0x0

#define HWIO_MSS_MODEM_MEM_SLP_CNTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00000060)
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_RMSK                                       0x20f00
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_IN          \
        in_dword_masked(HWIO_MSS_MODEM_MEM_SLP_CNTL_ADDR, HWIO_MSS_MODEM_MEM_SLP_CNTL_RMSK)
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_MEM_SLP_CNTL_ADDR, m)
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_OUT(v)      \
        out_dword(HWIO_MSS_MODEM_MEM_SLP_CNTL_ADDR,v)
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MODEM_MEM_SLP_CNTL_ADDR,m,v,HWIO_MSS_MODEM_MEM_SLP_CNTL_IN)
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_MODEM_NR_SLP_NRET_N_BMSK                   0x20000
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_MODEM_NR_SLP_NRET_N_SHFT                      0x11
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_RFC_SLP_NRET_N_BMSK                          0x800
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_RFC_SLP_NRET_N_SHFT                            0xb
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_RFC_SLP_RET_N_BMSK                           0x400
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_RFC_SLP_RET_N_SHFT                             0xa
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_CCS_SLP_NRET_N_BMSK                          0x200
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_CCS_SLP_NRET_N_SHFT                            0x9
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_CCS_SLP_RET_N_BMSK                           0x100
#define HWIO_MSS_MODEM_MEM_SLP_CNTL_CCS_SLP_RET_N_SHFT                             0x8

#define HWIO_MSS_CLOCK_SPDM_MON_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00000064)
#define HWIO_MSS_CLOCK_SPDM_MON_RMSK                                               0x3
#define HWIO_MSS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, HWIO_MSS_CLOCK_SPDM_MON_RMSK)
#define HWIO_MSS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_MSS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_MSS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_MSS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_MSS_CLOCK_SPDM_MON_IN)
#define HWIO_MSS_CLOCK_SPDM_MON_Q6_MON_CLKEN_BMSK                                  0x2
#define HWIO_MSS_CLOCK_SPDM_MON_Q6_MON_CLKEN_SHFT                                  0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_BMSK                                 0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_SHFT                                 0x0

#define HWIO_MSS_BBRX0_MUX_SEL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000068)
#define HWIO_MSS_BBRX0_MUX_SEL_RMSK                                                0x3
#define HWIO_MSS_BBRX0_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_MUX_SEL_ADDR, HWIO_MSS_BBRX0_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX0_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX0_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX0_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX0_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX0_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX0_MUX_SEL_IN)
#define HWIO_MSS_BBRX0_MUX_SEL_SECOND_MUX_SEL_BMSK                                 0x2
#define HWIO_MSS_BBRX0_MUX_SEL_SECOND_MUX_SEL_SHFT                                 0x1
#define HWIO_MSS_BBRX0_MUX_SEL_FIRST_MUX_SEL_BMSK                                  0x1
#define HWIO_MSS_BBRX0_MUX_SEL_FIRST_MUX_SEL_SHFT                                  0x0

#define HWIO_MSS_BBRX1_MUX_SEL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x0000006c)
#define HWIO_MSS_BBRX1_MUX_SEL_RMSK                                                0x3
#define HWIO_MSS_BBRX1_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_MUX_SEL_ADDR, HWIO_MSS_BBRX1_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX1_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX1_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX1_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX1_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX1_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX1_MUX_SEL_IN)
#define HWIO_MSS_BBRX1_MUX_SEL_SECOND_MUX_SEL_BMSK                                 0x2
#define HWIO_MSS_BBRX1_MUX_SEL_SECOND_MUX_SEL_SHFT                                 0x1
#define HWIO_MSS_BBRX1_MUX_SEL_FIRST_MUX_SEL_BMSK                                  0x1
#define HWIO_MSS_BBRX1_MUX_SEL_FIRST_MUX_SEL_SHFT                                  0x0

#define HWIO_MSS_BBRX2_MUX_SEL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000070)
#define HWIO_MSS_BBRX2_MUX_SEL_RMSK                                                0x3
#define HWIO_MSS_BBRX2_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX2_MUX_SEL_ADDR, HWIO_MSS_BBRX2_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX2_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX2_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX2_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX2_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX2_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX2_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX2_MUX_SEL_IN)
#define HWIO_MSS_BBRX2_MUX_SEL_SECOND_MUX_SEL_BMSK                                 0x2
#define HWIO_MSS_BBRX2_MUX_SEL_SECOND_MUX_SEL_SHFT                                 0x1
#define HWIO_MSS_BBRX2_MUX_SEL_FIRST_MUX_SEL_BMSK                                  0x1
#define HWIO_MSS_BBRX2_MUX_SEL_FIRST_MUX_SEL_SHFT                                  0x0

#define HWIO_MSS_BBRX3_MUX_SEL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000074)
#define HWIO_MSS_BBRX3_MUX_SEL_RMSK                                                0x3
#define HWIO_MSS_BBRX3_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX3_MUX_SEL_ADDR, HWIO_MSS_BBRX3_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX3_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX3_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX3_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX3_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX3_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX3_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX3_MUX_SEL_IN)
#define HWIO_MSS_BBRX3_MUX_SEL_SECOND_MUX_SEL_BMSK                                 0x2
#define HWIO_MSS_BBRX3_MUX_SEL_SECOND_MUX_SEL_SHFT                                 0x1
#define HWIO_MSS_BBRX3_MUX_SEL_FIRST_MUX_SEL_BMSK                                  0x1
#define HWIO_MSS_BBRX3_MUX_SEL_FIRST_MUX_SEL_SHFT                                  0x0

#define HWIO_MSS_DEBUG_CLOCK_CTL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000078)
#define HWIO_MSS_DEBUG_CLOCK_CTL_RMSK                                             0x3f
#define HWIO_MSS_DEBUG_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, HWIO_MSS_DEBUG_CLOCK_CTL_RMSK)
#define HWIO_MSS_DEBUG_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CLOCK_CTL_IN)
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_MUX_SEL_BMSK                                 0x20
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_MUX_SEL_SHFT                                  0x5
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_BMSK                          0x10
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_SHFT                           0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_BMSK                           0x8
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_SHFT                           0x3
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_BMSK                           0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_SHFT                           0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_BMSK                           0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_SHFT                           0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_BMSK                           0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_SHFT                           0x0

#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR                                (MSS_PERPH_REG_BASE      + 0x0000007c)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_RMSK                                       0x3
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_IN          \
        in_dword_masked(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR, HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_RMSK)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR, m)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR,v)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_ADDR,m,v,HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_IN)
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_PLLTEST_BMSK                               0x2
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_PLLTEST_SHFT                               0x1
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_EXT_CLOCK_MUX_SEL_BMSK                     0x1
#define HWIO_MSS_BBRX_EXT_CLOCK_MUX_SEL_EXT_CLOCK_MUX_SEL_SHFT                     0x0

#define HWIO_MSS_BBRX_CTL_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00000094)
#define HWIO_MSS_BBRX_CTL_RMSK                                                     0x3
#define HWIO_MSS_BBRX_CTL_IN          \
        in_dword_masked(HWIO_MSS_BBRX_CTL_ADDR, HWIO_MSS_BBRX_CTL_RMSK)
#define HWIO_MSS_BBRX_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX_CTL_ADDR, m)
#define HWIO_MSS_BBRX_CTL_OUT(v)      \
        out_dword(HWIO_MSS_BBRX_CTL_ADDR,v)
#define HWIO_MSS_BBRX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX_CTL_ADDR,m,v,HWIO_MSS_BBRX_CTL_IN)
#define HWIO_MSS_BBRX_CTL_BBRX_HS_TEST_MUX_CTL_BMSK                                0x3
#define HWIO_MSS_BBRX_CTL_BBRX_HS_TEST_MUX_CTL_SHFT                                0x0

#define HWIO_MSS_DEBUG_CTL_ADDR                                             (MSS_PERPH_REG_BASE      + 0x00000098)
#define HWIO_MSS_DEBUG_CTL_RMSK                                                    0x7
#define HWIO_MSS_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, HWIO_MSS_DEBUG_CTL_RMSK)
#define HWIO_MSS_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CTL_IN)
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_BMSK                              0x4
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_SHFT                              0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_BMSK                                0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_SHFT                                0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_BMSK                               0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_SHFT                               0x0

#define HWIO_MSS_POWER_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x0000009c)
#define HWIO_MSS_POWER_RMSK                                                 0xffffffff
#define HWIO_MSS_POWER_IN          \
        in_dword_masked(HWIO_MSS_POWER_ADDR, HWIO_MSS_POWER_RMSK)
#define HWIO_MSS_POWER_INM(m)      \
        in_dword_masked(HWIO_MSS_POWER_ADDR, m)
#define HWIO_MSS_POWER_OUT(v)      \
        out_dword(HWIO_MSS_POWER_ADDR,v)
#define HWIO_MSS_POWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_POWER_ADDR,m,v,HWIO_MSS_POWER_IN)
#define HWIO_MSS_POWER_RESERVE_31_17_BMSK                                   0xfffe0000
#define HWIO_MSS_POWER_RESERVE_31_17_SHFT                                         0x11
#define HWIO_MSS_POWER_BHS_ON_ST_BMSK                                          0x10000
#define HWIO_MSS_POWER_BHS_ON_ST_SHFT                                             0x10
#define HWIO_MSS_POWER_RESERVE_15_1_BMSK                                        0xfffe
#define HWIO_MSS_POWER_RESERVE_15_1_SHFT                                           0x1
#define HWIO_MSS_POWER_BHS_ON_BMSK                                                 0x1
#define HWIO_MSS_POWER_BHS_ON_SHFT                                                 0x0

#define HWIO_MSS_MSA_NC_HM_ADDR                                             (MSS_PERPH_REG_BASE      + 0x000000a0)
#define HWIO_MSS_MSA_NC_HM_RMSK                                             0xffffffff
#define HWIO_MSS_MSA_NC_HM_IN          \
        in_dword_masked(HWIO_MSS_MSA_NC_HM_ADDR, HWIO_MSS_MSA_NC_HM_RMSK)
#define HWIO_MSS_MSA_NC_HM_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_NC_HM_ADDR, m)
#define HWIO_MSS_MSA_NC_HM_OUT(v)      \
        out_dword(HWIO_MSS_MSA_NC_HM_ADDR,v)
#define HWIO_MSS_MSA_NC_HM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_NC_HM_ADDR,m,v,HWIO_MSS_MSA_NC_HM_IN)
#define HWIO_MSS_MSA_NC_HM_RESERVE_31_9_BMSK                                0xfffffe00
#define HWIO_MSS_MSA_NC_HM_RESERVE_31_9_SHFT                                       0x9
#define HWIO_MSS_MSA_NC_HM_NAV_BMSK                                              0x100
#define HWIO_MSS_MSA_NC_HM_NAV_SHFT                                                0x8
#define HWIO_MSS_MSA_NC_HM_RESERVE_7_4_BMSK                                       0xf0
#define HWIO_MSS_MSA_NC_HM_RESERVE_7_4_SHFT                                        0x4
#define HWIO_MSS_MSA_NC_HM_CRYPTO_BMSK                                             0xf
#define HWIO_MSS_MSA_NC_HM_CRYPTO_SHFT                                             0x0

#define HWIO_MSS_CXM_ADDR                                                   (MSS_PERPH_REG_BASE      + 0x000000a4)
#define HWIO_MSS_CXM_RMSK                                                   0xffffffff
#define HWIO_MSS_CXM_IN          \
        in_dword_masked(HWIO_MSS_CXM_ADDR, HWIO_MSS_CXM_RMSK)
#define HWIO_MSS_CXM_INM(m)      \
        in_dword_masked(HWIO_MSS_CXM_ADDR, m)
#define HWIO_MSS_CXM_OUT(v)      \
        out_dword(HWIO_MSS_CXM_ADDR,v)
#define HWIO_MSS_CXM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CXM_ADDR,m,v,HWIO_MSS_CXM_IN)
#define HWIO_MSS_CXM_RESERVE_31_12_BMSK                                     0xfffff000
#define HWIO_MSS_CXM_RESERVE_31_12_SHFT                                            0xc
#define HWIO_MSS_CXM_USE_EXTERNAL_WLAN_BMSK                                      0x800
#define HWIO_MSS_CXM_USE_EXTERNAL_WLAN_SHFT                                        0xb
#define HWIO_MSS_CXM_BT_TX_ACT_LST_SUBFRM_BMSK                                   0x400
#define HWIO_MSS_CXM_BT_TX_ACT_LST_SUBFRM_SHFT                                     0xa
#define HWIO_MSS_CXM_WLAN_TX_ACT_LST_SUBFRM_BMSK                                 0x200
#define HWIO_MSS_CXM_WLAN_TX_ACT_LST_SUBFRM_SHFT                                   0x9
#define HWIO_MSS_CXM_BT_TX_S_WAN_BMSK                                            0x100
#define HWIO_MSS_CXM_BT_TX_S_WAN_SHFT                                              0x8
#define HWIO_MSS_CXM_WLAN_TX_S_WAN_BMSK                                           0x80
#define HWIO_MSS_CXM_WLAN_TX_S_WAN_SHFT                                            0x7
#define HWIO_MSS_CXM_BT_TX_REGISTERED_BMSK                                        0x40
#define HWIO_MSS_CXM_BT_TX_REGISTERED_SHFT                                         0x6
#define HWIO_MSS_CXM_WLAN_TX_REGISTERED_BMSK                                      0x20
#define HWIO_MSS_CXM_WLAN_TX_REGISTERED_SHFT                                       0x5
#define HWIO_MSS_CXM_WLAN_TX_STICKY_BMSK                                          0x10
#define HWIO_MSS_CXM_WLAN_TX_STICKY_SHFT                                           0x4
#define HWIO_MSS_CXM_WLAN_TX_STICKY_CLR_BMSK                                       0x8
#define HWIO_MSS_CXM_WLAN_TX_STICKY_CLR_SHFT                                       0x3
#define HWIO_MSS_CXM_CXM_WAKEUP_CLR_BMSK                                           0x4
#define HWIO_MSS_CXM_CXM_WAKEUP_CLR_SHFT                                           0x2
#define HWIO_MSS_CXM_BT_TX_ON_BLANKING_EN_BMSK                                     0x2
#define HWIO_MSS_CXM_BT_TX_ON_BLANKING_EN_SHFT                                     0x1
#define HWIO_MSS_CXM_WLAN_TX_ON_BLANKING_EN_BMSK                                   0x1
#define HWIO_MSS_CXM_WLAN_TX_ON_BLANKING_EN_SHFT                                   0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_00_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000400)
#define HWIO_MSS_RELAY_MSG_SHADOW_00_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_00_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_00_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_00_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_00_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_00_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_00_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_00_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_00_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_00_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_00_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_00_RELAY_MSG_SHADOW_DATA_00_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_00_RELAY_MSG_SHADOW_DATA_00_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_01_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000404)
#define HWIO_MSS_RELAY_MSG_SHADOW_01_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_01_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_01_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_01_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_01_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_01_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_01_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_01_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_01_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_01_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_01_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_01_RELAY_MSG_SHADOW_DATA_01_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_01_RELAY_MSG_SHADOW_DATA_01_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_02_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000408)
#define HWIO_MSS_RELAY_MSG_SHADOW_02_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_02_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_02_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_02_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_02_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_02_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_02_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_02_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_02_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_02_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_02_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_02_RELAY_MSG_SHADOW_DATA_02_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_02_RELAY_MSG_SHADOW_DATA_02_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_03_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000040c)
#define HWIO_MSS_RELAY_MSG_SHADOW_03_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_03_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_03_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_03_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_03_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_03_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_03_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_03_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_03_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_03_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_03_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_03_RELAY_MSG_SHADOW_DATA_03_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_03_RELAY_MSG_SHADOW_DATA_03_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_04_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000410)
#define HWIO_MSS_RELAY_MSG_SHADOW_04_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_04_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_04_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_04_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_04_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_04_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_04_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_04_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_04_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_04_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_04_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_04_RELAY_MSG_SHADOW_DATA_04_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_04_RELAY_MSG_SHADOW_DATA_04_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_05_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000414)
#define HWIO_MSS_RELAY_MSG_SHADOW_05_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_05_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_05_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_05_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_05_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_05_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_05_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_05_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_05_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_05_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_05_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_05_RELAY_MSG_SHADOW_DATA_05_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_05_RELAY_MSG_SHADOW_DATA_05_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_06_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000418)
#define HWIO_MSS_RELAY_MSG_SHADOW_06_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_06_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_06_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_06_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_06_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_06_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_06_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_06_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_06_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_06_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_06_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_06_RELAY_MSG_SHADOW_DATA_06_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_06_RELAY_MSG_SHADOW_DATA_06_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_07_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000041c)
#define HWIO_MSS_RELAY_MSG_SHADOW_07_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_07_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_07_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_07_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_07_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_07_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_07_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_07_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_07_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_07_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_07_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_07_RELAY_MSG_SHADOW_DATA_07_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_07_RELAY_MSG_SHADOW_DATA_07_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_08_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000420)
#define HWIO_MSS_RELAY_MSG_SHADOW_08_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_08_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_08_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_08_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_08_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_08_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_08_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_08_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_08_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_08_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_08_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_08_RELAY_MSG_SHADOW_DATA_08_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_08_RELAY_MSG_SHADOW_DATA_08_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_09_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000424)
#define HWIO_MSS_RELAY_MSG_SHADOW_09_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_09_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_09_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_09_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_09_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_09_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_09_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_09_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_09_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_09_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_09_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_09_RELAY_MSG_SHADOW_DATA_09_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_09_RELAY_MSG_SHADOW_DATA_09_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_10_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000428)
#define HWIO_MSS_RELAY_MSG_SHADOW_10_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_10_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_10_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_10_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_10_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_10_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_10_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_10_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_10_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_10_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_10_RELAY_MSG_SHADOW_DATA_10_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_10_RELAY_MSG_SHADOW_DATA_10_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_11_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000042c)
#define HWIO_MSS_RELAY_MSG_SHADOW_11_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_11_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_11_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_11_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_11_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_11_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_11_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_11_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_11_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_11_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_11_RELAY_MSG_SHADOW_DATA_11_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_11_RELAY_MSG_SHADOW_DATA_11_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_12_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000430)
#define HWIO_MSS_RELAY_MSG_SHADOW_12_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_12_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_12_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_12_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_12_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_12_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_12_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_12_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_12_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_12_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_12_RELAY_MSG_SHADOW_DATA_12_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_12_RELAY_MSG_SHADOW_DATA_12_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_13_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000434)
#define HWIO_MSS_RELAY_MSG_SHADOW_13_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_13_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_13_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_13_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_13_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_13_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_13_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_13_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_13_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_13_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_13_RELAY_MSG_SHADOW_DATA_13_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_13_RELAY_MSG_SHADOW_DATA_13_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_14_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000438)
#define HWIO_MSS_RELAY_MSG_SHADOW_14_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_14_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_14_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_14_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_14_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_14_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_14_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_14_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_14_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_14_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_14_RELAY_MSG_SHADOW_DATA_14_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_14_RELAY_MSG_SHADOW_DATA_14_SHFT                 0x0

#define HWIO_MSS_RELAY_MSG_SHADOW_15_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000043c)
#define HWIO_MSS_RELAY_MSG_SHADOW_15_RMSK                                   0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_15_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_15_ADDR, HWIO_MSS_RELAY_MSG_SHADOW_15_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW_15_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW_15_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW_15_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW_15_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW_15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW_15_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW_15_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW_15_RELAY_MSG_SHADOW_DATA_15_BMSK          0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW_15_RELAY_MSG_SHADOW_DATA_15_SHFT                 0x0

#define HWIO_MSS_MPLL1_MODE_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001020)
#define HWIO_MSS_MPLL1_MODE_RMSK                                              0x3fffff
#define HWIO_MSS_MPLL1_MODE_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MODE_ADDR, HWIO_MSS_MPLL1_MODE_RMSK)
#define HWIO_MSS_MPLL1_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MODE_ADDR, m)
#define HWIO_MSS_MPLL1_MODE_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_MODE_ADDR,v)
#define HWIO_MSS_MPLL1_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_MODE_ADDR,m,v,HWIO_MSS_MPLL1_MODE_IN)
#define HWIO_MSS_MPLL1_MODE_RESERVE_21_4_BMSK                                 0x3ffff0
#define HWIO_MSS_MPLL1_MODE_RESERVE_21_4_SHFT                                      0x4
#define HWIO_MSS_MPLL1_MODE_PLL_PLLTEST_BMSK                                       0x8
#define HWIO_MSS_MPLL1_MODE_PLL_PLLTEST_SHFT                                       0x3
#define HWIO_MSS_MPLL1_MODE_PLL_RESET_N_BMSK                                       0x4
#define HWIO_MSS_MPLL1_MODE_PLL_RESET_N_SHFT                                       0x2
#define HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_BMSK                                      0x2
#define HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_SHFT                                      0x1
#define HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_BMSK                                       0x1
#define HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_SHFT                                       0x0

#define HWIO_MSS_MPLL1_L_VAL_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001024)
#define HWIO_MSS_MPLL1_L_VAL_RMSK                                                 0x7f
#define HWIO_MSS_MPLL1_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_L_VAL_ADDR, HWIO_MSS_MPLL1_L_VAL_RMSK)
#define HWIO_MSS_MPLL1_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_L_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_L_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_L_VAL_ADDR,m,v,HWIO_MSS_MPLL1_L_VAL_IN)
#define HWIO_MSS_MPLL1_L_VAL_PLL_L_BMSK                                           0x7f
#define HWIO_MSS_MPLL1_L_VAL_PLL_L_SHFT                                            0x0

#define HWIO_MSS_MPLL1_M_VAL_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001028)
#define HWIO_MSS_MPLL1_M_VAL_RMSK                                              0x7ffff
#define HWIO_MSS_MPLL1_M_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_M_VAL_ADDR, HWIO_MSS_MPLL1_M_VAL_RMSK)
#define HWIO_MSS_MPLL1_M_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_M_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_M_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_M_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_M_VAL_ADDR,m,v,HWIO_MSS_MPLL1_M_VAL_IN)
#define HWIO_MSS_MPLL1_M_VAL_PLL_M_BMSK                                        0x7ffff
#define HWIO_MSS_MPLL1_M_VAL_PLL_M_SHFT                                            0x0

#define HWIO_MSS_MPLL1_N_VAL_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000102c)
#define HWIO_MSS_MPLL1_N_VAL_RMSK                                              0x7ffff
#define HWIO_MSS_MPLL1_N_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_N_VAL_ADDR, HWIO_MSS_MPLL1_N_VAL_RMSK)
#define HWIO_MSS_MPLL1_N_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_N_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_N_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_N_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_N_VAL_ADDR,m,v,HWIO_MSS_MPLL1_N_VAL_IN)
#define HWIO_MSS_MPLL1_N_VAL_PLL_N_BMSK                                        0x7ffff
#define HWIO_MSS_MPLL1_N_VAL_PLL_N_SHFT                                            0x0

#define HWIO_MSS_MPLL1_USER_CTL_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001030)
#define HWIO_MSS_MPLL1_USER_CTL_RMSK                                        0xffffffff
#define HWIO_MSS_MPLL1_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_USER_CTL_ADDR, HWIO_MSS_MPLL1_USER_CTL_RMSK)
#define HWIO_MSS_MPLL1_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_USER_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_USER_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_USER_CTL_ADDR,m,v,HWIO_MSS_MPLL1_USER_CTL_IN)
#define HWIO_MSS_MPLL1_USER_CTL_ADDITIVE_FACTOR_BMSK                        0xc0000000
#define HWIO_MSS_MPLL1_USER_CTL_ADDITIVE_FACTOR_SHFT                              0x1e
#define HWIO_MSS_MPLL1_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_BMSK            0x30000000
#define HWIO_MSS_MPLL1_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_SHFT                  0x1c
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_27_25_BMSK                       0xe000000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_27_25_SHFT                            0x19
#define HWIO_MSS_MPLL1_USER_CTL_MN_EN_BMSK                                   0x1000000
#define HWIO_MSS_MPLL1_USER_CTL_MN_EN_SHFT                                        0x18
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_23_13_BMSK                       0xffe000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_23_13_SHFT                            0xd
#define HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_BMSK                                 0x1000
#define HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_SHFT                                    0xc
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_11_10_BMSK                          0xc00
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_11_10_SHFT                            0xa
#define HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_BMSK                                 0x300
#define HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_SHFT                                   0x8
#define HWIO_MSS_MPLL1_USER_CTL_INV_OUTPUT_BMSK                                   0x80
#define HWIO_MSS_MPLL1_USER_CTL_INV_OUTPUT_SHFT                                    0x7
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_6_5_BMSK                              0x60
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_6_5_SHFT                               0x5
#define HWIO_MSS_MPLL1_USER_CTL_LVTEST_EN_BMSK                                    0x10
#define HWIO_MSS_MPLL1_USER_CTL_LVTEST_EN_SHFT                                     0x4
#define HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_BMSK                                    0x8
#define HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_SHFT                                    0x3
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX2_EN_BMSK                                     0x4
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX2_EN_SHFT                                     0x2
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_BMSK                                      0x2
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_SHFT                                      0x1
#define HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_BMSK                                     0x1
#define HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_SHFT                                     0x0

#define HWIO_MSS_MPLL1_CONFIG_CTL_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_MPLL1_CONFIG_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_MPLL1_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, HWIO_MSS_MPLL1_CONFIG_CTL_RMSK)
#define HWIO_MSS_MPLL1_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,m,v,HWIO_MSS_MPLL1_CONFIG_CTL_IN)
#define HWIO_MSS_MPLL1_CONFIG_CTL_MULTIPLICATIONFACTOR_BMSK                 0xc0000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_MULTIPLICATIONFACTOR_SHFT                       0x1e
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_BIT_29_27_BMSK                    0x38000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_BIT_29_27_SHFT                          0x1b
#define HWIO_MSS_MPLL1_CONFIG_CTL_OVDREFCFG_BMSK                             0x6000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_OVDREFCFG_SHFT                                  0x19
#define HWIO_MSS_MPLL1_CONFIG_CTL_RING_OSC_LATCH_EN_BMSK                     0x1000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RING_OSC_LATCH_EN_SHFT                          0x18
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_BIT_23_18_BMSK                      0xfc0000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_BIT_23_18_SHFT                          0x12
#define HWIO_MSS_MPLL1_CONFIG_CTL_PDN_OVDCNT_BMSK                              0x20000
#define HWIO_MSS_MPLL1_CONFIG_CTL_PDN_OVDCNT_SHFT                                 0x11
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_OVDCNT_BMSK                              0x18000
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_OVDCNT_SHFT                                  0xf
#define HWIO_MSS_MPLL1_CONFIG_CTL_RVSIG_DEL_BMSK                                0x6000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RVSIG_DEL_SHFT                                   0xd
#define HWIO_MSS_MPLL1_CONFIG_CTL_PFD_DZSEL_BMSK                                0x1800
#define HWIO_MSS_MPLL1_CONFIG_CTL_PFD_DZSEL_SHFT                                   0xb
#define HWIO_MSS_MPLL1_CONFIG_CTL_FORCE_ISEED_BMSK                               0x400
#define HWIO_MSS_MPLL1_CONFIG_CTL_FORCE_ISEED_SHFT                                 0xa
#define HWIO_MSS_MPLL1_CONFIG_CTL_ICP_SEED_SEL_BMSK                              0x300
#define HWIO_MSS_MPLL1_CONFIG_CTL_ICP_SEED_SEL_SHFT                                0x8
#define HWIO_MSS_MPLL1_CONFIG_CTL_SEL_IREG_REP_BMSK                               0xc0
#define HWIO_MSS_MPLL1_CONFIG_CTL_SEL_IREG_REP_SHFT                                0x6
#define HWIO_MSS_MPLL1_CONFIG_CTL_SEL_IREG_OSC_BMSK                               0x30
#define HWIO_MSS_MPLL1_CONFIG_CTL_SEL_IREG_OSC_SHFT                                0x4
#define HWIO_MSS_MPLL1_CONFIG_CTL_CAPSEL_BMSK                                      0xc
#define HWIO_MSS_MPLL1_CONFIG_CTL_CAPSEL_SHFT                                      0x2
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_LOCKDET_BMSK                                 0x3
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_LOCKDET_SHFT                                 0x0

#define HWIO_MSS_MPLL1_TEST_CTL_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001038)
#define HWIO_MSS_MPLL1_TEST_CTL_RMSK                                        0xffffffff
#define HWIO_MSS_MPLL1_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_TEST_CTL_ADDR, HWIO_MSS_MPLL1_TEST_CTL_RMSK)
#define HWIO_MSS_MPLL1_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_TEST_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_TEST_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_TEST_CTL_ADDR,m,v,HWIO_MSS_MPLL1_TEST_CTL_IN)
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_31_23_BMSK                          0xff800000
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_31_23_SHFT                                0x17
#define HWIO_MSS_MPLL1_TEST_CTL_RDAC_CURRENT_BMSK                             0x700000
#define HWIO_MSS_MPLL1_TEST_CTL_RDAC_CURRENT_SHFT                                 0x14
#define HWIO_MSS_MPLL1_TEST_CTL_NOISE_GEN_EN_BMSK                              0x80000
#define HWIO_MSS_MPLL1_TEST_CTL_NOISE_GEN_EN_SHFT                                 0x13
#define HWIO_MSS_MPLL1_TEST_CTL_NMO_FREQ_BMSK                                  0x60000
#define HWIO_MSS_MPLL1_TEST_CTL_NMO_FREQ_SHFT                                     0x11
#define HWIO_MSS_MPLL1_TEST_CTL_NMO_OUT_EN_BMSK                                0x10000
#define HWIO_MSS_MPLL1_TEST_CTL_NMO_OUT_EN_SHFT                                   0x10
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_15_10_BMSK                              0xfc00
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_15_10_SHFT                                 0xa
#define HWIO_MSS_MPLL1_TEST_CTL_IEXT_SEL_BMSK                                    0x200
#define HWIO_MSS_MPLL1_TEST_CTL_IEXT_SEL_SHFT                                      0x9
#define HWIO_MSS_MPLL1_TEST_CTL_DTEST_SEL_BMSK                                   0x180
#define HWIO_MSS_MPLL1_TEST_CTL_DTEST_SEL_SHFT                                     0x7
#define HWIO_MSS_MPLL1_TEST_CTL_BYP_TESTAMP_BMSK                                  0x40
#define HWIO_MSS_MPLL1_TEST_CTL_BYP_TESTAMP_SHFT                                   0x6
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_SEL_BMSK                                   0x30
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_SEL_SHFT                                    0x4
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_SEL_BMSK                                    0xc
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_SEL_SHFT                                    0x2
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_EN_BMSK                                     0x2
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_EN_SHFT                                     0x1
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_EN_BMSK                                     0x1
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_EN_SHFT                                     0x0

#define HWIO_MSS_MPLL1_STATUS_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000103c)
#define HWIO_MSS_MPLL1_STATUS_RMSK                                             0x3ffff
#define HWIO_MSS_MPLL1_STATUS_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_STATUS_ADDR, HWIO_MSS_MPLL1_STATUS_RMSK)
#define HWIO_MSS_MPLL1_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_STATUS_ADDR, m)
#define HWIO_MSS_MPLL1_STATUS_PLL_ACTIVE_FLAG_BMSK                             0x20000
#define HWIO_MSS_MPLL1_STATUS_PLL_ACTIVE_FLAG_SHFT                                0x11
#define HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_BMSK                                0x10000
#define HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_SHFT                                   0x10
#define HWIO_MSS_MPLL1_STATUS_PLL_D_BMSK                                        0xffff
#define HWIO_MSS_MPLL1_STATUS_PLL_D_SHFT                                           0x0

#define HWIO_MSS_UIM0_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001060)
#define HWIO_MSS_UIM0_BCR_RMSK                                              0x80000001
#define HWIO_MSS_UIM0_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, HWIO_MSS_UIM0_BCR_RMSK)
#define HWIO_MSS_UIM0_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, m)
#define HWIO_MSS_UIM0_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_BCR_ADDR,v)
#define HWIO_MSS_UIM0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_BCR_ADDR,m,v,HWIO_MSS_UIM0_BCR_IN)
#define HWIO_MSS_UIM0_BCR_BUS_UIM0_SM_ARES_IN_BMSK                          0x80000000
#define HWIO_MSS_UIM0_BCR_BUS_UIM0_SM_ARES_IN_SHFT                                0x1f
#define HWIO_MSS_UIM0_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_UIM0_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_UIM1_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001064)
#define HWIO_MSS_UIM1_BCR_RMSK                                              0x80000001
#define HWIO_MSS_UIM1_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, HWIO_MSS_UIM1_BCR_RMSK)
#define HWIO_MSS_UIM1_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, m)
#define HWIO_MSS_UIM1_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_BCR_ADDR,v)
#define HWIO_MSS_UIM1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_BCR_ADDR,m,v,HWIO_MSS_UIM1_BCR_IN)
#define HWIO_MSS_UIM1_BCR_BUS_UIM1_SM_ARES_IN_BMSK                          0x80000000
#define HWIO_MSS_UIM1_BCR_BUS_UIM1_SM_ARES_IN_SHFT                                0x1f
#define HWIO_MSS_UIM1_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_UIM1_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_Q6SS_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001068)
#define HWIO_MSS_Q6SS_BCR_RMSK                                              0x80000001
#define HWIO_MSS_Q6SS_BCR_IN          \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, HWIO_MSS_Q6SS_BCR_RMSK)
#define HWIO_MSS_Q6SS_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, m)
#define HWIO_MSS_Q6SS_BCR_OUT(v)      \
        out_dword(HWIO_MSS_Q6SS_BCR_ADDR,v)
#define HWIO_MSS_Q6SS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6SS_BCR_ADDR,m,v,HWIO_MSS_Q6SS_BCR_IN)
#define HWIO_MSS_Q6SS_BCR_BUS_Q6_SM_ARES_IN_BMSK                            0x80000000
#define HWIO_MSS_Q6SS_BCR_BUS_Q6_SM_ARES_IN_SHFT                                  0x1f
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_NAV_CE_BRIDGE_BCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x0000106c)
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_RMSK                                     0x80000001
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_CE_BRIDGE_BCR_ADDR, HWIO_MSS_NAV_CE_BRIDGE_BCR_RMSK)
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_CE_BRIDGE_BCR_ADDR, m)
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_CE_BRIDGE_BCR_ADDR,v)
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_CE_BRIDGE_BCR_ADDR,m,v,HWIO_MSS_NAV_CE_BRIDGE_BCR_IN)
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_NAV_CE_BRIDGE_ARES_IN_BMSK               0x80000000
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_NAV_CE_BRIDGE_ARES_IN_SHFT                     0x1f
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_BLK_ARES_BMSK                                   0x1
#define HWIO_MSS_NAV_CE_BRIDGE_BCR_BLK_ARES_SHFT                                   0x0

#define HWIO_MSS_UIM0_CBCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x00001078)
#define HWIO_MSS_UIM0_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_CBCR_ADDR, HWIO_MSS_UIM0_CBCR_RMSK)
#define HWIO_MSS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_UIM0_CBCR_IN)
#define HWIO_MSS_UIM0_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_UIM0_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_UIM0_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_UIM0_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_UIM1_CBCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x0000107c)
#define HWIO_MSS_UIM1_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_CBCR_ADDR, HWIO_MSS_UIM1_CBCR_RMSK)
#define HWIO_MSS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_UIM1_CBCR_IN)
#define HWIO_MSS_UIM1_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_UIM1_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_UIM1_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_UIM1_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_XO_UIM0_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001080)
#define HWIO_MSS_XO_UIM0_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_XO_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_UIM0_CBCR_ADDR, HWIO_MSS_XO_UIM0_CBCR_RMSK)
#define HWIO_MSS_XO_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_XO_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_XO_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_UIM0_CBCR_ADDR,m,v,HWIO_MSS_XO_UIM0_CBCR_IN)
#define HWIO_MSS_XO_UIM0_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_XO_UIM0_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_XO_UIM0_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_XO_UIM0_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_XO_UIM1_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001084)
#define HWIO_MSS_XO_UIM1_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_XO_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_UIM1_CBCR_ADDR, HWIO_MSS_XO_UIM1_CBCR_RMSK)
#define HWIO_MSS_XO_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_XO_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_XO_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_UIM1_CBCR_ADDR,m,v,HWIO_MSS_XO_UIM1_CBCR_IN)
#define HWIO_MSS_XO_UIM1_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_XO_UIM1_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_XO_UIM1_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_XO_UIM1_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_XO_MODEM_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001088)
#define HWIO_MSS_XO_MODEM_CBCR_RMSK                                         0x80000000
#define HWIO_MSS_XO_MODEM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_MODEM_CBCR_ADDR, HWIO_MSS_XO_MODEM_CBCR_RMSK)
#define HWIO_MSS_XO_MODEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_MODEM_CBCR_ADDR, m)
#define HWIO_MSS_XO_MODEM_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_XO_MODEM_CBCR_CLKOFF_SHFT                                        0x1f

#define HWIO_MSS_BUS_UIM0_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001090)
#define HWIO_MSS_BUS_UIM0_CBCR_RMSK                                         0x80007ff1
#define HWIO_MSS_BUS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, HWIO_MSS_BUS_UIM0_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM0_CBCR_IN)
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_BMSK                           0x4000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_SHFT                              0xe
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                         0x2000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                            0xd
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                        0x1000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                           0xc
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_BMSK                                       0xf00
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_SHFT                                         0x8
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_BMSK                                         0xf0
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_SHFT                                          0x4
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BUS_UIM1_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001094)
#define HWIO_MSS_BUS_UIM1_CBCR_RMSK                                         0x80007ff1
#define HWIO_MSS_BUS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, HWIO_MSS_BUS_UIM1_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM1_CBCR_IN)
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_BMSK                           0x4000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_SHFT                              0xe
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                         0x2000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                            0xd
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                        0x1000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                           0xc
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_BMSK                                       0xf00
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_SHFT                                         0x8
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_BMSK                                         0xf0
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_SHFT                                          0x4
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BUS_CSR_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001098)
#define HWIO_MSS_BUS_CSR_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_BUS_CSR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CSR_CBCR_ADDR, HWIO_MSS_BUS_CSR_CBCR_RMSK)
#define HWIO_MSS_BUS_CSR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CSR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CSR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CSR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CSR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CSR_CBCR_ADDR,m,v,HWIO_MSS_BUS_CSR_CBCR_IN)
#define HWIO_MSS_BUS_CSR_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_CSR_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_CSR_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_BUS_CSR_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_BUS_BRIDGE_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000109c)
#define HWIO_MSS_BUS_BRIDGE_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_BUS_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_BRIDGE_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_MODEM_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x000010a0)
#define HWIO_MSS_BUS_MODEM_CBCR_RMSK                                        0x80000000
#define HWIO_MSS_BUS_MODEM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CBCR_ADDR, HWIO_MSS_BUS_MODEM_CBCR_RMSK)
#define HWIO_MSS_BUS_MODEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MODEM_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_MODEM_CBCR_CLKOFF_SHFT                                       0x1f

#define HWIO_MSS_BUS_Q6_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x000010a4)
#define HWIO_MSS_BUS_Q6_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_Q6_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_Q6_CBCR_ADDR, HWIO_MSS_BUS_Q6_CBCR_RMSK)
#define HWIO_MSS_BUS_Q6_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_Q6_CBCR_ADDR, m)
#define HWIO_MSS_BUS_Q6_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_Q6_CBCR_ADDR,v)
#define HWIO_MSS_BUS_Q6_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_Q6_CBCR_ADDR,m,v,HWIO_MSS_BUS_Q6_CBCR_IN)
#define HWIO_MSS_BUS_Q6_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_Q6_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_Q6_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_Q6_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR                                (MSS_PERPH_REG_BASE      + 0x000010a8)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_RMSK                                0x80000001
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_CLKOFF_BMSK                         0x80000000
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_CLKOFF_SHFT                               0x1f
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_CLKEN_BMSK                                 0x1
#define HWIO_MSS_BUS_NAV_CE_BRIDGE_CBCR_CLKEN_SHFT                                 0x0

#define HWIO_MSS_BUS_CRYPTO_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010ac)
#define HWIO_MSS_BUS_CRYPTO_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_BUS_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, HWIO_MSS_BUS_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_BUS_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_BUS_CRYPTO_CBCR_IN)
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_BUS_NAV_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x000010b0)
#define HWIO_MSS_BUS_NAV_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_BUS_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, HWIO_MSS_BUS_NAV_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR                              (MSS_PERPH_REG_BASE      + 0x000010b4)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_RMSK                              0x80000001
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR, HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_RMSK)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_ADDR,m,v,HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_IN)
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKOFF_BMSK                       0x80000000
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKOFF_SHFT                             0x1f
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKEN_BMSK                               0x1
#define HWIO_MSS_BUS_NC_HM_BRIDGE_CX_CBCR_CLKEN_SHFT                               0x0

#define HWIO_MSS_BUS_ATB_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x000010b8)
#define HWIO_MSS_BUS_ATB_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_BUS_ATB_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, HWIO_MSS_BUS_ATB_CBCR_RMSK)
#define HWIO_MSS_BUS_ATB_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, m)
#define HWIO_MSS_BUS_ATB_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_ATB_CBCR_ADDR,v)
#define HWIO_MSS_BUS_ATB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_ATB_CBCR_ADDR,m,v,HWIO_MSS_BUS_ATB_CBCR_IN)
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR                              (MSS_PERPH_REG_BASE      + 0x000010c0)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_RMSK                              0x80000000
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR, HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_RMSK)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_CLKOFF_BMSK                       0x80000000
#define HWIO_MSS_BUS_MODEM_BRIDGE_CX_CBCR_CLKOFF_SHFT                             0x1f

#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR                                (MSS_PERPH_REG_BASE      + 0x000010c4)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK                                0x80000001
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, m)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,v)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,m,v,HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_BMSK                         0x80000000
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_SHFT                               0x1f
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_BMSK                                 0x1
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_SHFT                                 0x0

#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR                                (MSS_PERPH_REG_BASE      + 0x000010cc)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_RMSK                                0x80000000
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR, HWIO_MSS_BUS_COMBODAC_COMP_CBCR_RMSK)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COMBODAC_COMP_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_CLKOFF_BMSK                         0x80000000
#define HWIO_MSS_BUS_COMBODAC_COMP_CBCR_CLKOFF_SHFT                               0x1f

#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x000010d8)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_RMSK                                   0x80000000
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR, HWIO_MSS_MODEM_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_CLKOFF_BMSK                            0x80000000
#define HWIO_MSS_MODEM_SNOC_AXI_CBCR_CLKOFF_SHFT                                  0x1f

#define HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x000010dc)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_RMSK                                     0x80000001
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR, HWIO_MSS_NAV_SNOC_AXI_CBCR_RMSK)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR, m)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR,v)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_SNOC_AXI_CBCR_ADDR,m,v,HWIO_MSS_NAV_SNOC_AXI_CBCR_IN)
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKOFF_BMSK                              0x80000000
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKOFF_SHFT                                    0x1f
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKEN_BMSK                                      0x1
#define HWIO_MSS_NAV_SNOC_AXI_CBCR_CLKEN_SHFT                                      0x0

#define HWIO_MSS_MPLL2_MAIN_BUS_CBCR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x000010e0)
#define HWIO_MSS_MPLL2_MAIN_BUS_CBCR_RMSK                                   0xc0000000
#define HWIO_MSS_MPLL2_MAIN_BUS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL2_MAIN_BUS_CBCR_ADDR, HWIO_MSS_MPLL2_MAIN_BUS_CBCR_RMSK)
#define HWIO_MSS_MPLL2_MAIN_BUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL2_MAIN_BUS_CBCR_ADDR, m)
#define HWIO_MSS_MPLL2_MAIN_BUS_CBCR_CLKOFF_BMSK                            0x80000000
#define HWIO_MSS_MPLL2_MAIN_BUS_CBCR_CLKOFF_SHFT                                  0x1f
#define HWIO_MSS_MPLL2_MAIN_BUS_CBCR_CLKOFF_MODEM_BMSK                      0x40000000
#define HWIO_MSS_MPLL2_MAIN_BUS_CBCR_CLKOFF_MODEM_SHFT                            0x1e

#define HWIO_MSS_MPLL1_MAIN_CBCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010e8)
#define HWIO_MSS_MPLL1_MAIN_CBCR_RMSK                                       0x80000001
#define HWIO_MSS_MPLL1_MAIN_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR, HWIO_MSS_MPLL1_MAIN_CBCR_RMSK)
#define HWIO_MSS_MPLL1_MAIN_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_MAIN_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_MAIN_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_MAIN_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_MAIN_CBCR_IN)
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_MSS_MPLL1_MAIN_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x000010ec)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_RMSK                                   0xc0000000
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR, HWIO_MSS_MPLL1_MAIN_BUS_CBCR_RMSK)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MAIN_BUS_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_CLKOFF_BMSK                            0x80000000
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_CLKOFF_SHFT                                  0x1f
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_CLKOFF_MODEM_BMSK                      0x40000000
#define HWIO_MSS_MPLL1_MAIN_BUS_CBCR_CLKOFF_MODEM_SHFT                            0x1e

#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR                                 (MSS_PERPH_REG_BASE      + 0x000010f0)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_RMSK                                 0x80000001
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR, HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_RMSK)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_IN)
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKOFF_BMSK                          0x80000000
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKOFF_SHFT                                0x1f
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKEN_BMSK                                  0x1
#define HWIO_MSS_MPLL1_EARLY_DIV5_CBCR_CLKEN_SHFT                                  0x0

#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR                                 (MSS_PERPH_REG_BASE      + 0x000010f4)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_RMSK                                 0x80000001
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR, HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_RMSK)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_IN)
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKOFF_BMSK                          0x80000000
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKOFF_SHFT                                0x1f
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKEN_BMSK                                  0x1
#define HWIO_MSS_MPLL1_EARLY_DIV3_CBCR_CLKEN_SHFT                                  0x0

#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR                             (MSS_PERPH_REG_BASE      + 0x00001100)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_RMSK                             0x80000001
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR, HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_RMSK)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_IN)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKOFF_BMSK                      0x80000000
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKOFF_SHFT                            0x1f
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKEN_BMSK                              0x1
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV3_CBCR_CLKEN_SHFT                              0x0

#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR                             (MSS_PERPH_REG_BASE      + 0x00001104)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_RMSK                             0x80000001
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR, HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_RMSK)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR,v)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_ADDR,m,v,HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_IN)
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKOFF_BMSK                      0x80000000
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKOFF_SHFT                            0x1f
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKEN_BMSK                              0x1
#define HWIO_MSS_MPLL1_OUT_EARLY_DIV5_CBCR_CLKEN_SHFT                              0x0

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001108)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                          0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                          0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                          0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000110c)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                               0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                       0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                         0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                        0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                         0x0

#define HWIO_MSS_Q6_CMD_RCGR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001110)
#define HWIO_MSS_Q6_CMD_RCGR_RMSK                                           0x80000003
#define HWIO_MSS_Q6_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_Q6_CMD_RCGR_ADDR, HWIO_MSS_Q6_CMD_RCGR_RMSK)
#define HWIO_MSS_Q6_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_CMD_RCGR_ADDR, m)
#define HWIO_MSS_Q6_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_Q6_CMD_RCGR_ADDR,v)
#define HWIO_MSS_Q6_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_CMD_RCGR_ADDR,m,v,HWIO_MSS_Q6_CMD_RCGR_IN)
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_OFF_BMSK                                  0x80000000
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_OFF_SHFT                                        0x1f
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_EN_BMSK                                          0x2
#define HWIO_MSS_Q6_CMD_RCGR_ROOT_EN_SHFT                                          0x1
#define HWIO_MSS_Q6_CMD_RCGR_UPDATE_BMSK                                           0x1
#define HWIO_MSS_Q6_CMD_RCGR_UPDATE_SHFT                                           0x0

#define HWIO_MSS_Q6_CFG_RCGR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001114)
#define HWIO_MSS_Q6_CFG_RCGR_RMSK                                                0x71f
#define HWIO_MSS_Q6_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_Q6_CFG_RCGR_ADDR, HWIO_MSS_Q6_CFG_RCGR_RMSK)
#define HWIO_MSS_Q6_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_CFG_RCGR_ADDR, m)
#define HWIO_MSS_Q6_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_Q6_CFG_RCGR_ADDR,v)
#define HWIO_MSS_Q6_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_CFG_RCGR_ADDR,m,v,HWIO_MSS_Q6_CFG_RCGR_IN)
#define HWIO_MSS_Q6_CFG_RCGR_SRC_SEL_BMSK                                        0x700
#define HWIO_MSS_Q6_CFG_RCGR_SRC_SEL_SHFT                                          0x8
#define HWIO_MSS_Q6_CFG_RCGR_SRC_DIV_BMSK                                         0x1f
#define HWIO_MSS_Q6_CFG_RCGR_SRC_DIV_SHFT                                          0x0

#define HWIO_MSS_UIM_CMD_RCGR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001118)
#define HWIO_MSS_UIM_CMD_RCGR_RMSK                                          0x80000003
#define HWIO_MSS_UIM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, HWIO_MSS_UIM_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM_CMD_RCGR_IN)
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_BMSK                                 0x80000000
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_SHFT                                       0x1f
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_BMSK                                         0x2
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_SHFT                                         0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_BMSK                                          0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_SHFT                                          0x0

#define HWIO_MSS_UIM_CFG_RCGR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000111c)
#define HWIO_MSS_UIM_CFG_RCGR_RMSK                                                0x1f
#define HWIO_MSS_UIM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, HWIO_MSS_UIM_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM_CFG_RCGR_IN)
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_BMSK                                        0x1f
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_SHFT                                         0x0

#define HWIO_MSS_BBRX0_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001120)
#define HWIO_MSS_BBRX0_CBCR_RMSK                                            0x80000000
#define HWIO_MSS_BBRX0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_CBCR_ADDR, HWIO_MSS_BBRX0_CBCR_RMSK)
#define HWIO_MSS_BBRX0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_CBCR_ADDR, m)
#define HWIO_MSS_BBRX0_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BBRX0_CBCR_CLKOFF_SHFT                                           0x1f

#define HWIO_MSS_BBRX1_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001124)
#define HWIO_MSS_BBRX1_CBCR_RMSK                                            0x80000000
#define HWIO_MSS_BBRX1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_CBCR_ADDR, HWIO_MSS_BBRX1_CBCR_RMSK)
#define HWIO_MSS_BBRX1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_CBCR_ADDR, m)
#define HWIO_MSS_BBRX1_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BBRX1_CBCR_CLKOFF_SHFT                                           0x1f

#define HWIO_MSS_BBRX2_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001128)
#define HWIO_MSS_BBRX2_CBCR_RMSK                                            0x80000000
#define HWIO_MSS_BBRX2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BBRX2_CBCR_ADDR, HWIO_MSS_BBRX2_CBCR_RMSK)
#define HWIO_MSS_BBRX2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX2_CBCR_ADDR, m)
#define HWIO_MSS_BBRX2_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BBRX2_CBCR_CLKOFF_SHFT                                           0x1f

#define HWIO_MSS_BBRX3_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000112c)
#define HWIO_MSS_BBRX3_CBCR_RMSK                                            0x80000000
#define HWIO_MSS_BBRX3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BBRX3_CBCR_ADDR, HWIO_MSS_BBRX3_CBCR_RMSK)
#define HWIO_MSS_BBRX3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX3_CBCR_ADDR, m)
#define HWIO_MSS_BBRX3_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BBRX3_CBCR_CLKOFF_SHFT                                           0x1f

#define HWIO_MSS_BIMC_THROTTLE_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001130)
#define HWIO_MSS_BIMC_THROTTLE_RMSK                                                0x7
#define HWIO_MSS_BIMC_THROTTLE_IN          \
        in_dword_masked(HWIO_MSS_BIMC_THROTTLE_ADDR, HWIO_MSS_BIMC_THROTTLE_RMSK)
#define HWIO_MSS_BIMC_THROTTLE_INM(m)      \
        in_dword_masked(HWIO_MSS_BIMC_THROTTLE_ADDR, m)
#define HWIO_MSS_BIMC_THROTTLE_OUT(v)      \
        out_dword(HWIO_MSS_BIMC_THROTTLE_ADDR,v)
#define HWIO_MSS_BIMC_THROTTLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIMC_THROTTLE_ADDR,m,v,HWIO_MSS_BIMC_THROTTLE_IN)
#define HWIO_MSS_BIMC_THROTTLE_MSS_DANGER_BMSK                                     0x6
#define HWIO_MSS_BIMC_THROTTLE_MSS_DANGER_SHFT                                     0x1
#define HWIO_MSS_BIMC_THROTTLE_MSS_SAFE_BMSK                                       0x1
#define HWIO_MSS_BIMC_THROTTLE_MSS_SAFE_SHFT                                       0x0

#define HWIO_MSS_RESERVE_02_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001134)
#define HWIO_MSS_RESERVE_02_RMSK                                            0xffffffff
#define HWIO_MSS_RESERVE_02_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, HWIO_MSS_RESERVE_02_RMSK)
#define HWIO_MSS_RESERVE_02_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, m)
#define HWIO_MSS_RESERVE_02_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_02_ADDR,v)
#define HWIO_MSS_RESERVE_02_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_02_ADDR,m,v,HWIO_MSS_RESERVE_02_IN)
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_BMSK                             0xfffffffe
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_SHFT                                    0x1
#define HWIO_MSS_RESERVE_02_GNSS_ADC_TEST_CK_SEL_BMSK                              0x1
#define HWIO_MSS_RESERVE_02_GNSS_ADC_TEST_CK_SEL_SHFT                              0x0

#define HWIO_MSS_BBRX0_MISC_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001138)
#define HWIO_MSS_BBRX0_MISC_RMSK                                                   0xf
#define HWIO_MSS_BBRX0_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX0_MISC_ADDR, HWIO_MSS_BBRX0_MISC_RMSK)
#define HWIO_MSS_BBRX0_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX0_MISC_ADDR, m)
#define HWIO_MSS_BBRX0_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX0_MISC_ADDR,v)
#define HWIO_MSS_BBRX0_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX0_MISC_ADDR,m,v,HWIO_MSS_BBRX0_MISC_IN)
#define HWIO_MSS_BBRX0_MISC_SRC_DIV_BMSK                                           0xf
#define HWIO_MSS_BBRX0_MISC_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_BBRX1_MISC_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000113c)
#define HWIO_MSS_BBRX1_MISC_RMSK                                                   0xf
#define HWIO_MSS_BBRX1_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX1_MISC_ADDR, HWIO_MSS_BBRX1_MISC_RMSK)
#define HWIO_MSS_BBRX1_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX1_MISC_ADDR, m)
#define HWIO_MSS_BBRX1_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX1_MISC_ADDR,v)
#define HWIO_MSS_BBRX1_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX1_MISC_ADDR,m,v,HWIO_MSS_BBRX1_MISC_IN)
#define HWIO_MSS_BBRX1_MISC_SRC_DIV_BMSK                                           0xf
#define HWIO_MSS_BBRX1_MISC_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_BBRX2_MISC_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001140)
#define HWIO_MSS_BBRX2_MISC_RMSK                                                   0xf
#define HWIO_MSS_BBRX2_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX2_MISC_ADDR, HWIO_MSS_BBRX2_MISC_RMSK)
#define HWIO_MSS_BBRX2_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX2_MISC_ADDR, m)
#define HWIO_MSS_BBRX2_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX2_MISC_ADDR,v)
#define HWIO_MSS_BBRX2_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX2_MISC_ADDR,m,v,HWIO_MSS_BBRX2_MISC_IN)
#define HWIO_MSS_BBRX2_MISC_SRC_DIV_BMSK                                           0xf
#define HWIO_MSS_BBRX2_MISC_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_BBRX3_MISC_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001144)
#define HWIO_MSS_BBRX3_MISC_RMSK                                                   0xf
#define HWIO_MSS_BBRX3_MISC_IN          \
        in_dword_masked(HWIO_MSS_BBRX3_MISC_ADDR, HWIO_MSS_BBRX3_MISC_RMSK)
#define HWIO_MSS_BBRX3_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BBRX3_MISC_ADDR, m)
#define HWIO_MSS_BBRX3_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BBRX3_MISC_ADDR,v)
#define HWIO_MSS_BBRX3_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BBRX3_MISC_ADDR,m,v,HWIO_MSS_BBRX3_MISC_IN)
#define HWIO_MSS_BBRX3_MISC_SRC_DIV_BMSK                                           0xf
#define HWIO_MSS_BBRX3_MISC_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR                                (MSS_PERPH_REG_BASE      + 0x0000114c)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_RMSK                                      0x3f
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR, HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_RMSK)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_ADDR,m,v,HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_IN)
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_REF_CLK_SEL_BMSK                          0x30
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_REF_CLK_SEL_SHFT                           0x4
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_SRC_DIV_BMSK                               0xf
#define HWIO_MSS_BUS_RBCPR_REF_DIV_MISC_SRC_DIV_SHFT                               0x0

#define HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x00001150)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK                                     0x80000003
#define HWIO_MSS_UIM0_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_BMSK                            0x80000000
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_SHFT                                  0x1f
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_BMSK                                    0x2
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_SHFT                                    0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_BMSK                                     0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_SHFT                                     0x0

#define HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x00001154)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK                                         0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_BMSK                                    0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_SHFT                                       0xc

#define HWIO_MSS_UIM0_MND_M_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001158)
#define HWIO_MSS_UIM0_MND_M_RMSK                                                0xffff
#define HWIO_MSS_UIM0_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, HWIO_MSS_UIM0_MND_M_RMSK)
#define HWIO_MSS_UIM0_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, m)
#define HWIO_MSS_UIM0_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_M_ADDR,v)
#define HWIO_MSS_UIM0_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_M_ADDR,m,v,HWIO_MSS_UIM0_MND_M_IN)
#define HWIO_MSS_UIM0_MND_M_M_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM0_MND_M_M_VALUE_SHFT                                           0x0

#define HWIO_MSS_UIM0_MND_N_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000115c)
#define HWIO_MSS_UIM0_MND_N_RMSK                                                0xffff
#define HWIO_MSS_UIM0_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, HWIO_MSS_UIM0_MND_N_RMSK)
#define HWIO_MSS_UIM0_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, m)
#define HWIO_MSS_UIM0_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_N_ADDR,v)
#define HWIO_MSS_UIM0_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_N_ADDR,m,v,HWIO_MSS_UIM0_MND_N_IN)
#define HWIO_MSS_UIM0_MND_N_N_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM0_MND_N_N_VALUE_SHFT                                           0x0

#define HWIO_MSS_UIM0_MND_D_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001160)
#define HWIO_MSS_UIM0_MND_D_RMSK                                                0xffff
#define HWIO_MSS_UIM0_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, HWIO_MSS_UIM0_MND_D_RMSK)
#define HWIO_MSS_UIM0_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, m)
#define HWIO_MSS_UIM0_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_D_ADDR,v)
#define HWIO_MSS_UIM0_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_D_ADDR,m,v,HWIO_MSS_UIM0_MND_D_IN)
#define HWIO_MSS_UIM0_MND_D_D_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM0_MND_D_D_VALUE_SHFT                                           0x0

#define HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x00001164)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK                                     0x80000003
#define HWIO_MSS_UIM1_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_BMSK                            0x80000000
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_SHFT                                  0x1f
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_BMSK                                    0x2
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_SHFT                                    0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_BMSK                                     0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_SHFT                                     0x0

#define HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x00001168)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK                                         0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_BMSK                                    0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_SHFT                                       0xc

#define HWIO_MSS_UIM1_MND_M_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000116c)
#define HWIO_MSS_UIM1_MND_M_RMSK                                                0xffff
#define HWIO_MSS_UIM1_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, HWIO_MSS_UIM1_MND_M_RMSK)
#define HWIO_MSS_UIM1_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, m)
#define HWIO_MSS_UIM1_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_M_ADDR,v)
#define HWIO_MSS_UIM1_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_M_ADDR,m,v,HWIO_MSS_UIM1_MND_M_IN)
#define HWIO_MSS_UIM1_MND_M_M_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM1_MND_M_M_VALUE_SHFT                                           0x0

#define HWIO_MSS_UIM1_MND_N_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001170)
#define HWIO_MSS_UIM1_MND_N_RMSK                                                0xffff
#define HWIO_MSS_UIM1_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, HWIO_MSS_UIM1_MND_N_RMSK)
#define HWIO_MSS_UIM1_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, m)
#define HWIO_MSS_UIM1_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_N_ADDR,v)
#define HWIO_MSS_UIM1_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_N_ADDR,m,v,HWIO_MSS_UIM1_MND_N_IN)
#define HWIO_MSS_UIM1_MND_N_N_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM1_MND_N_N_VALUE_SHFT                                           0x0

#define HWIO_MSS_UIM1_MND_D_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001174)
#define HWIO_MSS_UIM1_MND_D_RMSK                                                0xffff
#define HWIO_MSS_UIM1_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, HWIO_MSS_UIM1_MND_D_RMSK)
#define HWIO_MSS_UIM1_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, m)
#define HWIO_MSS_UIM1_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_D_ADDR,v)
#define HWIO_MSS_UIM1_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_D_ADDR,m,v,HWIO_MSS_UIM1_MND_D_IN)
#define HWIO_MSS_UIM1_MND_D_D_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM1_MND_D_D_VALUE_SHFT                                           0x0

#define HWIO_MSS_UIM2_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001178)
#define HWIO_MSS_UIM2_BCR_RMSK                                              0x80000001
#define HWIO_MSS_UIM2_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_BCR_ADDR, HWIO_MSS_UIM2_BCR_RMSK)
#define HWIO_MSS_UIM2_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_BCR_ADDR, m)
#define HWIO_MSS_UIM2_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_BCR_ADDR,v)
#define HWIO_MSS_UIM2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_BCR_ADDR,m,v,HWIO_MSS_UIM2_BCR_IN)
#define HWIO_MSS_UIM2_BCR_BUS_UIM2_SM_ARES_IN_BMSK                          0x80000000
#define HWIO_MSS_UIM2_BCR_BUS_UIM2_SM_ARES_IN_SHFT                                0x1f
#define HWIO_MSS_UIM2_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_UIM2_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_UIM2_CBCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x0000117c)
#define HWIO_MSS_UIM2_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_CBCR_ADDR, HWIO_MSS_UIM2_CBCR_RMSK)
#define HWIO_MSS_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_CBCR_ADDR,m,v,HWIO_MSS_UIM2_CBCR_IN)
#define HWIO_MSS_UIM2_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_UIM2_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_UIM2_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_UIM2_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_XO_UIM2_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001180)
#define HWIO_MSS_XO_UIM2_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_XO_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_UIM2_CBCR_ADDR, HWIO_MSS_XO_UIM2_CBCR_RMSK)
#define HWIO_MSS_XO_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_XO_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_XO_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_UIM2_CBCR_ADDR,m,v,HWIO_MSS_XO_UIM2_CBCR_IN)
#define HWIO_MSS_XO_UIM2_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_XO_UIM2_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_XO_UIM2_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_XO_UIM2_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_BUS_UIM2_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001184)
#define HWIO_MSS_BUS_UIM2_CBCR_RMSK                                         0x80007ff1
#define HWIO_MSS_BUS_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, HWIO_MSS_BUS_UIM2_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM2_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM2_CBCR_IN)
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_BMSK                           0x4000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_SHFT                              0xe
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_BMSK                         0x2000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_SHFT                            0xd
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                        0x1000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                           0xc
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_BMSK                                       0xf00
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_SHFT                                         0x8
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_BMSK                                         0xf0
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_SHFT                                          0x4
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x00001188)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK                                     0x80000003
#define HWIO_MSS_UIM2_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_BMSK                            0x80000000
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_SHFT                                  0x1f
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_BMSK                                    0x2
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_SHFT                                    0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_BMSK                                     0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_SHFT                                     0x0

#define HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR                                     (MSS_PERPH_REG_BASE      + 0x0000118c)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK                                         0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_BMSK                                    0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_SHFT                                       0xc

#define HWIO_MSS_UIM2_MND_M_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001190)
#define HWIO_MSS_UIM2_MND_M_RMSK                                                0xffff
#define HWIO_MSS_UIM2_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, HWIO_MSS_UIM2_MND_M_RMSK)
#define HWIO_MSS_UIM2_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, m)
#define HWIO_MSS_UIM2_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_M_ADDR,v)
#define HWIO_MSS_UIM2_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_M_ADDR,m,v,HWIO_MSS_UIM2_MND_M_IN)
#define HWIO_MSS_UIM2_MND_M_M_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM2_MND_M_M_VALUE_SHFT                                           0x0

#define HWIO_MSS_UIM2_MND_N_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001194)
#define HWIO_MSS_UIM2_MND_N_RMSK                                                0xffff
#define HWIO_MSS_UIM2_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, HWIO_MSS_UIM2_MND_N_RMSK)
#define HWIO_MSS_UIM2_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, m)
#define HWIO_MSS_UIM2_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_N_ADDR,v)
#define HWIO_MSS_UIM2_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_N_ADDR,m,v,HWIO_MSS_UIM2_MND_N_IN)
#define HWIO_MSS_UIM2_MND_N_N_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM2_MND_N_N_VALUE_SHFT                                           0x0

#define HWIO_MSS_UIM2_MND_D_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001198)
#define HWIO_MSS_UIM2_MND_D_RMSK                                                0xffff
#define HWIO_MSS_UIM2_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, HWIO_MSS_UIM2_MND_D_RMSK)
#define HWIO_MSS_UIM2_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, m)
#define HWIO_MSS_UIM2_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_D_ADDR,v)
#define HWIO_MSS_UIM2_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_D_ADDR,m,v,HWIO_MSS_UIM2_MND_D_IN)
#define HWIO_MSS_UIM2_MND_D_D_VALUE_BMSK                                        0xffff
#define HWIO_MSS_UIM2_MND_D_D_VALUE_SHFT                                           0x0

#define HWIO_MSS_BUS_MGPI_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x0000119c)
#define HWIO_MSS_BUS_MGPI_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_BUS_MGPI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, HWIO_MSS_BUS_MGPI_CBCR_RMSK)
#define HWIO_MSS_BUS_MGPI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MGPI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MGPI_CBCR_ADDR,v)
#define HWIO_MSS_BUS_MGPI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MGPI_CBCR_ADDR,m,v,HWIO_MSS_BUS_MGPI_CBCR_IN)
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_MGPI_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000011a0)
#define HWIO_MSS_MGPI_BCR_RMSK                                              0x80000001
#define HWIO_MSS_MGPI_BCR_IN          \
        in_dword_masked(HWIO_MSS_MGPI_BCR_ADDR, HWIO_MSS_MGPI_BCR_RMSK)
#define HWIO_MSS_MGPI_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MGPI_BCR_ADDR, m)
#define HWIO_MSS_MGPI_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MGPI_BCR_ADDR,v)
#define HWIO_MSS_MGPI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MGPI_BCR_ADDR,m,v,HWIO_MSS_MGPI_BCR_IN)
#define HWIO_MSS_MGPI_BCR_BUS_MGPI_ARES_IN_BMSK                             0x80000000
#define HWIO_MSS_MGPI_BCR_BUS_MGPI_ARES_IN_SHFT                                   0x1f
#define HWIO_MSS_MGPI_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_MGPI_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_BUS_RBCPR_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x000011b0)
#define HWIO_MSS_BUS_RBCPR_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_RBCPR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, HWIO_MSS_BUS_RBCPR_CBCR_RMSK)
#define HWIO_MSS_BUS_RBCPR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,m,v,HWIO_MSS_BUS_RBCPR_CBCR_IN)
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_RBCPR_BCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x000011b4)
#define HWIO_MSS_RBCPR_BCR_RMSK                                                    0x1
#define HWIO_MSS_RBCPR_BCR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BCR_ADDR, HWIO_MSS_RBCPR_BCR_RMSK)
#define HWIO_MSS_RBCPR_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BCR_ADDR, m)
#define HWIO_MSS_RBCPR_BCR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_BCR_ADDR,v)
#define HWIO_MSS_RBCPR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_BCR_ADDR,m,v,HWIO_MSS_RBCPR_BCR_IN)
#define HWIO_MSS_RBCPR_BCR_BLK_ARES_BMSK                                           0x1
#define HWIO_MSS_RBCPR_BCR_BLK_ARES_SHFT                                           0x0

#define HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR                                    (MSS_PERPH_REG_BASE      + 0x000011b8)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_RMSK                                    0x80000001
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR, HWIO_MSS_BUS_RBCPR_REF_CBCR_RMSK)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_REF_CBCR_ADDR,m,v,HWIO_MSS_BUS_RBCPR_REF_CBCR_IN)
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_CLKOFF_BMSK                             0x80000000
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_CLKOFF_SHFT                                   0x1f
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_CLKEN_BMSK                                     0x1
#define HWIO_MSS_BUS_RBCPR_REF_CBCR_CLKEN_SHFT                                     0x0

#define HWIO_MSS_Q6SS_CP_BCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001200)
#define HWIO_MSS_Q6SS_CP_BCR_RMSK                                           0x80000001
#define HWIO_MSS_Q6SS_CP_BCR_IN          \
        in_dword_masked(HWIO_MSS_Q6SS_CP_BCR_ADDR, HWIO_MSS_Q6SS_CP_BCR_RMSK)
#define HWIO_MSS_Q6SS_CP_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6SS_CP_BCR_ADDR, m)
#define HWIO_MSS_Q6SS_CP_BCR_OUT(v)      \
        out_dword(HWIO_MSS_Q6SS_CP_BCR_ADDR,v)
#define HWIO_MSS_Q6SS_CP_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6SS_CP_BCR_ADDR,m,v,HWIO_MSS_Q6SS_CP_BCR_IN)
#define HWIO_MSS_Q6SS_CP_BCR_BUS_Q6_SM_ARES_IN_BMSK                         0x80000000
#define HWIO_MSS_Q6SS_CP_BCR_BUS_Q6_SM_ARES_IN_SHFT                               0x1f
#define HWIO_MSS_Q6SS_CP_BCR_BLK_ARES_BMSK                                         0x1
#define HWIO_MSS_Q6SS_CP_BCR_BLK_ARES_SHFT                                         0x0

#define HWIO_MSS_XBAR_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001204)
#define HWIO_MSS_XBAR_BCR_RMSK                                                     0x1
#define HWIO_MSS_XBAR_BCR_IN          \
        in_dword_masked(HWIO_MSS_XBAR_BCR_ADDR, HWIO_MSS_XBAR_BCR_RMSK)
#define HWIO_MSS_XBAR_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XBAR_BCR_ADDR, m)
#define HWIO_MSS_XBAR_BCR_OUT(v)      \
        out_dword(HWIO_MSS_XBAR_BCR_ADDR,v)
#define HWIO_MSS_XBAR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XBAR_BCR_ADDR,m,v,HWIO_MSS_XBAR_BCR_IN)
#define HWIO_MSS_XBAR_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_XBAR_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_BUS_Q6_CP_CBCR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001208)
#define HWIO_MSS_BUS_Q6_CP_CBCR_RMSK                                        0x80000001
#define HWIO_MSS_BUS_Q6_CP_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_Q6_CP_CBCR_ADDR, HWIO_MSS_BUS_Q6_CP_CBCR_RMSK)
#define HWIO_MSS_BUS_Q6_CP_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_Q6_CP_CBCR_ADDR, m)
#define HWIO_MSS_BUS_Q6_CP_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_Q6_CP_CBCR_ADDR,v)
#define HWIO_MSS_BUS_Q6_CP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_Q6_CP_CBCR_ADDR,m,v,HWIO_MSS_BUS_Q6_CP_CBCR_IN)
#define HWIO_MSS_BUS_Q6_CP_CBCR_CLKOFF_BMSK                                 0x80000000
#define HWIO_MSS_BUS_Q6_CP_CBCR_CLKOFF_SHFT                                       0x1f
#define HWIO_MSS_BUS_Q6_CP_CBCR_CLKEN_BMSK                                         0x1
#define HWIO_MSS_BUS_Q6_CP_CBCR_CLKEN_SHFT                                         0x0

#define HWIO_MSS_Q6_CP_CMD_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000120c)
#define HWIO_MSS_Q6_CP_CMD_RCGR_RMSK                                        0x80000003
#define HWIO_MSS_Q6_CP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_Q6_CP_CMD_RCGR_ADDR, HWIO_MSS_Q6_CP_CMD_RCGR_RMSK)
#define HWIO_MSS_Q6_CP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_CP_CMD_RCGR_ADDR, m)
#define HWIO_MSS_Q6_CP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_Q6_CP_CMD_RCGR_ADDR,v)
#define HWIO_MSS_Q6_CP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_CP_CMD_RCGR_ADDR,m,v,HWIO_MSS_Q6_CP_CMD_RCGR_IN)
#define HWIO_MSS_Q6_CP_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_MSS_Q6_CP_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_MSS_Q6_CP_CMD_RCGR_ROOT_EN_BMSK                                       0x2
#define HWIO_MSS_Q6_CP_CMD_RCGR_ROOT_EN_SHFT                                       0x1
#define HWIO_MSS_Q6_CP_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_MSS_Q6_CP_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_MSS_Q6_CP_CFG_RCGR_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001210)
#define HWIO_MSS_Q6_CP_CFG_RCGR_RMSK                                             0x71f
#define HWIO_MSS_Q6_CP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_Q6_CP_CFG_RCGR_ADDR, HWIO_MSS_Q6_CP_CFG_RCGR_RMSK)
#define HWIO_MSS_Q6_CP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6_CP_CFG_RCGR_ADDR, m)
#define HWIO_MSS_Q6_CP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_Q6_CP_CFG_RCGR_ADDR,v)
#define HWIO_MSS_Q6_CP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6_CP_CFG_RCGR_ADDR,m,v,HWIO_MSS_Q6_CP_CFG_RCGR_IN)
#define HWIO_MSS_Q6_CP_CFG_RCGR_SRC_SEL_BMSK                                     0x700
#define HWIO_MSS_Q6_CP_CFG_RCGR_SRC_SEL_SHFT                                       0x8
#define HWIO_MSS_Q6_CP_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_MSS_Q6_CP_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR                                 (MSS_PERPH_REG_BASE      + 0x00001214)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK                                 0x80000003
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_BMSK                        0x80000000
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_SHFT                              0x1f
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_BMSK                                0x2
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_SHFT                                0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_BMSK                                 0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_SHFT                                 0x0

#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR                                 (MSS_PERPH_REG_BASE      + 0x00001218)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK                                     0x371f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_BMSK                                0x3000
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_SHFT                                   0xc
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_BMSK                              0x700
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_SHFT                                0x8
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_BMSK                               0x1f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_SHFT                                0x0

#define HWIO_MSS_BIT_COXM_MND_M_ADDR                                        (MSS_PERPH_REG_BASE      + 0x0000121c)
#define HWIO_MSS_BIT_COXM_MND_M_RMSK                                            0xffff
#define HWIO_MSS_BIT_COXM_MND_M_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, HWIO_MSS_BIT_COXM_MND_M_RMSK)
#define HWIO_MSS_BIT_COXM_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_M_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_M_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_M_IN)
#define HWIO_MSS_BIT_COXM_MND_M_M_VALUE_BMSK                                    0xffff
#define HWIO_MSS_BIT_COXM_MND_M_M_VALUE_SHFT                                       0x0

#define HWIO_MSS_BIT_COXM_MND_N_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001220)
#define HWIO_MSS_BIT_COXM_MND_N_RMSK                                            0xffff
#define HWIO_MSS_BIT_COXM_MND_N_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, HWIO_MSS_BIT_COXM_MND_N_RMSK)
#define HWIO_MSS_BIT_COXM_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_N_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_N_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_N_IN)
#define HWIO_MSS_BIT_COXM_MND_N_N_VALUE_BMSK                                    0xffff
#define HWIO_MSS_BIT_COXM_MND_N_N_VALUE_SHFT                                       0x0

#define HWIO_MSS_BIT_COXM_MND_D_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001224)
#define HWIO_MSS_BIT_COXM_MND_D_RMSK                                            0xffff
#define HWIO_MSS_BIT_COXM_MND_D_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, HWIO_MSS_BIT_COXM_MND_D_RMSK)
#define HWIO_MSS_BIT_COXM_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_D_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_D_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_D_IN)
#define HWIO_MSS_BIT_COXM_MND_D_D_VALUE_BMSK                                    0xffff
#define HWIO_MSS_BIT_COXM_MND_D_D_VALUE_SHFT                                       0x0

#define HWIO_MSS_BIT_COXM_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x0000122c)
#define HWIO_MSS_BIT_COXM_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_BIT_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, HWIO_MSS_BIT_COXM_CBCR_RMSK)
#define HWIO_MSS_BIT_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BIT_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BIT_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_CBCR_ADDR,m,v,HWIO_MSS_BIT_COXM_CBCR_IN)
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BUS_COXM_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001230)
#define HWIO_MSS_BUS_COXM_CBCR_RMSK                                         0x80007ff1
#define HWIO_MSS_BUS_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, HWIO_MSS_BUS_COXM_CBCR_RMSK)
#define HWIO_MSS_BUS_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BUS_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_COXM_CBCR_ADDR,m,v,HWIO_MSS_BUS_COXM_CBCR_IN)
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_BMSK                           0x4000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_SHFT                              0xe
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                         0x2000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                            0xd
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                        0x1000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                           0xc
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_BMSK                                       0xf00
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_SHFT                                         0x8
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_BMSK                                         0xf0
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_SHFT                                          0x4
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_COXM_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001234)
#define HWIO_MSS_COXM_BCR_RMSK                                              0x80000001
#define HWIO_MSS_COXM_BCR_IN          \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, HWIO_MSS_COXM_BCR_RMSK)
#define HWIO_MSS_COXM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, m)
#define HWIO_MSS_COXM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_COXM_BCR_ADDR,v)
#define HWIO_MSS_COXM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_COXM_BCR_ADDR,m,v,HWIO_MSS_COXM_BCR_IN)
#define HWIO_MSS_COXM_BCR_BUS_UIM0_SM_ARES_IN_BMSK                          0x80000000
#define HWIO_MSS_COXM_BCR_BUS_UIM0_SM_ARES_IN_SHFT                                0x1f
#define HWIO_MSS_COXM_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_COXM_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_BUS_MODEM_CMD_RCGR_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001238)
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_RMSK                                    0x80000001
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CMD_RCGR_ADDR, HWIO_MSS_BUS_MODEM_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MODEM_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MODEM_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_MODEM_CMD_RCGR_IN)
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_ROOT_OFF_BMSK                           0x80000000
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_ROOT_OFF_SHFT                                 0x1f
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_UPDATE_BMSK                                    0x1
#define HWIO_MSS_BUS_MODEM_CMD_RCGR_UPDATE_SHFT                                    0x0

#define HWIO_MSS_BUS_MODEM_CFG_RCGR_ADDR                                    (MSS_PERPH_REG_BASE      + 0x0000123c)
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_RMSK                                         0x71f
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CFG_RCGR_ADDR, HWIO_MSS_BUS_MODEM_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MODEM_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MODEM_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MODEM_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_MODEM_CFG_RCGR_IN)
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_MODEM_SRC_SEL_BMSK                           0x700
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_MODEM_SRC_SEL_SHFT                             0x8
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_MODEM_SRC_DIV_BMSK                            0x1f
#define HWIO_MSS_BUS_MODEM_CFG_RCGR_MODEM_SRC_DIV_SHFT                             0x0

#define HWIO_MSS_TCSR_ACC_SEL_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000f000)
#define HWIO_MSS_TCSR_ACC_SEL_RMSK                                                 0x3
#define HWIO_MSS_TCSR_ACC_SEL_IN          \
        in_dword_masked(HWIO_MSS_TCSR_ACC_SEL_ADDR, HWIO_MSS_TCSR_ACC_SEL_RMSK)
#define HWIO_MSS_TCSR_ACC_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_TCSR_ACC_SEL_ADDR, m)
#define HWIO_MSS_TCSR_ACC_SEL_OUT(v)      \
        out_dword(HWIO_MSS_TCSR_ACC_SEL_ADDR,v)
#define HWIO_MSS_TCSR_ACC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_TCSR_ACC_SEL_ADDR,m,v,HWIO_MSS_TCSR_ACC_SEL_IN)
#define HWIO_MSS_TCSR_ACC_SEL_ACC_MEM_SEL_BMSK                                     0x3
#define HWIO_MSS_TCSR_ACC_SEL_ACC_MEM_SEL_SHFT                                     0x0


#endif /* __MSS_RBCPR_ACCESS_HWIO_H__ */
