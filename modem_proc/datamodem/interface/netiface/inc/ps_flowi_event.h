#ifndef PS_FLOWI_EVENT_H
#define PS_FLOWI_EVENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ F L O W I _ E V E N T S . H

DESCRIPTION
  This is the header file for the PS flow event functions.

EXTERNAL FUNCTIONS
  PS_FLOW_EVENT_INIT()
    Initializes the global queues for ps_flow

  PS_FLOWI_PHYS_LINK_EV_CBACK_REG()
    Registers a flow for all phys link state change events on the phys link
    it is bound to

  PS_FLOWI_PHYS_LINK_EV_CBACK_DEREG()
    Deregisters a flow for all phys link state change events on the phys link
    it is bound to.

  PS_FLOWI_INVOKE_EVENT_CBACKS()
    Invokes all of the event callbacks for a given flow and event

  PS_FLOWI_ASSOC_FLOW_EV_CBACK_REG
    This function registers a flow for events on its associated ps_flow

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_flowi_event.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/14/08    pp     Common Modem Interface: Public/Private API split.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"



#include "ps_iface_defs.h"
#include "ps_flow_event.h"
#include "queue.h"
#include "dcc_task_defs.h"

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
TYPEDEF PS_FLOW_EVENT_BUF_TYPE

DESCRIPTION
  This is the type that is placed on the event callback queues.
===========================================================================*/
typedef struct
{
  q_link_type                 link;
  ps_iface_event_enum_type    event_name;
  q_type                    * event_q_ptr;
  ps_flow_event_cback_type    event_cback_f_ptr;
  void                      * user_data_ptr;
} ps_flow_event_buf_type;

/*===========================================================================
TYPEDEF PS_LOGICAL_FLOW_ASSOC_FLOW_EV_CMD_TYPE

DESCRIPTION
  Structure to be used while posting DCC commands.
===========================================================================*/
typedef struct
{
  ps_flow_type                * assoc_flow_ptr;
  ps_iface_event_enum_type      event;
  ps_iface_event_info_u_type    event_info;
  uint8                         flow_cookie;
  uint8                         logical_flow_cookie;
  void                        * user_data_ptr;
} ps_logical_flow_assoc_flow_ev_cmd_type;

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_FLOW_EVENT_INIT()

DESCRIPTION
  This function initializes the global queues for ps_flow.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flow_event_init
(
  void
);

/*===========================================================================
FUNCTION PS_FLOWI_PHYS_LINK_EV_CBACK_REG()

DESCRIPTION
  This function registers a flow for all phys link state change events on
  the phys link it is bound to.

PARAMETERS
  flow_ptr : ptr to flow which is interested in phys link events

RETURN VALUE
  None

DEPENDENCIES
  flow_ptr must be valid and it must be bound to a phys link

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_phys_link_ev_cback_reg
(
  ps_flow_type  * flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_PHYS_LINK_EV_CBACK_DEREG()

DESCRIPTION
  This function de-registers a flow for all phys link state change events on
  the phys link it is bound to.

PARAMETERS
  flow_ptr : ptr to flow which is interested in phys link events

RETURN VALUE
  None

DEPENDENCIES
  flow_ptr must be valid and it must be bound to a phys link. If ps_low is
  not registered for phys link events, this function will result in crash

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_phys_link_ev_cback_dereg
(
  ps_flow_type  * flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_INVOKE_EVENT_CBACKS()

DESCRIPTION
  This function will invoke all of the event callbacks for a given flow
  and event.  It will also call the global callbacks for said event, if any.

PARAMETERS
  flow_ptr   : ptr to the flow on which we are operating
  event      : the event for which callbacks need to be called
  event_info : event information that needs to be passed into callback.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_invoke_event_cbacks
(
  ps_flow_type                * flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info
);

/*===========================================================================
FUNCTION PS_FLOWI_ASSOC_FLOW_EV_CBACK_REG

DESCRIPTION
  This function registers a flow for events on its associated ps_flow.
  Only the stable events (such as activated, suspended etc.) are registered.
  The transitory events (such as activating, suspending etc.) are not
  handled by a logical flow.

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
   0 : on success
  -1 : on failure (any of the passed in parameters is bad)

DEPENDENCIES
  flow_ptr must be valid and it must be associated to another flow_ptr

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_flowi_assoc_flow_ev_cback_reg
(
  ps_flow_type                        * flow_ptr,
  ps_flow_assoc_flow_event_cback_type   assoc_flow_event_cback_f_ptr,
  int16                               * ps_errno
);

/*===========================================================================
FUNCTION PS_FLOWI_ASSOC_FLOW_EV_CBACK_DEREG

DESCRIPTION
  This function deregisters a flow from events on its associated ps_flow.
  Only the stable events (such as activated, suspended etc.) are handled.
  The transitory events (such as activating, suspending etc.) are not
  handled by a logical flow.

PARAMETERS
  flow_ptr : ptr to flow

RETURN VALUE
   0 : on success
  -1 : on failure (any of the passed in parameters is bad)

DEPENDENCIES
  flow_ptr must be valid and it must be associated to another flow_ptr

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_assoc_flow_ev_cback_dereg
(
  ps_flow_type                        * flow_ptr
);

/*===========================================================================
FUNCTION PS_FLOWI_PROCESS_ASSOC_FLOW_EV()

DESCRIPTION
  This function receives all flow event callbacks registered by the logical
  flow with the physical flow.

PARAMETERS
  cmd           : Command paramter for this event handler,
                  DCC_LOGICAL_FLOW_ASSOC_FLOW_EV_CMD
  user_data_ptr : Ptr to the dcc command structure encapsulating
                  ds_logical_flow_ev_cb_cmd_type which contains the parameters from
                  ps_flowi_invoke_event_cbacks

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_flowi_process_assoc_flow_ev
(
  dcc_cmd_enum_type   cmd,
  void              * user_data_ptr
);

#endif /* PS_FLOWI_EVENT_H */
