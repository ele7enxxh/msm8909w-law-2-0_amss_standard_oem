/*
===========================================================================
*/
/**
  @file VCSCPU.c 
  
  CPU-related functions for the VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/src/VCSCPU.c#1 $
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


#include <DALSys.h>
#include "VCSDriver.h"
#include "HALldo.h"
#include "DALDeviceId.h"
#include <npa_resource.h>


/*=========================================================================
      Externs
==========================================================================*/


/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_InitLDOVoltageTable
** =========================================================================*/
/**
  Initializes the voltage table for this HW version.

  @param pLDO [in]         -- Pointer rail node.
  @param pCornerConfig [in] -- Pointer rail corner config for this HW version.
  @return
  DAL_ERROR if a voltage table not initialized, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult VCS_InitLDOVoltageTable
(
  VCSLDONodeType   *pLDO,
  VCSLDOConfigType *pLDOConfig

)
{
  DALResult               eResult;
  uint32                  i, nSize;
  VCSCornerVoltageType   *pBSPVoltage, *pLDOVoltage;
  VCSLDOVoltageTableType *pVoltageTable;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pLDO == NULL || pLDOConfig == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }


  /*-----------------------------------------------------------------------*/
  /* Validate the BSP config.                                              */
  /*-----------------------------------------------------------------------*/

  if (pLDOConfig->nNumCornerVoltages == 0 ||
      pLDOConfig->nNumCornerVoltages >= VCS_CORNER_NUM_OF_CORNERS ||
      pLDOConfig->pCornerVoltage == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for LDO voltage table.                               */
  /*-----------------------------------------------------------------------*/

  nSize = sizeof (*pLDO->pVoltageTable);

  eResult = DALSYS_Malloc(nSize, (void **)&pLDO->pVoltageTable);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  memset((void *)pLDO->pVoltageTable, 0x0, nSize);


  /*-----------------------------------------------------------------------*/
  /* Allocate storage for corner voltages.                                 */
  /*-----------------------------------------------------------------------*/

  pVoltageTable = pLDO->pVoltageTable;
  pVoltageTable->nNumCornerVoltages = pLDOConfig->nNumCornerVoltages;

  nSize =
    pVoltageTable->nNumCornerVoltages * sizeof (*pVoltageTable->pCornerVoltage);

  eResult = DALSYS_Malloc(nSize, (void **)&pVoltageTable->pCornerVoltage);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Populate corner voltage table.                                        */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pVoltageTable->nNumCornerVoltages; i++)
  {
    // Copy corner voltage table from BSP
    pBSPVoltage = pLDOConfig->pCornerVoltage;
    pLDOVoltage = pVoltageTable->pCornerVoltage;

    pLDOVoltage[i].eCorner    = pBSPVoltage[i].eCorner;
    pLDOVoltage[i].nVoltageUV = pBSPVoltage[i].nVoltageUV;
  }

  /*-----------------------------------------------------------------------*/
  /* Populate corner voltage table.                                        */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < ARR_SIZE(pVoltageTable->apCornerMap); i++)
  {
    pVoltageTable->apCornerMap[i] = NULL;
  }

  for (i = 0; i < pVoltageTable->nNumCornerVoltages; i++)
  {
    pLDOVoltage = &pVoltageTable->pCornerVoltage[i];
    pVoltageTable->apCornerMap[pLDOVoltage->eCorner] = pLDOVoltage;
  }

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : VCS_InitLDO
** =========================================================================*/
/**
  Initializes the LDO structures for the specified CPU.

  @param pCPU [in] -- Pointer to CPU node.
  @return
  DAL_ERROR if LDO couldn't be initialized, other DAL_SUCCESS.

  @dependencies
  None.
*/
static DALResult VCS_InitLDO
(
  VCSCPUNodeType *pCPU
)
{
  DALResult               eResult;
  uint32                  nSize;
  VCSLDONodeType         *pLDO;
  VCSLDOConfigType       *pLDOConfig;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pCPU == NULL ||
      pCPU->pBSPConfig == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pLDOConfig = pCPU->pBSPConfig->pLDOConfig;

  /*-----------------------------------------------------------------------*/
  /* Nohting to do if no LDO present.                                      */
  /*-----------------------------------------------------------------------*/

  if (pLDOConfig == NULL)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Allocate memory for LDO node.                                         */
  /*-----------------------------------------------------------------------*/

  nSize = sizeof (*pCPU->pLDO);

  eResult = DALSYS_Malloc(nSize, (void **)&pCPU->pLDO);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  memset((void *)pCPU->pLDO, 0x0, nSize);

  /*-----------------------------------------------------------------------*/
  /* Store LDO properties.                                                 */
  /*-----------------------------------------------------------------------*/

  pLDO = pCPU->pLDO;
  pLDO->eLDO             = pLDOConfig->eLDO;
  pLDO->eCorner          = VCS_CORNER_OFF;
  pLDO->nVoltageUV       = 0;
  pLDO->nNumRestrictions = 0;
  pLDO->pBSPConfig       = pLDOConfig;

  /*-----------------------------------------------------------------------*/
  /* Explicitly disable per the BSP configuration.                         */
  /*-----------------------------------------------------------------------*/

  if (!pLDOConfig->bEnable)
  {
    pLDO->nDisable |= VCS_FLAG_DISABLED_BY_BSP;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the LDO voltage table.                                     */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitLDOVoltageTable(pLDO, pLDOConfig);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : VCS_InitCPUs
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitCPUs
(
  VCSDrvCtxt       *pDrvCtxt,
  VCSBSPConfigType *pBSPConfig
)
{
  DALResult         eResult;
  uint32            i, j, k, nSize;
  VCSCPUNodeType   *pCPU;
  VCSRailNodeType  *pRail;

  /*-----------------------------------------------------------------------*/
  /* Nothing to do if no CPUs present.                                     */
  /*-----------------------------------------------------------------------*/

  if (pBSPConfig->nNumCPUConfigs == 0)
  {
    return DAL_SUCCESS;
  }

  pDrvCtxt->nNumCPUs = pBSPConfig->nNumCPUConfigs;

  /*-----------------------------------------------------------------------*/
  /* Allocate memory for the CPU nodes.                                    */
  /*-----------------------------------------------------------------------*/

  nSize = pDrvCtxt->nNumCPUs * sizeof (*pDrvCtxt->aCPUs);

  eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aCPUs);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to allocate %lu bytes for %lu CPUs.",
      nSize,
      pDrvCtxt->nNumCPUs);

    return eResult;
  }

  memset((void *)pDrvCtxt->aCPUs, 0x0, nSize);

  /*-----------------------------------------------------------------------*/
  /* Initialize the CPU nodes.                                             */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumCPUs; i++)
  {
    pCPU = &pDrvCtxt->aCPUs[i];

    /*
     * Link the BSP data.`
     */
    pCPU->pBSPConfig = &pBSPConfig->pCPUConfig[i];

    /*
     * Save CPU enum.
     */
    pCPU->eCPU = pCPU->pBSPConfig->eCPU;

    /*
     * Create entry in CPU enum to node map.
     */
    pDrvCtxt->apCPUMap[pCPU->eCPU] = pCPU;

    /*
     * Increment the count for the rail powering this CPU.
     */
    pRail = pDrvCtxt->apRailMap[pCPU->pBSPConfig->eRail];
    if (pRail == NULL)
    {
      return DAL_ERROR_INTERNAL;
    }

    pRail->nNumCPUs++;

    /*
     * Init the LDO if present for this CPU
     */
    eResult = VCS_InitLDO(pCPU);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Unable to initialize LDO for CPU[%lu].",
        pCPU->eCPU);

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Connect the Rails to each of the CPUs which it powers.                */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumRails; i++)
  {
    pRail = &pDrvCtxt->aRails[i];

    /*
     * Skip this rail if there are no CPU's to map.
     */
    if (pRail->nNumCPUs == 0)
    {
      continue;
    }

    /*
     * Allocate storage for pointers to CPU nodes.
     */
    nSize = pRail->nNumCPUs * sizeof (*pRail->apCPU);

    eResult = DALSYS_Malloc(nSize, (void **)&pRail->apCPU);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Unable to allocate %lu bytes for %lu CPUs.",
        nSize,
        pDrvCtxt->nNumCPUs);

      return eResult;
    }

    memset((void *)pRail->apCPU, 0x0, nSize);

    /*
     * Iterate through CPU list and store pointers to those on this rail.
     */
    for (j = 0, k = 0; j < pRail->nNumCPUs; j++, k++)
    {
      for ( ; k < pDrvCtxt->nNumCPUs; k++)
      {
        pCPU = &pDrvCtxt->aCPUs[j];
        if (pCPU->pBSPConfig->eRail == pRail->eRail)
        {
          pRail->apCPU[j] = pCPU;
          break;
        }
      }
    }
  }

  return DAL_SUCCESS;
}


static void VCS_EnableLDO
(
  VCSLDONodeType *pLDO,
  VCSCornerType   eCorner
)
{
  VCSCornerVoltageType *pCornerVoltage;

  /*-----------------------------------------------------------------------*/
  /* Set the LDO voltage.                                                  */
  /*-----------------------------------------------------------------------*/

  pCornerVoltage = pLDO->pVoltageTable->apCornerMap[eCorner];
  HAL_ldo_SetLDOVoltage(pLDO->eLDO, pCornerVoltage->nVoltageUV);

  /*-----------------------------------------------------------------------*/
  /* Switch the CPU to the LDO.                                            */
  /*-----------------------------------------------------------------------*/

  if (pLDO->eCorner == VCS_CORNER_OFF)
  {
    HAL_ldo_EnableLDO(pLDO->eLDO);
  }

  /*-----------------------------------------------------------------------*/
  /* Update the context.                                                   */
  /*-----------------------------------------------------------------------*/

  pLDO->eCorner    = eCorner;
  pLDO->nVoltageUV = pCornerVoltage->nVoltageUV;

  /*-----------------------------------------------------------------------*/
  /* Broadcast state change.                                               */
  /*-----------------------------------------------------------------------*/

  // TODO Invoke NPA API
}


static void VCS_DisableLDO
(
  VCSLDONodeType *pLDO
)
{
  /*-----------------------------------------------------------------------*/
  /* Check if already disabled.                                            */
  /*-----------------------------------------------------------------------*/

  if (pLDO->eCorner == VCS_CORNER_OFF)
  {
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Switch to BHS.                                                        */
  /*-----------------------------------------------------------------------*/

  HAL_ldo_DisableLDO(pLDO->eLDO);

  /*-----------------------------------------------------------------------*/
  /* Update the context.                                                   */
  /*-----------------------------------------------------------------------*/

  pLDO->eCorner = VCS_CORNER_OFF;
  pLDO->nVoltageUV = 0;

  /*-----------------------------------------------------------------------*/
  /* Broadcast state change.                                               */
  /*-----------------------------------------------------------------------*/

  // TODO Invoke NPA API
}


/* =========================================================================
**  Function : VCS_SetLDOCorner
** =========================================================================*/
/*
  See VCSDriver.h
*/
  
DALResult VCS_SetLDOCorner
(
  VCSLDONodeType *pLDO,
  VCSCornerType   eLDOCorner,
  VCSCornerType   eRailCorner
)
{
  /*-----------------------------------------------------------------------*/
  /* Validate pointer.                                                     */
  /*-----------------------------------------------------------------------*/

  if (pLDO == NULL ||
      eLDOCorner >= VCS_CORNER_NUM_OF_CORNERS ||
      eRailCorner >= VCS_CORNER_NUM_OF_CORNERS)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Short circuit if no change in LDO corner.                             */
  /*-----------------------------------------------------------------------*/

  if (pLDO->eCorner == eLDOCorner)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Enable the LDO if the corner is supported and enough headroom exists. */
  /*-----------------------------------------------------------------------*/

  if (pLDO->pVoltageTable->apCornerMap[eLDOCorner] != NULL &&
      eLDOCorner + pLDO->pBSPConfig->nHeadroom <= eRailCorner)
  {
    VCS_EnableLDO(pLDO, eLDOCorner);
  }
  else
  {
    VCS_DisableLDO(pLDO);
  }

  /*-----------------------------------------------------------------------*/
  /* Switch to LDO if enough headroom.                                     */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : VCS_SetCPUCorner
** =========================================================================*/
/*
  See DDIVCS.h
*/

DALResult VCS_SetCPUCorner
(
  VCSDrvCtxt   *pDrvCtxt,
  ClockCPUType  eCPU,
  VCSCornerType eCornerRequest
)
{
  DALResult        eResult;
  VCSRailNodeType *pRail;
  VCSCPUNodeType  *pCPU;
  VCSCornerType    eCornerStart;


  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (eCPU >= CLOCK_CPU_NUM_OF_CPUS ||
      eCornerRequest >= VCS_CORNER_NUM_OF_CORNERS)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Verify the CPU is supported.                                          */
  /*-----------------------------------------------------------------------*/

  pCPU = pDrvCtxt->apCPUMap[eCPU];
  if (pCPU == NULL)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  /*-----------------------------------------------------------------------*/
  /* Short circuit if the new request is same as current.                  */
  /*-----------------------------------------------------------------------*/

  if (pCPU->eCorner == eCornerRequest)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Get handle to rail node.                                              */
  /*-----------------------------------------------------------------------*/

  pRail = pDrvCtxt->apRailMap[pCPU->pBSPConfig->eRail];
  if (pRail == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Protect operation on rail with a critical section.                    */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(pRail->resource.handle);

  /*-----------------------------------------------------------------------*/
  /* Keep track of the rail corner pre-CPU vote.                           */
  /*-----------------------------------------------------------------------*/

  eCornerStart = pRail->eCorner;

  /*-----------------------------------------------------------------------*/
  /* Set request attribute to fire-and-forget if we are setting a lower    */
  /* voltage corner.                                                       */
  /*-----------------------------------------------------------------------*/

  if(eCornerRequest < pCPU->eCorner)
  {
    npa_set_request_attribute(pCPU->NPAHandle, NPA_REQUEST_FIRE_AND_FORGET);
  }

  /*-----------------------------------------------------------------------*/
  /* Make a corner request with the provided NPA handle.                   */
  /*-----------------------------------------------------------------------*/

  npa_issue_scalar_request(pCPU->NPAHandle, eCornerRequest);

  /*-----------------------------------------------------------------------*/
  /* If the rail corner didn't change after the CPU vote then it means     */
  /* that the CPU is <= the aggregated rail vote.  In this case the driver */
  /* function for the rail would not have been invoked and we must take    */
  /* care to update the LDO state directly.                                */
  /*-----------------------------------------------------------------------*/

  if (pRail->eCorner == eCornerStart &&
      pCPU->pLDO && !pCPU->pLDO->nDisable)
  {
    eResult = VCS_SetLDOCorner(pCPU->pLDO, eCornerRequest, pRail->eCorner);
    if (eResult != DAL_SUCCESS)
    {
      npa_resource_unlock(pRail->resource.handle);

      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to update LDO corner.");

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update context.                                                       */
  /*-----------------------------------------------------------------------*/

  pCPU->eCorner = eCornerRequest;

  /*-----------------------------------------------------------------------*/
  /* Release lock.                                                         */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(pRail->resource.handle);

  return DAL_SUCCESS;

}


/* =========================================================================
**  Function : VCS_AddLDORestriction
** =========================================================================*/
/*
  See DDIVCS.h
*/

DALResult VCS_AddLDORestriction
(
  VCSDrvCtxt   *pDrvCtxt,
  ClockCPUType  eCPU
)
{
  VCSCPUNodeType  *pCPU;
  VCSRailNodeType *pRail;
  VCSLDONodeType  *pLDO;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  pCPU = pDrvCtxt->apCPUMap[eCPU];
  if (pCPU == NULL || pCPU->pLDO == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pLDO = pCPU->pLDO;

  /*-----------------------------------------------------------------------*/
  /* Get pointer to the rail node.                                         */
  /*-----------------------------------------------------------------------*/

  pRail = pDrvCtxt->apRailMap[pCPU->pBSPConfig->eRail];
  if (pRail == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Protect operation on rail/ldo with a critical section.                */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(pRail->resource.handle);

  /*-----------------------------------------------------------------------*/
  /* Increment the number of restrictions on the LDO.                      */
  /*-----------------------------------------------------------------------*/

  pLDO->nNumRestrictions += 1;

  /*-----------------------------------------------------------------------*/
  /* If the LDO is already disabled, we are done.                          */
  /*-----------------------------------------------------------------------*/

  if (!(pLDO->nDisable & VCS_FLAG_DISABLED_BY_SWCLIENT))
  {
    /*
     * Disable the LDO.
     */
    VCS_DisableLDO(pLDO);

    /*
     * Update the disable flag.
     */
    pLDO->nDisable |= VCS_FLAG_DISABLED_BY_SWCLIENT;
  }

  /*-----------------------------------------------------------------------*/
  /* Release lock.                                                         */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(pRail->resource.handle);

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : VCS_RemoveLDORestriction
** =========================================================================*/
/*
  See DDIVCS.h
*/

DALResult VCS_RemoveLDORestriction
(
  VCSDrvCtxt   *pDrvCtxt,
  ClockCPUType  eCPU
)
{
  DALResult        eResult;
  VCSCPUNodeType  *pCPU;
  VCSRailNodeType *pRail;
  VCSLDONodeType  *pLDO;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  pCPU = pDrvCtxt->apCPUMap[eCPU];
  if (pCPU == NULL || pCPU->pLDO == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pLDO = pCPU->pLDO;

  /*-----------------------------------------------------------------------*/
  /* Get pointer to the rail node.                                         */
  /*-----------------------------------------------------------------------*/

  pRail = pDrvCtxt->apRailMap[pCPU->pBSPConfig->eRail];
  if (pRail == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Protect operation on rail/ldo with a critical section.                */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(pRail->resource.handle);

  /*-----------------------------------------------------------------------*/
  /* Decrement the number of restrictions on the LDO.                      */
  /*-----------------------------------------------------------------------*/

  if (pLDO->nNumRestrictions > 0)
  {
    pLDO->nNumRestrictions -= 1;
  }

  /*-----------------------------------------------------------------------*/
  /* If the number of restrictions on the LDO has gone down to zero, clear */
  /* the relevant bit in the disable flag.                                 */
  /*-----------------------------------------------------------------------*/

  if (pLDO->nDisable == 0)
  {
    pLDO->nDisable &= ~VCS_FLAG_DISABLED_BY_SWCLIENT;
  }

  /*-----------------------------------------------------------------------*/
  /* Call UpdateLDOCorner with the LDO's current request on the LDO. This  */
  /* will potentially turn ON the LDO if it is not disabled for any other  */
  /* reason, and there is enough headroom. This also handles disabling     */
  /* CPR sensors if required.                                              */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_SetLDOCorner(pLDO, pLDO->eCorner, pRail->eCorner);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Failed to update LDO corner.");
  }

  /*-----------------------------------------------------------------------*/
  /* Release lock.                                                         */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(pRail->resource.handle);

  return eResult;
}



