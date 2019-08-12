/*
==============================================================================

FILE:         HALclkDDRDIMWrapper.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   DDR DIM Wrapper clocks.

   List of clock domains:
   -HAL_clk_mGCCBIMCDDRCPLL0ClkDomain
   -HAL_clk_mGCCBIMCDDRCPLL1ClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8936/src/GCC/HALclkDDRDIMWrapper.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
03/28/2014            Auto-generated.

==============================================================================
            Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkTest.h"
#include "HALclkGeneric.h"
#include "HALclkHWIO.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;


/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mBIMCDDRCPLL0ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCDDRCPLL0ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_ddr_cpll0_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_DDR_CPLL0_CLK
  },
  {
    /* .szClockName      = */ "gcc_bimc_ddr_ch0_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_DDR_CH0_CLK
  },
};


/*
 * HAL_clk_mGCCBIMCDDRCPLL0ClkDomain
 *
 * BIMC DDR CPLL0 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRCPLL0ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_DDR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCDDRCPLL0ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCDDRCPLL0ClkDomainClks)/sizeof(HAL_clk_mBIMCDDRCPLL0ClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mBIMCDDRCPLL1ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCDDRCPLL1ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_ddr_cpll1_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_DDR_CPLL1_CLK
  },
};


/*
 * HAL_clk_mGCCBIMCDDRCPLL1ClkDomain
 *
 * BIMC DDR CPLL1 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRCPLL1ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_DDR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCDDRCPLL1ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCDDRCPLL1ClkDomainClks)/sizeof(HAL_clk_mBIMCDDRCPLL1ClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};


