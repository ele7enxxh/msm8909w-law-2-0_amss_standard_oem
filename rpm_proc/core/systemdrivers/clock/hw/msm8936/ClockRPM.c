/** @file ClockRPM.c

  This file implements specialized image specific functions such
  as init image for RPM, we have few place holders for now.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/hw/msm8936/ClockRPM.c#1 $
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

extern boolean message_ram_vmpm_isMBADone(void);

/*=========================================================================
      Type Definitions
==========================================================================*/
#define XO_LEVEL                0
#define MAX_LEVEL               0x7FFFFFFF
#define XO_SPEED                19200
#define BIMC_PRE_COLLAPSE_FREQ  100000
#define BIMC_DDR_LEGACY_MODE    0x00000400
#define DEHR_SAVE_DONE          0x00000001
#define DEHR_RESTORE_DONE       0x00010000
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
static ClockVddMinType clkVddMin;    // Clocks turn off during xo shutdown

volatile uint32 clk_dbg_bimc_pc_count = 0;
volatile uint32 clk_dbg_bimc_pc_restore_count = 0;

typedef struct
{
  char   *szClockName;
  ClockIdType  *pClockId;
} clock_id_type;

static ClockIdType              nClockDehr;
static boolean                  bBIMCPC;
static ClockPowerDomainIdType   nBIMCPowerDomainID; 

ClockIdType                     nClockBIMCSysMMNOCAXI;
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

  /* Default setting for SYSMMNOC clock */
  pNPAResources->SysMMNOCClockResource.nMinLevel = 0;
  pNPAResources->SysMMNOCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for PCNOC clock */
  pNPAResources->PCNOCClockResource.nMinLevel = 0;
  pNPAResources->PCNOCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for BIMC clock */
  pNPAResources->BIMCClockResource.nMinLevel = 0;
  pNPAResources->BIMCClockResource.nMaxLevel = MAX_LEVEL;

  
  /* Default setting for APSS_TCU_ASYNC clock */
  pNPAResources->APSSTCUASYNCClockResource.nMinLevel = 0;
  pNPAResources->APSSTCUASYNCClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for Q6 TBU clock */
  pNPAResources->Q6TBUClockResource.nMinLevel = 0;
  pNPAResources->Q6TBUClockResource.nMaxLevel = MAX_LEVEL;

  /* Default setting for BIMC GPU  clock */
  pNPAResources->BIMCGPUClockResource.nMinLevel = 0;
  pNPAResources->BIMCGPUClockResource.nMaxLevel = MAX_LEVEL;

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

  pQDSSClockResource->nQDSSCurrLevel = CLOCK_QDSS_LEVEL_OFF;

  for (i=0; i < QDSS_CLOCK_NUM; i++)
  {
	if ( DAL_SUCCESS != Clock_GetClockId(sQDSSClock[i].szClockName, 
		sQDSSClock[i].pClockId) )
	{
      return DAL_ERROR;
	}
    Clock_EnableClock(*sQDSSClock[i].pClockId);
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
  /* Get the current Sytem NOC configuration.                              */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->SNOCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->SNOCClockResource.pClock );

 /*-----------------------------------------------------------------------*/
  /* Get the current Sytem MMNOC AXI configuration.                              */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->SysMMNOCClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;
  
  Clock_EnableClock( (ClockIdType)pNPAResources->SysMMNOCClockResource.pClock );
  Clock_EnableClock( nClockBIMCSysMMNOCAXI );

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
  /* Get the current Q6_TBU clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->Q6TBUClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->Q6TBUClockResource.pClock );

  /*-----------------------------------------------------------------------*/
  /* Get the current BIMC_GPU clk configuration.                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DetectClockConfig(&pNPAResources->BIMCGPUClockResource );
  if ( eResult != DAL_SUCCESS )
    return eResult;

  Clock_EnableClock( (ClockIdType)pNPAResources->BIMCGPUClockResource.pClock );
 
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
  /* Get Sytem MMNOC AXI clock ID                                          */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_GetClockId( "gcc_sys_mm_noc_axi_clk",
                           (ClockIdType *)&pNPAResources->SysMMNOCClockResource.pClock );

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get System MMNOC AXI clock ID.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Get BIMC SYS MMNOC AXI clock ID                                       */
  /*-----------------------------------------------------------------------*/
  eRes = Clock_GetClockId( "gcc_bimc_sysnoc_mm_axi_clk", &nClockBIMCSysMMNOCAXI);

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get BIMC System MMNOC AXI clock ID.");
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
  eRes = Clock_GetClockId( "gcc_bimc_clk", 
                           (ClockIdType *)&pNPAResources->BIMCClockResource.pClock );

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


  /* Call Enable BIMC power domain for increment reference count, so it can collapse later */
  Clock_GetPowerDomainId( "VDD_BIMC", &nBIMCPowerDomainID); 
  Clock_EnablePowerDomain( nBIMCPowerDomainID, TRUE ); 
  
  /* Initial DEHR clock */
  Clock_GetClockId( "gcc_dehr_clk", &nClockDehr);
  
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

  clk1_adapter = rpm_create_npa_adapter(RPM_CLOCK_1_REQ, 3); // Bus clocks: [pcnoc, snoc, sysmmnoc]
  rpm_associate_npa_resource(clk1_adapter, 0, pNPAResources->PCNOCClockResource.NPANode.Resource.name);
  rpm_associate_npa_resource(clk1_adapter, 1, pNPAResources->SNOCClockResource.NPANode.Resource.name);
  rpm_associate_npa_resource(clk1_adapter, 2, pNPAResources->SysMMNOCClockResource.NPANode.Resource.name);

  clk2_adapter = rpm_create_npa_adapter(RPM_CLOCK_2_REQ, 1); // Memory clocks: [bimc ]
  rpm_associate_npa_resource(clk2_adapter, 0, pNPAResources->BIMCClockResource.NPANode.Resource.name);

  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/

  /* Init is done, resume voltage scaling with update PMIC */
  Clock_VoltageResume( pDrvCtxt, TRUE );

  return DAL_SUCCESS;

} /* END Clock_InitImage */

/* =========================================================================
**  Function : Clock_BIMCPowerCollapsed
** =========================================================================*/
/**
  This API is used for putting BIMC to power collapsed mode.
  It switches BIMC clock to lowest speed, put DDR to self-refresh mode, and turn off BIMC power.
  
  @param 
      None

  @return
      None

  @dependencies
      None

*/
void Clock_BIMCPowerCollapsed
(
  void
)
{
  Clock_NPAResourcesType  *pNPAResources;
  ClockResourceType       *pBIMCResource;
  uint32                  nSlowestBIMCSpeed;
  boolean                 mba_init = FALSE;  
  
  pNPAResources = Clock_GetNPAResources();

  /* Switch BIMC clock to lowest speed if it has not in that level yet */
  pBIMCResource = &pNPAResources->BIMCClockResource;
  nSlowestBIMCSpeed = pBIMCResource->pClock->pDomain->pBSPConfig[0].nFreqHz/1000;
  pBIMCResource->nSavLevel = pBIMCResource->nCurLevel;
  if ( pBIMCResource->pClock->pDomain->pBSPConfig[pBIMCResource->nCurLevel].nFreqHz/1000 != nSlowestBIMCSpeed )
  {
    Clock_BIMCSetFrequency( pBIMCResource, nSlowestBIMCSpeed );
  }

  /* Enable DEHR clock */
  Clock_EnableClock( nClockDehr );

  mba_init = message_ram_vmpm_isMBADone();
  
  /* If BIMC in power collapsed mode */
  if ( (mba_init == TRUE) && (bBIMCPC == TRUE )&& HAL_clk_IsDEHREnable())
  {
    /* Put DDR to self-refresh and freeze IO */
    ddr_enter_power_collapse( nSlowestBIMCSpeed  );

    /* Debug variable count the no. of Collapses*/
    clk_dbg_bimc_pc_count++;

    /* Disable BIMC power domain for BIMC collapse */
    Clock_DisablePowerDomain( nBIMCPowerDomainID );
  }
  else
  {
    /* Put DDR in self-refresh mode */
    ddr_pre_xo_shutdown( nSlowestBIMCSpeed  );
  }
}


/* =========================================================================
**  Function : Clock_WaitForBIMCPowerRestore
** =========================================================================*/
/**
  Wait for BIMC power restore success then enable BIMC back to previous speed,
  and then call DDR to exit self-refresh mode.
  
  @param 
      None

  @return
      None

  @dependencies
      None

*/
void Clock_WaitForBIMCPowerRestored( void )
{
  ClockResourceType      *pBIMCResource;
  Clock_NPAResourcesType *pNPAResources;
  uint32                  nSlowestBIMCSpeed;
  boolean                 mba_init = FALSE;
  
  pNPAResources = Clock_GetNPAResources();

  /* Use referent pointer for shorten name */
  pBIMCResource = &(pNPAResources->BIMCClockResource);
  
  nSlowestBIMCSpeed = pBIMCResource->pClock->pDomain->pBSPConfig[0].nFreqHz/1000;

  mba_init = message_ram_vmpm_isMBADone();

  if ( (mba_init == TRUE) && (bBIMCPC == TRUE )&& HAL_clk_IsDEHREnable())
  {
    /* Poll and wait until BIMC power collapsed and DEHR restored done */
    while ( Clock_IsPowerDomainEnabled(nBIMCPowerDomainID) == FALSE );
    while ( (HAL_clk_GetDEHRStatus() & DEHR_RESTORE_DONE) != DEHR_RESTORE_DONE );

    /* Disable DEHR clock */
    Clock_DisableClock( nClockDehr );

    /* Debug variable to count num of BIMC restores */
    clk_dbg_bimc_pc_restore_count++;

    /* Call DDR to for exit power collapse */
    ddr_exit_power_collapse( nSlowestBIMCSpeed );
  }
  else
  {
    /* Disable DEHR clock */
    Clock_DisableClock( nClockDehr );
    /* Take DDR out of self-refresh */
    ddr_post_xo_shutdown( pBIMCResource->pClock->pDomain->pBSPConfig[pBIMCResource->nSavLevel].nFreqHz/1000 );
  }

  /* Restore clock to previous state */
  Clock_BIMCSetFrequency( pBIMCResource, 
                          pBIMCResource->pClock->pDomain->pBSPConfig[pBIMCResource->nSavLevel].nFreqHz/1000 );
}


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
  boolean                 mba_init = FALSE;
  
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
      QDSSDDRTraceDisable();
    }

    bBIMCPC = (nFlags == 2);
    Clock_BIMCPowerCollapsed();
  }
  else
  {

    /*Turn off misc clocks that are not required*/
//    HAL_clk_MiscClkDisableDeepSleep();   


    /* Collapse all non-BIMC clocks to XO speed */
    if ( pNPAResources->SNOCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->SNOCClockResource.nSavLevel = pNPAResources->SNOCClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->SNOCClockResource.pClock->pDomain, 
                            &pNPAResources->SNOCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->SysMMNOCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->SysMMNOCClockResource.nSavLevel = pNPAResources->SysMMNOCClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->SysMMNOCClockResource.pClock->pDomain, 
                            &pNPAResources->SysMMNOCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
      
      Clock_DisableClock( (ClockIdType)pNPAResources->SysMMNOCClockResource.pClock );
      Clock_DisableClock( nClockBIMCSysMMNOCAXI );
    }

    if ( pNPAResources->APSSTCUASYNCClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->APSSTCUASYNCClockResource.nSavLevel = pNPAResources->APSSTCUASYNCClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->APSSTCUASYNCClockResource.pClock->pDomain, 
                            &pNPAResources->APSSTCUASYNCClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->Q6TBUClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->Q6TBUClockResource.nSavLevel = pNPAResources->Q6TBUClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->Q6TBUClockResource.pClock->pDomain, 
                            &pNPAResources->Q6TBUClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->BIMCGPUClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->BIMCGPUClockResource.nSavLevel = pNPAResources->BIMCGPUClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->BIMCGPUClockResource.pClock->pDomain, 
                            &pNPAResources->BIMCGPUClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    if ( pNPAResources->APSSAXIClockResource.nCurLevel != XO_LEVEL )
    {
      pNPAResources->APSSAXIClockResource.nSavLevel = pNPAResources->APSSAXIClockResource.nCurLevel;
      Clock_SetClockConfig( pNPAResources->APSSAXIClockResource.pClock->pDomain, 
                            &pNPAResources->APSSAXIClockResource.pClock->pDomain->pBSPConfig[XO_LEVEL] );
    }

    
     mba_init = message_ram_vmpm_isMBADone();
    /* BIMC Power collapsed enable.  Wait until BIMC collapsed done */
    if ((mba_init == TRUE) && (bBIMCPC == TRUE )&& HAL_clk_IsDEHREnable())
    {
      /* Poll and wait until BIMC collapsed complete before turn off the rest of the clocks */
      while ( Clock_IsPowerDomainEnabled(nBIMCPowerDomainID) == TRUE );
      while ( (HAL_clk_GetDEHRStatus() & DEHR_SAVE_DONE) != DEHR_SAVE_DONE );      
    }

    /* Disable DEHR clock */
    Clock_DisableClock( nClockDehr );

    /* Disable BIMC clock */
    Clock_DisableClock( (ClockIdType)pNPAResources->BIMCClockResource.pClock );

    /* Collapse all non-BIMC clocks to XO speed
    * PCNOC Should be done after BIMC as RPM is running OFF PCNOC
    */

     /* Turn clocks to RAMs off */
    /* Check if this is required??*/
    Clock_EnableClkVddMin( FALSE );

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

    /*
    * On 8x26/8x10 as A7 used to keep its vote ON for GPLL0 in APPS power collapse 
    * and hence this was required to reset FSM for GPLL0 during deep sleep. 
    * We need to find out if A53 has the same behaviour??
    */
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
  Clock_NPAResourcesType  *pNPAResources;
  ClockDrvCtxt        *pDrvCtxt = Clock_GetDrvCtxt();
  boolean             mba_init = FALSE;
  if (!pDrvCtxt)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Device Contex pointer NULL.");
    return DAL_ERROR;
  }

  /* Don't allow voltage change during processor restore */
  Clock_VoltageSuspend( pDrvCtxt, TRUE);

  pNPAResources = Clock_GetNPAResources();

  /* BIMC restored. */
  if ( nFlags != 0 )
  {
    Clock_WaitForBIMCPowerRestored();
    
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
    
    /* Turn clocks to RAMs On */
    /* Check if this is required? */
    Clock_EnableClkVddMin( TRUE );
  
    if ( pNPAResources->CPUClockResource.nCurLevel != XO_LEVEL )
    {
       // Need to restore CPU clock up first for higher performance
       Clock_SetClockConfig( pNPAResources->CPUClockResource.pClock->pDomain, 
                             &pNPAResources->CPUClockResource.pClock->pDomain->pBSPConfig[pNPAResources->CPUClockResource.nSavLevel] );
    }
    if ( pNPAResources->PCNOCClockResource.nCurLevel != XO_LEVEL )
    {
       /* Need to restore PCNOC before BIMC as RPM runs OFF PCNOC */
       Clock_SetClockConfig( pNPAResources->PCNOCClockResource.pClock->pDomain, 
                             &pNPAResources->PCNOCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->PCNOCClockResource.nSavLevel] );
    } 

    /* Enable BIMC clock */
    Clock_EnableClock( (ClockIdType)pNPAResources->BIMCClockResource.pClock );

    if ( pNPAResources->SNOCClockResource.nCurLevel != XO_LEVEL )
    {
      /* Switch the rest to previous clock speed */
      Clock_SetClockConfig( pNPAResources->SNOCClockResource.pClock->pDomain, 
                            &pNPAResources->SNOCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->SNOCClockResource.nSavLevel] );
    }

    if ( pNPAResources->SysMMNOCClockResource.nCurLevel != XO_LEVEL )
    {
      /* Switch the rest to previous clock speed */
      Clock_SetClockConfig( pNPAResources->SysMMNOCClockResource.pClock->pDomain, 
                            &pNPAResources->SysMMNOCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->SysMMNOCClockResource.nSavLevel] );
      Clock_EnableClock( (ClockIdType)pNPAResources->SysMMNOCClockResource.pClock );
      Clock_EnableClock( nClockBIMCSysMMNOCAXI );
    }

   if ( pNPAResources->APSSTCUASYNCClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfig( pNPAResources->APSSTCUASYNCClockResource.pClock->pDomain, 
                            &pNPAResources->APSSTCUASYNCClockResource.pClock->pDomain->pBSPConfig[pNPAResources->APSSTCUASYNCClockResource.nSavLevel] );
    }

   if ( pNPAResources->Q6TBUClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfig( pNPAResources->Q6TBUClockResource.pClock->pDomain, 
                            &pNPAResources->Q6TBUClockResource.pClock->pDomain->pBSPConfig[pNPAResources->Q6TBUClockResource.nSavLevel] );
    }

   if ( pNPAResources->BIMCGPUClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfig( pNPAResources->BIMCGPUClockResource.pClock->pDomain, 
                            &pNPAResources->BIMCGPUClockResource.pClock->pDomain->pBSPConfig[pNPAResources->BIMCGPUClockResource.nSavLevel] );
    }

     if ( pNPAResources->APSSAXIClockResource.nCurLevel != XO_LEVEL )
    {
       Clock_SetClockConfig( pNPAResources->APSSAXIClockResource.pClock->pDomain, 
                            &pNPAResources->APSSAXIClockResource.pClock->pDomain->pBSPConfig[pNPAResources->APSSAXIClockResource.nSavLevel] );
    }
    

   /*Turn misc clocks on*/ 
//    HAL_clk_MiscClkRestoreDuringWakeup();

    /* Enable DEHR clock */
     Clock_EnableClock( nClockDehr );

     mba_init = message_ram_vmpm_isMBADone();

     if ((mba_init == TRUE) && (bBIMCPC == TRUE )&& HAL_clk_IsDEHREnable())   
     {   
       /* Enable BIMC power domain for BIMC collapse */
       Clock_EnablePowerDomain( nBIMCPowerDomainID, TRUE ); 
     }
  }

  /* 
   * Allow voltage change.
   */
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
  return (Clock_DriverInit() == DAL_SUCCESS) ? TRUE : FALSE;
} /* END Clock_Init */


/* =========================================================================
**  Function : Clock_BIMCGetMiscInfo
** =========================================================================*/
/**
  Get GCC_BIMC_MISC register info

  @param
    None

  @return
    Return GCC_BIMC_MISC info

  @dependencies
    None

  @sa
    None
*/

uint32 Clock_BIMCGetMiscInfo( void )
{
  return HAL_clk_BIMCGetMiscInfo();
}
