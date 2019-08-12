/*
==============================================================================

FILE:         HALclkRBCPRMX.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   RBCPRMX clocks.

   List of clock domains:
     - HAL_clk_mGCCRBCPRMXClkDomain


   List of power domains:



==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/GCC/HALclkRBCPRMX.c#1 $

when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
06/05/2014           Auto-generated.


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
 *  HAL_clk_mRBCPRMXClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mRBCPRMXClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_rbcpr_mx_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RBCPR_MX_CBCR), HWIO_OFFS(GCC_RBCPR_MX_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RBCPR_MX_CLK
  },
};


/*
 * HAL_clk_mGCCRBCPRMXClkDomain
 *
 * RBCPRMX clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCRBCPRMXClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_RBCPR_MX_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mRBCPRMXClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mRBCPRMXClkDomainClks)/sizeof(HAL_clk_mRBCPRMXClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

