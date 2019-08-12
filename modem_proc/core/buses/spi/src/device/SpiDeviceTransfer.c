/*
===========================================================================

FILE:   SpiDeviceTransfer.c

DESCRIPTION:

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/device/SpiDeviceTransfer.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
07/14/12 ag     Fixed a bug pointer check, and fixed EA specific warnings.
03/28/12 ddk    Fixed a bug in setting write buffer pointer.
12/10/12 ag     Created.
===========================================================================
        Copyright c 2011 Qualcomm Technologies Incorporated.
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

/*Linked Buffers Byte Level Operations*/

static SpiDeviceTransferDataResult 
linkedBufferGetNextByte
(
 SpiDeviceTransferType *transfer,
 uint8 *buf
 )
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_GET_DATA_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);
    uint32 do_while_constant = 0;
	do{
		if(!lData->uCurWriteAvail){
			if (!lData->uWriteVectorAvail){
				break;
			}
			lData->curWriteVector++;
			lData->uWriteVectorAvail--;
			lData->curWritePtr = (uint8 *)lData->curWriteVector->pData->virtualAddr;
			lData->curWritePtrPhys = (uint8 *)lData->curWriteVector->pData->physicalAddr;
			lData->uCurWriteAvail = lData->curWriteVector->uLen;
		}
		if(!(lData->curWritePtr) || !(lData->uCurWriteAvail) || !(buf)){
			break;
		}
		*buf = *(lData->curWritePtr);
		lData->curWritePtr++;
		lData->uCurWriteAvail--;
		res = SPIDEVICE_TRANSFER_GET_DATA_SUCCESS;
	}while (do_while_constant);

	return res;
}


static SpiDeviceTransferDataResult 
linkedBufferGetNextWord
(
 SpiDeviceTransferType *transfer,
 uint32 *buf
 )
{
	uint8 *bytePtr = (uint8 *) buf;
	uint32 bytesGet;
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_GET_DATA_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);

	if((buf) && (lData->curWritePtr)){
		if(lData->uCurWriteAvail >= 4){
			if(lData->curWritePtr){
				*buf = *((uint32 *)lData->curWritePtr);
				lData->curWritePtr +=4;
				lData->uCurWriteAvail -= 4;
				res = SPIDEVICE_TRANSFER_GET_DATA_SUCCESS;
				bytesGet = 4;
			}
		} else {
			*buf = 0;/*Get the available data, rest is padded to zero*/
			do {
				res = transfer->getNextByte(transfer, bytePtr++);
				bytesGet = (uint32)bytePtr - (uint32)buf; 
			}while((SPIDEVICE_TRANSFER_GET_DATA_SUCCESS == res) && (bytesGet < 4));
			/*bytePtr gets incre mented regardless of whether there was another byte
			 * available. Therefore we check if the we got a byte the last time, and
			 * if we didn't we reduce bytesGet by 1.
			 */
			if(SPIDEVICE_TRANSFER_GET_DATA_NONE == res){
				bytesGet--;
			}
			SpiDeviceGetTransferDataResult(bytesGet,res);
		}
	}
	return res;
}

static SpiDeviceTransferDataResult 
linkedBufferSetNextByte
(
 SpiDeviceTransferType *transfer,
 uint8 *buf
 )
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_SET_DATA_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);
	uint32 do_while_constant = 0;

	do{
		if(!lData->uCurReadAvail){
			if (!lData->uReadVectorAvail){
				break;
			}
			lData->curReadVector++;
			lData->uReadVectorAvail--;
			lData->curReadPtr = (uint8 *)lData->curReadVector->pData->virtualAddr;
			lData->curReadPtrPhys = (uint8 *)lData->curReadVector->pData->physicalAddr;
			lData->uCurReadAvail = lData->curReadVector->uLen;
		}
		if(!(lData->curReadPtr) || !(lData->uCurReadAvail) || !(buf)){
			break;
		}
		*(lData->curReadPtr) = *buf;
		lData->curReadPtr++;
		lData->uCurReadAvail--;
		res = SPIDEVICE_TRANSFER_SET_DATA_SUCCESS;
	}while (do_while_constant);
	return res;
}


static SpiDeviceTransferDataResult 
linkedBufferSetNextWord
(
 SpiDeviceTransferType *transfer,
 uint32 *buf
 )
{
	uint8 *bytePtr = (uint8 *) buf;
	uint32 bytesSet = 0;
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_SET_DATA_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);

	if((buf) && (lData->curReadPtr)){
		if(lData->uCurReadAvail >= 4){
			if(lData->curReadPtr){
				*((uint32 *)lData->curReadPtr) = *buf;
				lData->curReadPtr +=4;
				lData->uCurReadAvail -= 4;
				res = SPIDEVICE_TRANSFER_SET_DATA_SUCCESS;
				bytesSet = 4;
			}
		} else {
			do {
				res = transfer->setNextByte(transfer, bytePtr++);
				bytesSet = (uint32)bytePtr - (uint32)buf;
			}while((SPIDEVICE_TRANSFER_SET_DATA_SUCCESS == res) && (bytesSet < 4) );
			/*BytePtr is set to buffer to start with if we atleast got one
			 * valid byte then bytePtr - buf will be > 1*/
			/*bytePtr gets incremented regardless of whether there was another byte
			 * available. Therefore we check if the we got a byte the last time, and
			 * if we didn't we reduce bytesGet by 1.
			 */
			if(SPIDEVICE_TRANSFER_GET_DATA_NONE == res){
				bytesSet--;
			}
			SpiDeviceSetTransferDataResult(bytesSet,res);
		}
	}
	return res;
}

/*Linked Buffers BAM level operations*/

static SpiDeviceTransferDataResult
linkedBufferGetWriteBuf
(
 SpiDeviceTransferType *transfer,
 uint8 **buf
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);

	if ((!lData->curWritePtr) || (!buf)){
		return res;
	}
	*buf = lData->curWritePtr;
	res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	return res;
}

static SpiDeviceTransferDataResult
linkedBufferGetWriteBufPhys
(
 SpiDeviceTransferType *transfer,
 uint8 **buf
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);

	if ((!lData->curWritePtrPhys) || (!buf)){
		return res;
	}
	*buf = lData->curWritePtrPhys;
	res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	return res;
}

static uint32
linkedBufGetWriteLen
(
 SpiDeviceTransferType *transfer
)
{
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);
	return lData->uCurWriteAvail;
}

static SpiDeviceTransferDataResult
linkedBufferMoveWriteNext
(
 SpiDeviceTransferType *transfer
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);
	uint32 do_while_constant = 0;

	do{
		if (!lData->uWriteVectorAvail){
			lData->curWritePtr = (uint8 *)NULL;
			lData->curWritePtrPhys = (uint8 *)NULL;
			lData->uCurWriteAvail = 0;
			break;
		}
		lData->curWriteVector++;
		lData->uWriteVectorAvail--;
		lData->curWritePtr = (uint8 *)lData->curWriteVector->pData->virtualAddr;
		lData->curWritePtrPhys = (uint8 *)lData->curWriteVector->pData->physicalAddr;
		lData->uCurWriteAvail = lData->curWriteVector->uLen;
		res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	}while(do_while_constant);

	return res;
}

static SpiDeviceTransferDataResult
linkedBufferGetReadBuf
(
 SpiDeviceTransferType *transfer,
 uint8 **buf
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);

	if ((!lData->curReadPtr) || (!buf)){
		return res;
	}
	*buf = lData->curReadPtr;
	res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	return res;
}

static SpiDeviceTransferDataResult
linkedBufferGetReadBufPhys
(
 SpiDeviceTransferType *transfer,
 uint8 **buf
)
{
	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);

	if ((!lData->curReadPtrPhys) || (!buf)){
		return res;
	}
	*buf = lData->curReadPtrPhys;
	res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	return res;
}

static uint32
linkedBufGetReadLen
(
 SpiDeviceTransferType *transfer
)
{
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);
	return lData->uCurReadAvail;
}
static SpiDeviceTransferDataResult
linkedBufferMoveReadNext
(
 SpiDeviceTransferType *transfer
)
{

	SpiDeviceTransferDataResult res = SPIDEVICE_TRANSFER_BUFF_NONE;
	SpiDeviceLinkedBuffData *lData = &(transfer->data.lData);
	uint32 do_while_constant = 0;

	do{
		if (!lData->uReadVectorAvail){
			lData->curReadPtr = (uint8 *)NULL;
			lData->curReadPtrPhys = (uint8 *)NULL;
			lData->uCurReadAvail = 0;
			break;
		}
		lData->curReadVector++;
		lData->uReadVectorAvail--;
		lData->curReadPtr = (uint8 *)lData->curReadVector->pData->virtualAddr;
		lData->curReadPtrPhys = (uint8 *)lData->curReadVector->pData->physicalAddr;
		lData->uCurReadAvail = lData->curReadVector->uLen;
		res = SPIDEVICE_TRANSFER_BUFF_AVAILABLE;
	}while(do_while_constant);

	return res;
}

void 
SpiDeviceLinkedBufferTransferInit
(
	SpiDeviceTransferType *transfer, 
	SpiBufferType *readVector,
	uint32 uNumReadVecs,
	SpiBufferType *writeVector,
	uint32 uNumWriteVecs
)
{
	SpiDeviceLinkedBuffData *lData;
	if(!transfer || !(readVector || writeVector)){
		return;
	}
	lData = &(transfer->data.lData);

	transfer->eDataType = SPIDEVICE_TRANSFER_LINKED_BUFF_DATA;
	transfer->getNextWord = linkedBufferGetNextWord;
	transfer->setNextWord = linkedBufferSetNextWord;
	transfer->getNextByte = linkedBufferGetNextByte;
	transfer->setNextByte = linkedBufferSetNextByte;
	/*read*/
	transfer->getReadBuf	      =	linkedBufferGetReadBuf;
	transfer->getReadBufPhys      =	linkedBufferGetReadBufPhys;
	transfer->moveReadNext		  =	linkedBufferMoveReadNext;
	transfer->getReadBufLen		  =	linkedBufGetReadLen;
	/*write*/
	transfer->getWriteBuf		  =	linkedBufferGetWriteBuf;
	transfer->getWriteBufPhys	  =	linkedBufferGetWriteBufPhys;
	transfer->moveWriteNext		  =	linkedBufferMoveWriteNext;
	transfer->getWriteBufLen	  =	linkedBufGetWriteLen;

    if(readVector != NULL){
        lData->curReadVector = readVector;
        lData->curReadPtr = (uint8 *)readVector->pData->virtualAddr;
        lData->curReadPtrPhys = (uint8 *)readVector->pData->physicalAddr;
        lData->uCurReadAvail = readVector->uLen;
        lData->uReadVectorAvail = uNumReadVecs;
    }

    if(writeVector != NULL){
        lData->curWriteVector = writeVector;
        lData->curWritePtr = (uint8 *)writeVector->pData->virtualAddr;
        lData->curWritePtrPhys = (uint8 *)writeVector->pData->physicalAddr;
        lData->uCurWriteAvail = writeVector->uLen;
        lData->uWriteVectorAvail = uNumWriteVecs;
    }
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
