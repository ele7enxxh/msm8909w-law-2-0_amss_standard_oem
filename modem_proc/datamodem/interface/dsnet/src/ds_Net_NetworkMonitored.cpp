/*===========================================================================
  FILE: DS_Net_NetworkMonitored.cpp

  OVERVIEW: This file provides implementation of the NetworkMonitored class.

  DEPENDENCIES: None

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_NetworkMonitored.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2010-09-13 ea  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_Net_NetworkMonitored.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace ds::Net::Conversion;
using namespace NetPlatform;

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
NetworkMonitored::NetworkMonitored
(
  Policy *    argPolicyPtr
) : Network(argPolicyPtr, NetworkMode::QDS_MONITORED),
    mpSigBusBearerTech(0),
    mpSigBusIPAddr(0),
    mpSigBusExtendedIPConfig(0),
    mpSigBusQoSAware(0),
    mpSigBusDelExtIPv6Addr(0),
    mpSigBusBearerTechEx(0),
    mpSigBusOutage(0),
    mpSigBusHandoffInit(0),
    mpSigBusHandoffSuccess(0),
    mpSigBusHandoffFailure(0),
    mpSigBusRFConditions(0),
    mpSigBusExtendedIpConfigEx(0),
    mpSigBusApnParamChange(0)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkMonitored::NetworkMonitored(): Obj 0x%p",
                            this);

} /* NetworkMonitored::NetworkMonitored() */

/*---------------------------------------------------------------------------
  Functions inherited from Network
---------------------------------------------------------------------------*/
ds::ErrorType NetworkMonitored::Stop
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EINVAL;
} /* Stop() */

ds::ErrorType NetworkMonitored::BringUpInterface
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EINVAL;
} /* BringUpInterface() */

void NetworkMonitored::ProcessIfaceStateEvent
(
  EventInfoType *  eventInfoPtr
)
{
  int32 ifaceHandle = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == eventInfoPtr)
  {
    LOG_MSG_ERROR_1 ("NetworkMonitored::ProcessIfaceStateEvent(): Obj 0x%p",
                     this);
    return;
  }

  /*-------------------------------------------------------------------------
    Ignore IFACE_ROUTEABLE_EV
  -------------------------------------------------------------------------*/
  if ((int32)IFACE_ROUTEABLE_EV == eventInfoPtr->psEventName)
  {
    return;
  }

  mpICritSect->Enter();

  do
  {
    ifaceHandle = GetHandle();

    LOG_MSG_INFO1_4 ("NetworkMonitored::ProcessIfaceStateEvent(): "
                     "Handle 0x%x, event %d, event handle 0x%x, obj 0x%p",
                     ifaceHandle,
                     eventInfoPtr->psEventName,
                     eventInfoPtr->handle,
                     this);

    /*-----------------------------------------------------------------------
      Process the event only if the event occurred on this handle
    -----------------------------------------------------------------------*/
    if (ifaceHandle != eventInfoPtr->handle)
    {
      LOG_MSG_INFO3_4 ("NetworkMonitored::ProcessIfaceStateEvent(): "
                       "Handle 0x%x != event handle 0x%x, event %d, obj 0x%p",
                       ifaceHandle,
                       eventInfoPtr->handle,
                       eventInfoPtr->psEventName,
                       this);
      break;
    }

    /*-----------------------------------------------------------------------
      Note: if the iface bound to this network object in OPEN_IN_PROGRESS
      state, we don't check if it fits our policy.
      In the case where this network is associated with a primary
      net monitored object, we may want to associate the monitored network
      with a new iface corresponding to its policy.
      Current code does not reflect this behavior.
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Cache the previous state for any iface state change notifications.
    -----------------------------------------------------------------------*/
    //TODO Cache it from event itself as by the time PS task processes the
    //event, iface cud be in use for somebody else
    (void) GetPreviousState (&mCachedPreviousState);

    /*-----------------------------------------------------------------------
      Store the last netdown reason and notify the client.

      Don't reset handle since it's a monitored network.
    -----------------------------------------------------------------------*/
    if (NetworkEvent::QDS_EV_IFACE_DOWN == eventInfoPtr->eventName)
    {
      (void) GetLastNetworkDownReason
             (
               ifaceHandle,
               (NetPlatform::NetworkDownReasonType *)&mLastNetDownReason
             );

      SetHandle (0);

      // Release 1X network if exist, since handle is now 0
      if (0 != mpTechObjNetwork1x)
      {
        DS_UTILS_RELEASEIF (mpTechObjNetwork1x);

        mpTechObjNetwork1xPriv = 0;
      }
    }

    Notify (eventInfoPtr->eventName);
  } while (0);

  mpICritSect->Leave();
  return;

} /* NetworkMonitored::ProcessIfaceStateEvent() */

ds::ErrorType NetworkMonitored::NotifyNetDown2Monitored
(
 void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  (void) GetPreviousState (&mCachedPreviousState);

  // set handle must precede the Notify() below
  SetHandle (0);

  // Release 1X network if exist, since handle is now 0
  if (0 != mpTechObjNetwork1x)
  {
    DS_UTILS_RELEASEIF (mpTechObjNetwork1x);

    mpTechObjNetwork1xPriv = 0;
  }
  
  Notify (NetworkEvent::QDS_EV_IFACE_DOWN);

  LOG_MSG_FUNCTION_EXIT_1 ("NetworkMonitored::NotifyNetDown2Monitored(): "
                           "Obj 0x%p", this);
  mpICritSect->Leave();

  return AEE_SUCCESS;
} /* NetworkMonitored::NotifyNetDown2Monitored() */

ds::ErrorType NetworkMonitored::Init
(
  void
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkMonitored::Init(): Obj 0x%p", this);
  
  do
  {
    /*-----------------------------------------------------------------------
      Create signal buses to hold event registration info
    -----------------------------------------------------------------------*/
    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusBearerTech);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusBearerTech, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusIPAddr);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusIPAddr, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusExtendedIPConfig);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusExtendedIPConfig, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusQoSAware);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusQoSAware, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusDelExtIPv6Addr);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusDelExtIPv6Addr, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusBearerTechEx);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusBearerTechEx, obj 0x%p",
                       this);
      break;
    }
    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusOutage);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusOutage, obj 0x%p",
                       this);
      break;
    }
    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusHandoffInit);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusHandoffInit, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusHandoffSuccess);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusHandoffSuccess, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusHandoffFailure);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusHandoffFailure, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusExtendedIpConfigEx);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusExtendedIpConfigEx, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusApnParamChange);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusApnParamChange, obj 0x%p",
                       this);
      break;
    }

    result = DS_Utils_CreateInstance(
      0,
      AEECLSID_CSignalBus,
      (void **) &mpSigBusRFConditions);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("NetworkMonitored::Init(): Failed to alloc mpSigBusRFConditions, obj 0x%p",
                       this);
      break;
    }
    result = AEE_SUCCESS;

  } while (0);

  if (AEE_SUCCESS == result)
  {
    result = Network::Init();
  }

  /*-------------------------------------------------------------------------
    Cleanup in case of error (::Destructor won't do this in case of error)
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    // state change
    ASSERT(0);
    DS_UTILS_RELEASEIF (mpSigBusBearerTech);
    DS_UTILS_RELEASEIF (mpSigBusBearerTechEx);
    DS_UTILS_RELEASEIF (mpSigBusIPAddr);
    DS_UTILS_RELEASEIF (mpSigBusExtendedIPConfig);
    DS_UTILS_RELEASEIF (mpSigBusQoSAware);
    DS_UTILS_RELEASEIF (mpSigBusDelExtIPv6Addr);
    DS_UTILS_RELEASEIF (mpSigBusOutage);
    DS_UTILS_RELEASEIF (mpSigBusHandoffInit);
    DS_UTILS_RELEASEIF (mpSigBusHandoffSuccess);
    DS_UTILS_RELEASEIF (mpSigBusHandoffFailure);
    DS_UTILS_RELEASEIF (mpSigBusRFConditions);
    DS_UTILS_RELEASEIF (mpSigBusExtendedIpConfigEx);
    DS_UTILS_RELEASEIF (mpSigBusApnParamChange);
  }
  return result;

} /* NetworkMonitored::Init() */

void NetworkMonitored::Destructor
(
  void
)
  throw()
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == mpICritSect)
  {
    LOG_MSG_ERROR_1("Network::Destructor(): Obj 0x%p "
                    "Network wasn't fully initialized", this);
    return;
  }

  mpICritSect->Enter();

  // state change
  DS_UTILS_RELEASEIF (mpSigBusBearerTech);
  DS_UTILS_RELEASEIF (mpSigBusBearerTechEx);
  DS_UTILS_RELEASEIF (mpSigBusIPAddr);
  DS_UTILS_RELEASEIF (mpSigBusExtendedIPConfig);
  DS_UTILS_RELEASEIF (mpSigBusQoSAware);
  DS_UTILS_RELEASEIF (mpSigBusDelExtIPv6Addr);
  DS_UTILS_RELEASEIF (mpSigBusOutage);
  DS_UTILS_RELEASEIF (mpSigBusHandoffInit);
  DS_UTILS_RELEASEIF (mpSigBusHandoffSuccess);
  DS_UTILS_RELEASEIF (mpSigBusHandoffFailure);
  DS_UTILS_RELEASEIF (mpSigBusRFConditions);
  DS_UTILS_RELEASEIF (mpSigBusExtendedIpConfigEx);
  DS_UTILS_RELEASEIF (mpSigBusApnParamChange);
  Network::Destructor();

  mpICritSect->Leave();
} /* NetworkMonitored::Destructor() */

ds::ErrorType NetworkMonitored::GetSignalBus
(
 ds::Net::EventType  eventID,
 ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppISigBus)
  {
    LOG_MSG_INVALID_INPUT_1 ("NetworkMonitored::GetSignalBus(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  *ppISigBus = NULL;

  mpICritSect->Enter();

  switch (eventID)
  {
    case  NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED:
    {
      *ppISigBus = mpSigBusBearerTech;
      break;
    }
    case  NetworkEvent::QDS_EV_IP_ADDR_CHANGED:
    {
      *ppISigBus = mpSigBusIPAddr;
      break;
    }
    case  NetworkControlEvent::QDS_EV_EXTENDED_IP_CONFIG:
    {
      *ppISigBus = mpSigBusExtendedIPConfig;
      break;
    }
    case  NetworkExtEvent::QDS_EV_QOS_AWARENESS:
    {
      *ppISigBus = mpSigBusQoSAware;
      break;
    }
    case NetworkEvent::QDS_EV_IPV6_EXT_ADDR_DEL_EV:
    {
      *ppISigBus = mpSigBusDelExtIPv6Addr;
      break;
    }
    case  NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED_EX:
    {
      *ppISigBus = mpSigBusBearerTechEx;
      break;
    }
    case  NetworkEvent::QDS_EV_OUTAGE:
    {
      *ppISigBus = mpSigBusOutage;
      break;
    }
    case NetworkExtEvent::QDS_EV_HANDOFF_INIT:
    {
      *ppISigBus = mpSigBusHandoffInit;
      break;
    }

    case NetworkExtEvent::QDS_EV_HANDOFF_SUCCESS:
    {
      *ppISigBus = mpSigBusHandoffSuccess;
      break;
    }

    case NetworkExtEvent::QDS_EV_HANDOFF_FAILURE:
    {
      *ppISigBus = mpSigBusHandoffFailure;
      break;
    }
    case  NetworkEvent::QDS_EV_RF_CONDITIONS_CHANGED:
    {
      *ppISigBus = mpSigBusRFConditions;
      break;
    }

    case NetworkExtEvent::QDS_EV_EXTENDED_IP_CONFIG_EX:
      {
        *ppISigBus = mpSigBusExtendedIpConfigEx;
        break;
      }

    case NetworkExtEvent::QDS_EV_APN_PARAM_EV:
    {
      *ppISigBus = mpSigBusApnParamChange;
	    break;
    }

    default:
    {
      mpICritSect->Leave();
      return Network::GetSignalBus(eventID, ppISigBus);
    }
  }
  /*-------------------------------------------------------------------------
    Handle the case where event is supported by this object.
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS == result)
  {
    if (0 == *ppISigBus)
    {
      LOG_MSG_INFO1_2 ("NetworkMonitored::GetSignalBus(): "
                       "NULL signal bus for event 0x%x, obj 0x%p",
                       eventID, this);
      result = QDS_EINVAL;
    }
    else
    {
      result = AEE_SUCCESS;
      (void) (*ppISigBus)->AddRef();
    }
  }

  LOG_MSG_FUNCTION_EXIT_4 ("NetworkMonitored::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p, event 0x%x",
                           result, *ppISigBus, this, eventID);

  mpICritSect->Leave();
  return result;

}

NetworkMonitored::~NetworkMonitored
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("NetworkMonitored::~NetworkMonitored(): Obj 0x%p",
                            this);

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* NetworkMonitored::~NetworkMonitored() */
