/** @file ClockRPM.c

  This file implements specialized image specific functions such
  as init image for RPM, we have few place holders for now.

  Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/hw/mdm9x45/ClockRPM.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
  07/14/14   pbitra     Ported from 9x35, updated for 9x45.

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
#include "Clock.h"
#include "ClockRPMBIMC.h"
#include "CoreVerify.h"


/*=========================================================================
      Type Definitions
==========================================================================*/
#define XO_LEVEL                0
#define MAX_LEVEL               0xFFFFFFFF

/*=========================================================================
      Module Data
==========================================================================*/

static rpm_npa_adapter clk0_adapter; // Misc clocks: [CXO, QDSS]
static rpm_npa_adapter clk1_adapter; // Bus clocks: [pnoc, snoc, cnoc, mm/axinoc, mm/ahbnoc, mm/ocmemnoc]
static rpm_npa_adapter clk2_adapter; // Memory clocks: [bimc]
static rpm_npa_adapter qpic_adapter; // QPIC
static rpm_npa_adapter ipa_adapter;  // IPA
static rpm_npa_adapter ce_adapter;   // CE - Crypto Engine 


/* =========================================================================
**  Function : Clock_BusSetMinMax
**      Set default Min/Max frequency for bus profiling
** =========================================================================*/
void Clock_BusSetMinMax( Clock_NPAResourcesType  *pNPAResources )
{
  /* Default setting for CPU clock */
  pNPAResources->CPUClockResource.nMinLevel = 0;
  pNPAResources->CPUClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for SNOC clock */
  pNPAResources->SNOCClockResource.nMinLevel = 0;
  pNPAResources->SNOCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for PCNOC clock */
  pNPAResources->PCNOCClockResource.nMinLevel = 0;
  pNPAResources->PCNOCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for BIMC clock */
  pNPAResources->BIMCClockResource.nMinLevel = 0;
  pNPAResources->BIMCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for QPIC clock */
  pNPAResources->QPICClockResource.nMinLevel = 0;
  pNPAResources->QPICClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for IPA clock */
  pNPAResources->IPAClockResource.nMinLevel = 0;
  pNPAResources->IPAClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for CE clock */
  pNPAResources->CEClockResource.ClkCE.nMinLevel = 0;
  pNPAResources->CEClockResource.ClkCE.nMaxLevel = MAX_LEVEL;
}


/* =========================================================================
**  Function : Clock_QDSSConfig
** =========================================================================*/
static DALResult Clock_QDSSConfig(
  ClockQDSSResourceType *pQDSSClockResource
)
{
  pQDSSClockResource->bQDSSEnabled    = FALSE;
  pQDSSClockResource->nQDSSCurrLevel = CLOCK_QDSS_LEVEL_OFF;

  /* Get the clock Ids */
  assert( Clock_GetClockId("gcc_qdss_cfg_ahb_clk", (ClockIdType *)&pQDSSClockResource->pQDSSCfgAHBClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_dap_ahb_clk", (ClockIdType *)&pQDSSClockResource->pQDSSDapAHBClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_etr_usb_clk", (ClockIdType *)&pQDSSClockResource->pQDSSEtrUSBClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSATClk) == DAL_SUCCESS);

  Clock_EnableClock( (ClockIdType)pQDSSClockResource->pQDSSATClk);  

  /* Get SYS_NOC_AT_CLK */
  assert( Clock_GetClockId("gcc_sys_noc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSysNocATClk) == DAL_SUCCESS);
  Clock_EnableClock( (ClockIdType)pQDSSClockResource->pQDSSSysNocATClk); 

  /* Get PCNOC_AT_CLK */
  assert( Clock_GetClockId("gcc_pcnoc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSPCNocATClk) == DAL_SUCCESS);
  Clock_EnableClock( (ClockIdType)pQDSSClockResource->pQDSSPCNocATClk);  

  assert( Clock_GetClockId("gcc_qdss_traceclkin_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTraceClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_stm_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSTMClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_sys_noc_qdss_stm_axi_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSTMAXIClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_dap_clk", (ClockIdType *)&pQDSSClockResource->pQDSSDapClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_tsctr_div2_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv2Clk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_tsctr_div3_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv3Clk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_tsctr_div4_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv4Clk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_tsctr_div8_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv8Clk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_qdss_tsctr_div16_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv16Clk) == DAL_SUCCESS);

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
  boolean                 bClockOn;
  Clock_NPAResourcesType  *pNPAResources;

  pNPAResources = Clock_GetNPAResources();

  Clock_IsClockOn((ClockIdType)pNPAResources->QDSSClockResource.pQDSSCfgAHBClk, &bClockOn );

  return bClockOn;
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
  uint32                     nSupCfg;
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
  nSupCfg = 0;
  pClockResource->nCurLevel = MAX_LEVEL;
  while ( pCfg[nCfg].nFreqHz != 0 )
  {
    /* Check only support BSP */
    if ( Clock_IsBSPSupported(&pCfg[nCfg]) )
    {
      nSupCfg = nCfg;
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
  if ( pClockResource->nMinLevel > nSupCfg )
  {
    pClockResource->nMinLevel = nSupCfg;   
  }
  // Convert support index to BSP index
  // Reset default max level if it is greater than max support BSP
  if ( pClockResource->nMaxLevel > nSupCfg )
  {
    pClockResource->nMaxLevel = nSupCfg;
  }
  // Convert support index to BSP index
  /* Assert if current BSP data does not match with Boot config level */
  assert( pClockResource->nCurLevel != MAX_LEVEL );

  /* Assert if min is greater than max */
  assert( pClockResource->nMinLevel <= pClockResource->nMaxLevel );

  /* Set active mux config and boot up clock frequency */
  pClock->pDomain->pActiveMuxConfig = &pClock->pDomain->pBSPConfig[pClockResource->nCurLevel];

  return DAL_SUCCESS;

}

/* =========================================================================
**  Function : Clock_SetupXONode
** =========================================================================*/
/**
  Call back function for XO node.

  This function is invoked when the "\xo\cxo" NPA node is setup.

  @param
   npa_callback parameters

  @return
    None.

  @dependencies
    None.
*/
static void Clock_SetupXONode(Clock_NPAResourcesType  *pNPAResources, unsigned int unused2, const char **resources, unsigned int num_resources)
{
  boolean bEnabled;
  
  pNPAResources->XOHandle = npa_create_sync_client("/xo/cxo", "ClockRPM", NPA_CLIENT_REQUIRED );
  CORE_VERIFY_PTR(pNPAResources->XOHandle);
  
  Clock_IsClockEnabled((ClockIdType)pNPAResources->IPAClockResource.pClock, &bEnabled);
  if ( bEnabled )
  {
	/* Vote for xo if the IPA clocks are ON */
	npa_issue_required_request(pNPAResources->XOHandle, 1);  
  }
}


/* =========================================================================
**  Function : Clock_SetupBIMCNode
** =========================================================================*/
/**
  Call back function for BIMC node.

  This function is invoked when the "\clk\bimc" NPA node is setup.

  @param
   npa_callback parameters

  @return
    None.

  @dependencies
    None.
*/
static void Clock_SetupBIMCNode(Clock_NPAResourcesType  *pNPAResources, unsigned int unused2, const char **resources, unsigned int num_resources)
{
  boolean bEnabled;
  
  pNPAResources->BIMCHandle = npa_create_sync_client("/clk/bimc", "ClockRPM", NPA_CLIENT_REQUIRED );
  CORE_VERIFY_PTR(pNPAResources->BIMCHandle);
  
  Clock_IsClockEnabled((ClockIdType)pNPAResources->IPAClockResource.pClock, &bEnabled);
  if ( bEnabled )
  {
	  /* Vote for bimc if the IPA clocks are ON */
	  npa_issue_required_request(pNPAResources->BIMCHandle, 1); 
  }
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
  DALResult              eResult;
  boolean bEnabled;
	
  /*-----------------------------------------------------------------------*/
  /* Get the current RPM configuration.                                    */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig( &pNPAResources->CPUClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  /* Clock already enable in boot.  Enable clock for reference count */
  Clock_EnableClock( (ClockIdType)pNPAResources->CPUClockResource.pClock );

  
  /*-----------------------------------------------------------------------*/
  /* Get the current Sytem NOC configuration.                              */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig( &pNPAResources->SNOCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  /* Clock already enable in boot.  Enable clock for reference count */
  Clock_EnableClock( (ClockIdType)pNPAResources->SNOCClockResource.pClock );
  
  /*-----------------------------------------------------------------------*/
  /* Get the current Peripheral Config NOC configuration.                  */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig( &pNPAResources->PCNOCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  /* Clock already enable in boot.  Enable clock for reference count */
  Clock_EnableClock( (ClockIdType)pNPAResources->PCNOCClockResource.pClock );

  /*-----------------------------------------------------------------------*/
  /* Get the current BIMC configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig( &pNPAResources->BIMCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  /* Clock already enable in boot.  Enable clock for reference count */
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
  /* Get the current IPA Configuration.                         */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig( &pNPAResources->IPAClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;
	
 /* Set up XO node request  */
  npa_resource_available_cb( "/xo/cxo", (npa_callback)Clock_SetupXONode, pNPAResources);
 
 /* Set up BIMC node request  */
  npa_resource_available_cb( "/clk/bimc", (npa_callback)Clock_SetupBIMCNode, pNPAResources);

  Clock_IsClockEnabled((ClockIdType)pNPAResources->IPAClockResource.pClock, &bEnabled);
  /*
   * If the clocks were enabled in SBL already, we call EnableClock to bumpup the clocks' refcount
   * so that we have a valid count while disabling the clocks.
   */
  if ( bEnabled )
  {
    Clock_EnableClock( (ClockIdType)pNPAResources->IPAClockResource.pClock );
    Clock_EnableClock( (ClockIdType)pNPAResources->pIPAAHBClk );
    Clock_EnableClock( (ClockIdType)pNPAResources->pIPASysAxiClk );
    Clock_EnableClock( (ClockIdType)pNPAResources->pIPASleepClk );
  }

  /*-----------------------------------------------------------------------*/
  /* Get the current Crypto Configuration.                                    */
  /*-----------------------------------------------------------------------*/
  eResult = Clock_DetectClockConfig( &pNPAResources->CEClockResource.ClkCE );
  if ( eResult != DAL_SUCCESS )
    return eResult;
 
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
  assert( Clock_GetClockId("gcc_qpic_ahb_clk",(ClockIdType *)&pNPAResources->pQPICAHBClk) == DAL_SUCCESS);

  /* Mark QPIC clock as non-suppressible clock */
  CLOCK_FLAG_CLEAR( pNPAResources->QPICClockResource.pClock->pDomain, DOMAIN_CPU);

  // Check if these clock ON by boot loader
  Clock_IsClockOn((ClockIdType)pNPAResources->QPICClockResource.pClock, &pNPAResources->bQPICEnable );

}


static void Clock_InitIPA( void )
{

  Clock_NPAResourcesType  *pNPAResources = Clock_GetNPAResources();

  assert( Clock_GetClockId("gcc_ipa_clk",(ClockIdType *)&pNPAResources->IPAClockResource.pClock) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_ipa_ahb_clk",(ClockIdType *)&pNPAResources->pIPAAHBClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_sys_noc_ipa_axi_clk",(ClockIdType *)&pNPAResources->pIPASysAxiClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_ipa_sleep_clk",(ClockIdType *)&pNPAResources->pIPASleepClk) == DAL_SUCCESS);

}


static DALResult Clock_ConfigBIMCClocks( void )
{
  DALResult               eRes;
  BIMCResourceType        *pBIMCRsc;
  Clock_NPAResourcesType  *pNPAResources;

  pNPAResources = Clock_GetNPAResources();
  pBIMCRsc = &pNPAResources->BIMCResource;
 
  pBIMCRsc->eBIMCState = BIMC_ON;
  pBIMCRsc->bPowerCollapsed = TRUE;

  eRes = Clock_GetClockId( "gcc_bimc_clk", (ClockIdType *)&pNPAResources->BIMCClockResource.pClock);
  assert (eRes == DAL_SUCCESS);
  
  /* Initial DEHR clock */
  eRes = Clock_GetClockId( "gcc_dehr_clk", &pBIMCRsc->nClockDehr );
  assert (eRes == DAL_SUCCESS);

  /* Call Enable BIMC power domain for increment reference count, so it can collapse later */
  eRes = Clock_GetPowerDomainId( "VDD_BIMC", &pBIMCRsc->nBIMCPowerID );
  assert (eRes == DAL_SUCCESS);

  Clock_EnablePowerDomain( pBIMCRsc->nBIMCPowerID, TRUE ); 

  return DAL_SUCCESS;
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
  DALResult eRes;
  Clock_NPAResourcesType  *pNPAResources;
 
  pNPAResources = Clock_GetNPAResources();
  pDrvCtxt = Clock_GetDrvCtxt();

  /* Default DCVS is enabled. */
  pDrvCtxt->bDCVSEnabled = TRUE;
 
  SWEVENT(CLOCK_EVENT_INIT);

  /* Set bus Min/Max for profiling */
  Clock_BusSetMinMax( pNPAResources );

  /* Suspend voltage scaling during init */
  Clock_VoltageSuspend( pDrvCtxt, TRUE);

  /*-----------------------------------------------------------------------*/
  /* Get RPM clock ID and enable for reference count                       */
  /*-----------------------------------------------------------------------*/
  assert( Clock_GetClockId("gcc_rpm_proc_fclk",
                           (ClockIdType *)&pNPAResources->CPUClockResource.pClock) == DAL_SUCCESS);

  /*-----------------------------------------------------------------------*/
  /* Marking RPM CPU clock as non-voltage change                           */
  /*-----------------------------------------------------------------------*/
  CLOCK_FLAG_SET(pNPAResources->CPUClockResource.pClock->pDomain, 
                 DOMAIN_HOLD_VREG_REQUEST);		

  /*-----------------------------------------------------------------------*/
  /* Get Sytem NOC clock ID and enable for reference count                 */
  /*-----------------------------------------------------------------------*/
  assert( Clock_GetClockId("gcc_sys_noc_axi_clk",
                           (ClockIdType *)&pNPAResources->SNOCClockResource.pClock ) == DAL_SUCCESS);

  /*-----------------------------------------------------------------------*/
  /* Get Peripheral Config NOC clock ID                                           */
  /*-----------------------------------------------------------------------*/

  assert( Clock_GetClockId("gcc_pcnoc_ahb_clk",
                           (ClockIdType *)&pNPAResources->PCNOCClockResource.pClock ) == DAL_SUCCESS);

  /*-----------------------------------------------------------------------*/
  /* Get CE (Crypto Engine ) clock ID                                    */
  /*-----------------------------------------------------------------------*/
  assert( Clock_GetClockId( "gcc_crypto_clk", 
                           (ClockIdType *)&pNPAResources->CEClockResource.ClkCE.pClock ) == DAL_SUCCESS);  
  assert( Clock_GetClockId( "gcc_crypto_axi_clk", 
                           (ClockIdType *)&pNPAResources->CEClockResource.pCEAXIClk ) == DAL_SUCCESS);
  assert( Clock_GetClockId( "gcc_crypto_ahb_clk", 
                           (ClockIdType *)&pNPAResources->CEClockResource.pCEAHBClk ) == DAL_SUCCESS);
  /*-----------------------------------------------------------------------*/
  /* Get BIMC clock ID                                                     */
  /*-----------------------------------------------------------------------*/
  Clock_ConfigBIMCClocks();
  /*-----------------------------------------------------------------------*/
  /* Get IPA clock ID                                                     */
  /*-----------------------------------------------------------------------*/
  Clock_InitIPA();


  /*-----------------------------------------------------------------------*/
  /* Get QPIC clock ID                                                     */
  /*-----------------------------------------------------------------------*/
  Clock_InitQPIC();

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

  /* Moved this after Clock_DetectRPMConfig to let PCNOC initialize first so 
     that the clock domain shared by PCNOC and QPIC AHB clock gets initialized
     properly */
  if ( pNPAResources->bQPICEnable )
  {
    Clock_EnableClock( (ClockIdType)pNPAResources->QPICClockResource.pClock );
    Clock_EnableClock( (ClockIdType)pNPAResources->pQPICAHBClk );
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize NPA clock data                                             */
  /*-----------------------------------------------------------------------*/
  Clock_InitNPA();

  /*-----------------------------------------------------------------------*/
  /* Expose the nodes to the outside world via rpmserver                   */
  /*-----------------------------------------------------------------------*/
  clk0_adapter = rpm_create_npa_adapter(RPM_CLOCK_0_REQ, 3); // Misc clocks: [CXO, DCVS, QDSS]
  rpm_associate_npa_resource(clk0_adapter, 0, pNPAResources->CXONPANode.Resource.name);
  rpm_associate_npa_resource(clk0_adapter, 1, pNPAResources->QDSSClockResource.NPANode.Resource.name);
  rpm_associate_npa_resource(clk0_adapter, 2, pNPAResources->DCVSEnaNPANode.Resource.name);

  clk1_adapter = rpm_create_npa_adapter(RPM_CLOCK_1_REQ, 2); // Bus clocks: [pcnoc, snoc]
  rpm_associate_npa_resource(clk1_adapter, 0, pNPAResources->PCNOCClockResource.NPANode.Resource.name);
  rpm_associate_npa_resource(clk1_adapter, 1, pNPAResources->SNOCClockResource.NPANode.Resource.name);

  clk2_adapter = rpm_create_npa_settling_adapter(RPM_CLOCK_2_REQ, 1); // Memory clocks: [bimc]
  rpm_associate_npa_resource(clk2_adapter, 0, pNPAResources->BIMCClockResource.NPANode.Resource.name);

  qpic_adapter = rpm_create_npa_adapter(RPM_CLOCK_QPIC_REQ, 1); // qpic clock
  rpm_associate_npa_resource(qpic_adapter, 0, pNPAResources->QPICClockResource.NPANode.Resource.name);

  ipa_adapter = rpm_create_npa_adapter(RPM_CLOCK_IPA_REQ, 1); // ipa clock
  rpm_associate_npa_resource(ipa_adapter, 0, pNPAResources->IPAClockResource.NPANode.Resource.name);

  ce_adapter = rpm_create_npa_adapter(RPM_CLOCK_CE_REQ, 1); // CE clock
  rpm_associate_npa_resource(ce_adapter, 0, pNPAResources->CEClockResource.ClkCE.NPANode.Resource.name);  
  
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
  Clock_NPAResourcesType* pNPAResources;
  ClockDrvCtxt            *pDrvCtxt;  
  boolean                 bBIMCPC;
  
  pNPAResources = Clock_GetNPAResources();
  pDrvCtxt = Clock_GetDrvCtxt();

  /* Suspend change voltage during power collapse */
  Clock_VoltageSuspend( pDrvCtxt, TRUE);

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
	  
    /* Turn off misc clocks that are not required */
    HAL_clk_MiscClkDisableDeepSleep();

    /* Collapse all non-BIMC clocks to XO speed */
    if ( pNPAResources->IPAClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->IPAClockResource.nSavLevel = pNPAResources->IPAClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->IPAClockResource.pClock->pDomain,
                            &pNPAResources->IPAClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->SNOCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->SNOCClockResource.nSavLevel = pNPAResources->SNOCClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->SNOCClockResource.pClock->pDomain,
                            &pNPAResources->SNOCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->PCNOCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->PCNOCClockResource.nSavLevel = pNPAResources->PCNOCClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->PCNOCClockResource.pClock->pDomain,
                            &pNPAResources->PCNOCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    // Switch CPU clock to last for highest performance
    if ( pNPAResources->CPUClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->CPUClockResource.nSavLevel = pNPAResources->CPUClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->CPUClockResource.pClock->pDomain, 
                            &pNPAResources->CPUClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }
     
    HAL_clk_GPLL0_disable_deep_sleep(); 
    
  }

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
  Clock_NPAResourcesType* pNPAResources;
  ClockDrvCtxt            *pDrvCtxt;  

  pDrvCtxt = Clock_GetDrvCtxt();

  /* Don't allow voltage change during processor restore */
  Clock_VoltageSuspend( pDrvCtxt, TRUE);  

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
    
    HAL_clk_GPLL0_FSM_reset_deep_sleep();
    
    // Need to restore CPU clock up first for higher performance
    if ( pNPAResources->CPUClockResource.nCurLevel != XO_LEVEL )
    {
      Clock_SetClockConfig( pNPAResources->CPUClockResource.pClock->pDomain, 
                            &pNPAResources->CPUClockResource.pClock->pDomain->pBSPConfig[pNPAResources->CPUClockResource.nSavLevel] );
    }

    /* Switch the rest to previous clock speed */
    if ( pNPAResources->PCNOCClockResource.nCurLevel != XO_LEVEL )
    {
      Clock_SetClockConfig( pNPAResources->PCNOCClockResource.pClock->pDomain, 
                            &pNPAResources->PCNOCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->PCNOCClockResource.nSavLevel] );
    }

    if ( pNPAResources->SNOCClockResource.nCurLevel != XO_LEVEL )
    {
      Clock_SetClockConfig( pNPAResources->SNOCClockResource.pClock->pDomain,
                            &pNPAResources->SNOCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->SNOCClockResource.nSavLevel] );
    }

    if ( pNPAResources->IPAClockResource.nCurLevel != XO_LEVEL )
    {
      Clock_SetClockConfig( pNPAResources->IPAClockResource.pClock->pDomain,
                            &pNPAResources->IPAClockResource.pClock->pDomain->pBSPConfig[pNPAResources->IPAClockResource.nSavLevel] );
    }

    /* Turn misc clocks on */ 
    HAL_clk_MiscClkRestoreDuringWakeup();

    /* Restore BIMC */
    Clock_SetBIMCPowerRestore();
  }

  /* Restore done, allow voltage change when exit power collapse, but don't update PMIC */
  Clock_VoltageResume( pDrvCtxt, FALSE );

  return DAL_SUCCESS;

} /* END Clock_ProcessorRestore */


/* =========================================================================
**  Function : Clock_Init
** =========================================================================*/
/*
  See ClockDriver.h
*/

boolean Clock_Init (void)
{
  Clock_DriverInit();

  return TRUE;

} /* END Clock_Init */

