/*
==============================================================================

FILE:         HALclkSMMU.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   SMMU clocks.

   List of clock domains:
     - HAL_clk_mGCCAPSSTCUClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x07/src/GCC/HALclkSMMU.c#2 $

==============================================================================
            Copyright (c) 2015 QUALCOMM Technologies Incorporated.
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
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mAPSSTCUClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mAPSSTCUClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_apss_tcu_async_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_APSS_TCU_ASYNC_CLK
  },
  {
    /* .szClockName      = */ "gcc_bimc_tcu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_TCU_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_TCU_CLK
  },
};


/*
 * HAL_clk_mGCCAPSSTCUClkDomain
 *
 * APSSTCU clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSTCUClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_APSS_TCU_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mAPSSTCUClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mAPSSTCUClkDomainClks)/sizeof(HAL_clk_mAPSSTCUClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


