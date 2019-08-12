/*
===========================================================================
*/
/**
  @file VCSRails.c 
  
  Rail-related functions for the VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/src/VCSRails.c#1 $
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

#include "VCSDriver.h"
#include "DALDeviceId.h"
#include <DALSys.h>


/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_InitRailVoltageTable
** =========================================================================*/
/**
  Initializes the voltage table for this HW version.

  @param pRail [in]         -- Pointer rail node.
  @param pCornerConfig [in] -- Pointer rail corner config for this HW version.
  @return
  DAL_ERROR if a voltage table not initialized, other DAL_SUCCESS.

  @dependencies
  None.
*/

static DALResult VCS_InitRailVoltageTable
(
  VCSRailNodeType         *pRail,
  VCSRailCornerConfigType *pCornerConfig
)
{
  DALResult                  eResult;
  uint32                     i, nSize;
  VCSCornerVoltageRangeType *pBSPVoltageRange, *pRailVoltageRange;
  VCSRailVoltageTableType   *pVoltageTable;
  VCSDrvCtxt                *pDrvCtxt;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pCornerConfig == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Nothing to do for rails without voltage ranges.                       */
  /*-----------------------------------------------------------------------*/

  if (pCornerConfig->nNumVoltageRanges == 0)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the number of voltage corners in the BSP.                    */
  /*-----------------------------------------------------------------------*/

  if (pCornerConfig->nNumVoltageRanges >= VCS_CORNER_NUM_OF_CORNERS ||
      pCornerConfig->pVoltageRange == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the current corner                                           */
  /*-----------------------------------------------------------------------*/

  if(pRail->eCorner >= VCS_CORNER_NUM_OF_CORNERS)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the driver context.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = VCS_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for rail voltage table.                              */
  /*-----------------------------------------------------------------------*/

  nSize = sizeof(*pRail->pVoltageTable);

  eResult = DALSYS_Malloc(nSize, (void **)&pRail->pVoltageTable);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to allocate %lu bytes for rail[%s].",
      nSize,
      pRail->pBSPConfig->szName);

    return eResult;
  }

  memset((void *)pRail->pVoltageTable, 0x0, nSize);

  /*-----------------------------------------------------------------------*/
  /* Allocate storage for corner voltage ranges.                           */
  /*-----------------------------------------------------------------------*/

  pVoltageTable = pRail->pVoltageTable;
  pVoltageTable->nNumVoltageRanges = pCornerConfig->nNumVoltageRanges;

  nSize =
    pVoltageTable->nNumVoltageRanges * sizeof(*pVoltageTable->pVoltageRange);

  eResult = DALSYS_Malloc(nSize, (void **)&pVoltageTable->pVoltageRange);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to allocate %lu bytes for rail[%s].",
      nSize,
      pRail->pBSPConfig->szName);

    return eResult;
  }

  memset((void *)pVoltageTable->pVoltageRange, 0x0, nSize);

  /*-----------------------------------------------------------------------*/
  /* Populate corner voltage table.                                        */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pVoltageTable->nNumVoltageRanges; i++)
  {
    // Copy corner voltage table from BSP
    pBSPVoltageRange  = &pCornerConfig->pVoltageRange[i];
    pRailVoltageRange = &pVoltageTable->pVoltageRange[i];

    if (pBSPVoltageRange->eCorner >= VCS_CORNER_NUM_OF_CORNERS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Invalid corner[%lu] for rail[%s] specified in BSP",
        pBSPVoltageRange->eCorner,
        pRail->pBSPConfig->szName);

      return DAL_ERROR_INTERNAL;
    }

    pRailVoltageRange->eCorner = pBSPVoltageRange->eCorner;
    pRailVoltageRange->nMinUV  = pBSPVoltageRange->nMinUV;
    pRailVoltageRange->nMaxUV  = pBSPVoltageRange->nMaxUV;

    /*
     * Hook up corner mapping.
     */
      pVoltageTable->apCornerMap[pRailVoltageRange->eCorner] = pRailVoltageRange;
    }

  /*
   * Map the raw voltage.
   */
  pRail->nVoltageUV = pVoltageTable->apCornerMap[pRail->eCorner]->nMaxUV;

  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : VCS_DetectRailBSPVersion
** =========================================================================*/
/**
 *
  See VCSDriver.h
*/

DALResult VCS_DetectRailBSPVersion
(
  VCSRailNodeType          *pRail,
  VCSRailCornerConfigType **pCornerConfig
)
{
  uint32                   i;
  VCSRailCornerConfigType *pIteratorCornerConfig;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || pCornerConfig == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize return parameter.                                          */
  /*-----------------------------------------------------------------------*/

  *pCornerConfig = NULL;

  /*-----------------------------------------------------------------------*/
  /* Detect which BSP data to use for this HW version.                     */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pRail->pBSPConfig->nNumCornerConfigs; i++)
  {
    pIteratorCornerConfig = &pRail->pBSPConfig->pCornerConfig[i];
    if (VCS_IsBSPSupported(&pIteratorCornerConfig->HWVersion))
    {
      *pCornerConfig = pIteratorCornerConfig;
      break;
    }
  }

  if (*pCornerConfig == NULL)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : VCS_InitRails
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitRails
(
  VCSDrvCtxt       *pDrvCtxt,
  VCSBSPConfigType *pBSPConfig
)
{
  uint32                   i, nSize;
  DALResult                eResult;
  VCSRailNodeType         *pRail;
  VCSRailCornerConfigType *pCornerConfig;

  /*-----------------------------------------------------------------------*/
  /* Make sure we found at least one rail.                                 */
  /*-----------------------------------------------------------------------*/

  if (pBSPConfig->nNumRailConfigs == 0)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: VCS_InitRails found 0 rails.");

    return DAL_ERROR;
  }

  pDrvCtxt->nNumRails = pBSPConfig->nNumRailConfigs;

  /*-----------------------------------------------------------------------*/
  /* Allocate memory for the rail nodes.                                   */
  /*-----------------------------------------------------------------------*/

  nSize = pDrvCtxt->nNumRails * sizeof (*pDrvCtxt->aRails);

  eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aRails);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to allocate %lu bytes for %lu rails.",
      nSize,
      pDrvCtxt->nNumRails);

    return eResult;
  }

  memset((void *)pDrvCtxt->aRails, 0x0, nSize);

  /*-----------------------------------------------------------------------*/
  /* Initialize the rail nodes.                                            */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pDrvCtxt->nNumRails; i++)
  {
    pRail = &pDrvCtxt->aRails[i];

    /*
     * Link the rail node to its BSP config data.
     */
    pRail->pBSPConfig = &pBSPConfig->pRailConfig[i];

    /*
     * Detect the corner config for this HW version.
     */
    eResult = VCS_DetectRailBSPVersion(pRail, &pCornerConfig);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to detect a valid BSP version.");

      return eResult;
    }

    /*
     * Store entry in Rail enum to node map
     */
    pDrvCtxt->apRailMap[pRail->pBSPConfig->eRail] = pRail;

    /*
     * Store rail properties.
     */
    pRail->eRail       = pRail->pBSPConfig->eRail;
    pRail->eCornerMin  = pCornerConfig->eCornerMin;
    pRail->eCornerMax  = pCornerConfig->eCornerMax;
    pRail->eCornerInit = pCornerConfig->eCornerInit;
    pRail->eMode       = VCS_RAIL_MODE_CPR;

    /*
     * Do not invoke CPR until it completes initialization.
     */
    pRail->nDisableCPR = VCS_FLAG_DISABLED_BY_CPR;

    /*
     * Explicitly disable per the BSP configuration
     */
    if (!pRail->pBSPConfig->bEnableCPR)
    {
      pRail->nDisableCPR |= VCS_FLAG_DISABLED_BY_BSP;
    }

    if (!pRail->pBSPConfig->bEnableDVS)
    {
      pRail->nDisableDVS |= VCS_FLAG_DISABLED_BY_BSP;
    }

    /*
     * Set current rail corner to initial corner.
     */
    pRail->eCorner = pRail->eCornerInit;

    /*
     * Initialize the voltage table
     */
    eResult = VCS_InitRailVoltageTable(pRail, pCornerConfig);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to initialize voltage table for rail[%s]",
        pRail->pBSPConfig->szName);

      return eResult;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the corner map from VCS to PMIC.                           */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->pnCornerPMICMap = pBSPConfig->pnCornerPMICMap;

  return DAL_SUCCESS;

} /* END VCS_InitRails */

