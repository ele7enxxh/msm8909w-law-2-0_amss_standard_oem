#ifndef __VMPM_HW_HWIO_H__
#define __VMPM_HW_HWIO_H__
/*
===========================================================================
*/
/**
  @file vmpm_hw_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MPM2_SLP_CNTR

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/vmpm_hw_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/
#include "HALcomdef.h"

extern uint8* vmpm_HwBaseAddr;
#define MPM2_MPM_BASE (uint32)vmpm_HwBaseAddr

/*----------------------------------------------------------------------------
 * MODULE: MPM2_SLP_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_SLP_CNTR_REG_BASE                                    (MPM2_MPM_BASE      + 0x00003000)
#define MPM2_SLP_CNTR_REG_BASE_OFFS                               0x00003000

#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR               (MPM2_SLP_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_OFFS               (MPM2_SLP_CNTR_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK               0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, m)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_BMSK          0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_SHFT                 0x0


#endif /* __VMPM_HW_HWIO_H__ */

