/*===========================================================================
  FILE: ds_Net_MCastSessionPriv.cpp

  OVERVIEW: This file provides implementation of the MCastSession class.

  DEPENDENCIES: None

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MCastSessionPriv.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-30 kr  Free floating changes
  2008-07-29 vm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATUTE_DATA_PS_MCAST
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_MCastSessionPriv.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"

using namespace ds::Net;
using namespace ds::Net::Conversion;
using namespace ds::Error;

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
MCastSessionPriv::MCastSessionPriv
(
  int32          ifaceHandle,
  int32          mcastHandle,
  MCastJoinFlagsType regFlag
)
: Handle (ifaceHandle),
  mMCastHandle (mcastHandle),
  mStatusInfoCode(0),
  mJoinFlag(regFlag),
  refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("MCastSessionPriv::MCastSessionPriv(): "
                            "Obj 0x%p, iface handle 0x%x, mcast handle 0x%x",
                            this, ifaceHandle, mcastHandle);

  memset((void*)&mRegState_1_0, 0, sizeof(MCastStateChangedType));

  memset((void*)&mRegState_2_0, 0, sizeof(MCastStateChangedType));

  //TODO Move out to Init function. Go through constructors of all dsnet/dss
  //objects and fix this

  (void) DS_Utils_CreateInstance (NULL, AEECLSID_CSignalBus,
                                  (void **)(&mpSigBusRegStatus1_0));
  (void) DS_Utils_CreateInstance (NULL, AEECLSID_CSignalBus,
                                  (void **)(&mpSigBusRegStatus2_0));
  (void) DS_Utils_CreateInstance (NULL, AEECLSID_CSignalBus,
                                  (void **)(&mpSigBusTechStatus));

  if (AEE_SUCCESS != Handle::Init(EventManager::mcastObjList))
  {
    LOG_MSG_ERROR_3 ("MCastSessionPriv::MCastSessionPriv(): "
                     "Failed to init the Handle   "
                     "obj 0x%p, if handle 0x%x, mcastHandle 0x%x",
                     this, ifaceHandle, mcastHandle);
    return;
  }

} /* MCastSessionPriv::MCastSessionPriv() */

void MCastSessionPriv::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  DS_UTILS_RELEASEIF (mpSigBusTechStatus);
  DS_UTILS_RELEASEIF (mpSigBusRegStatus2_0);
  DS_UTILS_RELEASEIF (mpSigBusRegStatus1_0);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor of the base class.
  -------------------------------------------------------------------------*/
  Handle::Destructor();

  mpICritSect->Leave();
} /* MCastSessionPriv::Destructor() */

MCastSessionPriv::~MCastSessionPriv
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* MCastSessionPriv::~MCastSessionPriv() */


/*---------------------------------------------------------------------------
  Inherited functions from IMCastSessionPriv.
---------------------------------------------------------------------------*/
/**
  @brief      This function issues an IOCTL request to leave the multicast
              group it is associated with.

              This function issues an IOCTL request to leave the multicast
              group it is associated with.

  @param      None

  @see        No external dependencies

  @return     AEE_SUCCESS - if the request is issued to leave the MCast group
              successfully. This does not mean that Mcast group is left
              already.
              Error code - on error.
*/
ds::ErrorType MCastSessionPriv::Leave
(
  void
)
{
  NetPlatform::MCastLeaveType       leaveInfo;
  ds::ErrorType                     result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Create arguments for MCAST_LEAVE ioctl.
  -------------------------------------------------------------------------*/
  memset (&leaveInfo, 0, sizeof (NetPlatform::MCastLeaveType));

  mpICritSect->Enter();

  leaveInfo.handle = mMCastHandle;

  /*-------------------------------------------------------------------------
    Call the iface IOCTL to leave the MCast session.
  -------------------------------------------------------------------------*/
  result = NetPlatform::IfaceIoctl (GetHandle(),
                                    NetPlatform::IFACE_IOCTL_MCAST_LEAVE,
                                    static_cast <void *> (&leaveInfo));

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionPriv::Leave(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* MCastSessionPriv::Leave() */

ds::ErrorType MCastSessionPriv::GetRegStateAndInfoCodeBCMCS1_0
(
  MCastStateChangedType* RegStateAndInfoCodeBCMCS1_0
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == RegStateAndInfoCodeBCMCS1_0)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionPriv::GetRegStateAndInfoCodeBCMCS1_0():"
                             " NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  *RegStateAndInfoCodeBCMCS1_0 = mRegState_1_0;

  LOG_MSG_FUNCTION_EXIT_3 ("MCastSessionPriv::GetRegStateAndInfoCodeBCMCS1_0(): "
                           "Reg state %d, info code %d, obj 0x%p",
                           RegStateAndInfoCodeBCMCS1_0->regState,
                           RegStateAndInfoCodeBCMCS1_0->infoCode,
                           this);
  mpICritSect->Leave();

  return AEE_SUCCESS;
} /* MCastSessionPriv::GetRegStateAndInfoCodeBCMCS1_0() */

ds::ErrorType MCastSessionPriv::GetRegStateAndInfoCodeBCMCS2_0
(
  MCastStateChangedType* RegStateAndInfoCodeBCMCS2_0
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == RegStateAndInfoCodeBCMCS2_0)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionPriv::GetRegStateAndInfoCodeBCMCS2_0():"
                             " NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  *RegStateAndInfoCodeBCMCS2_0 = mRegState_2_0;

  LOG_MSG_FUNCTION_EXIT_3 ("MCastSessionPriv::GetRegStateAndInfoCodeBCMCS2_0(): "
                           "Reg state %d, info code %d, obj 0x%p",
                           RegStateAndInfoCodeBCMCS2_0->regState,
                           RegStateAndInfoCodeBCMCS2_0->infoCode,
                           this);
  mpICritSect->Leave();

  return AEE_SUCCESS;
} /* MCastSessionPriv::GetRegStateAndInfoCodeBCMCS2_0() */

ds::ErrorType MCastSessionPriv::GetTechStatusInfoCode
(
  MCastInfoCodeType *  techStatusInfoCode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == techStatusInfoCode)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionPriv::GetTechStatusInfoCode(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  *techStatusInfoCode = mStatusInfoCode;

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionPriv::GetTechStatusInfoCode(): "
                           "Tech status %d, obj 0x%p",
                           *techStatusInfoCode, this);
  mpICritSect->Leave();

  return AEE_SUCCESS;
} /* MCastSessionPriv::GetTechStatusInfoCode() */

/*---------------------------------------------------------------------------
  Inherited function from ds::Utils::INode interface.
---------------------------------------------------------------------------*/
/**
  @brief      This method implements the INode interface's Process() function.

              This function is used in case of event handling. Whenever an
              event occurs the event manager singleton traverses the
              network factory. On each network object this Process() function
              is called. If it is a MCast event, the network object calls
              Traverse on the MCastManager. MCastManager calls this Process()
              function on each of the MCast nodes. This Process() function is
              handled thus:

              Switch (EventGroup)
                Case EVENT_GROUP_MCAST:
                  If this MCast object registered for the event
                    If this->handle == eventinfo->handle
                    Notify()
                    Return TRUE.
                  End.
                  Return TRUE.

                Case Default:
                  ASSERT (0)
                  Return FALSE
              End

  @param[in]  userDataPtr: Event info for the event.

  @see        No external dependencies

  @return     TRUE - If the Process() succeeds.
              FALSE - If the Process() fails.
*/
boolean MCastSessionPriv::Process
(
  void *  userDataPtr
)
{
  EventInfoType *                     pEventInfo;
  ps_iface_mcast_event_info_type *    pPSMCastEventInfo;
  MCastStateChangedType *             regStateToUpdate = 0;
  boolean                             retVal = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == userDataPtr)
  {
    LOG_MSG_ERROR_1 ("MCastSessionPriv::Process(): NULL ev info, obj 0x%p",
                     this);
    return FALSE;
  }

  pEventInfo = static_cast <EventInfoType *>(userDataPtr);

  mpICritSect->Enter();

  LOG_MSG_INFO1_5 ("MCastSessionPriv::Process(): Handle 0x%x, event 0x%x, "
                   "group %d, event handle 0x%x, obj 0x%p",
                   mMCastHandle,
                   pEventInfo->eventName,
                   pEventInfo->eventGroup,
                   pEventInfo->handle,
                   this);

  do
  {
    if (mMCastHandle == pEventInfo->userHandle)
    {
      /*---------------------------------------------------------------------
        Set the MCast Registration status depending upon event
        and BCMCS version, if received info code is related to BCMCS 1.0, DVBH
        or MFLO - only BCMCS 1.0 state is updated,
        if received info code is BCMCS 2.0 related
        - only BCMCS 2.0 state is updated
      ---------------------------------------------------------------------*/
      pPSMCastEventInfo =
        (ps_iface_mcast_event_info_type *) pEventInfo->psEventInfo;

      if (MCastInfoCodeIsBCMCS1_0(pPSMCastEventInfo->info_code) ||
          MCastInfoCodeIsDVBHOrMFLO(pPSMCastEventInfo->info_code) ||
          PS_IFACE_MCAST_IC_NOT_SPECIFIED == pPSMCastEventInfo->info_code ||
          PS_IFACE_MCAST_MBMS_SYSTEM_UNAVAILABLE ==
            pPSMCastEventInfo->info_code)
      {
        regStateToUpdate = &mRegState_1_0;
        if (MCastSessionEvent::QDS_EV_REGISTRATION_STATE ==
              pEventInfo->eventName)
        {
          pEventInfo->eventName =
            MCastSessionPrivEvent::QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0;
        }
      }
      else
      {
        regStateToUpdate = &mRegState_2_0;
        if (MCastSessionEvent::QDS_EV_REGISTRATION_STATE ==
              pEventInfo->eventName)
        {
          pEventInfo->eventName =
            MCastSessionPrivEvent::QDS_EV_REGISTRATION_STATE_PRIV_BCMCS2_0;
        }
      }

      switch (pEventInfo->psEventName)
      {
        case IFACE_MCAST_REGISTER_SUCCESS_EV:
        {
          regStateToUpdate->regState = MCastRegState::MCAST_REGISTER_SUCCESS;
          break;
        }

        case IFACE_MCAST_REGISTER_FAILURE_EV:
        {
          regStateToUpdate->regState = MCastRegState::MCAST_REGISTER_FAILURE;
          break;
        }

        case IFACE_MCAST_DEREGISTERED_EV:
        {
          regStateToUpdate->regState = MCastRegState::MCAST_DEREGISTERED;
          break;
        }
      } /* switch */

      /*---------------------------------------------------------------------
        Set the MCast technology status depending upon event info.
      ---------------------------------------------------------------------*/
      if (MCastSessionEvent::QDS_EV_TECHNOLOGY_STATUS == pEventInfo->eventName)
      {
        pEventInfo->eventName =
          MCastSessionPrivEvent::QDS_EV_TECHNOLOGY_STATUS_PRIV;

        mStatusInfoCode = (MCastInfoCodeType) pPSMCastEventInfo->info_code;
      }
      else
      {
        regStateToUpdate->infoCode =
          (MCastInfoCodeType) pPSMCastEventInfo->info_code;
      }

      /*---------------------------------------------------------------------
        Default back to Handle::Process()
      ---------------------------------------------------------------------*/
      retVal = Handle::Process(userDataPtr);
    }
  } while (0);

  mpICritSect->Leave();
  return retVal;

} /* MCastSessionPriv::Process() */

ds::ErrorType MCastSessionPriv::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("MCastSessionPriv::GetSignalBus(): "
                            "Obj 0x%p, event 0x%x, ppISigBus 0x%x ",
                             this, eventID, ppISigBus);

  if (0 == ppISigBus)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (eventID)
  {
    case  MCastSessionPrivEvent::QDS_EV_TECHNOLOGY_STATUS_PRIV:
    {
      *ppISigBus = mpSigBusTechStatus;
      break;
    }

    case  MCastSessionPrivEvent::QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0:
    {
      *ppISigBus = mpSigBusRegStatus1_0;
      break;
    }

    case  MCastSessionPrivEvent::QDS_EV_REGISTRATION_STATE_PRIV_BCMCS2_0:
    {
      *ppISigBus = mpSigBusRegStatus2_0;
      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("MCastSessionPriv::GetSignalBus(): "
                               "Unknown event 0x%x, obj 0x%p", eventID, this);
      result = QDS_EINVAL;
      break;
    }
  } /* switch (event ID) */

  /*-------------------------------------------------------------------------
    Handle the case where event is supported by this object.
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS == result)
  {
    if (0 == *ppISigBus)
    {
      LOG_MSG_INFO1_2 ("MCastSessionPriv::GetSignalBus(): "
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

  LOG_MSG_FUNCTION_EXIT_3 ("MCastSessionPriv::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p",
                           result, *ppISigBus, this);
  mpICritSect->Leave();

  return result;
} /* MCastSessionPriv::GetSignalBus() */
  #endif // FEATUTE_DATA_PS_MCAST
