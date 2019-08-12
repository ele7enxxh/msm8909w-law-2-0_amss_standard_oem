/*===========================================================================
  FILE: DS_Net_NetworkActive.cpp

  OVERVIEW: This file provides implementation of the NetworkActive class.

  DEPENDENCIES: None

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkActive.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-02-04 svj New DIAG event to generate data cal status information
  2010-09-13 ea  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Net_NetworkActive.h"
#include "ds_Net_Platform.h"
#include "ds_Net_INetworkFactory.h"
#include "ps_policy_mgr.h"


using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
NetworkActive::NetworkActive
(
  Policy *    argPolicyPtr
) : Network(argPolicyPtr, NetworkMode::QDS_ACTIVE)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkActive::NetworkActive(): Obj 0x%p", this);

} /* NetworkActive::NetworkActive() */

/*---------------------------------------------------------------------------
  Functions inherited from Network
---------------------------------------------------------------------------*/
ds::ErrorType NetworkActive::Stop
(
  void
)
{
  ds::ErrorType  result = AEE_SUCCESS;
  int32 ifaceHandle = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkActive::Stop(): Obj 0x%p", this);

  mpICritSect->Enter();

  do
  {
    ifaceHandle = GetHandle();

    /*-----------------------------------------------------------------------
      If net handle associated with this object is NULL, return SUCCESS
    -----------------------------------------------------------------------*/
    if (0 == ifaceHandle)
    {
      LOG_MSG_INFO1_1 ("NetworkActive::Stop(): NULL handle, obj 0x%p", this);
      break;
    }

    if (FALSE == mBringupFirst)
    {
      LOG_MSG_INFO1_2 ("NetworkActive::Stop(): Iface 0x%x not brought by obj %p",
                       ifaceHandle, this);
      break;
    }

    /*-----------------------------------------------------------------------
      If net handle associated with this object is NULL, or if teardown
      has been called already, return SUCCESS
    -----------------------------------------------------------------------*/
    if (TRUE == mTeardown)
    {
      LOG_MSG_INFO1_1 ("NetworkActive::Stop(): teardown already called, "
                       "obj 0x%p", this);
      break;
    }

    mTeardown = TRUE;

    /*-----------------------------------------------------------------------
      Call tear_down_cmd on the network interface.
    -----------------------------------------------------------------------*/
    result = IfaceTearDownCmdEx (ifaceHandle,
                                 static_cast <IPolicy *>(mpPolicy),  
                                 NULL);
    if (AEE_SUCCESS == result)
    {
      /*---------------------------------------------------------------------
        Cache previous state, reset handle and notify application.
      ---------------------------------------------------------------------*/
      (void) GetPreviousState (&mCachedPreviousState);

      SetHandle (0);
      
      // Release 1X network if exist, since handle is now 0
      if (0 != mpTechObjNetwork1x)
      {
        DS_UTILS_RELEASEIF (mpTechObjNetwork1x);

        mpTechObjNetwork1xPriv = 0;
      }
      
      Notify (NetworkEvent::QDS_EV_IFACE_DOWN);
    }
  } while (0);

  mpICritSect->Leave();

  /*-------------------------------------------------------------------------
    Return SUCCESS always.
  -------------------------------------------------------------------------*/
  return AEE_SUCCESS;

} /* NetworkActive::Stop() */

ds::ErrorType NetworkActive::BringUpInterface
(
  void
)
{
  ds::ErrorType     result = AEE_SUCCESS;
  int32             ifaceHandle = -1;
  NetworkStateType  netState = NetworkState::QDS_CLOSED;
  EventType         eventId = -1;
  ps_iface_net_down_reason_type net_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkActive::BringUpInterface(): Obj 0x%p",
                            this);

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Bringup can be performed only once. For repeat calls, return error code
      depending upon current iface state. Retrieve the object variable using
      object's cirtical section.
    -----------------------------------------------------------------------*/
    ifaceHandle = GetHandle();

    if (TRUE == mBringupFirst) {
      LOG_MSG_INFO1_1("NetworkActive::BringUpInterface(): "
        "Modem application did not wait for event DS_ENETNONET/DS_ENETISCONN obj 0x%p", this);
          result = QDS_EINVAL;
          break;
        }

    /*-----------------------------------------------------------------------
      Set the first-time-bring-up flag to true. Reset the mTeardown flag, so
      network can be brought down again
    -----------------------------------------------------------------------*/
    mTeardown     = FALSE;
    mBringupFirst = TRUE;

    /*-----------------------------------------------------------------------
      Perform Bringup.
    -----------------------------------------------------------------------*/
    result = IfaceBringUpByPolicy (static_cast <IPolicy *>(mpPolicy),
                                   &ifaceHandle,
                                   &mAppPriority,
                                   &net_down_reason);
      if (AEE_EWOULDBLOCK == result) {
        mBringupAgain = TRUE;
        mWouldblock = TRUE;
    }

    /*-----------------------------------------------------------------------
      Failure case
    -----------------------------------------------------------------------*/
    if (QDS_ENETCLOSEINPROGRESS != result &&
        AEE_EWOULDBLOCK != result &&
        AEE_SUCCESS != result)
    {
      LOG_MSG_INFO1_1("NetworkActive::BringUpInterface(): "
                      "Failed to bring up iface, obj 0x%p", this);
      mBringupFirst = FALSE;
      mLastNetDownReason = net_down_reason;
      break;
    }

    LOG_MSG_INFO1_2("NetworkActive::BringUpInterface(): "
                    "Obtained handle 0x%x, obj 0x%p", ifaceHandle, this);

    SetHandle(ifaceHandle);

    if (AEE_SUCCESS == result)
    {
      (void) GetIfaceState (&netState);
      eventId = NetworkState2Event (netState);

      Notify (eventId);
    }
    else if (QDS_ENETCLOSEINPROGRESS == result)
    {
      /*---------------------------------------------------------------------
        QDS_ENETCLOSEINPROGRESS needs to be translated to AEE_EWOULDBLOCK.
        We need to bring up the interface after iface down.
      ---------------------------------------------------------------------*/
      result        = AEE_EWOULDBLOCK;
      mBringupAgain = TRUE;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_6 ("NetworkActive::BringUpInterface(): "
                           "Result 0x%x, handle 0x%x, obj 0x%p, "
                           "mBringupFirst %d, mBringupAgain %d, mTeardown %d",
                           result, ifaceHandle, this, 
                           mBringupFirst, mBringupAgain, mTeardown);
  mpICritSect->Leave();
  return result;

} /* NetworkActive::BringUpInterface() */

void NetworkActive::ProcessIfaceStateEvent
(
  EventInfoType *  eventInfoPtr
)
{
  NetworkStateType        netState = NetworkState::QDS_CLOSED;
  ds::ErrorType           result = AEE_SUCCESS;
  int32                   ifaceHandle = -1;
  int32                   newIfaceHandle = -1;
  ps_iface_net_down_reason_type net_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == eventInfoPtr)
  {
    LOG_MSG_ERROR_1 ("NetworkActive::ProcessIfaceStateEvent(): Obj 0x%p",
                     this);
    return;
  }

  mpICritSect->Enter();

  do
  {
    ifaceHandle = GetHandle();

    /*-----------------------------------------------------------------------
      Process the event only if the event occurred on this handle
    -----------------------------------------------------------------------*/
    if (ifaceHandle != eventInfoPtr->handle)
    {
      LOG_MSG_INFO3_4 ("NetworkActive::ProcessIfaceStateEvent(): "
                       "Handle 0x%x != event handle 0x%x, event %d, obj 0x%p",
                       ifaceHandle,
                       eventInfoPtr->handle,
                       eventInfoPtr->psEventName,
                       this);
      break;
    }

    LOG_MSG_INFO1_6 ("NetworkActive::ProcessIfaceStateEvent(): "
                     "obj 0x%p, Handle 0x%x, event %d, "
                     "mBringupFirst %d, mBringupAgain %d, mTeardown %d",
                     this,
                     ifaceHandle,
                     eventInfoPtr->psEventName,
                     mBringupFirst, mBringupAgain, mTeardown);
    /*-----------------------------------------------------------------------
      Cache the previous state for any iface state change notifications.
    -----------------------------------------------------------------------*/
    (void) GetIfaceState (&netState);
    (void) GetPreviousState (&mCachedPreviousState);

    if ((IFACE_UP_EV == eventInfoPtr->psEventName) || (IFACE_COMING_UP_EV == eventInfoPtr->psEventName)) {
      if (TRUE == mWouldblock) {
        mBringupAgain = FALSE;
        mWouldblock = FALSE;
      }
    }

    //MSR Check if ROUTEABLE_EV is needed. Also check other events

    /*-----------------------------------------------------------------------
      Ignore IFACE_ROUTEABLE_EV always.
    -----------------------------------------------------------------------*/
    if ((int32) IFACE_ROUTEABLE_EV == eventInfoPtr->psEventName)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Ignore CLOSE_IN_PROGRESS event when mBringupAgain flag is set.
    -----------------------------------------------------------------------*/
    if (TRUE == mBringupAgain &&
        NetworkState::QDS_CLOSE_IN_PROGRESS == netState)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Ignore any event other then the below when mBringupAgain flag is set.
        NetworkEvent::QDS_EV_IFACE_DOWN
        IFACE_ACTIVE_OUT_OF_USE_EV
    -----------------------------------------------------------------------*/
    if (TRUE == mBringupAgain &&
        (!(NetworkEvent::QDS_EV_IFACE_DOWN == eventInfoPtr->eventName) ||
          (static_cast<int32>(IFACE_ACTIVE_OUT_OF_USE_EV) == eventInfoPtr->psEventName))
        )
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Reset the handle if the state is closed. Also, treat
      IFACE_ACTIVE_OUT_OF_USE_EV as IFACE_DOWN event.
    -----------------------------------------------------------------------*/
    if (NetworkEvent::QDS_EV_IFACE_DOWN == eventInfoPtr->eventName ||
        static_cast<int32>(IFACE_ACTIVE_OUT_OF_USE_EV) == eventInfoPtr->psEventName)
    {
      if (FALSE == mBringupAgain)
      {
        /*-------------------------------------------------------------------
          Reset the handle as the state is closed.
        -------------------------------------------------------------------*/
        SetHandle (0);
      }
      else
      {
        /*-------------------------------------------------------------------
          BringupFlag processing.
          Reset bringup flag on CLOSE event.
          Ignore (Dont post event to app) for QDS_CLOSED and CLOSE_IN_PROG
          events.
        -------------------------------------------------------------------*/
        mTeardown     = FALSE;
        mBringupAgain = FALSE;
        mWouldblock   = FALSE;

        /*-------------------------------------------------------------------
          Bringup network again based on policy. If same network cannot be
          brought up, the tear it down and set handle to 0. Reset
          the mTeardown flag, so network can be brought down again

                                 IfaceBringUpByPolicy()
                                /                       \
                            SUCESS                     FAILURE
                      /               \                       \
              ifaceHandle ==       ifaceHandle !=         1.SetHandle(0);
              newIfaceHandle       newIfaceHandle         2.Notify(STATE);

              1.NO-OP, return;    1.SetHandle(0);
                                  2.Notify(STATE);
                                  3.IfaceTearDownCmd(newHandle)
        -------------------------------------------------------------------*/
        result = IfaceBringUpByPolicy (static_cast <IPolicy *> (mpPolicy),
                                       &newIfaceHandle,
                                       &mAppPriority,
                                       &net_down_reason);

        //TODO Handle CLOSEINPROG again
        if (AEE_SUCCESS == result || AEE_EWOULDBLOCK == result)
        {
          // Once app calls dss_pppopen(), it can call dss_get_ifaceid() to get iface ID.
          // DSS verifies that the app is bound to this iface ID until app calls dss_pppclose().
          // Since first byte of iface ID is an index to a global iface ptr array,
          // two different ifaces will have two different iface IDs.
          // So, when DSNet silently brings up call for the second time
          // (see call to IfaceBringUpByPolicy above),
          // it verifies that the iface is the same as what it used previously.
          // Note: It is legal for the app to call dss_get_ifaceid()
          // BEFORE getting an IFACE_UP event.
          if (newIfaceHandle != ifaceHandle)
          {
            LOG_MSG_INFO1_3 ("NetworkActive::ProcessIfaceStateEvent(): "
                             "newIfaceHandle (0x%h) != ifaceHandle (0x%h) obj 0x%p",
                             newIfaceHandle, ifaceHandle, this);

            SetHandle (0);

            Notify (eventInfoPtr->eventName);
            (void) IfaceTearDownCmdEx (newIfaceHandle,
                                       static_cast <IPolicy *>(mpPolicy),
                                       NULL);
          }
        }
        else
        {
          SetHandle (0);
          mLastNetDownReason = net_down_reason;
          Notify (eventInfoPtr->eventName);
        }

        break;
      }
    }

    if (0 == GetHandle())
    {
      // Release 1X network if exist, since handle is now 0
      if (0 != mpTechObjNetwork1x)
      {
        DS_UTILS_RELEASEIF (mpTechObjNetwork1x);

        mpTechObjNetwork1xPriv = 0;
      }
    }

    /*-----------------------------------------------------------------------
      Notify event to application.
    -----------------------------------------------------------------------*/
    Notify (eventInfoPtr->eventName);
  } while (0);

  mpICritSect->Leave();
  return;

} /* NetworkActive::ProcessIfaceStateEvent() */

ds::ErrorType NetworkActive::NotifyNetDown2Monitored
(
 void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EINVAL;
} /* NetworkActive::NotifyNetDown2Monitored() */

void NetworkActive::Destructor
(
  void
)
 throw()
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkActive::Destructor(): Obj 0x%p", this);

  if (0 == mpICritSect)
  {
    LOG_MSG_ERROR_1("Network::Destructor(): Obj 0x%p "
                    "Network wasn't fully initialized", this);
    return;
  }

  mpICritSect->Enter();

  Network::Destructor();

  mpICritSect->Leave();
} /* NetworkActive::Destructor() */

NetworkActive::~NetworkActive
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkActive::~NetworkActive(): Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* NetworkActive::~NetworkActive() */


ds::ErrorType NetworkActive::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("NetworkActive::GetSignalBus(): Obj 0x%p, event 0x%x",
                            this, eventID);

  if (0 == ppISigBus)
  {
    LOG_MSG_INVALID_INPUT_1 ("NetworkActive::GetSignalBus(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  return Network::GetSignalBus(eventID, ppISigBus);
}
