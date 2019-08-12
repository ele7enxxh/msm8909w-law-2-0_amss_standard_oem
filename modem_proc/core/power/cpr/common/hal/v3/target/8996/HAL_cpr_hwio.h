

#ifndef __HAL_CPR_HWIO_H__
#define __HAL_CPR_HWIO_H__
/*
===========================================================================
*/
/**
  @file HAL_cpr_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MSM8996 (Istari) [istari_v1.0_p2q1r8.1.7_MTO]
 
  This file contains HWIO register definitions for the following modules:
    CPR3

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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/cpr/common/hal/v3/target/8996/HAL_cpr_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: CPR3
 *--------------------------------------------------------------------------*/

#define CPR3_REG_BASE                                                    (CPR3_BASE      + 0x00000000)

#define HWIO_CPR_VERSION_ADDR(x)                                         ((x) + 0x00000000)
#define HWIO_CPR_VERSION_RMSK                                            0xffffffff
#define HWIO_CPR_VERSION_IN(x)      \
        in_dword_masked(HWIO_CPR_VERSION_ADDR(x), HWIO_CPR_VERSION_RMSK)
#define HWIO_CPR_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_CPR_VERSION_ADDR(x), m)
#define HWIO_CPR_VERSION_CPR_VERSION_BMSK                                0xffffffff
#define HWIO_CPR_VERSION_CPR_VERSION_SHFT                                       0x0

#define HWIO_CPR_FSM_CTL_ADDR(x)                                         ((x) + 0x00000004)
#define HWIO_CPR_FSM_CTL_RMSK                                            0xffffffff
#define HWIO_CPR_FSM_CTL_IN(x)      \
        in_dword_masked(HWIO_CPR_FSM_CTL_ADDR(x), HWIO_CPR_FSM_CTL_RMSK)
#define HWIO_CPR_FSM_CTL_INM(x, m)      \
        in_dword_masked(HWIO_CPR_FSM_CTL_ADDR(x), m)
#define HWIO_CPR_FSM_CTL_OUT(x, v)      \
        out_dword(HWIO_CPR_FSM_CTL_ADDR(x),v)
#define HWIO_CPR_FSM_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_FSM_CTL_ADDR(x),m,v,HWIO_CPR_FSM_CTL_IN(x))
#define HWIO_CPR_FSM_CTL_COUNT_REPEAT_BMSK                               0xfffffe00
#define HWIO_CPR_FSM_CTL_COUNT_REPEAT_SHFT                                      0x9
#define HWIO_CPR_FSM_CTL_SET_HTOL_MODE_BMSK                                   0x100
#define HWIO_CPR_FSM_CTL_SET_HTOL_MODE_SHFT                                     0x8
#define HWIO_CPR_FSM_CTL_COUNT_MODE_BMSK                                       0xc0
#define HWIO_CPR_FSM_CTL_COUNT_MODE_SHFT                                        0x6
#define HWIO_CPR_FSM_CTL_IDLE_CLOCKS_BMSK                                      0x3e
#define HWIO_CPR_FSM_CTL_IDLE_CLOCKS_SHFT                                       0x1
#define HWIO_CPR_FSM_CTL_LOOP_EN_BMSK                                           0x1
#define HWIO_CPR_FSM_CTL_LOOP_EN_SHFT                                           0x0

#define HWIO_CPR_FSM_STA_ADDR(x)                                         ((x) + 0x00000008)
#define HWIO_CPR_FSM_STA_RMSK                                                   0x3
#define HWIO_CPR_FSM_STA_IN(x)      \
        in_dword_masked(HWIO_CPR_FSM_STA_ADDR(x), HWIO_CPR_FSM_STA_RMSK)
#define HWIO_CPR_FSM_STA_INM(x, m)      \
        in_dword_masked(HWIO_CPR_FSM_STA_ADDR(x), m)
#define HWIO_CPR_FSM_STA_PAGE_IS_AGE_BMSK                                       0x2
#define HWIO_CPR_FSM_STA_PAGE_IS_AGE_SHFT                                       0x1
#define HWIO_CPR_FSM_STA_BUSY_BMSK                                              0x1
#define HWIO_CPR_FSM_STA_BUSY_SHFT                                              0x0

#define HWIO_CPR_TIMER_AUTO_CONT_ADDR(x)                                 ((x) + 0x0000000c)
#define HWIO_CPR_TIMER_AUTO_CONT_RMSK                                    0xffffffff
#define HWIO_CPR_TIMER_AUTO_CONT_IN(x)      \
        in_dword_masked(HWIO_CPR_TIMER_AUTO_CONT_ADDR(x), HWIO_CPR_TIMER_AUTO_CONT_RMSK)
#define HWIO_CPR_TIMER_AUTO_CONT_INM(x, m)      \
        in_dword_masked(HWIO_CPR_TIMER_AUTO_CONT_ADDR(x), m)
#define HWIO_CPR_TIMER_AUTO_CONT_OUT(x, v)      \
        out_dword(HWIO_CPR_TIMER_AUTO_CONT_ADDR(x),v)
#define HWIO_CPR_TIMER_AUTO_CONT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_TIMER_AUTO_CONT_ADDR(x),m,v,HWIO_CPR_TIMER_AUTO_CONT_IN(x))
#define HWIO_CPR_TIMER_AUTO_CONT_AUTO_CONT_INTERVAL_BMSK                 0xffffffff
#define HWIO_CPR_TIMER_AUTO_CONT_AUTO_CONT_INTERVAL_SHFT                        0x0

#define HWIO_CPR_TIMER_CLAMP_ADDR(x)                                     ((x) + 0x00000010)
#define HWIO_CPR_TIMER_CLAMP_RMSK                                        0x800000ff
#define HWIO_CPR_TIMER_CLAMP_IN(x)      \
        in_dword_masked(HWIO_CPR_TIMER_CLAMP_ADDR(x), HWIO_CPR_TIMER_CLAMP_RMSK)
#define HWIO_CPR_TIMER_CLAMP_INM(x, m)      \
        in_dword_masked(HWIO_CPR_TIMER_CLAMP_ADDR(x), m)
#define HWIO_CPR_TIMER_CLAMP_OUT(x, v)      \
        out_dword(HWIO_CPR_TIMER_CLAMP_ADDR(x),v)
#define HWIO_CPR_TIMER_CLAMP_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_TIMER_CLAMP_ADDR(x),m,v,HWIO_CPR_TIMER_CLAMP_IN(x))
#define HWIO_CPR_TIMER_CLAMP_CPR_DISABLE_VALID_BMSK                      0x80000000
#define HWIO_CPR_TIMER_CLAMP_CPR_DISABLE_VALID_SHFT                            0x1f
#define HWIO_CPR_TIMER_CLAMP_CLAMP_TIMER_INTERVAL_BMSK                         0xff
#define HWIO_CPR_TIMER_CLAMP_CLAMP_TIMER_INTERVAL_SHFT                          0x0

#define HWIO_CPR_STEP_QUOT_INIT_ADDR(x)                                  ((x) + 0x00000014)
#define HWIO_CPR_STEP_QUOT_INIT_RMSK                                          0xfff
#define HWIO_CPR_STEP_QUOT_INIT_IN(x)      \
        in_dword_masked(HWIO_CPR_STEP_QUOT_INIT_ADDR(x), HWIO_CPR_STEP_QUOT_INIT_RMSK)
#define HWIO_CPR_STEP_QUOT_INIT_INM(x, m)      \
        in_dword_masked(HWIO_CPR_STEP_QUOT_INIT_ADDR(x), m)
#define HWIO_CPR_STEP_QUOT_INIT_OUT(x, v)      \
        out_dword(HWIO_CPR_STEP_QUOT_INIT_ADDR(x),v)
#define HWIO_CPR_STEP_QUOT_INIT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_STEP_QUOT_INIT_ADDR(x),m,v,HWIO_CPR_STEP_QUOT_INIT_IN(x))
#define HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MAX_BMSK                       0xfc0
#define HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MAX_SHFT                         0x6
#define HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MIN_BMSK                        0x3f
#define HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MIN_SHFT                         0x0

#define HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x)                               ((x) + 0x00000020)
#define HWIO_CPR_BIST_CHAIN_CHECK0_RMSK                                  0x80000fff
#define HWIO_CPR_BIST_CHAIN_CHECK0_IN(x)      \
        in_dword_masked(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x), HWIO_CPR_BIST_CHAIN_CHECK0_RMSK)
#define HWIO_CPR_BIST_CHAIN_CHECK0_INM(x, m)      \
        in_dword_masked(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x), m)
#define HWIO_CPR_BIST_CHAIN_CHECK0_OUT(x, v)      \
        out_dword(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x),v)
#define HWIO_CPR_BIST_CHAIN_CHECK0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x),m,v,HWIO_CPR_BIST_CHAIN_CHECK0_IN(x))
#define HWIO_CPR_BIST_CHAIN_CHECK0_EN_CHAIN_CHECK_BMSK                   0x80000000
#define HWIO_CPR_BIST_CHAIN_CHECK0_EN_CHAIN_CHECK_SHFT                         0x1f
#define HWIO_CPR_BIST_CHAIN_CHECK0_SCLK_CNT_EXPECTED_BMSK                     0xfff
#define HWIO_CPR_BIST_CHAIN_CHECK0_SCLK_CNT_EXPECTED_SHFT                       0x0

#define HWIO_CPR_BIST_CHAIN_CHECK1_ADDR(x)                               ((x) + 0x00000024)
#define HWIO_CPR_BIST_CHAIN_CHECK1_RMSK                                  0xe0ffffff
#define HWIO_CPR_BIST_CHAIN_CHECK1_IN(x)      \
        in_dword_masked(HWIO_CPR_BIST_CHAIN_CHECK1_ADDR(x), HWIO_CPR_BIST_CHAIN_CHECK1_RMSK)
#define HWIO_CPR_BIST_CHAIN_CHECK1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_BIST_CHAIN_CHECK1_ADDR(x), m)
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_DONE_BMSK                    0x80000000
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_DONE_SHFT                          0x1f
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_FAIL_BMSK                    0x60000000
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_FAIL_SHFT                          0x1d
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT1_BMSK                          0xfff000
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT1_SHFT                               0xc
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT0_BMSK                             0xfff
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT0_SHFT                               0x0

#define HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x)                                ((x) + 0x00000028)
#define HWIO_CPR_BIST_QUOT_CHECK0_RMSK                                   0x80ffffff
#define HWIO_CPR_BIST_QUOT_CHECK0_IN(x)      \
        in_dword_masked(HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x), HWIO_CPR_BIST_QUOT_CHECK0_RMSK)
#define HWIO_CPR_BIST_QUOT_CHECK0_INM(x, m)      \
        in_dword_masked(HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x), m)
#define HWIO_CPR_BIST_QUOT_CHECK0_OUT(x, v)      \
        out_dword(HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x),v)
#define HWIO_CPR_BIST_QUOT_CHECK0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x),m,v,HWIO_CPR_BIST_QUOT_CHECK0_IN(x))
#define HWIO_CPR_BIST_QUOT_CHECK0_FAIL_BMSK                              0x80000000
#define HWIO_CPR_BIST_QUOT_CHECK0_FAIL_SHFT                                    0x1f
#define HWIO_CPR_BIST_QUOT_CHECK0_QUOT_ABSMAX_BMSK                         0xfff000
#define HWIO_CPR_BIST_QUOT_CHECK0_QUOT_ABSMAX_SHFT                              0xc
#define HWIO_CPR_BIST_QUOT_CHECK0_QUOT_ABSMIN_BMSK                            0xfff
#define HWIO_CPR_BIST_QUOT_CHECK0_QUOT_ABSMIN_SHFT                              0x0

#define HWIO_CPR_BIST_QUOT_CHECK1_ADDR(x)                                ((x) + 0x0000002c)
#define HWIO_CPR_BIST_QUOT_CHECK1_RMSK                                   0xffffffff
#define HWIO_CPR_BIST_QUOT_CHECK1_IN(x)      \
        in_dword_masked(HWIO_CPR_BIST_QUOT_CHECK1_ADDR(x), HWIO_CPR_BIST_QUOT_CHECK1_RMSK)
#define HWIO_CPR_BIST_QUOT_CHECK1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_BIST_QUOT_CHECK1_ADDR(x), m)
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MAX_BMSK                      0xf0000000
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MAX_SHFT                            0x1c
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MIN_BMSK                       0xf000000
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MIN_SHFT                            0x18
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MAX_BMSK                       0xfff000
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MAX_SHFT                            0xc
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MIN_BMSK                          0xfff
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MIN_SHFT                            0x0

#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_ADDR(base,n)                        ((base) + 0x00000030 + 0x4 * (n))
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_RMSK                                0xffffffff
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_MAXn                                         2
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_INI(base,n)        \
        in_dword_masked(HWIO_CPR_BIST_FAIL_MAP_MIN_n_ADDR(base,n), HWIO_CPR_BIST_FAIL_MAP_MIN_n_RMSK)
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_BIST_FAIL_MAP_MIN_n_ADDR(base,n), mask)
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_BIST_FAIL_MAP_MIN_BMSK              0xffffffff
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_BIST_FAIL_MAP_MIN_SHFT                     0x0

#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_ADDR(base,n)                        ((base) + 0x00000050 + 0x4 * (n))
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_RMSK                                0xffffffff
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_MAXn                                         2
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_INI(base,n)        \
        in_dword_masked(HWIO_CPR_BIST_FAIL_MAP_MAX_n_ADDR(base,n), HWIO_CPR_BIST_FAIL_MAP_MAX_n_RMSK)
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_BIST_FAIL_MAP_MAX_n_ADDR(base,n), mask)
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_BIST_FAIL_MAP_MAX_BMSK              0xffffffff
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_BIST_FAIL_MAP_MAX_SHFT                     0x0

#define HWIO_CPR_DEBUG_ADDR(x)                                           ((x) + 0x00000070)
#define HWIO_CPR_DEBUG_RMSK                                                    0x3f
#define HWIO_CPR_DEBUG_IN(x)      \
        in_dword_masked(HWIO_CPR_DEBUG_ADDR(x), HWIO_CPR_DEBUG_RMSK)
#define HWIO_CPR_DEBUG_INM(x, m)      \
        in_dword_masked(HWIO_CPR_DEBUG_ADDR(x), m)
#define HWIO_CPR_DEBUG_OUT(x, v)      \
        out_dword(HWIO_CPR_DEBUG_ADDR(x),v)
#define HWIO_CPR_DEBUG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_DEBUG_ADDR(x),m,v,HWIO_CPR_DEBUG_IN(x))
#define HWIO_CPR_DEBUG_CPR_LOG_THREAD_SEL_BMSK                                 0x38
#define HWIO_CPR_DEBUG_CPR_LOG_THREAD_SEL_SHFT                                  0x3
#define HWIO_CPR_DEBUG_DEBUG_BUS_SEL_BMSK                                       0x7
#define HWIO_CPR_DEBUG_DEBUG_BUS_SEL_SHFT                                       0x0

#define HWIO_CPR_DEBUG_LOG_n_ADDR(base,n)                                ((base) + 0x00000080 + 0x4 * (n))
#define HWIO_CPR_DEBUG_LOG_n_RMSK                                        0x3fff000e
#define HWIO_CPR_DEBUG_LOG_n_MAXn                                                 7
#define HWIO_CPR_DEBUG_LOG_n_INI(base,n)        \
        in_dword_masked(HWIO_CPR_DEBUG_LOG_n_ADDR(base,n), HWIO_CPR_DEBUG_LOG_n_RMSK)
#define HWIO_CPR_DEBUG_LOG_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_DEBUG_LOG_n_ADDR(base,n), mask)
#define HWIO_CPR_DEBUG_LOG_n_MID_COUNT_BMSK                              0x3fff0000
#define HWIO_CPR_DEBUG_LOG_n_MID_COUNT_SHFT                                    0x10
#define HWIO_CPR_DEBUG_LOG_n_UP_FLAG_BMSK                                       0x8
#define HWIO_CPR_DEBUG_LOG_n_UP_FLAG_SHFT                                       0x3
#define HWIO_CPR_DEBUG_LOG_n_MID_FLAG_BMSK                                      0x4
#define HWIO_CPR_DEBUG_LOG_n_MID_FLAG_SHFT                                      0x2
#define HWIO_CPR_DEBUG_LOG_n_DOWN_FLAG_BMSK                                     0x2
#define HWIO_CPR_DEBUG_LOG_n_DOWN_FLAG_SHFT                                     0x1

#define HWIO_CPR_GCNT0_ADDR(x)                                           ((x) + 0x000000a0)
#define HWIO_CPR_GCNT0_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT0_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT0_ADDR(x), HWIO_CPR_GCNT0_RMSK)
#define HWIO_CPR_GCNT0_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT0_ADDR(x), m)
#define HWIO_CPR_GCNT0_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT0_ADDR(x),v)
#define HWIO_CPR_GCNT0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT0_ADDR(x),m,v,HWIO_CPR_GCNT0_IN(x))
#define HWIO_CPR_GCNT0_GCNT0_BMSK                                             0x3ff
#define HWIO_CPR_GCNT0_GCNT0_SHFT                                               0x0

#define HWIO_CPR_GCNT1_ADDR(x)                                           ((x) + 0x000000a4)
#define HWIO_CPR_GCNT1_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT1_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT1_ADDR(x), HWIO_CPR_GCNT1_RMSK)
#define HWIO_CPR_GCNT1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT1_ADDR(x), m)
#define HWIO_CPR_GCNT1_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT1_ADDR(x),v)
#define HWIO_CPR_GCNT1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT1_ADDR(x),m,v,HWIO_CPR_GCNT1_IN(x))
#define HWIO_CPR_GCNT1_GCNT1_BMSK                                             0x3ff
#define HWIO_CPR_GCNT1_GCNT1_SHFT                                               0x0

#define HWIO_CPR_GCNT2_ADDR(x)                                           ((x) + 0x000000a8)
#define HWIO_CPR_GCNT2_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT2_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT2_ADDR(x), HWIO_CPR_GCNT2_RMSK)
#define HWIO_CPR_GCNT2_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT2_ADDR(x), m)
#define HWIO_CPR_GCNT2_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT2_ADDR(x),v)
#define HWIO_CPR_GCNT2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT2_ADDR(x),m,v,HWIO_CPR_GCNT2_IN(x))
#define HWIO_CPR_GCNT2_GCNT2_BMSK                                             0x3ff
#define HWIO_CPR_GCNT2_GCNT2_SHFT                                               0x0

#define HWIO_CPR_GCNT3_ADDR(x)                                           ((x) + 0x000000ac)
#define HWIO_CPR_GCNT3_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT3_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT3_ADDR(x), HWIO_CPR_GCNT3_RMSK)
#define HWIO_CPR_GCNT3_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT3_ADDR(x), m)
#define HWIO_CPR_GCNT3_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT3_ADDR(x),v)
#define HWIO_CPR_GCNT3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT3_ADDR(x),m,v,HWIO_CPR_GCNT3_IN(x))
#define HWIO_CPR_GCNT3_GCNT3_BMSK                                             0x3ff
#define HWIO_CPR_GCNT3_GCNT3_SHFT                                               0x0

#define HWIO_CPR_GCNT4_ADDR(x)                                           ((x) + 0x000000b0)
#define HWIO_CPR_GCNT4_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT4_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT4_ADDR(x), HWIO_CPR_GCNT4_RMSK)
#define HWIO_CPR_GCNT4_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT4_ADDR(x), m)
#define HWIO_CPR_GCNT4_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT4_ADDR(x),v)
#define HWIO_CPR_GCNT4_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT4_ADDR(x),m,v,HWIO_CPR_GCNT4_IN(x))
#define HWIO_CPR_GCNT4_GCNT4_BMSK                                             0x3ff
#define HWIO_CPR_GCNT4_GCNT4_SHFT                                               0x0

#define HWIO_CPR_GCNT5_ADDR(x)                                           ((x) + 0x000000b4)
#define HWIO_CPR_GCNT5_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT5_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT5_ADDR(x), HWIO_CPR_GCNT5_RMSK)
#define HWIO_CPR_GCNT5_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT5_ADDR(x), m)
#define HWIO_CPR_GCNT5_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT5_ADDR(x),v)
#define HWIO_CPR_GCNT5_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT5_ADDR(x),m,v,HWIO_CPR_GCNT5_IN(x))
#define HWIO_CPR_GCNT5_GCNT5_BMSK                                             0x3ff
#define HWIO_CPR_GCNT5_GCNT5_SHFT                                               0x0

#define HWIO_CPR_GCNT6_ADDR(x)                                           ((x) + 0x000000b8)
#define HWIO_CPR_GCNT6_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT6_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT6_ADDR(x), HWIO_CPR_GCNT6_RMSK)
#define HWIO_CPR_GCNT6_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT6_ADDR(x), m)
#define HWIO_CPR_GCNT6_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT6_ADDR(x),v)
#define HWIO_CPR_GCNT6_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT6_ADDR(x),m,v,HWIO_CPR_GCNT6_IN(x))
#define HWIO_CPR_GCNT6_GCNT6_BMSK                                             0x3ff
#define HWIO_CPR_GCNT6_GCNT6_SHFT                                               0x0

#define HWIO_CPR_GCNT7_ADDR(x)                                           ((x) + 0x000000bc)
#define HWIO_CPR_GCNT7_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT7_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT7_ADDR(x), HWIO_CPR_GCNT7_RMSK)
#define HWIO_CPR_GCNT7_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT7_ADDR(x), m)
#define HWIO_CPR_GCNT7_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT7_ADDR(x),v)
#define HWIO_CPR_GCNT7_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT7_ADDR(x),m,v,HWIO_CPR_GCNT7_IN(x))
#define HWIO_CPR_GCNT7_GCNT7_BMSK                                             0x3ff
#define HWIO_CPR_GCNT7_GCNT7_SHFT                                               0x0

#define HWIO_CPR_GCNT8_ADDR(x)                                           ((x) + 0x000000c0)
#define HWIO_CPR_GCNT8_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT8_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT8_ADDR(x), HWIO_CPR_GCNT8_RMSK)
#define HWIO_CPR_GCNT8_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT8_ADDR(x), m)
#define HWIO_CPR_GCNT8_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT8_ADDR(x),v)
#define HWIO_CPR_GCNT8_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT8_ADDR(x),m,v,HWIO_CPR_GCNT8_IN(x))
#define HWIO_CPR_GCNT8_GCNT8_BMSK                                             0x3ff
#define HWIO_CPR_GCNT8_GCNT8_SHFT                                               0x0

#define HWIO_CPR_GCNT9_ADDR(x)                                           ((x) + 0x000000c4)
#define HWIO_CPR_GCNT9_RMSK                                                   0x3ff
#define HWIO_CPR_GCNT9_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT9_ADDR(x), HWIO_CPR_GCNT9_RMSK)
#define HWIO_CPR_GCNT9_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT9_ADDR(x), m)
#define HWIO_CPR_GCNT9_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT9_ADDR(x),v)
#define HWIO_CPR_GCNT9_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT9_ADDR(x),m,v,HWIO_CPR_GCNT9_IN(x))
#define HWIO_CPR_GCNT9_GCNT9_BMSK                                             0x3ff
#define HWIO_CPR_GCNT9_GCNT9_SHFT                                               0x0

#define HWIO_CPR_GCNT10_ADDR(x)                                          ((x) + 0x000000c8)
#define HWIO_CPR_GCNT10_RMSK                                                  0x3ff
#define HWIO_CPR_GCNT10_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT10_ADDR(x), HWIO_CPR_GCNT10_RMSK)
#define HWIO_CPR_GCNT10_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT10_ADDR(x), m)
#define HWIO_CPR_GCNT10_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT10_ADDR(x),v)
#define HWIO_CPR_GCNT10_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT10_ADDR(x),m,v,HWIO_CPR_GCNT10_IN(x))
#define HWIO_CPR_GCNT10_GCNT10_BMSK                                           0x3ff
#define HWIO_CPR_GCNT10_GCNT10_SHFT                                             0x0

#define HWIO_CPR_GCNT11_ADDR(x)                                          ((x) + 0x000000cc)
#define HWIO_CPR_GCNT11_RMSK                                                  0x3ff
#define HWIO_CPR_GCNT11_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT11_ADDR(x), HWIO_CPR_GCNT11_RMSK)
#define HWIO_CPR_GCNT11_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT11_ADDR(x), m)
#define HWIO_CPR_GCNT11_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT11_ADDR(x),v)
#define HWIO_CPR_GCNT11_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT11_ADDR(x),m,v,HWIO_CPR_GCNT11_IN(x))
#define HWIO_CPR_GCNT11_GCNT11_BMSK                                           0x3ff
#define HWIO_CPR_GCNT11_GCNT11_SHFT                                             0x0

#define HWIO_CPR_GCNT12_ADDR(x)                                          ((x) + 0x000000d0)
#define HWIO_CPR_GCNT12_RMSK                                                  0x3ff
#define HWIO_CPR_GCNT12_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT12_ADDR(x), HWIO_CPR_GCNT12_RMSK)
#define HWIO_CPR_GCNT12_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT12_ADDR(x), m)
#define HWIO_CPR_GCNT12_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT12_ADDR(x),v)
#define HWIO_CPR_GCNT12_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT12_ADDR(x),m,v,HWIO_CPR_GCNT12_IN(x))
#define HWIO_CPR_GCNT12_GCNT12_BMSK                                           0x3ff
#define HWIO_CPR_GCNT12_GCNT12_SHFT                                             0x0

#define HWIO_CPR_GCNT13_ADDR(x)                                          ((x) + 0x000000d4)
#define HWIO_CPR_GCNT13_RMSK                                                  0x3ff
#define HWIO_CPR_GCNT13_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT13_ADDR(x), HWIO_CPR_GCNT13_RMSK)
#define HWIO_CPR_GCNT13_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT13_ADDR(x), m)
#define HWIO_CPR_GCNT13_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT13_ADDR(x),v)
#define HWIO_CPR_GCNT13_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT13_ADDR(x),m,v,HWIO_CPR_GCNT13_IN(x))
#define HWIO_CPR_GCNT13_GCNT13_BMSK                                           0x3ff
#define HWIO_CPR_GCNT13_GCNT13_SHFT                                             0x0

#define HWIO_CPR_GCNT14_ADDR(x)                                          ((x) + 0x000000d8)
#define HWIO_CPR_GCNT14_RMSK                                                  0x3ff
#define HWIO_CPR_GCNT14_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT14_ADDR(x), HWIO_CPR_GCNT14_RMSK)
#define HWIO_CPR_GCNT14_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT14_ADDR(x), m)
#define HWIO_CPR_GCNT14_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT14_ADDR(x),v)
#define HWIO_CPR_GCNT14_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT14_ADDR(x),m,v,HWIO_CPR_GCNT14_IN(x))
#define HWIO_CPR_GCNT14_GCNT14_BMSK                                           0x3ff
#define HWIO_CPR_GCNT14_GCNT14_SHFT                                             0x0

#define HWIO_CPR_GCNT15_ADDR(x)                                          ((x) + 0x000000dc)
#define HWIO_CPR_GCNT15_RMSK                                                  0x3ff
#define HWIO_CPR_GCNT15_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT15_ADDR(x), HWIO_CPR_GCNT15_RMSK)
#define HWIO_CPR_GCNT15_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT15_ADDR(x), m)
#define HWIO_CPR_GCNT15_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT15_ADDR(x),v)
#define HWIO_CPR_GCNT15_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT15_ADDR(x),m,v,HWIO_CPR_GCNT15_IN(x))
#define HWIO_CPR_GCNT15_GCNT15_BMSK                                           0x3ff
#define HWIO_CPR_GCNT15_GCNT15_SHFT                                             0x0

#define HWIO_CPR_SENSOR_BYPASS_WRITEn_ADDR(base,n)                       ((base) + 0x000000e0 + 0x4 * (n))
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_RMSK                               0xffffffff
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_MAXn                                        2
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_OUTI(base,n,val)    \
        out_dword(HWIO_CPR_SENSOR_BYPASS_WRITEn_ADDR(base,n),val)
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_SENSOR_BYPASS_BMSK                 0xffffffff
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_SENSOR_BYPASS_SHFT                        0x0

#define HWIO_CPR_SENSOR_BYPASS_READn_ADDR(base,n)                        ((base) + 0x00000100 + 0x4 * (n))
#define HWIO_CPR_SENSOR_BYPASS_READn_RMSK                                0xffffffff
#define HWIO_CPR_SENSOR_BYPASS_READn_MAXn                                         2
#define HWIO_CPR_SENSOR_BYPASS_READn_INI(base,n)        \
        in_dword_masked(HWIO_CPR_SENSOR_BYPASS_READn_ADDR(base,n), HWIO_CPR_SENSOR_BYPASS_READn_RMSK)
#define HWIO_CPR_SENSOR_BYPASS_READn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_SENSOR_BYPASS_READn_ADDR(base,n), mask)
#define HWIO_CPR_SENSOR_BYPASS_READn_SENSOR_BYPASS_BMSK                  0xffffffff
#define HWIO_CPR_SENSOR_BYPASS_READn_SENSOR_BYPASS_SHFT                         0x0

#define HWIO_CPR_SENSOR_MASK_WRITEn_ADDR(base,n)                         ((base) + 0x00000120 + 0x4 * (n))
#define HWIO_CPR_SENSOR_MASK_WRITEn_RMSK                                 0xffffffff
#define HWIO_CPR_SENSOR_MASK_WRITEn_MAXn                                          2
#define HWIO_CPR_SENSOR_MASK_WRITEn_OUTI(base,n,val)    \
        out_dword(HWIO_CPR_SENSOR_MASK_WRITEn_ADDR(base,n),val)
#define HWIO_CPR_SENSOR_MASK_WRITEn_SENSOR_MASK_BMSK                     0xffffffff
#define HWIO_CPR_SENSOR_MASK_WRITEn_SENSOR_MASK_SHFT                            0x0

#define HWIO_CPR_SENSOR_MASK_READn_ADDR(base,n)                          ((base) + 0x00000140 + 0x4 * (n))
#define HWIO_CPR_SENSOR_MASK_READn_RMSK                                  0xffffffff
#define HWIO_CPR_SENSOR_MASK_READn_MAXn                                           2
#define HWIO_CPR_SENSOR_MASK_READn_INI(base,n)        \
        in_dword_masked(HWIO_CPR_SENSOR_MASK_READn_ADDR(base,n), HWIO_CPR_SENSOR_MASK_READn_RMSK)
#define HWIO_CPR_SENSOR_MASK_READn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_SENSOR_MASK_READn_ADDR(base,n), mask)
#define HWIO_CPR_SENSOR_MASK_READn_SENSOR_MASK_BMSK                      0xffffffff
#define HWIO_CPR_SENSOR_MASK_READn_SENSOR_MASK_SHFT                             0x0

#define HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n)                            ((base) + 0x00000200 + 0x4 * (n))
#define HWIO_CPR_SENSOR_THREAD_n_RMSK                                           0x1
#define HWIO_CPR_SENSOR_THREAD_n_MAXn                                            72
#define HWIO_CPR_SENSOR_THREAD_n_INI(base,n)        \
        in_dword_masked(HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n), HWIO_CPR_SENSOR_THREAD_n_RMSK)
#define HWIO_CPR_SENSOR_THREAD_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n), mask)
#define HWIO_CPR_SENSOR_THREAD_n_OUTI(base,n,val)    \
        out_dword(HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n),val)
#define HWIO_CPR_SENSOR_THREAD_n_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n),mask,val,HWIO_CPR_SENSOR_THREAD_n_INI(base,n))
#define HWIO_CPR_SENSOR_THREAD_n_THREAD_ID_BMSK                                 0x1
#define HWIO_CPR_SENSOR_THREAD_n_THREAD_ID_SHFT                                 0x0

#define HWIO_CPR_CONT_CMD_t_ADDR(base,t)                                 ((base) + 0x00000800 + 0x440 * (t))
#define HWIO_CPR_CONT_CMD_t_RMSK                                                0x1
#define HWIO_CPR_CONT_CMD_t_MAXt                                                  0
#define HWIO_CPR_CONT_CMD_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_CONT_CMD_t_ADDR(base,t),val)
#define HWIO_CPR_CONT_CMD_t_VDD_CHANGED_ONE_STEP_BMSK                           0x1
#define HWIO_CPR_CONT_CMD_t_VDD_CHANGED_ONE_STEP_SHFT                           0x0

#define HWIO_CPR_THRESHOLD_t_ADDR(base,t)                                ((base) + 0x00000808 + 0x440 * (t))
#define HWIO_CPR_THRESHOLD_t_RMSK                                           0x3ffff
#define HWIO_CPR_THRESHOLD_t_MAXt                                                 0
#define HWIO_CPR_THRESHOLD_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_THRESHOLD_t_ADDR(base,t), HWIO_CPR_THRESHOLD_t_RMSK)
#define HWIO_CPR_THRESHOLD_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_THRESHOLD_t_ADDR(base,t), mask)
#define HWIO_CPR_THRESHOLD_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_THRESHOLD_t_ADDR(base,t),val)
#define HWIO_CPR_THRESHOLD_t_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_CPR_THRESHOLD_t_ADDR(base,t),mask,val,HWIO_CPR_THRESHOLD_t_INI(base,t))
#define HWIO_CPR_THRESHOLD_t_UP_THRESHOLD_BMSK                              0x3e000
#define HWIO_CPR_THRESHOLD_t_UP_THRESHOLD_SHFT                                  0xd
#define HWIO_CPR_THRESHOLD_t_DN_THRESHOLD_BMSK                               0x1f00
#define HWIO_CPR_THRESHOLD_t_DN_THRESHOLD_SHFT                                  0x8
#define HWIO_CPR_THRESHOLD_t_CONSECUTIVE_UP_BMSK                               0xf0
#define HWIO_CPR_THRESHOLD_t_CONSECUTIVE_UP_SHFT                                0x4
#define HWIO_CPR_THRESHOLD_t_CONSECUTIVE_DN_BMSK                                0xf
#define HWIO_CPR_THRESHOLD_t_CONSECUTIVE_DN_SHFT                                0x0

#define HWIO_CPR_MASK_THREAD_t_ADDR(base,t)                              ((base) + 0x0000080c + 0x440 * (t))
#define HWIO_CPR_MASK_THREAD_t_RMSK                                      0x8000ffff
#define HWIO_CPR_MASK_THREAD_t_MAXt                                               0
#define HWIO_CPR_MASK_THREAD_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_MASK_THREAD_t_ADDR(base,t), HWIO_CPR_MASK_THREAD_t_RMSK)
#define HWIO_CPR_MASK_THREAD_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_MASK_THREAD_t_ADDR(base,t), mask)
#define HWIO_CPR_MASK_THREAD_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_MASK_THREAD_t_ADDR(base,t),val)
#define HWIO_CPR_MASK_THREAD_t_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_CPR_MASK_THREAD_t_ADDR(base,t),mask,val,HWIO_CPR_MASK_THREAD_t_INI(base,t))
#define HWIO_CPR_MASK_THREAD_t_DISABLE_THREAD_BMSK                       0x80000000
#define HWIO_CPR_MASK_THREAD_t_DISABLE_THREAD_SHFT                             0x1f
#define HWIO_CPR_MASK_THREAD_t_RO_MASK4THREAD_BMSK                           0xffff
#define HWIO_CPR_MASK_THREAD_t_RO_MASK4THREAD_SHFT                              0x0

#define HWIO_CPR_RESULT0_t_ADDR(base,t)                                  ((base) + 0x00000810 + 0x440 * (t))
#define HWIO_CPR_RESULT0_t_RMSK                                            0x1fffff
#define HWIO_CPR_RESULT0_t_MAXt                                                   0
#define HWIO_CPR_RESULT0_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_RESULT0_t_ADDR(base,t), HWIO_CPR_RESULT0_t_RMSK)
#define HWIO_CPR_RESULT0_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_RESULT0_t_ADDR(base,t), mask)
#define HWIO_CPR_RESULT0_t_ERROR_LT_0_BMSK                                 0x100000
#define HWIO_CPR_RESULT0_t_ERROR_LT_0_SHFT                                     0x14
#define HWIO_CPR_RESULT0_t_ERROR_BMSK                                       0xfff00
#define HWIO_CPR_RESULT0_t_ERROR_SHFT                                           0x8
#define HWIO_CPR_RESULT0_t_ERROR_STEPS_BMSK                                    0xf8
#define HWIO_CPR_RESULT0_t_ERROR_STEPS_SHFT                                     0x3
#define HWIO_CPR_RESULT0_t_STEP_UP_BMSK                                         0x4
#define HWIO_CPR_RESULT0_t_STEP_UP_SHFT                                         0x2
#define HWIO_CPR_RESULT0_t_STEP_DN_BMSK                                         0x2
#define HWIO_CPR_RESULT0_t_STEP_DN_SHFT                                         0x1
#define HWIO_CPR_RESULT0_t_BUSY_BMSK                                            0x1
#define HWIO_CPR_RESULT0_t_BUSY_SHFT                                            0x0

#define HWIO_CPR_RESULT1_t_ADDR(base,t)                                  ((base) + 0x00000814 + 0x440 * (t))
#define HWIO_CPR_RESULT1_t_RMSK                                          0xffffffff
#define HWIO_CPR_RESULT1_t_MAXt                                                   0
#define HWIO_CPR_RESULT1_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_RESULT1_t_ADDR(base,t), HWIO_CPR_RESULT1_t_RMSK)
#define HWIO_CPR_RESULT1_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_RESULT1_t_ADDR(base,t), mask)
#define HWIO_CPR_RESULT1_t_SEL_MAX_BMSK                                  0xf0000000
#define HWIO_CPR_RESULT1_t_SEL_MAX_SHFT                                        0x1c
#define HWIO_CPR_RESULT1_t_SEL_MIN_BMSK                                   0xf000000
#define HWIO_CPR_RESULT1_t_SEL_MIN_SHFT                                        0x18
#define HWIO_CPR_RESULT1_t_QUOT_MAX_BMSK                                   0xfff000
#define HWIO_CPR_RESULT1_t_QUOT_MAX_SHFT                                        0xc
#define HWIO_CPR_RESULT1_t_QUOT_MIN_BMSK                                      0xfff
#define HWIO_CPR_RESULT1_t_QUOT_MIN_SHFT                                        0x0

#define HWIO_CPR_RESULT2_t_ADDR(base,t)                                  ((base) + 0x00000818 + 0x440 * (t))
#define HWIO_CPR_RESULT2_t_RMSK                                          0xffff0fff
#define HWIO_CPR_RESULT2_t_MAXt                                                   0
#define HWIO_CPR_RESULT2_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_RESULT2_t_ADDR(base,t), HWIO_CPR_RESULT2_t_RMSK)
#define HWIO_CPR_RESULT2_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_RESULT2_t_ADDR(base,t), mask)
#define HWIO_CPR_RESULT2_t_SENSOR_MAX_BMSK                               0xff000000
#define HWIO_CPR_RESULT2_t_SENSOR_MAX_SHFT                                     0x18
#define HWIO_CPR_RESULT2_t_SENSOR_MIN_BMSK                                 0xff0000
#define HWIO_CPR_RESULT2_t_SENSOR_MIN_SHFT                                     0x10
#define HWIO_CPR_RESULT2_t_STEP_QUOT_MAX_BMSK                                 0xfc0
#define HWIO_CPR_RESULT2_t_STEP_QUOT_MAX_SHFT                                   0x6
#define HWIO_CPR_RESULT2_t_STEP_QUOT_MIN_BMSK                                  0x3f
#define HWIO_CPR_RESULT2_t_STEP_QUOT_MIN_SHFT                                   0x0

#define HWIO_CPR_IRQ_EN_t_ADDR(base,t)                                   ((base) + 0x0000081c + 0x440 * (t))
#define HWIO_CPR_IRQ_EN_t_RMSK                                           0x8000000f
#define HWIO_CPR_IRQ_EN_t_MAXt                                                    0
#define HWIO_CPR_IRQ_EN_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_IRQ_EN_t_ADDR(base,t), HWIO_CPR_IRQ_EN_t_RMSK)
#define HWIO_CPR_IRQ_EN_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_IRQ_EN_t_ADDR(base,t), mask)
#define HWIO_CPR_IRQ_EN_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_IRQ_EN_t_ADDR(base,t),val)
#define HWIO_CPR_IRQ_EN_t_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_CPR_IRQ_EN_t_ADDR(base,t),mask,val,HWIO_CPR_IRQ_EN_t_INI(base,t))
#define HWIO_CPR_IRQ_EN_t_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                0x80000000
#define HWIO_CPR_IRQ_EN_t_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                      0x1f
#define HWIO_CPR_IRQ_EN_t_UP_FLAG_EN_BMSK                                       0x8
#define HWIO_CPR_IRQ_EN_t_UP_FLAG_EN_SHFT                                       0x3
#define HWIO_CPR_IRQ_EN_t_MID_FLAG_EN_BMSK                                      0x4
#define HWIO_CPR_IRQ_EN_t_MID_FLAG_EN_SHFT                                      0x2
#define HWIO_CPR_IRQ_EN_t_DOWN_FLAG_EN_BMSK                                     0x2
#define HWIO_CPR_IRQ_EN_t_DOWN_FLAG_EN_SHFT                                     0x1
#define HWIO_CPR_IRQ_EN_t_CPR_DONE_EN_BMSK                                      0x1
#define HWIO_CPR_IRQ_EN_t_CPR_DONE_EN_SHFT                                      0x0

#define HWIO_CPR_IRQ_CLEAR_t_ADDR(base,t)                                ((base) + 0x00000820 + 0x440 * (t))
#define HWIO_CPR_IRQ_CLEAR_t_RMSK                                        0x8000000f
#define HWIO_CPR_IRQ_CLEAR_t_MAXt                                                 0
#define HWIO_CPR_IRQ_CLEAR_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_IRQ_CLEAR_t_ADDR(base,t),val)
#define HWIO_CPR_IRQ_CLEAR_t_CLAMP_CHANGE_WHILE_BUSY_CLEAR_BMSK          0x80000000
#define HWIO_CPR_IRQ_CLEAR_t_CLAMP_CHANGE_WHILE_BUSY_CLEAR_SHFT                0x1f
#define HWIO_CPR_IRQ_CLEAR_t_UP_FLAG_CLEAR_BMSK                                 0x8
#define HWIO_CPR_IRQ_CLEAR_t_UP_FLAG_CLEAR_SHFT                                 0x3
#define HWIO_CPR_IRQ_CLEAR_t_MID_FLAG_CLEAR_BMSK                                0x4
#define HWIO_CPR_IRQ_CLEAR_t_MID_FLAG_CLEAR_SHFT                                0x2
#define HWIO_CPR_IRQ_CLEAR_t_DOWN_FLAG_CLEAR_BMSK                               0x2
#define HWIO_CPR_IRQ_CLEAR_t_DOWN_FLAG_CLEAR_SHFT                               0x1
#define HWIO_CPR_IRQ_CLEAR_t_CPR_DONE_CLEAR_BMSK                                0x1
#define HWIO_CPR_IRQ_CLEAR_t_CPR_DONE_CLEAR_SHFT                                0x0

#define HWIO_CPR_IRQ_STATUS_t_ADDR(base,t)                               ((base) + 0x00000824 + 0x440 * (t))
#define HWIO_CPR_IRQ_STATUS_t_RMSK                                       0x8000000f
#define HWIO_CPR_IRQ_STATUS_t_MAXt                                                0
#define HWIO_CPR_IRQ_STATUS_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_IRQ_STATUS_t_ADDR(base,t), HWIO_CPR_IRQ_STATUS_t_RMSK)
#define HWIO_CPR_IRQ_STATUS_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_IRQ_STATUS_t_ADDR(base,t), mask)
#define HWIO_CPR_IRQ_STATUS_t_CLAMP_CHANGE_WHILE_BUSY_BMSK               0x80000000
#define HWIO_CPR_IRQ_STATUS_t_CLAMP_CHANGE_WHILE_BUSY_SHFT                     0x1f
#define HWIO_CPR_IRQ_STATUS_t_UP_FLAG_BMSK                                      0x8
#define HWIO_CPR_IRQ_STATUS_t_UP_FLAG_SHFT                                      0x3
#define HWIO_CPR_IRQ_STATUS_t_MID_FLAG_BMSK                                     0x4
#define HWIO_CPR_IRQ_STATUS_t_MID_FLAG_SHFT                                     0x2
#define HWIO_CPR_IRQ_STATUS_t_DOWN_FLAG_BMSK                                    0x2
#define HWIO_CPR_IRQ_STATUS_t_DOWN_FLAG_SHFT                                    0x1
#define HWIO_CPR_IRQ_STATUS_t_CPR_DONE_BMSK                                     0x1
#define HWIO_CPR_IRQ_STATUS_t_CPR_DONE_SHFT                                     0x0

#define HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t)                       ((base) + 0x00000828 + 0x440 * (t))
#define HWIO_CPR_TARGET_MODE_SELECT_t_RMSK                                      0xf
#define HWIO_CPR_TARGET_MODE_SELECT_t_MAXt                                        0
#define HWIO_CPR_TARGET_MODE_SELECT_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t), HWIO_CPR_TARGET_MODE_SELECT_t_RMSK)
#define HWIO_CPR_TARGET_MODE_SELECT_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t), mask)
#define HWIO_CPR_TARGET_MODE_SELECT_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t),val)
#define HWIO_CPR_TARGET_MODE_SELECT_t_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t),mask,val,HWIO_CPR_TARGET_MODE_SELECT_t_INI(base,t))
#define HWIO_CPR_TARGET_MODE_SELECT_t_TARGET_MODE_SELECT_BMSK                   0xf
#define HWIO_CPR_TARGET_MODE_SELECT_t_TARGET_MODE_SELECT_SHFT                   0x0

#define HWIO_CPR_TARGET0_t_m_ADDR(base,t,m)                              ((base) + 0x00000840 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET0_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET0_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET0_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET0_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET0_t_m_ADDR(base,t,m), HWIO_CPR_TARGET0_t_m_RMSK)
#define HWIO_CPR_TARGET0_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET0_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET0_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET0_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET0_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET0_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET0_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET0_t_m_TARGET0_BMSK                                     0xfff
#define HWIO_CPR_TARGET0_t_m_TARGET0_SHFT                                       0x0

#define HWIO_CPR_TARGET1_t_m_ADDR(base,t,m)                              ((base) + 0x00000844 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET1_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET1_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET1_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET1_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET1_t_m_ADDR(base,t,m), HWIO_CPR_TARGET1_t_m_RMSK)
#define HWIO_CPR_TARGET1_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET1_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET1_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET1_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET1_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET1_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET1_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET1_t_m_TARGET1_BMSK                                     0xfff
#define HWIO_CPR_TARGET1_t_m_TARGET1_SHFT                                       0x0

#define HWIO_CPR_TARGET2_t_m_ADDR(base,t,m)                              ((base) + 0x00000848 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET2_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET2_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET2_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET2_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET2_t_m_ADDR(base,t,m), HWIO_CPR_TARGET2_t_m_RMSK)
#define HWIO_CPR_TARGET2_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET2_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET2_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET2_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET2_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET2_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET2_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET2_t_m_TARGET2_BMSK                                     0xfff
#define HWIO_CPR_TARGET2_t_m_TARGET2_SHFT                                       0x0

#define HWIO_CPR_TARGET3_t_m_ADDR(base,t,m)                              ((base) + 0x0000084c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET3_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET3_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET3_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET3_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET3_t_m_ADDR(base,t,m), HWIO_CPR_TARGET3_t_m_RMSK)
#define HWIO_CPR_TARGET3_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET3_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET3_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET3_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET3_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET3_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET3_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET3_t_m_TARGET3_BMSK                                     0xfff
#define HWIO_CPR_TARGET3_t_m_TARGET3_SHFT                                       0x0

#define HWIO_CPR_TARGET4_t_m_ADDR(base,t,m)                              ((base) + 0x00000850 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET4_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET4_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET4_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET4_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET4_t_m_ADDR(base,t,m), HWIO_CPR_TARGET4_t_m_RMSK)
#define HWIO_CPR_TARGET4_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET4_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET4_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET4_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET4_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET4_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET4_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET4_t_m_TARGET4_BMSK                                     0xfff
#define HWIO_CPR_TARGET4_t_m_TARGET4_SHFT                                       0x0

#define HWIO_CPR_TARGET5_t_m_ADDR(base,t,m)                              ((base) + 0x00000854 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET5_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET5_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET5_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET5_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET5_t_m_ADDR(base,t,m), HWIO_CPR_TARGET5_t_m_RMSK)
#define HWIO_CPR_TARGET5_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET5_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET5_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET5_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET5_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET5_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET5_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET5_t_m_TARGET5_BMSK                                     0xfff
#define HWIO_CPR_TARGET5_t_m_TARGET5_SHFT                                       0x0

#define HWIO_CPR_TARGET6_t_m_ADDR(base,t,m)                              ((base) + 0x00000858 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET6_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET6_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET6_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET6_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET6_t_m_ADDR(base,t,m), HWIO_CPR_TARGET6_t_m_RMSK)
#define HWIO_CPR_TARGET6_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET6_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET6_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET6_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET6_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET6_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET6_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET6_t_m_TARGET6_BMSK                                     0xfff
#define HWIO_CPR_TARGET6_t_m_TARGET6_SHFT                                       0x0

#define HWIO_CPR_TARGET7_t_m_ADDR(base,t,m)                              ((base) + 0x0000085c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET7_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET7_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET7_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET7_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET7_t_m_ADDR(base,t,m), HWIO_CPR_TARGET7_t_m_RMSK)
#define HWIO_CPR_TARGET7_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET7_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET7_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET7_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET7_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET7_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET7_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET7_t_m_TARGET7_BMSK                                     0xfff
#define HWIO_CPR_TARGET7_t_m_TARGET7_SHFT                                       0x0

#define HWIO_CPR_TARGET8_t_m_ADDR(base,t,m)                              ((base) + 0x00000860 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET8_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET8_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET8_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET8_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET8_t_m_ADDR(base,t,m), HWIO_CPR_TARGET8_t_m_RMSK)
#define HWIO_CPR_TARGET8_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET8_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET8_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET8_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET8_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET8_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET8_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET8_t_m_TARGET8_BMSK                                     0xfff
#define HWIO_CPR_TARGET8_t_m_TARGET8_SHFT                                       0x0

#define HWIO_CPR_TARGET9_t_m_ADDR(base,t,m)                              ((base) + 0x00000864 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET9_t_m_RMSK                                             0xfff
#define HWIO_CPR_TARGET9_t_m_MAXt                                                 0
#define HWIO_CPR_TARGET9_t_m_MAXm                                                 3
#define HWIO_CPR_TARGET9_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET9_t_m_ADDR(base,t,m), HWIO_CPR_TARGET9_t_m_RMSK)
#define HWIO_CPR_TARGET9_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET9_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET9_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET9_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET9_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET9_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET9_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET9_t_m_TARGET9_BMSK                                     0xfff
#define HWIO_CPR_TARGET9_t_m_TARGET9_SHFT                                       0x0

#define HWIO_CPR_TARGET10_t_m_ADDR(base,t,m)                             ((base) + 0x00000868 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET10_t_m_RMSK                                            0xfff
#define HWIO_CPR_TARGET10_t_m_MAXt                                                0
#define HWIO_CPR_TARGET10_t_m_MAXm                                                3
#define HWIO_CPR_TARGET10_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET10_t_m_ADDR(base,t,m), HWIO_CPR_TARGET10_t_m_RMSK)
#define HWIO_CPR_TARGET10_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET10_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET10_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET10_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET10_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET10_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET10_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET10_t_m_TARGET10_BMSK                                   0xfff
#define HWIO_CPR_TARGET10_t_m_TARGET10_SHFT                                     0x0

#define HWIO_CPR_TARGET11_t_m_ADDR(base,t,m)                             ((base) + 0x0000086c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET11_t_m_RMSK                                            0xfff
#define HWIO_CPR_TARGET11_t_m_MAXt                                                0
#define HWIO_CPR_TARGET11_t_m_MAXm                                                3
#define HWIO_CPR_TARGET11_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET11_t_m_ADDR(base,t,m), HWIO_CPR_TARGET11_t_m_RMSK)
#define HWIO_CPR_TARGET11_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET11_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET11_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET11_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET11_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET11_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET11_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET11_t_m_TARGET11_BMSK                                   0xfff
#define HWIO_CPR_TARGET11_t_m_TARGET11_SHFT                                     0x0

#define HWIO_CPR_TARGET12_t_m_ADDR(base,t,m)                             ((base) + 0x00000870 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET12_t_m_RMSK                                            0xfff
#define HWIO_CPR_TARGET12_t_m_MAXt                                                0
#define HWIO_CPR_TARGET12_t_m_MAXm                                                3
#define HWIO_CPR_TARGET12_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET12_t_m_ADDR(base,t,m), HWIO_CPR_TARGET12_t_m_RMSK)
#define HWIO_CPR_TARGET12_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET12_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET12_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET12_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET12_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET12_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET12_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET12_t_m_TARGET12_BMSK                                   0xfff
#define HWIO_CPR_TARGET12_t_m_TARGET12_SHFT                                     0x0

#define HWIO_CPR_TARGET13_t_m_ADDR(base,t,m)                             ((base) + 0x00000874 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET13_t_m_RMSK                                            0xfff
#define HWIO_CPR_TARGET13_t_m_MAXt                                                0
#define HWIO_CPR_TARGET13_t_m_MAXm                                                3
#define HWIO_CPR_TARGET13_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET13_t_m_ADDR(base,t,m), HWIO_CPR_TARGET13_t_m_RMSK)
#define HWIO_CPR_TARGET13_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET13_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET13_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET13_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET13_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET13_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET13_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET13_t_m_TARGET13_BMSK                                   0xfff
#define HWIO_CPR_TARGET13_t_m_TARGET13_SHFT                                     0x0

#define HWIO_CPR_TARGET14_t_m_ADDR(base,t,m)                             ((base) + 0x00000878 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET14_t_m_RMSK                                            0xfff
#define HWIO_CPR_TARGET14_t_m_MAXt                                                0
#define HWIO_CPR_TARGET14_t_m_MAXm                                                3
#define HWIO_CPR_TARGET14_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET14_t_m_ADDR(base,t,m), HWIO_CPR_TARGET14_t_m_RMSK)
#define HWIO_CPR_TARGET14_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET14_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET14_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET14_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET14_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET14_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET14_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET14_t_m_TARGET14_BMSK                                   0xfff
#define HWIO_CPR_TARGET14_t_m_TARGET14_SHFT                                     0x0

#define HWIO_CPR_TARGET15_t_m_ADDR(base,t,m)                             ((base) + 0x0000087c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET15_t_m_RMSK                                            0xfff
#define HWIO_CPR_TARGET15_t_m_MAXt                                                0
#define HWIO_CPR_TARGET15_t_m_MAXm                                                3
#define HWIO_CPR_TARGET15_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET15_t_m_ADDR(base,t,m), HWIO_CPR_TARGET15_t_m_RMSK)
#define HWIO_CPR_TARGET15_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET15_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET15_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET15_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET15_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET15_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET15_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET15_t_m_TARGET15_BMSK                                   0xfff
#define HWIO_CPR_TARGET15_t_m_TARGET15_SHFT                                     0x0


#endif /* __HAL_CPR_HWIO_H__ */

