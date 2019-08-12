/*===========================================================================

                     D S _ R M N E T _ M E T A _ S M . C
DESCRIPTION

  The Data Services Rm Network interface State Machine source file.

  This state machine manages the Rm network interface and coordinates
  wireless WAN connectivity between the Rm and Um interfaces over all
  supported radio technologies.

EXTERNALIZED FUNCTIONS

  rmnet_meta_sm_init()
    Allocate and initialize all data required for the RmNet State Machine.

  rmnet_meta_sm_post_event()
    Post an event to RmNet State Machine.

  rmnet_meta_sm_process_ev_cmd()
    Process a cmd posted to Rm State Machine.

  rmnet_meta_sm_in_call()
    Returns true if in a packet call.

  rmnet_meta_sm_iface()
    Returns the ps_iface controlled by the given RMNET RmSM instance

  rmnet_meta_sm_set_network_cfg_params()
    This function is called to set the parameters received in Start Network
    Interface for network configuration in the rmnet info structure.

  rmnet_meta_sm_get_um_iface()
     Returns the UM ps_iface ptr for the given RmSM instance

  rmnet_meta_sm_is_auto_connect_enabled()
    Returns whether auto_connect is enabled on this instance or not

  rmnet_meta_sm_set_autoconnect_delay_timer()
    Delays the auto-connect timer

Copyright (c) 2004-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_meta_sm.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/16/15    wc     Fix packet filtering issue
06/25/14    wc     Enable QMAPv3
05/09/14    wc     Autoconnect IPv6 fix
04/09/14    ash    Added Prefix Delegation support.
03/17/14    vrk    CM SS events handling for SGLTE+G CM redesign
03/14/14    wc     MBIM flow control
02/28/14    wc     Support $QCRMCALL
01/21/14    wc     Ethernet mode changes
12/30/13    ssb    Deprecate prefix is handled in Um iface teardown
12/20/13    vrk    Removed assert(0) in rmnet_iface_ioctl_hdlr()
11/01/13    rk     Removed feature mobileap.
10/21/13    sah    Corrected handling of variable, autoconnect_next_backoff_time 
10/03/13    ms     Dont allow new emb call if arbitration in progress already
                   for another Rmnet instance with same Um iface ptr.
09/27/13    wc     Support dynamic port configuration
09/06/13    sah    Fix for KW Errors
08/12/13    bh     Fixed incorrect setting of proc id
07/31/13    vrk    Fixed KW Error
07/12/13    cx     Change the ps_icmp6_get_dest_mac_addr argument due to memcpy replacement
06/04/13    ash    Make logical iface as input to get_ipv6_ext_addr 
06/10/13    pgm    Added flow control for Legacy Wmk.
05/30/13    wc     Non-MBIM packet filtering
04/17/13    wc     Reject unrecognized data format
02/04/13    svj    Changes to call new API ps_route_tear_down instead
                   of ps_iface_tear_down_cmd
03/21/13    wc     QMAP support
01/22/12    svj    Remove autoconnect NV writes for default values
12/10/12    svj    Support 8 concurrent PDN's
12/06/12    at     Fix for the USB TE-A5 data path issue in ETHERNET mode.
12/04/12    at     Reset eth_arped for RNDIS mode upon NULL state
11/30/12    wc     Defer resetting of data format on DTR low
10/26/12    wc     Enable NAT for every data call if configured in EFS
10/31/12    wc     Memory optimization
10/23/12    svj    Fix to disable aggregation on non A2 ports
10/23/12    gk     Disabled Data Aggregation
10/09/12    sj     8 PDN support changes
09/29/12    tw     Deprecate old IPv6 prefix
09/19/12    wc     Disable data aggregation except MBIM for FUSION 3
09/12/12    sb     Move IPv6 Rmnet state to be routable in SoftAP mode even
                   in the absence of RA.
08/24/12    gk     Fixed v4 and v6 calls getting out of sync issue, when a
                   second set_data_format was sent and v6 call is up
09/06/12    pgm    Support for different APNS in Dual-IP.
09/05/12    wc     Fix DTR race condition
08/01/12    pgm    Outstanding mbim_pkt_info handling to avoid race condition
                   b/w DCC and PS_RM task.
07/31/12    sb     Fixed Compiler Warning.
07/26/12    pgm    Fix to set default ndp sig in QCNCM/MBIM mode.
06/20/12    sa     Setting appropriate data aggregation modes.
07/11/12    pgm    Added support for RNDIS agg mode.
07/10/12    pgm    Fix for routing packets to the right PDN in a multi-PDN
                   call after flow is enabled.
07/06/12    wc     Support QCMAP rmnet EFS configuration
06/14/12    sj     Reset eth_arped flag to FALSE when call goes down so that
                   eth_header values can be repopulated during next call
06/07/12    wc     Fix writing of wrong rmnet_config.txt
06/06/12    sb     Fix to gracefully handle attempts to connect on more than 5 pdn's.
05/14/12    sb     Fixed KW Warnings.
05/08/12    sn     Updating Rmnet data format client state when DTR is High.
05/07/12    sa     Fix to store correct qmi index from rmnet instance.
04/18/12    sn     Enabling Tx flow when a call is being brought up. This
                   resets Tx flow state if it is disabled in previous call.
04/12/2012  pgm    Moving Legacy path processing to PS task from PS_RM to
                   avoid race condition issues.
04/05/12    pgm    Reset A2 mode when setting QC_NCM/MBIM mode as A2 could be
                   in TLP mode earlier.
03/14/12    wc     Added get_proc_id_from_inst routine
03/13/12    wc     Use iface handle to determine if two um ifaces are the same
03/09/12    sn     Fixed instance used as index for autoconnect setting nv.
01/03/12    sa     Changes for Set/Get autoconnect and updating efs.
02/26/12    pgm    Added PS_IFACE_COMMON_DEFAULT_IS_FLOW_ENABLED for flow
                   control check in rmnet_meta_smi_rx_ready.
02/22/12    wc     Enable EFS to configure proc_id
02/21/12    wc     Mainline eMBMS
02/17/12    sb     Fixed Compiler and KW Warnings.
02/02/12    cy     Added support for MBIM NTB parameters
02/10/12    rt     Added support for SoftAPv6
01/12/12    wc     Call a2 tlp functions only for a2 port
12/27/11    wc     De-featurize FEATURE_DATA_FUSION_PS_ARBITRATION_MGR
12/19/11    wc     Fix rmnet dual-ip signal issue
12/06/11    wc     Reset arb_close_in_progress flag when rm iface is down
11/02/11    wc     Add DTR on SIO control channel feature
11/01/11    am     Added 9x15 SoftAP support.
10/13/11    wc     Increase RMNET_RX_WM_DNE
10/04/11    sa     Q6 free floating changes for QMI.
09/22/11    am     Added DL MBIM support.
09/22/11    pgm    Added MBIM sig hdlr registration for MBIM mode.
09/18/11    wc     Nikel and MPSS.NI.1.1 port configuration
09/07/11    pgm    Added check for sizeof(rmnet_meta_smi_qos_hdr_type) to be
                   even as a part of UL optimization.
09/07/11    pgm    Added support for sig handlers for UL optimization.
09/02/11    wc     Add check to prevent autoconnect from starting if port
                   is not active.
08/18/11    sa     Sending No Effect as return value when the call is
                   already up and we try to set data format.
08/10/11    wc     Data aggregation based on CPU utilization
08/01/11    wc     Print packet type in F3 when dropping packet
07/26/11    ua     Fixed Data Call status indication for Tethered calls.
07/21/11    wc     Reject SNI if Um iface is already used by another RmNet
                   instance.
06/24/11    asn    Support for new Iface generic indication Active Out of Use
06/30/11    ua     Fixing RDM, DCC race condition due to with
                   rmnet_dual_smi_info is not getting initalized which is
                   required at RmNet init.
01/11/11    kk     Added support for data_call_type TLV in data_call_status
                   indication.
06/15/11    sa     Fixed compiler warning.
05/30/11    kk     Fix to prevent PS/DCC race condition in ETH mode.
05/17/11    kk     Adding support for DPL logging on upto ten RmNet ifaces.
05/17/11    sy     Fix to set phys link ptr in rmnet cache info to NULL after
                   de-registering from physlink events.
05/02/11    ua     Enabling Autoconnect to working with first instance across
                   all targets.
05/02/11    ua     Allowing Autoconnect with NV 3534 also. Only IPV4 is
                   supported with this NV autoconnect.
02/14/2011  ua     Added support for DUAL IP Autoconnect.
04/08/11    kk     Re-merging support for DOS flag in QoS header.
03/09/11    kk     Removing DCC command buffer dependency from RmNet.
02/07/11    kk     Fixed LINT critical errors.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free
                   Floating environment.
12/14/10    ash    Delete IPv6 Addresses when IPv6 call goes down.
07/16/10    asn    Support for Arbitration - round II
07/01/10    asn    Support for Arbitration
06/19/10    vs     Changes for Dual IP support over single QMI instance
06/10/10    kk     Added supported for DOS flag in QoS header.
10/23/09    ar     Moved RDM coupling for port management to QMUX
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
02/12/09    rt     Removed assert for configuration failure event in GOING
                   DOWN state.
09/21/08    pp     Metainfo optimizations.
06/18/08    mct    Fixed a corner case where call could tear down at the exact
                   time addresses are assigned.
04/01/08    mct    Added IP mode support.
10/01/07    mct    Rearchitected RmNet to support multiple interface types.
                   Added IPv6 support.
07/19/07    ks     Reset autoconnect_timer back to MIN when DTR drops.
                   Don't report failure to DCTM if DTR drops.
07/18/07    ks     Don't report DCTM failure for UM iface down as DS mode
                   handler already reports it.
06/21/07    ks     added state to track whether rmnet is registered for
                   default flow enabled/disabled event, to avoid calling
                   reg/dereg apis back to back.
05/28/07    ks     added autoconnect backoff timer retry mechanism.
05/28/07   jd/ks   Post a cmd to DS when tlp_change_cb occurs.
04/24/07    ks     Added check for flow enabled in rmnet_meta_smi_rm_rx_data().
02/22/06    ks     Handling DHCP_COMPLETE_EV in UP_STATE.
02/06/07    scb    Fixed Klockwork Critical Error in rmnet_meta_smi_set_mac()
01/08/07    jd     Added Winmobile multiple RmNet support
12/05/06    jd     Drop packets bigger than 1500 bytes rather than asserting
11/29/06    jd     Added DCTM support to RmNet
11/13/06    ks     Added support for MULTIPLE_NAI feature in rmnet.
10/09/06    ks     Added rmnet_meta_sm_process_cmd to process DS rmnet cmds.
10/09/06    ks     Reverted previous change.
10/09/06    ks     Added rmnet_meta_sm_process_cmd to process DS rmnet cmds.
                   Removed asserts during init to handle wrong nv item values
09/21/06    ks     Clear rx and tx wmks when the call ends.
09/13/06    jd     Changes to use PS extended signal mask
08/23/06    ks     Changes to support multiple rmnet instances. Added
                   rmnet_meta_sm_is_auto_connect_enabled().
07/20/06    ks     Added check for dhcp_handle before calling dhcp_stop and
                   arp_flush.
07/17/06    ks     Removed unnecessary rx and tx wmks from rmnet_info struct.
05/24/06    ks     Set bring_up to true in acl_policy as route_acl post
                   processing looks at bring_up flag to set iface in use.
03/13/06    jd     Removed notifications (rmnet_meta_sm_reg_notification)
03/07/06    jd     Added support for TLP
03/06/06    ks     Posting RMNET_META_SM_RM_DEV_START_PKT_EV if DTR is high in
                   NULL state. Added rmnet_meta_sm_get_um_iface().
01/25/06    ks     Initialize profile number and tech pref in net_params
                   struct in rmnet_meta_smi_info_type to default values for
                   autoconnect.
01/09/06    jd     Added support for bringing up 802.11 interface
12/02/05    ks     Added support for Profiles.
10/31/05    ks     Stop DHCP when RMNET SM transitions to NULL from UP state.
10/31/05    ks     Register for phys_link flow enable/disable events. Added
                   check for phys_link flow control in rmnet_meta_smi_rm_rx_data().
10/24/05    ks     Fixed pdp profile number for UMTS to use the ATCOP family.
10/18/05    ks     Checking if USB current device is NETWORK_DEVICE before
                   initializing rmnet instance in rmnet_meta_sm_init().
10/10/05    ks     Removed SIO DATA WWAN RDM device.
09/13/05    jd     Added support for starting UMTS calls
09/13/05    jd/ks  Added rmnet_meta_sm_process_dtr_changed_cmd().
09/09/05    jd     Added rmnet_meta_sm_set_auth_creds() to set the rmnet_meta_sm's
                   ps_iface auth settings.  Check NV for autoconnect setting.
09/08/05    ks     Changed qmi_ws_if to qmi_if.
06/17/05    jd     Fixed assert on UM_ADDR_CHANGE event during call setup
05/31/05    jd     Code review updates
05/17/05    jd     Moved initialization of QMI services to QMUX module
05/11/05    ks     Some clean up.
04/26/05    jd     ARP proxy check compares IP addr's in network order
04/19/05    jd     Set RmNet ps iface as broadcast interface
03/15/05    ks     Added support for Ethernet Logging.
03/14/05    ks     Removed featurization
03/08/05    ks     Clean up and changed name from wwan_rmsm.c
11/22/04    hba    Integrate LLC Mode 0 code...
10/27/04    jd     Added wwan_rmsm_iface() to return current ps_iface in use.
                   Fixed some missing taskfree's in Rm_start_pkt.
                   Generate Rm_start_pkt result notifications for WWAN start
                   packet session result.
                   Featurized auto-call connection (later: make runtime cfg)
                   Send media disconnect, unbridge Um&Rm after call teardown.
07/12/04    jd     Formalized wwani_rmsm_set_mac() API to set Rm MAC address.
                   Activate context after SIOlib is set to PKT mode -
                   ensure that data path is configured before data is sent.
05/12/04    jd     Added call startup result notification callback
                   Added rm_dev_end_pkt_event handlign (same as rm_link_down)
                   Renamed  wwan_rmsm_is_packet_call() as wwan_rmsm_in_call()
04/01/04    jd     Added wwan_rmsm_is_packet_call()
02/06/04    jd     Initial implementation
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "event_defs.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

/* use dsm watermark callback with arguments - define here until other
   dsm users update their code so we can mainline this feature -
   no problem with compilation since 2 args are placed in R0, R1 on arm,
   thus are overwritten/ignored where feature isn't defined.
   (no stack overflows) */
#define FEATURE_DSM_WM_CB

#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "ps_crit_sect.h"
#include "queue.h"
#include "dsm.h"

#include "ps_mem.h"
#include "ps_metai_info.h"
#include "ps_iface_defs.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps_ifacei_event.h"
#include "ps_route.h"
#include "ps_route_iface.h"
#include "ps_flow_event.h"
#include "ps_flow.h"

#include "ps_iface_logging.h"
#include "ps_logging_defs.h"
#include "secutil.h"
#include "ran.h"
#include "secerrno.h"
#include "ds_flow_control.h"

#include "dcc_task_defs.h"
#include "dcc_task_svc.h"

#include "ds_rmnet_sm_ext.h"
#include "ds_rmnet_smi.h"
#include "ds_rmnet_xport.h"
#include "ds_rmnet_sio_rt_acl.h"
#include "ds_rmnet_defs.h"
#include "ds_rmnet_utils.h"

#include "ds_qmux.h"
#include "ds_qmi_defs.h"
#include "ds_qmi_ctl.h"
#include "ds_qmi_svc.h"
#include "dsati.h"
/* need to remove this dependency added for profile support */
#include "ds_qmi_wds.h"

#ifdef FEATURE_HS_USB
  #include "hsu_config_selector.h"
#endif
#include "rdevmap.h"

#include "ps_iface_ioctl.h"

#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#endif /* FEATURE_QXDM_WM_TRACING */

#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_3gpp2.h"

#include "ds_qmi_cflog.h"

#include "ps_arp.h"

#include "ps_icmp6_nd.h"
#include "ps_ifacei_addr_v6.h"
#include "ps_ip4_hdr.h"

#include "ps_stat_iface.h"
#include "ds_qmi_nv_util.h"

#include "ps_policyi_mgr.h"
#include "ds_Utils_DebugMsg.h"
#include "dssocket.h"

#ifdef FEATURE_DATA_PS_ADDR_MGMT
#include "ps_ifacei_addr_mgmt.h"
#endif /* FEATURE_DATA_PS_ADDR_MGMT */

#include "ps_system_heap.h"
#include "ps_ifacei_utils.h"

#include "ps_rm_defs.h"
#include "ps_rm_svc.h"

#include <stringl/stringl.h>
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_sys_types.h"

#if defined(FEATURE_RMNET_DATA_AGG_TIMER) && defined(FEATURE_CPU_BASED_FLOW_CONTROL)
#include "fc.h"
#include "tmc.h"
#endif /* FEATURE_RMNET_DATA_AGG_TIMER && FEATURE_CPU_BASED_FLOW_CONTROL */

#include "ps_wmk_logging.h"
#include "ps_sys.h"
#include "ds_qmap.h"

#include "ps_dpm.h"

#include "ds3gmgr.h"

#include "dsmip_api.h"
/*===========================================================================

                                EXTERNAL DECLARATIONS

===========================================================================*/
extern ps_iface_type * ps_route_look_up_iface_by_policy
(
  acl_policy_info_type          * acl_pi_ptr,
  boolean                       * arb_candidate,
  uint32                        * iface_priority_mask,
  ps_iface_net_down_reason_type * net_down_reason
);

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*===========================================================================

                               DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Serving system changed bitmask
---------------------------------------------------------------------------*/
#define RMNET_META_SM_CM_SRV_SYS_CHANGED_MASK (0x00                   |   \
                                               CM_SS_SRV_STATUS_MASK  |   \
                                               CM_SS_ROAM_STATUS_MASK |   \
                                               CM_SS_HDR_ROAM_STATUS_MASK | \
                                               CM_SS_HDR_SRV_STATUS_MASK )


/*---------------------------------------------------------------------------
  RMNET_META_SM state management and event processing functions
---------------------------------------------------------------------------*/
#define AUTOCONNECT_BACKOFF_TIMER_MIN_VALUE  1
#define AUTOCONNECT_BACKOFF_TIMER_MAX_VALUE  120

/* Maximum number of active Ifaces */
#define RMNET_IFACE_MAX (PS_SYS_MAX_AVAIL_PDNS * 2)

typedef enum {
  RMNET_QOS_FLAG_TYPE_NO_FLAGS  = 0x0,
  RMNET_QOS_FLAG_TYPE_DOS       = 0x1,
  RMNET_QOS_FLAG_TYPE_ALL_FLAGS = 0xFF
} rmnet_qos_flag_e_type;

static dpl_iid_ifname_enum_type dpl_ifname_by_rmnet_instance[RMNET_IFACE_MAX];

ps_crit_sect_type rmnet_crit_section;
ps_crit_sect_type rmnet_rx_sig_crit_section;
ps_crit_sect_type rmnet_rx_pkt_hdlr_crit_section;

rex_crit_sect_type lan_llc_inst_to_um_iface_ptr_crit_sec;

/*---------------------------------------------------------------------
  UINT64 Application IDs for Arbitration. Temporary position.
---------------------------------------------------------------------*/
#define APP_ID_TE_DUN    0x0000000100000000ULL
#define APP_ID_APPS      0x0000000200000000ULL
#define APP_ID_TE_RMNET  0x0000000300000000ULL

/*---------------------------------------------------------------------
  Datastructure to hold mapping between sio port to proc id
---------------------------------------------------------------------*/
typedef struct rmnet_sio_port_to_proc_id_map_s
{
  sio_port_id_type sio_port;
  uint32           proc_id;
} rmnet_sio_port_to_proc_id_map_type;

/*------------------------------------------------------------------
      EFS Configuration and parsing definitions
 -------------------------------------------------------------------*/
#define RMNET_EFS_INVALID_PROFILE     0xFF
#define RMNET_EFS_AUTOCONNECT_INVALID 0xFF


extern lan_lle_cb_type  lle_array[LAN_LLE_MAX];


rmnet_efs_global_config_s_type rmnet_efs_global_config_params;

#define RMNET_AUTOCONNECT_MAX  8 /* Changing this requires change of NV */
static rmnet_efs_autoconnect_s_type rmnet_efs_autoconnect_config[RMNET_AUTOCONNECT_MAX];

/*------------------------------------------------------------------
   RMNET $QCRMCALL
-------------------------------------------------------------------*/
typedef enum
{
  RMNET_QCRMCALL_STATE_NONE   = 0,
  RMNET_QCRMCALL_STATE_START  = 1,
  RMNET_QCRMCALL_STATE_UP     = 2,
  RMNET_QCRMCALL_STATE_STOP   = 3
} rmnet_qcrmcall_state_enum;

typedef struct rmnet_qcrmcall_inst_s
{
  uint32                     ep_id;
  uint8                      mux_id;
  rmnet_qcrmcall_state_enum  v4_state;
  rmnet_qcrmcall_state_enum  v6_state; 
  rmnet_qcrmcall_cb_type     v4_cb;
  rmnet_qcrmcall_cb_type     v6_cb;
} rmnet_qcrmcall_inst_type;

static rmnet_qcrmcall_inst_type rmnet_qcrmcall_inst[RMNET_QCRMCALL_MAX];

static void rmnet_meta_smi_process_qcrmcall_cmd
(
  rmnet_qcrmcall_action_e_type    action,
  rmnet_qcrmcall_request_type   * request,
  rmnet_qcrmcall_cb_type          callback
);

static void rmnet_meta_smi_update_qcrmcall_state
(
  rmnet_smi_info_type * info
);

/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/

static boolean rmnet_meta_smi_init
(
  rmnet_instance_e_type  instance
);

static boolean rmnet_meta_smi_iface_init
(
  uint8 i
);

static boolean rmnet_meta_smi_reg_events
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_dereg_events
(
  rmnet_smi_info_type *  info
);

static boolean rmnet_meta_smi_reg_physlink_events
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_dereg_physlink_events
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_process_dtr_changed_cmd
(
  rmnet_smi_info_type  * info,
  boolean                dtr_asserted
);

static void rmnet_meta_smi_process_phys_link_up_down_event
(
  ps_phys_link_type            *this_phys_link_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_data_ptr
);

static int rmnet_meta_smi_start_addr_cfg_sm
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_stop_addr_cfg_sm
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_retrieve_default_profile_num
(
  ds_profile_tech_etype       profile_type,
  uint8                       profile_family,
  uint16                    * default_profile_num
);

/*---------------------------------------------------------------------------
  RMNET_META_SM state management and event processing functions
---------------------------------------------------------------------------*/

static void rmnet_meta_smi_transition_state
(
  rmnet_smi_info_type       * info,
  rmnet_meta_smi_state_type   new_state              /* state to tranistion to */
);

static void rmnet_meta_smi_handle_rm_dev_start_pkt_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_rm_link_down_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_um_iface_down_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_um_iface_up_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_addr_cfg_complete_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_addr_cfg_failure_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_addr_cfg_changed_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_um_iface_authenticating_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_um_iface_arb_ev
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_um_iface_bearer_tech_changed_ev
(
    rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_handle_ep_cfg_done_ev
(
    rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_update_physlink
(
  rmnet_instance_e_type  instance
);

static void rmnet_meta_smi_log_wmk_levels
(
   rmnet_instance_e_type            rmnet_instance,
   ps_phys_link_type               *um_phys_link_ptr,
   ps_wmk_logging_dorm_state_type   dormancy_status
);

/*---------------------------------------------------------------------------
  RMNET_META_SM callback functions registered with various external modules
---------------------------------------------------------------------------*/
static boolean rmnet_meta_smi_verify_dtr_high
(
  rmnet_smi_info_type   *info
);

static void rmnet_meta_smi_um_iface_ev_hdlr_cb
(
  ps_iface_type *this_iface_ptr,           /* Pointer to the interface     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

static void rmnet_meta_smi_um_phys_link_ev_hdlr_cb
(
  ps_phys_link_type *this_phys_link_ptr,   /* Pointer to the interface     */
  ps_iface_event_enum_type   event,        /* Event type occurred          */
  ps_iface_event_info_u_type event_info,   /* Event specific info block    */
  void *user_data_ptr           /* Data passed during event registration   */
);

static void rmnet_meta_smi_um_flow_ev_hdlr_cb
(
  ps_flow_type                *this_flow_ptr,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                        *user_data_ptr
);

static void rmnet_meta_smi_set_mac
(
  ps_iface_type *  iface,
  byte *           hw_addr,
  byte             hw_addr_len
);

#if defined(FEATURE_IP_CALL)
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
static boolean rmnet_meta_smi_oprt_mode_change_cb
(
  sys_oprt_mode_e_type  new_oprt_mode,
  cmipapp_trans_id_type trans_id
);
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
#endif /* defined(FEATURE_IP_CALL) */


/*---------------------------------------------------------------------------
  Physical link data transfer functions
---------------------------------------------------------------------------*/
static void rmnet_meta_smi_init_rndis_eth_hdr
(
  rmnet_smi_info_type *  info
);

static void rmnet_meta_smi_autoconnect_timer_cback
(
  timer_cb_data_type  user_data
);

static void rmnet_meta_smi_process_autoconnect_timer_cmd_hdlr
(
  void              *user_data
);

static void rmnet_meta_smi_update_autoconnect_roam_status
(
  void
);

static void rmnet_meta_smi_update_autoconnect_state
(
  rmnet_smi_info_type  * info
);

static void rmnet_meta_smi_cm_ss_event_cb
(
  cm_ss_event_e_type            ss_event,    /* SS event that just occured */
  const cm_mm_msim_ss_info_s_type *  ss_info_ptr  /* ptr to serving system info */
);

static void rmnet_meta_smi_process_cm_ss_event
(
  cm_ss_event_e_type      ss_event,
  rmnet_meta_sm_cm_ss_info_type  ss_info,
  sys_modem_as_id_e_type  asubs_id
);

static boolean rmnet_meta_smi_reg_cm
(
  void
);

#if defined(FEATURE_IP_CALL)
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
static void rmnet_meta_smi_cm_op_mode_change_cmd_hdlr
(
  rmnet_smi_cmd_type * cmd_ptr
);

static void rmnet_meta_smi_initialize_cmipapp_info
(
  void
);
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
#endif /* defined(FEATURE_IP_CALL) */


/*--------------------------------------------------------------------------
  IOCTL Handler
--------------------------------------------------------------------------*/

LOCAL int rmnet_iface_ioctl_hdlr
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
);

#ifdef FEATURE_QXDM_WM_TRACING
/*--------------------------------------------------------------------------
  RMNET callback function for DSM watermark reporting to FC
--------------------------------------------------------------------------*/
static void rmnet_meta_smi_fc_get_wm_trace_cb
(
  uint32            user_data,
  fc_wm_stat_type  *wm_stat_ptr
);
#endif /* FEATURE_QXDM_WM_TRACING */

/*--------------------------------------------------------------------------
  Utility routine to get proc id
--------------------------------------------------------------------------*/
static boolean rmnet_meta_smi_find_proc_id(
  uint32           *proc_id,
  rmnet_instance_e_type rmnet_inst
);

static boolean rmnet_um_iface_in_use(
  rmnet_smi_info_type  * info,
  boolean                check_arb
);

static int rmnet_tear_down_um_iface(
  rmnet_smi_info_type  * info
);

static boolean rmnet_set_rm_iface_addr_family
(
  ps_iface_type    * rm_iface_ptr,
  uint8              addr_family
);

/*===========================================================================

                     EFS Configuration and Parsing Function

===========================================================================*/
static void rmnet_meta_sm_get_efs_config_params
(
  void
);

/*---------------------------------------------------------------------------
  RMNET Data Aggregation
---------------------------------------------------------------------------*/
#if defined(FEATURE_RMNET_DATA_AGG_TIMER) && defined(FEATURE_CPU_BASED_FLOW_CONTROL)

uint8 rmnet_da_cpu_high = 60;  // CPU utilization high threshold
uint8 rmnet_da_cpu_low  = 40;  // CPU utilization low threshold

extern boolean rmnet_sio_da_enabled; // Is DA enabled (ds_rmnet_sio.c)
q_type rmnet_fc_cmd_q;

static boolean rmnet_meta_smi_fc_register
(
  void
);

static boolean rmnet_meta_sm_process_fc_sig
(
   dcc_sig_enum_type sig,
   void *user_data_ptr
);

#endif /* FEATURE_RMNET_DATA_AGG_TIMER && FEATURE_CPU_BASED_FLOW_CONTROL */

static void rmnet_meta_smi_ep_cfg_done_cb
(
  void  * user_info
);

static boolean rmnet_meta_smi_in_call_cb
(
  void  * user_info
);

static void rmnet_meta_smi_dtr_cb
(
  void   * user_info,
  boolean  dtr_asserted
);

static boolean rmnet_set_manual_dns_settings
(
  rmnet_smi_dual_ip_info_type *info
);

static boolean rmnet_meta_smi_set_default_net_params
(
  rmnet_smi_info_type  * info
);

static boolean rmnet_meta_smi_set_default_autoconn_params
(
  rmnet_smi_info_type  * info
);

static boolean rmnet_meta_smi_update_eth_header
(
  rmnet_smi_info_type  * info
);

static void rmnet_meta_smi_llc_tx_data
(
  dsm_item_type  ** pkt,
  void            * user_data
);

static boolean rmnet_meta_smi_qos_or_te_fc_enabled
(
  rmnet_smi_info_type  * info
);

static void rmnet_meta_smi_rx_hiwater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
);

static void rmnet_meta_smi_rx_lowater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
);

static void rmnet_meta_smi_tx_hiwater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
);

static void rmnet_meta_smi_tx_lowater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
);


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  RmNet state machine info block
---------------------------------------------------------------------------*/
rmnet_smi_info_type * rmnet_smi_info[RMNET_INSTANCE_MAX];

rmnet_smi_dual_ip_info_type * rmnet_smi_dual_ip_info[RMNET_DUAL_IP_MAX];

/* MAX of 16 Rmnet ifaces supported(8 for IPV4 and 8 for IPV6) */
static rmnet_smi_iface_info_type * rmnet_smi_iface_info[RMNET_IFACE_MAX];

/*---------------------------------------------------------------------------
  RmNet cm info block
---------------------------------------------------------------------------*/
static rmnet_smi_cm_info_type  rmnet_smi_cm_info;

static rmnet_smi_eri_roam_ind_info_type rmnet_smi_eri_roam_ind_info;

#if defined(FEATURE_IP_CALL)
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/*---------------------------------------------------------------------------
  CMIPAPP info for rmnet interface
---------------------------------------------------------------------------*/
static rmnet_smi_cmipapp_info_type rmnet_smi_cmipapp_info;
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
#endif /* defined(FEATURE_IP_CALL) */

/*---------------------------------------------------------------------------
  Function table registered with siolib to manage serial port
---------------------------------------------------------------------------*/
#ifndef T_ARM
/*---------------------------------------------------------------------------
  These names need to be kept in sync with the enums which they describe.
---------------------------------------------------------------------------*/
static char *rmnet_meta_smi_state_names[RMNET_META_SM_MAX_STATE] =
{
  "Null state",
  "Um Configuring",
  "Um up & Rm configuring",
  "Um & Rm up",
  "Rm Reconfiguring",
  "Waiting for Um to go down",
  "Waiting for Arb to complete",
  "EP Configuring"
};

/*
static char *rmnet_meta_smi_event_names[RMNET_META_SM_MAX_EV] =
{
  "Rm wants packet call",
  "Rm link layer is down",
  "Um interface is down",
  "Um interface is up",
  "Rm addr cfg complete",
  "Rm addr cfg failed",
  "Um Addr cfg changed",
  "Um interface is down arb"
};
*/
#endif /* T_ARM */

/*---------------------------------------------------------------------------
  RMNET RMSM command handler array
---------------------------------------------------------------------------*/
typedef void (*rmnet_meta_smi_cmd_hdlr_type)( rmnet_smi_info_type *);
static rmnet_meta_smi_cmd_hdlr_type rmnet_meta_smi_cmd_hdlr[RMNET_META_SM_MAX_EV] =
{
  rmnet_meta_smi_handle_rm_dev_start_pkt_ev,                        /* event 0 */
  rmnet_meta_smi_handle_rm_link_down_ev,                            /* event 1 */
  rmnet_meta_smi_handle_um_iface_down_ev,                           /* event 2 */
  rmnet_meta_smi_handle_um_iface_up_ev,                             /* event 3 */
  rmnet_meta_smi_handle_addr_cfg_complete_ev,                       /* event 4 */
  rmnet_meta_smi_handle_addr_cfg_failure_ev,                        /* event 5 */
  rmnet_meta_smi_handle_addr_cfg_changed_ev,                        /* event 6 */
  rmnet_meta_smi_handle_um_iface_authenticating_ev,                 /* event 7 */
  rmnet_meta_smi_handle_um_iface_arb_ev,                            /* event 8 */
  rmnet_meta_smi_handle_um_iface_bearer_tech_changed_ev,            /* event 9 */
  rmnet_meta_smi_handle_ep_cfg_done_ev,                             /* event 10 */
};

/*---------------------------------------------------------------------------
  Look up table to get the um_iface_ptr from LAN LLC instance
---------------------------------------------------------------------------*/
ps_iface_type * lan_llc_inst_to_um_iface_ptr_lut[LAN_LLE_MAX];


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_SM_INIT()

  DESCRIPTION
    This function initializes the RMNET state machine.
    Called from serving task's task init function.

  PARAMETERS
    None

  FUTURE:
    iface_ptr: Pointer to the interface which is used by this instance
               of Rm state machine to bring up pkt calls.

  RETURN VALUE
    Signals to be handled by RmNet

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_sm_init
(
  void
)
{
  uint8                  j;
  rmnet_smi_cmd_type   * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_INIT_CRIT_SECTION(&rmnet_crit_section);
  PS_INIT_CRIT_SECTION(&rmnet_rx_sig_crit_section);
  PS_INIT_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);

  rex_init_crit_sect(&lan_llc_inst_to_um_iface_ptr_crit_sec);

  /* Initialize dpl_ifname_by_rmnet_instance, rmnet_signals_table
      for max ifaces */
  for(j = 0; j < RMNET_IFACE_MAX; j++)
  {
    dpl_ifname_by_rmnet_instance[j] = DPL_IID_IFNAME_SIO_IFACE_RMNET_1 + j;
  }

  /* Register command handlers with DCC Task */
  (void) dcc_set_cmd_handler(DCC_RMNET_SM_CMD, rmnet_sm_process_cmd);
  (void) dcc_set_cmd_handler(DCC_RMNET_SM_EV_CMD, rmnet_sm_process_ev_cmd);

  /*-------------------------------------------------------------------------
    Check if QOS header type size is even else the function
    ps_ul_optimized_hdlr_hget32_unaligned needs to be changed which reads
    16 bits.
  -------------------------------------------------------------------------*/
  if(((sizeof(rmnet_meta_smi_qos_hdr_type)) % 2 ) != 0)
  {
    DATA_ERR_FATAL("rmnet_meta_smi_qos_hdr_type not even");
  }

  /* Read the EFS configuration at bootup and cache the settings. */
  rmnet_meta_sm_get_efs_config_params();

  memset(rmnet_smi_iface_info, 0, sizeof(rmnet_smi_iface_info));

  /*-------------------------------------------------------------------------
    Initialize the rmnet_smi_cm_info and set SS roaming status to  roaming
    (by default) for autoconnect
  -------------------------------------------------------------------------*/
  memset(&rmnet_smi_cm_info, 0, sizeof(rmnet_smi_cm_info_type));
  rmnet_smi_cm_info.ss_roaming_status = SYS_ROAM_STATUS_ON;

  memset(&rmnet_smi_eri_roam_ind_info, 0,
                      sizeof(rmnet_smi_eri_roam_ind_info_type));

  if (QMI_NV_STATUS_OK != qmi_nv_file_read(QMI_NV_FILE_ROAM_IND_LIST,
                                  (void *) rmnet_smi_eri_roam_ind_info.data,
                                  RMNET_ROAM_IND_FILE_MAX_LEN,
                                  (fs_size_t*)&rmnet_smi_eri_roam_ind_info.len))
  {
    LOG_MSG_ERROR_0("Reading QMI_NV_FILE_RMNET_ROAM_IND failed");
  }

  /*-------------------------------------------------------------------------
    Initialize the RMNET CM client & register for serving system events
  -------------------------------------------------------------------------*/
  if( CM_CLIENT_OK != cm_client_init( CM_CLIENT_TYPE_RM_NETWORK_INTERFACE,
                                      &rmnet_smi_cm_info.cm_client_id) )
  {
    ERR ("WWAN interface layer initialization failed!",0,0,0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    register call manager serving system event notification
  -------------------------------------------------------------------------*/
  if( CM_CLIENT_OK != cm_mm_client_ss_reg_msim(
                        rmnet_smi_cm_info.cm_client_id,
                        rmnet_meta_smi_cm_ss_event_cb,
                        CM_CLIENT_EVENT_REG,
                        CM_SS_EVENT_SRV_CHANGED,    /* min event to report */
                        CM_SS_EVENT_REG_REJECT,     /* max event to report */
                        NULL,                        /* other client errors */
                        SYS_MODEM_AS_ID_MASK_ANY))   /*Subs_id mask*/
  {
    LOG_MSG_ERROR_0("CM client SS event reg failed");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Activate the registered callback functions with CM.
    Activating these by posting a DCC cmd (instead of doing in init()
    to avoid having a lot of cm events during dcc_task init time
    leading to exhaustion of DCC cmd buffers
  -------------------------------------------------------------------------*/
  cmd_ptr = (rmnet_smi_cmd_type *)RMNET_SMI_GET_CMD_BUF_ASSERT(DCC_RMNET_SM_CMD);
  if(cmd_ptr != NULL)
  {
    cmd_ptr->sm_id     = RMNET_META_SM;
    cmd_ptr->cmd_type  = RMNET_SM_CMD_CM_CLIENT_ACTIVATE;
    dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr);
  }
  else
  {
    return FALSE;
  }


#if defined(FEATURE_IP_CALL)
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    rmnet_meta_smi_initialize_cmipapp_info();
    if (CMIPAPP_STATUS_OK != cmipapp_id_allocate((cmipapp_call_type_e_type)CM_CALL_TYPE_NONE_MASK,
                                                 (cmipapp_sys_mode_e_type) SYS_SYS_MODE_MASK_NONE,
                                                 &rmnet_smi_cmipapp_info.client_id))
    {
      LOG_MSG_ERROR_0("CM IP App client alloc failed");
      return FALSE;
    }

    if (CMIPAPP_STATUS_OK != cmipapp_reg_oprt_mode_func(
                               rmnet_smi_cmipapp_info.client_id,
                               (cmipapp_inform_oprt_mode_f_type*)&rmnet_meta_smi_oprt_mode_change_cb))
    {
      LOG_MSG_ERROR_0("CM IP App client oprt mode reg failed");
      return FALSE;
    }
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
#endif /* defined(FEATURE_IP_CALL) */


#if defined(FEATURE_RMNET_DATA_AGG_TIMER) && defined(FEATURE_CPU_BASED_FLOW_CONTROL)
  // Set FC signal handler
  (void)dcc_set_sig_handler(
                      DCC_RMNET_FC_SIGNAL,
                      rmnet_meta_sm_process_fc_sig,
                      NULL );
  dcc_enable_sig( DCC_RMNET_FC_SIGNAL );

  // Register with FC
  memset( &rmnet_fc_cmd_q, 0, sizeof(rmnet_fc_cmd_q) );
  (void)q_init( &rmnet_fc_cmd_q );
  if ( rmnet_meta_smi_fc_register() != TRUE )
  {
    LOG_MSG_ERROR_0("Failed to register with Flow Control (FC) module");
  }
#endif /* FEATURE_RMNET_DATA_AGG_TIMER && FEATURE_CPU_BASED_FLOW_CONTROL */

  ds_qmap_init();

  return TRUE;
} /* rmnet_sm_init() */



/*===========================================================================
  FUNCTION RMNET_META_SM_POST_EVENT()

  DESCRIPTION
    This function posts an event to RMNetSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
    instance:  Instance of RmNetSm to post event to
    event: The event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_post_event
(
  rmnet_instance_e_type      instance,
  rmnet_meta_sm_event_type   event
)
{
  rmnet_smi_ev_cmd_type * cmd_ptr;
  rmnet_smi_info_type   * info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL)
  {
    ASSERT(0);
    return;
  }

  if (event >= RMNET_META_SM_MAX_EV)
  {
    LOG_MSG_ERROR_1("Invalid RMNET_META_SM event %d, ignored", event);
    ASSERT (0);
    return;
  }

#ifdef T_ARM
  LOG_MSG_INFO1_3( "RMNET inst %d, Event %d posted to RMNET_META_SM in state %d",
                   instance, event, info->meta_sm.state );
#else
  MSG_SPRINTF_3(MSG_SSID_DS_PS, MSG_LEGACY_HIGH,
                "RMNET inst %d, Event %d posted to RMNET_META_SM in state %d",
                instance, event, info->meta_sm.state);
#endif

  /*-------------------------------------------------------------------------
    Get a DCC command buffer
  -------------------------------------------------------------------------*/
  cmd_ptr = (rmnet_smi_ev_cmd_type *)RMNET_SMI_GET_CMD_BUF_ASSERT(DCC_RMNET_SM_EV_CMD);
  if( cmd_ptr == NULL )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Post cmd to DCC task corresponding to each event
  -------------------------------------------------------------------------*/
  cmd_ptr->sm_id             = RMNET_META_SM;
  cmd_ptr->event             = event;
  cmd_ptr->rmnet_sm_info_ptr = (void *) info;

  dcc_send_cmd_ex(DCC_RMNET_SM_EV_CMD, (void *)cmd_ptr);

} /* rmnet_meta_sm_post_event() */


/*===========================================================================
  FUNCTION RMNET_META_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM event cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_process_ev_cmd
(
  dcc_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  rmnet_smi_info_type      * info;
  rmnet_meta_sm_event_type   event;
  rmnet_smi_ev_cmd_type    * cmd_ptr;
  rmnet_instance_e_type      rmnet_inst;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_ptr = (rmnet_smi_ev_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == DCC_RMNET_SM_EV_CMD) && 
                      (cmd_ptr->sm_id == RMNET_META_SM));

  event = (rmnet_meta_sm_event_type) cmd_ptr->event;
  info  = (rmnet_smi_info_type *)
          cmd_ptr->rmnet_sm_info_ptr;

  /*-------------------------------------------------------------------------
    Verify that RMNET_META_SM is initialized
  -------------------------------------------------------------------------*/
  if (info == NULL)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("Can't process event %d, RMNET_META_SM not inited", event);
    return;
  }

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  LOG_MSG_INFO2_3( "RmNetSm event %d in state %d, RMNET inst %d",
                   event, info->meta_sm.state, rmnet_inst);

  /*-------------------------------------------------------------------------
    Validate posted event before processing
  -------------------------------------------------------------------------*/
  if (event >= RMNET_META_SM_MAX_EV)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("Invald RMNET_META_SM event %d, ignoring", event );
    return;
  }

  /*-------------------------------------------------------------------------
    Validate RMNET_META_SM state before processing event - this allows event handler
    functions to assume that state is valid.
  -------------------------------------------------------------------------*/
  if (info->meta_sm.state >= RMNET_META_SM_MAX_STATE)
  {
    ASSERT( 0 );
    LOG_MSG_ERROR_1("Invalid RMNET_META_SM state %d", info->meta_sm.state);
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke associated command handler function
  -------------------------------------------------------------------------*/
  (*(rmnet_meta_smi_cmd_hdlr[event]))(info);

} /* rmnet_meta_sm_process_ev_cmd() */

/*===========================================================================
  FUNCTION RMNET_META_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet Meta SM cmd.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_process_cmd
(
  dcc_cmd_enum_type    cmd,
  void                *user_data_ptr
)
{
  rmnet_smi_info_type *  info;
  rmnet_smi_info_type *  info_v6;
  rmnet_smi_cmd_type   * cmd_ptr;
  rmnet_smi_dual_ip_info_type * dual_ip_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = (  rmnet_smi_cmd_type *)user_data_ptr;
  ASSERT (cmd_ptr && (cmd == DCC_RMNET_SM_CMD) && 
                (cmd_ptr->sm_id == RMNET_META_SM));

  switch(cmd_ptr->cmd_type)
  {
    case RMNET_SM_CMD_TX_FLOW_ENABLE:
      if (cmd_ptr->info_ptr == NULL)
      {
        break;
      }

      dual_ip_info = (rmnet_smi_dual_ip_info_type *) cmd_ptr->info_ptr;

     /*---------------------------------------------------------------------
        Enable outgoing data on the Rm interface.
        The air interface should check if ps_iface flow is enabled before
        sending data on to this interface.
      ---------------------------------------------------------------------*/
      info = dual_ip_info->info_v4;
      if(info != NULL && info->rmnet_iface_ptr != NULL)
      {
        ps_iface_enable_flow(info->rmnet_iface_ptr, DS_FLOW_RMSM_MASK);
      }

      info_v6 = dual_ip_info->info_v6;
      if(info_v6 != NULL && info_v6->rmnet_iface_ptr != NULL)
      {
        ps_iface_enable_flow(info_v6->rmnet_iface_ptr, DS_FLOW_RMSM_MASK);
      }
      break;

    case RMNET_SM_CMD_TX_FLOW_DISABLE:
      if (cmd_ptr->info_ptr == NULL)
      {
        return;
      }

      dual_ip_info = (rmnet_smi_dual_ip_info_type *) cmd_ptr->info_ptr;

      info = dual_ip_info->info_v4;
      if(info != NULL && info->rmnet_iface_ptr != NULL)
      {
        ps_iface_disable_flow(info->rmnet_iface_ptr, DS_FLOW_RMSM_MASK);
      }

      info_v6 = dual_ip_info->info_v6;
      if(info_v6 != NULL && info_v6->rmnet_iface_ptr != NULL)
      {
        ps_iface_disable_flow(info_v6->rmnet_iface_ptr, DS_FLOW_RMSM_MASK);
      }
      break;

    case RMNET_SM_CMD_CM_CLIENT_ACTIVATE:
      if (!rmnet_meta_smi_reg_cm())
      {
        LOG_MSG_INFO1_0("rmnet_meta_smi_reg_cm() failed");
      }
      break;

    case RMNET_SM_CMD_NEW_CM_SS_INFO:
      rmnet_meta_smi_process_cm_ss_event(cmd_ptr->data.cm_ss.event,
                                         cmd_ptr->data.cm_ss.info,
                                         cmd_ptr->data.cm_ss.asubs_id);
      break;

#if defined(FEATURE_IP_CALL)
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
     case RMNET_SM_CMD_CM_OP_MODE_CHANGE:
      rmnet_meta_smi_cm_op_mode_change_cmd_hdlr (cmd_ptr);
      break;
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
#endif /* defined(FEATURE_IP_CALL) */

    case RMNET_SM_CMD_AUTOCONNECT_TIMER:
      rmnet_meta_smi_process_autoconnect_timer_cmd_hdlr(cmd_ptr->info_ptr);
    break;

    case RMNET_SM_CMD_PROCESS_PHYS_LINK_UP_DOWN_EVT:
      rmnet_meta_smi_process_phys_link_up_down_event(
                           cmd_ptr->data.phys_link_status.phys_link_ptr,
                           cmd_ptr->data.phys_link_status.event,
                           cmd_ptr->data.phys_link_status.event_info,
                           cmd_ptr->data.phys_link_status.user_data_ptr);

    break;

    case RMNET_SM_CMD_QCRMCALL:
      rmnet_meta_smi_process_qcrmcall_cmd(
                          cmd_ptr->data.qcrmcall.action,
                          &cmd_ptr->data.qcrmcall.request,
                          cmd_ptr->data.qcrmcall.callback);
      break;

    default:
      LOG_MSG_INFO1_1("Received Unhandled rmnet_meta_sm cmd %d",
                      cmd_ptr->cmd_type);
      ASSERT(0);
      break;
  }
} /* rmnet_meta_sm_process_cmd() */


/*===========================================================================
  FUNCTION RMNET_META_SM_IN_CALL()

  DESCRIPTION
    Returns true if we're in a packet call.  Called by ws_if.c

  PARAMETERS
    instance - the RMNET Rm interface to query

  RETURN VALUE
    TRUE  - Rm interface 'instance' is starting/in/stopping a packet call
    FALSE - Rm interface 'instance' is idle

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_in_call
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  /*-------------------------------------------------------------------------
    We are in a packet call if rmsm state is non-null
  -------------------------------------------------------------------------*/
  return (info == NULL ||
      info->meta_sm.state == RMNET_META_SM_NULL_STATE) ? FALSE : TRUE;

} /* rmnet_meta_sm_in_call() */


/*===========================================================================
  FUNCTION RMNET_META_SM_IFACE()

  DESCRIPTION
    Returns the ps_iface controlled by the given RMNET RmSM instance

  PARAMETERS
    instance - the RMNET RmSM instance to query

  RETURN VALUE
    ps_iface *  - Pointer to the Rm ps_iface controlled by the spec'd RmSM
    NULL        - RmNetSM not initialized

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type * rmnet_meta_sm_iface
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  return (info == NULL) ? NULL : info->rmnet_iface_ptr;
} /* rmnet_meta_sm_iface() */


/*===========================================================================
FUNCTION RMNET_META_SM_SET_NETWORK_CFG_PARAMS

DESCRIPTION
  This function is called to set the parameters received in Start Network
  Interface for network configuration in the rmnet info structure.

PARAMETERS
  instance       : RmNet instance
  net_cfg_params : Network Configuration parameters

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rmnet_meta_sm_set_network_cfg_params
(
  rmnet_instance_e_type      instance,
  rmnet_meta_sm_network_info_type *  net_cfg_params
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL)
  {
    ASSERT(0);
    return;
  }

  memscpy(&(info->meta_sm.net_params),
          sizeof(rmnet_meta_sm_network_info_type),
          net_cfg_params,
          sizeof(rmnet_meta_sm_network_info_type));

} /* rmnet_meta_sm_set_network_cfg_params() */


/*===========================================================================
  FUNCTION RMNET_SM_GET_UM_IFACE()

  DESCRIPTION
    Returns the UM ps_iface ptr for the given RmNetSM instance

  PARAMETERS
    instance - the RmSM instance to query

  RETURN VALUE
    NULL        - Currently out of call
    ps_iface *  - Pointer to the UM ps_iface controlled by the spec'd RmSM

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type * rmnet_meta_sm_get_um_iface
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  return (info == NULL) ? NULL : info->um_iface_ptr;
} /* rmnet_sm_get_um_iface() */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_UM_IFACE_EX()

  DESCRIPTION
    Returns the UM ps_iface ptr for the given RmNetSM instance. If not
    available, gets it from net params.

  PARAMETERS
    instance - the RmSM instance to query

  RETURN VALUE
    NULL        - Currently out of call
    ps_iface *  - Pointer to the UM ps_iface controlled by the spec'd RmSM

  DEPENDENCIES
    This function is expected to be called only after net_param is set.

  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type * rmnet_meta_sm_get_um_iface_ex
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type        * info = NULL;
  rmnet_meta_sm_arb_info_type  arb_info = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL)
  {
    return NULL;
  }

  if (info->um_iface_ptr != NULL)
  {
    return info->um_iface_ptr;
  }

  return rmnet_meta_smi_get_um_iface_ptr(
                       &info->meta_sm.net_params,
                       RMNET_RT_LK_UP_TYPE_IFACE,
                       NULL,
                       &arb_info);

} /* rmnet_meta_sm_get_um_iface_ex() */

/*===========================================================================
  FUNCTION rmnet_smi_retrieve_rm_ifaces_from_netparam()

  DESCRIPTION
    This function returns Rm ifaces associated with filter id.

  PARAMETERS
    filter_id     :  packet filter id
    rm_iface_ptr  :  First rm iface ptr
    rm_iface_ptr2 :  Second rm iface ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_smi_retrieve_rm_ifaces_from_netparam
(
  uint16                filter_id,
  ps_iface_type      ** rm_iface_ptr1,
  ps_iface_type      ** rm_iface_ptr2
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL != rm_iface_ptr1)
  {
    *rm_iface_ptr1 = NULL;
  }

  if (NULL != rm_iface_ptr2)
  {
    *rm_iface_ptr2 = NULL;
  }
  
  /*-------------------------------------------------------------------
    Loop through all rmnet instaces and compare the filter id, if they
    match then retrieve rm iface associated with rmnet instance.
  -------------------------------------------------------------------*/  
  for (i = 0; i < RMNET_INSTANCE_MAX; i++) 
    {
    if (rmnet_smi_info[i] != NULL &&
        rmnet_smi_info[i]->meta_sm.net_params.packet_filter_id == filter_id)
      {
        if (NULL != rm_iface_ptr1 && NULL == *rm_iface_ptr1)
        {
        *rm_iface_ptr1 = rmnet_smi_info[i]->rmnet_iface_ptr;
        }
        else if (NULL != rm_iface_ptr2 && NULL == *rm_iface_ptr2)
        {
        *rm_iface_ptr2 = rmnet_smi_info[i]->rmnet_iface_ptr;
      }
    }
  } 
  
  return;
} /* rmnet_smi_retrieve_rm_ifaces_from_netparam() */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_AUTOCONNECT_CONFIG()

  DESCRIPTION
    Returns autoconnect config for a particular end point

  PARAMETERS
    ep_id:            end point id
    mux_id:           mux_id

  RETURN VALUE
    autoconnect config ptr
    NULL if not found

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static rmnet_efs_autoconnect_s_type * rmnet_meta_smi_get_autoconnect_config
(
  uint32                    ep_id,
  uint8                     mux_id
)
{
  rmnet_efs_autoconnect_s_type  * autoconnect_config;
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ep_id == 0)
  {
    return NULL;
  }

  for (i = 0; i < RMNET_AUTOCONNECT_MAX; i++)
  {
    autoconnect_config = &rmnet_efs_autoconnect_config[i];
    if (autoconnect_config->ep_id == ep_id &&
        autoconnect_config->mux_id == mux_id &&
        (autoconnect_config->autoconnect_setting == RMNET_AUTOCONNECT_ENABLED ||
         autoconnect_config->autoconnect_setting == RMNET_AUTOCONNECT_PAUSED))
    {
      return autoconnect_config;
    }
  }

  return NULL;

} /* rmnet_meta_smi_get_autoconnect_config() */

/*===========================================================================
  FUNCTION RMNET_META_SM_AUTOCONNECT_BIND()

  DESCRIPTION
    Automatically bind rmnet if autoconnect is enabled in EFS

  PARAMETERS
    ep_id:            end point id

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_autoconnect_bind
(
  uint32  ep_id
)
{
  rmnet_efs_autoconnect_s_type  * autoconnect_config;
  rmnet_data_format_type          data_format;
  uint32                          data_format_mask;
  rmnet_set_data_fmt_e_type       data_format_client;
  int                             i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ep_id == 0)
  {
    return;
  }

  // For all autoconnect config that has this ep_id
  for (i = 0; i < RMNET_AUTOCONNECT_MAX; i++)
  {
    autoconnect_config = &rmnet_efs_autoconnect_config[i];
    if (autoconnect_config->ep_id == ep_id &&
        autoconnect_config->autoconnect_setting == RMNET_AUTOCONNECT_ENABLED)
    {
      // Set data format only if the format has not been set
      data_format_client = rmnet_phys_xport_data_format_client(ep_id);
      if (data_format_client == RMNET_SET_DATA_FORMAT_NONE ||
          data_format_client == RMNET_SET_DATA_FORMAT_DTR_LOW)
      {
        memset(&data_format, 0, sizeof(data_format));
        data_format.link_prot =
                   (rmnet_sm_link_prot_e_type)autoconnect_config->link_prot;
        data_format.qos = autoconnect_config->qos;
        data_format.ul_data_agg_protocol =
                   (rmnet_data_agg_enum_type)autoconnect_config->ul_data_format;
        data_format.dl_data_agg_protocol =
                   (rmnet_data_agg_enum_type)autoconnect_config->dl_data_format;

        data_format_mask = RMNET_DATA_FORMAT_MASK_LINK_PROT |
                            RMNET_DATA_FORMAT_MASK_QOS |
                            RMNET_DATA_FORMAT_MASK_UL_DATA_AGG |
                            RMNET_DATA_FORMAT_MASK_DL_DATA_AGG;

        LOG_MSG_INFO1_1("Setting autoconnect data format on EP 0x%x", ep_id);
        rmnet_phys_xport_set_data_format(
                    ep_id, &data_format, data_format_mask,
                    RMNET_SET_DATA_FORMAT_DTR_HGH);
      }

      rmnet_meta_sm_bind(ep_id, autoconnect_config->mux_id);
    }
  }

} /* rmnet_meta_sm_autoconnect_bind() */


/*===========================================================================
  FUNCTION RMNET_META_SM_IS_AUTO_CONNECT_ENABLED()

  DESCRIPTION
    Returns whether auto_connect is enabled on this instance or not

  PARAMETERS
    instance - the RmNet instance

  RETURN VALUE
    TRUE  - auto connect is enabled
    FALSE - auto connect is disabled

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_is_auto_connect_enabled
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  return (info == NULL) ? FALSE : info->meta_sm.settings.auto_connect_enabled;

} /* rmnet_meta_sm_is_auto_connect_enabled() */

/*===========================================================================
  FUNCTION RMNET_META_SM_ENABLE_AUTO_CONNECT()

  DESCRIPTION
    Enable Autoconnect

  PARAMETERS
    instance - the RmNet instance

  RETURN VALUE
    TRUE  - auto connect is enabled
    FALSE - auto connect is disabled

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_enable_auto_connect
(
  rmnet_instance_e_type  instance,
  boolean                enable
)
{
  rmnet_meta_sm_autoconnect_setting_e_type  autoconnect_setting;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* convert boolean to auto connect setting */
  autoconnect_setting =
          enable ? RMNET_AUTOCONNECT_ENABLED : RMNET_AUTOCONNECT_DISABLED;

  /*-----------------------------------------------------------------------
    Calling new function to set autoconnect settings.
  -----------------------------------------------------------------------*/
  return(rmnet_meta_sm_set_autoconnect_setting(instance, autoconnect_setting,
                   rmnet_meta_sm_get_autoconnect_roam_setting(instance)));

} /* rmnet_meta_sm_enable_auto_connect() */

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_AUTOCONNECT_SETTING()

  DESCRIPTION
    set autoconnect setting

  PARAMETERS
    instance - the RmNet instance
    autoconnect_setting  - the autoconnect value

  RETURN VALUE
    TRUE  - set autoconnect setting is successful
    FALSE - set autoconnect setting is unsuccessful

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_set_autoconnect_setting
(
  rmnet_instance_e_type                         instance,
  rmnet_meta_sm_autoconnect_setting_e_type      autoconnect_setting,
  rmnet_meta_sm_autoconnect_roam_setting_e_type roam_setting
)
{
  rmnet_smi_info_type           * info = NULL;
  rmnet_efs_autoconnect_s_type  * autoconnect_config;
  rmnet_data_format_type        * data_format;
  uint32                          ep_id;
  uint8                           mux_id;
  int                             i;
  int                             status = 0;
  boolean                         result = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL ||
      info->xport_info == NULL ||
      info->xport_info->phys_xport == NULL)
  {
    status = 1;
    result = FALSE;
    goto send_result;
  }

  ep_id = info->xport_info->ep_id;
  mux_id = info->xport_info->mux_id;
  data_format = &info->xport_info->phys_xport->data_format;

  if (ep_id == 0)
  {
    status = 2;
    result = FALSE;
    goto send_result;
  }

  /*-----------------------------------------------------------------------
    Get current autoconnect settings. Return the config even if autoconnect
    is disabled so we can use the EFS autoconnect parameters
  -----------------------------------------------------------------------*/
  autoconnect_config = rmnet_meta_smi_get_autoconnect_config(ep_id, mux_id);
  if (autoconnect_config == NULL)
  {
    // This is a new autoconnect setting
    if (autoconnect_setting == RMNET_AUTOCONNECT_ENABLED)
    {
      for (i = 0; i < RMNET_AUTOCONNECT_MAX; i++)
      {
        autoconnect_config = &rmnet_efs_autoconnect_config[i];
        if (autoconnect_config->autoconnect_setting == RMNET_AUTOCONNECT_DISABLED)
        {
          break;
        }
      }
      if (i >= RMNET_AUTOCONNECT_MAX)
      {
        status = 3;
        result = FALSE;
        goto send_result;
      }
      // Use existing data format values
      autoconnect_config = &rmnet_efs_autoconnect_config[i];
      autoconnect_config->ep_id = ep_id;
      autoconnect_config->mux_id = mux_id;
      autoconnect_config->link_prot = (uint8)data_format->link_prot;
      autoconnect_config->qos = data_format->qos;
      autoconnect_config->ul_data_format = (uint8)data_format->ul_data_agg_protocol;
      autoconnect_config->dl_data_format = (uint8)data_format->dl_data_agg_protocol;
      autoconnect_config->umts_profile = RMNET_EFS_INVALID_PROFILE;
      autoconnect_config->cdma_profile = RMNET_EFS_INVALID_PROFILE;
      autoconnect_config->subs_id = PS_SYS_DEFAULT_SUBS;
    }
    else
    {
      status = 4;
      result = TRUE;
      goto send_result;
    }
  }

  /* Determine new autoconnect setting and IP family type */
  if (autoconnect_setting == RMNET_AUTOCONNECT_ENABLED)
  {
    if (autoconnect_config->autoconnect_setting == RMNET_AUTOCONNECT_DISABLED)
    {
      // Reset IP type
      autoconnect_config->ip_type = 0;
    }
    autoconnect_config->autoconnect_setting = RMNET_AUTOCONNECT_ENABLED;

    if ( (autoconnect_config->ip_type == QMI_AF_INET && instance % 2 == 1) ||
         (autoconnect_config->ip_type == QMI_AF_INET6 && instance % 2 == 0) ||
         (autoconnect_config->ip_type == QMI_AF_INET_4_6) )
    {
      // Both v4 and v6 are enabled
      autoconnect_config->ip_type = QMI_AF_INET_4_6;
    }
    else
    {
      // Only v4 or v6 is enabled
      autoconnect_config->ip_type = (instance % 2 == 0) ? QMI_AF_INET : QMI_AF_INET6;
    }
  }
  else
  {
    // DISABLE or PAUSE
    if ( (autoconnect_config->ip_type == QMI_AF_INET && instance % 2 == 1) ||
         (autoconnect_config->ip_type == QMI_AF_INET6 && instance % 2 == 0) ||
         (autoconnect_config->ip_type == QMI_AF_INET_4_6) )
    {
      // Only v4 or v6 is disabled
      autoconnect_config->ip_type = (instance % 2 == 0) ? QMI_AF_INET6 : QMI_AF_INET;
    }
    else
    {
      // Both v4 and v6 are disabled
      autoconnect_config->autoconnect_setting = autoconnect_setting;
    }
  }

  /* Determine new roam setting */
  if (roam_setting == RMNET_AUTOCONNECT_ROAM_SETTING_ALWAYS ||
      roam_setting == RMNET_AUTOCONNECT_ROAM_SETTING_HOME_ONLY)
  {
    autoconnect_config->roam_setting = roam_setting;
  }
  else
  {
    autoconnect_config->roam_setting = RMNET_AUTOCONNECT_ROAM_SETTING_ALWAYS;
  }

  // Write NV. PAUSE is a temporary state so don't write it
  if (autoconnect_setting != RMNET_AUTOCONNECT_PAUSED)
  {
    if (qmi_nv_write(QMI_NV_ITEM_RMNET_AUTOCONNECT, 0, 0,
                     rmnet_efs_autoconnect_config,
                     sizeof(rmnet_efs_autoconnect_config)) != QMI_NV_STATUS_OK)
    {
      status = 5;
      result = FALSE;
      goto send_result;
    }
  }

  // Update autoconnect settings
  if (info->meta_sm.state == RMNET_META_SM_NULL_STATE)
  {
    rmnet_meta_smi_set_default_net_params(info);
  }
  rmnet_meta_smi_set_default_autoconn_params(info);

send_result:
  if (status)
  {
    LOG_MSG_ERROR_1("Failed to set autoconnect setting. status %d", status);
  }
  return result;

} /* rmnet_meta_sm_set_autoconnect_setting() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_AUTOCONNECT_SETTING()

  DESCRIPTION
    get autoconnect setting

  PARAMETERS
    instance - the RmNet instance

  RETURN VALUE
    uint8  - auto connect setting value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_meta_sm_autoconnect_setting_e_type rmnet_meta_sm_get_autoconnect_setting
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type  * info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  /* return autoconnect setting from cache */
  return (info == NULL) ?  RMNET_AUTOCONNECT_DISABLED :
                           info->meta_sm.settings.autoconnect_setting;

} /* rmnet_meta_sm_get_autoconnect_setting() */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_AUTOCONNECT_ROAM_SETTING()

  DESCRIPTION
    get autoconnect roam setting

  PARAMETERS
    instance - the RmNet instance

  RETURN VALUE
    roam_setting  - auto connect roam value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
rmnet_meta_sm_autoconnect_roam_setting_e_type rmnet_meta_sm_get_autoconnect_roam_setting
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type * info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  /* return autoconnect roaming setting from cache */
  return (info == NULL) ? RMNET_AUTOCONNECT_ROAM_SETTING_MAX :
                          info->meta_sm.settings.autoconnect_roam_setting;

} /* rmnet_meta_sm_get_autoconnect_roam_setting() */


/*===========================================================================
  FUNCTION RMNET_META_SM_GET_PREV_BYTE_STATS()

  DESCRIPTION
    Returns the last call byte stats

  PARAMETERS
    instance - the RmSM instance to query
    last_call_rx_bytes - pointer to Rx byte stats var
    last_call_tx_bytes - pointer to Tx byte stats var

  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_get_prev_byte_stats
(
  rmnet_instance_e_type  instance,
  uint64 * last_call_rx_bytes,
  uint64 * last_call_tx_bytes
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( RMNET_INSTANCE_MAX > instance &&
       NULL != last_call_rx_bytes &&
       NULL != last_call_tx_bytes )
  {
    info = rmnet_smi_info[instance];
  }

  if (info != NULL)
  {
    *last_call_rx_bytes = info->meta_sm.last_call_rx_bytes;
    *last_call_tx_bytes = info->meta_sm.last_call_tx_bytes;
  }

} /* rmnet_meta_sm_get_prev_byte_stats() */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_IPV4_MANUAL_DNS_ADDRS()

  DESCRIPTION
    Gets the manual DNS settings for this instance.  Each address that has not
    been set, the type will be returned as IP_ADDR_INVALID.

  PARAMETERS
    instance    - the RmNet instance
    pri_dns_ptr - primary DNS address pointer
    sec_dns_ptr - secondary DNS address pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_get_ipv4_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }
  
  if (info == NULL)
  {
    ASSERT(0);
    return;
  }

  if (NULL != pri_dns_ptr)
  {
    memscpy(pri_dns_ptr,
            sizeof(ip_addr_type),
           &info->meta_sm.settings.pri_dns_addr,
           sizeof(ip_addr_type));
  }
  if (NULL != sec_dns_ptr)
  {
    memscpy(sec_dns_ptr,
            sizeof(ip_addr_type),
           &info->meta_sm.settings.sec_dns_addr,
           sizeof(ip_addr_type));
  }
} /* rmnet_meta_sm_get_ipv4_manual_dns_addrs */

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_IPV4_MANUAL_DNS_ADDRS()

  DESCRIPTION
    Sets the manual IPv4 DNS settings used for this instance.

  PARAMETERS
    instance    - the RmNet instance
    pri_dns_ptr - primary DNS address
    sec_dns_ptr - secondary DNS address

  RETURN VALUE
    0  - update successful
    -1 - udpate failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int rmnet_meta_sm_set_ipv4_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
)
{
  rmnet_smi_info_type  * info = NULL;
  qmi_nv_status_e_type   qmi_nv_status;
  qmi_nv_dns_addr_type   qmi_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL)
  {
    ASSERT(0);
    return -1;
  }

  if (info->xport_info == NULL)
  {
    LOG_MSG_ERROR_1("Xport Info is null for instance %d", instance);
    return -1;
  }

  /* only check primary address if provided */
  if (NULL != pri_dns_ptr)
  {
    /* check for IPv4 address type */
    if (IPV4_ADDR != pri_dns_ptr->type)
    {
      LOG_MSG_ERROR_2("non-IPv4 type passed, pri_dns_ptr=0x%p type=%d",
                      pri_dns_ptr, pri_dns_ptr->type);
      return -1;
    }
  }

  /* only check secondary address if provided */
  if (NULL != sec_dns_ptr)
  {
    /* check for IPv4 address type */
    if (IPV4_ADDR != sec_dns_ptr->type)
    {
      LOG_MSG_ERROR_2("non-IPv4 type passed, sec_dns_ptr=0x%p type=%d",
                      sec_dns_ptr, sec_dns_ptr->type);
      return -1;
    }
  }

  if (NULL != pri_dns_ptr)
  {
    memset(&qmi_nv_item, 0, sizeof(qmi_nv_item));
    /* update the primary DNS setting */
    qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI,
                                info->xport_info->ep_id,
                                info->xport_info->mux_id,
                                &qmi_nv_item,
                                sizeof(qmi_nv_item));
    if ((QMI_NV_STATUS_OK == qmi_nv_status) &&
        (qmi_nv_item.dns_addr == pri_dns_ptr->addr.v4))
    {
      LOG_MSG_INFO2_3("pri_dns_ptr value already set %d, %d, %d",qmi_nv_item.dns_addr,pri_dns_ptr->addr.v4,qmi_nv_status);
    }
    else
    {
      /* update the NV item */
      LOG_MSG_INFO1_1("Updating the NV item with the address %d", pri_dns_ptr->addr.v4);
      qmi_nv_item.ep_id    = info->xport_info->ep_id;
      qmi_nv_item.mux_id   = info->xport_info->mux_id;
      qmi_nv_item.dns_addr = pri_dns_ptr->addr.v4;
      qmi_nv_status = qmi_nv_write(QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI,
                                   info->xport_info->ep_id,
                                   info->xport_info->mux_id,
                                   &qmi_nv_item,
                                   sizeof(qmi_nv_item));
      if( QMI_NV_STATUS_OK != qmi_nv_status )
      {
        LOG_MSG_ERROR_2 ("Unable to write NV for pri_dns_ptr (0x%p - %d)",
                         pri_dns_ptr, qmi_nv_status);
        return -1;
      }

      /* update the cached value */
      info->meta_sm.settings.pri_dns_addr.type = IPV4_ADDR;
      info->meta_sm.settings.pri_dns_addr.addr.v4 = pri_dns_ptr->addr.v4;
    }
  }

  if (NULL != sec_dns_ptr)
  {
    /* update the secondary DNS setting */
    memset(&qmi_nv_item, 0, sizeof(qmi_nv_item));
    qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC,
                                info->xport_info->ep_id,
                                info->xport_info->mux_id,
                                &qmi_nv_item,
                                sizeof(qmi_nv_item));
    if ((QMI_NV_STATUS_OK == qmi_nv_status) &&
        (qmi_nv_item.dns_addr == sec_dns_ptr->addr.v4))
    {
      LOG_MSG_INFO2_0("sec_dns_ptr value already set");
    }
    else
    {
      /* update the NV item */
      qmi_nv_item.ep_id    = info->xport_info->ep_id;
      qmi_nv_item.mux_id   = info->xport_info->mux_id;
      qmi_nv_item.dns_addr = sec_dns_ptr->addr.v4;
      qmi_nv_status = qmi_nv_write(QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC,
                                   info->xport_info->ep_id,
                                   info->xport_info->mux_id,
                                   &qmi_nv_item,
                                   sizeof(qmi_nv_item));
      if( QMI_NV_STATUS_OK != qmi_nv_status )
      {
        LOG_MSG_ERROR_2 ("Unable to write NV for sec_dns_ptr (0x%p - %d)",
                         sec_dns_ptr, qmi_nv_status);
        return -1;
      }

      /* update the cached value */
      info->meta_sm.settings.sec_dns_addr.type = IPV4_ADDR;
      info->meta_sm.settings.sec_dns_addr.addr.v4 = sec_dns_ptr->addr.v4;
    }
  }

  return 0;
} /* rmnet_meta_sm_set_ipv4_manual_dns_addrs() */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_IPV6_MANUAL_DNS_ADDRS()

  DESCRIPTION
    Gets the manual DNS settings for this instance.  Each address that has not
    been set, the type will be returned as IP_ADDR_INVALID.

  PARAMETERS
    instance    - the RmNet instance
    pri_dns_ptr - primary DNS address pointer
    sec_dns_ptr - secondary DNS address pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_get_ipv6_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL)
  {
    ASSERT(0);
    return;
  }

  if (NULL != pri_dns_ptr)
  {
    memscpy(pri_dns_ptr,
            sizeof(ip_addr_type),
           &info->meta_sm.settings.ipv6_pri_dns_addr,
           sizeof(ip_addr_type));
  }
  if (NULL != sec_dns_ptr)
  {
    memscpy(sec_dns_ptr,
            sizeof(ip_addr_type),
           &info->meta_sm.settings.ipv6_sec_dns_addr,
           sizeof(ip_addr_type));
  }
} /* rmnet_meta_sm_get_ipv6_manual_dns_addrs */

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_IPV6_MANUAL_DNS_ADDRS()

  DESCRIPTION
    Sets the manual IPv6 DNS settings used for this instance.

  PARAMETERS
    instance    - the RmNet instance
    pri_dns_ptr - primary DNS address
    sec_dns_ptr - secondary DNS address

  RETURN VALUE
    0  - update successful
    -1 - udpate failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int rmnet_meta_sm_set_ipv6_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
)
{
  rmnet_smi_info_type     * info = NULL;
  qmi_nv_status_e_type      qmi_nv_status;
  qmi_nv_ipv6_dns_addr_type qmi_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  if (info == NULL)
  {
    ASSERT(0);
    return -1;
  }
  if (info->xport_info == NULL)
  {
    return -1;
  }
  /* only check primary address if provided */
  if (NULL != pri_dns_ptr)
  {
    /* check for IPv4 address type */
    if (IPV6_ADDR != pri_dns_ptr->type)
    {
      LOG_MSG_ERROR_2("non-IPv6 type passed, pri_dns_ptr=0x%p type=%d",
                      pri_dns_ptr, pri_dns_ptr->type);
      return -1;
    }
  }

  /* only check secondary address if provided */
  if (NULL != sec_dns_ptr)
  {
    /* check for IPv6 address type */
    if (IPV6_ADDR != sec_dns_ptr->type)
    {
      LOG_MSG_ERROR_2("non-IPv6 type passed, sec_dns_ptr=0x%p type=%d",
                      sec_dns_ptr, sec_dns_ptr->type);
      return -1;
    }
  }

  if (NULL != pri_dns_ptr)
  {
    /* update the primary DNS setting */
    memset(&qmi_nv_item, 0, sizeof(qmi_nv_item));
    qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI,
                                info->xport_info->ep_id,
                                info->xport_info->mux_id,
                                &qmi_nv_item,
                                sizeof(qmi_nv_item));
    if ((QMI_NV_STATUS_OK == qmi_nv_status) &&
        (qmi_nv_item.dns_addr[0] == pri_dns_ptr->addr.v6[0]) &&
        (qmi_nv_item.dns_addr[1] == pri_dns_ptr->addr.v6[1]))
    {
      LOG_MSG_INFO2_0("pri_dns_ptr value already set");
    }
    else
    {
      /* update the NV item */
      qmi_nv_item.ep_id       = info->xport_info->ep_id;
      qmi_nv_item.mux_id      = info->xport_info->mux_id;
      qmi_nv_item.dns_addr[0] = pri_dns_ptr->addr.v6[0];
      qmi_nv_item.dns_addr[1] = pri_dns_ptr->addr.v6[1];
      qmi_nv_status = qmi_nv_write(QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI,
                                   info->xport_info->ep_id,
                                   info->xport_info->mux_id,
                                   &qmi_nv_item,
                                   sizeof(qmi_nv_item));
      if( QMI_NV_STATUS_OK != qmi_nv_status )
      {
        LOG_MSG_ERROR_2 ("Unable to write NV for pri_dns_ptr (0x%p - %d)",
                         pri_dns_ptr, qmi_nv_status);
        return -1;
      }

      /* update the cached value */
      info->meta_sm.settings.ipv6_pri_dns_addr.type = IPV6_ADDR;
      info->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[0] = pri_dns_ptr->addr.v6[0];
      info->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[1] = pri_dns_ptr->addr.v6[1];
    }
  }

  if (NULL != sec_dns_ptr)
  {
    /* update the secondary DNS setting */
    memset(&qmi_nv_item, 0, sizeof(qmi_nv_item));
    qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC,
                                info->xport_info->ep_id,
                                info->xport_info->mux_id,
                                &qmi_nv_item,
                                sizeof(qmi_nv_item));
    if ((QMI_NV_STATUS_OK == qmi_nv_status) &&
        (qmi_nv_item.dns_addr[0] == sec_dns_ptr->addr.v6[0]) &&
        (qmi_nv_item.dns_addr[1] == sec_dns_ptr->addr.v6[1]))
    {
      LOG_MSG_INFO2_0("sec_dns_ptr value already set");
    }
    else
    {
      /* update the NV item */
      qmi_nv_item.ep_id       = info->xport_info->ep_id;
      qmi_nv_item.mux_id      = info->xport_info->mux_id;
      qmi_nv_item.dns_addr[0] = sec_dns_ptr->addr.v6[0];
      qmi_nv_item.dns_addr[1] = sec_dns_ptr->addr.v6[1];
      qmi_nv_status = qmi_nv_write(QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC,
                                   info->xport_info->ep_id,
                                   info->xport_info->mux_id,
                                   &qmi_nv_item,
                                   sizeof(qmi_nv_item));
      if( QMI_NV_STATUS_OK != qmi_nv_status )
      {
        LOG_MSG_ERROR_2 ("Unable to write NV for sec_dns_ptr (0x%p - %d)",
                         sec_dns_ptr, qmi_nv_status);
        return -1;
      }

      /* update the cached value */
      info->meta_sm.settings.ipv6_sec_dns_addr.type = IPV6_ADDR;
      info->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[0] = sec_dns_ptr->addr.v6[0];
      info->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[1] = sec_dns_ptr->addr.v6[1];
    }
  }

  return 0;
} /* rmnet_meta_sm_set_ipv6_manual_dns_addrs() */

void rmnet_meta_sm_reg_iface_binding_cb
(
  rmnet_instance_e_type inst,
  rmnet_iface_binding_complete_cb_f_type rmnet_iface_binding_complete_cb,
  void *  user_data
)
{
  rmnet_smi_info_type *  info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(rmnet_iface_binding_complete_cb);
  info = rmnet_smi_info[inst];
  ASSERT(info);

  info->meta_sm.iface_binding.complete_cback = rmnet_iface_binding_complete_cb;
  info->meta_sm.iface_binding.user_data = user_data;
}

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_PROC_ID_FROM_INST

  DESCRIPTION
    This function returns the proc_id for a particular rmnet instance

  PARAMETERS
    proc_id : proc_id address
    rmnet_inst : Rmnet instance

  RETURN VALUE
    TRUE: SUCCESS
    FALSE: FAILURE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_get_proc_id_from_inst
(
  rmnet_meta_sm_proc_id_e_type * proc_id,
  rmnet_instance_e_type          rmnet_inst
)
{
  uint32           tmp_proc_id = RMNET_PROC_ID_MAX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (proc_id == NULL)
  {
    LOG_MSG_ERROR_0("NULL proc_id address");
    return FALSE;
  }

  *proc_id = RMNET_PROC_ID_MAX;

  if (rmnet_inst >= RMNET_INSTANCE_MAX || rmnet_smi_info[rmnet_inst] == NULL)
  {
    LOG_MSG_ERROR_1("Invalid rmnet inst [%d]", rmnet_inst);
    return FALSE;
  }

  if (!rmnet_meta_smi_find_proc_id(&tmp_proc_id, rmnet_inst))
  {
    tmp_proc_id = RMNET_PROC_ID_APPS1;
  }
  *proc_id = (rmnet_meta_sm_proc_id_e_type)tmp_proc_id;

  LOG_MSG_INFO2_2("rmnet inst [%d] proc_id [%d]", rmnet_inst, *proc_id);
  return TRUE;

} /* rmnet_meta_sm_get_proc_id_from_inst() */


/*===========================================================================

                        INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_META_SMI_FIND_PROC_ID()

  DESCRIPTION
    Utility routine to get proc id. if SNI tlv has sent the call type as
    embedded, we would set proc id to embedded else we would set the proc
    id from sio port

  PARAMETERS
    sio_port
    proc_id
    rmnet_inst

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_smi_find_proc_id
(
  uint32                * proc_id,
  rmnet_instance_e_type   rmnet_inst
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( proc_id == NULL ||
       rmnet_inst >= RMNET_INSTANCE_MAX ||
       rmnet_smi_info[rmnet_inst] == NULL )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If call type tlv sent in SNI request is embedded then overwrite the 
    proc_id to embedded.
  -------------------------------------------------------------------------*/
  if ( RMNET_CALL_TYPE_EMBEDDED == 
              rmnet_smi_info[rmnet_inst]->meta_sm.net_params.call_type )
  {
    *proc_id = RMNET_PROC_ID_APPS1;
  }
  else
  {
    *proc_id = RMNET_PROC_ID_LAPTOP1;
  }

  LOG_MSG_INFO2_2("rmnet inst %d proc_id [%d]", rmnet_inst, *proc_id);

  return TRUE;

} /* rmnet_meta_smi_find_proc_id() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_XPORT_INIT()

  DESCRIPTION
    Initialize the rmnet xport structure

  PARAMETERS
    xport_info:  pointer to rmnet xport structure
    user_info:   user info ptr

  RETURN VALUE
    TRUE/FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_xport_init
(
  rmnet_xport_type        * xport_info,
  void                    * user_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (xport_info == NULL)
  {
    return FALSE;
  }

  memset(xport_info, 0, sizeof(rmnet_xport_type));

  /*-------------------------------------------------------------------------
    Watermark queues
  -------------------------------------------------------------------------*/
  dsm_queue_init( &xport_info->tx_wmk,
                  RMNET_TX_WM_DNE,
                  &xport_info->tx_wmk_q );

  dsm_queue_init( &xport_info->rx_wmk,
                  RMNET_RX_WM_DNE,
                  &xport_info->rx_wmk_q );

  /*-------------------------------------------------------------------------
    TX watermark
  -------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_3C_HSDPA
  xport_info->tx_wmk.lo_watermark    = 144000;
  xport_info->tx_wmk.hi_watermark    = 360000;
#else
  xport_info->tx_wmk.lo_watermark    = 8192;
  xport_info->tx_wmk.hi_watermark    = 65536;
#endif /* FEATURE_WCDMA_3C_HSDPA */

  xport_info->tx_wmk.dont_exceed_cnt = RMNET_TX_WM_DNE;
  xport_info->tx_wmk.dne_q_cnt       = RMNET_TX_WM_Q_CNT_DNE;

  xport_info->tx_wmk.lowater_func_ptr    = rmnet_meta_smi_tx_lowater_func;
  xport_info->tx_wmk.lowater_func_data   = xport_info;
  xport_info->tx_wmk.hiwater_func_ptr    = rmnet_meta_smi_tx_hiwater_func;
  xport_info->tx_wmk.hiwater_func_data   = xport_info;

  /*-------------------------------------------------------------------------
    RX watermark
  -------------------------------------------------------------------------*/
  xport_info->rx_wmk.lo_watermark    = RMNET_RX_WM_LOW;
  xport_info->rx_wmk.hi_watermark    = RMNET_RX_WM_HIGH;
  xport_info->rx_wmk.dont_exceed_cnt = RMNET_RX_WM_DNE;
  xport_info->rx_wmk.dne_q_cnt       = RMNET_RX_WM_Q_CNT_DNE;

  xport_info->rx_wmk.lowater_func_ptr    = rmnet_meta_smi_rx_lowater_func;
  xport_info->rx_wmk.lowater_func_data   = xport_info;
  xport_info->rx_wmk.hiwater_func_ptr    = rmnet_meta_smi_rx_hiwater_func;
  xport_info->rx_wmk.hiwater_func_data   = xport_info;
  xport_info->rx_wmk.non_empty_func_ptr  = rmnet_xport_default_rx_cb;

  /*-------------------------------------------------------------------------
    Callback functions
  -------------------------------------------------------------------------*/
  xport_info->in_call_cb = rmnet_meta_smi_in_call_cb;
  xport_info->ep_cfg_cb = rmnet_meta_smi_ep_cfg_done_cb;
  xport_info->dtr_cb = rmnet_meta_smi_dtr_cb;

  return TRUE;

} /* rmnet_meta_smi_xport_init() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_SET_DEFAULT_NET_PARAMS()

  DESCRIPTION
    This function sets the default rmnet parameters

  PARAMETERS
    info: rmnet_smi_info ptr

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
   RmNet instance is bound

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_set_default_net_params
(
  rmnet_smi_info_type  * info
)
{
  rmnet_efs_autoconnect_s_type  * autoconnect_config;
  uint16                          umts_default_profile_num = RMNET_EFS_INVALID_PROFILE;
  uint16                          cdma_default_profile_num = RMNET_EFS_INVALID_PROFILE;
  uint8                           subs_id = PS_SYS_DEFAULT_SUBS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (info == NULL)
  {
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If autoconnect is configured, use autoconnect configuration
  -------------------------------------------------------------------------*/
  autoconnect_config = rmnet_meta_smi_get_autoconnect_config(
                              RMNET_EP_ID(info),
                              RMNET_MUX_ID(info));
  if (autoconnect_config != NULL)
  {
    umts_default_profile_num = autoconnect_config->umts_profile;
    cdma_default_profile_num = autoconnect_config->cdma_profile;
    subs_id                  = autoconnect_config->subs_id;
  }

  /*-------------------------------------------------------------------------
    Determine default profiles
  -------------------------------------------------------------------------*/
  if (umts_default_profile_num == RMNET_EFS_INVALID_PROFILE)
  {
    rmnet_meta_smi_retrieve_default_profile_num(
                            DS_PROFILE_TECH_3GPP,
                            DS_PROFILE_3GPP_RMNET_PROFILE_FAMILY,
                            &umts_default_profile_num);
  }

  if (cdma_default_profile_num == RMNET_EFS_INVALID_PROFILE)
  {
    rmnet_meta_smi_retrieve_default_profile_num(
                            DS_PROFILE_TECH_3GPP2,
                            0,
                            &cdma_default_profile_num);
  }

  info->meta_sm.net_params.umts_profile_num = umts_default_profile_num;
  info->meta_sm.net_params.cdma_profile_num = (uint8)cdma_default_profile_num;
  info->meta_sm.net_params.tech_pref = WWAN_GROUP;
  info->meta_sm.net_params.subscription_id  = subs_id;

  return TRUE;

} /* rmnet_meta_smi_set_default_net_params() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_SET_DEFAULT_AUTOCONN_PARAMS()

  DESCRIPTION
    This function initialize the autoconnect settings 

  PARAMETERS
    info: rmnet_smi_info ptr

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
   RmNet instance is bound

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_set_default_autoconn_params
(
  rmnet_smi_info_type  * info
)
{
  rmnet_efs_autoconnect_s_type  * autoconnect_config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (RMNET_EP_ID(info) == 0)
  {
    // This could happen if all WDS clients are released while a
    // pending call is being torn down (rmnet unbind)
    return FALSE;
  }

  autoconnect_config = rmnet_meta_smi_get_autoconnect_config(
                           RMNET_EP_ID(info),
                           RMNET_MUX_ID(info));

  if (autoconnect_config != NULL)
  {
    if (autoconnect_config->autoconnect_setting == RMNET_AUTOCONNECT_ENABLED)
    {
      if (info == rmnet_smi_dual_ip_info[info->constants.dual_ip_inst]->info_v4)
      {
        info->meta_sm.net_params.ip_family_pref = QMI_AF_INET;
        info->meta_sm.settings.autoconnect_setting =
              (autoconnect_config->ip_type == QMI_AF_INET ||
               autoconnect_config->ip_type == QMI_AF_INET_4_6) ?
               RMNET_AUTOCONNECT_ENABLED : RMNET_AUTOCONNECT_DISABLED;
      }
      else if (info == rmnet_smi_dual_ip_info[info->constants.dual_ip_inst]->info_v6)
      {
        info->meta_sm.net_params.ip_family_pref = QMI_AF_INET6;
        info->meta_sm.settings.autoconnect_setting =
              (autoconnect_config->ip_type == QMI_AF_INET6 ||
               autoconnect_config->ip_type == QMI_AF_INET_4_6) ?
               RMNET_AUTOCONNECT_ENABLED : RMNET_AUTOCONNECT_DISABLED;
      }
    }
    else
    {
      // PAUSED
      info->meta_sm.settings.autoconnect_setting = autoconnect_config->autoconnect_setting;
    }

    info->meta_sm.settings.autoconnect_roam_setting =
      (rmnet_meta_sm_autoconnect_roam_setting_e_type)autoconnect_config->roam_setting;
  }
  else
  {
    info->meta_sm.settings.autoconnect_setting = RMNET_AUTOCONNECT_DISABLED;
  }

  /*-------------------------------------------------------------------------
    Determine the new autoconnect state
  -------------------------------------------------------------------------*/
  rmnet_meta_smi_update_autoconnect_state(info);

  return TRUE;

} /* rmnet_meta_smi_set_default_autoconn_params() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_INIT()

  DESCRIPTION
    This function initializes the state machine.

  PARAMETERS
    iface_ptr: Pointer to the interface which is used by this instance
               of Rm state machine to bring up pkt calls.

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_init
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type  * info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( instance >= RMNET_INSTANCE_MAX ||
              rmnet_smi_info[instance] != NULL )
  {
    ASSERT(0);
    return FALSE;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(info, sizeof(rmnet_smi_info_type),
                           rmnet_smi_info_type*);
  if (info == NULL)
  {
    return FALSE;
  }

  rmnet_smi_info[instance] = info;
  memset (info, 0, sizeof(rmnet_smi_info_type));
  info->meta_sm.state = RMNET_META_SM_NULL_STATE;

  /*-------------------------------------------------------------------------
    populate the constant values in rmnet_meta_sm_info based on the instance
  -------------------------------------------------------------------------*/
  info->constants.dual_ip_inst = instance / 2;
  info->constants.rmnet_inst = instance;

  /*-------------------------------------------------------------------------
    Initialize autoconnect settings
  -------------------------------------------------------------------------*/
  info->meta_sm.settings.auto_connect_enabled      = FALSE;
  info->meta_sm.settings.autoconnect_setting       = RMNET_AUTOCONNECT_DISABLED;
  info->meta_sm.settings.autoconnect_roam_setting  = RMNET_AUTOCONNECT_ROAM_SETTING_MAX;

  /*-------------------------------------------------------------------------
    Initialize manual DNS settings (IPv4 and IPv6)
  -------------------------------------------------------------------------*/
  info->meta_sm.settings.pri_dns_addr.type = IP_ADDR_INVALID;
  info->meta_sm.settings.pri_dns_addr.addr.v4 = 0;
  info->meta_sm.settings.sec_dns_addr.type = IP_ADDR_INVALID;
  info->meta_sm.settings.sec_dns_addr.addr.v4 = 0;

  info->meta_sm.settings.ipv6_pri_dns_addr.type = IP_ADDR_INVALID;
  info->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[0] = 0;
  info->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[1] = 0;
  info->meta_sm.settings.ipv6_sec_dns_addr.type = IP_ADDR_INVALID;
  info->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[0] = 0;
  info->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[1] = 0;

  /*-------------------------------------------------------------------------
    Initialize the ps_iface cback buf_ptrs for Um iface event handling
  -------------------------------------------------------------------------*/
// put the event bufs below into an array indexed by IFACE_EV of interest
  info->meta_sm.um_iface_active_out_of_use_buf_ptr
                                          = ps_iface_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_iface_ev_hdlr_cb,
                                             (void *) info
                                           );
  info->meta_sm.um_iface_up_buf_ptr       = ps_iface_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_iface_ev_hdlr_cb,
                                             (void *) info
                                           );
  info->meta_sm.um_iface_down_buf_ptr     = ps_iface_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_iface_ev_hdlr_cb,
                                             (void *) info
                                           );

  info->meta_sm.um_iface_authenticating_buf_ptr = ps_iface_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_iface_ev_hdlr_cb,
                                             (void *) info
                                           );

  info->meta_sm.um_iface_lingering_buf_ptr  = ps_iface_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_iface_ev_hdlr_cb,
                                             (void *) info
                                           );

  info->meta_sm.um_iface_bearer_tech_change_buf_ptr =
            ps_iface_alloc_event_cback_buf(
                                            rmnet_meta_smi_um_iface_ev_hdlr_cb,
                                            (void *) info
                                           );
  info->meta_sm.um_flow_enabled_buf_ptr   = ps_iface_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_iface_ev_hdlr_cb,
                                             (void *) info
                                           );

  info->meta_sm.um_flow_disabled_buf_ptr  = ps_iface_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_iface_ev_hdlr_cb,
                                             (void *) info
                                           );

  info->meta_sm.um_phys_link_flow_enabled_buf_ptr = ps_phys_link_alloc_event_cback_buf(
                                              rmnet_meta_smi_um_phys_link_ev_hdlr_cb,
                                              (void *)info
                                            );

  info->meta_sm.um_phys_link_flow_disabled_buf_ptr = ps_phys_link_alloc_event_cback_buf(
                                               rmnet_meta_smi_um_phys_link_ev_hdlr_cb,
                                               (void *)info
                                             );

  info->meta_sm.um_flow_tx_enabled_buf_ptr = ps_flow_alloc_event_cback_buf(
                                              rmnet_meta_smi_um_flow_ev_hdlr_cb,
                                              (void *)info
                                            );

  info->meta_sm.um_flow_tx_disabled_buf_ptr = ps_flow_alloc_event_cback_buf(
                                              rmnet_meta_smi_um_flow_ev_hdlr_cb,
                                              (void *)info
                                            );

  info->meta_sm.um_phys_link_up_buf_ptr   = ps_phys_link_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_phys_link_ev_hdlr_cb,
                                             (void *) info
                                           );

  info->meta_sm.um_phys_link_down_buf_ptr   = ps_phys_link_alloc_event_cback_buf(
                                             rmnet_meta_smi_um_phys_link_ev_hdlr_cb,
                                             (void *) info
                                           );
  /*-------------------------------------------------------------------------
    return false if any event buffer allocation failed
  -------------------------------------------------------------------------*/
  if ( ( info->meta_sm.um_iface_active_out_of_use_buf_ptr == NULL )     ||
       ( info->meta_sm.um_iface_down_buf_ptr == NULL )     ||
       ( info->meta_sm.um_iface_up_buf_ptr == NULL )       ||
       ( info->meta_sm.um_iface_authenticating_buf_ptr == NULL )     ||
       ( info->meta_sm.um_iface_lingering_buf_ptr == NULL )     ||
       ( info->meta_sm.um_flow_enabled_buf_ptr == NULL )     ||
       ( info->meta_sm.um_flow_disabled_buf_ptr == NULL )    ||
       ( info->meta_sm.um_phys_link_flow_enabled_buf_ptr == NULL )   ||
       ( info->meta_sm.um_phys_link_flow_disabled_buf_ptr == NULL ) ||
       ( info->meta_sm.um_flow_tx_enabled_buf_ptr == NULL ) ||
       ( info->meta_sm.um_flow_tx_disabled_buf_ptr == NULL) ||
       (info->meta_sm.um_iface_bearer_tech_change_buf_ptr == NULL) ||
       ( info->meta_sm.um_phys_link_up_buf_ptr == NULL )   ||
       ( info->meta_sm.um_phys_link_down_buf_ptr == NULL ))
  {
    LOG_MSG_ERROR_0("Event buf allocation failed");
    ASSERT(0);
  }

  /*-------------------------------------------------------------------------
    set the initial link protocol tx function to not use TLP, and register
    for notification when TLP setting changes.
  -------------------------------------------------------------------------*/
  info->meta_sm.llc_started  = FALSE;

  timer_def( &(info->meta_sm.autoconnect_timer),
             NULL,
             NULL,
             0,
             rmnet_meta_smi_autoconnect_timer_cback,
             (timer_cb_data_type)info );

  info->meta_sm.autoconnect_next_backoff_time = AUTOCONNECT_BACKOFF_TIMER_MIN_VALUE;

  /*-------------------------------------------------------------------------
    Initialize call end reason
  -------------------------------------------------------------------------*/
  info->meta_sm.call_end_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;

  /*-------------------------------------------------------------------------
    Initialize flag to monitor Iface teardown because of arbitration
  -------------------------------------------------------------------------*/
  info->meta_sm.arb_close_in_progress  = FALSE;
  info->meta_sm.proc_id                = RMNET_PROC_ID_MAX;

  /*-------------------------------------------------------------------------
    Initialize address configuration state machines.
  -------------------------------------------------------------------------*/
  rmnet_v4_sm_init(instance);

#ifdef FEATURE_DATA_PS_IPV6
  rmnet_v6_sm_init(instance);
#endif /* FEATURE_DATA_PS_IPV6 */

  return TRUE;

} /* rmnet_meta_smi_init() */



/*===========================================================================
  FUNCTION RMNET_META_SMI_IFACE_INIT()

  DESCRIPTION
    This function creats the Rmnet Ifaces and initializes them.

  PARAMETERS
    uint8: iface number

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_iface_init
(
  uint8 i
)
{
  rmnet_smi_iface_info_type *  info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(i < RMNET_IFACE_MAX);
  info = rmnet_smi_iface_info[i];
  if (info != NULL)
  {
    DATA_ERR_FATAL("Info not NULL");
    return FALSE;
  }

  // Allocate iface info structure
  PS_SYSTEM_HEAP_MEM_ALLOC(info, sizeof(rmnet_smi_iface_info_type),
                           rmnet_smi_iface_info_type*);
  if (info == NULL)
  {
    return FALSE;
  }

  memset(info, 0, sizeof(rmnet_smi_iface_info_type));
  rmnet_smi_iface_info[i] = info;

  /*-------------------------------------------------------------------------
    RmNet iface is created and managed by RmNetSm.  It is another instance
    of the (rm) SIO_IFACE.

    As SIO won't ever have more than a single physlink use '1' explicitly.

    ACLs are needed for DHCP server to resolve the RmNet interface during
    socket2() call w/ policy info.

    This is created before qmux_init so WDS can register iface callbacks on
    rmnet iface at initialization time.
  -------------------------------------------------------------------------*/
  info->rmnet_sio_acl.acl_fptr       = rmnet_sio_rt_acl;
  info->rmnet_sio_acl.post_proc_fptr = rmnet_sio_rt_acl_post_proc;
  if ( ps_iface_create(&info->rmnet_iface,
                       SIO_IFACE, //resolve bc_rm
                       &info->rmnet_sio_acl,      /* outgoing ACL */
                       NULL,                      /* incoming ACL */
                       &info->rmnet_pkt_phys_link,
                       1) == -1 )
  {
    LOG_MSG_ERROR_1("Failed to create rmnet iface [%d]", i);
    ASSERT(0);
    PS_SYSTEM_HEAP_MEM_FREE(rmnet_smi_iface_info[i]);
    return FALSE;
  }

  info->rmnet_iface.group_mask = (uint16)RM_GROUP;
  /* initialize ioctl handler */
  info->rmnet_iface.iface_ioctl_f_ptr = rmnet_iface_ioctl_hdlr;

  /*-------------------------------------------------------------------------
    Support logging on this interface
    - interface description is the default text (NULL)
  -------------------------------------------------------------------------*/
  if( i < RMNET_IFACE_MAX )
  {
    (void) ps_iface_dpl_support_network_logging(
             &info->rmnet_iface,
             dpl_ifname_by_rmnet_instance[i] );
    //add to info
     // MPDP - replace DPL ifname above by rmnet iface instnace
    (void) ps_iface_dpl_set_iface_desc(&info->rmnet_iface, NULL);

    (void) ps_iface_dpl_support_link_logging(
             &info->rmnet_iface,
             dpl_ifname_by_rmnet_instance[i] );
  }

  /*-------------------------------------------------------------------------
    Enable the RMNET interface
    bringup will fail as we don't register bring_up_cmd_hdlr
    bring up and tear down functions are not registered since sockets
    origination over SIO is not supported by this RMNET_META_SM.
  -------------------------------------------------------------------------*/
  ps_iface_enable_ind( &info->rmnet_iface );

  /*-------------------------------------------------------------------------
    Allow link layer broadcast addressing on this interface
  -------------------------------------------------------------------------*/
  PS_IFACE_SET_IS_BCAST_IFACE( &info->rmnet_iface, TRUE );

  /* Also init the lan_llc_instance and RX_Sig
     For these also (like ifaces) only a MAX of 5 are needed
     since only a max of 5 calls (3 unicast + 2 mcast) will be supported
     across 5 (A11) + 3 (laptop) Rmnet instances */

  switch(i)
  {
    case 0:
      info->constants.lan_llc_instance = LAN_LLE_RMNET1;
      break;

    case 1:
      info->constants.lan_llc_instance = LAN_LLE_RMNET1_2;
      break;

    case 2:
      info->constants.lan_llc_instance = LAN_LLE_RMNET2;
      break;

    case 3:
      info->constants.lan_llc_instance = LAN_LLE_RMNET2_2;
      break;

    case 4:
      info->constants.lan_llc_instance = LAN_LLE_RMNET3;
      break;

    case 5:
      info->constants.lan_llc_instance = LAN_LLE_RMNET3_2;
      break;

    case 6:
      info->constants.lan_llc_instance = LAN_LLE_RMNET4;
      break;

    case 7:
      info->constants.lan_llc_instance = LAN_LLE_RMNET4_2;
      break;
    case 8:
      info->constants.lan_llc_instance = LAN_LLE_RMNET5;
      break;

    case 9:
      info->constants.lan_llc_instance = LAN_LLE_RMNET5_2;
      break;

    case 10:
      info->constants.lan_llc_instance = LAN_LLE_RMNET6;
      break;

    case 11:
      info->constants.lan_llc_instance = LAN_LLE_RMNET6_2;
      break;

    case 12:
      info->constants.lan_llc_instance = LAN_LLE_RMNET7;
      break;

    case 13:
      info->constants.lan_llc_instance = LAN_LLE_RMNET7_2;
      break;

    case 14:
      info->constants.lan_llc_instance = LAN_LLE_RMNET8;
      break;

    case 15:
      info->constants.lan_llc_instance = LAN_LLE_RMNET8_2;
      break;

    default:
      LOG_MSG_ERROR_1("Unknown rmnet iface inst [%d]", i);
  }

  return TRUE;
}

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_RM_IFACE_PTR_FROM_INSTANCE

  DESCRIPTION
    This function returns the RM IFACE PTR allocated for a particular call on a
    particular instance.

  PARAMETERS
  instance: Rmnet Instance on which the call was established.

  RETURN VALUE
    Rm Iface Pointer.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type* rmnet_meta_sm_get_laptop_rm_iface_ptr
(
  rmnet_instance_e_type instance
)
{
  ASSERT( instance < RMNET_INSTANCE_MAX );

  return (rmnet_smi_info[instance] == NULL) ?
                NULL : rmnet_smi_info[instance]->rmnet_iface_ptr;

}/* rmnet_meta_smi_get_um_iface_ptr */


/*===========================================================================
  FUNCTION RMNET_META_SMI_GET_UM_IFACE_PTR


  DESCRIPTION
    This function retrieves the pointer to UM iface in the system which will
    be used to originate the packet data call.  The iface pointer is
    obtained by routing lookup.

    An UM iface is returned only if there is one available and it is not
    in IFACE_DOWN state.

    If the acl_pol_ptr is not NULL, then the acl policy information is
    also returned in the acl_pol_ptr parameter

  PARAMETERS
    net_params    - Parameters to build the acl policy
    rt_lk_up_type - Route lookup type option
    acl_pol_ptr   - If non-NULL, acl_policy is returned to the caller

  RETURN VALUE
    Pointer to allocated interface, or NULL if none available

  DEPENDENCIES
    Note that context lock is required because sockets calls originate
    in PS task context.  So we need to make sure that if we decide to
    originate on a particular iface, IFACE_state should be set to
    something other than DOWN and DISABLED and the UM packet iface
    ACLs should be disabled to prevent sockets call before
    freeing up the locks. This ensures only one type of call originates
    at a time on each interface.

  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type* rmnet_meta_smi_get_um_iface_ptr
(
  rmnet_meta_sm_network_info_type *   net_params,
  rmnet_meta_sm_route_look_up_e_type  rt_lk_up_type, /* bring_up/look up only */
  acl_policy_info_type            *   acl_pol_ptr,
  rmnet_meta_sm_arb_info_type     *   arb_info_ptr
)
{
  ps_iface_type *           um_iface_ptr;
  acl_policy_info_type      acl_policy_info;
//  ip_pkt_info_type          ip_info;
  uint32 iface_priority_mask = 0;
  uint8  arb_candidate = 0;
  uint64            tmp_app_id;
  uint32            proc_id = RMNET_PROC_ID_MAX;
  ps_iface_net_down_reason_type   net_down_reason;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Resolve the Um inteface
    make sure ROUTE_DEFAULT_DST is in network order (it's 0x00000000 for now)
  -------------------------------------------------------------------------*/

  memset( &acl_policy_info, 0, sizeof(acl_policy_info_type) );
  acl_init_policy_info(&acl_policy_info);

  acl_policy_info.policy_flag     = DSS_IFACE_POLICY_ANY;
  acl_policy_info.iface.kind      = DSS_IFACE_NAME;
  acl_policy_info.iface.info.name = DSS_IFACE_WWAN; //DSS_IFACE_ANY;

  switch(rt_lk_up_type)
  {
    case RMNET_RT_LK_UP_TYPE_BRINGUP:
      acl_policy_info.bring_up        = TRUE;
      break;

    case RMNET_RT_LK_UP_TYPE_IFACE:
      acl_policy_info.lookup_only = TRUE;
      break;

    case RMNET_RT_LK_UP_TYPE_DATAPATH:
      acl_policy_info.data_path_policy_flag = PS_ACL_POLICY_ROUTEABLE_ONLY;
      /* do nothing as both flags are supposed to be FALSE */
      break;

    default:
      ASSERT(0);
      return NULL;
  }

  switch(net_params->ip_family_pref)
  {
    case QMI_AF_ANY:     /* not supported currently */
    case QMI_AF_INET:
      acl_policy_info.ip_family       = IFACE_IPV4_ADDR_FAMILY;
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case QMI_AF_INET6:
      acl_policy_info.ip_family       = IFACE_IPV6_ADDR_FAMILY;
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    case QMI_AF_UNSPEC:
      acl_policy_info.ip_family       = IFACE_UNSPEC_ADDR_FAMILY;
      break;

    default:
      LOG_MSG_ERROR_1("Invalid RmNet IP family: %d!",
                      net_params->ip_family_pref);
      return NULL;
  }
  /*-------------------------------------------------------------------------
    TO DO - possibly move this in a function in wds
    When you reach here tech_pref WILL have a value from the enum
    ps_iface_name_enum_type
  -------------------------------------------------------------------------*/

  acl_policy_info.iface.info.name =
                            (dss_iface_name_enum_type) net_params->tech_pref;

  /* populate umts profile num: upper byte contains profile family and lower
     byte contains the profile index. Caller of this function needs to ensure
     that both bytes get populated appropriately */
  acl_policy_info.pdp_info = (acl_pdp_ctx_cfg_type)(uint32)
                                              (net_params->umts_profile_num);

  /*-----------------------------------------------------------------------
    copy APN info to policy for validation using route_get
  -----------------------------------------------------------------------*/
  acl_policy_info.apn.length = net_params->apn.length;
  memscpy(acl_policy_info.apn.name,
          net_params->apn.length,
         net_params->apn.name,
         net_params->apn.length);

  /* populate cdma profile number
  - default (0) or client provided value */
  acl_policy_info.data_session_profile_id = net_params->cdma_profile_num;

  /* Disabling Um iface ID overwriting for EPC and NAT */
#if !defined(FEATURE_EPC_HANDOFF)
  if(net_params->um_iface_id != 0) //if um iface handle is provided
  {
    if(ps_iface_get_handle(net_params->um_iface_id) == NULL)
    {
      LOG_MSG_INFO1_0("Invalid UM iface handle passed in Rmnet call bring up req");
      return NULL;
    }
    acl_policy_info.iface.info.id = net_params->um_iface_id;

    acl_policy_info.iface.kind      = DSS_IFACE_ID;
  }
#endif /* !(FEATURE_EPC_HANDOFF) */

#ifdef FEATURE_DATA_QMI_MCAST
  if ( (net_params->tech_pref == CDMA_BCAST_IFACE) ||
       (net_params->tech_pref == DVBH_IFACE) ||
       (net_params->tech_pref == MBMS_IFACE) ||
       (net_params->tech_pref == FLO_IFACE) )
  {
    net_params->is_mcast_iface = TRUE;
  }
#endif /* FEATURE_DATA_QMI_MCAST */

  if (net_params->tech_pref == EMBMS_IFACE)
  {
    net_params->is_embms_iface = TRUE;
  }

  proc_id = (net_params->call_type == RMNET_CALL_TYPE_EMBEDDED) ?
                     RMNET_PROC_ID_APPS1 : RMNET_PROC_ID_LAPTOP1;

  acl_policy_info.proc_id = (acl_proc_id_type)proc_id;
  if (proc_id == RMNET_PROC_ID_APPS1)
  {
    acl_policy_info.app_identifier = APP_ID_APPS;
  }
  else
  {
    acl_policy_info.app_identifier = APP_ID_TE_RMNET;
  }

  tmp_app_id = acl_policy_info.app_identifier;
  LOG_MSG_INFO1_2( "Iface lookup, app_id (upper32bits) [0x%x], proc_id [%d]",
                   (uint32)(tmp_app_id >> 32), acl_policy_info.proc_id );

  /*-------------------------------------------------------------------------
    Update subs id in the policy
  -------------------------------------------------------------------------*/
  acl_policy_info.subs_id         = net_params->subscription_id;
  acl_policy_info.apn_type        = net_params->apn_type;

  /*-------------------------------------------------------------------------
    Do the interface lookup using routing
  -------------------------------------------------------------------------*/
  um_iface_ptr = NULL;

  um_iface_ptr =  ps_route_look_up_iface_by_policy( &acl_policy_info,
                                                    &arb_candidate,
                                                    &iface_priority_mask,
                                                    &net_down_reason
                                                  );
  if( um_iface_ptr == NULL )
  {
    LOG_MSG_ERROR_0("look_up_iface_by_policy: none available");
    return NULL;
  }

  LOG_MSG_INFO1_3( "lookup: RmNet got Um Iface [0x%x:%x], prio mask [0x%x]",
                   (uint32)(um_iface_ptr->name),
                   (uint32)(um_iface_ptr->instance),
                   iface_priority_mask );
  LOG_MSG_INFO1_1( "lookup: is_arb [%d]", arb_candidate );

  arb_info_ptr->iface_priority_mask    = iface_priority_mask;
  arb_info_ptr->is_iface_arb_candidate = arb_candidate;

  if (acl_pol_ptr != NULL)
  {
    memscpy(acl_pol_ptr, 
            sizeof(acl_policy_info_type), 
            &acl_policy_info, 
            sizeof(acl_policy_info_type));
  }
  return um_iface_ptr;

} /* rmnet_meta_smi_get_um_iface_ptr() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_BRING_UP_UM_IFACE_BY_POLICY


  DESCRIPTION
    This function is used to bringup Iface by policy may involve arb.

    An UM iface is returned only if there is one available and it is not
    in IFACE_DOWN state.

  PARAMETERS
    net_params    - Parameters to build the acl policy
    if_ptr        - pointer to Um Iface
    rmnet_inst    - corresponding RmNet instance
    ps_errno      - pointer to errno generated when ret code is -1

  RETURN VALUE
    0  on SUCCESS
   -1  on FAILURE

  DEPENDENCIES

  SIDE EFFECTS
    None
===========================================================================*/
int rmnet_meta_smi_bring_up_um_iface_by_policy
(
  rmnet_meta_sm_network_info_type * net_params,
  ps_iface_type                  ** if_ptr,
  rmnet_instance_e_type             rmnet_inst,
  int16                           * ps_errno,
  ps_iface_net_down_reason_type   * net_down_reason
)
{
  ps_iface_type *           um_iface_ptr;
  acl_policy_info_type      acl_policy_info;
  int    result   = 0;
  uint64                    tmp_app_id;
  uint32                    proc_id = RMNET_PROC_ID_MAX;
  rmnet_smi_info_type      *info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(rmnet_inst < RMNET_INSTANCE_MAX);
  info = rmnet_smi_info[rmnet_inst];
  ASSERT(info);

  memset( &acl_policy_info, 0, sizeof(acl_policy_info_type) );
  acl_init_policy_info(&acl_policy_info);

  acl_policy_info.policy_flag     = DSS_IFACE_POLICY_ANY;
  acl_policy_info.iface.kind      = DSS_IFACE_NAME;
  acl_policy_info.iface.info.name = DSS_IFACE_WWAN; //DSS_IFACE_ANY;

  acl_policy_info.bring_up        = TRUE;
  acl_policy_info.subs_id         = net_params->subscription_id;
  acl_policy_info.apn_type        = net_params->apn_type;

  switch(net_params->ip_family_pref)
  {
    case QMI_AF_ANY:     /* not supported currently */
    case QMI_AF_INET:
      acl_policy_info.ip_family       = IFACE_IPV4_ADDR_FAMILY;
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case QMI_AF_INET6:
      acl_policy_info.ip_family       = IFACE_IPV6_ADDR_FAMILY;
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    case QMI_AF_UNSPEC:
      acl_policy_info.ip_family       = IFACE_UNSPEC_ADDR_FAMILY;
      break;

    default:
      LOG_MSG_ERROR_1("Invalid RmNet IP family: %d!",
                      net_params->ip_family_pref);
      *ps_errno = DS_EINVAL;
      return -1;
  }
  /*-------------------------------------------------------------------------
    TO DO - possibly move this in a function in wds
    When you reach here tech_pref WILL have a value from the enum
    ps_iface_name_enum_type
  -------------------------------------------------------------------------*/

  acl_policy_info.iface.info.name =
                            (dss_iface_name_enum_type) net_params->tech_pref;

  /* populate umts profile num: upper byte contains profile family and lower
     byte contains the profile index. Caller of this function needs to ensure
     that both bytes get populated appropriately */
  acl_policy_info.pdp_info = (acl_pdp_ctx_cfg_type)(uint32)
                                              (net_params->umts_profile_num);

  /*-----------------------------------------------------------------------
    copy APN info to policy
  -----------------------------------------------------------------------*/
  acl_policy_info.apn.length = net_params->apn.length;
  memscpy( acl_policy_info.apn.name,
           net_params->apn.length,
          net_params->apn.name,
          net_params->apn.length );

  /* populate cdma profile number
  - default (0) or client provided value */
  acl_policy_info.data_session_profile_id = net_params->cdma_profile_num;

  /* Disabling Um iface ID overwriting for EPC and NAT */
#if !defined(FEATURE_EPC_HANDOFF)
  if(net_params->um_iface_id != 0) //if um iface handle is provided
  {
    if(ps_iface_get_handle(net_params->um_iface_id) == NULL)
    {
      LOG_MSG_INFO1_0("Invalid UM iface handle passed in Rmnet call bring up req");
      *ps_errno = DS_EINVAL;
      return -1;
    }
    acl_policy_info.iface.info.id = net_params->um_iface_id;

    acl_policy_info.iface.kind    = DSS_IFACE_ID;
  }
#endif /* !(FEATURE_EPC_HANDOFF) */

#ifdef FEATURE_DATA_QMI_MCAST
  if ( (net_params->tech_pref == CDMA_BCAST_IFACE) ||
       (net_params->tech_pref == DVBH_IFACE) ||
       (net_params->tech_pref == MBMS_IFACE) ||
       (net_params->tech_pref == FLO_IFACE) )
  {
    net_params->is_mcast_iface = TRUE;
  }
#endif /* FEATURE_DATA_QMI_MCAST */

  if (net_params->tech_pref == EMBMS_IFACE)
  {
    net_params->is_embms_iface = TRUE;
  }

  LOG_MSG_INFO1_1( "Iface lookup, rmnet inst [%d]", rmnet_inst );

  if ( !rmnet_meta_smi_find_proc_id( &proc_id, rmnet_inst ) )
  {
    proc_id = RMNET_PROC_ID_APPS1;
    LOG_MSG_INFO2_0( "Unable to find proc_id, assume APPS/emb" );
  }

  acl_policy_info.proc_id = (acl_proc_id_type)proc_id;
  if (proc_id == RMNET_PROC_ID_APPS1)
  {
    acl_policy_info.app_identifier = APP_ID_APPS;
  }
  else
  {
    acl_policy_info.app_identifier = APP_ID_TE_RMNET;
  }

  /* Rm_inst_handle identifies the port on which the call is 
     being attempted, hence set the dual ip instance in the acl policy 
     structure. */
  acl_policy_info.rm_inst_handle = info->constants.dual_ip_inst;

  /* set proc_id in meta sm */
  info->meta_sm.proc_id = (rmnet_meta_sm_proc_id_e_type) acl_policy_info.proc_id;

  tmp_app_id = acl_policy_info.app_identifier;
  LOG_MSG_INFO1_3( "Iface lookup, rmnet inst [%d], app_id (upper32bits) [0x%x], proc_id [%d]",
                   rmnet_inst,
                   (uint32)(tmp_app_id>>32),
                   acl_policy_info.proc_id );

#ifdef FEATURE_DATA_LTE
  if(info->meta_sm.net_params.is_handoff_context_valid)
  {
    LOG_MSG_INFO1_0("Handoff context value set, copying info over to acl");
    acl_policy_info.handoff_ctxt = info->meta_sm.net_params.handoff_context;
    acl_policy_info.handoff_ctxt.handoff_class = PS_IFACE_HANDOFF_CLASS_EPC;
  }
#endif /* FEATURE_DATA_LTE */


  /*-------------------------------------------------------------------------
    Do the interface bring-up. This will involve arbitration.
  -------------------------------------------------------------------------*/
  um_iface_ptr = NULL;
  result =  ps_route_bring_up_iface_by_policy_ex( &acl_policy_info,
                                                  &um_iface_ptr,
                                                  ps_errno,
                                                 (void *)&net_params->cfg_info,
                                                  net_down_reason);
  *if_ptr = um_iface_ptr;

  /* very verbose */
  LOG_MSG_INFO1_2( "bringup: bring_up_iface_by_policy, ret [%d], errno [%d]",
                   result, *ps_errno );
  if ( result == 0 && um_iface_ptr != NULL )
  {
    LOG_MSG_INFO1_3( "bringup: RmNet inst %d got Um Iface [0x%x:%x]",
                     rmnet_inst,
                     (uint32)(um_iface_ptr->name),
                     (uint32)(um_iface_ptr->instance));
  }
  return result;
} /* rmnet_meta_smi_bring_up_um_iface_by_policy() */


/*===========================================================================
  FUNCTION RMNET_META_SMII_SET_MAC

  DESCRIPTION
    Set the specified ps_iface's MAC address to the specified value.

  PARAMETERS
    iface:        interface whose MAC address is updated
    hw_addr:      value of MAC address to be set
    hw_addr_len:  len of MAC address to be set

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
// TODO - make this a ps_iface_* function
static void rmnet_meta_smi_set_mac
(
  ps_iface_type *  iface,
  uint8 *          hw_addr,
  uint8            hw_addr_len
)
{
  uint8*           iface_hw_addr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(hw_addr_len <= PS_IFACE_HW_ADDR_LEN_MAX(iface) && 
           (hw_addr_len == LAN_IEEE_MAC_ADDR_LEN));

  iface_hw_addr = PS_IFACE_HW_ADDR_PTR( iface );

  if(NULL == iface_hw_addr)
  {
     DATA_ERR_FATAL("The Interface hardware address is NULL");
     return;
  }

  /*-------------------------------------------------------------------------
    Set the MAC address on the specified interface.  It is left-justified
    in memory (packed towards the start of HW addr buffer).
  -------------------------------------------------------------------------*/
  PS_IFACE_SET_HW_ADDR_LEN( iface, hw_addr_len );
  memset( iface_hw_addr, 0, PS_IFACE_HW_ADDR_LEN_MAX(iface));
  memscpy( iface_hw_addr , 
           hw_addr_len,
           hw_addr, 
           hw_addr_len );
} /* rmnet_meta_smi_set_mac() */



/*===========================================================================
  FUNCTION RMNET_META_SMI_REG_EVENTS()

  DESCRIPTION
    This function registers RmNet Meta SM for all common iface, flow, and
    phys link events.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    TRUE if success
    FALSE if failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_reg_events
(
  rmnet_smi_info_type *  info
)
{
  boolean is_err = FALSE;
  ps_iface_event_enum_type event = IFACE_MIN_EV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( info->um_iface_ptr == NULL )
  {
    LOG_MSG_ERROR_0("shldn't register Iface ev for NULL Iface" );
    return FALSE;
  }

  LOG_MSG_INFO2_0( "Registering all common iface, flow, and phys link events" );

  /*---------------------------------------------------------------------
    Register for desired events with Um_PKT_IFACE

    TODO - put the event bufs below into an array indexed by IFACE_EV of
           interest, then loop through the events.
  ---------------------------------------------------------------------*/
  do
  {
    if ( ps_iface_event_cback_reg
                           (
                             info->um_iface_ptr,
                             IFACE_ACTIVE_OUT_OF_USE_EV,
                             info->meta_sm.um_iface_active_out_of_use_buf_ptr
                           ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_ACTIVE_OUT_OF_USE_EV;
      break;
    }
  
    if ( ps_iface_event_cback_reg( info->um_iface_ptr,
                                   IFACE_UP_EV,
                                   info->meta_sm.um_iface_up_buf_ptr
                                 ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_UP_EV;
      break;
    }
  
    if ( ps_iface_event_cback_reg( info->um_iface_ptr,
                                   IFACE_DOWN_EV,
                                   info->meta_sm.um_iface_down_buf_ptr
                                 ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_DOWN_EV;
      break;
    }
  
    if ( ps_iface_event_cback_reg( info->um_iface_ptr,
                                   IFACE_AUTHENTICATING_EV,
                                   info->meta_sm.um_iface_authenticating_buf_ptr
                                 ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_AUTHENTICATING_EV;
      break;
    }
  
    if(ps_iface_event_cback_reg( info->um_iface_ptr,
                                   IFACE_LINGERING_EV,
                                   info->meta_sm.um_iface_lingering_buf_ptr
                                 ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_LINGERING_EV;
      break;
    }
  
    if(ps_iface_event_cback_reg(
                           info->um_iface_ptr,
  #ifdef FEATURE_DATA_WLAN_MAPCON
                           IFACE_BEARER_TECH_EX_CHANGED_EV,
                           info->meta_sm.um_iface_bearer_tech_change_buf_ptr
                           ) < 0)
    {
        event = IFACE_BEARER_TECH_EX_CHANGED_EV;
  #else
                           IFACE_BEARER_TECH_CHANGED_EV,
                           info->meta_sm.um_iface_bearer_tech_change_buf_ptr
                           ) < 0)
    {
        event = IFACE_BEARER_TECH_CHANGED_EV;
  #endif /* FEATURE_DATA_WLAN_MAPCON */
        is_err = TRUE;
        break;
    }
    if(ps_iface_event_cback_reg( info->um_iface_ptr,
                                 IFACE_FLOW_ENABLED_EV,
                                 info->meta_sm.um_flow_enabled_buf_ptr
                               ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_FLOW_ENABLED_EV;
      break;
    }
  
    if(ps_iface_event_cback_reg( info->um_iface_ptr,
                                 IFACE_FLOW_DISABLED_EV,
                                 info->meta_sm.um_flow_disabled_buf_ptr
                               ) < 0 )
    {
      is_err = TRUE;
      event = IFACE_FLOW_DISABLED_EV;
      break;
    }
  }while(0);
  if(is_err == TRUE)
  {
    LOG_MSG_ERROR_1("reg failed for event (%d)",event);
    return FALSE;
  }
  
  if ((RMNET_QOS_ENABLED(info) == FALSE) && (RMNET_TE_FC_ENABLED(info) == FALSE)) // i.e. Non-qos
  {
    if(ps_flow_event_cback_reg(
                             PS_IFACE_GET_DEFAULT_FLOW(info->um_iface_ptr),
                             FLOW_TX_ENABLED_EV,
                             info->meta_sm.um_flow_tx_enabled_buf_ptr) < 0)
    {
      LOG_MSG_ERROR_0("FLOW TX_ENABLED_EV reg failed" );
      return FALSE;
    }
 
    if(ps_flow_event_cback_reg(
                           PS_IFACE_GET_DEFAULT_FLOW(info->um_iface_ptr),
                           FLOW_TX_DISABLED_EV,
                           info->meta_sm.um_flow_tx_disabled_buf_ptr) < 0)
    {
      LOG_MSG_ERROR_0("FLOW TX_DISABLED_EV reg failed" );
      return FALSE;
    }
 
    info->meta_sm.default_flow_ev_reg = TRUE;
  }
 
  return TRUE;
} /* rmnet_meta_smi_reg_events() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_REG_PHYSLINK_EVENTS()

  DESCRIPTION
    This function registers RmNet Meta SM for phys link events.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    TRUE if success
    FALSE if failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_reg_physlink_events
(
  rmnet_smi_info_type *  info
)
{
  boolean is_err = FALSE;
  ps_iface_event_enum_type event = IFACE_MIN_EV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0( "Registering phys link events" );

  /*---------------------------------------------------------------------
     Cache the um iface phys link
     - to be able to access phys link ptr even it is unbound from um iface
       in the case of the phys link dynamic binding
  ----------------------------------------------------------------------*/
  info->meta_sm.um_iface_phys_link_ptr =
                                 PS_IFACE_GET_PHYS_LINK(info->um_iface_ptr);

  if (info->meta_sm.um_iface_phys_link_ptr == NULL)
  {
    LOG_MSG_ERROR_0( "Physlink ptr is NULL, don't register for physlink events" );
    return FALSE;
  }

  /*---------------------------------------------------------------------
   * The phys link here is primary phys link. There is no need to
   * register phys link flow enable/disable event if QoS is enabled.
  ----------------------------------------------------------------------*/
  do
  {
    if ((RMNET_QOS_ENABLED(info) == FALSE) && (RMNET_TE_FC_ENABLED(info) == FALSE))
    {
      if(ps_phys_link_event_cback_reg(
                                info->meta_sm.um_iface_phys_link_ptr,
                                PHYS_LINK_FLOW_ENABLED_EV,
                                info->meta_sm.um_phys_link_flow_enabled_buf_ptr
                              ) < 0 )
      {
        is_err = TRUE;
        event = PHYS_LINK_FLOW_ENABLED_EV;
        break;
      }
  
      if(ps_phys_link_event_cback_reg(
                                info->meta_sm.um_iface_phys_link_ptr,
                                PHYS_LINK_FLOW_DISABLED_EV,
                                info->meta_sm.um_phys_link_flow_disabled_buf_ptr
                              ) < 0)
      {
        is_err = TRUE;
        event = PHYS_LINK_FLOW_DISABLED_EV;
        break;
      }
    }
  
    if(ps_phys_link_event_cback_reg(
                                info->meta_sm.um_iface_phys_link_ptr,
                                PHYS_LINK_UP_EV,
                                info->meta_sm.um_phys_link_up_buf_ptr
                              ) < 0)
    {
      is_err = TRUE;
      event = PHYS_LINK_UP_EV;
      break;
    }
  
    if(ps_phys_link_event_cback_reg(
                                info->meta_sm.um_iface_phys_link_ptr,
                                PHYS_LINK_DOWN_EV,
                                info->meta_sm.um_phys_link_down_buf_ptr
                              ) < 0)
    {
      is_err = TRUE;
      event = PHYS_LINK_DOWN_EV;
      break;
    }
  }while(0);
  if(is_err == TRUE)
  {
    LOG_MSG_ERROR_1("reg failed for evt = (%d)",event );
    return FALSE;
  }
  
  return TRUE;
} /* rmnet_meta_smi_reg_physlink_events() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_DEREG_PHYSLINK_EVENTS()

  DESCRIPTION
    This function de-egisters RmNet Meta SM for phys link events.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_dereg_physlink_events
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0( "De-registering phys link events" );

  /*---------------------------------------------------------------------
    Since de-reg is called after phys link is un-bound from the iface,
    access the un-bound phys link ptr through cache.
  ----------------------------------------------------------------------*/
  if (info->meta_sm.um_iface_phys_link_ptr != NULL)
  {
    if ((RMNET_QOS_ENABLED(info) == FALSE) && (RMNET_TE_FC_ENABLED(info) == FALSE))
    {
      ps_phys_link_event_cback_dereg(
                              info->meta_sm.um_iface_phys_link_ptr,
                              PHYS_LINK_FLOW_ENABLED_EV,
                              info->meta_sm.um_phys_link_flow_enabled_buf_ptr
                              );

      ps_phys_link_event_cback_dereg(
                              info->meta_sm.um_iface_phys_link_ptr,
                              PHYS_LINK_FLOW_DISABLED_EV,
                              info->meta_sm.um_phys_link_flow_disabled_buf_ptr
                            );
    }

    ps_phys_link_event_cback_dereg(
                              info->meta_sm.um_iface_phys_link_ptr,
                              PHYS_LINK_UP_EV,
                              info->meta_sm.um_phys_link_up_buf_ptr
                              );

    ps_phys_link_event_cback_dereg(
                              info->meta_sm.um_iface_phys_link_ptr,
                              PHYS_LINK_DOWN_EV,
                              info->meta_sm.um_phys_link_down_buf_ptr
                            );

    info->meta_sm.um_iface_phys_link_ptr = NULL;
  }
  else
  {
    LOG_MSG_ERROR_0( "Physlink ptr is NULL, don't de-reg for physlink events" );
  }
} /* rmnet_meta_smi_dereg_physlink_events() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_DEREG_EVENTS()

  DESCRIPTION
    This function deregisters RmNet Meta SM for all common iface, flow, and
    phys link events.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_dereg_events
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( info->um_iface_ptr == NULL )
  {
    LOG_MSG_ERROR_0("shldn't deregister Iface ev for NULL Iface" );
    return;
  }

  LOG_MSG_INFO2_0( "Deregistering all common iface, flow, and phys link events" );

  ps_iface_event_cback_dereg(
                          info->um_iface_ptr,
                          IFACE_ACTIVE_OUT_OF_USE_EV,
                          info->meta_sm.um_iface_active_out_of_use_buf_ptr );

  ps_iface_event_cback_dereg( info->um_iface_ptr,
                              IFACE_UP_EV,
                              info->meta_sm.um_iface_up_buf_ptr );

  ps_iface_event_cback_dereg( info->um_iface_ptr,
                              IFACE_DOWN_EV,
                              info->meta_sm.um_iface_down_buf_ptr );

  ps_iface_event_cback_dereg( info->um_iface_ptr,
                              IFACE_AUTHENTICATING_EV,
                              info->meta_sm.um_iface_authenticating_buf_ptr );

  ps_iface_event_cback_dereg( info->um_iface_ptr,
                              IFACE_LINGERING_EV,
                              info->meta_sm.um_iface_lingering_buf_ptr );

  ps_iface_event_cback_dereg( info->um_iface_ptr,
#ifdef FEATURE_DATA_WLAN_MAPCON
                              IFACE_BEARER_TECH_EX_CHANGED_EV,
#else
                              IFACE_BEARER_TECH_CHANGED_EV,
#endif /* FEATURE_DATA_WLAN_MAPCON */
                              info->meta_sm.um_iface_bearer_tech_change_buf_ptr );

  ps_iface_event_cback_dereg( info->um_iface_ptr,
                              IFACE_FLOW_ENABLED_EV,
                              info->meta_sm.um_flow_enabled_buf_ptr );

  ps_iface_event_cback_dereg( info->um_iface_ptr,
                              IFACE_FLOW_DISABLED_EV,
                              info->meta_sm.um_flow_disabled_buf_ptr );

  rmnet_meta_smi_dereg_physlink_events(info);

  if(info->meta_sm.default_flow_ev_reg == TRUE)
  {
    ps_flow_event_cback_dereg(
                         PS_IFACE_GET_DEFAULT_FLOW(info->um_iface_ptr),
                         FLOW_TX_ENABLED_EV,
                         info->meta_sm.um_flow_tx_enabled_buf_ptr);
    ps_flow_event_cback_dereg(
                         PS_IFACE_GET_DEFAULT_FLOW(info->um_iface_ptr),
                         FLOW_TX_DISABLED_EV,
                         info->meta_sm.um_flow_tx_disabled_buf_ptr);
    info->meta_sm.default_flow_ev_reg = FALSE;
  }
} /* rmnet_meta_smi_dereg_events() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_START_ADDR_CFG_SM()

  DESCRIPTION
    This function initiates the interface specific address configuration
    state machine determined by the Um iface received by the routing lookup.

    The supported address cfg state machines are:

    1) IPv4
    2) IPv6
    3) Multicast

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
     0 if success
    -1 if failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static int rmnet_meta_smi_start_addr_cfg_sm
(
  rmnet_smi_info_type *  info
)
{
  lan_llc_start_info_type  start_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------
    Initialize start info to zero.
  ---------------------------------------------------------------------*/
  memset (&start_info, 0, sizeof(start_info));

  /*-------------------------------------------------------------------------
    Check if this is a mcast instance or not
  -------------------------------------------------------------------------*/
  if( (info->meta_sm.net_params.is_mcast_iface == TRUE) ||
      (info->meta_sm.net_params.is_embms_iface == TRUE) )
  {
    /*-----------------------------------------------------------------------
      Since no addr config post addr config event
    -----------------------------------------------------------------------*/
    rmnet_meta_sm_post_event(RMNET_META_SM_INFO_TO_INSTANCE(info),
                             RMNET_META_SM_ADDR_CFG_COMPLETE_EV);

    /*-----------------------------------------------------------------------
      Post configuring on Rm as we are skipping addr config for mcast case
    -----------------------------------------------------------------------*/
    (void) ps_iface_configuring_ind(info->rmnet_iface_ptr);
  }
  else /* unicast */
  {
    if(ps_iface_addr_family_is_v4(info->um_iface_ptr))
    {
      rmnet_v4_sm_init_config(info, &start_info);

      rmnet_v4_sm_post_event(RMNET_META_SM_INFO_TO_INSTANCE(info),
                             RMNET_V4_SM_START_ADDR_CFG_EV);
    }
#ifdef FEATURE_DATA_PS_IPV6
    else if(ps_iface_addr_family_is_v6(info->um_iface_ptr))
    {
      //TO DO - mcast for IPv6
      rmnet_v6_sm_init_config(info, &start_info);

      rmnet_v6_sm_post_event(RMNET_META_SM_INFO_TO_INSTANCE(info),
                             RMNET_V6_SM_START_ADDR_CFG_EV);
    }
#endif /* FEATURE_DATA_PS_IPV6 */
    else
    {
      LOG_MSG_ERROR_0("Unsupported interface type!");
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    If the interface is configured in IP only mode, do not start LAN LLC.
  -------------------------------------------------------------------------*/
  switch (RMNET_LINK_PROT(info))
  {
    case RMNET_IP_MODE:
      break;

    case RMNET_ETHERNET_MODE:
      /*---------------------------------------------------------------------------
        Init LAN LLC with the appropriate RmNet callbacks.
      ---------------------------------------------------------------------------*/
      start_info.iface_ptr       = info->rmnet_iface_ptr;
      start_info.lle_mode        = LAN_LLC_MODE_0;
      start_info.lle_rx_sig      = 0;
      start_info.rx_f_ptr        = NULL;
      start_info.rx_f_user_data  = NULL;
      start_info.tx_f_ptr        = rmnet_meta_smi_llc_tx_data;
      start_info.tx_f_user_data  = info->xport_info;

      (void)lan_llc_start( info->constants.lan_llc_instance, &start_info );

      info->meta_sm.llc_started = TRUE;

      /*---------------------------------------------------------------------------
        Allocate RNDIS hdr to pushdown on DL Ethernet packets
      ---------------------------------------------------------------------------*/
      if (RMNET_DL_DATA_AGG(info) == RMNET_ENABLE_DATA_AGG_RNDIS)
      {

        rmnet_meta_smi_init_rndis_eth_hdr(info);

      }
      else
      {
        PS_SYSTEM_HEAP_MEM_ALLOC(info->meta_sm.eth_header, 
                                 sizeof(llc_frm_packed_mode_zero_type),
                                 llc_frm_packed_mode_zero_type*);
        if (info->meta_sm.eth_header == NULL)
        {
          return -1;
        }
      }
      break;

    default:
      LOG_MSG_ERROR_1("Default:link_prot %d", RMNET_LINK_PROT(info));
      return -1;
  }

  return 0;
} /* rmnet_meta_smi_start_addr_cfg_sm() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_STOP_ADDR_CFG_SM()

  DESCRIPTION
    This function terminates the address configuration state machine
    determined by the Um iface received selected during the routing lookup.

    The supported address cfg state machines are:

    1) IPv4
    2) IPv6
    3) Multicast

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_stop_addr_cfg_sm
(
  rmnet_smi_info_type *  info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
      /*---------------------------------------------------------------------
        Stop the address configuration state machine(s). DO NOT do this for
        UM_CONFIGURING_STATE as we have not yet initialized the sub state
        machine.
      ---------------------------------------------------------------------*/
      if(ps_iface_addr_family_is_v4(info->um_iface_ptr))
      {
        rmnet_v4_sm_post_event(RMNET_META_SM_INFO_TO_INSTANCE(info),
                               RMNET_V4_SM_STOP_ADDR_CFG_EV);
      }
#ifdef FEATURE_DATA_PS_IPV6
      else if(ps_iface_addr_family_is_v6(info->um_iface_ptr))
      {
        rmnet_v6_sm_post_event(RMNET_META_SM_INFO_TO_INSTANCE(info),
                               RMNET_V6_SM_STOP_ADDR_CFG_EV);
      }
#endif /* FEATURE_DATA_PS_IPV6 */
      else
      {
        LOG_MSG_ERROR_0("Unsupported interface type!");
      }

      break;

    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    case RMNET_META_SM_NULL_STATE:
      /* Do nothing since the sub SM has not been started. */
      break;

    default:
    {
      DATA_ERR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_stop_addr_cfg_sm() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_RETRIEVE_DEFAULT_PROFILE_NUM()

  DESCRIPTION
  Get the default profile number for tech type.

  PARAMETERS
   profile_type (IN)  : Profile Type (3GPP/3GPP2)
   profile_num  (OUT) : Default Profile Number

  RETURN VALUE
   None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_retrieve_default_profile_num
(
  ds_profile_tech_etype        profile_type,
  uint8                        profile_family,
  uint16                      *default_profile_num
)
{
  ds_profile_status_etype           profile_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  *default_profile_num = 0;

  /*-------------------------------------------------------------------------
    Call the API to get the default profile number.
  -------------------------------------------------------------------------*/
  profile_status = ds_profile_get_default_profile_num(profile_type,
                                                      (uint32)profile_family,
                                                      default_profile_num);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS &&
      profile_status != DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE)
  {
    LOG_MSG_ERROR_2 ("Get Default Profile for type %d failed. Error %d",
                     profile_type, profile_status);
  }

  return;

} /* rmnet_meta_smi_retrieve_default_profile_num() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_RM_DEV_START_PKT_EV()

  DESCRIPTION
    This function processes RMNET_META_SM_RM_DEV_START_PKT_EV event.

    This event indicates that the tethered device wants to start a packet
    data session.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_rm_dev_start_pkt_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd RM_DEV_START_PKT_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);
#else
  if( info->meta_sm.state > RMNET_META_SM_MIN_STATE && 
      info->meta_sm.state < RMNET_META_SM_MAX_STATE )
  {
    MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                  "Recvd RM_DEV_START_PKT_EV, RMNET inst %d, state '%s'\n",
                  rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
  }
  else
  {
    LOG_MSG_ERROR_1("Recvd EP_CFG_DONE_EV, invalid state %d", 
                     info->meta_sm.state );
  }
#endif

    /* clear autoconnect timer, as we are starting the call */
  (void) timer_clr( &(info->meta_sm.autoconnect_timer), T_NONE );

  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Reset flow control status.
      ---------------------------------------------------------------------*/
      PS_ENTER_CRIT_SECTION(&rmnet_crit_section);
      info->rx_fc_mask = 0;
      PS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

      /*---------------------------------------------------------------------
        Transition to UM_CONFIGURING state.
      ---------------------------------------------------------------------*/
      rmnet_meta_smi_transition_state(info, RMNET_META_SM_UM_CONFIGURING_STATE);
      break;
    }
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Unexpected RM_DEV_START_PKT_EV in %d state",
                      info->meta_sm.state);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_rm_dev_start_pkt_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_RM_LINK_DOWN_EV

  DESCRIPTION
    This function processes RMNET_META_SM_RM_LINK_DOWN_EV event.

    This event indicates that the tethered device is closing the packet data
    session, and so the RMNET_META_SM should release associated packet sessions.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_rm_link_down_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd RM_LINK_DOWN_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);
#else
  if( info->meta_sm.state > RMNET_META_SM_MIN_STATE && 
      info->meta_sm.state < RMNET_META_SM_MAX_STATE )
  {
    MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                  "Recvd RM_LINK_DOWN_EV, RMNET inst %d, state '%s'\n",
                  rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
  }
  else
  {
    LOG_MSG_ERROR_1("Recvd EP_CFG_DONE_EV, invalid state %d", 
                     info->meta_sm.state );
  }
#endif

  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      /* Rm bailed during call setup/handoff, fall through */
      LOG_MSG_INFO2_0("Call setup/reconfig failed");

      if(info->meta_sm.dtr_is_high == TRUE)
      {
        rmnet_sm_ext_call_failure();
      }
    }

    /* fall through */
    case RMNET_META_SM_NET_IF_UP_STATE:
    {
      /* Reset the arb flag. If this flag is still set, um iface_down_ev
       * will be converted to um iface_down_arb_ev, and be rejected in
       * rmnet_meta_sm_waiting_for_um_down state */
      info->meta_sm.arb_close_in_progress = FALSE;

      rmnet_meta_smi_transition_state(info, RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE);
      break;
    }

    case RMNET_META_SM_NULL_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO1_1("Ignoring RM_LINK_DOWN_EV in %d state",
                      info->meta_sm.state);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_rm_link_down_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_UM_IFACE_DOWN_EV

  DESCRIPTION
    This function processes RMNET_META_SM_UM_IFACE_DOWN_EV event.

    This event indicates that network has closed the packet data session,
    and so the RMNET_META_SM should release the tethered packet session.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_um_iface_down_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type             rmnet_inst;
  ps_dpm_rm_dereg_iface_info_type   dereg_info;
  int32                             result;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd UM_IFACE_DOWN_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd UM_IFACE_DOWN_EV, RMNET inst %d, state '%s'\n",
                rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
#endif

  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
      /* update the tx rx bytes for this call before rmnet iface goes down */
      if((E_SUCCESS != ps_stat_get_iface(PS_STAT_IFACE_BYTES_RX, info->rmnet_iface_ptr, &info->meta_sm.last_call_rx_bytes, 8)) ||
         (E_SUCCESS != ps_stat_get_iface(PS_STAT_IFACE_BYTES_TX, info->rmnet_iface_ptr, &info->meta_sm.last_call_tx_bytes, 8)))
      {
        MSG_ERROR("ps_iface_stat failure while caching byte stats for rmnet call",0,0,0);
        info->meta_sm.last_call_rx_bytes = 0;
        info->meta_sm.last_call_tx_bytes = 0;
      }
      
      /*---------------------------------------------------------------------
        Delete any mbim filters associated with the call.
      ---------------------------------------------------------------------*/  
        rmnet_utils_packet_filter_handle_iface_down
        (
        info->meta_sm.net_params.packet_filter_id,
          info->rmnet_iface_ptr
        );
                                                
      /*---------------------------------------------------------------------
           Un Register with DPM
         ---------------------------------------------------------------------*/      
      if (info->dpm_rm_registered)
      {
        dereg_info.sio_stream_id = info->xport_info->stream_id;
        dereg_info.rm_iface_ptr = info->rmnet_iface_ptr;
      
        result =  ps_dpm_rm_dereg_iface(&dereg_info);
        if(DSS_SUCCESS != result)
        {
          LOG_MSG_ERROR_1("DPM deregistration unsuccessful %d", result);
          ASSERT(0);
        }
        info->dpm_rm_registered = FALSE;
      }
      
      rmnet_meta_smi_transition_state(info, RMNET_META_SM_NULL_STATE);
      break;

    case RMNET_META_SM_NULL_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Unexpected UM_IFACE_DOWN_EV in %d state",
                      info->meta_sm.state);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_um_iface_down_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_UM_IFACE_UP_EV

  DESCRIPTION
    This function processes RMNET_META_SM_UM_IFACE_UP_EV event.

    This event indicates that network has closed the packet data session,
    and so the RMNET_META_SM should release the tethered packet session.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_um_iface_up_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd UM_IFACE_UP_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state );
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd UM_IFACE_UP_EV, RMNET inst %d, state '%s'\n",
                rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
#endif

  /*-------------------------------------------------------------------------
    Um interface controller has [re]established Um interface and put it
    in up state.  Generate routeable indication on that (Um) interface
    to indicate that the address is in use by us.
  -------------------------------------------------------------------------*/
  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    {
      if( ps_iface_state(info->um_iface_ptr) == IFACE_UP ||
          ps_iface_state(info->um_iface_ptr) == IFACE_ROUTEABLE )
      {
      /*---------------------------------------------------------------------
        In dynamic binding (e.g LTE), physlink is bound to iface after iface
        is UP. Physlink pointer will be NULL before the binding. For ifaces
        with static binding, physlink ev registration still happens before
        any data transfer begins i.e. before moving to Rm configuring state.
        Flow enabled event will not be missed because the event is sent right
        after event registration.
      ---------------------------------------------------------------------*/
        LOG_MSG_INFO1_1( "um iface ptr name 0x%x", info->um_iface_ptr->name );

        if ( rmnet_meta_smi_reg_physlink_events(info) == TRUE )
        {
          ps_iface_routeable_ind( info->um_iface_ptr );
          if (RMNET_XPORT_EP_CONFIGURING(info->xport_info))
          {
            LOG_MSG_INFO1_0( "Iface up ev, transition to EP_CONFIGURING_STATE" );
            rmnet_meta_smi_transition_state (info,
                                           RMNET_META_SM_EP_CONFIGURING_STATE);
          }
          else
          {
            LOG_MSG_INFO1_0( "Iface up ev, transition to RM_CONFIGURING_STATE" );
            rmnet_meta_smi_transition_state (info,
                                           RMNET_META_SM_RM_CONFIGURING_STATE);
          }
        }
        else
        {
          rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                    RMNET_META_SM_UM_IFACE_DOWN_EV );
        }
      }
      else
      {
        LOG_MSG_INFO2_0("UM iface not UP, Ignore UP_EV! let Iface Down ev do clean up");
      }
      break;
    }

    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      if(ps_iface_state(info->um_iface_ptr) == IFACE_UP)
      {
        ps_iface_routeable_ind( info->um_iface_ptr );
      }
      else
      {
        LOG_MSG_INFO2_0("UM iface not UP, Ignore UP_EV! let Iface Down ev do clean up");
      }
      break;
    }

    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:  /* Um out of sync w/ RmSm */
    {
      if (info->meta_sm.restore_um_up)
      {
        LOG_MSG_INFO2_0 ("Um interface restored to UP as before rmnet call");
        break;
      }
    }

    /* fall through */
    case RMNET_META_SM_NULL_STATE:         /* event should be deregistered */
    default:
    {
      LOG_MSG_ERROR_1("Invalid RmNETSM state (%d)!", info->meta_sm.state);
      //ASSERT(0);
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_um_iface_up_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_ADDR_CFG_COMPLETE_EV

  DESCRIPTION
    This function processes RMNET_META_SM_ADDR_CFG_COMPLETE_EV event.

    This event indicates that address configuration on the Rm has completed
    successfully. The call is now up and data can be transferred.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_addr_cfg_complete_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd ADDR_CFG_COMPLETE_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd ADDR_CFG_COMPLETE_EV, RMNET inst %d, state '%s'\n",
                rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
#endif

  switch( info->meta_sm.state )
  {

    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    {
      rmnet_meta_smi_transition_state(info, RMNET_META_SM_NET_IF_UP_STATE);
      break;
    }

    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    case RMNET_META_SM_NULL_STATE:
    {
      /*-----------------------------------------------------------------------
        Ignore in this state. Scenario: disconnect occurs from the ext device
        at the same time that address cfg completes in the sub state
        machine. When LINK_DOWN event is processed meta sm goes to waiting for
        Um down and then to NULL, and once sub sm comes up it posts
        ADDR_CFG_COMPLETE in this state. Sub sm is cleaned up in um iface down
        processing.
      -----------------------------------------------------------------------*/
      LOG_MSG_INFO1_1("Unexpected ADDR_CFG_COMPLETE_EV in %d state",
                      info->meta_sm.state);
      break;
    }

    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Unexpected ADDR_CFG_COMPLETE_EV in %d state",
                      info->meta_sm.state);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_addr_cfg_complete_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_ADDR_CFG_FAILURE_EV

  DESCRIPTION
    This function processes RMNET_META_SM_ADDR_CFG_FAILURE_EV event.

    This event indicates that Rm failed to properly complete its address
    configuration. Possible causes include DHCP (for IPv4 calls) failing to
    start or the guard timer expiring.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_addr_cfg_failure_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd ADDR_CFG_FAILURE_EV, RMNET inst %d state %d",
                   rmnet_inst, info->meta_sm.state);
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd ADDR_CFG_FAILURE_EV, RMNET inst %d, state '%s'\n",
                rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
#endif

  switch( info->meta_sm.state )
  {

    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    {
      rmnet_meta_smi_transition_state(info,
                                      RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE);
      break;
    }

    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    {
      /* Do Nothing */
      LOG_MSG_INFO1_0("ADDR_CFG_FAILURE_EV received in WAITING_FOR_UM_DOWN_STATE");
      break;
    }

    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    case RMNET_META_SM_NULL_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Unexpected ADDR_CFG_FAILURE_EV in %d state",
                      info->meta_sm.state);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_addr_cfg_failure_ev() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_ADDR_CFG_CHANGED_EV

  DESCRIPTION
    This function processes RMNET_META_SM_ADDR_CFG_CHANGED_EV event.

    This event indicates that Um has changed addresses and now the Rm
    is in the process of reconfiguring its address.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_addr_cfg_changed_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd ADDR_CFG_CHANGED_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state);
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd ADDR_CFG_CHANGED_EV, RMNET inst %d, state '%s'\n",
                rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
#endif

  switch( info->meta_sm.state )
  {

    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    {
      rmnet_meta_smi_transition_state(info,
                                      RMNET_META_SM_RM_RECONFIGURING_STATE);
      break;
    }

    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    case RMNET_META_SM_NULL_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Unexpected ADDR_CFG_CHANGED_EV in %d state",
                      info->meta_sm.state);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_addr_cfg_changed_ev() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_UM_IFACE_AUTHENTICATING_EV

  DESCRIPTION
    This function processes RMNET_SM_UM_IFACE_AUTHENTICATING_EV event.

    This event indicates that authentication process is in progress,
    and so the RMNET_SM should send an authenticating event on rm iface.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_um_iface_authenticating_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd UM_IFACE_AUTHENTICATING_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state );
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd UM_IFACE_AUTHENTICATING_EV, RMNET inst %d, state '%s'\n",
                rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
#endif

  /*-------------------------------------------------------------------------
    Um interface controller has put Um interface in authenticating state.
    Generate authenticating indication on (Rm) interface
  -------------------------------------------------------------------------*/
  ps_iface_generic_ind( info->rmnet_iface_ptr, IFACE_AUTHENTICATING_EV, NULL );

} /* rmnet_meta_smi_handle_um_iface_authenticating_ev() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_UM_IFACE_BEARER_TECH_CHANGED_EV

  DESCRIPTION
    This function processes
RMNET_META_SMI_HANDLE_UM_IFACE_BEARER_TECH_CHANGED_EV event.

    This event indicates that bearer tech has changed update the phys links
accordingly.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_um_iface_bearer_tech_changed_ev
(
  rmnet_smi_info_type *  info
)
{
  ps_iface_type *base_um_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update phys links where there is an active call */
  if( info->meta_sm.state != RMNET_META_SM_NULL_STATE )
  {
    rmnet_meta_smi_update_physlink(RMNET_META_SM_INFO_TO_INSTANCE(info));

    if ( PS_IFACE_IS_VALID(info->um_iface_ptr) &&
         info->meta_sm.state == RMNET_META_SM_NET_IF_UP_STATE &&
         info->meta_sm.softap_mode == FALSE)
    {
      /* Reset the optimized DL path for the new base Um iface.
       * The older Um reset is taken care by framework.
       * Do this only if RmNet call is up, If its coming up
       * the RmNet SM would take care of setting this path.
       * */
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      #ifdef FEATURE_DATA_PS_464XLAT
        base_um_iface_ptr = PS_DPM_GET_BASE_IFACE(info->um_iface_ptr);
      #else
        base_um_iface_ptr = PS_IFACE_GET_BASE_IFACE(info->um_iface_ptr);
      #endif /* FEATURE_DATA_PS_464XLAT */
      PS_IFACEI_SET_OPT_RM_IFACE(base_um_iface_ptr, info->rmnet_iface_ptr);
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_INFO1_2("RmNet Bearer Tech Change Event: "
                      "Set DL opt path for iface0x%x:%d",
                      info->um_iface_ptr->name, info->um_iface_ptr->instance);
    }
  }
}/* rmnet_meta_smi_handle_um_iface_bearer_tech_changed_ev */

/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_UM_IFACE_ARB_EV

  DESCRIPTION
    This function processes RMNET_SM_UM_IFACE_ARB_EV event.

    This event indicates that arbitration process is in progress.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_um_iface_arb_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd UM_IFACE_ARB_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state );
#else
  MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "Recvd UM_IFACE_ARB_EV, RMNET inst %d, state '%s'\n",
                rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
#endif

  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
      rmnet_meta_smi_transition_state(info, RMNET_META_SM_WAITING_FOR_UM_ARB_STATE);
      break;

    case RMNET_META_SM_RM_CONFIGURING_STATE:
    case RMNET_META_SM_NET_IF_UP_STATE:
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    case RMNET_META_SM_NULL_STATE:
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      /*---------------------------------------------------------------------
        Its error to get this event in all other states.
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Unexpected UM_IFACE_ARB_EV in %d state",
                      info->meta_sm.state);
      break;
    }

    default:
    {
      DATA_ERR_FATAL("Bad RmNetSM state!");
      break;
    }
  } /* switch( info->meta_sm.state ) */
} /* rmnet_meta_smi_handle_um_iface_arb_ev() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_HANDLE_EP_CFG_DONE_EV

  DESCRIPTION
    This function processes RMNET_SM_PORT_EP_CFG_DONE_EV event.

  PARAMETERS
    info - pointer to the interface controller's control block

  RETURN VALUE
    None

  DEPENDENCIES
    must !ONLY! be called from command dispatcher
    - state and initialization of the control block is verified there

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_handle_ep_cfg_done_ev
(
  rmnet_smi_info_type *  info
)
{
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

#ifdef T_ARM
  LOG_MSG_INFO2_2( "Recvd EP_CFG_DONE_EV, RMNET inst %d, state %d",
                   rmnet_inst, info->meta_sm.state );
#else
  if( info->meta_sm.state > RMNET_META_SM_MIN_STATE && 
      info->meta_sm.state < RMNET_META_SM_MAX_STATE )
  {
    MSG_SPRINTF_2(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                  "Recvd EP_CFG_DONE_EV, RMNET inst %d, state '%s'\n",
                  rmnet_inst, rmnet_meta_smi_state_names[info->meta_sm.state] );
  }
  else
  {
    LOG_MSG_ERROR_1("Recvd EP_CFG_DONE_EV, invalid state %d", 
                     info->meta_sm.state );
  }
#endif

  switch( info->meta_sm.state )
  {
    case RMNET_META_SM_EP_CONFIGURING_STATE:
      rmnet_meta_smi_transition_state(
                     info, RMNET_META_SM_RM_CONFIGURING_STATE);
      break;

    default:
      break;
  } 

} /* rmnet_meta_smi_handle_ep_cfg_done_ev() */


/*===========================================================================
FUNCTION   RMNET_META_SMI_IFACE_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on Um interface.  Various Um iface events are registered based on
  call type during the call setup type.  Each iface event generates
  appropriate rmnet_meta_sm event .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rmnet_meta_smi_um_iface_ev_hdlr_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  rmnet_smi_info_type *  info;
  rmnet_instance_e_type      rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data_ptr);

  info = (rmnet_smi_info_type *) user_data_ptr;
  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  LOG_MSG_INFO2_3( "Recvd iface ev %d from 1x i/f 0x%x, RMNET inst %d",
                   event, this_iface_ptr, rmnet_inst );

  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/

  switch( event )
  {
    case IFACE_FLOW_ENABLED_EV:
    {
      /*---------------------------------------------------------------------
        For network model calls send signal to PS to resume pulling PPP
        packets from SIO rx_wm, laptop flow will be enabled by rx_wm lo
        water func.
      ---------------------------------------------------------------------*/
      break;
    }

    case IFACE_FLOW_DISABLED_EV:
    {
      /*---------------------------------------------------------------------
        For rmnet calls we don't need to do anything when UM flow is
        disabled, rx function will stop pulling PPP packets from SIO rx_wm.
        rx_wm hi water function will disable laptop flow.
      ---------------------------------------------------------------------*/
      break;
    }

    case IFACE_LINGERING_EV:
    case IFACE_ACTIVE_OUT_OF_USE_EV:
    case IFACE_DOWN_EV:
    {
      /*---------------------------------------------------------------------
        UM iface is down. This event can also be generated when we register
        for events since that happens during call setup ie UM is in DOWN
        state. So check the previous state before taking any action. Also
        initiate the teardown of the SUB_SM, ensuring it is cleaned up
        prior to Um tear down and the um_iface_ptr being cleared from the
        RmNet instance.
        If iface down is because of arbitration, post IFACE_ARB_EV so that
        SM moves to new state to handle this special case.
      ---------------------------------------------------------------------*/
      if( info->meta_sm.arb_close_in_progress )
      {
        LOG_MSG_INFO1_1( "IFACE_DOWN_EV during arb, reason [%d], posting UM_IFACE_DOWN_ARB_EV",
                         event_info.iface_down_info.netdown_reason );
        rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_UM_IFACE_DOWN_ARB_EV );
      }
      else
      {
        info->meta_sm.call_end_reason =
          event_info.iface_down_info.netdown_reason;

        LOG_MSG_INFO1_1( "Um iface down, reason [%d], posting RMNET_META_SM_UM_IFACE_DOWN_EV",
                         event_info.iface_down_info.netdown_reason );

        rmnet_meta_smi_stop_addr_cfg_sm(info);

        rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_UM_IFACE_DOWN_EV );
      }

      break;
    }

    case IFACE_UP_EV:
    {
      /*---------------------------------------------------------------------
        Traffic channel on UM has come up. This event is used for Relay and
        network model call setup.
      ---------------------------------------------------------------------*/
      rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                RMNET_META_SM_UM_IFACE_UP_EV );
      break;
    }

    case IFACE_AUTHENTICATING_EV:
    {
      /*---------------------------------------------------------------------
        Authentication in progress
      ---------------------------------------------------------------------*/
      rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                RMNET_META_SM_UM_IFACE_AUTHENTICATING_EV );
      break;
    }

#ifdef FEATURE_DATA_WLAN_MAPCON
    case IFACE_BEARER_TECH_EX_CHANGED_EV:
#else
    case IFACE_BEARER_TECH_CHANGED_EV:
#endif /* FEATURE_DATA_WLAN_MAPCON */
    {
      rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                RMNET_META_SM_UM_IFACE_BEARER_TECH_CHANGED_EV
                              );
          break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Unregistered event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */
} /* rmnet_meta_smi_um_iface_ev_hdlr_cb() */



/*===========================================================================
FUNCTION   RMNET_META_SMI_UM_PHYS_LINK_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_phys_link events
  occur on Um interface.  Various Um iface events are registered based on
  call type during the call setup type.  Each iface event generates
  appropriate rmnet_meta_sm event .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rmnet_meta_smi_um_phys_link_ev_hdlr_cb
(
  ps_phys_link_type             *this_phys_link_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                          *user_data_ptr
)
{
  rmnet_smi_info_type *  info;
  rmnet_instance_e_type          rmnet_inst;
  rmnet_smi_cmd_type   * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data_ptr);

  info = (rmnet_smi_info_type *) user_data_ptr;
  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  LOG_MSG_INFO2_3( "Recvd phys link ev %d from phys_link ptr 0x%x, RMNET inst %d",
                   event, this_phys_link_ptr, rmnet_inst);

  /*-------------------------------------------------------------------------
    Act on change in phys link flow control
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case PHYS_LINK_FLOW_ENABLED_EV:
    {
      /*---------------------------------------------------------------------
        For network model calls send signal to PS to resume pulling LLC
        packets from SIO rx_wm, laptop flow will later be enabled by rx_wm
        low water callback.
      ---------------------------------------------------------------------*/
      break;
    }

    case PHYS_LINK_FLOW_DISABLED_EV:
    {
      /*---------------------------------------------------------------------
        For rmnet calls we don't need to do anything when UM flow is
        disabled, rx function will stop pulling PPP packets from SIO rx_wm.
        (laptop flow is disabled only when rx_wm hi water is reached)
      ---------------------------------------------------------------------*/
      break;
    }

    case PHYS_LINK_UP_EV:
    case PHYS_LINK_DOWN_EV:
    {
       /*-------------------------------------------------------------------------
          Get a DCC command buffer
       -------------------------------------------------------------------------*/
       cmd_ptr = (rmnet_smi_cmd_type *)RMNET_SMI_GET_CMD_BUF_ASSERT(DCC_RMNET_SM_CMD);
       if (cmd_ptr == NULL)
       {
          return;
       }

       /*-------------------------------------------------------------------------
          Post cmd to DS task.
       -------------------------------------------------------------------------*/
       cmd_ptr->sm_id     = RMNET_META_SM;
       cmd_ptr->cmd_type  = RMNET_SM_CMD_PROCESS_PHYS_LINK_UP_DOWN_EVT;
       cmd_ptr->info_ptr  = (void *) info;
       cmd_ptr->data.phys_link_status.phys_link_ptr = this_phys_link_ptr;
       cmd_ptr->data.phys_link_status.event         = event;
       cmd_ptr->data.phys_link_status.event_info    = event_info;
       cmd_ptr->data.phys_link_status.user_data_ptr = user_data_ptr;

       dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr);
       break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Unregistered event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */
} /* rmnet_meta_smi_um_phys_link_ev_hdlr_cb() */



/*===========================================================================
FUNCTION   RMNET_META_SMI_UM_FLOW_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_phys_link events
  occur on Um interface.  Various Um iface events are registered based on
  call type during the call setup type.  Each iface event generates
  appropriate rmnet_meta_sm event .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rmnet_meta_smi_um_flow_ev_hdlr_cb
(
  ps_flow_type                *this_flow_ptr,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                        *user_data_ptr
)
{
  rmnet_smi_info_type *  info;
  rmnet_instance_e_type        rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data_ptr);

  info = (rmnet_smi_info_type *) user_data_ptr;
  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  LOG_MSG_INFO2_3( "Recvd flow ev %d from flow ptr 0x%x, RMNET inst %d",
                   event, this_flow_ptr, rmnet_inst );

  /*-------------------------------------------------------------------------
    Act on change in phys link flow control
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case FLOW_TX_ENABLED_EV:
    {
      /*---------------------------------------------------------------------
        For network model calls send signal to PS to resume pulling LLC
        packets from SIO rx_wm, laptop flow will later be enabled by rx_wm
        low water callback.
      ---------------------------------------------------------------------*/
      break;
    }

    case FLOW_TX_DISABLED_EV:
    {
      /*---------------------------------------------------------------------
        For rmnet calls we don't need to do anything when UM flow is
        disabled, rx function will stop pulling PPP packets from SIO rx_wm.
        (laptop flow is disabled only when rx_wm hi water is reached)
      ---------------------------------------------------------------------*/
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Unregistered event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */
} /* rmnet_meta_smi_um_flow_ev_hdlr_cb() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_PROCESS_PHYS_LINK_UP_DOWN_EVENT()

  DESCRIPTION
    This function is called from DCC task and is used to process phys link
    events on UM iface. RMNET_META_SMI_UM_PHYS_LINK_EV_HDLR_CB() posts a
    RMNET_SM_CMD to handle these events in DCC task. This function generates
    DIAG event with Rm watermark levels on PHYS_LINK/PHYS_LINK_DOWN events.

  PARAMETERS
    this_phys_link_ptr:  Pointer to Physlink
    event: The event that is being posted to the state machine.
    event_info    : info for the event
    user_data_ptr : user data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_process_phys_link_up_down_event
(
  ps_phys_link_type            *this_phys_link_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_data_ptr
)
{
  rmnet_smi_info_type *  info;
  rmnet_instance_e_type          rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (user_data_ptr && this_phys_link_ptr);

  info = (rmnet_smi_info_type *) user_data_ptr;
  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  LOG_MSG_INFO2_3( "Recvd phys link ev %d from phys_link ptr 0x%x, RMNET inst %d",
                   event, this_phys_link_ptr, rmnet_inst);

  switch( event )
  {
    case PHYS_LINK_UP_EV:
    {
     /*---------------------------------------------------------------------
       Generate wm levels DIAG event for phys_link_up_ev
      ---------------------------------------------------------------------*/
      rmnet_meta_smi_log_wmk_levels(rmnet_inst, this_phys_link_ptr,
                                     PS_WMK_LOGGING_ACTIVE);
      break;
    }

    case PHYS_LINK_DOWN_EV:
    {
     /*---------------------------------------------------------------------
       Generate wm levels DIAG event for phys_link_down_ev
      ---------------------------------------------------------------------*/
      rmnet_meta_smi_log_wmk_levels(rmnet_inst, this_phys_link_ptr,
                                    PS_WMK_LOGGING_DORMANT);

      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Unregistered event %d recvd, ignoring", event);
      break;
    }
  } /* switch( event ) */

} /* rmnet_meta_smi_process_phys_link_up_down_event */

/*===========================================================================
  FUNCTION RMNET_META_SMI_AUTCONNECT_TIMER_CBACK()

  DESCRIPTION
    Autoconnect retry timer callback

  PARAMETERS
    user_data -> rmnet_smi_info

  RETURN VALUE
    None

  DEPENDENCIES
    rmnet_sio_init() must have been called previously

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_autoconnect_timer_cback
(
  timer_cb_data_type  user_data
)
{
  rmnet_smi_cmd_type   * cmd_ptr;
  rmnet_smi_info_type  * info;

  ASSERT (user_data);
  info = (rmnet_smi_info_type *) user_data;
  /*-------------------------------------------------------------------------
    Get a DCC command buffer
  -------------------------------------------------------------------------*/
  cmd_ptr = (rmnet_smi_cmd_type *)RMNET_SMI_GET_CMD_BUF_ASSERT(DCC_RMNET_SM_CMD);
  if (cmd_ptr == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Post cmd to DS task.
  -------------------------------------------------------------------------*/
  cmd_ptr->sm_id     = RMNET_META_SM;
  cmd_ptr->cmd_type  = RMNET_SM_CMD_AUTOCONNECT_TIMER;
  cmd_ptr->info_ptr  = (void *) info;

  dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr);
} /* rmnet_meta_smi_autoconnect_timer_cback() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_PROCESS_AUTCONNECT_TIMER_CMD_HDLR()

  DESCRIPTION
    Autoconnect retry timer command handler

  PARAMETERS
    user_data -> rmnet_smi_info

  RETURN VALUE
    None

  DEPENDENCIES
    rmnet_sio_init() must have been called previously

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_process_autoconnect_timer_cmd_hdlr
(
  void              *user_data
)
{
  rmnet_smi_info_type           * info;
  rmnet_efs_autoconnect_s_type  * autoconnect_config;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  info = (rmnet_smi_info_type *) user_data;
  ASSERT (info);

  LOG_MSG_INFO1_2(" Autoconnect: Timer expiry for EP 0x%x autoconnect config %d",
                  RMNET_EP_ID(info), info->meta_sm.settings.auto_connect_enabled);

  autoconnect_config = rmnet_meta_smi_get_autoconnect_config(
                          RMNET_EP_ID(info),
                          RMNET_MUX_ID(info));
  if (autoconnect_config == NULL ||
      autoconnect_config->autoconnect_setting != RMNET_AUTOCONNECT_ENABLED)
  {
    LOG_MSG_ERROR_0("Autoconnect not enabled");
    return;
  }

  if(info == rmnet_smi_dual_ip_info[info->constants.dual_ip_inst]->info_v4 )
  {
    if (autoconnect_config->ip_type == QMI_AF_INET6)
    {
      LOG_MSG_INFO1_0("EFS:IPV6 ONLY; exiting from V4 timer expiry");
      return;
    }
  }
  else if(info == rmnet_smi_dual_ip_info[info->constants.dual_ip_inst]->info_v6 )
  {
    if (autoconnect_config->ip_type == QMI_AF_INET)
    {
      LOG_MSG_INFO1_0("EFS:IPV4 ONLY; exiting from V6 timer expiry");
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Post dev_start_pkt ev only if dtr is high (as it could have gone low
    between timer start and expiry)
  -------------------------------------------------------------------------*/

  if( (info->meta_sm.settings.auto_connect_enabled) &&
      (rmnet_meta_smi_verify_dtr_high(info) == TRUE) )
  {
    LOG_MSG_INFO1_0("autoconnect retry timer expired.");
    rmnet_meta_smi_handle_rm_dev_start_pkt_ev(info);
  }
  else
  {
    (void) timer_clr( &(info->meta_sm.autoconnect_timer), T_NONE );
    info->meta_sm.autoconnect_next_backoff_time = AUTOCONNECT_BACKOFF_TIMER_MIN_VALUE;
    (void) timer_set( &(info->meta_sm.autoconnect_timer),
                        info->meta_sm.autoconnect_next_backoff_time, 0, T_SEC );
  }
} /* rmnet_meta_smi_process_autoconnect_timer_cmd_hdlr */


/*===========================================================================
  FUNCTION RMNET_META_SMI_TRANSITION_STATE()

  DESCRIPTION
    This function performs state transition actions.

  PARAMETERS
    new_state: the state to transition to

  RETURN VALUE
    None

  DEPENDENCIES
    Its the caller's responsibility that it initiates only valid state
    transitions. This function does not check if a state transition is
    valid or not.

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_transition_state
(
  rmnet_smi_info_type      *  info,
  rmnet_meta_smi_state_type   new_state
)
{
  int16                       ps_iface_errno = 0;
  byte                        rm_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  ps_iface_event_info_u_type  event_info;
  void * net_info_cfg_ptr;
  int retval;
  uint8    i;
  rmnet_instance_e_type  rmnet_inst;
#ifdef FEATURE_DATA_RMNET_USB_STANDARD_ECM
  sio_ioctl_param_type ioctl_param;
#endif
#ifdef FEATURE_QXDM_WM_TRACING
  uint32                       call_id = 0;
  sint15                       ioctl_errno = 0;
#endif /* FEATURE_QXDM_WM_TRACING */
  rmnet_meta_sm_arb_info_type     arb_info;
  ps_iface_type         *base_um_iface_ptr;
  rmnet_smi_dual_ip_info_type  * dual_ip_info = NULL;
  ps_iface_net_down_reason_type  down_reason = PS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
  ps_dpm_rm_reg_iface_info_type  reg_info;
  int16                          ps_errno;
  int32                          result;
  rmnet_meta_sm_call_e_type      rm_call_type;
  ps_iface_down_mh_event_info_type  down_event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset( &arb_info, 0, sizeof(rmnet_meta_sm_arb_info_type) );

  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);
  dual_ip_info = rmnet_smi_dual_ip_info[info->constants.dual_ip_inst];
  ASSERT(dual_ip_info); 

#ifdef T_ARM
  LOG_MSG_INFO2_3( "RMNET_META_SM inst %d, state goes %d to %d",
                   rmnet_inst, info->meta_sm.state, new_state);
#else
  MSG_SPRINTF_3(MSG_SSID_DS_PS, MSG_LEGACY_MED,
                "RMNET_META_SM inst %d, state goes '%s' to '%s'\n",
                rmnet_inst,
                rmnet_meta_smi_state_names[info->meta_sm.state],
                rmnet_meta_smi_state_names[new_state] );
#endif

  ds_qmi_cflog_meta_sm_state_change( (uint32) info->meta_sm.state,
                                     (uint32) new_state );

  memset( &event_info, 0, sizeof(event_info) );
  info->meta_sm.state = new_state;

  /*-------------------------------------------------------------------------
    Perform action associated with entering into new state.
  -------------------------------------------------------------------------*/
  switch( new_state )
  {
    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_NULL_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_NULL_STATE:
    {
      PS_ENTER_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);
      info->has_flow = FALSE;

      dual_ip_info->rx_flow_count =
             dual_ip_info->info_v4->has_flow +
             dual_ip_info->info_v6->has_flow;

      /*---------------------------------------------------------------------
        Pkt call is terminating, Perform all the necessary cleanup
      ---------------------------------------------------------------------*/
      if(info->meta_sm.llc_started)
      {
        (void) lan_llc_stop( info->constants.lan_llc_instance );
        info->meta_sm.llc_started = FALSE;
      }

#ifdef FEATURE_QXDM_WM_TRACING
      /*---------------------------------------------------------------------
        Deregister for Watermark tracing.
      ---------------------------------------------------------------------*/
      fc_post_deregister_cmd_wm_trace(info->call_id, RMNET_CLIENT);
      info->call_id = 0;
#endif /* FEATURE_QXDM_WM_TRACING */

      /*clear WM queue only if this is last SM to go down*/
      if( 0 == dual_ip_info->rx_flow_count )
      {
        rmnet_logical_xport_clear_wmk_queues(info->xport_info);
      }
      /*Retrieve the call type before clearing net_params structure */
      rm_call_type = info->meta_sm.net_params.call_type;
      /*---------------------------------------------------------------------
        Set the Um interface bringup preferences back to default values
        before the next session
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DS_MULTIPLE_NAI
    #ifdef FEATURE_DS_MOBILE_IP
      mip_reg_ses_info_cb( NULL );
    #endif
#endif      
      memset( &(info->meta_sm.net_params),
              0x0,
              sizeof(rmnet_meta_sm_network_info_type) );

      info->meta_sm.net_params.call_type = rm_call_type;
      rmnet_meta_smi_set_default_net_params(info);
      rmnet_meta_smi_set_default_autoconn_params(info);

#ifdef FEATURE_DATA_PS_ADDR_MGMT
      if(info->rmnet_iface_ptr && 
         ps_iface_addr_family_is_v6(info->rmnet_iface_ptr))
      {
        /*-------------------------------------------------------------------------
	      Send Deprecate RA for v6 iface.
        -------------------------------------------------------------------------*/
        ps_iface_addr_mgmt_deprecate_prefix(info->rmnet_iface_ptr);
      }
#endif /* FEATURE_DATA_PS_ADDR_MGMT */


      /*---------------------------------------------------------------------
        Unbridge Rm and Um interfaces
      ---------------------------------------------------------------------*/
      if (info->rmnet_iface_ptr)
      {
        ps_iface_set_bridge( info->rmnet_iface_ptr, NULL );
      }
      if (info->um_iface_ptr)
      {
        rex_enter_crit_sect(&lan_llc_inst_to_um_iface_ptr_crit_sec);
        lan_llc_inst_to_um_iface_ptr_lut[info->constants.lan_llc_instance] = NULL;
        rex_leave_crit_sect(&lan_llc_inst_to_um_iface_ptr_crit_sec);

        ps_iface_set_bridge( info->um_iface_ptr, NULL );

        PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
        #ifdef FEATURE_DATA_PS_464XLAT
          base_um_iface_ptr = PS_DPM_GET_BASE_IFACE(info->um_iface_ptr);
        #else
          base_um_iface_ptr = PS_IFACE_GET_BASE_IFACE(info->um_iface_ptr);
        #endif /* FEATURE_DATA_PS_464XLAT */
        PS_IFACEI_SET_OPT_RM_IFACE(base_um_iface_ptr, NULL);
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        LOG_MSG_INFO1_2("RmNet NULL state: Reset DL opt path for iface0x%x:%d",
                        info->um_iface_ptr->name,
                        info->um_iface_ptr->instance);

        /*-------------------------------------------------------------------
          Deregister all the Um interface, flow, and phys link event callbacks
        -------------------------------------------------------------------*/
        rmnet_meta_smi_dereg_events(info);

        /*-------------------------------------------------------------------
          Unbind from the Um interface
        -------------------------------------------------------------------*/
        info->um_iface_ptr = NULL;
        info->um_iface_handle = 0;
      }
      /*---------------------------------------------------------------------
        By default, don't restore resolved Um interface to UP after RmNet
        call teardown.  We'll query the Um state before calling bringup and
        save the previous state in this restore_um_up variable if this
        needs to be done.
      ---------------------------------------------------------------------*/
      info->meta_sm.restore_um_up = FALSE;

      /*---------------------------------------------------------------------
        Check if RM is down due to 2 non-dual IP rmnets sharing the same PDN
      ---------------------------------------------------------------------*/
      if (info->net_down_2_rmnet)
      {
        info->meta_sm.call_end_reason =
                     PS_NET_DOWN_REASON_INTERNAL_CALL_ALREADY_PRESENT;
        info->net_down_2_rmnet = FALSE;
      }

      /*---------------------------------------------------------------------
        Rm iface is down - Indicate that we are not in call anymore
      ---------------------------------------------------------------------*/
      if (info->rmnet_iface_ptr)
      {
        ps_phys_link_gone_ind(PS_IFACE_GET_PHYS_LINK(info->rmnet_iface_ptr));
        
        memset(&down_event_info, 0, sizeof(down_event_info));
        down_event_info.down_reason = info->meta_sm.call_end_reason;
        down_event_info.bearer_tech.rat_value =  DS_SYS_RAT_UNKNOWN;
        ps_iface_down_ind(info->rmnet_iface_ptr,
                          &down_event_info);
        /*-------------------------------------------------------------
          Clear the powersave mode filters installed, if any
        ----------------------------------------------------------------*/
        ps_iface_ipfltr_delete( info->rmnet_iface_ptr, IP_FLTR_CLIENT_POWERSAVE,
                                PS_IFACE_IPFLTR_DELETE_ALL_HANDLE, &ps_errno );
      }

    #ifdef FEATURE_DATA_RMNET_USB_STANDARD_ECM
      /* send notification only if this is last SM to go down*/
      if( 0 == dual_ip_info->rx_flow_count )
      {
        LOG_MSG_INFO2_0("Sending Link DOWN notification to ECM driver");
        rmnet_sio_link_up_notify(info->meta_sm.rmnet_sio_handle,
                                 FALSE); //link_DOWN
      }
    #endif
      if (info->rmnet_iface_ptr != NULL)
      {
        /* clear the RmNet iface to Rmnet instance binding */
        for (i = 0; i < RMNET_IFACE_MAX; i++)
        {
          if ( (rmnet_smi_iface_info[i] != NULL) &&
               (&rmnet_smi_iface_info[i]->rmnet_iface == info->rmnet_iface_ptr) )
          {
            break;
          }
        }

        ASSERT((i < RMNET_IFACE_MAX) && (rmnet_smi_iface_info[i]->in_use == TRUE));
        rmnet_smi_iface_info[i]->in_use = FALSE;
      }

      /* make the RmNet iface, llc_inst, rx_sig available for use
         by other rmnet instances */
      info->rmnet_iface_ptr = NULL;
      info->constants.lan_llc_instance = LAN_LLE_MIN;

      info->meta_sm.call_end_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;

      /* reset arb related flag */
      info->meta_sm.arb_close_in_progress = FALSE;

      /* deregister RX function only if this is last SM to go down*/
      if( 0 == dual_ip_info->rx_flow_count )
      {
        /*---------------------------------------------------------------------
          Deregister rmnet_sio rx function
        ---------------------------------------------------------------------*/
        rmnet_logical_xport_set_rx_cb( info->xport_info, NULL, info );

        if (info->meta_sm.proc_id == RMNET_PROC_ID_LAPTOP1)
        {
          rmnet_phys_xport_dec_teth_call_cnt(dual_ip_info->xport_info.phys_xport);
        }
        /*---------------------------------------------------------------------
          Close RmNet xport if no call is active
        ---------------------------------------------------------------------*/
        if (rmnet_meta_smi_in_call_cb(dual_ip_info) == FALSE)
        {
          ds_qmap_clean_cmd(info->xport_info);
          rmnet_logical_xport_close(&dual_ip_info->xport_info);
        }
      }

      info->meta_sm.proc_id               = RMNET_PROC_ID_MAX;

      // Update qcrmcall state
      rmnet_meta_smi_update_qcrmcall_state(info);

      /*---------------------------------------------------------------------
        Restart the autoconnect timer if autoconnect enabled and dtr is high.

        If the cable is unplugged don't auto reconnect.
      ---------------------------------------------------------------------*/
      if( (info->meta_sm.settings.auto_connect_enabled) &&
          (rmnet_meta_smi_verify_dtr_high(info) == TRUE) )
      {
        LOG_MSG_INFO1_0("Starting autoconnect timer in NULL state");
        timer_set( &(info->meta_sm.autoconnect_timer),
                    info->meta_sm.autoconnect_next_backoff_time,
                    0,
                    T_SEC );
        info->meta_sm.autoconnect_next_backoff_time *= 2;
        if(info->meta_sm.autoconnect_next_backoff_time >
           AUTOCONNECT_BACKOFF_TIMER_MAX_VALUE)
        {
          info->meta_sm.autoconnect_next_backoff_time =
            AUTOCONNECT_BACKOFF_TIMER_MAX_VALUE;
        }
      }

      /*---------------------------------------------------------------------------
        Free RNDIS hdr if allocated.
      ---------------------------------------------------------------------------*/
      if (RMNET_DL_DATA_AGG(info) == RMNET_ENABLE_DATA_AGG_RNDIS &&
          info->meta_sm.rndis_eth_hdr != NULL)
      {
        PS_SYSTEM_HEAP_MEM_FREE(info->meta_sm.rndis_eth_hdr);
        info->meta_sm.eth_arped = FALSE;
      }

      else if (RMNET_LINK_PROT(info) == RMNET_ETHERNET_MODE &&
          info->meta_sm.eth_header != NULL)
      {
        PS_SYSTEM_HEAP_MEM_FREE(info->meta_sm.eth_header);
        info->meta_sm.eth_header = NULL;
        info->meta_sm.eth_arped = FALSE;
      }

      info->meta_sm.softap_mode = FALSE;
      PS_LEAVE_CRIT_SECTION(&rmnet_rx_pkt_hdlr_crit_section);
      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_UM_CONFIGURING_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_UM_CONFIGURING_STATE:
    {

      /*---------------------------------------------------------------------
        Bring up the Um interface
      ---------------------------------------------------------------------*/

      /* Check if it is the first tethered call coming up */
      if (info->meta_sm.net_params.call_type == RMNET_CALL_TYPE_LAPTOP) 
      {
        rmnet_phys_xport_inc_teth_call_cnt(dual_ip_info->xport_info.phys_xport);
      }
      /* Open RmNet xport */
      if (!rmnet_logical_xport_open(&dual_ip_info->xport_info))
      {
        rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_UM_IFACE_DOWN_EV );
        return;
      }

      LOG_MSG_INFO1_2("RMNET data format UL [%d] DL [%d]",
                   RMNET_UL_DATA_AGG(info), RMNET_DL_DATA_AGG(info));

      /* Find a free Rmnet Iface for this Rmnet instance*/
      for (i=0; i < RMNET_IFACE_MAX; i++)
      {
        if ( (rmnet_smi_iface_info[i] == NULL) ||
             (rmnet_smi_iface_info[i]->in_use == FALSE) )
        {
          break;
        }
      }

      ASSERT(i < RMNET_IFACE_MAX);

      if (rmnet_smi_iface_info[i] == NULL)
      {
        if (rmnet_meta_smi_iface_init(i) == FALSE)
        {
          LOG_MSG_ERROR_1("Unable to init rmnet iface [%d]", i);
          ASSERT(0);
          break;
        }
        ASSERT(rmnet_smi_iface_info[i] != NULL);
      }

      rmnet_smi_iface_info[i]->in_use = TRUE;

      /* populate the rmnet instance with the iface, llc_inst and rx_sig */
      info->rmnet_iface_ptr = &(rmnet_smi_iface_info[i]->rmnet_iface);
      info->constants.lan_llc_instance = rmnet_smi_iface_info[i]->constants.lan_llc_instance;

      /*---------------------------------------------------------------------
        Set the network params values (to configure UM) in rmnet sm info
        struct before calling bring_up
      ---------------------------------------------------------------------*/
      rmnet_sm_config_net_params(&(info->meta_sm.net_params.cfg_info), info->meta_sm.net_params.call_type);

      /*---------------------------------------------------------------------
       * Resolve default subscription
      ---------------------------------------------------------------------*/
      if (info->meta_sm.net_params.subscription_id == PS_SYS_DEFAULT_SUBS)
      {
        info->meta_sm.net_params.subscription_id = 
          ps_sys_get_default_data_subscription();
      }
      LOG_MSG_INFO2_2("rmnet inst %d subs id %d",
                 rmnet_inst, info->meta_sm.net_params.subscription_id);

      /*---------------------------------------------------------------------
        Initialize RM iface address family.
        Will be reset to UM iface address family when UM is available
      ---------------------------------------------------------------------*/
      rmnet_set_rm_iface_addr_family(info->rmnet_iface_ptr, info->meta_sm.net_params.ip_family_pref);

      /*---------------------------------------------------------------------
        Bring up a data call if we have coverage (IFACE state != DISABLED)
        and if there is no call currently active over Um packet
        (IFACE state == DOWN).

        Note that context lock is required because sockets calls originate
        in another task context (PS).

        So we need to make sure that if we decide to originate,
        IFACE state should be set to something other than DOWN (or DISABLED)
        (to prevent async call) before freeingup the locks. This ensures
        only one type of 1x call originates at a time.
      ---------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Generate a "coming up" indication using internal API since we don't
        support bring_up_cmd handler yet.

        later - remove this, and QMI should call bringup on RmNet iface which
        should establish the laptop rmnet call.  But will have to ensure
        sockets can't bring up the RmNet interface (would need a way to
        distinguish how interfaces are to be bridged - or just have generic
        routing to handle the flow of traffic between ifaces).
      ---------------------------------------------------------------------*/
      event_info.state = info->rmnet_iface_ptr->iface_private.state;
      info->rmnet_iface_ptr->iface_private.state = IFACE_COMING_UP;
      ps_ifacei_invoke_event_cbacks(info->rmnet_iface_ptr,
                                    NULL,
                                    IFACE_COMING_UP_EV,
                                    event_info);

      /* inform QMI WDS of the rmnet_iface_ptr chosen, since it needs to
         listen on the rmnet iface events
         Need to do this after rmnet goes to COMING_UP, because otherwise
         WDS gets a DOWN_EV immediately after registering.
      */

      if (info == dual_ip_info->info_v4)
      {
        wds_rmnet_iface_binding_complete_cb(
                info->rmnet_iface_ptr, RMNET_EP_ID(info), RMNET_MUX_ID(info), 0,
                info->meta_sm.net_params.subscription_id);
      }
      else
      {
        wds_rmnet_iface_binding_complete_cb(
                info->rmnet_iface_ptr, RMNET_EP_ID(info), RMNET_MUX_ID(info), 1,
                info->meta_sm.net_params.subscription_id);
      }

      /*---------------------------------------------------------------------
        Formulate the Rm network interface's 802.3 MAC address

        Vendor ID (most significant 3 bytes) is set to the IEEE assigned
        Vendor ID for the customer.

        TODO - add an NV item for this.

        Device ID (least significant 3 bytes) is set based on the media
        type and underlying QMI link ID.

        byte     description
        ----     -----------
        05 MSB   \
        04       +  Vendor ID = hard coded MSM Net device VID (TBD)
        03       /
        02       media type => see header file rmnet_mac_media_e_type
        01       rmnet dual ip instance
        00       mux id
      ---------------------------------------------------------------------*/

#define QMI_ETH_VENDOR_ID  (0x0250F3) /* To be assigned */

#define QMI_ETH_DEVICE_ID_USB  (0x00)
      rm_hw_addr[0] = (QMI_ETH_VENDOR_ID >> 16) & 0xFF;
      rm_hw_addr[1] = (QMI_ETH_VENDOR_ID >> 8 ) & 0xFF;
      rm_hw_addr[2] = (QMI_ETH_VENDOR_ID      ) & 0xFF;
      rm_hw_addr[3] = QMI_ETH_DEVICE_ID_USB;
      rm_hw_addr[4] = (info->constants.dual_ip_inst) & 0xFF;
      rm_hw_addr[5] = (RMNET_MUX_ID(info))           & 0xFF;

      LOG_MSG_INFO2_2( "Set Rm MAC address to 0x%x:0x%x",
                       QMI_ETH_VENDOR_ID,
                       QMI_ETH_DEVICE_ID_USB << 16 | (info->constants.dual_ip_inst << 8 | RMNET_MUX_ID(info)) );

    #ifdef FEATURE_DATA_RMNET_USB_STANDARD_ECM
      memset(&ioctl_param, 0, sizeof(ioctl_param));

      rmnet_sio_ioctl(info->meta_sm.rmnet_sio_handle,SIO_IOCTL_IS_STANDARD_ECM,&ioctl_param);
      /* GET MAC addr from ECM only if in STD ECM mode */
      if(ioctl_param.is_standard_ecm == TRUE)
      {
        rmnet_sio_ioctl(info->meta_sm.rmnet_sio_handle,SIO_IOCTL_GET_MAC_ADDRESS,&ioctl_param);
          /* add sio ioctl to query MAC address from USB and use it instead of the above
             value */

        memscpy(rm_hw_addr, 
                (LAN_IEEE_MAC_ADDR_LEN > SIO_MAC_ADDR_STR_SIZE ? SIO_MAC_ADDR_STR_SIZE : LAN_IEEE_MAC_ADDR_LEN), 
                ioctl_param.mac_address,
          (LAN_IEEE_MAC_ADDR_LEN > SIO_MAC_ADDR_STR_SIZE ? SIO_MAC_ADDR_STR_SIZE : LAN_IEEE_MAC_ADDR_LEN) );
      }
    #endif

      /*---------------------------------------------------------------------
        Set the Rm network interface's hardware address.
      ---------------------------------------------------------------------*/
      rmnet_meta_smi_set_mac(info->rmnet_iface_ptr,
                              rm_hw_addr,
                              sizeof (rm_hw_addr) );

      /*---------------------------------------------------------------------
        if the valid_fields are 0, implies that NO overrides were set in SNI
        so pass NULL in bring_up() in which case the mode handler will retrieve
        the values from profile.
        Note - Unlike legacy modem calls, For Rmnet call UMTS mode handler will
        calculate the challenge as in sockets case
      ---------------------------------------------------------------------*/
      net_info_cfg_ptr = info->meta_sm.net_params.cfg_info.valid_fields ?
                           &(info->meta_sm.net_params.cfg_info) : NULL;

      LOG_MSG_INFO2_1( "net info cfg ptr 0x%x", net_info_cfg_ptr );


      retval = rmnet_meta_smi_bring_up_um_iface_by_policy( &info->meta_sm.net_params,
                                                           &info->um_iface_ptr,
                                                           rmnet_inst,
                                                           &ps_iface_errno,
                                                           &down_reason ) ;

      // Set RM iface addr family
      if (info->um_iface_ptr != NULL)
      {
        info->um_iface_handle = PS_IFACE_GET_EVENT_HANDLE(info->um_iface_ptr);
        LOG_MSG_INFO2_2("Um iface 0x%x handle 0x%x",
                        info->um_iface_ptr, info->um_iface_handle);
        rmnet_set_rm_iface_addr_family(
                     info->rmnet_iface_ptr,
                     ps_iface_get_addr_family(info->um_iface_ptr));
      }

      // If um iface is already in use by another rmnet instance, reject SNI.
      // Do this check if the um iface is up already or bringup in progress
      if ( (retval == 0 || ps_iface_errno == DS_EWOULDBLOCK) &&
           (info->um_iface_ptr != NULL) )
      {
        if ( rmnet_um_iface_in_use(info, FALSE) )
        {
          LOG_MSG_ERROR_1( "Um iface 0x%x in use by another rmnet instance, tearing down",
                           info->um_iface_ptr );

          // Tear down um iface (since we already called bringup).
          // This will just decrease the ref count and return 0 normally.
          // If the recall returns EWOULDBLOCK, still need to do the same.
          (void) rmnet_tear_down_um_iface( info );

          // Set retval to -1 for later clean up (fall through)
          info->um_iface_ptr = NULL;
          info->um_iface_handle = 0;
          retval = -1;
          ps_iface_errno = DS_ECONNREFUSED;
          down_reason = PS_NET_DOWN_REASON_INTERNAL_CALL_ALREADY_PRESENT;
        }
      }

      // If um iface is already in use by another rmnet instance
      // with arb_close_in_progress set to TRUE, reject SNI.
      // Do this check if the um iface is up already or bringup in progress
      if ( ps_iface_errno == DS_ENETCLOSEINPROGRESS && 
           rmnet_um_iface_in_use(info, TRUE) )
      {
        LOG_MSG_ERROR_1( "Um iface 0x%x in use by another rmnet instance, tearing down",
                         info->um_iface_ptr );

        // Tear down um iface (since we already called bringup).
        // This will just decrease the ref count and return 0 normally.
        // If the recall returns EWOULDBLOCK, still need to do the same.
        (void) rmnet_tear_down_um_iface( info );

        // Set retval to -1 for later clean up (fall through)
        info->um_iface_ptr = NULL;
        info->um_iface_handle = 0;
        retval = -1;
        ps_iface_errno = DS_ECONNREFUSED;
        down_reason = PS_NET_DOWN_REASON_INTERNAL_CALL_ALREADY_PRESENT;
      }

      rex_enter_crit_sect(&lan_llc_inst_to_um_iface_ptr_crit_sec);
      lan_llc_inst_to_um_iface_ptr_lut[info->constants.lan_llc_instance] = info->um_iface_ptr;
      rex_leave_crit_sect(&lan_llc_inst_to_um_iface_ptr_crit_sec);

      if( retval == 0 )
      {
        /* UP event handler will be called when we reg below */
        /*-------------------------------------------------------------------
          If bringup returns 0, interface is UP already.

          Routing rules should really enforce whether we can use this
          interface.  For now, assume that an UP interface is in use,
          unless the ALWAYS_ON attribute is set (i.e. WLAN only) or if it is
          a BCMCS/MCAST iface (which goes UP right after it is known that
          the network supports the technology).
        -------------------------------------------------------------------*/
        if ( (PS_IFACE_GET_IS_ALWAYS_ON(info->um_iface_ptr)) ||
             (PS_IFACE_GET_IS_ACTIVE(info->um_iface_ptr)) ||
             (info->meta_sm.net_params.is_mcast_iface == TRUE) )
        {
          LOG_MSG_INFO2_2("Always on or mcast Iface 0x%x handle 0x%x already up",
                          info->um_iface_ptr, info->um_iface_handle);
          info->meta_sm.restore_um_up = TRUE;
        }
        else
        {
          /*---------------------------------------------------------------
            UM iface is already UP or was lingering.
            Nothing needs to be done here. Register for events.
          ---------------------------------------------------------------*/
          LOG_MSG_INFO2_2("UM iface 0x%x handle 0x%x is already UP",
                          info->um_iface_ptr, info->um_iface_handle);
        }
      }
      else if( ps_iface_errno == DS_EWOULDBLOCK )  // retval == -1
      {
        /*-------------------------------------------------------------------
          Bringup in progress.
        -------------------------------------------------------------------*/
        LOG_MSG_INFO1_2("Iface 0x%x handle 0x%x bringup in progress",
                        info->um_iface_ptr, info->um_iface_handle);
      }
      else if( ps_iface_errno == DS_ENETCLOSEINPROGRESS )
      {
        /*-------------------------------------------------------------------
          Arbitration resulted in Iface teardown in progress. Register new
          set of Iface event handlers for arbitration.
        -------------------------------------------------------------------*/
        LOG_MSG_INFO1_2("Iface 0x%x handle 0x%x arb close in progress",
                        info->um_iface_ptr, info->um_iface_handle);
        info->meta_sm.arb_close_in_progress = TRUE;
      }
      else //ret_val == -1 and errno unhandled
      {
        /*-------------------------------------------------------------------
          Bringup failed.  Cleanup RMNET_META_SM and current pkt data call.
        -------------------------------------------------------------------*/
        LOG_MSG_ERROR_0( "UM bringup failed, posting UM_IFACE_DOWN" );

        memset(&down_event_info, 0, sizeof(down_event_info));
        down_event_info.down_reason = down_reason;
        down_event_info.bearer_tech.rat_value =  DS_SYS_RAT_UNKNOWN;
        ps_iface_down_ind( info->rmnet_iface_ptr,
                           &down_event_info );

        #ifdef FEATURE_DATA_RMNET_USB_STANDARD_ECM
        LOG_MSG_INFO1_0( "Sending Link DOWN notification to ECM driver" );
        rmnet_sio_link_up_notify( info->meta_sm.rmnet_sio_handle,
                                  FALSE ); //link_DOWN
        #endif

        /*-------------------------------------------------------------------
          Iface in incorrect state or bringup failure. Reset SM to NULL.
        -------------------------------------------------------------------*/
        rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_UM_IFACE_DOWN_EV );

        break;
      } /* if retval */


      /*-------------------------------------------------------------------
        Register all the Um interface, flow, and phys link event callbacks
      -------------------------------------------------------------------*/
      if(rmnet_meta_smi_reg_events(info) == FALSE)
      {
        LOG_MSG_ERROR_0( "Failed to register for events" );
        info->meta_sm.arb_close_in_progress = FALSE;
        rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_UM_IFACE_DOWN_EV );
      }

      if(retval == 0)
      {
        /*ps_iface_bring_up returned 0*/

        if( ps_iface_state(info->um_iface_ptr) == IFACE_ROUTEABLE )
        {
          /*UM iface is already brough up and posisbly in use by
          another entity hence it is in routeable state. In this
          situation behaviour should be as if UM iface UP event
          arrived.*/
          LOG_MSG_INFO1_0("UM iface is in ROUTEABLE state");

          /*---------------------------------------------------------------------
            In dynamic binding (e.g LTE), physlink is bound to iface after iface
            is UP. Physlink pointer will be NULL before the binding. For ifaces
            with static binding, physlink ev registration still happens before
            any data transfer begins i.e. before moving to Rm configuring state.
            Flow enabled event will not be missed because the event is sent right
            after event registration.
          ---------------------------------------------------------------------*/
          LOG_MSG_INFO1_1( "um iface ptr name 0x%x",
                           info->um_iface_ptr->name );

          if ( rmnet_meta_smi_reg_physlink_events(info) == TRUE )
          {
            if (RMNET_XPORT_EP_CONFIGURING(info->xport_info))
            {
              rmnet_meta_smi_transition_state (info,
                                             RMNET_META_SM_EP_CONFIGURING_STATE);
            }
            else
            {
              rmnet_meta_smi_transition_state (info,
                                             RMNET_META_SM_RM_CONFIGURING_STATE);
            }
          }
          else
          {
            rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                      RMNET_META_SM_UM_IFACE_DOWN_EV );
          }
        }
      }

      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_WAITING_FOR_UM_ARB_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_WAITING_FOR_UM_ARB_STATE:
    {
        /*---------------------------------------------------------------------
          Enter this state when UM Iface ARB event is recevied
        ---------------------------------------------------------------------*/

        /*--------------------------------------------------------------------
          Deregister all the Um interface, flow, and phys link event callbacks
        ---------------------------------------------------------------------*/
        /* this routine tries to dereg physlink events too, confirm it's ok */
        LOG_MSG_INFO1_0( "In Arb, entered RMNET_META_SM_WAITING_FOR_UM_ARB_STATE" );

        rmnet_meta_smi_dereg_events( info );

        rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);
        retval = rmnet_meta_smi_bring_up_um_iface_by_policy( &info->meta_sm.net_params,
                                                             &info->um_iface_ptr,
                                                             rmnet_inst,
                                                             &ps_iface_errno,
                                                             &down_reason);

        if (info->um_iface_ptr != NULL)
        {
          info->um_iface_handle = PS_IFACE_GET_EVENT_HANDLE(info->um_iface_ptr);
          LOG_MSG_INFO2_2("Um iface 0x%x handle 0x%x",
                          info->um_iface_ptr, info->um_iface_handle);
        }

        if (retval == 0)
        {
          LOG_MSG_INFO1_2( "Arb Iface 0x%x handle 0x%x already up, unexpected, post Iface UP ev",
                           info->um_iface_ptr, info->um_iface_handle );
          info->meta_sm.arb_close_in_progress = FALSE;
          rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                    RMNET_META_SM_UM_IFACE_UP_EV );
        }
        else if( ps_iface_errno == DS_EWOULDBLOCK )
        {
          /*-------------------------------------------------------------------
            Bringup in progress.
          -------------------------------------------------------------------*/
          info->meta_sm.arb_close_in_progress = FALSE;
          LOG_MSG_INFO1_2( "Arb Iface 0x%x handle 0x%x bringup in progress",
                           info->um_iface_ptr, info->um_iface_handle );
        }
        else if( ps_iface_errno == DS_ENETCLOSEINPROGRESS )
        {
          /*-------------------------------------------------------------------
            Close is still in progress.
          -------------------------------------------------------------------*/
          info->meta_sm.arb_close_in_progress = TRUE;
          LOG_MSG_INFO1_2( "Arb Iface 0x%x handle 0x%x close is still in progress",
                           info->um_iface_ptr, info->um_iface_handle );
        }
        else
        {
          LOG_MSG_ERROR_2( "Arb Iface 0x%x, unhandled errno [%d]",
                           info->um_iface_ptr, ps_iface_errno );
          info->meta_sm.arb_close_in_progress = FALSE;
          rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                    RMNET_META_SM_UM_IFACE_DOWN_EV );
          break;
        }

      /*-------------------------------------------------------------------
        Register all the Um interface, flow, and phys link event callbacks
      -------------------------------------------------------------------*/
      if(rmnet_meta_smi_reg_events(info) == FALSE)
      {
        LOG_MSG_ERROR_0( "Failed to register for events" );
        info->meta_sm.arb_close_in_progress = FALSE;
        rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_UM_IFACE_DOWN_EV );
      }

      break; /* RMNET_META_SM_WAITING_FOR_UM_ARB_STATE */
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_RM_CONFIGURING_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_RM_CONFIGURING_STATE:
    {

      
      /*---------------------------------------------------------------------
        Start LAN LLC and initiate the Rm interface specific address
        configuration state machine.
        We need to start DHCP here first because as soon Rm post phys link
        up, runtime_settings is invoked which requires DHCP to be up too.
        Also ps_iface_set_bridge if invoked for RmNet ethernet mode
        would expect lan_llc to be started already.
      ---------------------------------------------------------------------*/
#ifndef TEST_FRAMEWORK
      if(-1 == rmnet_meta_smi_start_addr_cfg_sm(info))
      {
        LOG_MSG_ERROR_0("Rm bringup failed, posting Rm LINK_DOWN" );
        rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_RM_LINK_DOWN_EV );
        break;
      }
#endif /* TEST_FRAMEWORK */

      /*---------------------------------------------------------------------
        Bridge Rm iface.
      ---------------------------------------------------------------------*/
      ps_iface_set_bridge(info->rmnet_iface_ptr, info->um_iface_ptr );

      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
    
      #ifdef FEATURE_DATA_PS_464XLAT
        base_um_iface_ptr = PS_DPM_GET_BASE_IFACE(info->um_iface_ptr);   
      #else
        base_um_iface_ptr = PS_IFACE_GET_BASE_IFACE(info->um_iface_ptr);   
      #endif /* FEATURE_DATA_PS_464XLAT */

      if (PS_IFACE_IS_VALID(base_um_iface_ptr))
      {
         /*---------------------------------------------------------------------
          Set opt rm iface for um iface
        ---------------------------------------------------------------------*/ 
        PS_IFACEI_SET_OPT_RM_IFACE(base_um_iface_ptr, info->rmnet_iface_ptr);
      }
      else
      {
        LOG_MSG_ERROR_1("Invalid um base iface 0x%p", base_um_iface_ptr);
      }      

      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      
      /*---------------------------------------------------------------------
        Register with DPM
      ---------------------------------------------------------------------*/
      reg_info.rm_iface_ptr      = info->rmnet_iface_ptr;
      reg_info.sio_stream_id     = info->xport_info->stream_id;
      reg_info.sio_rx_wm_ptr     = &((info->xport_info)->rx_wmk);
      reg_info.sio_tx_wm_ptr     = &((info->xport_info)->tx_wmk);
      reg_info.link_prot         = RMNET_LINK_PROT(info);
      reg_info.data_format       = (uint8)RMNET_QOS_ENABLED(info);
      reg_info.te_flow_control   = RMNET_TE_FC_ENABLED(info);
      reg_info.data_agg_protocol = RMNET_DL_DATA_AGG(info);      
      reg_info.lan_llc_instance  = info->constants.lan_llc_instance;  
      reg_info.qos_header_format = (uint8)RMNET_QOS_HEADER_TYPE(info);
      result =  ps_dpm_rm_reg_iface(&reg_info, &ps_errno);
      if(DSS_SUCCESS != result)
      {
        if (ps_errno == DS_EINVAL)
        {
          LOG_MSG_ERROR_0("Two rmnet connecting to same pdn.");
          info->net_down_ep_id = 0;  // XXX Get 1st rmnet ep_id/mux_id
          info->net_down_mux_id = 0;
          info->net_down_2_rmnet = TRUE;

          /* Need to stop addr sm otherwise client may get rm configuring ind */
          rmnet_meta_smi_stop_addr_cfg_sm(info);

          rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                    RMNET_META_SM_RM_LINK_DOWN_EV );
          break;
        }
        else
        {
          LOG_MSG_ERROR_1("DPM registration insuccessful %d", result);
          ASSERT(0);         
        }
      }

      info->dpm_rm_registered = TRUE;

      ps_iface_phys_link_up_ind(info->rmnet_iface_ptr );

      info->has_flow = TRUE;


      dual_ip_info->rx_flow_count =
        dual_ip_info->info_v4->has_flow +
        dual_ip_info->info_v6->has_flow;

      /* if this is the first SM to start call then register call backs and enable flow*/
      if( 1 == dual_ip_info->rx_flow_count )
      {
        /*---------------------------------------------------------------------
          Enable the Tx flow.
        ---------------------------------------------------------------------*/
        ps_iface_enable_flow(info->rmnet_iface_ptr, DS_FLOW_RMSM_MASK);
      }
      
      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_NET_IF_UP_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_NET_IF_UP_STATE:
    {
      /*---------------------------------------------------------------------
        This is the common state for data connection up.  TE has configured
        its IP address at this point
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0 ("Host has configured IP address");

      /*---------------------------------------------------------------------
        Bridge Um interface
      ---------------------------------------------------------------------*/
      if (!info->meta_sm.softap_mode)
      {
        ps_iface_set_bridge(info->um_iface_ptr, info->rmnet_iface_ptr);
      }

      /*---------------------------------------------------------------------
        Call came up successfully, reset the back off timer to initial value
      ---------------------------------------------------------------------*/
      info->meta_sm.autoconnect_next_backoff_time = AUTOCONNECT_BACKOFF_TIMER_MIN_VALUE;

      /*---------------------------------------------------------------------
        IP Interface is now available, so put it in routeable state which
        indicates to call is connected.  QMI WDS listens and generates
        appropriate indication messages to the RMNET miniport indicating that
        the call is now connected.

        TODO: Check if phys link should be UP at configuring time. We can
              try moving this to RM_CONFIGURING.
      ---------------------------------------------------------------------*/
      ps_iface_routeable_ind(info->rmnet_iface_ptr);

#ifdef FEATURE_QXDM_WM_TRACING
      /*---------------------------------------------------------------------
        Register with FC for WMk reporting.
      ---------------------------------------------------------------------*/
      if(0 != ps_phys_link_ioctl(PS_IFACE_GET_PHYS_LINK(info->um_iface_ptr),
                                 PS_PHYS_LINK_IOCTL_GET_CALL_ID,
                                 (void*)&call_id,
                                 &ioctl_errno))
      {
        LOG_MSG_ERROR_1("Cannot get the Call ID. Error %d", ioctl_errno);
      }
      else
      {
        info->call_id = call_id;
        fc_post_reg_cmd_wm_trace(UNKNOWN_TECH,
                                 call_id,
                                 RMNET_CLIENT,
                                 rmnet_meta_smi_fc_get_wm_trace_cb,
                                 (uint32)info);
      }
#endif /* FEATURE_QXDM_WM_TRACING */

      if (RMNET_LINK_PROT(info) == RMNET_ETHERNET_MODE)
      {
        if (!rmnet_meta_smi_update_eth_header(info))
        {
          LOG_MSG_ERROR_0("Failed to update Eth header, tearing down");
          rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                    RMNET_META_SM_RM_LINK_DOWN_EV);
        }
      }

      // Update qcrmcall state
      rmnet_meta_smi_update_qcrmcall_state(info);

      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_RM_RECONFIGURING_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_RM_RECONFIGURING_STATE:
    {
      /*---------------------------------------------------------------------
        Unbridge Rm and Um interfaces
      ---------------------------------------------------------------------*/
      ps_iface_set_bridge( info->um_iface_ptr, NULL );

      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE:
    {
      /* If in configuring state, couldnt get any valid IFACE and to avoid
               the race condition, of processing Rm Link Down before IFACE DOWN.
               Ignore, processing of this if the Um Iface Pointer is NULL  There should be an Iface
               down event following this. As Iface Pointer is invalid, nothing can be done in this state. */
      if (!PS_IFACE_IS_VALID(info->um_iface_ptr))
      {
        LOG_MSG_ERROR_1( "Ignoring processing of state [%d] when Iface is invalid",
                         RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE );
        return;
      }
      /*---------------------------------------------------------------------
        If Um was UP before RmNet started using it, restore the original
        state.  If the other user of the interface has already relinquished
        the Um, the teardown below will put Um interface to DOWN state.
      ---------------------------------------------------------------------*/
      if ( TRUE == info->meta_sm.restore_um_up )
      {
        ps_iface_up_ind (info->um_iface_ptr);
      }

      /* Reset DL opt cfg if any */
      if (PS_IFACE_IS_VALID(info->um_iface_ptr))
      {
        PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
        #ifdef FEATURE_DATA_PS_464XLAT
          base_um_iface_ptr = PS_DPM_GET_BASE_IFACE(info->um_iface_ptr);
        #else
          base_um_iface_ptr = PS_IFACE_GET_BASE_IFACE(info->um_iface_ptr);
        #endif /* FEATURE_DATA_PS_464XLAT */
        PS_IFACEI_SET_OPT_RM_IFACE(base_um_iface_ptr, NULL);
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        LOG_MSG_INFO1_2("RmNet waiting for Um Down State: "
                        "Reset DL opt path for iface0x%x:%d",
                        info->um_iface_ptr->name,
                        info->um_iface_ptr->instance);

      }

      /*---------------------------------------------------------------------
        Since TE has terminated the call, we need to cleanup Um before
        completely terminating the call.  Tear it down now.
        If tear down cmd fails, Um is already be down. Passing NULL policy
        here, since we don't cache it.
      ---------------------------------------------------------------------*/
      retval = ps_route_tear_down_iface_by_policy(info->um_iface_ptr,
                                                  NULL,
                                                  &ps_iface_errno,
                                                  NULL);
      LOG_MSG_INFO1_1( "PS route teardown errno [%d]", ps_iface_errno );


      /*---------------------------------------------------------------------
        Check if tear_down_cmd returned success (0) or if no tear_down_hdlr
        registered. In either case, trigger rm bring down
      ---------------------------------------------------------------------*/
      if((retval == 0) || (ps_iface_errno == DS_EOPNOTSUPP))
      {
        /*-------------------------------------------------------------------
          If the Um iface is already down we should cleanup the call
          on Rm since we won't get any other notification from Um
          controller
        -------------------------------------------------------------------*/

        LOG_MSG_INFO1_0( "tear down cmd returns success, posting RMNET_META_SM_UM_IFACE_DOWN_EV" );

        info->meta_sm.call_end_reason = PS_NET_DOWN_REASON_CLIENT_END;

        /* clean up v4/v6 sub state machine */
        rmnet_meta_smi_stop_addr_cfg_sm(info);

        rmnet_meta_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                  RMNET_META_SM_UM_IFACE_DOWN_EV );
      }
      break;
    }

    /*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        RMNET_META_SM_EP_CONFIGURING_STATE
    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
    case RMNET_META_SM_EP_CONFIGURING_STATE:
    {
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Invalid state %d", new_state);
      ASSERT(0);
    }
  } /* switch(new state) */
} /* rmnet_meta_smi_transition_state() */

/*===========================================================================
  FUNCTION RMNET_META_SM_VERIFY_DTR_HIGH_PER_INSTANCE()

  DESCRIPTION
    To check if DTR is high or low based on the instance ID.

  PARAMETERS
    instannce: instance of rmnet_meta_smi

  RETURN VALUE
    boolean : TRUE  = DTR is high
              FALSE = DTR is low

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_verify_dtr_high_per_instance
(
  rmnet_instance_e_type   instance
)
{
  rmnet_smi_info_type   * info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX )
  {
    info = rmnet_smi_info[instance];
  }

  if (info != NULL)
  {
    return info->meta_sm.dtr_is_high;
  }

  return FALSE;

} /* rmnet_meta_sm_verify_dtr_high_per_instance() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_VERIFY_DTR_HIGH()

  DESCRIPTION
    To check if DTR is high or low.

  PARAMETERS
    info: ptr to rmnet_smi_info

  RETURN VALUE
    boolean : TRUE  = DTR is high
              FALSE = DTR is low

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_verify_dtr_high
(
  rmnet_smi_info_type   *info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (info != NULL)
  {
    return info->meta_sm.dtr_is_high;
  }

  return FALSE;

} /* rmnet_meta_smi_verify_dtr_high() */


/*===========================================================================
  FUNCTION RMNET_META_SM_PROCESS_DTR_CHANGED_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM DTR event cmd.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the RmNet sm
              event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_process_dtr_changed_cmd
(
  rmnet_smi_info_type  * info,
  boolean                dtr_asserted
)
{
  rmnet_instance_e_type  rmnet_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (info);
  rmnet_instance = RMNET_META_SM_INFO_TO_INSTANCE(info);
  ASSERT(rmnet_instance < RMNET_INSTANCE_MAX);

  LOG_MSG_INFO1_2 ("DTR or binding change for RMNET inst [%d] DTR [%d]",
                   rmnet_instance, dtr_asserted);

  /*-------------------------------------------------------------------------
    Handle DTR state change.

    If DTR drops, end the call by posting RMNET_META_SM_LINK_DOWN_EV, and tell
    the QMI module that the link is down.

    If DTR goes high, check if autoconnect is enabled.  If so, post
    RM_DEV_START_PKT_EV to start a new call.
  -------------------------------------------------------------------------*/
  if ( FALSE == dtr_asserted )
  {
    if(info->meta_sm.dtr_is_high == FALSE)
    {
      LOG_MSG_INFO1_1("Ignoring DTR low, RMNET inst %d", rmnet_instance);
      return;
    }

    info->meta_sm.dtr_is_high = FALSE;
    if (info->meta_sm.state != RMNET_META_SM_NULL_STATE)
    {
      LOG_MSG_INFO1_1( "DTR dropped - end packet call, RMNET inst %d",
                       rmnet_instance );
      rmnet_meta_sm_post_event(rmnet_instance, RMNET_META_SM_RM_LINK_DOWN_EV);
    }

    /*-----------------------------------------------------------------------
      Rearm autoconnect when cable is disconnected
      TODO - merge Keyur's fix for disabling device
    -----------------------------------------------------------------------*/
    if (info->meta_sm.settings.auto_connect_enabled)
    {
      LOG_MSG_INFO1_1( "rearm autoconnect trigger, RMNET inst %d",
                       rmnet_instance );
      info->meta_sm.auto_connect_trigger = TRUE;
      /*---------------------------------------------------------------------
        Resetting the backoff time to MIN as dtr dropped
      ---------------------------------------------------------------------*/
      info->meta_sm.autoconnect_next_backoff_time =
        AUTOCONNECT_BACKOFF_TIMER_MIN_VALUE;
    }
  }
  else
  {
    if(info->meta_sm.dtr_is_high == TRUE)
    {
      LOG_MSG_INFO1_0("Ignoring DTR High");
      return;
    }

    info->meta_sm.dtr_is_high = TRUE;
    if (info->meta_sm.auto_connect_trigger)
    {
      LOG_MSG_INFO1_1 ("DTR went high - start packet call, RMNET inst %d",
                       rmnet_instance );
      rmnet_meta_sm_post_event(rmnet_instance,
                               RMNET_META_SM_RM_DEV_START_PKT_EV);
      info->meta_sm.auto_connect_trigger = FALSE;
    }
  }
} /* rmnet_meta_smi_process_dtr_changed_cmd() */

/*===========================================================================
FUNCTION RMNET_META_SMI_INIT_RNDIS_ETH_HDR

DESCRIPTION
  Inits RNDIS ETH HDR that needs to be added for DL packets.

PARAMETERS
  item_head_ptr -  message (in dsm chain) to be transmitted
  user_data     -  user data passed to lan_llc_start(),
                   contains rmnet_meta_sm info ptr

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void rmnet_meta_smi_init_rndis_eth_hdr
(
  rmnet_smi_info_type * info
)
{
  rmnet_rndis_eth_hdr_type * rndis_eth_hdr = NULL;

  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(rndis_eth_hdr, 
                                  sizeof(rmnet_rndis_eth_hdr_type),
                                  rmnet_rndis_eth_hdr_type*);

  if (rndis_eth_hdr == NULL)
  {
    return;
  }
  memset(rndis_eth_hdr, 0, sizeof(rmnet_rndis_eth_hdr_type));

  rndis_eth_hdr->rndis_hdr.msg_type = RMNET_RNDIS_MSG_TYPE;
  rndis_eth_hdr->rndis_hdr.data_offset = RMNET_RNDIS_PAYLOAD_OFFSET;

  /*Update back in info*/
  info->meta_sm.rndis_eth_hdr = rndis_eth_hdr;

}/*rmnet_meta_smi_init_rndis_eth_hdr()*/

/*===========================================================================
FUNCTION RMNET_META_SMI_RX_READY

DESCRIPTION


PARAMETERS


RETURN VALUE


DEPENDENCIES


SIDE EFFECTS
  None.

===========================================================================*/
boolean rmnet_meta_smi_rx_ready
(
  rmnet_smi_info_type    * info
)
{

  if(!rmnet_meta_smi_qos_or_te_fc_enabled(info)) //legacy i.e. non-qos
  {
    if((PS_IFACE_COMMON_DEFAULT_IS_FLOW_ENABLED(info->um_iface_ptr)))
      return TRUE;
    else
      return FALSE;
  }

  /*-------------------------------------------------------------------------
    QoS enabled
  -------------------------------------------------------------------------*/
  return PS_IFACE_FLOW_ENABLED(info->um_iface_ptr);
}

/*===========================================================================
FUNCTION RMNET_META_SMI_PROCESS_QOS_HDR

DESCRIPTION
  Processes QOS header

PARAMETERS
  user_data     -  user data passed to lan_llc_start(),
                   contains rmnet_meta_sm info ptr

RETURN VALUE


DEPENDENCIES


SIDE EFFECTS
  None.

===========================================================================*/
void rmnet_meta_smi_process_qos_hdr
(
  void *                user_data,
  dsm_item_type *       pkt_ptr,
  ps_meta_info_type_ex  ** meta_info_ex_ptr_ptr,
  rmnet_meta_smi_qos_hdr_type * qos_hdr
)
{
  rmnet_smi_info_type    * info = NULL;
  ps_tx_meta_info_type * tx_meta_info_ptr;
  ps_meta_info_type_ex * meta_info_ex_ptr;
  uint8                  octet_val = 0;
  struct ip              pkt_ip4_hdr = {0,};
  rmnet_instance_e_type  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == user_data)
  {
    LOG_MSG_ERROR_0("rmnet_meta_smi_process_qos_hdr(): user_data ptr is NULL");
    return ;
  }

  info = (rmnet_smi_info_type *) user_data;
  rmnet_inst = RMNET_META_SM_INFO_TO_INSTANCE(info);

  /*-------------------------------------------------------------------------
    QoS enabled
  -------------------------------------------------------------------------*/

  if(qos_hdr->flow_ptr != NULL)
  {
    /* don't assert because of race condition */
    //ASSERT(PS_FLOW_IS_VALID(qos_hdr.flow_ptr));

    LOG_MSG_INFO3_2("got pkt on qos flow 0x%x, RMNET inst %d",
                    qos_hdr->flow_ptr, rmnet_inst);

    /* always fwd on default if flow is !activated */
    if(PS_FLOW_GET_STATE(qos_hdr->flow_ptr) != FLOW_ACTIVATED)
    {
      LOG_MSG_INFO3_0("fwding pkt on default flow");
      qos_hdr->flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(info->um_iface_ptr);
    }
  }
  else
  {
    LOG_MSG_INFO3_1("got pkt on default flow, RMNET inst %d", rmnet_inst);
    /* if 0 - then get default flow */
    qos_hdr->flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(info->um_iface_ptr);
  }

  /* NOT checking whether TX flow is ENABLED for sec/pri..
      assumption: if any flow is TX_DISABLED, driver gets the ind and stops
      sending on that flow. Any packets driver had already sent before
      processing the ind can be queued on flow wmk
      verify HW and DNE values ok for the above */

  /* generate meta_info - TO DO caching */

  /* Since packet needs to be routed to Um Iface, its better we create TX meta info
     directly here */
  PS_TX_META_INFO_GET_ALL(tx_meta_info_ptr);

  if ((tx_meta_info_ptr == NULL ) ||
      (PS_TX_META_GET_RT_META_INFO_PTR(tx_meta_info_ptr) == NULL))
  {
    DATA_ERR_FATAL("Cannot allocate meta_info");
    return ;
  }

  PS_TX_META_SET_ROUTING_CACHE(tx_meta_info_ptr, info->um_iface_ptr);

  if ( FALSE != RMNET_QOS_ENABLED(info) )
  {
    PS_TX_META_SET_FILTER_RESULT(tx_meta_info_ptr,
                               IP_FLTR_CLIENT_QOS_OUTPUT,
                               qos_hdr->flow_ptr);
  }

  /* If the DOS flag is set, propagate the information to the PS layer
   * by setting the appropriate flag in meta info. Use the IP_ID from
   * the packet as the DOS ack handle. */
  if (qos_hdr->flags & ((uint8)RMNET_QOS_FLAG_TYPE_DOS))
  {
    /* Extract the IP version. DOS is supported only for IPv4 packets. */
    if ( (sizeof(uint8) == dsm_extract( pkt_ptr,
                                        0,
                                        &octet_val,
                                        sizeof(uint8))) &&
         (IP_V4 == (ip_version_enum_type)(octet_val >> 4)) )
    {
      /* Peek and extract the IP_ID from the IPv4 header. */
      if (IPLEN == dsm_extract( pkt_ptr,
                                0,
                                (void*)&pkt_ip4_hdr,
                                IPLEN ))
      {
        ASSERT(tx_meta_info_ptr->pkt_meta_info_ptr != NULL);
        tx_meta_info_ptr->pkt_meta_info_ptr->tx_flags |= MSG_FAST_EXPEDITE;
        tx_meta_info_ptr->pkt_meta_info_ptr->dos_ack_handle = pkt_ip4_hdr.id;
        LOG_MSG_INFO2_1( "DOS enabled on pkt, dos_ack_handle:0x%x",
                         tx_meta_info_ptr->pkt_meta_info_ptr->dos_ack_handle);
      }
      else
      {
        LOG_MSG_INFO1_0("Invalid IP header, DOS not enabled");
      }
    }
    else
    {
      LOG_MSG_INFO1_0("Invalid IP version, DOS not enabled");
    }
  }

  /* Update meta_info_ex_ptr : create meta_info_ex_ptr and embed
  tx_meta_info_ptr inside */
  PS_META_INFO_GET_EX(meta_info_ex_ptr);
  if (meta_info_ex_ptr == NULL )
  {
    DATA_ERR_FATAL("Cannot allocate ps_meta_info_type_ex");
    return ;
  }

  meta_info_ex_ptr->tx_meta_info_ptr = tx_meta_info_ptr;
  (*meta_info_ex_ptr_ptr) = meta_info_ex_ptr;
} /* rmnet_meta_smi_process_qos_hdr() */


/*===========================================================================
  FUNCTION RMNET_META_SM_INIT_DUAL_IP_INSTANCE

  DESCRIPTION
    Open the specified RmNet instance. This function runs in DCC task and posts
    the result to QMUX that runs in qmi_modem task.

  PARAMETERS
    instance : instance of RmNET to open
    port_id :  SIO port identifier

  RETURN VALUE
    void

  SIDE EFFECTS
    none
===========================================================================*/
static boolean rmnet_meta_sm_init_dual_ip_instance
(
  uint8                  dual_ip_inst
)
{
  rmnet_smi_dual_ip_info_type * dual_ip_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( dual_ip_inst >= RMNET_DUAL_IP_MAX ||
          rmnet_smi_dual_ip_info[dual_ip_inst] != NULL )
  {
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Allocate the dual IP info structure
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dual_ip_info, sizeof(rmnet_smi_dual_ip_info_type),
                            rmnet_smi_dual_ip_info_type*);

  if (dual_ip_info == NULL)
  {
    return FALSE;
  }

  memset(dual_ip_info, 0, sizeof(rmnet_smi_dual_ip_info_type));

  /*-------------------------------------------------------------------------
    Init rmnet xport
  -------------------------------------------------------------------------*/
  if (!rmnet_logical_xport_init(&dual_ip_info->xport_info,
                    rmnet_meta_smi_xport_init, dual_ip_info))
  {
    LOG_MSG_ERROR_0("Failed to init rmnet xport");
    PS_SYSTEM_HEAP_MEM_FREE(dual_ip_info);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Init rmnet instances
  -------------------------------------------------------------------------*/
  if ( ! rmnet_meta_smi_init(dual_ip_inst * 2) ||
       ! rmnet_meta_smi_init(dual_ip_inst * 2 + 1) )
  {
    LOG_MSG_ERROR_1("Failed to open RMNET dual insts %d", dual_ip_inst);
    PS_SYSTEM_HEAP_MEM_FREE(dual_ip_info);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Initialize other structures
  -------------------------------------------------------------------------*/
  dual_ip_info->info_v4 = rmnet_smi_info[dual_ip_inst * 2];
  dual_ip_info->info_v6 = rmnet_smi_info[dual_ip_inst * 2 + 1];

  if (dual_ip_info->info_v4 == NULL || dual_ip_info->info_v6 == NULL)
  {
    ASSERT(0);
    PS_SYSTEM_HEAP_MEM_FREE(dual_ip_info);
    return FALSE;
  }

  dual_ip_info->info_v4->xport_info = &dual_ip_info->xport_info;
  dual_ip_info->info_v6->xport_info = &dual_ip_info->xport_info;

  dual_ip_info->info_v4->has_flow = FALSE;
  dual_ip_info->info_v6->has_flow = FALSE;
  dual_ip_info->rx_flow_count = 0;

  /*-------------------------------------------------------------------------
    Setup Legacy Watermarks, Init Signal and enable flow for legacy packets
  -------------------------------------------------------------------------*/
  dual_ip_info->legacy_flow_enabled = TRUE;

  rmnet_smi_dual_ip_info[dual_ip_inst] = dual_ip_info;

  return TRUE;

} /* rmnet_meta_sm_init_dual_ip_instance() */


/*===========================================================================
FUNCTION RMNET_IFACE_IOCTL_HDLR

DESCRIPTION
  iface ioctl Callback

PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on
  ioctl_name        - Type of request
  argval_ptr        - QoS specified by the application
  *ps_errno         - Ptr to error code (if any)

DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure

SIDE EFFECTS

===========================================================================*/
LOCAL int rmnet_iface_ioctl_hdlr
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  int ret_val;
  rmnet_instance_e_type  i;
  sio_port_id_type * output;
  sio_stream_id_type * stream_id;
  rmnet_sm_link_prot_e_type* link_prot_output;
  ps_iface_ioctl_rmnet_watermarks_type * wmk_output;

  lan_lle_enum_type         lan_llc_instance = LAN_LLE_MIN;
  ps_ip_addr_type           ip_addr;
  ps_iface_type            *bridge_iface_ptr = NULL;
  //lan_llc_start_info_type  *ptr_start_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ret_val = -1;

  if(argval_ptr == NULL || iface_ptr == NULL || ps_errno == NULL)
  {
    LOG_MSG_ERROR_0("IOCTL arguments invalid. Return -1");
    return ret_val;
  }
  memset(&ip_addr, 0, sizeof(ps_ip_addr_type));
  memset(&lan_llc_instance, 0, sizeof(lan_lle_enum_type));

  LOG_MSG_INFO3_1("rmnet_iface_ioctl_hdlr:processing IOCTL (%d)",ioctl_name);
  switch ( ioctl_name )
  {
    case PS_IFACE_IOCTL_RM_GET_SIO_PORT:
      output = (sio_port_id_type *) argval_ptr;
      for (i = RMNET_INSTANCE_MIN; i < RMNET_INSTANCE_MAX; i++) {
        if (rmnet_smi_info[i] != NULL &&
            rmnet_smi_info[i]->rmnet_iface_ptr == iface_ptr)
        {
          *output =
            rmnet_logical_xport_get_port(rmnet_smi_info[i]->xport_info);
          ret_val = 0;
          break;//exit the loop
        }
      }
      break;

    case PS_IFACE_IOCTL_RM_GET_STREAM_ID:
      stream_id = (sio_stream_id_type *) argval_ptr;
      for (i = RMNET_INSTANCE_MIN; i < RMNET_INSTANCE_MAX; i++) {
        if (rmnet_smi_info[i] != NULL &&
            rmnet_smi_info[i]->rmnet_iface_ptr == iface_ptr)
        {
          *stream_id = rmnet_smi_info[i]->xport_info->stream_id;
          ret_val = 0;
          break;//exit the loop
        }
      }
      break;

    case PS_IFACE_IOCTL_RMNET_GET_LINK_PROTO_TYPE:
      link_prot_output = (rmnet_sm_link_prot_e_type*) argval_ptr;
      for (i = RMNET_INSTANCE_MIN; i < RMNET_INSTANCE_MAX; i++) {
        if (rmnet_smi_info[i] != NULL &&
            rmnet_smi_info[i]->rmnet_iface_ptr == iface_ptr)
        {
          *link_prot_output = RMNET_LINK_PROT(rmnet_smi_info[i]);
          ret_val = 0;
          break;//exit the loop
        }
      }
      break;

    case PS_IFACE_IOCTL_RMNET_GET_IPV4_DEST_MAC_ADDR:
      for (i = RMNET_INSTANCE_MIN; i < RMNET_INSTANCE_MAX; i++) {
        if (rmnet_smi_info[i] != NULL &&
            rmnet_smi_info[i]->rmnet_iface_ptr == iface_ptr)
        {
          lan_llc_instance = rmnet_smi_info[i]->constants.lan_llc_instance;
          break;//exit the loop
        }
      }
      if(lan_llc_instance == LAN_LLE_MIN)
      {
        LOG_MSG_INFO1_1("Could not get LAN LLC instance from RmNet Iface. IOCTL: %d",
                        ioctl_name);
        ret_val = -1;
      }
      else
      {
        ip_addr.type = IPV4_ADDR;
        ps_iface_get_cached_addr( ps_iface_bridge_iface(iface_ptr), &ip_addr );

        ret_val = arp_get_dest_mac_addr(lan_llc_instance,
                                        &ip_addr,
                                        (ps_iface_ioctl_mac_addr_type)
                                        argval_ptr,
                                        LAN_IEEE_MAC_ADDR_LEN);
      }
      break;

    case PS_IFACE_IOCTL_RMNET_GET_IPV6_DEST_MAC_ADDR:
      ip_addr.type = IPV6_ADDR;
      bridge_iface_ptr = ps_iface_bridge_iface(iface_ptr);
      if(bridge_iface_ptr == NULL)
      {
        LOG_MSG_ERROR_0("bridge_iface_ptr returned as NULL");
        ret_val = -1;
        break;
      }

      if (ps_iface_get_ipv6_ext_addr(bridge_iface_ptr, &ip_addr) == FALSE)
      {
         LOG_MSG_ERROR_1("Could not get the ipv6 address. Fail IOCTL: %d",
                         ioctl_name);
         ret_val = -1;
         break;
      }
      else
      {
        ret_val = ps_icmp6_get_dest_mac_addr (
                                iface_ptr,
                                &(ip_addr.addr.v6),
                                (ps_iface_ioctl_mac_addr_type)argval_ptr,
                                PS_IFACE_MAX_HW_ADDR_LEN
                               );
      }
      break;

    case PS_IFACE_IOCTL_RMNET_IS_TETHERED_CALL:
      {
        uint32 tmp_var;
        uint8  instance_idx = 0;

        ps_iface_ioctl_rmnet_is_tethered_call_type *result =
          (ps_iface_ioctl_rmnet_is_tethered_call_type *)argval_ptr;
        rmnet_smi_info_type *info =
         (rmnet_smi_info_type*)iface_ptr->iface_private.tx_cmd_info;

        if ((uint32)info < LAN_LLE_MAX)
        {
          for (instance_idx = 0; instance_idx < RMNET_INSTANCE_MAX; instance_idx++)
          {
            if (rmnet_smi_info[instance_idx] != NULL &&
                rmnet_smi_info[instance_idx]->rmnet_iface_ptr == iface_ptr)
            {
              /* RmNet in eth mode, info wont work */
              lan_llc_instance = (lan_lle_enum_type)(uint32)rmnet_smi_info[instance_idx]->constants.lan_llc_instance;
              if((lan_llc_instance <= LAN_LLE_MIN) || (lan_llc_instance >= LAN_LLE_MAX))
              {
                LOG_MSG_INFO1_1("Could not get LAN LLC instance from RmNet Iface. IOCTL: %d",
                                ioctl_name);
                ret_val = -1;
              }
              else
              {
                info = rmnet_smi_info[instance_idx];
              }
              break;
            }
          }
        }

        if (NULL == info || instance_idx == RMNET_INSTANCE_MAX)
        {
          LOG_MSG_ERROR_2("Couldnt find matching rmnet instance %d or info NULL 0x%x",
                          instance_idx, info);
          ret_val = -1;
          break;
        }

        *result = rmnet_meta_smi_find_proc_id(&tmp_var,
                                   RMNET_META_SM_INFO_TO_INSTANCE(info));
        
        /* if call is embedded then its not tethered, so we should return false */
        if(RMNET_PROC_ID_APPS1 == tmp_var)
        {
          *result = FALSE;
        }

        LOG_MSG_INFO1_1("Returning is_tethered IOCTL result as %d", *result);
        ret_val = 0;
      }
      break;

    case PS_IFACE_IOCTL_RMNET_SET_SOFTAP_MODE:
      {
        uint8  instance_idx = 0;
        ps_iface_ioctl_rmnet_softap_mode_type *softap_mode =
         (ps_iface_ioctl_rmnet_softap_mode_type *)argval_ptr;
        rmnet_smi_info_type *info =
         (rmnet_smi_info_type*)iface_ptr->iface_private.tx_cmd_info;

        if ((uint32)info < LAN_LLE_MAX)
        {
          for (instance_idx = 0; instance_idx < RMNET_INSTANCE_MAX; instance_idx++)
          {
            if (rmnet_smi_info[instance_idx] != NULL &&
                rmnet_smi_info[instance_idx]->rmnet_iface_ptr == iface_ptr)
            {
              /* RmNet in eth mode, info wont work */
              lan_llc_instance = (lan_lle_enum_type)(uint32)rmnet_smi_info[instance_idx]->constants.lan_llc_instance;
              if((lan_llc_instance <= LAN_LLE_MIN) || (lan_llc_instance >= LAN_LLE_MAX))
              {
                LOG_MSG_INFO1_1("Could not get LAN LLC instance from RmNet Iface. IOCTL: %d",
                                ioctl_name);
                ret_val = -1;
              }
              else
              {
                info = rmnet_smi_info[instance_idx];
              }
              break;
            }
          }
        }

        if (instance_idx == RMNET_INSTANCE_MAX || NULL == info)
        {
          LOG_MSG_ERROR_2("Couldnt find matching rmnet instance %d or info NULL 0x%x",
                          instance_idx, info);
          break;
        }

        LOG_MSG_INFO1_2("Setting softap mode for RmNet instance %d as %d",
                        RMNET_META_SM_INFO_TO_INSTANCE(info), *softap_mode);
        info->meta_sm.softap_mode = *softap_mode;

        /*-------------------------------------------------------------------
           For SoftAP IPv6 LAN setup happens without WWAN.
           Move the RMNET V6 state machine which is waiting for RA event.
         ------------------------------------------------------------------*/
        if(ps_iface_addr_family_is_v6(info->um_iface_ptr))
        {
          LOG_MSG_INFO1_1("SoftAP IPv6 on RmNet instance %d",
                          RMNET_META_SM_INFO_TO_INSTANCE(info));
          rmnet_v6_sm_post_event(RMNET_META_SM_INFO_TO_INSTANCE(info),
                                 RMNET_V6_SM_RA_SENT_EV);

          /*-------------------------------------------------------------------
            For SoftAP IPV6 LAN to be completely up in ethernet mode we need
            Link Layer address also to be resolved for the RMNET V6 state
            machine to be routeable.
          -------------------------------------------------------------------*/
          if (RMNET_LINK_PROT(info) == RMNET_ETHERNET_MODE)
          {
            LOG_MSG_INFO1_0( "Softap ethernet mode:posting RMNET_V6_SM_LL_ADDR_RESOL_EV" );
            rmnet_v6_sm_post_event( RMNET_META_SM_INFO_TO_INSTANCE(info),
                                    RMNET_V6_SM_LL_ADDR_RESOL_EV );
          }
        }

        ret_val = 0;
      }
      break;

    case PS_IFACE_IOCTL_RMNET_GET_WATERMARKS:
      wmk_output = (ps_iface_ioctl_rmnet_watermarks_type *) argval_ptr;
      for (i = RMNET_INSTANCE_MIN; i < RMNET_INSTANCE_MAX; i++)
      {
        if (rmnet_smi_info[i] != NULL &&
            rmnet_smi_info[i]->rmnet_iface_ptr == iface_ptr)
        {
          ret_val = rmnet_logical_xport_get_wmk_queues(
                        rmnet_smi_info[i]->xport_info,
                        (dsm_watermark_type **)&wmk_output->rx_wmk,
                        (dsm_watermark_type **)&wmk_output->tx_wmk);
          break; //exit the loop
        }
      }
      *ps_errno = (ret_val == 0) ? 0 : DS_EINVAL;
      break;

    default:
      LOG_MSG_ERROR_1("Invalid request from IOCTL: %d", ioctl_name);

      *ps_errno =  DS_EINVAL;
      ret_val = -1;
      break;
  }

  return ret_val;

}

/*===========================================================================
  FUNCTION RMNET_META_SMI_LOG_WMK_LEVELS

  DESCRIPTION
    Called when PHYSLINK UP/DOWN event occurs. This function logs Watermark
    counts as DIAG events.

  PARAMETERS
    rmnet_instance   : Rmnet instance.
    um_phys_link_ptr : Pointer to the physlink
    dormancy_status  : Values: 1 - Dormant 2- Active

  RETURN VALUE
    none

  SIDE EFFECTS
    none
===========================================================================*/
static void rmnet_meta_smi_log_wmk_levels
(
   rmnet_instance_e_type           rmnet_instance,
   ps_phys_link_type              *um_phys_link_ptr,
   ps_wmk_logging_dorm_state_type dormancy_status
)
{
  rmnet_smi_info_type   *info;
  int16                  retval;
  dsm_watermark_type    *rx_wmk = NULL;
  dsm_watermark_type    *tx_wmk = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(rmnet_instance < RMNET_INSTANCE_MAX);

  if(um_phys_link_ptr == NULL)
  {
     LOG_MSG_ERROR_0("rmnet_meta_smi_log_wmk_levels: Null physlink pointer" );
     return ;
  }

  info = rmnet_smi_info[rmnet_instance];
  ASSERT(info);
  retval = rmnet_logical_xport_get_wmk_queues(
                     info->xport_info, &rx_wmk, &tx_wmk);
  if(retval != 0)
  {
    LOG_MSG_ERROR_1("Cannot get wmk queues from RMNET SIO instance %d ",
                    rmnet_instance);
    return;
  }

  /*---------------------------------------------------------------------
    Generate DIAG events with watermark information
  ---------------------------------------------------------------------*/
  if(-1 == ps_wmk_logging_event_report(um_phys_link_ptr,
                                 EVENT_DS_DORMANCY_STATUS_RM_QUEUE_STATS,
                                 dormancy_status,
                                 rx_wmk->current_cnt,
                                 tx_wmk->current_cnt))
  {
    LOG_MSG_ERROR_0("Cannot log Rm watermark counts");
  }

  return;

} /* rmnet_meta_smi_log_wmk_levels() */

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
  FUNCTION RMNET_META_SMI_FC_GET_WM_TRACE_CB

  DESCRIPTION
    Callback called by FC module to query for the RMNET SIO wmk statistics

  PARAMETERS
    user_data   : uint32 handle containing the RMNET SM info pointer.
    wm_stat_ptr : pointer to the Watermark statistics.

  RETURN VALUE
    none

  SIDE EFFECTS
    none
===========================================================================*/
static void rmnet_meta_smi_fc_get_wm_trace_cb
(
  uint32            user_data,
  fc_wm_stat_type  *wm_stat_ptr
)
{
  rmnet_smi_info_type   *info;
  dsm_watermark_type    *rx_wmk = NULL;
  dsm_watermark_type    *tx_wmk = NULL;
  int16                  retval;
  uint8                  wm_index;

  /*-------------------------------------------------------------------------
    There should be space for wmk stats.
  -------------------------------------------------------------------------*/
  wm_index = wm_stat_ptr->num_wms;
  if(wm_index >= MAX_WM_CNT)
  {
    LOG_MSG_ERROR_0("No space in the wmk stats buffer");
    return;
  }

  info = (rmnet_smi_info_type*)user_data;

  retval = rmnet_logical_xport_get_wmk_queues(
                   info->xport_info, &rx_wmk, &tx_wmk);

  if(retval != 0)
  {
    LOG_MSG_ERROR_0("Cannot get the wmk queues from RMNET SIO");
    return;
  }

  /*-------------------------------------------------------------------------
    Get the Rx Wmk stats.
  -------------------------------------------------------------------------*/
  dsm_get_wm_stats(rx_wmk,
                   &(wm_stat_ptr->wm_info[wm_index].wm_counts));
  wm_stat_ptr->wm_info[wm_index].tech_id  = UNKNOWN_TECH;
  wm_stat_ptr->wm_info[wm_index].call_id  = info->call_id;
  wm_stat_ptr->wm_info[wm_index].wm_id    = WM_DIR_RMNET_SIO_UL;
  wm_index++;

  if(wm_index >= MAX_WM_CNT)
  {
    wm_stat_ptr->num_wms = wm_index;
    LOG_MSG_ERROR_0("No space in the wmk stats buffer");
    return;
  }
  /*-------------------------------------------------------------------------
    Get the Tx Wmk stats.
  -------------------------------------------------------------------------*/
  dsm_get_wm_stats(tx_wmk,
                   &(wm_stat_ptr->wm_info[wm_index].wm_counts));
  wm_stat_ptr->wm_info[wm_index].tech_id  = UNKNOWN_TECH;
  wm_stat_ptr->wm_info[wm_index].call_id  = info->call_id;
  wm_stat_ptr->wm_info[wm_index].wm_id    = WM_DIR_RMNET_SIO_DL;
  wm_index++;

  wm_stat_ptr->num_wms = wm_index;

  return;
} /* rmnet_meta_smi_fc_get_wm_trace_cb() */
#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================
  FUNCTION RMNET_META_SMI_UPDATE_PHYSLINK()

  DESCRIPTION
    Updates cached physlink information.

  PARAMETERS
    instance - the RmSM instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_smi_update_physlink
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type *  info;
  ps_phys_link_type *new_phys_link_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(instance < RMNET_INSTANCE_MAX);

  info = rmnet_smi_info[instance];
  ASSERT(info);

  new_phys_link_ptr = PS_IFACE_GET_PHYS_LINK(info->um_iface_ptr);

  if( NULL != info->meta_sm.um_iface_phys_link_ptr &&
      NULL != new_phys_link_ptr &&
      new_phys_link_ptr != info->meta_sm.um_iface_phys_link_ptr)
  {
    LOG_MSG_INFO2_2 ("Changing physlink! old physlink (0x%x), new physlink (0x%x)",
                     info->meta_sm.um_iface_phys_link_ptr, new_phys_link_ptr);

    rmnet_meta_smi_dereg_physlink_events(info);
    (void)rmnet_meta_smi_reg_physlink_events(info);
  }

  return;
} /* rmnet_meta_smi_update_physlink */


/*===========================================================================
  FUNCTION RMNET_META_SMI_UPDATE_AUTOCONNECT_STATE()

  DESCRIPTION
  Determine autoconnect state(Enable or disable) based on new setting

  Auto connect state can be determined using this formula.

  auto_connect_state = (autconnect_setting &&
                        roam_setting((home && SS roaming status) || roaming always))

  By default:
    autconnect_setting = 0 (Disabled).
    roam_setting = 0(roaming alwasys)
    SS roaming status = 0(roaming)
    auto_connect_state = FALSE

  For backward compatibility(prior ICD version 3.1):
  The default value would be consider for roam_setting, SS roaming status.
  So auto_connect_state = autconnect_setting;

  PARAMETERS
    instance - the RmNet instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_update_autoconnect_state
(
  rmnet_smi_info_type  * info
)
{
  boolean                auto_connect_state = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (info == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Finding the auto connect current state.
  -------------------------------------------------------------------------*/
  if( RMNET_AUTOCONNECT_ENABLED == info->meta_sm.settings.autoconnect_setting )
  {
    /* If roam setting is home only and ss roaming status is home
       then set auto_connect_state to true*/
    if ( RMNET_AUTOCONNECT_ROAM_SETTING_HOME_ONLY ==
                  info->meta_sm.settings.autoconnect_roam_setting )
    {
      if(rmnet_smi_cm_info.ss_roaming_status == SYS_ROAM_STATUS_OFF )
      {
        auto_connect_state = TRUE;
      }
      else if((rmnet_smi_eri_roam_ind_info.len  > 0) &&
              (rmnet_smi_eri_roam_ind_info.len  <= RMNET_ROAM_IND_FILE_MAX_LEN))
      {
        uint8 i;
        for(i = 0; i < rmnet_smi_eri_roam_ind_info.len; i++)
        {
          if(rmnet_smi_cm_info.ss_roaming_status ==
                                           rmnet_smi_eri_roam_ind_info.data[i] )
          {
            auto_connect_state = TRUE;
            break;
          }
        }
      }
    }
    /* If roam setting is roaming or known set auto_connect_state is true*/
    else
    {
      auto_connect_state = TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    If autoconnect state has changed then update the cache and set trigger.
  -------------------------------------------------------------------------*/
  if(info->meta_sm.settings.auto_connect_enabled != auto_connect_state)
  {
    LOG_MSG_INFO1_3("Auto_connect_state:%d has changed, Auto_connect_setting:%d, roam_setting:%d",
                    info->meta_sm.settings.auto_connect_enabled,
                    info->meta_sm.settings.autoconnect_setting,
                    info->meta_sm.settings.autoconnect_roam_setting);

    /* update the autoconnect state in cache.*/
    info->meta_sm.settings.auto_connect_enabled = auto_connect_state;

    /* update the autoconnect timer trigger */
    info->meta_sm.auto_connect_trigger =
      info->meta_sm.settings.auto_connect_enabled ? TRUE : FALSE;
  }
} /* rmnet_meta_smi_update_autoconnect_state() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_UPDATE_AUTOCONNECT_ROAM_STATUS()

  DESCRIPTION
    update roaming status whenever SS roaming status changed

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_update_autoconnect_roam_status
(
  void
)
{
  rmnet_instance_e_type  i;
  sys_roam_status_e_type roam_status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* for autoconnect, the roaming is default */
  roam_status = SYS_ROAM_STATUS_ON;

  /*-------------------------------------------------------------------------
    Find the current SS roaming status .
  -------------------------------------------------------------------------*/
  if(rmnet_smi_cm_info.inited)
  {
    if(SYS_SRV_STATUS_SRV == rmnet_smi_cm_info.ss_info.srv_status )
    {
      roam_status = rmnet_smi_cm_info.ss_info.roam_status;
    }
  }

  /*-------------------------------------------------------------------------
    update the roaming status and auto connect state.
  -------------------------------------------------------------------------*/
  if( rmnet_smi_cm_info.ss_roaming_status != roam_status)
  {
    LOG_MSG_INFO1_2("SS Roaming status has changed g_roaming status:%d, ss roaming_status:%d",
                    rmnet_smi_cm_info.ss_roaming_status, roam_status);

    rmnet_smi_cm_info.ss_roaming_status = roam_status;
    for (i = RMNET_INSTANCE_MIN; i < RMNET_INSTANCE_MAX; i++)
    {
      /* Check RMNET instance should be initialized*/
      if (rmnet_smi_info[i] != NULL)
      {
        /* Determine autoconnect state(Enable or disable) based on new setting */
        LOG_MSG_INFO1_1("Update new roaming status to Determine autoconnect state:%d",
                        i);
        rmnet_meta_smi_update_autoconnect_state(rmnet_smi_info[i]);

        /*-----------------------------------------------------------------------------------
        See the below TT for when the auto connect data session should be easblished
        based on various autoconnect related settings.
        ---------------------------------------------------------------------------------
        autconnect_  roam_     SS roaming   Packet Data .
        setting      setting   status       Session
        ---------------------------------------------------------------------------------
          0         0 or 0xFF    x           Not established.
        (default,   (default, (dont care)
         Disable)    roaming)
        ---------------------------------------------------------------------------------
          1         0 or 0xFF   1          Established - immediately,
        (Enable)   (roaming) (Roaming)     after power cycle, Lpm->online
                                           recover from loss of network,
        This setting support the backward compatibility
        ---------------------------------------------------------------------------------
          1        0 or 0xFF    0          Established - immediately,
        (Enable)   (Roaming) (Home)        after power cycle, Lpm->online
                                           recover from loss of network,
        ---------------------------------------------------------------------------------
          1           1         1          Not established
        (Enable)   (home)   (roaming)
        ---------------------------------------------------------------------------------
          1           1         0          Established - immediately,
        (Enable)   (home)     (home)       after power cycle, Lpm->online
                                           recover from loss of network,
        ----------------------------------------------------------------------------------
          2           X         X          if in a autoconnect packet call, no one started
        (Paused)  (dont care) (dont care)  Then will stop the packet call().

                                           After power cycled, it become enabled state and
                                           Data session will estabished.
        ----------------------------------------------------------------------------------
          0           X         X          if in a autoconnect packet call, no one started
        (Disable) (dont care) (dont care)  Then will stop the packet call().

        ---------------------------------------------------------------------------------------*/

        /*---------------------------------------------------------------------
          set auto_connect timer if autoconnect enabled, dtr is high and haven't
          started the data call. This will avoid the race condition during power
          up time (getting roaming_status after registered).

          Come back from roaming area to home, this will set auto_connect timer if
          autoconnect is enabled.
        ---------------------------------------------------------------------*/
        if( (rmnet_smi_info[i]->meta_sm.settings.auto_connect_enabled) &&
                (rmnet_meta_smi_verify_dtr_high(rmnet_smi_info[i]) == TRUE) )
        {
          if (!rmnet_meta_sm_in_call(i))
          {
            LOG_MSG_INFO1_0("Starting autoconnect timer in NULL state");
            timer_set( &(rmnet_smi_info[i]->meta_sm.autoconnect_timer),
                         rmnet_smi_info[i]->meta_sm.autoconnect_next_backoff_time,
                        0,
                        T_SEC );
            rmnet_smi_info[i]->meta_sm.autoconnect_next_backoff_time *= 2;
            if(rmnet_smi_info[i]->meta_sm.autoconnect_next_backoff_time >
               AUTOCONNECT_BACKOFF_TIMER_MAX_VALUE)
            {
              rmnet_smi_info[i]->meta_sm.autoconnect_next_backoff_time =
                AUTOCONNECT_BACKOFF_TIMER_MAX_VALUE;
            }
          }
          else
          {
            LOG_MSG_INFO2_0("Packet call isalready started");
          }
        }
      }
    }
  }
  else
  {
    LOG_MSG_INFO2_0("SS roaming status not changed");
  }
} /* rmnet_meta_smi_update_autoconnect_roam_status() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_REG_CM()

  DESCRIPTION
    Register the CM SS event notification

  PARAMETERS
    None

  RETURN VALUE
    TRUE  - registration succeeded
    FALSE - registration failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_smi_reg_cm
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Activate the registered callback functions with CM.
    Activating these by posting a DCC  (instead of doing in init()
    to avoid having a lot of cm events during ds_task init time
    leading to exhaustion of DCC cmd buffers
  -------------------------------------------------------------------------*/
  if( CM_CLIENT_OK != cm_client_act( rmnet_smi_cm_info.cm_client_id ) )
  {
    LOG_MSG_ERROR_0("CM client callback activation failed");
    return FALSE;
  }

  return TRUE;
} /* rmnet_meta_smi_reg_cm() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_IS_DEFAULT_SUB()

  DESCRIPTION
    This function will check if the given subscription id is Default 
    Data subscription or not

  PARAMETERS
     ss_sub_id  - subscription id

  RETURN VALUE
    True  - if given sub is Default Data Subscription
    False - if not

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_is_default_sub
(
  sys_modem_as_id_e_type sub_id
)
{
  sys_modem_as_id_e_type asub_id;
  boolean  ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /* Process SS event only if this is default data subscription*/
  asub_id = ds3g_get_ps_subs_id();

  if( (asub_id < SYS_MODEM_AS_ID_1) || (asub_id >= SYS_MODEM_AS_ID_MAX) ||
      (asub_id != sub_id) )
  {
     ret_val = FALSE;
  }
  return ret_val;
}

/*===========================================================================
  FUNCTION RMNET_META_SMI_CM_SS_EVENT_CB()

  DESCRIPTION
    Rmnet interface callback for CM serving system event.

    This function is called by CM when a serving system event occurs.
    Note that the callback is called in CM (i.e. not DS) context.

  PARAMETERS
    ss_event:     event that just occurred.
    ss_info_ptr:  current serving system info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_cm_ss_event_cb
(
  cm_ss_event_e_type            ss_event,    /* SS event that just occured */
  const cm_mm_msim_ss_info_s_type *  ss_info_ptr  /* ptr to serving system info */
)
{
  rmnet_smi_cmd_type * cmd_ptr;
  uint8   stack_id,i;
  sys_sys_mode_e_type curr_sys_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ss_info_ptr)
  {
    LOG_MSG_ERROR_0("ss_info_ptr is NULL");
    return;
  }
  else if(!rmnet_meta_smi_is_default_sub(ss_info_ptr->asubs_id))
  {
     LOG_MSG_INFO1_1("ss event for subs_id: %d, non default subscription.Ignore", 
                     ss_info_ptr->asubs_id);
    return;
  }
  if( (ss_info_ptr->number_of_stacks == 0) || (ss_info_ptr->number_of_stacks > CM_NO_STACKS) )
  {
    LOG_MSG_ERROR_2("invalid number of Stacks:%d in ss event %d",ss_info_ptr->number_of_stacks,ss_event);
    return;
  }

  /*By default it is main stack*/
  stack_id = 0;
  /*Get the current sysmode and select that stack*/
  curr_sys_mode = ds3g_get_current_preferred_mode_ex(ss_info_ptr->asubs_id);
  
  for ( i = 0; i < ss_info_ptr->number_of_stacks; i++ )
  {
    if (ss_info_ptr->stack_info[i].sys_mode == curr_sys_mode)
    {
      stack_id = i;
      break;
    }
  }
  /*-------------------------------------------------------------------------
    Post a cmd to rmnet (DS task)  if an ss event of interest has occured
  -------------------------------------------------------------------------*/
  switch(ss_event)
  {
    case CM_SS_EVENT_SRV_CHANGED:
    /* CM_SS_EVENT_INFO event will be generated by CM only in below cases:
      (1) after coming out of power collapse to convey latest ss_info snapshot
      (2) only if we explicitly request for ss info using
          cm_ss_cmd_get_ss_info() api */
    case CM_SS_EVENT_INFO:
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    case CM_SS_EVENT_SRV_NEW:
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
      cmd_ptr = (rmnet_smi_cmd_type *)RMNET_SMI_GET_CMD_BUF(DCC_RMNET_SM_CMD);
      if( cmd_ptr != NULL)
      {
        cmd_ptr->sm_id     = RMNET_META_SM;
        cmd_ptr->cmd_type  = RMNET_SM_CMD_NEW_CM_SS_INFO;
        cmd_ptr->data.cm_ss.info.changed_fields  = 
                       (ss_info_ptr->changed_fields_subs | ss_info_ptr->stack_info[stack_id].changed_fields);
        cmd_ptr->data.cm_ss.info.srv_status      = ss_info_ptr->stack_info[stack_id].srv_status;
        cmd_ptr->data.cm_ss.info.roam_status     = ss_info_ptr->stack_info[stack_id].roam_status;
        cmd_ptr->data.cm_ss.asubs_id         = ss_info_ptr->asubs_id;
        cmd_ptr->data.cm_ss.event = ss_event;
        dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr);
      }
      break;

    default:
      LOG_MSG_INFO2_1 ("Unhandled cm ss event (%d) ignoring", ss_event);
  }
} /* rmnet_meta_smi_cm_ss_event_cb() */


/*===========================================================================
  FUNCTION RMNET_META_SMI_PROCESS_CM_SS_EVENT()

  DESCRIPTION
    Process the CM SS event generated and call coresponding functions in
    Rmnet interface to update the roaming status
  PARAMETERS
    ss_event  :  CM SS event
    ss_info   :  serving system info obtained from CM SS event

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_smi_process_cm_ss_event
(
  cm_ss_event_e_type      ss_event,
  rmnet_meta_sm_cm_ss_info_type ss_info,
  sys_modem_as_id_e_type  asubs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process SS event only if this is default data subscription*/
   if(!rmnet_meta_smi_is_default_sub(asubs_id))
   {
      LOG_MSG_INFO1_1("ss event for subs_id: %d, non default subscription.Ignore", asubs_id);
      return;
   }

   LOG_MSG_INFO2_4("Processing CM SS event (%x), changed_fields = (0x%x) (0x%x), sub_id %d",
                   ss_event,
                   QWORD_HIGH(ss_info.changed_fields),
                   QWORD_LOW(ss_info.changed_fields),asubs_id);

  switch (ss_event)
  {
    case CM_SS_EVENT_SRV_NEW:
    case CM_SS_EVENT_SRV_CHANGED:
    case CM_SS_EVENT_INFO:
    {
      if ( ((ss_event == CM_SS_EVENT_SRV_CHANGED) &&
            (ss_info.changed_fields & RMNET_META_SM_CM_SRV_SYS_CHANGED_MASK))||
           (ss_event == CM_SS_EVENT_INFO) ||
           (ss_event == CM_SS_EVENT_SRV_NEW) )
      {
        /* Determine whether to signal serving system change */
        if ((rmnet_smi_cm_info.ss_info.srv_status != ss_info.srv_status) ||
            (rmnet_smi_cm_info.ss_info.roam_status != ss_info.roam_status))
          /* one of the fileds of ss_info has new information */
        {
          LOG_MSG_INFO2_0("Got new serving system info, saving");
          memscpy(&rmnet_smi_cm_info.ss_info,
                  sizeof(  rmnet_meta_sm_cm_ss_info_type),
                 &ss_info,
                 sizeof(  rmnet_meta_sm_cm_ss_info_type));
          rmnet_smi_cm_info.inited = TRUE;

          /* update the roaming status to determine the autoconnect state */
          rmnet_meta_smi_update_autoconnect_roam_status();
        }
        else
        {
          LOG_MSG_INFO3_0("No new info in SS event, ignoring");
        }
      }
      break;
    }

    default:
      LOG_MSG_ERROR_1("Shouldn't be getting this event (%d) here", ss_event);
      break;
  }
} /* rmnet_meta_smi_process_cm_ss_event() */

#if defined(FEATURE_IP_CALL)
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/*===========================================================================
  FUNCTION RMNET_META_SMI_OPRT_MODE_CHANGE_CB ()

  DESCRIPTION
    Callback function for starting operation mode change procedure

  PARAMETERS
    New operation mode
    New trans ID

  RETURN VALUE
    TRUE if succeeded in sending follow-up command
    FALSE otherwise

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_oprt_mode_change_cb
(
  sys_oprt_mode_e_type  new_oprt_mode,
  cmipapp_trans_id_type new_trans_id
)
{
  rmnet_smi_cmd_type   * cmd_ptr;

  LOG_MSG_INFO1_2("rmnet_meta_smi_oprt_mode_change_cb: new mode [%d] transaction ID %p",
                  new_oprt_mode, new_trans_id);

  /* Get a DCC command buffer */
  cmd_ptr = (rmnet_smi_cmd_type *)RMNET_SMI_GET_CMD_BUF_ASSERT(DCC_RMNET_SM_CMD);
  if( cmd_ptr == NULL )
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Post cmd to DS task corresponding to each event
  -------------------------------------------------------------------------*/
  cmd_ptr->sm_id     = RMNET_META_SM;
  cmd_ptr->cmd_type  = RMNET_SM_CMD_CM_OP_MODE_CHANGE;
  cmd_ptr->data.cm_op_mode_change.new_oprt_mode = new_oprt_mode;
  cmd_ptr->data.cm_op_mode_change.new_trans_id = new_trans_id;
  dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr);

  return TRUE;

} /* rmnet_meta_smi_oprt_mode_change_cb */


/*===========================================================================
  FUNCTION RMNET_META_SMI_CM_OP_MODE_CHANGE_CMD_HDLR()

  DESCRIPTION
    This function processes a RmNet RmSM CMIPAPP Operation mode change event cmd.

  PARAMETERS
    cmd_ptr:  serving task command buffer containing the cmipapp event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_cm_op_mode_change_cmd_hdlr
(
  rmnet_smi_cmd_type * cmd_ptr
)
{
  rmnet_instance_e_type        i;
  rmnet_smi_cmipapp_info_type *info_ptr = &rmnet_smi_cmipapp_info;

  ASSERT (cmd_ptr && (cmd_ptr->cmd_type == RMNET_SM_CMD_CM_OP_MODE_CHANGE) &&
  	          (cmd_ptr->sm_id    == RMNET_META_SM));

  LOG_MSG_INFO1_2("Enter rmnet_meta_smi_cm_op_mode_change_cmd_hdlr: "
                  "new mode [%d] transID %p",
                  info_ptr->new_oprt_mode, info_ptr->new_trans_id);

  rmnet_smi_cmipapp_info.new_trans_id    =
                      cmd_ptr->data.cm_op_mode_change.new_trans_id;
  rmnet_smi_cmipapp_info.new_oprt_mode   =
                      cmd_ptr->data.cm_op_mode_change.new_oprt_mode;

  switch(info_ptr->new_oprt_mode)
  {
    case SYS_OPRT_MODE_PWROFF:
    case SYS_OPRT_MODE_LPM:
      info_ptr->cur_oprt_mode = info_ptr->new_oprt_mode;
      info_ptr->cur_trans_id  = info_ptr->new_trans_id;

      /* Find any instance in packet call and disconnect it  */
      for(i = RMNET_INSTANCE_MIN; i < RMNET_INSTANCE_MAX; i++)
      {
        /* Delay any auto-connect timer as needed */
        rmnet_meta_sm_set_autoconnect_delay_timer (i,
                                        AUTOCONNECT_BACKOFF_TIMER_MAX_VALUE);
      }

    default: /* NOT in any power off mode  */

      /*---------------------------------------------------------------------
        If mode changed to any not power off mode (such as Online), need to
          - Respond complete of callback procedure
          - Clean up data data structure
      ---------------------------------------------------------------------*/
      cmipapp_oprt_mode_cnf (info_ptr->client_id,
                             info_ptr->new_trans_id);

      // clean up data structure "rmnet_smi_cmipapp_info"
      rmnet_meta_smi_initialize_cmipapp_info(); ;
      break;

  } /* switch() */

} /* rmnet_meta_smi_cm_op_mode_change_cmd_hdlr */

/*===========================================================================
  FUNCTION RMNET_META_SMI_INITIALIZE_CMIPAPP_INFO()

  DESCRIPTION
    Clean up rmnet_smi_cmipapp_info data structure

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_initialize_cmipapp_info
(
  void
)
{
  LOG_MSG_INFO2_0("rmnet_meta_smi_initialize_cmipapp_info");

  /* DO NOT initialize the client_id data element since it should contain
  ** the client id allocated(assigned) until re-allocated or sytem reset
  */

  /* Initialize others */
  rmnet_smi_cmipapp_info.cur_trans_id         = (cmipapp_trans_id_type) (-1);
  rmnet_smi_cmipapp_info.new_trans_id         = (cmipapp_trans_id_type) (-1);
  rmnet_smi_cmipapp_info.cur_oprt_mode        = SYS_OPRT_MODE_NONE;
  rmnet_smi_cmipapp_info.new_oprt_mode        = SYS_OPRT_MODE_NONE;
} /* rmnet_meta_smi_initialize_cmipapp_info */

#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */
#endif /* defined(FEATURE_IP_CALL) */

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_AUTOCONNECT_DELAY_TIMER()

  DESCRIPTION
    Delay the auto-connect timer

  PARAMETERS
    Starting time to delay

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_set_autoconnect_delay_timer
(
  rmnet_instance_e_type instance,
  uint32 delay_time
)
{
  rmnet_smi_info_type *  info;

  ASSERT(instance < RMNET_INSTANCE_MAX);

  info = rmnet_smi_info[instance];
  if (info == NULL)
  {
    return;
  }

  if (info->meta_sm.settings.auto_connect_enabled)
  {
    LOG_MSG_INFO2_1("rmnet_meta_sm_set_autoconnect_delay_timer = (%d)",
                    instance);

    /*---------------------------------------------------------------------
      if  data call is on going,
        - Just delay Auto connect backoff time as given
      else
        - Stop current auto-connect timer running
        - Delay Auto connect backoff time as given
        - Start auto-connect timer with the changed time
     ---------------------------------------------------------------------*/
    if (rmnet_meta_sm_in_call(instance))
    {
      info->meta_sm.autoconnect_next_backoff_time = delay_time;
    }
    else
    {
      (void) timer_clr( &(info->meta_sm.autoconnect_timer), T_NONE );
      info->meta_sm.autoconnect_next_backoff_time = delay_time;
      (void) timer_set( &(info->meta_sm.autoconnect_timer),
                          info->meta_sm.autoconnect_next_backoff_time, 0, T_SEC );
    }
  }

} /* rmnet_meta_sm_set_autoconnect_delay_timer */

/**************************************************************************
            EFS Configuration and Parsing Functions and Utilities
***************************************************************************/

/* ===========================================================================
FUNCTION RMNET_META_SM_GET_EFS_CONFIG_PARAMS

DESCRIPTION
  This function reads the file rmnet_config.txt from the efs root directory and
  parses it and saves the information. If the file is not present,
  or the content of the file is corrupted, creates a default file with
  just autoconnect to be INVALID so that the Autoconnect can be used from NV
if
  present.

PARAMETERS
  config_params : Output parameters containing the parsed EFS configuration.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void rmnet_meta_sm_get_efs_config_params
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
   * Global config
  -------------------------------------------------------------------------*/
  memset(&rmnet_efs_global_config_params,
                     0, sizeof(rmnet_efs_global_config_params));

  if (qmi_nv_read(QMI_NV_ITEM_RMNET_GLOBAL_CONFIG, 0, 0,
                  &rmnet_efs_global_config_params,
                  sizeof(rmnet_efs_global_config_params)) != QMI_NV_STATUS_OK)
  {
    /* Initialize global config params to defaults */
    rmnet_efs_global_config_params.ul_agg_prot_mask =
               (RMNET_GLOBAL_EFS_DATA_FORMAT_MBIM_MASK |
                RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_MASK |
                RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_V3_MASK);

    rmnet_efs_global_config_params.dl_agg_prot_mask =
               (RMNET_GLOBAL_EFS_DATA_FORMAT_MBIM_MASK |
                RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_MASK |
                RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_V3_MASK);
  }

  /*------------------------------------------------------------------------
   * Autoconnect settings
  -------------------------------------------------------------------------*/
  memset(&rmnet_efs_autoconnect_config,
                      0, sizeof(rmnet_efs_autoconnect_config));

  if (qmi_nv_read(QMI_NV_ITEM_RMNET_AUTOCONNECT, 0, 0,
                  rmnet_efs_autoconnect_config,
                  sizeof(rmnet_efs_autoconnect_config)) != QMI_NV_STATUS_OK)
  {
    memset(&rmnet_efs_autoconnect_config,
                   0, sizeof(rmnet_efs_autoconnect_config));
  }

  /*------------------------------------------------------------------------
   * $QCRMCALL table
  -------------------------------------------------------------------------*/
  memset(rmnet_qcrmcall_inst, 0, sizeof(rmnet_qcrmcall_inst));

} /* rmnet_meta_sm_get_efs_config_params() */

/*===========================================================================
FUNCTION RMNET_UM_IFACE_IN_USE

DESCRIPTION
  This function checks if the um iface is already used by
  another rmnet instance.

PARAMETERS
  info: pointer to the interface controller's control block
  check_arb: check arb in progress case

RETURN VALUE
  TRUE:  um iface is already used by another rmnet instance
  FALSE: um iface is not used by another rmnet instance

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
static boolean rmnet_um_iface_in_use
(
   rmnet_smi_info_type  * info,
   boolean                check_arb
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (info != NULL) && (info->um_iface_ptr != NULL) )
  {
    // check if um iface is already in use by another rmnet instance
    for (i = RMNET_INSTANCE_MIN; i < RMNET_INSTANCE_MAX; i++)
    {
      if ( (info != rmnet_smi_info[i]) &&
           (rmnet_smi_info[i] != NULL) &&
           (rmnet_smi_info[i]->um_iface_ptr == info->um_iface_ptr) &&
           (rmnet_smi_info[i]->um_iface_handle == info->um_iface_handle) &&
           (check_arb == FALSE || rmnet_smi_info[i]->meta_sm.arb_close_in_progress) )
      {
        if (rmnet_smi_info[i]->xport_info != NULL)
        {
          info->net_down_ep_id = rmnet_smi_info[i]->xport_info->ep_id;
          info->net_down_mux_id = rmnet_smi_info[i]->xport_info->mux_id;
          info->net_down_2_rmnet = TRUE;
        }
        LOG_MSG_INFO2_6("Um Iface [0x%x] handle [0x%x] is already used by "
                        "rmnet inst [%d] ep_id [0x%x] mux_id [%d] arb [%d]",
                        info->um_iface_ptr,
                        info->um_iface_handle,
                        i,
                        info->net_down_ep_id,
                        info->net_down_mux_id,
                        check_arb);
        return TRUE;
      }
    }
  }

  return FALSE;
} /* rmnet_um_iface_in_use */

/*===========================================================================
FUNCTION RMNET_TEAR_DOWN_UM_IFACE

DESCRIPTION
  This function tears down the um iface. The um iface must have already
  been brought up by rmnet_meta_smi_bring_up_um_iface_by_policy().

PARAMETERS
  info: pointer to the interface controller's control block

RETURN VALUE
  Tear down status

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
static int rmnet_tear_down_um_iface
(
  rmnet_smi_info_type   * info
)
{
  int16               ps_iface_errno = 0;
  int                 retval;

  ASSERT(info != NULL && info->um_iface_ptr != NULL);

  /*---------------------------------------------------------------------
     Try to tear down the UM iface. Passing NULL for policy
     since don't cache it.
  ---------------------------------------------------------------------*/
  retval = ps_route_tear_down_iface_by_policy(info->um_iface_ptr,
                                              NULL,
                                              &ps_iface_errno,
                                              NULL);

  LOG_MSG_INFO2_2( "ps iface tear down retval [%d], errno [%d]",
                   retval, ps_iface_errno );

  return retval;

} /* rmnet_tear_down_um_iface */

#if defined(FEATURE_RMNET_DATA_AGG_TIMER) && defined(FEATURE_CPU_BASED_FLOW_CONTROL)
/*===========================================================================
FUNCTION RMNET_META_SMI_FC_REGISTER

DESCRIPTION
  This function registers CPU usage signal with FC

PARAMETERS
  None

RETURN VALUE
  TRUE: Registration is succesfully
  FALSE: Registration failed

DEPENDENCIES
  rmnet_fc_cmd_q must be initialized.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean rmnet_meta_smi_fc_register
(
  void
)
{
  fc_in_cmd_type *fc_ptr = NULL;

  fc_ptr = fc_get_in_cmd_buf();
  if (fc_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Can't get FC cmd buffer");
    return FALSE;
  }

  fc_ptr->cmd_hdr.cmd_id = FC_IN_REGISTER_CMD;
  fc_ptr->cmd_data.fc_reg.tcb = rex_self();
  fc_ptr->cmd_data.fc_reg.q = &rmnet_fc_cmd_q;
  fc_ptr->cmd_data.fc_reg.sig = ((rex_sigs_type) 1 << DCC_RMNET_FC_SIGNAL);
  fc_ptr->cmd_data.fc_reg.cmd_id = 0;
  fc_ptr->cmd_data.fc_reg.fc_cpu_threshold.high = rmnet_da_cpu_high;
  fc_ptr->cmd_data.fc_reg.fc_cpu_threshold.low = rmnet_da_cpu_low;
  fc_ptr->cmd_data.fc_reg.fc_cpu_threshold.off = 0;

  fc_put_in_cmd(fc_ptr);
  LOG_MSG_INFO2_0("RMNET registered with FC");

  return TRUE;
} /* rmnet_meta_smi_fc_register */

/*===========================================================================
FUNCTION RMNET_META_SM_PROCESS_FC_SIG

DESCRIPTION
  This function handles signal from FC, set data aggregation mode accordingly.

PARAMETERS
  sig: signal from FC
  user_data_ptr: user data ptr comes with the signal

RETURN VALUE
  None

DEPENDENCIES
  rmnet_meta_smi_fc_register

SIDE EFFECTS
  None.

===========================================================================*/
static boolean rmnet_meta_sm_process_fc_sig
(
   dcc_sig_enum_type sig,
   void *user_data_ptr
)
{
  fc_out_cmd_type *cmd_ptr = NULL;
  fc_out_cmd_enum_type fc_cmd;

  while ( (cmd_ptr = (fc_out_cmd_type *)q_get(&rmnet_fc_cmd_q)) != NULL )
  {
    fc_cmd = cmd_ptr->cmd_hdr.cmd_id;
#ifndef FEATURE_FC_CPU_MONITOR
    mem_free(&tmc_heap_small, cmd_ptr);
#else
    fc_put_cmd_buf_in_free_q(cmd_ptr);
#endif /* FEATURE_FC_CPU_MONITOR */

    switch (fc_cmd)
    {
      case FC_OUT_UP_CMD:
        // CPU load < threshold.low, turn off DA
        if (rmnet_sio_da_enabled == TRUE)
        {
          LOG_MSG_INFO2_1("FC cmd %d, turn off DA", fc_cmd);
          rmnet_sio_da_enabled = FALSE;
        }
        break;

      case FC_OUT_DN_CMD:
        // CPU load > threshold.high, turn on DA
        if (rmnet_sio_da_enabled == FALSE)
        {
          LOG_MSG_INFO2_1("FC cmd %d, turn on DA", fc_cmd);
          rmnet_sio_da_enabled = TRUE;
        }
        break;

      default:
        break;
    }
  }

  return TRUE;
} /* rmnet_meta_sm_process_fc_sig */
#endif /* FEATURE_RMNET_DATA_AGG_TIMER && FEATURE_CPU_BASED_FLOW_CONTROL */

/*===========================================================================
FUNCTION RMNET_META_SM_IS_IFACE_AVAIL

DESCRIPTION
  This function checks if there is a free iface available.

PARAMETERS
  None

RETURN VALUE
  TRUE: Iface available
  FALSE: Iface unavailable

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean rmnet_meta_sm_is_iface_avail
(
)
{
  uint8 i;

  /* Check if there is a free Rmnet Iface*/
  for(i=0; i < RMNET_IFACE_MAX; i++)
  {
    if ( (rmnet_smi_iface_info[i] == NULL) ||
         (rmnet_smi_iface_info[i]->in_use == FALSE) )
    {
      return TRUE;
    }
  }
  return FALSE;
} /* rmnet_meta_sm_is_iface_avail */

/*===========================================================================
FUNCTION RMNET_SET_RM_IFACE_ADDR_FAMILY

DESCRIPTION
  Set RM iface addr family

PARAMETERS
  rm_iface_ptr: RM iface pointer
  addr_family : address family

RETURN VALUE
  TRUE if success. FALSE otherwise.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean rmnet_set_rm_iface_addr_family
(
  ps_iface_type    * rm_iface_ptr,
  uint8              addr_family
)
{
  uint8 ip_type = IFACE_ANY_ADDR_FAMILY;

  if (!PS_IFACE_IS_VALID(rm_iface_ptr))
  {
    ASSERT(0);
    return FALSE;
  }

  if ( (addr_family == IFACE_IPV4_ADDR_FAMILY) ||
       (addr_family == IFACE_IPV6_ADDR_FAMILY) )
  {
    ip_type = addr_family;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  rm_iface_ptr->iface_private.addr_family = ip_type;
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  LOG_MSG_INFO2_2("Setting RM iface %p addr family to %d",
                  rm_iface_ptr, ip_type);

  return TRUE;

} /* rmnet_set_rm_iface_addr_family() */

/*===========================================================================
FUNCTION RMNET_META_SMI_QOS_OR_TE_FC_ENABLED 

DESCRIPTION
  This function checks if QOS header or TE flow control is enabled

PARAMETERS
  info: RmNet meta smi pointer

DEPENDENCIES
  None

RETURN VALUE
 TRUE : If QOS or TE flow control is enabled
 FALSE: QOS and TE flow control are not enabled

SIDE EFFECTS
  None.
===========================================================================*/
static boolean rmnet_meta_smi_qos_or_te_fc_enabled
(
  rmnet_smi_info_type  * info
)
{
  rmnet_smi_dual_ip_info_type  * dual_ip_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (info != NULL)
  {
    dual_ip_info = rmnet_smi_dual_ip_info[info->constants.dual_ip_inst];
    if (dual_ip_info != NULL)
    {
      if (RMNET_XPORT_QOS_ENABLED(&dual_ip_info->xport_info) ||
          RMNET_XPORT_TE_FC_ENABLED(&dual_ip_info->xport_info))
      {
        return TRUE;
      }
    }
  }

  return FALSE;

} /* rmnet_meta_smi_qos_or_te_fc_enabled() */

/*===========================================================================
FUNCTION RMNET_META_SM_QOS_OR_TE_FC_ENABLED 

DESCRIPTION
  This function checks if QOS header or TE flow control is enabled

PARAMETERS
  instance       : RmNet instance

DEPENDENCIES
  None

RETURN VALUE
 TRUE : If QOS or TE flow control is enabled
 FALSE: QOS and TE flow control are not enabled

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rmnet_meta_sm_qos_or_te_fc_enabled
(
  rmnet_instance_e_type  instance
)
{
  rmnet_smi_info_type * info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (instance < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[instance];
  }

  return rmnet_meta_smi_qos_or_te_fc_enabled(info);

} /* rmnet_meta_sm_qos_or_te_fc_enabled() */

/*===========================================================================
FUNCTION RMNET_META_SM_BIND

DESCRIPTION
  This function binds a logical rmnet instance to a physical port

PARAMETERS
  ep_id:       physical port end point id
  mux_id:      logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
boolean rmnet_meta_sm_bind
(
  uint32          ep_id,
  uint8           mux_id
)
{
  rmnet_phys_xport_type  * phys_xport;
  int                      i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ep_id == 0)
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
   * Get the phys xport
  -------------------------------------------------------------------------*/
  phys_xport = rmnet_phys_xport_from_ep_id(ep_id);
  if (phys_xport == NULL)
  {
    LOG_MSG_ERROR_1("Unknown EP 0x%x", ep_id);
    return  FALSE;
  }

  /*------------------------------------------------------------------------
   * Find existing rmnet instance
  -------------------------------------------------------------------------*/
  for (i = 0; i < RMNET_DUAL_IP_MAX; i++)
  {
    if (rmnet_smi_dual_ip_info[i] != NULL &&
        rmnet_smi_dual_ip_info[i]->xport_info.ep_id == ep_id &&
        rmnet_smi_dual_ip_info[i]->xport_info.mux_id == mux_id)
    {
      LOG_MSG_INFO1_2("Rmnet EP 0x%x mux_id %d already bound", ep_id, mux_id);
      return TRUE;
    }
  }

  /*------------------------------------------------------------------------
   * Check if we need to put the instance in legacy section
  -------------------------------------------------------------------------*/
  i = QMI_INSTANCE_MAX;
  if (mux_id == 0)
  {
    i = qmux_get_inst_from_ep_id(ep_id);
  }

  /*-------------------------------------------------------------------------
   * Allocate a new rmnet instance in the mux section
  -------------------------------------------------------------------------*/
  if (i >= QMI_INSTANCE_MAX)
  {
    for (i = QMI_INSTANCE_MAX; i < RMNET_DUAL_IP_MAX; i++)
    {
      if (rmnet_smi_dual_ip_info[i] == NULL ||
          rmnet_smi_dual_ip_info[i]->xport_info.ep_id == 0)
      {
        break;
      }
    }
  }

  if (i >= RMNET_DUAL_IP_MAX)
  {
    LOG_MSG_ERROR_0("No available RMNET instance");
    return FALSE;
  }

  if (rmnet_smi_dual_ip_info[i] == NULL)
  {
    if ( !rmnet_meta_sm_init_dual_ip_instance(i) )
    {
      LOG_MSG_ERROR_1("RMNET dual IP inst %d open failed", i);
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------------
   * Bind to phys xport
  -------------------------------------------------------------------------*/
  if ( !rmnet_logical_xport_bind(
                  &rmnet_smi_dual_ip_info[i]->xport_info,
                  phys_xport,
                  mux_id) )
  {
    LOG_MSG_ERROR_2("Unable to bind RMNET EP 0x%x mux_id %d", ep_id, mux_id);
    return FALSE;
  }
  else
  {
    LOG_MSG_INFO1_3("Rmnet EP 0x%x mux_id %d bound to dual IP inst %d",
                    ep_id, mux_id, i);
  }

  /*-------------------------------------------------------------------------
   * Set default rmnet parameters
  -------------------------------------------------------------------------*/
  rmnet_meta_smi_set_default_net_params(rmnet_smi_dual_ip_info[i]->info_v4);
  rmnet_meta_smi_set_default_net_params(rmnet_smi_dual_ip_info[i]->info_v6);

  /*-------------------------------------------------------------------------
   * Set autoconnect settings
  -------------------------------------------------------------------------*/
  rmnet_meta_smi_set_default_autoconn_params(rmnet_smi_dual_ip_info[i]->info_v4);
  rmnet_meta_smi_set_default_autoconn_params(rmnet_smi_dual_ip_info[i]->info_v6);

  /*-------------------------------------------------------------------------
   * Get manual DNS settings
  -------------------------------------------------------------------------*/
  if (rmnet_set_manual_dns_settings(rmnet_smi_dual_ip_info[i]) == FALSE)
  {
    LOG_MSG_ERROR_0("Error setting manual dns settings");
  }
  return TRUE;

} /* rmnet_meta_sm_bind() */

/*===========================================================================
FUNCTION RMNET_META_SM_UNBIND

DESCRIPTION
  This function unbinds a logical rmnet instance from a physical port

PARAMETERS
  ep_id:         physical port end point id
  mux_id:        logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
boolean rmnet_meta_sm_unbind
(
  uint32          ep_id,
  uint8           mux_id
)
{
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ep_id == 0)
  {
    return FALSE;
  }

  /*------------------------------------------------------------------------
   * Find existing rmnet instance
  -------------------------------------------------------------------------*/
  for (i = 0; i < RMNET_DUAL_IP_MAX; i++)
  {
    if (rmnet_smi_dual_ip_info[i] != NULL &&
        rmnet_smi_dual_ip_info[i]->xport_info.ep_id == ep_id &&
        rmnet_smi_dual_ip_info[i]->xport_info.mux_id == mux_id)
    {
      break;
    }
  }

  if (i >= RMNET_DUAL_IP_MAX)
  {
    LOG_MSG_ERROR_2("Rmnet unbind ep 0x%x mux_id %d Not found", ep_id, mux_id);
    return FALSE;
  }
  /*----------------------------------------------------------------------------
    We need to unbind here even if autoconnect is enabled. Because, in 
    physical_xport close we are clearing out the xport_list. And we aren't 
    binding it to logical port in sm_bind as it wasn't unbound earlier.So DTR 
    callback won't be called when physical xport is opened again causing 
    autoconnect failure.
  ---------------------------------------------------------------------------*/
#if 0
  /*------------------------------------------------------------------------
   * If autoconnect is enabled, no need to unbind.
  -------------------------------------------------------------------------*/
  if (rmnet_meta_smi_get_autoconnect_config(ep_id, mux_id) != NULL)
  {
    LOG_MSG_INFO1_2("Autconnect enabled, no unbind for ep_id 0x%x mux_id %d",
                         ep_id, mux_id);
    return TRUE;
  }
#endif

  /*-------------------------------------------------------------------------
   * Unbind the xport
  -------------------------------------------------------------------------*/
  rmnet_logical_xport_unbind(&rmnet_smi_dual_ip_info[i]->xport_info);

  LOG_MSG_INFO1_1("Rmnet dual IP inst %d unbound", i);

  return TRUE;

} /* rmnet_meta_sm_unbind() */

/*===========================================================================
FUNCTION RMNET_INST_FROM_EP_AND_MUX_ID

DESCRIPTION
  This function retrieves rmnet instance from ep_id and mux_id

PARAMETERS
  ep_id:       physical port end point id
  mux_id:      logical rmnet mux id
  is_default:  is default instance

DEPENDENCIES
  None

RETURN VALUE
  rmnet instance number
  RMNET_INSTANCE_MAX if failure

SIDE EFFECTS
  None.
===========================================================================*/
rmnet_instance_e_type rmnet_inst_from_ep_and_mux_id
(
  uint32     ep_id,
  uint8      mux_id,
  boolean    is_default
)
{
  rmnet_smi_dual_ip_info_type  * dual_ip_info;
  rmnet_instance_e_type          rmnet_inst = RMNET_INSTANCE_MAX;
  int                            i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < RMNET_DUAL_IP_MAX; i++)
  {
    dual_ip_info = rmnet_smi_dual_ip_info[i];
    if ( dual_ip_info != NULL &&
         dual_ip_info->xport_info.mux_id == mux_id &&
         dual_ip_info->xport_info.ep_id == ep_id )
    {
      rmnet_inst = (rmnet_instance_e_type) 
                         (is_default ? i << 1 : (i << 1) + 1); 
    }
  }

  return rmnet_inst;

} /* rmnet_inst_from_ep_and_mux_id() */

/*===========================================================================
FUNCTION RMNET_GET_NET_DOWN_EP_AND_MUX_ID

DESCRIPTION
  This function retrieves the ep_id and mux_id of rmnet instance which has
  call already up and caused the call on the current rmnet instance to be
  torn down.

PARAMETERS
  rmnet_inst:  rmnet instance
  ep_id:       output address of ep_id
  mux_id:      output address of mux_id

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean rmnet_get_net_down_ep_and_mux_id
(
  rmnet_instance_e_type    rmnet_inst,
  uint32                 * ep_id,
  uint8                  * mux_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rmnet_inst >= RMNET_INSTANCE_MAX ||
      rmnet_smi_info[rmnet_inst] == NULL ||
      ep_id == NULL || mux_id == NULL)
  {
    LOG_MSG_ERROR_1("Invalid parameters %d", rmnet_inst);
    return FALSE;
  }

  *ep_id = rmnet_smi_info[rmnet_inst]->net_down_ep_id;
  *mux_id = rmnet_smi_info[rmnet_inst]->net_down_mux_id;

  return TRUE;

} /* rmnet_get_net_down_ep_and_mux_id() */

/*===========================================================================
FUNCTION RMNET_META_SMI_EP_CFG_DONE_CB

DESCRIPTION
  This is rmnet callback function for EP config done event

PARAMETERS
  user_info:  pointer to rmnet_smi_dual_ip_info structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void rmnet_meta_smi_ep_cfg_done_cb
(
  void  * user_info
)
{
  rmnet_smi_dual_ip_info_type  * dual_ip_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dual_ip_info = (rmnet_smi_dual_ip_info_type *)user_info;
  if (dual_ip_info != NULL)
  {
    if (dual_ip_info->info_v4 != NULL)
    {
      rmnet_meta_smi_handle_ep_cfg_done_ev(dual_ip_info->info_v4);
    }
    if (dual_ip_info->info_v6 != NULL)
    {
      rmnet_meta_smi_handle_ep_cfg_done_ev(dual_ip_info->info_v6);
    }
  }

} /* rmnet_meta_smi_ep_cfg_done_cb() */

/*===========================================================================
FUNCTION RMNET_META_SMI_IN_CALL_CB

DESCRIPTION
  This is rmnet callback function to check in call status

PARAMETERS
  user_info:  pointer to rmnet_smi_dual_ip_info structure

DEPENDENCIES
  None

RETURN VALUE
  TRUE: rmnet is in call
  FALSE: rmnet is not in call

SIDE EFFECTS
  None.
===========================================================================*/
static boolean rmnet_meta_smi_in_call_cb
(
  void  * user_info
)
{
  rmnet_smi_dual_ip_info_type  * dual_ip_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dual_ip_info = (rmnet_smi_dual_ip_info_type *)user_info;
  if (dual_ip_info != NULL)
  {
    if ( rmnet_meta_sm_in_call(
               RMNET_META_SM_INFO_TO_INSTANCE(dual_ip_info->info_v4)) ||
         rmnet_meta_sm_in_call(
               RMNET_META_SM_INFO_TO_INSTANCE(dual_ip_info->info_v6)) )
    {
      return TRUE;
    }
  }

  return FALSE;

} /* rmnet_meta_smi_in_call_cb() */

/*===========================================================================
FUNCTION RMNET_META_SMI_DTR_CB

DESCRIPTION
  This is rmnet DTR callback function

PARAMETERS
  user_info:  pointer to rmnet_smi_dual_ip_info structure
  dtr_assert: is DTR asserted

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void rmnet_meta_smi_dtr_cb
(
  void   * user_info,
  boolean  dtr_asserted
)
{
  rmnet_smi_dual_ip_info_type  * dual_ip_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dual_ip_info = (rmnet_smi_dual_ip_info_type *)user_info;
  if (dual_ip_info != NULL)
  {
    rmnet_meta_smi_process_dtr_changed_cmd(
                         dual_ip_info->info_v4, dtr_asserted);
    rmnet_meta_smi_process_dtr_changed_cmd(
                         dual_ip_info->info_v6, dtr_asserted);
  }

} /* rmnet_meta_smi_dtr_cb() */

/*===========================================================================
FUNCTION RMNET_META_SMI_DO_RX_FC

DESCRIPTION
  This function handles flow enable or disable events

PARAMETERS
  info:         pointer to rmnet smi info 
  fc_mask:      flow control mask indentifies who is sending fc event
  disable_flow: is flow disabled (flow control enabled)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void rmnet_meta_smi_do_rx_fc
(
  rmnet_smi_info_type  * info,
  uint32                 fc_mask,
  boolean                disable_flow
)
{
  uint32  old_mask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (info == NULL ||
      info->meta_sm.state == RMNET_META_SM_NULL_STATE ||
      info->meta_sm.state == RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE)
  {
    return;
  }

  // If QoS or TE flow control is enabled, QMI QoS will send FC indications
  if (rmnet_meta_smi_qos_or_te_fc_enabled(info))
  {
    return;
  }

  // Send WDS/QMAP FC indications
  old_mask = info->rx_fc_mask;

  if (disable_flow)
  {
    info->rx_fc_mask |= fc_mask;
    if (old_mask == 0 && info->rx_fc_mask != 0)
    {
      wds_rmnet_flow_ctrl_cb(
                 RMNET_EP_ID(info),
                 RMNET_MUX_ID(info),
                 RMNET_META_SM_INFO_TO_INSTANCE(info) & 0x01,
                 TRUE);
    }
  }
  else
  {
    info->rx_fc_mask &= (~fc_mask);
    if (old_mask != 0 && info->rx_fc_mask == 0)
    {
      wds_rmnet_flow_ctrl_cb(
                 RMNET_EP_ID(info),
                 RMNET_MUX_ID(info),
                 RMNET_META_SM_INFO_TO_INSTANCE(info) & 0x01,
                 FALSE);
    }
  }

  LOG_MSG_INFO2_1("RX FC mask 0x%x", info->rx_fc_mask);

} /* rmnet_meta_smi_do_rx_fc() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_DO_RX_WMK_FC()

  DESCRIPTION
    This function handles RX watermark flow control.

  PARAMETERS
    dual_ip_info:  rmnet dual ip info pointer
    disable:       flow disable

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_do_rx_wmk_fc
(
  rmnet_smi_dual_ip_info_type  * dual_ip_info,
  boolean                        disable
)
{
  rmnet_data_agg_enum_type       ul_agg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dual_ip_info == NULL || 
      dual_ip_info->info_v4 == NULL || dual_ip_info->info_v6 == NULL)
  {
    return;
  }

  ul_agg = RMNET_XPORT_UL_DATA_AGG(&dual_ip_info->xport_info);

  /*------------------------------------------------------------------------
   * If QMAP is used, don't do SIO flow control because this will
   * flow control all PDNs. AP is supposed to do either WDS/QoS or QMAP
   * in-band flow control if QMAP is enabled.
  -------------------------------------------------------------------------*/
  if (RMNET_DATA_AGG_IS_QMAP(ul_agg) &&
      RMNET_XPORT_IN_BAND_FC(&dual_ip_info->xport_info)) // XXX Remove this check
  {
    if (rmnet_meta_smi_qos_or_te_fc_enabled(dual_ip_info->info_v4))
    {
      // XXX Convert to QOS/QMAP flow control
    }
    else
    {
      // Convert to WDS/QMAP flow control
      rmnet_meta_smi_do_rx_fc(
                     dual_ip_info->info_v4,
                     RMNET_FC_MASK_SIO,
                     disable);

      rmnet_meta_smi_do_rx_fc(
                     dual_ip_info->info_v6,
                     RMNET_FC_MASK_SIO,
                     disable);
    }
  }
  else if (ul_agg == RMNET_ENABLE_DATA_AGG_MBIM &&
           dual_ip_info->xport_info.data_format_cache.mbim_sw_deagg)
  {
    // MBIM specific
    rmnet_phys_xport_set_flow_mask(
               dual_ip_info->xport_info.phys_xport,
               1 << dual_ip_info->xport_info.mux_id,
               disable);
  }
  else
  {
    // Do SIO flow control
    rmnet_logical_xport_set_rx_flow(&dual_ip_info->xport_info, disable);
  }

} /* rmnet_meta_smi_do_rx_wmk_fc() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_RX_HIWATER_FUNC()

  DESCRIPTION
    Receive watermark high callback.
    Disable rx flow on the data link specified by user_data

  PARAMETERS
    wm_ptr          -  receive watermark on which rx data is enqueued
    user_data_ptr   -  rmnet_xport_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_rx_hiwater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
)
{
  rmnet_xport_type  * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&rmnet_crit_section);

  xport_info = (rmnet_xport_type *)user_data_ptr;
  if (xport_info != NULL && !xport_info->rx_wmk_fc)
  {
    xport_info->rx_wmk_fc = TRUE;
    rmnet_meta_smi_do_rx_wmk_fc(
           (rmnet_smi_dual_ip_info_type *)xport_info->user_info, TRUE);
  }

  PS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

} /* rmnet_meta_smi_rx_hiwater_func() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_RX_LOWATER_FUNC()

  DESCRIPTION
    Receive watermark low callback.
    Enable rx flow on the data link specified by user_data

  PARAMETERS
    wm_ptr           -  receive watermark on which rx data is enqueued
    user_data_ptr    -  rmnet_xport_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_rx_lowater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
)
{
  rmnet_xport_type  * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&rmnet_crit_section);

  xport_info = (rmnet_xport_type *)user_data_ptr;
  if (xport_info != NULL && xport_info->rx_wmk_fc)
  {
    xport_info->rx_wmk_fc = FALSE;
    rmnet_meta_smi_do_rx_wmk_fc(
           (rmnet_smi_dual_ip_info_type *)xport_info->user_info, FALSE);
  }

  PS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

} /* rmnet_meta_smi_rx_lowater_func() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_DO_TX_WMK_FC()

  DESCRIPTION
    This function handles RX watermark flow control.

  PARAMETERS
    dual_ip_info:  rmnet dual ip info pointer
    disable:       flow disable

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_do_tx_wmk_fc
(
  rmnet_smi_dual_ip_info_type  * dual_ip_info,
  boolean                        disable
)
{
  rmnet_smi_cmd_type  * cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (dual_ip_info == NULL)
  {
    return;
  }

  cmd_ptr = (rmnet_smi_cmd_type *)
                    RMNET_SMI_GET_CMD_BUF(DCC_RMNET_SM_CMD);
  if (cmd_ptr == NULL)
  {
    return;
  }

  cmd_ptr->sm_id    = RMNET_META_SM;
  cmd_ptr->info_ptr = dual_ip_info;

  if (disable)
  {
    cmd_ptr->cmd_type = RMNET_SM_CMD_TX_FLOW_DISABLE;
  }
  else /* enable */
  {
    cmd_ptr->cmd_type = RMNET_SM_CMD_TX_FLOW_ENABLE;
  }

  dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr);

  LOG_MSG_INFO2_1("TX FLOW CTRL: %c", disable ? '+' : '-');

} /* rmnet_meta_smi_do_tx_wmk_fc() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_TX_HIWATER_FUNC()

  DESCRIPTION
    Receive watermark high callback.
    Disable tx flow on the data link specified by user_data

  PARAMETERS
    tx_wm_ptr       -  receive watermark on which tx data is enqueued
    user_data_ptr   -  rmnet_xport_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_tx_hiwater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
)
{
  rmnet_xport_type  * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&rmnet_crit_section);

  xport_info = (rmnet_xport_type *)user_data_ptr;
  if (xport_info != NULL && !xport_info->tx_wmk_fc)
  {
    xport_info->tx_wmk_fc = TRUE;
    rmnet_meta_smi_do_tx_wmk_fc(
           (rmnet_smi_dual_ip_info_type *)xport_info->user_info, TRUE);
  }

  PS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

} /* rmnet_meta_smi_tx_hiwater_func() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_TX_LOWATER_FUNC()

  DESCRIPTION
    Receive watermark low callback.
    Enable tx flow on the data link specified by user_data

  PARAMETERS
    wm_ptr           -  receive watermark on which tx data is enqueued
    user_data_ptr    -  rmnet_xport_type ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_tx_lowater_func
(
  struct dsm_watermark_type_s * wm_ptr,
  void                        * user_data_ptr
)
{
  rmnet_xport_type  * xport_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&rmnet_crit_section);

  xport_info = (rmnet_xport_type *)user_data_ptr;
  if (xport_info != NULL && xport_info->tx_wmk_fc)
  {
    xport_info->tx_wmk_fc = FALSE;
    rmnet_meta_smi_do_tx_wmk_fc(
           (rmnet_smi_dual_ip_info_type *)xport_info->user_info, FALSE);
  }

  PS_LEAVE_CRIT_SECTION(&rmnet_crit_section);

} /* rmnet_meta_smi_tx_lowater_func() */

/*===========================================================================
FUNCTION RMNET_SEND_QMAP_CMD

DESCRIPTION
  This function sends a QMAP command

PARAMETERS
  rmnet_inst:  rmnet instance
  cmd_size:    QMAP command size
  cmd_ptr:     QMAP command pointer

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rmnet_send_qmap_cmd
(
  rmnet_instance_e_type    rmnet_inst,
  uint16                   cmd_size,
  void                   * cmd_ptr
)
{
  rmnet_smi_info_type  * info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rmnet_inst < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[rmnet_inst];
  }

  if (info != NULL)
  {
    return rmnet_logical_xport_send_qmap_cmd(
               info->xport_info, cmd_size, cmd_ptr);
  }

  return FALSE;

} /* rmnet_send_qmap_cmd() */

/*===========================================================================
  FUNCTION RMNET_SET_MANUAL_DNS_SETTINGS()

  DESCRIPTION
    Sets the manual DNS settings used for the instance associated with ep_id and mux_id.

  PARAMETERS
    ep_id - End point id associated with the instance

  RETURN VALUE
    TRUE  - update successful
    FALSE - udpate failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_set_manual_dns_settings
(
  rmnet_smi_dual_ip_info_type *info
)
{
  qmi_nv_dns_addr_type  qmi_ipv4_dns_nv_item;
  qmi_nv_ipv6_dns_addr_type qmi_ipv6_dns_nv_item;
  qmi_nv_status_e_type qmi_nv_status = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*Setting Primary IPv4 Dns*/
  memset(&qmi_ipv4_dns_nv_item, 0, sizeof(qmi_ipv4_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv4_dns_nv_item,
                              sizeof(qmi_ipv4_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      LOG_MSG_INFO2_1("Manual IPv4 DNS setting (pri) read [%x]",
              qmi_ipv4_dns_nv_item.dns_addr);
      info->info_v4->meta_sm.settings.pri_dns_addr.type = IPV4_ADDR;
      info->info_v4->meta_sm.settings.pri_dns_addr.addr.v4 = qmi_ipv4_dns_nv_item.dns_addr;
    
  }
  else
  {
    LOG_MSG_INFO1_0("No Manual Primary Dns set");
  }

  /*Setting Secondary DNS*/
  memset(&qmi_ipv4_dns_nv_item, 0, sizeof(qmi_ipv4_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv4_dns_nv_item,
                              sizeof(qmi_ipv4_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      LOG_MSG_INFO2_1("Manual IPv4 DNS setting (sec) read [%x]",
              qmi_ipv4_dns_nv_item.dns_addr);
      info->info_v4->meta_sm.settings.sec_dns_addr.type = IPV4_ADDR;
      info->info_v4->meta_sm.settings.sec_dns_addr.addr.v4 = qmi_ipv4_dns_nv_item.dns_addr;
  }
  else
  {
    LOG_MSG_INFO1_0("No Manual Secondary Dns set");
  }

  /*Setting Secondary Ipv6 DNS*/
  memset(&qmi_ipv6_dns_nv_item, 0, sizeof(qmi_ipv6_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv6_dns_nv_item,
                              sizeof(qmi_ipv6_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      LOG_MSG_INFO2_2("Manual IPv6 primary DNS setting (pri) read [%x], [%x]",
              qmi_ipv6_dns_nv_item.dns_addr[0],qmi_ipv6_dns_nv_item.dns_addr[1]);
      info->info_v6->meta_sm.settings.ipv6_pri_dns_addr.type = IPV6_ADDR;
      info->info_v6->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[0] = qmi_ipv6_dns_nv_item.dns_addr[0];
      info->info_v6->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[1] = qmi_ipv6_dns_nv_item.dns_addr[1];
  }
  else
  {
    LOG_MSG_INFO1_0("No Manual Ipv6 Primary Dns set");
  }

  /*Setting Secondary Ipv6 DNS*/
  memset(&qmi_ipv6_dns_nv_item, 0, sizeof(qmi_ipv6_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv6_dns_nv_item,
                              sizeof(qmi_ipv6_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      LOG_MSG_INFO2_2("Manual IPv6 secondary DNS setting (sec) read [%x], [%x]",
              qmi_ipv6_dns_nv_item.dns_addr[0],qmi_ipv6_dns_nv_item.dns_addr[1]);
      info->info_v6->meta_sm.settings.ipv6_sec_dns_addr.type = IPV6_ADDR;
      info->info_v6->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[0] = qmi_ipv6_dns_nv_item.dns_addr[0];
      info->info_v6->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[1] = qmi_ipv6_dns_nv_item.dns_addr[1];
  }
  else
  {
    LOG_MSG_INFO1_0("No Manual Ipv6 Secondary Dns set");
  }
  /* need to set IPv6 addresses on both instances (IPv4+6 and IPv6-only) to keep
     them in sync */
  memset(&qmi_ipv4_dns_nv_item, 0, sizeof(qmi_ipv4_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv4_dns_nv_item,
                              sizeof(qmi_ipv4_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      LOG_MSG_INFO2_1("Manual IPv4 DNS setting (pri) read [%x]",
                    qmi_ipv4_dns_nv_item.dns_addr);
      info->info_v6->meta_sm.settings.pri_dns_addr.type = IPV4_ADDR;
      info->info_v6->meta_sm.settings.pri_dns_addr.addr.v4 = qmi_ipv4_dns_nv_item.dns_addr;
    
  }
  else
  {
    LOG_MSG_INFO1_0("No Manual Primary Dns set");
  }

  /*Setting Secondary DNS*/
  memset(&qmi_ipv4_dns_nv_item, 0, sizeof(qmi_ipv4_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv4_dns_nv_item,
                              sizeof(qmi_ipv4_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      LOG_MSG_INFO2_1("Manual IPv4 DNS setting (sec) read [%x]",
                     qmi_ipv4_dns_nv_item.dns_addr);
      info->info_v6->meta_sm.settings.sec_dns_addr.type = IPV4_ADDR;
      info->info_v6->meta_sm.settings.sec_dns_addr.addr.v4 = qmi_ipv4_dns_nv_item.dns_addr;
  }
  else
  {
    LOG_MSG_INFO1_0("No Manual Secondary Dns set");
  }

  /*Setting Secondary Ipv6 DNS*/
  memset(&qmi_ipv6_dns_nv_item, 0, sizeof(qmi_ipv6_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv6_dns_nv_item,
                              sizeof(qmi_ipv6_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      LOG_MSG_INFO2_2("Manual IPv6 primary DNS setting (pri) read [%x], [%x]",
              qmi_ipv6_dns_nv_item.dns_addr[0],qmi_ipv6_dns_nv_item.dns_addr[1]);
      info->info_v4->meta_sm.settings.ipv6_pri_dns_addr.type = IPV6_ADDR;
      info->info_v4->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[0] = qmi_ipv6_dns_nv_item.dns_addr[0];
      info->info_v4->meta_sm.settings.ipv6_pri_dns_addr.addr.v6[1] = qmi_ipv6_dns_nv_item.dns_addr[1];
  }
  else
  {
    LOG_MSG_INFO1_0("No Manual Ipv6 Primary Dns set");
  }

  /*Setting Secondary Ipv6 DNS*/
  memset(&qmi_ipv6_dns_nv_item, 0, sizeof(qmi_ipv6_dns_nv_item));
  qmi_nv_status = qmi_nv_read(QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC,
                              info->xport_info.ep_id,
                              info->xport_info.mux_id,
                              &qmi_ipv6_dns_nv_item,
                              sizeof(qmi_ipv6_dns_nv_item));
  if (QMI_NV_STATUS_OK == qmi_nv_status)
  {
      LOG_MSG_INFO2_2("Manual IPv6 secondary DNS setting (sec) read [%x], [%x]",
              qmi_ipv6_dns_nv_item.dns_addr[0],qmi_ipv6_dns_nv_item.dns_addr[1]);
      info->info_v4->meta_sm.settings.ipv6_sec_dns_addr.type = IPV6_ADDR;
      info->info_v4->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[0] = qmi_ipv6_dns_nv_item.dns_addr[0];
      info->info_v4->meta_sm.settings.ipv6_sec_dns_addr.addr.v6[1] = qmi_ipv6_dns_nv_item.dns_addr[1];
  }
  else
  {
    LOG_MSG_INFO1_0("No Manual Ipv6 Secondary Dns set");
  }

  return TRUE;
} /* rmnet_set_manual_dns_settings() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_UPDATE_ETH_HEADER()

  DESCRIPTION
     Updates Ethernet header

  PARAMETERS

  RETURN VALUE
    TRUE - Success
    FALSE - Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean rmnet_meta_smi_update_eth_header
(
  rmnet_smi_info_type  * info
)
{
  llc_frm_packed_mode_zero_type  eth_hdr;
  ps_ip_addr_type                ip_addr;
  ps_dpm_rm_eth_hdr_info_type    eth_hdr_info;
  boolean                        is_v6 = FALSE;
  uint8                          status = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (info == NULL)
  {
    ASSERT(0);
    return FALSE;
  }

  if (info->um_iface_ptr == NULL || info->rmnet_iface_ptr == NULL)
  {
    status = 1;
    goto send_result;
  }

  // Get destination MAC address
  if (ps_iface_addr_family_is_v4(info->um_iface_ptr))
  {
    ip_addr.type = IPV4_ADDR;
    ps_iface_get_cached_addr(info->um_iface_ptr, &ip_addr);
    eth_hdr.ethertype = ps_htons(LAN_ETHERTYPE_IPV4);

    if (arp_get_dest_mac_addr(
              info->constants.lan_llc_instance,
              &ip_addr,
              eth_hdr.dst_hw_addr,
              LAN_IEEE_MAC_ADDR_LEN) != 0)
    {
      status = 2;
      goto send_result;
    }
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if (ps_iface_addr_family_is_v6(info->um_iface_ptr))
  {
    ip_addr.type = IPV6_ADDR;
    if (!ps_iface_get_ipv6_ext_addr(info->um_iface_ptr, &ip_addr))
    {
      status = 3;
      goto send_result;
    }
    eth_hdr.ethertype = ps_htons(LAN_ETHERTYPE_IPV6);
    is_v6 = TRUE;

    if (ps_icmp6_get_dest_mac_addr(
                     info->rmnet_iface_ptr,
                     &(ip_addr.addr.v6),
                     eth_hdr.dst_hw_addr,
                     LAN_IEEE_MAC_ADDR_LEN) != 0)
    {
      status = 4;
      goto send_result;
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */
  else
  {
    status = 5;
    goto send_result;
  }

  // Get source MAC address
  memscpy(eth_hdr.src_hw_addr,
          LAN_IEEE_MAC_ADDR_LEN,
          info->rmnet_iface_ptr->iface_private.hw_addr,
          LAN_IEEE_MAC_ADDR_LEN);

  // Notify HW of the Ethernet header
  if (!rmnet_logical_xport_update_eth_header(
          info->xport_info, &eth_hdr, (uint8)sizeof(eth_hdr), is_v6))
  {
    status = 6;
    goto send_result;
  }

  // Notify DPM
  eth_hdr_info.sio_stream_id = info->xport_info->stream_id;
  eth_hdr_info.rm_iface_ptr  = info->rmnet_iface_ptr;
  eth_hdr_info.eth_hdr       = eth_hdr;

  ps_dpm_rm_config_eth_hdr(&eth_hdr_info);

send_result:
  if (status)
  {
    LOG_MSG_ERROR_2("Failed to update Eth header rmnet inst %d, status %d",
          RMNET_META_SM_INFO_TO_INSTANCE(info), status);
    return FALSE;
  }

  LOG_MSG_INFO1_6("Dest mac addr %x:%x:%x:%x:%x:%x",
                     eth_hdr.dst_hw_addr[0],
                     eth_hdr.dst_hw_addr[1],
                     eth_hdr.dst_hw_addr[2],
                     eth_hdr.dst_hw_addr[3],
                     eth_hdr.dst_hw_addr[4],
                     eth_hdr.dst_hw_addr[5]);

  return TRUE;

} /* rmnet_meta_smi_update_eth_header() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_LLC_TX_DATA()

  DESCRIPTION
    This function is called by the bound lan llc instance to transmit a framed
    packet over the RmNet interface.

  PARAMETERS
    pkt           -  message (in dsm chain) to be transmitted
    user_data     -  user data passed to lan_llc_start(),
                     contains rmnet_xport_info ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_llc_tx_data
(
  dsm_item_type  ** pkt,
  void            * user_data
)
{
  rmnet_logical_xport_transmit((rmnet_xport_type *)user_data, pkt);

} /* rmnet_meta_smi_llc_tx_data() */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_EP_MUX_ID_BY_UM_IFACE()

  DESCRIPTION
    Returns the ep id and mux id for the given Um iface ptr(physical iface)

  PARAMETERS
    um_iface_ptr : The Um iface ptr for which the proc_id is sought

  RETURN VALUE
    TRUE     : Success
    FALSE    : Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_get_ep_mux_id_by_um_iface
(
  ps_iface_type                  *um_iface_ptr,
  uint32                         *ep_id,
  uint8                          *mux_id
)
{
  uint32                        inst;
  ps_iface_type                 *assoc_iface_ptr    = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!um_iface_ptr)
  {
    LOG_MSG_ERROR_0("Null um_iface");
    return FALSE;
  }
  
  for (inst = RMNET_INSTANCE_MIN; inst < RMNET_INSTANCE_MAX; inst++)
  {
    if (rmnet_smi_info[inst] != NULL)
    {
      if (rmnet_smi_info[inst]->um_iface_ptr)
      {
        assoc_iface_ptr = 
          PS_IFACE_GET_ASSOC_IFACE(rmnet_smi_info[inst]->um_iface_ptr);

        /* If there is a match with given um iface(MH iface) retrieve
           ep and mux id */
        if ((assoc_iface_ptr != NULL) && (assoc_iface_ptr == um_iface_ptr))
        {
          *ep_id   =  rmnet_smi_info[inst]->xport_info->ep_id;
          *mux_id  =  rmnet_smi_info[inst]->xport_info->mux_id;

          LOG_MSG_INFO3_2("Ep id %d, mux_id %d", *ep_id, *mux_id);
          return TRUE;
        }
      }
    }
  } 

  return FALSE;
} /* rmnet_meta_sm_get_ep_mux_id_by_um_iface */

/*===========================================================================
  FUNCTION RMNET_QCRMCALL_INST_ADD()

  DESCRIPTION
    This function adds the rmnet instance to the qcrmcall table

  PARAMETERS
    ep_id:     End point ID
    mux_id:    Mux ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_qcrmcall_inst_add
(
  uint32   ep_id,
  uint8    mux_id
)
{
  int  i;
  int  err = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rmnet_inst_from_ep_and_mux_id(
            ep_id, mux_id, TRUE) >= RMNET_INSTANCE_MAX)
  {
    err = 1;
    goto send_result;
  }

  // Check if ep_id, mux_id is already in the tbl
  for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
  {
    if (rmnet_qcrmcall_inst[i].ep_id == ep_id &&
        rmnet_qcrmcall_inst[i].mux_id == mux_id)
    {
      goto send_result;
    }
  }

  // Find an available slot
  for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
  {
    if (rmnet_qcrmcall_inst[i].ep_id == 0)
    {
      memset(&rmnet_qcrmcall_inst[i], 0, sizeof(rmnet_qcrmcall_inst_type));
      rmnet_qcrmcall_inst[i].ep_id = ep_id;
      rmnet_qcrmcall_inst[i].mux_id = mux_id;
      goto send_result;
    }
  }

  if (i >= RMNET_QCRMCALL_MAX)
  {
    err = 2;
    goto send_result;
  }

send_result:
  if (err != 0)
  {
    LOG_MSG_ERROR_3("Failed to add qcrmcall inst for "
               "ep_id 0x%x, mux_id %d, err %d",
               ep_id, mux_id, err);
  }
  else
  {
    LOG_MSG_INFO2_2("Added qcrmcall inst ep_id 0x%x mux_id %d",
              ep_id, mux_id);
  }

} /* rmnet_qcrmcall_inst_add() */

/*===========================================================================
  FUNCTION RMNET_QCRMCALL_INST_REMOVE()

  DESCRIPTION
    This function removes qcrmcall instance

  PARAMETERS
    ep_id:     End point ID

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_qcrmcall_inst_remove
(
  uint32   ep_id
)
{
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ep_id == 0)
  {
    return;
  }

  for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
  {
    if (rmnet_qcrmcall_inst[i].ep_id == ep_id)
    {
      memset(&rmnet_qcrmcall_inst[i], 0, sizeof(rmnet_qcrmcall_inst_type));
    }
  }

} /* rmnet_qcrmcall_inst_remove() */

/*===========================================================================
  FUNCTION RMNET_SEND_QCRMCALL_REQUEST

  DESCRIPTION
    This function sends a qcrmcall request to RMNET

  PARAMETERS
    action:        qcrmcall reqeust type
    request:       request info
    callback:      Callback to be invoked for sending result 

  RETURN VALUE
    TRUE: success
    FALSE: failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_send_qcrmcall_request
(
  rmnet_qcrmcall_action_e_type    action,
  rmnet_qcrmcall_request_type   * request,
  rmnet_qcrmcall_cb_type          callback
)
{
  rmnet_smi_cmd_type               * cmd_ptr;
  rmnet_meta_sm_network_info_type  * sni_params_ptr = NULL;
  int                                err = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Sanity checks
    -----------------------------------------------------------------------*/
    if (action != QCRMCALL_ACTION_STOP &&
        action != QCRMCALL_ACTION_START &&
        action != QCRMCALL_ACTION_QUERY)
    {
      err = 1;
      break;
    }

    if (request == NULL)
    {
      err = 2;
      break;
    }

    if (action == QCRMCALL_ACTION_START)
    {
      if (request->start.sni_params == NULL)
      {
        err = 3;
        break;
      }

      PS_SYSTEM_HEAP_MEM_ALLOC(sni_params_ptr,
                      sizeof(rmnet_meta_sm_network_info_type),
                      rmnet_meta_sm_network_info_type*);
      if (sni_params_ptr == NULL)
      {
        err = 4;
        break;
      }

      memscpy(sni_params_ptr, sizeof(rmnet_meta_sm_network_info_type),
            request->start.sni_params, sizeof(rmnet_meta_sm_network_info_type));
    }

    /*-----------------------------------------------------------------------
     Get a DCC command buffer
     -----------------------------------------------------------------------*/
    cmd_ptr = (rmnet_smi_cmd_type *)RMNET_SMI_GET_CMD_BUF(DCC_RMNET_SM_CMD);
    if (cmd_ptr == NULL)
    {
      if (sni_params_ptr != NULL)
      {
        PS_SYSTEM_HEAP_MEM_FREE(sni_params_ptr);
      }
      err = 5;
      break;
    }

    /*-----------------------------------------------------------------------
       Post cmd to DS task.
    -----------------------------------------------------------------------*/
    cmd_ptr->sm_id                  = RMNET_META_SM;
    cmd_ptr->cmd_type               = RMNET_SM_CMD_QCRMCALL;
    cmd_ptr->data.qcrmcall.action   = action;
    cmd_ptr->data.qcrmcall.request  = * request;
    cmd_ptr->data.qcrmcall.callback = callback;

    if (action == QCRMCALL_ACTION_START)
    {
      cmd_ptr->data.qcrmcall.request.start.sni_params = sni_params_ptr;
    }

    dcc_send_cmd_ex(DCC_RMNET_SM_CMD, (void *)cmd_ptr);

  } while (0);

  if (err)
  {
    LOG_MSG_ERROR_2("Failed to send qcrmcall action [%d] err %d",
                       action, err);
    return FALSE;
  }

  return TRUE;

} /* rmnet_send_qcrmcall_request() */

/*===========================================================================
  FUNCTION RMNET_QCRMCALL_INST_TO_RMNET_INST()

  DESCRIPTION
    This function returns rmnet instance from qcrmcall instance

  PARAMETERS
    qcrmcall_inst: qcrmcall instance
    ip_family:     IP family

  RETURN VALUE
    rmnet instance

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 rmnet_qcrmcall_inst_to_rmnet_inst
(
  uint8  qcrmcall_inst,
  uint8  ip_family
)
{
  uint8  rmnet_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qcrmcall_inst >= RMNET_QCRMCALL_MAX ||
      rmnet_qcrmcall_inst[qcrmcall_inst].ep_id == 0)
  {
    return RMNET_INSTANCE_MAX;
  }

  rmnet_inst = rmnet_inst_from_ep_and_mux_id(
                   rmnet_qcrmcall_inst[qcrmcall_inst].ep_id,
                   rmnet_qcrmcall_inst[qcrmcall_inst].mux_id,
                   ip_family != IPV6_ADDR);

  return rmnet_inst;

} /* rmnet_qcrmcall_inst_to_rmnet_inst() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_PROCESS_QCRMALL_CMD

  DESCRIPTION
    This function handles a qcrmcall request

  PARAMETERS
    action:        qcrmcall reqeust type
    request:       request info
    callback:      Callback to be invoked for sending result 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_process_qcrmcall_cmd
(
  rmnet_qcrmcall_action_e_type    action,
  rmnet_qcrmcall_request_type   * request,
  rmnet_qcrmcall_cb_type          callback
)
{
  uint8                       rmnet_inst;
  uint8                       qcrmcall_inst;
  rmnet_qcrmcall_result_type  result;
  int                         err = 0;
  int                         i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (request == NULL)
  {
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_1("$QCRMCALL action [%d]", action);

  memset(&result, 0, sizeof(result));

  if (action == QCRMCALL_ACTION_START)
  {
    qcrmcall_inst = request->start.qcrmcall_inst;

    result.sni.qcrmcall_inst = qcrmcall_inst;
    if (request->start.sni_params != NULL)
    {
      result.sni.ip_family = request->start.sni_params->ip_family_pref;
    }
    else
    {
      result.sni.ip_family = 0;
      result.sni.status = FALSE;
      err = 1;
      goto send_result;
    }

    if (qcrmcall_inst >= RMNET_QCRMCALL_MAX)
    {
      PS_SYSTEM_HEAP_MEM_FREE(request->start.sni_params);
      result.sni.status = FALSE;
      err = 2;
      goto send_result;
    }

    rmnet_inst = rmnet_qcrmcall_inst_to_rmnet_inst(
                   request->start.qcrmcall_inst,
                   request->start.sni_params->ip_family_pref);
    if (rmnet_inst >= RMNET_INSTANCE_MAX)
    {
      PS_SYSTEM_HEAP_MEM_FREE(request->start.sni_params);
      result.sni.status = FALSE;
      err = 3;
      goto send_result;
    }

    if (rmnet_meta_sm_in_call(rmnet_inst))
    {
      PS_SYSTEM_HEAP_MEM_FREE(request->start.sni_params);
      result.sni.status = FALSE;
      err = 4;
      goto send_result;
    }

    if (request->start.sni_params->ip_family_pref == IPV6_ADDR)
    {
      rmnet_qcrmcall_inst[qcrmcall_inst].v6_state = RMNET_QCRMCALL_STATE_START;
      rmnet_qcrmcall_inst[qcrmcall_inst].v6_cb = callback;
    }
    else
    {
      rmnet_qcrmcall_inst[qcrmcall_inst].v4_state = RMNET_QCRMCALL_STATE_START;
      rmnet_qcrmcall_inst[qcrmcall_inst].v4_cb = callback;
    }

    // Start the call
    rmnet_meta_sm_set_network_cfg_params(
                  rmnet_inst, request->start.sni_params);
    rmnet_meta_smi_handle_rm_dev_start_pkt_ev(rmnet_smi_info[rmnet_inst]);

    PS_SYSTEM_HEAP_MEM_FREE(request->start.sni_params);
    return;
  }
  else if (action == QCRMCALL_ACTION_STOP)
  {
    qcrmcall_inst = request->stop.qcrmcall_inst;

    result.sni.qcrmcall_inst = qcrmcall_inst;
    result.sni.ip_family = request->stop.ip_family;

    if (request->stop.qcrmcall_inst >= RMNET_QCRMCALL_MAX)
    {
      result.sni.status = FALSE;
      err = 5;
      goto send_result;
    }

    rmnet_inst = rmnet_qcrmcall_inst_to_rmnet_inst(
                   request->stop.qcrmcall_inst,
                   request->stop.ip_family);
    if (rmnet_inst >= RMNET_INSTANCE_MAX)
    {
      result.sni.status = FALSE;
      err = 6;
      goto send_result;
    }

    if (!rmnet_meta_sm_in_call(rmnet_inst))
    {
      result.sni.status = FALSE;
      err = 7;
      goto send_result;
    }

    if (request->stop.ip_family == IPV6_ADDR)
    {
      if (rmnet_qcrmcall_inst[qcrmcall_inst].v6_state != RMNET_QCRMCALL_STATE_START &&
          rmnet_qcrmcall_inst[qcrmcall_inst].v6_state != RMNET_QCRMCALL_STATE_UP)
      {
        result.sni.status = FALSE;
        err = 8;
        goto send_result;
      }
      // If there is a pending start, send failure response
      if (rmnet_qcrmcall_inst[qcrmcall_inst].v6_state == RMNET_QCRMCALL_STATE_START)
      {
        if (rmnet_qcrmcall_inst[qcrmcall_inst].v6_cb)
        {
          result.sni.status = FALSE;
          rmnet_qcrmcall_inst[qcrmcall_inst].v6_cb(QCRMCALL_ACTION_START, &result);
        }
      }
      rmnet_qcrmcall_inst[qcrmcall_inst].v6_state = RMNET_QCRMCALL_STATE_STOP;
      rmnet_qcrmcall_inst[qcrmcall_inst].v6_cb = callback;
    }
    else
    {
      if (rmnet_qcrmcall_inst[qcrmcall_inst].v4_state != RMNET_QCRMCALL_STATE_START &&
          rmnet_qcrmcall_inst[qcrmcall_inst].v4_state != RMNET_QCRMCALL_STATE_UP)
      {
        result.sni.status = FALSE;
        err = 9;
        goto send_result;
      }
      // If there is a pending start, send failure response
      if (rmnet_qcrmcall_inst[qcrmcall_inst].v4_state == RMNET_QCRMCALL_STATE_START)
      {
        if (rmnet_qcrmcall_inst[qcrmcall_inst].v4_cb)
        {
          result.sni.status = FALSE;
          rmnet_qcrmcall_inst[qcrmcall_inst].v4_cb(QCRMCALL_ACTION_START, &result);
        }
      }
      rmnet_qcrmcall_inst[qcrmcall_inst].v4_state = RMNET_QCRMCALL_STATE_STOP;
      rmnet_qcrmcall_inst[qcrmcall_inst].v4_cb = callback;
    }

    // Stop the call
    rmnet_meta_smi_handle_rm_link_down_ev(rmnet_smi_info[rmnet_inst]);
    return;
  }
  else if (action == QCRMCALL_ACTION_QUERY)
  {
    for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
    {
      if (rmnet_qcrmcall_inst[i].ep_id != 0)
      {
        if (rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_UP)
        {
          result.list.qcrmcall[result.list.num_qcrmcall].qcrmcall_inst = i;
          result.list.qcrmcall[result.list.num_qcrmcall++].ip_family = IPV4_ADDR;
        }
        if (rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_UP)
        {
          result.list.qcrmcall[result.list.num_qcrmcall].qcrmcall_inst = i;
          result.list.qcrmcall[result.list.num_qcrmcall++].ip_family = IPV6_ADDR;
        }
      }
    }
  }

send_result:
  if (action == QCRMCALL_ACTION_START || action == QCRMCALL_ACTION_STOP)
  {
    LOG_MSG_ERROR_2("$QCRMCALL action [%d] failed err %d", action, err);
  }

  if (callback)
  {
    callback(action, &result);
  }

} /* rmnet_meta_smi_process_qcrmcall_cmd() */

/*===========================================================================
  FUNCTION RMNET_META_SMI_UPDATE_QCRMALL_STATE

  DESCRIPTION
    This function updates qcrmcall state

  PARAMETERS
    info: rmnet smi info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void rmnet_meta_smi_update_qcrmcall_state
(
  rmnet_smi_info_type * info
)
{
  rmnet_smi_dual_ip_info_type  * dual_ip_info;
  rmnet_qcrmcall_action_e_type   action = QCRMCALL_ACTION_NONE;
  rmnet_qcrmcall_result_type     result;
  int                            i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (info == NULL)
  {
    ASSERT(0);
    return;
  }

  dual_ip_info = rmnet_smi_dual_ip_info[info->constants.dual_ip_inst];
  if (dual_ip_info == NULL)
  {
    ASSERT(0);
    return;
  }

  // Check if this is a qcrmcall instance
  for (i = 0; i < RMNET_QCRMCALL_MAX; i++)
  {
    if (rmnet_qcrmcall_inst[i].ep_id == RMNET_EP_ID(info) &&
        rmnet_qcrmcall_inst[i].mux_id == RMNET_MUX_ID(info))
    {
      break;
    }
  }

  if (i >= RMNET_QCRMCALL_MAX)
  {
    return;
  }

  // Send qcrmcall result if triggered by atcop
  memset(&result, 0, sizeof(result));
  result.sni.qcrmcall_inst = i;

  if (info == dual_ip_info->info_v4)
  {
    if (info->meta_sm.state == RMNET_META_SM_NET_IF_UP_STATE && 
        rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_START)
    {
      rmnet_qcrmcall_inst[i].v4_state = RMNET_QCRMCALL_STATE_UP;
      action = QCRMCALL_ACTION_START;
      result.sni.status = TRUE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_START)
    {
      rmnet_qcrmcall_inst[i].v4_state = RMNET_QCRMCALL_STATE_NONE;
      action = QCRMCALL_ACTION_START;
      result.sni.status = FALSE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_STOP)
    {
      rmnet_qcrmcall_inst[i].v4_state = RMNET_QCRMCALL_STATE_NONE;
      action = QCRMCALL_ACTION_STOP;
      result.sni.status = TRUE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v4_state == RMNET_QCRMCALL_STATE_UP)
    {
      rmnet_qcrmcall_inst[i].v4_state = RMNET_QCRMCALL_STATE_NONE;
    }

    // Send result and reset qcrmcall state
    if (action != QCRMCALL_ACTION_NONE && rmnet_qcrmcall_inst[i].v4_cb)
    {
      result.sni.ip_family = IPV4_ADDR;
      rmnet_qcrmcall_inst[i].v4_cb(action, &result);
      rmnet_qcrmcall_inst[i].v4_cb = NULL;
      return;
    }
  }

  if (info == dual_ip_info->info_v6)
  {
    if (info->meta_sm.state == RMNET_META_SM_NET_IF_UP_STATE && 
        rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_START)
    {
      rmnet_qcrmcall_inst[i].v6_state = RMNET_QCRMCALL_STATE_UP;
      action = QCRMCALL_ACTION_START;
      result.sni.status = TRUE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_START)
    {
      rmnet_qcrmcall_inst[i].v6_state = RMNET_QCRMCALL_STATE_NONE;
      action = QCRMCALL_ACTION_START;
      result.sni.status = FALSE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_STOP)
    {
      rmnet_qcrmcall_inst[i].v6_state = RMNET_QCRMCALL_STATE_NONE;
      action = QCRMCALL_ACTION_STOP;
      result.sni.status = TRUE;
    }
    else if (info->meta_sm.state == RMNET_META_SM_NULL_STATE && 
             rmnet_qcrmcall_inst[i].v6_state == RMNET_QCRMCALL_STATE_UP)
    {
      rmnet_qcrmcall_inst[i].v6_state = RMNET_QCRMCALL_STATE_NONE;
    }

    // Send result and reset qcrmcall state
    if (action != QCRMCALL_ACTION_NONE && rmnet_qcrmcall_inst[i].v6_cb)
    {
      result.sni.ip_family = IPV6_ADDR;
      rmnet_qcrmcall_inst[i].v6_cb(action, &result);
      rmnet_qcrmcall_inst[i].v6_cb = NULL;
      return;
    }
  }

} /* rmnet_meta_smi_update_qcrmcall_state() */

/*===========================================================================
FUNCTION RMNET_META_SM_UM_IFACE_IN_USE

DESCRIPTION
  This function checks if the um iface is already used by
  another rmnet instance.

PARAMETERS
  um_iface_ptr: um iface to check
  ep_id:        ptr to ep_id which has um iface present
  mux_id:       ptr to mux_id which has um iface present 

RETURN VALUE
  TRUE:  um iface is already used by another rmnet instance
  FALSE: um iface is not used by another rmnet instance

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean rmnet_meta_sm_um_iface_in_use
(
  ps_iface_type  * um_iface_ptr,
  uint32         * ep_id,
  uint8          * mux_id
)
{
  rmnet_smi_info_type  * info;
  int                    i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (um_iface_ptr == NULL || ep_id == NULL || mux_id == NULL)
  {
    return FALSE;
  }

  for (i = 0; i < RMNET_INSTANCE_MAX; i++)
  {
    info = rmnet_smi_info[i];
    if (info != NULL && info->um_iface_ptr == um_iface_ptr &&
        info->xport_info != NULL)
    {
      *ep_id = info->xport_info->ep_id;
      *mux_id = info->xport_info->mux_id;
      return TRUE;
    }
  }

  return FALSE;

} /* rmnet_meta_sm_um_iface_in_use() */

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_SUBS_ID

  DESCRIPTION
    This function sets the subs id of RMNET instance

  PARAMETERS
    rmnet_inst: RmNet instance
    subs_id : Subscription identifier

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_meta_sm_set_subs_id
(
  rmnet_instance_e_type rmnet_inst,
  uint32                subs_id
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rmnet_inst < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[rmnet_inst];
  }

  if (info != NULL)
  {
    info->subs_id = subs_id;
    LOG_MSG_INFO2_2("Setting subs %d, RMNET inst %d", subs_id, rmnet_inst);
  }
}/* rmnet_meta_sm_set_subs_id */

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_SUBS_ID

  DESCRIPTION
    This function gets the subs id of RMNET instance

  PARAMETERS
    rmnet_inst: RmNet instance
    subs_id : Subscription identifier

  RETURN VALUE
    TRUE  - Success
    FALSE - Failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_sm_get_subs_id
(
  rmnet_instance_e_type   rmnet_inst,
  uint32                * subs_id
)
{
  rmnet_smi_info_type *  info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rmnet_inst < RMNET_INSTANCE_MAX)
  {
    info = rmnet_smi_info[rmnet_inst];
  }

  if (info != NULL)
  {
    *subs_id = info->subs_id;
    LOG_MSG_INFO2_2("Returning subs %d, RMNET inst %d", *subs_id, rmnet_inst);
    return TRUE;
  }

  return FALSE;
}/* rmnet_meta_sm_get_subs_id */
