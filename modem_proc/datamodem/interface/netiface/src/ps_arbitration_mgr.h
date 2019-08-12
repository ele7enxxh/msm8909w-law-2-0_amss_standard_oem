#ifndef PS_ARBITRATION_MGR_H
#define PS_ARBITRATION_MGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   P S _ A R B I T R A T I O N _ M G R . H


GENERAL DESCRIPTION
  This file defines the API exported by arbitration manager module

EXTERNAL FUNCTIONS
  PS_ARBITRATION_MGR_NET_ARBITRATE()
    Checks if apps on some interface can be preempted to support high priority
    app

Copyright (c) 2007-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_arbitration_mgr.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"



/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  TYPEDEF PS_ARBITRATION_EVENT_ENUM_TYPE

  Type enumerating the possible ps arbitration events to be generated.
---------------------------------------------------------------------------*/

typedef enum
{
  EVENT_PS_ARBITRATION_APP_GRANTED_IFACE = 0, /* Low priority followed by
                                                 high priority app */
  EVENT_PS_ARBITRATION_APP_DENIED_IFACE  = 1, /* High priority followed by
                                                 low priority app*/
  EVENT_PS_ARBITRATION_APP_GRANTED_IFACE_LOOKUP  = 2,

  EVENT_PS_ARBITRATION_APP_DENIED_IFACE_LOOKUP   = 3,

  EVENT_PS_ARBITRATION_MAX               = 255 /* To make byte aligned */
} ps_arbitration_event_enum_type;


/*---------------------------------------------------------------------------
  TYPEDEF PS_ARBITRATION_EVENT_PAYLOAD_TYPE

  PS Arbitration event payload type. 
---------------------------------------------------------------------------*/
typedef struct
{
  uint64                          app_type;
  int32                           app_priority;
  int32                           profile_id;
  ps_arbitration_event_enum_type  ps_arbitration_event;
} ps_arbitration_event_payload_type;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_ARBITRATION_MGR_NET_ARBITRATE()

DESCRIPTION
  This function arbitrates among a list of interfaces which are already in
  use by some other application. Checks if applications on any interface
  can be preempted to support the passed in app

PARAMETERS
  acl_pi_ptr            : Policy info of the app which wants to come up
  iface_in_use_ptr_arr  : Array of interfaces which are already in use
  num_iface_in_use      : Number of elements in iface_in_use_ptr_arr

RETURN VALUE
  ps_iface_ptr : on success
  NULL         : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  If a suitable interface is found, it is torn down in order to preempt
  all the apps using that interface
===========================================================================*/
ps_iface_type *ps_arbitration_mgr_net_arbitrate
(
  acl_policy_info_type  * acl_pi_ptr,
  ps_iface_type        ** iface_in_use_ptr_arr,
  uint8                   num_iface_in_use
);

/*===========================================================================
FUNCTION PS_ARBITRATION_MGR_NET_ARBITRATE_LOOKUP()

DESCRIPTION
  This function arbitrates among a list of interfaces which are already in
  use by some other application. Just selects and Iface.

PARAMETERS
  acl_pi_ptr            : Policy info of the app which wants to come up
  iface_in_use_ptr_arr  : Array of interfaces which are already in use
  num_iface_in_use      : Number of elements in iface_in_use_ptr_arr

RETURN VALUE
  ps_iface_ptr : on success
  NULL         : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  If a suitable interface is found, it is torn down in order to preempt
  all the apps using that interface
===========================================================================*/
ps_iface_type *ps_arbitration_mgr_net_arbitrate_lookup
(
  acl_policy_info_type  * acl_pi_ptr,
  ps_iface_type        ** iface_in_use_ptr_arr,
  uint8                   num_iface_in_use
);

#endif /* PS_ARBITRATION_MGR_H */
