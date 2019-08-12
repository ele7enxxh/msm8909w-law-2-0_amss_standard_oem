/*
==============================================================================

FILE:         HALclkBLSP1.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BLSP1 clocks.

   List of clock domains:
   -HAL_clk_mGCCBLSPUARTSIMClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/gcc/HALclkBLSP1.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
09/26/2013            Auto-generated.

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
 *  HAL_clk_mBLSPUARTSIMClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBLSPUARTSIMClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_blsp1_uart1_sim_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_UART1_SIM_CBCR), HWIO_OFFS(GCC_BLSP1_UART1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_UART1_SIM_CLK
  },
  {
    /* .szClockName      = */ "gcc_blsp1_uart2_sim_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_UART2_SIM_CBCR), HWIO_OFFS(GCC_BLSP1_UART2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_UART2_SIM_CLK
  },
};


/*
 * HAL_clk_mGCCBLSPUARTSIMClkDomain
 *
 * BLSP UART SIM clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSPUARTSIMClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BLSP_UART_SIM_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBLSPUARTSIMClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBLSPUARTSIMClkDomainClks)/sizeof(HAL_clk_mBLSPUARTSIMClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


