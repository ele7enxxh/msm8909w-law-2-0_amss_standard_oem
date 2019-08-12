#ifndef DS707_PKT_MGR_H
#define DS707_PKT_MGR_H
/*===========================================================================

                        D S 7 0 7 _ P K T _ M G R 
GENERAL DESCRIPTION
  This file is the controller for 1X packet calls.  It does not have any
  async/fax functionality.
  
  This module mainly controls the traffic channel portion of the PS_IFACE.
  This module works in conjunction with ds707_rmsm.c and ds707_dssnet.c
  
  This file mainly concerns itself with interacting with CM/MC, in order to
  bring up and tear down the traffic channel, as well as going into and 
  coming out of dormancy.
  
  This module does not know if the packet call is laptop or sockets, or even
  if it is relay or network.

EXTERNALIZED FUNCTIONS
  DS_VERIFY_SO
    Called by MC before making a packet data call.  Required because system
    P_REV can change between the the origination request to CM and the actual
    request going out over the air.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds707_pkt_mgr_init() & ds707_pkt_cm_init() must be called at startup.

Copyright (c) 2002-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.h_v   1.4   27 Nov 2002 11:02:18   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_pkt_mgr.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/23/15    sd     Fixed throttling info updation on query through QMI WDS. 
08/05/14    ms     DSDA Phase 2 changes.
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
06/04/14    sd     Fix to utilize DSM memory event callbacks to enable and
                   disable flow control.
04/09/14    ssb    Featurize DS707 specific apis under FEATURE_DATA_IS707
03/04/14    sd     Changes made to notify DSD to indicate 1x service 
                   availability only after ESPM is received.
06/25/13    sd     Boundary check for 707-mip profile variables.
06/03/13    sd     ATCoP decoupling changes
11/02/12    ssb    Adding utility funtion to increment and get number of 
                   MIP to SIP Fallback attempts
10/17/12     ms    Change rev link ack counter data type to uint32.
10/11/12    sd     Compilation Fixes
09/25/12    dvk    Merged fix to add support to tear down dormant C2K data 
                   call when DDS switch happens.
09/03/12    dvk    Accessor function to set BP paramaters value to the 
                   specified values. 
08/22/12    ss     Added efs init function.
07/17/12    ms     Dynamic back pressure removal support for TCP throughput
                   optimization.
04/20/12    op     Added support to get the dns_v4_addr_req_type NV item
01/19/12    jz     pdn failure fallback
01/16/12    ack    Fix to post a cmd for iface bringup/teardown
01/05/12    msh    Cannot use errno as variable name  
11/16/11    dvk    Merged changes to have pre-dorm and call throttle 
                   structures as PACKED structures. Changed pre-dorm params 
                   order inline with QMI. 
09/21/11    vpk    Provide support for primary physlink to outlive iface
09/08/11   ash     Added support for eHRPD to HRPD fallback
08/20/11    ttv    GSDI to MMGSDI_SESSION api migration changes.
08/15/11    ms     Fixed graceful teardown in LPM
08/11/11    var    Resolve Compiler warnings & KW errors.
06/13/11    dvk    Merged support to get pre-dormant data call info, to set/get 
                   CAM timer value, to enable/disable SCRM/RDUD, to get SIP/MIP 
                   status and to get call throttle info. Also added accessor 
                   function to set SO for pre-dormancy call.
04/11/11    ack    Removing FEATURE_UIM_SUPPORT_HOTSWAP wrap
04/11/11    ttv    Added support for Hot Swap feature.
03/21/11    ack    Init Crit Sect Variable before usage
03/15/11    mg     Global variable cleanup
03/14/11    mga    Merged changes to set the MTU accordingly
03/02/11    op     Added access functions to get/set hdr hybrid status
02/25/11    ms     Support for KDDI special NAI and Password for AN HDR
                   authentication.
11/22/10    sn     Replaced REX premption macros with critical section for 
                   mutual exclusion.
11/03/10    mg     Added ds707 critical section global variable
05/04/10    mg     Dual IP feature
03/26/10    gc     Dont allow CAM to Set CTA timer to 1sec in 1xonly mode
03/04/10    spn    Updated the Data Bearer Indications
09/04/09    ss     CMI SU level modifications.
07/27/09    spn    Removed functions get/set active_hdr_prot.
07/17/09    ls     Change FEATURE_CSIM to FEATURE_MMGSDI_SESSION_LIB
06/04/09    ls     Support CSIM feature
03/09/09    sa     AU level CMI modifications.
03/20/09    spn    changed ds707_pkt_state to ds707_pdn_state
03/20/09    spn    Increased number of PDNs to 3 for eHRPD 
03/12/09  ssh/spn  PPP VSNCP interface changes
02/09/09    spn    Moved RMSM PDN instance to the RMSM Module
10/06/08    sn     Fix to make sure HDR SMP deactivate is not sent when 1x 
                   data call ends.
09/27/08    pp     Metainfo optimizations.
04/02/08    hs     Added function declaration for ds707_pkt_mgr_update_cdsn
10/18/07    sc     Changed ds707_pkt_is_um_iface_up() to take care of 
                   Routable and Lingering Iface.
07/02/07    sc     Added ds707_linger_cmd - fn pointer for Dormant 2 (iface
                                            lingering) state
                         ds707_pkt_mgr_setup_pkt_call_info
02/12/07    az     Support to request VJ compression during PPP negotiation
12/01/06    rsj    Added new structure definition for 
                   ds707_mode_handler_information_type. 
                   This structure is used to store global information
                   can be used for JCDMA JATE requirement.
                   Added ds707_pkt_mgr_set_last_orig_fail_reason and 
                   ds707_pkt_mgr_get_last_orig_fail_reason,
                   ds707_pkt_mgr_clear_last_orig_fail_reason functions to 
                   operate on global mode handler information structure.
09/11/06    as     Added new utility function to determine if Iface is up
05/19/06    ks     Including file ps_svc.h.
03/18/06    spn    Added a function to clear the primary TC to RLP bindings.
02/14/06    sy     Added support for multiple RLP flows in the forward 
                   direction.Needed to extern some function prototypes.
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
12/23/05    ksu    EMPA RSP: two fwd rx queues
12/23/05    ksu    EMPA RSP: two rev watermarks per ip flow
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP
                   resync during a handup from 1x to DO
02/09/05    kvd    Renamed dsidrXXX functions dsrdudXXX.
02/03/05    kvd    Added new function proto-types for IDR module.
11/30/04    kvd    Added new function proto-types for CAM module.
09/10/04    vas    QOS related changes
08/02/04    ak     Added extern for getting ipv6 iface.
07/29/04    kvd    Added externs  definitions for ds707_bring_up_cmd,
                   ds707_tear_down_cmd.
07/01/04    atp    Brought back verify_so() as CP needs it. 
03/19/04    ak     Removed ifdef around get_iface_ptr; added extern for
                   get_ipv4_iface_ptr().
02/19/04    ak     Removed dssnet4_get_sm and dssnet6_get_sm extern's.
11/27/02    atp    Added ds707_pkt_get_curr_tx_wm().
11/18/02    ak     Updated file header commments.
10/29/02    atp    Added support for SDB.
09/24/02    usb    Added user_data to ppp tx and rx function callbacks  
07/31/02    aku    Externalized function ds707_pkt_is_dormant().
07/24/02    ak     Implementation for enabling/disabling holddown timer.
07/24/02    usb    Error handling from PPP configuration
07/16/02    ak     Removed NET_MODEL featurization.  Changed names of
                   exported funcs from 'pkti' -> 'pkt'.
07/15/02    aku    Exported  functions ds707_pkti_tx_um_SN_data() and
                   ds707_pkti_rx_um_SN_data()
05/10/02    ak     Added extern for signal_processing().  Fixed typo.
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#ifdef FEATURE_DATA_IS707
#include "cm.h"
#include "ds707.h"
#include "ds3geventmgr.h"
#include "dsm.h"
#include "dstask_v.h"
#include "ps_phys_link.h"
#include "ps_svc.h"
#include "rex.h"
#include "sys.h"
#include "dsm.h"
#include "queue.h"
#include "dsrlp_v.h"
#include "ps_svc.h"
#include "ps_ppp.h"
#include "ps_iface_defs.h"
#include "ds707_iface4_hdlr.h"
#include "dsgen_iface6_hdlr.h"
#include "ds707_extif.h"
#include "nv.h"        /* Interface to NV services */
#include "ps_acl.h"
#include "dsmip_api.h"
#include "dsmip_v.h"
#include "ds3gmgr.h"
#include "ps_sys_conf.h"
#include "ps_sys.h"
#include "ps_sys_event.h"
#include "ps_sys_ioctl.h"
#ifdef FEATURE_DS_CAM
  #include "dsat707extctab.h"
#endif /* FEATURE_DS_CAM */
#include "ds3gflowmgr.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "ds3gmmgsdiif.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
                            TYPEDEFS
===========================================================================*/
#define ds707_data_session_profile_id     int32

/*-------------------------------------------------------------------------
  Global variable for 707 critical sections.
-------------------------------------------------------------------------*/
extern rex_crit_sect_type ds707_global_crit_sect;
extern rex_crit_sect_type ds707_chs_global_crit_sect;
extern rex_crit_sect_type ds707_scrm_global_crit_sect;

  /*-------------------------------------------------------------------------
  Dial string for Packet/QNC calls. Defaults to "#777". Note that if the
  default string is changed, the length variable should also be updated.
-------------------------------------------------------------------------*/
extern byte ds707_pkt_mgr_orig_str[];

/*-------------------------------------------------------------------------
  Dial string for forcing the pkt call to be CDMA only. By default
  this string is not populated. It should be retrieved from NV
-------------------------------------------------------------------------*/
extern byte ds707_pkt_mgr_force_cdma_dial_str[];
extern byte ds707_pkt_mgr_force_cdma_str_len;

/*-------------------------------------------------------------------------
    Define an enum to hold the last end call reason from CM. Used for JCDMA 
    JATE requirement. 
  -------------------------------------------------------------------------*/
typedef enum
{
  DS707_ORIG_FAIL_REASON_NONE = 1,
  DS707_ORIG_FAIL_CALL_THROTTLE = 2
} ds707_orig_fail_reason_enum_type;

#ifdef FEATURE_MMGSDI_SESSION_LIB
 /*-------------------------------------------------------------------------
    Define an enum to hold corresponding update from MMGSDI interface
  -------------------------------------------------------------------------*/
typedef enum
{
  DS707_MMGSDI_CDMA_NV_UPDATE             = 1,
  DS707_MMGSDI_CDMA_3GPD_3GPDOPM_UPDATE   = 2,
  DS707_MMGSDI_CDMA_3GPD_MIPSP_UPDATE     = 3,
  DS707_MMGSDI_CDMA_PROFILE_UPDATE        = 4,
  DS707_MMGSDI_CDMA_AT_CMD_INIT           = 5
} ds707_pkt_mgr_mmgsdi_update_enum_type;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

 /*-------------------------------------------------------------------------
    Define a global structure that can be used to store mode handler specific
    information to be passed through function calls.
  -------------------------------------------------------------------------*/
typedef struct
{
  ds707_orig_fail_reason_enum_type call_orig_fail_reason;
  ps_iface_net_down_reason_type    call_end_reason;
} ds707_mode_handler_information_type;


#define DS707_PKT_MGR_APN_MAX_VAL_LEN                100

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK

#define DEFAULT_MAX_HARD_FAILURE_COUNT 1
#define DEFAULT_MAX_SOFT_FAILURE_COUNT 3


/*--------------------------------------------------------------------------- 
  NV item for eHRPD to HRPD fallback feature.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  boolean  enabled;                   /* Is eHRPD to HRPD feature enabled. */
  boolean  propose_ehrpd_after_irat;  /* If set HDR proposed eHRPD on L to e*/
  boolean  is_ehrpd_roaming_disabled; /* If set dont propose eHRPD during roam*/
  uint8    max_hard_fallback_count;   /* Max number of hard failure */
  uint8    max_soft_fallback_count;   /* Max number of soft failure */
  uint16   back_to_ehrpd_timer;       /* reserved for future use */ 
} ds707_pkt_mgr_ehrpd_to_hrpd_fb_config_type;

/*--------------------------------------------------------------------------- 
  eHRPD to HRPD fallback feature control block. 
  Holds roaming indication and ppp_failed status.
---------------------------------------------------------------------------*/
typedef struct 
{ 
  boolean  is_roam;           /* Indicates if the device is in Roam network */
  boolean  ppp_failed;        /* Indicates that Max Soft/Hard failure occured */
} ds707_pkt_mgr_ehrpd_to_hrpd_fb_cb_type;
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_EIDLE_SCI
/*-------------------------------------------------------------------------
    Define a structure that can be used to store slot mode option and
    long sleep enable flag
  -------------------------------------------------------------------------*/
typedef struct
{
  uint8 slotted_mode_option;
  boolean long_sleep_enabled;
}ds707_pkt_mgr_evdo_page_monitor_period_type;

/*---------------------------------------------------------------------------
  CB function pointer used to inform the following events.
  IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV
  IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV
  if evt is SUCCESS(0), failure code is NULL
  if evt is FAILURE(-1), failure code is valid
---------------------------------------------------------------------------*/
typedef void (ds707_pkt_mgr_set_evdo_page_monitor_period_cb) 
               (int16 evt, 
                 ps_hdr_slotted_mode_failure_code_enum_type* failure_code); 

/*---------------------------------------------------------------------------
  CB function pointer used to inform the following session change event.
  IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV
  This call back function will be registered during initialization/bootup
---------------------------------------------------------------------------*/
typedef void (ds707_pkt_mgr_evdo_page_monitor_period_changed_hdlr)
                (ds707_pkt_mgr_evdo_page_monitor_period_type epmp);      

#endif /* FEATURE_EIDLE_SCI */

/* Structure that stores pre-dormant data call information. */
typedef PACKED struct PACKED_POST
{
  uint16 pkt_data_so;                 /* SO value of pre-dormant data call */
  uint8  data_sess_ntw;               /* Sys mode of pre-dormant data call */
} ds707_pre_dorm_params;

/*---------------------------------------------------------------------------
  NV item for Dynamic back pressure removal feature
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  /* Back Pressure removal flag */
  uint8            enabled; 

  /* Dynamic BP timer value in milli secs */
  uint32           timer_val;

}ds707_back_pressure_data_s;

typedef PACKED struct PACKED_POST
{
  ds707_back_pressure_data_s      data;

  /* PS timer handle for dynamic back pressure disable */
  ps_timer_handle_type            tmr_hndl_dyn_back_pressure;

  /* Current TCP Rev link Ack counter */
  uint32                         tcp_rev_link_ack_counter;

  /* Last TCP Rev link Ack count */
  uint32                         last_tcp_rev_link_ack_count;

}ds707_back_pressure_params_type;

/*---------------------------------------------------------------------------
  CB function pointer used to inform call throttle status.

  'int16' indicates the error reason. Following errors can be returned.
  DS_ENOERR - Indicates operation succeeded.
  DS_ENOMEM - Indicates memory could not be allocated in CM to get SS info.
---------------------------------------------------------------------------*/
typedef void (call_thr_info_hdlr_cb) (void*, int16);

/*-------------------------------------------------------------------------
  Holds sub command info required for command processing after task switch    
-------------------------------------------------------------------------*/
typedef enum
{
  DS707_CM_NOTIFY_CMD_MIN       = 0,
  DS707_PWROFF_LPM_MODE         = 1,
  DS707_SUB_NOT_AVAIL_NOTIFY    = 2,
  DS707_CM_NOTIFY_CMD_MAX
} ds707_pkt_mgr_cm_notify_cmd_enum_type;

typedef struct
{
  sys_modem_as_id_e_type                subs_id;
  ds707_pkt_mgr_cm_notify_cmd_enum_type cm_notify_type;
} ds707_pkt_mgr_cm_notify_cmd_info_type;

/*-------------------------------------------------------------------------
  Holds sub command info required for  processing  Notification from MCFG 
-------------------------------------------------------------------------*/
typedef struct
{
  struct
  {
    sys_modem_as_id_e_type   subs_id;
    mmgsdi_slot_id_enum_type slot;
  }nv_refresh_info;  
}ds707_pkt_mcfg_notify_cmd_type;


/*-------------------------------------------------------------------------
  Holds MIN APN update information
-------------------------------------------------------------------------*/
typedef struct
{
  char old_apn[DS707_PKT_MGR_APN_MAX_VAL_LEN];
  char new_apn[DS707_PKT_MGR_APN_MAX_VAL_LEN];
  uint8 is_modify;
}ds707_pkt_min_apn_update_cmd_type;

/*-------------------------------------------------------------------------
  Storage for 1x 707 profile values
-------------------------------------------------------------------------*/
typedef struct
{
  uint8     ds707_crm_val;
  uint8     ds707_qcqnc_val;
  uint8     ds707_qcso_val;
} ds707_pkt_mgr_1x_profile;

typedef struct
{
  ds707_pkt_mgr_1x_profile   ds707;
#ifdef FEATURE_DS_MOBILE_IP
  dsmip_1x_profile           mip;
#endif  /* FEATURE_DS_MOBILE_IP */
} ds707_mip_profile_type;

typedef enum
{
  DS707_CRM_VAL         = 0,
  DS707_QCQNC_VAL       = 1,
  DS707_QCSO_VAL        = 2,
  DS707_MDR_VAL         = 3,
  DS707_QCSCRM_VAL      = 4,
  DS707_QCTRTL_VAL      = 5,
  DS707_CTA_VAL         = 6,
  DS707_QCCHS_VAL       = 7,
  DS707_QCQOSPRI_VAL    = 8,
  DS707_CMUX_VAL        = 9,
  DS707_QCDNS_VAL       = 10,
  DS707_PARAMS_MAX_VAL
} ds707_val_enum_type;

typedef struct
{
  ps_iface_type *iface_ptr;
  boolean is_user_data_set;
  union
  {
    ds3g_pkt_call_info_type pkt_call_info;
    network_params_info_type nw_params_info;
  }user_data;

} ds707_bring_up_cmd_info_type;

typedef struct
{
  ps_iface_type *iface_ptr;
  boolean lpm_mode;
  boolean is_user_data_set;
  union
  {
    ps_iface_tear_down_client_data_info_type tear_down_info;
  }user_data;

} ds707_tear_down_cmd_info_type;

typedef struct
{
  ppp_dev_enum_type   ppp_dev;
  ppp_protocol_e_type protocol;
  ppp_event_e_type    ppp_event;                 /* Down/Up/Resync         */
  void               *user_data;
  uint32              session_handle;
  ppp_fail_reason_e_type  fail_reason;
}ds707_ppp_ev_cmd_type;

typedef PACKED struct PACKED_POST
{
  dword hdr_remaining_thr_time;  /* Remaining call throttling time in secs */
  dword cdma_remaining_thr_time; /* Remaining call throttling time in secs */
} ds707_call_throttle_info;

/*---------------------------------------------------------------------------
  Structure passed with DS_CMD_707_ROAM_CHG command.
---------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type  subscription_id;
  sys_sys_mode_e_type     sys_mode;
  sys_roam_status_e_type  roam_status;
}ds707_roaming_ind_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Fix for compilation error in qchat
  TODO: Need to remove in future
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef ds3g_pkt_call_info_type ds707_pkt_call_info_type;

/*===========================================================================
                            VARIABLES
===========================================================================*/
#define DSRLPI_MAX_SRID_INDEX        DSRLP_MAX_RSCB_ELEMENTS

#define DS707_NON_ARRAY              0

#define CRM_VAL_UPPER_BOUND          2
#define CRM_VAL_LOWER_BOUND          0
#define QCQNC_VAL_UPPER_BOUND        1
#define QCQNC_VAL_LOWER_BOUND        0
#define QCSO_VAL_UPPER_BOUND         DS_SO_SET_COUNT - 1
#define QCSO_VAL_LOWER_BOUND         0
#define MDR_VAL_UPPER_BOUND          DS_MDR_MODE_MAX - 1
#define MDR_VAL_LOWER_BOUND          0
#define QCSCRM_VAL_UPPER_BOUND       1
#define QCSCRM_VAL_LOWER_BOUND       0
#define QCTRTL_VAL_UPPER_BOUND       1
#define QCTRTL_VAL_LOWER_BOUND       0
#define CTA_VAL_UPPER_BOUND          255
#define CTA_VAL_LOWER_BOUND          0
#define QCCHS_VAL_UPPER_BOUND        255
#define QCCHS_VAL_LOWER_BOUND        0
#define QCQOSPRI_VAL_UPPER_BOUND     DS707_QOS_DESIRED_PRI_MAX
#define QCQOSPRI_VAL_LOWER_BOUND     0

#define MAX_QCDNS_VAL                2

#ifdef FEATURE_DS_IS707A_CMUX
#define MAX_CMUX_VAL                 2
#else
#define MAX_CMUX_VAL                 0
#endif /* FEATURE_DS_IS707A_CMUX */

/* Constants for MIN conversion routines */
#define DS707_IMSI_STR_MAX_LEN      16 
#define DS707_IMSI_MCC_OFFSET        0
#define DS707_IMSI_MNC_OFFSET        3
#define DS707_IMSI_MIN2_OFFSET       5
#define DS707_IMSI_MIN1_OFFSET       8

/* Number of digits in the A-key */
#define A_KEY_AUTH_DIGITS            sizeof(qword)

/*-------------------------------------------------------------------------
  MTU size for eHRPD and 1x, HRPD
-------------------------------------------------------------------------*/

#define EHRPD_MTU_SIZE 1428

#define HRPD_MTU_SIZE 1500

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION DS707_PKT_MGR_IS_PS_IFACE_ACTIVE()

DESCRIPTION
  This macro returns a boolean indicating whether the iface pointer passed 
  is not in DOWN/DISABLED/INVALID state.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  TRUE: if iface_ptr is not in DOWN/DISABLED/INVALID state
  FALSE: otherwise.
===========================================================================*/
#define DS707_PKT_MGR_IS_PS_IFACE_ACTIVE(iface_ptr)                        \
  ((IFACE_DOWN != ps_iface_state(iface_ptr))                   \
   && (IFACE_STATE_INVALID != ps_iface_state(iface_ptr))       \
   && (IFACE_DISABLED != ps_iface_state(iface_ptr)))

/*===========================================================================
FUNCTION: DS707_PKT_MGR_GET_BEARER_TECH_INFO

DESCRIPTION:
  Send bearer techonology in the struct passed.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean ds707_pkt_mgr_get_bearer_tech_info
(
  ps_iface_bearer_technology_type * bearer_tech
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_GET_HDR_SESS_INFO

DESCRIPTION   Util function to return HDR RAT and SO Mask based on lower 
              layer query info 

DEPENDENCIES  None

RETURN VALUE  TRUE  - On Success 
              FALSE - Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_mgr_get_hdr_sess_info(uint32* rat_mask, uint32* so_mask);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION: DS707_PKT_MGR_GET_BEARER_TECH_INFO_EX

DESCRIPTION:
  Send bearer techonology in the struct passed.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean ds707_pkt_mgr_get_bearer_tech_info_ex
(
  ds_sys_system_status_info_type * bearer_tech
);
#endif

/*===========================================================================
FUNCTION      DS707_PKT_MGR_HDLR_UPDATE_CDSN

DESCRIPTION   Called everytime the CDSN is updated.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_update_cdsn
(
  sys_sys_mode_e_type current_data_sess_network
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_PREV_CALL_SO

DESCRIPTION   This function will return SO of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
word ds707_pkt_get_prev_call_so 
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_SET_PREV_CALL_SO

DESCRIPTION   This function will set value of SO of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_set_prev_call_so 
(
  uint16 prev_call_so
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_PREV_SR_ID

DESCRIPTION   This function will return SR ID of previous call.

DEPENDENCIES  None.

RETURN VALUE  Byte

SIDE EFFECTS  None.
===========================================================================*/
byte ds707_pkt_get_prev_sr_id 
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_SET_PREV_SR_ID

DESCRIPTION   This function sets SR ID of previous call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_set_prev_sr_id 
(
  byte prev_sr_id
);

/*===========================================================================
FUNCTION      DS_VERIFY_SO

DESCRIPTION   Called by MC.  Asks DS if the originating SO is correct.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
word ds_verify_so
(
  word so_to_verify           /* SO to verify                              */
);

/*===========================================================================
FUNCTION      DS707_PKT_SET_PRI_TC_RLP

DESCRIPTION   This function sets the RLP of the Primary TC

DEPENDENCIES  None.

PARAMETERS    The RLP Id to set the primary TC to

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_set_pri_tc_rlp
(
  uint8 rlp_flow,
  dsrlp_ver_enum_type rlp_ver
);

/*===========================================================================
FUNCTION      DS707_PKT_CLEAR_PRI_TC_RLP

DESCRIPTION   This function clears the RLP of the Primary TC

DEPENDENCIES  None.

PARAMETERS    None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_clear_pri_tc_rlp
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_SETUP_RLP

DESCRIPTION   Sets up RLP with a TX watermark and RLP RX func and RLP RX
              queue.  If the inputs are NULL, then use defaults as
              defined by this module.  Else use the ones passed into this
              function.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_setup_rlp
(
  dsm_watermark_type  tx_wm_list[],             /* Tx wms ptr for txmit frs*/
  void               (*post_rx_func_ptr)(uint32),/* rx cback for rx frames */
  dsm_watermark_type  post_rx_wm_list[]          /* queue for rx'ed frames */
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_OUTAGE_NOTIFICATION

DESCRIPTION   Passes on the outage notification indication received from
              lower layer to PS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_outage_notification
(  
  uint16 time_to_outage,
  uint16 duration_of_outage
);

/*===========================================================================
FUNCTION      DS707_PKT_BEARER_TECH_CHANGED_NOTIFICATION

DESCRIPTION   Gives a notification to APPS (through PS) of the bearer tech 
              changed event

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_bearer_tech_changed_notification
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_BEARER_TECH_CHANGED_NOTIFICATION

DESCRIPTION   Gives a notification to APPS (through PS) of the bearer tech 
              changed event

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_bearer_tech_reset
(
  void
);


/*===========================================================================
FUNCTION      DS707_PKT_TRASH_PRI_RLP_TX_WMKS

DESCRIPTION   This function is registered with PS_IFACE.  When user calls
              this (via PS_IFACE), this will go ahead an empty the RLP
              TX watermark of all items (i.e, rev link).  By "empty" we
              mean that all data will be trashed.

              This trashes the tx watermarks of all RLP instances.

              THIS EXECUTES IN CALLER's CONTEXT.  May not be in DS TASK.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_trash_pri_rlp_tx_wmks
(
  ps_iface_type    *iface_ptr
);

/*===========================================================================
FUNCTION      DS707_CONFIGURE_VJ_COMPRESSION_FOR_PPP

DESCRIPTION   Called to request VJ compression during PPP negotiation

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_configure_VJ_compression_for_PPP
(

  /* pointer to PPP config info structure. Used during PPP setup */
  ppp_dev_opts_type   * ppp_config_info

);

/*===========================================================================
FUNCTION      DS707_CONFIGURE_JCDMA_PARAMS_FOR_PPP

DESCRIPTION   Called to request JCDMA phone parameters to be configured prior 
              to PPP negotiation.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_configure_jcdma_params_for_ppp
(
  ppp_dev_opts_type   * ppp_config_info /* pointer to PPP config info 
                                           structure. Used during PPP setup*/
);

/*===========================================================================
FUNCTION      DS707_CONFIGURE_JCDMA_SPECIAL_AN_AUTH_PARAMS

DESCRIPTION   Called to fill in JCDMA auth info for HDR AN PPP authentication.

DEPENDENCIES  None

RETURN VALUE  Boolean - TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_configure_jcdma_special_an_auth_params
(
  ppp_auth_info_type   *ppp_auth_info /* pointer to PPP auth info structure 
                                         used during PPP AN auth phase */
);

/*===========================================================================
FUNCTION      DS707_PKT_SETUP_SN_PPP

DESCRIPTION   Called when IFACE is coming up, to intialize PPP for the
              SN PPP.  This the interface used for 1X.

DEPENDENCIES  None

RETURN VALUE  TRUE for successful PPP config, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
int ds707_pkt_setup_SN_ppp_for_rmsm
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_PHYS_LINK_PTR

DESCRIPTION   Given a packet state instance, returns the pointer to the
              phys link.

DEPENDENCIES  None.

RETURN VALUE  pointer to a pkt state

SIDE EFFECTS  None.
===========================================================================*/
ps_phys_link_type* ds707_pkt_get_pri_phys_link_ptr
(
  void
);

/*===========================================================================
FUNCTION      DS707_GET_PHYS_LINK_PTR

DESCRIPTION   Given the pkt instance, and the index to the tc_state, 
              returns the corresponding phys link_ptr. This function
              retrieves secondary phys links.

DEPENDENCIES  None.

RETURN VALUE  Pointer to phys link.

SIDE EFFECTS  None.
===========================================================================*/
ps_phys_link_type* ds707_get_phys_link_ptr
(
  uint32 i
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_IFACE_PTR_FROM_PS_FLOW

DESCRIPTION   Given a flow pointer, returns the pointer to the
              corresponding iface.

DEPENDENCIES  None.

RETURN VALUE  pointer to the IFACE ptr - whether v4 or v6.

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type * ds707_pkt_get_iface_ptr_from_ps_flow
(
  ps_flow_type * ps_flow
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_NV_REFRESH

DESCRIPTION   This function is called when NV Refresh event 
              occurs during sim swap. Init and re-read all the
              NV & EFS items from nv_refresh_list 

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_nv_refresh
(
  sys_modem_as_id_e_type
);

/*==================================================================================
FUNCTION       DS707_CURR_SUBS_ID

DESCRIPTION    Return the current subs id in a global variable
 
DEPENDENCIES   None.

RETURN VALUE   Return the current subs id 
              (sys_modem_as_id_e_type) 

SIDE EFFECTS   None

=====================================================================================*/
sys_modem_as_id_e_type ds707_curr_subs_id();

/*===========================================================================
FUNCTION      DS707_PKT_MGR_DS3G_EVENT_CB

DESCRIPTION   Callback function that gets invoked on event 
              registeration 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_ds3g_event_cb
(
   ds3geventmgr_event_type        event_id,
   ds3geventmgr_filter_type       *filter_info_ptr,
   void                           *event_info_ptr,
   void                           *data_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_SET_IFACE_PTR

DESCRIPTION   Creates a mapping between PS_FLOW and PS_IFACE. Using a flow 
              ptr, one can access the corresponding iface through
              ds707_pkt_get_iface_ptr_from_ps_flow()

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_set_iface_ptr
(
  ps_flow_type    * ps_flow,
  ps_iface_type   * ps_iface_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_IS_DORMANT

DESCRIPTION   Returns TRUE if iface is dormant.  Else returns FALSE.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_dormant
(
  ps_phys_link_type    *phys_link_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_IS_UM_IFACE_UP

DESCRIPTION   Returns TRUE if iface is UP, ROUTEABLE, LINGERING.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_um_iface_up
(
  ps_iface_type    *ps_iface_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_ENABLE_HOLDDOWN_TIMER

DESCRIPTION   Sockets call this function to enable or disable dormancy timer

DEPENDENCIES  If multiple calls of this function are made, principle used
              is last caller wins.

              Input TRUE  - use the holddown timer when going dormant.
                    FALSE - do not use the holddown timer when dormant.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_enable_holddown_timer
(
  boolean is_enabled
);

/*===========================================================================
FUNCTION      DS707_PKT_IS_PZID_ORIG_ALLOWED

DESCRIPTION   Returns TRUE or FALSE, indicating if an orig is allowed on the
              specified IFACE.
              
              Done solely based on holddown timer.

DEPENDENCIES  None.

RETURN VALUE  TRUE - can originate.  FALSE - cannot originate.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_pzid_orig_allowed
(
  uint32 pkt_instance
);

/*===========================================================================
FUNCTION      DS707_PKTI_REORIG

DESCRIPTION   Called when the RLP TX watermark goes non-empty when
              dormant.  Sets a signal so that the DS task goes ahead
              and processes this function.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkti_reorig
(
  struct dsm_watermark_type_s *wm_ptr,
  void                        *user_data_ptr
);

/*===========================================================================
FUNCTION      DS707_RX_UM_SN_SIG_HDLR

DESCRIPTION   Called by PPP to get forward-link data from RLP.  Specific to
              HDR SN/1x-Data.

DEPENDENCIES  None.

RETURN VALUE  TRUE : Clears the signal.
             
SIDE EFFECTS  None.
===========================================================================*/
extern boolean ds707_rx_UM_SN_sig_hdlr
(
  ps_sig_enum_type sig,
  void *dummy
);

/*===========================================================================
FUNCTION      DS707_PKT_TX_UM_SN_IP_data

DESCRIPTION   Called by PhysLink to transmit rev-link data over RLP to the 
              base station.  This is for the SN link/1X-data.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_tx_data
(
  ps_phys_link_type * phys_link_ptr,
  dsm_item_type     **dsm_item_ptr_ptr,
  ps_meta_info_type * meta_info_ptr,
  void              * tx_cmd_info
);

/*===========================================================================
FUNCTION      DS707_PKT_TX_UM_SN_DATA

DESCRIPTION   Called by PPP to transmit rev-link data over RLP to the base
              station.  This is for the SN link/1X-data.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_tx_um_SN_data
(
  dsm_item_type   **dsm_item_ptr_ptr,
  void             *meta_info_ptr,
  void *user_data
);

/*===========================================================================
FUNCTION      DS707_PKT_GET_PKT_DATA_SESS_STATE

DESCRIPTION   This function returns whether there is a packet data session
              and whether it is active or dormant on HDR or CDMA.
              
              NOTE: This should be registered as a callback with CM and not
              be called directly by anybody.

DEPENDENCIES  If changing this function, please go through all the places
              this function is referenced and make sure that the desired
              meaning of active and dormant in all those places is 
              consistent with what this function provides.

RETURN VALUE  Returns whether the data session is active/dormant on CDMA/HDR
              or no data session

SIDE EFFECTS  None.
===========================================================================*/
ds707_pkt_data_sess_state_type ds707_get_pkt_data_sess_state
( 
  void 
);

/*===========================================================================
FUNCTION       DS707_PKT_GET_CURRENT_DATA_SESS_NETWORK

DESCRIPTION    Returns the current network that the call is on

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
sys_sys_mode_e_type ds707_pkt_get_current_data_sess_network
(
  void
);

/*===========================================================================
FUNCTION       DS707_PKT_SET_CURRENT_DATA_SESS_NETWORK

DESCRIPTION    Sets the current network that the call is on

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_set_current_data_sess_network
(
  sys_sys_mode_e_type curr_data_session_network
);

/*===========================================================================
FUNCTION       DS707_PKT_GET_LAST_DATA_SESS_NETWORK

DESCRIPTION    Returns the last network that the data session was on. If we 
               currently have a dormant or active data session, this will be 
               CDMA, HDR or NO_SRV depending on where the data session was 
               before moving to this system. If we do not currently have a 
               packet data session, this will be NO_SRV.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
sys_sys_mode_e_type ds707_pkt_get_last_data_sess_network
(
  void
);

/*===========================================================================
FUNCTION       DS707_PKT_SET_LAST_DATA_SESS_NETWORK

DESCRIPTION    Sets the last network that the data session was on. This will 
               be set to CDMA, HDR or NO_SRV depending on where the data 
               session was on.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_set_last_data_sess_network
(
  sys_sys_mode_e_type data_sess_network
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_INIT_CRIT_SECT

DESCRIPTION   Called once at mobile power-up.  Initializes ds707 global crit 
              section variables
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_init_crit_sect
(
  void
);

/*===========================================================================
FUNCTION      DS707_JCDMA_MODE_GONULLTMR_INIT

DESCRIPTION   Called once at mobile power-up to init JCDMA mode and go null 
              timer. 
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_mode_gonulltmr_init
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_POWERUP_INIT

DESCRIPTION   Called once at mobile power-up to init necessary data 
              structures 
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_powerup_init
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_INIT

DESCRIPTION   Called once at dstask init.  Registers the appropriate 
              functionality with 3G DSMGR.
 
DEPENDENCIES  None.

RETURN VALUE  Signals that DS should look for.

SIDE EFFECTS  None
===========================================================================*/
rex_sigs_type ds707_pkt_mgr_init
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_CM_INIT

DESCRIPTION   Called once at startup.  Registers with CM for things specific 
              to IS-707.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_cm_init
(
  cm_client_id_type        cm_id
);

/*===========================================================================
FUNCTION      DS707_PKT_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to this 
              mode handler.
              
              There is no featurization of commands here.  Functionality
              should be FEATURE'ized elsewhere, so that these commands are 
              generated appopriately.  For example, the SID module only
              generates its command if the SID FEATURE's is defined.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  Can result in various actions, such as call bring up/tear
              down, etc.
===========================================================================*/
void ds707_pkt_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_PROCESS_SIGNALS

DESCRIPTION   Processes sigals set by other parts of the system.  Currently,
              no signals should be set for this mode handler.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_process_signals
(
  rex_sigs_type   sigs
);

/*===========================================================================
FUNCTION DS707_SOCKET_BRING_UP_CMD()

DESCRIPTION
  This function is used to bring up an iface by kickstarting the DSSNET
  engine

PARAMETERS
  ds_cmd_ptr: Cmd buffer passed with the cmd 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e715 unused variables*/
void ds707_socket_bring_up_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION DS707_IFACE_BRING_UP_CMD()

DESCRIPTION
  This function is used to bring up an iface.  Will call the registered
  callback if the interface is in the down state.  If it is in the up state,
  it returns success.  If the interface is in the coming state or down it
  return DS_EWOULDBLOCK.  Finally if it is in the going down state, it
  returns DS_ECLOSEINPROGRESS.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ds707_bring_up_cmd
(
  ps_iface_type *this_iface_ptr,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION DS707_SOCKET_TEAR_DOWN_CMD()

DESCRIPTION
  This function is used to stop an iface.

PARAMETERS
  ds_cmd_ptr: Cmd buffer passed with the cmd 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e715 unused variables*/
void ds707_socket_tear_down_cmd
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION DS707_TEAR_DOWN_CMD()

DESCRIPTION
  This function is used to stop an iface.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ds707_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  void          *client_data_ptr
);

/*===========================================================================
FUNCTION DS707_TEAR_DOWN_CMD_PROCESSOR()

DESCRIPTION
  This function is used to stop an iface. The third parameter is used to 
  determine whether Low Power mode is enabled. If it is enabled we
  need to check whether the tear_down is because of inactivity or normal
  teardown and take action accordingly. This is done by calling the function
  ds707_rmsm_graceful_ppp_tear_down_cmd(). In all other cases the normal
  ds707_rmsm_pkt_tear_down_cmd() is called.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  client_data_ptr: data, that is passed to the client handler function.
  is_lpm_mode: indicator of whether lpm mode is being used.   
 
RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ds707_tear_down_cmd_processor
(
  ps_iface_type *this_iface_ptr,
  void          *client_data_ptr,
  boolean       is_lpm_mode
);

/*===========================================================================
FUNCTION DS707_LINGER_CMD()

DESCRIPTION
  This function is used to make an iface go in LINGERING.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ds707_linger_cmd
(
  ps_iface_type *this_iface_ptr,
  void          *client_data_ptr
);

#ifdef FEATURE_HDR
#ifdef FEATURE_DS_CAM

/*===========================================================================
FUNCTION       DS707_PKT_POST_COLOC_INFO_CHANGED_CMD

DESCRIPTION    Posts new colocation info along with COLOC_INFO_CHANGED_CMD
               to DS. DS  updated its local copy of colocation info on
               receiving this command.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/

void ds707_pkt_post_coloc_info_changed_cmd
(
  boolean evdo_colocated
);

/*===========================================================================
FUNCTION DS707_PKT_SET_CAM_CTA_TIMEOUT

DESCRIPTION    This function sets cam timeout ( RLP idle timeout, after which
               TC is brought down ) to the value specified by the CAM module.
               Current value of cta_timeout is saved so that its restored
               after TC goes down. Note that this value is specified in sec.

DEPENDENCIES   None

RETURN VALUE   None.

SIDE EFFECTS   Rlp idle timeout is now cam_cta_timout.

===========================================================================*/

void ds707_pkt_set_cam_cta_timeout
(
  dword         duration                 /* time interval, in msec        */
);

/*===========================================================================
FUNCTION DS707_PKT_RESTORE_CAM_CTA_TIMEOUT

DESCRIPTION    This function restores cta timeout to what it was before CAM
               module set it to CAM cta timeout.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Rlp idle timeout is now what is set by atcop.

===========================================================================*/

void ds707_pkt_restore_cam_cta_timeout
(
  void
);

#endif /* FEATURE_DS_CAM */
#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION DS707_GET_CURR_MODE_PREF

DESCRIPTION    This function returns the current phone's preferred mode.

DEPENDENCIES   None.

RETURN VALUE    current phone's preferred mode..

SIDE EFFECTS   None

===========================================================================*/
nv_mode_enum_type ds707_get_curr_mode_pref
(
void
);

#ifdef FEATURE_DS_RDUD
/*===========================================================================
FUNCTION DS707_PKT_SET_RDUD_CTA_TIMEOUT

DESCRIPTION    This function sets rdud timeout (RLP idle timeout, after which
               TC is brought down) to the value specified by the RDUD module.
               Current value of cta_timeout is saved so that it is restored
               after TC goes down. Note that this value is specified in sec.

DEPENDENCIES   None

RETURN VALUE   None.

SIDE EFFECTS   Rlp idle timeout is now rdud_cta_timeout.

===========================================================================*/

void ds707_pkt_set_rdud_cta_timeout
(
  dword         duration                 /* time interval, in sec          */
);

/*===========================================================================
FUNCTION DS707_PKT_RESTORE_RDUD_CTA_TIMEOUT

DESCRIPTION    This function restores cta timeout to what it was before RDUD
               module set it to RDUD cta timeout.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Rlp idle timeout is now what is set by atcop.

===========================================================================*/

void ds707_pkt_restore_rdud_cta_timeout
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_SET_RDUD_CTA_DEFAULT_VALUE

DESCRIPTION    This function changes the default value of cta timer. (Called 
               from ioctl to change default value of cta timer). 

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Default value of CTA timer is changed.

===========================================================================*/

void ds707_pkt_set_rdud_cta_default_value
(
  dword         duration                 /* time interval, in sec          */
);
#endif /* FEATURE_DS_RDUD */

/*===========================================================================
FUNCTION      DS707_PKT_ACQUIRE_PPP_RESYNC_LOCK

DESCRIPTION   This function tries to acquire the PPP resync lock for all 
              the ifaces that are up (IP_V4 only, IP_V6 only or both)
              
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_acquire_ppp_resync_lock
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_CHECK_PPP_RESYNC_LOCK

DESCRIPTION   This function checks the PPP resync lock for any 
              the ifaces that are up (IP_V4 only, IP_V6 only or both). If 
              any of the  ifaces have acquired the PPP resync lock, it
              registers the lock release callback and returns the status
              
DEPENDENCIES  None.

PARAMETERS    pointer to call back function to be called upon lock release

RETURN VALUE  TRUE if the PPP resync lock has been acquired, FALSE otherwise

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_check_ppp_resync_lock
(
  void (* ppp_resync_lock_release_cb)(void)
);

/*===========================================================================
FUNCTION      DS707_PKT_RELEASE_PPP_RESYNC_LOCK

DESCRIPTION   This function releases the PPP resync lock for iface 
              corresponding to the dssnet instance pointer passed as a
              parameter. This is registered with the DSSNET instances
              to be notified upon completion of PPP resync (either
              successful or failure scenario)
              
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_release_ppp_resync_lock
(
  void *dssnet_instance
);

/*===========================================================================
FUNCTION      DS707_PKTI_UM_RLP_FWD_FUNC

DESCRIPTION   Called (via function pointer) by RLP when data is received on
              the reverse link.  Used by both SN and AN RLP sessions.  This
              will signal PS task that there is reverse link RLP/PPP data
              to process.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
extern void ds707_pkti_um_rlp_fwd_func
(
  uint32 unused
);

/*===========================================================================
FUNCTION      DS707_PKT_IS_HANDOFF_FROM_1X_TO_HDR

DESCRIPTION   
              
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
extern boolean ds707_pkt_is_handoff_from_1x_to_hdr
(
  sys_sys_mode_e_type old_nw, 
  sys_sys_mode_e_type new_nw
);

/*===========================================================================
FUNCTION      DS707_PKT_IS_HANDOFF_FROM_HDR_TO_1X

DESCRIPTION   
              
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
extern boolean ds707_pkt_is_handoff_from_hdr_to_1x
(
  sys_sys_mode_e_type old_nw, 
  sys_sys_mode_e_type new_nw
);

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_CURRENT_HYBRID_PREF

DESCRIPTION    This function returns the current hybrid pref for AT.

DEPENDENCIES   None.

RETURN VALUE   TRUE - If AT is in Hybrid ON mode
               FALSE - If AT is in Hybrid OFF mode

SIDE EFFECTS   None

===========================================================================*/
boolean ds707_pkt_mgr_get_current_hybrid_pref
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_CURRENT_HYBRID_PREF

DESCRIPTION    This function sets the hybrid pref in cm_ph_event_info.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_current_hybrid_pref
(
  boolean
);
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SETUP_PKT_CALL_INFO

DESCRIPTION   This function sets up pkt_call_info_type structure to populate
              information needed to originate or end calls. Currently this 
              function is used only by sockets calls.

DEPENDENCIES  None.

RETURN VALUE  Void* - ptr to the global pkt_call_info_type structure.
                    - NULL in case of error

SIDE EFFECTS  None
===========================================================================*/
void* ds707_pkt_mgr_setup_pkt_call_info
(
  ps_iface_type *iface_ptr
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_LAST_ORIG_FAIL_REASON

DESCRIPTION    This function returns the reason the last data call
               failed during origination (if applicable).

DEPENDENCIES   None.

RETURN VALUE   A value defined in ds707_orig_fail_reason_enum_type indicating
               the reason the call was ended.

SIDE EFFECTS   None

===========================================================================*/

ds707_orig_fail_reason_enum_type ds707_pkt_mgr_get_last_orig_fail_reason
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_DSSNET4_SM_CB

DESCRIPTION    Returns the DSSNET4 SM for a given IFACE

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
dssnet4_sm_cb_type* ds707_pkt_mgr_get_dssnet4_sm_cb
(
  ps_iface_type* ps_iface_ptr
);

/*===========================================================================
FUNCTION      DS707_GET_CALL_THROTTLE_INFO

DESCRIPTION
  This function gets the call throttle info for both 1x and HDR from CM and 
  returns them in the callback function registered by the caller.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_get_call_throttle_info
(
  call_thr_info_hdlr_cb *call_thr_info_hdlr_func_ptr,
  int16                 *p_errno
);

/*===========================================================================

FUNCTION       DS707_PKT_MGR_IS_ALLOW_FALLBACK_TO_SIP

DESCRIPTION    This function is used to judge whether fallback to sip when 
               mip failure happens 
        
DEPENDENCIES   

RETURN VALUE    TRUE: can fallback 
                FALSE: cannot fallback 

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_pkt_mgr_is_allow_fallback_to_sip
(
  mip_fail_reason_e_type  mip_status,
  byte                    mip_rrp_code
);




/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_LAST_ORIG_FAIL_REASON

DESCRIPTION    This function sets the reason the last data call
               failed during origination (if applicable).

DEPENDENCIES   None.

RETURN VALUE   0 upon sucess, -1 on failure

SIDE EFFECTS   None

===========================================================================*/

int ds707_pkt_mgr_set_last_orig_fail_reason
(
  /* The reason the call has ended from CM*/
  cm_call_end_e_type end_reason
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_GET_PRI_PHYS_LINK

DESCRIPTION
  This function returns the primary physical link. Increments the ref count 
  of physical link and ds707_pkt_mgr_release_pri_phys_link to be called 
  when phys link usage is over. 

PARAMETERS    None

DEPENDENCIES  Physical link needs to be up before calling this.

RETURN VALUE  Primary physical link.

SIDE EFFECTS  None
===========================================================================*/
ps_phys_link_type *ds707_pkt_mgr_get_pri_phys_link
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_RELEASE_PRI_PHYS_LINK

DESCRIPTION
  This function decrements the primary physical link's ref count and calls
go null on physical link.

PARAMETERS    Primary physlink which was returned by 
              ds707_pkt_mgr_get_pri_phys_link

DEPENDENCIES  Assumes caller has called ds707_pkt_mgr_get_pri_phys_link().

RETURN VALUE  Returns the error code about the success/ failure of 
              physical link go null.

SIDE EFFECTS If not freed then phys link will continue to be up.
===========================================================================*/
int16 ds707_pkt_mgr_release_pri_phys_link
(
  ps_phys_link_type *ps_phys_link_ptr
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_LAST_CALL_END_REASON

DESCRIPTION    This function returns the reason the last data call
               failed during origination (if applicable).

DEPENDENCIES   None.

RETURN VALUE   A value defined in ps_iface_net_down_reason_type indicating
               the reason the call was ended.

SIDE EFFECTS   None

===========================================================================*/

ps_iface_net_down_reason_type ds707_pkt_mgr_get_last_call_end_reason
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_LAST_CALL_END_REASON

DESCRIPTION    This function sets the reason the last data call
               failed during origination (if applicable).

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_last_call_end_reason
(
  /* The reason the call has ended from CM*/
  cm_call_end_e_type end_reason,
  /* The call cmd error code from CM */
  cm_call_cmd_err_e_type   call_cmd_err
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_LAST_CALL_END_REASON_FROM_CALL_CMD_ERR

DESCRIPTION    This function sets the reason the last data call
               end reason based on cm call cmd err info

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
ps_iface_net_down_reason_type 
ds707_pkt_mgr_set_last_call_end_reason_from_call_cmd_err
(
  /* The call cmd error code from CM */
  cm_call_cmd_err_e_type   call_cmd_err
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_CURRENT_SYS_MODE

DESCRIPTION    This function sets the current mode.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_current_sys_mode
(
  sys_sys_mode_e_type sys_mode
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_CURRENT_SYS_MODE

DESCRIPTION    This function returns the current mode.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
sys_sys_mode_e_type ds707_pkt_mgr_get_current_sys_mode
(
  void
);


/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_1X_PROFILE_VAL

DESCRIPTION
  This function will return value for the requested val.
  The first parameter is enum type for the val, second parameter is
  the value pointer and the third parameter specifies the number of
  elements to be returned if the val is of array type and it should 
  be 0 if the val is of non-array type.
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE : Value of the corresponding val from static profile was 
         returned successfully.
  FALSE: Value of the corresponding val from static profile was 
         not returned successfully.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_mgr_get_1x_profile_val
(
  ds707_val_enum_type        type,
  void                      *val,
  uint8                      num_elements
);


/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_1X_PROFILE_VAL

DESCRIPTION
  This function will set members of ds707_mip_static_profile .
  The first parameter is enum type for the val, second parameter is
  the value pointer and the third parameter specifies the number of
  elements to be filled in the corresponding variable of static
  profile if the val is of array type and it should  be 0 if the val 
  is of non-array type.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - True : Success
            False: Failure

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_mgr_set_1x_profile_val
(
  ds707_val_enum_type        type,
  void                      *val,
  uint8                      num_elements
);

/*===========================================================================

FUNCTION       DS707_SEC_PKT_MGR_GET_ALL_SECONDARY_FLOWS

DESCRIPTION    This is utility function and will return all the flows 
               associated with this iface        
        
DEPENDENCIES   

RETURN VALUE    1.Array of ps_flows
                2. number of ps_flows

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_sec_pkt_mgr_get_all_secondary_flows(

  /* iface ptr for which flows are requested */
  ps_iface_type     * this_iface_ptr,

  /* array of ps flow pointers with active fltrs.. OUTPUT parameter */
  ps_flow_type     * ps_flow_ptr_ptr[],

  /* array element count max*/
  uint8              max_flow_count, 
  /* number of ds flows in the above array.. OUTPUT parameter */
  uint8          * num_ps_flows
);

/*===========================================================================
FUNCTION      DS707_UPDATE_CALL_THROTTLE_INFO

DESCRIPTION
  This function stores temporarily the call throttle info for both 1x and HDR 
  received from CM and sends it to client who registered a callback function 
  with C2K Mode Handler.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_update_call_throttle_info
(
  const ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_TEAR_DOWN_ALL

DESCRIPTION   To tear down all active call 

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_tear_down_all
(
  void          *tear_down_info_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_END_REASON_FOR_ALL_IFACES

DESCRIPTION   To set call end reason for all valid ifaces

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_set_end_reason_for_all_ifaces
(
  ps_iface_net_down_reason_type call_end_reason
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_1X_DATA_PROFILE_PTR

DESCRIPTION
  This function will return address of ds707_mip_static_profile.
 
DEPENDENCIES
  None

RETURN VALUE
  Address of ds707_mip_static_profile

SIDE EFFECTS
  None
===========================================================================*/
ds707_mip_profile_type* ds707_pkt_mgr_get_1x_data_profile_ptr
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_CLEAR_LAST_ORIG_FAIL_REASON

DESCRIPTION    This function clears the reason the last data call's
               origination failed (if applicable).

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_clear_last_orig_fail_reason
(
  void
);

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
FUNCTION       DS707_PKT_MGR_SET_LAST_TUN_CALL_END_REASON

DESCRIPTION    This function sets the reason the last tunnel call
               ended (either failed during origination/normal teardown)

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_last_tun_call_end_reason
(
  uint32 end_reason
);
#endif /* FEATURE_DATA_OPTHO */

/*===========================================================================
FUNCTION DS707_PKT_MGR_CLEAR_LAST_CALL_END_REASON

DESCRIPTION    This function clears the reason the last data call's
               origination failed (if applicable).

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_clear_last_call_end_reason
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_CHECK_DCM_JCDMA_ALLOWED

DESCRIPTION    This function determines whether to allow or deny 
               further processing.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
boolean ds707_pkt_mgr_check_dcm_jcdma_allowed
(
  void
);

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_DSSNET6_SM_CB

DESCRIPTION    Returns the DSSNET6 SM for a given IFACE

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
dssnet6_sm_cb_type* ds707_pkt_mgr_get_dssnet6_sm_cb
(
  ps_iface_type* ps_iface_ptr
);
#endif /* FEATURE_DATA_PS_IPV6 */


/*===========================================================================
FUNCTION      DS707_PKT_IS_EHRPD_MODE_OF_OPERATION

DESCRIPTION   Utility function to see if the current serving system is eRHPD

DEPENDENCIES  None

RETURN VALUE  TRUE  - if serving system is eHRPD
              FALSE - otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_is_ehrpd_mode_of_operation
(
  void
);


#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION DS707_PKT_MGR_MMGSDI_UPDATE

DESCRIPTION    DS3GMMGSDI interface invoke 1xMH to update corresponding 1x
               CDMA services

DEPENDENCIES   None.

RETURN VALUE   TURE: corresponding 1x update is successful
               FALSE: corresponding 1x udpate fails

SIDE EFFECTS   None

===========================================================================*/
boolean ds707_pkt_mgr_mmgsdi_update
(

  ds707_pkt_mgr_mmgsdi_update_enum_type mmgsdi_update_type
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_MMGSDI_RESET

DESCRIPTION    Card removed so resetting corresponding 1x CDMA data

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_mmgsdi_reset(void);

/*===========================================================================
  FUNCTION DS707_MMGSDI_RESPONSE_CB()

  DESCRIPTION
    Callback function called by mmgsdi to return response

  PARAMETERS
    status  : return status
    cnf     : type of cnf response
    cnf_ptr : response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_mmgsdi_response_cb
(
  mmgsdi_return_enum_type  status, 
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type *  cnf_ptr
);

/*===========================================================================
FUNCTION DS707_READ_JCDMA_CARD_INFO()

DESCRIPTION
  Reads the JCDMA crad info.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds707_read_jcdma_card_info
(
  mmgsdi_session_id_type session_id
);
/*===========================================================================
FUNCTION DS3G_MMGSDI_STORE_JCDMA_CARD_INFO()

DESCRIPTION
  This function stores the jcdma card info into ds3g_mmgsdi_jcdma_card_info
  received from mmgsdi.

PARAMETERS
  cnf_type : Confirm type from mmgsdi.
  cnf_data : confirm data from mmgsdi.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds707_store_jcdma_card_info
(
  mmgsdi_cnf_enum_type cnf_type,
  mmgsdi_cnf_type      *cnf_data
);

/*===========================================================================
FUNCTION DS707_GET_JCDMA_CARD_INFO()

DESCRIPTION
  Returns the JCDMA crad info.

PARAMETERS
  None

RETURN VALUE
  MMGSDI_JCDMA_GRAY_CARD      : If card is a JCDMA card.
  MMGSDI_JCDMA_NON_JCDMA_CARD : If not a JCDMA crad.


DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_jcdma_card_info_enum_type  ds707_get_jcdma_card_info(void);

/*===========================================================================
FUNCTION DS707_INVALIDATE_JCDMA_INFO()

DESCRIPTION
  Invalidates JCDMA card info because some card error occured.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds707_invalidate_jcdma_card_info(void);

#endif /* FEATURE_MMGSDI_SESSION_LIB*/

/*===========================================================================

FUNCTION       DS707_PKT_MGR_IS_DATA_ON_EHRPD

DESCRIPTION    This is utility function and will return if the data call on 
               C2K is eHRPD 
        
DEPENDENCIES   

RETURN VALUE    boolean

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_pkt_mgr_is_data_on_ehrpd(void);

/*===========================================================================
FUNCTION       DS707_PKT_MGR_ALL_IFACES_DOWN

DESCRIPTION    This is utility function and will return true if all ifaces 
               are in down state. 
        
DEPENDENCIES   None.

RETURN VALUE   boolean

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_pkt_mgr_all_ifaces_down(void);

/*===========================================================================

FUNCTION       DS707_PKT_MGR_GET_MTU_SIZE_FROM_NV

DESCRIPTION    This function reads the NV items and provides the MTU size. 
               If the NV is inactive, the default values will be provided.
        
DEPENDENCIES   

RETURN VALUE    MTU value

SIDE EFFECTS   None.
===========================================================================*/
uint16 ds707_pkt_mgr_get_mtu_size_from_nv( void );

/*===========================================================================
FUNCTION      DSHDR_GET_MOBILE_IP_USES_CCOA

DESCRIPTION   Returns boolean dshdr_mobile_ip_uses_ccoa

DEPENDENCIES  None.

RETURN VALUE  Boolean

SIDE EFFECTS  None.
===========================================================================*/
boolean dshdr_get_mobile_ip_uses_ccoa(void);

/*===========================================================================
FUNCTION      DS707_GET_PRE_DORMANCY_PARAMS

DESCRIPTION   This function returns pre-dormant data call information such 
              as System Mode and Service Option (SO) used.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reasons are applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.
  DS_EOPNOTSUPP - Indicates system mode is non-C2K, in which case, pre-dormant
                  information is not applicable and so operation not supported
                  error is informed.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_get_pre_dormancy_params
(
  void     *argval_ptr, 
  int16    *p_errno
);

/*===========================================================================
FUNCTION      DS707_GET_CAM_TIMER

DESCRIPTION   
  This function gets CAM timer value currently being used for data call in 
  seconds from CAM module and returns it caller of this function. Caller of 
  this function should convert it to milliseconds or any other qualifier if 
  required.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_get_cam_timer
(
  void     *argval_ptr, 
  int16    *p_errno
);

/*===========================================================================
FUNCTION      DS707_SET_CAM_TIMER

DESCRIPTION   
  This function inform CAM module about the new CAM timer value to be used. The 
  new CAM timer value will take effect immediately if CAM module is not active. 
  If CAM is active, the new CAM timer value will take effect from next data 
  call onwards. This function receives the new CAM timer value in seconds and 
  CAM module converts it to milliseconds.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_set_cam_timer
(
  void     *argval_ptr, 
  int16    *p_errno
);

/*===========================================================================
FUNCTION      DS707_IS_SCRM_EABLED

DESCRIPTION   
  This function returns the information about whether SCRM (Supplemental 
  Channel Request Message) is enabled or not for data calls.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_is_scrm_enabled
(
  void     *argval_ptr, 
  int16    *p_errno
);

/*===========================================================================
FUNCTION      DS707_ENABLE_DISABLE_SCRM

DESCRIPTION   
  This function allows enabling or disabling of SCRM (Supplemental Channel 
  Request Message) for data calls.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_enable_disable_scrm
(
  void     *argval_ptr, 
  int16    *p_errno
);

/*===========================================================================
FUNCTION      DS707_IS_RDUD_ENABLED

DESCRIPTION   
  This function returns the information about whether RDUD (Reduced Dormancy 
  followed by Unsolicited Data) is enabled or not for data calls.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_is_rdud_enabled
(
  void     *argval_ptr, 
  int16    *p_errno
);

/*===========================================================================
FUNCTION      DS707_ENABLE_DISABLE_RDUD

DESCRIPTION   
  This function allows enabling or disabling of RDUD (Reduced Dormancy 
  followed by Unsolicited Data) for data calls. If RDUD module is not active, 
  then the new value of either enabling or disabling will take effect 
  immediately. If RDUD is active, the value of either enabling or disabling 
  will take effect from next data call onwards.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_enable_disable_rdud
(
  void     *argval_ptr, 
  int16    *p_errno
);

/*===========================================================================
FUNCTION      DS707_GET_SS_INFO

DESCRIPTION   Function to send cmd to CM to get status of SS info. 

DEPENDENCIES  None.

RETURN VALUE  TRUE indicates SUCCESS.
              FALSE indicates FAILURE.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_get_ss_info
(
  void
);
#ifdef FEATURE_EIDLE_SCI

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_EVDO_FORCE_LONG_SLEEP

DESCRIPTION   
  This function allows to set EVDO force long sleep throgh MH to HDR layer

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_mgr_set_evdo_force_long_sleep
(
  /* options for long sleep */ 
  void *argval_ptr,
  /* error number incase of failure */
  int16 *ps_errno
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_GET_EVDO_FORCE_LONG_SLEEP

DESCRIPTION   
  This function allows to get EVDO page monitor period through MH
  from HDR layer

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_mgr_get_evdo_page_monitor_period
(
  /* page monitor value, filled in */
  void *argval_ptr, 
  /* error number incase of failure */
  int16 *ps_errno
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_EVDO_PAGE_MONITOR_PERIOD

DESCRIPTION   
  This function allows to set slot mode option through MH to HDR
  layer. Mean while, it also sets evdo page monitor period callback 
  function. This call back function is an asynchronous function. It
  will be called when receiving IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV
  and IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV from HDR layer

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.
  DS_EOPNOTSUPP - SLotted mode is not supported
  DS_EFAULT - Slotted mode is out of range or invalid input data
  DS_ENONET - HDR System is not available
  

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_mgr_set_evdo_page_monitor_period
(
  /* option for slotted mode */
  uint8 slotted_mode_option, 
  /* async callback for notification about status of slotted mode option */
  ds707_pkt_mgr_set_evdo_page_monitor_period_cb* fn_ptr,
  /* error number incase of failure */
  int16* ps_errno
);     

/*===========================================================================
FUNCTION      DS707_PKT_MGR_REGISTER_CB_FOR_EVDO_SESSION_CHANGE_EVT

DESCRIPTION   
  This function allows to invoked registered callback
  if evdo session change event is triggered by HDR layer. 
  This is an asynchronuous call. This is triggered if SCI mode 
  is changed.

DEPENDENCIES  None.

RETURN VALUE
  0 indicates SUCCESS.
  -1 indicates FAILURE.

Following error reason(s) is(are) applicable for this function call.
  DS_EFAULT - Indicates invalid arguments passed to this function.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_mgr_register_cb_for_evdo_session_change_evt
(
  /* async callback for notifying session mode changed */
  ds707_pkt_mgr_evdo_page_monitor_period_changed_hdlr* fn_ptr, 
  /* error number incase of failure */
  int16* ps_errno
);

#endif /* FEATURE_EIDLE_SCI */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_UTIL_INIT

DESCRIPTION   
  Initiailize packet manager util

DEPENDENCIES  None.

RETURN VALUE None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_util_init
(
  void
);

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/*===========================================================================
FUNCTION      DS707_GET_MAX_PPP_HARD_FAILURES

DESCRIPTION   Returns value of ehrpd_to_hrpd_fb_config.max_hard_fallback_count

DEPENDENCIES  None.

RETURN VALUE  uint8

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds707_get_max_ppp_hard_failures(void);

/*===========================================================================
FUNCTION      DS707_GET_MAX_PPP_SOFT_FAILURES

DESCRIPTION   Returns value of ehrpd_to_hrpd_fb_config.max_soft_fallback_count

DEPENDENCIES  None.

RETURN VALUE  uint8

SIDE EFFECTS  None.
===========================================================================*/
uint8 ds707_get_max_ppp_soft_failures(void);

/*===========================================================================
FUNCTION      DS707_SET_PPP_FAILURE_FLAG

DESCRIPTION   Sets ehrpd_to_hrpd_fb_cb.ppp_failed flag. 
              This function should be called when
              max of hard or soft ppp failures reached.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_set_ppp_failure_flag
(
  boolean ppp_failed
);

/*===========================================================================
FUNCTION      DS707_SET_ROAM_FLAG

DESCRIPTION   Sets ehrpd_to_hrpd_fb_cb.is_roam. 
              This function should be called when
              device moves into roaming network.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_set_roam_flag
(
  boolean is_roam
);

/*===========================================================================
FUNCTION      DS707_IS_EHRPD_TO_HRPD_FB_ENABLED

DESCRIPTION   Returns boolean ehrpd_to_hrpd_fb_config.enabled

DEPENDENCIES  None.

RETURN VALUE  booolean 
TRUE -  If eHRPD to HRPD feature is enabled in NV 
FALSE - If not. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_is_ehrpd_to_hrpd_fb_enabled(void);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_LTE_ATTACH_SUCESS_IND

DESCRIPTION   This function indicates DS that LTE attach is successful and 
              DS can inform HDR layer to propose eHRPD next time when the
              device acquires DO signal, required for eHRPD to HRPD
              fallback feature.
 
PARAMETERS    None
 
DEPENDENCIES  None.
 
RETURN VALUE  None 

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_lte_attach_success_ind(void);

#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

/*===========================================================================
FUNCTION ds707_pkt_mgr_get_dns_v4_addr_req_type

DESCRIPTION
  Returns the request method in which to obtain the DNS IPv4 address during
  VSNCP negotiation

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  0 : None
  1 : IPCP packet in PCO
  2 : DNS v4 Request Container in PCO
  3 : Both methods

SIDE EFFECTS
  None

===========================================================================*/
vsncp_3gpp2i_dns_v4_addr_req_type ds707_pkt_mgr_get_dns_v4_addr_req_type(void);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_POST_GRACEFUL_PPP_TERMINATION_CMD

DESCRIPTION   Handles graceful PPP termination asynchronously by posting msg 
              to DS task. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_post_graceful_ppp_termination_cmd
(
  ps_iface_net_down_reason_type end_reason
);

#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION      DS707_IS_EHRPD_CAPABILITY_ALLOWED

DESCRIPTION   This function checks following flags and decide whether 
              eHRPD capability is allowed or not. This function should be called
              by hdrpacdsif.c

DEPENDENCIES  None.

RETURN VALUE  boolean 
  TRUE  - eHRPD allowed. 
  FALSE - eHRPD not allowed. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_is_ehrpd_capability_allowed(void);

#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================
FUNCTION DS707_IS_POLICY_HANDOFF_CLASS_EPC

DESCRIPTION
  Util function in to check if the passed in policy's handoff_class
  is of EPC class

DEPENDENCIES
  None.

PARAMETERS
  policy_info_ptr  pointer to policy info data structure.

RETURN VALUE
  TRUE - If Handoff Class is EPC
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds707_is_policy_handoff_class_epc
(
  acl_policy_info_type *policy_info_ptr
);
#endif /* FEATURE_EPC_HANDOFF */

/*===========================================================================
FUNCTION      DS707_CLOSE_HDR_SESSION

DESCRIPTION   To close HDR session

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_close_hdr_session(boolean close_is_silent);

#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
/*===========================================================================
FUNCTION      DS707_PKT_MGR_MIP_CAN_DO_SESS_CLOSE

DESCRIPTION   This function returns whether to close session upon mip failure 
              with cause err 67 (authentication error) 

DEPENDENCIES  None

RETURN VALUE  TRUE: to close session 
              FALSE: do NOT close session 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_mgr_mip_can_do_sess_close(byte mip_rrp_code);

#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/

#ifdef FEATURE_DATA_FALLBACK_LIST
/*===========================================================================
FUNCTION      DS707_PKT_MGR_CLEAR_PDN_FB_COUNT

DESCRIPTION
  This function clears the PDN failure count
PARAMETERS    

DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS None
===========================================================================*/
void ds707_pkt_mgr_clear_pdn_fb_count(void);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_FB_LIST_FALLBACK

DESCRIPTION
  This function let HDR fallback to HRPD when detects the input apn is in
  the fallback list

PARAMETERS    Primary physlink which was returned by 
              ds707_pkt_mgr_get_pri_phys_link

DEPENDENCIES  Assumes caller has called ds707_pkt_mgr_get_pri_phys_link().

RETURN VALUE  TRUE: DID fallback 
              FALSE: did NOT fallback 

SIDE EFFECTS None
===========================================================================*/
boolean ds707_pkt_mgr_fb_list_fallback(
                                   ps_iface_type *ps_iface_ptr,
                                   const char  *apn_string,
                                   const uint8 apn_len
                                );
#endif /*FEATURE_DATA_FALLBACK_LIST*/

/*===========================================================================

FUNCTION       DS707_PKT_MGR_ALL_DIAL_STR_DO_PKT_CALL

DESCRIPTION    This function is used to judge whether all dial string should 
               do pkt call when the dial string is passed to ds707_async_mgr.c 
        
DEPENDENCIES   

RETURN VALUE    TRUE: do packet call anyway
                FALSE: otherwise

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_pkt_mgr_all_dial_str_do_pkt_call(
                                              const byte *dial_str
                                              );
/*===========================================================================
FUNCTION DS707_PKT_MGR_IGNORE_LAST_PDN_IA_TIMER

DESCRIPTION
  returns whether to ignore the last PDN inactivity timer

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - to ignore
  FALSE - to proceed

SIDE EFFECTS
  None

===========================================================================*/
boolean ds707_pkt_mgr_ignore_last_pdn_ia_timer(void);

/*===========================================================================
  FUNCTION DS_3GPP2_UPDATE_DATA_CONFIG_INFO
  
  DESCRIPTION
    Puts all the required nv item file paths in the data_config_info.conf 
    file. Would be called from dsutils during powerup.
    
  PARAMETERS  
    file_paths_buf: To hold the efs item file paths
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    0 for success.
    -1 for failure.
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
int32 ds_3gpp2_update_data_config_info
(
  char  *file_paths_buf
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_3GPP2_EFS_ITEM_FILE_PATH_SIZE

DESCRIPTION   Returns the total 3gpp2 EFS item file paths size

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_pkt_mgr_3gpp2_efs_item_file_path_size
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_BP_PARAMS

DESCRIPTION   This function sets the BP paramaters value to the specified 
              values.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_set_bp_params(boolean is_enabled, uint32 timerVal);

/*===========================================================================
FUNCTION      DS707_DDS_CHANGE_HDLR

DESCRIPTION
  This function handles DDS switch related actions for the C2K data call. 
  DS aborts the dormant data call in case of DDS switch. For non-dormant 
  data call, CM sends call end and so data call is torn down.

PARAMETERS
  subs_id - New Subscription ID received as part of DDS switch.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds707_dds_change_hdlr
(
  sys_modem_as_id_e_type  new_subs_id
);

/*===========================================================================
FUNCTION       DS707_PKT_MGR_INCR_NUM_MIP_TO_SIP_TRANSITIONS

DESCRIPTION    This function increments the number of MIP to SIP transitions that 
               happened from power on till another power cycle is initiated. 
               This is used mainly for statistics purpose

DEPENDENCIES   None.

RETURN VALUE   none
 
SIDE EFFECTS   None
===========================================================================*/
void ds707_pkt_mgr_incr_num_mip_to_sip_transitions(void);

/*===========================================================================
FUNCTION       DS707_PKT_MGR_GET_NUM_MIP_TO_SIP_TRANSITIONS

DESCRIPTION    This function gives the number of MIP to SIP transitions that 
               happened from power on till another power cycle is initiated. 
               This is used mainly for statistics purpose

DEPENDENCIES   None.

RETURN VALUE   uint32 - No. of MIP to SIP transitions till a 
                        reset/powercycle is initiated.
 
SIDE EFFECTS   None

===========================================================================*/
uint32 ds707_pkt_mgr_get_num_mip_to_sip_transitions(void);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_NV_SYNC

DESCRIPTION   This function reads all the 707-specific NV items out of
              NV.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_nv_sync(void);

/*===========================================================================
FUNCTION DS707_PKT_MGR_READ_MIN1

DESCRIPTION
  This function reads the NV item NV_MIN1_I and prints the output into the
  string provided.
  The NV Item MIN1 is a 24-bit bit-string that contains 7 decimal digits
  encoded in this fashion.
  |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  |      10-bits      | 4-bits|     10-bits         |
  |<----------------->|<----->|<------------------->|
  |      3-digits     |1-digit|     3-digits        |
  |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  The output string contains the decimal digits in a NUL-terminated string.

DEPENDENCIES
  The argument has to be a non-NULL string pointer.

PARAMETERS
  digits : non-NULL string pointer to output the MIN1 digits to.

RETURN VALUE
  FALSE  if - null input or output string pointers are provided
            - output string length is out of range
            - NV item NV_MIN1_I can't be read
  TRUE   otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_mgr_read_min1
(
  byte *digits
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_READ_MIN2

DESCRIPTION
  This function reads the NV item NV_MIN2_I and prints the output into the
  string provided.
  The NV Item MIN2 is a 10-bit (16-bit in storage) bit-string that contains
  3 decimal digits encoded in this fashion.
  |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  |   6-bits  |     10-bits         |
  |<--------->|<------------------->|
  |  padding  |     3-digits        |
  |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  The output string contains the decimal digits in a NUL-terminated string.

DEPENDENCIES
  The argument has to be a non-NULL string pointer.

PARAMETERS
  digits : non-NULL string pointer to output the MIN1 digits to.

RETURN VALUE
  FALSE  if - null input or output string pointers are provided
            - output string length is out of range
            - NV item NV_MIN2_I can't be read
  TRUE   otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_mgr_read_min2
(
  byte *digits
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_READ_IMSI_MCC

DESCRIPTION
  This function reads the NV item NV_IMSI_MCC_I and prints the output into
  the string provided.
  The NV Item MCC is a 10-bit (16-bit in storage) bit-string that contains
  3 decimal digits encoded in this fashion.
  |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  |   6-bits  |     10-bits         |
  |<--------->|<------------------->|
  |  padding  |     3-digits        |
  |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
  The output string contains the decimal digits in a NUL-terminated string.

DEPENDENCIES
  The argument has to be a non-NULL string pointer.

PARAMETERS
  digits : non-NULL string pointer to output the MIN1 digits to.

RETURN VALUE
  FALSE  if - null input or output string pointers are provided
            - output string length is out of range
            - NV item NV_IMSI_MCC_I can't be read
  TRUE   otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_mgr_read_imsi_mcc
(
  byte *digits
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_READ_IMSI_MNC

DESCRIPTION
  This function reads the NV item NV_IMSI_11_12_I and prints the output into
  the string provided.
  The NV Item MNC is a 8-bit integer in the range of 0 to 99.
  The output string contains the decimal digits in a NUL-terminated string.

DEPENDENCIES
  The argument has to be a non-NULL string pointer.

PARAMETERS
  digits : non-NULL string pointer to output the MIN1 digits to.

RETURN VALUE
  FALSE  if - null input or output string pointers are provided
            - output string length is out of range
            - NV item NV_IMSI_11_12_I can't be read
  TRUE   otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_mgr_read_imsi_mnc
(
  byte *digits
);


/*===========================================================================
FUNCTION DS707_PKT_MGR_QCMIP_SET_MDR_VAL

DESCRIPTION
  This function checks the value of QCMIP.  If it is greater than 0, it
  modifies the MDR value to SO 33 preferred.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_mgr_qcmip_set_mdr_val
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_QCMIP_SET_CRM_VAL

DESCRIPTION
  This function checks the value of QCMIP.  If it is greater than 0, it
  modifies the CRM value to 2, which enables network model operation.
  Changing the value QCMIP back to 0 will restore CRM to its original value
  before it was changed by QCMIP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_mgr_qcmip_set_crm_val
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_SET_MDR_VAL_AND_SO_PKT_RECAL

DESCRIPTION
  This function sets the MDR value and synchronizes the available
  service options.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  Boolean indicating success/fail.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds707_pkt_mgr_set_mdr_val_and_so_pkt_recal
(
  unsigned int    qcmdr_val
);

/*===========================================================================
FUNCTION       DS707_IS_TERM_IN_PROGRESS

DESCRIPTION    This function returns if termination is in progress.

DEPENDENCIES   None

RETURN VALUE   TRUE : termination in progress 
               FALSE: not in progress
 
SIDE EFFECTS   None

===========================================================================*/
boolean ds707_is_term_in_progress
(
  void
); /* ds707_is_term_in_progress */

/*===========================================================================
FUNCTION DS707_POST_FLOW_GONULL_IFACE_DOWN_IND

DESCRIPTION
  This function post flow go null ind (so the phys link is unbinded
  and ref count decreased) and iface down ind

PARAMETERS
 Iface ptr
 Call end reason.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_post_flow_gonull_iface_down_ind
(
  ps_iface_type *this_iface_ptr,
  ps_iface_net_down_reason_type reason

);

/*===========================================================================
FUNCTION DS707_PKT_MGR_GET_CDMA_RAT_INFO

DESCRIPTION
  This function returns the RAT and SO masks for 1x.
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean - TRUE if RAT is valid and ESPM indiaction is received.
                 FALSE otherwise.

SIDE EFFECTS   None                 
==========================================================================*/
boolean ds707_pkt_mgr_get_cdma_rat_info
(
  uint32  *rat_mask,
  uint32  *so_mask
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_PH_EVENT_OPRT_MODE_CMD

DESCRIPTION   Process the CM_PH_EVENT_OPRT_MODE with LPM/OFFLINE/RESET/POWEROFF
              command obtained from CM

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_ph_event_oprt_mode_cmd
(
  ds_cmd_type *ds_cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_GRACEFUL_PPP_TERMINATION

DESCRIPTION   Handles graceful PPP termination. Currently used in 
              LPM/PWROFF/Card reset events

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_graceful_ppp_termination
(
  ps_iface_net_down_reason_type end_reason
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_PROCESS_PH_SYS_SEL_PREF_NORMAL_CMD

DESCRIPTION   Process the ph_sys_sel_pref command obtained from CM.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_process_ph_sys_sel_pref_normal_cmd
(
  ds_cmd_type *ds_cmd_ptr
);

/*===========================================================================
FUNCTION DS707_REG_UNREG_DSM_MEM_EVENT

DESCRIPTION
  This function registers/de-registers DS707's callback function for DSM memory
  events for DSM_MEM_LEVEL_LINK_LAYER_MANY and 
  DSM_MEM_LEVEL_LINK_LAYER_FEW.
  
PARAMETERS  
  boolean - flag to indicate whether the function call is for 
  registration or deregistration.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
==========================================================================*/
void ds707_reg_unreg_dsm_mem_event
(
  boolean reg_flag
);

/*===========================================================================
FUNCTION DS707_GET_DSM_FC_FLAG

DESCRIPTION
  This function returns the value for ds707_dsm_fc_flag.
  ds707_dsm_fc_flag is a global variable which indicates whether flow control
  is enabled or disabled depending on the DSM memory events.
  
PARAMETERS  
  None.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Boolean - Current value of ds707_dsm_fc_flag.
==========================================================================*/
boolean ds707_get_dsm_fc_flag
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_READ_HRPD_V6_ENABLED

DESCRIPTION   This function reads the EFS item and sets the 
              config file for IPv6 enabled for HRPD
  
PARAMETERS    None. 
      
DEPENDENCIES  None. 
  
RETURN VALUE  None.
==========================================================================*/
void ds707_pkt_mgr_read_hrpd_v6_enabled(void);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_AIMS_NV

DESCRIPTION   Read the NV item to check if AIMS mode is on
  
PARAMETERS    None. 
      
DEPENDENCIES  None. 
  
RETURN VALUE  None.
==========================================================================*/
void ds707_pkt_mgr_aims_nv(void);

/*===========================================================================
FUNCTION DS707_IS_BP_REMOVAL_ENABLED

DESCRIPTION
  This function returns the flag value which indicates whether back pressure
  removal is enabled or disabled.
  
PARAMETERS  
  None.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Boolean - TRUE if back pressure removal is enabled
            FALSE otherwise
==========================================================================*/
boolean ds707_is_bp_removal_enabled
(
  void
);

/*===========================================================================
FUNCTION DS707_PKT_MGR_READ_BP_ENABLED_NV

DESCRIPTION
  This function Initializes the back pressure related params.
  
PARAMETERS  
  None.
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Boolean - TRUE if back pressure removal is enabled
            FALSE otherwise
==========================================================================*/
void ds707_pkt_mgr_read_bp_enabled_nv(void);

/*===========================================================================
FUNCTION      DS707_PKT_BRING_DOWN_PHYS_LINK

DESCRIPTION   Bring down phys link (Currently used when in partial context)

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_bring_down_phys_link
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_LOCAL_ABORT

DESCRIPTION   Handles Local abort for NV refresh. Currently used in 
              nv refresh event case.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_local_abort
(
  uint32 subs_id,
  ps_iface_net_down_reason_type end_reason
);

/*===========================================================================
FUNCTION      DS707_PKT_SYS_EVENT_IND

DESCRIPTION   Calls ps_sys_event_ind_ex() for a given sys event on  subsID passed

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_sys_event_ind
(
  ps_sys_event_enum_type             event,
  void                               *event_data_ptr,
  ps_sys_subscription_enum_type     subs_id
);

/*===========================================================================
FUNCTION      ds707_pkt_mgr_check_coex_data_status

DESCRIPTION   This utility function disables all the flows bound to the physlink if there is coex 
data issue.
                       Otherwise it just calls phys link enable 

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  All the phys links are disabled.
===========================================================================*/

void ds707_pkt_mgr_check_coex_data_status(ps_iface_type *iface_ptr);


#ifdef FEATURE_DATA_FALLBACK_LIST
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* FEATURE_DATA_FALLBACK_LIST */

/*===========================================================================
FUNCTION       DS707_PKT_MGR_SET_FAILOVER_IP_MODE

DESCRIPTION    This function reads the failover ip type. Uses the failover mode 
               to set addr family of iface incase of dual IP when ip family in
               policy is UNSPEC

DEPENDENCIES   None

RETURN VALUE   None
 
SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_set_failover_ip_mode(void);

/*===========================================================================
FUNCTION       DS707_PKT_MGR_INIT_IGNORE_LAST_PDN_IA_TIMER_NV

DESCRIPTION    This function reads the ignore_last_pdn_ia_timer NV

DEPENDENCIES   None

RETURN VALUE   None
 
SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_init_ignore_last_pdn_ia_timer_nv(void);

/*===========================================================================
FUNCTION       DS707_PKT_MGR_ADDITIONAL_DIAL_STR_INIT

DESCRIPTION    This function reads nv for DS707_ADDITIONAL_DIAL_STR_EFS_PATH 
               for addional dial string provisioning

DEPENDENCIES   None

RETURN VALUE   None
 
SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_additional_dial_str_init(void);

/*===========================================================================
FUNCTION       DS707_PKT_MGR_FB_LIST_INIT

DESCRIPTION    This function initialize the parameters related to fallback pdn 
               list 

DEPENDENCIES   None

RETURN VALUE   None
 
SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_fb_list_init(void);


/*===========================================================================
FUNCTION       DS707_PKT_MGR_INIT_MIP_DO_SESS_CLOSE_NV

DESCRIPTION    This function reads nv on whether to close session 
               upon mip failure with cause err 67 (authentication error),
               and records it in static variable

DEPENDENCIES   None

RETURN VALUE   None
 
SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_init_mip_do_sess_close_nv(void);

/*===========================================================================
FUNCTION       DS707_PKT_MGR_READ_DNS_V4_ADDR_REQ_NV

DESCRIPTION    This function reads the dns_v4_addr_req_type NV. This NV
               specifies the method in which to obtain the DNS IPv4 address
               during VSNCP negotiation.

DEPENDENCIES   None

RETURN VALUE   None
 
SIDE EFFECTS   None

===========================================================================*/
void ds707_pkt_mgr_read_dns_v4_addr_req_nv( void );

/*===========================================================================
FUNCTION:     DS707_PKT_MGR_READ_EHRPD_TO_HRPD_FB_NV

DESCRIPTION:  This function reads the ehrpd_to_hrpd_fb NV. This NV specifies 
              ehrpd to hrpd fallback configuration.  

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void ds707_pkt_mgr_read_ehrpd_to_hrpd_fb_nv( void );

/*===========================================================================
FUNCTION      DS707_PKT_MGR_TRAFFIC_CH_TEARDOWN_EFS

DESCRIPTION   This function initializes the Data Services Mode Specific
              Handler interface.  It initializes the critical section
              used for call operations. 
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_traffic_ch_teardown_efs( void );

/*===========================================================================
FUNCTION DS707_PKT_MGR_UTIL_GET_THROTTLE_INFO_REQUESTED_FLAG

DESCRIPTION   This function returns the flag which was added for the case where 
              QMI queries 3GPP2 MH about the throttling status, for which
              CM_SS_EVT_ORIG_THR_STATUS_MASK will not be set.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_mgr_util_get_throttle_info_requested_flag( void );

/*===========================================================================
FUNCTION DS707_PKT_MGR_UTIL_SET_THROTTLE_INFO_REQUESTED_FLAG

DESCRIPTION  This function sets the value for the flag which was added for the 
             case where QMI queries 3GPP2 MH about the throttling status, for 
             which CM_SS_EVT_ORIG_THR_STATUS_MASK will not be set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_mgr_util_set_throttle_info_requested_flag
(
  boolean flag
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_PKT_MGR_H    */

