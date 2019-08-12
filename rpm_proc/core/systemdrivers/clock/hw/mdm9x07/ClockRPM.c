/** @file ClockRPM.c

  This file implements specialized image specific functions such
  as init image for RPM, we have few place holders for now.

  Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/hw/mdm9x07/ClockRPM.c#7 $
  $DateTime: 2015/12/03 06:44:36 $
  $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/05/12   vtw     Created from 8974.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <busywait.h>

#include "DALDeviceId.h"
#include "ClockDriver.h"
#include "HALclk.h"
#include "ClockRPMNPA.h"
#include "rpmserver.h"
#include "QDSSLite.h"
#include "swevent.h"
#include "ddr_drivers.h"
#include "HALclk.h"
#include "Clock.h"
#include "ClockCommon.h"
#include "ClockRPMBIMC.h"


/*=========================================================================
      Type Definitions
==========================================================================*/
#define XO_LEVEL                0
#define MAX_LEVEL               0x7FFFFFFF
#define QDSS_CLOCK_NUM          14


/*
 * ClockVddMinType
 * Structure containing the list of clocks to RAMs that need to turn off during vdd min
 */
typedef struct
{
  ClockIdType nSPMISerClk;
  ClockIdType nIMEMAxiClk;
  ClockIdType nMsgRAMClk;
} ClockVddMinType;

/*=========================================================================
      Module Data
==========================================================================*/

static rpm_npa_adapter clk0_adapter; // Misc clocks: [CXO, QDSS, dcvs.ena]
static rpm_npa_adapter clk1_adapter; // Bus clocks: [pcnoc, snoc]
static rpm_npa_adapter clk2_adapter; // Memory clocks: [bimc]
static ClockVddMinType clkVddMin;    // Clocks turn off during xo shutdown
static rpm_npa_adapter qpic_adapter; // QPIC

typedef struct
{
  char   *szClockName;
  ClockIdType  *pClockId;
} clock_id_type;


uint32 part_num_8208 = 0x962;
uint32 hal_part_num = 0;


/* =========================================================================
**  Function : Clock_BusSetMinMax
**      Set default Min/Max frequency for bus profiling
** =========================================================================*/
void Clock_BusSetMinMax( Clock_NPAResourcesType  *pNPAResources )
{
  /* Default setting for CPU clock */
  pNPAResources->CPUClockResource.nMinLevel = 0;
  pNPAResources->CPUClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for PCNOC clock */
  pNPAResources->PCNOCClockResource.nMinLevel = 0;
  pNPAResources->PCNOCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for BIMC clock */
  pNPAResources->BIMCClockResource.nMinLevel = 0;
  pNPAResources->BIMCClockResource.nMaxLevel = MAX_LEVEL;

 
  /* Default setting for QPIC clock */
  pNPAResources->QPICClockResource.nMinLevel = 0;
  pNPAResources->QPICClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for APSS_TCU_ASYNC clock */
  pNPAResources->APSSTCUASYNCClockResource.nMinLevel = 0;
  pNPAResources->APSSTCUASYNCClockResource.nMaxLevel = MAX_LEVEL;
  
  /* Default setting for MSS_TBU_ASYNC clock */
  pNPAResources->MSSTBUASYNCClockResource.nMinLevel = 0;
  pNPAResources->MSSTBUASYNCClockResource.nMaxLevel = MAX_LEVEL;

}



/* =========================================================================
**  Function : Clock_QDSSConfig
** =========================================================================*/
static DALResult Clock_QDSSConfig(
  ClockQDSSResourceType *pQDSSClockResource
)
{
  uint32 i;
  /*
   * QDSS clocks support by this driver. 
   */
  clock_id_type sQDSSClock[QDSS_CLOCK_NUM] =
  {
    {"gcc_qdss_cfg_ahb_clk", (ClockIdType *)&pQDSSClockResource->pQDSSCfgAHBClk},
    {"gcc_qdss_dap_ahb_clk", (ClockIdType *)&pQDSSClockResource->pQDSSDapAHBClk},
    {"gcc_qdss_etr_usb_clk", (ClockIdType *)&pQDSSClockResource->pQDSSEtrUSBClk},
    /* gcc_qdss_rbcpr_xpu_ahb_clk is removed in Bear */
    {"gcc_qdss_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSATClk},
//    {"gcc_sys_noc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSysNocATClk},
    {"gcc_pcnoc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSPCNocATClk},

    {"gcc_qdss_traceclkin_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTraceClk},
    {"gcc_qdss_stm_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSTMClk},
//    {"gcc_sys_noc_qdss_stm_axi_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSTMAXIClk},
    {"gcc_pcnoc_qdss_stm_axi_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSTMAXIClk},
    {"gcc_qdss_dap_clk", (ClockIdType *)&pQDSSClockResource->pQDSSDapClk},

    {"gcc_qdss_tsctr_div2_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv2Clk},
    {"gcc_qdss_tsctr_div3_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv3Clk},
    {"gcc_qdss_tsctr_div4_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv4Clk},
    {"gcc_qdss_tsctr_div8_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv8Clk},
    {"gcc_qdss_tsctr_div16_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv16Clk}
  };

  clock_id_type sQDSSATClock[2] =
  {
    {"gcc_qdss_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSATClk},
 //   {"gcc_sys_noc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSysNocATClk},
    {"gcc_pcnoc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSPCNocATClk},
  };

  pQDSSClockResource->nQDSSCurrLevel = CLOCK_QDSS_LEVEL_OFF;

  for (i=0; i < QDSS_CLOCK_NUM; i++)
  {
    if ( DAL_SUCCESS != Clock_GetClockId(sQDSSClock[i].szClockName, 
          sQDSSClock[i].pClockId) )
    {
      return DAL_ERROR;
    }
  }

  for (i=0; i < (sizeof(sQDSSATClock)/sizeof(clock_id_type)) ; i++)
  {
    Clock_EnableClock(*sQDSSATClock[i].pClockId);
  }

  /* QDSS clocks voltage are programming manually since they depending on CX voltage */
  CLOCK_FLAG_SET( pQDSSClockResource->pQDSSTraceClk->pDomain, 
                  DOMAIN_HOLD_VREG_REQUEST);	
  CLOCK_FLAG_SET( pQDSSClockResource->pQDSSATClk->pDomain, 
                  DOMAIN_HOLD_VREG_REQUEST);	
  CLOCK_FLAG_SET( pQDSSClockResource->pQDSSTSCTRDiv2Clk->pDomain, 
                  DOMAIN_HOLD_VREG_REQUEST);
  CLOCK_FLAG_SET( pQDSSClockResource->pQDSSSTMClk->pDomain, 
                  DOMAIN_HOLD_VREG_REQUEST);


  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : Clock_IsQDSSOn
** =========================================================================*/
/**
  This function determines if QDSS clocks are ON
  
  @param 
      None
  
  @return
      TRUE if QDSS clocks are ON, FALSE otherwise
*/
boolean Clock_IsQDSSOn(void)
{
   Clock_NPAResourcesType  *pNPAResources;
   pNPAResources = Clock_GetNPAResources();
   return (pNPAResources->QDSSClockResource.nQDSSCurrLevel != CLOCK_QDSS_LEVEL_OFF);
}

/* =========================================================================
**  Function : Clock_DetectClockConfig
** =========================================================================*/
/**
  Detects current configuration of clock

  This function is invoked at driver initialization to detect the current
  clock configuration.

  @param
    pClockResource [in] -- Clock node for detection

  @return
    DAL_ERROR if configuration was not valid, other DAL_SUCCESS.

  @dependencies
    None.
*/
static DALResult Clock_DetectClockConfig
(
  ClockResourceType *pClockResource
)
{
  uint32                     nCfg;
  uint32                     nsupCfg;
  uint32                     nDiv2x;
  ClockMuxConfigType         *pCfg;
  HAL_clk_ClockMuxConfigType MuxCfg; 
  ClockNodeType              *pClock;

  pClock = pClockResource->pClock;

  /*-----------------------------------------------------------------------*/
  /* Get the current clock configuration.                                  */
  /*-----------------------------------------------------------------------*/

  HAL_clk_DetectClockMuxConfig(pClock->pDomain->HALHandle, &MuxCfg);
  
  /*
   * If we got back nDiv2x less than 2 (Bypass or Div1), set to 2 since 
   *   MuxConfig will substract 1 when it used HALF_DIVIDER(mux)
   */
  if (MuxCfg.nDiv2x < 2)
  {
    MuxCfg.nDiv2x = 2;
  }
  
  /* Get Min & Max support frequency in BSP */
  pCfg = pClock->pDomain->pBSPConfig;

  /* Loop thru the end of BSP data for finding Max Level */
  nCfg = 0;
  nsupCfg = 0;
  pClockResource->nCurLevel = MAX_LEVEL;
  while ( pCfg[nCfg].nFreqHz != 0 )
  {
    /* Check only support BSP */
    if ( Clock_IsBSPSupported(&pCfg[nCfg]) )
  {
    nsupCfg = nCfg;
    nDiv2x = pCfg[nCfg].HALConfig.nDiv2x;
    if (nDiv2x < 2)
    {    
      nDiv2x = 2;
    }
  
      /* Get current configure that match with BSP data */
      if ( (pClockResource->nCurLevel == MAX_LEVEL) &&
           (MuxCfg.eSource == pCfg[nCfg].HALConfig.eSource) &&
           (MuxCfg.nDiv2x == nDiv2x) )
      {
        pClockResource->nCurLevel = nCfg;
      }
    }
    nCfg++;
  }

  // Error fatal for NULL terminal BSP
  assert( nCfg > 0 );

  // Reset default min level if it is greater than max support BSP
  if ( pClockResource->nMinLevel > nsupCfg )
  {
    pClockResource->nMinLevel = nsupCfg;   
  }
  // Convert support index to BSP index
  // Reset default max level if it is greater than max support BSP
  if ( pClockResource->nMaxLevel > nsupCfg )
  {
    pClockResource->nMaxLevel = nsupCfg;
  }

  /* Assert if current BSP data does not match with Boot config level */
  assert( pClockResource->nCurLevel != MAX_LEVEL );

  /* Assert if min is greater than max */
  assert( pClockResource->nMinLevel <= pClockResource->nMaxLevel );

  /* Set active mux config and boot up clock frequency */
  pClock->pDomain->pActiveMuxConfig = &pClock->pDomain->pBSPConfig[pClockResource->nCurLevel];

  return DAL_SUCCESS;

}


/* =========================================================================
**  Function : Clock_DetectRPMConfig
** =========================================================================*/
/**
  Detects current configuration of RPM clock

  This function is invoked at driver initialization to detect the current
  RPM configuration.

  @param
    pNPAResources [in] - NPA resources for clock node

  @return
    DAL_ERROR if configuration was not valid, other DAL_SUCCESS.

  @dependencies
    None.
*/
static DALResult Clock_DetectRPMConfig
(
  Clock_NPAResourcesType* pNPAResources
)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Get the current RPM configuration.                                    */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->CPUClockResource);
  if ( eResult != DAL_SUCCESS )
    return eResult;

  /* Clock already enable in boot.  Enable clock for reference count */
  Clock_EnableClock( (ClockIdType)pNPAResources->CPUClockResource.pClock );


  /*-----------------------------------------------------------------------*/
  /* Get the current Peripheral Config NOC configuration.                  */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->PCNOCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->PCNOCClockResource.pClock );

  /*-----------------------------------------------------------------------*/
  /* Get the current BIMC configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->BIMCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->BIMCClockResource.pClock );
  
  
  /*-----------------------------------------------------------------------*/
  /* Get the current QPIC configuration.                                   */
  /*-----------------------------------------------------------------------*/

  if ( pNPAResources->bQPICEnable )
  {
  eResult = Clock_DetectClockConfig( &pNPAResources->QPICClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the current APSS_TCU_ASYNC clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->APSSTCUASYNCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;
 
  Clock_EnableClock( (ClockIdType)pNPAResources->APSSTCUASYNCClockResource.pClock );
  
  
  /*-----------------------------------------------------------------------*/
  /* Get the current MSS_TBU_ASYNC clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->MSSTBUASYNCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->MSSTBUASYNCClockResource.pClock );

 
  /*-----------------------------------------------------------------------*/
  /* Setup QDSS resource                                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_QDSSConfig( &pNPAResources->QDSSClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  return DAL_SUCCESS;

} /* END Clock_DetectRPMConfig */

static void Clock_InitQPIC( void )
{
  Clock_NPAResourcesType  *pNPAResources = Clock_GetNPAResources();

  assert( Clock_GetClockId("gcc_qpic_clk",(ClockIdType *)&pNPAResources->QPICClockResource.pClock) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qpic_system_clk",(ClockIdType *)&pNPAResources->pQPICSysClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qpic_ahb_clk",(ClockIdType *)&pNPAResources->pQPICAHBClk) == DAL_SUCCESS);

  /* Mark QPIC clock as non-suppressible clock */
  CLOCK_FLAG_CLEAR( pNPAResources->QPICClockResource.pClock->pDomain, DOMAIN_CPU);

  // Check if these clock ON by boot loader
  Clock_IsClockOn((ClockIdType)pNPAResources->QPICClockResource.pClock, &pNPAResources->bQPICEnable );

}

/* =========================================================================
**  Function : Clock_ConfigBIMCClocks
** =========================================================================*/
/**
  This API is used to config BIMC clocks ID and BIMC power domain
   
  @param 
    None
  
  @return
    DAL_ERROR if configuration was not valid, other DAL_SUCCESS.
  
  @dependencies
    None.
*/ 
  
DALResult Clock_ConfigBIMCClocks( void )
{
  DALResult               eRes;
  BIMCResourceType        *pBIMCRsc;
  Clock_NPAResourcesType  *pNPAResources;

  pNPAResources = Clock_GetNPAResources();
  pBIMCRsc = &pNPAResources->BIMCResource;
 
  pBIMCRsc->eBIMCState = BIMC_ON;
  pBIMCRsc->bPowerCollapsed = TRUE;

  eRes = Clock_GetClockId( "gcc_bimc_clk", 
                           (ClockIdType *)&pNPAResources->BIMCClockResource.pClock );
  if ( eRes != DAL_SUCCESS )
  {
    return DAL_ERROR;
  }

  eRes = Clock_GetClockId( "gcc_dehr_clk", &pBIMCRsc->nClockDehr );
  if ( eRes != DAL_SUCCESS )
  {
    return DAL_ERROR;
  }

  /* Call Enable BIMC power domain for increment reference count, so it can collapse later */
  eRes = Clock_GetPowerDomainId( "VDD_BIMC", &pBIMCRsc->nBIMCPowerID );
  if ( eRes != DAL_SUCCESS )
  {
    return DAL_ERROR;
  }
  Clock_EnablePowerDomain( pBIMCRsc->nBIMCPowerID, TRUE ); 

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : Clock_InitClkVddMin
** =========================================================================*/
/*
  This API is used for initialize all the clocks that need to manage to turn off
  during vdd min.
   
  @param 
    None
  
  @return
    None
  
  @dependencies
    None.
 */ 

static void Clock_InitClkVddMin( void )
{
  /* gcc_spmi_ser_clk */
  assert( Clock_GetClockId("gcc_spmi_ser_clk", &clkVddMin.nSPMISerClk) == DAL_SUCCESS );
  Clock_EnableClock( clkVddMin.nSPMISerClk );

  /* gcc_imem_axi_clk */ 
  assert ( Clock_GetClockId("gcc_imem_axi_clk", &clkVddMin.nIMEMAxiClk) == DAL_SUCCESS );
  Clock_EnableClock( clkVddMin.nIMEMAxiClk );

  /* gcc_msg_ram_ahb_clk */
  assert( Clock_GetClockId("gcc_msg_ram_ahb_clk", &clkVddMin.nMsgRAMClk) == DAL_SUCCESS );
  Clock_EnableClock( clkVddMin.nMsgRAMClk );
}


/* =========================================================================
**  Function : Clock_EnableClkVddMin
** =========================================================================*/
/*
  This API is used for enable/disable all the clocks that need to manage
  during vdd min.
   
  @param 
    bEnable - TRUE  - Enable all the clocks
              FALSE - Disalbe all the clocks
  
  @return
    None
  
  @dependencies
    None.
 */ 

static void Clock_EnableClkVddMin( boolean bEnable )
{
  if ( bEnable )
  {
    /* gcc_spmi_ser_clk */
    Clock_EnableClock( clkVddMin.nSPMISerClk );

    /* gcc_imem_axi_clk */ 
    Clock_EnableClock( clkVddMin.nIMEMAxiClk );

    /* gcc_msg_ram_ahb_clk */
    Clock_EnableClock( clkVddMin.nMsgRAMClk );

  }
  else
  {
    /* gcc_spmi_ser_clk */
    Clock_DisableClock( clkVddMin.nSPMISerClk );

    /* gcc_imem_axi_clk */ 
    Clock_DisableClock( clkVddMin.nIMEMAxiClk );

    /* gcc_msg_ram_ahb_clk */
    Clock_DisableClock( clkVddMin.nMsgRAMClk );
   
  }
}


/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/*
  See DalClock.h
*/

DALResult Clock_InitImage
(
  void
)
{

  ClockDrvCtxt            *pDrvCtxt;
  DALResult               eRes;
  Clock_NPAResourcesType  *pNPAResources;

  pNPAResources = Clock_GetNPAResources();
  pDrvCtxt = Clock_GetDrvCtxt();

  /* Default DCVS is disable. */
  pDrvCtxt->bDCVSEnabled = FALSE;

  SWEVENT(CLOCK_EVENT_INIT);


  /* Set bus Min/Max for profiling */
  Clock_BusSetMinMax( pNPAResources );

  /* Suspend voltage scaling during init */
  Clock_VoltageSuspend( pDrvCtxt, TRUE);

  /*-----------------------------------------------------------------------*/
  /* Get RPM clock ID and enable for reference count                       */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_GetClockId( "gcc_rpm_proc_fclk",
                           (ClockIdType *)&pNPAResources->CPUClockResource.pClock );

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get RPM clock ID.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Marking RPM CPU clock as non-voltage change                           */
  /*-----------------------------------------------------------------------*/
  CLOCK_FLAG_SET(pNPAResources->CPUClockResource.pClock->pDomain,
                 DOMAIN_HOLD_VREG_REQUEST);


  /*-----------------------------------------------------------------------*/
  /* Get Peripheral Config NOC clock ID                                    */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_GetClockId( "gcc_pcnoc_ahb_clk",
                           (ClockIdType *)&pNPAResources->PCNOCClockResource.pClock );

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get Peripheral NOC clock ID.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Get BIMC clock ID                                                     */
  /*-----------------------------------------------------------------------*/
  eRes = Clock_ConfigBIMCClocks();

  if (eRes != DAL_SUCCESS) 
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get BIMC clock ID.");
    return eRes;
  }
  
 
  /*-----------------------------------------------------------------------*/
  /* Get QPIC clock ID                                                     */
  /*-----------------------------------------------------------------------*/
  Clock_InitQPIC();

  /*-----------------------------------------------------------------------*/
  /* Get APSS_TCU_AYNC clock ID                                                     */
  /*-----------------------------------------------------------------------*/
  eRes = Clock_GetClockId( "gcc_apss_tcu_async_clk", 
                           (ClockIdType *)&pNPAResources->APSSTCUASYNCClockResource.pClock );

  if (eRes != DAL_SUCCESS) 
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get APSS_TCU_ASYNC clock ID.");
    return eRes;
  }
  /*-----------------------------------------------------------------------*/
  /* Get MSS_TBU_AYNC clock ID                                                 */
  /*-----------------------------------------------------------------------*/
  eRes = Clock_GetClockId( "gcc_mss_tbu_axi_clk", 
                           (ClockIdType *)&pNPAResources->MSSTBUASYNCClockResource.pClock );

  if (eRes != DAL_SUCCESS) 
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get MSS_TBU_ASYNC clock ID.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Detect current clocks setting and enable their clocks and sources     */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_DetectRPMConfig( pNPAResources );

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to current detect RPM clock.");
    return eRes;
  }
  
  /* Initialize a list of clocks that needs to manage during xo shutdown */
  Clock_InitClkVddMin();
  
 /* Moved this after Clock_DetectRPMConfig to let PCNOC initialize first so
    that the clock domain shared by PCNOC and QPIC AHB clock gets initialized
    properly */
  if ( pNPAResources->bQPICEnable )
  {
    Clock_EnableClock( (ClockIdType)pNPAResources->QPICClockResource.pClock );
    Clock_EnableClock( (ClockIdType)pNPAResources->pQPICSysClk );
    Clock_EnableClock( (ClockIdType)pNPAResources->pQPICAHBClk );
  }
  
  Clock_EnableSource(pDrvCtxt, &pDrvCtxt->aSources[pDrvCtxt->anSourceIndex[HAL_CLK_SOURCE_GPLL1]], FALSE);

  Clock_DalDebugInit (pNPAResources);
  /*-----------------------------------------------------------------------*/
  /* Initialize NPA clock data                                             */
  /*-----------------------------------------------------------------------*/
  Clock_InitNPA();

  /*-----------------------------------------------------------------------*/
  /* Expose the nodes to the outside world via rpmserver                   */
  /*-----------------------------------------------------------------------*/
  clk0_adapter = rpm_create_npa_adapter(RPM_CLOCK_0_REQ, 5); // Misc clocks: [CXO, QDSS, dcvs.ena, apcs.corner, gpll0]
  rpm_associate_npa_resource(clk0_adapter, 0, pNPAResources->CXONPANode.Resource.name);
  rpm_associate_npa_resource(clk0_adapter, 1, pNPAResources->QDSSClockResource.NPANode.Resource.name);
  rpm_associate_npa_resource(clk0_adapter, 2, pNPAResources->DCVSEnaNPANode.Resource.name);
  rpm_associate_npa_resource(clk0_adapter, 4, pNPAResources->GPLL0ClockResource.NPANode.Resource.name);

  clk1_adapter = rpm_create_npa_adapter(RPM_CLOCK_1_REQ, 1); // Bus clocks: [pcnoc]
  rpm_associate_npa_resource(clk1_adapter, 0, pNPAResources->PCNOCClockResource.NPANode.Resource.name);

  clk2_adapter = rpm_create_npa_settling_adapter(RPM_CLOCK_2_REQ, 1); // Memory clocks: [bimc ]
  rpm_associate_npa_resource(clk2_adapter, 0, pNPAResources->BIMCClockResource.NPANode.Resource.name);

  qpic_adapter = rpm_create_npa_adapter(RPM_CLOCK_QPIC_REQ, 1); // qpic clock
  rpm_associate_npa_resource(qpic_adapter, 0, pNPAResources->QPICClockResource.NPANode.Resource.name);

  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/

  /* Init is done, resume voltage scaling with update PMIC */
  Clock_VoltageResume( pDrvCtxt, TRUE );

  return DAL_SUCCESS;

} /* END Clock_InitImage */


/* =========================================================================
**  Function : Clock_ProcessorSleep
** =========================================================================*/
/*
  See Clock.h
*/

DALResult Clock_ProcessorSleep
(
  ClockSleepModeType  eMode,
  uint32              nFlags
)
{
  Clock_NPAResourcesType  *pNPAResources;
  ClockDrvCtxt            *pDrvCtxt;  
  boolean                  bBIMCPC;
  
  pNPAResources = Clock_GetNPAResources();
  pDrvCtxt = Clock_GetDrvCtxt();

  /* Suspend change voltage during power collapse */
  Clock_VoltageSuspend( pDrvCtxt, TRUE);

  SWEVENT( CLOCK_EVENT_POWER_COLLAPSE, CLOCK_SWE_PROCESSOR_ENTER );

  /* Put BIMC in power collapsed */
  if ( nFlags != 0 )
  {
    /* Switch QDSS clocks to XO speed if they are ON */
    if ( pNPAResources->QDSSClockResource.nQDSSCurrLevel != CLOCK_QDSS_LEVEL_OFF )
    {
      pNPAResources->QDSSClockResource.nQDSSSaveSpeed = pNPAResources->QDSSClockResource.nQDSSCurrSpeed;
      Clock_SetQDSSClocks(&pNPAResources->QDSSClockResource, CLOCK_QDSS_SPEED_XO);
    }

    bBIMCPC = (nFlags == 2);
    Clock_SetBIMCPowerCollapse(bBIMCPC);
  }
  else
  {
    /* Wait for BIMC collapsed done before collapse all non-BIMC clocks to XO speed */
    Clock_WaitForBIMCPowerCollapse();

    /*Turn off misc clocks that are not required*/
//    HAL_clk_MiscClkDisableDeepSleep();   

  
    if ( pNPAResources->APSSTCUASYNCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->APSSTCUASYNCClockResource.nSavLevel = pNPAResources->APSSTCUASYNCClockResource.nCurLevel;
      Clock_SetClockConfigEx( pNPAResources->APSSTCUASYNCClockResource.pClock, 
                            &pNPAResources->APSSTCUASYNCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->MSSTBUASYNCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->MSSTBUASYNCClockResource.nSavLevel = pNPAResources->MSSTBUASYNCClockResource.nCurLevel;
      Clock_SetClockConfigEx( pNPAResources->MSSTBUASYNCClockResource.pClock, 
                            &pNPAResources->MSSTBUASYNCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }
	
    /* Turn clocks to RAMs off */
    /* Check if this is required??*/
    Clock_EnableClkVddMin( FALSE );
	
    /* Collapse all non-BIMC clocks to XO speed
    * PCNOC Should be done after BIMC as RPM is running OFF PCNOC
    */
    if ( pNPAResources->PCNOCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->PCNOCClockResource.nSavLevel = pNPAResources->PCNOCClockResource.nCurLevel;
      Clock_SetClockConfigEx( pNPAResources->PCNOCClockResource.pClock, 
                            &pNPAResources->PCNOCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    // Switch CPU clock to last for highest performance
    if ( pNPAResources->CPUClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->CPUClockResource.nSavLevel = pNPAResources->CPUClockResource.nCurLevel;
      Clock_SetClockConfigEx( pNPAResources->CPUClockResource.pClock, 
                            &pNPAResources->CPUClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    /*
    * On 8x26/8x10 as A7 used to keep its vote ON for GPLL0 in APPS power collapse 
    * and hence this was required to reset FSM for GPLL0 during deep sleep. 
    * We need to find out if A53 has the same behaviour??
    */
    HAL_clk_GPLL0_disable_deep_sleep(); 

  /*
   * HW CR:QCTDD02264618
   * On mdm9x07 because of SR PLL lock issue, GPLL1 is always kept on.
   * It is only disabled before going into XO shutdown, and enabled back after coming out of 
   * XO shutdown. There is a work around to avoid this issue, it requires changing 
   * config ctl register before and after PLL lock, but this register cannot be modified
   * while PLL is running. Since GPLL1 is shared, hence this sequence cannot be implemented as multiple
   * subsystem will be trying to enable/disable the PLL at the same time. To solve this issue, we avoid the PLL lock at run time
   * by keeping the PLL always ON. 
   * This new workaround sequence is however implemented in the SBL where the PLL is enabled for first time,
   * and after coming out of XO shutdown.
   */
    
    HAL_clk_GPLL1_disable_deep_sleep(); 
  }

  SWEVENT( CLOCK_EVENT_POWER_COLLAPSE, CLOCK_SWE_PROCESSOR_DONE );

  /* Sleep done, disable voltage suspend but don't update PMIC */
  Clock_VoltageResume( pDrvCtxt, FALSE );

  return DAL_SUCCESS;

} /* END Clock_ProcessorSleep */


/* =========================================================================
**  Function : Clock_ProcessorRestore
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_ProcessorRestore
(
  ClockSleepModeType  eMode,
  uint32              nFlags
)
{
  Clock_NPAResourcesType  *pNPAResources;
  ClockDrvCtxt        *pDrvCtxt = Clock_GetDrvCtxt();
  if (!pDrvCtxt)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Device Contex pointer NULL.");
    return DAL_ERROR;
  }

  /* Don't allow voltage change during processor restore */
  Clock_VoltageSuspend( pDrvCtxt, TRUE);

  SWEVENT( CLOCK_EVENT_POWER_RESTORE, CLOCK_SWE_PROCESSOR_ENTER );
  
  pNPAResources = Clock_GetNPAResources();

  /* BIMC restored. */
  if ( nFlags != 0 )
  {
    Clock_WaitForBIMCPowerRestore();
    
    /* Restore QDSS clocks */
    if ( pNPAResources->QDSSClockResource.nQDSSCurrLevel != CLOCK_QDSS_LEVEL_OFF )
    {
      Clock_SetQDSSClocks(&pNPAResources->QDSSClockResource, pNPAResources->QDSSClockResource.nQDSSSaveSpeed);
    }

  }
  else
  {

    /*
    * On 8x26/8x10 as A7 used to keep its vote ON for GPLL0 in APPS power collapse 
    * and hence this was required to reset FSM for GPLL0 during deep sleep. 
    * We need to find out if A53 has the same behaviour??
    */    
    HAL_clk_GPLL0_FSM_reset_deep_sleep();

  /*
   * HW CR:QCTDD02264618
   * On mdm9x07 because of SR PLL lock issue, GPLL1 is always kept on.
   * It is only disabled before going into XO shutdown, and enabled back after coming out of 
   * XO shutdown. There is a work around to avoid this issue, it requires changing 
   * config ctl register before and after PLL lock, but this register cannot be modified
   * while PLL is running. Since GPLL1 is shared, hence this sequence cannot be implemented as multiple
   * subsystem will be trying to enable/disable the PLL at the same time. To solve this issue, we avoid the PLL lock at run time
   * by keeping the PLL always ON. 
   * This new workaround sequence is however implemented in the SBL where the PLL is enabled for first time,
   * and after coming out of XO shutdown.
   */
    HAL_clk_GPLL1_FSM_reset_deep_sleep();
    
    /* Turn clocks to RAMs On */
    /* Check if this is required? */
    Clock_EnableClkVddMin( TRUE );
  
    if ( pNPAResources->CPUClockResource.nCurLevel != XO_LEVEL )
    {
       // Need to restore CPU clock up first for higher performance
       Clock_SetClockConfigEx( pNPAResources->CPUClockResource.pClock, 
                             &pNPAResources->CPUClockResource.pClock->pDomain->pBSPConfig[pNPAResources->CPUClockResource.nSavLevel] );
    }
    if ( pNPAResources->PCNOCClockResource.nCurLevel != XO_LEVEL )
    {
       /* Need to restore PCNOC before BIMC as RPM runs OFF PCNOC */
       Clock_SetClockConfigEx( pNPAResources->PCNOCClockResource.pClock, 
                             &pNPAResources->PCNOCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->PCNOCClockResource.nSavLevel] );
    } 

   if ( pNPAResources->APSSTCUASYNCClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfigEx( pNPAResources->APSSTCUASYNCClockResource.pClock, 
                            &pNPAResources->APSSTCUASYNCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->APSSTCUASYNCClockResource.nSavLevel] );
    }
	
   if ( pNPAResources->MSSTBUASYNCClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfigEx( pNPAResources->MSSTBUASYNCClockResource.pClock, 
                            &pNPAResources->MSSTBUASYNCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->MSSTBUASYNCClockResource.nSavLevel] );
    }

   /*Turn misc clocks on*/ 
//    HAL_clk_MiscClkRestoreDuringWakeup();

    /* Restore BIMC */
    Clock_SetBIMCPowerRestore();
  }

  /* 
   * Allow voltage change.
   */
  Clock_VoltageResume( pDrvCtxt, FALSE );

  SWEVENT( CLOCK_EVENT_POWER_RESTORE, CLOCK_SWE_PROCESSOR_DONE );

  return DAL_SUCCESS;

} /* END Clock_ProcessorRestore */



/*Defining  property handle.*/
DALSYS_PROPERTY_HANDLE_DECLARE(hPropDbg);

#define DEFAULT_DAL_FREQUENCY 0xFFFFFFFF

DALResult Clock_DalCfgRead(const char *pDbgDalStrProp, uint32 **pData, uint32 nElems)
{
  DALSYSPropertyVar      PropVal;
  DALResult              eResult;
  
  eResult = DALSYS_GetPropertyValue(hPropDbg, pDbgDalStrProp, 0, &PropVal); 
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_GetPropertyValue failed.");
    return eResult;
  }
  /*If number of elements does not match return error*/
  if(nElems == PropVal.dwLen)
  {
    *pData = PropVal.Val.pdwVal;
  }
  else 
    return DAL_ERROR;

  
  return eResult;
 
}

static void  Clock_SetClockDebugMinMax
(
  ClockResourceType *pClockResource,
  const char *pDbgDalStrProp,
  uint32 nDebugVolLvl
)
{
  uint32                     nCfg;
  ClockMuxConfigType         *pCfg;
  ClockNodeType              *pClock;
  uint32                     *nDebugVal;
  uint32                     nSup = 0;
  uint32                     nFreqMin;
  uint32                     nFreqMax;
  uint32                     nDbgMinlvl = 1;
  uint32                     nDbgMaxlvl = 0;
  DALResult                  eResult;
  pClock = pClockResource->pClock;
  
  if(nDebugVolLvl < CLOCK_VREG_NUM_LEVELS)
  {
    /* if the XML file has valid voltage corner, in that case the clocks will be set to 
           that voltage corner and the fmin and fmax entries in the xml is ignored */
    nFreqMin = DEFAULT_DAL_FREQUENCY;
    nFreqMax = DEFAULT_DAL_FREQUENCY;
  }
  else if (pDbgDalStrProp)
  {
   /* If XML does not have valid voltage corner, in that case  fmin and fmax 
        entries in the xml is applied*/
    eResult = Clock_DalCfgRead(pDbgDalStrProp, &nDebugVal, 2) ;
    if(eResult == DAL_SUCCESS)
    {
      nFreqMin = nDebugVal[0];
      nFreqMax = nDebugVal[1];
      if(nFreqMin == DEFAULT_DAL_FREQUENCY || nFreqMax == DEFAULT_DAL_FREQUENCY)
        return;
    }
    else 
      return;
  }
  else 
  {
    return;
  }

  /* Get Min & Max support frequency in BSP */
  pCfg = pClock->pDomain->pBSPConfig;

  /* Loop thru the end of BSP data for finding Max Level */
  nCfg = 0;
  while ( pCfg[nCfg].nFreqHz != 0 )
  {
    /* Check only support BSP */
    if ( Clock_IsBSPSupported(&pCfg[nCfg]))
    {
      nSup = nCfg;
      /*This will assign  the fmax for a voltage corner and fmax supported just less than the one specified in the XML*/
      if(pCfg[nCfg].nFreqHz <= nFreqMax && pCfg[nCfg].eVRegLevel <= nDebugVolLvl)
      {
        nDbgMaxlvl = nCfg;
      }
      /*This will assign  fmin for a voltage corner and fmin spported just less than the one specified in the XML*/
      if(pCfg[nCfg].nFreqHz <= nFreqMin && pCfg[nCfg].eVRegLevel <= nDebugVolLvl)
      {
        nDbgMinlvl = nCfg;
      }
    }
    nCfg++;
  }

   // Error fatal for NULL terminal BSP
  assert( nCfg > 0 );

  /*Error checking of frequencies specified in the XML. it is done only when valid voltage corner is not selected*/
  if(!(nDebugVolLvl < CLOCK_VREG_NUM_LEVELS))
  {
    /*This is for error checking if the entries in the xml are with in the bounds of frequency plan, if not reject it */
    if((nFreqMax < pCfg[0].nFreqHz  && nFreqMin < pCfg[0].nFreqHz) ||  
       (nFreqMax > pCfg[nSup].nFreqHz  && nFreqMin > pCfg[nSup].nFreqHz))
    {
      return;
    }

    /*If Fmin-xml does not match with any of the perf level then we select the perf level just higher than the Fmin-xml value*/
    if(nFreqMin > pCfg[nDbgMinlvl].nFreqHz)
    {
      nDbgMinlvl++;
    }
  }

  /*Ignore if the min level is more that the max level and min level or min/ max level is not with in the clock plan bounds */
  if((nDbgMinlvl <= nDbgMaxlvl) &&
      (pClockResource->nMinLevel <= nDbgMinlvl) && 
      (nDbgMaxlvl <= pClockResource->nMaxLevel) )
  {
     pClockResource->nMinLevel = nDbgMinlvl;
     pClockResource->nMaxLevel = nDbgMaxlvl;
  }

  return;
}



/* =========================================================================
**  Function : Clock_DalDebugInit
** =========================================================================*/

boolean Clock_DalDebugInit (Clock_NPAResourcesType  *pNPAResources)
{
  uint32                      *nDebugVal;
  uint32                      nIndex = 0, nrailVolLvl ;
  uint32                      nClockVolLvl = CLOCK_VREG_NUM_LEVELS;
  ClockDrvCtxt                *pDrvCtxt = Clock_GetDrvCtxt();
  DALResult                   eResult;

  eResult = DALSYS_GetDALPropertyHandle(DALDEVICEID_CLOCK, hPropDbg);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }


  Clock_DebugCfgType aClock_DebugCfg[] =
  {
    {&pNPAResources->CPUClockResource, "lim_cpu"},
    {&pNPAResources->PCNOCClockResource, "lim_pcnoc"},
    {&pNPAResources->BIMCClockResource, "lim_bimc"},
    {&pNPAResources->QPICClockResource, "lim_qpic"},
  };
  
  
  eResult = Clock_DalCfgRead("clk_volt_corn", &nDebugVal, 1) ;
  if(eResult == DAL_SUCCESS)
  {
   nrailVolLvl = nDebugVal[0];
   nIndex =  1; /*ignoring CLOCK_VREG_LEVEL_OFF*/
   while(nIndex < CLOCK_VREG_NUM_LEVELS)
   {
     if(nrailVolLvl == pDrvCtxt->mVregRailmap[nIndex])
     {
       nClockVolLvl = nIndex;
       /*Do not break, as we need to pick the last entry corresponding to nrailVolLvl in the array.
         This is required since CLOCK_VREG_LEVEL_RETENTION, CLOCK_VREG_LEVEL_LOW_MINUS, CLOCK_VREG_LEVEL_LOW
         all points to RAIL_VOLTAGE_LEVEL_SVS, and only last entry for RAIL_VOLTAGE_LEVEL_SVS is valid.
         Look into the array ClockVregRailMapConfig in ClockBSP.c
       */
       //break;
     }
     nIndex++;
   }
  }

  nIndex =  0;

  while(nIndex < sizeof(aClock_DebugCfg)/sizeof(Clock_DebugCfgType))
  {
    Clock_SetClockDebugMinMax(aClock_DebugCfg[nIndex].ClockResDbg, 
                             aClock_DebugCfg[nIndex].pDbgDalStrProp, 
                             nClockVolLvl);
    nIndex++;
  }

  return DAL_SUCCESS;

}
/* =========================================================================
**  Function : Clock_Init
** =========================================================================*/
/*
  See ClockDriver.h
*/
boolean Clock_Init (void)
{
  return (Clock_DriverInit() == DAL_SUCCESS) ? TRUE : FALSE;
} /* END Clock_Init */


