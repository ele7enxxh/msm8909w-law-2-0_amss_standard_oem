/*===========================================================================
FILE: ds_Sock_NetworkCtl.cpp

OVERVIEW:
  This file provides partial implementation of the NetworkCtl class.

DEPENDENCIES: None

Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_NetworkCtl.cpp#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-01-05 vm Created module

===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ds_Sock_NetworkCtl.h"
#include "ds_Utils_ISignalCtl.h"
#include "ds_Utils_ISignal.h"
#include "ds_Net_Platform.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Utils_CSignalFactory.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"
#include "ds_Net_CreateInstance.h"


using namespace ds::Net;
using namespace ds::Sock;
using namespace ds::Error;
using namespace ds;

/*===========================================================================

LOCAL DATA DEFINITIONS

===========================================================================*/

/*===========================================================================

PUBLIC CLASS METHODS

===========================================================================*/

NetworkCtl::NetworkCtl
(
  void
)  : networkPtr(0),
     policyPtr(0),
     ifaceUpSignalCtlPtr(0),
     ifaceDownSignalCtlPtr(0),
     clientIfaceUpSignalPtr(0),
     clientIfaceDownSignalPtr(0),     
     critSectPtr(0)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* NetworkCtl::NetworkCtl() */

ds::ErrorType NetworkCtl::Init
(
  ICritSect* critSect
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == critSect)
  {
    LOG_MSG_ERROR_0 ("NetworkCtl::Init(): "
                     "NULL critSectPtr");
    return QDS_EBADF;
  }

  // use the same critical section as the socket
  critSectPtr = critSect;
  (void) critSectPtr->AddRef();
  
  return AEE_SUCCESS;
}/* NetworkCtl::Init() */

ds::ErrorType NetworkCtl::CloneMembers
(
  NetworkCtl*        *networkCtlPtr
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(0 != policyPtr)
  {
    (*networkCtlPtr)->policyPtr = policyPtr;
    (void) policyPtr->AddRef();
  }

  if(0 != networkPtr)
  {
    (*networkCtlPtr)->networkPtr = networkPtr;
    (void) networkPtr->AddRef();
    result = (*networkCtlPtr)->RegNetworkStateChangedEvent();
  }

  return result;
}/* NetworkCtl::CloneMembers */

boolean NetworkCtl::IsNetworkBound
(
  void
)
{
  boolean ret = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret = (0 != networkPtr);
  
  LOG_MSG_FUNCTION_EXIT_3( "NetworkCtl::IsNetworkBound(): "
                           "obj 0x%x, networkPtr 0x%x ret 0x%x", this, networkPtr, ret);
  return ret;
}/* NetworkCtl::IsNetworkBound() */

ds::ErrorType NetworkCtl::SetNetPolicy
(
  IPolicy                *newPolicyPtr
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == newPolicyPtr)
  {
    LOG_MSG_ERROR_1("NetworkCtl::SetNetPolicy(): "
                    "newPolicyPtr is null", 	 0);
    result = QDS_EFAULT;
    goto bail;
  }

  /*-------------------------------------------------------------------------
  Decrement refCnt of current policy object since it is not cached any more
  -------------------------------------------------------------------------*/
  DS_UTILS_RELEASEIF(policyPtr);

  /*-------------------------------------------------------------------------
  Cache policy and increment refCnt
  -------------------------------------------------------------------------*/
  policyPtr = newPolicyPtr;

  (void) policyPtr->AddRef();

  result = AEE_SUCCESS;

/* fall through */

bail:
  LOG_MSG_FUNCTION_EXIT_2( "NetworkCtl::SetNetPolicy(): "
                           "obj 0x%x, newPolicyPtr 0x%x", this, newPolicyPtr);

  return result;
}/* NetworkCtl::SetNetPolicy() */

ds::ErrorType NetworkCtl::UpdateRouteScope
(
  ps_route_scope_type    *routeScope
)
{
  ds::ErrorType   dsErrno = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // Only update route scope when network is not bound. TBD
  // Need to clarify this, IsNetworkBound should not be releated to route scope updates.
  if (IsNetworkBound() || 0 == routeScope)
  {
    dsErrno = QDS_EFAULT;
  }
  else
  {
    dsErrno = NetPlatform::GetRouteScopeByPolicy(policyPtr, routeScope);
  }

  LOG_MSG_FUNCTION_EXIT_3( "NetworkCtl::UpdateRouteScope(): "
                           "obj 0x%x, routeScope 0x%x dsErrno 0x%x", this, routeScope, dsErrno);

  return dsErrno;
}/* NetworkCtl::UpdateRouteScope() */

ds::ErrorType NetworkCtl::UpdateNetwork
(
  ds::Net::INetwork *dsNetObjPtr
)
{
  INetworkPriv    *networkPrivObjPtr = 0;
  ds::ErrorType   dsErrno = AEE_SUCCESS;

  LOG_MSG_FUNCTION_ENTRY_4( "NetworkCtl::UpdateNetwork(): "
                            "obj 0x%x, dsNetObjPtr 0x%x NetworkPtr 0x%x policyPtr 0x%x ", 
                            this, dsNetObjPtr, networkPtr, policyPtr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == dsNetObjPtr)
  {
    return QDS_EFAULT;
  }
  /*-------------------------------------------------------------------------
    It is invalid to associate a controller with a INetwork object
    if controller is already associated with one
  -------------------------------------------------------------------------*/
  if (0 != networkPtr)
  {
    dsErrno = QDS_EOPNOTSUPP;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    It is invalid to associate a controller with a IPolicy object
    if controller is already associated with one
  -------------------------------------------------------------------------*/
  if(0 != policyPtr)
  {
    dsErrno = QDS_EOPNOTSUPP;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Fetch networkPriv object associated with dsNetObjPtr
    networkPriv's refCnt is incremented by dsNetObjPtr->QueryInterface()
  -------------------------------------------------------------------------*/
  dsErrno = dsNetObjPtr->QueryInterface
    (
    AEEIID_INetworkPriv,
    reinterpret_cast <void **> ( &networkPrivObjPtr)
    );

  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_1( "NetworkCtl::UpdateNetwork(): "
                     "QueryInterface:dsErrno 0x%x", dsErrno);
    goto bail;
  }

  dsErrno = networkPrivObjPtr->GetPolicy(&policyPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_1( "NetworkCtl::UpdateNetwork(): "
                     "GetPolicy:dsErrno 0x%x", dsErrno);
    goto bail;
  }

  DS_UTILS_RELEASEIF(networkPrivObjPtr);

  /*-------------------------------------------------------------------------
    Cache INetwork object and increment refCnt
  -------------------------------------------------------------------------*/
  networkPtr = dsNetObjPtr;
  (void) networkPtr->AddRef();

  /*-------------------------------------------------------------------------
    Register with network object for state change events. This event is used
    to take appropriate action as network state changes. For example, connected
    TCP sockets are closed once network object goes DOWN
  -------------------------------------------------------------------------*/
  dsErrno = RegNetworkStateChangedEvent();
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_1( "NetworkCtl::UpdateNetwork(): "
                     "RegNetworkStateChangedEvent:dsErrno 0x%x", dsErrno);
    goto bail;
  }

  return AEE_SUCCESS;

bail:
  DS_UTILS_RELEASEIF(networkPrivObjPtr);
  DS_UTILS_RELEASEIF(networkPtr);

  return dsErrno;
}/* NetworkCtl::UpdateNetwork() */

ds::ErrorType NetworkCtl::SetNetwork
(
  ds::Net::INetwork *dsNetObjPtr
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(0 == policyPtr)
  {
    result = UpdateNetwork(dsNetObjPtr);
  }
  else
  {
    LOG_MSG_ERROR_1( "NetworkCtl::SetNetwork(): "
                     "policyPtr not null:policyPtr 0x%x", policyPtr);
    result = QDS_EOPNOTSUPP;
  }

  LOG_MSG_FUNCTION_EXIT_3( "NetworkCtl::SetNetwork(): "
                           "obj 0x%x, Net obj 0x%x result 0x%x",
                            this, dsNetObjPtr, result);

  return result;
}/* NetworkCtl::SetNetwork() */


void NetworkCtl::Destructor
(
  void
)
throw()
{
  ::ICritSect  *localCritSectPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == critSectPtr)
  {
    return;
  }

  localCritSectPtr = critSectPtr;
  localCritSectPtr->Enter();

  DS_UTILS_RELEASEIF(networkPtr);
  DS_UTILS_RELEASEIF(policyPtr);
  DS_UTILS_RELEASEIF(ifaceUpSignalCtlPtr);
  DS_UTILS_RELEASEIF(ifaceDownSignalCtlPtr);
  DS_UTILS_RELEASEIF(clientIfaceUpSignalPtr);
  DS_UTILS_RELEASEIF(clientIfaceDownSignalPtr);

  SignalHandlerBase::Destructor();

  /*-------------------------------------------------------------------------
    Perform reset of object's critSectPtr within critical section.
  -------------------------------------------------------------------------*/
  critSectPtr = 0;

  localCritSectPtr->Leave();
  DS_UTILS_RELEASEIF(localCritSectPtr);

}/* NetworkCtl::Destructor() */

ds::ErrorType  NetworkCtl::QueryInterface
(
  AEEIID   iid,
  void **  objPtrPtr
)
throw()
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == objPtrPtr)
  {
    LOG_MSG_ERROR_0( "NetworkCtl::QueryInterface(): "
                     "objPtrPtr null");

    return QDS_EFAULT;
  }
  switch (iid)
  {
  case AEEIID_IQI:
    {
      //TODO Fix this. fails if static_cast is used coz IQI is present from
      //multiple base interfaces
      *objPtrPtr = reinterpret_cast <IQI *> ( this);
      break;
    }

  default:
    {
      LOG_MSG_INVALID_INPUT_2("NetworkCtl::QueryInterface(): "
                              "Unknown iid 0x%x, sock 0x%x", iid, this);
      dsErrno = AEE_ECLASSNOTSUPPORT;
      goto bail;
    }
  } /* switch (iid) */

  (void) AddRef();

  LOG_MSG_FUNCTION_EXIT_2( "NetworkCtl::QueryInterface(): "
                           "NetworkCtl Sock 0x%x iid 0x%x", this, iid);

  return AEE_SUCCESS;

bail:

  return dsErrno;
}/* NetworkCtl::QueryInterface() */

ds::ErrorType NetworkCtl::RegNetworkStateChangedEvent
(
  void
)
{
  ISignalFactory      *signalFactoryPtr = 0;
  ds::ErrorType       dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(0 == networkPtr)
  {
    dsErrno = QDS_EFAULT;
    goto bail;
  }

  dsErrno = DS_Utils_CreateInstance(
                                     0,
                                     AEECLSID_CSignalFactory,
                                     reinterpret_cast <void **> ( &signalFactoryPtr)
                                   );
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_0( "NetworkCtl::RegNetworkStateChangedEvent(): "
                     "CreateInstance failed");
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  // register QDS_EV_IFACE_UP
  dsErrno = RegSignal2Event
    (
      signalFactoryPtr,
      NetworkEvent::QDS_EV_IFACE_UP,
      EventCallbackIfaceUp,
      &ifaceUpSignalCtlPtr
    );
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_1( "NetworkCtl::RegNetworkStateChangedEvent(): "
                     "register QDS_EV_IFACE_UP failed:dsErrno 0x%x", dsErrno);
    goto bail;
  }

  // register QDS_EV_IFACE_DOWN
  dsErrno = RegSignal2Event
    (
      signalFactoryPtr,
      NetworkEvent::QDS_EV_IFACE_DOWN,
      EventCallbackIfaceDown,
      &ifaceDownSignalCtlPtr
    );
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_1( "NetworkCtl::RegNetworkStateChangedEvent(): "
                     "register QDS_EV_IFACE_DOWN failed:dsErrno 0x%x",
                     dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Delete SignalFactory object. Could have created a SignalFactory
    object at power up and not ever delete it but this object is not needed
    most of the time and by deleting the object, less memory is consumed
  -------------------------------------------------------------------------*/
  DS_UTILS_RELEASEIF(signalFactoryPtr);

  LOG_MSG_FUNCTION_EXIT_1( "NetworkCtl::RegNetworkStateChangedEvent(): "
                           " NetworkCtl obj 0x%x", this);

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Release all objects
  -------------------------------------------------------------------------*/
bail:
  DS_UTILS_RELEASEIF(ifaceUpSignalCtlPtr);
  DS_UTILS_RELEASEIF(ifaceDownSignalCtlPtr);
  DS_UTILS_RELEASEIF(signalFactoryPtr);

  return dsErrno;
} /* NetworkCtl::RegNetworkStateChangedEvent() */

ds::ErrorType NetworkCtl::RegSignal2Event
(
  ISignalFactory        *signalFactoryPtr,
  ds::Net::EventType    eventType,
  SignalHandlerCallBack signalHandlerCallBack,
  ISignalCtl            **signalCtlPtr
)
{
  ds::ErrorType       dsErrno = AEE_SUCCESS;
  ISignal             *signalPtr = 0;
  IQI                 *pRegObj = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("NetworkCtl::RegSignal2Event(): "
                           "NetworkCtl obj 0x%x, eventType 0x%x",
                           this, eventType);
  
  dsErrno = signalFactoryPtr->CreateSignal
    (
      &signalHandler,
      signalHandlerCallBack,
      this,
      &signalPtr,
      signalCtlPtr
    );
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_1( "NetworkCtl::RegSignal2Event(): "
                     "CreateSignal failed:dsErrno 0x%x", dsErrno);
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for STATE event with network object
  -------------------------------------------------------------------------*/
  dsErrno = networkPtr->OnStateChange(signalPtr,
                                      eventType,
                                      &pRegObj);
  /*-------------------------------------------------------------------------
    regObj mechanism is currently not in effect by dsnet layer.
    No need to hold the allocated object to maintain the event registration.
    For design simplicity we release the regObj immediately.
    If and when dsnet layer enforces the registration object concept 
    this code need to be adapted accordingly.
  -------------------------------------------------------------------------*/
   DS_UTILS_RELEASEIF(pRegObj);

   /*-------------------------------------------------------------------------
    Release the Signal object as it is not not needed anymore. Enabling and
    disabling the signal can be done by using the Signal Ctrl object
  -------------------------------------------------------------------------*/
   DS_UTILS_RELEASEIF(signalPtr);

  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_1( "NetworkCtl::RegSignal2Event(): "
                     "OnStateChange failed:dsErrno 0x%x", dsErrno);
    goto bail;
  }
  
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:

  return dsErrno;
} /* NetworkCtl::RegSignal2Event() */

void NetworkCtl::EventCallbackIfaceUp
(
  SignalHandlerBase *  pSignalHandlerBaseParam
)
{
  NetworkCtl *  ctlPtr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("NetworkCtl::EventCallbackIfaceUp(): "
                           "NetworkCtl obj 0x%x", pSignalHandlerBaseParam);

  if (NULL == pSignalHandlerBaseParam)
  {
    ASSERT( 0);
    return;
  }

  ctlPtr = static_cast <NetworkCtl *> ( pSignalHandlerBaseParam);

  (ctlPtr->critSectPtr)->Enter();

  if(0 != ctlPtr->ifaceUpSignalCtlPtr)
  {
    (void) (ctlPtr->ifaceUpSignalCtlPtr)->Enable();
  }

  if(0 != ctlPtr->clientIfaceUpSignalPtr)
  {
    (void) (ctlPtr->clientIfaceUpSignalPtr)->Set();
  }

  (ctlPtr->critSectPtr)->Leave();

  return;
} /* NetworkCtl::EventCallbackIfaceUp() */

void NetworkCtl::EventCallbackIfaceDown
(
  SignalHandlerBase *  pSignalHandlerBaseParam
)
{
  NetworkCtl *  ctlPtr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("NetworkCtl::EventCallbackIfaceDown(): "
                           "NetworkCtl obj 0x%x", pSignalHandlerBaseParam);

  if (NULL == pSignalHandlerBaseParam)
  {
    ASSERT( 0);
    return;
  }

  ctlPtr = static_cast <NetworkCtl *> ( pSignalHandlerBaseParam);

  (ctlPtr->critSectPtr)->Enter();

  if(0 != ctlPtr->ifaceDownSignalCtlPtr)
  {
    (void) (ctlPtr->ifaceDownSignalCtlPtr)->Enable();
  }

  if(0 != ctlPtr->clientIfaceDownSignalPtr)
  {
    (void) (ctlPtr->clientIfaceDownSignalPtr)->Set();
  }

  (ctlPtr->critSectPtr)->Leave();

  return;
} /* NetworkCtl::EventCallbackIfaceDown() */

ds::ErrorType NetworkCtl::GetNetworkState
(
  NetworkStateType *netObjState
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(TRUE == IsNetworkBound())
  {
    result = networkPtr->GetState(netObjState);
  }
  else
  {
    LOG_MSG_ERROR_0( "NetworkCtl::GetNetworkState(): "
                     "IsNetworkBound failed");

    result = QDS_EFAULT;
  }
  LOG_MSG_FUNCTION_EXIT_2( "NetworkCtl::GetNetworkState(): "
                           "NetworkCtl obj 0x%x GetState 0x%x", this, result);

  return result;
}/* NetworkCtl::GetNetworkState() */

ds::ErrorType NetworkCtl::GetIfaceId
(
  IfaceIdType *ifaceId
)
{
  ds::ErrorType result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(TRUE == IsNetworkBound())
  {
    result = networkPtr->GetIfaceId(ifaceId);
  }
  else
  {
    LOG_MSG_ERROR_0( "NetworkCtl::GetIfaceId(): "
                     "IsNetworkBound failed");
    result = QDS_EFAULT;
  }

  LOG_MSG_FUNCTION_EXIT_2( "NetworkCtl::GetIfaceId(): "
                           "NetworkCtl obj 0x%x result: 0x%x", this, result);
  return result;
}/* NetworkCtl::GetIfaceId() */

void NetworkCtl::Close
(
  void
)
throw()
{
  critSectPtr->Enter();

  DetachSignal(&ifaceUpSignalCtlPtr);
  DetachSignal(&ifaceDownSignalCtlPtr);
  
  critSectPtr->Leave();

}/* NetworkCtl::Close() */

void NetworkCtl::DetachSignal
(
  ISignalCtl **signalCtlPtr
)
throw()
{
  if (0 != *signalCtlPtr)
  {
    (void) (*signalCtlPtr)->Detach();      /*lint !e1550 */
    DS_UTILS_RELEASEIF(*signalCtlPtr);
  }
}/* NetworkCtl::Close() */

ds::ErrorType NetworkCtl::RegisterClientSignal
(
  ISignal *clientIfaceUpSignal,
  ISignal *clientIfaceDownSignal,
  IQI**    regObj
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_3("NetworkCtl::RegisterClientSignal(): "
                           "NetworkCtl obj 0x%x, clientIfaceUpSignal 0x%x, clientIfaceDownSignal 0x%x",
                           this, clientIfaceUpSignal, clientIfaceDownSignal);

  if((0 == clientIfaceUpSignal) ||
     (0 == clientIfaceDownSignal) ||
     (0 == regObj))
  {
    LOG_MSG_ERROR_1( "NetworkCtl::RegisterClientSignal(): "
                     "IsNetworkBound failed:regObj 0x%x", regObj);
    return QDS_EFAULT;
  }
  /*-------------------------------------------------------------------------
    Due to a possible security issue where applications may invoke this
    function many times thus causing undesirable extra memory allocation that
    may be entailed in an implementation of socket events registration,
    we should return an object that the application will hold for each
    RegEvent invocation. A Limit that CS imposes on the number of objects the
    application can hold resolves the security issue.

    If the implementation of RegEvent is changed an appropriate
    object must be returned.
  -------------------------------------------------------------------------*/
  *regObj = new RegBound();

  clientIfaceUpSignalPtr = clientIfaceUpSignal;
  (void) clientIfaceUpSignalPtr->AddRef();

  clientIfaceDownSignalPtr = clientIfaceDownSignal;
  (void) clientIfaceDownSignalPtr->AddRef();
  
  
  return AEE_SUCCESS;

}/* NetworkCtl::RegisterClientSignal() */

IPolicy*  NetworkCtl::GetPolicy
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("NetworkCtl::GetPolicy(): "
                           "NetworkCtl obj 0x%x", this);
  if(0 != policyPtr)
  {
    (void) policyPtr->AddRef();
  }

  return policyPtr;
}/* NetworkCtl::GetPolicy() */

