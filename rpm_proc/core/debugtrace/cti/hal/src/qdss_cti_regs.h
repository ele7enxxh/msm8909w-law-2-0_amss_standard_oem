#ifndef __QDSS_CTI_REGS_H__
#define __QDSS_CTI_REGS_H__
/*
===========================================================================
*/
/**
  @file qdss_cti_regs.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    QDSS_CTI_CSCTI

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/cti/hal/src/qdss_cti_regs.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: QDSS_CTI_CSCTI
 *--------------------------------------------------------------------------*/

extern volatile uint32 QDSS_CTI_CSCTI_REG_BASE;

#define HWIO_QDSS_CTICONTROL_ADDR                              (QDSS_CTI_CSCTI_REG_BASE      + 0x00000000)
#define HWIO_QDSS_CTICONTROL_RMSK                                     0x1
#define HWIO_QDSS_CTICONTROL_IN          \
        in_dword_masked(HWIO_QDSS_CTICONTROL_ADDR, HWIO_QDSS_CTICONTROL_RMSK)
#define HWIO_QDSS_CTICONTROL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTICONTROL_ADDR, m)
#define HWIO_QDSS_CTICONTROL_OUT(v)      \
        out_dword(HWIO_QDSS_CTICONTROL_ADDR,v)
#define HWIO_QDSS_CTICONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTICONTROL_ADDR,m,v,HWIO_QDSS_CTICONTROL_IN)
#define HWIO_QDSS_CTICONTROL_GLBEN_BMSK                               0x1
#define HWIO_QDSS_CTICONTROL_GLBEN_SHFT                               0x0

#define HWIO_QDSS_CTIINTACK_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000010)
#define HWIO_QDSS_CTIINTACK_RMSK                                     0xff
#define HWIO_QDSS_CTIINTACK_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINTACK_ADDR,v)
#define HWIO_QDSS_CTIINTACK_INTACK_BMSK                              0xff
#define HWIO_QDSS_CTIINTACK_INTACK_SHFT                               0x0

#define HWIO_QDSS_CTIAPPSET_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000014)
#define HWIO_QDSS_CTIAPPSET_RMSK                                     0xff
#define HWIO_QDSS_CTIAPPSET_OUT(v)      \
        out_dword(HWIO_QDSS_CTIAPPSET_ADDR,v)
#define HWIO_QDSS_CTIAPPSET_APPSET_BMSK                              0xff
#define HWIO_QDSS_CTIAPPSET_APPSET_SHFT                               0x0

#define HWIO_QDSS_CTIAPPCLEAR_ADDR                             (QDSS_CTI_CSCTI_REG_BASE      + 0x00000018)
#define HWIO_QDSS_CTIAPPCLEAR_RMSK                                    0xf
#define HWIO_QDSS_CTIAPPCLEAR_OUT(v)      \
        out_dword(HWIO_QDSS_CTIAPPCLEAR_ADDR,v)
#define HWIO_QDSS_CTIAPPCLEAR_APPCLEAR_BMSK                           0xf
#define HWIO_QDSS_CTIAPPCLEAR_APPCLEAR_SHFT                           0x0

#define HWIO_QDSS_CTIAPPPULSE_ADDR                             (QDSS_CTI_CSCTI_REG_BASE      + 0x0000001c)
#define HWIO_QDSS_CTIAPPPULSE_RMSK                                   0xff
#define HWIO_QDSS_CTIAPPPULSE_OUT(v)      \
        out_dword(HWIO_QDSS_CTIAPPPULSE_ADDR,v)
#define HWIO_QDSS_CTIAPPPULSE_APPULSE_BMSK                           0xff
#define HWIO_QDSS_CTIAPPPULSE_APPULSE_SHFT                            0x0

#define HWIO_QDSS_CTIINEN0_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000020)
#define HWIO_QDSS_CTIINEN0_RMSK                                       0xf
#define HWIO_QDSS_CTIINEN0_IN          \
        in_dword_masked(HWIO_QDSS_CTIINEN0_ADDR, HWIO_QDSS_CTIINEN0_RMSK)
#define HWIO_QDSS_CTIINEN0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIINEN0_ADDR, m)
#define HWIO_QDSS_CTIINEN0_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINEN0_ADDR,v)
#define HWIO_QDSS_CTIINEN0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIINEN0_ADDR,m,v,HWIO_QDSS_CTIINEN0_IN)
#define HWIO_QDSS_CTIINEN0_TRIGINEN_BMSK                              0xf
#define HWIO_QDSS_CTIINEN0_TRIGINEN_SHFT                              0x0

#define HWIO_QDSS_CTIINEN1_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000024)
#define HWIO_QDSS_CTIINEN1_RMSK                                       0xf
#define HWIO_QDSS_CTIINEN1_IN          \
        in_dword_masked(HWIO_QDSS_CTIINEN1_ADDR, HWIO_QDSS_CTIINEN1_RMSK)
#define HWIO_QDSS_CTIINEN1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIINEN1_ADDR, m)
#define HWIO_QDSS_CTIINEN1_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINEN1_ADDR,v)
#define HWIO_QDSS_CTIINEN1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIINEN1_ADDR,m,v,HWIO_QDSS_CTIINEN1_IN)
#define HWIO_QDSS_CTIINEN1_TRIGINEN_BMSK                              0xf
#define HWIO_QDSS_CTIINEN1_TRIGINEN_SHFT                              0x0

#define HWIO_QDSS_CTIINEN2_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000028)
#define HWIO_QDSS_CTIINEN2_RMSK                                       0xf
#define HWIO_QDSS_CTIINEN2_IN          \
        in_dword_masked(HWIO_QDSS_CTIINEN2_ADDR, HWIO_QDSS_CTIINEN2_RMSK)
#define HWIO_QDSS_CTIINEN2_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIINEN2_ADDR, m)
#define HWIO_QDSS_CTIINEN2_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINEN2_ADDR,v)
#define HWIO_QDSS_CTIINEN2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIINEN2_ADDR,m,v,HWIO_QDSS_CTIINEN2_IN)
#define HWIO_QDSS_CTIINEN2_TRIGINEN_BMSK                              0xf
#define HWIO_QDSS_CTIINEN2_TRIGINEN_SHFT                              0x0

#define HWIO_QDSS_CTIINEN3_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x0000002c)
#define HWIO_QDSS_CTIINEN3_RMSK                                       0xf
#define HWIO_QDSS_CTIINEN3_IN          \
        in_dword_masked(HWIO_QDSS_CTIINEN3_ADDR, HWIO_QDSS_CTIINEN3_RMSK)
#define HWIO_QDSS_CTIINEN3_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIINEN3_ADDR, m)
#define HWIO_QDSS_CTIINEN3_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINEN3_ADDR,v)
#define HWIO_QDSS_CTIINEN3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIINEN3_ADDR,m,v,HWIO_QDSS_CTIINEN3_IN)
#define HWIO_QDSS_CTIINEN3_TRIGINEN_BMSK                              0xf
#define HWIO_QDSS_CTIINEN3_TRIGINEN_SHFT                              0x0

#define HWIO_QDSS_CTIINEN4_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000030)
#define HWIO_QDSS_CTIINEN4_RMSK                                       0xf
#define HWIO_QDSS_CTIINEN4_IN          \
        in_dword_masked(HWIO_QDSS_CTIINEN4_ADDR, HWIO_QDSS_CTIINEN4_RMSK)
#define HWIO_QDSS_CTIINEN4_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIINEN4_ADDR, m)
#define HWIO_QDSS_CTIINEN4_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINEN4_ADDR,v)
#define HWIO_QDSS_CTIINEN4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIINEN4_ADDR,m,v,HWIO_QDSS_CTIINEN4_IN)
#define HWIO_QDSS_CTIINEN4_TRIGINEN_BMSK                              0xf
#define HWIO_QDSS_CTIINEN4_TRIGINEN_SHFT                              0x0

#define HWIO_QDSS_CTIINEN5_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000034)
#define HWIO_QDSS_CTIINEN5_RMSK                                       0xf
#define HWIO_QDSS_CTIINEN5_IN          \
        in_dword_masked(HWIO_QDSS_CTIINEN5_ADDR, HWIO_QDSS_CTIINEN5_RMSK)
#define HWIO_QDSS_CTIINEN5_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIINEN5_ADDR, m)
#define HWIO_QDSS_CTIINEN5_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINEN5_ADDR,v)
#define HWIO_QDSS_CTIINEN5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIINEN5_ADDR,m,v,HWIO_QDSS_CTIINEN5_IN)
#define HWIO_QDSS_CTIINEN5_TRIGINEN_BMSK                              0xf
#define HWIO_QDSS_CTIINEN5_TRIGINEN_SHFT                              0x0

#define HWIO_QDSS_CTIINEN6_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000038)
#define HWIO_QDSS_CTIINEN6_RMSK                                       0xf
#define HWIO_QDSS_CTIINEN6_IN          \
        in_dword_masked(HWIO_QDSS_CTIINEN6_ADDR, HWIO_QDSS_CTIINEN6_RMSK)
#define HWIO_QDSS_CTIINEN6_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIINEN6_ADDR, m)
#define HWIO_QDSS_CTIINEN6_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINEN6_ADDR,v)
#define HWIO_QDSS_CTIINEN6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIINEN6_ADDR,m,v,HWIO_QDSS_CTIINEN6_IN)
#define HWIO_QDSS_CTIINEN6_TRIGINEN_BMSK                              0xf
#define HWIO_QDSS_CTIINEN6_TRIGINEN_SHFT                              0x0

#define HWIO_QDSS_CTIINEN7_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x0000003c)
#define HWIO_QDSS_CTIINEN7_RMSK                                       0xf
#define HWIO_QDSS_CTIINEN7_IN          \
        in_dword_masked(HWIO_QDSS_CTIINEN7_ADDR, HWIO_QDSS_CTIINEN7_RMSK)
#define HWIO_QDSS_CTIINEN7_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIINEN7_ADDR, m)
#define HWIO_QDSS_CTIINEN7_OUT(v)      \
        out_dword(HWIO_QDSS_CTIINEN7_ADDR,v)
#define HWIO_QDSS_CTIINEN7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIINEN7_ADDR,m,v,HWIO_QDSS_CTIINEN7_IN)
#define HWIO_QDSS_CTIINEN7_TRIGINEN_BMSK                              0xf
#define HWIO_QDSS_CTIINEN7_TRIGINEN_SHFT                              0x0

#define HWIO_QDSS_CTIOUTEN0_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x000000a0)
#define HWIO_QDSS_CTIOUTEN0_RMSK                                      0xf
#define HWIO_QDSS_CTIOUTEN0_IN          \
        in_dword_masked(HWIO_QDSS_CTIOUTEN0_ADDR, HWIO_QDSS_CTIOUTEN0_RMSK)
#define HWIO_QDSS_CTIOUTEN0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIOUTEN0_ADDR, m)
#define HWIO_QDSS_CTIOUTEN0_OUT(v)      \
        out_dword(HWIO_QDSS_CTIOUTEN0_ADDR,v)
#define HWIO_QDSS_CTIOUTEN0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIOUTEN0_ADDR,m,v,HWIO_QDSS_CTIOUTEN0_IN)
#define HWIO_QDSS_CTIOUTEN0_TRIGOUTEN_BMSK                            0xf
#define HWIO_QDSS_CTIOUTEN0_TRIGOUTEN_SHFT                            0x0

#define HWIO_QDSS_CTIOUTEN1_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x000000a4)
#define HWIO_QDSS_CTIOUTEN1_RMSK                                      0xf
#define HWIO_QDSS_CTIOUTEN1_IN          \
        in_dword_masked(HWIO_QDSS_CTIOUTEN1_ADDR, HWIO_QDSS_CTIOUTEN1_RMSK)
#define HWIO_QDSS_CTIOUTEN1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIOUTEN1_ADDR, m)
#define HWIO_QDSS_CTIOUTEN1_OUT(v)      \
        out_dword(HWIO_QDSS_CTIOUTEN1_ADDR,v)
#define HWIO_QDSS_CTIOUTEN1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIOUTEN1_ADDR,m,v,HWIO_QDSS_CTIOUTEN1_IN)
#define HWIO_QDSS_CTIOUTEN1_TRIGOUTEN_BMSK                            0xf
#define HWIO_QDSS_CTIOUTEN1_TRIGOUTEN_SHFT                            0x0

#define HWIO_QDSS_CTIOUTEN2_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x000000a8)
#define HWIO_QDSS_CTIOUTEN2_RMSK                                      0xf
#define HWIO_QDSS_CTIOUTEN2_IN          \
        in_dword_masked(HWIO_QDSS_CTIOUTEN2_ADDR, HWIO_QDSS_CTIOUTEN2_RMSK)
#define HWIO_QDSS_CTIOUTEN2_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIOUTEN2_ADDR, m)
#define HWIO_QDSS_CTIOUTEN2_OUT(v)      \
        out_dword(HWIO_QDSS_CTIOUTEN2_ADDR,v)
#define HWIO_QDSS_CTIOUTEN2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIOUTEN2_ADDR,m,v,HWIO_QDSS_CTIOUTEN2_IN)
#define HWIO_QDSS_CTIOUTEN2_TRIGOUTEN_BMSK                            0xf
#define HWIO_QDSS_CTIOUTEN2_TRIGOUTEN_SHFT                            0x0

#define HWIO_QDSS_CTIOUTEN3_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x000000ac)
#define HWIO_QDSS_CTIOUTEN3_RMSK                                      0xf
#define HWIO_QDSS_CTIOUTEN3_IN          \
        in_dword_masked(HWIO_QDSS_CTIOUTEN3_ADDR, HWIO_QDSS_CTIOUTEN3_RMSK)
#define HWIO_QDSS_CTIOUTEN3_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIOUTEN3_ADDR, m)
#define HWIO_QDSS_CTIOUTEN3_OUT(v)      \
        out_dword(HWIO_QDSS_CTIOUTEN3_ADDR,v)
#define HWIO_QDSS_CTIOUTEN3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIOUTEN3_ADDR,m,v,HWIO_QDSS_CTIOUTEN3_IN)
#define HWIO_QDSS_CTIOUTEN3_TRIGOUTEN_BMSK                            0xf
#define HWIO_QDSS_CTIOUTEN3_TRIGOUTEN_SHFT                            0x0

#define HWIO_QDSS_CTIOUTEN4_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x000000b0)
#define HWIO_QDSS_CTIOUTEN4_RMSK                                      0xf
#define HWIO_QDSS_CTIOUTEN4_IN          \
        in_dword_masked(HWIO_QDSS_CTIOUTEN4_ADDR, HWIO_QDSS_CTIOUTEN4_RMSK)
#define HWIO_QDSS_CTIOUTEN4_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIOUTEN4_ADDR, m)
#define HWIO_QDSS_CTIOUTEN4_OUT(v)      \
        out_dword(HWIO_QDSS_CTIOUTEN4_ADDR,v)
#define HWIO_QDSS_CTIOUTEN4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIOUTEN4_ADDR,m,v,HWIO_QDSS_CTIOUTEN4_IN)
#define HWIO_QDSS_CTIOUTEN4_TRIGOUTEN_BMSK                            0xf
#define HWIO_QDSS_CTIOUTEN4_TRIGOUTEN_SHFT                            0x0

#define HWIO_QDSS_CTIOUTEN5_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x000000b4)
#define HWIO_QDSS_CTIOUTEN5_RMSK                                      0xf
#define HWIO_QDSS_CTIOUTEN5_IN          \
        in_dword_masked(HWIO_QDSS_CTIOUTEN5_ADDR, HWIO_QDSS_CTIOUTEN5_RMSK)
#define HWIO_QDSS_CTIOUTEN5_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIOUTEN5_ADDR, m)
#define HWIO_QDSS_CTIOUTEN5_OUT(v)      \
        out_dword(HWIO_QDSS_CTIOUTEN5_ADDR,v)
#define HWIO_QDSS_CTIOUTEN5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIOUTEN5_ADDR,m,v,HWIO_QDSS_CTIOUTEN5_IN)
#define HWIO_QDSS_CTIOUTEN5_TRIGOUTEN_BMSK                            0xf
#define HWIO_QDSS_CTIOUTEN5_TRIGOUTEN_SHFT                            0x0

#define HWIO_QDSS_CTIOUTEN6_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x000000b8)
#define HWIO_QDSS_CTIOUTEN6_RMSK                                      0xf
#define HWIO_QDSS_CTIOUTEN6_IN          \
        in_dword_masked(HWIO_QDSS_CTIOUTEN6_ADDR, HWIO_QDSS_CTIOUTEN6_RMSK)
#define HWIO_QDSS_CTIOUTEN6_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIOUTEN6_ADDR, m)
#define HWIO_QDSS_CTIOUTEN6_OUT(v)      \
        out_dword(HWIO_QDSS_CTIOUTEN6_ADDR,v)
#define HWIO_QDSS_CTIOUTEN6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIOUTEN6_ADDR,m,v,HWIO_QDSS_CTIOUTEN6_IN)
#define HWIO_QDSS_CTIOUTEN6_TRIGOUTEN_BMSK                            0xf
#define HWIO_QDSS_CTIOUTEN6_TRIGOUTEN_SHFT                            0x0

#define HWIO_QDSS_CTIOUTEN7_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x000000bc)
#define HWIO_QDSS_CTIOUTEN7_RMSK                                      0xf
#define HWIO_QDSS_CTIOUTEN7_IN          \
        in_dword_masked(HWIO_QDSS_CTIOUTEN7_ADDR, HWIO_QDSS_CTIOUTEN7_RMSK)
#define HWIO_QDSS_CTIOUTEN7_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIOUTEN7_ADDR, m)
#define HWIO_QDSS_CTIOUTEN7_OUT(v)      \
        out_dword(HWIO_QDSS_CTIOUTEN7_ADDR,v)
#define HWIO_QDSS_CTIOUTEN7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIOUTEN7_ADDR,m,v,HWIO_QDSS_CTIOUTEN7_IN)
#define HWIO_QDSS_CTIOUTEN7_TRIGOUTEN_BMSK                            0xf
#define HWIO_QDSS_CTIOUTEN7_TRIGOUTEN_SHFT                            0x0

#define HWIO_QDSS_CTITRIGINSTATUS_ADDR                         (QDSS_CTI_CSCTI_REG_BASE      + 0x00000130)
#define HWIO_QDSS_CTITRIGINSTATUS_RMSK                               0xff
#define HWIO_QDSS_CTITRIGINSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_CTITRIGINSTATUS_ADDR, HWIO_QDSS_CTITRIGINSTATUS_RMSK)
#define HWIO_QDSS_CTITRIGINSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTITRIGINSTATUS_ADDR, m)
#define HWIO_QDSS_CTITRIGINSTATUS_TRIGINSTATUS_BMSK                  0xff
#define HWIO_QDSS_CTITRIGINSTATUS_TRIGINSTATUS_SHFT                   0x0

#define HWIO_QDSS_CTITRIGOUTSTATUS_ADDR                        (QDSS_CTI_CSCTI_REG_BASE      + 0x00000134)
#define HWIO_QDSS_CTITRIGOUTSTATUS_RMSK                              0xff
#define HWIO_QDSS_CTITRIGOUTSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_CTITRIGOUTSTATUS_ADDR, HWIO_QDSS_CTITRIGOUTSTATUS_RMSK)
#define HWIO_QDSS_CTITRIGOUTSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTITRIGOUTSTATUS_ADDR, m)
#define HWIO_QDSS_CTITRIGOUTSTATUS_TRIGOUTSTATUS_BMSK                0xff
#define HWIO_QDSS_CTITRIGOUTSTATUS_TRIGOUTSTATUS_SHFT                 0x0

#define HWIO_QDSS_CTICHINSTATUS_ADDR                           (QDSS_CTI_CSCTI_REG_BASE      + 0x00000138)
#define HWIO_QDSS_CTICHINSTATUS_RMSK                                  0xf
#define HWIO_QDSS_CTICHINSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_CTICHINSTATUS_ADDR, HWIO_QDSS_CTICHINSTATUS_RMSK)
#define HWIO_QDSS_CTICHINSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTICHINSTATUS_ADDR, m)
#define HWIO_QDSS_CTICHINSTATUS_CTICHINSTATUS_BMSK                    0xf
#define HWIO_QDSS_CTICHINSTATUS_CTICHINSTATUS_SHFT                    0x0

#define HWIO_QDSS_CTICHOUTSTATUS_ADDR                          (QDSS_CTI_CSCTI_REG_BASE      + 0x0000013c)
#define HWIO_QDSS_CTICHOUTSTATUS_RMSK                                 0xf
#define HWIO_QDSS_CTICHOUTSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_CTICHOUTSTATUS_ADDR, HWIO_QDSS_CTICHOUTSTATUS_RMSK)
#define HWIO_QDSS_CTICHOUTSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTICHOUTSTATUS_ADDR, m)
#define HWIO_QDSS_CTICHOUTSTATUS_CTICHOUTSTATUS_BMSK                  0xf
#define HWIO_QDSS_CTICHOUTSTATUS_CTICHOUTSTATUS_SHFT                  0x0

#define HWIO_QDSS_CTIGATE_ADDR                                 (QDSS_CTI_CSCTI_REG_BASE      + 0x00000140)
#define HWIO_QDSS_CTIGATE_RMSK                                        0xf
#define HWIO_QDSS_CTIGATE_IN          \
        in_dword_masked(HWIO_QDSS_CTIGATE_ADDR, HWIO_QDSS_CTIGATE_RMSK)
#define HWIO_QDSS_CTIGATE_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTIGATE_ADDR, m)
#define HWIO_QDSS_CTIGATE_OUT(v)      \
        out_dword(HWIO_QDSS_CTIGATE_ADDR,v)
#define HWIO_QDSS_CTIGATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTIGATE_ADDR,m,v,HWIO_QDSS_CTIGATE_IN)
#define HWIO_QDSS_CTIGATE_CTIGATEEN3_BMSK                             0x8
#define HWIO_QDSS_CTIGATE_CTIGATEEN3_SHFT                             0x3
#define HWIO_QDSS_CTIGATE_CTIGATEEN2_BMSK                             0x4
#define HWIO_QDSS_CTIGATE_CTIGATEEN2_SHFT                             0x2
#define HWIO_QDSS_CTIGATE_CTIGATEEN1_BMSK                             0x2
#define HWIO_QDSS_CTIGATE_CTIGATEEN1_SHFT                             0x1
#define HWIO_QDSS_CTIGATE_CTIGATEEN0_BMSK                             0x1
#define HWIO_QDSS_CTIGATE_CTIGATEEN0_SHFT                             0x0

#define HWIO_QDSS_CTI_ASICCTL_ADDR                                 (QDSS_CTI_CSCTI_REG_BASE      + 0x00000144)
#define HWIO_QDSS_CTI_ASICCTL_RMSK                                       0xff
#define HWIO_QDSS_CTI_ASICCTL_IN          \
        in_dword_masked(HWIO_QDSS_CTI_ASICCTL_ADDR, HWIO_QDSS_CTI_ASICCTL_RMSK)
#define HWIO_QDSS_CTI_ASICCTL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_ASICCTL_ADDR, m)
#define HWIO_QDSS_CTI_ASICCTL_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_ASICCTL_ADDR,v)
#define HWIO_QDSS_CTI_ASICCTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTI_ASICCTL_ADDR,m,v,HWIO_QDSS_CTI_ASICCTL_IN)
#define HWIO_QDSS_CTI_ASICCTL_ASICCTL_BMSK                               0xff
#define HWIO_QDSS_CTI_ASICCTL_ASICCTL_SHFT                                0x0

#define HWIO_QDSS_CTI_ITCHINACK_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000edc)
#define HWIO_QDSS_CTI_ITCHINACK_RMSK                                      0xf
#define HWIO_QDSS_CTI_ITCHINACK_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_ITCHINACK_ADDR,v)
#define HWIO_QDSS_CTI_ITCHINACK_CTCHINACK_BMSK                            0xf
#define HWIO_QDSS_CTI_ITCHINACK_CTCHINACK_SHFT                            0x0

#define HWIO_QDSS_CTI_ITTRIGINACK_ADDR                             (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ee0)
#define HWIO_QDSS_CTI_ITTRIGINACK_RMSK                                   0xff
#define HWIO_QDSS_CTI_ITTRIGINACK_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_ITTRIGINACK_ADDR,v)
#define HWIO_QDSS_CTI_ITTRIGINACK_CTTRIGINACK_BMSK                       0xff
#define HWIO_QDSS_CTI_ITTRIGINACK_CTTRIGINACK_SHFT                        0x0

#define HWIO_QDSS_CTI_ITCHOUT_ADDR                                 (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ee4)
#define HWIO_QDSS_CTI_ITCHOUT_RMSK                                        0xf
#define HWIO_QDSS_CTI_ITCHOUT_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_ITCHOUT_ADDR,v)
#define HWIO_QDSS_CTI_ITCHOUT_CTCHOUT_BMSK                                0xf
#define HWIO_QDSS_CTI_ITCHOUT_CTCHOUT_SHFT                                0x0

#define HWIO_QDSS_CTI_ITTRIGOUT_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ee8)
#define HWIO_QDSS_CTI_ITTRIGOUT_RMSK                                     0xff
#define HWIO_QDSS_CTI_ITTRIGOUT_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_ITTRIGOUT_ADDR,v)
#define HWIO_QDSS_CTI_ITTRIGOUT_CTTRIGOUT_BMSK                           0xff
#define HWIO_QDSS_CTI_ITTRIGOUT_CTTRIGOUT_SHFT                            0x0

#define HWIO_QDSS_CTI_ITCHOUTACK_ADDR                              (QDSS_CTI_CSCTI_REG_BASE      + 0x00000eec)
#define HWIO_QDSS_CTI_ITCHOUTACK_RMSK                                     0xf
#define HWIO_QDSS_CTI_ITCHOUTACK_IN          \
        in_dword_masked(HWIO_QDSS_CTI_ITCHOUTACK_ADDR, HWIO_QDSS_CTI_ITCHOUTACK_RMSK)
#define HWIO_QDSS_CTI_ITCHOUTACK_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_ITCHOUTACK_ADDR, m)
#define HWIO_QDSS_CTI_ITCHOUTACK_CTCHOUTACK_BMSK                          0xf
#define HWIO_QDSS_CTI_ITCHOUTACK_CTCHOUTACK_SHFT                          0x0

#define HWIO_QDSS_CTI_ITTRIGOUTACK_ADDR                            (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ef0)
#define HWIO_QDSS_CTI_ITTRIGOUTACK_RMSK                                  0xff
#define HWIO_QDSS_CTI_ITTRIGOUTACK_IN          \
        in_dword_masked(HWIO_QDSS_CTI_ITTRIGOUTACK_ADDR, HWIO_QDSS_CTI_ITTRIGOUTACK_RMSK)
#define HWIO_QDSS_CTI_ITTRIGOUTACK_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_ITTRIGOUTACK_ADDR, m)
#define HWIO_QDSS_CTI_ITTRIGOUTACK_CTTRIGOUTACK_BMSK                     0xff
#define HWIO_QDSS_CTI_ITTRIGOUTACK_CTTRIGOUTACK_SHFT                      0x0

#define HWIO_QDSS_CTI_ITCHIN_ADDR                                  (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ef4)
#define HWIO_QDSS_CTI_ITCHIN_RMSK                                         0xf
#define HWIO_QDSS_CTI_ITCHIN_IN          \
        in_dword_masked(HWIO_QDSS_CTI_ITCHIN_ADDR, HWIO_QDSS_CTI_ITCHIN_RMSK)
#define HWIO_QDSS_CTI_ITCHIN_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_ITCHIN_ADDR, m)
#define HWIO_QDSS_CTI_ITCHIN_CTCHIN_BMSK                                  0xf
#define HWIO_QDSS_CTI_ITCHIN_CTCHIN_SHFT                                  0x0

#define HWIO_QDSS_CTI_ITTRIGIN_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ef8)
#define HWIO_QDSS_CTI_ITTRIGIN_RMSK                                      0xff
#define HWIO_QDSS_CTI_ITTRIGIN_IN          \
        in_dword_masked(HWIO_QDSS_CTI_ITTRIGIN_ADDR, HWIO_QDSS_CTI_ITTRIGIN_RMSK)
#define HWIO_QDSS_CTI_ITTRIGIN_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_ITTRIGIN_ADDR, m)
#define HWIO_QDSS_CTI_ITTRIGIN_CTTRIGIN_BMSK                             0xff
#define HWIO_QDSS_CTI_ITTRIGIN_CTTRIGIN_SHFT                              0x0

#define HWIO_QDSS_CTI_ITCTRL_ADDR                                  (QDSS_CTI_CSCTI_REG_BASE      + 0x00000f00)
#define HWIO_QDSS_CTI_ITCTRL_RMSK                                         0x1
#define HWIO_QDSS_CTI_ITCTRL_IN          \
        in_dword_masked(HWIO_QDSS_CTI_ITCTRL_ADDR, HWIO_QDSS_CTI_ITCTRL_RMSK)
#define HWIO_QDSS_CTI_ITCTRL_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_ITCTRL_ADDR, m)
#define HWIO_QDSS_CTI_ITCTRL_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_ITCTRL_ADDR,v)
#define HWIO_QDSS_CTI_ITCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTI_ITCTRL_ADDR,m,v,HWIO_QDSS_CTI_ITCTRL_IN)
#define HWIO_QDSS_CTI_ITCTRL_INTEGRATION_MODE_BMSK                        0x1
#define HWIO_QDSS_CTI_ITCTRL_INTEGRATION_MODE_SHFT                        0x0

#define HWIO_QDSS_CTI_CLAIMSET_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fa0)
#define HWIO_QDSS_CTI_CLAIMSET_RMSK                                       0xf
#define HWIO_QDSS_CTI_CLAIMSET_IN          \
        in_dword_masked(HWIO_QDSS_CTI_CLAIMSET_ADDR, HWIO_QDSS_CTI_CLAIMSET_RMSK)
#define HWIO_QDSS_CTI_CLAIMSET_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_CLAIMSET_ADDR, m)
#define HWIO_QDSS_CTI_CLAIMSET_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_CLAIMSET_ADDR,v)
#define HWIO_QDSS_CTI_CLAIMSET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTI_CLAIMSET_ADDR,m,v,HWIO_QDSS_CTI_CLAIMSET_IN)
#define HWIO_QDSS_CTI_CLAIMSET_CLAIMSET_W_BMSK                            0xf
#define HWIO_QDSS_CTI_CLAIMSET_CLAIMSET_W_SHFT                            0x0
#define HWIO_QDSS_CTI_CLAIMSET_CLAIMSET_R_BMSK                            0xf
#define HWIO_QDSS_CTI_CLAIMSET_CLAIMSET_R_SHFT                            0x0

#define HWIO_QDSS_CTI_CLAIMCLR_ADDR                                (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fa4)
#define HWIO_QDSS_CTI_CLAIMCLR_RMSK                                       0xf
#define HWIO_QDSS_CTI_CLAIMCLR_IN          \
        in_dword_masked(HWIO_QDSS_CTI_CLAIMCLR_ADDR, HWIO_QDSS_CTI_CLAIMCLR_RMSK)
#define HWIO_QDSS_CTI_CLAIMCLR_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_CLAIMCLR_ADDR, m)
#define HWIO_QDSS_CTI_CLAIMCLR_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_CLAIMCLR_ADDR,v)
#define HWIO_QDSS_CTI_CLAIMCLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTI_CLAIMCLR_ADDR,m,v,HWIO_QDSS_CTI_CLAIMCLR_IN)
#define HWIO_QDSS_CTI_CLAIMCLR_CLAIMCLR_W_BMSK                            0xf
#define HWIO_QDSS_CTI_CLAIMCLR_CLAIMCLR_W_SHFT                            0x0
#define HWIO_QDSS_CTI_CLAIMCLR_CLAIMCLR_R_BMSK                            0xf
#define HWIO_QDSS_CTI_CLAIMCLR_CLAIMCLR_R_SHFT                            0x0

#define HWIO_QDSS_CTI_LAR_ADDR                                     (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_CTI_LAR_RMSK                                     0xffffffff
#define HWIO_QDSS_CTI_LAR_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_LAR_ADDR,v)
#define HWIO_QDSS_CTI_LAR_ACCESS_W_BMSK                            0xffffffff
#define HWIO_QDSS_CTI_LAR_ACCESS_W_SHFT                                   0x0

#define HWIO_QDSS_CTI_LSR_ADDR                                     (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_CTI_LSR_RMSK                                            0x7
#define HWIO_QDSS_CTI_LSR_IN          \
        in_dword_masked(HWIO_QDSS_CTI_LSR_ADDR, HWIO_QDSS_CTI_LSR_RMSK)
#define HWIO_QDSS_CTI_LSR_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_LSR_ADDR, m)
#define HWIO_QDSS_CTI_LSR_LOCKTYPE_BMSK                                   0x4
#define HWIO_QDSS_CTI_LSR_LOCKTYPE_SHFT                                   0x2
#define HWIO_QDSS_CTI_LSR_LOCKGRANT_BMSK                                  0x2
#define HWIO_QDSS_CTI_LSR_LOCKGRANT_SHFT                                  0x1
#define HWIO_QDSS_CTI_LSR_LOCKEXIST_BMSK                                  0x1
#define HWIO_QDSS_CTI_LSR_LOCKEXIST_SHFT                                  0x0

#define HWIO_QDSS_CTI_AUTHSTATUS_ADDR                              (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fb8)
#define HWIO_QDSS_CTI_AUTHSTATUS_RMSK                                    0xff
#define HWIO_QDSS_CTI_AUTHSTATUS_IN          \
        in_dword_masked(HWIO_QDSS_CTI_AUTHSTATUS_ADDR, HWIO_QDSS_CTI_AUTHSTATUS_RMSK)
#define HWIO_QDSS_CTI_AUTHSTATUS_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_AUTHSTATUS_ADDR, m)
#define HWIO_QDSS_CTI_AUTHSTATUS_SNID_BMSK                               0xc0
#define HWIO_QDSS_CTI_AUTHSTATUS_SNID_SHFT                                0x6
#define HWIO_QDSS_CTI_AUTHSTATUS_SID_BMSK                                0x30
#define HWIO_QDSS_CTI_AUTHSTATUS_SID_SHFT                                 0x4
#define HWIO_QDSS_CTI_AUTHSTATUS_NSNID_BMSK                               0xc
#define HWIO_QDSS_CTI_AUTHSTATUS_NSNID_SHFT                               0x2
#define HWIO_QDSS_CTI_AUTHSTATUS_NSID_BMSK                                0x3
#define HWIO_QDSS_CTI_AUTHSTATUS_NSID_SHFT                                0x0

#define HWIO_QDSS_CTI_DEVID_ADDR                                   (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fc8)
#define HWIO_QDSS_CTI_DEVID_RMSK                                      0xfff1f
#define HWIO_QDSS_CTI_DEVID_IN          \
        in_dword_masked(HWIO_QDSS_CTI_DEVID_ADDR, HWIO_QDSS_CTI_DEVID_RMSK)
#define HWIO_QDSS_CTI_DEVID_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_DEVID_ADDR, m)
#define HWIO_QDSS_CTI_DEVID_NUMCH_BMSK                                0xf0000
#define HWIO_QDSS_CTI_DEVID_NUMCH_SHFT                                   0x10
#define HWIO_QDSS_CTI_DEVID_NUMTRIG_BMSK                               0xff00
#define HWIO_QDSS_CTI_DEVID_NUMTRIG_SHFT                                  0x8
#define HWIO_QDSS_CTI_DEVID_EXTMUXNUM_BMSK                               0x1f
#define HWIO_QDSS_CTI_DEVID_EXTMUXNUM_SHFT                                0x0

#define HWIO_QDSS_CTI_DEVTYPE_ADDR                                 (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fcc)
#define HWIO_QDSS_CTI_DEVTYPE_RMSK                                       0xff
#define HWIO_QDSS_CTI_DEVTYPE_IN          \
        in_dword_masked(HWIO_QDSS_CTI_DEVTYPE_ADDR, HWIO_QDSS_CTI_DEVTYPE_RMSK)
#define HWIO_QDSS_CTI_DEVTYPE_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_DEVTYPE_ADDR, m)
#define HWIO_QDSS_CTI_DEVTYPE_SUB_TYPE_BMSK                              0xf0
#define HWIO_QDSS_CTI_DEVTYPE_SUB_TYPE_SHFT                               0x4
#define HWIO_QDSS_CTI_DEVTYPE_MAJOR_TYPE_BMSK                             0xf
#define HWIO_QDSS_CTI_DEVTYPE_MAJOR_TYPE_SHFT                             0x0

#define HWIO_QDSS_CTI_PERIPHID0_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fe0)
#define HWIO_QDSS_CTI_PERIPHID0_RMSK                                     0xff
#define HWIO_QDSS_CTI_PERIPHID0_IN          \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID0_ADDR, HWIO_QDSS_CTI_PERIPHID0_RMSK)
#define HWIO_QDSS_CTI_PERIPHID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID0_ADDR, m)
#define HWIO_QDSS_CTI_PERIPHID0_PART_0_BMSK                              0xff
#define HWIO_QDSS_CTI_PERIPHID0_PART_0_SHFT                               0x0

#define HWIO_QDSS_CTI_PERIPHID1_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fe4)
#define HWIO_QDSS_CTI_PERIPHID1_RMSK                                     0xff
#define HWIO_QDSS_CTI_PERIPHID1_IN          \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID1_ADDR, HWIO_QDSS_CTI_PERIPHID1_RMSK)
#define HWIO_QDSS_CTI_PERIPHID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID1_ADDR, m)
#define HWIO_QDSS_CTI_PERIPHID1_DES_0_BMSK                               0xf0
#define HWIO_QDSS_CTI_PERIPHID1_DES_0_SHFT                                0x4
#define HWIO_QDSS_CTI_PERIPHID1_PART_1_BMSK                               0xf
#define HWIO_QDSS_CTI_PERIPHID1_PART_1_SHFT                               0x0

#define HWIO_QDSS_CTI_PERIPHID2_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fe8)
#define HWIO_QDSS_CTI_PERIPHID2_RMSK                                     0xff
#define HWIO_QDSS_CTI_PERIPHID2_IN          \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID2_ADDR, HWIO_QDSS_CTI_PERIPHID2_RMSK)
#define HWIO_QDSS_CTI_PERIPHID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID2_ADDR, m)
#define HWIO_QDSS_CTI_PERIPHID2_REVISION_BMSK                            0xf0
#define HWIO_QDSS_CTI_PERIPHID2_REVISION_SHFT                             0x4
#define HWIO_QDSS_CTI_PERIPHID2_JEDEC_BMSK                                0x8
#define HWIO_QDSS_CTI_PERIPHID2_JEDEC_SHFT                                0x3
#define HWIO_QDSS_CTI_PERIPHID2_DES_1_BMSK                                0x7
#define HWIO_QDSS_CTI_PERIPHID2_DES_1_SHFT                                0x0

#define HWIO_QDSS_CTI_PERIPHID3_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fec)
#define HWIO_QDSS_CTI_PERIPHID3_RMSK                                     0xff
#define HWIO_QDSS_CTI_PERIPHID3_IN          \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID3_ADDR, HWIO_QDSS_CTI_PERIPHID3_RMSK)
#define HWIO_QDSS_CTI_PERIPHID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID3_ADDR, m)
#define HWIO_QDSS_CTI_PERIPHID3_REVAND_BMSK                              0xf0
#define HWIO_QDSS_CTI_PERIPHID3_REVAND_SHFT                               0x4
#define HWIO_QDSS_CTI_PERIPHID3_CMOD_BMSK                                 0xf
#define HWIO_QDSS_CTI_PERIPHID3_CMOD_SHFT                                 0x0

#define HWIO_QDSS_CTI_PERIPHID4_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fd0)
#define HWIO_QDSS_CTI_PERIPHID4_RMSK                                     0xff
#define HWIO_QDSS_CTI_PERIPHID4_IN          \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID4_ADDR, HWIO_QDSS_CTI_PERIPHID4_RMSK)
#define HWIO_QDSS_CTI_PERIPHID4_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID4_ADDR, m)
#define HWIO_QDSS_CTI_PERIPHID4_SIZE_BMSK                                0xf0
#define HWIO_QDSS_CTI_PERIPHID4_SIZE_SHFT                                 0x4
#define HWIO_QDSS_CTI_PERIPHID4_DES_2_BMSK                                0xf
#define HWIO_QDSS_CTI_PERIPHID4_DES_2_SHFT                                0x0

#define HWIO_QDSS_CTI_PERIPHID5_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fd4)
#define HWIO_QDSS_CTI_PERIPHID5_RMSK                               0xffffffff
#define HWIO_QDSS_CTI_PERIPHID5_IN          \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID5_ADDR, HWIO_QDSS_CTI_PERIPHID5_RMSK)
#define HWIO_QDSS_CTI_PERIPHID5_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID5_ADDR, m)
#define HWIO_QDSS_CTI_PERIPHID5_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_PERIPHID5_ADDR,v)
#define HWIO_QDSS_CTI_PERIPHID5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTI_PERIPHID5_ADDR,m,v,HWIO_QDSS_CTI_PERIPHID5_IN)
#define HWIO_QDSS_CTI_PERIPHID5_PERIPHID5_BMSK                     0xffffffff
#define HWIO_QDSS_CTI_PERIPHID5_PERIPHID5_SHFT                            0x0

#define HWIO_QDSS_CTI_PERIPHID6_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fd8)
#define HWIO_QDSS_CTI_PERIPHID6_RMSK                               0xffffffff
#define HWIO_QDSS_CTI_PERIPHID6_IN          \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID6_ADDR, HWIO_QDSS_CTI_PERIPHID6_RMSK)
#define HWIO_QDSS_CTI_PERIPHID6_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID6_ADDR, m)
#define HWIO_QDSS_CTI_PERIPHID6_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_PERIPHID6_ADDR,v)
#define HWIO_QDSS_CTI_PERIPHID6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTI_PERIPHID6_ADDR,m,v,HWIO_QDSS_CTI_PERIPHID6_IN)
#define HWIO_QDSS_CTI_PERIPHID6_PERIPHID6_BMSK                     0xffffffff
#define HWIO_QDSS_CTI_PERIPHID6_PERIPHID6_SHFT                            0x0

#define HWIO_QDSS_CTI_PERIPHID7_ADDR                               (QDSS_CTI_CSCTI_REG_BASE      + 0x00000fdc)
#define HWIO_QDSS_CTI_PERIPHID7_RMSK                               0xffffffff
#define HWIO_QDSS_CTI_PERIPHID7_IN          \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID7_ADDR, HWIO_QDSS_CTI_PERIPHID7_RMSK)
#define HWIO_QDSS_CTI_PERIPHID7_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_PERIPHID7_ADDR, m)
#define HWIO_QDSS_CTI_PERIPHID7_OUT(v)      \
        out_dword(HWIO_QDSS_CTI_PERIPHID7_ADDR,v)
#define HWIO_QDSS_CTI_PERIPHID7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_CTI_PERIPHID7_ADDR,m,v,HWIO_QDSS_CTI_PERIPHID7_IN)
#define HWIO_QDSS_CTI_PERIPHID7_PERIPHID7_BMSK                     0xffffffff
#define HWIO_QDSS_CTI_PERIPHID7_PERIPHID7_SHFT                            0x0

#define HWIO_QDSS_CTI_COMPID0_ADDR                                 (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ff0)
#define HWIO_QDSS_CTI_COMPID0_RMSK                                       0xff
#define HWIO_QDSS_CTI_COMPID0_IN          \
        in_dword_masked(HWIO_QDSS_CTI_COMPID0_ADDR, HWIO_QDSS_CTI_COMPID0_RMSK)
#define HWIO_QDSS_CTI_COMPID0_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_COMPID0_ADDR, m)
#define HWIO_QDSS_CTI_COMPID0_PRMBL_0_BMSK                               0xff
#define HWIO_QDSS_CTI_COMPID0_PRMBL_0_SHFT                                0x0

#define HWIO_QDSS_CTI_COMPID1_ADDR                                 (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ff4)
#define HWIO_QDSS_CTI_COMPID1_RMSK                                       0xff
#define HWIO_QDSS_CTI_COMPID1_IN          \
        in_dword_masked(HWIO_QDSS_CTI_COMPID1_ADDR, HWIO_QDSS_CTI_COMPID1_RMSK)
#define HWIO_QDSS_CTI_COMPID1_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_COMPID1_ADDR, m)
#define HWIO_QDSS_CTI_COMPID1_CLASS_BMSK                                 0xf0
#define HWIO_QDSS_CTI_COMPID1_CLASS_SHFT                                  0x4
#define HWIO_QDSS_CTI_COMPID1_PRMBL_1_BMSK                                0xf
#define HWIO_QDSS_CTI_COMPID1_PRMBL_1_SHFT                                0x0

#define HWIO_QDSS_CTI_COMPID2_ADDR                                 (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ff8)
#define HWIO_QDSS_CTI_COMPID2_RMSK                                       0xff
#define HWIO_QDSS_CTI_COMPID2_IN          \
        in_dword_masked(HWIO_QDSS_CTI_COMPID2_ADDR, HWIO_QDSS_CTI_COMPID2_RMSK)
#define HWIO_QDSS_CTI_COMPID2_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_COMPID2_ADDR, m)
#define HWIO_QDSS_CTI_COMPID2_PRMBL_2_BMSK                               0xff
#define HWIO_QDSS_CTI_COMPID2_PRMBL_2_SHFT                                0x0

#define HWIO_QDSS_CTI_COMPID3_ADDR                                 (QDSS_CTI_CSCTI_REG_BASE      + 0x00000ffc)
#define HWIO_QDSS_CTI_COMPID3_RMSK                                       0xff
#define HWIO_QDSS_CTI_COMPID3_IN          \
        in_dword_masked(HWIO_QDSS_CTI_COMPID3_ADDR, HWIO_QDSS_CTI_COMPID3_RMSK)
#define HWIO_QDSS_CTI_COMPID3_INM(m)      \
        in_dword_masked(HWIO_QDSS_CTI_COMPID3_ADDR, m)
#define HWIO_QDSS_CTI_COMPID3_PRMBL_3_BMSK                               0xff
#define HWIO_QDSS_CTI_COMPID3_PRMBL_3_SHFT                                0x0


#endif /* __QDSS_CTI_REGS_H__ */
