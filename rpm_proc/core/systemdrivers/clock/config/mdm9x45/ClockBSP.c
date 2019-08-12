/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock regime bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/config/mdm9x45/ClockBSP.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/14/14   pbitra     Ported from 9x35, updated for 9x45.

==============================================================================
            Copyright (c) 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBSP.h"


/*=========================================================================
      Data Declarations
==========================================================================*/

/*
 * Clock source configuration data.
 */
const ClockSourceConfigType SourceConfig[] =
{

  /*-----------------------------------------------------------------------*/
  /* XO                                                                    */
  /*-----------------------------------------------------------------------*/

  {
    /* .eSource            = */ HAL_CLK_SOURCE_XO,
    /* .HALConfig          = */ { HAL_CLK_SOURCE_NULL },
    /* .nConfigMask        = */ 0,
    /* .nFreqHz            = */ 19200 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* GPLL0 - General purpose PLL                                           */
  /*-----------------------------------------------------------------------*/

  {
    /* .eSource            = */ HAL_CLK_SOURCE_GPLL0,
    /* .HALConfig            */ {
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO2,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   1,
    /* .HALConfig.nL       = */   31,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   0,
    /* .HALConfig.nAlpha   = */   0,
    /* .HALConfig.nAlphaU  = */   0x40,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 600000 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* GPLL2 - BIMC PLL                                                      */
  /*-----------------------------------------------------------------------*/

  {
    /* .eSource            = */ HAL_CLK_SOURCE_GPLL2,
    /* .HALConfig            */ {
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO2,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   1,
    /* .HALConfig.nL       = */   42,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   0,
    /* .HALConfig.nAlpha   = */   0xAAAAAAAA,
    /* .HALConfig.nAlphaU  = */   0x1A,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 808400 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* GPLL3 - BIMC PLL                                                      */
  /*-----------------------------------------------------------------------*/

  {
    /* .eSource            = */ HAL_CLK_SOURCE_GPLL3,
    /* .HALConfig            */ {
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO0,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   1,
    /* .HALConfig.nL       = */   54,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   0,
    /* .HALConfig.nAlpha   = */   0,
    /* .HALConfig.nAlphaU  = */   0,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 1036800 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  { HAL_CLK_SOURCE_NULL }

};


/* =========================================================================
**    nFreqHz       { eSource, nDiv2x, nM, nN, n2D },      eVRegLevel
** =========================================================================*/

/*
 * RPM clock configurations.
 */
const ClockMuxConfigType RPMClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 171430000, { HAL_CLK_SOURCE_GPLL0,  7, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  6, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 240000000, { HAL_CLK_SOURCE_GPLL0,  5, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 300000000, { HAL_CLK_SOURCE_GPLL0,  4, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
};

/*
 * System NOC clock configurations
 */
const ClockMuxConfigType SystemNOCClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  50000000, { HAL_CLK_SOURCE_GPLL0, 24, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  85710000, { HAL_CLK_SOURCE_GPLL0, 14, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 150000000, { HAL_CLK_SOURCE_GPLL0,  8, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  6, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
};

/*
 * Peripheral Config NOC clock configurations.
 */
const ClockMuxConfigType PCNOClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  50000000, { HAL_CLK_SOURCE_GPLL0, 24, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 133330000, { HAL_CLK_SOURCE_GPLL0,  9, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
};

/*
 * BIMC clock configurations.
 */
const ClockMuxConfigType BIMCClockConfig[] =
{
  {   9600000, { HAL_CLK_SOURCE_XO,    1, 1,  1, 1 }, CLOCK_VREG_LEVEL_LOW      },
  {  50000000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW      },
  {  75000000, { HAL_CLK_SOURCE_GPLL0,  8, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW      },
  { 100000000, { HAL_CLK_SOURCE_GPLL0,  6, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW      },
  { 150000000, { HAL_CLK_SOURCE_GPLL0,  4, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW      },
  { 202100000, { HAL_CLK_SOURCE_GPLL2,  4, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW      },
  { 259200000, { HAL_CLK_SOURCE_GPLL3,  4, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL  },
  { 300000000, { HAL_CLK_SOURCE_GPLL0,  2, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL   },
  { 404200000, { HAL_CLK_SOURCE_GPLL2,  2, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL  }, 
  { 518400000, { HAL_CLK_SOURCE_GPLL3,  2, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH     }, 
  { 0 }
};


/*
 * QPIC clock configurations.
 */
const ClockMuxConfigType QPICClockConfig[] =
{
  {  50000000, { HAL_CLK_SOURCE_GPLL0, 24, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW      },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL, },
  { 0 }
};


/*
 * QDSS AT clock configurations.
 */
const ClockMuxConfigType QDSSATClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  6, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * QDSS Trace clock configurations.
 */
const ClockMuxConfigType QDSSTraceClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 150000000, { HAL_CLK_SOURCE_GPLL0,  8, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  6, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * QDSS STM clock configurations.
 */
const ClockMuxConfigType QDSSSTMClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  6, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * QDSS TSCTR Div-2 clock configurations.
 */
const ClockMuxConfigType QDSSTSCTRDiv2ClockConfig[] =
{
  {  9600000, { HAL_CLK_SOURCE_XO,    1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 150000000, { HAL_CLK_SOURCE_GPLL0, 4, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 300000000, { HAL_CLK_SOURCE_GPLL0, 2, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * RBCPR clock configurations.
 */
const ClockMuxConfigType RBCPRClockConfig[] =
{
  { 19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 50000000, { HAL_CLK_SOURCE_GPLL0, 24, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};

/*
 * RBCPR MX clock configurations.
 */
const ClockMuxConfigType RBCPRMXClockConfig[] =
{
  { 19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 50000000, { HAL_CLK_SOURCE_GPLL0, 24, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};


/*
 * SPMI AHB clock configurations. 
 */
const ClockMuxConfigType SPMIAHBClockConfig[] =
{  
  {  50000000, { HAL_CLK_SOURCE_GPLL0, 24, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0,  12, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * SPMI SER clock configurations. 
 */
const ClockMuxConfigType SPMISERClockConfig[] =
{
  { 19200000, { HAL_CLK_SOURCE_XO,  1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};

/*
 * IPA clock configurations. 
 */
const ClockMuxConfigType IPAClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  75000000, { HAL_CLK_SOURCE_GPLL0, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  85710000, { HAL_CLK_SOURCE_GPLL0, 14, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 150000000, { HAL_CLK_SOURCE_GPLL0,  8, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 171430000, { HAL_CLK_SOURCE_GPLL0,  7, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  6, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  {  0 }
};

/*
 * CE (Crypto Engine) clock configurations.
 */
const ClockMuxConfigType CEClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     2, 0,  0, 0 }, CLOCK_VREG_LEVEL_LOW     },
  {  42860000, { HAL_CLK_SOURCE_GPLL0, 28, 0,  0, 0 }, CLOCK_VREG_LEVEL_LOW     }, 
  {  85710000, { HAL_CLK_SOURCE_GPLL0, 14, 0,  0, 0 }, CLOCK_VREG_LEVEL_LOW     },
  { 171430000, { HAL_CLK_SOURCE_GPLL0,  7, 0,  0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  6, 0,  0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 },
};

/*
 * Clock Log Default Configuration.
 *
 * NOTE: An .nGlobalLogFlags value of 0x12 will log only clock frequency
 *       changes and source state changes by default.
 */
const ClockLogType ClockLogDefaultConfig[] =
{
  {
    /* .nLogSize        = */ 4096,
    /* .nGlobalLogFlags = */ 0x12
  }
};
