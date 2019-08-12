/** @file ClockRPM.c

  This file implements specialized image specific functions such
  as init image for RPM, we have few place holders for now.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/hw/msm8952/ClockRPM.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
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
#define QDSS_CLOCK_NUM          15

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
static rpm_npa_adapter clk1_adapter; // Bus clocks: [pcnoc, snoc, sysmmnoc]
static rpm_npa_adapter clk2_adapter; // Memory clocks: [bimc]
static rpm_npa_adapter ipa_adapter;  // IPA
static ClockVddMinType clkVddMin;    // Clocks turn off during xo shutdown

typedef struct
{
  char   *szClockName;
  ClockIdType  *pClockId;
} clock_id_type;

ClockIdType                     nClockBIMCAPSSAXI;

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

  /* Default setting for IPA clock */
  pNPAResources->IPAClockResource.nMinLevel = 0;
  pNPAResources->IPAClockResource.nMaxLevel = MAX_LEVEL;

  
  /* Default setting for APSS_TCU_ASYNC clock */
  pNPAResources->APSSTCUASYNCClockResource.nMinLevel = 0;
  pNPAResources->APSSTCUASYNCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for Q6 TBU clock */
  pNPAResources->Q6TBUClockResource.nMinLevel = 0;
  pNPAResources->Q6TBUClockResource.nMaxLevel = MAX_LEVEL;

   /* Default setting for APSS_AXI clock */
  pNPAResources->APSSAXIClockResource.nMinLevel = 0;
  pNPAResources->APSSAXIClockResource.nMaxLevel = MAX_LEVEL;

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
    {"gcc_sys_noc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSysNocATClk},
    {"gcc_pcnoc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSPCNocATClk},

    {"gcc_qdss_traceclkin_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTraceClk},
    {"gcc_qdss_stm_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSTMClk},
    {"gcc_sys_noc_qdss_stm_axi_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSTMAXIClk},
    {"gcc_qdss_dap_clk", (ClockIdType *)&pQDSSClockResource->pQDSSDapClk},

    {"gcc_qdss_tsctr_div2_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv2Clk},
    {"gcc_qdss_tsctr_div3_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv3Clk},
    {"gcc_qdss_tsctr_div4_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv4Clk},
    {"gcc_qdss_tsctr_div8_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv8Clk},
    {"gcc_qdss_tsctr_div16_clk", (ClockIdType *)&pQDSSClockResource->pQDSSTSCTRDiv16Clk}
  };

  clock_id_type sQDSSATClock[3] =
  {
    {"gcc_qdss_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSATClk},
    {"gcc_sys_noc_at_clk", (ClockIdType *)&pQDSSClockResource->pQDSSSysNocATClk},
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
  assert(pNPAResources->XOHandle);
  
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
  assert(pNPAResources->BIMCHandle);
  
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
  DALResult eResult;
  boolean bEnabled;

  /*-----------------------------------------------------------------------*/
  /* Get the current RPM configuration.                                    */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->CPUClockResource);
  if ( eResult != DAL_SUCCESS )
    return eResult;

  /* Clock already enable in boot.  Enable clock for reference count */
  Clock_EnableClock( (ClockIdType)pNPAResources->CPUClockResource.pClock );


  /*-----------------------------------------------------------------------*/
  /* Get the current Sytem NOC configuration.                              */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->SNOCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->SNOCClockResource.pClock );

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
  /* Get the current APSS_TCU_ASYNC clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->APSSTCUASYNCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;
 
  Clock_EnableClock( (ClockIdType)pNPAResources->APSSTCUASYNCClockResource.pClock );

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
    Clock_EnableClock( (ClockIdType)pNPAResources->pIPATBUClk );
    Clock_EnableClock( (ClockIdType)pNPAResources->pIPA2XClk );
    Clock_EnableClock( (ClockIdType)pNPAResources->pIPASleepClk );
  }
  /*-----------------------------------------------------------------------*/
  /* Get the current Q6_TBU clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->Q6TBUClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->Q6TBUClockResource.pClock );

 
  /*-----------------------------------------------------------------------*/
  /* Get the current APSS_AXI clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->APSSAXIClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;
 
  Clock_EnableClock((ClockIdType)pNPAResources->APSSAXIClockResource.pClock);
  Clock_EnableClock(nClockBIMCAPSSAXI);
 
  /*-----------------------------------------------------------------------*/
  /* Setup QDSS resource                                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_QDSSConfig( &pNPAResources->QDSSClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  return DAL_SUCCESS;

} /* END Clock_DetectRPMConfig */


static void Clock_InitIPA( void )
{

  Clock_NPAResourcesType  *pNPAResources = Clock_GetNPAResources();

  assert( Clock_GetClockId("gcc_ipa_clk",(ClockIdType *)&pNPAResources->IPAClockResource.pClock) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_ipa_ahb_clk",(ClockIdType *)&pNPAResources->pIPAAHBClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_sys_noc_ipa_axi_clk",(ClockIdType *)&pNPAResources->pIPASysAxiClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_ipa_2x_clk",(ClockIdType *)&pNPAResources->pIPA2XClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_ipa_tbu_clk",(ClockIdType *)&pNPAResources->pIPATBUClk) == DAL_SUCCESS);
  assert( Clock_GetClockId("gcc_ipa_sleep_clk",(ClockIdType *)&pNPAResources->pIPASleepClk) == DAL_SUCCESS);
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

  /* Initialize a list of clocks that needs to manage during xo shutdown */
  Clock_InitClkVddMin();

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
  /* Get Sytem NOC clock ID and enable for reference count                 */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_GetClockId( "gcc_sys_noc_axi_clk",
                           (ClockIdType *)&pNPAResources->SNOCClockResource.pClock );

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get System NOC clock ID.");
    return eRes;
  }

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
  /* Get Q6_TBU clock ID                                                   */
  /*-----------------------------------------------------------------------*/
  eRes = Clock_GetClockId( "gcc_mss_q6_bimc_axi_clk", 
                           (ClockIdType *)&pNPAResources->Q6TBUClockResource.pClock );

  if (eRes != DAL_SUCCESS) 
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get Q6 TBU clock ID.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Get APSS AXI clock ID                                          */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_GetClockId( "gcc_apss_axi_clk",
                           (ClockIdType *)&pNPAResources->APSSAXIClockResource.pClock );

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get APSS AXI clock ID.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Get BIMC APSS AXI clock ID                                       */
  /*-----------------------------------------------------------------------*/
  eRes = Clock_GetClockId( "gcc_bimc_apss_axi_clk", &nClockBIMCAPSSAXI);

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get BIMC APSS AXI clock ID.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Get IPA clock ID                                                     */
  /*-----------------------------------------------------------------------*/
  Clock_InitIPA();


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

  /*-----------------------------------------------------------------------*/
  /* Initialize NPA clock data                                             */
  /*-----------------------------------------------------------------------*/
  Clock_InitNPA();

  /*-----------------------------------------------------------------------*/
  /* Expose the nodes to the outside world via rpmserver                   */
  /*-----------------------------------------------------------------------*/
  clk0_adapter = rpm_create_npa_adapter(RPM_CLOCK_0_REQ, 3); // Misc clocks: [CXO, QDSS, dcvs.ena]
  rpm_associate_npa_resource(clk0_adapter, 0, pNPAResources->CXONPANode.Resource.name);
  rpm_associate_npa_resource(clk0_adapter, 1, pNPAResources->QDSSClockResource.NPANode.Resource.name);
  rpm_associate_npa_resource(clk0_adapter, 2, pNPAResources->DCVSEnaNPANode.Resource.name);

  clk1_adapter = rpm_create_npa_adapter(RPM_CLOCK_1_REQ, 2); // Bus clocks: [pcnoc, snoc]
  rpm_associate_npa_resource(clk1_adapter, 0, pNPAResources->PCNOCClockResource.NPANode.Resource.name);
  rpm_associate_npa_resource(clk1_adapter, 1, pNPAResources->SNOCClockResource.NPANode.Resource.name);

  clk2_adapter = rpm_create_npa_settling_adapter(RPM_CLOCK_2_REQ, 1); // Memory clocks: [bimc ]
  rpm_associate_npa_resource(clk2_adapter, 0, pNPAResources->BIMCClockResource.NPANode.Resource.name);

  ipa_adapter = rpm_create_npa_adapter(RPM_CLOCK_IPA_REQ, 1); // IPA  clock
  rpm_associate_npa_resource(ipa_adapter, 0, pNPAResources->IPAClockResource.NPANode.Resource.name);

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
  boolean                 bBIMCPC;
  
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
      QDSSDDRTraceDisable();
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
      Clock_SetClockConfigEx( pNPAResources->SNOCClockResource.pClock, 
                            &pNPAResources->SNOCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }


    if ( pNPAResources->APSSTCUASYNCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->APSSTCUASYNCClockResource.nSavLevel = pNPAResources->APSSTCUASYNCClockResource.nCurLevel;
      Clock_SetClockConfigEx( pNPAResources->APSSTCUASYNCClockResource.pClock, 
                            &pNPAResources->APSSTCUASYNCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->Q6TBUClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->Q6TBUClockResource.nSavLevel = pNPAResources->Q6TBUClockResource.nCurLevel;
      Clock_SetClockConfigEx( pNPAResources->Q6TBUClockResource.pClock, 
                            &pNPAResources->Q6TBUClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->APSSAXIClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->APSSAXIClockResource.nSavLevel = pNPAResources->APSSAXIClockResource.nCurLevel;
      Clock_SetClockConfigEx( pNPAResources->APSSAXIClockResource.pClock, 
                            &pNPAResources->APSSAXIClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
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

  SWEVENT( CLOCK_EVENT_POWER_RESTORE, CLOCK_SWE_PROCESSOR_ENTER );
  
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
      QDSSDDRTraceEnable();
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
 
    /* Turn clocks to RAMs On */
    /* Check if this is required? */
    Clock_EnableClkVddMin( TRUE );
	
    if ( pNPAResources->SNOCClockResource.nCurLevel != XO_LEVEL )
    {
      /* Switch the rest to previous clock speed */
      Clock_SetClockConfigEx( pNPAResources->SNOCClockResource.pClock, 
                            &pNPAResources->SNOCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->SNOCClockResource.nSavLevel] );
    }

   if ( pNPAResources->APSSTCUASYNCClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfigEx( pNPAResources->APSSTCUASYNCClockResource.pClock, 
                            &pNPAResources->APSSTCUASYNCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->APSSTCUASYNCClockResource.nSavLevel] );
    }

   if ( pNPAResources->Q6TBUClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfigEx( pNPAResources->Q6TBUClockResource.pClock, 
                            &pNPAResources->Q6TBUClockResource.pClock->pDomain->pBSPConfig[pNPAResources->Q6TBUClockResource.nSavLevel] );
    }


     if ( pNPAResources->APSSAXIClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfigEx( pNPAResources->APSSAXIClockResource.pClock, 
                            &pNPAResources->APSSAXIClockResource.pClock->pDomain->pBSPConfig[pNPAResources->APSSAXIClockResource.nSavLevel] );
    }
    
    if ( pNPAResources->IPAClockResource.nCurLevel != XO_LEVEL )
    {
      Clock_SetClockConfig( pNPAResources->IPAClockResource.pClock->pDomain,
                            &pNPAResources->IPAClockResource.pClock->pDomain->pBSPConfig[pNPAResources->IPAClockResource.nSavLevel] );
    }

   /*Turn misc clocks on*/ 
//    HAL_clk_MiscClkRestoreDuringWakeup();

    /* Restore BIMC */
    Clock_SetBIMCPowerRestore();

 } /* 
   * Allow voltage change.
   */
  Clock_VoltageResume( pDrvCtxt, FALSE );

  SWEVENT( CLOCK_EVENT_POWER_RESTORE, CLOCK_SWE_PROCESSOR_DONE );
  
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
  return (Clock_DriverInit() == DAL_SUCCESS) ? TRUE : FALSE;
}

