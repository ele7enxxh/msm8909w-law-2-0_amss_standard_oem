/*==============================================================================

FILE:      DalSpiFwk.c

DESCRIPTION: This file implements a Spi DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/driver/DalSpiFwk.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-06-06  dk   Added dynamic device context initialisation requirement.
  2010-06-14  ag   Based On Legacy DAL SPI driver, just moved for DAL1.

============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "ddispi.h"
#include "DalSpi.h"
#include "DALSys.h"
#include "string.h" //for memset warning


DALResult 
Spi_DalSpi_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
Spi_DalSpi_AddRef(DalSpiHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
Spi_DalSpi_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
    if ( 0 == dwref )
    {
      //Release the client ctxt;
      DALSYS_Free(h->pClientCtxt);
    }
	return dwref;
}

static DALResult 
Spi_DalSpi_Init(DalDeviceHandle *h)
{  
   SpiClientCtxt *pClientCtxt = (SpiClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pSpiDevCtxt->DevId,
	                           pClientCtxt->pSpiDevCtxt->hProp);
   return Spi_DeviceInit(h->pClientCtxt);
}

static DALResult 
Spi_DalSpi_DeInit(DalDeviceHandle *h)
{
   return Spi_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
Spi_DalSpi_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return Spi_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
Spi_DalSpi_Open(DalDeviceHandle* h, uint32 mode)
{
    return Spi_Open(h->pClientCtxt,mode);
}

static DALResult 
Spi_DalSpi_Close(DalDeviceHandle* h)
{
    return Spi_Close(h->pClientCtxt);
}

static DALResult 
Spi_DalSpi_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALSPI_INTERFACE_VERSION;
   return Spi_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
Spi_DalSpi_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalSpi Interface. 
------------------------------------------------------------------------------*/

static DALResult 
Spi_DalSpi_GetInputBlockSize( DalDeviceHandle * h,  void*  inputBlockSize, uint32 olen1) 
{
	return Spi_GetInputBlockSize(((DalSpiHandle *)h)->pClientCtxt, (uint32 *)inputBlockSize, olen1);
}
static DALResult 
Spi_DalSpi_GetOutputBlockSize( DalDeviceHandle * h,  void*  outputBlockSize, uint32 olen1) 
{
	return Spi_GetOutputBlockSize(((DalSpiHandle *)h)->pClientCtxt, (uint32 *)outputBlockSize, olen1);
}
static DALResult 
Spi_DalSpi_ConfigureDevice( DalDeviceHandle * h,  const SpiDeviceInfoType *  deviceInfo) 
{
	return Spi_ConfigureDevice(((DalSpiHandle *)h)->pClientCtxt, deviceInfo);
}
static DALResult 
Spi_DalSpi_GetDeviceConfiguration( DalDeviceHandle * h,  void * deviceInfo, uint32 olen1) 
{
	return Spi_GetDeviceConfiguration(((DalSpiHandle *)h)->pClientCtxt, (SpiDeviceInfoType *)deviceInfo, olen1);
}
static DALResult 
Spi_DalSpi_StartTransfer( DalDeviceHandle * h,  SpiTransferType *  transfer,  SpiDataAddrType*  outputAddr,  int  outputDataLen,  SpiDataAddrType*  inputAddr,  int  inputDataLen,  int*  inputDataLenReq, uint32 olen6,  void*  transferId, uint32 olen7) 
{
	return Spi_StartTransfer(((DalSpiHandle *)h)->pClientCtxt, transfer, outputAddr, outputDataLen, inputAddr, inputDataLen, inputDataLenReq, olen6, (uint32 *) transferId, olen7);
}
static DALResult 
Spi_DalSpi_GetTransferStatus( DalDeviceHandle * h,  uint32  transferId) 
{
	return Spi_GetTransferStatus(((DalSpiHandle *)h)->pClientCtxt, transferId);
}
static DALResult 
Spi_DalSpi_Send( DalDeviceHandle * h,  SpiDataAddrType*  outputAddr,  int  outputDataLen) 
{
	return Spi_Send(((DalSpiHandle *)h)->pClientCtxt, outputAddr, outputDataLen);
}
static DALResult 
Spi_DalSpi_Receive( DalDeviceHandle * h,  uint32  u32NumInputTransfers,  SpiDataAddrType*  inputAddr,  int  inputDataLen,  int*  inputDataLenReq, uint32 olen4) 
{
	return Spi_Receive(((DalSpiHandle *)h)->pClientCtxt, u32NumInputTransfers, inputAddr, inputDataLen, inputDataLenReq, olen4);
}
static DALResult 
Spi_DalSpi_SendAndReceive( DalDeviceHandle * h,  SpiDataAddrType*  outputAddr,  int  outputDataLen,  uint32  u32NumInputTransfers,  SpiDataAddrType*  inputAddr,  int  inputDataLen,  int*  inputDataLenReq, uint32 olen6) 
{
	return Spi_SendAndReceive(((DalSpiHandle *)h)->pClientCtxt, outputAddr, outputDataLen, u32NumInputTransfers, inputAddr, inputDataLen, inputDataLenReq, olen6);
}
static DALResult 
Spi_DalSpi_Reset( DalDeviceHandle * h) 
{
	return Spi_Reset(((DalSpiHandle *)h)->pClientCtxt);
}

static void 
Spi_InitInterface(SpiClientCtxt* pclientCtxt)
{
    static const DalSpi vtbl = {
       {
          Spi_DalSpi_Attach,
          Spi_DalSpi_Detach,
          Spi_DalSpi_Init,
          Spi_DalSpi_DeInit,
          Spi_DalSpi_Open,
          Spi_DalSpi_Close,
          Spi_DalSpi_Info,
          Spi_DalSpi_PowerEvent,
          Spi_DalSpi_SysRequest
        } ,
		Spi_DalSpi_GetInputBlockSize,
		Spi_DalSpi_GetOutputBlockSize,
		Spi_DalSpi_ConfigureDevice,
		Spi_DalSpi_GetDeviceConfiguration,
		Spi_DalSpi_StartTransfer,
		Spi_DalSpi_GetTransferStatus,
		Spi_DalSpi_Send,
		Spi_DalSpi_Receive,
                Spi_DalSpi_SendAndReceive,
		Spi_DalSpi_Reset
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalSpiHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalSpiHandle.pVtbl  = &vtbl;
    pclientCtxt->DalSpiHandle.pClientCtxt = pclientCtxt;

}

DALResult 
Spi_DalSpi_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
   DALResult nErr = DAL_SUCCESS;
   
   if ( phDalDevice != NULL )
   {
    
      SpiClientCtxt *pclientCtxt = NULL;
      static SpiDrvCtxt * drvCtxt = NULL;
           
      if(NULL == drvCtxt) /* add mutual exclusion to this part as desired. */
      {
         uint32 num_devices;
         DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
         DALSYSPropertyVar prop;
         uint32 size;

         if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(DeviceId, hProp )) 
         {
            return DAL_ERROR;
         }
   
         if (DAL_SUCCESS != DALSYS_GetPropertyValue(hProp, "num_devices",
                                                    0, &prop )) 
         {
            return DAL_ERROR;
         }
         num_devices = (uint32)prop.Val.dwVal;

         size = sizeof(SpiDrvCtxt) + sizeof(SpiDevCtxt)*(num_devices-1);
         
         nErr = DALSYS_Malloc(size,(void **)&drvCtxt);

         if ((DAL_SUCCESS != nErr) || (drvCtxt == NULL)) 
         {
            return DAL_ERROR;
         }
         
         DALSYS_memset(drvCtxt,0,size);
         
         drvCtxt->SpiDALVtbl.Spi_DriverInit = Spi_DriverInit;
         drvCtxt->SpiDALVtbl.Spi_DriverDeInit = Spi_DriverDeInit ;
         drvCtxt->dwNumDev = num_devices;
         drvCtxt->dwSizeDevCtxt = sizeof(SpiDevCtxt);
      }
 

      nErr = DALSYS_Malloc(sizeof(SpiClientCtxt),(void **)&pclientCtxt);


      *phDalDevice = NULL;
   
      if ((DAL_SUCCESS == nErr) && (pclientCtxt != NULL)) 
      {
         DALSYS_memset(pclientCtxt,0,sizeof(SpiClientCtxt));
         nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)drvCtxt,
                                   (DALClientCtxt *)pclientCtxt);
         if (DAL_SUCCESS == nErr)
         {
            Spi_InitInterface(pclientCtxt);
            Spi_DalSpi_AddRef(&(pclientCtxt->DalSpiHandle));
            *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalSpiHandle);
         }
      }
   }
   return nErr;
}

