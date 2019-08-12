#ifndef PS_SOCK_IO_SOCKETIOFACTORY_H
#define PS_SOCK_IO_SOCKETIOFACTORY_H
/*===========================================================================
  @file PS_Sock_IO_SocketIOFactory.h

  This class acts as a factory for IReader and IWriter interfaces.

  Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/PS_Sock_IO_SocketIOFactory.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"

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
        @brief A factory class which creates IReader and IWriter interfaces.

        Clients of SocketIO have exposure to the abstract IReader and IWriter
        interfaces and not to the concrete ByteStream and DgramQueue classes.
        This class provides a factory interface to the clients and depending
        on the protocol type, creates appropriate concrete objects.

        @see PS::Sock::IO::ByteStream, PS::Sock::IO::DgramQueue
      */
      class SocketIOFactory
      {
        public:
          /**
            @brief Returns concrete reader and writer objects.

            Given protocol used by the client, this method creates concrete
            reader and writer objects.

            If protocol is
              @li TCP, uses PS::Sock::IO::ByteStream
              @li UDP or ICMP, uses PS::Sock::IO::DgramQueue

            @param[in]   protocol      Protocol used by client which determines
                                       the type of concrete implementation to
                                       use
            @param[out]  readerPtrPtr  Updated with reader object
            @param[out]  writerPtrPtr  Updated with writer object
            @param[in]   critSectPtr   Used by concrete implementation to
                                       provide mutual exclusion between reader
                                       and writer to the data pipe

            @retval AEE_SUCCESS         Reader/Writer are created
            @retval EMFILE   Out of memory
            @retval EFAULT   NULL arguments
          */
          static ds::ErrorType GetReaderAndWriter
          (
            ds::Sock::ProtocolType  protocol,
            IReader **              readerPtrPtr,
            IWriter **              writerPtrPtr,
            ICritSect *             critSectPtr
          );
      };

    } /* namespace IO */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_IO_SOCKETIOFACTORY_H */
