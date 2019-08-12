#ifndef _DS_RMNET_SMI_H
#define _DS_RMNET_SMI_H
/*===========================================================================

                        D S _ R M N E T _ S M I . H

DESCRIPTION

  Rm Network device - State machine definitions internal header file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2007-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/src/ds_rmnet_smi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/10/14    vrk    CM SS events handling for SGLTE+G CM redesign
09/27/13    wc     Support dynamic port configuration
06/10/13    pgm    Added flow control for Legacy Wmk.
07/11/12    pgm    Changing Legacy Wmk DNE cnt to a finite value.
07/11/12    pgm    RNDIS agg mode support.
07/10/12    pgm    Fix for routing packets to the right PDN in a multi-PDN
                   call after flow is enabled.
04/12/12    pgm    RmNet Legacy path definitions.
03/13/12    wc     Use iface handle to determine if two um ifaces are the same
02/01/12    cy     Support for packet filter feature
01/31/12    wc     Increase RMNET_RX_WM_DNE again
12/09/11    rt     Added the support for Stateless DHCPv6 server.
10/12/11    am     Added 9x15 SoftAP support.
10/13/11    wc     Increase RMNET_RX_WM_DNE
10/04/11    sa     Q6 free floating changes for QMI.
09/22/11    am     Added DL MBIM support.
05/09/11    asn    Adding support for new active Iface event
01/11/11    kk     Added support for data_call_type TLV in data_call_status
                   indication.
02/14/2011  ua     Added support for DUAL IP Autoconnect.
03/09/11    kk     Removing DCC command buffer dependency from RmNet.
07/09/10    asn    Support for Arbitration
06/19/10    vs     Changes for Dual IP support over single QMI instance
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
08/31/07    mct    Created file
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "timer.h"

#include "ps_iface.h"
#include "ps_phys_link.h"
#include "ps_lan_llc.h"
#include "ps_svc.h"
#include "ps_acl.h"

#include "dsm.h"
#include "ds_qmi_svc_ext.h"
#include "ds_rmnet_sm_ext.h"
#include "ds_rmnet_meta_sm.h"
#include "ds_rmnet_v4_sm.h"
#include "ds_rmnet_v6_sm.h"
#include "ds_rmnet_utils.h"
#include "ds_rmnet_xport.h"

#include "ps_lan_llci.h"
#include "ps_dpm_dl_opt.h"
#include "ps_iface_mbim_defs.h"

#include "dhcp6_sl_server_mgr.h"
/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

#define RMNET_ROAM_IND_FILE_MAX_LEN   256

/*---------------------------------------------------------------------------
  RMNET SIO Watermark level definitions
---------------------------------------------------------------------------*/
#define RMNET_RX_WM_LOW   15000
#define RMNET_RX_WM_HIGH  45000
#define RMNET_RX_WM_DNE   250000
#define RMNET_RX_WM_Q_CNT_DNE 1500

#define RMNET_LLC_RX_WM_LOW   15000
#define RMNET_LLC_RX_WM_HIGH  25000
#define RMNET_LLC_RX_WM_DNE   50000

/* RMNET_TX_WM_DNE = DSM_DS_SMALL_ITEM_CNT * DSM_DS_SMALL_ITEM_SIZ
Static counts of DSM have changed */
#define RMNET_TX_WM_DNE       456576
#define RMNET_TX_WM_Q_CNT_DNE 2000

#define RMNET_META_SM_INFO_TO_INSTANCE(info) \
                           ((info)->constants.rmnet_inst)

#define RMNET_EP_ID(info)  RMNET_XPORT_EP_ID((info)->xport_info)
#define RMNET_MUX_ID(info) RMNET_XPORT_MUX_ID((info)->xport_info)

/*---------------------------------------------------------------------------
  Data format macros
---------------------------------------------------------------------------*/
#define RMNET_LINK_PROT(info)   RMNET_XPORT_LINK_PROT((info)->xport_info)
#define RMNET_QOS_ENABLED(info) RMNET_XPORT_QOS_ENABLED((info)->xport_info)
#define RMNET_TE_FC_ENABLED(info) RMNET_XPORT_TE_FC_ENABLED((info)->xport_info)
#define RMNET_UL_DATA_AGG(info) RMNET_XPORT_UL_DATA_AGG((info)->xport_info)
#define RMNET_DL_DATA_AGG(info) RMNET_XPORT_DL_DATA_AGG((info)->xport_info)
#define RMNET_NDP_SIG(info)     RMNET_XPORT_NDP_SIG((info)->xport_info)
#define RMNET_IN_BAND_FC(info)  RMNET_XPORT_IN_BAND_FC((info)->xport_info)
#define RMNET_QOS_HEADER_TYPE(info)  RMNET_XPORT_QOS_HEADER_TYPE((info)->xport_info)

#define RMNET_DATA_AGG_IS_QMAP(data_agg) \
       ((data_agg) == RMNET_ENABLE_DATA_AGG_QMAP || \
        (data_agg) == RMNET_ENABLE_DATA_AGG_QMAP_V2 || \
        (data_agg) == RMNET_ENABLE_DATA_AGG_QMAP_V3)

/*---------------------------------------------------------------------------
  Enum type indicating the RmNet state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_INVALID_SM = 0,
  RMNET_META_SM    = 1,
  RMNET_V4_SM      = 2,
  RMNET_V6_SM      = 3,
  RMNET_MAX_SM
} rmnet_smi_e_type;

/*---------------------------------------------------------------------------
  Declaration of the states of the Meta RmNet state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_META_SM_MIN_STATE                 = -1,
  RMNET_META_SM_NULL_STATE                = 0,   /* Initial state           */
  RMNET_META_SM_UM_CONFIGURING_STATE      = 1,   /* Um coming up            */
  RMNET_META_SM_RM_CONFIGURING_STATE      = 2,   /* Rm and Um up            */
  RMNET_META_SM_NET_IF_UP_STATE           = 3,
  RMNET_META_SM_RM_RECONFIGURING_STATE    = 4,   /* Um is temp unavailable  */
  RMNET_META_SM_WAITING_FOR_UM_DOWN_STATE = 5,   /* Um going down, Rm down  */
  RMNET_META_SM_WAITING_FOR_UM_ARB_STATE  = 6,
  RMNET_META_SM_EP_CONFIGURING_STATE      = 7,
  RMNET_META_SM_MAX_STATE
} rmnet_meta_smi_state_type;

/*---------------------------------------------------------------------------
  Declaration of the states of the IPv6 RmNet state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_V4_SM_MIN_STATE               = -1,
  RMNET_V4_SM_NULL_STATE              = 0, /* Initial state           */
  RMNET_V4_SM_DHCP_WAITING_STATE      = 1, /* Waiting for DHCP
                                              server to start         */
  RMNET_V4_SM_DHCP_IN_PROGRESS_STATE  = 2, /* Rm configuring DHCP     */
  RMNET_V4_SM_ADDR_CFG_COMPLETE_STATE = 3, /* Rm address configured   */
  RMNET_V4_SM_MAX_STATE
} rmnet_v4_smi_state_type;

/*---------------------------------------------------------------------------
  Declaration of the states of the IPv6 RmNet state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_V6_SM_MIN_STATE                 = -1,
  RMNET_V6_SM_NULL_STATE                = 0,   /* Initial state           */
  RMNET_V6_SM_DHCP6_WAITING_STATE       = 1,   /* Wait for DHCP6 start    */
  RMNET_V6_SM_AUTOCFG_IN_PROGRESS_STATE = 2,   /* Addr cfg in progress    */
  RMNET_V6_SM_ADDR_CFG_COMPLETE_STATE   = 3,   /* Addr cfg successful     */
  RMNET_V6_SM_WAIT_FOR_LL_ADDR_RESOL_STATE = 4,/* wait for LL addr resolution*/
  RMNET_V6_SM_MAX_STATE
} rmnet_v6_smi_state_type;

typedef enum
{
  RMNET_SM_CMD_TX_FLOW_ENABLE  = 1,    /* RmNet iface flow enable        */
  RMNET_SM_CMD_TX_FLOW_DISABLE = 2,    /* RmNet iface flow disable       */
  RMNET_SM_CMD_DHCP_MSG        = 3,    /* RmNet iface dhcp message event */
  RMNET_SM_CMD_CM_CLIENT_ACTIVATE = 7, /* CM Client activation           */
  RMNET_SM_CMD_NEW_CM_SS_INFO     = 8, /* CM SS roaming status change    */
  RMNET_SM_CMD_CM_OP_MODE_CHANGE  = 9, /* CMIPAPP Operation mode changed */
  RMNET_SM_CMD_AUTOCONNECT_TIMER  = 10,/* Autoconnect retry timer        */
  RMNET_SM_CMD_DHCP_START_OR_FAIL = 11,/* Either of dhcp start or fail   */
  RMNET_SM_CMD_PROCESS_PHYS_LINK_UP_DOWN_EVT = 12,/* Phys link ev hdlr cb*/
  RMNET_SM_CMD_QCRMCALL        = 13,
  RMNET_SM_CMD_MAX
} rmnet_sm_cmd_e_type;

/*---------------------------------------------------------------------------
  Function pointer used to Tx data when a link protocol other than
  Ethernet is specified.
---------------------------------------------------------------------------*/
typedef void (*rmnet_lp_tx_f_ptr_type)(dsm_item_type**, void*);

/* NTB16 header for DL path only, with hardcoded datagrams */

typedef PACKED struct PACKED_POST
{
  ps_iface_mbim_ntb_nth_hdr                nthc;
  ps_iface_mbim_ntb_ndp_hdr                ndpc;
  ps_iface_mbim_ntb_ndp_ip_datagram_info   ipd[PS_DL_OPT_MBIM_MAX_NDP_ENTRIES];
}rmnet_ntb_hdr_type;

/*--------------------------------------------------------------------------
                            RNDIS Data types
--------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32  msg_type;                /* RNDIS NW msg type = 1 */
  uint32  msg_len;                 /* RNDIS msg len = RNDIS_HDR+PAYLOAD+PAD */
  uint32  data_offset;             /* Payload offset from DataOffset field */
  uint32  data_len;                /* Payload length */
  uint32  oob_data_offset;
  uint32  oob_data_len;
  uint32  num_oob_data_ele;
  uint32  per_pkt_info_offset;
  uint32  per_pkt_info_len;
  uint64  rsvd;                    /*Reserved=0*/
} rmnet_rndis_hdr_type;

typedef PACKED struct PACKED_POST
{
  rmnet_rndis_hdr_type           rndis_hdr;   /* RNDIS hdr */
  llc_frm_packed_mode_zero_type  eth_hdr;     /* Ethernet hdr */
} rmnet_rndis_eth_hdr_type;

/*RNDIS header constants*/

#define RMNET_RNDIS_MSG_TYPE          0x00000001
#define RMNET_RNDIS_RSVD              0x0
#define RMNET_RNDIS_PAYLOAD_OFFSET    36
#define RMNET_RNDIS_DATA_OFFSET       8

/*--------------------------------------------------------------------------
                            Flow Control Mask
--------------------------------------------------------------------------*/
#define RMNET_FC_MASK_SIO         0x01
#define RMNET_FC_MASK_IFACE       0x02
#define RMNET_FC_MASK_FLOW        0x04
#define RMNET_FC_MASK_PHYS_LINK   0x08

/*---------------------------------------------------------------------------
  The Rm state machine's information block
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    Common to all RmNet SMs.
  -------------------------------------------------------------------------*/
  ps_iface_type *            rmnet_iface_ptr;
  ps_iface_type *            um_iface_ptr;      /* Um iface providing data conn*/
  ps_iface_type *            um_iface_ptr_dup;  /* Um iface ptr cached by QMI  */
  ps_iface_event_handle_type um_iface_handle;   /* Um iface handle             */
  rmnet_smi_e_type           sub_sm_type;       /* Sub state machine type      */
#ifdef FEATURE_QXDM_WM_TRACING
  uint32                     call_id;           /* Call Id for FC WM logging   */
#endif /* FEATURE_QXDM_WM_TRACING */

  struct constants_s
  {
    lan_lle_enum_type     lan_llc_instance;
    uint8                 dual_ip_inst;
    rmnet_instance_e_type rmnet_inst;
  } constants;

  rmnet_xport_type      * xport_info;           /* Logical transport info */
  boolean                 has_flow;
  uint32                  subs_id;     // Subs id of the current call
  /*-------------------------------------------------------------------------
    EP and mux ID of rmnet instance which has call already up and caused
    the call on this instance to be torn down.
  -------------------------------------------------------------------------*/
  uint32                  net_down_ep_id;
  uint8                   net_down_mux_id;
  boolean                 net_down_2_rmnet;

  boolean                 dpm_rm_registered;

  /*-------------------------------------------------------------------------
    For flow control
  -------------------------------------------------------------------------*/
  uint32                  rx_fc_mask;

  /*-------------------------------------------------------------------------
    Meta SM specific information
  -------------------------------------------------------------------------*/
  struct meta_sm_s
  {
    rmnet_meta_smi_state_type  state;              /* State machine state */
    timer_type                 autoconnect_timer;  /* autoconnect retry timer */
    uint32                     autoconnect_next_backoff_time;
    boolean                    restore_um_up;      /* 1 if um was up before call */
    boolean                    auto_connect_trigger;

  /*-----------------------------------------------------------------------------
    Only one of these tx functions will be relevent depending upon which link
    protocol is being used.
  -----------------------------------------------------------------------------*/
    rmnet_lp_tx_f_ptr_type lp_tx_f_ptr;      /* Special link protocol f_ptr */
    boolean                llc_started;      /* if llc_start has been called*/
    boolean                dtr_is_high;
    ps_iface_net_down_reason_type call_end_reason; /* Um iface down reason */

    /*-----------------------------------------------------------------------
      Um interface callback buffers
    -----------------------------------------------------------------------*/
    void *um_iface_active_out_of_use_buf_ptr;
                                         /* Event buf for UM Iface OO USE  */
    void *um_iface_up_buf_ptr;           /* Event buf for UM Iface UP      */
    void *um_iface_down_buf_ptr;         /* Event buf for UM Iface DOWN    */
    void *um_flow_enabled_buf_ptr;       /* Event buf for UM Flow ON       */
    void *um_flow_disabled_buf_ptr;      /* Event buf for UM Flow OFF      */
    void *um_phys_link_flow_enabled_buf_ptr; /*  buf: UM Phys link Flow ON */
    void *um_phys_link_flow_disabled_buf_ptr; /* buf: UM Phys link Flow OFF*/
    void *um_flow_tx_enabled_buf_ptr;
    void *um_flow_tx_disabled_buf_ptr;
    void *um_iface_authenticating_buf_ptr;
                                  /* Event buf for UM Iface AUTHENTICATING */
    void *um_iface_lingering_buf_ptr;  /* Event buf for UM Iface lingering */
    void *um_iface_bearer_tech_change_buf_ptr; /* Event buf for Um bearer tech
                                                  change */
    void *um_phys_link_up_buf_ptr;               /*  buf: UM Phys link UP  */
    void *um_phys_link_down_buf_ptr;            /* buf: UM Phys link DOWN  */

    /*-----------------------------------------------------------------------
      Um iface phys link pointer cache
    ------------------------------------------------------------------------*/
    ps_phys_link_type *um_iface_phys_link_ptr;/* cache for um iface physlink */

    struct
    {
      rmnet_iface_binding_complete_cb_f_type  complete_cback;
      void *  user_data;
    } iface_binding;

    struct settings_s
    {
      boolean  auto_connect_enabled; /* Um connection follows cable connect status */
      rmnet_meta_sm_autoconnect_setting_e_type       autoconnect_setting;
      rmnet_meta_sm_autoconnect_roam_setting_e_type  autoconnect_roam_setting;

      ip_addr_type pri_dns_addr;
      ip_addr_type sec_dns_addr;

      ip_addr_type ipv6_pri_dns_addr;
      ip_addr_type ipv6_sec_dns_addr;
    } settings;

    rmnet_meta_sm_network_info_type  net_params;
    boolean                          default_flow_ev_reg;
    boolean                          arb_close_in_progress; /* flag to monitor Iface
                                                               teardown due to arb   */
    rmnet_meta_sm_proc_id_e_type     proc_id; /* proc id for the instance */
    boolean                          softap_mode; /* SoftAP mode for RmNet  */

    /* byte stats for last call made
         rx - packets received from TE
         tx - packets sent to TE */
    uint64  last_call_tx_bytes;
    uint64  last_call_rx_bytes;

    /* NTB specific header for MBIM call
     * Used only for DL path
     */
    dsm_item_type                   *aggr_pkt;
    llc_frm_packed_mode_zero_type   *eth_header;
    boolean                          eth_arped;

    /* RNDIS hdr - used in Eth mode */
    rmnet_rndis_eth_hdr_type        *rndis_eth_hdr;

  } meta_sm;

  /*-------------------------------------------------------------------------
    IPv4 RmSM specific information
  -------------------------------------------------------------------------*/
  struct v4_sm_s
  {
    boolean                  inited;        /* State machine initialized?  */
    rmnet_v4_smi_state_type  state;      /* RmNet IPv4 state machine state */
    timer_type               guard_timer;         /* IPv4 DHCP guard timer */
    void *                   dhcp_handle;   /* handle return on dhcp_start */

    void *um_addr_changed_buf_ptr;      /* Event buf for UM IP addr change */
  } v4_sm;

  /*-------------------------------------------------------------------------
    IPv6 RmSM specific information
  -------------------------------------------------------------------------*/
  struct v6_sm_s
  {
    boolean                      inited;    /* State machine initialized?  */
    rmnet_v6_smi_state_type      state;  /* RmNet IPv6 state machine state */
    timer_type                   guard_timer;     /* IPv6 DHCP guard timer */
    ps_iface_ipfltr_handle_type  um_fltr_handle;       /* Um ICMP6 filters */
    ps_iface_ipfltr_handle_type  rm_fltr_handle;       /* Rm ICMP6 filters */
    boolean                      ll_addr_resol_ev_recd;/* LL Addr resol event recd? */
    dhcp6_sl_server_duid         dhcp6_server_duid;
    void *                       dhcp6_handle; /* handle from dhcp6 server */

    void                         *um_prefix_update_buf_ptr;
  } v6_sm;

} rmnet_smi_info_type;


typedef struct
{
  ps_iface_type         rmnet_iface;        /* Rm iface initiating call    */
  ps_phys_link_type     rmnet_pkt_phys_link;/* physlink for RMNET pkt iface*/
  acl_type              rmnet_sio_acl;      /* sio interface acl           */
  boolean               in_use;
  struct
  {
    lan_lle_enum_type    lan_llc_instance;
  } constants;
} rmnet_smi_iface_info_type;


typedef struct
{
  rmnet_xport_type       xport_info;
  rmnet_smi_info_type *  info_v4;
  rmnet_smi_info_type *  info_v6;
  uint8                  rx_flow_count;

  /*-----------------------------------------------------------------------
    RMNET Legacy path watermark/queue (PS_RM -> PS)
  -----------------------------------------------------------------------*/
  boolean            legacy_flow_enabled;
} rmnet_smi_dual_ip_info_type;

typedef PACKED struct PACKED_POST
{
  uint8           version;
  uint8           flags;
  ps_flow_type *  flow_ptr;
} rmnet_meta_smi_qos_hdr_type;

/*---------------------------------------------------------------------------
  Subset of the CM SS info structure needed by rmnet
---------------------------------------------------------------------------*/
typedef struct
{
  uint64                  changed_fields;
  sys_srv_status_e_type   srv_status;
  sys_roam_status_e_type  roam_status;
} rmnet_meta_sm_cm_ss_info_type;

/*---------------------------------------------------------------------------
  Struct used to pass WWAN RMSM event and info pointer
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 * rmnet_sm_info_ptr;
  int      sm_id;
  uint32   event;
} rmnet_smi_ev_cmd_type;

typedef struct
{
  void *  info_ptr;
  int     cmd_type;
  int     sm_id;
  union
  {
    struct
    {
      uint32  event;
    } event;
    struct
    {
      uint32  ipv4_addr;
      uint8   msg_type;
    } dhcp_msg;
    struct
    {
      void * handle;
    } dhcp_started;
    struct
    {
      uint8  dl_ver;
      uint8  ul_ver;
    } tlp_change;
    struct
    {
      rmnet_meta_sm_cm_ss_info_type  info;
      cm_ss_event_e_type             event;
      sys_modem_as_id_e_type      asubs_id;
    } cm_ss;
    struct
    {
      ps_phys_link_type          *phys_link_ptr;
      ps_iface_event_enum_type   event;
      ps_iface_event_info_u_type event_info;
      void                       *user_data_ptr;
    } phys_link_status;
#if defined (FEATURE_IP_CALL)
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
    struct
    {
      sys_oprt_mode_e_type  new_oprt_mode;
      cmipapp_trans_id_type new_trans_id;
    } cm_op_mode_change;
    struct
    {
      rmnet_instance_e_type instance;
    } cm_op_mode_change_comp;
#endif /* defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900) */
#endif /* defined (FEATURE_IP_CALL) */

    struct
    {
      rmnet_qcrmcall_action_e_type action;
      rmnet_qcrmcall_request_type  request;
      rmnet_qcrmcall_cb_type       callback;
    } qcrmcall;

  } data;
} rmnet_smi_cmd_type;

/*---------------------------------------------------------------------------
  CM info for rmnet interface
---------------------------------------------------------------------------*/
typedef struct
{
  cm_client_id_type                               cm_client_id;
  rmnet_meta_sm_cm_ss_info_type                   ss_info;
  boolean                                         inited;
  sys_roam_status_e_type                          ss_roaming_status;
} rmnet_smi_cm_info_type;

/*---------------------------------------------------------------------------
  Eri roam ind file info for rmnet interface
---------------------------------------------------------------------------*/
typedef struct
{
  uint16      len;
  uint8       data[RMNET_ROAM_IND_FILE_MAX_LEN];
} rmnet_smi_eri_roam_ind_info_type;

#if defined (FEATURE_IP_CALL)
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
/*---------------------------------------------------------------------------
  CMIPAPP info for rmnet interface
---------------------------------------------------------------------------*/
typedef struct
{
  cmipapp_id_type          client_id;
  cmipapp_trans_id_type    cur_trans_id;
  cmipapp_trans_id_type    new_trans_id;
  sys_oprt_mode_e_type     cur_oprt_mode;
  sys_oprt_mode_e_type     new_oprt_mode;
} rmnet_smi_cmipapp_info_type;
#endif /* defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900) */
#endif /* defined (FEATURE_IP_CALL) */

/*---------------------------------------------------------------------------
  NV struct for storing DNS addresses for each instance
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                                        ep_id;
  uint8                                         mux_id;
  uint32                                        dns_addr;
} qmi_nv_dns_addr_type;

/*---------------------------------------------------------------------------
  NV struct for storing IPv6 DNS addresses for each instance
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32                                        ep_id;
  uint8                                         mux_id;
  uint64                                        dns_addr[2];
} qmi_nv_ipv6_dns_addr_type;

extern rmnet_smi_info_type * rmnet_smi_info[RMNET_INSTANCE_MAX];

extern ps_iface_type * lan_llc_inst_to_um_iface_ptr_lut[LAN_LLE_MAX];


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION RMNET_META_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM event cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          : RmNet cmd to be processed
    user_data_ptr: private data buffer containing the RmNet sm
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
);


/*===========================================================================
  FUNCTION RMNET_META_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet Meta SM cmd.

  PARAMETERS
    cmd          : RmNet cmd to be processed
    user_data_ptr: private data buffer containing the RmNet sm
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
);


/*===========================================================================
  FUNCTION RMNET_V4_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM event cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          : RmNet cmd to be processed
    user_data_ptr: private data buffer containing the RmNet sm
                   event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v4_sm_process_ev_cmd
(
  dcc_cmd_enum_type cmd,
  void * user_data_ptr
);


/*===========================================================================
  FUNCTION RMNET_V4_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM cmd.

  PARAMETERS
    cmd          : RmNet cmd to be processed
    user_data_ptr: private data buffer containing the RmNet sm
                   event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v4_sm_process_cmd
(
  dcc_cmd_enum_type cmd,
  void * user_data_ptr
);


/*===========================================================================
  FUNCTION RMNET_V6_SM_PROCESS_EV_CMD()

  DESCRIPTION
    This function processes a RmNet IPv6 SM event cmd.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          : RmNet cmd to be processed
    user_data_ptr: private data buffer containing the RmNet sm
                   event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v6_sm_process_ev_cmd
(
  dcc_cmd_enum_type         cmd,
  void * user_data_ptr
);


/*===========================================================================
  FUNCTION RMNET_V6_SM_PROCESS_CMD()

  DESCRIPTION
    This function processes a RmNet RmSM cmd.

  PARAMETERS
    cmd          : RmNet cmd to be processed
    user_data_ptr: private data buffer containing the RmNet sm
                   event information

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v6_sm_process_cmd
(
  dcc_cmd_enum_type         cmd,
  void * user_data_ptr
);

/*===========================================================================
  FUNCTION RMNET_V4_SM_INIT_CONFIG()

  DESCRIPTION
    This function initializes the IPv4 specific (Rm iface and LLC) parameters.

  PARAMETERS
    start_info: The LLC start information to populate.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v4_sm_init_config
(
  rmnet_smi_info_type     * info,
  lan_llc_start_info_type * start_info
);

/*===========================================================================
  FUNCTION RMNET_V6_SM_INIT_CONFIG()

  DESCRIPTION
    This function initializes the IPv6 specific (Rm iface and LLC) parameters.

  PARAMETERS
    start_info: The LLC start information to populate.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_v6_sm_init_config
(
  rmnet_smi_info_type     * info,
  lan_llc_start_info_type * start_info
);
/*===========================================================================
  FUNCTION RMNET_SMI_GET_CMD_BUF()

  DESCRIPTION
    This function allocates a new RmNet SM command buffer

  PARAMETERS
    cmd_id : the command type i.e. DCC_RMNET_SM_CMD or DCC_RMNET_SM_EV_CMD
    filename: filename of caller.
    line_num: line number of caller.
    grceful: gracefull or graceless handling.

  RETURN VALUE
    cmd_ptr - pointer to the newly allocated command buffer

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void *rmnet_smi_get_cmd_buf
(
  dcc_cmd_enum_type cmd_id,
  const char* filename,
  uint32      line_num,
  boolean     gracefull
);

/*===========================================================================
  FUNCTION RMNET_META_SMI_RX_READY()

  DESCRIPTION


  PARAMETERS


  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_meta_smi_rx_ready
(
  rmnet_smi_info_type    * info
);

/*===========================================================================
  FUNCTION RMNET_META_SMI_PROCESS_QOS_HDR()

  DESCRIPTION


  PARAMETERS


  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void rmnet_meta_smi_process_qos_hdr
(
  void *                user_data,
  dsm_item_type *       pkt_ptr,
  ps_meta_info_type_ex  ** meta_info_ex_ptr_ptr,
  rmnet_meta_smi_qos_hdr_type * qos_hdr
);

#endif /* _DS_RMNET_SMI_H */
