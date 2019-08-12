/*==============================================================================

FILE:      DalSpi.c

DESCRIPTION: This file implements a DAL Spi Device Driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008-2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/driver/DalSpi.c#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-01-31  ms   Fixed SPI_Open Failure for Multiple Attach/Detach case.
  2014-01-13  ms   Updated with common logging mechanism
  2013-10-14  ms   In spi_Open/Close, lock added to protect client context.
  2013-08-07  sg   Freeing allocate resources in device deinit
  2011-02-11  sg   Added loggings updates.
  2011-10-18  ag   Added check to see if a device's work loop is created, if 
  				   it is then we don't create it again. During Device detach we 
				   don't destroy the workloop and the events used with it.
  2011-10-7   ag   Replace DalDevice_Detach with DAL_DevieDetach.
  2011-09-14  ag   In Spi_DeviceDeInit, call DalDevice_Detach on DAL0 handle.
  2011-07-15  ag   In SpiServeNextTransaction set isThereNext to FALSE when
  				   there are no pending transactions. Though isThereNext is 
				   set to FALSE in the begining it's subject to change when
				   multiple transactions or processed in the same invocation
				   of SpiServeNextTransaction.
  2011-02-09  ag   In Spi_SendAndReceive, call SpiConfigDefaultDevice only
  			       if the device hasn't been configured yet.
  2010-11-01  ag    Check whether pTransac2Schedule->pInputData and 
  					pTransac2Schedule->pInputData are not null in 
					SpiStartTransaction.
  2010-04-14  ag	Created, based on legacy DalSpi.c which was unlayered. 
                    This file contains the DAL1 functionality of the SPI driver

============================================================================*/
#include "DalSpi.h"


#include "DALDeviceId.h"
#include "SpiLog.h"
#include "DALSysCmn.h"

#define SPI_DEFAULT_Deassertion_Time    3175
#define SPI_DEFAULT_MIN_SlaveFrequencyHz    0
#define SPI_DEFAULT_MAX_SlaveFrequencyHz    6300000

/* Note: The lock name on Linux is upto 11 characters */
/* XX is the SPI core number */
/** 1 second TIMEOUT for failing to get the lock.
 * This time depends on the maximum transaction time on the other processor.
 * This is not the sleep time between attempts to get the lock.
 */ 
static const uint32 spi_mp_lock_timeout_msec = 1000;
 
 /*---------------------------------------------------------------------------- 
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static DALResult SpiStartTransaction(SpiDevCtxt * pDevCtxt,
                                SpiTransactionType *pTransac2Schedule);

/*===========================================================================
Name:	SpiConfigDefaultDevice

Description: Configure the device with default settings

Parameters:
	spiDeviceConfig - Pointer to device config, it'll be filled with default
					  configuration.

Dependencies:

Return:

===========================================================================*/
static void SpiConfigDefaultDevice(SpiDeviceInfoType * spiDeviceConfig)
{
   SpiDeviceParametersType spi_default_device_parameter
      = { SPI_CLK_NORMAL, SPI_CLK_IDLE_LOW, SPI_INPUT_FIRST_MODE, 
          /* u32DeassertionTime */
          SPI_DEFAULT_Deassertion_Time,

          /* u32MinSlaveFrequencyHz */
          SPI_DEFAULT_MIN_SlaveFrequencyHz, 
          
          /* u32MaxSlaveFrequencyHz */
          SPI_DEFAULT_MAX_SlaveFrequencyHz, 
          
          SPI_CS_ACTIVE_LOW, SPI_CS_KEEP_ASSERTED};

   SpiDeviceBoardInfoType spi_default_device_board_info
      = {/* nSlaveNumber */
         0, 

         SPI_CORE_MODE_MASTER};

   SpiTransferParametersType spi_default_transfer_parameter
      = { /* N */
          8,
		  SPI_TRANSFER_MODE_DEFAULT,
          SPI_LOOPBACK_DISABLED, SPI_INPUT_PACKING_DISABLED, 
          SPI_OUTPUT_UNPACKING_DISABLED,
   
          /* slaveTimeoutUs */
          0x0};
   
   spiDeviceConfig->deviceParameters = spi_default_device_parameter;
   spiDeviceConfig->deviceBoardInfo = spi_default_device_board_info;
   spiDeviceConfig->transferParameters = spi_default_transfer_parameter;
}

/*===========================================================================
Name:	SpiServeNextTransaction

Description:This is the workloop to check if there is transaction in
            a device's transaction queue. If yes, get the 
            cross-platform lock and serve it.
Parameters:
	hEvent	- Handle to event synchronization object.
	pInDevCtxt- pointer to SPI device context.

Dependencies:

Return:
	 SPI_COMPLETE/DAL_SUCCESS when the transfer is successful.
	 SPI_FAILURE/DAL_ERROR when there is a error in transfer.
===========================================================================*/
static DALResult SpiServeNextTransaction
(DALSYSEventHandle hEvent, void * pInDevCtxt)
{
	SpiDevCtxt * pDevCtxt = (SpiDevCtxt *) pInDevCtxt;
	DALBOOL isThereNext = FALSE;

	uint32 nTempCurrentIdx, nLastTranIdx;

	SpiTransactionType *pPrevTransaction;
	SpiTransactionQueueType *pTransacQueue = &(pDevCtxt->spiTransactionQueue);


	do {
		/*Set the transaction in Progress flag, need to get
		 * the synchronization for transaction queue*/
		DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);
		pTransacQueue->bTransactionInProcess = TRUE;
		DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

		/* Need to hold the spiCore syn object before we start
		 * the transaction */
		/*Get the PD lock before starting the transfer */
		nTempCurrentIdx = pTransacQueue->nCurrentIdx;
		{
			SpiTransactionType *pCur = &pTransacQueue->aTransactions[nTempCurrentIdx];
			/* Set transaction result to SPI_IN_PROGRESS and start the transaction*/
			pCur->eTransactionResult = SPI_IN_PROGRESS;
			(DAL_SUCCESS == SpiStartTransaction(pDevCtxt, pCur))?
				(pCur->eTransactionResult = SPI_COMPLETE): (pCur->eTransactionResult = SPI_FAILURE);
		}
		DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);
		/* Go to the next transaction */
		pTransacQueue->nCurrentIdx = 
			(pTransacQueue->nCurrentIdx + 1) % SPI_TRANSACTION_QUEUE_SIZE;
		pTransacQueue->nNumPending--;
		if(pTransacQueue->nNumPending > 0)
		{
			isThereNext = TRUE;
		}
		else 
		{
			pTransacQueue->bTransactionInProcess = FALSE;
			isThereNext = FALSE;
		}

		/* here we notify user the status of last transaction */
		(0 == pTransacQueue->nCurrentIdx) ? (nLastTranIdx = SPI_TRANSACTION_QUEUE_SIZE - 1) :
			(nLastTranIdx = pTransacQueue->nCurrentIdx - 1);
		pPrevTransaction = &(pTransacQueue->aTransactions[nLastTranIdx]);

		/* This the entry in the queue could be overwiten by a new 
		   transaction before user calls getTransferStatus call.
		   But this is unlikely if the queue is not always full and
		   if the status check happens fast.
		   */
		if(pPrevTransaction->spiTransfer.dalSysEvent)
		{
		        /* if the event has call back, DALSYS will call the callback 
			 * function
			 */
			SPIDEVICE_LOG(INFO, "Notify Transfer(%x) Complete,Device=0x%x",
							     pPrevTransaction->u32TransferId, pDevCtxt->DevId);
			DALSYS_EventCtrl(pPrevTransaction->spiTransfer.dalSysEvent, 
					DALSYS_EVENT_CTRL_TRIGGER);
		}

		DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

	}while(isThereNext == TRUE);

	return DAL_SUCCESS;
}

/*===========================================================================
Name:	SpiInit

Description: Initializes the SPI driver

Parameters:
	pClientCtxt     - Context of the client calling the function.

Dependencies:

Return:

===========================================================================*/
static SpiInitResultType SpiInit(SpiClientCtxt *pclientCtxt)
{
	// uint8 nInstanceId;
	//   uint8 nDeviceId;
	SpiInitResultType result = SPI_INIT_SUCCESS;
	SpiDevCtxt *pDevCtxt = pclientCtxt->pSpiDevCtxt;


	/* About protecting the integrety of spiTransactionQueue and 
	   devie configuration:
	   This function is invoked in the SPI_DeviceInit nested 
	   in DalDevice_Init. It is called at the very beginning of DAL device
	   initialization and only once. No simulataneous excution possible.
	   So, no need for a synchronization scheme here.
	   */

	/* Points to the current ongoing transaction in the Transaction DB */
	pDevCtxt->spiTransactionQueue.nCurrentIdx = 0;

	pDevCtxt->spiTransactionQueue.u32TotalTransactions = 0;

	/* This is the total number of transactions saved in the Transaction DB.
	 * Each of these transactions will be processed in order */
	pDevCtxt->spiTransactionQueue.nNumPending = 0;
	pDevCtxt->spiTransactionQueue.bTransactionInProcess = FALSE;

	pDevCtxt->bTransInProgress = FALSE;

	SpiConfigDefaultDevice(&(pclientCtxt->spiDeviceConfig) );
	/* Clock Enable when module is loaded */
	/* This should be removed once CLK-Resource-manager is used */

	/* Move the driver initialization, and note we need this information
	 * in SpiDmInit so I have moved teh SpiEnableClock before SpiDmInit */
	//FIXME: Need to take care of this.

	return result;
} /* END SpiInit */

/*===========================================================================
Name:	SpiAddTransaction

Description:	Adds an SPI transaction to the transaction queue.

Parameters:
	pClientCtxt     - Context of the client calling the function.
	pSpiTransfer    - Transfer parameters
	outputAddr      - Pointer to the output buffer
	outputDataLen   - Length of output buffer
	inputAddr       - Pointer to input buffer
	inputDataLen    - Length of input buffer
	inputDataLenReq - Length of data received
	transferId      - Transfer ID assigned to the transfer by the SPI DAL1
	 				  driver
Dependencies: Called only after acquiring the hDeviceSynchronization mutex.

Return:
	SPI_QUEUED 	- the transfer was successfully added to SPI DAL1 queue.
	SPI_FULL 	- Was unable to add the transaction to the SPI DAL1 queue,
	              because the queue was full.
	SPI_INVALID_PARAMETER - The transfer parameter was un-acceptable.
===========================================================================*/
static SpiTransferStatus SpiAddTransaction
(
 SpiClientCtxt * pClientCtxt,   const SpiTransferType* pSpiTransfer,
  SpiDataAddrType* outputAddr,   int  outputDataLen,   
  SpiDataAddrType* inputAddr,   int  inputDataLen,   int* inputDataLenReq,
  uint32* transferId
)
{
   SpiTransactionType *pNewTransaction;
   uint32 nNextTransactionIdx;

   SpiDevCtxt *pDevCtxt = pClientCtxt->pSpiDevCtxt;
   SpiTransactionQueueType *pTransacQueue = &(pDevCtxt->spiTransactionQueue);

   if(SPI_TRANSACTION_QUEUE_SIZE == pTransacQueue->nNumPending)
   {
      /* Transaction Queue is full, try later */
      return SPI_FULL;
   }

   nNextTransactionIdx = 
      (pTransacQueue->nCurrentIdx + pTransacQueue->nNumPending) % 
      SPI_TRANSACTION_QUEUE_SIZE;

   pNewTransaction = &pTransacQueue->aTransactions[nNextTransactionIdx];

   DALSYS_memset(pNewTransaction, 0x0, sizeof(SpiTransactionType));

   /* init as SPI_FAILURE */
   pNewTransaction->eTransactionResult = SPI_FAILURE;

   if((0 == pSpiTransfer->u32NumInputTransfers) && (0 == pSpiTransfer->u32NumOutputTransfers))
   {
      return SPI_INVALID_PARAMETER;
   }
   /* Setup the transaction structure with the appropriate parms, so that it can be
	* processed by the workloop */
   pNewTransaction->u32TransferId = pTransacQueue->u32TotalTransactions++;
   pNewTransaction->eTransactionResult = SPI_QUEUED;
   pNewTransaction->spiTransfer = *pSpiTransfer;
   pNewTransaction->transactionDeviceConfig = pClientCtxt->spiDeviceConfig;

   pNewTransaction->pInputData = inputAddr;
   pNewTransaction->pOutputData = outputAddr;

   pNewTransaction->inputDataLen = inputDataLen;
   pNewTransaction->outputDataLen = outputDataLen;

   pTransacQueue->nNumPending++;

   *transferId = pNewTransaction->u32TransferId;

   SPIDEVICE_LOG(INFO, "Added TX id %x to Queue,Device=0x%x",
						pNewTransaction->u32TransferId, pDevCtxt->DevId);

   return SPI_QUEUED;
} /* END SpiAddTransaction */



/*===========================================================================
Name:	SpiStartTransaction

Description: Calls into SPI DAL0 for starting a SPI transfer.

Parameters:
	pDevCtxt		  - SPI DAL1 device context.
	pTransac2Schedule - Pointer to transaction in transaction queue.

Dependencies:

Return: returns the return value of SpiDevice_WriteRead.

===========================================================================*/
static DALResult SpiStartTransaction(SpiDevCtxt * pDevCtxt,
                                SpiTransactionType *pTransac2Schedule)
{

	SpiDataAddrType *pRead, *pWrite;
	SpiDeviceInfoType *pDevConfig;
	uint32 uReadLen, uWriteLen;

	pRead = pTransac2Schedule->pInputData;
	pWrite = pTransac2Schedule->pOutputData;
	uReadLen = pTransac2Schedule->inputDataLen;
	uWriteLen = pTransac2Schedule->outputDataLen;
	pDevConfig = &(pTransac2Schedule->transactionDeviceConfig);

	if(!uReadLen)
	{
		pRead = NULL;
	} 
	if(!uWriteLen)
	{
		pWrite = NULL;
	}
    SPIDEVICE_LOG(INFO, "Transfer(%x) Queued to HW,Device=0x%x",
						pTransac2Schedule->u32TransferId, pDevCtxt->DevId);	
	return SpiDevice_WriteRead(pDevCtxt->hSpiDevice, pDevConfig, pRead,uReadLen, pWrite, uWriteLen);

} /* END SpiStartTransaction */

/*===========================================================================
Name:	Spi_DriverInit

Description: Filler function, maybe enhanced in the future

Parameters:
	pDrvCtxt

Dependencies:

Return:

===========================================================================*/
DALResult Spi_DriverInit(SpiDrvCtxt *pDrvCtxt)
{
   SpiLog_Init();
   return DAL_SUCCESS;
}

/*===========================================================================
Name:	Spi_DriverDeInit

Description: Filler function, maybe enhanced in the future

Parameters:
	pCtxt

Dependencies:

Return:

===========================================================================*/
DALResult Spi_DriverDeInit(SpiDrvCtxt *pCtxt)
{

   return DAL_SUCCESS;
}

/*===========================================================================
Name:	Spi_DeviceInit

Description: Does all device specific initialization: creates sync objects,
			 event objects, work loops etc. Connects to appropriate SPI PD.

Parameters:
	pClientCtxt

Dependencies:

Return:
		DAL_SUCCESS init was successful.
		DAL_ERROR   encountered an error
===========================================================================*/
DALResult Spi_DeviceInit(SpiClientCtxt *pClientCtxt)
{
   SpiDevCtxt *pDevCtxt = pClientCtxt->pSpiDevCtxt;
	
   if(SpiDevice_Init(pDevCtxt->DevId, &pDevCtxt->hSpiDevice))
   {
	   return DAL_ERROR;
   }

   /*--------------------------------------------------------------------------
     Initialize Synchronization Object.
     This is used to synchronize between client calls on same device

     --------------------------------------------------------------------------*/
  if(!pDevCtxt->dwWorkLoopCreated)
  {
	  if(DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
				  &(pDevCtxt->hDeviceSynchronization),
				  &(pDevCtxt->DeviceSyncObject))
			  != DAL_SUCCESS)
	  {
		  SPIDEVICE_LOG(ERROR,"Spi_DeviceInit:"
                       "Error in synchronize between client and device=0x%x",
                        pDevCtxt->DevId);
		  return DAL_ERROR;
	  }

	  if(DAL_SUCCESS != DALSYS_RegisterWorkLoop
			  (0, 10, &(pDevCtxt->hStartTransferWorkLoop), NULL)
		)
	  {
		  SPIDEVICE_LOG(ERROR, "Spi_DeviceInit:"
                        "spi fail to create workloop, device=0x%x",
                         pDevCtxt->DevId);
		  return DAL_ERROR;
	  }

	  /* Create Asyc Object dedicated for workloop */
	  if(DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
				  &(pDevCtxt->hStartTransferEvent),
				  NULL))
	  {
		  SPIDEVICE_LOG(ERROR,  "Spi_DeviceInit:"
                        "Error creating start workloop event, device=0x%x",
                         pDevCtxt->DevId);
		  return DAL_ERROR;
	  }

	  if(DAL_SUCCESS != DALSYS_AddEventToWorkLoop(pDevCtxt->hStartTransferWorkLoop,
				  SpiServeNextTransaction,
				  (void *) pDevCtxt,
				  pDevCtxt->hStartTransferEvent,
				  NULL))
	  {
		  SPIDEVICE_LOG(ERROR, "Spi_DeviceInit:"
                        "unable to add event workloop, device=0x%x",
                         pDevCtxt->DevId);
		  return DAL_ERROR;
	  }
	  pDevCtxt->dwWorkLoopCreated = 1;
  }


   if( SpiInit(pClientCtxt) != SPI_INIT_SUCCESS)
   {
	    SPIDEVICE_LOG(ERROR, "Spi_DeviceInit:"
                      "Error in initialization of SPI driver, device=0x%x",
                       pDevCtxt->DevId);
	    return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

/*===========================================================================
Name:	Spi_DeviceDeInit

Description: Does device specific de-initialization. Destroys all objects
 			 that were allocated during the device intialization.
Parameters:
	pClientCtxt

Dependencies:

Return: DAL_SUCCESS

===========================================================================*/
DALResult Spi_DeviceDeInit(SpiClientCtxt *pClientCtxt)
{
   SpiDevCtxt *pDevCtxt = pClientCtxt->pSpiDevCtxt;

   if(pDevCtxt->hSpiDevice)
   {
	   SPIDEVICE_LOG(INFO, "DE-INITIALIZED, Device=0x%x", pDevCtxt->DevId);
	   SpiDevice_DeInit(pDevCtxt->hSpiDevice);
	   if(pDevCtxt->dwRefs == 1)
	   {
   	      DALSYS_DestroySyncObject(pDevCtxt->hDeviceSynchronization);
    	  DALSYS_DestroyWorkLoopObject(pDevCtxt->hStartTransferWorkLoop);
    	  DALSYS_DestroyEventObject(pDevCtxt->hStartTransferEvent);
		  pDevCtxt->dwWorkLoopCreated = 0; 
	   }
	   pDevCtxt->hSpiDevice = NULL;
   }
   return DAL_SUCCESS;
}

/*===========================================================================
Name:	Spi_PowerEvent

Description: Filler function, maybe enhanced in the future

Parameters:
	pCtxt
	PowerCmd
	PowerDomain

Dependencies:

Return:

===========================================================================*/
DALResult 
Spi_PowerEvent(SpiClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

/*===========================================================================
Name:	Spi_Open

Description: Filler function, maybe enhanced in the future, it currently
			 just sets teh client's configuration to the default
			 configuration.
Parameters:
	pclientCtxt
	dwaccessMode 

Dependencies:

Return:

===========================================================================*/
DALResult 
Spi_Open(SpiClientCtxt *pclientCtxt, uint32 dwaccessMode )
{
   	DALResult eResult = DAL_ERROR;
   	SpiDevCtxt *pDevCtxt;

   if ((NULL == pclientCtxt) ||
		   (NULL == (pDevCtxt = pclientCtxt->pSpiDevCtxt)))
   {
	   return eResult;
   }

   DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

   if (TRUE == pclientCtxt->bClientIsDeviceOpened)
   {
	  return eResult;
   }

   do
   {
	  if (0 == pDevCtxt->uDalOpenCount)
	  {
		 if (SPIDEVICE_RES_SUCCESS !=
			 SpiDevice_SetPowerState(pDevCtxt->hSpiDevice, 
					 SPIDEVICE_POWER_STATE_2))
		 {
			break;
		 }

	  }
	  pDevCtxt->uDalOpenCount++;
	  pclientCtxt->bClientIsDeviceOpened = TRUE;
	  eResult = DAL_SUCCESS;
   } while(0);

   DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

   return eResult;
}

/*===========================================================================
Name:	Spi_Close

Description: Filler function, maybe enhanced in the future

Parameters:
	pCtxt

Dependencies:

Return:

===========================================================================*/
DALResult 
Spi_Close(SpiClientCtxt *pclientCtxt)
{
   DALResult eResult = DAL_ERROR;
   SpiDevCtxt *pDevCtxt;

   if ((NULL == pclientCtxt) ||
		   (NULL == (pDevCtxt = pclientCtxt->pSpiDevCtxt)))
   {
	   return eResult;
   }

   DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

   if ((FALSE == pclientCtxt->bClientIsDeviceOpened) ||
	   ((0 == pDevCtxt->uDalOpenCount)))
   {
	  return eResult;
   }

   do
   {
	  if (1 == pDevCtxt->uDalOpenCount)
	  {
		 if (SPIDEVICE_RES_SUCCESS !=
			 SpiDevice_SetPowerState(pDevCtxt->hSpiDevice, 
					 SPIDEVICE_POWER_STATE_0))
		 {
			break;
		 }
	  }

	  pDevCtxt->uDalOpenCount--;
	  pclientCtxt->bClientIsDeviceOpened = FALSE;
	  eResult = DAL_SUCCESS;

   } while(0);

   DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

    return eResult;
}

/*===========================================================================
Name:	Spi_Info

Description: Filler function, maybe enhanced in the future

Parameters:
	pCtxt
	pdeviceInfo
	dwSize

Dependencies:

Return:

===========================================================================*/
DALResult 
Spi_Info(SpiClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}

/*===========================================================================
Name:	Spi_GetInputBlockSize

Description:

Parameters:
	pClientCtxt
	inputBlockSize
	olen1

Dependencies:

Return:

===========================================================================*/
DALResult 
Spi_GetInputBlockSize( SpiClientCtxt * pClientCtxt,  uint32 *  inputBlockSize, uint32 olen1) 
{
   SpiInfoType spiInfo;
   SpiDevice_GetHwInfo(pClientCtxt->pSpiDevCtxt->hSpiDevice,
		   &spiInfo);
   *inputBlockSize = spiInfo.uInputBlockSize;

   return DAL_SUCCESS;
}

/*===========================================================================
Name:	Spi_GetOutputBlockSize

Description:

Parameters:
	pClientCtxt
	outputBlockSize
	olen1

Dependencies:

Return:

===========================================================================*/
DALResult 
Spi_GetOutputBlockSize( SpiClientCtxt * pClientCtxt,  uint32 *  outputBlockSize, uint32 olen1) 
{
   SpiInfoType spiInfo;
   SpiDevice_GetHwInfo(pClientCtxt->pSpiDevCtxt->hSpiDevice,
		   &spiInfo);
   *outputBlockSize = spiInfo.uOutputBlockSize;

   return DAL_SUCCESS;
}

/*===========================================================================
Name:	Spi_ConfigureDevice

Description: Set the device configuration of the client.

Parameters:
	pClientCtxt - Current client context.
	deviceInfo  - pointer to device configuration that needs to be set for 
				  this client.
Dependencies:

Return:
	DAL_SUCCESS
	DAL_ERROR
===========================================================================*/
DALResult 
Spi_ConfigureDevice( SpiClientCtxt * pClientCtxt,  const SpiDeviceInfoType *  pSpiDeviceInfo) 
{
   DALResult dalResult = DAL_SUCCESS;

   SpiDevCtxt *pDevCtxt = pClientCtxt->pSpiDevCtxt;

   DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

   pClientCtxt->spiDeviceConfig = *pSpiDeviceInfo;
   pClientCtxt->spiDeviceConfigured = 1;

   DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

   return dalResult;
}

/*===========================================================================
Name:	Spi_GetDeviceConfiguration

Description: Returns the current device configuration for the client.

Parameters:
	pClientCtxt - Current client context.
	deviceInfo  - pointer to be filled with the current device configuration.

Dependencies:

Return:
	DAL_SUCCESS
	DAL_ERROR

===========================================================================*/
DALResult 
Spi_GetDeviceConfiguration( SpiClientCtxt * pClientCtxt,  SpiDeviceInfoType * deviceInfo, uint32 olen1) 
{
   *deviceInfo = pClientCtxt->spiDeviceConfig;

   return DAL_SUCCESS;
}


/*===========================================================================
Name:	Spi_StartTransfer

Description:
			This function, adds the SPI data transfer request in to the SPI
			DAL1 queue. If it's unable to add it to the queue, it returns 
			error. If the transfer request was added successfully to the 
			queue, it returns success immeadiately if the transfer request was
			asynchronous, if not it waits till the transfer completes and the 
			returns the status of the transfer.
Parameters:
	pClientCtxt     - Context of the client calling the function.
	pSpiTransfer    - Transfer parameters
	outputAddr      - Pointer to the output buffer
	outputDataLen   - Length of output buffer
	inputAddr       - Pointer to input buffer
	inputDataLen    - Length of input buffer
	inputDataLenReq - Length of data received
	transferId      - Transfer ID assigned to the transfer by the SPI DAL1
	
Dependencies:

Return:
	SPI_QUEUED     The transfer is successfully started, this is for async case.
	SPI_COMPLETE   The transfer is successfully completed, this is for sync case.
	SPI_FAILURE    The transfer could not be setup or there was an error
	               in carrying out the transfer.
	SPI_BUSY       The transfer is not setup because SPI core is busy
	SPI_FULL       The transfer cannnot be queued because DAL SPI queue is full
	SPI_INVALID_PARAMETER Invalid parameter, 
                 for example SPI_TRANSFER_MODE_FIFO is used for transfer mode.
===========================================================================*/
DALResult Spi_StartTransfer
( SpiClientCtxt * pClientCtxt, SpiTransferType *  pSpiTransfer,
  SpiDataAddrType*  outputAddr,  int  outputDataLen,  
  SpiDataAddrType*  inputAddr,  int  inputDataLen,  int*  inputDataLenReq, uint32 olen6,
  uint32*  pTransferId, uint32 olen7) 
{
   DALBOOL bScheduleTransfer = FALSE;
   SpiTransferStatus transfStatus;
   
   SpiDevCtxt *pDevCtxt = pClientCtxt->pSpiDevCtxt;
   SpiTransactionQueueType *pTransacQueue = &(pDevCtxt->spiTransactionQueue);
   SpiTransactionType * pCurrentTransaction = NULL;
   uint32 u32TransactionIdx = 0;
   
   /* remember whether we need wait for transaction to complete */
   DALBOOL bWaitEvent = FALSE;

   /* create a WaitEvent if client does not provide one */
   if(NULL == pSpiTransfer->dalSysEvent)
   {
      if(DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL,
                            &(pSpiTransfer->dalSysEvent),
                            NULL)
         != DAL_SUCCESS)
      {
         return DAL_ERROR;
      }
      bWaitEvent = TRUE;
   }

   DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

   /* parameter checking */
   transfStatus = SpiAddTransaction(pClientCtxt, pSpiTransfer, outputAddr, 
                                      outputDataLen, inputAddr, inputDataLen, 
                                      inputDataLenReq, pTransferId);

   if((SPI_QUEUED == transfStatus) )
   {
      /* index of the just queued transaction */
      u32TransactionIdx = (*pTransferId)%SPI_TRANSACTION_QUEUE_SIZE;

      if (!(pTransacQueue->bTransactionInProcess))
      {
         pTransacQueue->bTransactionInProcess = TRUE;
      
         bScheduleTransfer = TRUE;
      }
   }

   DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

   if(bScheduleTransfer)
   {

      DALSYS_EventCtrl(pDevCtxt->hStartTransferEvent, DALSYS_EVENT_CTRL_TRIGGER);
   }

   if(bWaitEvent)
   {
      if(SPI_QUEUED == transfStatus)
      {
        DALSYS_EventWait(pSpiTransfer->dalSysEvent);

        /* update the new transfer status */
        pCurrentTransaction = 
           &(pTransacQueue->aTransactions[u32TransactionIdx]);
        transfStatus = pCurrentTransaction->eTransactionResult;
      }
      DALSYS_DestroyObject(pSpiTransfer->dalSysEvent);
	  pSpiTransfer->dalSysEvent = NULL;

   }

   SPIDEVICE_LOG(INFO, "Spi_StartTransfer:"
             "SPI data transfer request status to DAL1 queue %x, Device=0x%x",
              transfStatus, pDevCtxt->DevId);
   return (DALResult) transfStatus;
}

/*===========================================================================
Name:	Spi_GetTransferStatus

Description: returns the status of a SPI transaction, for a given transferId

Parameters:
	pClientCtxt     - Context of the client calling the function.
	transferId

Dependencies:

Return:
	SPI_COMPLETE   The transfer is successfully completed.
    SPI_FAILURE    The transfer failed.
    SPI_QUEUED     The transfer is still in the queue.
    SPI_IN_PROGRESS The transfer is being processed by the SPI core.
    SPI_INVALID_ID The transfer ID is invalid.
===========================================================================*/
DALResult 
Spi_GetTransferStatus( SpiClientCtxt * pClientCtxt,  uint32  transferId) 
{
   uint32 u32TransactionIdx;
   SpiTransactionType *pTransaction;
   SpiTransferStatus transfStatus;

   SpiDevCtxt *pDevCtxt = pClientCtxt->pSpiDevCtxt;
   SpiTransactionQueueType *pTransacQueue = &(pDevCtxt->spiTransactionQueue);

   u32TransactionIdx = transferId%SPI_TRANSACTION_QUEUE_SIZE;

   pTransaction = &(pTransacQueue->aTransactions[u32TransactionIdx]);

   if(pTransaction->u32TransferId == transferId)
   {
      transfStatus = pTransaction->eTransactionResult;
   }
   else
   {
      transfStatus = SPI_INVALID_ID;
   }

   return (DALResult) transfStatus;
}

/*===========================================================================
Name:	Spi_Send

Description:

Parameters:
	pClientCtxt     - Context of the client calling the function.
	outputAddr      - Pointer to the output buffer
	outputDataLen   - Length of output buffer

Dependencies:

Return:
	SPI_COMPLETE   The read is successfully completed.
    SPI_FAILURE    The transfer could not be setup or there was an error
                   in carrying out the transfer
===========================================================================*/
DALResult Spi_Send( SpiClientCtxt * pClientCtxt,  
                    SpiDataAddrType*  outputAddr,  
                    int  outputDataLen)
{
   SpiTransferType sendTransfer;
   uint32 dummyTransferId;
   int dummyInputDataLenReq;
   DALResult dalResult;

   if(!pClientCtxt->spiDeviceConfigured)
   {
      /* The Client is not configured the device
         use default setting */
      SpiConfigDefaultDevice(&(pClientCtxt->spiDeviceConfig));
   }

   sendTransfer.u32NumOutputTransfers = outputDataLen;
   sendTransfer.u32NumInputTransfers = 0;

   sendTransfer.dalSysEvent = NULL;
   sendTransfer.queueIfBusy = TRUE;
   sendTransfer.transferMode = SPI_TRANSFER_MODE_DEFAULT;
   sendTransfer.u32Priority = 0;

   dalResult = Spi_StartTransfer
      ( pClientCtxt, &sendTransfer, outputAddr, outputDataLen,   
        NULL,  0,  &dummyInputDataLenReq, 0, &dummyTransferId, 0);

   return dalResult;
}

/*===========================================================================
Name:	Spi_Receive

Description:
	Conduct a data read-only transfer, get input data.
    The data transfer will be queued for its desired SPI core with 
    its desired configuration.
Parameters:
	pClientCtxt     - Context of the client calling the function.
	inputAddr       - Pointer to input buffer
	inputDataLen    - Length of input buffer
	inputDataLenReq - Length of data received
	u32NumInputTransfers - Number of input transfers to be executed
Dependencies:

Return:
	SPI_COMPLETE   The Receive is successfully completed.
    SPI_FAILURE    The transfer could not be setup or there was an error
                   in carrying out the transfer
    SPI_FULL       SPI DAL's working queue is full. It cannot process this request.
===========================================================================*/
DALResult 
Spi_Receive( SpiClientCtxt * pClientCtxt,  uint32  u32NumInputTransfers,
                  SpiDataAddrType*  inputAddr,  int  inputDataLen,  
                  int*  inputDataLenReq, uint32 olen4) 
{
   SpiTransferType receiveTransfer;
   uint32 dummyTransferId;
   DALResult dalResult;

   if(!pClientCtxt->spiDeviceConfigured)
   {
      /* The Client is not configured the device
         use default setting */
      SpiConfigDefaultDevice(&(pClientCtxt->spiDeviceConfig));
   }

   receiveTransfer.u32NumOutputTransfers = 0;
   receiveTransfer.u32NumInputTransfers = u32NumInputTransfers;

   receiveTransfer.dalSysEvent = NULL;
   receiveTransfer.queueIfBusy = TRUE;
   receiveTransfer.transferMode = SPI_TRANSFER_MODE_DEFAULT;
   receiveTransfer.u32Priority = 0;

   dalResult = Spi_StartTransfer
      ( pClientCtxt, &receiveTransfer, NULL, 0,   
        inputAddr,  inputDataLen,  inputDataLenReq, olen4,
        &dummyTransferId, 0);

   return dalResult;
}

/*===========================================================================
Name:	Spi_SendAndReceive

Description:
	Conduct a bidirectional data transfer.
    The data transfer will be conducted for its desired SPI core with 
    its desired configuration.
    This function is synchrous. It blocks until all data are read or failure happens

Parameters:
	pClientCtxt     	 - Context of the client calling the function.
	outputAddr      	 - Pointer to the output buffer
	outputDataLen   	 - Length of output buffer
	inputAddr       	 - Pointer to input buffer
	inputDataLen    	 - Length of input buffer
	inputDataLenReq 	 - Length of data received
	transferId      	 - Transfer ID assigned to the transfer by the SPI DAL1
	u32NumInputTransfers - Number of transfers.

Dependencies:

Return:
	SPI_COMPLETE   The Receive is successfully completed.
    SPI_FAILURE    The transfer could not be setup or there was an error
                   in carrying out the transfer
    SPI_FULL       SPI DAL's working queue is full. It cannot process this request.
===========================================================================*/
DALResult Spi_SendAndReceive(SpiClientCtxt * pClientCtxt,  
                                  SpiDataAddrType*  outputAddr,  int  outputDataLen,
                                  uint32  u32NumInputTransfers,  
                                  SpiDataAddrType*  inputAddr,  int  inputDataLen,  int*  inputDataLenReq,
                                  uint32 olen6) 
{
   /* the transfer parameters for SendAndReceive (sar) */
   SpiTransferType sarTransfer;
   uint32 dummyTransferId;
   DALResult dalResult;

   if(!pClientCtxt->spiDeviceConfigured)
   {
	   /* use default setting */
	   SpiConfigDefaultDevice(&(pClientCtxt->spiDeviceConfig));
   }

   sarTransfer.u32NumOutputTransfers = outputDataLen;
   sarTransfer.u32NumInputTransfers = u32NumInputTransfers;

   sarTransfer.dalSysEvent = NULL;
   sarTransfer.queueIfBusy = TRUE;
   sarTransfer.transferMode = SPI_TRANSFER_MODE_DEFAULT;
   sarTransfer.u32Priority = 0;

   dalResult = Spi_StartTransfer
      ( pClientCtxt, &sarTransfer, outputAddr, outputDataLen,
        inputAddr,  inputDataLen,  inputDataLenReq, olen6,
        &dummyTransferId, 0);

   return dalResult;
}

/*===========================================================================
Name:	Spi_Reset

Description:
			Abort all the transactions in the transaction queue. Soft reset the SPI core.
            This function is provided to slave mode transfer. In slave mode, the core
            replies on the master device to proceed the transfer. If master does not
            responds, the slave could stick in a transfer forever. This function
            can be used to get the slave out of the foreverness.
Parameters:
	pClientCtxt     - Context of the client calling the function.

Dependencies:

Return:
     DAL_SUCCESS    The operation is successfully completed.
     DAL_ERROR      The operation failed.
===========================================================================*/
DALResult Spi_Reset( SpiClientCtxt * pClientCtxt) 
{
	SpiDevCtxt *pDevCtxt = pClientCtxt->pSpiDevCtxt;
	SpiTransactionQueueType *pTransacQueue = &(pDevCtxt->spiTransactionQueue);

	uint32 nTempCurrentIdx;
	SpiTransactionType * currentTransaction;

	/* freeze the transaction queue */
	DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

	/* sync with the one in SpiServeNextTransaction */

	if(!pTransacQueue->bTransactionInProcess)
	{
		/* we are done */
		return DAL_SUCCESS;
	}

	pTransacQueue->bTransactionInProcess = FALSE;
	nTempCurrentIdx = pTransacQueue->nCurrentIdx;
	currentTransaction = &pTransacQueue->aTransactions[nTempCurrentIdx];


	/* we are done with the last transaction. 
	   release mutex to give other processor a chance
	   */

	while (pTransacQueue->nNumPending > 0)
	{
		/* notify current transaction client the failure */
		currentTransaction->eTransactionResult = SPI_FAILURE;

		if(currentTransaction->spiTransfer.dalSysEvent)
		{
			/* if the event has call back, DALSYS will call the callback 
			 * function
			 */
			DALSYS_EventCtrl(currentTransaction->spiTransfer.dalSysEvent, 
					DALSYS_EVENT_CTRL_TRIGGER);
		}

		/* Go to the next transaction */
		pTransacQueue->nNumPending--;

		/* Go to the next transaction */
		pTransacQueue->nCurrentIdx = 
			(pTransacQueue->nCurrentIdx + 1) % SPI_TRANSACTION_QUEUE_SIZE;
		currentTransaction = &pTransacQueue->aTransactions[pTransacQueue->nCurrentIdx];
	}

	DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

	return DAL_SUCCESS;
}
