#ifndef __MSMHWIOREG_H__
#define __MSMHWIOREG_H__
/*
===========================================================================
*/
/**
  @file msmhwioreg.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    APCS_KPSS_WDT
    MPM2_PSHOLD
    MPM2_SLP_CNTR

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/bsp/msmhwioreg.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

//#define KPSS_BASE 0xf9000000

/*----------------------------------------------------------------------------
 * MODULE: APCS_KPSS_WDT
 *--------------------------------------------------------------------------*/

#define APCS_KPSS_WDT_REG_BASE                                    (KPSS_BASE      + 0x00017000)

#define HWIO_APCS_WDOG_SECURE_ADDR                                (APCS_KPSS_WDT_REG_BASE      + 0x00000000)
#define HWIO_APCS_WDOG_SECURE_RMSK                                       0x1
#define HWIO_APCS_WDOG_SECURE_IN          \
        in_dword_masked(HWIO_APCS_WDOG_SECURE_ADDR, HWIO_APCS_WDOG_SECURE_RMSK)
#define HWIO_APCS_WDOG_SECURE_INM(m)      \
        in_dword_masked(HWIO_APCS_WDOG_SECURE_ADDR, m)
#define HWIO_APCS_WDOG_SECURE_OUT(v)      \
        out_dword(HWIO_APCS_WDOG_SECURE_ADDR,v)
#define HWIO_APCS_WDOG_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_WDOG_SECURE_ADDR,m,v,HWIO_APCS_WDOG_SECURE_IN)
#define HWIO_APCS_WDOG_SECURE_WDT_BMSK                                   0x1
#define HWIO_APCS_WDOG_SECURE_WDT_SHFT                                   0x0

#define HWIO_APCS_WDOG_RESET_ADDR                                 (APCS_KPSS_WDT_REG_BASE      + 0x00000004)
#define HWIO_APCS_WDOG_RESET_RMSK                                        0x1
#define HWIO_APCS_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_APCS_WDOG_RESET_ADDR,v)
#define HWIO_APCS_WDOG_RESET_RESET_BMSK                                  0x1
#define HWIO_APCS_WDOG_RESET_RESET_SHFT                                  0x0

#define HWIO_APCS_WDOG_CTL_ADDR                                   (APCS_KPSS_WDT_REG_BASE      + 0x00000008)
#define HWIO_APCS_WDOG_CTL_RMSK                                          0x3
#define HWIO_APCS_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_APCS_WDOG_CTL_ADDR,v)
#define HWIO_APCS_WDOG_CTL_UNMASKED_INT_ENABLE_BMSK                      0x2
#define HWIO_APCS_WDOG_CTL_UNMASKED_INT_ENABLE_SHFT                      0x1
#define HWIO_APCS_WDOG_CTL_ENABLE_BMSK                                   0x1
#define HWIO_APCS_WDOG_CTL_ENABLE_SHFT                                   0x0

#define HWIO_APCS_WDOG_STATUS_ADDR                                (APCS_KPSS_WDT_REG_BASE      + 0x0000000c)
#define HWIO_APCS_WDOG_STATUS_RMSK                                0xffffffff
#define HWIO_APCS_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_APCS_WDOG_STATUS_ADDR, HWIO_APCS_WDOG_STATUS_RMSK)
#define HWIO_APCS_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_APCS_WDOG_STATUS_ADDR, m)
#define HWIO_APCS_WDOG_STATUS_COUNT_BMSK                          0xfffffff8
#define HWIO_APCS_WDOG_STATUS_COUNT_SHFT                                 0x3
#define HWIO_APCS_WDOG_STATUS_CNT_RESET_STATUS_BMSK                      0x4
#define HWIO_APCS_WDOG_STATUS_CNT_RESET_STATUS_SHFT                      0x2
#define HWIO_APCS_WDOG_STATUS_FROZEN_BMSK                                0x2
#define HWIO_APCS_WDOG_STATUS_FROZEN_SHFT                                0x1
#define HWIO_APCS_WDOG_STATUS_EXPIRED_STATUS_BMSK                        0x1
#define HWIO_APCS_WDOG_STATUS_EXPIRED_STATUS_SHFT                        0x0

#define HWIO_APCS_WDOG_BARK_TIME_ADDR                             (APCS_KPSS_WDT_REG_BASE      + 0x00000010)
#define HWIO_APCS_WDOG_BARK_TIME_RMSK                                0xfffff
#define HWIO_APCS_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_APCS_WDOG_BARK_TIME_ADDR, HWIO_APCS_WDOG_BARK_TIME_RMSK)
#define HWIO_APCS_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_APCS_WDOG_BARK_TIME_ADDR, m)
#define HWIO_APCS_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_APCS_WDOG_BARK_TIME_ADDR,v)
#define HWIO_APCS_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_WDOG_BARK_TIME_ADDR,m,v,HWIO_APCS_WDOG_BARK_TIME_IN)
#define HWIO_APCS_WDOG_BARK_TIME_DATA_BMSK                           0xfffff
#define HWIO_APCS_WDOG_BARK_TIME_DATA_SHFT                               0x0

#define HWIO_APCS_WDOG_BITE_TIME_ADDR                             (APCS_KPSS_WDT_REG_BASE      + 0x00000014)
#define HWIO_APCS_WDOG_BITE_TIME_RMSK                                0xfffff
#define HWIO_APCS_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_APCS_WDOG_BITE_TIME_ADDR, HWIO_APCS_WDOG_BITE_TIME_RMSK)
#define HWIO_APCS_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_APCS_WDOG_BITE_TIME_ADDR, m)
#define HWIO_APCS_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_APCS_WDOG_BITE_TIME_ADDR,v)
#define HWIO_APCS_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_WDOG_BITE_TIME_ADDR,m,v,HWIO_APCS_WDOG_BITE_TIME_IN)
#define HWIO_APCS_WDOG_BITE_TIME_DATA_BMSK                           0xfffff
#define HWIO_APCS_WDOG_BITE_TIME_DATA_SHFT                               0x0

#define HWIO_APCS_WDOG_TEST_LOAD_STATUS_ADDR                      (APCS_KPSS_WDT_REG_BASE      + 0x00000018)
#define HWIO_APCS_WDOG_TEST_LOAD_STATUS_RMSK                             0x1
#define HWIO_APCS_WDOG_TEST_LOAD_STATUS_IN          \
        in_dword_masked(HWIO_APCS_WDOG_TEST_LOAD_STATUS_ADDR, HWIO_APCS_WDOG_TEST_LOAD_STATUS_RMSK)
#define HWIO_APCS_WDOG_TEST_LOAD_STATUS_INM(m)      \
        in_dword_masked(HWIO_APCS_WDOG_TEST_LOAD_STATUS_ADDR, m)
#define HWIO_APCS_WDOG_TEST_LOAD_STATUS_SYNC_STATUS_BMSK                 0x1
#define HWIO_APCS_WDOG_TEST_LOAD_STATUS_SYNC_STATUS_SHFT                 0x0

#define HWIO_APCS_WDOG_TEST_LOAD_ADDR                             (APCS_KPSS_WDT_REG_BASE      + 0x0000001c)
#define HWIO_APCS_WDOG_TEST_LOAD_RMSK                                    0x1
#define HWIO_APCS_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_APCS_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_APCS_WDOG_TEST_LOAD_LOAD_BMSK                               0x1
#define HWIO_APCS_WDOG_TEST_LOAD_LOAD_SHFT                               0x0

#define HWIO_APCS_WDOG_TEST_ADDR                                  (APCS_KPSS_WDT_REG_BASE      + 0x00000020)
#define HWIO_APCS_WDOG_TEST_RMSK                                     0xfffff
#define HWIO_APCS_WDOG_TEST_IN          \
        in_dword_masked(HWIO_APCS_WDOG_TEST_ADDR, HWIO_APCS_WDOG_TEST_RMSK)
#define HWIO_APCS_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_APCS_WDOG_TEST_ADDR, m)
#define HWIO_APCS_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_APCS_WDOG_TEST_ADDR,v)
#define HWIO_APCS_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_WDOG_TEST_ADDR,m,v,HWIO_APCS_WDOG_TEST_IN)
#define HWIO_APCS_WDOG_TEST_LOAD_VALUE_BMSK                          0xfffff
#define HWIO_APCS_WDOG_TEST_LOAD_VALUE_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                       (MPM2_MPM_BASE      + 0x0000b000)

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                 (MPM2_PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_RMSK                        0x1
#define HWIO_MPM2_MPM_PS_HOLD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, HWIO_MPM2_MPM_PS_HOLD_RMSK)
#define HWIO_MPM2_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM2_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM2_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM2_MPM_PS_HOLD_IN)
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK                 0x1
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_SLP_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_SLP_CNTR_REG_BASE                                    (MPM2_MPM_BASE      + 0x00003000)

#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR               (MPM2_SLP_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK               0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, m)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_BMSK          0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_SHFT                 0x0


#endif /* __MSMHWIOREG_H__ */
