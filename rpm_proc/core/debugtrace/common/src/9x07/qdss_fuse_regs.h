#ifndef QDSS_FUSE_REG_H
#define QDSS_FUSE_REG_H

/*
===========================================================================
*/
/**
  @file qdss_fuse.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

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

  $Header: //components/rel/rpm.bf/2.1.1/core/debugtrace/common/src/9x07/qdss_fuse_regs.h#1 $
  $DateTime: 2015/10/06 06:01:22 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

//#define SECURITY_CONTROL_CORE_REG_BASE                                                                             (SECURITY_CONTROL_BASE      + 0x00000000)
//#define SECURITY_CONTROL_CORE_REG_BASE_PHYS                                                                        (SECURITY_CONTROL_BASE_PHYS + 0x00000000)
//#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                                        0x00000000

#define SECURITY_CONTROL_BASE_SIZE                                  0x00010000
#define SECURITY_CONTROL_BASE_PHYS                                  0x600A0000

#ifndef SECURITY_CONTROL_BASE 
   #define SECURITY_CONTROL_BASE                                       SECURITY_CONTROL_BASE_PHYS
#endif


#define HWIO_ACC_IR_ADDR                                                                   (SECURITY_CONTROL_BASE + 0x00002000)
#define HWIO_ACC_IR_PHYS                                                                   (SECURITY_CONTROL_BASE + 0x00002000)
#define HWIO_ACC_IR_OFFS                                                                      (0x00002000)
#define HWIO_ACC_IR_RMSK                                                                            0x1f
#define HWIO_ACC_IR_POR                                                                       0x00000000
#define HWIO_ACC_IR_ATTR                                                                             0x2
#define HWIO_ACC_IR_OUT(v)      \
        out_dword(HWIO_ACC_IR_ADDR,v)
#define HWIO_ACC_IR_INSTRUCTION_BMSK                                                                0x1f
#define HWIO_ACC_IR_INSTRUCTION_SHFT                                                                 0x0
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_IDCODE_FVAL                                                  0x1
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_REQVERID_FVAL                                                0x2
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_STARTACC_FVAL                                                0x3
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_STOPACC_FVAL                                                 0x4
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_LOCKOUT_FVAL                                                 0x5
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_INITFCT_FVAL                                                 0x6
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_FCT_FVAL                                                     0x7
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_TESTMEM_FVAL                                                 0x8
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_TESTROM_FVAL                                                 0x9
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_TESTNVM_FVAL                                                 0xa
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_TESTRNG_FVAL                                                 0xb
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_SCAN_FVAL                                                    0xc
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_SHARENVMWR_FVAL                                              0xd
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_SHARENVMRD_FVAL                                              0xe
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_EXITTEST_FVAL                                                0xf
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_REQRESP_FVAL                                                0x10
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_BYPASS_FVAL                                                 0x1f

#define HWIO_ACC_DR_ADDR                                                                   (SECURITY_CONTROL_BASE + 0x00002004)
#define HWIO_ACC_DR_PHYS                                                                   (SECURITY_CONTROL_BASE + 0x00002004)
#define HWIO_ACC_DR_OFFS                                                                      (0x00002004)
#define HWIO_ACC_DR_RMSK                                                                      0xffffffff
#define HWIO_ACC_DR_POR                                                                       0x00000000
#define HWIO_ACC_DR_ATTR                                                                             0x3
#define HWIO_ACC_DR_IN      \
        in_dword_masked(HWIO_ACC_DR_ADDR, HWIO_ACC_DR_RMSK)
#define HWIO_ACC_DR_INM(m)      \
        in_dword_masked(HWIO_ACC_DR_ADDR, m)
#define HWIO_ACC_DR_OUT(v)      \
        out_dword(HWIO_ACC_DR_ADDR,v)
#define HWIO_ACC_DR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ACC_DR_ADDR,m,v,HWIO_ACC_DR_IN)
#define HWIO_ACC_DR_DR_BMSK                                                                   0xffffffff
#define HWIO_ACC_DR_DR_SHFT                                                                          0x0

#define HWIO_ACC_VERID_ADDR                                                                (SECURITY_CONTROL_BASE + 0x00002008)
#define HWIO_ACC_VERID_PHYS                                                                (SECURITY_CONTROL_BASE + 0x00002008)
#define HWIO_ACC_VERID_OFFS                                                                   (0x00002008)
#define HWIO_ACC_VERID_RMSK                                                                       0xffff
#define HWIO_ACC_VERID_POR                                                                    0x00000202
#define HWIO_ACC_VERID_ATTR                                                                          0x1
#define HWIO_ACC_VERID_IN      \
        in_dword_masked(HWIO_ACC_VERID_ADDR, HWIO_ACC_VERID_RMSK)
#define HWIO_ACC_VERID_INM(m)      \
        in_dword_masked(HWIO_ACC_VERID_ADDR, m)
#define HWIO_ACC_VERID_FWVERID_BMSK                                                               0xff00
#define HWIO_ACC_VERID_FWVERID_SHFT                                                                  0x8
#define HWIO_ACC_VERID_HWVERID_BMSK                                                                 0xff
#define HWIO_ACC_VERID_HWVERID_SHFT                                                                  0x0

#define HWIO_ACC_FEATSETn_ADDR(base,n)                                                        ((base) + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_PHYS(base,n)                                                        ((base) + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_OFFS(base,n)                                                        (0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_RMSK                                                                0xffffffff
#define HWIO_ACC_FEATSETn_MAXn                                                                         7
#define HWIO_ACC_FEATSETn_POR                                                                 0x00000000
#define HWIO_ACC_FEATSETn_ATTR                                                                       0x1
#define HWIO_ACC_FEATSETn_INI(base,n)        \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(base,n), HWIO_ACC_FEATSETn_RMSK)
#define HWIO_ACC_FEATSETn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(base,n), mask)
#define HWIO_ACC_FEATSETn_FEAT_BMSK                                                           0xffffffff
#define HWIO_ACC_FEATSETn_FEAT_SHFT                                                                  0x0

#define HWIO_ACC_STATE_ADDR                                                                (SECURITY_CONTROL_BASE + 0x00002038)
#define HWIO_ACC_STATE_PHYS                                                                (SECURITY_CONTROL_BASE + 0x00002038)
#define HWIO_ACC_STATE_OFFS                                                                   (0x00002038)
#define HWIO_ACC_STATE_RMSK                                                                          0x7
#define HWIO_ACC_STATE_POR                                                                    0x00000000
#define HWIO_ACC_STATE_ATTR                                                                          0x1
#define HWIO_ACC_STATE_IN      \
        in_dword_masked(HWIO_ACC_STATE_ADDR, HWIO_ACC_STATE_RMSK)
#define HWIO_ACC_STATE_INM(m)      \
        in_dword_masked(HWIO_ACC_STATE_ADDR, m)
#define HWIO_ACC_STATE_ACC_READY_BMSK                                                                0x4
#define HWIO_ACC_STATE_ACC_READY_SHFT                                                                0x2
#define HWIO_ACC_STATE_ACC_READY_NOT_READY_FVAL                                                      0x0
#define HWIO_ACC_STATE_ACC_READY_READY_FVAL                                                          0x1
#define HWIO_ACC_STATE_ACC_LOCKED_BMSK                                                               0x2
#define HWIO_ACC_STATE_ACC_LOCKED_SHFT                                                               0x1
#define HWIO_ACC_STATE_ACC_LOCKED_UNLOCKED_FVAL                                                      0x0
#define HWIO_ACC_STATE_ACC_LOCKED_LOCKED_FVAL                                                        0x1
#define HWIO_ACC_STATE_ACC_STOP_BMSK                                                                 0x1
#define HWIO_ACC_STATE_ACC_STOP_SHFT                                                                 0x0
#define HWIO_ACC_STATE_ACC_STOP_ACTIVE_FVAL                                                          0x0
#define HWIO_ACC_STATE_ACC_STOP_STOPPED_FVAL                                                         0x1

#define HWIO_QFPROM_BLOW_TIMER_ADDR                                                        (SECURITY_CONTROL_BASE + 0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_PHYS                                                        (SECURITY_CONTROL_BASE + 0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_OFFS                                                           (0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_RMSK                                                                0xfff
#define HWIO_QFPROM_BLOW_TIMER_POR                                                            0x00000000
#define HWIO_QFPROM_BLOW_TIMER_ATTR                                                                  0x3
#define HWIO_QFPROM_BLOW_TIMER_IN      \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, HWIO_QFPROM_BLOW_TIMER_RMSK)
#define HWIO_QFPROM_BLOW_TIMER_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, m)
#define HWIO_QFPROM_BLOW_TIMER_OUT(v)      \
        out_dword(HWIO_QFPROM_BLOW_TIMER_ADDR,v)
#define HWIO_QFPROM_BLOW_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BLOW_TIMER_ADDR,m,v,HWIO_QFPROM_BLOW_TIMER_IN)
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_BMSK                                                     0xfff
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_SHFT                                                       0x0

#define HWIO_QFPROM_TEST_CTRL_ADDR                                                         (SECURITY_CONTROL_BASE + 0x00002040)
#define HWIO_QFPROM_TEST_CTRL_PHYS                                                         (SECURITY_CONTROL_BASE + 0x00002040)
#define HWIO_QFPROM_TEST_CTRL_OFFS                                                            (0x00002040)
#define HWIO_QFPROM_TEST_CTRL_RMSK                                                                   0xf
#define HWIO_QFPROM_TEST_CTRL_POR                                                             0x00000000
#define HWIO_QFPROM_TEST_CTRL_ATTR                                                                   0x3
#define HWIO_QFPROM_TEST_CTRL_IN      \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, HWIO_QFPROM_TEST_CTRL_RMSK)
#define HWIO_QFPROM_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, m)
#define HWIO_QFPROM_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_TEST_CTRL_ADDR,v)
#define HWIO_QFPROM_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_TEST_CTRL_ADDR,m,v,HWIO_QFPROM_TEST_CTRL_IN)
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_BMSK                                                       0x8
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_SHFT                                                       0x3
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_BMSK                                                        0x4
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_SHFT                                                        0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_BMSK                                                        0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_SHFT                                                        0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_BMSK                                                  0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_SHFT                                                  0x0

#define HWIO_QFPROM_ACCEL_ADDR                                                             (SECURITY_CONTROL_BASE + 0x00002044)
#define HWIO_QFPROM_ACCEL_PHYS                                                             (SECURITY_CONTROL_BASE + 0x00002044)
#define HWIO_QFPROM_ACCEL_OFFS                                                                (0x00002044)
#define HWIO_QFPROM_ACCEL_RMSK                                                                     0xfff
#define HWIO_QFPROM_ACCEL_POR                                                                 0x00000104
#define HWIO_QFPROM_ACCEL_ATTR                                                                       0x3
#define HWIO_QFPROM_ACCEL_IN      \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, HWIO_QFPROM_ACCEL_RMSK)
#define HWIO_QFPROM_ACCEL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, m)
#define HWIO_QFPROM_ACCEL_OUT(v)      \
        out_dword(HWIO_QFPROM_ACCEL_ADDR,v)
#define HWIO_QFPROM_ACCEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_ACCEL_ADDR,m,v,HWIO_QFPROM_ACCEL_IN)
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_BMSK                                                     0x800
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_SHFT                                                       0xb
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_BMSK                                                   0x700
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_SHFT                                                     0x8
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_BMSK                                                         0xff
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_SHFT                                                          0x0

#define HWIO_QFPROM_BLOW_STATUS_ADDR                                                       (SECURITY_CONTROL_BASE + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_PHYS                                                       (SECURITY_CONTROL_BASE + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_OFFS                                                          (0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_RMSK                                                                 0x3
#define HWIO_QFPROM_BLOW_STATUS_POR                                                           0x00000000
#define HWIO_QFPROM_BLOW_STATUS_ATTR                                                                 0x1
#define HWIO_QFPROM_BLOW_STATUS_IN      \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, HWIO_QFPROM_BLOW_STATUS_RMSK)
#define HWIO_QFPROM_BLOW_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, m)
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_BMSK                                                   0x2
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_SHFT                                                   0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_NO_ERROR_FVAL                                          0x0
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_ERROR_FVAL                                             0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_BMSK                                                     0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_SHFT                                                     0x0

#define HWIO_QFPROM_ROM_ERROR_ADDR                                                         (SECURITY_CONTROL_BASE + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_PHYS                                                         (SECURITY_CONTROL_BASE + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_OFFS                                                            (0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_RMSK                                                                   0x1
#define HWIO_QFPROM_ROM_ERROR_POR                                                             0x00000000
#define HWIO_QFPROM_ROM_ERROR_ATTR                                                                   0x1
#define HWIO_QFPROM_ROM_ERROR_IN      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, HWIO_QFPROM_ROM_ERROR_RMSK)
#define HWIO_QFPROM_ROM_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, m)
#define HWIO_QFPROM_ROM_ERROR_ERROR_BMSK                                                             0x1
#define HWIO_QFPROM_ROM_ERROR_ERROR_SHFT                                                             0x0
#define HWIO_QFPROM_ROM_ERROR_ERROR_NO_ERROR_FVAL                                                    0x0
#define HWIO_QFPROM_ROM_ERROR_ERROR_ERROR_FVAL                                                       0x1

#define HWIO_QFPROM_BIST_CTRL_ADDR                                                         (SECURITY_CONTROL_BASE + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_PHYS                                                         (SECURITY_CONTROL_BASE + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_OFFS                                                            (0x00002050)
#define HWIO_QFPROM_BIST_CTRL_RMSK                                                                  0x7f
#define HWIO_QFPROM_BIST_CTRL_POR                                                             0x00000000
#define HWIO_QFPROM_BIST_CTRL_ATTR                                                                   0x3
#define HWIO_QFPROM_BIST_CTRL_IN      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, HWIO_QFPROM_BIST_CTRL_RMSK)
#define HWIO_QFPROM_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, m)
#define HWIO_QFPROM_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_BIST_CTRL_ADDR,v)
#define HWIO_QFPROM_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BIST_CTRL_ADDR,m,v,HWIO_QFPROM_BIST_CTRL_IN)
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_BMSK                                                      0x7c
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_SHFT                                                       0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_BMSK                                                        0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_SHFT                                                        0x1
#define HWIO_QFPROM_BIST_CTRL_START_BMSK                                                             0x1
#define HWIO_QFPROM_BIST_CTRL_START_SHFT                                                             0x0

#define HWIO_QFPROM_BIST_ERROR_ADDR                                                        (SECURITY_CONTROL_BASE + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_PHYS                                                        (SECURITY_CONTROL_BASE + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_OFFS                                                           (0x00002054)
#define HWIO_QFPROM_BIST_ERROR_RMSK                                                           0xffffffff
#define HWIO_QFPROM_BIST_ERROR_POR                                                            0x00000000
#define HWIO_QFPROM_BIST_ERROR_ATTR                                                                  0x1
#define HWIO_QFPROM_BIST_ERROR_IN      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, HWIO_QFPROM_BIST_ERROR_RMSK)
#define HWIO_QFPROM_BIST_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR_ERROR_BMSK                                                     0xffffffff
#define HWIO_QFPROM_BIST_ERROR_ERROR_SHFT                                                            0x0
#define HWIO_QFPROM_BIST_ERROR_ERROR_NO_ERROR_FVAL                                                   0x0
#define HWIO_QFPROM_BIST_ERROR_ERROR_ERROR_FVAL                                                      0x1

#define HWIO_QFPROM_HASH_SIGNATUREn_ADDR(base,n)                                              ((base) + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_PHYS(base,n)                                              ((base) + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_OFFS(base,n)                                              (0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_RMSK                                                      0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_MAXn                                                               7
#define HWIO_QFPROM_HASH_SIGNATUREn_POR                                                       0x00000000
#define HWIO_QFPROM_HASH_SIGNATUREn_ATTR                                                             0x1
#define HWIO_QFPROM_HASH_SIGNATUREn_INI(base,n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(base,n), HWIO_QFPROM_HASH_SIGNATUREn_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(base,n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_BMSK                                           0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_SHFT                                                  0x0

#define HWIO_HW_KEY_STATUS_ADDR                                                            (SECURITY_CONTROL_BASE + 0x00002080)
#define HWIO_HW_KEY_STATUS_PHYS                                                            (SECURITY_CONTROL_BASE + 0x00002080)
#define HWIO_HW_KEY_STATUS_OFFS                                                               (0x00002080)
#define HWIO_HW_KEY_STATUS_RMSK                                                                     0x1f
#define HWIO_HW_KEY_STATUS_POR                                                                0x00000000
#define HWIO_HW_KEY_STATUS_ATTR                                                                      0x1
#define HWIO_HW_KEY_STATUS_IN      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, HWIO_HW_KEY_STATUS_RMSK)
#define HWIO_HW_KEY_STATUS_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, m)
#define HWIO_HW_KEY_STATUS_KDF_DONE_BMSK                                                            0x10
#define HWIO_HW_KEY_STATUS_KDF_DONE_SHFT                                                             0x4
#define HWIO_HW_KEY_STATUS_KDF_DONE_KDF_OPERATION_IS_PROGRESS_FVAL                                   0x0
#define HWIO_HW_KEY_STATUS_KDF_DONE_KDF_OPERATION_HAS_COMPLETED_FVAL                                 0x1
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BMSK                                                     0x8
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_SHFT                                                     0x3
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_NOT_BLOCKED_FVAL                                         0x0
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BLOCKED_FVAL                                             0x1
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BMSK                                                    0x4
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_SHFT                                                    0x2
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_NOT_BLOCKED_FVAL                                        0x0
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BLOCKED_FVAL                                            0x1
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BMSK                                         0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_SHFT                                         0x1
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_NOT_BLOWN_FVAL                               0x0
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BLOWN_FVAL                                   0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BMSK                                         0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_SHFT                                         0x0
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_NOT_BLOWN_FVAL                               0x0
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BLOWN_FVAL                                   0x1

#define HWIO_RESET_JDR_STATUS_ADDR                                                         (SECURITY_CONTROL_BASE + 0x00002084)
#define HWIO_RESET_JDR_STATUS_PHYS                                                         (SECURITY_CONTROL_BASE + 0x00002084)
#define HWIO_RESET_JDR_STATUS_OFFS                                                            (0x00002084)
#define HWIO_RESET_JDR_STATUS_RMSK                                                                   0x3
#define HWIO_RESET_JDR_STATUS_POR                                                             0x00000000
#define HWIO_RESET_JDR_STATUS_ATTR                                                                   0x1
#define HWIO_RESET_JDR_STATUS_IN      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, HWIO_RESET_JDR_STATUS_RMSK)
#define HWIO_RESET_JDR_STATUS_INM(m)      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, m)
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_BMSK                                                       0x2
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_SHFT                                                       0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_BMSK                                              0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_SHFT                                              0x0

#define HWIO_FEC_ESR_ADDR                                                                  (SECURITY_CONTROL_BASE + 0x00002090)
#define HWIO_FEC_ESR_PHYS                                                                  (SECURITY_CONTROL_BASE + 0x00002090)
#define HWIO_FEC_ESR_OFFS                                                                     (0x00002090)
#define HWIO_FEC_ESR_RMSK                                                                          0xfff
#define HWIO_FEC_ESR_POR                                                                      0x00000000
#define HWIO_FEC_ESR_ATTR                                                                            0x3
#define HWIO_FEC_ESR_IN      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, HWIO_FEC_ESR_RMSK)
#define HWIO_FEC_ESR_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, m)
#define HWIO_FEC_ESR_OUT(v)      \
        out_dword(HWIO_FEC_ESR_ADDR,v)
#define HWIO_FEC_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_ADDR,m,v,HWIO_FEC_ESR_IN)
#define HWIO_FEC_ESR_CORR_SW_ACC_BMSK                                                              0x800
#define HWIO_FEC_ESR_CORR_SW_ACC_SHFT                                                                0xb
#define HWIO_FEC_ESR_CORR_BOOT_ROM_BMSK                                                            0x200
#define HWIO_FEC_ESR_CORR_BOOT_ROM_SHFT                                                              0x9
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_BMSK                                                          0x100
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_SHFT                                                            0x8
#define HWIO_FEC_ESR_CORR_MULT_BMSK                                                                 0x80
#define HWIO_FEC_ESR_CORR_MULT_SHFT                                                                  0x7
#define HWIO_FEC_ESR_CORR_SEEN_BMSK                                                                 0x40
#define HWIO_FEC_ESR_CORR_SEEN_SHFT                                                                  0x6
#define HWIO_FEC_ESR_CORR_SEEN_NO_CORRECTION_FVAL                                                    0x0
#define HWIO_FEC_ESR_CORR_SEEN_CORRECTION_FVAL                                                       0x1
#define HWIO_FEC_ESR_ERR_SW_ACC_BMSK                                                                0x20
#define HWIO_FEC_ESR_ERR_SW_ACC_SHFT                                                                 0x5
#define HWIO_FEC_ESR_ERR_BOOT_ROM_BMSK                                                               0x8
#define HWIO_FEC_ESR_ERR_BOOT_ROM_SHFT                                                               0x3
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_BMSK                                                             0x4
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_SHFT                                                             0x2
#define HWIO_FEC_ESR_ERR_MULT_BMSK                                                                   0x2
#define HWIO_FEC_ESR_ERR_MULT_SHFT                                                                   0x1
#define HWIO_FEC_ESR_ERR_SEEN_BMSK                                                                   0x1
#define HWIO_FEC_ESR_ERR_SEEN_SHFT                                                                   0x0
#define HWIO_FEC_ESR_ERR_SEEN_NO_ERROR_FVAL                                                          0x0
#define HWIO_FEC_ESR_ERR_SEEN_ERROR_FVAL                                                             0x1

#define HWIO_FEC_EAR_ADDR                                                                  (SECURITY_CONTROL_BASE + 0x00002094)
#define HWIO_FEC_EAR_PHYS                                                                  (SECURITY_CONTROL_BASE + 0x00002094)
#define HWIO_FEC_EAR_OFFS                                                                     (0x00002094)
#define HWIO_FEC_EAR_RMSK                                                                     0xffffffff
#define HWIO_FEC_EAR_POR                                                                      0x00000000
#define HWIO_FEC_EAR_ATTR                                                                            0x1
#define HWIO_FEC_EAR_IN      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, HWIO_FEC_EAR_RMSK)
#define HWIO_FEC_EAR_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, m)
#define HWIO_FEC_EAR_CORR_ADDR_BMSK                                                           0xffff0000
#define HWIO_FEC_EAR_CORR_ADDR_SHFT                                                                 0x10
#define HWIO_FEC_EAR_ERR_ADDR_BMSK                                                                0xffff
#define HWIO_FEC_EAR_ERR_ADDR_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_JTAG_ID_ADDR                                                      (SECURITY_CONTROL_BASE + 0x00004000)
#define HWIO_QFPROM_CORR_JTAG_ID_PHYS                                                      (SECURITY_CONTROL_BASE + 0x00004000)
#define HWIO_QFPROM_CORR_JTAG_ID_OFFS                                                         (0x00004000)
#define HWIO_QFPROM_CORR_JTAG_ID_RMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_POR                                                          0x00000000
#define HWIO_QFPROM_CORR_JTAG_ID_ATTR                                                                0x1
#define HWIO_QFPROM_CORR_JTAG_ID_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, HWIO_QFPROM_CORR_JTAG_ID_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_PTE_DATA0_BMSK                                               0xf0000000
#define HWIO_QFPROM_CORR_JTAG_ID_PTE_DATA0_SHFT                                                     0x1c
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_BMSK                                               0xff00000
#define HWIO_QFPROM_CORR_JTAG_ID_FEATURE_ID_SHFT                                                    0x14
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_BMSK                                                    0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_JTAG_ID_SHFT                                                        0x0

#define HWIO_QFPROM_CORR_PTE1_ADDR                                                         (SECURITY_CONTROL_BASE + 0x00004004)
#define HWIO_QFPROM_CORR_PTE1_PHYS                                                         (SECURITY_CONTROL_BASE + 0x00004004)
#define HWIO_QFPROM_CORR_PTE1_OFFS                                                            (0x00004004)
#define HWIO_QFPROM_CORR_PTE1_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE1_POR                                                             0x00000000
#define HWIO_QFPROM_CORR_PTE1_ATTR                                                                   0x1
#define HWIO_QFPROM_CORR_PTE1_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, HWIO_QFPROM_CORR_PTE1_RMSK)
#define HWIO_QFPROM_CORR_PTE1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE1_ADDR, m)
#define HWIO_QFPROM_CORR_PTE1_PTE_DATA1_BMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_PTE1_PTE_DATA1_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_ADDR                                                   (SECURITY_CONTROL_BASE + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_PHYS                                                   (SECURITY_CONTROL_BASE + 0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_OFFS                                                      (0x00004008)
#define HWIO_QFPROM_CORR_SERIAL_NUM_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_POR                                                       0x00000000
#define HWIO_QFPROM_CORR_SERIAL_NUM_ATTR                                                             0x1
#define HWIO_QFPROM_CORR_SERIAL_NUM_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_SERIAL_NUM_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_PTE2_ADDR                                                         (SECURITY_CONTROL_BASE + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_PHYS                                                         (SECURITY_CONTROL_BASE + 0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_OFFS                                                            (0x0000400c)
#define HWIO_QFPROM_CORR_PTE2_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE2_POR                                                             0x00000000
#define HWIO_QFPROM_CORR_PTE2_ATTR                                                                   0x1
#define HWIO_QFPROM_CORR_PTE2_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, HWIO_QFPROM_CORR_PTE2_RMSK)
#define HWIO_QFPROM_CORR_PTE2_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE2_ADDR, m)
#define HWIO_QFPROM_CORR_PTE2_PTE_DATA2_BMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_PTE2_PTE_DATA2_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR                                               (SECURITY_CONTROL_BASE + 0x00004010)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PHYS                                               (SECURITY_CONTROL_BASE + 0x00004010)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OFFS                                                  (0x00004010)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_POR                                                   0x00000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ATTR                                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE_BMSK                                            0xfff00000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE_SHFT                                                  0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_BMSK                                             0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_SHFT                                                0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_BMSK                                             0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_SHFT                                                0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE18_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_BMSK                                             0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_SHFT                                                0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE17_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_BMSK                                             0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_SHFT                                                0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE16_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE15_BMSK                                              0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE15_SHFT                                                 0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE15_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE15_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                       0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                          0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_ALLOW_READ_FVAL                               0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_DISABLE_READ_FVAL                             0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_BMSK                                           0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_SHFT                                              0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_ALLOW_READ_FVAL                                   0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_CONFIG_DISABLE_READ_FVAL                                 0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_BMSK                                                0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_ALLOW_READ_FVAL                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_DISABLE_READ_FVAL                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_BMSK                                               0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PK_HASH_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                           0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                             0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_ALLOW_READ_FVAL                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_DISABLE_READ_FVAL                                0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                          0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                            0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_ALLOW_READ_FVAL                                 0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_DISABLE_READ_FVAL                               0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                  0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                 0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                  0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                            0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                             0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_ALLOW_READ_FVAL                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_DISABLE_READ_FVAL                                0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                             0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                              0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_ALLOW_READ_FVAL                                   0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_DISABLE_READ_FVAL                                 0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                        0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_ALLOW_READ_FVAL                              0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_3_DISABLE_READ_FVAL                            0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                         0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                         0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_ALLOW_READ_FVAL                              0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_2_DISABLE_READ_FVAL                            0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                         0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_ALLOW_READ_FVAL                              0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ANTI_ROLLBACK_1_DISABLE_READ_FVAL                            0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                              0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                              0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_ALLOW_READ_FVAL                                   0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_DISABLE_READ_FVAL                                 0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_BMSK                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_SHFT                                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_ALLOW_READ_FVAL                                          0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_DISABLE_READ_FVAL                                        0x1

#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR                                               (SECURITY_CONTROL_BASE + 0x00004014)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PHYS                                               (SECURITY_CONTROL_BASE + 0x00004014)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OFFS                                                  (0x00004014)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_POR                                                   0x00000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ATTR                                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE_BMSK                                            0xfff00000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE_SHFT                                                  0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_BMSK                                             0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_SHFT                                                0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_BMSK                                             0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_SHFT                                                0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE18_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_BMSK                                             0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_SHFT                                                0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE17_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_BMSK                                             0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_SHFT                                                0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE16_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE15_BMSK                                              0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE15_SHFT                                                 0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE15_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE15_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                       0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                          0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_ALLOW_WRITE_FVAL                              0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_DISABLE_WRITE_FVAL                            0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_BMSK                                           0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_SHFT                                              0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_ALLOW_WRITE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_CONFIG_DISABLE_WRITE_FVAL                                0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_BMSK                                                0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_SHFT                                                   0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_ALLOW_WRITE_FVAL                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_DISABLE_WRITE_FVAL                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_BMSK                                               0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_SHFT                                                 0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PK_HASH_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                           0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                             0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_ALLOW_WRITE_FVAL                                 0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_DISABLE_WRITE_FVAL                               0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                          0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                            0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_ALLOW_WRITE_FVAL                                0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_DISABLE_WRITE_FVAL                              0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                  0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                 0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                  0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                            0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                             0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_ALLOW_WRITE_FVAL                                 0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_DISABLE_WRITE_FVAL                               0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                             0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                              0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_ALLOW_WRITE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_DISABLE_WRITE_FVAL                                0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_BMSK                                        0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_SHFT                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_ALLOW_WRITE_FVAL                             0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_3_DISABLE_WRITE_FVAL                           0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_BMSK                                         0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_SHFT                                         0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_ALLOW_WRITE_FVAL                             0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_2_DISABLE_WRITE_FVAL                           0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_BMSK                                         0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_SHFT                                         0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_ALLOW_WRITE_FVAL                             0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ANTI_ROLLBACK_1_DISABLE_WRITE_FVAL                           0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                              0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                              0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_ALLOW_WRITE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_DISABLE_WRITE_FVAL                                0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_BMSK                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_SHFT                                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_ALLOW_WRITE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_DISABLE_WRITE_FVAL                                       0x1

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR                                          (SECURITY_CONTROL_BASE + 0x00004018)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_PHYS                                          (SECURITY_CONTROL_BASE + 0x00004018)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_OFFS                                             (0x00004018)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_POR                                              0x00000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ATTR                                                    0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_BMSK                              0xfc000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_PIL_SUBSYSTEM0_SHFT                                    0x1a
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_TZ_BMSK                                           0x3fff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_TZ_SHFT                                                 0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_SBL1_BMSK                                             0xffe
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_SBL1_SHFT                                               0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_BMSK                               0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_SHFT                               0x0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_RPMB_KEY_NOT_PROVISIONED_FVAL        0x0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RPMB_KEY_PROVISIONED_RPMB_KEY_PROVISIONED_FVAL          0x1

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR                                          (SECURITY_CONTROL_BASE + 0x0000401c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PHYS                                          (SECURITY_CONTROL_BASE + 0x0000401c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_OFFS                                             (0x0000401c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_POR                                              0x00000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ATTR                                                    0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_BMSK                                     0xfffc0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_APPSBL0_SHFT                                           0x12
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_BMSK                                 0x3ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_PIL_SUBSYSTEM1_SHFT                                     0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR                                          (SECURITY_CONTROL_BASE + 0x00004020)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_PHYS                                          (SECURITY_CONTROL_BASE + 0x00004020)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_OFFS                                             (0x00004020)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_POR                                              0x00000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ATTR                                                    0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_APPSBL1_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR                                          (SECURITY_CONTROL_BASE + 0x00004024)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_PHYS                                          (SECURITY_CONTROL_BASE + 0x00004024)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_OFFS                                             (0x00004024)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_POR                                              0x00000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ATTR                                                    0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_BMSK                     0xff000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_SHFT                           0x18
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED_FVAL        0x0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_15_FVAL        0xf
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_14_FVAL       0x1e
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_13_FVAL       0x2d
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_12_FVAL       0x3c
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_11_FVAL       0x4b
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_10_FVAL       0x5a
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_9_FVAL       0x69
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_8_FVAL       0x78
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_7_FVAL       0x87
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_6_FVAL       0x96
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_5_FVAL       0xa5
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_4_FVAL       0xb4
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_3_FVAL       0xc3
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_2_FVAL       0xd2
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_1_FVAL       0xe1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ROOT_CERT_PK_HASH_INDEX_PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0_FVAL       0xf0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_HYPERVISOR_BMSK                                    0xfff000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_HYPERVISOR_SHFT                                         0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                              0xff0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                0x4
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_APPSBL2_BMSK                                            0xf
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_APPSBL2_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR                                          (SECURITY_CONTROL_BASE + 0x00004028)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_PHYS                                          (SECURITY_CONTROL_BASE + 0x00004028)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_OFFS                                             (0x00004028)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_POR                                              0x00000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ATTR                                                    0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_BMSK                                         0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MSS_SHFT                                               0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_BMSK                                             0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_MBA_SHFT                                                0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR                                          (SECURITY_CONTROL_BASE + 0x0000402c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_PHYS                                          (SECURITY_CONTROL_BASE + 0x0000402c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_OFFS                                             (0x0000402c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_POR                                              0x00000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ATTR                                                    0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_IN      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_BMSK                                      0xffffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_SPARE0_SHFT                                             0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_BMSK                     0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_SHFT                      0x0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED_FVAL        0x0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_15_FVAL        0xf
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_14_FVAL       0x1e
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_13_FVAL       0x2d
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_12_FVAL       0x3c
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_11_FVAL       0x4b
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_10_FVAL       0x5a
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_9_FVAL       0x69
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_8_FVAL       0x78
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_7_FVAL       0x87
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_6_FVAL       0x96
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_5_FVAL       0xa5
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_4_FVAL       0xb4
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_3_FVAL       0xc3
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_2_FVAL       0xd2
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_DEVICE_FIXED_TO_CERTIFICATE_1_FVAL       0xe1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_MODEM_ROOT_CERT_PK_HASH_INDEX_PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0_FVAL       0xf0


#define HWIO_SEC_CTRL_HW_VERSION_ADDR                                                      (SECURITY_CONTROL_BASE + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_PHYS                                                      (SECURITY_CONTROL_BASE + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_OFFS                                                         (0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_RMSK                                                         0xffffffff
#define HWIO_SEC_CTRL_HW_VERSION_POR                                                          0x40000000
#define HWIO_SEC_CTRL_HW_VERSION_ATTR                                                                0x1
#define HWIO_SEC_CTRL_HW_VERSION_IN      \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, HWIO_SEC_CTRL_HW_VERSION_RMSK)
#define HWIO_SEC_CTRL_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, m)
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_BMSK                                                   0xf0000000
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_SHFT                                                         0x1c
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_BMSK                                                    0xfff0000
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_SHFT                                                         0x10
#define HWIO_SEC_CTRL_HW_VERSION_STEP_BMSK                                                        0xffff
#define HWIO_SEC_CTRL_HW_VERSION_STEP_SHFT                                                           0x0

#define HWIO_FEATURE_CONFIG0_ADDR                                                          (SECURITY_CONTROL_BASE + 0x00006004)
#define HWIO_FEATURE_CONFIG0_PHYS                                                          (SECURITY_CONTROL_BASE + 0x00006004)
#define HWIO_FEATURE_CONFIG0_OFFS                                                             (0x00006004)
#define HWIO_FEATURE_CONFIG0_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG0_POR                                                              0x00000000
#define HWIO_FEATURE_CONFIG0_ATTR                                                                    0x1
#define HWIO_FEATURE_CONFIG0_IN      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, HWIO_FEATURE_CONFIG0_RMSK)
#define HWIO_FEATURE_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, m)
#define HWIO_FEATURE_CONFIG0_SPARE_31_29_BMSK                                                 0xe0000000
#define HWIO_FEATURE_CONFIG0_SPARE_31_29_SHFT                                                       0x1d
#define HWIO_FEATURE_CONFIG0_MDP_EFUSE_LTC0_DISABLE_BMSK                                      0x10000000
#define HWIO_FEATURE_CONFIG0_MDP_EFUSE_LTC0_DISABLE_SHFT                                            0x1c
#define HWIO_FEATURE_CONFIG0_NIDNT_DISABLE_BMSK                                                0x8000000
#define HWIO_FEATURE_CONFIG0_NIDNT_DISABLE_SHFT                                                     0x1b
#define HWIO_FEATURE_CONFIG0_SPARE_26_BMSK                                                     0x4000000
#define HWIO_FEATURE_CONFIG0_SPARE_26_SHFT                                                          0x1a
#define HWIO_FEATURE_CONFIG0_ACC_DISABLE_BMSK                                                  0x2000000
#define HWIO_FEATURE_CONFIG0_ACC_DISABLE_SHFT                                                       0x19
#define HWIO_FEATURE_CONFIG0_ACC_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_FEATURE_CONFIG0_ACC_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_BMSK                                          0x1000000
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_SHFT                                               0x18
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_BMSK                                           0x800000
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_SHFT                                               0x17
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_BMSK                                              0x400000
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_SHFT                                                  0x16
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_BMSK                                                   0x200000
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_SHFT                                                       0x15
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_BMSK                                                   0x100000
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_SHFT                                                       0x14
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_BMSK                                            0xe0000
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_SHFT                                               0x11
#define HWIO_FEATURE_CONFIG0_MODEM_UIM2_DISABLE_BMSK                                             0x10000
#define HWIO_FEATURE_CONFIG0_MODEM_UIM2_DISABLE_SHFT                                                0x10
#define HWIO_FEATURE_CONFIG0_MODEM_UIM2_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_FEATURE_CONFIG0_MODEM_UIM2_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_FEATURE_CONFIG0_MODEM_UIM1_DISABLE_BMSK                                              0x8000
#define HWIO_FEATURE_CONFIG0_MODEM_UIM1_DISABLE_SHFT                                                 0xf
#define HWIO_FEATURE_CONFIG0_MODEM_UIM1_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_FEATURE_CONFIG0_MODEM_UIM1_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_FEATURE_CONFIG0_MODEM_SMMU_DISABLE_BMSK                                              0x4000
#define HWIO_FEATURE_CONFIG0_MODEM_SMMU_DISABLE_SHFT                                                 0xe
#define HWIO_FEATURE_CONFIG0_MODEM_SMMU_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_FEATURE_CONFIG0_MODEM_SMMU_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_FEATURE_CONFIG0_MODEM_DSDA_DISABLE_BMSK                                              0x2000
#define HWIO_FEATURE_CONFIG0_MODEM_DSDA_DISABLE_SHFT                                                 0xd
#define HWIO_FEATURE_CONFIG0_MODEM_DSDA_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_FEATURE_CONFIG0_MODEM_DSDA_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_FEATURE_CONFIG0_SPARE_12_BMSK                                                        0x1000
#define HWIO_FEATURE_CONFIG0_SPARE_12_SHFT                                                           0xc
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_BMSK                                              0x800
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_SHFT                                                0xb
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_BMSK                                             0x400
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_SHFT                                               0xa
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_BMSK                                            0x200
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_SHFT                                              0x9
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_BMSK                                          0x100
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_SHFT                                            0x8
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                                      0x80
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                                       0x7
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                                      0x40
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                                       0x6
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_BMSK                                             0x20
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_SHFT                                              0x5
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_BMSK                                                0x10
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_SHFT                                                 0x4
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_BMSK                                                  0x8
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_SHFT                                                  0x3
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_BMSK                                                0x4
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_SHFT                                                0x2
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_BMSK                                                   0x2
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_SHFT                                                   0x1
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_BMSK                                                   0x1
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_SHFT                                                   0x0
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_DISABLE_FVAL                                           0x1

#define HWIO_FEATURE_CONFIG1_ADDR                                                          (SECURITY_CONTROL_BASE + 0x00006008)
#define HWIO_FEATURE_CONFIG1_PHYS                                                          (SECURITY_CONTROL_BASE + 0x00006008)
#define HWIO_FEATURE_CONFIG1_OFFS                                                             (0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG1_POR                                                              0x00000000
#define HWIO_FEATURE_CONFIG1_ATTR                                                                    0x1
#define HWIO_FEATURE_CONFIG1_IN      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                 0x80000000
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                       0x1f
#define HWIO_FEATURE_CONFIG1_SPARE0_BMSK                                                      0x7fff8000
#define HWIO_FEATURE_CONFIG1_SPARE0_SHFT                                                             0xf
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION3_BMSK                                             0x6000
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION3_SHFT                                                0xd
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION2_BMSK                                             0x1800
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION2_SHFT                                                0xb
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION1_BMSK                                              0x600
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION1_SHFT                                                0x9
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION0_BMSK                                              0x180
#define HWIO_FEATURE_CONFIG1_APPS_CONFIGURATION0_SHFT                                                0x7
#define HWIO_FEATURE_CONFIG1_SPARE_BMSK                                                             0x40
#define HWIO_FEATURE_CONFIG1_SPARE_SHFT                                                              0x6
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_BMSK                                                 0x20
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_SHFT                                                  0x5
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_DEFAULT_POWER_ON_BRINGUP_DISABLE_BOOT_FSM_FVAL        0x0
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_BOOT_FSM_BASED_BRING_UP_FVAL                          0x1
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_BMSK                                               0x18
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_SHFT                                                0x3
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_GDHS_CNT_128_CLAMP_RESET_128_L2_RESET_256_FVAL        0x0
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_GDHS_CNT_32_CLAMP_RESET_48_L2_RESET_128_FVAL        0x2
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_GDHS_CNT_16_CLAMP_RESET_23_L2_RESET_103_FVAL        0x3
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_BMSK                                                      0x6
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_SHFT                                                      0x1
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_ENUM_1_5GHZ_FVAL                                          0x0
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_ENUM_1_4GHZ_FVAL                                          0x2
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_ENUM_1_2GHZ_FVAL                                          0x3
#define HWIO_FEATURE_CONFIG1_APPS_L2_CACHE_UPPER_BANK_DISABLE_BMSK                                   0x1
#define HWIO_FEATURE_CONFIG1_APPS_L2_CACHE_UPPER_BANK_DISABLE_SHFT                                   0x0
#define HWIO_FEATURE_CONFIG1_APPS_L2_CACHE_UPPER_BANK_DISABLE_ENUM_512KB_FVAL                        0x0
#define HWIO_FEATURE_CONFIG1_APPS_L2_CACHE_UPPER_BANK_DISABLE_ENUM_1MB_FVAL                          0x1

#define HWIO_FEATURE_CONFIG2_ADDR                                                          (SECURITY_CONTROL_BASE + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_PHYS                                                          (SECURITY_CONTROL_BASE + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_OFFS                                                             (0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG2_POR                                                              0x00000000
#define HWIO_FEATURE_CONFIG2_ATTR                                                                    0x1
#define HWIO_FEATURE_CONFIG2_IN      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_SPARE1_BMSK                                                      0xffe00000
#define HWIO_FEATURE_CONFIG2_SPARE1_SHFT                                                            0x15
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_BMSK                                       0x100000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_SHFT                                           0x14
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_BMSK                                         0x80000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_SHFT                                            0x13
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                     0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_BMSK                                          0x40000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_SHFT                                             0x12
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_BMSK                                           0x20000
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_SHFT                                              0x11
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_BMSK                                           0x10000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_SHFT                                              0x10
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_BMSK                                         0x8000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_SHFT                                            0xf
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_BMSK                                          0x4000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_SHFT                                             0xe
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                     0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_BMSK                                           0x2000
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_SHFT                                              0xd
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_BMSK                                           0x1000
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_SHFT                                              0xc
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_BMSK                                                0x800
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_SHFT                                                  0xb
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_BMSK                                                0x400
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_SHFT                                                  0xa
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_BMSK                                         0x200
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_SHFT                                           0x9
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                    0x0
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                   0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_BMSK                                             0x100
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_SHFT                                               0x8
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_BMSK                                         0x80
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_SHFT                                          0x7
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_BMSK                                              0x40
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_SHFT                                               0x6
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_BMSK                                             0x20
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_SHFT                                              0x5
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_BMSK                                              0x10
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_SHFT                                               0x4
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_BMSK                                               0x8
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_SHFT                                               0x3
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_BMSK                                                   0x4
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_SHFT                                                   0x2
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_BMSK                                                    0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_SHFT                                                    0x1
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_BMSK                                                        0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_SHFT                                                        0x0
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_DISABLE_FVAL                                                0x1

#define HWIO_FEATURE_CONFIG3_ADDR                                                          (SECURITY_CONTROL_BASE + 0x00006010)
#define HWIO_FEATURE_CONFIG3_PHYS                                                          (SECURITY_CONTROL_BASE + 0x00006010)
#define HWIO_FEATURE_CONFIG3_OFFS                                                             (0x00006010)
#define HWIO_FEATURE_CONFIG3_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG3_POR                                                              0x00000000
#define HWIO_FEATURE_CONFIG3_ATTR                                                                    0x1
#define HWIO_FEATURE_CONFIG3_IN      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, HWIO_FEATURE_CONFIG3_RMSK)
#define HWIO_FEATURE_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, m)
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_BMSK                                      0xf8000000
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_SHFT                                            0x1b
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_BMSK                                     0x4000000
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_SHFT                                          0x1a
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_BMSK                                            0x3ffc000
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_SHFT                                                  0xe
#define HWIO_FEATURE_CONFIG3_SPARE1_BMSK                                                          0x3c00
#define HWIO_FEATURE_CONFIG3_SPARE1_SHFT                                                             0xa
#define HWIO_FEATURE_CONFIG3_MODEM_PBL_PATCH_VERSION_BMSK                                          0x3e0
#define HWIO_FEATURE_CONFIG3_MODEM_PBL_PATCH_VERSION_SHFT                                            0x5
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_BMSK                                            0x1f
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_SHFT                                             0x0

#define HWIO_FEATURE_CONFIG4_ADDR                                                          (SECURITY_CONTROL_BASE + 0x00006014)
#define HWIO_FEATURE_CONFIG4_PHYS                                                          (SECURITY_CONTROL_BASE + 0x00006014)
#define HWIO_FEATURE_CONFIG4_OFFS                                                             (0x00006014)
#define HWIO_FEATURE_CONFIG4_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG4_POR                                                              0x00000000
#define HWIO_FEATURE_CONFIG4_ATTR                                                                    0x1
#define HWIO_FEATURE_CONFIG4_IN      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, HWIO_FEATURE_CONFIG4_RMSK)
#define HWIO_FEATURE_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, m)
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK                            0xffffffff
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                                   0x0

#define HWIO_FEATURE_CONFIG5_ADDR                                                          (SECURITY_CONTROL_BASE + 0x00006018)
#define HWIO_FEATURE_CONFIG5_PHYS                                                          (SECURITY_CONTROL_BASE + 0x00006018)
#define HWIO_FEATURE_CONFIG5_OFFS                                                             (0x00006018)
#define HWIO_FEATURE_CONFIG5_RMSK                                                             0xffffffff
#define HWIO_FEATURE_CONFIG5_POR                                                              0x00000000
#define HWIO_FEATURE_CONFIG5_ATTR                                                                    0x1
#define HWIO_FEATURE_CONFIG5_IN      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, HWIO_FEATURE_CONFIG5_RMSK)
#define HWIO_FEATURE_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, m)
#define HWIO_FEATURE_CONFIG5_SPARE3_BMSK                                                      0xf8000000
#define HWIO_FEATURE_CONFIG5_SPARE3_SHFT                                                            0x1b
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_BMSK                                           0x7800000
#define HWIO_FEATURE_CONFIG5_MODEM_PBL_PLL_CTRL_SHFT                                                0x17
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_BMSK                                             0x780000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_SHFT                                                 0x13
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_BMSK                                           0x78000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_SHFT                                               0xf
#define HWIO_FEATURE_CONFIG5_APPS_AARCH64_ENABLE_BMSK                                             0x4000
#define HWIO_FEATURE_CONFIG5_APPS_AARCH64_ENABLE_SHFT                                                0xe
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_BMSK                                             0x3000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_SHFT                                                0xc
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_XO_FVAL                                             0x0
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_ENUM_384_MHZ_FVAL                                   0x1
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_ENUM_614_4_MHZ_FVAL                                 0x2
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_ENUM_998_4_MHZ_FVAL                                 0x3
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_ROM_BMSK                                               0x800
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_ROM_SHFT                                                 0xb
#define HWIO_FEATURE_CONFIG5_MSA_ENA_BMSK                                                          0x400
#define HWIO_FEATURE_CONFIG5_MSA_ENA_SHFT                                                            0xa
#define HWIO_FEATURE_CONFIG5_FORCE_MSA_AUTH_EN_BMSK                                                0x200
#define HWIO_FEATURE_CONFIG5_FORCE_MSA_AUTH_EN_SHFT                                                  0x9
#define HWIO_FEATURE_CONFIG5_FORCE_MSA_AUTH_EN_MODEM_IMAGE_NOT_AUTHENTICATED_FVAL                    0x0
#define HWIO_FEATURE_CONFIG5_FORCE_MSA_AUTH_EN_FORCE_MODEM_IMAGE_AUTHENTICATION_FVAL                 0x1
#define HWIO_FEATURE_CONFIG5_SPARE_8_7_BMSK                                                        0x180
#define HWIO_FEATURE_CONFIG5_SPARE_8_7_SHFT                                                          0x7
#define HWIO_FEATURE_CONFIG5_MODEM_BOOT_FROM_ROM_BMSK                                               0x40
#define HWIO_FEATURE_CONFIG5_MODEM_BOOT_FROM_ROM_SHFT                                                0x6
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_BMSK                                            0x20
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_SHFT                                             0x5
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_ENABLE_PATCHING_FVAL                             0x0
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_DISABLE_PATCHING_FVAL                            0x1
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                                 0x1f
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                                  0x0

#define HWIO_OEM_CONFIG0_ADDR                                                              (SECURITY_CONTROL_BASE + 0x0000601c)
#define HWIO_OEM_CONFIG0_PHYS                                                              (SECURITY_CONTROL_BASE + 0x0000601c)
#define HWIO_OEM_CONFIG0_OFFS                                                                 (0x0000601c)
#define HWIO_OEM_CONFIG0_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG0_POR                                                                  0x00000000
#define HWIO_OEM_CONFIG0_ATTR                                                                        0x1
#define HWIO_OEM_CONFIG0_IN      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, HWIO_OEM_CONFIG0_RMSK)
#define HWIO_OEM_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, m)
#define HWIO_OEM_CONFIG0_SPARE0_BMSK                                                          0xff800000
#define HWIO_OEM_CONFIG0_SPARE0_SHFT                                                                0x17
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_BMSK                                                   0x400000
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_SHFT                                                       0x16
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_BMSK                                               0x3c0000
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_SHFT                                                   0x12
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_BMSK                                                 0x20000
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_SHFT                                                    0x11
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_NOT_SECURE_FVAL                                          0x0
#define HWIO_OEM_CONFIG0_SPARE_REG19_SECURE_SECURE_FVAL                                              0x1
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_BMSK                                                 0x10000
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_SHFT                                                    0x10
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_NOT_SECURE_FVAL                                          0x0
#define HWIO_OEM_CONFIG0_SPARE_REG18_SECURE_SECURE_FVAL                                              0x1
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_BMSK                                                  0x8000
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_SHFT                                                     0xf
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_NOT_SECURE_FVAL                                          0x0
#define HWIO_OEM_CONFIG0_SPARE_REG17_SECURE_SECURE_FVAL                                              0x1
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_BMSK                                                  0x4000
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_SHFT                                                     0xe
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_NOT_SECURE_FVAL                                          0x0
#define HWIO_OEM_CONFIG0_SPARE_REG16_SECURE_SECURE_FVAL                                              0x1
#define HWIO_OEM_CONFIG0_SPARE_REG15_SECURE_BMSK                                                  0x2000
#define HWIO_OEM_CONFIG0_SPARE_REG15_SECURE_SHFT                                                     0xd
#define HWIO_OEM_CONFIG0_SPARE_REG15_SECURE_NOT_SECURE_FVAL                                          0x0
#define HWIO_OEM_CONFIG0_SPARE_REG15_SECURE_SECURE_FVAL                                              0x1
#define HWIO_OEM_CONFIG0_WDOG_EN_BMSK                                                             0x1000
#define HWIO_OEM_CONFIG0_WDOG_EN_SHFT                                                                0xc
#define HWIO_OEM_CONFIG0_WDOG_EN_USE_GPIO_FVAL                                                       0x0
#define HWIO_OEM_CONFIG0_WDOG_EN_IGNORE_GPIO_ENABLE_WDOG_FVAL                                        0x1
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_BMSK                                                     0x800
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_SHFT                                                       0xb
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_NORMAL_MODE_FVAL                                           0x0
#define HWIO_OEM_CONFIG0_SPDM_SECURE_MODE_SECURE_MODE_FVAL                                           0x1
#define HWIO_OEM_CONFIG0_ALT_SD_PORT_FOR_BOOT_BMSK                                                 0x400
#define HWIO_OEM_CONFIG0_ALT_SD_PORT_FOR_BOOT_SHFT                                                   0xa
#define HWIO_OEM_CONFIG0_ALT_SD_PORT_FOR_BOOT_BOOT_FROM_SD_CARD_CONNECTED_ON_SDC2_PORT_FVAL          0x0
#define HWIO_OEM_CONFIG0_ALT_SD_PORT_FOR_BOOT_BOOT_FROM_SD_CARD_CONNECTED_ON_SDC3_PORT_FVAL          0x1
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                        0x200
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                          0x9
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_BMSK                                                  0x100
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_SHFT                                                    0x8
#define HWIO_OEM_CONFIG0_FAST_BOOT_BMSK                                                             0xe0
#define HWIO_OEM_CONFIG0_FAST_BOOT_SHFT                                                              0x5
#define HWIO_OEM_CONFIG0_FAST_BOOT_DEFAULT_FVAL                                                      0x0
#define HWIO_OEM_CONFIG0_FAST_BOOT_SD_SDC2_EMMC_SDC1_FVAL                                            0x1
#define HWIO_OEM_CONFIG0_FAST_BOOT_EMMC_SDC1_FVAL                                                    0x2
#define HWIO_OEM_CONFIG0_FAST_BOOT_USB_FVAL                                                          0x3
#define HWIO_OEM_CONFIG0_FAST_BOOT_HSIC_FVAL                                                         0x4
#define HWIO_OEM_CONFIG0_FAST_BOOT_NAND_FVAL                                                         0x5
#define HWIO_OEM_CONFIG0_FAST_BOOT_SPI_ON_BLSP1_QUP1_FVAL                                            0x6
#define HWIO_OEM_CONFIG0_FAST_BOOT_SPI_ON_BLSP1_QUP2_FVAL                                            0x7
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_BMSK                                                   0x10
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_SHFT                                                    0x4
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_ENUM_19_2_MHZ_FVAL                                      0x0
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_ENUM_25_MHZ_FVAL                                        0x1
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_BMSK                                                    0x8
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_SHFT                                                    0x3
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL                         0x1
#define HWIO_OEM_CONFIG0_SPARE_BMSK                                                                  0x4
#define HWIO_OEM_CONFIG0_SPARE_SHFT                                                                  0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_BMSK                                                           0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_SHFT                                                           0x1
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_TIMEOUT_DISABLED_FVAL                                          0x0
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                                       0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_BMSK                                                        0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_SHFT                                                        0x0
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_DOWNLOADER_ENABLED_FVAL                                     0x0
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_DOWNLOADER_DISABLED_FVAL                                    0x1

#define HWIO_OEM_CONFIG1_ADDR                                                              (SECURITY_CONTROL_BASE + 0x00006020)
#define HWIO_OEM_CONFIG1_PHYS                                                              (SECURITY_CONTROL_BASE + 0x00006020)
#define HWIO_OEM_CONFIG1_OFFS                                                                 (0x00006020)
#define HWIO_OEM_CONFIG1_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG1_POR                                                                  0x00000000
#define HWIO_OEM_CONFIG1_ATTR                                                                        0x1
#define HWIO_OEM_CONFIG1_IN      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, HWIO_OEM_CONFIG1_RMSK)
#define HWIO_OEM_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, m)
#define HWIO_OEM_CONFIG1_SPARE1_BMSK                                                          0xff800000
#define HWIO_OEM_CONFIG1_SPARE1_SHFT                                                                0x17
#define HWIO_OEM_CONFIG1_ANTI_ROLLBACK_FEATURE_EN_BMSK                                          0x780000
#define HWIO_OEM_CONFIG1_ANTI_ROLLBACK_FEATURE_EN_SHFT                                              0x13
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_BMSK                                               0x40000
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_SHFT                                                  0x12
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_BMSK                                                0x20000
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_SHFT                                                   0x11
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_BMSK                                                 0x10000
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_SHFT                                                    0x10
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_BMSK                                                   0x8000
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_SHFT                                                      0xf
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_BMSK                                                   0x4000
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_SHFT                                                      0xe
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_BMSK                                                0x2000
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_SHFT                                                   0xd
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_BMSK                                                 0x1000
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_SHFT                                                    0xc
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_BMSK                                                   0x800
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_SHFT                                                     0xb
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_BMSK                                                   0x400
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_SHFT                                                     0xa
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_OEM_CONFIG1_SPARE1_DISABLE_BMSK                                                       0x200
#define HWIO_OEM_CONFIG1_SPARE1_DISABLE_SHFT                                                         0x9
#define HWIO_OEM_CONFIG1_SPARE0_DISABLE_BMSK                                                       0x100
#define HWIO_OEM_CONFIG1_SPARE0_DISABLE_SHFT                                                         0x8
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_BMSK                                                 0x80
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_SHFT                                                  0x7
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_OEM_CONFIG1_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_BMSK                                                     0x40
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_SHFT                                                      0x6
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG1_RPM_DAPEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_OEM_CONFIG1_RPM_WCSS_NIDEN_DISABLE_BMSK                                                0x20
#define HWIO_OEM_CONFIG1_RPM_WCSS_NIDEN_DISABLE_SHFT                                                 0x5
#define HWIO_OEM_CONFIG1_RPM_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_OEM_CONFIG1_RPM_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_BMSK                                                     0x10
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_SHFT                                                      0x4
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG1_RPM_DBGEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_BMSK                                                     0x8
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_SHFT                                                     0x3
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_OEM_CONFIG1_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_BMSK                                                      0x4
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_SHFT                                                      0x2
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG1_MSS_NIDEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_BMSK                                                      0x2
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_SHFT                                                      0x1
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG1_MSS_DBGEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_OEM_CONFIG1_ALL_DEBUG_DISABLE_BMSK                                                      0x1
#define HWIO_OEM_CONFIG1_ALL_DEBUG_DISABLE_SHFT                                                      0x0
#define HWIO_OEM_CONFIG1_ALL_DEBUG_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG1_ALL_DEBUG_DISABLE_DISABLE_FVAL                                              0x1

#define HWIO_OEM_CONFIG2_ADDR                                                              (SECURITY_CONTROL_BASE + 0x00006024)
#define HWIO_OEM_CONFIG2_PHYS                                                              (SECURITY_CONTROL_BASE + 0x00006024)
#define HWIO_OEM_CONFIG2_OFFS                                                                 (0x00006024)
#define HWIO_OEM_CONFIG2_RMSK                                                                 0xffffffff
#define HWIO_OEM_CONFIG2_POR                                                                  0x00000000
#define HWIO_OEM_CONFIG2_ATTR                                                                        0x1
#define HWIO_OEM_CONFIG2_IN      \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, HWIO_OEM_CONFIG2_RMSK)
#define HWIO_OEM_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, m)
#define HWIO_OEM_CONFIG2_OEM_PRODUCT_ID_BMSK                                                  0xffff0000
#define HWIO_OEM_CONFIG2_OEM_PRODUCT_ID_SHFT                                                        0x10
#define HWIO_OEM_CONFIG2_OEM_HW_ID_BMSK                                                           0xffff
#define HWIO_OEM_CONFIG2_OEM_HW_ID_SHFT                                                              0x0



#endif /* QDSS_FUSE_REGS_H */
