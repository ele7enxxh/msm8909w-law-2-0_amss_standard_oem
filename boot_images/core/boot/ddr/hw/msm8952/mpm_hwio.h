#ifndef __MPM_HWIO_H__
#define __MPM_HWIO_H__
/*
===========================================================================
*/
/**
  @file mpm_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MPM2_APU
    MPM2_G_CTRL_CNTR
    MPM2_G_RD_CNTR
    MPM2_MPM
    MPM2_PSHOLD
    MPM2_QTIMR_AC
    MPM2_QTIMR_V1
    MPM2_SLP_CNTR
    MPM2_TSENS
    MPM2_TSENS_TM
    MPM2_TSYNC
    MPM2_WDOG

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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/msm8952/mpm_hwio.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MPM2_APU
 *--------------------------------------------------------------------------*/

#define MPM2_APU_REG_BASE                                         (MPM2_MPM_BASE      + 0x00007000)
#define MPM2_APU_REG_BASE_PHYS                                    (MPM2_MPM_BASE_PHYS + 0x00007000)
#define MPM2_APU_REG_BASE_OFFS                                    0x00007000

#define HWIO_MPM2_XPU_SCR_ADDR                                    (MPM2_APU_REG_BASE      + 0x00000000)
#define HWIO_MPM2_XPU_SCR_PHYS                                    (MPM2_APU_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_XPU_SCR_OFFS                                    (MPM2_APU_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_XPU_SCR_RMSK                                         0x77f
#define HWIO_MPM2_XPU_SCR_IN          \
        in_dword_masked(HWIO_MPM2_XPU_SCR_ADDR, HWIO_MPM2_XPU_SCR_RMSK)
#define HWIO_MPM2_XPU_SCR_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_SCR_ADDR, m)
#define HWIO_MPM2_XPU_SCR_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_SCR_ADDR,v)
#define HWIO_MPM2_XPU_SCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_SCR_ADDR,m,v,HWIO_MPM2_XPU_SCR_IN)
#define HWIO_MPM2_XPU_SCR_SCLEIE_BMSK                                  0x400
#define HWIO_MPM2_XPU_SCR_SCLEIE_SHFT                                    0xa
#define HWIO_MPM2_XPU_SCR_SCFGEIE_BMSK                                 0x200
#define HWIO_MPM2_XPU_SCR_SCFGEIE_SHFT                                   0x9
#define HWIO_MPM2_XPU_SCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_MPM2_XPU_SCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_MPM2_XPU_SCR_NSRGCLEE_BMSK                                 0x40
#define HWIO_MPM2_XPU_SCR_NSRGCLEE_SHFT                                  0x6
#define HWIO_MPM2_XPU_SCR_NSCFGE_BMSK                                   0x20
#define HWIO_MPM2_XPU_SCR_NSCFGE_SHFT                                    0x5
#define HWIO_MPM2_XPU_SCR_SDCDEE_BMSK                                   0x10
#define HWIO_MPM2_XPU_SCR_SDCDEE_SHFT                                    0x4
#define HWIO_MPM2_XPU_SCR_SEIE_BMSK                                      0x8
#define HWIO_MPM2_XPU_SCR_SEIE_SHFT                                      0x3
#define HWIO_MPM2_XPU_SCR_SCLERE_BMSK                                    0x4
#define HWIO_MPM2_XPU_SCR_SCLERE_SHFT                                    0x2
#define HWIO_MPM2_XPU_SCR_SCFGERE_BMSK                                   0x2
#define HWIO_MPM2_XPU_SCR_SCFGERE_SHFT                                   0x1
#define HWIO_MPM2_XPU_SCR_XPUNSE_BMSK                                    0x1
#define HWIO_MPM2_XPU_SCR_XPUNSE_SHFT                                    0x0

#define HWIO_MPM2_XPU_SWDR_ADDR                                   (MPM2_APU_REG_BASE      + 0x00000004)
#define HWIO_MPM2_XPU_SWDR_PHYS                                   (MPM2_APU_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_XPU_SWDR_OFFS                                   (MPM2_APU_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_XPU_SWDR_RMSK                                          0x1
#define HWIO_MPM2_XPU_SWDR_IN          \
        in_dword_masked(HWIO_MPM2_XPU_SWDR_ADDR, HWIO_MPM2_XPU_SWDR_RMSK)
#define HWIO_MPM2_XPU_SWDR_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_SWDR_ADDR, m)
#define HWIO_MPM2_XPU_SWDR_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_SWDR_ADDR,v)
#define HWIO_MPM2_XPU_SWDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_SWDR_ADDR,m,v,HWIO_MPM2_XPU_SWDR_IN)
#define HWIO_MPM2_XPU_SWDR_SCFGWD_BMSK                                   0x1
#define HWIO_MPM2_XPU_SWDR_SCFGWD_SHFT                                   0x0

#define HWIO_MPM2_XPU_SEAR0_ADDR                                  (MPM2_APU_REG_BASE      + 0x00000040)
#define HWIO_MPM2_XPU_SEAR0_PHYS                                  (MPM2_APU_REG_BASE_PHYS + 0x00000040)
#define HWIO_MPM2_XPU_SEAR0_OFFS                                  (MPM2_APU_REG_BASE_OFFS + 0x00000040)
#define HWIO_MPM2_XPU_SEAR0_RMSK                                  0xffffffff
#define HWIO_MPM2_XPU_SEAR0_IN          \
        in_dword_masked(HWIO_MPM2_XPU_SEAR0_ADDR, HWIO_MPM2_XPU_SEAR0_RMSK)
#define HWIO_MPM2_XPU_SEAR0_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_SEAR0_ADDR, m)
#define HWIO_MPM2_XPU_SEAR0_PA_BMSK                               0xffffffff
#define HWIO_MPM2_XPU_SEAR0_PA_SHFT                                      0x0

#define HWIO_MPM2_XPU_SESR_ADDR                                   (MPM2_APU_REG_BASE      + 0x00000048)
#define HWIO_MPM2_XPU_SESR_PHYS                                   (MPM2_APU_REG_BASE_PHYS + 0x00000048)
#define HWIO_MPM2_XPU_SESR_OFFS                                   (MPM2_APU_REG_BASE_OFFS + 0x00000048)
#define HWIO_MPM2_XPU_SESR_RMSK                                   0x8000000f
#define HWIO_MPM2_XPU_SESR_IN          \
        in_dword_masked(HWIO_MPM2_XPU_SESR_ADDR, HWIO_MPM2_XPU_SESR_RMSK)
#define HWIO_MPM2_XPU_SESR_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_SESR_ADDR, m)
#define HWIO_MPM2_XPU_SESR_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_SESR_ADDR,v)
#define HWIO_MPM2_XPU_SESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_SESR_ADDR,m,v,HWIO_MPM2_XPU_SESR_IN)
#define HWIO_MPM2_XPU_SESR_MULTI_BMSK                             0x80000000
#define HWIO_MPM2_XPU_SESR_MULTI_SHFT                                   0x1f
#define HWIO_MPM2_XPU_SESR_CLMULTI_BMSK                                  0x8
#define HWIO_MPM2_XPU_SESR_CLMULTI_SHFT                                  0x3
#define HWIO_MPM2_XPU_SESR_CFGMULTI_BMSK                                 0x4
#define HWIO_MPM2_XPU_SESR_CFGMULTI_SHFT                                 0x2
#define HWIO_MPM2_XPU_SESR_CLIENT_BMSK                                   0x2
#define HWIO_MPM2_XPU_SESR_CLIENT_SHFT                                   0x1
#define HWIO_MPM2_XPU_SESR_CFG_BMSK                                      0x1
#define HWIO_MPM2_XPU_SESR_CFG_SHFT                                      0x0

#define HWIO_MPM2_XPU_SESRRESTORE_ADDR                            (MPM2_APU_REG_BASE      + 0x0000004c)
#define HWIO_MPM2_XPU_SESRRESTORE_PHYS                            (MPM2_APU_REG_BASE_PHYS + 0x0000004c)
#define HWIO_MPM2_XPU_SESRRESTORE_OFFS                            (MPM2_APU_REG_BASE_OFFS + 0x0000004c)
#define HWIO_MPM2_XPU_SESRRESTORE_RMSK                            0x8000000f
#define HWIO_MPM2_XPU_SESRRESTORE_IN          \
        in_dword_masked(HWIO_MPM2_XPU_SESRRESTORE_ADDR, HWIO_MPM2_XPU_SESRRESTORE_RMSK)
#define HWIO_MPM2_XPU_SESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_SESRRESTORE_ADDR, m)
#define HWIO_MPM2_XPU_SESRRESTORE_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_SESRRESTORE_ADDR,v)
#define HWIO_MPM2_XPU_SESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_SESRRESTORE_ADDR,m,v,HWIO_MPM2_XPU_SESRRESTORE_IN)
#define HWIO_MPM2_XPU_SESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_MPM2_XPU_SESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_MPM2_XPU_SESRRESTORE_CLMULTI_BMSK                           0x8
#define HWIO_MPM2_XPU_SESRRESTORE_CLMULTI_SHFT                           0x3
#define HWIO_MPM2_XPU_SESRRESTORE_CFGMULTI_BMSK                          0x4
#define HWIO_MPM2_XPU_SESRRESTORE_CFGMULTI_SHFT                          0x2
#define HWIO_MPM2_XPU_SESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_MPM2_XPU_SESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_MPM2_XPU_SESRRESTORE_CFG_BMSK                               0x1
#define HWIO_MPM2_XPU_SESRRESTORE_CFG_SHFT                               0x0

#define HWIO_MPM2_XPU_SESYNR0_ADDR                                (MPM2_APU_REG_BASE      + 0x00000050)
#define HWIO_MPM2_XPU_SESYNR0_PHYS                                (MPM2_APU_REG_BASE_PHYS + 0x00000050)
#define HWIO_MPM2_XPU_SESYNR0_OFFS                                (MPM2_APU_REG_BASE_OFFS + 0x00000050)
#define HWIO_MPM2_XPU_SESYNR0_RMSK                                0xffffffff
#define HWIO_MPM2_XPU_SESYNR0_IN          \
        in_dword_masked(HWIO_MPM2_XPU_SESYNR0_ADDR, HWIO_MPM2_XPU_SESYNR0_RMSK)
#define HWIO_MPM2_XPU_SESYNR0_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_SESYNR0_ADDR, m)
#define HWIO_MPM2_XPU_SESYNR0_ATID_BMSK                           0xff000000
#define HWIO_MPM2_XPU_SESYNR0_ATID_SHFT                                 0x18
#define HWIO_MPM2_XPU_SESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_MPM2_XPU_SESYNR0_AVMID_SHFT                                0x10
#define HWIO_MPM2_XPU_SESYNR0_ABID_BMSK                               0xe000
#define HWIO_MPM2_XPU_SESYNR0_ABID_SHFT                                  0xd
#define HWIO_MPM2_XPU_SESYNR0_APID_BMSK                               0x1f00
#define HWIO_MPM2_XPU_SESYNR0_APID_SHFT                                  0x8
#define HWIO_MPM2_XPU_SESYNR0_AMID_BMSK                                 0xff
#define HWIO_MPM2_XPU_SESYNR0_AMID_SHFT                                  0x0

#define HWIO_MPM2_XPU_SESYNR1_ADDR                                (MPM2_APU_REG_BASE      + 0x00000054)
#define HWIO_MPM2_XPU_SESYNR1_PHYS                                (MPM2_APU_REG_BASE_PHYS + 0x00000054)
#define HWIO_MPM2_XPU_SESYNR1_OFFS                                (MPM2_APU_REG_BASE_OFFS + 0x00000054)
#define HWIO_MPM2_XPU_SESYNR1_RMSK                                0xffffffff
#define HWIO_MPM2_XPU_SESYNR1_IN          \
        in_dword_masked(HWIO_MPM2_XPU_SESYNR1_ADDR, HWIO_MPM2_XPU_SESYNR1_RMSK)
#define HWIO_MPM2_XPU_SESYNR1_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_SESYNR1_ADDR, m)
#define HWIO_MPM2_XPU_SESYNR1_DCD_BMSK                            0x80000000
#define HWIO_MPM2_XPU_SESYNR1_DCD_SHFT                                  0x1f
#define HWIO_MPM2_XPU_SESYNR1_AC_BMSK                             0x40000000
#define HWIO_MPM2_XPU_SESYNR1_AC_SHFT                                   0x1e
#define HWIO_MPM2_XPU_SESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_MPM2_XPU_SESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_MPM2_XPU_SESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_MPM2_XPU_SESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_MPM2_XPU_SESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_MPM2_XPU_SESYNR1_ABURST_SHFT                               0x1b
#define HWIO_MPM2_XPU_SESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_MPM2_XPU_SESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_MPM2_XPU_SESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_MPM2_XPU_SESYNR1_AWRITE_SHFT                               0x19
#define HWIO_MPM2_XPU_SESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_MPM2_XPU_SESYNR1_AFULL_SHFT                                0x18
#define HWIO_MPM2_XPU_SESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_MPM2_XPU_SESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_MPM2_XPU_SESYNR1_AOOO_BMSK                             0x400000
#define HWIO_MPM2_XPU_SESYNR1_AOOO_SHFT                                 0x16
#define HWIO_MPM2_XPU_SESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_MPM2_XPU_SESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_MPM2_XPU_SESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_MPM2_XPU_SESYNR1_ASIZE_SHFT                                0x10
#define HWIO_MPM2_XPU_SESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_MPM2_XPU_SESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_MPM2_XPU_SESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_MPM2_XPU_SESYNR1_ALEN_SHFT                                  0x8
#define HWIO_MPM2_XPU_SESYNR1_AINST_BMSK                                0x80
#define HWIO_MPM2_XPU_SESYNR1_AINST_SHFT                                 0x7
#define HWIO_MPM2_XPU_SESYNR1_APROTNS_BMSK                              0x40
#define HWIO_MPM2_XPU_SESYNR1_APROTNS_SHFT                               0x6
#define HWIO_MPM2_XPU_SESYNR1_APRIV_BMSK                                0x20
#define HWIO_MPM2_XPU_SESYNR1_APRIV_SHFT                                 0x5
#define HWIO_MPM2_XPU_SESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_MPM2_XPU_SESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_MPM2_XPU_SESYNR1_ASHARED_BMSK                               0x8
#define HWIO_MPM2_XPU_SESYNR1_ASHARED_SHFT                               0x3
#define HWIO_MPM2_XPU_SESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_MPM2_XPU_SESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_MPM2_XPU_SESYNR2_ADDR                                (MPM2_APU_REG_BASE      + 0x00000058)
#define HWIO_MPM2_XPU_SESYNR2_PHYS                                (MPM2_APU_REG_BASE_PHYS + 0x00000058)
#define HWIO_MPM2_XPU_SESYNR2_OFFS                                (MPM2_APU_REG_BASE_OFFS + 0x00000058)
#define HWIO_MPM2_XPU_SESYNR2_RMSK                                       0x7
#define HWIO_MPM2_XPU_SESYNR2_IN          \
        in_dword_masked(HWIO_MPM2_XPU_SESYNR2_ADDR, HWIO_MPM2_XPU_SESYNR2_RMSK)
#define HWIO_MPM2_XPU_SESYNR2_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_SESYNR2_ADDR, m)
#define HWIO_MPM2_XPU_SESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_MPM2_XPU_SESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_MPM2_XPU_SESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_MPM2_XPU_SESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_MPM2_XPU_SESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_MPM2_XPU_SESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_MPM2_XPU_MCR_ADDR                                    (MPM2_APU_REG_BASE      + 0x00000100)
#define HWIO_MPM2_XPU_MCR_PHYS                                    (MPM2_APU_REG_BASE_PHYS + 0x00000100)
#define HWIO_MPM2_XPU_MCR_OFFS                                    (MPM2_APU_REG_BASE_OFFS + 0x00000100)
#define HWIO_MPM2_XPU_MCR_RMSK                                         0x71f
#define HWIO_MPM2_XPU_MCR_IN          \
        in_dword_masked(HWIO_MPM2_XPU_MCR_ADDR, HWIO_MPM2_XPU_MCR_RMSK)
#define HWIO_MPM2_XPU_MCR_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_MCR_ADDR, m)
#define HWIO_MPM2_XPU_MCR_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_MCR_ADDR,v)
#define HWIO_MPM2_XPU_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_MCR_ADDR,m,v,HWIO_MPM2_XPU_MCR_IN)
#define HWIO_MPM2_XPU_MCR_CLEIE_BMSK                                   0x400
#define HWIO_MPM2_XPU_MCR_CLEIE_SHFT                                     0xa
#define HWIO_MPM2_XPU_MCR_CFGEIE_BMSK                                  0x200
#define HWIO_MPM2_XPU_MCR_CFGEIE_SHFT                                    0x9
#define HWIO_MPM2_XPU_MCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_MPM2_XPU_MCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_MPM2_XPU_MCR_DCDEE_BMSK                                    0x10
#define HWIO_MPM2_XPU_MCR_DCDEE_SHFT                                     0x4
#define HWIO_MPM2_XPU_MCR_EIE_BMSK                                       0x8
#define HWIO_MPM2_XPU_MCR_EIE_SHFT                                       0x3
#define HWIO_MPM2_XPU_MCR_CLERE_BMSK                                     0x4
#define HWIO_MPM2_XPU_MCR_CLERE_SHFT                                     0x2
#define HWIO_MPM2_XPU_MCR_CFGERE_BMSK                                    0x2
#define HWIO_MPM2_XPU_MCR_CFGERE_SHFT                                    0x1
#define HWIO_MPM2_XPU_MCR_XPUMSAE_BMSK                                   0x1
#define HWIO_MPM2_XPU_MCR_XPUMSAE_SHFT                                   0x0

#define HWIO_MPM2_XPU_MEAR0_ADDR                                  (MPM2_APU_REG_BASE      + 0x00000140)
#define HWIO_MPM2_XPU_MEAR0_PHYS                                  (MPM2_APU_REG_BASE_PHYS + 0x00000140)
#define HWIO_MPM2_XPU_MEAR0_OFFS                                  (MPM2_APU_REG_BASE_OFFS + 0x00000140)
#define HWIO_MPM2_XPU_MEAR0_RMSK                                  0xffffffff
#define HWIO_MPM2_XPU_MEAR0_IN          \
        in_dword_masked(HWIO_MPM2_XPU_MEAR0_ADDR, HWIO_MPM2_XPU_MEAR0_RMSK)
#define HWIO_MPM2_XPU_MEAR0_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_MEAR0_ADDR, m)
#define HWIO_MPM2_XPU_MEAR0_PA_BMSK                               0xffffffff
#define HWIO_MPM2_XPU_MEAR0_PA_SHFT                                      0x0

#define HWIO_MPM2_XPU_MESR_ADDR                                   (MPM2_APU_REG_BASE      + 0x00000148)
#define HWIO_MPM2_XPU_MESR_PHYS                                   (MPM2_APU_REG_BASE_PHYS + 0x00000148)
#define HWIO_MPM2_XPU_MESR_OFFS                                   (MPM2_APU_REG_BASE_OFFS + 0x00000148)
#define HWIO_MPM2_XPU_MESR_RMSK                                   0x8000000f
#define HWIO_MPM2_XPU_MESR_IN          \
        in_dword_masked(HWIO_MPM2_XPU_MESR_ADDR, HWIO_MPM2_XPU_MESR_RMSK)
#define HWIO_MPM2_XPU_MESR_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_MESR_ADDR, m)
#define HWIO_MPM2_XPU_MESR_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_MESR_ADDR,v)
#define HWIO_MPM2_XPU_MESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_MESR_ADDR,m,v,HWIO_MPM2_XPU_MESR_IN)
#define HWIO_MPM2_XPU_MESR_MULTI_BMSK                             0x80000000
#define HWIO_MPM2_XPU_MESR_MULTI_SHFT                                   0x1f
#define HWIO_MPM2_XPU_MESR_CLMULTI_BMSK                                  0x8
#define HWIO_MPM2_XPU_MESR_CLMULTI_SHFT                                  0x3
#define HWIO_MPM2_XPU_MESR_CFGMULTI_BMSK                                 0x4
#define HWIO_MPM2_XPU_MESR_CFGMULTI_SHFT                                 0x2
#define HWIO_MPM2_XPU_MESR_CLIENT_BMSK                                   0x2
#define HWIO_MPM2_XPU_MESR_CLIENT_SHFT                                   0x1
#define HWIO_MPM2_XPU_MESR_CFG_BMSK                                      0x1
#define HWIO_MPM2_XPU_MESR_CFG_SHFT                                      0x0

#define HWIO_MPM2_XPU_MESRRESTORE_ADDR                            (MPM2_APU_REG_BASE      + 0x0000014c)
#define HWIO_MPM2_XPU_MESRRESTORE_PHYS                            (MPM2_APU_REG_BASE_PHYS + 0x0000014c)
#define HWIO_MPM2_XPU_MESRRESTORE_OFFS                            (MPM2_APU_REG_BASE_OFFS + 0x0000014c)
#define HWIO_MPM2_XPU_MESRRESTORE_RMSK                            0x8000000f
#define HWIO_MPM2_XPU_MESRRESTORE_IN          \
        in_dword_masked(HWIO_MPM2_XPU_MESRRESTORE_ADDR, HWIO_MPM2_XPU_MESRRESTORE_RMSK)
#define HWIO_MPM2_XPU_MESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_MESRRESTORE_ADDR, m)
#define HWIO_MPM2_XPU_MESRRESTORE_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_MESRRESTORE_ADDR,v)
#define HWIO_MPM2_XPU_MESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_MESRRESTORE_ADDR,m,v,HWIO_MPM2_XPU_MESRRESTORE_IN)
#define HWIO_MPM2_XPU_MESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_MPM2_XPU_MESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_MPM2_XPU_MESRRESTORE_CLMULTI_BMSK                           0x8
#define HWIO_MPM2_XPU_MESRRESTORE_CLMULTI_SHFT                           0x3
#define HWIO_MPM2_XPU_MESRRESTORE_CFGMULTI_BMSK                          0x4
#define HWIO_MPM2_XPU_MESRRESTORE_CFGMULTI_SHFT                          0x2
#define HWIO_MPM2_XPU_MESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_MPM2_XPU_MESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_MPM2_XPU_MESRRESTORE_CFG_BMSK                               0x1
#define HWIO_MPM2_XPU_MESRRESTORE_CFG_SHFT                               0x0

#define HWIO_MPM2_XPU_MESYNR0_ADDR                                (MPM2_APU_REG_BASE      + 0x00000150)
#define HWIO_MPM2_XPU_MESYNR0_PHYS                                (MPM2_APU_REG_BASE_PHYS + 0x00000150)
#define HWIO_MPM2_XPU_MESYNR0_OFFS                                (MPM2_APU_REG_BASE_OFFS + 0x00000150)
#define HWIO_MPM2_XPU_MESYNR0_RMSK                                0xffffffff
#define HWIO_MPM2_XPU_MESYNR0_IN          \
        in_dword_masked(HWIO_MPM2_XPU_MESYNR0_ADDR, HWIO_MPM2_XPU_MESYNR0_RMSK)
#define HWIO_MPM2_XPU_MESYNR0_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_MESYNR0_ADDR, m)
#define HWIO_MPM2_XPU_MESYNR0_ATID_BMSK                           0xff000000
#define HWIO_MPM2_XPU_MESYNR0_ATID_SHFT                                 0x18
#define HWIO_MPM2_XPU_MESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_MPM2_XPU_MESYNR0_AVMID_SHFT                                0x10
#define HWIO_MPM2_XPU_MESYNR0_ABID_BMSK                               0xe000
#define HWIO_MPM2_XPU_MESYNR0_ABID_SHFT                                  0xd
#define HWIO_MPM2_XPU_MESYNR0_APID_BMSK                               0x1f00
#define HWIO_MPM2_XPU_MESYNR0_APID_SHFT                                  0x8
#define HWIO_MPM2_XPU_MESYNR0_AMID_BMSK                                 0xff
#define HWIO_MPM2_XPU_MESYNR0_AMID_SHFT                                  0x0

#define HWIO_MPM2_XPU_MESYNR1_ADDR                                (MPM2_APU_REG_BASE      + 0x00000154)
#define HWIO_MPM2_XPU_MESYNR1_PHYS                                (MPM2_APU_REG_BASE_PHYS + 0x00000154)
#define HWIO_MPM2_XPU_MESYNR1_OFFS                                (MPM2_APU_REG_BASE_OFFS + 0x00000154)
#define HWIO_MPM2_XPU_MESYNR1_RMSK                                0xffffffff
#define HWIO_MPM2_XPU_MESYNR1_IN          \
        in_dword_masked(HWIO_MPM2_XPU_MESYNR1_ADDR, HWIO_MPM2_XPU_MESYNR1_RMSK)
#define HWIO_MPM2_XPU_MESYNR1_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_MESYNR1_ADDR, m)
#define HWIO_MPM2_XPU_MESYNR1_DCD_BMSK                            0x80000000
#define HWIO_MPM2_XPU_MESYNR1_DCD_SHFT                                  0x1f
#define HWIO_MPM2_XPU_MESYNR1_AC_BMSK                             0x40000000
#define HWIO_MPM2_XPU_MESYNR1_AC_SHFT                                   0x1e
#define HWIO_MPM2_XPU_MESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_MPM2_XPU_MESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_MPM2_XPU_MESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_MPM2_XPU_MESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_MPM2_XPU_MESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_MPM2_XPU_MESYNR1_ABURST_SHFT                               0x1b
#define HWIO_MPM2_XPU_MESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_MPM2_XPU_MESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_MPM2_XPU_MESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_MPM2_XPU_MESYNR1_AWRITE_SHFT                               0x19
#define HWIO_MPM2_XPU_MESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_MPM2_XPU_MESYNR1_AFULL_SHFT                                0x18
#define HWIO_MPM2_XPU_MESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_MPM2_XPU_MESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_MPM2_XPU_MESYNR1_AOOO_BMSK                             0x400000
#define HWIO_MPM2_XPU_MESYNR1_AOOO_SHFT                                 0x16
#define HWIO_MPM2_XPU_MESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_MPM2_XPU_MESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_MPM2_XPU_MESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_MPM2_XPU_MESYNR1_ASIZE_SHFT                                0x10
#define HWIO_MPM2_XPU_MESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_MPM2_XPU_MESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_MPM2_XPU_MESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_MPM2_XPU_MESYNR1_ALEN_SHFT                                  0x8
#define HWIO_MPM2_XPU_MESYNR1_AINST_BMSK                                0x80
#define HWIO_MPM2_XPU_MESYNR1_AINST_SHFT                                 0x7
#define HWIO_MPM2_XPU_MESYNR1_APROTNS_BMSK                              0x40
#define HWIO_MPM2_XPU_MESYNR1_APROTNS_SHFT                               0x6
#define HWIO_MPM2_XPU_MESYNR1_APRIV_BMSK                                0x20
#define HWIO_MPM2_XPU_MESYNR1_APRIV_SHFT                                 0x5
#define HWIO_MPM2_XPU_MESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_MPM2_XPU_MESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_MPM2_XPU_MESYNR1_ASHARED_BMSK                               0x8
#define HWIO_MPM2_XPU_MESYNR1_ASHARED_SHFT                               0x3
#define HWIO_MPM2_XPU_MESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_MPM2_XPU_MESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_MPM2_XPU_MESYNR2_ADDR                                (MPM2_APU_REG_BASE      + 0x00000158)
#define HWIO_MPM2_XPU_MESYNR2_PHYS                                (MPM2_APU_REG_BASE_PHYS + 0x00000158)
#define HWIO_MPM2_XPU_MESYNR2_OFFS                                (MPM2_APU_REG_BASE_OFFS + 0x00000158)
#define HWIO_MPM2_XPU_MESYNR2_RMSK                                       0x7
#define HWIO_MPM2_XPU_MESYNR2_IN          \
        in_dword_masked(HWIO_MPM2_XPU_MESYNR2_ADDR, HWIO_MPM2_XPU_MESYNR2_RMSK)
#define HWIO_MPM2_XPU_MESYNR2_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_MESYNR2_ADDR, m)
#define HWIO_MPM2_XPU_MESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_MPM2_XPU_MESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_MPM2_XPU_MESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_MPM2_XPU_MESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_MPM2_XPU_MESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_MPM2_XPU_MESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_MPM2_XPU_CR_ADDR                                     (MPM2_APU_REG_BASE      + 0x00000080)
#define HWIO_MPM2_XPU_CR_PHYS                                     (MPM2_APU_REG_BASE_PHYS + 0x00000080)
#define HWIO_MPM2_XPU_CR_OFFS                                     (MPM2_APU_REG_BASE_OFFS + 0x00000080)
#define HWIO_MPM2_XPU_CR_RMSK                                        0x1071f
#define HWIO_MPM2_XPU_CR_IN          \
        in_dword_masked(HWIO_MPM2_XPU_CR_ADDR, HWIO_MPM2_XPU_CR_RMSK)
#define HWIO_MPM2_XPU_CR_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_CR_ADDR, m)
#define HWIO_MPM2_XPU_CR_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_CR_ADDR,v)
#define HWIO_MPM2_XPU_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_CR_ADDR,m,v,HWIO_MPM2_XPU_CR_IN)
#define HWIO_MPM2_XPU_CR_MSAE_BMSK                                   0x10000
#define HWIO_MPM2_XPU_CR_MSAE_SHFT                                      0x10
#define HWIO_MPM2_XPU_CR_CLEIE_BMSK                                    0x400
#define HWIO_MPM2_XPU_CR_CLEIE_SHFT                                      0xa
#define HWIO_MPM2_XPU_CR_CFGEIE_BMSK                                   0x200
#define HWIO_MPM2_XPU_CR_CFGEIE_SHFT                                     0x9
#define HWIO_MPM2_XPU_CR_DYNAMIC_CLK_EN_BMSK                           0x100
#define HWIO_MPM2_XPU_CR_DYNAMIC_CLK_EN_SHFT                             0x8
#define HWIO_MPM2_XPU_CR_DCDEE_BMSK                                     0x10
#define HWIO_MPM2_XPU_CR_DCDEE_SHFT                                      0x4
#define HWIO_MPM2_XPU_CR_EIE_BMSK                                        0x8
#define HWIO_MPM2_XPU_CR_EIE_SHFT                                        0x3
#define HWIO_MPM2_XPU_CR_CLERE_BMSK                                      0x4
#define HWIO_MPM2_XPU_CR_CLERE_SHFT                                      0x2
#define HWIO_MPM2_XPU_CR_CFGERE_BMSK                                     0x2
#define HWIO_MPM2_XPU_CR_CFGERE_SHFT                                     0x1
#define HWIO_MPM2_XPU_CR_XPUVMIDE_BMSK                                   0x1
#define HWIO_MPM2_XPU_CR_XPUVMIDE_SHFT                                   0x0

#define HWIO_MPM2_XPU_RPU_ACRn_ADDR(n)                            (MPM2_APU_REG_BASE      + 0x000000a0 + 0x4 * (n))
#define HWIO_MPM2_XPU_RPU_ACRn_PHYS(n)                            (MPM2_APU_REG_BASE_PHYS + 0x000000a0 + 0x4 * (n))
#define HWIO_MPM2_XPU_RPU_ACRn_OFFS(n)                            (MPM2_APU_REG_BASE_OFFS + 0x000000a0 + 0x4 * (n))
#define HWIO_MPM2_XPU_RPU_ACRn_RMSK                               0xffffffff
#define HWIO_MPM2_XPU_RPU_ACRn_MAXn                                        0
#define HWIO_MPM2_XPU_RPU_ACRn_INI(n)        \
        in_dword_masked(HWIO_MPM2_XPU_RPU_ACRn_ADDR(n), HWIO_MPM2_XPU_RPU_ACRn_RMSK)
#define HWIO_MPM2_XPU_RPU_ACRn_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_XPU_RPU_ACRn_ADDR(n), mask)
#define HWIO_MPM2_XPU_RPU_ACRn_OUTI(n,val)    \
        out_dword(HWIO_MPM2_XPU_RPU_ACRn_ADDR(n),val)
#define HWIO_MPM2_XPU_RPU_ACRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_XPU_RPU_ACRn_ADDR(n),mask,val,HWIO_MPM2_XPU_RPU_ACRn_INI(n))
#define HWIO_MPM2_XPU_RPU_ACRn_RWE_BMSK                           0xffffffff
#define HWIO_MPM2_XPU_RPU_ACRn_RWE_SHFT                                  0x0

#define HWIO_MPM2_XPU_EAR0_ADDR                                   (MPM2_APU_REG_BASE      + 0x000000c0)
#define HWIO_MPM2_XPU_EAR0_PHYS                                   (MPM2_APU_REG_BASE_PHYS + 0x000000c0)
#define HWIO_MPM2_XPU_EAR0_OFFS                                   (MPM2_APU_REG_BASE_OFFS + 0x000000c0)
#define HWIO_MPM2_XPU_EAR0_RMSK                                   0xffffffff
#define HWIO_MPM2_XPU_EAR0_IN          \
        in_dword_masked(HWIO_MPM2_XPU_EAR0_ADDR, HWIO_MPM2_XPU_EAR0_RMSK)
#define HWIO_MPM2_XPU_EAR0_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_EAR0_ADDR, m)
#define HWIO_MPM2_XPU_EAR0_PA_BMSK                                0xffffffff
#define HWIO_MPM2_XPU_EAR0_PA_SHFT                                       0x0

#define HWIO_MPM2_XPU_ESR_ADDR                                    (MPM2_APU_REG_BASE      + 0x000000c8)
#define HWIO_MPM2_XPU_ESR_PHYS                                    (MPM2_APU_REG_BASE_PHYS + 0x000000c8)
#define HWIO_MPM2_XPU_ESR_OFFS                                    (MPM2_APU_REG_BASE_OFFS + 0x000000c8)
#define HWIO_MPM2_XPU_ESR_RMSK                                    0x8000000f
#define HWIO_MPM2_XPU_ESR_IN          \
        in_dword_masked(HWIO_MPM2_XPU_ESR_ADDR, HWIO_MPM2_XPU_ESR_RMSK)
#define HWIO_MPM2_XPU_ESR_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_ESR_ADDR, m)
#define HWIO_MPM2_XPU_ESR_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_ESR_ADDR,v)
#define HWIO_MPM2_XPU_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_ESR_ADDR,m,v,HWIO_MPM2_XPU_ESR_IN)
#define HWIO_MPM2_XPU_ESR_MULTI_BMSK                              0x80000000
#define HWIO_MPM2_XPU_ESR_MULTI_SHFT                                    0x1f
#define HWIO_MPM2_XPU_ESR_CLMULTI_BMSK                                   0x8
#define HWIO_MPM2_XPU_ESR_CLMULTI_SHFT                                   0x3
#define HWIO_MPM2_XPU_ESR_CFGMULTI_BMSK                                  0x4
#define HWIO_MPM2_XPU_ESR_CFGMULTI_SHFT                                  0x2
#define HWIO_MPM2_XPU_ESR_CLIENT_BMSK                                    0x2
#define HWIO_MPM2_XPU_ESR_CLIENT_SHFT                                    0x1
#define HWIO_MPM2_XPU_ESR_CFG_BMSK                                       0x1
#define HWIO_MPM2_XPU_ESR_CFG_SHFT                                       0x0

#define HWIO_MPM2_XPU_ESRRESTORE_ADDR                             (MPM2_APU_REG_BASE      + 0x000000cc)
#define HWIO_MPM2_XPU_ESRRESTORE_PHYS                             (MPM2_APU_REG_BASE_PHYS + 0x000000cc)
#define HWIO_MPM2_XPU_ESRRESTORE_OFFS                             (MPM2_APU_REG_BASE_OFFS + 0x000000cc)
#define HWIO_MPM2_XPU_ESRRESTORE_RMSK                             0x8000000f
#define HWIO_MPM2_XPU_ESRRESTORE_IN          \
        in_dword_masked(HWIO_MPM2_XPU_ESRRESTORE_ADDR, HWIO_MPM2_XPU_ESRRESTORE_RMSK)
#define HWIO_MPM2_XPU_ESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_ESRRESTORE_ADDR, m)
#define HWIO_MPM2_XPU_ESRRESTORE_OUT(v)      \
        out_dword(HWIO_MPM2_XPU_ESRRESTORE_ADDR,v)
#define HWIO_MPM2_XPU_ESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_XPU_ESRRESTORE_ADDR,m,v,HWIO_MPM2_XPU_ESRRESTORE_IN)
#define HWIO_MPM2_XPU_ESRRESTORE_MULTI_BMSK                       0x80000000
#define HWIO_MPM2_XPU_ESRRESTORE_MULTI_SHFT                             0x1f
#define HWIO_MPM2_XPU_ESRRESTORE_CLMULTI_BMSK                            0x8
#define HWIO_MPM2_XPU_ESRRESTORE_CLMULTI_SHFT                            0x3
#define HWIO_MPM2_XPU_ESRRESTORE_CFGMULTI_BMSK                           0x4
#define HWIO_MPM2_XPU_ESRRESTORE_CFGMULTI_SHFT                           0x2
#define HWIO_MPM2_XPU_ESRRESTORE_CLIENT_BMSK                             0x2
#define HWIO_MPM2_XPU_ESRRESTORE_CLIENT_SHFT                             0x1
#define HWIO_MPM2_XPU_ESRRESTORE_CFG_BMSK                                0x1
#define HWIO_MPM2_XPU_ESRRESTORE_CFG_SHFT                                0x0

#define HWIO_MPM2_XPU_ESYNR0_ADDR                                 (MPM2_APU_REG_BASE      + 0x000000d0)
#define HWIO_MPM2_XPU_ESYNR0_PHYS                                 (MPM2_APU_REG_BASE_PHYS + 0x000000d0)
#define HWIO_MPM2_XPU_ESYNR0_OFFS                                 (MPM2_APU_REG_BASE_OFFS + 0x000000d0)
#define HWIO_MPM2_XPU_ESYNR0_RMSK                                 0xffffffff
#define HWIO_MPM2_XPU_ESYNR0_IN          \
        in_dword_masked(HWIO_MPM2_XPU_ESYNR0_ADDR, HWIO_MPM2_XPU_ESYNR0_RMSK)
#define HWIO_MPM2_XPU_ESYNR0_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_ESYNR0_ADDR, m)
#define HWIO_MPM2_XPU_ESYNR0_ATID_BMSK                            0xff000000
#define HWIO_MPM2_XPU_ESYNR0_ATID_SHFT                                  0x18
#define HWIO_MPM2_XPU_ESYNR0_AVMID_BMSK                             0xff0000
#define HWIO_MPM2_XPU_ESYNR0_AVMID_SHFT                                 0x10
#define HWIO_MPM2_XPU_ESYNR0_ABID_BMSK                                0xe000
#define HWIO_MPM2_XPU_ESYNR0_ABID_SHFT                                   0xd
#define HWIO_MPM2_XPU_ESYNR0_APID_BMSK                                0x1f00
#define HWIO_MPM2_XPU_ESYNR0_APID_SHFT                                   0x8
#define HWIO_MPM2_XPU_ESYNR0_AMID_BMSK                                  0xff
#define HWIO_MPM2_XPU_ESYNR0_AMID_SHFT                                   0x0

#define HWIO_MPM2_XPU_ESYNR1_ADDR                                 (MPM2_APU_REG_BASE      + 0x000000d4)
#define HWIO_MPM2_XPU_ESYNR1_PHYS                                 (MPM2_APU_REG_BASE_PHYS + 0x000000d4)
#define HWIO_MPM2_XPU_ESYNR1_OFFS                                 (MPM2_APU_REG_BASE_OFFS + 0x000000d4)
#define HWIO_MPM2_XPU_ESYNR1_RMSK                                 0xffffffff
#define HWIO_MPM2_XPU_ESYNR1_IN          \
        in_dword_masked(HWIO_MPM2_XPU_ESYNR1_ADDR, HWIO_MPM2_XPU_ESYNR1_RMSK)
#define HWIO_MPM2_XPU_ESYNR1_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_ESYNR1_ADDR, m)
#define HWIO_MPM2_XPU_ESYNR1_DCD_BMSK                             0x80000000
#define HWIO_MPM2_XPU_ESYNR1_DCD_SHFT                                   0x1f
#define HWIO_MPM2_XPU_ESYNR1_AC_BMSK                              0x40000000
#define HWIO_MPM2_XPU_ESYNR1_AC_SHFT                                    0x1e
#define HWIO_MPM2_XPU_ESYNR1_BURSTLEN_BMSK                        0x20000000
#define HWIO_MPM2_XPU_ESYNR1_BURSTLEN_SHFT                              0x1d
#define HWIO_MPM2_XPU_ESYNR1_ARDALLOCATE_BMSK                     0x10000000
#define HWIO_MPM2_XPU_ESYNR1_ARDALLOCATE_SHFT                           0x1c
#define HWIO_MPM2_XPU_ESYNR1_ABURST_BMSK                           0x8000000
#define HWIO_MPM2_XPU_ESYNR1_ABURST_SHFT                                0x1b
#define HWIO_MPM2_XPU_ESYNR1_AEXCLUSIVE_BMSK                       0x4000000
#define HWIO_MPM2_XPU_ESYNR1_AEXCLUSIVE_SHFT                            0x1a
#define HWIO_MPM2_XPU_ESYNR1_AWRITE_BMSK                           0x2000000
#define HWIO_MPM2_XPU_ESYNR1_AWRITE_SHFT                                0x19
#define HWIO_MPM2_XPU_ESYNR1_AFULL_BMSK                            0x1000000
#define HWIO_MPM2_XPU_ESYNR1_AFULL_SHFT                                 0x18
#define HWIO_MPM2_XPU_ESYNR1_ARDBEADNDXEN_BMSK                      0x800000
#define HWIO_MPM2_XPU_ESYNR1_ARDBEADNDXEN_SHFT                          0x17
#define HWIO_MPM2_XPU_ESYNR1_AOOO_BMSK                              0x400000
#define HWIO_MPM2_XPU_ESYNR1_AOOO_SHFT                                  0x16
#define HWIO_MPM2_XPU_ESYNR1_APREQPRIORITY_BMSK                     0x380000
#define HWIO_MPM2_XPU_ESYNR1_APREQPRIORITY_SHFT                         0x13
#define HWIO_MPM2_XPU_ESYNR1_ASIZE_BMSK                              0x70000
#define HWIO_MPM2_XPU_ESYNR1_ASIZE_SHFT                                 0x10
#define HWIO_MPM2_XPU_ESYNR1_AMSSSELFAUTH_BMSK                        0x8000
#define HWIO_MPM2_XPU_ESYNR1_AMSSSELFAUTH_SHFT                           0xf
#define HWIO_MPM2_XPU_ESYNR1_ALEN_BMSK                                0x7f00
#define HWIO_MPM2_XPU_ESYNR1_ALEN_SHFT                                   0x8
#define HWIO_MPM2_XPU_ESYNR1_AINST_BMSK                                 0x80
#define HWIO_MPM2_XPU_ESYNR1_AINST_SHFT                                  0x7
#define HWIO_MPM2_XPU_ESYNR1_APROTNS_BMSK                               0x40
#define HWIO_MPM2_XPU_ESYNR1_APROTNS_SHFT                                0x6
#define HWIO_MPM2_XPU_ESYNR1_APRIV_BMSK                                 0x20
#define HWIO_MPM2_XPU_ESYNR1_APRIV_SHFT                                  0x5
#define HWIO_MPM2_XPU_ESYNR1_AINNERSHARED_BMSK                          0x10
#define HWIO_MPM2_XPU_ESYNR1_AINNERSHARED_SHFT                           0x4
#define HWIO_MPM2_XPU_ESYNR1_ASHARED_BMSK                                0x8
#define HWIO_MPM2_XPU_ESYNR1_ASHARED_SHFT                                0x3
#define HWIO_MPM2_XPU_ESYNR1_AMEMTYPE_BMSK                               0x7
#define HWIO_MPM2_XPU_ESYNR1_AMEMTYPE_SHFT                               0x0

#define HWIO_MPM2_XPU_ESYNR2_ADDR                                 (MPM2_APU_REG_BASE      + 0x000000d8)
#define HWIO_MPM2_XPU_ESYNR2_PHYS                                 (MPM2_APU_REG_BASE_PHYS + 0x000000d8)
#define HWIO_MPM2_XPU_ESYNR2_OFFS                                 (MPM2_APU_REG_BASE_OFFS + 0x000000d8)
#define HWIO_MPM2_XPU_ESYNR2_RMSK                                        0x7
#define HWIO_MPM2_XPU_ESYNR2_IN          \
        in_dword_masked(HWIO_MPM2_XPU_ESYNR2_ADDR, HWIO_MPM2_XPU_ESYNR2_RMSK)
#define HWIO_MPM2_XPU_ESYNR2_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_ESYNR2_ADDR, m)
#define HWIO_MPM2_XPU_ESYNR2_MODEM_PRT_HIT_BMSK                          0x4
#define HWIO_MPM2_XPU_ESYNR2_MODEM_PRT_HIT_SHFT                          0x2
#define HWIO_MPM2_XPU_ESYNR2_SECURE_PRT_HIT_BMSK                         0x2
#define HWIO_MPM2_XPU_ESYNR2_SECURE_PRT_HIT_SHFT                         0x1
#define HWIO_MPM2_XPU_ESYNR2_NONSECURE_PRT_HIT_BMSK                      0x1
#define HWIO_MPM2_XPU_ESYNR2_NONSECURE_PRT_HIT_SHFT                      0x0

#define HWIO_MPM2_XPU_IDR0_ADDR                                   (MPM2_APU_REG_BASE      + 0x00000074)
#define HWIO_MPM2_XPU_IDR0_PHYS                                   (MPM2_APU_REG_BASE_PHYS + 0x00000074)
#define HWIO_MPM2_XPU_IDR0_OFFS                                   (MPM2_APU_REG_BASE_OFFS + 0x00000074)
#define HWIO_MPM2_XPU_IDR0_RMSK                                   0xc000bfff
#define HWIO_MPM2_XPU_IDR0_IN          \
        in_dword_masked(HWIO_MPM2_XPU_IDR0_ADDR, HWIO_MPM2_XPU_IDR0_RMSK)
#define HWIO_MPM2_XPU_IDR0_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_IDR0_ADDR, m)
#define HWIO_MPM2_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_BMSK          0x80000000
#define HWIO_MPM2_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_SHFT                0x1f
#define HWIO_MPM2_XPU_IDR0_SAVERESTORE_HW_EN_BMSK                 0x40000000
#define HWIO_MPM2_XPU_IDR0_SAVERESTORE_HW_EN_SHFT                       0x1e
#define HWIO_MPM2_XPU_IDR0_BLED_BMSK                                  0x8000
#define HWIO_MPM2_XPU_IDR0_BLED_SHFT                                     0xf
#define HWIO_MPM2_XPU_IDR0_XPUT_BMSK                                  0x3000
#define HWIO_MPM2_XPU_IDR0_XPUT_SHFT                                     0xc
#define HWIO_MPM2_XPU_IDR0_PT_BMSK                                     0x800
#define HWIO_MPM2_XPU_IDR0_PT_SHFT                                       0xb
#define HWIO_MPM2_XPU_IDR0_MV_BMSK                                     0x400
#define HWIO_MPM2_XPU_IDR0_MV_SHFT                                       0xa
#define HWIO_MPM2_XPU_IDR0_NRG_BMSK                                    0x3ff
#define HWIO_MPM2_XPU_IDR0_NRG_SHFT                                      0x0

#define HWIO_MPM2_XPU_IDR1_ADDR                                   (MPM2_APU_REG_BASE      + 0x00000078)
#define HWIO_MPM2_XPU_IDR1_PHYS                                   (MPM2_APU_REG_BASE_PHYS + 0x00000078)
#define HWIO_MPM2_XPU_IDR1_OFFS                                   (MPM2_APU_REG_BASE_OFFS + 0x00000078)
#define HWIO_MPM2_XPU_IDR1_RMSK                                   0x7f3ffeff
#define HWIO_MPM2_XPU_IDR1_IN          \
        in_dword_masked(HWIO_MPM2_XPU_IDR1_ADDR, HWIO_MPM2_XPU_IDR1_RMSK)
#define HWIO_MPM2_XPU_IDR1_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_IDR1_ADDR, m)
#define HWIO_MPM2_XPU_IDR1_AMT_HW_ENABLE_BMSK                     0x40000000
#define HWIO_MPM2_XPU_IDR1_AMT_HW_ENABLE_SHFT                           0x1e
#define HWIO_MPM2_XPU_IDR1_CLIENT_ADDR_WIDTH_BMSK                 0x3f000000
#define HWIO_MPM2_XPU_IDR1_CLIENT_ADDR_WIDTH_SHFT                       0x18
#define HWIO_MPM2_XPU_IDR1_CONFIG_ADDR_WIDTH_BMSK                   0x3f0000
#define HWIO_MPM2_XPU_IDR1_CONFIG_ADDR_WIDTH_SHFT                       0x10
#define HWIO_MPM2_XPU_IDR1_QRIB_EN_BMSK                               0x8000
#define HWIO_MPM2_XPU_IDR1_QRIB_EN_SHFT                                  0xf
#define HWIO_MPM2_XPU_IDR1_ASYNC_MODE_BMSK                            0x4000
#define HWIO_MPM2_XPU_IDR1_ASYNC_MODE_SHFT                               0xe
#define HWIO_MPM2_XPU_IDR1_CONFIG_TYPE_BMSK                           0x2000
#define HWIO_MPM2_XPU_IDR1_CONFIG_TYPE_SHFT                              0xd
#define HWIO_MPM2_XPU_IDR1_CLIENT_PIPELINE_ENABLED_BMSK               0x1000
#define HWIO_MPM2_XPU_IDR1_CLIENT_PIPELINE_ENABLED_SHFT                  0xc
#define HWIO_MPM2_XPU_IDR1_MSA_CHECK_HW_ENABLE_BMSK                    0x800
#define HWIO_MPM2_XPU_IDR1_MSA_CHECK_HW_ENABLE_SHFT                      0xb
#define HWIO_MPM2_XPU_IDR1_XPU_SYND_REG_ABSENT_BMSK                    0x400
#define HWIO_MPM2_XPU_IDR1_XPU_SYND_REG_ABSENT_SHFT                      0xa
#define HWIO_MPM2_XPU_IDR1_TZXPU_BMSK                                  0x200
#define HWIO_MPM2_XPU_IDR1_TZXPU_SHFT                                    0x9
#define HWIO_MPM2_XPU_IDR1_NVMID_BMSK                                   0xff
#define HWIO_MPM2_XPU_IDR1_NVMID_SHFT                                    0x0

#define HWIO_MPM2_XPU_REV_ADDR                                    (MPM2_APU_REG_BASE      + 0x0000007c)
#define HWIO_MPM2_XPU_REV_PHYS                                    (MPM2_APU_REG_BASE_PHYS + 0x0000007c)
#define HWIO_MPM2_XPU_REV_OFFS                                    (MPM2_APU_REG_BASE_OFFS + 0x0000007c)
#define HWIO_MPM2_XPU_REV_RMSK                                    0xffffffff
#define HWIO_MPM2_XPU_REV_IN          \
        in_dword_masked(HWIO_MPM2_XPU_REV_ADDR, HWIO_MPM2_XPU_REV_RMSK)
#define HWIO_MPM2_XPU_REV_INM(m)      \
        in_dword_masked(HWIO_MPM2_XPU_REV_ADDR, m)
#define HWIO_MPM2_XPU_REV_MAJOR_BMSK                              0xf0000000
#define HWIO_MPM2_XPU_REV_MAJOR_SHFT                                    0x1c
#define HWIO_MPM2_XPU_REV_MINOR_BMSK                               0xfff0000
#define HWIO_MPM2_XPU_REV_MINOR_SHFT                                    0x10
#define HWIO_MPM2_XPU_REV_STEP_BMSK                                   0xffff
#define HWIO_MPM2_XPU_REV_STEP_SHFT                                      0x0

#define HWIO_MPM2_XPU_RGn_RACRm_ADDR(n,m)                         (MPM2_APU_REG_BASE      + 0x00000200 + 0x80 * (n) + 0x4 * (m))
#define HWIO_MPM2_XPU_RGn_RACRm_PHYS(n,m)                         (MPM2_APU_REG_BASE_PHYS + 0x00000200 + 0x80 * (n) + 0x4 * (m))
#define HWIO_MPM2_XPU_RGn_RACRm_OFFS(n,m)                         (MPM2_APU_REG_BASE_OFFS + 0x00000200 + 0x80 * (n) + 0x4 * (m))
#define HWIO_MPM2_XPU_RGn_RACRm_RMSK                               0x3ff03ff
#define HWIO_MPM2_XPU_RGn_RACRm_MAXn                                      10
#define HWIO_MPM2_XPU_RGn_RACRm_MAXm                                       0
#define HWIO_MPM2_XPU_RGn_RACRm_INI2(n,m)        \
        in_dword_masked(HWIO_MPM2_XPU_RGn_RACRm_ADDR(n,m), HWIO_MPM2_XPU_RGn_RACRm_RMSK)
#define HWIO_MPM2_XPU_RGn_RACRm_INMI2(n,m,mask)    \
        in_dword_masked(HWIO_MPM2_XPU_RGn_RACRm_ADDR(n,m), mask)
#define HWIO_MPM2_XPU_RGn_RACRm_OUTI2(n,m,val)    \
        out_dword(HWIO_MPM2_XPU_RGn_RACRm_ADDR(n,m),val)
#define HWIO_MPM2_XPU_RGn_RACRm_OUTMI2(n,m,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_XPU_RGn_RACRm_ADDR(n,m),mask,val,HWIO_MPM2_XPU_RGn_RACRm_INI2(n,m))
#define HWIO_MPM2_XPU_RGn_RACRm_ROGE_BMSK                          0x2000000
#define HWIO_MPM2_XPU_RGn_RACRm_ROGE_SHFT                               0x19
#define HWIO_MPM2_XPU_RGn_RACRm_ROE_BMSK                           0x1000000
#define HWIO_MPM2_XPU_RGn_RACRm_ROE_SHFT                                0x18
#define HWIO_MPM2_XPU_RGn_RACRm_ROVMID_BMSK                         0xff0000
#define HWIO_MPM2_XPU_RGn_RACRm_ROVMID_SHFT                             0x10
#define HWIO_MPM2_XPU_RGn_RACRm_RWGE_BMSK                              0x200
#define HWIO_MPM2_XPU_RGn_RACRm_RWGE_SHFT                                0x9
#define HWIO_MPM2_XPU_RGn_RACRm_RWE_BMSK                               0x100
#define HWIO_MPM2_XPU_RGn_RACRm_RWE_SHFT                                 0x8
#define HWIO_MPM2_XPU_RGn_RACRm_RWVMID_BMSK                             0xff
#define HWIO_MPM2_XPU_RGn_RACRm_RWVMID_SHFT                              0x0

#define HWIO_MPM2_XPU_RGn_SCR_ADDR(n)                             (MPM2_APU_REG_BASE      + 0x00000250 + 0x80 * (n))
#define HWIO_MPM2_XPU_RGn_SCR_PHYS(n)                             (MPM2_APU_REG_BASE_PHYS + 0x00000250 + 0x80 * (n))
#define HWIO_MPM2_XPU_RGn_SCR_OFFS(n)                             (MPM2_APU_REG_BASE_OFFS + 0x00000250 + 0x80 * (n))
#define HWIO_MPM2_XPU_RGn_SCR_RMSK                                      0x3f
#define HWIO_MPM2_XPU_RGn_SCR_MAXn                                        10
#define HWIO_MPM2_XPU_RGn_SCR_INI(n)        \
        in_dword_masked(HWIO_MPM2_XPU_RGn_SCR_ADDR(n), HWIO_MPM2_XPU_RGn_SCR_RMSK)
#define HWIO_MPM2_XPU_RGn_SCR_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_XPU_RGn_SCR_ADDR(n), mask)
#define HWIO_MPM2_XPU_RGn_SCR_OUTI(n,val)    \
        out_dword(HWIO_MPM2_XPU_RGn_SCR_ADDR(n),val)
#define HWIO_MPM2_XPU_RGn_SCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_XPU_RGn_SCR_ADDR(n),mask,val,HWIO_MPM2_XPU_RGn_SCR_INI(n))
#define HWIO_MPM2_XPU_RGn_SCR_SCLROE_BMSK                               0x20
#define HWIO_MPM2_XPU_RGn_SCR_SCLROE_SHFT                                0x5
#define HWIO_MPM2_XPU_RGn_SCR_VMIDCLROE_BMSK                            0x10
#define HWIO_MPM2_XPU_RGn_SCR_VMIDCLROE_SHFT                             0x4
#define HWIO_MPM2_XPU_RGn_SCR_MSACLROE_BMSK                              0x8
#define HWIO_MPM2_XPU_RGn_SCR_MSACLROE_SHFT                              0x3
#define HWIO_MPM2_XPU_RGn_SCR_VMIDCLRWE_BMSK                             0x4
#define HWIO_MPM2_XPU_RGn_SCR_VMIDCLRWE_SHFT                             0x2
#define HWIO_MPM2_XPU_RGn_SCR_MSACLRWE_BMSK                              0x2
#define HWIO_MPM2_XPU_RGn_SCR_MSACLRWE_SHFT                              0x1
#define HWIO_MPM2_XPU_RGn_SCR_NS_BMSK                                    0x1
#define HWIO_MPM2_XPU_RGn_SCR_NS_SHFT                                    0x0

#define HWIO_MPM2_XPU_RGn_MCR_ADDR(n)                             (MPM2_APU_REG_BASE      + 0x00000254 + 0x80 * (n))
#define HWIO_MPM2_XPU_RGn_MCR_PHYS(n)                             (MPM2_APU_REG_BASE_PHYS + 0x00000254 + 0x80 * (n))
#define HWIO_MPM2_XPU_RGn_MCR_OFFS(n)                             (MPM2_APU_REG_BASE_OFFS + 0x00000254 + 0x80 * (n))
#define HWIO_MPM2_XPU_RGn_MCR_RMSK                                      0x3f
#define HWIO_MPM2_XPU_RGn_MCR_MAXn                                        10
#define HWIO_MPM2_XPU_RGn_MCR_INI(n)        \
        in_dword_masked(HWIO_MPM2_XPU_RGn_MCR_ADDR(n), HWIO_MPM2_XPU_RGn_MCR_RMSK)
#define HWIO_MPM2_XPU_RGn_MCR_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_XPU_RGn_MCR_ADDR(n), mask)
#define HWIO_MPM2_XPU_RGn_MCR_OUTI(n,val)    \
        out_dword(HWIO_MPM2_XPU_RGn_MCR_ADDR(n),val)
#define HWIO_MPM2_XPU_RGn_MCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_XPU_RGn_MCR_ADDR(n),mask,val,HWIO_MPM2_XPU_RGn_MCR_INI(n))
#define HWIO_MPM2_XPU_RGn_MCR_MSACLROE_BMSK                             0x20
#define HWIO_MPM2_XPU_RGn_MCR_MSACLROE_SHFT                              0x5
#define HWIO_MPM2_XPU_RGn_MCR_VMIDCLROE_BMSK                            0x10
#define HWIO_MPM2_XPU_RGn_MCR_VMIDCLROE_SHFT                             0x4
#define HWIO_MPM2_XPU_RGn_MCR_SCLROE_BMSK                                0x8
#define HWIO_MPM2_XPU_RGn_MCR_SCLROE_SHFT                                0x3
#define HWIO_MPM2_XPU_RGn_MCR_VMIDCLE_BMSK                               0x4
#define HWIO_MPM2_XPU_RGn_MCR_VMIDCLE_SHFT                               0x2
#define HWIO_MPM2_XPU_RGn_MCR_SCLE_BMSK                                  0x2
#define HWIO_MPM2_XPU_RGn_MCR_SCLE_SHFT                                  0x1
#define HWIO_MPM2_XPU_RGn_MCR_MSAE_BMSK                                  0x1
#define HWIO_MPM2_XPU_RGn_MCR_MSAE_SHFT                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_G_CTRL_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_G_CTRL_CNTR_REG_BASE                             (MPM2_MPM_BASE      + 0x00001000)
#define MPM2_G_CTRL_CNTR_REG_BASE_PHYS                        (MPM2_MPM_BASE_PHYS + 0x00001000)
#define MPM2_G_CTRL_CNTR_REG_BASE_OFFS                        0x00001000

#define HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR                      (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_PHYS                      (MPM2_G_CTRL_CNTR_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_OFFS                      (MPM2_G_CTRL_CNTR_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_RMSK                           0x103
#define HWIO_MPM2_MPM_CONTROL_CNTCR_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR, HWIO_MPM2_MPM_CONTROL_CNTCR_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCR_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_FCREQ_BMSK                     0x100
#define HWIO_MPM2_MPM_CONTROL_CNTCR_FCREQ_SHFT                       0x8
#define HWIO_MPM2_MPM_CONTROL_CNTCR_HDBG_BMSK                        0x2
#define HWIO_MPM2_MPM_CONTROL_CNTCR_HDBG_SHFT                        0x1
#define HWIO_MPM2_MPM_CONTROL_CNTCR_EN_BMSK                          0x1
#define HWIO_MPM2_MPM_CONTROL_CNTCR_EN_SHFT                          0x0

#define HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR                      (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_PHYS                      (MPM2_G_CTRL_CNTR_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_OFFS                      (MPM2_G_CTRL_CNTR_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_RMSK                           0x102
#define HWIO_MPM2_MPM_CONTROL_CNTSR_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR, HWIO_MPM2_MPM_CONTROL_CNTSR_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_FCACK_BMSK                     0x100
#define HWIO_MPM2_MPM_CONTROL_CNTSR_FCACK_SHFT                       0x8
#define HWIO_MPM2_MPM_CONTROL_CNTSR_HDBG_BMSK                        0x2
#define HWIO_MPM2_MPM_CONTROL_CNTSR_HDBG_SHFT                        0x1

#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR                   (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000008)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_PHYS                   (MPM2_G_CTRL_CNTR_REG_BASE_PHYS + 0x00000008)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_OFFS                   (MPM2_G_CTRL_CNTR_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_RMSK                   0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR, HWIO_MPM2_MPM_CONTROL_CNTCV_LO_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCV_LO_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_LOAD_VAL_BMSK          0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_LOAD_VAL_SHFT                 0x0

#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR                   (MPM2_G_CTRL_CNTR_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_PHYS                   (MPM2_G_CTRL_CNTR_REG_BASE_PHYS + 0x0000000c)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_OFFS                   (MPM2_G_CTRL_CNTR_REG_BASE_OFFS + 0x0000000c)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_RMSK                     0xffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR, HWIO_MPM2_MPM_CONTROL_CNTCV_HI_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCV_HI_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_LOAD_VAL_BMSK            0xffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_LOAD_VAL_SHFT                 0x0

#define HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR                    (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000020)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_PHYS                    (MPM2_G_CTRL_CNTR_REG_BASE_PHYS + 0x00000020)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_OFFS                    (MPM2_G_CTRL_CNTR_REG_BASE_OFFS + 0x00000020)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_RMSK                    0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR, HWIO_MPM2_MPM_CONTROL_CNTFID0_RMSK)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTFID0_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_FREQ_BMSK               0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_FREQ_SHFT                      0x0

#define HWIO_MPM2_MPM_CONTROL_ID_ADDR                         (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_MPM_CONTROL_ID_PHYS                         (MPM2_G_CTRL_CNTR_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_MPM2_MPM_CONTROL_ID_OFFS                         (MPM2_G_CTRL_CNTR_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_MPM2_MPM_CONTROL_ID_RMSK                         0xffffffff
#define HWIO_MPM2_MPM_CONTROL_ID_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_ID_ADDR, HWIO_MPM2_MPM_CONTROL_ID_RMSK)
#define HWIO_MPM2_MPM_CONTROL_ID_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_ID_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_ID_MAJOR_BMSK                   0xf0000000
#define HWIO_MPM2_MPM_CONTROL_ID_MAJOR_SHFT                         0x1c
#define HWIO_MPM2_MPM_CONTROL_ID_MINOR_BMSK                    0xfff0000
#define HWIO_MPM2_MPM_CONTROL_ID_MINOR_SHFT                         0x10
#define HWIO_MPM2_MPM_CONTROL_ID_STEP_BMSK                        0xffff
#define HWIO_MPM2_MPM_CONTROL_ID_STEP_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_G_RD_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_G_RD_CNTR_REG_BASE                         (MPM2_MPM_BASE      + 0x00002000)
#define MPM2_G_RD_CNTR_REG_BASE_PHYS                    (MPM2_MPM_BASE_PHYS + 0x00002000)
#define MPM2_G_RD_CNTR_REG_BASE_OFFS                    0x00002000

#define HWIO_MPM2_MPM_READ_CNTCV_LO_ADDR                (MPM2_G_RD_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_READ_CNTCV_LO_PHYS                (MPM2_G_RD_CNTR_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_MPM_READ_CNTCV_LO_OFFS                (MPM2_G_RD_CNTR_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_READ_CNTCV_LO_RMSK                0xffffffff
#define HWIO_MPM2_MPM_READ_CNTCV_LO_IN          \
        in_dword_masked(HWIO_MPM2_MPM_READ_CNTCV_LO_ADDR, HWIO_MPM2_MPM_READ_CNTCV_LO_RMSK)
#define HWIO_MPM2_MPM_READ_CNTCV_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_READ_CNTCV_LO_ADDR, m)
#define HWIO_MPM2_MPM_READ_CNTCV_LO_VALUE_BMSK          0xffffffff
#define HWIO_MPM2_MPM_READ_CNTCV_LO_VALUE_SHFT                 0x0

#define HWIO_MPM2_MPM_READ_CNTCV_HI_ADDR                (MPM2_G_RD_CNTR_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_READ_CNTCV_HI_PHYS                (MPM2_G_RD_CNTR_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_MPM_READ_CNTCV_HI_OFFS                (MPM2_G_RD_CNTR_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_MPM_READ_CNTCV_HI_RMSK                  0xffffff
#define HWIO_MPM2_MPM_READ_CNTCV_HI_IN          \
        in_dword_masked(HWIO_MPM2_MPM_READ_CNTCV_HI_ADDR, HWIO_MPM2_MPM_READ_CNTCV_HI_RMSK)
#define HWIO_MPM2_MPM_READ_CNTCV_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_READ_CNTCV_HI_ADDR, m)
#define HWIO_MPM2_MPM_READ_CNTCV_HI_VALUE_BMSK            0xffffff
#define HWIO_MPM2_MPM_READ_CNTCV_HI_VALUE_SHFT                 0x0

#define HWIO_MPM2_MPM_READ_ID_ADDR                      (MPM2_G_RD_CNTR_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_MPM_READ_ID_PHYS                      (MPM2_G_RD_CNTR_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_MPM2_MPM_READ_ID_OFFS                      (MPM2_G_RD_CNTR_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_MPM2_MPM_READ_ID_RMSK                      0xffffffff
#define HWIO_MPM2_MPM_READ_ID_IN          \
        in_dword_masked(HWIO_MPM2_MPM_READ_ID_ADDR, HWIO_MPM2_MPM_READ_ID_RMSK)
#define HWIO_MPM2_MPM_READ_ID_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_READ_ID_ADDR, m)
#define HWIO_MPM2_MPM_READ_ID_MAJOR_BMSK                0xf0000000
#define HWIO_MPM2_MPM_READ_ID_MAJOR_SHFT                      0x1c
#define HWIO_MPM2_MPM_READ_ID_MINOR_BMSK                 0xfff0000
#define HWIO_MPM2_MPM_READ_ID_MINOR_SHFT                      0x10
#define HWIO_MPM2_MPM_READ_ID_STEP_BMSK                     0xffff
#define HWIO_MPM2_MPM_READ_ID_STEP_SHFT                        0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_MPM
 *--------------------------------------------------------------------------*/

#define MPM2_MPM_REG_BASE                                                     (MPM2_MPM_BASE      + 0x00000000)
#define MPM2_MPM_REG_BASE_PHYS                                                (MPM2_MPM_BASE_PHYS + 0x00000000)
#define MPM2_MPM_REG_BASE_OFFS                                                0x00000000

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_1_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_2_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000008)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x00000008)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_3_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR                                  (MPM2_MPM_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x0000000c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x0000000c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_4_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000010)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x00000010)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x00000010)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_5_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000014)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x00000014)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x00000014)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_6_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000018)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x00000018)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x00000018)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_7_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR                                  (MPM2_MPM_REG_BASE      + 0x0000001c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x0000001c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x0000001c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_8_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000020)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x00000020)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x00000020)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_RMSK                                  0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_DELAY_BMSK                            0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_DELAY_SHFT                                  0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_ID_BMSK                          0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_ID_SHFT                               0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_ADDR_BMSK                         0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_ADDR_SHFT                              0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_DATA_BMSK                             0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_9_SLAVE_DATA_SHFT                              0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000024)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000024)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000024)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_10_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000028)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000028)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000028)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_11_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR                                 (MPM2_MPM_REG_BASE      + 0x0000002c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x0000002c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x0000002c)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR, HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_ON_CFG_12_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000030)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000030)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000030)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_1_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000034)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000034)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000034)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_2_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000038)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000038)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000038)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_3_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR                                 (MPM2_MPM_REG_BASE      + 0x0000003c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x0000003c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x0000003c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_4_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000040)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000040)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000040)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_5_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000044)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000044)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000044)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_6_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000048)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000048)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000048)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_7_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR                                 (MPM2_MPM_REG_BASE      + 0x0000004c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x0000004c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x0000004c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_8_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR                                 (MPM2_MPM_REG_BASE      + 0x00000050)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x00000050)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x00000050)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_DELAY_BMSK                           0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_DELAY_SHFT                                 0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_ID_BMSK                         0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_ID_SHFT                              0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_ADDR_BMSK                        0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_ADDR_SHFT                             0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_DATA_BMSK                            0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_9_SLAVE_DATA_SHFT                             0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR                                (MPM2_MPM_REG_BASE      + 0x00000054)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_PHYS                                (MPM2_MPM_REG_BASE_PHYS + 0x00000054)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_OFFS                                (MPM2_MPM_REG_BASE_OFFS + 0x00000054)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_RMSK                                0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_DELAY_BMSK                          0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_DELAY_SHFT                                0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_ID_BMSK                        0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_ID_SHFT                             0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_ADDR_BMSK                       0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_ADDR_SHFT                            0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_DATA_BMSK                           0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_10_SLAVE_DATA_SHFT                            0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR                                (MPM2_MPM_REG_BASE      + 0x00000058)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_PHYS                                (MPM2_MPM_REG_BASE_PHYS + 0x00000058)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_OFFS                                (MPM2_MPM_REG_BASE_OFFS + 0x00000058)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_RMSK                                0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_DELAY_BMSK                          0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_DELAY_SHFT                                0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_ID_BMSK                        0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_ID_SHFT                             0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_ADDR_BMSK                       0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_ADDR_SHFT                            0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_DATA_BMSK                           0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_11_SLAVE_DATA_SHFT                            0x0

#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR                                (MPM2_MPM_REG_BASE      + 0x0000005c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_PHYS                                (MPM2_MPM_REG_BASE_PHYS + 0x0000005c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_OFFS                                (MPM2_MPM_REG_BASE_OFFS + 0x0000005c)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_RMSK                                0xffffffff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR, HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_RMSK)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR, m)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR,v)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_ADDR,m,v,HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_IN)
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_DELAY_BMSK                          0xf0000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_DELAY_SHFT                                0x1c
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_ID_BMSK                        0xf000000
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_ID_SHFT                             0x18
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_ADDR_BMSK                       0xffff00
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_ADDR_SHFT                            0x8
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_DATA_BMSK                           0xff
#define HWIO_MPM2_MPM_PMIC_VDD_OFF_CFG_12_SLAVE_DATA_SHFT                            0x0

#define HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR                                       (MPM2_MPM_REG_BASE      + 0x00000060)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x00000060)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x00000060)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_RMSK                                             0xff
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR, HWIO_MPM2_MPM_SPMI_CMD_CFG_RMSK)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR, m)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR,v)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_SPMI_CMD_CFG_ADDR,m,v,HWIO_MPM2_MPM_SPMI_CMD_CFG_IN)
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_NUM_CMD_OFF_BMSK                                 0xf0
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_NUM_CMD_OFF_SHFT                                  0x4
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_NUM_CMD_ON_BMSK                                   0xf
#define HWIO_MPM2_MPM_SPMI_CMD_CFG_NUM_CMD_ON_SHFT                                   0x0

#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR                                    (MPM2_MPM_REG_BASE      + 0x00000064)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_PHYS                                    (MPM2_MPM_REG_BASE_PHYS + 0x00000064)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_OFFS                                    (MPM2_MPM_REG_BASE_OFFS + 0x00000064)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_RMSK                                         0x7ff
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR, HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_RMSK)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR, m)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR,v)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_ADDR,m,v,HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_IN)
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_XO_DIV_BMSK                                  0x600
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_XO_DIV_SHFT                                    0x9
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_RO_DIV_BMSK                                  0x1ff
#define HWIO_MPM2_MPM_SPMI_CDIV_CNTRL_RO_DIV_SHFT                                    0x0

#define HWIO_MPM2_MPM_CXO_CTRL_ADDR                                           (MPM2_MPM_REG_BASE      + 0x00000068)
#define HWIO_MPM2_MPM_CXO_CTRL_PHYS                                           (MPM2_MPM_REG_BASE_PHYS + 0x00000068)
#define HWIO_MPM2_MPM_CXO_CTRL_OFFS                                           (MPM2_MPM_REG_BASE_OFFS + 0x00000068)
#define HWIO_MPM2_MPM_CXO_CTRL_RMSK                                                  0x3
#define HWIO_MPM2_MPM_CXO_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CXO_CTRL_ADDR, HWIO_MPM2_MPM_CXO_CTRL_RMSK)
#define HWIO_MPM2_MPM_CXO_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CXO_CTRL_ADDR, m)
#define HWIO_MPM2_MPM_CXO_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CXO_CTRL_ADDR,v)
#define HWIO_MPM2_MPM_CXO_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CXO_CTRL_ADDR,m,v,HWIO_MPM2_MPM_CXO_CTRL_IN)
#define HWIO_MPM2_MPM_CXO_CTRL_CXOPAD_DIS_BMSK                                       0x2
#define HWIO_MPM2_MPM_CXO_CTRL_CXOPAD_DIS_SHFT                                       0x1
#define HWIO_MPM2_MPM_CXO_CTRL_CXO_DIS_BMSK                                          0x1
#define HWIO_MPM2_MPM_CXO_CTRL_CXO_DIS_SHFT                                          0x0

#define HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR                                       (MPM2_MPM_REG_BASE      + 0x0000006c)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x0000006c)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x0000006c)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_RMSK                                             0x7f
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR, HWIO_MPM2_MPM_PXO_OSC_CTRL_RMSK)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR, m)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR,v)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PXO_OSC_CTRL_ADDR,m,v,HWIO_MPM2_MPM_PXO_OSC_CTRL_IN)
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_GAIN_BMSK                                0x70
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_GAIN_SHFT                                 0x4
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXOPAD_CORE_DIS_BMSK                              0x8
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXOPAD_CORE_DIS_SHFT                              0x3
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_SLEEP_BMSK                                0x4
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_SLEEP_SHFT                                0x2
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXOPAD_HV_DIS_BMSK                                0x2
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXOPAD_HV_DIS_SHFT                                0x1
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_RF_BYPASS_BMSK                            0x1
#define HWIO_MPM2_MPM_PXO_OSC_CTRL_PXO_OSC_RF_BYPASS_SHFT                            0x0

#define HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR                                      (MPM2_MPM_REG_BASE      + 0x00000070)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_PHYS                                      (MPM2_MPM_REG_BASE_PHYS + 0x00000070)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_OFFS                                      (MPM2_MPM_REG_BASE_OFFS + 0x00000070)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_RMSK                                         0xf5fff
#define HWIO_MPM2_MPM_LOW_POWER_CFG_IN          \
        in_dword_masked(HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR, HWIO_MPM2_MPM_LOW_POWER_CFG_RMSK)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR, m)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR,v)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_LOW_POWER_CFG_ADDR,m,v,HWIO_MPM2_MPM_LOW_POWER_CFG_IN)
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SMEM_EN_BMSK                                 0x80000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SMEM_EN_SHFT                                    0x13
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_FREEZE_IO_M_BMSK                     0x40000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_FREEZE_IO_M_SHFT                        0x12
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CLAMP_MEM_BMSK                       0x20000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CLAMP_MEM_SHFT                          0x11
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_HW_RESTORED_BMSK                     0x10000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_HW_RESTORED_SHFT                        0x10
#define HWIO_MPM2_MPM_LOW_POWER_CFG_ENABLE_CXOPAD_GATING_BMSK                     0x4000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_ENABLE_CXOPAD_GATING_SHFT                        0xe
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_TRST_N_BMSK                           0x1000
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_TRST_N_SHFT                              0xc
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CORE_RESET_BMSK                        0x800
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CORE_RESET_SHFT                          0xb
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CLAMPS_BMSK                            0x400
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_CLAMPS_SHFT                              0xa
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_FREEZE_BMSK                            0x200
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DISABLE_FREEZE_SHFT                              0x9
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DEBUG_ON_BMSK                                  0x100
#define HWIO_MPM2_MPM_LOW_POWER_CFG_DEBUG_ON_SHFT                                    0x8
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SW_EBI1_CTL_ENABLE_BMSK                         0x80
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SW_EBI1_CTL_ENABLE_SHFT                          0x7
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SW_EBI1_CTL_VALUE_BMSK                          0x40
#define HWIO_MPM2_MPM_LOW_POWER_CFG_SW_EBI1_CTL_VALUE_SHFT                           0x6
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDMX_MIN_EN_BMSK                               0x20
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDMX_MIN_EN_SHFT                                0x5
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDMX_PC_EN_BMSK                                0x10
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDMX_PC_EN_SHFT                                 0x4
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDCX_MIN_EN_BMSK                                0x8
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDCX_MIN_EN_SHFT                                0x3
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDCX_PC_EN_BMSK                                 0x4
#define HWIO_MPM2_MPM_LOW_POWER_CFG_VDDCX_PC_EN_SHFT                                 0x2
#define HWIO_MPM2_MPM_LOW_POWER_CFG_PXO_SD_EN_BMSK                                   0x2
#define HWIO_MPM2_MPM_LOW_POWER_CFG_PXO_SD_EN_SHFT                                   0x1
#define HWIO_MPM2_MPM_LOW_POWER_CFG_CXO_SD_EN_BMSK                                   0x1
#define HWIO_MPM2_MPM_LOW_POWER_CFG_CXO_SD_EN_SHFT                                   0x0

#define HWIO_MPM2_MPM_LOW_POWER_STATUS_ADDR                                   (MPM2_MPM_REG_BASE      + 0x00000074)
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_PHYS                                   (MPM2_MPM_REG_BASE_PHYS + 0x00000074)
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_OFFS                                   (MPM2_MPM_REG_BASE_OFFS + 0x00000074)
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_RMSK                                       0x3fff
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_IN          \
        in_dword_masked(HWIO_MPM2_MPM_LOW_POWER_STATUS_ADDR, HWIO_MPM2_MPM_LOW_POWER_STATUS_RMSK)
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_LOW_POWER_STATUS_ADDR, m)
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_SPMI_SEQ_FSM_BMSK                          0x3000
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_SPMI_SEQ_FSM_SHFT                             0xc
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_SMEM_FSM_STATE_BMSK                         0xe00
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_SMEM_FSM_STATE_SHFT                           0x9
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_VDDMX_DOWN_BMSK                             0x100
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_VDDMX_DOWN_SHFT                               0x8
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_VDDCX_DOWN_BMSK                              0x80
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_VDDCX_DOWN_SHFT                               0x7
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_PXO_DOWN_BMSK                                0x40
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_PXO_DOWN_SHFT                                 0x6
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_CXO_DOWN_BMSK                                0x20
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_CXO_DOWN_SHFT                                 0x5
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_POWER_DOWN_BMSK                              0x10
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_POWER_DOWN_SHFT                               0x4
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_FSM_STATE_BMSK                                0xf
#define HWIO_MPM2_MPM_LOW_POWER_STATUS_FSM_STATE_SHFT                                0x0

#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR                              (MPM2_MPM_REG_BASE      + 0x00000078)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_PHYS                              (MPM2_MPM_REG_BASE_PHYS + 0x00000078)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_OFFS                              (MPM2_MPM_REG_BASE_OFFS + 0x00000078)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_RMSK                              0xffffffff
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR, HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_RMSK)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR, m)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR,v)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_ADDR,m,v,HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_IN)
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_CXO_WARMUP_TIME_BMSK              0xffff0000
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_CXO_WARMUP_TIME_SHFT                    0x10
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_POWER_RAMPUP_TIME_BMSK                0xffff
#define HWIO_MPM2_MPM_CXO_POWER_RAMPUP_TIME_POWER_RAMPUP_TIME_SHFT                   0x0

#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR                                    (MPM2_MPM_REG_BASE      + 0x0000007c)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_PHYS                                    (MPM2_MPM_REG_BASE_PHYS + 0x0000007c)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_OFFS                                    (MPM2_MPM_REG_BASE_OFFS + 0x0000007c)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_RMSK                                        0xffff
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR, HWIO_MPM2_MPM_PXO_WARMUP_TIME_RMSK)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR, m)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR,v)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PXO_WARMUP_TIME_ADDR,m,v,HWIO_MPM2_MPM_PXO_WARMUP_TIME_IN)
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_PXO_WARMUP_TIME_BMSK                        0xffff
#define HWIO_MPM2_MPM_PXO_WARMUP_TIME_PXO_WARMUP_TIME_SHFT                           0x0

#define HWIO_MPM2_MPM_HARDWARE_RESTORED_ADDR                                  (MPM2_MPM_REG_BASE      + 0x00000080)
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_PHYS                                  (MPM2_MPM_REG_BASE_PHYS + 0x00000080)
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_OFFS                                  (MPM2_MPM_REG_BASE_OFFS + 0x00000080)
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_RMSK                                         0x1
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_HARDWARE_RESTORED_ADDR,v)
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_DATA_BMSK                                    0x1
#define HWIO_MPM2_MPM_HARDWARE_RESTORED_DATA_SHFT                                    0x0

#define HWIO_MPM2_MPM_INT_CLEAR_1_ADDR                                        (MPM2_MPM_REG_BASE      + 0x00000084)
#define HWIO_MPM2_MPM_INT_CLEAR_1_PHYS                                        (MPM2_MPM_REG_BASE_PHYS + 0x00000084)
#define HWIO_MPM2_MPM_INT_CLEAR_1_OFFS                                        (MPM2_MPM_REG_BASE_OFFS + 0x00000084)
#define HWIO_MPM2_MPM_INT_CLEAR_1_RMSK                                        0xffffffff
#define HWIO_MPM2_MPM_INT_CLEAR_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_CLEAR_1_ADDR,v)
#define HWIO_MPM2_MPM_INT_CLEAR_1_INT_CLEAR_1_BMSK                            0xffffffff
#define HWIO_MPM2_MPM_INT_CLEAR_1_INT_CLEAR_1_SHFT                                   0x0

#define HWIO_MPM2_MPM_INT_CLEAR_2_ADDR                                        (MPM2_MPM_REG_BASE      + 0x00000088)
#define HWIO_MPM2_MPM_INT_CLEAR_2_PHYS                                        (MPM2_MPM_REG_BASE_PHYS + 0x00000088)
#define HWIO_MPM2_MPM_INT_CLEAR_2_OFFS                                        (MPM2_MPM_REG_BASE_OFFS + 0x00000088)
#define HWIO_MPM2_MPM_INT_CLEAR_2_RMSK                                        0xffffffff
#define HWIO_MPM2_MPM_INT_CLEAR_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_CLEAR_2_ADDR,v)
#define HWIO_MPM2_MPM_INT_CLEAR_2_INT_CLEAR_2_BMSK                            0xffffffff
#define HWIO_MPM2_MPM_INT_CLEAR_2_INT_CLEAR_2_SHFT                                   0x0

#define HWIO_MPM2_MPM_INT_EN_1_ADDR                                           (MPM2_MPM_REG_BASE      + 0x0000008c)
#define HWIO_MPM2_MPM_INT_EN_1_PHYS                                           (MPM2_MPM_REG_BASE_PHYS + 0x0000008c)
#define HWIO_MPM2_MPM_INT_EN_1_OFFS                                           (MPM2_MPM_REG_BASE_OFFS + 0x0000008c)
#define HWIO_MPM2_MPM_INT_EN_1_RMSK                                           0xffffffff
#define HWIO_MPM2_MPM_INT_EN_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_EN_1_ADDR, HWIO_MPM2_MPM_INT_EN_1_RMSK)
#define HWIO_MPM2_MPM_INT_EN_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_EN_1_ADDR, m)
#define HWIO_MPM2_MPM_INT_EN_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_EN_1_ADDR,v)
#define HWIO_MPM2_MPM_INT_EN_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_EN_1_ADDR,m,v,HWIO_MPM2_MPM_INT_EN_1_IN)
#define HWIO_MPM2_MPM_INT_EN_1_INT_EN_1_BMSK                                  0xffffffff
#define HWIO_MPM2_MPM_INT_EN_1_INT_EN_1_SHFT                                         0x0

#define HWIO_MPM2_MPM_INT_EN_2_ADDR                                           (MPM2_MPM_REG_BASE      + 0x00000090)
#define HWIO_MPM2_MPM_INT_EN_2_PHYS                                           (MPM2_MPM_REG_BASE_PHYS + 0x00000090)
#define HWIO_MPM2_MPM_INT_EN_2_OFFS                                           (MPM2_MPM_REG_BASE_OFFS + 0x00000090)
#define HWIO_MPM2_MPM_INT_EN_2_RMSK                                           0xffffffff
#define HWIO_MPM2_MPM_INT_EN_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_EN_2_ADDR, HWIO_MPM2_MPM_INT_EN_2_RMSK)
#define HWIO_MPM2_MPM_INT_EN_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_EN_2_ADDR, m)
#define HWIO_MPM2_MPM_INT_EN_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_EN_2_ADDR,v)
#define HWIO_MPM2_MPM_INT_EN_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_EN_2_ADDR,m,v,HWIO_MPM2_MPM_INT_EN_2_IN)
#define HWIO_MPM2_MPM_INT_EN_2_INT_EN_2_BMSK                                  0xffffffff
#define HWIO_MPM2_MPM_INT_EN_2_INT_EN_2_SHFT                                         0x0

#define HWIO_MPM2_MPM_INT_POLARITY_1_ADDR                                     (MPM2_MPM_REG_BASE      + 0x00000094)
#define HWIO_MPM2_MPM_INT_POLARITY_1_PHYS                                     (MPM2_MPM_REG_BASE_PHYS + 0x00000094)
#define HWIO_MPM2_MPM_INT_POLARITY_1_OFFS                                     (MPM2_MPM_REG_BASE_OFFS + 0x00000094)
#define HWIO_MPM2_MPM_INT_POLARITY_1_RMSK                                     0xffffffff
#define HWIO_MPM2_MPM_INT_POLARITY_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_POLARITY_1_ADDR, HWIO_MPM2_MPM_INT_POLARITY_1_RMSK)
#define HWIO_MPM2_MPM_INT_POLARITY_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_POLARITY_1_ADDR, m)
#define HWIO_MPM2_MPM_INT_POLARITY_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_POLARITY_1_ADDR,v)
#define HWIO_MPM2_MPM_INT_POLARITY_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_POLARITY_1_ADDR,m,v,HWIO_MPM2_MPM_INT_POLARITY_1_IN)
#define HWIO_MPM2_MPM_INT_POLARITY_1_INT_POLARITY_1_BMSK                      0xffffffff
#define HWIO_MPM2_MPM_INT_POLARITY_1_INT_POLARITY_1_SHFT                             0x0

#define HWIO_MPM2_MPM_INT_POLARITY_2_ADDR                                     (MPM2_MPM_REG_BASE      + 0x00000098)
#define HWIO_MPM2_MPM_INT_POLARITY_2_PHYS                                     (MPM2_MPM_REG_BASE_PHYS + 0x00000098)
#define HWIO_MPM2_MPM_INT_POLARITY_2_OFFS                                     (MPM2_MPM_REG_BASE_OFFS + 0x00000098)
#define HWIO_MPM2_MPM_INT_POLARITY_2_RMSK                                     0xffffffff
#define HWIO_MPM2_MPM_INT_POLARITY_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_POLARITY_2_ADDR, HWIO_MPM2_MPM_INT_POLARITY_2_RMSK)
#define HWIO_MPM2_MPM_INT_POLARITY_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_POLARITY_2_ADDR, m)
#define HWIO_MPM2_MPM_INT_POLARITY_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_POLARITY_2_ADDR,v)
#define HWIO_MPM2_MPM_INT_POLARITY_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_POLARITY_2_ADDR,m,v,HWIO_MPM2_MPM_INT_POLARITY_2_IN)
#define HWIO_MPM2_MPM_INT_POLARITY_2_INT_POLARITY_2_BMSK                      0xffffffff
#define HWIO_MPM2_MPM_INT_POLARITY_2_INT_POLARITY_2_SHFT                             0x0

#define HWIO_MPM2_MPM_DETECT_CTL_1_ADDR                                       (MPM2_MPM_REG_BASE      + 0x0000009c)
#define HWIO_MPM2_MPM_DETECT_CTL_1_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x0000009c)
#define HWIO_MPM2_MPM_DETECT_CTL_1_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x0000009c)
#define HWIO_MPM2_MPM_DETECT_CTL_1_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_1_ADDR, HWIO_MPM2_MPM_DETECT_CTL_1_RMSK)
#define HWIO_MPM2_MPM_DETECT_CTL_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_1_ADDR, m)
#define HWIO_MPM2_MPM_DETECT_CTL_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DETECT_CTL_1_ADDR,v)
#define HWIO_MPM2_MPM_DETECT_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DETECT_CTL_1_ADDR,m,v,HWIO_MPM2_MPM_DETECT_CTL_1_IN)
#define HWIO_MPM2_MPM_DETECT_CTL_1_DETECT_CTL_1_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_1_DETECT_CTL_1_SHFT                                 0x0

#define HWIO_MPM2_MPM_DETECT_CTL_2_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000a0)
#define HWIO_MPM2_MPM_DETECT_CTL_2_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x000000a0)
#define HWIO_MPM2_MPM_DETECT_CTL_2_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x000000a0)
#define HWIO_MPM2_MPM_DETECT_CTL_2_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_2_ADDR, HWIO_MPM2_MPM_DETECT_CTL_2_RMSK)
#define HWIO_MPM2_MPM_DETECT_CTL_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_2_ADDR, m)
#define HWIO_MPM2_MPM_DETECT_CTL_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DETECT_CTL_2_ADDR,v)
#define HWIO_MPM2_MPM_DETECT_CTL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DETECT_CTL_2_ADDR,m,v,HWIO_MPM2_MPM_DETECT_CTL_2_IN)
#define HWIO_MPM2_MPM_DETECT_CTL_2_DETECT_CTL_2_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_2_DETECT_CTL_2_SHFT                                 0x0

#define HWIO_MPM2_MPM_DETECT_CTL_3_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000a4)
#define HWIO_MPM2_MPM_DETECT_CTL_3_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x000000a4)
#define HWIO_MPM2_MPM_DETECT_CTL_3_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x000000a4)
#define HWIO_MPM2_MPM_DETECT_CTL_3_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_3_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_3_ADDR, HWIO_MPM2_MPM_DETECT_CTL_3_RMSK)
#define HWIO_MPM2_MPM_DETECT_CTL_3_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_3_ADDR, m)
#define HWIO_MPM2_MPM_DETECT_CTL_3_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DETECT_CTL_3_ADDR,v)
#define HWIO_MPM2_MPM_DETECT_CTL_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DETECT_CTL_3_ADDR,m,v,HWIO_MPM2_MPM_DETECT_CTL_3_IN)
#define HWIO_MPM2_MPM_DETECT_CTL_3_DETECT_CTL_3_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_3_DETECT_CTL_3_SHFT                                 0x0

#define HWIO_MPM2_MPM_DETECT_CTL_4_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000a8)
#define HWIO_MPM2_MPM_DETECT_CTL_4_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x000000a8)
#define HWIO_MPM2_MPM_DETECT_CTL_4_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x000000a8)
#define HWIO_MPM2_MPM_DETECT_CTL_4_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_4_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_4_ADDR, HWIO_MPM2_MPM_DETECT_CTL_4_RMSK)
#define HWIO_MPM2_MPM_DETECT_CTL_4_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DETECT_CTL_4_ADDR, m)
#define HWIO_MPM2_MPM_DETECT_CTL_4_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DETECT_CTL_4_ADDR,v)
#define HWIO_MPM2_MPM_DETECT_CTL_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DETECT_CTL_4_ADDR,m,v,HWIO_MPM2_MPM_DETECT_CTL_4_IN)
#define HWIO_MPM2_MPM_DETECT_CTL_4_DETECT_CTL_4_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_DETECT_CTL_4_DETECT_CTL_4_SHFT                                 0x0

#define HWIO_MPM2_MPM_INT_STATUS_1_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000ac)
#define HWIO_MPM2_MPM_INT_STATUS_1_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x000000ac)
#define HWIO_MPM2_MPM_INT_STATUS_1_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x000000ac)
#define HWIO_MPM2_MPM_INT_STATUS_1_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_INT_STATUS_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_STATUS_1_ADDR, HWIO_MPM2_MPM_INT_STATUS_1_RMSK)
#define HWIO_MPM2_MPM_INT_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_STATUS_1_ADDR, m)
#define HWIO_MPM2_MPM_INT_STATUS_1_INT_STATUS_1_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_INT_STATUS_1_INT_STATUS_1_SHFT                                 0x0

#define HWIO_MPM2_MPM_INT_STATUS_2_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000b0)
#define HWIO_MPM2_MPM_INT_STATUS_2_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x000000b0)
#define HWIO_MPM2_MPM_INT_STATUS_2_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x000000b0)
#define HWIO_MPM2_MPM_INT_STATUS_2_RMSK                                       0xffffffff
#define HWIO_MPM2_MPM_INT_STATUS_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_STATUS_2_ADDR, HWIO_MPM2_MPM_INT_STATUS_2_RMSK)
#define HWIO_MPM2_MPM_INT_STATUS_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_STATUS_2_ADDR, m)
#define HWIO_MPM2_MPM_INT_STATUS_2_INT_STATUS_2_BMSK                          0xffffffff
#define HWIO_MPM2_MPM_INT_STATUS_2_INT_STATUS_2_SHFT                                 0x0

#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR                                 (MPM2_MPM_REG_BASE      + 0x000000b4)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x000000b4)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x000000b4)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR, HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_RMSK)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR, m)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR,v)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_ADDR,m,v,HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_IN)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_MPM_INT_WKUP_CLK_SEL_1_BMSK          0xffffffff
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_1_MPM_INT_WKUP_CLK_SEL_1_SHFT                 0x0

#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR                                 (MPM2_MPM_REG_BASE      + 0x000000b8)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_PHYS                                 (MPM2_MPM_REG_BASE_PHYS + 0x000000b8)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_OFFS                                 (MPM2_MPM_REG_BASE_OFFS + 0x000000b8)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_RMSK                                 0xffffffff
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_IN          \
        in_dword_masked(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR, HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_RMSK)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR, m)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR,v)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_ADDR,m,v,HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_IN)
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_MPM_INT_WKUP_CLK_SEL_2_BMSK          0xffffffff
#define HWIO_MPM2_MPM_INT_WKUP_CLK_SEL_2_MPM_INT_WKUP_CLK_SEL_2_SHFT                 0x0

#define HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR                                       (MPM2_MPM_REG_BASE      + 0x000000bc)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_PHYS                                       (MPM2_MPM_REG_BASE_PHYS + 0x000000bc)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_OFFS                                       (MPM2_MPM_REG_BASE_OFFS + 0x000000bc)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_RMSK                                             0x1f
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR, HWIO_MPM2_MPM_DEBUG_BUS_EN_RMSK)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR, m)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR,v)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DEBUG_BUS_EN_ADDR,m,v,HWIO_MPM2_MPM_DEBUG_BUS_EN_IN)
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_QTMR_TEST_BUS_EN_BMSK                            0x10
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_QTMR_TEST_BUS_EN_SHFT                             0x4
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_TEST_BUS_S_EN_BMSK                                0x8
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_TEST_BUS_S_EN_SHFT                                0x3
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_TEST_BUS_M_EN_BMSK                                0x4
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_TEST_BUS_M_EN_SHFT                                0x2
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_DEBUG_EN_1_BMSK                                   0x2
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_DEBUG_EN_1_SHFT                                   0x1
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_DEBUG_EN_BMSK                                     0x1
#define HWIO_MPM2_MPM_DEBUG_BUS_EN_DEBUG_EN_SHFT                                     0x0

#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR                                   (MPM2_MPM_REG_BASE      + 0x000000c0)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_PHYS                                   (MPM2_MPM_REG_BASE_PHYS + 0x000000c0)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_OFFS                                   (MPM2_MPM_REG_BASE_OFFS + 0x000000c0)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_RMSK                                          0xf
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR, HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_RMSK)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR, m)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR,v)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ADDR,m,v,HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_IN)
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ENABLE_BMSK                                   0x8
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_ENABLE_SHFT                                   0x3
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_TIME_BMSK                                     0x7
#define HWIO_MPM2_MPM_PEN_DEBOUNCE_CTL_TIME_SHFT                                     0x0

#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR                                      (MPM2_MPM_REG_BASE      + 0x000000c4)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_PHYS                                      (MPM2_MPM_REG_BASE_PHYS + 0x000000c4)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_OFFS                                      (MPM2_MPM_REG_BASE_OFFS + 0x000000c4)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_RMSK                                             0x3
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_IN          \
        in_dword_masked(HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR, HWIO_MPM2_MPM_WARM_BOOT_CFG_RMSK)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR, m)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR,v)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_WARM_BOOT_CFG_ADDR,m,v,HWIO_MPM2_MPM_WARM_BOOT_CFG_IN)
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ENABLE_EBI2_BMSK                                 0x2
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ENABLE_EBI2_SHFT                                 0x1
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ENABLE_EBI1_BMSK                                 0x1
#define HWIO_MPM2_MPM_WARM_BOOT_CFG_ENABLE_EBI1_SHFT                                 0x0

#define HWIO_MPM2_MPM_REGn_FILE_ADDR(n)                                       (MPM2_MPM_REG_BASE      + 0x000000c8 + 0x4 * (n))
#define HWIO_MPM2_MPM_REGn_FILE_PHYS(n)                                       (MPM2_MPM_REG_BASE_PHYS + 0x000000c8 + 0x4 * (n))
#define HWIO_MPM2_MPM_REGn_FILE_OFFS(n)                                       (MPM2_MPM_REG_BASE_OFFS + 0x000000c8 + 0x4 * (n))
#define HWIO_MPM2_MPM_REGn_FILE_RMSK                                          0xffffffff
#define HWIO_MPM2_MPM_REGn_FILE_MAXn                                                   8
#define HWIO_MPM2_MPM_REGn_FILE_INI(n)        \
        in_dword_masked(HWIO_MPM2_MPM_REGn_FILE_ADDR(n), HWIO_MPM2_MPM_REGn_FILE_RMSK)
#define HWIO_MPM2_MPM_REGn_FILE_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_MPM_REGn_FILE_ADDR(n), mask)
#define HWIO_MPM2_MPM_REGn_FILE_OUTI(n,val)    \
        out_dword(HWIO_MPM2_MPM_REGn_FILE_ADDR(n),val)
#define HWIO_MPM2_MPM_REGn_FILE_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_MPM_REGn_FILE_ADDR(n),mask,val,HWIO_MPM2_MPM_REGn_FILE_INI(n))
#define HWIO_MPM2_MPM_REGn_FILE_DATA_BMSK                                     0xffffffff
#define HWIO_MPM2_MPM_REGn_FILE_DATA_SHFT                                            0x0

#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR                                   (MPM2_MPM_REG_BASE      + 0x000000ec)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_PHYS                                   (MPM2_MPM_REG_BASE_PHYS + 0x000000ec)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_OFFS                                   (MPM2_MPM_REG_BASE_OFFS + 0x000000ec)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_RMSK                                          0x7
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR, HWIO_MPM2_MPM_CNTR_CLK_CONTROL_RMSK)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR, m)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR,v)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CNTR_CLK_CONTROL_ADDR,m,v,HWIO_MPM2_MPM_CNTR_CLK_CONTROL_IN)
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_CLK_STATUS_BMSK                               0x4
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_CLK_STATUS_SHFT                               0x2
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_SW_OVERWRITE_SWITCH_BMSK                      0x2
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_SW_OVERWRITE_SWITCH_SHFT                      0x1
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_SW_CLK_SWITCH_BMSK                            0x1
#define HWIO_MPM2_MPM_CNTR_CLK_CONTROL_SW_CLK_SWITCH_SHFT                            0x0

#define HWIO_MPM2_MPM_CNTR_FRAC_ADDR                                          (MPM2_MPM_REG_BASE      + 0x000000f0)
#define HWIO_MPM2_MPM_CNTR_FRAC_PHYS                                          (MPM2_MPM_REG_BASE_PHYS + 0x000000f0)
#define HWIO_MPM2_MPM_CNTR_FRAC_OFFS                                          (MPM2_MPM_REG_BASE_OFFS + 0x000000f0)
#define HWIO_MPM2_MPM_CNTR_FRAC_RMSK                                              0xffff
#define HWIO_MPM2_MPM_CNTR_FRAC_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_FRAC_ADDR, HWIO_MPM2_MPM_CNTR_FRAC_RMSK)
#define HWIO_MPM2_MPM_CNTR_FRAC_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_FRAC_ADDR, m)
#define HWIO_MPM2_MPM_CNTR_FRAC_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CNTR_FRAC_ADDR,v)
#define HWIO_MPM2_MPM_CNTR_FRAC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CNTR_FRAC_ADDR,m,v,HWIO_MPM2_MPM_CNTR_FRAC_IN)
#define HWIO_MPM2_MPM_CNTR_FRAC_LOAD_VAL_BMSK                                     0xffff
#define HWIO_MPM2_MPM_CNTR_FRAC_LOAD_VAL_SHFT                                        0x0

#define HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR                                     (MPM2_MPM_REG_BASE      + 0x000000f4)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_PHYS                                     (MPM2_MPM_REG_BASE_PHYS + 0x000000f4)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_OFFS                                     (MPM2_MPM_REG_BASE_OFFS + 0x000000f4)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_RMSK                                     0xffffffff
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_IN          \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR, HWIO_MPM2_MPM_CNTR_INCR_DATA_RMSK)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR, m)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR,v)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CNTR_INCR_DATA_ADDR,m,v,HWIO_MPM2_MPM_CNTR_INCR_DATA_IN)
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_MULT_BMSK                                0xffffffff
#define HWIO_MPM2_MPM_CNTR_INCR_DATA_MULT_SHFT                                       0x0

#define HWIO_MPM2_MPM_HW_ID_ADDR                                              (MPM2_MPM_REG_BASE      + 0x000000f8)
#define HWIO_MPM2_MPM_HW_ID_PHYS                                              (MPM2_MPM_REG_BASE_PHYS + 0x000000f8)
#define HWIO_MPM2_MPM_HW_ID_OFFS                                              (MPM2_MPM_REG_BASE_OFFS + 0x000000f8)
#define HWIO_MPM2_MPM_HW_ID_RMSK                                              0xffffffff
#define HWIO_MPM2_MPM_HW_ID_IN          \
        in_dword_masked(HWIO_MPM2_MPM_HW_ID_ADDR, HWIO_MPM2_MPM_HW_ID_RMSK)
#define HWIO_MPM2_MPM_HW_ID_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_HW_ID_ADDR, m)
#define HWIO_MPM2_MPM_HW_ID_MAJOR_BMSK                                        0xf0000000
#define HWIO_MPM2_MPM_HW_ID_MAJOR_SHFT                                              0x1c
#define HWIO_MPM2_MPM_HW_ID_MINOR_BMSK                                         0xfff0000
#define HWIO_MPM2_MPM_HW_ID_MINOR_SHFT                                              0x10
#define HWIO_MPM2_MPM_HW_ID_STEP_BMSK                                             0xffff
#define HWIO_MPM2_MPM_HW_ID_STEP_SHFT                                                0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                                                    (MPM2_MPM_BASE      + 0x0000b000)
#define MPM2_PSHOLD_REG_BASE_PHYS                                               (MPM2_MPM_BASE_PHYS + 0x0000b000)
#define MPM2_PSHOLD_REG_BASE_OFFS                                               0x0000b000

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                                              (MPM2_PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_PHYS                                              (MPM2_PSHOLD_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_OFFS                                              (MPM2_PSHOLD_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_RMSK                                                     0x1
#define HWIO_MPM2_MPM_PS_HOLD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, HWIO_MPM2_MPM_PS_HOLD_RMSK)
#define HWIO_MPM2_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM2_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM2_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM2_MPM_PS_HOLD_IN)
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK                                              0x1
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_SHFT                                              0x0

#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR                                (MPM2_PSHOLD_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_PHYS                                (MPM2_PSHOLD_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_OFFS                                (MPM2_PSHOLD_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_RMSK                                       0x1
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_IN          \
        in_dword_masked(HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR, HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_RMSK)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR, m)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR,v)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR,m,v,HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_IN)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_DDR_PHY_FREEZEIO_EBI1_BMSK                 0x1
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_DDR_PHY_FREEZEIO_EBI1_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_QTIMR_AC
 *--------------------------------------------------------------------------*/

#define MPM2_QTIMR_AC_REG_BASE                                  (MPM2_MPM_BASE      + 0x00004000)
#define MPM2_QTIMR_AC_REG_BASE_PHYS                             (MPM2_MPM_BASE_PHYS + 0x00004000)
#define MPM2_QTIMR_AC_REG_BASE_OFFS                             0x00004000

#define HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR                           (MPM2_QTIMR_AC_REG_BASE      + 0x00000000)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_PHYS                           (MPM2_QTIMR_AC_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_OFFS                           (MPM2_QTIMR_AC_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_RMSK                           0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTFRQ_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR, HWIO_MPM2_QTMR_AC_CNTFRQ_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR, m)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR,v)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR,m,v,HWIO_MPM2_QTMR_AC_CNTFRQ_IN)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_CNTFRQ_BMSK                    0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTFRQ_CNTFRQ_SHFT                           0x0

#define HWIO_MPM2_QTMR_AC_CNTSR_ADDR                            (MPM2_QTIMR_AC_REG_BASE      + 0x00000004)
#define HWIO_MPM2_QTMR_AC_CNTSR_PHYS                            (MPM2_QTIMR_AC_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_QTMR_AC_CNTSR_OFFS                            (MPM2_QTIMR_AC_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_QTMR_AC_CNTSR_RMSK                                   0x1
#define HWIO_MPM2_QTMR_AC_CNTSR_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTSR_ADDR, HWIO_MPM2_QTMR_AC_CNTSR_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTSR_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTSR_ADDR, m)
#define HWIO_MPM2_QTMR_AC_CNTSR_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_AC_CNTSR_ADDR,v)
#define HWIO_MPM2_QTMR_AC_CNTSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTSR_ADDR,m,v,HWIO_MPM2_QTMR_AC_CNTSR_IN)
#define HWIO_MPM2_QTMR_AC_CNTSR_NSN_BMSK                               0x1
#define HWIO_MPM2_QTMR_AC_CNTSR_NSN_SHFT                               0x0
#define HWIO_MPM2_QTMR_AC_CNTSR_NSN_SECURE_ONLY_FVAL                   0x0
#define HWIO_MPM2_QTMR_AC_CNTSR_NSN_SECURE_OR_NONSECURE_FVAL           0x1

#define HWIO_MPM2_QTMR_AC_CNTTID_ADDR                           (MPM2_QTIMR_AC_REG_BASE      + 0x00000008)
#define HWIO_MPM2_QTMR_AC_CNTTID_PHYS                           (MPM2_QTIMR_AC_REG_BASE_PHYS + 0x00000008)
#define HWIO_MPM2_QTMR_AC_CNTTID_OFFS                           (MPM2_QTIMR_AC_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM2_QTMR_AC_CNTTID_RMSK                           0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTTID_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTTID_ADDR, HWIO_MPM2_QTMR_AC_CNTTID_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTTID_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTTID_ADDR, m)
#define HWIO_MPM2_QTMR_AC_CNTTID_F7_CFG_BMSK                    0xf0000000
#define HWIO_MPM2_QTMR_AC_CNTTID_F7_CFG_SHFT                          0x1c
#define HWIO_MPM2_QTMR_AC_CNTTID_F6_CFG_BMSK                     0xf000000
#define HWIO_MPM2_QTMR_AC_CNTTID_F6_CFG_SHFT                          0x18
#define HWIO_MPM2_QTMR_AC_CNTTID_F5_CFG_BMSK                      0xf00000
#define HWIO_MPM2_QTMR_AC_CNTTID_F5_CFG_SHFT                          0x14
#define HWIO_MPM2_QTMR_AC_CNTTID_F4_CFG_BMSK                       0xf0000
#define HWIO_MPM2_QTMR_AC_CNTTID_F4_CFG_SHFT                          0x10
#define HWIO_MPM2_QTMR_AC_CNTTID_F3_CFG_BMSK                        0xf000
#define HWIO_MPM2_QTMR_AC_CNTTID_F3_CFG_SHFT                           0xc
#define HWIO_MPM2_QTMR_AC_CNTTID_F2_CFG_BMSK                         0xf00
#define HWIO_MPM2_QTMR_AC_CNTTID_F2_CFG_SHFT                           0x8
#define HWIO_MPM2_QTMR_AC_CNTTID_F1_CFG_BMSK                          0xf0
#define HWIO_MPM2_QTMR_AC_CNTTID_F1_CFG_SHFT                           0x4
#define HWIO_MPM2_QTMR_AC_CNTTID_F0_CFG_BMSK                           0xf
#define HWIO_MPM2_QTMR_AC_CNTTID_F0_CFG_SHFT                           0x0
#define HWIO_MPM2_QTMR_AC_CNTTID_F0_CFG_FI_FVAL                        0x0
#define HWIO_MPM2_QTMR_AC_CNTTID_F0_CFG_FVI_FVAL                       0x1
#define HWIO_MPM2_QTMR_AC_CNTTID_F0_CFG_FPLO_FVAL                      0x2
#define HWIO_MPM2_QTMR_AC_CNTTID_F0_CFG_RSVD_FVAL                      0x3

#define HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n)                      (MPM2_QTIMR_AC_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_MPM2_QTMR_AC_CNTACR_n_PHYS(n)                      (MPM2_QTIMR_AC_REG_BASE_PHYS + 0x00000040 + 0x4 * (n))
#define HWIO_MPM2_QTMR_AC_CNTACR_n_OFFS(n)                      (MPM2_QTIMR_AC_REG_BASE_OFFS + 0x00000040 + 0x4 * (n))
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RMSK                               0x3f
#define HWIO_MPM2_QTMR_AC_CNTACR_n_MAXn                                  0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_INI(n)        \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n), HWIO_MPM2_QTMR_AC_CNTACR_n_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTACR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n), mask)
#define HWIO_MPM2_QTMR_AC_CNTACR_n_OUTI(n,val)    \
        out_dword(HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n),val)
#define HWIO_MPM2_QTMR_AC_CNTACR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n),mask,val,HWIO_MPM2_QTMR_AC_CNTACR_n_INI(n))
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWPT_BMSK                          0x20
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWPT_SHFT                           0x5
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWPT_ACCESS_DENIED_FVAL             0x0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWPT_ACCESS_ALLOWED_FVAL            0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWVT_BMSK                          0x10
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWVT_SHFT                           0x4
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWVT_ACCESS_DENIED_FVAL             0x0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWVT_ACCESS_ALLOWED_FVAL            0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RVOFF_BMSK                          0x8
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RVOFF_SHFT                          0x3
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RVOFF_ACCESS_DENIED_FVAL            0x0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RVOFF_ACCESS_ALLOWED_FVAL           0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RFRQ_BMSK                           0x4
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RFRQ_SHFT                           0x2
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RFRQ_ACCESS_DENIED_FVAL             0x0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RFRQ_ACCESS_ALLOWED_FVAL            0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPVCT_BMSK                          0x2
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPVCT_SHFT                          0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPVCT_ACCESS_DENIED_FVAL            0x0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPVCT_ACCESS_ALLOWED_FVAL           0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPCT_BMSK                           0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPCT_SHFT                           0x0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPCT_ACCESS_DENIED_FVAL             0x0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPCT_ACCESS_ALLOWED_FVAL            0x1

#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n)                  (MPM2_QTIMR_AC_REG_BASE      + 0x00000080 + 0x8 * (n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_PHYS(n)                  (MPM2_QTIMR_AC_REG_BASE_PHYS + 0x00000080 + 0x8 * (n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_OFFS(n)                  (MPM2_QTIMR_AC_REG_BASE_OFFS + 0x00000080 + 0x8 * (n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_RMSK                     0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_MAXn                              0
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_INI(n)        \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n), HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n), mask)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_OUTI(n,val)    \
        out_dword(HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n),val)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n),mask,val,HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_INI(n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_BMSK          0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_SHFT                 0x0

#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n)                  (MPM2_QTIMR_AC_REG_BASE      + 0x00000084 + 0x8 * (n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_PHYS(n)                  (MPM2_QTIMR_AC_REG_BASE_PHYS + 0x00000084 + 0x8 * (n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_OFFS(n)                  (MPM2_QTIMR_AC_REG_BASE_OFFS + 0x00000084 + 0x8 * (n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_RMSK                       0xffffff
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_MAXn                              0
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_INI(n)        \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n), HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n), mask)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_OUTI(n,val)    \
        out_dword(HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n),val)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n),mask,val,HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_INI(n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_BMSK            0xffffff
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_SHFT                 0x0

#define HWIO_MPM2_QTMR_AC_CFG_ADDR                              (MPM2_QTIMR_AC_REG_BASE      + 0x00000fc0)
#define HWIO_MPM2_QTMR_AC_CFG_PHYS                              (MPM2_QTIMR_AC_REG_BASE_PHYS + 0x00000fc0)
#define HWIO_MPM2_QTMR_AC_CFG_OFFS                              (MPM2_QTIMR_AC_REG_BASE_OFFS + 0x00000fc0)
#define HWIO_MPM2_QTMR_AC_CFG_RMSK                                     0x1
#define HWIO_MPM2_QTMR_AC_CFG_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CFG_ADDR, HWIO_MPM2_QTMR_AC_CFG_RMSK)
#define HWIO_MPM2_QTMR_AC_CFG_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CFG_ADDR, m)
#define HWIO_MPM2_QTMR_AC_CFG_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_AC_CFG_ADDR,v)
#define HWIO_MPM2_QTMR_AC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CFG_ADDR,m,v,HWIO_MPM2_QTMR_AC_CFG_IN)
#define HWIO_MPM2_QTMR_AC_CFG_TEST_BUS_EN_BMSK                         0x1
#define HWIO_MPM2_QTMR_AC_CFG_TEST_BUS_EN_SHFT                         0x0

#define HWIO_MPM2_QTMR_AC_VERSION_ADDR                          (MPM2_QTIMR_AC_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_QTMR_AC_VERSION_PHYS                          (MPM2_QTIMR_AC_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_MPM2_QTMR_AC_VERSION_OFFS                          (MPM2_QTIMR_AC_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_MPM2_QTMR_AC_VERSION_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_AC_VERSION_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_AC_VERSION_ADDR, HWIO_MPM2_QTMR_AC_VERSION_RMSK)
#define HWIO_MPM2_QTMR_AC_VERSION_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_VERSION_ADDR, m)
#define HWIO_MPM2_QTMR_AC_VERSION_MAJOR_BMSK                    0xf0000000
#define HWIO_MPM2_QTMR_AC_VERSION_MAJOR_SHFT                          0x1c
#define HWIO_MPM2_QTMR_AC_VERSION_MINOR_BMSK                     0xfff0000
#define HWIO_MPM2_QTMR_AC_VERSION_MINOR_SHFT                          0x10
#define HWIO_MPM2_QTMR_AC_VERSION_STEP_BMSK                         0xffff
#define HWIO_MPM2_QTMR_AC_VERSION_STEP_SHFT                            0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_QTIMR_V1
 *--------------------------------------------------------------------------*/

#define MPM2_QTIMR_V1_REG_BASE                                    (MPM2_MPM_BASE      + 0x00005000)
#define MPM2_QTIMR_V1_REG_BASE_PHYS                               (MPM2_MPM_BASE_PHYS + 0x00005000)
#define MPM2_QTIMR_V1_REG_BASE_OFFS                               0x00005000

#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000000)
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_PHYS                          (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_OFFS                          (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPCT_LO_ADDR, HWIO_MPM2_QTMR_V1_CNTPCT_LO_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPCT_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000004)
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_PHYS                          (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_OFFS                          (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_RMSK                            0xffffff
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPCT_HI_ADDR, HWIO_MPM2_QTMR_V1_CNTPCT_HI_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPCT_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000008)
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_PHYS                          (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000008)
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_OFFS                          (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVCT_LO_ADDR, HWIO_MPM2_QTMR_V1_CNTVCT_LO_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVCT_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_PHYS                          (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x0000000c)
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_OFFS                          (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x0000000c)
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_RMSK                            0xffffff
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVCT_HI_ADDR, HWIO_MPM2_QTMR_V1_CNTVCT_HI_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVCT_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTFRQ_ADDR                             (MPM2_QTIMR_V1_REG_BASE      + 0x00000010)
#define HWIO_MPM2_QTMR_V1_CNTFRQ_PHYS                             (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000010)
#define HWIO_MPM2_QTMR_V1_CNTFRQ_OFFS                             (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000010)
#define HWIO_MPM2_QTMR_V1_CNTFRQ_RMSK                             0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTFRQ_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTFRQ_ADDR, HWIO_MPM2_QTMR_V1_CNTFRQ_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTFRQ_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000014)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PHYS                          (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000014)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_OFFS                          (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000014)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_RMSK                               0x303
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR, HWIO_MPM2_QTMR_V1_CNTPL0ACR_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTPL0ACR_IN)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0CTEN_BMSK                       0x200
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0CTEN_SHFT                         0x9
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0VTEN_BMSK                       0x100
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0VTEN_SHFT                         0x8
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0VCTEN_BMSK                        0x2
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0VCTEN_SHFT                        0x1
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0PCTEN_BMSK                        0x1
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0PCTEN_SHFT                        0x0

#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_ADDR                         (MPM2_QTIMR_V1_REG_BASE      + 0x00000018)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_PHYS                         (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000018)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_OFFS                         (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000018)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVOFF_LO_ADDR, HWIO_MPM2_QTMR_V1_CNTVOFF_LO_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVOFF_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_ADDR                         (MPM2_QTIMR_V1_REG_BASE      + 0x0000001c)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_PHYS                         (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x0000001c)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_OFFS                         (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x0000001c)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_RMSK                           0xffffff
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVOFF_HI_ADDR, HWIO_MPM2_QTMR_V1_CNTVOFF_HI_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVOFF_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR                       (MPM2_QTIMR_V1_REG_BASE      + 0x00000020)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_PHYS                       (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000020)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_OFFS                       (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000020)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR, HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_IN)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR                       (MPM2_QTIMR_V1_REG_BASE      + 0x00000024)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_PHYS                       (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000024)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_OFFS                       (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000024)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_RMSK                         0xffffff
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR, HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_IN)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000028)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_PHYS                          (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000028)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_OFFS                          (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000028)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR, HWIO_MPM2_QTMR_V1_CNTP_TVAL_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTP_TVAL_IN)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR                           (MPM2_QTIMR_V1_REG_BASE      + 0x0000002c)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_PHYS                           (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x0000002c)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_OFFS                           (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x0000002c)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_RMSK                                  0x7
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR, HWIO_MPM2_QTMR_V1_CNTP_CTL_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTP_CTL_IN)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_ISTAT_INTERRUPT_NOT_PENDING_FVAL        0x0
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_ISTAT_INTERRUPT_PENDING_FVAL          0x1
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_IMSK_UNMASK_INTERRUPT_FVAL            0x0
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_IMSK_MASK_INTERRUPT_FVAL              0x1
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_EN_SHFT                               0x0
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_EN_DISABLED_FVAL                      0x0
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_EN_ENABLED_FVAL                       0x1

#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR                       (MPM2_QTIMR_V1_REG_BASE      + 0x00000030)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_PHYS                       (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000030)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_OFFS                       (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000030)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR, HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_IN)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR                       (MPM2_QTIMR_V1_REG_BASE      + 0x00000034)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_PHYS                       (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000034)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_OFFS                       (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000034)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_RMSK                         0xffffff
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR, HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_IN)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000038)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_PHYS                          (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000038)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_OFFS                          (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000038)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR, HWIO_MPM2_QTMR_V1_CNTV_TVAL_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTV_TVAL_IN)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR                           (MPM2_QTIMR_V1_REG_BASE      + 0x0000003c)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_PHYS                           (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x0000003c)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_OFFS                           (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x0000003c)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_RMSK                                  0x7
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR, HWIO_MPM2_QTMR_V1_CNTV_CTL_RMSK)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTV_CTL_IN)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_ISTAT_INTERRUPT_NOT_PENDING_FVAL        0x0
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_ISTAT_INTERRUPT_PENDING_FVAL          0x1
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_IMSK_UNMASK_INTERRUPT_FVAL            0x0
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_IMSK_MASK_INTERRUPT_FVAL              0x1
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_EN_SHFT                               0x0
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_EN_DISABLED_FVAL                      0x0
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_EN_ENABLED_FVAL                       0x1

#define HWIO_MPM2_QTMR_V1_VERSION_ADDR                            (MPM2_QTIMR_V1_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_QTMR_V1_VERSION_PHYS                            (MPM2_QTIMR_V1_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_MPM2_QTMR_V1_VERSION_OFFS                            (MPM2_QTIMR_V1_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_MPM2_QTMR_V1_VERSION_RMSK                            0xffffffff
#define HWIO_MPM2_QTMR_V1_VERSION_IN          \
        in_dword_masked(HWIO_MPM2_QTMR_V1_VERSION_ADDR, HWIO_MPM2_QTMR_V1_VERSION_RMSK)
#define HWIO_MPM2_QTMR_V1_VERSION_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_VERSION_ADDR, m)
#define HWIO_MPM2_QTMR_V1_VERSION_MAJOR_BMSK                      0xf0000000
#define HWIO_MPM2_QTMR_V1_VERSION_MAJOR_SHFT                            0x1c
#define HWIO_MPM2_QTMR_V1_VERSION_MINOR_BMSK                       0xfff0000
#define HWIO_MPM2_QTMR_V1_VERSION_MINOR_SHFT                            0x10
#define HWIO_MPM2_QTMR_V1_VERSION_STEP_BMSK                           0xffff
#define HWIO_MPM2_QTMR_V1_VERSION_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_SLP_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_SLP_CNTR_REG_BASE                                    (MPM2_MPM_BASE      + 0x00003000)
#define MPM2_SLP_CNTR_REG_BASE_PHYS                               (MPM2_MPM_BASE_PHYS + 0x00003000)
#define MPM2_SLP_CNTR_REG_BASE_OFFS                               0x00003000

#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR               (MPM2_SLP_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_PHYS               (MPM2_SLP_CNTR_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_OFFS               (MPM2_SLP_CNTR_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK               0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, m)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_BMSK          0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSENS
 *--------------------------------------------------------------------------*/

#define MPM2_TSENS_REG_BASE                                                             (MPM2_MPM_BASE      + 0x00008000)
#define MPM2_TSENS_REG_BASE_PHYS                                                        (MPM2_MPM_BASE_PHYS + 0x00008000)
#define MPM2_TSENS_REG_BASE_OFFS                                                        0x00008000

#define HWIO_MPM2_TSENS_HW_VER_ADDR                                                     (MPM2_TSENS_REG_BASE      + 0x00000000)
#define HWIO_MPM2_TSENS_HW_VER_PHYS                                                     (MPM2_TSENS_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_TSENS_HW_VER_OFFS                                                     (MPM2_TSENS_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_TSENS_HW_VER_RMSK                                                     0xffffffff
#define HWIO_MPM2_TSENS_HW_VER_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_HW_VER_ADDR, HWIO_MPM2_TSENS_HW_VER_RMSK)
#define HWIO_MPM2_TSENS_HW_VER_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_HW_VER_ADDR, m)
#define HWIO_MPM2_TSENS_HW_VER_MAJOR_BMSK                                               0xf0000000
#define HWIO_MPM2_TSENS_HW_VER_MAJOR_SHFT                                                     0x1c
#define HWIO_MPM2_TSENS_HW_VER_MINOR_BMSK                                                0xfff0000
#define HWIO_MPM2_TSENS_HW_VER_MINOR_SHFT                                                     0x10
#define HWIO_MPM2_TSENS_HW_VER_STEP_BMSK                                                    0xffff
#define HWIO_MPM2_TSENS_HW_VER_STEP_SHFT                                                       0x0

#define HWIO_MPM2_TSENS_CTRL_ADDR                                                       (MPM2_TSENS_REG_BASE      + 0x00000004)
#define HWIO_MPM2_TSENS_CTRL_PHYS                                                       (MPM2_TSENS_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_TSENS_CTRL_OFFS                                                       (MPM2_TSENS_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_TSENS_CTRL_RMSK                                                       0xfff83fff
#define HWIO_MPM2_TSENS_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CTRL_ADDR, HWIO_MPM2_TSENS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_CTRL_IN)
#define HWIO_MPM2_TSENS_CTRL_TSENS_MTC_EN_BMSK                                          0x80000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_MTC_EN_SHFT                                                0x1f
#define HWIO_MPM2_TSENS_CTRL_TSENS_MTC_EN_DISABLED_FVAL                                        0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_MTC_EN_ENABLED_FVAL                                         0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_CPU_HIGH_LOW_INT_EN_BMSK                             0x40000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_CPU_HIGH_LOW_INT_EN_SHFT                                   0x1e
#define HWIO_MPM2_TSENS_CTRL_TSENS_CPU_HIGH_LOW_INT_EN_DISABLED_FVAL                           0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_CPU_HIGH_LOW_INT_EN_ENABLED_FVAL                            0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_AUTO_ADJUST_PERIOD_EN_BMSK                           0x20000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_AUTO_ADJUST_PERIOD_EN_SHFT                                 0x1d
#define HWIO_MPM2_TSENS_CTRL_TSENS_AUTO_ADJUST_PERIOD_EN_DISABLED_FVAL                         0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_AUTO_ADJUST_PERIOD_EN_ENABLED_FVAL                          0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_BMSK                                           0x10000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_SHFT                                                 0x1c
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_UNCLAMPED_FVAL                                        0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_CLAMP_CLAMPED_FVAL                                          0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_BMSK                                        0x8000000
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_SHFT                                             0x1b
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_DISABLED_FVAL                                     0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_BYPASS_EN_ENABLED_FVAL                                      0x1
#define HWIO_MPM2_TSENS_CTRL_MEASURE_PERIOD_BMSK                                         0x7f80000
#define HWIO_MPM2_TSENS_CTRL_MEASURE_PERIOD_SHFT                                              0x13
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_BMSK                                               0x2000
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_SHFT                                                  0xd
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_DISABLED_FVAL                                         0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR10_EN_ENABLED_FVAL                                          0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_BMSK                                                0x1000
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_SHFT                                                   0xc
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR9_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_BMSK                                                 0x800
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_SHFT                                                   0xb
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR8_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_BMSK                                                 0x400
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_SHFT                                                   0xa
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR7_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_BMSK                                                 0x200
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_SHFT                                                   0x9
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR6_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_BMSK                                                 0x100
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_SHFT                                                   0x8
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR5_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_BMSK                                                  0x80
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_SHFT                                                   0x7
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR4_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_BMSK                                                  0x40
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_SHFT                                                   0x6
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR3_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_BMSK                                                  0x20
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_SHFT                                                   0x5
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR2_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_BMSK                                                  0x10
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_SHFT                                                   0x4
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR1_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_BMSK                                                   0x8
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_SHFT                                                   0x3
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_DISABLED_FVAL                                          0x0
#define HWIO_MPM2_TSENS_CTRL_SENSOR0_EN_ENABLED_FVAL                                           0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_BMSK                                            0x4
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_SHFT                                            0x2
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_INTERNAL_OSCILLATOR_FVAL                        0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_ADC_CLK_SEL_EXTERNAL_CLOCK_SOURCE_FVAL                      0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_BMSK                                                 0x2
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_SHFT                                                 0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_RESET_DEASSERTED_FVAL                                0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_SW_RST_RESET_ASSERTED_FVAL                                  0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_BMSK                                                     0x1
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_SHFT                                                     0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_DISABLED_FVAL                                            0x0
#define HWIO_MPM2_TSENS_CTRL_TSENS_EN_ENABLED_FVAL                                             0x1

#define HWIO_MPM2_TSENS_TEST_CTRL_ADDR                                                  (MPM2_TSENS_REG_BASE      + 0x00000008)
#define HWIO_MPM2_TSENS_TEST_CTRL_PHYS                                                  (MPM2_TSENS_REG_BASE_PHYS + 0x00000008)
#define HWIO_MPM2_TSENS_TEST_CTRL_OFFS                                                  (MPM2_TSENS_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM2_TSENS_TEST_CTRL_RMSK                                                        0x1f
#define HWIO_MPM2_TSENS_TEST_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_TEST_CTRL_ADDR, HWIO_MPM2_TSENS_TEST_CTRL_RMSK)
#define HWIO_MPM2_TSENS_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_TEST_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_TEST_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_TEST_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_TEST_CTRL_IN)
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_BMSK                                         0x1e
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_SEL_SHFT                                          0x1
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_BMSK                                           0x1
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_SHFT                                           0x0
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_DISABLED_FVAL                             0x0
#define HWIO_MPM2_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_ENABLED_FVAL                              0x1

#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n)                                  (MPM2_TSENS_REG_BASE      + 0x0000000c + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_PHYS(n)                                  (MPM2_TSENS_REG_BASE_PHYS + 0x0000000c + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_OFFS(n)                                  (MPM2_TSENS_REG_BASE_OFFS + 0x0000000c + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_RMSK                                       0x3fffff
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAXn                                             10
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n), HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n),val)
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_ADDR(n),mask,val,HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_INI(n))
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_BMSK                       0x200000
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_SHFT                           0x15
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_NORMAL_OPERATION_FVAL           0x0
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_STATUS_MASK_MASK_OFF_MAX_STATUS_FVAL        0x1
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_BMSK                       0x100000
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_SHFT                           0x14
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_NORMAL_OPERATION_FVAL           0x0
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_STATUS_MASK_MASK_OFF_MIN_STATUS_FVAL        0x1
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_THRESHOLD_BMSK                          0xffc00
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MAX_THRESHOLD_SHFT                              0xa
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_THRESHOLD_BMSK                            0x3ff
#define HWIO_MPM2_TSENS_Sn_MIN_MAX_STATUS_CTRL_MIN_THRESHOLD_SHFT                              0x0

#define HWIO_MPM2_TS_CONTROL_ADDR                                                       (MPM2_TSENS_REG_BASE      + 0x0000004c)
#define HWIO_MPM2_TS_CONTROL_PHYS                                                       (MPM2_TSENS_REG_BASE_PHYS + 0x0000004c)
#define HWIO_MPM2_TS_CONTROL_OFFS                                                       (MPM2_TSENS_REG_BASE_OFFS + 0x0000004c)
#define HWIO_MPM2_TS_CONTROL_RMSK                                                        0x1ffffff
#define HWIO_MPM2_TS_CONTROL_IN          \
        in_dword_masked(HWIO_MPM2_TS_CONTROL_ADDR, HWIO_MPM2_TS_CONTROL_RMSK)
#define HWIO_MPM2_TS_CONTROL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TS_CONTROL_ADDR, m)
#define HWIO_MPM2_TS_CONTROL_OUT(v)      \
        out_dword(HWIO_MPM2_TS_CONTROL_ADDR,v)
#define HWIO_MPM2_TS_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TS_CONTROL_ADDR,m,v,HWIO_MPM2_TS_CONTROL_IN)
#define HWIO_MPM2_TS_CONTROL_TS_CONTROL_BMSK                                             0x1ffffff
#define HWIO_MPM2_TS_CONTROL_TS_CONTROL_SHFT                                                   0x0

#define HWIO_MPM2_TS_CONFIG_ADDR                                                        (MPM2_TSENS_REG_BASE      + 0x00000050)
#define HWIO_MPM2_TS_CONFIG_PHYS                                                        (MPM2_TSENS_REG_BASE_PHYS + 0x00000050)
#define HWIO_MPM2_TS_CONFIG_OFFS                                                        (MPM2_TSENS_REG_BASE_OFFS + 0x00000050)
#define HWIO_MPM2_TS_CONFIG_RMSK                                                              0xff
#define HWIO_MPM2_TS_CONFIG_IN          \
        in_dword_masked(HWIO_MPM2_TS_CONFIG_ADDR, HWIO_MPM2_TS_CONFIG_RMSK)
#define HWIO_MPM2_TS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_MPM2_TS_CONFIG_ADDR, m)
#define HWIO_MPM2_TS_CONFIG_OUT(v)      \
        out_dword(HWIO_MPM2_TS_CONFIG_ADDR,v)
#define HWIO_MPM2_TS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TS_CONFIG_ADDR,m,v,HWIO_MPM2_TS_CONFIG_IN)
#define HWIO_MPM2_TS_CONFIG_TS_CONFIG_BMSK                                                    0xff
#define HWIO_MPM2_TS_CONFIG_TS_CONFIG_SHFT                                                     0x0

#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR                                              (MPM2_TSENS_REG_BASE      + 0x00000054)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_PHYS                                              (MPM2_TSENS_REG_BASE_PHYS + 0x00000054)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_OFFS                                              (MPM2_TSENS_REG_BASE_OFFS + 0x00000054)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_RMSK                                                   0x1ff
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_IN          \
        in_dword_masked(HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR, HWIO_MPM2_TS_CONFIG_REMOTE_IN_RMSK)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_INM(m)      \
        in_dword_masked(HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR, m)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_OUT(v)      \
        out_dword(HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR,v)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TS_CONFIG_REMOTE_IN_ADDR,m,v,HWIO_MPM2_TS_CONFIG_REMOTE_IN_IN)
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_TS_CONFIG_REMOTE_IN_BMSK                               0x1ff
#define HWIO_MPM2_TS_CONFIG_REMOTE_IN_TS_CONFIG_REMOTE_IN_SHFT                                 0x0

#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR                                   (MPM2_TSENS_REG_BASE      + 0x00000058)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_PHYS                                   (MPM2_TSENS_REG_BASE_PHYS + 0x00000058)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_OFFS                                   (MPM2_TSENS_REG_BASE_OFFS + 0x00000058)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_RMSK                                         0xff
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR, HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_RMSK)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR, m)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR,v)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_ADDR,m,v,HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_IN)
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_POWERDOWN_MEASURE_PERIOD_BMSK                0xff
#define HWIO_MPM2_TSENS_POWERDOWN_MEASURE_PERIOD_POWERDOWN_MEASURE_PERIOD_SHFT                 0x0

#define HWIO_MPM2_TSENS_CPU_INDEXES_ADDR                                                (MPM2_TSENS_REG_BASE      + 0x0000005c)
#define HWIO_MPM2_TSENS_CPU_INDEXES_PHYS                                                (MPM2_TSENS_REG_BASE_PHYS + 0x0000005c)
#define HWIO_MPM2_TSENS_CPU_INDEXES_OFFS                                                (MPM2_TSENS_REG_BASE_OFFS + 0x0000005c)
#define HWIO_MPM2_TSENS_CPU_INDEXES_RMSK                                                0xffffffff
#define HWIO_MPM2_TSENS_CPU_INDEXES_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_INDEXES_ADDR, HWIO_MPM2_TSENS_CPU_INDEXES_RMSK)
#define HWIO_MPM2_TSENS_CPU_INDEXES_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_INDEXES_ADDR, m)
#define HWIO_MPM2_TSENS_CPU_INDEXES_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_CPU_INDEXES_ADDR,v)
#define HWIO_MPM2_TSENS_CPU_INDEXES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CPU_INDEXES_ADDR,m,v,HWIO_MPM2_TSENS_CPU_INDEXES_IN)
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU7_INDEX_BMSK                                     0xf0000000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU7_INDEX_SHFT                                           0x1c
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU6_INDEX_BMSK                                      0xf000000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU6_INDEX_SHFT                                           0x18
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU5_INDEX_BMSK                                       0xf00000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU5_INDEX_SHFT                                           0x14
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU4_INDEX_BMSK                                        0xf0000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU4_INDEX_SHFT                                           0x10
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU3_INDEX_BMSK                                         0xf000
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU3_INDEX_SHFT                                            0xc
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU2_INDEX_BMSK                                          0xf00
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU2_INDEX_SHFT                                            0x8
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU1_INDEX_BMSK                                           0xf0
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU1_INDEX_SHFT                                            0x4
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU0_INDEX_BMSK                                            0xf
#define HWIO_MPM2_TSENS_CPU_INDEXES_CPU0_INDEX_SHFT                                            0x0

#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n)                                          (MPM2_TSENS_REG_BASE      + 0x00000060 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PHYS(n)                                          (MPM2_TSENS_REG_BASE_PHYS + 0x00000060 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_OFFS(n)                                          (MPM2_TSENS_REG_BASE_OFFS + 0x00000060 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_RMSK                                             0xffff07ff
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_MAXn                                                      3
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n), HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_RMSK)
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n), mask)
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n),val)
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ADDR(n),mask,val,HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_INI(n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ZONE_MTC_EN_BMSK                                 0x80000000
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_ZONE_MTC_EN_SHFT                                       0x1f
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_TH2_VIOLATED_BMSK                     0x7c000000
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_TH2_VIOLATED_SHFT                           0x1a
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_TH1_VIOLATED_BMSK                      0x3e00000
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_TH1_VIOLATED_SHFT                           0x15
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_COOL_BMSK                               0x1f0000
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_PS_COMMAND_COOL_SHFT                                   0x10
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_SENSOR_MAP_BMSK                                       0x7ff
#define HWIO_MPM2_TSENS_MTC_ZONEn_CTRL_SENSOR_MAP_SHFT                                         0x0

#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n)                                       (MPM2_TSENS_REG_BASE      + 0x00000070 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_PHYS(n)                                       (MPM2_TSENS_REG_BASE_PHYS + 0x00000070 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_OFFS(n)                                       (MPM2_TSENS_REG_BASE_OFFS + 0x00000070 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_RMSK                                             0xfffff
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_MAXn                                                  10
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n), HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_RMSK)
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n), mask)
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n),val)
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_ADDR(n),mask,val,HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_INI(n))
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_TH2_BMSK                                         0xffc00
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_TH2_SHFT                                             0xa
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_TH1_BMSK                                           0x3ff
#define HWIO_MPM2_TSENS_MTC_Sn_THRESHOLDS_TH1_SHFT                                             0x0

#define HWIO_MPM2_TSENS_MTC_MARGIN_ADDR                                                 (MPM2_TSENS_REG_BASE      + 0x0000009c)
#define HWIO_MPM2_TSENS_MTC_MARGIN_PHYS                                                 (MPM2_TSENS_REG_BASE_PHYS + 0x0000009c)
#define HWIO_MPM2_TSENS_MTC_MARGIN_OFFS                                                 (MPM2_TSENS_REG_BASE_OFFS + 0x0000009c)
#define HWIO_MPM2_TSENS_MTC_MARGIN_RMSK                                                      0x3ff
#define HWIO_MPM2_TSENS_MTC_MARGIN_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_MARGIN_ADDR, HWIO_MPM2_TSENS_MTC_MARGIN_RMSK)
#define HWIO_MPM2_TSENS_MTC_MARGIN_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_MARGIN_ADDR, m)
#define HWIO_MPM2_TSENS_MTC_MARGIN_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_MTC_MARGIN_ADDR,v)
#define HWIO_MPM2_TSENS_MTC_MARGIN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_MARGIN_ADDR,m,v,HWIO_MPM2_TSENS_MTC_MARGIN_IN)
#define HWIO_MPM2_TSENS_MTC_MARGIN_TH2_MARGIN_VALUE_BMSK                                     0x3e0
#define HWIO_MPM2_TSENS_MTC_MARGIN_TH2_MARGIN_VALUE_SHFT                                       0x5
#define HWIO_MPM2_TSENS_MTC_MARGIN_TH1_MARGIN_VALUE_BMSK                                      0x1f
#define HWIO_MPM2_TSENS_MTC_MARGIN_TH1_MARGIN_VALUE_SHFT                                       0x0

#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR                                             (MPM2_TSENS_REG_BASE      + 0x000000a0)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_PHYS                                             (MPM2_TSENS_REG_BASE_PHYS + 0x000000a0)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_OFFS                                             (MPM2_TSENS_REG_BASE_OFFS + 0x000000a0)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_RMSK                                                   0x1f
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR, HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_IN)
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_DEBUG_BUS_SEL_BMSK                                     0x1e
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_DEBUG_BUS_SEL_SHFT                                      0x1
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_DEBUG_BUS_EN_BMSK                                       0x1
#define HWIO_MPM2_TSENS_DEBUG_BUS_CTRL_DEBUG_BUS_EN_SHFT                                       0x0

#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_ADDR                                      (MPM2_TSENS_REG_BASE      + 0x000000a4)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_PHYS                                      (MPM2_TSENS_REG_BASE_PHYS + 0x000000a4)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_OFFS                                      (MPM2_TSENS_REG_BASE_OFFS + 0x000000a4)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_RMSK                                      0xffffffff
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_ADDR, HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_RMSK)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_ADDR, m)
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_DEBUG_BUS_VALUE_BMSK                      0xffffffff
#define HWIO_MPM2_TSENS_DEBUG_BUS_MIRROR_READ_DEBUG_BUS_VALUE_SHFT                             0x0

#define HWIO_MPM2_TSENS_SROT_SPARE_ADDR                                                 (MPM2_TSENS_REG_BASE      + 0x000000a8)
#define HWIO_MPM2_TSENS_SROT_SPARE_PHYS                                                 (MPM2_TSENS_REG_BASE_PHYS + 0x000000a8)
#define HWIO_MPM2_TSENS_SROT_SPARE_OFFS                                                 (MPM2_TSENS_REG_BASE_OFFS + 0x000000a8)
#define HWIO_MPM2_TSENS_SROT_SPARE_RMSK                                                 0xffffffff
#define HWIO_MPM2_TSENS_SROT_SPARE_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_SROT_SPARE_ADDR, HWIO_MPM2_TSENS_SROT_SPARE_RMSK)
#define HWIO_MPM2_TSENS_SROT_SPARE_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_SROT_SPARE_ADDR, m)
#define HWIO_MPM2_TSENS_SROT_SPARE_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_SROT_SPARE_ADDR,v)
#define HWIO_MPM2_TSENS_SROT_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_SROT_SPARE_ADDR,m,v,HWIO_MPM2_TSENS_SROT_SPARE_IN)
#define HWIO_MPM2_TSENS_SROT_SPARE_VALUE_BMSK                                           0xffffffff
#define HWIO_MPM2_TSENS_SROT_SPARE_VALUE_SHFT                                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSENS_TM
 *--------------------------------------------------------------------------*/

#define MPM2_TSENS_TM_REG_BASE                                                                    (MPM2_MPM_BASE      + 0x00009000)
#define MPM2_TSENS_TM_REG_BASE_PHYS                                                               (MPM2_MPM_BASE_PHYS + 0x00009000)
#define MPM2_TSENS_TM_REG_BASE_OFFS                                                               0x00009000

#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR                                           (MPM2_TSENS_TM_REG_BASE      + 0x00000000)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_PHYS                                           (MPM2_TSENS_TM_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_OFFS                                           (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_RMSK                                                  0x1
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR, HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_RMSK)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR, m)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR,v)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_ADDR,m,v,HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_IN)
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_BMSK                                     0x1
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_SHFT                                     0x0
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_DISABLED_FVAL                            0x0
#define HWIO_MPM2_TSENS_UPPER_LOWER_INTERRUPT_CTRL_INTERRUPT_EN_ENABLED_FVAL                             0x1

#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n)                                        (MPM2_TSENS_TM_REG_BASE      + 0x00000004 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_PHYS(n)                                        (MPM2_TSENS_TM_REG_BASE_PHYS + 0x00000004 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_OFFS(n)                                        (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000004 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_RMSK                                             0x3fffff
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_MAXn                                                   10
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n), HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_RMSK)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n),val)
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_ADDR(n),mask,val,HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_INI(n))
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_BMSK                            0x200000
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_SHFT                                0x15
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_NORMAL_OPERATION_FVAL                0x0
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_STATUS_CLR_CLEAR_UPPER_STATUS_FVAL              0x1
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_BMSK                            0x100000
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_SHFT                                0x14
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_NORMAL_OPERATION_FVAL                0x0
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_STATUS_CLR_CLEAR_LOWER_STATUS_FVAL              0x1
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_THRESHOLD_BMSK                              0xffc00
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_UPPER_THRESHOLD_SHFT                                  0xa
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_THRESHOLD_BMSK                                0x3ff
#define HWIO_MPM2_TSENS_Sn_UPPER_LOWER_STATUS_CTRL_LOWER_THRESHOLD_SHFT                                  0x0

#define HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n)                                                         (MPM2_TSENS_TM_REG_BASE      + 0x00000044 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_STATUS_PHYS(n)                                                         (MPM2_TSENS_TM_REG_BASE_PHYS + 0x00000044 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_STATUS_OFFS(n)                                                         (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000044 + 0x4 * (n))
#define HWIO_MPM2_TSENS_Sn_STATUS_RMSK                                                                0x7fff
#define HWIO_MPM2_TSENS_Sn_STATUS_MAXn                                                                    10
#define HWIO_MPM2_TSENS_Sn_STATUS_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n), HWIO_MPM2_TSENS_Sn_STATUS_RMSK)
#define HWIO_MPM2_TSENS_Sn_STATUS_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_Sn_STATUS_ADDR(n), mask)
#define HWIO_MPM2_TSENS_Sn_STATUS_VALID_BMSK                                                          0x4000
#define HWIO_MPM2_TSENS_Sn_STATUS_VALID_SHFT                                                             0xe
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_BMSK                                                     0x2000
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_SHFT                                                        0xd
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_NOT_VIOLATED_FVAL                             0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_VIOLATED_FVAL                                 0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_BMSK                                                   0x1000
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_SHFT                                                      0xc
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_NOT_VIOLATED_FVAL                         0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_VIOLATED_FVAL                             0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_BMSK                                                    0x800
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_SHFT                                                      0xb
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_NOT_VIOLATED_FVAL                         0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_VIOLATED_FVAL                             0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_BMSK                                                      0x400
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_SHFT                                                        0xa
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_NOT_VIOLATED_FVAL                             0x0
#define HWIO_MPM2_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_VIOLATED_FVAL                                 0x1
#define HWIO_MPM2_TSENS_Sn_STATUS_LAST_TEMP_BMSK                                                       0x3ff
#define HWIO_MPM2_TSENS_Sn_STATUS_LAST_TEMP_SHFT                                                         0x0

#define HWIO_MPM2_TSENS_TRDY_ADDR                                                                 (MPM2_TSENS_TM_REG_BASE      + 0x00000084)
#define HWIO_MPM2_TSENS_TRDY_PHYS                                                                 (MPM2_TSENS_TM_REG_BASE_PHYS + 0x00000084)
#define HWIO_MPM2_TSENS_TRDY_OFFS                                                                 (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000084)
#define HWIO_MPM2_TSENS_TRDY_RMSK                                                                        0x7
#define HWIO_MPM2_TSENS_TRDY_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_TRDY_ADDR, HWIO_MPM2_TSENS_TRDY_RMSK)
#define HWIO_MPM2_TSENS_TRDY_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_TRDY_ADDR, m)
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_BMSK                                                            0x4
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_SHFT                                                            0x2
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_ON_FVAL                                                  0x0
#define HWIO_MPM2_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_OFF_FVAL                                                 0x1
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_BMSK                                                            0x2
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_SHFT                                                            0x1
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_ON_FVAL                                                  0x0
#define HWIO_MPM2_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_OFF_FVAL                                                 0x1
#define HWIO_MPM2_TSENS_TRDY_TRDY_BMSK                                                                   0x1
#define HWIO_MPM2_TSENS_TRDY_TRDY_SHFT                                                                   0x0
#define HWIO_MPM2_TSENS_TRDY_TRDY_TEMPERATURE_MEASUREMENT_IN_PROGRESS_FVAL                               0x0
#define HWIO_MPM2_TSENS_TRDY_TRDY_TEMPERATURE_READING_IS_READY_FVAL                                      0x1

#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_ADDR                                        (MPM2_TSENS_TM_REG_BASE      + 0x00000088)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_PHYS                                        (MPM2_TSENS_TM_REG_BASE_PHYS + 0x00000088)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_OFFS                                        (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000088)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_RMSK                                            0xffff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_ADDR, HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_RMSK)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_ADDR, m)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_CPU_HIGH_INT_BMSK                               0xff00
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_CPU_HIGH_INT_SHFT                                  0x8
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_CPU_LOW_INT_BMSK                                  0xff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_STATUS_CPU_LOW_INT_SHFT                                   0x0

#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR                                      (MPM2_TSENS_TM_REG_BASE      + 0x0000008c)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_PHYS                                      (MPM2_TSENS_TM_REG_BASE_PHYS + 0x0000008c)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_OFFS                                      (MPM2_TSENS_TM_REG_BASE_OFFS + 0x0000008c)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_RMSK                                         0x3ffff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR, HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_RMSK)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR, m)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR,v)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_ADDR,m,v,HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_IN)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_HIGH_OVERRIDE_EN_BMSK                    0x20000
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_HIGH_OVERRIDE_EN_SHFT                       0x11
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_LOW_OVERRIDE_EN_BMSK                     0x10000
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_LOW_OVERRIDE_EN_SHFT                        0x10
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_HIGH_INT_OVERRIDE_VALUE_BMSK              0xff00
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_HIGH_INT_OVERRIDE_VALUE_SHFT                 0x8
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_LOW_INT_OVERRIDE_VALUE_BMSK                 0xff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_INTERRUPT_OVERRIDE_CPU_LOW_INT_OVERRIDE_VALUE_SHFT                  0x0

#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n)                                      (MPM2_TSENS_TM_REG_BASE      + 0x00000090 + 0x4 * (n))
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_PHYS(n)                                      (MPM2_TSENS_TM_REG_BASE_PHYS + 0x00000090 + 0x4 * (n))
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_OFFS(n)                                      (MPM2_TSENS_TM_REG_BASE_OFFS + 0x00000090 + 0x4 * (n))
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_RMSK                                           0x3fffff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_MAXn                                                  7
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n), HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_RMSK)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n), mask)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n),val)
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_ADDR(n),mask,val,HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_INI(n))
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_HIGH_INT_MASK_BMSK                         0x200000
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_HIGH_INT_MASK_SHFT                             0x15
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_LOW_INT_MASK_BMSK                          0x100000
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_LOW_INT_MASK_SHFT                              0x14
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_HIGH_THRESHOLD_BMSK                         0xffc00
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_HIGH_THRESHOLD_SHFT                             0xa
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_LOW_THRESHOLD_BMSK                            0x3ff
#define HWIO_MPM2_TSENS_CPU_HIGH_LOW_n_INT_THRESHOLD_CPU_LOW_THRESHOLD_SHFT                              0x0

#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n)                                                 (MPM2_TSENS_TM_REG_BASE      + 0x000000c0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_PHYS(n)                                                 (MPM2_TSENS_TM_REG_BASE_PHYS + 0x000000c0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_OFFS(n)                                                 (MPM2_TSENS_TM_REG_BASE_OFFS + 0x000000c0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_RMSK                                                           0x3
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_MAXn                                                             3
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n), HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_RMSK)
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n), mask)
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n),val)
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_ADDR(n),mask,val,HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_INI(n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_TH2_MTC_IN_EFFECT_BMSK                                         0x2
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_TH2_MTC_IN_EFFECT_SHFT                                         0x1
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_TH1_MTC_IN_EFFECT_BMSK                                         0x1
#define HWIO_MPM2_TSENS_MTC_ZONEn_SW_MASK_TH1_MTC_IN_EFFECT_SHFT                                         0x0

#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_ADDR(n)                                                     (MPM2_TSENS_TM_REG_BASE      + 0x000000d0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PHYS(n)                                                     (MPM2_TSENS_TM_REG_BASE_PHYS + 0x000000d0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_OFFS(n)                                                     (MPM2_TSENS_TM_REG_BASE_OFFS + 0x000000d0 + 0x4 * (n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_RMSK                                                        0x7fffffff
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_MAXn                                                                 3
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_INI(n)        \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_LOG_ADDR(n), HWIO_MPM2_TSENS_MTC_ZONEn_LOG_RMSK)
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_TSENS_MTC_ZONEn_LOG_ADDR(n), mask)
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_OUTI(n,val)    \
        out_dword(HWIO_MPM2_TSENS_MTC_ZONEn_LOG_ADDR(n),val)
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_MTC_ZONEn_LOG_ADDR(n),mask,val,HWIO_MPM2_TSENS_MTC_ZONEn_LOG_INI(n))
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOGS_VALID_BMSK                                  0x40000000
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOGS_VALID_SHFT                                        0x1e
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG5_BMSK                                        0x3e000000
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG5_SHFT                                              0x19
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG4_BMSK                                         0x1f00000
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG4_SHFT                                              0x14
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG3_BMSK                                           0xf8000
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG3_SHFT                                               0xf
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG2_BMSK                                            0x7c00
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG2_SHFT                                               0xa
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG1_BMSK                                             0x3e0
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LOG1_SHFT                                               0x5
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LATEST_BMSK                                            0x1f
#define HWIO_MPM2_TSENS_MTC_ZONEn_LOG_PS_COMMAND_LATEST_SHFT                                             0x0

#define HWIO_MPM2_TSENS_TM_SPARE_ADDR                                                             (MPM2_TSENS_TM_REG_BASE      + 0x000000e0)
#define HWIO_MPM2_TSENS_TM_SPARE_PHYS                                                             (MPM2_TSENS_TM_REG_BASE_PHYS + 0x000000e0)
#define HWIO_MPM2_TSENS_TM_SPARE_OFFS                                                             (MPM2_TSENS_TM_REG_BASE_OFFS + 0x000000e0)
#define HWIO_MPM2_TSENS_TM_SPARE_RMSK                                                             0xffffffff
#define HWIO_MPM2_TSENS_TM_SPARE_IN          \
        in_dword_masked(HWIO_MPM2_TSENS_TM_SPARE_ADDR, HWIO_MPM2_TSENS_TM_SPARE_RMSK)
#define HWIO_MPM2_TSENS_TM_SPARE_INM(m)      \
        in_dword_masked(HWIO_MPM2_TSENS_TM_SPARE_ADDR, m)
#define HWIO_MPM2_TSENS_TM_SPARE_OUT(v)      \
        out_dword(HWIO_MPM2_TSENS_TM_SPARE_ADDR,v)
#define HWIO_MPM2_TSENS_TM_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_TSENS_TM_SPARE_ADDR,m,v,HWIO_MPM2_TSENS_TM_SPARE_IN)
#define HWIO_MPM2_TSENS_TM_SPARE_VALUE_BMSK                                                       0xffffffff
#define HWIO_MPM2_TSENS_TM_SPARE_VALUE_SHFT                                                              0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSYNC
 *--------------------------------------------------------------------------*/

#define MPM2_TSYNC_REG_BASE                                  (MPM2_MPM_BASE      + 0x00006000)
#define MPM2_TSYNC_REG_BASE_PHYS                             (MPM2_MPM_BASE_PHYS + 0x00006000)
#define MPM2_TSYNC_REG_BASE_OFFS                             0x00006000

#define HWIO_MPM2_MPM_TSYNC_CTL_ADDR                         (MPM2_TSYNC_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_TSYNC_CTL_PHYS                         (MPM2_TSYNC_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_MPM_TSYNC_CTL_OFFS                         (MPM2_TSYNC_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_TSYNC_CTL_RMSK                              0x7ff
#define HWIO_MPM2_MPM_TSYNC_CTL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_CTL_ADDR, HWIO_MPM2_MPM_TSYNC_CTL_RMSK)
#define HWIO_MPM2_MPM_TSYNC_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_CTL_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_CTL_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_CTL_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_CTL_IN)
#define HWIO_MPM2_MPM_TSYNC_CTL_WIDTH_BMSK                        0x7fc
#define HWIO_MPM2_MPM_TSYNC_CTL_WIDTH_SHFT                          0x2
#define HWIO_MPM2_MPM_TSYNC_CTL_IRQ_BMSK                            0x2
#define HWIO_MPM2_MPM_TSYNC_CTL_IRQ_SHFT                            0x1
#define HWIO_MPM2_MPM_TSYNC_CTL_IRQ_DISABLE_FVAL                    0x0
#define HWIO_MPM2_MPM_TSYNC_CTL_IRQ_ENABLE_FVAL                     0x1
#define HWIO_MPM2_MPM_TSYNC_CTL_ENABLE_BMSK                         0x1
#define HWIO_MPM2_MPM_TSYNC_CTL_ENABLE_SHFT                         0x0
#define HWIO_MPM2_MPM_TSYNC_CTL_ENABLE_DISABLE_FVAL                 0x0
#define HWIO_MPM2_MPM_TSYNC_CTL_ENABLE_ENABLE_FVAL                  0x1

#define HWIO_MPM2_MPM_TSYNC_CMD_ADDR                         (MPM2_TSYNC_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_TSYNC_CMD_PHYS                         (MPM2_TSYNC_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_MPM_TSYNC_CMD_OFFS                         (MPM2_TSYNC_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_MPM_TSYNC_CMD_RMSK                                0x1
#define HWIO_MPM2_MPM_TSYNC_CMD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_CMD_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_CMD_START_BMSK                          0x1
#define HWIO_MPM2_MPM_TSYNC_CMD_START_SHFT                          0x0
#define HWIO_MPM2_MPM_TSYNC_CMD_START_NO_ACTION_FVAL                0x0
#define HWIO_MPM2_MPM_TSYNC_CMD_START_ARM_FVAL                      0x1

#define HWIO_MPM2_MPM_TSYNC_START_ADDR                       (MPM2_TSYNC_REG_BASE      + 0x00000008)
#define HWIO_MPM2_MPM_TSYNC_START_PHYS                       (MPM2_TSYNC_REG_BASE_PHYS + 0x00000008)
#define HWIO_MPM2_MPM_TSYNC_START_OFFS                       (MPM2_TSYNC_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM2_MPM_TSYNC_START_RMSK                        0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_START_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_ADDR, HWIO_MPM2_MPM_TSYNC_START_RMSK)
#define HWIO_MPM2_MPM_TSYNC_START_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_START_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_START_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_START_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_START_IN)
#define HWIO_MPM2_MPM_TSYNC_START_TIME_BMSK                   0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_START_TIME_SHFT                         0x0

#define HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR                 (MPM2_TSYNC_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_PHYS                 (MPM2_TSYNC_REG_BASE_PHYS + 0x0000000c)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_OFFS                 (MPM2_TSYNC_REG_BASE_OFFS + 0x0000000c)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_RMSK                       0xff
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_START_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_START_PHASE_IN)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_PHASE_BMSK                 0xff
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_PHASE_SHFT                  0x0

#define HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR                      (MPM2_TSYNC_REG_BASE      + 0x00000010)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHYS                      (MPM2_TSYNC_REG_BASE_PHYS + 0x00000010)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_OFFS                      (MPM2_TSYNC_REG_BASE_OFFS + 0x00000010)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_RMSK                       0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR, HWIO_MPM2_MPM_TSYNC_PERIOD_RMSK)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_PERIOD_IN)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PERIOD_BMSK                0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PERIOD_SHFT                      0x0

#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR                (MPM2_TSYNC_REG_BASE      + 0x00000014)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_PHYS                (MPM2_TSYNC_REG_BASE_PHYS + 0x00000014)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_OFFS                (MPM2_TSYNC_REG_BASE_OFFS + 0x00000014)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_RMSK                      0xff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_IN)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_PHASE_BMSK                0xff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_PHASE_SHFT                 0x0

#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR                 (MPM2_TSYNC_REG_BASE      + 0x00000018)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_PHYS                 (MPM2_TSYNC_REG_BASE_PHYS + 0x00000018)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_OFFS                 (MPM2_TSYNC_REG_BASE_OFFS + 0x00000018)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_RMSK                  0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR, HWIO_MPM2_MPM_TSYNC_STATUS_TIME_RMSK)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_TIME_BMSK             0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_TIME_SHFT                   0x0

#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR                (MPM2_TSYNC_REG_BASE      + 0x0000001c)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_PHYS                (MPM2_TSYNC_REG_BASE_PHYS + 0x0000001c)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_OFFS                (MPM2_TSYNC_REG_BASE_OFFS + 0x0000001c)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_RMSK                      0xff
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_PHASE_BMSK                0xff
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_PHASE_SHFT                 0x0

#define HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR                       (MPM2_TSYNC_REG_BASE      + 0x00000020)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_PHYS                       (MPM2_TSYNC_REG_BASE_PHYS + 0x00000020)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_OFFS                       (MPM2_TSYNC_REG_BASE_OFFS + 0x00000020)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_RMSK                              0x1
#define HWIO_MPM2_MPM_GRFC_CXO_EN_IN          \
        in_dword_masked(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR, HWIO_MPM2_MPM_GRFC_CXO_EN_RMSK)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR, m)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR,v)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR,m,v,HWIO_MPM2_MPM_GRFC_CXO_EN_IN)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_MUX_SEL_BMSK                      0x1
#define HWIO_MPM2_MPM_GRFC_CXO_EN_MUX_SEL_SHFT                      0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_WDOG
 *--------------------------------------------------------------------------*/

#define MPM2_WDOG_REG_BASE                                      (MPM2_MPM_BASE      + 0x0000a000)
#define MPM2_WDOG_REG_BASE_PHYS                                 (MPM2_MPM_BASE_PHYS + 0x0000a000)
#define MPM2_WDOG_REG_BASE_OFFS                                 0x0000a000

#define HWIO_MPM2_WDOG_RESET_REG_ADDR                           (MPM2_WDOG_REG_BASE      + 0x00000000)
#define HWIO_MPM2_WDOG_RESET_REG_PHYS                           (MPM2_WDOG_REG_BASE_PHYS + 0x00000000)
#define HWIO_MPM2_WDOG_RESET_REG_OFFS                           (MPM2_WDOG_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_WDOG_RESET_REG_RMSK                                  0x1
#define HWIO_MPM2_WDOG_RESET_REG_OUT(v)      \
        out_dword(HWIO_MPM2_WDOG_RESET_REG_ADDR,v)
#define HWIO_MPM2_WDOG_RESET_REG_WDOG_RESET_BMSK                       0x1
#define HWIO_MPM2_WDOG_RESET_REG_WDOG_RESET_SHFT                       0x0

#define HWIO_MPM2_WDOG_CTL_REG_ADDR                             (MPM2_WDOG_REG_BASE      + 0x00000004)
#define HWIO_MPM2_WDOG_CTL_REG_PHYS                             (MPM2_WDOG_REG_BASE_PHYS + 0x00000004)
#define HWIO_MPM2_WDOG_CTL_REG_OFFS                             (MPM2_WDOG_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM2_WDOG_CTL_REG_RMSK                             0x80000007
#define HWIO_MPM2_WDOG_CTL_REG_IN          \
        in_dword_masked(HWIO_MPM2_WDOG_CTL_REG_ADDR, HWIO_MPM2_WDOG_CTL_REG_RMSK)
#define HWIO_MPM2_WDOG_CTL_REG_INM(m)      \
        in_dword_masked(HWIO_MPM2_WDOG_CTL_REG_ADDR, m)
#define HWIO_MPM2_WDOG_CTL_REG_OUT(v)      \
        out_dword(HWIO_MPM2_WDOG_CTL_REG_ADDR,v)
#define HWIO_MPM2_WDOG_CTL_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_WDOG_CTL_REG_ADDR,m,v,HWIO_MPM2_WDOG_CTL_REG_IN)
#define HWIO_MPM2_WDOG_CTL_REG_WDOG_CLK_EN_BMSK                 0x80000000
#define HWIO_MPM2_WDOG_CTL_REG_WDOG_CLK_EN_SHFT                       0x1f
#define HWIO_MPM2_WDOG_CTL_REG_WDOG_CLK_EN_DISABLE_FVAL                0x0
#define HWIO_MPM2_WDOG_CTL_REG_WDOG_CLK_EN_ENABLE_FVAL                 0x1
#define HWIO_MPM2_WDOG_CTL_REG_CHIP_AUTOPET_EN_BMSK                    0x4
#define HWIO_MPM2_WDOG_CTL_REG_CHIP_AUTOPET_EN_SHFT                    0x2
#define HWIO_MPM2_WDOG_CTL_REG_CHIP_AUTOPET_EN_DISABLE_FVAL            0x0
#define HWIO_MPM2_WDOG_CTL_REG_CHIP_AUTOPET_EN_ENABLE_FVAL             0x1
#define HWIO_MPM2_WDOG_CTL_REG_HW_SLEEP_WAKEUP_EN_BMSK                 0x2
#define HWIO_MPM2_WDOG_CTL_REG_HW_SLEEP_WAKEUP_EN_SHFT                 0x1
#define HWIO_MPM2_WDOG_CTL_REG_HW_SLEEP_WAKEUP_EN_DISABLE_FVAL         0x0
#define HWIO_MPM2_WDOG_CTL_REG_HW_SLEEP_WAKEUP_EN_ENABLE_FVAL          0x1
#define HWIO_MPM2_WDOG_CTL_REG_WDOG_EN_BMSK                            0x1
#define HWIO_MPM2_WDOG_CTL_REG_WDOG_EN_SHFT                            0x0
#define HWIO_MPM2_WDOG_CTL_REG_WDOG_EN_DISABLE_FVAL                    0x0
#define HWIO_MPM2_WDOG_CTL_REG_WDOG_EN_ENABLE_FVAL                     0x1

#define HWIO_MPM2_WDOG_STATUS_REG_ADDR                          (MPM2_WDOG_REG_BASE      + 0x00000008)
#define HWIO_MPM2_WDOG_STATUS_REG_PHYS                          (MPM2_WDOG_REG_BASE_PHYS + 0x00000008)
#define HWIO_MPM2_WDOG_STATUS_REG_OFFS                          (MPM2_WDOG_REG_BASE_OFFS + 0x00000008)
#define HWIO_MPM2_WDOG_STATUS_REG_RMSK                             0xfffff
#define HWIO_MPM2_WDOG_STATUS_REG_IN          \
        in_dword_masked(HWIO_MPM2_WDOG_STATUS_REG_ADDR, HWIO_MPM2_WDOG_STATUS_REG_RMSK)
#define HWIO_MPM2_WDOG_STATUS_REG_INM(m)      \
        in_dword_masked(HWIO_MPM2_WDOG_STATUS_REG_ADDR, m)
#define HWIO_MPM2_WDOG_STATUS_REG_WDOG_CNT_BMSK                    0xfffff
#define HWIO_MPM2_WDOG_STATUS_REG_WDOG_CNT_SHFT                        0x0

#define HWIO_MPM2_WDOG_BARK_VAL_REG_ADDR                        (MPM2_WDOG_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_WDOG_BARK_VAL_REG_PHYS                        (MPM2_WDOG_REG_BASE_PHYS + 0x0000000c)
#define HWIO_MPM2_WDOG_BARK_VAL_REG_OFFS                        (MPM2_WDOG_REG_BASE_OFFS + 0x0000000c)
#define HWIO_MPM2_WDOG_BARK_VAL_REG_RMSK                        0x800fffff
#define HWIO_MPM2_WDOG_BARK_VAL_REG_IN          \
        in_dword_masked(HWIO_MPM2_WDOG_BARK_VAL_REG_ADDR, HWIO_MPM2_WDOG_BARK_VAL_REG_RMSK)
#define HWIO_MPM2_WDOG_BARK_VAL_REG_INM(m)      \
        in_dword_masked(HWIO_MPM2_WDOG_BARK_VAL_REG_ADDR, m)
#define HWIO_MPM2_WDOG_BARK_VAL_REG_OUT(v)      \
        out_dword(HWIO_MPM2_WDOG_BARK_VAL_REG_ADDR,v)
#define HWIO_MPM2_WDOG_BARK_VAL_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_WDOG_BARK_VAL_REG_ADDR,m,v,HWIO_MPM2_WDOG_BARK_VAL_REG_IN)
#define HWIO_MPM2_WDOG_BARK_VAL_REG_SYNC_STATUS_BMSK            0x80000000
#define HWIO_MPM2_WDOG_BARK_VAL_REG_SYNC_STATUS_SHFT                  0x1f
#define HWIO_MPM2_WDOG_BARK_VAL_REG_WDOG_BARK_VAL_BMSK             0xfffff
#define HWIO_MPM2_WDOG_BARK_VAL_REG_WDOG_BARK_VAL_SHFT                 0x0

#define HWIO_MPM2_WDOG_BITE_VAL_REG_ADDR                        (MPM2_WDOG_REG_BASE      + 0x00000010)
#define HWIO_MPM2_WDOG_BITE_VAL_REG_PHYS                        (MPM2_WDOG_REG_BASE_PHYS + 0x00000010)
#define HWIO_MPM2_WDOG_BITE_VAL_REG_OFFS                        (MPM2_WDOG_REG_BASE_OFFS + 0x00000010)
#define HWIO_MPM2_WDOG_BITE_VAL_REG_RMSK                        0x800fffff
#define HWIO_MPM2_WDOG_BITE_VAL_REG_IN          \
        in_dword_masked(HWIO_MPM2_WDOG_BITE_VAL_REG_ADDR, HWIO_MPM2_WDOG_BITE_VAL_REG_RMSK)
#define HWIO_MPM2_WDOG_BITE_VAL_REG_INM(m)      \
        in_dword_masked(HWIO_MPM2_WDOG_BITE_VAL_REG_ADDR, m)
#define HWIO_MPM2_WDOG_BITE_VAL_REG_OUT(v)      \
        out_dword(HWIO_MPM2_WDOG_BITE_VAL_REG_ADDR,v)
#define HWIO_MPM2_WDOG_BITE_VAL_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_WDOG_BITE_VAL_REG_ADDR,m,v,HWIO_MPM2_WDOG_BITE_VAL_REG_IN)
#define HWIO_MPM2_WDOG_BITE_VAL_REG_SYNC_STATUS_BMSK            0x80000000
#define HWIO_MPM2_WDOG_BITE_VAL_REG_SYNC_STATUS_SHFT                  0x1f
#define HWIO_MPM2_WDOG_BITE_VAL_REG_WDOG_BITE_VAL_BMSK             0xfffff
#define HWIO_MPM2_WDOG_BITE_VAL_REG_WDOG_BITE_VAL_SHFT                 0x0


#endif /* __MPM_HWIO_H__ */
