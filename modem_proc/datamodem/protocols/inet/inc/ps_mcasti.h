#ifndef PS_MCASTI_H
#define PS_MCASTI_H
/*===========================================================================
             Internet Group Management Protocol (IGMP)
                            And
                Multicast Listener Discovery (MLD)

           I N T E R N A L   A P I   H E A D E R   F I L E

DESCRIPTION
 The IGMP and MLD internal API header file.

Copyright (c) 2006-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_mcasti.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/12/13    sd     Cryptographic vulnerability - replacing ran_next APIs.
07/05/13    ds     Added the structure definition from ps_mcast.h.
12/14/08    pp     Common Modem Interface: Public/Private API split.
06/26/07    rs     Added macros to support multicast functions.
11/15/06    rt     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/



#include "ps_mem.h"
#include "ps_in.h"
#include "ps_iface_mcast.h"
#include "ps_svc.h"
#include "pstimer.h"
#include "ps_mcast.h"
#include "ran.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  IGMP/MLD structure to send JOIN/LEAVE message.
---------------------------------------------------------------------------*/

typedef struct {
  ps_iface_type        * iface_ptr;
  ps_ip_addr_type        mcast_grp;
} ps_mcast_send_msg_type;

/*---------------------------------------------------------------------------
 All Router Multicast Group  224.0.0.2
---------------------------------------------------------------------------*/
#define PS_MCAST_ALL_ROUTER_IPV4 0x020000E0

/*---------------------------------------------------------------------------
 All Hosts Multicast Group 224.0.0.1
---------------------------------------------------------------------------*/
#define PS_MCAST_ALL_HOSTS_IPV4 0x010000E0

/*---------------------------------------------------------------------------
 Unsolicited Report Interval in milliseconds
---------------------------------------------------------------------------*/
#define MCAST_REPORT_INTERVAL  10000

/*---------------------------------------------------------------------------
 Version 1 router present timeout in milliseconds
---------------------------------------------------------------------------*/
#define MCAST_V1_PRESENT_INTERVAL  400000

/*---------------------------------------------------------------------------
 IGMPv1 querier response interval in milliseconds
---------------------------------------------------------------------------*/
#define MCAST_V1_RESPONSE_INTERVAL  10000

/*---------------------------------------------------------------------------
 Macros to check if two given addresses are equal. The first argument is
 of type struct ps_mcast_cb *, and the second argument is of type
 ps_ip_addr_type.
---------------------------------------------------------------------------*/
/* #define PS_MCAST_ARE_V4_ADDR_EQUAL (a, b)                                   \
 (((ps_mcast_cb_type *)(a))->mcast_grp.type == IPV4_ADDR                    \
  && ((ps_ip_addr_type)(b)).type == IPV4_ADDR                               \
  && ((ps_mcast_cb_type *)(a))->mcast_grp.addr.v4.ps_s_addr ==                 \
     ((ps_ip_addr_type)(b)).addr.v4.ps_s_addr) */

#define PS_MCAST_ARE_V4_ADDR_EQUAL(a,b)                                   \
 ((a->mcast_grp).type == IPV4_ADDR                                            \
  && b.type == IPV4_ADDR                                                    \
  && (a->mcast_grp).addr.v4.ps_s_addr ==                                         \
     b.addr.v4.ps_s_addr)

#define PS_MCAST_ARE_V6_ADDR_EQUAL(a,b)                                   \
  ((a->mcast_grp).type == IPV6_ADDR                                           \
  && b.type == IPV6_ADDR                                                    \
  && PS_IN6_ARE_ADDR_EQUAL((a->mcast_grp).addr.v6.ps_s6_addr, b.addr.v6.ps_s6_addr)) 

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION ps_mcast_set_report_timer 
DESCRIPTION
  This function sets multicast report timer corresponding to the given
  multicast group.

PARAMETERS
  if_mcast_grp: multicast group whose timer needs to be set
  response_time: response time within which report needs to be sent

RETURN VALUE
  None 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void ps_mcast_set_report_timer 
(
  struct ps_mcast_cb   * if_mcast_cb,
  int                    response_time
);

/*===========================================================================
FUNCTION PS_MCASTI_INIT()

DESCRIPTION
  Initializes ps mcast module

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mcasti_init
(
  void
);


/*===========================================================================

FUNCTION PS_MCASTI_SEND_REPORT_MSG

DESCRIPTION
  This function is used to send a IGMP/MLD JOIN request. This function checks
  the type of IP multicast group address and calls IGMP specific output
  function for IPv4 and MLD specific output function for IPv6.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_mcasti_send_report_msg
(
  ps_cmd_enum_type   cmd,
  void             * mcast_info
);


/*===========================================================================

FUNCTION PS_MCASTI_SEND_LEAVE_MSG

DESCRIPTION
  This function is used to send a IGMP/MLD LEAVE msg. This function checks
  the type of IP multicast group address and calls IGMP specific output
  function for IPv4 and MLD specific output function for IPv6.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_mcasti_send_leave_msg
(
  ps_cmd_enum_type   cmd,
  void             * mcast_info
);

/*===========================================================================

FUNCTION PS_MCASTI_JOIN

DESCRIPTION
  This function is used by generic (non-MBMS) applications to send a MCAST
  JOIN request. This function checks the type of IP multicast group address
  and calls IGMP specific output function for IPv4 and MLD specific ouput
  function for IPv6.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_mcasti_join
(
  ps_cmd_enum_type   cmd,
  void             * mcast_info
);


/*===========================================================================

FUNCTION PS_MCASTI_LEAVE

DESCRIPTION
  This function is used by generic (non-MBMS) applications to start MCAST
  LEAVE procedure for the specified multicast group on the given interface.
  This function calls another function to decrement the membership count of
  the group and send a leave group message if the membership count gets
  zero.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_mcasti_leave
(
  ps_cmd_enum_type   cmd,
  void             * mcast_info
);


/*===========================================================================
FUNCTION PS_MCAST_ADD_GROUP()

DESCRIPTION
  This function is called by the function ps_mcasti_join() when an
  application wants to join a group. In case the group is not already present
  in the multicast group list, this function adds the group to the list and
  sends a JOIN message after setting a timer. In case the group already
  existed, its membership count is incremented by one.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  mcast_grp: multicast group

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mcast_add_group
(
  ps_iface_type        *this_iface_ptr,
  ps_ip_addr_type       mcast_grp
);


/*===========================================================================
FUNCTION PS_MCAST_DELETE_GROUP()

DESCRIPTION
  This function is called by the function ps_mcasti_leave() when an
  application wants to leave a group. The function decrements the membership
  count of the group by one. If the resulting count becomes zero, the group
  is deleted from the multicast group list. This function also sends a LEAVE
  group message out.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  mcast_grp: multicast group

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mcast_delete_group
(
  ps_iface_type               *this_iface_ptr,
  ps_ip_addr_type             mcast_grp
);


/*===========================================================================
FUNCTION PS_MCAST_TIMEOUT_CB()

DESCRIPTION
  This function serves as a call back function and is called to send a JOIN
  message when the Unsolicited Report Interval timer expires. This function
  is also called by the function ps_mcast_add_group(). The only job
  of this function is to prepare a JOIN message and send it to the group
  being joined.

PARAMETERS
  arg: contains pointer to a member in the linked list of mcast groups

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mcast_timeout_cb
(
  void            * arg
);


/*===========================================================================
FUNCTION PS_MCAST_START_V1_TIMER()

DESCRIPTION
  This function is called by the function ps_iface_igmp_start_timers() when
  the querier is running IGMP v1. This function starts the version 1 router
  present timer.

PARAMETERS
  this_iface_ptr: interface pointer

RETURN VALUE
  0 on success and -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mcast_start_v1_timer
(
  ps_iface_type  * this_iface_ptr
);


/*===========================================================================
FUNCTION PS_MCAST_QUERIER_V1_CB()

DESCRIPTION
  This function serves as a call back function that gets called when the
  Version 1 router present timer expires. This function frees the timer
  handler associated with the interface so that subsequent membership
  reports are sent as IGMP v1 membership reports.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mcast_querier_v1_cb
(
  void * nullvalue
);


#endif  /* PS_MCASTI_H */
