/*===========================================================================
  FILE: NetworkIPv6.cpp

  OVERVIEW: This file provides implementation of the NetworkIPv6 class.

  DEPENDENCIES: None

  Copyright (c) 2007 -2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkIPv6.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-30 kr  Free floating changes
  2009-02-27 hm  Added IPv6 priv addr and prefixes support.
  2008-04-07 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_Conversion.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_NetworkIPv6.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Net_NetworkIPv6Address.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"
#include "ps_ip6_addr.h"
#include "ps_system_heap.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace ds::Net::Conversion;
using namespace NetPlatform;

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
NetworkIPv6::NetworkIPv6
(
  int32      ifaceHandle
)
: Handle (ifaceHandle),
  mpSigBusPrefixChanged (0),
  refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("NetworkIPv6::NetworkIPv6(): Obj 0x%p, handle 0x%x",
                            this, ifaceHandle);
} /* NetworkIPv6::NetworkIPv6() */


void NetworkIPv6::SetIfaceHandle
 (
   int32 objHandle
 )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("NetworkIPv6::SetIfaceHandle(): Obj 0x%p, handle 0x%x",
                             this, objHandle);
  SetHandle(objHandle);
  return;
} /* NetworkIPv6::SetIfaceHandle */

ds::ErrorType NetworkIPv6::Init
(
  void
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusPrefixChanged);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("NetworkIPv6::Init(): "
                       "Couldn't create SignalBus, obj 0x%x", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = Handle::Init(EventManager::networkIPv6ObjList);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkIPv6::Init() :"
                       "Failed to init the Handle, obj 0x%p", this);
      break;
    }

  } while (0);

  /*-------------------------------------------------------------------------
    Cleanup in case of error (::Destructor won't do this in case of error)
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    DS_UTILS_RELEASEIF (mpSigBusPrefixChanged);
  }

  LOG_MSG_FUNCTION_EXIT_2 ("NetworkIPv6::Init(): Result 0x%x, obj 0x%p",
                           result, this);
  return result;

} /* NetworkIPv6::Init() */

void NetworkIPv6::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Due to 2 stage constructor pattern - NetworkIPv6::Init can fail.
    No need to free resources (including Handle::Destructor call)
    in this certain case as it is done in NetworkIPv6::Init
  -------------------------------------------------------------------------*/
  if (0 == mpICritSect)
  {
    LOG_MSG_ERROR_1("NetworkIPv6::Destructor(): Obj 0x%p "
                    "NetworkIPv6 wasn't fully initialized", this);
    return;
  }

  mpICritSect->Enter();

  DS_UTILS_RELEASEIF (mpSigBusPrefixChanged);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor for the base class.
  -------------------------------------------------------------------------*/
  Handle::Destructor();

  mpICritSect->Leave();

} /* NetworkIPv6::Destructor() */

NetworkIPv6::~NetworkIPv6
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkIPv6::~NetworkIPv6(): Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* NetworkIPv6::~NetworkIPv6() */

/*---------------------------------------------------------------------------
  Functions inherited from IQI interface.
---------------------------------------------------------------------------*/
ds::ErrorType NetworkIPv6::QueryInterface
(
  AEEIID   iid,
  void **  ppo
)
throw()
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("NetworkIPv6::QueryInterface(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (iid)
  {
    case AEEIID_INetworkIPv6:
    case AEEIID_IQI:
    {
      *ppo   = static_cast <INetworkIPv6 *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_INetworkIPv6Priv:
    {
      *ppo   = static_cast <INetworkIPv6Priv *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    default:
    {
      result = AEE_ECLASSNOTSUPPORT;
      break;
    }
  } /* switch (iid) */

  LOG_MSG_FUNCTION_EXIT_4 ("NetworkIPv6::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p, iid 0x%x",
                           result, *ppo, this, iid);
  mpICritSect->Leave();

  return result;
} /* NetworkIPv6::QueryInterface() */


/*---------------------------------------------------------------------------
  Forwarder functions for Handle object.
---------------------------------------------------------------------------*/
ds::ErrorType NetworkIPv6::OnStateChange
(
  ISignal *           signalObj,
  ds::Net::EventType  eventID,
  IQI **              regObj
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (IPv6PrivEvent::QDS_EV_PREFIX_CHANGED != eventID)
  {
    LOG_MSG_INVALID_INPUT_2 ("NetworkIPv6::OnStateChange(): "
                             "Unsupported event 0x%x, obj 0x%p",
                             eventID, this);
    return QDS_EINVAL;
  }

  mpICritSect->Enter();

  result = Handle::OnStateChange(signalObj, eventID, regObj);

  LOG_MSG_FUNCTION_EXIT_4 ("NetworkIPv6::OnStateChange(): "
                           "Result 0x%x, obj 0x%p , event 0x%x, signal 0x%p",
                            result, this, eventID, signalObj);
  mpICritSect->Leave();

  return result;
} /* NetworkIPv6::OnStateChange() */

ds::ErrorType NetworkIPv6::GeneratePrivAddr
(
  boolean          isUnique,
  IIPv6Address **  ppIPv6PrivAddr
)
{
  NetworkIPv6Address *        pNetIPv6Addr = 0;
  IPv6PrivAddrIoctlArgType    ioctlArg;
  PSIPAddrType                psPrivIPv6Addr;
  ds::INAddr6Type             dsPrivIPv6Addr = {0};
  ds::ErrorType               result;
  int32                       ifaceHandle;
  IPv6AddrStateType           AddrState;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == ppIPv6PrivAddr)
  {
    LOG_MSG_INVALID_INPUT_1 ("NetworkIPv6::GeneratePrivAddr(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Call the IOCTL to generate IPv6 Priv Addr
    -----------------------------------------------------------------------*/
    ifaceHandle = GetHandle();

    memset (&ioctlArg, 0, sizeof (ioctlArg));
    memset (&psPrivIPv6Addr, 0, sizeof (psPrivIPv6Addr));

    ioctlArg.ip_addr              = (ip_addr_type *) &psPrivIPv6Addr;
    ioctlArg.iid_params.is_unique = isUnique;

    result = IfaceIoctl (ifaceHandle,
                         IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR,
                         static_cast <void *> (&ioctlArg));

    if (AEE_SUCCESS == result)
    {
      (void) PS2DSIPAddr (&psPrivIPv6Addr, dsPrivIPv6Addr);
      AddrState = IPv6AddrState::PRIV_ADDR_AVAILABLE;
    }
    else if (AEE_EWOULDBLOCK == result)
    {
      AddrState = IPv6AddrState::PRIV_ADDR_WAITING;
    }
    else
    {
      LOG_MSG_ERROR_1 ("NetworkIPv6::GeneratePrivAddr(): "
                       "Failed to generate private addr, obj 0x%p", this);
      break;
    }

    //TODO Combine contruction and init in to a CreateInstance method
    /*-----------------------------------------------------------------------
      Generate a NetworkIPv6Addr object in the correct state.
    -----------------------------------------------------------------------*/
    pNetIPv6Addr =
      new NetworkIPv6Address (dsPrivIPv6Addr, AddrState);
    if (0 == pNetIPv6Addr)
    {
      LOG_MSG_ERROR_1 ("NetworkIPv6::GeneratePrivAddr(): "
                       "Failed to alloc NetworkIPv6Address, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = pNetIPv6Addr->Init(ifaceHandle);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    *ppIPv6PrivAddr = static_cast <IIPv6Address *> (pNetIPv6Addr);
  } while (0);

  if (AEE_SUCCESS != result)
  {
    DS_UTILS_RELEASEIF(pNetIPv6Addr);
  }

  LOG_MSG_FUNCTION_EXIT_4 ("NetworkIPv6::GeneratePrivAddr(): "
                           "Result 0x%x, IIPv6Address 0x%p, obj 0x%p,  unique %d",
                           result, *ppIPv6PrivAddr, this, isUnique);
  mpICritSect->Leave();

  return result;
} /* NetworkIPv6::GeneratePrivAddr() */

/*-------------------------------------------------------------------------
  Methods from INetworkIPv6Priv interface
-------------------------------------------------------------------------*/
ds::ErrorType NetworkIPv6::GetAllIPv6Prefixes
(
  IPv6PrivPrefixInfoType *  pPrefixes,
  int                       allPrefixesLen,
  int *                     allPrefixesLenReq
)
{
  IPv6GetAllPrefixesType  ioctlArg;
  ds::ErrorType           result;
  int                     index;
  int                     seqNumItems = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Prepare the IOCTL arguments to get IPv6 prefixes
  -------------------------------------------------------------------------*/
  memset (&ioctlArg, 0, sizeof (ioctlArg));

  PS_SYSTEM_HEAP_MEM_ALLOC(ioctlArg.prefix_info_ptr,
                           sizeof(IPv6PrefixInfoType) * MAX_IPV6_PREFIXES,
                           IPv6PrefixInfoType*);

  if (0 == ioctlArg.prefix_info_ptr)
  {
    return AEE_ENOMEMORY;
  }

  ioctlArg.num_prefixes = MAX_IPV6_PREFIXES;

  mpICritSect->Enter();

  do
  {
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_ALL_V6_PREFIXES,
                         static_cast <void *> (&ioctlArg));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = ds::Utils::Conversion::ProcessLenReq (allPrefixesLen,
                                                   allPrefixesLenReq,
                                                   ioctlArg.num_prefixes,
                                                   &seqNumItems);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1("NetworkIPv6::GetAllIPv6Prefixes(): Failed to convert "
                      "IPv6 prefixes in PS format to QCM format, "
                      "obj 0x%p", this);
      break;
    }

    for (index = 0; index < seqNumItems && 0 != pPrefixes; index++)
    {
       PS2DSIPv6Prefix (&ioctlArg.prefix_info_ptr[index], &pPrefixes[index]);
    }
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE(ioctlArg.prefix_info_ptr);

  LOG_MSG_FUNCTION_EXIT_2 ("NetworkIPv6::GetAllIPv6Prefixes(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* NetworkIPv6::GetAllIPv6Prefixes() */

ds::ErrorType NetworkIPv6::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppISigBus)
  {
    LOG_MSG_INVALID_INPUT_1 ("NetworkIPv6::GetSignalBus(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (eventID)
  {
    case IPv6PrivEvent::QDS_EV_PREFIX_CHANGED:
    {
      *ppISigBus = mpSigBusPrefixChanged;
      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("NetworkIPv6::GetSignalBus(): "
                               "Unknown event 0x%x, obj 0x%p", eventID, this);
      result = QDS_EINVAL;
      break;
    }
  }

  /*-------------------------------------------------------------------------
    Handle the case where event is supported by this object.
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS == result)
  {
    if (0 == *ppISigBus)
    {
      LOG_MSG_INFO1_2 ("NetworkIPv6::GetSignalBus(): "
                       "NULL signal bus for event 0x%x, obj 0x%p",
                       eventID, this);
      result = QDS_EINVAL;
    }
    else
    {
      result = AEE_SUCCESS;
      (void) (*ppISigBus)->AddRef();
    }
  }

  LOG_MSG_FUNCTION_EXIT_4 ("NetworkIPv6::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p, event 0x%x",
                           result, *ppISigBus, this, eventID);
  mpICritSect->Leave();

  return result;
} /* NetworkIPv6::GetSignalBus() */
