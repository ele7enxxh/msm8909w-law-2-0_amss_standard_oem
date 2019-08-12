/*
===========================================================================
*/
/**
  @file ClockMSSConfigBus.c

  MPSS clock driver support for the MSS Config Bus and the Crypto client.
  This file contains the NPA nodes for both of these components.
*/
/*
  ====================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/hw/msm8909/mss/src/ClockMSSConfigBus.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  07/10/12   frv     Created.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "ClockDriver.h"
#include "ClockMSS.h"
#include "ClockSWEVT.h"

#include <DALSys.h>
#include <npa.h>
#include <npa_resource.h>
#include <npa_remote.h>
#include <npa_remote_resource.h>


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * Clock_NPAConfigBusResourcesType
 *
 * Structure containing the NPA node and resource data for the MSS config bus.
 *
 * resource       - MSS Config Bus resource data   - /clk/mss/config_bus
 * node           - MSS Config Bus node data       - /node/clk/mss/config_bus
 */
typedef struct
{
  npa_resource_definition resource;
  npa_node_definition     node;
} Clock_NPAConfigBusResourcesType;


/*
 * Clock_NPACryptoResourcesType
 *
 * Structure containing the NPA node and resource data for the crypto clock.
 *
 * resource    - Crypto client resource data - /clk/mss/crypto
 * node        - Crypto client node data     - /node/clk/mss/crypto
 */
typedef struct
{
  npa_resource_definition   resource;
  npa_node_definition       node;
} Clock_NPACryptoResourcesType;


/*=========================================================================
      Prototypes
==========================================================================*/

static npa_resource_state Clock_NPAConfigBusDriverFunc
(
  struct npa_resource *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState
);


static npa_resource_state Clock_NPACryptoDriverFunc
(
  struct npa_resource *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState
);


static npa_query_status Clock_NPAConfigBusResourceQuery
(
  npa_resource    *resource,
  unsigned int     id,
  npa_query_type  *result
);


/*=========================================================================
      Data
==========================================================================*/


/*
 * Define the MSS config bus clock resources.
 */
static Clock_NPAConfigBusResourcesType Clock_NPAConfigBusResources =
{
  /*
   * resource
   */
  {
    CLOCK_NPA_NODE_NAME_CONFIG_BUS,
    "KHz",
    0, /* Max, fill in later */
    &npa_max_plugin,
    NPA_RESOURCE_DEFAULT,
    (npa_user_data)NULL,
    Clock_NPAConfigBusResourceQuery
  },

  /*
   * node
   */
  {
    "/node" CLOCK_NPA_NODE_NAME_CONFIG_BUS,   /* name       */
    Clock_NPAConfigBusDriverFunc,             /* driver_fcn */
    NPA_NODE_DEFAULT,                         /* attributes */
    NULL,                                     /* data       */
    NPA_EMPTY_ARRAY,
    1, &Clock_NPAConfigBusResources.resource
  },
};


/*
 * Define the crypto clock resources.
 */
static Clock_NPACryptoResourcesType Clock_NPACryptoResources =
{
  /*
   * resource
   */
  {
    CLOCK_NPA_NODE_NAME_CRYPTO,
    "0/1",
    1,
    &npa_max_plugin,
    NPA_RESOURCE_DEFAULT,
    (npa_user_data)NULL,
    },

  /*
   * node
   */
    {
    "/node" CLOCK_NPA_NODE_NAME_CRYPTO,   /* name       */
    Clock_NPACryptoDriverFunc,            /* driver_fcn */
    NPA_NODE_DEFAULT,                     /* attributes */
    NULL,                                 /* data       */
    NPA_EMPTY_ARRAY,
    1, &Clock_NPACryptoResources.resource
  }
};


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_SetConfigBusConfig
** =========================================================================*/
/**
  Set MSS config bus configuration.

  This function sets the MSS config bus configuration based on input parameters.

  @param *pDrvCtxt [in] -- Pointer to driver context.
  @param  pConfig  [in] -- New configuration.

  @return
  None.

  @dependencies
  None.
*/

void Clock_SetConfigBusConfig
(
  ClockDrvCtxt             *pDrvCtxt,
  ClockConfigBusConfigType *pConfig
)
{
  ClockImageCtxtType           *pImageCtxt;
  ClockConfigBusConfigType     *pPreviousConfig;

  pImageCtxt =(ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pPreviousConfig = pImageCtxt->ConfigBusCtxt.pConfig;

  /*-----------------------------------------------------------------------*/
  /* Short-circuit if the configuration is already active.                 */
  /*-----------------------------------------------------------------------*/

  if (pConfig == pPreviousConfig)
  {
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Thread safety - Critical Section [Start]                              */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Switch MSS config bus clock.                                          */
  /*-----------------------------------------------------------------------*/

  Clock_SetClockConfig(
    pDrvCtxt,
    pImageCtxt->ConfigBusCtxt.pClock->pDomain,
    &pConfig->Mux);

  /*-----------------------------------------------------------------------*/
  /* Update state.                                                         */
  /*-----------------------------------------------------------------------*/

  pImageCtxt->ConfigBusCtxt.pConfig = pConfig;

  /*-----------------------------------------------------------------------*/
  /* Thread safety - Critical Section [End]                                */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_FREE(pDrvCtxt);

} /* END of Clock_SetConfigBusConfig */


/* =========================================================================
**  Function : Clock_InitConfigBus
** =========================================================================*/
/*
  See ClockMSS.h
*/

DALResult Clock_InitConfigBus
(
  ClockDrvCtxt *pDrvCtxt
)
{
  uint32             nPL, nConfig;
  npa_resource_state nInitialRequest;
  ClockImageCtxtType  *pImageCtxt;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Init MSS config bus NPA resource.                                     */
  /*-----------------------------------------------------------------------*/

  Clock_NPAConfigBusResources.node.data = (npa_user_data)pDrvCtxt;

  nPL = pImageCtxt->pBSPConfig->pConfigBusPerfConfig->nMaxPerfLevel;
  nConfig = pImageCtxt->pBSPConfig->pConfigBusPerfConfig->anPerfLevel[nPL];

  Clock_NPAConfigBusResources.resource.max =
    pImageCtxt->pBSPConfig->pConfigBusConfig[nConfig].Mux.nFreqHz / 1000;

  nInitialRequest = Clock_NPAConfigBusResources.resource.max;

  npa_define_node(&Clock_NPAConfigBusResources.node, &nInitialRequest, NULL);

  /*-----------------------------------------------------------------------*/
  /* Init crypto NPA resource.                                             */
  /*-----------------------------------------------------------------------*/

  Clock_NPACryptoResources.node.data = (npa_user_data)pDrvCtxt;

  nInitialRequest = 0;

  npa_define_node(&Clock_NPACryptoResources.node, &nInitialRequest, NULL);

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitPLL */


/* =========================================================================
**  Function : Clock_NPAConfigBusDriverFunc
** =========================================================================*/
/**
  Handle state changes on the MSS config bus NPA node.

  This function handles state changes on the MSS config bus node.

  @param pResource [in] -- The NPA resource being requested.
  @param hClient [in]   -- Pointer to the client making the request.
  @param nState [in]    -- New state of the resource.

  @return
  New state of the resource.

  @dependencies
  None.
*/

npa_resource_state Clock_NPAConfigBusDriverFunc
(
  struct npa_resource *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState
)
{
  uint32 nPL, nConfig, nMinPL, nMaxPL;
  uint32                     nResultFreqHz, nMinFreqHz;
  ClockDrvCtxt              *pDrvCtxt;
  ClockImageCtxtType        *pImageCtxt;
  ClockConfigBusConfigType  *pConfigBusConfig;

  nMinFreqHz = NPA_TO_HZ(nState);
  pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pConfigBusConfig = pImageCtxt->pBSPConfig->pConfigBusConfig;

  /*-----------------------------------------------------------------------*/
  /* Find minimum performance level.                                       */
  /*-----------------------------------------------------------------------*/

  nMinPL = pImageCtxt->pBSPConfig->pConfigBusPerfConfig->nMinPerfLevel;
  nMaxPL = pImageCtxt->pBSPConfig->pConfigBusPerfConfig->nMaxPerfLevel;

  if (nState == NPA_MAX_STATE)
  {
    nPL = nMaxPL;
  }
  else
  {
    for (nPL = nMinPL; nPL < nMaxPL; nPL++)
    {
      nConfig = pImageCtxt->pBSPConfig->pConfigBusPerfConfig->anPerfLevel[nPL];

      if (pConfigBusConfig[nConfig].Mux.nFreqHz >= nMinFreqHz)
      {
        break;
      }
    }
  }

  nConfig = pImageCtxt->pBSPConfig->pConfigBusPerfConfig->anPerfLevel[nPL];

  /*-----------------------------------------------------------------------*/
  /* Switch the bus clock.                                                 */
  /*-----------------------------------------------------------------------*/

  Clock_SetConfigBusConfig(pDrvCtxt, &pConfigBusConfig[nConfig]);

  /*-----------------------------------------------------------------------*/
  /* Return new state of this resource.                                    */
  /*-----------------------------------------------------------------------*/

  nResultFreqHz = pImageCtxt->ConfigBusCtxt.pConfig->Mux.nFreqHz / 1000;

  return (npa_resource_state)nResultFreqHz;

} /* END Clock_NPAConfigBusDriverFunc */


/* =========================================================================
**  Function : Clock_NPACryptoDriverFunc
** =========================================================================*/
/**
  Handle state changes on the crypto clock NPA node.

  This function handles state changes on the crypto clock node.

  @param pResource [in] -- The NPA resource being requested.
  @param hClient [in]   -- Pointer to the client making the request.
  @param nState [in]    -- New state of the resource.

  @return
  New state of the resource.

  @dependencies
  None.
*/

static npa_resource_state Clock_NPACryptoDriverFunc
(
  struct npa_resource *pResource,
  npa_client_handle    hClient,
  npa_resource_state   nState
)
{
  ClockDrvCtxt *pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  ClockImageCtxtType  *pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Turn on the crypto clock when > 0 is requested.                       */
  /*-----------------------------------------------------------------------*/

  if (nState)
  {
    Clock_EnableClock(pDrvCtxt, (ClockIdType)pImageCtxt->ConfigBusCtxt.pCryptoClock);
  }

  /*-----------------------------------------------------------------------*/
  /* Turn off the crypto clock when 0 is requested.                        */
  /*-----------------------------------------------------------------------*/

  else
  {
    Clock_DisableClock(pDrvCtxt, (ClockIdType) pImageCtxt->ConfigBusCtxt.pCryptoClock);
  }

  return nState;

} /* END Clock_NPACryptoDriverFunc */


/* =========================================================================
**  Function : Clock_NPAConfigBusResourceQuery
** =========================================================================*/
/**
  NPA CPU resource query function.

  This function is called to get the /clk/mss/config_bus frequency.

  @param *resource  [in]  -- Pointer to the resource in question
  @param id         [in]  -- ID of the query.
  @param *result    [out] -- Pointer to the data to be filled by this function.

  @return
  npa_query_status - NPA_QUERY_SUCCESS, if query supported.
                   - NPA_QUERY_UNSUPPORTED_QUERY_ID, if query not supported.

  @dependencies
   None.
*/

static npa_query_status Clock_NPAConfigBusResourceQuery
(
  npa_resource    *pResource,
  unsigned int     id,
  npa_query_type  *result
)
{
  ClockDrvCtxt                 *pDrvCtxt;
  ClockImageCtxtType           *pImageCtxt;
  ClockConfigBusConfigType     *pBusConfig;
  ClockConfigBusPerfConfigType *pBusPerfConfig = NULL;
  npa_query_status              nStatus;
  uint32                        nMinPL, nMaxPL, nConfig, nPerfLevel;
  uint32                        nMinFreq, nNumPerfLevels;

  pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;
  pBusConfig = pImageCtxt->pBSPConfig->pConfigBusConfig;
  pBusPerfConfig = NULL;
  nMinFreq = 0;
  nNumPerfLevels = 0;
  nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;

  /*-----------------------------------------------------------------------*/
  /* Get offset for any performance level requests.                        */
  /*-----------------------------------------------------------------------*/

  if( id >= CLOCK_NPA_QUERY_PERF_LEVEL_KHZ)
  {
    nPerfLevel = ((uint32)id - (uint32)CLOCK_NPA_QUERY_PERF_LEVEL_KHZ);
  }
  else
  {
    return nStatus;
  }

  /*-----------------------------------------------------------------------*/
  /* Get a handle to the bus performance data.                             */
  /*-----------------------------------------------------------------------*/

  pBusPerfConfig = pImageCtxt->pBSPConfig->pConfigBusPerfConfig;

  if( pBusPerfConfig != NULL )
  {
    /*
     * Get min and max performance levels of the bus.
     */
    nMinPL = pBusPerfConfig->nMinPerfLevel;
    nMaxPL = pBusPerfConfig->nMaxPerfLevel;
    nNumPerfLevels = nMaxPL - nMinPL + 1;
    nConfig = pBusPerfConfig->anPerfLevel[nMinPL];
    nMinFreq = pBusConfig[nConfig].Mux.nFreqHz/1000;

    /*
     * If this is a performance level indexing request then return the
     * frequency corresponding to this performance level.
     */
    if( nPerfLevel < nNumPerfLevels )
    {
      nConfig = pBusPerfConfig->anPerfLevel[nPerfLevel];
      result->type = NPA_QUERY_TYPE_VALUE;
      result->data.value = pBusConfig[nConfig].Mux.nFreqHz/1000;
      nStatus = NPA_QUERY_SUCCESS;
    }
  }

  if( nStatus != NPA_QUERY_SUCCESS )
  {
    switch( id )
    {
      /*
       * Returns the number of performance levels.
       */
      case CLOCK_NPA_QUERY_NUM_PERF_LEVELS:
        result->type = NPA_QUERY_TYPE_VALUE;
        result->data.value = nNumPerfLevels;
        nStatus = NPA_QUERY_SUCCESS;
        break;

      /*
       * Return the minimum frequency in KHz.
       */
      case CLOCK_NPA_QUERY_MIN_FREQ_KHZ:
        result->type = NPA_QUERY_TYPE_VALUE;
        result->data.value = nMinFreq;
        nStatus = NPA_QUERY_SUCCESS;
        break;

      default:
        break;
    }
  }

  return nStatus;

} /* END Clock_NPAConfigBusResourceQuery */

