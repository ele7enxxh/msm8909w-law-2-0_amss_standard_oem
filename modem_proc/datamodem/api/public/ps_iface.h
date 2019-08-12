#ifndef PS_IFACE_H
#define PS_IFACE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ I F A C E . H

DESCRIPTION
  Header file defining all of the data types and the interface control block
  for the ps_iface architecture.

HOW THE INTERFACE IS TO BE USED:
  There are three types of functions here:
  configuration (e.g. ps_iface_event_cback_reg())
  commands      (e.g. ps_iface_bring_up_cmd())
  indications   (e.g. ps_iface_up_ind())

  COMMANDS can be called by anyone: the intention is that they are to be used
  to request something of an interface (such as bringing it up).
  INDICATIONS and CONFIGURATION functions are intended to be called
  exclusively by the client/owner of the interface, so for example a mode
  specific handler should be the only person calling the up indication - and
  this should be done when the interface is up (e.g. for UMTS the PDP context
  is established, and PPP is established - for PDP Type PPP).

DEPENDENCIES
  None of these functions should EVER be called from Interrupt context!

EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2014-08-08 sc  Moved PS_IFACE_SET_IS_ACTIVE definition to  source file
  2014-04-09 ash Added Prefix Delegation support.
  2014-01-19 cx  Support for unused external addresses probe optimization
  2013-06-21 ash Support MTU upto 2000
  2013-02-04 svj New DIAG event to generate data cal status information
  2012-07-12 tw  Multi PDN over 1 RMNET   
  2012-21-11 svj Added support for Operator Reserved Protocol Information.
  2012-02-10 rt  Added additional field in ps_iface_addr_mgmt_cb_f_ptr_type
  2012-02-01 am  Adding port ranges to optimized filters.  
  2011-11-28 sun Adding support for DPL logging using log codes 
  2011-09-15 msr New API to post a cmd to PS task for flow control
  2011-09-14 am  Added structs for optimized DL path config.  
  08/31/11    hs     Added ioctl DSS_IFACE_IOCTL_GET_SUBNET_MASK. 
  2011-08-28 aa  Added struct qos_net_initiated_req
  2011-08-23 ag  Added the support to get NBNS addresses from the Iface.
  2011-08-12 msr Moved PS_IFACE_COMMON_IS_FLOW_ENABLED to non-API files so
                 that it can enter critical section to prevent race conditions
  2011-07-15 msr Ensuring that PS_IFACE_SET_MTU doesn't set MTU to a lower
                 value than what is defined by the spec
  2011-02-03 cp  Modified PS_IFACE_GET_MTU to get the mtu value from
                 the base Iface.
  2011-01-25 dm  Added callback fn for stats overriding.
  2010-10-23 asn Added blob in Iface private to hold arbitration info
  2010-08-19 ash Changed ps_iface_addr_mgmt_handle_type from int16 to int32
  2010-08-05 yz  Added support for iface stats adjustment function
  2010-07-30 bq  Move NAT related declaration into ps_nat.h.
  2010-07-20 vs  New macro to set/get firewall details
  2010-06-30 rt  IFACE struct modification to allow NAT.
  2010-06-28 ea  Added a field for the DNS cache queue.
  2010-05-20 ss  Added iface_capability enum, as  well as macros to
                 set/get/reset
  2010-05-19 msr Added a new variable to ps_iface to store the reason why
                 bring_up_cmd_f_ptr failed

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "queue.h"
#include "list.h"
#include "ps_iface_defs.h"
#include "ps_ip_netconfig.h"
#include "ps_iface_ioctl.h"
#include "ps_meta_info.h"
#include "ps_tx_meta_info.h"
#include "ps_rx_meta_info.h"
#include "pstimer.h"
#include "ps_iface_utils.h"
#include "ps_flow.h"
#include "ps_iface_flow.h"
#include "ps_phys_link.h"
#include "ps_policy_mgr.h"
#include "ps_iface_mcast.h"
#include "ps_logging_defs.h"
#include "ps_iface_addr_v6.h"
#include "ps_iface_addr_mgmt.h"
#include "IxErrno.h"
#include "ds_sys.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  GLOBAL_IFACE_PTR_ARRAY - Array of pointers to all of the the created
  interfaces on the system.  The semantics of this array are that empty
  slots have a value of 0, any other values indicate the slot points to an
  iface.
---------------------------------------------------------------------------*/
extern ps_iface_type *global_iface_ptr_array[MAX_SYSTEM_IFACES];

#define PS_IFACE_INVALID_NAME 0x00
#define PS_IFACE_INVALID_INSTANCE 0xFF
#define PS_IFACE_INVALID_IFACE_INDEX 0xFFFFFFFF

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_COOKIE_TYPE

DESCRIPTION
  type for the cookie that is used to identify an interface.
---------------------------------------------------------------------------*/
typedef uint16 ps_iface_cookie_type;

#define MAX_SYSTEM_MTU 2000      /* MAX system MTU supported by Modem      */
#define DEFAULT_DATA_UNIT 1500   /* used for MRU and MTU                   */
#define DEFAULT_IP6_DATA_UNIT 1280 /* Minimum MTU for IPv6                 */

#define PS_IFACE_MAX_HW_ADDR_LEN 6 /* Size of the hardware address in bytes */

#define PS_IFACE_MAX_MODEM_PCO_INFO 10 /* Max no of modem reserved pco records*/

/* Max lenght of App specific information in Operator reserved PCO */
#define MAX_OP_RES_PCO_APP_SPECIFIC_LEN 255

/*---------------------------------------------------------------------------
  Provides statistics adjustment to the current iface stats
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  pkts_rx;             /* rx pkt count adjustment */
  uint32  pkts_tx;             /* tx pkt count adjustment */
  uint64  bytes_rx;            /* rx byte count adjustment */
  uint64  bytes_tx;            /* tx byte count adjustment */
} ps_iface_stat_adjustment_type;

/*---------------------------------------------------------------------------
  Mode Handlers use this to provide the override values that can be used to
  override the current stats stored in PS iface.
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_iface_stats_type ps_iface_stat_override_type;

/*---------------------------------------------------------------------------
  Used by the Mode Handlers that have the PS stats Override functionality
  implemented to identify the operation type (GET or RESET).
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_STAT_OP_TYPE_GET   = 0,
  PS_IFACE_STAT_OP_TYPE_RESET = 1
} ps_iface_stat_override_op_enum_type;

/*===========================================================================
                               CMD F_PTR TYPES
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_CMD_F_PTR_TYPE

DESCRIPTION
  Definitition of the function for an interfaces command functions.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  client_data:    pointer to any client data that needs to be passed in

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef int (*ps_iface_cmd_f_ptr_type)
(
  ps_iface_type *this_iface_ptr,
  void *client_data
);

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_CMD_F_PTR_EX_TYPE

DESCRIPTION
  Definitition of the function for an interfaces command functions.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  app_priority:   priority of requesting application. range is [0,31]
  client_data:    pointer to any client data that needs to be passed in

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef int (*ps_iface_cmd_f_ptr_ex_type)
(
  ps_iface_type *this_iface_ptr,
  int32          app_priority,
  void          *client_data
);

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_HANDLE_EVENT_F_PTR_TYPE

DESCRIPTION
  Definitition of the function for an event handling for iface modehandlers

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  event_name:   Event name
  event_info:    Event info

RETURN VALUE
  0 : Event handled by modehandler
 -1 : Event not handled by modehandler
---------------------------------------------------------------------------*/
typedef int (*ps_iface_handle_event_f_ptr_type)
(
  ps_iface_type                        * this_iface_ptr,
  ps_iface_event_enum_type               event_name,
  ps_iface_event_info_u_type           * event_info
);

/*===========================================================================
TYPEDEF PS_IFACE_IOCTL_F_PTR_TYPE()

DESCRIPTION
  This function performs various operations on the given ps interface.
  Typically, these operations are to get or set a value.

DEPENDENCIES
  None.

PARAMETERS

  ps_iface_type*            - Pointer to ps_iface

  dss_iface_ioctl_type      - The operation name

  void*                     - Pointer to operation specific structure


  sint15*                   - Error code returned in case of failure (Error
                              values are those defined in dserrno.h)

                              DS_EBADF - Returned by dss_iface_ioctl() if the
                              specified id_ptr is invalid (i.e. id_ptr does
                              not map to a valid ps_iface_ptr).


                              DS_EINVAL - Returned by dss_iface_ioctl() when
                              the specified IOCTL does not belong to the
                              common set of IOCTLs and there is no IOCTL mode
                              handler registered for the specified interface.

                              DS_EOPNOTSUPP - Returned by the lower level
                              IOCTL mode handler when specified IOCTL is not
                              supported by the interface. For instance, this
                              would be returned by interfaces that do not
                              support a certain "iface specific common IOCTL"
                              (i.e. these are common IOCTLs, but the
                              implementation is mode specific, for example,
                              GO_DORMANT).

                              DS_EFAULT - This error code is returned if the
                              specified arguments for the IOCTL are correct
                              but dss_iface_ioctl() or a mode handler
                              encounters an error while executing the IOCTL..
                              For instance, if the 1X interface cannot
                              "GO_DORMANT" it would return this error.

                              DS_NOMEMORY - This error code is returned if we
                              run out of buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
typedef int (*ps_iface_ioctl_f_ptr_type)
(
  ps_iface_type            *this_iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
);


/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_BRIDGE_PROC_F_PTR_TYPE

DESCRIPTION
  Definitition of the function for an interfaces command functions.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  client_data:    pointer to any client data that needs to be passed in

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef int (*ps_iface_bridge_proc_f_ptr_type)
(
  ps_iface_type *this_iface_ptr,
  ps_iface_type *target_iface_ptr,
  void *client_data
);

typedef int (*ps_iface_bridge_target_proc_f_ptr_type)
(
  ps_iface_type *this_iface_ptr,
  ps_iface_type *src_iface_ptr,
  void *client_data
);

/*---------------------------------------------------------------------------
  Address management APIs
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_ADDR_MGMT_ADDR_DELETED       = 0,
  IFACE_ADDR_MGMT_ADDR_ADDED         = 1,
  IFACE_ADDR_MGMT_ADDR_UPDATED       = 2
} ps_iface_addr_mgmt_event_type;

/*---------------------------------------------------------------------------
  Handle to an IPv6 address management structure.
---------------------------------------------------------------------------*/
typedef int32 ps_iface_addr_mgmt_handle_type;

/*-----------------------------------------------------------------------------
  Address management function pointer types. The first is for address event
  callbacks and the second is the interface specific DAD function.
-----------------------------------------------------------------------------*/
/*===========================================================================
  TYPEDEF PS_IFACE_ADDR_MGMT_CB_F_PTR_TYPE()
===========================================================================*/
/** 
  @Description
  Typedef for address event callbacks.

  @param[in] ip_addr          : Address for which callback is being invoked
  @param[in] addr_event       : Address event for which callback is being 
                                invoked
  @param[in] linked_iface_ptr : Handle to the iface linked to this address-
                                this is the Rm iface for EXTERNAL address, 
                                and the WWAN iface for WWAN (Um) address 
                                stored on Soft-AP iface in soft-ap mode
  @param[in] user_data        : Cookie supplied during registration

  @return
  0 on success
 -1 on failure
  
  @dependencies 
  None.
*/
typedef int (*ps_iface_addr_mgmt_cb_f_ptr_type)
(
  ps_ip_addr_type                  ip_addr,
  ps_iface_addr_mgmt_event_type    addr_event,
  ps_iface_type *                  linked_iface_ptr,
  void                           * user_data
);

/*===========================================================================
  TYPEDEF PS_IFACE_ADDR_MGMT_DAD_F_PTR_TYPE()
===========================================================================*/
/** 
  @Description
  Typedef for address event callbacks.

  @param[in] ip_addr          : Address for which callback is being invoked
  @param[in] handle           : Handle to the address mgmt table
  @param[in] user_data        : Cookie supplied during registration

  @return
  0 on success
 -1 on failure
  
  @dependencies 
  None.
*/
typedef int (*ps_iface_addr_mgmt_dad_f_ptr_type)
(
  ps_iface_type                  * iface_ptr,
  ps_iface_addr_mgmt_handle_type * handle,
  void                           * user_data
);

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_TX_CMD_F_PTR_TYPE

DESCRIPTION
  Definition for the txmit cmd function of an interface.

  ALL MUST tx_cmds run in the context of PS task.

  Once this function is called - the pkt_chain_ptr & meta_info_ptr is the
  responsibility of the interface client and must be eventually freed by the
  interface client.  That is, the tx function MUST free the memory that is
  passed to it (the pkt_chain_ptr, and meta_info_ptr).

PARAMS
  this_iface_ptr:   Ptr to interface that this pkt was received on - and to
                 operate on.
  pkt_chain_ptr: Ptr to chain of dsm items that hold the payload that needs
                 to be transmitted.
  meta_info_ptr: Ptr to dsm item that contains the meta info for this pkt.
  tx_cmd_info:   info to pass to registered cmd handler

RETURN VALUE
  0 on success
 -1 on failure
---------------------------------------------------------------------------*/
typedef int (*ps_iface_tx_cmd_f_ptr_type)
(
  ps_iface_type        *this_iface_ptr,
  dsm_item_type       **pkt_chain_ptr,
  ps_tx_meta_info_type *meta_info_ptr,
  void                 *tx_cmd_info
);


/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IP_INPUT_F_PTR_TYPE

DESCRIPTION
  Definition for IP input function type.  This should be set to point to the
  input function of the appropriate IP stack (i.e. IPv4 or IPv6).

PARAMS
  this_iface_ptr:   Ptr to interface that this pkt was received on - and to
                 operate on.
  pkt_chain_ptr: Ptr to chain of dsm items that hold the payload that needs
                 to be transmitted.
  meta_info_ptr: Ptr to dsm item that contains the meta info for this pkt.
  tx_cmd_info:   info to pass to registered cmd handler

RETURN VALUE
  0 on success
 -1 on failure
---------------------------------------------------------------------------*/
typedef int (*ps_iface_ip_input_f_ptr_type)
(
  ps_iface_type  *in_iface_ptr,   /* Input interface                   */
  dsm_item_type  *rx_pkt,         /* Input packet                      */
  uint8          *source,         /* Media source address, if any      */
  int             broadcast,      /* Packet had link broadcast address */
  ps_rx_meta_info_type * meta_info_ptr /* RX meta info ptr             */
);


/*===========================================================================
                             EVENT RELATED STUFF
===========================================================================*/

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_EVENT_CBACK_TYPE

DESCRIPTION
  Defines for the event callbacks. There can be any number of events per
  interface and the memory is owned by the clients.

PARAMS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  event: the event being registered for
  event_info: information specific to the event: for details look at typedef
  user_data_ptr: data to pass back to the user

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ps_iface_event_cback_type)
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
);

/*===========================================================================
                            STATISTCS RELATED STUFF
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_STAT_DESC_F_PTR_TYPE

DESCRIPTION
  Definitition of the function called when iface wants to fill a description.

PARAMETERS
  iface_ptr         : pointer to the iface control blocl
  log_pkt_ptr       : pointer to the log packet to fill with description
  log_pkt_avail_size: Remaining size of the log_pkt_ptr

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ps_iface_stat_desc_f_ptr_type)
(
  ps_iface_type * iface_ptr,
  uint8         * log_pkt_ptr,
  int32           log_pkt_avail_size
);
/*===========================================================================
TYPEDEF PS_IFACE_STAT_DESC_LEN_F_PTR_TYPE()

DESCRIPTION
  Definition of the function called when iface wants to determine how long
  the description is going to be

PARAMETERS
  iface_ptr: Pointer to the iface control block

RETURN VALUE
  Length

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
typedef int32 (*ps_iface_stat_desc_len_f_ptr_type)
(
  ps_iface_type * iface_ptr
);

/*===========================================================================
TYPEDEF PS_IFACE_STAT_ADJUSTMENT_F_PTR_TYPE()

DESCRIPTION
  Definition of the function called when iface wants to obtain stat
  adjustment from client

PARAMETERS
  iface_ptr:  Pointer to the iface control block
  adjustment: Pointer to the adjustment result

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
typedef void (*ps_iface_stat_adjustment_f_ptr_type)
(
  ps_iface_type                 *iface_ptr,
  ps_iface_stat_adjustment_type *adjustment
);

/*===========================================================================
                            DORMANCY RELATED STUFF
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_DORM_ACTION_F_PTR_TYPE

DESCRIPTION
  Definitition of the function called when phys link wants to know if it
  is okay to orig out of dormancy.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  this_phys_link_ptr: Phys link asking for dorm status
  action:         Either wants to orig or wants to go dormant.
  reason:         Indicates why we are originating.

RETURN VALUE
   0:  it is ok to orig.
  -1:  not ok to orig (do not orig)
---------------------------------------------------------------------------*/
typedef int (*ps_iface_dorm_action_f_ptr_type)
(
  ps_iface_type                   *this_iface_ptr,
  ps_phys_link_type               *phys_link_ptr,
  ps_iface_dorm_action_enum_type   action,
  ps_iface_dorm_reason_enum_type   reason
);

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_DATA_PROC_F_PTR_TYPE

DEFINITION
  Definition for the function type that is used to operate on a packet chain
  within the interface.  This could be used for example for the rx_proc
  function that is part of a logical interface, for instance to remove the
  encapsulation for a tunnel interface.

PARAMETERS
  pkt_chain_ptr: Ptr to packet chain on which to operate.

RETURN VALUE
  function specfic: see iface control block
---------------------------------------------------------------------------*/
typedef int (*ps_iface_data_proc_f_ptr_type)
(
  ps_iface_type        *this_iface_ptr,
  dsm_item_type       **pkt_ref_ptr,
  ps_rx_meta_info_type *meta_info_ptr
);

typedef struct
{
  list_link_type    link;
  ps_ip_addr_type   sip_addr;
} ps_iface_sip_serv_addr_type;

typedef struct
{
  list_link_type             link;
  ps_iface_domain_name_type  name;
}ps_iface_domain_name_node_type;

typedef ps_iface_domain_name_node_type
  ps_iface_sip_serv_domain_name_type;

typedef ps_iface_domain_name_node_type
  ps_iface_domain_name_search_list_node_type;


typedef struct
{
  uint16  mcc;
  uint16  mnc;
  boolean mnc_includes_pcs_digit;
  uint8 app_specific_info_len;
  uint8  app_specific_info[MAX_OP_RES_PCO_APP_SPECIFIC_LEN];
  uint16 container_id;
}ps_iface_op_reserved_pco_type;

/*---------------------------------------------------------------------------
  Modem reserved pco information type.
  num_pcos     : no of valid pco entries 
  pco_info_ptr : ptr to the pco information
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                          num_pcos;
  ps_iface_op_reserved_pco_type * pco_info_ptr;
} ps_iface_modem_reserved_pco_type;

/*---------------------------------------------------------------------------
  The enum type for Authentication preference(s)
  Max value 0xff is for INTERNAL USE ONLY
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_AUTH_PREF_NONE = 0x0000,         /* No authentication           */
  PS_IFACE_AUTH_PREF_PAP  = 0x0001,         /* PAP authentication          */
  PS_IFACE_AUTH_PREF_CHAP = 0x0002,         /* CHAP authentication         */
  PS_IFACE_AUTH_PREF_MAX  = 0xFFFF          /* force max to 0xFFFF so that
                                               enum is defined as a uint16 */
} ps_iface_auth_alg_pref_bitmask_e_type;

/*---------------------------------------------------------------------------
  The PS iface MIPv6 Bootstrap Configuration Info type
---------------------------------------------------------------------------*/
typedef struct
{
 struct ps_in6_addr mip6_home_agent_addr;
 struct ps_in6_addr mip6_home_addr;
 struct ps_in6_addr mip6_home_link_prefix;
 struct ps_in_addr  mip6_home_agent_ipv4_addr;
 uint32             hl_prefix_len;
 boolean            visited_nw_info;
} ps_iface_mip6_bootstrap_cfg_info_type;


/*---------------------------------------------------------------------------
  The PS iface Event Info Cache type
---------------------------------------------------------------------------*/
typedef struct iface_event_info_cache_s
{
  struct
  {
    ps_iface_ipfltr_client_id_enum_type  client;
    int                                  curr_fltr_cnt;
  } ipfltr_info;

  struct
  {
    ps_flow_type  * flow_ptr;
    uint8           prev_cnt;
    uint8           new_cnt;
  } rx_fltr_info;


  struct
  {
    ps_iface_addr_family_type  old_addr_family;
    ps_iface_addr_family_type  new_addr_family;
  } addr_family_info;

  struct
  {
    ps_iface_bearer_technology_type     old_bearer_tech;
    ps_iface_bearer_technology_type     new_bearer_tech;
  } bearer_tech_changed_info;

  ds_sys_system_status_info_type        bearer_tech_ex_changed_info;


  ps_iface_priv_ipv6_addr_info_type       priv_ipv6_addr;
  ps_iface_prefix_update_type             prefix_info;
  ps_ip_addr_type                         ip_addr;
  uint32                                  flow_mask;
  ps_iface_mt_handle_type                 mt_handle;
  ps_iface_mcast_event_info_type          mcast_info;
  ps_iface_state_enum_type                state;
  ps_extended_info_code_enum_type         qos_aware_info_code;
  ps_iface_rf_conditions_info_type        rf_conditions_change_info;
  ps_iface_outage_notification_event_info_type
                                          outage_notification_info;
  ps_hdr_rev0_rate_inertia_failure_code_enum_type
                                          hdr_rev0_rate_inertia_failure_code;
  ps_iface_ioctl_extended_ip_config_type  extended_ip_config_succeeded;

  struct
  {
    ps_hdr_slotted_mode_failure_code_enum_type  hdr_slotted_mode_failure_code;
    uint8                                       sm_current_sci;
  } slotted_mode_info;

  struct
  {
    ps_iface_state_enum_type       state;
    ps_iface_net_down_reason_type  netdown_reason;
  } iface_down_info;

  uint16                      ra_lifetime;
  uint8                       n_filters;

  ps_iface_changed_apn_param_info_type  changed_apn_param_info;

} iface_event_info_cache_type;

/*===========================================================================
TYPEDEF PS_IFACE_CAPABILITY_ENUM_TYPE

DESCRIPTION
  Lists various capabilities of an iface. If a particular bit is ON, then
  the iface will have corresponding capability

  NOTE : Enums are not bit masks. They represent bit position in a mask which
         represents a particular capability

    PS_IFACE_CAPABILITY_DEFAULT         - if this bit is ON, flow is a default
                                         flow

===========================================================================*/
typedef enum
{
  PS_IFACE_CAPABILITY_MIN              = 0x0,
  PS_IFACE_CAPABILITY_DEFAULT          = 0x0,
  PS_IFACE_CAPABILITY_A2_CAPABLE       = 0x1,
  PS_IFACE_CAPABILITY_IPSEC_FLTR       = 0x2,
  PS_IFACE_CAPABILITY_CLAT_CAPABLE     = 0x3,
  PS_IFACE_CAPABILITY_MAX,
  PS_IFACE_CAPABILITY_MAX_ALLOWED      = 0x8,
  PS_IFACE_CAPABILITY_FORCE_SIGNED_32_BIT = 0x7FFFFFFF
} ps_iface_capability_enum_type;

typedef uint8  ps_iface_capability_mask_type;

/*===========================================================================
  Mask to specify supported flags in iface.
===========================================================================*/
typedef enum
{
  PS_IFACE_SUPPORTED_FLAG_MIN = 0x00,
  PS_IFACE_SUPPORTED_FLAG_KEEP_ALIVE = 0x00,
  PS_IFACE_SUPPORTED_FLAG_MAX
} ps_iface_supported_flag_enum_type;

typedef uint32 ps_iface_supported_flags_mask_type;

/*===========================================================================
  An optimized filter specification for the DL optimized data path.
===========================================================================*/
#define PS_IFACE_MAX_OPT_FILTER_ENTRIES (50)

/*===========================================================================
  A global optimized filter specification for the DL optimized data path.
==========================================================================*/
#define MAX_GLOBAL_OPT_FILTER_ENTRIES (7)

typedef struct ps_dl_opt_filter_spec_s
{
  uint32   fi_handle;
  uint16   dst_port;
  uint16   dst_port_range;
  uint16   src_port;
  uint16   src_port_range;
  uint8    protocol;
} ps_dl_opt_filter_spec_type;

/*===========================================================================
  An optimized filter specification for the UL optimized data path.
===========================================================================*/
typedef struct ps_ul_opt_filter_spec_s
{
  uint32   fi_handle;
  uint16   dst_port;
  uint16   dst_port_range;
  uint16   src_port;
  uint16   src_port_range;
  uint8    protocol;
} ps_ul_opt_filter_spec_type;

/*===========================================================================
                           INTERFACE CONTROL BLOCK

TYPEDEF PS_IFACE_TYPE

DESCRIPTION
  The interface control block.
  Each interface has an instance of this data structure.
  Interface can be dynamically created and destroyed and can have
  multiple instances (PDP-IP-0, PDP-IP-1, ...)
  The output of each iface always goes to another iface. The IP stack is also
  initialized as an interface.
  ALL REGISTERED CMDs & INDICATIONs must be under TASKLOCKs & INTLOCKs.
===========================================================================*/
struct ps_iface_s
{
  /*-------------------------------------------------------------------------
    The private information that should only ever be changed through ps_iface
    functions.  The data in here should NOT changed directly by a client.

    NOTE: This MUST be the first element as the q link needs to be the first
          element in the structure.
  -------------------------------------------------------------------------*/
  struct
  {
    q_link_type    link;               /* link pointer - MUST BE FIRST     */
    q_type         event_q_array[MAX_EVENT_QUEUES]; /* event callback array */
    ps_ifacei_v6_addr_type *ipv6_addrs[MAX_IPV6_ADDRS];   /* v6 addr info  */
    uint64                prefix_cache;     /* Cached prefix */
    uint8                 prefix_cache_len; /* Cached prefix length  */
    /* Prefix cache, to be used only for Rmnet iface, 0 on all other ifaces */
    ps_timer_handle_type  ipv6_ext_addr_silent_timer; /*probing silent timer*/
    ps_iface_type *this_iface;         /* if NULL the block is invalid     */
    ps_iface_type *tx_bridge_ptr;      /* pointer to iface we are bridging */
    ps_iface_type *assoc_iface_ptr;    /* ptr associated interface         */
    ps_iface_type *trat_iface_ptr;      /* ptr to TRAT                     */
    ps_iface_tx_cmd_f_ptr_type tx_cmd; /* set via accessor function        */
    ps_iface_data_proc_f_ptr_type  rx_proc_f_ptr; /* rx_proc for log ifaces*/
    void          *tx_cmd_info;        /* information to use with tx_cmd   */
    ps_iface_ip_input_f_ptr_type ip_input;/* input function pointer for IP */
    uint32         tx_flow_mask;       /* if !=0, iface is flow_ctl        */
    ps_timer_handle_type iface_linger_timer;         /* IFACE linger timer */
    uint32         linger_timeout_val;       /* IFACE linger timeout value */
    uint32         profile_id_3gpp;       /* 3gpp profile used to bring up */
                                                           /*    data call */
    uint32         profile_id_3gpp2;     /* 3gpp2 profile used to bring up */
                                                           /*    data call */

    ps_iface_addr_family_type  addr_family;           /* iface addr family */
    uint32         ipv4_addr;          /* this interfaces IP address       */
    /* this interface's hardware address */
    uint8          hw_addr[PS_IFACE_MAX_HW_ADDR_LEN];
    uint8          hw_addr_len;        /* hw_addr length (< ARP_HLN_MAX)   */
    boolean        inherit_ip_info;    /* FALSE (default) - local IP info  */
                                       /* stored locally.  TRUE - see      */
                                       /* associated PS_IFACE              */

    ps_iface_cookie_type  cookie;      /* cookie identifying an instance   */
    ps_iface_state_enum_type state;    /* interface state                  */


    /*-----------------------------------------------------------------------
      The Tx IP filter information that is associated with this iface
    -----------------------------------------------------------------------*/
    q_type  ipfltr_info[IP_FLTR_CLIENT_MAX];

    /*-----------------------------------------------------------------------
      The powersave rx filtering info that is associated with this iface. 
      Powersave mode indicates whether the filters needs to be executed or not.
      When powersave mode is enabled and there is filter rule match found for
      an input packet, we will exit powersave mode if auto exit mode is 
      enabled.
      -----------------------------------------------------------------------*/
    struct 
    {
      /* FALSE(disable)/TRUE(enable) powersave filtering mode */
      boolean enable_filter_mode;      
      /* FALSE(disable)/TRUE(enable) powersave filtering auto exitmode */
      boolean enable_auto_exit_mode;   
    } powersave_filtering_settings;

    /*-----------------------------------------------------------------------
      The Rx IP filter information that is associated with this iface
    -----------------------------------------------------------------------*/
    list_type  rx_qos_fltr_node_list;    /* List of pointers to Rx filters */

    uint32         priority_mask;      /* Priority mask of this iface      */

    /*-----------------------------------------------------------------------
      IFACE Ref Count. How many clients of a given priority are actively
      using the IFACE?
    -----------------------------------------------------------------------*/
    uint8          ref_cnt[PS_POLICY_MGR_IFACE_PRIORITY_MAX];

    /*-----------------------------------------------------------------------
      This flag defines the call termination policy for an interface.  If set
      to TRUE the interface should be closed gracefully, even if this means
      re-establishing a dormant physical link.  The default value of this
      flag is set to FALSE.
    -----------------------------------------------------------------------*/
    boolean   graceful_dormant_close;

    /*-----------------------------------------------------------------------
      This flag indicates whether an interface supports broadcast packets. If
      set to TRUE, the interface supports broadcast.
      Default value should be FALSE
    -----------------------------------------------------------------------*/
    boolean   is_bcast_iface;

    /*-----------------------------------------------------------------------
      This flag indicates whether an interface isa proxy for a remoted Um
      iface. If set to TRUE, the interface is a proxy.
      Default value should be FALSE
    -----------------------------------------------------------------------*/
    boolean   is_proxy_iface;

    /*-----------------------------------------------------------------------
      This flag indicates that the interface is kept active even if there is
      no application using it. E.g., the interface is brought up along with
      the default attach procedure by the iface controller.
    ------------------------------------------------------------------------*/
    boolean   is_active;

    /*-----------------------------------------------------------------------
      This flag indicates whether an interface is "always on" when the media
      is present, i.e. doesn't require a bring up command to initiate IP
      connectivity.  e.g. ethernet, 802.11.
    -----------------------------------------------------------------------*/
    boolean   is_always_on;

    /*-----------------------------------------------------------------------
      This flag indicates whether an interface has firewall enabled
    -----------------------------------------------------------------------*/
    boolean   is_firewall_enabled;

    /*-----------------------------------------------------------------------
      This flag indicates whether packet(s) is allowed/disallowed
      through firewall.
        TRUE  - Packets matching firewall rules will be allowed
        FALSE - Packets matching firewall rules will be disallowed
    -----------------------------------------------------------------------*/
    boolean   is_pkt_allowed;

    /*-----------------------------------------------------------------------
      The rmnet instance on which the iface was brought up
    -----------------------------------------------------------------------*/
    int8      rm_inst_handle;

    struct
    {
      void  * iface_up_ev_cback_buf_ptr;
      void  * iface_down_ev_cback_buf_ptr;
      void  * iface_ev_cback_buf_ptr;
    } logical_iface_info;

    /*-----------------------------------------------------------------------
      The flow information pertaining to this iface
    -----------------------------------------------------------------------*/
    struct
    {
      ps_flow_type  * default_flow_ptr;       /* Ptr to default flow       */
      list_type       sec_flow_list;          /* List of secondary flows   */
      uint8           num_sec_flows;          /* Number of secondary flows */
      uint8           next_cookie; /* Cookie assigned to the next new flow */
    } flow;

    list_type       net_initiated_qos_req_list;
    /**< List of Network Initiated QoS requests. Each request contains
         QoS filter specification */

    /*-----------------------------------------------------------------------
      The phys_link information. Populated only for static phys link binding
    -----------------------------------------------------------------------*/
    struct
    {
      ps_phys_link_type *array;
      uint8              instances;
      uint8              primary;
    } phys_link;

    boolean is_logical; /* TRUE: logical iface; FALSE: physical iface      */
    boolean in_use;     /* iface has been acquired for bring up */
    boolean looked_up;  /* iface acquired for something other than bring up*/
    boolean is_shareable; /* if set to TRUE, overrides all shareable checks
                           * for example for PROC_ID/routeable.
                           * Default value is FALSE.
                           */

    uint32  iface_index;/* Index of this iface in global_iface_array */
    acl_proc_id_type       proc_id;

    ps_timer_handle_type querier_v1_timer; /* captures whether the querier
                                              running on the interface is
                                              running IGMPv1 */
    struct ps_mcast_cb  *if_mcast_grps; /* list of multicast groups that this
                                           interface is interested in */

    /*-----------------------------------------------------------------------
      DHCP stateless client handle
    -----------------------------------------------------------------------*/
    void * dhcp_stateless_client_handle;

    /*-----------------------------------------------------------------------
      DHCP stateful client handle
    -----------------------------------------------------------------------*/
    void * dhcp_stateful_client_handle;

    /*-----------------------------------------------------------------------
      DHCP6 server handle
    -----------------------------------------------------------------------*/
    void * dhcp6_server_handle;

    /*-----------------------------------------------------------------------
      Reason for ps_iface_bring_up_cmd_f_ptr failure
    -----------------------------------------------------------------------*/
    int16  ps_errno_for_bring_up_cmd_f_ptr_failure;

    /*-----------------------------------------------------------------------
      mask identifying various capabilities of this iface
    -----------------------------------------------------------------------*/
    ps_iface_capability_mask_type   capability_mask;

    /*---------------------------------------------------------------------------
      Q of cache entries.
    ---------------------------------------------------------------------------*/
    q_type dns_cache_q;

    /*-----------------------------------------------------------------------
      Arbitration info blob
    -----------------------------------------------------------------------*/
    struct
    {
      boolean in_progress;
    } arbitration_info;

    /*----------------------------------------------------------------------- 
      Optimized DL path config
    -----------------------------------------------------------------------*/
    struct
    {
      ps_iface_type              * rm_iface_ptr;
      ps_dl_opt_filter_spec_type * fltr_arr;
    } dl_opt_cfg;

    /*----------------------------------------------------------------------- 
     if local abort is set, then modehandler will not bring down
     the call from network, it will silently clear up the call on modem 
     side
    -----------------------------------------------------------------------*/   
    struct
    {
      boolean                        local_abort;
    } pdn_teardown_config;

    /*----------------------------------------------------------------------- 
                 DHCPv6 Prefix delegation config
    -----------------------------------------------------------------------*/
    ps_iface_ip6_prefix_mode_type ipv6_prefix_mode;  /* Prefix mode of iface */
    ps_ifacei_prefix_mgmt_type    ipv6_prefix_mgmt;  /* Prefix Management module */

    uint32   subs_id;           /* Subscription id */
    ps_iface_apn_enum_type  apn_type; /* APN type */
    boolean  is_unbridged;            /* Software path enabled*/
    uint8    unbridge_ref_count;     /*number of unbridges by all clients*/

    /*-----------------------------------------------------------------------
      Bit mask identifying various supported proprties of this iface
    -----------------------------------------------------------------------*/
    ps_iface_supported_flags_mask_type   properties_mask;
  } iface_private;

  /*-----------------------------------------------------------------------
    qos_fltr_offloaded_to_hw set to true if qos filtering is offloaded 
    to IPA hardware
  -----------------------------------------------------------------------*/    
  boolean                         qos_fltr_offloaded_to_hw;

  /*-----------------------------------------------------------------------
    This enum type indicates type of handoff in progress
  -----------------------------------------------------------------------*/
  ps_iface_handoff_class_enum_type  handoff_class;
  
  /*-------------------------------------------------------------------------
    Fields that identify the interface. Can be changed by clients.
  -------------------------------------------------------------------------*/
  uint16                        group_mask;
  ps_iface_name_enum_type       name;
  uint8                         instance;

  /*-------------------------------------------------------------------------
    DPL control block for ps_iface
      - tx_cb   : control block for Tx side
      - recv_cb : control block for Rx side
      - tx_dpl_id   : DPL IID of this iface. Protocol field must always be 0
      - recv_dpl_id : DPL IID of this iface. Protocol field must always be 0
      - desc    : unique description of this iface
  -------------------------------------------------------------------------*/
  struct
  {
    dpl_net_dir_cb_type  tx_cb;
    dpl_net_dir_cb_type  recv_cb;
    dpl_iid_type         tx_dpl_id;
    dpl_iid_type         recv_dpl_id;
    char                 desc[DPL_IFNAME_DESC_S_LEN];
  } dpl_net_cb;

  /*-------------------------------------------------------------------------
    Master copy of DPL's flow logging settings. Since secondary ps_flows
    are dynamically created, a copy is maintained in ps_iface and when
    ps_flow is created, these settings are populated in to it
      - snaplen                    : partial logging length for network
                                     protocols
      - mask                       : A set bit implies that logging is enabled
                                     for corresponding protocol
                                     (see netprot enum)
      - is_logging_enabled         : Indicates if logging is enabled or not
  -------------------------------------------------------------------------*/
  struct
  {
    uint32        mask;
    uint32        snaplen[DPL_IID_NETPROT_MAX];
    boolean       is_logging_enabled;
  } dpl_sec_flow_copy_cb;

  /*-------------------------------------------------------------------------
    PS Iface authentication information

    This is populated via ps_iface_auth_set_params()
    This is cleared when the ps_iface goes DOWN.
  -------------------------------------------------------------------------*/
  network_params_info_type  net_cfg_params;

  /*-------------------------------------------------------------------------
    void pointer which the client can use to store whatever information they
    desire.
  -------------------------------------------------------------------------*/
  void                   *client_data_ptr;

  /*-------------------------------------------------------------------------
    IP info fields - struct defined in ps_ip_addr.h
  -------------------------------------------------------------------------*/
  network_info_type       net_info;
  #define v4_net_info net_info.net_ip.v4
  #define v6_net_info net_info.net_ip.v6

  list_type        sip_serv_addr_list;
  list_type        sip_serv_domain_name_list;

  list_type        domain_name_search_list;

  ps_iface_op_reserved_pco_type *operator_reserved_pco_ptr;

  ps_iface_modem_reserved_pco_type * modem_reserved_pco_ptr;

  ps_iface_mip6_bootstrap_cfg_info_type *mip6_bootstrap_info;

  /*-------------------------------------------------------------------------
    Statistics for ps_iface
  -------------------------------------------------------------------------*/
  struct iface_stats
  {
    uint64  bytes_rx;
    uint64  bytes_tx;
    uint32  pkts_rx;
    uint32  mcast_pkts_rx;
    uint32  pkts_dropped_rx;
    uint32  pkts_tx;
    uint32  mcast_pkts_tx;
    uint32  pkts_dropped_tx;
    uint32  queried;
  } iface_i_stats;

  /*-------------------------------------------------------------------------
    Reset packet stats request is not propagated to Hardware. Hence the need to 
    cache Hardware pkt stats. When client queries for pkt stats, we return the 
    difference of what hardware returns and cached value.
  -------------------------------------------------------------------------*/  
  ps_iface_stat_adjustment_type  stat_adjustment_cache;
  
  /*-------------------------------------------------------------------------
    Last IO timestamp - used in Network Linger calculations
  -------------------------------------------------------------------------*/
  uint32        last_io_time;

  /*-------------------------------------------------------------------------
    Commands - called from PS->Interface client
  -------------------------------------------------------------------------*/
  ps_iface_cmd_f_ptr_type bring_up_cmd_f_ptr;
  ps_iface_cmd_f_ptr_type tear_down_cmd_f_ptr;
  ps_iface_cmd_f_ptr_type linger_cmd_f_ptr;
  ps_iface_cmd_f_ptr_type in_use_cmd_f_ptr;  /* 1st app of the active iface */
  ps_iface_cmd_f_ptr_type out_use_cmd_f_ptr; /* last app of the active iface*/
  ps_iface_cmd_f_ptr_ex_type bring_up_cmd_f_ptr_ex;  /* ext ver of bring up cmd fptr */
  ps_iface_cmd_f_ptr_ex_type tear_down_cmd_f_ptr_ex; /* ext ver of tear down cmd fptr */

  ps_iface_handle_event_f_ptr_type handle_event_f_ptr; /* event handling fptr */

  /*-------------------------------------------------------------------------
    bridge command handler
  -------------------------------------------------------------------------*/
  ps_iface_bridge_proc_f_ptr_type bridge_proc_f_ptr;

  ps_iface_bridge_target_proc_f_ptr_type bridge_target_proc_f_ptr;

  /*-------------------------------------------------------------------------
    iface ioctl function pointer.
  -------------------------------------------------------------------------*/
  ps_iface_ioctl_f_ptr_type      iface_ioctl_f_ptr;

  /*-------------------------------------------------------------------------
    Function  called when phys link is doing a dormancy related action.
    If not set, then IFACE will always indicate that it is okay to
    proceed with the specified action.
  -------------------------------------------------------------------------*/
  ps_iface_dorm_action_f_ptr_type    dorm_action_f_ptr;

  /*-------------------------------------------------------------------------
    Function called by stats module to fill the description associated with
    this iface.  Can be set by mode handlers to allow for different iface
    types to have different descriptions
  -------------------------------------------------------------------------*/
  ps_iface_stat_desc_f_ptr_type      ps_iface_stat_fill_desc_f_ptr;
  ps_iface_stat_desc_len_f_ptr_type  ps_iface_stat_get_desc_len_f_ptr;

  /*-------------------------------------------------------------------------
    Function called to get stats adjustment factor from client
  -------------------------------------------------------------------------*/
  ps_iface_stat_adjustment_f_ptr_type ps_iface_stat_get_adjustment_f_ptr;

  /*-------------------------------------------------------------------------
    Function called by the address management layer to indicate to mode
    handlers of address changes
  -------------------------------------------------------------------------*/
  ps_iface_addr_mgmt_cb_f_ptr_type  addr_cb_f_ptr;

  /*-------------------------------------------------------------------------
    Interface specific DAD function called by the address management layer to
    verify IPv6 addresses.
  -------------------------------------------------------------------------*/
  ps_iface_addr_mgmt_dad_f_ptr_type dad_f_ptr;

  /*-------------------------------------------------------------------------
    Interface specific outgoing ACL function ptr
  -------------------------------------------------------------------------*/
  acl_type                     *outgoing_acl_ptr;

  iface_event_info_cache_type   event_info_cache;
  
  /*-------------------------------------------------------------------------
    If set, mbim filters associated with iface would be in restrictive mode
    powersave_restrictive == TRUE : 
      Whitelist i.e allow the packet to be sent further  if filter match.
    powersave_restrictive == FALSE : 
      Blacklist i.e drop the packet if filter match
  -------------------------------------------------------------------------*/  
  boolean                       powersave_restrictive;
  
  /*-------------------------------------------------------------------------
    CLAT PDN context
  -------------------------------------------------------------------------*/
  void                        * clat_pdn_cntxt;
}; /* ps_iface_s */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      PUBLIC MACRO/FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
MACRO IPV4_ADDR_MSG()

DESCRIPTION
  This macro prints an IPV4 address to F3.

PARAMETERS
  ip_addr: The IPV4 address in host byte order.

RETURN VALUE
  none
===========================================================================*/
#define IPV4_ADDR_MSG(ip_addr) MSG_4(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV4 Address is %d.%d.%d.%d", \
                        (unsigned char)(ip_addr), \
                        (unsigned char)(ip_addr >> 8), \
                        (unsigned char)(ip_addr >> 16) , \
                        (unsigned char)(ip_addr >> 24))

/*===========================================================================
FUNCTION PS_IFACE_IS_VALID()

DESCRIPTION
  This macro returns a boolean indicating whether the iface pointer passed in
  is valid.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  TRUE: if iface_ptr is valid
  FALSE: otherwise.
===========================================================================*/
#define PS_IFACE_IS_VALID(iface_ptr)                        \
  (((iface_ptr) != NULL) &&                                 \
   ((iface_ptr)->iface_private.this_iface == (iface_ptr)))

/*===========================================================================
FUNCTION PS_IFACE_IS_LOGICAL()

DESCRIPTION
  This macro returns a boolean indicating whether the iface pointer passed in
  is a logical iface.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  TRUE: if iface_ptr is a logical one
  FALSE: otherwise.
===========================================================================*/
#define PS_IFACE_IS_LOGICAL(iface_ptr)                                   \
  (PS_IFACE_IS_VALID(iface_ptr) ? PS_IFACEI_IS_LOGICAL(iface_ptr)        \
                                : FALSE)

/*===========================================================================
FUNCTION PS_IFACE_RESET_LOGICAL()

DESCRIPTION
  This function reset is_logical flag of an iface to FALSE for intending to use
  the iface as a physical iface.

  Note: When an iface is created, it is set to be logical or physical by
  default depending on the phys link ptr parameter being NULL or not.

  In the case that the iface is to support dynamic phys link binding, the iface
  is created with phys link pointer passed as NULL and the user needs rest the
  is_logical flag for using the iface as physical.

  The user should also make sure correct tx_cmd for physical iface is used.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  None
===========================================================================*/
INLINE void ps_iface_reset_logical
(
  ps_iface_type *iface_ptr
)
{
  if (PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.is_logical = FALSE;
  }
} /* ps_iface_reset_logical() */

/*===========================================================================
FUNCTION PS_IFACE_SET_ASSOC_IFACE()

DESCRIPTION
  This macro sets the associated iface for a logical iface.

PARAMETERS
  this_iface: pointer to the interface in question.
  assoc_iface: Iface pointer to associate with.

RETURN VALUE
  None.
===========================================================================*/
#define PS_IFACE_SET_ASSOC_IFACE(this_iface, assoc_iface)                \
  ((this_iface)->iface_private.assoc_iface_ptr = (assoc_iface))

/*===========================================================================
FUNCTION PS_IFACE_GET_ASSOC_IFACE()

DESCRIPTION
  This macro returns a ps_iface_ptr pointing to the associated iface of the
  logical iface that is specified.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  Associated ps_iface_ptr if passed in iface_ptr is a logical iface.
  Null: otherwise.
===========================================================================*/
#define PS_IFACE_GET_ASSOC_IFACE ps_iface_get_assoc_iface
INLINE ps_iface_type *ps_iface_get_assoc_iface
(
  ps_iface_type *iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_IFACE_IS_LOGICAL(iface_ptr))
  {
    if (NULL != iface_ptr->iface_private.assoc_iface_ptr)
    {
      return iface_ptr->iface_private.assoc_iface_ptr;
    }
    else
    {
      return iface_ptr->iface_private.trat_iface_ptr;
    }
  }
  return NULL;
} /* ps_iface_get_assoc_iface() */

#define PS_IFACE_SET_TRAT_IFACE( this_iface, trat_iface)                 \
  ((this_iface)->iface_private.trat_iface_ptr = (trat_iface))

#define PS_IFACE_GET_TRAT_IFACE( ps_iface_ptr)                           \
  (PS_IFACE_IS_LOGICAL( ps_iface_ptr)                                    \
     ? (ps_iface_ptr)->iface_private.trat_iface_ptr                      \
     : NULL)

/*===========================================================================
FUNCTION PS_IFACE_GET_PHYS_LINK()

DESCRIPTION
  This macro will return the primary phys link associated with the specified
  iface.

PARAMETERS
  iface_ptr: pointer to the interface.

RETURN VALUE
  pointer to primary phys link.
===========================================================================*/
#define PS_IFACE_GET_PHYS_LINK ps_iface_get_phys_link
INLINE ps_phys_link_type *ps_iface_get_phys_link
(
  ps_iface_type *iface_ptr
)
{
  if (PS_IFACE_IS_VALID(iface_ptr))
  {
    return PS_IFACEI_GET_PHYS_LINK(iface_ptr);
  }
  return NULL;

} /* ps_iface_get_phys_link() */

/*===========================================================================
MACRO PS_IFACE_GET_PHYS_LINK_BY_INST()

DESCRIPTION
  This macro will return the phys link associated with the specified
  iface for the particular instance passed.

PARAMETERS
  iface_ptr: pointer to the interface.
  instance : phys_link instance

RETURN VALUE
  pointer to phys link.
===========================================================================*/
#define PS_IFACE_GET_PHYS_LINK_BY_INST(iface_ptr, instance)          \
  (PS_IFACE_IS_VALID(iface_ptr)                                      \
    ? PS_IFACEI_GET_PHYS_LINK_BY_INST(iface_ptr, instance)           \
    : NULL)

/*===========================================================================
MACRO PS_IFACE_GET_NUM_PHYS_LINKS()

DESCRIPTION
  This macro returns the number of phys_links associated with this iface.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  Success : Number of phys links
  Failure : 0

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_NUM_PHYS_LINKS( iface_ptr )                     \
  (PS_IFACE_IS_VALID(iface_ptr)                                      \
    ? (iface_ptr)->iface_private.phys_link.instances                       \
    : 0)

/*===========================================================================
MACRO PS_IFACE_IS_PHYS_LINK_VALID()

DESCRIPTION
  Validates whether the specified phys link belongs to the specified iface.

PARAMETERS
  iface_ptr     :  Pointer to interface
  phys_link_ptr :  Pointer to the phys link to validate

RETURN VALUE
  TRUE  : If phys link belongs to ps_iface
  FALSE : If iface or phys link is invalid or if iface is logical or if
          phys link doesn't belong to iface
===========================================================================*/
#define PS_IFACE_IS_PHYS_LINK_VALID(iface_ptr, phys_link_ptr)            \
  ((PS_IFACE_IS_VALID(iface_ptr) && !PS_IFACEI_IS_LOGICAL(iface_ptr) &&  \
    PS_PHYS_LINK_IS_VALID(phys_link_ptr))                                \
      ? PS_IFACEI_IS_PHYS_LINK_VALID(iface_ptr, phys_link_ptr)           \
      : FALSE)


/*===========================================================================
FUNCTION PS_IFACE_GET_GRACEFUL_CLOSE()

DESCRIPTION
  This macro will return the value of the graceful_dormant_close variable

PARAMETERS
  iface_ptr: pointer to the interface.

RETURN VALUE
  Value of graceful_dormant_close variable.
===========================================================================*/
#define PS_IFACE_GET_GRACEFUL_CLOSE ps_iface_get_graceful_close
INLINE boolean ps_iface_get_graceful_close
(
  ps_iface_type * iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    return (iface_ptr)->iface_private.graceful_dormant_close;
  }

  return FALSE;

} /* ps_iface_get_graceful_close() */


/*===========================================================================
FUNCTION PS_IFACE_SET_GRACEFUL_CLOSE()

DESCRIPTION
  This macro will set the value of the graceful_dormant_close variable

PARAMETERS
  iface_ptr: pointer to the interface.
  boolean:   value of graceful_dormant_close

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_GRACEFUL_CLOSE ps_iface_set_graceful_close
INLINE void ps_iface_set_graceful_close
(
  ps_iface_type *iface_ptr,
  boolean graceful_close
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.graceful_dormant_close = graceful_close;
  }

} /*  ps_iface_set_graceful_close() */


/*===========================================================================
FUNCTION PS_IFACE_GET_IS_BCAST_IFACE()

DESCRIPTION
  This macro will return the value of the is_bcast_iface variable

PARAMETERS
  iface_ptr: pointer to the interface.

RETURN VALUE
  Value of the is_bcast_iface variable
===========================================================================*/
#define PS_IFACE_GET_IS_BCAST_IFACE ps_iface_get_is_bcast_iface
INLINE boolean ps_iface_get_is_bcast_iface
(
  ps_iface_type * iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    return (iface_ptr)->iface_private.is_bcast_iface;
  }

  return FALSE;

} /* ps_iface_get_is_bcast_iface() */


/*===========================================================================
FUNCTION PS_IFACE_SET_IS_BCAST_IFACE()

DESCRIPTION
  This macro will set the value of the is_bcast_iface variable

PARAMETERS
  iface_ptr: pointer to the interface.
  boolean:   value of is_bcast_iface

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_IS_BCAST_IFACE ps_iface_set_is_bcast_iface
INLINE void ps_iface_set_is_bcast_iface
(
  ps_iface_type *iface_ptr,
  boolean is_bcast_iface
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.is_bcast_iface = is_bcast_iface;
  }

} /*  ps_iface_set_is_bcast_iface() */


/*===========================================================================
FUNCTION PS_IFACE_GET_IS_PROXY_IFACE()

DESCRIPTION
  This macro will return the value of the is_proxy_iface variable

PARAMETERS
  iface_ptr: pointer to the interface.

RETURN VALUE
  Value of the is_proxy_iface variable
===========================================================================*/
#define PS_IFACE_GET_IS_PROXY_IFACE ps_iface_get_is_proxy_iface
INLINE boolean ps_iface_get_is_proxy_iface
(
  ps_iface_type * iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    return (iface_ptr)->iface_private.is_proxy_iface;
  }

  return FALSE;

} /* ps_iface_get_is_proxy_iface() */


/*===========================================================================
FUNCTION PS_IFACE_SET_IS_PROXY_IFACE()

DESCRIPTION
  This macro will set the value of the is_proxy_iface variable

PARAMETERS
  iface_ptr: pointer to the interface.
  boolean:   value of is_proxy_iface

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_IS_PROXY_IFACE ps_iface_set_is_proxy_iface
INLINE void ps_iface_set_is_proxy_iface
(
  ps_iface_type *iface_ptr,
  boolean is_proxy_iface
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.is_proxy_iface = is_proxy_iface;
  }

} /*  ps_iface_set_is_proxy_iface() */

/*===========================================================================
FUNCTION PS_IFACE_GET_IS_ACTIVE()

DESCRIPTION
  This macro will get the value of the is_active variable

PARAMETERS
  iface_ptr: pointer to the interface.

RETURN VALUE
  Value of the is_active variable
===========================================================================*/
#define PS_IFACE_GET_IS_ACTIVE ps_iface_get_is_active
INLINE boolean ps_iface_get_is_active
(
  ps_iface_type * iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    return (iface_ptr)->iface_private.is_active;
  }

  return FALSE;
} /* ps_iface_get_is_active() */

/*===========================================================================
FUNCTION PS_IFACE_SET_INHERIT_IP_INFO()

DESCRIPTION
  This macro sets inherit_ip_info for the iface.

PARAMETERS
  iface_ptr: Target iface ptr
  inherit_ip_info: Inherit IP info flag

NOTE: only one capability can be set each time

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_INHERIT_IP_INFO ps_iface_set_inherit_ip_info
INLINE void ps_iface_set_inherit_ip_info
(
  ps_iface_type * iface_ptr,
  boolean         inherit_ip_info
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
   (iface_ptr)->iface_private.inherit_ip_info = inherit_ip_info;
  }
} /* ps_iface_set_inherit_ip_info */

/*===========================================================================
FUNCTION PS_IFACE_SET_IS_ACTIVE()

DESCRIPTION
  This macro will set the value of the is_active variable

PARAMETERS
  iface_ptr: pointer to the interface.
  boolean:   value of is_active

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_IS_ACTIVE ps_iface_set_is_active
void ps_iface_set_is_active
(
  ps_iface_type *iface_ptr,
  boolean        is_active
);

/*===========================================================================
FUNCTION PS_IFACE_GET_IS_ALWAYS_ON()

DESCRIPTION
  This macro will get the value of the is_always_on variable

PARAMETERS
  attr:      pointer to the interface.
  iface_ptr: pointer to the interface.

RETURN VALUE
  Value of the is_bcast_iface variable
===========================================================================*/
#define PS_IFACE_GET_IS_ALWAYS_ON ps_iface_get_is_always_on
INLINE boolean ps_iface_get_is_always_on
(
  ps_iface_type * iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    return (iface_ptr)->iface_private.is_always_on;
  }

  return FALSE;

} /* ps_iface_get_is_always_on() */


/*===========================================================================
FUNCTION PS_IFACE_SET_IS_ALWAYS_ON()

DESCRIPTION
  This macro will set the value of the is_always_on variable

PARAMETERS
  iface_ptr: pointer to the interface.
  boolean:   value of is_bcast_iface

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_IS_ALWAYS_ON ps_iface_set_is_always_on
INLINE void ps_iface_set_is_always_on
(
  ps_iface_type *iface_ptr,
  boolean is_always_on
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.is_always_on = is_always_on;
  }

} /*  ps_iface_set_is_always_on() */

/*===========================================================================
FUNCTION PS_IFACE_GET_IS_FIREWALL_ENABLED()

DESCRIPTION
  This macro will return the value of the is_firewall_enabled variable

PARAMETERS
  iface_ptr: pointer to the interface.

RETURN VALUE
  Value of the is_firewall_enabled variable
===========================================================================*/
#define PS_IFACE_GET_IS_FIREWALL_ENABLED ps_iface_get_is_firewall_enabled
INLINE boolean ps_iface_get_is_firewall_enabled
(
  ps_iface_type * iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    return (iface_ptr)->iface_private.is_firewall_enabled;
  }

  return FALSE;

} /* ps_iface_get_is_firewall_enabled() */

/*===========================================================================
FUNCTION PS_IFACE_SET_IS_FIREWALL_ENABLED()

DESCRIPTION
  This macro will set the value of the is_firewall_enabled variable

PARAMETERS
  iface_ptr: pointer to the interface.
  boolean:   value of is_firewall_enabled

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_IS_FIREWALL_ENABLED ps_iface_set_is_firewall_enabled
INLINE void ps_iface_set_is_firewall_enabled
(
  ps_iface_type *iface_ptr,
  boolean is_firewall_enabled
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.is_firewall_enabled = is_firewall_enabled;
  }

} /*  ps_iface_set_is_firewall_enabled() */

/*===========================================================================
FUNCTION PS_IFACE_GET_IS_PKT_ALLOWED_THRU_FIREWALL()

DESCRIPTION
  This macro will return the value of the is_pkt_allowed through
  firewall variable

PARAMETERS
  iface_ptr: pointer to the interface.

RETURN VALUE
  Value of the is_pkt_allowed variable
===========================================================================*/
#define PS_IFACE_GET_IS_PKT_ALLOWED_THRU_FIREWALL                           \
ps_iface_get_is_pkt_allowed_thru_firewall
INLINE boolean ps_iface_get_is_pkt_allowed_thru_firewall
(
  ps_iface_type * iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    return (iface_ptr)->iface_private.is_pkt_allowed;
  }

  return FALSE;

} /* ps_iface_get_is_pkt_allowed_thru_firewall() */

/*===========================================================================
FUNCTION PS_IFACE_SET_IS_PKT_ALLOWED_THRU_FIREWALL()

DESCRIPTION
  This macro will set the value of the variable is_pkt_allowed
  through firewall

PARAMETERS
  iface_ptr: pointer to the interface.
  boolean:   value of is_pkt_allowed

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_IS_PKT_ALLOWED_THRU_FIREWALL                           \
ps_iface_set_is_pkt_allowed_thru_firewall
INLINE void ps_iface_set_is_pkt_allowed_thru_firewall
(
  ps_iface_type *iface_ptr,
  boolean is_pkt_allowed
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.is_pkt_allowed = is_pkt_allowed;
  }

} /*  ps_iface_set_is_pkt_allowed_thru_firewall() */

/*===========================================================================
FUNCTION PS_IFACE_STATE()

DESCRIPTION
  This macro returns the state of the interface that was passed in.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  the state of the passed in interface.
===========================================================================*/
INLINE ps_iface_state_enum_type ps_iface_state
(
  ps_iface_type *iface_ptr
)
{
  if( PS_IFACE_IS_VALID(iface_ptr) )
  {
    return ((iface_ptr)->iface_private.state);
  }

  return IFACE_STATE_INVALID;

} /*  ps_iface_state() */


/*===========================================================================
FUNCITON PS_IFACE_PHYS_STATE()

DESCRIPTION
  This macro returns the state of the interface that was passed in.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  the state of the passed in interface.
===========================================================================*/
INLINE phys_link_state_type ps_iface_phys_link_state
(
  ps_iface_type *iface_ptr
)
{
  ps_phys_link_type *phys_link_ptr = NULL;
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(iface_ptr);
    if(phys_link_ptr != NULL)
    {
      return PS_PHYS_LINK_GET_STATE(phys_link_ptr);
    }
  }
  return PHYS_LINK_DOWN;
} /*  ps_iface_phys_link_state() */


/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_IS_GONE

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
INLINE boolean ps_iface_phys_link_is_gone
(
  ps_iface_type *this_iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ps_iface_phys_link_state(this_iface_ptr) == PHYS_LINK_NULL)
  {
    return TRUE;
  }

  return FALSE;
} /* ps_iface_phys_link_is_gone() */


/*===========================================================================
FUNCTION PS_IFACE_GET_COOKIE()

DESCRIPTION
  This macro returns the interface cookie, which is unique per interface.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  the cookie for the passed in interface
===========================================================================*/
INLINE ps_iface_cookie_type ps_iface_get_cookie
(
  ps_iface_type *iface_ptr
)
{
  if( PS_IFACE_IS_VALID(iface_ptr) )
  {
    return ((iface_ptr)->iface_private.cookie);
  }
  return 0;

} /*  ps_iface_get_cookie() */

/*===========================================================================
FUNCTION PS_IFACE_IP_V4_ADDR()

DESCRIPTION
  This macro returns the IPv4 address assigned to the interface.  Calls a
  function, as this becomes more complex with logical ifaces.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  IP address, 0 if interface pointer is NULL
===========================================================================*/
#define PS_IFACE_IP_V4_ADDR( iface_ptr )                              \
ps_iface_get_v4_addr(iface_ptr)

/*===========================================================================
MACRO PS_IFACE_HW_ADDR_PTR()

DESCRIPTION
  This macro returns the pointer to the ps_iface's private hardware address

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  HW address, 0 if interface pointer is NULL
===========================================================================*/
#define PS_IFACE_HW_ADDR_PTR( iface_ptr )                              \
  (PS_IFACE_IS_VALID(iface_ptr)                                        \
    ? ((iface_ptr)->iface_private.hw_addr)                                   \
    : 0x00000000 )

/*===========================================================================
MACRO PS_IFACE_HW_ADDR_LEN()

DESCRIPTION
  This macro returns the specified interface's hardware address length

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  Length of current HW address, 0 if interface pointer is NULL
===========================================================================*/
#define PS_IFACE_HW_ADDR_LEN( iface_ptr )                              \
  (PS_IFACE_IS_VALID(iface_ptr)                                        \
    ? ((iface_ptr)->iface_private.hw_addr_len)                               \
    : 0x00)

/*===========================================================================
MACRO PS_IFACE_SET_HW_ADDR_LEN()

DESCRIPTION
  This macro sets the specified interface's hardware address length

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACE_SET_HW_ADDR_LEN( iface_ptr, len )                     \
  if (PS_IFACE_IS_VALID(iface_ptr))                                    \
  {                                                                    \
    (iface_ptr)->iface_private.hw_addr_len = len;                      \
  }

/*===========================================================================
MACRO PS_IFACE_HW_ADDR_LEN_MAX()

DESCRIPTION
  This macro returns the maximum length of the specified interface's
  hardware address

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  Maximum length of HW address, 0 if interface pointer is NULL
===========================================================================*/
#define PS_IFACE_HW_ADDR_LEN_MAX( iface_ptr )                              \
  (PS_IFACE_IS_VALID(iface_ptr)                                            \
    ? (sizeof((iface_ptr)->iface_private.hw_addr))                         \
    : 0x00)

/*===========================================================================
FUNCTION PS_IFACE_IS_PHYSICAL_IFACE()

DESCRIPTION
  This macro returns if given iface is physical iface or not

PARAMETERS
  iface_ptr: pointer to the interface this is bridged to.

RETURN VALUE
  TRUE : Iface is physical iface
  FALSE : iface is not physical iface
===========================================================================*/
#define ps_iface_is_physical_iface PS_IFACE_IS_PHYSICAL_IFACE
INLINE boolean ps_iface_is_physical_iface
(
  ps_iface_type *iface_ptr
)
{
  return (CDMA_SN_IFACE == iface_ptr->name ||
          UMTS_IFACE == iface_ptr->name ||
          WLAN_IFACE == iface_ptr->name ||
          IWLAN_S2B_IFACE == iface_ptr->name ||
          IWLAN_3GPP_IFACE == iface_ptr->name ||
          IWLAN_3GPP2_IFACE == iface_ptr->name ||
          EMBMS_IFACE == iface_ptr->name);
}
    
/*===========================================================================
FUNCTION PS_IFACE_BRIDGE_IFACE()

DESCRIPTION
  This macro will call the transmit of the interface that is passed in.

PARAMETERS
  iface_ptr: pointer to the interface this is bridged to.

RETURN VALUE
  NULL: if not set
  the address of the interface if it is set.
===========================================================================*/
INLINE ps_iface_type *ps_iface_bridge_iface
(
  ps_iface_type *iface_ptr
)
{
  if( PS_IFACE_IS_VALID(iface_ptr) )
  {
    return ((iface_ptr)->iface_private.tx_bridge_ptr);
  }
  return NULL;
}

/*===========================================================================
FUNCTION PS_IFACE_FLOW_ENABLED()

DESCRIPTION
  This macro returns whether the interface is has flow disabled.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  TRUE: if flow is disabled
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_FLOW_ENABLED ps_iface_flow_enabled
INLINE boolean ps_iface_flow_enabled
(
  ps_iface_type *iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr) &&
     ((iface_ptr)->iface_private.tx_flow_mask == ALL_FLOWS_ENABLED))
  {
    return TRUE;
  }
  return FALSE;

} /*  ps_iface_flow_enabled() */

/*===========================================================================
MACRO PS_IFACE_GET_FLOW_MASK()

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
INLINE uint32 ps_iface_get_flow_mask
(
  ps_iface_type *iface_ptr
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    return ( (iface_ptr)->iface_private.tx_flow_mask );
  }

  return 0;
} /*  ps_iface_get_flow_mask() */

/*===========================================================================
MACRO PS_IFACE_HANDLE_FLOW_CTL()

DESCRIPTION
  This macro will free a packet if the interface passed in is flow
  controlled, and will return -1

PARAMETERS
  iface_ptr: pointer to the interface on which to TX.
  pkt_ptr_ptr:  dsm item that was being Txed
  meta_info_ptr: meta infor for the packet being TXed
===========================================================================*/
#define PS_IFACE_HANDLE_FLOW_CTL(iface_ptr, pkt_ptr_ptr, meta_info_ptr)     \
  if(iface_ptr->iface_private.tx_flow_mask != 0)                            \
  {                                                                         \
    PS_TX_META_INFO_FREE(&meta_info_ptr);                                      \
    dsm_free_packet(pkt_chain_ptr);                                         \
                                                                            \
    return -1;                                                              \
  }

/*===========================================================================
MACRO PS_IFACE_NUM_FILTERS()

DESCRIPTION
  This macro returns the current number of IP filters in the iface for the
  specified client.

PARAMETERS
  iface_ptr: pointer to the interface on which operation is requested.
  client: filtering client id for which the filter count is requested

RETURN VALUE
  As described above.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_NUM_FILTERS( iface_ptr, client )                        \
  (PS_IFACE_IS_VALID(iface_ptr)                                          \
    ? PS_IFACEI_NUM_FILTERS(iface_ptr, client)                           \
    : -1)

/*===========================================================================
MACRO PS_IFACE_GET_ID()

DESCRIPTION
  This macro returns the opaque id of the iface given an iface ptr.
  This id can be used by external clients to access/control the iface.

PARAMETERS
  iface_ptr: pointer to the interface on which operation is requested.

RETURN VALUE
  A valid iface id or PS_IFACE_INVALID_ID if iface_ptr is NULL.

DEPENDENCIES
  iface_ptr should be valid.

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_ID( iface_ptr )                                    \
   ( (PS_IFACE_IS_VALID(iface_ptr))                                     \
     ?  ( ((uint32)((iface_ptr)->iface_private.iface_index)) << 24 |    \
          0x00FFFF00 | PS_IFACE_DEFAULT_FLOW_INST )                     \
     : PS_IFACE_INVALID_ID)


/*===========================================================================
MACRO PS_IFACE_GET_ID2()

DESCRIPTION
  This macro returns the opaque id of the iface given an iface name and
  instance. This id can be used by external clients to access/control
  the iface.

PARAMETERS
  iface_name: Name of an iface of type ps_iface_name_enum_type
  iface_inst: Instance of an iface (uint8)

RETURN VALUE
  A valid iface id corresponding to supplied iface name and iface instance
  and primary physical link instance.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_ID2  ps_iface_get_id2
INLINE ps_iface_id_type ps_iface_get_id2
(
  ps_iface_name_enum_type  iface_name,
  uint8                    iface_inst
)
{
  ps_iface_type  ** if_ptr;
  uint32             i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if_ptr = global_iface_ptr_array;
  for (i = 0; i < MAX_SYSTEM_IFACES && NULL != *if_ptr; i++, if_ptr++)
  {
    if ((*if_ptr)->name == iface_name && (*if_ptr)->instance == iface_inst)
    {
      return ( (i << 24) | 0x00FFFF00 | (uint32)PS_IFACE_DEFAULT_FLOW_INST );
    }
  }

  return PS_IFACE_INVALID_ID;
}



/*===========================================================================
MACRO PS_IFACE_GET_ID3()

DESCRIPTION
  This macro returns the opaque id of the iface given an iface name iface
  instance and physical link instance. This id can be used by external clients
  to access/control the iface.

PARAMETERS
  iface_name : Name of an iface of type ps_iface_name_enum_type
  iface_inst : Instance of an iface
  flow_inst  : Instance of the physical link

RETURN VALUE
  An iface id.

DEPENDENCIES
  Parameters must be valid. Else invalid iface id will be returned which will
  fail when operated upon.

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_ID3  ps_iface_get_id3
INLINE ps_iface_id_type ps_iface_get_id3
(
  ps_iface_name_enum_type   iface_name,
  uint8                     iface_inst,
  uint8                     flow_inst
)
{
  ps_iface_type  ** if_ptr;
  uint32             i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if_ptr = global_iface_ptr_array;
  for (i = 0; i < MAX_SYSTEM_IFACES && NULL != *if_ptr; i++, if_ptr++)
  {
    if ((*if_ptr)->name == iface_name && (*if_ptr)->instance == iface_inst)
    {
      return ((i << 24) | 0x00FFFF00 | (uint32)flow_inst );
    }
  }

  return PS_IFACE_INVALID_ID;
}


/*===========================================================================
MACRO PS_IFACE_GET_NAME_FROM_ID()

DESCRIPTION
  This macro returns the iface name given an iface id.

PARAMETERS
  iface_id: Opaque id which uniquely identifies and iface

RETURN VALUE
  Iface name.

DEPENDENCIES
  iface_id should be valid.

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_NAME_FROM_ID( iface_id )         \
  ( ((uint8)(iface_id >> 24) < MAX_SYSTEM_IFACES )                        \
      ? global_iface_ptr_array[(uint8)(iface_id >> 24)]->name             \
      : PS_IFACE_INVALID_NAME )


/*===========================================================================
MACRO PS_IFACE_GET_NAME()

DESCRIPTION
  This macro returns the iface name given an iface ptr.

PARAMETERS
  iface_ptr: iface pointer

RETURN VALUE
  Iface name.

DEPENDENCIES
  iface_id should be valid.

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_NAME( iface_ptr )         \
  ( (iface_ptr != NULL ) ? iface_ptr->name : PS_IFACE_INVALID_NAME )


/*===========================================================================
MACRO PS_IFACE_GET_INST_FROM_ID()

DESCRIPTION
  This macro returns the iface instance given an iface id.

PARAMETERS
  iface_id: Opaque id which uniquely identifies and iface

RETURN VALUE
  Iface instance.

DEPENDENCIES
  iface_id should be valid.

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_INST_FROM_ID( iface_id )       \
      ( ((iface_id >> 24) < MAX_SYSTEM_IFACES )                         \
      ? global_iface_ptr_array[(uint8)(iface_id >> 24)]->instance       \
      : PS_IFACE_INVALID_INSTANCE )

/*===========================================================================
MACRO PS_IFACE_IS_ID_A_GROUP()

DESCRIPTION
  This macro returns a boolean indicating whether the passed iface name is
  the name of an iface group or not.

PARAMETERS
  iface_name: name of the iface

RETURN VALUE
  TRUE: if the iface name corresponds to a iface group
  FALSE: otherwise.
===========================================================================*/
#define PS_IFACE_IS_ID_A_GROUP( iface_id )                                  \
      ( ((iface_id >> 24) < MAX_SYSTEM_IFACES)                              \
      ? !(((uint16)global_iface_ptr_array[(uint8)(iface_id >> 24)]->name)   \
        & 0x8000)                                                           \
      : 0 )


/*===========================================================================
MACRO PS_IFACE_IS_UP()

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
#define PS_IFACE_IS_UP( iface_ptr )                                 \
  (PS_IFACE_IS_VALID(iface_ptr)                                     \
    ? ((iface_ptr)->iface_private.state == IFACE_COMING_UP   ||           \
       (iface_ptr)->iface_private.state == IFACE_ROUTEABLE   ||           \
       (iface_ptr)->iface_private.state == IFACE_CONFIGURING ||           \
       (iface_ptr)->iface_private.state == IFACE_UP)                      \
    : FALSE)


/*===========================================================================
MACRO PS_IFACE_IS_IN_USE()

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
#define PS_IFACE_IS_IN_USE( iface_ptr )                          \
  (PS_IFACE_IS_VALID(iface_ptr) ? (iface_ptr)->iface_private.in_use    \
                                : FALSE)

/*===========================================================================
MACRO PS_IFACE_IS_IN_LAPTOP_CALL()

DESCRIPTION
  This macro returns TRUE if iface is in a laptop call

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  TRUE: if iface is in a laptop call
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IS_IN_LAPTOP_CALL( iface_ptr )                          \
  (PS_IFACE_IS_VALID(iface_ptr)                                          \
     ? ((iface_ptr)->iface_private.proc_id != PS_IFACE_PROC_ID_NONE &&   \
        (iface_ptr)->iface_private.proc_id != PS_IFACE_PROC_ID_LOCAL)    \
     : FALSE)

/*===========================================================================
MACRO PS_IFACE_IS_ADDR_FAMILY_V4()

DESCRIPTION
  This macro returns TRUE if the address family of the iface is IPv4.
  Returns False otherwise.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  TRUE: if iface family is IPv4
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IS_ADDR_FAMILY_V4( iface_ptr )                          \
  (PS_IFACE_IS_VALID(iface_ptr) ? ps_iface_addr_family_is_v4(iface_ptr)  \
                                : FALSE)


/*===========================================================================
MACRO PS_IFACE_SET_IN_USE()

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
#define PS_IFACE_SET_IN_USE( iface_ptr )                         \
  if (PS_IFACE_IS_VALID(iface_ptr))                              \
  {                                                              \
    (iface_ptr)->iface_private.in_use = TRUE;                          \
  }


/*===========================================================================
MACRO PS_IFACE_RESET_IN_USE()

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
#define PS_IFACE_RESET_IN_USE( iface_ptr )                         \
  if (PS_IFACE_IS_VALID(iface_ptr))                                \
  {                                                                \
    (iface_ptr)->iface_private.in_use = FALSE;                           \
  }

/*===========================================================================
MACRO PS_IFACE_GET_PROC_ID()

DESCRIPTION
  This macro returns proc_id in the iface

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_PROC_ID( iface_ptr )                                \
  (PS_IFACE_IS_VALID(iface_ptr)                                          \
     ? (iface_ptr)->iface_private.proc_id                                \
     : PS_IFACE_PROC_ID_NONE)

/*===========================================================================
MACRO PS_IFACE_SET_PROC_ID()

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
#define PS_IFACE_SET_PROC_ID( iface_ptr, user_proc_id )                  \
  if (PS_IFACE_IS_VALID(iface_ptr))                              \
  {                                                              \
    PS_IFACEI_SET_PROC_ID( iface_ptr, user_proc_id );                     \
  }

/*===========================================================================
MACRO PS_IFACE_RESET_PROC_ID()

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
#define PS_IFACE_RESET_PROC_ID( iface_ptr )                              \
  if (PS_IFACE_IS_VALID(iface_ptr))                               \
  {                                                               \
    PS_IFACEI_RESET_PROC_ID( iface_ptr );                                \
  }

/*===========================================================================
MACRO PS_IFACE_SET_SUBS_ID()

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
#define PS_IFACE_SET_SUBS_ID ps_iface_set_subs_id
INLINE void ps_iface_set_subs_id
(
  ps_iface_type *iface_ptr,
  uint32         subs_id
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.subs_id = subs_id;
  }

} /*  ps_iface_set_subs_id() */

/*===========================================================================
MACRO PS_IFACE_GET_SUBS_ID()

DESCRIPTION
  This macro gets the subs id(Subscription ID) in the iface

PARAMETERS
  iface_ptr: Target iface ptr  

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_SUBS_ID( iface_ptr )                                \
  (PS_IFACE_IS_VALID(iface_ptr)                                          \
     ? (iface_ptr)->iface_private.subs_id                                \
     : 0x00)


/*===========================================================================
MACRO PS_IFACE_SET_APN_TYPE()

DESCRIPTION
  This macro sets the APN type in the iface

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
#define PS_IFACE_SET_APN_TYPE ps_iface_set_apn_type
INLINE void ps_iface_set_apn_type
(
  ps_iface_type                 *iface_ptr,
  ps_iface_apn_enum_type         apn_type
)
{
  if(PS_IFACE_IS_VALID(iface_ptr))
  {
    (iface_ptr)->iface_private.apn_type = apn_type;
  }

} /*  ps_iface_set_apn_type() */

/*===========================================================================
MACRO PS_IFACE_GET_APN_TYPE()

DESCRIPTION
  This macro gets the apn type in the iface

PARAMETERS
  iface_ptr: Target iface ptr  

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_APN_TYPE( iface_ptr )                               \
  (PS_IFACE_IS_VALID(iface_ptr)                                          \
     ? (iface_ptr)->iface_private.apn_type                               \
     : 0x00)

/*===========================================================================
MACRO PS_IFACE_IS_LOOKED_UP()

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
#define PS_IFACE_IS_LOOKED_UP( iface_ptr )                         \
  (PS_IFACE_IS_VALID(iface_ptr) ? (iface_ptr)->iface_private.looked_up   \
                                : FALSE)


/*===========================================================================
MACRO PS_IFACE_SET_LOOKED_UP()

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
#define PS_IFACE_SET_LOOKED_UP( iface_ptr )                      \
  if (PS_IFACE_IS_VALID(iface_ptr))                              \
  {                                                              \
    (iface_ptr)->iface_private.looked_up = TRUE;                       \
  }


/*===========================================================================
MACRO PS_IFACE_RESET_LOOKED_UP()

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
#define PS_IFACE_RESET_LOOKED_UP( iface_ptr )                     \
  if (PS_IFACE_IS_VALID(iface_ptr))                               \
  {                                                               \
    (iface_ptr)->iface_private.looked_up = FALSE;                       \
  }


/*===========================================================================
MACRO PS_IFACE_IP_INFO_IS_INHERITED

DESCRIPTION
  This macro returns TRUE if the inherit flag is TRUE *and* if there is
  an associated IFACE.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  TRUE  - ip info inherited.
  FALSE - ip info not inherited.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IP_INFO_IS_INHERITED(iface_ptr)                  \
  (PS_IFACE_IS_VALID(iface_ptr)                                   \
     ? PS_IFACEI_IP_INFO_IS_INHERITED(iface_ptr)                 \
     : FALSE)

/*===========================================================================
MACRO PS_IFACE_GET_IFACE_INDEX()

DESCRIPTION
  This macro gets the private iface_index for the iface.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  iface_index

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_IFACE_INDEX( iface_ptr )                     \
  ( PS_IFACE_IS_VALID(iface_ptr) ?                                \
    ((iface_ptr)->iface_private.iface_index) : PS_IFACE_INVALID_IFACE_INDEX )

/*===========================================================================
MACRO PS_IFACE_SET_V4_DNS_SERVER_ADDR()

DESCRIPTION
  This macro sets primary and secondary DNS servers on a given interface

PARAMETERS
  iface_ptr          : Target iface ptr
  prim_dns_serv_addr : Primary DNS server's address
  seco_dns_serv_addr : Secondary DNS server's address

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_SET_V4_DNS_SERVER_ADDR( iface_ptr, prim_dns_serv_addr,  \
                                         seco_dns_serv_addr )            \
  if (PS_IFACE_IS_VALID(iface_ptr))                                      \
  {                                                                      \
    (iface_ptr)->v4_net_info.primary_dns.ps_s_addr   = prim_dns_serv_addr;  \
    (iface_ptr)->v4_net_info.secondary_dns.ps_s_addr = seco_dns_serv_addr;  \
  }

/*===========================================================================
MACRO PS_IFACE_SET_V4_GATEWAY_ADDR()

DESCRIPTION
  This macro sets the gateway address on a given interface

PARAMETERS
  iface_ptr          : Target iface ptr
  gw_addr            : Gateway address

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_SET_V4_GATEWAY_ADDR( iface_ptr, gw_addr )               \
  if (PS_IFACE_IS_VALID(iface_ptr))                                      \
  {                                                                      \
    (iface_ptr)->v4_net_info.gateway.ps_s_addr  = gw_addr;               \
  }

/*===========================================================================
MACRO PS_IFACE_SET_V4_NETMASK()

DESCRIPTION
  This macro sets the net mask on a given interface

PARAMETERS
  iface_ptr          : Target iface ptr
  net_mask           : Net Mask

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_SET_V4_NET_MASK( iface_ptr, new_net_mask )              \
  if (PS_IFACE_IS_VALID(iface_ptr))                                      \
  {                                                                      \
    (iface_ptr)->v4_net_info.net_mask.ps_s_addr  = new_net_mask;         \
  }

/*===========================================================================
MACRO PS_IFACE_GET_MTU()

DESCRIPTION
  This macro gets MTU of the iface

PARAMETERS
  iface_ptr : Target iface ptr

RETURN VALUE
  mtu : MTU of the iface
  0   : if iface is invalid

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_MTU( iface_ptr) ps_iface_get_mtu( iface_ptr )

/*===========================================================================
MACRO PS_IFACE_SET_MTU()

DESCRIPTION
  This macro sets MTU for the iface

PARAMETERS
  iface_ptr : Target iface ptr
  mtu       : MTU to be set

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_SET_MTU( iface_ptr, new_mtu)                            \
  ps_iface_set_mtu( iface_ptr, new_mtu )

/*===========================================================================
MACRO PS_IFACE_SET_IFACE_CAPABILITY()

DESCRIPTION
  This macro sets a capability for the iface.

PARAMETERS
  iface_ptr: Target iface ptr
  capability: capability which needs to be set

NOTE: only one capability can be set each time

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_CAPABILITY(iface_ptr, capability)                     \
  if (PS_IFACE_IS_VALID(iface_ptr) &&                                      \
      (capability) >= PS_IFACE_CAPABILITY_MIN &&                          \
      (capability) < PS_IFACE_CAPABILITY_MAX)                             \
  {                                                                      \
    ((iface_ptr)->iface_private.capability_mask |= (0x1 << (capability))); \
  }

/*===========================================================================
MACRO PS_IFACE_RESET_CAPABILITY()

DESCRIPTION
  Resets a capability of an iface.

  NOTE : Only one capability can be reset each time.

PARAMETERS
  iface_ptr   : pointer to iface
  capability : capability which needs to be reset

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACE_RESET_CAPABILITY(iface_ptr, capability)                   \
  if (PS_IFACE_IS_VALID(iface_ptr) &&                                      \
      (capability) >= PS_IFACE_CAPABILITY_MIN &&                          \
      (capability) < PS_IFACE_CAPABILITY_MAX)                             \
  {                                                                      \
    ((iface_ptr)->iface_private.capability_mask &= ~(0x1 << (capability))); \
  }

/*===========================================================================
MACRO PS_IFACE_GET_CAPABILITY()

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
#define PS_IFACE_GET_CAPABILITY(iface_ptr, capability)                     \
  ((PS_IFACE_IS_VALID(iface_ptr) &&                                        \
    (capability) >= PS_IFACE_CAPABILITY_MIN &&                            \
    (capability) < PS_IFACE_CAPABILITY_MAX)                               \
      ?  ((iface_ptr)->iface_private.capability_mask & (0x1 << (capability))) \
      : FALSE)


/**
  @def      PS_IFACE_GET_EVENT_HANDLE(iface_ptr)  
  @brief    Get event handle from iface index and cookie values
 
  @details  calclate event handle from iface index and cookie value 
  
  @param
    iface_ptr[in]    pointer to iface
 
  @return
    event handle
  
*/
#define PS_IFACE_GET_EVENT_HANDLE( iface_ptr )      \
  ( (iface_ptr->iface_private.iface_index << 16) |  \
    ((iface_ptr->iface_private.cookie)) )

/*===========================================================================
MACRO PS_IFACE_IS_CALL_FROM_TETHERED_HOST()

DESCRIPTION
  This macro returns true if iface is bound to tethered host.

PARAMETERS
  iface_ptr  : iface instance ptr

RETURN VALUE
  TRUE  : iface is bound to tethered host
  FALSE : iface is not bound to tethered host

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IS_CALL_FROM_TETHERED_HOST  ps_iface_is_call_from_tethered_host
INLINE boolean ps_iface_is_call_from_tethered_host
(
  ps_iface_type * iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_IFACE_PROC_ID_TE_RMNET == PS_IFACE_GET_PROC_ID(iface_ptr) ||
      PS_IFACE_PROC_ID_TE_DUN == PS_IFACE_GET_PROC_ID(iface_ptr))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
MACRO PS_IFACE_GET_3GPP_PROFILE_ID()

DESCRIPTION
  This macro returns the the 3gpp profile id

PARAMETERS
  iface_ptr: pointer to iface

RETURN VALUE
  Success : 3gpp profile ID
  Failure : 0xFFFFFFFF

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_3GPP_PROFILE_ID(iface_ptr)                    \
  (PS_IFACE_IS_VALID(iface_ptr)                                      \
    ? (iface_ptr)->iface_private.profile_id_3gpp                       \
    : 0xFFFFFFFFUL)

/*===========================================================================
MACRO PS_IFACE_GET_3GPP2_PROFILE_ID()

DESCRIPTION
  This macro returns the the 3gpp2 profile id

PARAMETERS
  iface_ptr: pointer to iface

RETURN VALUE
  Success : 3gpp2 profile ID
  Failure : 0xFFFFFFFF

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_3GPP2_PROFILE_ID(iface_ptr)                    \
  (PS_IFACE_IS_VALID(iface_ptr)                                      \
    ? (iface_ptr)->iface_private.profile_id_3gpp2                       \
    : 0xFFFFFFFFUL)

/*===========================================================================
MACRO PS_IFACE_SET_PROPERTY()

DESCRIPTION
  This macro sets a property for the iface.

PARAMETERS
  iface_ptr: Target iface ptr
  propert : property which needs to be set

NOTE: only one property can be set each time

RETURN VALUE
  none
===========================================================================*/
#define PS_IFACE_SET_PROPERTY(iface_ptr, mask)                          \
  if (PS_IFACE_IS_VALID(iface_ptr) &&                                   \
      (mask) >= PS_IFACE_SUPPORTED_FLAG_MIN &&                          \
      (mask) < PS_IFACE_SUPPORTED_FLAG_MAX)                             \
  {                                                                     \
    (iface_ptr)->iface_private.properties_mask |= ( 0x1 << (mask) );    \
  }

/*===========================================================================
MACRO PS_IFACE_RESET_PROPERTY()

DESCRIPTION
  Resets a property of an iface.

  NOTE : Only one property can be reset each time.

PARAMETERS
  iface_ptr   : pointer to iface
  property : property which needs to be reset

RETURN VALUE
  None
===========================================================================*/
#define PS_IFACE_RESET_PROPERTY(iface_ptr, mask)                           \
  if (PS_IFACE_IS_VALID(iface_ptr) &&                                      \
      (mask) >= PS_IFACE_SUPPORTED_FLAG_MIN &&                             \
      (mask) < PS_IFACE_SUPPORTED_FLAG_MAX)                                \
  {                                                                        \
    (iface_ptr)->iface_private.properties_mask &= ~(0x1 << (mask));        \
  }

/*===========================================================================
MACRO PS_IFACE_GET_PROPERTY()

DESCRIPTION
  Returns TRUE if a property of an iface is turned on.

  NOTE : Only one property can be fetched each time.

PARAMETERS
  iface_ptr   : pointer to iface
  property    : property which needs to be tested

RETURN VALUE
  TRUE  : if property is turned on
  FALSE : otherwise
===========================================================================*/
#define PS_IFACE_GET_PROPERTY(iface_ptr, mask)                           \
  ((PS_IFACE_IS_VALID(iface_ptr) && (mask) >= PS_IFACE_SUPPORTED_FLAG_MIN && \
    (mask) < PS_IFACE_SUPPORTED_FLAG_MAX)                                \
      ? ((iface_ptr)->iface_private.properties_mask & (0x1 << (mask)))   \
      : FALSE)
    
/*===========================================================================
MACRO PS_IFACE_SET_IPV6_GATEWAY_IID()

DESCRIPTION
  This macro sets the gateway iid for default ipv6 addr

PARAMETERS
  iface_ptr: pointer to iface
  iid      : gateway iid

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_SET_IPV6_GATEWAY_IID(iface_ptr, iid)                 \
  if (PS_IFACE_IS_VALID(iface_ptr))                                   \
  {                                                                   \
    (iface_ptr)->iface_private.ipv6_addrs[0]->gateway_iid  = iid;     \
  }    
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
int32 ps_iface_create_internal
(
  ps_iface_type            * this_iface_ptr,
  ps_iface_name_enum_type    name,
  ps_phys_link_type        * phys_link_array,
  uint8                      num_phys_links
);

/*===========================================================================
FUNCTION PS_IFACE_CREATE()

DESCRIPTION
    This will memset()s the private data struct to 0, initializes the
    "this_ptr" to point to the interface control block itself, initializes
    the queues and the TX ptr to the default value.

    NOTE: the memset() sets the iface_state to DISABLED.

PARAMETERS
    this_iface_ptr: ptr to interface control block on which to operate on.
    name:     the name of the interface (like CDMA_SN_IFACE)
    this_iface_outgoing_acl_ptr: pointer to the outgoing acl to include the
                                 defailt ACL in the creation of the IFACE.
    this_iface_incoming_acl_ptr: pointer to the incoming acl to include the
                                 defailt ACL in the creation of the IFACE.
    phys_link_array: array of physlinks associated with this interface.
    num_phys_links: the number of physlinks in the array

RETURN VALUE
  -1  - error in iface creation
  >=0 - Else instance number of the newly created iface

DEPENDENCIES
  The interface being created MUST NOT be freed before this function returns

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_create
(
  ps_iface_type          *this_iface_ptr,
  ps_iface_name_enum_type name,
  acl_type               *this_iface_outgoing_acl_ptr,
  acl_type               *this_iface_incoming_acl_ptr,
  ps_phys_link_type      *phys_link_array,
  uint8                   num_phys_links
);

/*===========================================================================
FUNCTION PS_IFACE_ACTIVE_ASSOCIATE()

DESCRIPTION
  This function is used to (re)associate a logical iface with another iface
  Currently can only associate to a physical iface.

  This will register interest in the underlying phys link (i.e, inc() its
  ref cnt), as well as bring_up() the target physical ps_iface.  If the
  logical_iface was pointing at another ps_iface previously, then that
  ps_iface is torn down.  If that also implies that the logical_iface was
  pointing at a different phys_link, then the old phys_link's ref_Cnt is
  also decremented.

  NOTE: This should be called after the logical iface has been created i.e.
  after the call to ps_iface_create()

PARAMETERS
  logical_iface_ptr: Ptr to the logical interface
  this_iface_ptr: Ptr to interface control block to associate with.
  inherit_ip_info: TRUE = IP info inherited from base iface.
  ps_errno: Returns an error code.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  This function MUST be TASKLOCK()ed

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_active_associate
(
  ps_iface_type                  *logical_iface_ptr,
  ps_iface_type                  *this_iface_ptr,
  boolean                         inherit_ip_info,
  int16                          *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_PASSIVE_ASSOCIATE()

DESCRIPTION
  This function is used to (re)associate a logical iface with another iface
  Currently can only associate to a physical iface.

  This function does not explicity express interest in an iface. In other
  words, no ref counts are changed, and no interfaces are brought up as
  a result of this call.

  If the inherit_ip_info boolean is TRUE, then also drills down
  via new assoc_iface to get new addr_family.

  NOTE: This should be called after the logical iface has been created i.e.
  after the call to ps_iface_create()

PARAMETERS
  logical_iface_ptr: Ptr to the logical interface
  this_iface_ptr: Ptr to interface control block to associate with.
  inherit_ip_info: TRUE = IP info inherited from base iface.
  ps_errno: Returns an error code.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  This function MUST be TASKLOCK()ed

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_passive_associate
(
  ps_iface_type                  *logical_iface_ptr,
  ps_iface_type                  *new_assoc_iface_ptr,
  boolean                         inherit_ip_info,
  int16                          *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ACTIVE_DISASSOCIATE()

DESCRIPTION
  This function is used to disassociate a logical iface from the underlying
  ps_iface.

  This will de_register interest in the underlying phys link (i.e, dec() its
  ref cnt), as well as tear_down() the target physical ps_iface.

  NOTE: This should be called after the logical iface has been created i.e.
  after the call to ps_iface_create()

PARAMETERS
  logical_iface_ptr: Ptr to the logical interface
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function for
                   underlying tear_down.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  This function MUST be TASKLOCK()ed

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_active_disassociate
(
  ps_iface_type                  *logical_iface_ptr,
  int16                          *ps_errno,
  void                           *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_PASSIVE_DISASSOCIATE()

DESCRIPTION
  This function is used to passively disassociate a logical iface from the
  underlying ps_iface.

  NOTE: This should be called after the logical iface has been created i.e.
  after the call to ps_iface_create()

PARAMETERS
  logical_iface_ptr: Ptr to the logical interface
  ps_errno: Returns an error code.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  This function MUST be TASKLOCK()ed

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_passive_disassociate
(
  ps_iface_type                  *logical_iface_ptr,
  int16                          *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_DELETE()

DESCRIPTION
  It will flush all routes for this interface.  Then will memset() the
  entire data structure.

  NOTE: This should be called AFTER the delete_ind() has been called -
  otherwise it will be impossible to call the callbacks (they will no longer
  be linked to the iface.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  This function MUST be TASKLOCK()ed

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_delete
(
  ps_iface_type *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ALLOC_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to allocate the memory used to register for ps_iface
  event callbacks.

PARAMETERS
  event_cback_f_ptr: the callback to be registered
  user_data_ptr: data to be called with the callback.

RETURN VALUE
  NULL: if allocation failed
  ptr to memory being allocated.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void *ps_iface_alloc_event_cback_buf
(
  ps_iface_event_cback_type event_cback_f_ptr,
  void                     *user_data_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_FREE_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to allocate the memory used to register for ps_iface
  event callbacks.

PARAMETERS
  buf: the previously allocated buffer.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_free_event_cback_buf
(
  void *buf
);


/*===========================================================================
FUNCTION PS_IFACE_EVENT_CBACK_REG()

DESCRIPTION
  Registers callbacks for events on an interface.  Whenever an event occurs,
  the callback for that event is called.  If the event is true when
  registration happens the callback is called immediately.  This will fail if
  either the interface is invalid, or the buffer to queue (with callback and
  user data pointer) is NULL.  A callback will only be queued if the event
  has not previously been queued.

  Memory is owned by the caller and it is put into the queue for that
  event.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  event:  Event that triggers the callbacks
  buf:    Buffer that holds the callback pointer. Should not be NULL.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  The link field must have been initialized with q_link().

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_event_cback_reg
(
  ps_iface_type           *this_iface_ptr,
  ps_iface_event_enum_type event,
  void                    *buf
);


/*===========================================================================
FUNCTION PS_IFACE_EVENT_CBACK_DEREG()

DESCRIPTION
  DeRegisters callbacks for a specific event on an interface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  event:  Event that triggers the callbacks
  buf:    Buffer that holds the callback pointer. This buffer is removed
          from the queue of callbacks. Should not be NULL.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_event_cback_dereg
(
  ps_iface_type           *this_iface_ptr,
  ps_iface_event_enum_type event,
  void                    *buf
);


/*===========================================================================
FUNCTION PS_IFACE_GET_HANDLE()

DESCRIPTION
  This function returns a ps_iface handle based on the interface name and
  instance passed in.  If no such interface exists, a value of NULL is
  returned.

PARAMETERS
  Pointer to struct containing name and instance information.

RETURN VALUE
  ps_iface handle on success
  NULL on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type *ps_iface_get_handle
(
  ps_iface_id_type iface_id
);

/*===========================================================================
FUNCTION PS_IFACE_SET_BRIDGE()

DESCRIPTION
  This function will set the bridge pointer and inform the address server
  the bridging mode.  If the bridged_iface_ptr is NULL this will "un-bridge"
  the interface.

PARAMETERS
  this_iface_ptr:    ptr to interface control block on which to operate on.
  bridged_iface_ptr: ptr to interface to bridge to.

RETURN VALUE
  None

DEPENDENCIES
  This should only ever be called by a client from a SINGLE task context.

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_set_bridge
(
  ps_iface_type *this_iface_ptr,
  ps_iface_type *bridged_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_SET_TX_FUNCTION()

DESCRIPTION
  This function is used to set the TX function.  Unsetting the function is
  done by passing in NULL for the tx_cmd parameter.

PARAMETERS
  this_iface_ptr:  interface on which to operate
  tx_cmd:          Tx command being set
  tx_cmd_info:     Info to pass to registered cmd handler

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_set_tx_function
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_tx_cmd_f_ptr_type tx_cmd,
  void                      *tx_cmd_info
);

/*===========================================================================
FUNCTION PS_IFACE_SET_RX_FUNCTION()

DESCRIPTION
  This function is used to set the RX function.

PARAMETERS
  this_iface_ptr:  interface on which to operate
  rx_cmd:          Rx command being set

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_set_rx_function
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_data_proc_f_ptr_type  rx_cmd
);

/*===========================================================================
MACRO PS_IFACE_UNSET_TX_FUNCTION()

DESCRIPTION
  This macro is simply a shortcut for clearing the tx function.  It simply
  calls the set command with the NULL as the argument.

PARAMETERS
  this_iface_ptr:  interface on which to operate

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_iface_unset_tx_function( this_iface_ptr) \
  ps_iface_set_tx_function((this_iface_ptr), NULL, NULL)


/*===========================================================================
FUNCTION PS_IFACE_ENABLE_FLOW()

DESCRIPTION
  Interface user enables flow on the interface.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_enable_flow
(
  ps_iface_type *this_iface_ptr,
  uint32         flow_mask
);

/*===========================================================================
FUNCTION PS_IFACE_DISABLE_FLOW()

DESCRIPTION
  client disables flow on the interface.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_disable_flow
(
  ps_iface_type *this_iface_ptr,
  uint32         flow_mask
);

/*===========================================================================
FUNCTION PS_IFACE_ENABLE_FLOW_EX()

DESCRIPTION
  Posts a command to PS task to enable flow on the interface.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_enable_flow_ex
(
  ps_iface_type  * this_iface_ptr,
  uint32           flow_mask
);

/*===========================================================================
FUNCTION PS_IFACE_DISABLE_FLOW_EX()

DESCRIPTION
  Posts a command to PS task to disable flow on the interface.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_disable_flow_ex
(
  ps_iface_type  * this_iface_ptr,
  uint32           flow_mask
);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                       IP ADDRESS INTERFACE FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_IFACE_SET_V4_ADDR()

DESCRIPTION
  This function is used to set the local address - this should only be called
  by the addr_event() function - DO NOT TELL the address server - the caller
  is responsible for that.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr: V4 address to set.

RETURN VALUE
  0 if sucessful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_v4_addr
(
  ps_iface_type          * this_iface_ptr,
  const ps_ip_addr_type  * ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_ADDR()

DESCRIPTION

  NOTE THAT THIS FUNCTION IS DEPRECATED. USE PS_IFACE_SET_V4_ADDR() INSTEAD.

  This function is used to set the local address - this should only be called
  by the addr_event() function - DO NOT TELL the address server - the caller
  is responsible for that.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr: V4 address to set.

RETURN VALUE
  0 if sucessful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_iface_set_addr ps_iface_set_v4_addr

/*===========================================================================
FUNCTION PS_IFACE_GET_ADDR()

DESCRIPTION
  This function is used to get the local address of the passed in interface.
  If on interface type is specified in the ip_addr_ptr, either v4 or v6 will
  be returned depending on which one is set (v4 will always be returned if it
  is set).

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_addr
(
  ps_iface_type *this_iface_ptr,
  ps_ip_addr_type  *ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_IPV6_ADDR()

DESCRIPTION
  This function will return IPv6 address for the interface with appropriate
  scope based on the passed destination address.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  dest_ip_ptr:    Destination IPv6 address.
  ip_addr_ptr:    value return - the address will be  will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_ipv6_addr
(
  ps_iface_type            *this_iface_ptr,
  const struct ps_in6_addr *dest_ip_ptr,
  ps_ip_addr_type          *ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_CACHED_ADDR()

DESCRIPTION
  This function will return the address that is stored in the requested IP
  address field of the interface regardless of the state.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_cached_addr
(
  ps_iface_type *this_iface_ptr,
  ps_ip_addr_type  *ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_FAMILY_IS_ANY()

DESCRIPTION
  This function determines if the specified ps_iface is an ANY iface. This
  means that the interface supports receiving both IPv4 and IPv6 packets.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  true if any iface, false otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_addr_family_is_any
(
  const ps_iface_type        *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_FAMILY_IS_V4()

DESCRIPTION
  This function determines if the specified ps_iface is an IPv4 iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  true if v4 iface, false otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_addr_family_is_v4
(
  const ps_iface_type        *iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_FAMILY_IS_V6()

DESCRIPTION
  This function determines if the specified ps_iface is an IPv6 iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  true if v6 iface, false otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_addr_family_is_v6
(
  const ps_iface_type        *iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_ADDR_FAMILY()

DESCRIPTION
  This function will configure the interface to support the requested IP
  address family.

PARAMETERS
  this_iface_ptr:  ptr to interface control block on which to operate on.
  addr_family:    the address family to set this interface up for.

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_addr_family
(
  ps_iface_type *this_iface_ptr,
  ps_iface_addr_family_type  addr_family
);

/*===========================================================================
FUNCTION PS_IFACE_GET_ADDR_FAMILY()

DESCRIPTION
  This function retrieves the address family an interface is supporting

PARAMETERS
  this_iface_ptr:  ptr to interface control block on which to operate on.

RETURN VALUE
  The address family on the curent interface
  IFACE_INVALID_ADDR_FAMILY on failure.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_addr_family_type  ps_iface_get_addr_family
(
  ps_iface_type          *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_PEER_ADDR()

DESCRIPTION
  This function will return the address of the interfaces peer (e.g. the
  GGSN, or the PDSN) if this is a point-to-point link.  If the interface
  isn't point-to-point the invalid address will be returned.

PARAMETERS
  this_iface_ptr:  ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_peer_addr
(
  const ps_iface_type *this_iface_ptr,
  ps_ip_addr_type     *ip_addr_ptr
);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                  COMMANDS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_IFACE_BRING_UP_CMD()

DESCRIPTION
  This function is used to bring up an iface.  Will call the registered
  callback if the interface is in the down state.  If it is in the up state,
  it returns success.  If the interface is in the coming state or down it
  returne ewouldblock.  Finally if it is in the going down state, it returns
  closeinprogress.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ps_errno:       value to return error codes
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_bring_up_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_BRING_UP_CMD_EX()

DESCRIPTION
  This function is used to bring up an iface.  Will call the registered
  callback if the interface is in the down state.  If it is in the up state,
  it returns success.  If the interface is in the coming state or down it
  return DS_EWOULDBLOCK.  Finally if it is in the going down state, it
  returns DS_ECLOSEINPROGRESS.

  The operations goes like the following:
    - For a non-active iface,
      1. increment client ref count
      2. bring up iface
        - internally, call bring up cback is called only if the iface is in
          down state.
    - For an active iface
      1. increment client ref count
      2. bring up iface
        - internally, bring up cback is called only if the iface is in down
          state, e.g., the iface has not been brought up by the interface
          controller or by an app.
      3. check the ref count and call in_use_cback for the 1st iface client

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  app_priority : priority of the app requesting for iface to be up.
  ps_errno: value to return error codes
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: if interface is UP on return, operation succeeded
 -1: if interface is not UP on return
     operation result is in ps_errno:
     DS_EWOULDBLOCK:  interface is COMING UP
     else:            interface is DOWN and bring up failed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_bring_up_cmd_ex
(
  ps_iface_type  * this_iface_ptr,
  int32            app_priority,
  int16          * ps_errno,
  void           * client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ACTIVE_BRING_UP_CMD()

DESCRIPTION
  This function is used to bring up an iface.  This function is not required
  to be called for an application. It does not modify the ref_count. It simply
  tries to bring up an iface. Will call the registered
  callback if the interface is in the down state.  If it is in the up state,
  it returns success.  If the interface is in the coming state or down it
  returne ewouldblock.  Finally if it is in the going down state, it returns
  closeinprogress.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ps_errno:       value to return error codes
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_active_bring_up_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ACTIVE_BRING_UP_CMD_EX()

DESCRIPTION
  This function is used to bring up an iface.  The application priority is
  available in this routine and is used to setup the priority mask of the Iface.
  This function is not required to be called for an application. It does not
  modify the ref_count. It simply tries to bring up an iface. It calls registered
  callback if the interface is in the down state.  If it is in the up state,
  it returns success.  If the interface is in the coming state or down it will
  return EWOULDBLOCK.  Finally if it is in the going down state, it returns
  closeinprogress.

PARAMETERS
  this_iface_ptr:  ptr to interface control block on which to operate on.
  app_priority:    application priority, range [0,31]
  ps_errno:        value to return error codes
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_active_bring_up_cmd_ex
(
  ps_iface_type *this_iface_ptr,
  int32          app_priority,
  int16         *ps_errno,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_TEAR_DOWN_CMD()

DESCRIPTION
  This function is used to stop an iface.  It

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_TEAR_DOWN_CMD_EX()

DESCRIPTION
  This function is used to stop an iface.  If it is in the down state,
  it returns success.  If the interface is in the coming up or going down
  state it return DS_EWOULDBLOCK.

  The operations goes like the following:
    - For a non-active iface,
      1. decrement client ref count
      2. if ref count is down to 0, actually tear down iface
    - For an active iface
      1. decrement client ref count
      2. if ref count is down to 0, do not actually tear down iface. Instead,
         call out_use_cback for the last app using the iface (iface controller
         should explicitly tear down active ifaces).

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  app_priority : priority of the app for which iface is torn down.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_tear_down_cmd_ex
(
  ps_iface_type  * this_iface_ptr,
  int32            app_priority,
  int16          * ps_errno,
  void           * client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ACTIVE_TEAR_DOWN_CMD()

DESCRIPTION
  This function is used to stop an iface. It does not modify the ref count. It
  maybe called in cases other than the last app using the interface is done.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_active_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ACTIVE_TEAR_DOWN_CMD_EX()

DESCRIPTION
  This function is used to stop an iface. It does not modify the ref count. It
  maybe called in cases other than the last app using the interface is done.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  app_priority:    application priority, range [0,31]
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_active_tear_down_cmd_ex
(
  ps_iface_type *this_iface_ptr,
  int32          app_priority,
  int16         *ps_errno,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GO_NULL_CMD()

DESCRIPTION
  This function is used to stop an iface.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_go_null_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_UP_CMD()

DESCRIPTION
  This function is used to bring up the physical link on an iface.
  // more detail here
PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_phys_link_up_cmd
(
  const ps_iface_type *this_iface_ptr,
  int16               *ps_errno,
  void                *client_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_DOWN_CMD()

DESCRIPTION
  This function is used to tear down the physical link of an iface.
  // more detail here
PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_phys_link_down_cmd
(
  const ps_iface_type *this_iface_ptr,
  int16               *ps_errno,
  void                *client_data_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_TX_CMD()

DESCRIPTION
  This function will transmit to the iface specified - it checks if the state
  is UP, ROUTEABLE or CONFIGURING if it isn't then the interface must be a
  route on demand interface, if it is not, then the default tx functions is
  called (which frees the memory).

  Note: this function does not pay attention to the flow control state of the
        interface.  The client callback must do this.

PARAMETERS
  this_iface_ptr:    ptr to interface control block on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received for tx
  meta_info_ptr:     ptr to meta info (per pkt information)

RETURN VALUE
  0: if the packet was transmitted
 -1: if not

DEPENDENCIES
  None

SIDE EFFECTS
  The memory that was passed in will be freed by one of the tx functions.
===========================================================================*/
int ps_iface_tx_cmd
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **pkt_ref_ptr,
  ps_tx_meta_info_type  *meta_info_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_INPUT()

DESCRIPTION
   Interface client gives the protocol stack a packet to transmit.

PARAMETERS
  this_iface_ptr:    ptr to interface control block on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received
  meta_info_ptr:     ptr to meta info (TX/RX meta information)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_input
(
  ps_iface_type         *this_iface_ptr,
  dsm_item_type        **pkt_ref_ptr,
  ps_meta_info_type_ex  *meta_info_ptr
);



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                                 INDICATIONS

  These are intended to only be called by the interface clents/owners.  While
  there is no way to enforce this, having someone else call the indications
  is not supported behavior.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_IFACE_ENABLE_IND()

DESCRIPTION
  The given interface has been enabled.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_enable_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_DISABLED_IND()

DESCRIPTION
  The given interface has been disabled.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_disabled_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_DOWN_IND()

DESCRIPTION
  The given interface has been stopped.   Call callbacks.
  This also sets the physical link state to DOWN.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  down_ev_info_ptr : Iface down indication information

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_down_ind
(
  ps_iface_type                          * this_iface_ptr,
  ps_iface_down_mh_event_info_type       * down_ev_info_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_DOWN_IND_EX()

DESCRIPTION
  The given interface has been stopped.   Call callbacks. Pass the reason for
  the interface going down. This also sets the physical link state to DOWN.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  ps_iface_net_down_reason_type : network down reason.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_down_ind_ex
(
  ps_iface_type                * this_iface_ptr,
  ps_iface_net_down_reason_type  down_reason
);

/*===========================================================================
FUNCTION PS_IFACE_CONFIGURING_IND()

DESCRIPTION
  The given interface has been set to configuring.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_configuring_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_ROUTEABLE_IND()

DESCRIPTION
  The given interface has been set to routeable. The interface needs to be in
  the UP state before this indication is called.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  The interface should have been in the UP state.

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_routeable_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_UP_IND()

DESCRIPTION
  The given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_up_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_DOWN_IND()

DESCRIPTION
  The physical link of the given interface is down (707 dormancy).  Call
  callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_phys_link_down_ind
(
  const ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_UP_IND()

DESCRIPTION
  The physical link of the given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_phys_link_up_ind
(
  const ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_DELETED_IND()

DESCRIPTION
  The given interface has been deleted.  Call the callbacks.

  NOTE: This needs to be called before the

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_deleted_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_VALID_RA_IND()

DESCRIPTION
  This fucntion is called to indicate that a valid RA is received
  Appropriate event callback is posted.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on
  ra_life_time: lifetime of the RA

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_valid_ra_ind
(
  ps_iface_type                       *this_iface_ptr,
  uint16                               ra_life_time
);


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_UPDATED_IND()

DESCRIPTION
  This function is called to indicate that the number of IP filters
  installed in this iface by the specified client have changed.
  Appropriate event callback is posted.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on
  client_id: filtering client id for which the filters changed
  n_added: number of new filters added
  n_deleted: number of existing filters deleted

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_ipfltr_updated_ind
(
  ps_iface_type                       *this_iface_ptr,
  ps_iface_ipfltr_client_id_enum_type client_id,
  int                                 prev_cnt,
  int                                 new_cnt
);

/*===========================================================================
FUNCTION PS_IFACE_GENERIC_IND()

DESCRIPTION
  This function is called to indicate that an iface event has occurred. This
  function is to be used for iface events that are not directly applicable
  to the iface itself. (Ie. generic iface events)

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on
  event: event that has occurred.
  usr_data_ptr: data associated with the event.
RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_generic_ind
(
  ps_iface_type                       *this_iface_ptr,
  ps_iface_event_enum_type             event,
  void                                *event_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_CHANGE_PRI_PHYS_LINK()

DESCRIPTION
  This function is called to change the primary phys link associated with the
  iface.  The new primary phys link should be specified.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  iface_ptr: ptr to interface control block on which to operate on
  phys_link_ptr:  New primary phys link.
  info_code:      Extended info code indicating the reason of the primary
                  change.

RETURN VALUE
  None

DEPENDENCIES
  phys_link_ptr must be associatd with this iface.

SIDE EFFECTS
  Clients implcitly accessing the primary phys link through the iface ptr
  will obtain the new phys link.
===========================================================================*/
void ps_iface_change_pri_phys_link
(
  ps_iface_type                    * iface_ptr,
  ps_phys_link_type                * new_pri_phys_link_ptr,
  ps_extended_info_code_enum_type    info_code
);


/*===========================================================================
FUNCTION PS_IFACE_GET_V4_ADDR

DESCRIPTION
  This function returns the IPV4 address of an iface.  If the iface is
  NULL or IPV6, then it returns NULL.

PARAMETERS
  this_iface_ptr: Target iface ptr

RETURN VALUE
  An IPV4 address if the iface is valid and in the IPV4 family.  Else
  returns NULL.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 ps_iface_get_v4_addr
(
  ps_iface_type       *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_V4_DNS_ADDRS

DESCRIPTION
  This function returns the primary and secondary DNS addr's on the
  IPV4 iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  prim_dns_ptr:  storage for primary dns address
  sec_dns_ptr:  storage for secondary dns address

RETURN VALUE
  None.  However, if the addr family is not IPV4, then the input
  parameters are stored with zero.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_v4_dns_addrs
(
  ps_iface_type         *this_iface_ptr,
  ip_addr_type          *prim_dns_ptr,
  ip_addr_type          *sec_dns_ptr
);

/**
  PS_IFACE_GET_V4_NBNS_ADDRS
  This function returns the primary and secondary NBNS addr's on the
  IPV4 iface.

  @param[in] this_iface_ptr   Pointer to interface control block on which to
                              operate on.
  @param[out] prim_nbns_ptr   Storage for primary nbns address.
  @param[out] sec_nbns_ptr    Storage for secondary nbns address.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  None.
*/
void ps_iface_get_v4_nbns_addrs
(
  ps_iface_type         *this_iface_ptr,
  ip_addr_type          *prim_nbns_ptr,
  ip_addr_type          *sec_nbns_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_ALL_DNS_ADDRS

DESCRIPTION
  This function returns all the DNS addresses stored on the interface.

PARAMETERS
  this_iface_ptr:     ptr to interface control block on which to operate on.
  dns_addrs_ptr:      The ptr to the DNS addrs
  num_dns_addrs_ptr:  The number of DNS addresses space is allocated for.
                      This is an output param as well and will be set to the
                      # of valid DNS addresses actually returned.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_all_dns_addrs
(
  ps_iface_type         *this_iface_ptr,
  ip_addr_type          *dns_addrs_ptr,
  uint8                 *num_dns_addrs_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_IS_PHYS_LINK_VALID()

DESCRIPTION
  This function validates whether the specified phys link belongs to the
  specified iface.

PARAMETERS
  iface_ptr:  Pointer to interface
  phys_link_ptr:  Pointer to the phys link to validate

RETURN VALUE
  TRUE  If phys link belongs to ps_iface
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_iface_is_phys_link_valid PS_IFACE_IS_PHYS_LINK_VALID

/*===========================================================================
FUNCTION PS_IFACE_SET_NET_PARAMS()

DESCRIPTION
  This function sets the authentication parameters to be used next time the
  interface is brought up.
  The stored parameters will be cleared when the ps_iface goes DOWN.

PARAMETERS
  iface_ptr               : iface where filters are installed
  net_info_cfg_ptr        : Network configuration info ptr

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern int ps_iface_set_net_params
(
  ps_iface_type                   * iface_ptr,
  const network_params_info_type  * net_info_cfg_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_V4_DNS_ADDRS

DESCRIPTION
  This function sets the primary and secondary DNS addr's on the
  IPV4 iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  prim_dns_ptr:  input primary dns address
  sec_dns_ptr:   input secondary dns address

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_v4_dns_addrs
(
  ps_iface_type               *this_iface_ptr,
  const ps_ip_addr_type       *prim_dns_ptr,
  const ps_ip_addr_type       *sec_dns_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ENABLE_IFACE_LINGER()

DESCRIPTION
  This function enables linger on the interface

PARAMETERS
  this_iface_ptr     : ptr to interface control block on which to operate on.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_enable_iface_linger
(
  ps_iface_type  * this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_DISABLE_IFACE_LINGER()

DESCRIPTION
  This function disables linger on the interface

PARAMETERS
  this_iface_ptr     : ptr to interface control block on which to operate on.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_disable_iface_linger
(
  ps_iface_type  * this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_LINGER_TIMEOUT_VAL()

DESCRIPTION
  This function sets the value for linger timer

PARAMETERS
  this_iface_ptr     : ptr to interface control block on which to operate on.
  linger_timeout_val : timeout value

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_iface_set_linger_timeout_val
(
  ps_iface_type  * this_iface_ptr,
  const uint32     linger_timeout_val
);


/*===========================================================================
FUNCTION PS_IFACE_GET_LINGER_TIMEOUT_VAL()

DESCRIPTION
  This function fetches the value for linger timer

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  linger timeout value if successful
  0 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 ps_iface_get_linger_timeout_val
(
  ps_iface_type  * this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_SIP_SERV_ADDR

DESCRIPTION
  This function sets the SIP server addresses on an iface. This function
  overwrites the previously stored SIP addresses, if any. Additionally if
  this function is called with count value of 0 the SIP server list
  is cleared.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addr array.
  count : count of the SIP server addresses in the array.
  ps_errno : return error code.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_sip_serv_addr
(
  ps_iface_type      *this_iface_ptr,
  ps_ip_addr_type    *sip_serv_addr_ptr_array,
  uint32              count,
  errno_enum_type    *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_GET_SIP_SERV_ADDR

DESCRIPTION
  This function returns the list of sip server addresses stored in the iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addresses array.
  count : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_sip_serv_addr
(
  ps_iface_type      *this_iface_ptr,
  ip_addr_type       *sip_serv_addr_ptr_array,
  uint32             *count
);

/*===========================================================================
FUNCTION PS_IFACE_GET_SIP_SERV_ADDR_EX

DESCRIPTION
  This function returns the list of sip server addresses stored in the iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  sip_serv_addr_ptr_array: pointer to the SIP server addresses array.
  count : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  0 Success
 -1 Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_get_sip_serv_addr_ex
(
  ps_iface_type      *this_iface_ptr,
  ip_addr_type       *sip_serv_addr_ptr_array,
  uint32             *count
);

/*===========================================================================
FUNCTION PS_IFACE_SET_SIP_DOMAIN_NAMES

DESCRIPTION
  This function sets the SIP server domain names on an iface. This function
  overwrites the previously stored names, if any. If this function is
  called with a count value of 0 the SIP domain name list is cleared.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  sip_serv_domain_ptr_array: pointer to the SIP domain names array.
  count : count of the SIP server domain names in the array.
  ps_errno : return error code.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  It is expected that the caller of this function would pass the domain
  names in the form of NULL terminated string.

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_sip_domain_names
(
  ps_iface_type              *this_iface_ptr,
  ps_iface_domain_name_type  *sip_serv_domain_ptr_array,
  uint32                      count,
  errno_enum_type            *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_GET_SIP_DOMAIN_NAMES

DESCRIPTION

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  sip_serv_domain_ptr_array: pointer to an array of SIP domain names.
  count : count of the SIP server domain names in the array.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_sip_domain_names
(
  ps_iface_type              *this_iface_ptr,
  ps_iface_domain_name_type  *sip_serv_domain_ptr_array,
  uint32                     *count
);


/*===========================================================================
FUNCTION PS_IFACE_SET_DOMAIN_NAME_SEARCH_LIST

DESCRIPTION

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  dns_serv_domain_ptr_array: pointer to the DNS domain names array.
  count : count of the DNS server domain names in the array.
  ps_errno : return error code.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_domain_name_search_list
(
  ps_iface_type              *this_iface_ptr,
  ps_iface_domain_name_type  *domain_name_ptr_array,
  uint32                      count,
  errno_enum_type            *ps_errno
);


/*===========================================================================
FUNCTION PS_IFACE_GET_DOMAIN_NAME_SEARCH_LIST

DESCRIPTION

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  domain_name_ptr_array: pointer to an array of DNS domain names.
  count : count of the DNS server domain names in the array.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_domain_name_search_list
(
  ps_iface_type              *this_iface_ptr,
  ps_iface_domain_name_type  *domain_name_ptr_array,
  uint32                     *count
);

/*===========================================================================
FUNCTION PS_IFACE_FREE_SIP_ADDR_LIST

DESCRIPTION
  This function frees the SIP server address list.

PARAMETERS
  sip_addr_list : pointer to the link list containing the SIP addresses.

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_free_sip_addr_list
(
  list_type         *sip_addr_list
);

/*===========================================================================
FUNCTION PS_IFACE_FREE_DOMAIN_NAME_LIST

DESCRIPTION
  This function frees the domain name list which can be either SIP
  server domain name list or DNS domain name search list.

PARAMETERS
  domain_name_list : pointer to the link list containing
                         domain names.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_free_domain_name_list
(
  list_type         *domain_name_list
);

/*===========================================================================
FUNCTION PS_IFACE_SET_OPERATOR_RESERVED_PCO

DESCRIPTION
  This function sets the Operator reserved PCO information on an iface. This function
  overwrites the previously stored  PCO information, if any. 

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  operator_reserved_pco_info_ptr: pointer to the operator reserved PCO info
  ps_errno : return error code.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_operator_reserved_pco
(
  ps_iface_type                     *this_iface_ptr,
  ps_iface_op_reserved_pco_type     *operator_reserved_pco_info_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_OPERATOR_RESERVED_PCO

DESCRIPTION
  This function returns the Operator reserved PCO information in the iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  operator_reserved_pco_info_ptr: pointer to the operator reserved PCO info
 
RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_get_operator_reserved_pco
(
  ps_iface_type                        *this_iface_ptr,
  ps_iface_op_reserved_pco_type        *operator_reserved_pco_info_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_MODEM_RESERVED_PCO

DESCRIPTION
  Protocal Configuration option(PCO) has APN, PCO container ID and PCO message 
  string and used by the network for authentication purpose.
  This function sets the Modem reserved PCO information on an iface. This function
  overwrites the previously stored  PCO information, if any. The PCO information
  is saved on the iface by the modehandler after the PDN connectivity is established
  and the PCO is successfully negotiated.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  modem_reserved_pco_info_ptr: pointer to the modem reserved PCO info

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_modem_reserved_pco
(
  ps_iface_type                        * this_iface_ptr,
  ps_iface_modem_reserved_pco_type     * modem_reserved_pco_info_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_MODEM_RESERVED_PCO

DESCRIPTION
  This function returns the Modem reserved PCO information stored in the iface.
  The ATP application queries for the PCO information.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  operator_reserved_pco_info_ptr: pointer to the modem reserved PCO info
 
RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_get_modem_reserved_pco
(
  ps_iface_type                        * this_iface_ptr,
  ps_iface_modem_reserved_pco_type     * modem_reserved_pco_info_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_DHCP_STATELESS_CLIENT_HANDLE()

DESCRIPTION
  This function is used to set the dhcp_stateless_client_handle in ps_iface.
  NULL is passed to reset the handle.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.
  dhcp_client_handle : handle to set

RETURN VALUE
  0 if sucessful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_dhcp_stateless_client_handle
(
  ps_iface_type  *this_iface_ptr,
  void           *dhcp_client_handle
);


/*===========================================================================
FUNCTION PS_IFACE_GET_DHCP_STATELESS_CLIENT_HANDLE()

DESCRIPTION
  This function is used to get the dhcp_stateless_client_handle in ps_iface.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  The dhcp client handle, or NULL if the iface is invalid.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void * ps_iface_get_dhcp_stateless_client_handle
(
  ps_iface_type  *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_SET_DHCP_STATEFUL_CLIENT_HANDLE()

DESCRIPTION
  This function is used to set the dhcp_stateful_client_handle in ps_iface.
  NULL is passed to reset the handle.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.
  dhcp_client_handle : handle to set

RETURN VALUE
  0 if sucessful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_dhcp_stateful_client_handle
(
  ps_iface_type  *this_iface_ptr,
  void           *dhcp_client_handle
);


/*===========================================================================
FUNCTION PS_IFACE_GET_DHCP_STATEFUL_CLIENT_HANDLE()

DESCRIPTION
  This function is used to get the dhcp_stateful_client_handle in ps_iface.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  The dhcp client handle, or NULL if the iface is invalid.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void * ps_iface_get_dhcp_stateful_client_handle
(
  ps_iface_type  *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_DHCP6_SERVER_HANDLE()

DESCRIPTION
  This macro is used to set the dhcp6_server_handle in ps_iface. NULL is
  passed to reset the handle.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.
  dhcp6_server_handle : handle to set

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_SET_DHCP6_SERVER_HANDLE(this_iface_ptr, handle)           \
  if (PS_IFACE_IS_VALID(this_iface_ptr))                                   \
  {                                                                        \
    (this_iface_ptr)->iface_private.dhcp6_server_handle = (handle);        \
  }

/*===========================================================================
FUNCTION PS_IFACE_GET_DHCP6_SERVER_HANDLE()

DESCRIPTION
  This MACRO is used to get the dhcp6_server_handle in ps_iface.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  The dhcp6 server handle, or NULL if the iface is invalid.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_DHCP6_SERVER_HANDLE(this_iface_ptr)                   \
  (PS_IFACE_IS_VALID(this_iface_ptr)                                       \
   ? ((this_iface_ptr)->iface_private.dhcp6_server_handle)                  \
   : NULL)

/*===========================================================================
FUNCTION PS_IFACE_SET_IS_SHAREABLE()

DESCRIPTION
  This macro sets the is_shareable flag for an iface.

PARAMETERS
  this_iface: pointer to the interface in question.
  value     : booelan value of is_shareable to be set.

RETURN VALUE
  None.
===========================================================================*/
#define PS_IFACE_SET_IS_SHAREABLE(this_iface_ptr, value)                   \
  if (PS_IFACE_IS_VALID(this_iface_ptr))                                   \
  {                                                                        \
    (this_iface_ptr)->iface_private.is_shareable = (value);                \
  }

/*===========================================================================
FUNCTION PS_IFACE_GET_IS_SHAREABLE()

DESCRIPTION
  This macro gets the is_shareable flag for an iface.

PARAMETERS
  this_iface: pointer to the interface in question.

RETURN VALUE
  Value of is_shareable flag.
===========================================================================*/
#define PS_IFACE_GET_IS_SHAREABLE(this_iface_ptr)                          \
  (PS_IFACE_IS_VALID(this_iface_ptr)                                       \
    ? (this_iface_ptr)->iface_private.is_shareable                         \
    : FALSE)

/*===========================================================================
MACRO PS_IFACE_GET_RM_INST_HANDLE()

DESCRIPTION
  This macro retrieves the rmnet instance from the iface.

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_GET_RM_INST_HANDLE( this_iface_ptr )                      \
  (PS_IFACE_IS_VALID(this_iface_ptr)                                       \
    ? (this_iface_ptr)->iface_private.rm_inst_handle                       \
    : -1)

/*===========================================================================
FUNCTION PS_IFACE_MCAST_GET_GROUP_LIST

DESCRIPTION
  This function gets pointer to pointer to first group in the list of
  multicast groups associated with the given interface.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  double pointer to first group in the list on success and NULL on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
struct ps_mcast_cb ** ps_iface_mcast_get_group_list
(
  ps_iface_type  * this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_MCAST_GET_QUERIER_V1_TIMER

DESCRIPTION
  This function gets the current value of the querier V1 timer associated
  with the interface.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  Value of the querier V1 timer

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_timer_handle_type ps_iface_mcast_get_querier_v1_timer
(
  ps_iface_type  * this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ENABLE_MULTICAST

DESCRIPTION
  This function allocates the querier V1 timer associated
  with the interface.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_enable_multicast
(
  ps_iface_type  * this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_DISABLE_MULTICAST

DESCRIPTION
  This function cancels and frees the querier V1 timer associated with the
  interface. This function also leaves all the multicast groups that the
  interface is a member of.

PARAMETERS
  this_iface_ptr : ptr to interface control block on which to operate on.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_disable_multicast
(
  ps_iface_type  * this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADD_STATIC_NAT_ENTRY

DESCRIPTION
  This function add a static nat entry

PARAMETERS
  entry_ptr: ptr to the NAT entry to be added

RETURN VALUE
   0 if successful
   -1 if fails

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
int ps_iface_add_static_nat_entry
(
  ps_iface_ioctl_add_static_nat_entry_type * entry_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_DELETE_STATIC_NAT_ENTRY

DESCRIPTION
  This function delete a static nat entry

PARAMETERS
  entry_ptr: ptr to the NAT entry to be deleted

RETURN VALUE
   0 if successful
   -1 if fails

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
int ps_iface_delete_static_nat_entry
(
  ps_iface_ioctl_delete_static_nat_entry_type * entry_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_STATIC_NAT_ENTRY

DESCRIPTION
  This function get static nat entries

PARAMETERS
  entry_ptr: ptr to the NAT entries to be returned

RETURN VALUE
   0 if successful
   -1 if fails

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
int ps_iface_get_static_nat_entry
(
  ps_iface_ioctl_get_static_nat_entry_type * entry_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_DYNAMIC_NAT_ENTRY_TIMEOUT

DESCRIPTION
  This function get dynamic NAT entry timeout

PARAMETERS
  timeout_ptr: ptr to the timeout

RETURN VALUE
   0 if successful
   -1 if fails

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
int ps_iface_get_dynamic_nat_entry_timeout
(
  ps_iface_ioctl_get_dynamic_nat_entry_timeout_type * timeout_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_DYNAMIC_NAT_ENTRY_TIMEOUT

DESCRIPTION
  This function set dynamic NAT entry timeout

PARAMETERS
  timeout_ptr: ptr to the timeout

RETURN VALUE
   0 if successful
   -1 if fails

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
int ps_iface_set_dynamic_nat_entry_timeout
(
  ps_iface_ioctl_set_dynamic_nat_entry_timeout_type * timeout_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_MTU

DESCRIPTION
  This function gets the MTU of the iface. If the IFACE is logical,
  it gets the MTU from the base iface.

PARAMETERS
  iface_ptr : Target iface ptr

RETURN VALUE
  mtu : MTU of the iface
  0   : if iface is invalid

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 ps_iface_get_mtu
(
  ps_iface_type      *iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_MTU

DESCRIPTION
  This function sets the MTU of the iface. MTU is always capped at minimum
  possibe MTU defined in RFCs. For V4, it is 576 and for V6, it is 1280.

PARAMETERS
  iface_ptr : Target iface ptr
  new_mtu   : MTU to set

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_set_mtu
(
  ps_iface_type  * iface_ptr,
  uint16           new_mtu
);

/**
  @brief Retrieve the iface from event handle. 
 
  This method retrieves the iface pointer from event handle
  
  @param
    handle[in]    event handle
 
  @return
    iface pointer  on success
    0              on failure
  
  @dependencies 
    None
*/
ps_iface_type * ps_iface_get_iface_from_event_handle
(
  ps_iface_event_handle_type event_handle
);


/**
  @brief Util function to print ip config details (IP addr, DNS
         addr, SIP addr)
 
  @param 
    ps_iface_ptr[in]         Iface related to the IP cfg
                             container
    iface_ip_config_ptr[in]  IP config container
 
  @return
     None
  
  @dependencies 
    None
*/
void ps_iface_log_ip_config
(
  ps_iface_type           *ps_iface_ptr,
  ps_iface_ip_config_type *iface_ip_config_ptr
);

/**
  @brief copies ip config details (IP addr, DNS addr, SIP addr)
         into the ip config container from the given iface
 
  @param 
    src_iface_ptr[in]         Source Iface from which IP details
                              need to be retrieved
    iface_ip_config_ptr[out]  IP config container
 
  @return
     0 on successful copy of IP config
    -1 on failure
  
  @dependencies 
    None
*/
int ps_iface_get_ip_config
(
  ps_iface_type           *ps_iface_ptr, 
  ps_iface_ip_config_type *iface_ip_config_ptr
);

/**
  @brief Sets ip config details (IP addr, DNS addr, SIP addr) to
         the given iface
 
  @param 
    dst_iface_ptr[in]         ptr to destination iface
    iface_ip_config_ptr[in]   container that contains ip config
                              details
   
  @return
     0 on successful
    -1 on failure
  
  @dependencies 
    None
*/
int ps_iface_set_ip_config
(
  ps_iface_type           *ps_iface_ptr,
  ps_iface_ip_config_type *iface_ip_config_ptr
);

/**
  @brief Find the root/logical Iface for a given array of Ifaces
 
  @param 
    logical_iface_arr_ptr[out]  output array of Ifaces
    num_logical_iface[out]      num of Ifaces returned
    iface_arr_ptr[in]           input array of Ifaces
    num_iface[in]               num of input Ifaces
   
  @return
     0 on successful
    -1 on failure
  
  @dependencies 
    None
*/
int ps_iface_get_root_iface
(
  ps_iface_type **logical_iface_arr_ptr,
  uint8          *num_logical_iface,
  ps_iface_type **iface_arr_ptr,
  uint8           num_iface
);


/**
  @brief IWLAN mode handler calls this function to set the MAC address
         The MAC adddress is saved in the interface structure as well as 
         the technology specific configuration cache.
 
  @param 
    ps_iface_ptr[in]            input iface
    subs_id[in]                 subscription id
    mac_addr[in]                mac address
   
  @return
     0 on success
    -1 on failure
  
  @dependencies 
    None
*/
int ps_iface_set_wlan_mac_addr
(
  ps_iface_type                  * ps_iface_ptr,
  ds_sys_subscription_enum_type    subs_id,
  ds_sys_wlan_mac_addr_type      * mac_addr_ptr  
);


/*===========================================================================
MACRO PS_IFACE_POLICY_IS_CALL_FROM_TETHERED_HOST()

DESCRIPTION
  This macro returns true if policy ptr is bound to tethered host.

PARAMETERS
  policy_info_ptr  : acl policy ptr

RETURN VALUE
  TRUE  : acl policy is bound to tethered host
  FALSE : acl policy is not bound to tethered host

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_POLICY_IS_CALL_FROM_TETHERED_HOST  ps_iface_policy_is_call_from_tethered_host
INLINE boolean ps_iface_policy_is_call_from_tethered_host
(
  acl_policy_info_type * policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((PS_IFACE_PROC_ID_TE_RMNET == policy_info_ptr->proc_id) ||
      (PS_IFACE_PROC_ID_TE_DUN == policy_info_ptr->proc_id))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#ifdef __cplusplus
}
#endif

#endif /* PS_IFACE_H */
