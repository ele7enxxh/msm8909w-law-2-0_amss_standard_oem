#ifndef __HALHWIOBIMC_H__
#define __HALHWIOBIMC_H__
/*
===========================================================================
*/
/**
  @file HALhwioBimc.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following bases:
    BIMC

  'Include' filters applied: <none>
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2011-2014 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/icb/src/common/HALbimcHwioGeneric.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: BIMC_MISC
 *--------------------------------------------------------------------------*/

#define BIMC_MISC_REG_BASE(b)                                                   ((b)      + 0x00000000)

#define BIMC_MISC_HW_VERSION_ADDR(b)                                       (BIMC_MISC_REG_BASE(b) + 0x00000000)
#define BIMC_MISC_HW_VERSION_RMSK                                       0xffffffff
#define BIMC_MISC_HW_VERSION_IN(b)       \
        in_dword_masked(BIMC_MISC_HW_VERSION_ADDR(b), BIMC_MISC_HW_VERSION_RMSK)
#define BIMC_MISC_HW_VERSION_INM(b,m)   \
        in_dword_masked(BIMC_MISC_HW_VERSION_ADDR(b), m)
#define BIMC_MISC_HW_VERSION_MAJOR_BMSK                                 0xf0000000
#define BIMC_MISC_HW_VERSION_MAJOR_SHFT                                       0x1c
#define BIMC_MISC_HW_VERSION_MINOR_BMSK                                  0xfff0000
#define BIMC_MISC_HW_VERSION_MINOR_SHFT                                       0x10
#define BIMC_MISC_HW_VERSION_STEP_BMSK                                      0xffff
#define BIMC_MISC_HW_VERSION_STEP_SHFT                                         0x0

#define BIMC_MISC_CORE_INFO_ADDR(b)                                        (BIMC_MISC_REG_BASE(b) + 0x00000004)
#define BIMC_MISC_CORE_INFO_RMSK                                        0xffffffff
#define BIMC_MISC_CORE_INFO_IN(b)       \
        in_dword_masked(BIMC_MISC_CORE_INFO_ADDR(b), BIMC_MISC_CORE_INFO_RMSK)
#define BIMC_MISC_CORE_INFO_INM(b,m)   \
        in_dword_masked(BIMC_MISC_CORE_INFO_ADDR(b), m)
#define BIMC_MISC_CORE_INFO_INFO_BMSK                                   0xffffffff
#define BIMC_MISC_CORE_INFO_INFO_SHFT                                          0x0

#define BIMC_MISC_HW_INFO_ADDR(b)                                          (BIMC_MISC_REG_BASE(b) + 0x00000008)
#define BIMC_MISC_HW_INFO_RMSK                                          0xffffffff
#define BIMC_MISC_HW_INFO_IN(b)       \
        in_dword_masked(BIMC_MISC_HW_INFO_ADDR(b), BIMC_MISC_HW_INFO_RMSK)
#define BIMC_MISC_HW_INFO_INM(b,m)   \
        in_dword_masked(BIMC_MISC_HW_INFO_ADDR(b), m)
#define BIMC_MISC_HW_INFO_MAJOR_BMSK                                    0xff000000
#define BIMC_MISC_HW_INFO_MAJOR_SHFT                                          0x18
#define BIMC_MISC_HW_INFO_BRANCH_BMSK                                     0xff0000
#define BIMC_MISC_HW_INFO_BRANCH_SHFT                                         0x10
#define BIMC_MISC_HW_INFO_MINOR_BMSK                                        0xff00
#define BIMC_MISC_HW_INFO_MINOR_SHFT                                           0x8
#define BIMC_MISC_HW_INFO_ECO_BMSK                                            0xff
#define BIMC_MISC_HW_INFO_ECO_SHFT                                             0x0

#define BIMC_MISC_CORE_CLK_SEL_ADDR(b)                                     (BIMC_MISC_REG_BASE(b) + 0x00000020)
#define BIMC_MISC_CORE_CLK_SEL_RMSK                                            0x3
#define BIMC_MISC_CORE_CLK_SEL_IN(b)       \
        in_dword_masked(BIMC_MISC_CORE_CLK_SEL_ADDR(b), BIMC_MISC_CORE_CLK_SEL_RMSK)
#define BIMC_MISC_CORE_CLK_SEL_INM(b,m)   \
        in_dword_masked(BIMC_MISC_CORE_CLK_SEL_ADDR(b), m)
#define BIMC_MISC_CORE_CLK_SEL_OUT(b,v)    \
        out_dword(BIMC_MISC_CORE_CLK_SEL_ADDR(b),v)
#define BIMC_MISC_CORE_CLK_SEL_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_MISC_CORE_CLK_SEL_ADDR(b),m,v,BIMC_MISC_CORE_CLK_SEL_IN(b))
#define BIMC_MISC_CORE_CLK_SEL_CLK_2X_MODE_BMSK                                0x2
#define BIMC_MISC_CORE_CLK_SEL_CLK_2X_MODE_SHFT                                0x1
#define BIMC_MISC_CORE_CLK_SEL_CLK_2X_MODE_CTRL_EN_BMSK                        0x1
#define BIMC_MISC_CORE_CLK_SEL_CLK_2X_MODE_CTRL_EN_SHFT                        0x0

#define BIMC_MISC_BIMC_CORE_CLK_PERIOD_ADDR(b)                             (BIMC_MISC_REG_BASE(b) + 0x00000024)
#define BIMC_MISC_BIMC_CORE_CLK_PERIOD_RMSK                                   0xff
#define BIMC_MISC_BIMC_CORE_CLK_PERIOD_IN(b)       \
        in_dword_masked(BIMC_MISC_BIMC_CORE_CLK_PERIOD_ADDR(b), BIMC_MISC_BIMC_CORE_CLK_PERIOD_RMSK)
#define BIMC_MISC_BIMC_CORE_CLK_PERIOD_INM(b,m)   \
        in_dword_masked(BIMC_MISC_BIMC_CORE_CLK_PERIOD_ADDR(b), m)
#define BIMC_MISC_BIMC_CORE_CLK_PERIOD_OUT(b,v)    \
        out_dword(BIMC_MISC_BIMC_CORE_CLK_PERIOD_ADDR(b),v)
#define BIMC_MISC_BIMC_CORE_CLK_PERIOD_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_MISC_BIMC_CORE_CLK_PERIOD_ADDR(b),m,v,BIMC_MISC_BIMC_CORE_CLK_PERIOD_IN(b))
#define BIMC_MISC_BIMC_CORE_CLK_PERIOD_PERIOD_BMSK                            0xff
#define BIMC_MISC_BIMC_CORE_CLK_PERIOD_PERIOD_SHFT                             0x0

#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(b,n)                     (BIMC_MISC_REG_BASE(b) + 0x00000100 + 0x4 * (n))
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_RMSK                        0x100000ff
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_MAXn                                 1
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_INI(b,n)        \
        in_dword_masked(BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(b,n), BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_RMSK)
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_INMI(b,n,mask)    \
        in_dword_masked(BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(b,n), (mask))
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_OUTI(n,val)    \
        out_dword(BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(b,n),val)
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_OUTMI(n,mask,val) \
        out_dword_masked_ns(BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_ADDR(b,n),mask,val,BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_INI(b,n))
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_ENABLE_BMSK                 0x10000000
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_ENABLE_SHFT                       0x1c
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                0xff
#define BIMC_MISC_SCMOn_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                 0x0

#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(b,n)                      (BIMC_MISC_REG_BASE(b) + 0x00000140 + 0x4 * (n))
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_RMSK                         0x100000ff
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_MAXn                                  1
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_INI(b,n)        \
        in_dword_masked(BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(b,n), BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_RMSK)
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_INMI(b,n,mask)    \
        in_dword_masked(BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(b,n), (mask))
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_OUTI(n,val)    \
        out_dword(BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(b,n),val)
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_OUTMI(n,mask,val) \
        out_dword_masked_ns(BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_ADDR(b,n),mask,val,BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_INI(b,n))
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_ENABLE_BMSK                  0x10000000
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_ENABLE_SHFT                        0x1c
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                 0xff
#define BIMC_MISC_DPEn_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                  0x0

#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR(b)           (BIMC_MISC_REG_BASE(b) + 0x00000180)
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_RMSK                 0x100000ff
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_IN(b)       \
        in_dword_masked(BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR(b), BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_RMSK)
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_INM(b,m)    \
        in_dword_masked(BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR(b), m)
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_OUT(b,v)    \
        out_dword(BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR(b),v)
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_ADDR(b),m,v,BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_IN(b))
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_BMSK   0x10000000
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_SHFT         0x1c
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK         0xff
#define BIMC_MPORT_CORE_DOMAIN_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT          0x0

#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(b,n)                   (BIMC_MISC_REG_BASE(b)   + 0x000001c0 + 0x4 * (n))
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_RMSK                        0x100000ff
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_MAXn                                 6
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_INI(b,n)      \
        in_dword_masked(BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(b,n), BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_RMSK)
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_INMI(b,n,mask)    \
        in_dword_masked(BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(b,n), mask)
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_OUTI(b,n,val)    \
        out_dword(BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(b,n),val)
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_OUTIM(b,n,mask,val) \
        out_dword_masked_ns(BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_ADDR(b,n),mask,val,BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_INI(b,n))
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_BMSK          0x10000000
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_SHFT                0x1c
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                0xff
#define BIMC_MPORTn_CLK_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                 0x0

#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR(b)                           (BIMC_MISC_REG_BASE(b)   + 0x00000200)
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_RMSK                              0x100000ff
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_IN(b)       \
        in_dword_masked(BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR(b), BIMC_SWAY_LOCAL_CGC_THRESHOLD_RMSK)
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_INM(b,m)      \
        in_dword_masked(BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR(b), m)
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_OUT(b,v)      \
        out_dword(BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR,(b)v)
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_SWAY_LOCAL_CGC_THRESHOLD_ADDR(b),m,v,BIMC_SWAY_LOCAL_CGC_THRESHOLD_IN(b))
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_BMSK                0x10000000
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_THRESHOLD_ENA_SHFT                      0x1c
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_BMSK                      0xff
#define BIMC_SWAY_LOCAL_CGC_THRESHOLD_THRESHOLD_VAL_SHFT                       0x0

#define BIMC_SCMOn_CGC_CFG_ADDR(b,n)                                    (BIMC_MISC_REG_BASE(b)   + 0x00000240 + 0x4 * (n))
#define BIMC_SCMOn_CGC_CFG_RMSK                                         0x110000ff
#define BIMC_SCMOn_CGC_CFG_MAXn                                                  1
#define BIMC_SCMOn_CGC_CFG_INI(b,n)        \
        in_dword_masked(BIMC_SCMOn_CGC_CFG_ADDR(b,n), BIMC_SCMOn_CGC_CFG_RMSK)
#define BIMC_SCMOn_CGC_CFG_INMI(b,n,mask)    \
        in_dword_masked(BIMC_SCMOn_CGC_CFG_ADDR(b,n), mask)
#define BIMC_SCMOn_CGC_CFG_OUTI(b,n,val)    \
        out_dword(BIMC_SCMOn_CGC_CFG_ADDR(b,n),val)
#define BIMC_SCMOn_CGC_CFG_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_SCMOn_CGC_CFG_ADDR(b,n),mask,val,BIMC_SCMOn_CGC_CFG_INI(b,n))
#define BIMC_SCMOn_CGC_CFG_CLK_DIS_BMSK                                 0x10000000
#define BIMC_SCMOn_CGC_CFG_CLK_DIS_SHFT                                       0x1c
#define BIMC_SCMOn_CGC_CFG_THRESHOLD_ENA_BMSK                            0x1000000
#define BIMC_SCMOn_CGC_CFG_THRESHOLD_ENA_SHFT                                 0x18
#define BIMC_SCMOn_CGC_CFG_THRESHOLD_VAL_BMSK                                 0xff
#define BIMC_SCMOn_CGC_CFG_THRESHOLD_VAL_SHFT                                  0x0

#define BIMC_DPEn_CGC_CFG_ADDR(b,n)                                     (BIMC_MISC_REG_BASE(b)   + 0x00000280 + 0x4 * (n))
#define BIMC_DPEn_CGC_CFG_RMSK                                          0x110000ff
#define BIMC_DPEn_CGC_CFG_MAXn                                                   1
#define BIMC_DPEn_CGC_CFG_INI(b,n)        \
        in_dword_masked(BIMC_DPEn_CGC_CFG_ADDR(b,n), BIMC_DPEn_CGC_CFG_RMSK)
#define BIMC_DPEn_CGC_CFG_INMI(b,n,mask)    \
        in_dword_masked(BIMC_DPEn_CGC_CFG_ADDR(b,n), mask)
#define BIMC_DPEn_CGC_CFG_OUTI(b,n,val)    \
        out_dword(BIMC_DPEn_CGC_CFG_ADDR(b,n),val)
#define BIMC_DPEn_CGC_CFG_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_DPEn_CGC_CFG_ADDR(b,n),mask,val,BIMC_DPEn_CGC_CFG_INI(b,n))
#define BIMC_DPEn_CGC_CFG_CLK_DIS_BMSK                                  0x10000000
#define BIMC_DPEn_CGC_CFG_CLK_DIS_SHFT                                        0x1c
#define BIMC_DPEn_CGC_CFG_THRESHOLD_ENA_BMSK                             0x1000000
#define BIMC_DPEn_CGC_CFG_THRESHOLD_ENA_SHFT                                  0x18
#define BIMC_DPEn_CGC_CFG_THRESHOLD_VAL_BMSK                                  0xff
#define BIMC_DPEn_CGC_CFG_THRESHOLD_VAL_SHFT                                   0x0

#define BIMC_CALIBRATION_CFG_ADDR(b)                                    (BIMC_MISC_REG_BASE(b)   + 0x00000400)
#define BIMC_CALIBRATION_CFG_RMSK                                       0x10ffffff
#define BIMC_CALIBRATION_CFG_IN(b)       \
        in_dword_masked(BIMC_CALIBRATION_CFG_ADDR(b), BIMC_CALIBRATION_CFG_RMSK)
#define BIMC_CALIBRATION_CFG_INM(b,m)    \
        in_dword_masked(BIMC_CALIBRATION_CFG_ADDR(b), m)
#define BIMC_CALIBRATION_CFG_OUT(b,v)    \
        out_dword(BIMC_CALIBRATION_CFG_ADDR(b),v)
#define BIMC_CALIBRATION_CFG_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_CALIBRATION_CFG_ADDR(b),m,v,BIMC_CALIBRATION_CFG_IN(b))
#define BIMC_CALIBRATION_CFG_RAPID_CAL_ENA_BMSK                         0x10000000
#define BIMC_CALIBRATION_CFG_RAPID_CAL_ENA_SHFT                               0x1c
#define BIMC_CALIBRATION_CFG_COUNT_VAL_BMSK                               0xffffff
#define BIMC_CALIBRATION_CFG_COUNT_VAL_SHFT                                    0x0

#define BIMC_DFIn_LP_CFG_ADDR(b,n)                                        (BIMC_MISC_REG_BASE(b)   + 0x00000600 + 0x4 * (n))
#define BIMC_DFIn_LP_CFG_RMSK                                               0xf11f
#define BIMC_DFIn_LP_CFG_MAXn                                                    1
#define BIMC_DFIn_LP_CFG_INI(b,n)        \
        in_dword_masked(BIMC_DFIn_LP_CFG_ADDR(b,n), BIMC_DFIn_LP_CFG_RMSK)
#define BIMC_DFIn_LP_CFG_INMI(b,n,mask)    \
        in_dword_masked(BIMC_DFIn_LP_CFG_ADDR(b,n), mask)
#define BIMC_DFIn_LP_CFG_OUTI(b,n,val)    \
        out_dword(BIMC_DFIn_LP_CFG_ADDR(b,n),val)
#define BIMC_DFIn_LP_CFG_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_DFIn_LP_CFG_ADDR(b,n),mask,val,BIMC_DFIn_LP_CFG_INI(b,n))
#define BIMC_DFIn_LP_CFG_RFU_BMSK                                           0xf000
#define BIMC_DFIn_LP_CFG_RFU_SHFT                                              0xc
#define BIMC_DFIn_LP_CFG_LP_ACK_BMSK                                         0x100
#define BIMC_DFIn_LP_CFG_LP_ACK_SHFT                                           0x8
#define BIMC_DFIn_LP_CFG_LP_REQ_BMSK                                          0x10
#define BIMC_DFIn_LP_CFG_LP_REQ_SHFT                                           0x4
#define BIMC_DFIn_LP_CFG_LP_WAKEUP_BMSK                                        0xf
#define BIMC_DFIn_LP_CFG_LP_WAKEUP_SHFT                                        0x0

#define BIMC_HFSC_CFG_ADDR(b)                                           (BIMC_MISC_REG_BASE(b)   + 0x00000800)
#define BIMC_HFSC_CFG_RMSK                                                     0x1
#define BIMC_HFSC_CFG_IN(b)       \
        in_dword_masked(BIMC_HFSC_CFG_ADDR(b), BIMC_HFSC_CFG_RMSK)
#define BIMC_HFSC_CFG_INM(b,m)    \
        in_dword_masked(BIMC_HFSC_CFG_ADDR(b), m)
#define BIMC_HFSC_CFG_OUT(b,v)    \
        out_dword(BIMC_HFSC_CFG_ADDR(b),v)
#define BIMC_HFSC_CFG_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_HFSC_CFG_ADDR(b),m,v,BIMC_HFSC_CFG_IN(b))
#define BIMC_HFSC_CFG_DISABLE_BMSK                                             0x1
#define BIMC_HFSC_CFG_DISABLE_SHFT                                             0x0

#define BIMC_HFSC_STATUS_ADDR(b)                                        (BIMC_MISC_REG_BASE(b)   + 0x0000080c)
#define BIMC_HFSC_STATUS_RMSK                                                  0x1
#define BIMC_HFSC_STATUS_IN(b)       \
        in_dword_masked(BIMC_HFSC_STATUS_ADDR(b), BIMC_HFSC_STATUS_RMSK)
#define BIMC_HFSC_STATUS_INM(b,m)      \
        in_dword_masked(BIMC_HFSC_STATUS_ADDR(b), m)
#define BIMC_HFSC_STATUS_BUSY_BMSK                                             0x1
#define BIMC_HFSC_STATUS_BUSY_SHFT                                             0x0

#define BIMC_HFSC_STAGE1_CTRL_ADDR(b)                                   (BIMC_MISC_REG_BASE(b)   + 0x00000840)
#define BIMC_HFSC_STAGE1_CTRL_RMSK                                            0x11
#define BIMC_HFSC_STAGE1_CTRL_IN(b)       \
        in_dword_masked(BIMC_HFSC_STAGE1_CTRL_ADDR(b), BIMC_HFSC_STAGE1_CTRL_RMSK)
#define BIMC_HFSC_STAGE1_CTRL_INM(b,m)    \
        in_dword_masked(BIMC_HFSC_STAGE1_CTRL_ADDR(b), m)
#define BIMC_HFSC_STAGE1_CTRL_OUT(b,v)    \
        out_dword(BIMC_HFSC_STAGE1_CTRL_ADDR(b),v)
#define BIMC_HFSC_STAGE1_CTRL_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_HFSC_STAGE1_CTRL_ADDR(b),m,v,BIMC_HFSC_STAGE1_CTRL_IN(b))
#define BIMC_HFSC_STAGE1_CTRL_ACK_BMSK                                        0x10
#define BIMC_HFSC_STAGE1_CTRL_ACK_SHFT                                         0x4
#define BIMC_HFSC_STAGE1_CTRL_REQ_BMSK                                         0x1
#define BIMC_HFSC_STAGE1_CTRL_REQ_SHFT                                         0x0

#define BIMC_HFSC_STAGE2_CTRL_ADDR(b)                                   (BIMC_MISC_REG_BASE(b)   + 0x00000860)
#define BIMC_HFSC_STAGE2_CTRL_RMSK                                            0x11
#define BIMC_HFSC_STAGE2_CTRL_IN(b)       \
        in_dword_masked(BIMC_HFSC_STAGE2_CTRL_ADDR(b), BIMC_HFSC_STAGE2_CTRL_RMSK)
#define BIMC_HFSC_STAGE2_CTRL_INM(b,m)    \
        in_dword_masked(BIMC_HFSC_STAGE2_CTRL_ADDR(b), m)
#define BIMC_HFSC_STAGE2_CTRL_OUT(b,v)    \
        out_dword(BIMC_HFSC_STAGE2_CTRL_ADDR(b),v)
#define BIMC_HFSC_STAGE2_CTRL_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_HFSC_STAGE2_CTRL_ADDR(b),m,v,BIMC_HFSC_STAGE2_CTRL_IN(b))
#define BIMC_HFSC_STAGE2_CTRL_ACK_BMSK                                        0x10
#define BIMC_HFSC_STAGE2_CTRL_ACK_SHFT                                         0x4
#define BIMC_HFSC_STAGE2_CTRL_REQ_BMSK                                         0x1
#define BIMC_HFSC_STAGE2_CTRL_REQ_SHFT                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_GLOBAL0
 *--------------------------------------------------------------------------*/

#define BIMC_GLOBAL0_REG_BASE(b)                                   ((b)      + 0x00000000)

#define BIMC_QOS_CTRL_ADDR(b)                                                              (BIMC_GLOBAL0_REG_BASE(b) + 0x00000900)
#define BIMC_QOS_CTRL_RMSK                                                                        0x1
#define BIMC_QOS_CTRL_IN(b)      \
        in_dword_masked(BIMC_QOS_CTRL_ADDR(b), BIMC_QOS_CTRL_RMSK)
#define BIMC_QOS_CTRL_INM(b, m)      \
        in_dword_masked(BIMC_QOS_CTRL_ADDR(b), m)
#define BIMC_QOS_CTRL_OUT(b, v)      \
        out_dword(BIMC_QOS_CTRL_ADDR(b),v)
#define BIMC_QOS_CTRL_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_QOS_CTRL_ADDR(b),m,v,BIMC_QOS_CTRL_IN(b))
#define BIMC_QOS_CTRL_QOS_AGGR_ENABLE_BMSK                                                        0x1
#define BIMC_QOS_CTRL_QOS_AGGR_ENABLE_SHFT                                                        0x0

#define BIMC_QOS_CFG_ADDR(b)                                                               (BIMC_GLOBAL0_REG_BASE(b) + 0x00000904)
#define BIMC_QOS_CFG_RMSK                                                                       0x7ff
#define BIMC_QOS_CFG_IN(b)      \
        in_dword_masked(BIMC_QOS_CFG_ADDR(b), BIMC_QOS_CFG_RMSK)
#define BIMC_QOS_CFG_INM(b, m)      \
        in_dword_masked(BIMC_QOS_CFG_ADDR(b), m)
#define BIMC_QOS_CFG_OUT(b, v)      \
        out_dword(BIMC_QOS_CFG_ADDR(b),v)
#define BIMC_QOS_CFG_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_QOS_CFG_ADDR(b),m,v,BIMC_QOS_CFG_IN(b))
#define BIMC_QOS_CFG_QOS_ASSIST_REQ_LEGACY_SEL_BMSK                                             0x400
#define BIMC_QOS_CFG_QOS_ASSIST_REQ_LEGACY_SEL_SHFT                                               0xa
#define BIMC_QOS_CFG_QOS_ASSIST_REQ_MODE_BMSK                                                   0x200
#define BIMC_QOS_CFG_QOS_ASSIST_REQ_MODE_SHFT                                                     0x9
#define BIMC_QOS_CFG_CGC_BYPASS_BMSK                                                            0x100
#define BIMC_QOS_CFG_CGC_BYPASS_SHFT                                                              0x8
#define BIMC_QOS_CFG_RFU_7_BMSK                                                                  0x80
#define BIMC_QOS_CFG_RFU_7_SHFT                                                                   0x7
#define BIMC_QOS_CFG_DOWNTIME_REQ_MAX_SKEW_BMSK                                                  0x70
#define BIMC_QOS_CFG_DOWNTIME_REQ_MAX_SKEW_SHFT                                                   0x4
#define BIMC_QOS_CFG_RFU_3_BMSK                                                                   0x8
#define BIMC_QOS_CFG_RFU_3_SHFT                                                                   0x3
#define BIMC_QOS_CFG_MSTR_DANGER_MAX_SKEW_BMSK                                                    0x7
#define BIMC_QOS_CFG_MSTR_DANGER_MAX_SKEW_SHFT                                                    0x0

#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_ADDR(b)                                           (BIMC_GLOBAL0_REG_BASE(b) + 0x00000908)
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_RMSK                                              0xffffffff
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_IN(b)      \
        in_dword_masked(BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_ADDR(b), BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_RMSK)
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_INM(b, m)      \
        in_dword_masked(BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_ADDR(b), m)
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_OUT(b, v)      \
        out_dword(BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_ADDR(b),v)
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_ADDR(b),m,v,BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_IN(b))
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_POST_STALL_TIMEOUT_CNT_LO_URG_BMSK                0xffff0000
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_POST_STALL_TIMEOUT_CNT_LO_URG_SHFT                      0x10
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_PRE_STALL_TIMEOUT_CNT_LO_URG_BMSK                     0xffff
#define BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_PRE_STALL_TIMEOUT_CNT_LO_URG_SHFT                        0x0

#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_ADDR(b)                                          (BIMC_GLOBAL0_REG_BASE(b) + 0x0000090c)
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_RMSK                                             0xffffffff
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_IN(b)      \
        in_dword_masked(BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_ADDR(b), BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_RMSK)
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_INM(b, m)      \
        in_dword_masked(BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_ADDR(b), m)
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_OUT(b, v)      \
        out_dword(BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_ADDR(b),v)
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_ADDR(b),m,v,BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_IN(b))
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_POST_STALL_TIMEOUT_CNT_HI_URG_BMSK               0xffff0000
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_POST_STALL_TIMEOUT_CNT_HI_URG_SHFT                     0x10
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_PRE_STALL_TIMEOUT_CNT_HI_URG_BMSK                    0xffff
#define BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_PRE_STALL_TIMEOUT_CNT_HI_URG_SHFT                       0x0

#define BIMC_QOS_STATUS_ADDR(b)                                                            (BIMC_GLOBAL0_REG_BASE(b) + 0x00000910)
#define BIMC_QOS_STATUS_RMSK                                                                      0xf
#define BIMC_QOS_STATUS_IN(b)      \
        in_dword_masked(BIMC_QOS_STATUS_ADDR(b), BIMC_QOS_STATUS_RMSK)
#define BIMC_QOS_STATUS_INM(b, m)      \
        in_dword_masked(BIMC_QOS_STATUS_ADDR(b), m)
#define BIMC_QOS_STATUS_PRE_STALL_TIMEOUT_BMSK                                                    0xf
#define BIMC_QOS_STATUS_PRE_STALL_TIMEOUT_SHFT                                                    0x0

#define BIMC_QOS_STATUS_CLR_ADDR(b)                                                        (BIMC_GLOBAL0_REG_BASE(b) + 0x00000914)
#define BIMC_QOS_STATUS_CLR_RMSK                                                                  0xf
#define BIMC_QOS_STATUS_CLR_OUT(b, v)      \
        out_dword(BIMC_QOS_STATUS_CLR_ADDR(b),v)
#define BIMC_QOS_STATUS_CLR_PRE_STALL_TIMEOUT_CLR_BMSK                                            0xf
#define BIMC_QOS_STATUS_CLR_PRE_STALL_TIMEOUT_CLR_SHFT                                            0x0

#define BIMC_QOS_FSSH_CTRL_ADDR(b)                                                         (BIMC_GLOBAL0_REG_BASE(b) + 0x00000920)
#define BIMC_QOS_FSSH_CTRL_RMSK                                                                   0x1
#define BIMC_QOS_FSSH_CTRL_IN(b)      \
        in_dword_masked(BIMC_QOS_FSSH_CTRL_ADDR(b), BIMC_QOS_FSSH_CTRL_RMSK)
#define BIMC_QOS_FSSH_CTRL_INM(b, m)      \
        in_dword_masked(BIMC_QOS_FSSH_CTRL_ADDR(b), m)
#define BIMC_QOS_FSSH_CTRL_OUT(b, v)      \
        out_dword(BIMC_QOS_FSSH_CTRL_ADDR(b),v)
#define BIMC_QOS_FSSH_CTRL_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_QOS_FSSH_CTRL_ADDR(b),m,v,BIMC_QOS_FSSH_CTRL_IN(b))
#define BIMC_QOS_FSSH_CTRL_FSSH_DISABLE_BMSK                                                      0x1
#define BIMC_QOS_FSSH_CTRL_FSSH_DISABLE_SHFT                                                      0x0

#define BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_ADDR(b)                                 (BIMC_GLOBAL0_REG_BASE(b) + 0x00000930)
#define BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_RMSK                                       0x1ffff
#define BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_IN(b)      \
        in_dword_masked(BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_ADDR(b), BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_RMSK)
#define BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_INM(b, m)      \
        in_dword_masked(BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_ADDR(b), m)
#define BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_OUT(b, v)      \
        out_dword(BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_ADDR(b),v)
#define BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_ADDR(b),m,v,BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_IN(b))
#define BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_THRESHOLD_PERIOD_BMSK                      0x1ffff
#define BIMC_QOS_FSSH_FREQ_THRESHOLD_URGENCY_BAND0_THRESHOLD_PERIOD_SHFT                          0x0

#define BIMC_QOS_FSSH_URGENCY_SEL_ADDR(b)                                                  (BIMC_GLOBAL0_REG_BASE(b) + 0x00000940)
#define BIMC_QOS_FSSH_URGENCY_SEL_RMSK                                                           0x11
#define BIMC_QOS_FSSH_URGENCY_SEL_IN(b)      \
        in_dword_masked(BIMC_QOS_FSSH_URGENCY_SEL_ADDR(b), BIMC_QOS_FSSH_URGENCY_SEL_RMSK)
#define BIMC_QOS_FSSH_URGENCY_SEL_INM(b, m)      \
        in_dword_masked(BIMC_QOS_FSSH_URGENCY_SEL_ADDR(b), m)
#define BIMC_QOS_FSSH_URGENCY_SEL_OUT(b, v)      \
        out_dword(BIMC_QOS_FSSH_URGENCY_SEL_ADDR(b),v)
#define BIMC_QOS_FSSH_URGENCY_SEL_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_QOS_FSSH_URGENCY_SEL_ADDR(b),m,v,BIMC_QOS_FSSH_URGENCY_SEL_IN(b))
#define BIMC_QOS_FSSH_URGENCY_SEL_DIFF_BAND_URGENCY_BMSK                                         0x10
#define BIMC_QOS_FSSH_URGENCY_SEL_DIFF_BAND_URGENCY_SHFT                                          0x4
#define BIMC_QOS_FSSH_URGENCY_SEL_SAME_BAND_URGENCY_BMSK                                          0x1
#define BIMC_QOS_FSSH_URGENCY_SEL_SAME_BAND_URGENCY_SHFT                                          0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_GLOBAL1
 *--------------------------------------------------------------------------*/

#define BIMC_GLOBAL1_REG_BASE(b)                                   ((b)      + 0x00001000)

#define BIMC_GLOBAL1_COMPONENT_INFO_ADDR(b)                   (BIMC_GLOBAL1_REG_BASE(b) + 0x00000000)
#define BIMC_GLOBAL1_COMPONENT_INFO_RMSK                            0xffff
#define BIMC_GLOBAL1_COMPONENT_INFO_IN(b)       \
        in_dword_masked(BIMC_GLOBAL1_COMPONENT_INFO_ADDR(b), BIMC_GLOBAL1_COMPONENT_INFO_RMSK)
#define BIMC_GLOBAL1_COMPONENT_INFO_INM(b,m)   \
        in_dword_masked(BIMC_GLOBAL1_COMPONENT_INFO_ADDR(b), m)
#define BIMC_GLOBAL1_COMPONENT_INFO_SUB_TYPE_BMSK                   0xff00
#define BIMC_GLOBAL1_COMPONENT_INFO_SUB_TYPE_SHFT                      0x8
#define BIMC_GLOBAL1_COMPONENT_INFO_TYPE_BMSK                         0xff
#define BIMC_GLOBAL1_COMPONENT_INFO_TYPE_SHFT                          0x0

#define BIMC_BRIC_HW_INFO_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x00000010)
#define BIMC_BRIC_HW_INFO_RMSK                                  0xffffffff
#define BIMC_BRIC_HW_INFO_IN(b)      \
        in_dword_masked(BIMC_BRIC_HW_INFO_ADDR(b), BIMC_BRIC_HW_INFO_RMSK)
#define BIMC_BRIC_HW_INFO_INM(b,m)   \
        in_dword_masked(BIMC_BRIC_HW_INFO_ADDR(b), m)
#define BIMC_BRIC_HW_INFO_MAJOR_BMSK                            0xff000000
#define BIMC_BRIC_HW_INFO_MAJOR_SHFT                                  0x18
#define BIMC_BRIC_HW_INFO_BRANCH_BMSK                             0xff0000
#define BIMC_BRIC_HW_INFO_BRANCH_SHFT                                 0x10
#define BIMC_BRIC_HW_INFO_MINOR_BMSK                                0xff00
#define BIMC_BRIC_HW_INFO_MINOR_SHFT                                   0x8
#define BIMC_BRIC_HW_INFO_ECO_BMSK                                    0xff
#define BIMC_BRIC_HW_INFO_ECO_SHFT                                     0x0

#define BIMC_BRIC_CONFIGURATION_INFO_0_ADDR(b)                (BIMC_GLOBAL1_REG_BASE(b) + 0x00000020)
#define BIMC_BRIC_CONFIGURATION_INFO_0_RMSK                     0xffffffff
#define BIMC_BRIC_CONFIGURATION_INFO_0_IN(b)      \
        in_dword_masked(BIMC_BRIC_CONFIGURATION_INFO_0_ADDR(b), BIMC_BRIC_CONFIGURATION_INFO_0_RMSK)
#define BIMC_BRIC_CONFIGURATION_INFO_0_INM(b,m)   \
        in_dword_masked(BIMC_BRIC_CONFIGURATION_INFO_0_ADDR(b), m)
#define BIMC_BRIC_CONFIGURATION_INFO_0_ADDR_WIDTH_BMSK          0xff000000
#define BIMC_BRIC_CONFIGURATION_INFO_0_ADDR_WIDTH_SHFT                0x18
#define BIMC_BRIC_CONFIGURATION_INFO_0_BUSID_BMSK                 0xff0000
#define BIMC_BRIC_CONFIGURATION_INFO_0_BUSID_SHFT                     0x10
#define BIMC_BRIC_CONFIGURATION_INFO_0_NUM_SWAYS_BMSK               0xff00
#define BIMC_BRIC_CONFIGURATION_INFO_0_NUM_SWAYS_SHFT                  0x8
#define BIMC_BRIC_CONFIGURATION_INFO_0_NUM_MPORTS_BMSK                0xff
#define BIMC_BRIC_CONFIGURATION_INFO_0_NUM_MPORTS_SHFT                 0x0

#define BIMC_BRIC_CONFIGURATION_INFO_1_ADDR(b)                (BIMC_GLOBAL1_REG_BASE(b) + 0x00000030)
#define BIMC_BRIC_CONFIGURATION_INFO_1_RMSK                     0xffffffff
#define BIMC_BRIC_CONFIGURATION_INFO_1_IN(b)       \
        in_dword_masked(BIMC_BRIC_CONFIGURATION_INFO_1_ADDR(b), BIMC_BRIC_CONFIGURATION_INFO_1_RMSK)
#define BIMC_BRIC_CONFIGURATION_INFO_1_INM(b,m)   \
        in_dword_masked(BIMC_BRIC_CONFIGURATION_INFO_1_ADDR(b), m)
#define BIMC_BRIC_CONFIGURATION_INFO_1_DATA_WIDTH_BMSK          0xffff0000
#define BIMC_BRIC_CONFIGURATION_INFO_1_DATA_WIDTH_SHFT                0x10
#define BIMC_BRIC_CONFIGURATION_INFO_1_TID_WIDTH_BMSK               0xff00
#define BIMC_BRIC_CONFIGURATION_INFO_1_TID_WIDTH_SHFT                  0x8
#define BIMC_BRIC_CONFIGURATION_INFO_1_MID_WIDTH_BMSK                 0xff
#define BIMC_BRIC_CONFIGURATION_INFO_1_MID_WIDTH_SHFT                  0x0

#define BIMC_INTERRUPT_STATUS_0_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x00000100)
#define BIMC_INTERRUPT_STATUS_0_RMSK                                  0x7f
#define BIMC_INTERRUPT_STATUS_0_IN(b)       \
        in_dword_masked(BIMC_INTERRUPT_STATUS_0_ADDR(b), BIMC_INTERRUPT_STATUS_0_RMSK)
#define BIMC_INTERRUPT_STATUS_0_INM(b,m)   \
        in_dword_masked(BIMC_INTERRUPT_STATUS_0_ADDR(b), m)
#define BIMC_INTERRUPT_STATUS_0_MPORT_BMSK                            0x7f
#define BIMC_INTERRUPT_STATUS_0_MPORT_SHFT                             0x0

#define BIMC_INTERRUPT_CLEAR_0_ADDR(b)                              (BIMC_GLOBAL1_REG_BASE(b) + 0x00000108)
#define BIMC_INTERRUPT_CLEAR_0_RMSK                                   0x7f
#define BIMC_INTERRUPT_CLEAR_0_OUT(b,v)    \
        out_dword(BIMC_INTERRUPT_CLEAR_0_ADDR(b),v)
#define BIMC_INTERRUPT_CLEAR_0_MPORT_BMSK                             0x7f
#define BIMC_INTERRUPT_CLEAR_0_MPORT_SHFT                              0x0

#define BIMC_INTERRUPT_ENABLE_0_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x0000010c)
#define BIMC_INTERRUPT_ENABLE_0_RMSK                                  0x7f
#define BIMC_INTERRUPT_ENABLE_0_IN(b)       \
        in_dword_masked(BIMC_INTERRUPT_ENABLE_0_ADDR(b), BIMC_INTERRUPT_ENABLE_0_RMSK)
#define BIMC_INTERRUPT_ENABLE_0_INM(b,m)   \
        in_dword_masked(BIMC_INTERRUPT_ENABLE_0_ADDR(b), m)
#define BIMC_INTERRUPT_ENABLE_0_OUT(b,v)    \
        out_dword(BIMC_INTERRUPT_ENABLE_0_ADDR(b),v)
#define BIMC_INTERRUPT_ENABLE_0_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_INTERRUPT_ENABLE_0_ADDR(b),m,v,BIMC_INTERRUPT_ENABLE_0_IN(b))
#define BIMC_INTERRUPT_ENABLE_0_MPORT_BMSK                            0x7f
#define BIMC_INTERRUPT_ENABLE_0_MPORT_SHFT                             0x0

#define BIMC_INTERRUPT_STATUS_1_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x00000110)
#define BIMC_INTERRUPT_STATUS_1_RMSK                                  0x1f
#define BIMC_INTERRUPT_STATUS_1_IN(b)       \
        in_dword_masked(BIMC_INTERRUPT_STATUS_1_ADDR(b), BIMC_INTERRUPT_STATUS_1_RMSK)
#define BIMC_INTERRUPT_STATUS_1_INM(b,m)   \
        in_dword_masked(BIMC_INTERRUPT_STATUS_1_ADDR(b), m)
#define BIMC_INTERRUPT_STATUS_1_SWAY_0_BMSK                           0x1f
#define BIMC_INTERRUPT_STATUS_1_SWAY_0_SHFT                            0x0

#define BIMC_INTERRUPT_CLEAR_1_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x00000118)
#define BIMC_INTERRUPT_CLEAR_1_RMSK                                   0x1f
#define BIMC_INTERRUPT_CLEAR_1_OUT(b,v)    \
        out_dword(BIMC_INTERRUPT_CLEAR_1_ADDR(b),v)
#define BIMC_INTERRUPT_CLEAR_1_SWAY_0_BMSK                            0x1f
#define BIMC_INTERRUPT_CLEAR_1_SWAY_0_SHFT                             0x0

#define BIMC_INTERRUPT_ENABLE_1_ADDR(b)                            (BIMC_GLOBAL1_REG_BASE(b) + 0x0000011c)
#define BIMC_INTERRUPT_ENABLE_1_RMSK                                  0x1f
#define BIMC_INTERRUPT_ENABLE_1_IN(b)       \
        in_dword_masked(BIMC_INTERRUPT_ENABLE_1_ADDR(b), BIMC_INTERRUPT_ENABLE_1_RMSK)
#define BIMC_INTERRUPT_ENABLE_1_INM(b,m)   \
        in_dword_masked(BIMC_INTERRUPT_ENABLE_1_ADDR(b), m)
#define BIMC_INTERRUPT_ENABLE_1_OUT(b,v)    \
        out_dword(BIMC_INTERRUPT_ENABLE_1_ADDR(b),v)
#define BIMC_INTERRUPT_ENABLE_1_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_INTERRUPT_ENABLE_1_ADDR(b),m,v,BIMC_INTERRUPT_ENABLE_1_IN(b))
#define BIMC_INTERRUPT_ENABLE_1_SWAY_0_BMSK                           0x1f
#define BIMC_INTERRUPT_ENABLE_1_SWAY_0_SHFT                            0x0

#define BIMC_INTERRUPT_STATUS_2_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x00000120)
#define BIMC_INTERRUPT_STATUS_2_RMSK                                  0x1f
#define BIMC_INTERRUPT_STATUS_2_IN(b)       \
        in_dword_masked(BIMC_INTERRUPT_STATUS_2_ADDR(b), BIMC_INTERRUPT_STATUS_2_RMSK)
#define BIMC_INTERRUPT_STATUS_2_INM(b,m)   \
        in_dword_masked(BIMC_INTERRUPT_STATUS_2_ADDR(b), m)
#define BIMC_INTERRUPT_STATUS_2_SWAY_1_BMSK                           0x1f
#define BIMC_INTERRUPT_STATUS_2_SWAY_1_SHFT                            0x0

#define BIMC_INTERRUPT_CLEAR_2_ADDR(b)                              (BIMC_GLOBAL1_REG_BASE(b) + 0x00000128)
#define BIMC_INTERRUPT_CLEAR_2_RMSK                                   0x1f
#define BIMC_INTERRUPT_CLEAR_2_OUT(b,v)    \
        out_dword(BIMC_INTERRUPT_CLEAR_2_ADDR(b),v)
#define BIMC_INTERRUPT_CLEAR_2_SWAY_1_BMSK                            0x1f
#define BIMC_INTERRUPT_CLEAR_2_SWAY_1_SHFT                             0x0

#define BIMC_INTERRUPT_ENABLE_2_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x0000012c)
#define BIMC_INTERRUPT_ENABLE_2_RMSK                                  0x1f
#define BIMC_INTERRUPT_ENABLE_2_IN(b)       \
        in_dword_masked(BIMC_INTERRUPT_ENABLE_2_ADDR(b), BIMC_INTERRUPT_ENABLE_2_RMSK)
#define BIMC_INTERRUPT_ENABLE_2_INM(b,m)   \
        in_dword_masked(BIMC_INTERRUPT_ENABLE_2_ADDR(b), m)
#define BIMC_INTERRUPT_ENABLE_2_OUT(b,v)    \
        out_dword(BIMC_INTERRUPT_ENABLE_2_ADDR(b),v)
#define BIMC_INTERRUPT_ENABLE_2_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_INTERRUPT_ENABLE_2_ADDR(b),m,v,BIMC_INTERRUPT_ENABLE_2_IN(b))
#define BIMC_INTERRUPT_ENABLE_2_SWAY_1_BMSK                           0x1f
#define BIMC_INTERRUPT_ENABLE_2_SWAY_1_SHFT                            0x0

#define BIMC_INTERRUPT_STATUS_3_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x00000130)
#define BIMC_INTERRUPT_STATUS_3_RMSK                                  0x1f
#define BIMC_INTERRUPT_STATUS_3_IN(b)       \
        in_dword_masked(BIMC_INTERRUPT_STATUS_3_ADDR(b), BIMC_INTERRUPT_STATUS_3_RMSK)
#define BIMC_INTERRUPT_STATUS_3_INM(b,m)   \
        in_dword_masked(BIMC_INTERRUPT_STATUS_3_ADDR(b), m)
#define BIMC_INTERRUPT_STATUS_3_SWAY_2_BMSK                           0x1f
#define BIMC_INTERRUPT_STATUS_3_SWAY_2_SHFT                            0x0

#define BIMC_INTERRUPT_CLEAR_3_ADDR(b)                              (BIMC_GLOBAL1_REG_BASE(b) + 0x00000138)
#define BIMC_INTERRUPT_CLEAR_3_RMSK                                   0x1f
#define BIMC_INTERRUPT_CLEAR_3_OUT(b,v)    \
        out_dword(BIMC_INTERRUPT_CLEAR_3_ADDR(b),v)
#define BIMC_INTERRUPT_CLEAR_3_SWAY_2_BMSK                            0x1f
#define BIMC_INTERRUPT_CLEAR_3_SWAY_2_SHFT                             0x0

#define BIMC_INTERRUPT_ENABLE_3_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x0000013c)
#define BIMC_INTERRUPT_ENABLE_3_RMSK                                  0x1f
#define BIMC_INTERRUPT_ENABLE_3_IN(b)       \
        in_dword_masked(BIMC_INTERRUPT_ENABLE_3_ADDR(b), BIMC_INTERRUPT_ENABLE_3_RMSK)
#define BIMC_INTERRUPT_ENABLE_3_INM(b,m)   \
        in_dword_masked(BIMC_INTERRUPT_ENABLE_3_ADDR(b), m)
#define BIMC_INTERRUPT_ENABLE_3_OUT(b,v)    \
        out_dword(BIMC_INTERRUPT_ENABLE_3_ADDR(b),v)
#define BIMC_INTERRUPT_ENABLE_3_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_INTERRUPT_ENABLE_3_ADDR(b),m,v,BIMC_INTERRUPT_ENABLE_3_IN(b))
#define BIMC_INTERRUPT_ENABLE_3_SWAY_2_BMSK                           0x1f
#define BIMC_INTERRUPT_ENABLE_3_SWAY_2_SHFT                            0x0

#define BIMC_REF_TIMER_INTERVAL_ADDR(b)                           (BIMC_GLOBAL1_REG_BASE(b) + 0x00000200)
#define BIMC_REF_TIMER_INTERVAL_RMSK                                0xffff
#define BIMC_REF_TIMER_INTERVAL_IN(b)       \
        in_dword_masked(BIMC_REF_TIMER_INTERVAL_ADDR(b), BIMC_REF_TIMER_INTERVAL_RMSK)
#define BIMC_REF_TIMER_INTERVAL_INM(b,m)   \
        in_dword_masked(BIMC_REF_TIMER_INTERVAL_ADDR(b), m)
#define BIMC_REF_TIMER_INTERVAL_OUT(b,v)    \
        out_dword(BIMC_REF_TIMER_INTERVAL_ADDR(b),v)
#define BIMC_REF_TIMER_INTERVAL_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_REF_TIMER_INTERVAL_ADDR(b),m,v,BIMC_REF_TIMER_INTERVAL_IN(b))
#define BIMC_REF_TIMER_INTERVAL_INTERVAL_BMSK                       0xffff
#define BIMC_REF_TIMER_INTERVAL_INTERVAL_SHFT                          0x0

#define BIMC_DEBUG_SELECT_ADDR(b)                             (BIMC_GLOBAL1_REG_BASE(b) + 0x00000210)
#define BIMC_DEBUG_SELECT_RMSK                                  0xffffffff
#define BIMC_DEBUG_SELECT_IN(b)       \
        in_dword_masked(BIMC_DEBUG_SELECT_ADDR(b), BIMC_DEBUG_SELECT_RMSK)
#define BIMC_DEBUG_SELECT_INM(b,m)   \
        in_dword_masked(BIMC_DEBUG_SELECT_ADDR(b), m)
#define BIMC_DEBUG_SELECT_OUT(b,v)    \
        out_dword(BIMC_DEBUG_SELECT_ADDR(b),v)
#define BIMC_DEBUG_SELECT_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_DEBUG_SELECT_ADDR(b),m,v,BIMC_DEBUG_SELECT_IN(b))
#define BIMC_DEBUG_SELECT_COMPONENT_BMSK                        0xff000000
#define BIMC_DEBUG_SELECT_COMPONENT_SHFT                              0x18
#define BIMC_DEBUG_SELECT_INSTANCE_BMSK                           0xff0000
#define BIMC_DEBUG_SELECT_INSTANCE_SHFT                               0x10
#define BIMC_DEBUG_SELECT_SELECT_BMSK                               0xffff
#define BIMC_DEBUG_SELECT_SELECT_SHFT                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_GLOBAL2
 *--------------------------------------------------------------------------*/

#define BIMC_GLOBAL2_REG_BASE(b)                                         ((b)      + 0x00002000)

#define BIMC_GLOBAL2_COMPONENT_INFO_ADDR(b)                         (BIMC_GLOBAL2_REG_BASE(b) + 0x00000000)
#define BIMC_GLOBAL2_COMPONENT_INFO_RMSK                             0xffff
#define BIMC_GLOBAL2_COMPONENT_INFO_IN(b)       \
        in_dword_masked(BIMC_GLOBAL2_COMPONENT_INFO_ADDR(b), BIMC_GLOBAL2_COMPONENT_INFO_RMSK)
#define BIMC_GLOBAL2_COMPONENT_INFO_INM(b,m)   \
        in_dword_masked(BIMC_GLOBAL2_COMPONENT_INFO_ADDR(b), m)
#define BIMC_GLOBAL2_COMPONENT_INFO_SUB_TYPE_BMSK                    0xff00
#define BIMC_GLOBAL2_COMPONENT_INFO_SUB_TYPE_SHFT                       0x8
#define BIMC_GLOBAL2_COMPONENT_INFO_TYPE_BMSK                          0xff
#define BIMC_GLOBAL2_COMPONENT_INFO_TYPE_SHFT                           0x0

#define BIMC_DEFAULT_SEGMENT_ADDR(b)                                (BIMC_GLOBAL2_REG_BASE(b) + 0x000001f0)
#define BIMC_DEFAULT_SEGMENT_RMSK                                0x800f000f
#define BIMC_DEFAULT_SEGMENT_IN(b)       \
        in_dword_masked(BIMC_DEFAULT_SEGMENT_ADDR(b), BIMC_DEFAULT_SEGMENT_RMSK)
#define BIMC_DEFAULT_SEGMENT_INM(m)      \
        in_dword_masked(BIMC_DEFAULT_SEGMENT_ADDR(b), m)
#define BIMC_DEFAULT_SEGMENT_OUT(b,v)    \
        out_dword(BIMC_DEFAULT_SEGMENT_ADDR(b),v)
#define BIMC_DEFAULT_SEGMENT_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_DEFAULT_SEGMENT_ADDR(b),m,v,BIMC_DEFAULT_SEGMENT_IN(b))
#define BIMC_DEFAULT_SEGMENT_REDIRECT_EN_BMSK                    0x80000000
#define BIMC_DEFAULT_SEGMENT_REDIRECT_EN_SHFT                          0x1f
#define BIMC_DEFAULT_SEGMENT_REDIRECT_BMSK                          0xf0000
#define BIMC_DEFAULT_SEGMENT_REDIRECT_SHFT                             0x10
#define BIMC_DEFAULT_SEGMENT_DEFAULT_BMSK                               0xf
#define BIMC_DEFAULT_SEGMENT_DEFAULT_SHFT                               0x0

#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_ADDR(b,n,m)                 (BIMC_GLOBAL2_REG_BASE(b) + 0x00000200 + 0x80 * (n) + 0x10 * (m))
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_RMSK                        0xfff0040f
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_MAXn                                 4
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_INI2(b,n,m)        \
        in_dword_masked(BIMC_SEGMENTn_ADDR_BASEm_LOWER_ADDR(b,n,m), BIMC_SEGMENTn_ADDR_BASEm_LOWER_RMSK)
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_INMI2(b,n,m,mask)    \
        in_dword_masked(BIMC_SEGMENTn_ADDR_BASEm_LOWER_ADDR(b,n,m), (mask))
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_OUTI2(b,n,m,val)    \
        out_dword(BIMC_SEGMENTn_ADDR_BASEm_LOWER_ADDR(b,n,m),val)
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_OUTMI2(b,n,m,mask,val) \
        out_dword_masked_ns(BIMC_SEGMENTn_ADDR_BASEm_LOWER_ADDR(b,n,m),mask,val,BIMC_SEGMENTn_ADDR_BASEm_LOWER_INI2(b,n,m))
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_BASE_31_20_BMSK             0xfff00000
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_BASE_31_20_SHFT                   0x14
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_BASE_10_BMSK                     0x400
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_BASE_10_SHFT                       0xa
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_TYPE_BMSK                          0x2
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_TYPE_SHFT                          0x1
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_ENABLE_BMSK                        0x1
#define BIMC_SEGMENTn_ADDR_BASEm_LOWER_ENABLE_SHFT                        0x0

#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_ADDR(b,n,m)                 (BIMC_GLOBAL2_REG_BASE(b) + 0x00000204 + 0x80 * (n) + 0x10 * (m))
#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_RMSK                               0xf
#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_MAXn                                 4
#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_INI2(b,n,m)        \
        in_dword_masked(BIMC_SEGMENTn_ADDR_BASEm_UPPER_ADDR(b,n,m), BIMC_SEGMENTn_ADDR_BASEm_UPPER_RMSK)
#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_INMI2(b,n,m,mask)    \
        in_dword_masked(BIMC_SEGMENTn_ADDR_BASEm_UPPER_ADDR(b,n,m), (mask))
#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_OUTI2(b,n,m,val)    \
        out_dword(BIMC_SEGMENTn_ADDR_BASEm_UPPER_ADDR(b,n,m),val)
#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_OUTMI2(b,n,m,mask,val) \
        out_dword_masked_ns(BIMC_SEGMENTn_ADDR_BASEm_UPPER_ADDR(b,n,m),mask,val,BIMC_SEGMENTn_ADDR_BASEm_UPPER_INI2(b,n,m))
#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_BASE_35_32_BMSK                    0xf
#define BIMC_SEGMENTn_ADDR_BASEm_UPPER_BASE_35_32_SHFT                    0x0

#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR(b,n,m)                (BIMC_GLOBAL2_REG_BASE(b) + 0x00000208 + 0x80 * (n) + 0x10 * (m))
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK                        0xfff00400
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_MAXn                                 4
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_INI2(b,n,m)        \
        in_dword_masked(BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR(b,n,m), BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK)
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_INMI2(b,n,m,mask)    \
        in_dword_masked(BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR(b,n,m), (mask))
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_OUTI2(b,n,m,val)    \
        out_dword(BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR(b,n,m),val)
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_OUTMI2(b,n,m,mask,val) \
        out_dword_masked_ns(BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR(b,n,m),mask,val,BIMC_SEGMENTn_ADDR_MASKm_LOWER_INI2(b,n,m))
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_MASK_31_20_BMSK             0xfff00000
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_MASK_31_20_SHFT                   0x14
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_MASK_10_BMSK                     0x400
#define BIMC_SEGMENTn_ADDR_MASKm_LOWER_MASK_10_SHFT                       0xa

#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_ADDR(b,n,m)                  (BIMC_GLOBAL2_REG_BASE(b) + 0x0000020c + 0x80 * (n) + 0x10 * (m))
#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_RMSK                               0xf
#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_MAXn                                 4
#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_INI2(b,n,m)        \
        in_dword_masked(BIMC_SEGMENTn_ADDR_MASKm_UPPER_ADDR(b,n,m), BIMC_SEGMENTn_ADDR_MASKm_UPPER_RMSK)
#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_INMI2(b,n,m,mask)    \
        in_dword_masked(BIMC_SEGMENTn_ADDR_MASKm_UPPER_ADDR(b,n,m), (mask))
#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_OUTI2(b,n,m,val)    \
        out_dword(BIMC_SEGMENTn_ADDR_MASKm_UPPER_ADDR(b,n,m),val)
#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_OUTMI2(b,n,m,mask,val) \
        out_dword_masked_ns(BIMC_SEGMENTn_ADDR_MASKm_UPPER_ADDR(b,n,m),mask,val,BIMC_SEGMENTn_ADDR_MASKm_UPPER_INI2(b,n,m))
#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_MASK_35_32_BMSK                    0xf
#define BIMC_SEGMENTn_ADDR_MASKm_UPPER_MASK_35_32_SHFT                    0x0

#define BIMC_REMAPn_ADDR_BASE_LOWER_ADDR(b,n)                      (BIMC_GLOBAL2_REG_BASE(b)   + 0x00000a00 + 0x40 * (n))
#define BIMC_REMAPn_ADDR_BASE_LOWER_RMSK                         0xf000040f
#define BIMC_REMAPn_ADDR_BASE_LOWER_MAXn                                  3
#define BIMC_REMAPn_ADDR_BASE_LOWER_INI(b,n)        \
        in_dword_masked(BIMC_REMAPn_ADDR_BASE_LOWER_ADDR(b,n), BIMC_REMAPn_ADDR_BASE_LOWER_RMSK)
#define BIMC_REMAPn_ADDR_BASE_LOWER_INMI(b,n,mask)    \
        in_dword_masked(BIMC_REMAPn_ADDR_BASE_LOWER_ADDR(b,n), mask)
#define BIMC_REMAPn_ADDR_BASE_LOWER_OUTI(b,n,val)    \
        out_dword(BIMC_REMAPn_ADDR_BASE_LOWER_ADDR(b,n),val)
#define BIMC_REMAPn_ADDR_BASE_LOWER_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_REMAPn_ADDR_BASE_LOWER_ADDR(b,n),mask,val,BIMC_REMAPn_ADDR_BASE_LOWER_INI(b,n))
#define BIMC_REMAPn_ADDR_BASE_LOWER_BASE_31_28_BMSK              0xf0000000
#define BIMC_REMAPn_ADDR_BASE_LOWER_BASE_31_28_SHFT                    0x1c
#define BIMC_REMAPn_ADDR_BASE_LOWER_BASE_10_BMSK                      0x400
#define BIMC_REMAPn_ADDR_BASE_LOWER_BASE_10_SHFT                        0xa
#define BIMC_REMAPn_ADDR_BASE_LOWER_RFU_BMSK                            0xe
#define BIMC_REMAPn_ADDR_BASE_LOWER_RFU_SHFT                            0x1
#define BIMC_REMAPn_ADDR_BASE_LOWER_ENABLE_BMSK                         0x1
#define BIMC_REMAPn_ADDR_BASE_LOWER_ENABLE_SHFT                         0x0

#define BIMC_REMAPn_ADDR_BASE_UPPER_ADDR(b,n)                      (BIMC_GLOBAL2_REG_BASE(b)   + 0x00000a04 + 0x40 * (n))
#define BIMC_REMAPn_ADDR_BASE_UPPER_RMSK                                0xf
#define BIMC_REMAPn_ADDR_BASE_UPPER_MAXn                                  3
#define BIMC_REMAPn_ADDR_BASE_UPPER_INI(b,n)        \
        in_dword_masked(BIMC_REMAPn_ADDR_BASE_UPPER_ADDR(b,n), BIMC_REMAPn_ADDR_BASE_UPPER_RMSK)
#define BIMC_REMAPn_ADDR_BASE_UPPER_INMI(b,n,mask)    \
        in_dword_masked(BIMC_REMAPn_ADDR_BASE_UPPER_ADDR(b,n), mask)
#define BIMC_REMAPn_ADDR_BASE_UPPER_OUTI(b,n,val)    \
        out_dword(BIMC_REMAPn_ADDR_BASE_UPPER_ADDR(b,n),val)
#define BIMC_REMAPn_ADDR_BASE_UPPER_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_REMAPn_ADDR_BASE_UPPER_ADDR(b,n),mask,val,BIMC_REMAPn_ADDR_BASE_UPPER_INI(b,n))
#define BIMC_REMAPn_ADDR_BASE_UPPER_BASE_35_32_BMSK                     0xf
#define BIMC_REMAPn_ADDR_BASE_UPPER_BASE_35_32_SHFT                     0x0

#define BIMC_REMAPn_ADDR_MASK_LOWER_ADDR(b,n)                      (BIMC_GLOBAL2_REG_BASE(b)   + 0x00000a08 + 0x40 * (n))
#define BIMC_REMAPn_ADDR_MASK_LOWER_RMSK                         0xf0000400
#define BIMC_REMAPn_ADDR_MASK_LOWER_MAXn                                  3
#define BIMC_REMAPn_ADDR_MASK_LOWER_INI(b,n)        \
        in_dword_masked(BIMC_REMAPn_ADDR_MASK_LOWER_ADDR(b,n), BIMC_REMAPn_ADDR_MASK_LOWER_RMSK)
#define BIMC_REMAPn_ADDR_MASK_LOWER_INMI(b,n,mask)    \
        in_dword_masked(BIMC_REMAPn_ADDR_MASK_LOWER_ADDR(b,n), mask)
#define BIMC_REMAPn_ADDR_MASK_LOWER_OUTI(b,n,val)    \
        out_dword(BIMC_REMAPn_ADDR_MASK_LOWER_ADDR(b,n),val)
#define BIMC_REMAPn_ADDR_MASK_LOWER_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_REMAPn_ADDR_MASK_LOWER_ADDR(b,n),mask,val,BIMC_REMAPn_ADDR_MASK_LOWER_INI(b,n))
#define BIMC_REMAPn_ADDR_MASK_LOWER_MASK_31_28_BMSK              0xf0000000
#define BIMC_REMAPn_ADDR_MASK_LOWER_MASK_31_28_SHFT                    0x1c
#define BIMC_REMAPn_ADDR_MASK_LOWER_MASK_10_BMSK                      0x400
#define BIMC_REMAPn_ADDR_MASK_LOWER_MASK_10_SHFT                        0xa

#define BIMC_REMAPn_ADDR_MASK_UPPER_ADDR(b,n)                      (BIMC_GLOBAL2_REG_BASE(b)   + 0x00000a0c + 0x40 * (n))
#define BIMC_REMAPn_ADDR_MASK_UPPER_RMSK                                0xf
#define BIMC_REMAPn_ADDR_MASK_UPPER_MAXn                                  3
#define BIMC_REMAPn_ADDR_MASK_UPPER_INI(b,n)        \
        in_dword_masked(BIMC_REMAPn_ADDR_MASK_UPPER_ADDR(b,n), BIMC_REMAPn_ADDR_MASK_UPPER_RMSK)
#define BIMC_REMAPn_ADDR_MASK_UPPER_INMI(b,n,mask)    \
        in_dword_masked(BIMC_REMAPn_ADDR_MASK_UPPER_ADDR(b,n), mask)
#define BIMC_REMAPn_ADDR_MASK_UPPER_OUTI(b,n,val)    \
        out_dword(BIMC_REMAPn_ADDR_MASK_UPPER_ADDR(b,n),val)
#define BIMC_REMAPn_ADDR_MASK_UPPER_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_REMAPn_ADDR_MASK_UPPER_ADDR(b,n),mask,val,BIMC_REMAPn_ADDR_MASK_UPPER_INI(b,n))
#define BIMC_REMAPn_ADDR_MASK_UPPER_MASK_35_32_BMSK                     0xf
#define BIMC_REMAPn_ADDR_MASK_UPPER_MASK_35_32_SHFT                     0x0

#define BIMC_REMAPn_OP0_ADDR(b,n)                                  (BIMC_GLOBAL2_REG_BASE(b)   + 0x00000a20 + 0x40 * (n))
#define BIMC_REMAPn_OP0_RMSK                                       0xff0011
#define BIMC_REMAPn_OP0_MAXn                                              3
#define BIMC_REMAPn_OP0_INI(b,n)        \
        in_dword_masked(BIMC_REMAPn_OP0_ADDR(b,n), BIMC_REMAPn_OP0_RMSK)
#define BIMC_REMAPn_OP0_INMI(b,n,mask)    \
        in_dword_masked(BIMC_REMAPn_OP0_ADDR(b,n), mask)
#define BIMC_REMAPn_OP0_OUTI(b,n,val)    \
        out_dword(BIMC_REMAPn_OP0_ADDR(b,n),val)
#define BIMC_REMAPn_OP0_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_REMAPn_OP0_ADDR(b,n),mask,val,BIMC_REMAPn_OP0_INI(b,n))
#define BIMC_REMAPn_OP0_OPERAND_BMSK                               0xff0000
#define BIMC_REMAPn_OP0_OPERAND_SHFT                                   0x10
#define BIMC_REMAPn_OP0_OPCODE_BMSK                                    0x10
#define BIMC_REMAPn_OP0_OPCODE_SHFT                                     0x4
#define BIMC_REMAPn_OP0_OPCODE_ENABLE_BMSK                              0x1
#define BIMC_REMAPn_OP0_OPCODE_ENABLE_SHFT                              0x0

#define BIMC_REMAPn_OP1_ADDR(b,n)                                  (BIMC_GLOBAL2_REG_BASE(b)   + 0x00000a24 + 0x40 * (n))
#define BIMC_REMAPn_OP1_RMSK                                       0xff0011
#define BIMC_REMAPn_OP1_MAXn                                              3
#define BIMC_REMAPn_OP1_INI(b,n)        \
        in_dword_masked(BIMC_REMAPn_OP1_ADDR(b,n), BIMC_REMAPn_OP1_RMSK)
#define BIMC_REMAPn_OP1_INMI(b,n,mask)    \
        in_dword_masked(BIMC_REMAPn_OP1_ADDR(b,n), mask)
#define BIMC_REMAPn_OP1_OUTI(b,n,val)    \
        out_dword(BIMC_REMAPn_OP1_ADDR(b,n),val)
#define BIMC_REMAPn_OP1_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(BIMC_REMAPn_OP1_ADDR(b,n),mask,val,BIMC_REMAPn_OP1_INI(b,n))
#define BIMC_REMAPn_OP1_OPERAND_BMSK                               0xff0000
#define BIMC_REMAPn_OP1_OPERAND_SHFT                                   0x10
#define BIMC_REMAPn_OP1_OPCODE_BMSK                                    0x10
#define BIMC_REMAPn_OP1_OPCODE_SHFT                                     0x4
#define BIMC_REMAPn_OP1_OPCODE_ENABLE_BMSK                              0x1
#define BIMC_REMAPn_OP1_OPCODE_ENABLE_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_M_GENERIC
 *--------------------------------------------------------------------------*/

#define BIMC_M_REG_BASE(b)                                                       ((b)      + 0x00008000)

#define BIMC_M_COMPONENT_INFO_ADDR(b,n)                                     (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000000)
#define BIMC_M_COMPONENT_INFO_RMSK                                          0xffffff
#define BIMC_M_COMPONENT_INFO_INI(b,n)    \
        in_dword_masked(BIMC_M_COMPONENT_INFO_ADDR(b,n), BIMC_M_COMPONENT_INFO_RMSK)
#define BIMC_M_COMPONENT_INFO_INMI(b,n,m) \
        in_dword_masked(BIMC_M_COMPONENT_INFO_ADDR(b,n), m)
#define BIMC_M_COMPONENT_INFO_INSTANCE_BMSK                                 0xff0000
#define BIMC_M_COMPONENT_INFO_INSTANCE_SHFT                                     0x10
#define BIMC_M_COMPONENT_INFO_SUB_TYPE_BMSK                                   0xff00
#define BIMC_M_COMPONENT_INFO_SUB_TYPE_SHFT                                      0x8
#define BIMC_M_COMPONENT_INFO_TYPE_BMSK                                         0xff
#define BIMC_M_COMPONENT_INFO_TYPE_SHFT                                          0x0

#define BIMC_M_CONFIGURATION_INFO_0_ADDR(b,n)                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000020)
#define BIMC_M_CONFIGURATION_INFO_0_RMSK                                  0xff00ffff
#define BIMC_M_CONFIGURATION_INFO_0_INI(b,n)    \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_0_ADDR(b,n), BIMC_M_CONFIGURATION_INFO_0_RMSK)
#define BIMC_M_CONFIGURATION_INFO_0_INMI(b,n,m) \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_0_ADDR(b,n), m)
#define BIMC_M_CONFIGURATION_INFO_0_SYNC_MODE_BMSK                        0xff000000
#define BIMC_M_CONFIGURATION_INFO_0_SYNC_MODE_SHFT                              0x18
#define BIMC_M_CONFIGURATION_INFO_0_CONNECTION_TYPE_BMSK                      0xff00
#define BIMC_M_CONFIGURATION_INFO_0_CONNECTION_TYPE_SHFT                         0x8
#define BIMC_M_CONFIGURATION_INFO_0_FUNCTIONALITY_BMSK                          0xff
#define BIMC_M_CONFIGURATION_INFO_0_FUNCTIONALITY_SHFT                           0x0

#define BIMC_M_CONFIGURATION_INFO_1_ADDR(b,n)                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000030)
#define BIMC_M_CONFIGURATION_INFO_1_RMSK                                  0xffffffff
#define BIMC_M_CONFIGURATION_INFO_1_INI(b,n)    \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_1_ADDR(b,n), BIMC_M_CONFIGURATION_INFO_1_RMSK)
#define BIMC_M_CONFIGURATION_INFO_1_INMI(b,n,m) \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_1_ADDR(b,n), m)
#define BIMC_M_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_BMSK                0xffffffff
#define BIMC_M_CONFIGURATION_INFO_1_SWAY_CONNECTIVITY_SHFT                       0x0

#define BIMC_M_CONFIGURATION_INFO_2_ADDR(b,n)                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000040)
#define BIMC_M_CONFIGURATION_INFO_2_RMSK                                  0xffffffff
#define BIMC_M_CONFIGURATION_INFO_2_INI(b,n)    \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_2_ADDR(b,n), BIMC_M_CONFIGURATION_INFO_2_RMSK)
#define BIMC_M_CONFIGURATION_INFO_2_INMI(b,n,m) \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_2_ADDR(b,n), m)
#define BIMC_M_CONFIGURATION_INFO_2_M_DATA_WIDTH_BMSK                     0xffff0000
#define BIMC_M_CONFIGURATION_INFO_2_M_DATA_WIDTH_SHFT                           0x10
#define BIMC_M_CONFIGURATION_INFO_2_M_TID_WIDTH_BMSK                          0xff00
#define BIMC_M_CONFIGURATION_INFO_2_M_TID_WIDTH_SHFT                             0x8
#define BIMC_M_CONFIGURATION_INFO_2_M_MID_WIDTH_BMSK                            0xff
#define BIMC_M_CONFIGURATION_INFO_2_M_MID_WIDTH_SHFT                             0x0

#define BIMC_M_CONFIGURATION_INFO_3_ADDR(b,n)                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000050)
#define BIMC_M_CONFIGURATION_INFO_3_RMSK                                  0xffffffff
#define BIMC_M_CONFIGURATION_INFO_3_INI(b,n)    \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_3_ADDR(b,n), BIMC_M_CONFIGURATION_INFO_3_RMSK)
#define BIMC_M_CONFIGURATION_INFO_3_INMI(b,n,m) \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_3_ADDR(b,n), m)
#define BIMC_M_CONFIGURATION_INFO_3_RCH_DEPTH_BMSK                        0xff000000
#define BIMC_M_CONFIGURATION_INFO_3_RCH_DEPTH_SHFT                              0x18
#define BIMC_M_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                          0xff0000
#define BIMC_M_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                              0x10
#define BIMC_M_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                            0xff00
#define BIMC_M_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                               0x8
#define BIMC_M_CONFIGURATION_INFO_3_ACH_DEPTH_BMSK                              0xff
#define BIMC_M_CONFIGURATION_INFO_3_ACH_DEPTH_SHFT                               0x0

#define BIMC_M_CONFIGURATION_INFO_4_ADDR(b,n)                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000060)
#define BIMC_M_CONFIGURATION_INFO_4_RMSK                                      0xffff
#define BIMC_M_CONFIGURATION_INFO_4_INI(b,n)    \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_4_ADDR(b,n), BIMC_M_CONFIGURATION_INFO_4_RMSK)
#define BIMC_M_CONFIGURATION_INFO_4_INMI(b,n,m) \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_4_ADDR(b,n), m)
#define BIMC_M_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_BMSK                 0xff00
#define BIMC_M_CONFIGURATION_INFO_4_REORDER_BUFFER_DEPTH_SHFT                    0x8
#define BIMC_M_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_BMSK                    0xff
#define BIMC_M_CONFIGURATION_INFO_4_REORDER_TABLE_DEPTH_SHFT                     0x0

#define BIMC_M_CONFIGURATION_INFO_5_ADDR(b,n)                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000070)
#define BIMC_M_CONFIGURATION_INFO_5_RMSK                                       0x111
#define BIMC_M_CONFIGURATION_INFO_5_INI(b,n)    \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_5_ADDR(b,n), BIMC_M_CONFIGURATION_INFO_5_RMSK)
#define BIMC_M_CONFIGURATION_INFO_5_INMI(b,n,m) \
        in_dword_masked(BIMC_M_CONFIGURATION_INFO_5_ADDR(b,n), m)
#define BIMC_M_CONFIGURATION_INFO_5_MP2ARB_PIPELINE_EN_BMSK                    0x100
#define BIMC_M_CONFIGURATION_INFO_5_MP2ARB_PIPELINE_EN_SHFT                      0x8
#define BIMC_M_CONFIGURATION_INFO_5_MPBUF_PIPELINE_EN_BMSK                      0x10
#define BIMC_M_CONFIGURATION_INFO_5_MPBUF_PIPELINE_EN_SHFT                       0x4
#define BIMC_M_CONFIGURATION_INFO_5_M2MP_PIPELINE_EN_BMSK                        0x1
#define BIMC_M_CONFIGURATION_INFO_5_M2MP_PIPELINE_EN_SHFT                        0x0

#define BIMC_M_INTERRUPT_STATUS_ADDR(b,n)                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000100)
#define BIMC_M_INTERRUPT_STATUS_RMSK                                             0x3
#define BIMC_M_INTERRUPT_STATUS_INI(b,n)    \
        in_dword_masked(BIMC_M_INTERRUPT_STATUS_ADDR(b,n), BIMC_M_INTERRUPT_STATUS_RMSK)
#define BIMC_M_INTERRUPT_STATUS_INMI(b,n,m) \
        in_dword_masked(BIMC_M_INTERRUPT_STATUS_ADDR(b,n), m)

#define BIMC_M_INTERRUPT_CLEAR_ADDR(b,n)                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000108)
#define BIMC_M_INTERRUPT_CLEAR_RMSK                                              0x3
#define BIMC_M_INTERRUPT_CLEAR_OUTI(b,n,v)    \
        out_dword(BIMC_M_INTERRUPT_CLEAR_ADDR(b,n),v)

#define BIMC_M_INTERRUPT_ENABLE_ADDR(b,n)                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x0000010c)
#define BIMC_M_INTERRUPT_ENABLE_RMSK                                             0x3
#define BIMC_M_INTERRUPT_ENABLE_INI(b,n)    \
        in_dword_masked(BIMC_M_INTERRUPT_ENABLE_ADDR(b,n), BIMC_M_INTERRUPT_ENABLE_RMSK)
#define BIMC_M_INTERRUPT_ENABLE_INMI(b,n,m) \
        in_dword_masked(BIMC_M_INTERRUPT_ENABLE_ADDR(b,n), m)
#define BIMC_M_INTERRUPT_ENABLE_OUTI(b,n,v)    \
        out_dword(BIMC_M_INTERRUPT_ENABLE_ADDR(b,n),v)
#define BIMC_M_INTERRUPT_ENABLE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_INTERRUPT_ENABLE_ADDR(b,n),m,v,BIMC_M_INTERRUPT_ENABLE_INI(b,n))

#define BIMC_M_CLOCK_CTRL_ADDR(b,n)                                         (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000200)
#define BIMC_M_CLOCK_CTRL_RMSK                                                   0x3
#define BIMC_M_CLOCK_CTRL_INI(b,n)    \
        in_dword_masked(BIMC_M_CLOCK_CTRL_ADDR(b,n), BIMC_M_CLOCK_CTRL_RMSK)
#define BIMC_M_CLOCK_CTRL_INMI(b,n,m) \
        in_dword_masked(BIMC_M_CLOCK_CTRL_ADDR(b,n), m)
#define BIMC_M_CLOCK_CTRL_OUTI(b,n,v)    \
        out_dword(BIMC_M_CLOCK_CTRL_ADDR(b,n),v)
#define BIMC_M_CLOCK_CTRL_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_CLOCK_CTRL_ADDR(b,n),m,v,BIMC_M_CLOCK_CTRL_INI(b,n))
#define BIMC_M_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_BMSK                            0x2
#define BIMC_M_CLOCK_CTRL_MASTER_CLOCK_GATING_EN_SHFT                            0x1
#define BIMC_M_CLOCK_CTRL_CORE_CLOCK_GATING_EN_BMSK                              0x1
#define BIMC_M_CLOCK_CTRL_CORE_CLOCK_GATING_EN_SHFT                              0x0

#define BIMC_M_MODE_ADDR(b,n)                                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000210)
#define BIMC_M_MODE_RMSK                                                  0xf00fff17
#define BIMC_M_MODE_INI(b,n)    \
        in_dword_masked(BIMC_M_MODE_ADDR(b,n), BIMC_M_MODE_RMSK)
#define BIMC_M_MODE_INMI(b,n,m) \
        in_dword_masked(BIMC_M_MODE_ADDR(b,n), m)
#define BIMC_M_MODE_OUTI(b,n,v)    \
        out_dword(BIMC_M_MODE_ADDR(b,n),v)
#define BIMC_M_MODE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_MODE_ADDR(b,n),m,v,BIMC_M_MODE_INI(b,n))
#define BIMC_M_MODE_WR_GATHER_BEATS_BMSK                                  0xf0000000
#define BIMC_M_MODE_WR_GATHER_BEATS_SHFT                                        0x1c
#define BIMC_M_MODE_RD_GATHER_BEATS_BMSK                                     0xf0000
#define BIMC_M_MODE_RD_GATHER_BEATS_SHFT                                        0x10
#define BIMC_M_MODE_RCSSH_BMSK                                                0x8000
#define BIMC_M_MODE_RCSSH_SHFT                                                   0xf
#define BIMC_M_MODE_RCOSH_BMSK                                                0x4000
#define BIMC_M_MODE_RCOSH_SHFT                                                   0xe
#define BIMC_M_MODE_RCISH_BMSK                                                0x2000
#define BIMC_M_MODE_RCISH_SHFT                                                   0xd
#define BIMC_M_MODE_RCNSH_BMSK                                                0x1000
#define BIMC_M_MODE_RCNSH_SHFT                                                   0xc
#define BIMC_M_MODE_RNCSSH_BMSK                                                0x800
#define BIMC_M_MODE_RNCSSH_SHFT                                                  0xb
#define BIMC_M_MODE_RNCOSH_BMSK                                                0x400
#define BIMC_M_MODE_RNCOSH_SHFT                                                  0xa
#define BIMC_M_MODE_RNCISH_BMSK                                                0x200
#define BIMC_M_MODE_RNCISH_SHFT                                                  0x9
#define BIMC_M_MODE_RNCNSH_BMSK                                                0x100
#define BIMC_M_MODE_RNCNSH_SHFT                                                  0x8
#define BIMC_M_MODE_NARROW_WRITES_BMSK                                          0x10
#define BIMC_M_MODE_NARROW_WRITES_SHFT                                           0x4
#define BIMC_M_MODE_FIODV_BMSK                                                   0x4
#define BIMC_M_MODE_FIODV_SHFT                                                   0x2
#define BIMC_M_MODE_FIOSO_BMSK                                                   0x2
#define BIMC_M_MODE_FIOSO_SHFT                                                   0x1
#define BIMC_M_MODE_ORDERING_MODEL_BMSK                                          0x1
#define BIMC_M_MODE_ORDERING_MODEL_SHFT                                          0x0

#define BIMC_M_WR_WAY_CROSSING_ADDR(b)                                    (BIMC_M_REG_BASE(b)   + 0x00000220)
#define BIMC_M_WR_WAY_CROSSING_RMSK                                             0x1f
#define BIMC_M_WR_WAY_CROSSING_IN(b)       \
        in_dword_masked(BIMC_M_WR_WAY_CROSSING_ADDR(b), BIMC_M_WR_WAY_CROSSING_RMSK)
#define BIMC_M_WR_WAY_CROSSING_INM(b,m)    \
        in_dword_masked(BIMC_M_WR_WAY_CROSSING_ADDR(b), m)
#define BIMC_M_WR_WAY_CROSSING_OUT(b,v)    \
        out_dword(BIMC_M_WR_WAY_CROSSING_ADDR(b),v)
#define BIMC_M_WR_WAY_CROSSING_OUTM(b,m,v) \
        out_dword_masked_ns(BIMC_M_WR_WAY_CROSSING_ADDR(b),m,v,BIMC_M_WR_WAY_CROSSING_IN(b))
#define BIMC_M_WR_WAY_CROSSING_REG_BMSK                                         0x1f
#define BIMC_M_WR_WAY_CROSSING_REG_SHFT                                          0x0

#define BIMC_M_PRIORITYLVL_OVERRIDE_ADDR(b,n)                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000230)
#define BIMC_M_PRIORITYLVL_OVERRIDE_RMSK                                       0x301
#define BIMC_M_PRIORITYLVL_OVERRIDE_INI(b,n)    \
        in_dword_masked(BIMC_M_PRIORITYLVL_OVERRIDE_ADDR(b,n), BIMC_M_PRIORITYLVL_OVERRIDE_RMSK)
#define BIMC_M_PRIORITYLVL_OVERRIDE_INMI(b,n,m) \
        in_dword_masked(BIMC_M_PRIORITYLVL_OVERRIDE_ADDR(b,n), m)
#define BIMC_M_PRIORITYLVL_OVERRIDE_OUTI(b,n,v)    \
        out_dword(BIMC_M_PRIORITYLVL_OVERRIDE_ADDR(b,n),v)
#define BIMC_M_PRIORITYLVL_OVERRIDE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_PRIORITYLVL_OVERRIDE_ADDR(b,n),m,v,BIMC_M_PRIORITYLVL_OVERRIDE_INI(b,n))
#define BIMC_M_PRIORITYLVL_OVERRIDE_PRIORITYLVL_BMSK                           0x300
#define BIMC_M_PRIORITYLVL_OVERRIDE_PRIORITYLVL_SHFT                             0x8
#define BIMC_M_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_BMSK                    0x1
#define BIMC_M_PRIORITYLVL_OVERRIDE_OVERRIDE_PRIORITYLVL_SHFT                    0x0

#define BIMC_M_READ_COMMAND_OVERRIDE_ADDR(b,n)                              (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000240)
#define BIMC_M_READ_COMMAND_OVERRIDE_RMSK                                  0x3071f7f
#define BIMC_M_READ_COMMAND_OVERRIDE_INI(b,n)    \
        in_dword_masked(BIMC_M_READ_COMMAND_OVERRIDE_ADDR(b,n), BIMC_M_READ_COMMAND_OVERRIDE_RMSK)
#define BIMC_M_READ_COMMAND_OVERRIDE_INMI(b,n,m) \
        in_dword_masked(BIMC_M_READ_COMMAND_OVERRIDE_ADDR(b,n), m)
#define BIMC_M_READ_COMMAND_OVERRIDE_OUTI(b,n,v)    \
        out_dword(BIMC_M_READ_COMMAND_OVERRIDE_ADDR(b,n),v)
#define BIMC_M_READ_COMMAND_OVERRIDE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_READ_COMMAND_OVERRIDE_ADDR(b,n),m,v,BIMC_M_READ_COMMAND_OVERRIDE_INI(b,n))
#define BIMC_M_READ_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                     0x3000000
#define BIMC_M_READ_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                          0x18
#define BIMC_M_READ_COMMAND_OVERRIDE_AMEMTYPE_BMSK                           0x70000
#define BIMC_M_READ_COMMAND_OVERRIDE_AMEMTYPE_SHFT                              0x10
#define BIMC_M_READ_COMMAND_OVERRIDE_ATRANSIENT_BMSK                          0x1000
#define BIMC_M_READ_COMMAND_OVERRIDE_ATRANSIENT_SHFT                             0xc
#define BIMC_M_READ_COMMAND_OVERRIDE_ASHARED_BMSK                              0x800
#define BIMC_M_READ_COMMAND_OVERRIDE_ASHARED_SHFT                                0xb
#define BIMC_M_READ_COMMAND_OVERRIDE_AREDIRECT_BMSK                            0x400
#define BIMC_M_READ_COMMAND_OVERRIDE_AREDIRECT_SHFT                              0xa
#define BIMC_M_READ_COMMAND_OVERRIDE_AOOO_BMSK                                 0x200
#define BIMC_M_READ_COMMAND_OVERRIDE_AOOO_SHFT                                   0x9
#define BIMC_M_READ_COMMAND_OVERRIDE_AINNERSHARED_BMSK                         0x100
#define BIMC_M_READ_COMMAND_OVERRIDE_AINNERSHARED_SHFT                           0x8
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                 0x40
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                  0x6
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                     0x20
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                      0x5
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                   0x10
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                    0x4
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                       0x8
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                       0x3
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                     0x4
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                     0x2
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                          0x2
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                          0x1
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                  0x1
#define BIMC_M_READ_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                  0x0

#define BIMC_M_WRITE_COMMAND_OVERRIDE_ADDR(b,n)                             (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000250)
#define BIMC_M_WRITE_COMMAND_OVERRIDE_RMSK                                 0x3071f7f
#define BIMC_M_WRITE_COMMAND_OVERRIDE_INI(b,n)    \
        in_dword_masked(BIMC_M_WRITE_COMMAND_OVERRIDE_ADDR(b,n), BIMC_M_WRITE_COMMAND_OVERRIDE_RMSK)
#define BIMC_M_WRITE_COMMAND_OVERRIDE_INMI(b,n,m) \
        in_dword_masked(BIMC_M_WRITE_COMMAND_OVERRIDE_ADDR(b,n), m)
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OUTI(b,n,v)    \
        out_dword(BIMC_M_WRITE_COMMAND_OVERRIDE_ADDR(b,n),v)
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_WRITE_COMMAND_OVERRIDE_ADDR(b,n),m,v,BIMC_M_WRITE_COMMAND_OVERRIDE_INI(b,n))
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_BMSK                    0x3000000
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AREQPRIORITY_SHFT                         0x18
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AMEMTYPE_BMSK                          0x70000
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AMEMTYPE_SHFT                             0x10
#define BIMC_M_WRITE_COMMAND_OVERRIDE_ATRANSIENT_BMSK                         0x1000
#define BIMC_M_WRITE_COMMAND_OVERRIDE_ATRANSIENT_SHFT                            0xc
#define BIMC_M_WRITE_COMMAND_OVERRIDE_ASHARED_BMSK                             0x800
#define BIMC_M_WRITE_COMMAND_OVERRIDE_ASHARED_SHFT                               0xb
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AREDIRECT_BMSK                           0x400
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AREDIRECT_SHFT                             0xa
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AOOO_BMSK                                0x200
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AOOO_SHFT                                  0x9
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AINNERSHARED_BMSK                        0x100
#define BIMC_M_WRITE_COMMAND_OVERRIDE_AINNERSHARED_SHFT                          0x8
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_BMSK                0x40
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREQPRIORITY_SHFT                 0x6
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_BMSK                    0x20
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AMEMTYPE_SHFT                     0x5
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_BMSK                  0x10
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_ATRANSIENT_SHFT                   0x4
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_BMSK                      0x8
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_ASHARED_SHFT                      0x3
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_BMSK                    0x4
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AREDIRECT_SHFT                    0x2
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_BMSK                         0x2
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AOOO_SHFT                         0x1
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_BMSK                 0x1
#define BIMC_M_WRITE_COMMAND_OVERRIDE_OVERRIDE_AINNERSHARED_SHFT                 0x0

#define BIMC_M_PIPE0_GATHERING_ADDR(b,n)                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000260)
#define BIMC_M_PIPE0_GATHERING_RMSK                                          0xf000f
#define BIMC_M_PIPE0_GATHERING_IN(b,n)         \
        in_dword_masked(BIMC_M_PIPE0_GATHERING_ADDR(b,n), BIMC_M_PIPE0_GATHERING_RMSK)
#define BIMC_M_PIPE0_GATHERING_INM(b,n,m)      \
        in_dword_masked(BIMC_M_PIPE0_GATHERING_ADDR(b,n), m)
#define BIMC_M_PIPE0_GATHERING_OUT(b,n,v)      \
        out_dword(BIMC_M_PIPE0_GATHERING_ADDR(b,n),v)
#define BIMC_M_PIPE0_GATHERING_OUTM(b,n,m,v)   \
        out_dword_masked_ns(BIMC_M_PIPE0_GATHERING_ADDR(b,n),m,v,BIMC_M_PIPE0_GATHERING_IN(b,n))
#define BIMC_M_PIPE0_GATHERING_WR_GATHER_BEATS_BMSK                          0xf0000
#define BIMC_M_PIPE0_GATHERING_WR_GATHER_BEATS_SHFT                             0x10
#define BIMC_M_PIPE0_GATHERING_RD_GATHER_BEATS_BMSK                              0xf
#define BIMC_M_PIPE0_GATHERING_RD_GATHER_BEATS_SHFT                              0x0

#define BIMC_M_PIPE1_GATHERING_ADDR(b,n)                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000264)
#define BIMC_M_PIPE1_GATHERING_RMSK                                          0xf000f
#define BIMC_M_PIPE1_GATHERING_IN(b,n)         \
        in_dword_masked(BIMC_M_PIPE1_GATHERING_ADDR(b,n), BIMC_M_PIPE1_GATHERING_RMSK)
#define BIMC_M_PIPE1_GATHERING_INM(b,n,m)      \
        in_dword_masked(BIMC_M_PIPE1_GATHERING_ADDR(b,n), m)
#define BIMC_M_PIPE1_GATHERING_OUT(b,n,v)      \
        out_dword(BIMC_M_PIPE1_GATHERING_ADDR(b,n),v)
#define BIMC_M_PIPE1_GATHERING_OUTM(b,n,m,v)   \
        out_dword_masked_ns(BIMC_M_PIPE1_GATHERING_ADDR(b,n),m,v,BIMC_M_PIPE1_GATHERING_IN(b,n))
#define BIMC_M_PIPE1_GATHERING_WR_GATHER_BEATS_BMSK                          0xf0000
#define BIMC_M_PIPE1_GATHERING_WR_GATHER_BEATS_SHFT                             0x10
#define BIMC_M_PIPE1_GATHERING_RD_GATHER_BEATS_BMSK                              0xf
#define BIMC_M_PIPE1_GATHERING_RD_GATHER_BEATS_SHFT                              0x0

#define BIMC_M_PIPE2_GATHERING_ADDR(b,n)                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000268)
#define BIMC_M_PIPE2_GATHERING_RMSK                                          0xf000f
#define BIMC_M_PIPE2_GATHERING_IN(b,n)         \
        in_dword_masked(BIMC_M_PIPE2_GATHERING_ADDR(b,n), BIMC_M_PIPE2_GATHERING_RMSK)
#define BIMC_M_PIPE2_GATHERING_INM(b,n,m)      \
        in_dword_masked(BIMC_M_PIPE2_GATHERING_ADDR(b,n), m)
#define BIMC_M_PIPE2_GATHERING_OUT(b,n,v)      \
        out_dword(BIMC_M_PIPE2_GATHERING_ADDR(b,n),v)
#define BIMC_M_PIPE2_GATHERING_OUTM(b,n,m,v)   \
        out_dword_masked_ns(BIMC_M_PIPE2_GATHERING_ADDR(b,n),m,v,BIMC_M_PIPE2_GATHERING_IN(b,n))
#define BIMC_M_PIPE2_GATHERING_WR_GATHER_BEATS_BMSK                          0xf0000
#define BIMC_M_PIPE2_GATHERING_WR_GATHER_BEATS_SHFT                             0x10
#define BIMC_M_PIPE2_GATHERING_RD_GATHER_BEATS_BMSK                              0xf
#define BIMC_M_PIPE2_GATHERING_RD_GATHER_BEATS_SHFT                              0x0

#define BIMC_M_RD_TRACKING_INDEX_ADDR(b,n)                                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000270)
#define BIMC_M_RD_TRACKING_INDEX_RMSK                                                         0xfffff
#define BIMC_M_RD_TRACKING_INDEX_INI(b,n)      \
        in_dword_masked(BIMC_M_RD_TRACKING_INDEX_ADDR(b,n), BIMC_M_RD_TRACKING_INDEX_RMSK)
#define BIMC_M_RD_TRACKING_INDEX_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_RD_TRACKING_INDEX_ADDR(b,n), m)
#define BIMC_M_RD_TRACKING_INDEX_OUTI(b,n,v)      \
        out_dword(BIMC_M_RD_TRACKING_INDEX_ADDR(b,n),v)
#define BIMC_M_RD_TRACKING_INDEX_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_RD_TRACKING_INDEX_ADDR(b,n),m,v,BIMC_M_RD_TRACKING_INDEX_INI(b,n))
#define BIMC_M_RD_TRACKING_INDEX_INDEX_BMSK                                                   0xfffff
#define BIMC_M_RD_TRACKING_INDEX_INDEX_SHFT                                                       0x0

#define BIMC_M_WR_TRACKING_INDEX_ADDR(b,n)                                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000274)
#define BIMC_M_WR_TRACKING_INDEX_RMSK                                                         0xfffff
#define BIMC_M_WR_TRACKING_INDEX_INI(b,n)      \
        in_dword_masked(BIMC_M_WR_TRACKING_INDEX_ADDR(b,n), BIMC_M_WR_TRACKING_INDEX_RMSK)
#define BIMC_M_WR_TRACKING_INDEX_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_WR_TRACKING_INDEX_ADDR(b,n), m)
#define BIMC_M_WR_TRACKING_INDEX_OUTI(b,n,v)      \
        out_dword(BIMC_M_WR_TRACKING_INDEX_ADDR(b,n),v)
#define BIMC_M_WR_TRACKING_INDEX_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_WR_TRACKING_INDEX_ADDR(b,n),m,v,BIMC_M_WR_TRACKING_INDEX_INI(b,n))
#define BIMC_M_WR_TRACKING_INDEX_INDEX_BMSK                                                   0xfffff
#define BIMC_M_WR_TRACKING_INDEX_INDEX_SHFT                                                       0x0

#define BIMC_M_BKE_ENABLE_ADDR(b,n)                                         (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000300)
#define BIMC_M_BKE_ENABLE_RMSK                                                             0xff000001
#define BIMC_M_BKE_ENABLE_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_ENABLE_ADDR(b,n), BIMC_M_BKE_ENABLE_RMSK)
#define BIMC_M_BKE_ENABLE_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_ENABLE_ADDR(b,n), m)
#define BIMC_M_BKE_ENABLE_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_ENABLE_ADDR(b,n),v)
#define BIMC_M_BKE_ENABLE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_ENABLE_ADDR(b,n),m,v,BIMC_M_BKE_ENABLE_INI(b,n))
#define BIMC_M_BKE_ENABLE_QOS_POLICY_SEL_BMSK                             0xff000000
#define BIMC_M_BKE_ENABLE_QOS_POLICY_SEL_SHFT                                   0x18
#define BIMC_M_BKE_ENABLE_ENABLE_BMSK                                            0x1
#define BIMC_M_BKE_ENABLE_ENABLE_SHFT                                            0x0

#define BIMC_M_BKE_GRANT_PERIOD_ADDR(b,n)                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000304)
#define BIMC_M_BKE_GRANT_PERIOD_RMSK                                           0x3ff
#define BIMC_M_BKE_GRANT_PERIOD_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_GRANT_PERIOD_ADDR(b,n), BIMC_M_BKE_GRANT_PERIOD_RMSK)
#define BIMC_M_BKE_GRANT_PERIOD_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_GRANT_PERIOD_ADDR(b,n), m)
#define BIMC_M_BKE_GRANT_PERIOD_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_GRANT_PERIOD_ADDR(b,n),v)
#define BIMC_M_BKE_GRANT_PERIOD_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_GRANT_PERIOD_ADDR(b,n),m,v,BIMC_M_BKE_GRANT_PERIOD_INI(b,n))
#define BIMC_M_BKE_GRANT_PERIOD_GRANT_PERIOD_BMSK                              0x3ff
#define BIMC_M_BKE_GRANT_PERIOD_GRANT_PERIOD_SHFT                                0x0

#define BIMC_M_BKE_GRANT_COUNT_ADDR(b,n)                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000308)
#define BIMC_M_BKE_GRANT_COUNT_RMSK                                           0xffff
#define BIMC_M_BKE_GRANT_COUNT_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_GRANT_COUNT_ADDR(b,n), BIMC_M_BKE_GRANT_COUNT_RMSK)
#define BIMC_M_BKE_GRANT_COUNT_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_GRANT_COUNT_ADDR(b,n), m)
#define BIMC_M_BKE_GRANT_COUNT_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_GRANT_COUNT_ADDR(b,n),v)
#define BIMC_M_BKE_GRANT_COUNT_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_GRANT_COUNT_ADDR(b,n),m,v,BIMC_M_BKE_GRANT_COUNT_INI(b,n))
#define BIMC_M_BKE_GRANT_COUNT_GRANT_COUNT_BMSK                               0xffff
#define BIMC_M_BKE_GRANT_COUNT_GRANT_COUNT_SHFT                                  0x0

#define BIMC_M_BKE_THRESHOLD_HIGH_ADDR(b,n)                                 (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000320)
#define BIMC_M_BKE_THRESHOLD_HIGH_RMSK                                        0xffff
#define BIMC_M_BKE_THRESHOLD_HIGH_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_THRESHOLD_HIGH_ADDR(b,n), BIMC_M_BKE_THRESHOLD_HIGH_RMSK)
#define BIMC_M_BKE_THRESHOLD_HIGH_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_THRESHOLD_HIGH_ADDR(b,n), m)
#define BIMC_M_BKE_THRESHOLD_HIGH_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_THRESHOLD_HIGH_ADDR(b,n),v)
#define BIMC_M_BKE_THRESHOLD_HIGH_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_THRESHOLD_HIGH_ADDR(b,n),m,v,BIMC_M_BKE_THRESHOLD_HIGH_INI(b,n))
#define BIMC_M_BKE_THRESHOLD_HIGH_THRESHOLD_BMSK                              0xffff
#define BIMC_M_BKE_THRESHOLD_HIGH_THRESHOLD_SHFT                                 0x0

#define BIMC_M_BKE_THRESHOLD_MEDIUM_ADDR(b,n)                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000324)
#define BIMC_M_BKE_THRESHOLD_MEDIUM_RMSK                                      0xffff
#define BIMC_M_BKE_THRESHOLD_MEDIUM_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_THRESHOLD_MEDIUM_ADDR(b,n), BIMC_M_BKE_THRESHOLD_MEDIUM_RMSK)
#define BIMC_M_BKE_THRESHOLD_MEDIUM_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_THRESHOLD_MEDIUM_ADDR(b,n), m)
#define BIMC_M_BKE_THRESHOLD_MEDIUM_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_THRESHOLD_MEDIUM_ADDR(b,n),v)
#define BIMC_M_BKE_THRESHOLD_MEDIUM_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_THRESHOLD_MEDIUM_ADDR(b,n),m,v,BIMC_M_BKE_THRESHOLD_MEDIUM_INI(b,n))
#define BIMC_M_BKE_THRESHOLD_MEDIUM_THRESHOLD_BMSK                            0xffff
#define BIMC_M_BKE_THRESHOLD_MEDIUM_THRESHOLD_SHFT                               0x0

#define BIMC_M_BKE_THRESHOLD_LOW_ADDR(b,n)                                  (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000328)
#define BIMC_M_BKE_THRESHOLD_LOW_RMSK                                         0xffff
#define BIMC_M_BKE_THRESHOLD_LOW_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_THRESHOLD_LOW_ADDR(b,n), BIMC_M_BKE_THRESHOLD_LOW_RMSK)
#define BIMC_M_BKE_THRESHOLD_LOW_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_THRESHOLD_LOW_ADDR(b,n), m)
#define BIMC_M_BKE_THRESHOLD_LOW_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_THRESHOLD_LOW_ADDR(b,n),v)
#define BIMC_M_BKE_THRESHOLD_LOW_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_THRESHOLD_LOW_ADDR(b,n),m,v,BIMC_M_BKE_THRESHOLD_LOW_INI(b,n))
#define BIMC_M_BKE_THRESHOLD_LOW_THRESHOLD_BMSK                               0xffff
#define BIMC_M_BKE_THRESHOLD_LOW_THRESHOLD_SHFT                                  0x0

#define BIMC_M_BKE_HEALTH_0_CONFIG_ADDR(b,n)                                (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000340)
#define BIMC_M_BKE_HEALTH_0_CONFIG_RMSK                                   0x80000303
#define BIMC_M_BKE_HEALTH_0_CONFIG_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_HEALTH_0_CONFIG_ADDR(b,n), BIMC_M_BKE_HEALTH_0_CONFIG_RMSK)
#define BIMC_M_BKE_HEALTH_0_CONFIG_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_HEALTH_0_CONFIG_ADDR(b,n), m)
#define BIMC_M_BKE_HEALTH_0_CONFIG_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_HEALTH_0_CONFIG_ADDR(b,n),v)
#define BIMC_M_BKE_HEALTH_0_CONFIG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_HEALTH_0_CONFIG_ADDR(b,n),m,v,BIMC_M_BKE_HEALTH_0_CONFIG_INI(b,n))
#define BIMC_M_BKE_HEALTH_0_CONFIG_LIMIT_COMMANDS_BMSK                    0x80000000
#define BIMC_M_BKE_HEALTH_0_CONFIG_LIMIT_COMMANDS_SHFT                          0x1f
#define BIMC_M_BKE_HEALTH_0_CONFIG_AREQPRIORITY_BMSK                           0x300
#define BIMC_M_BKE_HEALTH_0_CONFIG_AREQPRIORITY_SHFT                             0x8
#define BIMC_M_BKE_HEALTH_0_CONFIG_PRIORITYLVL_BMSK                              0x3
#define BIMC_M_BKE_HEALTH_0_CONFIG_PRIORITYLVL_SHFT                              0x0

#define BIMC_M_BKE_HEALTH_1_CONFIG_ADDR(b,n)                                (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000344)
#define BIMC_M_BKE_HEALTH_1_CONFIG_RMSK                                   0x80000303
#define BIMC_M_BKE_HEALTH_1_CONFIG_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_HEALTH_1_CONFIG_ADDR(b,n), BIMC_M_BKE_HEALTH_1_CONFIG_RMSK)
#define BIMC_M_BKE_HEALTH_1_CONFIG_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_HEALTH_1_CONFIG_ADDR(b,n), m)
#define BIMC_M_BKE_HEALTH_1_CONFIG_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_HEALTH_1_CONFIG_ADDR(b,n),v)
#define BIMC_M_BKE_HEALTH_1_CONFIG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_HEALTH_1_CONFIG_ADDR(b,n),m,v,BIMC_M_BKE_HEALTH_1_CONFIG_INI(b,n))
#define BIMC_M_BKE_HEALTH_1_CONFIG_LIMIT_COMMANDS_BMSK                    0x80000000
#define BIMC_M_BKE_HEALTH_1_CONFIG_LIMIT_COMMANDS_SHFT                          0x1f
#define BIMC_M_BKE_HEALTH_1_CONFIG_AREQPRIORITY_BMSK                           0x300
#define BIMC_M_BKE_HEALTH_1_CONFIG_AREQPRIORITY_SHFT                             0x8
#define BIMC_M_BKE_HEALTH_1_CONFIG_PRIORITYLVL_BMSK                              0x3
#define BIMC_M_BKE_HEALTH_1_CONFIG_PRIORITYLVL_SHFT                              0x0

#define BIMC_M_BKE_HEALTH_2_CONFIG_ADDR(b,n)                                (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000348)
#define BIMC_M_BKE_HEALTH_2_CONFIG_RMSK                                   0x80000303
#define BIMC_M_BKE_HEALTH_2_CONFIG_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_HEALTH_2_CONFIG_ADDR(b,n), BIMC_M_BKE_HEALTH_2_CONFIG_RMSK)
#define BIMC_M_BKE_HEALTH_2_CONFIG_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_HEALTH_2_CONFIG_ADDR(b,n), m)
#define BIMC_M_BKE_HEALTH_2_CONFIG_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_HEALTH_2_CONFIG_ADDR(b,n),v)
#define BIMC_M_BKE_HEALTH_2_CONFIG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_HEALTH_2_CONFIG_ADDR(b,n),m,v,BIMC_M_BKE_HEALTH_2_CONFIG_INI(b,n))
#define BIMC_M_BKE_HEALTH_2_CONFIG_LIMIT_COMMANDS_BMSK                    0x80000000
#define BIMC_M_BKE_HEALTH_2_CONFIG_LIMIT_COMMANDS_SHFT                          0x1f
#define BIMC_M_BKE_HEALTH_2_CONFIG_AREQPRIORITY_BMSK                           0x300
#define BIMC_M_BKE_HEALTH_2_CONFIG_AREQPRIORITY_SHFT                             0x8
#define BIMC_M_BKE_HEALTH_2_CONFIG_PRIORITYLVL_BMSK                              0x3
#define BIMC_M_BKE_HEALTH_2_CONFIG_PRIORITYLVL_SHFT                              0x0

#define BIMC_M_BKE_HEALTH_3_CONFIG_ADDR(b,n)                                (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x0000034c)
#define BIMC_M_BKE_HEALTH_3_CONFIG_RMSK                                        0x303
#define BIMC_M_BKE_HEALTH_3_CONFIG_INI(b,n)    \
        in_dword_masked(BIMC_M_BKE_HEALTH_3_CONFIG_ADDR(b,n), BIMC_M_BKE_HEALTH_3_CONFIG_RMSK)
#define BIMC_M_BKE_HEALTH_3_CONFIG_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BKE_HEALTH_3_CONFIG_ADDR(b,n), m)
#define BIMC_M_BKE_HEALTH_3_CONFIG_OUTI(b,n,v)    \
        out_dword(BIMC_M_BKE_HEALTH_3_CONFIG_ADDR(b,n),v)
#define BIMC_M_BKE_HEALTH_3_CONFIG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE_HEALTH_3_CONFIG_ADDR(b,n),m,v,BIMC_M_BKE_HEALTH_3_CONFIG_INI(b,n))
#define BIMC_M_BKE_HEALTH_3_CONFIG_AREQPRIORITY_BMSK                           0x300
#define BIMC_M_BKE_HEALTH_3_CONFIG_AREQPRIORITY_SHFT                             0x8
#define BIMC_M_BKE_HEALTH_3_CONFIG_PRIORITYLVL_BMSK                              0x3
#define BIMC_M_BKE_HEALTH_3_CONFIG_PRIORITYLVL_SHFT                              0x0

#define BIMC_M_BUFFER_STATUS_0_ADDR(b,n)                                      (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000400)
#define BIMC_M_BUFFER_STATUS_0_RMSK                                          0xff333333
#define BIMC_M_BUFFER_STATUS_0_INI(b,n)    \
        in_dword_masked(BIMC_M_BUFFER_STATUS_0_ADDR(b,n), BIMC_M_BUFFER_STATUS_0_RMSK)
#define BIMC_M_BUFFER_STATUS_0_INMI(b,n,m) \
        in_dword_masked(BIMC_M_BUFFER_STATUS_0_ADDR(b,n), m)
#define BIMC_M_BUFFER_STATUS_0_RCB_DATA_RD_FULL_BMSK                      0x80000000
#define BIMC_M_BUFFER_STATUS_0_RCB_DATA_RD_FULL_SHFT                            0x1f
#define BIMC_M_BUFFER_STATUS_0_RCB_DATA_RD_EMPTY_BMSK                     0x40000000
#define BIMC_M_BUFFER_STATUS_0_RCB_DATA_RD_EMPTY_SHFT                           0x1e
#define BIMC_M_BUFFER_STATUS_0_RCB_CTRL_RD_FULL_BMSK                      0x20000000
#define BIMC_M_BUFFER_STATUS_0_RCB_CTRL_RD_FULL_SHFT                            0x1d
#define BIMC_M_BUFFER_STATUS_0_RCB_CTRL_RD_EMPTY_BMSK                     0x10000000
#define BIMC_M_BUFFER_STATUS_0_RCB_CTRL_RD_EMPTY_SHFT                           0x1c
#define BIMC_M_BUFFER_STATUS_0_RCB_DATA_WR_FULL_BMSK                       0x8000000
#define BIMC_M_BUFFER_STATUS_0_RCB_DATA_WR_FULL_SHFT                            0x1b
#define BIMC_M_BUFFER_STATUS_0_RCB_DATA_WR_EMPTY_BMSK                      0x4000000
#define BIMC_M_BUFFER_STATUS_0_RCB_DATA_WR_EMPTY_SHFT                           0x1a
#define BIMC_M_BUFFER_STATUS_0_RCB_CTRL_WR_FULL_BMSK                       0x2000000
#define BIMC_M_BUFFER_STATUS_0_RCB_CTRL_WR_FULL_SHFT                            0x19
#define BIMC_M_BUFFER_STATUS_0_RCB_CTRL_WR_EMPTY_BMSK                      0x1000000
#define BIMC_M_BUFFER_STATUS_0_RCB_CTRL_WR_EMPTY_SHFT                           0x18
#define BIMC_M_BUFFER_STATUS_0_BCB_RD_FULL_BMSK                             0x200000
#define BIMC_M_BUFFER_STATUS_0_BCB_RD_FULL_SHFT                                 0x15
#define BIMC_M_BUFFER_STATUS_0_BCB_RD_EMPTY_BMSK                            0x100000
#define BIMC_M_BUFFER_STATUS_0_BCB_RD_EMPTY_SHFT                                0x14
#define BIMC_M_BUFFER_STATUS_0_BCB_WR_FULL_BMSK                              0x20000
#define BIMC_M_BUFFER_STATUS_0_BCB_WR_FULL_SHFT                                 0x11
#define BIMC_M_BUFFER_STATUS_0_BCB_WR_EMPTY_BMSK                             0x10000
#define BIMC_M_BUFFER_STATUS_0_BCB_WR_EMPTY_SHFT                                0x10
#define BIMC_M_BUFFER_STATUS_0_WCB_RD_FULL_BMSK                               0x2000
#define BIMC_M_BUFFER_STATUS_0_WCB_RD_FULL_SHFT                                  0xd
#define BIMC_M_BUFFER_STATUS_0_WCB_RD_EMPTY_BMSK                              0x1000
#define BIMC_M_BUFFER_STATUS_0_WCB_RD_EMPTY_SHFT                                 0xc
#define BIMC_M_BUFFER_STATUS_0_WCB_WR_FULL_BMSK                                0x200
#define BIMC_M_BUFFER_STATUS_0_WCB_WR_FULL_SHFT                                  0x9
#define BIMC_M_BUFFER_STATUS_0_WCB_WR_EMPTY_BMSK                               0x100
#define BIMC_M_BUFFER_STATUS_0_WCB_WR_EMPTY_SHFT                                 0x8
#define BIMC_M_BUFFER_STATUS_0_ACB_RD_FULL_BMSK                                 0x20
#define BIMC_M_BUFFER_STATUS_0_ACB_RD_FULL_SHFT                                  0x5
#define BIMC_M_BUFFER_STATUS_0_ACB_RD_EMPTY_BMSK                                0x10
#define BIMC_M_BUFFER_STATUS_0_ACB_RD_EMPTY_SHFT                                 0x4
#define BIMC_M_BUFFER_STATUS_0_ACB_WR_FULL_BMSK                                  0x2
#define BIMC_M_BUFFER_STATUS_0_ACB_WR_FULL_SHFT                                  0x1
#define BIMC_M_BUFFER_STATUS_0_ACB_WR_EMPTY_BMSK                                 0x1
#define BIMC_M_BUFFER_STATUS_0_ACB_WR_EMPTY_SHFT                                 0x0

#define BIMC_M_BUFFER_STATUS_1_ADDR(b,n)                                      (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000410)
#define BIMC_M_BUFFER_STATUS_1_RMSK                                           0x3333
#define BIMC_M_BUFFER_STATUS_1_IN          \
        in_dword_masked(BIMC_M_BUFFER_STATUS_1_ADDR, BIMC_M_BUFFER_STATUS_1_RMSK)
#define BIMC_M_BUFFER_STATUS_1_INM(m)      \
        in_dword_masked(BIMC_M_BUFFER_STATUS_1_ADDR, m)
#define BIMC_M_BUFFER_STATUS_1_WUB_RD_FULL_BMSK                               0x2000
#define BIMC_M_BUFFER_STATUS_1_WUB_RD_FULL_SHFT                                  0xd
#define BIMC_M_BUFFER_STATUS_1_WUB_RD_EMPTY_BMSK                              0x1000
#define BIMC_M_BUFFER_STATUS_1_WUB_RD_EMPTY_SHFT                                 0xc
#define BIMC_M_BUFFER_STATUS_1_WUB_WR_FULL_BMSK                                0x200
#define BIMC_M_BUFFER_STATUS_1_WUB_WR_FULL_SHFT                                  0x9
#define BIMC_M_BUFFER_STATUS_1_WUB_WR_EMPTY_BMSK                               0x100
#define BIMC_M_BUFFER_STATUS_1_WUB_WR_EMPTY_SHFT                                 0x8
#define BIMC_M_BUFFER_STATUS_1_AUB_RD_FULL_BMSK                                 0x20
#define BIMC_M_BUFFER_STATUS_1_AUB_RD_FULL_SHFT                                  0x5
#define BIMC_M_BUFFER_STATUS_1_AUB_RD_EMPTY_BMSK                                0x10
#define BIMC_M_BUFFER_STATUS_1_AUB_RD_EMPTY_SHFT                                 0x4
#define BIMC_M_BUFFER_STATUS_1_AUB_WR_FULL_BMSK                                  0x2
#define BIMC_M_BUFFER_STATUS_1_AUB_WR_FULL_SHFT                                  0x1
#define BIMC_M_BUFFER_STATUS_1_AUB_WR_EMPTY_BMSK                                 0x1
#define BIMC_M_BUFFER_STATUS_1_AUB_WR_EMPTY_SHFT                                 0x0

#define BIMC_M_BKE1_ENABLE_ADDR(b,n)                                                         (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000800)
#define BIMC_M_BKE1_ENABLE_RMSK                                                            0xff000001
#define BIMC_M_BKE1_ENABLE_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_ENABLE_ADDR(b,n), BIMC_M_BKE1_ENABLE_RMSK)
#define BIMC_M_BKE1_ENABLE_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_ENABLE_ADDR(b,n), m)
#define BIMC_M_BKE1_ENABLE_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_ENABLE_ADDR(b,n),v)
#define BIMC_M_BKE1_ENABLE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_ENABLE_ADDR(b,n),m,v,BIMC_M_BKE1_ENABLE_INI(b,n))
#define BIMC_M_BKE1_ENABLE_QOS_POLICY_SEL_BMSK                                             0xff000000
#define BIMC_M_BKE1_ENABLE_QOS_POLICY_SEL_SHFT                                                   0x18
#define BIMC_M_BKE1_ENABLE_ENABLE_BMSK                                                            0x1
#define BIMC_M_BKE1_ENABLE_ENABLE_SHFT                                                            0x0

#define BIMC_M_BKE1_GRANT_PERIOD_ADDR(b,n)                                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000804)
#define BIMC_M_BKE1_GRANT_PERIOD_RMSK                                                           0x3ff
#define BIMC_M_BKE1_GRANT_PERIOD_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_GRANT_PERIOD_ADDR(b,n), BIMC_M_BKE1_GRANT_PERIOD_RMSK)
#define BIMC_M_BKE1_GRANT_PERIOD_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_GRANT_PERIOD_ADDR(b,n), m)
#define BIMC_M_BKE1_GRANT_PERIOD_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_GRANT_PERIOD_ADDR(b,n),v)
#define BIMC_M_BKE1_GRANT_PERIOD_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_GRANT_PERIOD_ADDR(b,n),m,v,BIMC_M_BKE1_GRANT_PERIOD_INI(b,n))
#define BIMC_M_BKE1_GRANT_PERIOD_GRANT_PERIOD_BMSK                                              0x3ff
#define BIMC_M_BKE1_GRANT_PERIOD_GRANT_PERIOD_SHFT                                                0x0

#define BIMC_M_BKE1_GRANT_COUNT_ADDR(b,n)                                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000808)
#define BIMC_M_BKE1_GRANT_COUNT_RMSK                                                           0xffff
#define BIMC_M_BKE1_GRANT_COUNT_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_GRANT_COUNT_ADDR(b,n), BIMC_M_BKE1_GRANT_COUNT_RMSK)
#define BIMC_M_BKE1_GRANT_COUNT_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_GRANT_COUNT_ADDR(b,n), m)
#define BIMC_M_BKE1_GRANT_COUNT_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_GRANT_COUNT_ADDR(b,n),v)
#define BIMC_M_BKE1_GRANT_COUNT_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_GRANT_COUNT_ADDR(b,n),m,v,BIMC_M_BKE1_GRANT_COUNT_INI(b,n))
#define BIMC_M_BKE1_GRANT_COUNT_GRANT_COUNT_BMSK                                               0xffff
#define BIMC_M_BKE1_GRANT_COUNT_GRANT_COUNT_SHFT                                                  0x0

#define BIMC_M_BKE1_THRESHOLD_HIGH_ADDR(b,n)                                                 (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000820)
#define BIMC_M_BKE1_THRESHOLD_HIGH_RMSK                                                        0xffff
#define BIMC_M_BKE1_THRESHOLD_HIGH_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_THRESHOLD_HIGH_ADDR(b,n), BIMC_M_BKE1_THRESHOLD_HIGH_RMSK)
#define BIMC_M_BKE1_THRESHOLD_HIGH_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_THRESHOLD_HIGH_ADDR(b,n), m)
#define BIMC_M_BKE1_THRESHOLD_HIGH_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_THRESHOLD_HIGH_ADDR(b,n),v)
#define BIMC_M_BKE1_THRESHOLD_HIGH_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_THRESHOLD_HIGH_ADDR(b,n),m,v,BIMC_M_BKE1_THRESHOLD_HIGH_INI(b,n))
#define BIMC_M_BKE1_THRESHOLD_HIGH_THRESHOLD_BMSK                                              0xffff
#define BIMC_M_BKE1_THRESHOLD_HIGH_THRESHOLD_SHFT                                                 0x0

#define BIMC_M_BKE1_THRESHOLD_MEDIUM_ADDR(b,n)                                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000824)
#define BIMC_M_BKE1_THRESHOLD_MEDIUM_RMSK                                                      0xffff
#define BIMC_M_BKE1_THRESHOLD_MEDIUM_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_THRESHOLD_MEDIUM_ADDR(b,n), BIMC_M_BKE1_THRESHOLD_MEDIUM_RMSK)
#define BIMC_M_BKE1_THRESHOLD_MEDIUM_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_THRESHOLD_MEDIUM_ADDR(b,n), m)
#define BIMC_M_BKE1_THRESHOLD_MEDIUM_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_THRESHOLD_MEDIUM_ADDR(b,n),v)
#define BIMC_M_BKE1_THRESHOLD_MEDIUM_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_THRESHOLD_MEDIUM_ADDR(b,n),m,v,BIMC_M_BKE1_THRESHOLD_MEDIUM_INI(b,n))
#define BIMC_M_BKE1_THRESHOLD_MEDIUM_THRESHOLD_BMSK                                            0xffff
#define BIMC_M_BKE1_THRESHOLD_MEDIUM_THRESHOLD_SHFT                                               0x0

#define BIMC_M_BKE1_THRESHOLD_LOW_ADDR(b,n)                                                  (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000828)
#define BIMC_M_BKE1_THRESHOLD_LOW_RMSK                                                         0xffff
#define BIMC_M_BKE1_THRESHOLD_LOW_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_THRESHOLD_LOW_ADDR(b,n), BIMC_M_BKE1_THRESHOLD_LOW_RMSK)
#define BIMC_M_BKE1_THRESHOLD_LOW_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_THRESHOLD_LOW_ADDR(b,n), m)
#define BIMC_M_BKE1_THRESHOLD_LOW_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_THRESHOLD_LOW_ADDR(b,n),v)
#define BIMC_M_BKE1_THRESHOLD_LOW_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_THRESHOLD_LOW_ADDR(b,n),m,v,BIMC_M_BKE1_THRESHOLD_LOW_INI(b,n))
#define BIMC_M_BKE1_THRESHOLD_LOW_THRESHOLD_BMSK                                               0xffff
#define BIMC_M_BKE1_THRESHOLD_LOW_THRESHOLD_SHFT                                                  0x0

#define BIMC_M_BKE1_HEALTH_0_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000840)
#define BIMC_M_BKE1_HEALTH_0_RMSK                                                          0x80000303
#define BIMC_M_BKE1_HEALTH_0_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_HEALTH_0_ADDR(b,n), BIMC_M_BKE1_HEALTH_0_RMSK)
#define BIMC_M_BKE1_HEALTH_0_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_HEALTH_0_ADDR(b,n), m)
#define BIMC_M_BKE1_HEALTH_0_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_HEALTH_0_ADDR(b,n),v)
#define BIMC_M_BKE1_HEALTH_0_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_HEALTH_0_ADDR(b,n),m,v,BIMC_M_BKE1_HEALTH_0_INI(b,n))
#define BIMC_M_BKE1_HEALTH_0_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE1_HEALTH_0_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE1_HEALTH_0_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE1_HEALTH_0_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE1_HEALTH_0_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE1_HEALTH_0_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE1_HEALTH_1_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000844)
#define BIMC_M_BKE1_HEALTH_1_RMSK                                                          0x80000303
#define BIMC_M_BKE1_HEALTH_1_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_HEALTH_1_ADDR(b,n), BIMC_M_BKE1_HEALTH_1_RMSK)
#define BIMC_M_BKE1_HEALTH_1_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_HEALTH_1_ADDR(b,n), m)
#define BIMC_M_BKE1_HEALTH_1_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_HEALTH_1_ADDR(b,n),v)
#define BIMC_M_BKE1_HEALTH_1_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_HEALTH_1_ADDR(b,n),m,v,BIMC_M_BKE1_HEALTH_1_INI(b,n))
#define BIMC_M_BKE1_HEALTH_1_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE1_HEALTH_1_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE1_HEALTH_1_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE1_HEALTH_1_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE1_HEALTH_1_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE1_HEALTH_1_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE1_HEALTH_2_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000848)
#define BIMC_M_BKE1_HEALTH_2_RMSK                                                          0x80000303
#define BIMC_M_BKE1_HEALTH_2_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_HEALTH_2_ADDR(b,n), BIMC_M_BKE1_HEALTH_2_RMSK)
#define BIMC_M_BKE1_HEALTH_2_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_HEALTH_2_ADDR(b,n), m)
#define BIMC_M_BKE1_HEALTH_2_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_HEALTH_2_ADDR(b,n),v)
#define BIMC_M_BKE1_HEALTH_2_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_HEALTH_2_ADDR(b,n),m,v,BIMC_M_BKE1_HEALTH_2_INI(b,n))
#define BIMC_M_BKE1_HEALTH_2_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE1_HEALTH_2_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE1_HEALTH_2_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE1_HEALTH_2_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE1_HEALTH_2_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE1_HEALTH_2_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE1_HEALTH_3_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x0000084c)
#define BIMC_M_BKE1_HEALTH_3_RMSK                                                               0x303
#define BIMC_M_BKE1_HEALTH_3_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE1_HEALTH_3_ADDR(b,n), BIMC_M_BKE1_HEALTH_3_RMSK)
#define BIMC_M_BKE1_HEALTH_3_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE1_HEALTH_3_ADDR(b,n), m)
#define BIMC_M_BKE1_HEALTH_3_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE1_HEALTH_3_ADDR(b,n),v)
#define BIMC_M_BKE1_HEALTH_3_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE1_HEALTH_3_ADDR(b,n),m,v,BIMC_M_BKE1_HEALTH_3_INI(b,n))
#define BIMC_M_BKE1_HEALTH_3_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE1_HEALTH_3_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE1_HEALTH_3_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE1_HEALTH_3_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE2_ENABLE_ADDR(b,n)                                                         (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000900)
#define BIMC_M_BKE2_ENABLE_RMSK                                                            0xff000001
#define BIMC_M_BKE2_ENABLE_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_ENABLE_ADDR(b,n), BIMC_M_BKE2_ENABLE_RMSK)
#define BIMC_M_BKE2_ENABLE_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_ENABLE_ADDR(b,n), m)
#define BIMC_M_BKE2_ENABLE_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_ENABLE_ADDR(b,n),v)
#define BIMC_M_BKE2_ENABLE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_ENABLE_ADDR(b,n),m,v,BIMC_M_BKE2_ENABLE_INI(b,n))
#define BIMC_M_BKE2_ENABLE_QOS_POLICY_SEL_BMSK                                             0xff000000
#define BIMC_M_BKE2_ENABLE_QOS_POLICY_SEL_SHFT                                                   0x18
#define BIMC_M_BKE2_ENABLE_ENABLE_BMSK                                                            0x1
#define BIMC_M_BKE2_ENABLE_ENABLE_SHFT                                                            0x0

#define BIMC_M_BKE2_GRANT_PERIOD_ADDR(b,n)                                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000904)
#define BIMC_M_BKE2_GRANT_PERIOD_RMSK                                                           0x3ff
#define BIMC_M_BKE2_GRANT_PERIOD_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_GRANT_PERIOD_ADDR(b,n), BIMC_M_BKE2_GRANT_PERIOD_RMSK)
#define BIMC_M_BKE2_GRANT_PERIOD_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_GRANT_PERIOD_ADDR(b,n), m)
#define BIMC_M_BKE2_GRANT_PERIOD_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_GRANT_PERIOD_ADDR(b,n),v)
#define BIMC_M_BKE2_GRANT_PERIOD_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_GRANT_PERIOD_ADDR(b,n),m,v,BIMC_M_BKE2_GRANT_PERIOD_INI(b,n))
#define BIMC_M_BKE2_GRANT_PERIOD_GRANT_PERIOD_BMSK                                              0x3ff
#define BIMC_M_BKE2_GRANT_PERIOD_GRANT_PERIOD_SHFT                                                0x0

#define BIMC_M_BKE2_GRANT_COUNT_ADDR(b,n)                                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000908)
#define BIMC_M_BKE2_GRANT_COUNT_RMSK                                                           0xffff
#define BIMC_M_BKE2_GRANT_COUNT_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_GRANT_COUNT_ADDR(b,n), BIMC_M_BKE2_GRANT_COUNT_RMSK)
#define BIMC_M_BKE2_GRANT_COUNT_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_GRANT_COUNT_ADDR(b,n), m)
#define BIMC_M_BKE2_GRANT_COUNT_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_GRANT_COUNT_ADDR(b,n),v)
#define BIMC_M_BKE2_GRANT_COUNT_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_GRANT_COUNT_ADDR(b,n),m,v,BIMC_M_BKE2_GRANT_COUNT_INI(b,n))
#define BIMC_M_BKE2_GRANT_COUNT_GRANT_COUNT_BMSK                                               0xffff
#define BIMC_M_BKE2_GRANT_COUNT_GRANT_COUNT_SHFT                                                  0x0

#define BIMC_M_BKE2_THRESHOLD_HIGH_ADDR(b,n)                                                 (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000920)
#define BIMC_M_BKE2_THRESHOLD_HIGH_RMSK                                                        0xffff
#define BIMC_M_BKE2_THRESHOLD_HIGH_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_THRESHOLD_HIGH_ADDR(b,n), BIMC_M_BKE2_THRESHOLD_HIGH_RMSK)
#define BIMC_M_BKE2_THRESHOLD_HIGH_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_THRESHOLD_HIGH_ADDR(b,n), m)
#define BIMC_M_BKE2_THRESHOLD_HIGH_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_THRESHOLD_HIGH_ADDR(b,n),v)
#define BIMC_M_BKE2_THRESHOLD_HIGH_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_THRESHOLD_HIGH_ADDR(b,n),m,v,BIMC_M_BKE2_THRESHOLD_HIGH_INI(b,n))
#define BIMC_M_BKE2_THRESHOLD_HIGH_THRESHOLD_BMSK                                              0xffff
#define BIMC_M_BKE2_THRESHOLD_HIGH_THRESHOLD_SHFT                                                 0x0

#define BIMC_M_BKE2_THRESHOLD_MEDIUM_ADDR(b,n)                                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000924)
#define BIMC_M_BKE2_THRESHOLD_MEDIUM_RMSK                                                      0xffff
#define BIMC_M_BKE2_THRESHOLD_MEDIUM_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_THRESHOLD_MEDIUM_ADDR(b,n), BIMC_M_BKE2_THRESHOLD_MEDIUM_RMSK)
#define BIMC_M_BKE2_THRESHOLD_MEDIUM_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_THRESHOLD_MEDIUM_ADDR(b,n), m)
#define BIMC_M_BKE2_THRESHOLD_MEDIUM_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_THRESHOLD_MEDIUM_ADDR(b,n),v)
#define BIMC_M_BKE2_THRESHOLD_MEDIUM_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_THRESHOLD_MEDIUM_ADDR(b,n),m,v,BIMC_M_BKE2_THRESHOLD_MEDIUM_INI(b,n))
#define BIMC_M_BKE2_THRESHOLD_MEDIUM_THRESHOLD_BMSK                                            0xffff
#define BIMC_M_BKE2_THRESHOLD_MEDIUM_THRESHOLD_SHFT                                               0x0

#define BIMC_M_BKE2_THRESHOLD_LOW_ADDR(b,n)                                                  (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000928)
#define BIMC_M_BKE2_THRESHOLD_LOW_RMSK                                                         0xffff
#define BIMC_M_BKE2_THRESHOLD_LOW_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_THRESHOLD_LOW_ADDR(b,n), BIMC_M_BKE2_THRESHOLD_LOW_RMSK)
#define BIMC_M_BKE2_THRESHOLD_LOW_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_THRESHOLD_LOW_ADDR(b,n), m)
#define BIMC_M_BKE2_THRESHOLD_LOW_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_THRESHOLD_LOW_ADDR(b,n),v)
#define BIMC_M_BKE2_THRESHOLD_LOW_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_THRESHOLD_LOW_ADDR(b,n),m,v,BIMC_M_BKE2_THRESHOLD_LOW_INI(b,n))
#define BIMC_M_BKE2_THRESHOLD_LOW_THRESHOLD_BMSK                                               0xffff
#define BIMC_M_BKE2_THRESHOLD_LOW_THRESHOLD_SHFT                                                  0x0

#define BIMC_M_BKE2_HEALTH_0_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000940)
#define BIMC_M_BKE2_HEALTH_0_RMSK                                                          0x80000303
#define BIMC_M_BKE2_HEALTH_0_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_HEALTH_0_ADDR(b,n), BIMC_M_BKE2_HEALTH_0_RMSK)
#define BIMC_M_BKE2_HEALTH_0_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_HEALTH_0_ADDR(b,n), m)
#define BIMC_M_BKE2_HEALTH_0_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_HEALTH_0_ADDR(b,n),v)
#define BIMC_M_BKE2_HEALTH_0_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_HEALTH_0_ADDR(b,n),m,v,BIMC_M_BKE2_HEALTH_0_INI(b,n))
#define BIMC_M_BKE2_HEALTH_0_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE2_HEALTH_0_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE2_HEALTH_0_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE2_HEALTH_0_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE2_HEALTH_0_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE2_HEALTH_0_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE2_HEALTH_1_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000944)
#define BIMC_M_BKE2_HEALTH_1_RMSK                                                          0x80000303
#define BIMC_M_BKE2_HEALTH_1_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_HEALTH_1_ADDR(b,n), BIMC_M_BKE2_HEALTH_1_RMSK)
#define BIMC_M_BKE2_HEALTH_1_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_HEALTH_1_ADDR(b,n), m)
#define BIMC_M_BKE2_HEALTH_1_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_HEALTH_1_ADDR(b,n),v)
#define BIMC_M_BKE2_HEALTH_1_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_HEALTH_1_ADDR(b,n),m,v,BIMC_M_BKE2_HEALTH_1_INI(b,n))
#define BIMC_M_BKE2_HEALTH_1_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE2_HEALTH_1_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE2_HEALTH_1_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE2_HEALTH_1_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE2_HEALTH_1_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE2_HEALTH_1_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE2_HEALTH_2_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000948)
#define BIMC_M_BKE2_HEALTH_2_RMSK                                                          0x80000303
#define BIMC_M_BKE2_HEALTH_2_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_HEALTH_2_ADDR(b,n), BIMC_M_BKE2_HEALTH_2_RMSK)
#define BIMC_M_BKE2_HEALTH_2_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_HEALTH_2_ADDR(b,n), m)
#define BIMC_M_BKE2_HEALTH_2_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_HEALTH_2_ADDR(b,n),v)
#define BIMC_M_BKE2_HEALTH_2_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_HEALTH_2_ADDR(b,n),m,v,BIMC_M_BKE2_HEALTH_2_INI(b,n))
#define BIMC_M_BKE2_HEALTH_2_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE2_HEALTH_2_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE2_HEALTH_2_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE2_HEALTH_2_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE2_HEALTH_2_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE2_HEALTH_2_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE2_HEALTH_3_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x0000094c)
#define BIMC_M_BKE2_HEALTH_3_RMSK                                                               0x303
#define BIMC_M_BKE2_HEALTH_3_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE2_HEALTH_3_ADDR(b,n), BIMC_M_BKE2_HEALTH_3_RMSK)
#define BIMC_M_BKE2_HEALTH_3_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE2_HEALTH_3_ADDR(b,n), m)
#define BIMC_M_BKE2_HEALTH_3_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE2_HEALTH_3_ADDR(b,n),v)
#define BIMC_M_BKE2_HEALTH_3_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE2_HEALTH_3_ADDR(b,n),m,v,BIMC_M_BKE2_HEALTH_3_INI(b,n))
#define BIMC_M_BKE2_HEALTH_3_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE2_HEALTH_3_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE2_HEALTH_3_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE2_HEALTH_3_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE3_ENABLE_ADDR(b,n)                                                         (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A00)
#define BIMC_M_BKE3_ENABLE_RMSK                                                            0xff000001
#define BIMC_M_BKE3_ENABLE_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_ENABLE_ADDR(b,n), BIMC_M_BKE3_ENABLE_RMSK)
#define BIMC_M_BKE3_ENABLE_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_ENABLE_ADDR(b,n), m)
#define BIMC_M_BKE3_ENABLE_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_ENABLE_ADDR(b,n),v)
#define BIMC_M_BKE3_ENABLE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_ENABLE_ADDR(b,n),m,v,BIMC_M_BKE3_ENABLE_INI(b,n))
#define BIMC_M_BKE3_ENABLE_QOS_POLICY_SEL_BMSK                                             0xff000000
#define BIMC_M_BKE3_ENABLE_QOS_POLICY_SEL_SHFT                                                   0x18
#define BIMC_M_BKE3_ENABLE_ENABLE_BMSK                                                            0x1
#define BIMC_M_BKE3_ENABLE_ENABLE_SHFT                                                            0x0

#define BIMC_M_BKE3_GRANT_PERIOD_ADDR(b,n)                                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A04)
#define BIMC_M_BKE3_GRANT_PERIOD_RMSK                                                           0x3ff
#define BIMC_M_BKE3_GRANT_PERIOD_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_GRANT_PERIOD_ADDR(b,n), BIMC_M_BKE3_GRANT_PERIOD_RMSK)
#define BIMC_M_BKE3_GRANT_PERIOD_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_GRANT_PERIOD_ADDR(b,n), m)
#define BIMC_M_BKE3_GRANT_PERIOD_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_GRANT_PERIOD_ADDR(b,n),v)
#define BIMC_M_BKE3_GRANT_PERIOD_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_GRANT_PERIOD_ADDR(b,n),m,v,BIMC_M_BKE3_GRANT_PERIOD_INI(b,n))
#define BIMC_M_BKE3_GRANT_PERIOD_GRANT_PERIOD_BMSK                                              0x3ff
#define BIMC_M_BKE3_GRANT_PERIOD_GRANT_PERIOD_SHFT                                                0x0

#define BIMC_M_BKE3_GRANT_COUNT_ADDR(b,n)                                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A08)
#define BIMC_M_BKE3_GRANT_COUNT_RMSK                                                           0xffff
#define BIMC_M_BKE3_GRANT_COUNT_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_GRANT_COUNT_ADDR(b,n), BIMC_M_BKE3_GRANT_COUNT_RMSK)
#define BIMC_M_BKE3_GRANT_COUNT_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_GRANT_COUNT_ADDR(b,n), m)
#define BIMC_M_BKE3_GRANT_COUNT_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_GRANT_COUNT_ADDR(b,n),v)
#define BIMC_M_BKE3_GRANT_COUNT_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_GRANT_COUNT_ADDR(b,n),m,v,BIMC_M_BKE3_GRANT_COUNT_INI(b,n))
#define BIMC_M_BKE3_GRANT_COUNT_GRANT_COUNT_BMSK                                               0xffff
#define BIMC_M_BKE3_GRANT_COUNT_GRANT_COUNT_SHFT                                                  0x0

#define BIMC_M_BKE3_THRESHOLD_HIGH_ADDR(b,n)                                                 (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A20)
#define BIMC_M_BKE3_THRESHOLD_HIGH_RMSK                                                        0xffff
#define BIMC_M_BKE3_THRESHOLD_HIGH_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_THRESHOLD_HIGH_ADDR(b,n), BIMC_M_BKE3_THRESHOLD_HIGH_RMSK)
#define BIMC_M_BKE3_THRESHOLD_HIGH_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_THRESHOLD_HIGH_ADDR(b,n), m)
#define BIMC_M_BKE3_THRESHOLD_HIGH_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_THRESHOLD_HIGH_ADDR(b,n),v)
#define BIMC_M_BKE3_THRESHOLD_HIGH_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_THRESHOLD_HIGH_ADDR(b,n),m,v,BIMC_M_BKE3_THRESHOLD_HIGH_INI(b,n))
#define BIMC_M_BKE3_THRESHOLD_HIGH_THRESHOLD_BMSK                                              0xffff
#define BIMC_M_BKE3_THRESHOLD_HIGH_THRESHOLD_SHFT                                                 0x0

#define BIMC_M_BKE3_THRESHOLD_MEDIUM_ADDR(b,n)                                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A24)
#define BIMC_M_BKE3_THRESHOLD_MEDIUM_RMSK                                                      0xffff
#define BIMC_M_BKE3_THRESHOLD_MEDIUM_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_THRESHOLD_MEDIUM_ADDR(b,n), BIMC_M_BKE3_THRESHOLD_MEDIUM_RMSK)
#define BIMC_M_BKE3_THRESHOLD_MEDIUM_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_THRESHOLD_MEDIUM_ADDR(b,n), m)
#define BIMC_M_BKE3_THRESHOLD_MEDIUM_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_THRESHOLD_MEDIUM_ADDR(b,n),v)
#define BIMC_M_BKE3_THRESHOLD_MEDIUM_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_THRESHOLD_MEDIUM_ADDR(b,n),m,v,BIMC_M_BKE3_THRESHOLD_MEDIUM_INI(b,n))
#define BIMC_M_BKE3_THRESHOLD_MEDIUM_THRESHOLD_BMSK                                            0xffff
#define BIMC_M_BKE3_THRESHOLD_MEDIUM_THRESHOLD_SHFT                                               0x0

#define BIMC_M_BKE3_THRESHOLD_LOW_ADDR(b,n)                                                  (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A28)
#define BIMC_M_BKE3_THRESHOLD_LOW_RMSK                                                         0xffff
#define BIMC_M_BKE3_THRESHOLD_LOW_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_THRESHOLD_LOW_ADDR(b,n), BIMC_M_BKE3_THRESHOLD_LOW_RMSK)
#define BIMC_M_BKE3_THRESHOLD_LOW_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_THRESHOLD_LOW_ADDR(b,n), m)
#define BIMC_M_BKE3_THRESHOLD_LOW_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_THRESHOLD_LOW_ADDR(b,n),v)
#define BIMC_M_BKE3_THRESHOLD_LOW_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_THRESHOLD_LOW_ADDR(b,n),m,v,BIMC_M_BKE3_THRESHOLD_LOW_INI(b,n))
#define BIMC_M_BKE3_THRESHOLD_LOW_THRESHOLD_BMSK                                               0xffff
#define BIMC_M_BKE3_THRESHOLD_LOW_THRESHOLD_SHFT                                                  0x0

#define BIMC_M_BKE3_HEALTH_0_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A40)
#define BIMC_M_BKE3_HEALTH_0_RMSK                                                          0x80000303
#define BIMC_M_BKE3_HEALTH_0_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_HEALTH_0_ADDR(b,n), BIMC_M_BKE3_HEALTH_0_RMSK)
#define BIMC_M_BKE3_HEALTH_0_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_HEALTH_0_ADDR(b,n), m)
#define BIMC_M_BKE3_HEALTH_0_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_HEALTH_0_ADDR(b,n),v)
#define BIMC_M_BKE3_HEALTH_0_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_HEALTH_0_ADDR(b,n),m,v,BIMC_M_BKE3_HEALTH_0_INI(b,n))
#define BIMC_M_BKE3_HEALTH_0_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE3_HEALTH_0_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE3_HEALTH_0_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE3_HEALTH_0_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE3_HEALTH_0_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE3_HEALTH_0_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE3_HEALTH_1_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A44)
#define BIMC_M_BKE3_HEALTH_1_RMSK                                                          0x80000303
#define BIMC_M_BKE3_HEALTH_1_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_HEALTH_1_ADDR(b,n), BIMC_M_BKE3_HEALTH_1_RMSK)
#define BIMC_M_BKE3_HEALTH_1_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_HEALTH_1_ADDR(b,n), m)
#define BIMC_M_BKE3_HEALTH_1_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_HEALTH_1_ADDR(b,n),v)
#define BIMC_M_BKE3_HEALTH_1_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_HEALTH_1_ADDR(b,n),m,v,BIMC_M_BKE3_HEALTH_1_INI(b,n))
#define BIMC_M_BKE3_HEALTH_1_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE3_HEALTH_1_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE3_HEALTH_1_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE3_HEALTH_1_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE3_HEALTH_1_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE3_HEALTH_1_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE3_HEALTH_2_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A48)
#define BIMC_M_BKE3_HEALTH_2_RMSK                                                          0x80000303
#define BIMC_M_BKE3_HEALTH_2_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_HEALTH_2_ADDR(b,n), BIMC_M_BKE3_HEALTH_2_RMSK)
#define BIMC_M_BKE3_HEALTH_2_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_HEALTH_2_ADDR(b,n), m)
#define BIMC_M_BKE3_HEALTH_2_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_HEALTH_2_ADDR(b,n),v)
#define BIMC_M_BKE3_HEALTH_2_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_HEALTH_2_ADDR(b,n),m,v,BIMC_M_BKE3_HEALTH_2_INI(b,n))
#define BIMC_M_BKE3_HEALTH_2_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKE3_HEALTH_2_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKE3_HEALTH_2_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE3_HEALTH_2_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE3_HEALTH_2_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE3_HEALTH_2_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKE3_HEALTH_3_ADDR(b,n)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000A4c)
#define BIMC_M_BKE3_HEALTH_3_RMSK                                                               0x303
#define BIMC_M_BKE3_HEALTH_3_INI(b,n)      \
        in_dword_masked(BIMC_M_BKE3_HEALTH_3_ADDR(b,n), BIMC_M_BKE3_HEALTH_3_RMSK)
#define BIMC_M_BKE3_HEALTH_3_INMI(b,n,m)      \
        in_dword_masked(BIMC_M_BKE3_HEALTH_3_ADDR(b,n), m)
#define BIMC_M_BKE3_HEALTH_3_OUTI(b,n,v)      \
        out_dword(BIMC_M_BKE3_HEALTH_3_ADDR(b,n),v)
#define BIMC_M_BKE3_HEALTH_3_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_M_BKE3_HEALTH_3_ADDR(b,n),m,v,BIMC_M_BKE3_HEALTH_3_INI(b,n))
#define BIMC_M_BKE3_HEALTH_3_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKE3_HEALTH_3_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKE3_HEALTH_3_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKE3_HEALTH_3_PRIORITYLVL_SHFT                                                     0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_M_BKEn_GENERIC
 *--------------------------------------------------------------------------*/
#define BIMC_M_BKEn_ENABLE_ADDR(b,n,o)                                                         (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000800 + (0x100 * (o)))
#define BIMC_M_BKEn_ENABLE_RMSK                                                            0xff000001
#define BIMC_M_BKEn_ENABLE_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_ENABLE_ADDR(b,n,o), BIMC_M_BKEn_ENABLE_RMSK)
#define BIMC_M_BKEn_ENABLE_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_ENABLE_ADDR(b,n,o), m)
#define BIMC_M_BKEn_ENABLE_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_ENABLE_ADDR(b,n,o),v)
#define BIMC_M_BKEn_ENABLE_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_ENABLE_ADDR(b,n,o),m,v,BIMC_M_BKEn_ENABLE_INI2(b,n,o))
#define BIMC_M_BKEn_ENABLE_QOS_POLICY_SEL_BMSK                                             0xff000000
#define BIMC_M_BKEn_ENABLE_QOS_POLICY_SEL_SHFT                                                   0x18
#define BIMC_M_BKEn_ENABLE_ENABLE_BMSK                                                            0x1
#define BIMC_M_BKEn_ENABLE_ENABLE_SHFT                                                            0x0

#define BIMC_M_BKEn_GRANT_PERIOD_ADDR(b,n,o)                                                   (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000804 + (0x100 * (o)))
#define BIMC_M_BKEn_GRANT_PERIOD_RMSK                                                           0x3ff
#define BIMC_M_BKEn_GRANT_PERIOD_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_GRANT_PERIOD_ADDR(b,n,o), BIMC_M_BKEn_GRANT_PERIOD_RMSK)
#define BIMC_M_BKEn_GRANT_PERIOD_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_GRANT_PERIOD_ADDR(b,n,o), m)
#define BIMC_M_BKEn_GRANT_PERIOD_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_GRANT_PERIOD_ADDR(b,n,o),v)
#define BIMC_M_BKEn_GRANT_PERIOD_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_GRANT_PERIOD_ADDR(b,n,o),m,v,BIMC_M_BKEn_GRANT_PERIOD_INI2(b,n,o))
#define BIMC_M_BKEn_GRANT_PERIOD_GRANT_PERIOD_BMSK                                              0x3ff
#define BIMC_M_BKEn_GRANT_PERIOD_GRANT_PERIOD_SHFT                                                0x0

#define BIMC_M_BKEn_GRANT_COUNT_ADDR(b,n,o)                                                    (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000808 + (0x100 * (o)))
#define BIMC_M_BKEn_GRANT_COUNT_RMSK                                                           0xffff
#define BIMC_M_BKEn_GRANT_COUNT_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_GRANT_COUNT_ADDR(b,n,o), BIMC_M_BKEn_GRANT_COUNT_RMSK)
#define BIMC_M_BKEn_GRANT_COUNT_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_GRANT_COUNT_ADDR(b,n,o), m)
#define BIMC_M_BKEn_GRANT_COUNT_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_GRANT_COUNT_ADDR(b,n,o),v)
#define BIMC_M_BKEn_GRANT_COUNT_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_GRANT_COUNT_ADDR(b,n,o),m,v,BIMC_M_BKEn_GRANT_COUNT_INI2(b,n,o))
#define BIMC_M_BKEn_GRANT_COUNT_GRANT_COUNT_BMSK                                               0xffff
#define BIMC_M_BKEn_GRANT_COUNT_GRANT_COUNT_SHFT                                                  0x0

#define BIMC_M_BKEn_THRESHOLD_HIGH_ADDR(b,n,o)                                                 (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000820 + (0x100 * (o)))
#define BIMC_M_BKEn_THRESHOLD_HIGH_RMSK                                                        0xffff
#define BIMC_M_BKEn_THRESHOLD_HIGH_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_THRESHOLD_HIGH_ADDR(b,n,o), BIMC_M_BKEn_THRESHOLD_HIGH_RMSK)
#define BIMC_M_BKEn_THRESHOLD_HIGH_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_THRESHOLD_HIGH_ADDR(b,n,o), m)
#define BIMC_M_BKEn_THRESHOLD_HIGH_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_THRESHOLD_HIGH_ADDR(b,n,o),v)
#define BIMC_M_BKEn_THRESHOLD_HIGH_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_THRESHOLD_HIGH_ADDR(b,n,o),m,v,BIMC_M_BKEn_THRESHOLD_HIGH_INI2(b,n,o))
#define BIMC_M_BKEn_THRESHOLD_HIGH_THRESHOLD_BMSK                                              0xffff
#define BIMC_M_BKEn_THRESHOLD_HIGH_THRESHOLD_SHFT                                                 0x0

#define BIMC_M_BKEn_THRESHOLD_MEDIUM_ADDR(b,n,o)                                               (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000824 + (0x100 * (o)))
#define BIMC_M_BKEn_THRESHOLD_MEDIUM_RMSK                                                      0xffff
#define BIMC_M_BKEn_THRESHOLD_MEDIUM_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_THRESHOLD_MEDIUM_ADDR(b,n,o), BIMC_M_BKEn_THRESHOLD_MEDIUM_RMSK)
#define BIMC_M_BKEn_THRESHOLD_MEDIUM_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_THRESHOLD_MEDIUM_ADDR(b,n,o), m)
#define BIMC_M_BKEn_THRESHOLD_MEDIUM_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_THRESHOLD_MEDIUM_ADDR(b,n,o),v)
#define BIMC_M_BKEn_THRESHOLD_MEDIUM_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_THRESHOLD_MEDIUM_ADDR(b,n,o),m,v,BIMC_M_BKEn_THRESHOLD_MEDIUM_INI2(b,n,o))
#define BIMC_M_BKEn_THRESHOLD_MEDIUM_THRESHOLD_BMSK                                            0xffff
#define BIMC_M_BKEn_THRESHOLD_MEDIUM_THRESHOLD_SHFT                                               0x0

#define BIMC_M_BKEn_THRESHOLD_LOW_ADDR(b,n,o)                                                  (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000828 + (0x100 * (o)))
#define BIMC_M_BKEn_THRESHOLD_LOW_RMSK                                                         0xffff
#define BIMC_M_BKEn_THRESHOLD_LOW_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_THRESHOLD_LOW_ADDR(b,n,o), BIMC_M_BKEn_THRESHOLD_LOW_RMSK)
#define BIMC_M_BKEn_THRESHOLD_LOW_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_THRESHOLD_LOW_ADDR(b,n,o), m)
#define BIMC_M_BKEn_THRESHOLD_LOW_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_THRESHOLD_LOW_ADDR(b,n,o),v)
#define BIMC_M_BKEn_THRESHOLD_LOW_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_THRESHOLD_LOW_ADDR(b,n,o),m,v,BIMC_M_BKEn_THRESHOLD_LOW_INI2(b,n,o))
#define BIMC_M_BKEn_THRESHOLD_LOW_THRESHOLD_BMSK                                               0xffff
#define BIMC_M_BKEn_THRESHOLD_LOW_THRESHOLD_SHFT                                                  0x0

#define BIMC_M_BKEn_HEALTH_0_ADDR(b,n,o)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000840 + (0x100 * (o)))
#define BIMC_M_BKEn_HEALTH_0_RMSK                                                          0x80000303
#define BIMC_M_BKEn_HEALTH_0_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_HEALTH_0_ADDR(b,n,o), BIMC_M_BKEn_HEALTH_0_RMSK)
#define BIMC_M_BKEn_HEALTH_0_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_HEALTH_0_ADDR(b,n,o), m)
#define BIMC_M_BKEn_HEALTH_0_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_HEALTH_0_ADDR(b,n,o),v)
#define BIMC_M_BKEn_HEALTH_0_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_HEALTH_0_ADDR(b,n,o),m,v,BIMC_M_BKEn_HEALTH_0_INI2(b,n,o))
#define BIMC_M_BKEn_HEALTH_0_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKEn_HEALTH_0_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKEn_HEALTH_0_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKEn_HEALTH_0_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKEn_HEALTH_0_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKEn_HEALTH_0_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKEn_HEALTH_1_ADDR(b,n,o)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000844 + (0x100 * (o)))
#define BIMC_M_BKEn_HEALTH_1_RMSK                                                          0x80000303
#define BIMC_M_BKEn_HEALTH_1_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_HEALTH_1_ADDR(b,n,o), BIMC_M_BKEn_HEALTH_1_RMSK)
#define BIMC_M_BKEn_HEALTH_1_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_HEALTH_1_ADDR(b,n,o), m)
#define BIMC_M_BKEn_HEALTH_1_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_HEALTH_1_ADDR(b,n,o),v)
#define BIMC_M_BKEn_HEALTH_1_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_HEALTH_1_ADDR(b,n,o),m,v,BIMC_M_BKEn_HEALTH_1_INI2(b,n,o))
#define BIMC_M_BKEn_HEALTH_1_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKEn_HEALTH_1_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKEn_HEALTH_1_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKEn_HEALTH_1_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKEn_HEALTH_1_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKEn_HEALTH_1_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKEn_HEALTH_2_ADDR(b,n,o)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x00000848 + (0x100 * (o)))
#define BIMC_M_BKEn_HEALTH_2_RMSK                                                          0x80000303
#define BIMC_M_BKEn_HEALTH_2_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_HEALTH_2_ADDR(b,n,o), BIMC_M_BKEn_HEALTH_2_RMSK)
#define BIMC_M_BKEn_HEALTH_2_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_HEALTH_2_ADDR(b,n,o), m)
#define BIMC_M_BKEn_HEALTH_2_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_HEALTH_2_ADDR(b,n,o),v)
#define BIMC_M_BKEn_HEALTH_2_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_HEALTH_2_ADDR(b,n,o),m,v,BIMC_M_BKEn_HEALTH_2_INI2(b,n,o))
#define BIMC_M_BKEn_HEALTH_2_LIMIT_COMMANDS_BMSK                                           0x80000000
#define BIMC_M_BKEn_HEALTH_2_LIMIT_COMMANDS_SHFT                                                 0x1f
#define BIMC_M_BKEn_HEALTH_2_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKEn_HEALTH_2_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKEn_HEALTH_2_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKEn_HEALTH_2_PRIORITYLVL_SHFT                                                     0x0

#define BIMC_M_BKEn_HEALTH_3_ADDR(b,n,o)                                                       (BIMC_M_REG_BASE(b) + (0x4000 * (n)) + 0x0000084c + (0x100 * (o)))
#define BIMC_M_BKEn_HEALTH_3_RMSK                                                               0x303
#define BIMC_M_BKEn_HEALTH_3_INI2(b,n,o)      \
        in_dword_masked(BIMC_M_BKEn_HEALTH_3_ADDR(b,n,o), BIMC_M_BKEn_HEALTH_3_RMSK)
#define BIMC_M_BKEn_HEALTH_3_INMI2(b,n,o,m)      \
        in_dword_masked(BIMC_M_BKEn_HEALTH_3_ADDR(b,n,o), m)
#define BIMC_M_BKEn_HEALTH_3_OUTI2(b,n,o,v)      \
        out_dword(BIMC_M_BKEn_HEALTH_3_ADDR(b,n,o),v)
#define BIMC_M_BKEn_HEALTH_3_OUTMI2(b,n,o,m,v) \
        out_dword_masked_ns(BIMC_M_BKEn_HEALTH_3_ADDR(b,n,o),m,v,BIMC_M_BKEn_HEALTH_3_INI2(b,n,o))
#define BIMC_M_BKEn_HEALTH_3_AREQPRIORITY_BMSK                                                  0x300
#define BIMC_M_BKEn_HEALTH_3_AREQPRIORITY_SHFT                                                    0x8
#define BIMC_M_BKEn_HEALTH_3_PRIORITYLVL_BMSK                                                     0x3
#define BIMC_M_BKEn_HEALTH_3_PRIORITYLVL_SHFT                                                     0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_GENERIC_GENERAL
 *--------------------------------------------------------------------------*/
#define BIMC_S_REG_BASE(b)                                                   ((b)      + 0x00048000)

#define BIMC_S_COMPONENT_INFO_ADDR(b,n)                                 (BIMC_S_REG_BASE(b) + (0x8000 * (n)) + 0x00000000)
#define BIMC_S_COMPONENT_INFO_RMSK                                      0xffffff
#define BIMC_S_COMPONENT_INFO_INI(b,n)    \
        in_dword_masked(BIMC_S_COMPONENT_INFO_ADDR(b,n), BIMC_S_COMPONENT_INFO_RMSK)
#define BIMC_S_COMPONENT_INFO_INMI(b,n,m) \
        in_dword_masked(BIMC_S_COMPONENT_INFO_ADDR(b,n), m)
#define BIMC_S_COMPONENT_INFO_INSTANCE_BMSK                             0xff0000
#define BIMC_S_COMPONENT_INFO_INSTANCE_SHFT                                 0x10
#define BIMC_S_COMPONENT_INFO_SUB_TYPE_BMSK                               0xff00
#define BIMC_S_COMPONENT_INFO_SUB_TYPE_SHFT                                  0x8
#define BIMC_S_COMPONENT_INFO_TYPE_BMSK                                     0xff
#define BIMC_S_COMPONENT_INFO_TYPE_SHFT                                      0x0

#define BIMC_S_HW_INFO_ADDR(b,n)                                        (BIMC_S_REG_BASE(b) + (0x8000 * (n)) + 0x00000010)
#define BIMC_S_HW_INFO_RMSK                                           0xffffffff
#define BIMC_S_HW_INFO_INI(b,n)    \
        in_dword_masked(BIMC_S_HW_INFO_ADDR(b,n), BIMC_S_HW_INFO_RMSK)
#define BIMC_S_HW_INFO_INMI(b,n,m) \
        in_dword_masked(BIMC_S_HW_INFO_ADDR(b,n), m)
#define BIMC_S_HW_INFO_MAJOR_BMSK                                     0xff000000
#define BIMC_S_HW_INFO_MAJOR_SHFT                                           0x18
#define BIMC_S_HW_INFO_BRANCH_BMSK                                      0xff0000
#define BIMC_S_HW_INFO_BRANCH_SHFT                                          0x10
#define BIMC_S_HW_INFO_MINOR_BMSK                                         0xff00
#define BIMC_S_HW_INFO_MINOR_SHFT                                            0x8
#define BIMC_S_HW_INFO_ECO_BMSK                                             0xff
#define BIMC_S_HW_INFO_ECO_SHFT                                              0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_SCMO_GENERIC
 *--------------------------------------------------------------------------*/
#define BIMC_S_SCMO_REG_BASE(b)                                                 ((b)      + 0x00048000)

#define BIMC_S_SCMO_CONFIGURATION_INFO_0_ADDR(b,n)                           (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000020)
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_RMSK                              0xffffffff
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_0_ADDR(b,n), BIMC_S_SCMO_CONFIGURATION_INFO_0_RMSK)
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_0_ADDR(b,n), m)
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_DATA_WIDTH_BMSK                   0xffff0000
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_DATA_WIDTH_SHFT                         0x10
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_TID_WIDTH_BMSK                        0xff00
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_TID_WIDTH_SHFT                           0x8
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_MID_WIDTH_BMSK                          0xff
#define BIMC_S_SCMO_CONFIGURATION_INFO_0_MID_WIDTH_SHFT                           0x0

#define BIMC_S_SCMO_CONFIGURATION_INFO_1_ADDR(b,n)                           (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000030)
#define BIMC_S_SCMO_CONFIGURATION_INFO_1_RMSK                              0xffffffff
#define BIMC_S_SCMO_CONFIGURATION_INFO_1_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_1_ADDR(b,n), BIMC_S_SCMO_CONFIGURATION_INFO_1_RMSK)
#define BIMC_S_SCMO_CONFIGURATION_INFO_1_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_1_ADDR(b,n), m)
#define BIMC_S_SCMO_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_BMSK           0xffffffff
#define BIMC_S_SCMO_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_SHFT                  0x0

#define BIMC_S_SCMO_CONFIGURATION_INFO_2_ADDR(b,n)                           (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000040)
#define BIMC_S_SCMO_CONFIGURATION_INFO_2_RMSK                                0xff00ff
#define BIMC_S_SCMO_CONFIGURATION_INFO_2_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_2_ADDR(b,n), BIMC_S_SCMO_CONFIGURATION_INFO_2_RMSK)
#define BIMC_S_SCMO_CONFIGURATION_INFO_2_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_2_ADDR(b,n), m)
#define BIMC_S_SCMO_CONFIGURATION_INFO_2_NUM_GLOBAL_MONS_BMSK                0xff0000
#define BIMC_S_SCMO_CONFIGURATION_INFO_2_NUM_GLOBAL_MONS_SHFT                    0x10
#define BIMC_S_SCMO_CONFIGURATION_INFO_2_VMID_WIDTH_BMSK                         0xff
#define BIMC_S_SCMO_CONFIGURATION_INFO_2_VMID_WIDTH_SHFT                          0x0

#define BIMC_S_SCMO_CONFIGURATION_INFO_3_ADDR(b,n)                           (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000050)
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_RMSK                              0xffffffff
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_3_ADDR(b,n), BIMC_S_SCMO_CONFIGURATION_INFO_3_RMSK)
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_3_ADDR(b,n), m)
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_RCH0_CTRL_DEPTH_BMSK              0xff000000
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_RCH0_CTRL_DEPTH_SHFT                    0x18
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_RCH0_DEPTH_BMSK                     0xff0000
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_RCH0_DEPTH_SHFT                         0x10
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                        0xff00
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                           0x8
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                          0xff
#define BIMC_S_SCMO_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                           0x0

#define BIMC_S_SCMO_CONFIGURATION_INFO_4_ADDR(b,n)                           (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000060)
#define BIMC_S_SCMO_CONFIGURATION_INFO_4_RMSK                                  0xffff
#define BIMC_S_SCMO_CONFIGURATION_INFO_4_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_4_ADDR(b,n), BIMC_S_SCMO_CONFIGURATION_INFO_4_RMSK)
#define BIMC_S_SCMO_CONFIGURATION_INFO_4_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_4_ADDR(b,n), m)
#define BIMC_S_SCMO_CONFIGURATION_INFO_4_RCH1_CTRL_DEPTH_BMSK                  0xff00
#define BIMC_S_SCMO_CONFIGURATION_INFO_4_RCH1_CTRL_DEPTH_SHFT                     0x8
#define BIMC_S_SCMO_CONFIGURATION_INFO_4_RCH1_DEPTH_BMSK                         0xff
#define BIMC_S_SCMO_CONFIGURATION_INFO_4_RCH1_DEPTH_SHFT                          0x0

#define BIMC_S_SCMO_CONFIGURATION_INFO_5_ADDR(b,n)                           (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000070)
#define BIMC_S_SCMO_CONFIGURATION_INFO_5_RMSK                                  0xffff
#define BIMC_S_SCMO_CONFIGURATION_INFO_5_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_5_ADDR(b,n), BIMC_S_SCMO_CONFIGURATION_INFO_5_RMSK)
#define BIMC_S_SCMO_CONFIGURATION_INFO_5_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_5_ADDR(b,n), m)
#define BIMC_S_SCMO_CONFIGURATION_INFO_5_DPE_CQ_DEPTH_BMSK                     0xff00
#define BIMC_S_SCMO_CONFIGURATION_INFO_5_DPE_CQ_DEPTH_SHFT                        0x8
#define BIMC_S_SCMO_CONFIGURATION_INFO_5_DDR_BUS_WIDTH_BMSK                      0xff
#define BIMC_S_SCMO_CONFIGURATION_INFO_5_DDR_BUS_WIDTH_SHFT                       0x0

#define BIMC_S_SCMO_CONFIGURATION_INFO_6_ADDR(b,n)                           (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000080)
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_RMSK                                  0x1111
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_6_ADDR(b,n), BIMC_S_SCMO_CONFIGURATION_INFO_6_RMSK)
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CONFIGURATION_INFO_6_ADDR(b,n), m)
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_WBUFC_PIPE_BMSK                       0x1000
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_WBUFC_PIPE_SHFT                          0xc
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_RDOPT_PIPE_BMSK                        0x100
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_RDOPT_PIPE_SHFT                          0x8
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_ACHAN_INTF_PIPE_BMSK                    0x10
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_ACHAN_INTF_PIPE_SHFT                     0x4
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_ADDR_DECODE_HT_BMSK                      0x1
#define BIMC_S_SCMO_CONFIGURATION_INFO_6_ADDR_DECODE_HT_SHFT                      0x0

#define BIMC_S_SCMO_INTERRUPT_STATUS_ADDR(b,n)                               (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000100)
#define BIMC_S_SCMO_INTERRUPT_STATUS_RMSK                                         0x1
#define BIMC_S_SCMO_INTERRUPT_STATUS_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_INTERRUPT_STATUS_ADDR(b,n), BIMC_S_SCMO_INTERRUPT_STATUS_RMSK)
#define BIMC_S_SCMO_INTERRUPT_STATUS_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_INTERRUPT_STATUS_ADDR(b,n), m)
#define BIMC_S_SCMO_INTERRUPT_STATUS_ERR_OCCURRED_BMSK                            0x1
#define BIMC_S_SCMO_INTERRUPT_STATUS_ERR_OCCURRED_SHFT                            0x0

#define BIMC_S_SCMO_INTERRUPT_CLEAR_ADDR(b,n)                                (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000108)
#define BIMC_S_SCMO_INTERRUPT_CLEAR_RMSK                                          0x1
#define BIMC_S_SCMO_INTERRUPT_CLEAR_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_INTERRUPT_CLEAR_ADDR(b,n),v)
#define BIMC_S_SCMO_INTERRUPT_CLEAR_IRQ_CLR_BMSK                                  0x1
#define BIMC_S_SCMO_INTERRUPT_CLEAR_IRQ_CLR_SHFT                                  0x0

#define BIMC_S_SCMO_INTERRUPT_ENABLE_ADDR(b,n)                               (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x0000010c)
#define BIMC_S_SCMO_INTERRUPT_ENABLE_RMSK                                         0x1
#define BIMC_S_SCMO_INTERRUPT_ENABLE_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_INTERRUPT_ENABLE_ADDR(b,n), BIMC_S_SCMO_INTERRUPT_ENABLE_RMSK)
#define BIMC_S_SCMO_INTERRUPT_ENABLE_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_INTERRUPT_ENABLE_ADDR(b,n), m)
#define BIMC_S_SCMO_INTERRUPT_ENABLE_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_INTERRUPT_ENABLE_ADDR(b,n),v)
#define BIMC_S_SCMO_INTERRUPT_ENABLE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_INTERRUPT_ENABLE_ADDR(b,n),m,v,BIMC_S_SCMO_INTERRUPT_ENABLE_INI(b,n))
#define BIMC_S_SCMO_INTERRUPT_ENABLE_IRQ_EN_BMSK                                  0x1
#define BIMC_S_SCMO_INTERRUPT_ENABLE_IRQ_EN_SHFT                                  0x0

#define BIMC_S_SCMO_ESYN_ADDR_ADDR(b,n)                                      (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000120)
#define BIMC_S_SCMO_ESYN_ADDR_RMSK                                         0xffffffff
#define BIMC_S_SCMO_ESYN_ADDR_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_ESYN_ADDR_ADDR(b,n), BIMC_S_SCMO_ESYN_ADDR_RMSK)
#define BIMC_S_SCMO_ESYN_ADDR_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_ESYN_ADDR_ADDR(b,n), m)
#define BIMC_S_SCMO_ESYN_ADDR_ERR_ADDR_BMSK                                0xffffffff
#define BIMC_S_SCMO_ESYN_ADDR_ERR_ADDR_SHFT                                       0x0

#define BIMC_S_SCMO_ESYN_APACKET_0_ADDR(b,n)                                 (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000128)
#define BIMC_S_SCMO_ESYN_APACKET_0_RMSK                                    0xff1fffff
#define BIMC_S_SCMO_ESYN_APACKET_0_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_ESYN_APACKET_0_ADDR(b,n), BIMC_S_SCMO_ESYN_APACKET_0_RMSK)
#define BIMC_S_SCMO_ESYN_APACKET_0_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_ESYN_APACKET_0_ADDR(b,n), m)
#define BIMC_S_SCMO_ESYN_APACKET_0_ERR_ATID_BMSK                           0xff000000
#define BIMC_S_SCMO_ESYN_APACKET_0_ERR_ATID_SHFT                                 0x18
#define BIMC_S_SCMO_ESYN_APACKET_0_ERR_AVMID_BMSK                            0x1f0000
#define BIMC_S_SCMO_ESYN_APACKET_0_ERR_AVMID_SHFT                                0x10
#define BIMC_S_SCMO_ESYN_APACKET_0_ERR_AMID_BMSK                               0xffff
#define BIMC_S_SCMO_ESYN_APACKET_0_ERR_AMID_SHFT                                  0x0

#define BIMC_S_SCMO_ESYN_APACKET_1_ADDR(b,n)                                 (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x0000012c)
#define BIMC_S_SCMO_ESYN_APACKET_1_RMSK                                     0x10ff117
#define BIMC_S_SCMO_ESYN_APACKET_1_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_ESYN_APACKET_1_ADDR(b,n), BIMC_S_SCMO_ESYN_APACKET_1_RMSK)
#define BIMC_S_SCMO_ESYN_APACKET_1_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_ESYN_APACKET_1_ADDR(b,n), m)
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_CODE_BMSK                            0x1000000
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_CODE_SHFT                                 0x18
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_ALEN_BMSK                              0xf0000
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_ALEN_SHFT                                 0x10
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_ASIZE_BMSK                              0xe000
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_ASIZE_SHFT                                 0xd
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_ABURST_BMSK                             0x1000
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_ABURST_SHFT                                0xc
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_AEXCLUSIVE_BMSK                          0x100
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_AEXCLUSIVE_SHFT                            0x8
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_APROTNS_BMSK                              0x10
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_APROTNS_SHFT                               0x4
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_AOOORD_BMSK                                0x4
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_AOOORD_SHFT                                0x2
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_AOOOWR_BMSK                                0x2
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_AOOOWR_SHFT                                0x1
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_AWRITE_BMSK                                0x1
#define BIMC_S_SCMO_ESYN_APACKET_1_ERR_AWRITE_SHFT                                0x0

#define BIMC_S_SCMO_CLOCK_CTRL_ADDR(b,n)                                     (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000200)
#define BIMC_S_SCMO_CLOCK_CTRL_RMSK                                        0xffff1111
#define BIMC_S_SCMO_CLOCK_CTRL_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CLOCK_CTRL_ADDR(b,n), BIMC_S_SCMO_CLOCK_CTRL_RMSK)
#define BIMC_S_SCMO_CLOCK_CTRL_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CLOCK_CTRL_ADDR(b,n), m)
#define BIMC_S_SCMO_CLOCK_CTRL_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_CLOCK_CTRL_ADDR(b,n),v)
#define BIMC_S_SCMO_CLOCK_CTRL_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_CLOCK_CTRL_ADDR(b,n),m,v,BIMC_S_SCMO_CLOCK_CTRL_INI(b,n))
#define BIMC_S_SCMO_CLOCK_CTRL_PEN_CMD_CG_EN_BMSK                             0x10000
#define BIMC_S_SCMO_CLOCK_CTRL_PEN_CMD_CG_EN_SHFT                                0x10
#define BIMC_S_SCMO_CLOCK_CTRL_RCH_CG_EN_BMSK                                  0x1000
#define BIMC_S_SCMO_CLOCK_CTRL_RCH_CG_EN_SHFT                                     0xc
#define BIMC_S_SCMO_CLOCK_CTRL_FLUSH_CG_EN_BMSK                                 0x100
#define BIMC_S_SCMO_CLOCK_CTRL_FLUSH_CG_EN_SHFT                                   0x8
#define BIMC_S_SCMO_CLOCK_CTRL_WCH_CG_EN_BMSK                                    0x10
#define BIMC_S_SCMO_CLOCK_CTRL_WCH_CG_EN_SHFT                                     0x4
#define BIMC_S_SCMO_CLOCK_CTRL_ACH_CG_EN_BMSK                                     0x1
#define BIMC_S_SCMO_CLOCK_CTRL_ACH_CG_EN_SHFT                                     0x0

#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_ADDR(b,n)                             (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000400)
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_RMSK                                      0xff
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_SLV_INTERLEAVE_CFG_ADDR(b,n), BIMC_S_SCMO_SLV_INTERLEAVE_CFG_RMSK)
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_SLV_INTERLEAVE_CFG_ADDR(b,n), m)
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_SLV_INTERLEAVE_CFG_ADDR(b,n),v)
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_SLV_INTERLEAVE_CFG_ADDR(b,n),m,v,BIMC_S_SCMO_SLV_INTERLEAVE_CFG_INI(b,n))
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS1_BMSK                       0x10
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS1_SHFT                        0x4
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS0_BMSK                        0x1
#define BIMC_S_SCMO_SLV_INTERLEAVE_CFG_INTERLEAVE_CS0_SHFT                        0x0

#define BIMC_S_SCMO_ADDR_BASE_CSn_ADDR(b,n,o)                               (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000410 + 0x4 * (o))
#define BIMC_S_SCMO_ADDR_BASE_CSn_RMSK                                         0xffff
#define BIMC_S_SCMO_ADDR_BASE_CSn_MAXn                                              1
#define BIMC_S_SCMO_ADDR_BASE_CSn_INI2(b,n,o)        \
        in_dword_masked(BIMC_S_SCMO_ADDR_BASE_CSn_ADDR(b,n,o), BIMC_S_SCMO_ADDR_BASE_CSn_RMSK)
#define BIMC_S_SCMO_ADDR_BASE_CSn_INMI2(b,n,o,mask)    \
        in_dword_masked(BIMC_S_SCMO_ADDR_BASE_CSn_ADDR(b,n,o), (mask))
#define BIMC_S_SCMO_ADDR_BASE_CSn_OUTI2(b,n,o,val)    \
        out_dword(BIMC_S_SCMO_ADDR_BASE_CSn_ADDR(b,n,o),val)
#define BIMC_S_SCMO_ADDR_BASE_CSn_OUTIM2(b,n,o,mask,val) \
        out_dword_masked_ns(BIMC_S_SCMO_ADDR_BASE_CSn_ADDR(b,n,o),mask,val,BIMC_S_SCMO_ADDR_BASE_CSn_INI2(b,n,o))
#define BIMC_S_SCMO_ADDR_BASE_CSn_ADDR_BASE_BMSK                                 0xfc
#define BIMC_S_SCMO_ADDR_BASE_CSn_ADDR_BASE_SHFT                                  0x2

#define BIMC_S_SCMO_ADDR_MAP_CSn_ADDR(b,n,o)                                (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000420 + 0x4 * (o))
#define BIMC_S_SCMO_ADDR_MAP_CSn_RMSK                                          0xffff
#define BIMC_S_SCMO_ADDR_MAP_CSn_MAXn                                               1
#define BIMC_S_SCMO_ADDR_MAP_CSn_INI2(b,n,o)        \
        in_dword_masked(BIMC_S_SCMO_ADDR_MAP_CSn_ADDR(b,n,o), BIMC_S_SCMO_ADDR_MAP_CSn_RMSK)
#define BIMC_S_SCMO_ADDR_MAP_CSn_INMI2(b,n,o,mask)    \
        in_dword_masked(BIMC_S_SCMO_ADDR_MAP_CSn_ADDR(b,n,o), (mask))
#define BIMC_S_SCMO_ADDR_MAP_CSn_OUTI2(b,n,o,val)    \
        out_dword(BIMC_S_SCMO_ADDR_MAP_CSn_ADDR(b,n,o),val)
#define BIMC_S_SCMO_ADDR_MAP_CSn_OUTIM2(b,n,o,mask,val) \
        out_dword_masked_ns(BIMC_S_SCMO_ADDR_MAP_CSn_ADDR(b,n,o),mask,val,BIMC_S_SCMO_ADDR_MAP_CSn_INI2(b,n,o))
#define BIMC_S_SCMO_ADDR_MAP_CSn_RANK_EN_BMSK                                  0x8000
#define BIMC_S_SCMO_ADDR_MAP_CSn_RANK_EN_SHFT                                     0xf
#define BIMC_S_SCMO_ADDR_MAP_CSn_ADDR_MODE_BMSK                                0x1000
#define BIMC_S_SCMO_ADDR_MAP_CSn_ADDR_MODE_SHFT                                   0xc
#define BIMC_S_SCMO_ADDR_MAP_CSn_BANK_SIZE_BMSK                                 0x100
#define BIMC_S_SCMO_ADDR_MAP_CSn_BANK_SIZE_SHFT                                   0x8
#define BIMC_S_SCMO_ADDR_MAP_CSn_ROW_SIZE_BMSK                                   0x30
#define BIMC_S_SCMO_ADDR_MAP_CSn_ROW_SIZE_SHFT                                    0x4
#define BIMC_S_SCMO_ADDR_MAP_CSn_COL_SIZE_BMSK                                    0x3
#define BIMC_S_SCMO_ADDR_MAP_CSn_COL_SIZE_SHFT                                    0x0

#define BIMC_S_SCMO_ADDR_MASK_CSn_ADDR(b,n,o)                               (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000430 + 0x4 * (o))
#define BIMC_S_SCMO_ADDR_MASK_CSn_RMSK                                         0xffff
#define BIMC_S_SCMO_ADDR_MASK_CSn_MAXn                                              1
#define BIMC_S_SCMO_ADDR_MASK_CSn_INI2(b,n,o)        \
        in_dword_masked(BIMC_S_SCMO_ADDR_MASK_CSn_ADDR(b,n,o), BIMC_S_SCMO_ADDR_MASK_CSn_RMSK)
#define BIMC_S_SCMO_ADDR_MASK_CSn_INMI2(b,n,o,mask)    \
        in_dword_masked(BIMC_S_SCMO_ADDR_MASK_CSn_ADDR(b,n,o), (mask))
#define BIMC_S_SCMO_ADDR_MASK_CSn_OUTI2(b,n,o,val)    \
        out_dword(BIMC_S_SCMO_ADDR_MASK_CSn_ADDR(b,n,o),val)
#define BIMC_S_SCMO_ADDR_MASK_CSn_OUTIM2(b,n,o,mask,val) \
        out_dword_masked_ns(BIMC_S_SCMO_ADDR_MASK_CSn_ADDR(b,n,o),mask,val,BIMC_S_SCMO_ADDR_MASK_CSn_INI2(b,n,o))
#define BIMC_S_SCMO_ADDR_MASK_CSn_ADDR_MASK_BMSK                                 0xfc
#define BIMC_S_SCMO_ADDR_MASK_CSn_ADDR_MASK_SHFT                                  0x2

#define BIMC_S_SCMO_SLV_STATUS_ADDR(b,n)                                     (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000450)
#define BIMC_S_SCMO_SLV_STATUS_RMSK                                            0xff31
#define BIMC_S_SCMO_SLV_STATUS_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_SLV_STATUS_ADDR(b,n), BIMC_S_SCMO_SLV_STATUS_RMSK)
#define BIMC_S_SCMO_SLV_STATUS_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_SLV_STATUS_ADDR(b,n), m)
#define BIMC_S_SCMO_SLV_STATUS_GLOBAL_MONS_IN_USE_BMSK                         0xff00
#define BIMC_S_SCMO_SLV_STATUS_GLOBAL_MONS_IN_USE_SHFT                            0x8
#define BIMC_S_SCMO_SLV_STATUS_RANK_IDLE_BMSK                                    0x30
#define BIMC_S_SCMO_SLV_STATUS_RANK_IDLE_SHFT                                     0x4
#define BIMC_S_SCMO_SLV_STATUS_SLAVE_IDLE_BMSK                                    0x1
#define BIMC_S_SCMO_SLV_STATUS_SLAVE_IDLE_SHFT                                    0x0

#define BIMC_S_SCMO_CMD_BUF_CFG_ADDR(b,n)                                    (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000500)
#define BIMC_S_SCMO_CMD_BUF_CFG_RMSK                                            0xf1f
#define BIMC_S_SCMO_CMD_BUF_CFG_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CMD_BUF_CFG_ADDR(b,n), BIMC_S_SCMO_CMD_BUF_CFG_RMSK)
#define BIMC_S_SCMO_CMD_BUF_CFG_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CMD_BUF_CFG_ADDR(b,n), m)
#define BIMC_S_SCMO_CMD_BUF_CFG_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_CMD_BUF_CFG_ADDR(b,n),v)
#define BIMC_S_SCMO_CMD_BUF_CFG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_CMD_BUF_CFG_ADDR(b,n),m,v,BIMC_S_SCMO_CMD_BUF_CFG_INI(b,n))
#define BIMC_S_SCMO_CMD_BUF_CFG_CMD_ORDERING_BMSK                               0x300
#define BIMC_S_SCMO_CMD_BUF_CFG_CMD_ORDERING_SHFT                                 0x8
#define BIMC_S_SCMO_CMD_BUF_CFG_HP_CMD_AREQPRIO_MAP_BMSK                         0x10
#define BIMC_S_SCMO_CMD_BUF_CFG_HP_CMD_AREQPRIO_MAP_SHFT                          0x4
#define BIMC_S_SCMO_CMD_BUF_CFG_HP_CMD_Q_DEPTH_BMSK                               0x7
#define BIMC_S_SCMO_CMD_BUF_CFG_HP_CMD_Q_DEPTH_SHFT                               0x0

#define BIMC_S_SCMO_CMD_BUF_STATUS_ADDR(b,n)                                 (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000520)
#define BIMC_S_SCMO_CMD_BUF_STATUS_RMSK                                          0xff
#define BIMC_S_SCMO_CMD_BUF_STATUS_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CMD_BUF_STATUS_ADDR(b,n), BIMC_S_SCMO_CMD_BUF_STATUS_RMSK)
#define BIMC_S_SCMO_CMD_BUF_STATUS_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CMD_BUF_STATUS_ADDR(b,n), m)
#define BIMC_S_SCMO_CMD_BUF_STATUS_HP_CMD_BUF_ENTRIES_IN_USE_BMSK                0xf0
#define BIMC_S_SCMO_CMD_BUF_STATUS_HP_CMD_BUF_ENTRIES_IN_USE_SHFT                 0x4
#define BIMC_S_SCMO_CMD_BUF_STATUS_LP_CMD_BUF_ENTRIES_IN_USE_BMSK                 0xf
#define BIMC_S_SCMO_CMD_BUF_STATUS_LP_CMD_BUF_ENTRIES_IN_USE_SHFT                 0x0

#define BIMC_S_SCMO_RCH_SELECT_ADDR(b,n)                                     (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000540)
#define BIMC_S_SCMO_RCH_SELECT_RMSK                                        0xffffffff
#define BIMC_S_SCMO_RCH_SELECT_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_RCH_SELECT_ADDR(b,n), BIMC_S_SCMO_RCH_SELECT_RMSK)
#define BIMC_S_SCMO_RCH_SELECT_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_RCH_SELECT_ADDR(b,n), m)
#define BIMC_S_SCMO_RCH_SELECT_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_RCH_SELECT_ADDR(b,n),v)
#define BIMC_S_SCMO_RCH_SELECT_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_RCH_SELECT_ADDR(b,n),m,v,BIMC_S_SCMO_RCH_SELECT_INI(b,n))
#define BIMC_S_SCMO_RCH_SELECT_RCH_PORTS_BMSK                              0xffffffff
#define BIMC_S_SCMO_RCH_SELECT_RCH_PORTS_SHFT                                     0x0

#define BIMC_S_SCMO_RCH_BKPR_CFG_ADDR(b,n)                                   (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000544)
#define BIMC_S_SCMO_RCH_BKPR_CFG_RMSK                                      0xffffffff
#define BIMC_S_SCMO_RCH_BKPR_CFG_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_RCH_BKPR_CFG_ADDR(b,n), BIMC_S_SCMO_RCH_BKPR_CFG_RMSK)
#define BIMC_S_SCMO_RCH_BKPR_CFG_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_RCH_BKPR_CFG_ADDR(b,n), m)
#define BIMC_S_SCMO_RCH_BKPR_CFG_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_RCH_BKPR_CFG_ADDR(b,n),v)
#define BIMC_S_SCMO_RCH_BKPR_CFG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_RCH_BKPR_CFG_ADDR(b,n),m,v,BIMC_S_SCMO_RCH_BKPR_CFG_INI(b,n))
#define BIMC_S_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_HI_TH_BMSK                 0x3f000000
#define BIMC_S_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_HI_TH_SHFT                       0x18
#define BIMC_S_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_LO_TH_BMSK                   0x3f0000
#define BIMC_S_SCMO_RCH_BKPR_CFG_RCH1_FIFO_BKPR_LO_TH_SHFT                       0x10
#define BIMC_S_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_HI_TH_BMSK                     0x3f00
#define BIMC_S_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_HI_TH_SHFT                        0x8
#define BIMC_S_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_LO_TH_BMSK                       0x3f
#define BIMC_S_SCMO_RCH_BKPR_CFG_RCH0_FIFO_BKPR_LO_TH_SHFT                        0x0

#define BIMC_S_SCMO_RCH_STATUS_ADDR(b,n)                                     (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000560)
#define BIMC_S_SCMO_RCH_STATUS_RMSK                                           0x33333
#define BIMC_S_SCMO_RCH_STATUS_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_RCH_STATUS_ADDR(b,n), BIMC_S_SCMO_RCH_STATUS_RMSK)
#define BIMC_S_SCMO_RCH_STATUS_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_RCH_STATUS_ADDR(b,n), m)
#define BIMC_S_SCMO_RCH_STATUS_PRQ_FIFO_FULL_BMSK                             0x20000
#define BIMC_S_SCMO_RCH_STATUS_PRQ_FIFO_FULL_SHFT                                0x11
#define BIMC_S_SCMO_RCH_STATUS_PRQ_FIFO_EMPTY_BMSK                            0x10000
#define BIMC_S_SCMO_RCH_STATUS_PRQ_FIFO_EMPTY_SHFT                               0x10
#define BIMC_S_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_FULL_BMSK                        0x2000
#define BIMC_S_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_FULL_SHFT                           0xd
#define BIMC_S_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_EMPTY_BMSK                       0x1000
#define BIMC_S_SCMO_RCH_STATUS_RCH1_QUAL_FIFO_EMPTY_SHFT                          0xc
#define BIMC_S_SCMO_RCH_STATUS_RCH1_DATA_FIFO_FULL_BMSK                         0x200
#define BIMC_S_SCMO_RCH_STATUS_RCH1_DATA_FIFO_FULL_SHFT                           0x9
#define BIMC_S_SCMO_RCH_STATUS_RCH1_DATA_FIFO_EMPTY_BMSK                        0x100
#define BIMC_S_SCMO_RCH_STATUS_RCH1_DATA_FIFO_EMPTY_SHFT                          0x8
#define BIMC_S_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_FULL_BMSK                          0x20
#define BIMC_S_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_FULL_SHFT                           0x5
#define BIMC_S_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_EMPTY_BMSK                         0x10
#define BIMC_S_SCMO_RCH_STATUS_RCH0_QUAL_FIFO_EMPTY_SHFT                          0x4
#define BIMC_S_SCMO_RCH_STATUS_RCH0_DATA_FIFO_FULL_BMSK                           0x2
#define BIMC_S_SCMO_RCH_STATUS_RCH0_DATA_FIFO_FULL_SHFT                           0x1
#define BIMC_S_SCMO_RCH_STATUS_RCH0_DATA_FIFO_EMPTY_BMSK                          0x1
#define BIMC_S_SCMO_RCH_STATUS_RCH0_DATA_FIFO_EMPTY_SHFT                          0x0

#define BIMC_S_SCMO_WCH_BUF_CFG_ADDR(b,n)                                    (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000580)
#define BIMC_S_SCMO_WCH_BUF_CFG_RMSK                                             0xff
#define BIMC_S_SCMO_WCH_BUF_CFG_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_WCH_BUF_CFG_ADDR(b,n), BIMC_S_SCMO_WCH_BUF_CFG_RMSK)
#define BIMC_S_SCMO_WCH_BUF_CFG_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_WCH_BUF_CFG_ADDR(b,n), m)
#define BIMC_S_SCMO_WCH_BUF_CFG_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_WCH_BUF_CFG_ADDR(b,n),v)
#define BIMC_S_SCMO_WCH_BUF_CFG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_WCH_BUF_CFG_ADDR(b,n),m,v,BIMC_S_SCMO_WCH_BUF_CFG_INI(b,n))
#define BIMC_S_SCMO_WCH_BUF_CFG_WRITE_BLOCK_READ_BMSK                            0x10
#define BIMC_S_SCMO_WCH_BUF_CFG_WRITE_BLOCK_READ_SHFT                             0x4
#define BIMC_S_SCMO_WCH_BUF_CFG_COALESCE_EN_BMSK                                  0x1
#define BIMC_S_SCMO_WCH_BUF_CFG_COALESCE_EN_SHFT                                  0x0

#define BIMC_S_SCMO_WCH_STATUS_ADDR(b,n)                                     (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x000005a0)
#define BIMC_S_SCMO_WCH_STATUS_RMSK                                             0x333
#define BIMC_S_SCMO_WCH_STATUS_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_WCH_STATUS_ADDR(b,n), BIMC_S_SCMO_WCH_STATUS_RMSK)
#define BIMC_S_SCMO_WCH_STATUS_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_WCH_STATUS_ADDR(b,n), m)
#define BIMC_S_SCMO_WCH_STATUS_BRESP_FIFO_FULL_BMSK                             0x200
#define BIMC_S_SCMO_WCH_STATUS_BRESP_FIFO_FULL_SHFT                               0x9
#define BIMC_S_SCMO_WCH_STATUS_BRESP_FIFO_EMPTY_BMSK                            0x100
#define BIMC_S_SCMO_WCH_STATUS_BRESP_FIFO_EMPTY_SHFT                              0x8
#define BIMC_S_SCMO_WCH_STATUS_WDATA_FIFO_FULL_BMSK                              0x20
#define BIMC_S_SCMO_WCH_STATUS_WDATA_FIFO_FULL_SHFT                               0x5
#define BIMC_S_SCMO_WCH_STATUS_WDATA_FIFO_EMPTY_BMSK                             0x10
#define BIMC_S_SCMO_WCH_STATUS_WDATA_FIFO_EMPTY_SHFT                              0x4
#define BIMC_S_SCMO_WCH_STATUS_WBUF_FULL_BMSK                                     0x2
#define BIMC_S_SCMO_WCH_STATUS_WBUF_FULL_SHFT                                     0x1
#define BIMC_S_SCMO_WCH_STATUS_WBUF_EMPTY_BMSK                                    0x1
#define BIMC_S_SCMO_WCH_STATUS_WBUF_EMPTY_SHFT                                    0x0

#define BIMC_S_SCMO_FLUSH_CFG_ADDR(b,n)                                      (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x000005c0)
#define BIMC_S_SCMO_FLUSH_CFG_RMSK                                         0xffffffff
#define BIMC_S_SCMO_FLUSH_CFG_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_FLUSH_CFG_ADDR(b,n), BIMC_S_SCMO_FLUSH_CFG_RMSK)
#define BIMC_S_SCMO_FLUSH_CFG_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_FLUSH_CFG_ADDR(b,n), m)
#define BIMC_S_SCMO_FLUSH_CFG_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_FLUSH_CFG_ADDR(b,n),v)
#define BIMC_S_SCMO_FLUSH_CFG_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_FLUSH_CFG_ADDR(b,n),m,v,BIMC_S_SCMO_FLUSH_CFG_INI(b,n))
#define BIMC_S_SCMO_FLUSH_CFG_FLUSH_IN_ORDER_BMSK                          0x10000000
#define BIMC_S_SCMO_FLUSH_CFG_FLUSH_IN_ORDER_SHFT                                0x1c
#define BIMC_S_SCMO_FLUSH_CFG_FLUSH_IDLE_DELAY_BMSK                         0x3ff0000
#define BIMC_S_SCMO_FLUSH_CFG_FLUSH_IDLE_DELAY_SHFT                              0x10
#define BIMC_S_SCMO_FLUSH_CFG_FLUSH_UPPER_LIMIT_BMSK                            0xf00
#define BIMC_S_SCMO_FLUSH_CFG_FLUSH_UPPER_LIMIT_SHFT                              0x8
#define BIMC_S_SCMO_FLUSH_CFG_FLUSH_LOWER_LIMIT_BMSK                              0xf
#define BIMC_S_SCMO_FLUSH_CFG_FLUSH_LOWER_LIMIT_SHFT                              0x0

#define BIMC_S_SCMO_FLUSH_CMD_ADDR(b,n)                                      (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x000005c4)
#define BIMC_S_SCMO_FLUSH_CMD_RMSK                                                0xf
#define BIMC_S_SCMO_FLUSH_CMD_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_FLUSH_CMD_ADDR(b,n), BIMC_S_SCMO_FLUSH_CMD_RMSK)
#define BIMC_S_SCMO_FLUSH_CMD_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_FLUSH_CMD_ADDR(b,n), m)
#define BIMC_S_SCMO_FLUSH_CMD_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_FLUSH_CMD_ADDR(b,n),v)
#define BIMC_S_SCMO_FLUSH_CMD_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_FLUSH_CMD_ADDR(b,n),m,v,BIMC_S_SCMO_FLUSH_CMD_INI(b,n))
#define BIMC_S_SCMO_FLUSH_CMD_FLUSH_ALL_BUF_BMSK                                  0x3
#define BIMC_S_SCMO_FLUSH_CMD_FLUSH_ALL_BUF_SHFT                                  0x0

#define BIMC_S_SCMO_CMD_OPT_CFG0_ADDR(b,n)                                   (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000700)
#define BIMC_S_SCMO_CMD_OPT_CFG0_RMSK                                        0xffffff
#define BIMC_S_SCMO_CMD_OPT_CFG0_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CMD_OPT_CFG0_ADDR(b,n), BIMC_S_SCMO_CMD_OPT_CFG0_RMSK)
#define BIMC_S_SCMO_CMD_OPT_CFG0_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CMD_OPT_CFG0_ADDR(b,n), m)
#define BIMC_S_SCMO_CMD_OPT_CFG0_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_CMD_OPT_CFG0_ADDR(b,n),v)
#define BIMC_S_SCMO_CMD_OPT_CFG0_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_CMD_OPT_CFG0_ADDR(b,n),m,v,BIMC_S_SCMO_CMD_OPT_CFG0_INI(b,n))
#define BIMC_S_SCMO_CMD_OPT_CFG0_IGNORE_BANK_UNAVL_BMSK                      0x100000
#define BIMC_S_SCMO_CMD_OPT_CFG0_IGNORE_BANK_UNAVL_SHFT                          0x14
#define BIMC_S_SCMO_CMD_OPT_CFG0_MASK_CMDOUT_PRI_BMSK                         0x10000
#define BIMC_S_SCMO_CMD_OPT_CFG0_MASK_CMDOUT_PRI_SHFT                            0x10
#define BIMC_S_SCMO_CMD_OPT_CFG0_DPE_CMD_REORDERING_BMSK                       0x1000
#define BIMC_S_SCMO_CMD_OPT_CFG0_DPE_CMD_REORDERING_SHFT                          0xc
#define BIMC_S_SCMO_CMD_OPT_CFG0_WR_OPT_EN_BMSK                                 0x100
#define BIMC_S_SCMO_CMD_OPT_CFG0_WR_OPT_EN_SHFT                                   0x8
#define BIMC_S_SCMO_CMD_OPT_CFG0_RD_OPT_EN_BMSK                                  0x10
#define BIMC_S_SCMO_CMD_OPT_CFG0_RD_OPT_EN_SHFT                                   0x4
#define BIMC_S_SCMO_CMD_OPT_CFG0_PAGE_MGMT_POLICY_BMSK                            0x1
#define BIMC_S_SCMO_CMD_OPT_CFG0_PAGE_MGMT_POLICY_SHFT                            0x0

#define BIMC_S_SCMO_CMD_OPT_CFG1_ADDR(b,n)                                   (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000704)
#define BIMC_S_SCMO_CMD_OPT_CFG1_RMSK                                      0xffffffff
#define BIMC_S_SCMO_CMD_OPT_CFG1_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CMD_OPT_CFG1_ADDR(b,n), BIMC_S_SCMO_CMD_OPT_CFG1_RMSK)
#define BIMC_S_SCMO_CMD_OPT_CFG1_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CMD_OPT_CFG1_ADDR(b,n), m)
#define BIMC_S_SCMO_CMD_OPT_CFG1_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_CMD_OPT_CFG1_ADDR(b,n),v)
#define BIMC_S_SCMO_CMD_OPT_CFG1_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_CMD_OPT_CFG1_ADDR(b,n),m,v,BIMC_S_SCMO_CMD_OPT_CFG1_INI(b,n))
#define BIMC_S_SCMO_CMD_OPT_CFG1_HSTP_CMD_TIMEOUT_BMSK                     0x1f000000
#define BIMC_S_SCMO_CMD_OPT_CFG1_HSTP_CMD_TIMEOUT_SHFT                           0x18
#define BIMC_S_SCMO_CMD_OPT_CFG1_HP_CMD_TIMEOUT_BMSK                         0x1f0000
#define BIMC_S_SCMO_CMD_OPT_CFG1_HP_CMD_TIMEOUT_SHFT                             0x10
#define BIMC_S_SCMO_CMD_OPT_CFG1_MP_CMD_TIMEOUT_BMSK                           0x1f00
#define BIMC_S_SCMO_CMD_OPT_CFG1_MP_CMD_TIMEOUT_SHFT                              0x8
#define BIMC_S_SCMO_CMD_OPT_CFG1_LP_CMD_TIMEOUT_BMSK                             0x1f
#define BIMC_S_SCMO_CMD_OPT_CFG1_LP_CMD_TIMEOUT_SHFT                              0x0

#define BIMC_S_SCMO_CMD_OPT_CFG2_ADDR(b,n)                                   (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x00000708)
#define BIMC_S_SCMO_CMD_OPT_CFG2_RMSK                                          0xffff
#define BIMC_S_SCMO_CMD_OPT_CFG2_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CMD_OPT_CFG2_ADDR(b,n), BIMC_S_SCMO_CMD_OPT_CFG2_RMSK)
#define BIMC_S_SCMO_CMD_OPT_CFG2_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CMD_OPT_CFG2_ADDR(b,n), m)
#define BIMC_S_SCMO_CMD_OPT_CFG2_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_CMD_OPT_CFG2_ADDR(b,n),v)
#define BIMC_S_SCMO_CMD_OPT_CFG2_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_CMD_OPT_CFG2_ADDR(b,n),m,v,BIMC_S_SCMO_CMD_OPT_CFG2_INI(b,n))
#define BIMC_S_SCMO_CMD_OPT_CFG2_RWOPT_WR_CMD_TIMEOUT_BMSK                      0xf00
#define BIMC_S_SCMO_CMD_OPT_CFG2_RWOPT_WR_CMD_TIMEOUT_SHFT                        0x8
#define BIMC_S_SCMO_CMD_OPT_CFG2_RWOPT_RD_CMD_TIMEOUT_BMSK                        0xf
#define BIMC_S_SCMO_CMD_OPT_CFG2_RWOPT_RD_CMD_TIMEOUT_SHFT                        0x0

#define BIMC_S_SCMO_CMD_OPT_CFG3_ADDR(b,n)                                   (BIMC_S_SCMO_REG_BASE(b) + (0x8000 * (n)) + 0x0000070c)
#define BIMC_S_SCMO_CMD_OPT_CFG3_RMSK                                            0xff
#define BIMC_S_SCMO_CMD_OPT_CFG3_INI(b,n)    \
        in_dword_masked(BIMC_S_SCMO_CMD_OPT_CFG3_ADDR(b,n), BIMC_S_SCMO_CMD_OPT_CFG3_RMSK)
#define BIMC_S_SCMO_CMD_OPT_CFG3_INMI(b,n,m) \
        in_dword_masked(BIMC_S_SCMO_CMD_OPT_CFG3_ADDR(b,n), m)
#define BIMC_S_SCMO_CMD_OPT_CFG3_OUTI(b,n,v)    \
        out_dword(BIMC_S_SCMO_CMD_OPT_CFG3_ADDR(b,n),v)
#define BIMC_S_SCMO_CMD_OPT_CFG3_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SCMO_CMD_OPT_CFG3_ADDR(b,n),m,v,BIMC_S_SCMO_CMD_OPT_CFG3_INI(b,n))
#define BIMC_S_SCMO_CMD_OPT_CFG3_FLUSH_CMD_TIMEOUT_BMSK                          0x1f
#define BIMC_S_SCMO_CMD_OPT_CFG3_FLUSH_CMD_TIMEOUT_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_SWAY_GENERIC
 *--------------------------------------------------------------------------*/
#define BIMC_S_SWAY_REG_BASE(b)                                                 ((b)      + 0x00048000)

#define BIMC_S_SWAY_CONFIGURATION_INFO_0_ADDR(b,n)                        (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000020)
#define BIMC_S_SWAY_CONFIGURATION_INFO_0_RMSK                             0xff0000ff
#define BIMC_S_SWAY_CONFIGURATION_INFO_0_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_0_ADDR(b,n), BIMC_S_SWAY_CONFIGURATION_INFO_0_RMSK)
#define BIMC_S_SWAY_CONFIGURATION_INFO_0_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_0_ADDR(b,n), m)
#define BIMC_S_SWAY_CONFIGURATION_INFO_0_SYNC_MODE_BMSK                   0xff000000
#define BIMC_S_SWAY_CONFIGURATION_INFO_0_SYNC_MODE_SHFT                         0x18
#define BIMC_S_SWAY_CONFIGURATION_INFO_0_FUNCTIONALITY_BMSK                     0xff
#define BIMC_S_SWAY_CONFIGURATION_INFO_0_FUNCTIONALITY_SHFT                      0x0

#define BIMC_S_SWAY_CONFIGURATION_INFO_1_ADDR(b,n)                        (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000030)
#define BIMC_S_SWAY_CONFIGURATION_INFO_1_RMSK                             0xffffffff
#define BIMC_S_SWAY_CONFIGURATION_INFO_1_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_1_ADDR(b,n), BIMC_S_SWAY_CONFIGURATION_INFO_1_RMSK)
#define BIMC_S_SWAY_CONFIGURATION_INFO_1_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_1_ADDR(b,n), m)
#define BIMC_S_SWAY_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_BMSK          0xffffffff
#define BIMC_S_SWAY_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_SHFT                 0x0

#define BIMC_S_SWAY_CONFIGURATION_INFO_2_ADDR(b,n)                        (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000040)
#define BIMC_S_SWAY_CONFIGURATION_INFO_2_RMSK                             0xffff0000
#define BIMC_S_SWAY_CONFIGURATION_INFO_2_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_2_ADDR(b,n), BIMC_S_SWAY_CONFIGURATION_INFO_2_RMSK)
#define BIMC_S_SWAY_CONFIGURATION_INFO_2_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_2_ADDR(b,n), m)
#define BIMC_S_SWAY_CONFIGURATION_INFO_2_S_DATA_WIDTH_BMSK                0xffff0000
#define BIMC_S_SWAY_CONFIGURATION_INFO_2_S_DATA_WIDTH_SHFT                      0x10

#define BIMC_S_SWAY_CONFIGURATION_INFO_3_ADDR(b,n)                        (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000050)
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_RMSK                             0xffffffff
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_3_ADDR(b,n), BIMC_S_SWAY_CONFIGURATION_INFO_3_RMSK)
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_3_ADDR(b,n), m)
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_RCH0_DEPTH_BMSK                  0xff000000
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_RCH0_DEPTH_SHFT                        0x18
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_BCH_DEPTH_BMSK                     0xff0000
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_BCH_DEPTH_SHFT                         0x10
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_WCH_DEPTH_BMSK                       0xff00
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_WCH_DEPTH_SHFT                          0x8
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_ACH_DEPTH_BMSK                         0xff
#define BIMC_S_SWAY_CONFIGURATION_INFO_3_ACH_DEPTH_SHFT                          0x0

#define BIMC_S_SWAY_CONFIGURATION_INFO_4_ADDR(b,n)                        (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000060)
#define BIMC_S_SWAY_CONFIGURATION_INFO_4_RMSK                             0x800000ff
#define BIMC_S_SWAY_CONFIGURATION_INFO_4_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_4_ADDR(b,n), BIMC_S_SWAY_CONFIGURATION_INFO_4_RMSK)
#define BIMC_S_SWAY_CONFIGURATION_INFO_4_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_4_ADDR(b,n), m)
#define BIMC_S_SWAY_CONFIGURATION_INFO_4_DUAL_RCH_EN_BMSK                 0x80000000
#define BIMC_S_SWAY_CONFIGURATION_INFO_4_DUAL_RCH_EN_SHFT                       0x1f
#define BIMC_S_SWAY_CONFIGURATION_INFO_4_RCH1_DEPTH_BMSK                        0xff
#define BIMC_S_SWAY_CONFIGURATION_INFO_4_RCH1_DEPTH_SHFT                         0x0

#define BIMC_S_SWAY_CONFIGURATION_INFO_5_ADDR(b,n)                        (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000070)
#define BIMC_S_SWAY_CONFIGURATION_INFO_5_RMSK                             0x800000ff
#define BIMC_S_SWAY_CONFIGURATION_INFO_5_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_5_ADDR(b,n), BIMC_S_SWAY_CONFIGURATION_INFO_5_RMSK)
#define BIMC_S_SWAY_CONFIGURATION_INFO_5_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_5_ADDR(b,n), m)
#define BIMC_S_SWAY_CONFIGURATION_INFO_5_QCH_EN_BMSK                      0x80000000
#define BIMC_S_SWAY_CONFIGURATION_INFO_5_QCH_EN_SHFT                            0x1f
#define BIMC_S_SWAY_CONFIGURATION_INFO_5_QCH_DEPTH_BMSK                         0xff
#define BIMC_S_SWAY_CONFIGURATION_INFO_5_QCH_DEPTH_SHFT                          0x0

#define BIMC_S_SWAY_CONFIGURATION_INFO_6_ADDR(b,n)                        (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000080)
#define BIMC_S_SWAY_CONFIGURATION_INFO_6_RMSK                                    0x1
#define BIMC_S_SWAY_CONFIGURATION_INFO_6_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_6_ADDR(b,n), BIMC_S_SWAY_CONFIGURATION_INFO_6_RMSK)
#define BIMC_S_SWAY_CONFIGURATION_INFO_6_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_CONFIGURATION_INFO_6_ADDR(b,n), m)
#define BIMC_S_SWAY_CONFIGURATION_INFO_6_S2SW_PIPELINE_EN_BMSK                   0x1
#define BIMC_S_SWAY_CONFIGURATION_INFO_6_S2SW_PIPELINE_EN_SHFT                   0x0

#define BIMC_S_SWAY_INTERRUPT_STATUS_ADDR(b,n)                            (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000100)
#define BIMC_S_SWAY_INTERRUPT_STATUS_RMSK                                        0x3
#define BIMC_S_SWAY_INTERRUPT_STATUS_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_INTERRUPT_STATUS_ADDR(b,n), BIMC_S_SWAY_INTERRUPT_STATUS_RMSK)
#define BIMC_S_SWAY_INTERRUPT_STATUS_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_INTERRUPT_STATUS_ADDR(b,n), m)
#define BIMC_S_SWAY_INTERRUPT_STATUS_RFU_BMSK                                    0x3
#define BIMC_S_SWAY_INTERRUPT_STATUS_RFU_SHFT                                    0x0

#define BIMC_S_SWAY_INTERRUPT_CLEAR_ADDR(b,n)                             (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000108)
#define BIMC_S_SWAY_INTERRUPT_CLEAR_RMSK                                         0x3
#define BIMC_S_SWAY_INTERRUPT_CLEAR_OUTI(b,n,v)  \
        out_dword(BIMC_S_SWAY_INTERRUPT_CLEAR_ADDR(b,n),v)
#define BIMC_S_SWAY_INTERRUPT_CLEAR_RFU_BMSK                                     0x3
#define BIMC_S_SWAY_INTERRUPT_CLEAR_RFU_SHFT                                     0x0

#define BIMC_S_SWAY_INTERRUPT_ENABLE_ADDR(b,n)                            (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x0000010c)
#define BIMC_S_SWAY_INTERRUPT_ENABLE_RMSK                                        0x3
#define BIMC_S_SWAY_INTERRUPT_ENABLE_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_INTERRUPT_ENABLE_ADDR(b,n), BIMC_S_SWAY_INTERRUPT_ENABLE_RMSK)
#define BIMC_S_SWAY_INTERRUPT_ENABLE_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_INTERRUPT_ENABLE_ADDR(b,n), m)
#define BIMC_S_SWAY_INTERRUPT_ENABLE_OUTI(b,n,v)  \
        out_dword(BIMC_S_SWAY_INTERRUPT_ENABLE_ADDR(b,n),v)
#define BIMC_S_SWAY_INTERRUPT_ENABLE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SWAY_INTERRUPT_ENABLE_ADDR(b,n),m,v,BIMC_S_SWAY_INTERRUPT_ENABLE_INI(b,n))
#define BIMC_S_SWAY_INTERRUPT_ENABLE_RFU_BMSK                                    0x3
#define BIMC_S_SWAY_INTERRUPT_ENABLE_RFU_SHFT                                    0x0

#define BIMC_S_SWAY_CLOCK_CTRL_ADDR(b,n)                                  (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000200)
#define BIMC_S_SWAY_CLOCK_CTRL_RMSK                                              0x3
#define BIMC_S_SWAY_CLOCK_CTRL_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_CLOCK_CTRL_ADDR(b,n), BIMC_S_SWAY_CLOCK_CTRL_RMSK)
#define BIMC_S_SWAY_CLOCK_CTRL_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_CLOCK_CTRL_ADDR(b,n), m)
#define BIMC_S_SWAY_CLOCK_CTRL_OUTI(b,n,v)  \
        out_dword(BIMC_S_SWAY_CLOCK_CTRL_ADDR(b,n),v)
#define BIMC_S_SWAY_CLOCK_CTRL_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SWAY_CLOCK_CTRL_ADDR(b,n),m,v,BIMC_S_SWAY_CLOCK_CTRL_INI(b,n))
#define BIMC_S_SWAY_CLOCK_CTRL_SLAVE_CLOCK_GATING_EN_BMSK                        0x2
#define BIMC_S_SWAY_CLOCK_CTRL_SLAVE_CLOCK_GATING_EN_SHFT                        0x1
#define BIMC_S_SWAY_CLOCK_CTRL_CORE_CLOCK_GATING_EN_BMSK                         0x1
#define BIMC_S_SWAY_CLOCK_CTRL_CORE_CLOCK_GATING_EN_SHFT                         0x0

#define BIMC_S_SWAY_RCH_SELECT_ADDR(b,n)                                  (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000210)
#define BIMC_S_SWAY_RCH_SELECT_RMSK                                             0x7f
#define BIMC_S_SWAY_RCH_SELECT_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_RCH_SELECT_ADDR(b,n), BIMC_S_SWAY_RCH_SELECT_RMSK)
#define BIMC_S_SWAY_RCH_SELECT_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_RCH_SELECT_ADDR(b,n), m)
#define BIMC_S_SWAY_RCH_SELECT_UNUSED_BMSK                                      0x7f
#define BIMC_S_SWAY_RCH_SELECT_UNUSED_SHFT                                       0x0

#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_ADDR(b,n)                    (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000220)
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_RMSK                             0xffff
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_ADDR(b,n), BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_RMSK)
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_ADDR(b,n), m)
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_OUTI(b,n,v)  \
        out_dword(BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_ADDR(b,n),v)
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_ADDR(b,n),m,v,BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_IN(b,n))
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_WRITE_BMSK                       0xff00
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_WRITE_SHFT                          0x8
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_READ_BMSK                          0xff
#define BIMC_S_SWAY_MAX_OUTSTANDING_REQUESTS_READ_SHFT                           0x0

#define BIMC_S_SWAY_GATHERING_ADDR(b,n)                            (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000230)
#define BIMC_S_SWAY_GATHERING_RMSK                                        0xf
#define BIMC_S_SWAY_GATHERING_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_GATHERING_ADDR(b,n), BIMC_S_SWAY_GATHERING_RMSK)
#define BIMC_S_SWAY_GATHERING_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_GATHERING_ADDR(b,n), m)
#define BIMC_S_SWAY_GATHERING_OUTI(b,n,v)  \
        out_dword(BIMC_S_SWAY_GATHERING_ADDR(b,n),v)
#define BIMC_S_SWAY_GATHERING_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SWAY_GATHERING_ADDR(b,n),m,v,BIMC_S_SWAY_GATHERING_INI(b,n))
#define BIMC_S_SWAY_GATHERING_RD_GATHER_BEATS_BMSK                                    0xf
#define BIMC_S_SWAY_GATHERING_RD_GATHER_BEATS_SHFT                                    0x0

#define BIMC_S_SWAY_MODE_ADDR(b,n)                                  (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000240)
#define BIMC_S_SWAY_MODE_RMSK                                                         0x31
#define BIMC_S_SWAY_MODE_INI(b,n)      \
        in_dword_masked(BIMC_S_SWAY_MODE_ADDR(b,n), BIMC_S_SWAY_MODE_RMSK)
#define BIMC_S_SWAY_MODE_INMI(b,n,m)      \
        in_dword_masked(BIMC_S_SWAY_MODE_ADDR(b,n), m)
#define BIMC_S_SWAY_MODE_OUTI(b,n,v)      \
        out_dword(BIMC_S_SWAY_MODE_ADDR(b,n),v)
#define BIMC_S_SWAY_MODE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_SWAY_MODE_ADDR(b,n),m,v,BIMC_S_SWAY_MODE_INI(b,n))
#define BIMC_S_SWAY_MODE_CONVERT_16BYTE_EXCL_BMSK                                     0x20
#define BIMC_S_SWAY_MODE_CONVERT_16BYTE_EXCL_SHFT                                      0x5
#define BIMC_S_SWAY_MODE_CONVERT_ALL_EXCL_BMSK                                        0x10
#define BIMC_S_SWAY_MODE_CONVERT_ALL_EXCL_SHFT                                         0x4
#define BIMC_S_SWAY_MODE_BURST_SPLIT_BMSK                                              0x1
#define BIMC_S_SWAY_MODE_BURST_SPLIT_SHFT                                              0x0

#define BIMC_S_SWAY_BUFFER_STATUS_0_ADDR(b,n)                             (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000400)
#define BIMC_S_SWAY_BUFFER_STATUS_0_RMSK                                  0xff333333
#define BIMC_S_SWAY_BUFFER_STATUS_0_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_BUFFER_STATUS_0_ADDR(b,n), BIMC_S_SWAY_BUFFER_STATUS_0_RMSK)
#define BIMC_S_SWAY_BUFFER_STATUS_0_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_BUFFER_STATUS_0_ADDR(b,n), m)
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_DATA_RD_FULL_BMSK                0x80000000
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_DATA_RD_FULL_SHFT                      0x1f
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_DATA_RD_EMPTY_BMSK               0x40000000
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_DATA_RD_EMPTY_SHFT                     0x1e
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_CTRL_RD_FULL_BMSK                0x20000000
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_CTRL_RD_FULL_SHFT                      0x1d
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_CTRL_RD_EMPTY_BMSK               0x10000000
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_CTRL_RD_EMPTY_SHFT                     0x1c
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_DATA_WR_FULL_BMSK                 0x8000000
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_DATA_WR_FULL_SHFT                      0x1b
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_DATA_WR_EMPTY_BMSK                0x4000000
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_DATA_WR_EMPTY_SHFT                     0x1a
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_CTRL_WR_FULL_BMSK                 0x2000000
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_CTRL_WR_FULL_SHFT                      0x19
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_CTRL_WR_EMPTY_BMSK                0x1000000
#define BIMC_S_SWAY_BUFFER_STATUS_0_RCB0_CTRL_WR_EMPTY_SHFT                     0x18
#define BIMC_S_SWAY_BUFFER_STATUS_0_BCB_RD_FULL_BMSK                        0x200000
#define BIMC_S_SWAY_BUFFER_STATUS_0_BCB_RD_FULL_SHFT                            0x15
#define BIMC_S_SWAY_BUFFER_STATUS_0_BCB_RD_EMPTY_BMSK                       0x100000
#define BIMC_S_SWAY_BUFFER_STATUS_0_BCB_RD_EMPTY_SHFT                           0x14
#define BIMC_S_SWAY_BUFFER_STATUS_0_BCB_WR_FULL_BMSK                         0x20000
#define BIMC_S_SWAY_BUFFER_STATUS_0_BCB_WR_FULL_SHFT                            0x11
#define BIMC_S_SWAY_BUFFER_STATUS_0_BCB_WR_EMPTY_BMSK                        0x10000
#define BIMC_S_SWAY_BUFFER_STATUS_0_BCB_WR_EMPTY_SHFT                           0x10
#define BIMC_S_SWAY_BUFFER_STATUS_0_WCB_RD_FULL_BMSK                          0x2000
#define BIMC_S_SWAY_BUFFER_STATUS_0_WCB_RD_FULL_SHFT                             0xd
#define BIMC_S_SWAY_BUFFER_STATUS_0_WCB_RD_EMPTY_BMSK                         0x1000
#define BIMC_S_SWAY_BUFFER_STATUS_0_WCB_RD_EMPTY_SHFT                            0xc
#define BIMC_S_SWAY_BUFFER_STATUS_0_WCB_WR_FULL_BMSK                           0x200
#define BIMC_S_SWAY_BUFFER_STATUS_0_WCB_WR_FULL_SHFT                             0x9
#define BIMC_S_SWAY_BUFFER_STATUS_0_WCB_WR_EMPTY_BMSK                          0x100
#define BIMC_S_SWAY_BUFFER_STATUS_0_WCB_WR_EMPTY_SHFT                            0x8
#define BIMC_S_SWAY_BUFFER_STATUS_0_ACB_RD_FULL_BMSK                            0x20
#define BIMC_S_SWAY_BUFFER_STATUS_0_ACB_RD_FULL_SHFT                             0x5
#define BIMC_S_SWAY_BUFFER_STATUS_0_ACB_RD_EMPTY_BMSK                           0x10
#define BIMC_S_SWAY_BUFFER_STATUS_0_ACB_RD_EMPTY_SHFT                            0x4
#define BIMC_S_SWAY_BUFFER_STATUS_0_ACB_WR_FULL_BMSK                             0x2
#define BIMC_S_SWAY_BUFFER_STATUS_0_ACB_WR_FULL_SHFT                             0x1
#define BIMC_S_SWAY_BUFFER_STATUS_0_ACB_WR_EMPTY_BMSK                            0x1
#define BIMC_S_SWAY_BUFFER_STATUS_0_ACB_WR_EMPTY_SHFT                            0x0

#define BIMC_S_SWAY_BUFFER_STATUS_1_ADDR(b,n)                             (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000410)
#define BIMC_S_SWAY_BUFFER_STATUS_1_RMSK                                    0x330000
#define BIMC_S_SWAY_BUFFER_STATUS_1_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_BUFFER_STATUS_1_ADDR(b,n), BIMC_S_SWAY_BUFFER_STATUS_1_RMSK)
#define BIMC_S_SWAY_BUFFER_STATUS_1_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_BUFFER_STATUS_1_ADDR(b,n), m)
#define BIMC_S_SWAY_BUFFER_STATUS_1_WIB_RD_FULL_BMSK                        0x200000
#define BIMC_S_SWAY_BUFFER_STATUS_1_WIB_RD_FULL_SHFT                            0x15
#define BIMC_S_SWAY_BUFFER_STATUS_1_WIB_RD_EMPTY_BMSK                       0x100000
#define BIMC_S_SWAY_BUFFER_STATUS_1_WIB_RD_EMPTY_SHFT                           0x14
#define BIMC_S_SWAY_BUFFER_STATUS_1_WIB_WR_FULL_BMSK                         0x20000
#define BIMC_S_SWAY_BUFFER_STATUS_1_WIB_WR_FULL_SHFT                            0x11
#define BIMC_S_SWAY_BUFFER_STATUS_1_WIB_WR_EMPTY_BMSK                        0x10000
#define BIMC_S_SWAY_BUFFER_STATUS_1_WIB_WR_EMPTY_SHFT                           0x10

#define BIMC_S_SWAY_BUFFER_STATUS_2_ADDR(b,n)                             (BIMC_S_SWAY_REG_BASE(b) + (0x8000 * (n)) + 0x00000420)
#define BIMC_S_SWAY_BUFFER_STATUS_2_RMSK                                 0x330000ff
#define BIMC_S_SWAY_BUFFER_STATUS_2_INI(b,n)     \
        in_dword_masked(BIMC_S_SWAY_BUFFER_STATUS_2_ADDR(b,n), BIMC_S_SWAY_BUFFER_STATUS_2_RMSK)
#define BIMC_S_SWAY_BUFFER_STATUS_2_INMI(b,n,m)  \
        in_dword_masked(BIMC_S_SWAY_BUFFER_STATUS_2_ADDR(b,n), m)
#define BIMC_S_SWAY_BUFFER_STATUS_2_QCB_RD_FULL_BMSK                      0x20000000
#define BIMC_S_SWAY_BUFFER_STATUS_2_QCB_RD_FULL_SHFT                            0x1d
#define BIMC_S_SWAY_BUFFER_STATUS_2_QCB_RD_EMPTY_BMSK                     0x10000000
#define BIMC_S_SWAY_BUFFER_STATUS_2_QCB_RD_EMPTY_SHFT                           0x1c
#define BIMC_S_SWAY_BUFFER_STATUS_2_QCB_WR_FULL_BMSK                       0x2000000
#define BIMC_S_SWAY_BUFFER_STATUS_2_QCB_WR_FULL_SHFT                            0x19
#define BIMC_S_SWAY_BUFFER_STATUS_2_QCB_WR_EMPTY_BMSK                      0x1000000
#define BIMC_S_SWAY_BUFFER_STATUS_2_QCB_WR_EMPTY_SHFT                           0x18
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_DATA_RD_FULL_BMSK                      0x80
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_DATA_RD_FULL_SHFT                       0x7
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_DATA_RD_EMPTY_BMSK                     0x40
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_DATA_RD_EMPTY_SHFT                      0x6
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_CTRL_RD_FULL_BMSK                      0x20
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_CTRL_RD_FULL_SHFT                       0x5
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_CTRL_RD_EMPTY_BMSK                     0x10
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_CTRL_RD_EMPTY_SHFT                      0x4
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_DATA_WR_FULL_BMSK                       0x8
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_DATA_WR_FULL_SHFT                       0x3
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_DATA_WR_EMPTY_BMSK                      0x4
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_DATA_WR_EMPTY_SHFT                      0x2
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_CTRL_WR_FULL_BMSK                       0x2
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_CTRL_WR_FULL_SHFT                       0x1
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_CTRL_WR_EMPTY_BMSK                      0x1
#define BIMC_S_SWAY_BUFFER_STATUS_2_RCB1_CTRL_WR_EMPTY_SHFT                      0x0

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_ARB_GENERIC
 *--------------------------------------------------------------------------*/
#define BIMC_S_ARB_REG_BASE(b)                                                  ((b)      + 0x00049000)

#define BIMC_S_ARB_COMPONENT_INFO_ADDR(b,n)                                (BIMC_S_ARB_REG_BASE(b) + (0x8000 * (n)) + 0x00000000)
#define BIMC_S_ARB_COMPONENT_INFO_RMSK                                     0xffffff
#define BIMC_S_ARB_COMPONENT_INFO_INI(b,n)    \
        in_dword_masked(BIMC_S_ARB_COMPONENT_INFO_ADDR(b,n), BIMC_S_ARB_COMPONENT_INFO_RMSK)
#define BIMC_S_ARB_COMPONENT_INFO_INMI(b,n,m) \
        in_dword_masked(BIMC_S_ARB_COMPONENT_INFO_ADDR(b,n), m)
#define BIMC_S_ARB_COMPONENT_INFO_INSTANCE_BMSK                            0xff0000
#define BIMC_S_ARB_COMPONENT_INFO_INSTANCE_SHFT                                0x10
#define BIMC_S_ARB_COMPONENT_INFO_SUB_TYPE_BMSK                              0xff00
#define BIMC_S_ARB_COMPONENT_INFO_SUB_TYPE_SHFT                                 0x8
#define BIMC_S_ARB_COMPONENT_INFO_TYPE_BMSK                                    0xff
#define BIMC_S_ARB_COMPONENT_INFO_TYPE_SHFT                                     0x0

#define BIMC_S_ARB_CONFIGURATION_INFO_0_ADDR(b,n)                          (BIMC_S_ARB_REG_BASE(b) + (0x8000 * (n)) + 0x00000020)
#define BIMC_S_ARB_CONFIGURATION_INFO_0_RMSK                             0x8000ffff
#define BIMC_S_ARB_CONFIGURATION_INFO_0_INI(b,n)    \
        in_dword_masked(BIMC_S_ARB_CONFIGURATION_INFO_0_ADDR(b,n), BIMC_S_ARB_CONFIGURATION_INFO_0_RMSK)
#define BIMC_S_ARB_CONFIGURATION_INFO_0_INMI(b,n,m) \
        in_dword_masked(BIMC_S_ARB_CONFIGURATION_INFO_0_ADDR(b,n), m)
#define BIMC_S_ARB_CONFIGURATION_INFO_0_ARB2SW_PIPELINE_EN_BMSK          0x80000000
#define BIMC_S_ARB_CONFIGURATION_INFO_0_ARB2SW_PIPELINE_EN_SHFT                0x1f
#define BIMC_S_ARB_CONFIGURATION_INFO_0_WGB_DEPTH_BMSK                       0xff00
#define BIMC_S_ARB_CONFIGURATION_INFO_0_WGB_DEPTH_SHFT                          0x8
#define BIMC_S_ARB_CONFIGURATION_INFO_0_FUNCTIONALITY_BMSK                     0xff
#define BIMC_S_ARB_CONFIGURATION_INFO_0_FUNCTIONALITY_SHFT                      0x0

#define BIMC_S_ARB_CONFIGURATION_INFO_1_ADDR(b,n)                          (BIMC_S_ARB_REG_BASE(b) + (0x8000 * (n)) + 0x00000030)
#define BIMC_S_ARB_CONFIGURATION_INFO_1_RMSK                             0xffffffff
#define BIMC_S_ARB_CONFIGURATION_INFO_1_INI(b,n)    \
        in_dword_masked(BIMC_S_ARB_CONFIGURATION_INFO_1_ADDR(b,n), BIMC_S_ARB_CONFIGURATION_INFO_1_RMSK)
#define BIMC_S_ARB_CONFIGURATION_INFO_1_INMI(b,n,m) \
        in_dword_masked(BIMC_S_ARB_CONFIGURATION_INFO_1_ADDR(b,n), m)
#define BIMC_S_ARB_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_BMSK          0xffffffff
#define BIMC_S_ARB_CONFIGURATION_INFO_1_MPORT_CONNECTIVITY_SHFT                 0x0

#define BIMC_S_ARB_CLOCK_CTRL_ADDR(b,n)                                    (BIMC_S_ARB_REG_BASE(b) + (0x8000 * (n)) + 0x00000200)
#define BIMC_S_ARB_CLOCK_CTRL_RMSK                                              0x1
#define BIMC_S_ARB_CLOCK_CTRL_INI(b,n)    \
        in_dword_masked(BIMC_S_ARB_CLOCK_CTRL_ADDR(b,n), BIMC_S_ARB_CLOCK_CTRL_RMSK)
#define BIMC_S_ARB_CLOCK_CTRL_INMI(b,n,m) \
        in_dword_masked(BIMC_S_ARB_CLOCK_CTRL_ADDR(b,n), m)
#define BIMC_S_ARB_CLOCK_CTRL_OUTI(b,n,v)    \
        out_dword(BIMC_S_ARB_CLOCK_CTRL_ADDR(b,n),v)
#define BIMC_S_ARB_CLOCK_CTRL_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_ARB_CLOCK_CTRL_ADDR(b,n),m,v,BIMC_S_ARB_CLOCK_CTRL_INI(b,n))
/* Slaveway clock gating */
#define BIMC_S_ARB_CLOCK_CTRL_SLAVE_CLOCK_GATING_EN_BMSK                        0x2
#define BIMC_S_ARB_CLOCK_CTRL_SLAVE_CLOCK_GATING_EN_SHFT                        0x1
#define BIMC_S_ARB_CLOCK_CTRL_CORE_CLOCK_GATING_EN_BMSK                         0x1
#define BIMC_S_ARB_CLOCK_CTRL_CORE_CLOCK_GATING_EN_SHFT                         0x0
/* ARB port gating. */
#define BIMC_S_ARB_CLOCK_CTRL_CLOCK_GATING_EN_BMSK                              0x1
#define BIMC_S_ARB_CLOCK_CTRL_CLOCK_GATING_EN_SHFT                              0x0

#define BIMC_S_ARB_MODE_ADDR(b,n)                                          (BIMC_S_ARB_REG_BASE(b) + (0x8000 * (n)) + 0x00000210)
#define BIMC_S_ARB_MODE_RMSK                                             0xf0000001
#define BIMC_S_ARB_MODE_INI(b,n)    \
        in_dword_masked(BIMC_S_ARB_MODE_ADDR(b,n), BIMC_S_ARB_MODE_RMSK)
#define BIMC_S_ARB_MODE_INMI(b,n,m) \
        in_dword_masked(BIMC_S_ARB_MODE_ADDR(b,n), m)
#define BIMC_S_ARB_MODE_OUTI(b,n,v)    \
        out_dword(BIMC_S_ARB_MODE_ADDR(b,n),v)
#define BIMC_S_ARB_MODE_OUTMI(b,n,m,v) \
        out_dword_masked_ns(BIMC_S_ARB_MODE_ADDR(b,n),m,v,BIMC_S_ARB_MODE_INI(b,n))
#define BIMC_S_ARB_MODE_WR_GRANTS_AHEAD_BMSK                             0xf0000000
#define BIMC_S_ARB_MODE_WR_GRANTS_AHEAD_SHFT                                   0x1c
#define BIMC_S_ARB_MODE_PRIORITY_RR_EN_BMSK                                     0x1
#define BIMC_S_ARB_MODE_PRIORITY_RR_EN_SHFT                                     0x0

#endif /* __HALHWIOBIMC_H__ */
