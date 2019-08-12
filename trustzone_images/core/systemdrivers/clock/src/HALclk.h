/*
===========================================================================
*/
/**
  @file HALclk.h

*/
/*
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/clock/src/HALclk.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $
  ====================================================================
*/

#ifndef __HALCLK_H__
#define __HALCLK_H__

/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Source enumeration.
 */
typedef enum
{
  HAL_CLK_SOURCE_XO = 0,
  HAL_CLK_SOURCE_GPLL0 = 1,
  HAL_CLK_SOURCE_TOTAL = 2
} HAL_clk_SourceType;

/*
 * HAL_clk_ClockMuxConfigType
 *
 * Parameters used for configuring a standard clock multiplexer.
 *
 *  nSource  - The source to use.
 *  nDiv2x   - The integer (2 * divider) value to use.
 *  nM       - The M value for any M/N counter, or 0 to bypass.
 *  nN       - The N value for any M/N counter.
 *  n2D      - Twice the D value for any M/N counter.
 */
typedef struct
{
  HAL_clk_SourceType eSource;
  uint32 nDiv2x;
  uint32 nM;
  uint32 nN;
  uint32 n2D;
} HAL_clk_ClockMuxConfigType;

/*
 * HAL_clk_SourceDescType
 *
 * nModeAddr - Address of the mode register.
 * nVoteAddr - Address of the vote register.
 * nVoteMask - Mask for the voting register.
 */
typedef struct
{
  uint32 nModeAddr;
  uint32 nVoteMask;
} HAL_clk_SourceDescType;

/*
 * HAL_clk_ClockDomainDescType
 *
 * nCGRAddr - Address of the domain RCGR register.
 */
typedef struct HAL_clk_ClockDomainDescType
{
  uint32 nCGRAddr;
  uint32 *aSourceMap;
} HAL_clk_ClockDomainDescType;

/*
 * HAL_clk_ClockDescType
 *
 * nCBCRAddr - Address of the clock CBCR register.
 * nVoteMask - Mask for the voting registers.
 * bPrevEnabled - Used to save the clock enable state before it is enabled.
 */
typedef struct HAL_clk_ClockDescType
{
  uint32 nCBCRAddr;
  uint32 nVoteMask;
  uint32 bPrevEnabled;
} HAL_clk_ClockDescType;

/*
 * HAL_clk_PowerDomainDesc
 *
 * nGDSCRAddr - Address of the power domain GDSCR register.
 * bPrevEnabled - Used to save the power domain enable state before it
 *                is enabled.
 */
typedef struct HAL_clk_PowerDomainDesc
{
  uint32 nGDSCRAddr;
  uint32 bPrevEnabled;
} HAL_clk_PowerDomainDescType;


/*=========================================================================
      Interface Definitions
==========================================================================*/

/* =========================================================================
**  Function : HAL_clk_EnableClock
** =========================================================================*/
/**
  Enable a clock.
*/

void HAL_clk_EnableClock
(
  HAL_clk_ClockDescType *pClock
);


/* =========================================================================
**  Function : HAL_clk_DisableClock
** =========================================================================*/
/**
  Disable a clock.
*/

void HAL_clk_DisableClock
(
  HAL_clk_ClockDescType *pClock
);


/* =========================================================================
**  Function : HAL_clk_WaitForClockOn
** =========================================================================*/
/**
  Wait for clock to turn on.
*/

boolean HAL_clk_WaitForClockOn
(
  HAL_clk_ClockDescType *pClock
);


/* =========================================================================
**  Function : HAL_clk_ConfigClockMux
** =========================================================================*/
/**
  Configure a clock mux.
*/

void HAL_clk_ConfigClockMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
);


/* =========================================================================
**  Function : HAL_clk_EnableSource
** =========================================================================*/
/**
  Enable a clock.
*/

void HAL_clk_EnableSource
(
  HAL_clk_SourceDescType *pSource
);


/* =========================================================================
**  Function : HAL_clk_DisableSource
** =========================================================================*/
/**
  Disable a clock.
*/

void HAL_clk_DisableSource
(
  HAL_clk_SourceDescType *pSource
);


/* =========================================================================
**  Function : HAL_clk_EnablePowerDomain
** =========================================================================*/
/**
  Enable a power domain.
*/

void HAL_clk_EnablePowerDomain
(
  HAL_clk_PowerDomainDescType *pPowerDomain
);


/* =========================================================================
**  Function : HAL_clk_DisablePowerDomain
** =========================================================================*/
/**
  Disable a power domain.
*/

void HAL_clk_DisablePowerDomain
(
  HAL_clk_PowerDomainDescType *pPowerDomain
);


/* =========================================================================
**  Function : HAL_clk_WaitForPowerDomainOn
** =========================================================================*/
/**
  Wait for power domain to turn on.
*/

boolean HAL_clk_WaitForPowerDomainOn
(
  HAL_clk_PowerDomainDescType *pPowerDomain
);

#endif /* __HALCLK_H__ */

