#ifndef __DSS_NETAPP_H__
#define __DSS_NETAPP_H__

/*======================================================

FILE:  DSS_NetApp.h

SERVICES:
Network Application class in Backward Compatibility Layer

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetApp.h#1 $
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
#include "comdef.h"
#include "customer.h"
#include "dssocket.h"

#include "DSS_NetActive.h"
#include "DSS_NetMonitored.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Net_INetworkExt2.h"
#include "ds_Net_IQoSManager.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_IFirewallRule.h"

#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "ds_Utils_ICritSect.h"
#include "DSS_Common.h"
#include "DSS_NetQoSDefault.h"
#include "DSS_NetQoSSecondary.h"

#include "DSS_MCast.h"
#include "ds_Net_IMCastMBMSCtrlPriv.h"
#include "ds_Net_IIPv6Address.h"
#include "ds_Net_ITechUMTS.h"
#include "ds_Net_IPhysLink.h"

#include "ds_Utils_CSSupport.h"
#include "ds_Utils_SignalHandlerBase.h"


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
#ifdef FEATUTE_DATA_PS_MCAST
class DSSMCast;
class DSSNetMCastMBMSCtrl;
#endif // FEATUTE_DATA_PS_MCAST
//===================================================================
//              Macro Definitions
//===================================================================


//===================================================================
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSSNetApp
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

enum NetHandleType
{
   NETLIB1,
   NETLIB2
};

class DSSNetApp : public IWeakRef
{
//-------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------
  public:
    DSSNetApp();
    virtual ~DSSNetApp() throw() {};
    virtual void Destructor(void) throw();

    static DSSNetApp* CreateInstance();

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------
     // TODO: maybe the functions should return ::AEEResult
    inline void GetNetHandle(sint15* pNetHandle)
    {
       *pNetHandle = mNetHandle;
    }
    
    void SetNetHandle(sint15 netHandle);
    
    void GetNetworkIsUp(boolean *pNetworkIsUp);

    inline void GetIfaceId(dss_iface_id_type* pIfaceId)
    {
       mpPrimaryNetMonitored->GetIfaceId(pIfaceId);
    }
    
    // dss_sock_cb_fcn_type includes the user data.
    inline void GetSocketCallback(dss_sock_cb_fcn_type* pSockCb)
    {
       *pSockCb = mSockCb;
    }
    // dss_sock_cb_fcn_type includes the user data.
    inline void SetSocketCallback(dss_sock_cb_fcn_type* pSockCb)
    {
       mSockCb = *pSockCb;
    }

    inline AEEResult GetNetworkCallback(dss_net_cb_fcn* pNetCb)
    {
       *pNetCb = mNetCb;
       return AEE_SUCCESS;
    }

    inline void SetNetworkCallback(dss_net_cb_fcn netCb)
    {
       mNetCb = netCb;
    }

    inline AEEResult GetNetworkUserData(void** ppNetUserData)
    {
       *ppNetUserData = mpNetUserData;
       return AEE_SUCCESS;
    }

    inline void SetNetworkUserData(void* pNetUserData)
    {
       mpNetUserData = pNetUserData;
    }

    inline void SetPPPIsOpen(boolean pppIsOpen)
    {
       mpPrimaryNetMonitored->SetPPPIsOpen(pppIsOpen);
    }

    AEEResult GetPolicy(ds::Net::IPolicy** ppIDSNetPolicy);

    inline void GetIpSecDisabled(boolean *pbIpSecDisabled)
    {
       *pbIpSecDisabled = mbIpsecDisabled;
    }

    inline void SetIpSecDisabled(boolean bIpSecDisabled)
    {
       mbIpsecDisabled = bIpSecDisabled;
    }

    inline AEEResult SetLegacyPolicy(dss_net_policy_info_type * from)
    {
       if (NULL == from) {
          SetIpSecDisabled(FALSE);
       } else {
         SetIpSecDisabled(from->ipsec_disabled);
       }

       return AEE_SUCCESS;
    }

    inline AEEResult GetLegacyPolicy(dss_net_policy_info_type * to)
    {
       GetIpSecDisabled(&(to->ipsec_disabled));
       return AEE_SUCCESS;
    }

    inline void GetNumOfSockets(int *pnNumOfSockets)
    {
       *pnNumOfSockets = mnNumOfSockets;
    }

    inline void SetNumOfSockets(int nNumOfSockets)
    {
       mnNumOfSockets = nNumOfSockets;
    }

    inline void ResetLastIfaceStateSentToApp(void)
    {
       DSSCritScope cs(*mpCritSect);
       
       mbLastIfaceStateSentToApp = IFACE_STATE_INVALID;
    }
    
    inline void GetNetDownReason(dss_net_down_reason_type* lastNetDownReason)
    {
      DSSCritScope cs(*mpCritSect);

      *lastNetDownReason = mLastNetDownReason;
    }

    AEEResult SetPolicy(ds::Net::IPolicy* pIDSNetPolicy);
    AEEResult GetPhysLinkObject(uint32 ifaceid,ds::Net::IPhysLink **pPhyslink);
    AEEResult Init(ds::Net::INetwork* pIDSNetwork);
    AEEResult GetNetActive(DSSNetActive** ppNetActive);
    AEEResult GetPrimaryNetMonitored(DSSNetMonitored** ppNetMonitored);
    AEEResult GetNetMonitoredByIfaceId(dss_iface_id_type ifaceId,
                                       bool bAllowToCreateSecondary,
                                       DSSNetMonitored** ppNetMonitored);
    AEEResult StopNetActive();
    AEEResult FreeNetActive();
    AEEResult AddNetMonitored(DSSNetMonitored* pNetMonitored);
    AEEResult UpdateNetDownReason();
    AEEResult PPPOpen();

//-------------------------------------------------------------------------
//  Defintions of IQI and IWeakRef Methods
//-------------------------------------------------------------------------
    DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------
  protected:
     AEEResult CreateNetActive();

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
  protected:
      sint15 mNetHandle;                        // nethandle representing this network library connection.

      DSSNetActive* mpNetActive;                 // active network
      DSSNetMonitored* mpPrimaryNetMonitored;    // monitored network, with the same iface id as the active network
                                                 // uses to register to events on the primary network
      DSSNetMonitored* mpNetMonitoredList;       // list of monitored networks

      dss_sock_cb_fcn_type mSockCb;             // socket callback function + User Data

      ds::Net::IPolicy* mpIDSNetPolicy;

      // TODO: int?
      int mnNumOfSockets;

      // Need to add methods for the secondary structures management.
      // TBD Need to add support to query the DSSNetApp about if it was created by open_netlib1 or 2.

      boolean  mbIpsecDisabled;  // Is IPSEC disabled ?

      ICritSect* mpCritSect;

      dss_iface_ioctl_state_type mbLastIfaceStateSentToApp;
      dss_net_cb_fcn mNetCb; // network callback function
      void* mpNetUserData; // User data for network callback
      dss_net_down_reason_type mLastNetDownReason; // Last Network down reason

   private:
      void * operator new (
         unsigned int numBytes
      ) throw();

      void   operator delete (
         void *  bufPtr
      ) throw();

};

//===================================================================

#endif // __DSS_NETAPP_H__

