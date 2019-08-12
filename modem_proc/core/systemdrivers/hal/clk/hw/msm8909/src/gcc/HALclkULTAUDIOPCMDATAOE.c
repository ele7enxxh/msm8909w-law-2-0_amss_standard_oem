/*
==============================================================================

FILE:         HALclkULTAUDIOPCMDATAOE.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   ULTAUDIO PCM DATA OE clocks.

   List of clock domains:
   -HAL_clk_mGCCULTAUDIOPCMDATAOEClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/gcc/HALclkULTAUDIOPCMDATAOE.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
07/01/2014            Auto-generated.

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
 *  HAL_clk_mULTAUDIOPCMDATAOEClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mULTAUDIOPCMDATAOEClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_pcm_data_oe_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_PCM_DATA_OE_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_PCM_DATA_OE_CLK
  },
};


/*
 * HAL_clk_mGCCULTAUDIOPCMDATAOEClkDomain
 *
 * ULTAUDIO PCM DATA OE clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCULTAUDIOPCMDATAOEClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ULTAUDIO_PCM_DATA_OE_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mULTAUDIOPCMDATAOEClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mULTAUDIOPCMDATAOEClkDomainClks)/sizeof(HAL_clk_mULTAUDIOPCMDATAOEClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mLPASSClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


