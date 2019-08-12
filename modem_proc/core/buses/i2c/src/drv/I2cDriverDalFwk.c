/*=============================================================================

  FILE:   DALI2CFWK.C

  OVERVIEW: This file implements a framework I2C DeviceDriver.
 
            Copyright c 2008-2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary
  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cDriverDalFwk.c#1 $
  $DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $
 
  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  08/25/15 VG     Added changes for I2C Lite Driver
  02/22/10 UR     Checking for return value of DALSYS_Malloc before doing a
                  DALSYS_memset.
  02/02/10 UR     Changed Bus Acquire Wait Time resolution to milliseconds
  06/29/08 PS     Created

  ===========================================================================*/
 

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "I2cDriverDalFwk.h"
#include "DALDeviceId.h"

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

#define  I2C_GET_DRV_DEVICE(h) (((DalI2cClientCtxt *)((h)->pClientCtxt))->pDalI2cDevCtxt->i2cBus)

#define DAL_I2CDEVID_TO_BUSID(A)                            \
        ((A) == DALDEVICEID_I2C_DEVICE_1)? I2CDRV_I2C_1 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_2)? I2CDRV_I2C_2 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_3)? I2CDRV_I2C_3 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_4)? I2CDRV_I2C_4 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_5)? I2CDRV_I2C_5 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_6)? I2CDRV_I2C_6 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_7)? I2CDRV_I2C_7 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_8)? I2CDRV_I2C_8 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_9)? I2CDRV_I2C_9 :   \
        ((A) == DALDEVICEID_I2C_DEVICE_10)? I2CDRV_I2C_10 : \
        ((A) == DALDEVICEID_I2C_DEVICE_11)? I2CDRV_I2C_11 : \
        ((A) == DALDEVICEID_I2C_DEVICE_12)? I2CDRV_I2C_12 : \
        I2CDRV_I2C_DEV_INVALID

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

DALResult
I2C_DalI2C_Attach
(
   const char *,
   DALDEVICEID,
   DalDeviceHandle **
);


/*-------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ----------------------------------------------------------------------*/

static uint32
I2C_DalI2C_AddRef
(
   DalI2CHandle* h
)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

/** @brief Detaches from the device.

    @param[in] h Pointer to dal device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static uint32
I2C_DalI2C_Detach
(
   DalDeviceHandle* h
)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   if( 0 == dwref ) {
      /* Release the client ctxt*/
      DALSYS_Free(h->pClientCtxt);
   }
   return dwref;
}

/** @brief Initializes the i2c device.

    @param[in] h Pointer to dal device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Init
(
   DalDeviceHandle *h
)
{
   I2cDrv_I2cBusId i2cBusId;
   int32 res;   
   DalI2cClientCtxt *pClientCtxt = (DalI2cClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pDalI2cDevCtxt->DevId,
	                           pClientCtxt->pDalI2cDevCtxt->hProp);
   i2cBusId = DAL_I2CDEVID_TO_BUSID(pClientCtxt->pDalI2cDevCtxt->DevId);

   res = I2cDrv_Open(i2cBusId, &I2C_GET_DRV_DEVICE(h), 0);
   if (I2C_RES_SUCCESS != res)
   {
      return (DALResult)res;     
   }
   res = I2cDrv_Cmd(&I2C_GET_DRV_DEVICE(h), I2cDrvCmd_Vote_PnocOn, NULL);
   return (DALResult)res;
}

/** @brief Deinitializes the i2c device.

    @param[in] h Pointer to dal device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_DeInit
(
   DalDeviceHandle *h
)
{
   int32 res1, res2;

   res1 = I2cDrv_Cmd(&I2C_GET_DRV_DEVICE(h), I2cDrvCmd_Vote_PnocOff, NULL);
   res2 = I2cDrv_Close(&I2C_GET_DRV_DEVICE(h));
   if (I2C_RES_SUCCESS != res1)
   {
      return (DALResult)res1;     
   }
   return (DALResult)res2;
}

/** @brief Sends a power event the i2c device.

    @param[in] h           Pointer to dal device handle.
    @param[in] PowerCmd    Power cmd.
    @param[in] PowerDomain Power domain.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_PowerEvent
(
   DalDeviceHandle *h,
   DalPowerCmd PowerCmd,
   DalPowerDomain PowerDomain 
)
{
   return DAL_SUCCESS;
}

/** @brief Opens the handle to the i2c device.

    @param[in] h           Pointer to dal device handle.
    @param[in] mode        Open mode.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Open
(
   DalDeviceHandle* h,
   uint32           mode
)
{
   
   return I2cDrv_Cmd(&I2C_GET_DRV_DEVICE(h), I2cDrvCmd_Vote_I2cClocsOn, NULL);     
}

/** @brief Closes the handle to the i2c device.

    @param[in] h           Pointer to dal device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Close
(
   DalDeviceHandle* h
)
{
   

   
   return I2cDrv_Cmd(&I2C_GET_DRV_DEVICE(h), I2cDrvCmd_Vote_I2cClocsOff, NULL);
}

/** @brief Opens the handle to the i2c device.

    @param[in]  h           Pointer to dal device handle.
    @param[out] pInfo       Pointer to info structure.
    @param[in]  uInfoSize   Info size.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Info
(
   DalDeviceHandle *h,
   DalDeviceInfo   *pInfo,
   uint32           uInfoSize
)
{
   pInfo->Version = DALI2C_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

/** @brief Sends a sys request to the i2c device.

    @param[in]   h               Pointer to dal device handle.
    @param[out]  ReqIdx          Request index.
    @param[in]   pSrcBuf         Pointer to src buffer.
    @param[in]   uSrcBufLen      Src buffer length.
    @param[out]  pDestBuf        Pointer to dest buffer.
    @param[out]  uDestBufLen     Dest buffer length.
    @param[out]  puDestBufLenReq Dest buffer length req.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_SysRequest
(
   DalDeviceHandle  *h,
   DalSysReq         ReqIdx,
   const void       *pSrcBuf,
   uint32            uSrcBufLen,
   void             *pDestBuf,
   uint32            uDestBufLen,
   uint32           *puDestBufLenReq
)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalI2C Interface.
------------------------------------------------------------------------------*/

/** @brief Performs a synchronous read transfer on the i2c
           device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pTransfer       Pointer to transfer structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] puNumCompleted  Pointer to number of completed
                                transfers.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Read
(
   DalDeviceHandle  *h,
   I2cTransfer      *pTransfer,
   I2cClientConfig  *pClntCfg,
   uint32           *puNumCompleted
)
{
   I2cDrv_I2cBus *pbus = &I2C_GET_DRV_DEVICE(h);
   pbus->clntCfg = *pClntCfg;
   pbus->clntCfg.uSlaveAddr = pTransfer->tranCfg.uSlaveAddr; // old API support

   return (DALResult)I2cDrv_Read(pbus, pTransfer, puNumCompleted);
}

/** @brief Performs a write transfer on the i2c device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pTransfer       Pointer to transfer structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] puNumCompleted  Pointer to number of complete
                                transfers.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Write
(
   DalDeviceHandle  *h,
   I2cTransfer      *pTransfer,
   I2cClientConfig  *pClntCfg,
   uint32           *puNumCompleted
)
{
   I2cDrv_I2cBus *pbus = &I2C_GET_DRV_DEVICE(h);
   pbus->clntCfg = *pClntCfg;
   pbus->clntCfg.uSlaveAddr = pTransfer->tranCfg.uSlaveAddr; // old API support
      
   return (DALResult)I2cDrv_Write(pbus, pTransfer, puNumCompleted);
}

/** @brief Opens the handle to the i2c device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pSequence       Pointer to sequence structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] pIoRes          Pointer to IO result.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_BatchTransfer
(
   DalDeviceHandle *h,
   I2cSequence     *pSequence,
   I2cClientConfig *pClntCfg,
   I2cIoResult     *pIoRes
)
{
   I2cDrv_I2cBus *pbus = &I2C_GET_DRV_DEVICE(h);
   pbus->clntCfg = *pClntCfg;
   pbus->clntCfg.uSlaveAddr = pSequence->pTransfer->tranCfg.uSlaveAddr; // old API support

   return I2cDrv_BatchTransfer( pbus, pSequence, pIoRes);
}

/** @brief Performs an asynchronous read transfer on the i2c
           device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pTransfer       Pointer to transfer structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] pIoRes          Pointer to io result.
    @param[out] pCallbackFn     Pointer to callback function.
    @param[out] pArg            Pointer to arg passed to 
                                callback function.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_AsyncRead
(
   DalDeviceHandle *h,
   I2cTransfer     *pTransfer,
   I2cClientConfig *pClntCfg,
   I2cIoResult     *pIoRes,
   DDIDRV_ASYNC_CB  pCallbackFn,
   void            *pArg
)
{
   return DAL_ERROR;
}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pTransfer       Pointer to transfer structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] pIoRes          Pointer to io result.
    @param[out] pCallbackFn     Pointer to callback function.
    @param[out] pArg            Pointer to arg passed to 
                                callback function.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_AsyncWrite
(
   DalDeviceHandle  *h,
   I2cTransfer      *pTransfer,
   I2cClientConfig  *pClntCfg,
   I2cIoResult      *pIoRes,
   DDIDRV_ASYNC_CB   pCallbackFn,
   void             *pArg
)
{
   return DAL_ERROR;
}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pSequence       Pointer to sequence structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] pIoRes          Pointer to io result.
    @param[out] pCallbackFn     Pointer to callback function.
    @param[out] pArg            Pointer to arg passed to 
                                callback function.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_AsyncBatchTransfer
(
   DalDeviceHandle *h,
   I2cSequence     *pSequence,
   I2cClientConfig *pClntCfg,
   I2cIoResult     *pIoRes,
   DDIDRV_ASYNC_CB  pCallbackFn,
   void            *pArg
)
{
   return DAL_ERROR;
}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  pclientCtxt     Pointer to client context.
    
    @return          Nothing.
  */
static void
I2C_InitInterface
(
   DalI2cClientCtxt* pclientCtxt
)
{
    static const DalI2C vtbl = {
       {
          I2C_DalI2C_Attach,
          I2C_DalI2C_Detach,
          I2C_DalI2C_Init,
          I2C_DalI2C_DeInit,
          I2C_DalI2C_Open,
          I2C_DalI2C_Close,
          I2C_DalI2C_Info,
          I2C_DalI2C_PowerEvent,
          I2C_DalI2C_SysRequest
        } ,
		I2C_DalI2C_Read,
		I2C_DalI2C_Write,
		I2C_DalI2C_BatchTransfer,
		I2C_DalI2C_AsyncRead,
		I2C_DalI2C_AsyncWrite,
		I2C_DalI2C_AsyncBatchTransfer
    };
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalI2CHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalI2CHandle.pVtbl  = &vtbl;
    pclientCtxt->DalI2CHandle.pClientCtxt = pclientCtxt;

}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  pDalDrvCtxt     Pointer to dal driver context.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
int
I2C_DalI2C_DriverInit
(
   DalI2cDrvCtxt *pDalDrvCtxt
)
{
   return DAL_SUCCESS;
}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  pDalDrvCtxt     Pointer to dal driver context.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
int
I2C_DalI2C_DriverDeInit
(
   DalI2cDrvCtxt *pDalDrvCtxt
)
{
   return DAL_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Attaches to the device and creates a dal device
           handle.

    @param[in] pszArg       Char string.
    @param[in] DeviceId     Platform device id.
    @param[out] phDalDevice Pointer to pointer to device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
DALResult
I2C_DalI2C_Attach
(
   const char       *pszArg,
   DALDEVICEID       DeviceId,
   DalDeviceHandle **phDalDevice
)
{
  DALResult nErr;
  static DalI2cDrvCtxt *pDrvCtxt = NULL;
  uint32         uDrvCtxtSize;
  

  DalI2cClientCtxt *pclientCtxt =  NULL;

  if (NULL == pDrvCtxt) {
     
     if ( 0 == uNumI2cBuses ) {
         return DAL_ERROR;
     }
     uDrvCtxtSize = sizeof(DalI2cDrvCtxt) + (uNumI2cBuses - 1) *
     sizeof(DalI2cDevCtxt);
     nErr = DALSYS_Malloc(uDrvCtxtSize, (void **)&pDrvCtxt);

     if ( nErr != DAL_SUCCESS ) {
        return nErr;
     }

     if ( NULL == pDrvCtxt ) {
        return DAL_ERROR;
     }
     DALSYS_memset(pDrvCtxt, 0, uDrvCtxtSize);
     pDrvCtxt->I2CDALVtbl.I2C_DriverInit = I2C_DalI2C_DriverInit;
     pDrvCtxt->I2CDALVtbl.I2C_DriverDeInit = I2C_DalI2C_DriverDeInit;
     pDrvCtxt->dwNumDev = uNumI2cBuses;
     pDrvCtxt->dwSizeDevCtxt = sizeof(DalI2cDevCtxt);

  }

  nErr = DALSYS_Malloc(sizeof(DalI2cClientCtxt), (void **)&pclientCtxt);

  *phDalDevice = NULL;

  if ((DAL_SUCCESS == nErr) && (NULL != pclientCtxt)) {
    DALSYS_memset(pclientCtxt,0,sizeof(DalI2cClientCtxt));

    nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)pDrvCtxt,
        (DALClientCtxt *)pclientCtxt);
    if (DAL_SUCCESS == nErr)
    {
      I2C_InitInterface(pclientCtxt);
      I2C_DalI2C_AddRef(&(pclientCtxt->DalI2CHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalI2CHandle);
    }
  }
  return nErr;
}

