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

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8916/src/GCC/HALclkBIMC.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
09/23/2013            Auto-generated.

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
static void HAL_clk_BIMCConfigMux( HAL_clk_ClockDomainDescType *pmClockDomainDesc, const HAL_clk_ClockMuxConfigType * );

/* ============================================================================
**    debug typedefs and variables
** ==========================================================================*/

typedef enum {
  CLK_BIMC_HAL_BIMC_DEFAULT,
  CLK_BIMC_HAL_BIMC_ENTER,
  CLK_BIMC_HAL_BIMC_UPDATE_WRITE,
  CLK_BIMC_HAL_BIMC_UPDATE_DONE,
  CLK_BIMC_HAL_GPU_ENTER,
  CLK_BIMC_HAL_GPU_UPDATE_WRITE,
  CLK_BIMC_HAL_GPU_UPDATE_LOGS_1,
  CLK_BIMC_HAL_GPU_UPDATE_DONE,
  CLK_BIMC_HAL_GPU_UPDATE_LOGS_2
} clk_dbg_hal_bimc_marker_type;

volatile uint32 clk_dbg_bimc_switch_count = 0;
volatile uint32 clk_dbg_loop_count = 0;
volatile uint32 clk_dbg_loop_count_max = 0;
volatile clk_dbg_hal_bimc_marker_type clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_BIMC_DEFAULT;
#define CLK_DBG_REGLOG_MAX 20

struct clk_dbg_RegDump_type {
  uint32 GCC_BIMC_GFX_CBCR;
  uint32 GCC_GFX_TBU_CBCR;
  uint32 GCC_BIMC_GPU_CBCR;
  uint32 GCC_OXILI_GDSCR;
  uint32 GCC_BIMC_GPU_CFG_RCGR;
  uint32 GCC_BIMC_GPU_CMD_RCGR;
  uint32 GCC_GFX_TBU_BCR;
};

struct clk_dbg_clkSpeedDump_type {
  uint32 gcc_bimc_gfx_clk;
  uint32 gcc_gfx_tbu_clk;
  uint32 gcc_bimc_gpu_clk;
};

volatile struct clk_dbg_RegDump_type clk_dbg_RegDump[CLK_DBG_REGLOG_MAX];
volatile struct clk_dbg_RegDump_type clk_dbg_RegDump_issue, clk_dbg_RegDump_issue_2;
volatile struct clk_dbg_clkSpeedDump_type clk_dbg_clkSpeedDump[2];


//volatile clk_dbg_RegDump_type clk_dbg_RegDump;
volatile uint32 clk_dbg_RegDump_index=0;
volatile uint32  clk_dbg_clkSpeedDump_index=0;
void HAL_clk_CalcClockFreq_Util(volatile  uint32  nTestClock,volatile  uint32  *pnFreqHz);

void clk_dbg_dump_regs()
{
  uint32 i=clk_dbg_RegDump_index;
  clk_dbg_RegDump[i].GCC_BIMC_GFX_CBCR = HWIO_IN(GCC_BIMC_GFX_CBCR);
  clk_dbg_RegDump[i].GCC_GFX_TBU_CBCR = HWIO_IN(GCC_GFX_TBU_CBCR);
  clk_dbg_RegDump[i].GCC_BIMC_GPU_CBCR = HWIO_IN(GCC_BIMC_GPU_CBCR);
  clk_dbg_RegDump[i].GCC_OXILI_GDSCR = HWIO_IN(GCC_OXILI_GDSCR);
  clk_dbg_RegDump[i].GCC_BIMC_GPU_CFG_RCGR = HWIO_IN(GCC_BIMC_GPU_CFG_RCGR);
  clk_dbg_RegDump[i].GCC_BIMC_GPU_CMD_RCGR = HWIO_IN(GCC_BIMC_GPU_CMD_RCGR);
  clk_dbg_RegDump[i].GCC_GFX_TBU_BCR = HWIO_IN(GCC_GFX_TBU_BCR);
  clk_dbg_RegDump_index = (clk_dbg_RegDump_index+1)%CLK_DBG_REGLOG_MAX;
}
void clk_dbg_dump_speeds()
{
  uint32 i=clk_dbg_clkSpeedDump_index;
  volatile uint32 nTestclock,*ptr;

  nTestclock = HAL_CLK_GCC_TEST_GCC_BIMC_GFX_CLK;
  ptr = &clk_dbg_clkSpeedDump[i].gcc_bimc_gfx_clk;
  HAL_clk_CalcClockFreq_Util(nTestclock, ptr);
  
  nTestclock = HAL_CLK_GCC_TEST_GCC_GFX_TBU_CLK;
  ptr = &clk_dbg_clkSpeedDump[i].gcc_gfx_tbu_clk;
  HAL_clk_CalcClockFreq_Util(nTestclock, ptr);

  nTestclock = HAL_CLK_GCC_TEST_GCC_BIMC_GPU_CLK;
  ptr = &clk_dbg_clkSpeedDump[i].gcc_bimc_gpu_clk;
  HAL_clk_CalcClockFreq_Util(nTestclock, ptr);

  clk_dbg_clkSpeedDump_index = (clk_dbg_clkSpeedDump_index+1)%2;
}
void clk_dbg_dump_regs_issue()
{
  clk_dbg_RegDump_issue.GCC_BIMC_GFX_CBCR = HWIO_IN(GCC_BIMC_GFX_CBCR);
  clk_dbg_RegDump_issue.GCC_GFX_TBU_CBCR = HWIO_IN(GCC_GFX_TBU_CBCR);
  clk_dbg_RegDump_issue.GCC_BIMC_GPU_CBCR = HWIO_IN(GCC_BIMC_GPU_CBCR);
  clk_dbg_RegDump_issue.GCC_OXILI_GDSCR = HWIO_IN(GCC_OXILI_GDSCR);
  clk_dbg_RegDump_issue.GCC_BIMC_GPU_CMD_RCGR = HWIO_IN(GCC_BIMC_GPU_CMD_RCGR);
  clk_dbg_RegDump_issue.GCC_GFX_TBU_BCR = HWIO_IN(GCC_GFX_TBU_BCR);
}

void clk_dbg_dump_regs_issue_2()
{
  clk_dbg_RegDump_issue_2.GCC_BIMC_GFX_CBCR = HWIO_IN(GCC_BIMC_GFX_CBCR);
  clk_dbg_RegDump_issue_2.GCC_GFX_TBU_CBCR = HWIO_IN(GCC_GFX_TBU_CBCR);
  clk_dbg_RegDump_issue_2.GCC_BIMC_GPU_CBCR = HWIO_IN(GCC_BIMC_GPU_CBCR);
  clk_dbg_RegDump_issue_2.GCC_OXILI_GDSCR = HWIO_IN(GCC_OXILI_GDSCR);
  clk_dbg_RegDump_issue_2.GCC_BIMC_GPU_CMD_RCGR = HWIO_IN(GCC_BIMC_GPU_CMD_RCGR);
  clk_dbg_RegDump_issue_2.GCC_GFX_TBU_BCR = HWIO_IN(GCC_GFX_TBU_BCR);
}

/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;
extern HAL_clk_SourceMapType aGCCSourceMap[];

/* ============================================================================
**    Data
** ==========================================================================*/

/*
 * HAL_clk_BIMCDDRDomainControl
 *
 * BIMC clock domain control. (Note: this is not generated by HALclkGen.)
 */
HAL_clk_ClockDomainControlType HAL_clk_BIMCDDRDomainControl =
{
  /* .ConfigMux        = */ HAL_clk_BIMCConfigMux,
  /* .DetectMuxConfig = */  HAL_clk_GenericDetectMuxConfig,
  /* .pmSourceMap     = */  aGCCSourceMap
};
                                  
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
  /* .pmControl            = */ &HAL_clk_BIMCDDRDomainControl,
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
    /* .szClockName      = */ "gcc_pcnoc_ddr_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_DDR_CFG_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_DDR_CFG_CLK
  },
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
  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout5_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT5_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT5_AHB_CLK
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
//static HAL_clk_ClockDescType HAL_clk_mBIMCGPUClkDomainClks[] =
//{
//  {
//    /* .szClockName      = */ "gcc_gfx_tbu_clk",
//    /* .mRegisters       = */ { HWIO_OFFS(GCC_GFX_TBU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), HAL_CLK_FMSK(PROC_SMMU_CLK_BRANCH_ENA_VOTE, GFX_TBU_CLK_ENA) },
//    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
//    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_GFX_TBU_CLK
//  },
//  {
//    /* .szClockName      = */ "gcc_bimc_gpu_clk",
//    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_GPU_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
//    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
//    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_GPU_CLK
//  },
//  {
//    /* .szClockName      = */ "gcc_bimc_gfx_clk",
//    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_GFX_CBCR), HWIO_OFFS(GCC_OXILI_BCR), {0, 0} },
//    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
//    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_GFX_CLK
//  },
// };


/*
 * HAL_clk_mGCCBIMCGPUClkDomain
 *
 * BIMC GPU clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCGPUClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BIMC_GPU_CMD_RCGR),
  /* .pmClocks             = */ NULL,
  /* .nClockCount          = */ 0,
  /* .pmControl            = */ NULL,
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

/* ===========================================================================
**  HAL_clk_BIMCConfigMux
**
** ======================================================================== */

void HAL_clk_BIMCConfigMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  uint32                 nCmdCGRAddr, nCmdCGRVal;
  uint32                 nCfgCGRAddr, nCfgCGRVal;
  uint32                 nDiv2x;
  uint32                 nSourceIndex = 0;
  HAL_clk_SourceMapType *pmSourceMap;


  /*
   * Sanity check
   */
  if((pmConfig                                    == NULL) ||
     (pmClockDomainDesc                           == NULL) ||
     (pmClockDomainDesc->nCGRAddr                 == 0   ) || 
     (pmClockDomainDesc->pmControl                == NULL) ||
     (pmClockDomainDesc->pmControl->pmSourceMap   == NULL) )
  {
    return;
  }

  /*
   * Get current CMD and CFG register values
   */
  nCmdCGRAddr = pmClockDomainDesc->nCGRAddr;
  nCmdCGRVal  = inpdw(nCmdCGRAddr);
  nCfgCGRAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_CFG_REG_OFFSET;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Clear the fields
   */
  nCfgCGRVal &= ~(HAL_CLK_CFG_CGR_SRC_SEL_FMSK |
                  HAL_CLK_CFG_CGR_SRC_DIV_FMSK |
                  HAL_CLK_CFG_CGR_MODE_FMSK);

  /*
   * Get source index from source enum
   */
  pmSourceMap = pmClockDomainDesc->pmControl->pmSourceMap;
  nSourceIndex = HAL_clk_GenericSourceMapToHW(pmSourceMap, pmConfig->eSource);

  /*
   * Bail if could not find matching source index
   */
  if (nSourceIndex == HAL_CLK_SOURCE_INDEX_INVALID)
  {
    return;
  }

  /*
   * Program the source and divider values.
   */
  nCfgCGRVal |= ((nSourceIndex << HAL_CLK_CFG_CGR_SRC_SEL_SHFT) & HAL_CLK_CFG_CGR_SRC_SEL_FMSK);
  nCfgCGRVal |= ((HALF_DIVIDER(pmConfig) << HAL_CLK_CFG_CGR_SRC_DIV_SHFT) 
                  & HAL_CLK_CFG_CGR_SRC_DIV_FMSK);

  clk_dbg_bimc_switch_count++;
  clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_BIMC_ENTER;
  /*
   * Write the final CFG register value
   */
  outpdw(nCfgCGRAddr, nCfgCGRVal);

  /*
   * Trigger the update
   */
  nCmdCGRVal |= HAL_CLK_CMD_CFG_UPDATE_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_BIMC_UPDATE_WRITE;

  /*
   * Wait until update finishes
   */
  while(inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CFG_UPDATE_FMSK);

  clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_BIMC_UPDATE_DONE;
  
  /*
   * Get current CMD and CFG register values
   */
  nCmdCGRAddr = HAL_clk_mGCCBIMCGPUClkDomain.nCGRAddr;
  nCmdCGRVal  = inpdw(nCmdCGRAddr);
  nCfgCGRAddr = HAL_clk_mGCCBIMCGPUClkDomain.nCGRAddr + HAL_CLK_CFG_REG_OFFSET;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Clear the fields
   */
  nCfgCGRVal &= ~(HAL_CLK_CFG_CGR_SRC_SEL_FMSK |
                  HAL_CLK_CFG_CGR_SRC_DIV_FMSK |
                  HAL_CLK_CFG_CGR_MODE_FMSK);


  /* Choose a divider so that GCC_MMSS_BIMC_GFX_CFG_RCGR is half of BIMC freq */
  nDiv2x = pmConfig->nDiv2x;
  if((nDiv2x == 0) || (nDiv2x == 1)) {                   // If DDR divider is bypass (no divide)
    nDiv2x = 7;                       // use divider = 4 which is (2*4-1).
  } else if ((4 * nDiv2x) - 1 <= 31) { // If the divider can be doubled
    nDiv2x = (4 * nDiv2x) - 1;        // use double the divider.
  } else if ((2 * nDiv2x) - 1 <= 31) { // If the divider can be doubled
    nDiv2x = (2 * nDiv2x) - 1;        // use double the divider.
  } else {                            // Otherwise use the same divider
    nDiv2x = nDiv2x - 1;              // adjusted for HW register
  }

  /*
   * Program the source and divider values.
   */
  nCfgCGRVal |= ((nSourceIndex << HAL_CLK_CFG_CGR_SRC_SEL_SHFT) & HAL_CLK_CFG_CGR_SRC_SEL_FMSK);
  nCfgCGRVal |= ((nDiv2x << HAL_CLK_CFG_CGR_SRC_DIV_SHFT) & HAL_CLK_CFG_CGR_SRC_DIV_FMSK);

  /*
   * Write the final CFG register value
   */
  clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_GPU_ENTER;

  outpdw(nCfgCGRAddr, nCfgCGRVal);

  clk_dbg_dump_regs();  
  
  /*
   * Set ROOT_EN on for prevent glitch during switching since all the branch may be off.
   * This change applies to RPM image only.  Don't merge to other images.
   */
  nCmdCGRVal |= HAL_CLK_CMD_CGR_ROOT_EN_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait for the root to enable.
   */
  while(inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CGR_ROOT_OFF_FMSK);

  clk_dbg_dump_regs();

  /*
   * Trigger the update
   */
  nCmdCGRVal |= HAL_CLK_CMD_CFG_UPDATE_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_GPU_UPDATE_WRITE;
  clk_dbg_dump_regs();
  clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_GPU_UPDATE_LOGS_1;
  /*
   * Wait until update finishes
   */
  clk_dbg_loop_count = 0;
  while(inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CFG_UPDATE_FMSK){
    clk_dbg_loop_count++;
    if(clk_dbg_loop_count == 25)
    {
       clk_dbg_dump_regs_issue();
       clk_dbg_dump_speeds();
    }

    if(clk_dbg_loop_count == 300)
    {
      clk_dbg_dump_regs_issue_2();
      clk_dbg_dump_speeds();
    }
  };

  if (clk_dbg_loop_count > clk_dbg_loop_count_max)
  {
    clk_dbg_loop_count_max = clk_dbg_loop_count;
  }

  /*
   * Update success, clear ROOT_EN
   */
  nCmdCGRVal = inpdw(nCmdCGRAddr);
  nCmdCGRVal &= ~HAL_CLK_CMD_CGR_ROOT_EN_FMSK;

  outpdw(nCmdCGRAddr, nCmdCGRVal);
  
  clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_GPU_UPDATE_DONE;
  clk_dbg_dump_regs();
  clk_dbg_hal_bimc_marker = CLK_BIMC_HAL_GPU_UPDATE_LOGS_2;

} /* HAL_clk_BIMCConfigMux */


