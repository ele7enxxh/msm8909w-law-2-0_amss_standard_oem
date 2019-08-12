#ifndef __CLOCKBSP_H__
#define __CLOCKBSP_H__
/*
===========================================================================
*/
/**
  @file ClockBSP.h

  This file contains definitions for the clock BSP data.
*/
/*
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/clock/src/ClockBSP.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "HALclk.h"
#include "com_dtypes.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

/*
 * Get a clock vote mask.
 */
#define VOTE_MASK(x) HWIO_FMSK(PROC_CLK_BRANCH_ENA_VOTE, x)
#define PLL_VOTE_MASK(x) HWIO_FMSK(PROC_CLK_PLL_ENA_VOTE, x)

/*
 * Clock RPM type strings (little endian).
 */
#define CLOCK_RPM_TYPE_CLK0 0x306b6c63  /* clk0 */
#define CLOCK_RPM_TYPE_CLK1 0x316b6c63  /* clk1 */
#define CLOCK_RPM_TYPE_CLK2 0x326b6c63  /* clk2 */
#define CLOCK_RPM_TYPE_CE   0x6563      /* ce */

/*
 * Clock RPM unit strings (little endian).
 */
#define CLOCK_RPM_UNIT_KHZ      0x007a484b    /* KHz */
#define CLOCK_RPM_UNIT_ENABLE   0x62616e45    /* Enab(le) */
#define CLOCK_RPM_UNIT_STATE    0x54415453    /* STAT(E) */

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * ClockVRegLevelType
 */
typedef enum
{
  CLOCK_VREG_LEVEL_OFF          = 0,
  CLOCK_VREG_LEVEL_LOW          = 3,
  CLOCK_VREG_LEVEL_NOMINAL      = 4,
  CLOCK_VREG_LEVEL_NOMINAL_PLUS = 5,
  CLOCK_VREG_LEVEL_HIGH         = 6,
  CLOCK_VREG_NUM_LEVELS         = 7
} ClockVRegLevelType;

/*
 * ClockRPMResource
 *
 * szName - Name of the RPM resource.
 * nRPMType - RPM resource type.
 * nRPMId - RPM resource id.
 */
typedef struct ClockRPMResource
{
  const char *szName;
  uint32      nUnits;
  uint32      nRPMType;
  uint32      nRPMId;
} ClockRPMResource;

/*
 * ClockMuxConfigType
 *
 * nFreqHz - Frequency in Hz.
 * HALConfig - HAL mux configuration.
 * eVRegLevel - Voltage requirement.
 */
typedef struct
{
  uint32                      nFreqHz;
  HAL_clk_ClockMuxConfigType  HALConfig;
  ClockVRegLevelType          eVRegLevel;
} ClockMuxConfigType;

/*
 * Clocksource
 *
 * HALSource - HAL source configuration.
 * nReferenceCount - Reference count.
 */
typedef struct ClockSource ClockSource;
struct ClockSource
{
  HAL_clk_SourceDescType   HALSource;
  uint32                   nReferenceCount;
  ClockRPMResource        *pRPM;
  ClockSource             *pSource;
};

/*
 * ClockDomain
 *
 * HALClockDomain - Clock domain HAL structure.
 * aBSPConfig - Array of frequency configurations.
 * pActiveConfig - Active frequency configuration.
 * pSource - Domain source.
 * nReferenceCount - Reference count.
 */
typedef struct ClockDomain
{
  HAL_clk_ClockDomainDescType  HALClockDomain;
  ClockMuxConfigType          *aBSPConfig;
  ClockMuxConfigType          *pActiveConfig;
  ClockSource                 *pSource;
  uint32                       nReferenceCount;
} ClockDomain;

/*
 * Clock
 *
 * szName - Clock name.
 * HALClock - Clock HAL structure.
 * pDomain - Pointer to the clock's domain.
 * nReferenceCount - Reference count.
 */
typedef struct Clock
{
  char                  *szName;
  HAL_clk_ClockDescType  HALClock;
  ClockDomain           *pDomain;
  uint32                 nReferenceCount;
} Clock;

/*
 * ClockGroup
 *
 * aClocks - Array of clocks in the clock group.
 * aPowerDomains - Array of power domains in the clock group.
 */
typedef struct ClockGroup
{
  HAL_clk_ClockDescType       *aClocks;
  HAL_clk_PowerDomainDescType *aPowerDomains;
  HAL_clk_ClockDescType       *aAccessClocks;
  uint32                       nReferenceCount;
} ClockGroup;

/*
 * ClockTZBSP
 *
 * Main BSP data structure.
 */
typedef struct ClockTZBSP
{
  ClockRPMResource  *aRPMResources;
  ClockGroup        *aClockGroups;
  Clock             *aClocks;
  ClockSource       *aSources;
} ClockTZBSP;

#endif /* __CLOCKBSP_H__ */

