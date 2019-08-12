/*
==============================================================================

FILE:         HALclkRBCPR.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   RBCPR clocks.

   List of clock domains:
   -HAL_clk_mGCCRBCPRClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8916/src/GCC/HALclkRBCPR.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
09/23/2013            Auto-generated.

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


/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mRBCPRClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mRBCPRClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_rbcpr_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RBCPR_CBCR), HWIO_OFFS(GCC_RBCPR_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RBCPR_CLK
  },
};


/*
 * HAL_clk_mGCCRBCPRClkDomain
 *
 * RBCPR clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCRBCPRClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_RBCPR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mRBCPRClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mRBCPRClkDomainClks)/sizeof(HAL_clk_mRBCPRClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


