#ifndef DS707_PKT_MGRI_H
#define DS707_PKT_MGRI_H
/*===========================================================================

                        D S 7 0 7 _ P K T _ M G R I
GENERAL DESCRIPTION
  This file is the internal pkt_mgr include file.  
  
  IT SHOULD ONLY BE INCLUDED BY ds707_pkt_mgr*.c files.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2002-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.h_v   1.4   27 Nov 2002 11:02:18   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_pkt_mgri.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
-----------------------------------------------------------------------------  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/17/12   dvk     Merged fix to tear down 3gpp2 call when moving from 1x/HDR 
                   to 3gpp system if handoff is not allowed. 
11/18/11   vpk     HDR SU API cleanup
03/15/11    mg     Global variable cleanup
09/07/10    op     IPv6 enable/disable feature for HRPD
05/04/10    mg     Dual IP feature
03/20/09    spn    changed ds707_pkt_state to ds707_pdn_state
03/12/09  ssh/spn  PPP VSNCP interface changes
02/19/09    sk     eHRPD: UE initiated QoS
02/09/09    spn    Moved RMSM PDN instance to the RMSM Module. And Moved Data
                   Session Profile into the ds707_pkt_state
08/30/07    sk     Enable/Disable secondary physlink bound flows when primary
                   physlink bound flows are enabled/disabled
07/02/07    sc     Added two macros GET_DS707_DATA_SESSION_INFO_IPV4_PTR and
                   GET_DS707_DATA_SESSION_INFO_IPV6_PTR that return
                   ds707_data_session_info ptr given a packet instance.
05/01/07    sk     QoS Modify support
04/04/07    ac     Enabled independent inactivity timer for each DS flow
11/10/06    rsj    Added JCDMA_2 related changes. 
                   Created new enumerated type ds707_ordq_enum_type
                   Added member of ds707_ordq_enum_type in tc_state_type 
                   structure to allow ORDQ value to be passed from Data 
                   services to CM.
10/13/05    az     Support for IPv6
11/10/06    rsj    Added JCDMA_2 related changes. 
                   Created new enumerated type ds707_ordq_enum_type
                   Added member of ds707_ordq_enum_type in tc_state_type 
                   structure to allow ORDQ value to be passed from Data 
                   services to CM.
07/06/06    sk     QoS 2.0 Support
05/30/06    ksu    EMPA: RLP init code review comments
03/30/06    spn    Fixed Featurization for FEATURE_HDR_QOS. Added some 
                   functions.
03/23/06    ksu    convert RX PS command back to signal
02/22/06    ksu    don't float more than one RX command at a time
01/05/06    ksu    EMPA: rx queues per tc state
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
12/29/05    ksu    EMPA: framework for route protocol (RP)
12/23/05    ksu    EMPA RSP: two fwd rx queues
12/23/05    ksu    EMPA RSP: two rev watermarks per ip flow
11/07/05    sk     Added API bundling for QoS
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP 
                   resync during a handup from 1x to DO
03/29/05    gr     relocated the GET_TC_FROM_PHYS_LINK macro from sec pkt mgr
11/22/04    vas    Some code cleanup
10/21/04    kvd    cleaned up DS707 macros to be called with correct pkt_inst
09/10/04    vas    QOS related changes
04/24/04    ak     Lint fixes.
04/21/04    mct	   Renamed ds_dssnet6.h dsgen_iface6_hdlr.h.
03/22/04    ak     Extern for ok_to_orig().
03/15/04    ak     Merge to 03/15/04 IPV6 tip.
02/19/04    ak     Various upgrades to do IPV6.
01/08/03    ak     First version of file.
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
#include "ds707_pkt_mgr.h"
#include "dsm.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ps_phys_link.h"
#include "ds707_iface4_hdlr.h"
#include "queue.h"
#include "sys.h"
#include "ds707_pdn_context.h"
#include "hdrcom_api.h"
#include "hdrcp_api.h"

#ifdef FEATURE_DATA_PS_IPV6
  #include "dsgen_iface6_hdlr.h"
  #include "ps_ip6_sm.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ds707_qosdefs.h"
#ifdef FEATURE_HDR_QOS

#include "dsrlp_v.h"
#ifdef FEATURE_HDR_EMPA
#include "ps_rohc.h"
#endif /* FEATURE_HDR_EMPA */
#endif /* FEATURE_HDR_QOS */
#include "ps_hdlc_lib.h"
#include "dsrsp.h"
#ifdef FEATURE_EHRPD
#include "ps_ppp_ncp.h"
//#include "ds_tft_bldr.h"
#endif /*FEATURE_EHRPD */
#include "ds707_data_session_profile.h"



/*===========================================================================
                            TYPEDEFS
===========================================================================*/
#define DS707_PRI_TC_INST   0

#ifdef FEATURE_HDR_QOS
  
/* The number of phys links required in case QoS on HDR is supported is given
   by the total number of RLPs needed to be supported. */
  #define DS707_NUM_PHYS_LINKS (DSRLP_MAX_NUM_HDR_MRLP_REV + DSRLP_MAX_NUM_HDR_MRLP_FWD)

  /* Number of Reverse PS Flows. Same as number of Reverse reservation labels
     supported. Each PDN Instance has a default PS Flow, and hence 
     dependency on MAX_PDN_INSTANCES*/
     
  #define DS707_NUM_REV_PS_FLOWS ( DS707_N_RESV_MAX_REV + DS707_MAX_IFACES - 1 )

  /* Number of Reverse PS Flows. Same as number of Reverse reservation labels
     supported 
     Each PDN Instance has a default PS Flow, and hence dependency on 
     MAX_PDN_INSTANCES*/
  #define DS707_NUM_FWD_PS_FLOWS ( DS707_N_RESV_MAX_FWD + DS707_MAX_IFACES - 1 )

#else /* FEATURE_HDR_QOS */

  /* If no QoS is defined, only 1 Phys link is needed */
  #define DS707_NUM_PHYS_LINKS 1

  /* Number of Reverse PS Flows. Same as number of Reverse reservation labels supported */
  #define DS707_NUM_REV_PS_FLOWS 1

  /* Number of Reverse PS Flows. Same as number of Reverse reservation labels supported */
  #define DS707_NUM_FWD_PS_FLOWS 1

#endif /* FEATURE_HDR_QOS */


/*---------------------------------------------------------------------------
TYPEDEF DS707_PKT_DORM_ACTION_ENUM_TYPE

DESCRIPTION
  List of all the possible dormancy actions that phys link may want to do
  and may ask pkt mgr if it's okay to do that action.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_PKT_DORM_ACTION_MIN = 0,      /* DO NOT USE. Used for bounds check */
  DS707_PKT_DORM_ACTION_OK_TO_ORIG,   /* is it okay to orig from dormancy  */
  DS707_PKT_DORM_ACTION_OK_TO_GO_DORM,/* is it ok to go dormant            */
  DS707_PKT_DORM_ACTION_MAX           /* DO NOT USE. Used for bounds check */
} ds707_pkt_dorm_action_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF DS707_PKT_DORM_REASON_ENUM_TYPE

DESCRIPTION
  Contains reasons phys link wants to originate from dormancy.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_PKT_DORM_REASON_MIN = 0, /* DO NOT USE.  Used for bounds checking */
  DS707_PKT_DORM_REASON_DATA,    /* has data to send                      */
  DS707_PKT_DORM_REASON_PZ_S_NID,/* PZID/SID/NID changed                  */
  DS707_PKT_DORM_REASON_NETWORK, /* network changed                       */
  DS707_PKT_DORM_REASON_MAX      /* DO NOT USE.  Used for bounds checking */
} ds707_pkt_dorm_reason_enum_type;

/*---------------------------------------------------------------------------
  Each PS Flow is associated with a Reservation. This structure stores all
  related information for the PS Flow in the Forward & Reverses direction.
---------------------------------------------------------------------------*/
typedef struct ds707_tc_state_s ds707_tc_state_type;

typedef struct profile
{
  struct
  {
    uint16    profile[DS707_MAX_PROFILE_PER_FLOW];
    uint8     count;
    uint8     result_code;
  }fwd;
  struct
  {
    uint16    profile[DS707_MAX_PROFILE_PER_FLOW]; 
    uint8     count;
    uint8     result_code;
  }rev;
}profile_type;

typedef enum
{
  DS707_PKT_NOTIFY_INFO_CODE_INVALID  = 0,
  DS707_PKT_NOTIFY_INFO_CODE_AQOS     = 1,
  DS707_PKT_NOTIFY_INFO_CODE_DQOS     = 2
} ds707_pkt_notify_info_code;

typedef struct
{
  /*---------------------------------------------------------------------
  TO track the profile list.
  ----------------------------------------------------------------------*/
  profile_type    previous_confirm; /* Previous confirm profiles        */
  profile_type    confirmed;        /* Current confirm                  */
  profile_type    authenticated;    /* Authenticated profiles           */ 

  ps_flow_type *ps_flow_ptr;  /* Pointer to the PS Flow                    */
  ps_iface_type   * ps_iface_ptr; /* Pointer to corresponding iface 
                                     (v4 or v6) to which this ds_flow 
                                     (or ps_flow) belongs */
  ds707_tc_state_type *fwd_tc_ptr;  /* Pointer to the forward traffic chan */
  ds707_tc_state_type *rev_tc_ptr;  /* Pointer to the reverse traffic chan */
  uint32 dqos_user_data;    /* Opaque data used by DQOS module             */

  /*-------------------------------------------------------------------------
    The tx watermark list needs to point to an array of watermarks of
    length (at least) DSRSP_NUM_ROUTES
  -------------------------------------------------------------------------*/
  dsm_watermark_type *tx_wm_list;

  /*-------------------------------------------------------------------------
    State to keep track of whether air link QoS is granted or filters are
    installed on PDSN
  -------------------------------------------------------------------------*/
  struct
  {
    ds707_flow_fltr_state_enum_type          fltr_state;
    ds707_flow_air_link_qos_state_enum_type  air_link_qos_state;
  } state;

  uint8 fwd_resv_label; /* Reservation Label assigned for forward QoS      */
  uint8 rev_resv_label; /* Reservation Label assigned for reverse Qos      */
  ds707_direction_enum_type dirn; /* Type of the flow                      */
  ds707_ds_flow_state_enum_type ds_flow_state; /* state of the ds flow     */
  ps_extended_info_code_enum_type notified_info_code; /* previously notified
                                                                 info-code */
  struct
  { 
    ds707_pkt_notify_info_code notify_flag;
    ps_extended_info_code_enum_type aqos_info_code;
    ps_extended_info_code_enum_type dqos_info_code;
  } notify_info_code;
  
  qword qos_grant_timer_expire_time; /* The Time when grant timer started*/ 
  boolean qos_grant_timer_running;  /* indicates if the timer is running or 
                                                                        not*/
  ds707_flow_modify_state_enum_type qos_modify_state; /* QoS modify state */
  boolean ds707_flow_modify_flag;
  uint32 inactivity_timer_val;

  boolean nw_init_flow; /* to identify if nw init flow */
  uint32  nw_init_qos_tid; /* fjia: change to tidto identify if nw init flow */
} ds707_flow_type;


/*---------------------------------------------------------------------------
  ORDQ Enumerated type. This is used for JCDMA 2. Allows passing of
  information to ds707_pkti_end_call_hdlr for low battery event processing.
  This type is used by  ds707_pkt_mgr_iface_ioctl_go_null.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_ORDQ_NULL = 0,
  DS707_ORDQ_NONE = 1, 
  DS707_ORDQ_LOW_BATTERY = 2
} ds707_ordq_enum_type;

#ifdef FEATURE_DATA_OPTHO
/*-----------------------------------------------------------------------
Enum to determine the current tunnel phys link state 
The state is updated in the tc state ptr to be in sync with the PS 
phys link state. Not all transition states (Resuming, going down, 
going null) are captured here since there is no need for them right now.  
-----------------------------------------------------------------------*/
typedef enum
{
  DS707_TUN_PHYS_LINK_NULL = 0,
  DS707_TUN_PHYS_LINK_COMINGUP, 
  DS707_TUN_PHYS_LINK_UP,
  DS707_TUN_PHYS_LINK_DOWN,
  DS707_TUN_PHYS_LINK_MAX
} ds707_tun_phys_link_state;
#endif /* FEATURE_DATA_OPTHO */

/*---------------------------------------------------------------------------
  Traffic channel state info.  Contains info specific to one CM call.
  The RLP for a traffic channel could be either HDR RLP or 1X RLP.
  Data struct allows both to be intialized for a particular call, however,
  the TX & RX watermarks/queues (ie, connections to PPP/laptop) get set up
  for only one of the RLPs based on whether it is a 1X call or HDR call.
  
  A TC contains structures for both Forward and Reverse RLPs/Reservations.
  It is possible that both may be valid or only a single could be valid at
  a time.

  Note that, for the primary TC, both the Forward and the Reverse RLPs and
  reservations are valid.
     
---------------------------------------------------------------------------*/
struct ds707_tc_state_s
{
  byte                     alloc_sr_id;/* SR_ID allocated to this instance */
  ds707_so_type            so;         /* SO of this call                  */
  cm_call_id_type          call_id;    /* CM call id                       */

#ifdef FEATURE_DATA_OPTHO
  /*-----------------------------------------------------------------------
  Enum to determine the current tunnel phys link state while processing 
  tunnel calls via HDR. The state is updated in the tc state ptr to be 
  in sync with the PS phys link state. 
   
  When moving from prereg to direct mode, the tun_phys_link_state will 
  remain in DS707_TUN_PHYS_LINK_DOWN until the phys link is completely torn 
  down (i.e NULL state). In other words, any phys link state change in 
  direct mode (e.g coming up, resuming, up etc.) except state change to 
  NULL will not affect the tun_phys_link_state
  -----------------------------------------------------------------------*/
  ds707_tun_phys_link_state tun_phys_link_state;
#endif /* FEATURE_DATA_OPTHO */

  /* Structure stores all Forward RLP related information */
  struct
  {
    dsrlp_rlp_identity_type  flow;
    void                     *hdlc_inst_ptr;
    hdlc_config_type          hdlc_config;
    void                    (*post_rx_func_ptr)(uint32); /* RLP Rx callback*/

    /*-----------------------------------------------------------------------
      The rx WM list needs to point to an array of WMKs of
      length (at least) DSRSP_NUM_ROUTES
      ---------------------------------------------------------------------*/
    dsm_watermark_type      *post_rx_wm_list;  /* RLP Rx watermark */

#ifdef FEATURE_HDR_QOS
    /* RLP Rx queue list allocation for secondary RLPs (per tc state) */
    dsm_watermark_type       sec_rx_wm_list[DSRSP_NUM_ROUTES];
    q_type                   sec_rx_q_list[DSRSP_NUM_ROUTES];

#ifdef FEATURE_HDR_EMPA
    rohc_handle_type route_protocol_rohc_handle[DSRSP_NUM_ROUTES];
    rohc_handle_type flow_protocol_rohc_handle;
#endif /* FEATURE_HDR_EMPA */
#endif /* FEATURE_HDR_QOS */
    ds707_flow_type          *flow_ptr[DS707_NUM_FWD_PS_FLOWS];
  } fwd_rlp;

  /* Structure stores all Reverse RLP related information */
  struct
  {
    dsrlp_rlp_identity_type  flow;
    ds707_flow_type          *flow_ptr[DS707_NUM_REV_PS_FLOWS];
#ifdef FEATURE_HDR_EMPA
    rohc_handle_type route_protocol_rohc_handle[DSRSP_NUM_ROUTES];
    rohc_handle_type flow_protocol_rohc_handle;
#endif /* FEATURE_HDR_EMPA */
  } rev_rlp;

  /* Phys link poiner. Currently only the primary TC should point to a
     phys_link. All other TCs will have this set to NULL. */
  ps_phys_link_type        *ps_phys_link_ptr;

  /*Provide ORDQ reason enumerated type so we can pass information
    about reason to call go_null for low battery event processing*/
  ds707_ordq_enum_type      ordq_value;
 
};

/*---------------------------------------------------------------------------
  If this FLAG is true it means that we may need to initiate a PPP Resync
---------------------------------------------------------------------------*/
extern boolean                  ds707_pkt_mgr_ppp_resync_flag;

/*-------------------------------------------------------------------------
  To complete the PPP resync without interruption from GPS
  ppp_resync_lock_release_cb will be called upon successfully releasing
  the ppp_resync_lock, if a valid callback is registered
-------------------------------------------------------------------------*/
extern void (*ds707_pkt_mgr_ppp_resync_lock_release_cb)(void);

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*-------------------------------------------------------------------------
  The current 707 network - 1X, HDR or NONE
-------------------------------------------------------------------------*/
extern sys_sys_mode_e_type ds707_current_data_sess_network;

/*===========================================================================
                     INTERNAL MACRO DEFINITIONS
===========================================================================*/
/*===========================================================================
MACRO         VERIFY_PKT_INSTANCE

DESCRIPTION   Makes sure the packet instance is an okay value.

DEPENDENCIES  None.

RETURN VALUE  None.  Will ERR_FATAL if the value is no good.

SIDE EFFECTS  None.
===========================================================================*/
#define VERIFY_PKT_INSTANCE(x)
                                
/*===========================================================================
MACRO         DS707_PRI_DS_FLOW_PTR

DESCRIPTION   Given the pkt instance, returns pointer to the primary DS Flow
              Note that even though we refer to the fwd_rlp flow_ptr, we 
              could have used the rev_rlp flow_ptr too, since both of them
              point to the same element for the primary flow

              NOTE: There are multiple PRI DS Flows, one per iface. This
                    MACRO just returns the first of such primary DS flows

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
#define DS707_PRI_DS_FLOW_PTR(tc_state_ptr) ((tc_state_ptr)->fwd_rlp.flow_ptr[0])


/*===========================================================================
MACRO         GET_PKT_STATE_FROM_CALL_INFO

DESCRIPTION   Given the call_info_ptr, which is often passed to this
              module from 3G DSMGR, extracts out the pkt state instance.

DEPENDENCIES  The input 'x' is assumed to be of type:
                ds3g_pkt_call_info_type

RETURN VALUE  Returns the packet instance ID, which is the index into the
              array of pkt_staes.

SIDE EFFECTS  None.
===========================================================================*/
#define GET_PKT_INST_FROM_CALL_INFO(x)\
 (uint32)(((ds3g_pkt_call_info_type *)x)->pkt_instance)

/*===========================================================================
MACRO         GET_TC_FROM_PHYS_LINK_PTR

DESCRIPTION   Given the phys link pointer, returns the TC that the phys link
              is associated with. The TC is stored in the client_data_ptr of
              the phys link.

DEPENDENCIES  None.

RETURN VALUE  TC pinter

SIDE EFFECTS  None.
===========================================================================*/
#define GET_TC_FROM_PHYS_LINK_PTR(p_ptr) (ds707_tc_state_type *)((p_ptr)->client_data_ptr)


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_CAN_GO_DORMANT

DESCRIPTION   Returns TRUE if iface can go dormant.  Else returns FALSE

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkti_can_go_dormant
(
  ds707_tc_state_type      *tc_state_ptr,
  ps_phys_link_type        *phys_link_ptr
);

/*===========================================================================
FUNCTION      DS707_PKTI_OK_ORIG_FROM_DORM

DESCRIPTION   Returns TRUE if it is okay to originate from domancy.  FALSE
              otherwise.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkti_ok_orig_from_dorm
(
  ps_phys_link_type              *phys_link_ptr,
  ds707_pkt_dorm_reason_enum_type  reason
);

/*===========================================================================
FUNCTION      DS707_PKTI_IS_DORM_ACTION_ALLOWED

DESCRIPTION   Called for a specific Phys link.  Returns TRUE or FALSE, on 
              whether the dormancy action allowed (based on the current state
              of the phys link)

DEPENDENCIES  The assumption is that this function shall only be used by
              the primary phys link

RETURN VALUE   TRUE - ok to orig from dormancy or go dormant.
               FALSE - do not orig from dormancy or do not go dormant.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkti_is_dorm_action_allowed
(
  ps_phys_link_type               *phys_link_ptr,
  ds707_pkt_dorm_action_enum_type   action,
  ds707_pkt_dorm_reason_enum_type   reason
);

/*===========================================================================
FUNCTION      DS707_PKTI_GET_PKT_STATE_INDEX_CID

DESCRIPTION   Given a CM call id, will get the pkt state index.

DEPENDENCIES  None

RETURN VALUE  If found, will return the index.  If not found, then will
              return DS707_MAX_PKT_CALLS.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_pkti_get_pkt_state_index_cid
(
  cm_call_id_type       call_id
);

/*===========================================================================
FUNCTION      DS707_PKTI_GET_ORIG_SO

DESCRIPTION   Returns the SO to originate with.

DEPENDENCIES  None

RETURN VALUE  Service option to originate with.

SIDE EFFECTS  None.
===========================================================================*/
ds707_so_type ds707_pkti_get_orig_so
(
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_PS_TO_DS_FLOW_MAP

DESCRIPTION   Sets up the client_data_pointer of the PS flow to point to the
              specfied ds_flow_ptr

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_set_ps_to_ds_flow_map
(
  ps_flow_type  * ps_flow_ptr,
  ds707_flow_type *ds_flow_ptr
);

/*===========================================================================
FUNCTION      DS707_PKT_FLOW_GET_DS_FLOW

DESCRIPTION   Given a pointer to a PS Flow, gets the corresponding DS Flow
              that it is part of.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
ds707_flow_type * ds707_pkt_mgr_get_ds_flow
(
  ps_flow_type  * ps_flow_ptr
);


/*===========================================================================
FUNCTION      DSRLP_REG_SRVC_EX

DESCRIPTION   Registers the watermarks and queues/callbacks for an RLP
              session.

DEPENDENCIES  A valid sr_id value must be passed into this function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_reg_all_routes_for_mrlp
(
  byte                rscb_index,            /* index for a session id     */
  dsrlp_ver_enum_type ver,                   /* RLP version                */
  dsm_watermark_type  *tx_watermark_ptr,     /* ptr to Tx wm for txmit frs */
  void                (*post_rx_func_ptr)(uint32), /* rx callback for rx 
                                                                   frames  */
  dsm_watermark_type   post_rx_wm_list[],    /* watermarks for rx'ed frames*/
  uint32               post_rx_data          /* arg to post_rx_func_ptr()  */
);
/*===========================================================================
FUNCTION      DS707_PHYS_LINK_ENABLE_FLOW

DESCRIPTION   This utility function enables all the flows bound to the physlink

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_phys_link_enable_flow
(
  uint32 flow_mask
);

/*===========================================================================
FUNCTION      DS707_PHYS_LINK_DISABLE_FLOW

DESCRIPTION   This utility function disables all the flows bound to the 
              physlink

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_phys_link_disable_flow
(
  uint32 flow_mask
);

/*===========================================================================
FUNCTION      DS707_GET_LAST_BEARER_TECH

DESCRIPTION   Returns the struct containing the last bearer tech

DEPENDENCIES  None.

RETURN VALUE  ps_iface_bearer_technology_type

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_bearer_technology_type ds707_get_last_bearer_tech(void);

/*===========================================================================
FUNCTION      DS707_SET_LAST_BEARER_TECH

DESCRIPTION   Sets the struct containing the last bearer tech

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_set_last_bearer_tech(
  ps_iface_bearer_technology_type bearer_tech
);

/*===========================================================================
FUNCTION      DS707_GET_HAS_PHONE_ACQUIRED_SIGNAL

DESCRIPTION   Returns boolean stating whether phone has acquired signal

DEPENDENCIES  None.

RETURN VALUE  Boolean

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_get_has_phone_acquired_signal(void);

/*===========================================================================
FUNCTION      DS707_SET_HAS_PHONE_ACQUIRED_SIGNAL

DESCRIPTION   Sets the value of ds707_has_phone_acquired_signal flag

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_set_has_phone_acquired_signal(boolean acquired_signal_val);

/*===========================================================================
FUNCTION      DS707_GET_PPP_RESYNC_FLAG_PTR

DESCRIPTION   Returns the pointer to flag that tells whether reconnected 
              due a change in SID/NID/PZID

DEPENDENCIES  None.

RETURN VALUE  Boolean*

SIDE EFFECTS  None.
===========================================================================*/
boolean* ds707_get_ppp_resync_flag_ptr(void);

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS707_PKT_MGRI_GET_IP_FAILOVER_MODE

DESCRIPTION   This function returns the value of IP failover mode. This is 
              called to resolve address family when policy says UNSPEC
              and profile says V4_V6 for a call.

DEPENDENCIES  None.

RETURN VALUE  ip_addr_enum_type.

SIDE EFFECTS  None.
===========================================================================*/
ip_addr_enum_type ds707_pkt_mgri_get_ip_failover_mode(void);

/*===========================================================================
FUNCTION      DS707_IS_HRPD_IPV6_ENABLED

DESCRIPTION   Returns whether IPv6 is enabled through EFS item or not for HRPD.

DEPENDENCIES  None.

RETURN VALUE  Boolean.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_is_hrpd_ipv6_enabled(void);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION      DS707_GET_PRI_TC_PTR

DESCRIPTION   Given the pkt instance, returns the tc_state_ptr.

DEPENDENCIES  None.

RETURN VALUE  ds707_tc_state_type*

SIDE EFFECTS  None.
===========================================================================*/
ds707_tc_state_type* ds707_get_pri_tc_ptr(void);

/*===========================================================================
FUNCTION      DS707_GET_SEC_TC_PTR

DESCRIPTION   Given the pkt instance and index, returns a pointer to the
              tc_state_ptr.

DEPENDENCIES  None.

RETURN VALUE  ds707_tc_state_type*

SIDE EFFECTS  None.
===========================================================================*/
ds707_tc_state_type* ds707_get_sec_tc_ptr(uint32 index);

/*===========================================================================
FUNCTION      DS707_GET_FLOW_POOL_PTR

DESCRIPTION   Given index, returns flow pool pointer. 

DEPENDENCIES  None.

RETURN VALUE  ds707_flow_type*.

SIDE EFFECTS  None
===========================================================================*/
ds707_flow_type* ds707_get_flow_pool_ptr(uint8 index);

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
FUNCTION      DS707_TRANSITION_TUN_PHYS_LINK_STATE

DESCRIPTION   Given the tc_state_ptr, transitions the tun phys link to the 
              requested state 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_transition_tun_phys_link_state
(
  ds707_tc_state_type* tc_state_ptr,
  ds707_tun_phys_link_state tun_phys_link_state
);

/*===========================================================================
FUNCTION      DS707_GET_TUN_PHYS_LINK_STATE

DESCRIPTION   Given the tc_state_ptr, gets the current tun phys link state

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_tun_phys_link_state ds707_get_tun_phys_link_state
(
  ds707_tc_state_type* tc_state_ptr
);
#endif /* FEATURE_DATA_OPTHO */

/*===========================================================================
FUNCTION  DS707_PKT_MGR_HANDOFF_ALLOWED

DESCRIPTION
  This function determines if a handoff from an old_sys to new_sys is
  allowed. 

PARAMETERS
  sys_sys_mode_e_type: old pref sys
  sys_sys_mode_e_type: new pref sys

DEPENDENCIES
  NONE

RETURN VALUE
  Returns TRUE when handoff allowed
          FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds707_pkt_mgr_handoff_allowed(
  sys_sys_mode_e_type old_sys,
  sys_sys_mode_e_type new_sys
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_PKT_MGRI_H    */

