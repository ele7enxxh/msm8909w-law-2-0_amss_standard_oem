/**
 * @file:  DDISpmi.c
 * 
 * Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/platform/DDISpmi.c#1 $
 * $Change: 11985146 $
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "DDISpmi.h"
#include "DALFramework.h"
#include "DalDevice.h"
#include "SpmiBus.h"
#include "SpmiUtils.h"
#include "SpmiLogs.h"

#define MAX_INTRS_PER_CLIENT 8
#define SPMI_DAL_DEBUG_MAX_CLIENT_HANDLES 16
#define INVALID_PERIPH_ID -1

/*------------------------------------------------------------------------------
Declaring a "Spmi" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct SpmiDrvCtxt SpmiDrvCtxt;
typedef struct SpmiDevCtxt SpmiDevCtxt;
typedef struct SpmiClientCtxt SpmiClientCtxt;

/*------------------------------------------------------------------------------
Global variable
------------------------------------------------------------------------------*/
SpmiDevCtxt *pSpmiDalDeviceCtxt = 0; /* This pointer is just used for facilitating debug */

/*------------------------------------------------------------------------------
Declaring a private "Spmi" Vtable
------------------------------------------------------------------------------*/
typedef struct
{
    int32 periphID;
    uint8 mask;
    SpmiToken token;
}Spmi_RegisteredIntrType;

typedef struct SpmiDALVtbl SpmiDALVtbl;
struct SpmiDALVtbl
{
    int (*Spmi_DriverInit)(SpmiDrvCtxt *);
    int (*Spmi_DriverDeInit)(SpmiDrvCtxt *);
};

struct SpmiDevCtxt
{
    //Base Members
    uint32 dwRefs;
    uint32 dwDevCtxtRefIdx;
    SpmiDrvCtxt *pSpmiDrvCtxt;
    DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    const char *strDeviceName;
    uint32 Reserved[16];
    //Spmi Dev state can be added by developers here
    DALSYSSyncHandle hSpmiBusDeviceSynchronization;
    DALSYS_SYNC_OBJECT(SpmiBusDeviceSyncObject);
    DALSYSSyncHandle hSpmiIntrCtlrDeviceSynchronization;
    DALSYS_SYNC_OBJECT(SpmiIntrCtlrDeviceSyncObject);
    uint32 uDeviceInitDone;

    /* Debug registers */
    DalSpmiHandle *pSpmiHandles[SPMI_DAL_DEBUG_MAX_CLIENT_HANDLES];
    uint32 uHandlesCurrentlyActive;

};

struct SpmiDrvCtxt
{
    //Base Members
    SpmiDALVtbl SpmiDALVtbl;
    uint32 dwNumDev;
    uint32 dwSizeDevCtxt;
    uint32 bInit;
    uint32 dwRefs;
    SpmiDevCtxt SpmiDevCtxt[1];
    //Spmi Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "Spmi" Client Context
------------------------------------------------------------------------------*/
struct SpmiClientCtxt
{
    //Base Members
    uint32 dwRefs;
    uint32 dwAccessMode;
    void *pPortCtxt;
    SpmiDevCtxt *pSpmiDevCtxt;
    DalSpmiHandle DalSpmiHandle;
    //Spmi Client state can be added by developers here
    uint32 uOpened;
    Spmi_RegisteredIntrType registeredIntrs[MAX_INTRS_PER_CLIENT];
};

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
Spmi_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);



/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/

const DALREG_DriverInfo
DALSpmi_DriverInfo = { Spmi_DeviceAttach, 0, NULL };


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/


static uint32 Spmi_AddRef(DalSpmiHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->clientCtx));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult Spmi_DriverInit(SpmiDrvCtxt *pdrvCtxt)
{
    return DAL_SUCCESS;
}

DALResult Spmi_DriverDeInit(SpmiDrvCtxt *pdrvCtxt)
{
    return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/
static uint32 Spmi_DeviceDetach(DalDeviceHandle* h)
{

    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    uint32 dwref;
    uint32 i;

    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceDetach func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_DeviceDetach: DAL handle passed is NULL");
        return (uint32)DAL_ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;

    if(pSpmiDevCtxt->uDeviceInitDone) /* Do this only if the device is not de-inited already */
    {
        SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceDetach func Entry, handle: 0x%x", h);
        DALSYS_SyncEnter(pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization);
        
        for(int i = 0; i < MAX_INTRS_PER_CLIENT; i++) 
        {
            if(clientCtx->registeredIntrs[i].periphID != INVALID_PERIPH_ID)
            {
                SpmiBus_UnregisterIsr( &clientCtx->registeredIntrs[i].token, 0xFF );
            }
        }
        
        DALSYS_SyncLeave(pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization);
    }

    dwref = DALFW_Release((DALClientCtxt *)clientCtx);
    if(0 == dwref)
    {
        for(i=0; i<SPMI_DAL_DEBUG_MAX_CLIENT_HANDLES; i++)
        {
            if(pSpmiDevCtxt->pSpmiHandles[i] == (DalSpmiHandle *) h)
            {
                pSpmiDevCtxt->pSpmiHandles[i] = 0;
                break;
            }
        }
        pSpmiDevCtxt->uHandlesCurrentlyActive--;
        /* Release the client ctxt */
        DALSYS_Free(clientCtx);
        SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceDetach: Freeing the client ctxt with handle 0x%x", h);
    }

    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceDetach func Exit, dwref: %d", dwref);

    return dwref;
}

static DALResult Spmi_DeviceInit(DalDeviceHandle *h)
{

    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    DALResult result = DAL_SUCCESS;

    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceInit func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_DeviceInit: DAL handle passed is NULL");
        goto ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;


    /* Initialize Resource synchronization object. This sync object is used
    to synchronize between client calls on same device */
    if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
        &(pSpmiDevCtxt->hSpmiBusDeviceSynchronization),
        &(pSpmiDevCtxt->SpmiBusDeviceSyncObject) ))
    {
        pSpmiDevCtxt->hSpmiBusDeviceSynchronization = 0;
        result = DAL_ERROR;
        SPMI_LOG_FATAL(            "DalSpmi: Spmi_DeviceInit: ***SPMI_FATAL_ERROR*** Unable to create resource synchronization object");
        goto ERROR;
    }

    /* Initialize Resource synchronization object. This sync object is used
    to synchronize between client calls on same device */
    if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
        &(pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization),
        &(pSpmiDevCtxt->SpmiIntrCtlrDeviceSyncObject) ))
    {
        pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization = 0;
        DALSYS_DestroyObject( pSpmiDevCtxt->hSpmiBusDeviceSynchronization );
        pSpmiDevCtxt->hSpmiBusDeviceSynchronization = NULL;
        result = DAL_ERROR;
        SPMI_LOG_FATAL(            "DalSpmi: Spmi_DeviceInit: ***SPMI_FATAL_ERROR*** Unable to create resource synchronization object");
        goto ERROR;
    }


    /* Initialise Spmi Bus driver */
    if(DAL_SUCCESS != (result = (DALResult)SpmiBus_Init()))
    {
        SPMI_LOG_FATAL(            "DalSpmi: Spmi_DeviceInit: ***SPMI_FATAL_ERROR*** SpmiBus_Init failed with status %d", result);
        goto ERROR;
    }

    pSpmiDevCtxt->uDeviceInitDone = TRUE;

    SPMI_LOG_INFO("DalSpmi: Spmi_DeviceInit: Device initialized successfully");

ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceInit func Exit, Status: %d", result);
    return result;
}

static DALResult Spmi_DeviceDeInit(DalDeviceHandle *h)
{

    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    DALResult result = DAL_SUCCESS;

    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceDeInit func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_DeviceDeInit: DAL handle passed is NULL");
        goto ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;


    /* Destroy the Synchronization objects for this device */
    DALSYS_DestroyObject( pSpmiDevCtxt->hSpmiBusDeviceSynchronization );
    pSpmiDevCtxt->hSpmiBusDeviceSynchronization = NULL;

    DALSYS_DestroyObject( pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization );
    pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization = NULL;

    /* De-Initialise Spmi Bus driver */
    SpmiBus_DeInit();

    pSpmiDevCtxt->uDeviceInitDone = FALSE;


ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceDeInit func Exit, Status: %d", result);
    return result;
}

static DALResult Spmi_DevicePowerEvent(
                                       DalDeviceHandle *h,
                                       DalPowerCmd PowerCmd,
                                       DalPowerDomain PowerDomain )
{
    return DAL_SUCCESS;

}

static DALResult Spmi_DeviceOpen(DalDeviceHandle* h, uint32 mode)
{

    SpmiClientCtxt *clientCtx;
    DALResult result = DAL_SUCCESS;

    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceOpen func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_DeviceOpen: DAL handle passed is NULL");
        goto ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);

    clientCtx->uOpened = TRUE;

ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceOpen func Exit, Status: %d", result);
    return result;
}

static DALResult Spmi_DeviceClose(DalDeviceHandle* h)
{
    SpmiClientCtxt *clientCtx;
    DALResult result = DAL_SUCCESS;

    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceClose func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_DeviceClose: DAL handle passed is NULL");
        goto ERROR;
    }
    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);

    clientCtx->uOpened = FALSE;

ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceClose func Exit, Status: %d", result);
    return result;
}

static DALResult Spmi_DeviceInfo(
                                 DalDeviceHandle* h,
                                 DalDeviceInfo* info,
                                 uint32 infoSize)
{
    info->Version = DALSPMI_INTERFACE_VERSION;
    return DAL_SUCCESS;
}

static DALResult Spmi_DeviceSysRequest(
                                       DalDeviceHandle* h,
                                       DalSysReq ReqIdx,
                                       const void* SrcBuf,
                                       uint32 SrcBufLen,
                                       void* DestBuf,
                                       uint32 DestBufLen,
                                       uint32* DestBufLenReq)
{
    return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are extended in DalSpmi Interface.
------------------------------------------------------------------------------*/
static DALResult Spmi_ReadLong(
                               DalDeviceHandle *h,
                               uint32 uSlaveId,
                               Spmi_AccessPriority eAccessPriority,
                               uint32 uRegisterAddress,
                               unsigned char* pucData,
                               uint32 uDataLen,
                               uint32* puTotalBytesRead)
{
    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    DALResult result;

    SPMI_LOG_TRACE("DalSpmi: Spmi_ReadLong func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_ReadLong: DAL handle passed is NULL");
        goto ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;

    if(!clientCtx->uOpened)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_ReadLong: Client with device handle 0x%x has not called device open call",
            h);
        goto ERROR;
    }

    /* Enter the Critical Section now */
    DALSYS_SyncEnter(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);

    SPMI_LOG_TRACE(        "DalSpmi: Spmi_ReadLong: Entered critical section for client with device handle 0x%x",
        h);

    /* Call underlying SPMI driver api */
    result = (DALResult) SpmiBus_ReadLong(uSlaveId,
        eAccessPriority,
        uRegisterAddress,
        pucData,
        uDataLen,
        puTotalBytesRead);

    DALSYS_SyncLeave(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);

    SPMI_LOG_TRACE(        "DalSpmi: Spmi_ReadLong: Exited critical section for client with device handle 0x%x",
        h);

ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_ReadLong func Exit, Status: %d", result);
    return result;

}


static DALResult Spmi_WriteLong(
                                DalDeviceHandle *h,
                                uint32 uSlaveId,
                                Spmi_AccessPriority eAccessPriority,
                                uint32 uRegisterAddress,
                                unsigned char* pucData,
                                uint32 uDataLen)
{
    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    DALResult result;

    SPMI_LOG_TRACE("DalSpmi: Spmi_WriteLong func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_WriteLong: DAL handle passed is NULL");
        goto ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;

    if(!clientCtx->uOpened)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_WriteLong: Client with device handle 0x%x has not called device open call",
            h);
        goto ERROR;
    }

    /* Enter the Critical Section now */
    DALSYS_SyncEnter(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);

    SPMI_LOG_TRACE(        "DalSpmi: Spmi_WriteLong: Entered critical section for client with device handle 0x%x",
        h);

    /* Call underlying SPMI driver api */
    result = (DALResult) SpmiBus_WriteLong(uSlaveId,
        eAccessPriority,
        uRegisterAddress,
        pucData,
        uDataLen);

    DALSYS_SyncLeave(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);
    SPMI_LOG_TRACE(        "DalSpmi: Spmi_WriteLong: Exited critical section for client with device handle 0x%x",
        h);

ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_WriteLong func Exit, Status: %d", result);
    return result;
}

static DALResult Spmi_ReadModifyWriteLongByte(
    DalDeviceHandle *h,
    uint32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    uint32 uRegisterAddress,
    uint32 uData,
    uint32 uMask,
    uint8 *pucDataWritten)
{
    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    DALResult result;

    SPMI_LOG_TRACE("DalSpmi: Spmi_ReadModifyWriteLongByte func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_ReadModifyWriteLongByte: DAL handle passed is NULL");
        goto ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;

    if(!clientCtx->uOpened)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_ReadModifyWriteLongByte: Client with device handle 0x%x has not called device open call",
            h);
        goto ERROR;
    }

    /* Enter the Critical Section now */
    DALSYS_SyncEnter(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);

    SPMI_LOG_TRACE(        "DalSpmi: Spmi_ReadModifyWriteLongByte: Entered critical section for client with device handle 0x%x",
        h);

    /* Call underlying SPMI driver api */
    result = (DALResult) SpmiBus_ReadModifyWriteLongByte(
        uSlaveId,
        eAccessPriority,
        uRegisterAddress,
        uData,
        uMask,
        pucDataWritten);

    DALSYS_SyncLeave(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);
    SPMI_LOG_TRACE(        "DalSpmi: Spmi_ReadModifyWriteLongByte: Exited critical section for client with device handle 0x%x",
        h);

ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_ReadModifyWriteLongByte func Exit, Status: %d", result);
    return result;
}


static DALResult Spmi_ExecCommand(
                              DalDeviceHandle *h,
                              uint32 uSlaveId,
                              Spmi_AccessPriority eAccessPriority,
                              SpmiBus_CommandType eSpmiCommand)
{
    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    DALResult result;

    SPMI_LOG_TRACE("DalSpmi: Spmi_ExecCommand func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_ExecCommand: DAL handle passed is NULL");
        goto ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;

    if(!clientCtx->uOpened)
    {
        result = DAL_ERROR;
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_ExecCommand: Client with device handle 0x%x has not called device open call",
            h);
        goto ERROR;
    }

    /* Enter the Critical Section now */
    DALSYS_SyncEnter(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);

    SPMI_LOG_TRACE(        "DalSpmi: Spmi_ExecCommand: Entered critical section for client with device handle 0x%x",
        h);

    /* Call underlying SPMI driver api */
    result = (DALResult) SpmiBus_Command(uSlaveId,
        eAccessPriority, eSpmiCommand);

    DALSYS_SyncLeave(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);
    SPMI_LOG_TRACE(        "DalSpmi: Spmi_ExecCommand: Exited critical section for client with device handle 0x%x",
        h);

ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_ExecCommand func Exit, Status: %d", result);
    return result;

}

static DALResult Spmi_RegisterIsr(
                                  DalDeviceHandle *h,
                                  uint16 periphID,
                                  uint32 uExtendedIntrMask,
                                  const SpmiIsr isr,
                                  const void* ctxt)
{
    Spmi_RegisteredIntrType* regIntr = NULL;
    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    DALResult result;

    SPMI_LOG_TRACE("DalSpmi: Spmi_RegisterIsr func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        SPMI_LOG_ERROR( "DalSpmi: Spmi_RegisterIsr: Client with device handle 0x%x has not called device open call", h );
        return DAL_ERROR;
    }
    
    if(uExtendedIntrMask > 0xFF) {
        SPMI_LOG_ERROR( "DalSpmi: Spmi_RegisterIsr: Invalid mask %x", uExtendedIntrMask );
        return DAL_ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;

    if(!clientCtx->uOpened)
    {
        SPMI_LOG_ERROR( "DalSpmi: Spmi_RegisterIsr: Client with device handle 0x%x has not called device open call", h );
        return DAL_ERROR;
    }

    /* Enter the Critical Section now */
    DALSYS_SyncEnter(pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization);
    
    for(int i = 0; i < MAX_INTRS_PER_CLIENT; i++) 
    {
        if(clientCtx->registeredIntrs[i].periphID == (int32)periphID && 
           (clientCtx->registeredIntrs[i].mask & uExtendedIntrMask) != 0)
        {
            SPMI_LOG_WARNING( "Spmi_RegisterIsr: Interrupt already registered for periph %x, mask %x",
                              periphID, clientCtx->registeredIntrs[i].mask & uExtendedIntrMask );
            
            /* Leave the critical section now */
            DALSYS_SyncLeave(pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization);
            return DAL_ERROR;
        }
    }
        
    for(int i = 0; i < MAX_INTRS_PER_CLIENT; i++) 
    {
        if(clientCtx->registeredIntrs[i].periphID == INVALID_PERIPH_ID) 
        {
            regIntr = &clientCtx->registeredIntrs[i];
            break;
        }
    }
    
    if(regIntr == NULL)
    {
        SPMI_LOG_WARNING( "Spmi_RegisterIsr: No more interrupt slots available" );
        result = DAL_ERROR;
    }
    else if((result = (DALResult) SpmiBus_RegisterIsr( PPID_TO_SLAVE_ID( periphID ), PPID_TO_PERIPH_ID( periphID ),
                                                       uExtendedIntrMask, isr, ctxt, &regIntr->token )) == SPMI_SUCCESS) 
    {
        regIntr->periphID = (int32) periphID;
        regIntr->mask = uExtendedIntrMask;
    }
    
    /* Leave the critical section now */
    DALSYS_SyncLeave(pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization);

    SPMI_LOG_TRACE("DalSpmi: Spmi_RegisterIsr func Exit, Status: %d", result);
    return result;
}


static DALResult Spmi_UnregisterIsr(
                                    DalDeviceHandle *h,
                                    uint16 periphID,
                                    uint32 uExtendedIntrMask)
{
    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    DALResult result = DAL_ERROR;

    SPMI_LOG_TRACE("DalSpmi: Spmi_UnregisterIsr func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        SPMI_LOG_ERROR( "DalSpmi: Spmi_UnregisterIsr: DAL handle passed is NULL" );
        return DAL_ERROR;
    }
    
    if(uExtendedIntrMask > 0xFF) {
        SPMI_LOG_ERROR( "DalSpmi: Spmi_RegisterIsr: Invalid mask %x", uExtendedIntrMask );
        return DAL_ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;

    if(!clientCtx->uOpened)
    {
        SPMI_LOG_ERROR( "DalSpmi: Spmi_UnregisterIsr: Client with device handle 0x%x has not called device open call", h );
        return DAL_ERROR;
    }

    /* Enter the Critical Section now */
    DALSYS_SyncEnter(pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization);

    for(int i = 0; i < MAX_INTRS_PER_CLIENT; i++) 
    {
        uint8 intersectMask = clientCtx->registeredIntrs[i].mask & uExtendedIntrMask;
        
        if(clientCtx->registeredIntrs[i].periphID == (int32)periphID && intersectMask != 0)
        {
            SpmiToken* token = &clientCtx->registeredIntrs[i].token;
            
            if((result = (DALResult)SpmiBus_UnregisterIsr( token, intersectMask )) != SPMI_SUCCESS) 
            {
                break;
            }
            
            clientCtx->registeredIntrs[i].mask &= ~intersectMask;

            if(clientCtx->registeredIntrs[i].mask == 0) {
                clientCtx->registeredIntrs[i].periphID = INVALID_PERIPH_ID;
            }
        }
    }
    
    /* Leave the critical section now */
    DALSYS_SyncLeave(pSpmiDevCtxt->hSpmiIntrCtlrDeviceSynchronization);

    SPMI_LOG_TRACE("DalSpmi: Spmi_UnregisterIsr func Exit, Status: %d", result);
    return result;
}

static void
Spmi_InitInterface(SpmiClientCtxt* clientCtx)
{
    static const DalSpmi vtbl =
    {
        {
            Spmi_DeviceAttach,
                Spmi_DeviceDetach,
                Spmi_DeviceInit,
                Spmi_DeviceDeInit,
                Spmi_DeviceOpen,
                Spmi_DeviceClose,
                Spmi_DeviceInfo,
                Spmi_DevicePowerEvent,
                Spmi_DeviceSysRequest
        },

        Spmi_ReadLong,
        Spmi_WriteLong,
        Spmi_ExecCommand,
        Spmi_RegisterIsr,
        Spmi_UnregisterIsr,
        Spmi_ReadModifyWriteLongByte
    };
    /*--------------------------------------------------------------------------
    Depending upon client type setup the vtables (entry points)
    --------------------------------------------------------------------------*/
    clientCtx->DalSpmiHandle.dalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    clientCtx->DalSpmiHandle.vTbl = &vtbl;
    clientCtx->DalSpmiHandle.clientCtx = clientCtx;
}

DALResult Spmi_DeviceAttach(
                            const char *pszArg,
                            DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
    DALResult nErr;
    uint32 i;
    static SpmiDrvCtxt drvCtxt = {{Spmi_DriverInit,
        Spmi_DriverDeInit
    },1,
    sizeof(SpmiDevCtxt)};

    SpmiClientCtxt *clientCtx = NULL;

    pSpmiDalDeviceCtxt = &(drvCtxt.SpmiDevCtxt[0]); /* This pointer is just used for facilitating debug */

    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceAttach func Entry");

    nErr = DALSYS_Malloc(sizeof(SpmiClientCtxt),(void **)&clientCtx);
    
    *phDalDevice = NULL;

    if (DAL_SUCCESS == nErr) {
        DALSYS_memset(clientCtx,0,sizeof(SpmiClientCtxt));
        for(int i = 0; i < MAX_INTRS_PER_CLIENT; i++) {
           clientCtx->registeredIntrs[i].periphID = INVALID_PERIPH_ID;
        }

        nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
            (DALClientCtxt *)clientCtx);
        if (DAL_SUCCESS == nErr)
        {
            Spmi_InitInterface(clientCtx);
            Spmi_AddRef(&(clientCtx->DalSpmiHandle));
            *phDalDevice = (DalDeviceHandle *)&(clientCtx->DalSpmiHandle);
            for(i=0; i<SPMI_DAL_DEBUG_MAX_CLIENT_HANDLES; i++)
            {
                if(!clientCtx->pSpmiDevCtxt->pSpmiHandles[i])
                {
                    clientCtx->pSpmiDevCtxt->pSpmiHandles[i] = (DalSpmiHandle *) &(clientCtx->DalSpmiHandle);
                    break;
                }
            }
            clientCtx->pSpmiDevCtxt->uHandlesCurrentlyActive++;
            SPMI_LOG_INFO("DalSpmi: Spmi_DeviceAttach: New client ctxt created at 0x%x with handle 0x%x", clientCtx, *phDalDevice);
        }
    }

    SPMI_LOG_TRACE("DalSpmi: Spmi_DeviceAttach func Exit, Status: %d", nErr);
    return nErr;
}



