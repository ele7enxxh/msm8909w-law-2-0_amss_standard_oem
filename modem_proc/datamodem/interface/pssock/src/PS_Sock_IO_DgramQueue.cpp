/*===========================================================================
  FILE: PS_Sock_IO_DgramQueue.cpp

  OVERVIEW: This file extends implementation of SocketIO abstract class to
  support a data pipe which maintains message boundaries. This class is used
  by UDP sockets.

  DEPENDENCIES: None

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_IO_DgramQueue.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-03-25 hm  Correct dsm_empty_queue and dsm_queue_destroy usage.
  2010-02-05 ss  Replacing dsm_empty_queue with dsm_queue_destroy in 
                 ReleaseReader.
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
#include "PS_Sock_IO_DgramQueue.h"
#include "ds_Utils_DebugMsg.h"
#include "dserrno.h"


using namespace ds::Sock;
using namespace PS::Sock::IO;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void * DgramQueue::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_UDP_DGRAM_QUEUE_TYPE);
} /* DgramQueue::operator new() */


DgramQueue::DgramQueue
(
  ICritSect *  _critSectPtr
) :
    SocketIO( _critSectPtr),
    currPktCnt( 0),
    lowWMPktCnt( 0)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //TODO define a local const
  //Just to be safe, memset args to 0 in case dsm_queue_init() is not doing
  //the right thing. 
  memset (&dgramWM, 0, sizeof(dgramWM));
  memset (&dgramWMQueue, 0, sizeof(dgramWMQueue));
  dsm_queue_init( &dgramWM, 0x7FFFFFFF, &dgramWMQueue);
  return;

} /* DgramQueue::DgramQueue() */


void DgramQueue::operator delete
(
  void *  bufPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_MEM_FREE( bufPtr);
  return;

} /* DgramQueue::operator delete() */


DgramQueue::~DgramQueue
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
  dsm_queue_destroy( &dgramWM);
  currPktCnt = 0;

  critSectPtr->Leave();
  /*lint -restore */

  return;

} /* DgramQueue::~DgramQueue() */


int32 DgramQueue::Write
(
  const IPort1::SeqBytes       ioVecArr[],
  int32                        numIOVec,
  char *                       metaDataPtr,
  int32                        metaDataLen,
  int16 *                      psErrnoPtr
)
{
  dsm_item_type *  dsmItemHeadPtr = 0;
  dsm_item_type *  dsmItemTailPtr = 0;
  int32            tmpSize;
  int32            byteCount;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("DgramQueue::Write(): "
                    "NULL errno");
    return -1;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Data can't be written if dgram is closed or if shutdown for writes
  -------------------------------------------------------------------------*/
  if (WRITE_SHUTDOWN == state || CLOSED == state)
  {
    LOG_MSG_ERROR_1("DgramQueue::Write(): "
                    "Can't write, state %d", state);
    *psErrnoPtr = DS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Copy buffer in to the DSM item Q
  -------------------------------------------------------------------------*/
  byteCount = CopyBufToDSM( ioVecArr,
                            numIOVec,
                            &dsmItemHeadPtr,
                            &dsmItemTailPtr,
                            psErrnoPtr);
  if (-1 == byteCount)
  {
    LOG_MSG_INFO3_1("DgramQueue::Write(): "
                    "CopyBufToDSM failed, err %d", *psErrnoPtr);

    if (DS_EWOULDBLOCK == *psErrnoPtr)
    {
      lowWMPktCnt = currPktCnt * SocketIO::lowWMSizePercentage / 100;
    }

    goto bail;
  }
  else
  {
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
      dsm_free_packet( &dsmItemHeadPtr);
    }
    else
    {
      /*---------------------------------------------------------------------
        Reader is present

        Check if entire data is copied. Otherwise return failure to the
        client as DgramQueue doesn't support writing partial data in to the
        pipe
      ---------------------------------------------------------------------*/
      if (DS_EWOULDBLOCK == *psErrnoPtr)
      {
        /*-------------------------------------------------------------------
          errno is set to DS_EWOULDBLOCK if IOVec buffers can't be completely
          copied in to the Q. Set the lowWMPktCnt and free the DSM item chain
        -------------------------------------------------------------------*/
        lowWMPktCnt = currPktCnt * SocketIO::lowWMSizePercentage / 100;
        goto bail;
      }
      else
      {
        /*-------------------------------------------------------------------
          Push meta data on to the DSM item
        -------------------------------------------------------------------*/
        if (0 != metaDataPtr)
        {
          if (metaDataLen !=
                dsm_pushdown_packed_long( &dsmItemTailPtr,
                                           metaDataPtr,
                                           metaDataLen,
                                           DSM_DS_SMALL_ITEM_POOL))
          {
            LOG_MSG_ERROR_0("DgramQueue::Write(): "
                            "Couldn't push meta data");
            *psErrnoPtr = DS_EWOULDBLOCK;
            goto bail;
          }
        }

        /*-------------------------------------------------------------------
          Enqueue the DSM item in to Q. Notify IReaderListener if the Q was
          empty prior to the write
        -------------------------------------------------------------------*/
        dsm_enqueue( &dgramWM, &dsmItemHeadPtr);
        currPktCnt++;

        tmpSize   = currSize;
        currSize += byteCount;

        if (0 == tmpSize && 0 != readerListenerPtr)
        {
          readerListenerPtr->DataAvailable();
        }
      }
    }
  }

  critSectPtr->Leave();
  return byteCount;

  /*-------------------------------------------------------------------------
    Common error handling code

    Free the DSM item, leave critical section and return error
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  dsm_free_packet( &dsmItemHeadPtr);
  return -1;

} /* DgramQueue::Write() */


int32 DgramQueue::Write
(
  dsm_item_type **  dsmItemPtrPtr,
  int32             bufLen,
  char *            metaDataPtr,
  int32             metaDataLen,
  int16 *           psErrnoPtr
)
{
  int32  tmpSize;
  uint32  tmpDSMCount = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("DgramQueue::Write(): "
                    "NULL errno");
    return -1;
  }

  if (0 == dsmItemPtrPtr)
  {
    LOG_MSG_ERROR_0("DgramQueue::Write(): "
                    "NULL DSM item");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Data can't be written if dgram is closed or if shutdown for writes
  -------------------------------------------------------------------------*/
  if (WRITE_SHUTDOWN == state || CLOSED == state)
  {
    LOG_MSG_ERROR_1("DgramQueue::Write(): "
                    "Can't write, state %d", state);
    *psErrnoPtr = DS_EINVAL;
    goto bail;
  }

#ifdef FEATURE_DATA_PS_DEBUG
  /*-------------------------------------------------------------------------
    Safety check to make sure that the correct length is passed
  -------------------------------------------------------------------------*/
  ASSERT( bufLen == dsm_length_packet( *dsmItemPtrPtr));
#endif /* FEATURE_DATA_PS_DEBUG */

  /*-------------------------------------------------------------------------
    Push meta data on to the DSM item
  -------------------------------------------------------------------------*/
  if (0 != metaDataPtr)
  {
    if (metaDataLen != dsm_pushdown_packed_long( dsmItemPtrPtr,
                                                 metaDataPtr,
                                                 metaDataLen,
                                                 DSM_DS_SMALL_ITEM_POOL))
    {
      LOG_MSG_ERROR_0("DgramQueue::Write(): "
                      "Couldn't push meta data");
      *psErrnoPtr = DS_EWOULDBLOCK;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    1. Get amount of outstanding data in WM
    2. Enqueue the DSM item in to WM
    3. Get amount of outstanding data in WM and check if it increased. If so,
       it means that data is actually enqueued. If not, enqueue failed because
       DNE is hit
    4. If enqueue is successful, notify IReaderListener if the WM was empty
       prior to the write
  -------------------------------------------------------------------------*/
  tmpDSMCount = dgramWM.current_cnt;
  dsm_enqueue( &dgramWM, dsmItemPtrPtr);

  if (dgramWM.current_cnt > tmpDSMCount)
  {
    currPktCnt++;

    tmpSize   = currSize;
    currSize += bufLen;

    if (0 == tmpSize && 0 != readerListenerPtr)
    {
      readerListenerPtr->DataAvailable();
    }
  }
  else
  {
    bufLen = 0;
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

} /* DgramQueue::Write() */


int32 DgramQueue::SetWrCritSection
(
  ICritSect *  newCritSectPtr,
  int16 *     psErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return MethodNotSupported( psErrnoPtr);
} /* DgramQueue::SetWrCritSection() */


int32 DgramQueue::Read
(
  IPort1::SeqBytes       ioVecArr[],
  int32                  numIOVec,
  char *                 metaDataPtr,
  int32 *                metaDataLenPtr,
  int16 *                psErrnoPtr
)
{
  dsm_item_type *  dsmItemPtr = 0;
  int32            byteCount  = 0;
  int32            idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (-1 == Read( &dsmItemPtr, metaDataPtr, metaDataLenPtr, psErrnoPtr))
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Extract data from DSM item in to the IOVec buffers
  -------------------------------------------------------------------------*/
  for (idx = 0; idx < numIOVec; idx++)
  {
    byteCount +=
      dsm_pullup_long( &dsmItemPtr, ioVecArr[idx].data, ioVecArr[idx].dataLen);
  }

  /*-------------------------------------------------------------------------
    If all data in DSM item is not extracted return EMSGTRUNC (indicates to
    the caller that insufficient buffer is passed)
  -------------------------------------------------------------------------*/
  if (0 != dsmItemPtr)
  {
    dsm_free_packet( &dsmItemPtr);
    *psErrnoPtr = DS_EMSGTRUNC;
  }

  return byteCount;
} /* DgramQueue::Read() */


int32 DgramQueue::Read
(
  dsm_item_type **  dsmItemPtrPtr,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
)
{
  int32  byteCount;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0("DgramQueue::Read(): "
                    "NULL errno");
    return -1;
  }

  if (0 == dsmItemPtrPtr)
  {
    LOG_MSG_ERROR_0("DgramQueue::Read(): "
                    "NULL DSM item");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set to 0 so that dsm_free_packet() in bail section doesn't try to free
    garbage location
  -------------------------------------------------------------------------*/
  *dsmItemPtrPtr = 0;

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Data can't be read if dgram is closed or if shutdown for reads
  -------------------------------------------------------------------------*/
  if (READ_SHUTDOWN == state || CLOSED == state)
  {
    LOG_MSG_ERROR_1("DgramQueue::Read(): "
                    "Can't read, state %d", state);
    *psErrnoPtr = DS_EINVAL;
    byteCount   = -1;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Return EEOF/EWOULDBLOCK if dgram is empty
  -------------------------------------------------------------------------*/
  if (0 == currPktCnt)
  {
    if (WRITE_SHUTDOWN == state)
    {
      LOG_MSG_INFO1_0("DgramQueue::Read(): "
                      "Q is WR_SHUTDOWN");
      *psErrnoPtr = DS_EEOF;
      byteCount   = 0;
    }
    else
    {
      LOG_MSG_INFO1_0("DgramQueue::Read(): "
                      "Empty Q");
      *psErrnoPtr = DS_EWOULDBLOCK;
      byteCount   = -1;
    }

    goto bail;
  }

  /*-------------------------------------------------------------------------
    Dequeue a DSM item from Q and pull out meta data
  -------------------------------------------------------------------------*/
  *dsmItemPtrPtr = dsm_dequeue( &dgramWM);
  ASSERT( 0 != *dsmItemPtrPtr);

  currPktCnt--;

  if (0 != metaDataPtr)
  {
    if (0 == metaDataLenPtr)
    {
      LOG_MSG_ERROR_0("DgramQueue::Read(): "
                      "NULL meta len");
      *psErrnoPtr = DS_EFAULT;
      byteCount   = -1;
      goto bail;
    }

    if (*metaDataLenPtr != dsm_pullup_long( dsmItemPtrPtr,
                                            metaDataPtr,
                                            ( *metaDataLenPtr)))
    {
      LOG_MSG_ERROR_0("DgramQueue::Read(): "
                      "Couldn't pull up meta data");
      *psErrnoPtr = DS_EINVAL;
      byteCount   = -1;
      goto bail;
    }
  }

  byteCount = dsm_length_packet( *dsmItemPtrPtr);

  /*-------------------------------------------------------------------------
    Update the currSize
  -------------------------------------------------------------------------*/
  DecCurrSize( byteCount);

  critSectPtr->Leave();

  *psErrnoPtr = DS_ENOERR;
  return byteCount;

  /*-------------------------------------------------------------------------
    Common error handling code

    Free DSM item, leave critical section and return error
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  dsm_free_packet( dsmItemPtrPtr);
  return byteCount;

} /* DgramQueue::Read() */


int32 DgramQueue::Peek
(
  dsm_item_type **  dsmItemPtrPtr,
  int32             numBytesToPeek,
  int32             offsetToPeekAt,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return MethodNotSupported( psErrnoPtr);
} /* DgramQueue::Peek() */


int32 DgramQueue::FreeBytes
(
  int32    numBytesToFree,
  int16 *  psErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return MethodNotSupported( psErrnoPtr);
} /* DgramQueue::FreeBytes() */


int32 DgramQueue::SetRdCritSection
(
  ICritSect * newCritSectPtr,
  int16 *     psErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return MethodNotSupported( psErrnoPtr);
} /* DgramQueue::SetRdCritSection() */


void DgramQueue::ReleaseReader
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("DgramQueue::ReleaseReader(): "
                           "Dgram 0x%x", this);

  critSectPtr->Enter(); /*lint !e1550 */

  /*-------------------------------------------------------------------------
    Since reader is released, there is no need to maintain DSM items in the Q.
    No body is going to read them anyway.
  -------------------------------------------------------------------------*/
  dsm_empty_queue( &dgramWM);

  /*-------------------------------------------------------------------------
    Leave critical section before calling SocketIO's ReleaseReader. Otherwise,
    lock will not be released if object is destructed in
    SocketIO's ReleaseReader as once the object is destroyed, its members are
    not accessible
  -------------------------------------------------------------------------*/
  critSectPtr->Leave(); /*lint !e1550 */

  /*-------------------------------------------------------------------------
    Call the base class's method so that state is changed appropriately
  -------------------------------------------------------------------------*/
  SocketIO::ReleaseReader();

  LOG_MSG_FUNCTION_EXIT_1("DgramQueue::ReleaseReader(): "
                          "Success, dgram 0x%x", this);
  return;

} /* DgramQueue::ReleaseReader() */

void DgramQueue::SetRcvBufSize
(
  int32  size
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();

  SocketIO::SetRcvBufSize(size);

  dsm_set_dne(&dgramWM, size);

  critSectPtr->Leave();

  return;
} /* DgramQueue::SetRcvBufSize() */
