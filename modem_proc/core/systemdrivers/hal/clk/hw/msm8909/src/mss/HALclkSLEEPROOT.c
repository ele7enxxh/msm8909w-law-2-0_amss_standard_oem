/*
==============================================================================

FILE:         HALclkSLEEPROOT.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   SLEEP ROOT clocks.

   List of clock domains:
   -HAL_clk_mMSSSLEEPROOTClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/mss/HALclkSLEEPROOT.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
10/01/2013            Auto-generated.

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


/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mSLEEPROOTClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSLEEPROOTClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_sleep_q6",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_QDSP6SS_SLEEP_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_SLEEP_Q6
  },
};


/*
 * HAL_clk_mMSSSLEEPROOTClkDomain
 *
 * SLEEP ROOT clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSSLEEPROOTClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mSLEEPROOTClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSLEEPROOTClkDomainClks)/sizeof(HAL_clk_mSLEEPROOTClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};


