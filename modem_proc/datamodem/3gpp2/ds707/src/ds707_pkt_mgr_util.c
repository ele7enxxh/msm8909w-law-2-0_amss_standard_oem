/*===========================================================================

                   D S 7 0 7 _ P K T _ M G R _ U T I L . C
                   
GENERAL DESCRIPTION
  This file contains functions that are used internally by ds707_pkt_mgr*.c
  and externally also.  These are utility functions that return state, etc.

EXTERNALIZED FUNCTIONS
  DS707_PKT_IS_DORMANT
    Returns if the packet call is dormant.

  DS707_PKT_GET_PREV_CALL_SO
    Returns the service option of the previous packet call.

  DS707_PKT_GET_PREV_SR_ID
    Returns the SR_ID of the previous packet call.

  DS707_PKT_GET_CURR_TX_WM_FROM_RLP
    Returns a pointer to the wmk going to RLP.

  DS707_PKT_GET_CURR_TX_WM
    Returns a pointer to tx wmk from pkt state depending on call instance.

  DS707_PKT_REORIG
    Called when dormant and the RLP tx watermark goes non-empty.  Sets
    signal for DS task to re-originate out of dormancy.

  DS707_PKT_MGR_INIT
    Called at startup.  Initializes the entire 1X pkt system, including
    any supporting modules.

  DS707_PKT_CM_INIT
    Registers with CM for any 1X/707 specific functionality.

  DS707_PKT_PROCESS_CMD
    Main processing loop for any commands which are for 1x pkt.

  DS707_PKT_PROCESS_SIGNALS
    Main processing loop for any signals set for 1x pkt.

  DS707_PKT_GET_IFACE_PTR
    Returns pointer to the 1x pkt ps_iface.

  DS707_PKT_SETUP_RLP
    Called by ds707_rmsm when starting a call.  If in a laptop call, then
    a different set of watermarks are registered with RLP than if in either
    a network or sockets model call.

  DS_VERIFY_SO
    Called by MC before making a packet data call.  Required because system
    P_REV can change between the the origination request to CM and the actual
    request going out over the air.

  DS707_PKT_GET_ACTIVE_IFACE_PTR
    Returns a pointer to the iface, if it is active.  If not active, then
    returns NULL.

  DS707_PKT_SETUP_SN_PPP
    Called when the pkt iface is coming up, to initialize PPP.

  DS707_PKT_TX_UM_SN_DATA
    Called by PPP to put data into RLP rev link watermark.

  DS707_PKT_ENABLE_HOLDDOWN_TIMER
    Re-starts the dormant(holddown) timer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds707_pkt_mgr_init() must be called at startup.

  Copyright (c) 2002-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_util.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/23/15    sd     Fixed throttling info updation on query through QMI WDS. 
05/04/15    sc     Generate generic indications only for up ifaces
08/05/14    ms     DSDA Phase 2 changes.
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
04/04/14    niv    Klocwork error fixes.
11/15/13    sd     Made change to return an error flag to ATCoP when user tries
                   to set MDR to any value other than 3 for a non-zero 
                   MIP value.
10/17/13    sd     Fix to avoid the overwriting of NV item parameters
                   for general user profile.
07/25/13    sd     Fix to update MDR value to NV if the current task is DCC.
07/18/13    sd     Fixed incorrect check to set MDR depending on QCMIP value.
06/25/13    sd     Boundary check for 707-mip profile variables.
06/19/13    sd     Make network model as default call type for DUN call
06/03/13    sd     ATCoP decoupling changes
11/21/12    sd     Feature Dual SIM related changes.
10/17/12    dvk    Merged fix to tear down 3gpp2 call when moving from 1x/HDR 
                   to 3gpp system if handoff is not allowed. 
01/05/12    msh    Cannot use errno as variable name 
12/23/11    sb     Fixed compiler warnings 
12/07/11    jz     Add Close HDR function 
11/18/11    vpk    HDR SU API cleanup
11/16/11    dvk    Merged change related to HDR mode check addition in get 
                   pre-dorm params function.
11/09/11    jee    To fix phys link dorm handling
10/04/11    vpk    Provide support for primary physlink to outlive iface
09/26/11    msh    Fixed compiler warnings 
09/14/11    ms     Resolve KW errors 
08/11/11    var    Resolve Compiler warnings & KW errors.
08/09/11    sha    Global variable clean up.
06/13/11    dvk    Merged support to get pre-dormant data call info, to set/get 
                   CAM timer value, to enable/disable SCRM/RDUD, to get SIP/MIP 
                   status and to get call throttle info.
03/15/11    mg     Global variable cleanup
02/25/11    op     Klocwork fix
02/18/11    mg     Added utility to check if all ifaces are down
02/11/11    ack    Global Variable Cleanup
10/14/10    ss     Adding debug messages before ASSERT.
10/13/10    op     Migrated to MSG 2.0 macros
06/24/09    ms     Enable the Physlink for secondary flows with out 
                   checking for stream assignment.
03/20/09    spn    changed ds707_pkt_state to ds707_pdn_state
02/14/09    ls     Initialize epzid in ds3g_pkt_call_info_type.
10/06/08    sn     Fix to make sure HDR SMP deactivate is not sent when 1x 
                   data 
                   call ends.
10/18/07    sc     Changed ds707_pkt_is_um_iface_up() to take care of 
                   Routable and Lingering Iface.
08/30/07    sk     Enable/Disable secondary physlink bound flows when primary
                   physlink bound flows are enabled/disabled
07/02/07    sc     Added new funtion DS707_PKT_MGR_SETUP_PKT_CALL_INFO() and
                   new struct ds3g_pkt_call_info_type. The function sets up
                   the struct with info needed to originate or end calls.
09/11/06    as     Added new function to determine if Iface is UP.
03/30/06    spn    Fixed Featurization for FEATURE_HDR_QOS
03/18/06    spn    Added a function to clear the primary TC to RLP bindings.
12/23/05    ksu    EMPA RSP: two fwd rx queues
12/23/05    ksu    EMPA RSP: two rev watermarks per ip flow
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP 
                   resync during a handup from 1x to DO
09/09/05    vrk/gr EPZID enhancements related to L2/L3 ack
09/02/05    as/gr  using ds3g_is_dorm_allowed_in_pkt_mode instead of 
                   ds3g_is_pref_pkt_mode to check if dormany is allowed in 
                   current mode in ds707_pkti_can_go_dormant. 
06/15/05    gr     Added check in dormancy util to prohibit dormancy 
                   in PHYS_LINK_COMING_UP state
05/16/05    gr     Eliminated pzid_orig_allowed function and moved it into
                   dorm_action_allowed function
04/22/05    ks     Fixed change for phys link NULL state
09/08/04    vas    (For QOS) Change to be consistent with modifications in TC
                   structure
04/24/04    ak     Lint fixes.
04/21/04    mct	   Renamed ds_dssnet6.h dsgen_iface6_hdlr.h.
03/22/04    ak     Make use of dorm_status functionality.
02/19/04    ak     Removed functions for getting dssnet SM's.
11/04/04    rsl    Added code under FEATURE_HDR to get correct data in 
                   get_data_count_hdlr and get_call_info_hdlr.
10/30/03    vr     Inform hdrpac if there is a handdown from HDR to 1X under
                   FEATURE_HDR_PPP_RESYNC_ON_HANDDOWN
10/19/03    vas    End Key causes PPP abort if 
                   FEATURE_HDR_SESSION_TIED_TO_PPP is defined
10/28/03    ak     Fixes for JCDMA service options.  On incoming call, see
                   if packet allowed.  When transitioning in and out of
10/13/03    vr/kvd Added support for ALN and PPP resync on handdown to 1X
10/13/03    vas    Changed hdr rlp initialization/cleanup. Added code that
                   will register functions for RLP initialization when Data 
                   Session initiated
02/20/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#ifdef FEATURE_DATA_IS707
  #define FEATURE_DSM_WM_CB

  #include "cm.h"
  #include "ds3gmgr.h"
  #include "ds3gutil.h"
  #include "ds707.h"
  #include "ds707_dorm_timer.h"
  #include "ds707_p_rev.h"
  #include "ds707_pkt_mgr.h"
  #include "ds3gcfgmgr.h"
  #include "ds707_pkt_mgri.h"
  #include "ds707_so_async.h"
  #include "ds707_so_pkt.h"
  #include "ds707_iface4_hdlr.h"
  #include "dsat707extctab.h"
  #include "dsat707vendctab.h"
  #include "dsm.h"
  #include "dsrlpi.h"
  #include "dstaski.h"
  #include "dstask.h"
  #include "ps_iface.h"
  #include "ps_iface_defs.h"
  #include "ps_phys_link.h"
  #include "ds707_data_session_profile.h"
  #include "data_err_suppr.h"
  #include "ds707_pdn_context.h"

  #ifdef FEATURE_DATA_PS_IPV6
    #include "dsgen_iface6_hdlr.h"
    #include "ps_ip6_sm.h"
  #endif /* FEATURE_DATA_PS_IPV6 */

  #include "ds_1x_profile.h"

#ifdef FEATURE_HDR_QOS
#include "ds707_sec_pkt_mgr_util.h"
#include "hdrcom_api.h"
#endif

#ifdef FEATURE_EIDLE_SCI
#include "hdrpac.h"
#endif /* FEATURE_EIDLE_SCI */

#include "cai.h"
#include "dscam.h"
#include "dsati.h"
#include "dsrdud.h"
#include "ds3gcmif.h"

#include "hdrcp_api.h"
#include "ds_dsd_ext_i.h"

#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

#include "ds707_sys_chg_hdlr.h"
#include "ds707_roaming.h"
#include "ds707_jcdma_m51.h"
#include "ds707_epzid.h"
#ifdef FEATURE_DS_EPZID_HYSTERESIS
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

#include "dcc_task_svc.h"
/*===========================================================================
                            MACROS AND TYPEDEFS
===========================================================================*/
#define SUCCESS      0
#define FAILURE     -1

#define IMSI_HUNDREDS_DIGIT(a)  ((a/100)%10)
#define IMSI_TENS_DIGIT(a) ((a/10)%10)
#define IMSI_ONES_DIGIT(a)  (a%10)

#define MAX_1X_PROFILES  1

/*lint -save -e641 Reason for suppressing error 641*/

/*===========================================================================
                            VARIABLES
===========================================================================*/
/*-------------------------------------------------------------------------
  The current 707 network - 1X, HDR or NONE
-------------------------------------------------------------------------*/
sys_sys_mode_e_type ds707_current_data_sess_network = SYS_SYS_MODE_NO_SRV;

/*-------------------------------------------------------------------------
  The last 707 network - 1X, HDR or NONE
  If we have a dormant or active packet data session network, this will
  reflect where the data session was prior to moving to the current system.
  If we do not currently have a packet data session, this will be set to
  NO_SRV to indicate that we do not have a current packet data session.
-------------------------------------------------------------------------*/
static sys_sys_mode_e_type ds707_last_data_sess_network = SYS_SYS_MODE_NO_SRV;

/*------------------------------------------------------------------------- 
  info needed to orig / end call
-------------------------------------------------------------------------*/
static ds3g_pkt_call_info_type pkt_call_info; 

#ifdef FEATURE_DATA_OPTHO
/*---------------------------------------------------------------------------
  Used mainly for msg printing
  These need to be in sync with the ds707_tun_phys_link_state enum
---------------------------------------------------------------------------*/
static char *tun_phys_link_state_names[DS707_TUN_PHYS_LINK_MAX] =
{
  "TUN_PHYS_LINK_NULL",
  "TUN_PHYS_LINK_COMINGUP",
  "TUN_PHYS_LINK_UP",
  "TUN_PHYS_LINK_DOWN"
};
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_EIDLE_SCI
/*-------------------------------------------------------------------------
    Define a structure that can be to store QMI callbacks for pagemonitor 
    and  session change notifiers.    
-------------------------------------------------------------------------*/
typedef struct ds707_pkt_mgr_hdrpac_hdlr_tag
{ 
  /* Callback for slotted mode return*/
  ds707_pkt_mgr_set_evdo_page_monitor_period_cb *slotted_mode_hdlr;
  /* Callback for session change handler */
  ds707_pkt_mgr_evdo_page_monitor_period_changed_hdlr *session_chg_hdlr;
  
} ds707_pkt_mgr_hdrpac_hdlr_struct;

/*---------------------------------------------------------------------------
  Global variable to store the for QMI callbacks.
---------------------------------------------------------------------------*/
static ds707_pkt_mgr_hdrpac_hdlr_struct ds707_pkt_mgr_hdrpac_hdlr;

/*---------------------------------------------------------------------------
  MIP-707 1x static profile
---------------------------------------------------------------------------*/
ds707_mip_profile_type ds_1x_profiles[MAX_1X_PROFILES];
#define ds707_mip_static_profile ds_1x_profiles[0]

uint32  ds707_cta_val;
uint8   ds707_qcmdr_val;

/*---------------------------------------------------------------------------
  The array ds707_qcdns_val stores the HEX value obtained after 
  concatenating the incoming string.
  ds707_qcdns_val[0] is for Primary DNS IP address
  ds707_qcdns_val[1] is for Secondary DNS IP address
  Since we store Primary and Secondary DNS IP addresses at two separate
  locations we need an array. The "resolver" function, accesses the IP
  addresses through an index to this array.
---------------------------------------------------------------------------*/
uint32 ds707_qcdns_val[MAX_QCDNS_VAL];

#ifdef FEATURE_IS2000_R_SCH
/* TURN SCRM on/off */
uint8  ds707_qcscrm_val;
/* TURN R-SCH throttle on/off */
uint8  ds707_qctrtl_val;
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_CHS
/* Enable/disable Mobile-originated Control/Hold */
uint8  ds707_qcchs_val;
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_IS2000_REL_A
uint8  ds707_qcqospri_val;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_DS_IS707A_CMUX
uint8  ds707_cmux_val[MAX_CMUX_VAL];
#else
uint8  ds707_cmux_val;
#endif  /* FEATURE_DS_IS707A_CMUX */

/*-------------------------------------------------------------------------
  Dial string for Packet/QNC calls. Defaults to "#777". Note that if the
  default string is changed, the length variable should also be updated.
-------------------------------------------------------------------------*/
byte ds707_pkt_mgr_orig_str[NV_MAX_PKT_ORIG_DIGITS] = "#777";

/*-------------------------------------------------------------------------
  Length of Packet/QNC dial string (including the terminating NULL)
-------------------------------------------------------------------------*/
byte ds707_pkt_mgr_orig_str_len = 5;

/*-------------------------------------------------------------------------
  Dial string for forcing the pkt call to be CDMA only. By default
  this string is not populated. It should be retrieved from NV
-------------------------------------------------------------------------*/
byte ds707_pkt_mgr_force_cdma_dial_str[NV_MAX_PKT_ORIG_DIGITS] = "";
byte ds707_pkt_mgr_force_cdma_str_len = 0;

/*---------------------------------------------------------------------------
  The ds_atcop_crm_orig_val variable is used to store the original value
  of CRM.  A value of -1 indicates that it has not been initialized.
---------------------------------------------------------------------------*/
LOCAL signed int ds707_crm_orig_val = -1;

const byte mintab[] =
  { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

/*===========================================================================
FUNCTION      DS707_PKT_MGR_HANDLE_HDRPAC_EVENTS

DESCRIPTION   Handle events from hdrpac and route them to respective
             callbacks.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void ds707_pkt_mgr_handle_hdrpac_events
(
  ps_iface_event_enum_type evt,
  void *event_data_ptr
);
#endif /* FEATURE_EIDLE_SCI */

/*---------------------------------------------------------------------------
  This information is used in dormant re-origination state. So before
  originating a call, we want to  to know service option of the call before
  it went dormant.
---------------------------------------------------------------------------*/
static uint16 ds707_pkt_prev_data_call_so = CAI_SO_NULL;
static byte   ds707_pkt_prev_sr_id;

call_thr_info_hdlr_cb *call_thr_info_hdlr = NULL;
/*---------------------------------------------------------------------------
  If this FLAG is true it means that we may need to initiate a PPP Resync
---------------------------------------------------------------------------*/
boolean ds707_pkt_mgr_ppp_resync_flag;

/*--------------------------------------------------------------------------- 
  Whether currently the phone has acquried signal or not
---------------------------------------------------------------------------*/
static boolean ds707_has_phone_acquired_signal = FALSE;

/*---------------------------------------------------------------------------
 Pool of Phys links. Note that it to be an array of phys links since
 the entire array has to be passed to the ps_iface when ps_iface is 
 initialized
---------------------------------------------------------------------------*/
static ps_phys_link_type ds707_ps_phys_links[DS707_NUM_PHYS_LINKS];

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
     
  Pool of TC State, one for each RLP   
---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_QOS
static ds707_tc_state_type      ds707_tc_state[DS707_NUM_PHYS_LINKS];
#else
static ds707_tc_state_type      ds707_tc_state[1];
#endif

/*---------------------------------------------------------------------------
  Reference count of primary physical link
---------------------------------------------------------------------------*/
static int ds707_pri_physlink_refcount = 0;

/*---------------------------------------------------------------------------
  Flag to indicate if throttle info is requested therough QMI WDS.
---------------------------------------------------------------------------*/
static boolean ds707_throttle_info_requested_flag = FALSE;

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_USE_QNC

DESCRIPTION   Returns TRUE if QNC is enabled and should be used.

DEPENDENCIES  None.

RETURN VALUE  TRUE - use QNC SO's.  FALSE - use regular PKT SO's.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkti_use_qnc
(
  void
)
{
  word    p_rev_index;                  /* holds current sys's p_rev index */
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  p_rev_index = ds707_p_rev_get_index();
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  if ( (ds_1x_runtime_profile_ptr->ds707.ds707_qcqnc_val == TRUE)&&
       ( ( (p_rev_index == DS707_P_REV_3)               &&
           (ds707_qcmdr_val <= DS_MDR_MODE_SO33_PREF)
         )                                                ||
         ( (ds707_qcmdr_val == DS_MDR_MODE_NO_MDR)     &&
           (p_rev_index >= DS707_P_REV_5)
         )
       )
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* ds707_pkti_use_qnc() */

/*===========================================================================
                   FUNCTIONS INTERNAL TO DS707_PKT_MGR
===========================================================================*/

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
)
{
  boolean ret_val = FALSE;

  DATA_MDM_ARG_NOT_CONST(phys_link_ptr);

  switch (action)
  {
    case DS707_PKT_DORM_ACTION_OK_TO_ORIG:
      switch (reason)
      {
        case DS707_PKT_DORM_REASON_DATA:
        case DS707_PKT_DORM_REASON_NETWORK:
          if ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_DOWN) ||
              (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == 
                                                    PHYS_LINK_GOING_DOWN))
          {
            ret_val = TRUE;
          }
          break;

        case DS707_PKT_DORM_REASON_PZ_S_NID:
          if ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_DOWN) ||
              (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == 
                                                    PHYS_LINK_GOING_DOWN))
          {
            ret_val = TRUE;
          }
          break;

        default:
          DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Unknown dorm reason %d", reason);
          DS_FORCE_ASSERT();
          break;
      } /* switch (reason) */
      break;

    case DS707_PKT_DORM_ACTION_OK_TO_GO_DORM:
      if ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) != PHYS_LINK_NULL) &&
          (PS_PHYS_LINK_GET_STATE(phys_link_ptr) != PHYS_LINK_GOING_NULL) &&
          (PS_PHYS_LINK_GET_STATE(phys_link_ptr) != PHYS_LINK_COMING_UP))
      {
        ret_val = TRUE;
      }
      break;

    default:
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,"Unknown dorm action %d", action);
      DS_FORCE_ASSERT();
      break;
  } /* switch (action) */

  return(ret_val);

} /* ds707_pkti_is_dorm_action_allowed() */

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
)
{
  DATA_MDM_ARG_NOT_CONST(tc_state_ptr);
  DATA_MDM_ARG_NOT_CONST(phys_link_ptr);

  if ((ds707_pkti_is_dorm_action_allowed(
                                        phys_link_ptr,
                                        DS707_PKT_DORM_ACTION_OK_TO_GO_DORM,
                                        DS707_PKT_DORM_REASON_MIN 
                                        ) == TRUE)     &&
#ifdef FEATURE_DATA_IS707_ASYNC                                        
      (ds707_so_async_so_is_async(tc_state_ptr->so) == FALSE)   &&
#endif /* FEATURE_DATA_IS707_ASYNC */      
      (ds3g_is_dorm_allowed_in_pkt_mode(SYS_SYS_MODE_CDMA) == TRUE)        
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_pkti_can_go_dormant() */


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
)
{
  /*------------------------------------------------------------------------- 
    For RATs other than LTE, we would have torn down 1x/HDR context on sys
    change. But for LTE, we keep eHRPD context to support handoff. This may
    lead to some unintentional data coming on eHRPD IFACEs and will end up
    bringing up 1x/HDR CM call. This will abort the ongoing LTE data call
    in CM if LTE attach is in progress. To avoid this race condition we will
    not originate in 1x/HDR. 
   
    OptHo Note: In OptHo case, there'll be a tunnel mode eHRPD call while on
    LTE. This eHRPD call can go dormant and can reorig from dormancy when
    data is enqueued into the wmk. Hence the below chk is enhanced to return
    false only if the S101 SM is not in tunnel call allowed state
  -------------------------------------------------------------------------*/
#ifdef FEATURE_EPC_HANDOFF 
  if (ds3g_is_pref_pkt_mode(SYS_SYS_MODE_LTE)
#ifdef FEATURE_DATA_OPTHO
      && (DS707_S101_SM_TUNNEL_CALL_ALLOWED != ds707_s101_query_sm_state())
#endif /* FEATURE_DATA_OPTHO */
      )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "DS3G system is LTE, Can't originate 1x/HDR call ");
#ifdef FEATURE_DATA_OPTHO
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "and S101 not in tunnel call allowed state");
#endif /* FEATURE_DATA_OPTHO */
    return FALSE;
  }
#endif

#ifdef FEATURE_EHRPD
  /*--------------------------------------------------------------- 
    Sys Chg SM is in a transition state due to some IRAT. Don't
    allow any reorig until the sys chg SM moves back to NULL
  ---------------------------------------------------------------*/
  if (DS707_SYS_CHG_SM_NULL != ds707_query_sys_chg_sm_state())
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                  "ds707_pkti_ok_orig_from_dorm: Sys Chg SM not in NULL state,"
                  " Can't re-orig 1x/HDR call");
    return FALSE;
  }
#endif /* FEATURE_EHRPD */


  if (ds707_pkti_is_dorm_action_allowed(
                                       phys_link_ptr,
                                       DS707_PKT_DORM_ACTION_OK_TO_ORIG,
                                       reason 
                                       ) == TRUE
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_pkti_ok_orig_from_dorm() */


/*===========================================================================
FUNCTION      DS707_PKTI_GET_PKT_STATE_INDEX_CID

DESCRIPTION   Given a CM call id, will get the pkt state index.

DEPENDENCIES  None

RETURN VALUE  If found, will return the index.  If not found, then will
              return DS707_MAX_PDN_INSTANCES.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_pkti_get_pkt_state_index_cid
(
 cm_call_id_type       call_id
)
{
  ds707_tc_state_type   *tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    tc_state_ptr = ds707_get_pri_tc_ptr();
    if (tc_state_ptr->call_id == call_id)
    {
      return 0;
    }
    else
    {
      return(DS707_MAX_PDN_INSTANCES);
    }  
} /* ds707_pkti_get_pkt_state_index_cid() */

/*===========================================================================
FUNCTION      DS707_PKTI_GET_ORIG_SO

DESCRIPTION   Returns the SO to originate with.

DEPENDENCIES  None

RETURN VALUE  Service option to originate with.

SIDE EFFECTS  None.
===========================================================================*/
ds707_so_type ds707_pkti_get_orig_so(void)
{
  ds707_so_type                orig_so;          /* SO to orig with        */
  ds707_rateset_type           rateset;          /* originating rateset    */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_DS_IS707A_CMUX
  rateset = (ds707_rateset_type)
            (dsat707_cmux_rev_table[(ds707_cmux_val[1])] - 1);
#else
  rateset = (ds707_rateset_type)(ds707_cmux_val - 1);
#endif

#ifdef FEATURE_DATA_IS707_ASYNC
  if (ds707_pkti_use_qnc() == TRUE)
  {
    orig_so = ds707_so_async_get_orig(rateset);
  }
  else
#endif /* FEATURE_DATA_IS707_ASYNC */  	
  {
    orig_so = ds707_so_pkt_get_orig(rateset);
  }

  return(orig_so);
}

/*===========================================================================
                   FUNCTIONS EXTERNAL TO DS707_PKT_MGR
===========================================================================*/
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
)
{
  DATA_MDM_ARG_NOT_CONST(phys_link_ptr);

  if (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_DOWN)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_pkt_is_dormant() */

/*===========================================================================
FUNCTION      DS707_PKT_IS_UM_IFACE_UP

DESCRIPTION   Returns TRUE if iface is UP, ROUTEABLE, LINGERING.

DEPENDENCIES  phys_link should be up or coming up.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_um_iface_up
(
 ps_iface_type    *ps_iface_ptr
)
{
  ps_iface_state_enum_type    iface_state;

  if (ps_iface_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "NULL iface pointer passed in");
    return(FALSE);
  }
  iface_state = ps_iface_state(ps_iface_ptr);

  /*--------------------------------------------------------------------
    The checks for IFACE_UP is for sockets
  --------------------------------------------------------------------*/
  
  if ((iface_state == IFACE_UP) || 
      (iface_state == IFACE_ROUTEABLE) ||
      (iface_state == IFACE_LINGERING) 
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}


/*===========================================================================
FUNCTION      DS_VERIFY_SO

DESCRIPTION   Called by MC.  Asks DS if the originating SO is correct.

DEPENDENCIES  Called in task other than DS.  Necessary, as this is called
              even when doing an async/fax call.  Do not want to override
              those SO's.  We only initiate on phys_link_up, which sets
              pkt iface phys state = COMING_UP.  phys_link state set after
              phys_link_cmd called, which all happens under TASKLOCK.  So
              if called from other task, then ensured that phsy_link ==
              COMING_UP (and may not still be DOWN but soon will be
              COMING_UP).

RETURN VALUE  service option to orig with.

SIDE EFFECTS  None.
===========================================================================*/
word ds_verify_so
(
  word so_to_verify           /* SO to verify                              */
)
{
  ds707_so_type           cdma_so;       /* Service option for CDMA origs  */
  ps_phys_link_type      *phys_link_ptr; /* ptr to 1x iface                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if (PS_PHYS_LINK_GET_STATE(phys_link_ptr) != PHYS_LINK_COMING_UP)
  {
    /*-----------------------------------------------------------------------
      Called from something other than pkt call.
    -----------------------------------------------------------------------*/
    return(so_to_verify);
  }

  cdma_so = ds707_pkti_get_orig_so();

  if (cdma_so != so_to_verify)
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH, "Pkt SO changed %d -> %d",
                     so_to_verify, cdma_so);
  }
  return((word)cdma_so);
} /* ds_verify_so() */


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
)
{
  ps_phys_link_type          *phys_link_ptr;
  ds707_pkt_data_sess_state_type data_sess = DS707_PKT_DATA_SESS_NONE;
  sys_sys_mode_e_type        curr_data_session_network = SYS_SYS_MODE_NO_SRV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  curr_data_session_network = ds707_pkt_get_current_data_sess_network();

  if (ds707_pkt_is_dormant(phys_link_ptr) == TRUE)
  {
    if (curr_data_session_network == SYS_SYS_MODE_CDMA)
    {
      data_sess = DS707_PKT_DATA_SESS_CDMA_DORMANT;
    }
    else if (curr_data_session_network == SYS_SYS_MODE_HDR)
    {
      data_sess = DS707_PKT_DATA_SESS_HDR_DORMANT;
    }
  }
  else
  {
    if (curr_data_session_network == SYS_SYS_MODE_CDMA)
    {
      data_sess = DS707_PKT_DATA_SESS_CDMA_ACTIVE;
    }
    else if (curr_data_session_network == SYS_SYS_MODE_HDR)
    {
      data_sess = DS707_PKT_DATA_SESS_HDR_ACTIVE;
    }
  }

  return data_sess;
} /* ds707_get_pkt_data_sess_state() */


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
)
{
  return ds707_current_data_sess_network;
} /* ds707_pkt_get_current_data_sess_network() */

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
)
{
  ds707_current_data_sess_network = curr_data_session_network;
} /* ds707_pkt_get_current_data_sess_network() */


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
)
{
  return ds707_last_data_sess_network;
} /* ds707_pkt_get_last_data_sess_network() */

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
)
{
  ds707_last_data_sess_network = data_sess_network;
} /* ds707_pkt_set_last_data_sess_network() */

/*===========================================================================
FUNCTION      DS707_PKT_GET_PHYS_LINK_PTR

DESCRIPTION   Given a packet state instance, returns the pointer to the
              phys link.

DEPENDENCIES  None.

RETURN VALUE  pointer to the primary phys link

SIDE EFFECTS  None.
===========================================================================*/
ps_phys_link_type* ds707_pkt_get_pri_phys_link_ptr( void )
{
  return(&ds707_ps_phys_links[0]);
} 

/*===========================================================================
FUNCTION      DS707_GET_PHYS_LINK_PTR
  
DESCRIPTION   Given the pkt instance, and the index to the tc_state, 
              returns the corresponding phys link_ptr. This function
              retrieves secondary phys links.

DEPENDENCIES  None.

RETURN VALUE  Pointer to phys link.

SIDE EFFECTS  None.
===========================================================================*/
ps_phys_link_type* ds707_get_phys_link_ptr(uint32 i)
{
  return (&ds707_ps_phys_links[i]);
}

/*===========================================================================
FUNCTION      DS707_GET_PRI_TC_PTR

DESCRIPTION   Given the pkt instance, returns the tc_state_ptr.

DEPENDENCIES  None.

RETURN VALUE  ds707_tc_state_type*

SIDE EFFECTS  None.
===========================================================================*/
ds707_tc_state_type* ds707_get_pri_tc_ptr(void)
{
  return (&(ds707_tc_state[0]));
}

/*===========================================================================
FUNCTION      DS707_GET_SEC_TC_PTR

DESCRIPTION   Given the pkt instance and index, returns a pointer to the
              tc_state_ptr.

DEPENDENCIES  None.
  
RETURN VALUE  ds707_tc_state_type*

SIDE EFFECTS  None.
===========================================================================*/
ds707_tc_state_type* ds707_get_sec_tc_ptr(uint32 index)
{
  return (&(ds707_tc_state[index]));
}


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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_dorm_timer_enable(is_enabled);
} /*  ds707_pkt_enable_holddown_timer() */


/*===========================================================================
FUNCTION      DS707_PKT_GET_PREV_CALL_SO

DESCRIPTION   This function will return SO of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
word ds707_pkt_get_prev_call_so (void)
{
  return( ds707_pkt_prev_data_call_so );
} /* ds707_pkt_get_prev_call_so */

/*===========================================================================
FUNCTION      DS707_PKT_SET_PREV_CALL_SO

DESCRIPTION   This function will set value of SO of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_set_prev_call_so (uint16 prev_call_so)
{
  ds707_pkt_prev_data_call_so = prev_call_so;
} /* ds707_pkt_set_prev_call_so */


/*===========================================================================
FUNCTION      DS707_PKT_GET_PREV_SR_ID

DESCRIPTION   This function will return SR ID of previous call.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
byte ds707_pkt_get_prev_sr_id (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return( ds707_pkt_prev_sr_id );
} /* ds707_pkt_get_prev_sr_id() */

/*===========================================================================
FUNCTION      DS707_PKT_SET_PREV_SR_ID

DESCRIPTION   This function sets SR ID of previous call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_set_prev_sr_id ( byte prev_sr_id )
{
  ds707_pkt_prev_sr_id = prev_sr_id;
}



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
)
{
  ds707_tc_state_type *tc_state_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  tc_state_ptr = ds707_get_pri_tc_ptr();

  switch ( rlp_ver )
  {
    case DSRLP_VER_1X_3:
    case DSRLP_VER_HDR:
      if (( tc_state_ptr->fwd_rlp.flow.ver != DSRLP_VER_INVALID ) ||
          ( tc_state_ptr->rev_rlp.flow.ver != DSRLP_VER_INVALID ))
      {
        DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                         "Primary TCs old RLP is %d ver: %d not invalid",
                         tc_state_ptr->fwd_rlp.flow.flow,
                         tc_state_ptr->fwd_rlp.flow.ver);
      }
      tc_state_ptr->fwd_rlp.flow.flow = rlp_flow;
      tc_state_ptr->fwd_rlp.flow.ver  = rlp_ver;
      tc_state_ptr->rev_rlp.flow.flow = rlp_flow;
      tc_state_ptr->rev_rlp.flow.ver  = rlp_ver;
      break;

    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
      if ( tc_state_ptr->fwd_rlp.flow.ver != DSRLP_VER_INVALID )
      {
        DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                         "Primary TCs old RLP is %d ver: %d not invalid",
                         tc_state_ptr->fwd_rlp.flow.flow,
                         tc_state_ptr->fwd_rlp.flow.ver);
      }
      tc_state_ptr->fwd_rlp.flow.flow = rlp_flow;
      tc_state_ptr->fwd_rlp.flow.ver  = rlp_ver;
      break;

    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      if ( tc_state_ptr->rev_rlp.flow.ver != DSRLP_VER_INVALID )
      {
        DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                         "Primary TCs old RLP is %d ver: %d not invalid",
                         tc_state_ptr->rev_rlp.flow.flow,
                         tc_state_ptr->rev_rlp.flow.ver);
      }
      tc_state_ptr->rev_rlp.flow.flow = rlp_flow;
      tc_state_ptr->rev_rlp.flow.ver  = rlp_ver;
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "RLP ver: %d unrecognized", rlp_ver);
      return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "Allocated Primary TC to RLP %d ver %d", 
                   rlp_flow, 
                   rlp_ver);

} /* ds707_pkt_set_pri_tc_rlp() */



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
)
{
  ds707_tc_state_type *tc_state_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  tc_state_ptr = ds707_get_pri_tc_ptr();

  tc_state_ptr->fwd_rlp.flow.ver  = DSRLP_VER_INVALID;
  tc_state_ptr->rev_rlp.flow.ver  = DSRLP_VER_INVALID;
  DATA_IS707_MSG0(MSG_LEGACY_MED, "Primary TC deallocated");

} /* ds707_pkt_clean_pri_tc_rlp() */

/*===========================================================================
FUNCTION      DS707_GET_PPP_RESYNC_FLAG_PTR

DESCRIPTION   Returns the pointer to flag that tells whether reconnected 
              due a change in SID/NID/PZID

DEPENDENCIES  None.

RETURN VALUE  Boolean*

SIDE EFFECTS  None.
===========================================================================*/
boolean* ds707_get_ppp_resync_flag_ptr(void)
{
  return (&(ds707_pkt_mgr_ppp_resync_flag));
}

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
)
{
  void*                      dssnet_instance = NULL;
  boolean                    reg_status      = FALSE;
  uint8                      curr_pdn_index = 0;
  ps_iface_type*             iface_ptr;
  ds707_iface_table_type*    iface_tbl_ptr;
  ds707_pdn_context_cb_type* pdncntx_cb_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG0( MSG_LEGACY_MED, 
                   "Acquiring PPP resync lock");

  for (curr_pdn_index = 0;
       curr_pdn_index < DS707_MAX_PDN_INSTANCES;
       curr_pdn_index++ )
  {
    pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_instance(curr_pdn_index);
    if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
    {
      /*get V4 iface*/
      iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
      if (PS_IFACE_IS_VALID(iface_ptr))
      {
        /*get iface table entry*/
        iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);
        dssnet_instance = 
          (void*)ds707_pdncntx_get_dssnet4_sm_ptr(iface_tbl_ptr);
        /*-------------------------------------------------------------------
          Invoke the DSSNET API to register the lock release call back  
          function. The lock release call back function is called after the
          completion ofset the PPP resync flag to TRUE for this iface if 
          registration succeeded
        -------------------------------------------------------------------*/
        reg_status = dssnet4_reg_ppp_resync_done_cb(dssnet_instance, 
                                          ds707_pkt_release_ppp_resync_lock);
        if (reg_status == TRUE)
        {
          ds707_pdncntx_set_ppp_resync_lock(pdncntx_cb_ptr,TRUE);
        }
      }
#ifdef FEATURE_DATA_PS_IPV6
      else
      {
        /*get V6 iface*/
        iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
        if (PS_IFACE_IS_VALID(iface_ptr))
        {
          /*get iface table entry*/
          iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);
          dssnet_instance = 
            (void*)ds707_pdncntx_get_dssnet6_sm_ptr(iface_tbl_ptr);
          /*-------------------------------------------------------------------
            Invoke the DSSNET API to register the lock release call back  
            function. The lock release call back function is called after the
            completion ofset the PPP resync flag to TRUE for this iface if 
            registration succeeded
          -------------------------------------------------------------------*/
          reg_status = dssnet6_reg_ppp_resync_done_cb(dssnet_instance, 
                                            ds707_pkt_release_ppp_resync_lock);
          if (reg_status == TRUE)
          {
            ds707_pdncntx_set_ppp_resync_lock(pdncntx_cb_ptr,TRUE);
          }
        }
      }
#endif /*FEATURE_DATA_PS_IPV6*/
    }/*if PDN in use*/
  }/*for all in use PDNs*/
}

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
)
{
  /*-------------------------------------------------------------------------
    If the PPP resync lock has not been acquired return FALSE
  -------------------------------------------------------------------------*/
  boolean                    ret_val = FALSE;
  uint8                      curr_pdn_index = 0;
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Check PPP resync lock: %x", 
                   ppp_resync_lock_release_cb);
  /*-------------------------------------------------------------------------
    If the ppp resync lock has been acquired for the any iface, register
    the resync lock release call back and return TRUE to the module checking
    the status of PPP resync lock
  -------------------------------------------------------------------------*/
  for ( curr_pdn_index = 0;
        curr_pdn_index < DS707_MAX_PDN_INSTANCES;
        curr_pdn_index++ )
  {
    pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_instance(curr_pdn_index);
    if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
    { 
      if ( ds707_pdncntx_get_ppp_resync_lock(pdncntx_cb_ptr) == TRUE )
      {
        ds707_pkt_mgr_ppp_resync_lock_release_cb = 
                                                ppp_resync_lock_release_cb;
        ret_val = TRUE;
        break;
      }
    }/*IF PDN IN USE*/  
  }/*FOR ALL PDNS*/  

  return ret_val;
}

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
)
{
  boolean*                   ppp_resync_ptr;
  uint8                      curr_iface_index = 0;
  ps_iface_type*             iface_ptr;
  boolean                    call_ppp_resync_lock_cb = TRUE;
  ds707_iface_table_type*    ds707_iface_tbl_ptr = NULL;
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(dssnet_instance != NULL);
  /*lint -save -e774 Reason for suppressing error 774*/

  if (dssnet_instance == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid dssnet instance ptr");
    return;
  }
  /*lint -restore Restore lint error 774*/

  DATA_IS707_MSG1( MSG_LEGACY_MED, "Release PPP resync lock for dssnet: %x",
                   dssnet_instance);

  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {
    ds707_iface_tbl_ptr = ds707_get_iface_tbl_from_instance(curr_iface_index);
    iface_ptr = 
      ds707_pdncntx_get_iface_ptr_from_iface_tbl_index((uint8)curr_iface_index);
    if ( PS_IFACE_IS_VALID (iface_ptr) &&
         PS_IFACE_IS_IN_USE (iface_ptr) )
    {     
      if ( ( dssnet_instance == 
             (void*)ds707_pdncntx_get_dssnet4_sm_ptr(ds707_iface_tbl_ptr)) 
#ifdef FEATURE_DATA_PS_IPV6         
           ||
           ( dssnet_instance == 
             (void*)ds707_pdncntx_get_dssnet6_sm_ptr(ds707_iface_tbl_ptr)) 
#endif /* FEATURE_DATA_PS_IPV6 */         
           )
      {     
        /*get the pdn cntx cb for this iface table entry*/
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr(ds707_iface_tbl_ptr);     
        ds707_pdncntx_set_ppp_resync_lock(pdncntx_cb_ptr, FALSE);
        break;
      }
    }
  }  

  /*-------------------------------------------------------------------------
    If the PPP resync lock has been released for all the IFACES, call
    the ppp_resync_lock_release callback and de-register it
  -------------------------------------------------------------------------*/
  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_PDN_INSTANCES;
        curr_iface_index++ )
  {
    pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_instance(curr_iface_index);       
    if (ds707_pdncntx_get_ppp_resync_lock(pdncntx_cb_ptr) != FALSE)
    {     
      call_ppp_resync_lock_cb = FALSE;     
      break;
    }
  }  

  if ( call_ppp_resync_lock_cb == TRUE )
  {
    if (ds707_pkt_mgr_ppp_resync_lock_release_cb != NULL)
    {
      ds707_pkt_mgr_ppp_resync_lock_release_cb();
      ds707_pkt_mgr_ppp_resync_lock_release_cb = NULL;
    }

    /*-----------------------------------------------------------------------
      While Releasing the PPP resync lock and notifying the registered client
      about the completion of PPP resync, RESET the ppp_resync_flag to 
      prevent further attempts to do a PPP resync. This is only for 
      Sockets/MIP scenarios For Laptop SIP, resetting of this flag should
      happen elsewhere. 
    -----------------------------------------------------------------------*/
    ppp_resync_ptr  = ds707_get_ppp_resync_flag_ptr();
    *ppp_resync_ptr = FALSE;
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, "PPP resync flag set to %d", 
                     *ppp_resync_ptr);
  }
}

/*===========================================================================
FUNCTION      DS707_PKT_IS_HANDOFF_FROM_1X_TO_HDR

DESCRIPTION   
              
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_handoff_from_1x_to_hdr
(
  sys_sys_mode_e_type old_nw, 
  sys_sys_mode_e_type new_nw
)
{
  if ((old_nw == SYS_SYS_MODE_CDMA) && (new_nw == SYS_SYS_MODE_HDR))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION      DS707_PKT_IS_HANDOFF_FROM_HDR_TO_1X

DESCRIPTION   
              
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_is_handoff_from_hdr_to_1x
(
  sys_sys_mode_e_type old_nw, 
  sys_sys_mode_e_type new_nw
)
{
  if ((old_nw ==  SYS_SYS_MODE_HDR) && (new_nw ==  SYS_SYS_MODE_CDMA))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_PS_TO_DS_FLOW_MAP

DESCRIPTION   Sets up the client_data_pointer of the PS flow to point to the
              specfied transmit watermark

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_mgr_set_ps_to_ds_flow_map
(
  ps_flow_type  * ps_flow_ptr,
  ds707_flow_type *ds_flow_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ps_flow_ptr);
  ASSERT(ds_flow_ptr);

  ps_flow_ptr->client_data_ptr = (void *) ds_flow_ptr;
  ds_flow_ptr->ps_flow_ptr     = ps_flow_ptr;

}

/*===========================================================================
FUNCTION      DS707_PKT_FLOW_GET_DS_FLOW

DESCRIPTION   This function returns a ds flow pointer for a given PS flow
              If the ds flow is not found, NULL is returned

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
ds707_flow_type * ds707_pkt_mgr_get_ds_flow
(
  ps_flow_type  * ps_flow_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_flow_ptr == NULL)
  {
    return NULL;
  }
  /*lint -save -e731 Reason for suppressing error 731*/
  if ( PS_FLOW_IS_VALID( ps_flow_ptr) == FALSE )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid flow_ptr 0x%x", ps_flow_ptr);
    return NULL;
  }

  if ( ps_flow_ptr->client_data_ptr == NULL )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Invalid client_data_ptr for ps_flow_ptr 0x%x",
                    ps_flow_ptr);
    return NULL;
  }
  /*lint -restore Restore lint error 731*/
  return((ds707_flow_type *)ps_flow_ptr->client_data_ptr);
}

#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION      DSRLP_REG_SRVC_EX

DESCRIPTION   In turn calls dsrlp_reg_srvc() for each route. Just a wrapper.
              This function cannot be placed in ds707_sec_pkt_mgr_util.c
              because even DPA uses the new architecture. But its good to 
              featurize under FEATURE_HDR.

DEPENDENCIES  A valid sr_id value must be passed into this function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_reg_all_routes_for_mrlp
(
  byte                rscb_index,            /* index for a session id     */
  dsrlp_ver_enum_type ver,                   /* RLP version                */
  dsm_watermark_type  *tx_watermark_ptr,     /* ptr to Tx wm for txmit frs */
  void                (*post_rx_func_ptr)(uint32),
                                             /* rx callback for rx frames  */
  dsm_watermark_type   post_rx_wm_list[],    /* watermarks for rx'ed frames*/
  uint32               post_rx_data           /* arg to post_rx_func_ptr() */
)
{
    byte p;

    /* Loop through and call dsrlp_reg_srvc for each individual route */
    for (p = DSRSP_NUM_ROUTES; p > 0; p--) 
    {
        dsrlp_reg_srvc(rscb_index, ver, 
                       tx_watermark_ptr, 
                       post_rx_func_ptr, 
                       post_rx_wm_list ? &post_rx_wm_list[p - 1] : NULL, 
                       p - 1 , post_rx_data);
        DATA_IS707_MSG3(MSG_LEGACY_HIGH, 
                        "RLP reg rscb_index: %d route: %d post_rx_wm_list 0x%x", 
                        rscb_index, p - 1, &post_rx_wm_list[p - 1]);
    }
} /* dsrlp_reg_srvc_ex */
#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION      DS707_PKT_IFACE_GENERIC_IND

DESCRIPTION   Calls ps_iface_generic_ind() for both ifaces

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_iface_generic_ind
(
  ps_iface_event_enum_type             event,
  void                                *event_data_ptr
)
{
  ps_iface_type *       iface_ptr;
  uint8                 curr_iface_index = 0;
  /* ---------------------------------------------------------------------
       Post indication for both IPv4 and IPv6 
  ------------------------------------------------------------------------*/
  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES; 
        curr_iface_index++ )
  {      
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    if  ( PS_IFACE_IS_VALID(iface_ptr) &&
  	  ( ps_iface_state(iface_ptr) == IFACE_UP ||
            ps_iface_state(iface_ptr) == IFACE_ROUTEABLE ||
            ps_iface_state(iface_ptr) == IFACE_CONFIGURING ) )
    {
      ps_iface_generic_ind(iface_ptr,
                           event, 
                           event_data_ptr);
      DATA_IS707_MSG3( MSG_LEGACY_MED,
                       "event %d for iface 0x%x::%d", 
                       event, iface_ptr->name, iface_ptr->instance);
    }
  }
} /* ds707_pkt_iface_generic_ind() */

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
  void                                *event_data_ptr,
  ps_sys_subscription_enum_type     subs_id
)
{
  int16                 ps_errno;

  ps_sys_event_ind_ex( PS_SYS_TECH_3GPP2,
                       event ,
                       subs_id ,
                       event_data_ptr,
                       &ps_errno);
  DATA_IS707_MSG1( MSG_LEGACY_MED,"event %d ",event);
} /* ds707_pkt_sys_event_ind() */

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
void * ds707_pkt_mgr_setup_pkt_call_info(ps_iface_type *iface_ptr)
{
  if (!PS_IFACE_IS_VALID(iface_ptr)) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "Invalid iface ptr passed to setup_pkt_call_info");
    return(NULL);
  }
  else
  {
    /*-----------------------------------------------------------------------
      If the oldest_active_iface_ptr is NULL then set it with the iface ptr 
      that is passed.
    -----------------------------------------------------------------------*/
    if (ds707_data_session_get_oldest_active_iface_ptr() == NULL) 
    {
      ds707_data_session_set_oldest_active_iface_ptr(iface_ptr);
      DATA_IS707_MSG0(MSG_LEGACY_MED, "oldest_active_iface_ptr set");
    }
    /*-----------------------------------------------------------------------
      pass valid value as user info to phys_link_up_cmd. Create a struct of
      type ds3g_pkt_call_info_type and pass it as user_info.
      set the iface value in to to the current iface.
      and also set following:
      cmd_ptr->cmd.ds707_data.drs_bit = TRUE;
      pkt_call_info.iface_ptr = this_iface_ptr;
      pkt_call_info.drs_bit = TRUE;
      pkt_call_info.use_hdown_timer = TRUE;
      pkt_call_info.new_network = ds707_current_data_sess_network;
      pkt_call_info.force_cdma_only = FALSE;
      pkt_call_info.inc_call = FALSE;
   ------------------------------------------------------------------------*/
    pkt_call_info.iface_ptr = iface_ptr;
    pkt_call_info.drs_bit = TRUE;
    pkt_call_info.use_hdown_timer = TRUE;
    pkt_call_info.new_network = ds707_pkt_get_current_data_sess_network();
    pkt_call_info.force_cdma_only = FALSE;
    pkt_call_info.inc_call = FALSE;
    pkt_call_info.epzid = FALSE;
    return (&pkt_call_info);
   }
} /* ds707_pkt_mgr_setup_pkt_call_info */
/*===========================================================================
FUNCTION      DS707_PHYS_LINK_ENABLE_FLOW

DESCRIPTION   This utility function enables all the flows bound to the physlink

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  All the physlinks are enabled.
===========================================================================*/
void ds707_phys_link_enable_flow(uint32 flow_mask)
{
  ps_phys_link_type         *phys_link_ptr;
  ds707_tc_state_type       *tc_state_ptr;
  uint8                      i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  ps_phys_link_enable_flow(phys_link_ptr,flow_mask);
#ifdef FEATURE_HDR_QOS
  for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
  {
    tc_state_ptr = ds707_get_sec_tc_ptr(i);
    phys_link_ptr = tc_state_ptr->ps_phys_link_ptr;
    ASSERT (phys_link_ptr != NULL);
    /* Post enable flow to sec phys link only if some
       flow is associated with it */
    if (PS_PHYS_LINK_GET_REF_CNT(phys_link_ptr) > 0)
    {
      ps_phys_link_enable_flow(phys_link_ptr, flow_mask);
    }
  }       
#endif /* FEATURE_HDR_QOS */
}/*ds707_phys_link_enable_flow() */

/*===========================================================================
FUNCTION      DS707_PHYS_LINK_DISABLE_FLOW

DESCRIPTION   This utility function disables all the flows bound to the physlink

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  All the phys links are disabled.
===========================================================================*/
void ds707_phys_link_disable_flow(uint32 flow_mask)
{
  ps_phys_link_type         *phys_link_ptr;
  ds707_tc_state_type       *tc_state_ptr;
  uint8                      i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  ps_phys_link_disable_flow(phys_link_ptr,flow_mask);
#ifdef FEATURE_HDR_QOS
  for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
  {
    tc_state_ptr = ds707_get_sec_tc_ptr(i);
    phys_link_ptr = tc_state_ptr->ps_phys_link_ptr;
    ASSERT (phys_link_ptr != NULL);

    /* Post disable flow to sec phys link only if some
       flow is associated with it */
    if (PS_PHYS_LINK_GET_REF_CNT(phys_link_ptr) > 0)
    {
      ps_phys_link_disable_flow(phys_link_ptr, flow_mask);
    }
  }       
#endif /* FEATURE_HDR_QOS */
}/*ds707_phys_link_disable_flow*/

/*===========================================================================
FUNCTION      ds707_pkt_mgr_check_coex_data_status

DESCRIPTION   This utility function checks the current coex status from DSGMGR
                        And then disables all the flows bound to the physlink if there is coex 
                        data issue.  Otherwise it just calls phys link enable 

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  All the phys links are disabled.
===========================================================================*/

void ds707_pkt_mgr_check_coex_data_status(ps_iface_type *iface_ptr)
{

  boolean coex_flow_status = FALSE;
  sys_modem_as_id_e_type        resolve_subsid;

  if( iface_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Ignore due to invalid iface ptr");
    return;
  }
  if( TRUE == ds3g_resolve_subs_id(
                     PS_IFACE_GET_SUBS_ID(iface_ptr),
                     &resolve_subsid)
    )
  {
    coex_flow_status = 
             ds3gflowmgr_get_coex_data_flow_status(resolve_subsid);

    if(coex_flow_status == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Disable the Phys link with COEX_MASK");
     /*-----------------------------------------------------------------------
         Send PS phys link enable for all bearers for the corresponding subs id
    -   ----------------------------------------------------------------------*/
      ds707_phys_link_disable_flow( DS_FLOW_COEX_MASK );
    } 
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Enable the Phys link with COEX_MASK");
      /*-----------------------------------------------------------------------
        Send PS phys link enable for all bearers for the corresponding subs id
      -----------------------------------------------------------------------*/
      ds707_phys_link_enable_flow( DS_FLOW_COEX_MASK );
    } 
  }
}


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
)
{
  dssnet4_sm_cb_type         *ret_val_ptr = NULL;
  ds707_iface_table_type     *ds707_iface_tbl_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (PS_IFACE_IS_VALID(ps_iface_ptr))
  {
  if ( ps_iface_addr_family_is_v4(ps_iface_ptr) ) 
  {
      ds707_iface_tbl_ptr = 
        ds707_pdncntx_get_iface_tbl_entry_from_iface(ps_iface_ptr);
      ret_val_ptr = 
        ds707_pdncntx_get_dssnet4_sm_ptr(ds707_iface_tbl_ptr);
    }  
  }
  return ret_val_ptr;
}  /* ds707_pkt_mgr_get_dssnet4_sm_cb() */

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
)
{
  dssnet6_sm_cb_type         *ret_val_ptr = NULL;
  ds707_iface_table_type     *ds707_iface_tbl_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    if ( ps_iface_addr_family_is_v6(ps_iface_ptr) ) 
    {
      ds707_iface_tbl_ptr = 
        ds707_pdncntx_get_iface_tbl_entry_from_iface(ps_iface_ptr);
      ret_val_ptr = 
        ds707_pdncntx_get_dssnet6_sm_ptr(ds707_iface_tbl_ptr);
    }  
  }
  return ret_val_ptr;
}  /* ds707_pkt_mgr_get_dssnet6_sm_cb() */
#endif /* FEATURE_DATA_PS_IPV6 */
/*lint -restore Restore lint error 641*/

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

  uint8              max_flow_count,
  /* number of ds flows in the above array.. OUTPUT parameter */
  uint8          * num_ps_flows

  )
{
  ps_flow_type     * ps_flow_ptr;
  void             * ps_flow_handle;
  void             * new_ps_flow_handle;
  boolean            result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (num_ps_flows    == NULL) || 
      (ps_flow_ptr_ptr == NULL) ||
			(this_iface_ptr  == NULL))
  {
    return FALSE;
  }

  *num_ps_flows  = 0;

  ps_flow_handle = ps_iface_get_sec_flow_handle(this_iface_ptr);

  while (ps_flow_handle != NULL)
  {
    result = ps_iface_get_sec_flow_by_handle(this_iface_ptr,
                                             ps_flow_handle,
                                             &ps_flow_ptr,
                                             &new_ps_flow_handle);
    if (!result)
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Returning incomplete list of flows");
      return FALSE;
    }

    if(*num_ps_flows >= max_flow_count || *num_ps_flows >= DS707_MAX_DS_FLOWS )
    {
      return FALSE;
    }

    ps_flow_ptr_ptr[*num_ps_flows] = ps_flow_ptr;
    *num_ps_flows = *num_ps_flows + 1;
    
    ps_flow_handle = new_ps_flow_handle;

  } /* while (ps_flow_handle != NULL) */
  return TRUE;
} /* ds707_sec_pkt_mgr_get_all_secondary_flows */

/*===========================================================================

FUNCTION       DS707_PKT_MGR_IS_DATA_ON_EHRPD

DESCRIPTION    This is utility function and will return if the data call on 
               C2K is eHRPD 
        
DEPENDENCIES   

RETURN VALUE    boolean

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_pkt_mgr_is_data_on_ehrpd(void)
{
  uint8                      curr_pdn_index = 0;
  ps_iface_type*             iface_ptr;
  ds707_iface_table_type*    iface_tbl_ptr;
  ds707_pdn_context_cb_type* pdncntx_cb_ptr;

  dssnet4_sm_cb_type        * dssnet4_instance;
  dssnet6_sm_cb_type        * dssnet6_instance;
  boolean                     ret_val = FALSE; // Not in ehrpd dormant data call
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (curr_pdn_index = 0;
       curr_pdn_index < DS707_MAX_PDN_INSTANCES;
       curr_pdn_index++ )
  {
    pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_instance(curr_pdn_index);
    if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
    {
      /*get V4 iface*/
      iface_ptr = ds707_pdncntx_get_v4_iface(pdncntx_cb_ptr);
      if (PS_IFACE_IS_VALID(iface_ptr))
      {
        /*get iface table entry*/
        iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);
        dssnet4_instance = ds707_pdncntx_get_dssnet4_sm_ptr(iface_tbl_ptr);
        /*-------------------------------------------------------------------
        Check if this call is eHRPD
        -------------------------------------------------------------------*/
        if(dssnet4_instance != NULL &&
           dssnet4_instance->is_ehrpd_call == TRUE)
        {
          /*-----------------------------------------------------------------
          Check if this call is eHRPD
          -----------------------------------------------------------------*/
          ret_val = TRUE;
          break;
        }
      }
#ifdef FEATURE_DATA_PS_IPV6
      else
      {
        /*get V6 iface*/
        iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
        if (PS_IFACE_IS_VALID(iface_ptr))
        {
          /*get iface table entry*/
          iface_tbl_ptr = ds707_pdncntx_get_iface_tbl_entry_from_iface(iface_ptr);
          dssnet6_instance = ds707_pdncntx_get_dssnet6_sm_ptr(iface_tbl_ptr);
          /*-------------------------------------------------------------------
          Check if this call is eHRPD
          -------------------------------------------------------------------*/
          if(dssnet6_instance != NULL &&
             dssnet6_instance->is_ehrpd_call == TRUE)
          {
            /*-----------------------------------------------------------------
            Check if this call is eHRPD
            -----------------------------------------------------------------*/
            ret_val = TRUE;
            break;
          }
        }
      }
#endif /*FEATURE_DATA_PS_IPV6*/
    }/*if PDN in use*/
  }/*for all in use PDNs*/

  if(ret_val == TRUE)
  {
    /*-----------------------------------------------------------------
    This data call was in eHRPD
    -----------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "eHRPD call");
  }
  return ret_val;
}/*ds707_pkt_mgr_is_data_on_ehrpd()*/

/*===========================================================================
FUNCTION       DS707_PKT_MGR_ALL_IFACES_DOWN

DESCRIPTION    This is utility function and will return true if all ifaces 
               are in down state. 
        
DEPENDENCIES   None.

RETURN VALUE   boolean

SIDE EFFECTS   None.
===========================================================================*/
boolean ds707_pkt_mgr_all_ifaces_down(void)
{
  uint8 curr_iface_index = 0;
  ps_iface_type* temp_iface_ptr = NULL;
  boolean ret_val = TRUE;

  for (curr_iface_index = 0;
       curr_iface_index < DS707_MAX_IFACES;
       curr_iface_index++)
  {
    temp_iface_ptr = NULL ;
    temp_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    if( (ps_iface_state(temp_iface_ptr) != IFACE_DOWN) && 
        (ps_iface_state(temp_iface_ptr) != IFACE_STATE_INVALID) &&
        (ps_iface_state(temp_iface_ptr) != IFACE_DISABLED) )
    {
      DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                       "iface:0x%x:%d in %d state",
                       temp_iface_ptr->name,
                       temp_iface_ptr->instance,
                       ps_iface_state(temp_iface_ptr));
      ret_val = FALSE;
      break;
    }
  }

  return ret_val;
} /* ds707_pkt_mgr_all_ifaces_down */

/*===========================================================================
FUNCTION      DS707_GET_HAS_PHONE_ACQUIRED_SIGNAL

DESCRIPTION   Returns boolean stating whether phone has acquired signal

DEPENDENCIES  None.

RETURN VALUE  Boolean

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_get_has_phone_acquired_signal(void)
{
  return ds707_has_phone_acquired_signal;
}

/*===========================================================================
FUNCTION      DS707_SET_HAS_PHONE_ACQUIRED_SIGNAL

DESCRIPTION   Sets the value of ds707_has_phone_acquired_signal flag

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_set_has_phone_acquired_signal(boolean acquired_signal_val)
{
  ds707_has_phone_acquired_signal = acquired_signal_val;
}

/*===========================================================================
FUNCTION      DS707_GET_PRE_DORMANCY_PARAMS

DESCRIPTION   This function returns pre-dormant data call information such as 
              System Mode and Service Option (SO) used.

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
)
{
  ps_sys_ioctl_3gpp2_pre_dormancy_sys_info_type *pre_dorm_params;
  sys_sys_mode_e_type   curr_network_mode = SYS_SYS_MODE_NONE;

  /*-------------------------------------------------------------------
    If argval pointer or p_errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(p_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return FAILURE;
  }

  if(argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *p_errno = DS_EFAULT;
    return FAILURE;
  }

  curr_network_mode = ds3g_get_current_network_mode();

  /*-------------------------------------------------------------------
    If sys mode is not 1x/HDR, then return failure.
  --------------------------------------------------------------------*/
  if( curr_network_mode != SYS_SYS_MODE_CDMA && 
      curr_network_mode != SYS_SYS_MODE_HDR )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Current sys mode is not C2K (CDMA/HDR)." 
                   " Hence operation not supported.");
    *p_errno = DS_EOPNOTSUPP;
    return FAILURE;
  }

  pre_dorm_params = (ps_sys_ioctl_3gpp2_pre_dormancy_sys_info_type *) argval_ptr;

  pre_dorm_params->data_session_network = (uint8) ds707_pkt_get_last_data_sess_network();
  pre_dorm_params->service_option = (uint16) ds707_pkt_get_prev_call_so();

  if( DS_ISDATA_HDR(pre_dorm_params->service_option) )
  {
    pre_dorm_params->service_option = (uint16)CAI_SO_NULL;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, "LDSN %d prev_pkt_call_so %d", 
                  pre_dorm_params->data_session_network, 
                  pre_dorm_params->service_option );

  return SUCCESS;
} /* ds707_get_pre_dormancy_params() */

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
)
{
  /*-------------------------------------------------------------------
    If argval pointer or p_errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(p_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return FAILURE;
  }

  if(argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *p_errno = DS_EFAULT;
    return FAILURE;
  }

#ifdef FEATURE_DS_CAM
  *((uint32*)argval_ptr) = dscam_get_cam_timer_value();
  return SUCCESS;
#else
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DS CAM feature not supported.");
  *p_errno = DS_EOPNOTSUPP;
  return FAILURE;
#endif /* FEATURE_DS_CAM */

} /* ds707_get_cam_timer() */

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
)
{
#ifdef FEATURE_DS_CAM
  uint32 timer_val = 0;
#endif /* FEATURE_DS_CAM */
  /*-------------------------------------------------------------------
    If argval pointer or p_errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(p_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return FAILURE;
  }

  if(argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *p_errno = DS_EFAULT;
    return FAILURE;
  }

#ifdef FEATURE_DS_CAM
  timer_val = *((uint32*)argval_ptr);
  dscam_set_cam_timer_value( timer_val );
  return SUCCESS;
#else
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DS CAM feature not supported.");
  *p_errno = DS_EOPNOTSUPP;
  return FAILURE;
#endif /* FEATURE_DS_CAM */

} /* ds707_set_cam_timer() */

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
)
{
  /*-------------------------------------------------------------------
    If argval pointer or p_errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(p_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return FAILURE;
  }

  if(argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *p_errno = DS_EFAULT;
    return FAILURE;
  }

  *((boolean*)argval_ptr) = (boolean) ds707_qcscrm_val;
  DATA_IS707_MSG1(MSG_LEGACY_MED, "SCRM enabled is %d.", 
                  *((boolean*)argval_ptr));

  return SUCCESS;
} /* ds707_is_scrm_enabled() */

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
)
{
  boolean enable_scrm = FALSE;

  /*-------------------------------------------------------------------
    If argval pointer or p_errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(p_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return FAILURE;
  }

  if(argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *p_errno = DS_EFAULT;
    return FAILURE;
  }

  enable_scrm = *((boolean*)argval_ptr);
  DATA_IS707_MSG1(MSG_LEGACY_MED, "SCRM enable/disable is %d.", enable_scrm);

  ds707_qcscrm_val = enable_scrm;

  return SUCCESS;
} /* ds707_enable_disable_scrm() */

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
)
{
  /*-------------------------------------------------------------------
    If argval pointer or p_errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(p_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return FAILURE;
  }

  if(argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *p_errno = DS_EFAULT;
    return FAILURE;
  }

#ifdef FEATURE_DS_RDUD
  *((boolean*)argval_ptr) = dsrdud_is_rdud_enabled();
  return SUCCESS;
#else
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DS RDUD feature not supported.");
  *p_errno = DS_EOPNOTSUPP;
  return FAILURE;
#endif /* FEATURE_DS_RDUD */

} /* ds707_is_rdud_enabled() */

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
)
{
  /*-------------------------------------------------------------------
    If argval pointer or p_errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(p_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return FAILURE;
  }

  if(argval_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *p_errno = DS_EFAULT;
    return FAILURE;
  }

#ifdef FEATURE_DS_RDUD
  dsrdud_enable_disable_rdud( *((boolean*)argval_ptr) );
  return SUCCESS;
#else
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, "DS RDUD feature not supported.");
  *p_errno = DS_EOPNOTSUPP;
  return FAILURE;
#endif /* FEATURE_DS_RDUD */


} /* ds707_enable_disable_rdud() */

/*===========================================================================
FUNCTION      DS707_SS_CMD_INFO_CB

DESCRIPTION
  Callback function registered with CM to get status of SS info get request.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_ss_cmd_info_cb
(
  void                 *data_block_ptr,
  cm_ss_cmd_e_type     ss_cmd,
  cm_ss_cmd_err_e_type ss_cmd_err
)
{
  switch( ss_cmd )
  {
    case CM_SS_CMD_INFO_GET:
      if( ss_cmd_err == CM_SS_CMD_ERR_NO_BUF_L )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Buffer could not be allocated by CM.");
        ds707_throttle_info_requested_flag = FALSE;
      }
      break;

    default:
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "Callback called for unexpected SS command.");
      break;
  }
} /* ds707_ss_cmd_info_cb() */

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
)
{
  cm_client_id_type ds3g_cm_client_id;

  ds707_throttle_info_requested_flag = TRUE;
  ds3g_cm_client_id = ds3g_get_cm_client_id();
  return cm_ss_cmd_get_ss_info(ds707_ss_cmd_info_cb, NULL, ds3g_cm_client_id );

} /* ds707_get_ss_info */

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
  DS_ENOMEM - Indicates memory could not be allocated in CM to get SS info.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_get_call_throttle_info
(
  call_thr_info_hdlr_cb *call_thr_info_hdlr_func_ptr,
  int16                 *p_errno
)
{
  boolean result = FALSE;

  /*-------------------------------------------------------------------
    If argval pointer or p_errno is NULL, return failure.
  --------------------------------------------------------------------*/
  if(p_errno == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    return FAILURE;
  }

  if(call_thr_info_hdlr_func_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid argument(s) passed.");
    *p_errno = DS_EFAULT;
    return FAILURE;
  }

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
              "ds707_get_call_throttle_info: register QMI callback function");
  call_thr_info_hdlr = call_thr_info_hdlr_func_ptr;
  result = ds707_get_ss_info();

  if( result == FALSE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not post SS command info get.");
    *p_errno = DS_ENOMEM;
    return FAILURE;
  }

  return SUCCESS;
} /* ds707_get_call_throttle_info() */

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
)
{
  ds707_call_throttle_info thr_info = { 0 };
  ds_dsd_event_info_type  dsd_event_info = {0};
  ds707_call_throttle_info *call_thr_info_ptr = NULL;

  /* 'thr_uptime' indicates curr_uptime+throttle timer value. To find the 
     remaining throttle time, subtract 'curr_uptime' from 'thr_uptime'. */

  if((NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("ds_cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  call_thr_info_ptr = (ds707_call_throttle_info *)(ds_cmd_ptr->cmd_payload_ptr);

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "Remaining throttle time in secs: CDMA %ld, HDR %ld",
                   call_thr_info_ptr->cdma_remaining_thr_time,
                   call_thr_info_ptr->hdr_remaining_thr_time );

  thr_info.cdma_remaining_thr_time = 0;
  thr_info.hdr_remaining_thr_time = 0;


  /* CDMA call throttle status */
  if( call_thr_info_ptr->cdma_remaining_thr_time != 0 )
  {
    thr_info.cdma_remaining_thr_time 
                                = call_thr_info_ptr->cdma_remaining_thr_time;
    dsd_event_info.event = DS_DSD_CLIENT_RADIO_DCTMTHROTTLED_EV;
    ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
  }
  else
  {
    thr_info.cdma_remaining_thr_time = 0;
    dsd_event_info.event = DS_DSD_CLIENT_RADIO_DCTMUNTHROTTLED_EV;
    ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
  }

  /* HDR call throttle status */
  if( call_thr_info_ptr->hdr_remaining_thr_time != 0 )
  {
    thr_info.hdr_remaining_thr_time = call_thr_info_ptr->hdr_remaining_thr_time;

    /* Do not inform HDR throttle info to DSD if in EHSR state. 
       After we extinguish all EHSR retries, the sr_state will
       change to AUSR and at that time we'll trigger CM
       to provide the current throttle info. If HDR is 
       still in throttle state, we'll come here and then 
       report it to DSD  */ 
    if (FALSE == ds707_sys_chg_is_in_ehsr_state())
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_DCTMTHROTTLED_EV;
      ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, &dsd_event_info);
    }
  }
  else
  {
    thr_info.hdr_remaining_thr_time = 0;
    dsd_event_info.event = DS_DSD_CLIENT_RADIO_DCTMUNTHROTTLED_EV;
    ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, &dsd_event_info);
  }

  if( call_thr_info_hdlr != NULL )
  {
    call_thr_info_hdlr( (void*)&thr_info, (int16)DS_ENOERR );
    call_thr_info_hdlr = NULL;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Client handler function not registered."
                    "Hence, can't update client on throttling status.");
  }

  return;
} /* ds707_update_call_throttle_info() */

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
)
{
    int ret = 0;
    boolean *force_sleep = NULL;
    if (NULL == argval_ptr) {
        *ps_errno = DS_EFAULT; //Invalid parameter
        ret = -1;
        return ret;
    }
    force_sleep = (boolean*)argval_ptr;
    hdrpac_set_force_long_sleep(*force_sleep);
    return ret;
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_HANDLE_HDRPAC_EVENTS

DESCRIPTION   Handle events from hdrpac and route them to respective
             callbacks.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_handle_hdrpac_events
(
  /* event generated by hdrpac */
  ps_iface_event_enum_type evt,
  /* event payload */
  void *event_data_ptr
)
{
  int16 ps_errno;
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Received evt: %d", evt);

  if ((IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV == evt) ||
      (IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV == evt))
  {
    int16 ret = 0;
    if (IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV == evt) 
    {
      ret = -1;
    }
    if (NULL != ds707_pkt_mgr_hdrpac_hdlr.slotted_mode_hdlr)
    {
      ds707_pkt_mgr_hdrpac_hdlr.slotted_mode_hdlr(ret,(ps_hdr_slotted_mode_failure_code_enum_type*) event_data_ptr);
    }
  } 
  else if(IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV == evt)
  {
  #if 0
    if (NULL != ds707_pkt_mgr_hdrpac_hdlr.session_chg_hdlr)
    {
      ds707_pkt_mgr_hdrpac_hdlr.session_chg_hdlr(
        *((ds707_pkt_mgr_evdo_page_monitor_period_type *)event_data_ptr));
    }
#endif 
    ps_sys_event_ind_ex( PS_SYS_TECH_3GPP2,
                         PS_SYS_EVENT_3GPP2_EVDO_PAGE_MONITOR_PERIOD_CHANGE,
                         PS_SYS_PRIMARY_SUBS,
                         ((ps_sys_3gpp2_page_monitor_period_info_type *)event_data_ptr),
                         &ps_errno);
  }
}

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
)
{
  int ret = 0;
  hdrpac_page_monitor_period_info_type monitor_period_info;

  if (NULL == argval_ptr)
  {
    *ps_errno = DS_EFAULT; //Invalid parameter
    return -1;
  }

  hdrpac_get_evdo_page_monitor_period(&monitor_period_info);
  ((ps_sys_ioctl_3gpp2_set_page_monitor_type*)argval_ptr)->page_monitor_period = 
                                                           monitor_period_info.slot_cycle_index;
  
  return ret;
}

/*===========================================================================
FUNCTION      DS707_PKT_MGR_MAP_HDR_ERR

DESCRIPTION   
  This function maps hdr error to PS error

DEPENDENCIES  None.

RETURN VALUE
  PS error number equivalenet of hdr error number.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_pkt_mgr_map_hdr_err
(
  /* hdr errno to be mapped */
  hdrerrno_enum_type hdr_err
)
{
  int ps_errno = DS_EINVAL; 
  
  switch(hdr_err)
  {
    case E_NOT_SUPPORTED:
      ps_errno = DS_EOPNOTSUPP; 
      break;
    case E_OUT_OF_RANGE:
      ps_errno = DS_EFAULT; // Invalid parameter
      break;
    case E_NO_NET:
      ps_errno = DS_ENONET;
      break;
    default:
      break;
  }
  
  return ps_errno;
}

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
)
{
  int ret = 0;
  hdrerrno_enum_type hdr_err = 
    hdrpac_change_eidle_slotted_mode(slotted_mode_option);

  if (E_SUCCESS == hdr_err)
  {
    // Register slotted mode handler 
    ds707_pkt_mgr_hdrpac_hdlr.slotted_mode_hdlr = fn_ptr;
  }
  else
  {
    *ps_errno = ds707_pkt_mgr_map_hdr_err(hdr_err);
    ret = -1;
  }

  return ret;
}     

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
)
{
  int ret = 0;

  if (NULL == fn_ptr)
  {
    *ps_errno = DS_EFAULT; //Invalid parameter
    ret = -1;
  }

  ds707_pkt_mgr_hdrpac_hdlr.session_chg_hdlr = fn_ptr;

  return ret;
}

#endif /* FEATURE_EIDLE_SCI */

void ds707_pkt_mgr_util_init
(
  void
)
{
#ifdef FEATURE_EIDLE_SCI
  //Register for events from hdrpac
  hdrpac_register_for_iface_events(ds707_pkt_mgr_handle_hdrpac_events);  
#endif /* FEATURE_EIDLE_SCI */
}



/*===========================================================================
FUNCTION      DS707_PKT_MGR_GET_PRI_PHYS_LINK

DESCRIPTION
  This function returns the primary physical link. Increments the ref count 
  of physical link and ds707_pkt_mgr_release_pri_phys_link to be called 
  when phys link usage is over. 

PARAMETERS    None

DEPENDENCIES  Physical link needs to be up before calling this.

RETURN VALUE  Primary physical link.

SIDE EFFECTS None
===========================================================================*/
ps_phys_link_type *ds707_pkt_mgr_get_pri_phys_link
(
  void
)
{
   ps_phys_link_type* phys_link_ptr =
         ds707_pkt_get_pri_phys_link_ptr();

   PS_PHYS_LINKI_INC_REF_CNT(phys_link_ptr);
   ds707_pri_physlink_refcount++;

   return phys_link_ptr;
}

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

SIDE EFFECTS None
===========================================================================*/
int16 ds707_pkt_mgr_release_pri_phys_link
( 
  ps_phys_link_type* phys_link_ptr
)
{
  int ret_val;
  int16 err_num;

  /* Sanity check */
  ASSERT(ds707_pri_physlink_refcount != 0);

  ds707_pri_physlink_refcount--;
  PS_PHYS_LINKI_DEC_REF_CNT(phys_link_ptr);

  ret_val = ps_phys_link_go_null_cmd (
                phys_link_ptr,&err_num, NULL);

  if (ret_val < 0)
  {
     if (err_num != DS_EWOULDBLOCK)
     {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                        "phy link down fail, %d phys_link ret", err_num);
     }
     else
     {
       /* Since it returned ewouldblock, treat it as a SUCCESS */
       ret_val = 0;
     }
  }

  return ret_val;
}
/*===========================================================================
FUNCTION      DS707_CLOSE_HDR_SESSION

DESCRIPTION   To close HDR session

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_close_hdr_session(boolean close_is_silent)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "ds707_close_hdr_session, close_is_silent:%d",
                  close_is_silent);
  hdrcp_smp_deactivate(HDRHAI_PACKET_APPLICATION_CONTROLLER, close_is_silent);
} /* ds707_close_hdr_session() */

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
)
{
  ds707_tun_phys_link_state old_state;

  if (NULL == tc_state_ptr )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "TC state ptr is NULL");
    return;
  }

  old_state = tc_state_ptr->tun_phys_link_state;

  if (old_state == tun_phys_link_state)
  {
     DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                "Transition tun_phys_link_state to: %s"
                "old state == new state - Ignore transition",
                tun_phys_link_state_names[old_state]);
    return;
  }

  /* Below check is for debug purpose to ensure the transition
     from one state to another is a documented one. If not,
     it just flags as an error but doesn't stop any functionality
  */
  switch (old_state)
  {
    case DS707_TUN_PHYS_LINK_NULL:
    {
      if (DS707_TUN_PHYS_LINK_COMINGUP != tun_phys_link_state)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Transition tun_phys_link_state: "
                        "ALERT - INVALID TRANSITION");
      }

      break;
    }

    case DS707_TUN_PHYS_LINK_COMINGUP:
    {
      break;
    }

    case DS707_TUN_PHYS_LINK_UP:
    {
      if (DS707_TUN_PHYS_LINK_DOWN != tun_phys_link_state && 
          DS707_TUN_PHYS_LINK_NULL != tun_phys_link_state)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Transition tun_phys_link_state: "
                        "ALERT - INVALID TRANSITION");
      }
      break;
    }

    case DS707_TUN_PHYS_LINK_DOWN:
    {
      if (DS707_TUN_PHYS_LINK_COMINGUP != tun_phys_link_state && 
          DS707_TUN_PHYS_LINK_NULL != tun_phys_link_state)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Transition tun_phys_link_state: "
                        "ALERT - INVALID TRANSITION");
      }
      break;
    }

    default: 
    { 
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                      "Transition tun_phys_link_state: "
                      "Invalid tun_phys_link_state:%d passed",
                      tun_phys_link_state);
      return;
    }
  }

  /* Update the new state */
  tc_state_ptr->tun_phys_link_state = tun_phys_link_state;

  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                "Transition tun_phys_link_state: %s to: %s ",
                tun_phys_link_state_names[old_state], 
                tun_phys_link_state_names[tun_phys_link_state]);

  

} /* ds707_transition_tun_phys_link_state */

/*===========================================================================
FUNCTION      DS707_GET_TUN_PHYS_LINK_STATE

DESCRIPTION   Given the tc_state_ptr, gets the current tun phys link state

DEPENDENCIES  None

RETURN VALUE  TUN_PHYS_LINK_STATE

SIDE EFFECTS  None
===========================================================================*/
ds707_tun_phys_link_state ds707_get_tun_phys_link_state
(
  ds707_tc_state_type* tc_state_ptr
)
{
  if (NULL == tc_state_ptr )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "get_tun_phys_link_state: "
                                    "TC state ptr is NULL");
    return DS707_TUN_PHYS_LINK_NULL;
  }

  if (tc_state_ptr->tun_phys_link_state < DS707_TUN_PHYS_LINK_MAX) 
  {
    DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                  "get_tun_phys_link_state: current state: %s ",
                  tun_phys_link_state_names[tc_state_ptr->tun_phys_link_state]);
  }
 
  return tc_state_ptr->tun_phys_link_state;

}
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
)
{
  /*-------------------------------------------------------------------------
    default TRUE, because NO handoff allowed will tear down iface
  -------------------------------------------------------------------------*/
  boolean rval = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                  "ds707_pkt_mgr_handoff_allowed between: old: %d, new %d",
                  old_sys, new_sys);

  switch(old_sys)
  {
    case SYS_SYS_MODE_CDMA:
      {
        switch(new_sys)
        {
          case SYS_SYS_MODE_HDR:
          {
            #ifdef FEATURE_DATA_FUSION
              rval = FALSE;
            #else
              rval = TRUE;
            #endif
            break;
          }

          default:
          {
            DATA_IS707_MSG2(MSG_LEGACY_MED, 
                        "handoff not allowed betn old_sys: %d to new_sys: %d ",
                        old_sys, new_sys);
            rval = FALSE;
            break;
          }
        }
      }
      break;

    case SYS_SYS_MODE_HDR:
      {
        switch(new_sys)
        {
          case SYS_SYS_MODE_CDMA:
            {
              #ifdef FEATURE_DATA_FUSION
                rval = FALSE;
              #else
                rval = TRUE;
              #endif
              break;
            }
          case SYS_SYS_MODE_LTE:
            {
              /*---------------------------------------------------------------
                Since this is a DO-LTE handoff case, this would have been
                handled when CM indicated sys change so we assume handoff is
                allowed, because if it werent, then ds707 iface would have been
                already torn down
                If the ds707 iface is not yet torn down, and if the previous
                system was HRPD, then tear down the ds707 iface right away,
                do not wait for CM's call end. Its observed that CM sends call
                end ~30sec after LTE attach is complete, since CM waits
                for 30s silent redial period. Since LTE attach is complete,
                ds707 iface should be torn down.
                If all ifaces are down then return TRUE since partial context 
                might be maintained
              ---------------------------------------------------------------*/
              if((TRUE == ds707_pkt_mgr_all_ifaces_down()) || 
                 (TRUE == ds707_pkt_mgr_is_data_on_ehrpd()))
              {
                rval = TRUE;
              }
              else
              {
                rval = FALSE;
              }
              break;
            }
          default:
            {
              DATA_IS707_MSG2(MSG_LEGACY_MED, 
                              "handoff not allowed betn old_sys: %d to new_sys: %d ",
                              old_sys, new_sys);
              rval = FALSE;
              break;
            }
        }
      }
      break;
    
    default:
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unhandled old_sys; %d", old_sys);
      break;
    }
  }

  return rval;
}/* ds707_pkt_mgr_handoff_allowed() */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_NV_SYNC

DESCRIPTION   This function reads all the 707-specific NV items out of
              NV.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_nv_sync(void)
{
  nv_item_type        ds707_nv_item;
  nv_stat_enum_type   status;                /* status from NV call */
  boolean             mip_prof_inited = 0;
#ifdef FEATURE_IS2000_REL_A
  byte                temp_qos_pri = 0;
#endif /* FEATURE_IS2000_REL_A */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
   Get Mobile IP preference setting from NV.
  -------------------------------------------------------------------------*/

  status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_QCMIP_I, &ds707_nv_item, ds707_curr_subs_id());
  if( status == NV_DONE_S )
  {
  /*-------------------------------------------------------------------------
    Store $qcmip val retrieved from NV in static profile
  -------------------------------------------------------------------------*/
    ds707_mip_static_profile.mip.ds_qcmip_val = ds707_nv_item.ds_qcmip;
  }
  else
  {
    ds707_mip_static_profile.mip.ds_qcmip_val = 0;
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad NV read status %d for DS QCMIP. "
                    "Initializing to default value.", status);
  }
  
  if(ds707_mip_static_profile.mip.ds_qcmip_val != 0)
  {
    /*-------------------------------------------------------------------------
            Get MN-HA authentication method from NV.
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_MIP_2002BIS_MN_HA_AUTH_I, &ds707_nv_item, ds707_curr_subs_id());
    if( status == NV_DONE_S )
    {
      ds707_mip_static_profile.mip.ds_qcmipt_val = 
      	      ds707_nv_item.ds_mip_2002bis_mn_ha_auth;
    }
    else
    {
      ds707_mip_static_profile.mip.ds_qcmipt_val = 0;
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Bad NV read status %d for MH-HA auth method. "
                      "Initializing to default value.",
                      status);
    }
  
    /*-------------------------------------------------------------------------
      Get Mobile IP active profile from NV.
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_MIP_ACTIVE_PROF_I, &ds707_nv_item, ds707_curr_subs_id());
    if( status == NV_DONE_S )
    {
      ds707_mip_static_profile.mip.ds_qcmipp_val = 
      	                  ds707_nv_item.ds_mip_active_prof;
      ds707_mip_static_profile.mip.ds_qcmipgetp_val =
                          ds707_nv_item.ds_mip_active_prof;
    }
    else
    {
      ds707_mip_static_profile.mip.ds_qcmipp_val = 0;
      ds707_mip_static_profile.mip.ds_qcmipgetp_val = 0;
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                     "Bad NV read status %d for MIP active profile. "
                     "Initializing to default value.",
                     status);
    }
  
    /*-------------------------------------------------------------------------
       Enable or disable  profile?
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex(NV_DS_MIP_ENABLE_PROF_I, &ds707_nv_item, ds707_curr_subs_id());
    if( status == NV_DONE_S )
    {
      ds707_mip_static_profile.mip.ds_qcmipep_val = 
 	  	              ds707_nv_item.ds_mip_enable_prof[
 	  	                      ds707_mip_static_profile.mip.ds_qcmipp_val];
    }
    else
    {
      ds707_mip_static_profile.mip.ds_qcmipep_val = 0;
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
      	              "Bad NV read status %d for MIP enable. "
      	              "Initializing to default value.",
      	              status);
    }
   
    /*-------------------------------------------------------------------------
      Retreive the general user profile from NV
    -------------------------------------------------------------------------*/
    mip_prof_inited = ds_init_mip_prof();
    
    ds707_nv_item.ds_mip_gen_user_prof.index = 
      	       ds707_mip_static_profile.mip.ds_qcmipp_val;
    ds3gcfgmgr_read_legacy_nv_ex(NV_DS_MIP_GEN_USER_PROF_I,&ds707_nv_item,ds707_curr_subs_id());
    	
    if( TRUE == mip_prof_inited )
    {
     /*------------------------------------------------------------------------
            Store number retrieved from NV.
     ------------------------------------------------------------------------*/
      ds707_mip_static_profile.mip.ds_qcmiprt_val[0] =
                     ds707_nv_item.ds_mip_gen_user_prof.rev_tun_pref; 
      ds707_mip_static_profile.mip.ds_qcmiprt_val[1] = 1;
	  
      ds707_mip_static_profile.mip.ds_qcmipmaspi_val[0] =
                     ds707_nv_item.ds_mip_gen_user_prof.mn_aaa_spi; 
      ds707_mip_static_profile.mip.ds_qcmipmaspi_val[1] = 1;

      ds707_mip_static_profile.mip.ds_qcmipmhspi_val[0] =
                     ds707_nv_item.ds_mip_gen_user_prof.mn_ha_spi; 
      ds707_mip_static_profile.mip.ds_qcmipmhspi_val[1] = 1;
    }
   
    if(ds707_nv_item.ds_mip_gen_user_prof.nai_length != 0 )
    {
      memset(&ds707_mip_static_profile.mip.ds_qcmipnai_val[0],
             0x0,
             MAX_NAI_LEN);

      memscpy(&ds707_mip_static_profile.mip.ds_qcmipnai_val[0],
              MAX_NAI_LEN,
              (void*)ds707_nv_item.ds_mip_gen_user_prof.nai,
              ds707_nv_item.ds_mip_gen_user_prof.nai_length);
    }        

    ds707_mip_static_profile.mip.ds_qcmip_ha_num_val     =
                    ds707_nv_item.ds_mip_gen_user_prof.home_addr;
    ds707_mip_static_profile.mip.ds_qcmip_pri_ha_num_val =
                    ds707_nv_item.ds_mip_gen_user_prof.primary_ha_addr;			
    ds707_mip_static_profile.mip.ds_qcmip_sec_ha_num_val =
                    ds707_nv_item.ds_mip_gen_user_prof.secondary_ha_addr;
    
    /*-------------------------------------------------------------------------
      Retreive the shared secret user profile from NV
    -------------------------------------------------------------------------*/
    ds707_nv_item.ds_mip_ss_user_prof.index =
            ds707_mip_static_profile.mip.ds_qcmipp_val;
    ds3gcfgmgr_read_legacy_nv_ex(NV_DS_MIP_SS_USER_PROF_I,&ds707_nv_item,ds707_curr_subs_id());
   	
    if( ds707_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length!= 0 )
    {
      memset(&ds707_mip_static_profile.mip.ds_qcmipmass_val[0],
             0x0,
             MIP_MN_AAA_SS_MAX_LEN);

      memscpy(&ds707_mip_static_profile.mip.ds_qcmipmass_val[0],
              MIP_MN_AAA_SS_MAX_LEN,
              (void*)ds707_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret,
              ds707_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length);
    }

    if(ds707_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length!= 0 )
    {
      memset(&ds707_mip_static_profile.mip.ds_qcmipmhss_val[0],
             0x0,
             MIP_MN_HA_SS_MAX_LEN);

      memscpy(&ds707_mip_static_profile.mip.ds_qcmipmhss_val[0],
              MIP_MN_HA_SS_MAX_LEN,
              (void*)ds707_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret,
              ds707_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length);
    }	    
  }
  else
  {
    ds707_mip_static_profile.mip.ds_qcmipmaspi_val[0] = 0x2;
    ds707_mip_static_profile.mip.ds_qcmipmaspi_val[1] = 0;
    ds707_mip_static_profile.mip.ds_qcmipmhspi_val[0] = 0x3;
    ds707_mip_static_profile.mip.ds_qcmipmhspi_val[1] = 0;
  }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_QNC
  /*-------------------------------------------------------------------------
    Get QNC enable flag from NV.
  -------------------------------------------------------------------------*/
   status = ds3gcfgmgr_read_legacy_nv_ex( NV_DATA_QNC_ENABLED_I, &ds707_nv_item, ds707_curr_subs_id());
   if( status == NV_DONE_S )
   {
     /*------------------------------------------------------------------------
      Store number retrieved from NV.
     ------------------------------------------------------------------------*/
     ds707_mip_static_profile.ds707.ds707_qcqnc_val = ds707_nv_item.data_qnc_enabled;
   }
   else
   {
     ds707_mip_static_profile.ds707.ds707_qcqnc_val = DS_QNC_DEFAULT;
     DATA_IS707_MSG1(MSG_LEGACY_ERROR,
     	               "Bad NV read status %d for QNC enable flag. "
     	               "Initializing to default value.",
     	               status);
   }
#endif /* FEATURE_DS_QNC */

#ifdef FEATURE_DS_CHOOSE_SO
  /*-------------------------------------------------------------------------
    Get the service option selection from NV.
  -------------------------------------------------------------------------*/
   status = ds3gcfgmgr_read_legacy_nv_ex( NV_DATA_SO_SET_I, &ds707_nv_item, ds707_curr_subs_id());
   if( status == NV_DONE_S )
   {
   /*-------------------------------------------------------------------------
     Store number retrieved from NV.
     -------------------------------------------------------------------------*/
     ds707_mip_static_profile.ds707.ds707_qcso_val = ds707_nv_item.data_so_set;
   }
   else
   {
     ds707_mip_static_profile.ds707.ds707_qcso_val = DS_SO_SET_DEFAULT;
     DATA_IS707_MSG1(MSG_LEGACY_ERROR,
     	             "Bad NV read status %d for service option. "
     	             "Initializing to default value.",
                   status);
   }
#endif /* FEATURE_DS_CHOOSE_SO */

  /*-------------------------------------------------------------------------
    Get the MDR Mode from NV.
  -------------------------------------------------------------------------*/
   status = ds3gcfgmgr_read_legacy_nv_ex(NV_DATA_MDR_MODE_I, &ds707_nv_item, ds707_curr_subs_id());

   if( status == NV_DONE_S )
   {
    /*-------------------------------------------------------------------------
       Store the value retrieved from NV.
    -------------------------------------------------------------------------*/
     ds707_qcmdr_val = ds707_nv_item.data_mdr_mode;
   }
   else
   {
     ds707_qcmdr_val = DS_MDR_MODE_DEFAULT;
     DATA_IS707_MSG1(MSG_LEGACY_ERROR,
     	               "Bad NV read status %d for MDR mode. "
     	               "Initializing to default value.",
     	               status);
   }
  /*-------------------------------------------------------------------------
    Get the Packet/QNC dial string from NV.
  -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex(NV_DATA_PKT_ORIG_STR_I, &ds707_nv_item, ds707_curr_subs_id());

  if( status == NV_DONE_S )
  {
   /*-------------------------------------------------------------------------
     Store the value retrieved from NV.
   -------------------------------------------------------------------------*/
    ds707_pkt_mgr_orig_str_len = ds707_nv_item.data_pkt_orig_str.num_digits;
    if( (NV_MAX_PKT_ORIG_DIGITS + 1) > 
                ds707_nv_item.data_pkt_orig_str.num_digits )
    {
      if(ds707_nv_item.data_pkt_orig_str.num_digits == 0)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Error in copying Pkt dial string");
      }
      else
      {
        memscpy((void*)ds707_pkt_mgr_orig_str,
                NV_MAX_PKT_ORIG_DIGITS,
                (void *)ds707_nv_item.data_pkt_orig_str.digits,
                ds707_nv_item.data_pkt_orig_str.num_digits);
      }
    }
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for Pkt dial string",
                    status);
  }

#if defined(FEATURE_IS2000_R_SCH)
  /*-------------------------------------------------------------------------
    Get the SCRM Mode from NV.
  -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex(NV_DATA_SCRM_ENABLED_I, &ds707_nv_item, ds707_curr_subs_id());
  if( status == NV_DONE_S )
  {   
    /*-------------------------------------------------------------------------
      Store number retrieved from NV.
    -------------------------------------------------------------------------*/
    ds707_qcscrm_val = ds707_nv_item.data_scrm_enabled;
  }
  else
  {
    ds707_qcscrm_val = DS_SCRM_DEFAULT;
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for SCRM mode. "
                    "Initializing to default value.",
                    status);
  }
#endif /* FEATURE_IS2000_R_SCH    */

#if defined(FEATURE_IS2000_R_SCH)
  /*-------------------------------------------------------------------------
    Get the R-SCH throttle (QCTRTL) Mode from NV.
  -------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex(NV_DATA_TRTL_ENABLED_I, &ds707_nv_item, ds707_curr_subs_id());
  if( status == NV_DONE_S )
  {  
   /*-------------------------------------------------------------------------
     Store number retrieved from NV.
   -------------------------------------------------------------------------*/
    ds707_qctrtl_val = ds707_nv_item.data_trtl_enabled;
  }
  else
  {
    ds707_qctrtl_val = DS_TRTL_DEFAULT;
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for QCTRTL mode. "
                    "Initializing to default value.",
                    status);
  }
#endif /* FEATURE_IS2000_R_SCH */

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    /*-------------------------------------------------------------------------
      M511
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_JCDMA_M511_MODE_I, &ds707_nv_item, ds707_curr_subs_id());
    if( status == NV_DONE_S )
    {    
    /*-------------------------------------------------------------------------
      Store number retrieved from NV.
    -------------------------------------------------------------------------*/
      ds707_jcdma_m51_set_val(JCDMA_VAL_511,
                      (sys_jcdma_m511_e_type)(ds707_nv_item.jcdma_m511_mode));
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
      	              "Bad NV read status %d for M51 1",
      	              status);
    }

    /*-------------------------------------------------------------------------
      M512
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_JCDMA_M512_MODE_I, &ds707_nv_item, ds707_curr_subs_id());
    if( status == NV_DONE_S )
    {   
     /*-------------------------------------------------------------------------
       Store number retrieved from NV.
     -------------------------------------------------------------------------*/
      ds707_jcdma_m51_set_val(JCDMA_VAL_512,
                       (sys_jcdma_m512_e_type)(ds707_nv_item.jcdma_m512_mode));
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
      	              "Bad NV read status %d for M51 2",
      	              status);
    }
    /*-------------------------------------------------------------------------
      M513
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_JCDMA_M513_MODE_I, &ds707_nv_item, ds707_curr_subs_id());
    if( status == NV_DONE_S )
    {   
     /*-------------------------------------------------------------------------
       Store number retrieved from NV.
     -------------------------------------------------------------------------*/
      ds707_jcdma_m51_set_val(JCDMA_VAL_513,
                      (sys_jcdma_m513_e_type)(ds707_nv_item.jcdma_m513_mode));
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Bad NV read status %d for M51 3",
      	              status);
    }
   
    /*-------------------------------------------------------------------------
      Get DATA_FORCE_CDMA_PKT_STR from NV 
    --------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_DATA_FORCE_CDMA_PKT_STR_I, &ds707_nv_item, ds707_curr_subs_id());
    if( status == NV_DONE_S)
    {
      if (ds707_nv_item.data_force_cdma_pkt_str.num_digits <=  
                              sizeof(ds707_pkt_mgr_force_cdma_dial_str))
      {
        ds707_pkt_mgr_force_cdma_str_len =
                        ds707_nv_item.data_force_cdma_pkt_str.num_digits;
      }
      else
      {
        ds707_pkt_mgr_force_cdma_str_len = 
                        sizeof(ds707_pkt_mgr_force_cdma_dial_str);
        DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                        "Truncating Force CDMA Pkt Str to %d digits",
                        ds707_pkt_mgr_force_cdma_str_len);
      }

      if((NV_MAX_PKT_ORIG_DIGITS+ 1 ) > ds707_pkt_mgr_force_cdma_str_len)
      {
        memscpy((void*)ds707_pkt_mgr_force_cdma_dial_str,
                NV_MAX_PKT_ORIG_DIGITS,
                (void *)ds707_nv_item.data_force_cdma_pkt_str.digits,
                ds707_pkt_mgr_force_cdma_str_len);
      }
    }
    else if( status != NV_NOTACTIVE_S )
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Bad NV read status %d for DATA_FORCE_CDMA_PKT_STR",
                      status);
    }
  }

#ifdef FEATURE_DS_EPZID_HYSTERESIS
  /*------------------------------------------------------------------------
    Process the NV items to see if HAT/ HT changed. If they are
    not set in QXDM then use the default values of 30sec for HAT and 
    60sec for HT
  --------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_HYSTERISIS_ACTIVATION_TIMER_I, &ds707_nv_item, ds707_curr_subs_id() );
  if( status == NV_DONE_S )
  {
  /*------------------------------------------------------------------------
    Make sure the value is within the bounds. 
  ------------------------------------------------------------------------*/
    if( ds707_nv_item.hysterisis_activation_timer > 
                                 EPZID_HYSTERESIS_HAT_TIMER_IN_SEC )
    {
      ds707_nv_item.hysterisis_activation_timer = 
                                 EPZID_HYSTERESIS_HAT_TIMER_IN_SEC;
    }

    ds707_epzid_hysteresis_set_hat_timer_in_sec(
                 (uint8)ds707_nv_item.hysterisis_activation_timer);
#ifdef FEATURE_IS707B_ADDENDUM
    ds707_epzid_set_nv_hat_timer_in_sec(
                (uint8)ds707_nv_item.hysterisis_activation_timer);
#endif /* FEATURE_IS707B_ADDENDUM */
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for HAT timer",
                    status);
  }

  status = ds3gcfgmgr_read_legacy_nv_ex( NV_HYSTERISIS_TIMER_I, &ds707_nv_item, ds707_curr_subs_id() );
  if( status == NV_DONE_S )
  {
    /*------------------------------------------------------------------------
      Make sure the value is within the bounds. 
    ------------------------------------------------------------------------*/
    if(ds707_nv_item.hysterisis_timer > EPZID_HYSTERESIS_HT_TIMER_IN_SEC)
    {
      ds707_nv_item.hysterisis_timer = EPZID_HYSTERESIS_HT_TIMER_IN_SEC;
    }
    ds707_epzid_hysteresis_set_ht_timer_in_sec(
                     (uint8)ds707_nv_item.hysterisis_timer);
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for HT timer",
                    status);
  }

  status = ds3gcfgmgr_read_legacy_nv_ex( NV_ACTIVE_EPZID_TYPE_I, &ds707_nv_item, ds707_curr_subs_id() );
  if( status == NV_DONE_S )
  {
   /*------------------------------------------------------------------------
     Make sure the value is within the bounds. 
   ------------------------------------------------------------------------*/
    if(ds707_nv_item.active_epzid_type > NV_EPZID_PZID_SID_NID)
    {
      ds707_nv_item.active_epzid_type = DEFAULT_ACTIVE_EPZID_TYPE;
    }
    ds707_epzid_set_active_epzid_type(
                 (byte)ds707_nv_item.active_epzid_type);
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for ACTIVE_EPZID_TYPE",
                    status);
  }
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

  status = ds3gcfgmgr_read_legacy_nv_ex( NV_DSAT707_CTA_TIMER_I, &ds707_nv_item, ds707_curr_subs_id() );
  if( status != NV_DONE_S) 
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for CTA timer",
                    status);
    ds707_nv_item.dsat707_cta_timer = DS707_CTA_DEFAULT_VALUE;
  }
  if( ds707_nv_item.dsat707_cta_timer > DS707_CTA_UPPER_LIMIT )
  {
    ds707_nv_item.dsat707_cta_timer = DS707_CTA_DEFAULT_VALUE;   
  }
  ds707_cta_val = ds707_nv_item.dsat707_cta_timer;

#ifdef FEATURE_CDMA
#ifdef FEATURE_IS2000_REL_A
  if ((dsrlp_get_desired_qos_non_assur_pri(&temp_qos_pri)) == TRUE)
  {
    ds707_qcqospri_val = temp_qos_pri;
  }
  else
  {
    ds707_qcqospri_val = DSRLP_QOS_NON_ASSUR_DEFAULT_PRI_ADJ;
  }
#endif /*FEATURE_IS2000_REL_A*/
#endif/*FEATURE_CDMA*/

/*-------------------------------------------------------------------------------
  Setting default value for CRM 
-------------------------------------------------------------------------------*/
  ds707_mip_static_profile.ds707.ds707_crm_val = 2;

/*-------------------------------------------------------------------------------
  Setting default value for QCCHS 
-------------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_CHS
  ds707_qcchs_val = DS_CHS_DEFAULT;
#endif /* FEATURE_IS2000_CHS */

/*-------------------------------------------------------------------------------
  Setting default value for CMUX 
-------------------------------------------------------------------------------*/
#ifdef FEATURE_DS_IS707A_CMUX
  ds707_cmux_val[0] = 11;
  ds707_cmux_val[1] = 1;
#else
  ds707_cmux_val = 2;
#endif /* FEATURE_DS_IS707A_CMUX */

/*-------------------------------------------------------------------------------
  Read Primary DNS and Secondary DNS from the NV during
  initialization. If these items were never written in NV, the values are
  made as NULL.
-------------------------------------------------------------------------------*/
  ds707_qcdns_val[0] = 0;
  ds707_qcdns_val[1] = 0;

/*-------------------------------------------------------------------------
    Get Primary DNS IP addresses from NV. Look at the status returned. If
    status !=NV_DONE_S, then either NV was never written or it cannot be
    reused. Make IP addresses as NULL if NV was never written.
-------------------------------------------------------------------------*/
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_PRIMARY_DNS_I, &ds707_nv_item, ds707_curr_subs_id() );
  if ( status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
    -----------------------------------------------------------------------*/
    ds707_qcdns_val[0] = ds707_nv_item.primary_dns;
  }
  else if ( status == NV_NOTACTIVE_S )
  {
    /* NV item not populated; use default */
  }
  else 
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for Pri DNS NV",
                    status);
  }

  /*-------------------------------------------------------------------------
    Get Secondary DNS IP addresses from NV. Look at the status returned. If
    status !=NV_DONE_S, then either NV was never written or it cannot be
    reused. Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/  
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_SECONDARY_DNS_I, &ds707_nv_item, ds707_curr_subs_id() );
  if( status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
    -----------------------------------------------------------------------*/
    ds707_qcdns_val[1] = ds707_nv_item.secondary_dns;
  }
  else if ( status == NV_NOTACTIVE_S )
  {
    /* NV item not populated; use default */
  }
  else 
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                    "Bad NV read status %d for Sec. DNS NV",
                    status);
  }
     
  return;
} /* ds707_pkt_mgr_nv_sync */

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
)
{
  uint32            min_str = 0;
  nv_stat_enum_type nv_stat = NV_DONE_S;
  word              temp = 0;
  byte             *txt_ptr = digits;
  nv_item_type      ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (digits == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "ds707_pkt_mgr_read_min1(): recd NULL output string");
    return FALSE;
  }

  ds707_nv_item.min1.nam = (byte) CM_NAM_1;
  nv_stat = ds3gcfgmgr_read_legacy_nv_ex( NV_MIN1_I, &ds707_nv_item, ds707_curr_subs_id() );

  if (nv_stat != NV_DONE_S)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "ds707_pkt_mgr_read_min1(): unable to read MIN1 from NV");
    return FALSE;
  }
  min_str = (uint32)(ds707_nv_item.min1.min1[NV_CDMA_MIN_INDEX] & 0xFFFFFFL);

  /* Extract the first ten bits and convert them into three decimal digits. */
  temp       = (word)((min_str >> 14) & 0x3FF);
  *txt_ptr++ = mintab[IMSI_HUNDREDS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_TENS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_ONES_DIGIT(temp)];
  
  /* Extract the next four bits and convert them into one decimal digit. */
  temp = (word)(((min_str & 0x3FFFL) >> 10) & 0xF);
  *txt_ptr++ = (char)( ((temp == 10)? 0: temp) + '0' );

  /* Extract the last ten bits and convert them into three decimal digits. */
  temp       = (word)(min_str & 0x3FF);
  *txt_ptr++ = mintab[IMSI_HUNDREDS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_TENS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_ONES_DIGIT(temp)];
  *txt_ptr = 0;

  return TRUE;
}

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
)
{
  nv_stat_enum_type  nv_stat = NV_DONE_S;
  word               temp = 0;
  byte              *txt_ptr = digits;
  nv_item_type       ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (digits == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "ds707_pkt_mgr_read_min2(): recd NULL output string");
    return FALSE;
  }

  ds707_nv_item.min1.nam = (byte) CM_NAM_1;
  nv_stat = ds3gcfgmgr_read_legacy_nv_ex( NV_MIN2_I, &ds707_nv_item, ds707_curr_subs_id() );

  if (nv_stat != NV_DONE_S)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "ds707_pkt_mgr_read_min2(): unable to read MIN2 from NV");
    return FALSE;
  }
  temp = (word)(ds707_nv_item.min2.min2[NV_CDMA_MIN_INDEX] & 0x3FF);

  *txt_ptr++ = mintab[IMSI_HUNDREDS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_TENS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_ONES_DIGIT(temp)];
  *txt_ptr   = 0;

  return TRUE;
} 

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
)
{
  nv_stat_enum_type  nv_stat = NV_DONE_S;
  word               temp = 0;
  byte              *txt_ptr = digits;
  nv_item_type       ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (digits == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
    	               "ds707_pkt_mgr_read_imsi_mcc(): recd NULL output string");
    return FALSE;
  }

  ds707_nv_item.min1.nam = 0;
  nv_stat = ds3gcfgmgr_read_legacy_nv_ex( NV_IMSI_MCC_I, &ds707_nv_item, ds707_curr_subs_id() );

  if (nv_stat != NV_DONE_S)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
    	               "ds707_pkt_mgr_read_imsi_mcc(): "
    	               "unable to read MIN1 from NV");
    return FALSE;
  }
  temp = (word)(ds707_nv_item.imsi_mcc.imsi_mcc & 0x3FF);

  *txt_ptr++ = mintab[IMSI_HUNDREDS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_TENS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_ONES_DIGIT(temp)];
  *txt_ptr   = 0;

  return TRUE;
} 

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
)
{
  nv_stat_enum_type  nv_stat = NV_DONE_S;
  byte               temp = 0;
  byte              *txt_ptr = digits;
  nv_item_type       ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (digits == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "ds707_pkt_mgr_read_imsi_mnc(): recd NULL output string");
    return FALSE;
  }

  ds707_nv_item.min1.nam = 0;
  nv_stat = ds3gcfgmgr_read_legacy_nv_ex( NV_IMSI_11_12_I, &ds707_nv_item, ds707_curr_subs_id() );

  if (nv_stat != NV_DONE_S)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                 "ds707_pkt_mgr_read_imsi_mnc(): unable to read MIN1 from NV");
    return FALSE;
  }
  temp = (byte) ds707_nv_item.imsi_11_12.imsi_11_12;

  *txt_ptr++ = mintab[IMSI_TENS_DIGIT(temp)];
  *txt_ptr++ = mintab[IMSI_ONES_DIGIT(temp)];
  *txt_ptr   = 0;

  return TRUE;
}

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
)
{
#ifdef FEATURE_DS_MOBILE_IP
  uint8             mip_val = 0;
  nv_item_type      ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If QCMIP is greater than 0, MDR is set to 3.
  -------------------------------------------------------------------------*/
  (void) dsmip_get_1x_profile_val_num(DS_QCMIP_VAL,
                                      (void *)&mip_val,MIP_NON_ARRAY);
  if (mip_val > 0)
  {
    ds707_qcmdr_val = DS_MDR_MODE_SO33_PREF;
    /*-----------------------------------------------------------------------
       Modify value of MDR in NV. - later: check return value
       -----------------------------------------------------------------------*/
    ds707_nv_item.data_mdr_mode = ds707_qcmdr_val;
	
#ifdef FEATURE_MODEM_RCINIT_PHASE2
    if (rex_self() == rcinit_lookup_rextask("ds"))
    {
      (void) ds3gcfgmgr_write_legacy_nv_ex(NV_DATA_MDR_MODE_I, &ds707_nv_item, ds707_curr_subs_id());
    }
    else if (rex_self() == rcinit_lookup_rextask("dcc")) 
    {
      (void) dcc_put_nv_item (NV_DATA_MDR_MODE_I, &ds707_nv_item);
    }
#else
    if (rex_self() == &ds_tcb)
    {
      (void) ds3gcfgmgr_write_legacy_nv_ex (NV_DATA_MDR_MODE_I, &ds707_nv_item, ds707_curr_subs_id());
    }
    else if (rex_self() == &dcc_tcb) 
    {
      (void) dcc_put_nv_item (NV_DATA_MDR_MODE_I, &ds707_nv_item);
    }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
  }
#else
  DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_pkt_mgr_qcmip_set_mdr_val not supported.");
#endif /*FEATURE_DS_MOBILE_IP*/
  
  return;
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_QCMIP_SET_CRM_VAL

DESCRIPTION
  This function checks the value of QCMIP. If it is greater than 0, it
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
)
{
#ifdef FEATURE_DS_MOBILE_IP
  uint8 mip_val = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If QCMIP is greater than 0, CRM is set to 2 which enables network model
    operation.  The original value of CRM is stored so it can be restored
    later when QCMIP is set to 0.
  -------------------------------------------------------------------------*/
  (void) dsmip_get_1x_profile_val_num(DS_QCMIP_VAL,
                                      (void *)&mip_val,MIP_NON_ARRAY);
  if (mip_val > 0)
  {
    if (ds707_crm_orig_val == -1)
    {
      ds707_crm_orig_val = 
              (sint31) ds707_mip_static_profile.ds707.ds707_crm_val;
    }
    ds707_mip_static_profile.ds707.ds707_crm_val = DS_CRM_VAL_NETMODEL_MODE;
  }
  /*-------------------------------------------------------------------------
    Else QCMIP is 0 and the original value of CRM is restored if necessary.
  -------------------------------------------------------------------------*/
  else
  {
#ifdef FEATURE_DSAT_GOBI_DEFAULT_NETMODEL
    #error code not present
#else 
    if (ds707_crm_orig_val != -1)
    {     
      ds707_mip_static_profile.ds707.ds707_crm_val = (uint8)ds707_crm_orig_val;
      ds707_crm_orig_val = -1;
    }
#endif /* FEATURE_DSAT_GOBI_DEFAULT_NETMODEL */
  }
#else
  DATA_IS707_MSG0(MSG_LEGACY_MED,
                  "ds707_pkt_mgr_qcmip_set_crm_val not supported.");
#endif /* FEATURE_DS_MOBILE_IP */

  return;
}

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
)
{
  boolean  ret_val = TRUE;
#ifdef FEATURE_DS_MOBILE_IP
  uint8 mip_val = 0;
#endif /* FEATURE_DS_MOBILE_IP */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ds707_qcmdr_val = qcmdr_val;

#ifdef FEATURE_DS_MOBILE_IP
/*-------------------------------------------------------------------------
  If QCMIP is greater than 0, force set MDR to 3 if not and return FALSE.
-------------------------------------------------------------------------*/
  (void) dsmip_get_1x_profile_val_num(DS_QCMIP_VAL,
                                      (void *)&mip_val,MIP_NON_ARRAY);
  if ( mip_val > 0 && 
       ds707_qcmdr_val != DS_MDR_MODE_SO33_PREF )
  {
    ds707_qcmdr_val = DS_MDR_MODE_SO33_PREF;
    ret_val = FALSE;
  }
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    Re-synchronize available service options
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();

  return ret_val;
}

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
)
{
  return( &ds707_mip_static_profile );
}

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
)
{
  uint8   i = 0;
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(val == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Null pointer passed in "
                    "ds707_pkt_mgr_get_1x_profile_val");
    return ret_val;
  }
  
  switch(type)
  {
    case DS707_CRM_VAL:
      *((uint8 *)val) = ds707_mip_static_profile.ds707.ds707_crm_val;
      ret_val = TRUE;
      break;
      
    case DS707_QCQNC_VAL:
      *((uint8 *)val) = ds707_mip_static_profile.ds707.ds707_qcqnc_val;
      ret_val = TRUE;
      break;
      
    case DS707_QCSO_VAL:
      *((uint8 *)val) = ds707_mip_static_profile.ds707.ds707_qcso_val;
      ret_val = TRUE;
      break;
      
    case DS707_MDR_VAL:
      *((uint8 *)val) = ds707_qcmdr_val;
      ret_val = TRUE;
      break;
      
    case DS707_QCSCRM_VAL:
      *((uint8 *)val) = ds707_qcscrm_val;
      ret_val = TRUE;
      break;
      
    case DS707_QCTRTL_VAL:
      *((uint8 *)val) = ds707_qctrtl_val;
      ret_val = TRUE;
      break;
      
    case DS707_CTA_VAL:
      *((uint32 *)val) = ds707_cta_val;
      ret_val = TRUE;
      break;
      
    case DS707_QCCHS_VAL:
      *((uint8 *)val) = ds707_qcchs_val;
      ret_val = TRUE;
      break;
      
    case DS707_QCQOSPRI_VAL:
      *((uint8 *)val) = ds707_qcqospri_val;
      ret_val = TRUE;
      break;
      
    case DS707_CMUX_VAL:
#ifdef FEATURE_DS_IS707A_CMUX  
      if( num_elements > MAX_CMUX_VAL )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid no_of_elements in "
                        "ds707_pkt_mgr_get_1x_profile_val");
        break;
      }	

      for( i = 0; i < num_elements; i++ )
      {
        *(((uint8 *)val) + i) = ds707_cmux_val[i];
      }
#else
      *((uint8 *)val) = ds707_cmux_val;
#endif  /* FEATURE_DS_IS707A_CMUX */
      ret_val = TRUE;
      break;
      
    case DS707_QCDNS_VAL:
      if( num_elements > MAX_QCDNS_VAL )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid no_of_elements in "
                        "ds707_pkt_mgr_get_1x_profile_val");
        break;
      }	
      
      for( i = 0; i < num_elements; i++ )
      {
        *(((uint32 *)val)+i) = ds707_qcdns_val[i];
      }
      ret_val = TRUE;
      break;
      
    default:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "In ds707_pkt_mgr_get_1x_profile_val. "
                      "No match for type %d",
                      type);
  } 

  return ret_val;
}

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
  ds707_val_enum_type         type,
  void                       *val,
  uint8                       num_elements
)
{
  uint8   i = 0;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(val == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Null string passed in "
                    "ds707_pkt_mgr_set_1x_profile_val");
    return ret_val;
  }
  
  switch(type)
  {
    case DS707_CRM_VAL:
      if ( (*((uint8 *)val)) > CRM_VAL_UPPER_BOUND )
      {
        break;
      }
    	 
      ds707_mip_static_profile.ds707.ds707_crm_val = *((uint8 *)val);
      ret_val = TRUE;
      break;		
      
    case DS707_QCQNC_VAL:
      if ( (*((uint8 *)val)) > QCQNC_VAL_UPPER_BOUND )
      {
        break;
      }
      
      ds707_mip_static_profile.ds707.ds707_qcqnc_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
      
    case DS707_QCSO_VAL:
      if ( (*((uint8 *)val)) > QCSO_VAL_UPPER_BOUND )
      {
        break;
      }
    	 
      ds707_mip_static_profile.ds707.ds707_qcso_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
      
    case DS707_MDR_VAL:
      if ( (*((uint8 *)val)) > MDR_VAL_UPPER_BOUND )
      {
        break;
      }
    	 
      ds707_qcmdr_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
      
    case DS707_QCSCRM_VAL:
      if ( (*((uint8 *)val)) > QCSCRM_VAL_UPPER_BOUND )
      {
        break;
      }
    	 
      ds707_qcscrm_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
      
    case DS707_QCTRTL_VAL:
      if ( (*((uint8 *)val)) > QCTRTL_VAL_UPPER_BOUND )
      {
        break;
      }
    	 
      ds707_qctrtl_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
      
    case DS707_CTA_VAL:
      if ( (*((uint32 *)val)) > CTA_VAL_UPPER_BOUND )
      {
        break;
      }
    	 
      ds707_cta_val = *((uint32 *)val);
      ret_val = TRUE;
      break;
      
    case DS707_QCCHS_VAL:
    	 
      ds707_qcchs_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
      
    case DS707_QCQOSPRI_VAL:
      if ( (*((uint8 *)val)) > QCQOSPRI_VAL_UPPER_BOUND )
      {
        break;
      }
    	 
      ds707_qcqospri_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
      
    case DS707_CMUX_VAL:
#ifdef FEATURE_DS_IS707A_CMUX 
      if( num_elements > MAX_CMUX_VAL )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid no_of_elements");
        break;
      }	

      for( i = 0; i < num_elements; i++ )
      {
        ds707_cmux_val[i] = *(((uint8 *)val) + i);
      }
#else
      ds707_cmux_val = *((uint8 *)val);
#endif  /* FEATURE_DS_IS707A_CMUX */
      ret_val = TRUE;
      break;
      
    case DS707_QCDNS_VAL:
      if( num_elements > MAX_QCDNS_VAL )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid no_of_elements");
        break;
      }	

      for( i = 0; i < num_elements; i++ )
      {
        ds707_qcdns_val[i] = *(((uint32 *)val)+i);
      }
      ret_val = TRUE;
      break;
      
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,"No match for type %d",type);
  }

  return ret_val; 	
}

/*===========================================================================
FUNCTION DS707_PKT_MGR_UTIL_GET_THROTTLE_INFO_REQUESTED_FLAG

DESCRIPTION
  This function returns the flag which was added for the case where QMI 
  queries 3GPP2 MH about the throttling status, for which
  CM_SS_EVT_ORIG_THR_STATUS_MASK will not be set.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_mgr_util_get_throttle_info_requested_flag
(
  void
)
{
  return ds707_throttle_info_requested_flag;
}/* ds707_pkt_mgr_util_get_throttle_info_requested_flag */

/*===========================================================================
FUNCTION DS707_PKT_MGR_UTIL_SET_THROTTLE_INFO_REQUESTED_FLAG

DESCRIPTION
  This function sets the value for the flag which was added for the case where 
  QMI queries 3GPP2 MH about the throttling status, for which
  CM_SS_EVT_ORIG_THR_STATUS_MASK will not be set.

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
)
{
  ds707_throttle_info_requested_flag = flag;
}/* ds707_pkt_mgr_util_set_throttle_info_requested_flag */

#endif /* FEATURE_DATA_IS707 */
