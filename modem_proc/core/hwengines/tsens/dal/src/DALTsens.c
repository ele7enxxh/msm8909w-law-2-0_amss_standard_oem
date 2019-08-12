/*============================================================================
  FILE:         TsensDevice.c

  OVERVIEW:     Implementation of the TSENS device library

  DEPENDENCIES: None
 
                Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/tsens/dal/src/DALTsens.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $
 
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-11-27  SA   Add 8917 support.
  2015-11-05  PR   use VALID bit on supported targets.
  2014-02-03  jjo  Ported to modem.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALTsens.h"
#include "HALtsens.h"
#include "TsensiConversion.h"
#include "TsensBsp.h"
#include "msg.h"
#include "DDIChipInfo.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TSENS_BUSY_WAIT_TIME 25

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
   TSENS_DEVICE_STATE_INIT = 0,
   TSENS_DEVICE_STATE_ERROR,
   TSENS_DEVICE_STATE_READY
} TsensDeviceStateType;

typedef struct
{
   uint32 uNumSensors;
   uint32 uSensorsEnabledMask;
   TsensDeviceStateType eDeviceState;
   TsensBspType *pBsp;
   DALSYSSyncHandle hSync;
   DALSYS_SYNC_OBJECT(syncObject);
   DalDeviceHandle *phHWIO;
   DALBOOL bDataReady;
} TsensDeviceDataType;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
extern uint8 *gpuMpm2MpmBase;
extern uint8 *gpuSecCtrlBase;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static TsensDeviceDataType tsensDeviceData;
static TsensDeviceDataType *pDeviceData = NULL;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static void TsensLock(void)
{
   (void)DALSYS_SyncEnter(pDeviceData->hSync);
}

static void TsensUnLock(void)
{
   (void)DALSYS_SyncLeave(pDeviceData->hSync);
}

static DALResult Tsens_MapHWIORegion(const char *szBase,
                                     uint8 **ppuVirtAddr)
{
   DALResult status;

   status = DalHWIO_MapRegion(pDeviceData->phHWIO,
                              szBase,
                              ppuVirtAddr);

   return status;
}

static DALResult Tsens_GetTempAndCodeNoLock(uint32 uSensor, int32 *pnTempDegC, uint32 *puCode)
{
   DALResult status;
   uint32 uAdcCode;
   uint32 uAdcCodeTry1;
   uint32 uAdcCodeTry2;
   uint32 uAdcCodeTry3;
   boolean bValid;

   if ((1 << uSensor) & pDeviceData->uSensorsEnabledMask)
   {
      /* Read the ADC code */
      bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry1);
      if (bValid == TRUE)
      {
         uAdcCode = uAdcCodeTry1;
      }
      else
      {
         bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry2);
         if (bValid == TRUE)
         {
            uAdcCode = uAdcCodeTry2;
         }
         else
         {
            bValid = HAL_tsens_GetSensorPrevTemp(uSensor, &uAdcCodeTry3);
            if (bValid == TRUE)
            {
               uAdcCode = uAdcCodeTry3;
            }
            else if (uAdcCodeTry1 == uAdcCodeTry2)
            {
               uAdcCode = uAdcCodeTry1;
            }
            else if (uAdcCodeTry2 == uAdcCodeTry3)
            {
               uAdcCode = uAdcCodeTry2;
            }
            else
            {
               uAdcCode = uAdcCodeTry1;
            }
         }
      }
      *puCode = uAdcCode;
      status = Tsensi_ConvertToDegC(uSensor, uAdcCode, pnTempDegC);
   }
   else
   {
      status = TSENS_ERROR_UNSUPPORTED;
   }

   return status;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * Functions specific to the DalTsens interface
 * ----------------------------------------------------------------------*/
DALResult Tsens_GetTemp(TsensClientCtxt *pCtxt, uint32 uSensor, TsensTempType *pTemp)
{
   DALResult status;
   uint32 uAdcCode = 0;
   int32 nDelay_us;

   if (uSensor >= (sizeof(uint32) * 8))
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if (pTemp == NULL)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   if (pDeviceData->bDataReady == FALSE)
   {
      // Worst case wait is 1.5 ms. After that, data is ready unless TSENS gets disabled
      nDelay_us = (int32)(pDeviceData->uNumSensors * pDeviceData->pBsp->uSensorConvTime_us);

      do
      {
         if (HAL_tsens_TempMeasurementIsComplete() == FALSE)
         {
            DALSYS_BusyWait(TSENS_BUSY_WAIT_TIME);
            nDelay_us -= TSENS_BUSY_WAIT_TIME;
         }
         else
         {
            break;
         }

      } while (nDelay_us > 0);

      pDeviceData->bDataReady = TRUE;
   }

   TsensLock();
   status = Tsens_GetTempAndCodeNoLock(uSensor, &pTemp->nDegC, &uAdcCode);
   TsensUnLock();

   MSG_3(MSG_SSID_ADC, 
         MSG_LEGACY_HIGH, 
         "TSENS: Sensor = %u, DegC = %i, Code = 0x%x",
         uSensor,
         pTemp->nDegC,
         uAdcCode);

   return status;
}

DALResult Tsens_DeviceInit(TsensClientCtxt *pCtxt)
{
   DALResult status = DAL_SUCCESS;
   DALSYSPropertyVar propertyVar;
   TsensDevCtxt *pDevCtxt = pCtxt->pTsensDevCtxt;
   TsensBspType *pBsp;
   DalChipInfoFamilyType eChipInfoFamilyType;
   const char *pszAdcBspPropName = "tsens_bsp";

   pDeviceData = &tsensDeviceData;
   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_INIT;
   pDeviceData->bDataReady = FALSE;

   /* Get the chip family */
   eChipInfoFamilyType = DalChipInfo_ChipFamily();

   if (eChipInfoFamilyType == DALCHIPINFO_FAMILY_MSM8917)
   {
       pszAdcBspPropName = "tsens_bsp_8917";
   }

   /* Get the BSP */
   status = DALSYS_GetPropertyValue(pDevCtxt->hProp, 
                                    pszAdcBspPropName, 
                                    0, 
                                    &propertyVar);
   if (status != DAL_SUCCESS)
   {
      DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Tsens_DeviceInit : Device failed to get tsens_bsp property");

      goto error;
   }

   pDeviceData->pBsp = (TsensBspType *)propertyVar.Val.pStruct;
   
   pBsp = pDeviceData->pBsp;
   pDeviceData->uNumSensors = pBsp->uNumSensors;
   pDeviceData->uSensorsEnabledMask = pDeviceData->pBsp->uSensorEnableMask;

   /* Initialize synchronization object */
   status = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                              &pDeviceData->hSync,
                              &pDeviceData->syncObject);
   if (status != DAL_SUCCESS)
   {
      DALSYS_LogEvent((DALDEVICEID)DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Tsens_DeviceInit : Adc could not create device synchronization object\n");
      goto error;
   }
   
   /* Attach to the HWIO DAL and map the memory regions */
   status = DAL_DeviceAttach(DALDEVICEID_HWIO, &pDeviceData->phHWIO);
   if (status != DAL_SUCCESS)
   {
      DALSYS_LogEvent((DALDEVICEID)DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Tsens_DeviceInit : Failed to attach to HWIO with status=0x%08x\n", (unsigned int)status);
      goto error;
   }
   
   status = Tsens_MapHWIORegion("SECURITY_CONTROL", &gpuSecCtrlBase);
   if (status != DAL_SUCCESS)
   {
      DALSYS_LogEvent((DALDEVICEID)DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Tsens_DeviceInit : Failed to map memory for TSENS QFPROM registers with status=0x%08x\n", (unsigned int)status);
      goto error;
   }
   
   status = Tsens_MapHWIORegion("MPM2_MPM", &gpuMpm2MpmBase);
   if (status != DAL_SUCCESS)
   {
      DALSYS_LogEvent((DALDEVICEID)DALDEVICEID_TSENS, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Tsens_Init : Failed to map memory for TSENS control registers with status=0x%08x\n", (unsigned int)status);
      goto error;
   }

   /* Read the QFPROM registers and calculate the calibration parameters */   
   Tsensi_CalibrationInit(pBsp->uNumSensors, pBsp->panX1_default, pBsp->panM_default, pBsp->nY1, pBsp->nY2, pBsp->bTurboMode);

   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_READY;

   return DAL_SUCCESS;

error:
   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_ERROR;
   return status;
}

DALResult Tsens_DeviceDeInit(TsensClientCtxt *pCtxt)
{
   TsensLock();

   pDeviceData->bDataReady = FALSE;
   pDeviceData->eDeviceState = TSENS_DEVICE_STATE_INIT;

   TsensUnLock();

   return DAL_SUCCESS;
}

DALResult Tsens_GetNumSensors(TsensClientCtxt *pCtxt, uint32 *puNumSensors)
{
   if (puNumSensors == NULL)
   {
      return TSENS_ERROR_INVALID_PARAMETER;
   }

   *puNumSensors = pDeviceData->uNumSensors;

   return DAL_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Functions specific to the DAL interface
 * ----------------------------------------------------------------------*/
DALResult 
Tsens_DriverInit(TsensDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult 
Tsens_DriverDeInit(TsensDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult 
Tsens_PowerEvent(TsensClientCtxt *pCtxt, DalPowerCmd PowerCmd, DalPowerDomain PowerDomain)
{
   return DAL_SUCCESS;
}

DALResult 
Tsens_Open(TsensClientCtxt *pCtxt, uint32 dwaccessMode )
{
   return DAL_SUCCESS;
}

DALResult 
Tsens_Close(TsensClientCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult 
Tsens_Info(TsensClientCtxt *pCtxt, DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
   return DAL_SUCCESS;
}

