/*
==============================================================================

FILE:         HALclkRPM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   RPM clocks.

   List of clock domains:
   -HAL_clk_mGCCRPMClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8952/src/GCC/HALclkRPM.c#1 $

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
 *  HAL_clk_mRPMClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mRPMClkDomainClks[] =
{
  { /* Added by hand */
    /* .szClockName      = */ "gcc_rpm_proc_fclk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_PROC_FCLK
  },
  {
    /* .szClockName      = */ "gcc_pcnoc_rpm_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_RPM_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_RPM_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_rpm_cfg_xpu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RPM_CFG_XPU_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_CFG_XPU_CLK
  },
  {
    /* .szClockName      = */ "gcc_rpm_proc_hclk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RPM_PROC_HCLK_CBCR), HWIO_OFFS(GCC_RPM_MISC), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_PROC_HCLK
  },
  {
    /* .szClockName      = */ "gcc_rpm_bus_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RPM_BUS_AHB_CBCR), HWIO_OFFS(GCC_RPM_MISC), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_BUS_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_spdm_rpm_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_RPM_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_RPM_CY_CLK
  },
  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout6_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT6_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT6_AHB_CLK
  },
};


/*
 * HAL_clk_mGCCRPMClkDomain
 *
 * RPM clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCRPMClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_RPM_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mRPMClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mRPMClkDomainClks)/sizeof(HAL_clk_mRPMClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


