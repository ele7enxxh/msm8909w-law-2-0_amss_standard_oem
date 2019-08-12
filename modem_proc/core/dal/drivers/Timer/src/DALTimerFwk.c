/*==============================================================================

FILE:      DalTimerFwk.c

DESCRIPTION: This file implements a Timer DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDITimer.h"
#include "DALTimer.h"
#include "string.h" //for memset warning


DALResult 
Timer_DalTimer_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
Timer_DalTimer_AddRef(DalTimerHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

static void 
AllocateSystemWorkLoops(DALClientCtxt * pCtxt)
{
   DALSYSPropertyVar PropVar;
   DALSYSWorkLoopHandle hWorkLoop;
   uint32 i;
   DALDevCtxt *pDevCtxt = pCtxt->pDALDevCtxt;
   uint32 nWorkLoops = 1;  // default to one

   // If we have a property handle
   if(pDevCtxt->hProp[0])
   {
      /*------------------------------------------------------------------
      From DAL Props get the number of system workloops we are to allocate
      ------------------------------------------------------------------*/
   
      if(DAL_SUCCESS == DALSYS_GetPropertyValue(pDevCtxt->hProp, //Prop handle   
                                    "NumSysWorkLoops",     //Prop Name String
                                    0,                  //Not interested in Prop Id
                                    &PropVar))          //Output  
      {
         nWorkLoops = PropVar.Val.dwVal;
      }
   }
   if(nWorkLoops > DAL_MAX_NUM_SYSTEM_WORKLOOPS)
      nWorkLoops = DAL_MAX_NUM_SYSTEM_WORKLOOPS;
   for(i = 0; i<nWorkLoops; i++)
   {
      if(DAL_SUCCESS != DALSYS_RegisterWorkLoop(i,
                     20,  
                     &hWorkLoop, 
                     NULL ))
      {
         DALSYS_LogEvent(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR, "Unable to Allocate system workloops");
      }
     ((TimerClientCtxt *)pCtxt)->SystemTable[i+DAL_SYSTEM_TABLE_SYSTEM_WORKLOOPS] = (void *)hWorkLoop;
   }
   ((TimerClientCtxt *)pCtxt)->SystemTable[DAL_SYSTEM_TABLE_NUM_SYSTEM_WORKLOOPS] = (void *)nWorkLoops;  //index 0 contains actual num 
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
Timer_DalTimer_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	return dwref;
}

static DALResult 
Timer_DalTimer_Init(DalDeviceHandle *h)
{
    TimerClientCtxt *pTimerClientCtxt = (TimerClientCtxt *)(h->pClientCtxt);
	TimerDevCtxt *pTimerDevCtxt = pTimerClientCtxt->pTimerDevCtxt;
    /*----------------------------------------------------------------------
     Establish the DAL property handle for this device
    ----------------------------------------------------------------------*/
    DALSYS_GetDALPropertyHandle(pTimerDevCtxt->DevId,  pTimerDevCtxt->hProp);  
    if( DAL_SUCCESS == Timer_DeviceInit(pTimerClientCtxt))
    {
        AllocateSystemWorkLoops((DALClientCtxt *)pTimerClientCtxt);
        return DAL_SUCCESS;
    }
	return DAL_ERROR;
}

static DALResult 
Timer_DalTimer_DeInit(DalDeviceHandle *h)
{
   return Timer_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
Timer_DalTimer_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return Timer_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
Timer_DalTimer_Open(DalDeviceHandle* h, uint32 mode)
{
    return Timer_Open(h->pClientCtxt,mode);
}

static DALResult 
Timer_DalTimer_Close(DalDeviceHandle* h)
{
    return Timer_Close(h->pClientCtxt);
}

static DALResult 
Timer_DalTimer_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTIMER_INTERFACE_VERSION;
   return Timer_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
Timer_DalTimer_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   // Only supports this, returns pointer to table in *DestBufLenReq
   if(DALFW_SYSREQ_GET_SYSTEM_TABLE == ReqIdx)
   {
      *DestBufLenReq = (int)(&((TimerClientCtxt *)(h->pClientCtxt))->SystemTable[0]);
      return DAL_SUCCESS;
   }
   return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTimer Interface. 
------------------------------------------------------------------------------*/

static DALResult 
Timer_DalTimer_Register( DalDeviceHandle * h, const DALSYSEventHandle  hEvent, uint32  time) 
{
	return Timer_Register(((DalTimerHandle *)h)->pClientCtxt, hEvent, time);
}
static DALResult 
Timer_DalTimer_UnRegister( DalDeviceHandle * h, const DALSYSEventHandle  hEvent) 
{
	return Timer_UnRegister(((DalTimerHandle *)h)->pClientCtxt, hEvent);
}
static DALResult 
Timer_DalTimer_GetTimerCount( DalDeviceHandle * h, void * timer_count,  uint32 size) 
{
	return Timer_GetTimerCount(((DalTimerHandle *)h)->pClientCtxt, timer_count, size);
}

static void 
Timer_InitInterface(TimerClientCtxt* pclientCtxt)
{
    static const DalTimer vtbl = {
       {
          Timer_DalTimer_Attach,
          Timer_DalTimer_Detach,
          Timer_DalTimer_Init,
          Timer_DalTimer_DeInit,
          Timer_DalTimer_Open,
          Timer_DalTimer_Close,
          Timer_DalTimer_Info,
          Timer_DalTimer_PowerEvent,
          Timer_DalTimer_SysRequest
        } ,
      Timer_DalTimer_Register,
      Timer_DalTimer_UnRegister,
      Timer_DalTimer_GetTimerCount
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalTimerHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalTimerHandle.pVtbl  = &vtbl;
    pclientCtxt->DalTimerHandle.pClientCtxt = pclientCtxt;

}

DALResult 
Timer_DalTimer_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static TimerDrvCtxt drvCtxt = {{Timer_DriverInit,
									   Timer_DriverDeInit
									  },1,
									  sizeof(TimerDevCtxt)};
    static TimerClientCtxt clientCtxt;

    TimerClientCtxt *pclientCtxt = &clientCtxt;


	*phDalDevice = NULL;

        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            Timer_InitInterface(pclientCtxt);
			Timer_DalTimer_AddRef(&(pclientCtxt->DalTimerHandle));
			*phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTimerHandle);
        }
    return nErr;
}

