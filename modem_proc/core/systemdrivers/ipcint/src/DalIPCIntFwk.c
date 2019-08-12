/*==============================================================================

FILE:      DalIPCIntFwk.c

DESCRIPTION: This file implements a IPCInt DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDIIPCInt.h"
#include "DalIPCInt.h"
#include "DALSys.h"
#include "string.h" //for memset warning


DALResult 
IPCInt_DalIPCInt_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
IPCInt_DalIPCInt_AddRef(DalIPCIntHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
IPCInt_DalIPCInt_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	return dwref;
}

static DALResult 
IPCInt_DalIPCInt_Init(DalDeviceHandle *h)
{  
   IPCIntClientCtxt *pClientCtxt = (IPCIntClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pIPCIntDevCtxt->DevId,
	                           pClientCtxt->pIPCIntDevCtxt->hProp);
   return IPCInt_DeviceInit(h->pClientCtxt);
}

static DALResult 
IPCInt_DalIPCInt_DeInit(DalDeviceHandle *h)
{
   return IPCInt_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
IPCInt_DalIPCInt_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return IPCInt_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
IPCInt_DalIPCInt_Open(DalDeviceHandle* h, uint32 mode)
{
    return IPCInt_Open(h->pClientCtxt,mode);
}

static DALResult 
IPCInt_DalIPCInt_Close(DalDeviceHandle* h)
{
    return IPCInt_Close(h->pClientCtxt);
}

static DALResult 
IPCInt_DalIPCInt_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALIPCINT_INTERFACE_VERSION;
   return IPCInt_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
IPCInt_DalIPCInt_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalIPCInt Interface. 
------------------------------------------------------------------------------*/

static DALResult 
IPCInt_DalIPCInt_Trigger( DalDeviceHandle * h,  DalIPCIntProcessorType  eTarget,  DalIPCIntInterruptType  eInterrupt) 
{
	return IPCInt_Trigger(((DalIPCIntHandle *)h)->pClientCtxt, eTarget, eInterrupt);
}
static DALResult 
IPCInt_DalIPCInt_TriggerBySource( DalDeviceHandle * h,  DalIPCIntProcessorType  eSource,  DalIPCIntProcessorType  eTarget,  DalIPCIntInterruptType  eInterrupt) 
{
	return IPCInt_TriggerBySource(((DalIPCIntHandle *)h)->pClientCtxt, eSource, eTarget, eInterrupt);
}
static DALResult 
IPCInt_DalIPCInt_IsSupported( DalDeviceHandle * h,  DalIPCIntProcessorType  eTarget,  DalIPCIntInterruptType  eInterrupt) 
{
	return IPCInt_IsSupported(((DalIPCIntHandle *)h)->pClientCtxt, eTarget, eInterrupt);
}
static DALResult 
IPCInt_DalIPCInt_IsSupportedBySource( DalDeviceHandle * h,  DalIPCIntProcessorType  eSource,  DalIPCIntProcessorType  eTarget,  DalIPCIntInterruptType  eInterrupt) 
{
	return IPCInt_IsSupportedBySource(((DalIPCIntHandle *)h)->pClientCtxt, eSource, eTarget, eInterrupt);
}

static void 
IPCInt_InitInterface(IPCIntClientCtxt* pclientCtxt)
{
    static const DalIPCInt vtbl = {
       {
          IPCInt_DalIPCInt_Attach,
          IPCInt_DalIPCInt_Detach,
          IPCInt_DalIPCInt_Init,
          IPCInt_DalIPCInt_DeInit,
          IPCInt_DalIPCInt_Open,
          IPCInt_DalIPCInt_Close,
          IPCInt_DalIPCInt_Info,
          IPCInt_DalIPCInt_PowerEvent,
          IPCInt_DalIPCInt_SysRequest
        } ,
		IPCInt_DalIPCInt_Trigger,
		IPCInt_DalIPCInt_TriggerBySource,
		IPCInt_DalIPCInt_IsSupported,
		IPCInt_DalIPCInt_IsSupportedBySource
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalIPCIntHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalIPCIntHandle.pVtbl  = &vtbl;
    pclientCtxt->DalIPCIntHandle.pClientCtxt = pclientCtxt;

}

DALResult 
IPCInt_DalIPCInt_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static IPCIntDrvCtxt drvCtxt = {{IPCInt_DriverInit,
									   IPCInt_DriverDeInit
									  },1,
									  sizeof(IPCIntDevCtxt)};
    static IPCIntClientCtxt clientCtxt;

    IPCIntClientCtxt *pclientCtxt = &clientCtxt;


	*phDalDevice = NULL;

        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            IPCInt_InitInterface(pclientCtxt);
			IPCInt_DalIPCInt_AddRef(&(pclientCtxt->DalIPCIntHandle));
			*phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalIPCIntHandle);
        }
    return nErr;
}

