/**
  @file PlatformInfo.c

  Implementation file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/platforminfo/src/PlatformInfo.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"
#include "PlatformInfo.h"
#include "Chipinfo.h"
#include "msmhwiobase.h"
#include <stdlib.h>
#include <string.h>

/*=========================================================================
      Macro Definitions
==========================================================================*/

/*
 * Region of RPM MSG RAM dedicated for PlatformInfo.
 */
#define PLATFORMINFO_RPM_MSG_RAM_OFFS 0x00000B00
#define PLATFORMINFO_RPM_MSG_RAM_REGION \
  (RPM_MSG_RAM_BASE + PLATFORMINFO_RPM_MSG_RAM_OFFS)

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Structure to hold a key-value pair.
 */
typedef struct
{
  uint32 nKey;
  uint32 nValue;
} PlatformInfoKVPSType;

/*
 * Structure used to communicate information from SBL to RPM.
 */
typedef struct PlatformInfoMsgType
{
  PlatformInfoPlatformInfoType     PlatformInfo;
  uint32                           nNumKVPS;
  PlatformInfoKVPSType             aKVPS[];
} PlatformInfoMsgType;

/*
 * PlatformInfo driver context.
 */
typedef struct PlatformInfoDrvCtxt
{
  PlatformInfoPlatformInfoType     PlatformInfo;
  uint32                           nNumKVPS;
  PlatformInfoKVPSType             *aKVPS;
} PlatformInfoDrvCtxt;


/*=========================================================================
      Data
==========================================================================*/

/*
 * Driver context.
 */
static PlatformInfoDrvCtxt PlatformInfo_DriverCtxt;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : PlatformInfo_Init
** =========================================================================*/
/**
  Initialize the PlatformInfo driver.
*/

void PlatformInfo_Init
(
  void
)
{
  PlatformInfoMsgType *pPlatformInfo;
  uint32 nSize;

  /*
   * The RPM MSG RAM region is in use for 8974v1, so we can't initialize.
   */
  if (Chipinfo_GetFamily() == CHIPINFO_FAMILY_MSM8974 &&
      Chipinfo_GetVersion() < CHIPINFO_VERSION(2,0))
  {
    return;
  }

  pPlatformInfo = (PlatformInfoMsgType *)(PLATFORMINFO_RPM_MSG_RAM_REGION);

  PlatformInfo_DriverCtxt.PlatformInfo = pPlatformInfo->PlatformInfo;
  PlatformInfo_DriverCtxt.nNumKVPS = pPlatformInfo->nNumKVPS;

  if (PlatformInfo_DriverCtxt.nNumKVPS > 0)
  {
    nSize = PlatformInfo_DriverCtxt.nNumKVPS * sizeof(PlatformInfoKVPSType);
    PlatformInfo_DriverCtxt.aKVPS = (PlatformInfoKVPSType *) malloc(nSize);
    if (PlatformInfo_DriverCtxt.aKVPS == NULL)
    {
      return;
    }

    memcpy(PlatformInfo_DriverCtxt.aKVPS, pPlatformInfo->aKVPS, nSize);
  }

  return;

} /* END PlatformInfo_DriverInit */


/* =========================================================================
**  Function : PlatformInfo_GetPlatform
** =========================================================================*/
/*
  See PlatformInfo.h
*/

PlatformInfoPlatformType PlatformInfo_GetPlatform
(
  void
)
{
  return PlatformInfo_DriverCtxt.PlatformInfo.ePlatform;

} /* END PlatformInfo_GetPlatform */


/* =========================================================================
**  Function : PlatformInfo_GetPlatformInfo
** =========================================================================*/
/*
  See PlatformInfo.h
*/

boolean PlatformInfo_GetPlatformInfo
(
  PlatformInfoPlatformInfoType *pInfo
)
{
  if (pInfo != NULL)
  {
    *pInfo = PlatformInfo_DriverCtxt.PlatformInfo;
    return TRUE;
  }

  return FALSE;

} /* END PlatformInfo_GetPlatformInfo */


/* =========================================================================
**  Function : PlatformInfo_GetKeyValue
** =========================================================================*/
/*
  See PlatformInfo.h
*/

boolean PlatformInfo_GetKeyValue
(
  PlatformInfoKeyType eKey,
  uint32              *nValue
)
{
  uint32 i;

  if (nValue == NULL)
  {
    return FALSE;
  }

  for (i = 0; i < PlatformInfo_DriverCtxt.nNumKVPS; i++)
  {
    if (eKey == PlatformInfo_DriverCtxt.aKVPS[i].nKey)
    {
      *nValue = PlatformInfo_DriverCtxt.aKVPS[i].nValue;
      return TRUE;
    }
  }

  return FALSE;

} /* END PlatformInfo_GetKeyValue */

