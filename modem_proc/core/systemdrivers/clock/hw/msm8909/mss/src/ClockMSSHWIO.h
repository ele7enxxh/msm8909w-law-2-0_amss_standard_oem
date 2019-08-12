#ifndef __CLOCKMSSHWIO_H__
#define __CLOCKMSSHWIO_H__
/*
===========================================================================
*/
/**
  @file ClockMSSHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_PERPH
    SECURITY_CONTROL_CORE

  'Include' filters applied: MSS_TCSR_ACC_SEL, CORR_PTE1*[SECURITY_CONTROL_CORE]  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/hw/msm8909/mss/src/ClockMSSHWIO.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include <HALhwio.h>

/*
 * HWIO base definitions
 */
extern uint32                      HAL_clk_nHWIOBaseMSS;
#define MSS_TOP_BASE               HAL_clk_nHWIOBaseMSS
#define MSS_TOP_BASE_PHYS          0x04000000
#define MSS_TOP_BASE_SIZE          0x400000

extern uint32 			   HAL_clk_nHWIOBaseSecurity;
#define SECURITY_CONTROL_BASE      HAL_clk_nHWIOBaseSecurity
#define SECURITY_CONTROL_BASE_PHYS 0x00058000
#define SECURITY_CONTROL_BASE_SIZE 0x00008000


/*----------------------------------------------------------------------------
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                  (MSS_TOP_BASE      + 0x00180000)
#define MSS_PERPH_REG_BASE_OFFS                                             0x00180000

#define HWIO_MSS_TCSR_ACC_SEL_ADDR                                          (MSS_PERPH_REG_BASE      + 0x0000f000)
#define HWIO_MSS_TCSR_ACC_SEL_OFFS                                          (MSS_PERPH_REG_BASE_OFFS + 0x0000f000)
#define HWIO_MSS_TCSR_ACC_SEL_RMSK                                                 0x3
#define HWIO_MSS_TCSR_ACC_SEL_IN          \
        in_dword_masked(HWIO_MSS_TCSR_ACC_SEL_ADDR, HWIO_MSS_TCSR_ACC_SEL_RMSK)
#define HWIO_MSS_TCSR_ACC_SEL_INM(m)      \
        in_dword_masked(HWIO_MSS_TCSR_ACC_SEL_ADDR, m)
#define HWIO_MSS_TCSR_ACC_SEL_OUT(v)      \
        out_dword(HWIO_MSS_TCSR_ACC_SEL_ADDR,v)
#define HWIO_MSS_TCSR_ACC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_TCSR_ACC_SEL_ADDR,m,v,HWIO_MSS_TCSR_ACC_SEL_IN)
#define HWIO_MSS_TCSR_ACC_SEL_ACC_MEM_SEL_BMSK                                     0x3
#define HWIO_MSS_TCSR_ACC_SEL_ACC_MEM_SEL_SHFT                                     0x0

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                        (SECURITY_CONTROL_BASE      + 0x00000000)

#define HWIO_QFPROM_CORR_PTE1_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004004)
#define HWIO_QFPROM_CORR_PTE1_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE1_POR                                                             0x00000000
#define HWIO_QFPROM_CORR_PTE1_POR_RMSK                                                        0x00000000
#define HWIO_QFPROM_CORR_PTE1_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, HWIO_QFPROM_CORR_PTE1_RMSK)
#define HWIO_QFPROM_CORR_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, m)
#define HWIO_QFPROM_CORR_PTE1_IDDQ_BINNING_BMSK                                               0xfc000000
#define HWIO_QFPROM_CORR_PTE1_IDDQ_BINNING_SHFT                                                     0x1a
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_BMSK                                             0x2000000
#define HWIO_QFPROM_CORR_PTE1_PROCESS_NODE_ID_SHFT                                                  0x19
#define HWIO_QFPROM_CORR_PTE1_PTE_DATA1_BMSK                                                   0x1ffffff
#define HWIO_QFPROM_CORR_PTE1_PTE_DATA1_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_PTE2_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE2_POR                                                             0x00000000
#define HWIO_QFPROM_CORR_PTE2_POR_RMSK                                                        0x00000000
#define HWIO_QFPROM_CORR_PTE2_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, HWIO_QFPROM_CORR_PTE2_RMSK)
#define HWIO_QFPROM_CORR_PTE2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, m)
#define HWIO_QFPROM_CORR_PTE2_WAFER_ID_BMSK                                                   0xf8000000
#define HWIO_QFPROM_CORR_PTE2_WAFER_ID_SHFT                                                         0x1b
#define HWIO_QFPROM_CORR_PTE2_DIE_X_BMSK                                                       0x7f80000
#define HWIO_QFPROM_CORR_PTE2_DIE_X_SHFT                                                            0x13
#define HWIO_QFPROM_CORR_PTE2_DIE_Y_BMSK                                                         0x7f800
#define HWIO_QFPROM_CORR_PTE2_DIE_Y_SHFT                                                             0xb
#define HWIO_QFPROM_CORR_PTE2_FOUNDRY_ID_BMSK                                                      0x700
#define HWIO_QFPROM_CORR_PTE2_FOUNDRY_ID_SHFT                                                        0x8
#define HWIO_QFPROM_CORR_PTE2_LOGIC_RETENTION_BMSK                                                  0xe0
#define HWIO_QFPROM_CORR_PTE2_LOGIC_RETENTION_SHFT                                                   0x5
#define HWIO_QFPROM_CORR_PTE2_SPEED_BIN_BMSK                                                        0x1c
#define HWIO_QFPROM_CORR_PTE2_SPEED_BIN_SHFT                                                         0x2
#define HWIO_QFPROM_CORR_PTE2_MX_RET_BIN_BMSK                                                        0x3
#define HWIO_QFPROM_CORR_PTE2_MX_RET_BIN_SHFT                                                        0x0

#endif /* __CLOCKMSSHWIO_H__ */
