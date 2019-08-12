/*
===========================================================================

FILE:   SpiDevice.c

DESCRIPTION:
    This file contains the implementation of the API for the QUPSPIDeviceQup 
    Interface: IQupSpiDeviceQup 

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/device/SpiDevice.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
04/25/16 dpk    Added HS mode support and Set frequency API
04/21/15 dpk    Making the pointer to NULL after free
05/15/13 ag     Created.
===========================================================================
        Copyright (c) 2013-16 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential & Proprietary
===========================================================================
*/

#include "SpiDevice.h"
#include "SpiDeviceTransfer.h"
#include "SpiDevicePlatSvc.h"
#include "SpiDeviceOsSvc.h"
#include "HALqupe.h"
#define SPIDEVICE_LOG_H
#include "SpiLog.h"

/*  this can be a constant for now but it can be */
/*  read from enumeration data.  */
#define MAX_DEVICES    12
/*  Timeout value for while polling for QUP state validity check */
#define QUP_RUN_STATE_CHANGE_TIME_OUT_US         100
#define QUP_BUSY_WAIT_STEP_US                    5
#define QUP_BUSY_WAIT_POLL_STEP_US               1
#define QUP_BUSY_WAIT_POLL_MAX_US                2000


enum  SpiDevice_Error
{
   SPIDEVICE_ERR_BASE = SPIDEVICE_RES_ERROR_CLS_QUP_DEV, 
        /*  Under device class the first byte is reserved to qup errors */
   SPIDEVICE_ERR_INVALID_DEVICE_ID = 0x30000100,
   SPIDEVICE_ERR_INVALID_PTR,
   SPIDEVICE_ERR_IN_SEQ_OUT_OF_SYNC            ,
   SPIDEVICE_ERR_INVALID_RUNSTATE              ,
   SPIDEVICE_ERR_RUNSTATE_CHANGE_TIMEOUT       ,
   SPIDEVICE_ERR_INVALID_POWER_STATE           ,
   SPIDEVICE_ERR_BUS_BUSY                      ,

   SPIDEVICE_ERR_INVALID_DEV_HANDLE            ,
   SPIDEVICE_ERR_INVALID_BUFFER_POINTER         ,
   SPIDEVICE_ERR_INVALID_BUFFER_LEN            ,
   SPIDEVICE_ERR_INVALID_CONFIG_POINTER      ,
   SPIDEVICE_ERR_INVALID_TRAN_RESULT_POINTER   ,
   SPIDEVICE_ERR_INVALID_CALLBACK_FN_POINTER   ,
   SPIDEVICE_ERR_INVALID_CALLBACK_ARG_POINTER  ,
   SPIDEVICE_ERR_INVALID_NUM_BATCH_TRANSFER_VECS,
   SPIDEVICE_ERR_OPERATION_MODE_UNSUPPORTED,
   SPIDEVICE_ERR_NULL_PTR,
   SPIDEVICE_ERR_CLOCK_SETTING_FAILED
};
#define SPIDEVICE_VALIDATE_CONFIG_PARAMS(hDev,pClntCfg)\
     ( (NULL == hDev)?           SPIDEVICE_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pClntCfg)?      SPIDEVICE_ERR_INVALID_CONFIG_POINTER:     \
       SPIDEVICE_RES_SUCCESS \
     )

#define SPIDEVICE_VALIDATE_WR_PARAMS(pDev,cfg, pReadBuffer, uReadBufLen, pWriteBuffer, uWriteBufLen )\
	{\
      if(NULL == pDev) return SPIDEVICE_ERR_INVALID_DEV_HANDLE;\
      if((NULL == cfg)&&(NULL == pDev->pClntCfg))return SPIDEVICE_ERR_INVALID_CONFIG_POINTER;\
      if((NULL == pReadBuffer) && (NULL == pWriteBuffer)) return SPIDEVICE_ERR_INVALID_BUFFER_POINTER;\
      if((0 == uReadBufLen) && (0 == uWriteBufLen)) return SPIDEVICE_ERR_INVALID_BUFFER_LEN;\
	}

#define SPIDEVICE_VALIDATE_ASYNC_CALLBACK_PARAMS(pCallbackFn, pArg)\
     ( (NULL == pCallbackFn)?           SPIDEVICE_ERR_INVALID_CALLBACK_FN_POINTER:           \
       (NULL == pArg)?      SPIDEVICE_ERR_INVALID_CALLBACK_ARG_POINTER:     \
       SPIDEVICE_RES_SUCCESS \
     )

#define SPIDEVICE_VALIDATE_BATCH_TRANSFER_PARAMS(pDev, pTransfer, uNumTransferVecs, pQupConfig, puNumCompleted)\
	{\
      if(NULL == pDev) return SPIDEVICE_ERR_INVALID_DEV_HANDLE;\
      if((NULL == pQupConfig)&&(NULL == pDev->pClntCfg))return SPIDEVICE_ERR_INVALID_CONFIG_POINTER;\
      if(0 == uNumTransferVecs) return SPIDEVICE_ERR_INVALID_NUM_BATCH_TRANSFER_VECS;\
      if(NULL == pTransfer) return SPIDEVICE_ERR_INVALID_PTR;\
      if(NULL == puNumCompleted) return SPIDEVICE_ERR_INVALID_PTR;\
	}

#define SPIDEVICE_CHK_RESULT(res)\
   if (SPIDEVICE_RES_SUCCESS !=res) {\
      return res;\
   }
#define SPIDEVICE_CHK_RESULT_BREAK(res)\
   if (SPIDEVICE_RES_SUCCESS !=res) {\
      break;\
   }
#define SPIDEVICE_CHK_RESULT_VOID(res)\
   if (SPIDEVICE_RES_SUCCESS !=res) {\
      return;\
   }
#define SPIDEVICE_MAX(a,b) (a > b)? a:b
#define SPIDEVICE_BAM_TRANSFER_MIN_LEN 64

typedef enum SpiDevice_InitState
{
   SPIDEVICE_INITSTATE_UNINITIALIZED     = 0x0,
   SPIDEVICE_INITSTATE_TARGETINIT_DONE   = 0x1,
   SPIDEVICE_INITSTATE_EVENTS_DONE       = 0x2,
   SPIDEVICE_INITSTATE_DEVICEINIT_DONE   = 0x4,
   SPIDEVICE_INITSTATE_HARDWAREINIT_DONE = 0x8,
} SpiDevice_InitState;


typedef struct SpiDeviceClientConfig
{
   uint32 uBusFreqKhz;
   uint32 uByteTransferTimeoutUs;
} SpiDeviceClientConfig;


typedef struct SpiDevice_AsyncCallData
{
   SPI_DEV_ASYNC_CB                        pCallbackFn;
   SpiDevice_AsyncResult                   asyncResult;   
} SpiDevice_AsyncCallData;

typedef struct SpiDevice_DeviceCtxt
{
   uint32                              uDevOSId;
   uint32                              uQupCoreNum;
   SpiDevicePlatInfoType                    platInfo;
   char                                *pHalVer;
   SpiDevice_InitState                devInitState;
   SPIDEVICEOSSVC_EVENT_HANDLE                 hQupEvt;
   SPIDEVICE_PLATCFG_HANDLE                  hDevTgt;
   SpiDeviceInfoType                *pClntCfg;
   SpiDevice_PowerStates               devPowerState;
   SpiDevice_AsyncCallData         asyncCall;
   SpiDeviceTransferType         transferInfo;
   SpiDeviceBamType           *bamObj;
   uint32								uPollXferComplete;
} SpiDevice_DeviceCtxt;

static int32 SpiDevice_SetRunState( uint8 *pQupAddr, const HAL_qupe_RunStateType cRunState);
static int32 SpiDevice_ConfigureAppsFrequency( SpiDevice_DeviceCtxt *pdev, SpiDeviceInfoType *pQupConfig);


/** @brief Wait for state to go valid or timeout.
           
    @param[in] pQupAddr       QUP block address.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */

static int32
SpiDevice_WaitForRunStateValid(uint8 *pQupAddr)
{
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;

   while( (!HAL_qupe_RunStateIsValid(pQupAddr)) && (0 != uWaitTimeUs) ) {
      SpiDeviceOsSvc_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
   }
   if( 0 == uWaitTimeUs ) {
      return SPIDEVICE_ERR_INVALID_RUNSTATE;
   }

	return SPIDEVICE_RES_SUCCESS;
}
// assume power is on since this is tested
int32
SpiDevice_InitHw
(
   SpiDevice_DeviceCtxt *pDev
)
{
	ISpiDevice_Result res;

	HAL_qupe_Init((uint8*)pDev->platInfo.qupVirtBlockAddr,
		&pDev->pHalVer);
    res = (ISpiDevice_Result) SpiDevice_WaitForRunStateValid((uint8*)pDev->platInfo.qupVirtBlockAddr);

	if (SPIDEVICE_RES_SUCCESS != res) 
	{
		SPIDEVICE_LOG(ERROR,
		   "SpiDevice_InitHw: Invalid state after reset 0x%x\n",
		    (unsigned int)res);
		return (int32)res; 
	}

	HAL_qupe_SetSelectedMinicore((uint8*)pDev->platInfo.qupVirtBlockAddr,
		HAL_QUPE_MINI_CORE_SPI); 


	pDev->devInitState |= SPIDEVICE_INITSTATE_HARDWAREINIT_DONE;

	return SPIDEVICE_RES_SUCCESS;
}

/** @brief Initializes the device.

    This function initializes the device and creates the
    necessary data structures to support other calls into the
    device. This function may be called multiple times under 
    different conditions so what fails one time may succeed th
    next time.

    @param[in] uDevOsId    Device index.
    @param[in] phDev     Pointer to device handle.

    @return int32 - SPIDEVICE_RES_SUCCESS  on success, error
            code on error.
  */
int32
SpiDevice_Init
(
   uint32                               uDevOsId,
   SPIDEVICE_HANDLE                        *phDev
)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   int32 return_value = SPIDEVICE_RES_SUCCESS;
   SpiDevice_DeviceCtxt *pDev;
   uint32 do_while_constant = 0;

   if (NULL == phDev) 
   {
      SPIDEVICE_LOG(ERROR,"SpiDevice_Init: Null pointer passed \n");
      return SPIDEVICE_ERR_INVALID_DEV_HANDLE;
   }
   

   res = (int32)SpiDeviceOsSvc_Malloc((void **)&pDev,
                                         sizeof(SpiDevice_DeviceCtxt));
   
   if (SPIDEVICE_RES_SUCCESS != res) 
   {
      SPIDEVICE_LOG(ERROR,"SpiDevice_Init: Malloc failure\n");
      return res;
   }
   SpiDeviceOsSvc_Memset((void *)pDev, 0, sizeof(SpiDevice_DeviceCtxt));
   pDev->devInitState = SPIDEVICE_INITSTATE_UNINITIALIZED;

   do {
      res = (int32)SpiDevicePlatCfg_InitTarget(uDevOsId, &pDev->hDevTgt);
      
      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "SpiDevice_Init:SpiDevicePlatCfg_InitTarget Failure err 0x%x\n",
         (unsigned int)res);
         break;
      }
      pDev->devInitState |= SPIDEVICE_INITSTATE_TARGETINIT_DONE;

      res = (int32)SpiDevicePlatCfg_GetPlatformInfo(pDev->hDevTgt, &pDev->platInfo);

      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "SpiDevicePlatCfg_InitTarget:SpiDevicePlatCfg_GetPlatformInfo Failure err 0x%x\n",
         (unsigned int)res);
         break;
      }
     pDev->uQupCoreNum = pDev->platInfo.uQupCoreNum;
     pDev->bamObj = pDev->platInfo.spiBam;

      if (pDev->platInfo.bInterruptBased)
      {
         res = (int32)SpiDeviceOsSvc_CreateEvent(&pDev->hQupEvt);
         if (SPIDEVICE_RES_SUCCESS != res)
         {
            break;
         }
      }
      
      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "SpiDevicePlatCfg_InitTarget: SpiDeviceOsSvc_CreateEvent Failure err 0x%x\n",
         (unsigned int)res);
         break;
      }

      pDev->devInitState |= SPIDEVICE_INITSTATE_EVENTS_DONE; 
      pDev->devInitState |= SPIDEVICE_INITSTATE_DEVICEINIT_DONE;
      pDev->devPowerState = SPIDEVICE_POWER_STATE_0;
      *phDev = pDev;
        res = SpiDevice_InitHw(pDev);
   
         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
            "_SpiDevice_WriteRead_BamMode: SpiDevice_InitHw Failure err 0x%x\n",
            (unsigned int)res);
            break;
         }
   } while(do_while_constant);

   if (SPIDEVICE_RES_SUCCESS != res) 
   {  
      return_value = res;

      /*  in case of error undo initialization */
      if (SPIDEVICE_INITSTATE_TARGETINIT_DONE & pDev->devInitState) 
      {
         res = SpiDevicePlatCfg_DeInitTarget(pDev->hDevTgt);

         if (SPIDEVICE_RES_SUCCESS != res) 
         {
            SPIDEVICE_LOG(ERROR,
            "SpiDevicePlatCfg_InitTarget:SpiDevicePlatCfg_DeInitTarget Failure err 0x%x\n",
             (unsigned int)res);
         }
      }
      res =  SpiDeviceOsSvc_Free(pDev);
      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "SpiDevicePlatCfg_InitTarget:SpiDeviceOsSvc_Free Failure err 0x%x\n",
          (unsigned int)res);
      }

      pDev = NULL;
      *phDev = pDev;
   }

   /* Disable clocks */
   res = SpiDevice_SetPowerState(pDev, SPIDEVICE_POWER_STATE_0);
   if (SPIDEVICE_RES_SUCCESS != res)
   {
      SPIDEVICE_LOG(ERROR,
            "SpiDevicePlatCfg_InitTarget:SpiDevice_SetPowerState Failure err 0x%x\n",
             (unsigned int)res);
   }
   	
   return return_value;
}



/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.

    @param[in] hDev      Device handle.

    @return int32 - SPIDEVICE_RES_SUCCESS on success, error code
            on error.
  */
int32
SpiDevice_DeInit
(
   SPIDEVICE_HANDLE                        hDev 
)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   int32 tgtDeinitRes = SPIDEVICE_RES_SUCCESS;
   int32 closeHRes = SPIDEVICE_RES_SUCCESS;
   SpiDevice_DeviceCtxt *pDev = (SpiDevice_DeviceCtxt *)hDev;

   if (NULL == hDev) 
   {
      SPIDEVICE_LOG(ERROR,"SpiDevice_DeInit: Null pointer passed \n");
      return SPIDEVICE_ERR_INVALID_DEV_HANDLE;
   }

   if (SPIDEVICE_INITSTATE_EVENTS_DONE & pDev->devInitState)
   {
      if (pDev->platInfo.bInterruptBased)
      {
         closeHRes = (int32)SpiDeviceOsSvc_CloseHandle(pDev->hQupEvt);
         if (SPIDEVICE_RES_SUCCESS != closeHRes) 
         {
      SPIDEVICE_LOG(ERROR,
            "SpiDevice_DeInit:SpiDeviceOsSvc_CloseHandle Failure err 0x%x\n",
            (unsigned int)closeHRes);
         }
      }
   }

   if (SPIDEVICE_INITSTATE_TARGETINIT_DONE & pDev->devInitState)
   {
      tgtDeinitRes = (int32) SpiDevicePlatCfg_DeInitTarget(pDev->hDevTgt);
   }          

   if (SPIDEVICE_RES_SUCCESS != tgtDeinitRes) 
   {
      SPIDEVICE_LOG(ERROR,
      "SpiDevice_DeInit:SpiDevicePlatCfg_DeInitTarget Failure err 0x%x\n",
       (unsigned int)tgtDeinitRes);
   }
   
   res = (int32)SpiDeviceOsSvc_Free(pDev);

   if (SPIDEVICE_RES_SUCCESS != res) 
   {
      SPIDEVICE_LOG(ERROR,
      "SpiDevice_DeInit:SpiDeviceOsSvc_Free Failure err 0x%x\n",
       (unsigned int)res);
   }

   if (SPIDEVICE_RES_SUCCESS != closeHRes) {/*  return the first error */
      return closeHRes;
   }
   else if (SPIDEVICE_RES_SUCCESS != tgtDeinitRes) {
      return tgtDeinitRes;  
   }

   return res;
}



/** @brief Sets the runstate. It will wait untill the state is valid
           before trying to set the new state or timeout.
           After setting the state QUP_GetRunState() to check that the new
           state is valid.
    @param[in] uQupId       QUP index.
    @param[in] cRunState    The new run state.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */
static int32
SpiDevice_SetRunState
(
      uint8 *pQupAddr, 
      const HAL_qupe_RunStateType cRunState
)
{
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;
   while((!HAL_qupe_RunStateIsValid(pQupAddr)) && (0 != uWaitTimeUs)){
      SpiDeviceOsSvc_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
   }
   if(0 == uWaitTimeUs){
      return SPIDEVICE_ERR_INVALID_RUNSTATE;
   }

   HAL_qupe_SetRunState(pQupAddr, cRunState);
   while((!HAL_qupe_RunStateIsValid(pQupAddr)) && (0 != uWaitTimeUs)){
      SpiDeviceOsSvc_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
   }
   if(0 == uWaitTimeUs){
      return SPIDEVICE_ERR_RUNSTATE_CHANGE_TIMEOUT;
   }

   return SPIDEVICE_RES_SUCCESS;
}

/** @brief waits for any of the events specifiend it the argument.
    @param[in] pDev         Pointer to device structure.
    @return                 Nothing.
  */
static void
QUP_WaitForCompletion
(
   SpiDevice_DeviceCtxt                    *pDev 
)
{
   SpiDeviceBamResult *bamRes;
   int32 res;

   bamRes = pDev->bamObj->pollBamTransfer(pDev->bamObj);
   pDev->transferInfo.transferResult = bamRes->result;

   res = SpiDevice_SetRunState((uint8*)pDev->platInfo.qupVirtBlockAddr, HAL_QUPE_RUNSTATE_PAUSE);

   if (SPIDEVICE_RES_SUCCESS != res) 
   {
      SPIDEVICE_LOG(ERROR,
      "QUP_WaitForCompletion:SpiDevice_SetRunState Failure err 0x%x\n",
       (unsigned int)res);
   }

   res = SpiDevice_SetRunState((uint8*)pDev->platInfo.qupVirtBlockAddr, HAL_QUPE_RUNSTATE_RESET);

   if (SPIDEVICE_RES_SUCCESS != res) 
   {
      SPIDEVICE_LOG(ERROR,
      "QUP_WaitForCompletion:SpiDevice_SetRunState Failure err 0x%x\n",
       (unsigned int)res);
   }
      
}

/*======================================================================

  FUNCTION   

  DESCRIPTION  This function gets the device parameters for the specified
               device and returns them in the format required for the HAL.

  DEPENDENCIES    None
  PARAMETERS      pDeviceConfiguration - Pointer to the DAL device config
                     specified by the client.
                  pmHalDeviceParameters - Pointer to the HAL parameters
                     that will be returned.

  RETURN VALUE    None

  SIDE EFFECTS    None
======================================================================*/
static void SpiDevice_GetHalDeviceParameters
(
 const SpiDeviceInfoType * pDeviceConfiguration, 
 HAL_QUPE_SPI_DeviceParametersType *pmHalDeviceParameters
 )
{
   const SpiDeviceParametersType *pmDeviceParameters          
      = &(pDeviceConfiguration->deviceParameters);
   const SpiDeviceBoardInfoType *pDeviceBoardInfo
      = &(pDeviceConfiguration->deviceBoardInfo);

   switch (pmDeviceParameters->eClockPolarity)
   {
      case SPI_CLK_IDLE_HIGH:
         pmHalDeviceParameters->eClockPolarity = HAL_QUPE_SPI_CLK_IDLE_HIGH;
         break;

      case SPI_CLK_IDLE_LOW:
         pmHalDeviceParameters->eClockPolarity = HAL_QUPE_SPI_CLK_IDLE_LOW;
         break;

      default:
         pmHalDeviceParameters->eClockPolarity = HAL_QUPE_SPI_CLK_IDLE_HIGH;
         break;
   }

   switch (pmDeviceParameters->eShiftMode)
   {
      case SPI_INPUT_FIRST_MODE:
         pmHalDeviceParameters->eTransferMode = HAL_QUPE_SPI_INPUT_FIRST_MODE;
         break;

      case SPI_OUTPUT_FIRST_MODE:
         /* fallthrough */
      default:
         pmHalDeviceParameters->eTransferMode = HAL_QUPE_SPI_OUTPUT_FIRST_MODE;
   }

   switch(pmDeviceParameters->eClockMode)
   {
      case SPI_CLK_ALWAYS_ON:
         pmHalDeviceParameters->bClockAlwaysOn = TRUE;
         break;
      case SPI_CLK_NORMAL:
         /* fallthrough */
      default:
         pmHalDeviceParameters->bClockAlwaysOn = FALSE;
   }

   switch (pmDeviceParameters->eCSMode)
   {
      case SPI_CS_KEEP_ASSERTED:
         pmHalDeviceParameters->bChipSelectAlwaysOn = TRUE;
         break;
      case SPI_CS_DEASSERT:
         /* fallthough */
      default:
         pmHalDeviceParameters->bChipSelectAlwaysOn = FALSE;
		 break;
   }

   switch (pmDeviceParameters->eHSMode)
   {
      case SPI_HS_ENABLED:
         pmHalDeviceParameters->eSpiCoreHS =HAL_QUPE_SPI_HS_ENABLED;
         break;
      case SPI_HS_DISABLED:
         /* fallthrough */
      default:
         pmHalDeviceParameters->eSpiCoreHS = HAL_QUPE_SPI_HS_DISABLED;
		 break;
   }

   switch (pmDeviceParameters->eCSPolarity)
   {
      case SPI_CS_ACTIVE_LOW:
         pmHalDeviceParameters->eCSPolarity = HAL_QUPE_SPI_CS_ACTIVE_LOW;
         break;

      case SPI_CS_ACTIVE_HIGH:
         pmHalDeviceParameters->eCSPolarity = HAL_QUPE_SPI_CS_ACTIVE_HIGH;
         break;

      default:
         pmHalDeviceParameters->eCSPolarity = HAL_QUPE_SPI_CS_ACTIVE_LOW;
         break;
   }
   pmHalDeviceParameters->nChipSelectId = pDeviceBoardInfo->nSlaveNumber;
} 

/*======================================================================

  FUNCTION   

  DESCRIPTION  This function gets the transfer parameters for the specified
               device and returns them in the format required for the HAL.

  DEPENDENCIES    None
  PARAMETERS      pQupConfig- Pointer to the DAL device config
                     specified by the client.
                  pmHalTransferParameters - Pointer to the HAL parameters
                     that will be returned.

  RETURN VALUE    None

  SIDE EFFECTS    None
======================================================================*/
static void SpiDevice_GetHalTransferParameters
(
 const SpiDeviceInfoType * pQupConfig,
 HAL_QUPE_SPI_TransferParametersType *pmHalTransferParameters
 )
{
   const SpiTransferParametersType* pmTransferParameters
      = &(pQupConfig->transferParameters);

   pmHalTransferParameters->nNumBits = (uint8) pmTransferParameters->nNumBits;

   if(SPI_LOOPBACK_ENABLED == pmTransferParameters->eLoopbackMode)
   {
      pmHalTransferParameters->eLoopbackMode = HAL_QUPE_SPI_LOOPBACK_ENABLED;
   }
   else
   {
      pmHalTransferParameters->eLoopbackMode = HAL_QUPE_SPI_LOOPBACK_DISABLED;
   }
   pmHalTransferParameters->bReceivedDataPacked = TRUE;
   pmHalTransferParameters->bTransmitDataPacked = TRUE;
   if(pmHalTransferParameters->nNumBits > 16){
      pmHalTransferParameters->bReceivedDataPacked = FALSE;
      pmHalTransferParameters->bTransmitDataPacked = FALSE;
   }

   pmHalTransferParameters->eIOMode = HAL_QUPE_IO_MODE_BLOCK;

   /*
    * Always enable output bit shift enable so that there is
    * less confusion about how unpacked data is to be placed in the
    * user buffer.
    */
   pmHalTransferParameters->bOutputBitShift = TRUE;

   if(0 == pmHalTransferParameters->nNumInputTransfers)
   {
      pmHalTransferParameters->eTransferDirection = HAL_QUPE_SPI_OUTPUT_ONLY;
   }
   else if(0 == pmHalTransferParameters->nNumOutputTransfers)
   {
      pmHalTransferParameters->eTransferDirection = HAL_QUPE_SPI_INPUT_ONLY;
   }
   else
   {
      pmHalTransferParameters->eTransferDirection = HAL_QUPE_SPI_BI_DIRECTIONAL;
   }
} 


/** @brief Initializes transfer info structures.

    Initializes transfer info structures.

    @param[in] pDev           Pointer to device struct.
    @param[in] pTransfer      Pointer to transfer.
    @param[in] uNumTransfers  Number of transfers.

    @return                   Nothing.
  */
int32
SpiDevice_ConfigureQupSpiDevice_BamMode
(
   SpiDevice_DeviceCtxt                    *pDev, 
   SpiDeviceInfoType                         *pQupConfig
) 
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   uint32 u32MaxSlaveFrequency, u32MinSlaveFrequency, u32ActualClkSpeed = 0, 
      u32DeassertionTime, u32DeassertionClockTicks;
   SpiDeviceParametersType *pDeviceParameters;   

   HAL_QUPE_SPI_DeviceParametersType mHalDeviceParameters;

   if(SPI_CORE_MODE_SLAVE == pQupConfig->deviceBoardInfo.eCoreMode)
   {
      mHalDeviceParameters.eSpiCoreMode = HAL_QUPE_SPI_SLAVE;
   }
   else
   {
      mHalDeviceParameters.eSpiCoreMode = HAL_QUPE_SPI_MASTER;
   }

   pDeviceParameters = &(pQupConfig->deviceParameters);
   
   u32MaxSlaveFrequency = pDeviceParameters->u32MaxSlaveFrequencyHz;
   u32MinSlaveFrequency = pDeviceParameters->u32MinSlaveFrequencyHz;
   u32DeassertionTime = pDeviceParameters->u32DeassertionTime;
   u32ActualClkSpeed = u32MaxSlaveFrequency;

   
   if((u32MinSlaveFrequency > 0) && (u32ActualClkSpeed < u32MinSlaveFrequency))
   {
      SPIDEVICE_LOG(ERROR,"Failed to set frequency matching to slave spec");

      return SPIDEVICE_ERR_CLOCK_SETTING_FAILED;
   }

   /* Calculate the number of clock ticks to meet the deassertion time requirements */
   if(u32DeassertionTime > 0)
   {
      u32DeassertionClockTicks = 
            u32DeassertionTime / (1000000000UL / u32ActualClkSpeed) + 1;
   }
   else
   {
      u32DeassertionClockTicks = 0;
   }
   mHalDeviceParameters.nDeassertionClockTicks = u32DeassertionClockTicks;
   
   SpiDevice_GetHalDeviceParameters(pQupConfig, &mHalDeviceParameters);

   HAL_qupe_Spi_SetDeviceParameters((uint8*)pDev->platInfo.qupVirtBlockAddr, 
                       &mHalDeviceParameters);

   return res;
}

/** @brief This function selects the number of transfers needed.

    This function selects the number of transfers needed based on 
    the value of N size passed.

    @param[in] len            Length pased by user.
    @param[in] numBits     Value of N in bits.

    @return      uint32 No: of transfers needed..
  */
static uint32
SpiDevice_GetNumQUPTransfers
(
 uint32 len, 
 uint8 numBits
)
{
   uint32 numTransfers = 0;
   if(numBits > 16){
      numTransfers = len/sizeof(uint32);
   } else if(numBits > 8){
      numTransfers = len/sizeof(uint16);
   } else {
      numTransfers = len;
   }
   return numTransfers;
}

int32
SpiDevice_ValidateDeviceParams
(
   SpiDevice_DeviceCtxt                    *pDev,
   SpiDeviceInfoType                         *pQupConfig
 )
{
	pQupConfig = pQupConfig;
	pDev = pDev;
	//FIXME: Function in-complete
   return SPIDEVICE_RES_SUCCESS;

}
int32
SpiDevice_ValidateTransferParams
(
   SpiDevice_DeviceCtxt                    *pDev,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                  *pReadBuffer,
   uint32                           uReadBufLen,
   SpiDataAddrType                  *pWriteBuffer,
   uint32                           uWriteBufLen
)
{
	pDev = pDev;
	pQupConfig = pQupConfig;
	pReadBuffer = pReadBuffer;
	uReadBufLen = uReadBufLen;
	pWriteBuffer = pWriteBuffer;
	uWriteBufLen = uWriteBufLen;
	//FIXME: Function in-complete
   return SPIDEVICE_RES_SUCCESS;
}

//FIXME: Put function header.
int32
SpiDevice_ConfigureQupSpiTransfer_BamMode
(
   SpiDevice_DeviceCtxt                    *pDev,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                  *pReadBuffer,
   uint32                           uReadBufLen,
   SpiDataAddrType                  *pWriteBuffer,
   uint32                           uWriteBufLen
)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   HAL_QUPE_SPI_TransferParametersType mHalTransferParameters;

   SpiDeviceOsSvc_Memset((void *)&mHalTransferParameters, 0,
                         sizeof(HAL_QUPE_SPI_TransferParametersType));

   /*Setup output transfer params for HAL and QupTransferInfo*/
   mHalTransferParameters.nNumOutputTransfers = 0;
   pDev->transferInfo.bWriteTransferDone = TRUE;

   if((pWriteBuffer) && (pWriteBuffer->physicalAddr) && (uWriteBufLen)) {
      mHalTransferParameters.nNumOutputTransfers = 
         SpiDevice_GetNumQUPTransfers(uWriteBufLen, 
                   (uint8)pQupConfig->transferParameters.nNumBits);
      pDev->transferInfo.bWriteTransferDone = FALSE;
   }

   /*Setup input transfer params for HAL and QupTransferInfo*/
   mHalTransferParameters.nNumInputTransfers = 0;
   pDev->transferInfo.bReadTransferDone = TRUE;

   if((pReadBuffer) && (pReadBuffer->physicalAddr) && (uReadBufLen)){
      mHalTransferParameters.nNumInputTransfers = 
         SpiDevice_GetNumQUPTransfers(uReadBufLen, 
                   (uint8)pQupConfig->transferParameters.nNumBits);
      pDev->transferInfo.bReadTransferDone = FALSE;
   }

   /* Get the Transfer parameters */
   SpiDevice_GetHalTransferParameters(pQupConfig, &mHalTransferParameters);

   /* Set the transfer mode to BAM mode  */
   mHalTransferParameters.eIOMode = HAL_QUPE_IO_MODE_BAM;
   HAL_qupe_Spi_SetTransferParameters(
       (uint8*)pDev->platInfo.qupVirtBlockAddr,
       &mHalTransferParameters);
   return res;
}

static int32
_SpiDevice_WriteRead_BamMode
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                  *pReadBuffer,
   uint32                           uReadBufLen,
   SpiDataAddrType                  *pWriteBuffer,
   uint32                           uWriteBufLen,
   boolean                       eot
)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;
   uint32 do_while_constant = 0;

   do
   {
      res = SpiDevice_InitHw(pDev);

      if (res != SPIDEVICE_RES_SUCCESS)
      {
         SPIDEVICE_LOG(ERROR,
         "_SpiDevice_WriteRead_BamMode: SpiDevice_InitHw Failure err 0x%x\n",
         (unsigned int)res);
         break;
      }
   
   
      if (pDev->platInfo.bInterruptBased)
      {
         res  = SpiDeviceOsSvc_ClearEvent( pDev->hQupEvt);

         if (res != SPIDEVICE_RES_SUCCESS)
         {
         SPIDEVICE_LOG(ERROR,
                  "_SpiDevice_WriteRead_BamMode: SpiDeviceOsSvc_ClearEvent Failure err 0x%x\n",
                  (unsigned int)res);
            break;
         }
      }
   
      if(pQupConfig){
         res = SpiDevice_ValidateDeviceParams(pDev, pQupConfig);
   
         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
            "_SpiDevice_WriteRead_BamMode: SpiDevice_ValidateDeviceParams Failure err 0x%x\n",
            (unsigned int)res);
            break;
         }

         res = SpiDevice_ConfigureAppsFrequency(pDev, pQupConfig);
         SPIDEVICE_CHK_RESULT_BREAK(res);
		 
         res = SpiDevice_ConfigureQupSpiDevice_BamMode(pDev, pQupConfig);
         SPIDEVICE_CHK_RESULT_BREAK(res);
      }
   
      if(NULL == pQupConfig ){
         pQupConfig = pDev->pClntCfg;
      }
   
      res = SpiDevice_ValidateTransferParams(pDev, 
                   pQupConfig, pReadBuffer, 
                   uReadBufLen,pWriteBuffer, 
                   uWriteBufLen);
   
      if (res != SPIDEVICE_RES_SUCCESS)
      {
         SPIDEVICE_LOG(ERROR,
         "_SpiDevice_WriteRead_BamMode: SpiDevice_ValidateTransferParams Failure err 0x%x\n",
         (unsigned int)res);
         break;
      }
      
      res = SpiDevice_ConfigureQupSpiTransfer_BamMode(pDev, 
                   pQupConfig, pReadBuffer, 
                   uReadBufLen, pWriteBuffer, 
                   uWriteBufLen);
      
      SPIDEVICE_CHK_RESULT_BREAK(res);

      res = SpiDevice_SetRunState((uint8*)pDev->platInfo.qupVirtBlockAddr,
                                             HAL_QUPE_RUNSTATE_RUN);

      SPIDEVICE_CHK_RESULT_BREAK(res);

      if(pDev->bamObj)
      {
         res = pDev->bamObj->startBamTransfer(pDev->bamObj, 
                            &pDev->transferInfo,
                            (void *)pDev, 
                            eot);
   
         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
            "_SpiDevice_WriteRead_BamMode: startBamTransfer Failure err 0x%x\n",
            (unsigned int)res);
            break;
         } 
      }
   }while(do_while_constant);

   return res;
}


static int32
_SpiDevice_WriteRead
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                  *pReadBuffer,
   uint32                           uReadBufLen,
   SpiDataAddrType                  *pWriteBuffer,
   uint32                           uWriteBufLen
)
{
   int32 res = 0;
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;
   uint8 *readBuf = NULL; 
   uint8 *readBufPhys = NULL;
   uint32 uTotalReadLen = 0;
   uint8 *writeBuf = NULL;
   uint8 *writeBufPhys = NULL;
   uint32 uTotalWriteLen =0;

   SPIDEVICE_VALIDATE_WR_PARAMS(pDev, pQupConfig, pReadBuffer, uReadBufLen, pWriteBuffer, 
         uWriteBufLen);

   if ((pReadBuffer != NULL) && (pWriteBuffer != NULL))
   {
      readBuf = (uint8 *)pReadBuffer->virtualAddr; 
      readBufPhys = (uint8 *)pReadBuffer->physicalAddr;
      uTotalReadLen = uReadBufLen;
      writeBuf = (uint8 *)pWriteBuffer->virtualAddr;
      writeBufPhys = (uint8 *)pWriteBuffer->physicalAddr;
      uTotalWriteLen =uWriteBufLen;
   }
   else if ((pReadBuffer == NULL) && (pWriteBuffer != NULL))
   {
      writeBuf = (uint8 *)pWriteBuffer->virtualAddr;
      writeBufPhys = (uint8 *)pWriteBuffer->physicalAddr;
      uTotalWriteLen =uWriteBufLen;
   }
   else if ((pReadBuffer != NULL) && (pWriteBuffer == NULL))
   {
      readBuf = (uint8 *)pReadBuffer->virtualAddr; 
      readBufPhys = (uint8 *)pReadBuffer->physicalAddr;
      uTotalReadLen = uReadBufLen;
   }
   
   SpiDeviceSingleBufferTransferInit(&pDev->transferInfo, 
                    readBuf,readBufPhys, uTotalReadLen,
                    writeBuf,writeBufPhys,uTotalWriteLen);    
      
         res = _SpiDevice_WriteRead_BamMode(hDev, 
                         pQupConfig, pReadBuffer, 
                         uReadBufLen,pWriteBuffer, 
                         uWriteBufLen, TRUE);
   return res;
}


/** @brief Set the power state of the device.

    This Function sets the power state for the device.

    @param[in] hDev           Device handle.
    @param[in] ePowerState    the new power state

    @return          int32 .
  */
int32
SpiDevice_SetPowerState
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiDevice_PowerStates                   ePowerState
)
{
   SpiDevice_DeviceCtxt *pDev = (SpiDevice_DeviceCtxt *)hDev;

   if (NULL == hDev)
   {
	   SPIDEVICE_LOG(ERROR,"SpiDevice_SetPowerState: Null pointer passed \n");
	   return SPIDEVICE_ERR_INVALID_DEV_HANDLE;
   }

   return (int32)SpiDevicePlatCfg_SetPowerState(pDev->hDevTgt, ePowerState);
}

/** @brief Peform a bi-direction transfer

    Read a buffer from SPI device and/or Write a buffer of 
   data to a SPI device

    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pReadBuffer    Pointer where incoming data will
          be stored.
    @param[in] uReadBufLen    Number of bytes of valid data to be 
        stored in pReadbuffer
    @param[in] pWriteBuffer    Pointer where outgoing data is  
          stored.
    @param[in] uWriteBufLen    Number of bytes of valid data stored in 
        pWritebuffer
    @return          int32 .
  */
int32
SpiDevice_WriteRead
(
   SPIDEVICE_HANDLE                          hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pReadBuffer,
   uint32                                    uReadBufLen,
   SpiDataAddrType                           *pWriteBuffer,
   uint32                                    uWriteBufLen
)
{
   int32 res;
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;
   pDev->asyncCall.pCallbackFn = NULL;
   pDev->asyncCall.asyncResult.pArg        = NULL;

   if(SPIDEVICE_RES_SUCCESS != (res = _SpiDevice_WriteRead(hDev, pQupConfig, 
               pReadBuffer, uReadBufLen, pWriteBuffer, uWriteBufLen)))
   {
      SPIDEVICE_LOG(ERROR,
      "SpiDevice_WriteRead: Failure err 0x%x\n",
      (unsigned int)res);
      pDev->transferInfo.transferResult = res;
      return pDev->transferInfo.transferResult;
   }

   // Fixme: res is used only in the above log message. This causes warning
   // when logs are disabled.
   res = res;

   QUP_WaitForCompletion(pDev);
   return pDev->transferInfo.transferResult;
}

/** @brief Read a buffer from spi device.

    Read a buffer from spi device.
    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pReadBuffer    Pointer where incoming data will
          be stored.
    @param[in] uReadBufLen    Number of bytes of valid data to be 
        stored in pReadbuffer
    @return          int32 .
  */
int32
SpiDevice_Read
(
   SPIDEVICE_HANDLE                          hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pReadBuffer,
   uint32                                    uReadBufLen
) 
{
 return SpiDevice_WriteRead(hDev, pQupConfig, pReadBuffer, uReadBufLen, NULL, 0 );  
}

/** @brief Write a buffer to spi device.

    Write a buffer to spi device.
    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pWriteBuffer    Pointer where outgoing data is  
          stored.
    @param[in] uWriteBufLen    Number of bytes of valid data stored in 
        pWritebuffer
    @return          int32 .
  */

int32
SpiDevice_Write
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                  *pWriteBuffer,
   uint32                           uWriteBufLen
)
{
 return SpiDevice_WriteRead(hDev, pQupConfig, NULL, 0, pWriteBuffer, uWriteBufLen);  
}

/** @brief Configures Apps clock frequency

    Configures Apps clock frequency
    @param[in]  pDev            Pointer to device struct.
    @param[in]  pQupConfig      Pointer to the QUP config for data transfer.
          
    @return          int32 .
  */

static int32 SpiDevice_ConfigureAppsFrequency
(
   SpiDevice_DeviceCtxt                         *pdev,
   SpiDeviceInfoType                            *pQupConfig)
{
   int32 res = SPIDEVICE_ERR_CLOCK_SETTING_FAILED;
   uint32 max_frequency = pQupConfig->deviceParameters.u32MaxSlaveFrequencyHz;
   uint32 min_frequency = pQupConfig->deviceParameters.u32MinSlaveFrequencyHz;
   uint32 actual_frequency = 0;

   do
   {
      if (SPIDEVICE_RES_SUCCESS != SpiDevicePlatCfg_SetAppClkHz(pdev->hDevTgt, max_frequency, &actual_frequency))
      {
         SPIDEVICE_LOG(ERROR, "configure_frequency: IBUSPLAT_SetAppClkHz failed (0x%lx)", res);
         break;
      }   

      if ((min_frequency > 0) && (actual_frequency < min_frequency))
      {
         SPIDEVICE_LOG(ERROR, "failed to set frequency within specified parameters");
         break;
      }

      pQupConfig->deviceParameters.u32MaxSlaveFrequencyHz = actual_frequency;

      res = SPIDEVICE_RES_SUCCESS;
   } while(0);
   
   return res;
}


