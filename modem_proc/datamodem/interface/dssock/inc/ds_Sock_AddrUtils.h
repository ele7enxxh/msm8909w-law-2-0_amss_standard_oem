#ifndef DS_SOCK_ADDRUTILS_H
#define DS_SOCK_ADDRUTILS_H
/*===========================================================================
  @file SocketAddrUtils.h

  This file defines helper routines which translate SockAddrStorageType to
  SockAddrIN6InternalType and vice versa.

  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_AddrUtils.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Utils_StdErr.h"

#include <stringl.h>
#include "ds_Errors_Def.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_SockAddrInternalTypes.h"
#include "ds_Addr_Def.h"

#ifndef min
#define min(a,b)  ((a) < (b) ? (a):(b))
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    namespace AddrUtils
    {
      /*=====================================================================

                            PUBLIC FUNCTION DECLARATIONS

      =====================================================================*/
      /**
        @brief Translates SockAddrIN6InternalType to SockAddrInternalType.

        An address stored in SockAddrIN6InternalType format is translated in to
        SockAddrInternalType format based on socket family. If socket family is
        INET, a IPv4 address is copied in to SockAddrInternalType, else
        IPv6 address is copied.

        @param[in]  v6SockAddrPtr       SockAddrIN6InternalType instance to be
                                        translated
        @param[in]  sockFamily          Socket family
        @param[out] sockAddrStoragePtr  SockAddrInternalType instance in to
                                        which SockAddrIN6InternalType is translated

        @retval AEE_SUCCESS             SockAddrIN6InternalType is translated
                                        successfully
        @retval QDS_EFAULT   v6SockAddrPtr or sockAddrStoragePtr
                                        is NULL
        @retval QDS_EINVAL   sockFamily is INET but v6SockAddrPtr
                                        is not a IPv4 mapped IPv6 address

        @see ds::Error
      */
      ds::ErrorType GetSockAddrStorage
      (
        const ds::SockAddrIN6InternalType *  v6SockAddrPtr,
        ds::AddrFamilyType      family,
        ds::SockAddrInternalType *    sockAddrStoragePtr
      ) throw();

      /**
        @brief Translates SockAddrStorageType to SockAddrIN6InternalType.

        An address stored in SockAddrStorageType format is translated in to
        SockAddrIN6InternalType format based on address family in SockAddrStorageType
        instance. If address family is DSS_AF_INET, a IPv4 mapped IPv6 address
        is copied in to SockAddrIN6InternalType. If it is DSS_AF_INET6,
        SockAddrStorageType is cast to SockAddrIN6InternalType and is copied.

        If sockAddrStoragePtr is NULL, in6addr_any and 0 port is copied in to
        v6SockAddrPtr.

        @param[in]  sockAddrStoragePtr  SockAddrStorageType instance to be
                                        translated
        @param[out] v6SockAddrPtr       SockAddrIN6InternalType instance in to
                                        which SockAddrStorageType is translated

        @retval AEE_SUCCESS                  sAddrStorageType is translated
                                             successfully
        @retval QDS_EFAULT        v6SockAddrPtr is NULL
        @retval QDS_EAFNOSUPPORT  Address family in
                                             sockAddrStoragePtr is neither
                                             DSS_AF_INET nor DSS_AF_INET6

        @see ds::Error
      */
      ds::ErrorType GetSockAddrIN6
      (
        const ds::SockAddrStorageType        sockAddrStoragePtr,
        ds::SockAddrIN6InternalType *        v6SockAddrPtr
      );




       /**
        @brief Translates SockAddrInternalType to SockAddrIN6InternalType.

        An address stored in SockAddrInternalType format is translated in to
        SockAddrIN6InternalType format based on address family in SockAddrInternalType
        instance. If address family is DSS_AF_INET, a IPv4 mapped IPv6 address
        is copied in to SockAddrIN6InternalType. If it is DSS_AF_INET6,
        SockAddrInternalType is cast to SockAddrIN6InternalType and is copied.

        If sockAddrStoragePtr is NULL, in6addr_any and 0 port is copied in to
        v6SockAddrPtr.

        @param[in]  sockAddrPtr         SockAddrInternalType instance to be
                                        translated
        @param[out] v6SockAddrPtr       SockAddrIN6InternalType instance in to
                                        which SockAddrInternalType is translated

        @retval AEE_SUCCESS                  sockAddrPtr is translated
                                             successfully
        @retval QDS_EFAULT        v6SockAddrPtr is NULL
        @retval QDS_EAFNOSUPPORT  Address family in
                                             sockAddrPtr is neither
                                             DSS_AF_INET nor DSS_AF_INET6

        @see ds::Error
      */
      ds::ErrorType GetSockAddrIN6
      (
        const ds::SockAddrInternalType *   sockAddrPtr,
        ds::SockAddrIN6InternalType *        v6SockAddrPtr
      );





      /**
        @brief extracts family type from SockAddrInternalType 

        @param[in]  addrStoragePtr      SockAddrStorageType instance to extract family from
        @param[out] familyPtr           AddrFamilyType  instance in extract family to

        @retval AEE_SUCCESS             AddrFamilyType is extracted successfully
        @see ds::Error
      */
      inline ds::ErrorType GetFamily
      (
        const ds::SockAddrInternalType* addrStoragePtr,
        ds::AddrFamilyType* familyPtr
      ) throw()
      {
        *familyPtr = addrStoragePtr->v4.family;
        return AEE_SUCCESS;
      };


      /**
        @brief extracts family type from SockAddrStorageType 

        Family type is stored inside first 2 bytes of array which is inside the
        SockAddrStorageType, it is copied to provided output param

        @param[in]  addrStoragePtr      SockAddrStorageType instance to extract family from
        @param[out] familyPtr           AddrFamilyType  instance in extract family to

        @retval AEE_SUCCESS             AddrFamilyType is extracted successfully
        @see ds::Error
      */
      inline ds::ErrorType GetFamily
      (
        const ds::SockAddrStorageType addrStorage,
        ds::AddrFamilyType*           familyPtr
      ) throw()
      {
        memsmove((void*)(familyPtr),sizeof(ds::AddrFamilyType),
                 addrStorage,sizeof(ds::AddrFamilyType));
        return AEE_SUCCESS;
      };

      /**
        @brief puts family type to SockAddrStorageType 

        Family type is stored inside first 2 bytes of array which is inside the
        SockAddrStorageType, they are filled with family type from input param

        @param[in]  addrStoragePtr      SockAddrStorageType instance to fill family to 
        @param[out] familyPtr           AddrFamilyType  instance to fill family from

        @retval AEE_SUCCESS             AddrFamilyType is filled successfully
        @see ds::Error
      */
      inline ds::ErrorType SetFamily
      (
        ds::SockAddrStorageType addrStorage,
        ds::AddrFamilyType      family
      )
      {
        memsmove(addrStorage, sizeof(ds::SockAddrStorageType), 
                 (const void*)(&family),sizeof(ds::AddrFamilyType));
        return AEE_SUCCESS;
      };

      /**
        @brief puts family type to SockAddrInternalType 

        @param[in]  addrStorage         SockAddrInternalType instance to fill family to 
        @param[out] familyPtr           AddrFamilyType  instance to fill family from

        @retval AEE_SUCCESS             AddrFamilyType is filled successfully
        @see ds::Error
      */
      inline ds::ErrorType SetFamily
      (
        ds::SockAddrInternalType* addrStorage,
        ds::AddrFamilyType family
      )
      {
        addrStorage->v4.family = family;
        return AEE_SUCCESS;
      };

    } /* namespace AddrUtils */
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_ADDRUTILS_H */
