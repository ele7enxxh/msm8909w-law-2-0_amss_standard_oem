/** @file ClockRPM.c

  This file implements specialized image specific functions such
  as init image for RPM, we have few place holders for now.

  Copyright (c) 2014, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/hw/msm8909/ClockRPM.c#5 $
  $DateTime: 2015/07/02 03:40:56 $
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

  /* Default setting for SNOC clock */
  pNPAResources->SNOCClockResource.nMinLevel = 0;
  pNPAResources->SNOCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for PCNOC clock */
  pNPAResources->PCNOCClockResource.nMinLevel = 0;
  pNPAResources->PCNOCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for BIMC clock */
  pNPAResources->BIMCClockResource.nMinLevel = 0;

  hal_part_num = HAL_clk_GetHWParNum();
  if( hal_part_num == part_num_8208)
    pNPAResources->BIMCClockResource.nMaxLevel = 4;
  else
    pNPAResources->BIMCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for BIMC GPU  clock */
  pNPAResources->BIMCGPUClockResource.nMinLevel = 0;
  pNPAResources->BIMCGPUClockResource.nMaxLevel = MAX_LEVEL;
  
  /* Default setting for QPIC clock */
  pNPAResources->QPICClockResource.nMinLevel = 0;
  pNPAResources->QPICClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for APSS_TCU_ASYNC clock */
  pNPAResources->APSSTCUASYNCClockResource.nMinLevel = 0;
  pNPAResources->APSSTCUASYNCClockResource.nMaxLevel = MAX_LEVEL;

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

uint32 snoc_curr_vote = 0;


/* =========================================================================
**  Function : Clock_SNOCInitVote
** =========================================================================*/
/* This function is called to configure the intial snoc  vote depending on the state of the APPS and bimc frequency.
*/

static void Clock_SNOCInitVote(Clock_NPAResourcesType  *pNPAResources)
{
  uint32 curlevel;
  uint32 curfreq;
  
  if(!pNPAResources->SNOCHandle || !pNPAResources->SYSPCHandle)
      return;

  if( Clock_GetSubSysState(RPM_MASTER_APPS) == SUBSYS_ACTIVE_STATE)
  {
    
    curlevel = pNPAResources->BIMCClockResource.nCurLevel;
    curfreq = pNPAResources->BIMCClockResource.pClock->pDomain->pBSPConfig[curlevel].nFreqHz;
    curfreq = curfreq/2000;/*converting into Khz and dividing by half*/
    snoc_curr_vote = curfreq;
  }
  else 
  {
    snoc_curr_vote = 0;
  }
  
  npa_issue_required_request(pNPAResources->SNOCHandle, snoc_curr_vote); 
}



/* =========================================================================
**  Function : Clock_SetupBIMCNode
** =========================================================================*/
/**
  Call back function for SNOC node.

  This function is invoked when the "\clk\snoc" NPA node is setup.

  @param
   npa_callback parameters

  @return
    None.

  @dependencies
    None.
*/
static void Clock_SetupSNOCNode(Clock_NPAResourcesType  *pNPAResources, unsigned int unused2, const char **resources, unsigned int num_resources)
{
  pNPAResources->SNOCHandle = npa_create_sync_client("/clk/snoc", "ClockRPM", NPA_CLIENT_REQUIRED );
  assert(pNPAResources->SNOCHandle);

  Clock_SNOCInitVote(pNPAResources);

}


/* =========================================================================
**  Function : Clock_SetupSYSPCNode
** =========================================================================*/
/**
  Call back function for SNOC node.

  This function is invoked when the "/server/sys_pc_state" NPA node is setup.

  @param
   npa_callback parameters

  @return
    None.

  @dependencies
    None.
*/
static void Clock_SetupSYSPCNode(Clock_NPAResourcesType  *pNPAResources, unsigned int unused2, const char **resources, unsigned int num_resources)
{

  pNPAResources->SYSPCHandle = npa_create_query_handle("/server/sys_pc_state");
  assert(pNPAResources->SYSPCHandle);
  
  Clock_SNOCInitVote(pNPAResources);
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
  /* Get the current BIMC_GPU clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->BIMCGPUClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->BIMCGPUClockResource.pClock );
   
  /*-----------------------------------------------------------------------*/
  /* Get the current QPIC configuration.                                   */
  /*-----------------------------------------------------------------------*/

  if ( pNPAResources->bQPICEnable )
  {
  eResult = Clock_DetectClockConfig( &pNPAResources->QPICClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;
  }

  
  /* Set up SNOC node request  */
   npa_resource_available_cb( "/clk/snoc", (npa_callback)Clock_SetupSNOCNode, pNPAResources);

  /* Set up SYSPC node request  */
   npa_resource_available_cb( "/server/sys_pc_state", (npa_callback)Clock_SetupSYSPCNode, pNPAResources);


  /*-----------------------------------------------------------------------*/
  /* Get the current APSS_TCU_ASYNC clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->APSSTCUASYNCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;
 
  Clock_EnableClock( (ClockIdType)pNPAResources->APSSTCUASYNCClockResource.pClock );
 
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

  if ( pNPAResources->bQPICEnable )
  {
    Clock_EnableClock( (ClockIdType)pNPAResources->QPICClockResource.pClock );
    Clock_EnableClock( (ClockIdType)pNPAResources->pQPICSysClk );
    Clock_EnableClock( (ClockIdType)pNPAResources->pQPICAHBClk );
  }
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
  ClockResourceType       *pBIMCClockResource;
  HAL_clk_PLLConfigType   BIMCPllCfg;
  uint32                  nCfg;

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
  /* Get BIMC_GPU clock ID                                                 */
  /*-----------------------------------------------------------------------*/
  eRes = Clock_GetClockId( "gcc_bimc_gpu_clk", 
                           (ClockIdType *)&pNPAResources->BIMCGPUClockResource.pClock );

  if (eRes != DAL_SUCCESS) 
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get BIMC_GPU clock ID.");
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

  HAL_clk_DetectPLLConfig(HAL_CLK_SOURCE_BIMCPLL, &BIMCPllCfg);
  
  if(BIMCPllCfg.nL == 0x36)
  {
      nCfg = 0;
      pBIMCClockResource = &pNPAResources->BIMCClockResource;
      while(pBIMCClockResource->pClock->pDomain->pBSPConfig[nCfg].nFreqHz!=0)
      {      
          if(pBIMCClockResource->pClock->pDomain->pBSPConfig[nCfg].nFreqHz == 533000000)
          {
             pBIMCClockResource->pClock->pDomain->pBSPConfig[nCfg].nFreqHz = 518400000;
             break;
          }
          nCfg++;
       }
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

    

    /* Collapse all non-BIMC clocks to XO speed */
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

    if ( pNPAResources->BIMCGPUClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->BIMCGPUClockResource.nSavLevel = pNPAResources->BIMCGPUClockResource.nCurLevel;
      Clock_SetClockConfigEx( pNPAResources->BIMCGPUClockResource.pClock, 
                            &pNPAResources->BIMCGPUClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
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
	
   if ( pNPAResources->BIMCGPUClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfigEx( pNPAResources->BIMCGPUClockResource.pClock, 
                            &pNPAResources->BIMCGPUClockResource.pClock->pDomain->pBSPConfig[pNPAResources->BIMCGPUClockResource.nSavLevel] );
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



/* =========================================================================
**  Function : Clock_GetSubSysState
** =========================================================================*/
/* This function is used to get the state of a given subsystem. 
  It will query the /server/sys_pc_state resource to get the state of the all the subsystem
  npa_query will return the int value, bits of this integer is mapped to subsystem.
  Look for the rpm.h for the bit mapping, enum rpm_master_client gives the position of 
  the subsystem. If the bit is set to SUBSYS_ACTIVE_STATE it means  subsystem is is in active state, 
  while if is set to SUBSYS_PC_STATE it is power collapsed.
  
  @param
  SubSysType [in] -- Subsystem to be queried.

  @return
    SUBSYS_MAX_NUM_STATE if client handle is not created for sys_pc_state, 
    other return the state of the subsystem.

  @dependencies
    None.
*/
rpm_subSystemState Clock_GetSubSysState(rpm_master_client SubSysType)
{
  Clock_NPAResourcesType  *pNPAResources;
  pNPAResources = Clock_GetNPAResources();
  npa_query_type   sleep_query_result;
  npa_query_status sleep_query_status;
  rpm_subSystemState  sys_state;

  if(pNPAResources->SYSPCHandle != NULL)
  {
    sleep_query_status = npa_query(pNPAResources->SYSPCHandle, NPA_QUERY_CURRENT_STATE, &sleep_query_result);
    assert(NPA_QUERY_SUCCESS == sleep_query_status);
    assert(NPA_QUERY_TYPE_STATE == sleep_query_result.type);
  
    sys_state = (rpm_subSystemState)((sleep_query_result.data.state & (1 << SubSysType) ) >> SubSysType);
//    subSysState[SubSysType] = sys_state;
    return sys_state;
  }
  else
  {
    return SUBSYS_MAX_NUM_STATE;
  }
 
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


