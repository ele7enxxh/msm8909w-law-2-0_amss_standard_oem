#ifndef PS_SOCK_IO_DGRAMQUEUE_H
#define PS_SOCK_IO_DGRAMQUEUE_H
/*===========================================================================
  @file PS_Sock_IO_DgramQueue.h

  This file extends implementation of SocketIO abstract class to support a
  data pipe which maintains message boundaries. This class is used by
  UDP sockets.

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_IO_DgramQueue.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "queue.h"
#include "PS_Sock_IO_SocketIO.h"
#include "dserrno.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace PS
{
  namespace Sock
  {
    namespace IO
    {
      /**
        @brief Implements a data pipe which maintains message boundaries.

        Extends SocketIO abstract class to provide complete implementation of
        IWriter and IReader interfaces. It uses DSM items to store data.
        Boundaries are maintained across write operations, i.e. if IWriter
        writes 500 and 1000 bytes successively, IReader can read only
        500 bytes the first time even if it wants to read 1500 bytes. The
        subsequent read returns the 1000 bytes written by the second write
        operation. This class is used by UDP sockets.

        This class also supports meta data.
      */
      class DgramQueue : public SocketIO
      {
        public:
          /**
            @brief Overloaded operator new.

            Operator new is overloaded so that memory for DgramQueue is
            allocated from ps_mem.

            As the C Runtime Library throws a std::bad_alloc exception if the
            memory allocation fails, and since memory allocation could fail as
            memory is allocated from ps_mem, this method also handles the
            exception.

            @param[in] numBytes  The amount of memory to be allocated

            @retval address  Ptr to object if memory is allocated
            @retval 0        If memory couldn't be allocated
          */
          static void * operator new
          (
            unsigned int   numBytes
          ) throw();

          /**
            @brief Overloaded operator delete.

            Operator delete is overloaded so that memory for DgramQueue is
            de-allocated and is put back in to ps_mem

            @param[in] bufPtr  The object to be deleted

            @retval None
          */
          static void operator delete
          (
            void *  bufPtr
          );

          /**
            @brief Constructor for DgramQueue class.

            Initializes a DgramQueue object.

            @param[in]  critSectPtr  Mutex for reader and writer

            @retval None

            @see PS::Sock::IO::SocketIO::SocketIO
          */
          DgramQueue
          (
            ICritSect *  critSectPtr
          );

          /**
            @brief Destructor for DgramQueue class.

            Resets the DgramQueue object. If there is data in the data pipe,
            it is freed and the memory is reclaimed.

            @param None

            @retval None
          */
          virtual ~DgramQueue
          (
            void
          ) throw();

          /**
            @brief Implements PS::Sock::IO::SocketIO::Write.

            Provides implementation for PS::Sock::IO::SocketIO::Write.
            A new DSM item chain is allocated for each write and is enqueued
            in to the WM. This method return error if only partial amount of
            data could be copied.

            Writing 0 bytes to a dgram Q is allowed, and an empty DSM item
            is allocated and is enqueued in to the WM. Reader gets an empty
            DSM item next time it reads and processes it just like any other
            read.

            If meta data is passed, it is pushed on to the allocated DSM item
            chain.

            @see PS::Sock::IO::SocketIO::Write
          */
          virtual int32 Write
          (
            const IPort1::SeqBytes        ioVecArr[],
            int32                         numIOVec,
            char *                        metaDataPtr,
            int32                         metaDataLen,
            int16 *                       psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::Write.

            Provides implementation for PS::Sock::IO::SocketIO::Write.
            The input DSM chain is appended to the Q.

            Writing 0 bytes to a dgram Q is allowed, and the input DSM item is
            enqueued in to the WM. Reader gets an empty DSM item next time it
            reads and processes it just like any other read.

            If meta data is passed, it is pushed on to the allocated DSM item
            chain.

            @see PS::Sock::IO::SocketIO::Write
          */
          virtual int32 Write
          (
            dsm_item_type **  dsmItemPtrPtr,
            int32             bufLen,
            char *            metaDataPtr,
            int32             metaDataLen,
            int16 *           psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::IWriter::SetWrCritSection.

            Returns DS_EINVAL as this method is not supported.

            @see PS::Sock::IO::IWriter::SetWrCritSection
          */
          virtual int32 SetWrCritSection
          (
            ICritSect *            critSectPtr,
            int16 *                psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::Read.

            Provides implementation for PS::Sock::IO::SocketIO::Read.

            @see PS::Sock::IO::SocketIO::Read
          */
          virtual int32 Read
          (
            IPort1::SeqBytes        ioVecArr[],
            int32                   numIOVec,
            char *                  metaDataPtr,
            int32 *                 metaDataLenPtr,
            int16 *                 psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::Read.

            Provides implementation for PS::Sock::IO::SocketIO::Read.

            @see PS::Sock::IO::SocketIO::Read
          */
          virtual int32 Read
          (
            dsm_item_type **  dsmItemPtrPtr,
            char *            metaDataPtr,
            int32 *           metaDataLenPtr,
            int16 *           psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::Peek.

            Returns DS_EINVAL as this method is not supported.

            @see PS::Sock::IO::SocketIO::Peek
          */
          virtual int32 Peek
          (
            dsm_item_type **  dsmItemPtrPtr,
            int32             numBytesToPeek,
            int32             offsetToPeekAt,
            char *            metaDataPtr,
            int32 *           metaDataLenPtr,
            int16 *           psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::FreeBytes.

            Returns DS_EINVAL as this method is not supported.

            @see PS::Sock::IO::SocketIO::FreeBytes
          */
          virtual int32 FreeBytes
          (
            int32    numBytesToFree,
            int16 *  psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::GetPktCnt.

            Provides implementation for PS::Sock::IO::SocketIO::GetPktCnt.

            @see PS::Sock::IO::SocketIO::GetPktCnt
          */
          virtual int32 GetPktCnt
          (
            uint16 *  pktCntPtr,
            int16 *   psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::IReader::SetRdCritSection.

            Returns DS_EINVAL as this method is not supported.

            @see PS::Sock::IO::SocketIO::SetRdCritSection
          */
          virtual int32 SetRdCritSection
          (
            ICritSect *            critSectPtr,
            int16 *                psErrnoPtr
          );

          /**
            @brief Extends PS::Sock::IO::SocketIO::ReleaseReader.

            Extends implementation of PS::Sock::IO::SocketIO::ReleaseReader.
            Since reader is released and since data in the pipe will not be
            read, all the items in the pipe are freed.

            @see PS::Sock::IO::SocketIO::ReleaseReader
          */
          virtual void ReleaseReader
          (
            void
          ) throw();

          /**
            @brief Extends PS::Sock::IO::SocketIO::SetRcvBufSize.
	 
            Extends implementation of PS::Sock::IO::SocketIO::SetRcvBufSize.
            Sets the receive window size and also the dsm dne size
	 
            @see PS::Sock::IO::SocketIO::SetRcvBufSize
          */
          virtual void SetRcvBufSize
          (
            int32  size
          );

        private:
          /**
            @brief Decrements the size of the data pipe.

            Decrements the current size of the data pipe by the amount of read
            data. IReaderListener is notified if the data pipe becomes empty,
            and IWriterListener is notified if data pipe was full before and
            if space is available in the pipe now because data is read.
            In rare cases, it is possible that data pipe is full and that
            reader read all data in one shot. In this case, IReaderListener is
            notified before IWriterListener is notified. Otherwise, data could
            be written to the pipe in IWriterListener's callback and
            IReaderListener will be wrongly notified that data pipe is empty.

            @param[in]  numBytesExtracted  Value to decrement the currSize by

            @retval  None
          */
          void DecCurrSize
          (
            int32  numBytesExtracted
          );

          /**
            @brief A data structure to hold the data.

            Data that is written to a dgram needs to be maintained as a queue
            of DSM item chains in order to preserve the message boundaries.
            A watermark provides this functionality.
          */
          dsm_watermark_type  dgramWM;

          /**
            @brief A data structure needed by WM.

            Each watermark needs a queue to maintain DSM item chains and this
            member provides it.
          */
          q_type              dgramWMQueue;

          /**
            @brief Number of packets in the data pipe.

            Tracks the number of messages in the data pipe. Incremented on each
            successful Write() and decremented on each successful Read().
          */
          uint16              currPktCnt;

          /**
            @brief Low WM value for the data pipe.

            Defines the threshold (lowWMSizePercentage of currPktCnt) below
            which the writer is notified that space is available in the pipe
            for more writes.

            This class uses number of packets instead of number of bytes as
            low WM because, reader doesn't read data in chunks of MSS.
            If writer writes 5000 byte messages, reader gets the whole 5000
            byte message on each read and if 1500 bytes is used as low WM,
            then writer will not be notified until the data pipe becomes
            empty.
          */
          uint16              lowWMPktCnt;
      };

    } /* namespace IO */
  } /* namespace Sock */
} /* namespace PS */


/*===========================================================================

                      PUBLIC MEMBER FUNCTIONS

===========================================================================*/
inline int32 PS::Sock::IO::DgramQueue::GetPktCnt
(
  uint16 *  pktCntPtr,
  int16 *   psErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == psErrnoPtr)
  {
    LOG_MSG_ERROR_0( "NULL errno");
    return -1;
  }

  if (0 == pktCntPtr)
  {
    LOG_MSG_ERROR_0( "NULL arg");
    *psErrnoPtr = DS_EFAULT;
    return -1;
  }

  critSectPtr->Enter();
  *pktCntPtr = currPktCnt;
  critSectPtr->Leave();

  return 0;
} /* PS::Sock::IO::DgramQueue::GetPktCnt() */


inline void PS::Sock::IO::DgramQueue::DecCurrSize
(
  int32  numBytesExtracted
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_PS_DEBUG
  ASSERT( 0 <= numBytesExtracted);
#endif /* FEATURE_DATA_PS_DEBUG */

  currSize -= numBytesExtracted;

  /*-------------------------------------------------------------------------
    If WM is empty, notify the listener.

    Note that readerListener must be called before writerListener. Otherwise,
    writerListener could write data to the stream and readerListener will get
    the wrong impression that WM is empty even though it is not
  -------------------------------------------------------------------------*/
  if (0 == currSize)
  {
    /*-----------------------------------------------------------------------
      If Writer is released, reader shouldn't be notified that the queue is
      empty. Otherwise, reader will be blocked from calling Read() again and
      will never get EOF
    -----------------------------------------------------------------------*/
    if (WRITE_SHUTDOWN != state && 0 != readerListenerPtr)
    {
      readerListenerPtr->EmptyQueue();
    }
  }

  /*-------------------------------------------------------------------------
    If the stream was full previously, notify the listener that it can write
    data now
  -------------------------------------------------------------------------*/
  if (isHighWMHit && lowWMPktCnt >= currPktCnt)
  {
    isHighWMHit = false;

    if (0 != writerListenerPtr)
    {
      writerListenerPtr->BufferAvailable();
    }
  }

  return;

} /* PS::Sock::IO::DgramQueue::DecCurrSize() */

#endif /* PS_SOCK_IO_DGRAMQUEUE_H */
