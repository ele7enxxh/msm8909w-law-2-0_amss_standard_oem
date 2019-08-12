#ifndef _DS_RMNET_META_SM_H
#define _DS_RMNET_META_SM_H
/*===========================================================================

                     D S _ R M N E T _ M E T A _ S M . H
DESCRIPTION

  The Data Services WWAN Rm State Machine header file.

  This state machine manages the Rm network interface and coordinates
  wireless WAN connectivity between the Rm and Um interfaces over all
  supported radio technologies.

EXTERNALIZED FUNCTIONS

  rmnet_meta_sm_post_event()
    Post an event to WWAN Rm State Machine.

  rmnet_meta_sm_reg_notify()
    Register a callback to be called on specified RMSM event

  rmnet_meta_sm_in_call()
    Check if RMNET SM is active

  rmnet_meta_sm_iface()
    Return PS iface in use by RMNET sm

  rmnet_meta_sm_set_network_cfg_params()
    This function is called to set the parameters received in Start Network
    Interface for network configuration in the rmnet info structure.

  rmnet_meta_sm_get_um_iface()
     Returns the UM ps_iface ptr for the given RmSM instance

  rmnet_meta_sm_is_auto_connect_enabled()
    Returns whether auto_connect is enabled on this instance or not

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ds_rmnet_meta_sm.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
10/09/12    sj     8 PDN support changes
07/11/12    pgm    Added support for RNDIS agg mode.
06/06/12    sb     Fix to gracefully handle attempts to connect on more than 5 pdn's.
04/12/12    pgm    RmNet legacy path definitions.
03/14/12    wc     Add get_proc_id_from_inst routine
03/01/12    sa     Changes for Set/Get autoconnect and updating efs.
02/22/12    wc     Enable EFS to configure proc_id
02/21/12    wc     Mainline eMBMS
02/17/11    ua     Added interface support for $QCRMCALL.
02/02/12    cy     Added support for MBIM NTB parameters
11/02/11    wc     Add DTR on SIO control channel feature
01/11/11    kk     Added support for data_call_type TLV in data_call_status
                   indication.
02/14/11    ua     Added support for DUAL IP Autoconnect.
06/19/10    vs     Changes for Dual IP support over single QMI instance
10/23/09    ar     Added rmnet_meta_sm_open_instance() prototype
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
04/01/08    mct    Added support for IP Mode.
10/01/07    mct    Rearchitected RmNet to support multiple interface types.
                   Added IPv6 support.
11/29/06    jd     Added DCTM support to RmNet
08/23/06    ks     Changes to support multiple rmnet instances. Added
                   rmnet_meta_sm_is_auto_connect_enabled().
03/13/06    jd     Removed notifications (rmnet_meta_sm_reg_notification)
03/06/06    ks     Added rmnet_meta_sm_get_um_iface().
12/02/05    ks     Added support for Profiles.
10/04/05    ks     Removed feature AUTO_CALL_CONNECT as it is controlled by
                   a NV item now.
09/09/05    jd     Added prototype for rmnet_meta_sm_set_auth_creds
05/31/05    jd     Code review updates
03/14/05    ks     Removed featurization
03/07/04    ks     Initial implementation
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ps_iface.h"

#include "ds_rmnet_defs.h"
#include "ds_rmnet_sm_ext.h"
#include "ds_qmi_svc_ext.h"
#include "ps_iface_mbim_defs.h"
#include "sio.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  regional feature to auto-connect to WAN if prompted
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_META_SM_RM_DEV_START_PKT_EV  = 0,       /* Rm wants packet call         */
  RMNET_META_SM_RM_LINK_DOWN_EV      = 1,       /* Rm interface is down         */
  RMNET_META_SM_UM_IFACE_DOWN_EV     = 2,       /* Um interface is down         */
  RMNET_META_SM_UM_IFACE_UP_EV       = 3,       /* Um interface is up           */
  RMNET_META_SM_ADDR_CFG_COMPLETE_EV = 4,       /* Addr cfg in sub sm done      */
  RMNET_META_SM_ADDR_CFG_FAILURE_EV  = 5,       /* Addr cfg in sub SM failed    */
  RMNET_META_SM_ADDR_CFG_CHANGED_EV  = 6,       /* Um address changed           */
  RMNET_META_SM_UM_IFACE_AUTHENTICATING_EV = 7, /* Um interface authenticating  */
  RMNET_META_SM_UM_IFACE_DOWN_ARB_EV       = 8, /* Um interface down due to arb */
  RMNET_META_SM_UM_IFACE_BEARER_TECH_CHANGED_EV = 9, /* Um interface bearer
                                                          tech changed */
  RMNET_META_SM_EP_CFG_DONE          = 10,
  RMNET_META_SM_MAX_EV
} rmnet_meta_sm_event_type;

/*---------------------------------------------------------------------------
  Enum type indicating the underlying link protocol used by RmNet during the
  call.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_INVALID_MODE  = 0x0,
  RMNET_ETHERNET_MODE = 0x1,
  RMNET_IP_MODE       = 0x2,
  RMNET_ALL_MODE      = 0x3
} rmnet_sm_link_prot_e_type;

/*---------------------------------------------------------------------------
  QoS header format
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_QOS_FORMAT_NONE   = 0,
  RMNET_QOS_FORMAT_6_BYTE = 1,
  RMNET_QOS_FORMAT_8_BYTE = 2
} rmnet_qos_format_e_type;

#define RMNET_META_SM_EVENT_CMD DS_RMNET_META_SM_EV_CMD

typedef enum
{
  RMNET_RT_LK_UP_TYPE_BRINGUP = 0x0,
  RMNET_RT_LK_UP_TYPE_IFACE = 0x1,
  RMNET_RT_LK_UP_TYPE_DATAPATH = 0x2,
  RMNET_RT_LK_UP_TYPE_MAX
} rmnet_meta_sm_route_look_up_e_type;

typedef enum
{
  //0x0 is reserved for local (modem)
  //0x1 is reserved for RmSm
  RMNET_PROC_ID_APPS1   = 0x02,      /* AProc call    */
  RMNET_PROC_ID_LAPTOP1 = 0x03,      /* TE-RmNet call */
  RMNET_PROC_ID_MAX     = 0x7FFFFFFF
} rmnet_meta_sm_proc_id_e_type;

/*---------------------------------------------------------------------------
  enum for RmNet data aggregation
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_DISABLE_DATA_AGG           = 0,
  RMNET_ENABLE_DATA_AGG_TLP        = 1,
  RMNET_ENABLE_DATA_AGG_QC_NCM     = 2,
  RMNET_ENABLE_DATA_AGG_MBIM       = 3,
  RMNET_ENABLE_DATA_AGG_RNDIS      = 4,
  RMNET_ENABLE_DATA_AGG_QMAP       = 5,
  RMNET_ENABLE_DATA_AGG_QMAP_V2    = 6,
  RMNET_ENABLE_DATA_AGG_QMAP_V3    = 7,
  RMNET_DATA_AGG_MAX               = 0x7FFFFFFF
} rmnet_data_agg_enum_type;

/*---------------------------------------------------------------------------
  Data formats
---------------------------------------------------------------------------*/
typedef struct
{
  rmnet_set_data_fmt_e_type   data_format_client;
  rmnet_sm_link_prot_e_type   link_prot;
  uint8                       qos;
  rmnet_qos_format_e_type     qos_format;
  boolean                     mbim_sw_deagg;  // MBIM SW UL de-aggregation
  rmnet_data_agg_enum_type    ul_data_agg_protocol;
  rmnet_data_agg_enum_type    dl_data_agg_protocol;
  uint32                      ndp_signature;
  uint32                      ncm_signature;
  uint32                      dl_data_agg_max_num;
  uint32                      dl_data_agg_max_size;
  uint32                      ul_data_agg_max_num;
  uint32                      ul_data_agg_max_size;
  uint32                      dl_min_padding;
  boolean                     te_flow_control;
} rmnet_data_format_type;

typedef struct
{
  boolean in_band_fc;
} rmnet_qmap_settings_type;

/*---------------------------------------------------------------------------
  Struct defintion to pass into get_um_iface_ptr()
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 iface_priority_mask;
  uint8  is_iface_arb_candidate;
} rmnet_meta_sm_arb_info_type;

/*---------------------------------------------------------------------------
  Struct defintion to pass SNI info to rmnet_meta_sm
  consists of override parameters + tech preference, profile numbers
  rmnet_meta_sm stores this info and uses it to pass relevant info in
  route_get() and bring_up().
---------------------------------------------------------------------------*/
typedef struct
{
  network_params_info_type cfg_info;
  ps_iface_id_type  um_iface_id;
  ps_iface_name_enum_type tech_pref;
  uint8  cdma_profile_num;
  uint16 umts_profile_num;
  uint8  ip_family_pref;
  boolean               is_mcast_iface; /* if this rmnet instance is mcast */
  boolean               is_embms_iface;
  rmnet_meta_sm_call_e_type  call_type;
  uint32                subscription_id;
  uint8 ips_id;
  uint8 apn_type;
  uint16 packet_filter_id;
  struct
  {
    uint8 length;
    char name[255];
  } apn;
#ifdef FEATURE_DATA_LTE
  boolean               is_handoff_context_valid;
  acl_handoff_ctxt_type handoff_context;
#endif /* FEATURE_DATA_LTE */
} rmnet_meta_sm_network_info_type;

/*---------------------------------------------------------------------------
  EFS Config parameters. .
---------------------------------------------------------------------------*/
typedef struct rmnet_meta_sm_config
{
  uint32   instance_id;   /* contains instance ID  */
  uint32   link_prot;     /* Link protocol IP/ETH */
  boolean  qos_enabled;   /* QoS Enabled/Disabled  */
  rmnet_data_agg_enum_type  ul_tlp_enabled;   /* UL data agg format */
  uint32   autoconnect;    /* Autoconfig Enabled/Disabled/Invalid */
  uint32   ip_type;       /* IP Type IPV4/IPV6/IPV4V6 */
  uint32   umts_profile;  /* UMTS Profile */
  uint32   cdma_profile;  /* CDMA Profile  */
  rmnet_data_agg_enum_type  dl_data_agg_protocol; /* DL data agg format */
  rmnet_meta_sm_proc_id_e_type proc_id; /* proc_id */
}rmnet_meta_sm_config_s_type;

/*---------------------------------------------------------------------------
  EFS Global Config parameters.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint32   ul_agg_prot_mask;   /* UL global config  */
  uint32   dl_agg_prot_mask;   /* DL global config */
  boolean  eth_mode_enabled;   /* Ethernet mode */
  uint8    reserved[31];
} rmnet_efs_global_config_s_type;

#define RMNET_GLOBAL_EFS_DATA_FORMAT_DISABLE_MASK    (0x00000000)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_TLP_MASK        (0x00000001)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_QC_NCM_MASK     (0x00000002)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_MBIM_MASK       (0x00000004)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_RNDIS_MASK      (0x00000008)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_MASK       (0x00000010)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_V2_MASK    (0x00000020)
#define RMNET_GLOBAL_EFS_DATA_FORMAT_QMAP_V3_MASK    (0x00000040)

/*! @brief Define MBIM Header NTH16 Sig = 4 bytes 'NCMH' with 'N' in first byte
*/
#define RMNET_MBIM_HDR_NTH16_SIG   0x484D434E

//
/*! @brief Define MBIM Header NDP16 Sig = 4 bytes 'NCMx' with 'N' in first byte
*/
//#define RMNET_MBIM_HDR_NDP16_SIG   0x784D434E


/*! @brief Define QC Header NTH16 Sig = 4 bytes 'QNCM' with 'N' in first byte
*/
#define RMNET_QC_NCM_HDR_NTH16_SIG   0x4D434E51

/*! @brief Define QC Header NDP16 Sig = 4 bytes 'QNDP' with 'N' in first byte
*/
#define RMNET_QC_NCM_HDR_NDP16_SIG   0x50444E51

/*! @brief Define MBIM Header NDP16 Sig = 4 bytes 'IPS0'
*/
#define RMNET_MBIM_HDR_IPS0_NDP16_SIG   0x00535049


/*---------------------------------------------------------------------------
  callback function to be invoked when Rmnet inst - Rmnet iface binding occurs
---------------------------------------------------------------------------*/
typedef void (* rmnet_iface_binding_complete_cb_f_type)
(
  ps_iface_type * rmnet_iface_ptr,
  void *          user_data_ptr
);

typedef struct
{
  ps_iface_mbim_ntb_nth_hdr   nth_hdr;
  ps_iface_mbim_ntb_ndp_hdr * ndp_hdr;
  dsm_item_type             * ntb_dsm_item;   /*MBIM pkt*/

  uint16  ndp_datagram_offset;
  uint16  ndp_offset;
  uint8 * ndp_short_buf;  /*NDP buff with less than 10 IP pkts*/
  uint8 * ndp_long_buf;   /*NDP buff with greater than 10 IP pkts*/
} rmnet_mbim_pkt_info_type;

/*---------------------------------------------------------------------------
  Enum type indicating the auto_connect setting
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_AUTOCONNECT_DISABLED           = 0x00,
  RMNET_AUTOCONNECT_ENABLED            = 0x01,
  RMNET_AUTOCONNECT_PAUSED             = 0x02
} rmnet_meta_sm_autoconnect_setting_e_type;

/*---------------------------------------------------------------------------
  Enum type indicating the auto_connect roaming  setting.
---------------------------------------------------------------------------*/
typedef enum
{
  RMNET_AUTOCONNECT_ROAM_SETTING_ALWAYS        = 0x00,
  RMNET_AUTOCONNECT_ROAM_SETTING_HOME_ONLY     = 0x01,
  RMNET_AUTOCONNECT_ROAM_SETTING_MAX           = 0xFF,
} rmnet_meta_sm_autoconnect_roam_setting_e_type;

/*---------------------------------------------------------------------------
  EFS autoconnect settings
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8                        autoconnect_setting;
  uint8                        roam_setting;
  uint8                        ip_type;
  uint8                        link_prot;
  uint16                       umts_profile;
  uint16                       cdma_profile;
  uint8                        ul_data_format;
  uint8                        dl_data_format;
  uint8                        qos;
  uint8                        mux_id;
  uint32                       ep_id;
  uint8                        subs_id;
  uint8                        reserved[7];
} rmnet_efs_autoconnect_s_type;

typedef enum
{
  RMNET_ADDR_TYPE_IPV4= 0x04,
  RMNET_ADDR_TYPE_IPV6= 0x06,
  RMNET_ADDR_TYPE_IPV4V6  = 0x0A,
} rmnet_meta_sm_ip_addr_setting_e_type;

/*---------------------------------------------------------------------------
  Definitions for $QCRMCALL
---------------------------------------------------------------------------*/
#define RMNET_QCRMCALL_MAX  (8)

typedef enum
{
  QCRMCALL_ACTION_NONE   = 0,
  QCRMCALL_ACTION_START  = 1,
  QCRMCALL_ACTION_STOP   = 2,
  QCRMCALL_ACTION_QUERY  = 3,
} rmnet_qcrmcall_action_e_type;

/* qcrmcall_inst: starts from 0. IP Family: 4 or 6. Status = TRUE: success */
typedef union
{
  // START
  struct
  {
    uint8                             qcrmcall_inst;
    rmnet_meta_sm_network_info_type * sni_params;  
  } start;

  // STOP
  struct
  {
    uint8  qcrmcall_inst;
    uint8  ip_family;
  } stop;

} rmnet_qcrmcall_request_type;

typedef union
{
  // START and STOP
  struct
  {
    uint8      qcrmcall_inst;
    uint8      ip_family;
    boolean    status;
  } sni;

  // QUERY
  struct
  {
    uint8  num_qcrmcall;
    struct
    {
      uint8  qcrmcall_inst;
      uint8  ip_family;
    } qcrmcall[RMNET_QCRMCALL_MAX];
  } list;

} rmnet_qcrmcall_result_type;

typedef void (* rmnet_qcrmcall_cb_type)
(
  rmnet_qcrmcall_action_e_type  action,
  rmnet_qcrmcall_result_type  * result
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*===========================================================================
  FUNCTION RMNET_META_SM_POST_EVENT()

  DESCRIPTION
    This function posts an event to RMNetSM and enqueues the
    corresponding command to be processed in the DCC task.

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
extern void rmnet_meta_sm_post_event
(
  rmnet_instance_e_type       instance,
  rmnet_meta_sm_event_type    event
);

/*===========================================================================
  FUNCTION RMNET_META_SM_IN_CALL()

  DESCRIPTION
    Returns true if we're in a packet call

  PARAMETERS
    instance - the WWAN Rm interface to query

  RETURN VALUE
    TRUE  - Rm interface 'instance' is starting/in/stopping a packet call
    FALSE - Rm interface 'instance' is idle

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean rmnet_meta_sm_in_call
(
  rmnet_instance_e_type  instance
);

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
extern ps_iface_type * rmnet_meta_sm_iface
(
  rmnet_instance_e_type  instance
);

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
extern void rmnet_meta_sm_set_network_cfg_params
(
  rmnet_instance_e_type              instance,
  rmnet_meta_sm_network_info_type *  net_cfg_params
);

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_UM_IFACE()

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
extern ps_iface_type * rmnet_meta_sm_get_um_iface
(
  rmnet_instance_e_type  instance
);

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
);

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
);

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
extern boolean rmnet_meta_sm_is_auto_connect_enabled
(
  rmnet_instance_e_type  instance
);

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
extern boolean rmnet_meta_sm_enable_auto_connect
(
  rmnet_instance_e_type  instance,
  boolean                enable
);

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_AUTOCONNECT_SETTING()

  DESCRIPTION
    set autoconnect setting

  PARAMETERS
    instance - the RmNet instance
    autoconnect_setting  - the autoconnect setting value

  RETURN VALUE
    TRUE  - set autoconnect setting is successful
    FALSE - set autoconnect setting is unsuccessful

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean rmnet_meta_sm_set_autoconnect_setting
(
  rmnet_instance_e_type                         instance,
  rmnet_meta_sm_autoconnect_setting_e_type      autoconnect_setting,
  rmnet_meta_sm_autoconnect_roam_setting_e_type roam_setting
);

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_AUTOCONNECT_SETTING()

  DESCRIPTION
    get autoconnect setting

  PARAMETERS
    instance - the RmNet instance

  RETURN VALUE
    autoconnect_setting  - auto connect setting value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern rmnet_meta_sm_autoconnect_setting_e_type rmnet_meta_sm_get_autoconnect_setting
(
  rmnet_instance_e_type  instance
);

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_AUTOCONNECT_ROAM_SETTING()

  DESCRIPTION
    set autoconnect roam setting

  PARAMETERS
    instance      - the RmNet instance
    roam_setting  - the autoconnect roam value

  RETURN VALUE
    TRUE  - set autoconnect roam setting is successful
    FALSE - set autoconnect roam setting is unsuccessful

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean rmnet_meta_sm_set_autoconnect_roam_setting
(
  rmnet_instance_e_type                          instance,
  rmnet_meta_sm_autoconnect_roam_setting_e_type  roam_setting
);

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
extern rmnet_meta_sm_autoconnect_roam_setting_e_type rmnet_meta_sm_get_autoconnect_roam_setting
(
  rmnet_instance_e_type  instance
);

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
);

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
extern void rmnet_meta_sm_get_prev_byte_stats
(
  rmnet_instance_e_type  instance,
  uint64 * last_call_rx_bytes,
  uint64 * last_call_tx_bytes
);

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
extern void rmnet_meta_sm_get_ipv4_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
);

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
extern int rmnet_meta_sm_set_ipv4_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
);

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
extern void rmnet_meta_sm_get_ipv6_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
);

/*===========================================================================
  FUNCTION RMNET_META_SM_SET_IPV6_MANUAL_DNS_ADDRS()

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
extern int rmnet_meta_sm_set_ipv6_manual_dns_addrs
(
  rmnet_instance_e_type  instance,
  ip_addr_type           *pri_dns_ptr,
  ip_addr_type           *sec_dns_ptr
);

/*===========================================================================
FUNCTION RMNET_GET_NUM_INSTANCES

DESCRIPTION
  This function gets the number of enumerated RmNet instances

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  uint8 - number of RmNet instances

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 rmnet_get_num_instances
(
  void
);

extern ps_iface_type* rmnet_meta_smi_get_um_iface_ptr
(
  rmnet_meta_sm_network_info_type *  net_params,
  rmnet_meta_sm_route_look_up_e_type rt_lk_up_type,
  acl_policy_info_type            *  acl_pol_ptr,
  rmnet_meta_sm_arb_info_type     *  arb_info
);

extern void rmnet_meta_sm_reg_iface_binding_cb
(
  rmnet_instance_e_type inst,
  rmnet_iface_binding_complete_cb_f_type rmnet_iface_binding_complete_cb,
  void *  user_data
);

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
);

/*===========================================================================
  FUNCTION RMNET_META_SM_GET_RM_IFACE_PTR_FROM_INSTANCE

  DESCRIPTION
    This function returns the RM IFACE PTR allocated for a particular call on a
    particular instance.

  PARAMETERS
  instance: Rmnet Instance on which the call was established.

  RETURN VALUE
    Um Iface Pointer.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type* rmnet_meta_sm_get_laptop_rm_iface_ptr
(
  rmnet_instance_e_type instance
);

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
);

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

extern boolean rmnet_meta_sm_is_iface_avail
(
  void
);

/*===========================================================================
FUNCTION RMNET_META_SM_QOS_OR_TE_FC_ENABLED 

DESCRIPTION
  This function returns checks if QOS header or TE flow control is enabled

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
);

/*===========================================================================
FUNCTION RMNET_META_SM_BIND

DESCRIPTION
  This function binds a logical rmnet instance to a physical port

PARAMETERS
  ep_id:   physical port end point id
  mux_id:  logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rmnet_meta_sm_bind
(
  uint32     ep_id,
  uint8      mux_id
);

/*===========================================================================
FUNCTION RMNET_META_SM_UNBIND

DESCRIPTION
  This function unbinds a logical rmnet instance from a physical port

PARAMETERS
  ep_id:   physical port end point id
  mux_id:  logical rmnet mux id

DEPENDENCIES
  None

RETURN VALUE
  TRUE : success
  FALSE: failure

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rmnet_meta_sm_unbind
(
  uint32     ep_id,
  uint8      mux_id
);

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
extern rmnet_instance_e_type rmnet_inst_from_ep_and_mux_id
(
  uint32   ep_id,
  uint8    mux_id,
  boolean  is_default
);

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
extern boolean rmnet_get_net_down_ep_and_mux_id
(
  rmnet_instance_e_type    rmnet_inst,
  uint32                 * ep_id,
  uint8                  * mux_id
);

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
);

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
);

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
);

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
);

/*===========================================================================
  FUNCTION RMNET_SEND_QCRMCALL_REQUEST

  DESCRIPTION
    This function sends a qcrmcall request to RMNET

  PARAMETERS
    action:        qcrmcall reqeust type
    request:       request info
    qcrmcall_cb:   Callback to be invoked for getting result 

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
);

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
);

/*===========================================================================
  FUNCTION rmnet_smi_retrieve_rm_ifaces_from_netparam()

  DESCRIPTION
    This function returns Rm ifaces associated with ips id.

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
  ps_iface_type      ** rm_iface_ptr,
  ps_iface_type      ** rm_iface_ptr2
);

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
);

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
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* _DS_RMNET_META_SM_H */

