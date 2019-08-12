/*
===========================================================================
*/
/**
  @file ClockBSP.c

  Clock BSP data.
*/
/*
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/clock/config/mdm9x07/ClockBSP.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockTZ.h"
#include "ClockBSP.h"
#include "HALclkHWIO.h"
#include "HALhwio.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

#define BLSP_SPI_DOMAIN(a, b) \
  static ClockDomain BLSP##a##QUP##b##SPIAppsDomain = \
  { \
    .HALClockDomain = { HWIO_GCC_BLSP##a##_QUP##b##_SPI_APPS_CMD_RCGR_ADDR, aGCCSourceMap }, \
    .aBSPConfig     = aSPIConfig, \
    .pSource        = &aSources[HAL_CLK_SOURCE_XO] \
  }

#define BLSP_SPI_CLOCK(a, b) \
  { \
    .szName   = "gcc_blsp"#a"_qup"#b"_spi_apps_clk", \
    .HALClock = { HWIO_GCC_BLSP##a##_QUP##b##_SPI_APPS_CBCR_ADDR }, \
    .pDomain  = &BLSP##a##QUP##b##SPIAppsDomain \
  }

/*=========================================================================
      Data Declarations
==========================================================================*/

static ClockGroup aClockGroups[CLOCK_GROUP_TOTAL] =
{
  [CLOCK_GROUP_ABT] = {
    (HAL_clk_ClockDescType[]) {
      { HWIO_GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR_ADDR },
      { 0 }
    }
  },
  
  [CLOCK_GROUP_BUS] = {
    (HAL_clk_ClockDescType[]) {
      { HWIO_GCC_PCNOC_AHB_CBCR_ADDR },
      { HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR },
      { HWIO_GCC_PCNOC_AT_CBCR_ADDR },
      { 0 }
    }
  },

  [CLOCK_GROUP_INIT] = {
    (HAL_clk_ClockDescType[]) {
      { HWIO_GCC_SPDM_MSTR_AHB_CBCR_ADDR },
      { HWIO_GCC_SPDM_CFG_AHB_CBCR_ADDR },
      { 0 }
    }
  },
};

/*----------------------------------------------------------------------*/
/* RPM Configuration                                                    */
/*----------------------------------------------------------------------*/

/*
 * The /xo/cxo RPM resource.
 */
static ClockRPMResource RPMXOResource =
  { NULL, CLOCK_RPM_UNIT_ENABLE, CLOCK_RPM_TYPE_CLK0, 0 };

static ClockRPMResource aRPMResources[] =
{
  { "/clk/qdss",      CLOCK_RPM_UNIT_STATE,  CLOCK_RPM_TYPE_CLK0,  1 },
  { "/clk/pcnoc",     CLOCK_RPM_UNIT_KHZ,    CLOCK_RPM_TYPE_CLK1,  0 },
  { "/clk/snoc",      CLOCK_RPM_UNIT_KHZ,    CLOCK_RPM_TYPE_CLK1,  1 },
  { "/clk/bimc",      CLOCK_RPM_UNIT_KHZ,    CLOCK_RPM_TYPE_CLK2,  0 },
  { 0 }
};

/*----------------------------------------------------------------------*/
/* Source Configuration                                                 */
/*----------------------------------------------------------------------*/

static ClockSource aSources[HAL_CLK_SOURCE_TOTAL] =
{
  [HAL_CLK_SOURCE_XO]     = { .pRPM = &RPMXOResource },
  [HAL_CLK_SOURCE_GPLL0]  = {
    .HALSource  = { HWIO_GCC_GPLL0_MODE_ADDR, PLL_VOTE_MASK(GPLL0) },
    .pSource    = &aSources[HAL_CLK_SOURCE_XO]
  }
};

static uint32 aGCCSourceMap[HAL_CLK_SOURCE_TOTAL] =
{
  [HAL_CLK_SOURCE_XO]     = 0,
  [HAL_CLK_SOURCE_GPLL0]  = 1
};

/*----------------------------------------------------------------------*/
/* Frequency Configuration                                              */
/*----------------------------------------------------------------------*/

static ClockMuxConfigType aSPIConfig[] =
{
  {    960000,     { HAL_CLK_SOURCE_XO,    20,  1,   2,   2 }, CLOCK_VREG_LEVEL_LOW},
  {   4800000,     { HAL_CLK_SOURCE_XO,     8,  0,   0,   0 }, CLOCK_VREG_LEVEL_LOW},
  {   9600000,     { HAL_CLK_SOURCE_XO,     4,  0,   0,   0 }, CLOCK_VREG_LEVEL_LOW},
  {  16000000,     { HAL_CLK_SOURCE_GPLL0, 20,  1,   5,   5 }, CLOCK_VREG_LEVEL_LOW},
  {  19200000,     { HAL_CLK_SOURCE_XO,     2,  0,   0,   0 }, CLOCK_VREG_LEVEL_LOW},
  {  25000000,     { HAL_CLK_SOURCE_GPLL0, 32,  1,   2,   2 }, CLOCK_VREG_LEVEL_LOW},
  { 0 }
};

/*----------------------------------------------------------------------*/
/* Domain Configuration                                                 */
/*----------------------------------------------------------------------*/

/*
 * Catch-all for clocks without controllable domains.
*/
static ClockDomain XOVoteDomain =
{
  .pSource = &aSources[HAL_CLK_SOURCE_XO]
};

BLSP_SPI_DOMAIN(1,1);
BLSP_SPI_DOMAIN(1,2);
BLSP_SPI_DOMAIN(1,3);
BLSP_SPI_DOMAIN(1,4);
BLSP_SPI_DOMAIN(1,5);
BLSP_SPI_DOMAIN(1,6);

/*----------------------------------------------------------------------*/
/* Clock Configuration                                                  */
/*----------------------------------------------------------------------*/

static Clock aClocks[] =
{
  {
    .szName   = "gcc_blsp1_ahb_clk",
    .HALClock = { HWIO_GCC_BLSP1_AHB_CBCR_ADDR, VOTE_MASK(BLSP1_AHB_CLK_ENA) },
    .pDomain  = &XOVoteDomain
  },
  {
    .szName   = "gcc_prng_ahb_clk",
    .HALClock = { HWIO_GCC_PRNG_AHB_CBCR_ADDR, VOTE_MASK(PRNG_AHB_CLK_ENA) },
    .pDomain  = &XOVoteDomain
  },
  {
    .szName   = "gcc_msg_ram_ahb_clk",
    .HALClock = { HWIO_GCC_MSG_RAM_AHB_CBCR_ADDR, VOTE_MASK(MSG_RAM_AHB_CLK_ENA) },
    .pDomain  = &XOVoteDomain
  },
  {
    .szName   = "gcc_apss_ahb_clk",
    .HALClock = { HWIO_GCC_APSS_AHB_CBCR_ADDR, VOTE_MASK(APSS_AHB_CLK_ENA) },
    .pDomain  = &XOVoteDomain
  },
  {
    .szName   = "gcc_mpm_ahb_clk",
    .HALClock = { HWIO_GCC_MPM_AHB_CBCR_ADDR, VOTE_MASK(MPM_AHB_CLK_ENA) },
    .pDomain  = &XOVoteDomain
  },
  {
    .szName   = "gcc_crypto_clk",
    .HALClock = { HWIO_GCC_CRYPTO_CBCR_ADDR, VOTE_MASK(CRYPTO_CLK_ENA) },
    .pDomain  = &XOVoteDomain
  },
   {
    .szName   = "gcc_crypto_ahb_clk",
    .HALClock = { HWIO_GCC_CRYPTO_AHB_CBCR_ADDR, VOTE_MASK(CRYPTO_AHB_CLK_ENA) },
    .pDomain  = &XOVoteDomain
  },
  {
    .szName   = "gcc_crypto_axi_clk",
    .HALClock = { HWIO_GCC_CRYPTO_AXI_CBCR_ADDR, VOTE_MASK(CRYPTO_AXI_CLK_ENA) },
    .pDomain  = &XOVoteDomain
  },
  BLSP_SPI_CLOCK(1,1),
  BLSP_SPI_CLOCK(1,2),
  BLSP_SPI_CLOCK(1,3),
  BLSP_SPI_CLOCK(1,4),
  BLSP_SPI_CLOCK(1,5),
  BLSP_SPI_CLOCK(1,6),
  { 0 }
};

/*
 * Main BSP data.
 */
ClockTZBSP ClockTZBSPConfig =
{
  .aRPMResources  = aRPMResources,
  .aClockGroups   = aClockGroups,
  .aClocks        = aClocks,
  .aSources       = aSources
};

