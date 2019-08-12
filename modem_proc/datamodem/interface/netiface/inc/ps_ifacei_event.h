#ifndef PS_IFACEI_EVENT_H
#define PS_IFACEI_EVENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I F A C E I _ E V E N T S . H

DESCRIPTION
  This is the header file for the PS interface event functions. This
  file is used for both ps_iface and phys_link events.

Copyright (c) 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_ifacei_event.h#1 $
  $DateTime: 2005/12/23 17:19:09

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ps_iface.h"
#include "ps_phys_link.h"
#include "queue.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
TYPEDEF PS_IFACE_EVENT_BUF_TYPE

DESCRIPTION
  This is the type that is placed on the event callback queues.
===========================================================================*/
typedef struct
{
  q_link_type               link;
  ps_iface_event_enum_type  event_name;
  q_type                   *event_q_ptr;
  ps_iface_event_cback_type event_cback_f_ptr;
  void                     *user_data_ptr;
} ps_iface_event_buf_type;

/*===========================================================================
TYPEDEF PS_IFACE_EVENT_PAYLOAD_TYPE

DESCRIPTION
  This is used for reporting the iface events to DIAG
===========================================================================*/
typedef struct
{
  ps_iface_name_enum_type  name;
  uint8                    instance;
  ps_iface_state_enum_type state;
} ps_iface_event_payload_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      PUBLIC FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_IFACEI_DORM_STATUS

DESCRIPTION
  Called by the underlying phys link when the phys link wants to either
  orig from dormancy or is going dormant.  Calls functions registered by
  IFACE mgr to return information to phys link.  If no functions are
  registered, then returns TRUE (i.e, meaning go dormant or ok to orig
  from dorm).

PARAMETERS
  phys_link_ptr:  Ptr to phys link control block on which is asking the
                  question.
  action:         Either wants to orig or wants to go dormant.
  reason:         Applies only for orig's.  Indicates why we are originating.
  unique_mem_ptr: Most likely (and should be) set to to the iface_ptr.

RETURN VALUE
  - 1:  Do not go dormant or do not orig from dormancy.
    0:  Ok to go dormant or ok to orig from dormancy.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_ifacei_dorm_status
(
  ps_phys_link_type               *phys_link_ptr,
  ps_iface_dorm_action_enum_type   action,
  ps_iface_dorm_reason_enum_type   reason,
  void                            *unique_mem_ptr
);

/*===========================================================================
FUNCTION PS_IFACEI_INVOKE_EVENT_CBACKS()

DESCRIPTION
  This function will invoke all of the event callbacks for a given interface
  and event.  It will also call the global callbacks for said event, if any.

PARAMETERS
  this_iface_ptr: ptr to the interface on which we are operating
  event: the event for which callbacks need to be called
  event_info: event information that needs to be passed into callback.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_invoke_event_cbacks
(
  ps_iface_type             *this_iface_ptr,
  ps_phys_link_type         *this_phys_link_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info
);

/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_EVENT_CBACK()

DESCRIPTION
  This event callback will be registered by ps_iface as a global event
  callback with phys link. This function will invoke all the global phys link
  event callbacks registed via ps_iface and also the iface specific phys link
  event callbacks.

PARAMETERS
  this_phys_link_ptr: ptr to the phys link on which we are operating
  this_iface_ptr: ptr to the interface on which we are operating
  event: the event for which callbacks need to be called
  event_info: event information that needs to be passed into callback.
  user_data_ptr: Pointer to user data info

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_phys_link_event_cback
(
  ps_phys_link_type         *this_phys_link_ptr,
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
);

#endif /* PS_IFACEI_EVENT_H */
