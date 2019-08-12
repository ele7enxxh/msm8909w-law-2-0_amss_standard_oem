/*
===========================================================================
  @file ClockSBLCommon.c

  This file provides common clock switch code and PLL setup.
===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  =========================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/clock/src/v3/ClockSBLCommon.c#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  =========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockSBLConfig.h"
#include "ClockHWIO.h"
#include "busywait.h"
#include "ClockSBLCommon.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

/*stromer */

/*
 * PLL register offsets.
 */
#define HWIO_GCC_STROMER_PLL_MODE_OFFS                                  0x00000000
#define HWIO_GCC_STROMER_PLL_L_VAL_OFFS                                 0x00000004
#define HWIO_GCC_STROMER_PLL_ALPHA_VAL_OFFS                             0x00000008
#define HWIO_GCC_STROMER_PLL_ALPHA_VAL_U_OFFS                           0x0000000c
#define HWIO_GCC_STROMER_PLL_USER_CTL_OFFS                              0x00000010
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_OFFS                            0x00000014
#define HWIO_GCC_STROMER_PLL_CONFIG_CTL_OFFS                            0x00000018
#define HWIO_GCC_STROMER_PLL_TEST_CTL_OFFS                              0x0000001c
#define HWIO_GCC_STROMER_PLL_TEST_CTL_U_OFFS                            0x00000020
#define HWIO_GCC_STROMER_PLL_STATUS_OFFS                                0x00000024

/*
 * PLL_MODE register definitions.
 */
#define HWIO_GCC_STROMER_PLL_MODE_PLL_LOCK_DET_BMSK                     0x80000000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_LOCK_DET_SHFT                           0x1f
#define HWIO_GCC_STROMER_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                  0x40000000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                        0x1e
#define HWIO_GCC_STROMER_PLL_MODE_PLL_ACK_LATCH_BMSK                    0x20000000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_ACK_LATCH_SHFT                          0x1d
#define HWIO_GCC_STROMER_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK         0x800000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT             0x17
#define HWIO_GCC_STROMER_PLL_MODE_PLL_UPDATE_BMSK                         0x400000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_UPDATE_SHFT                             0x16
#define HWIO_GCC_STROMER_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                 0x200000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                     0x15
#define HWIO_GCC_STROMER_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                   0x100000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                       0x14
#define HWIO_GCC_STROMER_PLL_MODE_PLL_BIAS_COUNT_BMSK                      0xfc000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_BIAS_COUNT_SHFT                          0xe
#define HWIO_GCC_STROMER_PLL_MODE_PLL_LOCK_COUNT_BMSK                       0x3f00
#define HWIO_GCC_STROMER_PLL_MODE_PLL_LOCK_COUNT_SHFT                          0x8
#define HWIO_GCC_STROMER_PLL_MODE_PLL_PLLTEST_BMSK                             0x8
#define HWIO_GCC_STROMER_PLL_MODE_PLL_PLLTEST_SHFT                             0x3
#define HWIO_GCC_STROMER_PLL_MODE_PLL_RESET_N_BMSK                             0x4
#define HWIO_GCC_STROMER_PLL_MODE_PLL_RESET_N_SHFT                             0x2
#define HWIO_GCC_STROMER_PLL_MODE_PLL_BYPASSNL_BMSK                            0x2
#define HWIO_GCC_STROMER_PLL_MODE_PLL_BYPASSNL_SHFT                            0x1
#define HWIO_GCC_STROMER_PLL_MODE_PLL_OUTCTRL_BMSK                             0x1
#define HWIO_GCC_STROMER_PLL_MODE_PLL_OUTCTRL_SHFT                             0x0

/*
 * PLL_USER_CTL register definitions.
 */
#define HWIO_GCC_STROMER_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK              0x8000000
#define HWIO_GCC_STROMER_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                   0x1b
#define HWIO_GCC_STROMER_PLL_USER_CTL_ALPHA_EN_BMSK                      0x1000000
#define HWIO_GCC_STROMER_PLL_USER_CTL_ALPHA_EN_SHFT                           0x18
#define HWIO_GCC_STROMER_PLL_USER_CTL_VCO_SEL_BMSK                        0x300000
#define HWIO_GCC_STROMER_PLL_USER_CTL_VCO_SEL_SHFT                            0x14
#define HWIO_GCC_STROMER_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                    0x7000
#define HWIO_GCC_STROMER_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                       0xc
#define HWIO_GCC_STROMER_PLL_USER_CTL_POST_DIV_RATIO_BMSK                    0x300
#define HWIO_GCC_STROMER_PLL_USER_CTL_POST_DIV_RATIO_SHFT                      0x8
#define HWIO_GCC_STROMER_PLL_USER_CTL_OUTPUT_INV_BMSK                         0x80
#define HWIO_GCC_STROMER_PLL_USER_CTL_OUTPUT_INV_SHFT                          0x7
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                     0x10
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                      0x4
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                     0x8
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                     0x3
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                      0x4
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                      0x2
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                       0x2
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                       0x1
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                      0x1
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                      0x0


/*
 * PLL_USER_CTL_U register definitions.
 */
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK          0x800
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT            0xb
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                 0x700
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                   0x8
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_DSM_BMSK                              0x80
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_DSM_SHFT                               0x7
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_WRITE_STATE_BMSK                      0x40
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_WRITE_STATE_SHFT                       0x6
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_TARGET_CTL_BMSK                       0x38
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_TARGET_CTL_SHFT                        0x3
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_LOCK_DET_BMSK                          0x4
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_LOCK_DET_SHFT                          0x2
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_FREEZE_PLL_BMSK                        0x2
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_FREEZE_PLL_SHFT                        0x1
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_TOGGLE_DET_BMSK                        0x1
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_TOGGLE_DET_SHFT                        0x0




/*SR PLL */


/*
 * PLL register offsets.
 */
#define HWIO_GCC_SR_PLL_MODE_OFFS                                  0x00000000
#define HWIO_GCC_SR_PLL_L_VAL_OFFS                                 0x00000004
#define HWIO_GCC_SR_PLL_M_VAL_OFFS                                 0x00000008
#define HWIO_GCC_SR_PLL_N_VAL_OFFS                                 0x0000000c
#define HWIO_GCC_SR_PLL_USER_CTL_OFFS                              0x00000010
#define HWIO_GCC_SR_PLL_CONFIG_CTL_OFFS                            0x00000014
#define HWIO_GCC_SR_PLL_TEST_CTL_OFFS                              0x00000018
#define HWIO_GCC_SR_PLL_STATUS_OFFS                                0x0000001c

/*
 * PLL_MODE register definitions.
 */
#define HWIO_GCC_SR_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                 0x200000
#define HWIO_GCC_SR_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                     0x15
#define HWIO_GCC_SR_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                   0x100000
#define HWIO_GCC_SR_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                       0x14
#define HWIO_GCC_SR_PLL_MODE_PLL_BIAS_COUNT_BMSK                      0xfc000
#define HWIO_GCC_SR_PLL_MODE_PLL_BIAS_COUNT_SHFT                          0xe
#define HWIO_GCC_SR_PLL_MODE_PLL_LOCK_COUNT_BMSK                       0x3f00
#define HWIO_GCC_SR_PLL_MODE_PLL_LOCK_COUNT_SHFT                          0x8
#define HWIO_GCC_SR_PLL_MODE_PLL_PLLTEST_BMSK                             0x8
#define HWIO_GCC_SR_PLL_MODE_PLL_PLLTEST_SHFT                             0x3
#define HWIO_GCC_SR_PLL_MODE_PLL_RESET_N_BMSK                             0x4
#define HWIO_GCC_SR_PLL_MODE_PLL_RESET_N_SHFT                             0x2
#define HWIO_GCC_SR_PLL_MODE_PLL_BYPASSNL_BMSK                            0x2
#define HWIO_GCC_SR_PLL_MODE_PLL_BYPASSNL_SHFT                            0x1
#define HWIO_GCC_SR_PLL_MODE_PLL_OUTCTRL_BMSK                             0x1
#define HWIO_GCC_SR_PLL_MODE_PLL_OUTCTRL_SHFT                             0x0

/*
 * PLL_USER_CTL register definitions.
 */
#define HWIO_GCC_SR_PLL_USER_CTL_ADDITIVE_FACTOR_BMSK              0xc0000000
#define HWIO_GCC_SR_PLL_USER_CTL_ADDITIVE_FACTOR_SHFT                    0x1e
#define HWIO_GCC_SR_PLL_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_BMSK  0x30000000
#define HWIO_GCC_SR_PLL_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_SHFT        0x1c
#define HWIO_GCC_SR_PLL_USER_CTL_MN_EN_BMSK                         0x1000000
#define HWIO_GCC_SR_PLL_USER_CTL_MN_EN_SHFT                              0x18
#define HWIO_GCC_SR_PLL_USER_CTL_VCO_SEL_BMSK                        0x300000
#define HWIO_GCC_SR_PLL_USER_CTL_VCO_SEL_SHFT                            0x14
#define HWIO_GCC_SR_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                    0x7000
#define HWIO_GCC_SR_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                       0xc
#define HWIO_GCC_SR_PLL_USER_CTL_POST_DIV_RATIO_BMSK                    0x300
#define HWIO_GCC_SR_PLL_USER_CTL_POST_DIV_RATIO_SHFT                      0x8
#define HWIO_GCC_SR_PLL_USER_CTL_OUTPUT_INV_BMSK                         0x80
#define HWIO_GCC_SR_PLL_USER_CTL_OUTPUT_INV_SHFT                          0x7
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_DIFF_90_BMSK                     0x40
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_DIFF_90_SHFT                      0x6
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_DIFF_0_BMSK                      0x20
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_DIFF_0_SHFT                       0x5
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                     0x10
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                      0x4
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                     0x8
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                     0x3
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                      0x4
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                      0x2
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                       0x2
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                       0x1
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                      0x1
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                      0x0


/*
 * PLL_STATUS register definitions.
 */
#define HWIO_GCC_SR_PLL_STATUS_PLL_ACTIVE_FLAG_BMSK                   0x20000
#define HWIO_GCC_SR_PLL_STATUS_PLL_ACTIVE_FLAG_SHFT                      0x11
#define HWIO_GCC_SR_PLL_STATUS_PLL_LOCK_DET_BMSK                      0x10000
#define HWIO_GCC_SR_PLL_STATUS_PLL_LOCK_DET_SHFT                         0x10
#define HWIO_GCC_SR_PLL_STATUS_PLL_D_BMSK                              0xffff
#define HWIO_GCC_SR_PLL_STATUS_PLL_D_SHFT                                 0x0





#define HWIO_PLL_MODE_OFFS_STROMER                 HWIO_OFFS(GCC_STROMER_PLL_MODE)
#define HWIO_PLL_L_VAL_OFFS_STROMER                HWIO_OFFS(GCC_STROMER_PLL_L_VAL)
#define HWIO_PLL_ALPHA_VAL_OFFS_STROMER            HWIO_OFFS(GCC_STROMER_PLL_ALPHA_VAL)
#define HWIO_PLL_ALPHA_VAL_U_OFFS_STROMER          HWIO_OFFS(GCC_STROMER_PLL_ALPHA_VAL_U)
#define HWIO_PLL_USER_CTL_OFFS_STROMER             HWIO_OFFS(GCC_STROMER_PLL_USER_CTL)
#define HWIO_PLL_CONFIG_CTL_OFFS_STROMER           HWIO_OFFS(GCC_STROMER_PLL_CONFIG_CTL)
#define HWIO_PLL_TEST_CTL_OFFS_STROMER             HWIO_OFFS(GCC_STROMER_PLL_TEST_CTL)


#define HWIO_PLL_MODE_OFFS_SR                 HWIO_OFFS(GCC_SR_PLL_MODE)
#define HWIO_PLL_L_VAL_OFFS_SR                HWIO_OFFS(GCC_SR_PLL_L_VAL)
#define HWIO_PLL_M_VAL_OFFS_SR                HWIO_OFFS(GCC_SR_PLL_M_VAL)
#define HWIO_PLL_N_VAL_OFFS_SR                HWIO_OFFS(GCC_SR_PLL_N_VAL)
#define HWIO_PLL_USER_CTL_OFFS_SR             HWIO_OFFS(GCC_SR_PLL_USER_CTL)
#define HWIO_PLL_CONFIG_CTL_OFFS_SR           HWIO_OFFS(GCC_SR_PLL_CONFIG_CTL)
#define HWIO_PLL_TEST_CTL_OFFS_SR             HWIO_OFFS(GCC_SR_PLL_TEST_CTL)
#define HWIO_PLL_STATUS_OFFS_SR               HWIO_OFFS(GCC_SR_PLL_STATUS)




#define CLOCK_PLL_MODE_ACTIVE  \
  (HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_OUTCTRL)  | \
   HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_BYPASSNL) | \
   HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_RESET_N))

/*
 * Definitions for configuring the PLL in FSM Mode
 */
#define CLOCK_PLL_BIAS_COUNT_VAL  (0x1 << HWIO_SHFT(GCC_STROMER_PLL_MODE, PLL_BIAS_COUNT))
#define CLOCK_PLL_LOCK_COUNT_VAL  (0x0 << HWIO_SHFT(GCC_STROMER_PLL_MODE, PLL_LOCK_COUNT))

/*
 * CLOCK_PLL_INIT_TIME_US
 *
 * Init time in microseconds after de-asserting PLL reset (normal mode).
 */
#define CLOCK_PLL_INIT_TIME_US 5

/*
 * CLOCK_PLL_WARMUP_TIME_US
 *
 * Warm-up time in microseconds after turning on the PLL.
 */
#define CLOCK_PLL_WARMUP_TIME_US 50

/*
 * NOT_2D / NOT_N_MINUS_M
 *
 * Macros to return the inverted value of the 2D field or (N - M)
 * in a type 1 mux structure.  Used to prepare the value for writing
 * to the hardware register field.
 */
#define NOT_2D(mux)         (~(mux)->n2D)
#define NOT_N_MINUS_M(mux)  (~((mux)->nN - (mux)->nM))

/*
 * HALF_DIVIDER
 *
 * Macro to return the normalized half divider for a given mux structure.
 * NOTE: Expecting (2 * divider) value as input.
 */
#define HALF_DIVIDER(mux)  ((mux)->nDiv2x ? (((mux)->nDiv2x) - 1) : 0)

/*
 * PLL lock timeout (2x max locking time).
 */
#define CLOCK_PLL_LOCK_TIMEOUT_US 200



/* Definitions for generalizing clock configuration */
#define CLOCK_CMD_CFG_UPDATE_FMSK   HWIO_FMSK(GCC_SDCC1_APPS_CMD_RCGR, UPDATE)

#define CLOCK_CFG_REG_OFFSET        (HWIO_ADDR(GCC_SDCC1_APPS_CFG_RCGR)-HWIO_ADDR(GCC_SDCC1_APPS_CMD_RCGR))
#define CLOCK_CFG_CGR_SRC_SEL_FMSK  HWIO_FMSK(GCC_SDCC1_APPS_CFG_RCGR, SRC_SEL)
#define CLOCK_CFG_CGR_SRC_SEL_SHFT  HWIO_SHFT(GCC_SDCC1_APPS_CFG_RCGR, SRC_SEL)
#define CLOCK_CFG_CGR_SRC_DIV_FMSK  HWIO_FMSK(GCC_SDCC1_APPS_CFG_RCGR, SRC_DIV)
#define CLOCK_CFG_CGR_SRC_DIV_SHFT  HWIO_SHFT(GCC_SDCC1_APPS_CFG_RCGR, SRC_DIV)

#define CLOCK_CFG_CGR_MODE_FMSK     HWIO_FMSK(GCC_SDCC1_APPS_CFG_RCGR, MODE)
#define CLOCK_CFG_CGR_MODE_SHFT     HWIO_SHFT(GCC_SDCC1_APPS_CFG_RCGR, MODE)
#define CLOCK_CFG_CGR_MODE_DUAL_EDGE_VAL  0x2
#define CLOCK_CFG_CGR_MODE_BYPASS_VAL     0x0

#define CLOCK_M_REG_OFFSET         (HWIO_ADDR(GCC_SDCC1_APPS_M)-HWIO_ADDR(GCC_SDCC1_APPS_CMD_RCGR))
#define CLOCK_N_REG_OFFSET         (HWIO_ADDR(GCC_SDCC1_APPS_N)-HWIO_ADDR(GCC_SDCC1_APPS_CMD_RCGR))
#define CLOCK_D_REG_OFFSET         (HWIO_ADDR(GCC_SDCC1_APPS_D)-HWIO_ADDR(GCC_SDCC1_APPS_CMD_RCGR))

/*=========================================================================
     Externs
==========================================================================*/

/*=========================================================================
      Function Prototypes
==========================================================================*/

boolean Clock_ConfigurePLL(const ClockConfigPLLType *pConfig);

/*=========================================================================
      Function Definitions
==========================================================================*/

boolean Clock_TriggerUpdate(uint32 cmdReg, uint32 nMask)
{
  uint32 nTimeout = 0;
  uint32 nVal;

  nVal = inpdw(cmdReg) | nMask;

  /* Trigger CMD_REG:UPDATE */
  outpdw(cmdReg, nVal);

  /* Wait for CMD_REG:UPDATE to clear, showing the clock switch is complete */
  while (inpdw(cmdReg) & nMask)
  {
    if (nTimeout++ >= CLOCK_UPDATE_TIMEOUT_US)
    {
      return FALSE;
    }
    busywait(1);
  }
  return TRUE;
}  
  
/* ============================================================================
**  Function : Clock_EnablePLL
** ============================================================================
*/
/*!
    Configure and enable a PLL.  If it is voteable, it will be set for FSM mode
    and voted for using the vote register.

    @param pConfig -  [IN] PLL configuration structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_EnablePLL(const ClockConfigPLLType *pConfig)
{
  uint32 nModeAddr, nStatusAddr,nStatusMask,nLockDetMask,nModeVal, nTimeout = 0;

  nModeAddr = pConfig->nPLLModeAddr;
  if(pConfig->ePLL != CLK_SOURCE_PLL_STROMER)
  {
     nStatusAddr = nModeAddr + HWIO_PLL_STATUS_OFFS_SR;
     nStatusMask = HWIO_FMSK(GCC_SR_PLL_STATUS, PLL_ACTIVE_FLAG);
     nLockDetMask = HWIO_FMSK(GCC_SR_PLL_STATUS, PLL_LOCK_DET);
  }
  else
  {
     nStatusAddr =  nModeAddr;
     nStatusMask =  HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_ACTIVE_FLAG);
     nLockDetMask = HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_LOCK_DET);
  } 
  nModeVal = inpdw(nModeAddr);

  if ( ((nModeVal & CLOCK_PLL_MODE_ACTIVE) == CLOCK_PLL_MODE_ACTIVE) ||
       (nModeVal & HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_VOTE_FSM_ENA)) )
  {
    /*
     * The PLL is already running, so vote for the resource but don't configure it.
     */
    if (pConfig->nVoteAddr != 0)
    {
      outpdw(pConfig->nVoteAddr, inpdw(pConfig->nVoteAddr) | pConfig->nVoteMask);

      /*
       * Wait for the PLL to go active.
       */
      nTimeout = 0;
      while ((inpdw(nStatusAddr) & nStatusMask) == 0)
      {
        if (nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US)
        {
          return FALSE;
        }
        busywait(1);
      }
    }

     return TRUE;
  }

  /*
   * Configure the PLL.
   */
  if(! Clock_ConfigurePLL(pConfig)) return FALSE;

  /*
   * Enable the PLL.
   */
  if (pConfig->nVoteAddr != 0)
  {
    outpdw(pConfig->nVoteAddr, inpdw(pConfig->nVoteAddr) | pConfig->nVoteMask);

    /*
     * Wait for the PLL to go active.
     */
    while ((inpdw(nStatusAddr) & nStatusMask) == 0)
       {
         if (nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US)
         {
           return FALSE;
         }
 
         busywait(1);
       }
    }
    else
    {
    nModeVal |= HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_BYPASSNL);
    outpdw(nModeAddr, nModeVal);
    busywait(CLOCK_PLL_INIT_TIME_US);

    nModeVal |= HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_RESET_N);
    outpdw(nModeAddr, nModeVal);
    busywait(CLOCK_PLL_WARMUP_TIME_US);

    while ((inpdw(nStatusAddr) & nLockDetMask) == 0)
    {
      if (nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US)
      {
        return FALSE;
      }

      busywait(1);
    }

    nModeVal |= HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_OUTCTRL);
    outpdw(nModeAddr, nModeVal);
  }

  return TRUE;

} /* END Clock_EnablePLL */


/* ============================================================================
**  Function : Clock_DisablePLL
** ============================================================================
*/
/*!
    Disable a PLL.

    @param pConfig -  [IN] PLL configuration structure
    @return
    TRUE -- The PLL was succesfully disabled.
    FALSE -- Disable failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_DisablePLL(const ClockConfigPLLType *pConfig)
{
  uint32 nModeAddr, nStatusAddr,nStatusMask, nModeVal, nTimeout = 0;

  nModeAddr = pConfig->nPLLModeAddr;
  if(pConfig->ePLL != CLK_SOURCE_PLL_STROMER)
  {
     nStatusAddr = nModeAddr + HWIO_PLL_STATUS_OFFS_SR;
     nStatusMask = HWIO_FMSK(GCC_SR_PLL_STATUS, PLL_ACTIVE_FLAG);
  }
  else
  {
     nStatusAddr = nModeAddr;
     nStatusMask = HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_ACTIVE_FLAG);
  } 
  nModeVal = inpdw(nModeAddr);

  if (nModeVal & HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_VOTE_FSM_ENA)) 
  {
    if (pConfig->nVoteAddr != 0)
    {
      outpdw(pConfig->nVoteAddr, (inpdw(pConfig->nVoteAddr) & ~pConfig->nVoteMask));

      while (inpdw(nStatusAddr) & nStatusMask)
      {
        if (nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US)
        {
          return FALSE;
        }
        busywait(1);
      }
      return TRUE;
    }
    /* An FSM enabled PLL requires a VoteAddr */
    return FALSE;
  }

  /*
   * Put the PLL in reset mode
   */
  nModeVal &= ~(HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_RESET_N) |
                HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_BYPASSNL) |
                HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_OUTCTRL));
  
  /* This is a non-shared PLL, so just turn it off */
  outpdw(nModeAddr, nModeVal);
  return TRUE;

} /* END Clock_DisablePLL */

/* ============================================================================
**  Function : Clock_IsPLLEnabled
** ============================================================================
*/

/*!
    Find out if a PLL is enabled already.

    @param pConfig -  [IN] PLL configuration structure
    @return
    TRUE -- The PLL was succesfully disabled.
    FALSE -- Disable failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_IsPLLEnabled(const ClockConfigPLLType *pConfig)
{
  uint32 nModeAddr, nModeVal;

  nModeAddr = pConfig->nPLLModeAddr;
  nModeVal = inpdw(nModeAddr);

  if (nModeVal & HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_VOTE_FSM_ENA))
  {
    if(inpdw(pConfig->nVoteAddr) & pConfig->nVoteMask)
    {
       return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    if(nModeVal & (HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_RESET_N) |
                HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_BYPASSNL) |
                HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_OUTCTRL)))
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
    
  }

} 
/* END Clock_IsPLLEnabled */

/* =========================================================================
**  Function : Clock_ConfigurePLL
** =========================================================================*/
/**
  Configures a PLL.

  @param *pConfig [in] -- PLL configuration

*/

boolean Clock_ConfigurePLL(const ClockConfigPLLType *pConfig)
{
  uint32 nModeAddr, nUserVal, nModeVal;

  nModeAddr = pConfig->nPLLModeAddr;
  nModeVal = inpdw(nModeAddr);

  if((nModeVal & CLOCK_PLL_MODE_ACTIVE) == CLOCK_PLL_MODE_ACTIVE)
  {
    return FALSE;
  }
  else if(nModeVal & HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_VOTE_FSM_ENA)) 
  {
    if((pConfig->nVoteAddr!=NULL)&&(inpdw(pConfig->nVoteAddr) & pConfig->nVoteMask))
    {
      return FALSE;
    }
  }

  if(pConfig->ePLL == CLK_SOURCE_PLL_STROMER)
  { /*
     * Program L and Alpha.
     */
    outpdw(nModeAddr + HWIO_PLL_L_VAL_OFFS_STROMER, pConfig->nL);
    outpdw(nModeAddr + HWIO_PLL_ALPHA_VAL_OFFS_STROMER, (uint32)pConfig->nAlpha);
    outpdw(nModeAddr + HWIO_PLL_ALPHA_VAL_U_OFFS_STROMER, (uint32)(pConfig->nAlpha >> 32));

    /*
     * Get the CONFIG value and clear out fields we will configure.
     */
     nUserVal = inpdw(nModeAddr + HWIO_PLL_USER_CTL_OFFS_STROMER);
     nUserVal &= ~(HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, ALPHA_EN)      |
                   HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, VCO_SEL)       |
                   HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, PRE_DIV_RATIO) |
                   HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, POST_DIV_RATIO));

     /*
      * Program the VCO.
      */
     nUserVal |= (pConfig->nVCO << HWIO_SHFT(GCC_STROMER_PLL_USER_CTL, VCO_SEL));

     /*
      * Program the pre-div value (div-1 to div-8).
      */
     if (pConfig->nPreDiv <= 8 && pConfig->nPreDiv > 0)
     {
       nUserVal |= ((pConfig->nPreDiv - 1) << HWIO_SHFT(GCC_STROMER_PLL_USER_CTL, PRE_DIV_RATIO));
     }

     /*
      * Program the post-div value (div-3 not supported)
      */
     if (pConfig->nPostDiv == 2)
     {
       nUserVal |= (1 << HWIO_SHFT(GCC_STROMER_PLL_USER_CTL, POST_DIV_RATIO));
     }
     else if (pConfig->nPostDiv == 4)
     {
      nUserVal |= (3 << HWIO_SHFT(GCC_STROMER_PLL_USER_CTL, POST_DIV_RATIO));
     }

     /*
      *  Check fractional output or integer output.
      */
     if (pConfig->nAlpha != 0)
     {
       nUserVal |= HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, ALPHA_EN);
     }

     /*
      * Enable MAIN_OUT_ENA bit. 
      */
     nUserVal |= HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, PLLOUT_LV_MAIN);
 
     /*
      * Finally program the CONFIG register.
      */
     outpdw(nModeAddr + HWIO_PLL_USER_CTL_OFFS_STROMER, nUserVal);

  } 
  else
  {
     /*
      * Program L, M, and N.
      */
     outpdw(nModeAddr + HWIO_PLL_L_VAL_OFFS_SR, pConfig->nL);
     outpdw(nModeAddr + HWIO_PLL_M_VAL_OFFS_SR, pConfig->nM);
     outpdw(nModeAddr + HWIO_PLL_N_VAL_OFFS_SR, pConfig->nN);
   
     /*
      * Get the CONFIG value and clear out fields we will configure.
      */
     nUserVal = inpdw(nModeAddr + HWIO_PLL_USER_CTL_OFFS_SR);
     nUserVal &= ~(HWIO_FMSK(GCC_SR_PLL_USER_CTL, MN_EN)         |
                  HWIO_FMSK(GCC_SR_PLL_USER_CTL, VCO_SEL)       |
                  HWIO_FMSK(GCC_SR_PLL_USER_CTL, PRE_DIV_RATIO) |
                  HWIO_FMSK(GCC_SR_PLL_USER_CTL, POST_DIV_RATIO));

     /*
      * Program the VCO.
      */
     nUserVal |= (pConfig->nVCO << HWIO_SHFT(GCC_SR_PLL_USER_CTL, VCO_SEL));

     /*
      * Program the pre-div value (div-1 to div-8).
      */
     if (pConfig->nPreDiv <= 8 && pConfig->nPreDiv > 0)
     {
       nUserVal |= ((pConfig->nPreDiv - 1) << HWIO_SHFT(GCC_SR_PLL_USER_CTL, PRE_DIV_RATIO));
     }

     /*
      * Program the post-div value (div-3 not supported)
      */
     if (pConfig->nPostDiv == 2)
     {
       nUserVal |= (1 << HWIO_SHFT(GCC_SR_PLL_USER_CTL, POST_DIV_RATIO));
     }
     else if (pConfig->nPostDiv == 4)
     {
       nUserVal |= (3 << HWIO_SHFT(GCC_SR_PLL_USER_CTL, POST_DIV_RATIO));
     }
   
     /*
      *  Check fractional output or integer output.
      */
     if (pConfig->nM != 0 && pConfig->nM < pConfig->nN)
     {
       nUserVal |= HWIO_FMSK(GCC_SR_PLL_USER_CTL, MN_EN);
     }

     /*
      * Enable MAIN_OUT_ENA bit. 
      */
     nUserVal |= HWIO_FMSK(GCC_SR_PLL_USER_CTL, PLLOUT_LV_MAIN);

     /*
      * Finally program the CONFIG register.
      */
     outpdw(nModeAddr + HWIO_PLL_USER_CTL_OFFS_SR, nUserVal);
 } 
  /*
   * Program the FSM portion of the mode register.
   */
  nModeVal &= ~HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_BIAS_COUNT);
  nModeVal &= ~HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_LOCK_COUNT);
  nModeVal |= CLOCK_PLL_BIAS_COUNT_VAL;
  nModeVal |= CLOCK_PLL_LOCK_COUNT_VAL;

  if (pConfig->nVoteAddr != 0)
  {
    nModeVal |= HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_VOTE_FSM_ENA);
  }

  outpdw(nModeAddr, nModeVal);
  
  if(pConfig->ePLL == CLK_SOURCE_PLL_STROMER)
  {
     /* The CONFIG_CTL resets with an incorrect default value.  Fix it. */
     outpdw(nModeAddr + HWIO_PLL_CONFIG_CTL_OFFS_STROMER, pConfig->nConfigCtl);
  }
  else
  {
     /* The CONFIG_CTL resets with an incorrect default value.  Fix it. */
     outpdw(nModeAddr + HWIO_PLL_CONFIG_CTL_OFFS_SR, pConfig->nConfigCtl);
  }
  return TRUE;
} /* END Clock_ConfigurePLL */


/* =========================================================================
**  Function : Clock_ConfigMux
** =========================================================================*/
/*!
    Configure a clock mux. 

    @param pConfig -  [IN] Clock mux config structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_ConfigMux (const ClockConfigMuxType *pConfig)
{
  uint32 nCmdCGRAddr, nCmdCGRVal, nCfgCGRAddr, nCfgCGRVal;
  uint32 nMAddr, nNAddr, nDAddr, nSource;
  uint32 nTimeout = 0;

  nSource = 0;

  if( ! Clock_SourceMapToMux( pConfig, &nSource)) return FALSE;
  
  nCmdCGRAddr = pConfig->nCMDCGRAddr;
  nCmdCGRVal  = inpdw(nCmdCGRAddr);
  nCfgCGRAddr = pConfig->nCMDCGRAddr + CLOCK_CFG_REG_OFFSET; 
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Clear the fields
   */
  nCfgCGRVal &= ~(CLOCK_CFG_CGR_SRC_SEL_FMSK |
                  CLOCK_CFG_CGR_SRC_DIV_FMSK |
                  CLOCK_CFG_CGR_MODE_FMSK);

  /*
   * Program the source and divider values.
   */
  nCfgCGRVal |= (nSource << CLOCK_CFG_CGR_SRC_SEL_SHFT)
                  & CLOCK_CFG_CGR_SRC_SEL_FMSK;
  nCfgCGRVal |= ((HALF_DIVIDER(pConfig) << CLOCK_CFG_CGR_SRC_DIV_SHFT)
                  & CLOCK_CFG_CGR_SRC_DIV_FMSK);

  /*
   * Set MND counter mode depending on if it is in use
   */
  if (pConfig->nM != 0 && (pConfig->nM < pConfig->nN))
  {
    nMAddr = pConfig->nCMDCGRAddr + CLOCK_M_REG_OFFSET;
    nNAddr = pConfig->nCMDCGRAddr + CLOCK_N_REG_OFFSET;
    nDAddr = pConfig->nCMDCGRAddr + CLOCK_D_REG_OFFSET;

    outpdw(nMAddr, pConfig->nM);
    outpdw(nNAddr, NOT_N_MINUS_M(pConfig));
    outpdw(nDAddr, NOT_2D(pConfig));

    nCfgCGRVal |= ((CLOCK_CFG_CGR_MODE_DUAL_EDGE_VAL << CLOCK_CFG_CGR_MODE_SHFT)
                    & CLOCK_CFG_CGR_MODE_FMSK);
  }

  /*
   * Write the final CFG register value
   */
  outpdw(nCfgCGRAddr, nCfgCGRVal);

  /*
   * Trigger the update
   */
  nCmdCGRVal |= CLOCK_CMD_CFG_UPDATE_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait until update finishes
   */
  while(inpdw(nCmdCGRAddr) & CLOCK_CMD_CFG_UPDATE_FMSK)
  {
    if (nTimeout++ >= CLOCK_UPDATE_TIMEOUT_US)
    {
      return FALSE;
    }

    busywait(1);
  }

  return TRUE;

} /* END Clock_ConfigMux */


/* =========================================================================
**  Function : Clock_ToggleClock
** =========================================================================*/
/*!
    Enable/Disable a Clock and poll for CLK_OFF BIT. 

    @param CBCR_addr - Address of the CBCR register
           enable :-  enable/disable the CBCR 
    TRUE -- CBCR programming successful.
    FALSE -- CBCR programming failed.

    @dependencies
    None.

    @sa None
*/

boolean Clock_ToggleClock(uint32 CBCR_addr, Clock_CBCRtoggleType toggle_clk)
{
  uint32 CBCR_value;
  
  if(toggle_clk >= NUM_CLK_TOGGLE_TYPES) return FALSE;

  CBCR_value = inpdw(CBCR_addr);

  if(toggle_clk == CLK_TOGGLE_ENABLE)  
  {
    CBCR_value = CBCR_value | HWIO_FMSK(GCC_SDCC1_APPS_CBCR, CLK_ENABLE);
    outpdw(CBCR_addr, CBCR_value);  
    do
    {
      CBCR_value = inpdw(CBCR_addr);
    }while((CBCR_value & HWIO_FMSK(GCC_SDCC1_APPS_CBCR, CLK_OFF)) != 0);

  }
  else
  {
    CBCR_value = CBCR_value & ~HWIO_FMSK(GCC_SDCC1_APPS_CBCR, CLK_ENABLE);
    outpdw(CBCR_addr, CBCR_value);  
  }
  return TRUE;
}  


/* =========================================================================
**  Function : Clock_DetectMuxCfg
** =========================================================================*/
/*!
    Detect a clock mux configuration. 

    @param pMuxCfg -  [IN] Clock mux config structure.
    @param CurrentCfg -  [IN] Current hardware Clock mux config structure.
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_DetectMuxCfg(const ClockConfigMuxType *pMuxCfg,ClockConfigMuxType *pCurrentCfg)
{
  uint32 nCfgCGRAddr, nCfgCGRVal;
  uint32 nSource;

  nSource = 0;
  nCfgCGRAddr = pMuxCfg->nCMDCGRAddr + CLOCK_CFG_REG_OFFSET; 
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * find the source and divider values.
   */
  nSource = (nCfgCGRVal & CLOCK_CFG_CGR_SRC_SEL_FMSK) >> CLOCK_CFG_CGR_SRC_SEL_SHFT;
  pCurrentCfg->eMux = pMuxCfg->eMux;
  if( ! Clock_MuxMapToSource(pCurrentCfg,nSource))
  {
    return FALSE;
  }
  
  /*
   * Get the divider
   */
  pCurrentCfg->nDiv2x = ((nCfgCGRVal & CLOCK_CFG_CGR_SRC_DIV_FMSK) >> CLOCK_CFG_CGR_SRC_DIV_SHFT) + 1;
  return TRUE;
} 



