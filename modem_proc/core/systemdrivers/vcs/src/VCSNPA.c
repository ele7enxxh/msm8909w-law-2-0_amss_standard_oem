/*
===========================================================================
*/
/**
  @file VCSNPA.c 
  
  NPA-related functions for the VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/src/VCSNPA.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/22/14   lil     Created.
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "comdef.h"
#include "DDIPlatformInfo.h"
#include "VCSDriver.h"
#include "pmapp_npa.h"
#include "DALDeviceId.h"
#include <DALSys.h>
#include <npa.h>
#include <npa_resource.h>
#include <npa_remote.h>
#include <npa_remote_resource.h>


/*=========================================================================
      Macros
==========================================================================*/


#define VCS_NPA_UNITS_RAIL "VReg Corner"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_NPAVoteDependency
** =========================================================================*/
/**
  Issue an NPA request on the rail resource dependency.

  This function makes an NPA request on a specific rail dependency resource.

  @param *pRail [in]   -- Pointer to rail node.
  @param  hClient [in] -- Client handle.
  @param  eCorner [in] -- Corner vote.

  @dependencies
  None.
*/

static void VCS_NPAVoteDependency
(
  VCSRailNodeType   *pRail,
  npa_client_handle  hClient,
  uint32             eCorner
)
{
  VCSDrvCtxt    *pDrvCtxt;
  VCSCornerType  eCornerAdjusted = eCorner;

  /*-----------------------------------------------------------------------*/
  /* Bring votes up to within the supported range if vote is not OFF.      */
  /*-----------------------------------------------------------------------*/

  if (eCorner != VCS_CORNER_OFF)
  {
    eCornerAdjusted = MAX(eCorner, pRail->eCornerMin);

    /*
     * Validate eCornerAdjusted
     */
    if(eCornerAdjusted >= VCS_CORNER_NUM_OF_CORNERS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Invalid corner[%lu] on rail[%s]",
        eCornerAdjusted,
        pRail->pBSPConfig->szName);

      return;
    }

    if (pRail->pVoltageTable)
    {
      for ( ; eCornerAdjusted <= pRail->eCornerMax; eCornerAdjusted++)
      {
    	if (pRail->pVoltageTable->apCornerMap[eCornerAdjusted] != NULL)
        {
          break;
        }
      }

      /*
       * Validate eCornerAdjusted again in the voltage table.
       */
      if (eCornerAdjusted > pRail->eCornerMax)
      {
        DALSYS_LogEvent(
          0,
          DALSYS_LOGEVENT_FATAL_ERROR,
          "DALLOG Device VCS: Request[%lu] on rail[%s] above max corner[%lu]",
          eCornerAdjusted,
          pRail->pBSPConfig->szName,
          pRail->eCornerMax);

        return;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Get the driver context.                                               */
  /*-----------------------------------------------------------------------*/
  
  pDrvCtxt = VCS_GetDrvCtxt();
  
  /*-----------------------------------------------------------------------*/
  /* Set request attribute to fire-and-forget if we are setting a lower    */
  /* voltage corner.                                                       */
  /*-----------------------------------------------------------------------*/

  if(eCornerAdjusted < pRail->eCorner)
  {
    npa_set_request_attribute(hClient, NPA_REQUEST_FIRE_AND_FORGET);
  }

  /*-----------------------------------------------------------------------*/
  /* Issue the request to satisfy the dependency.                          */
  /*-----------------------------------------------------------------------*/

  npa_issue_scalar_request(hClient, pDrvCtxt->pnCornerPMICMap[eCornerAdjusted]);

} /* END of VCS_NPAVoteDependency */


/* =========================================================================
**  Function : VCS_NPASatisfyRailDependencies
** =========================================================================*/
/**
  Issue NPA requests on the rail's resource dependencies.

  This function makes NPA requests on the rail's resource dependencies.

  @param *pRail [in]     -- Pointer to rail node.
  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in]   -- Client handle.

  @dependencies
  None.
*/

static void VCS_NPASatisfyRailDependencies
(
  VCSRailNodeType   *pRail,
  npa_resource      *pResource,
  npa_client_handle  hClient
)
{
  VCSCornerType     eCornerRequired, eCornerSuppressible;
  npa_client_handle hClientRequired, hClientSuppressible;

  /*-----------------------------------------------------------------------*/
  /* Determine which client handle to use and which value to send.         */
  /*-----------------------------------------------------------------------*/

  eCornerSuppressible = NPA_RESOURCE_SUPPRESSIBLE_REQUEST(pResource);
  eCornerRequired = NPA_RESOURCE_REQUIRED_REQUEST(pResource);

  hClientSuppressible = pRail->aDependency[VCS_NPA_RAIL_DEP_IDX_SUP].handle;
  hClientRequired = pRail->aDependency[VCS_NPA_RAIL_DEP_IDX_REQ].handle;

  if (hClient->type == NPA_CLIENT_SUPPRESSIBLE)
  {
    VCS_NPAVoteDependency(pRail, hClientSuppressible, eCornerSuppressible);
  }
  else if (hClient->type == NPA_CLIENT_REQUIRED)
  {
    VCS_NPAVoteDependency(pRail, hClientRequired, eCornerRequired);
  }
  else if (hClient->type == NPA_CLIENT_IMPULSE)
  {
    VCS_NPAVoteDependency(pRail, hClientSuppressible, eCornerSuppressible);
    VCS_NPAVoteDependency(pRail, hClientRequired, eCornerRequired);
  }

} /* END of VCS_NPASatisfyRailDependencies */


/* =========================================================================
**  Function : VCS_NPARailResourceQuery
** =========================================================================*/
/**
  NPA rail resource query function.

  This function is called to get the following rail information:
    -- Number of corners.
    -- Voltage table for each corner.
    -- Current corner (id / table index).

  @param *pResource [in]  -- Pointer to the resource in question
  @param  nID [in]        -- ID of the query.
  @param *pResult [out]   -- Pointer to the data to be filled by this function.

  @return
  npa_query_status - NPA_QUERY_SUCCESS, if query supported.
                   - NPA_QUERY_UNSUPPORTED_QUERY_ID, if query not supported.

  @dependencies
  None.
*/

static npa_query_status VCS_NPARailResourceQuery
(
  npa_resource   *pResource,
  unsigned int    nID,
  npa_query_type *pResult
)
{
  npa_query_status           nStatus = NPA_QUERY_SUCCESS;
  uint32                     nNumCorners, nCorner;
  VCSCornerVoltageRangeType *pVoltageCorner;
  VCSRailNodeType           *pRail = (VCSRailNodeType *)pResource->node->data;

  /*-----------------------------------------------------------------------*/
  /* Validate parameters.                                                  */
  /*-----------------------------------------------------------------------*/

  if (pResource == NULL || pResult == NULL || pRail == NULL)
  {
    return NPA_QUERY_NULL_POINTER;
  }

  /*-----------------------------------------------------------------------*/
  /* Check for the current corner.                                         */
  /*-----------------------------------------------------------------------*/

  if(nID == VCS_NPA_QUERY_VOLTAGE_CURRENT_LEVEL_ID)
  {
    /*
     * Store current corner request.
     */
    pResult->data.value = pRail->eCorner;
    pResult->type = NPA_QUERY_TYPE_VALUE;

    return NPA_QUERY_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Only process voltage related queries if we have a voltage table.      */
  /*-----------------------------------------------------------------------*/

  if (pRail->pVoltageTable == NULL)
  {
    return NPA_QUERY_UNSUPPORTED_QUERY_ID;
  }

  if (nID == VCS_NPA_QUERY_VOLTAGE_NUM_ENTRIES)
  {
    /*
     * Store the number of entries in the corner voltage table.
     */
    pResult->data.value = pRail->pVoltageTable->nNumVoltageRanges;
    pResult->type = NPA_QUERY_TYPE_VALUE;
  }
  else if(nID == VCS_NPA_QUERY_VOLTAGE_CURRENT_VOLTAGE_UV)
  {
    /*
     * Store current voltage request.
     */
    pResult->data.value = pRail->nVoltageUV;
    pResult->type = NPA_QUERY_TYPE_VALUE;
  }
  else if (nID >= VCS_NPA_QUERY_VOLTAGE_LEVEL_ID)
  {
    nNumCorners = pRail->pVoltageTable->nNumVoltageRanges;

    if (nID < VCS_NPA_QUERY_VOLTAGE_LEVEL_FLOOR)
    {
      nCorner = nID - VCS_NPA_QUERY_VOLTAGE_LEVEL_ID;
      if (nCorner >= nNumCorners)
      {
        nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      }
      else
      {
        pVoltageCorner = &pRail->pVoltageTable->pVoltageRange[nCorner];
        pResult->data.value = pVoltageCorner->eCorner;
        pResult->type = NPA_QUERY_TYPE_VALUE;
      }
    }
    else if (nID < VCS_NPA_QUERY_VOLTAGE_LEVEL_CEILING)
    {
      nCorner = nID - VCS_NPA_QUERY_VOLTAGE_LEVEL_FLOOR;
      if (nCorner >= nNumCorners)
      {
        nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      }
      else
      {
        pVoltageCorner = &pRail->pVoltageTable->pVoltageRange[nCorner];
        pResult->data.value = pVoltageCorner->nMinUV;
        pResult->type = NPA_QUERY_TYPE_VALUE;
      }
    }
    else
    {
      nCorner = nID - VCS_NPA_QUERY_VOLTAGE_LEVEL_CEILING;
      if (nCorner >= nNumCorners)
      {
        nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      }
      else
      {
        pVoltageCorner = &pRail->pVoltageTable->pVoltageRange[nCorner];
        pResult->data.value = pVoltageCorner->nMaxUV;
        pResult->type = NPA_QUERY_TYPE_VALUE;
      }
    }
  }
  else
  {
    nStatus = NPA_QUERY_UNSUPPORTED_QUERY_ID;
  }

  return nStatus;

}


/* =========================================================================
**  Function : VCS_NPARailDriverFunc
** =========================================================================*/
/**
  Handle request state changes on a rail's NPA resource.

  This is a generic NPA driver function applicable to all rail resources.
  This function does some common record keeping, and makes use of function
  pointers to invoke rail-specific functions to actually set the voltage.

  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in]   -- Client handle.
  @param  nState [in]    -- New request state.

  @return
  New resource state.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPARailDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  uint32           i;
  DALResult        eResult;
  VCSCPUNodeType  *pCPU;
  VCSLDONodeType  *pLDO;
  VCSRailNodeType *pRail;
  VCSCornerType    eCornerAggregated;
  VCSDrvCtxt      *pDrvCtxt;

  pRail = (VCSRailNodeType *)pResource->node->data;
  eCornerAggregated = nState;

  /*-----------------------------------------------------------------------*/
  /* Validate the max rail voltage corner before proceeding                */
  /*-----------------------------------------------------------------------*/

  if(pRail->eCornerMax >= VCS_CORNER_NUM_OF_CORNERS)
  {
    DALSYS_LogEvent(
    0,
    DALSYS_LOGEVENT_FATAL_ERROR,
    "DALLOG Device VCS: Invalid max corner [%lu] on rail[%s]",
    pRail->eCornerMax,
    pRail->pBSPConfig->szName);

    return (npa_resource_state)pRail->eCorner;
  }

  /*-----------------------------------------------------------------------*/
  /* Do nothing for init.                                                  */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    return pRail->eCornerInit;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the driver context.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = VCS_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* If any non-HW disable DVS bit is set then set the rail to max.        */
  /*-----------------------------------------------------------------------*/

  if (pRail->nDisableDVS & ~VCS_FLAG_DISABLED_HW_LOGIC)
  {
    eCornerAggregated = pRail->eCornerMax;
  }

  /*-----------------------------------------------------------------------*/
  /* Error if the voltage corner exceeds the current max corner.           */
  /*-----------------------------------------------------------------------*/

  if (eCornerAggregated > pRail->eCornerMax)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Request[%lu] on rail[%s] above max corner[%lu]",
      nState,
      pRail->pBSPConfig->szName,
      pRail->eCornerMax);

    return (npa_resource_state)pRail->eCorner;
  }

  /*-----------------------------------------------------------------------*/
  /* Round the request up within the supported range.                      */
  /*-----------------------------------------------------------------------*/

  eCornerAggregated = MAX(eCornerAggregated, pRail->eCornerMin);

    /*
     * Validate the corner
     */
    if(eCornerAggregated >= VCS_CORNER_NUM_OF_CORNERS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Invalid corner [%lu] on rail[%s]",
        eCornerAggregated,
        pRail->pBSPConfig->szName);

      return (npa_resource_state)pRail->eCorner;
    }

  /*-----------------------------------------------------------------------*/
  /* If a voltage table is present then find the min supported corner that */
  /* is greater than a equal to the request state.                         */
  /*-----------------------------------------------------------------------*/

  if (pRail->pVoltageTable)
  {
    for ( ; eCornerAggregated <= pRail->eCornerMax; eCornerAggregated++)
    {
      if (pRail->pVoltageTable->apCornerMap[eCornerAggregated] != NULL)
      {
        break;
      }
    }

    /*
     * Validate that we found a usable entry in the voltage table.
     */
    if (eCornerAggregated > pRail->eCornerMax)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Request[%lu] on rail[%s] above max corner[%lu]",
        nState,
        pRail->pBSPConfig->szName,
        pRail->eCornerMax);

      return (npa_resource_state)pRail->eCorner;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Handle the case where request on rail increased.                      */
  /*-----------------------------------------------------------------------*/

  if (eCornerAggregated > pResource->active_state)
  {
    /*
     * Satisfy rail dependency.
     */
    VCS_NPASatisfyRailDependencies(pRail, pResource, hClient);

    /*
     * Increase the rail voltage to requested corner.
     */
    if (pRail->fpSetRailCorner != NULL)
    {
      eResult = pRail->fpSetRailCorner(pRail, eCornerAggregated);
      if (eResult != DAL_SUCCESS)
      {
        DALSYS_LogEvent(
          0,
          DALSYS_LOGEVENT_FATAL_ERROR,
          "DALLOG Device VCS: Unable to set rail[%s] to voltage[%lu]",
          pRail->pBSPConfig->szName,
          eCornerAggregated);

        return (npa_resource_state)pRail->eCorner;
      }
    }

    /*
     * Switch CPU's to LDO mode if possible.
     */
    for (i = 0; i < pRail->nNumCPUs; i++)
    {
      pCPU = pRail->apCPU[i];
      pLDO = pCPU->pLDO;

      /*
       * Check if the LDO can now be enabled.
       */
      if (pLDO != NULL)
      {
        eResult = VCS_SetLDOCorner(pLDO, pLDO->eCorner, eCornerAggregated);
        if (eResult != DAL_SUCCESS)
        {
          DALSYS_LogEvent(
            0,
            DALSYS_LOGEVENT_FATAL_ERROR,
            "DALLOG Device VCS: Unable to set LDO[%s] to voltage[%lu]",
            pLDO->pBSPConfig->szName,
            eCornerAggregated);

          return (npa_resource_state)pRail->eCorner;
        }
      }
    } 
  }

  /*-----------------------------------------------------------------------*/
  /* Handle the case where request on rail decreased.                      */
  /*-----------------------------------------------------------------------*/

  else if (eCornerAggregated < pResource->active_state)
  { 
    /*
     * Switch CPU's to BHS mode if required.
     */
    for (i = 0; i < pRail->nNumCPUs; i++)
    {
      pCPU = pRail->apCPU[i];
      pLDO = pCPU->pLDO;

      /*
       * Check if the LDO must now be disabled..
       */
      if (pLDO != NULL)
      {
        eResult = VCS_SetLDOCorner(pLDO, pLDO->eCorner, eCornerAggregated);
        if (eResult != DAL_SUCCESS)
        {
          DALSYS_LogEvent(
            0,
            DALSYS_LOGEVENT_FATAL_ERROR,
            "DALLOG Device VCS: Unable to set LDO[%s] to voltage[%lu]",
            pLDO->pBSPConfig->szName,
            eCornerAggregated);

          return (npa_resource_state)pRail->eCorner;
        }
      }
    }

    /*
     * Decrease the rail voltage to requested corner.
     */
    if (pRail->fpSetRailCorner != NULL)
    {
      eResult = pRail->fpSetRailCorner(pRail, eCornerAggregated);
      if (eResult != DAL_SUCCESS)
      {
        DALSYS_LogEvent(
          0,
          DALSYS_LOGEVENT_FATAL_ERROR,
          "DALLOG Device VCS: Unable to set rail[%s] to voltage[%lu]",
          pRail->pBSPConfig->szName,
          eCornerAggregated);

        return (npa_resource_state)pRail->eCorner;
      }
    }

    /*
     * Satisfy rail dependency.
     */
    VCS_NPASatisfyRailDependencies(pRail, pResource, hClient);
  }

  /*-----------------------------------------------------------------------*/
  /* Handle the case where an impulse wants to force a voltage update.     */
  /*-----------------------------------------------------------------------*/

  else if (hClient->type == NPA_CLIENT_IMPULSE)
  {
    if (pRail->fpSetRailCorner != NULL)
    {
      eResult = pRail->fpSetRailCorner(pRail, eCornerAggregated);
      if (eResult != DAL_SUCCESS)
      {
        DALSYS_LogEvent(
          0,
          DALSYS_LOGEVENT_FATAL_ERROR,
          "DALLOG Device VCS: Unable to set rail[%s] to voltage[%lu]",
          pRail->pBSPConfig->szName,
          eCornerAggregated);

        return (npa_resource_state)pRail->eCorner;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Handle the case where the overall aggregated state didn't change but  */
  /* the aggregated votes for either required or suppressible alone have   */
  /* changed and thus we must update the dependency accordingly.           */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * Satisfy rail dependency.
     */
    VCS_NPASatisfyRailDependencies(pRail, pResource, hClient);
  }

  /*-----------------------------------------------------------------------*/
  /* Log the voltage corner switch.                                        */
  /*-----------------------------------------------------------------------*/

  if (eCornerAggregated != pRail->eCorner)
  {
    if (pRail->nVoltageUV)
    {
      ULOG_RT_PRINTF_4(
        pDrvCtxt->hVCSLog,
        "Rail[%s] Corner[%s -> %s] Voltage[%lu]",
        pRail->pBSPConfig->szName,
        pDrvCtxt->aszCornerNameMap[pRail->eCorner],
        pDrvCtxt->aszCornerNameMap[eCornerAggregated],
        pRail->nVoltageUV);
    }
    else
    {
      ULOG_RT_PRINTF_3(
        pDrvCtxt->hVCSLog,
        "Rail[%s] Corner[%s -> %s]",
        pRail->pBSPConfig->szName,
        pDrvCtxt->aszCornerNameMap[pRail->eCorner],
        pDrvCtxt->aszCornerNameMap[eCornerAggregated]);
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update the context.                                                   */
  /*-----------------------------------------------------------------------*/

  pRail->eCorner = eCornerAggregated;

  /*-----------------------------------------------------------------------*/
  /* Return the current rail corner.                                       */
  /*-----------------------------------------------------------------------*/

  return (npa_resource_state)pRail->eCorner;

} /* END VCS_NPARailDriverFunc */


/* =========================================================================
**  Function : VCS_NPARailNodeAvailableCallback
** =========================================================================*/
/**
  Callback after a rail node is created.

  This function is called by the NPA framework after a rail node
  is created.  The creation is delayed until all dependencies are also
  created.

  @param *pContext [in]      -- Context passed in npa_define_node_cb
  @param  nEventType [in]    -- Zero.
  @param *pNodeName [in]     -- Name of the node being created.
  @param  nNodeNameSize [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/

static void VCS_NPARailNodeAvailableCallback
(
  void         *pContext,
  unsigned int  nEventType,
  void         *pNodeName,
  unsigned int  nNodeNameSize
)
{
  uint32           i;
  VCSCPUNodeType  *pCPU;
  VCSRailNodeType *pRail;

  pRail = (VCSRailNodeType *)pContext;

  /*-----------------------------------------------------------------------*/
  /* Iterate through list of CPUs on this rail.                            */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pRail->nNumCPUs; i++)
  {
    /*
     * Create a suppressible NPA handle for each CPU to vote on this rail.
     */
    pCPU = pRail->apCPU[i];
    pCPU->NPAHandle =
      npa_create_sync_client(
        pRail->node.name, pCPU->pBSPConfig->szName, NPA_CLIENT_SUPPRESSIBLE);

    if (pCPU->NPAHandle == NULL)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to create a NPA client handle on rail for CPU.");
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Create an impulse client for this rail node.                          */
  /*-----------------------------------------------------------------------*/

  pRail->hClientImpulse =
    npa_create_sync_client(
      pRail->pBSPConfig->szName,
      pRail->pBSPConfig->szName,
      NPA_CLIENT_IMPULSE);
  if (pRail->hClientImpulse == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to create impulse client to rail[%s]",
      pRail->pBSPConfig->szName);
  }

} /* END of VCS_NPARailNodeAvailableCallback */


/* =========================================================================
**  Function : VCS_NPALDODriverFunc
** =========================================================================*/
/**
  Handle request state changes on a LDO's NPA resource.

  @param *pResource [in] -- Pointer to rail resource.
  @param  hClient [in]   -- Client handle.
  @param  nState [in]    -- New request state.

  @return
  New resource state.

  @dependencies
  None.
*/

static npa_resource_state VCS_NPALDODriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  return (npa_resource_state)0;
}


/* =========================================================================
**  Function : VCS_NPALDONodeAvailableCallback
** =========================================================================*/
/**
  Callback after a LDO node is created.

  This function is called by the NPA framework after a LDO node
  is created.

  @param *pContext [in]      -- Context passed in npa_define_node_cb
  @param  nEventType [in]    -- Zero.
  @param *pNodeName [in]     -- Name of the node being created.
  @param  nNodeNameSize [in] -- Length of the name.

  @return
  None.

  @dependencies
  None.
*/

static void VCS_NPALDONodeAvailableCallback
(
  void        *pContext,
  unsigned int nEventType,
  void        *pNodeName,
  unsigned int nNodeNameSize
)
{
  return;
}


/* =========================================================================
**  Function : VCS_InitNPAResourceLDO
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitNPAResourceLDO
(
  VCSCPUNodeType *pCPU
)
{
  npa_resource_state  nInitialState;
  VCSLDONodeType     *pLDO;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pCPU == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  pLDO = pCPU->pLDO;

  /*-----------------------------------------------------------------------*/
  /* Check if no LDO to perform on.                                        */
  /*-----------------------------------------------------------------------*/

  if (pLDO == NULL)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Populate resource.                                                    */
  /*-----------------------------------------------------------------------*/

  pLDO->resource.name       = pLDO->pBSPConfig->szName;
  pLDO->resource.units      = VCS_NPA_UNITS_RAIL;
  pLDO->resource.max        = 1;
  pLDO->resource.plugin     = &npa_binary_plugin;
  pLDO->resource.attributes = NPA_RESOURCE_DEFAULT;
  pLDO->resource.data       = (npa_user_data)pLDO;

  /*-----------------------------------------------------------------------*/
  /* Populate node information.                                            */
  /*-----------------------------------------------------------------------*/

  pLDO->node.name             = pLDO->pBSPConfig->szName;
  pLDO->node.driver_fcn       = VCS_NPALDODriverFunc;
  pLDO->node.attributes       = NPA_NODE_DEFAULT;
  pLDO->node.data             = (npa_user_data)pLDO;
  pLDO->node.dependency_count = 0;
  pLDO->node.dependencies     = NULL;
  pLDO->node.resource_count   = 1;
  pLDO->node.resources        = &pLDO->resource;

  /*-----------------------------------------------------------------------*/
  /* Get the resource's initial state.                                     */
  /*-----------------------------------------------------------------------*/

  nInitialState = (npa_resource_state)0;

  /*-----------------------------------------------------------------------*/
  /* Define the rail NPA node.                                             */
  /*-----------------------------------------------------------------------*/

  npa_define_node_cb(
    &pLDO->node,
    &nInitialState,
    VCS_NPALDONodeAvailableCallback,
    pLDO);

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : VCS_InitNPAResourceRail
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitNPAResourceRail
(
  VCSRailNodeType *pRail
)
{
  npa_resource_state   nInitialState;
  npa_node_dependency *pDependency;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers.                                                    */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Populate resource.                                                    */
  /*-----------------------------------------------------------------------*/

  pRail->resource.name       = pRail->pBSPConfig->szName;
  pRail->resource.units      = VCS_NPA_UNITS_RAIL;
  pRail->resource.max        = VCS_CORNER_MAX;
  pRail->resource.plugin     = &npa_max_plugin;
  pRail->resource.attributes = NPA_RESOURCE_DEFAULT;
  pRail->resource.data       = (npa_user_data)pRail;
  pRail->resource.query_fcn  = VCS_NPARailResourceQuery;

  /*-----------------------------------------------------------------------*/
  /* We have two dependency handles for each rail.                         */
  /* Example: /vdd/cx depends on /pmic/client/rail_cx, and needs both a    */
  /*          required and suppressible to it.                             */
  /*-----------------------------------------------------------------------*/

  pDependency = &pRail->aDependency[VCS_NPA_RAIL_DEP_IDX_REQ];
  pDependency->name = pRail->pBSPConfig->szNameDependency;
  pDependency->client_type = NPA_CLIENT_REQUIRED;

  pDependency = &pRail->aDependency[VCS_NPA_RAIL_DEP_IDX_SUP];
  pDependency->name = pRail->pBSPConfig->szNameDependency;
  pDependency->client_type = NPA_CLIENT_SUPPRESSIBLE;

  /*-----------------------------------------------------------------------*/
  /* Populate node information.                                            */
  /*-----------------------------------------------------------------------*/

  pRail->node.name             = pRail->pBSPConfig->szName;
  pRail->node.driver_fcn       = VCS_NPARailDriverFunc;
  pRail->node.attributes       = NPA_NODE_DEFAULT;
  pRail->node.data             = (npa_user_data)pRail;
  pRail->node.dependency_count = VCS_NPA_RAIL_NUM_DEPENDENCIES;
  pRail->node.dependencies     = pRail->aDependency;
  pRail->node.resource_count   = 1;
  pRail->node.resources        = &pRail->resource;

  /*-----------------------------------------------------------------------*/
  /* Get the resource's initial state.                                     */
  /*-----------------------------------------------------------------------*/

  nInitialState = (npa_resource_state)pRail->eCorner;

  /*-----------------------------------------------------------------------*/
  /* Define the rail NPA node.                                             */
  /*-----------------------------------------------------------------------*/

  npa_define_node_cb(
    &pRail->node,
    &nInitialState,
    VCS_NPARailNodeAvailableCallback,
    pRail);

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : VCS_InitNPAResources
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitNPAResources
(
  VCSDrvCtxt *pDrvCtxt
)
{
  uint32           i;
  DALResult        eResult;

  /*-----------------------------------------------------------------------*/
  /* Iterate through all the CPUs to define LDO resources.                 */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumCPUs; i++)
  {
    eResult = VCS_InitNPAResourceLDO(&pDrvCtxt->aCPUs[i]);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to create NPA resource for rail.");

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Iterate through all the rails to define rail resources.               */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumRails; i++)
  {
    eResult = VCS_InitNPAResourceRail(&pDrvCtxt->aRails[i]);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to create NPA resource for rail.");

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;
}
