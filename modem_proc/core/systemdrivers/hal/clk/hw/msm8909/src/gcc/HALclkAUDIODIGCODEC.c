/*
==============================================================================

FILE:         HALclkAUDIODIGCODEC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   AUDIO DIGCODEC clocks.

   List of clock domains:
   -HAL_clk_mGCCULTAUDIODIGCODECClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/gcc/HALclkAUDIODIGCODEC.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
09/04/2013            Auto-generated.

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mLPASSClockDomainControl;


/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mULTAUDIODIGCODECClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mULTAUDIODIGCODECClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_codec_digcodec_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_CODEC_DIGCODEC_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_CODEC_DIGCODEC_CLK
  },
};


/*
 * HAL_clk_mGCCULTAUDIODIGCODECClkDomain
 *
 * ULTAUDIO DIGCODEC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCULTAUDIODIGCODECClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_CODEC_DIGCODEC_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mULTAUDIODIGCODECClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mULTAUDIODIGCODECClkDomainClks)/sizeof(HAL_clk_mULTAUDIODIGCODECClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mLPASSClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


