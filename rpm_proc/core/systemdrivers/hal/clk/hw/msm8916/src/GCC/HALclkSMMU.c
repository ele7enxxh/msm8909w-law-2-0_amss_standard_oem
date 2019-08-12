/*
==============================================================================

FILE:         HALclkSMMU.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   SMMU clocks.

   List of clock domains:
   -HAL_clk_mGCCAPSSTCUASYNCClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8916/src/GCC/HALclkSMMU.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
12/03/2013            Auto-generated.

==============================================================================
            Copyright (c) 2013 QUALCOMM Technologies Incorporated.
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
extern HAL_clk_ClockDomainControlType  HAL_clk_mAPSSTCUASYNCClockDomainControl;

/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mAPSSTCUASYNCClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mAPSSTCUASYNCClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_apss_tcu_async_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_APSS_TCU_ASYNC_CLK
  },
};


/*
 * HAL_clk_mGCCAPSSTCUASYNCClkDomain
 *
 * APSS TCU ASYNC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSTCUASYNCClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_APSS_TCU_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mAPSSTCUASYNCClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mAPSSTCUASYNCClkDomainClks)/sizeof(HAL_clk_mAPSSTCUASYNCClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mAPSSTCUASYNCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


