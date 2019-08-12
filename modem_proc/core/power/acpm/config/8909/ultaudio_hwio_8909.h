#ifndef __ULTAUDIO_HWIO_H__
#define __ULTAUDIO_HWIO_H__
/*
===========================================================================
*/
/**
  @file ultaudio_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    LPASS_LPASS_CSR

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/acpm/config/8909/ultaudio_hwio_8909.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: LPASS_LPASS_CSR
 *--------------------------------------------------------------------------*/

#define LPASS_LPASS_CSR_REG_BASE                                                           (ULTAUDIO_CORE_BASE      + 0x00000100)
#define LPASS_LPASS_CSR_REG_BASE_OFFS                                                      0x00000100

#define HWIO_LPASS_CSR_GP_CTL_ADDR                                                         (LPASS_LPASS_CSR_REG_BASE      + 0x00000000)
#define HWIO_LPASS_CSR_GP_CTL_OFFS                                                         (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000000)
#define HWIO_LPASS_CSR_GP_CTL_RMSK                                                         0xffff0000
#define HWIO_LPASS_CSR_GP_CTL_POR                                                          0x00000000
#define HWIO_LPASS_CSR_GP_CTL_POR_RMSK                                                     0xffffffff
#define HWIO_LPASS_CSR_GP_CTL_ATTR                                                                0x3
#define HWIO_LPASS_CSR_GP_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_GP_CTL_ADDR, HWIO_LPASS_CSR_GP_CTL_RMSK)
#define HWIO_LPASS_CSR_GP_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_GP_CTL_ADDR, m)
#define HWIO_LPASS_CSR_GP_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_GP_CTL_ADDR,v)
#define HWIO_LPASS_CSR_GP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_GP_CTL_ADDR,m,v,HWIO_LPASS_CSR_GP_CTL_IN)
#define HWIO_LPASS_CSR_GP_CTL_SPARE_LPASS_BMSK                                             0xfffe0000
#define HWIO_LPASS_CSR_GP_CTL_SPARE_LPASS_SHFT                                                   0x11
#define HWIO_LPASS_CSR_GP_CTL_DRESET_EN_BMSK                                                  0x10000
#define HWIO_LPASS_CSR_GP_CTL_DRESET_EN_SHFT                                                     0x10

#define HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR                                                    (LPASS_LPASS_CSR_REG_BASE      + 0x00000004)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OFFS                                                    (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000004)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_RMSK                                                         0xf00
#define HWIO_LPASS_CSR_MEMTYPE_CTL_POR                                                     0x00000400
#define HWIO_LPASS_CSR_MEMTYPE_CTL_POR_RMSK                                                0xffffffff
#define HWIO_LPASS_CSR_MEMTYPE_CTL_ATTR                                                           0x3
#define HWIO_LPASS_CSR_MEMTYPE_CTL_IN          \
        in_dword_masked(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR, HWIO_LPASS_CSR_MEMTYPE_CTL_RMSK)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR, m)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR,v)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CSR_MEMTYPE_CTL_ADDR,m,v,HWIO_LPASS_CSR_MEMTYPE_CTL_IN)
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_BMSK                                           0xe00
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_SHFT                                             0x9
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_EN_BMSK                                        0x100
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_EN_SHFT                                          0x8
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_EN_DISABLE_FVAL                                  0x0
#define HWIO_LPASS_CSR_MEMTYPE_CTL_LPAIF_MEMTYPE_EN_ENABLE_FVAL                                   0x1

#define HWIO_LPASS_CSR_DEBUG_BUS_ADDR                                                      (LPASS_LPASS_CSR_REG_BASE      + 0x00000014)
#define HWIO_LPASS_CSR_DEBUG_BUS_OFFS                                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000014)
#define HWIO_LPASS_CSR_DEBUG_BUS_RMSK                                                      0xffffffff
#define HWIO_LPASS_CSR_DEBUG_BUS_POR                                                       0x00000000
#define HWIO_LPASS_CSR_DEBUG_BUS_POR_RMSK                                                  0xffffffff
#define HWIO_LPASS_CSR_DEBUG_BUS_ATTR                                                             0x1
#define HWIO_LPASS_CSR_DEBUG_BUS_IN          \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_BUS_ADDR, HWIO_LPASS_CSR_DEBUG_BUS_RMSK)
#define HWIO_LPASS_CSR_DEBUG_BUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_CSR_DEBUG_BUS_ADDR, m)
#define HWIO_LPASS_CSR_DEBUG_BUS_READ_BMSK                                                 0xffffffff
#define HWIO_LPASS_CSR_DEBUG_BUS_READ_SHFT                                                        0x0

#define HWIO_LPASS_AHBI_CFG_ADDR                                                           (LPASS_LPASS_CSR_REG_BASE      + 0x00000024)
#define HWIO_LPASS_AHBI_CFG_OFFS                                                           (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000024)
#define HWIO_LPASS_AHBI_CFG_RMSK                                                           0xfffffff1
#define HWIO_LPASS_AHBI_CFG_POR                                                            0x60512340
#define HWIO_LPASS_AHBI_CFG_POR_RMSK                                                       0xffffffff
#define HWIO_LPASS_AHBI_CFG_ATTR                                                                  0x3
#define HWIO_LPASS_AHBI_CFG_IN          \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_ADDR, HWIO_LPASS_AHBI_CFG_RMSK)
#define HWIO_LPASS_AHBI_CFG_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_ADDR, m)
#define HWIO_LPASS_AHBI_CFG_OUT(v)      \
        out_dword(HWIO_LPASS_AHBI_CFG_ADDR,v)
#define HWIO_LPASS_AHBI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBI_CFG_ADDR,m,v,HWIO_LPASS_AHBI_CFG_IN)
#define HWIO_LPASS_AHBI_CFG_RESAMPLER_PRIORITY_BMSK                                        0xf0000000
#define HWIO_LPASS_AHBI_CFG_RESAMPLER_PRIORITY_SHFT                                              0x1c
#define HWIO_LPASS_AHBI_CFG_SLIMBUS_PRIORITY_BMSK                                           0xf000000
#define HWIO_LPASS_AHBI_CFG_SLIMBUS_PRIORITY_SHFT                                                0x18
#define HWIO_LPASS_AHBI_CFG_MIDI_PRIORITY_BMSK                                               0xf00000
#define HWIO_LPASS_AHBI_CFG_MIDI_PRIORITY_SHFT                                                   0x14
#define HWIO_LPASS_AHBI_CFG_FABRIC_PRIORITY_BMSK                                              0xf0000
#define HWIO_LPASS_AHBI_CFG_FABRIC_PRIORITY_SHFT                                                 0x10
#define HWIO_LPASS_AHBI_CFG_DM_PRIORITY_BMSK                                                   0xf000
#define HWIO_LPASS_AHBI_CFG_DM_PRIORITY_SHFT                                                      0xc
#define HWIO_LPASS_AHBI_CFG_Q6SS_PRIORITY_BMSK                                                  0xf00
#define HWIO_LPASS_AHBI_CFG_Q6SS_PRIORITY_SHFT                                                    0x8
#define HWIO_LPASS_AHBI_CFG_AUDIO_IF_PRIORITY_BMSK                                               0xf0
#define HWIO_LPASS_AHBI_CFG_AUDIO_IF_PRIORITY_SHFT                                                0x4
#define HWIO_LPASS_AHBI_CFG_ROUND_ROBIN_EN_BMSK                                                   0x1
#define HWIO_LPASS_AHBI_CFG_ROUND_ROBIN_EN_SHFT                                                   0x0

#define HWIO_LPASS_AHBI_LOCK_CTL_ADDR                                                      (LPASS_LPASS_CSR_REG_BASE      + 0x00000028)
#define HWIO_LPASS_AHBI_LOCK_CTL_OFFS                                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000028)
#define HWIO_LPASS_AHBI_LOCK_CTL_RMSK                                                           0x3ff
#define HWIO_LPASS_AHBI_LOCK_CTL_POR                                                       0x00000000
#define HWIO_LPASS_AHBI_LOCK_CTL_POR_RMSK                                                  0xffffffff
#define HWIO_LPASS_AHBI_LOCK_CTL_ATTR                                                             0x3
#define HWIO_LPASS_AHBI_LOCK_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBI_LOCK_CTL_ADDR, HWIO_LPASS_AHBI_LOCK_CTL_RMSK)
#define HWIO_LPASS_AHBI_LOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBI_LOCK_CTL_ADDR, m)
#define HWIO_LPASS_AHBI_LOCK_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBI_LOCK_CTL_ADDR,v)
#define HWIO_LPASS_AHBI_LOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBI_LOCK_CTL_ADDR,m,v,HWIO_LPASS_AHBI_LOCK_CTL_IN)
#define HWIO_LPASS_AHBI_LOCK_CTL_SLIMBUS1_LOCK_EN_BMSK                                          0x200
#define HWIO_LPASS_AHBI_LOCK_CTL_SLIMBUS1_LOCK_EN_SHFT                                            0x9
#define HWIO_LPASS_AHBI_LOCK_CTL_HDMI_LOCK_EN_BMSK                                              0x100
#define HWIO_LPASS_AHBI_LOCK_CTL_HDMI_LOCK_EN_SHFT                                                0x8
#define HWIO_LPASS_AHBI_LOCK_CTL_SPDIFTX_LOCK_EN_BMSK                                            0x80
#define HWIO_LPASS_AHBI_LOCK_CTL_SPDIFTX_LOCK_EN_SHFT                                             0x7
#define HWIO_LPASS_AHBI_LOCK_CTL_RESAMPLER_LOCK_EN_BMSK                                          0x40
#define HWIO_LPASS_AHBI_LOCK_CTL_RESAMPLER_LOCK_EN_SHFT                                           0x6
#define HWIO_LPASS_AHBI_LOCK_CTL_SLIMBUS_LOCK_EN_BMSK                                            0x20
#define HWIO_LPASS_AHBI_LOCK_CTL_SLIMBUS_LOCK_EN_SHFT                                             0x5
#define HWIO_LPASS_AHBI_LOCK_CTL_MIDI_LOCK_EN_BMSK                                               0x10
#define HWIO_LPASS_AHBI_LOCK_CTL_MIDI_LOCK_EN_SHFT                                                0x4
#define HWIO_LPASS_AHBI_LOCK_CTL_FABRIC_LOCK_EN_BMSK                                              0x8
#define HWIO_LPASS_AHBI_LOCK_CTL_FABRIC_LOCK_EN_SHFT                                              0x3
#define HWIO_LPASS_AHBI_LOCK_CTL_DM_LOCK_EN_BMSK                                                  0x4
#define HWIO_LPASS_AHBI_LOCK_CTL_DM_LOCK_EN_SHFT                                                  0x2
#define HWIO_LPASS_AHBI_LOCK_CTL_Q6SS_LOCK_EN_BMSK                                                0x2
#define HWIO_LPASS_AHBI_LOCK_CTL_Q6SS_LOCK_EN_SHFT                                                0x1
#define HWIO_LPASS_AHBI_LOCK_CTL_AUDIO_IF_LOCK_EN_BMSK                                            0x1
#define HWIO_LPASS_AHBI_LOCK_CTL_AUDIO_IF_LOCK_EN_SHFT                                            0x0

#define HWIO_LPASS_AHBI_CFG_EXT_ADDR                                                       (LPASS_LPASS_CSR_REG_BASE      + 0x0000002c)
#define HWIO_LPASS_AHBI_CFG_EXT_OFFS                                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000002c)
#define HWIO_LPASS_AHBI_CFG_EXT_RMSK                                                            0xfff
#define HWIO_LPASS_AHBI_CFG_EXT_POR                                                        0x00000987
#define HWIO_LPASS_AHBI_CFG_EXT_POR_RMSK                                                   0xffffffff
#define HWIO_LPASS_AHBI_CFG_EXT_ATTR                                                              0x3
#define HWIO_LPASS_AHBI_CFG_EXT_IN          \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_EXT_ADDR, HWIO_LPASS_AHBI_CFG_EXT_RMSK)
#define HWIO_LPASS_AHBI_CFG_EXT_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBI_CFG_EXT_ADDR, m)
#define HWIO_LPASS_AHBI_CFG_EXT_OUT(v)      \
        out_dword(HWIO_LPASS_AHBI_CFG_EXT_ADDR,v)
#define HWIO_LPASS_AHBI_CFG_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBI_CFG_EXT_ADDR,m,v,HWIO_LPASS_AHBI_CFG_EXT_IN)
#define HWIO_LPASS_AHBI_CFG_EXT_SLIMBUS1_PRIORITY_BMSK                                          0xf00
#define HWIO_LPASS_AHBI_CFG_EXT_SLIMBUS1_PRIORITY_SHFT                                            0x8
#define HWIO_LPASS_AHBI_CFG_EXT_HDMI_PRIORITY_BMSK                                               0xf0
#define HWIO_LPASS_AHBI_CFG_EXT_HDMI_PRIORITY_SHFT                                                0x4
#define HWIO_LPASS_AHBI_CFG_EXT_SPDIFTX_PRIORITY_BMSK                                             0xf
#define HWIO_LPASS_AHBI_CFG_EXT_SPDIFTX_PRIORITY_SHFT                                             0x0

#define HWIO_LPASS_AHBX_CFG_ADDR                                                           (LPASS_LPASS_CSR_REG_BASE      + 0x00000040)
#define HWIO_LPASS_AHBX_CFG_OFFS                                                           (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000040)
#define HWIO_LPASS_AHBX_CFG_RMSK                                                           0x7773f77f
#define HWIO_LPASS_AHBX_CFG_POR                                                            0x4033962a
#define HWIO_LPASS_AHBX_CFG_POR_RMSK                                                       0xffffffff
#define HWIO_LPASS_AHBX_CFG_ATTR                                                                  0x3
#define HWIO_LPASS_AHBX_CFG_IN          \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_ADDR, HWIO_LPASS_AHBX_CFG_RMSK)
#define HWIO_LPASS_AHBX_CFG_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBX_CFG_ADDR, m)
#define HWIO_LPASS_AHBX_CFG_OUT(v)      \
        out_dword(HWIO_LPASS_AHBX_CFG_ADDR,v)
#define HWIO_LPASS_AHBX_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBX_CFG_ADDR,m,v,HWIO_LPASS_AHBX_CFG_IN)
#define HWIO_LPASS_AHBX_CFG_RESAMPLER_PRIORITY_BMSK                                        0x70000000
#define HWIO_LPASS_AHBX_CFG_RESAMPLER_PRIORITY_SHFT                                              0x1c
#define HWIO_LPASS_AHBX_CFG_SLIMBUS_PRIORITY_BMSK                                           0x7000000
#define HWIO_LPASS_AHBX_CFG_SLIMBUS_PRIORITY_SHFT                                                0x18
#define HWIO_LPASS_AHBX_CFG_MIDI_PRIORITY_BMSK                                               0x700000
#define HWIO_LPASS_AHBX_CFG_MIDI_PRIORITY_SHFT                                                   0x14
#define HWIO_LPASS_AHBX_CFG_SLIMBUS1_PRIORITY_BMSK                                            0x38000
#define HWIO_LPASS_AHBX_CFG_SLIMBUS1_PRIORITY_SHFT                                                0xf
#define HWIO_LPASS_AHBX_CFG_DM_PRIORITY_BMSK                                                   0x7000
#define HWIO_LPASS_AHBX_CFG_DM_PRIORITY_SHFT                                                      0xc
#define HWIO_LPASS_AHBX_CFG_HDMI_PRIORITY_BMSK                                                  0x700
#define HWIO_LPASS_AHBX_CFG_HDMI_PRIORITY_SHFT                                                    0x8
#define HWIO_LPASS_AHBX_CFG_AUDIO_IF_PRIORITY_BMSK                                               0x70
#define HWIO_LPASS_AHBX_CFG_AUDIO_IF_PRIORITY_SHFT                                                0x4
#define HWIO_LPASS_AHBX_CFG_SPDIFTX_PRIORITY_BMSK                                                 0xe
#define HWIO_LPASS_AHBX_CFG_SPDIFTX_PRIORITY_SHFT                                                 0x1
#define HWIO_LPASS_AHBX_CFG_ROUND_ROBIN_EN_BMSK                                                   0x1
#define HWIO_LPASS_AHBX_CFG_ROUND_ROBIN_EN_SHFT                                                   0x0

#define HWIO_LPASS_AHBX_LOCK_CTL_ADDR                                                      (LPASS_LPASS_CSR_REG_BASE      + 0x00000044)
#define HWIO_LPASS_AHBX_LOCK_CTL_OFFS                                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000044)
#define HWIO_LPASS_AHBX_LOCK_CTL_RMSK                                                           0x3f5
#define HWIO_LPASS_AHBX_LOCK_CTL_POR                                                       0x00000000
#define HWIO_LPASS_AHBX_LOCK_CTL_POR_RMSK                                                  0xffffffff
#define HWIO_LPASS_AHBX_LOCK_CTL_ATTR                                                             0x3
#define HWIO_LPASS_AHBX_LOCK_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBX_LOCK_CTL_ADDR, HWIO_LPASS_AHBX_LOCK_CTL_RMSK)
#define HWIO_LPASS_AHBX_LOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBX_LOCK_CTL_ADDR, m)
#define HWIO_LPASS_AHBX_LOCK_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBX_LOCK_CTL_ADDR,v)
#define HWIO_LPASS_AHBX_LOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBX_LOCK_CTL_ADDR,m,v,HWIO_LPASS_AHBX_LOCK_CTL_IN)
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS1_LOCK_EN_BMSK                                          0x200
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS1_LOCK_EN_SHFT                                            0x9
#define HWIO_LPASS_AHBX_LOCK_CTL_HDMI_LOCK_EN_BMSK                                              0x100
#define HWIO_LPASS_AHBX_LOCK_CTL_HDMI_LOCK_EN_SHFT                                                0x8
#define HWIO_LPASS_AHBX_LOCK_CTL_SPDIFTX_LOCK_EN_BMSK                                            0x80
#define HWIO_LPASS_AHBX_LOCK_CTL_SPDIFTX_LOCK_EN_SHFT                                             0x7
#define HWIO_LPASS_AHBX_LOCK_CTL_RESAMPLER_LOCK_EN_BMSK                                          0x40
#define HWIO_LPASS_AHBX_LOCK_CTL_RESAMPLER_LOCK_EN_SHFT                                           0x6
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS_LOCK_EN_BMSK                                            0x20
#define HWIO_LPASS_AHBX_LOCK_CTL_SLIMBUS_LOCK_EN_SHFT                                             0x5
#define HWIO_LPASS_AHBX_LOCK_CTL_MIDI_LOCK_EN_BMSK                                               0x10
#define HWIO_LPASS_AHBX_LOCK_CTL_MIDI_LOCK_EN_SHFT                                                0x4
#define HWIO_LPASS_AHBX_LOCK_CTL_DM_LOCK_EN_BMSK                                                  0x4
#define HWIO_LPASS_AHBX_LOCK_CTL_DM_LOCK_EN_SHFT                                                  0x2
#define HWIO_LPASS_AHBX_LOCK_CTL_AUDIO_IF_LOCK_EN_BMSK                                            0x1
#define HWIO_LPASS_AHBX_LOCK_CTL_AUDIO_IF_LOCK_EN_SHFT                                            0x0

#define HWIO_LPASS_AHBIX_SSR_CTL_ADDR                                                      (LPASS_LPASS_CSR_REG_BASE      + 0x00000048)
#define HWIO_LPASS_AHBIX_SSR_CTL_OFFS                                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000048)
#define HWIO_LPASS_AHBIX_SSR_CTL_RMSK                                                             0x1
#define HWIO_LPASS_AHBIX_SSR_CTL_POR                                                       0x00000000
#define HWIO_LPASS_AHBIX_SSR_CTL_POR_RMSK                                                  0xffffffff
#define HWIO_LPASS_AHBIX_SSR_CTL_ATTR                                                             0x3
#define HWIO_LPASS_AHBIX_SSR_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AHBIX_SSR_CTL_ADDR, HWIO_LPASS_AHBIX_SSR_CTL_RMSK)
#define HWIO_LPASS_AHBIX_SSR_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBIX_SSR_CTL_ADDR, m)
#define HWIO_LPASS_AHBIX_SSR_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AHBIX_SSR_CTL_ADDR,v)
#define HWIO_LPASS_AHBIX_SSR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBIX_SSR_CTL_ADDR,m,v,HWIO_LPASS_AHBIX_SSR_CTL_IN)
#define HWIO_LPASS_AHBIX_SSR_CTL_RESET_DISABLE_BMSK                                               0x1
#define HWIO_LPASS_AHBIX_SSR_CTL_RESET_DISABLE_SHFT                                               0x0

#define HWIO_LPASS_AHBIX_STATUS_ADDR                                                       (LPASS_LPASS_CSR_REG_BASE      + 0x00000050)
#define HWIO_LPASS_AHBIX_STATUS_OFFS                                                       (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000050)
#define HWIO_LPASS_AHBIX_STATUS_RMSK                                                             0xff
#define HWIO_LPASS_AHBIX_STATUS_POR                                                        0x00000000
#define HWIO_LPASS_AHBIX_STATUS_POR_RMSK                                                   0xffffffff
#define HWIO_LPASS_AHBIX_STATUS_ATTR                                                              0x1
#define HWIO_LPASS_AHBIX_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_AHBIX_STATUS_ADDR, HWIO_LPASS_AHBIX_STATUS_RMSK)
#define HWIO_LPASS_AHBIX_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBIX_STATUS_ADDR, m)
#define HWIO_LPASS_AHBIX_STATUS_Q6TCM_WRITE_ERROR_BMSK                                           0x80
#define HWIO_LPASS_AHBIX_STATUS_Q6TCM_WRITE_ERROR_SHFT                                            0x7
#define HWIO_LPASS_AHBIX_STATUS_Q6TCM_READ_ERROR_BMSK                                            0x40
#define HWIO_LPASS_AHBIX_STATUS_Q6TCM_READ_ERROR_SHFT                                             0x6
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_BOUNDARY_ERROR_BMSK                                  0x20
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_BOUNDARY_ERROR_SHFT                                   0x5
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_ERROR_BMSK                                           0x10
#define HWIO_LPASS_AHBIX_STATUS_AHBX_DECODE_ERROR_SHFT                                            0x4
#define HWIO_LPASS_AHBIX_STATUS_AHBX_MISALIGNED_BMSK                                              0x8
#define HWIO_LPASS_AHBIX_STATUS_AHBX_MISALIGNED_SHFT                                              0x3
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_BOUNDARY_ERROR_BMSK                                   0x4
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_BOUNDARY_ERROR_SHFT                                   0x2
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_ERROR_BMSK                                            0x2
#define HWIO_LPASS_AHBIX_STATUS_AHBI_DECODE_ERROR_SHFT                                            0x1
#define HWIO_LPASS_AHBIX_STATUS_AHBI_MISALIGNED_BMSK                                              0x1
#define HWIO_LPASS_AHBIX_STATUS_AHBI_MISALIGNED_SHFT                                              0x0

#define HWIO_LPASS_AHBIX_ACK_ADDR                                                          (LPASS_LPASS_CSR_REG_BASE      + 0x00000054)
#define HWIO_LPASS_AHBIX_ACK_OFFS                                                          (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000054)
#define HWIO_LPASS_AHBIX_ACK_RMSK                                                                0xff
#define HWIO_LPASS_AHBIX_ACK_POR                                                           0x00000000
#define HWIO_LPASS_AHBIX_ACK_POR_RMSK                                                      0xffffffff
#define HWIO_LPASS_AHBIX_ACK_ATTR                                                                 0x2
#define HWIO_LPASS_AHBIX_ACK_OUT(v)      \
        out_dword(HWIO_LPASS_AHBIX_ACK_ADDR,v)
#define HWIO_LPASS_AHBIX_ACK_Q6TCM_WRITE_ERROR_BMSK                                              0x80
#define HWIO_LPASS_AHBIX_ACK_Q6TCM_WRITE_ERROR_SHFT                                               0x7
#define HWIO_LPASS_AHBIX_ACK_Q6TCM_READ_ERROR_BMSK                                               0x40
#define HWIO_LPASS_AHBIX_ACK_Q6TCM_READ_ERROR_SHFT                                                0x6
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_BOUNDARY_ERROR_BMSK                                     0x20
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_BOUNDARY_ERROR_SHFT                                      0x5
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_ERROR_BMSK                                              0x10
#define HWIO_LPASS_AHBIX_ACK_AHBX_DECODE_ERROR_SHFT                                               0x4
#define HWIO_LPASS_AHBIX_ACK_AHBX_MISALIGNED_BMSK                                                 0x8
#define HWIO_LPASS_AHBIX_ACK_AHBX_MISALIGNED_SHFT                                                 0x3
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_BOUNDARY_ERROR_BMSK                                      0x4
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_BOUNDARY_ERROR_SHFT                                      0x2
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_ERROR_BMSK                                               0x2
#define HWIO_LPASS_AHBIX_ACK_AHBI_DECODE_ERROR_SHFT                                               0x1
#define HWIO_LPASS_AHBIX_ACK_AHBI_MISALIGNED_BMSK                                                 0x1
#define HWIO_LPASS_AHBIX_ACK_AHBI_MISALIGNED_SHFT                                                 0x0

#define HWIO_LPASS_AHBIX_EN_ADDR                                                           (LPASS_LPASS_CSR_REG_BASE      + 0x00000058)
#define HWIO_LPASS_AHBIX_EN_OFFS                                                           (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000058)
#define HWIO_LPASS_AHBIX_EN_RMSK                                                                 0xff
#define HWIO_LPASS_AHBIX_EN_POR                                                            0x00000012
#define HWIO_LPASS_AHBIX_EN_POR_RMSK                                                       0xffffffff
#define HWIO_LPASS_AHBIX_EN_ATTR                                                                  0x3
#define HWIO_LPASS_AHBIX_EN_IN          \
        in_dword_masked(HWIO_LPASS_AHBIX_EN_ADDR, HWIO_LPASS_AHBIX_EN_RMSK)
#define HWIO_LPASS_AHBIX_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_AHBIX_EN_ADDR, m)
#define HWIO_LPASS_AHBIX_EN_OUT(v)      \
        out_dword(HWIO_LPASS_AHBIX_EN_ADDR,v)
#define HWIO_LPASS_AHBIX_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AHBIX_EN_ADDR,m,v,HWIO_LPASS_AHBIX_EN_IN)
#define HWIO_LPASS_AHBIX_EN_Q6TCM_WRITE_ERROR_INT_EN_BMSK                                        0x80
#define HWIO_LPASS_AHBIX_EN_Q6TCM_WRITE_ERROR_INT_EN_SHFT                                         0x7
#define HWIO_LPASS_AHBIX_EN_Q6TCM_READ_ERROR_INT_EN_BMSK                                         0x40
#define HWIO_LPASS_AHBIX_EN_Q6TCM_READ_ERROR_INT_EN_SHFT                                          0x6
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_BOUNDARY_ERROR_INT_EN_BMSK                               0x20
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_BOUNDARY_ERROR_INT_EN_SHFT                                0x5
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_ERROR_INT_EN_BMSK                                        0x10
#define HWIO_LPASS_AHBIX_EN_AHBX_DECODE_ERROR_INT_EN_SHFT                                         0x4
#define HWIO_LPASS_AHBIX_EN_AHBX_MISALIGNED_INT_EN_BMSK                                           0x8
#define HWIO_LPASS_AHBIX_EN_AHBX_MISALIGNED_INT_EN_SHFT                                           0x3
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_BOUNDARY_ERROR_INT_EN_BMSK                                0x4
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_BOUNDARY_ERROR_INT_EN_SHFT                                0x2
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_ERROR_INT_EN_BMSK                                         0x2
#define HWIO_LPASS_AHBIX_EN_AHBI_DECODE_ERROR_INT_EN_SHFT                                         0x1
#define HWIO_LPASS_AHBIX_EN_AHBI_MISALIGNED_INT_EN_BMSK                                           0x1
#define HWIO_LPASS_AHBIX_EN_AHBI_MISALIGNED_INT_EN_SHFT                                           0x0

#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_ADDR                                                (LPASS_LPASS_CSR_REG_BASE      + 0x0000005c)
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_OFFS                                                (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000005c)
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_POR                                                 0x00000002
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_POR_RMSK                                            0xffffffff
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_ATTR                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPM_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPM_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPM_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPM_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPM_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_LPM_CGCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_ADDR                                            (LPASS_LPASS_CSR_REG_BASE      + 0x00000060)
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_OFFS                                            (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000060)
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_RMSK                                                   0x3
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_POR                                             0x00000002
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_POR_RMSK                                        0xffffffff
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_ATTR                                                   0x3
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_HW_CTL_BMSK                                            0x2
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_HW_CTL_SHFT                                            0x1
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_ADDR                                             (LPASS_LPASS_CSR_REG_BASE      + 0x00000064)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_OFFS                                             (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000064)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_RMSK                                                    0x3
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_POR                                              0x00000002
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_POR_RMSK                                         0xffffffff
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_ATTR                                                    0x3
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_HW_CTL_BMSK                                             0x2
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_HW_CTL_SHFT                                             0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_ADDR                                      (LPASS_LPASS_CSR_REG_BASE      + 0x0000006c)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_OFFS                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000006c)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_RMSK                                             0x3
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_POR                                       0x00000001
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_POR_RMSK                                  0xffffffff
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_ATTR                                             0x3
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_HW_CTL_BMSK                                      0x2
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_HW_CTL_SHFT                                      0x1
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_CLK_ENABLE_BMSK                                  0x1
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_CODEC_CGCR_CLK_ENABLE_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_ADDR                                          (LPASS_LPASS_CSR_REG_BASE      + 0x00000070)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_OFFS                                          (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000070)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_RMSK                                                 0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_POR                                           0x00000002
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_POR_RMSK                                      0xffffffff
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_ATTR                                                 0x3
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_HW_CTL_BMSK                                          0x2
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_HW_CTL_SHFT                                          0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_CLK_ENABLE_BMSK                                      0x1
#define HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_CLK_ENABLE_SHFT                                      0x0

#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_ADDR                                                (LPASS_LPASS_CSR_REG_BASE      + 0x00000074)
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_OFFS                                                (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000074)
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_RMSK                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_POR                                                 0x00000001
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_POR_RMSK                                            0xffffffff
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_ATTR                                                       0x3
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_VFR_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_VFR_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_VFR_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_VFR_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_VFR_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_VFR_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_HW_CTL_BMSK                                                0x2
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_HW_CTL_SHFT                                                0x1
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_LPASS_AUDIO_CORE_VFR_CGCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_ADDR                                  (LPASS_LPASS_CSR_REG_BASE      + 0x00000078)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_OFFS                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000078)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_RMSK                                         0x3
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_POR                                   0x00000002
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_POR_RMSK                              0xffffffff
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_ATTR                                         0x3
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_HW_CTL_BMSK                                  0x2
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_HW_CTL_SHFT                                  0x1
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_CLK_ENABLE_BMSK                              0x1
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_CLK_ENABLE_SHFT                              0x0

#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_ADDR                                  (LPASS_LPASS_CSR_REG_BASE      + 0x0000007c)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_OFFS                                  (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000007c)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_RMSK                                         0x3
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_POR                                   0x00000001
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_POR_RMSK                              0xffffffff
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_ATTR                                         0x3
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_HW_CTL_BMSK                                  0x2
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_HW_CTL_SHFT                                  0x1
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_CLK_ENABLE_BMSK                              0x1
#define HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_CLK_ENABLE_SHFT                              0x0

#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_ADDR                                      (LPASS_LPASS_CSR_REG_BASE      + 0x00000080)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_OFFS                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000080)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_RMSK                                             0x3
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_POR                                       0x00000001
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_POR_RMSK                                  0xffffffff
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_ATTR                                             0x3
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_HW_CTL_BMSK                                      0x2
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_HW_CTL_SHFT                                      0x1
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_CLK_ENABLE_BMSK                                  0x1
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_CLK_ENABLE_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_ADDR                                      (LPASS_LPASS_CSR_REG_BASE      + 0x00000084)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_OFFS                                      (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000084)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_RMSK                                             0x3
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_POR                                       0x00000001
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_POR_RMSK                                  0xffffffff
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_ATTR                                             0x3
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_HW_CTL_BMSK                                      0x2
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_HW_CTL_SHFT                                      0x1
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_CLK_ENABLE_BMSK                                  0x1
#define HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_CLK_ENABLE_SHFT                                  0x0

#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_ADDR                                        (LPASS_LPASS_CSR_REG_BASE      + 0x00000088)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_OFFS                                        (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000088)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_RMSK                                               0x3
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_POR                                         0x00000001
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_POR_RMSK                                    0xffffffff
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_ATTR                                               0x3
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_HW_CTL_BMSK                                        0x2
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_HW_CTL_SHFT                                        0x1
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_SLV_CGCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_ADDR                                        (LPASS_LPASS_CSR_REG_BASE      + 0x0000008c)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_OFFS                                        (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000008c)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_RMSK                                               0x3
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_POR                                         0x00000001
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_POR_RMSK                                    0xffffffff
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_ATTR                                               0x3
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_ADDR, HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_RMSK)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_IN)
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_HW_CTL_BMSK                                        0x2
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_HW_CTL_SHFT                                        0x1
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_LPASS_AUDIO_CORE_SLIMBUS_MST_CGCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_ADDR                                        (LPASS_LPASS_CSR_REG_BASE      + 0x00000090)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_OFFS                                        (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000090)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_RMSK                                               0x1
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_POR                                         0x00000000
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_POR_RMSK                                    0xffffffff
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_ATTR                                               0x1
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_ADDR, HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_RMSK)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_ADDR, m)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_LPASS_CSR_RIF_ADDR_INVALID_BMSK                    0x1
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_STATUS_LPASS_CSR_RIF_ADDR_INVALID_SHFT                    0x0

#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_ADDR                                           (LPASS_LPASS_CSR_REG_BASE      + 0x00000094)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_OFFS                                           (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000094)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_RMSK                                                  0x1
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_POR                                            0x00000000
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_POR_RMSK                                       0xffffffff
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_ATTR                                                  0x2
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_ADDR,v)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_LPASS_CSR_RIF_ADDR_INVALID_BMSK                       0x1
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_ACK_LPASS_CSR_RIF_ADDR_INVALID_SHFT                       0x0

#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_ADDR                                            (LPASS_LPASS_CSR_REG_BASE      + 0x00000098)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_OFFS                                            (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x00000098)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_RMSK                                                   0x1
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_POR                                             0x00000000
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_POR_RMSK                                        0xffffffff
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_ATTR                                                   0x3
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_IN          \
        in_dword_masked(HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_ADDR, HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_RMSK)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_ADDR, m)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_OUT(v)      \
        out_dword(HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_ADDR,v)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_ADDR,m,v,HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_IN)
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_LPASS_CSR_RIF_ADDR_INVALID_INT_EN_BMSK                 0x1
#define HWIO_LPASS_LPASS_CSR_ADDR_VALID_EN_LPASS_CSR_RIF_ADDR_INVALID_INT_EN_SHFT                 0x0

#define HWIO_LPASS_LPM_CTL_ADDR                                                            (LPASS_LPASS_CSR_REG_BASE      + 0x0000009c)
#define HWIO_LPASS_LPM_CTL_OFFS                                                            (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x0000009c)
#define HWIO_LPASS_LPM_CTL_RMSK                                                            0x80000000
#define HWIO_LPASS_LPM_CTL_POR                                                             0x00000000
#define HWIO_LPASS_LPM_CTL_POR_RMSK                                                        0xffffffff
#define HWIO_LPASS_LPM_CTL_ATTR                                                                   0x3
#define HWIO_LPASS_LPM_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPM_CTL_ADDR, HWIO_LPASS_LPM_CTL_RMSK)
#define HWIO_LPASS_LPM_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPM_CTL_ADDR, m)
#define HWIO_LPASS_LPM_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPM_CTL_ADDR,v)
#define HWIO_LPASS_LPM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPM_CTL_ADDR,m,v,HWIO_LPASS_LPM_CTL_IN)
#define HWIO_LPASS_LPM_CTL_SOFT_RESET_BMSK                                                 0x80000000
#define HWIO_LPASS_LPM_CTL_SOFT_RESET_SHFT                                                       0x1f
#define HWIO_LPASS_LPM_CTL_SOFT_RESET_DISABLE_FVAL                                                0x0
#define HWIO_LPASS_LPM_CTL_SOFT_RESET_ENABLE_FVAL                                                 0x1

#define HWIO_LPASS_LPAIF_CTL_ADDR                                                          (LPASS_LPASS_CSR_REG_BASE      + 0x000000b0)
#define HWIO_LPASS_LPAIF_CTL_OFFS                                                          (LPASS_LPASS_CSR_REG_BASE_OFFS + 0x000000b0)
#define HWIO_LPASS_LPAIF_CTL_RMSK                                                          0x80000000
#define HWIO_LPASS_LPAIF_CTL_POR                                                           0x00000000
#define HWIO_LPASS_LPAIF_CTL_POR_RMSK                                                      0xffffffff
#define HWIO_LPASS_LPAIF_CTL_ATTR                                                                 0x3
#define HWIO_LPASS_LPAIF_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_CTL_ADDR, HWIO_LPASS_LPAIF_CTL_RMSK)
#define HWIO_LPASS_LPAIF_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_CTL_ADDR, m)
#define HWIO_LPASS_LPAIF_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_CTL_ADDR,v)
#define HWIO_LPASS_LPAIF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_CTL_ADDR,m,v,HWIO_LPASS_LPAIF_CTL_IN)
#define HWIO_LPASS_LPAIF_CTL_SOFT_RESET_BMSK                                               0x80000000
#define HWIO_LPASS_LPAIF_CTL_SOFT_RESET_SHFT                                                     0x1f
#define HWIO_LPASS_LPAIF_CTL_SOFT_RESET_DISABLE_FVAL                                              0x0
#define HWIO_LPASS_LPAIF_CTL_SOFT_RESET_ENABLE_FVAL                                               0x1


#endif /* __ULTAUDIO_HWIO_H__ */