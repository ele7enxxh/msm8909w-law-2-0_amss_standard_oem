#ifndef PS_IFACEI_UTILS_H
#define PS_IFACEI_UTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I F A C E I _ U T I L S . H


GENERAL DESCRIPTION
  This file contains iface macros and other utilities used only by internal
  ps_iface functions.


Copyright (c) 2004-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_ifacei_utils.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
03/24/15    skc    Added PS_IFACE_SET_V6_IID_EX api
02/04/13    svj    New DIAG event to generate data cal status information  
09/22/11    am     Added #s for optimized DL config var access in iface_private.
01/07/11    cp     Removed the use of first hop mtu value.
08/16/10    cp     Propagating the first hop mtu value to TE.
12/14/08    pp     Common Modem Interface: Public/Private API split.
09/12/08    pp     Metainfo optimizations.
04/25/06    msr    L4/Tasklock code review changes
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
12/05/05    msr    Changed PS_IFACEI_NUM_FILTERS()
08/15/05    mct    Fixed naming causing some issues w/C++ compilers.
05/02/05    msr    Wrapped macros in parenthesis.
04/17/05    msr    Changed PS_IFACEI_IS_LOGICAL() and added
                   PS_IFACEI_GET_STATE().
04/16/05    ks     Checking for NULL state in PS_IFACEI_PHYS_LINK_IS_GONE()
03/01/05    sv     Fixed IFACEI_FLOW macros to return only the iface flow mask.
11/19/04    msr    Added more macros.
10/25/04    msr    Changed PS_IFACEI_[GS]ET_GRACEFUL_CLOSE macros to
                   manipulate "graceful_dormant_close" variable stored in
                   ps_iface. Fixed PS_IFACEI_GET_PHYS_LINK macro to inlude
                   the value of private.phys_link.primary
08/02/04    mct    created file.
===========================================================================*/

#include "queue.h"

#include "ps_crit_sect.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      PUBLIC MACRO/FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
MACRO PS_IFACEI_INC_NUM_SEC_FLOWS()

DESCRIPTION
  This macro increments the number of secondary flows associated with this
  iface.

PARAMETERS
  iface_ptr : Target iface ptr

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACEI_INC_NUM_SEC_FLOWS(iface_ptr)                           \
  ((iface_ptr)->iface_private.flow.num_sec_flows++)

/*===========================================================================
MACRO PS_IFACEI_DEC_NUM_SEC_FLOWS()

DESCRIPTION
  This macro decrements the number of secondary flows associated with this
  iface.

PARAMETERS
  iface_ptr : Target iface ptr

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACEI_DEC_NUM_SEC_FLOWS(iface_ptr)                           \
  ((iface_ptr)->iface_private.flow.num_sec_flows--)

/*===========================================================================
MACRO PS_IFACEI_INC_REF_CNT()

DESCRIPTION
  This macro increments the ref_cnt for a given priority. If ref_cnt
  transitions from 0 to 1, it also sets corresponding bit in the mask

PARAMETERS
  iface_ptr : Target iface ptr
  priority  : priority of the client which is using the iface

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACEI_INC_REF_CNT(iface_ptr, priority)                       \
  {                                                                      \
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                      \
    (iface_ptr)->iface_private.ref_cnt[priority]++;                      \
    if ((iface_ptr)->iface_private.ref_cnt[priority] == 1)               \
    {                                                                    \
      (iface_ptr)->iface_private.priority_mask |= (1 << (priority));     \
    }                                                                    \
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                      \
  }

/*===========================================================================
MACRO PS_IFACEI_DEC_REF_CNT()

DESCRIPTION
  This macro decrements the ref_cnt for a given priority. If ref_cnt
  transitions from 1 to 0, it also clears corresponding bit in the mask

PARAMETERS
  iface_ptr : Target iface ptr
  priority  : priority of the client which is using the iface

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACEI_DEC_REF_CNT(iface_ptr, priority)                       \
  if ((iface_ptr)->iface_private.ref_cnt[priority] > 0)                  \
  {                                                                      \
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                      \
    (iface_ptr)->iface_private.ref_cnt[priority]--;                      \
    if ((iface_ptr)->iface_private.ref_cnt[priority] == 0)               \
    {                                                                    \
      (iface_ptr)->iface_private.priority_mask &= ~((uint32)1 << (priority));    \
    }                                                                    \
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                      \
  }


/*===========================================================================
MACRO PS_IFACEI_GET_GRACEFUL_CLOSE()

DESCRIPTION
  This macro will return the value of the graceful_dormant_close variable in
  the primary phys link associated with the specified iface.

PARAMETERS
  iface_ptr: pointer to the interface.

RETURN VALUE
  Value of graceful_dormant_close variable.
===========================================================================*/
#define PS_IFACEI_GET_GRACEFUL_CLOSE( iface_ptr )                     \
   (iface_ptr)->iface_private.graceful_dormant_close;

/*===========================================================================
MACRO PS_IFACEI_GET_PRIORITY_MASK()

DESCRIPTION
  This macro gets the priority_mask of the iface.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  priority_mask
===========================================================================*/
#define PS_IFACEI_GET_PRIORITY_MASK( iface_ptr )                          \
  ((iface_ptr)->iface_private.priority_mask)

/*===========================================================================
MACRO PS_IFACEI_SET_GRACEFUL_CLOSE()

DESCRIPTION
  This macro will set the value of the graceful_dormant_close variable in the
  primary phys link associated with the specified iface.

PARAMETERS
  iface_ptr: pointer to the interface.
  boolean:   value of graceful_dormant_close

RETURN VALUE
  pointer to primary phys link.
===========================================================================*/
#define PS_IFACEI_SET_GRACEFUL_CLOSE( iface_ptr, graceful_close )     \
        (iface_ptr)->iface_private.graceful_dormant_close = graceful_close;

/*===========================================================================
MACRO PS_IFACEI_PHYS_STATE()

DESCRIPTION
  This macro returns the state of the interface that was passed in.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  the state of the passed in interface.
===========================================================================*/
#define PS_IFACEI_PHYS_LINK_STATE  ps_ifacei_phys_link_state
#define ps_ifacei_phys_link_state( iface_ptr )                   \
        PS_PHYS_LINK_GET_STATE(PS_IFACEI_GET_PHYS_LINK(iface_ptr))

/*===========================================================================
MACRO PS_IFACEI_GET_COOKIE()

DESCRIPTION
  This macro returns the interface cookie, which is unique per interface.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  the cookie for the passed in interface
===========================================================================*/
#define PS_IFACEI_GET_COOKIE  ps_ifacei_get_cookie
#define ps_ifacei_get_cookie( iface_ptr )        \
  (iface_ptr)->iface_private.cookie

/*===========================================================================
MACRO PS_IFACEI_IP_V4_ADDR()

DESCRIPTION
  This macro returns the IPv4 address assigne to the interface.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  IP address, 0 if interface pointer is NULL
===========================================================================*/
#define PS_IFACEI_IP_V4_ADDR( iface_ptr )                              \
   ps_iface_get_v4_addr(iface_ptr)

/*===========================================================================
MACRO PS_IFACEI_GET_IP_V6_IID()

DESCRIPTION
  This macro returns the IPv6 interface identifier (the v6 address suffix)
assigned to the interface (this is NOT the same as ps_iface).

PARAMETERS
  iface_ptr: pointer to the ps_iface interface in question.

RETURN VALUE
  IPv6 interface identifier (last 64 bits of the address), 0 if interface
pointer is NULL
===========================================================================*/
#define PS_IFACEI_GET_IP_V6_IID( iface_ptr )                     \
   ps_iface_get_v6_iid(iface_ptr)

/*===========================================================================
MACRO PS_IFACEI_SET_IP_V6_IID_EX()

DESCRIPTION
  This macro sets the IPv6 interface identifier (the v6 address suffix) in
the interface.

PARAMETERS
  iface_ptr: pointer to the interface in question.
  iid:       64-bit IPv6 interface identifier (the v6 address suffix)
  addfltr:   Flag to indicate if filter should be added.


RETURN VALUE
  FALSE if interface pointer is NULL, TRUE otherwise
===========================================================================*/
#define PS_IFACEI_SET_IP_V6_IID_EX( iface_ptr, iid, addfltr)               \
   ps_iface_set_v6_iid_ex(iface_ptr,iid,addfltr)

/*===========================================================================
MACRO PS_IFACEI_SET_IP_V6_IID()

DESCRIPTION
  This macro sets the IPv6 interface identifier (the v6 address suffix) in
the interface.

PARAMETERS
  iface_ptr: pointer to the interface in question.
  iid:       64-bit IPv6 interface identifier (the v6 address suffix)

RETURN VALUE
  FALSE if interface pointer is NULL, TRUE otherwise
===========================================================================*/
#define PS_IFACEI_SET_IP_V6_IID( iface_ptr, iid )                        \
   ps_iface_set_v6_iid(iface_ptr,iid)

/*===========================================================================
MACRO PS_IFACEI_SET_PEER_IP_V6_IID()

DESCRIPTION
  This macro sets the IPv6 interface identifier (the v6 address suffix)
  in the interface for the peer.

PARAMETERS
  iface_ptr: pointer to the interface in question.
  iid:       64-bit peer's IPv6 interface identifier (the v6 address suffix)

RETURN VALUE
  FALSE if interface pointer is NULL, TRUE otherwise
===========================================================================*/
#define PS_IFACEI_SET_PEER_IP_V6_IID( iface_ptr, iid )                   \
  ((iface_ptr)->v6_net_info.gateway_iid = (iid))

/*===========================================================================
MACRO PS_IFACEI_GET_BRIDGE_IFACE()

DESCRIPTION
  This macro will call the transmit of the interface that is passed in.

PARAMETERS
  iface_ptr: pointer to the interface this is bridged to.

RETURN VALUE
  NULL: if not set
  the address of the interface if it is set.
===========================================================================*/
#define PS_IFACEI_GET_BRIDGE_IFACE(iface_ptr)                            \
  (iface_ptr)->iface_private.tx_bridge_ptr

/*===========================================================================
MACRO PS_IFACEI_GET_FLOW_MASK()

DESCRIPTION
  This macro returns the flow mask of the passed in interface.

PARAMETERS
  iface_ptr: pointer to the interface whose flow mask is requested.

RETURN VALUE
  The flow mask of the interface in question

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_GET_FLOW_MASK  ps_ifacei_get_flow_mask
#define ps_ifacei_get_flow_mask( iface_ptr )                              \
        ((iface_ptr)->iface_private.tx_flow_mask )

/*===========================================================================
MACRO PS_IFACE_REG_SDB_STATUS_HANDLER()

DESCRIPTION
  This macro registers the specified fnc_ptr as the handler for the sdb
  status callback.

PARAMETERS
  iface_ptr: pointer to the interface whose flow mask is requested.
  fnc_ptr  : pointer to the function to be registered

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_REG_SDB_STATUS_HANDLER  ps_ifacei_reg_sdb_handler
#define ps_ifacei_reg_sdb_status_handler( iface_ptr, fcn_ptr )              \
  (PS_IFACEI_IS_LOGICAL(iface_ptr) ?                                        \
                 ((PS_IFACEI_GET_ASSOC_IFACE(iface_ptr))                    \
                         ->iface_private.sdb_status_handler_sock_fcn = fcn_ptr)   \
                         :                                                  \
                ((iface_ptr)->iface_private.sdb_status_handler_sock_fcn = fcn_ptr))

/*===========================================================================
FUNCTION PS_IFACEI_PHYS_LINK_IS_GONE

DESCRIPTION
  This function will look at the primary phys link for the interface and
  determine whether or not it is down and NOT dormant (i.e. gone).

PARAMETERS
  this_iface_ptr: iface which you are operating on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_PHYS_LINK_IS_GONE(this_iface_ptr)                        \
  (PS_IFACEI_PHYS_LINK_STATE(this_iface_ptr) == PHYS_LINK_NULL)

/*===========================================================================
MACRO PS_IFACEI_HW_ADDR_PTR()

DESCRIPTION
  This macro returns the pointer to the ps_iface's private hardware address

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  HW address
===========================================================================*/
#define PS_IFACEI_HW_ADDR_PTR( iface_ptr )  (iface_ptr)->iface_private.hw_addr

/*===========================================================================
MACRO PS_IFACEI_HW_ADDR_LEN()

DESCRIPTION
  This macro returns the specified interface's hardware address length

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  Length of HW address
===========================================================================*/
#define PS_IFACEI_HW_ADDR_LEN( iface_ptr )  (iface_ptr)->iface_private.hw_addr_len

/*===========================================================================
MACRO PS_IFACEI_SET_HW_ADDR_LEN()

DESCRIPTION
  This macro sets the specified interface's hardware address length

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACEI_SET_HW_ADDR_LEN( iface_ptr, len )                     \
  (iface_ptr)->iface_private.hw_addr_len = len

/*===========================================================================
MACRO PS_IFACEI_HW_ADDR_LEN_MAX()

DESCRIPTION
  This macro returns the maximum length of the specified interface's
  hardware address

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  Maximum length of HW address
===========================================================================*/
#define PS_IFACEI_HW_ADDR_LEN_MAX( iface_ptr )                              \
  sizeof((iface_ptr)->iface_private.hw_addr)

/*===========================================================================
MACRO PS_IFACEI_HANDLE_FLOW_CTL()

DESCRIPTION
  This macro will free a packet if the interface passed in is flow
  controlled, and will return -1

PARAMETERS
  iface_ptr: pointer to the interface on which to TX.
  pkt_ptr_ptr:  dsm item that was being Txed
  meta_info_ptr: meta infor for the packet being TXed
===========================================================================*/
#define PS_IFACEI_HANDLE_FLOW_CTL(iface_ptr, pkt_ptr_ptr, meta_info_ptr)    \
  if(iface_ptr->iface_private.tx_flow_mask != 0)                                  \
  {                                                                         \
    PS_TX_META_INFO_FREE(&meta_info_ptr);                                      \
    dsm_free_packet(pkt_chain_ptr);                                         \
                                                                            \
    return -1;                                                              \
  }

/*===========================================================================
MACRO PS_IFACEI_IS_UP()

DESCRIPTION
  This macro returns is the specified iface is up or in the process of
  coming up. Note that it returns false if the iface is in the process
  of going down.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  TRUE: if iface is UP, COMING_UP, CONFIGURING or ROUTEABLE
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_IS_UP( iface_ptr )                                  \
  ((iface_ptr)->iface_private.state == IFACE_COMING_UP   ||           \
   (iface_ptr)->iface_private.state == IFACE_ROUTEABLE   ||           \
   (iface_ptr)->iface_private.state == IFACE_CONFIGURING ||           \
   (iface_ptr)->iface_private.state == IFACE_UP)

/*===========================================================================
MACRO PS_IFACEI_IS_IN_USE()

DESCRIPTION
  This macro returns the in_use flag of the iface.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  TRUE: if iface is either up or in the process of coming up or has been
        acquired by one or more entities to bring it up.
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_IS_IN_USE( iface_ptr )  ((iface_ptr)->iface_private.in_use)

/*===========================================================================
MACRO PS_IFACEI_SET_IN_USE()

DESCRIPTION
  This macro sets the iface in_use flag to TRUE.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_IN_USE( iface_ptr )                         \
  (iface_ptr)->iface_private.in_use = TRUE;

/*===========================================================================
MACRO PS_IFACEI_RESET_IN_USE()

DESCRIPTION
  This macro sets the iface in_use flag to FALSE.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_RESET_IN_USE( iface_ptr )                         \
  (iface_ptr)->iface_private.in_use = FALSE;

/*===========================================================================
MACRO PS_IFACEI_SET_PROC_ID()

DESCRIPTION
  This macro sets proc_id in the iface

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_PROC_ID( iface_ptr, user_proc_id )                 \
  (iface_ptr)->iface_private.proc_id = user_proc_id;

/*===========================================================================
MACRO PS_IFACEI_RESET_PROC_ID()

DESCRIPTION
  This macro resets the proc_id in iface

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_RESET_PROC_ID( iface_ptr )                             \
  (iface_ptr)->iface_private.proc_id = PS_IFACE_PROC_ID_NONE;

/*===========================================================================
MACRO PS_IFACEI_SET_SUBS_ID()

DESCRIPTION
  This macro sets subs id(Subscription ID) in the iface

PARAMETERS
  iface_ptr: Target iface ptr
  subs_id: Subscription ID

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_SUBS_ID( iface_ptr, user_subs_id )                 \
  (iface_ptr)->iface_private.subs_id = user_subs_id;

/*===========================================================================
MACRO PS_IFACEI_SET_3GPP_PROFILE_ID()

DESCRIPTION
  This macro sets profile_id_3gpp in the iface

PARAMETERS
  iface_ptr: Target iface ptr
  profile_id: 3gpp profile identifier

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_3GPP_PROFILE_ID( iface_ptr, profile_id  )          \
  (iface_ptr)->iface_private.profile_id_3gpp = profile_id;           
  
/*===========================================================================
MACRO PS_IFACEI_RESET_3GPP_PROFILE_ID()

DESCRIPTION
  This macro resets the profile_id_3gpp in iface

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_RESET_3GPP_PROFILE_ID( iface_ptr )                     \
  (iface_ptr)->iface_private.profile_id_3gpp = 0xFFFFFFFFUL;

/*===========================================================================
MACRO PS_IFACEI_GET_3GPP_PROFILE_ID()

DESCRIPTION
  Returns the 3gpp profile id

PARAMETERS
  iface_ptr   : pointer to iface

RETURN VALUE
  3gpp profile ID
===========================================================================*/
#define PS_IFACEI_GET_3GPP_PROFILE_ID(iface_ptr)                          \
  (iface_ptr)->iface_private.profile_id_3gpp

/*===========================================================================
MACRO PS_IFACEI_SET_3GPP2_PROFILE_ID()

DESCRIPTION
  This macro sets profile_id_3gpp2 in the iface

PARAMETERS
  iface_ptr: Target iface ptr
  profile_id: 3gpp2 profile identifier

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_3GPP2_PROFILE_ID( iface_ptr, profile_id  )         \
  (iface_ptr)->iface_private.profile_id_3gpp2 = profile_id;          
  
/*===========================================================================
MACRO PS_IFACEI_RESET_3GPP2_PROFILE_ID()

DESCRIPTION
  This macro resets the profile_id_3gpp2 in iface

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_RESET_3GPP2_PROFILE_ID( iface_ptr )                    \
  (iface_ptr)->iface_private.profile_id_3gpp2 = 0xFFFFFFFFUL;

/*===========================================================================
MACRO PS_IFACEI_SET_RM_INST_HANDLE()

DESCRIPTION
  This macro saves the rmnet instance in iface

PARAMETERS
  iface_ptr   : Target iface ptr
  rm_instance : Rmnet handle that uniquely identifies the port

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_RM_INST_HANDLE( iface_ptr, rm_instance )          \
  (iface_ptr)->iface_private.rm_inst_handle = rm_instance;

/*===========================================================================
MACRO PS_IFACEI_RESET_RM_INST_HANDLE()

DESCRIPTION
  This macro resets the rmnet instance in iface

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_RESET_RM_INST_HANDLE( iface_ptr )                    \
  (iface_ptr)->iface_private.rm_inst_handle = -1;

/*===========================================================================
MACRO PS_IFACEI_GET_3GPP2_PROFILE_ID()

DESCRIPTION
  Returns the 3gpp2 profile id

PARAMETERS
  iface_ptr   : pointer to iface

RETURN VALUE
  3gpp profile ID
===========================================================================*/
#define PS_IFACEI_GET_3GPP2_PROFILE_ID(iface_ptr)                          \
  (iface_ptr)->iface_private.profile_id_3gpp2

/*===========================================================================
MACRO PS_IFACEI_IS_LOOKED_UP()

DESCRIPTION
  This macro returns the iface looked_up flag.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  TRUE: if the iface was picked up by a previous routing lookup
        (for something other than bring up)
  FALSE: either iface was never picked by a routing lookup or was picked up
         and acquired for bring up.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_IS_LOOKED_UP( iface_ptr )                      \
  ((iface_ptr)->iface_private.looked_up)

/*===========================================================================
MACRO PS_IFACEI_SET_LOOKED_UP()

DESCRIPTION
  This macro sets the iface looked_up flag to TRUE.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_LOOKED_UP( iface_ptr )                      \
  (iface_ptr)->iface_private.looked_up = TRUE;

/*===========================================================================
MACRO PS_IFACEI_RESET_LOOKED_UP()

DESCRIPTION
  This macro sets the iface looked_up flag to FALSE.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_RESET_LOOKED_UP( iface_ptr )                      \
  (iface_ptr)->iface_private.looked_up = FALSE;

/*===========================================================================
MACRO PS_IFACEI_SET_CAPABILITY()

DESCRIPTION
  Sets a capability of an iface.

  NOTE : Only one capability can be set each time.

PARAMETERS
  iface_ptr   : pointer to iface
  capability : capability which needs to be set

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACEI_SET_CAPABILITY(iface_ptr, capability)                    \
  ((iface_ptr)->iface_private.capability_mask |= (0x1 << (capability)))

/*===========================================================================
MACRO PS_IFACEI_RESET_CAPABILITY()

DESCRIPTION
  Resets a capability of an iface.

  NOTE : Only one capability can be reset each time.

PARAMETERS
  iface_ptr   : pointer to iface
  capability : capability which needs to be reset

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACEI_RESET_CAPABILITY(iface_ptr, capability)                  \
  ((iface_ptr)->iface_private.capability_mask &= ~(0x1 << (capability)))

/*===========================================================================
MACRO PS_IFACEI_GET_CAPABILITY()

DESCRIPTION
  Returns TRUE if a capability of an iface is turned on.

  NOTE : Only one capability can be fetched each time.

PARAMETERS
  iface_ptr   : pointer to iface
  capability : capability which needs to be tested

RETURN VALUE
  TRUE  : if capability is turned on
  FALSE : otherwise
===========================================================================*/
#define PS_IFACEI_GET_CAPABILITY(iface_ptr, capability)                    \
  ((iface_ptr)->iface_private.capability_mask & (0x1 << (capability)))

/*===========================================================================
MACRO PS_IFACEI_IS_IN_ARBITRATION()

DESCRIPTION
  This macro returns boolean to indicate whether Iface is being arbitrated
  or not

PARAMETERS
  iface_ptr : Target iface ptr

RETURN VALUE
  mtu : First Hop MTU of the iface
  0   : if iface is invalid

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_IS_IN_ARBITRATION( iface_ptr )                            \
  (iface_ptr)->iface_private.arbitration_info.in_progress

/*===========================================================================
MACRO PS_IFACEI_SET_ARBITRATION_IN_PROGRESS()

DESCRIPTION
  This macro sets arbitration in-progress flag

PARAMETERS
  iface_ptr : Target iface ptr

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_SET_ARBITRATION_IN_PROGRESS( iface_ptr )                  \
  (iface_ptr)->iface_private.arbitration_info.in_progress = TRUE;             

/*===========================================================================
MACRO PS_IFACEI_RESET_ARBITRATION_IN_PROGRESS()

DESCRIPTION
  This macro sets arbitration in-progress flag

PARAMETERS
  iface_ptr : Target iface ptr

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_RESET_ARBITRATION_IN_PROGRESS( iface_ptr )                \
  (iface_ptr)->iface_private.arbitration_info.in_progress = FALSE;

/*===========================================================================
MACRO PS_IFACEI_GET_OPT_FILTER_BASE()

DESCRIPTION
  Returns the base addr for optimized filter array. 

  NOTE : Particular to optimized data path only.

PARAMETERS
  iface_ptr   : pointer to iface

RETURN VALUE
  Optimized filters list head ptr
===========================================================================*/
#define PS_IFACEI_GET_OPT_FILTER_BASE(iface_ptr)                            \
  (iface_ptr)->iface_private.dl_opt_cfg.fltr_arr

/*===========================================================================
MACRO PS_IFACEI_SET_OPT_FILTER_BASE()

DESCRIPTION
  Sets the base addr for optimized filter array.

  NOTE : Particular to optimized data path only.

PARAMETERS
  iface_ptr   : pointer to iface

RETURN VALUE
  Optimized filters list head ptr
===========================================================================*/
#define PS_IFACEI_SET_OPT_FILTER_BASE(iface_ptr, fltr_base_ptr)                            \
  (iface_ptr)->iface_private.dl_opt_cfg.fltr_arr = fltr_base_ptr;

/*===========================================================================
MACRO PS_IFACEI_GET_OPT_BRIDGE_IFACE()

DESCRIPTION
  Returns the Rm iface in DL cfg for physical Um iface. 

  NOTE : Particular to optimized data path only. Executes in data path.

PARAMETERS
  iface_ptr   : pointer to iface

RETURN VALUE
  bridge_iface_ptr : pointer to bridged Rm iface
===========================================================================*/
#define PS_IFACEI_GET_OPT_RM_IFACE(iface_ptr)                            \
  (iface_ptr)->iface_private.dl_opt_cfg.rm_iface_ptr

/*===========================================================================
MACRO PS_IFACEI_SET_OPT_BRIDGE_IFACE()

DESCRIPTION
  Sets the Rm iface in DL cfg for physical Um iface only if its RmNet call.

  NOTE : Particular to optimized data path only. Executes in control path.

PARAMETERS
  iface_ptr        : pointer to iface
  bridge_iface_ptr : pointer to bridged Rm iface

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACEI_SET_OPT_RM_IFACE(iface_ptr, bridge_iface_ptr)             \
{                                                                           \
  ps_iface_type *tmp_iface_ptr = bridge_iface_ptr;                          \
  if(iface_ptr != NULL)                                                     \
  {                                                                         \
    if (PS_IFACE_IS_VALID(tmp_iface_ptr) &&                                 \
        (tmp_iface_ptr)->iface_private.proc_id > 1)                         \
    {                                                                       \
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                       \
      (iface_ptr)->iface_private.dl_opt_cfg.rm_iface_ptr = tmp_iface_ptr;   \
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                       \
    }                                                                       \
    else                                                                    \
    {                                                                       \
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                       \
      (iface_ptr)->iface_private.dl_opt_cfg.rm_iface_ptr = NULL;            \
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                       \
    }                                                                       \
  }                                                                         \
}

#endif /* PS_IFACEI_UTILS_H */
