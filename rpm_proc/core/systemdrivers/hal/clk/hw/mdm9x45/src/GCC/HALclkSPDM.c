/*
==============================================================================

FILE:         HALclkSPDM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   SPDM clocks.

   List of clock domains:
     - HAL_clk_mGCCSPDMFFClkDomain
     - HAL_clk_mGCCSPDMDEBUGCY4XClkDomain
     - HAL_clk_mGCCSPDMRPMCYClkDomain
     - HAL_clk_mGCCSPDMPNOCCYClkDomain
     - HAL_clk_mGCCSPDMBIMCCY4XClkDomain
     - HAL_clk_mGCCSPDMSNOCCY2XClkDomain
     - HAL_clk_mGCCSPDMULTAUDIOAHBFABRICCYClkDomain


   List of power domains:



==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/GCC/HALclkSPDM.c#1 $

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
 *  HAL_clk_mSPDMFFClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMFFClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_spdm_ff_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_FF_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_FF_CLK
  },
};


/*
 * HAL_clk_mGCCSPDMFFClkDomain
 *
 * SPDMFF clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMFFClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mSPDMFFClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMFFClkDomainClks)/sizeof(HAL_clk_mSPDMFFClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mSPDMDEBUGCY4XClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMDEBUGCY4XClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_spdm_debug_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_DEBUG_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ 0
  },
};


/*
 * HAL_clk_mGCCSPDMDEBUGCY4XClkDomain
 *
 * SPDMDEBUGCY4X clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMDEBUGCY4XClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mSPDMDEBUGCY4XClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMDEBUGCY4XClkDomainClks)/sizeof(HAL_clk_mSPDMDEBUGCY4XClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mSPDMRPMCYClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMRPMCYClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_spdm_rpm_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_RPM_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_RPM_CY_CLK
  },
};


/*
 * HAL_clk_mGCCSPDMRPMCYClkDomain
 *
 * SPDMRPMCY clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMRPMCYClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mSPDMRPMCYClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMRPMCYClkDomainClks)/sizeof(HAL_clk_mSPDMRPMCYClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mSPDMPNOCCYClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMPNOCCYClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_spdm_pcnoc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_PCNOC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_PCNOC_CY_CLK
  },
};


/*
 * HAL_clk_mGCCSPDMPNOCCYClkDomain
 *
 * SPDMPNOCCY clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMPNOCCYClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mSPDMPNOCCYClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMPNOCCYClkDomainClks)/sizeof(HAL_clk_mSPDMPNOCCYClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mSPDMBIMCCY4XClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMBIMCCY4XClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_spdm_bimc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_BIMC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_BIMC_CY_CLK
  },
};


/*
 * HAL_clk_mGCCSPDMBIMCCY4XClkDomain
 *
 * SPDMBIMCCY4X clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMBIMCCY4XClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mSPDMBIMCCY4XClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMBIMCCY4XClkDomainClks)/sizeof(HAL_clk_mSPDMBIMCCY4XClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mSPDMSNOCCY2XClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMSNOCCY2XClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_spdm_snoc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_SNOC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_SNOC_CY_CLK
  },
};


/*
 * HAL_clk_mGCCSPDMSNOCCY2XClkDomain
 *
 * SPDMSNOCCY2X clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMSNOCCY2XClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mSPDMSNOCCY2XClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMSNOCCY2XClkDomainClks)/sizeof(HAL_clk_mSPDMSNOCCY2XClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mSPDMULTAUDIOAHBFABRICCYClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPDMULTAUDIOAHBFABRICCYClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_ahbfabric_efabric_spdm_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AHBFABRIC_EFABRIC_SPDM_CLK
  },
};


/*
 * HAL_clk_mGCCSPDMULTAUDIOAHBFABRICCYClkDomain
 *
 * SPDMULTAUDIOAHBFABRICCY clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMULTAUDIOAHBFABRICCYClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mSPDMULTAUDIOAHBFABRICCYClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPDMULTAUDIOAHBFABRICCYClkDomainClks)/sizeof(HAL_clk_mSPDMULTAUDIOAHBFABRICCYClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

