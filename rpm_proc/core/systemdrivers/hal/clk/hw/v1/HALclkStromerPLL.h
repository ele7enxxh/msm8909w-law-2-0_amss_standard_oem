#ifndef __HALCLKSTROMERPLL_H__
#define __HALCLKSTROMERPLL_H__
/*
==============================================================================

FILE:         HALclkGenericPLL.h

DESCRIPTION:
  Internal, chipset specific PLL definitions for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/v1/HALclkStromerPLL.h#1 $

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
#define HWIO_PLL_ALPHA_VAL_OFFS                             0x00000008
#define HWIO_PLL_ALPHA_U_VAL_OFFS                           0x0000000c
#define HWIO_PLL_USER_CTL_OFFS                              0x00000010
#define HWIO_PLL_USER_CTL_U_OFFS                            0x00000014
#define HWIO_PLL_CONFIG_CTL_OFFS                            0x00000018
#define HWIO_PLL_TEST_CTL_OFFS                              0x0000001c
#define HWIO_PLL_TEST_CTL_U_OFFS                            0x00000020
#define HWIO_PLL_STATUS_OFFS                                0x00000024

/*
 * PLL_MODE register definitions.
 */
#define HWIO_PLL_MODE_PLL_LOCK_DET_BMSK                     0x80000000
#define HWIO_PLL_MODE_PLL_LOCK_DET_SHFT                           0x1f
#define HWIO_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                  0x40000000
#define HWIO_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                        0x1e
#define HWIO_PLL_MODE_PLL_ACK_LATCH_BMSK                    0x20000000
#define HWIO_PLL_MODE_PLL_ACK_LATCH_SHFT                          0x1d
#define HWIO_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK         0x800000
#define HWIO_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT             0x17
#define HWIO_PLL_MODE_PLL_UPDATE_BMSK                         0x400000
#define HWIO_PLL_MODE_PLL_UPDATE_SHFT                             0x16
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
#define HWIO_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK              0x8000000
#define HWIO_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                   0x1b
#define HWIO_PLL_USER_CTL_ALPHA_EN_BMSK                      0x1000000
#define HWIO_PLL_USER_CTL_ALPHA_EN_SHFT                           0x18
#define HWIO_PLL_USER_CTL_VCO_SEL_BMSK                        0x300000
#define HWIO_PLL_USER_CTL_VCO_SEL_SHFT                            0x14
#define HWIO_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                    0x7000
#define HWIO_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                       0xc
#define HWIO_PLL_USER_CTL_POST_DIV_RATIO_BMSK                    0x300
#define HWIO_PLL_USER_CTL_POST_DIV_RATIO_SHFT                      0x8
#define HWIO_PLL_USER_CTL_OUTPUT_INV_BMSK                         0x80
#define HWIO_PLL_USER_CTL_OUTPUT_INV_SHFT                          0x7
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
 * PLL_USER_CTL_U register definitions.
 */
#define HWIO_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK          0x800
#define HWIO_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT            0xb
#define HWIO_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                 0x700
#define HWIO_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                   0x8
#define HWIO_PLL_USER_CTL_U_DSM_BMSK                              0x80
#define HWIO_PLL_USER_CTL_U_DSM_SHFT                               0x7
#define HWIO_PLL_USER_CTL_U_WRITE_STATE_BMSK                      0x40
#define HWIO_PLL_USER_CTL_U_WRITE_STATE_SHFT                       0x6
#define HWIO_PLL_USER_CTL_U_TARGET_CTL_BMSK                       0x38
#define HWIO_PLL_USER_CTL_U_TARGET_CTL_SHFT                        0x3
#define HWIO_PLL_USER_CTL_U_LOCK_DET_BMSK                          0x4
#define HWIO_PLL_USER_CTL_U_LOCK_DET_SHFT                          0x2
#define HWIO_PLL_USER_CTL_U_FREEZE_PLL_BMSK                        0x2
#define HWIO_PLL_USER_CTL_U_FREEZE_PLL_SHFT                        0x1
#define HWIO_PLL_USER_CTL_U_TOGGLE_DET_BMSK                        0x1
#define HWIO_PLL_USER_CTL_U_TOGGLE_DET_SHFT                        0x0


/* ============================================================================
**    External Definitions
** ==========================================================================*/

#define HAL_CLK_PLL_HWIO_ADDR(reg, ctxt)  (ctxt->nAddr + HWIO_OFFS(reg))

#define HAL_CLK_PLL_HWIO_IN(reg, ctxt)       \
  inpdw(HAL_CLK_PLL_HWIO_ADDR(reg, ctxt))

#define HAL_CLK_PLL_HWIO_INF(reg, ctxt, fld) \
  ((inpdw(HAL_CLK_PLL_HWIO_ADDR(reg, ctxt)) & HWIO_FMSK(reg, fld)) >> HWIO_SHFT(reg, fld))

#define HAL_CLK_PLL_HWIO_OUT(reg, ctxt, val) \
  outpdw(HAL_CLK_PLL_HWIO_ADDR(reg, ctxt), val)

#define HAL_CLK_PLL_HWIO_OUTF(reg, ctxt, fld, val)           \
  outpdw(HAL_CLK_PLL_HWIO_ADDR(reg, ctxt),                   \
   (HAL_CLK_PLL_HWIO_IN(reg, ctxt) & ~HWIO_FMSK(reg, fld))   \
   | (((val) << HWIO_SHFT(reg, fld)) & HWIO_FMSK(reg, fld)))


/* ============================================================================
**    External Definitions
** ==========================================================================*/

extern HAL_clk_SourceControlType       HAL_clk_mGenericPLLControl;


/* ============================================================================
**    Functions
** ==========================================================================*/

/*
 * Source control functions.  These can be used if some of the control
 * needs to be overriden by a specific PLL.
 */
void    HAL_clk_StromerPLLEnable           ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_StromerPLLDisable          ( HAL_clk_SourceDescType *pSource );
boolean HAL_clk_StromerPLLIsEnabled        ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_StromerPLLConfig           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
void    HAL_clk_StromerPLLConfigPLL        ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig);
void    HAL_clk_StromerPLLDetectPLLConfig  ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
void    HAL_clk_StromerPLLEnableVote       ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_StromerPLLDisableVote      ( HAL_clk_SourceDescType *pSource );
boolean HAL_clk_StromerPLLIsVoteEnabled    ( HAL_clk_SourceDescType *pSource );




#endif  /* __HALCLKSTROMERPLL_H__*/

