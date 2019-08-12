
/*
===========================================================================
*/
/**
  @file ClockMSS.c 
  
  Main entry point for the MSS clock driver.
*/
/*  
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/hw/msm8909/mss/src/ClockMSS.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  10/19/11   vs      Created.
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "ClockDriver.h"
#include "ClockMSS.h"
#include "ClockSWEVT.h"
#include <CoreIni.h>
#include "DDIVCS.h"
#include "npa_resource.h"
#include "ClockMSSHWIO.h"


/*=========================================================================
      Macros
==========================================================================*/


/*
 * Clock ID's
 */
#define CLOCK_ID_CPU                        "clk_q6"
#define CLOCK_ID_MSS_CONFIG_BUS             "clk_mss_config_bus"
#define CLOCK_ID_CRYPTO                     "clk_bus_crypto"

#define Q6_FMAX_FUSE_SHIFT                   16
#define Q6_FMAX_FUSE_MASK		     0x7
/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Extern Definitions
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/
static const char *ClockNPANodeName[] =
{
  CLOCK_NPA_NODE_NAME_CRYPTO,
  CLOCK_NPA_NODE_NAME_CONFIG_BUS,
  CLOCK_NPA_NODE_NAME_CPU,
  CLOCK_NPA_NODE_NAME_CPU_TEST,
  CLOCK_NPA_NODE_NAME_PCNOC,
  CLOCK_NPA_NODE_NAME_CXO,
  CLOCK_NPA_NODE_NAME_QDSS,
  CLOCK_NPA_NODE_NAME_SNOC,
  CLOCK_NPA_NODE_NAME_BIMC
};


static ClockImageCtxtType Clock_ImageCtxt;


/* =========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_DetectPVSData
** =========================================================================*/
/**
  Detects efuse settings and keeps a handle to the appropriate PVS data.
 
  This function is invoked at driver initialization to detect efuse
  settings to distinguish b/w slow, nominal and fast part.
 
  @param *pDrvCtxt [in] -- Pointer to driver context.

  @return
  DAL_ERROR if detection fails, other DAL_SUCCESS.

  @dependencies
  None.
*/ 

static DALResult Clock_DetectPVSData
(
  ClockDrvCtxt *pDrvCtxt
)
{

  /*-----------------------------------------------------------------------*/
  /* Nothing needed here - VRegCornerData is part of BSP Config.           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : Clock_DetectBSPVersion
** =========================================================================*/
/**
  Detects which BSP configuration to use for the current HW.

  @param *pDrvCtxt [in] -- Pointer to driver context.

  @return
  DAL_ERROR if a valid configuration was not found, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult Clock_DetectBSPVersion
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockImageCtxtType     *pImageCtxt;
  ClockCPUPerfConfigType *pCPUPerfConfig;
  uint32                  i;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Detect which CPU BSP data to use for this HW version.                 */
  /*-----------------------------------------------------------------------*/

  pCPUPerfConfig = pImageCtxt->pBSPConfig->pCPUPerfConfig;
  for (i = 0; i < pImageCtxt->pBSPConfig->nNumCPUPerfLevelConfigs; i++)
  {
    if( (Clock_IsBSPSupported(&pCPUPerfConfig[i].HWVersion) == TRUE) && (Clock_IsSpeedBinSupported(pCPUPerfConfig[i].nSpeedBinFuse) == TRUE))
    {
      pImageCtxt->CPUCtxt.PerfConfig.HWVersion = pCPUPerfConfig[i].HWVersion;
      pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel = pCPUPerfConfig[i].anPerfLevel;
      pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel = pCPUPerfConfig[i].nMaxPerfLevel;
      pImageCtxt->CPUCtxt.PerfConfig.nMinPerfLevel = pCPUPerfConfig[i].nMinPerfLevel;
      pImageCtxt->CPUCtxt.PerfConfig.nNumPerfLevels = pCPUPerfConfig[i].nNumPerfLevels;
      pImageCtxt->CPUCtxt.PerfConfig.nSpeedBinFuse = pCPUPerfConfig[i].nSpeedBinFuse;

        break;
    }
  }

  if (i == pImageCtxt->pBSPConfig->nNumCPUPerfLevelConfigs)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END of Clock_DetectBSPVersion */


/* =========================================================================
**  Function : Clock_InitCPUConfig
** =========================================================================*/
/**
  Initializes current configuration of CPU clock
 
  This function is invoked at driver initialization to initialize the current
  CPU configuration.
 
  @param *pDrvCtxt [in] -- Pointer to driver context.

  @return
  DAL_ERROR if configuration was not valid, other DAL_SUCCESS.

  @dependencies
  None.
*/ 

static DALResult Clock_InitCPUConfig
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult            eRes;
  ClockNodeType       *pClock;
  ClockSourceNodeType *pSource;
  ClockCPUConfigType  *pConfig;
  ClockImageCtxtType  *pImageCtxt;
  uint32               nSourceIndex;
  uint32               nPL, nConfig;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Get CPU clock ID.                                                     */
  /*-----------------------------------------------------------------------*/

  eRes =
    Clock_GetClockId(
      pDrvCtxt, CLOCK_ID_CPU,
      (ClockIdType *)&pImageCtxt->CPUCtxt.pClock);
  if (eRes != DAL_SUCCESS)
  {
    return eRes;
  }
  
  pClock = pImageCtxt->CPUCtxt.pClock;

  /*-----------------------------------------------------------------------*/
  /* Find the max performance level.                                       */
  /*-----------------------------------------------------------------------*/

  nPL = pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel;
  nConfig = pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel[nPL];
  pConfig = &pImageCtxt->pBSPConfig->pCPUConfig[nConfig];

  /*-----------------------------------------------------------------------*/
  /* Configure the CPU to the max performance level.                       */
  /*-----------------------------------------------------------------------*/

  HAL_clk_ConfigClockMux(pClock->pDomain->HALHandle, &pConfig->Mux.HALConfig);

  /*-----------------------------------------------------------------------*/
  /* Update state.                                                         */
  /*-----------------------------------------------------------------------*/

  nSourceIndex = pDrvCtxt->anSourceIndex[pConfig->Mux.HALConfig.eSource];
  if (nSourceIndex == 0xFF)
  {
    return DAL_ERROR;
  }

  pSource = &pDrvCtxt->aSources[nSourceIndex];
  if (pSource == NULL)
  {
    return DAL_ERROR;
  }

  pClock->pDomain->pSource = pSource;
  pImageCtxt->CPUCtxt.pConfig = pConfig;
  pClock->pDomain->pActiveMuxConfig = &pConfig->Mux;

  /*-----------------------------------------------------------------------*/
  /* Ensure that the CPU core clock/domain/source reference counts are 1.  */
  /*-----------------------------------------------------------------------*/

  Clock_EnableClock(pDrvCtxt, (ClockIdType)pImageCtxt->CPUCtxt.pClock);

  /*-----------------------------------------------------------------------*/
  /* Initialize the DCVS module.                                           */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_InitDCVS(pDrvCtxt);
  if (eRes != DAL_SUCCESS)
  {
    return eRes;
  }

  return DAL_SUCCESS;

} /* END Clock_InitCPUConfig */


/* =========================================================================
**  Function : Clock_InitMSSConfigBusConfig
** =========================================================================*/
/**
  Initializes current configuration of the MSS config bus clock

  This function is invoked at driver initialization to init the current
  MSS config bus configuration.

  @param *pDrvCtxt [in] -- Pointer to driver context.

  @return
  DAL_ERROR if configuration was not valid, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult Clock_InitMSSConfigBusConfig
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult                 eRes;
  ClockNodeType                *pClock;
  ClockImageCtxtType       *pImageCtxt;
  uint32                    nCfg, nDiv2x, nSrcIdx, nPL;

  ClockConfigBusPerfConfigType *pPerfConfig;
  HAL_clk_ClockMuxConfigType    ActiveMuxCfg;
  ClockSourceNodeType          *pSourceDetected;
  HAL_clk_SourceType            eSourceDetected;
  ClockConfigBusConfigType     *pPerfLevelConfig, *pPerfLevelConfigMatch;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Get MSS Config Bus clock ID.                                          */
  /*-----------------------------------------------------------------------*/

  eRes =
    Clock_GetClockId(
      pDrvCtxt, CLOCK_ID_MSS_CONFIG_BUS,
      (ClockIdType *)&pImageCtxt->ConfigBusCtxt.pClock);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get MSS config bus clock ID.");
    return eRes;
  }

  pPerfConfig = pImageCtxt->pBSPConfig->pConfigBusPerfConfig;

  /*-----------------------------------------------------------------------*/
  /* Get proper clock and configuration data                               */
  /*-----------------------------------------------------------------------*/

  pClock = pImageCtxt->ConfigBusCtxt.pClock;

  /*-----------------------------------------------------------------------*/
  /* Thread safety.                                                        */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Get the current hardware configuration.                               */
  /*-----------------------------------------------------------------------*/

  HAL_clk_DetectClockMuxConfig(pClock->pDomain->HALHandle, &ActiveMuxCfg);

  /*
   * If we got back divider of 0, set to 2 for comparison.
   */
  if (ActiveMuxCfg.nDiv2x == 0)
  {
    ActiveMuxCfg.nDiv2x = 2;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate we have BSP data for the detected source.                    */
  /*-----------------------------------------------------------------------*/

  eSourceDetected = ActiveMuxCfg.eSource;
  nSrcIdx = pDrvCtxt->anSourceIndex[eSourceDetected];
  if (nSrcIdx == 0xFF)
  {
    DALCLOCK_FREE(pDrvCtxt);

    return DAL_ERROR;
  }

  pSourceDetected = &pDrvCtxt->aSources[nSrcIdx];

  if (pSourceDetected == NULL ||
      pSourceDetected->pBSPConfig == NULL ||
      pSourceDetected->pBSPConfig->pSourceFreqConfig == NULL)
  {
    DALCLOCK_FREE(pDrvCtxt);

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Go through config array to find a matching configuration.             */
  /*-----------------------------------------------------------------------*/

  pPerfLevelConfigMatch = NULL;
  for (nPL = pPerfConfig->nMinPerfLevel;
       nPL <= pPerfConfig->nMaxPerfLevel;
       nPL++)
  {
    nCfg = pPerfConfig->anPerfLevel[nPL];
    pPerfLevelConfig = &pImageCtxt->pBSPConfig->pConfigBusConfig[nCfg];

    nDiv2x = pPerfLevelConfig->Mux.HALConfig.nDiv2x;
    if (nDiv2x == 0)
    {
        nDiv2x = 2;
    }

    if (pPerfLevelConfig->Mux.HALConfig.eSource == eSourceDetected &&
        nDiv2x == ActiveMuxCfg.nDiv2x)
    {
      pPerfLevelConfigMatch = pPerfLevelConfig;
      break;
    }
  }

  if (pPerfLevelConfigMatch == NULL)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to enable the MSS config bus clock.");
    DALCLOCK_FREE(pDrvCtxt);
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Update the clock node with the matched configuration.                 */
  /*-----------------------------------------------------------------------*/
  pImageCtxt->ConfigBusCtxt.pConfig = pPerfLevelConfigMatch;
  pClock->pDomain->pActiveMuxConfig = &pPerfLevelConfigMatch->Mux;
  pClock->pDomain->pSource = pSourceDetected;

  /*-----------------------------------------------------------------------*/
  /* Ensure that MSS config bus clock/domain/source reference counts are 1.*/
  /*-----------------------------------------------------------------------*/

  eRes =
    Clock_EnableClock(
      pDrvCtxt, (ClockIdType)pImageCtxt->ConfigBusCtxt.pClock);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to enable the MSS config bus clock.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize MSS Config Bus module.                                     */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_InitConfigBus(pDrvCtxt);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to init the MSS config bus clock NPA node.");
     DALCLOCK_FREE(pDrvCtxt);
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Free.                                                                 */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_InitMSSConfigBusConfig */


/* =========================================================================
**  Function : Clock_InitImage
** =========================================================================*/
/*
  See ClockDriver.h
*/ 

DALResult Clock_InitImage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult              eRes;
  ClockPropertyValueType PropVal;

  /*-----------------------------------------------------------------------*/
  /* Assign the image context.                                             */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->pImageCtxt = &Clock_ImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Initialize the SW Events for Clocks.                                  */
  /*-----------------------------------------------------------------------*/

  Clock_SWEvent(CLOCK_EVENT_INIT, 0);

  /*-----------------------------------------------------------------------*/
  /* Attach to the VCS DAL.                                                */
  /*-----------------------------------------------------------------------*/

  eRes = DAL_StringDeviceAttach("VCS", &Clock_ImageCtxt.hVCS);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to attach to VCS DAL: %d", eRes);
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the CPU BSP Configuration.                                        */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_GetPropertyValue("ClockImageBSP", &PropVal);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
      "Unable to get Clock BSP Property: ClockImageBSP.");
    return eRes;
  }

  Clock_ImageCtxt.pBSPConfig = (ClockImageBSPConfigType *)PropVal;

  /*-----------------------------------------------------------------------*/
  /* Detect the BSP version to use.                                        */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_DetectBSPVersion(pDrvCtxt);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Failed to detect the BSP version.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Detect CPU efuse settings and update voltage data depending on slow,  */
  /* nominal or fast part.                                                 */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_DetectPVSData(pDrvCtxt);
  if (eRes != DAL_SUCCESS) 
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
      "Unable to detect CPU efuse settings - assuming SLOW part.");
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the XO module.                                             */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_InitXO(pDrvCtxt);
  if (eRes != DAL_SUCCESS) 
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to init XO.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the modem PLL resources.                                   */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_InitPLL(pDrvCtxt);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to init the PLL.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* As of now for 8909, we are configuring GPLL0/1 on modem. Later we will*/
  /* to boot once we start working on 8909.                                */
  /*-----------------------------------------------------------------------*/
  Clock_InitSource(pDrvCtxt, HAL_CLK_SOURCE_GPLL0, NULL);
  Clock_InitSource(pDrvCtxt, HAL_CLK_SOURCE_GPLL1, NULL);

  /*-----------------------------------------------------------------------*/
  /* Initialize CPU core clock frequency configuration.                    */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_InitCPUConfig(pDrvCtxt);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to detect CPU core clock configuration.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize MSS config bus clock frequency configuration.              */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_InitMSSConfigBusConfig(pDrvCtxt);
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to detect MSS config bus clock configuration.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Get Crypto clock ID.                                                  */
  /*-----------------------------------------------------------------------*/

  eRes =
    Clock_GetClockId(
      pDrvCtxt, CLOCK_ID_CRYPTO,
      (ClockIdType *)&Clock_ImageCtxt.ConfigBusCtxt.pCryptoClock);

  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to get Crypto clock ID.");
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Run the NPA BIST (if enabled) for Q6 & bus clocks switching           */
  /*-----------------------------------------------------------------------*/

  if (Clock_ImageCtxt.CPUCtxt.bNPABISTEnabled  || 
      Clock_ImageCtxt.ConfigBusCtxt.bNPABISTEnabled)
  {
    if (Clock_ImageCtxt.nNPABISTLogSize == 0)
    {
      Clock_ImageCtxt.nNPABISTLogSize = CLOCK_NPA_BIST_DEFAULT_LOG_SIZE;
    }
    Clock_NPA_BIST(pDrvCtxt);
  }
  
  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitImage */


/* =========================================================================
**  Function : ClockStub_InitImage
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult ClockStub_InitImage
(
  ClockDrvCtxt *pDrvCtxt
)
{
  uint32 i;

  /*-----------------------------------------------------------------------*/
  /* Initialize the SW Events for Clocks.                                  */
  /*-----------------------------------------------------------------------*/

  Clock_SWEvent(CLOCK_EVENT_INIT, 0);

  /*-----------------------------------------------------------------------*/
  /* Create stubs for each NPA node.                                       */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARRAY_SIZE(ClockNPANodeName); i++)
  {
    npa_stub_resource(ClockNPANodeName[i]);
  }

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitImage */


/* =========================================================================
**  Function : Clock_ProcessorSleep
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ProcessorSleep
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockSleepModeType  eMode,
  uint32              nFlags
)
{
  /*-----------------------------------------------------------------------*/
  /* There are currently no use cases for calling this API.  We'll trigger */
  /* an error to ensure it isn't called by mistake.                        */
  /*-----------------------------------------------------------------------*/

  DALSYS_LogEvent(
    DALDEVICEID_CLOCK,
    DALSYS_LOGEVENT_FATAL_ERROR,
    "Clock_ProcessorSleep API is currently unsupported.");

  return DAL_ERROR_NOT_SUPPORTED;

} /* END Clock_ProcessorSleep */


/* =========================================================================
**  Function : Clock_ProcessorRestore
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ProcessorRestore
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockSleepModeType  eMode,
  uint32              nFlags
) 
{
  /*-----------------------------------------------------------------------*/
  /* There are currently no use cases for calling this API.  We'll trigger */
  /* an error to ensure it isn't called by mistake.                        */
  /*-----------------------------------------------------------------------*/

  DALSYS_LogEvent(
    DALDEVICEID_CLOCK,
    DALSYS_LOGEVENT_FATAL_ERROR,
    "Clock_ProcessorRestore API is currently unsupported.");

  return DAL_ERROR_NOT_SUPPORTED;
  
} /* END Clock_ProcessorRestore */


/* =========================================================================
**  Function : Clock_LoadNV
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_LoadNV
(
  ClockDrvCtxt *pDrvCtxt
)
{
  CoreConfigHandle    hConfig;
  uint32              nReadResult, nData;
  DALResult           eResult;

  /*-----------------------------------------------------------------------*/
  /* Read clock configuration file.                                        */
  /*-----------------------------------------------------------------------*/

  hConfig = CoreIni_ConfigCreate(CLOCK_EFS_INI_FILENAME);
  if (hConfig == NULL)
  { 
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_INFO,
      "Unable to read EFS file: %s",
      CLOCK_EFS_INI_FILENAME);
  }

  /*-----------------------------------------------------------------------*/
  /* Update global flags.                                                  */
  /*-----------------------------------------------------------------------*/

  if (hConfig)
  {
    nReadResult =
      CoreConfig_ReadUint32(
        hConfig,
        CLOCK_EFS_MPSS_CONFIG_SECTION,
        CLOCK_EFS_MPSS_CLOCK_FLAGS_FLAG,
        (unsigned int *)&nData);
    if (nReadResult == CORE_CONFIG_SUCCESS)
    {
      pDrvCtxt->nGlobalFlags = nData;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update DCVS data.                                                     */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_LoadNV_DCVS(pDrvCtxt, hConfig);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to load DCVS EFS");
  }

  /*-----------------------------------------------------------------------*/
  /* Destroy the handle.                                                   */
  /*-----------------------------------------------------------------------*/

  if (hConfig)
  {
    CoreIni_ConfigDestroy(hConfig);
  }

  return eResult;

} /* END Clock_LoadNV */


/* =========================================================================
**  Function : Clock_ImageBIST
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_ImageBIST
(
  ClockDrvCtxt  *pDrvCtxt,
  boolean       *bBISTPassed,
  uint32        *nFailedTests
)
{

  /*
   * Nothing to do yet.
   */
  return DAL_SUCCESS;

} /* END Clock_ImageBIST */

/* =========================================================================
**  Function : Clock_IsSpeedBinSupported
** =========================================================================*/

boolean  Clock_IsSpeedBinSupported
(
 uint32 nSpeedBinFuse
)
{
  uint32  nPVS, CurrentSpeedBin;
   
  if (nSpeedBinFuse == 0xFF) return TRUE;

  nPVS = HWIO_IN(QFPROM_CORR_PTE1);
  CurrentSpeedBin = ((nPVS >> Q6_FMAX_FUSE_SHIFT) & Q6_FMAX_FUSE_MASK);
  if(CurrentSpeedBin == nSpeedBinFuse)
  {
    return TRUE;
  }
  else 
  {
    return FALSE;
  }
}

/* =========================================================================
**  Function : Clock_NPA_BIST
** =========================================================================*/
/**
  See ClockMSS.h

*/

void Clock_NPA_BIST(ClockDrvCtxt *pDrvCtxt)
{
  uint32                        i, nFreqHz_NPA_state, nFreqHz_Calc, nDisableDCS, nCfg;
  npa_query_type                qdsp6_state, config_bus_state, test_state;
  npa_client_handle             qdsp6_h, bus_h;
  DALResult                     eResult;
  ClockImageCtxtType           *pImageCtxt;
  ClockCPUPerfConfigType       *pCPUPerfConfig;
  ClockConfigBusPerfConfigType *pConfigBusPerfConfig;
  ClockCPUConfigType           *pCPUConfig;
  ClockConfigBusConfigType     *pConfigBusConfig;
  
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt; 
  pCPUPerfConfig = pImageCtxt->pBSPConfig->pCPUPerfConfig;
  pConfigBusPerfConfig = pImageCtxt->pBSPConfig->pConfigBusPerfConfig;
  pCPUConfig = pImageCtxt->pBSPConfig->pCPUConfig;
  pConfigBusConfig = pImageCtxt->pBSPConfig->pConfigBusConfig;
  
  /*-----------------------------------------------------------------------*/
  /* Initialize the NPA BIST log.                                          */
  /*-----------------------------------------------------------------------*/

  ULogFront_RealTimeInit (&pImageCtxt->hClockNPABISTLog, "Clock NPA BIST Log",
                          pImageCtxt->nNPABISTLogSize,
                          ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
  ULogCore_HeaderSet     (pImageCtxt->hClockNPABISTLog,
                          "Content-Type: text/tagged-log-1.0;");

  /*-----------------------------------------------------------------------*/
  /* Test clocks through NPA                                               */
  /*-----------------------------------------------------------------------*/
   
  if (pImageCtxt->CPUCtxt.bNPABISTEnabled)
  {
    /* Enable DCS after saving current nDisableDCS flag */
    nDisableDCS = pImageCtxt->CPUCtxt.nDisableDCS;
    pImageCtxt->CPUCtxt.nDisableDCS = 0;
    
    qdsp6_h = npa_create_sync_client(CLOCK_NPA_NODE_NAME_CPU, 
                                     "QDSP6_BIST", NPA_CLIENT_SUPPRESSIBLE);
                                  
    /* Save current Q6 state to restore later */
    npa_query_by_name(CLOCK_NPA_NODE_NAME_CPU, NPA_QUERY_CURRENT_STATE, 
                      &qdsp6_state);

    for(i = pCPUPerfConfig->nMinPerfLevel; i <= pCPUPerfConfig->nMaxPerfLevel; i++)
    {
      nCfg = pCPUPerfConfig->anPerfLevel[i];
      ULOG_RT_PRINTF_1 (pImageCtxt->hClockNPABISTLog, 
                        "Testing Q6 clock for frequency: %d Hz", 
                        pCPUConfig[nCfg].Mux.nFreqHz);

      npa_issue_scalar_request( qdsp6_h, pCPUConfig[nCfg].Mux.nFreqHz/1000 );
      npa_query_by_name(CLOCK_NPA_NODE_NAME_CPU, NPA_QUERY_CURRENT_STATE, 
                        &test_state);
      nFreqHz_NPA_state = NPA_TO_HZ(test_state.data.state);
      
      ULOG_RT_PRINTF_1 (pImageCtxt->hClockNPABISTLog, 
                            "Q6 resource state frequency   : %d Hz", 
                            nFreqHz_NPA_state);

      eResult = Clock_CalcClockFrequency(pDrvCtxt, 
                                         (ClockIdType) pImageCtxt->CPUCtxt.pClock, 
                                         &nFreqHz_Calc);
      
      if (DAL_SUCCESS == eResult)
      {
        if (ABS_DIFF(nFreqHz_NPA_state, nFreqHz_Calc) > 
            (CLOCK_NPA_BIST_ERROR_MARGIN_PERCENT/100)*nFreqHz_NPA_state)
        {
          ULOG_RT_PRINTF_1 (pImageCtxt->hClockNPABISTLog, 
                            "Q6 calculated frequency       : %d Hz", 
                            nFreqHz_Calc);
        }
        else
        {
          ULOG_RT_PRINTF_0 (pImageCtxt->hClockNPABISTLog, 
                            "Q6 CPU clock set frequency: SUCCESS");
        }
      }
      else
      {
        ULOG_RT_PRINTF_0 (pImageCtxt->hClockNPABISTLog, 
                          "Clock_CalcClockFrequency failed for Q6 clock");
      }
    }
    /* Restore Q6 state & nDisableDCS flag*/
    npa_issue_scalar_request( qdsp6_h, qdsp6_state.data.state );
    pImageCtxt->CPUCtxt.nDisableDCS = nDisableDCS;
  }
  
  if (pImageCtxt->ConfigBusCtxt.bNPABISTEnabled)
  {
    bus_h = npa_create_sync_client(CLOCK_NPA_NODE_NAME_CONFIG_BUS, 
                                   "MSS_CFG_BUS_BIST", NPA_CLIENT_SUPPRESSIBLE);
    
    /* Save current bus state to restore later */
    npa_query_by_name(CLOCK_NPA_NODE_NAME_CONFIG_BUS, NPA_QUERY_CURRENT_STATE, 
                      &config_bus_state);
                      
    for(i = pConfigBusPerfConfig->nMinPerfLevel; i <= pConfigBusPerfConfig->nMaxPerfLevel; i++)
    { 
      nCfg = pConfigBusPerfConfig->anPerfLevel[i];
      ULOG_RT_PRINTF_1 (pImageCtxt->hClockNPABISTLog, 
                        "Testing bus clock for frequency: %d Hz", 
                        pConfigBusConfig[nCfg].Mux.nFreqHz);

      npa_issue_scalar_request( bus_h, pConfigBusConfig[nCfg].Mux.nFreqHz/1000 );
      npa_query_by_name(CLOCK_NPA_NODE_NAME_CONFIG_BUS, NPA_QUERY_CURRENT_STATE, 
                        &test_state);
      nFreqHz_NPA_state = NPA_TO_HZ(test_state.data.state);
      
      ULOG_RT_PRINTF_1 (pImageCtxt->hClockNPABISTLog, 
                        "Bus resource state frequency   : %d Hz", 
                        nFreqHz_NPA_state);

      eResult = Clock_CalcClockFrequency(pDrvCtxt, 
                                         (ClockIdType) pImageCtxt->ConfigBusCtxt.pClock, 
                                         &nFreqHz_Calc);
      
      if (DAL_SUCCESS == eResult)
      {
        if (ABS_DIFF(nFreqHz_NPA_state, nFreqHz_Calc) > 
            (CLOCK_NPA_BIST_ERROR_MARGIN_PERCENT/100)*nFreqHz_NPA_state)
        {
          ULOG_RT_PRINTF_1 (pImageCtxt->hClockNPABISTLog, 
                            "Bus calculated frequency       : %d Hz", 
                            nFreqHz_Calc);
        }
        else
        {
          ULOG_RT_PRINTF_0 (pImageCtxt->hClockNPABISTLog, 
                            "Bus clock set frequency: SUCCESS");
        }
      }
      else
      {
        ULOG_RT_PRINTF_0 (pImageCtxt->hClockNPABISTLog, 
                          "Clock_CalcClockFrequency failed for bus clock");
      }
    }
    /* Restore bus state */
    npa_issue_scalar_request( bus_h, config_bus_state.data.state );
  }
  
  return;
  
} /* END Clock_NPA_BIST */
