#ifndef __DSS_SOCKET_H__
#define __DSS_SOCKET_H__

/*======================================================

FILE:  DSS_Socket.h

SERVICES:
Backward Compatibility Layer socket class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_Socket.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
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

#include "DSS_CritScope.h"

#include "ds_Sock_ISocket.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "ds_Net_IIPFilterRegPriv.h"
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

//===================================================================
//              Macro Definitions
//===================================================================


//===================================================================
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSSSocket
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================

class DSSSocket : public ds::Utils::SignalHandlerBase
{
//-------------------------------------------------------------------
//  Constructors/Desctructors
//-------------------------------------------------------------------
  public:
    // TODO: when I implement the signals, check if I need the user data

    // Two-phase construction: the constructors can't fail.
    // Init() should be called after construction to perform the part of the
    // initialization that can fail.
    DSSSocket(dss_sock_cb_fcn_type* sockCb, sint15 netApp);
    AEEResult Init();

    virtual void Destructor() throw();

    // This destructor is not used.
    //
    // In Weak Ref-using classes, function Destructor() does the cleanup,
    // and the real destructor is a no-op.
    virtual ~DSSSocket() throw(){ /* NO-OP */ };

    static DSSSocket * CreateInstance(dss_sock_cb_fcn_type* sockCb, sint15 netApp);

//-------------------------------------------------------------------
//  Interface members
//-------------------------------------------------------------------
   // Register to events, the eventMask is of C API events.
   AEEResult RegEvent(sint31 eventMask);

   // Deregister from events, the eventMask is of C API events.
   AEEResult DeRegEvent(sint31 eventMask);

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//-------------------------------------------------------------------

    inline void GetSockFd(sint15 *pSockFd) {
       *pSockFd = mSockFd;
    }
    inline void SetSockFd(sint15 sockFd) {
       mSockFd = sockFd;
    }
    inline sint15 GetNetApp() {
       return mNetApp;
    }
    inline AEEResult SetNetApp(sint15 netApp) {
       mNetApp = netApp;
       return AEE_SUCCESS;
    }
    inline void GetIDSSocket(ds::Sock::ISocket **ppIDSsocket) {
       // TODO: maybe it should not be inline?
       DSSCritScope cs(*mpCritSect);
       *ppIDSsocket = mpIDSSock;
       if (NULL != mpIDSSock) {
          (void)mpIDSSock->AddRef();
       }
    }
    inline void SetIDSSocket(ds::Sock::ISocket *pIDSsocket) {
       mpIDSSock = pIDSsocket;
       if (NULL != mpIDSSock) {
          (void)mpIDSSock->AddRef();
       }
    }
    inline void GetEventOccurredMask(uint32 *pMask) {
       *pMask = mEventOccurredMask;
    }
    inline void SetEventOccurredMask(uint32 mask) {
       mEventOccurredMask = mask;
    }
    inline void GetSigOnOffMask(uint32 *pMask) {
       *pMask = mSigOnOffMask;
    }
    inline void SetSigOnOffMask(uint32 mask) {
       mSigOnOffMask = mask;
    }
   inline dss_sock_cb_fcn_type GetCb() {
      return mSockCb;
   }
   inline void SetCb(dss_sock_cb_fcn_type* cb) {
      // TODO: is it ok to make the assignment in that way?
      mSockCb = *cb;
   }
   inline AEEResult GetSdbAckCb(dss_so_sdb_ack_cb_type *pSdbAckCb)
   {
      *pSdbAckCb = mSdbAckCb;
      return AEE_SUCCESS;
   }
   AEEResult SetSdbAckCb(dss_so_sdb_ack_cb_type *pSdbAckCb);
   inline AEEResult SetSockKind(int kind)
   {
      mSockKind = kind;
      return AEE_SUCCESS;
   }
   inline void GetSockKind(int* pKind)
   {
      *pKind = mSockKind;
   }

   inline void SetSockProtocol(int protocol)
   {
      mProtocol = protocol;
   }

   inline void GetProtocol(byte* pProtocol)
   {
     *pProtocol = mProtocol;
   }

   inline void GetIpSecDisabled(boolean *pbIpSecDisabled)
   {
      *pbIpSecDisabled = mbIpsecDisabled;
   }

   inline void SetIpSecDisabled(boolean bIpSecDisabled)
   {
      mbIpsecDisabled = bIpSecDisabled;
   }

   inline void GetFlowForwarding(int *pbFlowFrwding)
   {
      *pbFlowFrwding = mbFlowFrwding;
   }

   inline void SetFlowForwarding(int pbFlowFrwding)
   {
      mbFlowFrwding = pbFlowFrwding;
   }

   inline AEEResult SetLegacyPolicy(dss_net_policy_info_type * from)
   {
      SetIpSecDisabled(from->ipsec_disabled);
      return AEE_SUCCESS;
   }

   inline AEEResult GetLegacyPolicy(dss_net_policy_info_type * to)
   {
      GetIpSecDisabled(&(to->ipsec_disabled));
      return AEE_SUCCESS;
   }


//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------
  protected:
    // Return adr of member variable containing a pSignal for the event
    ISignal** GetSignal(ds::Sock::SocketEventType eventId);

    // Return adr of member variable containing a pSignalCtl for the event
    ISignalCtl** GetSignalCtl(ds::Sock::SocketEventType eventId);

    // Given event ID, release the corresponding Signal and SignalCtl
    void ReleaseSignalAndSignalCtl(ds::Sock::SocketEventType eventId);

    static SignalHandlerCallBack GetEventHandlerFunc(ds::Sock::SocketEventType eventId);
    AEEResult CreateSignal(ISignalCtl** ppiSignalCtl,
                           ISignal** ppiSignal,
                           SignalHandlerCallBack eventHandlerFunc);

    ds::Net::IIPFilterRegPriv* filterReg;
//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
     sint15 mSockFd;
     sint15 mNetApp;
     ds::Sock::ISocket* mpIDSSock;
     dss_sock_cb_fcn_type mSockCb; // socket CB provided by app + User Data pointer
                                   // relevant only if app create the socket via dss_socket2
                                   // TODO: declare dss_sock_cb_fcn in dssocket.h
                                   //       with extern "C" {} so it can be correctly
                                   //       called from DSS to the application

     uint32   mSigOnOffMask;
     uint32   mEventOccurredMask;

     ISignal* mpReadSignal;
     ISignal* mpWriteSignal;
     ISignal* mpCloseSignal;
     ISignal* mpAcceptSignal;
     ISignal* mpSdbAckSignal;

     ISignalCtl* mpReadSignalCtl;
     ISignalCtl* mpWriteSignalCtl;
     ISignalCtl* mpCloseSignalCtl;
     ISignalCtl* mpAcceptSignalCtl;
     ISignalCtl* mpSdbAckSignalCtl;

     dss_so_sdb_ack_cb_type mSdbAckCb;

     boolean  mbIpsecDisabled;  /* Is IPSEC disabled ? */

     // TODO change back to boolean when the following line is present in OEMSock_GetDisableFlowFwding : nFlag = 0
     int  mbFlowFrwding; /* this option currently not supported , we need to store this for backward compatibility */

     ICritSect* mpCritSect;

     int mSockKind;
     byte mProtocol;

     void GeneralSignalFcn(uint32 eventId);
     static void ReadSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam);
     static void WriteSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam);
     static void CloseSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam);
     static void AcceptSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam);
     static void StaticSdbAckSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam);
     void SdbAckSignalFcn();

  private:
     void * operator new (
        unsigned int numBytes
     )  throw();
};


//===================================================================

#endif // __DSS_SOCKET_H__
