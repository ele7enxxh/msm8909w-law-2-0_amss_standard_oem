/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/config/msm8909w/ClockBSP.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
06/05/13   vt      Init SPMI clocks to INTERNAL_CONTROL 
10/01/12   ll      Added PLL multiplier settings
09/11/12   ll      Added Enable Early Out flag to MPLL2
10/19/11   vs      Created. 

==============================================================================
            Copyright (c) 2011 Qualcomm Technologies Incorporated.
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
 *  SourceFreqConfig_XO
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_XO[] =
{
  {
    .nFreqHz    = 19200 * 1000,
    .HALConfig  = { HAL_CLK_SOURCE_NULL },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0, 0}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_GPLL0
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL0[] =
{
  {
    .nFreqHz    = 800000 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_XO,
      .eVCO           = HAL_CLK_PLL_VCO2,
      .nPreDiv        = 1,
      .nPostDiv       = 1,
      .nL             = 41,
      .nM             = 0,
      .nN             = 0,
      .nVCOMultiplier = 0,
      .nAlpha         = 0xaa000000,
      .nAlphaU        = 0xaa,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0, 0}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_GPLL1
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL1[] =
{
  {
    .nFreqHz    = 1228800 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_XO,
      .eVCO           = HAL_CLK_PLL_VCO1,
      .nPreDiv        = 1,
      .nPostDiv       = 2,
      .nL             = 64,
      .nM             = 0,
      .nN             = 1,
      .nVCOMultiplier = 0, /* Bypass multiplier for SR PLLs*/
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0, 0}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};

/*
 *  SourceFreqConfig_MPLL1
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_MPLL1[] =
{
  {
    .nFreqHz    = 576000 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_XO,
      .eVCO           = HAL_CLK_PLL_VCO1,
      .nPreDiv        = 1,
      .nPostDiv       = 2,
      .nL             = 60,
      .nM             = 0,
      .nN             = 0, /* Modem PBL is configuring this to 0 */
      .nVCOMultiplier = 4, /* Quadrupler mode for SR2 PLL */
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0, 0}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_MPLL1_OUT_EARLY_DIV3
 *
 *  Set of source frequency configurations.
 *
 *  NOTE: Early output from a PLL results in 2x the main output
 *        frequency which in this case results in:
 *
 *        F_mpll1earlyout = 576 * 2 = 1152 MHz
 *
 *        This source output is div3 of the above frequency.
 */
static ClockSourceFreqConfigType SourceFreqConfig_MPLL1_OUT_EARLY_DIV3[] =
{
  {
    .nFreqHz    = 384000 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_MPLL1,
      .eVCO           = HAL_CLK_PLL_VCO1,
      .nPreDiv        = 3,
      .nPostDiv       = 0,
      .nL             = 0,
      .nM             = 0,
      .nN             = 1,
      .nVCOMultiplier = 0, /* Bypass multiplier for derived source */
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0, 0}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_MPLL1_OUT_EARLY_DIV5
 *
 *  Set of source frequency configurations.
 *
 *  NOTE: Early output from a PLL results in 2x the main output
 *        frequency which in this case results in:
 *
 *        F_mpll1earlyout = 576 * 2 = 1152 MHz
 *
 *        This source output is div5 of the above frequency.
 */
static ClockSourceFreqConfigType SourceFreqConfig_MPLL1_OUT_EARLY_DIV5[] =
{
  {
    .nFreqHz    = 230400 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_MPLL1,
      .eVCO           = HAL_CLK_PLL_VCO1,
      .nPreDiv        = 5,
      .nPostDiv       = 0,
      .nL             = 0,
      .nM             = 0,
      .nN             = 1,
      .nVCOMultiplier = 0, /* Bypass multiplier for derived source */
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0, 0}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_MPLL2
 *
 *  Set of source frequency configurations.
 */
ClockSourceFreqConfigType SourceFreqConfig_MPLL2[] =
{
  {
    .nFreqHz	  = 384000 * 1000,
    .HALConfig  =
	{
	  .eSource		  = HAL_CLK_SOURCE_XO,
	  .eVCO			  = HAL_CLK_PLL_VCO3,
	  .nPreDiv		  = 1,
	  .nPostDiv		  = 1,
	  .nL 			  = 20,
	  .nM 		      = 0,
	  .nN 			  = 0,
	  .nVCOMultiplier = 0,
      .nAlpha 		  = 0,
      .nAlphaU		  = 0,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0x0, 0x0}, {0xFF, 0xFF} },
  },
  {
    .nFreqHz	  = 441600 * 1000,
    .HALConfig  =
	{
	  .eSource		  = HAL_CLK_SOURCE_XO,
	  .eVCO			  = HAL_CLK_PLL_VCO3,
	  .nPreDiv		  = 1,
	  .nPostDiv		  = 1,
	  .nL 			  = 23,
	  .nM 		      = 0,
	  .nN 			  = 0,
	  .nVCOMultiplier = 0,
      .nAlpha 		  = 0,
      .nAlphaU		  = 0,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0x0, 0x0}, {0xFF, 0xFF} },
  },
  {
    .nFreqHz    = 537600 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_XO,
      .eVCO           = HAL_CLK_PLL_VCO3,
      .nPreDiv        = 1,
      .nPostDiv       = 1,
      .nL             = 28,
      .nM             = 0,
      .nN             = 0,
      .nVCOMultiplier = 0,
      .nAlpha         = 0,
      .nAlphaU        = 0,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0x0, 0x0}, {0xFF, 0xFF} },
  },
  {
    .nFreqHz    = 576000 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_XO,
      .eVCO           = HAL_CLK_PLL_VCO3,
      .nPreDiv        = 1,
      .nPostDiv       = 1,
      .nL             = 30,
      .nM             = 0,
      .nN             = 0,
      .nVCOMultiplier = 0,
      .nAlpha         = 0,
      .nAlphaU        = 0,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0x0, 0x0}, {0xFF, 0xFF} },
  },
  {
    .nFreqHz    = 691200 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_XO,
      .eVCO           = HAL_CLK_PLL_VCO3,
      .nPreDiv        = 1,
      .nPostDiv       = 1,
      .nL             = 36,
      .nM             = 0,
      .nN             = 0,
      .nVCOMultiplier = 0,
      .nAlpha         = 0,
      .nAlphaU        = 0,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0x0, 0x0}, {0xFF, 0xFF} },
  },
  {
    .nFreqHz	  = 499200 * 1000,
    .HALConfig  =
	{
	  .eSource		  = HAL_CLK_SOURCE_XO,
	  .eVCO			  = HAL_CLK_PLL_VCO3,
	  .nPreDiv		  = 1,
	  .nPostDiv		  = 1,
	  .nL 			  = 26,
	  .nM 		      = 0,
	  .nN 			  = 0,
	  .nVCOMultiplier = 0,
      .nAlpha 		  = 0,
      .nAlphaU		  = 0,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0x0, 0x0}, {0xFF, 0xFF} },
  },
   {
    .nFreqHz    = 595200 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_XO,
      .eVCO           = HAL_CLK_PLL_VCO3,
      .nPreDiv        = 1,
      .nPostDiv       = 1,
      .nL             = 31,
      .nM             = 0,
      .nN             = 0,
      .nVCOMultiplier = 0,
      .nAlpha         = 0,
      .nAlphaU        = 0,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0x0, 0x0}, {0xFF, 0xFF} },
  },
  {
    .nFreqHz    = 633600 * 1000,
    .HALConfig  =
    {
      .eSource        = HAL_CLK_SOURCE_XO,
      .eVCO           = HAL_CLK_PLL_VCO3,
      .nPreDiv        = 1,
      .nPostDiv       = 1,
      .nL             = 33,
      .nM             = 0,
      .nN             = 0,
      .nVCOMultiplier = 0,
      .nAlpha         = 0,
      .nAlphaU        = 0,
    },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0x0, 0x0}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};

/*
 * SourceConfig
 *
 * Clock source configuration data.
 */
const ClockSourceConfigType SourceConfig[] =
{

  {
    SOURCE_NAME(XO),

    .nConfigMask       = 0,
    .pSourceFreqConfig = SourceFreqConfig_XO,
  },
  {
    SOURCE_NAME(GPLL0),

    .nConfigMask       = CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    .pSourceFreqConfig = SourceFreqConfig_GPLL0,
  },
  {
    SOURCE_NAME(GPLL1),

    .nConfigMask       = CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    .pSourceFreqConfig = SourceFreqConfig_GPLL1,
  },
  {
    SOURCE_NAME(MPLL1),

    .nConfigMask       = CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE,
    .pSourceFreqConfig = SourceFreqConfig_MPLL1,
  },
  {
    SOURCE_NAME(MPLL1_OUT_EARLY_DIV3),

    .nConfigMask       = 0,
    .pSourceFreqConfig = SourceFreqConfig_MPLL1_OUT_EARLY_DIV3,
  },
  {
    SOURCE_NAME(MPLL1_OUT_EARLY_DIV5),

    .nConfigMask       = 0,
    .pSourceFreqConfig = SourceFreqConfig_MPLL1_OUT_EARLY_DIV5,
  },
  {
    SOURCE_NAME(MPLL2),
     /* Q6 Turbo uses MPLL2 EARLY OUTPUT */
    .nConfigMask       = CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE
                       | CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE,
    .pSourceFreqConfig = SourceFreqConfig_MPLL2,
    .pCalibrationFreqConfig = &SourceFreqConfig_MPLL2[2], // 537.6Mhz

  },

  /* last entry */
  { SOURCE_NAME(NULL) }
};


/* ===================================================================================================================
**    nFreqHz,       { eSource, nDiv2x, nM, nN, n2D },      eVRegLevel,         { HW_MIN(maj, min), HW_MAX(maj, min) }
** ===================================================================================================================*/


/*
 * QUP I2C clock configuration.
 */
const ClockMuxConfigType  QUPI2CClockConfig[] =
{
  {  19200000,     { HAL_CLK_SOURCE_NULL,     2,    0,      0,     0 }, VCS_CORNER_LOW },
  {  50000000,     { HAL_CLK_SOURCE_GPLL0,   32,    0,      0,     0 }, VCS_CORNER_LOW },
  { 0 }
};

/*
 * SEC CTRL clock configurations
 */
const ClockMuxConfigType  SECCTRLClockConfig[] =
{
  {   2400000,     { HAL_CLK_SOURCE_XO,    16, 0, 0, 0 }, VCS_CORNER_LOW},
  {  19200000,     { HAL_CLK_SOURCE_XO,     2, 0, 0, 0 }, VCS_CORNER_LOW},
  { 0 }
};


/*
 * UART clock configurations.
 */
const ClockMuxConfigType  UARTClockConfig[] =
{
  {  3686400, { HAL_CLK_SOURCE_GPLL0,   2,    72,  15625,  15625 }, VCS_CORNER_LOW},
  {  7372800, { HAL_CLK_SOURCE_GPLL0,   2,   144,  15625,  15625 }, VCS_CORNER_LOW},
  { 14745600, { HAL_CLK_SOURCE_GPLL0,   2,   288,  15625,  15625 }, VCS_CORNER_LOW},
  { 16000000, { HAL_CLK_SOURCE_GPLL0,  20,     1,      5,      5 }, VCS_CORNER_LOW},
  { 19200000, { HAL_CLK_SOURCE_XO,      2,     0,      0,      0 }, VCS_CORNER_LOW},
  { 24000000, { HAL_CLK_SOURCE_GPLL0,   2,     3,    100,    100 }, VCS_CORNER_LOW},
  { 25000000, { HAL_CLK_SOURCE_GPLL0,  32,     1,      2,      2 }, VCS_CORNER_LOW},
  { 32000000, { HAL_CLK_SOURCE_GPLL0,   2,     1,     25,     25 }, VCS_CORNER_LOW},
  { 40000000, { HAL_CLK_SOURCE_GPLL0,   2,     1,     20,     20 }, VCS_CORNER_NOMINAL},
  { 46400000, { HAL_CLK_SOURCE_GPLL0,   2,    29,    500,    500 }, VCS_CORNER_NOMINAL},
  { 48000000, { HAL_CLK_SOURCE_GPLL0,   2,     3,     50,     50 }, VCS_CORNER_NOMINAL},
  { 51200000, { HAL_CLK_SOURCE_GPLL0,   2,     8,    125,    125 }, VCS_CORNER_NOMINAL},
  { 56000000, { HAL_CLK_SOURCE_GPLL0,   2,     7,    100,    100 }, VCS_CORNER_NOMINAL},
  { 58982400, { HAL_CLK_SOURCE_GPLL0,   2,  1152,  15625,  15625 }, VCS_CORNER_NOMINAL},
  { 60000000, { HAL_CLK_SOURCE_GPLL0,   2,     3,     40,     40 }, VCS_CORNER_NOMINAL},
  { 0 }
};


/*
 * QUP SPI clock configurations.
 */
const ClockMuxConfigType  QUPSPIClockConfig[] =
{
  {    960000,     { HAL_CLK_SOURCE_XO,    20,  1,   2,   2 }, VCS_CORNER_LOW},
  {   4800000,     { HAL_CLK_SOURCE_XO,     8,  0,   0,   0 }, VCS_CORNER_LOW},
  {   9600000,     { HAL_CLK_SOURCE_XO,     4,  0,   0,   0 }, VCS_CORNER_LOW},
  {  16000000,     { HAL_CLK_SOURCE_GPLL0, 20,  1,   5,   5 }, VCS_CORNER_LOW},
  {  19200000,     { HAL_CLK_SOURCE_XO,     2,  0,   0,   0 }, VCS_CORNER_LOW},
  {  25000000,     { HAL_CLK_SOURCE_GPLL0, 32,  1,   2,   2 }, VCS_CORNER_LOW},
  {  50000000,     { HAL_CLK_SOURCE_GPLL0, 32,  0,   0,   0 }, VCS_CORNER_NOMINAL},
  { 0 }
};


/*
 * GSBI SIM clock configurations.
 */
const ClockMuxConfigType  UARTSIMClockConfig[] =
{
  {   3840000,     { HAL_CLK_SOURCE_XO, 10,  0,  0,  0 }, VCS_CORNER_LOW},
  { 0 }
};


/*
 * UIM clock configurations.
 */
const ClockMuxConfigType  UIMSIMClockConfig[] =
{
  { 3840000, { HAL_CLK_SOURCE_XO,  10,  0,  0,  0 }, VCS_CORNER_LOW },
  { 4800000, { HAL_CLK_SOURCE_XO,   8,  0,  0,  0 }, VCS_CORNER_LOW },
  { 0 }
};

/*
 * UIM UART bit clock configurations.
 */
const ClockMuxConfigType  UIMUARTBitClockConfig[] =
{
  {       4807680, { HAL_CLK_SOURCE_XO,     2,    313,   1250,   1250 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       4953600, { HAL_CLK_SOURCE_XO,     2,    129,    500,    500 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       4961280, { HAL_CLK_SOURCE_XO,     2,    323,   1250,   1250 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       5120000, { HAL_CLK_SOURCE_XO,     2,      4,     15,     15 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       5283840, { HAL_CLK_SOURCE_XO,     2,    172,    625,    625 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       5285120, { HAL_CLK_SOURCE_XO,     2,   4129,  15000,  15000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {   5760 * 1000, { HAL_CLK_SOURCE_XO,     2,      3,     10,     10 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       5775360, { HAL_CLK_SOURCE_XO,     2,    188,    625,    625 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       6328320, { HAL_CLK_SOURCE_XO,     2,    206,    625,    625 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       6343680, { HAL_CLK_SOURCE_XO,     2,    413,   1250,   1250 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {   6400 * 1000, { HAL_CLK_SOURCE_XO,     2,      1,      3,      3 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       6604800, { HAL_CLK_SOURCE_XO,     2,     43,    125,    125 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       6606080, { HAL_CLK_SOURCE_XO,     2,   5161,  15000,  15000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       6606400, { HAL_CLK_SOURCE_XO,     2,   4129,  12000,  12000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       6606720, { HAL_CLK_SOURCE_XO,     2,   3441,  10000,  10000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       6607360, { HAL_CLK_SOURCE_XO,     2,   2581,   7500,   7500 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7045120, { HAL_CLK_SOURCE_XO,     2,    688,   1875,   1875 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7046400, { HAL_CLK_SOURCE_XO,     2,    367,   1000,   1000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7046880, { HAL_CLK_SOURCE_XO,     2,  14681,  40000,  40000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7047040, { HAL_CLK_SOURCE_XO,     2,  11011,  30000,  30000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7047680, { HAL_CLK_SOURCE_XO,     2,   2753,   7500,   7500 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7188480, { HAL_CLK_SOURCE_XO,     2,    234,    625,    625 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {   7200 * 1000, { HAL_CLK_SOURCE_XO,     2,      3,      8,      8 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7201280, { HAL_CLK_SOURCE_XO,     2,   2813,   7500,   7500 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7203840, { HAL_CLK_SOURCE_XO,     2,    469,   1250,   1250 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7432320, { HAL_CLK_SOURCE_XO,     2,   3871,  10000,  10000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {   7680 * 1000, { HAL_CLK_SOURCE_XO,     2,      2,      5,      5 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7925760, { HAL_CLK_SOURCE_XO,     2,    258,    625,    625 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7927680, { HAL_CLK_SOURCE_XO,     2,   4129,  10000,  10000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7928320, { HAL_CLK_SOURCE_XO,     2,   3097,   7500,   7500 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       7929600, { HAL_CLK_SOURCE_XO,     2,    413,   1000,   1000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {   8000 * 1000, { HAL_CLK_SOURCE_XO,     2,      5,     12,     12 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8257920, { HAL_CLK_SOURCE_XO,     2,   4301,  10000,  10000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8258560, { HAL_CLK_SOURCE_XO,     2,   1613,   3750,   3750 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8455680, { HAL_CLK_SOURCE_XO,     2,   1101,   2500,   2500 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8456320, { HAL_CLK_SOURCE_XO,     2,  13213,  30000,  30000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8458240, { HAL_CLK_SOURCE_XO,     2,    826,   1875,   1875 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {   8640 * 1000, { HAL_CLK_SOURCE_XO,     2,      9,     20,     20 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8806400, { HAL_CLK_SOURCE_XO,     2,    172,    375,    375 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8808320, { HAL_CLK_SOURCE_XO,     2,  13763,  30000,  30000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8808640, { HAL_CLK_SOURCE_XO,     2,  27527,  60000,  60000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       8808960, { HAL_CLK_SOURCE_XO,     2,   1147,   2500,   2500 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {   9000 * 1000, { HAL_CLK_SOURCE_XO,     2,     15,     32,     32 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       9000960, { HAL_CLK_SOURCE_XO,     2,    293,    625,    625 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {       9000960, { HAL_CLK_SOURCE_XO,     2,   2477,   5000,   5000 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  {   9600 * 1000, { HAL_CLK_SOURCE_XO,     2,      1,      2,      2 }, VCS_CORNER_LOW, { {0, 0}, {0xFF, 0xFF} } },
  { 0 }                                                                                      
};        

/*
 * LPASS AHB clock configurations.
 */
const ClockMuxConfigType  LPASSAHBClockConfig[] =
{
  {   3200 * 1000, { HAL_CLK_SOURCE_XO,      12, 0, 0, 0 }, VCS_CORNER_LOW },
  {   6400 * 1000, { HAL_CLK_SOURCE_XO,       6, 0, 0, 0 }, VCS_CORNER_LOW },
  {   9600 * 1000, { HAL_CLK_SOURCE_XO,       4, 0, 0, 0 }, VCS_CORNER_LOW },
  {  19200 * 1000, { HAL_CLK_SOURCE_XO,       2, 0, 0, 0 }, VCS_CORNER_LOW },
  {  40000 * 1000, { HAL_CLK_SOURCE_GPLL0,   20, 1, 2, 2 }, VCS_CORNER_LOW },
  {  66670 * 1000, { HAL_CLK_SOURCE_GPLL0,   24, 0, 0, 0 }, VCS_CORNER_NOMINAL },
  {  80000 * 1000, { HAL_CLK_SOURCE_GPLL0,   20, 0, 0, 0 }, VCS_CORNER_NOMINAL },
  { 100000 * 1000, { HAL_CLK_SOURCE_GPLL0,   16, 0, 0, 0 }, VCS_CORNER_NOMINAL },
  { 0 }
};

/*
 * LPASS CXO clock configurations.
 */
const ClockMuxConfigType AudioCXOClockConfig[] =
{
  { 19200 * 1000, { HAL_CLK_SOURCE_XO, 2, 0, 0, 0 }, VCS_CORNER_LOW},
  { 0 }
};

/*
 * LPASS Digicodec clock configurations.
 */
const ClockMuxConfigType AudioDigicodecClockConfig[] =
{
  {    256 * 1000, { HAL_CLK_SOURCE_GPLL1,  32, 1, 150, 150 }, VCS_CORNER_LOW},
  {   9600 * 1000, { HAL_CLK_SOURCE_XO,      4, 0,   0,   0 }, VCS_CORNER_LOW},
  {  12288 * 1000, { HAL_CLK_SOURCE_GPLL1,   2, 1,  50,  50 }, VCS_CORNER_LOW},
  { 0 }
};

/*
 * LPAIF clocks.
 */
const ClockMuxConfigType LPAIFClockConfig[] =
{
  {    256 * 1000, { HAL_CLK_SOURCE_GPLL1,  32, 1, 150, 150 }, VCS_CORNER_LOW},
  {    512 * 1000, { HAL_CLK_SOURCE_GPLL1,  32, 1,  75,  75 }, VCS_CORNER_LOW},
  {    768 * 1000, { HAL_CLK_SOURCE_GPLL1,  32, 1,  50,  50 }, VCS_CORNER_LOW},
  {   1024 * 1000, { HAL_CLK_SOURCE_GPLL1,  32, 2,  75,  75 }, VCS_CORNER_LOW},
  {   1536 * 1000, { HAL_CLK_SOURCE_GPLL1,  32, 1,  25,  25 }, VCS_CORNER_LOW},
  {   2048 * 1000, { HAL_CLK_SOURCE_GPLL1,  16, 2,  75,  75 }, VCS_CORNER_LOW},
  {   2304 * 1000, { HAL_CLK_SOURCE_GPLL1,  16, 3, 100, 100 }, VCS_CORNER_LOW},
  {   2400 * 1000, { HAL_CLK_SOURCE_XO,     16, 0,   0,   0 }, VCS_CORNER_LOW},
  {   3072 * 1000, { HAL_CLK_SOURCE_GPLL1,  16, 1,  25,  25 }, VCS_CORNER_LOW},
  {   4096 * 1000, { HAL_CLK_SOURCE_GPLL1,   8, 2,  75,  75 }, VCS_CORNER_LOW},
  {   6144 * 1000, { HAL_CLK_SOURCE_GPLL1,   8, 1,  25,  25 }, VCS_CORNER_LOW},
  {   8192 * 1000, { HAL_CLK_SOURCE_GPLL1,   4, 2,  75,  75 }, VCS_CORNER_LOW},
  {  12288 * 1000, { HAL_CLK_SOURCE_GPLL1,   2, 1,  50,  50 }, VCS_CORNER_LOW},
  { 0 }
};

/*
 * LPASS Audio EXT MClk2 clock configurations.
 */
const ClockMuxConfigType AudioExtMclk2ClockConfig[] =
{
  {    12288 * 1000, { HAL_CLK_SOURCE_GPLL1,  2, 1, 50, 50 }, VCS_CORNER_LOW},
  {     9600 * 1000, { HAL_CLK_SOURCE_XO,      4, 0, 0, 0 }, VCS_CORNER_LOW},
  { 0 }
};

/*
 * LPASS Audio PCM Data OE clock configurations.
 */
const ClockMuxConfigType AudioPCMDataOEClockConfig[] =
{
  {    72282 * 1000, { HAL_CLK_SOURCE_GPLL1,  17, 0, 0, 0 }, VCS_CORNER_LOW},
  {    24576 * 1000, { HAL_CLK_SOURCE_GPLL1,   2, 1, 25, 25 }, VCS_CORNER_LOW},
  {    12288 * 1000, { HAL_CLK_SOURCE_GPLL1,   2, 1, 50, 50 }, VCS_CORNER_LOW},
  {    19200 * 1000, { HAL_CLK_SOURCE_XO,      2, 0, 0, 0 }, VCS_CORNER_LOW},
  {     9600 * 1000, { HAL_CLK_SOURCE_XO,      4, 0, 0, 0 }, VCS_CORNER_LOW},
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
 * Clock Flag Init Config.
 */
const ClockFlagInitType ClockFlagInitConfig[] =
{
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    (void *)"clk_q6",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"clk_bus_slave_timeout",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"clk_mss_config_bus",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"clk_bus_uim0",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"clk_bus_uim1",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"clk_bus_uim2",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"mpll1_early_div3_cx_to_ls",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"mpll1_early_div5",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"gcc_spmi_ahb_clk",
    CLOCK_FLAG_INTERNAL_CONTROL
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"gcc_spmi_ser_clk",
    CLOCK_FLAG_INTERNAL_CONTROL
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"gcc_ultaudio_ahbfabric_efabric_spdm_clk",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"gcc_ultaudio_ahbfabric_ixfabric_clk",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"gcc_ultaudio_ahbfabric_ixfabric_lpm_clk",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"gcc_ultaudio_pcnoc_mport_clk",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK,
    (void *)"gcc_ultaudio_pcnoc_sway_clk",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"SLEEPCLK",
    CLOCK_FLAG_ALWAYS_ON
  },
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"MPLL1",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"MPLL1_OUT_EARLY_DIV3",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"MPLL1_OUT_EARLY_DIV5",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    (void *)"gcc_ultaudio_lpaif_pri_i2s_clk",
    CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    (void *)"gcc_ultaudio_lpaif_sec_i2s_clk",
    CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    (void *)"gcc_ultaudio_lpaif_aux_i2s_clk",
    CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF
  },
  {
    CLOCK_FLAG_NODE_TYPE_NONE,
    (void *)0,
    0
  }
};


/*
 * Stub flags.
 */
const ClockStubType ClockStubConfig =
{
  .bRUMI   = FALSE,
  .bVirtio = FALSE,
};

