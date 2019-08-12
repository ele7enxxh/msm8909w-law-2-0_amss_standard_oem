/*
===========================================================================
*/
/**
  @file VCSMSS.c 
  
  Main entry point for the MSS VCS driver.
*/
/*  
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/hw/msm8909/mss/src/VCSMSS.c#1 $
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

#include "DALDeviceId.h"
#include "VCSDriver.h"
#include "VCSMSS.h"
#include "VCSMSSHWIO.h"
#include "pmapp_pwr.h"
#include "pmapp_npa.h"
#include <CoreIni.h>
//#include "cpr.h"

/*=========================================================================
      Macros
==========================================================================*/


/*
 * VCS configuration names in EFS .ini file
 * Shared with ClockDriver
 */
#define VCS_EFS_INI_FILENAME                   "/nv/item_files/clock/settings.ini"

/*
 * EFS Sections for MPSS VCS.
 */
#define VCS_EFS_MPSS_RAIL_CX_CONFIG_SECTION    "MPSS_VDDCX"
#define VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION   "MPSS_VDDMSS"

/*
 * EFS Keys for MPSS VCS.
 */
#define VCS_EFS_RAIL_DVS_FLAG                  "EnableDVS"
#define VCS_EFS_RAIL_CPR_FLAG                  "EnableCPR"
#define VCS_EFS_RAIL_MIN_CORNER                "MinCorner"
#define VCS_EFS_RAIL_MAX_CORNER                "MaxCorner"

#define VCS_EFS_RAIL_VOLTAGE_RETENTION_MIN     "RetentionMin"
#define VCS_EFS_RAIL_VOLTAGE_RETENTION_MAX     "RetentionMax"

#define VCS_EFS_RAIL_VOLTAGE_LOW_MINUS_MIN     "LowMinusMin"
#define VCS_EFS_RAIL_VOLTAGE_LOW_MINUS_MAX     "LowMinusMax"

#define VCS_EFS_RAIL_VOLTAGE_LOW_MIN           "LowMin"
#define VCS_EFS_RAIL_VOLTAGE_LOW_MAX           "LowMax"

#define VCS_EFS_RAIL_VOLTAGE_NOMINAL_MIN       "NominalMin"
#define VCS_EFS_RAIL_VOLTAGE_NOMINAL_MAX       "NominalMax"

#define VCS_EFS_RAIL_VOLTAGE_NOMINAL_PLUS_MIN  "NominalPlusMin"
#define VCS_EFS_RAIL_VOLTAGE_NOMINAL_PLUS_MAX  "NominalPlusMax"

#define VCS_EFS_RAIL_VOLTAGE_TURBO_MIN         "TurboMin"
#define VCS_EFS_RAIL_VOLTAGE_TURBO_MAX         "TurboMax"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Extern Definitions
==========================================================================*/


extern void npa_update_resource_state(npa_resource *resource, npa_resource_state new_state);


/*=========================================================================
      Function prototypes
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


VCSImageCtxtType VCS_ImageCtxt;


/* =========================================================================
      Prototypes
==========================================================================*/


/* =========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_DetectLDOEFuse
** =========================================================================*/
/**
  Read LDO Trimmed Info

  This function will read an efuse field to detect whether the LDO has
  been trimmed and is safe to enable the LDO output.

  @return
  None.

  @dependencies
  None.
*/
#if 0
static void VCS_DetectLDOEFuse
(
  VCSLDONodeType *pLDO
)
{

} /* END VCS_DetectLDOEFuse */



/* =========================================================================
**  Function : VCS_MapCornerToAccMemSel
** =========================================================================*/
/**
  Maps a MSS voltage rail corner to an ACC memory select setting.

  This function maps VDD_MSS corners to ACC settings.

  @param eCorner [in] -- /vdd/mss corner to map.

  @return
  ACC memory select setting.

  @dependencies
  None.
*/
static uint32 VCS_MapCornerToAccMemSel
(
  VCSCornerType eCorner
)
{
  switch(eCorner)
  {
    case VCS_CORNER_TURBO:
    case VCS_CORNER_NOMINAL:        return 0x0;
    case VCS_CORNER_LOW:
    case VCS_CORNER_LOW_MINUS:      return 0x1;

    default:
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Invalid /vdd/mss corner passed to mapping function.");

      /*
       * This return value is necessary for the compiler even with ERR_FATAL.
       */
      return 0;
  }

} /* END VCS_MapCornerToAccMemSel */
#endif

/* =========================================================================
**  Function : VCS_SetRailMode
** =========================================================================*/
/*
  See DDIVCS.h
*/

DALResult VCS_SetRailMode
(
  VCSDrvCtxt     *pDrvCtxt,
  VCSRailType     eRail,
  VCSRailModeType eMode
)
{
  /*-----------------------------------------------------------------------*/
  /* We only support setting VDD_MSS rail to default mode.                 */
  /*-----------------------------------------------------------------------*/

  if (eMode != VCS_RAIL_MODE_CPR)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  /*-----------------------------------------------------------------------*/
  /* The rail is always in default mode. There's nothing to be done.       */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_SetRailMode */


/* =========================================================================
**  Function : VCS_SetRailVoltage
** =========================================================================*/
/*
  See DDIVCS.h
*/

DALResult VCS_SetRailVoltage
(
  VCSDrvCtxt  *pDrvCtxt,
  VCSRailType  eRail,
  uint32       nVoltageUV
)
{
  int                        nLockResult;
  pm_err_flag_type           pm_err;
  VCSRailNodeType           *pRail;
  VCSCornerVoltageRangeType *pVoltageRange;

  /*-----------------------------------------------------------------------*/
  /* We only support setting VDD_MSS rail voltage.                         */
  /*-----------------------------------------------------------------------*/

  if (eRail != VCS_RAIL_MSS)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  /*-----------------------------------------------------------------------*/
  /* Get rail node.                                                        */
  /*-----------------------------------------------------------------------*/

  pRail = pDrvCtxt->apRailMap[eRail];
  if (pRail == NULL || pRail->pVoltageTable == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Protect operation on rail with a critical section.                    */
  /* If the lock is already taken (outstanding voltage corner switch) then */
  /* return immediately.                                                   */
  /*-----------------------------------------------------------------------*/

  nLockResult = npa_resource_trylock(pRail->resource.handle);
  if (nLockResult == -1)
  {
    return DAL_ERROR_BUSY_RESOURCE;
  }

  /*-----------------------------------------------------------------------*/
  /* Get voltage range for the current corner.                             */
  /*-----------------------------------------------------------------------*/

  pVoltageRange = pRail->pVoltageTable->apCornerMap[pRail->eCorner];
  if (pVoltageRange == NULL)
  {
    npa_resource_unlock(pRail->resource.handle);
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the requested voltage.                                       */
  /*-----------------------------------------------------------------------*/

  if (nVoltageUV < pVoltageRange->nMinUV || nVoltageUV > pVoltageRange->nMaxUV)
  {
    npa_resource_unlock(pRail->resource.handle);
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Adjust the rail voltage.                                              */
  /*-----------------------------------------------------------------------*/

  pm_err = pmapp_pwr_set_vdd_mss(nVoltageUV);
  if (pm_err != PM_ERR_FLAG__SUCCESS)
  {
    npa_resource_unlock(pRail->resource.handle);

    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: PMIC failed to update VDD_MSS.");

    return DAL_ERROR_INTERNAL;
  }

  pRail->nVoltageUV = nVoltageUV;

  /*-----------------------------------------------------------------------*/
  /* Release lock.                                                         */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(pRail->resource.handle);

  return DAL_SUCCESS;

} /* END VCS_SetRailMode */

#if 0
/* =========================================================================
**  Function : VCS_SetVDDMSSCornerCPR
** =========================================================================*/
/**
  Set VDD_MSS rail to a requested corner in CPR mode.

  @param pRail [in]   -- Pointer to the rail node.
  @param eCorner [in] -- Requested corner.

  @return
  DAL_SUCCESS -- Successfully configured VDD_MSS to requested corner.
  DAL_ERROR -- Failed to configure VDD_MSS to requested corner.

  @dependencies
  None.
*/

static DALResult VCS_SetVDDMSSCornerCPR
(
  VCSRailNodeType *pRail,
  VCSCornerType    eCorner
)
{
  pm_err_flag_type           pm_err;
  uint32                     nVoltageUV;
  VCSCornerVoltageRangeType *pVoltageRange;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || eCorner >= VCS_CORNER_NUM_OF_CORNERS)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Init the voltage to the max voltage at the specified corner.          */
  /*-----------------------------------------------------------------------*/

  pVoltageRange = pRail->pVoltageTable->apCornerMap[eCorner];
  nVoltageUV = pVoltageRange->nMaxUV;

  /*-----------------------------------------------------------------------*/
  /* If the use of CPR is enabled, we temporarily disable it and get the   */
  /* voltage recommendation for the requested corner.                      */
  /*-----------------------------------------------------------------------*/

  if(!pRail->nDisableCPR)
  {
#if 0
    eResult = CPR_Disable(pRail->eRail);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to disable CPR.");

      return eResult;
    }
    eResult =
      CPR_GetRailVoltageRecommendation(
        pRail->eRail,
        eCorner,
        &nVoltageUV);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to get CPR voltage recommendation.");

      return eResult;
    }
#endif
  }

  /*-----------------------------------------------------------------------*/
  /* Bound the CPR requested voltage if necessary.                         */
  /*-----------------------------------------------------------------------*/

  if (nVoltageUV < pVoltageRange->nMinUV)
  {
    nVoltageUV = pVoltageRange->nMinUV;
  }
  else if (nVoltageUV > pVoltageRange->nMaxUV)
  {
    nVoltageUV = pVoltageRange->nMaxUV;
  }

  /*-----------------------------------------------------------------------*/
  /* Adjust the rail voltage.                                              */
  /*-----------------------------------------------------------------------*/

  pm_err = pmapp_pwr_set_vdd_mss(nVoltageUV);
  if (pm_err != PM_ERR_FLAG__SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }

  pRail->nVoltageUV = nVoltageUV;

  /*-----------------------------------------------------------------------*/
  /* If the use of CPR is enabled, re-enable it.                           */
  /*-----------------------------------------------------------------------*/

  if(!pRail->nDisableCPR)
  {
#if 0
    eResult = CPR_Enable(pRail->eRail);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(
        0,
        DALSYS_LOGEVENT_FATAL_ERROR,
        "DALLOG Device VCS: Failed to enable CPR.");

      return eResult;
    }
#endif    
  }

  return DAL_SUCCESS;

} /* END VCS_SetVDDMSSCornerCPR */


/* =========================================================================
**  Function : VCS_SetVDDMSSCornerMVC
** =========================================================================*/
/**
  Set VDD_MSS rail to a requested corner in MVC mode.

  @param pRail [in]   -- Pointer to the rail node.
  @param eCorner [in] -- Requested corner.

  @return
  DAL_SUCCESS -- Sucessfully configured VDD_MSS to requested corner.
  DAL_ERROR -- Failed to configure VDD_MSS to requested corner.

  @dependencies
  None.
*/

static DALResult VCS_SetVDDMSSCornerMVC
(
  VCSRailNodeType *pRail,
  VCSCornerType    eCorner
)
{
  /*-----------------------------------------------------------------------*/
  /* Validate rail pointer.                                                */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || eCorner >= VCS_CORNER_NUM_OF_CORNERS)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* We do not support MVC on this target.                                 */
  /* Do nothing and return success.                                        */
  /*-----------------------------------------------------------------------*/

  return DAL_ERROR_NOT_SUPPORTED;

} /* END VCS_SetVDDMSSCornerMVC */


/* =========================================================================
**  Function : VCS_SetVDDMSSCorner
** =========================================================================*/
/**
  Set VDD_MSS rail to a requested corner.

  @param pRail [in]   -- Pointer to the rail node.
  @param eCorner [in] -- Requested corner.

  @return
  DAL_SUCCESS -- Sucessfully configured VDD_MSS to requested corner.
  DAL_ERROR -- Failed to configure VDD_MSS to requested corner.

  @dependencies
  None.
*/

static DALResult VCS_SetVDDMSSCorner
(
  VCSRailNodeType *pRail,
  VCSCornerType    eCorner
)
{
  DALResult eResult = DAL_ERROR;

  /*-----------------------------------------------------------------------*/
  /* Sanity.                                                               */
  /*-----------------------------------------------------------------------*/

  if (pRail == NULL || eCorner >= VCS_CORNER_NUM_OF_CORNERS)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Pre-voltage update.                                                   */
  /*-----------------------------------------------------------------------*/

  if (eCorner < pRail->eCorner)
  {
    /*
     * ACC settings from high to low.
     */
    HWIO_OUTF(MVC_MEM_SVS, MEM_SVS, VCS_MapCornerToAccMemSel(eCorner));
  }

  /*-----------------------------------------------------------------------*/
  /* Update the voltage corner.                                            */
  /*-----------------------------------------------------------------------*/

  if (pRail->eMode == VCS_RAIL_MODE_CPR)
  {
    eResult = VCS_SetVDDMSSCornerCPR(pRail, eCorner);
  }
  else
  {
    eResult = VCS_SetVDDMSSCornerMVC(pRail, eCorner);
  }
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Failed to set rail to requested voltage.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Post-voltage update.                                                   */
  /*-----------------------------------------------------------------------*/

  if (eCorner > pRail->eCorner)
  {
    /*
     * ACC settings from high to low.
     */
    HWIO_OUTF(MVC_MEM_SVS, MEM_SVS, VCS_MapCornerToAccMemSel(eCorner));
  }

  return DAL_SUCCESS;

} /* END VCS_SetVDDMSSCorner */
#endif

#if 0
/* =========================================================================
**  Function : VCS_LoadNV_CX
** =========================================================================*/
/**
  Load EFS data for CX rail.

  @param pDrvCtxt [in] -- Pointer to the VCS driver context.
  @param hConfig [in] -- Handle to core config.

  @return
  DAL_SUCCESS -- Successfully parsed EFS data for CX.
  DAL_ERROR -- Failed to parse EFS data for CX.

  @dependencies
  None.
*/

static DALResult VCS_LoadNV_CX
(
  VCSDrvCtxt       *pDrvCtxt,
  CoreConfigHandle  hConfig
)
{
  VCSRailNodeType *pRail;
  uint32           nReadResult, nData;

  /*-----------------------------------------------------------------------*/
  /* Get rail nodes.                                                       */
  /*-----------------------------------------------------------------------*/

  pRail = pDrvCtxt->apRailMap[VCS_RAIL_CX];
  if (pRail == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Invalid CX rail structure");
  }

  /*-----------------------------------------------------------------------*/
  /* BEGIN CRITICAL SECTION: CX EFS data update.                           */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(pRail->resource.handle);

  /*-----------------------------------------------------------------------*/
  /* Update max corner.                                                    */
  /*-----------------------------------------------------------------------*/

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_CX_CONFIG_SECTION,
      VCS_EFS_RAIL_MAX_CORNER,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pRail->eCornerMax = MIN(nData, VCS_CORNER_MAX);
  }

  /*-----------------------------------------------------------------------*/
  /* Update min corner.                                                    */
  /*-----------------------------------------------------------------------*/

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_CX_CONFIG_SECTION,
      VCS_EFS_RAIL_MIN_CORNER,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pRail->eCornerMin = MIN(nData, pRail->eCornerMax);
  }

  /*-----------------------------------------------------------------------*/
  /* Update DVS.                                                           */
  /*-----------------------------------------------------------------------*/

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_CX_CONFIG_SECTION,
      VCS_EFS_RAIL_DVS_FLAG,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    if (nData == FALSE)
    {
      pRail->nDisableDVS |= VCS_FLAG_DISABLED_BY_EFS;
    }
    else
    {
      pRail->nDisableDVS = 0;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Notify clients they must reduce their vote due to new max.            */
  /*-----------------------------------------------------------------------*/

  if (pRail->resource.handle->request_state > pRail->eCornerMax)
  {
    // TODO once NPA broadcast API is available
  }

  /*-----------------------------------------------------------------------*/
  /* Verify votes reduced within supported range.                          */
  /*-----------------------------------------------------------------------*/

  if (pRail->resource.handle->request_state > pRail->eCornerMax)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: request[%l] on rail[%s] above max[%l]",
      pRail->resource.handle->request_state,
      pRail->pBSPConfig->szName,
      pRail->eCornerMax);
  }

  /*-----------------------------------------------------------------------*/
  /* Trigger impulse to update the rail active state.                      */
  /* NOTE: We update the resoure state to 0 in order for the impulse       */
  /*       request to make it's way into the driver function.              */
  /*       This workaround will be replaced by a new NPA API.              */
  /*-----------------------------------------------------------------------*/

  npa_update_resource_state(pRail->resource.handle, 0);
  npa_issue_impulse_request(pRail->hClientImpulse);

  /*-----------------------------------------------------------------------*/
  /* END CRITICAL SECTION: CX EFS data update.                             */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(pRail->resource.handle);

  return DAL_SUCCESS;

} /* END of VCS_LoadNV_CX */


/* =========================================================================
**  Function : VCS_LoadNV_MSS
** =========================================================================*/
/**
  Load EFS data for MSS rail.

  @param pDrvCtxt [in] -- Pointer to the VCS driver context.
  @param hConfig [in] -- Handle to core config.

  @return
  DAL_SUCCESS -- Successfully parsed EFS data for MSS.
  DAL_ERROR -- Failed to parse EFS data for MSS.

  @dependencies
  None.
*/

static DALResult VCS_LoadNV_MSS
(
  VCSDrvCtxt       *pDrvCtxt,
  CoreConfigHandle  hConfig
)
{
  VCSRailNodeType           *pRail;
  VCSCornerVoltageRangeType *pCornerRange;
  uint32                     nReadResult, nData;

  /*-----------------------------------------------------------------------*/
  /* Get rail nodes.                                                       */
  /*-----------------------------------------------------------------------*/

  pRail = pDrvCtxt->apRailMap[VCS_RAIL_MSS];
  if (pRail == NULL || pRail->pVoltageTable == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Invalid MSS rail structures");
  }

  /*-----------------------------------------------------------------------*/
  /* BEGIN CRITICAL SECTION: MSS EFS data update.                          */
  /*-----------------------------------------------------------------------*/

  npa_resource_lock(pRail->resource.handle);

  /*-----------------------------------------------------------------------*/
  /* Update CPR.                                                           */
  /*-----------------------------------------------------------------------*/

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_CPR_FLAG,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    if (nData == FALSE)
    {
      pRail->nDisableCPR |= VCS_FLAG_DISABLED_BY_EFS;
    }
    else
    {
      pRail->nDisableCPR = 0;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update max corner.                                                    */
  /*-----------------------------------------------------------------------*/

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_MAX_CORNER,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pRail->eCornerMax = MIN(nData, VCS_CORNER_MAX);
  }

  /*-----------------------------------------------------------------------*/
  /* Update min corner.                                                    */
  /*-----------------------------------------------------------------------*/

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_MIN_CORNER,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pRail->eCornerMin = MIN(nData, pRail->eCornerMax);
  }

  /*-----------------------------------------------------------------------*/
  /* Update DVS.                                                           */
  /*-----------------------------------------------------------------------*/

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_DVS_FLAG,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    if (nData == FALSE)
    {
      pRail->nDisableDVS |= VCS_FLAG_DISABLED_BY_EFS;
    }
    else
    {
      pRail->nDisableDVS = 0;
    }
  }

/* Feature added for Bringup. Need to remove after 3rd week of bringup */
#ifdef FEATURE_DISABLE_DCVS_BRINGUP
      pRail->nDisableDVS |= VCS_FLAG_DISABLED_BY_EFS;
#endif 
  /*-----------------------------------------------------------------------*/
  /* Update corner: LOW_MINUS                                              */
  /*-----------------------------------------------------------------------*/

  pCornerRange = pRail->pVoltageTable->apCornerMap[VCS_CORNER_LOW_MINUS];
  if (pCornerRange == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: No LOW_MINUS voltage data");
  }

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_VOLTAGE_LOW_MINUS_MIN,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pCornerRange->nMinUV = nData;
  }

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_VOLTAGE_LOW_MINUS_MAX,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pCornerRange->nMaxUV = nData;
  }

  /*-----------------------------------------------------------------------*/
  /* Update corner: LOW                                                    */
  /*-----------------------------------------------------------------------*/

  pCornerRange = pRail->pVoltageTable->apCornerMap[VCS_CORNER_LOW];
  if (pCornerRange == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: No LOW voltage data");
  }

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_VOLTAGE_LOW_MIN,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pCornerRange->nMinUV = nData;
  }

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_VOLTAGE_LOW_MAX,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pCornerRange->nMaxUV = nData;
  }

  /*-----------------------------------------------------------------------*/
  /* Update corner: NOMINAL                                                */
  /*-----------------------------------------------------------------------*/

  pCornerRange = pRail->pVoltageTable->apCornerMap[VCS_CORNER_NOMINAL];
  if (pCornerRange == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: No NOMINAL voltage data");
  }

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_VOLTAGE_NOMINAL_MIN,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pCornerRange->nMinUV = nData;
  }

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_VOLTAGE_NOMINAL_MAX,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pCornerRange->nMaxUV = nData;
  }

  /*-----------------------------------------------------------------------*/
  /* Update corner: TURBO                                                  */
  /*-----------------------------------------------------------------------*/

  pCornerRange = pRail->pVoltageTable->apCornerMap[VCS_CORNER_TURBO];
  if (pCornerRange == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: No TURBO voltage data");
  }

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_VOLTAGE_TURBO_MIN,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pCornerRange->nMinUV = nData;
  }

  nReadResult =
    CoreConfig_ReadUint32(
      hConfig,
      VCS_EFS_MPSS_RAIL_MSS_CONFIG_SECTION,
      VCS_EFS_RAIL_VOLTAGE_TURBO_MAX,
      (unsigned int *)&nData);
  if (nReadResult == CORE_CONFIG_SUCCESS)
  {
    pCornerRange->nMaxUV = nData;
  }

  /*-----------------------------------------------------------------------*/
  /* Notify clients they must reduce their vote due to new max.            */
  /*-----------------------------------------------------------------------*/

  if (pRail->resource.handle->request_state > pRail->eCornerMax)
  {
    // TODO once NPA broadcast API is available
  }

  /*-----------------------------------------------------------------------*/
  /* Verify votes reduced within supported range.                          */
  /*-----------------------------------------------------------------------*/

  if (pRail->resource.handle->request_state > pRail->eCornerMax)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: request[%l] on rail[%s] above max[%l]",
      pRail->resource.handle->request_state,
      pRail->pBSPConfig->szName,
      pRail->eCornerMax);
  }

  /*-----------------------------------------------------------------------*/
  /* Trigger impulse to update the rail active state.                      */
  /* NOTE: We update the resoure state to 0 in order for the impulse       */
  /*       request to make it's way into the driver function.              */
  /*       This workaround will be replaced by a new NPA API.              */
  /*-----------------------------------------------------------------------*/

  npa_update_resource_state(pRail->resource.handle, 0);
  npa_issue_impulse_request(pRail->hClientImpulse);

  /*-----------------------------------------------------------------------*/
  /* END CRITICAL SECTION: MSS EFS data update.                            */
  /*-----------------------------------------------------------------------*/

  npa_resource_unlock(pRail->resource.handle);

  return DAL_SUCCESS;

} /* END of VCS_LoadNV_MSS */
#endif

/* =========================================================================
**  Function : VCS_LoadNV
** =========================================================================*/
/*
  See VCSMSS.h
*/

void VCS_LoadNV
(
  void
)
{
  VCSDrvCtxt       *pDrvCtxt;

  pDrvCtxt = VCS_GetDrvCtxt();

  #if 0
  /*-----------------------------------------------------------------------*/
  /* Read clock configuration file.                                        */
  /*-----------------------------------------------------------------------*/

  hConfig = CoreIni_ConfigCreate(VCS_EFS_INI_FILENAME);
  if (hConfig == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_INFO,
      "DALLOG Device VCS: Unable to read EFS file: %s",
      VCS_EFS_INI_FILENAME);

    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Load the EFS data for CX.                                             */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_LoadNV_CX(pDrvCtxt, hConfig);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: unable to load EFS data for rail CX");

    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Load the EFS data for MSS.                                            */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_LoadNV_MSS(pDrvCtxt, hConfig);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: unable to load EFS data for rail CX");

    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Destroy the handle.                                                   */
  /*-----------------------------------------------------------------------*/

  CoreIni_ConfigDestroy(hConfig);
  #endif

} /* END VCS_LoadNV */

#if 0
/* =========================================================================
**  Function : VCS_InitVDDMSS
** =========================================================================*/
/**
  Initialize the VDD_MSS rail.

  This function initializes the VDD_MSS NPA resource.

  @param *pDrvCtxt [in] -- Pointer to driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

static DALResult VCS_InitVDDMSS
(
  VCSDrvCtxt *pDrvCtxt
)
{
  uint32                     i;
  VCSRailNodeType           *pRail;
  VCSImageCtxtType          *pImageCtxt;
  VCSCornerVoltageRangeType *pVoltageRange;
  VCSMVCVoltageType         *pMVCVoltage;

  pImageCtxt = (VCSImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Get a pointer to the VDD_MSS rail data node.                          */
  /*-----------------------------------------------------------------------*/

  pRail = pDrvCtxt->apRailMap[VCS_RAIL_MSS];
  if (pRail == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Hook up the rail's "set corner" function pointer.                     */
  /*-----------------------------------------------------------------------*/

  pRail->fpSetRailCorner = &VCS_SetVDDMSSCorner;

  /*-----------------------------------------------------------------------*/
  /* Check BSP config to determine if we should disable MVC.               */
  /*-----------------------------------------------------------------------*/

  if (!pImageCtxt->pBSPConfig->bEnableMVC)
  {
    pImageCtxt->MVCCtxt.nDisableMVC |= VCS_FLAG_DISABLED_BY_BSP;
  }

  /*-----------------------------------------------------------------------*/
  /* Fill in the corner, index, and max voltage for each MVC corner.       */
  /*-----------------------------------------------------------------------*/

  pMVCVoltage = &pImageCtxt->MVCCtxt.aMVCVoltage[0];
  pMVCVoltage->eCorner = VCS_CORNER_LOW_MINUS;
  pMVCVoltage->nRegIndex = 0;
  pVoltageRange = pRail->pVoltageTable->apCornerMap[VCS_CORNER_LOW_MINUS];
  if (pVoltageRange)
  {
    pMVCVoltage->nVoltageUV = pVoltageRange->nMaxUV;
  }

  pMVCVoltage = &pImageCtxt->MVCCtxt.aMVCVoltage[1];
  pMVCVoltage->eCorner = VCS_CORNER_LOW;
  pMVCVoltage->nRegIndex = 1;
  pVoltageRange = pRail->pVoltageTable->apCornerMap[VCS_CORNER_LOW];
  if (pVoltageRange)
  {
    pMVCVoltage->nVoltageUV = pVoltageRange->nMaxUV;
  }

  pMVCVoltage = &pImageCtxt->MVCCtxt.aMVCVoltage[2];
  pMVCVoltage->eCorner = VCS_CORNER_NOMINAL;
  pMVCVoltage->nRegIndex = 2;
  pVoltageRange = pRail->pVoltageTable->apCornerMap[VCS_CORNER_NOMINAL];
  if (pVoltageRange)
  {
    pMVCVoltage->nVoltageUV = pVoltageRange->nMaxUV;
  }

  pMVCVoltage = &pImageCtxt->MVCCtxt.aMVCVoltage[3];
  pMVCVoltage->eCorner = VCS_CORNER_TURBO;
  pMVCVoltage->nRegIndex = 3;
  pVoltageRange = pRail->pVoltageTable->apCornerMap[VCS_CORNER_TURBO];
  if (pVoltageRange)
  {
    pMVCVoltage->nVoltageUV = pVoltageRange->nMaxUV;
  }

  /*
   * Clear out the corner voltage map.
   */
  for (i = 0; i < ARR_SIZE(pImageCtxt->MVCCtxt.apMVCVoltageMap); i++)
  {
    pImageCtxt->MVCCtxt.apMVCVoltageMap[i] = NULL;
  }

  /*
   * Fill in the corner voltage map.
   */
  for (i = 0; i < VCS_MVC_NUM_OF_VOLTAGES; i++)
  {
    pMVCVoltage = &pImageCtxt->MVCCtxt.aMVCVoltage[i];
    pImageCtxt->MVCCtxt.apMVCVoltageMap[pMVCVoltage->eCorner] = pMVCVoltage;
  }

  return DAL_SUCCESS;

} /* END VCS_InitVDDMSS */
#endif

/* =========================================================================
**  Function : VCS_InitQ6LDO
** =========================================================================*/
/**
  Initialize the Q6 LDO

  @param *pDrvCtxt [in] -- Pointer to driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR_INTERNAL -- Q6 CPU node was not found.

  @dependencies
  None.
*/
#if 0

static DALResult VCS_InitQ6LDO
(
  VCSDrvCtxt *pDrvCtxt
)
{
  VCSCPUNodeType *pCPU;
  VCSLDONodeType *pLDO;

  /*-----------------------------------------------------------------------*/
  /* Get the Q6 CPU node.                                                  */
  /*-----------------------------------------------------------------------*/

  pCPU = pDrvCtxt->apCPUMap[CLOCK_CPU_MSS_Q6];
  if (pCPU == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to get CPU node.");

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Get the LDO node.                                                     */
  /*-----------------------------------------------------------------------*/

  pLDO = pCPU->pLDO;
  if (pLDO == NULL)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to get LDO node.");

    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Read the EFuse to determine whether LDO output should be allowed.     */
  /*-----------------------------------------------------------------------*/

  VCS_DetectLDOEFuse(pLDO);

  /*-----------------------------------------------------------------------*/
  /* Ensure we're in BHS mode before setting up the LDO config.            */
  /*-----------------------------------------------------------------------*/

  HAL_ldo_DisableLDO(pLDO->eLDO);

  /*-----------------------------------------------------------------------*/
  /* Program the LDO_CFG registers per recommendation from HW.             */
  /*-----------------------------------------------------------------------*/

  HAL_ldo_ConfigLDO(pLDO->eLDO);

  /*-----------------------------------------------------------------------*/
  /* Program the retention voltage.                                        */
  /*-----------------------------------------------------------------------*/

  HAL_ldo_SetLDORetentionVoltage(pLDO->eLDO, pLDO->pBSPConfig->nRetVoltageUV);

  return DAL_SUCCESS;

}
#endif


/* =========================================================================
**  Function : VCS_InitImage
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitImage
(
  VCSDrvCtxt *pDrvCtxt
)
{
  DALResult            eResult;
  VCSPropertyValueType PropVal;

  /*-----------------------------------------------------------------------*/
  /* Assign the image context.                                             */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->pImageCtxt = &VCS_ImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Get the image BSP/XML.                                                */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_GetPropertyValue("VCSImageBSPConfig", &PropVal);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: VCS_GetPropertyValue failed.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Store the pointer to the image BSP in the image context.              */
  /*-----------------------------------------------------------------------*/

  VCS_ImageCtxt.pBSPConfig = (VCSImageBSPConfigType *)PropVal;

#if 0
  /*-----------------------------------------------------------------------*/
  /* Initialize VDD_MSS.                                                   */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitVDDMSS(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to init VDD_MSS.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the Q6 LDO.                                                */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitQ6LDO(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to init Q6 LDO.");

    return eResult;
  }
#endif

  /*-----------------------------------------------------------------------*/
  /* Done.                                                                 */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_InitImage */


/* =========================================================================
**  Function : VCSStub_InitImage
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCSStub_InitImage
(
  VCSDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCSStub_InitImage */

