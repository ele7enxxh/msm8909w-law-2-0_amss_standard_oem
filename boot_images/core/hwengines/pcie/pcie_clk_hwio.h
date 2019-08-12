#ifndef __PCIE_GCC_CLK_CTL__
#define __PCIE_GCC_CLK_CTL__
/*
===========================================================================
*/
/**
  @file GCC_CLK_CTL
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG

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

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/pcie/pcie_clk_hwio.h#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "msmhwiobase.h"
#include "HALhwio.h"


/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/



#define GCC_CLK_CTL_REG_REG_BASE                                                                      (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_PHYS                                                                 (CLK_CTL_BASE_PHYS + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_OFFS                                                                 0x00000000

#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00001b40)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_PHYS                                                             (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001b40)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OFFS                                                             (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001b40)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_RMSK                                                                    0x1
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_POR                                                              0x00000000
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ATTR                                                                    0x3
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR, HWIO_GCC_PCIE_BOOT_CLOCK_CTL_RMSK)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR, m)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR,v)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR,m,v,HWIO_GCC_PCIE_BOOT_CLOCK_CTL_IN)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_SHFT                                                         0x0
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_DISABLE_FVAL                                                 0x0
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_ENABLE_FVAL                                                  0x1


#define HWIO_GCC_PCIE_BCR_ADDR                                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00001c00)
#define HWIO_GCC_PCIE_BCR_PHYS                                                                        (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001c00)
#define HWIO_GCC_PCIE_BCR_OFFS                                                                        (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001c00)
#define HWIO_GCC_PCIE_BCR_RMSK                                                                               0x1
#define HWIO_GCC_PCIE_BCR_POR                                                                         0x00000000
#define HWIO_GCC_PCIE_BCR_ATTR                                                                               0x3
#define HWIO_GCC_PCIE_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_BCR_ADDR, HWIO_GCC_PCIE_BCR_RMSK)
#define HWIO_GCC_PCIE_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_BCR_ADDR, m)
#define HWIO_GCC_PCIE_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_BCR_ADDR,v)
#define HWIO_GCC_PCIE_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_BCR_ADDR,m,v,HWIO_GCC_PCIE_BCR_IN)
#define HWIO_GCC_PCIE_BCR_BLK_ARES_BMSK                                                                      0x1
#define HWIO_GCC_PCIE_BCR_BLK_ARES_SHFT                                                                      0x0
#define HWIO_GCC_PCIE_BCR_BLK_ARES_DISABLE_FVAL                                                              0x0
#define HWIO_GCC_PCIE_BCR_BLK_ARES_ENABLE_FVAL                                                               0x1

#define HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00001c04)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001c04)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001c04)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_RMSK                                                               0xf0008001
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_POR                                                                0x80008000
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_ATTR                                                                      0x3
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR, HWIO_GCC_PCIE_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCIE_CFG_AHB_CBCR_IN)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                      0x70000000
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                            0x1c
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                              0x8000
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                 0xf
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_DISABLE_FVAL                                         0x0
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_ENABLE_FVAL                                          0x1
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                           0x0
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_ENABLE_DISABLE_FVAL                                                   0x0
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_ENABLE_ENABLE_FVAL                                                    0x1

#define HWIO_GCC_PCIE_PIPE_CBCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00001c08)
#define HWIO_GCC_PCIE_PIPE_CBCR_PHYS                                                                  (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001c08)
#define HWIO_GCC_PCIE_PIPE_CBCR_OFFS                                                                  (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001c08)
#define HWIO_GCC_PCIE_PIPE_CBCR_RMSK                                                                  0x80007ff1
#define HWIO_GCC_PCIE_PIPE_CBCR_POR                                                                   0x80004ff0
#define HWIO_GCC_PCIE_PIPE_CBCR_ATTR                                                                         0x3
#define HWIO_GCC_PCIE_PIPE_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CBCR_ADDR, HWIO_GCC_PCIE_PIPE_CBCR_RMSK)
#define HWIO_GCC_PCIE_PIPE_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_PIPE_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_PIPE_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_PIPE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_PIPE_CBCR_ADDR,m,v,HWIO_GCC_PCIE_PIPE_CBCR_IN)
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_OFF_BMSK                                                          0x80000000
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_OFF_SHFT                                                                0x1f
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_CORE_ON_BMSK                                                    0x4000
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_CORE_ON_SHFT                                                       0xe
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_CORE_ON_FORCE_DISABLE_FVAL                                         0x0
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_CORE_ON_FORCE_ENABLE_FVAL                                          0x1
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                                  0x2000
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                     0xd
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_ON_FORCE_DISABLE_FVAL                                       0x0
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_ON_FORCE_ENABLE_FVAL                                        0x1
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                                 0x1000
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                    0xc
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_OFF_FORCE_DISABLE_FVAL                                      0x0
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_OFF_FORCE_ENABLE_FVAL                                       0x1
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_BMSK                                                                0xf00
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_SHFT                                                                  0x8
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK0_FVAL                                                           0x0
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK1_FVAL                                                           0x1
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK2_FVAL                                                           0x2
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK3_FVAL                                                           0x3
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK4_FVAL                                                           0x4
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK5_FVAL                                                           0x5
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK6_FVAL                                                           0x6
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK7_FVAL                                                           0x7
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK8_FVAL                                                           0x8
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK9_FVAL                                                           0x9
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK10_FVAL                                                          0xa
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK11_FVAL                                                          0xb
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK12_FVAL                                                          0xc
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK13_FVAL                                                          0xd
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK14_FVAL                                                          0xe
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_CLOCK15_FVAL                                                          0xf
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_BMSK                                                                  0xf0
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_SHFT                                                                   0x4
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK0_FVAL                                                            0x0
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK1_FVAL                                                            0x1
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK2_FVAL                                                            0x2
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK3_FVAL                                                            0x3
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK4_FVAL                                                            0x4
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK5_FVAL                                                            0x5
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK6_FVAL                                                            0x6
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK7_FVAL                                                            0x7
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK8_FVAL                                                            0x8
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK9_FVAL                                                            0x9
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK10_FVAL                                                           0xa
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK11_FVAL                                                           0xb
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK12_FVAL                                                           0xc
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK13_FVAL                                                           0xd
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK14_FVAL                                                           0xe
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_CLOCK15_FVAL                                                           0xf
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_ENABLE_BMSK                                                              0x1
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_ENABLE_SHFT                                                              0x0
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_ENABLE_DISABLE_FVAL                                                      0x0
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_ENABLE_ENABLE_FVAL                                                       0x1

#define HWIO_GCC_PCIE_AXI_CBCR_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00001c0c)
#define HWIO_GCC_PCIE_AXI_CBCR_PHYS                                                                   (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001c0c)
#define HWIO_GCC_PCIE_AXI_CBCR_OFFS                                                                   (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001c0c)
#define HWIO_GCC_PCIE_AXI_CBCR_RMSK                                                                   0xf000fff1
#define HWIO_GCC_PCIE_AXI_CBCR_POR                                                                    0x8000cff0
#define HWIO_GCC_PCIE_AXI_CBCR_ATTR                                                                          0x3
#define HWIO_GCC_PCIE_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AXI_CBCR_ADDR, HWIO_GCC_PCIE_AXI_CBCR_RMSK)
#define HWIO_GCC_PCIE_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AXI_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AXI_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AXI_CBCR_ADDR,m,v,HWIO_GCC_PCIE_AXI_CBCR_IN)
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_OFF_BMSK                                                           0x80000000
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_OFF_SHFT                                                                 0x1f
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                          0x70000000
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                                0x1c
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                                  0x8000
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                     0xf
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_DISABLE_FVAL                                             0x0
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_ENABLE_FVAL                                              0x1
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                                     0x4000
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                                        0xe
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_CORE_ON_FORCE_DISABLE_FVAL                                          0x0
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_CORE_ON_FORCE_ENABLE_FVAL                                           0x1
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                                   0x2000
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                      0xd
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_ON_FORCE_DISABLE_FVAL                                        0x0
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_ON_FORCE_ENABLE_FVAL                                         0x1
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                                  0x1000
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                     0xc
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_OFF_FORCE_DISABLE_FVAL                                       0x0
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_OFF_FORCE_ENABLE_FVAL                                        0x1
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_BMSK                                                                 0xf00
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_SHFT                                                                   0x8
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK0_FVAL                                                            0x0
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK1_FVAL                                                            0x1
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK2_FVAL                                                            0x2
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK3_FVAL                                                            0x3
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK4_FVAL                                                            0x4
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK5_FVAL                                                            0x5
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK6_FVAL                                                            0x6
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK7_FVAL                                                            0x7
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK8_FVAL                                                            0x8
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK9_FVAL                                                            0x9
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK10_FVAL                                                           0xa
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK11_FVAL                                                           0xb
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK12_FVAL                                                           0xc
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK13_FVAL                                                           0xd
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK14_FVAL                                                           0xe
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_CLOCK15_FVAL                                                           0xf
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_BMSK                                                                   0xf0
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_SHFT                                                                    0x4
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK0_FVAL                                                             0x0
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK1_FVAL                                                             0x1
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK2_FVAL                                                             0x2
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK3_FVAL                                                             0x3
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK4_FVAL                                                             0x4
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK5_FVAL                                                             0x5
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK6_FVAL                                                             0x6
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK7_FVAL                                                             0x7
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK8_FVAL                                                             0x8
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK9_FVAL                                                             0x9
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK10_FVAL                                                            0xa
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK11_FVAL                                                            0xb
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK12_FVAL                                                            0xc
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK13_FVAL                                                            0xd
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK14_FVAL                                                            0xe
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_CLOCK15_FVAL                                                            0xf
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_ENABLE_BMSK                                                               0x1
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_ENABLE_SHFT                                                               0x0
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_ENABLE_DISABLE_FVAL                                                       0x0
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_ENABLE_ENABLE_FVAL                                                        0x1

#define HWIO_GCC_PCIE_SLEEP_CBCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00001c10)
#define HWIO_GCC_PCIE_SLEEP_CBCR_PHYS                                                                 (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001c10)
#define HWIO_GCC_PCIE_SLEEP_CBCR_OFFS                                                                 (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001c10)
#define HWIO_GCC_PCIE_SLEEP_CBCR_RMSK                                                                 0x80000001
#define HWIO_GCC_PCIE_SLEEP_CBCR_POR                                                                  0x80000000
#define HWIO_GCC_PCIE_SLEEP_CBCR_ATTR                                                                        0x3
#define HWIO_GCC_PCIE_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_SLEEP_CBCR_ADDR, HWIO_GCC_PCIE_SLEEP_CBCR_RMSK)
#define HWIO_GCC_PCIE_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_PCIE_SLEEP_CBCR_IN)
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_OFF_BMSK                                                         0x80000000
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_OFF_SHFT                                                               0x1f
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_ENABLE_BMSK                                                             0x1
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_ENABLE_SHFT                                                             0x0
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_ENABLE_DISABLE_FVAL                                                     0x0
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_ENABLE_ENABLE_FVAL                                                      0x1

#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00001c2c)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_PHYS                                                              (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001c2c)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_OFFS                                                              (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001c2c)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_RMSK                                                              0x80007ff1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_POR                                                               0x80004ff0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_ATTR                                                                     0x3
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR, HWIO_GCC_PCIE_AXI_MSTR_CBCR_RMSK)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR,m,v,HWIO_GCC_PCIE_AXI_MSTR_CBCR_IN)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_CORE_ON_BMSK                                                0x4000
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_CORE_ON_SHFT                                                   0xe
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_CORE_ON_FORCE_DISABLE_FVAL                                     0x0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_CORE_ON_FORCE_ENABLE_FVAL                                      0x1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                              0x2000
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                 0xd
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_ON_FORCE_DISABLE_FVAL                                   0x0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_ON_FORCE_ENABLE_FVAL                                    0x1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                             0x1000
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                0xc
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_OFF_FORCE_DISABLE_FVAL                                  0x0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_OFF_FORCE_ENABLE_FVAL                                   0x1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_BMSK                                                            0xf00
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_SHFT                                                              0x8
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK0_FVAL                                                       0x0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK1_FVAL                                                       0x1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK2_FVAL                                                       0x2
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK3_FVAL                                                       0x3
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK4_FVAL                                                       0x4
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK5_FVAL                                                       0x5
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK6_FVAL                                                       0x6
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK7_FVAL                                                       0x7
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK8_FVAL                                                       0x8
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK9_FVAL                                                       0x9
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK10_FVAL                                                      0xa
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK11_FVAL                                                      0xb
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK12_FVAL                                                      0xc
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK13_FVAL                                                      0xd
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK14_FVAL                                                      0xe
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_CLOCK15_FVAL                                                      0xf
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_BMSK                                                              0xf0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_SHFT                                                               0x4
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK0_FVAL                                                        0x0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK1_FVAL                                                        0x1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK2_FVAL                                                        0x2
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK3_FVAL                                                        0x3
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK4_FVAL                                                        0x4
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK5_FVAL                                                        0x5
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK6_FVAL                                                        0x6
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK7_FVAL                                                        0x7
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK8_FVAL                                                        0x8
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK9_FVAL                                                        0x9
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK10_FVAL                                                       0xa
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK11_FVAL                                                       0xb
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK12_FVAL                                                       0xc
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK13_FVAL                                                       0xd
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK14_FVAL                                                       0xe
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_CLOCK15_FVAL                                                       0xf
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_ENABLE_SHFT                                                          0x0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_ENABLE_DISABLE_FVAL                                                  0x0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_ENABLE_ENABLE_FVAL                                                   0x1

#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00001c14)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_PHYS                                                              (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001c14)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_OFFS                                                              (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001c14)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_RMSK                                                              0x80000013
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_POR                                                               0x80000000
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ATTR                                                                     0x3
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR, HWIO_GCC_PCIE_PIPE_CMD_RCGR_RMSK)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR, m)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR,v)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR,m,v,HWIO_GCC_PCIE_PIPE_CMD_RCGR_IN)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_OFF_SHFT                                                           0x1f
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                     0x10
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                      0x4
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_EN_BMSK                                                             0x2
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_EN_SHFT                                                             0x1
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_EN_DISABLE_FVAL                                                     0x0
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_EN_ENABLE_FVAL                                                      0x1
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_UPDATE_BMSK                                                              0x1
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_UPDATE_SHFT                                                              0x0
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_UPDATE_DISABLE_FVAL                                                      0x0
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_UPDATE_ENABLE_FVAL                                                       0x1

#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00001c18)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_PHYS                                                              (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001c18)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_OFFS                                                              (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001c18)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_RMSK                                                                   0x71f
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_POR                                                               0x00000000
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_ATTR                                                                     0x3
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR, HWIO_GCC_PCIE_PIPE_CFG_RCGR_RMSK)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR, m)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR,v)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR,m,v,HWIO_GCC_PCIE_PIPE_CFG_RCGR_IN)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_BMSK                                                           0x700
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SHFT                                                             0x8
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SRC0_FVAL                                                        0x0
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SRC1_FVAL                                                        0x1
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SRC2_FVAL                                                        0x2
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SRC3_FVAL                                                        0x3
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SRC4_FVAL                                                        0x4
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SRC5_FVAL                                                        0x5
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SRC6_FVAL                                                        0x6
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SRC7_FVAL                                                        0x7
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_BMSK                                                            0x1f
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_SHFT                                                             0x0
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_BYPASS_FVAL                                                      0x0
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV1_FVAL                                                        0x1
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV1_5_FVAL                                                      0x2
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV2_FVAL                                                        0x3
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV2_5_FVAL                                                      0x4
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV3_FVAL                                                        0x5
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV3_5_FVAL                                                      0x6
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV4_FVAL                                                        0x7
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV4_5_FVAL                                                      0x8
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV5_FVAL                                                        0x9
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV5_5_FVAL                                                      0xa
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV6_FVAL                                                        0xb
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV6_5_FVAL                                                      0xc
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV7_FVAL                                                        0xd
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV7_5_FVAL                                                      0xe
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV8_FVAL                                                        0xf
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV8_5_FVAL                                                     0x10
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV9_FVAL                                                       0x11
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV9_5_FVAL                                                     0x12
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV10_FVAL                                                      0x13
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV10_5_FVAL                                                    0x14
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV11_FVAL                                                      0x15
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV11_5_FVAL                                                    0x16
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV12_FVAL                                                      0x17
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV12_5_FVAL                                                    0x18
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV13_FVAL                                                      0x19
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV13_5_FVAL                                                    0x1a
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV14_FVAL                                                      0x1b
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV14_5_FVAL                                                    0x1c
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV15_FVAL                                                      0x1d
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV15_5_FVAL                                                    0x1e
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_DIV16_FVAL                                                      0x1f

#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e00)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e00)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e00)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_RMSK                                                               0x800000f3
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_POR                                                                0x80000000
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ATTR                                                                      0x3
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR, HWIO_GCC_PCIE_AUX_CMD_RCGR_RMSK)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR, m)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR,v)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR,m,v,HWIO_GCC_PCIE_AUX_CMD_RCGR_IN)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_OFF_SHFT                                                            0x1f
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_D_BMSK                                                             0x80
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_D_SHFT                                                              0x7
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_M_BMSK                                                             0x40
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_M_SHFT                                                              0x6
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_N_BMSK                                                             0x20
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_N_SHFT                                                              0x5
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                      0x10
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                       0x4
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_EN_BMSK                                                              0x2
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_EN_SHFT                                                              0x1
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_EN_DISABLE_FVAL                                                      0x0
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_EN_ENABLE_FVAL                                                       0x1
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_UPDATE_BMSK                                                               0x1
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_UPDATE_SHFT                                                               0x0
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_UPDATE_DISABLE_FVAL                                                       0x0
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_UPDATE_ENABLE_FVAL                                                        0x1

#define HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e04)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e04)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e04)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_RMSK                                                                   0x371f
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_POR                                                                0x00000000
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_ATTR                                                                      0x3
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR, HWIO_GCC_PCIE_AUX_CFG_RCGR_RMSK)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR, m)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR,v)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR,m,v,HWIO_GCC_PCIE_AUX_CFG_RCGR_IN)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_MODE_BMSK                                                              0x3000
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_MODE_SHFT                                                                 0xc
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_MODE_SINGLE_EDGE_FVAL                                                     0x0
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_MODE_DUAL_EDGE_FVAL                                                       0x1
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_MODE_SWALLOW_FVAL                                                         0x2
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_MODE_BYPASS_FVAL                                                          0x3
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_BMSK                                                            0x700
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SHFT                                                              0x8
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SRC0_FVAL                                                         0x0
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SRC1_FVAL                                                         0x1
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SRC2_FVAL                                                         0x2
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SRC3_FVAL                                                         0x3
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SRC4_FVAL                                                         0x4
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SRC5_FVAL                                                         0x5
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SRC6_FVAL                                                         0x6
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SRC7_FVAL                                                         0x7
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_BMSK                                                             0x1f
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_SHFT                                                              0x0
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_BYPASS_FVAL                                                       0x0
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV1_FVAL                                                         0x1
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV1_5_FVAL                                                       0x2
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV2_FVAL                                                         0x3
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV2_5_FVAL                                                       0x4
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV3_FVAL                                                         0x5
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV3_5_FVAL                                                       0x6
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV4_FVAL                                                         0x7
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV4_5_FVAL                                                       0x8
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV5_FVAL                                                         0x9
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV5_5_FVAL                                                       0xa
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV6_FVAL                                                         0xb
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV6_5_FVAL                                                       0xc
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV7_FVAL                                                         0xd
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV7_5_FVAL                                                       0xe
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV8_FVAL                                                         0xf
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV8_5_FVAL                                                      0x10
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV9_FVAL                                                        0x11
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV9_5_FVAL                                                      0x12
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV10_FVAL                                                       0x13
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV10_5_FVAL                                                     0x14
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV11_FVAL                                                       0x15
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV11_5_FVAL                                                     0x16
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV12_FVAL                                                       0x17
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV12_5_FVAL                                                     0x18
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV13_FVAL                                                       0x19
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV13_5_FVAL                                                     0x1a
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV14_FVAL                                                       0x1b
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV14_5_FVAL                                                     0x1c
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV15_FVAL                                                       0x1d
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV15_5_FVAL                                                     0x1e
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_DIV16_FVAL                                                       0x1f

#define HWIO_GCC_PCIE_AUX_M_ADDR                                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e08)
#define HWIO_GCC_PCIE_AUX_M_PHYS                                                                      (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e08)
#define HWIO_GCC_PCIE_AUX_M_OFFS                                                                      (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e08)
#define HWIO_GCC_PCIE_AUX_M_RMSK                                                                          0xffff
#define HWIO_GCC_PCIE_AUX_M_POR                                                                       0x00000000
#define HWIO_GCC_PCIE_AUX_M_ATTR                                                                             0x3
#define HWIO_GCC_PCIE_AUX_M_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_M_ADDR, HWIO_GCC_PCIE_AUX_M_RMSK)
#define HWIO_GCC_PCIE_AUX_M_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_M_ADDR, m)
#define HWIO_GCC_PCIE_AUX_M_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_M_ADDR,v)
#define HWIO_GCC_PCIE_AUX_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_M_ADDR,m,v,HWIO_GCC_PCIE_AUX_M_IN)
#define HWIO_GCC_PCIE_AUX_M_M_BMSK                                                                        0xffff
#define HWIO_GCC_PCIE_AUX_M_M_SHFT                                                                           0x0

#define HWIO_GCC_PCIE_AUX_N_ADDR                                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e0c)
#define HWIO_GCC_PCIE_AUX_N_PHYS                                                                      (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e0c)
#define HWIO_GCC_PCIE_AUX_N_OFFS                                                                      (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e0c)
#define HWIO_GCC_PCIE_AUX_N_RMSK                                                                          0xffff
#define HWIO_GCC_PCIE_AUX_N_POR                                                                       0x00000000
#define HWIO_GCC_PCIE_AUX_N_ATTR                                                                             0x3
#define HWIO_GCC_PCIE_AUX_N_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_N_ADDR, HWIO_GCC_PCIE_AUX_N_RMSK)
#define HWIO_GCC_PCIE_AUX_N_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_N_ADDR, m)
#define HWIO_GCC_PCIE_AUX_N_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_N_ADDR,v)
#define HWIO_GCC_PCIE_AUX_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_N_ADDR,m,v,HWIO_GCC_PCIE_AUX_N_IN)
#define HWIO_GCC_PCIE_AUX_N_NOT_N_MINUS_M_BMSK                                                            0xffff
#define HWIO_GCC_PCIE_AUX_N_NOT_N_MINUS_M_SHFT                                                               0x0

#define HWIO_GCC_PCIE_AUX_D_ADDR                                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e10)
#define HWIO_GCC_PCIE_AUX_D_PHYS                                                                      (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e10)
#define HWIO_GCC_PCIE_AUX_D_OFFS                                                                      (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e10)
#define HWIO_GCC_PCIE_AUX_D_RMSK                                                                          0xffff
#define HWIO_GCC_PCIE_AUX_D_POR                                                                       0x00000000
#define HWIO_GCC_PCIE_AUX_D_ATTR                                                                             0x3
#define HWIO_GCC_PCIE_AUX_D_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_D_ADDR, HWIO_GCC_PCIE_AUX_D_RMSK)
#define HWIO_GCC_PCIE_AUX_D_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_D_ADDR, m)
#define HWIO_GCC_PCIE_AUX_D_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_D_ADDR,v)
#define HWIO_GCC_PCIE_AUX_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_D_ADDR,m,v,HWIO_GCC_PCIE_AUX_D_IN)
#define HWIO_GCC_PCIE_AUX_D_NOT_2D_BMSK                                                                   0xffff
#define HWIO_GCC_PCIE_AUX_D_NOT_2D_SHFT                                                                      0x0

#define HWIO_GCC_USB_30_GDSCR_ADDR                                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00001b80)
#define HWIO_GCC_USB_30_GDSCR_PHYS                                                                    (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001b80)
#define HWIO_GCC_USB_30_GDSCR_OFFS                                                                    (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001b80)
#define HWIO_GCC_USB_30_GDSCR_RMSK                                                                    0xf8ffffff
#define HWIO_GCC_USB_30_GDSCR_POR                                                                     0x00222001
#define HWIO_GCC_USB_30_GDSCR_ATTR                                                                           0x3
#define HWIO_GCC_USB_30_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_USB_30_GDSCR_ADDR, HWIO_GCC_USB_30_GDSCR_RMSK)
#define HWIO_GCC_USB_30_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_30_GDSCR_ADDR, m)
#define HWIO_GCC_USB_30_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_30_GDSCR_ADDR,v)
#define HWIO_GCC_USB_30_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_30_GDSCR_ADDR,m,v,HWIO_GCC_USB_30_GDSCR_IN)
#define HWIO_GCC_USB_30_GDSCR_PWR_ON_BMSK                                                             0x80000000
#define HWIO_GCC_USB_30_GDSCR_PWR_ON_SHFT                                                                   0x1f
#define HWIO_GCC_USB_30_GDSCR_GDSC_STATE_BMSK                                                         0x78000000
#define HWIO_GCC_USB_30_GDSCR_GDSC_STATE_SHFT                                                               0x1b
#define HWIO_GCC_USB_30_GDSCR_EN_REST_WAIT_BMSK                                                         0xf00000
#define HWIO_GCC_USB_30_GDSCR_EN_REST_WAIT_SHFT                                                             0x14
#define HWIO_GCC_USB_30_GDSCR_EN_FEW_WAIT_BMSK                                                           0xf0000
#define HWIO_GCC_USB_30_GDSCR_EN_FEW_WAIT_SHFT                                                              0x10
#define HWIO_GCC_USB_30_GDSCR_CLK_DIS_WAIT_BMSK                                                           0xf000
#define HWIO_GCC_USB_30_GDSCR_CLK_DIS_WAIT_SHFT                                                              0xc
#define HWIO_GCC_USB_30_GDSCR_RETAIN_FF_ENABLE_BMSK                                                        0x800
#define HWIO_GCC_USB_30_GDSCR_RETAIN_FF_ENABLE_SHFT                                                          0xb
#define HWIO_GCC_USB_30_GDSCR_RETAIN_FF_ENABLE_DISABLE_FVAL                                                  0x0
#define HWIO_GCC_USB_30_GDSCR_RETAIN_FF_ENABLE_ENABLE_FVAL                                                   0x1
#define HWIO_GCC_USB_30_GDSCR_RESTORE_BMSK                                                                 0x400
#define HWIO_GCC_USB_30_GDSCR_RESTORE_SHFT                                                                   0xa
#define HWIO_GCC_USB_30_GDSCR_RESTORE_DISABLE_FVAL                                                           0x0
#define HWIO_GCC_USB_30_GDSCR_RESTORE_ENABLE_FVAL                                                            0x1
#define HWIO_GCC_USB_30_GDSCR_SAVE_BMSK                                                                    0x200
#define HWIO_GCC_USB_30_GDSCR_SAVE_SHFT                                                                      0x9
#define HWIO_GCC_USB_30_GDSCR_SAVE_DISABLE_FVAL                                                              0x0
#define HWIO_GCC_USB_30_GDSCR_SAVE_ENABLE_FVAL                                                               0x1
#define HWIO_GCC_USB_30_GDSCR_RETAIN_BMSK                                                                  0x100
#define HWIO_GCC_USB_30_GDSCR_RETAIN_SHFT                                                                    0x8
#define HWIO_GCC_USB_30_GDSCR_RETAIN_DISABLE_FVAL                                                            0x0
#define HWIO_GCC_USB_30_GDSCR_RETAIN_ENABLE_FVAL                                                             0x1
#define HWIO_GCC_USB_30_GDSCR_EN_REST_BMSK                                                                  0x80
#define HWIO_GCC_USB_30_GDSCR_EN_REST_SHFT                                                                   0x7
#define HWIO_GCC_USB_30_GDSCR_EN_REST_DISABLE_FVAL                                                           0x0
#define HWIO_GCC_USB_30_GDSCR_EN_REST_ENABLE_FVAL                                                            0x1
#define HWIO_GCC_USB_30_GDSCR_EN_FEW_BMSK                                                                   0x40
#define HWIO_GCC_USB_30_GDSCR_EN_FEW_SHFT                                                                    0x6
#define HWIO_GCC_USB_30_GDSCR_EN_FEW_DISABLE_FVAL                                                            0x0
#define HWIO_GCC_USB_30_GDSCR_EN_FEW_ENABLE_FVAL                                                             0x1
#define HWIO_GCC_USB_30_GDSCR_CLAMP_IO_BMSK                                                                 0x20
#define HWIO_GCC_USB_30_GDSCR_CLAMP_IO_SHFT                                                                  0x5
#define HWIO_GCC_USB_30_GDSCR_CLAMP_IO_DISABLE_FVAL                                                          0x0
#define HWIO_GCC_USB_30_GDSCR_CLAMP_IO_ENABLE_FVAL                                                           0x1
#define HWIO_GCC_USB_30_GDSCR_CLK_DISABLE_BMSK                                                              0x10
#define HWIO_GCC_USB_30_GDSCR_CLK_DISABLE_SHFT                                                               0x4
#define HWIO_GCC_USB_30_GDSCR_CLK_DISABLE_CLK_NOT_DISABLE_FVAL                                               0x0
#define HWIO_GCC_USB_30_GDSCR_CLK_DISABLE_CLK_DISABLE_FVAL                                                   0x1
#define HWIO_GCC_USB_30_GDSCR_PD_ARES_BMSK                                                                   0x8
#define HWIO_GCC_USB_30_GDSCR_PD_ARES_SHFT                                                                   0x3
#define HWIO_GCC_USB_30_GDSCR_PD_ARES_NO_RESET_FVAL                                                          0x0
#define HWIO_GCC_USB_30_GDSCR_PD_ARES_RESET_FVAL                                                             0x1
#define HWIO_GCC_USB_30_GDSCR_SW_OVERRIDE_BMSK                                                               0x4
#define HWIO_GCC_USB_30_GDSCR_SW_OVERRIDE_SHFT                                                               0x2
#define HWIO_GCC_USB_30_GDSCR_SW_OVERRIDE_DISABLE_FVAL                                                       0x0
#define HWIO_GCC_USB_30_GDSCR_SW_OVERRIDE_ENABLE_FVAL                                                        0x1
#define HWIO_GCC_USB_30_GDSCR_HW_CONTROL_BMSK                                                                0x2
#define HWIO_GCC_USB_30_GDSCR_HW_CONTROL_SHFT                                                                0x1
#define HWIO_GCC_USB_30_GDSCR_HW_CONTROL_DISABLE_FVAL                                                        0x0
#define HWIO_GCC_USB_30_GDSCR_HW_CONTROL_ENABLE_FVAL                                                         0x1
#define HWIO_GCC_USB_30_GDSCR_SW_COLLAPSE_BMSK                                                               0x1
#define HWIO_GCC_USB_30_GDSCR_SW_COLLAPSE_SHFT                                                               0x0
#define HWIO_GCC_USB_30_GDSCR_SW_COLLAPSE_DISABLE_FVAL                                                       0x0
#define HWIO_GCC_USB_30_GDSCR_SW_COLLAPSE_ENABLE_FVAL                                                        0x1

#define HWIO_GCC_PCIE_GDSCR_ADDR                                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e18)
#define HWIO_GCC_PCIE_GDSCR_PHYS                                                                      (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e18)
#define HWIO_GCC_PCIE_GDSCR_OFFS                                                                      (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e18)
#define HWIO_GCC_PCIE_GDSCR_RMSK                                                                      0xf8ffffff
#define HWIO_GCC_PCIE_GDSCR_POR                                                                       0x00222001
#define HWIO_GCC_PCIE_GDSCR_ATTR                                                                             0x3
#define HWIO_GCC_PCIE_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_GDSCR_ADDR, HWIO_GCC_PCIE_GDSCR_RMSK)
#define HWIO_GCC_PCIE_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_GDSCR_ADDR, m)
#define HWIO_GCC_PCIE_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_GDSCR_ADDR,v)
#define HWIO_GCC_PCIE_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_GDSCR_ADDR,m,v,HWIO_GCC_PCIE_GDSCR_IN)
#define HWIO_GCC_PCIE_GDSCR_PWR_ON_BMSK                                                               0x80000000
#define HWIO_GCC_PCIE_GDSCR_PWR_ON_SHFT                                                                     0x1f
#define HWIO_GCC_PCIE_GDSCR_GDSC_STATE_BMSK                                                           0x78000000
#define HWIO_GCC_PCIE_GDSCR_GDSC_STATE_SHFT                                                                 0x1b
#define HWIO_GCC_PCIE_GDSCR_EN_REST_WAIT_BMSK                                                           0xf00000
#define HWIO_GCC_PCIE_GDSCR_EN_REST_WAIT_SHFT                                                               0x14
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_WAIT_BMSK                                                             0xf0000
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_WAIT_SHFT                                                                0x10
#define HWIO_GCC_PCIE_GDSCR_CLK_DIS_WAIT_BMSK                                                             0xf000
#define HWIO_GCC_PCIE_GDSCR_CLK_DIS_WAIT_SHFT                                                                0xc
#define HWIO_GCC_PCIE_GDSCR_RETAIN_FF_ENABLE_BMSK                                                          0x800
#define HWIO_GCC_PCIE_GDSCR_RETAIN_FF_ENABLE_SHFT                                                            0xb
#define HWIO_GCC_PCIE_GDSCR_RETAIN_FF_ENABLE_DISABLE_FVAL                                                    0x0
#define HWIO_GCC_PCIE_GDSCR_RETAIN_FF_ENABLE_ENABLE_FVAL                                                     0x1
#define HWIO_GCC_PCIE_GDSCR_RESTORE_BMSK                                                                   0x400
#define HWIO_GCC_PCIE_GDSCR_RESTORE_SHFT                                                                     0xa
#define HWIO_GCC_PCIE_GDSCR_RESTORE_DISABLE_FVAL                                                             0x0
#define HWIO_GCC_PCIE_GDSCR_RESTORE_ENABLE_FVAL                                                              0x1
#define HWIO_GCC_PCIE_GDSCR_SAVE_BMSK                                                                      0x200
#define HWIO_GCC_PCIE_GDSCR_SAVE_SHFT                                                                        0x9
#define HWIO_GCC_PCIE_GDSCR_SAVE_DISABLE_FVAL                                                                0x0
#define HWIO_GCC_PCIE_GDSCR_SAVE_ENABLE_FVAL                                                                 0x1
#define HWIO_GCC_PCIE_GDSCR_RETAIN_BMSK                                                                    0x100
#define HWIO_GCC_PCIE_GDSCR_RETAIN_SHFT                                                                      0x8
#define HWIO_GCC_PCIE_GDSCR_RETAIN_DISABLE_FVAL                                                              0x0
#define HWIO_GCC_PCIE_GDSCR_RETAIN_ENABLE_FVAL                                                               0x1
#define HWIO_GCC_PCIE_GDSCR_EN_REST_BMSK                                                                    0x80
#define HWIO_GCC_PCIE_GDSCR_EN_REST_SHFT                                                                     0x7
#define HWIO_GCC_PCIE_GDSCR_EN_REST_DISABLE_FVAL                                                             0x0
#define HWIO_GCC_PCIE_GDSCR_EN_REST_ENABLE_FVAL                                                              0x1
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_BMSK                                                                     0x40
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_SHFT                                                                      0x6
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_DISABLE_FVAL                                                              0x0
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_ENABLE_FVAL                                                               0x1
#define HWIO_GCC_PCIE_GDSCR_CLAMP_IO_BMSK                                                                   0x20
#define HWIO_GCC_PCIE_GDSCR_CLAMP_IO_SHFT                                                                    0x5
#define HWIO_GCC_PCIE_GDSCR_CLAMP_IO_DISABLE_FVAL                                                            0x0
#define HWIO_GCC_PCIE_GDSCR_CLAMP_IO_ENABLE_FVAL                                                             0x1
#define HWIO_GCC_PCIE_GDSCR_CLK_DISABLE_BMSK                                                                0x10
#define HWIO_GCC_PCIE_GDSCR_CLK_DISABLE_SHFT                                                                 0x4
#define HWIO_GCC_PCIE_GDSCR_CLK_DISABLE_CLK_NOT_DISABLE_FVAL                                                 0x0
#define HWIO_GCC_PCIE_GDSCR_CLK_DISABLE_CLK_DISABLE_FVAL                                                     0x1
#define HWIO_GCC_PCIE_GDSCR_PD_ARES_BMSK                                                                     0x8
#define HWIO_GCC_PCIE_GDSCR_PD_ARES_SHFT                                                                     0x3
#define HWIO_GCC_PCIE_GDSCR_PD_ARES_NO_RESET_FVAL                                                            0x0
#define HWIO_GCC_PCIE_GDSCR_PD_ARES_RESET_FVAL                                                               0x1
#define HWIO_GCC_PCIE_GDSCR_SW_OVERRIDE_BMSK                                                                 0x4
#define HWIO_GCC_PCIE_GDSCR_SW_OVERRIDE_SHFT                                                                 0x2
#define HWIO_GCC_PCIE_GDSCR_SW_OVERRIDE_DISABLE_FVAL                                                         0x0
#define HWIO_GCC_PCIE_GDSCR_SW_OVERRIDE_ENABLE_FVAL                                                          0x1
#define HWIO_GCC_PCIE_GDSCR_HW_CONTROL_BMSK                                                                  0x2
#define HWIO_GCC_PCIE_GDSCR_HW_CONTROL_SHFT                                                                  0x1
#define HWIO_GCC_PCIE_GDSCR_HW_CONTROL_DISABLE_FVAL                                                          0x0
#define HWIO_GCC_PCIE_GDSCR_HW_CONTROL_ENABLE_FVAL                                                           0x1
#define HWIO_GCC_PCIE_GDSCR_SW_COLLAPSE_BMSK                                                                 0x1
#define HWIO_GCC_PCIE_GDSCR_SW_COLLAPSE_SHFT                                                                 0x0
#define HWIO_GCC_PCIE_GDSCR_SW_COLLAPSE_DISABLE_FVAL                                                         0x0
#define HWIO_GCC_PCIE_GDSCR_SW_COLLAPSE_ENABLE_FVAL                                                          0x1

#define HWIO_GCC_PCIEPHY_PHY_BCR_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e1c)
#define HWIO_GCC_PCIEPHY_PHY_BCR_PHYS                                                                 (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e1c)
#define HWIO_GCC_PCIEPHY_PHY_BCR_OFFS                                                                 (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e1c)
#define HWIO_GCC_PCIEPHY_PHY_BCR_RMSK                                                                        0x1
#define HWIO_GCC_PCIEPHY_PHY_BCR_POR                                                                  0x00000001
#define HWIO_GCC_PCIEPHY_PHY_BCR_ATTR                                                                        0x3
#define HWIO_GCC_PCIEPHY_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCIEPHY_PHY_BCR_ADDR, HWIO_GCC_PCIEPHY_PHY_BCR_RMSK)
#define HWIO_GCC_PCIEPHY_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIEPHY_PHY_BCR_ADDR, m)
#define HWIO_GCC_PCIEPHY_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIEPHY_PHY_BCR_ADDR,v)
#define HWIO_GCC_PCIEPHY_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIEPHY_PHY_BCR_ADDR,m,v,HWIO_GCC_PCIEPHY_PHY_BCR_IN)
#define HWIO_GCC_PCIEPHY_PHY_BCR_BLK_ARES_BMSK                                                               0x1
#define HWIO_GCC_PCIEPHY_PHY_BCR_BLK_ARES_SHFT                                                               0x0
#define HWIO_GCC_PCIEPHY_PHY_BCR_BLK_ARES_DISABLE_FVAL                                                       0x0
#define HWIO_GCC_PCIEPHY_PHY_BCR_BLK_ARES_ENABLE_FVAL                                                        0x1

#define HWIO_GCC_RBCPR_MX_BCR_ADDR                                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00001b00)
#define HWIO_GCC_RBCPR_MX_BCR_PHYS                                                                    (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001b00)
#define HWIO_GCC_RBCPR_MX_BCR_OFFS                                                                    (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001b00)
#define HWIO_GCC_RBCPR_MX_BCR_RMSK                                                                           0x1
#define HWIO_GCC_RBCPR_MX_BCR_POR                                                                     0x00000000
#define HWIO_GCC_RBCPR_MX_BCR_ATTR                                                                           0x3
#define HWIO_GCC_RBCPR_MX_BCR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_MX_BCR_ADDR, HWIO_GCC_RBCPR_MX_BCR_RMSK)
#define HWIO_GCC_RBCPR_MX_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_MX_BCR_ADDR, m)
#define HWIO_GCC_RBCPR_MX_BCR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_MX_BCR_ADDR,v)
#define HWIO_GCC_RBCPR_MX_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_MX_BCR_ADDR,m,v,HWIO_GCC_RBCPR_MX_BCR_IN)
#define HWIO_GCC_RBCPR_MX_BCR_BLK_ARES_BMSK                                                                  0x1
#define HWIO_GCC_RBCPR_MX_BCR_BLK_ARES_SHFT                                                                  0x0
#define HWIO_GCC_RBCPR_MX_BCR_BLK_ARES_DISABLE_FVAL                                                          0x0
#define HWIO_GCC_RBCPR_MX_BCR_BLK_ARES_ENABLE_FVAL                                                           0x1

#define HWIO_GCC_RBCPR_MX_CBCR_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00001b04)
#define HWIO_GCC_RBCPR_MX_CBCR_PHYS                                                                   (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001b04)
#define HWIO_GCC_RBCPR_MX_CBCR_OFFS                                                                   (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001b04)
#define HWIO_GCC_RBCPR_MX_CBCR_RMSK                                                                   0x80000001
#define HWIO_GCC_RBCPR_MX_CBCR_POR                                                                    0x80000000
#define HWIO_GCC_RBCPR_MX_CBCR_ATTR                                                                          0x3
#define HWIO_GCC_RBCPR_MX_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_MX_CBCR_ADDR, HWIO_GCC_RBCPR_MX_CBCR_RMSK)
#define HWIO_GCC_RBCPR_MX_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_MX_CBCR_ADDR, m)
#define HWIO_GCC_RBCPR_MX_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_MX_CBCR_ADDR,v)
#define HWIO_GCC_RBCPR_MX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_MX_CBCR_ADDR,m,v,HWIO_GCC_RBCPR_MX_CBCR_IN)
#define HWIO_GCC_RBCPR_MX_CBCR_CLK_OFF_BMSK                                                           0x80000000
#define HWIO_GCC_RBCPR_MX_CBCR_CLK_OFF_SHFT                                                                 0x1f
#define HWIO_GCC_RBCPR_MX_CBCR_CLK_ENABLE_BMSK                                                               0x1
#define HWIO_GCC_RBCPR_MX_CBCR_CLK_ENABLE_SHFT                                                               0x0
#define HWIO_GCC_RBCPR_MX_CBCR_CLK_ENABLE_DISABLE_FVAL                                                       0x0
#define HWIO_GCC_RBCPR_MX_CBCR_CLK_ENABLE_ENABLE_FVAL                                                        0x1

#define HWIO_GCC_RBCPR_MX_AHB_CBCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00001b08)
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001b08)
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001b08)
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_RMSK                                                               0xf0008001
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_POR                                                                0x80008000
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_ATTR                                                                      0x3
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_MX_AHB_CBCR_ADDR, HWIO_GCC_RBCPR_MX_AHB_CBCR_RMSK)
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_MX_AHB_CBCR_ADDR, m)
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_MX_AHB_CBCR_ADDR,v)
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_MX_AHB_CBCR_ADDR,m,v,HWIO_GCC_RBCPR_MX_AHB_CBCR_IN)
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                      0x70000000
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                            0x1c
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                              0x8000
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                 0xf
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_DISABLE_FVAL                                         0x0
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_ENABLE_FVAL                                          0x1
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_CLK_ENABLE_SHFT                                                           0x0
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_CLK_ENABLE_DISABLE_FVAL                                                   0x0
#define HWIO_GCC_RBCPR_MX_AHB_CBCR_CLK_ENABLE_ENABLE_FVAL                                                    0x1

#define HWIO_GCC_RBCPR_MX_CMD_RCGR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00001b0c)
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001b0c)
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001b0c)
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_RMSK                                                               0x80000013
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_POR                                                                0x80000000
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_ATTR                                                                      0x3
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_MX_CMD_RCGR_ADDR, HWIO_GCC_RBCPR_MX_CMD_RCGR_RMSK)
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_MX_CMD_RCGR_ADDR, m)
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_MX_CMD_RCGR_ADDR,v)
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_MX_CMD_RCGR_ADDR,m,v,HWIO_GCC_RBCPR_MX_CMD_RCGR_IN)
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_ROOT_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_ROOT_OFF_SHFT                                                            0x1f
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                      0x10
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                       0x4
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_ROOT_EN_BMSK                                                              0x2
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_ROOT_EN_SHFT                                                              0x1
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_ROOT_EN_DISABLE_FVAL                                                      0x0
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_ROOT_EN_ENABLE_FVAL                                                       0x1
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_UPDATE_BMSK                                                               0x1
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_UPDATE_SHFT                                                               0x0
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_UPDATE_DISABLE_FVAL                                                       0x0
#define HWIO_GCC_RBCPR_MX_CMD_RCGR_UPDATE_ENABLE_FVAL                                                        0x1

#define HWIO_GCC_RBCPR_MX_CFG_RCGR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00001b10)
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_PHYS                                                               (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001b10)
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_OFFS                                                               (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001b10)
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_RMSK                                                                    0x71f
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_POR                                                                0x00000000
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_ATTR                                                                      0x3
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_RBCPR_MX_CFG_RCGR_ADDR, HWIO_GCC_RBCPR_MX_CFG_RCGR_RMSK)
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_RBCPR_MX_CFG_RCGR_ADDR, m)
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_RBCPR_MX_CFG_RCGR_ADDR,v)
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RBCPR_MX_CFG_RCGR_ADDR,m,v,HWIO_GCC_RBCPR_MX_CFG_RCGR_IN)
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_BMSK                                                            0x700
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SHFT                                                              0x8
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SRC0_FVAL                                                         0x0
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SRC1_FVAL                                                         0x1
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SRC2_FVAL                                                         0x2
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SRC3_FVAL                                                         0x3
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SRC4_FVAL                                                         0x4
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SRC5_FVAL                                                         0x5
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SRC6_FVAL                                                         0x6
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_SEL_SRC7_FVAL                                                         0x7
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_BMSK                                                             0x1f
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_SHFT                                                              0x0
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_BYPASS_FVAL                                                       0x0
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV1_FVAL                                                         0x1
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV1_5_FVAL                                                       0x2
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV2_FVAL                                                         0x3
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV2_5_FVAL                                                       0x4
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV3_FVAL                                                         0x5
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV3_5_FVAL                                                       0x6
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV4_FVAL                                                         0x7
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV4_5_FVAL                                                       0x8
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV5_FVAL                                                         0x9
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV5_5_FVAL                                                       0xa
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV6_FVAL                                                         0xb
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV6_5_FVAL                                                       0xc
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV7_FVAL                                                         0xd
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV7_5_FVAL                                                       0xe
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV8_FVAL                                                         0xf
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV8_5_FVAL                                                      0x10
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV9_FVAL                                                        0x11
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV9_5_FVAL                                                      0x12
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV10_FVAL                                                       0x13
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV10_5_FVAL                                                     0x14
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV11_FVAL                                                       0x15
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV11_5_FVAL                                                     0x16
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV12_FVAL                                                       0x17
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV12_5_FVAL                                                     0x18
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV13_FVAL                                                       0x19
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV13_5_FVAL                                                     0x1a
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV14_FVAL                                                       0x1b
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV14_5_FVAL                                                     0x1c
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV15_FVAL                                                       0x1d
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV15_5_FVAL                                                     0x1e
#define HWIO_GCC_RBCPR_MX_CFG_RCGR_SRC_DIV_DIV16_FVAL                                                       0x1f

#define HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e40)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_PHYS                                                                (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e40)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_OFFS                                                                (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e40)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_RMSK                                                                       0x1
#define HWIO_GCC_PCIE_GPIO_LDO_EN_POR                                                                 0x00000001
#define HWIO_GCC_PCIE_GPIO_LDO_EN_ATTR                                                                       0x3
#define HWIO_GCC_PCIE_GPIO_LDO_EN_IN          \
        in_dword_masked(HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR, HWIO_GCC_PCIE_GPIO_LDO_EN_RMSK)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR, m)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR,v)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR,m,v,HWIO_GCC_PCIE_GPIO_LDO_EN_IN)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_LDO_ENABLE_BMSK                                                            0x1
#define HWIO_GCC_PCIE_GPIO_LDO_EN_LDO_ENABLE_SHFT                                                            0x0
#define HWIO_GCC_PCIE_GPIO_LDO_EN_LDO_ENABLE_DISABLE_FVAL                                                    0x0
#define HWIO_GCC_PCIE_GPIO_LDO_EN_LDO_ENABLE_ENABLE_FVAL                                                     0x1

#define HWIO_GCC_USB_SS_LDO_EN_ADDR                                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00001e44)
#define HWIO_GCC_USB_SS_LDO_EN_PHYS                                                                   (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001e44)
#define HWIO_GCC_USB_SS_LDO_EN_OFFS                                                                   (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001e44)
#define HWIO_GCC_USB_SS_LDO_EN_RMSK                                                                          0x1
#define HWIO_GCC_USB_SS_LDO_EN_POR                                                                    0x00000001
#define HWIO_GCC_USB_SS_LDO_EN_ATTR                                                                          0x3
#define HWIO_GCC_USB_SS_LDO_EN_IN          \
        in_dword_masked(HWIO_GCC_USB_SS_LDO_EN_ADDR, HWIO_GCC_USB_SS_LDO_EN_RMSK)
#define HWIO_GCC_USB_SS_LDO_EN_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_SS_LDO_EN_ADDR, m)
#define HWIO_GCC_USB_SS_LDO_EN_OUT(v)      \
        out_dword(HWIO_GCC_USB_SS_LDO_EN_ADDR,v)
#define HWIO_GCC_USB_SS_LDO_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_SS_LDO_EN_ADDR,m,v,HWIO_GCC_USB_SS_LDO_EN_IN)
#define HWIO_GCC_USB_SS_LDO_EN_LDO_ENABLE_BMSK                                                               0x1
#define HWIO_GCC_USB_SS_LDO_EN_LDO_ENABLE_SHFT                                                               0x0
#define HWIO_GCC_USB_SS_LDO_EN_LDO_ENABLE_DISABLE_FVAL                                                       0x0
#define HWIO_GCC_USB_SS_LDO_EN_LDO_ENABLE_ENABLE_FVAL                                                        0x1

#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00001b84)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_PHYS                                                            (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001b84)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_OFFS                                                            (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001b84)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_RMSK                                                            0xf0008001
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_POR                                                             0x80008000
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ATTR                                                                   0x3
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR, HWIO_GCC_USB_PHY_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_USB_PHY_CFG_AHB_CBCR_IN)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                   0x70000000
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                         0x1c
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                           0x8000
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                              0xf
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_DISABLE_FVAL                                      0x0
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_ENABLE_FVAL                                       0x1
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                        0x0
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_ENABLE_DISABLE_FVAL                                                0x0
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_ENABLE_ENABLE_FVAL                                                 0x1

#define HWIO_GCC_SNOC_DCD_CONFIG_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00001ac8)
#define HWIO_GCC_SNOC_DCD_CONFIG_PHYS                                                                 (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001ac8)
#define HWIO_GCC_SNOC_DCD_CONFIG_OFFS                                                                 (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001ac8)
#define HWIO_GCC_SNOC_DCD_CONFIG_RMSK                                                                 0x80007fff
#define HWIO_GCC_SNOC_DCD_CONFIG_POR                                                                  0x00004045
#define HWIO_GCC_SNOC_DCD_CONFIG_ATTR                                                                        0x3
#define HWIO_GCC_SNOC_DCD_CONFIG_IN          \
        in_dword_masked(HWIO_GCC_SNOC_DCD_CONFIG_ADDR, HWIO_GCC_SNOC_DCD_CONFIG_RMSK)
#define HWIO_GCC_SNOC_DCD_CONFIG_INM(m)      \
        in_dword_masked(HWIO_GCC_SNOC_DCD_CONFIG_ADDR, m)
#define HWIO_GCC_SNOC_DCD_CONFIG_OUT(v)      \
        out_dword(HWIO_GCC_SNOC_DCD_CONFIG_ADDR,v)
#define HWIO_GCC_SNOC_DCD_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SNOC_DCD_CONFIG_ADDR,m,v,HWIO_GCC_SNOC_DCD_CONFIG_IN)
#define HWIO_GCC_SNOC_DCD_CONFIG_DCD_ENABLE_BMSK                                                      0x80000000
#define HWIO_GCC_SNOC_DCD_CONFIG_DCD_ENABLE_SHFT                                                            0x1f
#define HWIO_GCC_SNOC_DCD_CONFIG_ALLOWED_DIV_BMSK                                                         0x7fff
#define HWIO_GCC_SNOC_DCD_CONFIG_ALLOWED_DIV_SHFT                                                            0x0

#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00001acc)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_PHYS                                                         (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001acc)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_OFFS                                                         (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001acc)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_RMSK                                                         0xffffffff
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_POR                                                          0x00100080
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ATTR                                                                0x3
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_IN          \
        in_dword_masked(HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR, HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_RMSK)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_INM(m)      \
        in_dword_masked(HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR, m)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_OUT(v)      \
        out_dword(HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR,v)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_ADDR,m,v,HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_IN)
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_BMSK                                               0xfffff000
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_SHFT                                                      0xc
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_BMSK                                                     0xfff
#define HWIO_GCC_SNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_SHFT                                                       0x0

#define HWIO_GCC_CNOC_DCD_CONFIG_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00001ad0)
#define HWIO_GCC_CNOC_DCD_CONFIG_PHYS                                                                 (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001ad0)
#define HWIO_GCC_CNOC_DCD_CONFIG_OFFS                                                                 (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001ad0)
#define HWIO_GCC_CNOC_DCD_CONFIG_RMSK                                                                 0x80007fff
#define HWIO_GCC_CNOC_DCD_CONFIG_POR                                                                  0x00004045
#define HWIO_GCC_CNOC_DCD_CONFIG_ATTR                                                                        0x3
#define HWIO_GCC_CNOC_DCD_CONFIG_IN          \
        in_dword_masked(HWIO_GCC_CNOC_DCD_CONFIG_ADDR, HWIO_GCC_CNOC_DCD_CONFIG_RMSK)
#define HWIO_GCC_CNOC_DCD_CONFIG_INM(m)      \
        in_dword_masked(HWIO_GCC_CNOC_DCD_CONFIG_ADDR, m)
#define HWIO_GCC_CNOC_DCD_CONFIG_OUT(v)      \
        out_dword(HWIO_GCC_CNOC_DCD_CONFIG_ADDR,v)
#define HWIO_GCC_CNOC_DCD_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CNOC_DCD_CONFIG_ADDR,m,v,HWIO_GCC_CNOC_DCD_CONFIG_IN)
#define HWIO_GCC_CNOC_DCD_CONFIG_DCD_ENABLE_BMSK                                                      0x80000000
#define HWIO_GCC_CNOC_DCD_CONFIG_DCD_ENABLE_SHFT                                                            0x1f
#define HWIO_GCC_CNOC_DCD_CONFIG_ALLOWED_DIV_BMSK                                                         0x7fff
#define HWIO_GCC_CNOC_DCD_CONFIG_ALLOWED_DIV_SHFT                                                            0x0

#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00001ad4)
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_PHYS                                                         (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001ad4)
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_OFFS                                                         (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001ad4)
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_RMSK                                                         0xffffffff
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_POR                                                          0x00100080
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_ATTR                                                                0x3
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_IN          \
        in_dword_masked(HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_ADDR, HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_RMSK)
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_INM(m)      \
        in_dword_masked(HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_ADDR, m)
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_OUT(v)      \
        out_dword(HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_ADDR,v)
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_ADDR,m,v,HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_IN)
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_BMSK                                               0xfffff000
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_SHFT                                                      0xc
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_BMSK                                                     0xfff
#define HWIO_GCC_CNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_SHFT                                                       0x0

#define HWIO_GCC_PNOC_DCD_CONFIG_ADDR                                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00001ad8)
#define HWIO_GCC_PNOC_DCD_CONFIG_PHYS                                                                 (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001ad8)
#define HWIO_GCC_PNOC_DCD_CONFIG_OFFS                                                                 (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001ad8)
#define HWIO_GCC_PNOC_DCD_CONFIG_RMSK                                                                 0x80007fff
#define HWIO_GCC_PNOC_DCD_CONFIG_POR                                                                  0x00004045
#define HWIO_GCC_PNOC_DCD_CONFIG_ATTR                                                                        0x3
#define HWIO_GCC_PNOC_DCD_CONFIG_IN          \
        in_dword_masked(HWIO_GCC_PNOC_DCD_CONFIG_ADDR, HWIO_GCC_PNOC_DCD_CONFIG_RMSK)
#define HWIO_GCC_PNOC_DCD_CONFIG_INM(m)      \
        in_dword_masked(HWIO_GCC_PNOC_DCD_CONFIG_ADDR, m)
#define HWIO_GCC_PNOC_DCD_CONFIG_OUT(v)      \
        out_dword(HWIO_GCC_PNOC_DCD_CONFIG_ADDR,v)
#define HWIO_GCC_PNOC_DCD_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PNOC_DCD_CONFIG_ADDR,m,v,HWIO_GCC_PNOC_DCD_CONFIG_IN)
#define HWIO_GCC_PNOC_DCD_CONFIG_DCD_ENABLE_BMSK                                                      0x80000000
#define HWIO_GCC_PNOC_DCD_CONFIG_DCD_ENABLE_SHFT                                                            0x1f
#define HWIO_GCC_PNOC_DCD_CONFIG_ALLOWED_DIV_BMSK                                                         0x7fff
#define HWIO_GCC_PNOC_DCD_CONFIG_ALLOWED_DIV_SHFT                                                            0x0

#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00001adc)
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_PHYS                                                         (GCC_CLK_CTL_REG_REG_BASE_PHYS + 0x00001adc)
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_OFFS                                                         (GCC_CLK_CTL_REG_REG_BASE_OFFS + 0x00001adc)
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_RMSK                                                         0xffffffff
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_POR                                                          0x00100080
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_ATTR                                                                0x3
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_IN          \
        in_dword_masked(HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_ADDR, HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_RMSK)
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_INM(m)      \
        in_dword_masked(HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_ADDR, m)
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_OUT(v)      \
        out_dword(HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_ADDR,v)
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_ADDR,m,v,HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_IN)
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_BMSK                                               0xfffff000
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_FIRST_CNT_SHFT                                                      0xc
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_BMSK                                                     0xfff
#define HWIO_GCC_PNOC_DCD_HYSTERESIS_CNT_NEXT_CNT_SHFT                                                       0x0


#endif /* __GCC_CLK_CTL__ */

