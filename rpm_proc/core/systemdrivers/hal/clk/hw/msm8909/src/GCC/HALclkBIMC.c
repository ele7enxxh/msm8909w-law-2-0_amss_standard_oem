/*
==============================================================================

FILE:         HALclkBIMC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BIMC clocks.

   List of clock domains:
   -HAL_clk_mGCCBIMCDDRClkDomain
   -HAL_clk_mGCCBIMCDDRXOClkDomain
   -HAL_clk_mGCCBIMCGPUClkDomain
   List of power domains:
   -HAL_clk_mGCCBIMCPowerDomain


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8909/src/GCC/HALclkBIMC.c#1 $

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
 *  HAL_clk_mBIMCDDRClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCDDRClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_apss_tcu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_APSS_TCU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), HAL_CLK_FMSK(PROC_SMMU_CLK_BRANCH_ENA_VOTE, APSS_TCU_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_APSS_TCU_CLK
  },
  {
    /* .szClockName      = */ "gcc_gfx_tcu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GFX_TCU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), HAL_CLK_FMSK(PROC_SMMU_CLK_BRANCH_ENA_VOTE, GFX_TCU_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_GFX_TCU_CLK
  },
  {
    /* .szClockName      = */ "gcc_mss_tbu_Q6_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_TBU_Q6_AXI_CBCR), HWIO_OFFS(GCC_SMMU_BCR), HAL_CLK_FMSK(PROC_SMMU_CLK_BRANCH_ENA_VOTE, MSS_TBU_Q6_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_TBU_Q6_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_mss_q6_bimc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_Q6_BIMC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_Q6_BIMC_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_spdm_bimc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_BIMC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_BIMC_CY_CLK
  },
  {
    /* .szClockName      = */ "gcc_bimc_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_CLK
  },
  {
    /* .szClockName      = */ "gcc_bimc_apss_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_APSS_AXI_CBCR), HWIO_OFFS(GCC_BIMC_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_APSS_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_bimc_tcu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_TCU_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_TCU_CLK
  },
  {
    /* .szClockName      = */ "gcc_apss_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_APSS_AXI_CBCR), 0, HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_APSS_AXI_CLK
  },
};


/*
 * HAL_clk_mGCCBIMCDDRClkDomain
 *
 * BIMC DDR clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BIMC_DDR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCDDRClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCDDRClkDomainClks)/sizeof(HAL_clk_mBIMCDDRClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mBIMCDDRXOClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCDDRXOClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_XO_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_XO_CLK
  },
  {
    /* .szClockName      = */ "gcc_ddr_dim_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DDR_DIM_CFG_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DDR_DIM_CFG_CLK
  },
  
};


/*
 * HAL_clk_mGCCBIMCDDRXOClkDomain
 *
 * BIMC DDR XO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRXOClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BIMC_DDR_XO_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCDDRXOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCDDRXOClkDomainClks)/sizeof(HAL_clk_mBIMCDDRXOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


                                    
/*                           
 *  HAL_clk_mBIMCGPUClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCGPUClkDomainClks[] =
{
  #if 0
  {
    /* .szClockName      = */ "gcc_gfx_tbu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GFX_TBU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_GFX_TBU_CLK
  },
  #endif
  {
    /* .szClockName      = */ "gcc_bimc_gpu_clk",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_GPU_CLK
  },
  #if 0
  {
    /* .szClockName      = */ "gcc_bimc_gfx_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_GFX_CBCR), HWIO_OFFS(GCC_OXILI_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_GFX_CLK
  },
  #endif
};


/*
 * HAL_clk_mGCCBIMCGPUClkDomain
 *
 * BIMC GPU clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCGPUClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BIMC_GPU_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCGPUClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCGPUClkDomainClks)/sizeof(HAL_clk_mBIMCGPUClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};



/*
 * HAL_clk_mGCCBIMCPowerDomain
 *
 * BIMC power domain.
 */
HAL_clk_PowerDomainDescType HAL_clk_mGCCBIMCPowerDomain =
{
  /* .szPowerDomainName       = */ "VDD_BIMC",
  /* .nGDSCRAddr              = */ HWIO_OFFS(GCC_BIMC_GDSCR),
  /* .pmControl               = */ &HAL_clk_mGenericPowerDomainControl,
  /* .pmNextPowerDomain       = */ NULL
};

/* ===========================================================================
**  HAL_clk_BIMCGetMiscInfo
**
** ======================================================================== */

uint32 HAL_clk_BIMCGetMiscInfo( void )
{
  return HWIO_IN(GCC_BIMC_MISC);
}


/* ===========================================================================
**  HAL_clk_GetDEHRStatus
**
** ======================================================================== */

uint32 HAL_clk_GetDEHRStatus( void )
{
  return HWIO_IN(DEHR_BIMC_DEHR_DMA_STATUS);
}

/* ===========================================================================
**  HAL_clk_IsDEHREnable
**
** ======================================================================== */

boolean HAL_clk_IsDEHREnable( void )
{
  return ( HWIO_INF(DEHR_BIMC_DEHR_SEC_CFG, DEHR_ENA) != 0 );
}


