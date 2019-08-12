/*
==============================================================================

FILE:         HALclk.c

DESCRIPTION:
  Common code for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/src/HALclk.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/18/11   vs      Ported from the latest clock driver.
01/24/13   pbitra  Replaced HAL_clk_GetChipId with HAL_clk_GetChipFamily
==============================================================================
            Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALclk.h>
#include "HALclkInternal.h"

#include <HALhwio.h>



/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/* ============================================================================
**    Typedefs
** ==========================================================================*/


/*
 * HAL_clk_Type
 *
 * Consolidated set of internal static data for the clock HAL.
 *
 * aSourceDesc        - Array of clock source descriptors.
 * pmClockDomainDesc  - Linked list of clock domain descriptors. 
 * pmPowerDomainDesc  - Linked list of power domain descriptors.
 * pmDebugDesc        - Linked list of debug descriptors.
 * nClocks            - Number of clocks
 * nClockDomains      - Number of clock domains
 * nPowerDomains      - Number of power domains
 * pmHALClkCtxt       - HAL clock driver context data pointer
 */
typedef struct
{
  HAL_clk_SourceDescType       aSourceDesc[HAL_CLK_NUM_OF_SOURCES];
  HAL_clk_ClockDomainDescType  *pmClockDomainDesc;
  HAL_clk_PowerDomainDescType  *pmPowerDomainDesc;
  HAL_clk_DebugDescType        *pmDebugDesc;
  uint32                       nClocks;
  uint32                       nClockDomains;
  uint32                       nPowerDomains;
  HAL_clk_ContextType          mHALClkCtxt;
} HAL_clk_Type;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk
 *
 * Consolidated static data for the clock HAL.
 *
 * We fill in some non-zero data to ensure that this ends up in the RW
 * segment instead of ZI since the clock HAL is used in the bootloader
 * before the ZI is cleared.
 */
static HAL_clk_Type HAL_clk;


/*
 * HAL_clk_Platform
 *
 * External reference to the platform configuration data.
 */
extern HAL_clk_PlatformType HAL_clk_Platform;


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_ClockControlDetectSourceTCXO
**
** ======================================================================== */

void HAL_clk_ClockControlDetectSourceTCXO
(
  HAL_clk_ClockDescType *pmClockDesc,
  HAL_clk_SourceType    *peSource
)
{
  *peSource = HAL_CLK_SOURCE_XO;

} /* END HAL_clk_ClockControlDetectSourceTCXO */


/* ===========================================================================
**  HAL_clk_ClockControlDetectSourceSleepClk
**
** ======================================================================== */

void HAL_clk_ClockControlDetectSourceSleepClk
(
  HAL_clk_ClockDescType *pmClockDesc,
  HAL_clk_SourceType    *peSource
)
{
  *peSource = HAL_CLK_SOURCE_SLEEPCLK;

} /* END HAL_clk_ClockControlDetectSourceSleepClk */


/* ===========================================================================
**  HAL_clk_ClockControlDetectSourceExternal
**
** ======================================================================== */

void HAL_clk_ClockControlDetectSourceExternal
(
  HAL_clk_ClockDescType *pmClockDesc,
  HAL_clk_SourceType    *peSource
)
{
  *peSource = HAL_CLK_SOURCE_EXTERNAL;

} /* END HAL_clk_ClockControlDetectSourceExternal */


/* ===========================================================================
**  HAL_clk_IsClockSupported
**
** ======================================================================== */

boolean HAL_clk_IsClockSupported
(
  HAL_clk_ClockHandleType pClockHandle,
  HAL_clk_ClockAPIType    eAPI
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  /*
   * Basic check.
   */
  if(pmClockDesc == NULL)
  {
    return FALSE;
  }

  if (eAPI == HAL_CLK_CLOCK_API_CALCFREQ)
  {
    return (pmClockDesc->nTestClock != 0);
  }

  if (pmClockDesc->pmControl  == NULL)
  {
    return FALSE;
  }

  /*
   * Check the API.
   */
  switch (eAPI)
  {
    case HAL_CLK_CLOCK_API_ENABLE:
      return (pmClockDesc->pmControl->Enable != NULL);

    case HAL_CLK_CLOCK_API_DISABLE:
      return (pmClockDesc->pmControl->Disable != NULL);

    case HAL_CLK_CLOCK_API_RESET:
      return (pmClockDesc->pmControl->Reset != NULL);

    case HAL_CLK_CLOCK_API_INVERT:
      return (pmClockDesc->pmControl->Reset != NULL);

    case HAL_CLK_CLOCK_API_ISON:
      return (pmClockDesc->pmControl->IsOn != NULL);

    case HAL_CLK_CLOCK_API_ISENABLED:
      return (pmClockDesc->pmControl->IsEnabled != NULL);

    case HAL_CLK_CLOCK_API_CONFIG:
      return (pmClockDesc->pmControl->Config != NULL);

    case HAL_CLK_CLOCK_API_CONFIGDIVIDER:
      return (pmClockDesc->pmControl->ConfigDivider != NULL);

    default:
      /*
       * Do nothing for unsupported APIs.
       */
      break;
  }

  /*
   * Unknown API.
   */
  return FALSE;

} /* END HAL_clk_IsClockSupported */


/* ===========================================================================
**  HAL_clk_IsClockRegimeSupported
**
** ======================================================================== */

boolean HAL_clk_IsClockRegimeSupported
(
  HAL_clk_ClockDomainHandleType pClockDomainHandle
)
{
  HAL_clk_ClockDomainDescType *pmClockDomainDesc =
      (HAL_clk_ClockDomainDescType *)pClockDomainHandle;

  /*
   * Basic check.
   */
  if(pmClockDomainDesc == NULL)
  {
    return FALSE;
  }

  /*
   * Supported.
   */
  return TRUE;

} /* END HAL_clk_IsClockRegimeSupported */


/* ===========================================================================
**  HAL_clk_EnableClock
**
** ======================================================================== */

void HAL_clk_EnableClock
(
  HAL_clk_ClockHandleType pClockHandle
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc                     != NULL &&
     pmClockDesc->pmControl          != NULL &&
     pmClockDesc->pmControl->Enable  != NULL )
  {
    pmClockDesc->pmControl->Enable(pmClockDesc);
  }

} /* END HAL_clk_EnableClock */


/* ===========================================================================
**  HAL_clk_EnableClockDomain
**
** ======================================================================== */

void HAL_clk_EnableClockDomain
(
  HAL_clk_ClockDomainHandleType  pClockDomainHandle,
  uint32                        *pnMask
)
{
  uint32                       i;
  HAL_clk_ClockDescType       *pmCurClock;
  HAL_clk_ClockDomainDescType *pmClockDomainDesc = 
    (HAL_clk_ClockDomainDescType *)pClockDomainHandle;

  if (pmClockDomainDesc == NULL || pnMask == NULL)
  {
    return;
  }

  for (i = 0; i < pmClockDomainDesc->nClockCount; i++)
  {
    pmCurClock = &pmClockDomainDesc->pmClocks[i];
    if (!HAL_clk_IsClockEnabled(pmCurClock))
    {
      if (*pnMask & (1 << i))
      {
        HAL_clk_EnableClock(pmCurClock);
      }
      *pnMask |= 1 << i;
    }
    else
    {
      *pnMask &= ~(1 << i);
    }
  }

  for (i = 0; i < pmClockDomainDesc->nClockCount; i++)
  {
    pmCurClock = &pmClockDomainDesc->pmClocks[i];
    HAL_clk_WaitForClockOn(pmCurClock);
  }

} /* END HAL_clk_EnableClockDomain */


/* ===========================================================================
**  HAL_clk_DisableClock
**
** ======================================================================== */

void HAL_clk_DisableClock
(
  HAL_clk_ClockHandleType pClockHandle
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc                     != NULL &&
     pmClockDesc->pmControl          != NULL &&
     pmClockDesc->pmControl->Disable != NULL )
  {
    pmClockDesc->pmControl->Disable(pmClockDesc);
  }

} /* END HAL_clk_DisableClock */


/* ===========================================================================
**  HAL_clk_DisableClockDomain
**
** ======================================================================== */

void HAL_clk_DisableClockDomain
(
  HAL_clk_ClockDomainHandleType pClockDomainHandle,
  uint32                        *pnMask
)
{
  uint32                       i;
  HAL_clk_ClockDescType       *pmCurClock;
  HAL_clk_ClockDomainDescType *pmClockDomainDesc = 
    (HAL_clk_ClockDomainDescType *)pClockDomainHandle;

  if (pmClockDomainDesc == NULL || pnMask == NULL)
  {
    return;
  }

  for (i = 0; i < pmClockDomainDesc->nClockCount; i++)
  {
    pmCurClock = &pmClockDomainDesc->pmClocks[i];

    if (HAL_clk_IsClockEnabled(pmCurClock))
    {
      if (*pnMask & (1 << i))
      {
        HAL_clk_DisableClock(pmCurClock);
      }
      *pnMask |= 1 << i;
    }
    else
    {
      *pnMask &= ~(1 << i);
    }
  }

  for (i = 0; i < pmClockDomainDesc->nClockCount; i++)
  {
    pmCurClock = &pmClockDomainDesc->pmClocks[i];
    HAL_clk_WaitForClockOff(pmCurClock);
  }

} /* END HAL_clk_DisableClockDomain */


/* ===========================================================================
**  HAL_clk_IsClockEnabled
**
** ======================================================================== */

boolean HAL_clk_IsClockEnabled
(
  HAL_clk_ClockHandleType pClockHandle
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc                        != NULL &&
     pmClockDesc->pmControl             != NULL &&
     pmClockDesc->pmControl->IsEnabled  != NULL )
  {
    return pmClockDesc->pmControl->IsEnabled(pmClockDesc);
  }

  return FALSE;

} /* END HAL_clk_IsClockEnabled */


/* ===========================================================================
**  HAL_clk_IsClockOn
**
** ======================================================================== */

boolean HAL_clk_IsClockOn
(
  HAL_clk_ClockHandleType pClockHandle
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc                   != NULL &&
     pmClockDesc->pmControl        != NULL &&
     pmClockDesc->pmControl->IsOn  != NULL )
  {
    return pmClockDesc->pmControl->IsOn(pmClockDesc);
  }

  return FALSE;

} /* END HAL_clk_IsClockOn */


/* ===========================================================================
**  HAL_clk_ResetClock
**
** ======================================================================== */

void HAL_clk_ResetClock
(
  HAL_clk_ClockHandleType pClockHandle,
  boolean                 bReset
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc                           != NULL &&
     pmClockDesc->pmControl                != NULL &&
     pmClockDesc->pmControl->Reset != NULL )
  {
    pmClockDesc->pmControl->Reset(pmClockDesc, bReset);
  }

} /* END HAL_clk_ResetClock */


/* ===========================================================================
**  HAL_clk_ConfigClockDivider
**
** ======================================================================== */

void HAL_clk_ConfigClockDivider
(
  HAL_clk_ClockHandleType pClockHandle,
  uint32                  nDiv
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc                           != NULL &&
     pmClockDesc->pmControl                != NULL &&
     pmClockDesc->pmControl->ConfigDivider != NULL )
  {
    pmClockDesc->pmControl->ConfigDivider(pmClockDesc, nDiv);
  }

} /* END HAL_clk_ConfigClockDivider */


/* ===========================================================================
**  HAL_clk_ConfigClock
**
** ======================================================================== */

void HAL_clk_ConfigClock
(
  HAL_clk_ClockHandleType pClockHandle,
  HAL_clk_ClockConfigType eConfig
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc                    != NULL &&
     pmClockDesc->pmControl         != NULL &&
     pmClockDesc->pmControl->Config != NULL )
  {
    pmClockDesc->pmControl->Config(pmClockDesc, eConfig);
  }

} /* END HAL_clk_ConfigClock */


/* ===========================================================================
**  HAL_clk_ConfigClockMux
**
** ======================================================================== */

void HAL_clk_ConfigClockMux
(
  HAL_clk_ClockDomainHandleType     pClockDomainHandle,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  HAL_clk_ClockDomainDescType *pmClockDomainDesc =
      (HAL_clk_ClockDomainDescType *)pClockDomainHandle;

  if(pmClockDomainDesc                       != NULL &&
     pmClockDomainDesc->pmControl            != NULL &&
     pmClockDomainDesc->pmControl->ConfigMux != NULL )
  {
    pmClockDomainDesc->pmControl->ConfigMux(pmClockDomainDesc, pmConfig);
  }

} /* END HAL_clk_ConfigClockMux */


/* ===========================================================================
**  HAL_clk_DetectClockConfig
**
** ======================================================================== */

void HAL_clk_DetectClockConfig
(
  HAL_clk_ClockHandleType  pClockHandle,
  uint32                  *pnConfig,
  boolean                 *pbValid
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  *pbValid = FALSE;

  if(pmClockDesc                              != NULL &&
     pmClockDesc->pmControl                   != NULL &&
     pmClockDesc->pmControl->DetectConfig     != NULL )
  {
    pmClockDesc->pmControl->DetectConfig(pmClockDesc, pnConfig, pbValid);
  }

} /* END HAL_clk_DetectClockConfig */


/* ===========================================================================
**  HAL_clk_DetectClockDivider
**
** ======================================================================== */

void HAL_clk_DetectClockDivider
(
  HAL_clk_ClockHandleType  pClockHandle,
  uint32                  *pnDiv
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  *pnDiv = 0;

  if(pmClockDesc                              != NULL &&
     pmClockDesc->pmControl                   != NULL &&
     pmClockDesc->pmControl->DetectDivider    != NULL )
  {
    pmClockDesc->pmControl->DetectDivider(pmClockDesc, pnDiv);
  }

} /* END HAL_clk_DetectClockDivider */


/* ===========================================================================
**  HAL_clk_DetectClockMuxConfig
**
** ======================================================================== */

void HAL_clk_DetectClockMuxConfig
(
  HAL_clk_ClockDomainHandleType  pClockDomainHandle,
  HAL_clk_ClockMuxConfigType    *pmConfig
)
{
  HAL_clk_ClockDomainDescType *pmClockDomainDesc =
      (HAL_clk_ClockDomainDescType *)pClockDomainHandle;


  if(pmClockDomainDesc                             != NULL &&
     pmClockDomainDesc->pmControl                  != NULL &&
     pmClockDomainDesc->pmControl->DetectMuxConfig != NULL &&
     pmConfig                                      != NULL )
  {
    pmClockDomainDesc->pmControl->DetectMuxConfig(pmClockDomainDesc, pmConfig);
  }
  else if ( pmConfig != NULL )
  {
    pmConfig->eSource = HAL_CLK_SOURCE_NULL;
  }

} /* END HAL_clk_DetectClockMuxConfig */


/* ===========================================================================
**  HAL_clk_ConfigFootswitch
**
** ======================================================================== */

void HAL_clk_ConfigFootswitch
(
  HAL_clk_ClockHandleType      pClockHandle,
  HAL_clk_FootswitchConfigType eConfig
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc                              != NULL &&
     pmClockDesc->pmControl                   != NULL &&
     pmClockDesc->pmControl->ConfigFootswitch != NULL )
  {
    pmClockDesc->pmControl->ConfigFootswitch(pmClockDesc, eConfig);
  }

} /* END HAL_clk_ConfigFootswitch */


/* ===========================================================================
**  HAL_clk_EnablePowerDomain
**
** ======================================================================== */

void HAL_clk_EnablePowerDomain
(
  HAL_clk_PowerDomainHandleType pPowerDomainHandle, 
  boolean                       bAsync
)
{
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc =
      (HAL_clk_PowerDomainDescType *)pPowerDomainHandle;

  if(pmPowerDomainDesc                      != NULL &&
     pmPowerDomainDesc->pmControl           != NULL &&
     pmPowerDomainDesc->pmControl->Enable   != NULL)
  {
    pmPowerDomainDesc->pmControl->Enable(pmPowerDomainDesc, bAsync);
  }

} /* END HAL_clk_EnablePowerDomain */


/* ===========================================================================
**  HAL_clk_DisablePowerDomain
**
** ======================================================================== */

void HAL_clk_DisablePowerDomain
(
  HAL_clk_PowerDomainHandleType pPowerDomainHandle
)
{
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc =
      (HAL_clk_PowerDomainDescType *)pPowerDomainHandle;

  if(pmPowerDomainDesc                      != NULL &&
     pmPowerDomainDesc->pmControl           != NULL &&
     pmPowerDomainDesc->pmControl->Disable  != NULL)
  {
    pmPowerDomainDesc->pmControl->Disable(pmPowerDomainDesc);
  }

} /* END HAL_clk_DisablePowerDomain */


/* ===========================================================================
**  HAL_clk_IsPowerDomainSupported
**
** ======================================================================== */

boolean HAL_clk_IsPowerDomainSupported
(
  HAL_clk_PowerDomainHandleType pPowerDomainHandle
)
{
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc =
      (HAL_clk_PowerDomainDescType *)pPowerDomainHandle;

  if((pmPowerDomainDesc != NULL) && (pmPowerDomainDesc->pmControl != NULL))
  {
    return TRUE;
  }

  return FALSE;

} /* END HAL_clk_IsPowerDomainSupported */


/* ===========================================================================
**  HAL_clk_IsPowerDomainEnabled
**
** ======================================================================== */

boolean HAL_clk_IsPowerDomainEnabled
(
  HAL_clk_PowerDomainHandleType pPowerDomainHandle
)
{
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc =
      (HAL_clk_PowerDomainDescType *)pPowerDomainHandle;

  if(pmPowerDomainDesc                        != NULL &&
     pmPowerDomainDesc->pmControl             != NULL &&
     pmPowerDomainDesc->pmControl->IsEnabled  != NULL)
  {
    return pmPowerDomainDesc->pmControl->IsEnabled(pmPowerDomainDesc);
  }

  return FALSE;

} /* END HAL_clk_IsPowerDomainEnabled */


/* ===========================================================================
**  HAL_clk_EnableSource
**
** ======================================================================== */

void HAL_clk_EnableSource
(
  HAL_clk_SourceType eSource
)
{
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->Enable != NULL)
  {
    HAL_clk.aSourceDesc[eSource].pControl->Enable(
      &HAL_clk.aSourceDesc[eSource]);
  }

} /* END HAL_clk_EnableSource */


/* ===========================================================================
**  HAL_clk_DisableSource
**
** ======================================================================== */

void HAL_clk_DisableSource
(
  HAL_clk_SourceType eSource
)
{
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->Disable != NULL)
  {
    HAL_clk.aSourceDesc[eSource].pControl->Disable(
      &HAL_clk.aSourceDesc[eSource]);
  }

} /* END HAL_clk_DisableSource */


/* ===========================================================================
**  HAL_clk_EnableSourceVote
**
** ======================================================================== */

void HAL_clk_EnableSourceVote
(
  HAL_clk_SourceType eSource
)
{
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->EnableVote != NULL)
  {
    HAL_clk.aSourceDesc[eSource].pControl->EnableVote(
      &HAL_clk.aSourceDesc[eSource]);
  }

} /* END HAL_clk_EnableSourceVote */


/* ===========================================================================
**  HAL_clk_DisableSourceVote
**
** ======================================================================== */

void HAL_clk_DisableSourceVote
(
  HAL_clk_SourceType eSource
)
{
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->DisableVote != NULL)
  {
    HAL_clk.aSourceDesc[eSource].pControl->DisableVote(
      &HAL_clk.aSourceDesc[eSource]);
  }

} /* END HAL_clk_DisableSourceVote */


/* ===========================================================================
**  HAL_clk_ConfigSource
**
** ======================================================================== */

void HAL_clk_ConfigSource
(
  HAL_clk_SourceType       eSource,
  HAL_clk_SourceConfigType eConfig
)
{
  /*
   * Configure the source if possible.
   */
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->Config != NULL)
  {
    HAL_clk.aSourceDesc[eSource].pControl->Config(
      &HAL_clk.aSourceDesc[eSource], eConfig);
  }

} /* END HAL_clk_ConfigSource */


/* ===========================================================================
**  HAL_clk_ConfigPLL
**
** ======================================================================== */

void HAL_clk_ConfigPLL
(
  HAL_clk_SourceType           eSource,
  const HAL_clk_PLLConfigType *pConfig
)
{
  /*
   * Configure the source if possible.
   */
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->ConfigPLL != NULL)
  {
    HAL_clk.aSourceDesc[eSource].pControl->ConfigPLL(
      &HAL_clk.aSourceDesc[eSource], pConfig);
  }

} /* END HAL_clk_ConfigPLL */


/* ===========================================================================
**  HAL_clk_DetectPLLConfig
**
** ======================================================================== */

void HAL_clk_DetectPLLConfig
(
  HAL_clk_SourceType     eSource,
  HAL_clk_PLLConfigType *pConfig
)
{
  /*
   * Initialize to NULL.
   */
  pConfig->eSource = HAL_CLK_SOURCE_NULL;

  /*
   * Request the frequency for this config.
   */
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->DetectPLLConfig != NULL)
  {
    HAL_clk.aSourceDesc[eSource].pControl->DetectPLLConfig(
      &HAL_clk.aSourceDesc[eSource], pConfig);
  }

} /* END HAL_clk_DetectPLLConfig */


/* ===========================================================================
**  HAL_clk_IsSourceVoteEnabled
**
** ======================================================================== */

boolean HAL_clk_IsSourceVoteEnabled
(
  HAL_clk_SourceType eSource
)
{
  /*
   * Invoke the callback.
   */
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->IsVoteEnabled != NULL)
  {
    return HAL_clk.aSourceDesc[eSource].pControl->IsVoteEnabled(
      &HAL_clk.aSourceDesc[eSource]);
  }

  return FALSE;

} /* END HAL_clk_IsSourceVoteEnabled */


/* ===========================================================================
**  HAL_clk_WaitForClockOff
**
** ======================================================================== */

boolean HAL_clk_WaitForClockOff
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uint32 nTimeout = 0;
  boolean bIsOn;

  if(pmClockDesc                   == NULL ||
     pmClockDesc->pmControl        == NULL ||
     pmClockDesc->pmControl->IsOn  == NULL )
  {
    return FALSE;
  }

  bIsOn = pmClockDesc->pmControl->IsOn(pmClockDesc);

  while (bIsOn && nTimeout++ < 100)
  {
    HAL_clk_BusyWait(1);
    bIsOn = pmClockDesc->pmControl->IsOn(pmClockDesc);
  }

  return !bIsOn;

} /* END HAL_clk_WaitForClockOff */


/* ===========================================================================
**  HAL_clk_WaitForClockOn
**
** ======================================================================== */

boolean HAL_clk_WaitForClockOn
(
  HAL_clk_ClockDescType *pmClockDesc
)
{
  uint32 nTimeout = 0;
  boolean bIsOn;

  if(pmClockDesc                   == NULL ||
     pmClockDesc->pmControl        == NULL ||
     pmClockDesc->pmControl->IsOn  == NULL )
  {
    return FALSE;
  }

  bIsOn = pmClockDesc->pmControl->IsOn(pmClockDesc);

  while (!bIsOn && nTimeout++ < 100)
  {
    HAL_clk_BusyWait(1);
    bIsOn = pmClockDesc->pmControl->IsOn(pmClockDesc);
  }

  return bIsOn;

} /* END HAL_clk_WaitForClockOn */


/* ===========================================================================
**  HAL_clk_IsSourceEnabled
**
** ======================================================================== */

boolean HAL_clk_IsSourceEnabled
(
  HAL_clk_SourceType eSource
)
{
  /*
   * Invoke the callback.
   */
  if (eSource < HAL_CLK_NUM_OF_SOURCES &&
      HAL_clk.aSourceDesc[eSource].pControl != NULL &&
      HAL_clk.aSourceDesc[eSource].pControl->IsEnabled != NULL)
  {
    return HAL_clk.aSourceDesc[eSource].pControl->IsEnabled(
      &HAL_clk.aSourceDesc[eSource]);
  }

  return FALSE;

} /* END HAL_clk_IsSourceEnabled */


/* ===========================================================================
**  HAL_clk_BusyWait
**
** ======================================================================== */

void HAL_clk_BusyWait
(
  uint32 uSec
)
{
  if (HAL_clk.mHALClkCtxt.BusyWait != NULL)
  {
     HAL_clk.mHALClkCtxt.BusyWait(uSec);
  }
  else
  {
    /* We can have our own nop loop here but it won't be precise since
     * we don't know how many time to iterate to meet the required uSec
     * pause - need to know cpu frequency.
     */
  }

} /* END HAL_clk_BusyWait */


/* ===========================================================================
**  HAL_clk_GetNumberOfClocks
**
** ======================================================================== */

uint32 HAL_clk_GetNumberOfClocks ( void )
{
  return HAL_clk.nClocks;

} /* END HAL_clk_GetNumberOfClocks */


/* ===========================================================================
**  HAL_clk_GetNumberOfClockDomains
**
** ======================================================================== */

uint32 HAL_clk_GetNumberOfClockDomains ( void )
{
  return HAL_clk.nClockDomains;

} /* END HAL_clk_GetNumberOfClockDomains */


/* ===========================================================================
**  HAL_clk_GetNumberOfPowerDomains
**
** ======================================================================== */

uint32 HAL_clk_GetNumberOfPowerDomains ( void )
{
  return HAL_clk.nPowerDomains;

} /* END HAL_clk_GetNumberOfPowerDomains */


/* ===========================================================================
**  HAL_clk_GetNextClockInDomain
**
** ======================================================================== */

HAL_clk_ClockHandleType HAL_clk_GetNextClockInDomain
(
  HAL_clk_ClockDomainHandleType   pClockDomainHandle,
  uint32                          nClockIndex
)
{
  HAL_clk_ClockDomainDescType *pmClockDomainDesc =
      (HAL_clk_ClockDomainDescType *)pClockDomainHandle;

  if((pmClockDomainDesc == NULL) || (nClockIndex >= pmClockDomainDesc->nClockCount))
  {
    return NULL;
  }
  else
  {
    return ((HAL_clk_ClockHandleType)(&pmClockDomainDesc->pmClocks[nClockIndex]));
  }

} /* END HAL_clk_GetNextClockInDomain */


/* ===========================================================================
**  HAL_clk_GetNextClockDomain
**
** ======================================================================== */

void HAL_clk_GetNextClockDomain
(
  HAL_clk_ClockDomainHandleType  pClockDomainHandle,
  HAL_clk_ClockDomainHandleType *pNextClockDomainHandle
)
{
  HAL_clk_ClockDomainDescType *pmClockDomainDesc =
      (HAL_clk_ClockDomainDescType *)pClockDomainHandle;

  /*
   * Sanity check
   */
  if(pmClockDomainDesc != NULL)
  {
    *pNextClockDomainHandle = (HAL_clk_ClockDomainHandleType)pmClockDomainDesc->pmNextClockDomain;
  }
  else
  {
    /*
     * Return head/first clock domain from the linked list
     */
    *pNextClockDomainHandle = (HAL_clk_ClockDomainHandleType)HAL_clk.pmClockDomainDesc;
  }

} /* END HAL_clk_GetNextClockDomain */


/* ===========================================================================
**  HAL_clk_GetNextPowerDomain
**
** ======================================================================== */

void HAL_clk_GetNextPowerDomain
(
  HAL_clk_PowerDomainHandleType  pPowerDomainHandle,
  HAL_clk_PowerDomainHandleType *pNextPowerDomainHandle
)
{
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc =
      (HAL_clk_PowerDomainDescType *)pPowerDomainHandle;

  /*
   * Sanity check
   */
  if(pmPowerDomainDesc != NULL)
  {
    *pNextPowerDomainHandle = (HAL_clk_PowerDomainHandleType)pmPowerDomainDesc->pmNextPowerDomain;
  }
  else
  {
    /*
     * Return head/first power domain from the linked list
     */
    *pNextPowerDomainHandle = (HAL_clk_PowerDomainHandleType)HAL_clk.pmPowerDomainDesc;
  }

} /* END HAL_clk_GetNextPowerDomain */


/* ===========================================================================
**  HAL_clk_GetNextDebugDesc
**
** ======================================================================== */

void HAL_clk_GetNextDebugDesc
( 
  HAL_clk_DebugDescType  *pmDebugDesc, 
  HAL_clk_DebugDescType **pmNextDebugDesc
)
{
  /*
   * Sanity check  
   */
  if(pmDebugDesc != NULL)
  {
    *pmNextDebugDesc = pmDebugDesc->pmNextDebugDesc;
  }
  else
  {
    /*
     * Return head/first debug descriptor from the linked list
     */
    *pmNextDebugDesc = HAL_clk.pmDebugDesc;
  }

} /* END HAL_clk_GetNextDebugDesc */


/* ===========================================================================
**  HAL_clk_GetClockName
**
** ======================================================================== */

void HAL_clk_GetClockName
(
  HAL_clk_ClockHandleType   pClockHandle,
  const char              **szClockName
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if(pmClockDesc != NULL)
  {
    *szClockName = pmClockDesc->szClockName;
  }

} /* END HAL_clk_GetClockName */


/* ===========================================================================
**  HAL_clk_GetPowerDomainName
**
** ======================================================================== */

void HAL_clk_GetPowerDomainName
(
  HAL_clk_PowerDomainHandleType   pPowerDomainHandle,
  const char                    **szPowerDomainName
)
{
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc =
    (HAL_clk_PowerDomainDescType *)pPowerDomainHandle;

  if(pmPowerDomainDesc != NULL)
  {
    *szPowerDomainName = pmPowerDomainDesc->szPowerDomainName;
  }

} /* END HAL_clk_GetPowerDomainName */


/* ===========================================================================
**  HAL_clk_InstallSource
**
** ======================================================================== */

void HAL_clk_InstallSource
(
  HAL_clk_SourceType         eSource,
  HAL_clk_SourceControlType *pControl,
  void                      *pContext
)
{
  /*
   * Sanity.
   */
  if (eSource >= HAL_CLK_NUM_OF_SOURCES)
  {
    return;
  }

  /*
   * Fill in the pointers in the source descriptor.
   */
  HAL_clk.aSourceDesc[eSource].eSource = eSource;
  HAL_clk.aSourceDesc[eSource].pControl = pControl;
  HAL_clk.aSourceDesc[eSource].pContext = pContext;

} /* END HAL_clk_InstallSource */


/* ===========================================================================
**  HAL_clk_InstallPowerDomain
**
** ======================================================================== */

void HAL_clk_InstallPowerDomain
(
  HAL_clk_PowerDomainDescType *pmPowerDomainDesc,
  uint32 nBaseAddress
)
{
  HAL_clk_PowerDomainDescType *pmTmpPowerDomainDesc;

  /*
   * Sanity check
   */
  if(pmPowerDomainDesc == NULL)
  {
    return;
  }

  /*
   * Update power domain descriptor linked list.
   * NOTE: inserting new node at the beginning of the list
   */
  pmTmpPowerDomainDesc =  HAL_clk.pmPowerDomainDesc;
  HAL_clk.pmPowerDomainDesc = pmPowerDomainDesc;
  HAL_clk.pmPowerDomainDesc->nGDSCRAddr += nBaseAddress;
  HAL_clk.pmPowerDomainDesc->pmNextPowerDomain = pmTmpPowerDomainDesc;

  /*
   * Increment power domain count
   */
  ++HAL_clk.nPowerDomains;

} /* END HAL_clk_InstallPowerDomain */


/* ===========================================================================
**  HAL_clk_InstallPowerDomains
**
** ======================================================================== */

void HAL_clk_InstallPowerDomains
(
  HAL_clk_PowerDomainDescType **pmPowerDomainDesc,
  uint32                      nBaseAddress
)
{
  while(*pmPowerDomainDesc != NULL)
  {
    HAL_clk_InstallPowerDomain(*pmPowerDomainDesc, nBaseAddress);
    pmPowerDomainDesc++;
  }
} /* END HAL_clk_InstallPowerDomains */


/* ===========================================================================
**  HAL_clk_InstallClockDomain
**
** ======================================================================== */

void HAL_clk_InstallClockDomain
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  uint32                       nBaseAddress
)
{
  HAL_clk_ClockDomainDescType *pmTmpClockDomainDesc;
  HAL_clk_ClockDescType       *pmTmpClockDesc;
  uint32                       nClockCount = 0;

  /*
   * Sanity check
   */
  if (pmClockDomainDesc == NULL)
  {
    return;
  }

  /*
   * Update clock domain descriptor linked list.
   * NOTE: inserting new node at the beginning of the list
   */
  pmTmpClockDomainDesc =  HAL_clk.pmClockDomainDesc;
  HAL_clk.pmClockDomainDesc = pmClockDomainDesc;

  /*
   * Only add the base address to nCGRAddr if it is non-zero.
   * If nCGRAddr is 0, it should remain 0 to indicate we wish to
   * disable frequency configuration on this clock domain.
   */
  if (HAL_clk.pmClockDomainDesc->nCGRAddr != 0)
  {
    HAL_clk.pmClockDomainDesc->nCGRAddr += nBaseAddress;
  }

  HAL_clk.pmClockDomainDesc->pmNextClockDomain = pmTmpClockDomainDesc;

  /*
   * Count number of clocks in this domain and optionally add the
   * base address to any registers.
   */
  while (nClockCount < pmClockDomainDesc->nClockCount)
  {
    pmTmpClockDesc = &(HAL_clk.pmClockDomainDesc->pmClocks[nClockCount]);

    if (pmTmpClockDesc->mRegisters.nBranchAddr!= 0)
    {
      pmTmpClockDesc->mRegisters.nBranchAddr += nBaseAddress;
    }
    if (pmTmpClockDesc->mRegisters.nResetAddr != 0)
    {
      pmTmpClockDesc->mRegisters.nResetAddr += nBaseAddress;
    }
    if (pmTmpClockDesc->mRegisters.mHWVotable.nAddr != 0)
    {
      pmTmpClockDesc->mRegisters.mHWVotable.nAddr += nBaseAddress;
    }

    /*
     * Increment clock count
     */
    nClockCount++;
  }

  /*
   * Update global clock count
   */
  HAL_clk.nClocks += nClockCount;

  /*
   * Increment clock domain count
   */
  HAL_clk.nClockDomains++;

} /* END HAL_clk_InstallClockDomain */


/* ===========================================================================
**  HAL_clk_InstallClockDomains
**
** ======================================================================== */

void HAL_clk_InstallClockDomains
(
  HAL_clk_ClockDomainDescType **pmClockDomainDesc,
  uint32                      nBaseAddress
)
{
  while(*pmClockDomainDesc != NULL)
  {
    HAL_clk_InstallClockDomain(*pmClockDomainDesc, nBaseAddress);
    pmClockDomainDesc++;
  }
} /* END HAL_clk_InstallClockDomains */


/* ===========================================================================
**  HAL_clk_InstallDebugDesc
**
** ======================================================================== */

void HAL_clk_InstallDebugDesc 
(
  HAL_clk_DebugDescType *pmDebugDesc 
)
{
  HAL_clk_DebugDescType *pmTmpDebugDesc;

  /*
   * Sanity check
   */
  if(pmDebugDesc == NULL)
  {
    return;
  }

  /*
   * Update debug domain descriptor linked list.
   * NOTE: inserting new node at the beginning of the list
   */
  pmTmpDebugDesc =  HAL_clk.pmDebugDesc;
  HAL_clk.pmDebugDesc = pmDebugDesc;
  HAL_clk.pmDebugDesc->pmNextDebugDesc = pmTmpDebugDesc;

} /* END HAL_clk_InstallDebugDesc */


/* ===========================================================================
**  HAL_clk_Init
**
** ======================================================================== */

void HAL_clk_Init
(
  HAL_clk_ContextType *pHALClkCtxt
)
{
  HAL_clk_InitFuncType fInit;
  uint32 n = 0;

  /*
   * Sanity check
   */
  if (pHALClkCtxt != NULL)
  {
     /*
      * Save the busy wait function pointer
      */
     HAL_clk.mHALClkCtxt.BusyWait = pHALClkCtxt->BusyWait;

     HAL_clk.mHALClkCtxt.nChipVersion = pHALClkCtxt->nChipVersion;

     HAL_clk.mHALClkCtxt.nFamily = pHALClkCtxt->nFamily;
  }

  /*
   * Init software state.
   */
  if (HAL_clk_Platform.afInitFuncs != NULL)
  {
    while ((fInit = HAL_clk_Platform.afInitFuncs[n++]) != NULL)
    {
      fInit();
    }
  }

} /* END HAL_clk_Init */


/* ===========================================================================
**  HAL_clk_GetHWIOBases
**
** ======================================================================== */

void HAL_clk_GetHWIOBases
(
  HAL_clk_HWIOBaseType **paHWIOBases
)
{
  *paHWIOBases = HAL_clk_Platform.aHWIOBases;

} /* END HAL_clk_GetHWIOBases */


/* ===========================================================================
**  HAL_clk_GetChipVersion
**
** ======================================================================== */

uint32 HAL_clk_GetChipVersion ( void )
{
  return HAL_clk.mHALClkCtxt.nChipVersion;

} /* END HAL_clk_GetChipVersion */


/* ===========================================================================
**  HAL_clk_GetChipFamily
**
** ======================================================================== */

uint32 HAL_clk_GetChipFamily ( void )
{
  return HAL_clk.mHALClkCtxt.nFamily;

} /* END HAL_clk_GetChipFamily */


/* ===========================================================================
**  HAL_clk_GetTestClockId
**
** ======================================================================== */

uint32 HAL_clk_GetTestClockId
(
  HAL_clk_ClockHandleType pClockHandle
)
{
  HAL_clk_ClockDescType *pmClockDesc = (HAL_clk_ClockDescType *)pClockHandle;

  if ( pmClockDesc != NULL )
  {
    return pmClockDesc->nTestClock;
  }
  else
  {
    return 0;
  }

} /* END HAL_clk_GetTestClockId */
