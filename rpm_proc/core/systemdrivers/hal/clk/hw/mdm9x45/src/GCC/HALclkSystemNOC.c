/*
==============================================================================

FILE:         HALclkSYSTEMNOC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   SYSTEMNOC clocks.

   List of clock domains:
     - HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain


   List of power domains:



==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/GCC/HALclkSystemNOC.c#1 $

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
 *  HAL_clk_mSYSTEMNOCBFDCDClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSYSTEMNOCBFDCDClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_sysnoc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_SYSNOC_AXI_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_SYSNOC_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_etr_usb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_ETR_USB_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_ETR_USB_CLK
  },
  {
    /* .szClockName      = */ "gcc_sys_noc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_AXI_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_AXI_CLK
  },
};


/*
 * HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain
 *
 * SYSTEMNOCBFDCD clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_SYSTEM_NOC_BFDCD_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mSYSTEMNOCBFDCDClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSYSTEMNOCBFDCDClkDomainClks)/sizeof(HAL_clk_mSYSTEMNOCBFDCDClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

