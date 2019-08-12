#ifndef __DSS_NETACTIVE_H__
#define __DSS_NETACTIVE_H__

/*======================================================

FILE:  DSS_NetActive.h

SERVICES:
Network Active class in Backward Compatibility Layer

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetActive.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-06-24 pg  Added to support qos handle when registration to net initiated qos.
  2013-11-01 rk  Removed feature mobileap.
  2011-05-07 aa  Class Created.

===========================================================================*/

//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "comdef.h"
#include "customer.h"
#include "dssocket.h"

#include "ds_Utils_StdErr.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Net_INetworkExt2.h"
#include "ds_Net_IQoSManager.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_IFirewallRule.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "DSS_CritScope.h"
#include "DSS_Common.h"
#include "DSS_NetQoSDefault.h"
#include "DSS_NetQoSSecondary.h"
#include "DSS_MCast.h"
#include "DSS_IfaceId.h"
#include "ds_Net_IMCastMBMSCtrlPriv.h"
#include "ds_Net_IIPv6Address.h"
#include "ds_Net_ITechUMTS.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_SignalHandlerBase.h"
#include "DSS_QoSNetInitiated.h"
#include "DSS_QoSNetInitiatedReq.h"
#include "ds_Net_IQoSNetInitiatedReq.h"

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
class DSSNetQoSSecondary;
class DSSNetQoSDefault;
#ifdef FEATUTE_DATA_PS_MCAST
class DSSNetMCastMBMSCtrl;
#endif // FEATUTE_DATA_PS_MCAST
class DSSPrivIpv6Addr;
class DSSPrimaryQoSModifyStatusHandler;

//===================================================================
//              Macro Definitions
//===================================================================


//===================================================================
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSSNetActive
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

class DSSNetActive: public ds::Utils::SignalHandlerBase
{
//-------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------
  public:
    DSSNetActive(sint15 netHandle);
    virtual ~DSSNetActive() throw() {};
    virtual void Destructor(void) throw();

    static DSSNetActive* CreateInstance(sint15 netHandle);

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------

    inline void GetNetHandle(sint15* pNetHandle)
    {
       *pNetHandle = mNetHandle;
    }

    inline void SetNetHandle(sint15 netHandle)
    {
       mNetHandle = netHandle;
    }

    inline AEEResult GetIfaceId(dss_iface_id_type* pIfaceId)
    {
       return GetIfaceIdByAppId(mNetHandle, pIfaceId);
    }

    inline AEEResult GetIfaceIdByAppId(sint15 appId, dss_iface_id_type* pIfaceId)
    {
      return DSSIfaceId::BuildIfaceId(mpIDSNetwork, DSSIfaceId::IFACE_ID_TYPE_CURRENT, appId, pIfaceId);
    }

    AEEResult GetIfaceIdForEventInfo(sint15 appId, dss_iface_id_type* pIfaceId);
    
    AEEResult GetIDSNetworkPrivObject(ds::Net::INetworkPriv** ppIDSNetworkPriv);

    inline void SetIDSNetworkPrivObject(ds::Net::INetworkPriv* pIDSNetworkPriv)
    {
       DSSCritScope cs(*mpCritSect);

       if (NULL != mpIDSNetworkPriv) {
          // This should never happened
          // IDSNetworkPriv is allocated and set together with the allocation of the DSSNetActive.
          // It is destroyed upon DSSNetActive destruction
          LOG_MSG_ERROR_0("SetIDSNetworkPrivObject: mpIDSNetworkPriv is NULL");
          ASSERT(0);
       }

       mpIDSNetworkPriv = pIDSNetworkPriv;
       (void)mpIDSNetworkPriv->AddRef();     // This shall keep the IDSNetworkPriv object alive until released in DSSNetActive DTor.
    }

    AEEResult GetIDSNetworkObject(ds::Net::INetwork** ppIDSNetwork);

    inline void SetIDSNetworkObject(ds::Net::INetwork* pIDSNetwork)
    {
       DSSCritScope cs(*mpCritSect);

       if (NULL != mpIDSNetwork) {
          // This should never happened
          // IDSNetworkPriv is allocated and set together with the allocation of the DSSNetActive.
          // It is destroyed upon DSSNetActive destruction
          LOG_MSG_ERROR_0("SetIDSNetworkObject: mpIDSNetwork is NULL");
          ASSERT(0);
       }

       mpIDSNetwork = pIDSNetwork;
       (void)mpIDSNetwork->AddRef();     // This shall keep the IDSNetwork object alive until released in DSSNetActive DTor.
    }

    AEEResult GetIDSNetworkExtObject(ds::Net::INetworkExt** ppIDSNetworkExt);

    inline void SetIDSNetworkExtObject(ds::Net::INetworkExt* pIDSNetworkExt)
    {
       DSSCritScope cs(*mpCritSect);

       if (NULL != mpIDSNetworkExt) {
          // This should never happened
          // IDSNetworkPriv is allocated and set together with the allocation of the DSSNetActive.
          // It is destroyed upon DSSNetActive destruction
          LOG_MSG_ERROR_0("SetIDSNetworkExtObject: mpIDSNetworkExt is NULL");
          ASSERT(0);
       }

       mpIDSNetworkExt = pIDSNetworkExt;
       (void)mpIDSNetworkExt->AddRef();     // This shall keep the IDSNetworkExt object alive until released in DSSNetActive DTor.
    }

    AEEResult GetIDSNetworkExt2Object(ds::Net::INetworkExt2** ppIDSNetworkExt2);

    inline void SetIDSNetworkExt2Object(ds::Net::INetworkExt2* pIDSNetworkExt2)
    {
       DSSCritScope cs(*mpCritSect);

       if (NULL != mpIDSNetworkExt2) {
          // This should never happened
          // IDSNetworkPriv is allocated and set together with the allocation of the DSSNetActive.
          // It is destroyed upon DSSNetActive destruction
          LOG_MSG_ERROR_0("SetIDSNetworkExt2Object: mpIDSNetworkExt2 is NULL");
          ASSERT(0);
       }

       mpIDSNetworkExt2 = pIDSNetworkExt2;
       (void)mpIDSNetworkExt2->AddRef();     // This shall keep the IDSNetworkExt2 object alive until released in DSSNetActive DTor.
    }

    // The following member functions serve iface_ioctl ds API. They return int as iface_ioctl returns int.
   AEEResult GetNetQoSManager(ds::Net::IQoSManager** ppNetQosManager);
   AEEResult SetNetQoSManager(ds::Net::IQoSManager* pNetQosManager);
#ifdef FEATUTE_DATA_PS_MCAST
   AEEResult GetNetMCastManager(ds::Net::IMCastManager** ppNetMCastManager);
   AEEResult SetNetMCastManager(ds::Net::IMCastManager* pNetMCastManager);
#endif // FEATUTE_DATA_PS_MCAST
   AEEResult GetNetFirewallManager(ds::Net::IFirewallManager** ppNetFirewallManager);
   AEEResult SetNetFirewallManager(ds::Net::IFirewallManager* pNetFirewallManager);
   AEEResult SetQoSDefault(ds::Net::IQoS* pNetQoSDefault);
   AEEResult GetQoSDefault(ds::Net::IQoS** ppNetQoSDefault);
   AEEResult GetDSSQoSNetInitiated(dss_qos_handle_type qos_handle, DSSQoSNetInitiated** ppDSSQoSNetInitiated);
   AEEResult AddNetQoSSecondary(ds::Net::IQoSSecondary* pNetQoSSecondary,
                                uint32 flowID,
                                dss_iface_ioctl_event_cb cback_fn,
                                void* user_data);
   AEEResult AddQoSNetInitiatedReq(
     ds::Net::IQoSNetInitiatedReq*         pNetQoSInitiated,
     dss_qos_net_initiated_req_handle_type qosNetInitiatedReqHandle,
     dss_iface_ioctl_event_cb              cback_fn,
     void*                                 user_data,
     dss_qos_handle_type*                  q_handle,
     dss_iface_id_type                     ifaceId,
     boolean                               real_flag);

   AEEResult AddQoSNetInitiated(
      DSSQoSNetInitiatedReq*  pDSSQoSNetInitiatedReq,
      dss_qos_handle_type     qos_handle,
      DSSQoSNetInitiated**    ppDSSQoSNetInitiated);
   AEEResult AddIpv6PrivAddr(ds::Net::IIPv6Address* pNetIpv6Address,
                             uint32 flowID,
                             boolean isUnique,
                             dss_iface_ioctl_event_cb cback_fn,
                             void* user_data);
   AEEResult GetDSSNetQoSSecondary(uint32 iface_id, DSSNetQoSSecondary** ppDSSNetQoSSecondary);
   AEEResult RemoveDSSNetQoSSecondary(uint32 iface_id);
   AEEResult RemoveDSSNetQoSNetInitiatedReqFromList(dss_qos_net_initiated_req_handle_type qosNetInitiatedReqHandle,
      DSSQoSNetInitiatedReq **ppDSSNetQoSNetInitiatedReq);
   AEEResult RemoveDSSNetQoSNetInitiatedFromList(dss_qos_handle_type   qosNetInitiatedHandle);
   AEEResult RemoveDSSNetQoSNetInitiatedAssocReq(DSSQoSNetInitiatedReq *pDSSNetQoSNetInitiatedReq);
   AEEResult RemoveDSSPrivIpv6Addr(ds::Net::IIPv6Address *pIDSNetIpv6Address);
#ifdef FEATUTE_DATA_PS_MCAST
   AEEResult RemoveDSSMCast(uint32 iface_id, ds::Net::EventType handlerType);
   AEEResult RemoveDSSMcastMBMSCtrl(ds::Net::IMCastMBMSCtrlPriv* pIDSNetMCastMBMS);
   AEEResult AddDSSMCast(ds::Net::IMCastSessionPriv* pMCastSession, uint32 flowID, dss_iface_ioctl_event_cb event_cb, void *user_data_ptr);
   AEEResult GetDSSMCast(uint32 iface_id, DSSMCast** ppDSSMCast);
#endif // FEATUTE_DATA_PS_MCAST
   AEEResult GetPhysLinkObject(uint32 ifaceid,ds::Net::IPhysLink **pPhyslink);
#ifdef FEATUTE_DATA_PS_MCAST
   AEEResult AddDSSMCastMBMSCtrl(ds::Net::IMCastMBMSCtrlPriv* pMCastMBMSCtrl,
                                 uint32 MCastMBMSHandle,
                                 dss_iface_ioctl_event_cb event_cb,
                                 void *user_data_ptr,
                                 dss_iface_id_type ifaceId);
   AEEResult GetMCastMBMSCtrl(uint32 iface_id, ds::Net::IMCastMBMSCtrlPriv** ppDSSMCastMBMSCtrl);
#endif // FEATUTE_DATA_PS_MCAST
   AEEResult AddDSSFirewallRule(ds::Net::IFirewallRule* pIFirewallRule, uint32* pFirewallRuleHandle);
   AEEResult GetDSSFirewallRule(uint32 firewallRuleHandle, ds::Net::IFirewallRule** ppIFirewallRule);
   AEEResult DeleteDSSFirewallRule(uint32 firewallRuleHandle);
   AEEResult FlushDSSFirewallRuleTable(void);
   AEEResult Init(ds::Net::IPolicy* pIDSNetPolicy, dss_net_cb_fcn* pNetCb, void* pNetUserData);
   
   // Frees up the memory used by mpDSSQoSSecondaryList mpDSSMCastList and mpDSSNetMCastMBMSList
   void FreeLists();

   AEEResult GetIfaceState(dss_iface_ioctl_state_type* pIfaceState);
   AEEResult GetNetworkState(ds::Net::NetworkStateType* pNetState);

   AEEResult RegEventCBPrimary(dss_iface_ioctl_event_enum_type event,
                               dss_iface_ioctl_event_cb event_cb,
                               void* user_data_ptr,
                               dss_iface_id_type ifaceId);
   template<typename HandlerType>
   AEEResult FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit);
   
   // Removes primary qos modify status handler (default qos handler)
   void RemovePrimaryQoSModifyStatusHandler();
   AEEResult RegAutoEvents(ds::Net::INetworkPriv* pIDSNetworkPriv);
   AEEResult PPPOpen();

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------
  protected:

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
  protected:
      static void IfaceUpOrDownSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam);

      sint15 mNetHandle;                        // nethandle representing this network library connection.

      ds::Net::INetworkPriv* mpIDSNetworkPriv;  // corresponding IDSNetworkPriv object for the nethandle.
      ds::Net::INetwork* mpIDSNetwork;
      ds::Net::INetworkExt* mpIDSNetworkExt;    // corresponding IDSNetworkExt object for the nethandle.
      ds::Net::INetworkExt2* mpIDSNetworkExt2;    // corresponding IDSNetworkExt2 object for the nethandle.

      ds::Net::IQoSManager* mpDSNetQoSManager;
      DSSNetQoSSecondary* mpDSSQoSSecondaryList;
      DSSQoSNetInitiatedReq* mpDSSQoSNetInitiatedReqList;
      DSSQoSNetInitiated* mpDSSQoSNetInitiatedList;
      DSSPrivIpv6Addr* mpDSSPrivIpv6AddrList;
      DSSNetQoSDefault* mpDSSNetQoSDefault;
#ifdef FEATUTE_DATA_PS_MCAST
      DSSMCast* mpDSSMCastList;
      DSSNetMCastMBMSCtrl* mpDSSNetMCastMBMSList;
      ds::Net::IMCastManager* mpDSNetMCastManager;
#endif // FEATUTE_DATA_PS_MCAST
      ds::Net::IFirewallManager* mpDSNetFirewallManager;
      dss_net_cb_fcn mNetCb; // network callback function
      void* mpNetUserData; // User data for network callback
      ISignal* mpIfaceUpSignal;
      ISignal* mpIfaceDownSignal;
      ISignalCtl* mpIfaceUpSignalCtl;
      ISignalCtl* mpIfaceDownSignalCtl;
      DSSPrimaryQoSModifyStatusHandler* mpPrimaryQoSModifyStatusHandler;
      dss_iface_ioctl_state_type mLastIfaceStateSentToApp;

      //TODO: This mapping can be enhanced to use a list instead.
      #define DSS_MAX_FIREWALL_HANDLES 10
      struct DSSFirewallHandleToObjectMapping
      {
        uint32                    handle;
        ds::Net::IFirewallRule *  pIFirewallRule;
      } DSSFirewallHandleToObject[DSS_MAX_FIREWALL_HANDLES];


      ICritSect* mpCritSect;

   private:
      boolean QoSNetInitiatedExists(ds::Net::IQoSNetInitiated* pQoSNetInitiated, 
                                    DSSQoSNetInitiatedReq* pDSSQoSNetInitiatedReq);

      void * operator new (
         unsigned int numBytes
      ) throw();

      void   operator delete (
         void *  bufPtr
      ) throw();

};

//===================================================================

#endif // __DSS_NETACTIVE_H__

