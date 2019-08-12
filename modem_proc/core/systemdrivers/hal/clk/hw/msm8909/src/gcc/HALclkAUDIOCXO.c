/*
==============================================================================

FILE:         HALclkAUDIOCXO.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   AUDIO CXO clocks.

   List of clock domains:
   -HAL_clk_mGCCULTAUDIOXOClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/gcc/HALclkAUDIOCXO.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
09/04/2013            Auto-generated.

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
 *  HAL_clk_mULTAUDIOXOClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mULTAUDIOXOClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_avsync_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_AVSYNC_XO_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AVSYNC_XO_CLK
  },
  {
    /* .szClockName      = */ "gcc_ultaudio_stc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_STC_XO_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_STC_XO_CLK
  },
};


/*
 * HAL_clk_mGCCULTAUDIOXOClkDomain
 *
 * ULTAUDIO XO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCULTAUDIOXOClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ULTAUDIO_XO_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mULTAUDIOXOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mULTAUDIOXOClkDomainClks)/sizeof(HAL_clk_mULTAUDIOXOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mLPASSClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


