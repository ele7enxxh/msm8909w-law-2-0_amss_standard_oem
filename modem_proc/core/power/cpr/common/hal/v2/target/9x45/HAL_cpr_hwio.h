#ifndef __HAL_CPR_HWIO_H__
#define __HAL_CPR_HWIO_H__
/*
===========================================================================
*/
/**
  @file HAL_cpr_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9x45 (Tesla) [tesla_v1.0_p3q3r19.6_MTO]
 
  This file contains HWIO register definitions for the following modules:
    RBCPR_WRAPPER

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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/cpr/common/hal/v2/target/9x45/HAL_cpr_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RBCPR_WRAPPER
 *--------------------------------------------------------------------------*/

#define RBCPR_WRAPPER_REG_BASE                                          (RBCPR_WRAPPER_BASE      + 0x00000000)
#define RBCPR_WRAPPER_REG_BASE_OFFS                                     0x00000000

#define HWIO_RBCPR_VERSION_ADDR(x)                                      ((x) + 0x00000000)
#define HWIO_RBCPR_VERSION_OFFS                                         (0x00000000)
#define HWIO_RBCPR_VERSION_RMSK                                         0xffffffff
#define HWIO_RBCPR_VERSION_IN(x)      \
        in_dword_masked(HWIO_RBCPR_VERSION_ADDR(x), HWIO_RBCPR_VERSION_RMSK)
#define HWIO_RBCPR_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_VERSION_ADDR(x), m)
#define HWIO_RBCPR_VERSION_RBCPR_VERSION_BMSK                           0xffffffff
#define HWIO_RBCPR_VERSION_RBCPR_VERSION_SHFT                                  0x0

#define HWIO_RBCPR_SENSOR_MASK0_ADDR(x)                                 ((x) + 0x00000020)
#define HWIO_RBCPR_SENSOR_MASK0_OFFS                                    (0x00000020)
#define HWIO_RBCPR_SENSOR_MASK0_RMSK                                    0xffffffff
#define HWIO_RBCPR_SENSOR_MASK0_IN(x)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_MASK0_ADDR(x), HWIO_RBCPR_SENSOR_MASK0_RMSK)
#define HWIO_RBCPR_SENSOR_MASK0_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_MASK0_ADDR(x), m)
#define HWIO_RBCPR_SENSOR_MASK0_OUT(x, v)      \
        out_dword(HWIO_RBCPR_SENSOR_MASK0_ADDR(x),v)
#define HWIO_RBCPR_SENSOR_MASK0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_SENSOR_MASK0_ADDR(x),m,v,HWIO_RBCPR_SENSOR_MASK0_IN(x))
#define HWIO_RBCPR_SENSOR_MASK0_SENSOR_MASK_BMSK                        0xffffffff
#define HWIO_RBCPR_SENSOR_MASK0_SENSOR_MASK_SHFT                               0x0

#define HWIO_RBCPR_SENSOR_MASK1_ADDR(x)                                 ((x) + 0x00000024)
#define HWIO_RBCPR_SENSOR_MASK1_OFFS                                    (0x00000024)
#define HWIO_RBCPR_SENSOR_MASK1_RMSK                                    0xffffffff
#define HWIO_RBCPR_SENSOR_MASK1_IN(x)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_MASK1_ADDR(x), HWIO_RBCPR_SENSOR_MASK1_RMSK)
#define HWIO_RBCPR_SENSOR_MASK1_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_MASK1_ADDR(x), m)
#define HWIO_RBCPR_SENSOR_MASK1_OUT(x, v)      \
        out_dword(HWIO_RBCPR_SENSOR_MASK1_ADDR(x),v)
#define HWIO_RBCPR_SENSOR_MASK1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_SENSOR_MASK1_ADDR(x),m,v,HWIO_RBCPR_SENSOR_MASK1_IN(x))
#define HWIO_RBCPR_SENSOR_MASK1_SENSOR_MASK_BMSK                        0xffffffff
#define HWIO_RBCPR_SENSOR_MASK1_SENSOR_MASK_SHFT                               0x0

#define HWIO_RBCPR_SENSOR_MASK2_ADDR(x)                                 ((x) + 0x00000028)
#define HWIO_RBCPR_SENSOR_MASK2_OFFS                                    (0x00000028)
#define HWIO_RBCPR_SENSOR_MASK2_RMSK                                    0xffffffff
#define HWIO_RBCPR_SENSOR_MASK2_IN(x)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_MASK2_ADDR(x), HWIO_RBCPR_SENSOR_MASK2_RMSK)
#define HWIO_RBCPR_SENSOR_MASK2_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_MASK2_ADDR(x), m)
#define HWIO_RBCPR_SENSOR_MASK2_OUT(x, v)      \
        out_dword(HWIO_RBCPR_SENSOR_MASK2_ADDR(x),v)
#define HWIO_RBCPR_SENSOR_MASK2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_SENSOR_MASK2_ADDR(x),m,v,HWIO_RBCPR_SENSOR_MASK2_IN(x))
#define HWIO_RBCPR_SENSOR_MASK2_SENSOR_MASK_BMSK                        0xffffffff
#define HWIO_RBCPR_SENSOR_MASK2_SENSOR_MASK_SHFT                               0x0

#define HWIO_RBCPR_SENSOR_MASK3_ADDR(x)                                 ((x) + 0x0000002c)
#define HWIO_RBCPR_SENSOR_MASK3_OFFS                                    (0x0000002c)
#define HWIO_RBCPR_SENSOR_MASK3_RMSK                                    0xffffffff
#define HWIO_RBCPR_SENSOR_MASK3_IN(x)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_MASK3_ADDR(x), HWIO_RBCPR_SENSOR_MASK3_RMSK)
#define HWIO_RBCPR_SENSOR_MASK3_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_MASK3_ADDR(x), m)
#define HWIO_RBCPR_SENSOR_MASK3_OUT(x, v)      \
        out_dword(HWIO_RBCPR_SENSOR_MASK3_ADDR(x),v)
#define HWIO_RBCPR_SENSOR_MASK3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_SENSOR_MASK3_ADDR(x),m,v,HWIO_RBCPR_SENSOR_MASK3_IN(x))
#define HWIO_RBCPR_SENSOR_MASK3_SENSOR_MASK_BMSK                        0xffffffff
#define HWIO_RBCPR_SENSOR_MASK3_SENSOR_MASK_SHFT                               0x0

#define HWIO_RBCPR_SENSOR_BYPASS0_ADDR(x)                               ((x) + 0x00000030)
#define HWIO_RBCPR_SENSOR_BYPASS0_OFFS                                  (0x00000030)
#define HWIO_RBCPR_SENSOR_BYPASS0_RMSK                                  0xffffffff
#define HWIO_RBCPR_SENSOR_BYPASS0_IN(x)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_BYPASS0_ADDR(x), HWIO_RBCPR_SENSOR_BYPASS0_RMSK)
#define HWIO_RBCPR_SENSOR_BYPASS0_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_BYPASS0_ADDR(x), m)
#define HWIO_RBCPR_SENSOR_BYPASS0_OUT(x, v)      \
        out_dword(HWIO_RBCPR_SENSOR_BYPASS0_ADDR(x),v)
#define HWIO_RBCPR_SENSOR_BYPASS0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_SENSOR_BYPASS0_ADDR(x),m,v,HWIO_RBCPR_SENSOR_BYPASS0_IN(x))
#define HWIO_RBCPR_SENSOR_BYPASS0_SENSOR_BYPASS_BMSK                    0xffffffff
#define HWIO_RBCPR_SENSOR_BYPASS0_SENSOR_BYPASS_SHFT                           0x0

#define HWIO_RBCPR_SENSOR_BYPASS1_ADDR(x)                               ((x) + 0x00000034)
#define HWIO_RBCPR_SENSOR_BYPASS1_OFFS                                  (0x00000034)
#define HWIO_RBCPR_SENSOR_BYPASS1_RMSK                                  0xffffffff
#define HWIO_RBCPR_SENSOR_BYPASS1_IN(x)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_BYPASS1_ADDR(x), HWIO_RBCPR_SENSOR_BYPASS1_RMSK)
#define HWIO_RBCPR_SENSOR_BYPASS1_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_BYPASS1_ADDR(x), m)
#define HWIO_RBCPR_SENSOR_BYPASS1_OUT(x, v)      \
        out_dword(HWIO_RBCPR_SENSOR_BYPASS1_ADDR(x),v)
#define HWIO_RBCPR_SENSOR_BYPASS1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_SENSOR_BYPASS1_ADDR(x),m,v,HWIO_RBCPR_SENSOR_BYPASS1_IN(x))
#define HWIO_RBCPR_SENSOR_BYPASS1_SENSOR_BYPASS_BMSK                    0xffffffff
#define HWIO_RBCPR_SENSOR_BYPASS1_SENSOR_BYPASS_SHFT                           0x0

#define HWIO_RBCPR_SENSOR_BYPASS2_ADDR(x)                               ((x) + 0x00000038)
#define HWIO_RBCPR_SENSOR_BYPASS2_OFFS                                  (0x00000038)
#define HWIO_RBCPR_SENSOR_BYPASS2_RMSK                                  0xffffffff
#define HWIO_RBCPR_SENSOR_BYPASS2_IN(x)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_BYPASS2_ADDR(x), HWIO_RBCPR_SENSOR_BYPASS2_RMSK)
#define HWIO_RBCPR_SENSOR_BYPASS2_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_BYPASS2_ADDR(x), m)
#define HWIO_RBCPR_SENSOR_BYPASS2_OUT(x, v)      \
        out_dword(HWIO_RBCPR_SENSOR_BYPASS2_ADDR(x),v)
#define HWIO_RBCPR_SENSOR_BYPASS2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_SENSOR_BYPASS2_ADDR(x),m,v,HWIO_RBCPR_SENSOR_BYPASS2_IN(x))
#define HWIO_RBCPR_SENSOR_BYPASS2_SENSOR_BYPASS_BMSK                    0xffffffff
#define HWIO_RBCPR_SENSOR_BYPASS2_SENSOR_BYPASS_SHFT                           0x0

#define HWIO_RBCPR_SENSOR_BYPASS3_ADDR(x)                               ((x) + 0x0000003c)
#define HWIO_RBCPR_SENSOR_BYPASS3_OFFS                                  (0x0000003c)
#define HWIO_RBCPR_SENSOR_BYPASS3_RMSK                                  0xffffffff
#define HWIO_RBCPR_SENSOR_BYPASS3_IN(x)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_BYPASS3_ADDR(x), HWIO_RBCPR_SENSOR_BYPASS3_RMSK)
#define HWIO_RBCPR_SENSOR_BYPASS3_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_SENSOR_BYPASS3_ADDR(x), m)
#define HWIO_RBCPR_SENSOR_BYPASS3_OUT(x, v)      \
        out_dword(HWIO_RBCPR_SENSOR_BYPASS3_ADDR(x),v)
#define HWIO_RBCPR_SENSOR_BYPASS3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_SENSOR_BYPASS3_ADDR(x),m,v,HWIO_RBCPR_SENSOR_BYPASS3_IN(x))
#define HWIO_RBCPR_SENSOR_BYPASS3_SENSOR_BYPASS_BMSK                    0xffffffff
#define HWIO_RBCPR_SENSOR_BYPASS3_SENSOR_BYPASS_SHFT                           0x0

#define HWIO_RBIF_PMIC_ADDR_ADDR(x)                                     ((x) + 0x00000040)
#define HWIO_RBIF_PMIC_ADDR_OFFS                                        (0x00000040)
#define HWIO_RBIF_PMIC_ADDR_RMSK                                          0xc03c03
#define HWIO_RBIF_PMIC_ADDR_IN(x)      \
        in_dword_masked(HWIO_RBIF_PMIC_ADDR_ADDR(x), HWIO_RBIF_PMIC_ADDR_RMSK)
#define HWIO_RBIF_PMIC_ADDR_INM(x, m)      \
        in_dword_masked(HWIO_RBIF_PMIC_ADDR_ADDR(x), m)
#define HWIO_RBIF_PMIC_ADDR_OUT(x, v)      \
        out_dword(HWIO_RBIF_PMIC_ADDR_ADDR(x),v)
#define HWIO_RBIF_PMIC_ADDR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBIF_PMIC_ADDR_ADDR(x),m,v,HWIO_RBIF_PMIC_ADDR_IN(x))
#define HWIO_RBIF_PMIC_ADDR_RBIF_PMIC_MX_DATA_MSB_BMSK                    0xc00000
#define HWIO_RBIF_PMIC_ADDR_RBIF_PMIC_MX_DATA_MSB_SHFT                        0x16
#define HWIO_RBIF_PMIC_ADDR_RBIF_PMIC_MX_ADDR_BMSK                          0x3000
#define HWIO_RBIF_PMIC_ADDR_RBIF_PMIC_MX_ADDR_SHFT                             0xc
#define HWIO_RBIF_PMIC_ADDR_RBIF_PMIC_CX_DATA_MSB_BMSK                       0xc00
#define HWIO_RBIF_PMIC_ADDR_RBIF_PMIC_CX_DATA_MSB_SHFT                         0xa
#define HWIO_RBIF_PMIC_ADDR_RBIF_PMIC_CX_ADDR_BMSK                             0x3
#define HWIO_RBIF_PMIC_ADDR_RBIF_PMIC_CX_ADDR_SHFT                             0x0

#define HWIO_RBCPR_TIMER_INTERVAL_ADDR(x)                               ((x) + 0x00000044)
#define HWIO_RBCPR_TIMER_INTERVAL_OFFS                                  (0x00000044)
#define HWIO_RBCPR_TIMER_INTERVAL_RMSK                                  0xffffffff
#define HWIO_RBCPR_TIMER_INTERVAL_IN(x)      \
        in_dword_masked(HWIO_RBCPR_TIMER_INTERVAL_ADDR(x), HWIO_RBCPR_TIMER_INTERVAL_RMSK)
#define HWIO_RBCPR_TIMER_INTERVAL_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_TIMER_INTERVAL_ADDR(x), m)
#define HWIO_RBCPR_TIMER_INTERVAL_OUT(x, v)      \
        out_dword(HWIO_RBCPR_TIMER_INTERVAL_ADDR(x),v)
#define HWIO_RBCPR_TIMER_INTERVAL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_TIMER_INTERVAL_ADDR(x),m,v,HWIO_RBCPR_TIMER_INTERVAL_IN(x))
#define HWIO_RBCPR_TIMER_INTERVAL_INTERVAL_BMSK                         0xffffffff
#define HWIO_RBCPR_TIMER_INTERVAL_INTERVAL_SHFT                                0x0

#define HWIO_RBIF_LIMIT_ADDR(x)                                         ((x) + 0x00000048)
#define HWIO_RBIF_LIMIT_OFFS                                            (0x00000048)
#define HWIO_RBIF_LIMIT_RMSK                                                 0xfff
#define HWIO_RBIF_LIMIT_IN(x)      \
        in_dword_masked(HWIO_RBIF_LIMIT_ADDR(x), HWIO_RBIF_LIMIT_RMSK)
#define HWIO_RBIF_LIMIT_INM(x, m)      \
        in_dword_masked(HWIO_RBIF_LIMIT_ADDR(x), m)
#define HWIO_RBIF_LIMIT_OUT(x, v)      \
        out_dword(HWIO_RBIF_LIMIT_ADDR(x),v)
#define HWIO_RBIF_LIMIT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBIF_LIMIT_ADDR(x),m,v,HWIO_RBIF_LIMIT_IN(x))
#define HWIO_RBIF_LIMIT_CEILING_BMSK                                         0xfc0
#define HWIO_RBIF_LIMIT_CEILING_SHFT                                           0x6
#define HWIO_RBIF_LIMIT_FLOOR_BMSK                                            0x3f
#define HWIO_RBIF_LIMIT_FLOOR_SHFT                                             0x0

#define HWIO_RBIF_TIMER_ADJUST_ADDR(x)                                  ((x) + 0x0000004c)
#define HWIO_RBIF_TIMER_ADJUST_OFFS                                     (0x0000004c)
#define HWIO_RBIF_TIMER_ADJUST_RMSK                                        0x1ffff
#define HWIO_RBIF_TIMER_ADJUST_IN(x)      \
        in_dword_masked(HWIO_RBIF_TIMER_ADJUST_ADDR(x), HWIO_RBIF_TIMER_ADJUST_RMSK)
#define HWIO_RBIF_TIMER_ADJUST_INM(x, m)      \
        in_dword_masked(HWIO_RBIF_TIMER_ADJUST_ADDR(x), m)
#define HWIO_RBIF_TIMER_ADJUST_OUT(x, v)      \
        out_dword(HWIO_RBIF_TIMER_ADJUST_ADDR(x),v)
#define HWIO_RBIF_TIMER_ADJUST_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBIF_TIMER_ADJUST_ADDR(x),m,v,HWIO_RBIF_TIMER_ADJUST_IN(x))
#define HWIO_RBIF_TIMER_ADJUST_AUTO_LOOP_DISABLE_BMSK                      0x10000
#define HWIO_RBIF_TIMER_ADJUST_AUTO_LOOP_DISABLE_SHFT                         0x10
#define HWIO_RBIF_TIMER_ADJUST_CLAMP_TIMER_INTERVAL_BMSK                    0xff00
#define HWIO_RBIF_TIMER_ADJUST_CLAMP_TIMER_INTERVAL_SHFT                       0x8
#define HWIO_RBIF_TIMER_ADJUST_CONSECUTIVE_DN_BMSK                            0xf0
#define HWIO_RBIF_TIMER_ADJUST_CONSECUTIVE_DN_SHFT                             0x4
#define HWIO_RBIF_TIMER_ADJUST_CONSECUTIVE_UP_BMSK                             0xf
#define HWIO_RBIF_TIMER_ADJUST_CONSECUTIVE_UP_SHFT                             0x0

#define HWIO_RBCPR_GCNT_TARGET0_ADDR(x)                                 ((x) + 0x00000060)
#define HWIO_RBCPR_GCNT_TARGET0_OFFS                                    (0x00000060)
#define HWIO_RBCPR_GCNT_TARGET0_RMSK                                      0x3fffff
#define HWIO_RBCPR_GCNT_TARGET0_IN(x)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET0_ADDR(x), HWIO_RBCPR_GCNT_TARGET0_RMSK)
#define HWIO_RBCPR_GCNT_TARGET0_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET0_ADDR(x), m)
#define HWIO_RBCPR_GCNT_TARGET0_OUT(x, v)      \
        out_dword(HWIO_RBCPR_GCNT_TARGET0_ADDR(x),v)
#define HWIO_RBCPR_GCNT_TARGET0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_GCNT_TARGET0_ADDR(x),m,v,HWIO_RBCPR_GCNT_TARGET0_IN(x))
#define HWIO_RBCPR_GCNT_TARGET0_GCNT0_BMSK                                0x3ff000
#define HWIO_RBCPR_GCNT_TARGET0_GCNT0_SHFT                                     0xc
#define HWIO_RBCPR_GCNT_TARGET0_TARGET0_BMSK                                 0xfff
#define HWIO_RBCPR_GCNT_TARGET0_TARGET0_SHFT                                   0x0

#define HWIO_RBCPR_GCNT_TARGET1_ADDR(x)                                 ((x) + 0x00000064)
#define HWIO_RBCPR_GCNT_TARGET1_OFFS                                    (0x00000064)
#define HWIO_RBCPR_GCNT_TARGET1_RMSK                                      0x3fffff
#define HWIO_RBCPR_GCNT_TARGET1_IN(x)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET1_ADDR(x), HWIO_RBCPR_GCNT_TARGET1_RMSK)
#define HWIO_RBCPR_GCNT_TARGET1_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET1_ADDR(x), m)
#define HWIO_RBCPR_GCNT_TARGET1_OUT(x, v)      \
        out_dword(HWIO_RBCPR_GCNT_TARGET1_ADDR(x),v)
#define HWIO_RBCPR_GCNT_TARGET1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_GCNT_TARGET1_ADDR(x),m,v,HWIO_RBCPR_GCNT_TARGET1_IN(x))
#define HWIO_RBCPR_GCNT_TARGET1_GCNT1_BMSK                                0x3ff000
#define HWIO_RBCPR_GCNT_TARGET1_GCNT1_SHFT                                     0xc
#define HWIO_RBCPR_GCNT_TARGET1_TARGET1_BMSK                                 0xfff
#define HWIO_RBCPR_GCNT_TARGET1_TARGET1_SHFT                                   0x0

#define HWIO_RBCPR_GCNT_TARGET2_ADDR(x)                                 ((x) + 0x00000068)
#define HWIO_RBCPR_GCNT_TARGET2_OFFS                                    (0x00000068)
#define HWIO_RBCPR_GCNT_TARGET2_RMSK                                      0x3fffff
#define HWIO_RBCPR_GCNT_TARGET2_IN(x)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET2_ADDR(x), HWIO_RBCPR_GCNT_TARGET2_RMSK)
#define HWIO_RBCPR_GCNT_TARGET2_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET2_ADDR(x), m)
#define HWIO_RBCPR_GCNT_TARGET2_OUT(x, v)      \
        out_dword(HWIO_RBCPR_GCNT_TARGET2_ADDR(x),v)
#define HWIO_RBCPR_GCNT_TARGET2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_GCNT_TARGET2_ADDR(x),m,v,HWIO_RBCPR_GCNT_TARGET2_IN(x))
#define HWIO_RBCPR_GCNT_TARGET2_GCNT2_BMSK                                0x3ff000
#define HWIO_RBCPR_GCNT_TARGET2_GCNT2_SHFT                                     0xc
#define HWIO_RBCPR_GCNT_TARGET2_TARGET2_BMSK                                 0xfff
#define HWIO_RBCPR_GCNT_TARGET2_TARGET2_SHFT                                   0x0

#define HWIO_RBCPR_GCNT_TARGET3_ADDR(x)                                 ((x) + 0x0000006c)
#define HWIO_RBCPR_GCNT_TARGET3_OFFS                                    (0x0000006c)
#define HWIO_RBCPR_GCNT_TARGET3_RMSK                                      0x3fffff
#define HWIO_RBCPR_GCNT_TARGET3_IN(x)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET3_ADDR(x), HWIO_RBCPR_GCNT_TARGET3_RMSK)
#define HWIO_RBCPR_GCNT_TARGET3_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET3_ADDR(x), m)
#define HWIO_RBCPR_GCNT_TARGET3_OUT(x, v)      \
        out_dword(HWIO_RBCPR_GCNT_TARGET3_ADDR(x),v)
#define HWIO_RBCPR_GCNT_TARGET3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_GCNT_TARGET3_ADDR(x),m,v,HWIO_RBCPR_GCNT_TARGET3_IN(x))
#define HWIO_RBCPR_GCNT_TARGET3_GCNT3_BMSK                                0x3ff000
#define HWIO_RBCPR_GCNT_TARGET3_GCNT3_SHFT                                     0xc
#define HWIO_RBCPR_GCNT_TARGET3_TARGET3_BMSK                                 0xfff
#define HWIO_RBCPR_GCNT_TARGET3_TARGET3_SHFT                                   0x0

#define HWIO_RBCPR_GCNT_TARGET4_ADDR(x)                                 ((x) + 0x00000070)
#define HWIO_RBCPR_GCNT_TARGET4_OFFS                                    (0x00000070)
#define HWIO_RBCPR_GCNT_TARGET4_RMSK                                      0x3fffff
#define HWIO_RBCPR_GCNT_TARGET4_IN(x)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET4_ADDR(x), HWIO_RBCPR_GCNT_TARGET4_RMSK)
#define HWIO_RBCPR_GCNT_TARGET4_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET4_ADDR(x), m)
#define HWIO_RBCPR_GCNT_TARGET4_OUT(x, v)      \
        out_dword(HWIO_RBCPR_GCNT_TARGET4_ADDR(x),v)
#define HWIO_RBCPR_GCNT_TARGET4_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_GCNT_TARGET4_ADDR(x),m,v,HWIO_RBCPR_GCNT_TARGET4_IN(x))
#define HWIO_RBCPR_GCNT_TARGET4_GCNT4_BMSK                                0x3ff000
#define HWIO_RBCPR_GCNT_TARGET4_GCNT4_SHFT                                     0xc
#define HWIO_RBCPR_GCNT_TARGET4_TARGET4_BMSK                                 0xfff
#define HWIO_RBCPR_GCNT_TARGET4_TARGET4_SHFT                                   0x0

#define HWIO_RBCPR_GCNT_TARGET5_ADDR(x)                                 ((x) + 0x00000074)
#define HWIO_RBCPR_GCNT_TARGET5_OFFS                                    (0x00000074)
#define HWIO_RBCPR_GCNT_TARGET5_RMSK                                      0x3fffff
#define HWIO_RBCPR_GCNT_TARGET5_IN(x)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET5_ADDR(x), HWIO_RBCPR_GCNT_TARGET5_RMSK)
#define HWIO_RBCPR_GCNT_TARGET5_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET5_ADDR(x), m)
#define HWIO_RBCPR_GCNT_TARGET5_OUT(x, v)      \
        out_dword(HWIO_RBCPR_GCNT_TARGET5_ADDR(x),v)
#define HWIO_RBCPR_GCNT_TARGET5_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_GCNT_TARGET5_ADDR(x),m,v,HWIO_RBCPR_GCNT_TARGET5_IN(x))
#define HWIO_RBCPR_GCNT_TARGET5_GCNT5_BMSK                                0x3ff000
#define HWIO_RBCPR_GCNT_TARGET5_GCNT5_SHFT                                     0xc
#define HWIO_RBCPR_GCNT_TARGET5_TARGET5_BMSK                                 0xfff
#define HWIO_RBCPR_GCNT_TARGET5_TARGET5_SHFT                                   0x0

#define HWIO_RBCPR_GCNT_TARGET6_ADDR(x)                                 ((x) + 0x00000078)
#define HWIO_RBCPR_GCNT_TARGET6_OFFS                                    (0x00000078)
#define HWIO_RBCPR_GCNT_TARGET6_RMSK                                      0x3fffff
#define HWIO_RBCPR_GCNT_TARGET6_IN(x)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET6_ADDR(x), HWIO_RBCPR_GCNT_TARGET6_RMSK)
#define HWIO_RBCPR_GCNT_TARGET6_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET6_ADDR(x), m)
#define HWIO_RBCPR_GCNT_TARGET6_OUT(x, v)      \
        out_dword(HWIO_RBCPR_GCNT_TARGET6_ADDR(x),v)
#define HWIO_RBCPR_GCNT_TARGET6_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_GCNT_TARGET6_ADDR(x),m,v,HWIO_RBCPR_GCNT_TARGET6_IN(x))
#define HWIO_RBCPR_GCNT_TARGET6_GCNT6_BMSK                                0x3ff000
#define HWIO_RBCPR_GCNT_TARGET6_GCNT6_SHFT                                     0xc
#define HWIO_RBCPR_GCNT_TARGET6_TARGET6_BMSK                                 0xfff
#define HWIO_RBCPR_GCNT_TARGET6_TARGET6_SHFT                                   0x0

#define HWIO_RBCPR_GCNT_TARGET7_ADDR(x)                                 ((x) + 0x0000007c)
#define HWIO_RBCPR_GCNT_TARGET7_OFFS                                    (0x0000007c)
#define HWIO_RBCPR_GCNT_TARGET7_RMSK                                      0x3fffff
#define HWIO_RBCPR_GCNT_TARGET7_IN(x)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET7_ADDR(x), HWIO_RBCPR_GCNT_TARGET7_RMSK)
#define HWIO_RBCPR_GCNT_TARGET7_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_GCNT_TARGET7_ADDR(x), m)
#define HWIO_RBCPR_GCNT_TARGET7_OUT(x, v)      \
        out_dword(HWIO_RBCPR_GCNT_TARGET7_ADDR(x),v)
#define HWIO_RBCPR_GCNT_TARGET7_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_GCNT_TARGET7_ADDR(x),m,v,HWIO_RBCPR_GCNT_TARGET7_IN(x))
#define HWIO_RBCPR_GCNT_TARGET7_GCNT7_BMSK                                0x3ff000
#define HWIO_RBCPR_GCNT_TARGET7_GCNT7_SHFT                                     0xc
#define HWIO_RBCPR_GCNT_TARGET7_TARGET7_BMSK                                 0xfff
#define HWIO_RBCPR_GCNT_TARGET7_TARGET7_SHFT                                   0x0

#define HWIO_RBCPR_STEP_QUOT_ADDR(x)                                    ((x) + 0x00000080)
#define HWIO_RBCPR_STEP_QUOT_OFFS                                       (0x00000080)
#define HWIO_RBCPR_STEP_QUOT_RMSK                                           0x1fff
#define HWIO_RBCPR_STEP_QUOT_IN(x)      \
        in_dword_masked(HWIO_RBCPR_STEP_QUOT_ADDR(x), HWIO_RBCPR_STEP_QUOT_RMSK)
#define HWIO_RBCPR_STEP_QUOT_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_STEP_QUOT_ADDR(x), m)
#define HWIO_RBCPR_STEP_QUOT_OUT(x, v)      \
        out_dword(HWIO_RBCPR_STEP_QUOT_ADDR(x),v)
#define HWIO_RBCPR_STEP_QUOT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_STEP_QUOT_ADDR(x),m,v,HWIO_RBCPR_STEP_QUOT_IN(x))
#define HWIO_RBCPR_STEP_QUOT_IDLE_CLOCKS_BMSK                               0x1f00
#define HWIO_RBCPR_STEP_QUOT_IDLE_CLOCKS_SHFT                                  0x8
#define HWIO_RBCPR_STEP_QUOT_STEP_QUOT_BMSK                                   0xff
#define HWIO_RBCPR_STEP_QUOT_STEP_QUOT_SHFT                                    0x0

#define HWIO_RBCPR_CTL_ADDR(x)                                          ((x) + 0x00000090)
#define HWIO_RBCPR_CTL_OFFS                                             (0x00000090)
#define HWIO_RBCPR_CTL_RMSK                                             0xff7fffff
#define HWIO_RBCPR_CTL_IN(x)      \
        in_dword_masked(HWIO_RBCPR_CTL_ADDR(x), HWIO_RBCPR_CTL_RMSK)
#define HWIO_RBCPR_CTL_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_CTL_ADDR(x), m)
#define HWIO_RBCPR_CTL_OUT(x, v)      \
        out_dword(HWIO_RBCPR_CTL_ADDR(x),v)
#define HWIO_RBCPR_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_CTL_ADDR(x),m,v,HWIO_RBCPR_CTL_IN(x))
#define HWIO_RBCPR_CTL_DN_THRESHOLD_BMSK                                0xf0000000
#define HWIO_RBCPR_CTL_DN_THRESHOLD_SHFT                                      0x1c
#define HWIO_RBCPR_CTL_UP_THRESHOLD_BMSK                                 0xf000000
#define HWIO_RBCPR_CTL_UP_THRESHOLD_SHFT                                      0x18
#define HWIO_RBCPR_CTL_MIN_MX_CX_DELTA_BMSK                               0x7c0000
#define HWIO_RBCPR_CTL_MIN_MX_CX_DELTA_SHFT                                   0x12
#define HWIO_RBCPR_CTL_CPR_DISABLE_VALID_BMSK                              0x20000
#define HWIO_RBCPR_CTL_CPR_DISABLE_VALID_SHFT                                 0x11
#define HWIO_RBCPR_CTL_MAX_MX_CX_DELTA_BMSK                                0x1f000
#define HWIO_RBCPR_CTL_MAX_MX_CX_DELTA_SHFT                                    0xc
#define HWIO_RBCPR_CTL_HW_TO_PMIC_MX_EN_BMSK                                 0x800
#define HWIO_RBCPR_CTL_HW_TO_PMIC_MX_EN_SHFT                                   0xb
#define HWIO_RBCPR_CTL_COUNT_MODE_BMSK                                       0x400
#define HWIO_RBCPR_CTL_COUNT_MODE_SHFT                                         0xa
#define HWIO_RBCPR_CTL_DEBUG_BUS_SEL_BMSK                                    0x380
#define HWIO_RBCPR_CTL_DEBUG_BUS_SEL_SHFT                                      0x7
#define HWIO_RBCPR_CTL_SW_AUTO_CONT_NACK_DN_EN_BMSK                           0x40
#define HWIO_RBCPR_CTL_SW_AUTO_CONT_NACK_DN_EN_SHFT                            0x6
#define HWIO_RBCPR_CTL_SW_AUTO_CONT_ACK_EN_BMSK                               0x20
#define HWIO_RBCPR_CTL_SW_AUTO_CONT_ACK_EN_SHFT                                0x5
#define HWIO_RBCPR_CTL_HW_TO_PMIC_EN_BMSK                                     0x10
#define HWIO_RBCPR_CTL_HW_TO_PMIC_EN_SHFT                                      0x4
#define HWIO_RBCPR_CTL_HW_TO_PMIC_EN_SW_MODE_FVAL                              0x0
#define HWIO_RBCPR_CTL_HW_TO_PMIC_EN_HW_MODE_FVAL                              0x1
#define HWIO_RBCPR_CTL_TIMER_EN_BMSK                                           0x8
#define HWIO_RBCPR_CTL_TIMER_EN_SHFT                                           0x3
#define HWIO_RBCPR_CTL_INCR_MODE_BMSK                                          0x4
#define HWIO_RBCPR_CTL_INCR_MODE_SHFT                                          0x2
#define HWIO_RBCPR_CTL_INCR_MODE_PROPORTIONAL_TO_QUOT_FVAL                     0x0
#define HWIO_RBCPR_CTL_INCR_MODE_INCREMENTAL_FVAL                              0x1
#define HWIO_RBCPR_CTL_FORCE_TIMER_BMSK                                        0x2
#define HWIO_RBCPR_CTL_FORCE_TIMER_SHFT                                        0x1
#define HWIO_RBCPR_CTL_LOOP_EN_BMSK                                            0x1
#define HWIO_RBCPR_CTL_LOOP_EN_SHFT                                            0x0

#define HWIO_RBIF_SW_VLEVEL_ADDR(x)                                     ((x) + 0x00000094)
#define HWIO_RBIF_SW_VLEVEL_OFFS                                        (0x00000094)
#define HWIO_RBIF_SW_VLEVEL_RMSK                                             0xfff
#define HWIO_RBIF_SW_VLEVEL_IN(x)      \
        in_dword_masked(HWIO_RBIF_SW_VLEVEL_ADDR(x), HWIO_RBIF_SW_VLEVEL_RMSK)
#define HWIO_RBIF_SW_VLEVEL_INM(x, m)      \
        in_dword_masked(HWIO_RBIF_SW_VLEVEL_ADDR(x), m)
#define HWIO_RBIF_SW_VLEVEL_OUT(x, v)      \
        out_dword(HWIO_RBIF_SW_VLEVEL_ADDR(x),v)
#define HWIO_RBIF_SW_VLEVEL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBIF_SW_VLEVEL_ADDR(x),m,v,HWIO_RBIF_SW_VLEVEL_IN(x))
#define HWIO_RBIF_SW_VLEVEL_SW_MX_VLEVEL_BMSK                                0xfc0
#define HWIO_RBIF_SW_VLEVEL_SW_MX_VLEVEL_SHFT                                  0x6
#define HWIO_RBIF_SW_VLEVEL_SW_VLEVEL_BMSK                                    0x3f
#define HWIO_RBIF_SW_VLEVEL_SW_VLEVEL_SHFT                                     0x0

#define HWIO_RBIF_CONT_ACK_CMD_ADDR(x)                                  ((x) + 0x00000098)
#define HWIO_RBIF_CONT_ACK_CMD_OFFS                                     (0x00000098)
#define HWIO_RBIF_CONT_ACK_CMD_RMSK                                     0xffffffff
#define HWIO_RBIF_CONT_ACK_CMD_OUT(x, v)      \
        out_dword(HWIO_RBIF_CONT_ACK_CMD_ADDR(x),v)
#define HWIO_RBIF_CONT_ACK_CMD_RBIF_CONT_ACK_CMD_BMSK                   0xffffffff
#define HWIO_RBIF_CONT_ACK_CMD_RBIF_CONT_ACK_CMD_SHFT                          0x0

#define HWIO_RBIF_CONT_NACK_CMD_ADDR(x)                                 ((x) + 0x0000009c)
#define HWIO_RBIF_CONT_NACK_CMD_OFFS                                    (0x0000009c)
#define HWIO_RBIF_CONT_NACK_CMD_RMSK                                    0xffffffff
#define HWIO_RBIF_CONT_NACK_CMD_OUT(x, v)      \
        out_dword(HWIO_RBIF_CONT_NACK_CMD_ADDR(x),v)
#define HWIO_RBIF_CONT_NACK_CMD_RBIF_CONT_NACK_CMD_BMSK                 0xffffffff
#define HWIO_RBIF_CONT_NACK_CMD_RBIF_CONT_NACK_CMD_SHFT                        0x0

#define HWIO_RBCPR_RESULT_0_ADDR(x)                                     ((x) + 0x000000a0)
#define HWIO_RBCPR_RESULT_0_OFFS                                        (0x000000a0)
#define HWIO_RBCPR_RESULT_0_RMSK                                           0xfffff
#define HWIO_RBCPR_RESULT_0_IN(x)      \
        in_dword_masked(HWIO_RBCPR_RESULT_0_ADDR(x), HWIO_RBCPR_RESULT_0_RMSK)
#define HWIO_RBCPR_RESULT_0_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_RESULT_0_ADDR(x), m)
#define HWIO_RBCPR_RESULT_0_BUSY_BMSK                                      0x80000
#define HWIO_RBCPR_RESULT_0_BUSY_SHFT                                         0x13
#define HWIO_RBCPR_RESULT_0_ERROR_LT_0_BMSK                                0x40000
#define HWIO_RBCPR_RESULT_0_ERROR_LT_0_SHFT                                   0x12
#define HWIO_RBCPR_RESULT_0_ERROR_BMSK                                     0x3ffc0
#define HWIO_RBCPR_RESULT_0_ERROR_SHFT                                         0x6
#define HWIO_RBCPR_RESULT_0_ERROR_STEPS_BMSK                                  0x3c
#define HWIO_RBCPR_RESULT_0_ERROR_STEPS_SHFT                                   0x2
#define HWIO_RBCPR_RESULT_0_STEP_UP_BMSK                                       0x2
#define HWIO_RBCPR_RESULT_0_STEP_UP_SHFT                                       0x1
#define HWIO_RBCPR_RESULT_0_STEP_DN_BMSK                                       0x1
#define HWIO_RBCPR_RESULT_0_STEP_DN_SHFT                                       0x0

#define HWIO_RBCPR_RESULT_1_ADDR(x)                                     ((x) + 0x000000a4)
#define HWIO_RBCPR_RESULT_1_OFFS                                        (0x000000a4)
#define HWIO_RBCPR_RESULT_1_RMSK                                           0xfffff
#define HWIO_RBCPR_RESULT_1_IN(x)      \
        in_dword_masked(HWIO_RBCPR_RESULT_1_ADDR(x), HWIO_RBCPR_RESULT_1_RMSK)
#define HWIO_RBCPR_RESULT_1_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_RESULT_1_ADDR(x), m)
#define HWIO_RBCPR_RESULT_1_SENSOR_SLOW_BMSK                               0xfe000
#define HWIO_RBCPR_RESULT_1_SENSOR_SLOW_SHFT                                   0xd
#define HWIO_RBCPR_RESULT_1_SEL_SLOW_BMSK                                   0x1c00
#define HWIO_RBCPR_RESULT_1_SEL_SLOW_SHFT                                      0xa
#define HWIO_RBCPR_RESULT_1_SENSOR_FAST_BMSK                                 0x3f8
#define HWIO_RBCPR_RESULT_1_SENSOR_FAST_SHFT                                   0x3
#define HWIO_RBCPR_RESULT_1_SEL_FAST_BMSK                                      0x7
#define HWIO_RBCPR_RESULT_1_SEL_FAST_SHFT                                      0x0

#define HWIO_RBIF_IRQ_EN_0_ADDR(x)                                      ((x) + 0x00000100)
#define HWIO_RBIF_IRQ_EN_0_OFFS                                         (0x00000100)
#define HWIO_RBIF_IRQ_EN_0_RMSK                                               0x7f
#define HWIO_RBIF_IRQ_EN_0_IN(x)      \
        in_dword_masked(HWIO_RBIF_IRQ_EN_0_ADDR(x), HWIO_RBIF_IRQ_EN_0_RMSK)
#define HWIO_RBIF_IRQ_EN_0_INM(x, m)      \
        in_dword_masked(HWIO_RBIF_IRQ_EN_0_ADDR(x), m)
#define HWIO_RBIF_IRQ_EN_0_OUT(x, v)      \
        out_dword(HWIO_RBIF_IRQ_EN_0_ADDR(x),v)
#define HWIO_RBIF_IRQ_EN_0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBIF_IRQ_EN_0_ADDR(x),m,v,HWIO_RBIF_IRQ_EN_0_IN(x))
#define HWIO_RBIF_IRQ_EN_0_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                    0x40
#define HWIO_RBIF_IRQ_EN_0_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                     0x6
#define HWIO_RBIF_IRQ_EN_0_MAX_FLAG_EN_BMSK                                   0x20
#define HWIO_RBIF_IRQ_EN_0_MAX_FLAG_EN_SHFT                                    0x5
#define HWIO_RBIF_IRQ_EN_0_UP_FLAG_EN_BMSK                                    0x10
#define HWIO_RBIF_IRQ_EN_0_UP_FLAG_EN_SHFT                                     0x4
#define HWIO_RBIF_IRQ_EN_0_MID_FLAG_EN_BMSK                                    0x8
#define HWIO_RBIF_IRQ_EN_0_MID_FLAG_EN_SHFT                                    0x3
#define HWIO_RBIF_IRQ_EN_0_DOWN_FLAG_EN_BMSK                                   0x4
#define HWIO_RBIF_IRQ_EN_0_DOWN_FLAG_EN_SHFT                                   0x2
#define HWIO_RBIF_IRQ_EN_0_MIN_FLAG_EN_BMSK                                    0x2
#define HWIO_RBIF_IRQ_EN_0_MIN_FLAG_EN_SHFT                                    0x1
#define HWIO_RBIF_IRQ_EN_0_RBCPR_DONE_EN_BMSK                                  0x1
#define HWIO_RBIF_IRQ_EN_0_RBCPR_DONE_EN_SHFT                                  0x0

#define HWIO_RBIF_IRQ_EN_1_ADDR(x)                                      ((x) + 0x00000104)
#define HWIO_RBIF_IRQ_EN_1_OFFS                                         (0x00000104)
#define HWIO_RBIF_IRQ_EN_1_RMSK                                               0x7f
#define HWIO_RBIF_IRQ_EN_1_IN(x)      \
        in_dword_masked(HWIO_RBIF_IRQ_EN_1_ADDR(x), HWIO_RBIF_IRQ_EN_1_RMSK)
#define HWIO_RBIF_IRQ_EN_1_INM(x, m)      \
        in_dword_masked(HWIO_RBIF_IRQ_EN_1_ADDR(x), m)
#define HWIO_RBIF_IRQ_EN_1_OUT(x, v)      \
        out_dword(HWIO_RBIF_IRQ_EN_1_ADDR(x),v)
#define HWIO_RBIF_IRQ_EN_1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBIF_IRQ_EN_1_ADDR(x),m,v,HWIO_RBIF_IRQ_EN_1_IN(x))
#define HWIO_RBIF_IRQ_EN_1_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                    0x40
#define HWIO_RBIF_IRQ_EN_1_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                     0x6
#define HWIO_RBIF_IRQ_EN_1_MAX_FLAG_EN_BMSK                                   0x20
#define HWIO_RBIF_IRQ_EN_1_MAX_FLAG_EN_SHFT                                    0x5
#define HWIO_RBIF_IRQ_EN_1_UP_FLAG_EN_BMSK                                    0x10
#define HWIO_RBIF_IRQ_EN_1_UP_FLAG_EN_SHFT                                     0x4
#define HWIO_RBIF_IRQ_EN_1_MID_FLAG_EN_BMSK                                    0x8
#define HWIO_RBIF_IRQ_EN_1_MID_FLAG_EN_SHFT                                    0x3
#define HWIO_RBIF_IRQ_EN_1_DOWN_FLAG_EN_BMSK                                   0x4
#define HWIO_RBIF_IRQ_EN_1_DOWN_FLAG_EN_SHFT                                   0x2
#define HWIO_RBIF_IRQ_EN_1_MIN_FLAG_EN_BMSK                                    0x2
#define HWIO_RBIF_IRQ_EN_1_MIN_FLAG_EN_SHFT                                    0x1
#define HWIO_RBIF_IRQ_EN_1_RBCPR_DONE_EN_BMSK                                  0x1
#define HWIO_RBIF_IRQ_EN_1_RBCPR_DONE_EN_SHFT                                  0x0

#define HWIO_RBIF_IRQ_EN_2_ADDR(x)                                      ((x) + 0x00000108)
#define HWIO_RBIF_IRQ_EN_2_OFFS                                         (0x00000108)
#define HWIO_RBIF_IRQ_EN_2_RMSK                                               0x7f
#define HWIO_RBIF_IRQ_EN_2_IN(x)      \
        in_dword_masked(HWIO_RBIF_IRQ_EN_2_ADDR(x), HWIO_RBIF_IRQ_EN_2_RMSK)
#define HWIO_RBIF_IRQ_EN_2_INM(x, m)      \
        in_dword_masked(HWIO_RBIF_IRQ_EN_2_ADDR(x), m)
#define HWIO_RBIF_IRQ_EN_2_OUT(x, v)      \
        out_dword(HWIO_RBIF_IRQ_EN_2_ADDR(x),v)
#define HWIO_RBIF_IRQ_EN_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBIF_IRQ_EN_2_ADDR(x),m,v,HWIO_RBIF_IRQ_EN_2_IN(x))
#define HWIO_RBIF_IRQ_EN_2_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                    0x40
#define HWIO_RBIF_IRQ_EN_2_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                     0x6
#define HWIO_RBIF_IRQ_EN_2_MAX_FLAG_EN_BMSK                                   0x20
#define HWIO_RBIF_IRQ_EN_2_MAX_FLAG_EN_SHFT                                    0x5
#define HWIO_RBIF_IRQ_EN_2_UP_FLAG_EN_BMSK                                    0x10
#define HWIO_RBIF_IRQ_EN_2_UP_FLAG_EN_SHFT                                     0x4
#define HWIO_RBIF_IRQ_EN_2_MID_FLAG_EN_BMSK                                    0x8
#define HWIO_RBIF_IRQ_EN_2_MID_FLAG_EN_SHFT                                    0x3
#define HWIO_RBIF_IRQ_EN_2_DOWN_FLAG_EN_BMSK                                   0x4
#define HWIO_RBIF_IRQ_EN_2_DOWN_FLAG_EN_SHFT                                   0x2
#define HWIO_RBIF_IRQ_EN_2_MIN_FLAG_EN_BMSK                                    0x2
#define HWIO_RBIF_IRQ_EN_2_MIN_FLAG_EN_SHFT                                    0x1
#define HWIO_RBIF_IRQ_EN_2_RBCPR_DONE_EN_BMSK                                  0x1
#define HWIO_RBIF_IRQ_EN_2_RBCPR_DONE_EN_SHFT                                  0x0

#define HWIO_RBIF_IRQ_CLEAR_ADDR(x)                                     ((x) + 0x00000110)
#define HWIO_RBIF_IRQ_CLEAR_OFFS                                        (0x00000110)
#define HWIO_RBIF_IRQ_CLEAR_RMSK                                              0x7f
#define HWIO_RBIF_IRQ_CLEAR_OUT(x, v)      \
        out_dword(HWIO_RBIF_IRQ_CLEAR_ADDR(x),v)
#define HWIO_RBIF_IRQ_CLEAR_CLAMP_CHANGE_WHILE_BUSY_CLEAR_BMSK                0x40
#define HWIO_RBIF_IRQ_CLEAR_CLAMP_CHANGE_WHILE_BUSY_CLEAR_SHFT                 0x6
#define HWIO_RBIF_IRQ_CLEAR_MAX_FLAG_CLEAR_BMSK                               0x20
#define HWIO_RBIF_IRQ_CLEAR_MAX_FLAG_CLEAR_SHFT                                0x5
#define HWIO_RBIF_IRQ_CLEAR_UP_FLAG_CLEAR_BMSK                                0x10
#define HWIO_RBIF_IRQ_CLEAR_UP_FLAG_CLEAR_SHFT                                 0x4
#define HWIO_RBIF_IRQ_CLEAR_MID_FLAG_CLEAR_BMSK                                0x8
#define HWIO_RBIF_IRQ_CLEAR_MID_FLAG_CLEAR_SHFT                                0x3
#define HWIO_RBIF_IRQ_CLEAR_DOWN_FLAG_CLEAR_BMSK                               0x4
#define HWIO_RBIF_IRQ_CLEAR_DOWN_FLAG_CLEAR_SHFT                               0x2
#define HWIO_RBIF_IRQ_CLEAR_MIN_FLAG_CLEAR_BMSK                                0x2
#define HWIO_RBIF_IRQ_CLEAR_MIN_FLAG_CLEAR_SHFT                                0x1
#define HWIO_RBIF_IRQ_CLEAR_RBCPR_DONE_CLEAR_BMSK                              0x1
#define HWIO_RBIF_IRQ_CLEAR_RBCPR_DONE_CLEAR_SHFT                              0x0

#define HWIO_RBIF_IRQ_STATUS_ADDR(x)                                    ((x) + 0x00000114)
#define HWIO_RBIF_IRQ_STATUS_OFFS                                       (0x00000114)
#define HWIO_RBIF_IRQ_STATUS_RMSK                                             0x7f
#define HWIO_RBIF_IRQ_STATUS_IN(x)      \
        in_dword_masked(HWIO_RBIF_IRQ_STATUS_ADDR(x), HWIO_RBIF_IRQ_STATUS_RMSK)
#define HWIO_RBIF_IRQ_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_RBIF_IRQ_STATUS_ADDR(x), m)
#define HWIO_RBIF_IRQ_STATUS_CLAMP_CHANGE_WHILE_BUSY_BMSK                     0x40
#define HWIO_RBIF_IRQ_STATUS_CLAMP_CHANGE_WHILE_BUSY_SHFT                      0x6
#define HWIO_RBIF_IRQ_STATUS_MAX_FLAG_BMSK                                    0x20
#define HWIO_RBIF_IRQ_STATUS_MAX_FLAG_SHFT                                     0x5
#define HWIO_RBIF_IRQ_STATUS_UP_FLAG_BMSK                                     0x10
#define HWIO_RBIF_IRQ_STATUS_UP_FLAG_SHFT                                      0x4
#define HWIO_RBIF_IRQ_STATUS_MID_FLAG_BMSK                                     0x8
#define HWIO_RBIF_IRQ_STATUS_MID_FLAG_SHFT                                     0x3
#define HWIO_RBIF_IRQ_STATUS_DOWN_FLAG_BMSK                                    0x4
#define HWIO_RBIF_IRQ_STATUS_DOWN_FLAG_SHFT                                    0x2
#define HWIO_RBIF_IRQ_STATUS_MIN_FLAG_BMSK                                     0x2
#define HWIO_RBIF_IRQ_STATUS_MIN_FLAG_SHFT                                     0x1
#define HWIO_RBIF_IRQ_STATUS_RBCPR_DONE_BMSK                                   0x1
#define HWIO_RBIF_IRQ_STATUS_RBCPR_DONE_SHFT                                   0x0

#define HWIO_RBCPR_QUOT_AVG_ADDR(x)                                     ((x) + 0x00000118)
#define HWIO_RBCPR_QUOT_AVG_OFFS                                        (0x00000118)
#define HWIO_RBCPR_QUOT_AVG_RMSK                                        0x3fffffff
#define HWIO_RBCPR_QUOT_AVG_IN(x)      \
        in_dword_masked(HWIO_RBCPR_QUOT_AVG_ADDR(x), HWIO_RBCPR_QUOT_AVG_RMSK)
#define HWIO_RBCPR_QUOT_AVG_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_QUOT_AVG_ADDR(x), m)
#define HWIO_RBCPR_QUOT_AVG_QUOT_SUM_BMSK                               0x3fffff00
#define HWIO_RBCPR_QUOT_AVG_QUOT_SUM_SHFT                                      0x8
#define HWIO_RBCPR_QUOT_AVG_SENSOR_SUM_BMSK                                   0xff
#define HWIO_RBCPR_QUOT_AVG_SENSOR_SUM_SHFT                                    0x0

#define HWIO_RBCPR_DEBUG0_ADDR(x)                                       ((x) + 0x0000011c)
#define HWIO_RBCPR_DEBUG0_OFFS                                          (0x0000011c)
#define HWIO_RBCPR_DEBUG0_RMSK                                          0xffffffff
#define HWIO_RBCPR_DEBUG0_IN(x)      \
        in_dword_masked(HWIO_RBCPR_DEBUG0_ADDR(x), HWIO_RBCPR_DEBUG0_RMSK)
#define HWIO_RBCPR_DEBUG0_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_DEBUG0_ADDR(x), m)
#define HWIO_RBCPR_DEBUG0_SCLK_CNT1_BMSK                                0xffe00000
#define HWIO_RBCPR_DEBUG0_SCLK_CNT1_SHFT                                      0x15
#define HWIO_RBCPR_DEBUG0_SCLK_CNT0_BMSK                                  0x1ffc00
#define HWIO_RBCPR_DEBUG0_SCLK_CNT0_SHFT                                       0xa
#define HWIO_RBCPR_DEBUG0_SCLK_CNT_FAIL_BMSK                                 0x300
#define HWIO_RBCPR_DEBUG0_SCLK_CNT_FAIL_SHFT                                   0x8
#define HWIO_RBCPR_DEBUG0_DBG_END_OF_CHAIN_DREG_BMSK                          0xc0
#define HWIO_RBCPR_DEBUG0_DBG_END_OF_CHAIN_DREG_SHFT                           0x6
#define HWIO_RBCPR_DEBUG0_DBG_SHIFT_RING_BMSK                                 0x3f
#define HWIO_RBCPR_DEBUG0_DBG_SHIFT_RING_SHFT                                  0x0

#define HWIO_RBCPR_DEBUG1_ADDR(x)                                       ((x) + 0x00000120)
#define HWIO_RBCPR_DEBUG1_OFFS                                          (0x00000120)
#define HWIO_RBCPR_DEBUG1_RMSK                                          0xffffffff
#define HWIO_RBCPR_DEBUG1_IN(x)      \
        in_dword_masked(HWIO_RBCPR_DEBUG1_ADDR(x), HWIO_RBCPR_DEBUG1_RMSK)
#define HWIO_RBCPR_DEBUG1_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_DEBUG1_ADDR(x), m)
#define HWIO_RBCPR_DEBUG1_DBG_END_OF_CHAIN_ENA_BMSK                     0x80000000
#define HWIO_RBCPR_DEBUG1_DBG_END_OF_CHAIN_ENA_SHFT                           0x1f
#define HWIO_RBCPR_DEBUG1_DBG_VALID_BMSK                                0x40000000
#define HWIO_RBCPR_DEBUG1_DBG_VALID_SHFT                                      0x1e
#define HWIO_RBCPR_DEBUG1_DBG_END_OF_CHAIN_MODE_BMSK                    0x20000000
#define HWIO_RBCPR_DEBUG1_DBG_END_OF_CHAIN_MODE_SHFT                          0x1d
#define HWIO_RBCPR_DEBUG1_DBG_END_OF_CHAIN_DATA_BMSK                    0x18000000
#define HWIO_RBCPR_DEBUG1_DBG_END_OF_CHAIN_DATA_SHFT                          0x1b
#define HWIO_RBCPR_DEBUG1_DBG_FSM_STATE_BMSK                             0x7000000
#define HWIO_RBCPR_DEBUG1_DBG_FSM_STATE_SHFT                                  0x18
#define HWIO_RBCPR_DEBUG1_QUOT_SLOW_BMSK                                  0xfff000
#define HWIO_RBCPR_DEBUG1_QUOT_SLOW_SHFT                                       0xc
#define HWIO_RBCPR_DEBUG1_QUOT_FAST_BMSK                                     0xfff
#define HWIO_RBCPR_DEBUG1_QUOT_FAST_SHFT                                       0x0

#define HWIO_RBCPR_DEBUG2_ADDR(x)                                       ((x) + 0x00000124)
#define HWIO_RBCPR_DEBUG2_OFFS                                          (0x00000124)
#define HWIO_RBCPR_DEBUG2_RMSK                                          0xffffffff
#define HWIO_RBCPR_DEBUG2_IN(x)      \
        in_dword_masked(HWIO_RBCPR_DEBUG2_ADDR(x), HWIO_RBCPR_DEBUG2_RMSK)
#define HWIO_RBCPR_DEBUG2_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_DEBUG2_ADDR(x), m)
#define HWIO_RBCPR_DEBUG2_DBG_SEL_DONE_BMSK                             0x80000000
#define HWIO_RBCPR_DEBUG2_DBG_SEL_DONE_SHFT                                   0x1f
#define HWIO_RBCPR_DEBUG2_DBG_SENSOR_DONE_BMSK                          0x40000000
#define HWIO_RBCPR_DEBUG2_DBG_SENSOR_DONE_SHFT                                0x1e
#define HWIO_RBCPR_DEBUG2_DBG_SENSOR_BMSK                               0x3f800000
#define HWIO_RBCPR_DEBUG2_DBG_SENSOR_SHFT                                     0x17
#define HWIO_RBCPR_DEBUG2_DBG_SEL_BMSK                                    0x700000
#define HWIO_RBCPR_DEBUG2_DBG_SEL_SHFT                                        0x14
#define HWIO_RBCPR_DEBUG2_DBG_QUOT_BMSK                                    0xfff00
#define HWIO_RBCPR_DEBUG2_DBG_QUOT_SHFT                                        0x8
#define HWIO_RBCPR_DEBUG2_DBG_STEP_QUOT_BMSK                                  0xff
#define HWIO_RBCPR_DEBUG2_DBG_STEP_QUOT_SHFT                                   0x0

#define HWIO_RBCPR_BIST_MINMAX_ADDR(x)                                  ((x) + 0x00000128)
#define HWIO_RBCPR_BIST_MINMAX_OFFS                                     (0x00000128)
#define HWIO_RBCPR_BIST_MINMAX_RMSK                                       0xffffff
#define HWIO_RBCPR_BIST_MINMAX_IN(x)      \
        in_dword_masked(HWIO_RBCPR_BIST_MINMAX_ADDR(x), HWIO_RBCPR_BIST_MINMAX_RMSK)
#define HWIO_RBCPR_BIST_MINMAX_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_BIST_MINMAX_ADDR(x), m)
#define HWIO_RBCPR_BIST_MINMAX_OUT(x, v)      \
        out_dword(HWIO_RBCPR_BIST_MINMAX_ADDR(x),v)
#define HWIO_RBCPR_BIST_MINMAX_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_BIST_MINMAX_ADDR(x),m,v,HWIO_RBCPR_BIST_MINMAX_IN(x))
#define HWIO_RBCPR_BIST_MINMAX_BIST_QUOT_ABSMIN_BMSK                      0xfff000
#define HWIO_RBCPR_BIST_MINMAX_BIST_QUOT_ABSMIN_SHFT                           0xc
#define HWIO_RBCPR_BIST_MINMAX_BIST_QUOT_ABSMAX_BMSK                         0xfff
#define HWIO_RBCPR_BIST_MINMAX_BIST_QUOT_ABSMAX_SHFT                           0x0

#define HWIO_RBCPR_BIST_RESULT_ADDR(x)                                  ((x) + 0x0000012c)
#define HWIO_RBCPR_BIST_RESULT_OFFS                                     (0x0000012c)
#define HWIO_RBCPR_BIST_RESULT_RMSK                                        0x1ffff
#define HWIO_RBCPR_BIST_RESULT_IN(x)      \
        in_dword_masked(HWIO_RBCPR_BIST_RESULT_ADDR(x), HWIO_RBCPR_BIST_RESULT_RMSK)
#define HWIO_RBCPR_BIST_RESULT_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_BIST_RESULT_ADDR(x), m)
#define HWIO_RBCPR_BIST_RESULT_SENSOR_ELAB_DONE_BMSK                       0x10000
#define HWIO_RBCPR_BIST_RESULT_SENSOR_ELAB_DONE_SHFT                          0x10
#define HWIO_RBCPR_BIST_RESULT_BIST_FAIL_BMSK                               0x8000
#define HWIO_RBCPR_BIST_RESULT_BIST_FAIL_SHFT                                  0xf
#define HWIO_RBCPR_BIST_RESULT_BIST_FAIL_LASTSEL_BMSK                       0x7000
#define HWIO_RBCPR_BIST_RESULT_BIST_FAIL_LASTSEL_SHFT                          0xc
#define HWIO_RBCPR_BIST_RESULT_BIST_FAIL_LASTQUOT_BMSK                       0xfff
#define HWIO_RBCPR_BIST_RESULT_BIST_FAIL_LASTQUOT_SHFT                         0x0

#define HWIO_RBCPR_BIST_FAIL_MAP0_ADDR(x)                               ((x) + 0x00000130)
#define HWIO_RBCPR_BIST_FAIL_MAP0_OFFS                                  (0x00000130)
#define HWIO_RBCPR_BIST_FAIL_MAP0_RMSK                                  0xffffffff
#define HWIO_RBCPR_BIST_FAIL_MAP0_IN(x)      \
        in_dword_masked(HWIO_RBCPR_BIST_FAIL_MAP0_ADDR(x), HWIO_RBCPR_BIST_FAIL_MAP0_RMSK)
#define HWIO_RBCPR_BIST_FAIL_MAP0_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_BIST_FAIL_MAP0_ADDR(x), m)
#define HWIO_RBCPR_BIST_FAIL_MAP0_BIST_FAIL_MAP_BMSK                    0xffffffff
#define HWIO_RBCPR_BIST_FAIL_MAP0_BIST_FAIL_MAP_SHFT                           0x0

#define HWIO_RBCPR_BIST_FAIL_MAP1_ADDR(x)                               ((x) + 0x00000134)
#define HWIO_RBCPR_BIST_FAIL_MAP1_OFFS                                  (0x00000134)
#define HWIO_RBCPR_BIST_FAIL_MAP1_RMSK                                  0xffffffff
#define HWIO_RBCPR_BIST_FAIL_MAP1_IN(x)      \
        in_dword_masked(HWIO_RBCPR_BIST_FAIL_MAP1_ADDR(x), HWIO_RBCPR_BIST_FAIL_MAP1_RMSK)
#define HWIO_RBCPR_BIST_FAIL_MAP1_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_BIST_FAIL_MAP1_ADDR(x), m)
#define HWIO_RBCPR_BIST_FAIL_MAP1_BIST_FAIL_MAP_BMSK                    0xffffffff
#define HWIO_RBCPR_BIST_FAIL_MAP1_BIST_FAIL_MAP_SHFT                           0x0

#define HWIO_RBCPR_BIST_FAIL_MAP2_ADDR(x)                               ((x) + 0x00000138)
#define HWIO_RBCPR_BIST_FAIL_MAP2_OFFS                                  (0x00000138)
#define HWIO_RBCPR_BIST_FAIL_MAP2_RMSK                                  0xffffffff
#define HWIO_RBCPR_BIST_FAIL_MAP2_IN(x)      \
        in_dword_masked(HWIO_RBCPR_BIST_FAIL_MAP2_ADDR(x), HWIO_RBCPR_BIST_FAIL_MAP2_RMSK)
#define HWIO_RBCPR_BIST_FAIL_MAP2_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_BIST_FAIL_MAP2_ADDR(x), m)
#define HWIO_RBCPR_BIST_FAIL_MAP2_BIST_FAIL_MAP_BMSK                    0xffffffff
#define HWIO_RBCPR_BIST_FAIL_MAP2_BIST_FAIL_MAP_SHFT                           0x0

#define HWIO_RBCPR_BIST_FAIL_MAP3_ADDR(x)                               ((x) + 0x0000013c)
#define HWIO_RBCPR_BIST_FAIL_MAP3_OFFS                                  (0x0000013c)
#define HWIO_RBCPR_BIST_FAIL_MAP3_RMSK                                  0xffffffff
#define HWIO_RBCPR_BIST_FAIL_MAP3_IN(x)      \
        in_dword_masked(HWIO_RBCPR_BIST_FAIL_MAP3_ADDR(x), HWIO_RBCPR_BIST_FAIL_MAP3_RMSK)
#define HWIO_RBCPR_BIST_FAIL_MAP3_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_BIST_FAIL_MAP3_ADDR(x), m)
#define HWIO_RBCPR_BIST_FAIL_MAP3_BIST_FAIL_MAP_BMSK                    0xffffffff
#define HWIO_RBCPR_BIST_FAIL_MAP3_BIST_FAIL_MAP_SHFT                           0x0

#define HWIO_RBCPR_LOG_0_ADDR(x)                                        ((x) + 0x00000140)
#define HWIO_RBCPR_LOG_0_OFFS                                           (0x00000140)
#define HWIO_RBCPR_LOG_0_RMSK                                           0xfffff7ff
#define HWIO_RBCPR_LOG_0_IN(x)      \
        in_dword_masked(HWIO_RBCPR_LOG_0_ADDR(x), HWIO_RBCPR_LOG_0_RMSK)
#define HWIO_RBCPR_LOG_0_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_LOG_0_ADDR(x), m)
#define HWIO_RBCPR_LOG_0_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_RBCPR_LOG_0_MID_COUNT_SHFT                                       0x12
#define HWIO_RBCPR_LOG_0_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_RBCPR_LOG_0_MX_VLEVEL_SHFT                                        0xc
#define HWIO_RBCPR_LOG_0_MAX_FLAG_BMSK                                       0x400
#define HWIO_RBCPR_LOG_0_MAX_FLAG_SHFT                                         0xa
#define HWIO_RBCPR_LOG_0_UP_FLAG_BMSK                                        0x200
#define HWIO_RBCPR_LOG_0_UP_FLAG_SHFT                                          0x9
#define HWIO_RBCPR_LOG_0_MID_FLAG_BMSK                                       0x100
#define HWIO_RBCPR_LOG_0_MID_FLAG_SHFT                                         0x8
#define HWIO_RBCPR_LOG_0_DOWN_FLAG_BMSK                                       0x80
#define HWIO_RBCPR_LOG_0_DOWN_FLAG_SHFT                                        0x7
#define HWIO_RBCPR_LOG_0_MIN_FLAG_BMSK                                        0x40
#define HWIO_RBCPR_LOG_0_MIN_FLAG_SHFT                                         0x6
#define HWIO_RBCPR_LOG_0_VLEVEL_BMSK                                          0x3f
#define HWIO_RBCPR_LOG_0_VLEVEL_SHFT                                           0x0

#define HWIO_RBCPR_LOG_1_ADDR(x)                                        ((x) + 0x00000144)
#define HWIO_RBCPR_LOG_1_OFFS                                           (0x00000144)
#define HWIO_RBCPR_LOG_1_RMSK                                           0xfffff7ff
#define HWIO_RBCPR_LOG_1_IN(x)      \
        in_dword_masked(HWIO_RBCPR_LOG_1_ADDR(x), HWIO_RBCPR_LOG_1_RMSK)
#define HWIO_RBCPR_LOG_1_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_LOG_1_ADDR(x), m)
#define HWIO_RBCPR_LOG_1_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_RBCPR_LOG_1_MID_COUNT_SHFT                                       0x12
#define HWIO_RBCPR_LOG_1_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_RBCPR_LOG_1_MX_VLEVEL_SHFT                                        0xc
#define HWIO_RBCPR_LOG_1_MAX_FLAG_BMSK                                       0x400
#define HWIO_RBCPR_LOG_1_MAX_FLAG_SHFT                                         0xa
#define HWIO_RBCPR_LOG_1_UP_FLAG_BMSK                                        0x200
#define HWIO_RBCPR_LOG_1_UP_FLAG_SHFT                                          0x9
#define HWIO_RBCPR_LOG_1_MID_FLAG_BMSK                                       0x100
#define HWIO_RBCPR_LOG_1_MID_FLAG_SHFT                                         0x8
#define HWIO_RBCPR_LOG_1_DOWN_FLAG_BMSK                                       0x80
#define HWIO_RBCPR_LOG_1_DOWN_FLAG_SHFT                                        0x7
#define HWIO_RBCPR_LOG_1_MIN_FLAG_BMSK                                        0x40
#define HWIO_RBCPR_LOG_1_MIN_FLAG_SHFT                                         0x6
#define HWIO_RBCPR_LOG_1_VLEVEL_BMSK                                          0x3f
#define HWIO_RBCPR_LOG_1_VLEVEL_SHFT                                           0x0

#define HWIO_RBCPR_LOG_2_ADDR(x)                                        ((x) + 0x00000148)
#define HWIO_RBCPR_LOG_2_OFFS                                           (0x00000148)
#define HWIO_RBCPR_LOG_2_RMSK                                           0xfffff7ff
#define HWIO_RBCPR_LOG_2_IN(x)      \
        in_dword_masked(HWIO_RBCPR_LOG_2_ADDR(x), HWIO_RBCPR_LOG_2_RMSK)
#define HWIO_RBCPR_LOG_2_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_LOG_2_ADDR(x), m)
#define HWIO_RBCPR_LOG_2_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_RBCPR_LOG_2_MID_COUNT_SHFT                                       0x12
#define HWIO_RBCPR_LOG_2_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_RBCPR_LOG_2_MX_VLEVEL_SHFT                                        0xc
#define HWIO_RBCPR_LOG_2_MAX_FLAG_BMSK                                       0x400
#define HWIO_RBCPR_LOG_2_MAX_FLAG_SHFT                                         0xa
#define HWIO_RBCPR_LOG_2_UP_FLAG_BMSK                                        0x200
#define HWIO_RBCPR_LOG_2_UP_FLAG_SHFT                                          0x9
#define HWIO_RBCPR_LOG_2_MID_FLAG_BMSK                                       0x100
#define HWIO_RBCPR_LOG_2_MID_FLAG_SHFT                                         0x8
#define HWIO_RBCPR_LOG_2_DOWN_FLAG_BMSK                                       0x80
#define HWIO_RBCPR_LOG_2_DOWN_FLAG_SHFT                                        0x7
#define HWIO_RBCPR_LOG_2_MIN_FLAG_BMSK                                        0x40
#define HWIO_RBCPR_LOG_2_MIN_FLAG_SHFT                                         0x6
#define HWIO_RBCPR_LOG_2_VLEVEL_BMSK                                          0x3f
#define HWIO_RBCPR_LOG_2_VLEVEL_SHFT                                           0x0

#define HWIO_RBCPR_LOG_3_ADDR(x)                                        ((x) + 0x0000014c)
#define HWIO_RBCPR_LOG_3_OFFS                                           (0x0000014c)
#define HWIO_RBCPR_LOG_3_RMSK                                           0xfffff7ff
#define HWIO_RBCPR_LOG_3_IN(x)      \
        in_dword_masked(HWIO_RBCPR_LOG_3_ADDR(x), HWIO_RBCPR_LOG_3_RMSK)
#define HWIO_RBCPR_LOG_3_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_LOG_3_ADDR(x), m)
#define HWIO_RBCPR_LOG_3_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_RBCPR_LOG_3_MID_COUNT_SHFT                                       0x12
#define HWIO_RBCPR_LOG_3_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_RBCPR_LOG_3_MX_VLEVEL_SHFT                                        0xc
#define HWIO_RBCPR_LOG_3_MAX_FLAG_BMSK                                       0x400
#define HWIO_RBCPR_LOG_3_MAX_FLAG_SHFT                                         0xa
#define HWIO_RBCPR_LOG_3_UP_FLAG_BMSK                                        0x200
#define HWIO_RBCPR_LOG_3_UP_FLAG_SHFT                                          0x9
#define HWIO_RBCPR_LOG_3_MID_FLAG_BMSK                                       0x100
#define HWIO_RBCPR_LOG_3_MID_FLAG_SHFT                                         0x8
#define HWIO_RBCPR_LOG_3_DOWN_FLAG_BMSK                                       0x80
#define HWIO_RBCPR_LOG_3_DOWN_FLAG_SHFT                                        0x7
#define HWIO_RBCPR_LOG_3_MIN_FLAG_BMSK                                        0x40
#define HWIO_RBCPR_LOG_3_MIN_FLAG_SHFT                                         0x6
#define HWIO_RBCPR_LOG_3_VLEVEL_BMSK                                          0x3f
#define HWIO_RBCPR_LOG_3_VLEVEL_SHFT                                           0x0

#define HWIO_RBCPR_LOG_4_ADDR(x)                                        ((x) + 0x00000150)
#define HWIO_RBCPR_LOG_4_OFFS                                           (0x00000150)
#define HWIO_RBCPR_LOG_4_RMSK                                           0xfffff7ff
#define HWIO_RBCPR_LOG_4_IN(x)      \
        in_dword_masked(HWIO_RBCPR_LOG_4_ADDR(x), HWIO_RBCPR_LOG_4_RMSK)
#define HWIO_RBCPR_LOG_4_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_LOG_4_ADDR(x), m)
#define HWIO_RBCPR_LOG_4_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_RBCPR_LOG_4_MID_COUNT_SHFT                                       0x12
#define HWIO_RBCPR_LOG_4_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_RBCPR_LOG_4_MX_VLEVEL_SHFT                                        0xc
#define HWIO_RBCPR_LOG_4_MAX_FLAG_BMSK                                       0x400
#define HWIO_RBCPR_LOG_4_MAX_FLAG_SHFT                                         0xa
#define HWIO_RBCPR_LOG_4_UP_FLAG_BMSK                                        0x200
#define HWIO_RBCPR_LOG_4_UP_FLAG_SHFT                                          0x9
#define HWIO_RBCPR_LOG_4_MID_FLAG_BMSK                                       0x100
#define HWIO_RBCPR_LOG_4_MID_FLAG_SHFT                                         0x8
#define HWIO_RBCPR_LOG_4_DOWN_FLAG_BMSK                                       0x80
#define HWIO_RBCPR_LOG_4_DOWN_FLAG_SHFT                                        0x7
#define HWIO_RBCPR_LOG_4_MIN_FLAG_BMSK                                        0x40
#define HWIO_RBCPR_LOG_4_MIN_FLAG_SHFT                                         0x6
#define HWIO_RBCPR_LOG_4_VLEVEL_BMSK                                          0x3f
#define HWIO_RBCPR_LOG_4_VLEVEL_SHFT                                           0x0

#define HWIO_RBCPR_LOG_5_ADDR(x)                                        ((x) + 0x00000154)
#define HWIO_RBCPR_LOG_5_OFFS                                           (0x00000154)
#define HWIO_RBCPR_LOG_5_RMSK                                           0xfffff7ff
#define HWIO_RBCPR_LOG_5_IN(x)      \
        in_dword_masked(HWIO_RBCPR_LOG_5_ADDR(x), HWIO_RBCPR_LOG_5_RMSK)
#define HWIO_RBCPR_LOG_5_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_LOG_5_ADDR(x), m)
#define HWIO_RBCPR_LOG_5_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_RBCPR_LOG_5_MID_COUNT_SHFT                                       0x12
#define HWIO_RBCPR_LOG_5_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_RBCPR_LOG_5_MX_VLEVEL_SHFT                                        0xc
#define HWIO_RBCPR_LOG_5_MAX_FLAG_BMSK                                       0x400
#define HWIO_RBCPR_LOG_5_MAX_FLAG_SHFT                                         0xa
#define HWIO_RBCPR_LOG_5_UP_FLAG_BMSK                                        0x200
#define HWIO_RBCPR_LOG_5_UP_FLAG_SHFT                                          0x9
#define HWIO_RBCPR_LOG_5_MID_FLAG_BMSK                                       0x100
#define HWIO_RBCPR_LOG_5_MID_FLAG_SHFT                                         0x8
#define HWIO_RBCPR_LOG_5_DOWN_FLAG_BMSK                                       0x80
#define HWIO_RBCPR_LOG_5_DOWN_FLAG_SHFT                                        0x7
#define HWIO_RBCPR_LOG_5_MIN_FLAG_BMSK                                        0x40
#define HWIO_RBCPR_LOG_5_MIN_FLAG_SHFT                                         0x6
#define HWIO_RBCPR_LOG_5_VLEVEL_BMSK                                          0x3f
#define HWIO_RBCPR_LOG_5_VLEVEL_SHFT                                           0x0

#define HWIO_RBCPR_LOG_6_ADDR(x)                                        ((x) + 0x00000158)
#define HWIO_RBCPR_LOG_6_OFFS                                           (0x00000158)
#define HWIO_RBCPR_LOG_6_RMSK                                           0xfffff7ff
#define HWIO_RBCPR_LOG_6_IN(x)      \
        in_dword_masked(HWIO_RBCPR_LOG_6_ADDR(x), HWIO_RBCPR_LOG_6_RMSK)
#define HWIO_RBCPR_LOG_6_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_LOG_6_ADDR(x), m)
#define HWIO_RBCPR_LOG_6_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_RBCPR_LOG_6_MID_COUNT_SHFT                                       0x12
#define HWIO_RBCPR_LOG_6_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_RBCPR_LOG_6_MX_VLEVEL_SHFT                                        0xc
#define HWIO_RBCPR_LOG_6_MAX_FLAG_BMSK                                       0x400
#define HWIO_RBCPR_LOG_6_MAX_FLAG_SHFT                                         0xa
#define HWIO_RBCPR_LOG_6_UP_FLAG_BMSK                                        0x200
#define HWIO_RBCPR_LOG_6_UP_FLAG_SHFT                                          0x9
#define HWIO_RBCPR_LOG_6_MID_FLAG_BMSK                                       0x100
#define HWIO_RBCPR_LOG_6_MID_FLAG_SHFT                                         0x8
#define HWIO_RBCPR_LOG_6_DOWN_FLAG_BMSK                                       0x80
#define HWIO_RBCPR_LOG_6_DOWN_FLAG_SHFT                                        0x7
#define HWIO_RBCPR_LOG_6_MIN_FLAG_BMSK                                        0x40
#define HWIO_RBCPR_LOG_6_MIN_FLAG_SHFT                                         0x6
#define HWIO_RBCPR_LOG_6_VLEVEL_BMSK                                          0x3f
#define HWIO_RBCPR_LOG_6_VLEVEL_SHFT                                           0x0

#define HWIO_RBCPR_LOG_7_ADDR(x)                                        ((x) + 0x0000015c)
#define HWIO_RBCPR_LOG_7_OFFS                                           (0x0000015c)
#define HWIO_RBCPR_LOG_7_RMSK                                           0xfffff7ff
#define HWIO_RBCPR_LOG_7_IN(x)      \
        in_dword_masked(HWIO_RBCPR_LOG_7_ADDR(x), HWIO_RBCPR_LOG_7_RMSK)
#define HWIO_RBCPR_LOG_7_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_LOG_7_ADDR(x), m)
#define HWIO_RBCPR_LOG_7_MID_COUNT_BMSK                                 0xfffc0000
#define HWIO_RBCPR_LOG_7_MID_COUNT_SHFT                                       0x12
#define HWIO_RBCPR_LOG_7_MX_VLEVEL_BMSK                                    0x3f000
#define HWIO_RBCPR_LOG_7_MX_VLEVEL_SHFT                                        0xc
#define HWIO_RBCPR_LOG_7_MAX_FLAG_BMSK                                       0x400
#define HWIO_RBCPR_LOG_7_MAX_FLAG_SHFT                                         0xa
#define HWIO_RBCPR_LOG_7_UP_FLAG_BMSK                                        0x200
#define HWIO_RBCPR_LOG_7_UP_FLAG_SHFT                                          0x9
#define HWIO_RBCPR_LOG_7_MID_FLAG_BMSK                                       0x100
#define HWIO_RBCPR_LOG_7_MID_FLAG_SHFT                                         0x8
#define HWIO_RBCPR_LOG_7_DOWN_FLAG_BMSK                                       0x80
#define HWIO_RBCPR_LOG_7_DOWN_FLAG_SHFT                                        0x7
#define HWIO_RBCPR_LOG_7_MIN_FLAG_BMSK                                        0x40
#define HWIO_RBCPR_LOG_7_MIN_FLAG_SHFT                                         0x6
#define HWIO_RBCPR_LOG_7_VLEVEL_BMSK                                          0x3f
#define HWIO_RBCPR_LOG_7_VLEVEL_SHFT                                           0x0

#define HWIO_RBCPR_HTOL_AGE_ADDR(x)                                     ((x) + 0x00000160)
#define HWIO_RBCPR_HTOL_AGE_OFFS                                        (0x00000160)
#define HWIO_RBCPR_HTOL_AGE_RMSK                                               0x7
#define HWIO_RBCPR_HTOL_AGE_IN(x)      \
        in_dword_masked(HWIO_RBCPR_HTOL_AGE_ADDR(x), HWIO_RBCPR_HTOL_AGE_RMSK)
#define HWIO_RBCPR_HTOL_AGE_INM(x, m)      \
        in_dword_masked(HWIO_RBCPR_HTOL_AGE_ADDR(x), m)
#define HWIO_RBCPR_HTOL_AGE_OUT(x, v)      \
        out_dword(HWIO_RBCPR_HTOL_AGE_ADDR(x),v)
#define HWIO_RBCPR_HTOL_AGE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RBCPR_HTOL_AGE_ADDR(x),m,v,HWIO_RBCPR_HTOL_AGE_IN(x))
#define HWIO_RBCPR_HTOL_AGE_CPR_AGE_DATA_STATUS_BMSK                           0x4
#define HWIO_RBCPR_HTOL_AGE_CPR_AGE_DATA_STATUS_SHFT                           0x2
#define HWIO_RBCPR_HTOL_AGE_AGE_PAGE_BMSK                                      0x2
#define HWIO_RBCPR_HTOL_AGE_AGE_PAGE_SHFT                                      0x1
#define HWIO_RBCPR_HTOL_AGE_HTOL_MODE_BMSK                                     0x1
#define HWIO_RBCPR_HTOL_AGE_HTOL_MODE_SHFT                                     0x0


#endif /* __HAL_CPR_HWIO_H__ */
