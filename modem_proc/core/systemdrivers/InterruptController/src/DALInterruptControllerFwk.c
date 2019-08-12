/*==============================================================================

FILE:      DalInterruptControllerFwk.c

DESCRIPTION: This file implements a InterruptController DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDIInterruptController.h"
#include "DALInterruptController.h"
#include "string.h" //for memset warning


DALResult 
InterruptController_DalInterruptController_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
InterruptController_DalInterruptController_AddRef(DalInterruptControllerHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
InterruptController_DalInterruptController_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	return dwref;
}

static DALResult 
InterruptController_DalInterruptController_Init(DalDeviceHandle *h)
{
   return InterruptController_DeviceInit(h->pClientCtxt);
}

static DALResult 
InterruptController_DalInterruptController_DeInit(DalDeviceHandle *h)
{
   return InterruptController_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
InterruptController_DalInterruptController_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return DAL_SUCCESS;
}

static DALResult 
InterruptController_DalInterruptController_Open(DalDeviceHandle* h, uint32 mode)
{
    return DAL_SUCCESS;
}

static DALResult 
InterruptController_DalInterruptController_Close(DalDeviceHandle* h)
{
    return DAL_SUCCESS;
}

static DALResult 
InterruptController_DalInterruptController_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALINTERRUPTCONTROLLER_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult 
InterruptController_DalInterruptController_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalInterruptController Interface. 
------------------------------------------------------------------------------*/

static DALResult 
InterruptController_DalInterruptController_RegisterISR( DalDeviceHandle * h, DALInterruptID  intrID, const DALISR  isr, const DALISRCtx  ctx, uint32  IntrFlags) 
{
	return InterruptController_RegisterISR(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID, isr, ctx, IntrFlags);
}

static DALResult 
InterruptController_DalInterruptController_RegisterIST( DalDeviceHandle * h, DALInterruptID  intrID, const DALISR  isr, const DALISRCtx  ctx, uint32  IntrFlags, char* pISTName) 
{
	return InterruptController_RegisterIST(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID, isr, ctx, IntrFlags, pISTName);
}

static DALResult 
InterruptController_DalInterruptController_RegisterEvent( DalDeviceHandle * h, DALInterruptID  intrID, const DALSYSEventHandle  hEvent, uint32  IntrFlags) 
{
	return InterruptController_RegisterEvent(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID, hEvent, IntrFlags);
}
static DALResult 
InterruptController_DalInterruptController_Unregister( DalDeviceHandle * h, DALInterruptID  intrID) 
{
	return InterruptController_Unregister(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
}
static DALResult 
InterruptController_DalInterruptController_InterruptDone( DalDeviceHandle * h, DALInterruptID  intrID) 
{
	return InterruptController_InterruptDone(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
}
static DALResult 
InterruptController_DalInterruptController_InterruptEnable( DalDeviceHandle * h, DALInterruptID  intrID) 
{
	return InterruptController_InterruptEnable(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
}
static DALResult 
InterruptController_DalInterruptController_InterruptDisable( DalDeviceHandle * h, DALInterruptID  intrID) 
{
	return InterruptController_InterruptDisable(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
}
static DALResult 
InterruptController_DalInterruptController_InterruptTrigger( DalDeviceHandle * h, DALInterruptID  intrID) 
{
	return InterruptController_InterruptTrigger(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
}
static DALResult 
InterruptController_DalInterruptController_InterruptClear( DalDeviceHandle * h, DALInterruptID  intrID) 
{
	return InterruptController_InterruptClear(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
}
static DALResult 
InterruptController_DalInterruptController_InterruptStatus( DalDeviceHandle * h, DALInterruptID  intrID) 
{
	return InterruptController_InterruptStatus(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
}
static DALResult 
InterruptController_DalInterruptController_RegisterIRQHandler( DalDeviceHandle * h, DALInterruptID  intrID, const DALIRQ  irq, const DALIRQCtx  ctx, uint32  bEnable) 
{
	return DAL_ERROR;
}
static DALResult 
InterruptController_DalInterruptController_SetTrigger( DalDeviceHandle * h, DALInterruptID intrID, uint32 nTrigger) 
{
	return InterruptController_SetTrigger(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID, nTrigger);
}
static DALResult
InterruptController_DalInterruptController_IsInterruptPending(DalDeviceHandle * h, DALInterruptID  intrID,void* bState,uint32 size)
{
  return InterruptController_IsInterruptPending(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID,(uint32*)bState);
}
static DALResult
InterruptController_DalInterruptController_IsInterruptEnabled(DalDeviceHandle * h, DALInterruptID  intrID,void* bState,uint32 size)
{
  return InterruptController_IsInterruptEnabled(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID,(uint32*)bState);
}
static DALResult
InterruptController_DalInterruptController_MapWakeupInterrupt(DalDeviceHandle * h, DALInterruptID  intrID,uint32 nWakeupIntID)
{
  return InterruptController_MapWakeupInterrupt(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID,nWakeupIntID);
}
static DALResult
InterruptController_DalInterruptController_IsAnyInterruptPending(DalDeviceHandle * h, uint32* bState,uint32 size)
{
  return InterruptController_IsAnyInterruptPending(((DalInterruptControllerHandle *)h)->pClientCtxt,bState);
}

static DALResult
InterruptController_DalInterruptController_Sleep(DalDeviceHandle * h, InterruptControllerSleepType sleep)
{
  return InterruptController_Sleep(((DalInterruptControllerHandle *)h)->pClientCtxt, sleep);
}

static DALResult
InterruptController_DalInterruptController_Wakeup(DalDeviceHandle * h,InterruptControllerSleepType sleep)
{
  return InterruptController_Wakeup(((DalInterruptControllerHandle *)h)->pClientCtxt,sleep);
}
static DALResult
InterruptController_DalInterruptController_GetInterruptTrigger(DalDeviceHandle * h,DALInterruptID intrID ,uint32* eTrigger, uint32 size)
{
  return InterruptController_GetInterruptTrigger(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID,eTrigger);
}

static DALResult
InterruptController_DalInterruptController_GetInterruptID(DalDeviceHandle * h,const char* szIntrName,uint32* pnIntrID, uint32 size)
{
  return InterruptController_GetInterruptID(((DalInterruptControllerHandle *)h)->pClientCtxt,szIntrName,pnIntrID);
}

static DALResult
InterruptController_DalInterruptController_LogState(DalDeviceHandle * h,void *pULog)
{
  return InterruptController_LogState(((DalInterruptControllerHandle *)h)->pClientCtxt,pULog);
}

static void 
InterruptController_InitInterface(InterruptControllerClientCtxt* pclientCtxt)
{
    static const DalInterruptController vtbl = {
       {
          InterruptController_DalInterruptController_Attach,
          InterruptController_DalInterruptController_Detach,
          InterruptController_DalInterruptController_Init,
          InterruptController_DalInterruptController_DeInit,
          InterruptController_DalInterruptController_Open,
          InterruptController_DalInterruptController_Close,
          InterruptController_DalInterruptController_Info,
          InterruptController_DalInterruptController_PowerEvent,
          InterruptController_DalInterruptController_SysRequest
        } ,
		InterruptController_DalInterruptController_RegisterISR,
		InterruptController_DalInterruptController_RegisterIST,
		InterruptController_DalInterruptController_RegisterEvent,
		InterruptController_DalInterruptController_Unregister,
		InterruptController_DalInterruptController_InterruptDone,
		InterruptController_DalInterruptController_InterruptEnable,
		InterruptController_DalInterruptController_InterruptDisable,
		InterruptController_DalInterruptController_InterruptTrigger,
		InterruptController_DalInterruptController_InterruptClear,
		InterruptController_DalInterruptController_InterruptStatus,
        InterruptController_DalInterruptController_RegisterIRQHandler,
        InterruptController_DalInterruptController_SetTrigger,
        InterruptController_DalInterruptController_IsInterruptPending,
        InterruptController_DalInterruptController_IsInterruptEnabled,
        InterruptController_DalInterruptController_MapWakeupInterrupt,
        InterruptController_DalInterruptController_IsAnyInterruptPending,
        InterruptController_DalInterruptController_Sleep,
        InterruptController_DalInterruptController_Wakeup,
        InterruptController_DalInterruptController_GetInterruptTrigger,
        InterruptController_DalInterruptController_GetInterruptID,
        InterruptController_DalInterruptController_LogState

    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalInterruptControllerHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalInterruptControllerHandle.pVtbl  = &vtbl;
    pclientCtxt->DalInterruptControllerHandle.pClientCtxt = pclientCtxt;

}

DALResult 
InterruptController_DalInterruptController_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static InterruptControllerDrvCtxt drvCtxt = {{InterruptController_DriverInit,
									   InterruptController_DriverDeInit
									  },1,
									  sizeof(InterruptControllerDevCtxt)};
    static InterruptControllerClientCtxt clientCtxt;

    InterruptControllerClientCtxt *pclientCtxt = &clientCtxt;


	*phDalDevice = NULL;

        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            InterruptController_InitInterface(pclientCtxt);
			InterruptController_DalInterruptController_AddRef(&(pclientCtxt->DalInterruptControllerHandle));
			*phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalInterruptControllerHandle);
        }
    return nErr;
}

