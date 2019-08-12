#ifndef DALSpi_H
#define DALSpi_H
/*==============================================================================

FILE:      DalSpi.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/driver/DalSpi.h#1 $$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-06-06  dk    Added dynamic device context initialisation requirement.
  2011-10-19  ag	Include DalSpiTarget.h for DALSPI_NUM_DEVICES.
  2011-10-18  ag	Added workLoopCreated to SpiDevCtxt.
  2010-04-11  ag	Created based on legacy DalSpi.h. This is a trimmed down
                    version for DAL1 SPI driver.
============================================================================*/

#include "DALFramework.h"
#include "ddispi.h"
#include "SpiDevice.h"
#include "DALPropDef.h"

#define SPI_TRANSACTION_QUEUE_SIZE 12

/*------------------------------------------------------------------------------
Declaring a "Spi" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct SpiDrvCtxt SpiDrvCtxt;
typedef struct SpiDevCtxt SpiDevCtxt;
typedef struct SpiClientCtxt SpiClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "Spi" Vtable
------------------------------------------------------------------------------*/
typedef struct SpiDALVtbl SpiDALVtbl;
struct SpiDALVtbl
{
	int (*Spi_DriverInit)(SpiDrvCtxt *);
	int (*Spi_DriverDeInit)(SpiDrvCtxt *);
};

/**
 * This is the internal transaction type used by the SPI
 * transaction queue.
 */
typedef struct
{
   /* unique id to identify the transaction in the queue
    * This number is also the sequence number of this
    * transaction in the queue since the queue starts.
    * u32TransferId%SPI_TRANSACTION_QUEUE_SIZE is the index
    * to the transaction in the transaction queue array.
    */
   uint32 u32TransferId;

   /* the status/result of the transaction */
   SpiTransferStatus eTransactionResult;

   /* the transfer parameters used for this transfer */
   SpiTransferType spiTransfer; 

   /* the device configuration for this transaction */
   SpiDeviceInfoType transactionDeviceConfig;

   /* Input buffer passed in by the client */
   SpiDataAddrType *pInputData;
   uint32 inputDataLen;
   
   /* Output buffer passed in by the client */
   SpiDataAddrType *pOutputData;
   uint32 outputDataLen;

   /* the number words to be copied from the input FIFO to the user buffer */
   uint32 nNumInputWords; 

   /* the IO mode to be used for this transfer when the user request 
    * transferMode to be default.
    */
   SpiTransferModeType eIOmode; 

} SpiTransactionType;

/* This is the SPI transacation queue.
 */
typedef struct {
   /* transaction queue implementation using circular array */
   SpiTransactionType aTransactions[SPI_TRANSACTION_QUEUE_SIZE];

   /* index into the transaction in aTransactions array being processed */
   uint32 nCurrentIdx;

   /* number of transactions in the queue waiting to be processed */
   uint32 nNumPending;
   
   /* number of transactions since the birth of this queue */
   uint32 u32TotalTransactions;
   
   /* indicate whether the queue is being processed by spi core */
   DALBOOL bTransactionInProcess;

} SpiTransactionQueueType;


/**
 * The return type is meant to be used for functions in SPI/DAL
 * which are not tied to the DAL interface directly
 */
typedef enum
{
   SPI_SUCCESS, /* SPI internal function call was successful*/
   SPI_ERROR, /* SPI internal function failed */
   SPI_RESULT_MAX = 0x7fffffff, /*Forcing enumb to be 32bit*/
}SpiResultType;

/**
 * The return type used by SpiInit
 */
typedef enum
{
   SPI_INIT_SUCCESS = 0, /**< the SPI driver was successfully initialized */
   SPI_INIT_FAILURE = 1 /**< the SPI driver initialized failed (out of memory) */
} SpiInitResultType;


struct SpiDevCtxt
{
	//Base Members
	uint32   dwRefs;                                    
    DALDEVICEID DevId;                                  
    uint32   dwDevCtxtRefIdx; 
    SpiDrvCtxt  *pSpiDrvCtxt;                             
	DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32 Reserved[16];

   /* mutex lock on the spiTransactionQueue */
   DALSYS_SYNC_OBJECT(DeviceSyncObject);
   DALSYSSyncHandle  hDeviceSynchronization;

   DALSYSEventHandle    hStartTransferEvent;
   DALSYSWorkLoopHandle hStartTransferWorkLoop;

   SpiTransactionQueueType spiTransactionQueue;

   /* bTransInProgress TRUE (also means we hold CROSSPLATFORM_MUTEX in MP 
	 			       environment) the work loop is processing a transaction,
					   it might have been completed, but has not been dequeued
					   yet.
                       FALSE (also means we don't hold CROSSPLATFORM_MUTEX in MP 
	 			       environment)the work loop is not actively processing a 
					   transaction.
   */
   DALBOOL      bTransInProgress;

/*All new addition to the layering*/
  SPIDEVICE_HANDLE hSpiDevice;
  uint32 dwWorkLoopCreated;
  uint32 uDalOpenCount;
};

struct SpiDrvCtxt
{
	//Base Members
	SpiDALVtbl SpiDALVtbl;
	uint32  dwNumDev;
	uint32  dwSizeDevCtxt;
	uint32  bInit;
    uint32  dwRefs;
	SpiDevCtxt SpiDevCtxt[1];
    //Spi Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "Spi" Client Context
------------------------------------------------------------------------------*/
struct SpiClientCtxt
{
	//Base Members
	uint32  dwRefs;                     
    uint32  dwAccessMode;  
    void *pPortCtxt;
    SpiDevCtxt *pSpiDevCtxt;            
    DalSpiHandle DalSpiHandle; 
	//Spi Client state can be added by developers here
   SpiDeviceInfoType spiDeviceConfig;
   /* spiDeviceConfigured: To track whether the client has configured  
                           a  SPI device, so that we don't override the client 
                           configuration while calling spi_send/spi_receive */
   uint32   spiDeviceConfigured; 
   DALBOOL bClientIsDeviceOpened;
};

DALResult Spi_DriverInit(SpiDrvCtxt *);
DALResult Spi_DriverDeInit(SpiDrvCtxt *);
DALResult Spi_DeviceInit(SpiClientCtxt *);
DALResult Spi_DeviceDeInit(SpiClientCtxt *);
DALResult Spi_PowerEvent(SpiClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult Spi_Open(SpiClientCtxt *, uint32);
DALResult Spi_Close(SpiClientCtxt *);
DALResult Spi_Info(SpiClientCtxt *,DalDeviceInfo *, uint32);
DALResult Spi_InheritObjects(SpiClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);

/*------------------------------------------------------------------------------
Functions specific to DalSpi interface
------------------------------------------------------------------------------*/
DALResult Spi_GetInputBlockSize( SpiClientCtxt *,  uint32 * , uint32 olen1);
DALResult Spi_GetOutputBlockSize( SpiClientCtxt *,  uint32 * , uint32 olen1);
DALResult Spi_ConfigureDevice( SpiClientCtxt *,  const SpiDeviceInfoType * );
DALResult Spi_GetDeviceConfiguration( SpiClientCtxt *,  SpiDeviceInfoType * , uint32 olen1);
DALResult Spi_StartTransfer( SpiClientCtxt *,  SpiTransferType * ,  SpiDataAddrType* ,  int ,  SpiDataAddrType* ,  int ,  int* , uint32 olen6,  uint32* , uint32 olen7);
DALResult Spi_GetTransferStatus( SpiClientCtxt *,  uint32);
DALResult Spi_Send( SpiClientCtxt *,  SpiDataAddrType* ,  int );
DALResult Spi_Receive( SpiClientCtxt *,  uint32 ,  SpiDataAddrType* ,  int ,  int* , uint32 olen4);
DALResult Spi_SendAndReceive( SpiClientCtxt *,  SpiDataAddrType* ,  int ,  uint32 ,  SpiDataAddrType* ,  int ,  int* , uint32 olen6);
DALResult Spi_Reset( SpiClientCtxt *);
#endif /*DALSpi_H*/
