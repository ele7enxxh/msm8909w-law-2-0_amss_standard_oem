/*
==============================================================================

FILE:         HALclkGCC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   GCC clocks.

   List of clock domains:
   -HAL_clk_mGCCSLEEPCLKClkDomain
   -HAL_clk_mGCCXOClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8909/src/GCC/HALclkGCC.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
08/21/2014            Auto-generated.

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
 *  HAL_clk_mSLEEPCLKClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSLEEPCLKClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qpic_system_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QPIC_SYSTEM_CBCR), HWIO_OFFS(GCC_QPIC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QPIC_SYSTEM_CLK
  },
  {
    /* .szClockName      = */ "gcc_usb_hs_inactivity_timers_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_USB_HS_INACTIVITY_TIMERS_CBCR), HWIO_OFFS(GCC_USB_HS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_USB_HS_INACTIVITY_TIMERS_CLK
  },
  {
    /* .szClockName      = */ "gcc_usb2a_phy_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_USB2A_PHY_SLEEP_CBCR), HWIO_OFFS(GCC_USB2A_PHY_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_USB2A_PHY_SLEEP_CLK
  },
  {
    /* .szClockName      = */ "gcc_blsp1_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_SLEEP_CBCR), HWIO_OFFS(GCC_BLSP1_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BLSP1_SLEEP_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_SLEEP_CLK
  },
  {
    /* .szClockName      = */ "gcc_rpm_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RPM_SLEEP_CBCR), HWIO_OFFS(GCC_RPM_MISC), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_SLEEP_CLK
  },
  {
    /* .szClockName      = */ "gcc_im_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_IM_SLEEP_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_IM_SLEEP_CLK
  },
  {
    /* .szClockName      = */ "gcc_bimc_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_SLEEP_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_SLEEP_CLK
  },
  {
    /* .szClockName      = */ "gcc_ddr_dim_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DDR_DIM_SLEEP_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DDR_DIM_SLEEP_CLK
  },
};


/*
 * HAL_clk_mGCCSLEEPCLKClkDomain
 *
 * SLEEP CLK clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSLEEPCLKClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_GCC_SLEEP_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mSLEEPCLKClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSLEEPCLKClkDomainClks)/sizeof(HAL_clk_mSLEEPCLKClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mXOClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mXOClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_pdm_xo4_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PDM_XO4_CBCR), HWIO_OFFS(GCC_PDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PDM_XO4_CLK
  },
  {
    /* .szClockName      = */ "gcc_rpm_timer_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RPM_TIMER_CBCR), HWIO_OFFS(GCC_RPM_MISC), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_TIMER_CLK
  },
  {
    /* .szClockName      = */ "gcc_spdm_ff_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_FF_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_FF_CLK
  },
  {
    /* .szClockName      = */ "gcc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_XO_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_XO_CLK
  },
  {
    /* .szClockName      = */ "gcc_xo_div4_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_XO_DIV4_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_XO_DIV4_CLK
  },
  {
    /* .szClockName      = */ "gcc_dcd_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DCD_XO_CBCR), HWIO_OFFS(GCC_DCD_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DCD_XO_CLK
  },
};


/*
 * HAL_clk_mGCCXOClkDomain
 *
 * XO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCXOClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_GCC_XO_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mXOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mXOClkDomainClks)/sizeof(HAL_clk_mXOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


