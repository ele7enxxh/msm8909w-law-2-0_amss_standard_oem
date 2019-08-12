/*
==============================================================================

FILE:         HALclkQDSS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   QDSS clocks.

   List of clock domains:
   -HAL_clk_mGCCQDSSATBClkDomain
   -HAL_clk_mGCCQDSSSTMClkDomain
   -HAL_clk_mGCCQDSSTRACEClkDomain
   -HAL_clk_mGCCQDSSTSCTRClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8936/src/GCC/HALclkQDSS.c#1 $

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
extern HAL_clk_ClockDomainControlType  HAL_clk_mQDSSATBClockDomainControl;
extern HAL_clk_ClockDomainControlType  HAL_clk_mQDSSTRACE_TSCTRDomainControl;

/* ============================================================================
**    Data
** ==========================================================================*/

                                    
/*                           
 *  HAL_clk_mQDSSATBClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSATBClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_sys_noc_at_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_AT_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_AT_CLK
  },
  {
    /* .szClockName      = */ "gcc_pcnoc_at_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_AT_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_AT_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_at_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_AT_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_AT_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSATBClkDomain
 *
 * QDSS ATB clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSATBClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_AT_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSATBClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSATBClkDomainClks)/sizeof(HAL_clk_mQDSSATBClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mQDSSATBClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mQDSSSTMClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSSTMClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_sys_noc_qdss_stm_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_QDSS_STM_AXI_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_QDSS_STM_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_stm_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_STM_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_STM_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSSTMClkDomain
 *
 * QDSS STM clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSSTMClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_STM_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSSTMClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSSTMClkDomainClks)/sizeof(HAL_clk_mQDSSSTMClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mQDSSTRACEClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTRACEClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_traceclkin_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TRACECLKIN_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TRACECLKIN_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTRACEClkDomain
 *
 * QDSS TRACE clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRACEClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_TRACECLKIN_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSTRACEClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTRACEClkDomainClks)/sizeof(HAL_clk_mQDSSTRACEClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mQDSSTRACE_TSCTRDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mQDSSTSCTRClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTSCTRClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div2_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV2_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV2_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div3_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV3_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV3_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_dap_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_DAP_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_DAP_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div4_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV4_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV4_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div8_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV8_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV8_CLK
  },
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div16_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV16_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV16_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTSCTRClkDomain
 *
 * QDSS TSCTR clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_TSCTR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSTSCTRClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTSCTRClkDomainClks)/sizeof(HAL_clk_mQDSSTSCTRClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mQDSSTRACE_TSCTRDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


