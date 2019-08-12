/*==============================================================================

FILE:      DalSoftwareSwitchesFwk.c

DESCRIPTION: This file implements a SoftwareSwitches DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDISoftwareSwitches.h"
#include "DalSoftwareSwitches.h"
#include "DALSys.h"
#include "string.h" //for memset warning


DALResult 
SoftwareSwitches_DalSoftwareSwitches_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
SoftwareSwitches_DalSoftwareSwitches_AddRef(DalSoftwareSwitchesHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
SoftwareSwitches_DalSoftwareSwitches_Detach(DalDeviceHandle* h)
{
  return DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
}

static DALResult 
SoftwareSwitches_DalSoftwareSwitches_Init(DalDeviceHandle *h)
{  
   SoftwareSwitchesClientCtxt *pClientCtxt = (SoftwareSwitchesClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pSoftwareSwitchesDevCtxt->DevId,
	                           pClientCtxt->pSoftwareSwitchesDevCtxt->hProp);
   return SoftwareSwitches_DeviceInit(h->pClientCtxt);
}

static DALResult 
SoftwareSwitches_DalSoftwareSwitches_DeInit(DalDeviceHandle *h)
{
   return SoftwareSwitches_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
SoftwareSwitches_DalSoftwareSwitches_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return SoftwareSwitches_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
SoftwareSwitches_DalSoftwareSwitches_Open(DalDeviceHandle* h, uint32 mode)
{
    return SoftwareSwitches_Open(h->pClientCtxt,mode);
}

static DALResult 
SoftwareSwitches_DalSoftwareSwitches_Close(DalDeviceHandle* h)
{
    return SoftwareSwitches_Close(h->pClientCtxt);
}

static DALResult 
SoftwareSwitches_DalSoftwareSwitches_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSOFTWARESWITCHES_INTERFACE_VERSION;
   return SoftwareSwitches_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
SoftwareSwitches_DalSoftwareSwitches_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalSoftwareSwitches Interface. 
------------------------------------------------------------------------------*/

static DALResult 
SoftwareSwitches_DalSoftwareSwitches_SetSwitch( DalDeviceHandle * h, uint32  software_switch) 
{
	return SoftwareSwitches_SetSwitch(((DalSoftwareSwitchesHandle *)h)->pClientCtxt, software_switch);
}
static DALResult 
SoftwareSwitches_DalSoftwareSwitches_ClearSwitch( DalDeviceHandle * h, uint32  software_switch) 
{
	return SoftwareSwitches_ClearSwitch(((DalSoftwareSwitchesHandle *)h)->pClientCtxt, software_switch);
}
static DALResult 
SoftwareSwitches_DalSoftwareSwitches_SetAll( DalDeviceHandle * h, uint32  software_switch) 
{
	return SoftwareSwitches_SetAll(((DalSoftwareSwitchesHandle *)h)->pClientCtxt, software_switch);
}
static DALResult 
SoftwareSwitches_DalSoftwareSwitches_GetMask( DalDeviceHandle * h, uint32  software_switch,  void * state, uint32  size) 
{
	return SoftwareSwitches_GetMask(((DalSoftwareSwitchesHandle *)h)->pClientCtxt, software_switch, (uint32*)state);
}

static void 
SoftwareSwitches_InitInterface(SoftwareSwitchesClientCtxt* pclientCtxt)
{
    static const DalSoftwareSwitches vtbl = {
       {
          SoftwareSwitches_DalSoftwareSwitches_Attach,
          SoftwareSwitches_DalSoftwareSwitches_Detach,
          SoftwareSwitches_DalSoftwareSwitches_Init,
          SoftwareSwitches_DalSoftwareSwitches_DeInit,
          SoftwareSwitches_DalSoftwareSwitches_Open,
          SoftwareSwitches_DalSoftwareSwitches_Close,
          SoftwareSwitches_DalSoftwareSwitches_Info,
          SoftwareSwitches_DalSoftwareSwitches_PowerEvent,
          SoftwareSwitches_DalSoftwareSwitches_SysRequest
        } ,
		SoftwareSwitches_DalSoftwareSwitches_SetSwitch,
		SoftwareSwitches_DalSoftwareSwitches_ClearSwitch,
		SoftwareSwitches_DalSoftwareSwitches_SetAll,
		SoftwareSwitches_DalSoftwareSwitches_GetMask
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalSoftwareSwitchesHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalSoftwareSwitchesHandle.pVtbl  = &vtbl;
    pclientCtxt->DalSoftwareSwitchesHandle.pClientCtxt = pclientCtxt;

}

DALResult 
SoftwareSwitches_DalSoftwareSwitches_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static SoftwareSwitchesDrvCtxt drvCtxt = {{SoftwareSwitches_DriverInit,
									   SoftwareSwitches_DriverDeInit
									  },1,
									  sizeof(SoftwareSwitchesDevCtxt)};

  static SoftwareSwitchesClientCtxt ClientCtxt;

  SoftwareSwitchesClientCtxt *pclientCtxt =  &ClientCtxt;
  *phDalDevice = NULL;

  nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
  if (DAL_SUCCESS == nErr)
  {
    SoftwareSwitches_InitInterface(pclientCtxt);
    SoftwareSwitches_DalSoftwareSwitches_AddRef(&(pclientCtxt->DalSoftwareSwitchesHandle));
    *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalSoftwareSwitchesHandle);
  }
      
  return nErr;
}
