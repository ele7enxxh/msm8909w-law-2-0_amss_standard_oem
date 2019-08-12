/*
===========================================================================
*/
/**
  @file ClockMSSXO.c 
  
  The file contains the resource definitions for XO management on MSS.
*/
/*  
  ====================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/hw/msm8909/mss/src/ClockMSSXO.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  11/05/12   frv     Updated comments/names.
  02/17/12   vs      Created.
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

/*
 * External header files.
 */
#include <sleep_lpr.h>
#include <npa.h>
#include <npa_resource.h>
#include <npa_remote.h>
#include <npa_remote_resource.h>

#include "DALDeviceId.h"

/*
 * KVP & RPM header files
 */
#include "kvp.h"
#include "rpm.h"
#include "rpmclient.h"

/*
 * Clock regime headers.
 */
#include "ClockDriver.h"
#include "HALclk.h"
#include "ClockMSS.h"
#include "ClockSWEVT.h"


/*=========================================================================
      Macros
==========================================================================*/


/*
 * NPA Resource handles
 */
#define NPA_RESOURCE_XO                                                \
  Clock_XO.resource.handle


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Clock_XOType
 *
 * Structure containing the XO NPA node and resource data.
 *
 *  resource    - CXO resource data   - /node/xo/cxo
 *  node        - CXO node data       - /xo/cxo
 *  lpr       - CXO lpr           - handle to sleep LPR.
 */
typedef struct
{
  npa_resource_definition   resource;
  npa_node_definition       node;
  npa_client_handle       lpr;
} ClockXOType;


/*=========================================================================
      Prototypes
==========================================================================*/

static npa_resource_state Clock_NPAXODriverFunc
(
  npa_resource *pResource,
  npa_client   *pClient,
  npa_resource_state nState
);


/*=========================================================================
      Variables
==========================================================================*/


/*
 * NPA remote XO resource.
 */
static npa_remote_resource_definition Clock_XORemote =
{
  "/clkregim/modem/xo/cxo",
  "clk0\x00\x00\x00\x00",
  "/protocol/rpm/rpm",
  &npa_max_plugin,
  npa_remote_resource_local_aggregation_driver_fcn, 
  "Enable/Disable",
  1
};


/*
 * ClockXO
 *
 * Low power resource data.
 */
static ClockXOType Clock_XO =
{
  /*
   * resource
   */
  {
    CLOCK_NPA_NODE_NAME_CXO,
    "on/off",
    CLOCK_SOURCE_REQUIRED,  /* max state */
    &npa_max_plugin, 
    NPA_RESOURCE_DEFAULT,
    (npa_user_data)HAL_CLK_SOURCE_XO
  },

  /*
   * node
   */
  { 
    "/node" CLOCK_NPA_NODE_NAME_CXO,     /* name */
    Clock_NPAXODriverFunc,               /* driver_fcn */
    NPA_NODE_DEFAULT,                    /* attributes */
    NULL,                                /* data */
    NPA_EMPTY_ARRAY,
    1, &Clock_XO.resource
  }
};


  /*
 * kvp for holding the ‘keep CXO resource en/dis abled' in the Sleep Set 
   */
static kvp_t* pCXODisableKVP; 
static kvp_t* pCXOEnableKVP; 

/*
 * kvp values 0 and 1 
 */
static const uint32 nCXODisableReq = 0; /* Represents the 0 value for the kvp */
static const uint32 nCXOEnableReq  = 1; /* Represents the 1 value for the kvp */

/*
 * kvp enable request macro 
 */
#define KVP_ENABLE_REQUEST 0x62616e45

/*
 * Each 'kvp' item has three fields: key, size of value and value 
 */
#define KVP_ITEM_SZ 4*3


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_NPAXODriverFunc
** =========================================================================*/
/**
  Handle state changes on the CXO node.
 
  This function handles state changes on the XO node and will forward
  updates to the low-power-resource.
 
  @param pResource [in] -- The NPA resource being requested.
  @param pClient [in]   -- Pointer to the client making the request.
  @param nState [in]    -- New state of the resource.

  @return
  New state of the resource.

  @dependencies
  None.
*/ 

static npa_resource_state Clock_NPAXODriverFunc
(
  npa_resource      *pResource,
  npa_client        *pClient,
  npa_resource_state nState
)
{
  /*-----------------------------------------------------------------------*/
  /* Issue a request to the sleep node to enable or disable this node's    */
  /* LPRMs during sleep.                                                   */
  /*  0: XO is not required at all by any insuppressible clocks.           */
  /*  1: XO is required by insuppressible clocks.                          */
  /*-----------------------------------------------------------------------*/

  if (Clock_XO.lpr)
  {
  if (nState == CLOCK_SOURCE_REQUIRED)
  {
      kvp_reset(pCXOEnableKVP);
      rpm_post_request(RPM_SLEEP_SET, RPM_CLOCK_0_REQ, 0, pCXOEnableKVP);
      npa_complete_request(Clock_XO.lpr);
  }
  else
  {
      kvp_reset(pCXODisableKVP);
      rpm_post_request(RPM_SLEEP_SET, RPM_CLOCK_0_REQ , 0, pCXODisableKVP);
      npa_issue_scalar_request(Clock_XO.lpr, SLEEP_LPRM_NUM(0));
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Log XO LPR event                                                      */
  /*-----------------------------------------------------------------------*/

  Clock_SWEvent(CLOCK_EVENT_XO, 1, !nState);

  return nState;

} /* END Clock_NPAXODriverFunc */


/* =========================================================================
**  Function : Clock_LPRNodeAvailableCallback
** =========================================================================*/
/**
  Callback when the XO LPR node is created.
 
  This function is called by the NPA framework when the given LPR node is
  created.  The creation is delayed until all dependencies are also
  created.
 
  @param *pContext [in] -- Context passed in npa_define_node_cb
  @param nEventType [in] -- Zero.
  @param *pNodeName [in] -- Name of the node being created.
  @param nNodeNameSize [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/ 

static void Clock_LPRNodeAvailableCallback
(
  void        *pContext,
  unsigned int nEventType,
  void        *pNodeName,
  unsigned int nNodeNameSize
)
{
  /*-----------------------------------------------------------------------*/
  /* At init time, register the LPR with the sleep LPR node, and request   */
  /* a bit mask to use for the CXO LPRMs from the sleep LPR node.          */
  /*-----------------------------------------------------------------------*/

  Clock_XO.lpr =
    npa_create_sync_client(
      SLEEP_LPR_NODE_NAME,
      "/clock",
      NPA_CLIENT_REQUIRED);

  if (!Clock_XO.lpr)
  {
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to create NPA client to cxo lpr");

    return;
  }

  sleep_define_lpr_str("cxo", Clock_XO.lpr);

  /*-----------------------------------------------------------------------*/
  /* Thread safety.                                                        */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(NPA_RESOURCE_XO);

  /*-----------------------------------------------------------------------*/
  /* Trigger CPU state update.                                             */
  /*-----------------------------------------------------------------------*/

  npa_assign_resource_state(
    NPA_RESOURCE_XO,
    Clock_NPAXODriverFunc(
      NPA_RESOURCE_XO, NULL, NPA_RESOURCE_XO->request_state));

  /*-----------------------------------------------------------------------*/
  /* Free.                                                                 */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(NPA_RESOURCE_XO);

} /* END Clock_LPRNodeAvailableCallback */


static void Clock_XONodeAvailableCallback
(
  void        *pContext,
  unsigned int nEventType,
  void        *pNodeName,
  unsigned int nNodeNameSize
)
{
  npa_node_definition *pNode = (npa_node_definition *)pContext;
  ClockDrvCtxt *pCtxt = (ClockDrvCtxt *)pNode->data;
  uint32 nIndex = 0xFF;

  /*-----------------------------------------------------------------------*/
  /* Create the appropriate NPA client.                                    */
  /*-----------------------------------------------------------------------*/

  if (pNode == &Clock_XO.node)
  {
    nIndex = pCtxt->anSourceIndex[HAL_CLK_SOURCE_XO];

    pCtxt->aSources[nIndex].NPAHandle = 
      npa_create_sync_client(
        Clock_XO.resource.name, "/clock", NPA_CLIENT_REQUIRED);
  }

} /* END Clock_XONodeAvailableCallback */


/* =========================================================================
**  Function : Clock_InitXO
** =========================================================================*/
/*
  See ClockMSS.h
*/

DALResult Clock_InitXO
(
  ClockDrvCtxt *pCtxt
)
{
  npa_resource_state nInitialState;

  /*-----------------------------------------------------------------------*/
  /* Allow space for two kvp values and populate them                      */
  /*-----------------------------------------------------------------------*/

  pCXODisableKVP = kvp_create(KVP_ITEM_SZ);
  pCXOEnableKVP  = kvp_create(KVP_ITEM_SZ);
  kvp_put(pCXOEnableKVP, KVP_ENABLE_REQUEST, sizeof(nCXOEnableReq), 
         (void *)&nCXOEnableReq);
  kvp_put(pCXODisableKVP, KVP_ENABLE_REQUEST, sizeof(nCXODisableReq), 
         (void *)&nCXODisableReq);

  /*-----------------------------------------------------------------------*/
  /* Init CXO node with initial vote of OFF in the sleep set.              */
  /*-----------------------------------------------------------------------*/

  Clock_XO.node.data = (npa_user_data)pCtxt;

  nInitialState = 0;

  npa_define_node_cb(
    &Clock_XO.node,
    &nInitialState,
    Clock_XONodeAvailableCallback,
    &Clock_XO.node
    );

  /*-----------------------------------------------------------------------*/
  /* Send initial vote of "1" to keep XO in the active set.                */
  /*-----------------------------------------------------------------------*/

  npa_remote_define_resource(&Clock_XORemote, 1, NULL);
 
  /*-----------------------------------------------------------------------*/
  /* Create callback for the Sleep XO LPR.                                 */
  /*-----------------------------------------------------------------------*/

  npa_resource_available_cb(
    SLEEP_LPR_NODE_NAME,
    Clock_LPRNodeAvailableCallback,
    NULL);
 
  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitXO*/

