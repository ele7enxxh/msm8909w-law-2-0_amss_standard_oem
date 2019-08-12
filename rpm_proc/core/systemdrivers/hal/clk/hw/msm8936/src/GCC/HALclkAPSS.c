/*
==============================================================================

FILE:         HALclkAPSS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   APSS clocks.

   List of clock domains:
   -HAL_clk_mGCCAPSSAHBClkDomain
   -HAL_clk_mGCCAPSSAXIClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8936/src/GCC/HALclkAPSS.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
05/30/2014            Auto-generated.

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
 *  HAL_clk_mAPSSAXIClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mAPSSAXIClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_apss_axi_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_APSS_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_apss_axi_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_APSS_AXI_CLK
  },
};


/*
 * HAL_clk_mGCCAPSSAXIClkDomain
 *
 * APSS AXI clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSAXIClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_APSS_AXI_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mAPSSAXIClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mAPSSAXIClkDomainClks)/sizeof(HAL_clk_mAPSSAXIClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


