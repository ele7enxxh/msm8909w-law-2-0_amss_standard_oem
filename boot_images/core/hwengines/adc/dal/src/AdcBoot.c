/*============================================================================
  FILE:         AdcLib.c

  OVERVIEW:     Implementation of the ADC library

  DEPENDENCIES: None

                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/dal/src/AdcBoot.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-19  jjo  Platform subtype check for 8974.
  2012-12-03  jjo  Added IADC.
  2012-07-01  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DDIAdcDevice.h"
#include "AdcBoot.h"
#include "AdcBootInternal.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "pm_version.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ADC_MAX_NUM_DEVICES 3

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
   const char* DeviceString;
   DalDeviceHandle *phDevice;
} AdcDeviceType;

typedef struct
{
   AdcBspType *pAdcBsp;
   AdcDeviceType aAdcDevice[ADC_MAX_NUM_DEVICES];
   uint32 uNumDevices;
} AdcBootContextType;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
extern DALResult
VAdc_DalAdcDevice_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
DALBOOL bAdcInitialized = FALSE;
static AdcBootContextType adcBootCtxt;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
DALResult AdcBoot_GetChannel(const char *pszInputName, AdcBootDeviceChannelType *pChannel)
{
   DALResult result;
   uint32 uDevice;
   uint32 uChannel;
   DalDeviceHandle *phDevice;

   if ((pszInputName == NULL) || (pChannel == NULL))
   {
      return DAL_ERROR;
   }

   for (uDevice = 0; uDevice < adcBootCtxt.uNumDevices; uDevice++)
   {
      phDevice = adcBootCtxt.aAdcDevice[uDevice].phDevice;

      result = DalAdcDevice_GetInputProperties(phDevice, (const char *)pszInputName, (uint32 *)&uChannel);
      if (result == DAL_SUCCESS)
      {
         pChannel->nDeviceIdx = uDevice;
         pChannel->nChannelIdx = uChannel;

         return DAL_SUCCESS;
      }
   }

   pChannel->nDeviceIdx = ADC_BOOT_INVALID_IDX;
   pChannel->nChannelIdx = ADC_BOOT_INVALID_IDX;

   return DAL_SUCCESS;
}

DALResult AdcBoot_Read(AdcBootDeviceChannelType *pBootChannel, AdcBootResultType *pAdcBootResult)
{
   DALResult result;
   uint32 uDevice;
   uint32 uChannel;
   DalDeviceHandle *phDevice;
   AdcDeviceResultType adcDeviceResult;

   if ((pBootChannel == NULL) || (pAdcBootResult == NULL))
   {
      return DAL_ERROR;
   }

   uDevice = pBootChannel->nDeviceIdx;

   if (uDevice >= adcBootCtxt.uNumDevices || uDevice >= ADC_MAX_NUM_DEVICES)
   {
      return DAL_ERROR;
   }

   uChannel = pBootChannel->nChannelIdx;
   phDevice = adcBootCtxt.aAdcDevice[uDevice].phDevice;

   adcDeviceResult.eStatus = ADC_DEVICE_RESULT_INVALID;
   adcDeviceResult.nPhysical = 0;
   adcDeviceResult.uPercent = 0;
   adcDeviceResult.uMicrovolts = 0;
   adcDeviceResult.uCode = 0;

   result = DalDevice_Open(phDevice, DAL_OPEN_EXCLUSIVE);
   if (result != DAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   result = DalAdcDevice_ReadChannelSync(phDevice, uChannel, &adcDeviceResult);
   if (result != DAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   DalDevice_Close(phDevice);
   if (result != DAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   if (adcDeviceResult.eStatus == ADC_DEVICE_RESULT_INVALID_CHANNEL)
   {
      return DAL_ERROR;
   }
   else if (adcDeviceResult.eStatus == ADC_DEVICE_RESULT_VALID)
   {
      pAdcBootResult->eStatus = ADC_BOOT_RESULT_VALID;
   }
   else
   {
      pAdcBootResult->eStatus = ADC_BOOT_RESULT_INVALID;
   }

   pAdcBootResult->nPhysical = adcDeviceResult.nPhysical;
   pAdcBootResult->nPercent = adcDeviceResult.uPercent;
   pAdcBootResult->nMicrovolts = adcDeviceResult.uMicrovolts;
   pAdcBootResult->nCode = adcDeviceResult.uCode;

   return DAL_SUCCESS;
}

DALResult AdcBoot_RecalibrateChannel(AdcBootDeviceChannelType *pBootChannel, AdcBootRecalibrateChannelResultType *pAdcBootRecalChanResult)
{
   DALResult result;
   uint32 uDevice;
   uint32 uChannel;
   DalDeviceHandle *phDevice;
   AdcDeviceRecalibrateResultType adcDeviceRecalibrateResult;

   if ((pBootChannel == NULL) || (pAdcBootRecalChanResult == NULL))
   {
      return DAL_ERROR;
   }

   uDevice = pBootChannel->nDeviceIdx;

   if (uDevice >= adcBootCtxt.uNumDevices || uDevice >= ADC_MAX_NUM_DEVICES)
   {
      return DAL_ERROR;
   }

   uChannel = pBootChannel->nChannelIdx;
   phDevice = adcBootCtxt.aAdcDevice[uDevice].phDevice;

   adcDeviceRecalibrateResult.eStatus = ADC_DEVICE_RESULT_INVALID;
   adcDeviceRecalibrateResult.nPhysical1 = 0;
   adcDeviceRecalibrateResult.nPhysical2 = 0;
   adcDeviceRecalibrateResult.uCode1 = 0;
   adcDeviceRecalibrateResult.uCode2 = 0;

   result = DalDevice_Open(phDevice, DAL_OPEN_EXCLUSIVE);
   if (result != DAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   result = DalAdcDevice_RecalibrateChannelSync(phDevice, uChannel, &adcDeviceRecalibrateResult);
   if (result != DAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   DalDevice_Close(phDevice);
   if (result != DAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   if (adcDeviceRecalibrateResult.eStatus == ADC_DEVICE_RESULT_INVALID_CHANNEL)
   {
      return DAL_ERROR;
   }
   else if (adcDeviceRecalibrateResult.eStatus == ADC_DEVICE_RESULT_VALID)
   {
      pAdcBootRecalChanResult->eStatus = ADC_BOOT_RESULT_VALID;
   }
   else
   {
      pAdcBootRecalChanResult->eStatus = ADC_BOOT_RESULT_INVALID;
   }

   pAdcBootRecalChanResult->nPhysical1 = adcDeviceRecalibrateResult.nPhysical1;
   pAdcBootRecalChanResult->nPhysical2 = adcDeviceRecalibrateResult.nPhysical2;
   pAdcBootRecalChanResult->uCode1 = adcDeviceRecalibrateResult.uCode1;
   pAdcBootRecalChanResult->uCode2 = adcDeviceRecalibrateResult.uCode2;

   return DAL_SUCCESS;
}

DALResult AdcBoot_Init(void)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hAdcBootProperties);
   DALSYSPropertyVar propertyVar;
   DALResult result;
   uint32 uDevice;
   DalDeviceHandle *phPlatformInfoDevice;
   DalChipInfoFamilyType eChipInfoFamilyType;
   DalPlatformInfoPlatformInfoType platformInfo;
   const char *pszAdcBspPropName = "ADC_BOOT_BSP";

   if (bAdcInitialized == TRUE)
   {
      return DAL_SUCCESS;
   }

   DALSYS_InitMod(NULL);

   /* Get the chip family */
   eChipInfoFamilyType = DalChipInfo_ChipFamily();

   if (eChipInfoFamilyType == DALCHIPINFO_FAMILY_MSM8974_PRO)
   {
      /* Get the platform info */
      result = DAL_PlatformInfoDeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfoDevice);
      if (result != DAL_SUCCESS)
      {
         return result;
      }

      result = DalPlatformInfo_GetPlatformInfo(phPlatformInfoDevice, &platformInfo);
      if (result != DAL_SUCCESS)
      {
         return result;
      }

      if (platformInfo.subtype == 0x01)
      {
         /* 8974PRO_PMA8084_PM8941 */
         pszAdcBspPropName = "ADC_BOOT_BSP_8974PRO_PMA8084_PM8941";
      }
   }

//TODO: Remove after 8952 PMIC compilation support is enabled.
//#ifndef FEATURE_NO_PMIC_SUPPORT
//   if (pm_get_pmic_model(0) == PMIC_IS_PM8950 && pm_get_pmic_model(1) == PMIC_IS_PMI8950)
//   {
//      /* 8952_PM8950_PMI8950 */
//      pszAdcBspPropName = "ADC_BOOT_BSP_8952_PM8950_PMI8950";
//   }
//#endif

   /* Get the properties */
   result = DALSYS_GetDALPropertyHandleStr("QAdc", hAdcBootProperties);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   result = DALSYS_GetPropertyValue(hAdcBootProperties,
                                    pszAdcBspPropName,
                                    0,
                                    &propertyVar);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   adcBootCtxt.pAdcBsp = (AdcBspType *)propertyVar.Val.pStruct;

   if (adcBootCtxt.pAdcBsp->uNumPhysicalDevices > ADC_MAX_NUM_DEVICES)
   {
      adcBootCtxt.uNumDevices = ADC_MAX_NUM_DEVICES;
   }
   else
   {
      adcBootCtxt.uNumDevices = adcBootCtxt.pAdcBsp->uNumPhysicalDevices;
   }

   for (uDevice = 0; uDevice < adcBootCtxt.uNumDevices; uDevice++)
   {
      adcBootCtxt.aAdcDevice[uDevice].DeviceString = adcBootCtxt.pAdcBsp->paAdcPhysicalDevices[uDevice].pszDevName;

      result = DAL_AdcDeviceDeviceAttach((adcBootCtxt.aAdcDevice[uDevice].DeviceString),
                                         &adcBootCtxt.aAdcDevice[uDevice].phDevice);
      if (result != DAL_SUCCESS)
      {
         DALSYS_DeInitMod();
         return DAL_ERROR;
      }
   }

   bAdcInitialized = TRUE;

   return DAL_SUCCESS;
}

DALResult AdcBoot_DeInit(void)
{
   bAdcInitialized = FALSE;

   return DAL_SUCCESS;
}

