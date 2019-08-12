/*
==============================================================================

FILE:         HALclkMPLLDivSources.c

DESCRIPTION:
  This file contains the HAL code for the MPLL early out div source control.
  These PLLs are of the SR, SR2 and HF PLL variety.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/HALclkMPLL1DivSources.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
12/10/12   frv     Created

==============================================================================
            Copyright (c) 2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALhwio.h"
#include "HALclkHWIO.h"
#include "HALclkGeneric.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/


static boolean HAL_clk_MPLL1DivEnable           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData);
static void    HAL_clk_MPLL1DivDisable          ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData);
static boolean HAL_clk_MPLL1DivIsEnabled        ( HAL_clk_SourceDescType *pSource );
static void    HAL_clk_MPLL1DivDetectPLLConfig  ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );


/* ============================================================================
**    Data Declarations
** ==========================================================================*/


/*
 * HAL_clk_mMPLL1OutEarlyDivClockControl
 *
 * Functions for controlling mpll1_out_early_divx clocks.
 * Although these are clock control functions, they actually get invoked
 * when we enable the MPLL1_OUT_EARLY_DIVx sources.
 */
HAL_clk_ClockControlType HAL_clk_mMPLL1OutEarlyDivClockControl =
{
  /* .Enable           = */ HAL_clk_GenericEnable,
  /* .Disable          = */ HAL_clk_GenericDisable,
  /* .IsEnabled        = */ HAL_clk_GenericIsEnabled,
  /* .IsOn             = */ HAL_clk_GenericIsOn,
  /* .Reset            = */ NULL,
  /* .IsReset          = */ NULL,
  /* .Config           = */ NULL,
  /* .DetectConfig     = */ NULL,
  /* .ConfigDivider    = */ NULL,
  /* .DetectDivider    = */ NULL,
  /* .ConfigFootswitch = */ NULL,
};


/*
 * HAL_clk_MPLLEarlyDivControl
 *
 * Control function array for the MPLL1 early div sources.
 */
static HAL_clk_SourceControlType HAL_clk_MPLL1DivControl =
{
  /* .Enable          = */ HAL_clk_MPLL1DivEnable,
  /* .Disable         = */ HAL_clk_MPLL1DivDisable,
  /* .IsEnabled       = */ HAL_clk_MPLL1DivIsEnabled,
  /* .Config          = */ NULL,
  /* .ConfigPLL       = */ NULL,
  /* .DetectPLLConfig = */ HAL_clk_MPLL1DivDetectPLLConfig,
  /* .EnableVote      = */ NULL,
  /* .DisableVote     = */ NULL,
  /* .IsVoteEnabled   = */ NULL
};


/*
 *  HAL_clk_mMPLL1Div3Clk
 *
 *  mpll1_out_early_div3 feeds into both the Q6 and Modem cores and is treated
 *  as a source.
 *  The input clock to the Modem core is called mpll1_early_div3 which branches
 *  off of mpll1_out_early_div3.
 *
 *  List of clock sources supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mMPLL1Div3Clk =
{
  /* .szClockName      = */ "mpll1_out_early_div3",
  /* .mRegisters       = */ { HWIO_OFFS(MSS_MPLL1_OUT_EARLY_DIV3_CBCR), 0, {0, 0} },
  /* .pmControl        = */ 0, /* we don't control this since modem doesn't know when BBRX needs it */
  /* .nTestClock       = */ 0
};


/*
 *  HAL_clk_mMPLL1Div5Clk
 *
 *  mpll1_out_early_div5 feeds into both the Q6 and Modem cores and is treated
 *  as a source.
 *  The input clock to the Modem core is called mpll1_early_div5 which branches
 *  off of mpll1_out_early_div5.
 *
 *  List of clock sources supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mMPLL1Div5Clk =
{
  /* .szClockName      = */ "mpll1_out_early_div5",
  /* .mRegisters       = */ { HWIO_OFFS(MSS_MPLL1_OUT_EARLY_DIV5_CBCR), 0, {0, 0} },
  /* .pmControl        = */ &HAL_clk_mMPLL1OutEarlyDivClockControl,
  /* .nTestClock       = */ 0
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_MPLL1DivEnable
**
** ======================================================================== */

static boolean HAL_clk_MPLL1DivEnable
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  HAL_clk_ClockHandleType hClock;

  /*
   * Validate source pointer.
   */
  if (pSource == NULL)
  {
    return FALSE;
  }

  switch(pSource->eSource)
  {
    case HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV3:
      hClock = (HAL_clk_ClockHandleType)&HAL_clk_mMPLL1Div3Clk;
      break;

    case HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV5:
      hClock = (HAL_clk_ClockHandleType)&HAL_clk_mMPLL1Div5Clk;
      break;

    default:
      return FALSE;
  }

  HAL_clk_EnableClock(hClock);

  return TRUE;

} /* END HAL_clk_MPLL1DivEnable */


/* ===========================================================================
**  HAL_clk_MPLL1DivDisable
**
** ======================================================================== */

static void HAL_clk_MPLL1DivDisable
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  HAL_clk_ClockHandleType hClock;

  /*
   * Validate source pointer.
   */
  if (pSource == NULL)
  {
    return;
  }

  switch(pSource->eSource)
  {
    case HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV3:
      hClock = (HAL_clk_ClockHandleType)&HAL_clk_mMPLL1Div3Clk;
      break;

    case HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV5:
      hClock = (HAL_clk_ClockHandleType)&HAL_clk_mMPLL1Div5Clk;
      break;

    default:
      return;
  }

  HAL_clk_DisableClock(hClock);

} /* END HAL_clk_MPLL1DivDisable */


/* ===========================================================================
**  HAL_clk_MPLL1DivIsEnabled
**
** ======================================================================== */

static boolean HAL_clk_MPLL1DivIsEnabled
(
  HAL_clk_SourceDescType *pSource
)
{
  HAL_clk_ClockHandleType hClock;

  /*
   * Validate source pointer.
   */
  if (pSource == NULL)
  {
    return FALSE;
  }

  switch(pSource->eSource)
  {
    case HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV3:
      hClock = (HAL_clk_ClockHandleType)&HAL_clk_mMPLL1Div3Clk;
      break;

    case HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV5:
      hClock = (HAL_clk_ClockHandleType)&HAL_clk_mMPLL1Div5Clk;
      break;

    default:
      return FALSE;
  }

  return HAL_clk_IsClockEnabled(hClock);

} /* END HAL_clk_MPLL1DivIsEnabled */


/* ===========================================================================
**  HAL_clk_MPLL1DivDetectPLLConfig
**
** ======================================================================== */

static void HAL_clk_MPLL1DivDetectPLLConfig
(
  HAL_clk_SourceDescType *pSource,
  HAL_clk_PLLConfigType  *pConfig
)
{
  /*
   * Validate source pointers.
   */
  if (pSource == NULL || pConfig == NULL)
  {
    return;
  }

  switch(pSource->eSource)
  {
    case HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV3:
      pConfig->nPreDiv = 3;
      break;

    case HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV5:
      pConfig->nPreDiv = 5;
      break;

    default:
      return;
  }

  /*
   * Manually match up the BSP config.
   */
  pConfig->eSource  = HAL_CLK_SOURCE_MPLL1;
  pConfig->nL       = 0;
  pConfig->nM       = 0;
  pConfig->nN       = 1;
  pConfig->nPostDiv = 0;

} /* END HAL_clk_MPLL1DivDetectPLLConfig */


/* ===========================================================================
**  HAL_clk_InstallMPLL1DivSources
**
** ======================================================================== */

void HAL_clk_InstallMPLL1DivSources(void)
{

  /*
   * Update the address offsets.
   */
  HAL_clk_mMPLL1Div3Clk.mRegisters.nBranchAddr += MSS_TOP_BASE;
  HAL_clk_mMPLL1Div5Clk.mRegisters.nBranchAddr += MSS_TOP_BASE;

  /*
   * Install source control handler
   */
  HAL_clk_InstallSource(
    HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV3,
    &HAL_clk_MPLL1DivControl,
    NULL);

  HAL_clk_InstallSource(
    HAL_CLK_SOURCE_MPLL1_OUT_EARLY_DIV5,
    &HAL_clk_MPLL1DivControl,
    NULL);

} /* END HAL_clk_InstallMPLL1DivSources */
