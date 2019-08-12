#ifndef __MPSS_8994_HAL_SLEEP_HWIO_H__
#define __MPSS_8994_HAL_SLEEP_HWIO_H__
/*
===========================================================================
*/
/**
  @file MPSS_8994_HAL_sleep_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_QDSP6SS_CSR
    MSS_PERPH

  'Include' filters applied: 
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

  $Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/hwio/8994/HALsleep_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: MSS_QDSP6SS_CSR
 *--------------------------------------------------------------------------*/

#define MSS_QDSP6SS_CSR_REG_BASE                                          (MODEM_TOP_BASE      + 0x00100000)
#define MSS_QDSP6SS_CSR_REG_BASE_OFFS                                     0x00100000

#define HWIO_MSS_QDSP6SS_INSTANCE_ID_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00000000)
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_OFFS                                 (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00000000)
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_RMSK                                        0x7
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_INSTANCE_ID_ADDR, HWIO_MSS_QDSP6SS_INSTANCE_ID_RMSK)
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_INSTANCE_ID_ADDR, m)
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_ID_BMSK                                     0x7
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_ID_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_ADDR                              (MSS_QDSP6SS_CSR_REG_BASE      + 0x00001000)
#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_OFFS                              (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00001000)
#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_RMSK                              0xffffffff
#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_ADDR, HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_RMSK)
#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_ADDR, m)
#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_ADDR,v)
#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_ADDR,m,v,HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_IN)
#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_VALUE_BMSK                        0xffffffff
#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_VALUE_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_ADDR                              (MSS_QDSP6SS_CSR_REG_BASE      + 0x00001004)
#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_OFFS                              (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00001004)
#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_RMSK                              0xffffffff
#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_ADDR, HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_RMSK)
#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_ADDR, m)
#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_ADDR,v)
#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_ADDR,m,v,HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_IN)
#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_VALUE_BMSK                        0xffffffff
#define HWIO_MSS_QDSP6SS_IDLT_COUNT_VAL_VALUE_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00001008)
#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_OFFS                                 (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00001008)
#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_RMSK                                        0x1
#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_IDLT_ENABLE_ADDR, HWIO_MSS_QDSP6SS_IDLT_ENABLE_RMSK)
#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_IDLT_ENABLE_ADDR, m)
#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_IDLT_ENABLE_ADDR,v)
#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_IDLT_ENABLE_ADDR,m,v,HWIO_MSS_QDSP6SS_IDLT_ENABLE_IN)
#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_EN_BMSK                                     0x1
#define HWIO_MSS_QDSP6SS_IDLT_ENABLE_EN_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_ADDR                            (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000100c)
#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_OFFS                            (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x0000100c)
#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_RMSK                                   0x6
#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_ADDR, HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_COUNT_SYNC_STATUS_BMSK                 0x4
#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_COUNT_SYNC_STATUS_SHFT                 0x2
#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_MATCH_SYNC_STATUS_BMSK                 0x2
#define HWIO_MSS_QDSP6SS_IDLT_SYNC_STATUS_MATCH_SYNC_STATUS_SHFT                 0x1

#define HWIO_MSS_QDSP6SS_WDOG_RESET_ADDR                                  (MSS_QDSP6SS_CSR_REG_BASE      + 0x00002000)
#define HWIO_MSS_QDSP6SS_WDOG_RESET_OFFS                                  (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00002000)
#define HWIO_MSS_QDSP6SS_WDOG_RESET_RMSK                                         0x1
#define HWIO_MSS_QDSP6SS_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_RESET_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_RESET_WDOG_RESET_BMSK                              0x1
#define HWIO_MSS_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_WDOG_CTL_ADDR                                    (MSS_QDSP6SS_CSR_REG_BASE      + 0x00002004)
#define HWIO_MSS_QDSP6SS_WDOG_CTL_OFFS                                    (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00002004)
#define HWIO_MSS_QDSP6SS_WDOG_CTL_RMSK                                           0x7
#define HWIO_MSS_QDSP6SS_WDOG_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_CTL_ADDR, HWIO_MSS_QDSP6SS_WDOG_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_WDOG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_WDOG_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_WDOG_CTL_IN)
#define HWIO_MSS_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK                            0x4
#define HWIO_MSS_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT                            0x2
#define HWIO_MSS_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_BMSK                           0x2
#define HWIO_MSS_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_SHFT                           0x1
#define HWIO_MSS_QDSP6SS_WDOG_CTL_ENABLE_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_WDOG_CTL_ENABLE_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_WDOG_STATUS_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00002008)
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_OFFS                                 (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00002008)
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_RMSK                                 0xf81fffff
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_STATUS_ADDR, HWIO_MSS_QDSP6SS_WDOG_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_BMSK               0x80000000
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_SHFT                     0x1f
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_BMSK           0x40000000
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_SHFT                 0x1e
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_BMSK           0x20000000
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_SHFT                 0x1d
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_BMSK            0x10000000
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_SHFT                  0x1c
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_BMSK            0x8000000
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_SHFT                 0x1b
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_WDOG_COUNT_BMSK                        0x1ffffe
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_WDOG_COUNT_SHFT                             0x1
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_BMSK                    0x1
#define HWIO_MSS_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_SHFT                    0x0

#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_ADDR                              (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000200c)
#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_OFFS                              (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x0000200c)
#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_RMSK                                 0xfffff
#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_ADDR, HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_RMSK)
#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_ADDR, m)
#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_ADDR,m,v,HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_IN)
#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK                            0xfffff
#define HWIO_MSS_QDSP6SS_WDOG_BARK_TIME_DATA_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_ADDR                              (MSS_QDSP6SS_CSR_REG_BASE      + 0x00002010)
#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_OFFS                              (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00002010)
#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_RMSK                                 0xfffff
#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_ADDR, HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_RMSK)
#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_ADDR, m)
#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_ADDR,m,v,HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_IN)
#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_DATA_BMSK                            0xfffff
#define HWIO_MSS_QDSP6SS_WDOG_BITE_TIME_DATA_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x00002014)
#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_OFFS                               (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00002014)
#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_RMSK                                  0xfffff
#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_ADDR, HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_RMSK)
#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_ADDR, m)
#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_ADDR,m,v,HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_IN)
#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_DATA_BMSK                             0xfffff
#define HWIO_MSS_QDSP6SS_WDOG_NMI_TIME_DATA_SHFT                                 0x0

#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_ADDR                              (MSS_QDSP6SS_CSR_REG_BASE      + 0x00002018)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_OFFS                              (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00002018)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_RMSK                                     0x1
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_LOAD_BMSK                                0x1
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_LOAD_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_WDOG_TEST_ADDR                                   (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000201c)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_OFFS                                   (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x0000201c)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_RMSK                                     0x1fffff
#define HWIO_MSS_QDSP6SS_WDOG_TEST_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_TEST_ADDR, HWIO_MSS_QDSP6SS_WDOG_TEST_RMSK)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_WDOG_TEST_ADDR, m)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_TEST_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_WDOG_TEST_ADDR,m,v,HWIO_MSS_QDSP6SS_WDOG_TEST_IN)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_SYNC_STATUS_BMSK                         0x100000
#define HWIO_MSS_QDSP6SS_WDOG_TEST_SYNC_STATUS_SHFT                             0x14
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_VALUE_BMSK                           0xfffff
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_VALUE_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_IPC_ADDR                                         (MSS_QDSP6SS_CSR_REG_BASE      + 0x00005000)
#define HWIO_MSS_QDSP6SS_IPC_OFFS                                         (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00005000)
#define HWIO_MSS_QDSP6SS_IPC_RMSK                                         0xffffffff
#define HWIO_MSS_QDSP6SS_IPC_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_IPC_ADDR,v)
#define HWIO_MSS_QDSP6SS_IPC_INTR_BMSK                                    0xffffffff
#define HWIO_MSS_QDSP6SS_IPC_INTR_SHFT                                           0x0

#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_ADDR                            (MSS_QDSP6SS_CSR_REG_BASE      + 0x00005010)
#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_OFFS                            (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00005010)
#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_RMSK                                   0x1
#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_ADDR, HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_RMSK)
#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_ADDR, m)
#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_ADDR,v)
#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_ADDR,m,v,HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_IN)
#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_EN_L1_BMSK                             0x1
#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_EN_L1_SHFT                             0x0

#define HWIO_MSS_QDSP6SS_BLOCK_INTR_ADDR                                  (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007008)
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_OFFS                                  (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007008)
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_RMSK                                         0x7
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_BLOCK_INTR_ADDR, HWIO_MSS_QDSP6SS_BLOCK_INTR_RMSK)
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_BLOCK_INTR_ADDR, m)
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_BLOCK_INTR_ADDR,v)
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_BLOCK_INTR_ADDR,m,v,HWIO_MSS_QDSP6SS_BLOCK_INTR_IN)
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_CLEAR_BLOCK_BMSK                             0x4
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_CLEAR_BLOCK_SHFT                             0x2
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_BLOCK_STATUS_BMSK                            0x2
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_BLOCK_STATUS_SHFT                            0x1
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_BLOCK_INTR_BMSK                              0x1
#define HWIO_MSS_QDSP6SS_BLOCK_INTR_BLOCK_INTR_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_PD_HS_START_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000700c)
#define HWIO_MSS_QDSP6SS_PD_HS_START_OFFS                                 (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x0000700c)
#define HWIO_MSS_QDSP6SS_PD_HS_START_RMSK                                        0x1
#define HWIO_MSS_QDSP6SS_PD_HS_START_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PD_HS_START_ADDR,v)
#define HWIO_MSS_QDSP6SS_PD_HS_START_CMD_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_PD_HS_START_CMD_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_PD_HS_DONE_ADDR                                  (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007010)
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_OFFS                                  (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007010)
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_RMSK                                         0x1
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PD_HS_DONE_ADDR, HWIO_MSS_QDSP6SS_PD_HS_DONE_RMSK)
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PD_HS_DONE_ADDR, m)
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_DONE_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_DONE_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_PU_HS_START_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007014)
#define HWIO_MSS_QDSP6SS_PU_HS_START_OFFS                                 (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007014)
#define HWIO_MSS_QDSP6SS_PU_HS_START_RMSK                                        0x1
#define HWIO_MSS_QDSP6SS_PU_HS_START_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PU_HS_START_ADDR,v)
#define HWIO_MSS_QDSP6SS_PU_HS_START_CMD_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_PU_HS_START_CMD_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR                                    (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000701c)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_OFFS                                    (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x0000701c)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_RMSK                                          0xff
#define HWIO_MSS_QDSP6SS_SLPC_CFG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR, HWIO_MSS_QDSP6SS_SLPC_CFG_RMSK)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR, m)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR,v)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR,m,v,HWIO_MSS_QDSP6SS_SLPC_CFG_IN)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_WAKEUP_IN_EN_BMSK                             0x80
#define HWIO_MSS_QDSP6SS_SLPC_CFG_WAKEUP_IN_EN_SHFT                              0x7
#define HWIO_MSS_QDSP6SS_SLPC_CFG_SLP_DELAY_CTL_BMSK                            0x40
#define HWIO_MSS_QDSP6SS_SLPC_CFG_SLP_DELAY_CTL_SHFT                             0x6
#define HWIO_MSS_QDSP6SS_SLPC_CFG_CLK_GATING_MODE_BMSK                          0x20
#define HWIO_MSS_QDSP6SS_SLPC_CFG_CLK_GATING_MODE_SHFT                           0x5
#define HWIO_MSS_QDSP6SS_SLPC_CFG_PD_HS_MODE_BMSK                               0x10
#define HWIO_MSS_QDSP6SS_SLPC_CFG_PD_HS_MODE_SHFT                                0x4
#define HWIO_MSS_QDSP6SS_SLPC_CFG_SAW_DBG_BMSK                                   0x8
#define HWIO_MSS_QDSP6SS_SLPC_CFG_SAW_DBG_SHFT                                   0x3
#define HWIO_MSS_QDSP6SS_SLPC_CFG_PD_SRC_SEL_BMSK                                0x6
#define HWIO_MSS_QDSP6SS_SLPC_CFG_PD_SRC_SEL_SHFT                                0x1
#define HWIO_MSS_QDSP6SS_SLPC_CFG_EXT_WAKEUP_ENA_BMSK                            0x1
#define HWIO_MSS_QDSP6SS_SLPC_CFG_EXT_WAKEUP_ENA_SHFT                            0x0

#define HWIO_MSS_QDSP6SS_SLPC_STATUS_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007020)
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_OFFS                                 (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007020)
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_RMSK                                        0x3
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLPC_STATUS_ADDR, HWIO_MSS_QDSP6SS_SLPC_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLPC_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_FSM_STATE_BMSK                              0x3
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_FSM_STATE_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR                              (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007024)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_OFFS                              (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007024)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_RMSK                                   0x1ff
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ARCH_BMSK                              0x100
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ARCH_SHFT                                0x8
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_RET_BMSK                                0x80
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_RET_SHFT                                 0x7
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_AXIM2_BMSK                              0x40
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_AXIM2_SHFT                               0x6
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_AXIS_BMSK                               0x20
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_AXIS_SHFT                                0x5
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ATBM_BMSK                               0x10
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ATBM_SHFT                                0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_AHBM_BMSK                                0x8
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_AHBM_SHFT                                0x3
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_AXIM_BMSK                                0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_AXIM_SHFT                                0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ISDB_ETM_BMSK                            0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ISDB_ETM_SHFT                            0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_CORE_BMSK                                0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_CORE_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_ADDR                             (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007028)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_OFFS                             (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007028)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_RMSK                                    0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_CLAMP_IO_BMSK                           0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLAMP_CLAMP_IO_SHFT                           0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000702c)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_OFFS                               (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x0000702c)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_RMSK                                     0x7d
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_AXIM2_BMSK                               0x40
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_AXIM2_SHFT                                0x6
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_AXIS_BMSK                                0x20
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_AXIS_SHFT                                 0x5
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ATBM_BMSK                                0x10
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ATBM_SHFT                                 0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_AHBM_BMSK                                 0x8
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_AHBM_SHFT                                 0x3
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_AXIM_BMSK                                 0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_AXIM_SHFT                                 0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_CORE_BMSK                                 0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_CORE_SHFT                                 0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007030)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_OFFS                               (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007030)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_RMSK                                 0x6fe0ff
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_CLAMP_QMC_MEM_BMSK                   0x400000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_CLAMP_QMC_MEM_SHFT                       0x16
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_CLAMP_WL_BMSK                        0x200000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_CLAMP_WL_SHFT                            0x15
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_MEM_PERIPH_BMSK                       0x80000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_MEM_PERIPH_SHFT                          0x13
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2DATA_STBY_BMSK                      0x40000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2DATA_STBY_SHFT                         0x12
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ETB_BMSK                              0x20000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ETB_SHFT                                 0x11
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2TAG_BMSK                            0x10000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2TAG_SHFT                               0x10
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L1IU_BMSK                              0x8000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L1IU_SHFT                                 0xf
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L1DU_BMSK                              0x4000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L1DU_SHFT                                 0xe
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2PLRU_BMSK                            0x2000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2PLRU_SHFT                               0xd
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2DATA_BMSK                              0xff
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2DATA_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007034)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_OFFS                               (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007034)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_RMSK                                      0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_BHS_BMSK                                  0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_BHS_SHFT                                  0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007038)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_OFFS                               (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007038)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_RMSK                                    0x125
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_VREF_SEL_BMSK                           0x100
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_VREF_SEL_SHFT                             0x8
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_FORCEOFF_BMSK                            0x20
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_FORCEOFF_SHFT                             0x5
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_FORCEON_BMSK                              0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_FORCEON_SHFT                              0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_BYPASS_OPEN_BMSK                          0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_LDO_BYPASS_OPEN_SHFT                          0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000703c)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_OFFS                               (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x0000703c)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_RMSK                                    0x1ff
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_SPARE1_BMSK                             0x100
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_SPARE1_SHFT                               0x8
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_BHS_BMSK                                 0x80
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_BHS_SHFT                                  0x7
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_XO_ENABLE_BMSK                           0x40
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_XO_ENABLE_SHFT                            0x6
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_CLAMP_MEM_BMSK                           0x20
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_CLAMP_MEM_SHFT                            0x5
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_CLAMP_IO_BMSK                            0x10
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_CLAMP_IO_SHFT                             0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_PLL_VOTE_BMSK                             0x8
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_PLL_VOTE_SHFT                             0x3
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_PLL_OUTDIS_BMSK                           0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_PLL_OUTDIS_SHFT                           0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_PLL_BYPASS_BMSK                           0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_PLL_BYPASS_SHFT                           0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_PLL_RESET_BMSK                            0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_EXT_PLL_RESET_SHFT                            0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007040)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_OFFS                               (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007040)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_RMSK                                     0x1f
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_FREEZE_BMSK                          0x10
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_FREEZE_SHFT                           0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_VOTE_BMSK                             0x8
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_VOTE_SHFT                             0x3
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_OUTDIS_BMSK                           0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_OUTDIS_SHFT                           0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_BYPASS_BMSK                           0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_BYPASS_SHFT                           0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_RESET_BMSK                            0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_PLL_PLL_RESET_SHFT                            0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_ADDR                          (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007044)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_OFFS                          (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007044)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_RMSK                                 0x7
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_WAKE_IRQ_BMSK                        0x4
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_WAKE_IRQ_SHFT                        0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_RESTORE_BMSK                         0x2
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_RESTORE_SHFT                         0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_SAVE_BMSK                            0x1
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_STATERET_SAVE_SHFT                            0x0

#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007050)
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_OFFS                               (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007050)
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_RMSK                                      0xf
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_ADDR, HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_IN)
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_CORE_CLK_OFF_OVRRD_BMSK                   0x8
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_CORE_CLK_OFF_OVRRD_SHFT                   0x3
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_BHS_ACK_OVRRD_BMSK                        0x4
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_BHS_ACK_OVRRD_SHFT                        0x2
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_PLL_OVRRD_BMSK                            0x2
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_PLL_OVRRD_SHFT                            0x1
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_EXT_PLL_OVRRD_BMSK                        0x1
#define HWIO_MSS_QDSP6SS_SPM_EVENT_CTL_EXT_PLL_OVRRD_SHFT                        0x0

#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007080)
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_OFFS                                 (MSS_QDSP6SS_CSR_REG_BASE_OFFS + 0x00007080)
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_RMSK                                 0xc0000003
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_MVC_PWR_REQ_ADDR, HWIO_MSS_QDSP6SS_MVC_PWR_REQ_RMSK)
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_MVC_PWR_REQ_ADDR, m)
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_MVC_PWR_REQ_ADDR,v)
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_MVC_PWR_REQ_ADDR,m,v,HWIO_MSS_QDSP6SS_MVC_PWR_REQ_IN)
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_STATUS_BMSK                          0x80000000
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_STATUS_SHFT                                0x1f
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_ACK_BMSK                             0x40000000
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_ACK_SHFT                                   0x1e
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_PWR_LVL_BMSK                                0x3
#define HWIO_MSS_QDSP6SS_MVC_PWR_REQ_PWR_LVL_SHFT                                0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                    (MODEM_TOP_BASE      + 0x00180000)
#define MSS_PERPH_REG_BASE_OFFS                                               0x00180000

#define HWIO_MSS_CLAMP_MEM_ADDR                                               (MSS_PERPH_REG_BASE      + 0x00000000)
#define HWIO_MSS_CLAMP_MEM_OFFS                                               (MSS_PERPH_REG_BASE_OFFS + 0x00000000)
#define HWIO_MSS_CLAMP_MEM_RMSK                                                     0x1f
#define HWIO_MSS_CLAMP_MEM_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, HWIO_MSS_CLAMP_MEM_RMSK)
#define HWIO_MSS_CLAMP_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_MEM_ADDR, m)
#define HWIO_MSS_CLAMP_MEM_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_MEM_ADDR,v)
#define HWIO_MSS_CLAMP_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_MEM_ADDR,m,v,HWIO_MSS_CLAMP_MEM_IN)
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_BMSK                                         0x10
#define HWIO_MSS_CLAMP_MEM_UNCLAMP_ALL_SHFT                                          0x4
#define HWIO_MSS_CLAMP_MEM_SPARE_3_TO_1_BMSK                                         0xe
#define HWIO_MSS_CLAMP_MEM_SPARE_3_TO_1_SHFT                                         0x1
#define HWIO_MSS_CLAMP_MEM_CLAMP_MEM_BMSK                                            0x1
#define HWIO_MSS_CLAMP_MEM_CLAMP_MEM_SHFT                                            0x0

#define HWIO_MSS_CLAMP_IO_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00000004)
#define HWIO_MSS_CLAMP_IO_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x00000004)
#define HWIO_MSS_CLAMP_IO_RMSK                                                     0x1ff
#define HWIO_MSS_CLAMP_IO_IN          \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, HWIO_MSS_CLAMP_IO_RMSK)
#define HWIO_MSS_CLAMP_IO_INM(m)      \
        in_dword_masked(HWIO_MSS_CLAMP_IO_ADDR, m)
#define HWIO_MSS_CLAMP_IO_OUT(v)      \
        out_dword(HWIO_MSS_CLAMP_IO_ADDR,v)
#define HWIO_MSS_CLAMP_IO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLAMP_IO_ADDR,m,v,HWIO_MSS_CLAMP_IO_IN)
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_BMSK                                         0x100
#define HWIO_MSS_CLAMP_IO_UNCLAMP_ALL_SHFT                                           0x8
#define HWIO_MSS_CLAMP_IO_SPARE_7_TO_2_BMSK                                         0xfc
#define HWIO_MSS_CLAMP_IO_SPARE_7_TO_2_SHFT                                          0x2
#define HWIO_MSS_CLAMP_IO_RBCPR_BMSK                                                 0x2
#define HWIO_MSS_CLAMP_IO_RBCPR_SHFT                                                 0x1
#define HWIO_MSS_CLAMP_IO_MODEM_BMSK                                                 0x1
#define HWIO_MSS_CLAMP_IO_MODEM_SHFT                                                 0x0

#define HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00000008)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OFFS                                        (MSS_PERPH_REG_BASE_OFFS + 0x00000008)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK                                               0xf
#define HWIO_MSS_BUS_MAXI2AXI_CFG_IN          \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, HWIO_MSS_BUS_MAXI2AXI_CFG_RMSK)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR, m)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,v)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MAXI2AXI_CFG_ADDR,m,v,HWIO_MSS_BUS_MAXI2AXI_CFG_IN)
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_BMSK                        0xc
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_NAV_AREQPRIORITY_SHFT                        0x2
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_BMSK                     0x3
#define HWIO_MSS_BUS_MAXI2AXI_CFG_I_AXI_CRYPTO_AREQPRIORITY_SHFT                     0x0

#define HWIO_MSS_ATB_ID_ADDR                                                  (MSS_PERPH_REG_BASE      + 0x0000000c)
#define HWIO_MSS_ATB_ID_OFFS                                                  (MSS_PERPH_REG_BASE_OFFS + 0x0000000c)
#define HWIO_MSS_ATB_ID_RMSK                                                        0x7f
#define HWIO_MSS_ATB_ID_IN          \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, HWIO_MSS_ATB_ID_RMSK)
#define HWIO_MSS_ATB_ID_INM(m)      \
        in_dword_masked(HWIO_MSS_ATB_ID_ADDR, m)
#define HWIO_MSS_ATB_ID_OUT(v)      \
        out_dword(HWIO_MSS_ATB_ID_ADDR,v)
#define HWIO_MSS_ATB_ID_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_ATB_ID_ADDR,m,v,HWIO_MSS_ATB_ID_IN)
#define HWIO_MSS_ATB_ID_ATB_ID_BMSK                                                 0x7f
#define HWIO_MSS_ATB_ID_ATB_ID_SHFT                                                  0x0

#define HWIO_MSS_DBG_BUS_CTL_ADDR                                             (MSS_PERPH_REG_BASE      + 0x00000010)
#define HWIO_MSS_DBG_BUS_CTL_OFFS                                             (MSS_PERPH_REG_BASE_OFFS + 0x00000010)
#define HWIO_MSS_DBG_BUS_CTL_RMSK                                                0x1ffff
#define HWIO_MSS_DBG_BUS_CTL_IN          \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, HWIO_MSS_DBG_BUS_CTL_RMSK)
#define HWIO_MSS_DBG_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DBG_BUS_CTL_ADDR, m)
#define HWIO_MSS_DBG_BUS_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DBG_BUS_CTL_ADDR,v)
#define HWIO_MSS_DBG_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DBG_BUS_CTL_ADDR,m,v,HWIO_MSS_DBG_BUS_CTL_IN)
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_OFFLINE_NOCSEL_BMSK                    0x1c000
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_OFFLINE_NOCSEL_SHFT                        0xe
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_CE_SEL_BMSK                         0x3800
#define HWIO_MSS_DBG_BUS_CTL_BRIC_AXI2AXI_NAV_CE_SEL_SHFT                            0xb
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_BMSK                                  0x700
#define HWIO_MSS_DBG_BUS_CTL_AHB2AXI_NAV_SEL_SHFT                                    0x8
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_BMSK                                       0xc0
#define HWIO_MSS_DBG_BUS_CTL_AHB2AHB_SEL_SHFT                                        0x6
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_BMSK                               0x3c
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_BUS_TOP_SEL_SHFT                                0x2
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_BMSK                               0x3
#define HWIO_MSS_DBG_BUS_CTL_MSS_DBG_GPIO_ATB_SEL_SHFT                               0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00000014)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OFFS                                   (MSS_PERPH_REG_BASE_OFFS + 0x00000014)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK                                          0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_ADDR,m,v,HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_IN)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_BMSK                                       0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_EN_EN_SHFT                                       0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR                               (MSS_PERPH_REG_BASE      + 0x00000018)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_OFFS                               (MSS_PERPH_REG_BASE_OFFS + 0x00000018)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK                                      0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_RMSK)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_ADDR, m)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_BMSK                               0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_STATUS_STATUS_SHFT                               0x0

#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x0000001c)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_OFFS                                  (MSS_PERPH_REG_BASE_OFFS + 0x0000001c)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_RMSK                                         0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_OUT(v)      \
        out_dword(HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_ADDR,v)
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_BMSK                                     0x1
#define HWIO_MSS_AHB_ACCESS_ERR_IRQ_CLR_CMD_SHFT                                     0x0

#define HWIO_MSS_RELAY_MSG_SHADOW0_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000020)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00000020)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, HWIO_MSS_RELAY_MSG_SHADOW0_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW0_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW0_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW0_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW0_RELAY_MSG_SHADOW0_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW1_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000024)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00000024)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, HWIO_MSS_RELAY_MSG_SHADOW1_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW1_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW1_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW1_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW1_RELAY_MSG_SHADOW1_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW2_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000028)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00000028)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, HWIO_MSS_RELAY_MSG_SHADOW2_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW2_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW2_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW2_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW2_RELAY_MSG_SHADOW2_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW3_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000002c)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x0000002c)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, HWIO_MSS_RELAY_MSG_SHADOW3_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW3_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW3_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW3_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW3_RELAY_MSG_SHADOW3_SHFT                            0x0

#define HWIO_MSS_MSA_ADDR                                                     (MSS_PERPH_REG_BASE      + 0x00000030)
#define HWIO_MSS_MSA_OFFS                                                     (MSS_PERPH_REG_BASE_OFFS + 0x00000030)
#define HWIO_MSS_MSA_RMSK                                                            0x3
#define HWIO_MSS_MSA_IN          \
        in_dword_masked(HWIO_MSS_MSA_ADDR, HWIO_MSS_MSA_RMSK)
#define HWIO_MSS_MSA_INM(m)      \
        in_dword_masked(HWIO_MSS_MSA_ADDR, m)
#define HWIO_MSS_MSA_OUT(v)      \
        out_dword(HWIO_MSS_MSA_ADDR,v)
#define HWIO_MSS_MSA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MSA_ADDR,m,v,HWIO_MSS_MSA_IN)
#define HWIO_MSS_MSA_MBA_OK_BMSK                                                     0x2
#define HWIO_MSS_MSA_MBA_OK_SHFT                                                     0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_BMSK                                                0x1
#define HWIO_MSS_MSA_CONFIG_LOCK_SHFT                                                0x0

#define HWIO_MSS_HW_VERSION_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00000034)
#define HWIO_MSS_HW_VERSION_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00000034)
#define HWIO_MSS_HW_VERSION_RMSK                                              0xffffffff
#define HWIO_MSS_HW_VERSION_IN          \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, HWIO_MSS_HW_VERSION_RMSK)
#define HWIO_MSS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_MSS_HW_VERSION_ADDR, m)
#define HWIO_MSS_HW_VERSION_MAJOR_BMSK                                        0xf0000000
#define HWIO_MSS_HW_VERSION_MAJOR_SHFT                                              0x1c
#define HWIO_MSS_HW_VERSION_MINOR_BMSK                                         0xfff0000
#define HWIO_MSS_HW_VERSION_MINOR_SHFT                                              0x10
#define HWIO_MSS_HW_VERSION_STEP_BMSK                                             0xffff
#define HWIO_MSS_HW_VERSION_STEP_SHFT                                                0x0

#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00000038)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OFFS                                    (MSS_PERPH_REG_BASE_OFFS + 0x00000038)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_RMSK                                    0x7fffffff
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR, HWIO_MSS_OFFLINE_MEM_SLP_CNTL_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR,v)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_MEM_SLP_CNTL_ADDR,m,v,HWIO_MSS_OFFLINE_MEM_SLP_CNTL_IN)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SPARE_30_27_BMSK                        0x78000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_SPARE_30_27_SHFT                              0x1b
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_DMEM_SLP_NRET_N_BMSK                0x4000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_DMEM_SLP_NRET_N_SHFT                     0x1a
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_DMEM_SLP_RET_N_BMSK                 0x2000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_DMEM_SLP_RET_N_SHFT                      0x19
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_PMEM_SLP_NRET_N_BMSK                0x1000000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_PMEM_SLP_NRET_N_SHFT                     0x18
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_PMEM_SLP_RET_N_BMSK                  0x800000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_PMEM_SLP_RET_N_SHFT                      0x17
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_IURF_SLP_NRET_N_BMSK                 0x400000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_IURF_SLP_NRET_N_SHFT                     0x16
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_IURF_SLP_RET_N_BMSK                  0x200000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_IURF_SLP_RET_N_SHFT                      0x15
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_LMEM_SLP_NRET_N_BMSK                 0x180000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_LMEM_SLP_NRET_N_SHFT                     0x13
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_LMEM_SLP_RET_N_BMSK                   0x40000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE2_LMEM_SLP_RET_N_SHFT                      0x12
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_DMEM_SLP_NRET_N_BMSK                  0x20000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_DMEM_SLP_NRET_N_SHFT                     0x11
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_DMEM_SLP_RET_N_BMSK                   0x10000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_DMEM_SLP_RET_N_SHFT                      0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_PMEM_SLP_NRET_N_BMSK                   0x8000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_PMEM_SLP_NRET_N_SHFT                      0xf
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_PMEM_SLP_RET_N_BMSK                    0x4000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_PMEM_SLP_RET_N_SHFT                       0xe
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_IURF_SLP_NRET_N_BMSK                   0x2000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_IURF_SLP_NRET_N_SHFT                      0xd
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_IURF_SLP_RET_N_BMSK                    0x1000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_IURF_SLP_RET_N_SHFT                       0xc
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_LMEM_SLP_NRET_N_BMSK                    0xc00
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_LMEM_SLP_NRET_N_SHFT                      0xa
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_LMEM_SLP_RET_N_BMSK                     0x200
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE1_LMEM_SLP_RET_N_SHFT                       0x9
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_DMEM_SLP_NRET_N_BMSK                    0x100
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_DMEM_SLP_NRET_N_SHFT                      0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_DMEM_SLP_RET_N_BMSK                      0x80
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_DMEM_SLP_RET_N_SHFT                       0x7
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_PMEM_SLP_NRET_N_BMSK                     0x40
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_PMEM_SLP_NRET_N_SHFT                      0x6
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_PMEM_SLP_RET_N_BMSK                      0x20
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_PMEM_SLP_RET_N_SHFT                       0x5
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_IURF_SLP_NRET_N_BMSK                     0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_IURF_SLP_NRET_N_SHFT                      0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_IURF_SLP_RET_N_BMSK                       0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_IURF_SLP_RET_N_SHFT                       0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_LMEM_SLP_NRET_N_BMSK                      0x6
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_LMEM_SLP_NRET_N_SHFT                      0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_LMEM_SLP_RET_N_BMSK                       0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL_VPE0_LMEM_SLP_RET_N_SHFT                       0x0

#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR                                   (MSS_PERPH_REG_BASE      + 0x0000003c)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OFFS                                   (MSS_PERPH_REG_BASE_OFFS + 0x0000003c)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_RMSK                                   0xffffffff
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR, HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_RMSK)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR, m)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR,v)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_ADDR,m,v,HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_IN)
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_31_19_BMSK                       0xfff80000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_SPARE_31_19_SHFT                             0x13
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_DMEM_SLP_NRET_N_BMSK                 0x40000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_DMEM_SLP_NRET_N_SHFT                    0x12
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_DMEM_SLP_RET_N_BMSK                  0x20000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_DMEM_SLP_RET_N_SHFT                     0x11
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_PMEM_SLP_NRET_N_BMSK                 0x10000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_PMEM_SLP_NRET_N_SHFT                    0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_PMEM_SLP_RET_N_BMSK                   0x8000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_PMEM_SLP_RET_N_SHFT                      0xf
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_IURF_SLP_NRET_N_BMSK                  0x4000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_IURF_SLP_NRET_N_SHFT                     0xe
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_IURF_SLP_RET_N_BMSK                   0x2000
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_IURF_SLP_RET_N_SHFT                      0xd
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_LMEM_SLP_NRET_N_BMSK                  0x1800
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_LMEM_SLP_NRET_N_SHFT                     0xb
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_LMEM_SLP_RET_N_BMSK                    0x400
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_VPE3_LMEM_SLP_RET_N_SHFT                      0xa
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_NRET_N_2_BMSK                         0x200
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_NRET_N_2_SHFT                           0x9
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_DB_CTL_PDMEM_SLP_NRET_N_BMSK                0x100
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_DB_CTL_PDMEM_SLP_NRET_N_SHFT                  0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_DB_CTL_PDMEM_SLP_RET_N_BMSK                  0x80
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_DB_CTL_PDMEM_SLP_RET_N_SHFT                   0x7
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_NRET_N_BMSK                            0x60
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_NRET_N_SHFT                             0x5
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_RET_N_BMSK                             0x10
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_MEMSS_RET_N_SHFT                              0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_NRET_N_BMSK                            0x8
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_NRET_N_SHFT                            0x3
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_RET_N_BMSK                             0x4
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_COMMON_RET_N_SHFT                             0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CCS0_SLP_NRET_N_BMSK                          0x2
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CCS0_SLP_NRET_N_SHFT                          0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CCS0_SLP_RET_N_BMSK                           0x1
#define HWIO_MSS_OFFLINE_MEM_SLP_CNTL2_CCS0_SLP_RET_N_SHFT                           0x0

#define HWIO_MSS_CLOCK_SPDM_MON_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00000040)
#define HWIO_MSS_CLOCK_SPDM_MON_OFFS                                          (MSS_PERPH_REG_BASE_OFFS + 0x00000040)
#define HWIO_MSS_CLOCK_SPDM_MON_RMSK                                                 0x1
#define HWIO_MSS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, HWIO_MSS_CLOCK_SPDM_MON_RMSK)
#define HWIO_MSS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_MSS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_MSS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_MSS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_MSS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_MSS_CLOCK_SPDM_MON_IN)
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_BMSK                                   0x1
#define HWIO_MSS_CLOCK_SPDM_MON_BUS_MON_CLKEN_SHFT                                   0x0

#define HWIO_MSS_DEBUG_CLOCK_CTL_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00000044)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OFFS                                         (MSS_PERPH_REG_BASE_OFFS + 0x00000044)
#define HWIO_MSS_DEBUG_CLOCK_CTL_RMSK                                         0xff80007f
#define HWIO_MSS_DEBUG_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, HWIO_MSS_DEBUG_CLOCK_CTL_RMSK)
#define HWIO_MSS_DEBUG_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CLOCK_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CLOCK_CTL_IN)
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_RESET_SEL_BMSK                           0xe0000000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_RESET_SEL_SHFT                                 0x1d
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_BYPASS_SEL_BMSK                          0x1c000000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLL_BYPASS_SEL_SHFT                                0x1a
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLLOUT_LV_TEST_SEL_BMSK                       0x3800000
#define HWIO_MSS_DEBUG_CLOCK_CTL_PLLOUT_LV_TEST_SEL_SHFT                            0x17
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL7_MUX_SEL_BMSK                            0x40
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL7_MUX_SEL_SHFT                             0x6
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL6_MUX_SEL_BMSK                            0x20
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL6_MUX_SEL_SHFT                             0x5
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_BMSK                            0x10
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL5_MUX_SEL_SHFT                             0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_BMSK                             0x8
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL4_MUX_SEL_SHFT                             0x3
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_BMSK                             0x4
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL3_MUX_SEL_SHFT                             0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_BMSK                             0x2
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL2_MUX_SEL_SHFT                             0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_BMSK                             0x1
#define HWIO_MSS_DEBUG_CLOCK_CTL_DBG_LEVEL1_MUX_SEL_SHFT                             0x0

#define HWIO_MSS_DEBUG_CTL_ADDR                                               (MSS_PERPH_REG_BASE      + 0x00000048)
#define HWIO_MSS_DEBUG_CTL_OFFS                                               (MSS_PERPH_REG_BASE_OFFS + 0x00000048)
#define HWIO_MSS_DEBUG_CTL_RMSK                                                      0x7
#define HWIO_MSS_DEBUG_CTL_IN          \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, HWIO_MSS_DEBUG_CTL_RMSK)
#define HWIO_MSS_DEBUG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_DEBUG_CTL_ADDR, m)
#define HWIO_MSS_DEBUG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_DEBUG_CTL_ADDR,v)
#define HWIO_MSS_DEBUG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DEBUG_CTL_ADDR,m,v,HWIO_MSS_DEBUG_CTL_IN)
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_BMSK                                0x4
#define HWIO_MSS_DEBUG_CTL_DAC_DISABLE_ON_Q6_DBG_SHFT                                0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_BMSK                                  0x2
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_DBG_SHFT                                  0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_BMSK                                 0x1
#define HWIO_MSS_DEBUG_CTL_GRFC_DISABLE_Q6_WDOG_SHFT                                 0x0

#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR                                     (MSS_PERPH_REG_BASE      + 0x0000004c)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OFFS                                     (MSS_PERPH_REG_BASE_OFFS + 0x0000004c)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK                                           0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN          \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, HWIO_MSS_CUSTOM_MEM_ARRSTBYN_RMSK)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_INM(m)      \
        in_dword_masked(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR, m)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUT(v)      \
        out_dword(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,v)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CUSTOM_MEM_ARRSTBYN_ADDR,m,v,HWIO_MSS_CUSTOM_MEM_ARRSTBYN_IN)
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_BMSK                                       0xff
#define HWIO_MSS_CUSTOM_MEM_ARRSTBYN_CTL_SHFT                                        0x0

#define HWIO_MSS_RELAY_MSG_SHADOW4_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000050)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00000050)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, HWIO_MSS_RELAY_MSG_SHADOW4_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW4_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW4_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW4_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW4_RELAY_MSG_SHADOW4_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW5_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000054)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00000054)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, HWIO_MSS_RELAY_MSG_SHADOW5_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW5_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW5_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW5_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW5_RELAY_MSG_SHADOW5_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW6_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000058)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00000058)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, HWIO_MSS_RELAY_MSG_SHADOW6_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW6_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW6_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW6_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW6_RELAY_MSG_SHADOW6_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW7_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000005c)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x0000005c)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, HWIO_MSS_RELAY_MSG_SHADOW7_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW7_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW7_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW7_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW7_RELAY_MSG_SHADOW7_SHFT                            0x0

#define HWIO_MSS_RELAY_MSG_SHADOW8_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00000060)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00000060)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RMSK                                       0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_IN          \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, HWIO_MSS_RELAY_MSG_SHADOW8_RMSK)
#define HWIO_MSS_RELAY_MSG_SHADOW8_INM(m)      \
        in_dword_masked(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR, m)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUT(v)      \
        out_dword(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,v)
#define HWIO_MSS_RELAY_MSG_SHADOW8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RELAY_MSG_SHADOW8_ADDR,m,v,HWIO_MSS_RELAY_MSG_SHADOW8_IN)
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_BMSK                     0xffffffff
#define HWIO_MSS_RELAY_MSG_SHADOW8_RELAY_MSG_SHADOW8_SHFT                            0x0

#define HWIO_MSS_OFFLINE_PLL_MODE_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001000)
#define HWIO_MSS_OFFLINE_PLL_MODE_OFFS                                        (MSS_PERPH_REG_BASE_OFFS + 0x00001000)
#define HWIO_MSS_OFFLINE_PLL_MODE_RMSK                                        0xa0ffff0f
#define HWIO_MSS_OFFLINE_PLL_MODE_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_MODE_ADDR, HWIO_MSS_OFFLINE_PLL_MODE_RMSK)
#define HWIO_MSS_OFFLINE_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_MODE_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_MODE_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_MODE_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_MODE_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_MODE_IN)
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_BMSK                           0x80000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_DET_SHFT                                 0x1f
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACK_LATCH_BMSK                          0x20000000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_ACK_LATCH_SHFT                                0x1d
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK               0x800000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                   0x17
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_UPDATE_BMSK                               0x400000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_UPDATE_SHFT                                   0x16
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                       0x200000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                           0x15
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                         0x100000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                             0x14
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BIAS_COUNT_BMSK                            0xfc000
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BIAS_COUNT_SHFT                                0xe
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_COUNT_BMSK                             0x3f00
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_LOCK_COUNT_SHFT                                0x8
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_PLLTEST_BMSK                                   0x8
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_PLLTEST_SHFT                                   0x3
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_RESET_N_BMSK                                   0x4
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_RESET_N_SHFT                                   0x2
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BYPASSNL_BMSK                                  0x2
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_BYPASSNL_SHFT                                  0x1
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_OUTCTRL_BMSK                                   0x1
#define HWIO_MSS_OFFLINE_PLL_MODE_PLL_OUTCTRL_SHFT                                   0x0

#define HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001004)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00001004)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_RMSK                                           0xffff
#define HWIO_MSS_OFFLINE_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_L_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_L_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_L_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_L_VAL_PLL_L_BMSK                                     0xffff
#define HWIO_MSS_OFFLINE_PLL_L_VAL_PLL_L_SHFT                                        0x0

#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001008)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_OFFS                                   (MSS_PERPH_REG_BASE_OFFS + 0x00001008)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_RMSK                                   0xffffffff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR, HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_IN)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_PLL_ALPHA_31_0_BMSK                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_PLL_ALPHA_31_0_SHFT                           0x0

#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR                                 (MSS_PERPH_REG_BASE      + 0x0000100c)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_OFFS                                 (MSS_PERPH_REG_BASE_OFFS + 0x0000100c)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_RMSK                                       0xff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR, HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_BMSK                       0xff
#define HWIO_MSS_OFFLINE_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_SHFT                        0x0

#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001010)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OFFS                                    (MSS_PERPH_REG_BASE_OFFS + 0x00001010)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_USER_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_USER_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_USER_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BIT31_28_BMSK                   0xf0000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BIT31_28_SHFT                         0x1c
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK                    0x8000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                         0x1b
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS26_25_BMSK                   0x6000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS26_25_SHFT                        0x19
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ALPHA_EN_BMSK                            0x1000000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_ALPHA_EN_SHFT                                 0x18
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS23_22_BMSK                    0xc00000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS23_22_SHFT                        0x16
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_VCO_SEL_BMSK                              0x300000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_VCO_SEL_SHFT                                  0x14
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS19_15_BMSK                     0xf8000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS19_15_SHFT                         0xf
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                          0x7000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                             0xc
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_BMSK                          0xf00
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_POST_DIV_RATIO_SHFT                            0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTPUT_INV_BMSK                               0x80
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_OUTPUT_INV_SHFT                                0x7
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                          0x60
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                           0x5
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                           0x10
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                            0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                           0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                           0x3
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                            0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                            0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                             0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                             0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                            0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                            0x0

#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00001014)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OFFS                                  (MSS_PERPH_REG_BASE_OFFS + 0x00001014)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RMSK                                  0xffffffff
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_USER_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_USER_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RESERVE_BITS31_12_BMSK                0xfffff000
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_RESERVE_BITS31_12_SHFT                       0xc
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LATCH_INTERFACE_BMSK                       0x800
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LATCH_INTERFACE_SHFT                         0xb
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                       0x700
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                         0x8
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_DSM_BMSK                                    0x80
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_DSM_SHFT                                     0x7
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_WRITE_STATE_BMSK                            0x40
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_WRITE_STATE_SHFT                             0x6
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TARGET_CTL_BMSK                             0x38
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TARGET_CTL_SHFT                              0x3
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LOCKDET_BMSK                                 0x4
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_LOCKDET_SHFT                                 0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_FREEZE_PLL_BMSK                              0x2
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_FREEZE_PLL_SHFT                              0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TOGGLE_DET_BMSK                              0x1
#define HWIO_MSS_OFFLINE_PLL_USER_CTL_U_TOGGLE_DET_SHFT                              0x0

#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00001018)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OFFS                                  (MSS_PERPH_REG_BASE_OFFS + 0x00001018)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RMSK                                  0xffffffff
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_BMSK          0x80000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_SHFT                0x1f
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_BMSK               0x40000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_SHFT                     0x1e
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_BMSK          0x3c000000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_SHFT                0x1a
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_BMSK              0x3800000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_SHFT                   0x17
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_BMSK                  0x700000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_SHFT                      0x14
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_BMSK                  0xff000
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_SHFT                      0xc
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_BMSK                  0xf00
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_SHFT                    0x8
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RESERVE_BITS7_6_BMSK                        0xc0
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_RESERVE_BITS7_6_SHFT                         0x6
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_REF_CYCLE_BMSK                              0x30
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_REF_CYCLE_SHFT                               0x4
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_KFN_BMSK                                     0xf
#define HWIO_MSS_OFFLINE_PLL_CONFIG_CTL_KFN_SHFT                                     0x0

#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x0000101c)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OFFS                                    (MSS_PERPH_REG_BASE_OFFS + 0x0000101c)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_TEST_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_TEST_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_TEST_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BIAS_GEN_TRIM_BMSK                      0xe0000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BIAS_GEN_TRIM_SHFT                            0x1d
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_PROCESS_CALB_BMSK                       0x1c000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_PROCESS_CALB_SHFT                             0x1a
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_BMSK               0x2000000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_SHFT                    0x19
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_FINE_FCW_BMSK                            0x1f00000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_FINE_FCW_SHFT                                 0x14
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                     0x80000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                        0x13
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_COARSE_FCW_BMSK                            0x7e000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_COARSE_FCW_SHFT                                0xd
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_COARSE_BMSK                        0x1000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_OVERRIDE_COARSE_SHFT                           0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DISABLE_LFSR_BMSK                            0x800
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DISABLE_LFSR_SHFT                              0xb
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_SEL_BMSK                               0x700
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_SEL_SHFT                                 0x8
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_EN_BMSK                                 0x80
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_DTEST_EN_SHFT                                  0x7
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BYP_TESTAMP_BMSK                              0x40
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_BYP_TESTAMP_SHFT                               0x6
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_SEL_BMSK                               0x30
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_SEL_SHFT                                0x4
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_SEL_BMSK                                0xc
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_SEL_SHFT                                0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_EN_BMSK                                 0x2
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST1_EN_SHFT                                 0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_EN_BMSK                                 0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_ATEST0_EN_SHFT                                 0x0

#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00001020)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OFFS                                  (MSS_PERPH_REG_BASE_OFFS + 0x00001020)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RMSK                                    0x3ffeff
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR, HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_RMSK)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_IN)
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_GLITCH_DET_CNT_BMSK                     0x300000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_GLITCH_DET_CNT_SHFT                         0x14
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DISABLE_GLITCH_DET_BMSK                  0x80000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DISABLE_GLITCH_DET_SHFT                     0x13
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_EXT_BMSK                  0x60000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_EXT_SHFT                     0x11
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DITHER_SEL_BMSK                          0x18000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DITHER_SEL_SHFT                              0xf
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PROCESS_CALB_SEL_BMSK                     0x4000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PROCESS_CALB_SEL_SHFT                        0xe
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_BMSK                0x2000
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_SHFT                   0xd
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_BMSK                       0x1800
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_DTEST_MODE_SEL_SHFT                          0xb
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                           0x600
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                             0x9
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_MAG_BMSK                              0xe0
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_MAG_SHFT                               0x5
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_GEN_BMSK                              0x10
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_NOISE_GEN_SHFT                               0x4
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                            0x8
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                            0x3
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                        0x6
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                        0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_CAL_CODE_UPDATE_BMSK                         0x1
#define HWIO_MSS_OFFLINE_PLL_TEST_CTL_U_CAL_CODE_UPDATE_SHFT                         0x0

#define HWIO_MSS_OFFLINE_PLL_STATUS_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001024)
#define HWIO_MSS_OFFLINE_PLL_STATUS_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x00001024)
#define HWIO_MSS_OFFLINE_PLL_STATUS_RMSK                                      0xffffffff
#define HWIO_MSS_OFFLINE_PLL_STATUS_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATUS_ADDR, HWIO_MSS_OFFLINE_PLL_STATUS_RMSK)
#define HWIO_MSS_OFFLINE_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_STATUS_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_STATUS_STATUS_31_0_BMSK                          0xffffffff
#define HWIO_MSS_OFFLINE_PLL_STATUS_STATUS_31_0_SHFT                                 0x0

#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR                                    (MSS_PERPH_REG_BASE      + 0x00001028)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OFFS                                    (MSS_PERPH_REG_BASE_OFFS + 0x00001028)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_RMSK                                    0xffffffff
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR, HWIO_MSS_OFFLINE_PLL_FREQ_CTL_RMSK)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR, m)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR,v)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_OFFLINE_PLL_FREQ_CTL_ADDR,m,v,HWIO_MSS_OFFLINE_PLL_FREQ_CTL_IN)
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_PLL_FREQ_CONTROL_WORD_BMSK              0xffffffff
#define HWIO_MSS_OFFLINE_PLL_FREQ_CTL_PLL_FREQ_CONTROL_WORD_SHFT                     0x0

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000102c)
#define HWIO_MSS_BUS_CMD_RCGR_OFFS                                            (MSS_PERPH_REG_BASE_OFFS + 0x0000102c)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                            0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                            0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001030)
#define HWIO_MSS_BUS_CFG_RCGR_OFFS                                            (MSS_PERPH_REG_BASE_OFFS + 0x00001030)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                                 0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                         0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                           0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                          0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_UIM_CMD_RCGR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_UIM_CMD_RCGR_OFFS                                            (MSS_PERPH_REG_BASE_OFFS + 0x00001034)
#define HWIO_MSS_UIM_CMD_RCGR_RMSK                                            0x80000003
#define HWIO_MSS_UIM_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, HWIO_MSS_UIM_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM_CMD_RCGR_IN)
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_BMSK                                   0x80000000
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_OFF_SHFT                                         0x1f
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_BMSK                                           0x2
#define HWIO_MSS_UIM_CMD_RCGR_ROOT_EN_SHFT                                           0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_BMSK                                            0x1
#define HWIO_MSS_UIM_CMD_RCGR_UPDATE_SHFT                                            0x0

#define HWIO_MSS_UIM_CFG_RCGR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001038)
#define HWIO_MSS_UIM_CFG_RCGR_OFFS                                            (MSS_PERPH_REG_BASE_OFFS + 0x00001038)
#define HWIO_MSS_UIM_CFG_RCGR_RMSK                                                  0x1f
#define HWIO_MSS_UIM_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, HWIO_MSS_UIM_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM_CFG_RCGR_IN)
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_BMSK                                          0x1f
#define HWIO_MSS_UIM_CFG_RCGR_SRC_DIV_SHFT                                           0x0

#define HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x0000103c)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x0000103c)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM0_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM0_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM0_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM0_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001040)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00001040)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM0_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM0_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM0_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM0_MND_M_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001044)
#define HWIO_MSS_UIM0_MND_M_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00001044)
#define HWIO_MSS_UIM0_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, HWIO_MSS_UIM0_MND_M_RMSK)
#define HWIO_MSS_UIM0_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_M_ADDR, m)
#define HWIO_MSS_UIM0_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_M_ADDR,v)
#define HWIO_MSS_UIM0_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_M_ADDR,m,v,HWIO_MSS_UIM0_MND_M_IN)
#define HWIO_MSS_UIM0_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM0_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM0_MND_N_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001048)
#define HWIO_MSS_UIM0_MND_N_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00001048)
#define HWIO_MSS_UIM0_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, HWIO_MSS_UIM0_MND_N_RMSK)
#define HWIO_MSS_UIM0_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_N_ADDR, m)
#define HWIO_MSS_UIM0_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_N_ADDR,v)
#define HWIO_MSS_UIM0_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_N_ADDR,m,v,HWIO_MSS_UIM0_MND_N_IN)
#define HWIO_MSS_UIM0_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM0_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM0_MND_D_ADDR                                              (MSS_PERPH_REG_BASE      + 0x0000104c)
#define HWIO_MSS_UIM0_MND_D_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x0000104c)
#define HWIO_MSS_UIM0_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM0_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, HWIO_MSS_UIM0_MND_D_RMSK)
#define HWIO_MSS_UIM0_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_MND_D_ADDR, m)
#define HWIO_MSS_UIM0_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_MND_D_ADDR,v)
#define HWIO_MSS_UIM0_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_MND_D_ADDR,m,v,HWIO_MSS_UIM0_MND_D_IN)
#define HWIO_MSS_UIM0_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM0_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_UIM0_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001050)
#define HWIO_MSS_UIM0_BCR_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x00001050)
#define HWIO_MSS_UIM0_BCR_RMSK                                                       0x1
#define HWIO_MSS_UIM0_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, HWIO_MSS_UIM0_BCR_RMSK)
#define HWIO_MSS_UIM0_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM0_BCR_ADDR, m)
#define HWIO_MSS_UIM0_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM0_BCR_ADDR,v)
#define HWIO_MSS_UIM0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM0_BCR_ADDR,m,v,HWIO_MSS_UIM0_BCR_IN)
#define HWIO_MSS_UIM0_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_UIM0_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001054)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x00001054)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM0_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM0_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM0_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM0_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001058)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x00001058)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM0_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM0_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM0_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM0_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM0_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000105c)
#define HWIO_MSS_BUS_UIM0_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x0000105c)
#define HWIO_MSS_BUS_UIM0_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM0_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, HWIO_MSS_BUS_UIM0_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM0_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM0_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM0_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM0_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM0_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM0_CBCR_IN)
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM0_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM0_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM0_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM0_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001060)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00001060)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM1_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM1_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM1_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM1_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001064)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00001064)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM1_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM1_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM1_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM1_MND_M_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001068)
#define HWIO_MSS_UIM1_MND_M_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00001068)
#define HWIO_MSS_UIM1_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, HWIO_MSS_UIM1_MND_M_RMSK)
#define HWIO_MSS_UIM1_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_M_ADDR, m)
#define HWIO_MSS_UIM1_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_M_ADDR,v)
#define HWIO_MSS_UIM1_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_M_ADDR,m,v,HWIO_MSS_UIM1_MND_M_IN)
#define HWIO_MSS_UIM1_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM1_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM1_MND_N_ADDR                                              (MSS_PERPH_REG_BASE      + 0x0000106c)
#define HWIO_MSS_UIM1_MND_N_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x0000106c)
#define HWIO_MSS_UIM1_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, HWIO_MSS_UIM1_MND_N_RMSK)
#define HWIO_MSS_UIM1_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_N_ADDR, m)
#define HWIO_MSS_UIM1_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_N_ADDR,v)
#define HWIO_MSS_UIM1_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_N_ADDR,m,v,HWIO_MSS_UIM1_MND_N_IN)
#define HWIO_MSS_UIM1_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM1_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM1_MND_D_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001070)
#define HWIO_MSS_UIM1_MND_D_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00001070)
#define HWIO_MSS_UIM1_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM1_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, HWIO_MSS_UIM1_MND_D_RMSK)
#define HWIO_MSS_UIM1_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_MND_D_ADDR, m)
#define HWIO_MSS_UIM1_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_MND_D_ADDR,v)
#define HWIO_MSS_UIM1_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_MND_D_ADDR,m,v,HWIO_MSS_UIM1_MND_D_IN)
#define HWIO_MSS_UIM1_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM1_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_UIM1_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001074)
#define HWIO_MSS_UIM1_BCR_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x00001074)
#define HWIO_MSS_UIM1_BCR_RMSK                                                       0x1
#define HWIO_MSS_UIM1_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, HWIO_MSS_UIM1_BCR_RMSK)
#define HWIO_MSS_UIM1_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM1_BCR_ADDR, m)
#define HWIO_MSS_UIM1_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM1_BCR_ADDR,v)
#define HWIO_MSS_UIM1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM1_BCR_ADDR,m,v,HWIO_MSS_UIM1_BCR_IN)
#define HWIO_MSS_UIM1_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_UIM1_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001078)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x00001078)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM1_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM1_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM1_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM1_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x0000107c)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x0000107c)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM1_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM1_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM1_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM1_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM1_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001080)
#define HWIO_MSS_BUS_UIM1_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x00001080)
#define HWIO_MSS_BUS_UIM1_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM1_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, HWIO_MSS_BUS_UIM1_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM1_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM1_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM1_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM1_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM1_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM1_CBCR_IN)
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM1_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM1_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM1_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM1_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001084)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00001084)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM2_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM2_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM2_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM2_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x00001088)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x00001088)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM2_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM2_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM2_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM2_MND_M_ADDR                                              (MSS_PERPH_REG_BASE      + 0x0000108c)
#define HWIO_MSS_UIM2_MND_M_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x0000108c)
#define HWIO_MSS_UIM2_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, HWIO_MSS_UIM2_MND_M_RMSK)
#define HWIO_MSS_UIM2_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_M_ADDR, m)
#define HWIO_MSS_UIM2_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_M_ADDR,v)
#define HWIO_MSS_UIM2_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_M_ADDR,m,v,HWIO_MSS_UIM2_MND_M_IN)
#define HWIO_MSS_UIM2_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM2_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM2_MND_N_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001090)
#define HWIO_MSS_UIM2_MND_N_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00001090)
#define HWIO_MSS_UIM2_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, HWIO_MSS_UIM2_MND_N_RMSK)
#define HWIO_MSS_UIM2_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_N_ADDR, m)
#define HWIO_MSS_UIM2_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_N_ADDR,v)
#define HWIO_MSS_UIM2_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_N_ADDR,m,v,HWIO_MSS_UIM2_MND_N_IN)
#define HWIO_MSS_UIM2_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM2_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM2_MND_D_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001094)
#define HWIO_MSS_UIM2_MND_D_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00001094)
#define HWIO_MSS_UIM2_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM2_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, HWIO_MSS_UIM2_MND_D_RMSK)
#define HWIO_MSS_UIM2_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_MND_D_ADDR, m)
#define HWIO_MSS_UIM2_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_MND_D_ADDR,v)
#define HWIO_MSS_UIM2_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_MND_D_ADDR,m,v,HWIO_MSS_UIM2_MND_D_IN)
#define HWIO_MSS_UIM2_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM2_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_UIM2_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001098)
#define HWIO_MSS_UIM2_BCR_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x00001098)
#define HWIO_MSS_UIM2_BCR_RMSK                                                       0x1
#define HWIO_MSS_UIM2_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM2_BCR_ADDR, HWIO_MSS_UIM2_BCR_RMSK)
#define HWIO_MSS_UIM2_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM2_BCR_ADDR, m)
#define HWIO_MSS_UIM2_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM2_BCR_ADDR,v)
#define HWIO_MSS_UIM2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM2_BCR_ADDR,m,v,HWIO_MSS_UIM2_BCR_IN)
#define HWIO_MSS_UIM2_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_UIM2_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x0000109c)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x0000109c)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM2_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM2_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM2_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM2_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x000010a0)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x000010a0)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM2_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM2_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM2_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM2_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM2_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x000010a4)
#define HWIO_MSS_BUS_UIM2_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x000010a4)
#define HWIO_MSS_BUS_UIM2_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM2_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, HWIO_MSS_BUS_UIM2_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM2_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM2_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM2_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM2_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM2_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM2_CBCR_IN)
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM2_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM2_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM2_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM2_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM2_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010a8)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x000010a8)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_RMSK                                       0x80000003
#define HWIO_MSS_UIM3_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR, HWIO_MSS_UIM3_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_UIM3_MND_CMD_RCGR_IN)
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_UIM3_MND_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_UIM3_MND_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_UIM3_MND_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010ac)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x000010ac)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_RMSK                                           0x3000
#define HWIO_MSS_UIM3_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR, HWIO_MSS_UIM3_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_UIM3_MND_CFG_RCGR_IN)
#define HWIO_MSS_UIM3_MND_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_MSS_UIM3_MND_CFG_RCGR_MODE_SHFT                                         0xc

#define HWIO_MSS_UIM3_MND_M_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000010b0)
#define HWIO_MSS_UIM3_MND_M_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x000010b0)
#define HWIO_MSS_UIM3_MND_M_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_M_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_M_ADDR, HWIO_MSS_UIM3_MND_M_RMSK)
#define HWIO_MSS_UIM3_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_M_ADDR, m)
#define HWIO_MSS_UIM3_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_M_ADDR,v)
#define HWIO_MSS_UIM3_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_M_ADDR,m,v,HWIO_MSS_UIM3_MND_M_IN)
#define HWIO_MSS_UIM3_MND_M_M_BMSK                                                0xffff
#define HWIO_MSS_UIM3_MND_M_M_SHFT                                                   0x0

#define HWIO_MSS_UIM3_MND_N_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000010b4)
#define HWIO_MSS_UIM3_MND_N_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x000010b4)
#define HWIO_MSS_UIM3_MND_N_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_N_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_N_ADDR, HWIO_MSS_UIM3_MND_N_RMSK)
#define HWIO_MSS_UIM3_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_N_ADDR, m)
#define HWIO_MSS_UIM3_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_N_ADDR,v)
#define HWIO_MSS_UIM3_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_N_ADDR,m,v,HWIO_MSS_UIM3_MND_N_IN)
#define HWIO_MSS_UIM3_MND_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_MSS_UIM3_MND_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_MSS_UIM3_MND_D_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000010b8)
#define HWIO_MSS_UIM3_MND_D_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x000010b8)
#define HWIO_MSS_UIM3_MND_D_RMSK                                                  0xffff
#define HWIO_MSS_UIM3_MND_D_IN          \
        in_dword_masked(HWIO_MSS_UIM3_MND_D_ADDR, HWIO_MSS_UIM3_MND_D_RMSK)
#define HWIO_MSS_UIM3_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_MND_D_ADDR, m)
#define HWIO_MSS_UIM3_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_MND_D_ADDR,v)
#define HWIO_MSS_UIM3_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_MND_D_ADDR,m,v,HWIO_MSS_UIM3_MND_D_IN)
#define HWIO_MSS_UIM3_MND_D_NOT_2D_BMSK                                           0xffff
#define HWIO_MSS_UIM3_MND_D_NOT_2D_SHFT                                              0x0

#define HWIO_MSS_UIM3_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x000010bc)
#define HWIO_MSS_UIM3_BCR_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x000010bc)
#define HWIO_MSS_UIM3_BCR_RMSK                                                       0x1
#define HWIO_MSS_UIM3_BCR_IN          \
        in_dword_masked(HWIO_MSS_UIM3_BCR_ADDR, HWIO_MSS_UIM3_BCR_RMSK)
#define HWIO_MSS_UIM3_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UIM3_BCR_ADDR, m)
#define HWIO_MSS_UIM3_BCR_OUT(v)      \
        out_dword(HWIO_MSS_UIM3_BCR_ADDR,v)
#define HWIO_MSS_UIM3_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UIM3_BCR_ADDR,m,v,HWIO_MSS_UIM3_BCR_IN)
#define HWIO_MSS_UIM3_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_UIM3_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x000010c0)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x000010c0)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR, HWIO_MSS_CARD_SRC_UIM3_CBCR_RMSK)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CARD_SRC_UIM3_CBCR_ADDR,m,v,HWIO_MSS_CARD_SRC_UIM3_CBCR_IN)
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_CARD_SRC_UIM3_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x000010c4)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x000010c4)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_RMSK                                      0x80000001
#define HWIO_MSS_UART_BIT_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR, HWIO_MSS_UART_BIT_UIM3_CBCR_RMSK)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_UART_BIT_UIM3_CBCR_ADDR,m,v,HWIO_MSS_UART_BIT_UIM3_CBCR_IN)
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKOFF_BMSK                               0x80000000
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKOFF_SHFT                                     0x1f
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKEN_BMSK                                       0x1
#define HWIO_MSS_UART_BIT_UIM3_CBCR_CLKEN_SHFT                                       0x0

#define HWIO_MSS_BUS_UIM3_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x000010c8)
#define HWIO_MSS_BUS_UIM3_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x000010c8)
#define HWIO_MSS_BUS_UIM3_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_UIM3_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_UIM3_CBCR_ADDR, HWIO_MSS_BUS_UIM3_CBCR_RMSK)
#define HWIO_MSS_BUS_UIM3_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_UIM3_CBCR_ADDR, m)
#define HWIO_MSS_BUS_UIM3_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_UIM3_CBCR_ADDR,v)
#define HWIO_MSS_BUS_UIM3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_UIM3_CBCR_ADDR,m,v,HWIO_MSS_BUS_UIM3_CBCR_IN)
#define HWIO_MSS_BUS_UIM3_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_UIM3_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_UIM3_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_UIM3_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_UIM3_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_UIM3_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_UIM3_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_UIM3_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_UIM3_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_MODEM_BCR_ADDR                                               (MSS_PERPH_REG_BASE      + 0x000010cc)
#define HWIO_MSS_MODEM_BCR_OFFS                                               (MSS_PERPH_REG_BASE_OFFS + 0x000010cc)
#define HWIO_MSS_MODEM_BCR_RMSK                                                      0x1
#define HWIO_MSS_MODEM_BCR_IN          \
        in_dword_masked(HWIO_MSS_MODEM_BCR_ADDR, HWIO_MSS_MODEM_BCR_RMSK)
#define HWIO_MSS_MODEM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MODEM_BCR_ADDR, m)
#define HWIO_MSS_MODEM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MODEM_BCR_ADDR,v)
#define HWIO_MSS_MODEM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MODEM_BCR_ADDR,m,v,HWIO_MSS_MODEM_BCR_IN)
#define HWIO_MSS_MODEM_BCR_BLK_ARES_BMSK                                             0x1
#define HWIO_MSS_MODEM_BCR_BLK_ARES_SHFT                                             0x0

#define HWIO_MSS_AXI_MODEM_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x000010d0)
#define HWIO_MSS_AXI_MODEM_CBCR_OFFS                                          (MSS_PERPH_REG_BASE_OFFS + 0x000010d0)
#define HWIO_MSS_AXI_MODEM_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_AXI_MODEM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_MODEM_CBCR_ADDR, HWIO_MSS_AXI_MODEM_CBCR_RMSK)
#define HWIO_MSS_AXI_MODEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_MODEM_CBCR_ADDR, m)
#define HWIO_MSS_AXI_MODEM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_MODEM_CBCR_ADDR,v)
#define HWIO_MSS_AXI_MODEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_MODEM_CBCR_ADDR,m,v,HWIO_MSS_AXI_MODEM_CBCR_IN)
#define HWIO_MSS_AXI_MODEM_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_AXI_MODEM_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_AXI_MODEM_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_AXI_MODEM_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x000010d4)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_OFFS                                   (MSS_PERPH_REG_BASE_OFFS + 0x000010d4)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_RMSK                                   0x80000001
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR, HWIO_MSS_XO_MODEM_OFFLINE_CBCR_RMSK)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR, m)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR,v)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_MODEM_OFFLINE_CBCR_ADDR,m,v,HWIO_MSS_XO_MODEM_OFFLINE_CBCR_IN)
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_CLKOFF_BMSK                            0x80000000
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_CLKOFF_SHFT                                  0x1f
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_CLKEN_BMSK                                    0x1
#define HWIO_MSS_XO_MODEM_OFFLINE_CBCR_CLKEN_SHFT                                    0x0

#define HWIO_MSS_Q6SS_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x000010d8)
#define HWIO_MSS_Q6SS_BCR_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x000010d8)
#define HWIO_MSS_Q6SS_BCR_RMSK                                                       0x1
#define HWIO_MSS_Q6SS_BCR_IN          \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, HWIO_MSS_Q6SS_BCR_RMSK)
#define HWIO_MSS_Q6SS_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_Q6SS_BCR_ADDR, m)
#define HWIO_MSS_Q6SS_BCR_OUT(v)      \
        out_dword(HWIO_MSS_Q6SS_BCR_ADDR,v)
#define HWIO_MSS_Q6SS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_Q6SS_BCR_ADDR,m,v,HWIO_MSS_Q6SS_BCR_IN)
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_Q6SS_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_BUS_Q6SS_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x000010dc)
#define HWIO_MSS_BUS_Q6SS_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x000010dc)
#define HWIO_MSS_BUS_Q6SS_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_Q6SS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_Q6SS_CBCR_ADDR, HWIO_MSS_BUS_Q6SS_CBCR_RMSK)
#define HWIO_MSS_BUS_Q6SS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_Q6SS_CBCR_ADDR, m)
#define HWIO_MSS_BUS_Q6SS_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_Q6SS_CBCR_ADDR,v)
#define HWIO_MSS_BUS_Q6SS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_Q6SS_CBCR_ADDR,m,v,HWIO_MSS_BUS_Q6SS_CBCR_IN)
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_Q6SS_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_NAV_BCR_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x000010e0)
#define HWIO_MSS_NAV_BCR_OFFS                                                 (MSS_PERPH_REG_BASE_OFFS + 0x000010e0)
#define HWIO_MSS_NAV_BCR_RMSK                                                        0x1
#define HWIO_MSS_NAV_BCR_IN          \
        in_dword_masked(HWIO_MSS_NAV_BCR_ADDR, HWIO_MSS_NAV_BCR_RMSK)
#define HWIO_MSS_NAV_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_NAV_BCR_ADDR, m)
#define HWIO_MSS_NAV_BCR_OUT(v)      \
        out_dword(HWIO_MSS_NAV_BCR_ADDR,v)
#define HWIO_MSS_NAV_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NAV_BCR_ADDR,m,v,HWIO_MSS_NAV_BCR_IN)
#define HWIO_MSS_NAV_BCR_BLK_ARES_BMSK                                               0x1
#define HWIO_MSS_NAV_BCR_BLK_ARES_SHFT                                               0x0

#define HWIO_MSS_BUS_NAV_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x000010e4)
#define HWIO_MSS_BUS_NAV_CBCR_OFFS                                            (MSS_PERPH_REG_BASE_OFFS + 0x000010e4)
#define HWIO_MSS_BUS_NAV_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, HWIO_MSS_BUS_NAV_CBCR_RMSK)
#define HWIO_MSS_BUS_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_NAV_CBCR_ADDR, m)
#define HWIO_MSS_BUS_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_NAV_CBCR_ADDR,v)
#define HWIO_MSS_BUS_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_NAV_CBCR_ADDR,m,v,HWIO_MSS_BUS_NAV_CBCR_IN)
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_NAV_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_NAV_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_XO_NAV_CBCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x000010e8)
#define HWIO_MSS_XO_NAV_CBCR_OFFS                                             (MSS_PERPH_REG_BASE_OFFS + 0x000010e8)
#define HWIO_MSS_XO_NAV_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_XO_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_NAV_CBCR_ADDR, HWIO_MSS_XO_NAV_CBCR_RMSK)
#define HWIO_MSS_XO_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_NAV_CBCR_ADDR, m)
#define HWIO_MSS_XO_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_NAV_CBCR_ADDR,v)
#define HWIO_MSS_XO_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_NAV_CBCR_ADDR,m,v,HWIO_MSS_XO_NAV_CBCR_IN)
#define HWIO_MSS_XO_NAV_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_XO_NAV_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_XO_NAV_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_XO_NAV_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR                                       (MSS_PERPH_REG_BASE      + 0x000010ec)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_OFFS                                       (MSS_PERPH_REG_BASE_OFFS + 0x000010ec)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_RMSK                                              0x1
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_IN          \
        in_dword_masked(HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR, HWIO_MSS_CE_NAV_BRIDGE_BCR_RMSK)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR, m)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_OUT(v)      \
        out_dword(HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR,v)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CE_NAV_BRIDGE_BCR_ADDR,m,v,HWIO_MSS_CE_NAV_BRIDGE_BCR_IN)
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_BLK_ARES_BMSK                                     0x1
#define HWIO_MSS_CE_NAV_BRIDGE_BCR_BLK_ARES_SHFT                                     0x0

#define HWIO_MSS_AXI_CE_NAV_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x000010f0)
#define HWIO_MSS_AXI_CE_NAV_CBCR_OFFS                                         (MSS_PERPH_REG_BASE_OFFS + 0x000010f0)
#define HWIO_MSS_AXI_CE_NAV_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_AXI_CE_NAV_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_CE_NAV_CBCR_ADDR, HWIO_MSS_AXI_CE_NAV_CBCR_RMSK)
#define HWIO_MSS_AXI_CE_NAV_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_CE_NAV_CBCR_ADDR, m)
#define HWIO_MSS_AXI_CE_NAV_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_CE_NAV_CBCR_ADDR,v)
#define HWIO_MSS_AXI_CE_NAV_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_CE_NAV_CBCR_ADDR,m,v,HWIO_MSS_AXI_CE_NAV_CBCR_IN)
#define HWIO_MSS_AXI_CE_NAV_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_AXI_CE_NAV_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_AXI_CE_NAV_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_AXI_CE_NAV_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x000010f4)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_OFFS                                  (MSS_PERPH_REG_BASE_OFFS + 0x000010f4)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_RMSK                                  0x80000001
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR, HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_RMSK)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_ADDR,m,v,HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_IN)
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_CLKOFF_BMSK                           0x80000000
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_CLKOFF_SHFT                                 0x1f
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_CLKEN_BMSK                                   0x1
#define HWIO_MSS_BUS_CE_NAV_BRIDGE_CBCR_CLKEN_SHFT                                   0x0

#define HWIO_MSS_AXI_VMIDMT_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x000010f8)
#define HWIO_MSS_AXI_VMIDMT_CBCR_OFFS                                         (MSS_PERPH_REG_BASE_OFFS + 0x000010f8)
#define HWIO_MSS_AXI_VMIDMT_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_AXI_VMIDMT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_AXI_VMIDMT_CBCR_ADDR, HWIO_MSS_AXI_VMIDMT_CBCR_RMSK)
#define HWIO_MSS_AXI_VMIDMT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_AXI_VMIDMT_CBCR_ADDR, m)
#define HWIO_MSS_AXI_VMIDMT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_AXI_VMIDMT_CBCR_ADDR,v)
#define HWIO_MSS_AXI_VMIDMT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_AXI_VMIDMT_CBCR_ADDR,m,v,HWIO_MSS_AXI_VMIDMT_CBCR_IN)
#define HWIO_MSS_AXI_VMIDMT_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_AXI_VMIDMT_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_AXI_VMIDMT_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_AXI_VMIDMT_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_CRYPTO_BCR_ADDR                                              (MSS_PERPH_REG_BASE      + 0x000010fc)
#define HWIO_MSS_CRYPTO_BCR_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x000010fc)
#define HWIO_MSS_CRYPTO_BCR_RMSK                                                     0x1
#define HWIO_MSS_CRYPTO_BCR_IN          \
        in_dword_masked(HWIO_MSS_CRYPTO_BCR_ADDR, HWIO_MSS_CRYPTO_BCR_RMSK)
#define HWIO_MSS_CRYPTO_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_CRYPTO_BCR_ADDR, m)
#define HWIO_MSS_CRYPTO_BCR_OUT(v)      \
        out_dword(HWIO_MSS_CRYPTO_BCR_ADDR,v)
#define HWIO_MSS_CRYPTO_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_CRYPTO_BCR_ADDR,m,v,HWIO_MSS_CRYPTO_BCR_IN)
#define HWIO_MSS_CRYPTO_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_MSS_CRYPTO_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_MSS_BUS_CRYPTO_CBCR_ADDR                                         (MSS_PERPH_REG_BASE      + 0x00001100)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OFFS                                         (MSS_PERPH_REG_BASE_OFFS + 0x00001100)
#define HWIO_MSS_BUS_CRYPTO_CBCR_RMSK                                         0x80000001
#define HWIO_MSS_BUS_CRYPTO_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, HWIO_MSS_BUS_CRYPTO_CBCR_RMSK)
#define HWIO_MSS_BUS_CRYPTO_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR, m)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,v)
#define HWIO_MSS_BUS_CRYPTO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CRYPTO_CBCR_ADDR,m,v,HWIO_MSS_BUS_CRYPTO_CBCR_IN)
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_MSS_BUS_CRYPTO_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001104)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OFFS                                   (MSS_PERPH_REG_BASE_OFFS + 0x00001104)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK                                   0x80000003
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CMD_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CMD_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_BMSK                          0x80000000
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_OFF_SHFT                                0x1f
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_BMSK                                  0x2
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_ROOT_EN_SHFT                                  0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_BMSK                                   0x1
#define HWIO_MSS_BIT_COXM_MND_CMD_RCGR_UPDATE_SHFT                                   0x0

#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR                                   (MSS_PERPH_REG_BASE      + 0x00001108)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OFFS                                   (MSS_PERPH_REG_BASE_OFFS + 0x00001108)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK                                       0x371f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, HWIO_MSS_BIT_COXM_MND_CFG_RCGR_RMSK)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_CFG_RCGR_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_CFG_RCGR_IN)
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_BMSK                                  0x3000
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_MODE_SHFT                                     0xc
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_BMSK                                0x700
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_SEL_SHFT                                  0x8
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_BMSK                                 0x1f
#define HWIO_MSS_BIT_COXM_MND_CFG_RCGR_SRC_DIV_SHFT                                  0x0

#define HWIO_MSS_BIT_COXM_MND_M_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000110c)
#define HWIO_MSS_BIT_COXM_MND_M_OFFS                                          (MSS_PERPH_REG_BASE_OFFS + 0x0000110c)
#define HWIO_MSS_BIT_COXM_MND_M_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_M_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, HWIO_MSS_BIT_COXM_MND_M_RMSK)
#define HWIO_MSS_BIT_COXM_MND_M_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_M_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_M_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_M_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_M_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_M_IN)
#define HWIO_MSS_BIT_COXM_MND_M_M_BMSK                                            0xffff
#define HWIO_MSS_BIT_COXM_MND_M_M_SHFT                                               0x0

#define HWIO_MSS_BIT_COXM_MND_N_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001110)
#define HWIO_MSS_BIT_COXM_MND_N_OFFS                                          (MSS_PERPH_REG_BASE_OFFS + 0x00001110)
#define HWIO_MSS_BIT_COXM_MND_N_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_N_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, HWIO_MSS_BIT_COXM_MND_N_RMSK)
#define HWIO_MSS_BIT_COXM_MND_N_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_N_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_N_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_N_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_N_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_N_IN)
#define HWIO_MSS_BIT_COXM_MND_N_NOT_N_MINUS_M_BMSK                                0xffff
#define HWIO_MSS_BIT_COXM_MND_N_NOT_N_MINUS_M_SHFT                                   0x0

#define HWIO_MSS_BIT_COXM_MND_D_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001114)
#define HWIO_MSS_BIT_COXM_MND_D_OFFS                                          (MSS_PERPH_REG_BASE_OFFS + 0x00001114)
#define HWIO_MSS_BIT_COXM_MND_D_RMSK                                              0xffff
#define HWIO_MSS_BIT_COXM_MND_D_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, HWIO_MSS_BIT_COXM_MND_D_RMSK)
#define HWIO_MSS_BIT_COXM_MND_D_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_MND_D_ADDR, m)
#define HWIO_MSS_BIT_COXM_MND_D_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_MND_D_ADDR,v)
#define HWIO_MSS_BIT_COXM_MND_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_MND_D_ADDR,m,v,HWIO_MSS_BIT_COXM_MND_D_IN)
#define HWIO_MSS_BIT_COXM_MND_D_NOT_2D_BMSK                                       0xffff
#define HWIO_MSS_BIT_COXM_MND_D_NOT_2D_SHFT                                          0x0

#define HWIO_MSS_COXM_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001118)
#define HWIO_MSS_COXM_BCR_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x00001118)
#define HWIO_MSS_COXM_BCR_RMSK                                                       0x1
#define HWIO_MSS_COXM_BCR_IN          \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, HWIO_MSS_COXM_BCR_RMSK)
#define HWIO_MSS_COXM_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_COXM_BCR_ADDR, m)
#define HWIO_MSS_COXM_BCR_OUT(v)      \
        out_dword(HWIO_MSS_COXM_BCR_ADDR,v)
#define HWIO_MSS_COXM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_COXM_BCR_ADDR,m,v,HWIO_MSS_COXM_BCR_IN)
#define HWIO_MSS_COXM_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_COXM_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_BUS_COXM_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000111c)
#define HWIO_MSS_BUS_COXM_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x0000111c)
#define HWIO_MSS_BUS_COXM_CBCR_RMSK                                           0x80007ff1
#define HWIO_MSS_BUS_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, HWIO_MSS_BUS_COXM_CBCR_RMSK)
#define HWIO_MSS_BUS_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BUS_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BUS_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_COXM_CBCR_ADDR,m,v,HWIO_MSS_BUS_COXM_CBCR_IN)
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_COXM_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_MSS_BUS_COXM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_MSS_BUS_COXM_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_MSS_BUS_COXM_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_COXM_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_BIT_COXM_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001120)
#define HWIO_MSS_BIT_COXM_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x00001120)
#define HWIO_MSS_BIT_COXM_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BIT_COXM_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, HWIO_MSS_BIT_COXM_CBCR_RMSK)
#define HWIO_MSS_BIT_COXM_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BIT_COXM_CBCR_ADDR, m)
#define HWIO_MSS_BIT_COXM_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BIT_COXM_CBCR_ADDR,v)
#define HWIO_MSS_BIT_COXM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BIT_COXM_CBCR_ADDR,m,v,HWIO_MSS_BIT_COXM_CBCR_IN)
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BIT_COXM_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BIT_COXM_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001124)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x00001124)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_RMSK                                      0x80000003
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR, HWIO_MSS_RBCPR_REF_CMD_RCGR_RMSK)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CMD_RCGR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CMD_RCGR_IN)
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_OFF_BMSK                             0x80000000
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_OFF_SHFT                                   0x1f
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_EN_BMSK                                     0x2
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_ROOT_EN_SHFT                                     0x1
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_UPDATE_BMSK                                      0x1
#define HWIO_MSS_RBCPR_REF_CMD_RCGR_UPDATE_SHFT                                      0x0

#define HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR                                      (MSS_PERPH_REG_BASE      + 0x00001128)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OFFS                                      (MSS_PERPH_REG_BASE_OFFS + 0x00001128)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_RMSK                                           0x71f
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR, HWIO_MSS_RBCPR_REF_CFG_RCGR_RMSK)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CFG_RCGR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CFG_RCGR_IN)
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_SEL_BMSK                                   0x700
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_SEL_SHFT                                     0x8
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_DIV_BMSK                                    0x1f
#define HWIO_MSS_RBCPR_REF_CFG_RCGR_SRC_DIV_SHFT                                     0x0

#define HWIO_MSS_RBCPR_BCR_ADDR                                               (MSS_PERPH_REG_BASE      + 0x0000112c)
#define HWIO_MSS_RBCPR_BCR_OFFS                                               (MSS_PERPH_REG_BASE_OFFS + 0x0000112c)
#define HWIO_MSS_RBCPR_BCR_RMSK                                                      0x1
#define HWIO_MSS_RBCPR_BCR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_BCR_ADDR, HWIO_MSS_RBCPR_BCR_RMSK)
#define HWIO_MSS_RBCPR_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_BCR_ADDR, m)
#define HWIO_MSS_RBCPR_BCR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_BCR_ADDR,v)
#define HWIO_MSS_RBCPR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_BCR_ADDR,m,v,HWIO_MSS_RBCPR_BCR_IN)
#define HWIO_MSS_RBCPR_BCR_BLK_ARES_BMSK                                             0x1
#define HWIO_MSS_RBCPR_BCR_BLK_ARES_SHFT                                             0x0

#define HWIO_MSS_BUS_RBCPR_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001130)
#define HWIO_MSS_BUS_RBCPR_CBCR_OFFS                                          (MSS_PERPH_REG_BASE_OFFS + 0x00001130)
#define HWIO_MSS_BUS_RBCPR_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_BUS_RBCPR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, HWIO_MSS_BUS_RBCPR_CBCR_RMSK)
#define HWIO_MSS_BUS_RBCPR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_RBCPR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_RBCPR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_RBCPR_CBCR_ADDR,m,v,HWIO_MSS_BUS_RBCPR_CBCR_IN)
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_BUS_RBCPR_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_RBCPR_REF_CBCR_ADDR                                          (MSS_PERPH_REG_BASE      + 0x00001134)
#define HWIO_MSS_RBCPR_REF_CBCR_OFFS                                          (MSS_PERPH_REG_BASE_OFFS + 0x00001134)
#define HWIO_MSS_RBCPR_REF_CBCR_RMSK                                          0x80000001
#define HWIO_MSS_RBCPR_REF_CBCR_IN          \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CBCR_ADDR, HWIO_MSS_RBCPR_REF_CBCR_RMSK)
#define HWIO_MSS_RBCPR_REF_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_RBCPR_REF_CBCR_ADDR, m)
#define HWIO_MSS_RBCPR_REF_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_RBCPR_REF_CBCR_ADDR,v)
#define HWIO_MSS_RBCPR_REF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RBCPR_REF_CBCR_ADDR,m,v,HWIO_MSS_RBCPR_REF_CBCR_IN)
#define HWIO_MSS_RBCPR_REF_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_MSS_RBCPR_REF_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_MSS_RBCPR_REF_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_MSS_RBCPR_REF_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_MSS_MGPI_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001138)
#define HWIO_MSS_MGPI_BCR_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x00001138)
#define HWIO_MSS_MGPI_BCR_RMSK                                                       0x1
#define HWIO_MSS_MGPI_BCR_IN          \
        in_dword_masked(HWIO_MSS_MGPI_BCR_ADDR, HWIO_MSS_MGPI_BCR_RMSK)
#define HWIO_MSS_MGPI_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MGPI_BCR_ADDR, m)
#define HWIO_MSS_MGPI_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MGPI_BCR_ADDR,v)
#define HWIO_MSS_MGPI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MGPI_BCR_ADDR,m,v,HWIO_MSS_MGPI_BCR_IN)
#define HWIO_MSS_MGPI_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_MGPI_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_BUS_MGPI_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x0000113c)
#define HWIO_MSS_BUS_MGPI_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x0000113c)
#define HWIO_MSS_BUS_MGPI_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_MGPI_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, HWIO_MSS_BUS_MGPI_CBCR_RMSK)
#define HWIO_MSS_BUS_MGPI_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MGPI_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MGPI_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MGPI_CBCR_ADDR,v)
#define HWIO_MSS_BUS_MGPI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MGPI_CBCR_ADDR,m,v,HWIO_MSS_BUS_MGPI_CBCR_IN)
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_MGPI_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_MGPI_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_BUS_ATB_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001140)
#define HWIO_MSS_BUS_ATB_CBCR_OFFS                                            (MSS_PERPH_REG_BASE_OFFS + 0x00001140)
#define HWIO_MSS_BUS_ATB_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_ATB_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, HWIO_MSS_BUS_ATB_CBCR_RMSK)
#define HWIO_MSS_BUS_ATB_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_ATB_CBCR_ADDR, m)
#define HWIO_MSS_BUS_ATB_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_ATB_CBCR_ADDR,v)
#define HWIO_MSS_BUS_ATB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_ATB_CBCR_ADDR,m,v,HWIO_MSS_BUS_ATB_CBCR_IN)
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_ATB_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_ATB_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR                                  (MSS_PERPH_REG_BASE      + 0x00001144)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OFFS                                  (MSS_PERPH_REG_BASE_OFFS + 0x00001144)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK                                  0x80000001
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_RMSK)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR, m)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,v)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_ADDR,m,v,HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_IN)
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_BMSK                           0x80000000
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKOFF_SHFT                                 0x1f
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_BMSK                                   0x1
#define HWIO_MSS_BUS_SLAVE_TIMEOUT_CBCR_CLKEN_SHFT                                   0x0

#define HWIO_MSS_MVC_BCR_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x00001148)
#define HWIO_MSS_MVC_BCR_OFFS                                                 (MSS_PERPH_REG_BASE_OFFS + 0x00001148)
#define HWIO_MSS_MVC_BCR_RMSK                                                        0x1
#define HWIO_MSS_MVC_BCR_IN          \
        in_dword_masked(HWIO_MSS_MVC_BCR_ADDR, HWIO_MSS_MVC_BCR_RMSK)
#define HWIO_MSS_MVC_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MVC_BCR_ADDR, m)
#define HWIO_MSS_MVC_BCR_OUT(v)      \
        out_dword(HWIO_MSS_MVC_BCR_ADDR,v)
#define HWIO_MSS_MVC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MVC_BCR_ADDR,m,v,HWIO_MSS_MVC_BCR_IN)
#define HWIO_MSS_MVC_BCR_BLK_ARES_BMSK                                               0x1
#define HWIO_MSS_MVC_BCR_BLK_ARES_SHFT                                               0x0

#define HWIO_MSS_BUS_MVC_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x0000114c)
#define HWIO_MSS_BUS_MVC_CBCR_OFFS                                            (MSS_PERPH_REG_BASE_OFFS + 0x0000114c)
#define HWIO_MSS_BUS_MVC_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_BUS_MVC_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_MVC_CBCR_ADDR, HWIO_MSS_BUS_MVC_CBCR_RMSK)
#define HWIO_MSS_BUS_MVC_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_MVC_CBCR_ADDR, m)
#define HWIO_MSS_BUS_MVC_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_MVC_CBCR_ADDR,v)
#define HWIO_MSS_BUS_MVC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_MVC_CBCR_ADDR,m,v,HWIO_MSS_BUS_MVC_CBCR_IN)
#define HWIO_MSS_BUS_MVC_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_BUS_MVC_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_BUS_MVC_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_BUS_MVC_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_STMR_BCR_ADDR                                                (MSS_PERPH_REG_BASE      + 0x00001150)
#define HWIO_MSS_STMR_BCR_OFFS                                                (MSS_PERPH_REG_BASE_OFFS + 0x00001150)
#define HWIO_MSS_STMR_BCR_RMSK                                                       0x1
#define HWIO_MSS_STMR_BCR_IN          \
        in_dword_masked(HWIO_MSS_STMR_BCR_ADDR, HWIO_MSS_STMR_BCR_RMSK)
#define HWIO_MSS_STMR_BCR_INM(m)      \
        in_dword_masked(HWIO_MSS_STMR_BCR_ADDR, m)
#define HWIO_MSS_STMR_BCR_OUT(v)      \
        out_dword(HWIO_MSS_STMR_BCR_ADDR,v)
#define HWIO_MSS_STMR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_STMR_BCR_ADDR,m,v,HWIO_MSS_STMR_BCR_IN)
#define HWIO_MSS_STMR_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_MSS_STMR_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_MSS_BUS_STMR_CBCR_ADDR                                           (MSS_PERPH_REG_BASE      + 0x00001154)
#define HWIO_MSS_BUS_STMR_CBCR_OFFS                                           (MSS_PERPH_REG_BASE_OFFS + 0x00001154)
#define HWIO_MSS_BUS_STMR_CBCR_RMSK                                           0x80000001
#define HWIO_MSS_BUS_STMR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_BUS_STMR_CBCR_ADDR, HWIO_MSS_BUS_STMR_CBCR_RMSK)
#define HWIO_MSS_BUS_STMR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_STMR_CBCR_ADDR, m)
#define HWIO_MSS_BUS_STMR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_STMR_CBCR_ADDR,v)
#define HWIO_MSS_BUS_STMR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_STMR_CBCR_ADDR,m,v,HWIO_MSS_BUS_STMR_CBCR_IN)
#define HWIO_MSS_BUS_STMR_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_MSS_BUS_STMR_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_MSS_BUS_STMR_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_MSS_BUS_STMR_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_MSS_XO_STMR_CBCR_ADDR                                            (MSS_PERPH_REG_BASE      + 0x00001158)
#define HWIO_MSS_XO_STMR_CBCR_OFFS                                            (MSS_PERPH_REG_BASE_OFFS + 0x00001158)
#define HWIO_MSS_XO_STMR_CBCR_RMSK                                            0x80000001
#define HWIO_MSS_XO_STMR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_STMR_CBCR_ADDR, HWIO_MSS_XO_STMR_CBCR_RMSK)
#define HWIO_MSS_XO_STMR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_STMR_CBCR_ADDR, m)
#define HWIO_MSS_XO_STMR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_STMR_CBCR_ADDR,v)
#define HWIO_MSS_XO_STMR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_STMR_CBCR_ADDR,m,v,HWIO_MSS_XO_STMR_CBCR_IN)
#define HWIO_MSS_XO_STMR_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_MSS_XO_STMR_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_MSS_XO_STMR_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_MSS_XO_STMR_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_MSS_XO_DTR_CBCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x0000115c)
#define HWIO_MSS_XO_DTR_CBCR_OFFS                                             (MSS_PERPH_REG_BASE_OFFS + 0x0000115c)
#define HWIO_MSS_XO_DTR_CBCR_RMSK                                             0x80000001
#define HWIO_MSS_XO_DTR_CBCR_IN          \
        in_dword_masked(HWIO_MSS_XO_DTR_CBCR_ADDR, HWIO_MSS_XO_DTR_CBCR_RMSK)
#define HWIO_MSS_XO_DTR_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_XO_DTR_CBCR_ADDR, m)
#define HWIO_MSS_XO_DTR_CBCR_OUT(v)      \
        out_dword(HWIO_MSS_XO_DTR_CBCR_ADDR,v)
#define HWIO_MSS_XO_DTR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_XO_DTR_CBCR_ADDR,m,v,HWIO_MSS_XO_DTR_CBCR_IN)
#define HWIO_MSS_XO_DTR_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_XO_DTR_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_MSS_XO_DTR_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_MSS_XO_DTR_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_MSS_SW_FEATURE_FUSES_ADDR                                        (MSS_PERPH_REG_BASE      + 0x00001170)
#define HWIO_MSS_SW_FEATURE_FUSES_OFFS                                        (MSS_PERPH_REG_BASE_OFFS + 0x00001170)
#define HWIO_MSS_SW_FEATURE_FUSES_RMSK                                        0xffffffff
#define HWIO_MSS_SW_FEATURE_FUSES_IN          \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES_ADDR, HWIO_MSS_SW_FEATURE_FUSES_RMSK)
#define HWIO_MSS_SW_FEATURE_FUSES_INM(m)      \
        in_dword_masked(HWIO_MSS_SW_FEATURE_FUSES_ADDR, m)
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT2_BMSK                   0x80000000
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT2_SHFT                         0x1f
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT1_BMSK                   0x40000000
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT1_SHFT                         0x1e
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT0_BMSK                   0x20000000
#define HWIO_MSS_SW_FEATURE_FUSES_MSMC_MDSP_FW_EN_BIT0_SHFT                         0x1d
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_DSDA_BMSK                        0x10000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_DSDA_SHFT                              0x1c
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_MODEM_BMSK                        0x8000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_MODEM_SHFT                             0x1b
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_ADC3_BMSK                         0x4000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_ADC3_SHFT                              0x1a
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_ADC2_BMSK                         0x2000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_ADC2_SHFT                              0x19
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_UL_CA_BMSK                    0x1000000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_UL_CA_SHFT                         0x18
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_60MHZ_BMSK                     0x800000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_60MHZ_SHFT                         0x17
#define HWIO_MSS_SW_FEATURE_FUSES_FETAURE_EN_LTE_40MHZ_BMSK                     0x400000
#define HWIO_MSS_SW_FEATURE_FUSES_FETAURE_EN_LTE_40MHZ_SHFT                         0x16
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_GERAN_BMSK                         0x300000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_GERAN_SHFT                             0x14
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_TDSCDMA_BMSK                        0xc0000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_TDSCDMA_SHFT                           0x12
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_ABOVE_CAT1_BMSK                 0x30000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_ABOVE_CAT1_SHFT                    0x10
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_ABOVE_CAT2_BMSK                  0xc000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_ABOVE_CAT2_SHFT                     0xe
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_BMSK                             0x3000
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_LTE_SHFT                                0xc
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_DC_BMSK                         0xc00
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_DC_SHFT                           0xa
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_MIMO_BMSK                       0x300
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_MIMO_SHFT                         0x8
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_BMSK                             0xc0
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_HSDPA_SHFT                              0x6
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_WCDMA_BMSK                             0x30
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_WCDMA_SHFT                              0x4
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_DO_BMSK                                 0xc
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_DO_SHFT                                 0x2
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_1X_BMSK                                 0x3
#define HWIO_MSS_SW_FEATURE_FUSES_FEATURE_EN_1X_SHFT                                 0x0

#define HWIO_MSS_RESERVE_02_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001174)
#define HWIO_MSS_RESERVE_02_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00001174)
#define HWIO_MSS_RESERVE_02_RMSK                                              0xffffffff
#define HWIO_MSS_RESERVE_02_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, HWIO_MSS_RESERVE_02_RMSK)
#define HWIO_MSS_RESERVE_02_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_02_ADDR, m)
#define HWIO_MSS_RESERVE_02_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_02_ADDR,v)
#define HWIO_MSS_RESERVE_02_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_02_ADDR,m,v,HWIO_MSS_RESERVE_02_IN)
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_BMSK                               0xffffffff
#define HWIO_MSS_RESERVE_02_MSS_RESERVE_02_SHFT                                      0x0

#define HWIO_MSS_RESERVE_03_ADDR                                              (MSS_PERPH_REG_BASE      + 0x00001178)
#define HWIO_MSS_RESERVE_03_OFFS                                              (MSS_PERPH_REG_BASE_OFFS + 0x00001178)
#define HWIO_MSS_RESERVE_03_RMSK                                              0xffffffff
#define HWIO_MSS_RESERVE_03_IN          \
        in_dword_masked(HWIO_MSS_RESERVE_03_ADDR, HWIO_MSS_RESERVE_03_RMSK)
#define HWIO_MSS_RESERVE_03_INM(m)      \
        in_dword_masked(HWIO_MSS_RESERVE_03_ADDR, m)
#define HWIO_MSS_RESERVE_03_OUT(v)      \
        out_dword(HWIO_MSS_RESERVE_03_ADDR,v)
#define HWIO_MSS_RESERVE_03_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_RESERVE_03_ADDR,m,v,HWIO_MSS_RESERVE_03_IN)
#define HWIO_MSS_RESERVE_03_MSS_RESERVE_03_BMSK                               0xffffffff
#define HWIO_MSS_RESERVE_03_MSS_RESERVE_03_SHFT                                      0x0


#endif /* __MPSS_8994_HAL_SLEEP_HWIO_H__ */
