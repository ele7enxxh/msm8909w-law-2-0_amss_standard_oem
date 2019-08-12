#ifndef __MBA_TIMER_H__
#define __MBA_TIMER_H__
/*
===========================================================================
*/
/**
  @file mba_timer.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    QDSP6SS_QTMR_F0_0
    QDSP6SS_QTMR_F1_1
    QDSP6SS_QTMR_AC

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/inc/mba_timer.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: QDSP6SS_QTMR_AC
 *--------------------------------------------------------------------------*/

#define QDSP6SS_QTMR_AC_REG_BASE                                   (MSS_TOP_BASE      + 0x00120000)

#define HWIO_QDSP6SS_QTMR_AC_CNTFRQ_ADDR                           (QDSP6SS_QTMR_AC_REG_BASE      + 0x00000000)
#define HWIO_QDSP6SS_QTMR_AC_CNTFRQ_RMSK                           0xffffffff
#define HWIO_QDSP6SS_QTMR_AC_CNTFRQ_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTFRQ_ADDR, HWIO_QDSP6SS_QTMR_AC_CNTFRQ_RMSK)
#define HWIO_QDSP6SS_QTMR_AC_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTFRQ_ADDR, m)
#define HWIO_QDSP6SS_QTMR_AC_CNTFRQ_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_AC_CNTFRQ_ADDR,v)
#define HWIO_QDSP6SS_QTMR_AC_CNTFRQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_AC_CNTFRQ_ADDR,m,v,HWIO_QDSP6SS_QTMR_AC_CNTFRQ_IN)
#define HWIO_QDSP6SS_QTMR_AC_CNTFRQ_CNTFRQ_BMSK                    0xffffffff
#define HWIO_QDSP6SS_QTMR_AC_CNTFRQ_CNTFRQ_SHFT                           0x0

#define HWIO_QDSP6SS_QTMR_AC_CNTSR_ADDR                            (QDSP6SS_QTMR_AC_REG_BASE      + 0x00000004)
#define HWIO_QDSP6SS_QTMR_AC_CNTSR_RMSK                                   0x3
#define HWIO_QDSP6SS_QTMR_AC_CNTSR_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTSR_ADDR, HWIO_QDSP6SS_QTMR_AC_CNTSR_RMSK)
#define HWIO_QDSP6SS_QTMR_AC_CNTSR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTSR_ADDR, m)
#define HWIO_QDSP6SS_QTMR_AC_CNTSR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_AC_CNTSR_ADDR,v)
#define HWIO_QDSP6SS_QTMR_AC_CNTSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_AC_CNTSR_ADDR,m,v,HWIO_QDSP6SS_QTMR_AC_CNTSR_IN)
#define HWIO_QDSP6SS_QTMR_AC_CNTSR_NSN_BMSK                               0x3
#define HWIO_QDSP6SS_QTMR_AC_CNTSR_NSN_SHFT                               0x0

#define HWIO_QDSP6SS_QTMR_AC_CNTTID_ADDR                           (QDSP6SS_QTMR_AC_REG_BASE      + 0x00000008)
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_RMSK                           0xffffffff
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTTID_ADDR, HWIO_QDSP6SS_QTMR_AC_CNTTID_RMSK)
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTTID_ADDR, m)
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F7_CFG_BMSK                    0xf0000000
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F7_CFG_SHFT                          0x1c
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F6_CFG_BMSK                     0xf000000
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F6_CFG_SHFT                          0x18
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F5_CFG_BMSK                      0xf00000
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F5_CFG_SHFT                          0x14
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F4_CFG_BMSK                       0xf0000
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F4_CFG_SHFT                          0x10
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F3_CFG_BMSK                        0xf000
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F3_CFG_SHFT                           0xc
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F2_CFG_BMSK                         0xf00
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F2_CFG_SHFT                           0x8
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F1_CFG_BMSK                          0xf0
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F1_CFG_SHFT                           0x4
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F0_CFG_BMSK                           0xf
#define HWIO_QDSP6SS_QTMR_AC_CNTTID_F0_CFG_SHFT                           0x0

#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n)                      (QDSP6SS_QTMR_AC_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RMSK                               0x3f
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_MAXn                                  1
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_INI(n)        \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n), HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RMSK)
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n), mask)
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_OUTI(n,val)    \
        out_dword(HWIO_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n),val)
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n),mask,val,HWIO_QDSP6SS_QTMR_AC_CNTACR_n_INI(n))
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RWPT_BMSK                          0x20
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RWPT_SHFT                           0x5
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RWVT_BMSK                          0x10
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RWVT_SHFT                           0x4
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RVOFF_BMSK                          0x8
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RVOFF_SHFT                          0x3
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RFRQ_BMSK                           0x4
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RFRQ_SHFT                           0x2
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RPVCT_BMSK                          0x2
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RPVCT_SHFT                          0x1
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RPCT_BMSK                           0x1
#define HWIO_QDSP6SS_QTMR_AC_CNTACR_n_RPCT_SHFT                           0x0

#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_ADDR(n)                  (QDSP6SS_QTMR_AC_REG_BASE      + 0x00000080 + 0x8 * (n))
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_RMSK                     0xffffffff
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_MAXn                              1
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_INI(n)        \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_ADDR(n), HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_RMSK)
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_INMI(n,mask)    \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_ADDR(n), mask)
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_OUTI(n,val)    \
        out_dword(HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_ADDR(n),val)
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_ADDR(n),mask,val,HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_INI(n))
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_BMSK          0xffffffff
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_ADDR(n)                  (QDSP6SS_QTMR_AC_REG_BASE      + 0x00000084 + 0x8 * (n))
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_RMSK                       0xffffff
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_MAXn                              1
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_INI(n)        \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_ADDR(n), HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_RMSK)
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_INMI(n,mask)    \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_ADDR(n), mask)
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_OUTI(n,val)    \
        out_dword(HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_ADDR(n),val)
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_ADDR(n),mask,val,HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_INI(n))
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_BMSK            0xffffff
#define HWIO_QDSP6SS_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_AC_VERSION_ADDR                          (QDSP6SS_QTMR_AC_REG_BASE      + 0x00000fd0)
#define HWIO_QDSP6SS_QTMR_AC_VERSION_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_AC_VERSION_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_VERSION_ADDR, HWIO_QDSP6SS_QTMR_AC_VERSION_RMSK)
#define HWIO_QDSP6SS_QTMR_AC_VERSION_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_AC_VERSION_ADDR, m)
#define HWIO_QDSP6SS_QTMR_AC_VERSION_MAJOR_BMSK                    0xf0000000
#define HWIO_QDSP6SS_QTMR_AC_VERSION_MAJOR_SHFT                          0x1c
#define HWIO_QDSP6SS_QTMR_AC_VERSION_MINOR_BMSK                     0xfff0000
#define HWIO_QDSP6SS_QTMR_AC_VERSION_MINOR_SHFT                          0x10
#define HWIO_QDSP6SS_QTMR_AC_VERSION_STEP_BMSK                         0xffff
#define HWIO_QDSP6SS_QTMR_AC_VERSION_STEP_SHFT                            0x0

/*----------------------------------------------------------------------------
 * MODULE: QDSP6SS_QTMR_F0_0
 *--------------------------------------------------------------------------*/

#define QDSP6SS_QTMR_F0_0_REG_BASE                                     (MSS_TOP_BASE      + 0x00121000)

#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_ADDR                          (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000000)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_CNTPCT_LO_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_ADDR                          (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000004)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_RMSK                            0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_0_CNTPCT_HI_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_ADDR                          (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000008)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_0_CNTVCT_LO_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_ADDR                          (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000000c)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_RMSK                            0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_0_CNTVCT_HI_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_ADDR                             (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000010)
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_RMSK                             0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_CNTFRQ_BMSK                      0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_0_CNTFRQ_SHFT                             0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR                          (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000014)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_RMSK                               0x303
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0CTEN_BMSK                       0x200
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0CTEN_SHFT                         0x9
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0VTEN_BMSK                       0x100
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0VTEN_SHFT                         0x8
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0VCTEN_BMSK                        0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0VCTEN_SHFT                        0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0PCTEN_BMSK                        0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0PCTEN_SHFT                        0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_ADDR                         (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000018)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_RMSK                         0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_CNTVOFF_L0_SHFT                     0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_ADDR                         (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000001c)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_RMSK                           0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_CNTVOFF_HI_SHFT                     0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR                       (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000020)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_RMSK                       0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR                       (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000024)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_RMSK                         0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR                          (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000028)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_0_CNTP_TVAL_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR                           (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000002c)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_RMSK                                  0x7
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ISTAT_BMSK                            0x4
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_ISTAT_SHFT                            0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_IMSK_BMSK                             0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_IMSK_SHFT                             0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_EN_BMSK                               0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_0_EN_SHFT                               0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR                       (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000030)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_RMSK                       0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR                       (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000034)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_RMSK                         0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR                          (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000038)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_0_CNTV_TVAL_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR                           (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000003c)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_RMSK                                  0x7
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_ISTAT_BMSK                            0x4
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_ISTAT_SHFT                            0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_IMSK_BMSK                             0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_IMSK_SHFT                             0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_EN_BMSK                               0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_0_EN_SHFT                               0x0

#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_ADDR                            (QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000fd0)
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_RMSK                            0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_VERSION_0_ADDR, HWIO_QDSP6SS_QTMR_V1_VERSION_0_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_VERSION_0_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_MAJOR_BMSK                      0xf0000000
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_MAJOR_SHFT                            0x1c
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_MINOR_BMSK                       0xfff0000
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_MINOR_SHFT                            0x10
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_STEP_BMSK                           0xffff
#define HWIO_QDSP6SS_QTMR_V1_VERSION_0_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: QDSP6SS_QTMR_F1_1
 *--------------------------------------------------------------------------*/

#define QDSP6SS_QTMR_F1_1_REG_BASE                                     (MSS_TOP_BASE      + 0x00122000)

#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR                          (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000000)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_1_CNTPCT_LO_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_ADDR                          (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000004)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_RMSK                            0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTPCT_HI_1_CNTPCT_HI_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_ADDR                          (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000008)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_LO_1_CNTVCT_LO_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_ADDR                          (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x0000000c)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_RMSK                            0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVCT_HI_1_CNTVCT_HI_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_ADDR                             (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000010)
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_RMSK                             0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_CNTFRQ_BMSK                      0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTFRQ_1_CNTFRQ_SHFT                             0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR                          (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000014)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_RMSK                               0x303
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0CTEN_BMSK                       0x200
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0CTEN_SHFT                         0x9
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0VTEN_BMSK                       0x100
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0VTEN_SHFT                         0x8
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0VCTEN_BMSK                        0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0VCTEN_SHFT                        0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0PCTEN_BMSK                        0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0PCTEN_SHFT                        0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_ADDR                         (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000018)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_RMSK                         0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_CNTVOFF_L0_SHFT                     0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_ADDR                         (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x0000001c)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_RMSK                           0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_CNTVOFF_HI_SHFT                     0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR                       (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000020)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_RMSK                       0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR                       (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000024)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_RMSK                         0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR                          (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000028)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTP_TVAL_1_CNTP_TVAL_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR                           (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x0000002c)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_RMSK                                  0x7
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_ISTAT_BMSK                            0x4
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_ISTAT_SHFT                            0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_IMSK_BMSK                             0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_IMSK_SHFT                             0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_EN_BMSK                               0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTP_CTL_1_EN_SHFT                               0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR                       (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000030)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_RMSK                       0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR                       (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000034)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_RMSK                         0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR                          (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000038)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_RMSK                          0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_CNTV_TVAL_1_CNTV_TVAL_SHFT                       0x0

#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR                           (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x0000003c)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_RMSK                                  0x7
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR, HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR,v)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR,m,v,HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_IN)
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_ISTAT_BMSK                            0x4
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_ISTAT_SHFT                            0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_IMSK_BMSK                             0x2
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_IMSK_SHFT                             0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_EN_BMSK                               0x1
#define HWIO_QDSP6SS_QTMR_V1_CNTV_CTL_1_EN_SHFT                               0x0

#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_ADDR                            (QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000fd0)
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_RMSK                            0xffffffff
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_IN          \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_VERSION_1_ADDR, HWIO_QDSP6SS_QTMR_V1_VERSION_1_RMSK)
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QTMR_V1_VERSION_1_ADDR, m)
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_MAJOR_BMSK                      0xf0000000
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_MAJOR_SHFT                            0x1c
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_MINOR_BMSK                       0xfff0000
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_MINOR_SHFT                            0x10
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_STEP_BMSK                           0xffff
#define HWIO_QDSP6SS_QTMR_V1_VERSION_1_STEP_SHFT                              0x0


#endif /* __MBA_TIMER_H__ */
