/*
==============================================================================

FILE:         HALclkULTAUDIOEXTMCLK2.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   ULTAUDIO EXT MCLK2 clocks.

   List of clock domains:
   -HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/gcc/HALclkULTAUDIOEXTMCLK2.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
07/08/2014            Auto-generated.

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mLPASSClockDomainControl;


/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mULTAUDIOEXTMCLK2ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mULTAUDIOEXTMCLK2ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_ext_mclk2_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_EXT_MCLK2_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_EXT_MCLK2_CLK
  },
};


/*
 * HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomain
 *
 * ULTAUDIO EXT MCLK2 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ULTAUDIO_EXT_MCLK2_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mULTAUDIOEXTMCLK2ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mULTAUDIOEXTMCLK2ClkDomainClks)/sizeof(HAL_clk_mULTAUDIOEXTMCLK2ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mLPASSClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


