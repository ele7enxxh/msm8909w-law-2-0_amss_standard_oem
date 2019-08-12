/*
==============================================================================

FILE:         HALclkBUS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BUS clocks.

   List of clock domains:
   -HAL_clk_mMSSBUSCLKClkDomain
   -HAL_clk_mMSSBUSCLKMODEMVDDMSSClkDomain
   -HAL_clk_mMSSBUSCLKNCVDDMSSClkDomain


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/mss/HALclkBUS.c#1 $

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


static boolean            HAL_clk_ConfigBusClkIsEnabled    ( HAL_clk_ClockDescType  *pmClockDesc );
static boolean            HAL_clk_ConfigBusClkIsOn         ( HAL_clk_ClockDescType  *pmClockDesc );
static void               HAL_clk_BusConfigMux             ( HAL_clk_ClockDomainDescType  *pmClockDomainDesc,const HAL_clk_ClockMuxConfigType *pmConfig);


/* ============================================================================
**    Externs
** ==========================================================================*/


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * aConfigBusSourceMap
 *
 * Config Bus HW source mapping
 *
 * NOTES:
 * - HAL_clk_SourceMapType is an array of mapped sources
 *   - see HALclkInternal.h.
 *
 * - If source index is reserved/not used in a clock diagram, please tie that
 *   to HAL_CLK_SOURCE_GROUND.
 *
 * - {HAL_CLK_SOURCE_NULL, HAL_CLK_SOURCE_INDEX_INVALID} is used to indicate
 *   the end of the mapping array. If we reach this element during our lookup,
 *   we'll know we could not find the matching source enum for the register
 *   value, or vice versa.
 *
 */
static HAL_clk_SourceMapType aConfigBusSourceMap[] =
{
  {HAL_CLK_SOURCE_XO,          0},
  {HAL_CLK_SOURCE_MPLL1,       1},
  {HAL_CLK_SOURCE_PLLTEST,     7},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
};


/*
 * HAL_clk_mConfigBusClockDomainControl
 *
 * Functions for controlling the MSS Config Bus clock domain
 */
HAL_clk_ClockDomainControlType HAL_clk_mConfigBusClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_BusConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aConfigBusSourceMap
};


/*
 * HAL_clk_mConfigBusClockControl
 *
 * Functions for controlling MSS Config Bus clock functions.
 */
HAL_clk_ClockControlType HAL_clk_mConfigBusClockControl =
{
  /* .Enable           = */ NULL,
  /* .Disable          = */ NULL,
  /* .IsEnabled        = */ HAL_clk_ConfigBusClkIsEnabled,
  /* .IsOn             = */ HAL_clk_ConfigBusClkIsOn,
  /* .Reset            = */ NULL,
  /* .IsReset          = */ NULL,
  /* .Config           = */ NULL,
  /* .DetectConfig     = */ NULL,
  /* .ConfigDivider    = */ NULL,
  /* .DetectDivider    = */ NULL,
  /* .ConfigFootswitch = */ NULL,
};


/*
 *  HAL_clk_mConfigBusClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mConfigBusClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_mss_config_bus",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mConfigBusClockControl,
    /* .nTestClock       = */ 0
  },
};


/*
 * HAL_clk_mMSSConfigBusClkDomain
 *
 * MSS config bus clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSConfigBusClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_BUS_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mConfigBusClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mConfigBusClkDomainClks)/sizeof(HAL_clk_mConfigBusClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mConfigBusClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

/*                           
 *  HAL_clk_mGCCSYSNOCClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCSYSNOCClkDomainClks[] =
{
  #if 0 //to avoid compilation error. this register doesnot exist.
  {
    /* .szClockName      = */ "clk_vmidmt_snoc_axi",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_VMIDMT_SNOC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_VMIDMT_SNOC_AXI
  },
  #endif
  {
    /* .szClockName      = */ "clk_modem_snoc_axi",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MODEM_SNOC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_MODEM_SNOC_AXI
  },
  {
    /* .szClockName      = */ "clk_nav_snoc_axi",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_NAV_SNOC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_NAV_SNOC_AXI
  },
};


/*
 * HAL_clk_mMSSGCCSYSNOCClkDomain
 *
 * GCC SYS NOC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSGCCSYSNOCClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mGCCSYSNOCClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCSYSNOCClkDomainClks)/sizeof(HAL_clk_mGCCSYSNOCClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};   

/*                           
 *  HAL_clk_mGCCPCNOCClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCPCNOCClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_modem_cfg_ahb",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MODEM_CFG_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_MODEM_CFG_AHB
  },
};


/*
 * HAL_clk_mMSSGCCPCNOCClkDomain
 *
 * GCC PCNOC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSGCCPCNOCClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mGCCPCNOCClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCPCNOCClkDomainClks)/sizeof(HAL_clk_mGCCPCNOCClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};
                                 
/*                           
 *  HAL_clk_mBUSCLKClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBUSCLKClkDomainClks[] =
{
#if 0 //to avoid compilation error. this register doesnot exist. need to discuss with MSS clock designer
  {
    /* .szClockName      = */ "clk_bus_coxm",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_COXM_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_COXM
  },
#endif  
  {
    /* .szClockName      = */ "clk_bus_uim0",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_UIM0_CBCR), HWIO_OFFS(MSS_UIM0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_UIM0
  },
  {
    /* .szClockName      = */ "clk_bus_uim1",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_UIM1_CBCR), HWIO_OFFS(MSS_UIM1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_UIM1
  },
  {
    /* .szClockName      = */ "clk_bus_uim2",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_UIM2_CBCR), HWIO_OFFS(MSS_UIM2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_UIM2
  },
  {
    /* .szClockName      = */ "clk_bus_csr",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_CSR_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_CSR
  },
  {
    /* .szClockName      = */ "clk_bus_atb",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_ATB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_ATB
  },
  {
    /* .szClockName      = */ "clk_bus_bridge",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_BRIDGE_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_BRIDGE
  },
  {
    /* .szClockName      = */ "clk_bus_q6",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_Q6_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_Q6
  },
  {
    /* .szClockName      = */ "clk_bus_slave_timeout",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_SLAVE_TIMEOUT_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_SLAVE_TIMEOUT
  },
#if 0 //to avoid compilation error. this register doesnot exist. need to discuss with MSS clock designer
  {
    /* .szClockName      = */ "clk_bus_rbcpr",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_RBCPR_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_RBCPR
  },
  {
    /* .szClockName      = */ "clk_bus_nc_hm_bridge_cx",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_NC_HM_BRIDGE_CX_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_NC_HM_BRIDGE_CX
  },
#endif
  {
    /* .szClockName      = */ "clk_bus_modem_bridge_cx",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_MODEM_BRIDGE_CX_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_MODEM_BRIDGE_CX
  },
  {
    /* .szClockName      = */ "clk_bus_nav",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_NAV_CBCR), HWIO_OFFS(MSS_NAV_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ 0
  },
  {
    /* .szClockName      = */ "clk_bus_nav_ce_bridge",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_NAV_CE_BRIDGE_CBCR), HWIO_OFFS(MSS_NAV_CE_BRIDGE_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_NC_HM_BRIDGE_CX
  },
  {
    /* .szClockName      = */ "clk_bus_crypto",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_CRYPTO_CBCR), HWIO_OFFS(MSS_CRYPTO_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ 0
  },
  {
    /* .szClockName      = */ "clk_bus_mgpi",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_MGPI_CBCR), HWIO_OFFS(MSS_MGPI_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ 0
  },
};


/*
 * HAL_clk_mMSSBUSCLKClkDomain
 *
 * BUS CLK clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSBUSCLKClkDomain =
{
  /* .nCGRAddr             = */ 0,  /* This is a shared resource -  Mux access removed */
  /* .pmClocks             = */ HAL_clk_mBUSCLKClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBUSCLKClkDomainClks)/sizeof(HAL_clk_mBUSCLKClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mBUSCLKMODEMVDDMSSClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBUSCLKMODEMVDDMSSClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_bus_modem",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_MODEM_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_MODEM_CX_TO_LS
  },
#if 0 //to avoid compilation error. this register doesnot exist. 
  {
    /* .szClockName      = */ "clk_bus_modem_cx_to_ls",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_MODEM_CX_TO_LS_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSS_TEST_CLK_BUS_MODEM_CX_TO_LS
  },
 #endif
};


/*
 * HAL_clk_mMSSBUSCLKMODEMVDDMSSClkDomain
 *
 * BUS CLK MODEM VDDMSS clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSBUSCLKMODEMVDDMSSClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mBUSCLKMODEMVDDMSSClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBUSCLKMODEMVDDMSSClkDomainClks)/sizeof(HAL_clk_mBUSCLKMODEMVDDMSSClkDomainClks[0]),
  /* .pmControl            = */ NULL,
  /* .pmNextClockDomain    = */ NULL
};


                                    


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/

/* ===========================================================================
**  HAL_clk_ConfigBusClkIsEnabled
**
** ======================================================================== */


boolean HAL_clk_ConfigBusClkIsEnabled
(
  HAL_clk_ClockDescType  *pmClockDesc
)
{
  /*
   * The config bus clock is always on when the Q6 is running.
   */
  return TRUE;

}  /* END HAL_clk_ConfigBusClkIsEnabled */


/* ===========================================================================
**  HAL_clk_ConfigBusClkIsOn
**
** ======================================================================== */

boolean HAL_clk_ConfigBusClkIsOn
(
  HAL_clk_ClockDescType  *pmClockDesc
)
{
  if(HWIO_INF(MSS_BUS_CMD_RCGR, ROOT_OFF))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}  /* END HAL_clk_ConfigBusClkIsOn */


/* ===========================================================================
**  HAL_clk_BusConfigMux
**
** ======================================================================== */

void HAL_clk_BusConfigMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  uint32                 nCmdCGRAddr, nCmdCGRVal;
  uint32                 nCfgCGRAddr, nCfgCGRVal;
  uint32                 nMAddr, nNAddr, nDAddr;
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
  if(pmConfig->eSource == HAL_CLK_SOURCE_XO)
  {
    nCfgCGRVal &= ~(HAL_CLK_CFG_CGR_SRC_SEL_FMSK |
                    HAL_CLK_CFG_CGR_MODE_FMSK);
  }
  else
  {
    nCfgCGRVal &= ~(HAL_CLK_CFG_CGR_SRC_SEL_FMSK |
                  HAL_CLK_CFG_CGR_SRC_DIV_FMSK |
                  HAL_CLK_CFG_CGR_MODE_FMSK);
  }

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
  if(pmConfig->eSource != HAL_CLK_SOURCE_XO)
  {
    nCfgCGRVal |= ((HALF_DIVIDER(pmConfig) << HAL_CLK_CFG_CGR_SRC_DIV_SHFT) 
                  & HAL_CLK_CFG_CGR_SRC_DIV_FMSK);
  }

  /*
   * Set MND counter mode depending on if it is in use
   */
  if (pmConfig->nM != 0 && (pmConfig->nM < pmConfig->nN))
  {    
    /*
     * Get M, N and D addresses
     */
    nMAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_M_REG_OFFSET;
    nNAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_N_REG_OFFSET;
    nDAddr = pmClockDomainDesc->nCGRAddr + HAL_CLK_D_REG_OFFSET;

    /*
     * Set M value
     */
    outpdw(nMAddr, pmConfig->nM);

    /*
     * Set N value
     */
    outpdw(nNAddr, NOT_N_MINUS_M(pmConfig));

    /*
     * Set D value
     */
    outpdw(nDAddr, NOT_2D(pmConfig));

    /*
     * Dual-edge mode
     */
    nCfgCGRVal |= ((HAL_CLK_CFG_CFG_DUAL_EDGE_MODE_VAL << HAL_CLK_CFG_CGR_MODE_SHFT) 
                    & HAL_CLK_CFG_CGR_MODE_FMSK);
  }

  /*
   * Write the final CFG register value
   */
  outpdw(nCfgCGRAddr, nCfgCGRVal);

  /*
   * Trigger the update
   */
  nCmdCGRVal |= HAL_CLK_CMD_CFG_UPDATE_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait until update finishes
   */
  while(inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CFG_UPDATE_FMSK);

} /* HAL_clk_BusConfigMux */