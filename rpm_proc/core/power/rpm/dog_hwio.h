#ifndef __DOG_HWIO_H__
#define __DOG_HWIO_H__
/*
===========================================================================
*/
/**
  @file dog_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    RPM_DEC

  'Include' filters applied: WDOG[RPM_DEC] 
  'Exclude' filters applied: RESERVED DUMMY 
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

  $Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/dog_hwio.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"

#define RPM_CSR_BASE                                                 0x00080000

/*----------------------------------------------------------------------------
 * MODULE: RPM_DEC
 *--------------------------------------------------------------------------*/

#define RPM_DEC_REG_BASE                                                 (RPM_CSR_BASE      + 0x00000000)

#define HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR                                 (RPM_DEC_REG_BASE      + 0x00000008)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_RMSK                                        0x1
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_IN          \
        in_dword_masked(HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR, HWIO_RPM_TIMERS_CLK_OFF_CTL_RMSK)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_INM(m)      \
        in_dword_masked(HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR, m)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_OUT(v)      \
        out_dword(HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR,v)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_TIMERS_CLK_OFF_CTL_ADDR,m,v,HWIO_RPM_TIMERS_CLK_OFF_CTL_IN)
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_WDOG_TIMER_CLK_OFF_BMSK                     0x1
#define HWIO_RPM_TIMERS_CLK_OFF_CTL_WDOG_TIMER_CLK_OFF_SHFT                     0x0

#define HWIO_RPM_WDOG_RESET_ADDR                                         (RPM_DEC_REG_BASE      + 0x00000040)
#define HWIO_RPM_WDOG_RESET_RMSK                                                0x3
#define HWIO_RPM_WDOG_RESET_IN          \
        in_dword_masked(HWIO_RPM_WDOG_RESET_ADDR, HWIO_RPM_WDOG_RESET_RMSK)
#define HWIO_RPM_WDOG_RESET_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_RESET_ADDR, m)
#define HWIO_RPM_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_RESET_ADDR,v)
#define HWIO_RPM_WDOG_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_RESET_ADDR,m,v,HWIO_RPM_WDOG_RESET_IN)
#define HWIO_RPM_WDOG_RESET_SYNC_STATUS_BMSK                                    0x2
#define HWIO_RPM_WDOG_RESET_SYNC_STATUS_SHFT                                    0x1
#define HWIO_RPM_WDOG_RESET_WDOG_RESET_BMSK                                     0x1
#define HWIO_RPM_WDOG_RESET_WDOG_RESET_SHFT                                     0x0

#define HWIO_RPM_WDOG_CTRL_ADDR                                          (RPM_DEC_REG_BASE      + 0x00000044)
#define HWIO_RPM_WDOG_CTRL_RMSK                                                 0x3
#define HWIO_RPM_WDOG_CTRL_IN          \
        in_dword_masked(HWIO_RPM_WDOG_CTRL_ADDR, HWIO_RPM_WDOG_CTRL_RMSK)
#define HWIO_RPM_WDOG_CTRL_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_CTRL_ADDR, m)
#define HWIO_RPM_WDOG_CTRL_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_CTRL_ADDR,v)
#define HWIO_RPM_WDOG_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_CTRL_ADDR,m,v,HWIO_RPM_WDOG_CTRL_IN)
#define HWIO_RPM_WDOG_CTRL_HW_WAKEUP_SLEEP_EN_BMSK                              0x2
#define HWIO_RPM_WDOG_CTRL_HW_WAKEUP_SLEEP_EN_SHFT                              0x1
#define HWIO_RPM_WDOG_CTRL_ENABLE_BMSK                                          0x1
#define HWIO_RPM_WDOG_CTRL_ENABLE_SHFT                                          0x0

#define HWIO_RPM_WDOG_BARK_TIME_ADDR                                     (RPM_DEC_REG_BASE      + 0x0000004c)
#define HWIO_RPM_WDOG_BARK_TIME_RMSK                                       0x1fffff
#define HWIO_RPM_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_RPM_WDOG_BARK_TIME_ADDR, HWIO_RPM_WDOG_BARK_TIME_RMSK)
#define HWIO_RPM_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_BARK_TIME_ADDR, m)
#define HWIO_RPM_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_BARK_TIME_ADDR,v)
#define HWIO_RPM_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_BARK_TIME_ADDR,m,v,HWIO_RPM_WDOG_BARK_TIME_IN)
#define HWIO_RPM_WDOG_BARK_TIME_SYNC_STATUS_BMSK                           0x100000
#define HWIO_RPM_WDOG_BARK_TIME_SYNC_STATUS_SHFT                               0x14
#define HWIO_RPM_WDOG_BARK_TIME_WDOG_BARK_VAL_BMSK                          0xfffff
#define HWIO_RPM_WDOG_BARK_TIME_WDOG_BARK_VAL_SHFT                              0x0

#define HWIO_RPM_WDOG_BITE_TIME_ADDR                                     (RPM_DEC_REG_BASE      + 0x00000050)
#define HWIO_RPM_WDOG_BITE_TIME_RMSK                                       0x1fffff
#define HWIO_RPM_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_RPM_WDOG_BITE_TIME_ADDR, HWIO_RPM_WDOG_BITE_TIME_RMSK)
#define HWIO_RPM_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_RPM_WDOG_BITE_TIME_ADDR, m)
#define HWIO_RPM_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_RPM_WDOG_BITE_TIME_ADDR,v)
#define HWIO_RPM_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPM_WDOG_BITE_TIME_ADDR,m,v,HWIO_RPM_WDOG_BITE_TIME_IN)
#define HWIO_RPM_WDOG_BITE_TIME_SYNC_STATUS_BMSK                           0x100000
#define HWIO_RPM_WDOG_BITE_TIME_SYNC_STATUS_SHFT                               0x14
#define HWIO_RPM_WDOG_BITE_TIME_WDOG_BITE_VAL_BMSK                          0xfffff
#define HWIO_RPM_WDOG_BITE_TIME_WDOG_BITE_VAL_SHFT                              0x0

#endif /* __DOG_HWIO_H__ */
