/*============================================================================
  FILE:         DALAdcFwk.c

  OVERVIEW:     Implementation of the ADC DAL

  DEPENDENCIES: None

                Copyright (c) 2009-2012 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/dal/src/DalAdcFwk.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-04-19  jdt  Updated for 8974.
  2010-06-20 Purna klock work fix in Adc_DalAdc_Attach
  2009-03-16  jdt  Initial revision.

============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDIAdc.h"
#include "DalAdc.h"

static DALBOOL init_done = FALSE;

AdcClientCtxt *gpAdcClientCtxt;

DALResult
Adc_DalAdc_Attach(const char *, DALDEVICEID,DalDeviceHandle **);

static uint32
Adc_DalAdc_AddRef(DalAdcHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32
Adc_DalAdc_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	 if(0 == dwref)	{
		//Release the client ctxt;
        DALSYS_Free(h->pClientCtxt);
	}
	return dwref;
}

static DALResult
Adc_DalAdc_Init(DalDeviceHandle *h)
{
   DALResult result;
   AdcClientCtxt *pClientCtxt = (AdcClientCtxt *)(h->pClientCtxt);

   if (init_done == TRUE)
   {
      return DAL_SUCCESS;
   }

   (void)DALSYS_GetDALPropertyHandle(pClientCtxt->pAdcDevCtxt->DevId,
	                           pClientCtxt->pAdcDevCtxt->hProp);
   result = Adc_DeviceInit(h->pClientCtxt);
   if (result == DAL_SUCCESS)
   {
      init_done = TRUE;
   }

   return result;
}

static DALResult
Adc_DalAdc_DeInit(DalDeviceHandle *h)
{
   return Adc_DeviceDeInit(h->pClientCtxt);
}

static DALResult
Adc_DalAdc_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
    return Adc_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult
Adc_DalAdc_Open(DalDeviceHandle* h, uint32 mode)
{
    return Adc_Open(h->pClientCtxt,mode);
}

static DALResult
Adc_DalAdc_Close(DalDeviceHandle* h)
{
    return Adc_Close(h->pClientCtxt);
}

static DALResult
Adc_DalAdc_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALADC_INTERFACE_VERSION;
   return Adc_Info(h->pClientCtxt,info,infoSize);
}

static DALResult
Adc_DalAdc_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalAdc Interface.
------------------------------------------------------------------------------*/

static DALResult
Adc_DalAdc_GetAdcInputProperties( DalDeviceHandle * h,  const char * pInputName,  uint32  nInputNameSize,  AdcInputPropertiesType * pAdcInputProps)
{
	return Adc_GetAdcInputProperties(((DalAdcHandle *)h)->pClientCtxt, pInputName, nInputNameSize, pAdcInputProps);
}
static DALResult
Adc_DalAdc_RequestConversion( DalDeviceHandle * h,  AdcRequestParametersType *pAdcParams, AdcRequestStatusType * pAdcRequestStatus)
{
	return Adc_RequestConversion(((DalAdcHandle *)h)->pClientCtxt, pAdcParams, pAdcRequestStatus);
}
static DALResult
Adc_DalAdc_RequestRecalibration( DalDeviceHandle * h,  AdcRequestParametersType *pAdcParams)
{
	return Adc_RequestRecalibration(((DalAdcHandle *)h)->pClientCtxt, pAdcParams);
}

static void
Adc_InitInterface(AdcClientCtxt* pclientCtxt)
{
    static const DalAdc vtbl = {
       {
          Adc_DalAdc_Attach,
          Adc_DalAdc_Detach,
          Adc_DalAdc_Init,
          Adc_DalAdc_DeInit,
          Adc_DalAdc_Open,
          Adc_DalAdc_Close,
          Adc_DalAdc_Info,
          Adc_DalAdc_PowerEvent,
          Adc_DalAdc_SysRequest
        } ,
      Adc_DalAdc_RequestConversion,
      Adc_DalAdc_RequestRecalibration,
      Adc_DalAdc_GetAdcInputProperties,
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalAdcHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalAdcHandle.pVtbl = &vtbl;
   pclientCtxt->DalAdcHandle.pClientCtxt = pclientCtxt;
}

DALResult
Adc_DalAdc_Attach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
    DALResult nErr;
        static AdcDrvCtxt drvCtxt = {{Adc_DriverInit, Adc_DriverDeInit},
                                      1,
                                      sizeof(AdcDevCtxt)};

    AdcClientCtxt *pclientCtxt =  NULL;

    nErr = DALSYS_Malloc(sizeof(AdcClientCtxt),(void **)&pclientCtxt);
    if((NULL == pclientCtxt) || (DAL_SUCCESS != nErr))
    {
       return nErr;
    }
    DALSYS_memset(pclientCtxt,0,sizeof(AdcClientCtxt));

    gpAdcClientCtxt = pclientCtxt;

    *phDalDevice = NULL;

     if (DAL_SUCCESS == nErr) {
        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
                                    (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            Adc_InitInterface(pclientCtxt);
            (void)Adc_DalAdc_AddRef(&(pclientCtxt->DalAdcHandle));
            *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalAdcHandle);
        }
    }
    return nErr;
}

