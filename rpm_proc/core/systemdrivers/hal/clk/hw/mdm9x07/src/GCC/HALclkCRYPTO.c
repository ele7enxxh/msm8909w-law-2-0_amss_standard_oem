/*
==============================================================================

FILE:         HALclkCRYPTO.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   CRYPTO clocks.

   List of clock domains:
   -HAL_clk_mGCCCRYPTOClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x07/src/GCC/HALclkCRYPTO.c#2 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
08/21/2014            Auto-generated.

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
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mCRYPTOClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCRYPTOClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_crypto_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_CRYPTO_CBCR), HWIO_OFFS(GCC_CRYPTO_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, CRYPTO_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_CRYPTO_CLK
  },
};


/*
 * HAL_clk_mGCCCRYPTOClkDomain
 *
 * CRYPTO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCCRYPTOClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_CRYPTO_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mCRYPTOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCRYPTOClkDomainClks)/sizeof(HAL_clk_mCRYPTOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


