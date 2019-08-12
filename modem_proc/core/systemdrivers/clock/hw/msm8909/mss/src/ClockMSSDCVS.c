/*
===========================================================================
*/
/**
  @file ClockMSSDCVS.c 
  
  DCVS NPA node definitions for the MSS clock driver.
*/
/*  
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/hw/msm8909/mss/src/ClockMSSDCVS.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  02/17/11   vs      Created.
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "DDIClock.h"
#include "ClockDriver.h"
#include "ClockMSS.h"
#include "ClockSWEVT.h"
#include "ClockMSSHWIO.h"
#include "DALDeviceId.h"

#include <DALSys.h>
#include <npa.h>
#include <npa_resource.h>
#include <npa_remote.h>
#include <npa_remote_resource.h>
#include "pmapp_npa.h"
#include "rcecb.h"
#include "rcinit.h"
#include "DDIVCS.h"

/*=========================================================================
      Externs
==========================================================================*/


extern void npa_update_resource_state(npa_resource *resource, npa_resource_state new_state);


/*=========================================================================
      Macros
==========================================================================*/


/*
 * NPA Resource handles
 */
#define NPA_RESOURCE_CLK_CPU                                              \
  Clock_NPACPUResources.aResource[CLOCK_MSS_RESOURCE_CLK_CPU].handle

#define NPA_RESOURCE_CLK_CPU_TEST                                         \
  Clock_NPACPUResources.aResource[CLOCK_MSS_RESOURCE_CLK_CPU_TEST].handle


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * Enumeration of local CPU resources.
 */
enum
{
  CLOCK_MSS_RESOURCE_CLK_CPU,
  CLOCK_MSS_RESOURCE_CLK_CPU_TEST,
  CLOCK_MSS_RESOURCE_TOTAL
};


/*
 * Clock_NPACPUResourcesType
 *
 * Structure containing the NPA node and resource data for local nodes.
 *
 * aResource[CLOCK_MSS_RESOURCE_TOTAL]
 *   - resource data   - /clk/cpu
 *   - resource data   - /clk/cpu.test
 * node           - CPU node data   - /node/clk/cpu
 * hClientBIMC    - NPA client      - /clk/bimc
 */
typedef struct
{
  npa_resource_definition aResource[CLOCK_MSS_RESOURCE_TOTAL];
  npa_node_definition       node;
  npa_client_handle       hClientBIMC;
} Clock_NPACPUResourcesType;


/*=========================================================================
      Prototypes
==========================================================================*/


static npa_resource_state Clock_NPACPUNodeDriverFunc
(
  npa_resource        *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState
);

static npa_resource_state Clock_NPACPUResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
);

static npa_resource_state Clock_NPACPUTestResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
);

static void Clock_NPACPUNodeAvailableCallback
(
  void        *pContext,
  unsigned int nEventType,
  void        *pNodeName,
  unsigned int nNodeNameSize
);

static void Clock_NPABIMCNodeAvailableCallback
(
  void         *pContext,
  unsigned int  nEventType,
  void         *pNodeName,
  unsigned int  nNodeNameSize
);

static npa_query_status Clock_NPACPUResourceQuery
( 
  npa_resource    *resource, 
  unsigned int     id,
  npa_query_type  *result
);

static void Clock_RCECBCallback
(
  void
);


/*=========================================================================
      Data
==========================================================================*/

/*
 * NPA remote resources used on MSS
 */
static npa_remote_resource_definition Clock_aNPARemoteResources[] =
{
  {
    CLOCK_NPA_NODE_NAME_QDSS,
    "clk0\x01\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn, 
    "STATE",
    NPA_MAX_STATE
  },
  {
    CLOCK_NPA_NODE_NAME_PCNOC,
    "clk1\x00\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn, 
    "KHz",
    NPA_MAX_STATE
  },
  {
    CLOCK_NPA_NODE_NAME_SNOC,
    "clk1\x01\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn, 
    "KHz",
    NPA_MAX_STATE
  },
  {
    "/clk/qpic",
    "qpic\x00\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn, 
    "KHz",
    NPA_MAX_STATE
  }
};


/*
 * BIMC NPA remote resource used
 */
static npa_remote_resource_definition Clock_aBIMCNPARemoteResource =
  {
  CLOCK_NPA_NODE_NAME_BIMC,
  "clk2\x00\x00\x00\x00",
    "/protocol/rpm/rpm",
    &npa_max_plugin,
    npa_remote_resource_local_aggregation_driver_fcn, 
    "KHz",
    NPA_MAX_STATE
};


/*
 * Define the local clock resources.
 */
static Clock_NPACPUResourcesType Clock_NPACPUResources =
{
  /*
   * aResource
   */
  {
    /*
     * CPU
   */
  {
    CLOCK_NPA_NODE_NAME_CPU,
    "KHz",
    0,  /* Max, fill in later */
    &npa_max_plugin, 
    NPA_RESOURCE_DEFAULT,
      (npa_user_data)NULL,
    Clock_NPACPUResourceQuery
  },

  /*
     * CPU Test
     */
    {
      CLOCK_NPA_NODE_NAME_CPU_TEST,
      "KHz",
      0,  /* Max, fill in later */
      &npa_max_plugin,
      NPA_RESOURCE_SINGLE_CLIENT,
      (npa_user_data)NULL,
      Clock_NPACPUResourceQuery
    }
  },

  /*
   * node
   */
  { 
    "/node" CLOCK_NPA_NODE_NAME_CPU,    /* name       */
    Clock_NPACPUNodeDriverFunc,                     /* driver_fcn */
    NPA_NODE_DEFAULT,                  /* attributes */
    NULL,                              /* data */
    NPA_EMPTY_ARRAY,
    ARR_SIZE(Clock_NPACPUResources.aResource),
    Clock_NPACPUResources.aResource
  }
};


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_SetCPUConfig
** =========================================================================*/
/**
  Sets CPU configuration.

  This function sets CPU configuration based on input parameters.

  @param *pDrvCtxt [in] -- Pointer to driver context.
  @param  pConfig  [in] -- New configuration.

  @return
  DAL_SUCCESS -- CPU clock set successfully.
  DAL_ERROR  --  CPU clock not set successfully.

  @dependencies
  None.
*/ 

static DALResult Clock_SetCPUConfig
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockCPUConfigType *pNewConfig
)
{
  ClockImageCtxtType *pImageCtxt;
  ClockCPUConfigType *pPreviousConfig;
  ClockNodeType      *pClock;
  DALResult           eResult;
  boolean             bResult;

  /*-----------------------------------------------------------------------*/
  /* Get config data.                                                      */
  /*-----------------------------------------------------------------------*/

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pPreviousConfig = pImageCtxt->CPUCtxt.pConfig;
  pClock = pImageCtxt->CPUCtxt.pClock;

  /*-----------------------------------------------------------------------*/
  /* Short-circuit if the configuration is already active.                 */
  /*-----------------------------------------------------------------------*/

  if (pNewConfig == pPreviousConfig)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Pre-frequency voltage update                                          */
  /*-----------------------------------------------------------------------*/

  if (pNewConfig->eCornerMSS > pPreviousConfig->eCornerMSS)
  {
    eResult =
      DalVCS_SetCPUCorner(
        pImageCtxt->hVCS,
        CLOCK_CPU_MSS_Q6,
        pNewConfig->eCornerMSS);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        DALDEVICEID_CLOCK,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to set CPU voltage.");

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Skip over HAL calls if HW_LOGIC flag is set.                          */
  /*-----------------------------------------------------------------------*/

  if (!(pImageCtxt->CPUCtxt.nDisableDCS & CLOCK_FLAG_DISABLED_HW_LOGIC))
  {

    /*
     *Update RCG before slewing the PLL if frequency is decreasing.
     */
    if ((pNewConfig->Mux.nFreqHz < pPreviousConfig->Mux.nFreqHz) &&
        ((pNewConfig->Mux.HALConfig.eSource !=
            pPreviousConfig->Mux.HALConfig.eSource) ||
         (pNewConfig->Mux.HALConfig.nDiv2x !=
            pPreviousConfig->Mux.HALConfig.nDiv2x)))
    {
      HAL_clk_ConfigClockMux(
        pClock->pDomain->HALHandle,
        &pNewConfig->Mux.HALConfig);
    }
  
    /*
     * Slew PLL.
     */
    if (pNewConfig->Mux.pSourceFreqConfig !=
          pPreviousConfig->Mux.pSourceFreqConfig)
    {
      bResult =
        HAL_clk_ConfigPLL(
          pNewConfig->Mux.HALConfig.eSource,
          &pNewConfig->Mux.pSourceFreqConfig->HALConfig,
          HAL_CLK_SOURCE_CONFIG_MODE_SLEW);
      if (bResult != TRUE)
      {
        DALSYS_LogEvent(
          DALDEVICEID_CLOCK,
          DALSYS_LOGEVENT_FATAL_ERROR,
          "Unable to slew Q6 PLL.");

        return DAL_ERROR_INTERNAL;
      }
    }

    /*
     * Update RCG after slewing the PLL if frequency is increasing.
     */
    if ((pNewConfig->Mux.nFreqHz > pPreviousConfig->Mux.nFreqHz) &&
        ((pNewConfig->Mux.HALConfig.eSource !=
            pPreviousConfig->Mux.HALConfig.eSource) ||
         (pNewConfig->Mux.HALConfig.nDiv2x !=
            pPreviousConfig->Mux.HALConfig.nDiv2x)))
  {
      HAL_clk_ConfigClockMux(
        pClock->pDomain->HALHandle,
        &pNewConfig->Mux.HALConfig);
  }

  }

  /*-----------------------------------------------------------------------*/
  /* Post-frequency voltage update                                         */
  /*-----------------------------------------------------------------------*/

  if (pNewConfig->eCornerMSS < pPreviousConfig->eCornerMSS)
  {
    eResult =
      DalVCS_SetCPUCorner(
        pImageCtxt->hVCS,
        CLOCK_CPU_MSS_Q6,
        pNewConfig->eCornerMSS);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        DALDEVICEID_CLOCK,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to set CPU voltage.");

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update state.                                                         */
  /*-----------------------------------------------------------------------*/

  pImageCtxt->CPUCtxt.pConfig = pNewConfig;

  /*-----------------------------------------------------------------------*/
  /* Log clock frequency.                                                  */
  /*-----------------------------------------------------------------------*/

  Clock_SWEvent(
    CLOCK_EVENT_CLOCK_FREQ,
    2,
    HAL_clk_GetTestClockId(pClock->HALHandle),
    pNewConfig->Mux.nFreqHz / 1000);

  return DAL_SUCCESS;

} /* END Clock_SetCPUConfig */


/* =========================================================================
**  Function : Clock_SetCPUFrequency
** =========================================================================*/
/**
  Sets CPU frequency

  This function sets CPU frequency based on input parameters.

  @param *pDrvCtxt [in] -- Pointer to driver context.
  @param  nFreqHz  [in] -- New frequency.

  @return
  DAL_SUCCESS -- CPU clock set successfully.
  DAL_ERROR  --  CPU clock not set successfully.

  @dependencies
  None.
*/ 

static DALResult Clock_SetCPUFrequency
(
  ClockDrvCtxt *pDrvCtxt,
  uint32        nFreqHz
) 
{
  ClockImageCtxtType     *pImageCtxt;
  ClockCPUPerfConfigType *pCPUPerfConfig;
  ClockCPUConfigType     *pCPUConfig;
  DALResult               eResult;
  uint32                  nCfg, nPL;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pCPUPerfConfig = &pImageCtxt->CPUCtxt.PerfConfig;
  pCPUConfig  = pImageCtxt->pBSPConfig->pCPUConfig;

  /*-----------------------------------------------------------------------*/
  /* Find minimum performance level.                                       */
  /*-----------------------------------------------------------------------*/

  nPL = pCPUPerfConfig->nMinPerfLevel;
  for ( ; nPL < pCPUPerfConfig->nMaxPerfLevel; nPL++)
  {
    nCfg = pCPUPerfConfig->anPerfLevel[nPL];
    if (pCPUConfig[nCfg].Mux.nFreqHz >= nFreqHz)
    {
      break;
    }
  }

  nCfg = pCPUPerfConfig->anPerfLevel[nPL];

  /*-----------------------------------------------------------------------*/
  /* Switch the CPU clock.                                                 */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_SetCPUConfig(pDrvCtxt, &pCPUConfig[nCfg]);

  return eResult;

} /* END of Clock_SetCPUFrequency */


/* =========================================================================
**  Function : Clock_FindCPUMaxConfigAtVoltage
** =========================================================================*/
/**
  Finds the maximum CPU config at the specified voltage level.
 
  @param *pDrvCtxt [in] -- Pointer to driver context.
  @param **pConfig[in] -- Pointer to CPU config pointer.
  @param nCorner[in]   -- Corner id from pmapp_npa.h

  @return
  DAL_ERROR if configuration was not valid, other DAL_SUCCESS.

  @dependencies
  None.
*/ 

DALResult Clock_FindCPUMaxConfigAtVoltage
(
  ClockDrvCtxt          *pDrvCtxt,
  ClockCPUConfigType  **pConfig,
  uint32                nCorner
)
{
  ClockImageCtxtType *pImageCtxt;
  ClockCPUConfigType *pCPUConfig, *pTempConfig;
  uint32              nMinPL, nMaxPL, nPL, nCfg;
 
  /*-----------------------------------------------------------------------*/
  /* Get config data.                                                      */
  /*-----------------------------------------------------------------------*/

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Validate argument.                                                    */
  /*-----------------------------------------------------------------------*/

  if (pConfig == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Find the highest frequency configuration.                             */
  /*-----------------------------------------------------------------------*/

  nMinPL = pImageCtxt->CPUCtxt.PerfConfig.nMinPerfLevel;
  nMaxPL = pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel;
  pCPUConfig = pImageCtxt->pBSPConfig->pCPUConfig;
  pTempConfig = NULL;

  for (nPL = nMinPL; nPL <= nMaxPL; nPL++)
  {
    nCfg = pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel[nPL];

    if (pCPUConfig[nCfg].eCornerMSS > nCorner)
    {
      break;
    }
    else
    {
      pTempConfig = &pCPUConfig[nCfg];
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Validate config was found.                                            */
  /*-----------------------------------------------------------------------*/

  if (pTempConfig == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Store config for caller.                                              */
  /*-----------------------------------------------------------------------*/

  *pConfig = pTempConfig;

  return DAL_SUCCESS;

} /* END Clock_FindCPUMaxConfigAtVoltage */


/* =========================================================================
**  Function : Clock_SetCPUMaxFrequencyAtCurrentVoltage
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetCPUMaxFrequencyAtCurrentVoltage
(
  ClockDrvCtxt *pDrvCtxt,
  boolean       bRequired,
  uint32       *pnResultFreqHz
)
  {
  DALResult           eResult;
  ClockImageCtxtType *pImageCtxt;
  ClockCPUConfigType *pConfig;
  uint32              nCorner;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Find max config @ current voltage if required.                        */
  /*-----------------------------------------------------------------------*/

  if (bRequired == TRUE)
    {
    /*
     * Find the max CPU config at the current voltage.
     */
    nCorner = pImageCtxt->CPUCtxt.pConfig->eCornerMSS;
    eResult = Clock_FindCPUMaxConfigAtVoltage(pDrvCtxt, &pConfig, nCorner);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Configure the CPU to the max supported frequency at the current voltage.
     */
    eResult = Clock_SetCPUFrequency(pDrvCtxt, pConfig->Mux.nFreqHz);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Overwrite the active state (in KHz) of the NPA node.
     */
    pConfig = pImageCtxt->CPUCtxt.pConfig;
    npa_assign_resource_state(NPA_RESOURCE_CLK_CPU, pConfig->Mux.nFreqHz / 1000);

    /*
     * Update result for caller.
     */
    if (pnResultFreqHz != NULL)
    {
      *pnResultFreqHz = pConfig->Mux.nFreqHz;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Restore CPU to requested state.                                       */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * Impulse request results in the resource restoring to its prior value.
     */
    npa_issue_impulse_request(pImageCtxt->CPUCtxt.hNPAClkCPUImpulse);

    /*
     * Update result for caller.
     */
    if (pnResultFreqHz != NULL)
    {
      *pnResultFreqHz = pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_SetCPUMaxFrequencyAtCurrentVoltage */


/* =========================================================================
**  Function : Clock_InitDCVS
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitDCVS
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockImageCtxtType *pImageCtxt;
  uint32              i, nPL, nConfig;
  RCECB_HANDLE        hRCRetVal = RCECB_NULL;
  npa_resource_state  nInitialStates[CLOCK_MSS_RESOURCE_TOTAL];
  
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* We must disable scaling until:                                        */
  /*  - EFS init has complete.                                             */
  /*-----------------------------------------------------------------------*/

  pImageCtxt->CPUCtxt.nDisableDCS = CLOCK_FLAG_DISABLED_BY_EFS;

  /*-----------------------------------------------------------------------*/
  /* Disable DCS if specified in the BSP.                                  */
  /*-----------------------------------------------------------------------*/

  if (pImageCtxt->pBSPConfig->bEnableDCS != TRUE)
  {
    pImageCtxt->CPUCtxt.nDisableDCS |= CLOCK_FLAG_DISABLED_BY_BSP;
  }

  /*-----------------------------------------------------------------------*/
  /* Create callback for BIMC clock node to initialize the min vote.       */
  /*-----------------------------------------------------------------------*/

  npa_resource_available_cb("/clk/bimc", Clock_NPABIMCNodeAvailableCallback, NULL);

  /*-----------------------------------------------------------------------*/
  /* Create callback for end of RCINIT to lower BIMC vote.                 */
  /*-----------------------------------------------------------------------*/

  hRCRetVal = rcecb_register_name(RCINIT_RCECB_INITGROUPS,
                                  (RCECB_CALLBACK_FNSIG)Clock_RCECBCallback);

  if (hRCRetVal == RCECB_NULL)
  {
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to register callback for end of RCINIT");
  }
  
  /*-----------------------------------------------------------------------*/
  /* Create our remote nodes.                                              */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARR_SIZE(Clock_aNPARemoteResources); i++)
  {
    npa_remote_define_resource(
      &Clock_aNPARemoteResources[i],
      (npa_resource_state)0,
      NULL);
  }

  /*-----------------------------------------------------------------------*/
  /* Create our BIMC remote node.                                          */
  /*-----------------------------------------------------------------------*/

  npa_remote_define_resource(
    &Clock_aBIMCNPARemoteResource,
    (npa_resource_state)NPA_MAX_STATE,
    NULL);

  /*-----------------------------------------------------------------------*/
  /* Init the NPA CPU resource/node                                        */
  /* NOTE: Our inital request is the current configuration.                */
  /*-----------------------------------------------------------------------*/

  Clock_NPACPUResources.node.data = (npa_user_data)pDrvCtxt;

  nPL     = pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel;
  nConfig = pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel[nPL];

  for (i = 0; i < Clock_NPACPUResources.node.resource_count; i++)
  {
    Clock_NPACPUResources.aResource[i].max =
      pImageCtxt->pBSPConfig->pCPUConfig[nConfig].Mux.nFreqHz / 1000;
  }

  nInitialStates[CLOCK_MSS_RESOURCE_CLK_CPU] =
    pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz / 1000;
  nInitialStates[CLOCK_MSS_RESOURCE_CLK_CPU_TEST] = 0;

  npa_define_node_cb(
    &Clock_NPACPUResources.node,
    nInitialStates,
    Clock_NPACPUNodeAvailableCallback,
    pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitDCVS */


/* =========================================================================
**  Function : Clock_EnableDCVS
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnableDCVS
(
  ClockDrvCtxt *pDrvCtxt
)
{
  return DAL_SUCCESS;

} /* END Clock_EnableDCVS */


/* =========================================================================
**  Function : Clock_NPACPUResourceDriverFunc
** =========================================================================*/
/**
  NPA node driver function for the CPU resource.
 
  This function handles minimum frequency requests for the CPU NPA resource.
 
  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
*/ 

static npa_resource_state Clock_NPACPUResourceDriverFunc
(
  npa_resource        *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState 
)
{
  ClockDrvCtxt             *pDrvCtxt;
  ClockImageCtxtType *pImageCtxt;
  DALResult           eResult;
  uint32              nDisableDCS, nFreqHz, nPL, nCfg;

  pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  nDisableDCS = pImageCtxt->CPUCtxt.nDisableDCS;
  nFreqHz = NPA_TO_HZ(nState);

  /*-----------------------------------------------------------------------*/
  /* Short-circuit if DCS is disabled by the CPU test.                     */
  /* This is because we don't want to modify the active state of /clk/cpu  */
  /* when a request has been made on /clk/cpu.test.                        */
  /*-----------------------------------------------------------------------*/
   
  if (nDisableDCS & CLOCK_FLAG_DISABLED_BY_CPU_TEST)
  {
    return pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz / 1000;
  }

  /*-----------------------------------------------------------------------*/
  /* If any other non-HW bit is set in the disable DCS flag then set the   */
  /* CPU to the maximum performance level.                                 */
  /*-----------------------------------------------------------------------*/

  nDisableDCS &= ~CLOCK_FLAG_DISABLED_BY_CPU_TEST;
  if (nDisableDCS & ~CLOCK_FLAG_DISABLED_HW_LOGIC)
    {
    nPL = pImageCtxt->CPUCtxt.PerfConfig.nMaxPerfLevel;
    nCfg = pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel[nPL];
    nFreqHz = pImageCtxt->pBSPConfig->pCPUConfig[nCfg].Mux.nFreqHz;
    }

  /*-----------------------------------------------------------------------*/
  /* Update clock frequency.                                               */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_SetCPUFrequency(pDrvCtxt, nFreqHz);
  if (eResult != DAL_SUCCESS)
    {
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to set CPU clock to %l",
      nFreqHz);
  }

  /*-----------------------------------------------------------------------*/
  /* Return active state of this resource.                                 */
  /*-----------------------------------------------------------------------*/

  return pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz / 1000;

} /* END Clock_NPACPUResourceDriverFunc */


/* =========================================================================
**  Function : Clock_NPACPUTestResourceDriverFunc
** =========================================================================*/
/**
  NPA node driver function for the CPU test resource.

  This function handles minimum frequency requests for the CPU test NPA resource.

  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
     */

static npa_resource_state Clock_NPACPUTestResourceDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
  {
  ClockDrvCtxt       *pDrvCtxt;
  ClockImageCtxtType *pImageCtxt;
  DALResult           eResult;

  pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Nothing to do for initialization.                                     */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
    {
    return pResource->active_state;
  }

  /*-----------------------------------------------------------------------*/
  /* Satisfy CPU test request frequency.                                   */
  /*-----------------------------------------------------------------------*/

  if (nState)
  {
    /*
     * Set CPU test disable bit in clock scaling flag.
     */
    pImageCtxt->CPUCtxt.nDisableDCS |= CLOCK_FLAG_DISABLED_BY_CPU_TEST;

    /*
     * Set CPU to frequency requested by the test client.
     */
    eResult = Clock_SetCPUFrequency(pDrvCtxt, NPA_TO_HZ(nState));
    if (eResult != DAL_SUCCESS)
  {
      DALSYS_LogEvent(
        DALDEVICEID_CLOCK,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to set test request CPU clock to %l",
        NPA_TO_HZ(nState));
  }
    /*
     * Update the active state of the /clk/cpu resource.
     */
  else
  {
      npa_assign_resource_state(
        NPA_RESOURCE_CLK_CPU,
        pImageCtxt->CPUCtxt.pConfig->Mux.nFreqHz / 1000);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Clear the disable flag when no client request present.                */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * Clear CPU test disable bit in clock scaling flag.
     */
    pImageCtxt->CPUCtxt.nDisableDCS &= ~CLOCK_FLAG_DISABLED_BY_CPU_TEST;

    /*
     * Set CPU to requested state.
     */
    npa_issue_impulse_request(pImageCtxt->CPUCtxt.hNPAClkCPUImpulse);
  }

  return nState;

} /* END of Clock_NPACPUTestResourceDriverFunc */


/* =========================================================================
**  Function : Clock_NPACPUNodeDriverFunc
** =========================================================================*/
/**
  NPA node driver function for the CPU node.

  This function handles minimum frequency requests for the CPU NPA node.

  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
*/

static npa_resource_state Clock_NPACPUNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  npa_resource_state nActiveState;

  if (pResource == NPA_RESOURCE_CLK_CPU)
  {
    nActiveState = Clock_NPACPUResourceDriverFunc(pResource, hClient, nState);
  }
  else if (pResource == NPA_RESOURCE_CLK_CPU_TEST)
  {
    nActiveState = Clock_NPACPUTestResourceDriverFunc(pResource, hClient, nState);
  }
  else
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_WARNING,
      "Unknown resource requested in CPU node driver func.");

    nActiveState = 0;
  }

  return nActiveState;

} /* END of Clock_NPACPUNodeDriverFunc */


/* =========================================================================
**  Function : Clock_NPACPUNodeAvailableCallback
** =========================================================================*/
/**
  Callback when a /clk/cpu node is created.

  This function is called by the NPA framework when the given /clk/cpu node
  is created.  The creation is delayed until all dependencies are also
  created.

  @param *pContext     [in] -- Context passed in npa_define_node_cb
  @param nEventType    [in] -- Zero.
  @param *pNodeName    [in] -- Name of the node being created.
  @param nNodeNameSize [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/ 

static void Clock_NPACPUNodeAvailableCallback
(
  void        *pContext,
  unsigned int nEventType,
  void        *pNodeName,
  unsigned int nNodeNameSize
)
{
  ClockDrvCtxt        *pDrvCtxt;
  ClockImageCtxtType  *pImageCtxt;

  pDrvCtxt = (ClockDrvCtxt *)pContext;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Create an impulse client for triggering re-aggregation on the CPU     */
  /* resource.                                                             */
  /*-----------------------------------------------------------------------*/

  pImageCtxt->CPUCtxt.hNPAClkCPUImpulse =
    npa_create_sync_client(
      Clock_NPACPUResources.aResource[CLOCK_MSS_RESOURCE_CLK_CPU].name,
      CLOCK_NPA_CLIENT_NAME_CLK_CPU_IMPULSE,
      NPA_CLIENT_IMPULSE);
  if (pImageCtxt->CPUCtxt.hNPAClkCPUImpulse == NULL)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to create NPA sync client %s->%s.",
      CLOCK_NPA_CLIENT_NAME_CLK_CPU_IMPULSE,
      CLOCK_NPA_NODE_NAME_CPU);
  }

} /* END Clock_NPACPUNodeAvailableCallback */


/* =========================================================================
**  Function : Clock_NPABIMCNodeAvailableCallback
** =========================================================================*/
/**
  Callback when a /clk/bimc node is created.

  This function is called by the NPA framework when the given /clk/bimc node
  is created.  The creation is delayed until all dependencies are also
  created.

  @param *pContext     [in] -- Context passed in npa_define_node_cb
  @param nEventType    [in] -- Zero.
  @param *pNodeName    [in] -- Name of the node being created.
  @param nNodeNameSize [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/

static void Clock_NPABIMCNodeAvailableCallback
(
  void        *pContext,
  unsigned int nEventType,
  void        *pNodeName,
  unsigned int nNodeNameSize
)
{

  Clock_NPACPUResources.hClientBIMC =
    npa_create_sync_client(
      "/clk/bimc",
      "/clock",
      NPA_CLIENT_SUPPRESSIBLE);

  if (Clock_NPACPUResources.hClientBIMC == NULL)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to create NPA sync client %s->%s.",
      "/clk/bimc",
      "/clock");
  }

  /*-----------------------------------------------------------------------*/
  /* Place floor vote in the active set for BIMC so that RPM never power   */
  /* collapses BIMC while MPSS is in the active set.                       */
  /* The value of 1 is used as the minimal value above 0 which will be     */
  /* mapped to an actual performance level on RPM.                         */
  /*-----------------------------------------------------------------------*/

  npa_issue_scalar_request(Clock_NPACPUResources.hClientBIMC, NPA_MAX_STATE);

} /* END Clock_NPABIMCNodeAvailableCallback */


/* =========================================================================
**  Function : Clock_RCECBCallback
** =========================================================================*/
/**
  Callback when the final RCINIT group completes.

  This function is called when the final RCINIT group completes in order for
  the clock driver to reduce its vote on /clk/bimc from MAX_NPA_STATE to 1.
  This is needed to ensure that some entity (the clock driver in this instance)
  maintains a non-zero vote on /clk/bimc so that RPM doesn't power collapse
  BIMC while MPSS is in the active set.

  @return
  None.

  @dependencies
  None.
*/
static void Clock_RCECBCallback
(
  void
)
{
  if (Clock_NPACPUResources.hClientBIMC == NULL)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "No /clk/bimc NPA client handle to vote on in RCECB callback");
  }

  npa_issue_scalar_request(Clock_NPACPUResources.hClientBIMC, 1);

} /* END Clock_RCECBCallback */


/* =========================================================================
**  Function : Clock_NPACPUResourceQuery
** =========================================================================*/
/**
  NPA CPU resource query function.
 
  This function is called by NPA query nodes to get a list of supported CPU
  frequencies, or the CPU's vote on its voltage rail.
 
  @param *resource  [in]  -- Pointer to the resource in question
  @param id         [in]  -- ID of the query.
  @param *result    [out] -- Pointer to the data to be filled by this function.

  @return
  npa_query_status - NPA_QUERY_SUCCESS, if query supported.
                   - NPA_QUERY_UNSUPPORTED_QUERY_ID, if query not supported.

  @dependencies
  None.
*/

static npa_query_status Clock_NPACPUResourceQuery
( 
  npa_resource    *pResource,
  unsigned int     id,
  npa_query_type  *pResult
)
{
  ClockDrvCtxt             *pDrvCtxt;
  ClockImageCtxtType       *pImageCtxt;
  ClockCPUConfigType       *pCPUConfig;
  ClockCPUPerfConfigType   *pCPUPerfConfig;
  uint32                    nMinPL, nMaxPL, nConfig,  nIndex;
  uint32                    nNumPerfLevels;
  npa_query_status          nStatus;
  DALResult                 eResult;

  pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pCPUConfig = pImageCtxt->pBSPConfig->pCPUConfig;
  pCPUPerfConfig = NULL;
  nNumPerfLevels = 0;
  nStatus  = NPA_QUERY_UNSUPPORTED_QUERY_ID;

  /*-----------------------------------------------------------------------*/
  /* Get a handle to CPU performance data.                                 */
  /*-----------------------------------------------------------------------*/

  pCPUPerfConfig = &pImageCtxt->CPUCtxt.PerfConfig;

    /*
   * Get min, max, and number of CPU performance levels.
     */
  nMinPL = pCPUPerfConfig->nMinPerfLevel;
  nMaxPL = pCPUPerfConfig->nMaxPerfLevel;
    nNumPerfLevels = nMaxPL - nMinPL + 1;

  /*-----------------------------------------------------------------------*/
  /* If this is a performance level indexing request then return the       */
  /* frequency corresponding to this performance level.                    */
  /*-----------------------------------------------------------------------*/

  if((id >= CLOCK_NPA_QUERY_PERF_LEVEL_KHZ) &&
     (id < CLOCK_NPA_QUERY_NUM_PERF_LEVELS))
  {
    nIndex = ((uint32)id - (uint32)CLOCK_NPA_QUERY_PERF_LEVEL_KHZ);
    if(nIndex < nNumPerfLevels)
    {
      nConfig = pCPUPerfConfig->anPerfLevel[nMinPL + nIndex];
      pResult->type = NPA_QUERY_TYPE_VALUE;
      pResult->data.value = pCPUConfig[nConfig].Mux.nFreqHz / 1000;
      nStatus = NPA_QUERY_SUCCESS;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Returns the number of performance levels.                             */
  /*-----------------------------------------------------------------------*/

  else if(id == CLOCK_NPA_QUERY_NUM_PERF_LEVELS)
    {
    pResult->type = NPA_QUERY_TYPE_VALUE;
    pResult->data.value = nNumPerfLevels;
        nStatus = NPA_QUERY_SUCCESS;
  }
    
  /*-----------------------------------------------------------------------*/
  /* If this is a max frequency at corner voltage indexing request then    */
  /* return the frequency corresponding to this corner voltage.            */
  /*-----------------------------------------------------------------------*/
  
  else if((id >= CLOCK_NPA_QUERY_CPU_MAX_KHZ_AT_CORNER) &&
          (id < CLOCK_NPA_QUERY_MIN_FREQ_KHZ))
  {
    nIndex = ((uint32)id - (uint32)CLOCK_NPA_QUERY_CPU_MAX_KHZ_AT_CORNER);
    if(nIndex < PMIC_NPA_MODE_ID_CORE_RAIL_MAX)
    {
      eResult = Clock_FindCPUMaxConfigAtVoltage(pDrvCtxt, &pCPUConfig, nIndex);
      if (eResult == DAL_SUCCESS)
      {
        pResult->type = NPA_QUERY_TYPE_VALUE;
        pResult->data.value = pCPUConfig->Mux.nFreqHz / 1000;
        nStatus = NPA_QUERY_SUCCESS;
      }
    }
  }
    
  /*-----------------------------------------------------------------------*/
  /* Return the minimum frequency in KHz.                                  */
  /*-----------------------------------------------------------------------*/
    
  else if (id == CLOCK_NPA_QUERY_MIN_FREQ_KHZ)
  {
    nConfig = pCPUPerfConfig->anPerfLevel[nMinPL];
    pResult->type = NPA_QUERY_TYPE_VALUE;
    pResult->data.value = pCPUConfig[nConfig].Mux.nFreqHz / 1000;
        nStatus = NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Return the CPU's corner vote on the voltage rail.                     */
  /*-----------------------------------------------------------------------*/

  else if (id == CLOCK_NPA_QUERY_CPU_RAIL_VOLTAGE_CORNER)
  {
    pResult->type = NPA_QUERY_TYPE_VALUE;
    pResult->data.value =
    pImageCtxt->CPUCtxt.pConfig->eCornerMSS;
    nStatus = NPA_QUERY_SUCCESS;
  }
  else if (id == CLOCK_NPA_QUERY_NUM_CORNERS)
  {
    pResult->type = NPA_QUERY_TYPE_VALUE;
    pResult->data.value = PMIC_NPA_MODE_ID_CORE_RAIL_MAX;
    nStatus = NPA_QUERY_SUCCESS;
  }

  return nStatus;

} /* END Clock_NPACPUResourceQuery */


/* =========================================================================
**  Function : Clock_LoadNV_DCVS
** =========================================================================*/
/*
  See ClockMSS.h
*/ 

DALResult Clock_LoadNV_DCVS
(
  ClockDrvCtxt     *pDrvCtxt,
  CoreConfigHandle  hConfig
)
{
  ClockImageCtxtType     *pImageCtxt;
  ClockCPUPerfConfigType *pCPUPerfConfig;
  uint32                  nReadResult, nData, nPL, nCfg, nFreqHz;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pCPUPerfConfig = &pImageCtxt->CPUCtxt.PerfConfig;

  /*-----------------------------------------------------------------------*/
  /* Make sure the node has been created before we attempt to update.      */
  /*-----------------------------------------------------------------------*/

  if (NPA_RESOURCE_CLK_CPU == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Thread safety.                                                        */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(NPA_RESOURCE_CLK_CPU);

  /*-----------------------------------------------------------------------*/
  /* Update max performance level.                                         */
  /*-----------------------------------------------------------------------*/

  if (hConfig)
  {
    nReadResult =
      CoreConfig_ReadUint32(
        hConfig,
        CLOCK_EFS_MPSS_CONFIG_SECTION,
        CLOCK_EFS_MPSS_MAX_FREQ_HZ_FLAG,
        (unsigned int *)&nData);
    if (nReadResult == CORE_CONFIG_SUCCESS)
    {
      nPL = pCPUPerfConfig->nNumPerfLevels - 1;
      for ( ; nPL > 0; nPL--)
      {
        nCfg = pCPUPerfConfig->anPerfLevel[nPL];
        if (pImageCtxt->pBSPConfig->pCPUConfig[nCfg].Mux.nFreqHz <= nData)
        {
          break;
        }
      }

      pCPUPerfConfig->nMaxPerfLevel = nPL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update min performance level.                                         */
  /*-----------------------------------------------------------------------*/

  if (hConfig)
{
    nReadResult =
      CoreConfig_ReadUint32(
        hConfig,
        CLOCK_EFS_MPSS_CONFIG_SECTION,
        CLOCK_EFS_MPSS_MIN_FREQ_HZ_FLAG,
        (unsigned int *)&nData);
    if (nReadResult == CORE_CONFIG_SUCCESS)
    {
      for (nPL = 0; nPL < pCPUPerfConfig->nMaxPerfLevel; nPL++)
      {
        nCfg = pCPUPerfConfig->anPerfLevel[nPL];
        if (pImageCtxt->pBSPConfig->pCPUConfig[nCfg].Mux.nFreqHz >= nData)
        {
          break;
        }
      }

      pCPUPerfConfig->nMinPerfLevel = nPL;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update DCS enable flag.                                               */
  /* Clear the EFS disable bit since it was set for initialization only.   */
  /* If the EFS file disables EFS then it will re-set the disable bit.     */
  /*-----------------------------------------------------------------------*/

  pImageCtxt->CPUCtxt.nDisableDCS &= ~CLOCK_FLAG_DISABLED_BY_EFS;
  if (hConfig)
  {
    nReadResult =
      CoreConfig_ReadUint32(
        hConfig,
        CLOCK_EFS_MPSS_CONFIG_SECTION,
        CLOCK_EFS_MPSS_DCS_FLAG,
        (unsigned int *)&nData);
    if (nReadResult == CORE_CONFIG_SUCCESS)
    {
      if (nData == FALSE)
      {
        pImageCtxt->CPUCtxt.nDisableDCS |= CLOCK_FLAG_DISABLED_BY_EFS;
  }
      else
  {
        pImageCtxt->CPUCtxt.nDisableDCS = 0;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Notify clients they must reduce their vote due to new max.            */
  /*-----------------------------------------------------------------------*/

  nPL = pCPUPerfConfig->nMaxPerfLevel;
  nCfg = pCPUPerfConfig->anPerfLevel[nPL];
  nFreqHz = pImageCtxt->pBSPConfig->pCPUConfig[nCfg].Mux.nFreqHz;

  if (NPA_RESOURCE_CLK_CPU->request_state > nFreqHz)
{
    // TODO once NPA broadcast API is available
  }

  /*-----------------------------------------------------------------------*/
  /* Verify votes reduced within supported range.                          */
  /*-----------------------------------------------------------------------*/

  if (NPA_RESOURCE_CLK_CPU->request_state > nFreqHz)
  {
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_WARNING,
      "Requests on /clk/cpu exceed the max supported");
  }

  /*-----------------------------------------------------------------------*/
  /* Trigger an impulse to update the CPU frequency per the EFS data.      */
  /* NOTE: We update the resoure state to 0 in order for the impulse       */
  /*       request to make it's way into the driver function.              */
  /*       This workaround will be replaced by a new NPA API.              */
  /*-----------------------------------------------------------------------*/

  npa_update_resource_state(NPA_RESOURCE_CLK_CPU, 0);
  npa_issue_impulse_request(pImageCtxt->CPUCtxt.hNPAClkCPUImpulse);

  /*-----------------------------------------------------------------------*/
  /* Free.                                                                 */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(NPA_RESOURCE_CLK_CPU);

  return DAL_SUCCESS;

} /* END Clock_LoadNV_DCVS */

