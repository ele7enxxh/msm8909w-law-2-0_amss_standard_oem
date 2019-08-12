#ifndef __QHSUSB_HWIO_DEPENDENCY_8909_H__
#define __QHSUSB_HWIO_DEPENDENCY_8909_H__
/*
===========================================================================
*/
/**
  @file qhsusb_hwio_dependency_8909.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    USB2_HSIC_USB_OTG_HS_BASE
    GCC_CLK_CTL_REG
    USB2_PHY_CM_DWC_USB2_SWG

  'Include' filters applied: USB[TLMM_CSR] USB[GCC_CLK_CTL_REG] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/wiredconnectivity/qhsusb_hwio_dependency_8909.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/******************************************************************************/
/*#includes from msm.h                                                  */
/******************************************************************************/
/*
 * Common types.
 */
#ifndef _ARM_ASM_
#include "comdef.h"
#endif

/*
 * HWIO access macros.
 */
#include "msmhwio.h"

/*
 * For now include private definitions here until drivers move to
 * include msmp.h directly.
 */
#ifdef FEATURE_LIBRARY_ONLY
#include "msmp.h"
#endif
/*******************************************************************************/
/* # includes which are needed by qhsusb register mapping                      */
/*******************************************************************************/
#include "msmhwiobase.h"

/*****************************************************************************/
/* #defines from msmhwioreg.h                                                */
/*****************************************************************************/


/*----------------------------------------------------------------------------
 * MODULE: USB2_HSIC_USB_OTG_HS_BASE
 *--------------------------------------------------------------------------*/
#define USB2_HSIC_USB_OTG_HS_EP8_PIPES2_BAGHEERA_BASE                                            (0x078C0000)
#define USB2_HSIC_USB_OTG_HS_BASE_REG_BASE                                                       (USB2_HSIC_USB_OTG_HS_EP8_PIPES2_BAGHEERA_BASE      + 0x00019000)
#define USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS                                                  (USB2_HSIC_USB_OTG_HS_EP8_PIPES2_BAGHEERA_BASE_PHYS + 0x00019000)
#define USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS                                                  0x00019000

#define HWIO_USB2_HSIC_USB_OTG_HS_ID_ADDR                                                        (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000000)
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_PHYS                                                        (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000000)
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_OFFS                                                        (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000000)
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_RMSK                                                          0xffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_POR                                                         0x0042fa05
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_POR_RMSK                                                    0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ID_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ID_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ID_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_REVISON_BMSK                                                  0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_REVISON_SHFT                                                      0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_NID_BMSK                                                        0xff00
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_NID_SHFT                                                           0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_ID_BMSK                                                           0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_ID_ID_SHFT                                                            0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000004)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000004)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000004)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_RMSK                                                      0x7ff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_POR                                                  0x000005c2
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_SM_BMSK                                                   0x600
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_SM_SHFT                                                     0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYM_BMSK                                                 0x1c0
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYM_SHFT                                                   0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYW_BMSK                                                  0x30
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_PHYW_SHFT                                                   0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_BWT_BMSK                                                    0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_BWT_SHFT                                                    0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_CLCK_BMSK                                                   0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_CLCK_SHFT                                                   0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_RT_BMSK                                                     0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_HWGENERAL_RT_SHFT                                                     0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_ADDR                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000008)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_PHYS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000008)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_OFFS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000008)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_RMSK                                                    0xffff000f
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_POR                                                     0x10020001
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_POR_RMSK                                                0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_TTPER_BMSK                                              0xff000000
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_TTPER_SHFT                                                    0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_TTASY_BMSK                                                0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_TTASY_SHFT                                                    0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_NPORT_BMSK                                                     0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_NPORT_SHFT                                                     0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_HC_BMSK                                                        0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_HWHOST_HC_SHFT                                                        0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_ADDR                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000000c)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_PHYS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000000c)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_OFFS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000000c)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_RMSK                                                        0x3f
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_POR                                                   0x00000011
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_POR_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_DEVEP_BMSK                                                  0x3e
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_DEVEP_SHFT                                                   0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_DC_BMSK                                                      0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_HWDEVICE_DC_SHFT                                                      0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_ADDR                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000010)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_PHYS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000010)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_OFFS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000010)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_RMSK                                                   0x80ffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_POR                                                    0x80070a08
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_POR_RMSK                                               0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXLCR_BMSK                                             0x80000000
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXLCR_SHFT                                                   0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXCHANADD_BMSK                                           0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXCHANADD_SHFT                                               0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXADD_BMSK                                                 0xff00
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXADD_SHFT                                                    0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXBURST_BMSK                                                 0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWTXBUF_TXBURST_SHFT                                                  0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_ADDR                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000014)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_PHYS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000014)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_OFFS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000014)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RMSK                                                       0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_POR                                                    0x00000808
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_POR_RMSK                                               0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_ADD_BMSK                                                0xff00
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_ADD_SHFT                                                   0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_BURST_BMSK                                                0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_HWRXBUF_RX_BURST_SHFT                                                 0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n)                                           (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_PHYS(n)                                           (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000040 + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_OFFS(n)                                           (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000040 + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_MAXn                                                      15
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_POR                                               0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_POR_RMSK                                          0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_INI(n)        \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n), HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_INMI(n,mask)    \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n), mask)
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_OUTI(n,val)    \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n),val)
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_ADDR(n),mask,val,HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_INI(n))
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_BMSK                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_SCRATCH_RAMn_SCRATCH_REGISTER_SHFT                                    0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000080)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000080)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000080)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_RMSK                                                  0xffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_POR                                                 0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK                                            0xffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0LD_GPTLD_SHFT                                                 0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000084)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_PHYS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000084)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_OFFS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000084)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_RMSK                                              0xc1ffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_POR                                               0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_POR_RMSK                                          0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK                                       0x80000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GTPRUN_SHFT                                             0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK                                       0x40000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTRST_SHFT                                             0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK                                       0x1000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTMODE_SHFT                                            0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_BMSK                                         0xffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER0CTRL_GPTCNT_SHFT                                              0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000088)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000088)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000088)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_RMSK                                                  0xffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_POR                                                 0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_GPTLD_BMSK                                            0xffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1LD_GPTLD_SHFT                                                 0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000008c)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_PHYS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000008c)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_OFFS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000008c)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_RMSK                                              0xc1ffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_POR                                               0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_POR_RMSK                                          0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_BMSK                                       0x80000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GTPRUN_SHFT                                             0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTRST_BMSK                                       0x40000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTRST_SHFT                                             0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_BMSK                                       0x1000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTMODE_SHFT                                            0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_BMSK                                         0xffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GPTIMER1CTRL_GPTCNT_SHFT                                              0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000090)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000090)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000090)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_RMSK                                                        0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_POR                                                  0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_AHB_BURST_BMSK                                              0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_BURST_AHB_BURST_SHFT                                              0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_ADDR                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000094)
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_PHYS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000094)
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_OFFS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000094)
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_RMSK                                                         0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_POR                                                   0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_POR_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_BMSK                                            0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_XTOR_STS_GRANT_STOLEN_SHFT                                            0x1

#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000098)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_PHYS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000098)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_OFFS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000098)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_RMSK                                                  0x8000001f
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_POR                                                   0x00000008
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_POR_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_ASYNC_BRIDGES_BYPASS_BMSK                             0x80000000
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_ASYNC_BRIDGES_BYPASS_SHFT                                   0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_INCR_OVERRIDE_BMSK                                          0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_INCR_OVERRIDE_SHFT                                           0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_HPROT_MODE_BMSK                                              0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_HPROT_MODE_SHFT                                              0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_BMSK                                      0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_GRANT_STOLEN_CLEAR_SHFT                                      0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_BMSK                                             0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_AHB_MODE_XTOR_BYPASS_SHFT                                             0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000009c)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000009c)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000009c)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_RMSK                                                0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_POR                                                 0x00000830
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_HW_QVERSION_BMSK                         0xffff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_USB_OTG_HS_HW_QVERSION_SHFT                               0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_SYS_CLK_SW_EN_BMSK                                      0x8000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_SYS_CLK_SW_EN_SHFT                                         0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_4_BMSK                                      0x4000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_4_SHFT                                         0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_USB_BAM_DISABLE_BMSK                                    0x2000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_USB_BAM_DISABLE_SHFT                                       0xd
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_DMA_HPROT_CTRL_BMSK                                     0x1000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_DMA_HPROT_CTRL_SHFT                                        0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ISO_FIX_EN_BMSK                                          0x800
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ISO_FIX_EN_SHFT                                            0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_DSC_PE_RST_EN_BMSK                                       0x400
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_DSC_PE_RST_EN_SHFT                                         0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK                             0x200
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_SHFT                               0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK                              0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_SHFT                                0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_RX_BUF_PENDING_EN_BMSK                                 0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_RX_BUF_PENDING_EN_SHFT                                  0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_BMSK                              0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_SHFT                               0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ULPI_SERIAL_EN_BMSK                                       0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_ULPI_SERIAL_EN_SHFT                                        0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_BMSK                              0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_PE_DP_TXFIFO_IDLE_FORCE_SHFT                               0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_BMSK                                       0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_TESTMUX_SEL_3_0_SHFT                                       0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000000a0)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_PHYS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000000a0)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_OFFS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000000a0)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RMSK                                               0x3ffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_POR                                               0x0038ff60
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_POR_RMSK                                          0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_BVALID_SW_SEL_BMSK                                 0x2000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_BVALID_SW_SEL_SHFT                                      0x19
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_BVALID_SW_BMSK                                     0x1000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_BVALID_SW_SHFT                                          0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_AVALID_SW_SEL_BMSK                                  0x800000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_AVALID_SW_SEL_SHFT                                      0x17
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_AVALID_SW_BMSK                                      0x400000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_AVALID_SW_SHFT                                          0x16
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_IDDIG_SW_SEL_BMSK                                   0x200000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_IDDIG_SW_SEL_SHFT                                       0x15
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_IDDIG_SW_BMSK                                       0x100000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_IDDIG_SW_SHFT                                           0x14
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ULPI_TX_PKT_EN_CLR_FIX_BMSK                          0x80000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ULPI_TX_PKT_EN_CLR_FIX_SHFT                             0x13
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_TRANSACTOR_NONPOST_WRITE_DIS_BMSK                    0x40000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_TRANSACTOR_NONPOST_WRITE_DIS_SHFT                       0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_FPR_CTRL_BMSK                                        0x20000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_FPR_CTRL_SHFT                                           0x11
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SW_RESET_ALT_EN_BMSK                                 0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SW_RESET_ALT_EN_SHFT                                    0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_DPSE_DMSE_HV_INTR_EN_BMSK                             0x8000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_DPSE_DMSE_HV_INTR_EN_SHFT                                0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_USB_HW_SW_EVENTS_TOG_EN_BMSK                          0x4000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_USB_HW_SW_EVENTS_TOG_EN_SHFT                             0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SYS_CLK_HOST_DEV_GATE_EN_BMSK                         0x2000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SYS_CLK_HOST_DEV_GATE_EN_SHFT                            0xd
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_LINESTATE_DIFF_WAKEUP_EN_BMSK                         0x1000
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_LINESTATE_DIFF_WAKEUP_EN_SHFT                            0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ULPI_LPM_PEND_EN_BMSK                                  0x800
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ULPI_LPM_PEND_EN_SHFT                                    0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RX_FULL_NAK_EN_BMSK                                    0x400
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RX_FULL_NAK_EN_SHFT                                      0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ENDLESS_TD_EN_BMSK                                     0x200
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ENDLESS_TD_EN_SHFT                                       0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SCRATCH_RAM_EN_BMSK                                    0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SCRATCH_RAM_EN_SHFT                                      0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_BMSK                                   0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_SESS_VLD_CTRL_EN_SHFT                                    0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_CI_T_WTSUSRSTHS_EN_BMSK                                 0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_CI_T_WTSUSRSTHS_EN_SHFT                                  0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_CI_T_UCH_EN_BMSK                                        0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_CI_T_UCH_EN_SHFT                                         0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_DP_RESET_BMSK                                           0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_DP_RESET_SHFT                                            0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ZLP_PRIME_BMSK                                           0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_ZLP_PRIME_SHFT                                           0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_NO_SOF_RX_FIFO_FULL_BMSK                                 0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_NO_SOF_RX_FIFO_FULL_SHFT                                 0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_WRONG_OPMODE_SUSP_BMSK                                   0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_WRONG_OPMODE_SUSP_SHFT                                   0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RESUME_END_INTER_BMSK                                    0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_GEN_CONFIG_2_RESUME_END_INTER_SHFT                                    0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000100)
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000100)
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000100)
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_RMSK                                                 0xffff00ff
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_POR                                                  0x01000040
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_HCIVERSION_BMSK                                      0xffff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_HCIVERSION_SHFT                                            0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_CAPLENGTH_BMSK                                             0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_CAPLENGTH_CAPLENGTH_SHFT                                              0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000104)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000104)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000104)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_RMSK                                                  0xff1ff1f
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_POR                                                  0x00010011
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_TT_BMSK                                             0xf000000
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_TT_SHFT                                                  0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PTT_BMSK                                             0xf00000
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PTT_SHFT                                                 0x14
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PI_BMSK                                                 0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PI_SHFT                                                    0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_CC_BMSK                                                0xf000
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_CC_SHFT                                                   0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PCC_BMSK                                                0xf00
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PCC_SHFT                                                  0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PPC_BMSK                                                   0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_PPC_SHFT                                                    0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PORTS_BMSK                                                0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_HCSPARAMS_N_PORTS_SHFT                                                0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000108)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000108)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000108)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_RMSK                                                     0xfff7
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_POR                                                  0x00000006
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_EECP_BMSK                                                0xff00
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_EECP_SHFT                                                   0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_IST_BMSK                                                   0xf0
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_IST_SHFT                                                    0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ASP_BMSK                                                    0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ASP_SHFT                                                    0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_PFL_BMSK                                                    0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_PFL_SHFT                                                    0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADC_BMSK                                                    0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_HCCPARAMS_ADC_SHFT                                                    0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000120)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000120)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000120)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_RMSK                                                    0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_POR                                                 0x00000001
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_DCIVERSION_BMSK                                         0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_DCIVERSION_DCIVERSION_SHFT                                            0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000124)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000124)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000124)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_RMSK                                                      0x19f
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_POR                                                  0x00000190
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_HC_BMSK                                                   0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_HC_SHFT                                                     0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DC_BMSK                                                    0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DC_SHFT                                                     0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DEN_BMSK                                                   0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_DCCPARAMS_DEN_SHFT                                                    0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000140)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_PHYS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000140)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_OFFS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000140)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RMSK                                                    0xfeffebff
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_POR                                                     0x00080000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_POR_RMSK                                                0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RST_CTRL_BMSK                                           0x80000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RST_CTRL_SHFT                                                 0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_BMSK                                      0x40000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_STP_CTRL_SHFT                                            0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_BMSK                                    0x20000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASYNC_INTR_CTRL_SHFT                                          0x1d
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK                                0x10000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SE0_GLITCH_FIX_CTRL_SHFT                                      0x1c
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS_3_WIRE_2_WIRE_SELECT_BMSK                             0x8000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS_3_WIRE_2_WIRE_SELECT_SHFT                                  0x1b
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_SER3_NOT6_SEL_BMSK                                  0x4000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ULPI_SER3_NOT6_SEL_SHFT                                       0x1a
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_BMSK                                       0x2000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SESS_VLD_CTRL_SHFT                                            0x19
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ITC_BMSK                                                  0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ITC_SHFT                                                      0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS2_BMSK                                                    0x8000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS2_SHFT                                                       0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ATDTW_BMSK                                                  0x4000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ATDTW_SHFT                                                     0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SUTW_BMSK                                                   0x2000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_SUTW_SHFT                                                      0xd
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASPE_BMSK                                                    0x800
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASPE_SHFT                                                      0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASP_BMSK                                                     0x300
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASP_SHFT                                                       0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_LR_BMSK                                                       0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_LR_SHFT                                                        0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_IAA_BMSK                                                      0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_IAA_SHFT                                                       0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASE_BMSK                                                      0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_ASE_SHFT                                                       0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_PSE_BMSK                                                      0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_PSE_SHFT                                                       0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS_BMSK                                                        0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_FS_SHFT                                                        0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RST_BMSK                                                       0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RST_SHFT                                                       0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RS_BMSK                                                        0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_USBCMD_RS_SHFT                                                        0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000144)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PHYS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000144)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_OFFS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000144)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_RMSK                                                    0xfb0df5ff
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_POR                                                     0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_POR_RMSK                                                0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ULPI_INTR_BMSK                                          0x80000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ULPI_INTR_SHFT                                                0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_SESS_VLD_CHG_BMSK                                   0x40000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_SESS_VLD_CHG_SHFT                                         0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_SESS_VLD_BMSK                                       0x20000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_SESS_VLD_SHFT                                             0x1d
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_ALT_INT_BMSK                                        0x10000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PHY_ALT_INT_SHFT                                              0x1c
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ASYNC_WAKEUP_INTR_BMSK                                   0x8000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ASYNC_WAKEUP_INTR_SHFT                                        0x1b
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_TI1_BMSK                                                 0x2000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_TI1_SHFT                                                      0x19
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_TI0_BMSK                                                 0x1000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_TI0_SHFT                                                      0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UPI_BMSK                                                   0x80000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UPI_SHFT                                                      0x13
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UAI_BMSK                                                   0x40000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UAI_SHFT                                                      0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_NAKI_BMSK                                                  0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_NAKI_SHFT                                                     0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_AS_BMSK                                                     0x8000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_AS_SHFT                                                        0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PS_BMSK                                                     0x4000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PS_SHFT                                                        0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_RCL_BMSK                                                    0x2000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_RCL_SHFT                                                       0xd
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_HCH_BMSK                                                    0x1000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_HCH_SHFT                                                       0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ULPII_BMSK                                                   0x400
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_ULPII_SHFT                                                     0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SLI_BMSK                                                     0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SLI_SHFT                                                       0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SRI_BMSK                                                      0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SRI_SHFT                                                       0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_URI_BMSK                                                      0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_URI_SHFT                                                       0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_AAI_BMSK                                                      0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_AAI_SHFT                                                       0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SEI_BMSK                                                      0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_SEI_SHFT                                                       0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_FRI_BMSK                                                       0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_FRI_SHFT                                                       0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PCI_BMSK                                                       0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_PCI_SHFT                                                       0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UEI_BMSK                                                       0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UEI_SHFT                                                       0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UI_BMSK                                                        0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_USBSTS_UI_SHFT                                                        0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000148)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_PHYS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000148)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_OFFS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000148)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_RMSK                                                   0xc30d05ff
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_POR                                                    0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_POR_RMSK                                               0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ULPI_INTR_EN_BMSK                                      0x80000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ULPI_INTR_EN_SHFT                                            0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_PHY_SESS_VLD_CHG_EN_BMSK                               0x40000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_PHY_SESS_VLD_CHG_EN_SHFT                                     0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_TIE1_BMSK                                               0x2000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_TIE1_SHFT                                                    0x19
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_TIE0_BMSK                                               0x1000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_TIE0_SHFT                                                    0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UPIE_BMSK                                                 0x80000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UPIE_SHFT                                                    0x13
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UAIE_BMSK                                                 0x40000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UAIE_SHFT                                                    0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_NAKE_BMSK                                                 0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_NAKE_SHFT                                                    0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ULPIE_BMSK                                                  0x400
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_ULPIE_SHFT                                                    0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SLE_BMSK                                                    0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SLE_SHFT                                                      0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SRE_BMSK                                                     0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SRE_SHFT                                                      0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_URE_BMSK                                                     0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_URE_SHFT                                                      0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_AAE_BMSK                                                     0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_AAE_SHFT                                                      0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SEE_BMSK                                                     0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_SEE_SHFT                                                      0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_FRE_BMSK                                                      0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_FRE_SHFT                                                      0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_PCE_BMSK                                                      0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_PCE_SHFT                                                      0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UEE_BMSK                                                      0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UEE_SHFT                                                      0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UE_BMSK                                                       0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_USBINTR_UE_SHFT                                                       0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000014c)
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_PHYS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000014c)
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_OFFS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000014c)
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_RMSK                                                       0x3fff
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_POR                                                    0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_POR_RMSK                                               0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_FRINDEX_BMSK                                               0x3fff
#define HWIO_USB2_HSIC_USB_OTG_HS_FRINDEX_FRINDEX_SHFT                                                  0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR                                          (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000154)
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_PHYS                                          (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000154)
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_OFFS                                          (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000154)
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_RMSK                                          0xfffff000
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_POR                                           0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_POR_RMSK                                      0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_PERBASE_BMSK                                  0xfffff000
#define HWIO_USB2_HSIC_USB_OTG_HS_PERIODICLISTBASE_PERBASE_SHFT                                         0xc

#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000154)
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000154)
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000154)
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_RMSK                                                0xff000000
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_POR                                                 0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADR_BMSK                                         0xfe000000
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADR_SHFT                                               0x19
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADRA_BMSK                                         0x1000000
#define HWIO_USB2_HSIC_USB_OTG_HS_DEVICEADDR_USBADRA_SHFT                                              0x18

#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000158)
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_PHYS                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000158)
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_OFFS                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000158)
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_RMSK                                             0xffffffe0
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_POR                                              0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_POR_RMSK                                         0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_BMSK                                     0xffffffe0
#define HWIO_USB2_HSIC_USB_OTG_HS_ASYNCLISTADDR_ASYBASE_SHFT                                            0x5

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR                                          (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000158)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_PHYS                                          (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000158)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_OFFS                                          (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000158)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_RMSK                                          0xfffff800
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_POR                                           0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_POR_RMSK                                      0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_BMSK                                   0xfffff800
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPOINTLISTADDR_EPBASE_SHFT                                          0xb

#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000015c)
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_PHYS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000015c)
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_OFFS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000015c)
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_RMSK                                                    0x7f000000
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_POR                                                     0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_POR_RMSK                                                0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_TTHA_BMSK                                               0x7f000000
#define HWIO_USB2_HSIC_USB_OTG_HS_TTCTRL_TTHA_SHFT                                                     0x18

#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000160)
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000160)
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000160)
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RMSK                                                     0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_POR                                                  0x00001010
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_TXPBURST_BMSK                                            0xff00
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_TXPBURST_SHFT                                               0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RXPBURST_BMSK                                              0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_BURSTSIZE_RXPBURST_SHFT                                               0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000164)
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_PHYS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000164)
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_OFFS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000164)
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_RMSK                                                0x3f1fff
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_POR                                               0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_POR_RMSK                                          0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_BMSK                                    0x3f0000
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXFIFOTHRES_SHFT                                        0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_BMSK                                      0x1f00
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHHEALTH_SHFT                                         0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHOH_BMSK                                            0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_TXFILLTUNING_TXSCHOH_SHFT                                             0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000170)
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_PHYS                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000170)
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_OFFS                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000170)
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_POR                                              0x08000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_POR_RMSK                                         0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_BMSK                                      0x80000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIWU_SHFT                                            0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK                                     0x40000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_SHFT                                           0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK                                      0x20000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_SHFT                                            0x1d
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIFORCE_BMSK                                   0x10000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIFORCE_SHFT                                         0x1c
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK                                       0x8000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPISS_SHFT                                            0x1b
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_BMSK                                     0x7000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIPORT_SHFT                                          0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_BMSK                                      0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT                                          0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK                                       0xff00
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT                                          0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_BMSK                                         0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_SHFT                                          0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000178)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_PHYS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000178)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_OFFS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000178)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_RMSK                                                    0xff00ff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_POR                                                   0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_POR_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPTN_BMSK                                               0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPTN_SHFT                                                   0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPRN_BMSK                                                   0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAK_EPRN_SHFT                                                    0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000017c)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000017c)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000017c)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_RMSK                                                  0xff00ff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_POR                                                 0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPTNE_BMSK                                            0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPTNE_SHFT                                                0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPRNE_BMSK                                                0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTNAKEN_EPRNE_SHFT                                                 0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000184)
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PHYS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000184)
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OFFS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000184)
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_RMSK                                                    0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_POR                                                     0xcc000004
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_POR_RMSK                                                0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTS_BMSK                                                0xc0000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTS_SHFT                                                      0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_STS_BMSK                                                0x20000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_STS_SHFT                                                      0x1d
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTW_BMSK                                                0x10000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTW_SHFT                                                      0x1c
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PSPD_BMSK                                                0xc000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PSPD_SHFT                                                     0x1a
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_SPRT_BMSK                                                0x2000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_SPRT_SHFT                                                     0x19
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PFSC_BMSK                                                0x1000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PFSC_SHFT                                                     0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PHCD_BMSK                                                 0x800000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PHCD_SHFT                                                     0x17
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKOC_BMSK                                                 0x400000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKOC_SHFT                                                     0x16
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKDS_BMSK                                                 0x200000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKDS_SHFT                                                     0x15
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKCN_BMSK                                                 0x100000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_WKCN_SHFT                                                     0x14
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTC_BMSK                                                   0xf0000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PTC_SHFT                                                      0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PIC_BMSK                                                    0xc000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PIC_SHFT                                                       0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PO_BMSK                                                     0x2000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PO_SHFT                                                        0xd
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PP_BMSK                                                     0x1000
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PP_SHFT                                                        0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_LS_BMSK                                                      0xc00
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_LS_SHFT                                                        0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_HSP_BMSK                                                     0x200
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_HSP_SHFT                                                       0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PR_BMSK                                                      0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PR_SHFT                                                        0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_SUSP_BMSK                                                     0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_SUSP_SHFT                                                      0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_FPR_BMSK                                                      0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_FPR_SHFT                                                       0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OCC_BMSK                                                      0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OCC_SHFT                                                       0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OCA_BMSK                                                      0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_OCA_SHFT                                                       0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PEC_BMSK                                                       0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PEC_SHFT                                                       0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PE_BMSK                                                        0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_PE_SHFT                                                        0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_CSC_BMSK                                                       0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_CSC_SHFT                                                       0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_CCS_BMSK                                                       0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_PORTSC_CCS_SHFT                                                       0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR                                                     (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001a4)
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_PHYS                                                     (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001a4)
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_OFFS                                                     (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001a4)
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_RMSK                                                     0x7f7f7fff
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_POR                                                      0x00000e20
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_POR_RMSK                                                 0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPIE_BMSK                                                0x40000000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPIE_SHFT                                                      0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSE_BMSK                                          0x20000000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSE_SHFT                                                0x1d
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIE_BMSK                                               0x10000000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIE_SHFT                                                     0x1c
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIE_BMSK                                                0x8000000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIE_SHFT                                                     0x1b
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIE_BMSK                                                0x4000000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIE_SHFT                                                     0x1a
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIE_BMSK                                                0x2000000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIE_SHFT                                                     0x19
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDIE_BMSK                                                 0x1000000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDIE_SHFT                                                      0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPIS_BMSK                                                  0x400000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPIS_SHFT                                                      0x16
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSS_BMSK                                            0x200000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MSS_SHFT                                                0x15
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIS_BMSK                                                 0x100000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSEIS_SHFT                                                     0x14
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIS_BMSK                                                  0x80000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSVIS_SHFT                                                     0x13
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIS_BMSK                                                  0x40000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASVIS_SHFT                                                     0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIS_BMSK                                                  0x20000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVVIS_SHFT                                                     0x11
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDIS_BMSK                                                   0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDIS_SHFT                                                      0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPS_BMSK                                                     0x4000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DPS_SHFT                                                        0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MST_BMSK                                              0x2000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_FIELD_1MST_SHFT                                                 0xd
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSE_BMSK                                                     0x1000
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSE_SHFT                                                        0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSV_BMSK                                                      0x800
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_BSV_SHFT                                                        0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASV_BMSK                                                      0x400
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ASV_SHFT                                                        0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVV_BMSK                                                      0x200
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_AVV_SHFT                                                        0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ID_BMSK                                                       0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_ID_SHFT                                                         0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HABA_BMSK                                                      0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HABA_SHFT                                                       0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HADP_BMSK                                                      0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HADP_SHFT                                                       0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDPU_BMSK                                                      0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_IDPU_SHFT                                                       0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DP_BMSK                                                        0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_DP_SHFT                                                         0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_OT_BMSK                                                         0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_OT_SHFT                                                         0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HAAR_BMSK                                                       0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_HAAR_SHFT                                                       0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_VC_BMSK                                                         0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_VC_SHFT                                                         0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_VD_BMSK                                                         0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_OTGSC_VD_SHFT                                                         0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001a8)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_PHYS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001a8)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_OFFS                                                   (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001a8)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_RMSK                                                         0x3f
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_POR                                                    0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_POR_RMSK                                               0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_VBPS_BMSK                                                    0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_VBPS_SHFT                                                     0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_SDIS_BMSK                                                    0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_SDIS_SHFT                                                     0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_SLOM_BMSK                                                     0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_SLOM_SHFT                                                     0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ES_BMSK                                                       0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_ES_SHFT                                                       0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_CM_BMSK                                                       0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_USBMODE_CM_SHFT                                                       0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001ac)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_PHYS                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001ac)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_OFFS                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001ac)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_RMSK                                                0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_POR                                             0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_POR_RMSK                                        0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_BMSK                                 0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSETUPSTAT_ENDPTSETUPSTAT_SHFT                                    0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001b0)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b0)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b0)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_RMSK                                                  0xff00ff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_POR                                                 0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PETB_BMSK                                             0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PETB_SHFT                                                 0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PERB_BMSK                                                 0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTPRIME_PERB_SHFT                                                  0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001b4)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b4)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b4)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_RMSK                                                  0xff00ff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_POR                                                 0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FETB_BMSK                                             0xff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FETB_SHFT                                                 0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FERB_BMSK                                                 0xff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTFLUSH_FERB_SHFT                                                  0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001b8)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001b8)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001b8)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_RMSK                                                 0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_POR                                                  0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ETBR_BMSK                                            0xffff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ETBR_SHFT                                                  0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ERBR_BMSK                                                0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTSTAT_ERBR_SHFT                                                   0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001bc)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_PHYS                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001bc)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_OFFS                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001bc)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_POR                                              0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_POR_RMSK                                         0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ETCE_BMSK                                        0xffff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT                                              0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ERCE_BMSK                                            0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCOMPLETE_ERCE_SHFT                                               0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001c0)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001c0)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001c0)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RMSK                                                  0x8d008d
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_POR                                                 0x00800080
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXE_BMSK                                              0x800000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXE_SHFT                                                  0x17
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXT_BMSK                                               0xc0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXT_SHFT                                                  0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXS_BMSK                                               0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_TXS_SHFT                                                  0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXE_BMSK                                                  0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXE_SHFT                                                   0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXT_BMSK                                                   0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXT_SHFT                                                   0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXS_BMSK                                                   0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRL0_RXS_SHFT                                                   0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n)                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001c0 + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_PHYS(n)                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001c0 + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_OFFS(n)                                             (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001c0 + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RMSK                                                  0xef00ef
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_MAXn                                                        15
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_POR                                                 0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_INI(n)        \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n), HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_INMI(n,mask)    \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n), mask)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_OUTI(n,val)    \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n),val)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_ADDR(n),mask,val,HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_INI(n))
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXE_BMSK                                              0x800000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXE_SHFT                                                  0x17
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXR_BMSK                                              0x400000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXR_SHFT                                                  0x16
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXI_BMSK                                              0x200000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXI_SHFT                                                  0x15
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXT_BMSK                                               0xc0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXT_SHFT                                                  0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXD_BMSK                                               0x20000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXD_SHFT                                                  0x11
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXS_BMSK                                               0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_TXS_SHFT                                                  0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXE_BMSK                                                  0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXE_SHFT                                                   0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXR_BMSK                                                  0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXR_SHFT                                                   0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXI_BMSK                                                  0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXI_SHFT                                                   0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXT_BMSK                                                   0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXT_SHFT                                                   0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXD_BMSK                                                   0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXD_SHFT                                                   0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXS_BMSK                                                   0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPTCTRLn_RXS_SHFT                                                   0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n)                                         (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x000001fc + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_PHYS(n)                                         (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x000001fc + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_OFFS(n)                                         (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x000001fc + 0x4 * (n))
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_RMSK                                              0x1f001f
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_MAXn                                                    15
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_POR                                             0x001f001f
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_POR_RMSK                                        0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_INI(n)        \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n), HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_INMI(n,mask)    \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n), mask)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_OUTI(n,val)    \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n),val)
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_ADDR(n),mask,val,HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_INI(n))
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_TX_PIPE_ID_BMSK                                   0x1f0000
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_TX_PIPE_ID_SHFT                                       0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_RX_PIPE_ID_BMSK                                       0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_ENDPT_PIPE_IDn_RX_PIPE_ID_SHFT                                        0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_ADDR                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000240)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_PHYS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000240)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_OFFS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000240)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_RMSK                                                  0x7ffeffff
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_POR                                                   0x388c3c3a
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_POR_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLK_REF_DIV2_BMSK                            0x40000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLK_REF_DIV2_SHFT                                  0x1e
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_PLLPTUNE_BMSK                                0x3c000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_PLLPTUNE_SHFT                                      0x1a
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_PLLITNUE_BMSK                                 0x3000000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_PLLITNUE_SHFT                                      0x18
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_VREGBYPASS_BMSK                                0x800000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_VREGBYPASS_SHFT                                    0x17
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_PLLBTUNE_BMSK                                  0x400000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_PLLBTUNE_SHFT                                      0x16
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK                  0x200000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_SHFT                      0x15
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSE_INTEN_BMSK                                0x100000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSE_INTEN_SHFT                                    0x14
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSEHV_CLAMP_EN_BMSK                            0x80000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSEHV_CLAMP_EN_SHFT                               0x13
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_ULPI_CLK_EN_BMSK                                0x40000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_ULPI_CLK_EN_SHFT                                   0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_REFCLKOUT_EN_BMSK                               0x20000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_REFCLKOUT_EN_SHFT                                  0x11
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_USE_CLKCORE_BMSK                                 0x8000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_USE_CLKCORE_SHFT                                    0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSE_INTEN_BMSK                                  0x4000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSE_INTEN_SHFT                                     0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSEHV_CLAMP_EN_BMSK                             0x2000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSEHV_CLAMP_EN_SHFT                                0xd
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_IDHV_CLAMP_EN_BMSK                               0x1000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_IDHV_CLAMP_EN_SHFT                                  0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_OTGSESSVLDHV_CLAMP_EN_BMSK                        0x800
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_OTGSESSVLDHV_CLAMP_EN_SHFT                          0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_PHY_MPM_HV_CLAMP_EN_BMSK                                   0x400
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_PHY_MPM_HV_CLAMP_EN_SHFT                                     0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_OTGSESSVLDHV_INTEN_BMSK                           0x200
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_OTGSESSVLDHV_INTEN_SHFT                             0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_IDHV_INTEN_BMSK                                   0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_IDHV_INTEN_SHFT                                     0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_ULPI_POR_BMSK                                      0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_ULPI_POR_SHFT                                       0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_FSEL_BMSK                                          0x70
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_FSEL_SHFT                                           0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_HOST_PORTCTRL_FORCE_SUSEN_BMSK                               0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_HOST_PORTCTRL_FORCE_SUSEN_SHFT                               0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_SIDDQ_BMSK                                          0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_SIDDQ_SHFT                                          0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_RETEN_BMSK                                          0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_RETEN_SHFT                                          0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_POR_BMSK                                            0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_CTRL_USB2_PHY_POR_SHFT                                            0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_ADDR                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000244)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_PHYS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000244)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_OFFS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000244)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_RMSK                                                  0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_POR                                                   0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_POR_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_USB_HS_TX_DEPTH_BMSK                                  0xffff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_USB_HS_TX_DEPTH_SHFT                                        0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_USB_HS_RX_DEPTH_BMSK                                      0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC1_USB_HS_RX_DEPTH_SHFT                                         0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_ADDR                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000248)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_PHYS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000248)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_OFFS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000248)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_RMSK                                                     0xfffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_POR                                                   0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_POR_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_HBM_BMSK                                             0x80000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_HBM_SHFT                                                0x13
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_EXT_HSIC_BMSK                                        0x40000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_EXT_HSIC_SHFT                                           0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SECURITY_BMSK                                        0x20000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SECURITY_SHFT                                           0x11
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_FS_SIE_BMSK                                          0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_FS_SIE_SHFT                                             0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SPS_AHB2AHB_BMSK                                      0x8000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SPS_AHB2AHB_SHFT                                         0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_LPM_SUPPORT_BMSK                                          0x4000
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_LPM_SUPPORT_SHFT                                             0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USB_HS_DEV_EP_BMSK                                        0x3e00
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USB_HS_DEV_EP_SHFT                                           0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_MAX_PIPES_BMSK                                             0x1f8
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_MAX_PIPES_SHFT                                               0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SPS_BMSK                                                 0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_SPS_SHFT                                                 0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_HSIC_BMSK                                                0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_USE_HSIC_SHFT                                                0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_UTMI_PHY_SW_IF_EN_BMSK                                       0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_GENERIC2_UTMI_PHY_SW_IF_EN_SHFT                                       0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000250)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000250)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000250)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_RMSK                                                0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_POR                                                 0x0000ffff
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_BMSK                            0xffff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_SHFT                                  0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_TX_EP_PRIME_L1_EN_BMSK                                  0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_EP_CTRL_TX_EP_PRIME_L1_EN_SHFT                                     0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000254)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PHYS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000254)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_OFFS                                                 (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000254)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_RMSK                                                     0x1fff
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_POR                                                  0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_POR_RMSK                                             0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_L1_SUSP_SLEEP_SEL_BMSK                                   0x1000
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_L1_SUSP_SLEEP_SEL_SHFT                                      0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PLL_PWR_DWN_EN_BMSK                                       0x800
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PLL_PWR_DWN_EN_SHFT                                         0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PHY_LPM_EN_BMSK                                           0x400
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PHY_LPM_EN_SHFT                                             0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_AHB_CLK_EN_BMSK                                      0x200
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_AHB_CLK_EN_SHFT                                        0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_FS_XCVR_CLK_EN_BMSK                                  0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_FS_XCVR_CLK_EN_SHFT                                    0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_SYS_CLK_EN_BMSK                                       0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_SYS_CLK_EN_SHFT                                        0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_XCVR_CLK_EN_BMSK                                      0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_GATE_XCVR_CLK_EN_SHFT                                       0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_L1_REMOTE_WAKEUP_EN_BMSK                                   0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_L1_REMOTE_WAKEUP_EN_SHFT                                    0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_LPM_EN_BMSK                                                0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_LPM_EN_SHFT                                                 0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_BMSK                                   0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_SHFT                                   0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_ADDR                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000258)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_PHYS                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000258)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_OFFS                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000258)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_RMSK                                               0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_POR                                                0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_POR_RMSK                                           0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_DEBUG_L1_LONG_ENT_CNT_BMSK                         0xffff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_DEBUG_L1_LONG_ENT_CNT_SHFT                               0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_DEBUG_L1_SHORT_ENT_CNT_BMSK                            0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_1_DEBUG_L1_SHORT_ENT_CNT_SHFT                               0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x0000025c)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_PHYS                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x0000025c)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_OFFS                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x0000025c)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_RMSK                                                   0x1fff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_POR                                                0x000000fc
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_POR_RMSK                                           0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_RMT_WKUP_TIME_BMSK                                  0x1e00
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_RMT_WKUP_TIME_SHFT                                     0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_FPR_BMSK                                             0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_FPR_SHFT                                               0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_PLL_BYPASSNL_BMSK                               0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_PLL_BYPASSNL_SHFT                                0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_PLL_RESET_BMSK                                  0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_PLL_RESET_SHFT                                   0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_GATE_BMSK                                       0x20
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_HSIC_CLK_GATE_SHFT                                        0x5
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_FS_XCVR_CLK_GATE_BMSK                                    0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_FS_XCVR_CLK_GATE_SHFT                                     0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_SYS_CLK_GATE_BMSK                                         0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_SYS_CLK_GATE_SHFT                                         0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_AHB_CLK_GATE_BMSK                                         0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_AHB_CLK_GATE_SHFT                                         0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_STATE_BMSK                                             0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_L1_STATE_SHFT                                             0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_DEBUG_L1_EN_BMSK                                          0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_DEBUG_2_DEBUG_L1_EN_SHFT                                          0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_ADDR                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000260)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_PHYS                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000260)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_OFFS                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000260)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_RMSK                                                  0x1f
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_POR                                             0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_POR_RMSK                                        0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_BREMOTEWAKE_BMSK                                      0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_BREMOTEWAKE_SHFT                                       0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_HIRD_BMSK                                              0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_ATTRIBUTES_HIRD_SHFT                                              0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000264)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000264)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000264)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_RMSK                                                     0xfff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_POR                                                 0x00000078
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_LINKPM_RESUME_TIME_BMSK                                  0xfff
#define HWIO_USB2_HSIC_USB_OTG_HS_LPM_TIMERS_LINKPM_RESUME_TIME_SHFT                                    0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_ADDR                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000270)
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_PHYS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000270)
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_OFFS                                                    (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000270)
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_RMSK                                                    0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_POR                                                     0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_POR_RMSK                                                0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_MAJOR_BMSK                                              0xf0000000
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_MAJOR_SHFT                                                    0x1c
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_MINOR_BMSK                                               0xfff0000
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_MINOR_SHFT                                                    0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_STEP_BMSK                                                   0xffff
#define HWIO_USB2_HSIC_USB_OTG_HS_HW_VER_STEP_SHFT                                                      0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_ADDR                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000274)
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_PHYS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000274)
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_OFFS                                                  (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000274)
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_RMSK                                                  0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_POR                                                   0x00000000
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_POR_RMSK                                              0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_USB2_TEST_BUS_BMSK                                    0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_TEST_BUS_USB2_TEST_BUS_SHFT                                           0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_ADDR                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000278)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_PHYS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000278)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_OFFS                                              (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000278)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_RMSK                                                0x3dfff7
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_POR                                               0x000c3c32
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_POR_RMSK                                          0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_2_BMSK            0x200000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_2_SHFT                0x15
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DMSE_INTEN_2_BMSK                          0x100000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DMSE_INTEN_2_SHFT                              0x14
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DMSEHV_CLAMP_EN_2_BMSK                      0x80000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DMSEHV_CLAMP_EN_2_SHFT                         0x13
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_ULPI_CLK_EN_2_BMSK                          0x40000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_ULPI_CLK_EN_2_SHFT                             0x12
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_HS2_DIG_CLAMP_N_2_BMSK                      0x10000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_HS2_DIG_CLAMP_N_2_SHFT                         0x10
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_USE_CLKCORE_2_BMSK                           0x8000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_USE_CLKCORE_2_SHFT                              0xf
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DPSE_INTEN_2_BMSK                            0x4000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DPSE_INTEN_2_SHFT                               0xe
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DPSEHV_CLAMP_EN_2_BMSK                       0x2000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_DPSEHV_CLAMP_EN_2_SHFT                          0xd
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_IDHV_CLAMP_EN_2_BMSK                         0x1000
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_IDHV_CLAMP_EN_2_SHFT                            0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_OTGSESSVLDHV_CLAMP_EN_2_BMSK                  0x800
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_OTGSESSVLDHV_CLAMP_EN_2_SHFT                    0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_PHY_MPM_HV_CLAMP_EN_2_BMSK                             0x400
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_PHY_MPM_HV_CLAMP_EN_2_SHFT                               0xa
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_OTGSESSVLDHV_INTEN_2_BMSK                     0x200
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_OTGSESSVLDHV_INTEN_2_SHFT                       0x9
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_IDHV_INTEN_2_BMSK                             0x100
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_IDHV_INTEN_2_SHFT                               0x8
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_ULPI_POR_2_BMSK                                0x80
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_ULPI_POR_2_SHFT                                 0x7
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_FSEL_2_BMSK                                    0x70
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_FSEL_2_SHFT                                     0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_SIDDQ_2_BMSK                                    0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_SIDDQ_2_SHFT                                    0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_RETEN_2_BMSK                                    0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_RETEN_2_SHFT                                    0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_POR_2_BMSK                                      0x1
#define HWIO_USB2_HSIC_USB_OTG_HS_PHY_SEC_CTRL_USB2_PHY_POR_2_SHFT                                      0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000340)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_PHYS                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000340)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_OFFS                                               (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000340)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_RMSK                                                 0x3fffff
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_POR                                                0x000b40a2
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_POR_RMSK                                           0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_HST_BMSK                                 0x3ff800
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_HST_SHFT                                      0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_DEV_BMSK                                    0x7ff
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_HS_BTO_TIME_HS_DEV_SHFT                                      0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000344)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_PHYS                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000344)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_OFFS                                            (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000344)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_RMSK                                              0x3fffff
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_POR                                             0x002b20cc
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_POR_RMSK                                        0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_LS_BMSK                                  0x3ff800
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_LS_SHFT                                       0xb
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_FS_BMSK                                     0x7ff
#define HWIO_USB2_HSIC_USB_OTG_HS_BTO_TIME_FS_LS_BTO_TIME_FS_SHFT                                       0x0

#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE      + 0x00000348)
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_PHYS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_PHYS + 0x00000348)
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_OFFS                                                (USB2_HSIC_USB_OTG_HS_BASE_REG_BASE_OFFS + 0x00000348)
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_RMSK                                                      0x7f
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_POR                                                 0x00000068
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_POR_RMSK                                            0xffffffff
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_IN          \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR, HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_RMSK)
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_INM(m)      \
        in_dword_masked(HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR, m)
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_OUT(v)      \
        out_dword(HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR,v)
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_ADDR,m,v,HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_IN)
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_VALUE_EN_BMSK                                     0x40
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_VALUE_EN_SHFT                                      0x6
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_RDATA_BMSK                                    0x30
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_RDATA_SHFT                                     0x4
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_WDATA_BMSK                                     0xc
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_WDATA_SHFT                                     0x2
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_NON_BUFF_ACCESS_BMSK                           0x3
#define HWIO_USB2_HSIC_USB_OTG_HS_QSB_VALUES_MEMTYPE_FOR_NON_BUFF_ACCESS_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                                (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_PHYS                                                           (CLK_CTL_BASE_PHYS + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_OFFS                                                           0x00000000

#define HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00029028)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_PHYS                                                         (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00029028)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_OFFS                                                         (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00029028)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_RMSK                                                         0x80000001
#define HWIO_GCC_QDSS_ETR_USB_CBCR_POR                                                          0x80000000
#define HWIO_GCC_QDSS_ETR_USB_CBCR_POR_RMSK                                                     0xffffffff
#define HWIO_GCC_QDSS_ETR_USB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR, HWIO_GCC_QDSS_ETR_USB_CBCR_RMSK)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR, m)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR,v)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QDSS_ETR_USB_CBCR_ADDR,m,v,HWIO_GCC_QDSS_ETR_USB_CBCR_IN)
#define HWIO_GCC_QDSS_ETR_USB_CBCR_CLK_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_QDSS_ETR_USB_CBCR_CLK_OFF_SHFT                                                       0x1f
#define HWIO_GCC_QDSS_ETR_USB_CBCR_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_QDSS_ETR_USB_CBCR_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_USB_HS_BCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00041000)
#define HWIO_GCC_USB_HS_BCR_PHYS                                                                (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041000)
#define HWIO_GCC_USB_HS_BCR_OFFS                                                                (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041000)
#define HWIO_GCC_USB_HS_BCR_RMSK                                                                       0x1
#define HWIO_GCC_USB_HS_BCR_POR                                                                 0x00000000
#define HWIO_GCC_USB_HS_BCR_POR_RMSK                                                            0xffffffff
#define HWIO_GCC_USB_HS_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_BCR_ADDR, HWIO_GCC_USB_HS_BCR_RMSK)
#define HWIO_GCC_USB_HS_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_BCR_ADDR, m)
#define HWIO_GCC_USB_HS_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_BCR_ADDR,v)
#define HWIO_GCC_USB_HS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_BCR_ADDR,m,v,HWIO_GCC_USB_HS_BCR_IN)
#define HWIO_GCC_USB_HS_BCR_BLK_ARES_BMSK                                                              0x1
#define HWIO_GCC_USB_HS_BCR_BLK_ARES_SHFT                                                              0x0

#define HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00041004)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_PHYS                                                        (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041004)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_OFFS                                                        (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041004)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_RMSK                                                        0x80007ff1
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_POR                                                         0x80004ff0
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_POR_RMSK                                                    0xffffffff
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CBCR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CBCR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_CORE_ON_BMSK                                          0x4000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_CORE_ON_SHFT                                             0xe
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                        0x2000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                           0xd
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                       0x1000
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                          0xc
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_WAKEUP_BMSK                                                      0xf00
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_WAKEUP_SHFT                                                        0x8
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_SLEEP_BMSK                                                        0xf0
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_SLEEP_SHFT                                                         0x4
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_USB_HS_SYSTEM_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_USB_HS_AHB_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00041008)
#define HWIO_GCC_USB_HS_AHB_CBCR_PHYS                                                           (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041008)
#define HWIO_GCC_USB_HS_AHB_CBCR_OFFS                                                           (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041008)
#define HWIO_GCC_USB_HS_AHB_CBCR_RMSK                                                           0xf000fff1
#define HWIO_GCC_USB_HS_AHB_CBCR_POR                                                            0x8000cff0
#define HWIO_GCC_USB_HS_AHB_CBCR_POR_RMSK                                                       0xffffffff
#define HWIO_GCC_USB_HS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, HWIO_GCC_USB_HS_AHB_CBCR_RMSK)
#define HWIO_GCC_USB_HS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_AHB_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_AHB_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_AHB_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_AHB_CBCR_IN)
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                  0x70000000
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                        0x1c
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                          0x8000
#define HWIO_GCC_USB_HS_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                             0xf
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_USB_HS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_USB_HS_AHB_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_USB_HS_AHB_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_USB_HS_AHB_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_USB_HS_AHB_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_USB_HS_AHB_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00041030)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_PHYS                                                   (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041030)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_OFFS                                                   (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041030)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_RMSK                                                   0xf0008001
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_POR                                                    0x80008000
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_POR_RMSK                                               0xffffffff
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR, HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_IN)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_CLK_OFF_BMSK                                           0x80000000
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_CLK_OFF_SHFT                                                 0x1f
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                          0x70000000
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                0x1c
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                  0x8000
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                     0xf
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004100c)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_PHYS                                             (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x0004100c)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_OFFS                                             (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0004100c)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_RMSK                                             0x80000001
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_POR                                              0x80000000
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_POR_RMSK                                         0xffffffff
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_RMSK)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR, m)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR,v)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_ADDR,m,v,HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_IN)
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_GCC_USB_HS_INACTIVITY_TIMERS_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00041010)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_PHYS                                                    (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041010)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_OFFS                                                    (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041010)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_RMSK                                                    0x80000013
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_POR                                                     0x80000000
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_POR_RMSK                                                0xffffffff
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_OFF_BMSK                                           0x80000000
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_OFF_SHFT                                                 0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                           0x10
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                            0x4
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_EN_BMSK                                                   0x2
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_ROOT_EN_SHFT                                                   0x1
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_UPDATE_BMSK                                                    0x1
#define HWIO_GCC_USB_HS_SYSTEM_CMD_RCGR_UPDATE_SHFT                                                    0x0

#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00041014)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_PHYS                                                    (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041014)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_OFFS                                                    (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041014)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_RMSK                                                         0x71f
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_POR                                                     0x00000000
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_POR_RMSK                                                0xffffffff
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR, HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_RMSK)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR, m)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR,v)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_ADDR,m,v,HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_IN)
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_SEL_BMSK                                                 0x700
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_SEL_SHFT                                                   0x8
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_DIV_BMSK                                                  0x1f
#define HWIO_GCC_USB_HS_SYSTEM_CFG_RCGR_SRC_DIV_SHFT                                                   0x0

#define HWIO_GCC_USB2A_PHY_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00041028)
#define HWIO_GCC_USB2A_PHY_BCR_PHYS                                                             (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041028)
#define HWIO_GCC_USB2A_PHY_BCR_OFFS                                                             (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041028)
#define HWIO_GCC_USB2A_PHY_BCR_RMSK                                                                    0x1
#define HWIO_GCC_USB2A_PHY_BCR_POR                                                              0x00000000
#define HWIO_GCC_USB2A_PHY_BCR_POR_RMSK                                                         0xffffffff
#define HWIO_GCC_USB2A_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB2A_PHY_BCR_ADDR, HWIO_GCC_USB2A_PHY_BCR_RMSK)
#define HWIO_GCC_USB2A_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2A_PHY_BCR_ADDR, m)
#define HWIO_GCC_USB2A_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2A_PHY_BCR_ADDR,v)
#define HWIO_GCC_USB2A_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2A_PHY_BCR_ADDR,m,v,HWIO_GCC_USB2A_PHY_BCR_IN)
#define HWIO_GCC_USB2A_PHY_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_USB2A_PHY_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00041034)
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_PHYS                                                      (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041034)
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_OFFS                                                      (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041034)
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_RMSK                                                             0x1
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_POR                                                       0x00000000
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_POR_RMSK                                                  0xffffffff
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB2_HS_PHY_ONLY_BCR_ADDR, HWIO_GCC_USB2_HS_PHY_ONLY_BCR_RMSK)
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2_HS_PHY_ONLY_BCR_ADDR, m)
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2_HS_PHY_ONLY_BCR_ADDR,v)
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2_HS_PHY_ONLY_BCR_ADDR,m,v,HWIO_GCC_USB2_HS_PHY_ONLY_BCR_IN)
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_BLK_ARES_BMSK                                                    0x1
#define HWIO_GCC_USB2_HS_PHY_ONLY_BCR_BLK_ARES_SHFT                                                    0x0

#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00041038)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_PHYS                                                    (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00041038)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_OFFS                                                    (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00041038)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_RMSK                                                           0x1
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_POR                                                     0x00000000
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_POR_RMSK                                                0xffffffff
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_IN          \
        in_dword_masked(HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_ADDR, HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_RMSK)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_ADDR, m)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_ADDR,v)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_ADDR,m,v,HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_IN)
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_BLK_ARES_BMSK                                                  0x1
#define HWIO_GCC_USB_HS_PHY_CFG_AHB_BCR_BLK_ARES_SHFT                                                  0x0

#define HWIO_GCC_QUSB2_PHY_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004103c)
#define HWIO_GCC_QUSB2_PHY_BCR_PHYS                                                             (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x0004103c)
#define HWIO_GCC_QUSB2_PHY_BCR_OFFS                                                             (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0004103c)
#define HWIO_GCC_QUSB2_PHY_BCR_RMSK                                                                    0x1
#define HWIO_GCC_QUSB2_PHY_BCR_POR                                                              0x00000000
#define HWIO_GCC_QUSB2_PHY_BCR_POR_RMSK                                                         0xffffffff
#define HWIO_GCC_QUSB2_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_QUSB2_PHY_BCR_ADDR, HWIO_GCC_QUSB2_PHY_BCR_RMSK)
#define HWIO_GCC_QUSB2_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_QUSB2_PHY_BCR_ADDR, m)
#define HWIO_GCC_QUSB2_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_QUSB2_PHY_BCR_ADDR,v)
#define HWIO_GCC_QUSB2_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_QUSB2_PHY_BCR_ADDR,m,v,HWIO_GCC_QUSB2_PHY_BCR_IN)
#define HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_QUSB2_PHY_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0004102c)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_PHYS                                                      (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x0004102c)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_OFFS                                                      (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x0004102c)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_RMSK                                                      0x80000001
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_POR                                                       0x80000000
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_POR_RMSK                                                  0xffffffff
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR, HWIO_GCC_USB2A_PHY_SLEEP_CBCR_RMSK)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB2A_PHY_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_USB2A_PHY_SLEEP_CBCR_IN)
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_USB2A_PHY_SLEEP_CBCR_CLK_ENABLE_SHFT                                                  0x0

#define HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00040000)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_PHYS                                                        (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00040000)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_OFFS                                                        (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00040000)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_RMSK                                                               0x1
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_POR                                                         0x00000000
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_POR_RMSK                                                    0xffffffff
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR, HWIO_GCC_USB_BOOT_CLOCK_CTL_RMSK)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR, m)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR,v)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_BOOT_CLOCK_CTL_ADDR,m,v,HWIO_GCC_USB_BOOT_CLOCK_CTL_IN)
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_USB_BOOT_CLOCK_CTL_CLK_ENABLE_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: USB2_PHY_CM_DWC_USB2_SW
 *--------------------------------------------------------------------------*/
#define USB2_PHY_CM_DWC_USB2_SW_BASE                                                        (0x0006C000)
#define USB2_PHY_CM_DWC_USB2_SW_REG_BASE                                                    (USB2_PHY_CM_DWC_USB2_SW_BASE      + 0x00000000)
#define USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_BASE_PHYS + 0x00000000)
#define USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS                                               0x00000000

#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000000)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000000)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000000)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_POR                                              0x00000000
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REVISION_ID0_ADDR, HWIO_USB2_PHY_USB_PHY_REVISION_ID0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REVISION_ID0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_STEP_7_0_BMSK                                          0xff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID0_STEP_7_0_SHFT                                           0x0

#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000004)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000004)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000004)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_POR                                              0x00000000
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REVISION_ID1_ADDR, HWIO_USB2_PHY_USB_PHY_REVISION_ID1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REVISION_ID1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_STEP_15_8_BMSK                                         0xff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID1_STEP_15_8_SHFT                                          0x0

#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000008)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000008)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000008)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_POR                                              0x00000002
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REVISION_ID2_ADDR, HWIO_USB2_PHY_USB_PHY_REVISION_ID2_RMSK)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REVISION_ID2_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_MINOR_7_0_BMSK                                         0xff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID2_MINOR_7_0_SHFT                                          0x0

#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000000c)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000000c)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000000c)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_POR                                              0x00000010
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REVISION_ID3_ADDR, HWIO_USB2_PHY_USB_PHY_REVISION_ID3_RMSK)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REVISION_ID3_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_MAJOR_BMSK                                             0xf0
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_MAJOR_SHFT                                              0x4
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_MINOR_11_8_BMSK                                         0xf
#define HWIO_USB2_PHY_USB_PHY_REVISION_ID3_MINOR_11_8_SHFT                                         0x0

#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_ADDR                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000010)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_PHYS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000010)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_OFFS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000010)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_RMSK                                                0xff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_POR                                           0x00000000
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_POR_RMSK                                      0xffffffff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_ADDR, HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_DEBUG_BUS_7_0_BMSK                                  0xff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT0_DEBUG_BUS_7_0_SHFT                                   0x0

#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_ADDR                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000014)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_PHYS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000014)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_OFFS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000014)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_RMSK                                                0xff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_POR                                           0x00000000
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_POR_RMSK                                      0xffffffff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_ADDR, HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_DEBUG_BUS_15_8_BMSK                                 0xff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT1_DEBUG_BUS_15_8_SHFT                                  0x0

#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_ADDR                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000018)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_PHYS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000018)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_OFFS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000018)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_RMSK                                                0xff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_POR                                           0x00000000
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_POR_RMSK                                      0xffffffff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_ADDR, HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_RMSK)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_DEBUG_BUS_23_16_BMSK                                0xff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT2_DEBUG_BUS_23_16_SHFT                                 0x0

#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_ADDR                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000001c)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_PHYS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000001c)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_OFFS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000001c)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_RMSK                                                0xff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_POR                                           0x00000000
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_POR_RMSK                                      0xffffffff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_ADDR, HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_RMSK)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_DEBUG_BUS_31_24_BMSK                                0xff
#define HWIO_USB2_PHY_USB_PHY_DEBUG_BUS_STAT3_DEBUG_BUS_31_24_SHFT                                 0x0

#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_ADDR                                       (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000020)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_PHYS                                       (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000020)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_OFFS                                       (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000020)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_RMSK                                             0xff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_POR                                        0x00000003
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_POR_RMSK                                   0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_ADDR, HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_RSVD_7_6_BMSK                                    0xc0
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_RSVD_7_6_SHFT                                     0x6
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_PHY_RX_TESTER_STATUS_BMSK                        0x3c
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_PHY_RX_TESTER_STATUS_SHFT                         0x2
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_RX_ALL_BAD_BMSK                                   0x2
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_RX_ALL_BAD_SHFT                                   0x1
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_RX_ALL_GOOD_BMSK                                  0x1
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_OUT_1_RX_ALL_GOOD_SHFT                                  0x0

#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000024)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000024)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000024)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_POR                                       0x00000000
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_ADDR, HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_RMSK)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_RSVD_7_6_BMSK                                   0xc0
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_RSVD_7_6_SHFT                                    0x6
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_VDATDETENB0_BMSK                                0x20
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_VDATDETENB0_SHFT                                 0x5
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_VDATSRCENB0_BMSK                                0x10
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_VDATSRCENB0_SHFT                                 0x4
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_RSVD_3_BMSK                                      0x8
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_RSVD_3_SHFT                                      0x3
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_CHRGSEL0_BMSK                                    0x4
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_CHRGSEL0_SHFT                                    0x2
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_DCDENB0_BMSK                                     0x2
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_DCDENB0_SHFT                                     0x1
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_RSVD_0_BMSK                                      0x1
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_OUTPUT_RSVD_0_SHFT                                      0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_ADDR                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000028)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_PHYS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000028)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_OFFS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000028)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_RMSK                                                0xfe
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_POR                                           0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_POR_RMSK                                      0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_RSVD_7_5_BMSK                                       0xe0
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_RSVD_7_5_SHFT                                        0x5
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_CHGDETINTLCH_BMSK                                   0x10
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_CHGDETINTLCH_SHFT                                    0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_DPINTLCH_BMSK                                        0x8
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_DPINTLCH_SHFT                                        0x3
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_DCDINTLCH_BMSK                                       0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_DCDINTLCH_SHFT                                       0x2
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_DMINTLCH_BMSK                                        0x2
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_IRQ_STAT_DMINTLCH_SHFT                                        0x1

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_ADDR                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000002c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_PHYS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000002c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_OFFS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000002c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_RMSK                                              0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_POR                                         0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_POR_RMSK                                    0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_RSVD_7_4_BMSK                                     0xf0
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_RSVD_7_4_SHFT                                      0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_TESTDATAOUT_BMSK                                   0xf
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST_OUT_1_TESTDATAOUT_SHFT                                   0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_ADDR                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000030)
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_PHYS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000030)
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_OFFS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000030)
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_RMSK                                               0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_POR                                          0x00000000
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_POR_RMSK                                     0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_RSVD_7_0_BMSK                                      0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_VEC4_STATUS_RSVD_7_0_SHFT                                       0x0

#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_ADDR                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000034)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_PHYS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000034)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_OFFS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000034)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_RMSK                                               0xff
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_POR                                          0x00000000
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_POR_RMSK                                     0xffffffff
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_RSVD_7_4_BMSK                                      0xf0
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_RSVD_7_4_SHFT                                       0x4
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_USB2_BC_IRQ_CI_BMSK                                 0x8
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_USB2_BC_IRQ_CI_SHFT                                 0x3
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_RSVD_2_0_BMSK                                       0x7
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC0_STATUS_RSVD_2_0_SHFT                                       0x0

#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_ADDR                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000038)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_PHYS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000038)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_OFFS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000038)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_RMSK                                               0xff
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_POR                                          0x00000000
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_POR_RMSK                                     0xffffffff
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_RSVD_7_0_BMSK                                      0xff
#define HWIO_USB2_PHY_USB_PHY_ULPI_VEC1_STATUS_RSVD_7_0_SHFT                                       0x0

#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_ADDR                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000003c)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_PHYS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000003c)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_OFFS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000003c)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_RMSK                                              0xff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_POR                                         0x00000000
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_POR_RMSK                                    0xffffffff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_ADDR, HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_BC_IRQ_CI_IRQ_STAT_BMSK                           0x80
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_BC_IRQ_CI_IRQ_STAT_SHFT                            0x7
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_DCDOUT_SET_DFT_IRQ_STAT_BMSK                      0x40
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_DCDOUT_SET_DFT_IRQ_STAT_SHFT                       0x6
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_DCDOUT_REG_IRQ_STAT_BMSK                          0x20
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_DCDOUT_REG_IRQ_STAT_SHFT                           0x5
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_FSVPLUS_TOG_IRQ_STAT_1_0_BMSK                     0x10
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_FSVPLUS_TOG_IRQ_STAT_1_0_SHFT                      0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_FSVPLUS_TOG_IRQ_STAT_0_1_BMSK                      0x8
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_FSVPLUS_TOG_IRQ_STAT_0_1_SHFT                      0x3
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_FSVMINUS_TOG_IRQ_STAT_1_0_BMSK                     0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_FSVMINUS_TOG_IRQ_STAT_1_0_SHFT                     0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_FSVMINUS_TOG_IRQ_STAT_0_1_BMSK                     0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_FSVMINUS_TOG_IRQ_STAT_0_1_SHFT                     0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_CHGDET_IRQ_STAT_BMSK                               0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_CHGDET_IRQ_STAT_SHFT                               0x0

#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_ADDR                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000040)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_PHYS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000040)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_OFFS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000040)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_RMSK                                              0xff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_POR                                         0x00000000
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_POR_RMSK                                    0xffffffff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_ADDR, HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_RSVD_7_4_BMSK                                     0xf0
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_RSVD_7_4_SHFT                                      0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_OTGSESSVLD_FALL_IRQ_STAT_BMSK                      0x8
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_OTGSESSVLD_FALL_IRQ_STAT_SHFT                      0x3
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_OTGSESSVLD_RISE_IRQ_STAT_BMSK                      0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_OTGSESSVLD_RISE_IRQ_STAT_SHFT                      0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_IDDIG_FALL_IRQ_STAT_BMSK                           0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_IDDIG_FALL_IRQ_STAT_SHFT                           0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_IDDIG_RISE_IRQ_STAT_BMSK                           0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS1_IDDIG_RISE_IRQ_STAT_SHFT                           0x0

#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_ADDR                                   (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000044)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_PHYS                                   (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000044)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_OFFS                                   (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000044)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_RMSK                                         0xff
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_POR                                    0x00000000
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_POR_RMSK                               0xffffffff
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_RSVD_7_0_BMSK                                0xff
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_ERR_CNT1_STATUS_RSVD_7_0_SHFT                                 0x0

#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_ADDR                                            (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000048)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_PHYS                                            (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000048)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_OFFS                                            (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000048)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_RMSK                                                  0xff
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_POR                                             0x00000000
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_POR_RMSK                                        0xffffffff
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_RSVD_7_0_BMSK                                         0xff
#define HWIO_USB2_PHY_USB_PHY_RX_CHK_STATUS_RSVD_7_0_SHFT                                          0x0

#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_ADDR                                 (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000004c)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_PHYS                                 (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000004c)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_OFFS                                 (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000004c)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_RMSK                                       0xff
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_POR                                  0x00000000
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_POR_RMSK                             0xffffffff
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_RSVD_7_1_BMSK                              0xfe
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_RSVD_7_1_SHFT                               0x1
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_REFCLK_RXTAP_VOUTP_MON_BMSK                 0x1
#define HWIO_USB2_PHY_USB_PHY_REFCLK_RXTAP_TEST_STATUS_REFCLK_RXTAP_VOUTP_MON_SHFT                 0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_ADDR                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000050)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_PHYS                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000050)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_OFFS                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000050)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_RMSK                                           0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_POR                                      0x00000000
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_POR_RMSK                                 0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_RX_DATA_BMSK                                   0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAL_STATUS_RX_DATA_SHFT                                    0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_ADDR                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000054)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_PHYS                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000054)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_OFFS                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000054)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_RMSK                                           0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_POR                                      0x00000000
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_POR_RMSK                                 0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_RX_DATA_BMSK                                   0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_DATAH_STATUS_RX_DATA_SHFT                                    0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000058)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000058)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000058)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_POR                                       0x00000000
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RSVD_7_BMSK                                     0x80
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RSVD_7_SHFT                                      0x7
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_LINESTATE_BMSK                                  0x60
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_LINESTATE_SHFT                                   0x5
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_TX_READY_BMSK                                   0x10
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_TX_READY_SHFT                                    0x4
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RX_ERROR_BMSK                                    0x8
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RX_ERROR_SHFT                                    0x3
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RX_ACTIVE_BMSK                                   0x4
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RX_ACTIVE_SHFT                                   0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RX_VALIDH_BMSK                                   0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RX_VALIDH_SHFT                                   0x1
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RX_VALIDL_BMSK                                   0x1
#define HWIO_USB2_PHY_USB_PHY_UTMI_RX_PORT_STATUS_RX_VALIDL_SHFT                                   0x0

#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_ADDR                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000005c)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_PHYS                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000005c)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_OFFS                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000005c)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_RMSK                                           0xff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_POR                                      0x00000000
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_POR_RMSK                                 0xffffffff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_ADDR, HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_RMSK)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IDDIG_1_0_BMSK                                 0x80
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IDDIG_1_0_SHFT                                  0x7
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_OTGSESSVLD_1_0_BMSK                            0x40
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_OTGSESSVLD_1_0_SHFT                             0x6
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_OTGSESSVLD_0_1_BMSK                            0x20
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_OTGSESSVLD_0_1_SHFT                             0x5
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_FSVPLUS_TOG_1_0_BMSK                           0x10
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_FSVPLUS_TOG_1_0_SHFT                            0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_FSVPLUS_TOG_0_1_BMSK                            0x8
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_FSVPLUS_TOG_0_1_SHFT                            0x3
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_FSVMINUS_TOG_1_0_BMSK                           0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_FSVMINUS_TOG_1_0_SHFT                           0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_FSVMINUS_TOG_0_1_BMSK                           0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_FSVMINUS_TOG_0_1_SHFT                           0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IDDIG_0_1_BMSK                                  0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_SRC_STATUS_IDDIG_0_1_SHFT                                  0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000060)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000060)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000060)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_RMSK                                                     0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_POR                                                0x00000002
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_IN)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_USB_HS_RX_TEST_EN_BMSK                                   0x80
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_USB_HS_RX_TEST_EN_SHFT                                    0x7
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_TERMSEL_BMSK                                             0x40
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_TERMSEL_SHFT                                              0x6
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OPMODE_BMSK                                              0x30
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_OPMODE_SHFT                                               0x4
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_RX_TESTER_RESET_BMSK                                      0x8
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_RX_TESTER_RESET_SHFT                                      0x3
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_PORTRESET_BMSK                                            0x4
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_PORTRESET_SHFT                                            0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_SLEEPM_BMSK                                               0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_SLEEPM_SHFT                                               0x1
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_RSVD_0_BMSK                                               0x1
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_RSVD_0_SHFT                                               0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000064)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000064)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000064)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_RMSK                                                     0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_POR                                                0x00000000
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_IN)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_RSVD_7_4_BMSK                                            0xf0
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_RSVD_7_4_SHFT                                             0x4
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_DMPULLDOWN_BMSK                                           0x8
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_DMPULLDOWN_SHFT                                           0x3
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_DPPULLDOWN_BMSK                                           0x4
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_DPPULLDOWN_SHFT                                           0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_XCVRSEL_BMSK                                              0x3
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL1_XCVRSEL_SHFT                                              0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000068)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000068)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000068)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_RMSK                                                     0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_POR                                                0x00000000
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_IN)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_DATAIN_7_0_BMSK                                          0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL2_DATAIN_7_0_SHFT                                           0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000006c)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000006c)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000006c)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_RMSK                                                     0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_POR                                                0x00000000
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_IN)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_DATAIN_15_8_BMSK                                         0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL3_DATAIN_15_8_SHFT                                          0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000070)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000070)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000070)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_RMSK                                                     0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_POR                                                0x00000000
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_IN)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_RSVD_7_3_BMSK                                            0xf8
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_RSVD_7_3_SHFT                                             0x3
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_IDPULLUP_BMSK                                             0x4
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_IDPULLUP_SHFT                                             0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_TXVALIDH_BMSK                                             0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_TXVALIDH_SHFT                                             0x1
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_TXVALID_BMSK                                              0x1
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL4_TXVALID_SHFT                                              0x0

#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000074)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000074)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000074)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_RMSK                                                     0xff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_POR                                                0x00000010
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR, HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_RMSK)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_IN)
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_RSVD_7_5_BMSK                                            0xe0
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_RSVD_7_5_SHFT                                             0x5
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_PICOPHY_UTMI_CONTROL_OVERWRITE_EN_BMSK                   0x10
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_PICOPHY_UTMI_CONTROL_OVERWRITE_EN_SHFT                    0x4
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_RSVD_3_2_BMSK                                             0xc
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_RSVD_3_2_SHFT                                             0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_POR_BMSK                                                  0x2
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_POR_SHFT                                                  0x1
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ATERESET_BMSK                                             0x1
#define HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ATERESET_SHFT                                             0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000078)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000078)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000078)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_POR                                       0x00000088
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_COMMONONN_BMSK                                  0x80
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_COMMONONN_SHFT                                   0x7
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_FSEL_BMSK                                       0x70
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_FSEL_SHFT                                        0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_RETENABLEN_BMSK                                  0x8
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_RETENABLEN_SHFT                                  0x3
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_BMSK                                       0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_SHFT                                       0x2
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_VATESTENB_BMSK                                   0x3
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_VATESTENB_SHFT                                   0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000007c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000007c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000007c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_POR                                       0x000000db
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_PLLBTUNE_BMSK                                   0x80
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_PLLBTUNE_SHFT                                    0x7
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_USE_CLKCORE_BMSK                                0x40
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_USE_CLKCORE_SHFT                                 0x6
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ACAENB0_BMSK                                    0x20
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ACAENB0_SHFT                                     0x5
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_VBUSVLDEXTSEL0_BMSK                             0x10
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_VBUSVLDEXTSEL0_SHFT                              0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_OTGDISABLE0_BMSK                                 0x8
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_OTGDISABLE0_SHFT                                 0x3
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_OTGTUNE0_BMSK                                    0x7
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_OTGTUNE0_SHFT                                    0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000080)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000080)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000080)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_POR                                       0x00000061
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_CLK_REF_DIV2_BMSK                               0x80
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_CLK_REF_DIV2_SHFT                                0x7
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_PLLPTUNE_BMSK                                   0x78
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_PLLPTUNE_SHFT                                    0x3
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_PLLITUNE_BMSK                                    0x6
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_PLLITUNE_SHFT                                    0x1
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_VREGBYPASS_BMSK                                  0x1
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_VREGBYPASS_SHFT                                  0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000084)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000084)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000084)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_POR                                       0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_RSVD_7_0_BMSK                                   0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON3_RSVD_7_0_SHFT                                    0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000088)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000088)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000088)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_POR                                              0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_RSVD_7_0_BMSK                                          0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL0_RSVD_7_0_SHFT                                           0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000008c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000008c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000008c)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_POR                                              0x00000023
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_RSVD_7_6_BMSK                                          0xc0
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_RSVD_7_6_SHFT                                           0x6
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_VBUSVLDEXT0_BMSK                                       0x20
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_VBUSVLDEXT0_SHFT                                        0x5
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_RSVD_4_3_BMSK                                          0x18
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_RSVD_4_3_SHFT                                           0x3
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ULPIPOR_BMSK                                            0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ULPIPOR_SHFT                                            0x2
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ID_HV_CLAMP_EN_N_BMSK                                   0x2
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ID_HV_CLAMP_EN_N_SHFT                                   0x1
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ULPI_CLK_EN_BMSK                                        0x1
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ULPI_CLK_EN_SHFT                                        0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000090)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000090)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000090)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_POR                                              0x00000040
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_USB2_SUSPEND_N_SEL_BMSK                                0x80
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_USB2_SUSPEND_N_SEL_SHFT                                 0x7
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_USB2_SUSPEND_N_BMSK                                    0x40
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_USB2_SUSPEND_N_SHFT                                     0x6
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_USB2_UTMI_CLK_EN_BMSK                                  0x20
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_USB2_UTMI_CLK_EN_SHFT                                   0x5
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_AUTORESUME_LEGACY_BMSK                                 0x10
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_AUTORESUME_LEGACY_SHFT                                  0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_AUTORESUME_BMSK                                         0x8
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_AUTORESUME_SHFT                                         0x3
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_RSVD_2_0_BMSK                                           0x7
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_RSVD_2_0_SHFT                                           0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000094)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000094)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000094)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_POR                                              0x00000004
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_RSVD_7_3_BMSK                                          0xf8
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_RSVD_7_3_SHFT                                           0x3
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK                           0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_CLAMP_MPM_DPSE_DMSE_EN_N_SHFT                           0x2
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_RSVD_1_BMSK                                             0x2
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_RSVD_1_SHFT                                             0x1
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_ANALOGTEST_EN_BMSK                                      0x1
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL3_ANALOGTEST_EN_SHFT                                      0x0

#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_ADDR                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000098)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_PHYS                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000098)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_OFFS                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000098)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_RMSK                                          0xff
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_POR                                     0x00000063
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_POR_RMSK                                0xffffffff
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_ADDR, HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_IN)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_SQRXTUNE0_BMSK                                0xe0
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_SQRXTUNE0_SHFT                                 0x5
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_RSVD_4_3_BMSK                                 0x18
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_RSVD_4_3_SHFT                                  0x3
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_COMPDISTUNE0_BMSK                              0x7
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_COMPDISTUNE0_SHFT                              0x0

#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_ADDR                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x0000009c)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_PHYS                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x0000009c)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_OFFS                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x0000009c)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_RMSK                                          0xff
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_POR                                     0x00000003
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_POR_RMSK                                0xffffffff
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_ADDR, HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_IN)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_TXPREEMPAMPTUNE0_BMSK                         0xc0
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_TXPREEMPAMPTUNE0_SHFT                          0x6
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_TXPREEMPPULSETUNE0_BMSK                       0x20
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_TXPREEMPPULSETUNE0_SHFT                        0x5
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_RSVD_4_BMSK                                   0x10
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_RSVD_4_SHFT                                    0x4
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_TXVREFTUNE0_BMSK                               0xf
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_TXVREFTUNE0_SHFT                               0x0

#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_ADDR                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000a0)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_PHYS                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000a0)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_OFFS                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000a0)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_RMSK                                          0xff
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_POR                                     0x0000001d
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_POR_RMSK                                0xffffffff
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_ADDR, HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_RMSK)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_IN)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_RSVD_7_6_BMSK                                 0xc0
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_RSVD_7_6_SHFT                                  0x6
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_TXRESTUNE0_BMSK                               0x30
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_TXRESTUNE0_SHFT                                0x4
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_TXHSXVTUNE0_BMSK                               0xc
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_TXHSXVTUNE0_SHFT                               0x2
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_TXRISETUNE0_BMSK                               0x3
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_TXRISETUNE0_SHFT                               0x0

#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_ADDR                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000a4)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_PHYS                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000a4)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_OFFS                                    (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000a4)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_RMSK                                          0xff
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_POR                                     0x00000003
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_POR_RMSK                                0xffffffff
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_ADDR, HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_RMSK)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_IN)
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_RSVD_7_4_BMSK                                 0xf0
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_RSVD_7_4_SHFT                                  0x4
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_TXFSLSTUNE0_BMSK                               0xf
#define HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_TXFSLSTUNE0_SHFT                               0x0

#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_ADDR                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000a8)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_PHYS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000a8)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_OFFS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000a8)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_RMSK                                              0xff
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_POR                                         0x00000040
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_POR_RMSK                                    0xffffffff
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_ADDR, HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_RMSK)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_IN)
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_VDATREFTUNE0_BMSK                                 0xc0
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_VDATREFTUNE0_SHFT                                  0x6
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_VDATDETENB0_BMSK                                  0x20
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_VDATDETENB0_SHFT                                   0x5
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_VDATSRCENB0_BMSK                                  0x10
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_VDATSRCENB0_SHFT                                   0x4
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_RSVD_3_BMSK                                        0x8
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_RSVD_3_SHFT                                        0x3
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_CHRGSEL0_BMSK                                      0x4
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_CHRGSEL0_SHFT                                      0x2
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_DCDENB0_BMSK                                       0x2
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_DCDENB0_SHFT                                       0x1
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_RSVD_0_BMSK                                        0x1
#define HWIO_USB2_PHY_USB_PHY_CHARGING_DET_CTRL_RSVD_0_SHFT                                        0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000ac)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000ac)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000ac)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_POR                                              0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_TESTDATAIN_BMSK                                        0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_TESTDATAIN_SHFT                                         0x0

#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000b0)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000b0)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000b0)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_POR                                              0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_ADDR, HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_TOGGLE_2WR_BMSK                                        0x80
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_TOGGLE_2WR_SHFT                                         0x7
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_USB_HSPW_CLK_480M_TEST_EN_BMSK                         0x40
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_USB_HSPW_CLK_480M_TEST_EN_SHFT                          0x6
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_TEST_WRITE_EN_BMSK                                     0x20
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_TEST_WRITE_EN_SHFT                                      0x5
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_TESTDATAOUTSEL_BMSK                                    0x10
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_TESTDATAOUTSEL_SHFT                                     0x4
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_TESTADDR_BMSK                                           0xf
#define HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_TESTADDR_SHFT                                           0x0

#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000b4)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000b4)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000b4)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_POR                                       0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_ADDR, HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_RX_PKT_VALUE_7_0_BMSK                           0xff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG0_RX_PKT_VALUE_7_0_SHFT                            0x0

#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000b8)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000b8)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000b8)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_POR                                       0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_ADDR, HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_RX_PKT_VALUE_15_8_BMSK                          0xff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG1_RX_PKT_VALUE_15_8_SHFT                           0x0

#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000bc)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000bc)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000bc)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_POR                                       0x00000000
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_ADDR, HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_RMSK)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_IN)
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_RX_PKT_VALUE_23_16_BMSK                         0xff
#define HWIO_USB2_PHY_USB_PHY_HS_RX_TESTER_1_REG3_RX_PKT_VALUE_23_16_SHFT                          0x0

#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_ADDR                                                (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000c0)
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_PHYS                                                (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000c0)
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_OFFS                                                (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000c0)
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_RMSK                                                       0x1
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_POR                                                 0x00000000
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_POR_RMSK                                            0xffffffff
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_ADDR, HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_RMSK)
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_IN)
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_SNPS_CTRL_USB2_PHY_BMSK                                    0x1
#define HWIO_USB2_PHY_USB_PHY_TCSR_CTRL_SNPS_CTRL_USB2_PHY_SHFT                                    0x0

#define HWIO_USB2_PHY_USB_PHY_CFG0_ADDR                                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000c4)
#define HWIO_USB2_PHY_USB_PHY_CFG0_PHYS                                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000c4)
#define HWIO_USB2_PHY_USB_PHY_CFG0_OFFS                                                     (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000c4)
#define HWIO_USB2_PHY_USB_PHY_CFG0_RMSK                                                           0xff
#define HWIO_USB2_PHY_USB_PHY_CFG0_POR                                                      0x00000001
#define HWIO_USB2_PHY_USB_PHY_CFG0_POR_RMSK                                                 0xffffffff
#define HWIO_USB2_PHY_USB_PHY_CFG0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_CFG0_ADDR, HWIO_USB2_PHY_USB_PHY_CFG0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_CFG0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_CFG0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_CFG0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_CFG0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_CFG0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_CFG0_IN)
#define HWIO_USB2_PHY_USB_PHY_CFG0_RSVD_7_3_BMSK                                                  0xf8
#define HWIO_USB2_PHY_USB_PHY_CFG0_RSVD_7_3_SHFT                                                   0x3
#define HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_CMN_CTRL_OVERRIDE_EN_BMSK                              0x4
#define HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_CMN_CTRL_OVERRIDE_EN_SHFT                              0x2
#define HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_DATAPATH_CTRL_OVERRIDE_EN_BMSK                         0x2
#define HWIO_USB2_PHY_USB_PHY_CFG0_UTMI_PHY_DATAPATH_CTRL_OVERRIDE_EN_SHFT                         0x1
#define HWIO_USB2_PHY_USB_PHY_CFG0_CTRL_VEC_IF_OVERRIDE_EN_BMSK                                    0x1
#define HWIO_USB2_PHY_USB_PHY_CFG0_CTRL_VEC_IF_OVERRIDE_EN_SHFT                                    0x0

#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000c8)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000c8)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000c8)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_RMSK                                                     0x7f
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_POR                                                0x00000000
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_IN          \
        in_dword_masked(HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_ADDR, HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_RMSK)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_ADDR, m)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_ADDR,v)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_ADDR,m,v,HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_IN)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_TESTBURNIN_BMSK                                          0x40
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_TESTBURNIN_SHFT                                           0x6
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_LOOPBACKENB_BMSK                                         0x20
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_LOOPBACKENB_SHFT                                          0x5
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSDPDATA_BMSK                                        0x10
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSDPDATA_SHFT                                         0x4
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSSEL_BMSK                                            0x8
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSSEL_SHFT                                            0x3
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSDMEN_BMSK                                           0x4
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSDMEN_SHFT                                           0x2
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSDPEN_BMSK                                           0x2
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSDPEN_SHFT                                           0x1
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSDMDATA_BMSK                                         0x1
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL0_BYPASSDMDATA_SHFT                                         0x0

#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000cc)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000cc)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000cc)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_RMSK                                                     0xff
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_POR                                                0x00000040
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_IN          \
        in_dword_masked(HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_ADDR, HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_RMSK)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_ADDR, m)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_ADDR,v)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_ADDR,m,v,HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_IN)
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_RSVD_7_BMSK                                              0x80
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_RSVD_7_SHFT                                               0x7
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_UTMI_TX_ENABLE_N_BMSK                                    0x40
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_UTMI_TX_ENABLE_N_SHFT                                     0x6
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_UTMI_TX_SE0_BMSK                                         0x20
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_UTMI_TX_SE0_SHFT                                          0x5
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_UTMI_TX_DAT_BMSK                                         0x10
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_UTMI_TX_DAT_SHFT                                          0x4
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_RSVD_3_BMSK                                               0x8
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_RSVD_3_SHFT                                               0x3
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_TESTCLK_SEL_BMSK                                          0x6
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_TESTCLK_SEL_SHFT                                          0x1
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_HSXCVREXTCTL0_BMSK                                        0x1
#define HWIO_USB2_PHY_UTMI_PHY_CMN_CTRL1_HSXCVREXTCTL0_SHFT                                        0x0

#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_ADDR                                                  (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000d0)
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_PHYS                                                  (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000d0)
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_OFFS                                                  (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000d0)
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_RMSK                                                        0xff
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_POR                                                   0x00000000
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_POR_RMSK                                              0xffffffff
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_IRQ_CMD_ADDR, HWIO_USB2_PHY_USB_PHY_IRQ_CMD_RMSK)
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_IRQ_CMD_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_IRQ_CMD_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_IRQ_CMD_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_IRQ_CMD_IN)
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_RSVD_7_1_BMSK                                               0xfe
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_RSVD_7_1_SHFT                                                0x1
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_IRQ_GLOBAL_CLEAR_BMSK                                        0x1
#define HWIO_USB2_PHY_USB_PHY_IRQ_CMD_IRQ_GLOBAL_CLEAR_SHFT                                        0x0

#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_ADDR                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000d4)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_PHYS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000d4)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_OFFS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000d4)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_RMSK                                                0xff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_POR                                           0x00000000
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_POR_RMSK                                      0xffffffff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_ADDR, HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_IN)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_BC_IRQ_CI_MASK_BMSK                                 0x80
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_BC_IRQ_CI_MASK_SHFT                                  0x7
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_DCDOUT_SET_DFT_MASK_BMSK                            0x40
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_DCDOUT_SET_DFT_MASK_SHFT                             0x6
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_DCDOUT_REG_MASK_BMSK                                0x20
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_DCDOUT_REG_MASK_SHFT                                 0x5
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_FSVPLUS_TOG_MASK_1_0_BMSK                           0x10
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_FSVPLUS_TOG_MASK_1_0_SHFT                            0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_FSVPLUS_TOG_MASK_0_1_BMSK                            0x8
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_FSVPLUS_TOG_MASK_0_1_SHFT                            0x3
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_FSVMINUS_TOG_MASK_1_0_BMSK                           0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_FSVMINUS_TOG_MASK_1_0_SHFT                           0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_FSVMINUS_TOG_MASK_0_1_BMSK                           0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_FSVMINUS_TOG_MASK_0_1_SHFT                           0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_CHGDET_MASK_BMSK                                     0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_CHGDET_MASK_SHFT                                     0x0

#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_ADDR                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000d8)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_PHYS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000d8)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_OFFS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000d8)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_RMSK                                                0xff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_POR                                           0x00000000
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_POR_RMSK                                      0xffffffff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_ADDR, HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_IN)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_RSVD_7_4_BMSK                                       0xf0
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_RSVD_7_4_SHFT                                        0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_OTGSESSVLD_FALL_MASK_BMSK                            0x8
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_OTGSESSVLD_FALL_MASK_SHFT                            0x3
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_OTGSESSVLD_RISE_MASK_BMSK                            0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_OTGSESSVLD_RISE_MASK_SHFT                            0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_IDDIG_FALL_MASK_BMSK                                 0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_IDDIG_FALL_MASK_SHFT                                 0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_IDDIG_RISE_MASK_BMSK                                 0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK1_IDDIG_RISE_MASK_SHFT                                 0x0

#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_ADDR                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000dc)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_PHYS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000dc)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_OFFS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000dc)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_RMSK                                               0xff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_POR                                          0x00000000
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_POR_RMSK                                     0xffffffff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_ADDR, HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_IN)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_BC_IRQ_CI_CLEAR_BMSK                               0x80
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_BC_IRQ_CI_CLEAR_SHFT                                0x7
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_DCDOUT_SET_DFT_CLEAR_BMSK                          0x40
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_DCDOUT_SET_DFT_CLEAR_SHFT                           0x6
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_DCDOUT_REG_CLEAR_BMSK                              0x20
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_DCDOUT_REG_CLEAR_SHFT                               0x5
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_FSVPLUS_TOG_CLEAR_1_0_BMSK                         0x10
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_FSVPLUS_TOG_CLEAR_1_0_SHFT                          0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_FSVPLUS_TOG_CLEAR_0_1_BMSK                          0x8
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_FSVPLUS_TOG_CLEAR_0_1_SHFT                          0x3
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_FSVMINUS_TOG_CLEAR_1_0_BMSK                         0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_FSVMINUS_TOG_CLEAR_1_0_SHFT                         0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_FSVMINUS_TOG_CLEAR_0_1_BMSK                         0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_FSVMINUS_TOG_CLEAR_0_1_SHFT                         0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_CHGDET_CLEAR_BMSK                                   0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_CHGDET_CLEAR_SHFT                                   0x0

#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_ADDR                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000e0)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_PHYS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000e0)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_OFFS                                         (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000e0)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_RMSK                                               0xff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_POR                                          0x00000000
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_POR_RMSK                                     0xffffffff
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_ADDR, HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_RMSK)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_IN)
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_RSVD_7_4_BMSK                                      0xf0
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_RSVD_7_4_SHFT                                       0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_OTGSESSVLD_FALL_CLEAR_BMSK                          0x8
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_OTGSESSVLD_FALL_CLEAR_SHFT                          0x3
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_OTGSESSVLD_RISE_CLEAR_BMSK                          0x4
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_OTGSESSVLD_RISE_CLEAR_SHFT                          0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_IDDIG_FALL_CLEAR_BMSK                               0x2
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_IDDIG_FALL_CLEAR_SHFT                               0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_IDDIG_RISE_CLEAR_BMSK                               0x1
#define HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR1_IDDIG_RISE_CLEAR_SHFT                               0x0

#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_ADDR                                              (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000e8)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_PHYS                                              (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000e8)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_OFFS                                              (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000e8)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_RMSK                                                    0xff
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_POR                                               0x0000000d
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_POR_RMSK                                          0xffffffff
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_ADDR, HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_RMSK)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_IN)
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_RSVD_7_4_BMSK                                           0xf0
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_RSVD_7_4_SHFT                                            0x4
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_REFCLK_SEL_BMSK                                          0xe
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_REFCLK_SEL_SHFT                                          0x1
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_REFCLK_RXTAP_EN_BMSK                                     0x1
#define HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_REFCLK_RXTAP_EN_SHFT                                     0x0

#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_ADDR                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000ec)
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_PHYS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000ec)
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_OFFS                                             (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000ec)
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_RMSK                                                   0xff
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_POR                                              0x00000001
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_POR_RMSK                                         0xffffffff
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_ADDR, HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_RMSK)
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_IN)
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_RSVD_7_1_BMSK                                          0xfe
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_RSVD_7_1_SHFT                                           0x1
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_PWRDN_B_BMSK                                            0x1
#define HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_PWRDN_B_SHFT                                            0x0

#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_ADDR                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000f0)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_PHYS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000f0)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_OFFS                                        (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000f0)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_RMSK                                              0xff
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_POR                                         0x00000000
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_POR_RMSK                                    0xffffffff
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_ADDR, HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_RMSK)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_IN)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_TXHSXVTUNE_BMSK                                   0x80
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_TXHSXVTUNE_SHFT                                    0x7
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_OTGTUNE_BMSK                                      0x40
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_OTGTUNE_SHFT                                       0x6
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_SQRXTUNE_BMSK                                     0x20
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_SQRXTUNE_SHFT                                      0x5
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_COMPDISTUNE_BMSK                                  0x10
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_COMPDISTUNE_SHFT                                   0x4
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_VBUSVLDEXTSEL_BMSK                                 0x8
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_VBUSVLDEXTSEL_SHFT                                 0x3
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_VBUSVLDEXT_BMSK                                    0x4
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_VBUSVLDEXT_SHFT                                    0x2
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_OTGDISABLE_BMSK                                    0x2
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_OTGDISABLE_SHFT                                    0x1
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_COMMONONN_BMSK                                     0x1
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_7_0_COMMONONN_SHFT                                     0x0

#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_ADDR                                       (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000f4)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_PHYS                                       (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000f4)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_OFFS                                       (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000f4)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_RMSK                                             0xff
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_POR                                        0x00000000
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_POR_RMSK                                   0xffffffff
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_ADDR, HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_RMSK)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_IN)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_ACAENB_CM_BMSK                                   0x80
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_ACAENB_CM_SHFT                                    0x7
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_CHRGSEL_BMSK                                     0x40
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_CHRGSEL_SHFT                                      0x6
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXPREEMPPULSETUNE_BMSK                           0x20
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXPREEMPPULSETUNE_SHFT                            0x5
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXPREEMPAMPTUNE_BMSK                             0x10
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXPREEMPAMPTUNE_SHFT                              0x4
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXRESTUNE_BMSK                                    0x8
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXRESTUNE_SHFT                                    0x3
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXRISETUNE_BMSK                                   0x4
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXRISETUNE_SHFT                                   0x2
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXVREFTUNE_BMSK                                   0x2
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXVREFTUNE_SHFT                                   0x1
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXFSLSTUNE_BMSK                                   0x1
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_15_8_TXFSLSTUNE_SHFT                                   0x0

#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000f8)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000f8)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000f8)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_POR                                       0x00000000
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_ADDR, HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_RMSK)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_IN)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_UTMI_TX_SE0_BMSK                                0x80
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_UTMI_TX_SE0_SHFT                                 0x7
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_UTMI_TX_DAT_BMSK                                0x40
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_UTMI_TX_DAT_SHFT                                 0x6
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_QSCRATCH_TESTCLK_BMSK                           0x20
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_QSCRATCH_TESTCLK_SHFT                            0x5
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_VATESTEN_CM_BMSK                                0x10
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_VATESTEN_CM_SHFT                                 0x4
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_TESTADDR_BMSK                                    0x8
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_TESTADDR_SHFT                                    0x3
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_TESTDATAIN_BMSK                                  0x4
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_TESTDATAIN_SHFT                                  0x2
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_TESTDATAOUTSEL_BMSK                              0x2
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_TESTDATAOUTSEL_SHFT                              0x1
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_DCDENB_BMSK                                      0x1
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_23_16_DCDENB_SHFT                                      0x0

#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_ADDR                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x000000fc)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_PHYS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x000000fc)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_OFFS                                      (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x000000fc)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_RMSK                                            0xff
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_POR                                       0x00000000
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_POR_RMSK                                  0xffffffff
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_ADDR, HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_RMSK)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_IN)
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_RSVD_7_2_BMSK                                   0xfc
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_RSVD_7_2_SHFT                                    0x2
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_AUTORESUME_BMSK                                  0x2
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_AUTORESUME_SHFT                                  0x1
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_UTMI_TX_ENABLE_N_BMSK                            0x1
#define HWIO_USB2_PHY_USB_PHY_ULPI_OVERRIDE_31_24_UTMI_TX_ENABLE_N_SHFT                            0x0

#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_ADDR                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000104)
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_PHYS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000104)
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_OFFS                                          (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000104)
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_RMSK                                                0xff
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_POR                                           0x00000000
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_POR_RMSK                                      0xffffffff
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_ADDR, HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_RMSK)
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_IN)
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_DEBUG_BUS_SEL_BMSK                                  0xe0
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_DEBUG_BUS_SEL_SHFT                                   0x5
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_DTESTMUX_SEL_BMSK                                   0x1f
#define HWIO_USB2_PHY_USB_PHY_TEST_DEBUG_CTRL_DTESTMUX_SEL_SHFT                                    0x0

#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_ADDR                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE      + 0x00000108)
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_PHYS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_PHYS + 0x00000108)
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_OFFS                                               (USB2_PHY_CM_DWC_USB2_SW_REG_BASE_OFFS + 0x00000108)
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_RMSK                                                     0xff
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_POR                                                0x00000000
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_POR_RMSK                                           0xffffffff
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_IN          \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_RESET_CTRL_ADDR, HWIO_USB2_PHY_USB_PHY_RESET_CTRL_RMSK)
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_INM(m)      \
        in_dword_masked(HWIO_USB2_PHY_USB_PHY_RESET_CTRL_ADDR, m)
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_OUT(v)      \
        out_dword(HWIO_USB2_PHY_USB_PHY_RESET_CTRL_ADDR,v)
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_USB2_PHY_USB_PHY_RESET_CTRL_ADDR,m,v,HWIO_USB2_PHY_USB_PHY_RESET_CTRL_IN)
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_RSVD_7_1_BMSK                                            0xfe
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_RSVD_7_1_SHFT                                             0x1
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_SW_RESET_BMSK                                             0x1
#define HWIO_USB2_PHY_USB_PHY_RESET_CTRL_SW_RESET_SHFT                                             0x0



#endif /* __QHSUSB_HWIO_DEPENDENCY_8909_H__ */
