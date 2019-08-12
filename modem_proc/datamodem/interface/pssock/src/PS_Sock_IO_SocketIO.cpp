/*===========================================================================
  FILE: PS_Sock_IO_SocketIO.cpp

  OVERVIEW: This abstract class provides implementation of the basic methods
  of IReader and IWriter interfaces.

  DEPENDENCIES: None

  Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_IO_SocketIO.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include <stringl.h>

#include "dsm.h"
#include "dserrno.h"
#include "PS_Sock_IO_SocketIO.h"
#include "ds_Utils_DebugMsg.h"


using namespace ds::Sock;
using namespace PS::Sock::IO;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
SocketIO::SocketIO
(
  ICritSect *  _critSectPtr
) :
    readerListenerPtr( 0),
    writerListenerPtr( 0),
    critSectPtr( _critSectPtr),
    maxSize( 0x7FFFFFFF),
    currSize( 0),
    state( OPEN),
    isHighWMHit( false)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) critSectPtr->AddRef();
  return;

} /* SocketIO::SocketIO() */


SocketIO::~SocketIO
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1( "SocketIO::~SocketIO(): "
                   "Obj 0x%x", this);

  /*lint -save -e1550, -e1551 */
  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Reset state to initial state so that code ASSERTs if client accesses the
    object after it is deleted,
  -------------------------------------------------------------------------*/
  state             = CLOSED;
  currSize          = 0;
  readerListenerPtr = 0;
  writerListenerPtr = 0;

  critSectPtr->Leave();

  (void) critSectPtr->Release();
  critSectPtr = 0;
  /*lint -restore */

  return;
} /* SocketIO::~SocketIO() */


void SocketIO::ReleaseReader
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("SocketIO::ReleaseReader(): "
                  "In state %d, SocketIO 0x%x", state, this);

  critSectPtr->Enter(); /*lint !e1550 */

  switch (state)
  {
    case CLOSED:
    case READ_SHUTDOWN:
    {
      LOG_MSG_ERROR_1("SocketIO::ReleaseReader(): "
                      "Already freed, SocketIO 0x%x", this);
      ASSERT(0);
      break;
    }

    case OPEN:
    {
      LOG_MSG_INFO1_2("SocketIO::ReleaseReader(): "
                      "New state %d, SocketIO 0x%x", READ_SHUTDOWN, this);
      state = READ_SHUTDOWN;
      break;
    }

    case WRITE_SHUTDOWN:
    {
      /*---------------------------------------------------------------------
        Both reader and writer are close. So delete the stream

        Leave the critical section before deleting the object as critical
        section object will be destructed as part of SocketIO destruction
      ---------------------------------------------------------------------*/
      critSectPtr->Leave(); /*lint !e1550 */
      delete this;
      return;
    }

    default:
    {
      LOG_MSG_ERROR_2("SocketIO::ReleaseReader(): "
                      "Unknown state %d, SocketIO 0x%x", state, this);
      ASSERT( 0);
      break;
    }
  }

  critSectPtr->Leave(); /*lint !e1550 */

  LOG_MSG_FUNCTION_EXIT_1("SocketIO::ReleaseReader(): "
                          "Success, SocketIO 0x%x", this);
  return;

} /* SocketIO::ReleaseReader() */


void SocketIO::ReleaseWriter
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("SocketIO::ReleaseWriter(): "
                  "In state %d, SocketIO 0x%x", state, this);


  critSectPtr->Enter(); /*lint !e1550 */

  switch (state)
  {
    case CLOSED:
    case WRITE_SHUTDOWN:
    {
      LOG_MSG_ERROR_1("SocketIO::ReleaseWriter(): "
                      "Already freed, SocketIO 0x%x", this);
      ASSERT( 0);
      break;
    }

    case OPEN:
    {
      LOG_MSG_INFO1_2("SocketIO::ReleaseWriter(): "
                      "New state %d, SocketIO 0x%x", WRITE_SHUTDOWN, this);
      state = WRITE_SHUTDOWN;

      /*---------------------------------------------------------------------
        Notify the readerListener so that it tries to read data and gets an
        appropriate error
      ---------------------------------------------------------------------*/
      if (0 != readerListenerPtr)
      {
        readerListenerPtr->DataAvailable();
      }

      break;
    }

    case READ_SHUTDOWN:
    {
      /*---------------------------------------------------------------------
        Both reader and writer are close. So delete the stream

        Leave the critical section before deleting the object as critical
        section object will be destructed as part of SocketIO destruction
      ---------------------------------------------------------------------*/
      critSectPtr->Leave(); /*lint !e1550 */
      delete this;
      return;
    }

    default:
    {
      LOG_MSG_ERROR_2("SocketIO::ReleaseWriter(): "
                      "Unknown state %d, SocketIO 0x%x", state, this);
      ASSERT(0);
      break;
    }
  }

  critSectPtr->Leave(); /*lint !e1550 */

  LOG_MSG_FUNCTION_EXIT_1("SocketIO::ReleaseWriter(): "
                          "Success, SocketIO 0x%x", this);
  return;

} /* SocketIO::ReleaseWriter() */


void SocketIO::SetSndBufSize
(
  int32  size
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("SocketIO::SetSndBufSize(): "
                           "SocketIO 0x%x size %d", this, size);

  critSectPtr->Enter();
  maxSize = size;
  critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_1("SocketIO::SetSndBufSize(): "
                          "Success, SocketIO 0x%x", this);
  return;

} /* SocketIO::SetSndBufSize() */


void SocketIO::SetRcvBufSize
(
  int32  size
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("SocketIO::SetRcvBufSize(): "
                           "SocketIO 0x%x size %d", this, size);

  critSectPtr->Enter();
  maxSize = size;
  critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_1("SocketIO::SetRcvBufSize(): "
                          "Success, SocketIO 0x%x", this);
  return;

} /* SocketIO::SetRcvBufSize() */


void SocketIO::RegisterReaderListener
(
  IReaderListener *  newReaderListenerPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("SocketIO::RegisterReaderListener(): "
                           "SocketIO 0x%x listener 0x%x",
                           this, newReaderListenerPtr);

  critSectPtr->Enter();
  readerListenerPtr = newReaderListenerPtr;
  critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_1("SocketIO::RegisterReaderListener(): "
                          "Success, SocketIO 0x%x", this);
  return;

} /* SocketIO::RegisterReaderListener() */


void SocketIO::RegisterWriterListener
(
  IWriterListener *  newWriterListenerPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("SocketIO::RegisterWriterListener(): "
                           "SocketIO 0x%x listener 0x%x",
                           this, newWriterListenerPtr);

  critSectPtr->Enter();
  writerListenerPtr = newWriterListenerPtr;
  critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_1("SocketIO::RegisterWriterListener(): "
                          "Success, SocketIO 0x%x", this);
  return;

} /* SocketIO::RegisterWriterListener() */


/*===========================================================================

                       PROTECTED MEMBER FUNCTIONS

===========================================================================*/
int32 SocketIO::CopyBufToDSM
(
  const IPort1::SeqBytes       ioVecArr[],
  int32                        numIOVec,
  dsm_item_type **             itemHeadPtrPtr,
  dsm_item_type **             itemTailPtrPtr,
  int16 *                      psErrnoPtr
)
{
  dsm_item_type *  dsmItemPtr;
  const byte *     addrToCopyFrom;
  byte *           addrToCopyInTo;
  int32            idx;
  int32            numBytesToCopyToDSMItem = 0;
  int32            numBytesToCopyFromIOVec;
  int32            numLeftOverBytesInDSMItem;
  int32            numCopiedBytes = 0;
  int32            remainingSize;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( 0 != itemHeadPtrPtr && 0 != itemTailPtrPtr && 0 != psErrnoPtr);

  /*-------------------------------------------------------------------------
    Filling errno with grabage so that its value doesn't conflict with
    valid error values
  -------------------------------------------------------------------------*/
  *psErrnoPtr = DS_ENOERR;

  /*-------------------------------------------------------------------------
    Can't write to the data pipe if it is full already
  -------------------------------------------------------------------------*/
  if (maxSize == currSize)
  {
    LOG_MSG_INFO1_1("SocketIO::CopyBufToDSM(): "
                    "Q 0x%x full", this);
    isHighWMHit = true;
    *psErrnoPtr = DS_EWOULDBLOCK;
    return -1;
  }

  remainingSize = maxSize - currSize;

  /*-------------------------------------------------------------------------
    If tail is not NULL, pack bytes in to it to avoid internal fragmentation.
    Otherwise, allocate a new DSM item

    numLeftOverBytesInDSMItem indicate how many bytes can be written to a
    DSM item. If the item is newly allocated, it is the size of DSM item,
    otherwise, it is the amount of space still left in the item
  -------------------------------------------------------------------------*/
  if (0 != *itemTailPtrPtr)
  {
    dsmItemPtr                = *itemTailPtrPtr;
    numLeftOverBytesInDSMItem = dsmItemPtr->size - dsmItemPtr->used;
    //TODO see if we can use dsm_pushdown_packed Try to use DSM APIs instead
    //of using private variables of DSM struct
  }
  else
  {
    dsmItemPtr = AllocDSMItem( remainingSize);
    if (0 == dsmItemPtr)
    {
      LOG_MSG_ERROR_0("SocketIO::CopyBufToDSM(): "
                      "Out of DSM items");
      *psErrnoPtr = DS_EWOULDBLOCK;
      return -1;
    }

    *itemHeadPtrPtr           = dsmItemPtr;
    *itemTailPtrPtr           = dsmItemPtr;
    numLeftOverBytesInDSMItem = dsmItemPtr->size;
  }

  /*-------------------------------------------------------------------------
    Copy data from each IOVec item in to the Q. To avoid internal
    fragmentation in DSM items, each allocated DSM item is completely filled
    before allocating a new DSM item

    //TODO Expplain why rem size is not checked here for 0
  -------------------------------------------------------------------------*/
  for (idx = 0; idx < numIOVec; idx++)
  {
    //TODO dsm_pushdown_packed does all this. So use it There cud be IOVec
    //API for DSM too.
    /*-----------------------------------------------------------------------
      Since an IOVec item could be copied in to more than one DSM item, data
      is not always copied from the begining. So addrToCopyFrom always starts
      from the first byte of IOVec item and is updated as data from this item
      is copied in to one or more DSM items in the inner loop
    -----------------------------------------------------------------------*/
    addrToCopyFrom = ioVecArr[idx].data;

    /*-----------------------------------------------------------------------
      If size of IOVec item is more than the amount of space available in
      the Q, copy till the Q becomes full and discard rest of the data.
    -----------------------------------------------------------------------*/
    numBytesToCopyFromIOVec = MIN( ioVecArr[idx].dataLen, remainingSize);

    /*-----------------------------------------------------------------------
      Copy data from IOVec item in to Q in to one or more DSM items
    -----------------------------------------------------------------------*/
    while (0 < numBytesToCopyFromIOVec)
    {
      /*---------------------------------------------------------------------
        Allocate a new DSM item, if previous item is filled completely and
        update Q's tail
      ---------------------------------------------------------------------*/
      if (0 == numLeftOverBytesInDSMItem)
      {
        dsmItemPtr = AllocDSMItem( remainingSize);
        if (0 == dsmItemPtr)
        {
          LOG_MSG_ERROR_0("SocketIO::CopyBufToDSM(): "
                          "Out of DSM items");
          *psErrnoPtr = DS_EWOULDBLOCK;
          goto bail;
        }

        numLeftOverBytesInDSMItem  = dsmItemPtr->size;
        (*itemTailPtrPtr)->pkt_ptr = dsmItemPtr;
        *itemTailPtrPtr            = dsmItemPtr;
      }

      numBytesToCopyToDSMItem =
        MIN( numLeftOverBytesInDSMItem, numBytesToCopyFromIOVec);

      /*---------------------------------------------------------------------
        If DSM item is newly allocated, data is copied in to it's first byte.
        Otherwise, data is appended to where the previous write left off.
      ---------------------------------------------------------------------*/
      addrToCopyInTo = dsmItemPtr->data_ptr + dsmItemPtr->used;
      memscpy( addrToCopyInTo, numBytesToCopyToDSMItem,
               addrToCopyFrom, numBytesToCopyToDSMItem);

      /*---------------------------------------------------------------------
        Update counters appropriately
      ---------------------------------------------------------------------*/
      addrToCopyFrom            += numBytesToCopyToDSMItem;
      numCopiedBytes            += numBytesToCopyToDSMItem;
      numLeftOverBytesInDSMItem -= numBytesToCopyToDSMItem;
      numBytesToCopyFromIOVec   -= numBytesToCopyToDSMItem;
      remainingSize             -= numBytesToCopyToDSMItem;

      dsmItemPtr->used += static_cast <uint16> ( numBytesToCopyToDSMItem);
    } /* IOVec is not completely copied */

  } /* for number of IOVec items */

  /*-------------------------------------------------------------------------
    If Q is full, update errno to notify the writer
    //TODO Document how DgramQ is flow controlled even when data pipe is not
    //full. Platform flow controls whenever Dgram returns EWOULDBLOCK
  -------------------------------------------------------------------------*/
  if (0 == remainingSize)
  {
    LOG_MSG_INFO1_1("SocketIO::CopyBufToDSM(): "
                    "Q 0x%x full", this);
    isHighWMHit = true;
    *psErrnoPtr = DS_EWOULDBLOCK;
  }

bail:
  return numCopiedBytes;

} /* SocketIO::CopyBufToDSM() */


int32 SocketIO::MethodNotSupported
(
  int16 *  psErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("SocketIO::MethodNotSupported(): "
                    "NULL errno");
    return -1;
  }

  *psErrnoPtr = DS_EINVAL;
  return -1;

} /* SocketIO::MethodNotSupported() */


/*===========================================================================

                  C Wrappers For C++ Interface Methods

===========================================================================*/
extern "C"
{

int32 Read
(
  void *            readerHandlePtr,
  dsm_item_type **  dsmItemPtrPtr,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
)
{
  IReader *  readerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    DATA_ERR_FATAL("Read(): NULL errno");
    return -1;
  }

  if (0 == readerHandlePtr)
  {
    DATA_ERR_FATAL("Read(): NULL reader");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  readerPtr = (IReader *) readerHandlePtr;

  return
    readerPtr->Read( dsmItemPtrPtr, metaDataPtr, metaDataLenPtr, psErrnoPtr);

} /* Read() */


int32 Peek
(
  void *            readerHandlePtr,
  dsm_item_type **  dsmItemPtrPtr,
  uint32            numBytes,
  uint32            offset,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
)
{
  IReader *  readerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    DATA_ERR_FATAL("Peek(): NULL errno");
    return -1;
  }

  if (0 == readerHandlePtr)
  {
    DATA_ERR_FATAL("Peek(): NULL reader");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  readerPtr = (IReader *) readerHandlePtr;

  return readerPtr->Peek( dsmItemPtrPtr,
                          numBytes,
                          offset,
                          metaDataPtr,
                          metaDataLenPtr,
                          psErrnoPtr);
} /* Peek() */


void FreeBytes
(
  void *   readerHandlePtr,
  int32    numBytes
)
{
  IReader *  readerPtr;
  int16      psErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == readerHandlePtr)
  {
    DATA_ERR_FATAL("FreeBytes(): NULL reader");
    return;
  }

  readerPtr = (IReader *) readerHandlePtr;

  (void) readerPtr->FreeBytes( numBytes, &psErrno);
  return;

} /* FreeBytes() */


void ReleaseReader
(
  void *  readerHandlePtr
)
{
  IReader *  readerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == readerHandlePtr)
  {
    DATA_ERR_FATAL("ReleaseReader(): NULL reader");
    return;
  }

  readerPtr = (IReader *) readerHandlePtr;

  readerPtr->ReleaseReader();
  return;

} /* ReleaseReader() */


void SetRcvBufSize
(
  void *  readerHandlePtr,
  int32   size
)
{
  IReader *  readerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == readerHandlePtr)
  {
    DATA_ERR_FATAL("SetRcvBufSize(): NULL reader");
    return;
  }

  readerPtr = (IReader *) readerHandlePtr;

  readerPtr->SetRcvBufSize( size);
  return;

} /* SetRcvBufSize() */


uint16 GetPktCnt
(
  void *  readerHandlePtr
)
{
  IReader *  readerPtr;
  uint16     pktCnt = 0;
  int16      psErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == readerHandlePtr)
  {
    DATA_ERR_FATAL("GetPktCnt(): NULL reader");
    return 0;
  }

  readerPtr = (IReader *) readerHandlePtr;
  (void) readerPtr->GetPktCnt( &pktCnt, &psErrno);

  return pktCnt;
} /* GetPktCnt() */


int32 Write
(
  void *            writerHandlePtr,
  dsm_item_type **  dsmItemPtrPtr,
  uint32            length,
  char *            metaDataPtr,
  int32             metaDataLen,
  int16 *           psErrnoPtr
)
{
  IWriter *  writerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    DATA_ERR_FATAL("Write(): NULL errno");
    return -1;
  }

  if (0 == writerHandlePtr)
  {
    DATA_ERR_FATAL("Write(): NULL writer");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  writerPtr = (IWriter *) writerHandlePtr;

  return writerPtr->Write( dsmItemPtrPtr,
                           length,
                           metaDataPtr,
                           metaDataLen,
                           psErrnoPtr);
} /* Write() */


void ReleaseWriter
(
  void *  writerHandlePtr
)
{
  IWriter *  writerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == writerHandlePtr)
  {
    DATA_ERR_FATAL("ReleaseWriter(): NULL writer");
    return;
  }

  writerPtr = (IWriter *) writerHandlePtr;

  writerPtr->ReleaseWriter();
  return;

} /* ReleaseWriter() */


void SetSndBufSize
(
  void *  writerHandlePtr,
  int32   size
)
{
  IWriter *  writerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == writerHandlePtr)
  {
    DATA_ERR_FATAL("SetSndBufSize(): NULL writer");
    return;
  }

  writerPtr = (IWriter *) writerHandlePtr;

  writerPtr->SetSndBufSize( size);
  return;

} /* SetSndBufSize() */


int32 GetSndCnt
(
  void *  writerHandlePtr
)
{
  IWriter *  writerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == writerHandlePtr)
  {
    DATA_ERR_FATAL("GetSndCnt(): NULL writer");
    return -1;
  }

  writerPtr = (IWriter *) writerHandlePtr;
  return writerPtr->GetSndCnt();

} /* GetSndCnt() */

} /* extern "C" */

