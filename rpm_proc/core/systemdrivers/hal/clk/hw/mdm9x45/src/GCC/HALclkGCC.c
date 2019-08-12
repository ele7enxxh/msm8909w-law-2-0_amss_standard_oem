/*
==============================================================================

FILE:         HALclkGCC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   GCC clocks.

   List of clock domains:
     - HAL_clk_mGCCGCCSLEEPClkDomain
     - HAL_clk_mGCCGCCXOClkDomain


   List of power domains:



==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/GCC/HALclkGCC.c#1 $

when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
07/15/2014           Auto-generated.


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
 *  HAL_clk_mGCCSLEEPClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCSLEEPClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ipa_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_IPA_SLEEP_CBCR), HWIO_OFFS(GCC_IPA_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_IPA_SLEEP_CLK
  },
  {
    /* .szClockName      = */ "gcc_qpic_system_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QPIC_SYSTEM_CBCR), HWIO_OFFS(GCC_QPIC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QPIC_SYSTEM_CLK
  },
};


/*
 * HAL_clk_mGCCGCCSLEEPClkDomain
 *
 * GCCSLEEP clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCSLEEPClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_GCC_SLEEP_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCSLEEPClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCSLEEPClkDomainClks)/sizeof(HAL_clk_mGCCSLEEPClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

/*                           
 *  HAL_clk_mGCCXOClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCXOClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_XO_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_XO_CLK
  },
  {
    /* .szClockName      = */ "gcc_xo_div4_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_XO_DIV4_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_XO_DIV4_CLK
  },
};


/*
 * HAL_clk_mGCCGCCXOClkDomain
 *
 * GCCXO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCXOClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_GCC_XO_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCXOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCXOClkDomainClks)/sizeof(HAL_clk_mGCCXOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


