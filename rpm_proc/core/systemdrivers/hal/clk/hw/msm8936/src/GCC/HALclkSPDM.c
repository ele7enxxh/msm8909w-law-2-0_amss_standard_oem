/*
==============================================================================

FILE:         HALclkSPDM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   SPDM clocks.

   List of clock domains:
   -HAL_clk_mGCCSPDMBIMCDDRCH0CYDIV8ClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8936/src/GCC/HALclkSPDM.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
04/01/2014            Auto-generated.

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
 *  HAL_clk_mSPDMBIMCDDRCH0CYDIV8ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMBIMCDDRCH0CYDIV8ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_spdm_bimc_ddr_ch0_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_BIMC_DDR_CH0_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_BIMC_DDR_CH0_CY_CLK
  },
};


/*
 * HAL_clk_mGCCSPDMBIMCDDRCH0CYDIV8ClkDomain
 *
 * SPDM BIMC DDR CH0 CYDIV8 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMBIMCDDRCH0CYDIV8ClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mSPDMBIMCDDRCH0CYDIV8ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMBIMCDDRCH0CYDIV8ClkDomainClks)/sizeof(HAL_clk_mSPDMBIMCDDRCH0CYDIV8ClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};

/*                           
 *  HAL_clk_mSPDMDEBUGCYDIV8ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMDEBUGCYDIV8ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_spdm_debug_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_DEBUG_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ NULL
  },
};


/*
 * HAL_clk_mGCCSPDMDEBUGCYDIV8ClkDomain
 *
 * SPDM DEBUG CYDIV4 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMDEBUGCYDIV8ClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mSPDMDEBUGCYDIV8ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMDEBUGCYDIV8ClkDomainClks)/sizeof(HAL_clk_mSPDMDEBUGCYDIV8ClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};


