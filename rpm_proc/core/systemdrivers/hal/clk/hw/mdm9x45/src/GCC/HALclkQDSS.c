/*
==============================================================================

FILE:         HALclkQDSS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   QDSS clocks.

   List of clock domains:
     - HAL_clk_mGCCQDSSTSCTRClkDomain
     - HAL_clk_mGCCQDSSTSCTRPREDIV3ClkDomain
     - HAL_clk_mGCCQDSSDAPClkDomain
     - HAL_clk_mGCCQDSSTSCTRPREDIV8ClkDomain
     - HAL_clk_mGCCQDSSSTMClkDomain
     - HAL_clk_mGCCQDSSTSCTRDIV4ClkDomain
     - HAL_clk_mGCCQDSSTRACECLKINClkDomain
     - HAL_clk_mGCCQDSSATClkDomain
     - HAL_clk_mGCCQDSSTSCTRPREDIV16ClkDomain


   List of power domains:



==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/GCC/HALclkQDSS.c#1 $

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
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


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
};


/*
 * HAL_clk_mGCCQDSSTSCTRClkDomain
 *
 * QDSSTSCTR clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_TSCTR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSTSCTRClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTSCTRClkDomainClks)/sizeof(HAL_clk_mQDSSTSCTRClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSTSCTRPREDIV3ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTSCTRPREDIV3ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div3_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV3_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV3_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTSCTRPREDIV3ClkDomain
 *
 * QDSSTSCTRPREDIV3 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRPREDIV3ClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mQDSSTSCTRPREDIV3ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTSCTRPREDIV3ClkDomainClks)/sizeof(HAL_clk_mQDSSTSCTRPREDIV3ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSDAPClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSDAPClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_dap_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_DAP_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_DAP_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSDAPClkDomain
 *
 * QDSSDAP clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSDAPClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mQDSSDAPClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSDAPClkDomainClks)/sizeof(HAL_clk_mQDSSDAPClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSTSCTRPREDIV8ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTSCTRPREDIV8ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div8_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV8_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV8_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTSCTRPREDIV8ClkDomain
 *
 * QDSSTSCTRPREDIV8 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRPREDIV8ClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mQDSSTSCTRPREDIV8ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTSCTRPREDIV8ClkDomainClks)/sizeof(HAL_clk_mQDSSTSCTRPREDIV8ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
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
    /* .szClockName      = */ "gcc_qdss_stm_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_STM_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_STM_CLK
  },
  {
    /* .szClockName      = */ "gcc_sys_noc_qdss_stm_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_QDSS_STM_AXI_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_QDSS_STM_AXI_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSSTMClkDomain
 *
 * QDSSSTM clock domain.
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
 *  HAL_clk_mQDSSTSCTRDIV4ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTSCTRDIV4ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div4_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV4_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV4_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTSCTRDIV4ClkDomain
 *
 * QDSSTSCTRDIV4 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRDIV4ClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mQDSSTSCTRDIV4ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTSCTRDIV4ClkDomainClks)/sizeof(HAL_clk_mQDSSTSCTRDIV4ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSTRACECLKINClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTRACECLKINClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_traceclkin_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TRACECLKIN_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TRACECLKIN_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTRACECLKINClkDomain
 *
 * QDSSTRACECLKIN clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRACECLKINClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_TRACECLKIN_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSTRACECLKINClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTRACECLKINClkDomainClks)/sizeof(HAL_clk_mQDSSTRACECLKINClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSATClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSATClkDomainClks[] =
{
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
  {
    /* .szClockName      = */ "gcc_sys_noc_at_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_AT_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_AT_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSATClkDomain
 *
 * QDSSAT clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSATClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_QDSS_AT_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQDSSATClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSATClkDomainClks)/sizeof(HAL_clk_mQDSSATClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mQDSSTSCTRPREDIV16ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQDSSTSCTRPREDIV16ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_qdss_tsctr_div16_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_TSCTR_DIV16_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_TSCTR_DIV16_CLK
  },
};


/*
 * HAL_clk_mGCCQDSSTSCTRPREDIV16ClkDomain
 *
 * QDSSTSCTRPREDIV16 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRPREDIV16ClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mQDSSTSCTRPREDIV16ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQDSSTSCTRPREDIV16ClkDomainClks)/sizeof(HAL_clk_mQDSSTSCTRPREDIV16ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

