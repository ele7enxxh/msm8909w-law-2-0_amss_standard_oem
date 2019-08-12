/*===========================================================================
  FILE: Network.cpp

  OVERVIEW: This file provides implementation of the Network class.

  DEPENDENCIES: None

  Copyright (c) 2008-2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Network.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2011-09-30 kr  Free floating changes
  2011-08-31 hs  Added GetSubnetMask
  2011-08-30 hs  Added IOCTL GetGatewayAddress.
  2011-08-24 hs  Added IOCTL GetDeviceName
  2010-06-02 cp  IOCTL changes for Soft AP DHCP server.
  2009-02-27 hm  Added IPv6 priv addr and prefixes support.
  2008-03-10 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include <stringl/stringl.h>

#include <string.h>
#include "ds_Utils_StdErr.h"
#include "ds_Net_INetwork1x.h"
#include "ds_Net_INetwork1xPriv.h"
#include "ds_Net_INetworkUMTS.h"
#include "ds_Net_IMCastMBMSCtrlPriv.h"
#include "ds_Net_INetworkControl.h"
#include "ds_Net_IFirewallManager.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_Conversion.h"
#include "ds_Net_Network.h"
#include "ds_Net_Platform.h"
#include "ds_Net_BearerTech.h"
#include "ds_Net_BearerTechEx.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Net_Network1X.h"
#include "ds_Net_NetworkUMTS.h"
#include "ds_Net_MCastMBMSCtrl.h"
#include "ds_Net_NetworkIPv6.h"
#include "ds_Net_NetworkFactory.h"
#include "ds_Net_QoSDefault.h"
#include "ds_Net_QoSSecondary.h"
#include "ds_Net_MCastSession.h"
#include "ds_Net_IPFilterSpec.h"
#include "ds_Net_FirewallManager.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"
#include "ps_system_heap.h"
#include "ds_Utils_SockAddrInternalTypes.h"
#include "ps_policy_mgr.h"
#include "ds_sys.h"

using namespace ds::Net;
using namespace ds::Net::Conversion;
using namespace ds::Error;
using namespace NetPlatform;

/*===========================================================================

                     PRIVATE FUNCTION DEFINITIONS

===========================================================================*/
ds::ErrorType Network::IPAddrIOCTL
(
  IfaceIoctlEnumType   ioctlName,
  ds::IPAddrType*      pDSIPAddr
)
{
  ds::ErrorType        result;
  PSIPAddrType         psIPAddr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validation.
  -------------------------------------------------------------------------*/
  if (0 == pDSIPAddr)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::IPAddrIOCTL(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Get the iface handle and call the relevant IOCTL
    -----------------------------------------------------------------------*/
    memset (&psIPAddr, 0, sizeof(PSIPAddrType));

    result = IfaceIoctl (GetHandle(),
                         ioctlName,
                         static_cast <void *> (&psIPAddr));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert IOCTL result back to ds::IPAddrType
    -----------------------------------------------------------------------*/
    result = PS2DSIPAddr (&psIPAddr, pDSIPAddr);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::IPAddrIOCTL(): Result 0x%x, IOCTL %d, "
                           "obj 0x%p", result, ioctlName, this);

  mpICritSect->Leave();
  return result;

} /* Network::IPAddrIOCTL() */

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
Network::Network
(
  Policy *          argPolicyPtr,
  NetworkModeType   networkMode
) : Handle(),
    mNetworkMode(networkMode),
    mFlags(0),
    mLastNetDownReason(0),
    mpPolicy (argPolicyPtr),
    mpIPFilterReg (NULL),
    mWouldblock(FALSE),
    mBringupAgain (FALSE),
    mBringupFirst (FALSE),
    mTeardown (FALSE),
    mOutageEventOccurred(FALSE),
    mFastDormStatus(0),

    // state change
    mpSigBusIfaceUp(0),
    mpSigBusIfaceGoingDown(0),
    mpSigBusIfaceDown(0),
    mpSigBusIfaceActiveOutOfUse(0),
    mpSigBusIfaceComingUp(0),
    mpSigBusIfaceConfiguring(0),
    mpSigBusIfaceRouteable(0),

    mpTechObjNetwork1x (NULL),
    mpTechObjNetwork1xPriv (NULL),
    mpNetworkUMTS (NULL),
    mpNetworkIPv6 (NULL),
    mCachedPreviousState(NetworkState::QDS_CLOSED),
    mAppPriority(PS_POLICY_MGR_LEGACY_PRIORITY),
    refCnt(1),
    weakRefCnt(1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("Network::Network(): Obj 0x%p, mode %d, argPolicyPtr 0x%x",
                            this, networkMode, argPolicyPtr);

  //TODO Move this to Init function
  if (0 == argPolicyPtr)
  {
    ASSERT (0);
    return;
  }

  (void) mpPolicy->AddRef();

  mLastNetDownReason = NetDownReason::QDS_NOT_SPECIFIED;

} /* Network::Network() */

ds::ErrorType Network::Init
(
  void
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Create signal buses to hold event registration info
    -----------------------------------------------------------------------*/
    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusIfaceUp);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network::Init(): Failed to alloc mpSigBusIfaceUp, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusIfaceGoingDown);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network::Init(): Failed to alloc mpSigBusIfaceGoingDown, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusIfaceDown);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network::Init(): Failed to alloc mpSigBusIfaceDown, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusIfaceActiveOutOfUse);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network::Init(): Failed to alloc mpSigBusIfaceActiveOutOfUse, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusIfaceComingUp);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network::Init(): Failed to alloc mpSigBusIfaceComingUp, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusIfaceConfiguring);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network::Init(): Failed to alloc mpSigBusIfaceConfiguring, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusIfaceRouteable);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network::Init(): Failed to alloc mpSigBusIfaceRouteable, obj 0x%p",
                       this);
      break;
    }


    result = Handle::Init(EventManager::networkObjList);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1("Network::Init() :  "
                      "Handle::Init failed", this);
      
      break;
    }

    result = AEE_SUCCESS;

  } while (0);

  /*-------------------------------------------------------------------------
    Cleanup in case of error (::Destructor won't do this in case of error)
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    DS_UTILS_RELEASEIF (mpPolicy);

    // state change
    DS_UTILS_RELEASEIF (mpSigBusIfaceUp);
    DS_UTILS_RELEASEIF (mpSigBusIfaceGoingDown);
    DS_UTILS_RELEASEIF (mpSigBusIfaceDown);
    DS_UTILS_RELEASEIF (mpSigBusIfaceActiveOutOfUse);
    DS_UTILS_RELEASEIF (mpSigBusIfaceComingUp);
    DS_UTILS_RELEASEIF (mpSigBusIfaceConfiguring);
    DS_UTILS_RELEASEIF (mpSigBusIfaceRouteable);
  }

  return result;

} /* Network::Init() */

void Network::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Due to 2 stage constructor pattern - Network::Init can fail.
    No need to free resources (including Handle::Destructor call)
    in this certain case as it is done in Network::Init
  -------------------------------------------------------------------------*/
  if (0 == mpICritSect)
  {
    LOG_MSG_ERROR_1("Network::Destructor(): Obj 0x%p "
                    "Network wasn't fully initialized", this);
    return;
  }

  mpICritSect->Enter();

  /*lint -save -e1550, -e1551 */

  /*-------------------------------------------------------------------------
    Release underlying PS_IFACE, if any is associated. Base class
    destructor is invoked after derived class is destroyed. So we can be
    confident that mpICritSect (maintained by Handle base object) would be
    valid during the course of destructor.
  -------------------------------------------------------------------------*/
  (void) Stop();

  if (0 != mpTechObjNetwork1x)
  {
    DS_UTILS_RELEASEIF (mpTechObjNetwork1x);

    mpTechObjNetwork1xPriv = 0;
  }
  /*lint -restore */

  DS_UTILS_RELEASEIF (mpPolicy);
  DS_UTILS_RELEASEIF (mpNetworkUMTS);
  DS_UTILS_RELEASEIF (mpNetworkIPv6);
  DS_UTILS_RELEASEIF (mpIPFilterReg);

  // state change
  DS_UTILS_RELEASEIF (mpSigBusIfaceUp);
  DS_UTILS_RELEASEIF (mpSigBusIfaceGoingDown);
  DS_UTILS_RELEASEIF (mpSigBusIfaceDown);
  DS_UTILS_RELEASEIF (mpSigBusIfaceActiveOutOfUse);
  DS_UTILS_RELEASEIF (mpSigBusIfaceComingUp);
  DS_UTILS_RELEASEIF (mpSigBusIfaceConfiguring);
  DS_UTILS_RELEASEIF (mpSigBusIfaceRouteable);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor of the base class.
  -------------------------------------------------------------------------*/
  Handle::Destructor();

  mpICritSect->Leave();

} /* Network::Destructor() */

Network::~Network
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* Network::~Network() */

ds::ErrorType Network::CreateQoSManager
(
  IQoSManager **  ppIQoSMgr
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppIQoSMgr)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::CreateQoSManager(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Create a new QoSManager object all the time. If the network object's
      handle changes anytime, old QoSManager does not make sense.
    -----------------------------------------------------------------------*/
    *ppIQoSMgr = static_cast <IQoSManager *> (new QoSManager (GetHandle(),
                                                              mNetworkMode));
    if (0 == *ppIQoSMgr)
    {
      LOG_MSG_ERROR_1 ("Network::CreateQoSManager(): Failed to alloc QoSManager, "
                       "obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = AEE_SUCCESS;
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::CreateQoSManager(): "
                           "Result 0x%x, QoSManager 0x%p, obj 0x%p",
                           result, *ppIQoSMgr, this);
  mpICritSect->Leave();

  return result;
} /* Network::CreateQoSManager() */
#ifdef FEATUTE_DATA_PS_MCAST
ds::ErrorType Network::CreateMCastManager
(
  IMCastManager **  ppIMCastMgr
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppIMCastMgr)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::CreateMCastManager(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Create a new MCastManager object all the time. If the network object's
      handle changes anytime, old MCastManager does not make sense.
    -----------------------------------------------------------------------*/
    *ppIMCastMgr =
      static_cast<IMCastManager *> (new MCastManager (GetHandle()
                                                      mNetworkMode));
    if (0 == *ppIMCastMgr)
    {
      LOG_MSG_ERROR_1 ("Network::CreateMCastManager(): Failed to alloc "
                       "MCastManager, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = AEE_SUCCESS;
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::CreateMCastManager(): "
                           "Result 0x%x, MCastManager 0x%p, obj 0x%p",
                           result, *ppIMCastMgr, this);
  mpICritSect->Leave();

  return result;
} /* Network::CreateMCastManager() */
#endif // FEATUTE_DATA_PS_MCAST
ds::ErrorType Network::LookupInterface
(
  void
)
{
  ds::ErrorType  result;
  int32          ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  ifaceHandle = GetHandle();

  do
  {
    /*-----------------------------------------------------------------------
      If the interface is already brought up/looked up, return SUCCESS
    -----------------------------------------------------------------------*/
    if (0 != ifaceHandle)
    {
      LOG_MSG_INFO1_1 ("Network::LookupInterface(): Already brought up or "
                       "looked up, obj 0x%p", this);
      result = AEE_SUCCESS;
      break;
    }

    /*-----------------------------------------------------------------------
      Perform routing lookup and bring up the network by policy.
    -----------------------------------------------------------------------*/
    result =
      IfaceLookUpByPolicy (static_cast <IPolicy *> (mpPolicy), &ifaceHandle);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Set the interface obtained as the handle in Handle object.
    -----------------------------------------------------------------------*/
    SetHandle(ifaceHandle);
  } while (0);

  if(AEE_SUCCESS == result)
  {
    LOG_MSG_FUNCTION_EXIT_3 ("Network::LookupInterface(): "
                             "Result 0x%x, iface handle 0x%x, obj 0x%p",
                             result, ifaceHandle, this);
  }
  else
  {
    LOG_MSG_FUNCTION_EXIT_2 ("Network::LookupInterface(): "
                             "Wasn't able to get valid handle from PS"
                             "Result 0x%x, obj 0x%p", result, this);
  }
  
  mpICritSect->Leave();
  return result;

} /* Network::LookupInterface() */

ds::ErrorType Network::ForceLookupInterfaceByIfaceId
(
  ds::Net::IfaceIdType ifaceId
)
{
  Policy             tmpPolicy;
  ds::ErrorType      result;
  int32              ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the interface is already brought up/looked up, return SUCCESS
  -------------------------------------------------------------------------*/
  tmpPolicy.SetIfaceId(ifaceId);

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Perform routing lookup and bring up the network by policy.
    -----------------------------------------------------------------------*/
    result =
      IfaceLookUpByPolicy (static_cast <IPolicy*> (&tmpPolicy), &ifaceHandle);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Set the interface obtained as the handle in Handle object.
    -----------------------------------------------------------------------*/
    SetHandle(ifaceHandle);
  } while (0);

  LOG_MSG_FUNCTION_EXIT_4 ("Network::ForceLookupInterfaceByIfaceId(): "
                           "Result 0x%x, ifaceId 0x%x, ifacehandle 0x%x, obj 0x%p",
                           result, ifaceId, ifaceHandle, this);
  mpICritSect->Leave();
  return result;

} /* Network::ForceLookupInterfaceByIfaceId() */

ds::ErrorType Network::GetStaleIfaceId
(
  ds::Net::IfaceIdType *  pIfaceId
)
{
  ds::ErrorType  result  = AEE_SUCCESS;
  int32          ifaceId = INVALID_IFACE_ID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == pIfaceId)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetStaleIfaceId(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Transform stale iface handle to iface ID
    -----------------------------------------------------------------------*/
    result = PSGetIfaceId (mStaleIfaceHandle, &ifaceId);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("Network::GetStaleIfaceId(): "
                       "PSGetIfaceId failed, obj 0x%p", this);
      break;
    }

    *pIfaceId = static_cast <ds::Net::IfaceIdType> (ifaceId);
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetStaleIfaceId(): "
                           "Result 0x%x, stale iface id 0x%x, obj 0x%p",
                           result, *pIfaceId, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetStaleIfaceId() */


ds::ErrorType Network::GetPolicy
(
  IPolicy **  ppIDSNetPolicy
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Network::GetPolicy(): Obj 0x%p, ppIDSNetPolicy 0x%x",
                             this, ppIDSNetPolicy);

  if (0 == ppIDSNetPolicy)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  *ppIDSNetPolicy = static_cast <IPolicy *> (mpPolicy);
  (void) mpPolicy->AddRef();

  mpICritSect->Leave();
  return AEE_SUCCESS;

} /* Network::GetPolicy() */

ds::ErrorType Network::SetPolicy
(
  IPolicy *  pIPolicy
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Network::SetPolicy(): Obj 0x%p, pIPolicy 0x%x",
                             this, pIPolicy);

  if (0 == pIPolicy)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  if (0 != mpPolicy)
  {
    (void) mpPolicy->Release();
  }

  mpPolicy = reinterpret_cast <Policy *> (pIPolicy);
  (void) mpPolicy->AddRef();

  mpICritSect->Leave();
  return AEE_SUCCESS;

} /* Network::SetPolicy() */

/*---------------------------------------------------------------------------
  Functions inherited from INetwork
---------------------------------------------------------------------------*/
ds::ErrorType Network::GetIfaceState
(
  NetworkStateType *  argNetState
)
{
  ds::ErrorType          result;
  int32                  ifaceHandle;
  IfaceStateEnumType     ifaceState;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == argNetState)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetIfaceState(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    ifaceHandle = GetHandle();
    if (0 == ifaceHandle)
    {
      LOG_MSG_INFO1_1 ("Network::GetIfaceState(): NULL handle, obj 0x%p",
                       this);
      *argNetState = NetworkState::QDS_CLOSED;
      result = AEE_SUCCESS;
      break;
    }

    /*-----------------------------------------------------------------------
      Call network platform IOCTL to get the interface state.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (ifaceHandle, IFACE_IOCTL_GET_STATE, &ifaceState);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Map Iface state to network state.
    -----------------------------------------------------------------------*/
    result = PS2DSIfaceState(ifaceState, argNetState);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetIfaceState(): "
                           "Result 0x%x, netstatus %d, obj 0x%p",
                           result, *argNetState, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetIfaceState() */

ds::ErrorType Network::GetLastNetDownReason
(
  NetDownReasonType *  argLastNetDownReason
)
{
  int32                              ifaceHandle;
  NetPlatform::NetworkDownReasonType lastNetDownReason;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == argLastNetDownReason)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetLastNetDownReason(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-----------------------------------------------------------------------
    In case the underlying interface is in DOWN state, but DOWN_EV havn't
    reached the ds_Net yet, take the net down reason from iface itself,
    otherwise 0x00 reason would be propagated to DSS / application
  -----------------------------------------------------------------------*/
  ifaceHandle = GetHandle();

  if(0 == ifaceHandle)
  {
    *argLastNetDownReason = mLastNetDownReason;
  }
  else
  {
    NetPlatform::GetLastNetworkDownReason (ifaceHandle, &lastNetDownReason);
    *argLastNetDownReason = (NetDownReasonType) lastNetDownReason;
  }

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetLastNetDownReason(): Reason %d, obj 0x%p",
                           mLastNetDownReason, this);
  mpICritSect->Leave();
  return AEE_SUCCESS;

} /* Network::GetLastNetDownReason() */


ds::ErrorType Network::GetIPAddr
(
  ds::IPAddrType *  argIPAddress
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return IPAddrIOCTL (IFACE_IOCTL_GET_IP_ADDR, argIPAddress);

} /* GetIPAddr() */

ds::ErrorType Network::GetNatPublicIPAddress
(
  ds::IPAddrType *  argIPAddress
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return IPAddrIOCTL (IFACE_IOCTL_GET_NAT_PUBLIC_IP_ADDR, argIPAddress);

} /* GetIPAddress() */

ds::ErrorType Network::DhcpArpCacheUpdate
(
  const ds::Net::DhcpArpCacheUpdateType *  dhcpArpCacheUpdateParam
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_DHCP_ARP_CACHE_UPDATE,
                       (void *) dhcpArpCacheUpdateParam);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::DhcpArpCacheUpdate(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network::DhcpArpCacheUpdate() */

ds::ErrorType Network::DhcpArpCacheClear
(
  const ds::Net::DhcpArpCacheClearType *  dhcpArpCacheClearParam
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_DHCP_ARP_CACHE_CLEAR,
                       (void *) dhcpArpCacheClearParam);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::DhcpArpCacheClear(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network::DhcpArpCacheClear() */

ds::ErrorType Network::GetDhcpDeviceInfo
(
  ds::Net::DhcpGetDeviceInfoType *  connDevInfo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == connDevInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetDhcpDeviceInfo(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  if (0 == connDevInfo->dev_info || 0 == connDevInfo->dev_infoLen ||
      0 != connDevInfo->dev_infoLenReq)
  {
    LOG_MSG_INVALID_INPUT_4 ("Network::GetDhcpDeviceInfo(): Invalid arg, "
                             "info 0x%p, len %d, lenReq %d, obj 0x%p",
                             connDevInfo->dev_info,
                             connDevInfo->dev_infoLen,
                             connDevInfo->dev_infoLenReq,
                             this);
    return QDS_EINVAL;
  }

  mpICritSect->Enter();

  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_DHCP_SERVER_GET_DEVICE_INFO,
                       static_cast <void *> (connDevInfo));

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetDhcpDeviceInfo(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network::GetDhcpDeviceInfo() */

//TODO Aren't NULL checks reqd for pSipDomainNames, pSipDomainNamesLenReq
//Also check if all info is printed. For example, how many domain names are
//requested by user and how many are returned by dsNet
ds::ErrorType Network::GetSIPServerDomainNames
(
  DomainName *  pSipDomainNames,
  int           sipServerDomainNamesLen,
  int *         pSipDomainNamesLenReq
)
{
  SipServerDomainNameInfoType   psSipDomainNames;
  ds::ErrorType                 result;
  int                           index;
  int                           seqNumItems = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Prepare the IOCTL arguments to get the SIP server domain names.
  -------------------------------------------------------------------------*/
  memset (&psSipDomainNames, 0, sizeof (psSipDomainNames));

  psSipDomainNames.count = PS_IFACE_MAX_SIP_SERVER_DOMAIN_NAMES;

  PS_SYSTEM_HEAP_MEM_ALLOC(
            psSipDomainNames.name_array,
            sizeof (DomainNameType) * PS_IFACE_MAX_SIP_SERVER_DOMAIN_NAMES,
            dss_iface_ioctl_domain_name_type*);

  if (0 == psSipDomainNames.name_array)
  {
    return AEE_ENOMEMORY;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Call the iface IOCTL to get SIP server domain names
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES,
                         static_cast <void *> (&psSipDomainNames));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = ds::Utils::Conversion::ProcessLenReq (sipServerDomainNamesLen,
                                                   pSipDomainNamesLenReq,
                                                   psSipDomainNames.count,
                                                   &seqNumItems);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1("Network::GetSIPServerDomainNames(): Failed to convert "
                      "SIP server domain names in PS format to QCM format, "
                      "obj 0x%p", this);
      break;
    }

    /*-----------------------------------------------------------------------
      Convert the individual SIP domain names into out argument
    -----------------------------------------------------------------------*/
    for (index = 0; index < seqNumItems; index++)
    {
      memscpy(pSipDomainNames[index],
        sizeof(DomainName),
        psSipDomainNames.name_array[index].domain_name,
        sizeof(DomainName));
    }
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE(psSipDomainNames.name_array);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetSIPServerDomainNames(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetSIPServerDomainNames() */

//TODO Aren't NULL checks reqd for pSipDomainNames, pSipDomainNamesLenReq
//Also check if all info is printed. For example, how many domain names are
//requested by user and how many are returned by dsNet
ds::ErrorType Network::GetSIPServerAddr
(
  ds::IPAddrType *  pSipServerAddr,
  int               sipServerAddrLen,
  int *             pSipServerAddrLenReq
)
{
  SipServerAddrInfoType sipServerAddr;
  ds::ErrorType         result;
  ds::ErrorType         copyResult  = AEE_SUCCESS;
  int32                 index;
  int                   seqNumItems = 0;
  int                   numCopied   = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset (&sipServerAddr, 0, sizeof (sipServerAddr));

  sipServerAddr.count = PS_IFACE_MAX_SIP_SERVER_ADDRESSES;

  PS_SYSTEM_HEAP_MEM_ALLOC(
    sipServerAddr.addr_array,
    (sizeof(ip_addr_type) * PS_IFACE_MAX_SIP_SERVER_ADDRESSES),
    ip_addr_type*);

  if (0 == sipServerAddr.addr_array)
  {
    return AEE_ENOMEMORY;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Call the iface IOCTL to get SIP server addresses
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_SIP_SERV_ADDR,
                         static_cast <void *> (&sipServerAddr));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert results to ds format
    -----------------------------------------------------------------------*/
    result = ds::Utils::Conversion::ProcessLenReq( sipServerAddrLen,
                                                   pSipServerAddrLenReq,
                                                   sipServerAddr.count,
                                                   &seqNumItems);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1("Network::GetSIPServerAddr(): Failed to convert "
                      "SIP addresses in PS format to QCM format, obj 0x%p",
                      this);
      break;
    }

    for (index = 0; index < seqNumItems; index++)
    {
      copyResult = PS2DSIPAddr (&(sipServerAddr.addr_array[index]),
                                &pSipServerAddr[numCopied]);
      if (AEE_SUCCESS != copyResult)
      {
        /*-------------------------------------------------------------------
          Do not copy invalid address
        -------------------------------------------------------------------*/
        LOG_MSG_INFO1_2( "Network::GetSIPServerAddr(): Not copying addr "
                         "at index %d, obj 0x%p", index, this);
        continue;
      }

      numCopied++;
    }

    *pSipServerAddrLenReq = numCopied;
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE(sipServerAddr.addr_array);

  LOG_MSG_FUNCTION_EXIT_3( "Network::GetSIPServerAddr(): "
                           "Result 0x%x, returning %d items, obj 0x%p",
                           result, *pSipServerAddrLenReq, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetSIPServerAddr() */

ds::ErrorType Network::GetQosAware
(
  boolean *  pGetQosAware
)
{
  ds::ErrorType  result;
  int32          ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    HACK to make event propagation work even after call goes down
  -------------------------------------------------------------------------*/
  ifaceHandle = GetHandle();
  if (0 == ifaceHandle && 0 != mStaleIfaceHandle)
  {
    ifaceHandle = mStaleIfaceHandle;
    LOG_MSG_INFO1_2("Network::GetQosAware(): "
                    "Using stale handle 0x%x for obj 0x%p",
                    mStaleIfaceHandle, this);
  }

  result = IfaceIoctlNonNullArg (ifaceHandle,
                                 IFACE_IOCTL_ON_QOS_AWARE_SYSTEM,
                                 static_cast <void *> (pGetQosAware));

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetQosAware(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetQosAware() */


ds::ErrorType Network::GetOutageInfo
(
  OutageInfoType *  pOutageInfo
)
{
  ds::ErrorType                result;
  OutageNotificationInfoType   outageInfo;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == pOutageInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetOutageInfo(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      In case that outage event didn't occurred, return invalid state
    -----------------------------------------------------------------------*/
    if (FALSE == mOutageEventOccurred)
    {
      LOG_MSG_INFO1_1 ("Network::GetOutageInfo(): Outage event hasn't occured, "
                       "obj 0x%p", this);

      pOutageInfo->state        = OutageState::QDS_INVALID;
      pOutageInfo->timeToOutage = 0;
      pOutageInfo->duration     = 0;

      result = AEE_SUCCESS;
      break;
    }

    /*-----------------------------------------------------------------------
      Call the iface IOCTL to get outage info.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_OUTAGE_NOTIFICATION_INFO,
                        static_cast <void *> (&outageInfo));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert to out-params.
    -----------------------------------------------------------------------*/
    pOutageInfo->timeToOutage = outageInfo.time_to_outage;
    pOutageInfo->duration     = outageInfo.duration;

    if (0 != pOutageInfo->timeToOutage)
    {
      pOutageInfo->state = OutageState::QDS_VALID;
    }
    else if (0 != pOutageInfo->duration)
    {
      pOutageInfo->state = OutageState::QDS_STARTED;
    }
    else
    {
      pOutageInfo->state = OutageState::QDS_EXPIRED;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_5 ("Network::GetOutageInfo(): Result 0x%x, "
                           "outage state %d, timeToOutage %d, duration %d, "
                           "obj 0x%p",
                           result,
                           pOutageInfo->state,
                           pOutageInfo->timeToOutage,
                           pOutageInfo->duration,
                           this);
  mpICritSect->Leave();
  return result;

} /* Network::GetOutageInfo() */

ds::ErrorType Network::GetFastDormStatusInfo
(
  uint8 *  piFastDormStatusInfo
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == piFastDormStatusInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetFastDormStatusInfo(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();
  *piFastDormStatusInfo = mFastDormStatus;

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetFastDormStatusInfo(): "
                           "Dormancy status %d, obj 0x%p",
                           mFastDormStatus, this);
  mpICritSect->Leave();
  return AEE_SUCCESS;

} /* Network::GetFastDormStatusInfo() */

ds::ErrorType Network::GetBearerInfo
(
  IBearerInfo **  ppIDSNetBearerInfo
)
{
  BearerTechType           bearerTechInfo;
  BearerTechRateType       pDataBearerRate;
  ds::ErrorType            result;
  int32                    ifaceHandle;
  ds::Net::IfaceNameType   ifaceName;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == ppIDSNetBearerInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetBearerInfo(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  *ppIDSNetBearerInfo = NULL;

  memset (&bearerTechInfo, 0, sizeof (BearerTechType));

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    HACK to make event propagation work even after call goes down
  -------------------------------------------------------------------------*/
  ifaceHandle = GetHandle();
  if (0 == ifaceHandle && 0 != mStaleIfaceHandle)
  {
    ifaceHandle = mStaleIfaceHandle;
    LOG_MSG_INFO1_2("Network::GetBearerInfo(): Using stale handle 0x%x, obj 0x%p",
                    mStaleIfaceHandle, this);
  }

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get bearer technology
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (ifaceHandle,
                       IFACE_IOCTL_GET_BEARER_TECHNOLOGY,
                       static_cast <void *> (&bearerTechInfo));
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Get bearer technology rate
  -------------------------------------------------------------------------*/
  result = IfaceIoctlNonNullArg (ifaceHandle,
                                 IFACE_IOCTL_GET_DATA_BEARER_RATE,
                                 static_cast <void *> (&pDataBearerRate));
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Convert the network type to iface name
  -------------------------------------------------------------------------*/
  switch (bearerTechInfo.current_network)
  {
    case PS_IFACE_NETWORK_CDMA:
    {
      ifaceName = IfaceName::IFACE_CDMA_SN;
      break;
    }

    case PS_IFACE_NETWORK_UMTS:
    {
      ifaceName = IfaceName::IFACE_UMTS;
      break;
    }

    case PS_IFACE_NETWORK_WLAN:
    {
      ifaceName = IfaceName::IFACE_WLAN;
      break;
    }

    default:
    {
      LOG_MSG_ERROR_2 ("Network::GetBearerInfo(): Unknown network %d, obj 0x%p",
                       bearerTechInfo.current_network, this);
      result = QDS_INTERNAL;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Construct the Bearer tech object using the IOCTL result.
  -------------------------------------------------------------------------*/
  *ppIDSNetBearerInfo =
    static_cast <IBearerInfo *>
      (new BearerTech (ifaceName,
                       bearerTechInfo.data.cdma_type.rat_mask,
                       bearerTechInfo.data.cdma_type.so_mask,
                       bearerTechInfo.data.umts_type.rat_mask,
                       &pDataBearerRate));
  if (0 == *ppIDSNetBearerInfo)
  {
    LOG_MSG_ERROR_1 ("Network::GetBearerInfo(): Failed to alloc BearerTech, "
                     "obj 0x%p", this);
    result = AEE_ENOMEMORY;
    goto bail;
  }

  result = AEE_SUCCESS;

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetBearerInfo(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetBearerInfo() */

ds::ErrorType Network::GetBearerTechEx
(
  IBearerTechEx **  ppIDSNetBearerTechEx
)
{
  BearerTechExType         bearerTechExInst;
  ds::ErrorType            result;
  int32                    ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppIDSNetBearerTechEx)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetBearerTechEx(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  *ppIDSNetBearerTechEx = NULL;

  memset (&bearerTechExInst, 0, sizeof (BearerTechExType));

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    HACK to make event propagation work even after call goes down
  -------------------------------------------------------------------------*/
  ifaceHandle = GetHandle();
  if (0 == ifaceHandle && 0 != mStaleIfaceHandle)
  {
    ifaceHandle = mStaleIfaceHandle;
    LOG_MSG_INFO1_2("Network::GetBearerTechEx(): Using stale handle 0x%x, obj 0x%p",
                    mStaleIfaceHandle, this);
  }

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get bearer technology
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (ifaceHandle,
                       IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX,
                       static_cast <void *> (&bearerTechExInst));
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Construct the Bearer tech object using the IOCTL result.
  -------------------------------------------------------------------------*/
  *ppIDSNetBearerTechEx =
    static_cast <IBearerTechEx *>
      (new BearerTechEx ((uint32)bearerTechExInst.technology, 
                         (uint32)bearerTechExInst.rat_value, 
                         (uint32)bearerTechExInst.so_mask));
  if (0 == *ppIDSNetBearerTechEx)
  {
    LOG_MSG_ERROR_1 ("Network::GetBearerTechEx(): Failed to alloc BearerTechEx, "
                     "obj 0x%p", this);
    result = AEE_ENOMEMORY;
    goto bail;
  }

  result = AEE_SUCCESS;

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetBearerTechEx(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetBearerTechEx() */

ds::ErrorType Network::GetDataPathBridgeMode
(
  DataPathBridgeModeType* pGetDataPathBridgeMode
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == pGetDataPathBridgeMode)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetDataPathBridgeMode(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  memset (pGetDataPathBridgeMode, 0, sizeof (DataPathBridgeModeType));

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get data path
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_GET_DATA_PATH,
                       static_cast <void *> (pGetDataPathBridgeMode));

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::GetDataPathBridgeMode(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetDataPathBridgeMode(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetDataPathBridgeMode() */

ds::ErrorType Network::SetDataPathBridgeMode
(
  DataPathBridgeModeType* pSetDataPathBridgeMode
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == pSetDataPathBridgeMode)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::SetDataPathBridgeMode(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get data path
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE,
                       static_cast <void *> (pSetDataPathBridgeMode));

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::SetDataPathBridgeMode(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::SetDataPathBridgeMode(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::SetDataPathBridgeMode() */

ds::ErrorType Network::GetOpResPco
(
  NetworkOpReservedPcoType* pGetOpResPco
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == pGetOpResPco)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetOpResPco(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  memset (pGetOpResPco, 0, sizeof (NetworkOpReservedPcoType));

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get data path
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_GET_OP_RES_PCO,
                       static_cast <void *> (pGetOpResPco));

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::GetOpResPco(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetOpResPco(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetOpResPco() */

ds::ErrorType Network::_3GPPLTEReestabCalltypeGet
(
  _3GPPLTEReestabCalltypeStruct* p3GPPLTEReestabCalltypeGet
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == p3GPPLTEReestabCalltypeGet)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::_3GPPLTEReestabCalltypeGet(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  memset (p3GPPLTEReestabCalltypeGet, 0, sizeof (_3GPPLTEReestabCalltypeStruct));

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get 3GPP LTE Reestab Calltype
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE,
                       static_cast <void *> (p3GPPLTEReestabCalltypeGet));

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::_3GPPLTEReestabCalltypeGet(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::_3GPPLTEReestabCalltypeGet(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::_3GPPLTEReestabCalltypeGet() */

ds::ErrorType Network::_3GPPLTEReestabCalltypeSet
(
  _3GPPLTEReestabCalltypeStruct* p3GPPLTEReestabCalltypeSet
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == p3GPPLTEReestabCalltypeSet)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::_3GPPLTEReestabCalltypeSet(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to set 3GPP LTE Reestab Calltype
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE,
                       static_cast <void *> (p3GPPLTEReestabCalltypeSet));

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::_3GPPLTEReestabCalltypeSet(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::_3GPPLTEReestabCalltypeSet(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::_3GPPLTEReestabCalltypeSet() */

ds::ErrorType Network::SetSilentRedial
(
  SilentRedialStruct *pSilentRedial
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == pSilentRedial)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::pSilentRedial(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to Set Silent Redial
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_SET_SILENT_REDIAL,
                       static_cast <void *>(pSilentRedial));

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::SetSilentRedial(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */
bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::SetSilentRedial(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::SetSilentRedial() */

ds::ErrorType Network::_3GPPFORCEPDNdisconnect()
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to disconnect PDN
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT,
                       NULL);

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::_3GPPFORCEPDNdisconnect(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::_3GPPFORCEPDNdisconnect(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::_3GPPFORCEPDNdisconnect() */

ds::ErrorType Network::Setkeepalive(KeepAliveType pKeepalive)
{
  ds::ErrorType result = AEE_SUCCESS;

   mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to disconnect PDN
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_KEEP_ALIVE,
                       &pKeepalive);

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::Setkeepalive(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::Setkeepalive(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
}/* Network::Setkeepalive() */


ds::ErrorType Network::GetAddressFamily
(
  ds::AddrFamilyType *  argAddressFamily
)
{
  ds::ErrorType        result;
  PSIPAddrFamilyType   addressFamily;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == argAddressFamily)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetAddressFamily(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Call the iface IOCTL to get address family
    -----------------------------------------------------------------------*/
    memset (&addressFamily, 0, sizeof (PSIPAddrFamilyType));
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_IP_FAMILY,
                         static_cast <void *> (&addressFamily));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = PS2DSAddrFamily(addressFamily, argAddressFamily);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetAddressFamily(): Result 0x%x, "
                           "family %d, obj 0x%p", result, addressFamily, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetAddressFamily() */

//TODO Aren't NULL checks reqd for pSipDomainNames, pSipDomainNamesLenReq
//Also check if all info is printed. For example, how many domain names are
//requested by user and how many are returned by dsNet
ds::ErrorType Network::GetDomainNameSearchList
(
  DomainName *  pDomainNameSearchList,
  int           domainNameSearchListLen,
  int *         pDomainNameSearchListLenReq
)
{
  DomainNameSearchListType  domainNameSearchList;
  ds::ErrorType             result;
  int                       index;
  int                       seqNumItems = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset (&domainNameSearchList, 0, sizeof (domainNameSearchList));
  PS_SYSTEM_HEAP_MEM_ALLOC(
    domainNameSearchList.name_array,
    sizeof (DomainNameType) * PS_IFACE_MAX_SEARCH_LIST_DOMAIN_NAMES,
    dss_iface_ioctl_domain_name_type*);

  if (0 == domainNameSearchList.name_array)
  {
    return AEE_ENOMEMORY;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Call the iface IOCTL to get domain name search list
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_DOMAIN_NAME_SEARCH_LIST,
                         static_cast <void *> (&domainNameSearchList));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert domain name search list into ds format
    -----------------------------------------------------------------------*/
    result = ds::Utils::Conversion::ProcessLenReq (domainNameSearchListLen,
                                                   pDomainNameSearchListLenReq,
                                                   domainNameSearchList.count,
                                                   &seqNumItems);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1("Network::GetDomainNameSearchList(): Failed to convert "
                      "domain name search list in PS format to QCM format, "
                      "obj 0x%p", this);
      break;
    }

    /*-----------------------------------------------------------------------
      Convert the individual domain name search list items into out argument
    -----------------------------------------------------------------------*/
    for (index = 0; index < seqNumItems; index++)
    {
      memscpy(pDomainNameSearchList[index],
        sizeof(DomainName),
        domainNameSearchList.name_array[index].domain_name,
        sizeof(DomainName));
    }
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE(domainNameSearchList.name_array);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetDomainNameSearchList(): "
                           "Result 0x%x, returning %d items, obj 0x%p",
                           result, seqNumItems, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetDomainNameSearchList() */

ds::ErrorType Network::GetNetMTU
(
  int *  pMTU
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_GET_MTU,
                                 static_cast <void *> (pMTU));

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetMTU(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetNetMTU() */

ds::ErrorType Network::GetHWAddress
(
  HWAddressType *  pHWAddr
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_GET_HW_ADDR,
                                 static_cast <void *> (pHWAddr));

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetHWAddress(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetHWAddress() */

ds::ErrorType Network::IsLaptopCallActive
(
  boolean *  pIsActive
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_IS_LAPTOP_CALL_ACTIVE,
                                 static_cast <void *> (pIsActive));

  LOG_MSG_FUNCTION_EXIT_2 ("Network::IsLaptopCallActive(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();
  return result;

} /* Network::IsLaptopCallActive() */


ds::ErrorType Network::GoDormant
(
  DormantReasonType  dormantReason
)
{
  IPhysLink *     txPhysLink = 0;
  ds::ErrorType   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Obtain Tx phys link and issue dormancy
    -----------------------------------------------------------------------*/
    result = GetTXPhysLink(&txPhysLink);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = txPhysLink->GoDormant(dormantReason);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GoDormant(): Result 0x%x, obj 0x%p dormancy reason %d",
                           result, this, dormantReason);

  DS_UTILS_RELEASEIF(txPhysLink);

  mpICritSect->Leave();
  return result;

} /* Network::GoDormant() */

ds::ErrorType Network::EnableDNSDuringIPCP
(
  boolean  enable
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_IPCP_DNS_OPT,
                                 static_cast <void *> (&enable));

  LOG_MSG_FUNCTION_EXIT_3 ("Network::EnableDNSDuringIPCP(): "
                           "Result 0x%x, obj 0x%p, enable %d", result, this, enable);
  mpICritSect->Leave();
  return result;

} /* Network::EnableDNSDuringIPCP() */

ds::ErrorType Network::RefreshDHCPConfigInfo
(
  void
)
{
  ds::ErrorType   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get hardware address.
  -------------------------------------------------------------------------*/
  result =
    IfaceIoctl (GetHandle(), IFACE_IOCTL_REFRESH_DHCP_CONFIG_INFO, NULL);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::RefreshDHCPConfigInfo(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();
  return result;

} /* Network::RefreshDHCPConfigInfo() */

ds::ErrorType Network::GetCurrRFCondition
(
  RFConditionType *  pDSRFCondition
)
{
  ds::ErrorType        result;
  RFConditionsType     rfCondition;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == pDSRFCondition)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetCurrRFCondition(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  memset (&rfCondition, 0, sizeof (RFConditionsType));

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Call the iface IOCTL to get RF conditions.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_RF_CONDITIONS,
                         static_cast <void *> (&rfCondition));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert to out args
    -----------------------------------------------------------------------*/
    *pDSRFCondition = (RFConditionType) rfCondition.rf_conditions;
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetCurrRFCondition(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetCurrRFCondition() */


ds::ErrorType Network::GetIfaceName
(
  ds::Net::IfaceNameType *  pIfaceName
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == pIfaceName)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetIfaceName(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  memset (pIfaceName, 0, sizeof (ds::Net::IfaceNameType));

  mpICritSect->Enter();

  result = IfaceIoctl(GetHandle(),
                      IFACE_IOCTL_GET_IFACE_NAME,
                      static_cast <void *> (pIfaceName));

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetIfaceName(): "
                           "Result 0x%x, iface name %d, obj 0x%p",
                           result, *pIfaceName, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetIfaceName() */

//TODO Enhance logging
ds::ErrorType Network::GetDNSAddr
(
  ::ds::IPAddrType *  pDNSAddrs,
  int                 dnsAddrsLen,
  int *               pDNSAddrsLenReq
)
{
  AllDNSAddrsType   allDNSAddrs;
  ds::ErrorType     result;
  ds::ErrorType     copyResult = AEE_SUCCESS;
  int               index;
  int               seqNumItems = 0;
  int               numCopied = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Prepare the IOCTL arguments to get the SIP server domain names.
  -----------------------------------------------------------------------*/
  memset (&allDNSAddrs, 0, sizeof (allDNSAddrs));

  PS_SYSTEM_HEAP_MEM_ALLOC(
    allDNSAddrs.dns_addrs_ptr,
    sizeof (ip_addr_type) * PS_IFACE_NUM_DNS_ADDRS,
    ip_addr_type*);

  if (0 == allDNSAddrs.dns_addrs_ptr)
  {
    return AEE_ENOMEMORY;
  }

  allDNSAddrs.num_dns_addrs = PS_IFACE_NUM_DNS_ADDRS;

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Call the iface IOCTL to get all DNS addresses
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_ALL_DNS_ADDRS,
                         static_cast <void *> (&allDNSAddrs));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = ds::Utils::Conversion::ProcessLenReq (dnsAddrsLen,
                                                   pDNSAddrsLenReq,
                                                   allDNSAddrs.num_dns_addrs,
                                                   &seqNumItems);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert to out params.
    -----------------------------------------------------------------------*/
    for (index = 0; index < seqNumItems; index++)
    {
      copyResult = PS2DSIPAddr (&allDNSAddrs.dns_addrs_ptr[index],
                                &pDNSAddrs[numCopied]);
      if (AEE_SUCCESS != copyResult)
      {
        /*-------------------------------------------------------------------
          Do not copy invalid address
        -------------------------------------------------------------------*/
        LOG_MSG_INFO1_2( "Network::GetDNSAddr(): Not copying addr "
                         "at index %d, obj 0x%p", index, this);
        continue;
      }

      numCopied++;
    }
  } while (0);

  /*-------------------------------------------------------------------
    LenReq parameter should reflect the number of valid addresses
    which were copied to output parameter
  -------------------------------------------------------------------*/
  if (NULL != pDNSAddrsLenReq)
  {
    *pDNSAddrsLenReq = numCopied;
  }

  PS_SYSTEM_HEAP_MEM_FREE(allDNSAddrs.dns_addrs_ptr);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetDNSAddr(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetDNSAddr() */

ds::ErrorType CDECL Network::GetNetworkStatistics
(
  NetworkStatsType *  stats
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = NetPlatform::GetIfaceStats(GetHandle(), stats);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetIfaceStats(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetIfaceStats() */

ds::ErrorType Network::ResetNetworkStatistics
(
  void
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = NetPlatform::ResetIfaceStats(GetHandle());

  LOG_MSG_FUNCTION_EXIT_2 ("Network::ResetIfaceStats(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Network::ResetIfaceStats() */

ds::ErrorType Network::GetDHCPRefreshResult
(
  boolean* res
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //TODO: Refresh results are obtained through events.
  //Store this information in the platform layer.
  (void) res;
  return 0;
} /* GetDHCPRefreshResult() */


ds::ErrorType Network::GetTXPhysLink
(
  ::IPhysLink **  txPhysLinkObj
)
{
  IQoSManager *   pQoSManager = NULL;
  IQoS *          pQoSDefault = NULL;
  ds::ErrorType   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == txPhysLinkObj)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetTXPhysLink(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    result = CreateQoSManager (&pQoSManager);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = pQoSManager->GetQosDefault (&pQoSDefault);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = pQoSDefault->GetTXPhysLink(txPhysLinkObj);
  } while (0);

  DS_UTILS_RELEASEIF(pQoSManager);
  DS_UTILS_RELEASEIF(pQoSDefault);

  mpICritSect->Leave();

  return result;
} /* Network::GetTXPhysLink() */

ds::ErrorType Network::GetRXPhysLink
(
  ::IPhysLink **  rxPhysLinkObj
)
{
  IQoSManager *  pQoSManager = NULL;
  IQoS *         pQoSDefault = NULL;
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == rxPhysLinkObj)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetRXPhysLink(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    result = CreateQoSManager (&pQoSManager);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = pQoSManager->GetQosDefault (&pQoSDefault);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = pQoSDefault->GetRXPhysLink(rxPhysLinkObj);
  } while (0);

  DS_UTILS_RELEASEIF(pQoSManager);
  DS_UTILS_RELEASEIF(pQoSDefault);

  mpICritSect->Leave();

  return result;
} /* Network::GetRXPhysLink() */


ds::ErrorType Network::SetPDNTeardownConfig
(
  PDNTeardownConfigType* pPDNTeardownConfig
)
{
  ds::ErrorType result = AEE_SUCCESS;
  ps_iface_ioctl_pdn_teardown_config_type   pdnTeardownConfig;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    mpICritSect->Enter();

    if (0 == pPDNTeardownConfig)
    {
      result = QDS_EFAULT;
      break;
    }

    pdnTeardownConfig.local_abort = pPDNTeardownConfig->local_abort;

    /*-------------------------------------------------------------------------
      Call the iface IOCTL to get data path
    -------------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_SET_PDN_TEARDOWN_CONFIG,
                         static_cast <void *> (&pdnTeardownConfig));
  
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while(0);

  /* Fall through */
  LOG_MSG_INFO2_2 ("Network::SetPDNTeardownConfig(): Result 0x%x, obj 0x%p",
                    result, this);
  mpICritSect->Leave();

  return result;
} /* Network::SetPDNTeardownConfig() */

ds::ErrorType Network::GetPDNTeardownConfig
(
  PDNTeardownConfigType* pPDNTeardownConfig
)
{
  ds::ErrorType result = AEE_SUCCESS;
  ps_iface_ioctl_pdn_teardown_config_type   pdnTeardownConfig;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    mpICritSect->Enter();

    memset (&pdnTeardownConfig, 0, 
            sizeof (ps_iface_ioctl_pdn_teardown_config_type));
            
    if (0 == pPDNTeardownConfig)
    {
      LOG_MSG_INVALID_INPUT_1 ("Network::GetPDNTeardownConfig(): NULL arg, obj 0x%p",
                               this);
      break;
    }

    memset (pPDNTeardownConfig, 0, sizeof (PDNTeardownConfigType));

    /*-------------------------------------------------------------------------
      Call the iface IOCTL to get data path
    -------------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_PDN_TEARDOWN_CONFIG,
                         static_cast <void *> (&pdnTeardownConfig));

    if (AEE_SUCCESS != result)
    {
      break;
    }
  
    pPDNTeardownConfig->local_abort =  pdnTeardownConfig.local_abort;
  } while(0);

  /* Fall through */
  LOG_MSG_INFO2_3 ("Network::GetPDNTeardownConfig(): Result 0x%x, "
                   "obj 0x%p pdnTeardownConfig %d",
                   result, this, pdnTeardownConfig.local_abort);

  mpICritSect->Leave();

  return result;
} /* Network::GetPDNTeardownConfig() */


/*---------------------------------------------------------------------------
  Inherited function from ds::Utils::INode interface.
---------------------------------------------------------------------------*/
boolean Network::Process
(
  void *  userDataPtr
)
{
  EventInfoType *  eventInfoPtr;
  int32            ifaceHandle;
  PSIPAddrType     pIPv6ExternalAddrDeletedEvInfo;
  ApnParamChangeList pApnParamChangeList ={0};
  HandoffEventInfoType *ho_ev = NULL;
  HandoffFailureEventInfoType * fail_ev = NULL;   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == userDataPtr)
  {
    LOG_MSG_ERROR_1 ("Network::Process(): NULL event info, obj 0x%p", this);
    return FALSE;
  }

  eventInfoPtr = static_cast <EventInfoType *> (userDataPtr);

  mpICritSect->Enter();

  ifaceHandle = GetHandle();

  LOG_MSG_INFO1_4 ("Network::Process(): "
                   "Handle 0x%x, event 0x%x, event handle 0x%x, obj 0x%p",
                   ifaceHandle,
                   eventInfoPtr->eventName,
                   eventInfoPtr->handle,
                   this);

  /*-------------------------------------------------------------------------
    Process the event only if
      1. It is a network Event
      2. If this object registered for the event
  -------------------------------------------------------------------------*/
  switch (eventInfoPtr->eventGroup)
  {
    case EVENT_GROUP_NETWORK:
    {
      if (EventIsNetworkState(eventInfoPtr->eventName))
      {
        ProcessIfaceStateEvent (eventInfoPtr);
      }
      else if (ifaceHandle == eventInfoPtr->handle)
      {
        if (NetworkExtEvent::QDS_EV_EXTENDED_IP_CONFIG_EX == eventInfoPtr->eventName)
        {
          mCachedExtendedMask = *(::ds::Net::NetworkExtendedIpConfigExType*)eventInfoPtr->psEventInfo;
        }
        else if((NetworkExtEvent::QDS_EV_HANDOFF_INIT == eventInfoPtr->eventName) 
                 || (NetworkExtEvent::QDS_EV_HANDOFF_SUCCESS == eventInfoPtr->eventName)
                 || (NetworkExtEvent::QDS_EV_HANDOFF_FAILURE == eventInfoPtr->eventName))
        {
          if(NetworkExtEvent::QDS_EV_HANDOFF_FAILURE == eventInfoPtr->eventName)
          { 
            fail_ev = (HandoffFailureEventInfoType*)eventInfoPtr->psEventInfo;
            mcachedHandoffFailureinfo.srat = (uint16)fail_ev->srat;
            mcachedHandoffFailureinfo.trat = (uint16)fail_ev->trat;
            mcachedHandoffFailureinfo.failure_reason = (uint32)fail_ev->ho_failure_reason;
          }
          else
          {  /*Init or Sucess event */
            ho_ev = (HandoffEventInfoType*)eventInfoPtr->psEventInfo;
            mcachedHandoffinfo.srat = (uint16)ho_ev->srat;
            mcachedHandoffinfo.trat = (uint16)ho_ev->trat;
          }
	}
        else if (NetworkEvent::QDS_EV_OUTAGE == eventInfoPtr->eventName)
        {
          mOutageEventOccurred = TRUE;
        }
        else if (NetworkEvent::QDS_EV_FAST_DORM_STATUS ==
                   eventInfoPtr->eventName)
        {
          mFastDormStatus = (uint8) ((uint32)(eventInfoPtr->psEventInfo));
        }
        else if (NetworkEvent::QDS_EV_IPV6_EXT_ADDR_DEL_EV == 
                 eventInfoPtr->eventName)
        {
          memscpy( (void *)&pIPv6ExternalAddrDeletedEvInfo,
            sizeof(PSIPAddrType),
            eventInfoPtr->psEventInfo,
            sizeof(PSIPAddrType));
          /*-------------------------------------------------------------------------
            Cache the deleted address in ds_Net, later the client will fetch it
          -------------------------------------------------------------------------*/          
          memscpy(mDeletedIPv6Addr,
            sizeof (ds::INAddr6Type),
            &pIPv6ExternalAddrDeletedEvInfo.addr.v6,
            sizeof (ds::INAddr6Type));
        }
        else if (NetworkExtEvent::QDS_EV_APN_PARAM_EV == 
                 eventInfoPtr->eventName)
        {
          memscpy ((void *)&pApnParamChangeList,
             sizeof(ApnParamChangeList),
             eventInfoPtr->psEventInfo,
             sizeof(ApnParamChangeList));
        }
        Notify (eventInfoPtr->eventName);
      }
      else if (ifaceHandle == 0 &&
               mStaleIfaceHandle == eventInfoPtr->handle &&
               (NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED ==
                  eventInfoPtr->eventName ||
                  NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED_EX ==
                  eventInfoPtr->eventName ||
                NetworkExtEvent::QDS_EV_QOS_AWARENESS ==
                  eventInfoPtr->eventName))
      {
        LOG_MSG_INFO1_2("Network::Process(): "
                        "Posting event 0x%x even though handle is 0 because of "
                        "stale handle 0x%x match",
                        eventInfoPtr->eventName, mStaleIfaceHandle);
        Notify (eventInfoPtr->eventName);
      }

      break;
    }

    default:
    {
      break;
    }
  } /* switch */

  mpICritSect->Leave();

  return TRUE;

} /* Network::Process() */

ds::ErrorType Network::OnStateChange
(
  ISignal *    signalObj,
  EventType    eventID,
  IQI **       regObj
 )
{
  ds::ErrorType  result = AEE_SUCCESS;
  EventType networkEventType = -1;
  NetworkStateType netState = NetworkState::QDS_CLOSED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("Network::OnStateChange(): "
                            "Obj 0x%p, event 0x%x, signal 0x%p",
                            this, eventID, signalObj);

  if (0 == signalObj)
  {
    return QDS_EFAULT;
  }

  switch (eventID)
  {
    // network state
    case NetworkEvent::QDS_EV_IFACE_UP:
    case NetworkEvent::QDS_EV_IFACE_GOING_DOWN:

    // dss cannot register to the following events,
    // no signals buses exist to support it:
    //NetworkEvent::QDS_EV_IFACE_ENABLED
    //NetworkEvent::QDS_EV_IFACE_DISABLED

    case NetworkEvent::QDS_EV_IFACE_DOWN:
    case NetworkEvent::QDS_EV_IFACE_ACTIVE_OUT_OF_USE:
    case NetworkEvent::QDS_EV_IFACE_COMING_UP:
    case NetworkEvent::QDS_EV_IFACE_CONFIGURING:
    case NetworkEvent::QDS_EV_IFACE_ROUTEABLE:

    case NetworkEvent::QDS_EV_IP_ADDR_CHANGED:
    case NetworkEvent::QDS_EV_OUTAGE:
    case NetworkEvent::QDS_EV_RF_CONDITIONS_CHANGED:
    case NetworkExtEvent::QDS_EV_QOS_AWARENESS:
    case NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED:
    case NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED_EX:
    case NetworkEvent::QDS_EV_IPV6_EXT_ADDR_DEL_EV:
    case NetworkControlEvent::QDS_EV_EXTENDED_IP_CONFIG:
    case NetworkExtEvent::QDS_EV_HANDOFF_INIT:
    case NetworkExtEvent::QDS_EV_HANDOFF_SUCCESS:
    case NetworkExtEvent::QDS_EV_HANDOFF_FAILURE:
    case NetworkExtEvent::QDS_EV_EXTENDED_IP_CONFIG_EX:
    case NetworkExtEvent::QDS_EV_APN_PARAM_EV:
    {
      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("Network::OnStateChange(): "
                               "Unknown event 0x%x, obj 0x%p", eventID, this);
      return AEE_EUNSUPPORTED;
    }
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Post event immediately for BEARER_TECH_CHANGED and STATE_CHANGED events
    so that application can query for the current state
  -------------------------------------------------------------------------*/
  if (NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED == eventID || 
      NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED_EX == eventID)
  {
     (void) signalObj->AddRef();
     (void) signalObj->Set();
     (void) signalObj->Release();
  }

  if (EventIsNetworkState(eventID))
  {
    (void) GetIfaceState (&netState);
    networkEventType = NetworkState2Event(netState);
    if (networkEventType == eventID)
    {
      (void) signalObj->AddRef();
      (void) signalObj->Set();
      (void) signalObj->Release();
    }
  }

  result = Handle::OnStateChange(signalObj, eventID, regObj);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::OnStateChange(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* Network::OnStateChange() */

/*---------------------------------------------------------------------------
  Methods from INetworkPriv interface.
---------------------------------------------------------------------------*/
ds::ErrorType Network::GetIfaceId
(
  IfaceIdType *  pIfaceId
)
{
  ds::ErrorType  result;
  int32          handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == pIfaceId)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetIfaceId(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    handle = GetHandle();
    if (0 == handle)
    {
      /*---------------------------------------------------------------------
        Perform network lookup as  network lookup or bringup has not happened
        yet
      ---------------------------------------------------------------------*/
      result = LookupInterface();
      if (AEE_SUCCESS != result)
      {
        break;
      }

      handle = GetHandle();
    }

    result = PSGetIfaceId (handle, (int32 *) pIfaceId);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_4 ("Network::GetIfaceId(): "
                           "Result 0x%x, handle 0x%x, iface id 0x%x, obj 0x%p",
                           result, handle, *pIfaceId, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetIfaceId() */

ds::ErrorType Network::GetIfaceIdNoLookup
(
 IfaceIdType* pIfaceId
)
{
  ds::ErrorType result;
  int32         handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == pIfaceId)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetIfaceIdNoLookup(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do 
  {
    handle = GetHandle();

    result = PSGetIfaceId (handle, (int32 *)pIfaceId);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);
  
  LOG_MSG_FUNCTION_EXIT_4 ("Network::GetIfaceIdNoLookup(): "
                           "Result 0x%x, handle 0x%x, iface id 0x%x, obj 0x%p",
                           result, handle, *pIfaceId, this);

  mpICritSect->Leave();

  return result;
} /* Network::GetIfaceIdNoLookup() */

ds::ErrorType Network::GetPreviousState
(
  NetworkStateType *  argNetState
)
{
  ds::ErrorType       result;
  int32               ifaceHandle;
  IfaceStateEnumType  ifaceState;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == argNetState)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetPreviousState(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    ifaceHandle = GetHandle();
    if (0 == ifaceHandle)
    {
      LOG_MSG_INFO1_1 ("Network::GetPreviousState(): NULL handle, obj 0x%p",
                       this);

      *argNetState = mCachedPreviousState;
      result       = AEE_SUCCESS;
      break;
    }

    /*-----------------------------------------------------------------------
      Call network platform IOCTL to get the interface state
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (ifaceHandle,
                         IFACE_IOCTL_GET_PREVIOUS_STATE,
                         &ifaceState);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Map Iface state to network state
    -----------------------------------------------------------------------*/
    result = PS2DSIfaceState(ifaceState, argNetState);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetPreviousState(): "
                           "Result 0x%x, Previous netstatus %d, obj 0x%p",
                           result, *argNetState, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetPreviousState() */

ds::ErrorType Network::GetState
(
  NetworkStateType *  argNetState
)
{
  ds::ErrorType  result;
  int32          ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == argNetState)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetState(): NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      If the handle is not set in the network object, return QDS_CLOSED.
    -----------------------------------------------------------------------*/
    ifaceHandle = GetHandle();
    if (0 == ifaceHandle)
    {
      LOG_MSG_INFO1_1 ("Network::GetState(): NULL handle, obj 0x%p", this);

      *argNetState = NetworkState::QDS_CLOSED;
      result       = AEE_SUCCESS;
      break;
    }

    /*-----------------------------------------------------------------------
      If the bring up flag is set, return QDS_OPEN_IN_PROGRESS
    -----------------------------------------------------------------------*/
    if (TRUE == mBringupAgain)
    {
      LOG_MSG_INFO1_1 ("Network::GetState(): "
                       "mBringUpAgain flag is set, obj 0x%p", this);

      *argNetState = NetworkState::QDS_OPEN_IN_PROGRESS;
      result       = AEE_SUCCESS;
      break;
    }

    /*-----------------------------------------------------------------------
      Otherwise, return the associated iface state
    -----------------------------------------------------------------------*/
    result = GetIfaceState (argNetState);
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetState(): Result 0x%x, "
                           "netstatus %d, obj 0x%p",
                           result, *argNetState, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetState() */


ds::ErrorType Network::GetPreviousIPAddr
(
  ds::IPAddrType *  argIPAddress
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return IPAddrIOCTL (IFACE_IOCTL_GET_PREVIOUS_IP_ADDR, argIPAddress);

} /* Network::GetPreviousIPAddr() */


ds::ErrorType Network::GetPreviousBearerInfo
(
  IBearerInfo **  ppIDSNetBearerInfo
)
{
  BearerTechType           bearerTechInfo;
  ds::ErrorType            result;
  int32                    ifaceHandle;
  ds::Net::IfaceNameType   ifaceName;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppIDSNetBearerInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetPreviousBearerInfo(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    HACK to make event propagation work even after call goes down
  -------------------------------------------------------------------------*/
  ifaceHandle = GetHandle();
  if (0 == ifaceHandle && 0 != mStaleIfaceHandle)
  {
    ifaceHandle = mStaleIfaceHandle;
    LOG_MSG_INFO1_2("Network::GetPreviousBearerInfo(): "
                    "Using stale handle 0x%x for obj 0x%p",
                    mStaleIfaceHandle, this);
  }

  *ppIDSNetBearerInfo = NULL;
  memset (&bearerTechInfo, 0, sizeof (BearerTechType));

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get bearer technology.
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (ifaceHandle,
                       IFACE_IOCTL_GET_PREVIOUS_BEARER_TECHNOLOGY,
                       static_cast <void *> (&bearerTechInfo));
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Convert the network type to iface name.
  -------------------------------------------------------------------------*/
  switch (bearerTechInfo.current_network)
  {
    case PS_IFACE_NETWORK_CDMA:
    {
      ifaceName = IfaceName::IFACE_CDMA_SN;
      break;
    }

    case PS_IFACE_NETWORK_UMTS:
    {
      ifaceName = IfaceName::IFACE_UMTS;
      break;
    }

    case PS_IFACE_NETWORK_WLAN:
    {
      ifaceName = IfaceName::IFACE_WLAN;
      break;
    }

    default:
    {
      LOG_MSG_ERROR_2 ("Network::GetPreviousBearerInfo(): Unknown network %d, "
                       "obj 0x%p", bearerTechInfo.current_network, this);
      result = QDS_INTERNAL;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Construct the Bearer tech object using the IOCTL result.
  -------------------------------------------------------------------------*/
  *ppIDSNetBearerInfo =
    static_cast <IBearerInfo *>
      (new BearerTech (ifaceName,
                       bearerTechInfo.data.cdma_type.rat_mask,
                       bearerTechInfo.data.cdma_type.so_mask,
                       bearerTechInfo.data.umts_type.rat_mask,
                       NULL));
  if (0 == *ppIDSNetBearerInfo)
  {
    LOG_MSG_ERROR_1 ("Network::GetPreviousBearerInfo(): Failed to alloc "
                     "BearerTech, obj 0x%p", this);
    result = AEE_ENOMEMORY;
    goto bail;
  }

  result = AEE_SUCCESS;

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetPreviousBearerInfo(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetPreviousBearerInfo() */

ds::ErrorType Network::GetQoSAwareInfoCode
(
  QoSInfoCodeType *  infoCode
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_GET_QOS_AWARE_INFO_CODE,
                                 static_cast <void *> (infoCode));

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetQoSAwareInfoCode(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetQoSAwareInfoCode() */


ds::ErrorType Network::GetAllIfaces
(
  IfaceIdType *  ifaceIdArr,
  int            ifaceIdArrLen,
  int *          ifaceIdArrLenReq
)
{
  PSAllIfacesType  allIfacesInfo;
  ds::ErrorType    result;
  unsigned int     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Network::GetAllIfaces(): "
                            "Obj 0x%p, iface id arr len %d",
                            this, ifaceIdArrLen);

  if (ifaceIdArrLen <= 0)
  {
    LOG_MSG_FUNCTION_EXIT_1 ("Network::GetAllIfaces(): "
                             "0 iface id arr len, obj 0x%p", this);
    return AEE_SUCCESS;
  }

  if (0 == ifaceIdArr)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetAllIfaces(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  memset (&allIfacesInfo, 0, sizeof(allIfacesInfo));

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Call the IOCTL
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_ALL_IFACES,
                         static_cast <void *> (&allIfacesInfo));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert to out params.
    -----------------------------------------------------------------------*/
    for (index = 0;
         index < allIfacesInfo.number_of_ifaces && 
         index < (unsigned int)ifaceIdArrLen;
         index++)
    {
      ifaceIdArr[index] = allIfacesInfo.ifaces[index];
    }

    if (0 != ifaceIdArrLenReq)
    {
      *ifaceIdArrLenReq = (int) allIfacesInfo.number_of_ifaces;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetAllIfaces(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetAllIfaces() */


ds::ErrorType Network::QueryInterface
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::QueryInterface(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (iid)
  {
    case AEEIID_INetwork:
    case AEEIID_IQI:
    {
      *ppo   = static_cast <INetwork *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_INetworkPriv:
    {
      *ppo   = static_cast <INetworkPriv *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_INetworkControl:
    {
      *ppo   = static_cast <INetworkControl *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_INetworkExt:
    {
      *ppo   = static_cast <INetworkExt *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_INetworkExt2:
    {
      *ppo   = static_cast <INetworkExt2 *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_IIPFilterManagerPriv:
    {
      *ppo   = static_cast <IIPFilterManagerPriv *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_IQoSManager:
    {
      result = CreateQoSManager (reinterpret_cast <IQoSManager **> (ppo));
      break;
    }
#ifdef FEATUTE_DATA_PS_MCAST
    case AEEIID_IMCastManager:
    {
      result = CreateMCastManager (reinterpret_cast <IMCastManager **> (ppo));
      break;
    }
#endif // FEATUTE_DATA_PS_MCAST
    case AEEIID_IFirewallManager:
    {
      result =
        CreateNetFirewallManager (reinterpret_cast <IFirewallManager **> (ppo));
      break;
    }
    case AEEIID_INetwork1x:
    case AEEIID_INetwork1xPriv:
    case AEEIID_INetworkUMTS:
    case AEEIID_INetworkIPv6:
    case AEEIID_INetworkIPv6Priv:
    {
      result = GetTechObject(iid, ppo);
      break;
    }

    default:
    {
      result = AEE_ECLASSNOTSUPPORT;
      break;
    }
  }

  LOG_MSG_FUNCTION_EXIT_4 ("Network::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p, iid 0x%x",
                           result, *ppo, this, iid);
  mpICritSect->Leave();

  return result;
} /* Network::QueryInterface() */

ds::ErrorType Network::CreateNetFirewallManager
(
  IFirewallManager **  ppIFirewallMgr
)
{
  ds::ErrorType  result;
  int32          ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == ppIFirewallMgr)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::CreateNetFirewallManager(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    ifaceHandle = GetHandle();
    if (0 == ifaceHandle)
    {
      LOG_MSG_ERROR_1 ("Network::CreateNetFirewallManager(): "
                       "NULL Iface handle, obj 0x%p", this);
      result = QDS_EINVAL;
      break;
    }

    /*-----------------------------------------------------------------------
      Create a new FirewallManager object all the time. If the network
      object's handle changes anytime, old FirewallManager does not make sense.
    -----------------------------------------------------------------------*/
    *ppIFirewallMgr =
      static_cast <IFirewallManager *> (new FirewallManager (ifaceHandle));
    if (0 == *ppIFirewallMgr)
    {
      LOG_MSG_ERROR_1 ("Network::CreateNetFirewallManager(): "
                       "Failed to alloc FirewallManager, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = AEE_SUCCESS;
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::CreateNetFirewallManager(): "
                           "Result 0x%x, FirewallManager 0x%p, obj 0x%p",
                           result, *ppIFirewallMgr, this);
  mpICritSect->Leave();
  return result;

} /* Network::CreateNetFirewallManager() */
ds::ErrorType Network::GetNetworkIPv6
(
  AEEIID   iid,
  void **  ppo
)
{
#ifdef FEATURE_DATA_PS_IPV6
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetNetworkIPv6(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  if (AEEIID_INetworkIPv6 != iid && AEEIID_INetworkIPv6Priv != iid)
  {
    LOG_MSG_INVALID_INPUT_2 ("Network::GetNetworkIPv6(): "
                             "Invalid iid 0x%x, obj 0x%p", iid, this);
    return AEE_ECLASSNOTSUPPORT;
  }

  mpICritSect->Enter();

  do
  {
    if (0 == mpNetworkIPv6)
    {
      //TODO What if handle is 0?
      mpNetworkIPv6 = new NetworkIPv6(GetHandle());
      if (0 == mpNetworkIPv6)
      {
        LOG_MSG_ERROR_1 ("Network::GetNetworkIPv6(): "
                         "Failed to alloc NetworkIPv6, obj 0x%p", this);
        result = AEE_ENOMEMORY;
        break;
      }

      //TODO Combine constructor and Init to do CreateInstance
      result = mpNetworkIPv6->Init();
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_ERROR_1 ("Network::GetNetworkIPv6(): "
                         "Failed to init NetworkIPv6, obj 0x%p", this);
        break;
      }
    }
    else
    {
      mpNetworkIPv6->SetIfaceHandle(GetHandle());
    }

    /*-----------------------------------------------------------------------
      We are returning an interface. Perform AddRef()
    -----------------------------------------------------------------------*/
    (void) mpNetworkIPv6->AddRef();

    if (AEEIID_INetworkIPv6 == iid)
    {
      *ppo = static_cast <INetworkIPv6 *> (mpNetworkIPv6);
    }
    else
    {
      *ppo = static_cast <INetworkIPv6Priv *> (mpNetworkIPv6);
    }

    result = AEE_SUCCESS;
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    DS_UTILS_RELEASEIF(mpNetworkIPv6);
  }

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetNetworkIPv6(): "
                           "Result 0x%x, NetworkIPv6 0x%p, obj 0x%p",
                           result, mpNetworkIPv6, this);
  mpICritSect->Leave();
  return result;

#else
  return AEE_ECLASSNOTSUPPORT;
#endif /* FEATURE_DATA_PS_IPV6 */

} /* Network::GetNetworkIPv6() */

ds::ErrorType Network::GetNetworkUMTS
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
  ds::Net::IfaceNameType   ifaceName;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetNetworkUMTS(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  if (AEEIID_INetworkUMTS != iid)
  {
    LOG_MSG_INVALID_INPUT_2 ("Network::GetNetworkUMTS(): "
                             "Invalid iid 0x%x, obj 0x%p", iid, this);
    return AEE_ECLASSNOTSUPPORT;
  }

  mpICritSect->Enter();

  do
  {
    if (0 == mpNetworkUMTS)
    {
      /*---------------------------------------------------------------------
        Make sure that iface name is compatible with IFACE_UMTS
      ---------------------------------------------------------------------*/
      result = GetIfaceName(&ifaceName);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_ERROR_1 ("Network::GetNetworkUMTS(): "
                         "Failed to get iface name, obj 0x%p", this);
        break;
      }

      if (IfaceName::IFACE_UMTS != ifaceName &&
          IfaceName::IFACE_STA != ifaceName &&
          IfaceName::IFACE_EPC != ifaceName)
      {
        LOG_MSG_ERROR_2 ("Network::GetNetworkUMTS(): "
                         "Incompatible iface name %d, obj 0x%p",
                         ifaceName, this);
        result = AEE_ECLASSNOTSUPPORT;
        break;
      }

      mpNetworkUMTS = static_cast <INetworkUMTS *> (new NetworkUMTS(this));
      if (0 == mpNetworkUMTS)
      {
        LOG_MSG_ERROR_1 ("Network::GetNetworkUMTS(): "
                         "Failed to alloc NetworkUMTS, obj 0x%p", this);
        result = AEE_ENOMEMORY;
        break;
      }
    }

    *ppo =  mpNetworkUMTS;
    (void) mpNetworkUMTS->AddRef();

    result = AEE_SUCCESS;
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetNetworkUMTS(): "
                           "Result 0x%x, NetworkUMTS 0x%p, obj 0x%p",
                           result, mpNetworkUMTS, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetNetworkUMTS() */


ds::ErrorType Network::GetNetwork1x
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType                 result;
  ds::Net::IfaceNameType        ifaceName;
  Network1X*                    pNetwork1x = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetNetwork1x(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  if (AEEIID_INetwork1x != iid && AEEIID_INetwork1xPriv != iid)
  {
    LOG_MSG_INVALID_INPUT_2 ("Network::GetNetwork1x(): "
                             "Invalid iid 0x%x, obj 0x%p", iid, this);
    return AEE_ECLASSNOTSUPPORT;
  }

  mpICritSect->Enter();

  do
  {
    if (0 == mpTechObjNetwork1x)
    {
      /*---------------------------------------------------------------------
        Make sure that iface name is compatible with IFACE_UMTS
      ---------------------------------------------------------------------*/
      result = GetIfaceName(&ifaceName);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_ERROR_1 ("Network::GetNetwork1x(): "
                         "Failed to get iface name, obj 0x%p", this);
        break;
      }

      if (IfaceName::IFACE_CDMA_SN != ifaceName &&
          IfaceName::IFACE_EPC != ifaceName &&
          IfaceName::IFACE_STA != ifaceName &&
          IfaceName::IFACE_CDMA_BCAST != ifaceName &&
          IfaceName::IFACE_CDMA_AN != ifaceName)
      {
        LOG_MSG_ERROR_2 ("Network::GetNetwork1x(): "
                         "Incompatible iface name %d, obj 0x%p",
                         ifaceName, this);
        result = AEE_ECLASSNOTSUPPORT;
        break;
      }

      //TODO Combine in to CreateInstance
      pNetwork1x = new Network1X (this, GetHandle());
      if (0 == pNetwork1x)
      {
        LOG_MSG_ERROR_1("Network::GetNetwork1x(): "
                        "Failed to alloc Network1x, obj 0x%p", this);
        result = AEE_ENOMEMORY;
        break;
      }

      result = pNetwork1x->Init();
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_ERROR_1("Network::GetNetwork1x(): "
                        "Failed to init Network1x, obj 0x%p", this);
        break;
      }

      mpTechObjNetwork1x     = static_cast <INetwork1x *> (pNetwork1x);
      mpTechObjNetwork1xPriv = static_cast <INetwork1xPriv *> (pNetwork1x);
    }

    if (AEEIID_INetwork1x == iid)
    {
      *ppo = mpTechObjNetwork1x;
      (void) mpTechObjNetwork1x->AddRef();
    }
    else
    {
      *ppo = mpTechObjNetwork1xPriv;
      (void) mpTechObjNetwork1xPriv->AddRef();
    }

    result = AEE_SUCCESS;

  } while (0);

  /*-------------------------------------------------------------------------
    Release Network1x in case of failure
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    DS_UTILS_RELEASEIF(pNetwork1x);
  }

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetNetwork1x(): "
                           "Result 0x%x, Network1x 0x%p, obj 0x%p",
                           result, *ppo, this);
  mpICritSect->Leave();

  return result;
} /* Network::GetNetwork1x() */


ds::ErrorType Network::GetTechObject
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetTechObject(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    switch (iid)
    {
      case AEEIID_INetworkIPv6:
      case AEEIID_INetworkIPv6Priv:
      {
        result = GetNetworkIPv6 (iid, ppo);
        break;
      }

      case AEEIID_INetworkUMTS:
      {
        result = GetNetworkUMTS (iid, ppo);
        break;
      }

      case AEEIID_INetwork1x:
      case AEEIID_INetwork1xPriv:
      {
        result = GetNetwork1x (iid, ppo);
        break;
      }

      default:
      {
        result = AEE_ECLASSNOTSUPPORT;
        break;
      }
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_4 ("Network::GetTechObject(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p, iid 0x%x",
                           result, *ppo, this, iid);
  mpICritSect->Leave();

  return result;
} /* Network::GetTechObject() */

/*---------------------------------------------------------------------------
  Inherited functions from IIPFilterMgr.
---------------------------------------------------------------------------*/
ds::ErrorType Network::RegisterFilters
(
  int                  fi_result,
  IIPFilterPriv **     ppIIPFilterSpec,
  int                  filtersLen,
  IIPFilterRegPriv **  ppIIPFfilterReg,
  IfaceGroupType       iface_group
)
{
  PSIfaceIPFilterAddParamType   filterAddParam;
  ds::ErrorType                 result;
  int32                         ifaceHandle;
  int32                         fltrHandle = PS_IFACE_IPFLTR_INVALID_HANDLE;
  int                           index;
  PSIFaceNameEnumType           ifaceGroup;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("Network::RegisterFilters(): "
                            "Obj 0x%p, #of fltr %d, fltr result %d",
                            this, filtersLen, fi_result);

  if (0 == ppIIPFilterSpec || 0 >= filtersLen)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::RegisterFilters(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  memset (&filterAddParam, 0, sizeof(filterAddParam));

  /*-------------------------------------------------------------------------
    Allocate memory to hold array to ip_filter_type
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC( filterAddParam.fi_ptr_arr,
                            sizeof (ip_filter_type) * filtersLen, 
                            void*);

  if  (0 == filterAddParam.fi_ptr_arr)
  {
    return AEE_ENOMEMORY;
  }

  filterAddParam.enable             = TRUE;
  filterAddParam.is_validated       = FALSE;
  filterAddParam.num_filters        = (uint8) filtersLen;
  filterAddParam.subset_id          = 0;
  filterAddParam.fi_result          = fi_result;
  filterAddParam.filter_type        = IPFLTR_DEFAULT_TYPE;
  filterAddParam.fltr_compare_f_ptr = NULL;
  filterAddParam.fltr_priority      = PS_IFACE_IPFLTR_PRIORITY_DEFAULT;

  for (index = 0; index < filtersLen; index++)
  {
    //TODO add validation
    (void) DS2PSIPFilterSpec
           (
             ppIIPFilterSpec[index],
             &(((ip_filter_type *) filterAddParam.fi_ptr_arr)[index])
           );
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      If filters are already installed, uninstall them.
    -----------------------------------------------------------------------*/
    DS_UTILS_RELEASEIF(mpIPFilterReg);

    ifaceHandle = GetHandle();
    if (AEE_SUCCESS != DS2PSIFaceNameType(iface_group, &ifaceGroup))
    {
      LOG_MSG_ERROR_1 ("Network::RegisterFilters(): Failed to convert group, "
                       "obj 0x%p", this);
      result = AEE_EFAILED;
      break;
    }
    result = PSIfaceIPFilterAdd (ifaceHandle,
                                 ifaceGroup,
                                 IP_FLTR_CLIENT_SOCKETS,
                                 &filterAddParam,
                                 &fltrHandle);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network::RegisterFilters(): Failed to install fltr, "
                       "obj 0x%p", this);
      break;
    }

    //TODO Ideally the constructor shud have taken filters as param ->
    //CreateInstance()
    mpIPFilterReg = new IPFilterReg (ifaceHandle, fltrHandle);
    if (0 == mpIPFilterReg)
    {
      LOG_MSG_ERROR_1 ("Network::RegisterFilters(): Failed to alloc IPFilterReg, "
                       "obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    (void) mpIPFilterReg->AddRef();
    result = AEE_SUCCESS;

    *ppIIPFfilterReg = static_cast <IIPFilterRegPriv *> (mpIPFilterReg);

    LOG_MSG_FUNCTION_EXIT_2 ("Network::RegisterFilters(): "
                             "Fltr handle %d, obj 0x%p", fltrHandle, this);
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    (void)
      PSIfaceIPFilterDelete (ifaceHandle, IP_FLTR_CLIENT_SOCKETS, fltrHandle);
  }

  PS_SYSTEM_HEAP_MEM_FREE (filterAddParam.fi_ptr_arr);

  mpICritSect->Leave();

  return result;
} /* Network::RegisterFilters() */

ds::ErrorType CDECL Network::GetQoSMode
(
  QoSModeType* qosMode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    currently this API is not supported
  -------------------------------------------------------------------------*/
  return AEE_EUNSUPPORTED;

} /* Network::GetQoSMode() */

ds::ErrorType CDECL Network::GetDormancyInfoCode
(
  DormancyInfoCodeType *  pDormancyInfoCode
)
{
  PhysLink *     pPhysLink = NULL;
  IPhysLink *    pIPhysLink = NULL;
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Get Dormancy info code from phys link.
    -----------------------------------------------------------------------*/
    result = GetTXPhysLink (&pIPhysLink);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    pPhysLink = reinterpret_cast <PhysLink *> (pIPhysLink);
    result    = pPhysLink->GetDormancyInfoCode (pDormancyInfoCode);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetDormancyInfoCode(): "
                           "Result 0x%x, obj 0x%p", result, this);

  DS_UTILS_RELEASEIF(pIPhysLink);
  mpICritSect->Leave();

  return result;
} /* Network::GetDormancyInfoCode() */

ds::ErrorType Network::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("Network::GetSignalBus(): Obj 0x%p, event 0x%x, ppISigBus 0x%x",
                            this, eventID, ppISigBus);

  if (0 == ppISigBus)
  {
    return QDS_EFAULT;
  }

  *ppISigBus = NULL;

  mpICritSect->Enter();

  switch (eventID)
  {
    case NetworkEvent::QDS_EV_IFACE_UP:
    {
      *ppISigBus = mpSigBusIfaceUp;
      break;
    }

    case NetworkEvent::QDS_EV_IFACE_GOING_DOWN:
    {
      *ppISigBus = mpSigBusIfaceGoingDown;
      break;
    }

    case NetworkEvent::QDS_EV_IFACE_DOWN:
    {
      *ppISigBus = mpSigBusIfaceDown;
      break;
    }

    case NetworkEvent::QDS_EV_IFACE_ACTIVE_OUT_OF_USE:
    {
      *ppISigBus = mpSigBusIfaceActiveOutOfUse;
      break;
    }

    case NetworkEvent::QDS_EV_IFACE_COMING_UP:
    {
      *ppISigBus = mpSigBusIfaceComingUp;
      break;
    }

    case NetworkEvent::QDS_EV_IFACE_CONFIGURING:
    {
      *ppISigBus = mpSigBusIfaceConfiguring;
      break;
    }

    case NetworkEvent::QDS_EV_IFACE_ROUTEABLE:
    {
      *ppISigBus = mpSigBusIfaceRouteable;
      break;
    }

    default:
    {
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
      LOG_MSG_INFO1_2 ("Network::GetSignalBus(): "
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

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p",
                           result, *ppISigBus, this);

  mpICritSect->Leave();
  return result;

} /* Network::GetSignalBus() */

ds::ErrorType Network::GoNull
(
  ReleaseReasonType  nullReason
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_GO_NULL,
                                 static_cast <void *> (&nullReason));

  LOG_MSG_FUNCTION_EXIT_3 ("Network::GoNull(): "
                           "Result 0x%x, obj 0x%p, GO_NULL reason %d",
                           result, this, nullReason);
  mpICritSect->Leave();

  return result;
} /* Network::GoNull() */


void Network::SetHandle
(
  int32  objHandle
)
{
  int32                                    ifaceHandle;
  NetPlatform::NetworkDownReasonType       lastNetDownReason;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Network::SetHandle(): Obj 0x%p, handle 0x%x",
                            this, objHandle);

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Reset mBringupAgain, mBringupFirst, mTeardown flags.
    Cache the last network down reason.
  -------------------------------------------------------------------------*/
  if (0 == objHandle)
  {
    mBringupFirst = FALSE;
    mWouldblock   = FALSE;
    mBringupAgain = FALSE;
    mTeardown     = FALSE;

    ifaceHandle = GetHandle();
    if (0 != ifaceHandle)
    {
      NetPlatform::GetLastNetworkDownReason (ifaceHandle, &lastNetDownReason);
      mLastNetDownReason = (NetDownReasonType) lastNetDownReason;
    }
  }

  /*-------------------------------------------------------------------------
    Call parent method
  -------------------------------------------------------------------------*/
  Handle::SetHandle(objHandle);

  mpICritSect->Leave();
  return;

} /* Network::SetHandle() */

ds::ErrorType Network::SetFMCTunnelParams
(
  const FMCTunnelParamsType *  tunnelParams
)
{
  NetPlatform::PSFMCTunnelParamsType     psTunnelParams;
  ds::SockAddrInternalType               tempAddr;
  ds::SockAddrIN6InternalType            v6RemoteAddr;
  ds::ErrorType                          result;
  uint16                                 psFamily;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&psTunnelParams, 0, sizeof(NetPlatform::PSFMCTunnelParamsType));
  memset(&tempAddr, 0, sizeof(ds::SockAddrInternalType));
  memset(&v6RemoteAddr, 0, sizeof(ds::SockAddrIN6InternalType));

  /*-------------------------------------------------------------------------
    Convert to PS socket address
  -------------------------------------------------------------------------*/
  memscpy(&tempAddr,
    sizeof(ds::SockAddrStorageType),
    tunnelParams->tunnelEndPointAddr,
    sizeof(ds::SockAddrStorageType));

  tempAddr.v4.family = ps_ntohs(tempAddr.v4.family);

  switch (tempAddr.v4.family)
  {
    case AddrFamily::QDS_AF_INET:
    {
      result = DS2PSSockAddrFamily(tempAddr.v4.family, &psFamily);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INVALID_INPUT_2("Network::SetFMCTunnelParams(): "
                                "Wrong addr family %d, obj 0x%p",
                                tempAddr.v4.family, this);
        return QDS_EINVAL;
      }

      tempAddr.v4.family                      = psFamily;
      psTunnelParams.tunnel_end_point_info_ptr =
        reinterpret_cast <ps_sockaddr *> (&tempAddr);

      break;
    }

    case AddrFamily::QDS_AF_INET6:
    {
      memscpy(&v6RemoteAddr,
        sizeof(ds::SockAddrIN6InternalType),
        tunnelParams->tunnelEndPointAddr,
        sizeof(ds::SockAddrIN6InternalType));

      v6RemoteAddr.family = ps_ntohs(v6RemoteAddr.family);

      result = DS2PSSockAddrFamily(v6RemoteAddr.family, &psFamily);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INVALID_INPUT_2("Network::SetFMCTunnelParams(): "
                                "Wrong addr family %d, obj 0x%p",
                                v6RemoteAddr.family, this);
        return QDS_EINVAL;
      }

      v6RemoteAddr.family                      = psFamily;
      psTunnelParams.tunnel_end_point_info_ptr =
        reinterpret_cast <ps_sockaddr *> (&v6RemoteAddr);

      break;
    }

    case AddrFamily::QDS_AF_UNSPEC:
    default:
    {
      LOG_MSG_INVALID_INPUT_2("Network::SetFMCTunnelParams(): "
                              "Wrong addr family %d, obj 0x%p",
                              tempAddr.v4.family, this);
      return QDS_EINVAL;
    }
  }

  psTunnelParams.addr_len       = tunnelParams->addrLen;
  psTunnelParams.is_nat_present = tunnelParams->IsNatPresent;
  psTunnelParams.stream_id      = tunnelParams->streamId;

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_UW_FMC_SET_TUNNEL_PARAMS,
                                 static_cast <void *> (&psTunnelParams));

  LOG_MSG_FUNCTION_EXIT_2 ("Network::SetFMCTunnelParams(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();
  return result;

} /* Network::SetFMCTunnelParams() */

ds::ErrorType Network::ResetFMCTunnelParams
(
  void
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctl(GetHandle(),
                      IFACE_IOCTL_UW_FMC_RESET_TUNNEL_PARAMS,
                      NULL);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::ResetFMCTunnelParams(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* Network::ResetFMCTunnelParams() */

ds::ErrorType Network::GetDeviceName
(
  DeviceName *  pDeviceName
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctl(GetHandle(),
                      IFACE_IOCTL_GET_DEVICE_NAME,
                      static_cast <void *> (pDeviceName));

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetDeviceName(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();
  return result;

} /* Network::GetDeviceName() */

ds::ErrorType Network::GetGatewayAddress
(
  ds::IPAddrType *  argIPAddress
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return IPAddrIOCTL (IFACE_IOCTL_GET_GATEWAY_ADDR, argIPAddress);

} /* Network::GetGatewayAddress() */

ds::ErrorType Network::GetSubnetMask
(
  ds::IPAddrType *  pSubnetMask
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return IPAddrIOCTL (IFACE_IOCTL_GET_SUBNET_MASK, pSubnetMask);

} /* Network::GetSubnetMask */

boolean Network::EventIsNetworkState
(
  int32 eventId
)
{
  boolean ret = false;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (eventId)
  {
    case NetworkEvent::QDS_EV_IFACE_UP:
    case NetworkEvent::QDS_EV_IFACE_GOING_DOWN:
    case NetworkEvent::QDS_EV_IFACE_ENABLED:
    case NetworkEvent::QDS_EV_IFACE_DISABLED:
    case NetworkEvent::QDS_EV_IFACE_DOWN:
    case NetworkEvent::QDS_EV_IFACE_ACTIVE_OUT_OF_USE:
    case NetworkEvent::QDS_EV_IFACE_COMING_UP:
    case NetworkEvent::QDS_EV_IFACE_CONFIGURING:
    case NetworkEvent::QDS_EV_IFACE_ROUTEABLE:
      ret = true;
      break;
  }

  return ret;

} /* Network::EventIsNetworkState */

EventType
Network::NetworkState2Event
(
  NetworkStateType argNetState
)
{
  int32 ret = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (argNetState)
  {
    case NetworkState::QDS_CLOSED:
    case NetworkState::QDS_LINGERING:
      ret = NetworkEvent::QDS_EV_IFACE_DOWN;
      break;

    case NetworkState::QDS_OPEN_IN_PROGRESS:
      ret = NetworkEvent::QDS_EV_IFACE_COMING_UP;
      break;

    case NetworkState::QDS_OPEN:
      ret = NetworkEvent::QDS_EV_IFACE_UP;
      break;

    case NetworkState::QDS_CLOSE_IN_PROGRESS:
      ret = NetworkEvent::QDS_EV_IFACE_GOING_DOWN;
      break;

    default:
      LOG_MSG_ERROR_1 ("Network::NetworkState2Event(): network state unsupported, "
                       "obj 0x%p", this);
  }

  return ret;

} /* Network::NetworkState2Event */

ds::ErrorType Network::GetExtIpv6DelAddr
(
  ds::INAddr6Type value
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  memscpy ( (void *)value, sizeof(ds::INAddr6Type),
           (const void *) mDeletedIPv6Addr, 
           sizeof(ds::INAddr6Type));

  LOG_MSG_FUNCTION_EXIT_1 ("Network::GetExtIpv6DelAddr(): , obj 0x%p", this);

  mpICritSect->Leave();
  
  return AEE_SUCCESS;

}

ds::ErrorType Network::GetExtIPConfigEx
(
 ::ds::Net::NetworkExtendedIpConfigExType* value
 )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  *value = mCachedExtendedMask;

  LOG_MSG_FUNCTION_EXIT_1 ("Network::GetExtIPConfigEx(): , obj 0x%p", this);

  mpICritSect->Leave();

  return AEE_SUCCESS;

}

ds::ErrorType Network::GetApnParamInfo
(
 ApnParamChangeList* pGetApnParamInfo
)
{
  ds::ErrorType result = AEE_EFAILED;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (0 == pGetApnParamInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetApnParamInfo(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to get data path
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_GET_CHANGED_APN_PARAM_INFO,
                       static_cast <void *> (pGetApnParamInfo));

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("Network::GetApnParamInfo(): "
                    "Failed, obj 0x%p", this);
    goto bail;
  }

  /* Fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetApnParamInfo(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
}

ds::ErrorType Network::GetMipInfo
(
  ds::IPAddrType* pMipInfo
)
{
  ds::ErrorType  result  = AEE_SUCCESS;
  ps_iface_ioctl_mip_ma_info_type psMipInfo;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // Verify that input params are non-NULL
  if (0 == pMipInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network::GetMipInfo(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Get the iface handle and call the relevant IOCTL
    -----------------------------------------------------------------------*/
    memset (&psMipInfo, 0, sizeof(psMipInfo));

    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_707_GET_MIP_MA_INFO,
                         static_cast <void*> (&psMipInfo));

    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert IOCTL result back to ds::IPAddrType
    -----------------------------------------------------------------------*/
    result = PS2DSIPAddr (&psMipInfo.fa_addr, pMipInfo);
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("Network::GetMipInfo(): Result 0x%x, "
                           "obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;
} /* Network::GetMipInfo() */

ds::ErrorType Network::GetHandoffInfo
(
 HandoffinfoType *value
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  *value = mcachedHandoffinfo;

  LOG_MSG_FUNCTION_EXIT_1 ("Network::GetHandoffInfo(): , obj 0x%p", this);

  mpICritSect->Leave();

  return AEE_SUCCESS;

}


ds::ErrorType Network::GetHandoffFailureInfo
(
 HandoffFailureinfoType *value
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  *value = mcachedHandoffFailureinfo;

  LOG_MSG_FUNCTION_EXIT_1 ("Network::GetHandoffFailureInfo(): , obj 0x%p", this);

  mpICritSect->Leave();

  return AEE_SUCCESS;

}

