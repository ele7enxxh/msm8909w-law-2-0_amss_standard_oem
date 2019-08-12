/*============================================================================
  FILE:         DALTsensFwk.c

  OVERVIEW:     Implementation of the TSENS DAL Framework

  DEPENDENCIES: None

                Copyright (c) 2010-2012, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/tsens/dal/src/DALTsensFwk.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $
 
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-02-03  jjo  Ported to modem.
 
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DALSys.h"
#include "DDITsens.h"
#include "DALTsens.h"
#include "string.h" //for memset warning

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/
DALResult
Tsens_DalTsens_Attach(const char *, DALDEVICEID,DalDeviceHandle **);

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
TsensDrvCtxt gTsensDrvCtxt = {{Tsens_DriverInit, Tsens_DriverDeInit},
                              1,
                              sizeof(TsensDevCtxt)};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static uint32
Tsens_DalTsens_AddRef(DalTsensHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/
static uint32
Tsens_DalTsens_Detach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   if (0 == dwref)
   {
      DALSYS_Free(h->pClientCtxt);
   }
   return dwref;
}

static DALResult
Tsens_DalTsens_Init(DalDeviceHandle *h)
{
   return Tsens_DeviceInit(h->pClientCtxt);
}

static DALResult
Tsens_DalTsens_DeInit(DalDeviceHandle *h)
{
   return Tsens_DeviceDeInit(h->pClientCtxt);
}

static DALResult
Tsens_DalTsens_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return Tsens_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult
Tsens_DalTsens_Open(DalDeviceHandle* h, uint32 mode)
{
   return Tsens_Open(h->pClientCtxt,mode);
}

static DALResult
Tsens_DalTsens_Close(DalDeviceHandle* h)
{
   return Tsens_Close(h->pClientCtxt);
}

static DALResult
Tsens_DalTsens_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTSENS_INTERFACE_VERSION;
   return Tsens_Info(h->pClientCtxt,info,infoSize);
}

static DALResult
Tsens_DalTsens_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                          const void* SrcBuf, uint32 SrcBufLen,
                          void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTsens Interface.
------------------------------------------------------------------------------*/
static DALResult
Tsens_DalTsens_GetTemp( DalDeviceHandle * h, uint32 nSensor, TsensTempType *pTemp)
{
   return Tsens_GetTemp(((DalTsensHandle *)h)->pClientCtxt, nSensor, pTemp);
}

static DALResult
Tsens_DalTsens_GetTempRange( DalDeviceHandle * h, uint32 nSensor, TsensTempRangeType * pTsensTempRange)
{
   return DAL_ERROR;
}

static DALResult
Tsens_DalTsens_SetThreshold( DalDeviceHandle * h, uint32 nSensor, TsensThresholdType eThreshold, int32 nDegC, DALSYSEventHandle hEvent)
{
   return DAL_ERROR;
}

static DALResult
Tsens_DalTsens_SetEnableThresholds( DalDeviceHandle * h, DALBOOL bEnableThresholds)
{
   return DAL_ERROR;
}

static DALResult
Tsens_DalTsens_SetCriticalThreshold( DalDeviceHandle * h, uint32 nSensor, TsensCriticalThresholdType eThreshold, int32 nDegC)
{
   return DAL_ERROR;
}

static DALResult
Tsens_DalTsens_GetNumSensors( DalDeviceHandle * h, uint32 *pnNumSensors)
{
   return Tsens_GetNumSensors(((DalTsensHandle *)h)->pClientCtxt, pnNumSensors);
}

static DALResult
Tsens_DalTsens_CheckCalibration( DalDeviceHandle * h, uint32 nSensor)
{
   return DAL_ERROR;
}


static void
Tsens_InitInterface(TsensClientCtxt* pClientCtxt)
{
   static const DalTsens vtbl = {
      {
         Tsens_DalTsens_Attach,
         Tsens_DalTsens_Detach,
         Tsens_DalTsens_Init,
         Tsens_DalTsens_DeInit,
         Tsens_DalTsens_Open,
         Tsens_DalTsens_Close,
         Tsens_DalTsens_Info,
         Tsens_DalTsens_PowerEvent,
         Tsens_DalTsens_SysRequest
      },
      Tsens_DalTsens_GetTemp,
      Tsens_DalTsens_GetTempRange,
      Tsens_DalTsens_SetThreshold,
      Tsens_DalTsens_SetEnableThresholds,
      Tsens_DalTsens_SetCriticalThreshold,
      Tsens_DalTsens_GetNumSensors,
      Tsens_DalTsens_CheckCalibration
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pClientCtxt->DalTsensHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pClientCtxt->DalTsensHandle.pVtbl = &vtbl;
   pClientCtxt->DalTsensHandle.pClientCtxt = pClientCtxt;

}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
DALResult
Tsens_DalTsens_Attach(const char *pszArg, 
                      DALDEVICEID DeviceId,
                      DalDeviceHandle **phDalDevice)
{
   DALResult nErr = DAL_ERROR;
   TsensClientCtxt *pClientCtxt = NULL;

   *phDalDevice = NULL;

   /* Allocate memory for the client context */
   nErr = DALSYS_Malloc(sizeof(TsensClientCtxt), (void **)&pClientCtxt);
   if ((NULL == pClientCtxt) || (DAL_SUCCESS != nErr))
   {
      nErr = DAL_ERROR;
      goto error;
   }
   DALSYS_memset(pClientCtxt, 0, sizeof(TsensClientCtxt));

   if (DeviceId != 0)
   {
      nErr = DALFW_AttachToDevice(DeviceId,
                                  (DALDrvCtxt *)&gTsensDrvCtxt,
                                  (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the device ID is available */
      nErr = DALSYS_GetDALPropertyHandle(DeviceId, pClientCtxt->pTsensDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }
   else
   {
      nErr = DALFW_AttachToStringDevice(pszArg,
                                        (DALDrvCtxt *)&gTsensDrvCtxt,
                                        (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the string device name is available */
      nErr = DALSYS_GetDALPropertyHandleStr(pszArg, pClientCtxt->pTsensDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }

   Tsens_InitInterface(pClientCtxt);
   (void)Tsens_DalTsens_AddRef(&(pClientCtxt->DalTsensHandle));
   *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalTsensHandle);

   return DAL_SUCCESS;

error:
   if (pClientCtxt != NULL)
   {
      DALSYS_Free(pClientCtxt);
   }

   return nErr;
}

