#ifndef PS_SOCK_IO_SOCKETIO_H
#define PS_SOCK_IO_SOCKETIO_H
/*===========================================================================
  @file PS_Sock_IO_SocketIO.h

  This abstract class provides implementation of the basic methods of IReader
  and IWriter interfaces.

  Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/PS_Sock_IO_SocketIO.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_DebugMsg.h"


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
        @brief Defines various states of data pipe.

        Defines various states that a data pipe goes through. CLOSED state is
        the initial and the final state. The state machine is depicted as

                               +------------+
                               |   CLOSED   |
                               +------------+
                                     |
                                     |  new ByteStream() / new DgramQueue()
                                     |
                                     V
                               +------------+
                               |    OPEN    |
                         ------+------------+------
                        /                          \
       ReleaseReader() /                            \ ReleaseWriter()
                      /                              \
                     V                                V
           +-------------------+            +---------------------+
           |   READ_SHUTDOWN   |            |   WRITE_SHUTDOWN    |
           +-------------------+            +---------------------+
                     \                                /
                      \                              /
       ReleaseWriter() \                            / ReleaseReader()
                        \                          /
                         ----->+------------+<-----
                               |   CLOSED   |
                               +------------+
      */
      typedef enum
      {
        CLOSED,
        OPEN,
        WRITE_SHUTDOWN,
        READ_SHUTDOWN
      } StateType;

      /**
        @brief An abstract class with basic implementation of IReader and
        IWriter interfaces.

        A lot of methods of IReader and IWriter interfaces are common
        irrespective of how the actual data pipe is implemented, and SocketIO
        class provides implementation for such methods. The complete data
        pipe is implemented by extending this class.
      */
      class SocketIO : public IReader, public IWriter
      {
        public:
          /**
            @brief Derives from PS::Sock::IO::IWriter::Write.

            A derived method of PS::Sock::IO::IWriter::Write. It is redefined
            so that it is available for derived classes of SocketIO.

            readerListenerPtr is notified on 0 to non-0 transition.

            @see PS::Sock::IO::IWriter::Write, PS::Sock::IO::IReaderListener
          */
          virtual int32 Write
          (
            const IPort1::SeqBytes        ioVecArr[],
            int32                         numIOVec,
            char *                        metaDataPtr,
            int32                         metaDataLen,
            int16 *                       psErrnoPtr
          ) = 0;

          /**
            @brief Derives from PS::Sock::IO::IWriter::Write.

            A derived method of PS::Sock::IO::IWriter::Write. It is redefined
            so that it is available for derived classes of SocketIO.

            readerListenerPtr is notified on 0 to non-0 transition.

            @see PS::Sock::IO::IWriter::Write, PS::Sock::IO::IReaderListener
          */
          virtual int32 Write
          (
            dsm_item_type **  dsmItemPtrPtr,
            int32             bufLen,
            char *            metaDataPtr,
            int32             metaDataLen,
            int16 *           psErrnoPtr
          ) = 0;

          /**
            @brief Implements PS::Sock::IO::IWriter::ReleaseWriter.

            Provides implementation for PS::Sock::IO::IWriter::ReleaseWriter.

            @see PS::Sock::IO::IWriter::ReleaseWriter
          */
          virtual void ReleaseWriter
          (
            void
          ) throw();

          /**
            @brief Implements PS::Sock::IO::IWriter::SetSndBufSize.

            Provides implementation for PS::Sock::IO::IWriter::SetSndBufSize.

            @see PS::Sock::IO::IWriter::SetSndBufSize
          */
          virtual void SetSndBufSize
          (
            int32  size
          );

          /**
            @brief Implements PS::Sock::IO::IWriter::GetSndCnt.

            Provides implementation for PS::Sock::IO::IWriter::GetSndCnt.

            @see PS::Sock::IO::IWriter::GetSndCnt
          */
          virtual int32 GetSndCnt
          (
            void
          );

          /**
            @brief Implements PS::Sock::IO::IWriter::SetWrCritSection.

            Provides implementation for
            PS::Sock::IO::IWriter::SetWrCritSection.

            @see PS::Sock::IO::IWriter::SetWrCritSection
          */
          virtual int32 SetWrCritSection
          (
            ICritSect *            critSectPtr,
            int16 *                psErrnoPtr
          ) = 0;

          /**
            @brief Implements PS::Sock::IO::IWriter::RegisterWriterListener.

            Provides implementation for
            PS::Sock::IO::IWriter::RegisterWriterListener.

            @see PS::Sock::IO::IWriter::RegisterWriterListener
          */
          virtual void RegisterWriterListener
          (
            IWriterListener *  writerListenerPtr
          );

          /**
            @brief Derives from PS::Sock::IO::IReader::Read.

            A derived method of PS::Sock::IO::IReader::Read. It is redefined
            so that it is available for derived classes of SocketIO.

            readerListenerPtr is notified, if data pipe becomes empty.
            Similarly if the data pipe was full before, writerListenerPtr is
            notified once low WM is hit.

            @see PS::Sock::IO::IReader::Read, PS::Sock::IO::IWriterListener,
                 PS::Sock::IO::IReaderListener
          */
          virtual int32 Read
          (
            IPort1::SeqBytes        ioVecArr[],
            int32                   numIOVec,
            char *                  metaDataPtr,
            int32 *                 metaDataLenPtr,
            int16 *                 psErrnoPtr
          ) = 0;

          /**
            @brief Derives from PS::Sock::IO::IReader::Read.

            A derived method of PS::Sock::IO::IReader::Read. It is redefined
            so that it is available for derived classes of SocketIO.

            readerListenerPtr is notified, if data pipe becomes empty. Since
            maxSize is not enforced if writer uses this method, high WM is
            never hit and hence writer is not notified when low WM is hit.

            @see PS::Sock::IO::IReader::Read, PS::Sock::IO::IReaderListener
          */
          virtual int32 Read
          (
            dsm_item_type **  dsmItemPtrPtr,
            char *            metaDataPtr,
            int32 *           metaDataLenPtr,
            int16 *           psErrnoPtr
          ) = 0;

          /**
            @brief Derives from PS::Sock::IO::IReader::Peek.

            A derived method of PS::Sock::IO::IReader::Peek. It is redefined
            so that it is available for derived classes of SocketIO.

            @see PS::Sock::IO::IReader::Peek
          */
          virtual int32 Peek
          (
            dsm_item_type **  dsmItemPtrPtr,
            int32             numBytesToPeek,
            int32             offsetToPeekAt,
            char *            metaDataPtr,
            int32 *           metaDataLenPtr,
            int16 *           psErrnoPtr
          ) = 0;

          /**
            @brief Derives from PS::Sock::IO::IReader::FreeBytes.

            A derived method of PS::Sock::IO::IReader::FreeBytes. It is
            redefined so that it is available for derived classes of SocketIO.

            readerListenerPtr is notified, if data pipe becomes empty.
            Similarly if the data pipe was full before, writerListenerPtr is
            notified once low WM is hit.

            @see PS::Sock::IO::IReader::FreeBytes,
                 PS::Sock::IO::IWriterListener, PS::Sock::IO::IReaderListener
          */
          virtual int32 FreeBytes
          (
            int32    numBytesToFree,
            int16 *  psErrnoPtr
          ) = 0;

          /**
            @brief Implements PS::Sock::IO::IReader::ReleaseReader.

            Provides implementation for PS::Sock::IO::IReader::ReleaseReader.

            @see PS::Sock::IO::IReader::ReleaseReader
          */
          virtual void ReleaseReader
          (
            void
          ) throw();

          /**
            @brief Implements PS::Sock::IO::IReader::SetRcvBufSize.

            Provides implementation for PS::Sock::IO::IReader::SetRcvBufSize.

            @see PS::Sock::IO::IReader::SetRcvBufSize
          */
          virtual void SetRcvBufSize
          (
            int32  size
          );

          /**
            @brief Implements PS::Sock::IO::IReader::GetRcvCnt.

            Provides implementation for PS::Sock::IO::IReader::GetRcvCnt.

            @see PS::Sock::IO::IReader::GetRcvCnt
          */
          virtual int32 GetRcvCnt
          (
            void
          );

          /**
            @brief Derives from PS::Sock::IO::IReader::GetPktCnt.

            A derived method of PS::Sock::IO::IReader::GetPktCnt. It is
            redefined so that it is available for derived classes of SocketIO.

            @see PS::Sock::IO::IReader::GetPktCnt
          */
          virtual int32 GetPktCnt
          (
            uint16 *  pktCntPtr,
            int16 *   psErrnoPtr
          ) = 0;

          /**
            @brief Implements PS::Sock::IO::IReader::SetRdCritSection.

            Provides implementation for
            PS::Sock::IO::IReader::SetRdCritSection.

            @see PS::Sock::IO::IReader::SetRdCritSection
          */
          virtual int32 SetRdCritSection
          (
            ICritSect *            critSectPtr,
            int16 *                psErrnoPtr
          ) = 0;

          /**
            @brief Implements PS::Sock::IO::IReader::RegisterReaderListener.

            Provides implementation for
            PS::Sock::IO::IReader::RegisterReaderListener.

            @see PS::Sock::IO::IReader::RegisterReaderListener
          */
          virtual void RegisterReaderListener
          (
            IReaderListener *  readerListenerPtr
          );

        protected:
          /**
            @brief Constructor for SocketIO class.

            Initializes a SocketIO object.

            @param[in]  critSectPtr  Mutex for reader and writer

            @retval None
          */
          SocketIO
          (
            ICritSect *  critSectPtr
          );

          /**
            @brief Destructor for SocketIO class.

            Resets the SocketIO object.
            //TODO

            @param None

            @retval None
          */
          virtual ~SocketIO
          (
            void
          ) throw();

          /**
            @brief Allocates a DSM item

            Allocates a DSM item from small or large item pools based on the
            requested size. If there aren't enough items in the selected pool,
            it allocates item from the other pool.

            @param  size  Requested size of the DSM item

            @retval  "DSM item"  On success
            @retval  0           On failure
          */
          dsm_item_type * AllocDSMItem
          (
            int32  size
          );

          //TODO Document
          int32 CopyBufToDSM
          (
            const IPort1::SeqBytes        ioVecArr[],
            int32                         numIOVec,
            dsm_item_type **              itemHeadPtrPtr,
            dsm_item_type **              itemTailPtrPtr,
            int16 *                       psErrnoPtr
          );

          //TODO Document
          int32 MethodNotSupported
          (
            int16 *  psErrnoPtr
          );

          /**
            Points to IReaderListener object registered by the client.
          */
          IReaderListener *      readerListenerPtr;

          /**
            Points to IWriterListener object registered by the client.
          */
          IWriterListener *      writerListenerPtr;

          /**
            Mutex providing mutual exclusion for reader and writer.
          */
          ICritSect *            critSectPtr;

          /**
            Maximum amount of data that can be outstanding in the data pipe.
          */
          int32                  maxSize;

          /**
            Current amount of data in the data pipe.
          */
          int32                  currSize;

          /**
            @brief Defines the percentage used in low WM size calculation.

            Low WM size for a data pipe is set to lowWMSizePercentage of the
            maximum value maintained by the data pipe.

            Other alternative is to use a fixed low WM of 2 MSS.

            Ideally, the low WM should be tuned to the amount of data that can
            be processed by the PS task in the time an application takes to
            write more data once it is notified fof WRITE_EV. An application
            may not have data handy all the time. It could trigger a file I/O
            or signal some other module to generate data. Not sure which
            approach is better.
          */
          static const int8      lowWMSizePercentage = 50;

          /**
            Tracks the state of the data pipe.
          */
          StateType              state;

          /**
            @brief Indicates if high WM is reached

            Once the data pipe becomes full, it is considered that high WM
            is reached and this member is set to true. IWriterListener is
            notified if high WM was reached before and if currSize is <= low WM
          */
          bool                   isHighWMHit;
      };

    } /* namespace IO */
  } /* namespace Sock */
} /* namespace PS */


/*===========================================================================

                      PUBLIC MEMBER FUNCTIONS

===========================================================================*/
inline int32 PS::Sock::IO::SocketIO::GetSndCnt
(
  void
)
{
  int32  tmpSize;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();
  tmpSize = currSize;
  critSectPtr->Leave();

  return tmpSize;
} /* PS::Sock::IO::SocketIO::GetSndCnt() */


inline int32 PS::Sock::IO::SocketIO::GetRcvCnt
(
  void
)
{
  int32  tmpSize;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();
  tmpSize = currSize;
  critSectPtr->Leave();

  return tmpSize;
} /* PS::Sock::IO::SocketIO::GetRcvCnt() */


//TODO always alloc a large item foe TCP and use this model for Dgram
inline dsm_item_type * PS::Sock::IO::SocketIO::AllocDSMItem
(
  int32  size
)
{
  dsm_mempool_id_enum_type  poolID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Threshold size to determine if a large or small item should be used
  -------------------------------------------------------------------------*/
#define SOCKETIO_SMALL_ITEM_THRESHOLD   (3 * DSM_DS_SMALL_ITEM_SIZ)

  /*-------------------------------------------------------------------------
    Determine the pool to use based on the requested size
  -------------------------------------------------------------------------*/
  poolID = ( size > SOCKETIO_SMALL_ITEM_THRESHOLD)
             ? DSM_DS_LARGE_ITEM_POOL
             : DSM_DS_SMALL_ITEM_POOL;

  /*-------------------------------------------------------------------------
    If DSM items in the chosen pool are getting exhausted, use the other pool
  -------------------------------------------------------------------------*/
  if (DSM_LESS_THAN_FEW_FREE_ITEMS( poolID))
  {
    poolID = DSM_DS_OTHER_POOL_ID( poolID);
    if (DSM_LESS_THAN_FEW_FREE_ITEMS( poolID))
    {
      LOG_MSG_ERROR_1( "Out of DSM items, pool %d", poolID);
      return 0;
    }
  }

  return dsm_new_buffer( poolID);
} /* PS::Sock::IO::SocketIO::AllocDSMItem() */

#endif /* PS_SOCK_IO_SOCKETIO_H */
