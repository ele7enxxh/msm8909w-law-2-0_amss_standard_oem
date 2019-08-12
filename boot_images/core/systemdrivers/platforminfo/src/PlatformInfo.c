/**
  @file PlatformInfo.c

  Interface implementation file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/platforminfo/src/PlatformInfo.c#1 $
  $DateTime: 2015/09/01 00:30:35 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "PlatformInfo.h"
#include "com_dtypes.h" /* smem.h needs to include this */
#include "smem.h"
#include "mobile.h"
#include "msmhwiobase.h"
#include "stringl/stringl.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

/*
 * Region of RPM MSG RAM dedicated for PlatformInfo.
 */
#define PLATFORMINFO_RPM_MSG_RAM_OFFS 0x00000B00
#define PLATFORMINFO_RPM_MSG_RAM_REGION \
  (RPM_SS_MSG_RAM_START_ADDRESS_BASE + PLATFORMINFO_RPM_MSG_RAM_OFFS)


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * PlatformInfoRPMType
 *
 */
typedef struct
{
  DalPlatformInfoPlatformInfoType   PlatformInfo;
  uint32                            nNumKVPS;
  PlatformInfoKVPSType              aKVPS[];
} PlatformInfoRPMType;

/*
 * PlatformInfoKVPSCDTType
 *
 *
 */
typedef PACKED struct
{
  uint8 nKey;
  uint8 nValue;
} PlatformInfoKVPSCDTType;

/*
 * PlatformInfoCDTType
 *
 * Storage structure which mimics the format of the EEPROM platform ID section.
 */
typedef PACKED struct
{
  uint8                 nVersion;
  uint8                 nPlatform;
  uint8                 nHWVersionMajor;
  uint8                 nHWVersionMinor;
  uint8                 nSubtype;
  uint8                 nNumKVPS;
  PlatformInfoKVPSCDTType  aKVPS[];
} PlatformInfoCDTType;

/*
 * PlatformInfoPmicFamilyType
 *
 * An enum to define Chip Family type.
 */
typedef enum
{
  PLATFORMINFO_PMIC_FAMILY_A     = 0,
  PLATFORMINFO_PMIC_FAMILY_B     = 1,
}PlatformInfoPmicFamilyType;


/*=========================================================================
      Function Prototypes
==========================================================================*/

static DALResult PlatformInfo_InitGlblCtxt (PlatformInfoDrvCtxt *pDrvCtxt);
static DALResult PlatformInfo_InitSMem     (PlatformInfoDrvCtxt *pDrvCtxt);
static DALResult PlatformInfo_CDTInit (PlatformInfoDrvCtxt *pDrvCtxt, PlatformInfoCDTType *pPlatformInfo);


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
DALResult PlatformInfo_DriverInit
(
  PlatformInfoDrvCtxt *pDrvCtxt
)
{

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : PlatformInfo_InitGlblCtxt
** =========================================================================*/
/**
  This function allocates and fills in the global context.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  None.
*/

static DALResult PlatformInfo_InitGlblCtxt
(
  PlatformInfoDrvCtxt *pDrvCtxt
)
{
  DALResult eResult;
  uint32 nSize, nKVPSSize;

  /*-----------------------------------------------------------------------*/
  /* Ensure global context has been initialized.                           */
  /*-----------------------------------------------------------------------*/

  DALGLBCTXT_Init();

  /*-----------------------------------------------------------------------*/
  /* Allocate the context.                                                 */
  /*-----------------------------------------------------------------------*/

  nKVPSSize = pDrvCtxt->nNumKVPS * sizeof(PlatformInfoKVPSType);
  nSize = sizeof(PlatformInfoGlbCtxType) + nKVPSSize;

  eResult = DALGLBCTXT_AllocCtxt("DAL_PLATFORM_INFO", nSize,
    DALGLBCTXT_LOCK_TYPE_SPINLOCK, (void **)&pDrvCtxt->pGlbCtxt);

  /*-----------------------------------------------------------------------*/
  /* Fill it in if possible.                                               */
  /*-----------------------------------------------------------------------*/

  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  pDrvCtxt->pGlbCtxt->PlatformInfo = pDrvCtxt->PlatformInfo;
  pDrvCtxt->pGlbCtxt->nNumKVPS = pDrvCtxt->nNumKVPS;
  memscpy(pDrvCtxt->pGlbCtxt->aKVPS, nKVPSSize, pDrvCtxt->aKVPS, nKVPSSize);

  return DAL_SUCCESS;

} /* PlatformInfo_InitGlblCtxt */


/* =========================================================================
**  Function : PlatformInfo_GetPlatform
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_GetPlatform
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  DalPlatformInfoPlatformType * pType
)
{
  if (pType != NULL)
  {
    *pType = pDrvCtxt->PlatformInfo.platform;
    return DAL_SUCCESS;
  }

  return DAL_ERROR;

} /* END PlatformInfo_GetPlatform */


/* =========================================================================
**  Function : PlatformInfo_GetPlatformInfo
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_GetPlatformInfo
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  DalPlatformInfoPlatformInfoType * pInfo
)
{
  if (pInfo != NULL)
  {
    *pInfo = pDrvCtxt->PlatformInfo;
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
  PlatformInfoDrvCtxt     *pDrvCtxt,
  DalPlatformInfoKeyType   eKey,
  uint32                  *nValue
)
{
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


/* =========================================================================
**  Function : PlatformInfo_InitSmem
** =========================================================================*/
/*
  Initializes and populates the SMEM (socinfo) location in which to store
  chip, platform, and build information.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
*/

static DALResult PlatformInfo_InitSMem
(
  PlatformInfoDrvCtxt *pDrvCtxt
)
{
  DalPlatformInfoSMemType *pSMem;
  DalDeviceHandle         *phChipInfo;
  DALResult                eResult;
  uint8                    nPmicDeviceIndex;
  pm_device_info_type      PmicDeviceInfo;

  pSMem = (DalPlatformInfoSMemType *)
    smem_alloc(SMEM_HW_SW_BUILD_ID, sizeof(DalPlatformInfoSMemType));
  if (pSMem == NULL)
  {
    return DAL_ERROR;
  }

  DALSYS_memset(pSMem, 0, sizeof(DalPlatformInfoSMemType));

  pSMem->nFormat = DALPLATFORMINFO_SMEM_FORMAT;
  strlcpy(pSMem->aBuildId, mob_sw_rev, DALPLATFORMINFO_SMEM_BUILD_ID_LENGTH);

  eResult = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);
  if (eResult == DAL_SUCCESS)
  {
    DalChipInfo_GetChipId(phChipInfo, (DalChipInfoIdType *)&pSMem->eChipId);
    DalChipInfo_GetChipVersion(phChipInfo, &pSMem->nChipVersion);
    DalChipInfo_GetRawChipId(phChipInfo, &pSMem->nRawChipId);
    DalChipInfo_GetRawChipVersion(phChipInfo, &pSMem->nRawChipVersion);
    DalChipInfo_GetFoundryId(phChipInfo,
      (DalChipInfoFoundryIdType *)&pSMem->nFoundryId);
    DalChipInfo_GetSerialNumber(phChipInfo,
      (DalChipInfoSerialNumType *)&pSMem->nChipSerial);
  }

  for (nPmicDeviceIndex = 0; nPmicDeviceIndex < DALPLATFORMINFO_SMEM_MAX_PMIC_DEVICES; nPmicDeviceIndex++)
  {
    if (pm_get_pmic_info(nPmicDeviceIndex, &PmicDeviceInfo) == PM_ERR_FLAG__SUCCESS)
    {
      pSMem->aPMICInfo[nPmicDeviceIndex].nPMICModel = ((PLATFORMINFO_PMIC_FAMILY_B << 16) | (PmicDeviceInfo.ePmicModel));

      pSMem->aPMICInfo[nPmicDeviceIndex].nPMICVersion =
        DALPLATFORMINFO_VERSION(PmicDeviceInfo.nPmicAllLayerRevision,
                                PmicDeviceInfo.nPmicMetalRevision);
    }
    else
    {
      pSMem->aPMICInfo[nPmicDeviceIndex].nPMICModel = PMIC_IS_INVALID;
      pSMem->aPMICInfo[nPmicDeviceIndex].nPMICVersion = 0;
    }
  }

  pSMem->ePlatformType = pDrvCtxt->PlatformInfo.platform;
  pSMem->nPlatformVersion = pDrvCtxt->PlatformInfo.version;
  pSMem->nPlatformSubtype = pDrvCtxt->PlatformInfo.subtype;
  pSMem->bFusion = pDrvCtxt->PlatformInfo.fusion;

  return DAL_SUCCESS;

} /* END PlatformInfo_InitSMem */


/* =========================================================================
**  Function : PlatformInfo_InitRPM
** =========================================================================*/
/*
  Initialize and populate the IMEM location used to communicate to RPM.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
*/

static DALResult PlatformInfo_InitRPM
(
  PlatformInfoDrvCtxt *pDrvCtxt
)
{
  PlatformInfoRPMType *pRPM;
  uint32 nKVPSSize;

  /*
   * The RPM MSG RAM region is in use for 8974v1, so we can't initialize.
   */
  if (DalChipInfo_ChipFamily() == DALCHIPINFO_FAMILY_MSM8974 &&
      DalChipInfo_ChipVersion() < DALCHIPINFO_VERSION(2,0))
  {
    return DAL_SUCCESS;
  }

  pRPM = (PlatformInfoRPMType *)(PLATFORMINFO_RPM_MSG_RAM_REGION);

  pRPM->PlatformInfo = pDrvCtxt->PlatformInfo;
  pRPM->nNumKVPS = pDrvCtxt->nNumKVPS;
  if (pRPM->nNumKVPS > 0)
  {
    nKVPSSize = pRPM->nNumKVPS * sizeof(PlatformInfoKVPSType);
    memscpy(pRPM->aKVPS, nKVPSSize, pDrvCtxt->aKVPS, nKVPSSize);
  }

  return DAL_SUCCESS;

} /* END PlatformInfo_InitRPM */


/* =========================================================================
**  Function : PlatformInfo_CDTInit
** =========================================================================*/
/*
  Populate the platform information based on data from the CDT.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pPlatformInfo [in] -- Pointer to the CDT data.
*/

static DALResult PlatformInfo_CDTInit
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  PlatformInfoCDTType *pPlatformInfo
)
{
  static boolean bCDTInitialized = FALSE;
  DalChipInfoFamilyType eFamily;
  DALResult eResult;
  uint32 nSize, i, nVal;

  if (bCDTInitialized == TRUE)
  {
    return DAL_SUCCESS;
  }

  if (pPlatformInfo == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Get platform type.                                                    */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->PlatformInfo.platform =
    (DalPlatformInfoPlatformType)pPlatformInfo->nPlatform;

  /*-----------------------------------------------------------------------*/
  /* Get platform version.                                                 */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->PlatformInfo.version =
    DALPLATFORMINFO_VERSION(pPlatformInfo->nHWVersionMajor,
                            pPlatformInfo->nHWVersionMinor);

  /*-----------------------------------------------------------------------*/
  /* Get platform sub-type.                                                */
  /*-----------------------------------------------------------------------*/

  if (pPlatformInfo->nVersion >= 2)
  {
    pDrvCtxt->PlatformInfo.subtype = pPlatformInfo->nSubtype;
  }

  /*-----------------------------------------------------------------------*/
  /* Get key-value pairs.                                                  */
  /*-----------------------------------------------------------------------*/

  if (pPlatformInfo->nVersion >= 3)
  {
    pDrvCtxt->nNumKVPS = pPlatformInfo->nNumKVPS;
    if (pDrvCtxt->nNumKVPS > 0)
    {
      nSize = pDrvCtxt->nNumKVPS * sizeof(PlatformInfoKVPSType);
      eResult = DALSYS_Malloc(nSize, (void **)&pDrvCtxt->aKVPS);
      if (eResult != DAL_SUCCESS)
      {
        return DAL_ERROR;
      }

      for (i = 0; i < pDrvCtxt->nNumKVPS; i++)
      {
        pDrvCtxt->aKVPS[i].nKey = pPlatformInfo->aKVPS[i].nKey;
        pDrvCtxt->aKVPS[i].nValue = pPlatformInfo->aKVPS[i].nValue;
      }
    }

    /*-----------------------------------------------------------------------*/
    /* Get Fusion.                                                           */
    /*-----------------------------------------------------------------------*/

    eResult =
      PlatformInfo_GetKeyValue(pDrvCtxt, DALPLATFORMINFO_KEY_FUSION, &nVal);
    if (eResult == DAL_SUCCESS)
    {
      pDrvCtxt->PlatformInfo.fusion = nVal;
    }
    else
    {
      /*
       * Override bFusion for legacy CDT's.
       */
      eFamily = DalChipInfo_ChipFamily();
      if ((eFamily == DALCHIPINFO_FAMILY_MSM8974 &&
            pDrvCtxt->PlatformInfo.platform == DALPLATFORMINFO_TYPE_CDP &&
            pDrvCtxt->PlatformInfo.subtype == 1) ||
          (eFamily == DALCHIPINFO_FAMILY_MDM9x25 &&
            pDrvCtxt->PlatformInfo.platform == DALPLATFORMINFO_TYPE_CDP &&
            pDrvCtxt->PlatformInfo.subtype == 1) ||
          (eFamily == DALCHIPINFO_FAMILY_MDM9x25 &&
            pDrvCtxt->PlatformInfo.platform == DALPLATFORMINFO_TYPE_MTP_MDM &&
            pDrvCtxt->PlatformInfo.subtype == 1))
      {
        pDrvCtxt->PlatformInfo.fusion = TRUE;
      }
    }
  }

  bCDTInitialized = TRUE;
  return DAL_SUCCESS;

} /* END PlatformInfo_CDTInit */


/* =========================================================================
**  Function : PlatformInfo_CDTConfigPreDDR
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_CDTConfigPreDDR
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  void                *pPlatformInfo
)
{
  DALResult eResult;

  /*
   * Initialize CDT data.
   */
  eResult = PlatformInfo_CDTInit(pDrvCtxt, (PlatformInfoCDTType *)pPlatformInfo);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  /*
   * Initialize the location for communication to RPM.
   */
  PlatformInfo_InitRPM(pDrvCtxt);

  return DAL_SUCCESS;

} /* END PlatformInfo_CDTConfigPreDDR*/


/* =========================================================================
**  Function : PlatformInfo_CDTConfigPostDDR
** =========================================================================*/
/*
  See DDIPlatformInfo.h
*/

DALResult PlatformInfo_CDTConfigPostDDR
(
  PlatformInfoDrvCtxt *pDrvCtxt,
  void                *pPlatformInfo
)
{
  DALResult eResult;

  /*
   * Initialize CDT data.
   */
  eResult = PlatformInfo_CDTInit(pDrvCtxt, (PlatformInfoCDTType *)pPlatformInfo);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  /*
   * Initialize the DAL global context.
   */
  PlatformInfo_InitGlblCtxt(pDrvCtxt);

  /*
   * Initialize the SMEM (socinfo) location.
   */
  eResult = PlatformInfo_InitSMem(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
}

