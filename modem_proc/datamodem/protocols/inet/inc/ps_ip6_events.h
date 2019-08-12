#ifndef _PS_IP6_EVENTS_H
#define _PS_IP6_EVENTS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ I P 6 _ E V E N T S . H

GENERAL DESCRIPTION
  This header file contains the definitions that are used to generate IPv6 
  events for DIAG. The following events are supported:

  EVENT_IPV6_SM_EVENT
    IPv6 state machine events

  EVENT_IPV6_SM_TRANSITION
    IPv6 state machine transitions

  EVENT_IPV6_PREFIX_UPDATE
    IPv6 prefix updates (added, removed, deprecated, updated)
  

    Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_ip6_events.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

===========================================================================*/
#include "comdef.h"

#include "event.h"
#include "event_defs.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ps_ip6_sm.h"

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/
/*--------------------------------------------------------------------------
TYPEDEF IP6_SM_EVENT_PAYLOAD_TYPE

DESCRIPTION
  IP6 state machine event payload type, for EVENT_IPV6_SM_EVENT
--------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_name_enum_type iface_name;
  uint8 iface_instance;
  ip6_sm_event_type event;
} ip6_sm_event_payload_type;

/*--------------------------------------------------------------------------
TYPEDEF IP6_SM_TRANSITION_EVENT_PAYLOAD_TYPE

DESCRIPTION
  IP6 state machine transition event payload type, for 
  EVENT_IPV6_SM_TRANSITION
--------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_name_enum_type iface_name;
  uint8 iface_instance;
  ip6_smi_state_type old_state;
  ip6_smi_state_type new_state;
} ip6_sm_transition_event_payload_type;

/*--------------------------------------------------------------------------
TYPEDEF IP6_PREFIX_UPDATE_EVENT_PAYLOAD_TYPE

DESCRIPTION
  IP6 prefix update event payload type, for EVENT_IPV6_PREFIX_UPDATE
--------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_name_enum_type iface_name;
  uint8 iface_instance;
  ps_iface_prefix_update_enum_type kind;
  uint64 prefix;
  uint8 prefix_len;
} ip6_prefix_update_event_payload_type;

/*===========================================================================
MACRO IP6_REPORT_PFX_UPDATE()

DESCRIPTION
  This macro is used to send a prefix update event to diag.

PARAMETERS
  ps_iface_type * if_ptr: pointer to the PS iface
  ps_iface_prefix_update_type * prefix_info: Prefix information

RETURN VALUE
  None
===========================================================================*/
#define IP6_REPORT_PFX_UPDATE( if_ptr, prefix_info )                        \
{                                                                           \
    ip6_prefix_update_event_payload_type event_payload;                     \
    memset(&event_payload,                                                  \
           0,                                                               \
           sizeof(ip6_prefix_update_event_payload_type));                   \
    event_payload.iface_name = if_ptr->name;                                \
    event_payload.iface_instance = if_ptr->instance;                        \
    event_payload.prefix = prefix_info.prefix.ps_s6_addr64[0];              \
    event_payload.prefix_len = prefix_info.prefix_len;                      \
    event_payload.kind = prefix_info.kind;                                  \
    event_report_payload( EVENT_IPV6_PREFIX_UPDATE,                         \
                          sizeof( event_payload ),                          \
                          &event_payload );                                 \
}
#endif /*  _PS_IP6_EVENTS_H */
