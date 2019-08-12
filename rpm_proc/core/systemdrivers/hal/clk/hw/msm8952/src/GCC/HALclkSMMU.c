/*
==============================================================================

FILE:         HALclkSMMU.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   SMMU clocks.

   List of clock domains:
   -HAL_clk_mGCCAPSSTCUClkDomain
   -HAL_clk_mGCCQ6TBUClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8952/src/GCC/HALclkSMMU.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
03/21/2014            Auto-generated.

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
    /* .szClockName      = */ "gcc_bimc_apss_tcu_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_APSS_TCU_CLK
  },
};


/*
 * HAL_clk_mGCCAPSSTCUClkDomain
 *
 * APSS TCU clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSTCUClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_APSS_TCU_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mAPSSTCUClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mAPSSTCUClkDomainClks)/sizeof(HAL_clk_mAPSSTCUClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mQ6TBUClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQ6TBUClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_mss_tbu_Q6_axi_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_TBU_Q6_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_mss_q6_bimc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_Q6_BIMC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_Q6_BIMC_AXI_CLK
  },
};


/*
 * HAL_clk_mGCCQ6TBUClkDomain
 *
 * Q6 TBU clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQ6TBUClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_Q6_TBU_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQ6TBUClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQ6TBUClkDomainClks)/sizeof(HAL_clk_mQ6TBUClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


