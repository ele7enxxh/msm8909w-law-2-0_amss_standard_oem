/*===========================================================================
  FILE: ds_Sock_EventManager.cpp

  OVERVIEW: This file provides implementation EventManager class.

  DEPENDENCIES: None

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_EventManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "target.h"

extern "C"
{
#include "ps_svc.h"
}

#include "ds_Utils_CSSupport.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Sock_EventManager.h"
#include "ds_Sock_EventDefs.h"
#include "ds_Sock_SocketFactory.h"
#include "ds_Sock_Socket.h"

#include "ps_iface.h"
#include "ps_flowi_event.h"
#include "ps_phys_link.h"
#include "ps_mem.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"
#include "ps_system_heap.h"

using namespace ds::Sock;
using namespace ds::Utils;
using namespace PS::Sock;


/*===========================================================================

                        PRIVATE DATA DECLARATIONS

===========================================================================*/
static struct
{
  void *  psFlowActivatedEvCbackBufPtr;
  void *  psFlowSuspendedEvCbackBufPtr;
  void *  psFlowTxEnabledEvCbackBufPtr;
  void *  psFlowTxDisabledEvCbackBufPtr;
  void *  psIfaceIPFltrUpdatedEvCbackBufPtr;
  void *  psIfaceAddrChangedEvCbackBufPtr;
  void *  psIfacePrefixUpdatedEvCbackBufPtr;
  void *  psIfaceAddrFamilyChangedEvCbackBufPtr;
  void *  psIfaceUpEvCbackBufPtr;
  void *  psIfaceRouteableEvCbackBufPtr;
  void *  psIfaceDownEvCbackBufPtr;
  void *  psIfaceGoingDownEvCbackBufPtr;
  void *  psIfaceLingeringEvCbackBufPtr;
  void *  psIfaceConfiguringEvCbackBufPtr;
  void *  psIfaceFlowEnabledEvCbackBufPtr;
  void *  psIfaceFlowDisabledEvCbackBufPtr;
  void *  physLinkUpEvCbackBufPtr;
  void *  physLinkDownEvCbackBufPtr;
  void *  physLinkFlowEnabledEvCbackBufPtr;
  void *  physLinkFlowDisabledEvCbackBufPtr;
  void *  physLinkDoSAckEvCbackBufPtr;
} EventCbackInfo;


/*===========================================================================

                        PRIVATE FUNCTION DEFINITIONS

===========================================================================*/
void PSIfaceEventCback
(
  ps_iface_type *             psIfacePtr,
  ps_iface_event_enum_type    psIfaceEvent,
  ps_iface_event_info_u_type  psIfaceEventInfo,
  void *                      userDataPtr
)
{
  ds::Sock::Event::EventInfoType *  sockEventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Propagate event only if IPv6 prefix is removed. Socket doesn't care about
    other type of prefix updates
  -------------------------------------------------------------------------*/
  if (IFACE_PREFIX_UPDATE_EV == psIfaceEvent &&
      PREFIX_REMOVED != psIfaceEventInfo.prefix_info.kind)
  {
    LOG_MSG_FUNCTION_EXIT_2("PSIfaceEventCback(): "
                            "Ignoring ev %d kind %d",
                            psIfaceEvent, psIfaceEventInfo.prefix_info.kind);
    return;
  }

  /*-------------------------------------------------------------------------
    Allocate memory and populate socket event info.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC( sockEventInfoPtr, sizeof( ds::Sock::Event::EventInfoType),
                            ds::Sock::Event::EventInfoType*);

  if (NULL == sockEventInfoPtr)
  {
    LOG_MSG_ERROR_1("PSIfaceEventCback(): "
                    "Out of memory, cannot alloc buf to process ev %d",
                    psIfaceEvent);
    return;
  }

  sockEventInfoPtr->eventGroup = ds::Sock::Event::EVENT_GROUP_NETWORK;
  sockEventInfoPtr->eventMask  = (int32) psIfaceEvent;
  sockEventInfoPtr->handle     = reinterpret_cast <int32> ( psIfacePtr);

  /*-------------------------------------------------------------------------
    Post the command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD, (void*)sockEventInfoPtr);

  LOG_MSG_FUNCTION_EXIT_2("PSIfaceEventCback(): "
  	                  "Ev %d Iface 0x%x Success", psIfaceEvent, psIfacePtr);
  return;

} /* PSIfaceEventCback() */

void PSFlowEventCback
(
  ps_flow_type *              psFlowPtr,
  ps_iface_event_enum_type    psFlowEvent,
  ps_iface_event_info_u_type  psFlowEventInfo,
  void *                      userDataPtr
)
{
  ds::Sock::Event::EventInfoType *  sockEventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  LOG_MSG_FUNCTION_ENTRY_2("PSFlowEventCback(): "
                           "Ev %d PSFlow", psFlowEvent, psFlowPtr);

  /*-------------------------------------------------------------------------
    Allocate memory and populate socket event info.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(sockEventInfoPtr, sizeof( ds::Sock::Event::EventInfoType),
                           ds::Sock::Event::EventInfoType*);

  if (NULL == sockEventInfoPtr)
  {
    LOG_MSG_ERROR_1("PSFlowEventCback(): "
                    "Out of memory, cannot alloc buf to process ev %d",
                    psFlowEvent);
    return;
  }

  sockEventInfoPtr->eventGroup = ds::Sock::Event::EVENT_GROUP_NETWORK;
  sockEventInfoPtr->eventMask  = (int32) psFlowEvent;
  sockEventInfoPtr->handle     = reinterpret_cast <int32> ( psFlowPtr);

  /*-------------------------------------------------------------------------
    Post the command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD, (void*)sockEventInfoPtr);


  return;
} /* PSFlowEventCback() */


void PhysLinkEventCback
(
  ps_phys_link_type *         physLinkPtr,
  ps_iface_event_enum_type    physLinkEvent,
  ps_iface_event_info_u_type  physLinkEventInfo,
  void *                      userDataPtr
)
{
  ds::Sock::Event::EventInfoType *    sockEventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("PhysLinkEventCback(): "
                           "Ev %d PhysLink 0x%x", physLinkEvent, physLinkPtr);


  /*-------------------------------------------------------------------------
    Allocate memory and populate socket event info.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(sockEventInfoPtr, sizeof( ds::Sock::Event::EventInfoType),
                           ds::Sock::Event::EventInfoType*);

  if (NULL == sockEventInfoPtr)
  {
    LOG_MSG_ERROR_1("PhysLinkEventCback(): "
                    "Out of memory, cannot alloc buf to process ev %d",
                    physLinkEvent);
    return;
  }

  sockEventInfoPtr->eventGroup = ds::Sock::Event::EVENT_GROUP_NETWORK;
  sockEventInfoPtr->eventMask  = (int32) physLinkEvent;
  sockEventInfoPtr->handle     = reinterpret_cast <int32> ( physLinkPtr);

  /*-------------------------------------------------------------------------
    Post the command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD, (void*)sockEventInfoPtr);

  LOG_MSG_FUNCTION_EXIT_0("PhysLinkEventCback(): "
                          "Success");
  return;
} /* PhysLinkEventCback() */


void PhysLinkDoSAckEventCback
(
  ps_phys_link_type *         physLinkPtr,
  ps_iface_event_enum_type    physLinkEvent,
  ps_iface_event_info_u_type  physLinkEventInfo,
  void *                      userDataPtr
)
{
  ds::Sock::Event::DoSAckEventInfoType *   dosAckEventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("PhysLinkDoSAckEventCback(): "
                           "PhysLink 0x%x", physLinkPtr);

  /*-------------------------------------------------------------------------
    Allocate memory and populate socket event info.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC( dosAckEventInfoPtr,
                           sizeof( ds::Sock::Event::DoSAckEventInfoType),
                           ds::Sock::Event::DoSAckEventInfoType*);

  if (NULL == dosAckEventInfoPtr)
  {
    LOG_MSG_ERROR_1("PhysLinkDoSAckEventCback(): "
                    "Out of memory, cannot alloc buf to process ev %d",
                    physLinkEvent);
    return;
  }

  dosAckEventInfoPtr->eventGroup = ds::Sock::Event::EVENT_GROUP_NETWORK;
  dosAckEventInfoPtr->eventMask  = (int32) physLinkEvent;
  dosAckEventInfoPtr->handle     = physLinkEventInfo.dos_ack_info.handle;

  dosAckEventInfoPtr->dosAckStatus =
    (DoSAckStatusType) physLinkEventInfo.dos_ack_info.status_info.status;

  dosAckEventInfoPtr->overflow =
    physLinkEventInfo.dos_ack_info.status_info.overflow;

  /*-------------------------------------------------------------------------
    Post the command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD, (void*)dosAckEventInfoPtr);

  return;
} /* PhysLinkDoSAckEventCback() */


void PSIfaceIPFltrEventCback
(
  ps_iface_type *             psIfacePtr,
  ps_iface_event_enum_type    psIfaceEvent,
  ps_iface_event_info_u_type  psIfaceEventInfo,
  void *                      userDataPtr
)
{
  ds::Sock::Event::IPFilterEventInfoType *  ipFltrEventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("PSIfaceIPFltrEventCback(): "
                           "Ev %d Iface 0x%x", psIfaceEvent, psIfacePtr);

  /*-------------------------------------------------------------------------
    Allocate memory and populate socket event info.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(ipFltrEventInfoPtr, 
                           sizeof(ds::Sock::Event::IPFilterEventInfoType),
                           ds::Sock::Event::IPFilterEventInfoType*);
  if (NULL == ipFltrEventInfoPtr)
  {
    return;
  }

  ipFltrEventInfoPtr->eventGroup = ds::Sock::Event::EVENT_GROUP_NETWORK;
  ipFltrEventInfoPtr->eventMask  = (int32) psIfaceEvent;
  ipFltrEventInfoPtr->handle     = reinterpret_cast <int32> ( psIfacePtr);
  ipFltrEventInfoPtr->clientID   = psIfaceEventInfo.ipfltr_info.client;

  /*-------------------------------------------------------------------------
    Post the command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD, (void*)ipFltrEventInfoPtr);

  return;

} /* PSIfaceIPFltrEventCback() */


void PSMemEventCback
(
  int16  wmVal
)
{
  ds::Sock::Event::EventInfoType *  sockEventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("PSMemEventCback(): "
                           "Val %d", wmVal);

  /*-------------------------------------------------------------------------
    Allocate memory and populate socket event info.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC( sockEventInfoPtr, sizeof( ds::Sock::Event::EventInfoType),
                            ds::Sock::Event::IPFilterEventInfoType*);

  if (NULL == sockEventInfoPtr)
  {
    LOG_MSG_ERROR_1("PSMemEventCback(): "
                    "Out of memory, cannot process ps mem ev, WM %d", wmVal);
    return;
  }

  sockEventInfoPtr->eventGroup = ds::Sock::Event::EVENT_GROUP_PS_MEM;
  sockEventInfoPtr->eventMask  = (int32) PSMemEvent::PS_MEM_BUF_AVAILABLE;
  sockEventInfoPtr->handle     = 0;

  /*-------------------------------------------------------------------------
    Post the command to PS task.
  -------------------------------------------------------------------------*/
  ps_send_cmd( PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD, (void*)sockEventInfoPtr);

  return;

} /* PSMemEventCback() */

static void ProcessGenericEventCmdHandler
(
  ps_cmd_enum_type    cmd,
  void *              userDataPtr
)
{
  SocketFactory *                     sockFactoryPtr;
  ds::Sock::Event::EventInfoType *    sockEventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == userDataPtr || PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD != cmd)
  {
    LOG_MSG_INVALID_INPUT_2("ProcessGenericEventCmdHandler(): "
                            "Invalid args: cmd %d, event info 0x%p",
                            cmd, userDataPtr);
    ASSERT( 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the event info
  -------------------------------------------------------------------------*/
  sockEventInfoPtr =
    reinterpret_cast <ds::Sock::Event::EventInfoType *> ( userDataPtr);

  LOG_MSG_INFO1_3("ProcessGenericEventCmdHandler(): "
                  "Processing ev %d, group %d, handle 0x%x",
                  sockEventInfoPtr->eventMask,
                  sockEventInfoPtr->eventGroup,
                  sockEventInfoPtr->handle);

  /*-------------------------------------------------------------------------
    Relay events to all the sockets in socket factory
  -------------------------------------------------------------------------*/
  sockFactoryPtr = SocketFactory::CreateInstance();
  ASSERT( 0 != sockFactoryPtr);

  (void) sockFactoryPtr->Traverse( sockEventInfoPtr);
  DS_UTILS_RELEASEIF( sockFactoryPtr);

  /*-------------------------------------------------------------------------
    Free the memory allocated for socket event info.
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_FREE( sockEventInfoPtr);

  LOG_MSG_FUNCTION_EXIT_1("ProcessGenericEventCmdHandler(): "
                          "Processed cmd %d", cmd);

} /* ProcessGenericEventCmdHandler() */


void ds::Sock::EventManager::Init
(
  void
)
{
  int32  retVal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Register command handlers. All iface/flow/phys_link/PS_MEM/DOS_ACK
    events are processed in PS task context.
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler( PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD,
                             ProcessGenericEventCmdHandler);

  /*-------------------------------------------------------------------------
    Register for IFACE_ADDR_CHANGED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceAddrChangedEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceAddrChangedEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_ADDR_CHANGED_EV,
                              EventCbackInfo.psIfaceAddrChangedEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_ADDR_FAMILY_CHANGED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceAddrFamilyChangedEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceAddrFamilyChangedEvCbackBufPtr)
  {
    goto bail;
  }

  retVal = ps_iface_event_cback_reg
           (
             NULL,
             IFACE_ADDR_FAMILY_CHANGED_EV,
             EventCbackInfo.psIfaceAddrFamilyChangedEvCbackBufPtr
           );
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_PREFIX_UPDATE_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfacePrefixUpdatedEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfacePrefixUpdatedEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_PREFIX_UPDATE_EV,
                              EventCbackInfo.psIfacePrefixUpdatedEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_ROUTEABLE_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceRouteableEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceRouteableEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_ROUTEABLE_EV,
                              EventCbackInfo.psIfaceRouteableEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_UP_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceUpEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceUpEvCbackBufPtr)
  {
    goto bail;
  }

  retVal = ps_iface_event_cback_reg( NULL,
                                     IFACE_UP_EV,
                                     EventCbackInfo.psIfaceUpEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_DOWN_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceDownEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceDownEvCbackBufPtr)
  {
    goto bail;
  }

  retVal = ps_iface_event_cback_reg( NULL,
                                     IFACE_DOWN_EV,
                                     EventCbackInfo.psIfaceDownEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_CONFIGURING_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceConfiguringEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceConfiguringEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_CONFIGURING_EV,
                              EventCbackInfo.psIfaceConfiguringEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_LINGERING_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceLingeringEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceLingeringEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_LINGERING_EV,
                              EventCbackInfo.psIfaceLingeringEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_GOING_DOWN_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceGoingDownEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceGoingDownEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_GOING_DOWN_EV,
                              EventCbackInfo.psIfaceGoingDownEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_IPFLTR_UPDATED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceIPFltrUpdatedEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceIPFltrEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceIPFltrUpdatedEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_IPFLTR_UPDATED_EV,
                              EventCbackInfo.psIfaceIPFltrUpdatedEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_FLOW_ENABLED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceFlowEnabledEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceFlowEnabledEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_FLOW_ENABLED_EV,
                              EventCbackInfo.psIfaceFlowEnabledEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for IFACE_FLOW_DISABLED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psIfaceFlowDisabledEvCbackBufPtr =
    ps_iface_alloc_event_cback_buf( PSIfaceEventCback, NULL);

  if (NULL == EventCbackInfo.psIfaceFlowDisabledEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_iface_event_cback_reg( NULL,
                              IFACE_FLOW_DISABLED_EV,
                              EventCbackInfo.psIfaceFlowDisabledEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for FLOW_ACTIVATED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psFlowActivatedEvCbackBufPtr =
    ps_flow_alloc_event_cback_buf( PSFlowEventCback, NULL);

  if (NULL == EventCbackInfo.psFlowActivatedEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_flow_event_cback_reg( NULL,
                             FLOW_ACTIVATED_EV,
                             EventCbackInfo.psFlowActivatedEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for FLOW_SUSPENDED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psFlowSuspendedEvCbackBufPtr =
    ps_flow_alloc_event_cback_buf( PSFlowEventCback, NULL);

  if (NULL == EventCbackInfo.psFlowSuspendedEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_flow_event_cback_reg( NULL,
                             FLOW_SUSPENDED_EV,
                             EventCbackInfo.psFlowSuspendedEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for FLOW_TX_ENABLED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psFlowTxEnabledEvCbackBufPtr =
    ps_flow_alloc_event_cback_buf( PSFlowEventCback, NULL);

  if (NULL == EventCbackInfo.psFlowTxEnabledEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_flow_event_cback_reg( NULL,
                             FLOW_TX_ENABLED_EV,
                             EventCbackInfo.psFlowTxEnabledEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for FLOW_TX_DISABLED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.psFlowTxDisabledEvCbackBufPtr =
    ps_flow_alloc_event_cback_buf( PSFlowEventCback, NULL);

  if (NULL == EventCbackInfo.psFlowTxDisabledEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_flow_event_cback_reg( NULL,
                             FLOW_TX_DISABLED_EV,
                             EventCbackInfo.psFlowTxDisabledEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_UP_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.physLinkUpEvCbackBufPtr =
    ps_phys_link_alloc_event_cback_buf( PhysLinkEventCback, NULL);

  if (NULL == EventCbackInfo.physLinkUpEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_phys_link_event_cback_reg( NULL,
                                  PHYS_LINK_UP_EV,
                                  EventCbackInfo.physLinkUpEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_DOWN_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.physLinkDownEvCbackBufPtr =
    ps_phys_link_alloc_event_cback_buf( PhysLinkEventCback, NULL);

  if (NULL == EventCbackInfo.physLinkDownEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_phys_link_event_cback_reg( NULL,
                                  PHYS_LINK_DOWN_EV,
                                  EventCbackInfo.physLinkDownEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_FLOW_ENABLED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.physLinkFlowEnabledEvCbackBufPtr =
    ps_phys_link_alloc_event_cback_buf( PhysLinkEventCback, NULL);

  if (NULL == EventCbackInfo.physLinkFlowEnabledEvCbackBufPtr)
  {
    goto bail;
  }

  retVal = ps_phys_link_event_cback_reg
           (
             NULL,
             PHYS_LINK_FLOW_ENABLED_EV,
             EventCbackInfo.physLinkFlowEnabledEvCbackBufPtr
           );
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_FLOW_DISABLED_EV with netiface module
  -------------------------------------------------------------------------*/
  EventCbackInfo.physLinkFlowDisabledEvCbackBufPtr =
    ps_phys_link_alloc_event_cback_buf( PhysLinkEventCback, NULL);

  if (NULL == EventCbackInfo.physLinkFlowDisabledEvCbackBufPtr)
  {
    goto bail;
  }

  retVal = ps_phys_link_event_cback_reg
           (
             NULL,
             PHYS_LINK_FLOW_DISABLED_EV,
             EventCbackInfo.physLinkFlowDisabledEvCbackBufPtr
           );
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for PHYS_LINK_707_DOS_ACK_EV with netiface module

    Handle DoS event separately as extra information needs to be passed along
    with the event
  -------------------------------------------------------------------------*/
  EventCbackInfo.physLinkDoSAckEvCbackBufPtr =
    ps_phys_link_alloc_event_cback_buf( PhysLinkDoSAckEventCback, NULL);

  if (NULL == EventCbackInfo.physLinkDoSAckEvCbackBufPtr)
  {
    goto bail;
  }

  retVal =
    ps_phys_link_event_cback_reg( NULL,
                                  PHYS_LINK_707_DOS_ACK_EV,
                                  EventCbackInfo.physLinkDoSAckEvCbackBufPtr);
  if (0 != retVal)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for ps_mem callbacks
  -------------------------------------------------------------------------*/
  ps_mem_reg_mem_avail_cb( PS_MEM_RT_META_INFO_TYPE, PSMemEventCback);
  ps_mem_reg_mem_avail_cb( PS_MEM_PKT_META_INFO_TYPE, PSMemEventCback);

  return;

bail:
  DATA_ERR_FATAL("ds::Sock::EventManager::Init(): Event registration failed");
  return;

} /* ds::Sock::EventManager::Init() */


void ds::Sock::EventManager::Deinit
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_iface_event_cback_dereg( NULL,
    IFACE_ADDR_CHANGED_EV,
    EventCbackInfo.psIfaceAddrChangedEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceAddrChangedEvCbackBufPtr );

  ps_iface_event_cback_dereg( NULL,
    IFACE_ADDR_FAMILY_CHANGED_EV,
    EventCbackInfo.psIfaceAddrFamilyChangedEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceAddrFamilyChangedEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_PREFIX_UPDATE_EV,
    EventCbackInfo.psIfacePrefixUpdatedEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfacePrefixUpdatedEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_ROUTEABLE_EV,
    EventCbackInfo.psIfaceRouteableEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceRouteableEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_UP_EV,
    EventCbackInfo.psIfaceUpEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceUpEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_DOWN_EV,
    EventCbackInfo.psIfaceDownEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceDownEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_CONFIGURING_EV,
    EventCbackInfo.psIfaceConfiguringEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceConfiguringEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_LINGERING_EV,
    EventCbackInfo.psIfaceLingeringEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceLingeringEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_GOING_DOWN_EV,
    EventCbackInfo.psIfaceGoingDownEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceGoingDownEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_IPFLTR_UPDATED_EV,
    EventCbackInfo.psIfaceIPFltrUpdatedEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceIPFltrUpdatedEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_FLOW_ENABLED_EV,
    EventCbackInfo.psIfaceFlowEnabledEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceFlowEnabledEvCbackBufPtr);

  ps_iface_event_cback_dereg( NULL,
    IFACE_FLOW_DISABLED_EV,
    EventCbackInfo.psIfaceFlowDisabledEvCbackBufPtr);
  ps_iface_free_event_cback_buf( EventCbackInfo.psIfaceFlowDisabledEvCbackBufPtr);

  ps_flow_event_cback_dereg( NULL,
    FLOW_ACTIVATED_EV,
    EventCbackInfo.psFlowActivatedEvCbackBufPtr);
  ps_flow_free_event_cback_buf( EventCbackInfo.psFlowActivatedEvCbackBufPtr);

  ps_flow_event_cback_dereg( NULL,
    FLOW_SUSPENDED_EV,
    EventCbackInfo.psFlowSuspendedEvCbackBufPtr);
  ps_flow_free_event_cback_buf( EventCbackInfo.psFlowSuspendedEvCbackBufPtr);

  ps_flow_event_cback_dereg( NULL,
    FLOW_TX_ENABLED_EV,
    EventCbackInfo.psFlowTxEnabledEvCbackBufPtr);
  ps_flow_free_event_cback_buf( EventCbackInfo.psFlowTxEnabledEvCbackBufPtr);

  ps_flow_event_cback_dereg( NULL,
    FLOW_TX_DISABLED_EV,
    EventCbackInfo.psFlowTxDisabledEvCbackBufPtr);
  ps_phys_link_free_event_cback_buf( EventCbackInfo.psFlowTxDisabledEvCbackBufPtr);

  ps_phys_link_event_cback_dereg( NULL,
    PHYS_LINK_UP_EV,
    EventCbackInfo.physLinkUpEvCbackBufPtr);
  ps_phys_link_free_event_cback_buf( EventCbackInfo.physLinkUpEvCbackBufPtr);

  ps_phys_link_event_cback_dereg( NULL,
    PHYS_LINK_DOWN_EV,
    EventCbackInfo.physLinkDownEvCbackBufPtr);
  ps_phys_link_free_event_cback_buf( EventCbackInfo.physLinkDownEvCbackBufPtr);

  ps_phys_link_event_cback_dereg( NULL,
    PHYS_LINK_FLOW_ENABLED_EV,
    EventCbackInfo.physLinkFlowEnabledEvCbackBufPtr);
  ps_phys_link_free_event_cback_buf( EventCbackInfo.physLinkFlowEnabledEvCbackBufPtr);

  ps_phys_link_event_cback_dereg( NULL,
    PHYS_LINK_FLOW_DISABLED_EV,
    EventCbackInfo.physLinkFlowDisabledEvCbackBufPtr);
  ps_phys_link_free_event_cback_buf( EventCbackInfo.physLinkFlowDisabledEvCbackBufPtr);

  ps_phys_link_event_cback_dereg( NULL,
    PHYS_LINK_707_DOS_ACK_EV,
    EventCbackInfo.physLinkDoSAckEvCbackBufPtr);
  ps_phys_link_free_event_cback_buf( EventCbackInfo.physLinkDoSAckEvCbackBufPtr);

  // TODO how do we deregister?
  //ps_mem_reg_mem_avail_cb( PS_MEM_RT_META_INFO_TYPE, PSMemEventCback);
  //ps_mem_reg_mem_avail_cb( PS_MEM_PKT_META_INFO_TYPE, PSMemEventCback);

} /* ds::Sock::EventManager::Deinit() */
