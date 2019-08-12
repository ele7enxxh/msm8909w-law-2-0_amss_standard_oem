#ifndef __HALCLKGENERICPLL_H__
#define __HALCLKGENERICPLL_H__
/*
==============================================================================

FILE:         HALclkGenericPLL.h

DESCRIPTION:
  Internal, chipset specific PLL definitions for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/v1/HALclkGenericPLL.h#1 $

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
void    HAL_clk_GenericPLLEnable           ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_GenericPLLDisable          ( HAL_clk_SourceDescType *pSource );
boolean HAL_clk_GenericPLLIsEnabled        ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_GenericPLLConfig           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
void    HAL_clk_GenericPLLConfigPLL        ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig );
void    HAL_clk_GenericPLLDetectPLLConfig  ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
void    HAL_clk_GenericPLLEnableVote       ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_GenericPLLDisableVote      ( HAL_clk_SourceDescType *pSource );
boolean HAL_clk_GenericPLLIsVoteEnabled    ( HAL_clk_SourceDescType *pSource );
void    HAL_clk_GenericPLLConfigLMN        ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pConfig );

/*
 * Install a generic PLL handler.
 */
void    HAL_clk_InstallPLL                 ( HAL_clk_SourceType eSource, HAL_clk_PLLContextType *pCtxt, uint32 nBaseAddress);


#endif  /* __HALCLKGENERICPLL_H__ */

