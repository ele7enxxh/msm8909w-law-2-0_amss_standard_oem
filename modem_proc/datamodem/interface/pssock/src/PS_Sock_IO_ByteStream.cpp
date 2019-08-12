/*===========================================================================
  FILE: PS_Sock_IO_ByteStream.cpp

  OVERVIEW: This file extends implementation of SocketIO abstract class to
  support a data pipe which doesn't maintain message boundaries. This class is
  used by TCP sockets.

  DEPENDENCIES: None

  Copyright (c) 2008 - 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_IO_ByteStream.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-04-11 ppk Drop packets if App no longer wish to read packets.
                 ReleaseReader: Explicit setting of dsmItemQTailPtr to 0 to
                 avoid DSM corruption.
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "dsm.h"
#include "ps_mem.h"
#include "PS_Sock_IO_ByteStream.h"
#include "ds_Utils_DebugMsg.h"
#include "dserrno.h"


using namespace ds::Sock;
using namespace PS::Sock::IO;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void * ByteStream::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_TCP_BYTE_STREAM_TYPE);
} /* ByteStream::operator new() */


ByteStream::ByteStream
(
  ICritSect *  _critSectPtr
) :
    SocketIO( _critSectPtr),
    dsmItemQHeadPtr( 0),
    dsmItemQTailPtr( 0),
    lowWMSize( ByteStream::defaultLowWMSize)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
} /* ByteStream::ByteStream() */


void ByteStream::operator delete
(
  void *  bufPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_MEM_FREE( bufPtr);
  return;

} /* ByteStream::operator delete() */


ByteStream::~ByteStream
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1("delete(): "
                  "Obj 0x%x", this);

  /*lint -save -e1550, -e1551 */
  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Free remaining items in the Q
  -------------------------------------------------------------------------*/
  dsm_free_packet( &dsmItemQHeadPtr);
  dsmItemQTailPtr = 0;

  critSectPtr->Leave();
  /*lint -restore */

  return;
} /* ByteStream::~ByteStream() */


int32 ByteStream::Write
(
  const IPort1::SeqBytes  ioVecArr[],
  int32                   numIOVec,
  char *                  metaDataPtr,
  int32                   metaDataLen,
  int16 *                 psErrnoPtr
)
{
  int32  byteCount;
  int32  tmpSize;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Write(): "
                    "NULL errno");
    return -1;
  }

  if (0 != metaDataPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Write(): "
                    "Non-NULL meta data");
    *psErrnoPtr = DS_EOPNOTSUPP;
    return -1;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Data can't be written if stream is closed or if shutdown for writes
  -------------------------------------------------------------------------*/
  if (WRITE_SHUTDOWN == state || CLOSED == state)
  {
    LOG_MSG_ERROR_1("ByteStream::Write(): "
                    "Can't write, state %d", state);
    *psErrnoPtr = DS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Copy buffer in to the DSM item Q
  -------------------------------------------------------------------------*/
  byteCount = CopyBufToDSM( ioVecArr,
                            numIOVec,
                            &dsmItemQHeadPtr,
                            &dsmItemQTailPtr,
                            psErrnoPtr);
  if (-1 == byteCount)
  {
    LOG_MSG_INFO3_1("ByteStream::Write(): "
                    "CopyBufToDSM failed, err %d", *psErrnoPtr);
    goto bail;
  }
  else
  {
    /*-----------------------------------------------------------------------
      If data is partially copied because SndQ is full, psErrno is set to
      EWOULDBLOCK, but for TCP it is not an error and hence reset psErrno. If
      psErrno is not reset, sockets library would return EWOULDBLOCK to the
      application which will treat it as an error
    -----------------------------------------------------------------------*/
    *psErrnoPtr = DS_ENOERR;

    /*-----------------------------------------------------------------------
      0 or more bytes are successfully copied
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Free DSM item if stream is in READ_SHUTDOWN state. As user is not gonna
      read data anymore, there is no need to keep DSM item in the queue.

      This chould have been checked prior to copying data, but wanted to make
      writer agnostic to whether reader has closed its side. If CopyBufToDSM
      wasn't called, byteCount is not known and the only option was to return
      0 in which case writer may freak out
    -----------------------------------------------------------------------*/
    if (READ_SHUTDOWN == state)
    {
      dsm_free_packet( &dsmItemQHeadPtr);
    }
    else
    {
      /*---------------------------------------------------------------------
        Reader is present and check if it needs to be notified
      ---------------------------------------------------------------------*/
      if (0 < byteCount)
      {
        /*-------------------------------------------------------------------
          Notify IReaderListener if the Q was empty prior to the write
        -------------------------------------------------------------------*/
        tmpSize   = currSize;
        currSize += byteCount;

        if (0 == tmpSize && 0 != readerListenerPtr)
        {
          readerListenerPtr->DataAvailable();
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Do nothing. Ideally DSM item must not be allocated if 0 client
          writes 0 bytes to a stream. The two cases in which a DSM item is
          allocated are
            1. Stream is empty
            2. Stream is not empty but dsmItemQTailPtr is completely filled

          It's easy to handle the first case (delete dsmItemQHeadPtr) but
          handling second case is not straightforward (delete dsmItemQTailPtr
          and update dsmItemQTailPtr to previous DSM item in chain).
          Writing 0 bytes is not a common case and even if it happens, since
          bytes are always packed, DSM item will not go waste
        -------------------------------------------------------------------*/
      }
    }
  }

  critSectPtr->Leave();
  return byteCount;

  /*-------------------------------------------------------------------------
    Common error handling code

    Leave critical section and return error
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return -1;

} /* ByteStream::Write() */


int32 ByteStream::Write
(
  dsm_item_type **  dsmItemPtrPtr,
  int32             bufLen,
  char *            metaDataPtr,
  int32             metaDataLen,
  int16 *           psErrnoPtr
)
{
  dsm_item_type *  itemTailPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Write(): "
                    "NULL errno");
    return -1;
  }

  if (0 == dsmItemPtrPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Write(): "
                    "NULL DSM item");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  if (0 != metaDataPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Write(): "
                    "Non-NULL meta data");
    *psErrnoPtr = DS_EOPNOTSUPP;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Return success as peer could send a 0 length packet
  -------------------------------------------------------------------------*/
  if (0 == *dsmItemPtrPtr || 0 == bufLen)
  {
    return 0;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Data can't be written if stream is closed or if shutdown for writes
  -------------------------------------------------------------------------*/
  if (WRITE_SHUTDOWN == state || CLOSED == state)
  {
    LOG_MSG_ERROR_1("ByteStream::Write(): "
                    "Can't write, state %d", state);
    *psErrnoPtr = DS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Free DSM item if stream is in READ_SHUTDOWN state since user is not
    going to read data anymore
  -------------------------------------------------------------------------*/
  if (READ_SHUTDOWN == state)
  {
    LOG_MSG_INFO1_1("ByteStream::Write(): "
                    "Dropping packet, state %d", state);
    dsm_free_packet( dsmItemPtrPtr);
  }
  else
  {
#ifdef FEATURE_DATA_PS_DEBUG
    /*-----------------------------------------------------------------------
      Safety check to make sure that the correct length is passed
    -----------------------------------------------------------------------*/
    ASSERT( bufLen == dsm_length_packet( *dsmItemPtrPtr));
#endif /* FEATURE_DATA_PS_DEBUG */

    /*-----------------------------------------------------------------------
      Update current amount of data in the stream
    -----------------------------------------------------------------------*/
    currSize += bufLen;

    /*-----------------------------------------------------------------------
      Update head and tail pointers of Stream
        1. If this is the first item in Q, head is updated to point to the
           item. Otherwise head is not updated
        2. Tail of the Q is updated with the tail of the new item
    -----------------------------------------------------------------------*/
    itemTailPtr = *dsmItemPtrPtr;
    while (0 != itemTailPtr->pkt_ptr)
    {
      itemTailPtr = itemTailPtr->pkt_ptr;
    }

    if (0 == dsmItemQTailPtr)
    {
      dsmItemQHeadPtr = *dsmItemPtrPtr;
      dsmItemQTailPtr = itemTailPtr;

      /*---------------------------------------------------------------------
        Tail is 0 only when there is no data in the stream. So, notify the
        client that it can read data now as it is a 0 to non-0 transition
      ---------------------------------------------------------------------*/
      if (0 != readerListenerPtr)
      {
        readerListenerPtr->DataAvailable();
      }
    }
    else
    {
      dsmItemQTailPtr->pkt_ptr = *dsmItemPtrPtr;
      dsmItemQTailPtr          = itemTailPtr;
    }
  }

  critSectPtr->Leave();

  *dsmItemPtrPtr = 0;
  return bufLen;

  /*-------------------------------------------------------------------------
    Common error handling code

    Leave critical section and return error
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return -1;

} /* ByteStream::Write() */


void ByteStream::SetSndBufSize
(
  int32  size
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("ByteStream::SetSndBufSize(): "
                           "Stream 0x%x size %d", this, size);

  critSectPtr->Enter();

  SocketIO::SetSndBufSize( size);
  lowWMSize = maxSize * SocketIO::lowWMSizePercentage / 100;

  critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_1("ByteStream::SetSndBufSize(): "
                          "Success, stream 0x%x", this);
  return;

} /* ByteStream::SetSndBufSize() */


int32 ByteStream::SetWrCritSection
(
  ICritSect * newCritSectPtr,
  int16 *     psErrnoPtr
)
{
  ICritSect * tmpCritSectPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("ByteStream::SetWrCritSection(): "
                           "Stream 0x%x CS 0x%x", this, newCritSectPtr);

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::SetWrCritSection(): "
                    "NULL errno");
    return -1;
  }

  if (0 == newCritSectPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::SetWrCritSection(): "
                    "NULL CS");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  tmpCritSectPtr = critSectPtr;

  tmpCritSectPtr->Enter();
  critSectPtr = newCritSectPtr;
  tmpCritSectPtr->Leave();

  (void) critSectPtr->AddRef();
  (void) tmpCritSectPtr->Release();

  LOG_MSG_FUNCTION_EXIT_1("ByteStream::SetWrCritSection(): "
                          "Success, stream 0x%x", this);
  return 0;

} /* ByteStream::SetWrCritSection() */


int32 ByteStream::Read
(
  IPort1::SeqBytes  ioVecArr[],
  int32             numIOVec,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
)
{
  int32  idx;
  int32  byteCount = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Read(): "
                    "NULL errno");
    return -1;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Data can't be read if stream is closed or if shutdown for reads
  -------------------------------------------------------------------------*/
  if (READ_SHUTDOWN == state || CLOSED == state)
  {
    LOG_MSG_ERROR_1("ByteStream::Read(): "
                    "Can't read, state %d", state);
    *psErrnoPtr = DS_EINVAL;
    byteCount   = -1;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Return EEOF/EWOULDBLOCK if stream is empty
  -------------------------------------------------------------------------*/
  if (0 == currSize)
  {
    if (WRITE_SHUTDOWN == state)
    {
      LOG_MSG_INFO1_0("ByteStream::Read(): "
                      "Q is WR_SHUTDOWN");
      *psErrnoPtr = DS_EEOF;
      byteCount   = 0;
    }
    else
    {
      LOG_MSG_INFO1_0("ByteStream::Read(): "
                      "Empty Q");
      *psErrnoPtr = DS_EWOULDBLOCK;
      byteCount   = -1;
    }

    goto bail;
  }

  /*-------------------------------------------------------------------------
    Extract data from DSM item Q in to the IOVec buffers
  -------------------------------------------------------------------------*/
  for (idx = 0; idx < numIOVec; idx++)
  {
    byteCount +=
      dsm_pullup_long( &dsmItemQHeadPtr, ioVecArr[idx].data, ioVecArr[idx].dataLen);
  }

  /*-------------------------------------------------------------------------
    Update the currSize
  -------------------------------------------------------------------------*/
  DecCurrSize( byteCount);

  critSectPtr->Leave();

  /*-------------------------------------------------------------------------
    Since meta data is not stored for streams, set length to 0
  -------------------------------------------------------------------------*/
  if (0 != metaDataLenPtr)
  {
    *metaDataLenPtr = 0;
  }

  *psErrnoPtr = DS_ENOERR;
  return byteCount;

  /*-------------------------------------------------------------------------
    Common error handling code

    Leave critical section and return error
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return byteCount;

} /* ByteStream::Read() */


int32 ByteStream::Read
(
  dsm_item_type **  dsmItemPtrPtr,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
)
{
  int32  tmpSize;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Read(): "
                    "NULL errno");
    return -1;
  }

  if (0 == dsmItemPtrPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Read(): "
                    "NULL DSM item");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Data can't be read if stream is closed or if shutdown for reads
  -------------------------------------------------------------------------*/
  if (READ_SHUTDOWN == state || CLOSED == state)
  {
    LOG_MSG_ERROR_1("ByteStream::Read(): "
                    "Can't read, state %d", state);
    *psErrnoPtr = DS_EINVAL;
    tmpSize     = -1;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Return EEOF/EWOULDBLOCK if stream is empty
  -------------------------------------------------------------------------*/
  if (0 == currSize)
  {
    if (WRITE_SHUTDOWN == state)
    {
      LOG_MSG_INFO1_0("ByteStream::Read(): "
                      "Q is WR_SHUTDOWN");
      *psErrnoPtr = DS_EEOF;
      tmpSize     = 0;
    }
    else
    {
      LOG_MSG_INFO1_0("ByteStream::Read(): "
                      "Empty Q");
      *psErrnoPtr = DS_EWOULDBLOCK;
      tmpSize     = -1;
    }

    goto bail;
  }

  *dsmItemPtrPtr  = dsmItemQHeadPtr;

  /*-------------------------------------------------------------------------
    Update the currSize
  -------------------------------------------------------------------------*/
  tmpSize = currSize;
  DecCurrSize( currSize);

  critSectPtr->Leave();

  /*-------------------------------------------------------------------------
    Since meta data is not stored for streams, set length to 0
  -------------------------------------------------------------------------*/
  if (0 != metaDataLenPtr)
  {
    *metaDataLenPtr = 0;
  }

  *psErrnoPtr = DS_ENOERR;
  return tmpSize;

  /*-------------------------------------------------------------------------
    Common error handling code

    Leave critical section and return error
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return tmpSize;

} /* ByteStream::Read() */


int32 ByteStream::Peek
(
  dsm_item_type **  dsmItemPtrPtr,
  int32             numBytesToPeek,
  int32             offsetToPeekAt,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
)
{
  dsm_item_type *           itemPtr;
  dsm_item_type *           pktHeadPtr = 0;
  dsm_item_type *           pktTailPtr = 0;
  int32                     toPeek;
  int32                     numPeeked;
  int32                     numExtracted;
  uint16                    offsetToWriteAt = 40;
  dsm_mempool_id_enum_type  poolID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Peek(): "
                    "NULL errno");
    return -1;
  }

  if (0 == dsmItemPtrPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::Peek(): "
                    "NULL DSM item");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Data can't be read if stream is closed or if shutdown for reads
  -------------------------------------------------------------------------*/
  if (READ_SHUTDOWN == state || CLOSED == state)
  {
    LOG_MSG_ERROR_1("ByteStream::Peek(): "
                    "Can't read, state %d", state);
    *psErrnoPtr = DS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Since meta data is not stored for streams, set length to 0
  -------------------------------------------------------------------------*/
  if (0 != metaDataLenPtr)
  {
    *metaDataLenPtr = 0;
  }

  /*-------------------------------------------------------------------------
    Allocate a DSM item and copy data from the queue. If the data doesn't fit
    in one DSM item, allocate as many DSM items as needed and chain them
  -------------------------------------------------------------------------*/
  numPeeked = 0;
  while (numPeeked < numBytesToPeek)
  {
    /*-----------------------------------------------------------------------
      Determine whether to allocate a small DSM item or large DSM item based
      on the amount of data to be copied
    -----------------------------------------------------------------------*/
    poolID = DSM_DS_POOL_SIZE( numBytesToPeek - numPeeked + offsetToWriteAt);

    itemPtr = dsm_offset_new_buffer( poolID, offsetToWriteAt);
    if (0 == itemPtr)
    {
      LOG_MSG_ERROR_0("ByteStream::Peek(): "
                      "Out of DSM items");
		  *psErrnoPtr = DS_ENOMEM;
      goto bail;
    }

    /*-----------------------------------------------------------------------
      Copy data in to the DSM item
    -----------------------------------------------------------------------*/
    toPeek       = MIN( itemPtr->size, numBytesToPeek - numPeeked);
    numExtracted = dsm_extract_long( dsmItemQHeadPtr,
                                     offsetToPeekAt,
                                     itemPtr->data_ptr,
                                     toPeek);

    numPeeked      += numExtracted;
    offsetToPeekAt += numExtracted;
    itemPtr->used   = static_cast <uint16> ( numExtracted);

    /*-----------------------------------------------------------------------
      Reset offsetToWriteAt as space needs to be reserved only in the first
      DSM item so that TCP/UDP/IP headers can be prepended
    -----------------------------------------------------------------------*/
    offsetToWriteAt = 0;

    /*-----------------------------------------------------------------------
      Form a packet chain. Note that we avoid calling dsm_append() for
      optimization reasons.
    -----------------------------------------------------------------------*/
    if (0 == pktHeadPtr)
    {
      pktHeadPtr = itemPtr;
      pktTailPtr = itemPtr;
    }
    else
    {
      pktTailPtr->pkt_ptr = itemPtr;
      pktTailPtr          = itemPtr;
    }

    //TODO Take another look at this func and try to avoid this check
    if (toPeek != numExtracted)
    {
      break;
    }
  }

  *dsmItemPtrPtr = pktHeadPtr;

  critSectPtr->Leave();
  return numPeeked;

  /*-------------------------------------------------------------------------
    Common error handling code

    Leave critical section and since the requested amount of data can't be
    copied, free the data that is already copied and return error
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  dsm_free_packet( &pktHeadPtr);
  return -1;

} /* ByteStream::Peek() */


int32 ByteStream::FreeBytes
(
  int32    numBytesToFree,
  int16 *  psErrnoPtr
)
{
  int32  numBytesPulledUp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::FreeBytes(): "
                    "NULL errno");
    return -1;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Remove bytes from DSM item Q
  -------------------------------------------------------------------------*/
  numBytesPulledUp =
    dsm_pullup_long( &dsmItemQHeadPtr, 0, numBytesToFree);

#ifdef FEATURE_DATA_PS_DEBUG
  ASSERT( numBytesToFree == numBytesPulledUp);
#endif /* FEATURE_DATA_PS_DEBUG */

  /*-------------------------------------------------------------------------
    Update the currSize
  -------------------------------------------------------------------------*/
  DecCurrSize( numBytesPulledUp);

  critSectPtr->Leave();
  return 0;

} /* ByteStream::FreeBytes() */


int32 ByteStream::GetPktCnt
(
  uint16 *  pktCntPtr,
  int16 *   psErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return MethodNotSupported( psErrnoPtr);
} /* ByteStream::GetPktCnt() */


int32 ByteStream::SetRdCritSection
(
  ICritSect * newCritSectPtr,
  int16 *     psErrnoPtr
)
{
  ICritSect * tmpCritSectPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("ByteStream::SetRdCritSection(): "
                           "Stream 0x%x CS 0x%x", this, newCritSectPtr);

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::SetRdCritSection(): "
                    "NULL errno");
    return -1;
  }

  if (0 == newCritSectPtr)
  {
    LOG_MSG_ERROR_0("ByteStream::SetRdCritSection(): "
                    "NULL CS");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  tmpCritSectPtr = critSectPtr;

  tmpCritSectPtr->Enter();
  critSectPtr = newCritSectPtr;
  tmpCritSectPtr->Leave();

  (void) critSectPtr->AddRef();
  (void) tmpCritSectPtr->Release();

  LOG_MSG_FUNCTION_EXIT_1("ByteStream::SetRdCritSection(): "
                          "Success, stream 0x%x", this);
  return 0;

} /* ByteStream::SetRdCritSection() */


void ByteStream::ReleaseReader
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("ByteStream::ReleaseReader(): "
                           "Stream 0x%x", this);

  critSectPtr->Enter();  /*lint !e1550 */

  /*-------------------------------------------------------------------------
    Since reader is released, there is no need to maintain DSM items in the Q.
    No body is going to read them anyway.
  -------------------------------------------------------------------------*/
  dsm_free_packet( &dsmItemQHeadPtr);
  dsmItemQTailPtr = 0;

  /*-------------------------------------------------------------------------
    Leave critical section before calling SocketIO's ReleaseReader. Otherwise,
    lock will not be released if object is destructed in
    SocketIO's ReleaseReader as once the object is destroyed, its members are
    not accessible
  -------------------------------------------------------------------------*/
  critSectPtr->Leave();  /*lint !e1550 */

  /*-------------------------------------------------------------------------
    Call the base class's method so that state is changed appropriately
  -------------------------------------------------------------------------*/
  SocketIO::ReleaseReader();

  LOG_MSG_FUNCTION_EXIT_1("ByteStream::ReleaseReader(): "
                          "Success, stream 0x%x", this);
  return;

} /* ByteStream::ReleaseReader() */

