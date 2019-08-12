#ifndef __HALCLKSRPLL_H__
#define __HALCLKSRPLL_H__
/*
==============================================================================

FILE:         HALclkSRPLL.h

DESCRIPTION:
  Internal, chipset specific PLL definitions for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/v1/HALclkSRPLL.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
07/11/11   vs      Created.

==============================================================================
            Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALclkInternal.h"



/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * PLL register offsets.
 */
#define HWIO_PLL_MODE_OFFS                                  0x00000000
#define HWIO_PLL_L_VAL_OFFS                                 0x00000004
#define HWIO_PLL_M_VAL_OFFS                                 0x00000008
#define HWIO_PLL_N_VAL_OFFS                                 0x0000000c
#define HWIO_PLL_USER_CTL_OFFS                              0x00000010
#define HWIO_PLL_CONFIG_CTL_OFFS                            0x00000014
#define HWIO_PLL_TEST_CTL_OFFS                              0x00000018
#define HWIO_PLL_STATUS_OFFS                                0x0000001c

/*
 * PLL_MODE register definitions.
 */
#define HWIO_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                 0x200000
#define HWIO_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                     0x15
#define HWIO_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                   0x100000
#define HWIO_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                       0x14
#define HWIO_PLL_MODE_PLL_BIAS_COUNT_BMSK                      0xfc000
#define HWIO_PLL_MODE_PLL_BIAS_COUNT_SHFT                          0xe
#define HWIO_PLL_MODE_PLL_LOCK_COUNT_BMSK                       0x3f00
#define HWIO_PLL_MODE_PLL_LOCK_COUNT_SHFT                          0x8
#define HWIO_PLL_MODE_PLL_PLLTEST_BMSK                             0x8
#define HWIO_PLL_MODE_PLL_PLLTEST_SHFT                             0x3
#define HWIO_PLL_MODE_PLL_RESET_N_BMSK                             0x4
#define HWIO_PLL_MODE_PLL_RESET_N_SHFT                             0x2
#define HWIO_PLL_MODE_PLL_BYPASSNL_BMSK                            0x2
#define HWIO_PLL_MODE_PLL_BYPASSNL_SHFT                            0x1
#define HWIO_PLL_MODE_PLL_OUTCTRL_BMSK                             0x1
#define HWIO_PLL_MODE_PLL_OUTCTRL_SHFT                             0x0

/*
 * PLL_USER_CTL register definitions.
 */
#define HWIO_PLL_USER_CTL_ADDITIVE_FACTOR_BMSK              0xc0000000
#define HWIO_PLL_USER_CTL_ADDITIVE_FACTOR_SHFT                    0x1e
#define HWIO_PLL_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_BMSK  0x30000000
#define HWIO_PLL_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_SHFT        0x1c
#define HWIO_PLL_USER_CTL_MN_EN_BMSK                         0x1000000
#define HWIO_PLL_USER_CTL_MN_EN_SHFT                              0x18
#define HWIO_PLL_USER_CTL_VCO_SEL_BMSK                        0x300000
#define HWIO_PLL_USER_CTL_VCO_SEL_SHFT                            0x14
#define HWIO_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                    0x7000
#define HWIO_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                       0xc
#define HWIO_PLL_USER_CTL_POST_DIV_RATIO_BMSK                    0x300
#define HWIO_PLL_USER_CTL_POST_DIV_RATIO_SHFT                      0x8
#define HWIO_PLL_USER_CTL_OUTPUT_INV_BMSK                         0x80
#define HWIO_PLL_USER_CTL_OUTPUT_INV_SHFT                          0x7
#define HWIO_PLL_USER_CTL_PLLOUT_DIFF_90_BMSK                     0x40
#define HWIO_PLL_USER_CTL_PLLOUT_DIFF_90_SHFT                      0x6
#define HWIO_PLL_USER_CTL_PLLOUT_DIFF_0_BMSK                      0x20
#define HWIO_PLL_USER_CTL_PLLOUT_DIFF_0_SHFT                       0x5
#define HWIO_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                     0x10
#define HWIO_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                      0x4
#define HWIO_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                     0x8
#define HWIO_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                     0x3
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                      0x4
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                      0x2
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                       0x2
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                       0x1
#define HWIO_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                      0x1
#define HWIO_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                      0x0


/*
 * PLL_STATUS register definitions.
 */
#define HWIO_PLL_STATUS_PLL_ACTIVE_FLAG_BMSK                   0x20000
#define HWIO_PLL_STATUS_PLL_ACTIVE_FLAG_SHFT                      0x11
#define HWIO_PLL_STATUS_PLL_LOCK_DET_BMSK                      0x10000
#define HWIO_PLL_STATUS_PLL_LOCK_DET_SHFT                         0x10
#define HWIO_PLL_STATUS_PLL_D_BMSK                              0xffff
#define HWIO_PLL_STATUS_PLL_D_SHFT                                 0x0


/* ============================================================================
**    External Definitions
** ==========================================================================*/


/* ============================================================================
**    External Definitions
** ==========================================================================*/


/* ============================================================================
**    Functions
** ==========================================================================*/

/*
 * Source control functions.  These can be used if some of the control
 * needs to be overridden by a specific PLL.
 */
static boolean HAL_clk_SRPLLEnable           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
static void    HAL_clk_SRPLLDisable          ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
static boolean HAL_clk_SRPLLIsEnabled        ( HAL_clk_SourceDescType *pSource );
static void    HAL_clk_SRPLLConfig           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
static boolean HAL_clk_SRPLLConfigPLL        ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig, HAL_clk_SourceConfigMode eMode );
static void    HAL_clk_SRPLLDetectPLLConfig  ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
static boolean HAL_clk_SRPLLEnableVote       ( HAL_clk_SourceDescType *pSource );
static void    HAL_clk_SRPLLDisableVote      ( HAL_clk_SourceDescType *pSource );
static boolean HAL_clk_SRPLLIsVoteEnabled    ( HAL_clk_SourceDescType *pSource );


#endif  /* __HALCLKSRPLL_H__ */

