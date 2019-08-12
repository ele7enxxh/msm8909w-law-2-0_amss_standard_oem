/*
===========================================================================
*/
/**
  @file ClockMSSBSP.c 
  
  BSP data for the MSS clock driver.
*/
/*  
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/config/msm8909w/ClockMSSBSP.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  09/23/13   stu     Branched from 8x62 code base.
  10/19/11   vs      Created.
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


#include "ClockMSSBSP.h"
#include "comdef.h"
#include "VCSDefs.h"


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Enumeration of QDSP6 performance levels.
 */
enum
{
  CLOCK_QDSP6_PERF_LEVEL_0,
  CLOCK_QDSP6_PERF_LEVEL_1,
  CLOCK_QDSP6_PERF_LEVEL_2,
  CLOCK_QDSP6_PERF_LEVEL_3,
  CLOCK_QDSP6_PERF_LEVEL_4,
  CLOCK_QDSP6_PERF_LEVEL_5,
  CLOCK_QDSP6_PERF_LEVEL_6,
  CLOCK_QDSP6_PERF_LEVEL_7,
  CLOCK_QDSP6_PERF_LEVEL_TOTAL
};


/*
 * Enumeration of QDSP6 configurations.
 */
enum
{
  CLOCK_QDSP6_CONFIG_XO,
  CLOCK_QDSP6_CONFIG_115P20MHz,
  CLOCK_QDSP6_CONFIG_144MHz,
  CLOCK_QDSP6_CONFIG_172P8MHz,
  CLOCK_QDSP6_CONFIG_230P4MHz_V1,
  CLOCK_QDSP6_CONFIG_230P4MHz,
  CLOCK_QDSP6_CONFIG_288MHz_V1,
  CLOCK_QDSP6_CONFIG_288MHz,
  CLOCK_QDSP6_CONFIG_345P6MHz,
  CLOCK_QDSP6_CONFIG_384MHz_V1S0,
  CLOCK_QDSP6_CONFIG_384MHz_V1,
  CLOCK_QDSP6_CONFIG_384MHz,
  CLOCK_QDSP6_CONFIG_441P6MHz,
  CLOCK_QDSP6_CONFIG_499P2MHz,
  CLOCK_QDSP6_CONFIG_537P6MHz,
  CLOCK_QDSP6_CONFIG_576MHz_V1,
  CLOCK_QDSP6_CONFIG_576MHz,
  CLOCK_QDSP6_CONFIG_595P2MHz,
  CLOCK_QDSP6_CONFIG_633P60MHz,
  CLOCK_QDSP6_CONFIG_691P20MHz,
  CLOCK_QDSP6_CONFIG_TOTAL
};


/*
 * Enumeration of MSS Config Bus performance levels.
 */
enum
{
  CLOCK_CONFIG_BUS_PERF_LEVEL_0,
  CLOCK_CONFIG_BUS_PERF_LEVEL_1,
  CLOCK_CONFIG_BUS_PERF_LEVEL_2,
  CLOCK_CONFIG_BUS_PERF_LEVEL_TOTAL
};


/*
 * Enumeration of MSS Config Bus configurations.
 */
enum
{
  CLOCK_CONFIG_BUS_CONFIG_XO,
  CLOCK_CONFIG_BUS_CONFIG_72MHz,
  CLOCK_CONFIG_BUS_CONFIG_144MHz,
  CLOCK_CONFIG_BUS_CONFIG_TOTAL
};


/*=========================================================================
      Data
==========================================================================*/

/*
 * The MPLL2 source frequency configurations are defined in ClockBSP.c
 */
extern ClockSourceFreqConfigType SourceFreqConfig_MPLL2[4];

/*
 * QDSP6 performance levels.
 */

static uint32 Clock_CPUPerfLevels_v1s0[] =
{
  CLOCK_QDSP6_CONFIG_XO,
  CLOCK_QDSP6_CONFIG_115P20MHz,
  CLOCK_QDSP6_CONFIG_144MHz,
  CLOCK_QDSP6_CONFIG_230P4MHz_V1,
  CLOCK_QDSP6_CONFIG_288MHz_V1,
  CLOCK_QDSP6_CONFIG_384MHz_V1S0,
  CLOCK_QDSP6_CONFIG_499P2MHz
};

/*
 * QDSP6 performance levels.
 */
static uint32 Clock_CPUPerfLevels_v1s1[] =
{
  CLOCK_QDSP6_CONFIG_XO,
  CLOCK_QDSP6_CONFIG_115P20MHz,
  CLOCK_QDSP6_CONFIG_144MHz,
  CLOCK_QDSP6_CONFIG_230P4MHz,
  CLOCK_QDSP6_CONFIG_288MHz_V1,
  CLOCK_QDSP6_CONFIG_384MHz_V1,
  CLOCK_QDSP6_CONFIG_576MHz_V1,
  CLOCK_QDSP6_CONFIG_595P2MHz
};

/*
 * QDSP6 performance levels.
 */
static uint32 Clock_CPUPerfLevels_v1s2[] =
{
  CLOCK_QDSP6_CONFIG_XO,
  CLOCK_QDSP6_CONFIG_115P20MHz,
  CLOCK_QDSP6_CONFIG_144MHz,
  CLOCK_QDSP6_CONFIG_230P4MHz,
  CLOCK_QDSP6_CONFIG_288MHz_V1,
  CLOCK_QDSP6_CONFIG_384MHz_V1,
  CLOCK_QDSP6_CONFIG_576MHz_V1,
  CLOCK_QDSP6_CONFIG_691P20MHz
};

/*
 * QDSP6 performance levels.
 */
static uint32 Clock_CPUPerfLevels_v2[] =
{
  CLOCK_QDSP6_CONFIG_XO,
  CLOCK_QDSP6_CONFIG_115P20MHz,
  CLOCK_QDSP6_CONFIG_144MHz,
  CLOCK_QDSP6_CONFIG_230P4MHz,
  CLOCK_QDSP6_CONFIG_288MHz,
  CLOCK_QDSP6_CONFIG_384MHz,
  CLOCK_QDSP6_CONFIG_576MHz,
  CLOCK_QDSP6_CONFIG_691P20MHz
};


/*
 * Config Bus performance levels
 */
static uint32 Clock_ConfigBusPerfLevels [] =
{
  CLOCK_CONFIG_BUS_CONFIG_XO,
  CLOCK_CONFIG_BUS_CONFIG_72MHz,
  CLOCK_CONFIG_BUS_CONFIG_144MHz
};


/*
 * Mux configuration for different CPU frequencies.
 */
static ClockCPUConfigType Clock_CPUConfig[] =
{
  /* NOTE: Divider value is (2*Div) due to potential use of fractional values */
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz    = 19200 * 1000,
      .HALConfig  = {HAL_CLK_SOURCE_XO, 2},
      .HWVersion  = {{0x0, 0x0}, {0xFF, 0xFF}},
    },
    .eCornerMSS   = VCS_CORNER_LOW
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 115200 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 10},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[3] // 576.00 MHz
    },
    .eCornerMSS   = VCS_CORNER_LOW
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 144000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 8},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[3] // 576 MHz
    },
    .eCornerMSS   = VCS_CORNER_LOW
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 172800 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 8},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[4] // 691.2 MHz
  },
    .eCornerMSS   = VCS_CORNER_LOW
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
  {
      .nFreqHz           = 230400 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 6},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[4] // 691.20 MHz
    },
    .eCornerMSS   = VCS_CORNER_NOMINAL  //as 8909 V1S0 Q6 SVS Fmax is 182MHz
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
  {
      .nFreqHz           = 230400 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 6},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[4] // 691.20 MHz
    },
    .eCornerMSS   = VCS_CORNER_LOW
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 288000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 4},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[3] // 576 MHz
    },
    .eCornerMSS   = VCS_CORNER_NOMINAL  //as 8909 V1S1 Q6 NOM Fmax is 384MHz 
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 288000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 4},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[3] // 576 MHz
    },
    .eCornerMSS   = VCS_CORNER_LOW
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 345600 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 4},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[4] // 691.2 MHz
  },
    .eCornerMSS   = VCS_CORNER_TURBO
  },
{
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 384000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[0] // 384.0 MHz
  },
    .eCornerMSS   = VCS_CORNER_TURBO  // As 8909 Q6 V1S0 NOMINAL Fmax is 291MHz 
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 384000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[0] // 384.0 MHz
  },
    .eCornerMSS        = VCS_CORNER_NOMINAL       // As 8909 Q6 V1 SVS Fmax is 230MHz and NOM 384MHz  
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 384000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[0] // 384.0 MHz
  },
    .eCornerMSS   = VCS_CORNER_LOW
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 441600 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[1] // 441.6.0 MHz
  },
    .eCornerMSS   = VCS_CORNER_TURBO
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 499200 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[5] // 499.2 MHz
  },
    .eCornerMSS   = VCS_CORNER_TURBO
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
  {
      .nFreqHz           = 537600 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[2] // 537.6 MHz
    },
    .eCornerMSS   = VCS_CORNER_TURBO
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
  {
      .nFreqHz           = 576000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[3] // 576 MHz
    },
    .eCornerMSS   = VCS_CORNER_TURBO // As 8909 Q6 V1 NOMINAL Fmax is 288MHz  
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
  {
      .nFreqHz           = 576000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[3] // 576 MHz
    },
    .eCornerMSS   = VCS_CORNER_NOMINAL
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 595200 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[6] // 595.2 MHz
    },
    .eCornerMSS   = VCS_CORNER_TURBO
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 633600 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[7] // 633.6 MHz
    },
    .eCornerMSS   = VCS_CORNER_TURBO
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 691200 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x0, 0x0}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[4] // 691.2 MHz
    },
    .eCornerMSS   = VCS_CORNER_TURBO
  },
};


/*
 * Performance level configuration data for the CPU clock.
 */
static ClockCPUPerfConfigType Clock_CPUPerfConfig[] =
{
  {
    .HWVersion      = {{0x0, 0x0}, {0x1, 0x1}},
    .nSpeedBinFuse  = 0x0,
    .nMinPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_1,
    .nMaxPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_6,
    .anPerfLevel    = Clock_CPUPerfLevels_v1s0,
    .nNumPerfLevels = ARR_SIZE(Clock_CPUPerfLevels_v1s0)
  },
  {
    .HWVersion      = {{0x0, 0x0}, {0x1, 0x1}},
    .nSpeedBinFuse  = 0x1,
    .nMinPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_1,
    .nMaxPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_6,
    .anPerfLevel    = Clock_CPUPerfLevels_v1s1,
    .nNumPerfLevels = ARR_SIZE(Clock_CPUPerfLevels_v1s1)
  },
  {
    .HWVersion      = {{0x0, 0x0}, {0x1, 0x1}},
    .nSpeedBinFuse  = 0x2,
    .nMinPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_1,
    .nMaxPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_6,
    .anPerfLevel    = Clock_CPUPerfLevels_v1s2,
    .nNumPerfLevels = ARR_SIZE(Clock_CPUPerfLevels_v1s2)
  },
  {
    .HWVersion      = {{0x2, 0x0}, {0xFF, 0xFF}},
    .nSpeedBinFuse  = 0xFF,
    .nMinPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_1,
    .nMaxPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_6,
    .anPerfLevel    = Clock_CPUPerfLevels_v2,
    .nNumPerfLevels = ARR_SIZE(Clock_CPUPerfLevels_v2)
  }
};


/*
 * Mux configuration for different MSS Config Bus frequencies.
 *
 * NOTE:
 *  The HW running off the MSS config bus is powered by /vdd/cx except for
 *  crypto which is powered by /vdd/mss.  The below configurations contain
 *  voltage requirements for both power domains to support all HW running
 *  off the MSS config bus.
 */
static ClockConfigBusConfigType Clock_ConfigBusConfig[] =
{
  /*  NOTE: Divider value is (2*Div) due to potential use of fractional values */
  {
    /* .Mux */   { 19200 * 1000, { HAL_CLK_SOURCE_XO, 2 }, VCS_CORNER_LOW }
  },
  {
    /* .Mux */   { 72000 * 1000, { HAL_CLK_SOURCE_MPLL1, 16 }, VCS_CORNER_LOW }
  },
  {
    /* .Mux */   { 144000 * 1000, { HAL_CLK_SOURCE_MPLL1, 8 }, VCS_CORNER_NOMINAL }
  }
};

/*
 * Performance level configuration data for the MSS Config Bus.
 */
static ClockConfigBusPerfConfigType Clock_ConfigBusPerfConfig =
{
  .nMinPerfLevel = CLOCK_CONFIG_BUS_PERF_LEVEL_0,
  .nMaxPerfLevel = CLOCK_CONFIG_BUS_PERF_LEVEL_2,
  .anPerfLevel   = Clock_ConfigBusPerfLevels
};


/*
 * Image BSP data
 */
const ClockImageBSPConfigType ClockImageBSPConfig =
{
  .bEnableDCS              = TRUE,
  .pCPUConfig              = Clock_CPUConfig,
  .pCPUPerfConfig          = Clock_CPUPerfConfig,
  .nNumCPUPerfLevelConfigs = ARR_SIZE(Clock_CPUPerfConfig),
  .pConfigBusConfig        = Clock_ConfigBusConfig,
  .pConfigBusPerfConfig    = &Clock_ConfigBusPerfConfig
};
