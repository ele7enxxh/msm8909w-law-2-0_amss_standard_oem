#ifndef __DSS_NETMONITORED_H__
#define __DSS_NETMONITORED_H__

/*======================================================

FILE:  DSS_NetMonitored.h

SERVICES:
Network Application class in Backward Compatibility Layer

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008 - 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetMonitored.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2011-08-31 hs  Added GetSubnetMask
  2011-08-30 hs  Added GetGatewayAddress
  2011-08-24 hs  Added GetDeviceName.
  2010-04-18 en  History added.

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
#include "DSS_IfaceId.h"

#include "DSS_MCast.h"
#include "ds_Net_IMCastMBMSCtrlPriv.h"
#include "ds_Net_IIPv6Address.h"
#include "ds_Net_ITechUMTS.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Utils_CSSupport.h"

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


//===================================================================
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSSNetMonitored
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

// Forward declarations to prevent circular inclusion of DSSNetMonitored.
class DSSEventHandler;
class DSSNetworkStateHandler;
class DSSNetworkIPHandler;
class DSSExtendedIPConfigHandler;
class DSSRFConditionsHandler;
class DSSBearerTechHandler;
class DSSBearerTechExHandler;
class DSSOutageHandler;
class DSSPhysLinkStateHandler;
class DSSFastDormStatusHandler;
class DSSQoSAwareUnAwareHandler;
class DSSPrimaryQoSModifyHandler;
class DSSQoSNetInitiatedReqHandler;
class DSSMTPDRequestHandler;
class DSSSlottedResultHandler;
class DSSSlottedSessionChangedHandler;
class DSSHDRRev0RateInteriaHandler;
class DSSQoSProfileChangedHandler;
class DSSIPv6PrefixChangedStateHandler;
class DSSNetQoSDefault;
class DSSNetworkExtIPv6DelAddrHandler;
class DSSHandoffInitHandler;
class DSSHandoffSuccessHandler;
class DSSHandoffFailureHandler;
class DSSExtendedIPConfigExHandler;
class DSSApnParamChangedHandler;

class DSSNetMonitored: public IWeakRef
{
//-------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------
  public:
    DSSNetMonitored(bool bPolicyBitIsOn);
    virtual ~DSSNetMonitored() throw() {};
    virtual void Destructor(void) throw();

    static DSSNetMonitored* CreateInstance(bool bPolicyBitIsOn);

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------
     // TODO: maybe the functions should return ::AEEResult
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
      sint15 netHandleTmp = 0;

      if (mPolicyBitIsOn) 
      { // we use NetHandle -1 since we want iface formatted to policy bit
        netHandleTmp = -1;
      }
      else
      {
        netHandleTmp = mNetHandle;
      }
      return GetIfaceIdByAppId(netHandleTmp, pIfaceId);
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
          // IDSNetworkPriv is allocated and set together with the allocation of the DSSNetMonitored.
          // It is destroyed upon DSSNetMonitored destruction
          LOG_MSG_ERROR_0("DSSNetMonitored::SetIDSNetworkPrivObject(): mpIDSNetworkPriv is NULL");
          ASSERT(0);
          return;
       }

       mpIDSNetworkPriv = pIDSNetworkPriv;
       (void)mpIDSNetworkPriv->AddRef();     // This shall keep the IDSNetworkPriv object alive until released in DSSNetMonitored DTor.
    }

    inline AEEResult ForceLookupInterfaceByIfaceId(dss_iface_id_type ifaceId)
    {
       dss_iface_id_type tmpIfaceId = ds::Net::INVALID_IFACE_ID;
       if (NULL == mpIDSNetworkPriv) {
          LOG_MSG_ERROR_0("DSSNetMonitored::ForceLookupInterfaceByIfaceId(): mpIDSNetworkPriv is NULL");
          return QDS_EINVAL;
       }

       tmpIfaceId = DSSIfaceId::DSSIfaceId2DSNetIfaceId(ifaceId);
       return mpIDSNetworkPriv->ForceLookupInterfaceByIfaceId(static_cast<ds::Net::IfaceIdType>(tmpIfaceId));
    }

    AEEResult NotifyNetDown2Monitored(void)
    {
       if (NULL == mpIDSNetworkPriv) {
          LOG_MSG_ERROR_0("DSSNetMonitored::NotifyNetDown2Monitored(): mpIDSNetworkPriv is NULL");
          return QDS_EINVAL;
       }

       return mpIDSNetworkPriv->NotifyNetDown2Monitored();
    }

    AEEResult GetIDSNetworkObject(ds::Net::INetwork** ppIDSNetwork);

    inline void SetIDSNetworkObject(ds::Net::INetwork* pIDSNetwork)
    {
       DSSCritScope cs(*mpCritSect);

       if (NULL != mpIDSNetwork) {
          // This should never happened
          // IDSNetworkPriv is allocated and set together with the allocation of the DSSNetMonitored.
          // It is destroyed upon DSSNetMonitored destruction
          LOG_MSG_ERROR_0("DSSNetMonitored::SetIDSNetworkObject(): mpIDSNetwork is NULL");
          ASSERT(0);
          return;
       }

       mpIDSNetwork = pIDSNetwork;
       (void)mpIDSNetwork->AddRef();     // This shall keep the IDSNetwork object alive until released in DSSNetMonitored DTor.
    }

    inline void GetPPPIsOpen(boolean *pPppIsOpen)
    {
       *pPppIsOpen = mPPPIsOpen;
    }

    inline void SetPPPIsOpen(boolean pppIsOpen)
    {
       mPPPIsOpen = pppIsOpen;
    }

    AEEResult GetIDSNetworkExtObject(ds::Net::INetworkExt** ppIDSNetworkExt);

    inline void SetIDSNetworkExtObject(ds::Net::INetworkExt* pIDSNetworkExt)
    {
       DSSCritScope cs(*mpCritSect);

       if (NULL != mpIDSNetworkExt) {
          // This should never happened
          // IDSNetworkPriv is allocated and set together with the allocation of the DSSNetMonitored.
          // It is destroyed upon DSSNetMonitored destruction
          LOG_MSG_ERROR_0("SetIDSNetworkExtObject: mpIDSNetworkExt is NULL");
          ASSERT(0);
          return;
       }

       mpIDSNetworkExt = pIDSNetworkExt;
       (void)mpIDSNetworkExt->AddRef();     // This shall keep the IDSNetworkExt object alive until released in DSSNetMonitored DTor.
    }

    AEEResult GetIDSNetworkExt2Object(ds::Net::INetworkExt2** ppIDSNetworkExt2);

    inline void SetIDSNetworkExt2Object(ds::Net::INetworkExt2* pIDSNetworkExt2)
    {
      DSSCritScope cs(*mpCritSect);

      if (NULL != mpIDSNetworkExt2) {
        // This should never happened
        // IDSNetworkPriv is allocated and set together with the allocation of the DSSNetMonitored.
        // It is destroyed upon DSSNetMonitored destruction
        LOG_MSG_ERROR_0("SetIDSNetworkExt2Object: mpIDSNetworkExt2 is NULL");
        ASSERT(0);
        return;
      }

      mpIDSNetworkExt2 = pIDSNetworkExt2;
      (void)mpIDSNetworkExt2->AddRef();     // This shall keep the IDSNetworkExt2 object alive until released in DSSNetMonitored DTor.
    }
    
    inline AEEResult GetMTPDIsRegistered(boolean* bMTPDIsReg)
    {
       if (NULL == mpMTPDRequestHandler)
       {
         *bMTPDIsReg = FALSE;
       }
       else
       {
          *bMTPDIsReg = TRUE;
       }

       return AEE_SUCCESS;
    }

    inline AEEResult GetNext(DSSNetMonitored** ppNetMonitored)
    {
       *ppNetMonitored = mNext;
       return AEE_SUCCESS;
    }

    inline AEEResult SetNext(DSSNetMonitored* pNetMonitored)
    {
       mNext = pNetMonitored;
       return AEE_SUCCESS;
    }

    AEEResult SetPolicy(ds::Net::IPolicy* pIDSNetPolicy);
    
    // The following member functions serve iface_ioctl ds API. They return int as iface_ioctl returns int.
    AEEResult GetIPv4Addr(dss_iface_ioctl_ipv4_addr_type* pIpv4Addr);
    AEEResult GetIPv6Addr(dss_iface_ioctl_ipv6_addr_type* pIpv6Addr);
    AEEResult GetIPv4PrimDnsAddr(dss_iface_ioctl_ipv4_prim_dns_addr_type* pIpv4Addr);
    AEEResult GetIPv6PrimDnsAddr(dss_iface_ioctl_ipv6_prim_dns_addr_type* pIpv6Addr);
    AEEResult GetIPv4SecoDnsAddr(dss_iface_ioctl_ipv4_seco_dns_addr_type* pIpv4Addr);
    AEEResult GetIPv6SecoDnsAddr(dss_iface_ioctl_ipv6_seco_dns_addr_type* pIpv6Addr);
    AEEResult GetAllDnsAddrs(dss_iface_ioctl_get_all_dns_addrs_type* pDnsAddrs);
    AEEResult SetQoSDefault(ds::Net::IQoS* pNetQoSDefault);
    AEEResult GetQoSDefault(ds::Net::IQoS** ppNetQoSDefault);
    AEEResult GetNetQoSManager(ds::Net::IQoSManager** ppNetQosManager);
    AEEResult SetNetQoSManager(ds::Net::IQoSManager* pNetQosManager);
    AEEResult GetMtu(dss_iface_ioctl_mtu_type* pMtu);
    AEEResult GetIPAddress(dss_iface_ioctl_get_ip_addr_type* pIpAddr);
    // TODO: maybe it should be declared separately since I'm not sure if it serves a specific ioctl
    AEEResult GetPreviousIPAddress(dss_iface_ioctl_get_ip_addr_type* pIpAddr);
    AEEResult GetIfaceState(dss_iface_ioctl_state_type* pIfaceState);

   AEEResult RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                        dss_iface_id_type ifaceId);
   AEEResult RegEventCB(dss_iface_ioctl_event_enum_type event,
                        dss_iface_ioctl_event_cb event_cb,
                        void* user_data_ptr,
                        dss_iface_id_type ifaceId);
   AEEResult DeregEventCB(dss_iface_ioctl_ev_cb_type* pEvArg);
   AEEResult DeregEventCB(dss_iface_ioctl_event_enum_type event);
   AEEResult GetRFConditions(dss_iface_ioctl_rf_conditions_type* pRFConds);
   AEEResult GetFastDormStatus(dss_iface_ioctl_fast_dormancy_status_event_type* pFastDormStatus);
   AEEResult GetBearerTech(dss_iface_ioctl_bearer_tech_type* pBearerTech);
   AEEResult GetBearerTechEx(dss_iface_ioctl_bearer_tech_ex_type* pBearerTechEx);
   AEEResult GetDataPathBridgeMode(dss_iface_ioctl_data_path_bridge_mode_type* pGetDataPath);
   AEEResult SetDataPathBridgeMode(dss_iface_ioctl_data_path_bridge_mode_type* pSetDataPath);
   AEEResult GetOpResPco(dss_iface_op_reserved_pco_type* pGetOpResPco);
   AEEResult GetApnParamInfo(dss_iface_ioctl_changed_apn_param_info_type* pApnParams);
   AEEResult _3GPPLTEReestabCalltypeGet(dss_iface_ioctl_3gpp_get_lte_reestab_calltype_type* p3gppLteReestabGet);
   AEEResult _3GPPLTEReestabCalltypeSet(dss_iface_ioctl_3gpp_set_lte_reestab_calltype_type* p3gppLteReestabSet);
   AEEResult SetSilentRedial(dss_iface_ioctl_set_silent_redial_type* pSilentRedial);
   AEEResult _3GPPFORCEPDNdisconnect();
   // TODO: maybe it should be declared separately since I'm not sure if it serves a specific ioctl
   AEEResult GetPreviousBearerTech(dss_iface_ioctl_bearer_tech_type* pBearerTech);
   AEEResult GetPhysLinkObject(uint32 ifaceid,ds::Net::IPhysLink **pPhyslink);
   AEEResult RegMTPDEventCB(dss_iface_ioctl_mt_reg_cb_type* pEvArg,
                            dss_iface_id_type ifaceId);
   AEEResult DeRegMTPDEventCB(dss_iface_ioctl_mt_dereg_cb_type* pEvArg);
   AEEResult Init(ds::Net::INetwork* pIDSNetwork);
   AEEResult GetPhysLink(ds::Net::IPhysLink** pIDSPhysLink);
   AEEResult GetTechUMTS(ds::Net::ITechUMTS** pIDSTechUMTS);
   AEEResult GetNetworkStatistics(dss_iface_ioctl_get_iface_stats_type* pStats);
   AEEResult ResetNetworkStatistics(void);
   AEEResult GetDeviceName(dss_iface_ioctl_device_name_type* pDeviceName);
   AEEResult GetGatewayAddress(dss_iface_ioctl_gateway_addr_type* pGatewayAddr);
   AEEResult GetSubnetMask(dss_iface_ioctl_subnet_mask_type* pSubnetMask);
   void ReleasePhysLink(void);
   AEEResult GetExtIPv6DelAddr(dss_iface_ioctl_ipv6_addr_type* pIpv6Addr);

   AEEResult GetExtIPConfigEx(dss_iface_ioctl_extended_ip_config_ex_type* pIpConfigEx);
   AEEResult Setkeepalive(dss_iface_keep_alive_flag_type* pKeepaliveflag);
   AEEResult GetPDNTeardownConfig(dss_iface_ioctl_pdn_teardown_config_type* pGetPDNTeardownConfig);
   AEEResult SetPDNTeardownConfig(dss_iface_ioctl_pdn_teardown_config_type* pSetPDNTeardownConfig);

//-------------------------------------------------------------------------
//  Definitions of IQI and IWeakRef Methods
//-------------------------------------------------------------------------
   DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------
  protected:

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
  protected:
      sint15 mNetHandle;                        // nethandle representing this network library connection.

      ds::Net::INetworkPriv* mpIDSNetworkPriv;  // corresponding IDSNetworkPriv object for the nethandle.
      ds::Net::INetwork* mpIDSNetwork;
      ds::Net::INetworkExt* mpIDSNetworkExt;    // corresponding IDSNetworkExt object for the nethandle.
      ds::Net::INetworkExt2* mpIDSNetworkExt2;  // corresponding IDSNetworkExt2 object for the nethandle.

      DSSNetQoSDefault* mpDSSNetQoSDefault;
      ds::Net::IQoSManager* mpDSNetQoSManager;

      DSSNetMonitored* mNext;                   // DSSNetMonitored list

      // These fields store the event handlers.
      DSSNetworkStateHandler* mpNetworkStateDownHandler;
      DSSNetworkStateHandler* mpNetworkStateUpHandler;
      DSSNetworkStateHandler* mpNetworkStateComingUpHandler;
      DSSNetworkStateHandler* mpNetworkStateGoingDownHandler;
      DSSNetworkIPHandler* mpNetIPHandler;
      DSSExtendedIPConfigHandler* mpExtendedIPConfigHandler;
      DSSRFConditionsHandler* mpRFConditionsHandler;
      DSSBearerTechHandler* mpBearerTechHandler;
      DSSBearerTechExHandler* mpBearerTechExHandler;
      DSSOutageHandler* mpOutageHandler;
      DSSPhysLinkStateHandler* mpPhysLinkDownHandler;
      DSSPhysLinkStateHandler* mpPhysLinkUpHandler;
      DSSPhysLinkStateHandler* mpPhysLinkComingUpHandler;
      DSSPhysLinkStateHandler* mpPhysLinkGoingDownHandler;
      DSSFastDormStatusHandler* mpFastDormStatusHandler; 
      DSSQoSAwareUnAwareHandler* mpQosAwareHandler;
      DSSQoSAwareUnAwareHandler* mpQosUnawareHandler;
      DSSPrimaryQoSModifyHandler* mpPrimaryQoSModifyHandler;
      DSSMTPDRequestHandler* mpMTPDRequestHandler;
      DSSSlottedResultHandler* mpSlottedResultSuccessHandler;
      DSSSlottedResultHandler* mpSlottedResultFailureHandler;
      DSSSlottedSessionChangedHandler* mpSlottedSessionChangedHandler;
      DSSQoSProfileChangedHandler* mpQoSProfileChangedHandler;
      DSSHDRRev0RateInteriaHandler* mpHDRRev0RateInteriaSuccessHandler;
      DSSHDRRev0RateInteriaHandler* mpHDRRev0RateInteriaFailureHandler;
      DSSIPv6PrefixChangedStateHandler* mpDSSIPv6PrefixChangedStateHandler;
      DSSNetworkExtIPv6DelAddrHandler   *mpNetworkExtIPv6DelAddrHandler;
      DSSHandoffInitHandler *mpHandoffInitHandler;
      DSSHandoffSuccessHandler *mpHandoffSuccessHandler;
      DSSHandoffFailureHandler *mpHandoffFailureHandler;
      DSSExtendedIPConfigExHandler *mpExtendedIpConfigExHandler;
      DSSApnParamChangedHandler *mpApnParamChangedHandler;
      boolean mPPPIsOpen;
      ICritSect* mpCritSect;
      boolean mPolicyBitIsOn;

      AEEResult GetEventHandler(dss_iface_ioctl_event_enum_type event,
                                DSSEventHandler** ppEventHandler, bool bInit);
      template<typename HandlerType>
      AEEResult FetchHandler(HandlerType** pHandler,
                             DSSEventHandler** ppEventHandler,
                             bool bInit,
                             dss_iface_ioctl_event_enum_type event);
      AEEResult ReleaseEventHandler(dss_iface_ioctl_event_enum_type event);

   private:
      ds::Net::IPhysLink*     mpDSNetPhysLink;
      ds::Net::ITechUMTS*     mpDSNetTechUMTS;

      void * operator new (
         unsigned int numBytes
      ) throw();

      void   operator delete (
         void *  bufPtr
      ) throw();
};

//===================================================================

#endif // __DSS_NETMONITORED_H__

