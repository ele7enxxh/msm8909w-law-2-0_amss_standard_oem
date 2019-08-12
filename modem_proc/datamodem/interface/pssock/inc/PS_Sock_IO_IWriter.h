#ifndef PS_SOCK_IO_IWRITER_H
#define PS_SOCK_IO_IWRITER_H
/*===========================================================================
  @file PS_Sock_IO_IWriter.h

  This file defines an interface to write data to a data pipe.

  Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/PS_Sock_IO_IWriter.h#1 $
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
#include "PS_Sock_IO_IWriterListener.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*lint -esym(1510, IWriter) */
namespace PS
{
  namespace Sock
  {
    namespace IO
    {
      /**
        @brief Defines an interface to write data to a data pipe.

        Defines methods which allows the user to write to a data pipe.
        The data pipe could be a queue or a watermark and this interface can
        be implemented differently to support writing to them.

        Each IWriter is associated with a IReader and they together form a
        data pipe. The interface assumes that same class implements both
        IReader and IWriter interfaces and hence provides a method to release
        the writer side. It is not a clean interface separation but gets the
        job done without proliferating the number of interfaces.
      */
      class IWriter
      {
        public:
          /**
            @brief Writes data from one or more IOVec buffers.

            Writes data from an array of IOVec buffers in to the data pipe.
            User specifies the number of buffers in the array and the length
            of each buffer. This method also associates meta data with the
            newly written data.

            A data pipe has a size which is defined by SetSndBufSize or
            SetRcvBufSize and it is always ensured that this size is not
            exceeded. Refer to implementation of this interface for how it is
            handled when size of data is more than the available space in the
            data pipe.

            If the operation fails, psErrnoPtr is populated with the failure
            reason. Possible values are
              @li DS_EOPNOTSUPP   Implementation doesn't support meta data
              @li DS_EINVAL       IWriter object is in invalid state and can't
                                  accept any more writes from the caller
              @li DS_EWOULDBLOCK  If data pipe is already full or if it can't
                                  accept data because size is more than the
                                  available space in the data pipe or because
                                  DSM items are exhausted

            @param[in]   ioVecArr     Array of IOVec buffers which contain the
                                      data to write to the data pipe updated
                                      with read data
            @param[in]   numIOVecArr  Number of IOVec buffers in the array
            @param[in]   metaDataPtr  Ptr to a buffer containing meta data.
                                      If non-NULL, meta data is associated
                                      with the written data. User could read
                                      the meta data along with the actual
                                      data using PS::Sock::IO::IReader::Read
            @param[in]   metaDataLen  Length of meta data
            @param[out]  psErrnoPtr   Updated with error value if operation
                                      fails

            @retval  "+ve value"  Number of written bytes. Could be 0 too
            @retval  -1           Operation failed and psErrnoPtr is updated
                                  with reason
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
            @brief Writes data from a DSM chain.

            Writes data from a DSM chain to the data pipe. User also specifies
            the size of data in the DSM chain. Taking length from user is not
            a clean interface, but PS stack knows the length from the TCP/UDP
            header aleady and its a lot of overhead in the data path to
            compute length using dsm_length_packet(). This method also
            associates meta data with the newly written data.

            Since DSM item is already allocated by the caller, data pipe's
            size is not enforced. The main reason for having size is not to
            hold on to lot of DSM items. It is assumed that caller doesn't use
            both writing DSM items and writing IOVec buffers. If caller does,
            size must be enforced for this method.

            Once DSM item is written to the data pipe, it must not be
            manipulated by the user. Otherwise, data pipe gets in to an
            incorrect state. To ensure this, DSM item ptr is set to 0 on
            successful return from this method.

            If the operation fails, psErrnoPtr is populated with the failure
            reason. Possible values are
              @li DS_EFAULT       Caller passed a NULL DSM item
              @li DS_EOPNOTSUPP   Implementation doesn't support meta data
              @li DS_EINVAL       IWriter object is in invalid state and can't
                                  accept any more writes from the caller

            @param[in,out]  dsmItemPtrPtr  A double ptr to DSM chain containing
                                           the data to write to the data pipe.
                                           Set to 0 upon return.
            @param[in]      bufLen         Length of DSM item
            @param[in]      metaDataPtr    Ptr to a buffer containing
                                           meta data. If non-NULL, meta data
                                           is associated with the written data.
                                           User could read the meta data along
                                           with the actual data using
                                           PS::Sock::IO::IReader::Read
            @param[in]      metaDataLen    Length of meta data
            @param[out]     psErrnoPtr     Updated with error value if
                                           operation fails

            @retval  "+ve value"  Number of written bytes. Could be 0 too
            @retval  -1           Operation failed and psErrnoPtr is updated
                                  with reason
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
            @brief Releases writer.

            A data pipe has two sides, a reader and a writer. This method
            releases the writer side of a data pipe. Once the writer is
            released, subsequent writes are silently discarded. The reader can
            continue to read though.

            @param  None

            @return  None
          */
          virtual void ReleaseWriter
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
          virtual void SetSndBufSize
          (
            int32  size
          ) = 0;

          /**
            @brief Returns the amount of data in the data pipe.

            Returns the amount of outstanding data in the data pipe.

            @param  None

            @return  Amount of data in the pipe
          */
          virtual int32 GetSndCnt
          (
            void
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
          virtual int32 SetWrCritSection
          (
            ICritSect *            critSectPtr,
            int16 *                psErrnoPtr
          ) = 0;

          /**
            @brief Registers a listener.

            Registers a listener for events notified by the writer.

            @param[in]  writerListenerPtr  Listener to register

            @return  None
          */
          virtual void RegisterWriterListener
          (
            IWriterListener *  writerListenerPtr
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
  @brief A C style wrapper for PS::Sock::IO::IWriter::Write

  Defines a C style wrapper around PS::Sock::IO::IWriter::Write so that
  PS Stack, which is implemented in C, can write data to a data pipe. C code
  doesn't understand classes or namespaces and hence C wrappers are needed.

  writerHandlePtr points to the IWriter object and this wrapper calls
  PS::Sock::IO::IWriter::Write on it.

  @see PS::Sock::IO::IWriter::Write
*/
int32 Write
(
  void *            writerHandlePtr,
  dsm_item_type **  dsmItemPtrPtr,
  int32             length,
  char *            metaDataPtr,
  int32             metaDataLen,
  int16 *           psErrnoPtr
);

/**
  @brief A C style wrapper for PS::Sock::IO::IWriter::ReleaseWriter

  Defines a C style wrapper around PS::Sock::IO::IWriter::ReleaseWriter so that
  PS Stack, which is implemented in C, can release writer side of a data pipe.
  C code doesn't understand classes or namespaces and hence C wrappers are
  needed.

  writerHandlePtr points to the IWriter object and this wrapper calls
  PS::Sock::IO::IWriter::ReleaseWriter on it.

  @see PS::Sock::IO::IWriter::ReleaseWriter
*/
void ReleaseWriter
(
  void *  writerHandlePtr
);

/**
  @brief A C style wrapper for PS::Sock::IO::IWriter::SetSndBufSize

  Defines a C style wrapper around PS::Sock::IO::IWriter::SetSndBufSize so that
  PS Stack, which is implemented in C, can set a size limit on a data pipe.
  C code doesn't understand classes or namespaces and hence C wrappers are
  needed.

  writerHandlePtr points to the IWriter object and this wrapper calls
  PS::Sock::IO::IWriter::SetSndBufSize on it.

  @see PS::Sock::IO::IWriter::SetSndBufSize
*/
void SetSndBufSize
(
  void *  writerHandlePtr,
  int32   size
);

/**
  @brief A C style wrapper for PS::Sock::IO::IWriter::GetSndCnt

  Defines a C style wrapper around PS::Sock::IO::IWriter::GetSndCnt so that
  PS Stack, which is implemented in C, can get the amount of data in a
  data pipe. C code doesn't understand classes or namespaces and hence
  C wrappers are needed.

  writerHandlePtr points to the IWriter object and this wrapper calls
  PS::Sock::IO::IWriter::GetSndCnt on it.

  @see PS::Sock::IO::IWriter::GetSndCnt
*/
int32 GetSndCnt
(
  void *  writerHandlePtr
);

#endif /* __cplusplus */
#endif /* PS_SOCK_IO_IWRITER_H */
