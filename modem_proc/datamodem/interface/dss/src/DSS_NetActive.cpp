/*======================================================

FILE:  DSS_NetActive.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSNetActive class

=====================================================

Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetActive.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-06-24 pg  Added to support qos handle when registration to net initiated qos.
  2013-11-01 rk  Removed feature mobileap.
  2011-07-15 rp  QShrink2 changes: MSG_* macros are removed from inline functions.
  2011-05-23 aa  Class Created.

===========================================================================*/


//===================================================================
//   Includes and Public Data Declarations
//===================================================================

#include "DSS_NetActive.h"
#include "DSS_Globals.h"
#include "DSS_Conversion.h"
#include "DSS_Common.h"

#include "DSS_NetworkStateHandler.h"
#include "DSS_NetworkIPHandler.h"
#include "DSS_ExtendedIPConfigHandler.h"
#include "DSS_RFConditionsHandler.h"
#include "DSS_BearerTechHandler.h"
#include "DSS_OutageHandler.h"
#include "DSS_PhysLinkStateHandler.h"
#include "DSS_PrimaryQoSModifyStatusHandler.h"
#include "DSS_MTPDRequestHandler.h"
#include "DSS_SlottedResultHandler.h"
#include "DSS_SlottedSessionChangedHandler.h"
#include "DSS_HDRRev0RateInteriaHandler.h"
#include "DSS_QoSProfileChangedHandler.h"
#include "DSS_IPv6PrefixChangedStateHandler.h"
#include "DSS_IDSNetworkPrivScope.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_MemoryManagement.h"
#include "DSS_PrivIpv6Addr.h"
#include "ds_Net_IIPv6Address.h"
#include "ds_Net_IQoSManagerPriv.h"
#include "DSS_GenScope.h"

#include "ds_Net_INetworkPriv.h"

#include "DSS_MCast.h"
#include "DSS_EventHandlerMCastMBMSCtrl.h"
#include "DSS_NetMCastMBMSCtrl.h"

#include "ds_Addr_Def.h"
#include "ds_Utils_Atomic.h"
#include "ds_Utils_CreateInstance.h"

using namespace ds::Net;
using namespace ds::Error;
//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------

//===================================================================
//              Macro Definitions
//===================================================================
#define DSS_DNS_PRIMARY_ADDR_IDX   0

#define DSS_DNS_SECONDARY_ADDR_IDX 1

#ifndef min
#define min(a,b)  ((a) < (b) ? (a):(b))
#endif

//===================================================================
//            DSSNetActive Functions Definitions
//===================================================================

// TODO: There's nothing preventing this class from being instantiated
// (has default operator new and operator delete)

//===================================================================
//  FUNCTION:   DSSNetActive::DSSNetActive
//
//  DESCRIPTION:
//  Constructor of the DSSNetActive class.
//===================================================================
DSSNetActive::DSSNetActive(sint15 netHandle):
   mNetHandle(netHandle), mpIDSNetworkPriv(NULL), mpIDSNetwork(NULL),
   mpIDSNetworkExt(NULL), mpIDSNetworkExt2(NULL), mpDSNetQoSManager(NULL),
   mpDSSQoSSecondaryList(NULL), mpDSSQoSNetInitiatedReqList(NULL),
   mpDSSQoSNetInitiatedList(NULL), mpDSSPrivIpv6AddrList(NULL), mpDSSNetQoSDefault(NULL),
#ifdef FEATUTE_DATA_PS_MCAST
   mpDSSMCastList(NULL), mpDSSNetMCastMBMSList(NULL), mpDSNetMCastManager(NULL),
#endif //FEATUTE_DATA_PS_MCAST
   mpDSNetFirewallManager(NULL)
   , mNetCb(NULL), mpNetUserData(NULL),
   mpIfaceUpSignal(NULL), mpIfaceDownSignal(NULL), mpIfaceUpSignalCtl(NULL),
   mpIfaceDownSignalCtl(NULL), mpPrimaryQoSModifyStatusHandler(NULL),
   mLastIfaceStateSentToApp(IFACE_STATE_INVALID), mpCritSect(NULL)
{
   memset(DSSFirewallHandleToObject, 0, 
          DSS_MAX_FIREWALL_HANDLES * sizeof(DSSFirewallHandleToObjectMapping));
   
   // DSSNetActive::Destructor makes use of mpCritSect before Releasing it
   // So we ASSERT here in case mpCritSect fails, otherwise 
   // DSSNetActive::Destructor will access uninitialized mpCritSect
   if (AEE_SUCCESS != DSSGlobals::Instance()->GetCritSect(&mpCritSect)) {
      DATA_ERR_FATAL("Couldn't fetch CritSect object from DSSGlobals");
   }
}

void * DSSNetActive::operator new (unsigned int numBytes) throw()
{
   return ps_mem_get_buf( PS_MEM_DSAL_NET_ACTIVE_TYPE);
}

void DSSNetActive::operator delete (void *  bufPtr) throw()
{
   PS_MEM_FREE(bufPtr);
}

AEEResult DSSNetActive::GetIfaceIdForEventInfo(sint15 appId, dss_iface_id_type* pIfaceId)
{
   AEEResult ret = AEE_SUCCESS;
   dss_iface_id_type ifaceId = DSS_IFACE_INVALID_ID;

   DSSCritScope cs(*mpCritSect);

   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }

   // get iface id
   ret = GetIfaceIdByAppId(appId, &ifaceId);
   if (AEE_SUCCESS != ret) {
      return ret;
   }

   // check if iface id is invalid
   // if iface id is invalid, get stale iface id
   if (DSS_IFACE_INVALID_ID == ifaceId) {
      LOG_MSG_INFO1_0("GetIfaceIdForEventInfo: "
                      "iface_id was cleared, providing saved iface id");

      // iface id is invalid. Get stale iface id.
      return DSSIfaceId::BuildIfaceId(
         mpIDSNetwork,
         DSSIfaceId::IFACE_ID_TYPE_STALE,
         appId,
         pIfaceId);
   }

   *pIfaceId = ifaceId;

   return AEE_SUCCESS;
}

AEEResult DSSNetActive::GetIDSNetworkPrivObject(INetworkPriv** ppIDSNetworkPriv)
{
   DSSCritScope cs(*mpCritSect);
   *ppIDSNetworkPriv = mpIDSNetworkPriv;
   if (NULL == mpIDSNetworkPriv) {
      return QDS_EINVAL;
   }
   (void)mpIDSNetworkPriv->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetActive::GetIDSNetworkObject(INetwork** ppIDSNetwork)
{
   // TODO: maybe it should not be inline?
   DSSCritScope cs(*mpCritSect);
   *ppIDSNetwork = mpIDSNetwork;
   if (NULL == mpIDSNetwork) {
      return QDS_EINVAL;
   }
   (void)mpIDSNetwork->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetActive::GetIDSNetworkExtObject(INetworkExt** ppIDSNetworkExt)
{
   DSSCritScope cs(*mpCritSect);
   *ppIDSNetworkExt = mpIDSNetworkExt;
   if (NULL == mpIDSNetworkExt) {
      return QDS_EINVAL;
   }
   (void)mpIDSNetworkExt->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetActive::GetIDSNetworkExt2Object(INetworkExt2** ppIDSNetworkExt2)
{
  DSSCritScope cs(*mpCritSect);
  *ppIDSNetworkExt2 = mpIDSNetworkExt2;
  if (NULL == mpIDSNetworkExt2) {
    return QDS_EINVAL;
  }
  (void)mpIDSNetworkExt2->AddRef();
  return AEE_SUCCESS;
}


//===================================================================
//  FUNCTION:   DSSNetActive::Init
//
//  DESCRIPTION:
//  Init the DSSNetActive class.
//===================================================================
AEEResult DSSNetActive::Init(ds::Net::IPolicy* pIDSNetPolicy,
                             dss_net_cb_fcn* pNetCb,
                             void* pNetUserData)
{
   INetworkFactory* pIDSNetworkFactory = NULL;
   INetwork* pIDSNetwork = NULL;
   INetworkPriv* pIDSNetworkPriv = NULL;
   INetworkExt* pIDSNetworkExt = NULL;
   INetworkExt2* pIDSNetworkExt2 = NULL;
   AEEResult res = AEE_SUCCESS;
   ISignalFactory *pSignalFactory = NULL;
   DSSGenScope scopeNetworkFactory;
   boolean wouldblockReturned = FALSE;

   res = DSSGlobals::Instance()->GetNetworkFactory(&pIDSNetworkFactory);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSNetActive::Init(): "
                      "GetNetworkFactory() failed: %d", res);
      goto bail;
   }
   scopeNetworkFactory.SetParams(pIDSNetworkFactory, DSSGenScope::IDSIQI_TYPE);

   // Create network and bring it up
   res = pIDSNetworkFactory->CreateNetwork(
      ds::Net::NetworkMode::QDS_ACTIVE,
      pIDSNetPolicy,
      &pIDSNetwork);
   if (AEE_SUCCESS != res && AEE_EWOULDBLOCK != res) {
      LOG_MSG_ERROR_1("DSSNetActive::Init(): "
                      "CreateNetwork() failed: %d", res);
      goto bail;
   }

   if (AEE_EWOULDBLOCK == res)
   {
      wouldblockReturned = TRUE;
   }

   // get signal factory
   res = DSSGlobals::Instance()->GetSignalFactory(&pSignalFactory);
   if (NULL == pSignalFactory || AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSNetActive::Init(): "
                      "Couldn't fetch pSignalFactory from DSSGlobals result = %d",
                      res);
      goto bail;
   }

   // create signal
   res = pSignalFactory->CreateSignal(
      &signalHandler,
      IfaceUpOrDownSignalFcn,
      this,
      &mpIfaceUpSignal,
      &mpIfaceUpSignalCtl);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSNetActive::Init(): "
                      "Couldn't create mpIfaceUpSignal result = %d", res);
       goto bail;
   }

   // create signal
   res = pSignalFactory->CreateSignal(
      &signalHandler,
      IfaceUpOrDownSignalFcn,
      this,
      &mpIfaceDownSignal,
      &mpIfaceDownSignalCtl);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSNetActive::Init(): "
                      "Couldn't create mpIfaceDownSignal result = %d", res);
       goto bail;
   }

   // init members
   mNetCb = *pNetCb;
   mpNetUserData = pNetUserData;

   // DSSNetActive takes care to AddRef pIDSNetwork once it is set
   SetIDSNetworkObject(pIDSNetwork);

   res = pIDSNetwork->QueryInterface(AEEIID_INetworkPriv, (void**)&pIDSNetworkPriv);
   if(AEE_SUCCESS != res){
      LOG_MSG_ERROR_1( "Couldn't QueryInterface for AEEIID_INetworkPriv result = %d",
                       res);
      goto bail;
   }

   // DSSNetActive takes care to AddRef pIDSNetworkPriv once it is set
   SetIDSNetworkPrivObject(pIDSNetworkPriv);

   res = pIDSNetwork->QueryInterface(AEEIID_INetworkExt, (void**)&pIDSNetworkExt);
   if(AEE_SUCCESS != res){
      LOG_MSG_ERROR_1( "Couldn't QueryInterface for AEEIID_INetworkExt result = %d",
                       res);
      goto bail;
   }

   // DSSNetActive takes care to AddRef pIDSNetworkExt once it is set
   SetIDSNetworkExtObject(pIDSNetworkExt);

   res = pIDSNetwork->QueryInterface(AEEIID_INetworkExt2, (void**)&pIDSNetworkExt2);
   if(AEE_SUCCESS != res){
      LOG_MSG_ERROR_1( "Couldn't QueryInterface for AEEIID_INetworkExt2 result = %d",
                       res);
      goto bail;
   }

   // DSSNetActive takes care to AddRef pIDSNetworkExt2 once it is set
   SetIDSNetworkExt2Object(pIDSNetworkExt2);

   // return EWOULDBLOCK in case it was the result from the bringup
   if (AEE_SUCCESS == res && TRUE == wouldblockReturned)
   {
      res = AEE_EWOULDBLOCK;
   }

/* fall through */

bail:

   DSSCommon::ReleaseIf((IQI**)&pIDSNetworkPriv);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetworkExt);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetworkExt2);
   DSSCommon::ReleaseIf((IQI**)&pSignalFactory);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);

   return res;
}

//===================================================================
//  FUNCTION:   DSSNetActive::RegAutoEvents
//
//  DESCRIPTION:
//  Registers object to the automatic events.
//===================================================================
AEEResult DSSNetActive::RegAutoEvents(ds::Net::INetworkPriv* pIDSNetworkPriv)
{
   IQI* pRegObj = NULL;
   AEEResult res = AEE_SUCCESS;

   // There are 2 events that every network is automatically registered to: iface up & iface down
   LOG_MSG_INFO1_1("DSSNetActive::RegAutoEvents(): "
                   "Registering to QDS_EV_STATE_CHANGED, NetworkPriv obj 0x%x",
                   pIDSNetworkPriv);

   // regObj mechanism is currently not in effect by dsnet layer.
   // No need for DSS to hold the allocated object to maintain the event registration.
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.

   res = pIDSNetworkPriv->OnStateChange(mpIfaceUpSignal, NetworkEvent::QDS_EV_IFACE_UP, &pRegObj);
   DSSCommon::ReleaseIf(&pRegObj);
   if (AEE_SUCCESS != res) {
     LOG_MSG_ERROR_1("DSSNetActive::RegAutoEvents(): "
                     "OnStateChange QDS_EV_IFACE_UP failed , res = 0x%x ",
                     res);
     return res;
   }

   res = pIDSNetworkPriv->OnStateChange(mpIfaceDownSignal, NetworkEvent::QDS_EV_IFACE_DOWN, &pRegObj);
   DSSCommon::ReleaseIf(&pRegObj);
   if (AEE_SUCCESS != res) {
     LOG_MSG_ERROR_1("DSSNetActive::RegAutoEvents(): "
                     "OnStateChange QDS_EV_IFACE_DOWN failed , res = 0x%x ",
                     res);
     return res;
   }

   return res;
}

//===================================================================
//  FUNCTION:   DSSNetActive::IfaceUpOrDownSignalFcn
//
//  DESCRIPTION:
//  Set Iface ID On Iface auto events
//===================================================================
void DSSNetActive::IfaceUpOrDownSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam)
{
  DSSNetActive *pNetActive = static_cast<DSSNetActive*>(pSignalHandlerBaseParam);
  DSSNetApp* pNetApp = NULL;
  DSSWeakRefScope WeakRefScope;
  dss_iface_ioctl_state_type state = IFACE_STATE_INVALID;
  sint15 dss_errno = DS_ENETNONET;
  bool invokeCback = false;
  dss_iface_id_type ifaceId = DSS_IFACE_INVALID_ID;
  dss_net_cb_fcn netCb = NULL;
  sint15 netHandle = -1;
  void *pUserData = NULL;
  boolean pppIsOpen = FALSE;
  DSSNetMonitored *pPrimaryNetMonitored = NULL;
  DSSNetMonitoredScope primaryNetMonitoredScope;
  AEEResult res = AEE_SUCCESS;

  LOG_MSG_FUNCTION_ENTRY_1("DSSNetActive::IfaceUpOrDownSignalFcn(): "
                           "NetApp 0x%x", pSignalHandlerBaseParam);

  if (NULL == pNetActive || NULL == pNetActive->mpIDSNetworkPriv) {
    LOG_MSG_ERROR_0("Invalid user data");
    return;
  }

  pNetActive->mpCritSect->Enter();

  // Store these before releasing the lock in order to safely use in CB.
  netCb = pNetActive->mNetCb;
  pUserData = pNetActive->mpNetUserData;

  // get NetApp and NetMonitored
  pNetActive->GetNetHandle(&netHandle);
  res = DSSGlobals::Instance()->GetNetApp(netHandle, &pNetApp);
  if (AEE_SUCCESS != res || NULL == pNetApp) {
    LOG_MSG_ERROR_1("NetApp is already removed by close_netlib call,"
                    "discarding the event for 0x%x", pNetActive);
    pNetActive->mpCritSect->Leave();
    return;
  }

  if (!WeakRefScope.Init(pNetApp)) {
    LOG_MSG_ERROR_1("Cannot GetStrongRef of NetApp,"
                    "discarding the event for 0x%x", pNetActive);
    pNetActive->mpCritSect->Leave();
    return;
  }

  (void)pNetApp->GetPrimaryNetMonitored(&pPrimaryNetMonitored);
  (void)primaryNetMonitoredScope.Init(pPrimaryNetMonitored);

  if (NULL != pNetActive->mpIfaceUpSignalCtl) {
    (void)(pNetActive->mpIfaceUpSignalCtl)->Enable();
  }

  if (NULL != pNetActive->mpIfaceDownSignalCtl) {
    (void)(pNetActive->mpIfaceDownSignalCtl)->Enable();
  }

  primaryNetMonitoredScope.Fetch()->GetPPPIsOpen(&pppIsOpen);
  if (FALSE == pppIsOpen) {
    pNetActive->mpCritSect->Leave();
    LOG_MSG_INFO1_0("DSSNetActive::IfaceUpOrDownSignalFcn(): "
                    "PPP open not called, do not post event");
    return;
  }

  pNetActive->GetIfaceState(&state);
  netHandle = pNetActive->mNetHandle;
  pNetActive->GetIfaceId(&ifaceId);
  if (IFACE_UP == state) {

    // Avoid sending multiple consecutive IFACE_UP events to the application.
    // Such may happen due to real time check in dsnet which sets the signal immediately
    // upon dss_pppopen, when DSS registers for those events. Possibly, until DSS gets
    // to check the iface state here above, that iface already went UP causing this
    // function to send event to app. However, when iface went UP it also set the signal
    // triggering another IFACE UP event that arrives to this function subsequently.
    if (IFACE_UP != pNetActive->mLastIfaceStateSentToApp) {
      dss_errno = DS_ENETISCONN;
      invokeCback = true;
      pNetActive->mLastIfaceStateSentToApp = IFACE_UP;
    }
    else {
      LOG_MSG_INFO1_2("DSSNetActive::IfaceUpOrDownSignalFcn(): "
                      "Avoid sending multiple consecutive IFACE_UP events to app, netHandle: %d, ifaceId: %u",
                      netHandle, ifaceId);
    }
  }
  else if (IFACE_DOWN == state) {
    // Avoid sending multiple consecutive IFACE_DOWN events to the application.
    if (IFACE_DOWN != pNetActive->mLastIfaceStateSentToApp) {
      dss_errno = DS_ENETNONET;
      invokeCback = true;
      pNetActive->mLastIfaceStateSentToApp = IFACE_DOWN;

      // UpdateNetDownReason should be called right before NetActive object is freed. 
      pNetApp->UpdateNetDownReason();

      pNetActive->RemovePrimaryQoSModifyStatusHandler();
      PS_MEM_DELETE(pNetActive->mpDSSNetQoSDefault);
      DSSCommon::ReleaseIf((IQI**)&(pNetActive->mpDSNetQoSManager));

        pNetApp->FreeNetActive();
        primaryNetMonitoredScope.Fetch()->SetPPPIsOpen(FALSE);
      }
      else {
      LOG_MSG_ERROR_2("DSSNetActive::IfaceUpOrDownSignalFcn(): "
                      "Multiple consecutive IFACE_DOWN events occurred in DSSNetActive, netHandle: %d, ifaceId: %u",
                      netHandle, ifaceId);
    }
  }

  // If app provided network events CB function invoke it.
  // Leave the critical section before calling APP callback to avoid the following race condition:
  // 1. Callback is invoked for IFACE_DOWN (DS_ENETNONET) event.
  // 2. App calls dss_close_netlib() which ends up releasing the pPrimaryNetMonitored object.
  // 3. Control returns to this function and we try to access released critical section.
  pNetActive->mpCritSect->Leave();

  if (true == invokeCback && NULL != netCb) {
    LOG_MSG_INFO1_3("DSSNetActive::IfaceUpOrDownSignalFcn(): "
                    "DSS called App callback, netHandle: %d, ifaceId: %u, state=%d",
                    netHandle, ifaceId, state);
    (void) netCb (
      netHandle,
      ifaceId,
      dss_errno,
      pUserData);
    LOG_MSG_INFO1_3("DSSNetActive::IfaceUpOrDownSignalFcn(): "
                    "DSS returned from App callback, netHandle: %d, ifaceId: %u, state=%d",
                    netHandle, ifaceId, state);
  }
}

//===================================================================
//  FUNCTION:   DSSNetActive::GetIfaceState
//
//  DESCRIPTION:
//  Gets iface state 
//===================================================================
AEEResult DSSNetActive::GetIfaceState(dss_iface_ioctl_state_type* pIfaceState)
{
   NetworkStateType netState = NetworkState::QDS_CLOSED;
   AEEResult res = AEE_SUCCESS;
   ds::Net::INetwork* pTmpIDSNetwork = NULL;

   mpCritSect->Enter();

   if (NULL == mpIDSNetwork) {
      mpCritSect->Leave();
      return QDS_EINVAL;
   }

   pTmpIDSNetwork = mpIDSNetwork;

   mpCritSect->Leave();
   IDS_ERR_RET(pTmpIDSNetwork->GetState(&netState));
   mpCritSect->Enter();
   
   res = DSSConversion::IDS2DSIfaceState(netState, pIfaceState);

   mpCritSect->Leave();
   return res;
}

//===================================================================
//  FUNCTION:   DSSNetActive::GetNetworkState
//
//  DESCRIPTION:
//  Gets network state
//===================================================================
AEEResult DSSNetActive::GetNetworkState(NetworkStateType* pNetState)
{
   NetworkStateType netState = NetworkState::QDS_CLOSED;
   ds::Net::INetwork* pTmpIDSNetwork = NULL;

   mpCritSect->Enter();

   if (NULL == mpIDSNetwork) {
      mpCritSect->Leave();
      return QDS_EINVAL;
   }

   pTmpIDSNetwork = mpIDSNetwork;

   mpCritSect->Leave();
   IDS_ERR_RET(pTmpIDSNetwork->GetState(&netState));
   mpCritSect->Enter();

   *pNetState = netState;

   mpCritSect->Leave();
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetActive::RegEventCBPrimary
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_REG_EVENT_CB
//===================================================================
AEEResult DSSNetActive::RegEventCBPrimary(dss_iface_ioctl_event_enum_type event,
                                          dss_iface_ioctl_event_cb event_cb,
                                          void* user_data_ptr,
                                          dss_iface_id_type ifaceId)
{
   DSSEventHandler* pEventHandler = NULL;
   sint15 netHandle = -1;

   if (DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV == event || DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV == event) {
      IDS_ERR_RET(FetchHandler(&mpPrimaryQoSModifyStatusHandler, &pEventHandler, true));
   } else {
      LOG_MSG_ERROR_1("DSSNetActive::RegEventCBPrimary(): "
                      "RegEventCBPrimary: trying to register for unknown event %d",
                      event);
      ASSERT(0);
      return QDS_INTERNAL;
   }

   // Register to the event.
   AEEResult res = (pEventHandler->Register(
      event,
      event_cb,
      user_data_ptr,
      ifaceId));
   if (AEE_SUCCESS != res) {
      GetNetHandle(&netHandle);
      if (QDS_EINPROGRESS == res) {
         LOG_MSG_ERROR_2("DSSNetActive::RegEventCBPrimary(): "
                         "App %d already has registered for event %d",
                         netHandle, event);
      } else {
      LOG_MSG_ERROR_2("DSSNetActive::RegEventCBPrimary(): "
                      "Error while registering app %d for event %d",
                      netHandle, event);
      }
      return res;
   }

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetActive::FetchHandler
//
//  DESCRIPTION: returns event handler and creates one,
//  if it it does not exist
//===================================================================
template<typename HandlerType>
AEEResult DSSNetActive::FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit)
{
   if (bInit) {
      if (NULL == *pHandler) {
         *pHandler = HandlerType::CreateInstance();
         if (NULL == *pHandler) {
            return AEE_ENOMEMORY;
         }
         IDS_ERR_RET((*pHandler)->Init(this));
      }
   } else {
      // bInit is false , we expect pHandler to have a value here
      if (0 == *pHandler) {
         return QDS_EFAULT;
      }
   }

   *ppEventHandler = *pHandler;
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetActive::RemovePrimaryQoSModifyStatusHandler
//
//  DESCRIPTION:
//  Releases mpPrimaryQoSModifyStatusHandler object.
//===================================================================
void DSSNetActive::RemovePrimaryQoSModifyStatusHandler()
{
   PS_MEM_RELEASE(mpPrimaryQoSModifyStatusHandler);
}

//===================================================================
//  FUNCTION:   DSSNetActive::Destructor
//
//  DESCRIPTION:
//  Destructor of the DSSNetActive class.
//===================================================================
/*lint -e{1551} */
void DSSNetActive::Destructor() throw()
{
   PS_MEM_DELETE(mpDSSNetQoSDefault);
   PS_MEM_RELEASE(mpPrimaryQoSModifyStatusHandler);

   DSSCommon::ReleaseIf((IQI**)&mpDSNetQoSManager);
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetworkPriv);
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetwork);
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetworkExt);
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetworkExt2);
#ifdef FEATUTE_DATA_PS_MCAST
   DSSCommon::ReleaseIf((IQI**)&mpDSNetMCastManager);
#endif // FEATUTE_DATA_PS_MCAST
   DSSCommon::ReleaseIf((IQI**)&mpDSNetFirewallManager);
   DSSCommon::ReleaseIf((IQI**)&mpIfaceUpSignal);
   DSSCommon::ReleaseIf((IQI**)&mpIfaceDownSignal);
   DSSCommon::ReleaseIf((IQI**)&mpIfaceUpSignalCtl);
   DSSCommon::ReleaseIf((IQI**)&mpIfaceDownSignalCtl);

   // needs to be before mpCritSect releasing, because FreeLists() is using it
   FreeLists();

   DSSCommon::ReleaseIf((IQI**)&mpCritSect);
   SignalHandlerBase::Destructor();
}
/*lint –restore */
AEEResult DSSNetActive::GetNetQoSManager(IQoSManager** ppNetQosManager)
{
   ds::Net::IQoSManager* pDSNetQoSManager = NULL;
   if(NULL == mpDSNetQoSManager) {

      // Create a new NetQoSManager since pNetActive does not have
      // a NetQoSManager instance.
      IDS_ERR_RET(mpIDSNetworkExt->CreateQoSManager(&pDSNetQoSManager));

   }

   DSSCritScope cs(*mpCritSect);
   if(NULL == mpDSNetQoSManager) {
     mpDSNetQoSManager = pDSNetQoSManager;
   }
   else{
     DSSCommon::ReleaseIf((IQI**)&pDSNetQoSManager);
   }
   *ppNetQosManager = mpDSNetQoSManager;
   (void) mpDSNetQoSManager->AddRef();
 
   return AEE_SUCCESS;
}

AEEResult DSSNetActive::SetNetQoSManager(IQoSManager* pNetQosManager)
{
   DSSCritScope cs(*mpCritSect);

   mpDSNetQoSManager = pNetQosManager;
   (void)mpDSNetQoSManager->AddRef();

   return AEE_SUCCESS;
}

AEEResult DSSNetActive::SetQoSDefault(IQoS* pNetQoSDefault)
{
   DSSCritScope cs(*mpCritSect);

   if (NULL != mpDSSNetQoSDefault) {
      // do nothing if QosDefault already exists
      return AEE_SUCCESS;
   }
   // AddRef is done inside
   mpDSSNetQoSDefault = PS_MEM_NEW(DSSNetQoSDefault(pNetQoSDefault));
   if (NULL == mpDSSNetQoSDefault) {
      LOG_MSG_ERROR_0( "Couldn't allocate DSSNetQoSDefault");
      return AEE_ENOMEMORY;
   }

   return AEE_SUCCESS;
}

AEEResult DSSNetActive::GetQoSDefault(IQoS** ppNetQoSDefault)
{
   if(NULL == mpDSSNetQoSDefault) {

      // Create a new DSSNetQosDefault since pNetActive does not have
      // a DSSNetQosDefault instance.
      IQoS*        pNetQoSDefault = NULL;
      IQoSManager* pNetQoSManager = NULL;

      IDS_ERR_RET(GetNetQoSManager(&pNetQoSManager));
      DSSGenScope scopeNetQosManager(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

      // Get the primary QoS
      IDS_ERR_RET(pNetQoSManager->GetQosDefault(&pNetQoSDefault));
      DSSGenScope scopeNetQosDefault(pNetQoSDefault,DSSGenScope::IDSIQI_TYPE);

      // Set the primary QoS in DSSNetActive (set the mpDSSNetQoSDefault)
      IDS_ERR_RET(SetQoSDefault(pNetQoSDefault));
   }

   return mpDSSNetQoSDefault->GetNetQoSDefault(ppNetQoSDefault);
}

AEEResult DSSNetActive::GetPhysLinkObject(uint32 ifaceid, IPhysLink** ppPhyslink)
{
   FlowID flowid                        = 0;
   DSSNetQoSSecondary *pDSSQosSecondary = NULL;
   IQoSSecondary      *pIDSQosSecondary = NULL;
   AEEResult res                        = AEE_SUCCESS;
   DSSIDSNetworkScope IDSNetworkScope;
   AEEResult nRes                       = AEE_SUCCESS;
   DSSQoSNetInitiated* pDSSQoSNetInitiated;

   flowid = DSSIfaceId::StripDownToFlowID(ifaceid);

   if (0 == flowid) { // No flow id defined
      res = QDS_EBADF;
      goto bail;
   }

   // Flow id is real - extract corresponding qos obj
   // Get the DSSNetQoSSecondary or DSSQoSNetInitiated object according to the provided QoSHandle.
   nRes = GetDSSNetQoSSecondary(ifaceid, &pDSSQosSecondary);
   if ( (AEE_SUCCESS != nRes) ||  (NULL == pDSSQosSecondary) ) {
      // No QoSSecondary object corresponding to provided iface id was found
      // Search if there is a corresponding QoSNetInitiated object
      nRes = GetDSSQoSNetInitiated(flowid, &pDSSQoSNetInitiated);
      if ( (AEE_SUCCESS != nRes) ||  (NULL == pDSSQoSNetInitiated) ) {
         // Could not find corresponding QoSSecondary nor QoSNetInitiated objects
         res = QDS_EFAULT;
         goto bail;
      }
      res = pDSSQoSNetInitiated->GetPhysLinkObject(ppPhyslink);
      goto bail;
   }

   // creating new block to prevent error C2362 (DSSGenScope)
   {
      res = pDSSQosSecondary->GetNetQoSSecondary(&pIDSQosSecondary);
      DSSGenScope GenScopeObject(pIDSQosSecondary, DSSGenScope::IDSIQI_TYPE);
      if (AEE_SUCCESS != res) {
         goto bail;
      }

      res = pIDSQosSecondary->GetTXPhysLink(ppPhyslink);
      if (AEE_SUCCESS != res) {
         goto bail;
      }
   }


bail:
   if (AEE_SUCCESS != res) {
      *ppPhyslink = NULL;
   }
   
   return res;
}

AEEResult DSSNetActive::AddNetQoSSecondary(IQoSSecondary* pNetQoSSecondary,
                                           uint32 flowID,
                                           dss_iface_ioctl_event_cb cback_fn,
                                           void* user_data)
{
  dss_iface_ioctl_ev_cb_type cbType;
  AEEResult res;
  AEEResult localRes;

  // allocate a new DSSQoS object and store the new pNetQoSSecondary object in it
  // mpNetQoSSecondary->AddRef() is done inside
  DSSNetQoSSecondary* pDSSNetQoSSecondary = PS_MEM_NEW(DSSNetQoSSecondary(pNetQoSSecondary, flowID));
  if (NULL == pDSSNetQoSSecondary) {
    LOG_MSG_ERROR_0("DSSNetActive::AddNetQoSSecondary(): "
                    "Couldn't allocate DSSNetQoSSecondary");
    return AEE_ENOMEMORY;
  }

  cbType.event_cb = cback_fn;
  cbType.user_data_ptr = user_data;

  // put the new DSSQoS at the beginning of the DSSQoS list.
  pDSSNetQoSSecondary->SetDSSNetActive(this);
  pDSSNetQoSSecondary->InsertToList(mpDSSQoSSecondaryList);
  mpDSSQoSSecondaryList = pDSSNetQoSSecondary;

  // registering DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV will cause to register also:
  // DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV
  cbType.event = DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV;
  // flowId is the iface ID
  res = pDSSNetQoSSecondary->RegEventCB(&cbType, flowID);
  if (AEE_SUCCESS != res) {
    goto bail;
  }

  cbType.event = DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV;
  // flowId is the iface ID
  res = pDSSNetQoSSecondary->RegEventCB(&cbType, flowID);
  if (AEE_SUCCESS != res) {
    goto bail;
  }

  // registering DSSIFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV will cause to register also:
  // DSSIFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV
  // DSSIFACE_IOCTL_QOS_UNAVAILABLE_EV
  cbType.event = DSS_IFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV;
  // flowId is the iface ID
  res = pDSSNetQoSSecondary->RegEventCB(&cbType, flowID);
  if (AEE_SUCCESS != res) {
    goto bail;
  }


  return AEE_SUCCESS;

bail:
  localRes = RemoveDSSNetQoSSecondary(flowID);
  if (AEE_SUCCESS != localRes) {
    LOG_MSG_ERROR_1("DSSNetActive::AddNetQoSSecondary(): "
                    "RemoveDSSNetQoSSecondary() failed: %d", res);
    // The objects are released as part of RemoveDSSNetQoSSecondary
    // If the above fails, it means that the object hasn't been found
    // in the list (and not released), and needs to be released here.
    PS_MEM_RELEASE(pDSSNetQoSSecondary);
  }

  return res;
}

AEEResult DSSNetActive::AddQoSNetInitiatedReq(
  IQoSNetInitiatedReq*                  pQoSNetInitiatedReq,
  dss_qos_net_initiated_req_handle_type qosNetInitiatedReqHandle,
  dss_iface_ioctl_event_cb              cback_fn,
  void*                                 user_data,
  dss_qos_handle_type*                  q_handle,
  dss_iface_id_type                     ifaceId,
  boolean                               real_flag
)
{
  dss_iface_ioctl_ev_cb_type cbType;
  AEEResult res;
  dss_qos_handle_type qos_handle = 0;

  // allocate a new DSSQoSNetInitiatedReq object and store the new
  // pQoSNetInitiatedReq object in it. pQoSNetInitiatedReq->AddRef() is done inside.
  DSSQoSNetInitiatedReq* pDSSQoSNetInitiatedReq = 
    PS_MEM_NEW(DSSQoSNetInitiatedReq(pQoSNetInitiatedReq, qosNetInitiatedReqHandle, ifaceId));
  if (NULL == pDSSQoSNetInitiatedReq) {
    LOG_MSG_ERROR_0("DSSNetActive::AddQoSNetInitiatedReq(): "
                    "Couldn't allocate DSSQoSNetInitiatedReq");
    return AEE_ENOMEMORY;
  }

  pDSSQoSNetInitiatedReq->SetDSSNetActive(this);
  pDSSQoSNetInitiatedReq->SetHandle(qosNetInitiatedReqHandle);  // DSS identifier of the request
  pDSSQoSNetInitiatedReq->SetNetHandle(mNetHandle);

  // Register for NIQ event
  cbType.app_id = mNetHandle;
  cbType.event_cb = cback_fn;
  cbType.user_data_ptr = user_data;
   
  if(TRUE == real_flag)
  {
     uint8  flowID;
     dss_iface_ioctl_event_info_union_type eventInfo;

	 LOG_MSG_INFO1_0("DSSNetActive::AddQoSNetInitiatedReq(): QoS Already Established");

	 res = pDSSQoSNetInitiatedReq->RegCB(&cbType);
	 if (AEE_SUCCESS != res) {
	   goto bail;
	 }
	 while (pQoSNetInitiatedReq->IsPendingMatchingFlow()) {
	   ds::ErrorType res = DSSGlobals::Instance()->GetFreeQoSFlowID(&flowID);
           if (AEE_SUCCESS != res) {
             LOG_MSG_ERROR_1("DSSNetActive::AddQoSNetInitiatedReq(): "
                             "Failed to GetFreeQoSFlowID. Req Object: 0x%p",
                              pDSSQoSNetInitiatedReq);
             res = DSS_ERROR;
             goto bail;
           }

           qos_handle = ifaceId | flowID;
           ds::ErrorType nRes4 = pDSSQoSNetInitiatedReq->CreateDSSQoSNetInitiated(qos_handle);
           if (AEE_SUCCESS != nRes4) {
             if (AEE_EALREADY == nRes4) {
               LOG_MSG_INFO1_1("DSSNetActive::AddQoSNetInitiatedReq(): "
                               "DSSQoSNetInitiated already exist. Req Object: 0x%p",
                                pDSSQoSNetInitiatedReq);
               DSSGlobals::Instance()->ReleaseQoSFlowID((uint8)(flowID));
               return DSS_SUCCESS;
             }
             else {
               LOG_MSG_ERROR_1("DSSNetActive::AddQoSNetInitiatedReq(): "
                               "Failed to create DSSQoSNetInitiated. Req Object: 0x%p",
                               pDSSQoSNetInitiatedReq);
               DSSGlobals::Instance()->ReleaseQoSNetInitiatedReqHandle(qos_handle);
               DSSGlobals::Instance()->ReleaseQoSFlowID((uint8)(flowID));
               res = DSS_ERROR;
               goto bail;
             }
         }
         memset(&eventInfo,0,sizeof(dss_iface_ioctl_event_info_union_type));
         pDSSQoSNetInitiatedReq->GetHandle(
           &eventInfo.qos_net_initiated_available_info.handle);
         eventInfo.qos_net_initiated_available_info.qos_handle = qos_handle;
         LOG_MSG_INFO1_1("DSSNetActive::AddQoSNetInitiatedReq(): qos_handle %d", qos_handle);
         /* Call App call back */
         cback_fn(DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV,eventInfo,user_data,mNetHandle,ifaceId);
      }
  }

  // Register for NIQ event	
  cbType.event = DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV;
  res = pDSSQoSNetInitiatedReq->RegEventCB(&cbType, ifaceId);
  if (AEE_SUCCESS != res) {
    goto bail;
  }

  // put the new DSSQoSNetInitiated at the beginning of the DSSQoSNetQosInitiatedReq list.   
  pDSSQoSNetInitiatedReq->InsertToList(mpDSSQoSNetInitiatedReqList);
  mpDSSQoSNetInitiatedReqList = pDSSQoSNetInitiatedReq;
  (*q_handle) = qos_handle;
  return AEE_SUCCESS;

bail:
  PS_MEM_RELEASE(pDSSQoSNetInitiatedReq);
  return res;
}

AEEResult DSSNetActive::AddQoSNetInitiated(
                                        DSSQoSNetInitiatedReq*  pDSSQoSNetInitiatedReq,
                                        dss_qos_handle_type     qos_handle,
                                        DSSQoSNetInitiated**    ppDSSQoSNetInitiated
                                        )
{
  IQoSManager* pNetQoSManager = NULL;
  IQoSManagerPriv* pNetQoSManagerPriv = NULL;
  IQoSNetInitiated* pQoSNetInitiated = NULL;
  IQoSNetInitiatedReq* pQoSNetInitiatedReq = NULL;
  AEEResult res = AEE_SUCCESS;      
  dss_iface_id_type ifaceId = DSS_IFACE_INVALID_ID;

  *ppDSSQoSNetInitiated = NULL;

  IDS_ERR_RET(GetNetQoSManager(&pNetQoSManager));
  DSSGenScope scopeNetQosManager(pNetQoSManager,DSSGenScope::IDSIQI_TYPE);

  IDS_ERR_RET(pNetQoSManager->QueryInterface(AEEIID_IQoSManagerPriv, (void**)&pNetQoSManagerPriv));
  DSSGenScope scopeNetQosManagerPriv(pNetQoSManagerPriv,DSSGenScope::IDSIQI_TYPE);

  // Create the QoSNetInitiated object that shall be used to set the QoS specification on the iface.
  IDS_ERR_RET(pDSSQoSNetInitiatedReq->GetQoSNetInitiatedReq(&pQoSNetInitiatedReq));
  DSSGenScope scopeQoSNetInitiatedReq(pQoSNetInitiatedReq,DSSGenScope::IDSIQI_TYPE);

  res = pNetQoSManagerPriv->CreateQoSNetInitiated(pQoSNetInitiatedReq,
    &pQoSNetInitiated);
  if (AEE_SUCCESS != res) {
    // TODO: cannot use bail here because of the scope (compilation error).
    // Change DSSGenScope to use Init() and not Ctor, so we can move it to the beginning of the function.
    return res;
  }
  DSSGenScope scopeQoSNetInitiated(pQoSNetInitiated,DSSGenScope::IDSIQI_TYPE);

  // Check that the newly created QosNetInitiated object does not already exist.
  // This can happen because of a race condition between flow notification when a new flow created
  // and real time check.
  if (TRUE == QoSNetInitiatedExists(pQoSNetInitiated, pDSSQoSNetInitiatedReq))
  {
    LOG_MSG_INFO1_1("DSSNetActive::AddQoSNetInitiated(): "
                    "QoSNetInitiated obj 0x%x already exist, not notifying application again",
                    pQoSNetInitiated);
    res = AEE_EALREADY;
    goto bail;
  }

  // Create a new DSSQoSNetInitiated object and store the new IQoSNetInitiated object in it
  // piQoSNetInitiated->AddRef() is done inside
  this->GetIfaceId(&ifaceId);
  *ppDSSQoSNetInitiated = PS_MEM_NEW(DSSQoSNetInitiated(pQoSNetInitiated, qos_handle, ifaceId, (uint32)pDSSQoSNetInitiatedReq));
  if (NULL == *ppDSSQoSNetInitiated) {
    LOG_MSG_ERROR_0("DSSNetActive::AddQoSNetInitiated(): "
                    "Couldn't allocate DSSQoSNetInitiated");
    res = AEE_ENOMEMORY;
    goto bail;
  }

  // Parent of QoSNetInitiated is this DSSNetActive object (same parent as the 
  // corresponding DSSQoSNetInitiatedReq object
  (*ppDSSQoSNetInitiated)->SetDSSNetActive(this);

  // put the new DSSQoSNetInitiated at the beginning of the DSSQoSNetQosInitiatedReq list.   
  {
     // using the critical section to guard access to mpDSSQoSNetInitiatedList
     DSSCritScope lockDSSQoSNetInitiatedList(*mpCritSect);
     (*ppDSSQoSNetInitiated)->InsertToList(mpDSSQoSNetInitiatedList);
     mpDSSQoSNetInitiatedList = *ppDSSQoSNetInitiated;

     (*ppDSSQoSNetInitiated)->SetNetHandle(mNetHandle);
  }

  return AEE_SUCCESS;

bail:
  if (*ppDSSQoSNetInitiated) {
    PS_MEM_RELEASE(*ppDSSQoSNetInitiated);
  }
  *ppDSSQoSNetInitiated = NULL;
  return res;
}

AEEResult DSSNetActive::AddIpv6PrivAddr(IIPv6Address* pNetIpv6Address,
                                     uint32 flowID,
                                     boolean isUnique,
                                     dss_iface_ioctl_event_cb cback_fn,
                                     void* user_data)
{

   dss_iface_ioctl_ev_cb_type cbType;
   AEEResult res;

   // allocate a new DSS Private IPv6 address object and store the new IIPv6Address object in it
   // AddRef() is inside
   DSSPrivIpv6Addr* pDSSPrivIpv6Addr = PS_MEM_NEW(DSSPrivIpv6Addr(pNetIpv6Address,flowID,isUnique));
   if (NULL == pDSSPrivIpv6Addr) {
      LOG_MSG_ERROR_0("DSSNetActive::AddIpv6PrivAddr(): "
                      "Couldn't allocate pDSSPrivIpv6Addr");
      return AEE_ENOMEMORY;
   }

   cbType.event_cb = cback_fn;
   cbType.user_data_ptr = user_data;

   // registering DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV will cause to register also:
   // DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV
   // DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV
   cbType.event = DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV;
   res = pDSSPrivIpv6Addr->RegEventCB(&cbType, flowID);
   if (AEE_SUCCESS != res) {
      goto bail;
   }

   // put the new IPv6 private address in the IPv6 private addresses list.
   pDSSPrivIpv6Addr->InsertToList(mpDSSPrivIpv6AddrList);
   pDSSPrivIpv6Addr->SetDSSNetActive(this);
   mpDSSPrivIpv6AddrList = pDSSPrivIpv6Addr;

   return AEE_SUCCESS;

bail:
   PS_MEM_RELEASE(pDSSPrivIpv6Addr);
   return res;

}

#ifdef FEATUTE_DATA_PS_MCAST
AEEResult DSSNetActive::AddDSSMCast(IMCastSessionPriv* pMCastSession,
                                    uint32 MCastHandle,
                                    dss_iface_ioctl_event_cb event_cb,
                                    void *user_data_ptr)
{
   // allocate a new DSSMCast object and store the new pMCastSession object in it
   // AddRef is done inside
   DSSMCast* pDSSMCast = PS_MEM_NEW(DSSMCast(pMCastSession, MCastHandle));
   if (NULL == pDSSMCast) {
      LOG_MSG_ERROR_0("DSSNetActive::AddDSSMCast(): "
                      "Couldn't allocate DSSMCast");
      return AEE_ENOMEMORY;
   }

   AEEResult res = pDSSMCast->RegEventCB(event_cb,
                                         user_data_ptr,
                                         DSS_IFACE_IOCTL_MCAST_REGISTER_SUCCESS_EV,
                                         MCastHandle);
   if(AEE_SUCCESS != res) {
      goto bail;
   }

   res = pDSSMCast->RegEventCB(event_cb,
                               user_data_ptr,
                               DSS_IFACE_IOCTL_MCAST_STATUS_EV,
                               MCastHandle);
   if(AEE_SUCCESS != res) {
      goto bail;
   }

   // put the new DSSMCast at the beginning of the DSSMCast list.
   pDSSMCast->InsertToList(mpDSSMCastList);
   pDSSMCast->SetDSSNetActive(this);
   mpDSSMCastList = pDSSMCast;

   return AEE_SUCCESS;
bail:

   PS_MEM_RELEASE(pDSSMCast);
   return res;
}
#endif // FEATUTE_DATA_PS_MCAST
AEEResult DSSNetActive::AddDSSFirewallRule(IFirewallRule* pIFirewallRule, uint32 *pHandle)
{
   int index;
   static uint32 firewallHandle = 1;
   DSSCritScope cs(*mpCritSect);

   ASSERT (NULL != pIFirewallRule);
   ASSERT (NULL != pHandle);

   for (index = 0; index < DSS_MAX_FIREWALL_HANDLES; index++) {
      
      if (0 == DSSFirewallHandleToObject[index].handle) {

         DSSFirewallHandleToObject[index].handle = firewallHandle;
         DSSFirewallHandleToObject[index].pIFirewallRule = pIFirewallRule;
         (void)DSSFirewallHandleToObject[index].pIFirewallRule->AddRef();
         *pHandle = firewallHandle;
         break;
      }
   }

   firewallHandle++;

   if (index == DSS_MAX_FIREWALL_HANDLES) {
      return AEE_ENOMEMORY;
   }

   return AEE_SUCCESS;
}


AEEResult DSSNetActive::GetDSSFirewallRule(uint32 firewallHandle, IFirewallRule** ppIFirewallRule)
{
   int index;
   DSSCritScope cs(*mpCritSect);

   ASSERT (NULL != ppIFirewallRule);
   *ppIFirewallRule = 0;

   for (index = 0; index < DSS_MAX_FIREWALL_HANDLES; index++) {

      if (firewallHandle == DSSFirewallHandleToObject[index].handle) {
         *ppIFirewallRule = DSSFirewallHandleToObject[index].pIFirewallRule;
         (void)DSSFirewallHandleToObject[index].pIFirewallRule->AddRef();
         break;
      }
   }

   if (index == DSS_MAX_FIREWALL_HANDLES) {
      return QDS_EBADF;
   }

   return AEE_SUCCESS;
}

AEEResult DSSNetActive::DeleteDSSFirewallRule(uint32 firewallHandle)
{
  int index = 0;
  ds::Net::IFirewallRule * pIFirewallRuleTemp = NULL;

  mpCritSect->Enter();
  for (index = 0; index < DSS_MAX_FIREWALL_HANDLES; index++) {
    if (firewallHandle == DSSFirewallHandleToObject[index].handle) {
      DSSFirewallHandleToObject[index].handle = 0;
      pIFirewallRuleTemp = DSSFirewallHandleToObject[index].pIFirewallRule;
      DSSFirewallHandleToObject[index].pIFirewallRule = NULL;
      // leave DSS CS before calling ds_Net functions which can lock PS CS
      mpCritSect->Leave();
      DSSCommon::ReleaseIf ((IQI **) (&pIFirewallRuleTemp));
      return AEE_SUCCESS;
    }
  }

  if (index == DSS_MAX_FIREWALL_HANDLES) {
    mpCritSect->Leave();
    return QDS_EBADF;
  }

  mpCritSect->Leave();
  return AEE_SUCCESS;
}

AEEResult DSSNetActive::FlushDSSFirewallRuleTable(void)
{
  int index;
  IFirewallRule** apIFirewallRule = NULL;

  PS_SYSTEM_HEAP_MEM_ALLOC(apIFirewallRule, 
                           sizeof(IFirewallRule**) * DSS_MAX_FIREWALL_HANDLES,
                           IFirewallRule**);
  if (NULL == apIFirewallRule) {
     return AEE_ENOMEMORY;
  }

  // Copy all IFirewallRule to helper array, and nullify 

  mpCritSect->Enter();

  for (index = 0; index < DSS_MAX_FIREWALL_HANDLES; index++) {
      apIFirewallRule[index] = DSSFirewallHandleToObject[index].pIFirewallRule;
      DSSFirewallHandleToObject[index].handle = 0;
      DSSFirewallHandleToObject[index].pIFirewallRule = NULL;
      
  }

  mpCritSect->Leave();

  for (index = 0; index < DSS_MAX_FIREWALL_HANDLES; index++) {
     DSSCommon::ReleaseIf ((IQI **) (&apIFirewallRule[index]));
  }

  PS_SYSTEM_HEAP_MEM_FREE(apIFirewallRule);
  return AEE_SUCCESS;
}

#ifdef FEATUTE_DATA_PS_MCAST
AEEResult DSSNetActive::AddDSSMCastMBMSCtrl(IMCastMBMSCtrlPriv* pMCastMBMSCtrl,
                                            uint32 MCastMBMSHandle,
                                            dss_iface_ioctl_event_cb event_cb,
                                            void *user_data_ptr,
                                            dss_iface_id_type ifaceId)
{
   // allocate a new DSSMCast object and store the new pMCastSession object in it
   // AddRef is done inside
   DSSNetMCastMBMSCtrl* pDSSNetMCastMBMSCtrl = PS_MEM_NEW(DSSNetMCastMBMSCtrl(pMCastMBMSCtrl, MCastMBMSHandle));
   if (NULL == pDSSNetMCastMBMSCtrl) {
      LOG_MSG_ERROR_0("DSSNetActive::AddDSSMCastMBMSCtrl(): "
                      "Couldn't allocate DSSNetMCastMBMSCtrl");
      return AEE_ENOMEMORY;
   }


   AEEResult res = pDSSNetMCastMBMSCtrl->RegEventCB(event_cb, user_data_ptr, ifaceId);
   if(AEE_SUCCESS != res) {
      goto bail;
   }

   // put the new DSSNetMCastMBMSCtrl at the beginning of the DSSNetMCastMBMSCtrl list.
   pDSSNetMCastMBMSCtrl->InsertToList(mpDSSNetMCastMBMSList);
   pDSSNetMCastMBMSCtrl->SetDSSNetActive(this);
   mpDSSNetMCastMBMSList = pDSSNetMCastMBMSCtrl;

   return AEE_SUCCESS;

bail:
   PS_MEM_RELEASE(pDSSNetMCastMBMSCtrl);
   return res;
}

AEEResult DSSNetActive::GetMCastMBMSCtrl(uint32 iface_id, IMCastMBMSCtrlPriv** ppDSSMCastMBMSCtrl)
{
   DSSNetMCastMBMSCtrl* pDSSMCastMBMS;
   pDSSMCastMBMS = mpDSSNetMCastMBMSList;
   uint32 tmpMCastMBMSHandle = 0;

   while(NULL != pDSSMCastMBMS)
   {
      // get the Flow id of the current node in the DSSMCast list
      pDSSMCastMBMS->GetMCastMBMSHandle(&tmpMCastMBMSHandle);

      // if the flow ID matches the QoSHandle the used provided then we found the
      // DSSMCast
      if (tmpMCastMBMSHandle == iface_id)
      {
         IDS_ERR_RET(pDSSMCastMBMS->GetMBMSCtrl(ppDSSMCastMBMSCtrl));
         return AEE_SUCCESS;
      }

      IDS_ERR_RET(pDSSMCastMBMS->GetNext(&pDSSMCastMBMS));
   }

   // We haven't found any DSSMCast that matches the provided QoS Handle
   *ppDSSMCastMBMSCtrl = NULL;
   return QDS_EFAULT;
}

AEEResult DSSNetActive::GetDSSMCast(uint32 iface_id, DSSMCast** ppDSSMCast)
{
   DSSMCast* pDSSMCast = mpDSSMCastList;
   uint32 tmpMCastHandle = 0;

   while(NULL != pDSSMCast)
   {
      // get the Flow id of the current node in the DSSMCast list
      pDSSMCast->GetMCastHandle(&tmpMCastHandle);

      // if the flow ID matches the QoSHandle the used provided then we found the
      // DSSMCast
      if (tmpMCastHandle == iface_id)
      {
         *ppDSSMCast = pDSSMCast;
         return AEE_SUCCESS;
      }

      IDS_ERR_RET(pDSSMCast->GetNext(&pDSSMCast));
   }

   // We haven't found any DSSMCast that matches the provided QoS Handle
   *ppDSSMCast = NULL;
   return QDS_EBADF;
}
#endif // FEATUTE_DATA_PS_MCAST

AEEResult DSSNetActive::GetDSSNetQoSSecondary(uint32 iface_id, DSSNetQoSSecondary** ppDSSNetQoSSecondary)
{
   DSSNetQoSSecondary* pDSSNetQoSSecondary = mpDSSQoSSecondaryList;
   uint32 tmpFlowID = 0;

   while(NULL != pDSSNetQoSSecondary)
   {
      // get the Flow id of the current node in the DSSNetQoSSecondary list
      pDSSNetQoSSecondary->GetFlowID(&tmpFlowID);

      // if the flow ID matches the QoSHandle the used provided then we found the
      // DSSNetQoSSecondary
      if (tmpFlowID == iface_id)
      {
         *ppDSSNetQoSSecondary = pDSSNetQoSSecondary;
         return AEE_SUCCESS;
      }

      IDS_ERR_RET(pDSSNetQoSSecondary->GetNext(&pDSSNetQoSSecondary));
   }

   // We haven't found any DSSNetQoSSecondary that matches the provided QoS Handle
   *ppDSSNetQoSSecondary = NULL;
   // The error value here sould be QDS_EBADAPP but to be backward compatible
   // with the legacy API we return QDS_EBADF
   return QDS_EBADF;
}

AEEResult DSSNetActive::GetDSSQoSNetInitiated(dss_qos_handle_type qos_handle, DSSQoSNetInitiated** ppDSSQoSNetInitiated)
{
  // using the critical section to guard access to mpDSSQoSNetInitiatedList
  DSSCritScope lockDSSQoSNetInitiatedList(*mpCritSect);
  DSSQoSNetInitiated* pDSSQoSNetInitiated = mpDSSQoSNetInitiatedList;
  dss_qos_handle_type tmpQoSHandle = 0;

  while(NULL != pDSSQoSNetInitiated)
  {
    // get the handle of the current node in the DSSQoSNetInitiated list
    pDSSQoSNetInitiated->GetHandle(&tmpQoSHandle);

    // if the handle matches the QoSHandle the user provided then we found the
    // pDSSQoSNetInitiated
    if (tmpQoSHandle == qos_handle)
    {
      *ppDSSQoSNetInitiated = pDSSQoSNetInitiated;
      return AEE_SUCCESS;
    }

    IDS_ERR_RET(pDSSQoSNetInitiated->GetNext(&pDSSQoSNetInitiated));
  }

  // We haven't found any DSSQoSNetInitiated that matches the provided QoS Handle
  *ppDSSQoSNetInitiated = NULL;
  // The error value here should be QDS_EBADAPP but to be backward compatible
  // with the legacy API we return QDS_EBADF
  return QDS_EBADF;
}

AEEResult DSSNetActive::RemoveDSSNetQoSSecondary(uint32 iface_id)
{
   DSSNetQoSSecondary *pDSSNetQoSSecondary = mpDSSQoSSecondaryList, *ptmpDSSNetQoSSecondary = mpDSSQoSSecondaryList;
   uint32 tmpFlowID = 0;

   // if we have an empty list
   if (NULL == pDSSNetQoSSecondary) {
      return QDS_EINVAL;
   }

   while(NULL != pDSSNetQoSSecondary)
   {
      // get the Flow id of the current node in the DSSNetQoSSecondary list
      pDSSNetQoSSecondary->GetFlowID(&tmpFlowID);

      // if the flow ID matches the QoSHandle the used provided then we found the
      // DSSNetQoSSecondary that we need to remove
      if (tmpFlowID == iface_id)
      {
         // if this is the first node in the list
         if (ptmpDSSNetQoSSecondary == pDSSNetQoSSecondary)
         {
            IDS_ERR_RET(pDSSNetQoSSecondary->GetNext(&mpDSSQoSSecondaryList));
            PS_MEM_RELEASE(pDSSNetQoSSecondary);
            return AEE_SUCCESS;
         }

         // Get the node after the node that is to be removed
         DSSNetQoSSecondary* pNextDSSQoS = NULL;
         IDS_ERR_RET(pDSSNetQoSSecondary->GetNext(&pNextDSSQoS));

         // Connect the two parts of the list (before and after the removed node).
         ptmpDSSNetQoSSecondary->InsertToList(pNextDSSQoS);

         // delete the DSSNetQoSSecondary instance.
         PS_MEM_RELEASE(pDSSNetQoSSecondary);

         return AEE_SUCCESS;
      }

      // Initialize pointers for next iteration
      ptmpDSSNetQoSSecondary = pDSSNetQoSSecondary;
      IDS_ERR_RET(pDSSNetQoSSecondary->GetNext(&pDSSNetQoSSecondary));
   }

   // We haven't found any DSSNetQoSSecondary that matches the provided QoS Handle
   return QDS_EINVAL;
}

AEEResult DSSNetActive::RemoveDSSNetQoSNetInitiatedReqFromList(
  dss_qos_net_initiated_req_handle_type   qosNetInitiatedReqHandle,
  DSSQoSNetInitiatedReq                 **ppDSSNetQoSNetInitiatedReq
  )
{
  DSSQoSNetInitiatedReq *pDSSNetQoSNetInitiatedReq = mpDSSQoSNetInitiatedReqList;
  DSSQoSNetInitiatedReq *ptmpDSSNetQoSNetInitiatedReq = mpDSSQoSNetInitiatedReqList;
  dss_qos_net_initiated_req_handle_type tmpQosNetInitiatedReqHandle = 0;   

  // if we have an empty list
  if (NULL == mpDSSQoSNetInitiatedReqList) {
    return QDS_EFAULT;
  }

  while (NULL != pDSSNetQoSNetInitiatedReq)
  {
    // get the handle of the current node in the DSSQoSNetInitiatedReq list
    pDSSNetQoSNetInitiatedReq->GetHandle(&tmpQosNetInitiatedReqHandle);

    // if the handles match we found the DSSQoSNetInitiatedReq we need to remove
    if (tmpQosNetInitiatedReqHandle == qosNetInitiatedReqHandle)
    {
      *ppDSSNetQoSNetInitiatedReq = pDSSNetQoSNetInitiatedReq;
      // if this is the first node in the list
      if (ptmpDSSNetQoSNetInitiatedReq == pDSSNetQoSNetInitiatedReq)
      {
        IDS_ERR_RET(pDSSNetQoSNetInitiatedReq->GetNext(&mpDSSQoSNetInitiatedReqList));            
        return AEE_SUCCESS;
      }

      // Get the node after the node that is to be removed
      DSSQoSNetInitiatedReq* pNextDSSQoSNetInitiatedReq = NULL;
      IDS_ERR_RET(pDSSNetQoSNetInitiatedReq->GetNext(&pNextDSSQoSNetInitiatedReq));

      // Connect the two parts of the list (before and after the removed node).
      ptmpDSSNetQoSNetInitiatedReq->InsertToList(pNextDSSQoSNetInitiatedReq);                  

      return AEE_SUCCESS;
    }

    // Initialize pointers for next iteration
    ptmpDSSNetQoSNetInitiatedReq = pDSSNetQoSNetInitiatedReq;
    IDS_ERR_RET(pDSSNetQoSNetInitiatedReq->GetNext(&pDSSNetQoSNetInitiatedReq));
  }

  // We haven't found any DSSQoSNetInitiatedReq that matches the provided QoS Handle
  return QDS_EFAULT;
}

AEEResult DSSNetActive::RemoveDSSNetQoSNetInitiatedFromList
(
 dss_qos_handle_type   qosNetInitiatedHandle
 )
{
  // using the critical section to guard access to mpDSSQoSNetInitiatedList
  DSSCritScope lockDSSQoSNetInitiatedList(*mpCritSect);

  // While looping, pDSSQoSNetInitiated holds the current node in the list 
  DSSQoSNetInitiated *pDSSQoSNetInitiated = mpDSSQoSNetInitiatedList;

  // While looping, tmp holds the previous node in the list 
  DSSQoSNetInitiated *ptmpDSSQoSNetInitiated = mpDSSQoSNetInitiatedList;

  // While looping, this holds the Handle value for current node in the list
  dss_qos_handle_type tmpQosNetInitiatedHandle = 0;   

  // if we have an empty list
  if (NULL == mpDSSQoSNetInitiatedList) {
    return QDS_EINVAL;
  }

  while (NULL != pDSSQoSNetInitiated)
  {
    // get the handle of the current node in the DSSQoSNetInitiated list
    pDSSQoSNetInitiated->GetHandle(&tmpQosNetInitiatedHandle);
    // if the handle is zero the iface is down
    if (0 == tmpQosNetInitiatedHandle) {
      return QDS_EINVAL;
    }
    // if the handles match we found the DSSQoSNetInitiatedReq we need to remove
    if (tmpQosNetInitiatedHandle == qosNetInitiatedHandle)
    {
      // if this is the first node in the list
      if (ptmpDSSQoSNetInitiated == pDSSQoSNetInitiated)
      {
        IDS_ERR_RET(pDSSQoSNetInitiated->GetNext(&mpDSSQoSNetInitiatedList));
        PS_MEM_RELEASE(pDSSQoSNetInitiated);
        return AEE_SUCCESS;
      }

      // Get the node after the node that is to be removed
      DSSQoSNetInitiated* pNextDSSQoSNetInitiated = NULL;
      IDS_ERR_RET(pDSSQoSNetInitiated->GetNext(&pNextDSSQoSNetInitiated));
      PS_MEM_RELEASE(pDSSQoSNetInitiated);
      // Connect the two parts of the list (before and after the removed node).
      ptmpDSSQoSNetInitiated->InsertToList(pNextDSSQoSNetInitiated);                  

      return AEE_SUCCESS;
    }

    // Initialize pointers for next iteration
    ptmpDSSQoSNetInitiated = pDSSQoSNetInitiated;
    IDS_ERR_RET(pDSSQoSNetInitiated->GetNext(&pDSSQoSNetInitiated));
  }

  // We haven't found any DSSQoSNetInitiated that matches the provided QoS Handle
  return QDS_EINVAL;
}

AEEResult DSSNetActive::RemoveDSSPrivIpv6Addr(IIPv6Address *pIDSNetIpv6Address)
{
   DSSPrivIpv6Addr *pDSSPrivIpv6Addr = mpDSSPrivIpv6AddrList, *ptmpDSSPrivIpv6Addr = mpDSSPrivIpv6AddrList;
   IIPv6Address         *tmpIvp6Addr =  NULL;

   // if we have an empty list
   if (NULL == mpDSSPrivIpv6AddrList) {
      return QDS_EINVAL;
   }

   while(NULL != pDSSPrivIpv6Addr)
   {
      // get the IDSNetIpv6Address of the current node in the DSSPrivIpv6Addr list
      DSSCommon::ReleaseIf(reinterpret_cast<IQI**>(&tmpIvp6Addr));
      IDS_ERR_RET(pDSSPrivIpv6Addr->GetIDSNetIpv6Address(&tmpIvp6Addr));

      // if the IDSNetIpv6Address matches the IDSNetIpv6Address the used provided then we found the
      // DSSPrivIpv6Addr that we need to remove
      if (tmpIvp6Addr == pIDSNetIpv6Address)
      {
         // if this is the first node in the list
         if (ptmpDSSPrivIpv6Addr == pDSSPrivIpv6Addr)
         {
            IDS_ERR_RET(pDSSPrivIpv6Addr->GetNext(&mpDSSPrivIpv6AddrList));
            PS_MEM_RELEASE(pDSSPrivIpv6Addr);
            DSSCommon::ReleaseIf((IQI**)&tmpIvp6Addr);
            return AEE_SUCCESS;
         }

         // Get the node after the node that is to be removed
         DSSPrivIpv6Addr* pNextDSSPrivIpv6 = NULL;
         IDS_ERR_RET(pDSSPrivIpv6Addr->GetNext(&pNextDSSPrivIpv6));

         // Connect the two parts of the list (before and after the removed node).
         ptmpDSSPrivIpv6Addr->InsertToList(pNextDSSPrivIpv6);

         // delete the DSSPrivIpv6Addr instance.
         PS_MEM_RELEASE(pDSSPrivIpv6Addr);
         DSSCommon::ReleaseIf((IQI**)&tmpIvp6Addr);
         return AEE_SUCCESS;
      }

      // Initialize pointers for next iteration
      ptmpDSSPrivIpv6Addr = pDSSPrivIpv6Addr;
      IDS_ERR_RET(pDSSPrivIpv6Addr->GetNext(&pDSSPrivIpv6Addr));
   }

   // We haven't found any DSSNetQoSSecondary that matches the provided QoS Handle
   DSSCommon::ReleaseIf((IQI**)&tmpIvp6Addr);
   return QDS_EINVAL;
}

#ifdef FEATUTE_DATA_PS_MCAST
AEEResult DSSNetActive::RemoveDSSMCast(uint32 iface_id, ds::Net::EventType handlerType)
{
   DSSMCast *pDSSMCast = mpDSSMCastList, *ptmpDSSMCast = mpDSSMCastList;
   uint32 tmpFlowID = 0;

   // if we have an empty list
   if (NULL == pDSSMCast) {
      return QDS_EINVAL;
   }

   while(NULL != pDSSMCast)
   {
      // get the Flow id of the current node in the DSSMCast list
      pDSSMCast->GetMCastHandle(&tmpFlowID);

      // if the flow ID matches the MCastHandle the used provided then we found the
      // DSSMCast that we need to remove
      if (tmpFlowID == iface_id)
      {
         // if this is the first node in the list
         if (ptmpDSSMCast == pDSSMCast)
         {
            IDS_ERR_RET(pDSSMCast->GetNext(&mpDSSMCastList));
            if(TRUE == pDSSMCast->DeregisterHandler(handlerType)){
               PS_MEM_RELEASE(pDSSMCast);
            }
            else/* GetNext moves mpDSSMCastList to NULL */
            {
               mpDSSMCastList = pDSSMCast;
            }
            return AEE_SUCCESS;
         }

         // Get the node after the node that is to be removed
         DSSMCast* pNextDSSMCast = NULL;
         IDS_ERR_RET(pDSSMCast->GetNext(&pNextDSSMCast));

         // delete the DSSMCast instance.
         if(TRUE == pDSSMCast->DeregisterHandler(handlerType)){

           // Connect the two parts of the list (before and after the removed node).
           ptmpDSSMCast->InsertToList(pNextDSSMCast);

           PS_MEM_RELEASE(pDSSMCast);
         }
         else/* GetNext moves pNextDSSMCast to next item */
         {
            pNextDSSMCast = pDSSMCast;
         }

         return AEE_SUCCESS;
      }

      // Initialize pointers for next iteration
      ptmpDSSMCast = pDSSMCast;
      IDS_ERR_RET(pDSSMCast->GetNext(&pDSSMCast));
   }

   // We haven't found any DSSMCast that matches the provided MCast Handle
   return QDS_EINVAL;
}

AEEResult DSSNetActive::RemoveDSSMcastMBMSCtrl(IMCastMBMSCtrlPriv* pIDSNetMCastMBMS)
{
   DSSNetMCastMBMSCtrl *pMCastMBMS = mpDSSNetMCastMBMSList, *pTmpCastMBMS = mpDSSNetMCastMBMSList;
   IMCastMBMSCtrlPriv      *pTmpIDSMCastMBMS = NULL;

   // if we have an empty list
   if (NULL == mpDSSNetMCastMBMSList) {
      return QDS_EINVAL;
   }

   while(NULL != pTmpCastMBMS)
   {
      // get the IDSNetMCastMBMSCtrl of the current node in the DSSNetMCastMBMSCtrl list
      DSSCommon::ReleaseIf(reinterpret_cast<IQI**>(&pTmpIDSMCastMBMS));
      IDS_ERR_RET(pMCastMBMS->GetMBMSCtrl(&pTmpIDSMCastMBMS));

      // if the IDSNetMCastMBMSCtrl matches the IDSNetMCastMBMSCtrl the used provided then we found the
      // DSSNetMCastMBMSCtrl that we need to remove
      if (pTmpIDSMCastMBMS == pIDSNetMCastMBMS)
      {
         // if this is the first node in the list
         if (pTmpCastMBMS == pMCastMBMS)
         {
            IDS_ERR_RET(pMCastMBMS->GetNext(&mpDSSNetMCastMBMSList));
            PS_MEM_RELEASE(pMCastMBMS);
            DSSCommon::ReleaseIf((IQI**)&pTmpIDSMCastMBMS);
            return AEE_SUCCESS;
         }

         // Get the node after the node that is to be removed
         DSSNetMCastMBMSCtrl* pNextMCastMBMS = NULL;
         IDS_ERR_RET(pMCastMBMS->GetNext(&pNextMCastMBMS));

         // Connect the two parts of the list (before and after the removed node).
         pTmpCastMBMS->InsertToList(pNextMCastMBMS);

         // delete the DSSPrivIpv6Addr instance.
         PS_MEM_RELEASE(pMCastMBMS);
         DSSCommon::ReleaseIf((IQI**)&pTmpIDSMCastMBMS);
         return AEE_SUCCESS;
      }

      // Initialize pointers for next iteration
      pTmpCastMBMS = pMCastMBMS;
      IDS_ERR_RET(pMCastMBMS->GetNext(&pMCastMBMS));
   }

   // We haven't found any DSSNetQoSSecondary that matches the provided QoS Handle
   DSSCommon::ReleaseIf((IQI**)&pTmpIDSMCastMBMS);
   return QDS_EINVAL;
}


AEEResult DSSNetActive::GetNetMCastManager(IMCastManager** ppNetMCastManager)
{
   if (NULL == ppNetMCastManager)
   {
      LOG_MSG_ERROR_0("DSSNetActive::GetNetMCastManager(): "
                      "ppNetMCastManager is NULL");
      return QDS_EINVAL;
   }

   if(NULL != mpDSNetMCastManager) {
      *ppNetMCastManager = mpDSNetMCastManager;
      (void)mpDSNetMCastManager->AddRef();
      return AEE_SUCCESS;
   }

   // Create a new NetQoSManager since pNetActive does not have
   // a NetQoSManager instance.
   IDS_ERR_RET(mpIDSNetworkExt->CreateMCastManager(ppNetMCastManager));

   // Add the new created pNetQoSManager to pNetActive.
   if (AEE_SUCCESS != SetNetMCastManager(*ppNetMCastManager)) {
      DSSCommon::ReleaseIf((IQI**)ppNetMCastManager);
   }

   return AEE_SUCCESS;
}

AEEResult DSSNetActive::SetNetMCastManager(IMCastManager* pNetMCastManager)
{
   mpDSNetMCastManager = pNetMCastManager;
   (void)mpDSNetMCastManager->AddRef();

   return AEE_SUCCESS;
}
#endif // FEATUTE_DATA_PS_MCAST
AEEResult DSSNetActive::GetNetFirewallManager(IFirewallManager** ppNetFirewallManager)
{
   if (NULL == ppNetFirewallManager)
   {
      LOG_MSG_ERROR_0("DSSNetActive::GetNetFirewallManager(): "
                      "ppNetFirewallManager is NULL");
      return QDS_EINVAL;
   }

   if(NULL != mpDSNetFirewallManager)
   {
      *ppNetFirewallManager = mpDSNetFirewallManager;
      (void)mpDSNetFirewallManager->AddRef();
      return AEE_SUCCESS;
   }

   // Create a new NetFirewallManager since pNetActive does not have 
   // a NetFirewallManager instance.  
   IDS_ERR_RET(mpIDSNetworkExt2->CreateNetFirewallManager(ppNetFirewallManager));

   // Add the new created pNetFirewallManager to pNetActive.
   if (AEE_SUCCESS != SetNetFirewallManager(*ppNetFirewallManager)) {
      DSSCommon::ReleaseIf((IQI**)ppNetFirewallManager);
   }

   return AEE_SUCCESS;
}

AEEResult DSSNetActive::SetNetFirewallManager(IFirewallManager* pNetFirewallManager)
{
   mpDSNetFirewallManager = pNetFirewallManager;
   (void)mpDSNetFirewallManager->AddRef();

   return AEE_SUCCESS;
}
void DSSNetActive::FreeLists()
{
   // using the critical section to guard access to mpDSSQoSNetInitiatedList
   DSSCritScope lockDSSQoSNetInitiatedList(*mpCritSect);

   DSSNetQoSSecondary* tempNetQosSec = NULL;
#ifdef FEATUTE_DATA_PS_MCAST
   DSSMCast* tempMcast = NULL;
   DSSNetMCastMBMSCtrl* tempNetMcastCtrl = NULL;
#endif // FEATUTE_DATA_PS_MCAST
   DSSPrivIpv6Addr* tempPrivIpv6 = NULL;
   DSSQoSNetInitiatedReq* tempNetQosNetInitiatedReq = NULL;
   DSSQoSNetInitiated*    tempNetQosNetInitiated    = NULL;
   int ret;

   while (NULL != mpDSSQoSSecondaryList)
   {
      ret = mpDSSQoSSecondaryList->GetNext(&tempNetQosSec);
      if (AEE_SUCCESS != ret)
      {
         return;
      }
      PS_MEM_RELEASE(mpDSSQoSSecondaryList);
      mpDSSQoSSecondaryList = tempNetQosSec;
   }

   while (NULL != mpDSSQoSNetInitiatedReqList)
   {
     ret = mpDSSQoSNetInitiatedReqList->GetNext(&tempNetQosNetInitiatedReq);
     if (AEE_SUCCESS != ret)
     {
       return;
     }
     PS_MEM_RELEASE(mpDSSQoSNetInitiatedReqList);
     mpDSSQoSNetInitiatedReqList = tempNetQosNetInitiatedReq;
   }

   while (NULL != mpDSSQoSNetInitiatedList)
   {
     ret = mpDSSQoSNetInitiatedList->GetNext(&tempNetQosNetInitiated);
     if (AEE_SUCCESS != ret)
     {
       return;
     }
     PS_MEM_RELEASE(mpDSSQoSNetInitiatedList);
     mpDSSQoSNetInitiatedList = tempNetQosNetInitiated;
   }
#ifdef FEATUTE_DATA_PS_MCAST
   while (NULL != mpDSSMCastList) {
      ret = mpDSSMCastList->GetNext(&tempMcast);
      if (AEE_SUCCESS != ret)
      {
         return;
      }
      PS_MEM_RELEASE(mpDSSMCastList);
      mpDSSMCastList = tempMcast;
   }

   while (NULL != mpDSSNetMCastMBMSList) {
      ret = mpDSSNetMCastMBMSList->GetNext(&tempNetMcastCtrl);
      if (AEE_SUCCESS != ret)
      {
         return;
      }
      PS_MEM_RELEASE(mpDSSNetMCastMBMSList);
      mpDSSNetMCastMBMSList = tempNetMcastCtrl;
   }
#endif // FEATUTE_DATA_PS_MCAST
   while (NULL != mpDSSPrivIpv6AddrList) {
      ret = mpDSSPrivIpv6AddrList->GetNext(&tempPrivIpv6);
      if (AEE_SUCCESS != ret)
      {
         return;
      }
      PS_MEM_RELEASE(mpDSSPrivIpv6AddrList);
      mpDSSPrivIpv6AddrList = tempPrivIpv6;
   }

   (void) FlushDSSFirewallRuleTable();
}

DSSNetActive* DSSNetActive::CreateInstance(sint15 netHandle)
{
   return new DSSNetActive(netHandle);
}

AEEResult DSSNetActive::PPPOpen()
{
   AEEResult res = AEE_SUCCESS;
   AEEResult localRes = AEE_SUCCESS;
   NetworkStateType netState = NetworkState::QDS_CLOSED;
   ds::Net::INetworkPriv* pTmpIDSNetworkPriv = NULL;

   mpCritSect->Enter();

   if (NULL == mpIDSNetworkPriv) {
      mpCritSect->Leave();
      return QDS_EINVAL;
   }

   // get the network state
   res = GetNetworkState(&netState);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSNetActive::PPPOpen(): "
                      "GetNetworkState failed to fetch state");
   }
   else {
      if ((NetworkState::QDS_OPEN_IN_PROGRESS != netState) &&
          (NetworkState::QDS_OPEN != netState)) {

         // NetworkState::QDS_CLOSED event did not occure on NetActive object,
         // otherwise NetApp's mpNetActive was NULL
         // and it was impossible to enter this method.
         // Also, netState is one of three:
         // NetworkState::QDS_CLOSE_IN_PROGRESS
         // NetworkState::QDS_LINGERING
         // NetworkState::QDS_CLOSED
         
         pTmpIDSNetworkPriv = mpIDSNetworkPriv;
         mpCritSect->Leave();

         res = pTmpIDSNetworkPriv->BringUpInterface();
         //"WARNING ?!!! res can be either SUCCESS or WOULDBLOCK at this point. Do not override it unless there was an error"
         mpCritSect->Enter();
         if (AEE_SUCCESS == res || AEE_EWOULDBLOCK == res) {
            localRes = RegAutoEvents(pTmpIDSNetworkPriv);
            if (AEE_SUCCESS != localRes) {
               res = localRes;
               LOG_MSG_ERROR_1("DSSNetActive::PPPOpen(): "
                               "RegAutoEvents failed , res = 0x%x ", res);
            }
         }
      }

      // if the network is up or coming up, return AEE_SUCCESS
   }

   mpCritSect->Leave();
   return res;
}

boolean DSSNetActive::QoSNetInitiatedExists(IQoSNetInitiated* pQoSNetInitiatedToCheck, 
                                            DSSQoSNetInitiatedReq* pDSSQoSNetInitiatedReq)
{
  // using the critical section to guard access to mpDSSQoSNetInitiatedList
  DSSCritScope lockDSSQoSNetInitiatedList(*mpCritSect);

  DSSQoSNetInitiated* pDSSQoSNetInitiated = mpDSSQoSNetInitiatedList;
  IQoSNetInitiated* pQoSNetInitiated = NULL;
  AEEResult res = AEE_EFAILED;
  
  if (NULL == pQoSNetInitiatedToCheck)
  {
    LOG_MSG_ERROR_0("DSSNetActive::QoSNetInitiatedExists(): "
                    "NULL arg passed");
    return FALSE;
  }

  while (NULL != pDSSQoSNetInitiated)
  {
    res = pDSSQoSNetInitiated->GetQoSNetInitiated(&pQoSNetInitiated);
    if (AEE_SUCCESS != res || NULL == pQoSNetInitiated)
    {
      LOG_MSG_ERROR_1("DSSNetActive::QoSNetInitiatedExists(): "
                      "GetQoSNetInitiated() failed %d", res);
      return FALSE;
    }
    DSSGenScope scopeQoSNetInitiated(pQoSNetInitiated,DSSGenScope::IDSIQI_TYPE);

    // To find an already existing DSSQoSNetInitiated object we need to both match its ID
    // and the corresponding DSSQoSNetInitiatedReq ojbect to know that an object for this 
    // combination already exists.
    if ((pQoSNetInitiated->GetQoSNetInitiatedId() == pQoSNetInitiatedToCheck->GetQoSNetInitiatedId()) &&
        ((uint32)pDSSQoSNetInitiatedReq) == pDSSQoSNetInitiated->GetDSSQoSNetInitiatedAssocReq())
    {
      return TRUE;
    }

    res = pDSSQoSNetInitiated->GetNext(&pDSSQoSNetInitiated);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSNetActive::QoSNetInitiatedExists(): "
                      "DSSQoSNETInitiated::GetNext() failed %d", res);
      return FALSE;
    }
  }

  return FALSE;
}

AEEResult DSSNetActive::RemoveDSSNetQoSNetInitiatedAssocReq(DSSQoSNetInitiatedReq *pDSSNetQoSNetInitiatedReq)
{
  // using the critical section to guard access to mpDSSQoSNetInitiatedList
  DSSCritScope lockDSSQoSNetInitiatedList(*mpCritSect);
  dss_qos_handle_type pHandle = 0;

  DSSQoSNetInitiated* pDSSQoSNetInitiated = mpDSSQoSNetInitiatedList;
  IQoSNetInitiated* pQoSNetInitiated = NULL;
  AEEResult res = AEE_EFAILED;
  boolean removeFromList = FALSE;

  if (NULL == pDSSNetQoSNetInitiatedReq)
  {
    LOG_MSG_ERROR_0("DSSNetActive::RemoveDSSNetQoSNetInitiatedReqAddrRefs(): "
                    "NULL arg passed");
    return res;
  }

  // Iterating over all the DSSQoSNetInitiated objects to find the ones which were associated to
  // the DSSQoSNetInitiatedReq
  while (NULL != pDSSQoSNetInitiated)
  {
    res = pDSSQoSNetInitiated->GetQoSNetInitiated(&pQoSNetInitiated);
    if (AEE_SUCCESS != res || NULL == pQoSNetInitiated)
    {
      LOG_MSG_ERROR_1("DSSNetActive::RemoveDSSNetQoSNetInitiatedReqAddrRefs(): "
                      "GetQoSNetInitiated() failed %d", res);
      return res;
    }
    DSSGenScope scopeQoSNetInitiated(pQoSNetInitiated,DSSGenScope::IDSIQI_TYPE);

    // Removing the saved Addr value in the DSSQoSNetInitiated object
    if (((uint32)pDSSNetQoSNetInitiatedReq) == pDSSQoSNetInitiated->GetDSSQoSNetInitiatedAssocReq())
    {
      pDSSQoSNetInitiated->ClearDSSQoSNetInitiatedAssocReq();
      removeFromList = TRUE;
    }
    pDSSQoSNetInitiated->GetHandle(&pHandle);
    //GetNext has to be invoked before invoking RemoveDSSNetQoSNetInitiatedFromList,
    //otherwise it would be called on a released object
    res = pDSSQoSNetInitiated->GetNext(&pDSSQoSNetInitiated);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSNetActive::RemoveDSSNetQoSNetInitiatedReqAddrRefs(): "
                      "DSSQoSNETInitiated::GetNext() failed %d", res);
      return res;
    }
    // Search in DSSQoSNetInitiated list
    if (TRUE == removeFromList) // only remove from list after ClearDSSQoSNetInitiatedAssocReq
    {
      removeFromList = FALSE;
      res = RemoveDSSNetQoSNetInitiatedFromList(pHandle);
      if (AEE_SUCCESS != res) 
      {
        LOG_MSG_ERROR_1("DSSNetActive::RemoveDSSNetQoSNetInitiatedAssocReq(): Failed in "
          "RemoveDSSNetQoSNetInitiatedFromList, error %d", res);
      }
    }
  }

  return AEE_SUCCESS;
}
