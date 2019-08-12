#ifndef PS_SOCK_PLATFORM_ISOCKETFACTORY_H
#define PS_SOCK_PLATFORM_ISOCKETFACTORY_H
/*===========================================================================
  @file IPlatformSocketFactory.h

  This file defines the interface for Sockets Platform. This interface
  abstracts various platforms such as WinMobile, Linux, and BMP etc. from
  the sockets library and provides methods to create a PlatformSocket.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/PS_Sock_Platform_ISocketFactory.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "PS_Sock_Platform_ISocket.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_StdErr.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
//TODO Document Class
namespace PS
{
  namespace Sock
  {
    namespace Platform
    {
      class ISocketFactory
      {                                                       /*lint !e578 */
        public:
          /**
            @brief Returns an ISocketFactory object.

            Returns an ISocketFactory object.

            @param None

            @retval address  Socket factory is created successfully
            @retval 0        Out of memory
          */
          static Platform::ISocketFactory * CreateInstance
          (
            void
          );

          /**
            @brief Creates an ISocket object.
            //TODO Document critSect

            Creates an ISocket object using the given family, sockType, and
            protocol parameters. ds::Sock::Family, ds::Sock::Type, and
            ds::Sock::Protocol define the supported options for family,
            sockType, and protocol respectively. If protocol is UNSPEC, TCP is
            used for STREAM sockets and UDP is used for DGRAM sockets.

            This method fails to create an ISocket object if
              @li Unsupported values are passed for family, sockType, or
                  protocol OR
              @li TCP is not used as protocol for STREAM sockets OR
              @li UDP or ICMP is not used as protocol for DGRAM sockets

            If the operation fails, dsErrnoPtr is populated with the failure
            reason. Possible values are
              @li QDS_EINVAL          Family is invalid
              @li QDS_ESOCKNOSUPPORT  Type is not supported
              @li QDS_EPROTONOSUPPORT Protocol is not supported
              @li QDS_EPROTOTYPE      (sockType, protocol)
                                                 combination is not
                                                 supported
              @retval QDS_EMFILE      Out of memory

            @param[in]  family      The family of socket to be created
            @param[in]  sockType    The type of socket to be created
            @param[in]  protocol    The protocol to be used
            @param[out] dsErrnoPtr  Updated with error value if the operation
                                    fails

            @retval object  Operation completed successfully
            @retval -1      Operation failed, and dsErrnoPtr is populated with
                            failure reason

            @see ds::Sock::Family, ds::Sock::Type, ds::Sock::Protocol,
                 ds::Error
          */
          virtual Platform::ISocket * CreateSocket
          (
            ds::AddrFamilyType      family,
            ds::Sock::SocketType    sockType,
            ds::Sock::ProtocolType  protocol,
            ICritSect *             critSectPtr,
            ds::ErrorType *         dsErrnoPtr
          ) = 0;

	  virtual ~ISocketFactory() {}

          /**
            @brief IQI interface Methods
          */
          DSIQI_IMPL_DEFAULTS_NO_QI()

      };
    } /* namespace Platfrom */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_PLATFORM_ISOCKETFACTORY_H */
