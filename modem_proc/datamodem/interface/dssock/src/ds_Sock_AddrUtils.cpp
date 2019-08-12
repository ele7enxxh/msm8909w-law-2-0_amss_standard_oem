/*===========================================================================
  FILE: ds_Sock_AddrUtils.cpp

  OVERVIEW: This file provides implementation of the AddrUtils class.

  DEPENDENCIES: None

  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_AddrUtils.cpp#1 $
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


#include <stringl.h>
#include "ds_Sock_AddrUtils.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_in.h"



using namespace ds::Sock;
using namespace ds::Error;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
ds::ErrorType AddrUtils::GetSockAddrStorage
(
  const ds::SockAddrIN6InternalType *  v6SockAddrPtr,
  ds::AddrFamilyType       family,
  ds::SockAddrInternalType*      sockAddrStoragePtr
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == v6SockAddrPtr || 0 == sockAddrStoragePtr)
  {
    LOG_MSG_ERROR_0("AddrUtils::GetSockAddrStorage(): "
                    "NULL args");
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    If family is V4, populate SockAddrStorageType with a V4 address, else with
    a V6 address
  -------------------------------------------------------------------------*/
  if (ds::AddrFamily::QDS_AF_INET == family)
  {
    /*-----------------------------------------------------------------------
      Since V6 address can't be returned as a V4 address, fail the operation
      if v6SockAddrPtr is a V6 address. Otherwise, return V4 address
    -----------------------------------------------------------------------*/
    if (false == PS_IN6_ARE_ADDR_EQUAL( v6SockAddrPtr->addr, &ps_in6addr_any) &&
        false == PS_IN6_IS_ADDR_V4MAPPED( v6SockAddrPtr->addr))
    {
      LOG_MSG_INVALID_INPUT_0("AddrUtils::GetSockAddrStorage(): "
                              "Can't copy V6 addr in to V4 storage");
      return QDS_EINVAL;
    }


    sockAddrStoragePtr->v4.family = ds::AddrFamily::QDS_AF_INET;
    sockAddrStoragePtr->v4.port   = v6SockAddrPtr->port;
    sockAddrStoragePtr->v4.addr   =
      PS_IN6_GET_V4_FROM_V4_MAPPED_V6( v6SockAddrPtr->addr);
  }
  else
  {


    memscpy( sockAddrStoragePtr, sizeof( ds::SockAddrInternalType), 
             v6SockAddrPtr, sizeof( ds::SockAddrIN6InternalType));

    sockAddrStoragePtr->v6.family = ds::AddrFamily::QDS_AF_INET6;
  }

  return AEE_SUCCESS;
} /* AddrUtils::GetSockAddrStorage() */


ds::ErrorType AddrUtils::GetSockAddrIN6
(
  const ds::SockAddrStorageType        sockAddrStoragePtr,
  ds::SockAddrIN6InternalType *        v6SockAddrPtr
)
{
   SockAddrINInternalType   v4SockAddrStorage={0};
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == v6SockAddrPtr)
  {
    LOG_MSG_ERROR_0("AddrUtils::GetSockAddrIN6(): "
                    "NULL v6SockAddr");
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    If sockAddrStoragePtr is NULL, return ps_in6addr_any and port 0
  -------------------------------------------------------------------------*/
  if (0 == sockAddrStoragePtr)
  {
    v6SockAddrPtr->family = ds::AddrFamily::QDS_AF_INET6;
    v6SockAddrPtr->port   = 0;
    memscpy( v6SockAddrPtr->addr, sizeof( INAddr6Type), &ps_in6addr_any, sizeof( ps_in6_addr));
    return AEE_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    If family in sockAddrStoragePtr is DSS_AF_INET, copy IPv4 mapped IPv6 address
    in to v6SockAddrPtr. If it is DSS_AF_INET6, cast sockAddrStoragePtr to
    SockAddrIN6InternalType and copy in to v6SockAddrPtr
  -------------------------------------------------------------------------*/
  (void)GetFamily(sockAddrStoragePtr,&family);

  if (ds::AddrFamily::QDS_AF_INET == family)
  {
    memscpy(&v4SockAddrStorage, sizeof(SockAddrINInternalType), 
            sockAddrStoragePtr, sizeof(SockAddrINInternalType));
    
    v6SockAddrPtr->family = ds::AddrFamily::QDS_AF_INET6;
    v6SockAddrPtr->port   = v4SockAddrStorage.port;

    PS_IN6_GET_V4_MAPPED_V6_FROM_V4( v6SockAddrPtr->addr,
                                     v4SockAddrStorage.addr);
  }
  else if (ds::AddrFamily::QDS_AF_INET6 == family)
  {
    memscpy( v6SockAddrPtr, sizeof(ds::SockAddrIN6InternalType),
             sockAddrStoragePtr, sizeof(ds::SockAddrIN6InternalType));
  }
  else
  {
    LOG_MSG_INVALID_INPUT_1("AddrUtils::GetSockAddrIN6(): "
                            "Unknown addr family %d", family);
    return QDS_EAFNOSUPPORT;
  }

  return AEE_SUCCESS;
} /* AddrUtils::GetSockAddrIN6() */




ds::ErrorType AddrUtils::GetSockAddrIN6
(
 const ds::SockAddrInternalType*    sockAddrStorage,
 ds::SockAddrIN6InternalType *        v6SockAddrPtr
 )
{
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (0 == v6SockAddrPtr)
   {
      LOG_MSG_ERROR_0("AddrUtils::GetSockAddrIN6(): "
                      "NULL v6SockAddr");
      return QDS_EFAULT;
   }

   /*-------------------------------------------------------------------------
   If sockAddrStoragePtr is NULL, return ps_in6addr_any and port 0
   -------------------------------------------------------------------------*/
   if (0 == sockAddrStorage)
   {
      v6SockAddrPtr->family = ds::AddrFamily::QDS_AF_INET6;
      v6SockAddrPtr->port   = 0;
      memscpy( v6SockAddrPtr->addr, sizeof( INAddr6Type),
               &ps_in6addr_any, sizeof( INAddr6Type));
      return AEE_SUCCESS;
   }

   /*-------------------------------------------------------------------------
   If family in sockAddrStoragePtr is DSS_AF_INET, copy IPv4 mapped IPv6 address
   in to v6SockAddrPtr. If it is DSS_AF_INET6, cast sockAddrStoragePtr to
   SockAddrIN6InternalType and copy in to v6SockAddrPtr
   -------------------------------------------------------------------------*/
   (void)GetFamily(sockAddrStorage,&family);

   if (ds::AddrFamily::QDS_AF_INET == family)
   {
      v6SockAddrPtr->family = ds::AddrFamily::QDS_AF_INET6;
      v6SockAddrPtr->port   = sockAddrStorage->v4.port;
      PS_IN6_GET_V4_MAPPED_V6_FROM_V4( v6SockAddrPtr->addr,
         sockAddrStorage->v4.addr);
   }
   else if (ds::AddrFamily::QDS_AF_INET6 == family)
   {
      memscpy( v6SockAddrPtr,  sizeof( SockAddrIN6InternalType),
               sockAddrStorage, sizeof( SockAddrIN6InternalType));
   }
   else
   {
      LOG_MSG_INVALID_INPUT_1("AddrUtils::GetSockAddrIN6(): "
                              "Unknown addr family %d", family);
      return QDS_EAFNOSUPPORT;
   }

   return AEE_SUCCESS;
} /* AddrUtils::GetSockAddrIN6() */


