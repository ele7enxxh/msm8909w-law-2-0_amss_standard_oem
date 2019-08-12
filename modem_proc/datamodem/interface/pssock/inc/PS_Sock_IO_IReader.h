#ifndef PS_SOCK_IO_IREADER_H
#define PS_SOCK_IO_IREADER_H
/*===========================================================================
  @file PS_Sock_IO_IReader.h

  This file defines an interface to read data from a data pipe.

  Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/PS_Sock_IO_IReader.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "dsm.h"

#ifdef __cplusplus
#include "ds_Utils_IPort1.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_ICritSect.h"
#include "PS_Sock_IO_IReaderListener.h"
#include "ds_Net_Def.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*lint -esym(1510, IReader) */
namespace PS
{
  namespace Sock
  {
    namespace IO
    {
      /**
        @brief Defines an interface to read data from a data pipe.

        Defines methods which allows the user to read from a data pipe.
        The data pipe could be a queue or a watermark and this interface can
        be implemented differently to support reading from them.

        Each IReader is associated with a IWriter and they together form a
        data pipe. The interface assumes that same class implements both
        IReader and IWriter interfaces and hence provides a method to release
        the reader side. It is not a clean interface separation but gets the
        job done without proliferating the number of interfaces.
      */
      class IReader
      {
        public:
          /**
            Reads data in to one or more IOVec buffers.

            Reads data from the data pipe in to an array of IOVec buffers.
            User specifies the number of buffers in the array and the length
            of each buffer. The read data is deleted from the data pipe. This
            method also returns meta data if any is associated with the actual
            data. User passes a character buffer to hold the meta data and the
            length of the buffer.

            If the operation fails, psErrnoPtr is populated with the failure
            reason. Possible values are
              @li DS_EFAULT       Caller passed a non-NULL metaDataPtr but a
                                  NULL metaDataLenPtr
              @li DS_EINVAL       IReader object is in invalid state and can't
                                  read any more data or if meta data couldn't
                                  be extracted
              @li DS_EEOF         Data pipe is empty and write direction is
                                  closed
              @li DS_EWOULDBLOCK  Data pipe is empty but write direction is
                                  not closed
              @li DS_EMSGTRUNC    Caller passed a smaller buffer than the size
                                  of the message in the data pipe

            @param[out]     ioVecArr        Array of IOVec buffers which are
                                            updated with read data
            @param[in]      numIOVecArr     Number of IOVec buffers in the
                                            array
            @param[out]     metaDataPtr     Ptr to a char buffer. If non-NULL
                                            and if meta data is associated
                                            with read data, this buffer is
                                            updated with meta data
            @param[in,out]  metaDataLenPtr  Contains length of buffer in
                                            metaDataPtr. If non-NULL,
                                            metaDataPtr is updated with those
                                            many bytes of meta data. If
                                            meta data is not present, this
                                            parameter is set to 0
            @param[out]     psErrnoPtr      Updated with error value if
                                            operation fails

            @retval  "+ve value"  Number of read bytes. Could be 0 too
            @retval  -1           Operation failed and psErrnoPtr is updated
                                  with reason
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
            Reads data in to a DSM chain.

            Reads data from the data pipe in to a DSM chain. The amount of
            data that is copied in to the DSM chain is implementation
            dependent. The read data is deleted from the data pipe. This
            method also returns meta data if any is associated with the actual
            data. User passes a character buffer to hold the meta data and the
            length of the buffer.

            If the operation fails, psErrnoPtr is populated with the failure
            reason. Possible values are
              @li DS_EFAULT       Caller passed a non-NULL metaDataPtr but a
                                  NULL metaDataLenPtr or caller passed a
                                  NULL DSM item
              @li DS_EINVAL       IReader object is in invalid state and can't
                                  read any more data or if meta data couldn't
                                  be extracted
              @li DS_EEOF         Data pipe is empty and write direction is
                                  closed
              @li DS_EWOULDBLOCK  Data pipe is empty but write direction is
                                  not closed

            @param[out]     dsmItemPtrPtr   DSM chain which is updated with
                                            read data
            @param[out]     metaDataPtr     Ptr to a char buffer. If non-NULL
                                            and if meta data is associated
                                            with read data, this buffer is
                                            updated with meta data
            @param[in,out]  metaDataLenPtr  Contains length of buffer in
                                            metaDataPtr. If non-NULL,
                                            metaDataPtr is updated with those
                                            many bytes of meta data. If
                                            meta data is not present, this
                                            parameter is set to 0
            @param[out]     psErrnoPtr      Updated with error value if
                                            operation fails

            @retval  "+ve value"  Number of read bytes. Could be 0 too
            @retval  -1           Operation failed and psErrnoPtr is updated
                                  with reason
          */
          virtual int32 Read
          (
            dsm_item_type **  dsmItemPtrPtr,
            char *            metaDataPtr,
            int32 *           metaDataLenPtr,
            int16 *           psErrnoPtr
          ) = 0;

          /**
            Reads data from the given offset in to a DSM chain.

            Reads data from the given offset in the data pipe in to a
            DSM chain. User specifies the amount of data to read. The read
            data is not deleted from the data pipe. This method also returns
            meta data if any is associated with the actual data. User passes a
            character buffer to hold the meta data and the length of the
            buffer.

            This method is useful for TCP sockets, which re-transmit a packet
            until peer acknowledges it. So TCP peeks data from the data pipe
            and deletes it when the ACK is received.

            If the operation fails, psErrnoPtr is populated with the failure
            reason. Possible values are
              @li DS_EFAULT       NULL dsmItemPtrPtr
              @li DS_EOPNOTSUPP   Implementation doesn't support this method
              @li DS_EINVAL       IReader object is in invalid state and can't
                                  read any more data
              @li DS_EEOF         Data pipe is empty and write direction is
                                  closed
              @li DS_EWOULDBLOCK  Data pipe is empty but write direction is
                                  not closed
              @li DS_ENOMEM       Out of DSM items

            @param[out]     dsmItemPtrPtr   DSM chain which is updated with
                                            read data
            @param[in]      numBytesToPeek  Amount of data to peek from the
                                            data pipe
            @param[in]      offsetToPeekAt  Offset in the data pipe to peek
                                            data from
            @param[out]     metaDataPtr     Ptr to a char buffer. If non-NULL
                                            and if meta data is associated
                                            with read data, this buffer is
                                            updated with meta data
            @param[in,out]  metaDataLenPtr  Contains length of buffer in
                                            metaDataPtr. If non-NULL,
                                            metaDataPtr is updated with those
                                            many bytes of meta data. If
                                            meta data is not present, this
                                            parameter is set to 0
            @param[out]     psErrnoPtr      Updated with error value if
                                            operation fails

            @retval  "+ve value"  Number of read bytes. Could be 0 too
            @retval  -1           Operation failed and psErrnoPtr is updated
                                  with reason
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
            Deletes data from the data pipe.

            Deletes the user specified amount of data from the data pipe.

            If the operation fails, psErrnoPtr is populated with the failure
            reason. Possible values are
              @li DS_EINVAL  Implementation doesn't support this operation

            @param[in]   numBytesToFree  Number of bytes to delete from the
                                         data pipe
            @param[out]  psErrnoPtr      Updated with error value if operation
                                         fails

            @retval   0  Success
            @retval  -1  Operation failed and psErrnoPtr is updated with reason
          */
          virtual int32 FreeBytes
          (
            int32    numBytesToFree,
            int16 *  psErrnoPtr
          ) = 0;

          /**
            @brief Releases reader.

            A data pipe has two sides, a reader and a writer. This method
            releases the reader side of a data pipe. Once the reader is
            released, user can't read any more data. The writer can continue
            to write though, but the data is silently discarded as there is no
            point in holding on to DSM items.

            @param  None

            @return  None
          */
          virtual void ReleaseReader
          (
            void
          ) throw() = 0;

          /**
            @brief Sets a limit on data pipe's size.

            Sets a limit on the maximum amount of data that can be outstanding
            in a data pipe. If data pipe is already of this size, and if the
            user tries to write more data, data is silently discarded.

            @param[in]  size  Max size for the data pipe

            @return  None
          */
          virtual void SetRcvBufSize
          (
            int32  size
          ) = 0;

          /**
            @brief Returns the amount of data in the data pipe.

            Returns the amount of outstanding data in the data pipe.

            @param  None

            @return  Amount of data in the pipe
          */
          virtual int32 GetRcvCnt
          (
            void
          ) = 0;

          /**
            @brief Returns the number of packets in the data pipe.

            Returns the number of packets in the data pipe. This count is
            incremented each time data is written to the pipe and is
            decremented each time data is read from the pipe.

            If the operation fails, psErrnoPtr is populated with the failure
            reason. Possible values are
              @li DS_EFAULT  Caller passed a NULL pktCntPtr
              @li DS_EINVAL  Implementation doesn't support this operation

            @param[out]  pktCntPtr   Updated with number of packets in the pipe
            @param[out]  psErrnoPtr  Updated with error value if operation fails

            @retval   0  Success
            @retval  -1  Operation failed and psErrnoPtr is updated with reason
          */
          virtual int32 GetPktCnt
          (
            uint16 *  pktCntPtr,
            int16 *   psErrnoPtr
          ) = 0;

          /**
            @brief Updates critical section for the data pipe.

            Some data pipe implementations may start with one critical section
            and then use another critical section. This method is provided to
            support such implementations.

            Both reader and writer has a method to update documentation.
            Ideally only one method such as "SetCritSection" should have been
            there, but then another interface is needed and both IReader and
            IWriter would derive from it. But if a class implements both
            IReader and IWriter, that class has to use virtual inheritance
            which is not preferred.

            If the operation fails, psErrnoPtr is populated with the failure
            reason. Possible values are
              @li DS_EFAULT  Caller passed a NULL critSectPtr
              @li DS_EINVAL  Implementation doesn't support this operation

            @param[in]   critSectPtr  New critical section to use
            @param[out]  psErrnoPtr   Updated with error value if operation
                                      fails

            @retval   0  Success
            @retval  -1  Operation failed and psErrnoPtr is updated with reason

            @return  None
          */
          virtual int32 SetRdCritSection
          (
            ICritSect *            critSectPtr,
            int16 *                psErrnoPtr
          ) = 0;

          /**
            @brief Registers a listener.

            Registers a listener for events notified by the reader.

            @param[in]  readerListener  Listener to register

            @return  None
          */

          virtual void RegisterReaderListener
          (
            IReaderListener *  readerListenerPtr
          ) = 0;
      };

    } /* namespace IO */
  } /* namespace Sock */
} /* namespace PS */

#else /* not __cplusplus */
/*===========================================================================

                 C Style Wrappers For IWriter Interface

===========================================================================*/
/**
  @brief A C style wrapper for PS::Sock::IO::IReader::Read

  Defines a C style wrapper around PS::Sock::IO::IReader::Read so that
  PS Stack, which is implemented in C, can read data from a data pipe. C code
  doesn't understand classes or namespaces and hence C wrappers are needed.

  readerHandlePtr points to the IReader object and this wrapper calls
  PS::Sock::IO::IReader::Read on it.

  @see PS::Sock::IO::IReader::Read
*/
int32 Read
(
  void *            readerHandlePtr,
  dsm_item_type **  dsmItemPtrPtr,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
);

/**
  @brief A C style wrapper for PS::Sock::IO::IReader::Peek

  Defines a C style wrapper around PS::Sock::IO::IReader::Peek so that
  PS Stack, which is implemented in C, can peek data from a data pipe. C code
  doesn't understand classes or namespaces and hence C wrappers are needed.

  readerHandlePtr points to the IReader object and this wrapper calls
  PS::Sock::IO::IReader::Peek on it.

  @see PS::Sock::IO::IReader::Peek
*/
int32 Peek
(
  void *            readerHandlePtr,
  dsm_item_type **  dsmItemPtrPtr,
  uint32            numBytesToPeek,
  uint32            offsetToPeekAt,
  char *            metaDataPtr,
  int32 *           metaDataLenPtr,
  int16 *           psErrnoPtr
);

/**
  @brief A C style wrapper for PS::Sock::IO::IReader::FreeBytes

  Defines a C style wrapper around PS::Sock::IO::IReader::FreeBytes so that
  PS Stack, which is implemented in C, can free data from a data pipe. C code
  doesn't understand classes or namespaces and hence C wrappers are needed.

  readerHandlePtr points to the IReader object and this wrapper calls
  PS::Sock::IO::IReader::FreeBytes on it.

  @see PS::Sock::IO::IReader::FreeBytes
*/
void FreeBytes
(
  void *   readerHandlePtr,
  int32    numBytesToFree
);

/**
  @brief A C style wrapper for PS::Sock::IO::IReader::ReleaseReader

  Defines a C style wrapper around PS::Sock::IO::IReader::ReleaseReader so that
  PS Stack, which is implemented in C, can release reader side of a data pipe.
  C code doesn't understand classes or namespaces and hence C wrappers are
  needed.

  readerHandlePtr points to the IReader object and this wrapper calls
  PS::Sock::IO::IReader::ReleaseReader on it.

  @see PS::Sock::IO::IReader::ReleaseReader
*/
void ReleaseReader
(
  void *  readerHandlePtr
);

/**
  @brief A C style wrapper for PS::Sock::IO::IReader::SetRcvBufSize

  Defines a C style wrapper around PS::Sock::IO::IReader::SetRcvBufSize so that
  PS Stack, which is implemented in C, can set a size limit on a data pipe.
  C code doesn't understand classes or namespaces and hence C wrappers are
  needed.

  readerHandlePtr points to the IReader object and this wrapper calls
  PS::Sock::IO::IReader::SetRcvBufSize on it.

  @see PS::Sock::IO::IReader::SetRcvBufSize
*/
void SetRcvBufSize
(
  void *  readerHandlePtr,
  int32   size
);

/**
  @brief A C style wrapper for PS::Sock::IO::IReader::GetPktCnt

  Defines a C style wrapper around PS::Sock::IO::IReader::GetPktCnt so that
  PS Stack, which is implemented in C, can get the number of packets in the
  data pipe. C code doesn't understand classes or namespaces and hence
  C wrappers are needed.

  readerHandlePtr points to the IReader object and this wrapper calls
  PS::Sock::IO::IReader::GetPktCnt on it.

  @see PS::Sock::IO::IReader::GetPktCnt
*/
uint16 GetPktCnt
(
  void *  readerHandlePtr
);

#endif /* __cplusplus */
#endif /* PS_SOCK_IO_IREADER_H */
