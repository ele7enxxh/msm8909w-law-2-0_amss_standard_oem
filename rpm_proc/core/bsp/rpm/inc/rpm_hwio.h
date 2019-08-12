#ifndef __RPM_HWIO_H__
#define __RPM_HWIO_H__
/*
===========================================================================
*/
/**
  @file rpm_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    RPM_APU
    RPM_DEC
    RPM_F0_QTMR_V1_F0
    RPM_F1_QTMR_V1_F1
    RPM_MPU
    RPM_MSG_RAM_RPU
    RPM_QTMR_AC
    RPM_VMIDMT

  'Include' filters applied:
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

  $Header: //components/rel/rpm.bf/2.1.1/core/bsp/rpm/inc/rpm_hwio.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

#define RPM_CSR_BASE                                                 0x00080000

/*----------------------------------------------------------------------------
 * MODULE: RPM_APU
 *--------------------------------------------------------------------------*/

#define RPM_APU_REG_BASE                                             (RPM_CSR_BASE      + 0x00007000)

#define HWIO_RPM_APU_XPU_SCR_ADDR                                    (RPM_APU_REG_BASE      + 0x00000000)
#define HWIO_RPM_APU_XPU_SCR_RMSK                                         0x17f
#define HWIO_RPM_APU_XPU_SCR_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_SCR_ADDR, HWIO_RPM_APU_XPU_SCR_RMSK)
#define HWIO_RPM_APU_XPU_SCR_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_SCR_ADDR, m)
#define HWIO_RPM_APU_XPU_SCR_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_SCR_ADDR,v)
#define HWIO_RPM_APU_XPU_SCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_SCR_ADDR,m,v,HWIO_RPM_APU_XPU_SCR_IN)
#define HWIO_RPM_APU_XPU_SCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_RPM_APU_XPU_SCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_RPM_APU_XPU_SCR_NSRGCLEE_BMSK                                 0x40
#define HWIO_RPM_APU_XPU_SCR_NSRGCLEE_SHFT                                  0x6
#define HWIO_RPM_APU_XPU_SCR_NSCFGE_BMSK                                   0x20
#define HWIO_RPM_APU_XPU_SCR_NSCFGE_SHFT                                    0x5
#define HWIO_RPM_APU_XPU_SCR_SDCDEE_BMSK                                   0x10
#define HWIO_RPM_APU_XPU_SCR_SDCDEE_SHFT                                    0x4
#define HWIO_RPM_APU_XPU_SCR_SEIE_BMSK                                      0x8
#define HWIO_RPM_APU_XPU_SCR_SEIE_SHFT                                      0x3
#define HWIO_RPM_APU_XPU_SCR_SCLERE_BMSK                                    0x4
#define HWIO_RPM_APU_XPU_SCR_SCLERE_SHFT                                    0x2
#define HWIO_RPM_APU_XPU_SCR_SCFGERE_BMSK                                   0x2
#define HWIO_RPM_APU_XPU_SCR_SCFGERE_SHFT                                   0x1
#define HWIO_RPM_APU_XPU_SCR_XPUNSE_BMSK                                    0x1
#define HWIO_RPM_APU_XPU_SCR_XPUNSE_SHFT                                    0x0

#define HWIO_RPM_APU_XPU_SWDR_ADDR                                   (RPM_APU_REG_BASE      + 0x00000004)
#define HWIO_RPM_APU_XPU_SWDR_RMSK                                          0x1
#define HWIO_RPM_APU_XPU_SWDR_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_SWDR_ADDR, HWIO_RPM_APU_XPU_SWDR_RMSK)
#define HWIO_RPM_APU_XPU_SWDR_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_SWDR_ADDR, m)
#define HWIO_RPM_APU_XPU_SWDR_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_SWDR_ADDR,v)
#define HWIO_RPM_APU_XPU_SWDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_SWDR_ADDR,m,v,HWIO_RPM_APU_XPU_SWDR_IN)
#define HWIO_RPM_APU_XPU_SWDR_SCFGWD_BMSK                                   0x1
#define HWIO_RPM_APU_XPU_SWDR_SCFGWD_SHFT                                   0x0

#define HWIO_RPM_APU_XPU_SEAR0_ADDR                                  (RPM_APU_REG_BASE      + 0x00000040)
#define HWIO_RPM_APU_XPU_SEAR0_RMSK                                  0xffffffff
#define HWIO_RPM_APU_XPU_SEAR0_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_SEAR0_ADDR, HWIO_RPM_APU_XPU_SEAR0_RMSK)
#define HWIO_RPM_APU_XPU_SEAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_SEAR0_ADDR, m)
#define HWIO_RPM_APU_XPU_SEAR0_PA_BMSK                               0xffffffff
#define HWIO_RPM_APU_XPU_SEAR0_PA_SHFT                                      0x0

#define HWIO_RPM_APU_XPU_SESR_ADDR                                   (RPM_APU_REG_BASE      + 0x00000048)
#define HWIO_RPM_APU_XPU_SESR_RMSK                                   0x80000003
#define HWIO_RPM_APU_XPU_SESR_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_SESR_ADDR, HWIO_RPM_APU_XPU_SESR_RMSK)
#define HWIO_RPM_APU_XPU_SESR_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_SESR_ADDR, m)
#define HWIO_RPM_APU_XPU_SESR_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_SESR_ADDR,v)
#define HWIO_RPM_APU_XPU_SESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_SESR_ADDR,m,v,HWIO_RPM_APU_XPU_SESR_IN)
#define HWIO_RPM_APU_XPU_SESR_MULTI_BMSK                             0x80000000
#define HWIO_RPM_APU_XPU_SESR_MULTI_SHFT                                   0x1f
#define HWIO_RPM_APU_XPU_SESR_CLIENT_BMSK                                   0x2
#define HWIO_RPM_APU_XPU_SESR_CLIENT_SHFT                                   0x1
#define HWIO_RPM_APU_XPU_SESR_CFG_BMSK                                      0x1
#define HWIO_RPM_APU_XPU_SESR_CFG_SHFT                                      0x0

#define HWIO_RPM_APU_XPU_SESRRESTORE_ADDR                            (RPM_APU_REG_BASE      + 0x0000004c)
#define HWIO_RPM_APU_XPU_SESRRESTORE_RMSK                            0x80000003
#define HWIO_RPM_APU_XPU_SESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_SESRRESTORE_ADDR, HWIO_RPM_APU_XPU_SESRRESTORE_RMSK)
#define HWIO_RPM_APU_XPU_SESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_SESRRESTORE_ADDR, m)
#define HWIO_RPM_APU_XPU_SESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_SESRRESTORE_ADDR,v)
#define HWIO_RPM_APU_XPU_SESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_SESRRESTORE_ADDR,m,v,HWIO_RPM_APU_XPU_SESRRESTORE_IN)
#define HWIO_RPM_APU_XPU_SESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_RPM_APU_XPU_SESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_RPM_APU_XPU_SESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_RPM_APU_XPU_SESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_RPM_APU_XPU_SESRRESTORE_CFG_BMSK                               0x1
#define HWIO_RPM_APU_XPU_SESRRESTORE_CFG_SHFT                               0x0

#define HWIO_RPM_APU_XPU_SESYNR0_ADDR                                (RPM_APU_REG_BASE      + 0x00000050)
#define HWIO_RPM_APU_XPU_SESYNR0_RMSK                                0xffffffff
#define HWIO_RPM_APU_XPU_SESYNR0_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_SESYNR0_ADDR, HWIO_RPM_APU_XPU_SESYNR0_RMSK)
#define HWIO_RPM_APU_XPU_SESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_SESYNR0_ADDR, m)
#define HWIO_RPM_APU_XPU_SESYNR0_ATID_BMSK                           0xff000000
#define HWIO_RPM_APU_XPU_SESYNR0_ATID_SHFT                                 0x18
#define HWIO_RPM_APU_XPU_SESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_RPM_APU_XPU_SESYNR0_AVMID_SHFT                                0x10
#define HWIO_RPM_APU_XPU_SESYNR0_ABID_BMSK                               0xe000
#define HWIO_RPM_APU_XPU_SESYNR0_ABID_SHFT                                  0xd
#define HWIO_RPM_APU_XPU_SESYNR0_APID_BMSK                               0x1f00
#define HWIO_RPM_APU_XPU_SESYNR0_APID_SHFT                                  0x8
#define HWIO_RPM_APU_XPU_SESYNR0_AMID_BMSK                                 0xff
#define HWIO_RPM_APU_XPU_SESYNR0_AMID_SHFT                                  0x0

#define HWIO_RPM_APU_XPU_SESYNR1_ADDR                                (RPM_APU_REG_BASE      + 0x00000054)
#define HWIO_RPM_APU_XPU_SESYNR1_RMSK                                0xffffffff
#define HWIO_RPM_APU_XPU_SESYNR1_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_SESYNR1_ADDR, HWIO_RPM_APU_XPU_SESYNR1_RMSK)
#define HWIO_RPM_APU_XPU_SESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_SESYNR1_ADDR, m)
#define HWIO_RPM_APU_XPU_SESYNR1_DCD_BMSK                            0x80000000
#define HWIO_RPM_APU_XPU_SESYNR1_DCD_SHFT                                  0x1f
#define HWIO_RPM_APU_XPU_SESYNR1_AC_BMSK                             0x40000000
#define HWIO_RPM_APU_XPU_SESYNR1_AC_SHFT                                   0x1e
#define HWIO_RPM_APU_XPU_SESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_RPM_APU_XPU_SESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_RPM_APU_XPU_SESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_RPM_APU_XPU_SESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_RPM_APU_XPU_SESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_RPM_APU_XPU_SESYNR1_ABURST_SHFT                               0x1b
#define HWIO_RPM_APU_XPU_SESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_RPM_APU_XPU_SESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_RPM_APU_XPU_SESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_RPM_APU_XPU_SESYNR1_AWRITE_SHFT                               0x19
#define HWIO_RPM_APU_XPU_SESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_RPM_APU_XPU_SESYNR1_AFULL_SHFT                                0x18
#define HWIO_RPM_APU_XPU_SESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_RPM_APU_XPU_SESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_RPM_APU_XPU_SESYNR1_AOOO_BMSK                             0x400000
#define HWIO_RPM_APU_XPU_SESYNR1_AOOO_SHFT                                 0x16
#define HWIO_RPM_APU_XPU_SESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_RPM_APU_XPU_SESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_RPM_APU_XPU_SESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_RPM_APU_XPU_SESYNR1_ASIZE_SHFT                                0x10
#define HWIO_RPM_APU_XPU_SESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_RPM_APU_XPU_SESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_RPM_APU_XPU_SESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_RPM_APU_XPU_SESYNR1_ALEN_SHFT                                  0x8
#define HWIO_RPM_APU_XPU_SESYNR1_AINST_BMSK                                0x80
#define HWIO_RPM_APU_XPU_SESYNR1_AINST_SHFT                                 0x7
#define HWIO_RPM_APU_XPU_SESYNR1_APROTNS_BMSK                              0x40
#define HWIO_RPM_APU_XPU_SESYNR1_APROTNS_SHFT                               0x6
#define HWIO_RPM_APU_XPU_SESYNR1_APRIV_BMSK                                0x20
#define HWIO_RPM_APU_XPU_SESYNR1_APRIV_SHFT                                 0x5
#define HWIO_RPM_APU_XPU_SESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_RPM_APU_XPU_SESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_RPM_APU_XPU_SESYNR1_ASHARED_BMSK                               0x8
#define HWIO_RPM_APU_XPU_SESYNR1_ASHARED_SHFT                               0x3
#define HWIO_RPM_APU_XPU_SESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_RPM_APU_XPU_SESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_RPM_APU_XPU_SESYNR2_ADDR                                (RPM_APU_REG_BASE      + 0x00000058)
#define HWIO_RPM_APU_XPU_SESYNR2_RMSK                                       0x7
#define HWIO_RPM_APU_XPU_SESYNR2_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_SESYNR2_ADDR, HWIO_RPM_APU_XPU_SESYNR2_RMSK)
#define HWIO_RPM_APU_XPU_SESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_SESYNR2_ADDR, m)
#define HWIO_RPM_APU_XPU_SESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_RPM_APU_XPU_SESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_RPM_APU_XPU_SESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_RPM_APU_XPU_SESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_RPM_APU_XPU_SESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_RPM_APU_XPU_SESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_RPM_APU_XPU_MCR_ADDR                                    (RPM_APU_REG_BASE      + 0x00000100)
#define HWIO_RPM_APU_XPU_MCR_RMSK                                         0x11f
#define HWIO_RPM_APU_XPU_MCR_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_MCR_ADDR, HWIO_RPM_APU_XPU_MCR_RMSK)
#define HWIO_RPM_APU_XPU_MCR_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_MCR_ADDR, m)
#define HWIO_RPM_APU_XPU_MCR_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_MCR_ADDR,v)
#define HWIO_RPM_APU_XPU_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_MCR_ADDR,m,v,HWIO_RPM_APU_XPU_MCR_IN)
#define HWIO_RPM_APU_XPU_MCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_RPM_APU_XPU_MCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_RPM_APU_XPU_MCR_DCDEE_BMSK                                    0x10
#define HWIO_RPM_APU_XPU_MCR_DCDEE_SHFT                                     0x4
#define HWIO_RPM_APU_XPU_MCR_EIE_BMSK                                       0x8
#define HWIO_RPM_APU_XPU_MCR_EIE_SHFT                                       0x3
#define HWIO_RPM_APU_XPU_MCR_CLERE_BMSK                                     0x4
#define HWIO_RPM_APU_XPU_MCR_CLERE_SHFT                                     0x2
#define HWIO_RPM_APU_XPU_MCR_CFGERE_BMSK                                    0x2
#define HWIO_RPM_APU_XPU_MCR_CFGERE_SHFT                                    0x1
#define HWIO_RPM_APU_XPU_MCR_XPUMSAE_BMSK                                   0x1
#define HWIO_RPM_APU_XPU_MCR_XPUMSAE_SHFT                                   0x0

#define HWIO_RPM_APU_XPU_MEAR0_ADDR                                  (RPM_APU_REG_BASE      + 0x00000140)
#define HWIO_RPM_APU_XPU_MEAR0_RMSK                                  0xffffffff
#define HWIO_RPM_APU_XPU_MEAR0_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_MEAR0_ADDR, HWIO_RPM_APU_XPU_MEAR0_RMSK)
#define HWIO_RPM_APU_XPU_MEAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_MEAR0_ADDR, m)
#define HWIO_RPM_APU_XPU_MEAR0_PA_BMSK                               0xffffffff
#define HWIO_RPM_APU_XPU_MEAR0_PA_SHFT                                      0x0

#define HWIO_RPM_APU_XPU_MESR_ADDR                                   (RPM_APU_REG_BASE      + 0x00000148)
#define HWIO_RPM_APU_XPU_MESR_RMSK                                   0x80000003
#define HWIO_RPM_APU_XPU_MESR_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_MESR_ADDR, HWIO_RPM_APU_XPU_MESR_RMSK)
#define HWIO_RPM_APU_XPU_MESR_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_MESR_ADDR, m)
#define HWIO_RPM_APU_XPU_MESR_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_MESR_ADDR,v)
#define HWIO_RPM_APU_XPU_MESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_MESR_ADDR,m,v,HWIO_RPM_APU_XPU_MESR_IN)
#define HWIO_RPM_APU_XPU_MESR_MULTI_BMSK                             0x80000000
#define HWIO_RPM_APU_XPU_MESR_MULTI_SHFT                                   0x1f
#define HWIO_RPM_APU_XPU_MESR_CLIENT_BMSK                                   0x2
#define HWIO_RPM_APU_XPU_MESR_CLIENT_SHFT                                   0x1
#define HWIO_RPM_APU_XPU_MESR_CFG_BMSK                                      0x1
#define HWIO_RPM_APU_XPU_MESR_CFG_SHFT                                      0x0

#define HWIO_RPM_APU_XPU_MESRRESTORE_ADDR                            (RPM_APU_REG_BASE      + 0x0000014c)
#define HWIO_RPM_APU_XPU_MESRRESTORE_RMSK                            0x80000003
#define HWIO_RPM_APU_XPU_MESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_MESRRESTORE_ADDR, HWIO_RPM_APU_XPU_MESRRESTORE_RMSK)
#define HWIO_RPM_APU_XPU_MESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_MESRRESTORE_ADDR, m)
#define HWIO_RPM_APU_XPU_MESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_MESRRESTORE_ADDR,v)
#define HWIO_RPM_APU_XPU_MESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_MESRRESTORE_ADDR,m,v,HWIO_RPM_APU_XPU_MESRRESTORE_IN)
#define HWIO_RPM_APU_XPU_MESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_RPM_APU_XPU_MESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_RPM_APU_XPU_MESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_RPM_APU_XPU_MESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_RPM_APU_XPU_MESRRESTORE_CFG_BMSK                               0x1
#define HWIO_RPM_APU_XPU_MESRRESTORE_CFG_SHFT                               0x0

#define HWIO_RPM_APU_XPU_MESYNR0_ADDR                                (RPM_APU_REG_BASE      + 0x00000150)
#define HWIO_RPM_APU_XPU_MESYNR0_RMSK                                0xffffffff
#define HWIO_RPM_APU_XPU_MESYNR0_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_MESYNR0_ADDR, HWIO_RPM_APU_XPU_MESYNR0_RMSK)
#define HWIO_RPM_APU_XPU_MESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_MESYNR0_ADDR, m)
#define HWIO_RPM_APU_XPU_MESYNR0_ATID_BMSK                           0xff000000
#define HWIO_RPM_APU_XPU_MESYNR0_ATID_SHFT                                 0x18
#define HWIO_RPM_APU_XPU_MESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_RPM_APU_XPU_MESYNR0_AVMID_SHFT                                0x10
#define HWIO_RPM_APU_XPU_MESYNR0_ABID_BMSK                               0xe000
#define HWIO_RPM_APU_XPU_MESYNR0_ABID_SHFT                                  0xd
#define HWIO_RPM_APU_XPU_MESYNR0_APID_BMSK                               0x1f00
#define HWIO_RPM_APU_XPU_MESYNR0_APID_SHFT                                  0x8
#define HWIO_RPM_APU_XPU_MESYNR0_AMID_BMSK                                 0xff
#define HWIO_RPM_APU_XPU_MESYNR0_AMID_SHFT                                  0x0

#define HWIO_RPM_APU_XPU_MESYNR1_ADDR                                (RPM_APU_REG_BASE      + 0x00000154)
#define HWIO_RPM_APU_XPU_MESYNR1_RMSK                                0xffffffff
#define HWIO_RPM_APU_XPU_MESYNR1_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_MESYNR1_ADDR, HWIO_RPM_APU_XPU_MESYNR1_RMSK)
#define HWIO_RPM_APU_XPU_MESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_MESYNR1_ADDR, m)
#define HWIO_RPM_APU_XPU_MESYNR1_DCD_BMSK                            0x80000000
#define HWIO_RPM_APU_XPU_MESYNR1_DCD_SHFT                                  0x1f
#define HWIO_RPM_APU_XPU_MESYNR1_AC_BMSK                             0x40000000
#define HWIO_RPM_APU_XPU_MESYNR1_AC_SHFT                                   0x1e
#define HWIO_RPM_APU_XPU_MESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_RPM_APU_XPU_MESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_RPM_APU_XPU_MESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_RPM_APU_XPU_MESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_RPM_APU_XPU_MESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_RPM_APU_XPU_MESYNR1_ABURST_SHFT                               0x1b
#define HWIO_RPM_APU_XPU_MESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_RPM_APU_XPU_MESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_RPM_APU_XPU_MESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_RPM_APU_XPU_MESYNR1_AWRITE_SHFT                               0x19
#define HWIO_RPM_APU_XPU_MESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_RPM_APU_XPU_MESYNR1_AFULL_SHFT                                0x18
#define HWIO_RPM_APU_XPU_MESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_RPM_APU_XPU_MESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_RPM_APU_XPU_MESYNR1_AOOO_BMSK                             0x400000
#define HWIO_RPM_APU_XPU_MESYNR1_AOOO_SHFT                                 0x16
#define HWIO_RPM_APU_XPU_MESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_RPM_APU_XPU_MESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_RPM_APU_XPU_MESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_RPM_APU_XPU_MESYNR1_ASIZE_SHFT                                0x10
#define HWIO_RPM_APU_XPU_MESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_RPM_APU_XPU_MESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_RPM_APU_XPU_MESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_RPM_APU_XPU_MESYNR1_ALEN_SHFT                                  0x8
#define HWIO_RPM_APU_XPU_MESYNR1_AINST_BMSK                                0x80
#define HWIO_RPM_APU_XPU_MESYNR1_AINST_SHFT                                 0x7
#define HWIO_RPM_APU_XPU_MESYNR1_APROTNS_BMSK                              0x40
#define HWIO_RPM_APU_XPU_MESYNR1_APROTNS_SHFT                               0x6
#define HWIO_RPM_APU_XPU_MESYNR1_APRIV_BMSK                                0x20
#define HWIO_RPM_APU_XPU_MESYNR1_APRIV_SHFT                                 0x5
#define HWIO_RPM_APU_XPU_MESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_RPM_APU_XPU_MESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_RPM_APU_XPU_MESYNR1_ASHARED_BMSK                               0x8
#define HWIO_RPM_APU_XPU_MESYNR1_ASHARED_SHFT                               0x3
#define HWIO_RPM_APU_XPU_MESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_RPM_APU_XPU_MESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_RPM_APU_XPU_MESYNR2_ADDR                                (RPM_APU_REG_BASE      + 0x00000158)
#define HWIO_RPM_APU_XPU_MESYNR2_RMSK                                       0x7
#define HWIO_RPM_APU_XPU_MESYNR2_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_MESYNR2_ADDR, HWIO_RPM_APU_XPU_MESYNR2_RMSK)
#define HWIO_RPM_APU_XPU_MESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_MESYNR2_ADDR, m)
#define HWIO_RPM_APU_XPU_MESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_RPM_APU_XPU_MESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_RPM_APU_XPU_MESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_RPM_APU_XPU_MESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_RPM_APU_XPU_MESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_RPM_APU_XPU_MESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_RPM_APU_XPU_CR_ADDR                                     (RPM_APU_REG_BASE      + 0x00000080)
#define HWIO_RPM_APU_XPU_CR_RMSK                                          0x11f
#define HWIO_RPM_APU_XPU_CR_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_CR_ADDR, HWIO_RPM_APU_XPU_CR_RMSK)
#define HWIO_RPM_APU_XPU_CR_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_CR_ADDR, m)
#define HWIO_RPM_APU_XPU_CR_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_CR_ADDR,v)
#define HWIO_RPM_APU_XPU_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_CR_ADDR,m,v,HWIO_RPM_APU_XPU_CR_IN)
#define HWIO_RPM_APU_XPU_CR_DYNAMIC_CLK_EN_BMSK                           0x100
#define HWIO_RPM_APU_XPU_CR_DYNAMIC_CLK_EN_SHFT                             0x8
#define HWIO_RPM_APU_XPU_CR_DCDEE_BMSK                                     0x10
#define HWIO_RPM_APU_XPU_CR_DCDEE_SHFT                                      0x4
#define HWIO_RPM_APU_XPU_CR_EIE_BMSK                                        0x8
#define HWIO_RPM_APU_XPU_CR_EIE_SHFT                                        0x3
#define HWIO_RPM_APU_XPU_CR_CLERE_BMSK                                      0x4
#define HWIO_RPM_APU_XPU_CR_CLERE_SHFT                                      0x2
#define HWIO_RPM_APU_XPU_CR_CFGERE_BMSK                                     0x2
#define HWIO_RPM_APU_XPU_CR_CFGERE_SHFT                                     0x1
#define HWIO_RPM_APU_XPU_CR_XPUVMIDE_BMSK                                   0x1
#define HWIO_RPM_APU_XPU_CR_XPUVMIDE_SHFT                                   0x0

#define HWIO_RPM_APU_XPU_RPU_ACRn_ADDR(n)                            (RPM_APU_REG_BASE      + 0x000000a0 + 0x4 * (n))
#define HWIO_RPM_APU_XPU_RPU_ACRn_RMSK                               0xffffffff
#define HWIO_RPM_APU_XPU_RPU_ACRn_MAXn                                        0
#define HWIO_RPM_APU_XPU_RPU_ACRn_INI(n)        \
        in_dword_masked(HWIO_RPM_APU_XPU_RPU_ACRn_ADDR(n), HWIO_RPM_APU_XPU_RPU_ACRn_RMSK)
#define HWIO_RPM_APU_XPU_RPU_ACRn_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_APU_XPU_RPU_ACRn_ADDR(n), mask)
#define HWIO_RPM_APU_XPU_RPU_ACRn_OUTI(n,val)    \
        out_dword(HWIO_RPM_APU_XPU_RPU_ACRn_ADDR(n),val)
#define HWIO_RPM_APU_XPU_RPU_ACRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_RPU_ACRn_ADDR(n),mask,val,HWIO_RPM_APU_XPU_RPU_ACRn_INI(n))
#define HWIO_RPM_APU_XPU_RPU_ACRn_RWE_BMSK                           0xffffffff
#define HWIO_RPM_APU_XPU_RPU_ACRn_RWE_SHFT                                  0x0

#define HWIO_RPM_APU_XPU_EAR0_ADDR                                   (RPM_APU_REG_BASE      + 0x000000c0)
#define HWIO_RPM_APU_XPU_EAR0_RMSK                                   0xffffffff
#define HWIO_RPM_APU_XPU_EAR0_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_EAR0_ADDR, HWIO_RPM_APU_XPU_EAR0_RMSK)
#define HWIO_RPM_APU_XPU_EAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_EAR0_ADDR, m)
#define HWIO_RPM_APU_XPU_EAR0_PA_BMSK                                0xffffffff
#define HWIO_RPM_APU_XPU_EAR0_PA_SHFT                                       0x0

#define HWIO_RPM_APU_XPU_ESR_ADDR                                    (RPM_APU_REG_BASE      + 0x000000c8)
#define HWIO_RPM_APU_XPU_ESR_RMSK                                    0x80000003
#define HWIO_RPM_APU_XPU_ESR_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_ESR_ADDR, HWIO_RPM_APU_XPU_ESR_RMSK)
#define HWIO_RPM_APU_XPU_ESR_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_ESR_ADDR, m)
#define HWIO_RPM_APU_XPU_ESR_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_ESR_ADDR,v)
#define HWIO_RPM_APU_XPU_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_ESR_ADDR,m,v,HWIO_RPM_APU_XPU_ESR_IN)
#define HWIO_RPM_APU_XPU_ESR_MULTI_BMSK                              0x80000000
#define HWIO_RPM_APU_XPU_ESR_MULTI_SHFT                                    0x1f
#define HWIO_RPM_APU_XPU_ESR_CLIENT_BMSK                                    0x2
#define HWIO_RPM_APU_XPU_ESR_CLIENT_SHFT                                    0x1
#define HWIO_RPM_APU_XPU_ESR_CFG_BMSK                                       0x1
#define HWIO_RPM_APU_XPU_ESR_CFG_SHFT                                       0x0

#define HWIO_RPM_APU_XPU_ESRRESTORE_ADDR                             (RPM_APU_REG_BASE      + 0x000000cc)
#define HWIO_RPM_APU_XPU_ESRRESTORE_RMSK                             0x80000003
#define HWIO_RPM_APU_XPU_ESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_ESRRESTORE_ADDR, HWIO_RPM_APU_XPU_ESRRESTORE_RMSK)
#define HWIO_RPM_APU_XPU_ESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_ESRRESTORE_ADDR, m)
#define HWIO_RPM_APU_XPU_ESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_APU_XPU_ESRRESTORE_ADDR,v)
#define HWIO_RPM_APU_XPU_ESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_ESRRESTORE_ADDR,m,v,HWIO_RPM_APU_XPU_ESRRESTORE_IN)
#define HWIO_RPM_APU_XPU_ESRRESTORE_MULTI_BMSK                       0x80000000
#define HWIO_RPM_APU_XPU_ESRRESTORE_MULTI_SHFT                             0x1f
#define HWIO_RPM_APU_XPU_ESRRESTORE_CLIENT_BMSK                             0x2
#define HWIO_RPM_APU_XPU_ESRRESTORE_CLIENT_SHFT                             0x1
#define HWIO_RPM_APU_XPU_ESRRESTORE_CFG_BMSK                                0x1
#define HWIO_RPM_APU_XPU_ESRRESTORE_CFG_SHFT                                0x0

#define HWIO_RPM_APU_XPU_ESYNR0_ADDR                                 (RPM_APU_REG_BASE      + 0x000000d0)
#define HWIO_RPM_APU_XPU_ESYNR0_RMSK                                 0xffffffff
#define HWIO_RPM_APU_XPU_ESYNR0_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_ESYNR0_ADDR, HWIO_RPM_APU_XPU_ESYNR0_RMSK)
#define HWIO_RPM_APU_XPU_ESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_ESYNR0_ADDR, m)
#define HWIO_RPM_APU_XPU_ESYNR0_ATID_BMSK                            0xff000000
#define HWIO_RPM_APU_XPU_ESYNR0_ATID_SHFT                                  0x18
#define HWIO_RPM_APU_XPU_ESYNR0_AVMID_BMSK                             0xff0000
#define HWIO_RPM_APU_XPU_ESYNR0_AVMID_SHFT                                 0x10
#define HWIO_RPM_APU_XPU_ESYNR0_ABID_BMSK                                0xe000
#define HWIO_RPM_APU_XPU_ESYNR0_ABID_SHFT                                   0xd
#define HWIO_RPM_APU_XPU_ESYNR0_APID_BMSK                                0x1f00
#define HWIO_RPM_APU_XPU_ESYNR0_APID_SHFT                                   0x8
#define HWIO_RPM_APU_XPU_ESYNR0_AMID_BMSK                                  0xff
#define HWIO_RPM_APU_XPU_ESYNR0_AMID_SHFT                                   0x0

#define HWIO_RPM_APU_XPU_ESYNR1_ADDR                                 (RPM_APU_REG_BASE      + 0x000000d4)
#define HWIO_RPM_APU_XPU_ESYNR1_RMSK                                 0xffffffff
#define HWIO_RPM_APU_XPU_ESYNR1_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_ESYNR1_ADDR, HWIO_RPM_APU_XPU_ESYNR1_RMSK)
#define HWIO_RPM_APU_XPU_ESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_ESYNR1_ADDR, m)
#define HWIO_RPM_APU_XPU_ESYNR1_DCD_BMSK                             0x80000000
#define HWIO_RPM_APU_XPU_ESYNR1_DCD_SHFT                                   0x1f
#define HWIO_RPM_APU_XPU_ESYNR1_AC_BMSK                              0x40000000
#define HWIO_RPM_APU_XPU_ESYNR1_AC_SHFT                                    0x1e
#define HWIO_RPM_APU_XPU_ESYNR1_BURSTLEN_BMSK                        0x20000000
#define HWIO_RPM_APU_XPU_ESYNR1_BURSTLEN_SHFT                              0x1d
#define HWIO_RPM_APU_XPU_ESYNR1_ARDALLOCATE_BMSK                     0x10000000
#define HWIO_RPM_APU_XPU_ESYNR1_ARDALLOCATE_SHFT                           0x1c
#define HWIO_RPM_APU_XPU_ESYNR1_ABURST_BMSK                           0x8000000
#define HWIO_RPM_APU_XPU_ESYNR1_ABURST_SHFT                                0x1b
#define HWIO_RPM_APU_XPU_ESYNR1_AEXCLUSIVE_BMSK                       0x4000000
#define HWIO_RPM_APU_XPU_ESYNR1_AEXCLUSIVE_SHFT                            0x1a
#define HWIO_RPM_APU_XPU_ESYNR1_AWRITE_BMSK                           0x2000000
#define HWIO_RPM_APU_XPU_ESYNR1_AWRITE_SHFT                                0x19
#define HWIO_RPM_APU_XPU_ESYNR1_AFULL_BMSK                            0x1000000
#define HWIO_RPM_APU_XPU_ESYNR1_AFULL_SHFT                                 0x18
#define HWIO_RPM_APU_XPU_ESYNR1_ARDBEADNDXEN_BMSK                      0x800000
#define HWIO_RPM_APU_XPU_ESYNR1_ARDBEADNDXEN_SHFT                          0x17
#define HWIO_RPM_APU_XPU_ESYNR1_AOOO_BMSK                              0x400000
#define HWIO_RPM_APU_XPU_ESYNR1_AOOO_SHFT                                  0x16
#define HWIO_RPM_APU_XPU_ESYNR1_APREQPRIORITY_BMSK                     0x380000
#define HWIO_RPM_APU_XPU_ESYNR1_APREQPRIORITY_SHFT                         0x13
#define HWIO_RPM_APU_XPU_ESYNR1_ASIZE_BMSK                              0x70000
#define HWIO_RPM_APU_XPU_ESYNR1_ASIZE_SHFT                                 0x10
#define HWIO_RPM_APU_XPU_ESYNR1_AMSSSELFAUTH_BMSK                        0x8000
#define HWIO_RPM_APU_XPU_ESYNR1_AMSSSELFAUTH_SHFT                           0xf
#define HWIO_RPM_APU_XPU_ESYNR1_ALEN_BMSK                                0x7f00
#define HWIO_RPM_APU_XPU_ESYNR1_ALEN_SHFT                                   0x8
#define HWIO_RPM_APU_XPU_ESYNR1_AINST_BMSK                                 0x80
#define HWIO_RPM_APU_XPU_ESYNR1_AINST_SHFT                                  0x7
#define HWIO_RPM_APU_XPU_ESYNR1_APROTNS_BMSK                               0x40
#define HWIO_RPM_APU_XPU_ESYNR1_APROTNS_SHFT                                0x6
#define HWIO_RPM_APU_XPU_ESYNR1_APRIV_BMSK                                 0x20
#define HWIO_RPM_APU_XPU_ESYNR1_APRIV_SHFT                                  0x5
#define HWIO_RPM_APU_XPU_ESYNR1_AINNERSHARED_BMSK                          0x10
#define HWIO_RPM_APU_XPU_ESYNR1_AINNERSHARED_SHFT                           0x4
#define HWIO_RPM_APU_XPU_ESYNR1_ASHARED_BMSK                                0x8
#define HWIO_RPM_APU_XPU_ESYNR1_ASHARED_SHFT                                0x3
#define HWIO_RPM_APU_XPU_ESYNR1_AMEMTYPE_BMSK                               0x7
#define HWIO_RPM_APU_XPU_ESYNR1_AMEMTYPE_SHFT                               0x0

#define HWIO_RPM_APU_XPU_ESYNR2_ADDR                                 (RPM_APU_REG_BASE      + 0x000000d8)
#define HWIO_RPM_APU_XPU_ESYNR2_RMSK                                        0x7
#define HWIO_RPM_APU_XPU_ESYNR2_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_ESYNR2_ADDR, HWIO_RPM_APU_XPU_ESYNR2_RMSK)
#define HWIO_RPM_APU_XPU_ESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_ESYNR2_ADDR, m)
#define HWIO_RPM_APU_XPU_ESYNR2_MODEM_PRT_HIT_BMSK                          0x4
#define HWIO_RPM_APU_XPU_ESYNR2_MODEM_PRT_HIT_SHFT                          0x2
#define HWIO_RPM_APU_XPU_ESYNR2_SECURE_PRT_HIT_BMSK                         0x2
#define HWIO_RPM_APU_XPU_ESYNR2_SECURE_PRT_HIT_SHFT                         0x1
#define HWIO_RPM_APU_XPU_ESYNR2_NONSECURE_PRT_HIT_BMSK                      0x1
#define HWIO_RPM_APU_XPU_ESYNR2_NONSECURE_PRT_HIT_SHFT                      0x0

#define HWIO_RPM_APU_XPU_IDR0_ADDR                                   (RPM_APU_REG_BASE      + 0x00000074)
#define HWIO_RPM_APU_XPU_IDR0_RMSK                                   0xc000bfff
#define HWIO_RPM_APU_XPU_IDR0_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_IDR0_ADDR, HWIO_RPM_APU_XPU_IDR0_RMSK)
#define HWIO_RPM_APU_XPU_IDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_IDR0_ADDR, m)
#define HWIO_RPM_APU_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_BMSK          0x80000000
#define HWIO_RPM_APU_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_SHFT                0x1f
#define HWIO_RPM_APU_XPU_IDR0_SAVERESTORE_HW_EN_BMSK                 0x40000000
#define HWIO_RPM_APU_XPU_IDR0_SAVERESTORE_HW_EN_SHFT                       0x1e
#define HWIO_RPM_APU_XPU_IDR0_BLED_BMSK                                  0x8000
#define HWIO_RPM_APU_XPU_IDR0_BLED_SHFT                                     0xf
#define HWIO_RPM_APU_XPU_IDR0_XPUT_BMSK                                  0x3000
#define HWIO_RPM_APU_XPU_IDR0_XPUT_SHFT                                     0xc
#define HWIO_RPM_APU_XPU_IDR0_PT_BMSK                                     0x800
#define HWIO_RPM_APU_XPU_IDR0_PT_SHFT                                       0xb
#define HWIO_RPM_APU_XPU_IDR0_MV_BMSK                                     0x400
#define HWIO_RPM_APU_XPU_IDR0_MV_SHFT                                       0xa
#define HWIO_RPM_APU_XPU_IDR0_NRG_BMSK                                    0x3ff
#define HWIO_RPM_APU_XPU_IDR0_NRG_SHFT                                      0x0

#define HWIO_RPM_APU_XPU_IDR1_ADDR                                   (RPM_APU_REG_BASE      + 0x00000078)
#define HWIO_RPM_APU_XPU_IDR1_RMSK                                   0x7f3ffeff
#define HWIO_RPM_APU_XPU_IDR1_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_IDR1_ADDR, HWIO_RPM_APU_XPU_IDR1_RMSK)
#define HWIO_RPM_APU_XPU_IDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_IDR1_ADDR, m)
#define HWIO_RPM_APU_XPU_IDR1_AMT_HW_ENABLE_BMSK                     0x40000000
#define HWIO_RPM_APU_XPU_IDR1_AMT_HW_ENABLE_SHFT                           0x1e
#define HWIO_RPM_APU_XPU_IDR1_CLIENT_ADDR_WIDTH_BMSK                 0x3f000000
#define HWIO_RPM_APU_XPU_IDR1_CLIENT_ADDR_WIDTH_SHFT                       0x18
#define HWIO_RPM_APU_XPU_IDR1_CONFIG_ADDR_WIDTH_BMSK                   0x3f0000
#define HWIO_RPM_APU_XPU_IDR1_CONFIG_ADDR_WIDTH_SHFT                       0x10
#define HWIO_RPM_APU_XPU_IDR1_QRIB_EN_BMSK                               0x8000
#define HWIO_RPM_APU_XPU_IDR1_QRIB_EN_SHFT                                  0xf
#define HWIO_RPM_APU_XPU_IDR1_ASYNC_MODE_BMSK                            0x4000
#define HWIO_RPM_APU_XPU_IDR1_ASYNC_MODE_SHFT                               0xe
#define HWIO_RPM_APU_XPU_IDR1_CONFIG_TYPE_BMSK                           0x2000
#define HWIO_RPM_APU_XPU_IDR1_CONFIG_TYPE_SHFT                              0xd
#define HWIO_RPM_APU_XPU_IDR1_CLIENT_PIPELINE_ENABLED_BMSK               0x1000
#define HWIO_RPM_APU_XPU_IDR1_CLIENT_PIPELINE_ENABLED_SHFT                  0xc
#define HWIO_RPM_APU_XPU_IDR1_MSA_CHECK_HW_ENABLE_BMSK                    0x800
#define HWIO_RPM_APU_XPU_IDR1_MSA_CHECK_HW_ENABLE_SHFT                      0xb
#define HWIO_RPM_APU_XPU_IDR1_XPU_SYND_REG_ABSENT_BMSK                    0x400
#define HWIO_RPM_APU_XPU_IDR1_XPU_SYND_REG_ABSENT_SHFT                      0xa
#define HWIO_RPM_APU_XPU_IDR1_TZXPU_BMSK                                  0x200
#define HWIO_RPM_APU_XPU_IDR1_TZXPU_SHFT                                    0x9
#define HWIO_RPM_APU_XPU_IDR1_NVMID_BMSK                                   0xff
#define HWIO_RPM_APU_XPU_IDR1_NVMID_SHFT                                    0x0

#define HWIO_RPM_APU_XPU_REV_ADDR                                    (RPM_APU_REG_BASE      + 0x0000007c)
#define HWIO_RPM_APU_XPU_REV_RMSK                                    0xffffffff
#define HWIO_RPM_APU_XPU_REV_IN          \
        in_dword_masked(HWIO_RPM_APU_XPU_REV_ADDR, HWIO_RPM_APU_XPU_REV_RMSK)
#define HWIO_RPM_APU_XPU_REV_INM(m)      \
        in_dword_masked(HWIO_RPM_APU_XPU_REV_ADDR, m)
#define HWIO_RPM_APU_XPU_REV_MAJOR_BMSK                              0xf0000000
#define HWIO_RPM_APU_XPU_REV_MAJOR_SHFT                                    0x1c
#define HWIO_RPM_APU_XPU_REV_MINOR_BMSK                               0xfff0000
#define HWIO_RPM_APU_XPU_REV_MINOR_SHFT                                    0x10
#define HWIO_RPM_APU_XPU_REV_STEP_BMSK                                   0xffff
#define HWIO_RPM_APU_XPU_REV_STEP_SHFT                                      0x0

#define HWIO_RPM_APU_XPU_RGn_RACRm_ADDR(n,m)                         (RPM_APU_REG_BASE      + 0x00000200 + 0x80 * (n)+0x4 * (m))
#define HWIO_RPM_APU_XPU_RGn_RACRm_RMSK                              0xffffffff
#define HWIO_RPM_APU_XPU_RGn_RACRm_MAXn                                       0
#define HWIO_RPM_APU_XPU_RGn_RACRm_MAXm                                       0
#define HWIO_RPM_APU_XPU_RGn_RACRm_INI2(n,m)        \
        in_dword_masked(HWIO_RPM_APU_XPU_RGn_RACRm_ADDR(n,m), HWIO_RPM_APU_XPU_RGn_RACRm_RMSK)
#define HWIO_RPM_APU_XPU_RGn_RACRm_INMI2(n,m,mask)    \
        in_dword_masked(HWIO_RPM_APU_XPU_RGn_RACRm_ADDR(n,m), mask)
#define HWIO_RPM_APU_XPU_RGn_RACRm_OUTI2(n,m,val)    \
        out_dword(HWIO_RPM_APU_XPU_RGn_RACRm_ADDR(n,m),val)
#define HWIO_RPM_APU_XPU_RGn_RACRm_OUTMI2(n,m,mask,val) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_RGn_RACRm_ADDR(n,m),mask,val,HWIO_RPM_APU_XPU_RGn_RACRm_INI2(n,m))
#define HWIO_RPM_APU_XPU_RGn_RACRm_RE_BMSK                           0xffffffff
#define HWIO_RPM_APU_XPU_RGn_RACRm_RE_SHFT                                  0x0

#define HWIO_RPM_APU_XPU_RGn_WACRm_ADDR(n,m)                         (RPM_APU_REG_BASE      + 0x00000220 + 0x80 * (n)+0x4 * (m))
#define HWIO_RPM_APU_XPU_RGn_WACRm_RMSK                              0xffffffff
#define HWIO_RPM_APU_XPU_RGn_WACRm_MAXn                                       0
#define HWIO_RPM_APU_XPU_RGn_WACRm_MAXm                                       0
#define HWIO_RPM_APU_XPU_RGn_WACRm_INI2(n,m)        \
        in_dword_masked(HWIO_RPM_APU_XPU_RGn_WACRm_ADDR(n,m), HWIO_RPM_APU_XPU_RGn_WACRm_RMSK)
#define HWIO_RPM_APU_XPU_RGn_WACRm_INMI2(n,m,mask)    \
        in_dword_masked(HWIO_RPM_APU_XPU_RGn_WACRm_ADDR(n,m), mask)
#define HWIO_RPM_APU_XPU_RGn_WACRm_OUTI2(n,m,val)    \
        out_dword(HWIO_RPM_APU_XPU_RGn_WACRm_ADDR(n,m),val)
#define HWIO_RPM_APU_XPU_RGn_WACRm_OUTMI2(n,m,mask,val) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_RGn_WACRm_ADDR(n,m),mask,val,HWIO_RPM_APU_XPU_RGn_WACRm_INI2(n,m))
#define HWIO_RPM_APU_XPU_RGn_WACRm_WE_BMSK                           0xffffffff
#define HWIO_RPM_APU_XPU_RGn_WACRm_WE_SHFT                                  0x0

#define HWIO_RPM_APU_XPU_RGn_SCR_ADDR(n)                             (RPM_APU_REG_BASE      + 0x00000250 + 0x80 * (n))
#define HWIO_RPM_APU_XPU_RGn_SCR_RMSK                                       0x1
#define HWIO_RPM_APU_XPU_RGn_SCR_MAXn                                         0
#define HWIO_RPM_APU_XPU_RGn_SCR_INI(n)        \
        in_dword_masked(HWIO_RPM_APU_XPU_RGn_SCR_ADDR(n), HWIO_RPM_APU_XPU_RGn_SCR_RMSK)
#define HWIO_RPM_APU_XPU_RGn_SCR_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_APU_XPU_RGn_SCR_ADDR(n), mask)
#define HWIO_RPM_APU_XPU_RGn_SCR_OUTI(n,val)    \
        out_dword(HWIO_RPM_APU_XPU_RGn_SCR_ADDR(n),val)
#define HWIO_RPM_APU_XPU_RGn_SCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_RGn_SCR_ADDR(n),mask,val,HWIO_RPM_APU_XPU_RGn_SCR_INI(n))
#define HWIO_RPM_APU_XPU_RGn_SCR_NS_BMSK                                    0x1
#define HWIO_RPM_APU_XPU_RGn_SCR_NS_SHFT                                    0x0

#define HWIO_RPM_APU_XPU_RGn_MCR_ADDR(n)                             (RPM_APU_REG_BASE      + 0x00000254 + 0x80 * (n))
#define HWIO_RPM_APU_XPU_RGn_MCR_RMSK                                       0x7
#define HWIO_RPM_APU_XPU_RGn_MCR_MAXn                                         0
#define HWIO_RPM_APU_XPU_RGn_MCR_INI(n)        \
        in_dword_masked(HWIO_RPM_APU_XPU_RGn_MCR_ADDR(n), HWIO_RPM_APU_XPU_RGn_MCR_RMSK)
#define HWIO_RPM_APU_XPU_RGn_MCR_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_APU_XPU_RGn_MCR_ADDR(n), mask)
#define HWIO_RPM_APU_XPU_RGn_MCR_OUTI(n,val)    \
        out_dword(HWIO_RPM_APU_XPU_RGn_MCR_ADDR(n),val)
#define HWIO_RPM_APU_XPU_RGn_MCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_APU_XPU_RGn_MCR_ADDR(n),mask,val,HWIO_RPM_APU_XPU_RGn_MCR_INI(n))
#define HWIO_RPM_APU_XPU_RGn_MCR_VMIDCLE_BMSK                               0x4
#define HWIO_RPM_APU_XPU_RGn_MCR_VMIDCLE_SHFT                               0x2
#define HWIO_RPM_APU_XPU_RGn_MCR_SCLE_BMSK                                  0x2
#define HWIO_RPM_APU_XPU_RGn_MCR_SCLE_SHFT                                  0x1
#define HWIO_RPM_APU_XPU_RGn_MCR_MSAE_BMSK                                  0x1
#define HWIO_RPM_APU_XPU_RGn_MCR_MSAE_SHFT                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: RPM_DEC
 *--------------------------------------------------------------------------*/

#define RPM_DEC_REG_BASE                                                 (RPM_CSR_BASE      + 0x00000000)

#define HWIO_RPM_HW_VERSION_ADDR                                         (RPM_DEC_REG_BASE      + 0x00000000)
#define HWIO_RPM_HW_VERSION_RMSK                                         0xffffffff
#define HWIO_RPM_HW_VERSION_IN          \
        in_dword_masked(HWIO_RPM_HW_VERSION_ADDR, HWIO_RPM_HW_VERSION_RMSK)
#define HWIO_RPM_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_RPM_HW_VERSION_ADDR, m)
#define HWIO_RPM_HW_VERSION_MAJOR_BMSK                                   0xf0000000
#define HWIO_RPM_HW_VERSION_MAJOR_SHFT                                         0x1c
#define HWIO_RPM_HW_VERSION_MINOR_BMSK                                    0xfff0000
#define HWIO_RPM_HW_VERSION_MINOR_SHFT                                         0x10
#define HWIO_RPM_HW_VERSION_STEP_BMSK                                        0xffff
#define HWIO_RPM_HW_VERSION_STEP_SHFT                                           0x0

#define HWIO_RPM_WFI_CONFIG_ADDR                                         (RPM_DEC_REG_BASE      + 0x00000004)
#define HWIO_RPM_WFI_CONFIG_RMSK                                                0x7
#define HWIO_RPM_WFI_CONFIG_IN          \
        in_dword_masked(HWIO_RPM_WFI_CONFIG_ADDR, HWIO_RPM_WFI_CONFIG_RMSK)
#define HWIO_RPM_WFI_CONFIG_INM(m)      \
        in_dword_masked(HWIO_RPM_WFI_CONFIG_ADDR, m)
#define HWIO_RPM_WFI_CONFIG_OUT(v)      \
        out_dword(HWIO_RPM_WFI_CONFIG_ADDR,v)
#define HWIO_RPM_WFI_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WFI_CONFIG_ADDR,m,v,HWIO_RPM_WFI_CONFIG_IN)
#define HWIO_RPM_WFI_CONFIG_CHIP_SLEEP_UPON_WFI_BMSK                            0x4
#define HWIO_RPM_WFI_CONFIG_CHIP_SLEEP_UPON_WFI_SHFT                            0x2
#define HWIO_RPM_WFI_CONFIG_BUS_CLK_HALT_BMSK                                   0x2
#define HWIO_RPM_WFI_CONFIG_BUS_CLK_HALT_SHFT                                   0x1
#define HWIO_RPM_WFI_CONFIG_PROC_CLK_HALT_BMSK                                  0x1
#define HWIO_RPM_WFI_CONFIG_PROC_CLK_HALT_SHFT                                  0x0

#define HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR                                 (RPM_DEC_REG_BASE      + 0x00000008)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_RMSK                                        0x1
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_IN          \
        in_dword_masked(HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR, HWIO_RPM_TIMERS_CLK_OFF_CTL_RMSK)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_INM(m)      \
        in_dword_masked(HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR, m)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_OUT(v)      \
        out_dword(HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR,v)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR,m,v,HWIO_RPM_TIMERS_CLK_OFF_CTL_IN)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_WDOG_TIMER_CLK_OFF_BMSK                     0x1
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_WDOG_TIMER_CLK_OFF_SHFT                     0x0

#define HWIO_RPM_IPC_ADDR                                                (RPM_DEC_REG_BASE      + 0x0000000c)
#define HWIO_RPM_IPC_RMSK                                                0xffffffff
#define HWIO_RPM_IPC_OUT(v)      \
        out_dword(HWIO_RPM_IPC_ADDR,v)
#define HWIO_RPM_IPC_RSRV_IPC_BMSK                                       0xf0000000
#define HWIO_RPM_IPC_RSRV_IPC_SHFT                                             0x1c
#define HWIO_RPM_IPC_APCS_TZ_IPC_BMSK                                     0xf000000
#define HWIO_RPM_IPC_APCS_TZ_IPC_SHFT                                          0x18
#define HWIO_RPM_IPC_SPARE_IPC_BMSK                                        0xf00000
#define HWIO_RPM_IPC_SPARE_IPC_SHFT                                            0x14
#define HWIO_RPM_IPC_WCN_IPC_BMSK                                           0xf0000
#define HWIO_RPM_IPC_WCN_IPC_SHFT                                              0x10
#define HWIO_RPM_IPC_MPSS_IPC_BMSK                                           0xf000
#define HWIO_RPM_IPC_MPSS_IPC_SHFT                                              0xc
#define HWIO_RPM_IPC_ADSP_IPC_BMSK                                            0xf00
#define HWIO_RPM_IPC_ADSP_IPC_SHFT                                              0x8
#define HWIO_RPM_IPC_APCS_HLOS_IPC_BMSK                                        0xf0
#define HWIO_RPM_IPC_APCS_HLOS_IPC_SHFT                                         0x4
#define HWIO_RPM_IPC_RPM_RSRV_BMSK                                              0xf
#define HWIO_RPM_IPC_RPM_RSRV_SHFT                                              0x0

#define HWIO_RPM_GPO_WDATA_ADDR                                          (RPM_DEC_REG_BASE      + 0x00000010)
#define HWIO_RPM_GPO_WDATA_RMSK                                          0xffffffff
#define HWIO_RPM_GPO_WDATA_IN          \
        in_dword_masked(HWIO_RPM_GPO_WDATA_ADDR, HWIO_RPM_GPO_WDATA_RMSK)
#define HWIO_RPM_GPO_WDATA_INM(m)      \
        in_dword_masked(HWIO_RPM_GPO_WDATA_ADDR, m)
#define HWIO_RPM_GPO_WDATA_OUT(v)      \
        out_dword(HWIO_RPM_GPO_WDATA_ADDR,v)
#define HWIO_RPM_GPO_WDATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_GPO_WDATA_ADDR,m,v,HWIO_RPM_GPO_WDATA_IN)
#define HWIO_RPM_GPO_WDATA_WDATA_BMSK                                    0xffffffff
#define HWIO_RPM_GPO_WDATA_WDATA_SHFT                                           0x0

#define HWIO_RPM_GPO_WDSET_ADDR                                          (RPM_DEC_REG_BASE      + 0x00000014)
#define HWIO_RPM_GPO_WDSET_RMSK                                          0xffffffff
#define HWIO_RPM_GPO_WDSET_OUT(v)      \
        out_dword(HWIO_RPM_GPO_WDSET_ADDR,v)
#define HWIO_RPM_GPO_WDSET_WDSET_BMSK                                    0xffffffff
#define HWIO_RPM_GPO_WDSET_WDSET_SHFT                                           0x0

#define HWIO_RPM_GPO_WDCLR_ADDR                                          (RPM_DEC_REG_BASE      + 0x00000018)
#define HWIO_RPM_GPO_WDCLR_RMSK                                          0xffffffff
#define HWIO_RPM_GPO_WDCLR_OUT(v)      \
        out_dword(HWIO_RPM_GPO_WDCLR_ADDR,v)
#define HWIO_RPM_GPO_WDCLR_WDCLR_BMSK                                    0xffffffff
#define HWIO_RPM_GPO_WDCLR_WDCLR_SHFT                                           0x0

#define HWIO_RPM_SLAVES_CLK_GATING_ADDR                                  (RPM_DEC_REG_BASE      + 0x0000001c)
#define HWIO_RPM_SLAVES_CLK_GATING_RMSK                                         0xf
#define HWIO_RPM_SLAVES_CLK_GATING_IN          \
        in_dword_masked(HWIO_RPM_SLAVES_CLK_GATING_ADDR, HWIO_RPM_SLAVES_CLK_GATING_RMSK)
#define HWIO_RPM_SLAVES_CLK_GATING_INM(m)      \
        in_dword_masked(HWIO_RPM_SLAVES_CLK_GATING_ADDR, m)
#define HWIO_RPM_SLAVES_CLK_GATING_OUT(v)      \
        out_dword(HWIO_RPM_SLAVES_CLK_GATING_ADDR,v)
#define HWIO_RPM_SLAVES_CLK_GATING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_SLAVES_CLK_GATING_ADDR,m,v,HWIO_RPM_SLAVES_CLK_GATING_IN)
#define HWIO_RPM_SLAVES_CLK_GATING_INTR_CLK_GATING_BMSK                         0x8
#define HWIO_RPM_SLAVES_CLK_GATING_INTR_CLK_GATING_SHFT                         0x3
#define HWIO_RPM_SLAVES_CLK_GATING_RAM_CLK_GATING_BMSK                          0x4
#define HWIO_RPM_SLAVES_CLK_GATING_RAM_CLK_GATING_SHFT                          0x2
#define HWIO_RPM_SLAVES_CLK_GATING_PERIPH_CLK_GATING_BMSK                       0x2
#define HWIO_RPM_SLAVES_CLK_GATING_PERIPH_CLK_GATING_SHFT                       0x1
#define HWIO_RPM_SLAVES_CLK_GATING_SLP_WKUP_FSM_CLK_GATING_BMSK                 0x1
#define HWIO_RPM_SLAVES_CLK_GATING_SLP_WKUP_FSM_CLK_GATING_SHFT                 0x0

#define HWIO_RPM_BOOTROM_REMAP_ADDR_ADDR                                 (RPM_DEC_REG_BASE      + 0x00000020)
#define HWIO_RPM_BOOTROM_REMAP_ADDR_RMSK                                 0xffffffff
#define HWIO_RPM_BOOTROM_REMAP_ADDR_IN          \
        in_dword_masked(HWIO_RPM_BOOTROM_REMAP_ADDR_ADDR, HWIO_RPM_BOOTROM_REMAP_ADDR_RMSK)
#define HWIO_RPM_BOOTROM_REMAP_ADDR_INM(m)      \
        in_dword_masked(HWIO_RPM_BOOTROM_REMAP_ADDR_ADDR, m)
#define HWIO_RPM_BOOTROM_REMAP_ADDR_BOOTROM_REMAP_ADDR_BMSK              0xffffffff
#define HWIO_RPM_BOOTROM_REMAP_ADDR_BOOTROM_REMAP_ADDR_SHFT                     0x0

#define HWIO_RPM_SYSIF_REMAP_ADDR0_ADDR                                  (RPM_DEC_REG_BASE      + 0x00000024)
#define HWIO_RPM_SYSIF_REMAP_ADDR0_RMSK                                  0xffffffff
#define HWIO_RPM_SYSIF_REMAP_ADDR0_IN          \
        in_dword_masked(HWIO_RPM_SYSIF_REMAP_ADDR0_ADDR, HWIO_RPM_SYSIF_REMAP_ADDR0_RMSK)
#define HWIO_RPM_SYSIF_REMAP_ADDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_SYSIF_REMAP_ADDR0_ADDR, m)
#define HWIO_RPM_SYSIF_REMAP_ADDR0_SYSIF_REMAP_ADDR0_BMSK                0xffffffff
#define HWIO_RPM_SYSIF_REMAP_ADDR0_SYSIF_REMAP_ADDR0_SHFT                       0x0

#define HWIO_RPM_SYSIF_REMAP_ADDR1_ADDR                                  (RPM_DEC_REG_BASE      + 0x00000028)
#define HWIO_RPM_SYSIF_REMAP_ADDR1_RMSK                                  0xffffffff
#define HWIO_RPM_SYSIF_REMAP_ADDR1_IN          \
        in_dword_masked(HWIO_RPM_SYSIF_REMAP_ADDR1_ADDR, HWIO_RPM_SYSIF_REMAP_ADDR1_RMSK)
#define HWIO_RPM_SYSIF_REMAP_ADDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_SYSIF_REMAP_ADDR1_ADDR, m)
#define HWIO_RPM_SYSIF_REMAP_ADDR1_SYSIF_REMAP_ADDR1_BMSK                0xffffffff
#define HWIO_RPM_SYSIF_REMAP_ADDR1_SYSIF_REMAP_ADDR1_SHFT                       0x0

#define HWIO_RPM_SYSIF_REMAP_ADDR2_ADDR                                  (RPM_DEC_REG_BASE      + 0x0000002c)
#define HWIO_RPM_SYSIF_REMAP_ADDR2_RMSK                                  0xffffffff
#define HWIO_RPM_SYSIF_REMAP_ADDR2_IN          \
        in_dword_masked(HWIO_RPM_SYSIF_REMAP_ADDR2_ADDR, HWIO_RPM_SYSIF_REMAP_ADDR2_RMSK)
#define HWIO_RPM_SYSIF_REMAP_ADDR2_INM(m)      \
        in_dword_masked(HWIO_RPM_SYSIF_REMAP_ADDR2_ADDR, m)
#define HWIO_RPM_SYSIF_REMAP_ADDR2_SYSIF_REMAP_ADDR2_BMSK                0xffffffff
#define HWIO_RPM_SYSIF_REMAP_ADDR2_SYSIF_REMAP_ADDR2_SHFT                       0x0

#define HWIO_RPM_INTR_POLARITY_0_ADDR                                    (RPM_DEC_REG_BASE      + 0x00000030)
#define HWIO_RPM_INTR_POLARITY_0_RMSK                                    0xffffffff
#define HWIO_RPM_INTR_POLARITY_0_IN          \
        in_dword_masked(HWIO_RPM_INTR_POLARITY_0_ADDR, HWIO_RPM_INTR_POLARITY_0_RMSK)
#define HWIO_RPM_INTR_POLARITY_0_INM(m)      \
        in_dword_masked(HWIO_RPM_INTR_POLARITY_0_ADDR, m)
#define HWIO_RPM_INTR_POLARITY_0_OUT(v)      \
        out_dword(HWIO_RPM_INTR_POLARITY_0_ADDR,v)
#define HWIO_RPM_INTR_POLARITY_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_INTR_POLARITY_0_ADDR,m,v,HWIO_RPM_INTR_POLARITY_0_IN)
#define HWIO_RPM_INTR_POLARITY_0_POLARITY_BMSK                           0xffffffff
#define HWIO_RPM_INTR_POLARITY_0_POLARITY_SHFT                                  0x0

#define HWIO_RPM_INTR_POLARITY_1_ADDR                                    (RPM_DEC_REG_BASE      + 0x00000034)
#define HWIO_RPM_INTR_POLARITY_1_RMSK                                    0xffffffff
#define HWIO_RPM_INTR_POLARITY_1_IN          \
        in_dword_masked(HWIO_RPM_INTR_POLARITY_1_ADDR, HWIO_RPM_INTR_POLARITY_1_RMSK)
#define HWIO_RPM_INTR_POLARITY_1_INM(m)      \
        in_dword_masked(HWIO_RPM_INTR_POLARITY_1_ADDR, m)
#define HWIO_RPM_INTR_POLARITY_1_OUT(v)      \
        out_dword(HWIO_RPM_INTR_POLARITY_1_ADDR,v)
#define HWIO_RPM_INTR_POLARITY_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_INTR_POLARITY_1_ADDR,m,v,HWIO_RPM_INTR_POLARITY_1_IN)
#define HWIO_RPM_INTR_POLARITY_1_POLARITY_BMSK                           0xffffffff
#define HWIO_RPM_INTR_POLARITY_1_POLARITY_SHFT                                  0x0

#define HWIO_RPM_INTR_EDG_LVL_0_ADDR                                     (RPM_DEC_REG_BASE      + 0x00000038)
#define HWIO_RPM_INTR_EDG_LVL_0_RMSK                                     0xffffffff
#define HWIO_RPM_INTR_EDG_LVL_0_IN          \
        in_dword_masked(HWIO_RPM_INTR_EDG_LVL_0_ADDR, HWIO_RPM_INTR_EDG_LVL_0_RMSK)
#define HWIO_RPM_INTR_EDG_LVL_0_INM(m)      \
        in_dword_masked(HWIO_RPM_INTR_EDG_LVL_0_ADDR, m)
#define HWIO_RPM_INTR_EDG_LVL_0_OUT(v)      \
        out_dword(HWIO_RPM_INTR_EDG_LVL_0_ADDR,v)
#define HWIO_RPM_INTR_EDG_LVL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_INTR_EDG_LVL_0_ADDR,m,v,HWIO_RPM_INTR_EDG_LVL_0_IN)
#define HWIO_RPM_INTR_EDG_LVL_0_EDG_LVL_BMSK                             0xffffffff
#define HWIO_RPM_INTR_EDG_LVL_0_EDG_LVL_SHFT                                    0x0

#define HWIO_RPM_INTR_EDG_LVL_1_ADDR                                     (RPM_DEC_REG_BASE      + 0x0000003c)
#define HWIO_RPM_INTR_EDG_LVL_1_RMSK                                     0xffffffff
#define HWIO_RPM_INTR_EDG_LVL_1_IN          \
        in_dword_masked(HWIO_RPM_INTR_EDG_LVL_1_ADDR, HWIO_RPM_INTR_EDG_LVL_1_RMSK)
#define HWIO_RPM_INTR_EDG_LVL_1_INM(m)      \
        in_dword_masked(HWIO_RPM_INTR_EDG_LVL_1_ADDR, m)
#define HWIO_RPM_INTR_EDG_LVL_1_OUT(v)      \
        out_dword(HWIO_RPM_INTR_EDG_LVL_1_ADDR,v)
#define HWIO_RPM_INTR_EDG_LVL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_INTR_EDG_LVL_1_ADDR,m,v,HWIO_RPM_INTR_EDG_LVL_1_IN)
#define HWIO_RPM_INTR_EDG_LVL_1_EDG_LVL_BMSK                             0xffffffff
#define HWIO_RPM_INTR_EDG_LVL_1_EDG_LVL_SHFT                                    0x0

#define HWIO_RPM_WDOG_RESET_ADDR                                         (RPM_DEC_REG_BASE      + 0x00000040)
#define HWIO_RPM_WDOG_RESET_RMSK                                                0x3
#define HWIO_RPM_WDOG_RESET_IN          \
        in_dword_masked(HWIO_RPM_WDOG_RESET_ADDR, HWIO_RPM_WDOG_RESET_RMSK)
#define HWIO_RPM_WDOG_RESET_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_RESET_ADDR, m)
#define HWIO_RPM_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_RESET_ADDR,v)
#define HWIO_RPM_WDOG_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_RESET_ADDR,m,v,HWIO_RPM_WDOG_RESET_IN)
#define HWIO_RPM_WDOG_RESET_SYNC_STATUS_BMSK                                    0x2
#define HWIO_RPM_WDOG_RESET_SYNC_STATUS_SHFT                                    0x1
#define HWIO_RPM_WDOG_RESET_WDOG_RESET_BMSK                                     0x1
#define HWIO_RPM_WDOG_RESET_WDOG_RESET_SHFT                                     0x0

#define HWIO_RPM_WDOG_CTRL_ADDR                                          (RPM_DEC_REG_BASE      + 0x00000044)
#define HWIO_RPM_WDOG_CTRL_RMSK                                                 0x3
#define HWIO_RPM_WDOG_CTRL_IN          \
        in_dword_masked(HWIO_RPM_WDOG_CTRL_ADDR, HWIO_RPM_WDOG_CTRL_RMSK)
#define HWIO_RPM_WDOG_CTRL_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_CTRL_ADDR, m)
#define HWIO_RPM_WDOG_CTRL_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_CTRL_ADDR,v)
#define HWIO_RPM_WDOG_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_CTRL_ADDR,m,v,HWIO_RPM_WDOG_CTRL_IN)
#define HWIO_RPM_WDOG_CTRL_HW_WAKEUP_SLEEP_EN_BMSK                              0x2
#define HWIO_RPM_WDOG_CTRL_HW_WAKEUP_SLEEP_EN_SHFT                              0x1
#define HWIO_RPM_WDOG_CTRL_ENABLE_BMSK                                          0x1
#define HWIO_RPM_WDOG_CTRL_ENABLE_SHFT                                          0x0

#define HWIO_RPM_WDOG_STATUS_ADDR                                        (RPM_DEC_REG_BASE      + 0x00000048)
#define HWIO_RPM_WDOG_STATUS_RMSK                                          0x7fffff
#define HWIO_RPM_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_RPM_WDOG_STATUS_ADDR, HWIO_RPM_WDOG_STATUS_RMSK)
#define HWIO_RPM_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_STATUS_ADDR, m)
#define HWIO_RPM_WDOG_STATUS_WDOG_COUNT_BMSK                               0x7ffff8
#define HWIO_RPM_WDOG_STATUS_WDOG_COUNT_SHFT                                    0x3
#define HWIO_RPM_WDOG_STATUS_WDOG_CNT_RESET_STATUS_BMSK                         0x4
#define HWIO_RPM_WDOG_STATUS_WDOG_CNT_RESET_STATUS_SHFT                         0x2
#define HWIO_RPM_WDOG_STATUS_WDOG_FROZEN_BMSK                                   0x2
#define HWIO_RPM_WDOG_STATUS_WDOG_FROZEN_SHFT                                   0x1
#define HWIO_RPM_WDOG_STATUS_WDOG_EXPIRED_STATUS_BMSK                           0x1
#define HWIO_RPM_WDOG_STATUS_WDOG_EXPIRED_STATUS_SHFT                           0x0

#define HWIO_RPM_WDOG_BARK_TIME_ADDR                                     (RPM_DEC_REG_BASE      + 0x0000004c)
#define HWIO_RPM_WDOG_BARK_TIME_RMSK                                       0x1fffff
#define HWIO_RPM_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_RPM_WDOG_BARK_TIME_ADDR, HWIO_RPM_WDOG_BARK_TIME_RMSK)
#define HWIO_RPM_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_BARK_TIME_ADDR, m)
#define HWIO_RPM_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_BARK_TIME_ADDR,v)
#define HWIO_RPM_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_BARK_TIME_ADDR,m,v,HWIO_RPM_WDOG_BARK_TIME_IN)
#define HWIO_RPM_WDOG_BARK_TIME_SYNC_STATUS_BMSK                           0x100000
#define HWIO_RPM_WDOG_BARK_TIME_SYNC_STATUS_SHFT                               0x14
#define HWIO_RPM_WDOG_BARK_TIME_WDOG_BARK_VAL_BMSK                          0xfffff
#define HWIO_RPM_WDOG_BARK_TIME_WDOG_BARK_VAL_SHFT                              0x0

#define HWIO_RPM_WDOG_BITE_TIME_ADDR                                     (RPM_DEC_REG_BASE      + 0x00000050)
#define HWIO_RPM_WDOG_BITE_TIME_RMSK                                       0x1fffff
#define HWIO_RPM_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_RPM_WDOG_BITE_TIME_ADDR, HWIO_RPM_WDOG_BITE_TIME_RMSK)
#define HWIO_RPM_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_BITE_TIME_ADDR, m)
#define HWIO_RPM_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_BITE_TIME_ADDR,v)
#define HWIO_RPM_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_BITE_TIME_ADDR,m,v,HWIO_RPM_WDOG_BITE_TIME_IN)
#define HWIO_RPM_WDOG_BITE_TIME_SYNC_STATUS_BMSK                           0x100000
#define HWIO_RPM_WDOG_BITE_TIME_SYNC_STATUS_SHFT                               0x14
#define HWIO_RPM_WDOG_BITE_TIME_WDOG_BITE_VAL_BMSK                          0xfffff
#define HWIO_RPM_WDOG_BITE_TIME_WDOG_BITE_VAL_SHFT                              0x0

#define HWIO_RPM_WDOG_TEST_LOAD_ADDR                                     (RPM_DEC_REG_BASE      + 0x00000054)
#define HWIO_RPM_WDOG_TEST_LOAD_RMSK                                            0x3
#define HWIO_RPM_WDOG_TEST_LOAD_IN          \
        in_dword_masked(HWIO_RPM_WDOG_TEST_LOAD_ADDR, HWIO_RPM_WDOG_TEST_LOAD_RMSK)
#define HWIO_RPM_WDOG_TEST_LOAD_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_TEST_LOAD_ADDR, m)
#define HWIO_RPM_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_RPM_WDOG_TEST_LOAD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_TEST_LOAD_ADDR,m,v,HWIO_RPM_WDOG_TEST_LOAD_IN)
#define HWIO_RPM_WDOG_TEST_LOAD_SYNC_STATUS_BMSK                                0x2
#define HWIO_RPM_WDOG_TEST_LOAD_SYNC_STATUS_SHFT                                0x1
#define HWIO_RPM_WDOG_TEST_LOAD_LOAD_BMSK                                       0x1
#define HWIO_RPM_WDOG_TEST_LOAD_LOAD_SHFT                                       0x0

#define HWIO_RPM_WDOG_TEST_ADDR                                          (RPM_DEC_REG_BASE      + 0x00000058)
#define HWIO_RPM_WDOG_TEST_RMSK                                            0x1fffff
#define HWIO_RPM_WDOG_TEST_IN          \
        in_dword_masked(HWIO_RPM_WDOG_TEST_ADDR, HWIO_RPM_WDOG_TEST_RMSK)
#define HWIO_RPM_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_TEST_ADDR, m)
#define HWIO_RPM_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_TEST_ADDR,v)
#define HWIO_RPM_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_TEST_ADDR,m,v,HWIO_RPM_WDOG_TEST_IN)
#define HWIO_RPM_WDOG_TEST_SYNC_STATUS_BMSK                                0x100000
#define HWIO_RPM_WDOG_TEST_SYNC_STATUS_SHFT                                    0x14
#define HWIO_RPM_WDOG_TEST_LOAD_VALUE_BMSK                                  0xfffff
#define HWIO_RPM_WDOG_TEST_LOAD_VALUE_SHFT                                      0x0

#define HWIO_RPM_TEST_BUS_SEL_ADDR                                       (RPM_DEC_REG_BASE      + 0x0000005c)
#define HWIO_RPM_TEST_BUS_SEL_RMSK                                              0x7
#define HWIO_RPM_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_RPM_TEST_BUS_SEL_ADDR, HWIO_RPM_TEST_BUS_SEL_RMSK)
#define HWIO_RPM_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_RPM_TEST_BUS_SEL_ADDR, m)
#define HWIO_RPM_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_RPM_TEST_BUS_SEL_ADDR,v)
#define HWIO_RPM_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_TEST_BUS_SEL_ADDR,m,v,HWIO_RPM_TEST_BUS_SEL_IN)
#define HWIO_RPM_TEST_BUS_SEL_VAL_BMSK                                          0x7
#define HWIO_RPM_TEST_BUS_SEL_VAL_SHFT                                          0x0

#define HWIO_RPM_PAGE_SELECT_ADDR                                        (RPM_DEC_REG_BASE      + 0x00000070)
#define HWIO_RPM_PAGE_SELECT_RMSK                                              0x3f
#define HWIO_RPM_PAGE_SELECT_IN          \
        in_dword_masked(HWIO_RPM_PAGE_SELECT_ADDR, HWIO_RPM_PAGE_SELECT_RMSK)
#define HWIO_RPM_PAGE_SELECT_INM(m)      \
        in_dword_masked(HWIO_RPM_PAGE_SELECT_ADDR, m)
#define HWIO_RPM_PAGE_SELECT_OUT(v)      \
        out_dword(HWIO_RPM_PAGE_SELECT_ADDR,v)
#define HWIO_RPM_PAGE_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_PAGE_SELECT_ADDR,m,v,HWIO_RPM_PAGE_SELECT_IN)
#define HWIO_RPM_PAGE_SELECT_PAGE_SELECT_BMSK                                  0x3f
#define HWIO_RPM_PAGE_SELECT_PAGE_SELECT_SHFT                                   0x0

/*----------------------------------------------------------------------------
 * MODULE: RPM_F0_QTMR_V1_F0
 *--------------------------------------------------------------------------*/

#define RPM_F0_QTMR_V1_F0_REG_BASE                                  (RPM_CSR_BASE      + 0x00003000)

#define HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_ADDR                          (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000000)
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_ADDR, HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_ADDR                          (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000004)
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_RMSK                            0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_ADDR, HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_ADDR                          (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000008)
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_ADDR, HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_ADDR                          (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x0000000c)
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_RMSK                            0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_ADDR, HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_RPM_F0_QTMR_V1_CNTFRQ_ADDR                             (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000010)
#define HWIO_RPM_F0_QTMR_V1_CNTFRQ_RMSK                             0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTFRQ_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTFRQ_ADDR, HWIO_RPM_F0_QTMR_V1_CNTFRQ_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTFRQ_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_ADDR                          (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000014)
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_RMSK                               0x303
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_ADDR, HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_PL0CTEN_BMSK                       0x200
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_PL0CTEN_SHFT                         0x9
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_PL0VTEN_BMSK                       0x100
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_PL0VTEN_SHFT                         0x8
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_PL0VCTEN_BMSK                        0x2
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_PL0VCTEN_SHFT                        0x1
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_PL0PCTEN_BMSK                        0x1
#define HWIO_RPM_F0_QTMR_V1_CNTPL0ACR_PL0PCTEN_SHFT                        0x0

#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_ADDR                         (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000018)
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_ADDR, HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_ADDR                         (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x0000001c)
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_RMSK                           0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_ADDR, HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_ADDR                       (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000020)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_ADDR, HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_ADDR                       (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000024)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_RMSK                         0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_ADDR, HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_ADDR                          (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000028)
#define HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_ADDR, HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_ADDR                           (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x0000002c)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_RMSK                                  0x7
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTP_CTL_ADDR, HWIO_RPM_F0_QTMR_V1_CNTP_CTL_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTP_CTL_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTP_CTL_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTP_CTL_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTP_CTL_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_RPM_F0_QTMR_V1_CNTP_CTL_EN_SHFT                               0x0

#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_ADDR                       (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000030)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_ADDR, HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_ADDR                       (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000034)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_RMSK                         0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_ADDR, HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_RPM_F0_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_ADDR                          (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000038)
#define HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_ADDR, HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_RPM_F0_QTMR_V1_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_ADDR                           (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x0000003c)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_RMSK                                  0x7
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTV_CTL_ADDR, HWIO_RPM_F0_QTMR_V1_CNTV_CTL_RMSK)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_CNTV_CTL_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_OUT(v)      \
        out_dword(HWIO_RPM_F0_QTMR_V1_CNTV_CTL_ADDR,v)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F0_QTMR_V1_CNTV_CTL_ADDR,m,v,HWIO_RPM_F0_QTMR_V1_CNTV_CTL_IN)
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_RPM_F0_QTMR_V1_CNTV_CTL_EN_SHFT                               0x0

#define HWIO_RPM_F0_QTMR_V1_VERSION_ADDR                            (RPM_F0_QTMR_V1_F0_REG_BASE      + 0x00000fd0)
#define HWIO_RPM_F0_QTMR_V1_VERSION_RMSK                            0xffffffff
#define HWIO_RPM_F0_QTMR_V1_VERSION_IN          \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_VERSION_ADDR, HWIO_RPM_F0_QTMR_V1_VERSION_RMSK)
#define HWIO_RPM_F0_QTMR_V1_VERSION_INM(m)      \
        in_dword_masked(HWIO_RPM_F0_QTMR_V1_VERSION_ADDR, m)
#define HWIO_RPM_F0_QTMR_V1_VERSION_MAJOR_BMSK                      0xf0000000
#define HWIO_RPM_F0_QTMR_V1_VERSION_MAJOR_SHFT                            0x1c
#define HWIO_RPM_F0_QTMR_V1_VERSION_MINOR_BMSK                       0xfff0000
#define HWIO_RPM_F0_QTMR_V1_VERSION_MINOR_SHFT                            0x10
#define HWIO_RPM_F0_QTMR_V1_VERSION_STEP_BMSK                           0xffff
#define HWIO_RPM_F0_QTMR_V1_VERSION_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: RPM_F1_QTMR_V1_F1
 *--------------------------------------------------------------------------*/

#define RPM_F1_QTMR_V1_F1_REG_BASE                                  (RPM_CSR_BASE      + 0x00004000)

#define HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_ADDR                          (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000000)
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_ADDR, HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_ADDR                          (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000004)
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_RMSK                            0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_ADDR, HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_ADDR                          (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000008)
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_ADDR, HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_ADDR                          (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x0000000c)
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_RMSK                            0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_ADDR, HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_RPM_F1_QTMR_V1_CNTFRQ_ADDR                             (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000010)
#define HWIO_RPM_F1_QTMR_V1_CNTFRQ_RMSK                             0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTFRQ_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTFRQ_ADDR, HWIO_RPM_F1_QTMR_V1_CNTFRQ_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTFRQ_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_ADDR                          (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000014)
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_RMSK                               0x303
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_ADDR, HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_PL0CTEN_BMSK                       0x200
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_PL0CTEN_SHFT                         0x9
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_PL0VTEN_BMSK                       0x100
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_PL0VTEN_SHFT                         0x8
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_PL0VCTEN_BMSK                        0x2
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_PL0VCTEN_SHFT                        0x1
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_PL0PCTEN_BMSK                        0x1
#define HWIO_RPM_F1_QTMR_V1_CNTPL0ACR_PL0PCTEN_SHFT                        0x0

#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_ADDR                         (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000018)
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_ADDR, HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_ADDR                         (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x0000001c)
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_RMSK                           0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_ADDR, HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_ADDR                       (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000020)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_ADDR, HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_ADDR                       (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000024)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_RMSK                         0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_ADDR, HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_ADDR                          (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000028)
#define HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_ADDR, HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_ADDR                           (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x0000002c)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_RMSK                                  0x7
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTP_CTL_ADDR, HWIO_RPM_F1_QTMR_V1_CNTP_CTL_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTP_CTL_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTP_CTL_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTP_CTL_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTP_CTL_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_RPM_F1_QTMR_V1_CNTP_CTL_EN_SHFT                               0x0

#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_ADDR                       (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000030)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_ADDR, HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_ADDR                       (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000034)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_RMSK                         0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_ADDR, HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_RPM_F1_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_ADDR                          (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000038)
#define HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_ADDR, HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_RPM_F1_QTMR_V1_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_ADDR                           (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x0000003c)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_RMSK                                  0x7
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTV_CTL_ADDR, HWIO_RPM_F1_QTMR_V1_CNTV_CTL_RMSK)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_CNTV_CTL_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_OUT(v)      \
        out_dword(HWIO_RPM_F1_QTMR_V1_CNTV_CTL_ADDR,v)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_F1_QTMR_V1_CNTV_CTL_ADDR,m,v,HWIO_RPM_F1_QTMR_V1_CNTV_CTL_IN)
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_RPM_F1_QTMR_V1_CNTV_CTL_EN_SHFT                               0x0

#define HWIO_RPM_F1_QTMR_V1_VERSION_ADDR                            (RPM_F1_QTMR_V1_F1_REG_BASE      + 0x00000fd0)
#define HWIO_RPM_F1_QTMR_V1_VERSION_RMSK                            0xffffffff
#define HWIO_RPM_F1_QTMR_V1_VERSION_IN          \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_VERSION_ADDR, HWIO_RPM_F1_QTMR_V1_VERSION_RMSK)
#define HWIO_RPM_F1_QTMR_V1_VERSION_INM(m)      \
        in_dword_masked(HWIO_RPM_F1_QTMR_V1_VERSION_ADDR, m)
#define HWIO_RPM_F1_QTMR_V1_VERSION_MAJOR_BMSK                      0xf0000000
#define HWIO_RPM_F1_QTMR_V1_VERSION_MAJOR_SHFT                            0x1c
#define HWIO_RPM_F1_QTMR_V1_VERSION_MINOR_BMSK                       0xfff0000
#define HWIO_RPM_F1_QTMR_V1_VERSION_MINOR_SHFT                            0x10
#define HWIO_RPM_F1_QTMR_V1_VERSION_STEP_BMSK                           0xffff
#define HWIO_RPM_F1_QTMR_V1_VERSION_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: RPM_MPU
 *--------------------------------------------------------------------------*/

#define RPM_MPU_REG_BASE                                             (RPM_CSR_BASE      + 0x00006000)

#define HWIO_RPM_MPU_XPU_SCR_ADDR                                    (RPM_MPU_REG_BASE      + 0x00000000)
#define HWIO_RPM_MPU_XPU_SCR_RMSK                                         0x17f
#define HWIO_RPM_MPU_XPU_SCR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_SCR_ADDR, HWIO_RPM_MPU_XPU_SCR_RMSK)
#define HWIO_RPM_MPU_XPU_SCR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_SCR_ADDR, m)
#define HWIO_RPM_MPU_XPU_SCR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_SCR_ADDR,v)
#define HWIO_RPM_MPU_XPU_SCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_SCR_ADDR,m,v,HWIO_RPM_MPU_XPU_SCR_IN)
#define HWIO_RPM_MPU_XPU_SCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_RPM_MPU_XPU_SCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_RPM_MPU_XPU_SCR_NSRGCLEE_BMSK                                 0x40
#define HWIO_RPM_MPU_XPU_SCR_NSRGCLEE_SHFT                                  0x6
#define HWIO_RPM_MPU_XPU_SCR_NSCFGE_BMSK                                   0x20
#define HWIO_RPM_MPU_XPU_SCR_NSCFGE_SHFT                                    0x5
#define HWIO_RPM_MPU_XPU_SCR_SDCDEE_BMSK                                   0x10
#define HWIO_RPM_MPU_XPU_SCR_SDCDEE_SHFT                                    0x4
#define HWIO_RPM_MPU_XPU_SCR_SEIE_BMSK                                      0x8
#define HWIO_RPM_MPU_XPU_SCR_SEIE_SHFT                                      0x3
#define HWIO_RPM_MPU_XPU_SCR_SCLERE_BMSK                                    0x4
#define HWIO_RPM_MPU_XPU_SCR_SCLERE_SHFT                                    0x2
#define HWIO_RPM_MPU_XPU_SCR_SCFGERE_BMSK                                   0x2
#define HWIO_RPM_MPU_XPU_SCR_SCFGERE_SHFT                                   0x1
#define HWIO_RPM_MPU_XPU_SCR_XPUNSE_BMSK                                    0x1
#define HWIO_RPM_MPU_XPU_SCR_XPUNSE_SHFT                                    0x0

#define HWIO_RPM_MPU_XPU_SWDR_ADDR                                   (RPM_MPU_REG_BASE      + 0x00000004)
#define HWIO_RPM_MPU_XPU_SWDR_RMSK                                          0x1
#define HWIO_RPM_MPU_XPU_SWDR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_SWDR_ADDR, HWIO_RPM_MPU_XPU_SWDR_RMSK)
#define HWIO_RPM_MPU_XPU_SWDR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_SWDR_ADDR, m)
#define HWIO_RPM_MPU_XPU_SWDR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_SWDR_ADDR,v)
#define HWIO_RPM_MPU_XPU_SWDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_SWDR_ADDR,m,v,HWIO_RPM_MPU_XPU_SWDR_IN)
#define HWIO_RPM_MPU_XPU_SWDR_SCFGWD_BMSK                                   0x1
#define HWIO_RPM_MPU_XPU_SWDR_SCFGWD_SHFT                                   0x0

#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ADDR                      (RPM_MPU_REG_BASE      + 0x00000008)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_RMSK                       0x3ff03ff
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ADDR, HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_RMSK)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ADDR, m)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ADDR,v)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ADDR,m,v,HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_IN)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ROGE_BMSK                  0x2000000
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ROGE_SHFT                       0x19
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ROE_BMSK                   0x1000000
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ROE_SHFT                        0x18
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ROVMID_BMSK                 0xff0000
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_ROVMID_SHFT                     0x10
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_RWGE_BMSK                      0x200
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_RWGE_SHFT                        0x9
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_RWE_BMSK                       0x100
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_RWE_SHFT                         0x8
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_RWVMID_BMSK                     0xff
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SSHADOW_RWVMID_SHFT                      0x0

#define HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_ADDR                    (RPM_MPU_REG_BASE      + 0x00000010)
#define HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_RMSK                       0xfffc0
#define HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_ADDR, HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_RMSK)
#define HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_ADDR, m)
#define HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_ADDR,v)
#define HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_ADDR,m,v,HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_IN)
#define HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_ADDR_BMSK                  0xfffc0
#define HWIO_RPM_MPU_XPU_PRTN_START_SSHADOW0_ADDR_SHFT                      0x6

#define HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_ADDR                      (RPM_MPU_REG_BASE      + 0x00000018)
#define HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_RMSK                         0xfffc0
#define HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_ADDR, HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_RMSK)
#define HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_ADDR, m)
#define HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_ADDR,v)
#define HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_ADDR,m,v,HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_IN)
#define HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_ADDR_BMSK                    0xfffc0
#define HWIO_RPM_MPU_XPU_PRTN_END_SSHADOW0_ADDR_SHFT                        0x6

#define HWIO_RPM_MPU_XPU_SEAR0_ADDR                                  (RPM_MPU_REG_BASE      + 0x00000040)
#define HWIO_RPM_MPU_XPU_SEAR0_RMSK                                  0xffffffff
#define HWIO_RPM_MPU_XPU_SEAR0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_SEAR0_ADDR, HWIO_RPM_MPU_XPU_SEAR0_RMSK)
#define HWIO_RPM_MPU_XPU_SEAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_SEAR0_ADDR, m)
#define HWIO_RPM_MPU_XPU_SEAR0_PA_BMSK                               0xffffffff
#define HWIO_RPM_MPU_XPU_SEAR0_PA_SHFT                                      0x0

#define HWIO_RPM_MPU_XPU_SESR_ADDR                                   (RPM_MPU_REG_BASE      + 0x00000048)
#define HWIO_RPM_MPU_XPU_SESR_RMSK                                   0x80000003
#define HWIO_RPM_MPU_XPU_SESR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESR_ADDR, HWIO_RPM_MPU_XPU_SESR_RMSK)
#define HWIO_RPM_MPU_XPU_SESR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESR_ADDR, m)
#define HWIO_RPM_MPU_XPU_SESR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_SESR_ADDR,v)
#define HWIO_RPM_MPU_XPU_SESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_SESR_ADDR,m,v,HWIO_RPM_MPU_XPU_SESR_IN)
#define HWIO_RPM_MPU_XPU_SESR_MULTI_BMSK                             0x80000000
#define HWIO_RPM_MPU_XPU_SESR_MULTI_SHFT                                   0x1f
#define HWIO_RPM_MPU_XPU_SESR_CLIENT_BMSK                                   0x2
#define HWIO_RPM_MPU_XPU_SESR_CLIENT_SHFT                                   0x1
#define HWIO_RPM_MPU_XPU_SESR_CFG_BMSK                                      0x1
#define HWIO_RPM_MPU_XPU_SESR_CFG_SHFT                                      0x0

#define HWIO_RPM_MPU_XPU_SESRRESTORE_ADDR                            (RPM_MPU_REG_BASE      + 0x0000004c)
#define HWIO_RPM_MPU_XPU_SESRRESTORE_RMSK                            0x80000003
#define HWIO_RPM_MPU_XPU_SESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESRRESTORE_ADDR, HWIO_RPM_MPU_XPU_SESRRESTORE_RMSK)
#define HWIO_RPM_MPU_XPU_SESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESRRESTORE_ADDR, m)
#define HWIO_RPM_MPU_XPU_SESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_SESRRESTORE_ADDR,v)
#define HWIO_RPM_MPU_XPU_SESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_SESRRESTORE_ADDR,m,v,HWIO_RPM_MPU_XPU_SESRRESTORE_IN)
#define HWIO_RPM_MPU_XPU_SESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_RPM_MPU_XPU_SESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_RPM_MPU_XPU_SESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_RPM_MPU_XPU_SESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_RPM_MPU_XPU_SESRRESTORE_CFG_BMSK                               0x1
#define HWIO_RPM_MPU_XPU_SESRRESTORE_CFG_SHFT                               0x0

#define HWIO_RPM_MPU_XPU_SESYNR0_ADDR                                (RPM_MPU_REG_BASE      + 0x00000050)
#define HWIO_RPM_MPU_XPU_SESYNR0_RMSK                                0xffffffff
#define HWIO_RPM_MPU_XPU_SESYNR0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESYNR0_ADDR, HWIO_RPM_MPU_XPU_SESYNR0_RMSK)
#define HWIO_RPM_MPU_XPU_SESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESYNR0_ADDR, m)
#define HWIO_RPM_MPU_XPU_SESYNR0_ATID_BMSK                           0xff000000
#define HWIO_RPM_MPU_XPU_SESYNR0_ATID_SHFT                                 0x18
#define HWIO_RPM_MPU_XPU_SESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_RPM_MPU_XPU_SESYNR0_AVMID_SHFT                                0x10
#define HWIO_RPM_MPU_XPU_SESYNR0_ABID_BMSK                               0xe000
#define HWIO_RPM_MPU_XPU_SESYNR0_ABID_SHFT                                  0xd
#define HWIO_RPM_MPU_XPU_SESYNR0_APID_BMSK                               0x1f00
#define HWIO_RPM_MPU_XPU_SESYNR0_APID_SHFT                                  0x8
#define HWIO_RPM_MPU_XPU_SESYNR0_AMID_BMSK                                 0xff
#define HWIO_RPM_MPU_XPU_SESYNR0_AMID_SHFT                                  0x0

#define HWIO_RPM_MPU_XPU_SESYNR1_ADDR                                (RPM_MPU_REG_BASE      + 0x00000054)
#define HWIO_RPM_MPU_XPU_SESYNR1_RMSK                                0xffffffff
#define HWIO_RPM_MPU_XPU_SESYNR1_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESYNR1_ADDR, HWIO_RPM_MPU_XPU_SESYNR1_RMSK)
#define HWIO_RPM_MPU_XPU_SESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESYNR1_ADDR, m)
#define HWIO_RPM_MPU_XPU_SESYNR1_DCD_BMSK                            0x80000000
#define HWIO_RPM_MPU_XPU_SESYNR1_DCD_SHFT                                  0x1f
#define HWIO_RPM_MPU_XPU_SESYNR1_AC_BMSK                             0x40000000
#define HWIO_RPM_MPU_XPU_SESYNR1_AC_SHFT                                   0x1e
#define HWIO_RPM_MPU_XPU_SESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_RPM_MPU_XPU_SESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_RPM_MPU_XPU_SESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_RPM_MPU_XPU_SESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_RPM_MPU_XPU_SESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_RPM_MPU_XPU_SESYNR1_ABURST_SHFT                               0x1b
#define HWIO_RPM_MPU_XPU_SESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_RPM_MPU_XPU_SESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_RPM_MPU_XPU_SESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_RPM_MPU_XPU_SESYNR1_AWRITE_SHFT                               0x19
#define HWIO_RPM_MPU_XPU_SESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_RPM_MPU_XPU_SESYNR1_AFULL_SHFT                                0x18
#define HWIO_RPM_MPU_XPU_SESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_RPM_MPU_XPU_SESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_RPM_MPU_XPU_SESYNR1_AOOO_BMSK                             0x400000
#define HWIO_RPM_MPU_XPU_SESYNR1_AOOO_SHFT                                 0x16
#define HWIO_RPM_MPU_XPU_SESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_RPM_MPU_XPU_SESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_RPM_MPU_XPU_SESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_RPM_MPU_XPU_SESYNR1_ASIZE_SHFT                                0x10
#define HWIO_RPM_MPU_XPU_SESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_RPM_MPU_XPU_SESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_RPM_MPU_XPU_SESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_RPM_MPU_XPU_SESYNR1_ALEN_SHFT                                  0x8
#define HWIO_RPM_MPU_XPU_SESYNR1_AINST_BMSK                                0x80
#define HWIO_RPM_MPU_XPU_SESYNR1_AINST_SHFT                                 0x7
#define HWIO_RPM_MPU_XPU_SESYNR1_APROTNS_BMSK                              0x40
#define HWIO_RPM_MPU_XPU_SESYNR1_APROTNS_SHFT                               0x6
#define HWIO_RPM_MPU_XPU_SESYNR1_APRIV_BMSK                                0x20
#define HWIO_RPM_MPU_XPU_SESYNR1_APRIV_SHFT                                 0x5
#define HWIO_RPM_MPU_XPU_SESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_RPM_MPU_XPU_SESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_RPM_MPU_XPU_SESYNR1_ASHARED_BMSK                               0x8
#define HWIO_RPM_MPU_XPU_SESYNR1_ASHARED_SHFT                               0x3
#define HWIO_RPM_MPU_XPU_SESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_RPM_MPU_XPU_SESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_RPM_MPU_XPU_SESYNR2_ADDR                                (RPM_MPU_REG_BASE      + 0x00000058)
#define HWIO_RPM_MPU_XPU_SESYNR2_RMSK                                       0x7
#define HWIO_RPM_MPU_XPU_SESYNR2_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESYNR2_ADDR, HWIO_RPM_MPU_XPU_SESYNR2_RMSK)
#define HWIO_RPM_MPU_XPU_SESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_SESYNR2_ADDR, m)
#define HWIO_RPM_MPU_XPU_SESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_RPM_MPU_XPU_SESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_RPM_MPU_XPU_SESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_RPM_MPU_XPU_SESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_RPM_MPU_XPU_SESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_RPM_MPU_XPU_SESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_RPM_MPU_XPU_MCR_ADDR                                    (RPM_MPU_REG_BASE      + 0x00000100)
#define HWIO_RPM_MPU_XPU_MCR_RMSK                                         0x11f
#define HWIO_RPM_MPU_XPU_MCR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_MCR_ADDR, HWIO_RPM_MPU_XPU_MCR_RMSK)
#define HWIO_RPM_MPU_XPU_MCR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_MCR_ADDR, m)
#define HWIO_RPM_MPU_XPU_MCR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_MCR_ADDR,v)
#define HWIO_RPM_MPU_XPU_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_MCR_ADDR,m,v,HWIO_RPM_MPU_XPU_MCR_IN)
#define HWIO_RPM_MPU_XPU_MCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_RPM_MPU_XPU_MCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_RPM_MPU_XPU_MCR_DCDEE_BMSK                                    0x10
#define HWIO_RPM_MPU_XPU_MCR_DCDEE_SHFT                                     0x4
#define HWIO_RPM_MPU_XPU_MCR_EIE_BMSK                                       0x8
#define HWIO_RPM_MPU_XPU_MCR_EIE_SHFT                                       0x3
#define HWIO_RPM_MPU_XPU_MCR_CLERE_BMSK                                     0x4
#define HWIO_RPM_MPU_XPU_MCR_CLERE_SHFT                                     0x2
#define HWIO_RPM_MPU_XPU_MCR_CFGERE_BMSK                                    0x2
#define HWIO_RPM_MPU_XPU_MCR_CFGERE_SHFT                                    0x1
#define HWIO_RPM_MPU_XPU_MCR_XPUMSAE_BMSK                                   0x1
#define HWIO_RPM_MPU_XPU_MCR_XPUMSAE_SHFT                                   0x0

#define HWIO_RPM_MPU_XPU_MEAR0_ADDR                                  (RPM_MPU_REG_BASE      + 0x00000140)
#define HWIO_RPM_MPU_XPU_MEAR0_RMSK                                  0xffffffff
#define HWIO_RPM_MPU_XPU_MEAR0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_MEAR0_ADDR, HWIO_RPM_MPU_XPU_MEAR0_RMSK)
#define HWIO_RPM_MPU_XPU_MEAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_MEAR0_ADDR, m)
#define HWIO_RPM_MPU_XPU_MEAR0_PA_BMSK                               0xffffffff
#define HWIO_RPM_MPU_XPU_MEAR0_PA_SHFT                                      0x0

#define HWIO_RPM_MPU_XPU_MESR_ADDR                                   (RPM_MPU_REG_BASE      + 0x00000148)
#define HWIO_RPM_MPU_XPU_MESR_RMSK                                   0x80000003
#define HWIO_RPM_MPU_XPU_MESR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESR_ADDR, HWIO_RPM_MPU_XPU_MESR_RMSK)
#define HWIO_RPM_MPU_XPU_MESR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESR_ADDR, m)
#define HWIO_RPM_MPU_XPU_MESR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_MESR_ADDR,v)
#define HWIO_RPM_MPU_XPU_MESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_MESR_ADDR,m,v,HWIO_RPM_MPU_XPU_MESR_IN)
#define HWIO_RPM_MPU_XPU_MESR_MULTI_BMSK                             0x80000000
#define HWIO_RPM_MPU_XPU_MESR_MULTI_SHFT                                   0x1f
#define HWIO_RPM_MPU_XPU_MESR_CLIENT_BMSK                                   0x2
#define HWIO_RPM_MPU_XPU_MESR_CLIENT_SHFT                                   0x1
#define HWIO_RPM_MPU_XPU_MESR_CFG_BMSK                                      0x1
#define HWIO_RPM_MPU_XPU_MESR_CFG_SHFT                                      0x0

#define HWIO_RPM_MPU_XPU_MESRRESTORE_ADDR                            (RPM_MPU_REG_BASE      + 0x0000014c)
#define HWIO_RPM_MPU_XPU_MESRRESTORE_RMSK                            0x80000003
#define HWIO_RPM_MPU_XPU_MESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESRRESTORE_ADDR, HWIO_RPM_MPU_XPU_MESRRESTORE_RMSK)
#define HWIO_RPM_MPU_XPU_MESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESRRESTORE_ADDR, m)
#define HWIO_RPM_MPU_XPU_MESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_MESRRESTORE_ADDR,v)
#define HWIO_RPM_MPU_XPU_MESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_MESRRESTORE_ADDR,m,v,HWIO_RPM_MPU_XPU_MESRRESTORE_IN)
#define HWIO_RPM_MPU_XPU_MESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_RPM_MPU_XPU_MESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_RPM_MPU_XPU_MESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_RPM_MPU_XPU_MESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_RPM_MPU_XPU_MESRRESTORE_CFG_BMSK                               0x1
#define HWIO_RPM_MPU_XPU_MESRRESTORE_CFG_SHFT                               0x0

#define HWIO_RPM_MPU_XPU_MESYNR0_ADDR                                (RPM_MPU_REG_BASE      + 0x00000150)
#define HWIO_RPM_MPU_XPU_MESYNR0_RMSK                                0xffffffff
#define HWIO_RPM_MPU_XPU_MESYNR0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESYNR0_ADDR, HWIO_RPM_MPU_XPU_MESYNR0_RMSK)
#define HWIO_RPM_MPU_XPU_MESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESYNR0_ADDR, m)
#define HWIO_RPM_MPU_XPU_MESYNR0_ATID_BMSK                           0xff000000
#define HWIO_RPM_MPU_XPU_MESYNR0_ATID_SHFT                                 0x18
#define HWIO_RPM_MPU_XPU_MESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_RPM_MPU_XPU_MESYNR0_AVMID_SHFT                                0x10
#define HWIO_RPM_MPU_XPU_MESYNR0_ABID_BMSK                               0xe000
#define HWIO_RPM_MPU_XPU_MESYNR0_ABID_SHFT                                  0xd
#define HWIO_RPM_MPU_XPU_MESYNR0_APID_BMSK                               0x1f00
#define HWIO_RPM_MPU_XPU_MESYNR0_APID_SHFT                                  0x8
#define HWIO_RPM_MPU_XPU_MESYNR0_AMID_BMSK                                 0xff
#define HWIO_RPM_MPU_XPU_MESYNR0_AMID_SHFT                                  0x0

#define HWIO_RPM_MPU_XPU_MESYNR1_ADDR                                (RPM_MPU_REG_BASE      + 0x00000154)
#define HWIO_RPM_MPU_XPU_MESYNR1_RMSK                                0xffffffff
#define HWIO_RPM_MPU_XPU_MESYNR1_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESYNR1_ADDR, HWIO_RPM_MPU_XPU_MESYNR1_RMSK)
#define HWIO_RPM_MPU_XPU_MESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESYNR1_ADDR, m)
#define HWIO_RPM_MPU_XPU_MESYNR1_DCD_BMSK                            0x80000000
#define HWIO_RPM_MPU_XPU_MESYNR1_DCD_SHFT                                  0x1f
#define HWIO_RPM_MPU_XPU_MESYNR1_AC_BMSK                             0x40000000
#define HWIO_RPM_MPU_XPU_MESYNR1_AC_SHFT                                   0x1e
#define HWIO_RPM_MPU_XPU_MESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_RPM_MPU_XPU_MESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_RPM_MPU_XPU_MESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_RPM_MPU_XPU_MESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_RPM_MPU_XPU_MESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_RPM_MPU_XPU_MESYNR1_ABURST_SHFT                               0x1b
#define HWIO_RPM_MPU_XPU_MESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_RPM_MPU_XPU_MESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_RPM_MPU_XPU_MESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_RPM_MPU_XPU_MESYNR1_AWRITE_SHFT                               0x19
#define HWIO_RPM_MPU_XPU_MESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_RPM_MPU_XPU_MESYNR1_AFULL_SHFT                                0x18
#define HWIO_RPM_MPU_XPU_MESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_RPM_MPU_XPU_MESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_RPM_MPU_XPU_MESYNR1_AOOO_BMSK                             0x400000
#define HWIO_RPM_MPU_XPU_MESYNR1_AOOO_SHFT                                 0x16
#define HWIO_RPM_MPU_XPU_MESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_RPM_MPU_XPU_MESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_RPM_MPU_XPU_MESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_RPM_MPU_XPU_MESYNR1_ASIZE_SHFT                                0x10
#define HWIO_RPM_MPU_XPU_MESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_RPM_MPU_XPU_MESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_RPM_MPU_XPU_MESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_RPM_MPU_XPU_MESYNR1_ALEN_SHFT                                  0x8
#define HWIO_RPM_MPU_XPU_MESYNR1_AINST_BMSK                                0x80
#define HWIO_RPM_MPU_XPU_MESYNR1_AINST_SHFT                                 0x7
#define HWIO_RPM_MPU_XPU_MESYNR1_APROTNS_BMSK                              0x40
#define HWIO_RPM_MPU_XPU_MESYNR1_APROTNS_SHFT                               0x6
#define HWIO_RPM_MPU_XPU_MESYNR1_APRIV_BMSK                                0x20
#define HWIO_RPM_MPU_XPU_MESYNR1_APRIV_SHFT                                 0x5
#define HWIO_RPM_MPU_XPU_MESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_RPM_MPU_XPU_MESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_RPM_MPU_XPU_MESYNR1_ASHARED_BMSK                               0x8
#define HWIO_RPM_MPU_XPU_MESYNR1_ASHARED_SHFT                               0x3
#define HWIO_RPM_MPU_XPU_MESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_RPM_MPU_XPU_MESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_RPM_MPU_XPU_MESYNR2_ADDR                                (RPM_MPU_REG_BASE      + 0x00000158)
#define HWIO_RPM_MPU_XPU_MESYNR2_RMSK                                       0x7
#define HWIO_RPM_MPU_XPU_MESYNR2_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESYNR2_ADDR, HWIO_RPM_MPU_XPU_MESYNR2_RMSK)
#define HWIO_RPM_MPU_XPU_MESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_MESYNR2_ADDR, m)
#define HWIO_RPM_MPU_XPU_MESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_RPM_MPU_XPU_MESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_RPM_MPU_XPU_MESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_RPM_MPU_XPU_MESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_RPM_MPU_XPU_MESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_RPM_MPU_XPU_MESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_RPM_MPU_XPU_CR_ADDR                                     (RPM_MPU_REG_BASE      + 0x00000080)
#define HWIO_RPM_MPU_XPU_CR_RMSK                                          0x11f
#define HWIO_RPM_MPU_XPU_CR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_CR_ADDR, HWIO_RPM_MPU_XPU_CR_RMSK)
#define HWIO_RPM_MPU_XPU_CR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_CR_ADDR, m)
#define HWIO_RPM_MPU_XPU_CR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_CR_ADDR,v)
#define HWIO_RPM_MPU_XPU_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_CR_ADDR,m,v,HWIO_RPM_MPU_XPU_CR_IN)
#define HWIO_RPM_MPU_XPU_CR_DYNAMIC_CLK_EN_BMSK                           0x100
#define HWIO_RPM_MPU_XPU_CR_DYNAMIC_CLK_EN_SHFT                             0x8
#define HWIO_RPM_MPU_XPU_CR_DCDEE_BMSK                                     0x10
#define HWIO_RPM_MPU_XPU_CR_DCDEE_SHFT                                      0x4
#define HWIO_RPM_MPU_XPU_CR_EIE_BMSK                                        0x8
#define HWIO_RPM_MPU_XPU_CR_EIE_SHFT                                        0x3
#define HWIO_RPM_MPU_XPU_CR_CLERE_BMSK                                      0x4
#define HWIO_RPM_MPU_XPU_CR_CLERE_SHFT                                      0x2
#define HWIO_RPM_MPU_XPU_CR_CFGERE_BMSK                                     0x2
#define HWIO_RPM_MPU_XPU_CR_CFGERE_SHFT                                     0x1
#define HWIO_RPM_MPU_XPU_CR_XPUVMIDE_BMSK                                   0x1
#define HWIO_RPM_MPU_XPU_CR_XPUVMIDE_SHFT                                   0x0

#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ADDR                       (RPM_MPU_REG_BASE      + 0x00000088)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_RMSK                        0x3ff03ff
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ADDR, HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_RMSK)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ADDR, m)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ADDR,v)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ADDR,m,v,HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_IN)
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ROGE_BMSK                   0x2000000
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ROGE_SHFT                        0x19
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ROE_BMSK                    0x1000000
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ROE_SHFT                         0x18
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ROVMID_BMSK                  0xff0000
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_ROVMID_SHFT                      0x10
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_RWGE_BMSK                       0x200
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_RWGE_SHFT                         0x9
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_RWE_BMSK                        0x100
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_RWE_SHFT                          0x8
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_RWVMID_BMSK                      0xff
#define HWIO_RPM_MPU_XPU_PRTN_RACR_SHADOW_RWVMID_SHFT                       0x0

#define HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_ADDR                     (RPM_MPU_REG_BASE      + 0x00000090)
#define HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_RMSK                        0xfffc0
#define HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_ADDR, HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_RMSK)
#define HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_ADDR, m)
#define HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_ADDR,v)
#define HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_ADDR,m,v,HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_IN)
#define HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_ADDR_BMSK                   0xfffc0
#define HWIO_RPM_MPU_XPU_PRTN_START_SHADOW0_ADDR_SHFT                       0x6

#define HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_ADDR                       (RPM_MPU_REG_BASE      + 0x00000098)
#define HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_RMSK                          0xfffc0
#define HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_ADDR, HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_RMSK)
#define HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_ADDR, m)
#define HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_ADDR,v)
#define HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_ADDR,m,v,HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_IN)
#define HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_ADDR_BMSK                     0xfffc0
#define HWIO_RPM_MPU_XPU_PRTN_END_SHADOW0_ADDR_SHFT                         0x6

#define HWIO_RPM_MPU_XPU_RPU_ACRn_ADDR(n)                            (RPM_MPU_REG_BASE      + 0x000000a0 + 0x4 * (n))
#define HWIO_RPM_MPU_XPU_RPU_ACRn_RMSK                               0xffffffff
#define HWIO_RPM_MPU_XPU_RPU_ACRn_MAXn                                        0
#define HWIO_RPM_MPU_XPU_RPU_ACRn_INI(n)        \
        in_dword_masked(HWIO_RPM_MPU_XPU_RPU_ACRn_ADDR(n), HWIO_RPM_MPU_XPU_RPU_ACRn_RMSK)
#define HWIO_RPM_MPU_XPU_RPU_ACRn_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_MPU_XPU_RPU_ACRn_ADDR(n), mask)
#define HWIO_RPM_MPU_XPU_RPU_ACRn_OUTI(n,val)    \
        out_dword(HWIO_RPM_MPU_XPU_RPU_ACRn_ADDR(n),val)
#define HWIO_RPM_MPU_XPU_RPU_ACRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_RPU_ACRn_ADDR(n),mask,val,HWIO_RPM_MPU_XPU_RPU_ACRn_INI(n))
#define HWIO_RPM_MPU_XPU_RPU_ACRn_RWE_BMSK                           0xffffffff
#define HWIO_RPM_MPU_XPU_RPU_ACRn_RWE_SHFT                                  0x0

#define HWIO_RPM_MPU_XPU_EAR0_ADDR                                   (RPM_MPU_REG_BASE      + 0x000000c0)
#define HWIO_RPM_MPU_XPU_EAR0_RMSK                                   0xffffffff
#define HWIO_RPM_MPU_XPU_EAR0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_EAR0_ADDR, HWIO_RPM_MPU_XPU_EAR0_RMSK)
#define HWIO_RPM_MPU_XPU_EAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_EAR0_ADDR, m)
#define HWIO_RPM_MPU_XPU_EAR0_PA_BMSK                                0xffffffff
#define HWIO_RPM_MPU_XPU_EAR0_PA_SHFT                                       0x0

#define HWIO_RPM_MPU_XPU_ESR_ADDR                                    (RPM_MPU_REG_BASE      + 0x000000c8)
#define HWIO_RPM_MPU_XPU_ESR_RMSK                                    0x80000003
#define HWIO_RPM_MPU_XPU_ESR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESR_ADDR, HWIO_RPM_MPU_XPU_ESR_RMSK)
#define HWIO_RPM_MPU_XPU_ESR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESR_ADDR, m)
#define HWIO_RPM_MPU_XPU_ESR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_ESR_ADDR,v)
#define HWIO_RPM_MPU_XPU_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_ESR_ADDR,m,v,HWIO_RPM_MPU_XPU_ESR_IN)
#define HWIO_RPM_MPU_XPU_ESR_MULTI_BMSK                              0x80000000
#define HWIO_RPM_MPU_XPU_ESR_MULTI_SHFT                                    0x1f
#define HWIO_RPM_MPU_XPU_ESR_CLIENT_BMSK                                    0x2
#define HWIO_RPM_MPU_XPU_ESR_CLIENT_SHFT                                    0x1
#define HWIO_RPM_MPU_XPU_ESR_CFG_BMSK                                       0x1
#define HWIO_RPM_MPU_XPU_ESR_CFG_SHFT                                       0x0

#define HWIO_RPM_MPU_XPU_ESRRESTORE_ADDR                             (RPM_MPU_REG_BASE      + 0x000000cc)
#define HWIO_RPM_MPU_XPU_ESRRESTORE_RMSK                             0x80000003
#define HWIO_RPM_MPU_XPU_ESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESRRESTORE_ADDR, HWIO_RPM_MPU_XPU_ESRRESTORE_RMSK)
#define HWIO_RPM_MPU_XPU_ESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESRRESTORE_ADDR, m)
#define HWIO_RPM_MPU_XPU_ESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_ESRRESTORE_ADDR,v)
#define HWIO_RPM_MPU_XPU_ESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_ESRRESTORE_ADDR,m,v,HWIO_RPM_MPU_XPU_ESRRESTORE_IN)
#define HWIO_RPM_MPU_XPU_ESRRESTORE_MULTI_BMSK                       0x80000000
#define HWIO_RPM_MPU_XPU_ESRRESTORE_MULTI_SHFT                             0x1f
#define HWIO_RPM_MPU_XPU_ESRRESTORE_CLIENT_BMSK                             0x2
#define HWIO_RPM_MPU_XPU_ESRRESTORE_CLIENT_SHFT                             0x1
#define HWIO_RPM_MPU_XPU_ESRRESTORE_CFG_BMSK                                0x1
#define HWIO_RPM_MPU_XPU_ESRRESTORE_CFG_SHFT                                0x0

#define HWIO_RPM_MPU_XPU_ESYNR0_ADDR                                 (RPM_MPU_REG_BASE      + 0x000000d0)
#define HWIO_RPM_MPU_XPU_ESYNR0_RMSK                                 0xffffffff
#define HWIO_RPM_MPU_XPU_ESYNR0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESYNR0_ADDR, HWIO_RPM_MPU_XPU_ESYNR0_RMSK)
#define HWIO_RPM_MPU_XPU_ESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESYNR0_ADDR, m)
#define HWIO_RPM_MPU_XPU_ESYNR0_ATID_BMSK                            0xff000000
#define HWIO_RPM_MPU_XPU_ESYNR0_ATID_SHFT                                  0x18
#define HWIO_RPM_MPU_XPU_ESYNR0_AVMID_BMSK                             0xff0000
#define HWIO_RPM_MPU_XPU_ESYNR0_AVMID_SHFT                                 0x10
#define HWIO_RPM_MPU_XPU_ESYNR0_ABID_BMSK                                0xe000
#define HWIO_RPM_MPU_XPU_ESYNR0_ABID_SHFT                                   0xd
#define HWIO_RPM_MPU_XPU_ESYNR0_APID_BMSK                                0x1f00
#define HWIO_RPM_MPU_XPU_ESYNR0_APID_SHFT                                   0x8
#define HWIO_RPM_MPU_XPU_ESYNR0_AMID_BMSK                                  0xff
#define HWIO_RPM_MPU_XPU_ESYNR0_AMID_SHFT                                   0x0

#define HWIO_RPM_MPU_XPU_ESYNR1_ADDR                                 (RPM_MPU_REG_BASE      + 0x000000d4)
#define HWIO_RPM_MPU_XPU_ESYNR1_RMSK                                 0xffffffff
#define HWIO_RPM_MPU_XPU_ESYNR1_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESYNR1_ADDR, HWIO_RPM_MPU_XPU_ESYNR1_RMSK)
#define HWIO_RPM_MPU_XPU_ESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESYNR1_ADDR, m)
#define HWIO_RPM_MPU_XPU_ESYNR1_DCD_BMSK                             0x80000000
#define HWIO_RPM_MPU_XPU_ESYNR1_DCD_SHFT                                   0x1f
#define HWIO_RPM_MPU_XPU_ESYNR1_AC_BMSK                              0x40000000
#define HWIO_RPM_MPU_XPU_ESYNR1_AC_SHFT                                    0x1e
#define HWIO_RPM_MPU_XPU_ESYNR1_BURSTLEN_BMSK                        0x20000000
#define HWIO_RPM_MPU_XPU_ESYNR1_BURSTLEN_SHFT                              0x1d
#define HWIO_RPM_MPU_XPU_ESYNR1_ARDALLOCATE_BMSK                     0x10000000
#define HWIO_RPM_MPU_XPU_ESYNR1_ARDALLOCATE_SHFT                           0x1c
#define HWIO_RPM_MPU_XPU_ESYNR1_ABURST_BMSK                           0x8000000
#define HWIO_RPM_MPU_XPU_ESYNR1_ABURST_SHFT                                0x1b
#define HWIO_RPM_MPU_XPU_ESYNR1_AEXCLUSIVE_BMSK                       0x4000000
#define HWIO_RPM_MPU_XPU_ESYNR1_AEXCLUSIVE_SHFT                            0x1a
#define HWIO_RPM_MPU_XPU_ESYNR1_AWRITE_BMSK                           0x2000000
#define HWIO_RPM_MPU_XPU_ESYNR1_AWRITE_SHFT                                0x19
#define HWIO_RPM_MPU_XPU_ESYNR1_AFULL_BMSK                            0x1000000
#define HWIO_RPM_MPU_XPU_ESYNR1_AFULL_SHFT                                 0x18
#define HWIO_RPM_MPU_XPU_ESYNR1_ARDBEADNDXEN_BMSK                      0x800000
#define HWIO_RPM_MPU_XPU_ESYNR1_ARDBEADNDXEN_SHFT                          0x17
#define HWIO_RPM_MPU_XPU_ESYNR1_AOOO_BMSK                              0x400000
#define HWIO_RPM_MPU_XPU_ESYNR1_AOOO_SHFT                                  0x16
#define HWIO_RPM_MPU_XPU_ESYNR1_APREQPRIORITY_BMSK                     0x380000
#define HWIO_RPM_MPU_XPU_ESYNR1_APREQPRIORITY_SHFT                         0x13
#define HWIO_RPM_MPU_XPU_ESYNR1_ASIZE_BMSK                              0x70000
#define HWIO_RPM_MPU_XPU_ESYNR1_ASIZE_SHFT                                 0x10
#define HWIO_RPM_MPU_XPU_ESYNR1_AMSSSELFAUTH_BMSK                        0x8000
#define HWIO_RPM_MPU_XPU_ESYNR1_AMSSSELFAUTH_SHFT                           0xf
#define HWIO_RPM_MPU_XPU_ESYNR1_ALEN_BMSK                                0x7f00
#define HWIO_RPM_MPU_XPU_ESYNR1_ALEN_SHFT                                   0x8
#define HWIO_RPM_MPU_XPU_ESYNR1_AINST_BMSK                                 0x80
#define HWIO_RPM_MPU_XPU_ESYNR1_AINST_SHFT                                  0x7
#define HWIO_RPM_MPU_XPU_ESYNR1_APROTNS_BMSK                               0x40
#define HWIO_RPM_MPU_XPU_ESYNR1_APROTNS_SHFT                                0x6
#define HWIO_RPM_MPU_XPU_ESYNR1_APRIV_BMSK                                 0x20
#define HWIO_RPM_MPU_XPU_ESYNR1_APRIV_SHFT                                  0x5
#define HWIO_RPM_MPU_XPU_ESYNR1_AINNERSHARED_BMSK                          0x10
#define HWIO_RPM_MPU_XPU_ESYNR1_AINNERSHARED_SHFT                           0x4
#define HWIO_RPM_MPU_XPU_ESYNR1_ASHARED_BMSK                                0x8
#define HWIO_RPM_MPU_XPU_ESYNR1_ASHARED_SHFT                                0x3
#define HWIO_RPM_MPU_XPU_ESYNR1_AMEMTYPE_BMSK                               0x7
#define HWIO_RPM_MPU_XPU_ESYNR1_AMEMTYPE_SHFT                               0x0

#define HWIO_RPM_MPU_XPU_ESYNR2_ADDR                                 (RPM_MPU_REG_BASE      + 0x000000d8)
#define HWIO_RPM_MPU_XPU_ESYNR2_RMSK                                        0x7
#define HWIO_RPM_MPU_XPU_ESYNR2_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESYNR2_ADDR, HWIO_RPM_MPU_XPU_ESYNR2_RMSK)
#define HWIO_RPM_MPU_XPU_ESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_ESYNR2_ADDR, m)
#define HWIO_RPM_MPU_XPU_ESYNR2_MODEM_PRT_HIT_BMSK                          0x4
#define HWIO_RPM_MPU_XPU_ESYNR2_MODEM_PRT_HIT_SHFT                          0x2
#define HWIO_RPM_MPU_XPU_ESYNR2_SECURE_PRT_HIT_BMSK                         0x2
#define HWIO_RPM_MPU_XPU_ESYNR2_SECURE_PRT_HIT_SHFT                         0x1
#define HWIO_RPM_MPU_XPU_ESYNR2_NONSECURE_PRT_HIT_BMSK                      0x1
#define HWIO_RPM_MPU_XPU_ESYNR2_NONSECURE_PRT_HIT_SHFT                      0x0

#define HWIO_RPM_MPU_XPU_UMR_RACR_ADDR                               (RPM_MPU_REG_BASE      + 0x000000e0)
#define HWIO_RPM_MPU_XPU_UMR_RACR_RMSK                               0xffffffff
#define HWIO_RPM_MPU_XPU_UMR_RACR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_UMR_RACR_ADDR, HWIO_RPM_MPU_XPU_UMR_RACR_RMSK)
#define HWIO_RPM_MPU_XPU_UMR_RACR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_UMR_RACR_ADDR, m)
#define HWIO_RPM_MPU_XPU_UMR_RACR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_UMR_RACR_ADDR,v)
#define HWIO_RPM_MPU_XPU_UMR_RACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_UMR_RACR_ADDR,m,v,HWIO_RPM_MPU_XPU_UMR_RACR_IN)
#define HWIO_RPM_MPU_XPU_UMR_RACR_RE_BMSK                            0xffffffff
#define HWIO_RPM_MPU_XPU_UMR_RACR_RE_SHFT                                   0x0

#define HWIO_RPM_MPU_XPU_UMR_WACR_ADDR                               (RPM_MPU_REG_BASE      + 0x000000e4)
#define HWIO_RPM_MPU_XPU_UMR_WACR_RMSK                               0xffffffff
#define HWIO_RPM_MPU_XPU_UMR_WACR_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_UMR_WACR_ADDR, HWIO_RPM_MPU_XPU_UMR_WACR_RMSK)
#define HWIO_RPM_MPU_XPU_UMR_WACR_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_UMR_WACR_ADDR, m)
#define HWIO_RPM_MPU_XPU_UMR_WACR_OUT(v)      \
        out_dword(HWIO_RPM_MPU_XPU_UMR_WACR_ADDR,v)
#define HWIO_RPM_MPU_XPU_UMR_WACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_UMR_WACR_ADDR,m,v,HWIO_RPM_MPU_XPU_UMR_WACR_IN)
#define HWIO_RPM_MPU_XPU_UMR_WACR_WE_BMSK                            0xffffffff
#define HWIO_RPM_MPU_XPU_UMR_WACR_WE_SHFT                                   0x0

#define HWIO_RPM_MPU_XPU_IDR0_ADDR                                   (RPM_MPU_REG_BASE      + 0x00000074)
#define HWIO_RPM_MPU_XPU_IDR0_RMSK                                   0xff3fbfff
#define HWIO_RPM_MPU_XPU_IDR0_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_IDR0_ADDR, HWIO_RPM_MPU_XPU_IDR0_RMSK)
#define HWIO_RPM_MPU_XPU_IDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_IDR0_ADDR, m)
#define HWIO_RPM_MPU_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_BMSK          0x80000000
#define HWIO_RPM_MPU_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_SHFT                0x1f
#define HWIO_RPM_MPU_XPU_IDR0_SAVERESTORE_HW_EN_BMSK                 0x40000000
#define HWIO_RPM_MPU_XPU_IDR0_SAVERESTORE_HW_EN_SHFT                       0x1e
#define HWIO_RPM_MPU_XPU_IDR0_MSB_BMSK                               0x3f000000
#define HWIO_RPM_MPU_XPU_IDR0_MSB_SHFT                                     0x18
#define HWIO_RPM_MPU_XPU_IDR0_LSB_BMSK                                 0x3f0000
#define HWIO_RPM_MPU_XPU_IDR0_LSB_SHFT                                     0x10
#define HWIO_RPM_MPU_XPU_IDR0_BLED_BMSK                                  0x8000
#define HWIO_RPM_MPU_XPU_IDR0_BLED_SHFT                                     0xf
#define HWIO_RPM_MPU_XPU_IDR0_XPUT_BMSK                                  0x3000
#define HWIO_RPM_MPU_XPU_IDR0_XPUT_SHFT                                     0xc
#define HWIO_RPM_MPU_XPU_IDR0_PT_BMSK                                     0x800
#define HWIO_RPM_MPU_XPU_IDR0_PT_SHFT                                       0xb
#define HWIO_RPM_MPU_XPU_IDR0_MV_BMSK                                     0x400
#define HWIO_RPM_MPU_XPU_IDR0_MV_SHFT                                       0xa
#define HWIO_RPM_MPU_XPU_IDR0_NRG_BMSK                                    0x3ff
#define HWIO_RPM_MPU_XPU_IDR0_NRG_SHFT                                      0x0

#define HWIO_RPM_MPU_XPU_IDR1_ADDR                                   (RPM_MPU_REG_BASE      + 0x00000078)
#define HWIO_RPM_MPU_XPU_IDR1_RMSK                                   0x7f3ffeff
#define HWIO_RPM_MPU_XPU_IDR1_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_IDR1_ADDR, HWIO_RPM_MPU_XPU_IDR1_RMSK)
#define HWIO_RPM_MPU_XPU_IDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_IDR1_ADDR, m)
#define HWIO_RPM_MPU_XPU_IDR1_AMT_HW_ENABLE_BMSK                     0x40000000
#define HWIO_RPM_MPU_XPU_IDR1_AMT_HW_ENABLE_SHFT                           0x1e
#define HWIO_RPM_MPU_XPU_IDR1_CLIENT_ADDR_WIDTH_BMSK                 0x3f000000
#define HWIO_RPM_MPU_XPU_IDR1_CLIENT_ADDR_WIDTH_SHFT                       0x18
#define HWIO_RPM_MPU_XPU_IDR1_CONFIG_ADDR_WIDTH_BMSK                   0x3f0000
#define HWIO_RPM_MPU_XPU_IDR1_CONFIG_ADDR_WIDTH_SHFT                       0x10
#define HWIO_RPM_MPU_XPU_IDR1_QRIB_EN_BMSK                               0x8000
#define HWIO_RPM_MPU_XPU_IDR1_QRIB_EN_SHFT                                  0xf
#define HWIO_RPM_MPU_XPU_IDR1_ASYNC_MODE_BMSK                            0x4000
#define HWIO_RPM_MPU_XPU_IDR1_ASYNC_MODE_SHFT                               0xe
#define HWIO_RPM_MPU_XPU_IDR1_CONFIG_TYPE_BMSK                           0x2000
#define HWIO_RPM_MPU_XPU_IDR1_CONFIG_TYPE_SHFT                              0xd
#define HWIO_RPM_MPU_XPU_IDR1_CLIENT_PIPELINE_ENABLED_BMSK               0x1000
#define HWIO_RPM_MPU_XPU_IDR1_CLIENT_PIPELINE_ENABLED_SHFT                  0xc
#define HWIO_RPM_MPU_XPU_IDR1_MSA_CHECK_HW_ENABLE_BMSK                    0x800
#define HWIO_RPM_MPU_XPU_IDR1_MSA_CHECK_HW_ENABLE_SHFT                      0xb
#define HWIO_RPM_MPU_XPU_IDR1_XPU_SYND_REG_ABSENT_BMSK                    0x400
#define HWIO_RPM_MPU_XPU_IDR1_XPU_SYND_REG_ABSENT_SHFT                      0xa
#define HWIO_RPM_MPU_XPU_IDR1_TZXPU_BMSK                                  0x200
#define HWIO_RPM_MPU_XPU_IDR1_TZXPU_SHFT                                    0x9
#define HWIO_RPM_MPU_XPU_IDR1_NVMID_BMSK                                   0xff
#define HWIO_RPM_MPU_XPU_IDR1_NVMID_SHFT                                    0x0

#define HWIO_RPM_MPU_XPU_REV_ADDR                                    (RPM_MPU_REG_BASE      + 0x0000007c)
#define HWIO_RPM_MPU_XPU_REV_RMSK                                    0xffffffff
#define HWIO_RPM_MPU_XPU_REV_IN          \
        in_dword_masked(HWIO_RPM_MPU_XPU_REV_ADDR, HWIO_RPM_MPU_XPU_REV_RMSK)
#define HWIO_RPM_MPU_XPU_REV_INM(m)      \
        in_dword_masked(HWIO_RPM_MPU_XPU_REV_ADDR, m)
#define HWIO_RPM_MPU_XPU_REV_MAJOR_BMSK                              0xf0000000
#define HWIO_RPM_MPU_XPU_REV_MAJOR_SHFT                                    0x1c
#define HWIO_RPM_MPU_XPU_REV_MINOR_BMSK                               0xfff0000
#define HWIO_RPM_MPU_XPU_REV_MINOR_SHFT                                    0x10
#define HWIO_RPM_MPU_XPU_REV_STEP_BMSK                                   0xffff
#define HWIO_RPM_MPU_XPU_REV_STEP_SHFT                                      0x0

#define HWIO_RPM_MPU_XPU_PRTn_RACRm_ADDR(n,m)                        (RPM_MPU_REG_BASE      + 0x00000200 + 0x80 * (n)+0x4 * (m))
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_RMSK                              0x3ff03ff
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_MAXn                                      5
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_MAXm                                      0
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_INI2(n,m)        \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_RACRm_ADDR(n,m), HWIO_RPM_MPU_XPU_PRTn_RACRm_RMSK)
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_INMI2(n,m,mask)    \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_RACRm_ADDR(n,m), mask)
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_OUTI2(n,m,val)    \
        out_dword(HWIO_RPM_MPU_XPU_PRTn_RACRm_ADDR(n,m),val)
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_OUTMI2(n,m,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTn_RACRm_ADDR(n,m),mask,val,HWIO_RPM_MPU_XPU_PRTn_RACRm_INI2(n,m))
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_ROGE_BMSK                         0x2000000
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_ROGE_SHFT                              0x19
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_ROE_BMSK                          0x1000000
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_ROE_SHFT                               0x18
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_ROVMID_BMSK                        0xff0000
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_ROVMID_SHFT                            0x10
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_RWGE_BMSK                             0x200
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_RWGE_SHFT                               0x9
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_RWE_BMSK                              0x100
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_RWE_SHFT                                0x8
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_RWVMID_BMSK                            0xff
#define HWIO_RPM_MPU_XPU_PRTn_RACRm_RWVMID_SHFT                             0x0

#define HWIO_RPM_MPU_XPU_PRTn_START0_ADDR(n)                         (RPM_MPU_REG_BASE      + 0x00000240 + 0x80 * (n))
#define HWIO_RPM_MPU_XPU_PRTn_START0_RMSK                               0xfffc0
#define HWIO_RPM_MPU_XPU_PRTn_START0_MAXn                                     5
#define HWIO_RPM_MPU_XPU_PRTn_START0_INI(n)        \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_START0_ADDR(n), HWIO_RPM_MPU_XPU_PRTn_START0_RMSK)
#define HWIO_RPM_MPU_XPU_PRTn_START0_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_START0_ADDR(n), mask)
#define HWIO_RPM_MPU_XPU_PRTn_START0_OUTI(n,val)    \
        out_dword(HWIO_RPM_MPU_XPU_PRTn_START0_ADDR(n),val)
#define HWIO_RPM_MPU_XPU_PRTn_START0_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTn_START0_ADDR(n),mask,val,HWIO_RPM_MPU_XPU_PRTn_START0_INI(n))
#define HWIO_RPM_MPU_XPU_PRTn_START0_ADDR_BMSK                          0xfffc0
#define HWIO_RPM_MPU_XPU_PRTn_START0_ADDR_SHFT                              0x6

#define HWIO_RPM_MPU_XPU_PRTn_END0_ADDR(n)                           (RPM_MPU_REG_BASE      + 0x00000248 + 0x80 * (n))
#define HWIO_RPM_MPU_XPU_PRTn_END0_RMSK                                 0xfffc0
#define HWIO_RPM_MPU_XPU_PRTn_END0_MAXn                                       5
#define HWIO_RPM_MPU_XPU_PRTn_END0_INI(n)        \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_END0_ADDR(n), HWIO_RPM_MPU_XPU_PRTn_END0_RMSK)
#define HWIO_RPM_MPU_XPU_PRTn_END0_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_END0_ADDR(n), mask)
#define HWIO_RPM_MPU_XPU_PRTn_END0_OUTI(n,val)    \
        out_dword(HWIO_RPM_MPU_XPU_PRTn_END0_ADDR(n),val)
#define HWIO_RPM_MPU_XPU_PRTn_END0_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTn_END0_ADDR(n),mask,val,HWIO_RPM_MPU_XPU_PRTn_END0_INI(n))
#define HWIO_RPM_MPU_XPU_PRTn_END0_ADDR_BMSK                            0xfffc0
#define HWIO_RPM_MPU_XPU_PRTn_END0_ADDR_SHFT                                0x6

#define HWIO_RPM_MPU_XPU_PRTn_SCR_ADDR(n)                            (RPM_MPU_REG_BASE      + 0x00000250 + 0x80 * (n))
#define HWIO_RPM_MPU_XPU_PRTn_SCR_RMSK                                      0x1
#define HWIO_RPM_MPU_XPU_PRTn_SCR_MAXn                                        5
#define HWIO_RPM_MPU_XPU_PRTn_SCR_INI(n)        \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_SCR_ADDR(n), HWIO_RPM_MPU_XPU_PRTn_SCR_RMSK)
#define HWIO_RPM_MPU_XPU_PRTn_SCR_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_SCR_ADDR(n), mask)
#define HWIO_RPM_MPU_XPU_PRTn_SCR_OUTI(n,val)    \
        out_dword(HWIO_RPM_MPU_XPU_PRTn_SCR_ADDR(n),val)
#define HWIO_RPM_MPU_XPU_PRTn_SCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTn_SCR_ADDR(n),mask,val,HWIO_RPM_MPU_XPU_PRTn_SCR_INI(n))
#define HWIO_RPM_MPU_XPU_PRTn_SCR_NS_BMSK                                   0x1
#define HWIO_RPM_MPU_XPU_PRTn_SCR_NS_SHFT                                   0x0

#define HWIO_RPM_MPU_XPU_PRTn_MCR_ADDR(n)                            (RPM_MPU_REG_BASE      + 0x00000254 + 0x80 * (n))
#define HWIO_RPM_MPU_XPU_PRTn_MCR_RMSK                                      0x7
#define HWIO_RPM_MPU_XPU_PRTn_MCR_MAXn                                        5
#define HWIO_RPM_MPU_XPU_PRTn_MCR_INI(n)        \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_MCR_ADDR(n), HWIO_RPM_MPU_XPU_PRTn_MCR_RMSK)
#define HWIO_RPM_MPU_XPU_PRTn_MCR_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_MPU_XPU_PRTn_MCR_ADDR(n), mask)
#define HWIO_RPM_MPU_XPU_PRTn_MCR_OUTI(n,val)    \
        out_dword(HWIO_RPM_MPU_XPU_PRTn_MCR_ADDR(n),val)
#define HWIO_RPM_MPU_XPU_PRTn_MCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MPU_XPU_PRTn_MCR_ADDR(n),mask,val,HWIO_RPM_MPU_XPU_PRTn_MCR_INI(n))
#define HWIO_RPM_MPU_XPU_PRTn_MCR_VMIDCLE_BMSK                              0x4
#define HWIO_RPM_MPU_XPU_PRTn_MCR_VMIDCLE_SHFT                              0x2
#define HWIO_RPM_MPU_XPU_PRTn_MCR_SCLE_BMSK                                 0x2
#define HWIO_RPM_MPU_XPU_PRTn_MCR_SCLE_SHFT                                 0x1
#define HWIO_RPM_MPU_XPU_PRTn_MCR_MSAE_BMSK                                 0x1
#define HWIO_RPM_MPU_XPU_PRTn_MCR_MSAE_SHFT                                 0x0

#define HWIO_RPM_MPU_XPU_PRTn_TRANS_COMM_ADDR(n)                     (RPM_MPU_REG_BASE      + 0x0000025c + 0x80 * (n))
#define HWIO_RPM_MPU_XPU_PRTn_TRANS_COMM_RMSK                               0x1
#define HWIO_RPM_MPU_XPU_PRTn_TRANS_COMM_MAXn                                 5
#define HWIO_RPM_MPU_XPU_PRTn_TRANS_COMM_OUTI(n,val)    \
        out_dword(HWIO_RPM_MPU_XPU_PRTn_TRANS_COMM_ADDR(n),val)
#define HWIO_RPM_MPU_XPU_PRTn_TRANS_COMM_CSRC_BMSK                          0x1
#define HWIO_RPM_MPU_XPU_PRTn_TRANS_COMM_CSRC_SHFT                          0x0

/*----------------------------------------------------------------------------
 * MODULE: RPM_MSG_RAM_RPU
 *--------------------------------------------------------------------------*/

#define RPM_MSG_RAM_RPU_REG_BASE                                         (RPM_MSG_RAM_BASE      + 0x00000000)

#define HWIO_RPM_MSG_RAM_XPU_SCR_ADDR                                    (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000000)
#define HWIO_RPM_MSG_RAM_XPU_SCR_RMSK                                         0x17f
#define HWIO_RPM_MSG_RAM_XPU_SCR_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SCR_ADDR, HWIO_RPM_MSG_RAM_XPU_SCR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_SCR_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SCR_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_SCR_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_SCR_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_SCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_SCR_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_SCR_IN)
#define HWIO_RPM_MSG_RAM_XPU_SCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_RPM_MSG_RAM_XPU_SCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_RPM_MSG_RAM_XPU_SCR_NSRGCLEE_BMSK                                 0x40
#define HWIO_RPM_MSG_RAM_XPU_SCR_NSRGCLEE_SHFT                                  0x6
#define HWIO_RPM_MSG_RAM_XPU_SCR_NSCFGE_BMSK                                   0x20
#define HWIO_RPM_MSG_RAM_XPU_SCR_NSCFGE_SHFT                                    0x5
#define HWIO_RPM_MSG_RAM_XPU_SCR_SDCDEE_BMSK                                   0x10
#define HWIO_RPM_MSG_RAM_XPU_SCR_SDCDEE_SHFT                                    0x4
#define HWIO_RPM_MSG_RAM_XPU_SCR_SEIE_BMSK                                      0x8
#define HWIO_RPM_MSG_RAM_XPU_SCR_SEIE_SHFT                                      0x3
#define HWIO_RPM_MSG_RAM_XPU_SCR_SCLERE_BMSK                                    0x4
#define HWIO_RPM_MSG_RAM_XPU_SCR_SCLERE_SHFT                                    0x2
#define HWIO_RPM_MSG_RAM_XPU_SCR_SCFGERE_BMSK                                   0x2
#define HWIO_RPM_MSG_RAM_XPU_SCR_SCFGERE_SHFT                                   0x1
#define HWIO_RPM_MSG_RAM_XPU_SCR_XPUNSE_BMSK                                    0x1
#define HWIO_RPM_MSG_RAM_XPU_SCR_XPUNSE_SHFT                                    0x0

#define HWIO_RPM_MSG_RAM_XPU_SWDR_ADDR                                   (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000004)
#define HWIO_RPM_MSG_RAM_XPU_SWDR_RMSK                                          0x1
#define HWIO_RPM_MSG_RAM_XPU_SWDR_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SWDR_ADDR, HWIO_RPM_MSG_RAM_XPU_SWDR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_SWDR_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SWDR_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_SWDR_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_SWDR_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_SWDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_SWDR_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_SWDR_IN)
#define HWIO_RPM_MSG_RAM_XPU_SWDR_SCFGWD_BMSK                                   0x1
#define HWIO_RPM_MSG_RAM_XPU_SWDR_SCFGWD_SHFT                                   0x0

#define HWIO_RPM_MSG_RAM_XPU_SEAR0_ADDR                                  (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000040)
#define HWIO_RPM_MSG_RAM_XPU_SEAR0_RMSK                                  0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_SEAR0_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SEAR0_ADDR, HWIO_RPM_MSG_RAM_XPU_SEAR0_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_SEAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SEAR0_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_SEAR0_PA_BMSK                               0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_SEAR0_PA_SHFT                                      0x0

#define HWIO_RPM_MSG_RAM_XPU_SESR_ADDR                                   (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000048)
#define HWIO_RPM_MSG_RAM_XPU_SESR_RMSK                                   0x80000003
#define HWIO_RPM_MSG_RAM_XPU_SESR_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESR_ADDR, HWIO_RPM_MSG_RAM_XPU_SESR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_SESR_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESR_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_SESR_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_SESR_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_SESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_SESR_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_SESR_IN)
#define HWIO_RPM_MSG_RAM_XPU_SESR_MULTI_BMSK                             0x80000000
#define HWIO_RPM_MSG_RAM_XPU_SESR_MULTI_SHFT                                   0x1f
#define HWIO_RPM_MSG_RAM_XPU_SESR_CLIENT_BMSK                                   0x2
#define HWIO_RPM_MSG_RAM_XPU_SESR_CLIENT_SHFT                                   0x1
#define HWIO_RPM_MSG_RAM_XPU_SESR_CFG_BMSK                                      0x1
#define HWIO_RPM_MSG_RAM_XPU_SESR_CFG_SHFT                                      0x0

#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_ADDR                            (RPM_MSG_RAM_RPU_REG_BASE      + 0x0000004c)
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_RMSK                            0x80000003
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_ADDR, HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_IN)
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_CFG_BMSK                               0x1
#define HWIO_RPM_MSG_RAM_XPU_SESRRESTORE_CFG_SHFT                               0x0

#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_ADDR                                (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000050)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_RMSK                                0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESYNR0_ADDR, HWIO_RPM_MSG_RAM_XPU_SESYNR0_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESYNR0_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_ATID_BMSK                           0xff000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_ATID_SHFT                                 0x18
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_AVMID_SHFT                                0x10
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_ABID_BMSK                               0xe000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_ABID_SHFT                                  0xd
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_APID_BMSK                               0x1f00
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_APID_SHFT                                  0x8
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_AMID_BMSK                                 0xff
#define HWIO_RPM_MSG_RAM_XPU_SESYNR0_AMID_SHFT                                  0x0

#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ADDR                                (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000054)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_RMSK                                0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESYNR1_ADDR, HWIO_RPM_MSG_RAM_XPU_SESYNR1_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESYNR1_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_DCD_BMSK                            0x80000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_DCD_SHFT                                  0x1f
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AC_BMSK                             0x40000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AC_SHFT                                   0x1e
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ABURST_SHFT                               0x1b
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AWRITE_SHFT                               0x19
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AFULL_SHFT                                0x18
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AOOO_BMSK                             0x400000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AOOO_SHFT                                 0x16
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ASIZE_SHFT                                0x10
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ALEN_SHFT                                  0x8
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AINST_BMSK                                0x80
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AINST_SHFT                                 0x7
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_APROTNS_BMSK                              0x40
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_APROTNS_SHFT                               0x6
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_APRIV_BMSK                                0x20
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_APRIV_SHFT                                 0x5
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ASHARED_BMSK                               0x8
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_ASHARED_SHFT                               0x3
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_RPM_MSG_RAM_XPU_SESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_ADDR                                (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000058)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_RMSK                                       0x7
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESYNR2_ADDR, HWIO_RPM_MSG_RAM_XPU_SESYNR2_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_SESYNR2_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_RPM_MSG_RAM_XPU_SESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_RPM_MSG_RAM_XPU_MCR_ADDR                                    (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000100)
#define HWIO_RPM_MSG_RAM_XPU_MCR_RMSK                                         0x11f
#define HWIO_RPM_MSG_RAM_XPU_MCR_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MCR_ADDR, HWIO_RPM_MSG_RAM_XPU_MCR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_MCR_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MCR_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_MCR_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_MCR_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_MCR_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_MCR_IN)
#define HWIO_RPM_MSG_RAM_XPU_MCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_RPM_MSG_RAM_XPU_MCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_RPM_MSG_RAM_XPU_MCR_DCDEE_BMSK                                    0x10
#define HWIO_RPM_MSG_RAM_XPU_MCR_DCDEE_SHFT                                     0x4
#define HWIO_RPM_MSG_RAM_XPU_MCR_EIE_BMSK                                       0x8
#define HWIO_RPM_MSG_RAM_XPU_MCR_EIE_SHFT                                       0x3
#define HWIO_RPM_MSG_RAM_XPU_MCR_CLERE_BMSK                                     0x4
#define HWIO_RPM_MSG_RAM_XPU_MCR_CLERE_SHFT                                     0x2
#define HWIO_RPM_MSG_RAM_XPU_MCR_CFGERE_BMSK                                    0x2
#define HWIO_RPM_MSG_RAM_XPU_MCR_CFGERE_SHFT                                    0x1
#define HWIO_RPM_MSG_RAM_XPU_MCR_XPUMSAE_BMSK                                   0x1
#define HWIO_RPM_MSG_RAM_XPU_MCR_XPUMSAE_SHFT                                   0x0

#define HWIO_RPM_MSG_RAM_XPU_MEAR0_ADDR                                  (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000140)
#define HWIO_RPM_MSG_RAM_XPU_MEAR0_RMSK                                  0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_MEAR0_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MEAR0_ADDR, HWIO_RPM_MSG_RAM_XPU_MEAR0_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_MEAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MEAR0_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_MEAR0_PA_BMSK                               0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_MEAR0_PA_SHFT                                      0x0

#define HWIO_RPM_MSG_RAM_XPU_MESR_ADDR                                   (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000148)
#define HWIO_RPM_MSG_RAM_XPU_MESR_RMSK                                   0x80000003
#define HWIO_RPM_MSG_RAM_XPU_MESR_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESR_ADDR, HWIO_RPM_MSG_RAM_XPU_MESR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_MESR_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESR_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_MESR_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_MESR_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_MESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_MESR_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_MESR_IN)
#define HWIO_RPM_MSG_RAM_XPU_MESR_MULTI_BMSK                             0x80000000
#define HWIO_RPM_MSG_RAM_XPU_MESR_MULTI_SHFT                                   0x1f
#define HWIO_RPM_MSG_RAM_XPU_MESR_CLIENT_BMSK                                   0x2
#define HWIO_RPM_MSG_RAM_XPU_MESR_CLIENT_SHFT                                   0x1
#define HWIO_RPM_MSG_RAM_XPU_MESR_CFG_BMSK                                      0x1
#define HWIO_RPM_MSG_RAM_XPU_MESR_CFG_SHFT                                      0x0

#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_ADDR                            (RPM_MSG_RAM_RPU_REG_BASE      + 0x0000014c)
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_RMSK                            0x80000003
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_ADDR, HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_IN)
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_CFG_BMSK                               0x1
#define HWIO_RPM_MSG_RAM_XPU_MESRRESTORE_CFG_SHFT                               0x0

#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_ADDR                                (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000150)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_RMSK                                0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESYNR0_ADDR, HWIO_RPM_MSG_RAM_XPU_MESYNR0_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESYNR0_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_ATID_BMSK                           0xff000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_ATID_SHFT                                 0x18
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_AVMID_SHFT                                0x10
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_ABID_BMSK                               0xe000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_ABID_SHFT                                  0xd
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_APID_BMSK                               0x1f00
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_APID_SHFT                                  0x8
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_AMID_BMSK                                 0xff
#define HWIO_RPM_MSG_RAM_XPU_MESYNR0_AMID_SHFT                                  0x0

#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ADDR                                (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000154)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_RMSK                                0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESYNR1_ADDR, HWIO_RPM_MSG_RAM_XPU_MESYNR1_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESYNR1_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_DCD_BMSK                            0x80000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_DCD_SHFT                                  0x1f
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AC_BMSK                             0x40000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AC_SHFT                                   0x1e
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ABURST_SHFT                               0x1b
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AWRITE_SHFT                               0x19
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AFULL_SHFT                                0x18
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AOOO_BMSK                             0x400000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AOOO_SHFT                                 0x16
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ASIZE_SHFT                                0x10
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ALEN_SHFT                                  0x8
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AINST_BMSK                                0x80
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AINST_SHFT                                 0x7
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_APROTNS_BMSK                              0x40
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_APROTNS_SHFT                               0x6
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_APRIV_BMSK                                0x20
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_APRIV_SHFT                                 0x5
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ASHARED_BMSK                               0x8
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_ASHARED_SHFT                               0x3
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_RPM_MSG_RAM_XPU_MESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_ADDR                                (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000158)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_RMSK                                       0x7
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESYNR2_ADDR, HWIO_RPM_MSG_RAM_XPU_MESYNR2_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_MESYNR2_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_RPM_MSG_RAM_XPU_MESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_RPM_MSG_RAM_XPU_CR_ADDR                                     (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000080)
#define HWIO_RPM_MSG_RAM_XPU_CR_RMSK                                          0x11f
#define HWIO_RPM_MSG_RAM_XPU_CR_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_CR_ADDR, HWIO_RPM_MSG_RAM_XPU_CR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_CR_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_CR_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_CR_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_CR_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_CR_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_CR_IN)
#define HWIO_RPM_MSG_RAM_XPU_CR_DYNAMIC_CLK_EN_BMSK                           0x100
#define HWIO_RPM_MSG_RAM_XPU_CR_DYNAMIC_CLK_EN_SHFT                             0x8
#define HWIO_RPM_MSG_RAM_XPU_CR_DCDEE_BMSK                                     0x10
#define HWIO_RPM_MSG_RAM_XPU_CR_DCDEE_SHFT                                      0x4
#define HWIO_RPM_MSG_RAM_XPU_CR_EIE_BMSK                                        0x8
#define HWIO_RPM_MSG_RAM_XPU_CR_EIE_SHFT                                        0x3
#define HWIO_RPM_MSG_RAM_XPU_CR_CLERE_BMSK                                      0x4
#define HWIO_RPM_MSG_RAM_XPU_CR_CLERE_SHFT                                      0x2
#define HWIO_RPM_MSG_RAM_XPU_CR_CFGERE_BMSK                                     0x2
#define HWIO_RPM_MSG_RAM_XPU_CR_CFGERE_SHFT                                     0x1
#define HWIO_RPM_MSG_RAM_XPU_CR_XPUVMIDE_BMSK                                   0x1
#define HWIO_RPM_MSG_RAM_XPU_CR_XPUVMIDE_SHFT                                   0x0

#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_ADDR(n)                            (RPM_MSG_RAM_RPU_REG_BASE      + 0x000000a0 + 0x4 * (n))
#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_RMSK                               0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_MAXn                                        0
#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_INI(n)        \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_ADDR(n), HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_ADDR(n), mask)
#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_OUTI(n,val)    \
        out_dword(HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_ADDR(n),val)
#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_ADDR(n),mask,val,HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_INI(n))
#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_RWE_BMSK                           0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_RPU_ACRn_RWE_SHFT                                  0x0

#define HWIO_RPM_MSG_RAM_XPU_EAR0_ADDR                                   (RPM_MSG_RAM_RPU_REG_BASE      + 0x000000c0)
#define HWIO_RPM_MSG_RAM_XPU_EAR0_RMSK                                   0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_EAR0_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_EAR0_ADDR, HWIO_RPM_MSG_RAM_XPU_EAR0_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_EAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_EAR0_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_EAR0_PA_BMSK                                0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_EAR0_PA_SHFT                                       0x0

#define HWIO_RPM_MSG_RAM_XPU_ESR_ADDR                                    (RPM_MSG_RAM_RPU_REG_BASE      + 0x000000c8)
#define HWIO_RPM_MSG_RAM_XPU_ESR_RMSK                                    0x80000003
#define HWIO_RPM_MSG_RAM_XPU_ESR_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESR_ADDR, HWIO_RPM_MSG_RAM_XPU_ESR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_ESR_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESR_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_ESR_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_ESR_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_ESR_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_ESR_IN)
#define HWIO_RPM_MSG_RAM_XPU_ESR_MULTI_BMSK                              0x80000000
#define HWIO_RPM_MSG_RAM_XPU_ESR_MULTI_SHFT                                    0x1f
#define HWIO_RPM_MSG_RAM_XPU_ESR_CLIENT_BMSK                                    0x2
#define HWIO_RPM_MSG_RAM_XPU_ESR_CLIENT_SHFT                                    0x1
#define HWIO_RPM_MSG_RAM_XPU_ESR_CFG_BMSK                                       0x1
#define HWIO_RPM_MSG_RAM_XPU_ESR_CFG_SHFT                                       0x0

#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_ADDR                             (RPM_MSG_RAM_RPU_REG_BASE      + 0x000000cc)
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_RMSK                             0x80000003
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_ADDR, HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_ADDR,v)
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_ADDR,m,v,HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_IN)
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_MULTI_BMSK                       0x80000000
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_MULTI_SHFT                             0x1f
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_CLIENT_BMSK                             0x2
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_CLIENT_SHFT                             0x1
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_CFG_BMSK                                0x1
#define HWIO_RPM_MSG_RAM_XPU_ESRRESTORE_CFG_SHFT                                0x0

#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_ADDR                                 (RPM_MSG_RAM_RPU_REG_BASE      + 0x000000d0)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_RMSK                                 0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESYNR0_ADDR, HWIO_RPM_MSG_RAM_XPU_ESYNR0_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESYNR0_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_ATID_BMSK                            0xff000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_ATID_SHFT                                  0x18
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_AVMID_BMSK                             0xff0000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_AVMID_SHFT                                 0x10
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_ABID_BMSK                                0xe000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_ABID_SHFT                                   0xd
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_APID_BMSK                                0x1f00
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_APID_SHFT                                   0x8
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_AMID_BMSK                                  0xff
#define HWIO_RPM_MSG_RAM_XPU_ESYNR0_AMID_SHFT                                   0x0

#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ADDR                                 (RPM_MSG_RAM_RPU_REG_BASE      + 0x000000d4)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_RMSK                                 0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESYNR1_ADDR, HWIO_RPM_MSG_RAM_XPU_ESYNR1_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESYNR1_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_DCD_BMSK                             0x80000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_DCD_SHFT                                   0x1f
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AC_BMSK                              0x40000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AC_SHFT                                    0x1e
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_BURSTLEN_BMSK                        0x20000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_BURSTLEN_SHFT                              0x1d
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ARDALLOCATE_BMSK                     0x10000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ARDALLOCATE_SHFT                           0x1c
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ABURST_BMSK                           0x8000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ABURST_SHFT                                0x1b
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AEXCLUSIVE_BMSK                       0x4000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AEXCLUSIVE_SHFT                            0x1a
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AWRITE_BMSK                           0x2000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AWRITE_SHFT                                0x19
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AFULL_BMSK                            0x1000000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AFULL_SHFT                                 0x18
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ARDBEADNDXEN_BMSK                      0x800000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ARDBEADNDXEN_SHFT                          0x17
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AOOO_BMSK                              0x400000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AOOO_SHFT                                  0x16
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_APREQPRIORITY_BMSK                     0x380000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_APREQPRIORITY_SHFT                         0x13
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ASIZE_BMSK                              0x70000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ASIZE_SHFT                                 0x10
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AMSSSELFAUTH_BMSK                        0x8000
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AMSSSELFAUTH_SHFT                           0xf
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ALEN_BMSK                                0x7f00
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ALEN_SHFT                                   0x8
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AINST_BMSK                                 0x80
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AINST_SHFT                                  0x7
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_APROTNS_BMSK                               0x40
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_APROTNS_SHFT                                0x6
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_APRIV_BMSK                                 0x20
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_APRIV_SHFT                                  0x5
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AINNERSHARED_BMSK                          0x10
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AINNERSHARED_SHFT                           0x4
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ASHARED_BMSK                                0x8
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_ASHARED_SHFT                                0x3
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AMEMTYPE_BMSK                               0x7
#define HWIO_RPM_MSG_RAM_XPU_ESYNR1_AMEMTYPE_SHFT                               0x0

#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_ADDR                                 (RPM_MSG_RAM_RPU_REG_BASE      + 0x000000d8)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_RMSK                                        0x7
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESYNR2_ADDR, HWIO_RPM_MSG_RAM_XPU_ESYNR2_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_ESYNR2_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_MODEM_PRT_HIT_BMSK                          0x4
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_MODEM_PRT_HIT_SHFT                          0x2
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_SECURE_PRT_HIT_BMSK                         0x2
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_SECURE_PRT_HIT_SHFT                         0x1
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_NONSECURE_PRT_HIT_BMSK                      0x1
#define HWIO_RPM_MSG_RAM_XPU_ESYNR2_NONSECURE_PRT_HIT_SHFT                      0x0

#define HWIO_RPM_MSG_RAM_XPU_IDR0_ADDR                                   (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000074)
#define HWIO_RPM_MSG_RAM_XPU_IDR0_RMSK                                   0xc03fbfff
#define HWIO_RPM_MSG_RAM_XPU_IDR0_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_IDR0_ADDR, HWIO_RPM_MSG_RAM_XPU_IDR0_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_IDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_IDR0_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_BMSK          0x80000000
#define HWIO_RPM_MSG_RAM_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_SHFT                0x1f
#define HWIO_RPM_MSG_RAM_XPU_IDR0_SAVERESTORE_HW_EN_BMSK                 0x40000000
#define HWIO_RPM_MSG_RAM_XPU_IDR0_SAVERESTORE_HW_EN_SHFT                       0x1e
#define HWIO_RPM_MSG_RAM_XPU_IDR0_LSB_BMSK                                 0x3f0000
#define HWIO_RPM_MSG_RAM_XPU_IDR0_LSB_SHFT                                     0x10
#define HWIO_RPM_MSG_RAM_XPU_IDR0_BLED_BMSK                                  0x8000
#define HWIO_RPM_MSG_RAM_XPU_IDR0_BLED_SHFT                                     0xf
#define HWIO_RPM_MSG_RAM_XPU_IDR0_XPUT_BMSK                                  0x3000
#define HWIO_RPM_MSG_RAM_XPU_IDR0_XPUT_SHFT                                     0xc
#define HWIO_RPM_MSG_RAM_XPU_IDR0_PT_BMSK                                     0x800
#define HWIO_RPM_MSG_RAM_XPU_IDR0_PT_SHFT                                       0xb
#define HWIO_RPM_MSG_RAM_XPU_IDR0_MV_BMSK                                     0x400
#define HWIO_RPM_MSG_RAM_XPU_IDR0_MV_SHFT                                       0xa
#define HWIO_RPM_MSG_RAM_XPU_IDR0_NRG_BMSK                                    0x3ff
#define HWIO_RPM_MSG_RAM_XPU_IDR0_NRG_SHFT                                      0x0

#define HWIO_RPM_MSG_RAM_XPU_IDR1_ADDR                                   (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000078)
#define HWIO_RPM_MSG_RAM_XPU_IDR1_RMSK                                   0x7f3ffeff
#define HWIO_RPM_MSG_RAM_XPU_IDR1_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_IDR1_ADDR, HWIO_RPM_MSG_RAM_XPU_IDR1_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_IDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_IDR1_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_IDR1_AMT_HW_ENABLE_BMSK                     0x40000000
#define HWIO_RPM_MSG_RAM_XPU_IDR1_AMT_HW_ENABLE_SHFT                           0x1e
#define HWIO_RPM_MSG_RAM_XPU_IDR1_CLIENT_ADDR_WIDTH_BMSK                 0x3f000000
#define HWIO_RPM_MSG_RAM_XPU_IDR1_CLIENT_ADDR_WIDTH_SHFT                       0x18
#define HWIO_RPM_MSG_RAM_XPU_IDR1_CONFIG_ADDR_WIDTH_BMSK                   0x3f0000
#define HWIO_RPM_MSG_RAM_XPU_IDR1_CONFIG_ADDR_WIDTH_SHFT                       0x10
#define HWIO_RPM_MSG_RAM_XPU_IDR1_QRIB_EN_BMSK                               0x8000
#define HWIO_RPM_MSG_RAM_XPU_IDR1_QRIB_EN_SHFT                                  0xf
#define HWIO_RPM_MSG_RAM_XPU_IDR1_ASYNC_MODE_BMSK                            0x4000
#define HWIO_RPM_MSG_RAM_XPU_IDR1_ASYNC_MODE_SHFT                               0xe
#define HWIO_RPM_MSG_RAM_XPU_IDR1_CONFIG_TYPE_BMSK                           0x2000
#define HWIO_RPM_MSG_RAM_XPU_IDR1_CONFIG_TYPE_SHFT                              0xd
#define HWIO_RPM_MSG_RAM_XPU_IDR1_CLIENT_PIPELINE_ENABLED_BMSK               0x1000
#define HWIO_RPM_MSG_RAM_XPU_IDR1_CLIENT_PIPELINE_ENABLED_SHFT                  0xc
#define HWIO_RPM_MSG_RAM_XPU_IDR1_MSA_CHECK_HW_ENABLE_BMSK                    0x800
#define HWIO_RPM_MSG_RAM_XPU_IDR1_MSA_CHECK_HW_ENABLE_SHFT                      0xb
#define HWIO_RPM_MSG_RAM_XPU_IDR1_XPU_SYND_REG_ABSENT_BMSK                    0x400
#define HWIO_RPM_MSG_RAM_XPU_IDR1_XPU_SYND_REG_ABSENT_SHFT                      0xa
#define HWIO_RPM_MSG_RAM_XPU_IDR1_TZXPU_BMSK                                  0x200
#define HWIO_RPM_MSG_RAM_XPU_IDR1_TZXPU_SHFT                                    0x9
#define HWIO_RPM_MSG_RAM_XPU_IDR1_NVMID_BMSK                                   0xff
#define HWIO_RPM_MSG_RAM_XPU_IDR1_NVMID_SHFT                                    0x0

#define HWIO_RPM_MSG_RAM_XPU_REV_ADDR                                    (RPM_MSG_RAM_RPU_REG_BASE      + 0x0000007c)
#define HWIO_RPM_MSG_RAM_XPU_REV_RMSK                                    0xffffffff
#define HWIO_RPM_MSG_RAM_XPU_REV_IN          \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_REV_ADDR, HWIO_RPM_MSG_RAM_XPU_REV_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_REV_INM(m)      \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_REV_ADDR, m)
#define HWIO_RPM_MSG_RAM_XPU_REV_MAJOR_BMSK                              0xf0000000
#define HWIO_RPM_MSG_RAM_XPU_REV_MAJOR_SHFT                                    0x1c
#define HWIO_RPM_MSG_RAM_XPU_REV_MINOR_BMSK                               0xfff0000
#define HWIO_RPM_MSG_RAM_XPU_REV_MINOR_SHFT                                    0x10
#define HWIO_RPM_MSG_RAM_XPU_REV_STEP_BMSK                                   0xffff
#define HWIO_RPM_MSG_RAM_XPU_REV_STEP_SHFT                                      0x0

#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ADDR(n,m)                         (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000200 + 0x80 * (n)+0x4 * (m))
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_RMSK                               0x3ff03ff
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_MAXn                                      31
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_MAXm                                       0
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_INI2(n,m)        \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ADDR(n,m), HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_INMI2(n,m,mask)    \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ADDR(n,m), mask)
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_OUTI2(n,m,val)    \
        out_dword(HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ADDR(n,m),val)
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_OUTMI2(n,m,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ADDR(n,m),mask,val,HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_INI2(n,m))
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ROGE_BMSK                          0x2000000
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ROGE_SHFT                               0x19
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ROE_BMSK                           0x1000000
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ROE_SHFT                                0x18
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ROVMID_BMSK                         0xff0000
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_ROVMID_SHFT                             0x10
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_RWGE_BMSK                              0x200
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_RWGE_SHFT                                0x9
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_RWE_BMSK                               0x100
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_RWE_SHFT                                 0x8
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_RWVMID_BMSK                             0xff
#define HWIO_RPM_MSG_RAM_XPU_RGn_RACRm_RWVMID_SHFT                              0x0

#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_ADDR(n)                             (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000250 + 0x80 * (n))
#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_RMSK                                       0x1
#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_MAXn                                        31
#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_INI(n)        \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_RGn_SCR_ADDR(n), HWIO_RPM_MSG_RAM_XPU_RGn_SCR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_RGn_SCR_ADDR(n), mask)
#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_OUTI(n,val)    \
        out_dword(HWIO_RPM_MSG_RAM_XPU_RGn_SCR_ADDR(n),val)
#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_RGn_SCR_ADDR(n),mask,val,HWIO_RPM_MSG_RAM_XPU_RGn_SCR_INI(n))
#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_NS_BMSK                                    0x1
#define HWIO_RPM_MSG_RAM_XPU_RGn_SCR_NS_SHFT                                    0x0

#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_ADDR(n)                             (RPM_MSG_RAM_RPU_REG_BASE      + 0x00000254 + 0x80 * (n))
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_RMSK                                       0x7
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_MAXn                                        31
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_INI(n)        \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_RGn_MCR_ADDR(n), HWIO_RPM_MSG_RAM_XPU_RGn_MCR_RMSK)
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_MSG_RAM_XPU_RGn_MCR_ADDR(n), mask)
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_OUTI(n,val)    \
        out_dword(HWIO_RPM_MSG_RAM_XPU_RGn_MCR_ADDR(n),val)
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_MSG_RAM_XPU_RGn_MCR_ADDR(n),mask,val,HWIO_RPM_MSG_RAM_XPU_RGn_MCR_INI(n))
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_VMIDCLE_BMSK                               0x4
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_VMIDCLE_SHFT                               0x2
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_SCLE_BMSK                                  0x2
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_SCLE_SHFT                                  0x1
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_MSAE_BMSK                                  0x1
#define HWIO_RPM_MSG_RAM_XPU_RGn_MCR_MSAE_SHFT                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: RPM_QTMR_AC
 *--------------------------------------------------------------------------*/

#define RPM_QTMR_AC_REG_BASE                                   (RPM_CSR_BASE      + 0x00002000)

#define HWIO_RPM_QTMR_AC_CNTFRQ_ADDR                           (RPM_QTMR_AC_REG_BASE      + 0x00000000)
#define HWIO_RPM_QTMR_AC_CNTFRQ_RMSK                           0xffffffff
#define HWIO_RPM_QTMR_AC_CNTFRQ_IN          \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTFRQ_ADDR, HWIO_RPM_QTMR_AC_CNTFRQ_RMSK)
#define HWIO_RPM_QTMR_AC_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTFRQ_ADDR, m)
#define HWIO_RPM_QTMR_AC_CNTFRQ_OUT(v)      \
        out_dword(HWIO_RPM_QTMR_AC_CNTFRQ_ADDR,v)
#define HWIO_RPM_QTMR_AC_CNTFRQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_QTMR_AC_CNTFRQ_ADDR,m,v,HWIO_RPM_QTMR_AC_CNTFRQ_IN)
#define HWIO_RPM_QTMR_AC_CNTFRQ_CNTFRQ_BMSK                    0xffffffff
#define HWIO_RPM_QTMR_AC_CNTFRQ_CNTFRQ_SHFT                           0x0

#define HWIO_RPM_QTMR_AC_CNTSR_ADDR                            (RPM_QTMR_AC_REG_BASE      + 0x00000004)
#define HWIO_RPM_QTMR_AC_CNTSR_RMSK                                   0x3
#define HWIO_RPM_QTMR_AC_CNTSR_IN          \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTSR_ADDR, HWIO_RPM_QTMR_AC_CNTSR_RMSK)
#define HWIO_RPM_QTMR_AC_CNTSR_INM(m)      \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTSR_ADDR, m)
#define HWIO_RPM_QTMR_AC_CNTSR_OUT(v)      \
        out_dword(HWIO_RPM_QTMR_AC_CNTSR_ADDR,v)
#define HWIO_RPM_QTMR_AC_CNTSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_QTMR_AC_CNTSR_ADDR,m,v,HWIO_RPM_QTMR_AC_CNTSR_IN)
#define HWIO_RPM_QTMR_AC_CNTSR_NSN_BMSK                               0x3
#define HWIO_RPM_QTMR_AC_CNTSR_NSN_SHFT                               0x0

#define HWIO_RPM_QTMR_AC_CNTTID_ADDR                           (RPM_QTMR_AC_REG_BASE      + 0x00000008)
#define HWIO_RPM_QTMR_AC_CNTTID_RMSK                           0xffffffff
#define HWIO_RPM_QTMR_AC_CNTTID_IN          \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTTID_ADDR, HWIO_RPM_QTMR_AC_CNTTID_RMSK)
#define HWIO_RPM_QTMR_AC_CNTTID_INM(m)      \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTTID_ADDR, m)
#define HWIO_RPM_QTMR_AC_CNTTID_F7_CFG_BMSK                    0xf0000000
#define HWIO_RPM_QTMR_AC_CNTTID_F7_CFG_SHFT                          0x1c
#define HWIO_RPM_QTMR_AC_CNTTID_F6_CFG_BMSK                     0xf000000
#define HWIO_RPM_QTMR_AC_CNTTID_F6_CFG_SHFT                          0x18
#define HWIO_RPM_QTMR_AC_CNTTID_F5_CFG_BMSK                      0xf00000
#define HWIO_RPM_QTMR_AC_CNTTID_F5_CFG_SHFT                          0x14
#define HWIO_RPM_QTMR_AC_CNTTID_F4_CFG_BMSK                       0xf0000
#define HWIO_RPM_QTMR_AC_CNTTID_F4_CFG_SHFT                          0x10
#define HWIO_RPM_QTMR_AC_CNTTID_F3_CFG_BMSK                        0xf000
#define HWIO_RPM_QTMR_AC_CNTTID_F3_CFG_SHFT                           0xc
#define HWIO_RPM_QTMR_AC_CNTTID_F2_CFG_BMSK                         0xf00
#define HWIO_RPM_QTMR_AC_CNTTID_F2_CFG_SHFT                           0x8
#define HWIO_RPM_QTMR_AC_CNTTID_F1_CFG_BMSK                          0xf0
#define HWIO_RPM_QTMR_AC_CNTTID_F1_CFG_SHFT                           0x4
#define HWIO_RPM_QTMR_AC_CNTTID_F0_CFG_BMSK                           0xf
#define HWIO_RPM_QTMR_AC_CNTTID_F0_CFG_SHFT                           0x0

#define HWIO_RPM_QTMR_AC_CNTACR_n_ADDR(n)                      (RPM_QTMR_AC_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_RPM_QTMR_AC_CNTACR_n_RMSK                               0x3f
#define HWIO_RPM_QTMR_AC_CNTACR_n_MAXn                                  1
#define HWIO_RPM_QTMR_AC_CNTACR_n_INI(n)        \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTACR_n_ADDR(n), HWIO_RPM_QTMR_AC_CNTACR_n_RMSK)
#define HWIO_RPM_QTMR_AC_CNTACR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTACR_n_ADDR(n), mask)
#define HWIO_RPM_QTMR_AC_CNTACR_n_OUTI(n,val)    \
        out_dword(HWIO_RPM_QTMR_AC_CNTACR_n_ADDR(n),val)
#define HWIO_RPM_QTMR_AC_CNTACR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_QTMR_AC_CNTACR_n_ADDR(n),mask,val,HWIO_RPM_QTMR_AC_CNTACR_n_INI(n))
#define HWIO_RPM_QTMR_AC_CNTACR_n_RWPT_BMSK                          0x20
#define HWIO_RPM_QTMR_AC_CNTACR_n_RWPT_SHFT                           0x5
#define HWIO_RPM_QTMR_AC_CNTACR_n_RWVT_BMSK                          0x10
#define HWIO_RPM_QTMR_AC_CNTACR_n_RWVT_SHFT                           0x4
#define HWIO_RPM_QTMR_AC_CNTACR_n_RVOFF_BMSK                          0x8
#define HWIO_RPM_QTMR_AC_CNTACR_n_RVOFF_SHFT                          0x3
#define HWIO_RPM_QTMR_AC_CNTACR_n_RFRQ_BMSK                           0x4
#define HWIO_RPM_QTMR_AC_CNTACR_n_RFRQ_SHFT                           0x2
#define HWIO_RPM_QTMR_AC_CNTACR_n_RPVCT_BMSK                          0x2
#define HWIO_RPM_QTMR_AC_CNTACR_n_RPVCT_SHFT                          0x1
#define HWIO_RPM_QTMR_AC_CNTACR_n_RPCT_BMSK                           0x1
#define HWIO_RPM_QTMR_AC_CNTACR_n_RPCT_SHFT                           0x0

#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_ADDR(n)                  (RPM_QTMR_AC_REG_BASE      + 0x00000080 + 0x8 * (n))
#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_RMSK                     0xffffffff
#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_MAXn                              1
#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_INI(n)        \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_ADDR(n), HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_RMSK)
#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_ADDR(n), mask)
#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_OUTI(n,val)    \
        out_dword(HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_ADDR(n),val)
#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_ADDR(n),mask,val,HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_INI(n))
#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_BMSK          0xffffffff
#define HWIO_RPM_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_SHFT                 0x0

#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_ADDR(n)                  (RPM_QTMR_AC_REG_BASE      + 0x00000084 + 0x8 * (n))
#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_RMSK                       0xffffff
#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_MAXn                              1
#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_INI(n)        \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_ADDR(n), HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_RMSK)
#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_ADDR(n), mask)
#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_OUTI(n,val)    \
        out_dword(HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_ADDR(n),val)
#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_ADDR(n),mask,val,HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_INI(n))
#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_BMSK            0xffffff
#define HWIO_RPM_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_SHFT                 0x0

#define HWIO_RPM_QTMR_AC_VERSION_ADDR                          (RPM_QTMR_AC_REG_BASE      + 0x00000fd0)
#define HWIO_RPM_QTMR_AC_VERSION_RMSK                          0xffffffff
#define HWIO_RPM_QTMR_AC_VERSION_IN          \
        in_dword_masked(HWIO_RPM_QTMR_AC_VERSION_ADDR, HWIO_RPM_QTMR_AC_VERSION_RMSK)
#define HWIO_RPM_QTMR_AC_VERSION_INM(m)      \
        in_dword_masked(HWIO_RPM_QTMR_AC_VERSION_ADDR, m)
#define HWIO_RPM_QTMR_AC_VERSION_MAJOR_BMSK                    0xf0000000
#define HWIO_RPM_QTMR_AC_VERSION_MAJOR_SHFT                          0x1c
#define HWIO_RPM_QTMR_AC_VERSION_MINOR_BMSK                     0xfff0000
#define HWIO_RPM_QTMR_AC_VERSION_MINOR_SHFT                          0x10
#define HWIO_RPM_QTMR_AC_VERSION_STEP_BMSK                         0xffff
#define HWIO_RPM_QTMR_AC_VERSION_STEP_SHFT                            0x0

/*----------------------------------------------------------------------------
 * MODULE: RPM_VMIDMT
 *--------------------------------------------------------------------------*/

#define RPM_VMIDMT_REG_BASE                                      (RPM_CSR_BASE      + 0x00008000)

#define HWIO_RPM_VMIDMT_SCR0_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_RPM_VMIDMT_SCR0_RMSK                                0x3fd707f5
#define HWIO_RPM_VMIDMT_SCR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SCR0_ADDR, HWIO_RPM_VMIDMT_SCR0_RMSK)
#define HWIO_RPM_VMIDMT_SCR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SCR0_ADDR, m)
#define HWIO_RPM_VMIDMT_SCR0_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_SCR0_ADDR,v)
#define HWIO_RPM_VMIDMT_SCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_SCR0_ADDR,m,v,HWIO_RPM_VMIDMT_SCR0_IN)
#define HWIO_RPM_VMIDMT_SCR0_NSCFG_BMSK                          0x30000000
#define HWIO_RPM_VMIDMT_SCR0_NSCFG_SHFT                                0x1c
#define HWIO_RPM_VMIDMT_SCR0_WACFG_BMSK                           0xc000000
#define HWIO_RPM_VMIDMT_SCR0_WACFG_SHFT                                0x1a
#define HWIO_RPM_VMIDMT_SCR0_RACFG_BMSK                           0x3000000
#define HWIO_RPM_VMIDMT_SCR0_RACFG_SHFT                                0x18
#define HWIO_RPM_VMIDMT_SCR0_SHCFG_BMSK                            0xc00000
#define HWIO_RPM_VMIDMT_SCR0_SHCFG_SHFT                                0x16
#define HWIO_RPM_VMIDMT_SCR0_MTCFG_BMSK                            0x100000
#define HWIO_RPM_VMIDMT_SCR0_MTCFG_SHFT                                0x14
#define HWIO_RPM_VMIDMT_SCR0_MEMATTR_BMSK                           0x70000
#define HWIO_RPM_VMIDMT_SCR0_MEMATTR_SHFT                              0x10
#define HWIO_RPM_VMIDMT_SCR0_USFCFG_BMSK                              0x400
#define HWIO_RPM_VMIDMT_SCR0_USFCFG_SHFT                                0xa
#define HWIO_RPM_VMIDMT_SCR0_GSE_BMSK                                 0x200
#define HWIO_RPM_VMIDMT_SCR0_GSE_SHFT                                   0x9
#define HWIO_RPM_VMIDMT_SCR0_STALLD_BMSK                              0x100
#define HWIO_RPM_VMIDMT_SCR0_STALLD_SHFT                                0x8
#define HWIO_RPM_VMIDMT_SCR0_TRANSIENTCFG_BMSK                         0xc0
#define HWIO_RPM_VMIDMT_SCR0_TRANSIENTCFG_SHFT                          0x6
#define HWIO_RPM_VMIDMT_SCR0_GCFGFIE_BMSK                              0x20
#define HWIO_RPM_VMIDMT_SCR0_GCFGFIE_SHFT                               0x5
#define HWIO_RPM_VMIDMT_SCR0_GCFGERE_BMSK                              0x10
#define HWIO_RPM_VMIDMT_SCR0_GCFGERE_SHFT                               0x4
#define HWIO_RPM_VMIDMT_SCR0_GFIE_BMSK                                  0x4
#define HWIO_RPM_VMIDMT_SCR0_GFIE_SHFT                                  0x2
#define HWIO_RPM_VMIDMT_SCR0_CLIENTPD_BMSK                              0x1
#define HWIO_RPM_VMIDMT_SCR0_CLIENTPD_SHFT                              0x0

#define HWIO_RPM_VMIDMT_SCR1_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000004)
#define HWIO_RPM_VMIDMT_SCR1_RMSK                                 0x1000100
#define HWIO_RPM_VMIDMT_SCR1_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SCR1_ADDR, HWIO_RPM_VMIDMT_SCR1_RMSK)
#define HWIO_RPM_VMIDMT_SCR1_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SCR1_ADDR, m)
#define HWIO_RPM_VMIDMT_SCR1_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_SCR1_ADDR,v)
#define HWIO_RPM_VMIDMT_SCR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_SCR1_ADDR,m,v,HWIO_RPM_VMIDMT_SCR1_IN)
#define HWIO_RPM_VMIDMT_SCR1_GASRAE_BMSK                          0x1000000
#define HWIO_RPM_VMIDMT_SCR1_GASRAE_SHFT                               0x18
#define HWIO_RPM_VMIDMT_SCR1_NSNUMSMRGO_BMSK                          0x100
#define HWIO_RPM_VMIDMT_SCR1_NSNUMSMRGO_SHFT                            0x8

#define HWIO_RPM_VMIDMT_SCR2_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_RPM_VMIDMT_SCR2_RMSK                                      0x1f
#define HWIO_RPM_VMIDMT_SCR2_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SCR2_ADDR, HWIO_RPM_VMIDMT_SCR2_RMSK)
#define HWIO_RPM_VMIDMT_SCR2_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SCR2_ADDR, m)
#define HWIO_RPM_VMIDMT_SCR2_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_SCR2_ADDR,v)
#define HWIO_RPM_VMIDMT_SCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_SCR2_ADDR,m,v,HWIO_RPM_VMIDMT_SCR2_IN)
#define HWIO_RPM_VMIDMT_SCR2_BPVMID_BMSK                               0x1f
#define HWIO_RPM_VMIDMT_SCR2_BPVMID_SHFT                                0x0

#define HWIO_RPM_VMIDMT_SACR_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_RPM_VMIDMT_SACR_RMSK                                0x70000013
#define HWIO_RPM_VMIDMT_SACR_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SACR_ADDR, HWIO_RPM_VMIDMT_SACR_RMSK)
#define HWIO_RPM_VMIDMT_SACR_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SACR_ADDR, m)
#define HWIO_RPM_VMIDMT_SACR_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_SACR_ADDR,v)
#define HWIO_RPM_VMIDMT_SACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_SACR_ADDR,m,v,HWIO_RPM_VMIDMT_SACR_IN)
#define HWIO_RPM_VMIDMT_SACR_BPRCNSH_BMSK                        0x40000000
#define HWIO_RPM_VMIDMT_SACR_BPRCNSH_SHFT                              0x1e
#define HWIO_RPM_VMIDMT_SACR_BPRCISH_BMSK                        0x20000000
#define HWIO_RPM_VMIDMT_SACR_BPRCISH_SHFT                              0x1d
#define HWIO_RPM_VMIDMT_SACR_BPRCOSH_BMSK                        0x10000000
#define HWIO_RPM_VMIDMT_SACR_BPRCOSH_SHFT                              0x1c
#define HWIO_RPM_VMIDMT_SACR_BPREQPRIORITYCFG_BMSK                     0x10
#define HWIO_RPM_VMIDMT_SACR_BPREQPRIORITYCFG_SHFT                      0x4
#define HWIO_RPM_VMIDMT_SACR_BPREQPRIORITY_BMSK                         0x3
#define HWIO_RPM_VMIDMT_SACR_BPREQPRIORITY_SHFT                         0x0

#define HWIO_RPM_VMIDMT_SIDR0_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_RPM_VMIDMT_SIDR0_RMSK                               0x88001eff
#define HWIO_RPM_VMIDMT_SIDR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR0_ADDR, HWIO_RPM_VMIDMT_SIDR0_RMSK)
#define HWIO_RPM_VMIDMT_SIDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR0_ADDR, m)
#define HWIO_RPM_VMIDMT_SIDR0_SES_BMSK                           0x80000000
#define HWIO_RPM_VMIDMT_SIDR0_SES_SHFT                                 0x1f
#define HWIO_RPM_VMIDMT_SIDR0_SMS_BMSK                            0x8000000
#define HWIO_RPM_VMIDMT_SIDR0_SMS_SHFT                                 0x1b
#define HWIO_RPM_VMIDMT_SIDR0_NUMSIDB_BMSK                           0x1e00
#define HWIO_RPM_VMIDMT_SIDR0_NUMSIDB_SHFT                              0x9
#define HWIO_RPM_VMIDMT_SIDR0_NUMSMRG_BMSK                             0xff
#define HWIO_RPM_VMIDMT_SIDR0_NUMSMRG_SHFT                              0x0

#define HWIO_RPM_VMIDMT_SIDR1_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_RPM_VMIDMT_SIDR1_RMSK                                   0x9f00
#define HWIO_RPM_VMIDMT_SIDR1_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR1_ADDR, HWIO_RPM_VMIDMT_SIDR1_RMSK)
#define HWIO_RPM_VMIDMT_SIDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR1_ADDR, m)
#define HWIO_RPM_VMIDMT_SIDR1_SMCD_BMSK                              0x8000
#define HWIO_RPM_VMIDMT_SIDR1_SMCD_SHFT                                 0xf
#define HWIO_RPM_VMIDMT_SIDR1_SSDTP_BMSK                             0x1000
#define HWIO_RPM_VMIDMT_SIDR1_SSDTP_SHFT                                0xc
#define HWIO_RPM_VMIDMT_SIDR1_NUMSSDNDX_BMSK                          0xf00
#define HWIO_RPM_VMIDMT_SIDR1_NUMSSDNDX_SHFT                            0x8

#define HWIO_RPM_VMIDMT_SIDR2_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_RPM_VMIDMT_SIDR2_RMSK                                     0xff
#define HWIO_RPM_VMIDMT_SIDR2_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR2_ADDR, HWIO_RPM_VMIDMT_SIDR2_RMSK)
#define HWIO_RPM_VMIDMT_SIDR2_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR2_ADDR, m)
#define HWIO_RPM_VMIDMT_SIDR2_OAS_BMSK                                 0xf0
#define HWIO_RPM_VMIDMT_SIDR2_OAS_SHFT                                  0x4
#define HWIO_RPM_VMIDMT_SIDR2_IAS_BMSK                                  0xf
#define HWIO_RPM_VMIDMT_SIDR2_IAS_SHFT                                  0x0

#define HWIO_RPM_VMIDMT_SIDR4_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_RPM_VMIDMT_SIDR4_RMSK                               0xffffffff
#define HWIO_RPM_VMIDMT_SIDR4_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR4_ADDR, HWIO_RPM_VMIDMT_SIDR4_RMSK)
#define HWIO_RPM_VMIDMT_SIDR4_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR4_ADDR, m)
#define HWIO_RPM_VMIDMT_SIDR4_MAJOR_BMSK                         0xf0000000
#define HWIO_RPM_VMIDMT_SIDR4_MAJOR_SHFT                               0x1c
#define HWIO_RPM_VMIDMT_SIDR4_MINOR_BMSK                          0xfff0000
#define HWIO_RPM_VMIDMT_SIDR4_MINOR_SHFT                               0x10
#define HWIO_RPM_VMIDMT_SIDR4_STEP_BMSK                              0xffff
#define HWIO_RPM_VMIDMT_SIDR4_STEP_SHFT                                 0x0

#define HWIO_RPM_VMIDMT_SIDR5_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_RPM_VMIDMT_SIDR5_RMSK                                 0xff03ff
#define HWIO_RPM_VMIDMT_SIDR5_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR5_ADDR, HWIO_RPM_VMIDMT_SIDR5_RMSK)
#define HWIO_RPM_VMIDMT_SIDR5_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR5_ADDR, m)
#define HWIO_RPM_VMIDMT_SIDR5_NUMMSDRB_BMSK                        0xff0000
#define HWIO_RPM_VMIDMT_SIDR5_NUMMSDRB_SHFT                            0x10
#define HWIO_RPM_VMIDMT_SIDR5_MSAE_BMSK                               0x200
#define HWIO_RPM_VMIDMT_SIDR5_MSAE_SHFT                                 0x9
#define HWIO_RPM_VMIDMT_SIDR5_QRIBE_BMSK                              0x100
#define HWIO_RPM_VMIDMT_SIDR5_QRIBE_SHFT                                0x8
#define HWIO_RPM_VMIDMT_SIDR5_NVMID_BMSK                               0xff
#define HWIO_RPM_VMIDMT_SIDR5_NVMID_SHFT                                0x0

#define HWIO_RPM_VMIDMT_SIDR7_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_RPM_VMIDMT_SIDR7_RMSK                                     0xff
#define HWIO_RPM_VMIDMT_SIDR7_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR7_ADDR, HWIO_RPM_VMIDMT_SIDR7_RMSK)
#define HWIO_RPM_VMIDMT_SIDR7_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SIDR7_ADDR, m)
#define HWIO_RPM_VMIDMT_SIDR7_MAJOR_BMSK                               0xf0
#define HWIO_RPM_VMIDMT_SIDR7_MAJOR_SHFT                                0x4
#define HWIO_RPM_VMIDMT_SIDR7_MINOR_BMSK                                0xf
#define HWIO_RPM_VMIDMT_SIDR7_MINOR_SHFT                                0x0

#define HWIO_RPM_VMIDMT_SGFAR0_ADDR                              (RPM_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_RPM_VMIDMT_SGFAR0_RMSK                              0xffffffff
#define HWIO_RPM_VMIDMT_SGFAR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFAR0_ADDR, HWIO_RPM_VMIDMT_SGFAR0_RMSK)
#define HWIO_RPM_VMIDMT_SGFAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFAR0_ADDR, m)
#define HWIO_RPM_VMIDMT_SGFAR0_SGFEA0_BMSK                       0xffffffff
#define HWIO_RPM_VMIDMT_SGFAR0_SGFEA0_SHFT                              0x0

#define HWIO_RPM_VMIDMT_SGFSR_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_RPM_VMIDMT_SGFSR_RMSK                               0xc0000022
#define HWIO_RPM_VMIDMT_SGFSR_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSR_ADDR, HWIO_RPM_VMIDMT_SGFSR_RMSK)
#define HWIO_RPM_VMIDMT_SGFSR_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSR_ADDR, m)
#define HWIO_RPM_VMIDMT_SGFSR_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_SGFSR_ADDR,v)
#define HWIO_RPM_VMIDMT_SGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_SGFSR_ADDR,m,v,HWIO_RPM_VMIDMT_SGFSR_IN)
#define HWIO_RPM_VMIDMT_SGFSR_MULTI_CLIENT_BMSK                  0x80000000
#define HWIO_RPM_VMIDMT_SGFSR_MULTI_CLIENT_SHFT                        0x1f
#define HWIO_RPM_VMIDMT_SGFSR_MULTI_CFG_BMSK                     0x40000000
#define HWIO_RPM_VMIDMT_SGFSR_MULTI_CFG_SHFT                           0x1e
#define HWIO_RPM_VMIDMT_SGFSR_CAF_BMSK                                 0x20
#define HWIO_RPM_VMIDMT_SGFSR_CAF_SHFT                                  0x5
#define HWIO_RPM_VMIDMT_SGFSR_USF_BMSK                                  0x2
#define HWIO_RPM_VMIDMT_SGFSR_USF_SHFT                                  0x1

#define HWIO_RPM_VMIDMT_SGFSRRESTORE_ADDR                        (RPM_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_RMSK                        0xc0000022
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSRRESTORE_ADDR, HWIO_RPM_VMIDMT_SGFSRRESTORE_RMSK)
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSRRESTORE_ADDR, m)
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_SGFSRRESTORE_ADDR,v)
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_SGFSRRESTORE_ADDR,m,v,HWIO_RPM_VMIDMT_SGFSRRESTORE_IN)
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_BMSK           0x80000000
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_SHFT                 0x1f
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_MULTI_CFG_BMSK              0x40000000
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_MULTI_CFG_SHFT                    0x1e
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_CAF_BMSK                          0x20
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_CAF_SHFT                           0x5
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_USF_BMSK                           0x2
#define HWIO_RPM_VMIDMT_SGFSRRESTORE_USF_SHFT                           0x1

#define HWIO_RPM_VMIDMT_SGFSYNDR0_ADDR                           (RPM_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_RPM_VMIDMT_SGFSYNDR0_RMSK                                0x132
#define HWIO_RPM_VMIDMT_SGFSYNDR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSYNDR0_ADDR, HWIO_RPM_VMIDMT_SGFSYNDR0_RMSK)
#define HWIO_RPM_VMIDMT_SGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSYNDR0_ADDR, m)
#define HWIO_RPM_VMIDMT_SGFSYNDR0_MSSSELFAUTH_BMSK                    0x100
#define HWIO_RPM_VMIDMT_SGFSYNDR0_MSSSELFAUTH_SHFT                      0x8
#define HWIO_RPM_VMIDMT_SGFSYNDR0_NSATTR_BMSK                          0x20
#define HWIO_RPM_VMIDMT_SGFSYNDR0_NSATTR_SHFT                           0x5
#define HWIO_RPM_VMIDMT_SGFSYNDR0_NSSTATE_BMSK                         0x10
#define HWIO_RPM_VMIDMT_SGFSYNDR0_NSSTATE_SHFT                          0x4
#define HWIO_RPM_VMIDMT_SGFSYNDR0_WNR_BMSK                              0x2
#define HWIO_RPM_VMIDMT_SGFSYNDR0_WNR_SHFT                              0x1

#define HWIO_RPM_VMIDMT_SGFSYNDR1_ADDR                           (RPM_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_RPM_VMIDMT_SGFSYNDR1_RMSK                            0x1010001
#define HWIO_RPM_VMIDMT_SGFSYNDR1_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSYNDR1_ADDR, HWIO_RPM_VMIDMT_SGFSYNDR1_RMSK)
#define HWIO_RPM_VMIDMT_SGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSYNDR1_ADDR, m)
#define HWIO_RPM_VMIDMT_SGFSYNDR1_MSDINDEX_BMSK                   0x1000000
#define HWIO_RPM_VMIDMT_SGFSYNDR1_MSDINDEX_SHFT                        0x18
#define HWIO_RPM_VMIDMT_SGFSYNDR1_SSDINDEX_BMSK                     0x10000
#define HWIO_RPM_VMIDMT_SGFSYNDR1_SSDINDEX_SHFT                        0x10
#define HWIO_RPM_VMIDMT_SGFSYNDR1_STREAMINDEX_BMSK                      0x1
#define HWIO_RPM_VMIDMT_SGFSYNDR1_STREAMINDEX_SHFT                      0x0

#define HWIO_RPM_VMIDMT_SGFSYNDR2_ADDR                           (RPM_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_RPM_VMIDMT_SGFSYNDR2_RMSK                           0x3f1fffff
#define HWIO_RPM_VMIDMT_SGFSYNDR2_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSYNDR2_ADDR, HWIO_RPM_VMIDMT_SGFSYNDR2_RMSK)
#define HWIO_RPM_VMIDMT_SGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SGFSYNDR2_ADDR, m)
#define HWIO_RPM_VMIDMT_SGFSYNDR2_ATID_BMSK                      0x3f000000
#define HWIO_RPM_VMIDMT_SGFSYNDR2_ATID_SHFT                            0x18
#define HWIO_RPM_VMIDMT_SGFSYNDR2_AVMID_BMSK                       0x1f0000
#define HWIO_RPM_VMIDMT_SGFSYNDR2_AVMID_SHFT                           0x10
#define HWIO_RPM_VMIDMT_SGFSYNDR2_ABID_BMSK                          0xe000
#define HWIO_RPM_VMIDMT_SGFSYNDR2_ABID_SHFT                             0xd
#define HWIO_RPM_VMIDMT_SGFSYNDR2_APID_BMSK                          0x1f00
#define HWIO_RPM_VMIDMT_SGFSYNDR2_APID_SHFT                             0x8
#define HWIO_RPM_VMIDMT_SGFSYNDR2_AMID_BMSK                            0xff
#define HWIO_RPM_VMIDMT_SGFSYNDR2_AMID_SHFT                             0x0

#define HWIO_RPM_VMIDMT_VMIDMTSCR0_ADDR                          (RPM_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_RPM_VMIDMT_VMIDMTSCR0_RMSK                                 0x1
#define HWIO_RPM_VMIDMT_VMIDMTSCR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_VMIDMTSCR0_ADDR, HWIO_RPM_VMIDMT_VMIDMTSCR0_RMSK)
#define HWIO_RPM_VMIDMT_VMIDMTSCR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_VMIDMTSCR0_ADDR, m)
#define HWIO_RPM_VMIDMT_VMIDMTSCR0_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_VMIDMTSCR0_ADDR,v)
#define HWIO_RPM_VMIDMT_VMIDMTSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_VMIDMTSCR0_ADDR,m,v,HWIO_RPM_VMIDMT_VMIDMTSCR0_IN)
#define HWIO_RPM_VMIDMT_VMIDMTSCR0_CLKONOFFE_BMSK                       0x1
#define HWIO_RPM_VMIDMT_VMIDMTSCR0_CLKONOFFE_SHFT                       0x0

#define HWIO_RPM_VMIDMT_CR0_ADDR                                 (RPM_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_RPM_VMIDMT_CR0_RMSK                                  0xfd70ff5
#define HWIO_RPM_VMIDMT_CR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_CR0_ADDR, HWIO_RPM_VMIDMT_CR0_RMSK)
#define HWIO_RPM_VMIDMT_CR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_CR0_ADDR, m)
#define HWIO_RPM_VMIDMT_CR0_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_CR0_ADDR,v)
#define HWIO_RPM_VMIDMT_CR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_CR0_ADDR,m,v,HWIO_RPM_VMIDMT_CR0_IN)
#define HWIO_RPM_VMIDMT_CR0_WACFG_BMSK                            0xc000000
#define HWIO_RPM_VMIDMT_CR0_WACFG_SHFT                                 0x1a
#define HWIO_RPM_VMIDMT_CR0_RACFG_BMSK                            0x3000000
#define HWIO_RPM_VMIDMT_CR0_RACFG_SHFT                                 0x18
#define HWIO_RPM_VMIDMT_CR0_SHCFG_BMSK                             0xc00000
#define HWIO_RPM_VMIDMT_CR0_SHCFG_SHFT                                 0x16
#define HWIO_RPM_VMIDMT_CR0_MTCFG_BMSK                             0x100000
#define HWIO_RPM_VMIDMT_CR0_MTCFG_SHFT                                 0x14
#define HWIO_RPM_VMIDMT_CR0_MEMATTR_BMSK                            0x70000
#define HWIO_RPM_VMIDMT_CR0_MEMATTR_SHFT                               0x10
#define HWIO_RPM_VMIDMT_CR0_VMIDPNE_BMSK                              0x800
#define HWIO_RPM_VMIDMT_CR0_VMIDPNE_SHFT                                0xb
#define HWIO_RPM_VMIDMT_CR0_USFCFG_BMSK                               0x400
#define HWIO_RPM_VMIDMT_CR0_USFCFG_SHFT                                 0xa
#define HWIO_RPM_VMIDMT_CR0_GSE_BMSK                                  0x200
#define HWIO_RPM_VMIDMT_CR0_GSE_SHFT                                    0x9
#define HWIO_RPM_VMIDMT_CR0_STALLD_BMSK                               0x100
#define HWIO_RPM_VMIDMT_CR0_STALLD_SHFT                                 0x8
#define HWIO_RPM_VMIDMT_CR0_TRANSIENTCFG_BMSK                          0xc0
#define HWIO_RPM_VMIDMT_CR0_TRANSIENTCFG_SHFT                           0x6
#define HWIO_RPM_VMIDMT_CR0_GCFGFIE_BMSK                               0x20
#define HWIO_RPM_VMIDMT_CR0_GCFGFIE_SHFT                                0x5
#define HWIO_RPM_VMIDMT_CR0_GCFGERE_BMSK                               0x10
#define HWIO_RPM_VMIDMT_CR0_GCFGERE_SHFT                                0x4
#define HWIO_RPM_VMIDMT_CR0_GFIE_BMSK                                   0x4
#define HWIO_RPM_VMIDMT_CR0_GFIE_SHFT                                   0x2
#define HWIO_RPM_VMIDMT_CR0_CLIENTPD_BMSK                               0x1
#define HWIO_RPM_VMIDMT_CR0_CLIENTPD_SHFT                               0x0

#define HWIO_RPM_VMIDMT_CR2_ADDR                                 (RPM_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_RPM_VMIDMT_CR2_RMSK                                       0x1f
#define HWIO_RPM_VMIDMT_CR2_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_CR2_ADDR, HWIO_RPM_VMIDMT_CR2_RMSK)
#define HWIO_RPM_VMIDMT_CR2_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_CR2_ADDR, m)
#define HWIO_RPM_VMIDMT_CR2_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_CR2_ADDR,v)
#define HWIO_RPM_VMIDMT_CR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_CR2_ADDR,m,v,HWIO_RPM_VMIDMT_CR2_IN)
#define HWIO_RPM_VMIDMT_CR2_BPVMID_BMSK                                0x1f
#define HWIO_RPM_VMIDMT_CR2_BPVMID_SHFT                                 0x0

#define HWIO_RPM_VMIDMT_ACR_ADDR                                 (RPM_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_RPM_VMIDMT_ACR_RMSK                                 0x70000013
#define HWIO_RPM_VMIDMT_ACR_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_ACR_ADDR, HWIO_RPM_VMIDMT_ACR_RMSK)
#define HWIO_RPM_VMIDMT_ACR_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_ACR_ADDR, m)
#define HWIO_RPM_VMIDMT_ACR_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_ACR_ADDR,v)
#define HWIO_RPM_VMIDMT_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_ACR_ADDR,m,v,HWIO_RPM_VMIDMT_ACR_IN)
#define HWIO_RPM_VMIDMT_ACR_BPRCNSH_BMSK                         0x40000000
#define HWIO_RPM_VMIDMT_ACR_BPRCNSH_SHFT                               0x1e
#define HWIO_RPM_VMIDMT_ACR_BPRCISH_BMSK                         0x20000000
#define HWIO_RPM_VMIDMT_ACR_BPRCISH_SHFT                               0x1d
#define HWIO_RPM_VMIDMT_ACR_BPRCOSH_BMSK                         0x10000000
#define HWIO_RPM_VMIDMT_ACR_BPRCOSH_SHFT                               0x1c
#define HWIO_RPM_VMIDMT_ACR_BPREQPRIORITYCFG_BMSK                      0x10
#define HWIO_RPM_VMIDMT_ACR_BPREQPRIORITYCFG_SHFT                       0x4
#define HWIO_RPM_VMIDMT_ACR_BPREQPRIORITY_BMSK                          0x3
#define HWIO_RPM_VMIDMT_ACR_BPREQPRIORITY_SHFT                          0x0

#define HWIO_RPM_VMIDMT_IDR0_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_RPM_VMIDMT_IDR0_RMSK                                 0x8001eff
#define HWIO_RPM_VMIDMT_IDR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR0_ADDR, HWIO_RPM_VMIDMT_IDR0_RMSK)
#define HWIO_RPM_VMIDMT_IDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR0_ADDR, m)
#define HWIO_RPM_VMIDMT_IDR0_SMS_BMSK                             0x8000000
#define HWIO_RPM_VMIDMT_IDR0_SMS_SHFT                                  0x1b
#define HWIO_RPM_VMIDMT_IDR0_NUMSIDB_BMSK                            0x1e00
#define HWIO_RPM_VMIDMT_IDR0_NUMSIDB_SHFT                               0x9
#define HWIO_RPM_VMIDMT_IDR0_NUMSMRG_BMSK                              0xff
#define HWIO_RPM_VMIDMT_IDR0_NUMSMRG_SHFT                               0x0

#define HWIO_RPM_VMIDMT_IDR1_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_RPM_VMIDMT_IDR1_RMSK                                    0x9f00
#define HWIO_RPM_VMIDMT_IDR1_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR1_ADDR, HWIO_RPM_VMIDMT_IDR1_RMSK)
#define HWIO_RPM_VMIDMT_IDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR1_ADDR, m)
#define HWIO_RPM_VMIDMT_IDR1_SMCD_BMSK                               0x8000
#define HWIO_RPM_VMIDMT_IDR1_SMCD_SHFT                                  0xf
#define HWIO_RPM_VMIDMT_IDR1_SSDTP_BMSK                              0x1000
#define HWIO_RPM_VMIDMT_IDR1_SSDTP_SHFT                                 0xc
#define HWIO_RPM_VMIDMT_IDR1_NUMSSDNDX_BMSK                           0xf00
#define HWIO_RPM_VMIDMT_IDR1_NUMSSDNDX_SHFT                             0x8

#define HWIO_RPM_VMIDMT_IDR2_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_RPM_VMIDMT_IDR2_RMSK                                      0xff
#define HWIO_RPM_VMIDMT_IDR2_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR2_ADDR, HWIO_RPM_VMIDMT_IDR2_RMSK)
#define HWIO_RPM_VMIDMT_IDR2_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR2_ADDR, m)
#define HWIO_RPM_VMIDMT_IDR2_OAS_BMSK                                  0xf0
#define HWIO_RPM_VMIDMT_IDR2_OAS_SHFT                                   0x4
#define HWIO_RPM_VMIDMT_IDR2_IAS_BMSK                                   0xf
#define HWIO_RPM_VMIDMT_IDR2_IAS_SHFT                                   0x0

#define HWIO_RPM_VMIDMT_IDR4_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_RPM_VMIDMT_IDR4_RMSK                                0xffffffff
#define HWIO_RPM_VMIDMT_IDR4_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR4_ADDR, HWIO_RPM_VMIDMT_IDR4_RMSK)
#define HWIO_RPM_VMIDMT_IDR4_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR4_ADDR, m)
#define HWIO_RPM_VMIDMT_IDR4_MAJOR_BMSK                          0xf0000000
#define HWIO_RPM_VMIDMT_IDR4_MAJOR_SHFT                                0x1c
#define HWIO_RPM_VMIDMT_IDR4_MINOR_BMSK                           0xfff0000
#define HWIO_RPM_VMIDMT_IDR4_MINOR_SHFT                                0x10
#define HWIO_RPM_VMIDMT_IDR4_STEP_BMSK                               0xffff
#define HWIO_RPM_VMIDMT_IDR4_STEP_SHFT                                  0x0

#define HWIO_RPM_VMIDMT_IDR5_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_RPM_VMIDMT_IDR5_RMSK                                  0xff03ff
#define HWIO_RPM_VMIDMT_IDR5_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR5_ADDR, HWIO_RPM_VMIDMT_IDR5_RMSK)
#define HWIO_RPM_VMIDMT_IDR5_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR5_ADDR, m)
#define HWIO_RPM_VMIDMT_IDR5_NUMMSDRB_BMSK                         0xff0000
#define HWIO_RPM_VMIDMT_IDR5_NUMMSDRB_SHFT                             0x10
#define HWIO_RPM_VMIDMT_IDR5_MSAE_BMSK                                0x200
#define HWIO_RPM_VMIDMT_IDR5_MSAE_SHFT                                  0x9
#define HWIO_RPM_VMIDMT_IDR5_QRIBE_BMSK                               0x100
#define HWIO_RPM_VMIDMT_IDR5_QRIBE_SHFT                                 0x8
#define HWIO_RPM_VMIDMT_IDR5_NVMID_BMSK                                0xff
#define HWIO_RPM_VMIDMT_IDR5_NVMID_SHFT                                 0x0

#define HWIO_RPM_VMIDMT_IDR7_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_RPM_VMIDMT_IDR7_RMSK                                      0xff
#define HWIO_RPM_VMIDMT_IDR7_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR7_ADDR, HWIO_RPM_VMIDMT_IDR7_RMSK)
#define HWIO_RPM_VMIDMT_IDR7_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_IDR7_ADDR, m)
#define HWIO_RPM_VMIDMT_IDR7_MAJOR_BMSK                                0xf0
#define HWIO_RPM_VMIDMT_IDR7_MAJOR_SHFT                                 0x4
#define HWIO_RPM_VMIDMT_IDR7_MINOR_BMSK                                 0xf
#define HWIO_RPM_VMIDMT_IDR7_MINOR_SHFT                                 0x0

#define HWIO_RPM_VMIDMT_GFAR0_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_RPM_VMIDMT_GFAR0_RMSK                               0xffffffff
#define HWIO_RPM_VMIDMT_GFAR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_GFAR0_ADDR, HWIO_RPM_VMIDMT_GFAR0_RMSK)
#define HWIO_RPM_VMIDMT_GFAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_GFAR0_ADDR, m)
#define HWIO_RPM_VMIDMT_GFAR0_GFEA0_BMSK                         0xffffffff
#define HWIO_RPM_VMIDMT_GFAR0_GFEA0_SHFT                                0x0

#define HWIO_RPM_VMIDMT_GFSR_ADDR                                (RPM_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_RPM_VMIDMT_GFSR_RMSK                                0xc00000a2
#define HWIO_RPM_VMIDMT_GFSR_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSR_ADDR, HWIO_RPM_VMIDMT_GFSR_RMSK)
#define HWIO_RPM_VMIDMT_GFSR_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSR_ADDR, m)
#define HWIO_RPM_VMIDMT_GFSR_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_GFSR_ADDR,v)
#define HWIO_RPM_VMIDMT_GFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_GFSR_ADDR,m,v,HWIO_RPM_VMIDMT_GFSR_IN)
#define HWIO_RPM_VMIDMT_GFSR_MULTI_CLIENT_BMSK                   0x80000000
#define HWIO_RPM_VMIDMT_GFSR_MULTI_CLIENT_SHFT                         0x1f
#define HWIO_RPM_VMIDMT_GFSR_MULTI_CFG_BMSK                      0x40000000
#define HWIO_RPM_VMIDMT_GFSR_MULTI_CFG_SHFT                            0x1e
#define HWIO_RPM_VMIDMT_GFSR_PF_BMSK                                   0x80
#define HWIO_RPM_VMIDMT_GFSR_PF_SHFT                                    0x7
#define HWIO_RPM_VMIDMT_GFSR_CAF_BMSK                                  0x20
#define HWIO_RPM_VMIDMT_GFSR_CAF_SHFT                                   0x5
#define HWIO_RPM_VMIDMT_GFSR_USF_BMSK                                   0x2
#define HWIO_RPM_VMIDMT_GFSR_USF_SHFT                                   0x1

#define HWIO_RPM_VMIDMT_GFSRRESTORE_ADDR                         (RPM_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_RPM_VMIDMT_GFSRRESTORE_RMSK                         0xc00000a2
#define HWIO_RPM_VMIDMT_GFSRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSRRESTORE_ADDR, HWIO_RPM_VMIDMT_GFSRRESTORE_RMSK)
#define HWIO_RPM_VMIDMT_GFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSRRESTORE_ADDR, m)
#define HWIO_RPM_VMIDMT_GFSRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_GFSRRESTORE_ADDR,v)
#define HWIO_RPM_VMIDMT_GFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_GFSRRESTORE_ADDR,m,v,HWIO_RPM_VMIDMT_GFSRRESTORE_IN)
#define HWIO_RPM_VMIDMT_GFSRRESTORE_MULTI_CLIENT_BMSK            0x80000000
#define HWIO_RPM_VMIDMT_GFSRRESTORE_MULTI_CLIENT_SHFT                  0x1f
#define HWIO_RPM_VMIDMT_GFSRRESTORE_MULTI_CFG_BMSK               0x40000000
#define HWIO_RPM_VMIDMT_GFSRRESTORE_MULTI_CFG_SHFT                     0x1e
#define HWIO_RPM_VMIDMT_GFSRRESTORE_PF_BMSK                            0x80
#define HWIO_RPM_VMIDMT_GFSRRESTORE_PF_SHFT                             0x7
#define HWIO_RPM_VMIDMT_GFSRRESTORE_CAF_BMSK                           0x20
#define HWIO_RPM_VMIDMT_GFSRRESTORE_CAF_SHFT                            0x5
#define HWIO_RPM_VMIDMT_GFSRRESTORE_USF_BMSK                            0x2
#define HWIO_RPM_VMIDMT_GFSRRESTORE_USF_SHFT                            0x1

#define HWIO_RPM_VMIDMT_GFSYNDR0_ADDR                            (RPM_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_RPM_VMIDMT_GFSYNDR0_RMSK                                 0x132
#define HWIO_RPM_VMIDMT_GFSYNDR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSYNDR0_ADDR, HWIO_RPM_VMIDMT_GFSYNDR0_RMSK)
#define HWIO_RPM_VMIDMT_GFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSYNDR0_ADDR, m)
#define HWIO_RPM_VMIDMT_GFSYNDR0_MSSSELFAUTH_BMSK                     0x100
#define HWIO_RPM_VMIDMT_GFSYNDR0_MSSSELFAUTH_SHFT                       0x8
#define HWIO_RPM_VMIDMT_GFSYNDR0_NSATTR_BMSK                           0x20
#define HWIO_RPM_VMIDMT_GFSYNDR0_NSATTR_SHFT                            0x5
#define HWIO_RPM_VMIDMT_GFSYNDR0_NSSTATE_BMSK                          0x10
#define HWIO_RPM_VMIDMT_GFSYNDR0_NSSTATE_SHFT                           0x4
#define HWIO_RPM_VMIDMT_GFSYNDR0_WNR_BMSK                               0x2
#define HWIO_RPM_VMIDMT_GFSYNDR0_WNR_SHFT                               0x1

#define HWIO_RPM_VMIDMT_GFSYNDR1_ADDR                            (RPM_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_RPM_VMIDMT_GFSYNDR1_RMSK                             0x1010001
#define HWIO_RPM_VMIDMT_GFSYNDR1_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSYNDR1_ADDR, HWIO_RPM_VMIDMT_GFSYNDR1_RMSK)
#define HWIO_RPM_VMIDMT_GFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSYNDR1_ADDR, m)
#define HWIO_RPM_VMIDMT_GFSYNDR1_MSDINDEX_BMSK                    0x1000000
#define HWIO_RPM_VMIDMT_GFSYNDR1_MSDINDEX_SHFT                         0x18
#define HWIO_RPM_VMIDMT_GFSYNDR1_SSDINDEX_BMSK                      0x10000
#define HWIO_RPM_VMIDMT_GFSYNDR1_SSDINDEX_SHFT                         0x10
#define HWIO_RPM_VMIDMT_GFSYNDR1_STREAMINDEX_BMSK                       0x1
#define HWIO_RPM_VMIDMT_GFSYNDR1_STREAMINDEX_SHFT                       0x0

#define HWIO_RPM_VMIDMT_GFSYNDR2_ADDR                            (RPM_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_RPM_VMIDMT_GFSYNDR2_RMSK                            0x3f1fffff
#define HWIO_RPM_VMIDMT_GFSYNDR2_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSYNDR2_ADDR, HWIO_RPM_VMIDMT_GFSYNDR2_RMSK)
#define HWIO_RPM_VMIDMT_GFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_GFSYNDR2_ADDR, m)
#define HWIO_RPM_VMIDMT_GFSYNDR2_ATID_BMSK                       0x3f000000
#define HWIO_RPM_VMIDMT_GFSYNDR2_ATID_SHFT                             0x18
#define HWIO_RPM_VMIDMT_GFSYNDR2_AVMID_BMSK                        0x1f0000
#define HWIO_RPM_VMIDMT_GFSYNDR2_AVMID_SHFT                            0x10
#define HWIO_RPM_VMIDMT_GFSYNDR2_ABID_BMSK                           0xe000
#define HWIO_RPM_VMIDMT_GFSYNDR2_ABID_SHFT                              0xd
#define HWIO_RPM_VMIDMT_GFSYNDR2_APID_BMSK                           0x1f00
#define HWIO_RPM_VMIDMT_GFSYNDR2_APID_SHFT                              0x8
#define HWIO_RPM_VMIDMT_GFSYNDR2_AMID_BMSK                             0xff
#define HWIO_RPM_VMIDMT_GFSYNDR2_AMID_SHFT                              0x0

#define HWIO_RPM_VMIDMT_VMIDMTCR0_ADDR                           (RPM_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_RPM_VMIDMT_VMIDMTCR0_RMSK                                  0x1
#define HWIO_RPM_VMIDMT_VMIDMTCR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_VMIDMTCR0_ADDR, HWIO_RPM_VMIDMT_VMIDMTCR0_RMSK)
#define HWIO_RPM_VMIDMT_VMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_VMIDMTCR0_ADDR, m)
#define HWIO_RPM_VMIDMT_VMIDMTCR0_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_VMIDMTCR0_ADDR,v)
#define HWIO_RPM_VMIDMT_VMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_VMIDMTCR0_ADDR,m,v,HWIO_RPM_VMIDMT_VMIDMTCR0_IN)
#define HWIO_RPM_VMIDMT_VMIDMTCR0_CLKONOFFE_BMSK                        0x1
#define HWIO_RPM_VMIDMT_VMIDMTCR0_CLKONOFFE_SHFT                        0x0

#define HWIO_RPM_VMIDMT_VMIDMTACR_ADDR                           (RPM_VMIDMT_REG_BASE      + 0x0000009c)
#define HWIO_RPM_VMIDMT_VMIDMTACR_RMSK                           0xffffffff
#define HWIO_RPM_VMIDMT_VMIDMTACR_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_VMIDMTACR_ADDR, HWIO_RPM_VMIDMT_VMIDMTACR_RMSK)
#define HWIO_RPM_VMIDMT_VMIDMTACR_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_VMIDMTACR_ADDR, m)
#define HWIO_RPM_VMIDMT_VMIDMTACR_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_VMIDMTACR_ADDR,v)
#define HWIO_RPM_VMIDMT_VMIDMTACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_VMIDMTACR_ADDR,m,v,HWIO_RPM_VMIDMT_VMIDMTACR_IN)
#define HWIO_RPM_VMIDMT_VMIDMTACR_RWE_BMSK                       0xffffffff
#define HWIO_RPM_VMIDMT_VMIDMTACR_RWE_SHFT                              0x0

#define HWIO_RPM_VMIDMT_NSCR0_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000400)
#define HWIO_RPM_VMIDMT_NSCR0_RMSK                                0xfd70ff5
#define HWIO_RPM_VMIDMT_NSCR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSCR0_ADDR, HWIO_RPM_VMIDMT_NSCR0_RMSK)
#define HWIO_RPM_VMIDMT_NSCR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSCR0_ADDR, m)
#define HWIO_RPM_VMIDMT_NSCR0_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_NSCR0_ADDR,v)
#define HWIO_RPM_VMIDMT_NSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_NSCR0_ADDR,m,v,HWIO_RPM_VMIDMT_NSCR0_IN)
#define HWIO_RPM_VMIDMT_NSCR0_WACFG_BMSK                          0xc000000
#define HWIO_RPM_VMIDMT_NSCR0_WACFG_SHFT                               0x1a
#define HWIO_RPM_VMIDMT_NSCR0_RACFG_BMSK                          0x3000000
#define HWIO_RPM_VMIDMT_NSCR0_RACFG_SHFT                               0x18
#define HWIO_RPM_VMIDMT_NSCR0_SHCFG_BMSK                           0xc00000
#define HWIO_RPM_VMIDMT_NSCR0_SHCFG_SHFT                               0x16
#define HWIO_RPM_VMIDMT_NSCR0_MTCFG_BMSK                           0x100000
#define HWIO_RPM_VMIDMT_NSCR0_MTCFG_SHFT                               0x14
#define HWIO_RPM_VMIDMT_NSCR0_MEMATTR_BMSK                          0x70000
#define HWIO_RPM_VMIDMT_NSCR0_MEMATTR_SHFT                             0x10
#define HWIO_RPM_VMIDMT_NSCR0_VMIDPNE_BMSK                            0x800
#define HWIO_RPM_VMIDMT_NSCR0_VMIDPNE_SHFT                              0xb
#define HWIO_RPM_VMIDMT_NSCR0_USFCFG_BMSK                             0x400
#define HWIO_RPM_VMIDMT_NSCR0_USFCFG_SHFT                               0xa
#define HWIO_RPM_VMIDMT_NSCR0_GSE_BMSK                                0x200
#define HWIO_RPM_VMIDMT_NSCR0_GSE_SHFT                                  0x9
#define HWIO_RPM_VMIDMT_NSCR0_STALLD_BMSK                             0x100
#define HWIO_RPM_VMIDMT_NSCR0_STALLD_SHFT                               0x8
#define HWIO_RPM_VMIDMT_NSCR0_TRANSIENTCFG_BMSK                        0xc0
#define HWIO_RPM_VMIDMT_NSCR0_TRANSIENTCFG_SHFT                         0x6
#define HWIO_RPM_VMIDMT_NSCR0_GCFGFIE_BMSK                             0x20
#define HWIO_RPM_VMIDMT_NSCR0_GCFGFIE_SHFT                              0x5
#define HWIO_RPM_VMIDMT_NSCR0_GCFGERE_BMSK                             0x10
#define HWIO_RPM_VMIDMT_NSCR0_GCFGERE_SHFT                              0x4
#define HWIO_RPM_VMIDMT_NSCR0_GFIE_BMSK                                 0x4
#define HWIO_RPM_VMIDMT_NSCR0_GFIE_SHFT                                 0x2
#define HWIO_RPM_VMIDMT_NSCR0_CLIENTPD_BMSK                             0x1
#define HWIO_RPM_VMIDMT_NSCR0_CLIENTPD_SHFT                             0x0

#define HWIO_RPM_VMIDMT_NSCR2_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000408)
#define HWIO_RPM_VMIDMT_NSCR2_RMSK                                     0x1f
#define HWIO_RPM_VMIDMT_NSCR2_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSCR2_ADDR, HWIO_RPM_VMIDMT_NSCR2_RMSK)
#define HWIO_RPM_VMIDMT_NSCR2_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSCR2_ADDR, m)
#define HWIO_RPM_VMIDMT_NSCR2_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_NSCR2_ADDR,v)
#define HWIO_RPM_VMIDMT_NSCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_NSCR2_ADDR,m,v,HWIO_RPM_VMIDMT_NSCR2_IN)
#define HWIO_RPM_VMIDMT_NSCR2_BPVMID_BMSK                              0x1f
#define HWIO_RPM_VMIDMT_NSCR2_BPVMID_SHFT                               0x0

#define HWIO_RPM_VMIDMT_NSACR_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000410)
#define HWIO_RPM_VMIDMT_NSACR_RMSK                               0x70000013
#define HWIO_RPM_VMIDMT_NSACR_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSACR_ADDR, HWIO_RPM_VMIDMT_NSACR_RMSK)
#define HWIO_RPM_VMIDMT_NSACR_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSACR_ADDR, m)
#define HWIO_RPM_VMIDMT_NSACR_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_NSACR_ADDR,v)
#define HWIO_RPM_VMIDMT_NSACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_NSACR_ADDR,m,v,HWIO_RPM_VMIDMT_NSACR_IN)
#define HWIO_RPM_VMIDMT_NSACR_BPRCNSH_BMSK                       0x40000000
#define HWIO_RPM_VMIDMT_NSACR_BPRCNSH_SHFT                             0x1e
#define HWIO_RPM_VMIDMT_NSACR_BPRCISH_BMSK                       0x20000000
#define HWIO_RPM_VMIDMT_NSACR_BPRCISH_SHFT                             0x1d
#define HWIO_RPM_VMIDMT_NSACR_BPRCOSH_BMSK                       0x10000000
#define HWIO_RPM_VMIDMT_NSACR_BPRCOSH_SHFT                             0x1c
#define HWIO_RPM_VMIDMT_NSACR_BPREQPRIORITYCFG_BMSK                    0x10
#define HWIO_RPM_VMIDMT_NSACR_BPREQPRIORITYCFG_SHFT                     0x4
#define HWIO_RPM_VMIDMT_NSACR_BPREQPRIORITY_BMSK                        0x3
#define HWIO_RPM_VMIDMT_NSACR_BPREQPRIORITY_SHFT                        0x0

#define HWIO_RPM_VMIDMT_NSGFAR0_ADDR                             (RPM_VMIDMT_REG_BASE      + 0x00000440)
#define HWIO_RPM_VMIDMT_NSGFAR0_RMSK                             0xffffffff
#define HWIO_RPM_VMIDMT_NSGFAR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFAR0_ADDR, HWIO_RPM_VMIDMT_NSGFAR0_RMSK)
#define HWIO_RPM_VMIDMT_NSGFAR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFAR0_ADDR, m)
#define HWIO_RPM_VMIDMT_NSGFAR0_GFEA0_BMSK                       0xffffffff
#define HWIO_RPM_VMIDMT_NSGFAR0_GFEA0_SHFT                              0x0

#define HWIO_RPM_VMIDMT_NSGFSR_ADDR                              (RPM_VMIDMT_REG_BASE      + 0x00000448)
#define HWIO_RPM_VMIDMT_NSGFSR_RMSK                              0xc00000a2
#define HWIO_RPM_VMIDMT_NSGFSR_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSR_ADDR, HWIO_RPM_VMIDMT_NSGFSR_RMSK)
#define HWIO_RPM_VMIDMT_NSGFSR_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSR_ADDR, m)
#define HWIO_RPM_VMIDMT_NSGFSR_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_NSGFSR_ADDR,v)
#define HWIO_RPM_VMIDMT_NSGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_NSGFSR_ADDR,m,v,HWIO_RPM_VMIDMT_NSGFSR_IN)
#define HWIO_RPM_VMIDMT_NSGFSR_MULTI_CLIENT_BMSK                 0x80000000
#define HWIO_RPM_VMIDMT_NSGFSR_MULTI_CLIENT_SHFT                       0x1f
#define HWIO_RPM_VMIDMT_NSGFSR_MULTI_CFG_BMSK                    0x40000000
#define HWIO_RPM_VMIDMT_NSGFSR_MULTI_CFG_SHFT                          0x1e
#define HWIO_RPM_VMIDMT_NSGFSR_PF_BMSK                                 0x80
#define HWIO_RPM_VMIDMT_NSGFSR_PF_SHFT                                  0x7
#define HWIO_RPM_VMIDMT_NSGFSR_CAF_BMSK                                0x20
#define HWIO_RPM_VMIDMT_NSGFSR_CAF_SHFT                                 0x5
#define HWIO_RPM_VMIDMT_NSGFSR_USF_BMSK                                 0x2
#define HWIO_RPM_VMIDMT_NSGFSR_USF_SHFT                                 0x1

#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_ADDR                       (RPM_VMIDMT_REG_BASE      + 0x0000044c)
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_RMSK                       0xc00000a2
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSRRESTORE_ADDR, HWIO_RPM_VMIDMT_NSGFSRRESTORE_RMSK)
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSRRESTORE_ADDR, m)
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_NSGFSRRESTORE_ADDR,v)
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_NSGFSRRESTORE_ADDR,m,v,HWIO_RPM_VMIDMT_NSGFSRRESTORE_IN)
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_BMSK          0x80000000
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_SHFT                0x1f
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_MULTI_CFG_BMSK             0x40000000
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_MULTI_CFG_SHFT                   0x1e
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_PF_BMSK                          0x80
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_PF_SHFT                           0x7
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_CAF_BMSK                         0x20
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_CAF_SHFT                          0x5
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_USF_BMSK                          0x2
#define HWIO_RPM_VMIDMT_NSGFSRRESTORE_USF_SHFT                          0x1

#define HWIO_RPM_VMIDMT_NSGFSYNDR0_ADDR                          (RPM_VMIDMT_REG_BASE      + 0x00000450)
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_RMSK                               0x132
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSYNDR0_ADDR, HWIO_RPM_VMIDMT_NSGFSYNDR0_RMSK)
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSYNDR0_ADDR, m)
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_BMSK                   0x100
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_SHFT                     0x8
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_NSATTR_BMSK                         0x20
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_NSATTR_SHFT                          0x5
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_NSSTATE_BMSK                        0x10
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_NSSTATE_SHFT                         0x4
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_WNR_BMSK                             0x2
#define HWIO_RPM_VMIDMT_NSGFSYNDR0_WNR_SHFT                             0x1

#define HWIO_RPM_VMIDMT_NSGFSYNDR1_ADDR                          (RPM_VMIDMT_REG_BASE      + 0x00000454)
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_RMSK                           0x1010001
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSYNDR1_ADDR, HWIO_RPM_VMIDMT_NSGFSYNDR1_RMSK)
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSYNDR1_ADDR, m)
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_MSDINDEX_BMSK                  0x1000000
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_MSDINDEX_SHFT                       0x18
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_SSDINDEX_BMSK                    0x10000
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_SSDINDEX_SHFT                       0x10
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_STREAMINDEX_BMSK                     0x1
#define HWIO_RPM_VMIDMT_NSGFSYNDR1_STREAMINDEX_SHFT                     0x0

#define HWIO_RPM_VMIDMT_NSGFSYNDR2_ADDR                          (RPM_VMIDMT_REG_BASE      + 0x00000458)
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_RMSK                          0x3f1fffff
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSYNDR2_ADDR, HWIO_RPM_VMIDMT_NSGFSYNDR2_RMSK)
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSGFSYNDR2_ADDR, m)
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_ATID_BMSK                     0x3f000000
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_ATID_SHFT                           0x18
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_AVMID_BMSK                      0x1f0000
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_AVMID_SHFT                          0x10
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_ABID_BMSK                         0xe000
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_ABID_SHFT                            0xd
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_APID_BMSK                         0x1f00
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_APID_SHFT                            0x8
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_AMID_BMSK                           0xff
#define HWIO_RPM_VMIDMT_NSGFSYNDR2_AMID_SHFT                            0x0

#define HWIO_RPM_VMIDMT_NSVMIDMTCR0_ADDR                         (RPM_VMIDMT_REG_BASE      + 0x00000490)
#define HWIO_RPM_VMIDMT_NSVMIDMTCR0_RMSK                                0x1
#define HWIO_RPM_VMIDMT_NSVMIDMTCR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_NSVMIDMTCR0_ADDR, HWIO_RPM_VMIDMT_NSVMIDMTCR0_RMSK)
#define HWIO_RPM_VMIDMT_NSVMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_NSVMIDMTCR0_ADDR, m)
#define HWIO_RPM_VMIDMT_NSVMIDMTCR0_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_NSVMIDMTCR0_ADDR,v)
#define HWIO_RPM_VMIDMT_NSVMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_NSVMIDMTCR0_ADDR,m,v,HWIO_RPM_VMIDMT_NSVMIDMTCR0_IN)
#define HWIO_RPM_VMIDMT_NSVMIDMTCR0_CLKONOFFE_BMSK                      0x1
#define HWIO_RPM_VMIDMT_NSVMIDMTCR0_CLKONOFFE_SHFT                      0x0

#define HWIO_RPM_VMIDMT_SSDR0_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000080)
#define HWIO_RPM_VMIDMT_SSDR0_RMSK                                      0x1
#define HWIO_RPM_VMIDMT_SSDR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_SSDR0_ADDR, HWIO_RPM_VMIDMT_SSDR0_RMSK)
#define HWIO_RPM_VMIDMT_SSDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_SSDR0_ADDR, m)
#define HWIO_RPM_VMIDMT_SSDR0_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_SSDR0_ADDR,v)
#define HWIO_RPM_VMIDMT_SSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_SSDR0_ADDR,m,v,HWIO_RPM_VMIDMT_SSDR0_IN)
#define HWIO_RPM_VMIDMT_SSDR0_RWE_BMSK                                  0x1
#define HWIO_RPM_VMIDMT_SSDR0_RWE_SHFT                                  0x0

#define HWIO_RPM_VMIDMT_MSDR0_ADDR                               (RPM_VMIDMT_REG_BASE      + 0x00000480)
#define HWIO_RPM_VMIDMT_MSDR0_RMSK                                      0x1
#define HWIO_RPM_VMIDMT_MSDR0_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_MSDR0_ADDR, HWIO_RPM_VMIDMT_MSDR0_RMSK)
#define HWIO_RPM_VMIDMT_MSDR0_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_MSDR0_ADDR, m)
#define HWIO_RPM_VMIDMT_MSDR0_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_MSDR0_ADDR,v)
#define HWIO_RPM_VMIDMT_MSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_MSDR0_ADDR,m,v,HWIO_RPM_VMIDMT_MSDR0_IN)
#define HWIO_RPM_VMIDMT_MSDR0_RWE_BMSK                                  0x1
#define HWIO_RPM_VMIDMT_MSDR0_RWE_SHFT                                  0x0

#define HWIO_RPM_VMIDMT_MCR_ADDR                                 (RPM_VMIDMT_REG_BASE      + 0x00000494)
#define HWIO_RPM_VMIDMT_MCR_RMSK                                        0x7
#define HWIO_RPM_VMIDMT_MCR_IN          \
        in_dword_masked(HWIO_RPM_VMIDMT_MCR_ADDR, HWIO_RPM_VMIDMT_MCR_RMSK)
#define HWIO_RPM_VMIDMT_MCR_INM(m)      \
        in_dword_masked(HWIO_RPM_VMIDMT_MCR_ADDR, m)
#define HWIO_RPM_VMIDMT_MCR_OUT(v)      \
        out_dword(HWIO_RPM_VMIDMT_MCR_ADDR,v)
#define HWIO_RPM_VMIDMT_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_MCR_ADDR,m,v,HWIO_RPM_VMIDMT_MCR_IN)
#define HWIO_RPM_VMIDMT_MCR_CLKONOFFE_BMSK                              0x4
#define HWIO_RPM_VMIDMT_MCR_CLKONOFFE_SHFT                              0x2
#define HWIO_RPM_VMIDMT_MCR_BPMSACFG_BMSK                               0x2
#define HWIO_RPM_VMIDMT_MCR_BPMSACFG_SHFT                               0x1
#define HWIO_RPM_VMIDMT_MCR_BPSMSACFG_BMSK                              0x1
#define HWIO_RPM_VMIDMT_MCR_BPSMSACFG_SHFT                              0x0

#define HWIO_RPM_VMIDMT_S2VRn_ADDR(n)                            (RPM_VMIDMT_REG_BASE      + 0x00000c00 + 0x4 * (n))
#define HWIO_RPM_VMIDMT_S2VRn_RMSK                               0x30ff7b1f
#define HWIO_RPM_VMIDMT_S2VRn_MAXn                                        0
#define HWIO_RPM_VMIDMT_S2VRn_INI(n)        \
        in_dword_masked(HWIO_RPM_VMIDMT_S2VRn_ADDR(n), HWIO_RPM_VMIDMT_S2VRn_RMSK)
#define HWIO_RPM_VMIDMT_S2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_VMIDMT_S2VRn_ADDR(n), mask)
#define HWIO_RPM_VMIDMT_S2VRn_OUTI(n,val)    \
        out_dword(HWIO_RPM_VMIDMT_S2VRn_ADDR(n),val)
#define HWIO_RPM_VMIDMT_S2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_S2VRn_ADDR(n),mask,val,HWIO_RPM_VMIDMT_S2VRn_INI(n))
#define HWIO_RPM_VMIDMT_S2VRn_TRANSIENTCFG_BMSK                  0x30000000
#define HWIO_RPM_VMIDMT_S2VRn_TRANSIENTCFG_SHFT                        0x1c
#define HWIO_RPM_VMIDMT_S2VRn_WACFG_BMSK                           0xc00000
#define HWIO_RPM_VMIDMT_S2VRn_WACFG_SHFT                               0x16
#define HWIO_RPM_VMIDMT_S2VRn_RACFG_BMSK                           0x300000
#define HWIO_RPM_VMIDMT_S2VRn_RACFG_SHFT                               0x14
#define HWIO_RPM_VMIDMT_S2VRn_NSCFG_BMSK                            0xc0000
#define HWIO_RPM_VMIDMT_S2VRn_NSCFG_SHFT                               0x12
#define HWIO_RPM_VMIDMT_S2VRn_TYPE_BMSK                             0x30000
#define HWIO_RPM_VMIDMT_S2VRn_TYPE_SHFT                                0x10
#define HWIO_RPM_VMIDMT_S2VRn_MEMATTR_BMSK                           0x7000
#define HWIO_RPM_VMIDMT_S2VRn_MEMATTR_SHFT                              0xc
#define HWIO_RPM_VMIDMT_S2VRn_MTCFG_BMSK                              0x800
#define HWIO_RPM_VMIDMT_S2VRn_MTCFG_SHFT                                0xb
#define HWIO_RPM_VMIDMT_S2VRn_SHCFG_BMSK                              0x300
#define HWIO_RPM_VMIDMT_S2VRn_SHCFG_SHFT                                0x8
#define HWIO_RPM_VMIDMT_S2VRn_VMID_BMSK                                0x1f
#define HWIO_RPM_VMIDMT_S2VRn_VMID_SHFT                                 0x0

#define HWIO_RPM_VMIDMT_AS2VRn_ADDR(n)                           (RPM_VMIDMT_REG_BASE      + 0x00000e00 + 0x4 * (n))
#define HWIO_RPM_VMIDMT_AS2VRn_RMSK                              0x70000013
#define HWIO_RPM_VMIDMT_AS2VRn_MAXn                                       0
#define HWIO_RPM_VMIDMT_AS2VRn_INI(n)        \
        in_dword_masked(HWIO_RPM_VMIDMT_AS2VRn_ADDR(n), HWIO_RPM_VMIDMT_AS2VRn_RMSK)
#define HWIO_RPM_VMIDMT_AS2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_RPM_VMIDMT_AS2VRn_ADDR(n), mask)
#define HWIO_RPM_VMIDMT_AS2VRn_OUTI(n,val)    \
        out_dword(HWIO_RPM_VMIDMT_AS2VRn_ADDR(n),val)
#define HWIO_RPM_VMIDMT_AS2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_RPM_VMIDMT_AS2VRn_ADDR(n),mask,val,HWIO_RPM_VMIDMT_AS2VRn_INI(n))
#define HWIO_RPM_VMIDMT_AS2VRn_RCNSH_BMSK                        0x40000000
#define HWIO_RPM_VMIDMT_AS2VRn_RCNSH_SHFT                              0x1e
#define HWIO_RPM_VMIDMT_AS2VRn_RCISH_BMSK                        0x20000000
#define HWIO_RPM_VMIDMT_AS2VRn_RCISH_SHFT                              0x1d
#define HWIO_RPM_VMIDMT_AS2VRn_RCOSH_BMSK                        0x10000000
#define HWIO_RPM_VMIDMT_AS2VRn_RCOSH_SHFT                              0x1c
#define HWIO_RPM_VMIDMT_AS2VRn_REQPRIORITYCFG_BMSK                     0x10
#define HWIO_RPM_VMIDMT_AS2VRn_REQPRIORITYCFG_SHFT                      0x4
#define HWIO_RPM_VMIDMT_AS2VRn_REQPRIORITY_BMSK                         0x3
#define HWIO_RPM_VMIDMT_AS2VRn_REQPRIORITY_SHFT                         0x0


#endif /* __RPM_HWIO_H__ */
