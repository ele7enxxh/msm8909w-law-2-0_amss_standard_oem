/*
===========================================================================

FILE:   SpiDevice.c

DESCRIPTION:
    This file contains the implementation of the API for the QUPSPIDeviceQup 
    Interface: IQupSpiDeviceQup 

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/device/SpiDevice.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
12/07/15 dk     Changed the procedure in handling the IN/OUT interrupts and reading FIFO.
09/15/15 ddk    Added code review comments.
04/22/15 dpk    Making the pointer to NULL after freeing.
01/13/14 ms     Updated with common logging mechanism
07/14/12 ag     Added updates to remove windows warnings and some interrupt
                Prototype changes.
05/24/12 ddk    Added updates to support common qup hal.
05/22/12 ddk    Added updates to calculate deassert waits.
04/20/12 ddk    Added updates to enable logging.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
04/13/12 ddk    Fixed null pointer accesses in duplex transmissions.
03/29/12 ddk    Added deinit requriements for bam mode.
03/28/12 ddk    Added QUP physical address info for BAM cmd desc creation.
03/28/12 ddk    Fixed incorrect setting of SPI core mode.
03/27/12 ddk    Fixed some issues in driver related to Slave selection
                and BAM mode selection.
03/27/12 ag     Added SPI BAM mode updates and transfer requirements.
12/10/12 ag     Created.
===========================================================================
        Copyright c 2011-2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDevice.h"
#include "SpiDeviceTransfer.h"
#include "SpiDevicePlatSvc.h"
#include "SpiDeviceOsSvc.h"
#include "HALqupe.h"
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

typedef struct SpiDevice_RegInfo
{
   uint32                              uInFreeSlotReqCnt;
   uint32                              uOutFreeSlotReqCnt;
   uint32                              uQupErrors;
   uint32                              uOperational;
   uint32                              uInFreeSlotServicedCnt;
   uint32                              uOutFreeSlotServicedCnt;
   uint32                              uCurOutTransfers;
   uint32                              uCurInTransfers;
   boolean                             uMaxInputDone;
   boolean                             uMaxOutputDone;
} SpiDevice_RegInfo;


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
   IBusPlatInfoType                    platInfo;
   HAL_qupe_HwInfoType                 devHwInfo;
   char                                *pHalVer;
   SpiDevice_InitState                devInitState;
   SpiDevice_RegInfo                  qupInfo;
   IBUSOS_EVENT_HANDLE                 hQupEvt;
   IBUSPLATCFG_HANDLE                  hDevTgt;
   SpiDeviceInfoType                *pClntCfg;
   SpiDevice_PowerStates               devPowerState;
   SpiDevice_AsyncCallData         asyncCall;
   SpiDeviceTransferType         transferInfo;
   SpiDeviceBamType           *bamObj;
   uint32								uPollXferComplete;
} SpiDevice_DeviceCtxt;

static uint32 QUP_ServiceQUP(SpiDevice_DeviceCtxt *pDev);
static uint32 QUP_PerformInputOutput(SpiDevice_DeviceCtxt *pDev);
static int32 SpiDevice_SetRunState( uint8 *pQupAddr, const HAL_qupe_RunStateType cRunState);

/** @brief This is a hook for the thread servicing the qup.

    This Function is called by the framework to handle the data
    transfer in response to qup irq events.

    @param[in] pdata    void pointer to data.

    @return             Nothing.
  */
uint32 SpiDevice_IstHook(void *pdata)
{
   if (NULL == pdata) {
      return 0;
   }
   return (QUP_PerformInputOutput((SpiDevice_DeviceCtxt *)pdata));
}
/** @brief This is a hook for the ISR servicing the qup.

    This Function is called by the framework to handle the irq
    events triggered by the core. This function does not do
    data transfer. The IST thread handles data transfer.

    @param[in] pdata    void pointer to data.

    @return             Nothing.
  */
uint32  SpiDevice_IsrHook(void *pdata)
{
   if (NULL == pdata) {
      return 0;
   }
   return (QUP_ServiceQUP((SpiDevice_DeviceCtxt*)pdata));
}
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
      IBUSOS_BusyWait(QUP_BUSY_WAIT_STEP_US);
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
	IBUSOS_Result res;

	HAL_qupe_Init((uint8*)pDev->platInfo.qupVirtBlockAddr,
		&pDev->pHalVer);
    res = (IBUSOS_Result) SpiDevice_WaitForRunStateValid((uint8*)pDev->platInfo.qupVirtBlockAddr);

	if (SPIDEVICE_RES_SUCCESS != res) 
	{
		SPIDEVICE_LOG(ERROR,
		   "SpiDevice_InitHw: Invalid state after reset 0x%x\n",
		    (unsigned int)res);
		return (int32)res; 
	}

	HAL_qupe_GetHwInfo(
		(uint8*)pDev->platInfo.qupVirtBlockAddr, 
		&pDev->devHwInfo);

	HAL_qupe_SetSelectedMinicore((uint8*)pDev->platInfo.qupVirtBlockAddr,
		HAL_QUPE_MINI_CORE_SPI); 

	if (pDev->platInfo.bInterruptBased) 
	{
		res = IBUSOS_RegisterISTHooks(pDev->uQupCoreNum,
			SpiDevice_IsrHook,
			(void*) pDev,
			SpiDevice_IstHook,
			(void*) pDev);

		if (SPIDEVICE_RES_SUCCESS != res) 
		{
			SPIDEVICE_LOG(ERROR,
				"SpiDevice_InitHw:IBUSOS_RegisterISTHooks Failure err 0x%x\n",
				(unsigned int)res);
			return (int32)res; 
		}
	}

	if (pDev->platInfo.bTcsrInterruptRequired) 
	{
		HAL_qupe_EnableTcsrInterrupt((uint8*)pDev->platInfo.tcsrVirtBlockAddr, 
				pDev->platInfo.uTcsrInterruptBitMask,
				pDev->platInfo.uTcsrInterruptBitShift);
	}

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
   

   res = (int32)IBUSOS_Malloc((void **)&pDev,
                                         sizeof(SpiDevice_DeviceCtxt));
   
   if (SPIDEVICE_RES_SUCCESS != res) 
   {
      SPIDEVICE_LOG(ERROR,"SpiDevice_Init: Malloc failure\n");
      return res;
   }
   IBUSOS_Memset((void *)pDev, 0, sizeof(SpiDevice_DeviceCtxt));
   pDev->devInitState = SPIDEVICE_INITSTATE_UNINITIALIZED;

   do {
      res = (int32)IBUSPLATCFG_InitTarget(uDevOsId, &pDev->hDevTgt);
      
      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "SpiDevice_Init:IBUSPLATCFG_InitTarget Failure err 0x%x\n",
         (unsigned int)res);
         break;
      }
      pDev->devInitState |= SPIDEVICE_INITSTATE_TARGETINIT_DONE;

      res = (int32)IBUSPLATCFG_GetPlatformInfo(pDev->hDevTgt, &pDev->platInfo);

      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "IBUSPLATCFG_InitTarget:IBUSPLATCFG_GetPlatformInfo Failure err 0x%x\n",
         (unsigned int)res);
         break;
      }
     pDev->uQupCoreNum = pDev->platInfo.uQupCoreNum;
     pDev->bamObj = pDev->platInfo.spiBam;

      if (pDev->platInfo.bInterruptBased)
      {
         res = (int32)IBUSOS_CreateEvent(&pDev->hQupEvt);
         if (SPIDEVICE_RES_SUCCESS != res)
         {
            break;
         }
      }
      
      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "IBUSPLATCFG_InitTarget: IBUSOS_CreateEvent Failure err 0x%x\n",
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
         /*Turn off the clocks after HW initialization*/
         /*TODO - Pls add this back*/
         IBUSPLATCFG_SetPowerState(pDev->hDevTgt, SPIDEVICE_POWER_STATE_0);
   } while(do_while_constant);

   if (SPIDEVICE_RES_SUCCESS != res) 
   {  
      return_value = res;

      /*  in case of error undo initialization */
      if (SPIDEVICE_INITSTATE_TARGETINIT_DONE & pDev->devInitState) 
      {
         res = IBUSPLATCFG_DeInitTarget(pDev->hDevTgt);

         if (SPIDEVICE_RES_SUCCESS != res) 
         {
            SPIDEVICE_LOG(ERROR,
            "IBUSPLATCFG_InitTarget:IBUSPLATCFG_DeInitTarget Failure err 0x%x\n",
             (unsigned int)res);
         }
      }
      res =  IBUSOS_Free(pDev);
      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "IBUSPLATCFG_InitTarget:IBUSOS_Free Failure err 0x%x\n",
          (unsigned int)res);
      }

      pDev = NULL;
      *phDev = pDev;
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
         closeHRes = (int32)IBUSOS_CloseHandle(pDev->hQupEvt);
         if (SPIDEVICE_RES_SUCCESS != closeHRes) 
         {
            SPIDEVICE_LOG(ERROR,
            "SpiDevice_DeInit:IBUSOS_CloseHandle Failure err 0x%x\n",
            (unsigned int)closeHRes);
         }
      }
   }

   if (SPIDEVICE_INITSTATE_TARGETINIT_DONE & pDev->devInitState)
   {
      tgtDeinitRes = (int32) IBUSPLATCFG_DeInitTarget(pDev->hDevTgt);
   }          

   if (SPIDEVICE_RES_SUCCESS != tgtDeinitRes) 
   {
      SPIDEVICE_LOG(ERROR,
      "SpiDevice_DeInit:IBUSPLATCFG_DeInitTarget Failure err 0x%x\n",
       (unsigned int)tgtDeinitRes);
   }
   
   res = (int32)IBUSOS_Free(pDev);

   if (SPIDEVICE_RES_SUCCESS != res) 
   {
      SPIDEVICE_LOG(ERROR,
      "SpiDevice_DeInit:IBUSOS_Free Failure err 0x%x\n",
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
/**
    This function services the queue.
    It checks the set operational flags and clears the service flags.

    @param[in, out] pInfo  Pointer to QupInfo structure.
    @return         Nothing.
  */

static uint32 QUP_ServiceQUP
(
   SpiDevice_DeviceCtxt *pDev
)
{
   SpiDevice_RegInfo *pQupInfo;
   uint32 uOperationalFlags;
   uint32 uSetServiceFlags = 0;

   pQupInfo = &pDev->qupInfo;

   /* Check operating and error status.                                      */
   /* According to the HW designer both the QUP error flags and the minicore
      status register need to be read and then the interrupts can be cleared.
      If status registers are not read then the interrupt will remain set.
      I have seen this happen for the interrupts asserted by the minicore.   */
   pQupInfo->uQupErrors   = HAL_qupe_GetErrStatusFlags((uint8*)pDev->platInfo.qupVirtBlockAddr);
   uOperationalFlags      = HAL_qupe_GetOpStatusFlags((uint8*)pDev->platInfo.qupVirtBlockAddr);
   pQupInfo->uOperational = uOperationalFlags;

   /* Check for errors.                                                      */
   if (pQupInfo->uQupErrors & HAL_QUPE_ERRSTATE_FIELD){
      /* HAL_qupe_LogRegs(pDev->uQupCoreNum);
      TODO: Register logging to be removed from HAL */
      HAL_qupe_Reset((uint8*)pDev->platInfo.qupVirtBlockAddr);
      SpiDevice_WaitForRunStateValid((uint8*)pDev->platInfo.qupVirtBlockAddr);
	   return (pQupInfo->uOutFreeSlotReqCnt + pQupInfo->uInFreeSlotReqCnt);
   }

   /* If output service then fill the output queue with another block.       */
   if ((uOperationalFlags & HAL_QUPE_OPSTATE_OUTPUT_SERVICE))  {
      if  (uOperationalFlags & HAL_QUPE_OPSTATE_OUT_BLOCK_WRITE_REQ)  {

            pQupInfo->uOutFreeSlotReqCnt += pDev->devHwInfo.uNumOutputBlockSlots;
      }

      if(uOperationalFlags & HAL_QUPE_OPSTATE_MAX_OUTPUT_DONE){
         pQupInfo->uMaxOutputDone = TRUE;
      }
      uSetServiceFlags |= HAL_QUPE_OPSTATE_OUTPUT_SERVICE;
   }
   /* If input service then get the blocks from queue.                       */
   if ((uOperationalFlags & HAL_QUPE_OPSTATE_INPUT_SERVICE))  {
      if  (uOperationalFlags & HAL_QUPE_OPSTATE_IN_BLOCK_READ_REQ)  {

            pQupInfo->uInFreeSlotReqCnt += pDev->devHwInfo.uNumInputBlockSlots;
      }

      if(uOperationalFlags & HAL_QUPE_OPSTATE_MAX_INPUT_DONE){
         pQupInfo->uMaxInputDone = TRUE;
      }
      uSetServiceFlags |= HAL_QUPE_OPSTATE_INPUT_SERVICE;
   }
   HAL_qupe_ClearOpStatusFlags((uint8*)pDev->platInfo.qupVirtBlockAddr, uSetServiceFlags);

   return (pQupInfo->uOutFreeSlotReqCnt + pQupInfo->uInFreeSlotReqCnt);
}

/**
    This function services the queue.
    It checks the set operational flags and clears the service flags.

    @param[in, out] pInfo  Pointer to QupInfo structure.
    @return         Nothing.
  */
static uint32 QUP_PerformOutput
(
   SpiDevice_DeviceCtxt               *pDev 
)
{ 
   SpiDevice_RegInfo            *pQupInfo = &(pDev->qupInfo);
   SpiDeviceTransferType       *transferInfo = &(pDev->transferInfo);
   SpiDeviceTransferDataResult  res;
   uint32                  writeData;
   
   /* If to be qued and space continue state machine */

   /* As we loop through the transfers we update read tr cnt 
   ** If output is done update the state of the output sequence 
   */
   while(pQupInfo->uOutFreeSlotReqCnt > pQupInfo->uOutFreeSlotServicedCnt)
   {
	   if(SPIDEVICE_TRANSFER_GET_DATA_NONE != 
			   (res = transferInfo->getNextWord(transferInfo, &writeData))){
        /*  push slot into output fifo */
        HAL_qupe_PutWordIntoOutFifo((uint8*)pDev->platInfo.qupVirtBlockAddr, writeData);
        pQupInfo->uOutFreeSlotServicedCnt++;
		   if(SPIDEVICE_TRANSFER_GET_DATA_DONE == res){
			   pDev->transferInfo.bWriteBufferEmpty = TRUE;
		   }

	   } else if(pQupInfo->uOutFreeSlotReqCnt - pQupInfo->uOutFreeSlotServicedCnt 
			   < pDev->devHwInfo.uNumOutputBlockSlots){
		  /*Write zeros till we get to the block boundary.*/
		  writeData = 0;
		  HAL_qupe_PutWordIntoOutFifo((uint8*)pDev->platInfo.qupVirtBlockAddr, writeData);
		  pQupInfo->uOutFreeSlotServicedCnt++;
	  }
	  else{
		  /*We should Error Fatal here or abort tranfer because we're provided with insufficient buffer space*/
     }
   }
   return pQupInfo->uOutFreeSlotServicedCnt;
}

/**
    This function services the queue.
    It checks the set operational flags and clears the service flags.

    @param[in, out] pInfo  Pointer to QupInfo structure.
    @return         Nothing.
  */
static uint32 QUP_PerformInput
(
   SpiDevice_DeviceCtxt               *pDev 
)
{ 
   int32 res;
   SpiDevice_RegInfo              *pQupInfo = &(pDev->qupInfo);
   SpiDeviceTransferType      *transferInfo = &(pDev->transferInfo);
   uint32                          readData;
   

   /* If transfer to be queued Check if there is space in the output fifo 
   ** If to be qued and space continue state machine 
   ** As we loop through the transfers we update read tr cnt
   ** If output is done update the state of the output sequence 
   */
   while(pQupInfo->uInFreeSlotReqCnt > pQupInfo->uInFreeSlotServicedCnt)
   {
      /*  get slot from fifo */
      HAL_qupe_GetWordFromInFifo(
                           (uint8*)pDev->platInfo.qupVirtBlockAddr, 
                           &readData); 
	  /*Even if there is */
		if(SPIDEVICE_TRANSFER_SET_DATA_NONE != 
				(res = transferInfo->setNextWord(transferInfo, &readData))){
		pQupInfo->uInFreeSlotServicedCnt++;
			if(SPIDEVICE_TRANSFER_SET_DATA_DONE == res){
				pDev->transferInfo.bReadBufferFull = TRUE;
			}
		} else if (pQupInfo->uInFreeSlotReqCnt - pQupInfo->uInFreeSlotServicedCnt 
				< pDev->devHwInfo.uNumInputBlockSlots ){
      pQupInfo->uInFreeSlotServicedCnt++;
	  } else {
		  /*We should Error Fatal here or abort tranfer because we're provided with insufficient buffer space*/
	  }
   }
   return pQupInfo->uInFreeSlotServicedCnt;
}

static void QUP_Completion
(
   SpiDevice_DeviceCtxt               *pDev
)
{
   int32  res;
   // if this is an asynchronous transfer then set results and
   // dispatch callback
   // else just signal the waiting thread
   if (NULL != pDev->asyncCall.pCallbackFn) { // blocking transfer
      pDev->asyncCall.asyncResult.nTransferStatus = pDev->transferInfo.transferResult;
      pDev->asyncCall.pCallbackFn(&pDev->asyncCall.asyncResult);
   }
   else {
		if (pDev->platInfo.bInterruptBased) /* Interrupt based wait.          */
		{
			res = (int32)IBUSOS_SetEvent(pDev->hQupEvt);

			if (SPIDEVICE_RES_SUCCESS != res) 
			{
				SPIDEVICE_LOG(ERROR,
						"QUP_Completion:IBUSOS_SetEvent Failure err 0x%x\n",
						(unsigned int)res);
			}
		} 
		else /* Set Polling mode complete flag*/
		{
			pDev->uPollXferComplete = 1;
		}
	}
}

static void QUP_Bam_Completion
(
   SpiDeviceBamResult *pResult
)
{
   SpiDevice_DeviceCtxt               *pDev;

   if(pResult){
      pDev = (SpiDevice_DeviceCtxt *)pResult->pUser; 
      pDev->transferInfo.transferResult = pResult->result;
      QUP_Completion(pDev);
   }
}
/**
    This function services the queue.
    It checks the set operational flags and clears the service flags.

    @param[in, out] pInfo  Pointer to QupInfo structure.
    @return         Nothing.
  */
static uint32 QUP_PerformInputOutput
(
   SpiDevice_DeviceCtxt               *pDev 
)
{
   SpiDeviceTransferType      *transferInfo = &(pDev->transferInfo);
   SpiDevice_RegInfo              *pQupInfo = &(pDev->qupInfo);
   int32  res;
   uint32 TxRxCount = 0;

   TxRxCount = QUP_PerformOutput(pDev);
   TxRxCount += QUP_PerformInput(pDev);

   if((pQupInfo->uMaxOutputDone)&& (transferInfo->bWriteBufferEmpty))
   {
      transferInfo->bWriteTransferDone = TRUE;
   }

   if((pQupInfo->uMaxInputDone) && (transferInfo->bReadBufferFull))
   {
      transferInfo->bReadTransferDone = TRUE;
   }

   if((transferInfo->bWriteTransferDone) && (transferInfo->bReadTransferDone)){
      res = SpiDevice_SetRunState((uint8*)pDev->platInfo.qupVirtBlockAddr, 
                                    HAL_QUPE_RUNSTATE_PAUSE);

      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "QUP_PerformInputOutput:SpiDevice_SetRunState Failure err 0x%x\n",
          (unsigned int)res);
      }

      res = SpiDevice_SetRunState((uint8*)pDev->platInfo.qupVirtBlockAddr, 
                           HAL_QUPE_RUNSTATE_RESET);

      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "QUP_PerformInputOutput:SpiDevice_SetRunState Failure err 0x%x\n",
         (unsigned int)res);
      }

      transferInfo->transferResult = SPIDEVICE_RES_SUCCESS;
	  pQupInfo->uOutFreeSlotReqCnt = pQupInfo->uOutFreeSlotServicedCnt = 0;
	  pQupInfo->uInFreeSlotReqCnt = pQupInfo->uInFreeSlotServicedCnt = 0;
     pQupInfo->uMaxInputDone = pQupInfo->uMaxOutputDone = FALSE;
		QUP_Completion(pDev);
   }
   return TxRxCount;
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
      IBUSOS_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
   }
   if(0 == uWaitTimeUs){
      return SPIDEVICE_ERR_INVALID_RUNSTATE;
   }

   HAL_qupe_SetRunState(pQupAddr, cRunState);
   while((!HAL_qupe_RunStateIsValid(pQupAddr)) && (0 != uWaitTimeUs)){
      IBUSOS_BusyWait(QUP_BUSY_WAIT_STEP_US);
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
   boolean                         bTimeout      = FALSE;
   IBUSOS_Result                   osRes;
   int32 res;
   
   if (pDev->platInfo.bInterruptBased) /* Interrupt based wait.          */
   {
      osRes = IBUSOS_Wait(pDev->hQupEvt, 1000);
      if (IBUSOS_RESULT_ERROR_TIMEOUT == osRes) {
         SPIDEVICE_LOG(ERROR,"QUP_WaitForCompletion:Timeout Failure\n");
         bTimeout = TRUE;
      }
      else if (SPIDEVICE_RES_SUCCESS != osRes) {
         pDev->transferInfo.transferResult = osRes;
      }else{
         pDev->transferInfo.transferResult = SPIDEVICE_RES_SUCCESS;
     }
      /* TODO: Add logging. */
   } 
   else /*POLLING MODE*/
   {
	   uint32 uTmpTimeout = QUP_BUSY_WAIT_POLL_MAX_US;
	   pDev->uPollXferComplete = 0;
	   /* loop until either an event happens or timeout                 */
	   do {
		   SpiDevice_IsrHook((void *)pDev);
		   SpiDevice_IstHook((void *)pDev);
		   IBUSOS_BusyWait(QUP_BUSY_WAIT_POLL_STEP_US);
		   if (uTmpTimeout > QUP_BUSY_WAIT_STEP_US)
		   {
			   uTmpTimeout -= QUP_BUSY_WAIT_STEP_US;
		   }
		   else
		   {
			   uTmpTimeout = 0;
		   }
	   }while((!pDev->uPollXferComplete) &&(uTmpTimeout > 0));

	   if(pDev->uPollXferComplete)
	   {
		   pDev->transferInfo.transferResult = SPIDEVICE_RES_SUCCESS;
	   }
	   else
	   {
		   bTimeout = TRUE;
	   }
   }

   if (bTimeout) { /*  set error code */
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
      

      pDev->transferInfo.transferResult = SPIDEVICE_RES_ERROR_CLS_QUP_DEV_TIMEOUT |
                  HAL_qupe_Pack28_OperationalState((uint8*)pDev->platInfo.qupVirtBlockAddr);
      
      SPIDEVICE_LOG(ERROR,
      "QUP_WaitForCompletion:transferResult  0x%x\n",
      (unsigned int) pDev->transferInfo.transferResult);
      
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
SpiDevice_ConfigureQupSpiDevice_BlockMode
(
   SpiDevice_DeviceCtxt                    *pDev, 
   SpiDeviceInfoType                         *pQupConfig
) 
{
   int32 res;
   
   HAL_QUPE_SPI_DeviceParametersType mHalDeviceParameters;
   uint32 u32MaxSlaveFrequency, u32MinSlaveFrequency, u32ActualClkSpeed = 0, 
      u32DeassertionTime, u32DeassertionClockTicks;
   SpiDeviceParametersType *pDeviceParameters;   

   if(SPI_CORE_MODE_SLAVE == pQupConfig->deviceBoardInfo.eCoreMode)
   {
     HAL_qupe_Spi_SetCoreMode((uint8*)pDev->platInfo.qupVirtBlockAddr, 
                        HAL_QUPE_SPI_SLAVE);
   }
   else
   {
      HAL_qupe_Spi_SetCoreMode((uint8*)pDev->platInfo.qupVirtBlockAddr, 
                       HAL_QUPE_SPI_MASTER);
   }

   pDeviceParameters = &(pQupConfig->deviceParameters);
   
   u32MaxSlaveFrequency = pDeviceParameters->u32MaxSlaveFrequencyHz;
   u32MinSlaveFrequency = pDeviceParameters->u32MinSlaveFrequencyHz;
   u32DeassertionTime = pDeviceParameters->u32DeassertionTime;

   res = IBUSPLATCFG_SetAppClkHz(pDev->hDevTgt,
                                 u32MaxSlaveFrequency,
                                 &u32ActualClkSpeed );
   
   if (res != SPIDEVICE_RES_SUCCESS)
   {
      SPIDEVICE_LOG(ERROR,
      "SpiDevice_ConfigureQupSpiDevice_BlockMode:"
      "IBUSPLATCFG_SetAppClkHz Failure err 0x%x\n",
      (unsigned int)res);
      return res ;
   }
   
   
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

   return 0;
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

   res = IBUSPLATCFG_SetAppClkHz(pDev->hDevTgt,
                                 u32MaxSlaveFrequency,
                                 &u32ActualClkSpeed );
   
   if (res != SPIDEVICE_RES_SUCCESS)
   {
      SPIDEVICE_LOG(ERROR,
      "SpiDevice_ConfigureQupSpiDevice_BamMode:"
      "IBUSPLATCFG_SetAppClkHz Failure err 0x%x\n",
      (unsigned int)res);
      return res ;
   }
   
   
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

   if(pDev->bamObj){
      pDev->bamObj->setQupDeviceParms(pDev->bamObj, &mHalDeviceParameters);
   }
  
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
   SPIDEVICE_UNREFERENCED_PARAMETER(pQupConfig);
   SPIDEVICE_UNREFERENCED_PARAMETER(pDev);

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
   SPIDEVICE_UNREFERENCED_PARAMETER(pQupConfig);
   SPIDEVICE_UNREFERENCED_PARAMETER(pDev);
   SPIDEVICE_UNREFERENCED_PARAMETER(pReadBuffer);
   SPIDEVICE_UNREFERENCED_PARAMETER(uReadBufLen);
   SPIDEVICE_UNREFERENCED_PARAMETER(pWriteBuffer);
   SPIDEVICE_UNREFERENCED_PARAMETER(uWriteBufLen);

   return SPIDEVICE_RES_SUCCESS;
}
//FIXME: Put function header.
int32
SpiDevice_ConfigureQupSpiTransfer_BlockMode
(
   SpiDevice_DeviceCtxt                    *pDev,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                  *pReadBuffer,
   uint32                           uReadBufLen,
   SpiDataAddrType                  *pWriteBuffer,
   uint32                           uWriteBufLen
)
{
   HAL_QUPE_SPI_TransferParametersType mHalTransferParameters;

   /*Setup output transfer params for HAL and QupTransferInfo*/
   mHalTransferParameters.nNumOutputTransfers = 0;
   pDev->transferInfo.bWriteTransferDone = TRUE;
   pDev->transferInfo.bWriteBufferEmpty = TRUE;

   if((pWriteBuffer) && (pWriteBuffer->virtualAddr) && (uWriteBufLen)) {
      mHalTransferParameters.nNumOutputTransfers = 
         SpiDevice_GetNumQUPTransfers(uWriteBufLen, 
			    (uint8)pQupConfig->transferParameters.nNumBits);
      pDev->transferInfo.bWriteTransferDone = FALSE;
      pDev->transferInfo.bWriteBufferEmpty = FALSE;
   }

   /*Setup input transfer params for HAL and QupTransferInfo*/
   mHalTransferParameters.nNumInputTransfers = 0;
   pDev->transferInfo.bReadTransferDone = TRUE;
   pDev->transferInfo.bReadBufferFull = TRUE;

   if((pReadBuffer) && (pReadBuffer->virtualAddr) && (uReadBufLen)){
      mHalTransferParameters.nNumInputTransfers = 
         SpiDevice_GetNumQUPTransfers(uReadBufLen, 
			     (uint8)pQupConfig->transferParameters.nNumBits);
      pDev->transferInfo.bReadTransferDone = FALSE;
      pDev->transferInfo.bReadBufferFull = FALSE;
   }

   /**/
   SpiDevice_GetHalTransferParameters(pQupConfig, &mHalTransferParameters);
   HAL_qupe_Spi_SetTransferParameters(
       (uint8*)pDev->platInfo.qupVirtBlockAddr,
       &mHalTransferParameters);

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

   if(pDev->bamObj){
      pDev->bamObj->setQupTransferParms(pDev->bamObj, &mHalTransferParameters);
   }
     
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
   //FIXME: See if we need this
   //SPIDEVICE_VALIDATE_WR_PARMS(pDev, pQupConfig, pReadBuffer, uReadBufLen, pWriteBuffer, uWriteBufLen);

   do
   {
      if(0 == (pDev->devInitState & SPIDEVICE_INITSTATE_HARDWAREINIT_DONE))
      {
         res = SpiDevice_InitHw(pDev);
   
         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
            "_SpiDevice_WriteRead_BamMode: SpiDevice_InitHw Failure err 0x%x\n",
            (unsigned int)res);
            break;
         }
      }
   
      if (pDev->platInfo.bInterruptBased)
      {
         res  = IBUSOS_ClearEvent( pDev->hQupEvt);

         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
                  "_SpiDevice_WriteRead_BamMode: IBUSOS_ClearEvent Failure err 0x%x\n",
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
   
      if(pDev->bamObj)
      {
         pDev->bamObj->setBamTransferCallback(pDev->bamObj, QUP_Bam_Completion);
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
_SpiDevice_WriteRead_BlockMode
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                  *pReadBuffer,
   uint32                           uReadBufLen,
   SpiDataAddrType                  *pWriteBuffer,
   uint32                           uWriteBufLen
)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;
   uint32 do_while_constant = 0;
   //FIXME: See if we need this
   //SPIDEVICE_VALIDATE_WR_PARMS(pDev, pQupConfig, pReadBuffer, uReadBufLen, pWriteBuffer, uWriteBufLen);
   do
   {
      if(0 == (pDev->devInitState & SPIDEVICE_INITSTATE_HARDWAREINIT_DONE))
      {
         res = SpiDevice_InitHw(pDev);

         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
            "_SpiDevice_WriteRead_BlockMode: SpiDevice_InitHw Failure err 0x%x\n",
            (unsigned int)res);
            break;
         }
      }
   
      if (pDev->platInfo.bInterruptBased)
      {
         res  = IBUSOS_ClearEvent( pDev->hQupEvt);

         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
                  "_SpiDevice_WriteRead_BlockMode: IBUSOS_ClearEvent Failure err 0x%x\n",
                  (unsigned int)res);
            break;
         }
      }
   
	  res = SpiDevice_SetRunState((uint8*)pDev->platInfo.qupVirtBlockAddr, 
                           HAL_QUPE_RUNSTATE_RESET);
	  if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "QUP_PerformInputOutput:SpiDevice_SetRunState Failure err 0x%x\n",
         (unsigned int)res);
      }
   
      if(pQupConfig)
      {
         res = SpiDevice_ValidateDeviceParams(pDev, pQupConfig);

         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
            "_SpiDevice_WriteRead_BlockMode:" 
            "SpiDevice_ValidateDeviceParams Failure err 0x%x\n",
            (unsigned int)res);
            break;
         }
   
         res = SpiDevice_ConfigureQupSpiDevice_BlockMode(pDev, pQupConfig);
         SPIDEVICE_CHK_RESULT_BREAK(res);
      }
   
      if(NULL == pQupConfig )
      {
         pQupConfig = pDev->pClntCfg;
      }
   
      res = SpiDevice_ValidateTransferParams(pDev, 
                    pQupConfig, pReadBuffer, 
                    uReadBufLen,pWriteBuffer, 
                    uWriteBufLen);

      if (res != SPIDEVICE_RES_SUCCESS)
      {
         SPIDEVICE_LOG(ERROR,
         "_SpiDevice_WriteRead_BlockMode:"
         "SpiDevice_ValidateTransferParams Failure err 0x%x\n",
         (unsigned int)res);
         break;
      }
      
      res = SpiDevice_ConfigureQupSpiTransfer_BlockMode(pDev, 
                    pQupConfig, pReadBuffer, 
                    uReadBufLen, pWriteBuffer, 
                    uWriteBufLen);

      SPIDEVICE_CHK_RESULT_BREAK(res);
      
      res = SpiDevice_SetRunState((uint8*)pDev->platInfo.qupVirtBlockAddr,
                            HAL_QUPE_RUNSTATE_RUN);

      if (SPIDEVICE_RES_SUCCESS != res) 
      {
         SPIDEVICE_LOG(ERROR,
         "_SpiDevice_WriteRead_BlockMode:"
         "SpiDevice_SetRunState Failure err 0x%x\n",
          (unsigned int)res);
         break;
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
   int32 res;
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
      
   if((pDev->bamObj) && (pDev->platInfo.bBamSupported) && 
      (SPIDEVICE_BAM_TRANSFER_MIN_LEN < 
      (SPIDEVICE_MAX(uReadBufLen, uWriteBufLen))))
   {
         res = _SpiDevice_WriteRead_BamMode(hDev, 
                         pQupConfig, pReadBuffer, 
                         uReadBufLen,pWriteBuffer, 
                         uWriteBufLen, TRUE);
   } else 
   {
      res = _SpiDevice_WriteRead_BlockMode(hDev, pQupConfig, 
            pReadBuffer, uReadBufLen, pWriteBuffer, uWriteBufLen);
   }

   return res;
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
   }


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

/** @brief Peform a bi-direction transfer

    Schedules a read/write/read-write(bi-directional) transfer.
    Once the transfer is complete or an error occurs
    the callback will be called.

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
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.
    @return          int32 .
  */
void
SpiDevice_AsyncWriteRead
(
   SPIDEVICE_HANDLE                          hDev      ,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pReadBuffer,
   uint32                                    uReadBufLen,
   SpiDataAddrType                           *pWriteBuffer,
   uint32                                    uWriteBufLen,
   SPI_DEV_ASYNC_CB                          pCallbackFn,
   void                                      *pArg            
)
{
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;
   int32 res;

   res = SPIDEVICE_VALIDATE_ASYNC_CALLBACK_PARAMS(pCallbackFn, pArg);
   SPIDEVICE_CHK_RESULT_VOID(res);

   pDev->asyncCall.pCallbackFn = pCallbackFn;
   pDev->asyncCall.asyncResult.pArg        = pArg;

   res =  _SpiDevice_WriteRead(hDev, pQupConfig, pReadBuffer, 
               uReadBufLen, pWriteBuffer, uWriteBufLen);
   
   if(SPIDEVICE_RES_SUCCESS == res){
      pDev->transferInfo.transferResult = res;
   }
   else
   {
      SPIDEVICE_LOG(ERROR,
      "SpiDevice_AsyncWriteRead: Failure err 0x%x\n",
      (unsigned int)res);
   }
   return;
}


/** @brief Schedules a buffer read from spi device.

    Schedules a buffer read from spi device.
    Once the read is complete or an error occurs
    the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pReadBuffer    Pointer where incoming data will
          be stored.
    @param[in] uReadBufLen    Number of bytes of valid data to be 
        stored in pReadbuffer
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   Nothing.
  */
void
SpiDevice_AsyncRead
(
   SPIDEVICE_HANDLE                          hDev,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pReadBuffer,
   uint32                                    uReadBufLen,
   SPI_DEV_ASYNC_CB                          pCallbackFn,
   void                                      *pArg            
) 
{
	SpiDevice_AsyncWriteRead(hDev, pQupConfig, pReadBuffer,
         uReadBufLen, NULL, 0, pCallbackFn, pArg);  
}

/** @brief Schedules a buffer write to spi device.

    Schedules a buffer write to spi device. Once the write is
    complete or an error occurs the callback will be called.

    @param[in] *pQupConfig   Pointer to the QUP config for data transfer.
    @param[in] pWriteBuffer    Pointer where outgoing data is  
          stored.
    @param[in] uWriteBufLen    Number of bytes of valid data stored in 
        pWritebuffer
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   Nothing.
  */
void
SpiDevice_AsyncWrite
(
   SPIDEVICE_HANDLE                          hDev,
   SpiDeviceInfoType                         *pQupConfig,
   SpiDataAddrType                           *pWriteBuffer,
   uint32                                    uWriteBufLen,
   SPI_DEV_ASYNC_CB                          pCallbackFn,
   void                                      *pArg            
)
{
 SpiDevice_AsyncWriteRead(hDev, pQupConfig, NULL, 0, 
       pWriteBuffer, uWriteBufLen, pCallbackFn, pArg);  
}


/** @brief Cancels ongoing transfer.

    Cancels ongoing transfer by reseting the core.

    @param[in] hDev           Device handle.

    @return                   Nothing.
  */
void
SpiDevice_CancelTransfer
(
   SPIDEVICE_HANDLE                        hDev
)
{
   SpiDevice_DeviceCtxt *pDev = (SpiDevice_DeviceCtxt *)hDev;
   SpiDeviceTransferType      *transferInfo = &(pDev->transferInfo);
   SpiDevice_RegInfo              *pQupInfo = &(pDev->qupInfo);

//Fixme: This more than just a HW reset.
   HAL_qupe_Reset((uint8*)pDev->platInfo.qupVirtBlockAddr);
   SpiDevice_WaitForRunStateValid((uint8*)pDev->platInfo.qupVirtBlockAddr);
   transferInfo->transferResult = SPIDEVICE_RES_ERROR_CLS_QUP_DEV_TIMEOUT;
   pQupInfo->uOutFreeSlotReqCnt = pQupInfo->uOutFreeSlotServicedCnt = 0;
   pQupInfo->uInFreeSlotReqCnt = pQupInfo->uInFreeSlotServicedCnt = 0;
   pQupInfo->uMaxInputDone = pQupInfo->uMaxOutputDone = FALSE;
   QUP_Completion(pDev);
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
   return (int32)IBUSPLATCFG_SetPowerState(pDev->hDevTgt, ePowerState);
}

/** @brief Gets the power state of the device.

    This Function gets the power state of the device.

    @param[in] hDev           Device handle.
    @param[in] pePowerState   Pointer to power state.

    @return          int32 .
  */
int32
SpiDevice_GetPowerState
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiDevice_PowerStates                   *pePowerState
)
{
   SpiDevice_DeviceCtxt *pDev = (SpiDevice_DeviceCtxt *)hDev;
   return IBUSPLATCFG_GetPowerState(pDev->hDevTgt, (uint32)pePowerState);
}

int32
_SpiDevice_BatchTransferBamMode
(
   SPIDEVICE_HANDLE                     hDev,
   SpiLinkedTransferType                *pTransfer,
   uint32                        uNumTransferVecs,
   SpiDeviceInfoType                    *pQupConfig,
   uint32                        *puNumCompleted
)
{
   int32 res;
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;

   SPIDEVICE_VALIDATE_BATCH_TRANSFER_PARAMS(pDev, pTransfer, uNumTransferVecs, 
         pQupConfig, puNumCompleted);
   *puNumCompleted = 0;

   do{
      SpiDeviceLinkedBufferTransferInit(&pDev->transferInfo,
            pTransfer->pReadBuffVec, pTransfer->uNumReadVecs,
            pTransfer->pWriteBuffVec, pTransfer->uNumWriteVecs);
      res = _SpiDevice_WriteRead_BamMode(hDev, pQupConfig, 
            pTransfer->pReadBuffVec->pData, pTransfer->pReadBuffVec->uLen,
            pTransfer->pWriteBuffVec->pData, pTransfer->pWriteBuffVec->uLen,
            (uNumTransferVecs == 1)?TRUE:FALSE);
      SPIDEVICE_CHK_RESULT_BREAK(pDev->transferInfo.transferResult);
   }while((--uNumTransferVecs) && (++pTransfer));

   if(SPIDEVICE_RES_SUCCESS != res){
      if(pDev->bamObj){
         pDev->bamObj->cancelBamTransfer(pDev->bamObj, &pDev->transferInfo);
      }
   }
   QUP_WaitForCompletion(pDev);
   return res;
}

/** @brief Register buffer and transfer traversers for
         batch transfers.
    Does a batch of transfers in a sequence.

    @param[in] hDev              Device handle.
   @param[in] bufTraverser     Buffer iterators
   @param[in] transTraverser   Transfer iterators
    @return          int32 .
*/
int32
_SpiDevice_BatchTransferBlockMode
(
   SPIDEVICE_HANDLE                     hDev,
   SpiLinkedTransferType                *pTransfer,
   uint32                               uNumTransferVecs,
   SpiDeviceInfoType                    *pQupConfig,
   uint32                               *puNumCompleted
)
{
   int32 res;
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;


   SPIDEVICE_VALIDATE_BATCH_TRANSFER_PARAMS(pDev, pTransfer, uNumTransferVecs, 
         pQupConfig, puNumCompleted);
   *puNumCompleted = 0;

   do{
      SpiDeviceLinkedBufferTransferInit(&pDev->transferInfo,
            pTransfer->pReadBuffVec, pTransfer->uNumReadVecs,
            pTransfer->pWriteBuffVec, pTransfer->uNumWriteVecs);
      res = _SpiDevice_WriteRead_BlockMode(hDev, pQupConfig, 
            pTransfer->pReadBuffVec->pData, pTransfer->pReadBuffVec->uLen,
            pTransfer->pWriteBuffVec->pData, pTransfer->pWriteBuffVec->uLen);
      SPIDEVICE_CHK_RESULT(pDev->transferInfo.transferResult);
      QUP_WaitForCompletion(pDev);
      SPIDEVICE_CHK_RESULT(pDev->transferInfo.transferResult);
      *puNumCompleted = *puNumCompleted + 1;
   }while((--uNumTransferVecs) && (++pTransfer));

   return res;
}

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] hDev              Device handle.
    @param[in] *pQupConfig    Pointer to the QUP config for data transfer.
    @param[in] hTransfer      Transfer handle.
    @param[out] puNumCompleted  Pointer to return number of completed transfers.

    @return          int32 .
*/
int32
SpiDevice_BatchTransfer
(
   SPIDEVICE_HANDLE                          hDev,
   SpiLinkedTransferType                     *pTransfer,
   uint32                                    uNumTransferVecs,
   SpiDeviceInfoType                         *pQupConfig,
   uint32                                    *puNumCompleted
)
{
   /* FIXME: */
   uint32 i = uNumTransferVecs;
   uint32 bamThreshold = 0;
   SpiLinkedTransferType  *tmpPtr = pTransfer;
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;

   SPIDEVICE_VALIDATE_BATCH_TRANSFER_PARAMS(pDev, pTransfer, uNumTransferVecs, 
      pQupConfig, puNumCompleted);
   do{
      bamThreshold += SPIDEVICE_MAX(tmpPtr->pReadBuffVec->uLen, tmpPtr->pWriteBuffVec->uLen);
   }while( (bamThreshold < SPIDEVICE_BAM_TRANSFER_MIN_LEN) && (--i) && (++tmpPtr));
   
   if((pDev->bamObj) && (bamThreshold > SPIDEVICE_BAM_TRANSFER_MIN_LEN)){
      _SpiDevice_BatchTransferBamMode(hDev, pTransfer, 
            uNumTransferVecs, pQupConfig, puNumCompleted);
   }else {
      _SpiDevice_BatchTransferBlockMode(hDev, pTransfer, 
            uNumTransferVecs, pQupConfig, puNumCompleted);
   }
   return 0;
}

/** @brief Gets the HW device information.


    @param[in] hDev           Device handle.
    @param[out] hDev           spiInfo.

    @return int32 - SPIDEVICE_RES_SUCCESS on success, error code
            on error.
  */
int32
SpiDevice_GetHwInfo
(
   SPIDEVICE_HANDLE                        hDev      ,
   SpiInfoType                             *spiInfo
)
{
   SpiDevice_DeviceCtxt                    *pDev = (SpiDevice_DeviceCtxt *)hDev;
   spiInfo->uInputBlockSize = pDev->devHwInfo.uNumInputBlockSlots*4;
   spiInfo->uOutputBlockSize = pDev->devHwInfo.uNumOutputBlockSlots*4;
   spiInfo->uInputFifoSize = pDev->devHwInfo.uNumInputBlockSlots*
                              4*pDev->devHwInfo.uNumInputBlocks;
   spiInfo->uOutputFifoSize = pDev->devHwInfo.uNumOutputBlockSlots*
                              4*pDev->devHwInfo.uNumOutputBlocks;
   return SPIDEVICE_RES_SUCCESS;  
}
