/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock regime bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/config/msm8909/ClockBSP.c#2 $

when       who     what, where, why

10/24/12   kma     Ported from 8x26 project.

==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
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
    /* .HALConfig.nL       = */   41,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   1,
    /* .HALConfig.nAlpha   = */   0xAA000000,
    /* .HALConfig.nAlphaU  = */   0xAA,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 800000 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* GPLL1 - General purpose PLL                                           */
  /*-----------------------------------------------------------------------*/

  {
    /* .eSource            = */ HAL_CLK_SOURCE_GPLL1,
    /* .HALConfig            */ {
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO1,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   2,
    /* .HALConfig.nL       = */   64,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   1,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 614400 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* BIMC_PLL                                                              */
  /*-----------------------------------------------------------------------*/

  {
    /* .eSource            = */ HAL_CLK_SOURCE_BIMCPLL,
    /* .HALConfig            */ {
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO2,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   1,
    /* .HALConfig.nL       = */   55,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   1,
    /* .HALConfig.nAlpha   = */   0x55000000,
    /* .HALConfig.nAlphaU  = */   0x85,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 1066000 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  { HAL_CLK_SOURCE_NULL }

};


/* =========================================================================
**    nFreqHz       { eSource, nDiv2x, nM, nN, n2D },      eVRegLevel
** =========================================================================*/

const ClockMuxConfigType RPMClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 177780000, { HAL_CLK_SOURCE_GPLL0,  9, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * System NOC clock configurations
 */
const ClockMuxConfigType SystemNOCClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  50000000, { HAL_CLK_SOURCE_GPLL0, 32, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 133330000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 160000000, { HAL_CLK_SOURCE_GPLL0, 10, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  8, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 266670000, { HAL_CLK_SOURCE_GPLL0,  6, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
};

/*
 * Peripheral Config NOC clock configurations.
 */
const ClockMuxConfigType PCNOClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  50000000, { HAL_CLK_SOURCE_GPLL0, 32, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 133330000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
};

#if 0
/*
  For now we don't need the below PLL Configurations for BIMC
  as we have no plans to re-configure the BIMC PLL.
  But in future we have 8936 coming with LPDDR3 @ 800MHz.
  So we may have to accomodate more Pef Levels with BIMC_PLL
  Hence this is only a place holder for now
*/
/*
 * BIMC configurations. The PLL number is changed at runtime.
 */
static ClockSourceConfigType BIMCPLLConfig[] =
{
  /* ============================================================================================================
  **  eSource, { eSource, eVCO, nPreDiv, nPostDiv, nL, nM, nN }, nConfigMask, nFreqHz, eVRegLevel, bInit, bOwner
  ** ============================================================================================================*/
  { HAL_CLK_SOURCE_RAW1, { HAL_CLK_SOURCE_XO, HAL_CLK_PLL_VCO1, 1, 1, 20,  5,  6 }, CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,  400000000}, /* Index =  0 */
  { HAL_CLK_SOURCE_RAW1, { HAL_CLK_SOURCE_XO, HAL_CLK_PLL_VCO1, 1, 1, 27, 17, 24 }, CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,  532000000}, /* Index =  1 */
  { HAL_CLK_SOURCE_RAW1, { HAL_CLK_SOURCE_XO, HAL_CLK_PLL_VCO1, 1, 1, 34, 13, 18 }, CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,  666667000}, /* Index =  1 */
};
#endif

/*
 * BIMC clock configurations.
 * Program Dividers for 2x Clock
 */
const ClockMuxConfigType BIMCClockConfig[] =
{
  {   9600000, { HAL_CLK_SOURCE_XO,     1, 1,  1, 1 }, CLOCK_VREG_LEVEL_LOW },
  {  50000000, { HAL_CLK_SOURCE_GPLL0, 16, 1,  1, 1 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0,  8, 1,  1, 1 }, CLOCK_VREG_LEVEL_LOW },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  4, 1,  1, 1 }, CLOCK_VREG_LEVEL_LOW },
  { 400000000, { HAL_CLK_SOURCE_GPLL0,  2, 1,  1, 1 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 533000000, { HAL_CLK_SOURCE_BIMCPLL,  2, 1,  1, 1 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
};

/*
 * BIMC GPU clock configurations.
 */
const ClockMuxConfigType BIMCGPUClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,    1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  50000000, { HAL_CLK_SOURCE_GPLL0,32, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  50000000, { HAL_CLK_SOURCE_GPLL0,32, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0,16, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,8, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 266670000, { HAL_CLK_SOURCE_GPLL0,6, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
};


/* 
 * gcc_apss_tcu_async_clk BSP configurations
 * This clock is scaled with BIMC according to the BIMC voltage level.
 * No separate NPA node for this and hence needs to be taken care of along with BIMC.
 */
const ClockMuxConfigType APSSTCUASYNCClockConfig[] =
{
   { 19200000, { HAL_CLK_SOURCE_XO,      1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
   {160000000, { HAL_CLK_SOURCE_GPLL0,  10, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
   {160000000, { HAL_CLK_SOURCE_GPLL0,  10, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
   {160000000, { HAL_CLK_SOURCE_GPLL0,  10, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
   {320000000, { HAL_CLK_SOURCE_GPLL0,   5, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL }, 
   {400000000, { HAL_CLK_SOURCE_GPLL0,   4, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
   { 0 }
};  
/*
 * QDSS AT clock configurations.
 */
const ClockMuxConfigType QDSSATClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  88890000, { HAL_CLK_SOURCE_GPLL0, 18, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 177780000, { HAL_CLK_SOURCE_GPLL0,  9, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * QDSS Trace clock configurations.
 * Using GPLL0 we can get 160/320 but it would need fractional dividers
 * which causes large jitter. This is jitter sensitive path having half cycle paths, 
 * i.e. flop - flop data should travel in half clock period
 * Hence we should not use GPLL0
 */
const ClockMuxConfigType QDSSTraceClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 153600000, { HAL_CLK_SOURCE_GPLL1, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 307200000, { HAL_CLK_SOURCE_GPLL1,  8, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * QDSS STM clock configurations.
 */
const ClockMuxConfigType QDSSSTMClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 100000000, { HAL_CLK_SOURCE_GPLL0, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 200000000, { HAL_CLK_SOURCE_GPLL0,  8, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 266670000, { HAL_CLK_SOURCE_GPLL0,	6, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
};

/*
 * QDSS TSCTR Div-2 clock configurations.
 * The divider configs correspond to Div1 because 
 * there is a static divider of 2 after the mux. So 
 * below frequencies match with div2 of the freq after mux.
 */
const ClockMuxConfigType QDSSTSCTRDiv2ClockConfig[] =
{
  {  19200000, { HAL_CLK_SOURCE_XO,    1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 153600000, { HAL_CLK_SOURCE_GPLL1, 8, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 307200000, { HAL_CLK_SOURCE_GPLL1, 4, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * RBCPR clock configurations.
 */
const ClockMuxConfigType RBCPRClockConfig[] =
{
  { 19200000, { HAL_CLK_SOURCE_XO,     1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 50000000, { HAL_CLK_SOURCE_GPLL0, 32, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};


/*
 * SPMI AHB clock configurations.
 */
const ClockMuxConfigType SPMIAHBClockConfig[] =
{
  { 50000000, { HAL_CLK_SOURCE_GPLL0, 32, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {100000000, { HAL_CLK_SOURCE_GPLL0, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
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
 * QPIC clock configurations. 
 */
const ClockMuxConfigType QPICClockConfig[] =
{
  { 19200000, { HAL_CLK_SOURCE_XO,  1, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 50000000, { HAL_CLK_SOURCE_GPLL0, 32, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {100000000, { HAL_CLK_SOURCE_GPLL0, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  {133000000, { HAL_CLK_SOURCE_GPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_HIGH },
  { 0 }
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

/*
 * Mapping for Vreg and Railway driver levels.
 *
 */

rail_voltage_level ClockVregRailMapConfig[CLOCK_VREG_NUM_LEVELS] = 
{
    RAIL_VOLTAGE_LEVEL_OFF,           /*CLOCK_VREG_LEVEL_OFF, index             0*/
    RAIL_VOLTAGE_LEVEL_SVS,           /*CLOCK_VREG_LEVEL_LOW, index             1*/
    RAIL_VOLTAGE_LEVEL_SVS_HIGH,      /*CLOCK_VREG_LEVEL_LOW_PLUS, index        2*/
    RAIL_VOLTAGE_LEVEL_NOMINAL,       /*CLOCK_VREG_LEVEL_NOMINAL, index         3*/
    RAIL_VOLTAGE_LEVEL_NOMINAL_HIGH,  /*CLOCK_VREG_LEVEL_NOMINAL_PLUS, index    4*/
    RAIL_VOLTAGE_LEVEL_TURBO,         /*CLOCK_VREG_LEVEL_HIGH, index            5*/
};

