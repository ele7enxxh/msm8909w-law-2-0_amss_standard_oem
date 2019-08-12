#ifndef __HALDOGHWIO_H__
#define __HALDOGHWIO_H__
/*
===========================================================================
*/
/**
  @file HALdogHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_QDSP6SS_CSR

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/debugtools/dog/src/hal/8974/modem/HALdogHWIO.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MSS_QDSP6SS_CSR
 *--------------------------------------------------------------------------*/

#define MSS_QDSP6SS_CSR_REG_BASE                                          (MSS_TOP_BASE      + 0x00100000)

#define HWIO_MSS_QDSP6SS_INSTANCE_ID_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00000000)
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_RMSK                                        0x7
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_INSTANCE_ID_ADDR, HWIO_MSS_QDSP6SS_INSTANCE_ID_RMSK)
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_INSTANCE_ID_ADDR, m)
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_ID_BMSK                                     0x7
#define HWIO_MSS_QDSP6SS_INSTANCE_ID_ID_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_IDLT_MATCH_VAL_ADDR                              (MSS_QDSP6SS_CSR_REG_BASE      + 0x00001000)
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
#define HWIO_MSS_QDSP6SS_WDOG_RESET_RMSK                                         0x1
#define HWIO_MSS_QDSP6SS_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_RESET_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_RESET_WDOG_RESET_BMSK                              0x1
#define HWIO_MSS_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_WDOG_CTL_ADDR                                    (MSS_QDSP6SS_CSR_REG_BASE      + 0x00002004)
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
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_RMSK                                     0x1
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_LOAD_BMSK                                0x1
#define HWIO_MSS_QDSP6SS_WDOG_TEST_LOAD_LOAD_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_WDOG_TEST_ADDR                                   (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000201c)
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
#define HWIO_MSS_QDSP6SS_IPC_RMSK                                         0xffffffff
#define HWIO_MSS_QDSP6SS_IPC_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_IPC_ADDR,v)
#define HWIO_MSS_QDSP6SS_IPC_INTR_BMSK                                    0xffffffff
#define HWIO_MSS_QDSP6SS_IPC_INTR_SHFT                                           0x0

#define HWIO_MSS_QDSP6SS_L1_IRQ_BACKUP_EN_ADDR                            (MSS_QDSP6SS_CSR_REG_BASE      + 0x00005010)
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

#define HWIO_MSS_QDSP6SS_PD_HS_START_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000700c)
#define HWIO_MSS_QDSP6SS_PD_HS_START_RMSK                                        0x1
#define HWIO_MSS_QDSP6SS_PD_HS_START_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PD_HS_START_ADDR,v)
#define HWIO_MSS_QDSP6SS_PD_HS_START_CMD_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_PD_HS_START_CMD_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_PD_HS_DONE_ADDR                                  (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007010)
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_RMSK                                         0x1
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PD_HS_DONE_ADDR, HWIO_MSS_QDSP6SS_PD_HS_DONE_RMSK)
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PD_HS_DONE_ADDR, m)
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_DONE_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_PD_HS_DONE_DONE_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_PU_HS_START_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007014)
#define HWIO_MSS_QDSP6SS_PU_HS_START_RMSK                                        0x1
#define HWIO_MSS_QDSP6SS_PU_HS_START_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PU_HS_START_ADDR,v)
#define HWIO_MSS_QDSP6SS_PU_HS_START_CMD_BMSK                                    0x1
#define HWIO_MSS_QDSP6SS_PU_HS_START_CMD_SHFT                                    0x0

#define HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR                                    (MSS_QDSP6SS_CSR_REG_BASE      + 0x0000701c)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_RMSK                                          0x1f
#define HWIO_MSS_QDSP6SS_SLPC_CFG_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR, HWIO_MSS_QDSP6SS_SLPC_CFG_RMSK)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR, m)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR,v)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SLPC_CFG_ADDR,m,v,HWIO_MSS_QDSP6SS_SLPC_CFG_IN)
#define HWIO_MSS_QDSP6SS_SLPC_CFG_EXT_AVS_SRC_EN_BMSK                           0x10
#define HWIO_MSS_QDSP6SS_SLPC_CFG_EXT_AVS_SRC_EN_SHFT                            0x4
#define HWIO_MSS_QDSP6SS_SLPC_CFG_SAW_DBG_BMSK                                   0x8
#define HWIO_MSS_QDSP6SS_SLPC_CFG_SAW_DBG_SHFT                                   0x3
#define HWIO_MSS_QDSP6SS_SLPC_CFG_PD_SRC_SEL_BMSK                                0x6
#define HWIO_MSS_QDSP6SS_SLPC_CFG_PD_SRC_SEL_SHFT                                0x1
#define HWIO_MSS_QDSP6SS_SLPC_CFG_EXT_WAKEUP_ENA_BMSK                            0x1
#define HWIO_MSS_QDSP6SS_SLPC_CFG_EXT_WAKEUP_ENA_SHFT                            0x0

#define HWIO_MSS_QDSP6SS_SLPC_STATUS_ADDR                                 (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007020)
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_RMSK                                        0x3
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLPC_STATUS_ADDR, HWIO_MSS_QDSP6SS_SLPC_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SLPC_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_FSM_STATE_BMSK                              0x3
#define HWIO_MSS_QDSP6SS_SLPC_STATUS_FSM_STATE_SHFT                              0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR                              (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007024)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_RMSK                                    0x3f
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_ARES_IN)
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
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_RMSK                                     0x3d
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_CLK_IN)
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
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_RMSK                                  0xf0007
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR, HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_RMSK)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR, m)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR,v)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ADDR,m,v,HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_IN)
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_MEM_PERIPH_BMSK                       0x80000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_MEM_PERIPH_SHFT                          0x13
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2DATA_STBY_BMSK                      0x40000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2DATA_STBY_SHFT                         0x12
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ETB_BMSK                              0x20000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_ETB_SHFT                                 0x11
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2TAG_BMSK                            0x10000
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2TAG_SHFT                               0x10
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2DATA_BMSK                               0x7
#define HWIO_MSS_QDSP6SS_SPMCTL_EN_MEM_L2DATA_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_SPMCTL_EN_BHS_ADDR                               (MSS_QDSP6SS_CSR_REG_BASE      + 0x00007034)
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


#endif /* __HALDOGHWIO_H__ */
