/*
==============================================================================

FILE:         HALclkIPA.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   IPA clocks.

   List of clock domains:
     - HAL_clk_mGCCIPAClkDomain


   List of power domains:



==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/GCC/HALclkIPA.c#1 $

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
 *  HAL_clk_mIPAClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mIPAClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ipa_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_IPA_CBCR), HWIO_OFFS(GCC_IPA_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_IPA_CLK
  },
  {
    /* .szClockName      = */ "gcc_sys_noc_ipa_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_IPA_AXI_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_IPA_AXI_CLK
  },
};


/*
 * HAL_clk_mGCCIPAClkDomain
 *
 * IPA clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCIPAClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_IPA_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mIPAClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mIPAClkDomainClks)/sizeof(HAL_clk_mIPAClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

