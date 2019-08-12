#ifndef __ABTIMEOUT_HWIO_H__
#define __ABTIMEOUT_HWIO_H__
/*
===========================================================================
*/
/**
  @file ABTimeout_HWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_CONF_BUS_TIMEOUT
    TCSR_TCSR_REGS
    TCSR_TCSR_REGS

  'Include' filters applied: TIMEOUT_INTR[TCSR_TCSR_REGS] BASE[MSS_CONF_BUS_TIMEOUT] BASE[LPASS_AHBI_TIME]
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/8909/ABTimeout_HWIO.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: MSS_CONF_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define MSS_CONF_BUS_TIMEOUT_REG_BASE                          (MSS_TOP_BASE      + 0x001b0000)

/*----------------------------------------------------------------------------
 * MODULE: LPASS_AHBI_TIME
 *--------------------------------------------------------------------------*/

#define LPASS_AHBI_TIME_REG_BASE                                 (ULTAUDIO_CORE_BASE      + 0x00005000)

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                           (CORE_TOP_CSR_BASE      + 0x00037000)

#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR                                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x00005000)

#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00016020)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK                                                                                0xffffffff
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_ULTAUDIO_IRQ_BMSK                                                          0x400000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_ULTAUDIO_IRQ_SHFT                                                              0x16
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_SLAVE_IRQ_BMSK                                                      0x100000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_SLAVE_IRQ_SHFT                                                          0x14
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_SNOC_S1_IRQ_BMSK                                                              0x400
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_SNOC_S1_IRQ_SHFT                                                                0xe
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_SNOC_S0_IRQ_BMSK                                                              0x100
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_SNOC_S0_IRQ_SHFT                                                                0xd
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S9_IRQ_BMSK                                                             0x200
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S9_IRQ_SHFT                                                               0x9
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S8_IRQ_BMSK                                                             0x100
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S8_IRQ_SHFT                                                               0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S7_IRQ_BMSK                                                              0x80
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S7_IRQ_SHFT                                                               0x7
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S6_IRQ_BMSK                                                              0x40
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S6_IRQ_SHFT                                                               0x6
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S5_IRQ_BMSK                                                              0x20
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S5_IRQ_SHFT                                                               0x5
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S4_IRQ_BMSK                                                              0x10
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S4_IRQ_SHFT                                                               0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S3_IRQ_BMSK                                                               0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S3_IRQ_SHFT                                                               0x3
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S2_IRQ_BMSK                                                               0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S2_IRQ_SHFT                                                               0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S1_IRQ_BMSK                                                               0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S1_IRQ_SHFT                                                               0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S0_IRQ_BMSK                                                               0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_BUS_TIMEOUT_PCNOC_S0_IRQ_SHFT                                                               0x0

#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00016060)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_RMSK                                                                            0xffffffff

#endif /* __ABTIMEOUT_HWIO_H__ */
