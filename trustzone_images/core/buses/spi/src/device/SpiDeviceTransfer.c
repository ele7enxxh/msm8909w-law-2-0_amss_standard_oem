/*
===========================================================================

FILE:   SpiDeviceTransfer.c

DESCRIPTION:

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/device/SpiDeviceTransfer.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
05/15/13 ag     Created.
===========================================================================
        Copyright c 2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "SpiDeviceTypes.h"
#include "SpiDevice.h"
#include "SpiDeviceTransfer.h"

#define SpiDeviceGetTransferDataResult(bytes, res)\
	switch(bytes){\
		case 4:\
			res = SPIDEVICE_TRANSFER_GET_DATA_SUCCESS;\
			break;\
		case 3:\
		case 2:\
			res = SPIDEVICE_TRANSFER_GET_DATA_PARTIAL;\
			break;\
		default:\
			res = SPIDEVICE_TRANSFER_GET_DATA_NONE;\
			break;\
	}
#define SpiDeviceSetTransferDataResult(bytes, res)\
	switch(bytes){\
		case 4:\
			res = SPIDEVICE_TRANSFER_SET_DATA_SUCCESS;\
			break;\
		case 3:\
		case 2:\
			res = SPIDEVICE_TRANSFER_SET_DATA_PARTIAL;\
			break;\
		default:\
			res = SPIDEVICE_TRANSFER_SET_DATA_NONE;\
			break;\
	}

static SpiDeviceTransferDataResult 
singleBufferGetNextByte
(
 SpiDeviceTransferType *transfer,
 uint8 *buf
 )
{
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_GET_DATA_NONE;
	if((sData->uWriteAvail > 0) && (sData->curWritePtr) && (buf)){
		*buf = *(sData->curWritePtr);
		sData->curWritePtr++;
		sData->uWriteAvail--;
		if(sData->uWriteAvail){
		res = SPIDEVICE_TRANSFER_GET_DATA_SUCCESS;
		}else{
			res = SPIDEVICE_TRANSFER_GET_DATA_DONE;
		}
	}
	return res;
}

static SpiDeviceTransferDataResult 
singleBufferGetNextWord
(
 SpiDeviceTransferType *transfer,
 uint32 *buf
 )
{
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_GET_DATA_NONE;
	uint8 *bytePtr = (uint8 *)buf;

	if((buf)&& (sData->curWritePtr)){
		if(sData->uWriteAvail >= 4) {
			*buf = *((uint32 *)sData->curWritePtr);
			sData->curWritePtr +=4;
			sData->uWriteAvail -=4;
			if(sData->uWriteAvail){
			res = SPIDEVICE_TRANSFER_GET_DATA_SUCCESS;
			}else{
				res = SPIDEVICE_TRANSFER_GET_DATA_DONE;
			}
		}
		else{
			*buf = 0;/*Get the available data, rest is padded to zero*/
			do {
				res = transfer->getNextByte(transfer, bytePtr++);
			}while(SPIDEVICE_TRANSFER_GET_DATA_SUCCESS == res );
		}
	}
	return res;
}

static SpiDeviceTransferDataResult 
singleBufferSetNextByte
(
 SpiDeviceTransferType *transfer,
 uint8 *buf
 )
{
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_SET_DATA_NONE;
	if((sData->uReadAvail > 0) && (sData->curReadPtr) && (buf)){
		*(sData->curReadPtr) = *buf;
		sData->curReadPtr++;
		sData->uReadAvail--;
		if(sData->uReadAvail){
		res = SPIDEVICE_TRANSFER_SET_DATA_SUCCESS;
		}else{
			res = SPIDEVICE_TRANSFER_SET_DATA_DONE;
		}
	}
	return res;
}

static SpiDeviceTransferDataResult 
singleBufferSetNextWord
(
 SpiDeviceTransferType *transfer,
 uint32 *buf
 )
{
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_SET_DATA_NONE;
	uint8 *bytePtr = (uint8 *)buf;

	if((buf) && (sData->curReadPtr)){
		if(sData->uReadAvail >= 4){
			*((uint32 *)sData->curReadPtr) = *buf;
			sData->curReadPtr +=4;
			sData->uReadAvail -=4;
			if(sData->uReadAvail){
			res = SPIDEVICE_TRANSFER_SET_DATA_SUCCESS;
			}else{
				res = SPIDEVICE_TRANSFER_SET_DATA_DONE;
			}
		}
		else{
			do {
				res = transfer->setNextByte(transfer, bytePtr++);
			}while(SPIDEVICE_TRANSFER_SET_DATA_SUCCESS == res );
		}
	}
	return res;
}
/*Single Buffers BAM level operations*/
static SpiDeviceTransferDataResult
singleBufferGetWrite
(
 SpiDeviceTransferType *transfer,
 uint8 **buf
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	if ((!sData->curWritePtr) || (!buf)){
		return res;
	}
	*buf = sData->curWritePtr;
	res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	return res;
}
static SpiDeviceTransferDataResult
singleBufferGetWritePhys
(
 SpiDeviceTransferType *transfer,
 uint8 **buf
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	if ((!sData->curWritePtrPhys) || (!buf)){
		return res;
	}
	*buf = sData->curWritePtrPhys;
	res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	return res;
}

static uint32
singleBufGetWriteLen
(
 SpiDeviceTransferType *transfer
)
{
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	return sData->uWriteAvail;
}

static SpiDeviceTransferDataResult
singleBufferMoveWriteNext
(
 SpiDeviceTransferType *transfer
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SPIDEVICE_UNREFERENCED_PARAMETER(transfer);
	return res;
}

static SpiDeviceTransferDataResult
singleBufferGetRead
(
 SpiDeviceTransferType *transfer,
 uint8 **buf
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	if ((!sData->curReadPtr) || (!buf)){
		return res;
	}
	*buf = sData->curReadPtr;
	res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	return res;
}

static SpiDeviceTransferDataResult
singleBufferGetReadPhys
(
 SpiDeviceTransferType *transfer,
 uint8 **buf
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	if ((!sData->curReadPtrPhys) || (!buf)){
		return res;
	}
	*buf = sData->curReadPtrPhys;
	res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	return res;
}

static uint32
singleBufGetReadLen
(
 SpiDeviceTransferType *transfer
)
{
	SpiDeviceSingleBuffData *sData = &(transfer->data.sData);
	return sData->uReadAvail;
}

static SpiDeviceTransferDataResult
singleBufferMoveReadNext
(
 SpiDeviceTransferType *transfer
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SPIDEVICE_UNREFERENCED_PARAMETER(transfer);
	return res;
}

void 
SpiDeviceSingleBufferTransferInit
(
 	SpiDeviceTransferType *transfer, 
 	uint8 *readBuf, 
 	uint8 *readBufPhys, 
 	uint32 uTotalReadLen, 
 	uint8 *writeBuf, 
 	uint8 *writeBufPhys, 
 	uint32 uTotalWriteLen 
)
{
	SpiDeviceSingleBuffData *sData;

	if(!transfer || !(readBuf || writeBuf)){
		return;
	}
	transfer->eDataType = SPIDEVICE_TRANSFER_SINGLE_BUFF_DATA;
	transfer->getNextWord = singleBufferGetNextWord;
	transfer->setNextWord = singleBufferSetNextWord;
	transfer->getNextByte = singleBufferGetNextByte;
	transfer->setNextByte = singleBufferSetNextByte;
	/*read */
	transfer->getReadBuf 		  =	singleBufferGetRead;
	transfer->getReadBufPhys      =	singleBufferGetReadPhys;
	transfer->getReadBufLen		  =	singleBufGetReadLen;
	transfer->moveReadNext	      =	singleBufferMoveReadNext;
	/*write*/
	transfer->getWriteBuf		  =	singleBufferGetWrite;
	transfer->getWriteBufPhys	  =	singleBufferGetWritePhys;
	transfer->getWriteBufLen	  =	singleBufGetWriteLen;
	transfer->moveWriteNext	      =	singleBufferMoveWriteNext;

	sData = &(transfer->data.sData);
	sData->curReadPtr = readBuf;
	sData->curReadPtrPhys = readBufPhys;
	sData->curWritePtr = writeBuf;
	sData->curWritePtrPhys = writeBufPhys;
	sData->uReadAvail = uTotalReadLen;
	sData->uWriteAvail = uTotalWriteLen;
}
