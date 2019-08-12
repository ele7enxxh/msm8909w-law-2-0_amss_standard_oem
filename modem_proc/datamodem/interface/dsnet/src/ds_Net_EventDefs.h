#ifndef DS_NET_EVENT_DEFS_H
#define DS_NET_EVENT_DEFS_H
/*=========================================================================*/
/*!
  @file
  ds_Net_EventDefs.h

  @brief
  This file defines the different event groups and EventInfo structure 
  that gets passed along to all the objects during event processing.

  @see
  ds_Net_EventManager.h for complete details about event processing in DSNET and
  DSSOCK layers.

  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_EventDefs.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-06-22 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{

/*!
  @brief
  This enum defines the different event groups used in event processing. 

  @details
  The event groups combine multiple PS iface/flow/phys link events into a 
  "group" depending upon the object that needs to process the event. For
  example, iface events such as bearer tech changed ev, IP address changed
  ev, iface state changed ev, qos aware/unaware notification, RF 
  conditions changed ev etc are grouped into EVENT_GROUP_NETWORK. The only
  purpose of event group is for an object to identify where to propagate
  the event to. 

  The QoS events, for example, are all grouped into EVENT_GROU_QOS. When
  the event actually arrives at Network object, the network object 
  identifies that this needs to be propagated either one of the QOS 
  objects, thus avoiding multiple comparisons.

  @see
  EventManger.h for description of Event processing in DSNET/DSSOCK layers.
*/
typedef enum
{
  EVENT_GROUP_MIN             = 0,
  EVENT_GROUP_INVALID         = EVENT_GROUP_MIN,
  EVENT_GROUP_NETWORK         = 1,
  EVENT_GROUP_QOS             = 2,
  EVENT_GROUP_PHYS_LINK       = 3,
  EVENT_GROUP_MCAST           = 4,
  EVENT_GROUP_NETWORK_MBMS    = 5,
  EVENT_GROUP_NETWORK_IPV6    = 6,
  EVENT_GROUP_NETWORK_1X      = 7,
  EVENT_GROUP_MTPD            = 8,
  EVENT_GROUP_MAX
} EventGroupType;


/*!
  @brief
  This is the structure that gets passed along to all the objects when 
  an event is triggered.

  @details
  This is the event info structure that contains the details related to the
  event. 

  @params eventGroup 
  Event Group for the event.
  
  @params eventName  
  The name of the event in ds layer format. 

  @params handle 
  Iface handle for the event. This is a integer representation of the 
  underlying iface, flow or phys link pointer.

  @params userHandle
  In case of certain events, there is an additional handle associated. For 
  example, mcast events happen on an iface, but also have an associated mcast
  handle. In this example, userHandle refers to the mcast handle for the event

  @params psEventName
  The name of the event in PS layer format. See ps_iface_event_enum_type.

  @param psEventInfo
  Any special event info associated with the events that needs to be 
  propagated to update the state of the objects themselves. The DSNET/DSSOCK
  layers event management uses the signal model to indicate event 
  availability to applications and no event info is used.

  @see
  EventManger.h for description of Event processing in DSNET/DSSOCK layers.
*/
typedef struct EventInfo
{
  EventGroupType    eventGroup;   
  int32             eventName;    
  int32             handle;
  int32             userHandle;
  int32             psEventName;
  void *            psEventInfo;
} EventInfoType;

} /* namespace Net */
}/* namespace ds */

#endif /* DS_NET_EVENT_DEFS_H */

