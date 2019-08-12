/*
==============================================================================

FILE:         HALclkGenericPLL.c

DESCRIPTION:
  This file contains the generic clock HAL code for the PLL control.
  These PLLs are of the SR, SR2 and HF PLL variety.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/v1/HALclkGenericPLL.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
09/14/12   lil     Modified enabling sequence for SR and SR2 PLLs
02/24/11   gfr     Created

==============================================================================
            Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALhwio.h"
#include "HALclkGenericPLL.h"

/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/

extern HAL_clk_SourceControlType HAL_clk_StromerPLLControl;

/*
 * HAL_CLK_PLL_MODE_*
 *
 * Definitions for configuring the PLLn_MODE register.
 */
#define HAL_CLK_PLL_MODE_ACTIVE  \
  (HWIO_FMSK(PLL_MODE, PLL_OUTCTRL)  | \
   HWIO_FMSK(PLL_MODE, PLL_BYPASSNL) | \
   HWIO_FMSK(PLL_MODE, PLL_RESET_N))

/*
 * Definitions for configuring the PLL in FSM Mode
 */
#define HAL_CLK_PLL_SR_BIAS_COUNT_VAL   (0x1 << HWIO_SHFT(PLL_MODE, PLL_BIAS_COUNT))
#define HAL_CLK_PLL_SR_LOCK_COUNT_VAL   (0x0 << HWIO_SHFT(PLL_MODE, PLL_LOCK_COUNT))
#define HAL_CLK_PLL_SR2_BIAS_COUNT_VAL  (0x6 << HWIO_SHFT(PLL_MODE, PLL_BIAS_COUNT))
#define HAL_CLK_PLL_SR2_LOCK_COUNT_VAL  (0x0 << HWIO_SHFT(PLL_MODE, PLL_LOCK_COUNT))

/*
 * HAL_CLK_PLL_INIT_TIME_US
 *
 * Init time in microseconds after de-asserting PLL reset (normal mode).
 */
#define HAL_CLK_PLL_INIT_TIME_US 5

/*
 * HAL_CLK_PLL_WARMUP_TIME_US
 *
 * Warm-up time in microseconds after turning on the PLL.
 */
#define HAL_CLK_PLL_WARMUP_TIME_US 50


/* ============================================================================
**    Data Declarations
** ==========================================================================*/

/*
 * HAL_clk_SRPLLControl
 *
 * Control function array for the SR/SR2 PLLs.
 */
HAL_clk_SourceControlType HAL_clk_GenericPLLControl =
{
  /* .Enable          = */ HAL_clk_GenericPLLEnable,
  /* .Disable         = */ HAL_clk_GenericPLLDisable,
  /* .IsEnabled       = */ HAL_clk_GenericPLLIsEnabled,
  /* .Config          = */ HAL_clk_GenericPLLConfig,
  /* .ConfigPLL       = */ HAL_clk_GenericPLLConfigPLL,
  /* .DetectPLLConfig = */ HAL_clk_GenericPLLDetectPLLConfig,
  /* .EnableVote      = */ HAL_clk_GenericPLLEnableVote,
  /* .DisableVote     = */ HAL_clk_GenericPLLDisableVote,
  /* .IsVoteEnabled   = */ HAL_clk_GenericPLLIsVoteEnabled
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/

/* ===========================================================================
**  HAL_clk_VCOModeMapToHW
**
** ======================================================================== */

static uint32 HAL_clk_VCOModeMapToHW
(
  uint32 nVCOMode
)
{
  if((nVCOMode == 2)||(nVCOMode == 4))
  {
    /*
     * PLL is in quadrupler mode if the nVCOMode is 4.
     * PLL is in doubler mode if the nVCOMode is 2.
     */
    return (nVCOMode - 1);
  }
  else
  {
    /*
     * All other PLL modes for now are invalid or not in use.
     * the value 0 is an indication to bypass VCO mode all together.
     */
    return 0;
  }

} /* END HAL_clk_VCOModeMapToHW */


/* ===========================================================================
**  HAL_clk_GenericPLLEnableVote
**
** ======================================================================== */

void HAL_clk_GenericPLLEnableVote
(
  HAL_clk_SourceDescType *pSource
)
{
  HAL_clk_PLLContextType *pCtxt;
  uint32 nVal;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Set the appropriate bit.
   */
  nVal = inpdw(pCtxt->VoterRegister.nAddr) | pCtxt->VoterRegister.nMask;
  outpdw(pCtxt->VoterRegister.nAddr, nVal);

  /*
   * Wait for the PLL to go active.
   */
  while (HAL_CLK_PLL_HWIO_INF(PLL_STATUS, pCtxt, PLL_ACTIVE_FLAG) != 1)
  {
    HAL_clk_BusyWait(10);
  }

} /* END HAL_clk_GenericPLLEnableVote */


/* ===========================================================================
**  HAL_clk_GenericPLLDisableVote
**
** ======================================================================== */

void HAL_clk_GenericPLLDisableVote
(
  HAL_clk_SourceDescType *pSource
)
{
  HAL_clk_PLLContextType *pCtxt;
  uint32 nVal;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Clear the appropriate bit.
   */
  nVal = inpdw(pCtxt->VoterRegister.nAddr) & ~pCtxt->VoterRegister.nMask;
  outpdw(pCtxt->VoterRegister.nAddr, nVal);

} /* END HAL_clk_GenericPLLDisableVote */


/* ===========================================================================
**  HAL_clk_GenericPLLIsVoteEnabled
**
** ======================================================================== */

boolean HAL_clk_GenericPLLIsVoteEnabled
(
  HAL_clk_SourceDescType *pSource
)
{
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Check the appropriate bit.
   */
  if (inpdw(pCtxt->VoterRegister.nAddr) & pCtxt->VoterRegister.nMask)
  {
    return TRUE;
  }

  return FALSE;

} /* END HAL_clk_GenericPLLIsVoteEnabled */


/* ===========================================================================
**  HAL_clk_GenericPLLEnable
**
** ======================================================================== */

void HAL_clk_GenericPLLEnable
(
  HAL_clk_SourceDescType *pSource
)
{
  uint32 nModeVal;
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Check if PLL is already enabled and return
   */
  if (HAL_clk_GenericPLLIsEnabled(pSource))
  {
    return;
  }

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Read current mode value.
   */
  nModeVal  = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pCtxt);

  /*
   * Put the PLL in reset mode
   */
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_RESET_N);
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_BYPASSNL);
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_OUTCTRL);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

  /*
   * Use appropriate enabling sequence based on PLL type (SR / SR2)
   */
  switch (pCtxt->ePLLType)
  {
    case HAL_CLK_PLL_SR:
      /*
       * De-assert PLL reset and bypass
       */
      nModeVal |= HWIO_FMSK(PLL_MODE, PLL_RESET_N);
      nModeVal |= HWIO_FMSK(PLL_MODE, PLL_BYPASSNL);
      HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);
      break;

    case HAL_CLK_PLL_SR2:
    default:
      /*
       * De-assert PLL bypass
       */
      nModeVal |= HWIO_FMSK(PLL_MODE, PLL_BYPASSNL);
      HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

      /*
       * Wait for PLL init time
       */
      HAL_clk_BusyWait(HAL_CLK_PLL_INIT_TIME_US);

      /*
       * De-assert PLL reset
       */
      nModeVal |= HWIO_FMSK(PLL_MODE, PLL_RESET_N);
      HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);
      break;
  }

  /*
   * Wait for the PLL warm-up time
   */
  HAL_clk_BusyWait(HAL_CLK_PLL_WARMUP_TIME_US);

  /*
   * Wait for lock.
   */
  
  while (HAL_CLK_PLL_HWIO_INF(PLL_STATUS, pCtxt, PLL_LOCK_DET) == 0)
  {
    HAL_clk_BusyWait(10);
  }

  /*
   * Put the PLL in active mode
   */
  nModeVal |= HWIO_FMSK(PLL_MODE, PLL_OUTCTRL);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

} /* END HAL_clk_GenericPLLEnable */


/* ===========================================================================
**  HAL_clk_GenericPLLDisable
**
** ======================================================================== */

void HAL_clk_GenericPLLDisable
(
  HAL_clk_SourceDescType *pSource
)
{
  uint32  nModeVal;
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Read current mode value.
   */
  nModeVal  = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pCtxt);

  /*
   * Put the PLL in reset mode
   */
  nModeVal &= ~(HWIO_FMSK(PLL_MODE, PLL_RESET_N) |
                HWIO_FMSK(PLL_MODE, PLL_BYPASSNL) |
                HWIO_FMSK(PLL_MODE, PLL_OUTCTRL));
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

} /* END HAL_clk_GenericPLLDisable */


/* ===========================================================================
**  HAL_clk_GenericPLLIsEnabled
**
** ======================================================================== */

boolean HAL_clk_GenericPLLIsEnabled
(
  HAL_clk_SourceDescType *pSource
)
{
  uint32 nModeVal;
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Read current mode value.
   */
  nModeVal  = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pCtxt);

  /*
   * Check whether the PLL is enabled.
   */
  return ((nModeVal & HAL_CLK_PLL_MODE_ACTIVE) == HAL_CLK_PLL_MODE_ACTIVE);

} /* END HAL_clk_GenericPLLIsEnabled */


/* ===========================================================================
**  HAL_clk_GenericPLLConfig
**
** ======================================================================== */

void HAL_clk_GenericPLLConfig
(
  HAL_clk_SourceDescType   *pSource,
  HAL_clk_SourceConfigType  eConfig
)
{
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Take action.
   */
  switch (eConfig)
  {
    case HAL_CLK_CONFIG_PLL_FSM_MODE_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pCtxt, PLL_VOTE_FSM_ENA, 1);
      break;

    case HAL_CLK_CONFIG_PLL_FSM_MODE_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pCtxt, PLL_VOTE_FSM_ENA, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, PLLOUT_LV_AUX, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, PLLOUT_LV_AUX, 1);
      break;

    case HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, PLLOUT_LV_AUX2, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, PLLOUT_LV_AUX2, 1);
      break;

    case HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, PLLOUT_LV_EARLY, 1);
      break;

    case HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pCtxt, PLLOUT_LV_EARLY, 0);
      break;

    default:
      break;
  }

} /* END HAL_clk_GenericPLLConfig */


/* ===========================================================================
**  HAL_clk_GenericPLLConfigPLL
**
** ======================================================================== */

void HAL_clk_GenericPLLConfigPLL
(
  HAL_clk_SourceDescType      *pSource,
  const HAL_clk_PLLConfigType *pConfig
)
{
  uint32  nModeVal, nUserVal;
  uint32  nPLLInFSMMode = 0;
  boolean bPLLEnabled = FALSE;
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Check PLL mode.
   */
  nPLLInFSMMode = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pCtxt) &
    HWIO_FMSK(PLL_MODE, PLL_VOTE_FSM_ENA);
  bPLLEnabled = HAL_clk_GenericPLLIsEnabled(pSource);

  /*
   * Disable the PLL.
   */
  if (bPLLEnabled)
  {
    HAL_clk_GenericPLLDisable(pSource);
  }

  /*
   * Read the PLLx_Mode value
   */
  nModeVal  = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pCtxt);

  /*
   * Disable FSM Mode
   */
  if (nPLLInFSMMode != 0)
  {
    nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_VOTE_FSM_ENA);
    HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);
  }

  /*
   * Program L/M/N value.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_L_VAL, pCtxt, pConfig->nL);
  HAL_CLK_PLL_HWIO_OUT(PLL_M_VAL, pCtxt, pConfig->nM);
  HAL_CLK_PLL_HWIO_OUT(PLL_N_VAL, pCtxt, pConfig->nN);

  /*
   * Get the CONFIG value and clear out fields we will configure.
   */
  nUserVal  =  HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pCtxt);
  nUserVal &= ~(HWIO_FMSK(PLL_USER_CTL, MN_EN)                       |
                HWIO_FMSK(PLL_USER_CTL, VCO_SEL)                     |
                HWIO_FMSK(PLL_USER_CTL, VCO_DOUBLER_QUADRUPLER_MODE) |
                HWIO_FMSK(PLL_USER_CTL, PRE_DIV_RATIO)               |
                HWIO_FMSK(PLL_USER_CTL, POST_DIV_RATIO));

  /*
   * Find the VCO 
   */
  switch (pConfig->eVCO)
  {
    case HAL_CLK_PLL_VCO3:
      nUserVal |= HWIO_FVAL(PLL_USER_CTL, VCO_SEL, 2);
      break;

    case HAL_CLK_PLL_VCO2:
      nUserVal |= HWIO_FVAL(PLL_USER_CTL, VCO_SEL, 1);
      break;

    default:
      break;
  }

  /*
   * Program the pre-div value (div-1 to div-8).
   */
  if (pConfig->nPreDiv <= 8 && pConfig->nPreDiv > 0)
  {
    nUserVal |= HWIO_FVAL(PLL_USER_CTL, PRE_DIV_RATIO, (pConfig->nPreDiv - 1));
  }

  /*
   * Program the post-div value (div-3 not supported)
   */
  if (pConfig->nPostDiv == 2)
  {
    nUserVal |= HWIO_FVAL(PLL_USER_CTL, POST_DIV_RATIO, 1);
  }
  else if (pConfig->nPostDiv == 4)
  {
    nUserVal |= HWIO_FVAL(PLL_USER_CTL, POST_DIV_RATIO, 3);
  }

  /*
   *  Check fractional output or integer output.
   */
  if (pConfig->nM != 0 && pConfig->nM < pConfig->nN)
  {
    nUserVal |= HWIO_FMSK(PLL_USER_CTL, MN_EN);
  }

  /*
   *  Program the VCO multiplier value.
   */
  nUserVal |= HWIO_FVAL(PLL_USER_CTL, VCO_DOUBLER_QUADRUPLER_MODE,
                          HAL_clk_VCOModeMapToHW(pConfig->nVCOMultiplier));

  /*
   * Enable MAIN_OUT_ENA bit. 
   */
  nUserVal |= HWIO_FMSK(PLL_USER_CTL, PLLOUT_LV_MAIN);

  /*
   * Finally program the USER_CTL register.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL, pCtxt, nUserVal);

  /*
   * Program the FSM portion of the mode register.
   */
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_BIAS_COUNT);
  nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_LOCK_COUNT);

  /*
   * Program the BIAS Count and Lock Count for SR and SR2 PLLs separately
   */
  switch (pCtxt->ePLLType)
  {
    case HAL_CLK_PLL_SR:
      nModeVal |= HAL_CLK_PLL_SR_BIAS_COUNT_VAL;
      nModeVal |= HAL_CLK_PLL_SR_LOCK_COUNT_VAL;
      break;

    case HAL_CLK_PLL_SR2:
    default:
      nModeVal |= HAL_CLK_PLL_SR2_BIAS_COUNT_VAL;
      nModeVal |= HAL_CLK_PLL_SR2_LOCK_COUNT_VAL;
      break;
  }
  nModeVal |= nPLLInFSMMode;

  /*
   * Program new mode value.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pCtxt, nModeVal);

  /*
   * Enable the PLL if it was on before and not in FSM mode (in FSM
   * mode this function is not used).
   */
  if (bPLLEnabled && nPLLInFSMMode == 0)
  {
    HAL_clk_GenericPLLEnable(pSource);
  }

} /* END HAL_clk_GenericPLLConfigPLL */


/* ===========================================================================
**  HAL_clk_GenericPLLDetectPLLConfig
**
** ======================================================================== */

void HAL_clk_GenericPLLDetectPLLConfig
(
  HAL_clk_SourceDescType *pSource,
  HAL_clk_PLLConfigType  *pConfig
)
{
  uint32 nConfigVal;
  HAL_clk_PLLContextType *pCtxt;

  /*
   * Get the PLL context.
   */
  pCtxt = (HAL_clk_PLLContextType *)pSource->pContext;

  /*
   * Read current L, M and N values.
   */
  pConfig->nL = HAL_CLK_PLL_HWIO_IN(PLL_L_VAL, pCtxt);
  pConfig->nM = HAL_CLK_PLL_HWIO_IN(PLL_M_VAL, pCtxt);
  pConfig->nN = HAL_CLK_PLL_HWIO_IN(PLL_N_VAL, pCtxt);

  /*
   * Determine the oscillator source.
   */
  pConfig->eSource = HAL_CLK_SOURCE_XO;

  /*
   * Get the VCO 
   */
  nConfigVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pCtxt);
  pConfig->eVCO =
    (HAL_clk_PLLVCOType)((nConfigVal & HWIO_FMSK(PLL_USER_CTL, VCO_SEL))
                            >> HWIO_SHFT(PLL_USER_CTL, VCO_SEL));

  /*
   * Get the pre and post dividers.
   */
  pConfig->nPreDiv = ((nConfigVal & HWIO_FMSK(PLL_USER_CTL, PRE_DIV_RATIO))
                         >> (HWIO_SHFT(PLL_USER_CTL, PRE_DIV_RATIO))) + 1;
  pConfig->nPostDiv = ((nConfigVal & HWIO_FMSK(PLL_USER_CTL, POST_DIV_RATIO))
                >> HWIO_SHFT(PLL_USER_CTL, POST_DIV_RATIO)) + 1;

} /* END HAL_clk_GenericPLLDetectPLLConfig */


/* ===========================================================================
**  HAL_clk_InstallPLL
**
** ======================================================================== */

void HAL_clk_InstallPLL
(
  HAL_clk_SourceType      eSource,
  HAL_clk_PLLContextType *pCtxt,
  uint32                  nBaseAddress
)
{
  /*
   * Add base address
   */
  pCtxt->nAddr += nBaseAddress;

  if (pCtxt->VoterRegister.nAddr != 0)
  {
    pCtxt->VoterRegister.nAddr += nBaseAddress;
  }

  /*
   * Install PLL handler
   */
  
  if (pCtxt->ePLLType == HAL_CLK_PLL_STROMER)
  {
    HAL_clk_InstallSource(eSource, &HAL_clk_StromerPLLControl, pCtxt);
  }
  else
  {
    HAL_clk_InstallSource(eSource, &HAL_clk_GenericPLLControl, pCtxt);
  }
} /* END HAL_clk_InstallPLL */
