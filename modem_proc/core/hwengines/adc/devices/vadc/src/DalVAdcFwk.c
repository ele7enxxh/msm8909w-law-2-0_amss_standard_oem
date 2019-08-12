/*============================================================================
  FILE:         DALVAdcFwk.c

  OVERVIEW:     Implementation of a Physical ADC device DAL for the PMIC4
                VADC module.
                
                Refer to 80-VB073-115 for the VADC Module Data Sheet.

  DEPENDENCIES: None
 
                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/devices/vadc/src/DalVAdcFwk.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-01-31  sa   Fix KW warning (free memory on error return).
  2013-06-19  jjo  Added support for multiple VADCs
  2012-17-10  jjo  Added synchronous call stubs.
  2012-06-13  jjo  Updated to use a DAL device ID string name.
  2009-03-16  jdt  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DALSys.h"
#include "DDIAdcDevice.h"
#include "DalVAdc.h"
#include "DALSys.h"
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
VAdc_DalAdcDevice_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
VAdcDrvCtxt gVAdcDrvCtxt = {{VAdc_DriverInit, VAdc_DriverDeInit},
                             VADC_MAX_NUM_DEVICES,
                             sizeof(VAdcDevCtxt)};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static uint32 
VAdc_DalAdcDevice_AddRef(DalAdcDeviceHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
VAdc_DalAdcDevice_Detach(DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult 
VAdc_DalAdcDevice_Init(DalDeviceHandle *h)
{  
   return VAdc_DeviceInit(h->pClientCtxt);
}

static DALResult 
VAdc_DalAdcDevice_DeInit(DalDeviceHandle *h)
{
   return VAdc_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
VAdc_DalAdcDevice_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
   return VAdc_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
VAdc_DalAdcDevice_Open(DalDeviceHandle* h, uint32 mode)
{
   return VAdc_Open(h->pClientCtxt,mode);
}

static DALResult 
VAdc_DalAdcDevice_Close(DalDeviceHandle* h)
{
   return VAdc_Close(h->pClientCtxt);
}

static DALResult 
VAdc_DalAdcDevice_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALADCDEVICE_INTERFACE_VERSION;
   return VAdc_Info(h->pClientCtxt, info, infoSize);
}

static DALResult 
VAdc_DalAdcDevice_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalAdcDevice Interface. 
------------------------------------------------------------------------------*/
static DALResult 
VAdc_DalAdcDevice_GetInputProperties(DalDeviceHandle * h, const char *pChannelName, uint32 *puChannelIdx)
{
   return VAdc_GetChannel(((DalAdcDeviceHandle *)h)->pClientCtxt, pChannelName, puChannelIdx);
}

static DALResult 
VAdc_DalAdcDevice_GetDeviceProperties( DalDeviceHandle * h, AdcDeviceDevicePropertiesType *pAdcDeviceProp)
{
	return VAdc_GetDeviceProperties(((DalAdcDeviceHandle *)h)->pClientCtxt, pAdcDeviceProp);
}

static DALResult 
VAdc_DalAdcDevice_GetChannelStatus( DalDeviceHandle * h, uint32 uChannelIdx, AdcDeviceChannelStatusType *pAdcChannelStatus)
{
	return VAdc_GetChannelStatus(((DalAdcDeviceHandle *)h)->pClientCtxt, uChannelIdx, pAdcChannelStatus);
}

static DALResult 
VAdc_DalAdcDevice_Calibrate( DalDeviceHandle * h, const DALSYSEventHandle hEvent, uint32 uEventParam)
{
	return VAdc_Calibrate(((DalAdcDeviceHandle *)h)->pClientCtxt, hEvent, uEventParam);
}

static DALResult 
VAdc_DalAdcDevice_RecalibrateChannel( DalDeviceHandle * h, uint32 uChannelIdx, const DALSYSEventHandle hEvent, uint32 uEventParam)
{
	return VAdc_RecalibrateChannel(((DalAdcDeviceHandle *)h)->pClientCtxt, uChannelIdx, hEvent, uEventParam);
}

static DALResult 
VAdc_DalAdcDevice_StartConversion(DalDeviceHandle * h, uint32 uChannelIdx, const DALSYSEventHandle hEvent, uint32 uEventParam)
{
	return VAdc_StartConversion(((DalAdcDeviceHandle *)h)->pClientCtxt, uChannelIdx, hEvent, uEventParam);
}

static DALResult 
VAdc_DalAdcDevice_RecalibrateChannelSync( DalDeviceHandle * h,  uint32 uChannelIdx, AdcDeviceRecalibrateResultType *pAdcDeviceRecalibrateResult)
{
   return DAL_ERROR;
}

static DALResult 
VAdc_DalAdcDevice_ReadChannelSync( DalDeviceHandle * h,  uint32 uChannelIdx, AdcDeviceResultType *pAdcDeviceReadResult)
{
   return DAL_ERROR;
}

static void 
VAdc_InitInterface(VAdcClientCtxt* pclientCtxt)
{
    static const DalAdcDevice vtbl = {
       {
          VAdc_DalAdcDevice_Attach,
          VAdc_DalAdcDevice_Detach,
          VAdc_DalAdcDevice_Init,
          VAdc_DalAdcDevice_DeInit,
          VAdc_DalAdcDevice_Open,
          VAdc_DalAdcDevice_Close,
          VAdc_DalAdcDevice_Info,
          VAdc_DalAdcDevice_PowerEvent,
          VAdc_DalAdcDevice_SysRequest
        },
        VAdc_DalAdcDevice_GetInputProperties,
        VAdc_DalAdcDevice_GetDeviceProperties,
        VAdc_DalAdcDevice_GetChannelStatus,
        VAdc_DalAdcDevice_Calibrate,
        VAdc_DalAdcDevice_RecalibrateChannel,
        VAdc_DalAdcDevice_StartConversion,
        VAdc_DalAdcDevice_RecalibrateChannelSync,
        VAdc_DalAdcDevice_ReadChannelSync
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalAdcDeviceHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalAdcDeviceHandle.pVtbl  = &vtbl;
    pclientCtxt->DalAdcDeviceHandle.pClientCtxt = pclientCtxt;

}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
DALResult 
VAdc_DalAdcDevice_Attach(const char *pszArg,
                         DALDEVICEID DeviceId,
                         DalDeviceHandle **phDalDevice)
{               
   DALResult nErr = DAL_ERROR;
   VAdcClientCtxt *pClientCtxt = NULL;

   *phDalDevice = NULL;

   /* Allocate memory for the client context */
   nErr = DALSYS_Malloc(sizeof(VAdcClientCtxt),(void **)&pClientCtxt);
   if(NULL == pClientCtxt || DAL_SUCCESS != nErr)
   {
      goto error;
   }
   DALSYS_memset(pClientCtxt, 0, sizeof(VAdcClientCtxt));

   if (DeviceId != 0)
   {
      nErr = DALFW_AttachToDevice(DeviceId,
                                  (DALDrvCtxt *)&gVAdcDrvCtxt,
                                  (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the string device name is available */
      nErr = DALSYS_GetDALPropertyHandle(DeviceId, pClientCtxt->pVAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }
   else
   {
      nErr = DALFW_AttachToStringDevice(pszArg,
                                        (DALDrvCtxt *)&gVAdcDrvCtxt,
                                        (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the string device name is available */
      nErr = DALSYS_GetDALPropertyHandleStr(pszArg, pClientCtxt->pVAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }

   VAdc_InitInterface(pClientCtxt);
   (void)VAdc_DalAdcDevice_AddRef(&(pClientCtxt->DalAdcDeviceHandle));
   *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalAdcDeviceHandle);

   return DAL_SUCCESS;

error:
   if (pClientCtxt != NULL)
   {
      DALSYS_Free(pClientCtxt);
   }

   return nErr;
}

