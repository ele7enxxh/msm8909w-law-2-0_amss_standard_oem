/*===========================================================================
  FILE: ds_Net_EventManager.cpp

  OVERVIEW: This file provides implementation EventManager class.

  DEPENDENCIES: None

  Copyright (c) 2007-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_EventManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "comdef.h"
#include <stringl/stringl.h>

extern "C"
{
#include "ps_svc.h"
}

#include "ds_Utils_StdErr.h"
#include "ps_system_heap.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_DebugMsg.h"

#include "ds_Net_EventManager.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_Platform.h"
#include "ds_Net_TechUMTSFactory.h"
#include "ds_Net_NetworkFactory.h"
#include "ds_Net_Conversion.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace ds::Utils;
using namespace NetPlatform;

/*===========================================================================

                        PRIVATE DATA DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Definition of static members of EventManager class.
---------------------------------------------------------------------------*/
Factory*       EventManager::networkObjList       = NULL;
Factory*       EventManager::qosObjList           = NULL;
Factory*       EventManager::physLinkObjList      = NULL;
Factory*       EventManager::mcastObjList         = NULL;
Factory*       EventManager::networkMBMSObjList   = NULL;
Factory*       EventManager::networkIPv6ObjList   = NULL;
Factory*       EventManager::network1XObjList     = NULL;
Factory*       EventManager::mtpdObjList          = NULL;

/*---------------------------------------------------------------------------
  DSNET Process Event Cmd data type.
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  int32                     eventName;
  IfaceEventInfoUnionType   eventInfo;
} ProcessEventCmdDataType;

/*===========================================================================

                        PRIVATE FUNCTION DEFINITIONS

===========================================================================*/
EventManager::EventManager
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("EventManager::EventManager(): "
                   "Creating EventManager obj 0x%p", this);

  networkObjList       = new Factory();
  qosObjList           = new Factory();
  physLinkObjList      = new Factory();
#ifdef FEATUTE_DATA_PS_MCAST
  mcastObjList         = new Factory();
#endif // FEATUTE_DATA_PS_MCAST
  networkMBMSObjList   = new Factory();
  networkIPv6ObjList   = new Factory();
  network1XObjList     = new Factory();
  mtpdObjList          = new Factory();

} /* EventManager() */

EventManager::~EventManager
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("EventManager::~EventManager(): " 
                   "Deleting Event Manager 0x%p", this);

  delete networkObjList;
  delete qosObjList;
  delete physLinkObjList;
#ifdef FEATUTE_DATA_PS_MCAST
  delete mcastObjList;
#endif // FEATUTE_DATA_PS_MCAST
  delete networkMBMSObjList;
  delete networkIPv6ObjList;
  delete network1XObjList;
  delete mtpdObjList;

} /* ~EventManager() */

static void ProcessEventCmdHandler
(
  ps_cmd_enum_type    cmd,
  void *              pUserData
)
{
  EventInfoType                              eventInfo;
  EventGroupType                             dsEventGroup = EVENT_GROUP_INVALID;
  int32                                      dsEventName = -1;
  int32                                      userHandle = 0;
  void *                                     pPSEventInfo = NULL;
  static IPv6PrivAddrEventInfoType           ipv6PrivAddrEventInfo;
  static IPv6PrefixUpdatedEventInfoType      ipv6PrefixUpdatedEventInfo;
  static ExtendedIpConfigExType              extendedIpConfigExType;
  static ApnParamInfoType                    apnParamInfoType;
#ifdef FEATUTE_DATA_PS_MCAST
  static ps_iface_mcast_event_info_type      mcastEventInfo;
#endif // FEATUTE_DATA_PS_MCAST
  static ps_iface_net_initiated_qos_available_info_type 
                                             iface_net_initiated_qos_available_info;
  static PhysLinkEventInfo                   pPhysLinkStateEventInfo;
  static PSIPAddrType                        pIPv6ExternalAddrDeletedEvInfo;
  static HandoffEventInfoType                pHandoffInfo;
  static HandoffFailureEventInfoType         pHandoffFailureInfo;
  IfaceEventInfoUnionType *                  pEventInfo;
  int32                                      result;
  ProcessEventCmdDataType *                  pProcessEventCmdData;
  int32                                      psEventName;
  int32                                      handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (NULL == pUserData || cmd != PS_DSNET_PROCESS_GENERIC_EVENT_CMD)
  {
    LOG_MSG_ERROR_2("ProcessEventCmdHandler(): "
                    "Invalid args: cmd %d, event info %p", cmd, pUserData);
    ASSERT (0);
    return;
  }

  pProcessEventCmdData = (ProcessEventCmdDataType *) pUserData;

  psEventName = pProcessEventCmdData->eventName;
  handle      = pProcessEventCmdData->handle;
  pEventInfo  = &(pProcessEventCmdData->eventInfo);

  LOG_MSG_INFO1_2 ("ProcessEventCmdHandler(): "
                   "Process event %d, handle 0x%x", psEventName, handle);

  /*-------------------------------------------------------------------------
    Convert ps_iface_event_enum_type to appropriate ds event name and group.
  -------------------------------------------------------------------------*/
  result = ds::Net::Conversion::PS2DSEventInfo (psEventName,
                                                pEventInfo,
                                                &dsEventName,
                                                &userHandle,
                                                &dsEventGroup);
  if (AEE_SUCCESS != result)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Special cases for events which propagate additional event information
    to the ds_Net object handling the event
  -------------------------------------------------------------------------*/
  switch (psEventName)
  {
    case IFACE_EXTENDED_IP_CONFIG_EX_EV:
      extendedIpConfigExType = pEventInfo->extended_ip_config_change_mask;
      pPSEventInfo = (void *) &extendedIpConfigExType;
      break;
    case IFACE_APN_PARAM_CHANGED_EV:
      apnParamInfoType = pEventInfo->apn_param_change_info_type;
      pPSEventInfo = (void *) &apnParamInfoType;
      break;
    case IFACE_PREFIX_UPDATE_EV:
      memscpy (&ipv6PrefixUpdatedEventInfo,
        sizeof (ipv6PrefixUpdatedEventInfo),
        &pEventInfo->prefix_info,
        sizeof (ipv6PrefixUpdatedEventInfo));
      pPSEventInfo = (void *) &ipv6PrefixUpdatedEventInfo;
      break;

    case IFACE_IPV6_PRIV_ADDR_GENERATED_EV:
    case IFACE_IPV6_PRIV_ADDR_DEPRECATED_EV:
    case IFACE_IPV6_PRIV_ADDR_EXPIRED_EV:
    case IFACE_IPV6_PRIV_ADDR_DELETED_EV:
      memscpy (&ipv6PrivAddrEventInfo,
        sizeof (ipv6PrivAddrEventInfo),
        &pEventInfo->priv_ipv6_addr,
        sizeof (ipv6PrivAddrEventInfo));

      pPSEventInfo = (void *) &ipv6PrivAddrEventInfo;
      break;

#ifdef FEATUTE_DATA_PS_MCAST
    case IFACE_MCAST_REGISTER_SUCCESS_EV:
    case IFACE_MCAST_REGISTER_FAILURE_EV:
    case IFACE_MCAST_DEREGISTERED_EV:
    case IFACE_MCAST_STATUS_EV:
      memscpy (&mcastEventInfo,
        sizeof (mcastEventInfo),
        &pEventInfo->mcast_info,
        sizeof (mcastEventInfo));
      pPSEventInfo = (void *) &mcastEventInfo;
      break;
#endif // FEATUTE_DATA_PS_MCAST
    case IFACE_NET_INITIATED_QOS_AVAILABLE_EV:
      memscpy (&iface_net_initiated_qos_available_info,
        sizeof (ps_iface_net_initiated_qos_available_info_type),
        &pEventInfo->iface_net_initiated_qos_available_info,
        sizeof (ps_iface_net_initiated_qos_available_info_type));
      pPSEventInfo = (void *) &iface_net_initiated_qos_available_info;
      break;

    case IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV:
      pPSEventInfo = 
        (void *)pEventInfo->slotted_mode_info.hdr_slotted_mode_failure_code;
      break;

    case IFACE_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV:
      pPSEventInfo = (void *) pEventInfo->hdr_rev0_rate_inertia_failure_code;
      break;

    case IFACE_FAST_DORMANCY_STATUS_EV:
      pPSEventInfo = (void *) pEventInfo->fast_dorm_status.dorm_status;
      break;

    case PHYS_LINK_DOWN_EV:
    case PHYS_LINK_COMING_UP_EV:
    case PHYS_LINK_UP_EV:
    case PHYS_LINK_GOING_DOWN_EV:
    case PHYS_LINK_GONE_EV:
    case PHYS_LINK_RESUMING_EV:
    case PHYS_LINK_GOING_NULL_EV:
      memscpy (&pPhysLinkStateEventInfo,
        sizeof (PhysLinkEventInfo),
        &(pEventInfo->phys_link_event_info),
        sizeof (PhysLinkEventInfo));
      pPSEventInfo = (void *) &pPhysLinkStateEventInfo;
      break;

    case IFACE_IPV6_EXTERNAL_ADDR_DELETED_EV:
      memscpy (&pIPv6ExternalAddrDeletedEvInfo,
        sizeof(PSIPAddrType),
        &(pEventInfo->ip_addr),
        sizeof(PSIPAddrType));
      pPSEventInfo = (void *) &pIPv6ExternalAddrDeletedEvInfo;
      break;
    case IFACE_HANDOFF_INIT_EV:
    case IFACE_HANDOFF_STATUS_SUCCESS_EV:
      memscpy (&pHandoffInfo,
	    sizeof(HandoffEventInfoType),
	    &(pEventInfo->handoff_event_info),
	    sizeof(HandoffEventInfoType));
      pPSEventInfo = (void *) &pHandoffInfo;
      break;

    case IFACE_HANDOFF_STATUS_FAILURE_EV:
       memscpy (&pHandoffFailureInfo,
	    sizeof(HandoffFailureEventInfoType),
	    &(pEventInfo->handoff_failure_event_info),
	    sizeof(HandoffFailureEventInfoType));
      pPSEventInfo = (void *) &pHandoffFailureInfo;
    break;
    default:
      break;

  } /* special handling for some IPV6 events */


  /*-------------------------------------------------------------------------
    Construct the event info data structure.
  -------------------------------------------------------------------------*/
  eventInfo.eventName   = dsEventName;
  eventInfo.userHandle  = userHandle;
  eventInfo.eventGroup  = dsEventGroup;
  eventInfo.handle      = handle;
  //TODO: Remove psEventName from here.
  eventInfo.psEventName = psEventName;
  eventInfo.psEventInfo = pPSEventInfo;

  /*-------------------------------------------------------------------------
    Traverse the list of DSNet objects depending upon the event group.
  -------------------------------------------------------------------------*/
  switch (dsEventGroup)
  {
    case EVENT_GROUP_NETWORK:
      (void) EventManager::networkObjList->Traverse ((void *) &eventInfo);
      break;
    case EVENT_GROUP_QOS:
      (void) EventManager::qosObjList->Traverse ((void *) &eventInfo);
      break;
    case EVENT_GROUP_PHYS_LINK:
      (void) EventManager::physLinkObjList->Traverse ((void *) &eventInfo);
      break;
#ifdef FEATUTE_DATA_PS_MCAST
    case EVENT_GROUP_MCAST:
      (void) EventManager::mcastObjList->Traverse ((void *) &eventInfo);
      break;
    case EVENT_GROUP_NETWORK_MBMS:
      (void) EventManager::networkMBMSObjList->Traverse ((void *) &eventInfo);
      break;
#endif // FEATUTE_DATA_PS_MCAST
    case EVENT_GROUP_NETWORK_IPV6:
      (void) EventManager::networkIPv6ObjList->Traverse ((void *) &eventInfo);
      break;
    case EVENT_GROUP_NETWORK_1X:
      (void) EventManager::network1XObjList->Traverse ((void *) &eventInfo);
      break;
    case EVENT_GROUP_MTPD:
      (void) EventManager::mtpdObjList->Traverse ((void *) &eventInfo);
      break;

    default:
       break;
  } /* switch (dsEventGroup) */

  /* Fall-through */

bail:
  PS_SYSTEM_HEAP_MEM_FREE (pUserData);
  return;

} /* ProcessEventCmdHandler() */

void EventManager::ProcessEvent
(
  int32                       handle,
  int32                       psEventName,
  IfaceEventInfoUnionType     argEventInfo,
  void *                      pUserData
)
{

  ProcessEventCmdDataType *   pProcessEventCmdData;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if 0
  LOG_MSG_FUNCTION_ENTRY_2 ("EventManager::ProcessEvent(): "
                            "Received event: %d, handle: 0x%x",
                            psEventName, handle);
#endif

  PS_SYSTEM_HEAP_MEM_ALLOC(pProcessEventCmdData, sizeof(ProcessEventCmdDataType),
                           ProcessEventCmdDataType*);
  if (NULL == pProcessEventCmdData)
  {
    LOG_MSG_ERROR_1 ("EventManager::ProcessEvent(): "
                     "Out of memory, cannot alloc buf for processing ev %d",
                     psEventName);
    return;
  }

  pProcessEventCmdData->handle    = handle;
  pProcessEventCmdData->eventName = psEventName;
  memscpy (&pProcessEventCmdData->eventInfo,
    sizeof (IfaceEventInfoUnionType),
    &argEventInfo,
    sizeof (IfaceEventInfoUnionType));


  /*-------------------------------------------------------------------------
    Send the command to process in PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd (PS_DSNET_PROCESS_GENERIC_EVENT_CMD,
               (void*) pProcessEventCmdData);

#if 0
  LOG_MSG_FUNCTION_EXIT_0 ("EventManager::ProcessEvent(): "
                           "SUCCESS");
#endif

  return;

} /* ProcessEvent() */

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
void EventManager::Init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Create an instance of EventManager singleton.
  -------------------------------------------------------------------------*/
  EventManager::InstanceInit();

  /*-------------------------------------------------------------------------
    Register command handler for processing all events.
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler (PS_DSNET_PROCESS_GENERIC_EVENT_CMD,
                             ProcessEventCmdHandler);

  /*-------------------------------------------------------------------------
    Register callback functions for events.
  -------------------------------------------------------------------------*/
  (void) RegIfaceCbackFcn(EventManager::ProcessEvent, NULL);
  (void) RegFlowCbackFcn(EventManager::ProcessEvent, NULL);
  (void) RegPhysLinkCbackFcn(EventManager::ProcessEvent, NULL);
} /* Init() */


void EventManager::Deinit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Delete the EventManager singleton.
  -------------------------------------------------------------------------*/
  EventManager::InstanceRelease();

  /*-------------------------------------------------------------------------
    Deregister callback functions for events.
  -------------------------------------------------------------------------*/
  // TODO not allowed at this point, should be
  //(void) RegIfaceCbackFcn(NULL, NULL);
  //(void) RegFlowCbackFcn(NULL, NULL);
  //(void) RegPhysLinkCbackFcn(NULL, NULL);
} /* Denit() */


