/*======================================================

FILE:  DSSGlobals.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSGlobals class

=====================================================

Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_Globals.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2010-04-18 en  History added.

===========================================================================*/

//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "DSS_Common.h"
#include "customer.h"

#include "ds_Utils_CCritSect.h"
#include "ds_Utils_CSignalFactory.h"

#include "ds_Net_INetworkPriv.h"

#include "DSS_Conversion.h"
#include "DSS_Globals.h"
#include "DSS_CritScope.h"
#include "ps_mem.h"
#include "DSS_MemoryManagement.h"
#include "DSS_IDSNetworkPrivScope.h"
#include "DSS_IDSNetPolicyPrivScope.h"

#include "ds_Net_CNetworkFactory.h"
#include "ds_Sock_CSocketFactory.h"
#include "dss_config.h"
#include "ds_Net_CreateInstance.h"
#include "ds_Sock_CreateInstance.h"
#include "ds_Utils_CreateInstance.h"

using namespace ds::Net;
using namespace ds::Error;

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------
#define NET_HANDLE_MASK 0x7FFF00

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

//===================================================================
//            DSSGlobals Functions Definitions
//===================================================================

//===================================================================
//  FUNCTION:   DSSGlobals::DSSGlobals
//
//  DESCRIPTION:
//  Constructor of the DSSGlobals class.
//===================================================================
DSSGlobals::DSSGlobals():
   mpNetFactory(0),
   mpSockFactory(0),
   mpSignalFactory(0),
   mpCritSect(0),
   mMapNetApp2Descriptor(
      PS_HANDLE_MGR_CLIENT_DSS_NETAPP,
      DSS_GLOBALS_BASE_DESCRIPTOR_MIN_NETAPP,
      DSS_GLOBALS_BASE_DESCRIPTOR_MAX_NETAPP),
   mMapSocket2Descriptor(
      PS_HANDLE_MGR_CLIENT_DSS_SOCK,
      DSS_GLOBALS_BASE_DESCRIPTOR_MIN_SOCKETS,
      DSS_GLOBALS_BASE_DESCRIPTOR_MAX_SOCKETS),
   mFilterRegInfoList(0),
   mTcpSocketsNum(0),
   mUdpSocketsNum(0)
{
   int i = 0;
   for (i = 0; i < NUM_OF_FLOW_IDS; ++i) {
      mQoSFlowIDs[i] = FALSE;
#ifdef FEATUTE_DATA_PS_MCAST
      mMCastFlowIDs[i] = FALSE;
#endif // FEATUTE_DATA_PS_MCAST
   }

   for (i = 0; i < NUM_OF_QOS_NET_INITIATED_REQUESTS; ++i) {
      mQoSNetInitiatedHandles[i] = FALSE;
   }
   
   mLastProvidedQoSNetInitiatedHandle = 0;
}

extern "C" void DSSGlobalInit
(
  void
)
{
  (void)DSSGlobals::Init();
}

extern "C" void DSSGlobalDeinit
(
  void
)
{
  (void)DSSGlobals::Deinit();
}

//===================================================================
//  FUNCTION:   DSSGlobals::~DSSGlobals
//
//  DESCRIPTION:
//  Destructor of the DSSGlobals class.
//===================================================================

/*lint -e{1551} */
DSSGlobals::~DSSGlobals()
{
   DSSCommon::ReleaseIf((IQI**)&mpNetFactory);
   DSSCommon::ReleaseIf((IQI**)&mpSockFactory);
   DSSCommon::ReleaseIf((IQI**)&mpSignalFactory);
   DSSCommon::ReleaseIf((IQI**)&mpCritSect);

   // TODO: loop over all the netApps and the sockets and delete all of them.
}
/*lint –restore */

//===================================================================
//  FUNCTION:   DSSGlobals::Instance
//
//  DESCRIPTION:
//    Any object in the DSS that needs any of the globals
//    that DSSGlobals implements uses this Instance member to gain
//    access to the *single* object of DSSGlobals.
//    This function is guaranteed to be called in power up 
//    from a single thread context and no lock is needed.
//===================================================================
DSSGlobals* DSSGlobals::Instance()
{
   if (NULL == pmInstance) {
     DATA_ERR_FATAL("DSSGlobals::Instance(): DSSGlobals::Init() must be called first ");
   }

   return pmInstance;
}

//===================================================================
//  FUNCTION:   DSSGlobals::GetCritSect
//
//  DESCRIPTION:
//    Getter for global critical section.
//    This function is guaranteed to be called in power up 
//    from a single thread context and no lock is needed.
//===================================================================
AEEResult DSSGlobals::GetCritSect(ICritSect** ppICritSect)
{
  if (NULL == pmInstance) {
      DATA_ERR_FATAL("DSSGlobals::GetCritSect(): DSSGlobals::Init() must be called first ");
   }

   if (NULL == ppICritSect) {
      return QDS_EINVAL;
   }

   *ppICritSect = mpCritSect;
   (void) mpCritSect->AddRef();

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSGlobals::Init
//
//  DESCRIPTION:
//    Init the singleton DSSGlobals. Must call this before using 
//    DSSGlobals::Instance()
//    This function is guaranteed to be called in power up 
//    from a single thread context and no lock is needed.
//===================================================================
void DSSGlobals::Init()
{
   int res;

   if (NULL != pmInstance) {
      LOG_MSG_ERROR_0 ("DSSGlobals::Init(): "
                       "Init has been already called! ");
      return;
   }

   DSSmem_pool_init();

   InstanceInit();
   if (NULL == pmInstance){
      DATA_ERR_FATAL("DSSGlobals::Init(): Can't allocate DSSGlobals singleton ");
   }

   res = DS_Utils_CreateInstance(NULL, AEECLSID_CCritSect,
                                 (void**)&pmInstance->mpCritSect);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSGlobals::Init(): "
                      "Can't create Critical Section (%d)", res);
      ASSERT(0);
   }

    pmInstance->mMapNetApp2Descriptor.Init();
    pmInstance->mMapSocket2Descriptor.Init();

}

//===================================================================
//  FUNCTION:   DSSGlobals::Deinit
//
//  DESCRIPTION:
//    Any object in the DSS that needs any of the globals
//    that DSSGlobals implements uses this Instance member to gain
//    access to the *single* object of DSSGlobals.

//===================================================================
void DSSGlobals::Deinit()
{

   if (NULL != pmInstance) {
      InstanceRelease();
   }
}

// TODO: do we need to lock the getters?
//===================================================================
//  FUNCTION:   DSSGlobals::GetSockFactory
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
void DSSGlobals::GetSockFactory(ds::Sock::ISocketFactory **ppFactory)
{
  DSSCritScope cs(*mpCritSect);

  if (NULL == mpSockFactory) {
      AEEResult res;

      res = DSSockCreateInstance(NULL,
                                 ds::Sock::AEECLSID_CSocketFactory,
                                 (void**)&mpSockFactory);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_0("DSSGlobals::GetSockFactory(): "
                         "Can't create SocketFactory");
         return;
      }
   }

   (void)mpSockFactory->AddRef();
   *ppFactory = mpSockFactory;
}

//===================================================================
//  FUNCTION:   DSSGlobals::GetNetworkFactory
//
//  DESCRIPTION:
//     Returns the network factory to the user
//===================================================================
AEEResult DSSGlobals::GetNetworkFactory(ds::Net::INetworkFactory **ppNetworkFactory)
{
   DSSCritScope cs(*mpCritSect);

   if (NULL == mpNetFactory) {
      AEEResult res = AEE_SUCCESS;
   
      res = DSNetCreateInstance(NULL,
                                AEECLSID_CNetworkFactory,
                                (void**)&mpNetFactory);

      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_0("GetNetworkFactory(): "
                         "Can't create NetworkFactory");
         return res;
      }
   }

   (void)mpNetFactory->AddRef();

   *ppNetworkFactory = mpNetFactory;

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSGlobals::GetSignalFactory
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
AEEResult DSSGlobals::GetSignalFactory(ISignalFactory **ppFactory)
{
   DSSCritScope cs(*mpCritSect);

   if (NULL == mpSignalFactory) {
      AEEResult res;

      res = DS_Utils_CreateInstance(NULL, AEECLSID_CSignalFactory,
                                    (void**)&mpSignalFactory);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_0("DSSGlobals::GetSignalFactory(): "
                         "Can't create SignalFactory");
         return res;
      }
   }

   (void)mpSignalFactory->AddRef();
   *ppFactory = mpSignalFactory;

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSGlobals::GetNetApp
//
//  DESCRIPTION:
//     Given a NetApp descriptor (net handle) this method returns
//     a pointer to the corresponding NetApp.
//===================================================================
AEEResult DSSGlobals::GetNetApp(sint15 netHandle, DSSNetApp **ppNetApp)
{
   DSSCritScope cs(*mpCritSect);
   AEEResult res = AEE_SUCCESS;

   // Check descriptor validity
   if (!mMapNetApp2Descriptor.DescriptorIsWithinBoundaries(netHandle))
   {
     LOG_MSG_ERROR_1("DescriptorIsWithinBoundaries failed, netHandle = %d",
                     netHandle);
     return QDS_EBADAPP;
   }

   // Get NetApp
   res = mMapNetApp2Descriptor.GetObjectByDescriptor(netHandle, ppNetApp);

   // Check if NetApp not found
   if (AEE_SUCCESS != res) {
     LOG_MSG_ERROR_1("NetApp not found, netHandle = %d", netHandle);
      return QDS_EBADAPP;
   }

   (void)(*ppNetApp)->SetNetHandle(netHandle);
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSGlobals::GetNetObjects
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================

AEEResult DSSGlobals::GetNetObjects(sint15 netHandle,
                                    dss_iface_id_type iface_id,
                                    bool bAllowSecondary,
                                    bool bAllowToCreateSecondary,
                                    bool bForbidTemporary,
                                    DSSNetApp** ppNetApp,
                                    DSSNetActiveScope* pNetActiveScope,
                                    DSSNetMonitoredScope* pNetMonitoredScope,
                                    dss_iface_ioctl_type ioctl_name,
                                    void* argval_ptr)
{
   DSSCritScope cs(*mpCritSect);
   DSSNetMonitored* pNetMonitored = NULL;
   AEEResult res = AEE_SUCCESS;

   // verify that policy bit is turned on.
   if (!DSSIfaceId::PolicyFlagIsOn(iface_id)) {
      // policy flag is off
      // try to extract nethandle form iface_id
      sint15 ifaceIdNetHandle = static_cast<sint15>((iface_id & NET_HANDLE_MASK) >> 8);
      if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID == netHandle) {
         // From this point on we shall use the netHandle as if it was directly provided by the app
         netHandle = ifaceIdNetHandle;
      }
      else {
         if (netHandle != ifaceIdNetHandle) {
            // Conflict between netHandle provided by app in the netHandle parameter and the netHandle
            // provided inside the iface_id (misuse by app!)
            LOG_MSG_ERROR_2("Conflict between netHandle provided by app in the netHandle parameter"
                            "and the netHandle provided inside the iface_id ( misuse by app! )    "
                            "netHandle = %d, ifaceIdNetHandle = %d",
                            netHandle, ifaceIdNetHandle);
            return QDS_EBADAPP;
         }
      }
   }

   // netHandle was not provided as a direct input nor found inside in the iface_id.
   // if current ioctl is qos , we can get it from qos_request->handle
   if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID == netHandle) {
      netHandle = GetNetHandleFromQoSRequestHandle(ioctl_name,argval_ptr);
   }

   // netHandle was not provided as a direct input nor found inside in the iface_id.
   // => The operation requested by the app shall be serviced via a *temporary* IDSNetwork object.
   if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID == netHandle) {
      if (TRUE == bForbidTemporary) {
         LOG_MSG_ERROR_0("DSSGlobals::GetNetObjects(): "
                         "TRUE == bForbidTemporary");
         return QDS_EBADF;
      }
      IDS_ERR_RET(CreateTempDSSNetMonitored(&pNetMonitored, iface_id));
      pNetMonitoredScope->Init(pNetMonitored);
      return AEE_SUCCESS;
   }

   // netHandle is known. Search in the primary and secondary DSSNetApps of this netHandle to find an appropriate DSSNetApp.
   // If not found, create a temporary DSSNetApp
   if (!IsValidNetApp(netHandle)) {
      LOG_MSG_ERROR_0("DSSGlobals::GetNetObjects(): "
                      "Invalid Application descriptor");
      return QDS_EBADF;
   }

   mMapNetApp2Descriptor.GetObjectByDescriptor(netHandle, ppNetApp);
   if (NULL == *ppNetApp) {
      LOG_MSG_ERROR_0("DSSGlobals::GetNetObjects(): "
                      "Invalid Application descriptor");
      return QDS_EBADAPP;
   }

   // Check if the provided iface id is the same as the one specified in the DSSNetApp.
   dss_iface_id_type PriIfaceId = INVALID_IFACE_ID;
   (*ppNetApp)->GetIfaceId(&PriIfaceId);
   if (DSSIfaceId::CompareIfaceIds(PriIfaceId,iface_id)) {
      res = pNetActiveScope->Init(*ppNetApp); // get net active
      if (AEE_SUCCESS != res)
      {
         LOG_MSG_INFO2_2("DSSGlobals::GetNetObjects(): "
                         "Failed to fetch NetActive from NetApp object. res=%d ppNetApp=0x%p",
                         res, *ppNetApp);
      }

      res = pNetMonitoredScope->Init(*ppNetApp); // get primary net monitored
      if (AEE_SUCCESS != res)
      {
         LOG_MSG_INFO2_2("DSSGlobals::GetNetObjects(): "
                         "Failed to fetch primary NetMonitored from NetApp object. res=%d ppNetApp=0x%p",
                         res, *ppNetApp);
      }
      
      return AEE_SUCCESS;
   }

   // bAllowSecondary == TRUE. Find corresponding secondary DSSNetMonitored
   // or create one if bAllowToCreateSecondary is TRUE
   if (TRUE == bAllowSecondary) {
      IDS_ERR_RET(pNetMonitoredScope->Init(*ppNetApp, iface_id, bAllowToCreateSecondary));
      return AEE_SUCCESS;
   }

   // The iface id of the Primary DSSNetApp does not correspond with the iface_id provided by the app
   if (TRUE == bForbidTemporary) {
      LOG_MSG_ERROR_0("DSSGlobals::GetNetObjects(): "
                      "TRUE == bForbidTemporary");
      return QDS_EBADAPP;
   }

   // Primary is incompatible with specified iface_id and secondary is not needed (synchronic operation).
   // Create a temporary DSSNetMonitored for this operation
   IDS_ERR_RET(CreateTempDSSNetMonitored(&pNetMonitored, iface_id));
   pNetMonitoredScope->Init(pNetMonitored);
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSGlobals::CreateTempDSSNetMonitored
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
AEEResult DSSGlobals::CreateTempDSSNetMonitored(DSSNetMonitored** ppNetMonitored,
                                                dss_iface_id_type iface_id)
{
   AEEResult res = AEE_SUCCESS;
   IPolicy* pIPolicy = NULL;
   INetworkFactory* pIDSNetworkFactory = NULL;
   INetwork* pIDSNetwork = NULL;
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;
   dss_iface_id_type tmpIfaceId = INVALID_IFACE_ID;
   DSSIDSNetPolicyPrivScope IDSNetPolicyPrivScope;
   DSSGenScope scopeNetworkFactory;

   // Create temporary DSSNetMonitored
   *ppNetMonitored = DSSNetMonitored::CreateInstance(FALSE);
   if (NULL == *ppNetMonitored) {
      LOG_MSG_ERROR_0("DSSGlobals::CreateTempDSSNetApp(): "
                      "Couldn't allocate DSSNetMonitored");
      res = AEE_ENOMEMORY;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetNetworkFactory(&pIDSNetworkFactory);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSGlobals::CreateTempDSSNetApp(): "
                      "Can't create IDSNetworkFactory");
      goto bail;
   }
   scopeNetworkFactory.SetParams(pIDSNetworkFactory, DSSGenScope::IDSIQI_TYPE);

   res = pIDSNetworkFactory->CreatePolicy(&pIPolicy);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSGlobals::CreateTempDSSNetApp(): "
                      "Can't create DSNetPolicy");
      goto bail;
   }

   IDSNetPolicyPrivScope.Init(pIPolicy);

   tmpIfaceId = DSSIfaceId::DSSIfaceId2DSNetIfaceId(iface_id);
   res = IDSNetPolicyPrivScope.Fetch()->SetIfaceId(static_cast<ds::Net::IfaceIdType>(tmpIfaceId));
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSGlobals::CreateTempDSSNetApp(): "
                      "Can't set iface id %u in policy", iface_id);
      goto bail;
   }

   // Create IDSNetwork in Monitored Mode
   res = pIDSNetworkFactory->CreateNetwork(ds::Net::NetworkMode::QDS_MONITORED,
                                           pIPolicy,
                                           &pIDSNetwork);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSGlobals::CreateTempDSSNetApp(): "
                      "Can't create IDSNetworkPriv");
      goto bail;
   }

   // DSSNetMonitored takes care to AddRef pIDSNetwork once it is set
   res = (*ppNetMonitored)->Init(pIDSNetwork);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSGlobals::CreateTempDSSNetApp(): "
                      "Can't Initialize DSSNetMonitored");
      goto bail;
   }

   res = IDSNetworkPrivScope.Init(*ppNetMonitored);
   if (AEE_SUCCESS != res) {
     LOG_MSG_ERROR_0("DSSGlobals::CreateTempDSSNetMonitored(): "
                     "Can't Initialize IDSNetworkPrivScope");
     goto bail;
   }

   (void) IDSNetworkPrivScope.Fetch()->LookupInterface();

   DSSCommon::ReleaseIf((IQI**)&pIPolicy);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);

   return AEE_SUCCESS;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIPolicy);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
   DSSCommon::ReleaseIf((IQI**)ppNetMonitored);

   return res;
}

//===================================================================
//  FUNCTION:   DSSGlobals::AreFlowIDsReleased
//
//  DESCRIPTION:
//===================================================================
bool DSSGlobals::AreFlowIDsReleased(void)
{
  DSSCritScope cs(*mpCritSect);

  for (int i=0; i<NUM_OF_FLOW_IDS; ++i)
  {
    if (TRUE == mQoSFlowIDs[i])
    {
      return FALSE;
    }
  }

  return TRUE;
}

//===================================================================
//  FUNCTION:   DSSGlobals::IsValidNetApp
//
//  DESCRIPTION:
//     Given a NetApp descriptor (net handle) this method returns
//     true when the NetApp descriptor exist in the NetApp List.
//===================================================================
bool DSSGlobals::IsValidNetApp(sint15 handle)
{
   DSSCritScope cs(*mpCritSect);
   bool exist = false;
   AEEResult res = AEE_SUCCESS;

   // Check that the given descriptor is valid
   if (!mMapNetApp2Descriptor.DescriptorIsWithinBoundaries(handle)) {
      return false;
   }

   res = mMapNetApp2Descriptor.DescriptorExist(handle, &exist);
   if (res != AEE_SUCCESS || false == exist)
   {
      return false;
   }

   return true;
}

//===================================================================
//  FUNCTION:   DSSGlobals::NetAppListFull
//
//  DESCRIPTION:
//     Returns true when the list is full, otherwise returns false.
//===================================================================
bool DSSGlobals::NetAppListFull()
{
   DSSCritScope cs(*mpCritSect);
   return mMapNetApp2Descriptor.Full();
}

//===================================================================
//  FUNCTION:   DSSGlobals::InsertNetApp
//
//  DESCRIPTION:
//    Inserts the given DSSNetApp object to the NetApp list.
//===================================================================
sint15 DSSGlobals::InsertNetApp(DSSNetApp *pNetApp)
{
   DSSCritScope cs(*mpCritSect);
   int32 netAppDescriptor = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;

   if (NULL == pNetApp) {
      LOG_MSG_ERROR_0("InsertNetApp given pNetApp is NULL");
      goto bail;
   }

   netAppDescriptor = mMapNetApp2Descriptor.Insert(pNetApp);

   if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID == netAppDescriptor) {
     goto bail;
   }

   LOG_MSG_INFO1_1(
"InsertNetApp allocated new NetApp handle %d", netAppDescriptor);

   /* Fall through */

bail:
   return static_cast<sint15>(netAppDescriptor);
}

//==============================================================================
//  FUNCTION:   DSSGlobals::RemoveNetApp
//
//  DESCRIPTION:
//     Given a descriptor (net handle) this method
//     removes a NetApp from the NetApp list
//
//  ASSUMPTIONS:
//     It is the responsibility of the caller to delete the DSSNetApp object.
//==============================================================================
AEEResult DSSGlobals::RemoveNetApp(sint15 netHandle)
{
   DSSCritScope cs(*mpCritSect);
   DSSNetApp*  pNetApp = NULL;
   AEEResult res = AEE_SUCCESS;

   if (!mMapNetApp2Descriptor.DescriptorIsWithinBoundaries(netHandle)) {
      return QDS_EBADAPP;
   }

   // Remove NetApp IAW its descriptor
   res = mMapNetApp2Descriptor.Remove(
      static_cast<int32>(netHandle), &pNetApp);

   return res;
}

//===================================================================
//  FUNCTION:   DSSGlobals::InsertSocket
//
//  DESCRIPTION:
//    Inserts the given pSocket to sockets array, returns socket
//    handle (descriptor), allocated to the newly inserted socket.
//    Returns DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID
//    in case of an error.
//===================================================================
sint15 DSSGlobals::InsertSocket(DSSSocket *pSocket)
{
   DSSCritScope cs(*mpCritSect);
   sint15 netHandle = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
   int curNumOfSockets = 0;
   int32 socketDescriptor = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
   DSSNetApp* pNetApp = NULL;
   AEEResult res = AEE_SUCCESS;

   if (NULL == pSocket) {
      LOG_MSG_ERROR_0("InsertSocket given pSocket is NULL");
      goto bail;
   }

 // allocate new socket handle (FD)
   socketDescriptor = mMapSocket2Descriptor.Insert(pSocket);
   if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID == socketDescriptor) {
       LOG_MSG_ERROR_0("DSSGlobals::InsertSocket(): Failed");
       goto bail;
   }

   LOG_MSG_INFO1_1("DSSGlobals::InsertSocket(): "
                   "InsertSocket allocated new socket handle %d",
                   socketDescriptor);

   netHandle = pSocket->GetNetApp();
   if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID != netHandle) {
      res = mMapNetApp2Descriptor.GetObjectByDescriptor(netHandle, &pNetApp);
      if (AEE_SUCCESS == res)
      {
         pNetApp->GetNumOfSockets(&curNumOfSockets);
        (void)pNetApp->SetNumOfSockets(1 + curNumOfSockets);
      }
   }

/* Fall through */

bail:

   // here we can safely cast cause we checked the value before
   return static_cast<sint15>(socketDescriptor);
}

//===================================================================
//  FUNCTION:   DSSGlobals::RemoveSocket
//
//  DESCRIPTION:
//     This method destroys the socket being removed and frees the
//     underlying memory. Existing pointers to the socket may no longer be
//     used after calling this function.
//===================================================================
AEEResult DSSGlobals::RemoveSocket(sint15 sockFd)
{
   DSSCritScope cs(*mpCritSect);
   DSSSocket *pSocket = NULL;
   DSSNetApp *pNetApp = NULL;
   sint15 netHandle = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
   int curNumOfSockets = 0;
   AEEResult res = AEE_SUCCESS;
   byte protocol = 0;

   // Remove socket IAW its socket descriptor
   res = mMapSocket2Descriptor.Remove(sockFd, &pSocket);
   if (AEE_SUCCESS != res) {
   LOG_MSG_ERROR_1("DSSGlobals::RemoveSocket(): "
                   "removing socket descriptor %d failed", sockFd);
      goto bail;
   }

   // Decrement the number of sockets associated with the DSSNetApp
   netHandle = pSocket->GetNetApp();
   if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID != netHandle) {
      res = mMapNetApp2Descriptor.GetObjectByDescriptor(netHandle, &pNetApp);
      if (AEE_SUCCESS == res)
      {
        pNetApp->GetNumOfSockets(&curNumOfSockets);
        (void)pNetApp->SetNumOfSockets(curNumOfSockets - 1);
      }
   }

   // Decrement the number of allocated TCP/UDP sockets
   // which is incremented in CreateSocket() on 
   // DSSSocket creation
   pSocket->GetProtocol(&protocol);
   if (PS_IPPROTO_TCP == protocol) {
      mTcpSocketsNum--;
   }
   else if (PS_IPPROTO_UDP == protocol) {
      mUdpSocketsNum--;
   }

   DSSCommon::ReleaseIf((IQI**)&pSocket);

/* fall through */

bail:

   return res;
}

//===================================================================
//  FUNCTION:   DSSGlobals::GetSocketById
//
//  DESCRIPTION:
//     Given a socket descriptor this method returns the socket
//     corresponding the socket descriptor
//===================================================================
AEEResult DSSGlobals::GetSocketById(sint15 sockfd,
                                    DSSSocket **ppDSSSocket)
{
   DSSCritScope cs(*mpCritSect);
   sint15 internalSocketFD = DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID;
   AEEResult res = AEE_SUCCESS;

   // Get socket according to its descriptor
   res = mMapSocket2Descriptor.GetObjectByDescriptor(
     static_cast<int32>(sockfd),
     ppDSSSocket);

   if (AEE_SUCCESS != res) {
      // LOG_MSG_ERROR is redundant here
      goto bail;
   }

   // Checking that socket descriptor parameter is the same as
   // socket descriptor in the socket object
   ((*ppDSSSocket)->GetSockFd(&internalSocketFD));
   if (internalSocketFD != sockfd) {
      LOG_MSG_ERROR_2("DSSGlobals::GetSocketById(): "
                      "socket descriptor provided by user %d"
                      "does not match socket descriptor stored in DSS list of sockets %d",
                      sockfd, internalSocketFD);

      // Can never happen unless there is a bug
      ASSERT(0);

      return QDS_EBADF;
   }

bail:

   return res;
}

//===================================================================
//  FUNCTION:   DSSGlobals::FindSocketWithEvents
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
AEEResult DSSGlobals::FindSocketWithEvents(sint15 dss_nethandle,
                                           sint15 *sockfd_ptr,
                                           uint32 *pEventOccurred,
                                           uint32 *pSigOnOff)
{
   DSSCritScope cs(*mpCritSect);
   DSSSocket* pSocket = NULL;
   int16 maxIndex = mMapSocket2Descriptor.GetMaxIndex();
   for (int i = 0; i < maxIndex; i++) {
      pSocket = mMapSocket2Descriptor.GetObjectByIndexUnchecked(i);
      if (NULL != pSocket) {
         sint15 nCurNetApp = pSocket->GetNetApp();
         if (dss_nethandle == nCurNetApp) {
            pSocket->GetEventOccurredMask(pEventOccurred);
            pSocket->GetSigOnOffMask(pSigOnOff);
            if (*pEventOccurred & *pSigOnOff) {
               pSocket->GetSockFd(sockfd_ptr);
               return AEE_SUCCESS;
            }
         }
      }
   }

   // nethandle wasn't found.
   return QDS_EBADF;
}

//===================================================================
//  FUNCTION:   DSSGlobals::CreateSocket
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
AEEResult DSSGlobals::CreateSocket(dss_sock_cb_fcn_type sockCb,
                                   sint15 sNetApp,
                                   sint15* psSockFd,
                                   DSSSocket** ppDSSSocket,
                                   int kind, byte protocol)
{
   AEEResult res = AEE_SUCCESS;
   bool socketListFull = true;

   if ( ((byte)PS_IPPROTO_TCP == protocol) &&
       (mTcpSocketsNum >= DSS_MAX_TCP_SOCKS) )
   {
      LOG_MSG_ERROR_1("DSSGlobals::CreateSocket(): "
                      "Can't create any more TCP sockets, maximum number of %d is created",
                      mTcpSocketsNum);
      return QDS_EMFILE;
   }

   if ( ((byte)PS_IPPROTO_UDP == protocol) &&
     (mUdpSocketsNum >= DSS_MAX_UDP_SOCKS) )
   {
      LOG_MSG_ERROR_1("DSSGlobals::CreateSocket(): "
                      "Can't create any more UDP sockets, maximum number of %d is created",
                      mUdpSocketsNum);
      return QDS_EMFILE;
   }

   *ppDSSSocket = DSSSocket::CreateInstance(&sockCb, sNetApp);
   if (0 == *ppDSSSocket) {
      LOG_MSG_ERROR_0("DSSGlobals::CreateSocket(): "
                      "Can't allocate DSSSocket");
      return AEE_ENOMEMORY;
   }
   res = (*ppDSSSocket)->Init();
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("DSSGlobals::CreateSocket(): "
                      "Can't init DSSSocket");
      DSSCommon::ReleaseIf((IQI**)ppDSSSocket);
      return res;
   }
   socketListFull = DSSGlobals::Instance()->SocketListFull();
   if (socketListFull) {
      LOG_MSG_ERROR_0("Too many descriptors are open");
      DSSCommon::ReleaseIf((IQI**)ppDSSSocket);
      return QDS_EMFILE;
   }
   *psSockFd = DSSGlobals::Instance()->InsertSocket(*ppDSSSocket);
   if (DSS_MAP_OBJECT_2_DESCRIPTOR_DESCRIPTOR_INVALID == *psSockFd) {
      LOG_MSG_ERROR_0("DSSGlobals::CreateSocket(): "
                      "Error allocationg socket descriptor");
      DSSCommon::ReleaseIf((IQI**)ppDSSSocket);
      return QDS_EMFILE;
   }
   (*ppDSSSocket)->SetSockFd(*psSockFd);
   (void)(*ppDSSSocket)->SetSockKind(kind);
   (*ppDSSSocket)->SetSockProtocol(protocol);

   // increment the number of TCP/UDP sockets
   if ((byte)PS_IPPROTO_TCP == protocol){
      mTcpSocketsNum++;
   }
   else if ((byte)PS_IPPROTO_UDP == protocol){
      mUdpSocketsNum++;
   }

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSGlobals::SocketListFull
//
//  DESCRIPTION:
//     Returns true when the list is full, otherwise returns false.
//===================================================================
bool DSSGlobals::SocketListFull()
{
   DSSCritScope cs(*mpCritSect);
   return mMapSocket2Descriptor.Full();
}

//===================================================================
//                 Helper / Internal Functions
//===================================================================

//===================================================================
//  FUNCTION:   GetFreeQoSFlowID
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
AEEResult DSSGlobals::GetFreeQoSFlowID(uint8 *pFlowID)
{
   DSSCritScope cs(*mpCritSect);
   
   for (int i=0; i<NUM_OF_FLOW_IDS; ++i)
   {
      if (FALSE == mQoSFlowIDs[i])
      {
         *pFlowID = i+1;
         mQoSFlowIDs[i] = TRUE;
         return AEE_SUCCESS;
      }
   }

   return QDS_EINVAL;
}

//===================================================================
//  FUNCTION:   GetFreeQoSNetInitiatedReqHandle
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
AEEResult DSSGlobals::GetFreeQoSNetInitiatedReqHandle(dss_qos_net_initiated_req_handle_type* handle)
{
   DSSCritScope cs(*mpCritSect);
   int startIndex = 0;

   // Set a start index for searching a free handle - consecutive to the last
   // allocated handle (to avoid allocating same handle in two consecutive
   // requests. This facilitates identification of the caller in case of many
   // requests and releases of Network Initiated QoS Status   
   switch (mLastProvidedQoSNetInitiatedHandle) {
            
      case 0:                                 // First ever allocation of handle
      case NUM_OF_QOS_NET_INITIATED_REQUESTS: // Last handle allocated was at the end of the array     
         startIndex = 0;
         break;

      default:
         startIndex = mLastProvidedQoSNetInitiatedHandle;
         break;
   }

   // Search for an available handle in the range "last provided handle .. end of array"
   for (int i=startIndex; i<NUM_OF_QOS_NET_INITIATED_REQUESTS; ++i)
   {
      if (FALSE == mQoSNetInitiatedHandles[i])
      {
         *handle = mLastProvidedQoSNetInitiatedHandle = i+1;
         mQoSNetInitiatedHandles[i] = TRUE;         
         return AEE_SUCCESS;
      }
   }

   // Search for an available handle in the range "start of array .. last provided handle"
   for (int i=0; i<startIndex; ++i)
   {
      if (FALSE == mQoSNetInitiatedHandles[i])
      {
         *handle = mLastProvidedQoSNetInitiatedHandle = i+1;
         mQoSNetInitiatedHandles[i] = TRUE;         
         return AEE_SUCCESS;
      }
   }

   // No free handles left
   return QDS_EINVAL;
}

//===================================================================
//  FUNCTION:   ReleaseQoSNetInitiatedHandle
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
void DSSGlobals::ReleaseQoSNetInitiatedReqHandle(dss_qos_net_initiated_req_handle_type handle)
throw () 
{
   DSSCritScope cs(*mpCritSect);
   if ( (handle > 0) && (handle <= NUM_OF_QOS_NET_INITIATED_REQUESTS) ) {
      mQoSNetInitiatedHandles[handle-1] = FALSE;    
   }
}
#ifdef FEATUTE_DATA_PS_MCAST
//===================================================================
//  FUNCTION:   GetFreeMCastFlowID
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
AEEResult DSSGlobals::GetFreeMCastFlowID(uint8 *pFlowID)
{
   DSSCritScope cs(*mpCritSect);

   for (int i=0; i<NUM_OF_FLOW_IDS; ++i)
   {
      if (FALSE == mMCastFlowIDs[i])
      {
         *pFlowID = i+1;
         mMCastFlowIDs[i] = TRUE;
         return AEE_SUCCESS;
      }
   }

   return QDS_EINVAL;
}
#endif // FEATUTE_DATA_PS_MCAST
//===================================================================
//  FUNCTION:   ReleaseQoSFlowID
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
void DSSGlobals::ReleaseQoSFlowID(uint8 flowID) throw()
{
   DSSCritScope cs(*mpCritSect);

   if (flowID > 0){
      mQoSFlowIDs[flowID - 1] = FALSE;
   }
   if (0 == flowID){
     mQoSFlowIDs[255] = FALSE;
}
}
#ifdef FEATUTE_DATA_PS_MCAST
//===================================================================
//  FUNCTION:   GetFreeFlowID
//
//  DESCRIPTION:
//     TODO: Documentation
//===================================================================
void DSSGlobals::ReleaseMCastFlowID(uint8 flowID) throw()
{
   DSSCritScope cs(*mpCritSect);

   if (flowID > 0){
      mMCastFlowIDs[flowID - 1] = FALSE;
   }
}
#endif // FEATUTE_DATA_PS_MCAST
AEEResult DSSGlobals::AddFilterRegObjectToList(IIPFilterRegPriv* filterReg, dss_iface_id_type iface_id, sint15 sockfd)
{
   DSSCritScope cs(*mpCritSect);
   FilterRegInfo* newFilterRegInfo = PS_MEM_NEW(FilterRegInfo);
   if (NULL == newFilterRegInfo) {
      LOG_MSG_ERROR_0("DSSGlobals::AddFilterRegObjectToList(): "
                      "Couldn't allocate FilterRegInfo");
      return AEE_ENOMEMORY;
   }

   newFilterRegInfo->filterReg = filterReg;
   (void)newFilterRegInfo->filterReg->AddRef();
   newFilterRegInfo->iface_id = iface_id;
   newFilterRegInfo->sockfd = sockfd;

   newFilterRegInfo->next = mFilterRegInfoList;
   mFilterRegInfoList = newFilterRegInfo;

   return AEE_SUCCESS;
}

AEEResult DSSGlobals::RemoveFilterRegObjectFromListByHandle(uint32 filterHandle)
{
  // match only filterHandle
  FilterRegInfo* save, *curr, *prev;
  FilterRegInfo * head = NULL;
  boolean bFilterIsFound = FALSE;
  boolean bHeadIsFound = FALSE;

  LOG_MSG_FUNCTION_ENTRY_1("DSSGlobals::RemoveFilterRegObjectFromListByHandle(): "
    "filterHandle 0x%x", filterHandle);
  // if the list is empty return.
  if (NULL == mFilterRegInfoList) {
    return AEE_SUCCESS;
  }

  mpCritSect->Enter();

  save = NULL;
  prev = NULL;
  curr = mFilterRegInfoList;
  while (curr){
    boolean bMatchingCondition = 
      ((uint32)curr->filterReg == filterHandle) ? 
      1 : 0;
    if (TRUE == bMatchingCondition) {
      save = curr->next;
      if (prev){
        prev->next = save;
      }
      curr->next = head;
      head = curr;
      bFilterIsFound = TRUE;
      curr = save;
    }
    else{
      if (!bHeadIsFound){
        mFilterRegInfoList = curr;
        bHeadIsFound = TRUE;
      }
      prev = curr;
      curr = curr->next;
    }
  }

  if (!bHeadIsFound){
    mFilterRegInfoList = NULL;
  }

  mpCritSect->Leave();

  while (head)
  {
    curr = head;
    head = head->next;
    DSSCommon::ReleaseIf((IQI**)&curr->filterReg);
    PS_MEM_DELETE(curr);
  }
  return (TRUE == bFilterIsFound) ? AEE_SUCCESS : QDS_EBADF;
}

AEEResult DSSGlobals::RemoveFilterRegObjectFromList(dss_iface_id_type iface_id, sint15 sockfd)
{
  // match both socket fd and interface id
  return RemoveAllMatchingFilterRegObjectFromList(iface_id, sockfd);
}

AEEResult DSSGlobals::RemoveFilterRegObjectFromList(sint15 sockfd)
{
   // match only socket fd
   return RemoveAllMatchingFilterRegObjectFromList(DSS_IFACE_INVALID_ID, sockfd);
}

AEEResult DSSGlobals::RemoveAllMatchingFilterRegObjectFromList(dss_iface_id_type iface_id, sint15 sockfd)
{
  FilterRegInfo* save, *curr, *prev;
  FilterRegInfo * head = NULL;
  boolean bFilterIsFound = FALSE;
  boolean bHeadIsFound = FALSE;

  LOG_MSG_FUNCTION_ENTRY_2("DSSGlobals::RemoveAllMatchingFilterRegObjectFromList(): "
                           "iface_id 0x%x, sockfd 0x%x", iface_id, sockfd);
  // if the list is empty return.
  if (NULL == mFilterRegInfoList) {
    return AEE_SUCCESS;
  }

  mpCritSect->Enter();

  save = NULL;
  prev = NULL;
  curr = mFilterRegInfoList;

  while (curr){
    boolean bMatchingCondition = 
      (DSS_IFACE_INVALID_ID == iface_id) ? 
      (sockfd == curr->sockfd) : (iface_id == curr->iface_id && sockfd == curr->sockfd); 
    if (TRUE == bMatchingCondition) {
      save = curr->next;
      if (prev){
        prev->next = save;
      }
      curr->next = head;
      head = curr;
      bFilterIsFound = TRUE;
      curr = save;
    }
    else{
      if (!bHeadIsFound){
        mFilterRegInfoList = curr;
        bHeadIsFound = TRUE;
      }
      prev = curr;
      curr = curr->next;
    }
  }

  if (!bHeadIsFound){
    mFilterRegInfoList = NULL;
  }

  mpCritSect->Leave();

  while (head)
  {
    curr = head;
    head = head->next;
      DSSCommon::ReleaseIf((IQI**)&curr->filterReg);
      PS_MEM_DELETE(curr);
  }
  return (TRUE == bFilterIsFound) ? AEE_SUCCESS : QDS_EBADF;
}

sint15 GetNetHandleFromQoSRequestHandle(dss_iface_ioctl_type ioctl_name,void* argval_ptr)
{
   dss_qos_handle_type qos_handle;
   switch (ioctl_name) {
      case DSS_IFACE_IOCTL_QOS_MODIFY:
         qos_handle = ((dss_iface_ioctl_qos_modify_type*)argval_ptr)->handle;
         break;
      case DSS_IFACE_IOCTL_QOS_RELEASE:
         qos_handle = ((dss_iface_ioctl_qos_release_type*)argval_ptr)->handle;
         break;
      case DSS_IFACE_IOCTL_QOS_SUSPEND:
         qos_handle = ((dss_iface_ioctl_qos_suspend_type*)argval_ptr)->handle;
         break;
      case DSS_IFACE_IOCTL_QOS_RESUME:
         qos_handle = ((dss_iface_ioctl_qos_resume_type*)argval_ptr)->handle;
         break;
      case DSS_IFACE_IOCTL_QOS_GET_STATUS:
         qos_handle = ((dss_iface_ioctl_qos_get_status_type*)argval_ptr)->handle;
         break;
      case DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC:
         qos_handle = ((dss_iface_ioctl_qos_get_flow_spec_type*)argval_ptr)->handle;
         break;
      case DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2:
         qos_handle = ((dss_iface_ioctl_qos_get_granted_flow_spec2_type*)argval_ptr)->handle;
         break;
      case DSS_IFACE_IOCTL_QOS_RELEASE_EX:
      case DSS_IFACE_IOCTL_QOS_SUSPEND_EX:
      case DSS_IFACE_IOCTL_QOS_RESUME_EX:
         if (NULL == ((dss_iface_ioctl_qos_release_ex_type*)argval_ptr)->handles_ptr) {
            return -1;
         }
         qos_handle = ((dss_iface_ioctl_qos_release_ex_type*)argval_ptr)->handles_ptr[0];
         break;

      default:
         return -1;
   }

   // qos_handle does not contain app id (probably error)
   if (qos_handle & IFACE_ID_POLICY_FLAG) {
      return -1;
   }

   return static_cast<sint15>((qos_handle & NET_HANDLE_MASK) >> 8);

}

void * DSSGlobals::operator new
(
   unsigned int numBytes
) throw()
{
   return ps_mem_get_buf( PS_MEM_DSAL_GLOBALS_TYPE);
} /* DSSGlobals::operator new() */


void DSSGlobals::operator delete
(
   void *  bufPtr
)
{
   PS_MEM_FREE(bufPtr);
   return;
} /* DSSGlobals::operator delete() */

