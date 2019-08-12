/*======================================================

FILE:  DSSNetApp.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSNetApp class

=====================================================

Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetApp.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/


//===================================================================
//   Includes and Public Data Declarations
//===================================================================

#include "DSS_NetApp.h"
#include "DSS_Globals.h"
#include "DSS_Conversion.h"
#include "DSS_Common.h"
#include "DSS_IDSNetActiveScope.h"
#include "DSS_NetMonitoredScope.h"

#include "DSS_NetworkStateHandler.h"
#include "DSS_NetworkIPHandler.h"
#include "DSS_ExtendedIPConfigHandler.h"
#include "DSS_RFConditionsHandler.h"
#include "DSS_BearerTechHandler.h"
#include "DSS_OutageHandler.h"
#include "DSS_PhysLinkStateHandler.h"
#include "DSS_QoSAwareUnAwareHandler.h"
#include "DSS_PrimaryQoSModifyHandler.h"
#include "DSS_PrimaryQoSModifyStatusHandler.h"
#include "DSS_MTPDRequestHandler.h"
#include "DSS_SlottedResultHandler.h"
#include "DSS_SlottedSessionChangedHandler.h"
#include "DSS_HDRRev0RateInteriaHandler.h"
#include "DSS_QoSProfileChangedHandler.h"
#include "DSS_IPv6PrefixChangedStateHandler.h"
#include "DSS_IDSNetworkPrivScope.h"
#include "DSS_IDSNetPolicyPrivScope.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_MemoryManagement.h"
#include "DSS_PrivIpv6Addr.h"
#include "ds_Net_IIPv6Address.h"
#include "DSS_GenScope.h"

#include "ds_Net_INetworkPriv.h"

#include "DSS_MCast.h"

#include "DSS_EventHandlerMCastMBMSCtrl.h"
#include "DSS_NetMCastMBMSCtrl.h"

#include "ds_Addr_Def.h"
#include "ds_Utils_CCritSect.h"
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
//            DSSNetApp Functions Definitions
//===================================================================

// TODO: There's nothing preventing this class from being instantiated
// (has default operator new and operator delete)

//===================================================================
//  FUNCTION:   DSSNetApp::DSSNetApp
//
//  DESCRIPTION:
//  Constructor of the DSSNetApp class.
//===================================================================
// TODO: this should be constructed in two phases (never-failing constructor and
// an init() method).

DSSNetApp::DSSNetApp(): refCnt(1), weakRefCnt(1), mNetHandle(-1),
   mpNetActive(NULL), mpPrimaryNetMonitored(NULL),
   mpNetMonitoredList(NULL), 
   mSockCb(),
   mpIDSNetPolicy(0), mnNumOfSockets(0), mbIpsecDisabled(FALSE),
   mpCritSect(NULL), mbLastIfaceStateSentToApp(IFACE_STATE_INVALID),
   mNetCb(NULL), mpNetUserData(NULL), mLastNetDownReason(DSS_NET_DOWN_REASON_NOT_SPECIFIED)
{
   // DSSNetApp::Destructor makes use of mpCritSect before Releasing it
   // So we ASSERT here in case mpCritSect fails, otherwise 
   // DSSNetApp::Destructor will access uninitialized mpCritSect
   if (AEE_SUCCESS != DSSGlobals::Instance()->GetCritSect(&mpCritSect)) {
      DATA_ERR_FATAL("DSSNetApp::DSSNetApp():Couldn't fetch CritSect object from DSSGlobals");
   }
}

void * DSSNetApp::operator new (unsigned int numBytes) throw()
{
   return ps_mem_get_buf( PS_MEM_DSAL_NET_APP_TYPE);
}

void DSSNetApp::operator delete (void *  bufPtr) throw()
{
   PS_MEM_FREE(bufPtr);
}

AEEResult DSSNetApp::GetPolicy(IPolicy** ppIDSNetPolicy)
{
   DSSCritScope cs(*mpCritSect);

   if (NULL == mpIDSNetPolicy) {
      return QDS_EINVAL;
   }

   *ppIDSNetPolicy = mpIDSNetPolicy;
   (void)mpIDSNetPolicy->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetApp::SetPolicy(ds::Net::IPolicy* pIDSNetPolicy)
{
   ds::Net::IPolicy* pIDSNetPolicyCopy = NULL;
   
   DSSCritScope cs(*mpCritSect);

   if (NULL != mpNetActive) {
     LOG_MSG_ERROR_0("Cannot set policy when network is up!");
     return QDS_EINVAL;
   }

   // If mpIDSNetPolicy is not NULL then before changing the pointer
   // we release the current reference to avoid memory leak.
   if (NULL != mpIDSNetPolicy) {
      DSSCommon::ReleaseIf((IQI**)&mpIDSNetPolicy);
   }

   mpIDSNetPolicy = pIDSNetPolicy;

   if (NULL != mpIDSNetPolicy)
   {
      (void)mpIDSNetPolicy->AddRef();
   }

   pIDSNetPolicyCopy = mpIDSNetPolicy;

   // DSSNetMonitored doesn't cache the policy.
   // It merely sets the policy to its ds::Net::Network object member
   (void)mpPrimaryNetMonitored->SetPolicy(pIDSNetPolicyCopy);

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetApp::Init
//
//  DESCRIPTION:
//  Init the DSSNetApp class.
//===================================================================
AEEResult DSSNetApp::Init(INetwork* pIDSNetwork)
{
   AEEResult res = AEE_SUCCESS;

   // This method isn't required to be thread safe
   // A single object of DSSNetApp can never be accessed
   // legally from two threads at the same time

   if (NULL != mpPrimaryNetMonitored) {
      LOG_MSG_ERROR_1("Init() already called on NetApp 0x%x", this);
      return QDS_EINVAL;
   }
   
   mpPrimaryNetMonitored = DSSNetMonitored::CreateInstance(FALSE);
   if (NULL == mpPrimaryNetMonitored) {
      res = AEE_ENOMEMORY;
      goto bail;
   }

   res = mpPrimaryNetMonitored->Init(pIDSNetwork);
   if (AEE_SUCCESS != res) {
      goto bail;
   }

   mpPrimaryNetMonitored->SetNetHandle(mNetHandle);

   return AEE_SUCCESS;

bail:
   PS_MEM_RELEASE(mpPrimaryNetMonitored);
   return res;
}

void DSSNetApp::SetNetHandle(sint15 netHandle)
{
   DSSNetMonitored* pNetMonitored = NULL;
   
   mNetHandle = netHandle;
   if (NULL != mpNetActive) {
      mpNetActive->SetNetHandle(netHandle);
   }

   mpPrimaryNetMonitored->SetNetHandle(netHandle);

   pNetMonitored = mpNetMonitoredList;
   while (NULL != pNetMonitored) {
      pNetMonitored->SetNetHandle(netHandle);
      pNetMonitored->GetNext(&pNetMonitored);
   }
}

void DSSNetApp::GetNetworkIsUp(boolean *pNetworkIsUp)
{
  boolean networkIsUp = FALSE;
  AEEResult res = AEE_SUCCESS;
  NetworkStateType netState = NetworkState::QDS_CLOSED;

  DSSCritScope cs(*mpCritSect);

  // Check that net NetActive object exists.
  if (NULL != mpNetActive) {

     // get the network state
     res = mpNetActive->GetNetworkState(&netState);
     if (AEE_SUCCESS != res) {
        LOG_MSG_ERROR_0("DSSNetApp::GetNetworkIsUp(): "
                        "GetNetworkState failed to fetch state");
     }
     else {
        // if the network is up or coming up, set return value to TRUE
        if ((NetworkState::QDS_OPEN_IN_PROGRESS == netState) ||
            (NetworkState::QDS_OPEN == netState) ||
            (NetworkState::QDS_CLOSE_IN_PROGRESS == netState)) {
           networkIsUp = TRUE;
        }
     }
  }

  *pNetworkIsUp = networkIsUp;
}

AEEResult DSSNetApp::CreateNetActive()
{
   AEEResult res = AEE_SUCCESS;
   AEEResult localRes = AEE_SUCCESS;
   DSSNetActiveScope netActiveScope;
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;

   LOG_MSG_FUNCTION_ENTRY_1("DSSNetApp::CreateNetActive(), NetApp 0x%x", this);
   
   if (NULL != mpNetActive) { // Optimization.
      LOG_MSG_ERROR_0("DSSNetApp::CreateNetActive(): "
                      "NetActive object already exists");
      return QDS_EINVAL;
   }
   
   // create DSSNetActive object and perform bringup
   mpNetActive = DSSNetActive::CreateInstance(mNetHandle);
   if (NULL == mpNetActive) {
      LOG_MSG_ERROR_1("DSSNetApp::CreateNetActive(): "
                      "No memory to create DSSNetActive instance: %d", res);
      res = AEE_ENOMEMORY;
      goto bail;
   }

   // Make sure that NetActive isn't released during this function
   localRes = netActiveScope.Init(this);
   if (AEE_SUCCESS != localRes) {
      res = localRes;
      LOG_MSG_ERROR_2("DSSNetApp::CreateNetActive(): "
                      "Failed to fetch NetActive from NetApp object. res=%d pNetApp=0x%x",
                      res, this);
      goto bail;
   }

   // DSSNetActive::Init() brings up the network
   res = netActiveScope.Fetch()->Init(mpIDSNetPolicy, &mNetCb, mpNetUserData);
   if ((AEE_SUCCESS != res) && (AEE_EWOULDBLOCK != res)) {
      LOG_MSG_ERROR_1("DSSNetApp::CreateNetActive(): "
                      "Failed to init DSSNetActive: %d", res);
      goto bail;
   }
   //"WARNING ?!!! res can be either SUCCESS or WOULDBLOCK at this point. Do not override it unless there was an error"
   // DSSNetApp should always have primary monitored network. we ASSERT on it.
   ASSERT(NULL != mpPrimaryNetMonitored);

   SetPPPIsOpen(TRUE);

   // register to auto events
   localRes = IDSNetworkPrivScope.Init(netActiveScope.Fetch());
   if (AEE_SUCCESS != localRes) {
      res = localRes;
      LOG_MSG_ERROR_1("DSSNetApp::CreateNetActive(): "
                      "IDSNetworkPrivScope.Init failed , res = 0x%x ", res);
      goto bail;
   }

   localRes = netActiveScope.Fetch()->RegAutoEvents(IDSNetworkPrivScope.Fetch());
   if (AEE_SUCCESS != localRes) {
      res = localRes;
      LOG_MSG_ERROR_1("DSSNetApp::CreateNetActive(): "
                      "RegAutoEvents failed , res = 0x%x ", res);
      goto bail;
   }

   return res;

bail: // Only on error
   SetPPPIsOpen(FALSE);
   PS_MEM_RELEASE(mpNetActive);
   return res;
}

//===================================================================
//  FUNCTION:   DSSNetApp::~DSSNetApp
//
//  DESCRIPTION:
//  Destructor of the DSSNetApp class.
//===================================================================
/*lint -e{1551} */
void DSSNetApp::Destructor() throw()
{
   PS_MEM_RELEASE(mpNetActive);
   PS_MEM_RELEASE(mpPrimaryNetMonitored);

   // free NetMonitored list
   while (NULL != mpNetMonitoredList) {
      DSSNetMonitored* pNetMonitored = mpNetMonitoredList;
      mpNetMonitoredList->GetNext(&mpNetMonitoredList);
      PS_MEM_RELEASE(pNetMonitored);
   }
   
   DSSCommon::ReleaseIf((IQI**)&mpIDSNetPolicy);
   DSSCommon::ReleaseIf((IQI**)&mpCritSect);
}
/*lint –restore */

AEEResult DSSNetApp::GetPhysLinkObject(uint32 ifaceid, IPhysLink** ppPhyslink)
{
   FlowID flowid                        = 0;
   DSSNetMonitoredScope netMonitoredScope;
   AEEResult res                        = AEE_SUCCESS;

   flowid = DSSIfaceId::StripDownToFlowID(ifaceid);

   if (0 == flowid) { // No flow id defined
      res = netMonitoredScope.Init(this, ifaceid, TRUE);
      if (AEE_SUCCESS != res) {
         goto bail;
      }

      // TODO: Known issue - GetPhysLink() may fail if the physical link object does not exist.
      //       Need to implement some mechanism to enable registration to PhysLink events when 
      //       physical link does not exist.
      res = netMonitoredScope.Fetch()->GetPhysLink(ppPhyslink);
   } else {           // Flow id is real - extract corresponding qos obj
      res = mpNetActive->GetPhysLinkObject(ifaceid, ppPhyslink);
   }

bail:
   if (AEE_SUCCESS != res) {
      *ppPhyslink = NULL;
   }
   return res;
}

AEEResult DSSNetApp::FreeNetActive()
{
   DSSNetActive* pNetActive = NULL;
   
   mpCritSect->Enter();
   pNetActive = mpNetActive;
   mpNetActive = NULL;
   mpCritSect->Leave();
   
   PS_MEM_RELEASE(pNetActive);
   return AEE_SUCCESS;
}

AEEResult DSSNetApp::StopNetActive()
{
   DSSIDSNetActiveScope IDSNetActiveScope;
   AEEResult res = AEE_SUCCESS;;
   
   if (NULL == mpNetActive) {
      LOG_MSG_ERROR_0("DSSNetApp::StopNetActive(): "
                      "mpNetActive is NULL");
      return QDS_EINVAL;
   }
   
   // stop dsnet NetworkActive object
   IDSNetActiveScope.Init(mpNetActive);
   res = IDSNetActiveScope.Fetch()->Stop();
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_2("DSSNetApp::StopNetActive(): "
                      "ds::Net::INetwork::Stop() failed 0x%x, result = %d",
                      IDSNetActiveScope.Fetch(), res);
      goto bail;
   }

   // Notify dsnet network associated with the primary NetNonitored
   // object that the network is closed.
   // This way sockets receive network change state event
   // when the network is closed
   res = mpPrimaryNetMonitored->NotifyNetDown2Monitored();
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_2("DSSNetApp::StopNetActive(): "
                      "NotifyNetDown2Monitored() failed 0x%x, result = %d",
                      mpPrimaryNetMonitored, res);
      goto bail;
   }

bail:

   return res;
}

AEEResult DSSNetApp::GetNetActive(DSSNetActive** ppNetActive)
{
   DSSCritScope cs(*mpCritSect);
   *ppNetActive = mpNetActive;
   
   if (NULL == *ppNetActive) {
      return QDS_EINVAL;
   }

   (*ppNetActive)->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetApp::GetPrimaryNetMonitored(
   DSSNetMonitored** ppNetMonitored)
{
   *ppNetMonitored = mpPrimaryNetMonitored;
   (*ppNetMonitored)->AddRef();
   return AEE_SUCCESS;
}

AEEResult DSSNetApp::GetNetMonitoredByIfaceId(
   dss_iface_id_type ifaceId,
   bool bAllowToCreateSecondary,
   DSSNetMonitored** ppNetMonitored)
{
   DSSNetMonitored* pNetMonitored = NULL;
   DSSIDSNetPolicyPrivScope IDSNetPolicyPrivScope;
   INetworkFactory* pIDSNetworkFactory = NULL;
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;
   INetwork* pIDSNetwork = NULL;
   AEEResult res = AEE_SUCCESS;
   dss_iface_id_type netAppIfaceId = INVALID_IFACE_ID;
   dss_iface_id_type tmpIfaceId = INVALID_IFACE_ID;
   DSSGenScope scopeNetworkFactory;
   bool policyFlagIsOn = FALSE;

   DSSCritScope cs(*mpCritSect);

   GetIfaceId(&netAppIfaceId);
   if (netAppIfaceId == ifaceId) {
      *ppNetMonitored = mpPrimaryNetMonitored;
   }
   else {
      // search the monitored network list

      pNetMonitored = mpNetMonitoredList;
      while (NULL != pNetMonitored) {
         dss_iface_id_type monitoredIfaceId = 0;
         pNetMonitored->GetIfaceId(&monitoredIfaceId);
         if (monitoredIfaceId == ifaceId) {
            break;
         }

         pNetMonitored->GetNext(&pNetMonitored);
      }

      *ppNetMonitored = pNetMonitored;
   }

   // Check if NetMonitored object was found
   if (NULL != *ppNetMonitored) {
      (*ppNetMonitored)->AddRef();
      return res;
   }
   
   // needs to be reomved b4 submission
   LOG_MSG_INFO1_2("CR497295 bAllowToCreateSecondary %d"
                   "DSSNetApp obj 0x%p", bAllowToCreateSecondary, this );

   // Appropriate NetMonitored not found. Create new one.
   if (TRUE == bAllowToCreateSecondary)
   {
      policyFlagIsOn = DSSIfaceId::PolicyFlagIsOn(ifaceId);
      pNetMonitored = DSSNetMonitored::CreateInstance(policyFlagIsOn);
   
   if (NULL == pNetMonitored) {
     LOG_MSG_ERROR_0("DSSNetApp::GetNetMonitoredByIfaceId(): "
                     "Couldn't allocate DSSNetMonitored");
      return AEE_ENOMEMORY;
   }

      LOG_MSG_INFO1_2("DSSNetApp::GetNetMonitoredByIfaceId(): "
                      "DSSNetApp obj 0x%p created new secondary DSSNetMonitored Obj 0x%p",
                      this, pNetMonitored);

   pNetMonitored->SetNetHandle(mNetHandle);

   res = IDSNetPolicyPrivScope.Init();
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSNetApp::GetNetMonitoredByIfaceId(): "
                      "Can't create DSNetPolicyPriv");
      goto bail;
   }

   tmpIfaceId = DSSIfaceId::DSSIfaceId2DSNetIfaceId(ifaceId);
   res = IDSNetPolicyPrivScope.Fetch()->SetIfaceId(
      static_cast<ds::Net::IfaceIdType>(tmpIfaceId));
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSNetApp::GetNetMonitoredByIfaceId(): "
                      "Can't set iface id %u in policy", ifaceId);
      goto bail;
   }

   res = DSSGlobals::Instance()->GetNetworkFactory(&pIDSNetworkFactory);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSNetApp::GetNetMonitoredByIfaceId(): "
                      "Can't create IDSNetworkFactory");
      goto bail;
   }
   scopeNetworkFactory.SetParams(pIDSNetworkFactory, DSSGenScope::IDSIQI_TYPE);

   // Create IDSNetwork in Monitored Mode
   res = pIDSNetworkFactory->CreateNetwork(
      ds::Net::NetworkMode::QDS_MONITORED,
      IDSNetPolicyPrivScope.Fetch(),
      &pIDSNetwork);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSNetApp::GetNetMonitoredByIfaceId(): "
                      "Can't create INetwork");
      goto bail;
   }

   // DSSNetMonitored takes care to AddRef pIDSNetwork once it is set
   res = pNetMonitored->Init(pIDSNetwork);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSNetApp::GetNetMonitoredByIfaceId(): "
                      "Can't Initialize DSSNetApp");
      goto bail;
   }

   res = IDSNetworkPrivScope.Init(pNetMonitored);
   if (AEE_SUCCESS != res) {
     LOG_MSG_ERROR_0("DSSNetApp::GetNetMonitoredByIfaceId(): "
                     "Can't Initialize DSSNetApp");
     goto bail;
   }

   // Search and bound to an iface
   res = IDSNetworkPrivScope.Fetch()->LookupInterface();
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSNetApp::GetNetMonitoredByIfaceId(): "
                      "LookupInterface failed");
      goto bail;
   }

   AddNetMonitored(pNetMonitored);

   *ppNetMonitored = pNetMonitored;

   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
   }
   else {
      LOG_MSG_ERROR_2("DSSNetApp::GetNetMonitoredByIfaceId(): "
                      "Coudn't find secondary NetMonitored with matching ifaceId 0x%x in DSSNetApp obj 0x%p",
                      ifaceId, this);
      res = QDS_EFAULT;
   }

   return res;

bail:
   DSSCommon::ReleaseIf((IQI**)&pNetMonitored);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
   return res;
}


AEEResult DSSNetApp::AddNetMonitored(DSSNetMonitored* pNetMonitored) 
{
   DSSCritScope cs(*mpCritSect);
   pNetMonitored->SetNext(mpNetMonitoredList);
   mpNetMonitoredList = pNetMonitored;
   pNetMonitored->SetNetHandle(mNetHandle);
   mpNetMonitoredList->AddRef();

   return AEE_SUCCESS;
}

AEEResult DSSNetApp::UpdateNetDownReason()
{
   AEEResult res = AEE_SUCCESS;
   dss_net_down_reason_type reason = DSS_NET_DOWN_REASON_NOT_SPECIFIED;
   NetDownReasonType netdownreason = NetDownReason::QDS_NOT_SPECIFIED;
   DSSIDSNetActiveScope IDSNetActiveScope;

   DSSCritScope cs(*mpCritSect);

   if(mpNetActive == NULL)
   {
      res = AEE_EFAILED;
      LOG_MSG_ERROR_0("DSSNetApp::UpdateNetDownReason(): "
                      "mpNetActive is NULL");
      goto bail;
   }

   res = IDSNetActiveScope.Init(mpNetActive);
   if(AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSNetApp::UpdateNetDownReason(): "
                      "IDSNetActiveScope.Init failed");
      goto bail;
   }

   res = IDSNetActiveScope.Fetch()->GetLastNetDownReason(&netdownreason);
   if(AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSNetApp::UpdateNetDownReason(): "
                      "GetLastNetDownReason failed");
      goto bail;
   }

   res = DSSConversion::IDS2DSNetworkDownReason(netdownreason, &reason);
   if(AEE_SUCCESS != res) {
     LOG_MSG_ERROR_0("DSSNetApp::UpdateNetDownReason(): "
                     "IDS2DSNetworkDownReason failed");
   }

   mLastNetDownReason = reason;

bail:

   return res;
}

DSSNetApp* DSSNetApp::CreateInstance()
{
   return new DSSNetApp();
}

AEEResult DSSNetApp::PPPOpen()
{
   AEEResult res = AEE_SUCCESS;
   AEEResult localRes = AEE_SUCCESS;
   dss_iface_id_type ifaceId = DSS_IFACE_INVALID_ID;
   boolean dataCallAlreadyUp = FALSE;
   boolean netActiveCreated = FALSE;
   NetworkStateType netState = NetworkState::QDS_CLOSED;

   DSSCritScope cs(*mpCritSect);

   SetPPPIsOpen(TRUE);

   ResetLastIfaceStateSentToApp();

   // Check that net NetActive object exists.
   if (NULL != mpNetActive) {
      LOG_MSG_INFO1_2("DSSNetApp::PPPOpen(): "
                      "Modem application did not wait for event DS_ENETNONET/DS_ENETISCONN object already exist, obj 0x%x mpNetActive 0x%x",
                      this, mpNetActive);
      res = mpNetActive->GetNetworkState(&netState);

      if (AEE_SUCCESS != res) {
        LOG_MSG_ERROR_0("DSSNetApp::PPPOpen(): "
          "GetNetworkState failed to fetch state");
      }
      else {
        if (NetworkState::QDS_OPEN == netState) {
          dataCallAlreadyUp = TRUE;
          goto bail;
        } else
        if (NetworkState::QDS_OPEN_IN_PROGRESS != netState) {
            res = QDS_ENETCLOSEINPROGRESS;
            goto bail;
        }
      }
      res = AEE_EFAILED;
      goto bail;
   }
   else {
      SetPPPIsOpen(FALSE);
      
      // Brings up the interface
      res = CreateNetActive();
      netActiveCreated = TRUE;
      
      if (NULL == mpNetActive) {
         LOG_MSG_ERROR_2("DSSNetApp::PPPOpen(): "
                         "mpNetActive is NULL after calling CreateNetActive on obj 0x%x, res %d",
                         this, res);
         res = AEE_EFAILED;
         goto bail;
      }
      
      // Checks interface bring-up status
      if ((AEE_SUCCESS) != res && (AEE_EWOULDBLOCK != res)) {
         LOG_MSG_ERROR_3("DSSNetApp::PPPOpen(): "
                         "Failed to bringup on obj 0x%x from mpNetActive 0x%x res %d",
                         this, mpNetActive, res);
         goto bail;
      }
   }

   if (FALSE == dataCallAlreadyUp) {
   // get NetActive's iface ID to be used in primaryNetMonitored object
   localRes = mpNetActive->GetIfaceId(&ifaceId);
   if (AEE_SUCCESS != localRes) {
      res = localRes;
      LOG_MSG_ERROR_3("DSSNetApp::PPPOpen(): "
                      "Failed to GetIfaceId on obj 0x%x from mpNetActive 0x%x res %d",
                      this, mpNetActive, res);
      goto bail;
   }

   // Make primaryNetMonitored point to same iface as the NetActive
   localRes = mpPrimaryNetMonitored->ForceLookupInterfaceByIfaceId(ifaceId);
   if (AEE_SUCCESS != localRes) {
      res = localRes;
      LOG_MSG_ERROR_3("DSSNetApp::PPPOpen(): "
                      "Failed to ForceLookupInterfaceByIfaceId on obj 0x%x "
                      "from mpPrimaryNetMonitored 0x%x res %d",
                      this, mpPrimaryNetMonitored, res);
      goto bail;
   }
   }

   return res;

bail:

   if(netActiveCreated == TRUE)
   {
      (void) FreeNetActive();
   }
   return res;
}
