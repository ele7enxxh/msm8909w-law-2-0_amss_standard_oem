/**
  @file PlatformInfo.c

  Interface implementation file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/platforminfo/src/PlatformInfo.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "PlatformInfo.h"
#include <stringl/stringl.h>
#include "DALGlbCtxt.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

typedef struct
{
  uint32 nKey;
  uint32 nValue;
} PlatformInfoKVPSType;

/*
 * PlatformInfoGlbCtxType
 *
 * Global context data structure for sharing the platform across
 * processors.
 */
typedef struct
{
  DALGLB_HEADER                     DALGLBHeader;
  DalPlatformInfoPlatformInfoType   PlatformInfo;
  uint32                            nNumKVPS;
  PlatformInfoKVPSType              aKVPS[];
} PlatformInfoGlbCtxType;

/*
 * PlatformInfo driver context.
 */
typedef struct PlatformInfoDrvCtxt
{
  DalPlatformInfoPlatformInfoType  PlatformInfo;
  uint32                           nNumKVPS;
  PlatformInfoKVPSType            *aKVPS;
} PlatformInfoDrvCtxt;


/*=========================================================================
      Data
==========================================================================*/

static PlatformInfoDrvCtxt PlatformInfo_DriverCtxt;

/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : PlatformInfo_DriverInit
** =========================================================================*/
/**
  Initialize the PlatformInfo driver.

  This function initializes the PlatformInfo driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult PlatformInfo_Init (void)
{
  PlatformInfoDrvCtxt *pDrvCtxt = &PlatformInfo_DriverCtxt;
  PlatformInfoGlbCtxType *pGlbCtxt;
  DALResult eResult;
  uint32 nSize;

  DALGLBCTXT_Init();
  DALGLBCTXT_FindCtxt("DAL_PLATFORM_INFO", (void **)&pGlbCtxt);

  if (pGlbCtxt == NULL)
  {
    return DAL_SUCCESS;
  }

  pDrvCtxt->PlatformInfo = pGlbCtxt->PlatformInfo;
  pDrvCtxt->nNumKVPS = pGlbCtxt->nNumKVPS;

  if (pDrvCtxt->nNumKVPS > 0)
  {
    nSize = sizeof(PlatformInfoKVPSType) * pDrvCtxt->nNumKVPS;
    eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aKVPS);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }

    memscpy(pDrvCtxt->aKVPS, nSize, pGlbCtxt->aKVPS, nSize);
  }

  return DAL_SUCCESS;

} /* END PlatformInfo_DriverInit */


/* =========================================================================
**  Function : PlatformInfo_GetPlatform
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DalPlatformInfoPlatformType PlatformInfo_GetPlatform (void)
{
  return PlatformInfo_DriverCtxt.PlatformInfo.platform;

} /* END PlatformInfo_GetPlatform */


/* =========================================================================
**  Function : PlatformInfo_GetPlatformInfo
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_GetPlatformInfo
(
  DalPlatformInfoPlatformInfoType *pInfo
)
{
  if (pInfo != NULL)
  {
    *pInfo = PlatformInfo_DriverCtxt.PlatformInfo;
    return DAL_SUCCESS;
  }

  return DAL_ERROR;

} /* END PlatformInfo_GetPlatformInfo */


/* =========================================================================
**  Function : PlatformInfo_GetKeyValue
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_GetKeyValue
(
  DalPlatformInfoKeyType   eKey,
  uint32                  *nValue
)
{
  PlatformInfoDrvCtxt *pDrvCtxt = &PlatformInfo_DriverCtxt;
  uint32 i;

  if (nValue == NULL)
  {
    return DAL_ERROR;
  }

  for (i = 0; i < pDrvCtxt->nNumKVPS; i++)
  {
    if (eKey == pDrvCtxt->aKVPS[i].nKey)
    {
      *nValue = pDrvCtxt->aKVPS[i].nValue;
      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR;

} /* END PlatformInfo_GetKeyValue */

