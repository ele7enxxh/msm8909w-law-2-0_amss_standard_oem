#ifndef __HALMSSMSA_HWIO_H__
#define __HALMSSMSA_HWIO_H__
/*
===========================================================================
*/
/**
  @file HALmssMSA_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_PERPH

  'Include' filters applied: MSA[MSS_PERPH] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/hwio/9607/HALmssMSA_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                (MSS_TOP_BASE      + 0x00180000)
#define MSS_PERPH_REG_BASE_PHYS                                           (MSS_TOP_BASE_PHYS + 0x00180000)
#define MSS_PERPH_REG_BASE_OFFS                                           0x00180000

#define HWIO_MSS_MSA_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x00000058)
#define HWIO_MSS_MSA_PHYS                                                 (MSS_PERPH_REG_BASE_PHYS + 0x00000058)
#define HWIO_MSS_MSA_OFFS                                                 (MSS_PERPH_REG_BASE_OFFS + 0x00000058)
#define HWIO_MSS_MSA_RMSK                                                        0x7
#define HWIO_MSS_MSA_IN          \
        in_dword_masked(HWIO_MSS_MSA_ADDR, HWIO_MSS_MSA_RMSK)
#define HWIO_MSS_MSA_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_ADDR, m)
#define HWIO_MSS_MSA_OUT(v)      \
        out_dword(HWIO_MSS_MSA_ADDR,v)
#define HWIO_MSS_MSA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_ADDR,m,v,HWIO_MSS_MSA_IN)
#define HWIO_MSS_MSA_FORCE_Q6_MSA_BMSK                                           0x4
#define HWIO_MSS_MSA_FORCE_Q6_MSA_SHFT                                           0x2
#define HWIO_MSS_MSA_FORCE_Q6_MSA_FORCE_Q6_MSA_NO_FVAL                           0x0
#define HWIO_MSS_MSA_FORCE_Q6_MSA_FORCE_Q6_MSA_YES_FVAL                          0x1
#define HWIO_MSS_MSA_MBA_OK_BMSK                                                 0x2
#define HWIO_MSS_MSA_MBA_OK_SHFT                                                 0x1
#define HWIO_MSS_MSA_MBA_OK_MBA_OK_NO_FVAL                                       0x0
#define HWIO_MSS_MSA_MBA_OK_MBA_OK_YES_FVAL                                      0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_BMSK                                            0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_SHFT                                            0x0
#define HWIO_MSS_MSA_CONFIG_LOCK_CONFIG_NOT_LOCK_FVAL                            0x0
#define HWIO_MSS_MSA_CONFIG_LOCK_CONFIG_LOCK_FVAL                                0x1

#define HWIO_MSS_MSA_NC_HM_ADDR                                           (MSS_PERPH_REG_BASE      + 0x000000a0)
#define HWIO_MSS_MSA_NC_HM_PHYS                                           (MSS_PERPH_REG_BASE_PHYS + 0x000000a0)
#define HWIO_MSS_MSA_NC_HM_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x000000a0)
#define HWIO_MSS_MSA_NC_HM_RMSK                                           0xffffffff
#define HWIO_MSS_MSA_NC_HM_IN          \
        in_dword_masked(HWIO_MSS_MSA_NC_HM_ADDR, HWIO_MSS_MSA_NC_HM_RMSK)
#define HWIO_MSS_MSA_NC_HM_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_NC_HM_ADDR, m)
#define HWIO_MSS_MSA_NC_HM_OUT(v)      \
        out_dword(HWIO_MSS_MSA_NC_HM_ADDR,v)
#define HWIO_MSS_MSA_NC_HM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_NC_HM_ADDR,m,v,HWIO_MSS_MSA_NC_HM_IN)
#define HWIO_MSS_MSA_NC_HM_RESERVE_31_9_BMSK                              0xfffffe00
#define HWIO_MSS_MSA_NC_HM_RESERVE_31_9_SHFT                                     0x9
#define HWIO_MSS_MSA_NC_HM_NAV_BMSK                                            0x100
#define HWIO_MSS_MSA_NC_HM_NAV_SHFT                                              0x8
#define HWIO_MSS_MSA_NC_HM_RESERVE_7_4_BMSK                                     0xf0
#define HWIO_MSS_MSA_NC_HM_RESERVE_7_4_SHFT                                      0x4
#define HWIO_MSS_MSA_NC_HM_CRYPTO_BMSK                                           0xf
#define HWIO_MSS_MSA_NC_HM_CRYPTO_SHFT                                           0x0


#endif /* __HALMSSMSA_HWIO_H__ */
