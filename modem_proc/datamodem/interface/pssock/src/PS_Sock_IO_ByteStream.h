#ifndef PS_SOCK_IO_BYTESTREAM_H
#define PS_SOCK_IO_BYTESTREAM_H
/*===========================================================================
  @file PS_Sock_IO_ByteStream.h

  This file extends implementation of SocketIO abstract class to support a
  data pipe which doesn't maintain message boundaries. This class is used by
  TCP sockets.

  Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_IO_ByteStream.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "PS_Sock_IO_SocketIO.h"


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
        @brief Implements a data pipe which doesn't maintain message
        boundaries.

        Extends SocketIO abstract class to provide complete implementation of
        IWriter and IReader interfaces. It uses DSM items to store data.
        Boundaries are not maintained across write operations and all data in
        the pipe is treated as one single data unit, i.e. if IWriter writes
        500 and 1000 bytes successively, IReader can read all 1500 bytes in
        one shot. This class is used by TCP sockets.
      */
      class ByteStream : public SocketIO
      {
        public:
          /**
            @brief Overloaded operator new.

            Operator new is overloaded so that memory for ByteStream is
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

            Operator delete is overloaded so that memory for ByteStream is
            de-allocated and is put back in to ps_mem

            @param[in] bufPtr  The object to be deleted

            @retval None
          */
          static void operator delete
          (
            void *  bufPtr
          );

          /**
            @brief Constructor for ByteStream class.

            Initializes a ByteStream object.

            @param[in]  critSectPtr  Mutex for reader and writer

            @retval None

            @see PS::Sock::IO::SocketIO::SocketIO
          */
          ByteStream
          (
            ICritSect *  critSectPtr
          );

          /**
            @brief Destructor for ByteStream class.

            Resets the ByteStream object. If there is data in the data pipe,
            it is freed and the memory is reclaimed.

            @param None

            @retval None
          */
          virtual ~ByteStream
          (
            void
          ) throw();

          /**
            @brief Implements PS::Sock::IO::SocketIO::Write.

            Provides implementation for PS::Sock::IO::SocketIO::Write.
            Data is tightly packed to avoid internal fragmentation, i.e. if a
            DSM item was allocated to store data from last write and if space
            is available, current write operation copies data in to that
            DSM item before allocating a new DSM item.

            This method copies data as long as
              @li Stream is not full
              @li DSM items are available
            and returns success even if only partial amount of data is copied.

            Even though writing 0 bytes to the stream is allowed, since
            boundaries are not maintained, this operation becomes a no-op.
            Reader never knows that 0 bytes are written.

            As ByteStream class doesn't support meta data, this method fails
            if caller passes meta data.

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

            Even though writing 0 bytes to the stream is allowed, since
            boundaries are not maintained, this operation becomes a no-op.
            Reader never knows that 0 bytes are written.

            As ByteStream class doesn't support meta data, this method fails
            if caller passes meta data.

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
            @brief Extends PS::Sock::IO::SocketIO::SetSndBufSize.

            Extends PS::Sock::IO::SocketIO::SetSndBufSize to set low WM to
            lowWMSizePercentage of maxSize.

            @see PS::Sock::IO::SocketIO::SetSndBufSize
          */
          virtual void SetSndBufSize
          (
            int32  size
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::SetWrCritSection.

            Provides implementation for
            PS::Sock::IO::SocketIO::SetWrCritSection.

            This method is provided to support listening sockets. The sockets
            in the backlog queue are created using the parent listening socket
            and hence its critical section is used while creating the
            ByteStream. Once a connection is accepted, socket in the backlog
            queue detaches itself from listening socket. It gets its own
            socket at the sockets library. Hence it is necessary to update the
            critical section to the newly accepted socket's critical section.
            Otherwise socket at sockets library and its SocketIO objects use
            different critical sections potentially causing a deadlock.

            @see PS::Sock::IO::SocketIO::SetWrCritSection
          */
          virtual int32 SetWrCritSection
          (
            ICritSect *            critSectPtr,
            int16 *                psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::Read.

            Provides implementation for PS::Sock::IO::SocketIO::Read. Since
            this class doesn't support meta data, metaDataPtr is set to 0.

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

            Provides implementation for PS::Sock::IO::SocketIO::Read. All the
            data in the pipe is returned to the caller. Since this class
            doesn't support meta data, metaDataPtr is set to 0.

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

            Provides implementation for PS::Sock::IO::SocketIO::Peek. Since
            this class doesn't support meta data, metaDataPtr is set to 0.

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

            Provides implementation for PS::Sock::IO::SocketIO::FreeBytes.

            @see PS::Sock::IO::SocketIO::FreeBytes
          */
          virtual int32 FreeBytes
          (
            int32    numBytesToFree,
            int16 *  psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::GetPktCnt.

            This method is not supported.

            @see PS::Sock::IO::SocketIO::GetPktCnt
          */
          virtual int32 GetPktCnt
          (
            uint16 *  pktCntPtr,
            int16 *   psErrnoPtr
          );

          /**
            @brief Implements PS::Sock::IO::SocketIO::SetRdCritSection.

            Provides implementation for
            PS::Sock::IO::SocketIO::SetRdCritSection.

            @see PS::Sock::IO::ByteStream::SetWrCritSection,
                 PS::Sock::IO::SocketIO::SetRdCritSection
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

            @param  numBytesExtracted  Value to decrement the currSize by

            @retval  None
          */
          void DecCurrSize
          (
            int32  numBytesExtracted
          );

          /**
            @brief Points to head of the data pipe.

            Stream's data pipe is maintained as a queue of DSM items and this
            member points to the head of the queue. Head is updated as data is
            dequeued from the stream.
          */
          dsm_item_type *     dsmItemQHeadPtr;

          /**
            @brief Points to tail of the data pipe.

            Stream's data pipe is maintained as a queue of DSM items and this
            member points to the tail of the queue. Tail is updated as data is
            enqueued to the stream.

            Technically, this member is not needed as dsm_append() could have
            been used instead, to append to the stream. But dsm_append()
            traverses the DSM item chain pointed to by dsmItemHeadPtr, to
            reach the end of the data pipe and appends the new data. This
            could be unoptimal depending on number of DSM items in the
            data pipe.
          */
          dsm_item_type *     dsmItemQTailPtr;

          /**
            @brief Low WM value for the data pipe.

            Defines the threshold (lowWMSizePercentage of maxSize) below which
            the writer is notified that space is available in the pipe for
            more writes.
          */
          int32               lowWMSize;

          /**
            @brief Default low WM value for the data pipe.

            Defines a default low WM size for all Byte Streams. If the client
            doesn't change the maxSize of the data pipe, this value is used
            as the threshold below which the writer is notified that space is
            available in the pipe for more writes.
          */
          static const int32  defaultLowWMSize = 1500;
      };

    } /* namespace IO */
  } /* namespace Sock */
} /* namespace PS */


/*===========================================================================

                      PUBLIC MEMBER FUNCTIONS

===========================================================================*/
inline void PS::Sock::IO::ByteStream::DecCurrSize
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
    If stream is empty, reset Q's head and tail pointers. Also notify the
    listener

    Note that readerListener must be called before writerListener. Otherwise,
    writerListener could write data to the stream and readerListener will get
    the wrong impression that stream is empty even though it is not
  -------------------------------------------------------------------------*/
  if (0 == currSize)
  {
    dsmItemQHeadPtr = dsmItemQTailPtr = 0;

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
  if (isHighWMHit && lowWMSize >= currSize)
  {
    isHighWMHit = false;

    if (0 != writerListenerPtr)
    {
      writerListenerPtr->BufferAvailable();
    }
  }

  return;

} /* PS::Sock::IO::ByteStream::DecCurrSize() */

#endif /* PS_SOCK_IO_BYTESTREAM_H */
