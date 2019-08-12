/*==============================================================================

FILE:      DalTLMMFwk.c

DESCRIPTION: This file implements a TLMM DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALTLMM.h"
#include "DALFramework.h"
#include "DALSys.h"
#include "DDITlmm.h"
#include "DALTLMM.h"
#include "DALSys.h"
#include "string.h" //for memset warning


DALResult 
TLMM_DalTlmm_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
TLMM_DalTlmm_AddRef(DalTlmmHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

TLMMDrvCtxt TLMM_DriverCtxt = 
{
    {
        TLMM_DriverInit,
 		TLMM_DriverDeInit
 	},1,
 	sizeof(TLMMDevCtxt)
};

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
TLMM_DalTlmm_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	return dwref;
}

static DALResult 
TLMM_DalTlmm_Init(DalDeviceHandle *h)
{  
   TLMMClientCtxt *pClientCtxt = (TLMMClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pDevCtxt->DevId,
	                           pClientCtxt->pDevCtxt->hProp);
   return TLMM_DeviceInit(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_DeInit(DalDeviceHandle *h)
{
   return TLMM_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return TLMM_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
TLMM_DalTlmm_Open(DalDeviceHandle* h, uint32 mode)
{
    return TLMM_Open(h->pClientCtxt,mode);
}

static DALResult 
TLMM_DalTlmm_Close(DalDeviceHandle* h)
{
    return TLMM_Close(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTLMM_INTERFACE_VERSION;
   return TLMM_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
TLMM_DalTlmm_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTlmm Interface. 
------------------------------------------------------------------------------*/

static DALResult 
TLMM_DalTlmm_ConfigGpio( DalDeviceHandle * h, DALGpioSignalType gpio_config, DALGpioEnableType enable) 
{
	return TLMM_ConfigGpio(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, enable);
}
static DALResult 
TLMM_DalTlmm_ConfigGpioGroup( DalDeviceHandle * h, DALGpioEnableType enable, DALGpioSignalType *gpio_configs, uint32 size) 
{
	return TLMM_ConfigGpioGroup(((DalTlmmHandle *)h)->pClientCtxt, enable, gpio_configs, size);
}
static DALResult 
TLMM_DalTlmm_TristateGpio( DalDeviceHandle * h, DALGpioSignalType gpio_config) 
{
	return TLMM_TristateGpio(((DalTlmmHandle *)h)->pClientCtxt, gpio_config);
}
static DALResult 
TLMM_DalTlmm_TristateGpioGroup( DalDeviceHandle * h, DALGpioSignalType* gpio_configs, uint32 size) 
{
	return TLMM_TristateGpioGroup(((DalTlmmHandle *)h)->pClientCtxt, gpio_configs, size);
}
static DALResult 
TLMM_DalTlmm_ConfigGpiosForSleep( DalDeviceHandle * h, DALGpioSleepConfigType gpio_sleep_config) 
{
	return TLMM_ConfigGpiosForSleep(((DalTlmmHandle *)h)->pClientCtxt, gpio_sleep_config);
}
static DALResult 
TLMM_DalTlmm_RestoreGpiosFromSleep( DalDeviceHandle * h, DALGpioSleepConfigType gpio_sleep_config) 
{
	return TLMM_RestoreGpiosFromSleep(((DalTlmmHandle *)h)->pClientCtxt, gpio_sleep_config);
}
static DALResult 
TLMM_DalTlmm_GetGpioNumber( DalDeviceHandle * h, DALGpioSignalType gpio_config, uint32 *  number) 
{
	return TLMM_GetGpioNumber(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, number);
}
static DALResult 
TLMM_DalTlmm_GpioIn( DalDeviceHandle * h, DALGpioSignalType gpio_config, DALGpioValueType* value) 
{
	return TLMM_GpioIn(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, value);
}
static DALResult 
TLMM_DalTlmm_GpioOut( DalDeviceHandle * h, DALGpioSignalType gpio_config, DALGpioValueType value) 
{
	return TLMM_GpioOut(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, value);
}
static DALResult 
TLMM_DalTlmm_GpioOutGroup( DalDeviceHandle * h, DALGpioSignalType* gpio_configs, uint32 size, DALGpioValueType value) 
{
	return TLMM_GpioOutGroup(((DalTlmmHandle *)h)->pClientCtxt, gpio_configs, size, value);
}
static DALResult 
TLMM_DalTlmm_SwitchGpioIntOwnership( DalDeviceHandle * h, uint32  owner) 
{
	return TLMM_SwitchGpioIntOwnership(((DalTlmmHandle *)h)->pClientCtxt, owner);
}
static DALResult 
TLMM_DalTlmm_SetPort( DalDeviceHandle * h, DALGpioPortType port, uint32  value) 
{
	return TLMM_SetPort(((DalTlmmHandle *)h)->pClientCtxt, port, value);
}
static DALResult 
TLMM_DalTlmm_ExternalGpioConfig( DalDeviceHandle * h, DALGpioSignalType gpio_config, DALGpioEnableType enable) 
{
	return TLMM_ConfigGpio(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, enable);
}
static DALResult 
TLMM_DalTlmm_RequestGpio( DalDeviceHandle * h, DALGpioSignalType gpio_config) 
{
	return TLMM_RequestGpio(((DalTlmmHandle *)h)->pClientCtxt, gpio_config);
}
static DALResult 
TLMM_DalTlmm_ReleaseGpio( DalDeviceHandle * h, DALGpioSignalType gpio_config) 
{
	return TLMM_ReleaseGpio(((DalTlmmHandle *)h)->pClientCtxt, gpio_config);
}
static DALResult 
TLMM_DalTlmm_GetGpioOwner( DalDeviceHandle * h, uint32 gpio_number, DALGpioOwnerType*  owner) 
{
	return TLMM_GetGpioOwner(((DalTlmmHandle *)h)->pClientCtxt, gpio_number, owner);
}
static DALResult 
TLMM_DalTlmm_GetCurrentConfig( DalDeviceHandle * h, uint32 gpio_number, DALGpioSignalType* gpio_config ) 
{
	return TLMM_GetCurrentConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, gpio_config );
}
static DALResult 
TLMM_DalTlmm_GetPinStatus( DalDeviceHandle * h, uint32  gpio_number, DALGpioStatusType* status ) 
{
	return TLMM_GetPinStatus( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, status );
}
static DALResult 
TLMM_DalTlmm_SetInactiveConfig( DalDeviceHandle * h, uint32 gpio_number, DALGpioSignalType gpio_config ) 
{
	return TLMM_SetInactiveConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, gpio_config );
}
static DALResult 
TLMM_DalTlmm_GetInactiveConfig( DalDeviceHandle * h, uint32 gpio_number, DALGpioSignalType* gpio_config ) 
{
	return TLMM_GetInactiveConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, gpio_config );
}
static DALResult 
TLMM_DalTlmm_LockGpio( DalDeviceHandle * h, DALGpioClientType client, uint32 gpio_number ) 
{
	return TLMM_LockGpio( ((DalTlmmHandle *)h)->pClientCtxt, client, gpio_number );
}
static DALResult 
TLMM_DalTlmm_UnlockGpio( DalDeviceHandle * h, DALGpioClientType client, uint32 gpio_number ) 
{
	return TLMM_UnlockGpio( ((DalTlmmHandle *)h)->pClientCtxt, client, gpio_number );
}
static DALResult 
TLMM_DalTlmm_IsGpioLocked( DalDeviceHandle * h, uint32 gpio_number, DALGpioLockType* lock ) 
{
	return TLMM_IsGpioLocked( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, lock );
}
static DALResult 
TLMM_DalTlmm_BypassSleep( DalDeviceHandle * h, uint32 gpio_number, DALGpioEnableType enable ) 
{
	return TLMM_BypassSleep( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, enable );
}
static DALResult 
TLMM_DalTlmm_AttachRemote( DalDeviceHandle * h, uint32  processor) 
{
	return TLMM_AttachRemote(((DalTlmmHandle *)h)->pClientCtxt, processor);
}
static DALResult 
TLMM_DalTlmm_GetOutput( DalDeviceHandle * h, uint32 gpio_number, DALGpioValueType* value ) 
{
	return TLMM_GetOutput( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, value );
}
static DALResult 
TLMM_DalTlmm_GetGpioId( DalDeviceHandle * h, const char* pszGpioStr, DALGpioIdType* pnGpioId ) 
{
	return TLMM_GetGpioId( ((DalTlmmHandle *)h)->pClientCtxt, pszGpioStr, pnGpioId );
}
static DALResult 
TLMM_DalTlmm_ConfigGpioId( DalDeviceHandle * h, DALGpioIdType nGpioId, DalTlmm_GpioConfigIdType* pUserSettings ) 
{
	return TLMM_ConfigGpioId( ((DalTlmmHandle *)h)->pClientCtxt, nGpioId, pUserSettings );
}
static DALResult 
TLMM_DalTlmm_ConfigGpioIdInactive( DalDeviceHandle * h, DALGpioIdType nGpioId ) 
{
	return TLMM_ConfigGpioIdInactive( ((DalTlmmHandle *)h)->pClientCtxt, nGpioId );
}
static DALResult 
TLMM_DalTlmm_GpioIdOut( DalDeviceHandle * h, DALGpioIdType nGpioId, DALGpioValueType eValue) 
{
	return TLMM_GpioIdOut(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, eValue);
}
static DALResult 
TLMM_DalTlmm_GpioIdIn( DalDeviceHandle * h, DALGpioIdType nGpioId, DALGpioValueType *eValue) 
{
	return TLMM_GpioIdIn(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, eValue);
}
static DALResult 
TLMM_DalTlmm_ReleaseGpioId( DalDeviceHandle * h, DALGpioIdType nGpioId) 
{
	return TLMM_ReleaseGpioId(((DalTlmmHandle *)h)->pClientCtxt, nGpioId);
}
static DALResult 
TLMM_DalTlmm_SelectGpioIdMode( DalDeviceHandle * h, DALGpioIdType nGpioId,  DalGpioModeType eMode, DalTlmm_GpioConfigIdType* pUserSettings) 
{
	return TLMM_SelectGpioIdMode(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, eMode, pUserSettings);
}
static DALResult 
TLMM_DalTlmm_GetGpioIdSettings( DalDeviceHandle * h, DALGpioIdType nGpioId, DalTlmm_GpioIdSettingsType* pGpioSettings) 
{
	return TLMM_GetGpioIdSettings(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, pGpioSettings);
}
static DALResult 
TLMM_DalTlmm_ConfigGpioIdModeIndexed( DalDeviceHandle * h, DALGpioIdType nGpioId, uint32 nMode) 
{
	return TLMM_ConfigGpioIdModeIndexed(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, nMode);
}

static void 
TLMM_InitInterface(TLMMClientCtxt* pclientCtxt)
{
    static const DalTlmm vtbl = {
       {
          TLMM_DalTlmm_Attach,
          TLMM_DalTlmm_Detach,
          TLMM_DalTlmm_Init,
          TLMM_DalTlmm_DeInit,
          TLMM_DalTlmm_Open,
          TLMM_DalTlmm_Close,
          TLMM_DalTlmm_Info,
          TLMM_DalTlmm_PowerEvent,
          TLMM_DalTlmm_SysRequest
        } ,
        TLMM_DalTlmm_ConfigGpio,
        TLMM_DalTlmm_ConfigGpioGroup,
        TLMM_DalTlmm_TristateGpio,
        TLMM_DalTlmm_TristateGpioGroup,
        TLMM_DalTlmm_ConfigGpiosForSleep,
        TLMM_DalTlmm_RestoreGpiosFromSleep,
        TLMM_DalTlmm_GetGpioNumber,
        TLMM_DalTlmm_GpioIn,
        TLMM_DalTlmm_GpioOut,
        TLMM_DalTlmm_GpioOutGroup,
        TLMM_DalTlmm_SwitchGpioIntOwnership,
        TLMM_DalTlmm_SetPort,
        TLMM_DalTlmm_ExternalGpioConfig,
        TLMM_DalTlmm_RequestGpio,
        TLMM_DalTlmm_ReleaseGpio,
        TLMM_DalTlmm_GetGpioOwner,
        TLMM_DalTlmm_GetCurrentConfig,
        TLMM_DalTlmm_GetPinStatus,
        TLMM_DalTlmm_SetInactiveConfig,
        TLMM_DalTlmm_GetInactiveConfig,
        TLMM_DalTlmm_LockGpio,
        TLMM_DalTlmm_UnlockGpio,
        TLMM_DalTlmm_IsGpioLocked,
        TLMM_DalTlmm_BypassSleep,
        TLMM_DalTlmm_AttachRemote,
        TLMM_DalTlmm_GetOutput,
        TLMM_DalTlmm_GetGpioId,
        TLMM_DalTlmm_ConfigGpioId,
        TLMM_DalTlmm_ConfigGpioIdInactive,
        TLMM_DalTlmm_GpioIdOut,
        TLMM_DalTlmm_GpioIdIn,
        TLMM_DalTlmm_ReleaseGpioId,
        TLMM_DalTlmm_SelectGpioIdMode,
        TLMM_DalTlmm_GetGpioIdSettings,
        TLMM_DalTlmm_ConfigGpioIdModeIndexed
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalTlmmHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalTlmmHandle.pVtbl  = &vtbl;
    pclientCtxt->DalTlmmHandle.pClientCtxt = pclientCtxt;

}

DALResult 
TLMM_DalTlmm_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
    static TLMMClientCtxt ClientCtxt;
    TLMMClientCtxt *pClientCtxt =  &ClientCtxt;

    *phDalDevice = NULL;

    nErr = DALFW_AttachToDevice(DALDEVICEID_TLMM,(DALDrvCtxt *)&TLMM_DriverCtxt,
		                        (DALClientCtxt *)pClientCtxt);
    if (DAL_SUCCESS == nErr)
    {
        TLMM_InitInterface(pClientCtxt);
		TLMM_DalTlmm_AddRef(&(pClientCtxt->DalTlmmHandle));
		*phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalTlmmHandle);
    }

    return nErr;
}

