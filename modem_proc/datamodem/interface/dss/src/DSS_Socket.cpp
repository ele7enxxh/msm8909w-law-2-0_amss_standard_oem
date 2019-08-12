/*======================================================

FILE:  DSS_Socket.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSSocket class

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_Socket.cpp#1 $
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
#include "DSS_Common.h"
#include "dserrno.h"

#include "DSS_Socket.h"
#include "DSS_Globals.h"
#include "DSS_CritScope.h"
#include "DSS_Conversion.h"
#include "ds_Sock_ISocketLocalPriv.h"
#include "ds_Sock_ISocketExt.h"
#include "ps_mem.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_Atomic.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"
#include "ds_Utils_DebugMsg.h"

using namespace ds::Error;

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
extern "C" void dssocki_sock_cb(sint15 dss_nethandle, sint15 sockfd, uint32 event_mask, void *user_data_ptr);

//===================================================================
//              Macro Definitions
//===================================================================

//===================================================================
//            DSSSocket Functions Definitions
//===================================================================

// TODO: documentation
DSSSocket::DSSSocket(dss_sock_cb_fcn_type* sockCb, sint15 netApp):
   mSockFd(-1), mNetApp(netApp), mpIDSSock(0), mSockCb(*sockCb), mSigOnOffMask(0),
   mEventOccurredMask(0), mpReadSignal(0), mpWriteSignal(0), mpCloseSignal(0),
   mpAcceptSignal(0), mpSdbAckSignal(0), mpReadSignalCtl(0), mpWriteSignalCtl(0),
   mpCloseSignalCtl(0), mpAcceptSignalCtl(0), mpSdbAckSignalCtl(0) ,mbIpsecDisabled(0),
   mbFlowFrwding(0), mpCritSect(0), mSockKind(0), mProtocol(0)
{
   mSdbAckCb.sdb_ack_cb = NULL;
   mSdbAckCb.user_data = NULL;
   filterReg = NULL;
}

AEEResult DSSSocket::Init()
{
   LOG_MSG_FUNCTION_ENTRY_0("DSSSocket::Init(): "
                            "Init()");

   IDS_ERR_RET(DSSGlobals::Instance()->GetCritSect(&mpCritSect));
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSSocket::~DSSSocket
//
//  DESCRIPTION:
//  Destructor of the DSSSocket class.
//===================================================================

/*lint -e{1551} */
void DSSSocket::Destructor() throw(){
   DSSCommon::ReleaseIf((IQI**)&mpIDSSock);

   DSSCommon::ReleaseIf((IQI**)&mpReadSignal);
   DSSCommon::ReleaseIf((IQI**)&mpWriteSignal);
   DSSCommon::ReleaseIf((IQI**)&mpCloseSignal);
   DSSCommon::ReleaseIf((IQI**)&mpAcceptSignal);
   DSSCommon::ReleaseIf((IQI**)&mpSdbAckSignal);

   DSSCommon::ReleaseIf((IQI**)&mpReadSignalCtl);
   DSSCommon::ReleaseIf((IQI**)&mpWriteSignalCtl);
   DSSCommon::ReleaseIf((IQI**)&mpCloseSignalCtl);
   DSSCommon::ReleaseIf((IQI**)&mpAcceptSignalCtl);
   DSSCommon::ReleaseIf((IQI**)&mpSdbAckSignalCtl);
   DSSCommon::ReleaseIf((IQI**)&mpCritSect);

   SignalHandlerBase::Destructor();
}
/*lint –restore */

// The signal for each event should: turn on the
// relevant bit in mEventOccurredMask (this is relevant only for old applications
// but if they use SignalBus we can do it anyway because this mask is used only by
// old apps), and if the relevant bit in mSigOnOffMask is on - call the callback

// TODO: documentation
void DSSSocket::GeneralSignalFcn(uint32 eventId)
{
   ds::Sock::SocketEventType IDSEventId = ds::Sock::SocketEvent::QDS_EV_WRITE;

   DSSCritScope cs(*mpCritSect);

   LOG_MSG_FUNCTION_ENTRY_2("DSSSocket::GeneralSignalFcn(): "
                            "GeneralSignalFcn(): event %d, sockFd %d",
                            eventId, mSockFd);

   uint32 tmpSigOnOffMask = mSigOnOffMask;

   // mSockCb should be always concurrent with pNetApp.mSockCb if socket was created with netapp - dss_socket
   // mSockCb can't be dssocki_sock_cb if socket wasn't created with netapp - dss_socket2
   // so we don't really need to get the sockCb stored inside the app, we can use the mSockCb directly

   // using old app model, update the mEventOccurredMask
   if (dssocki_sock_cb == mSockCb.sock_cb_fcn)
   {
      mEventOccurredMask |= eventId;

      // Note: Signal is not released in old app model.
   }
   // using new app model, registration is turned off automatically upon event cb
   else
   {
      mSigOnOffMask &= ~ eventId;

     (void)DSSConversion::DS2IDSEventId(eventId, &IDSEventId);
     ReleaseSignalAndSignalCtl(IDSEventId);
   }

   if (0 != (tmpSigOnOffMask & eventId)) {
      if (NULL != mSockCb.sock_cb_fcn) {
         LOG_MSG_INFO1_2("DSSSocket::GeneralSignalFcn(): "
                         "GeneralSignalFcn(): Calling user cb, event: %d, sockFd: %d",
                         eventId, mSockFd);
         mSockCb.sock_cb_fcn(mNetApp, mSockFd, eventId, mSockCb.sock_cb_user_data);
      }
   }
}

ISignal** DSSSocket::GetSignal(ds::Sock::SocketEventType IDSEventId)
{
   switch (IDSEventId) {
      case ds::Sock::SocketEvent::QDS_EV_WRITE:
         return &mpWriteSignal;
      case ds::Sock::SocketEvent::QDS_EV_READ:
         return &mpReadSignal;
      case ds::Sock::SocketEvent::QDS_EV_CLOSE:
         return &mpCloseSignal;
      case ds::Sock::SocketEvent::QDS_EV_ACCEPT:
         return &mpAcceptSignal;
      case ds::Sock::SocketEvent::QDS_EV_DOS_ACK:
         return &mpSdbAckSignal;
      default:
        LOG_MSG_ERROR_1("DSSSocket::GetSignal wrong eventID %d", IDSEventId);
        ASSERT(0);
   }

   return 0;
}

ISignalCtl** DSSSocket::GetSignalCtl(ds::Sock::SocketEventType IDSEventId)
{
   switch (IDSEventId) {
      case ds::Sock::SocketEvent::QDS_EV_WRITE:
         return &mpWriteSignalCtl;
      case ds::Sock::SocketEvent::QDS_EV_READ:
         return &mpReadSignalCtl;
      case ds::Sock::SocketEvent::QDS_EV_CLOSE:
         return &mpCloseSignalCtl;
      case ds::Sock::SocketEvent::QDS_EV_ACCEPT:
         return &mpAcceptSignalCtl;
      case ds::Sock::SocketEvent::QDS_EV_DOS_ACK:
         return &mpSdbAckSignalCtl;
      default:
         LOG_MSG_ERROR_1("DSSSocket::GetSignalCtl wrong eventID %d",
                         IDSEventId);
         ASSERT(0);
   }
   
   return 0;
}

void DSSSocket::ReleaseSignalAndSignalCtl(ds::Sock::SocketEventType IDSEventId)
{
   ISignal** ppSignal = NULL;
   ISignalCtl** ppSignalCtl = NULL;

   ppSignal = GetSignal(IDSEventId);
   ppSignalCtl = GetSignalCtl(IDSEventId);

   DSSCommon::ReleaseIf((IQI**)ppSignal);
   DSSCommon::ReleaseIf((IQI**)ppSignalCtl);

   // Note: Signal is not released in old app model.
}

SignalHandlerCallBack DSSSocket::GetEventHandlerFunc(ds::Sock::SocketEventType IDSEventId)
{
   switch (IDSEventId) {
      case ds::Sock::SocketEvent::QDS_EV_WRITE:
         return WriteSignalFcn;
      case ds::Sock::SocketEvent::QDS_EV_READ:
         return ReadSignalFcn;
      case ds::Sock::SocketEvent::QDS_EV_CLOSE:
         return CloseSignalFcn;
      case ds::Sock::SocketEvent::QDS_EV_ACCEPT:
         return AcceptSignalFcn;
      case ds::Sock::SocketEvent::QDS_EV_DOS_ACK:
         return StaticSdbAckSignalFcn;
      default:
        LOG_MSG_ERROR_1("DSSSocket::GetEventHandlerFunc wrong eventID %d",
                        IDSEventId);
        ASSERT(0);
   }

   return 0;
}

AEEResult DSSSocket::CreateSignal(ISignalCtl** ppiSignalCtl,
                                  ISignal** ppiSignal,
                                  SignalHandlerCallBack eventHandlerFunc)
{
   ISignalFactory *pSignalFactory = 0;
   IDS_ERR_RET(DSSGlobals::Instance()->GetSignalFactory(&pSignalFactory));
   if ((NULL == pSignalFactory) || (NULL == ppiSignalCtl) ||
       (NULL == ppiSignal) || (0 == eventHandlerFunc))
   {
      DATA_ERR_FATAL("DSSSocket::CreateSignal either wrong arguments or failed to fetch Signal Factory");
      return QDS_EINVAL;
   }

   LOG_MSG_FUNCTION_ENTRY_2("DSSSocket::CreateSignal(): "
                            "CreateSignal(): signal 0x%p signalCtl 0x%p",
                            *ppiSignal, *ppiSignalCtl);

   IDS_ERR_RET(pSignalFactory->CreateSignal(&signalHandler,
                                            eventHandlerFunc,
                                            this,
                                            ppiSignal,
                                            ppiSignalCtl));

   return AEE_SUCCESS;
}

AEEResult DSSSocket::RegEvent(sint31 eventMask)
{
   ds::Sock::SocketEventType IDSEventId = ds::Sock::SocketEvent::QDS_EV_WRITE;
   ds::Sock::ISocketExt *pIDSExtSokect = NULL;
   AEEResult res = AEE_SUCCESS;
   DSSCritScope cs(*mpCritSect);
   IQI*   pRegObj = NULL;

   LOG_MSG_FUNCTION_ENTRY_2("DSSSocket::RegEvent(): "
                            "RegEvent(): eventMask: %d, sockFd %d",
                            eventMask, mSockFd);
   
   res = mpIDSSock->QueryInterface(ds::Sock::AEEIID_ISocketExt, (void**)&(pIDSExtSokect));
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSSocket::RegEvent(): "
                      "QueryInterface(ds::Sock::AEEIID_ISocketExt) failed: %d",
                      res);
      goto bail;
   }

   for (uint32 eventId = DS_WRITE_EVENT; eventId <= DS_ACCEPT_EVENT; eventId <<= 1) {
      if (0 != (eventId & eventMask)) {
         // the logic should be performed even if we already registered to maintain backward compatibility, 
         // see implementations of dss_async_select for more details
         mSigOnOffMask |= eventId;
         IDS_ERR_RET(DSSConversion::DS2IDSEventId(eventId, &IDSEventId));

         /* Double pointer is used here and in the Signal pointer to set
            the corresponding member variable of DSSSocket when the signal
            is created. */
         ISignalCtl** ppiSignalCtl = GetSignalCtl(IDSEventId);
         if (NULL == ppiSignalCtl) {
            DATA_ERR_FATAL("DSSSocket::RegEvent could not get Signal Control");
            goto bail;
         }
            
         /* If the signalctl is null - this is the first registration.
            Create the signal and register it with the socket.
            Otherwise just enable the already registered signal. */
         if (NULL == *ppiSignalCtl) {
            ISignal** ppiSignal = GetSignal(IDSEventId);
            SignalHandlerCallBack eventHandlerFunc = GetEventHandlerFunc(eventId);
            res = CreateSignal(ppiSignalCtl, ppiSignal, eventHandlerFunc);
            if (AEE_SUCCESS != res) {
               goto bail;
            }

            res = pIDSExtSokect->RegEvent(*ppiSignal, IDSEventId, &pRegObj);
            // regObj mechanism is currently not in effect by dssock layer. 
            // No need for DSS to hold the allocated object to maintain the event registration. 
            // For design simplicity we release the regObj immediately.
            // If and when dssock layer enforces the registration object concept this code need to be adapted accordingly.
            DSSCommon::ReleaseIf(&pRegObj);
            if (AEE_SUCCESS != res) {
               LOG_MSG_ERROR_1("DSSSocket::RegEvent(): "
                               "RegEvent operation failed: %d", res);
               mSigOnOffMask &= ~eventId;
               goto bail;
            }
         }
         else {
            (void)(*ppiSignalCtl)->Enable();
         }
      }//(0 != (eventId & eventMask))
   }//for

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSExtSokect);
   return res;
}

AEEResult DSSSocket::DeRegEvent(sint31 eventMask)
{
    ds::Sock::SocketEventType IDSEventId = ds::Sock::SocketEvent::QDS_EV_WRITE;

   LOG_MSG_FUNCTION_ENTRY_2("DSSSocket::DeRegEvent(): "
                            "DeRegEvent(): eventMask: %d sockFd: %d",
                            eventMask, mSockFd);
   DSSCritScope cs(*mpCritSect);

   for (uint32 eventId = DS_WRITE_EVENT; eventId <= DS_ACCEPT_EVENT; eventId <<= 1) {
      if (0 != (eventId & eventMask)) {
         // The event is in the mask
         mSigOnOffMask &= ~eventId;

         IDS_ERR_RET(DSSConversion::DS2IDSEventId(eventId, &IDSEventId));

         ReleaseSignalAndSignalCtl(IDSEventId);

         // Note: Signal is not released in old app model.
      }
   }

   return AEE_SUCCESS;
}


AEEResult DSSSocket::SetSdbAckCb(dss_so_sdb_ack_cb_type *pSdbAckCb)
{
   DSSCritScope cs(*mpCritSect);
   ds::Sock::ISocketExt *pIDSExtSocket = NULL;
   AEEResult res = AEE_SUCCESS;
   ISignalCtl** ppiSignalCtl = NULL;
   ISignal** ppiSignal = NULL;
   SignalHandlerCallBack eventHandlerFunc = NULL;
   IQI*   pRegObj = NULL;
   
   res = mpIDSSock->QueryInterface(ds::Sock::AEEIID_ISocketExt, (void**)&(pIDSExtSocket));
   if (AEE_SUCCESS != res) {

      LOG_MSG_ERROR_1("DSSSocket::SetSdbAckCb(): "
                      "QueryInterface(ds::Sock::AEEIID_ISocketExt) failed: %d",
                      res);
     
      goto bail;
   }
   /* Double pointer is used here and in the Signal pointer to set
   the corresponding member variable of DSSSocket when the signal
   is created. */
   ppiSignalCtl = GetSignalCtl(ds::Sock::SocketEvent::QDS_EV_DOS_ACK);
   if (NULL == ppiSignalCtl) {

      DATA_ERR_FATAL("DSSSocket::SetSdbAckCb could not get Signal Control");
      goto bail;
   }

   /* If the SignalCtl is null - this is the first registration.
   Create the signal and register it with the socket.
   Otherwise just enable the already registered signal. */
   if (NULL == *ppiSignalCtl) {
      ppiSignal = GetSignal(ds::Sock::SocketEvent::QDS_EV_DOS_ACK);

      eventHandlerFunc = GetEventHandlerFunc(ds::Sock::SocketEvent::QDS_EV_DOS_ACK);
      
      res = CreateSignal(ppiSignalCtl, ppiSignal, eventHandlerFunc);

   if (AEE_SUCCESS != res) {
         
         LOG_MSG_ERROR_0("DSSSocket::SetSdbAckCb could not create a Signal/Signal Control");
         
         goto bail;
      }

      LOG_MSG_INFO1_1("DSSSocket::SetSdbAckCb(): "
                      "Registering to QDS_EV_DOS_ACK, SocketExt obj 0x%p",
                      pIDSExtSocket);

      res = pIDSExtSocket->RegEvent(mpSdbAckSignal, ds::Sock::SocketEvent::QDS_EV_DOS_ACK, &pRegObj);
      // regObj mechanism is currently not in effect by dssock layer. 
      // No need for DSS to hold the allocated object to maintain the event registration. 
      // For design simplicity we release the regObj immediately.
      // If and when dssock layer enforces the registration object concept this code need to be adapted accordingly.
      DSSCommon::ReleaseIf(&pRegObj);
      if (AEE_SUCCESS != res) {
         goto bail;
      }

      mSdbAckCb = *pSdbAckCb;
   } 
   else {
      (void)(*ppiSignalCtl)->Enable();
   }

/* fall through */

bail:

   DSSCommon::ReleaseIf((IQI**)&pIDSExtSocket);

   return res;
}

// TODO: documentation
void DSSSocket::ReadSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam)
{
   LOG_MSG_FUNCTION_ENTRY_0("DSSSocket::ReadSignalFcn(): "
                            "ReadSignalFcn()");
   static_cast<DSSSocket*>(pSignalHandlerBaseParam)->GeneralSignalFcn(DS_READ_EVENT);
}

// TODO: documentation
void DSSSocket::WriteSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam)
{
   LOG_MSG_FUNCTION_ENTRY_0("DSSSocket::WriteSignalFcn(): "
                            "WriteSignalFcn()");
   static_cast<DSSSocket*>(pSignalHandlerBaseParam)->GeneralSignalFcn(DS_WRITE_EVENT);
}

// TODO: documentation
void DSSSocket::CloseSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam)
{
   // We don't need to delete the DSSSocket, since we assume that the application will call
   // again to dss_close if this event was received
   LOG_MSG_FUNCTION_ENTRY_0("DSSSocket::CloseSignalFcn(): "
                            "CloseSignalFcn()");
   static_cast<DSSSocket*>(pSignalHandlerBaseParam)->GeneralSignalFcn(DS_CLOSE_EVENT);
}

// TODO: documentation
void DSSSocket::AcceptSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam)
{
   LOG_MSG_FUNCTION_ENTRY_0("DSSSocket::AcceptSignalFcn(): "
                            "AcceptSignalFcn()");
   static_cast<DSSSocket*>(pSignalHandlerBaseParam)->GeneralSignalFcn(DS_ACCEPT_EVENT);
}

// TODO: documentation
void DSSSocket::StaticSdbAckSignalFcn(SignalHandlerBase* pSignalHandlerBaseParam)
{
   LOG_MSG_FUNCTION_ENTRY_0("DSSSocket::StaticSdbAckSignalFcn(): "
                            "StaticSdbAckSignalFcn()");
   static_cast<DSSSocket*>(pSignalHandlerBaseParam)->SdbAckSignalFcn();
}

// TODO: documentation
void DSSSocket::SdbAckSignalFcn()
{
   ds::Sock::DoSAckStatusType newAPIStatus;
   dss_sdb_ack_status_enum_type oldAPIStatus;
   dss_sdb_ack_status_info_type statusInfo;
   int overflow;
   AEEResult res = AEE_SUCCESS;
   ds::Sock::ISocketExt *pIDSExtSokect = NULL;

   res = mpIDSSock->QueryInterface(ds::Sock::AEEIID_ISocketExt, (void**)&pIDSExtSokect);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSSocket::SdbAckSignalFcn(): "
                      "QueryInterface(AEEIID_ISocketExt) failed: %d", res);
      goto bail;
   }

   res = pIDSExtSokect->GetDoSAckInfo(&newAPIStatus, &overflow);
   if (AEE_SUCCESS != res){
      LOG_MSG_ERROR_0("DSSSocket::SdbAckSignalFcn(): "
                      "GetDoSAckInfo Failed");
      goto bail;
   }

   res = DSSConversion::IDS2DSSDBAckStatus(newAPIStatus, &oldAPIStatus);
   if (AEE_SUCCESS != res){
      LOG_MSG_ERROR_0("DSSSocket::SdbAckSignalFcn(): "
                      "Conversion of AckStatus Failed");
      goto bail;
   }

   if(mSdbAckCb.sdb_ack_cb == NULL) 
   {
      LOG_MSG_INFO1_0("DSSSocket::SdbAckSignalFcn(): "
                      "NULL mSdbAckCb");
   }
   else
   {
      statusInfo.overflow = overflow;
      statusInfo.status = oldAPIStatus;
      mSdbAckCb.sdb_ack_cb(mSockFd, &statusInfo, mSdbAckCb.user_data);
   }

   (void)mpSdbAckSignalCtl->Enable();

bail:

   DSSCommon::ReleaseIf((IQI**)&pIDSExtSokect);
}

void * DSSSocket::operator new
(
  unsigned int numBytes
) throw()
{
  return ps_mem_get_buf(PS_MEM_DSAL_SOCKET_TYPE);
} /* DSSSocket::operator new() */

DSSSocket * DSSSocket::CreateInstance(dss_sock_cb_fcn_type* sockCb, sint15 netApp)
{
   return new DSSSocket(sockCb, netApp);
}

