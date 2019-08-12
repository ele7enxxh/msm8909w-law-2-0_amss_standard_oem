#ifndef __LM_HWIO_H__
#define __LM_HWIO_H__
/*
===========================================================================
*/
/**
  @file lm_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG
    TCSR_TCSR_REGS

  'Include' filters applied: S1[TCSR_TCSR_REGS] S1[GCC_CLK_CTL_REG] 
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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/lm/inc/lm_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                                (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_PHYS                                                           (CLK_CTL_BASE_PHYS + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_OFFS                                                           0x00000000

#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00000008)
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_PHYS                                                       (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00000008)
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_OFFS                                                       (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00000008)
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_RMSK                                                       0xffffffff
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_IN          \
        in_dword_masked(HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR, HWIO_GCC_S1LM_MODEM_DIV_VOTE_RMSK)
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR, m)
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR,v)
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_S1LM_MODEM_DIV_VOTE_ADDR,m,v,HWIO_GCC_S1LM_MODEM_DIV_VOTE_IN)
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_SPARE_BITS_BMSK                                            0xfffffffe
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_SPARE_BITS_SHFT                                                   0x1
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_S1LM_MODEM_DIV_VOTE_BMSK                                          0x1
#define HWIO_GCC_S1LM_MODEM_DIV_VOTE_S1LM_MODEM_DIV_VOTE_SHFT                                          0x0

#define HWIO_GCC_S1LM_APPS_DIV_VOTE_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e000)
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_PHYS                                                        (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x0007e000)
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_OFFS                                                        (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0007e000)
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_RMSK                                                        0xffffffff
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_IN          \
        in_dword_masked(HWIO_GCC_S1LM_APPS_DIV_VOTE_ADDR, HWIO_GCC_S1LM_APPS_DIV_VOTE_RMSK)
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_S1LM_APPS_DIV_VOTE_ADDR, m)
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_S1LM_APPS_DIV_VOTE_ADDR,v)
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_S1LM_APPS_DIV_VOTE_ADDR,m,v,HWIO_GCC_S1LM_APPS_DIV_VOTE_IN)
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_SPARE_BITS31_18_BMSK                                        0xfffc0000
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_SPARE_BITS31_18_SHFT                                              0x12
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_SW_OVERRIDE_BMSK                                 0x20000
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_SW_OVERRIDE_SHFT                                    0x11
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_HW_SW_OVERRIDE_EN_CHICKEN_BIT_BMSK               0x10000
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_HW_SW_OVERRIDE_EN_CHICKEN_BIT_SHFT                  0x10
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_SPARE_BITS15_1_BMSK                                             0xfffe
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_SPARE_BITS15_1_SHFT                                                0x1
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_VOTE_BMSK                                            0x1
#define HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_VOTE_SHFT                                            0x0

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                           (CORE_TOP_CSR_BASE      + 0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_PHYS                                                                                      (CORE_TOP_CSR_BASE_PHYS + 0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_OFFS                                                                                      0x00037000

#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_ADDR                                                                        (TCSR_TCSR_REGS_REG_BASE      + 0x0000f000)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_PHYS                                                                        (TCSR_TCSR_REGS_REG_BASE_PHYS + 0x0000f000)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_OFFS                                                                        (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x0000f000)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_RMSK                                                                        0xffffffff
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_IN          \
        in_dword_masked(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_ADDR, HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_RMSK)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_INM(m)      \
        in_dword_masked(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_ADDR, m)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_OUT(v)      \
        out_dword(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_ADDR,v)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_ADDR,m,v,HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_IN)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_SPARE_APU_REG0_BMSK                                                         0xfffffffe
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_SPARE_APU_REG0_SHFT                                                                0x1
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_TCSR_S1LM_MODEM_TO_APPS_INT_BMSK                                                   0x1
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_TCSR_S1LM_MODEM_TO_APPS_INT_SHFT                                                   0x0

#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x0000f004)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_PHYS                                                                   (TCSR_TCSR_REGS_REG_BASE_PHYS + 0x0000f004)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_OFFS                                                                   (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x0000f004)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_RMSK                                                                   0xffffffff
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_IN          \
        in_dword_masked(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR, HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_RMSK)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_INM(m)      \
        in_dword_masked(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR, m)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_OUT(v)      \
        out_dword(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR,v)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_ADDR,m,v,HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_IN)
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_BMSK                                  0xffffffff
#define HWIO_TCSR_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_TCSR_S1LM_MODEM_TO_APPS_INT_DATA_SHFT                                         0x0

#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_ADDR                                                                        (TCSR_TCSR_REGS_REG_BASE      + 0x0001f000)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_PHYS                                                                        (TCSR_TCSR_REGS_REG_BASE_PHYS + 0x0001f000)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_OFFS                                                                        (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x0001f000)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_RMSK                                                                        0xffffffff
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_IN          \
        in_dword_masked(HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_ADDR, HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_RMSK)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_INM(m)      \
        in_dword_masked(HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_ADDR, m)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_OUT(v)      \
        out_dword(HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_ADDR,v)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_ADDR,m,v,HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_IN)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_SPARE_REG0_BMSK                                                             0xfffffffe
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_SPARE_REG0_SHFT                                                                    0x1
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_TCSR_S1LM_APPS_TO_MODEM_INT_BMSK                                                   0x1
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_TCSR_S1LM_APPS_TO_MODEM_INT_SHFT                                                   0x0

#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_ADDR                                                                   (TCSR_TCSR_REGS_REG_BASE      + 0x0001f004)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_PHYS                                                                   (TCSR_TCSR_REGS_REG_BASE_PHYS + 0x0001f004)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_OFFS                                                                   (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x0001f004)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_RMSK                                                                   0xffffffff
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_IN          \
        in_dword_masked(HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_ADDR, HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_RMSK)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_INM(m)      \
        in_dword_masked(HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_ADDR, m)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_OUT(v)      \
        out_dword(HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_ADDR,v)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_ADDR,m,v,HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_IN)
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_BMSK                                  0xffffffff
#define HWIO_TCSR_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_TCSR_S1LM_APPS_TO_MODEM_INT_DATA_SHFT                                         0x0


#endif /* __LM_HWIO_H__ */
