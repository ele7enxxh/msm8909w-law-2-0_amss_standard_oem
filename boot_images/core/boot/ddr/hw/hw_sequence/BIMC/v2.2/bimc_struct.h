/**
 * @file bimc_struct.h
 * @brief definitions for BIMC
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
09/01/13   cchou      Initial version.
================================================================================
                   Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                                             DEFINITIONS
==============================================================================*/

#ifndef _BIMC_STRUCT_SEQ_HEADER_
#define _BIMC_STRUCT_SEQ_HEADER_
#include "ddrss_seq_hwiobase.h"
#include "ddrss_seq_hwioreg.h"
#define SDRAM_CHANNEL_CH0                                            0
#define SDRAM_CHANNEL_CH1                                            1
#define SDRAM_RANK_NONE                                              0
#define SDRAM_RANK_CS0                                               1
#define SDRAM_RANK_CS1                                               2
#define SDRAM_RANK_BOTH                                              3
#define XO_SPEED_IN_KHZ                                              19200
#define SDRAM_LPDDR2                                                 2
#define SDRAM_LPDDR3                                                 5
#define SDRAM_PCDDR3                                                 4
#define EN_DDR_ODT                                                   1
#define SCALE_BOOT_PRE_SILICON                                       1
#define SR_IDLE_TIMER                                                50
#define SCALE_RANK1_ODT_ON_WR_RANK1                                  64
#define SCALE_RANK1_ODT_ON_WR_RANK0                                  32
#define SCALE_RANK1_ODT_ON_RD_RANK0                                  16
#define SCALE_RANK0_ODT_ON_WR_RANK1                                  4
#define SCALE_RANK0_ODT_ON_WR_RANK0                                  2
#define SCALE_RANK0_ODT_ON_RD_RANK1                                  1
/// for DOE non-AC parameters
#define RANK1_ODT_ON_WR_RANK1_SHFT                                   6
#define RANK1_ODT_ON_WR_RANK0_SHFT                                   5
#define RANK1_ODT_ON_RD_RANK0_SHFT                                   4
#define RANK0_ODT_ON_WR_RANK1_SHFT                                   2
#define RANK0_ODT_ON_WR_RANK0_SHFT                                   1
#define RANK0_ODT_ON_RD_RANK1_SHFT                                   0
#define ODT_OFF                                                      0x0
#define PC_SINGLE_RANK_WRITE_TARGET_READ_OFF                         0x2
#define DUAL_RANK_WRITE_TARGET_READ_OFF                              0x22
#define DUAL_RANK_WRITE_TARGET_READ_ON                               0x2B
#define DUAL_RANK_WRITE_INACTIVE_READ_OFF                            0x14
#define DUAL_RANK_WRITE_INACTIVE_READ_ON                             0x1D
#define PC_DUAL_RANK_WRITE_BOTH_READ_OFF                             0x36
#define PC_DUAL_RANK_WRITE_BOTH_READ_ON                              0x3F
#define LP_SINGLE_RANK_WRITE_ON_READ_OFF                             0x2
#define LP_DUAL_RANK_WRITE_ON_READ_OFF                               0x6
/// for misc_global
#define CLK_PERIOD_RESOLUTION_100PS                                  0
#define CLK_PERIOD_RESOLUTION_1PS                                    1



#endif
