/*==============================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R  F R A M E W O R K  

DESCRIPTION
  This file implements a GPIOInt DeviceDriver.
  The definitions and api in this module are used internally 
  in the GPIO interrupt controller and are not to be exposed to the clients.

REFERENCES

       Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/GPIOInt/src/GPIOIntFwk.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDIGPIOInt.h"
#include "GPIOInt.h"
#include "string.h" //for memset warning


DALResult 
GPIOInt_GPIOInt_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
GPIOInt_GPIOInt_AddRef(GPIOIntHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
GPIOInt_GPIOInt_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	 if(0 == dwref)	{
		//Release the client ctxt;
        DALSYS_Free(h->pClientCtxt);
	}
	return dwref;
}

static DALResult 
GPIOInt_GPIOInt_Init(DalDeviceHandle *h)
{  
   GPIOIntClientCtxt *pClientCtxt = (GPIOIntClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pGPIOIntDevCtxt->DevId,
	                           pClientCtxt->pGPIOIntDevCtxt->hProp);
   return GPIOIntr_DeviceInit(h->pClientCtxt);
}

static DALResult 
GPIOInt_GPIOInt_DeInit(DalDeviceHandle *h)
{
   return GPIOIntr_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
GPIOInt_GPIOInt_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return GPIOIntr_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
GPIOInt_GPIOInt_Open(DalDeviceHandle* h, uint32 mode)
{
    return GPIOIntr_Open(h->pClientCtxt,mode);
}

static DALResult 
GPIOInt_GPIOInt_Close(DalDeviceHandle* h)
{
    return GPIOIntr_Close(h->pClientCtxt);
}

static DALResult 
GPIOInt_GPIOInt_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = GPIOINT_INTERFACE_VERSION;
   return GPIOIntr_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
GPIOInt_GPIOInt_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalGPIOInt Interface. 
------------------------------------------------------------------------------*/

static DALResult 
GPIOInt_GPIOInt_SetTrigger( DalDeviceHandle * h, uint32  gpio, GPIOIntTriggerType  trigger) 
{
	return GPIOIntr_SetTrigger(((GPIOIntHandle *)h)->pClientCtxt, gpio, trigger);
}
static DALResult 
GPIOInt_GPIOInt_RegisterIsr( DalDeviceHandle * h, uint32  gpio, GPIOIntTriggerType trigger, 
                             GPIOINTISR isr,GPIOINTISRCtx param) 
{
	return GPIOIntr_RegisterIsr(((GPIOIntHandle *)h)->pClientCtxt, gpio, trigger,isr,param);
}
static DALResult 
GPIOInt_GPIOInt_RegisterEvent( DalDeviceHandle * h, uint32 gpio,GPIOIntTriggerType trigger, 
                                                                  DALSYSEventHandle event) 
{
	return GPIOIntr_RegisterEvent(((GPIOIntHandle *)h)->pClientCtxt,gpio,trigger,event);
}
static DALResult 
GPIOInt_GPIOInt_DeRegisterEvent( DalDeviceHandle * h, uint32 gpio,DALSYSEventHandle event) 
{
	return GPIOIntr_DeRegisterEvent(((GPIOIntHandle *)h)->pClientCtxt, gpio,event);
}
static DALResult 
GPIOInt_GPIOInt_DeregisterIsr( DalDeviceHandle * h, uint32  gpio, GPIOINTISR isr) 
{
	return GPIOIntr_DeregisterIsr(((GPIOIntHandle *)h)->pClientCtxt, gpio, isr);
}
static DALResult 
GPIOInt_GPIOInt_IsInterruptEnabled( DalDeviceHandle * h, uint32  gpio, void *  state, uint32  size) 
{
	return GPIOIntr_IsInterruptEnabled(((GPIOIntHandle *)h)->pClientCtxt, gpio, (uint32 *)state);
}
static DALResult 
GPIOInt_GPIOInt_IsInterruptPending( DalDeviceHandle * h, uint32  gpio, void *  state, uint32  size) 
{
	return GPIOIntr_IsInterruptPending(((GPIOIntHandle *)h)->pClientCtxt, gpio, (uint32 *)state);
}
static DALResult 
GPIOInt_GPIOInt_IsInterruptSet( DalDeviceHandle * h, uint32  gpio, void *  state, uint32  size) 
{
	return GPIOIntr_IsInterruptSet(((GPIOIntHandle *)h)->pClientCtxt, gpio, (uint32 *)state);
}
static DALResult 
GPIOInt_GPIOInt_Save( DalDeviceHandle * h) 
{
	return GPIOIntr_Save(((GPIOIntHandle *)h)->pClientCtxt);
}
static DALResult 
GPIOInt_GPIOInt_Restore( DalDeviceHandle * h) 
{
	return GPIOIntr_Restore(((GPIOIntHandle *)h)->pClientCtxt);
}

static DALResult 
GPIOInt_GPIOInt_DisableInterrupt( DalDeviceHandle * h, uint32 gpio) 
{
    return GPIOIntr_DisableInterrupt(((GPIOIntHandle *)h)->pClientCtxt, gpio);
}

static DALResult 
GPIOInt_GPIOInt_EnableInterrupt( DalDeviceHandle * h, uint32 gpio) 
{
	return GPIOIntr_EnableInterrupt(((GPIOIntHandle *)h)->pClientCtxt, gpio);
}

static DALResult 
GPIOInt_GPIOInt_InterruptNotify( DalDeviceHandle * h, uint32 gpio) 
{
  return DAL_ERROR;
}

static DALResult 
GPIOInt_GPIOInt_MonitorInterrupts( DalDeviceHandle * h, GPIOINTISR isr, uint32 enable) 
{
  /*
   * This API is not supported on 8660/8960.
   */
	return DAL_ERROR;
}

static DALResult 
GPIOInt_GPIOInt_MapMPMInterrupt( DalDeviceHandle * h, uint32 gpio, uint32 mpm_interrupt_id) 
{
  return GPIOIntr_MapMPMInterrupt(((GPIOIntHandle *)h)->pClientCtxt, gpio, mpm_interrupt_id);
}

static DALResult 
GPIOInt_GPIOInt_AttachRemote( DalDeviceHandle * h, uint32 processor) 
{
  /*
   * This API is not supported on 8660/8960.
   */
  return DAL_ERROR;
}

static DALResult 
GPIOInt_GPIOInt_TriggerInterrupt( DalDeviceHandle * h, uint32 gpio) 
{
  return GPIOIntr_TriggerInterrupt(((GPIOIntHandle *)h)->pClientCtxt,gpio);
}

static DALResult 
GPIOInt_GPIOInt_ClearInterrupt( DalDeviceHandle * h, uint32 gpio) 
{
  return GPIOIntr_ClearInterrupt(((GPIOIntHandle *)h)->pClientCtxt,gpio);
}

static DALResult 
GPIOInt_GPIOInt_SetDirectConnectGPIOMapping( DalDeviceHandle * h, uint32 gpio, uint32 direct_connect_line) 
{
  return GPIOIntr_SetDirectConnectGPIOMapping(((GPIOIntHandle *)h)->pClientCtxt,gpio,direct_connect_line);
}


static void 
GPIOInt_InitInterface(GPIOIntClientCtxt* pclientCtxt)
{
    static const GPIOInt vtbl = {
       {
          GPIOInt_GPIOInt_Attach,
          GPIOInt_GPIOInt_Detach,
          GPIOInt_GPIOInt_Init,
          GPIOInt_GPIOInt_DeInit,
          GPIOInt_GPIOInt_Open,
          GPIOInt_GPIOInt_Close,
          GPIOInt_GPIOInt_Info,
          GPIOInt_GPIOInt_PowerEvent,
          GPIOInt_GPIOInt_SysRequest
        } ,
	GPIOInt_GPIOInt_SetTrigger,
	GPIOInt_GPIOInt_RegisterIsr,
        GPIOInt_GPIOInt_RegisterEvent,
        GPIOInt_GPIOInt_DeRegisterEvent,
	GPIOInt_GPIOInt_DeregisterIsr,
	GPIOInt_GPIOInt_IsInterruptEnabled,
	GPIOInt_GPIOInt_IsInterruptPending,
	GPIOInt_GPIOInt_Save,
	GPIOInt_GPIOInt_Restore,
        GPIOInt_GPIOInt_DisableInterrupt,
        GPIOInt_GPIOInt_EnableInterrupt,
        GPIOInt_GPIOInt_InterruptNotify,
        GPIOInt_GPIOInt_MonitorInterrupts,
        GPIOInt_GPIOInt_MapMPMInterrupt,
        GPIOInt_GPIOInt_AttachRemote,
        GPIOInt_GPIOInt_TriggerInterrupt,
        GPIOInt_GPIOInt_ClearInterrupt,
        GPIOInt_GPIOInt_IsInterruptSet,
        GPIOInt_GPIOInt_SetDirectConnectGPIOMapping,

    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->GPIOIntHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->GPIOIntHandle.pVtbl  = &vtbl;
    pclientCtxt->GPIOIntHandle.pClientCtxt = pclientCtxt;

}

DALResult 
GPIOInt_GPIOInt_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static GPIOIntDrvCtxt drvCtxt = {{GPIOIntr_DriverInit,
									   GPIOIntr_DriverDeInit
									  },2,
									  sizeof(GPIOIntDevCtxt)};

    GPIOIntClientCtxt *pclientCtxt =  NULL;

    nErr = DALSYS_Malloc(sizeof(GPIOIntClientCtxt),(void **)&pclientCtxt);
    if(nErr != DAL_SUCCESS)
    {
      return nErr;
    }
    DALSYS_memset(pclientCtxt,0,sizeof(GPIOIntClientCtxt));

	*phDalDevice = NULL;

     if (DAL_SUCCESS == nErr) {
        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            GPIOInt_InitInterface(pclientCtxt);
			GPIOInt_GPIOInt_AddRef(&(pclientCtxt->GPIOIntHandle));
			*phDalDevice = (DalDeviceHandle *)&(pclientCtxt->GPIOIntHandle);
        }
     } 
    return nErr;
}

