/*===========================================================================
                  D S 7 0 7 _ I F A C E 4 _ H D L R . C
                   
                          Formerly:  dssnet4.c
                   
                   
DESCRIPTION
  The Data Services sockets network state machine implementation for IPv4. 
  This contains all the functions used by the sockets library to access the 
  network and communicate with the DS task. (This is an INTERNAL sockets
  file).
 
EXTERNALIZED FUNCTIONS
  dssnet4_sm_create()
    Creates an instance of the DSSNET4 State machine
  dssnet4_sm_post_event()
    Posts events to the DSSNET4 state machine
  dssnet4_sm_process_event()
    Processes events posted to the dssnet4 State machine. 
  is707_get_ppp_auth_info_from_nv()
    Retrieves the 1x PPP user ID and password from NV and stores them in 
    the provided PPP configuration structure.
  dssnet4_set_in_qnc_call()
    This function notifies dssnet if the mobile is in a QNC call. Called by
    ds707_pkt_mgr.
  dssnet4_override_qcmip()
    Override dsat707_qcmip value with 2.

 Copyright (c)2004-2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_iface4_hdlr.c#1 $
$Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/09/15    sd     Fallback support for CHAP challenge response calculation.
01/27/15    sd     Fixed MIP to SIP fallback handling for OMH profiles in
                   eHRPD mode.
11/18/14    skc    Removing FEATURE_UW_FMC
09/26/14    vm     Removed other iface in_use check when UE is in service  
08/05/14    ms     DSDA Phase 2 changes.
07/23/14    sd     Fix to use credentials from OMH profile in case of card mode
                   during MIP to SIP fallback.
07/18/14    hr     To support default APN
07/14/14    ms     Compiler warning fix.
07/02/14    ms     Allow MIP if HDR is in service even for P_rev < 6.
05/26/14    vm     Perform PPP resync for VSNCP cause code 9 and 11
05/21/14    ms     Move the pdn clear throttling as part of PPP event callback
                   to avoid the race condition between allocation and free.
12/23/13    vm     Stack memory optimization changes
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
06/03/13    sd     ATCoP decoupling changes
05/20/13    ssb    Reporting VSNCP error code to AP
11/02/12    ssb    Updating MIP to SIP Fallback Stats for Socket calls
08/23/12    jz     Gracefully tear down PPP upon receiving PDN ID in use
05/03/12    jee    To fix embedded/DUN failure
04/18/12    jz     Seperate old JCDMA and Simplified JCDMA 
04/12/12    jee    To propagate correct call end reason from MH
03/08/12    jee    Propagate call end reasons correctly
02/13/12    jz     Fixed PDN fallback didn't finish the state machine process 
                   and didn't set failure code 
01/25/12    msh    ppp_auth_ev_cback posts ppp close event to dssnet4 sm 
                   instead of transitioning state directly 
01/26/12    msh    Read internal auth nv item 
01/18/12    jz     PDN failure fallback list 
01/02/12    msh     Coan: Feature cleanup 
12/19/11    msh    Ignoring MIP_UP_SUCCESS_EV event in DSSNET4I_CLOSED_STATE 
11/28/11    jee    Fix to handle PPP fail reason
12/08/11    dvk    Merged PPP auth fail reason in DSSNET4 SM control block and 
                   overwrote PPP fail reason when handling PPP down event only 
                   if control block has PPP fail reason as NONE.
12/07/11    jee    To fix Arbitration issue with v4/v6 calls
12/07/11    jz     Close HDR session upon MIP authentication fail(err 67)
11/02/11    jee    To fix fallback to SIP
10/25/11    jee    To fix DCTM issue
10/28/11    ls     PDN throttle info notification through SYS IOCTL
10/04/11    jee    To handle device manager when hot swap happens
10/17/11    msh    Change to handle MIP call attempt in ehprd 
09/26/11    msh    Fixed compiler warnings 
07/07/11    vpk    Use NV params for PPP Auth when 
                   FEATURE_DATA_PS_INTERNAL_AUTH is defined
06/24/11    scb    Added support for EFS NV item is_fmc_enabled.
06/06/11    ash    Added support for PPP Partial Context
06/23/11    ls     Tearing down MIP call when PPP_CLOSE_CMD is received and
                   it is in EPC system
06/13/11    dvk    Merged support for PAP and CHAP auth failure reasons. 
05/11/11    ls     Read credential from NV/card for eHRPD profile 
05/04/11    ttv    Added changes to compute mip statistics.
03/28/11    ack    Fix to ignore certain states while processing phy_link_down_ev
03/23/11    op     Data SU API cleanup
03/15/11    mg     Global variable cleanup
04/11/11    jee    Fix to handle MIP RRP error code 129 
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
02/18/11    mg     Clear sys_info when all ifaces are down. 
02/11/11    ack    Global Variable Cleanup
01/28/11    jee    JCDMA feature cleanup
01/14/11    ack    Added PHY_LINK_DOWN_EV reg/dereg to dssnet4/6 SM
01/06/11    scb    Changes for UW FMC Feature
11/03/10    mg     Removing calls to TASKLOCK and TASKFREE. (used when updating 
                   ds707_master_session_key and len values. Since they are not 
                   being used in any module, commenting the related code for now.)
10/14/10    ss     Adding debug messages before ASSERT.
10/13/10    op     Migrated to MSG 2.0 macros
10/12/10    mg     Changes for error code propogation to ps_iface
09/27/10    ls     Use ERR_FATAL() when failed to allocate PPP events
09/07/10    op     IPv6 enable/disable feature for HRPD
08/24/10    ms     Set DNS addresses to default in UP state if they are empty.
08/10/10    rp     Fallback to SIP in resync state on MIP failure  
07/28/10   ttv     Merged the fix for the issue of not reading MIP active 
                   profile index from NV item NV_DS_MIP_ACTIVE_PROF_I during
                   call establishment.
07/26/10    gc     Added code to report call error due to PPP failure
07/19/10    jee    Added default value when NV_EHRPD_AUTH_IN_USIM_I read fails
07/18/10    mg     PDN throttling feature for eHRPD
06/09/10    ls     In dssnet4i_process_mip_dereged_ev(), we need to handle
                   MIP deregistration event while in NET_UP state
05/04/10    mg     Dual IP feature
04/19/10    gc     klocwork fixes
04/05/10    ls     Use ppp_event_handle to ensure PPP registeration that only
                   happens once
03/24/10    ls     dssnet4_dereg_phys_link_events() shall be called before
                   calling ps_iface_down_ind_ex() to avoid the race condition
03/15/10    ls     Silent Redial Feature
03/10/10    ls     When PPP invokes PPP_DOWN event through call back, it shall
                   update EPC sys info before posting DSSNET4_LINK_DOWN_EV
03/03/10    ms     Changes to read the auth credentials from iface For NDIS call
                   when legacy RUIM is used.
01/06/10    sn     Made changes to make sure that PPP events are registered 
                   only once per call.
12/21/09    mga    Merged changes to decide if EAP parameters should be read 
                   from card or NV, code rearrangement changes.
12/10/09    ps     Mainlining OMH feature based on new profile management
                   design.
09/18/09    ss     DataCommon CMI SU level impacts.
09/04/09    ss     CMI SU level modifications.
07/29/09    spn    Remember if the call was made on eHRPD or not.
07/27/09    spn    Added functionality to tear down the call after physlink 
                   up if the call is not allowed based on the profile 
                   selected by the application.
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
05/13/09    ls     Pass all MIP rrp code to PS
04/23/09    ls     Add the support to report MIP rrp code
04/22/09    mg     In ds707_iface4_hdlr_get_eap_parameter(), formed NAI from
                   new IMSI NV for eap_user_id. (EAP-AKA` feature)
03/12/2009 ssh/spn PPP VSNCP interface changes
03/12/09   ss      Moved the macro definitions PPP_DEFAULT_SN_PASSWORD and 
                   PPP_DEFAULT_SN_PASSWORD_LEN from here to header file.
03/04/09   sa      AU level CMI modifications.
02/11/2009  ls     In dssnet4i_handle_ppp_ev_cback(), it will verify IP 
                   address before posting event.  If IP addr is 0.0.0.0 for 
                   IPV4 or any invalid address, post DSSNET4_PPP_CLOSE_EV. 
                   Otherwise,  it posts DSSNET4_LINK_UP_WITH_SIP_EV.
02/09/09    spn    Moved ppp configure and vsncp configure for eHRPD to 
                   dsgen_iface_hdlr. 
12/05/08    spn    Populate User Data for AUTH CB. Propagate Profile info
                   into VSNCP config.
12/05/08    sk     EAP support for EHRPD
11/26/08    psng   Fixed compilation error.
11/24/08    psng   Added support to use legacy NV items if number of valid 
                   SIP profiles is zero.
11/20/08    psng   Fixed multiple profile CHAP authentication failure when
                   MS is in RUIM mode.
10/07/08    cs     Compiler Warning fix.
11/05/08    psng   Made Changes to use legacy credentials for MIP and MIP to 
                   SIP fallback when OMH is enabled.
23/10/08    psng   Fixed the compilation errors on JCDMA build.
10/07/08    psng   Replaced legacy F3 messages with 707 messages.
09/03/08    sc     Fix to enable UM PPP auth on a dual stack phone even if  
                   it is a MIPv4 call.
08/08/08    hs     Wrapped FEATURE_DS_MOBILE_IP_DEREG under
                   FEATURE_DS_MOBILE_IP
07/25/08    hs     Fixed compilation warnings
06/27/08    rt     Added support for IWLAN_3GPP2 and 1x-v4 interface mutual
                   exclusion.
06/17/08    psng   Added support for multiple app profiles for OMH.
05/23/08    ms     Fixed crash when handling unexpected MIP_FAILURE_EV
02/12/08    sc     Added support to propagate CM call end reason to IFACE.
01/22/08    scb/sy Mainlined features under FEATURE_JCDMA.
12/04/07    yz     Moved DSSNET4 MIP event registration out of 
                   dssnet4_sm_create into dssnet4i_process_link_up_with_mip_ev
11/15/07    sc     Added support to send PPP auth starting event to IFACE
10/23/07    ms     Featurized MIP code under FEATURE_MOBILE_IP
10/22/07    ms     Clean MIP call before fall back to SIP.
09/28/07    ms     Fixed DCTM reporting for MIP->SIP fallback for PPP failures.
08/21/07    vp     MIP CCoA related fixes
07/02/07    sc     Added support for Application Profile Specific 
                   requirements
02/12/07    az     Support to request VJ compression during PPP negotiation
02/09/06    as     Ignoring MIP_UP_SUCCESS event in WAITING_FOR_DEREG state
12/07/06    as     Updated SM for MIP deregistration
10/27/05    az     Changed the check for IPv6 qnc_call in 
                   dssnet4i_transition_state. Submitted for Irfan Khan.
10/13/05    az     Support for IPv6
09/27/06    sk     Added one sec connection delay timer
09/11/06    as     Fixed DCTM reporting for MIP->SIP fallback
03/02/06    ssh    Added support for default SN PPP password.
                   (FEATURE_DATA_PPP_DEFAULT_PASSWD)
03/02/06   az      Whenever PPP_abort() happens, added a call to silently 
                   cleanup all secondary flows.
02/14/06    sy     Populated the rx_sig_f_ptr to be used for HDLC library 
                   with QOS.
01/30/06   az      Fixed featurization and compilation warning
01/30/06   az      Created a handler for processing PPP down event 
                   triggered from the network.
1/17/06    squ/gr  Set hdlc_mode to HDLC_SW when features QoS and BCMCS are
                   not defined
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
10/31/05   as/kvd  updated ppp status report on PPP auth failure.
10/17/05   as/kvd  Added seperate callback for PPP auth protocol
10/05/05    mct    Fixed issue wherein existing secondary flows are not 
                   properly deleted during app initiated ppp close.
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP 
                   resync during a handup from 1x to DO
08/15/05    vrk    Changed hdlc_mode to HDLC_HW_PREF from HDLC_SW
07/29/05    kvd    Resolved some MIP featurization bugs.
04/29/05    kvd    Added hooks for reporting MIP, PPP fail reasons to DCTM.
04/29/05    vrk    Created Diag events to enable automated testing
4/20/05     mct    Removed refs to phys_link down, as dormancy is ignored.
04/19/05    mct    Added changes to support iface sharing w/v6 to ensure iface
                   always waits to come down after phys_link.
10/13/04    vp     Change the dsat_qcdns_val[0/1] to network order before 
                   assignment to default DNS addresses of the iface as it is
                   expected in network order now. 
12/07/04    kvd    Ignore LINK_UP_WITH_MIP/SIP while in LINK_CLOSING state.
12/01/04    sk	   Added code to support the state transition to 
                   DSSNET4I_LINK_OPENING_WITH_SIP_STATE if 
                   FEATURE_DS_MOBILE_IP is turned off
09/20/04    vas    Merged from the Sept Dev branch
08/17/04    kvd    Tear down the call if we come up with MIP, while  in
                   WAITING_FOR_LINK_UP_WITH_SIP state.
08/12/04    kvd    transition to CLOSED on link_down if in
                   DSSNET4I_LINK_OPENING_WITH_SIP_STATE state
07/13/04    kvd    Register & deregister PPP event callbacks on transition to
                   WAITING_FOR_LINK_OPENING_WITH_MIP/SIP & CLOSED 
                   respectively.
06/23/04    kvd    Added  new event MSM_PHYS_IFACE_UP to post to MIP metasm
                   on phys_link_up before we try MIP. MIP metasm uses this
                   event to do a deselect on MIP ICMP sockets to hold 
                   reading Router advs until MIP metasm is UP.
06/23/04    kvd    Reset  mip configuration on DSSNET close.
06/22/04    kvd    In ppp_open check if mip session is already configured.
                   If configured try to bring up laptop MIP call, else it is
                   sockets MIP call.
05/06/04    kvd    Resync PPP (and don't restart PPP) if PPP is already
                   established when falling back from MIP to SIP.
04/30/04    aku    Flush default route on transitioning to CLOSED state.
04/30/04    sv     Modified process_ppp_close_cmd_ev function to transition
                   the dssnet state to CLOSED_STATE when it is currently in 
                   CLOSED_STATE.
04/27/04    kvd    Added MIP performance call-setup-time report hooks.
04/27/04    kvd    give ppp_configuring indication while doing ppp_resync.
04/27/04    kvd    Added new API dssnet_set_ipcp_dns_opt.
04/19/04    jd     Fix compile errors when FEATURE_DS_MOBILE_IP turned off
04/16/04    jd     Ignore MIP_UP_SUCCESS event in NET_UP state (since this
                   occurs on every normal MIP re-reg without handoff).
                   Pass iface pointer to MIP instead of iface ID.
                   Remove dependencies on ps_mip_compat.h
04/16/04    jd/mt  Reenable iface tx flow on transition to WAIT_FOR_MIP_REG
03/20/04    ak     Changed name at top of file, to match file name change.
03/18/04    aku    Register for Phys link down and gone indications when
                   common dssnet bringup function is call and deregister
                   when transitioning to CLOSED state.
03/12/04    aku    Register for phys link up indication only when common 
                   dssnet bringup cmd is called. Deregister this indication
                   when processing the PHYS_LINK_UP_EV.
02/03/04    mct    Fixed dssnet4i_handle_ppp_ev_cback where accidently  
                   posting MIP callback in SIP. Also fixed 
                   dssnet4i_transition_state when link opening under SIP, 
                   ppp_start wasn't being called after ncp setup.
01/13/04    aku    Created Module
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_EPC_HANDOFF
#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
#endif /* FEATURE_EPC_HANDOFF */

#include "dstask.h"
#include "dstaski.h"
#include "dsat_v.h"
#include "data_err_suppr.h"

#include "ps_iface.h"
#include "ps_ifacei_utils.h"
#include "ds707_iface4_hdlr.h"
#include "dsgen_iface_hdlr.h"
#include "amssassert.h"
#include "msg.h"
#include "data_msg.h"
#include "ps_ppp.h"
#include "ds707_extif.h"
#include "nv_items.h"
#include "dshdr_dhcp_mgr.h"
#include "ds707_pkt_mgr.h"
#include "ds707_pkt_mgr_iface.h"
#include "ds3gcfgmgr.h"
#include "ds_flow_control.h"
#include "ps_phys_link.h"
#include "ps_route.h"
#include "ds_system_heap.h"
#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#include "dsat707vendctab.h"
#include "dsmip_v.h"
#include "dsmip_parse.h"
#include "dsmip_metasm.h"
#include "ds707_p_rev.h"
#include "ds707.h"
#include "ds707_event_defs.h"
#include "ds707_drs.h"

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#endif /* FEATURE_DS_MOBILE_IP */
#include "ds_1x_profile.h"
                                  
#ifdef FEATURE_CALL_THROTTLE
#include "dsdctm.h"
#endif /* FEATURE_CALL_THROTTLE */

#include "ds707_sec_pkt_mgr.h"
#include "ds707_data_session_profile.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "ds707_roaming.h"
#include "ds707_sys_chg_hdlr.h"
#include "ds707_pdn_context.h"

#ifdef FEATURE_EHRPD
#include "ps_ppp_defs.h"
#include "ps_iface_defs.h"
#include "ps_ppp_ncp.h"
#include "ps_ppp.h"
#include "ps_ppp_vsncp_3gpp2.h"
#include "ps_svc.h"
#include "ps_eap.h"
#include "dsutil.h"
#include "ds_ehrpd_pdn_mgr.h"

#include <stringl/stringl.h>
#include "ds707_devmgr.h"
#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "ds3gmmgsdiif.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_EHRPD */
#include "data_err_suppr.h"
#include "ds707_pkt_mgr_iface.h"

#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_EHRPD
#define EHRPD_EFS_FILE_NAME "/eHRPD/initial_sequence_number.txt"
#endif /* FEATURE_EHRPD */
#include "ps_ppp_defs.h"
#include "ps_crit_sect.h"
#include "nvruimi.h"
#include "modem_mem.h"
#include "mcfg_fs.h"

#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
#include "iwlan_iface_ext.h"
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

#include "mc.h"

/*lint -save -e655 Reason for suppressing error 655*/
/*lint -save -e715 Reason for suppressing error 715*/
/*lint -save -e641 Reason for suppressing error 641*/

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  #include "ds707_pkt_classifier.h"
#endif /* FEATURE_DATA_PKT_CLASSIFIER */

#include "ds3gdsdif.h"

#ifdef FEATURE_DS_MOBILE_IP

/*-------------------------------------------------------------------
  MIP Error code mask
-------------------------------------------------------------------*/
  #define DSSNET4_MIP_ERR_CODE_MASK 0x00010000

#endif /* FEATURE_DS_MOBILE_IP */
/*-------------------------------------------------------------------
  PPP Error code mask
-------------------------------------------------------------------*/
  #define DSSNET4_PPP_ERR_CODE_MASK 0x00070000

/*---------------------------------------------------------------------------
  Mask for VSNCP error codes in ps iface net down reasons
---------------------------------------------------------------------------*/
  #define DSSNET4_VSNCP_ERR_CODE_MASK 0x00080000

/*---------------------------------------------------------------------------
  AKA algo seuence no. item size
---------------------------------------------------------------------------*/
  #define AKA_ALGO_SEQ_NO_ITEM_SIZE 17

/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
MACRO DSSNET4_SMI_IS_VALID

DESCRIPTION
  Make sure the SM instance passed in is valid
---------------------------------------------------------------------------*/
#define DSSNET4I_IS_VALID( instance_ptr )                                   \
  (((instance_ptr) != NULL) && ((instance_ptr->this_sm_ptr) == instance_ptr))

#ifdef FEATURE_EHRPD
#define DS707_EAP_USER_COOKIE 0x32

#if 0
/*---------------------------------------------------------------------------
 These 2 globals are currently not used anywhere, and hence commenting
 them out                                                                    
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 Master session key is the value returned by EAP protocol after EAP success.
 This value is stored here and Data can use it.
---------------------------------------------------------------------------*/
uint8 ds707_master_session_key = 0;
uint16 ds707_master_session_key_len =0;
#endif 

/*---------------------------------------------------------------------------
  EAP_USER_DATA is the cookie passed to PPP layer during ppp configuration. 
  When EAP succeeds, the EAP returns the MASTER SESSION KEY. The callback
  is authenticated in this module through this cookie.
---------------------------------------------------------------------------*/
static uint8 ds707_eap_user_data = DS707_EAP_USER_COOKIE; /* Cookie*/ 
#endif /* FEATURE_EHRPD */



/*===========================================================================

                    FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
static void dssnet4i_transition_state
(
  dssnet4_sm_cb_type  *dssnet_sb_ptr,
  dssnet4i_state_type new_state
);

static void dssnet4i_handle_ppp_ev_cback
(
  ppp_dev_enum_type   ppp_dev,                   /* PPP Device             */
  ppp_protocol_e_type protocol,                  /* PPP Protocol           */
  ppp_event_e_type    ppp_event,                 /* Down/Up/Resync         */
  void               *user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  reason
);
static void dssnet4i_handle_ppp_auth_ev_cback
(
  ppp_dev_enum_type   ppp_dev,                   /* PPP Device             */
  ppp_protocol_e_type protocol,                  /* PPP Protocol           */
  ppp_event_e_type    ppp_event,                 /* SUCCESS/FAIL           */
  void                *user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
);

static void dssnet4i_handle_phys_link_ev_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
);

static void dssnet4i_process_phy_link_up_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_phy_link_gone_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_phy_link_down_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_link_up_with_sip_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_link_down_ev
(
  dssnet4_sm_cb_type *instance_ptr,            /* dssnet4 instance ptr      */
  ppp_protocol_e_type protocol,
  ppp_fail_reason_e_type  reason
);

static void dssnet4i_process_link_resync_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_ppp_close_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_flow_deleted_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_auth_starting_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_configure_ppp
(  
  dssnet4_sm_cb_type *dssnet4_instance_ptr,  /* dssnet4_instance_ptr       */
  ds707_data_session_profile_info_type *data_sess_profile_info,
                                             /* Data Session Profile Info  */
  boolean mip_call,                          /* Is this a MIP call ?       */
  ppp_dev_opts_type *ppp_config_ptr          /* PPP config block           */
);  

#ifdef FEATURE_DS_MOBILE_IP
static void dssnet4i_process_link_up_with_mip_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_mip_up_success_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);

static void dssnet4i_process_mip_failure_ev
(
  dssnet4_sm_cb_type *instance_ptr,           /* dssnet4 instance ptr      */
  mip_fail_reason_e_type  reason,
  byte               mip_rrp_code
);

#ifdef FEATURE_DS_MOBILE_IP_DEREG
static void dssnet4i_process_mip_dereged_ev
(
  dssnet4_sm_cb_type *instance_ptr           /* dssnet4 instance ptr      */
);
#endif /* FEATURE_DS_MOBILE_IP_DEREG */

static void dssnet4i_handle_mip_ev_cback
(
  mip_event_e_type        mip_event,          /* Sucees/Fail               */
  mip_fail_reason_e_type  fail_reason,
  byte                    mip_rrp_code,
  void                    *user_data
);
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_EHRPD
static void dssnet4i_handle_eap_auth_ev_cback
(
 uint8 *master_session_key,                 /* Master Session Key          */
 uint16 master_session_key_len,             /* Length of Master Session Key*/
 ps_ppp_eap_result_enum_type ps_eap_result, /* Result of EAP               */
 void *eap_user_data,                        
 uint64 *aka_algo_seqnum_arr,               /* Sequence Numbers            */
 uint8 aka_algo_seqnum_arr_num_elements     /* Number of Sequence Numbers  */ 
);

static boolean ds707_iface4_hdlr_form_eap_nai
(   
  nv_ehrpd_imsi_type   *ehrpd_imsi,
  ppp_auth_info_type *ppp_auth_info_ptr
);
#endif /* FEATURE_EHRPD */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSNET4_SM_CREATE()

DESCRIPTION
  This function is is used to create an instance of the dssnet4 state machine 
  control block. Called by the mode specific handler. The mode specific 
  handler allocates memory for the control block as well as the ps iface.

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnet4_sm_create
(
  dssnet4_sm_cb_type *instance_ptr,           /* dssnet4 instance ptr      */
  ps_iface_type      *ps_iface_ptr           /* Interface Ptr              */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Make sure the DSSNET4 control block and the ps iface are not NULL.
  -------------------------------------------------------------------------*/
  if ((instance_ptr == NULL) || ( ps_iface_ptr == NULL))
  {
    DATA_ERR_FATAL("instance_ptr or ps_iface_ptr is NULL");
    return -1;
  }

  if(instance_ptr->this_sm_ptr == instance_ptr)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, "DSSNet4 SM 0x%x already initialized", 
                     instance_ptr);
    return 0;
  }

  instance_ptr->state                   = DSSNET4I_CLOSED_STATE;
  instance_ptr->ps_iface_ptr            = ps_iface_ptr;
  instance_ptr->qnc_call                = FALSE;
  instance_ptr->te2_call_override_qcmip = FALSE;
  instance_ptr->is_te2_client           = FALSE;
  instance_ptr->is_ehrpd_call           = FALSE;
  instance_ptr->mip_call_started        = FALSE;
#ifdef FEATURE_DS_MOBILE_IP
  instance_ptr->want_mip                = FALSE;
  instance_ptr->will_sip                = FALSE;
  instance_ptr->work_mip                = FALSE;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  instance_ptr->dssnet_started_mip_call = FALSE;
#endif /* FEATURE_DS_MOBILE_IP_PERF */
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
   Initialize the PPP resync done callback function ptr to NULL
  -------------------------------------------------------------------------*/
  instance_ptr->ppp_resync_done_cb      = NULL;

  /*-------------------------------------------------------------------------
   Initialize the PPP event handler to NULL
  -------------------------------------------------------------------------*/
  instance_ptr->ppp_event_handle        = NULL;

  /*-------------------------------------------------------------------------
    Allocate buffer for Phys link Up Event. Note that the phys link up event 
    is registered only when the common dssnet_start() invokes
    the dssnet4_reg_phys_link_up_event() function.
  -------------------------------------------------------------------------*/
  instance_ptr->phys_link_up_cbuf = ps_iface_alloc_event_cback_buf(
                                          dssnet4i_handle_phys_link_ev_cback,
                                          instance_ptr);

  if (instance_ptr->phys_link_up_cbuf == NULL) 
  {
    ERR_FATAL("No PS IFACE Event Buffers Avail",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Allocate buffer for Phys link Gone Event.    
  -------------------------------------------------------------------------*/
  instance_ptr->phys_link_gone_cbuf = ps_iface_alloc_event_cback_buf(
                                          dssnet4i_handle_phys_link_ev_cback,
                                          instance_ptr);

  if (instance_ptr->phys_link_gone_cbuf == NULL) 
  {
    ERR_FATAL("No PS IFACE Event Buffers Avail",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Allocate buffer for Phys link Down Event.    
  -------------------------------------------------------------------------*/
  instance_ptr->phys_link_down_cbuf = ps_iface_alloc_event_cback_buf(
                                          dssnet4i_handle_phys_link_ev_cback,
                                          instance_ptr);

  if (instance_ptr->phys_link_down_cbuf == NULL) 
  {
    ERR_FATAL("No PS IFACE Event Buffers Avail",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Allocate buffer for iface flow deleted ev.    
  -------------------------------------------------------------------------*/
  instance_ptr->iface_flow_deleted_cbuf = ps_iface_alloc_event_cback_buf(
                                          dssnet4i_handle_phys_link_ev_cback,
                                          instance_ptr);

  if (instance_ptr->iface_flow_deleted_cbuf == NULL) 
  {
    ERR_FATAL("No PS IFACE Event Buffers Avail",0,0,0);
  }

  
#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Initialize MIP rrp code
    -------------------------------------------------------------------------*/
  dssnet4i_clear_mip_rrp_code(instance_ptr);
#endif /* FEATURE_DS_MOBILE_IP */
  instance_ptr->call_end_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  
  /*-------------------------------------------------------------------------
    This state machine is now initialized
  -------------------------------------------------------------------------*/
  instance_ptr->this_sm_ptr = instance_ptr;
  return 0;

} /* dssnet4_sm_create() */

/*===========================================================================
FUNCTION DSSNET4_SM_POST_EVENT()

DESCRIPTION
  This function is used to post events to the dssnet4 state machine by other 
  modules.  

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnet4_sm_post_event
(
  dssnet4_sm_cb_type     *instance_ptr,       /* dssnet4 instance ptr      */
  dssnet4_sm_event_type   event,             /* dssnet4 event type         */
  void*                   event_data        
)
{
  ds_cmd_type         *cmd_ptr;                             /* ps cmd type */
  ds_cmd_enum_type    cmd_id;
  ds_mip_ppp_status   *mip_ppp_status;
  qword                ts_null;
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
#endif
  ds_mip_ppp_status   *mip_ppp_status_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(event < DSSNET4_MAX_EV);

  /*-------------------------------------------------------------------------
    Verify that this state machine has been initialied.
  -------------------------------------------------------------------------*/
  if( !DSSNET4I_IS_VALID(instance_ptr) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Tried posting to invalid DSSNet4 instance");
    return -1;
  }

  DATA_IS707_MSG3( MSG_LEGACY_MED, 
                   "Posting event %d to DSSNet4(0x%x) in state %d.",
                   event,
                   instance_ptr,
                   instance_ptr->state);


  /*------------------------------------------------------------------------ 
    Initialize ts_null
  ------------------------------------------------------------------------*/
  qw_set(ts_null, 0, 0);

  /*-------------------------------------------------------------------------
    Initialize command name to DS_MIN_CMD.
  -------------------------------------------------------------------------*/
  cmd_id = DS_MIN_CMD;

  /*-------------------------------------------------------------------------
    Based on the event type, set the appropriate PS cmd 
  -------------------------------------------------------------------------*/
  switch(event)
  {
    case DSSNET4_PHY_LINK_UP_EV:
      cmd_id = DS_CMD_DSSNET4_PHY_LINK_UP_EV;
      break;
  
    case DSSNET4_PHY_LINK_GONE_EV:
      cmd_id = DS_CMD_DSSNET4_PHY_LINK_GONE_EV;
      break;

    case DSSNET4_PHY_LINK_DOWN_EV:
      cmd_id = DS_CMD_DSSNET4_PHY_LINK_DOWN_EV;
      break;
  
    case DSSNET4_LINK_UP_WITH_SIP_EV:
      cmd_id = DS_CMD_DSSNET4_LINK_UP_WITH_SIP_EV;
      break;
  
    case DSSNET4_LINK_DOWN_EV:
      cmd_id = DS_CMD_DSSNET4_LINK_DOWN_EV;
      break;
  
    case DSSNET4_LINK_RESYNC_EV:
#if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MOBILE_IP_PERF)
      if (!dshdr_get_mobile_ip_uses_ccoa()) 
      {      
        /*---------------------------------------------------------------------
          Notify M.IP call performance that Um PPP negotiation is starting.
        ---------------------------------------------------------------------*/
        mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_UM_PPP_DELAY);
        if (qw_cmp(mip_perf_info_ptr->ts_start, ts_null) == 0)
        {
          mip_perf_delay(mip_perf_info_ptr,
                         PERF_UM_PPP_DELAY,
                         PERF_START_TS);
        }
      }
#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DS_MOBILE_IP_PERF */

      cmd_id = DS_CMD_DSSNET4_LINK_RESYNC_EV;
      break;
  
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4_LINK_UP_WITH_MIP_EV:
      cmd_id = DS_CMD_DSSNET4_LINK_UP_WITH_MIP_EV;
      break;
  
    case DSSNET4_MIP_UP_SUCCESS_EV:
      cmd_id = DS_CMD_DSSNET4_MIP_UP_SUCCESS_EV;
      break;
  
    case DSSNET4_MIP_FAILURE_EV:
      cmd_id = DS_CMD_DSSNET4_MIP_FAILURE_EV;
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4_MIP_DEREGED_EV:
      cmd_id = DS_CMD_DSSNET4_MIP_DEREGED_EV;
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    case DSSNET4_PPP_CLOSE_EV:
      cmd_id = DS_CMD_DSSNET4_PPP_CLOSE_EV;
      break;
      
    case DSSNET4_FLOW_DELETED_EV:
      cmd_id = DS_CMD_DSSNET4_FLOW_DELETED_EV;
      break;

    case DSSNET4_PPP_AUTH_STARTED_EV:
      cmd_id = DS_CMD_DSSNET4_AUTH_STARTED_EV;
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return -1;
  } /* switch(event) */

  /*-------------------------------------------------------------------------
    Enqueue cmd with DS task if it has been set to a valid value 
  -------------------------------------------------------------------------*/
  if (cmd_id != DS_MIN_CMD)
  {
    if(
#ifdef FEATURE_DS_MOBILE_IP
       ( event == DSSNET4_MIP_FAILURE_EV ) ||
#endif /* FEATURE_DS_MOBILE_IP */
       ( event == DSSNET4_LINK_DOWN_EV))
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_mip_ppp_status));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return -1;
      }

      mip_ppp_status_ptr = (ds_mip_ppp_status *)(cmd_ptr->cmd_payload_ptr);

      if (event_data != NULL)
      {
        mip_ppp_status = (ds_mip_ppp_status *)(event_data);
        mip_ppp_status_ptr->ppp_protocol = mip_ppp_status->ppp_protocol;
        mip_ppp_status_ptr->ppp_fail_reason = mip_ppp_status->ppp_fail_reason;
#ifdef FEATURE_DS_MOBILE_IP
        if (!dshdr_get_mobile_ip_uses_ccoa()) 
        {
          mip_ppp_status_ptr->mip_fail_reason = mip_ppp_status->mip_fail_reason;
        }

        mip_ppp_status_ptr->mip_rrp_code = mip_ppp_status->mip_rrp_code;
#endif /* FEATURE_DS_MOBILE_IP */
        mip_ppp_status_ptr->dssnet_instance = mip_ppp_status->dssnet_instance;
      }

      else
      {
        mip_ppp_status_ptr->ppp_protocol = PPP_PROTOCOL_LCP;
        mip_ppp_status_ptr->ppp_fail_reason = PPP_FAIL_REASON_NONE;
#ifdef FEATURE_DS_MOBILE_IP
        if (!dshdr_get_mobile_ip_uses_ccoa()) 
        {
          mip_ppp_status_ptr->mip_fail_reason = 
                                                     MIP_FAIL_REASON_NONE;
        }

        mip_ppp_status_ptr->mip_rrp_code = MIP_RRP_CODE_SUCCESS;
#endif /* FEATURE_DS_MOBILE_IP */
        mip_ppp_status_ptr->dssnet_instance = (void *)instance_ptr;
      }
    }

    else
    {
      if( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL )
      {
        ASSERT(0);
        return -1;
      }

      cmd_ptr->cmd_payload_ptr = (void *)instance_ptr;
    }

    cmd_ptr->hdr.cmd_id = cmd_id;
    ds_put_cmd( cmd_ptr );
  }

  return 0;
} /* dssnet4_sm_post_event() */

/*===========================================================================
FUNCTION DSSNET4_SM_PROCESS_EVENT()

DESCRIPTION
  This function is used to process events posted to the dssnet4 state 
  machine. 

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_sm_process_event
(
  ds_cmd_type *cmd_ptr                          /* Pointer to ds_cmd_type  */
)
{
  dssnet4_sm_cb_type  *instance_ptr       = NULL;
  ds_mip_ppp_status   *mip_ppp_status_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    DATA_ERR_FATAL("cmd_ptr or cmd_payload_ptr is NULL");
    return;
  }

  if( ! ((cmd_ptr->hdr.cmd_id == DS_CMD_DSSNET4_LINK_DOWN_EV)
#ifdef FEATURE_DS_MOBILE_IP
         || (cmd_ptr->hdr.cmd_id == DS_CMD_DSSNET4_MIP_FAILURE_EV)
#endif /* FEATURE_DS_MOBILE_IP */
    ))
  {
    /*lint -save -e740 -e826 suppressing error 740*/
    instance_ptr = (dssnet4_sm_cb_type *)(cmd_ptr->cmd_payload_ptr);
    /*lint -restore*/
  }
  else
  {
    mip_ppp_status_ptr = (ds_mip_ppp_status *)(cmd_ptr->cmd_payload_ptr);
	  instance_ptr = mip_ppp_status_ptr->dssnet_instance;
  }

  switch (cmd_ptr->hdr.cmd_id) 
  {
    case DS_CMD_DSSNET4_PHY_LINK_UP_EV:
      dssnet4i_process_phy_link_up_ev(instance_ptr);
      break;
  
    case DS_CMD_DSSNET4_PHY_LINK_GONE_EV:
      dssnet4i_process_phy_link_gone_ev(instance_ptr);
      break;

    case DS_CMD_DSSNET4_PHY_LINK_DOWN_EV:
      dssnet4i_process_phy_link_down_ev(instance_ptr);
      break;
  
    case DS_CMD_DSSNET4_LINK_UP_WITH_SIP_EV:
      dssnet4i_process_link_up_with_sip_ev(instance_ptr);
      break;
  
    case DS_CMD_DSSNET4_LINK_DOWN_EV:
      dssnet4i_process_link_down_ev(
                              instance_ptr,
                              mip_ppp_status_ptr->ppp_protocol,
                              mip_ppp_status_ptr->ppp_fail_reason);
      break;
    
    case DS_CMD_DSSNET4_LINK_RESYNC_EV:
      dssnet4i_process_link_resync_ev(instance_ptr);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DS_CMD_DSSNET4_LINK_UP_WITH_MIP_EV:
      dssnet4i_process_link_up_with_mip_ev(instance_ptr);
      break;
    
    case DS_CMD_DSSNET4_MIP_UP_SUCCESS_EV:
      dssnet4i_process_mip_up_success_ev(instance_ptr);
      break;
    
    case DS_CMD_DSSNET4_MIP_FAILURE_EV:
      dssnet4i_process_mip_failure_ev(instance_ptr,
                                      mip_ppp_status_ptr->mip_fail_reason,
                                      mip_ppp_status_ptr->mip_rrp_code);
      break;

#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DS_CMD_DSSNET4_MIP_DEREGED_EV:
      dssnet4i_process_mip_dereged_ev(instance_ptr);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
  
    case DS_CMD_DSSNET4_PPP_CLOSE_EV:
      dssnet4i_process_ppp_close_ev(instance_ptr);
      break;
      
    case DS_CMD_DSSNET4_FLOW_DELETED_EV:
      dssnet4i_process_flow_deleted_ev(instance_ptr);
      break;
  
    case DS_CMD_DSSNET4_AUTH_STARTED_EV:
      dssnet4i_process_auth_starting_ev(instance_ptr);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity check
      ---------------------------------------------------------------------*/
      break;
  }/* end switch() */
              
  return;
} /* dssnet4_sm_process_event() */


#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION DSSNET4_REG_MIP_EVENT()

DESCRIPTION
  This function is used to register MIP events. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_reg_mip_event
(
  dssnet4_sm_cb_type *instance_ptr
)
{
  /*-------------------------------------------------------------------------
    Make sure the DSSNET4 control block is not NULL.
  -------------------------------------------------------------------------*/
  if (instance_ptr == NULL)
  {
    DATA_ERR_FATAL("DSSNET4 instance_ptr is NULL");
    return;
  }
        
  if (!dshdr_get_mobile_ip_uses_ccoa()) 
  {
    /*-----------------------------------------------------------------------
      Register for MIP SUCCESS / FAIL events.
    -----------------------------------------------------------------------*/
    instance_ptr->mip_event_handle = mip_event_alloc(
                                                dssnet4i_handle_mip_ev_cback,
                                                instance_ptr);
    if( NULL == instance_ptr->mip_event_handle )
    {
      ERR_FATAL( "Unable to acquire a MIP event handle", 0, 0, 0 );
    }
    else
    {
      (void) mip_event_register( instance_ptr->mip_event_handle,
                                 MIP_FAILURE_EV_MASK  
                               ); 
      (void) mip_event_register( instance_ptr->mip_event_handle,
                                 MIP_SUCCESS_EV_MASK
                               );  
    #ifdef FEATURE_DS_MOBILE_IP_DEREG
      (void) mip_event_register( instance_ptr->mip_event_handle,
                                 MIP_DEREGED_EV_MASK
                               );                           
    #endif /* FEATURE_DS_MOBILE_IP_DEREG */
    }
  }
}
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION IS707_GET_PPP_AUTH_INFO_FROM_NV

DESCRIPTION
  This function retrieves the 1x PPP user ID and password from NV
  and stores them in the provided PPP configuration structure.

  The feature EHRPD and OMH does not co-exist at present. The code is
  written on this assumption.  We shall re-vist this code when EHRPD, 
  OMH and KDDI will be supported at the same time.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'nv_get' results in a Wait and watchdog kicking
===========================================================================*/
void is707_get_ppp_auth_info_from_nv
(
  ppp_auth_info_type *ppp_auth_info_ptr,
  nv_stat_enum_type (* nv_get)(nv_items_enum_type, nv_item_type *, sys_modem_as_id_e_type)
)
{
  nv_stat_enum_type    nv_status;          /* status from NV call          */
  nv_item_type         nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check if the ppp_auth_info_ptr is valid 
  -------------------------------------------------------------------------*/
  if (NULL == ppp_auth_info_ptr)
  {
     ASSERT(0);
     return;
  }

/*lint -save -e831 suppressing error 831*/
#ifdef FEATURE_EHRPD
  if ( ds707_pkt_is_ehrpd_mode_of_operation() )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "eHRPD mode of Operation, get eap parameters" ); 
    /*-----------------------------------------------------------------------
     If eHRPD feature is defined and current serving system is eHRPD, the 
     authentication mechanism is EAP. The NAI and SHARED secret are read from
     NV.

     Note: This funcation is called by all types of call (i.e. embedded v4, 
     embedded v6, laptop, china telcom (prop.), etc. This configuration also
     allows those calls to configure EAP as long as eHRPD is negotiated.
    -----------------------------------------------------------------------*/
    ds707_iface4_hdlr_get_eap_parameter(ppp_auth_info_ptr);
#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
    return; 
  }
#endif /* FEATURE_EHRPD*/
 
 /*-------------------------------------------------------------------------
    Read the PPP User ID from NV. 
  -------------------------------------------------------------------------*/
  nv_status = nv_get( NV_PPP_USER_ID_I, &nv_item, ds707_curr_subs_id() );

  /*-------------------------------------------------------------------------
    If NV read succeeds, load the user ID into the provided ppp config
  -------------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S)
  {
    ppp_auth_info_ptr->user_id_len = nv_item.ppp_user_id.user_id_len;
    memscpy( ppp_auth_info_ptr->user_id_info,
	     PPP_MAX_USER_ID_LEN,
             (char *)nv_item.ppp_user_id.user_id,
            ppp_auth_info_ptr->user_id_len );
  } 
  /*-------------------------------------------------------------------------
    If NV was never written then the length of the User_id is set to 0.
    This will ensure that no user ID is included in the AP response.
  -------------------------------------------------------------------------*/
  else
  {
    /*-----------------------------------------------------------------------
      Set the user_id length to 0.
    -----------------------------------------------------------------------*/
    ppp_auth_info_ptr->user_id_len = 0;
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "PPP user ID never written");
  }

  /*-------------------------------------------------------------------------
    Read the PPP password from NV.  When using RUIM, this will return only
    the PAP password.  The CHAP password cannot be retrived, and is 
    maintained only in the card to be used when a CHAP command is issued
  -------------------------------------------------------------------------*/
  nv_status = nv_get( NV_PPP_PASSWORD_I, &nv_item, ds707_curr_subs_id() );
  /*-------------------------------------------------------------------------
    If NV read succeeded, load the password into the provided ppp config
  -------------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S)
  {
    ppp_auth_info_ptr->passwd_len = nv_item.ppp_password.password_len;
    memscpy( ppp_auth_info_ptr->passwd_info,
             PPP_MAX_PASSWD_LEN,
             (char *)nv_item.ppp_password.password,
             ppp_auth_info_ptr->passwd_len );
  }
/*lint -restore*/
  /*-------------------------------------------------------------------------
    If NV was never written then the length of the password is set to 0.
    This will ensure that no password is used in calculating AP response.
  -------------------------------------------------------------------------*/
  else
  {
#ifdef FEATURE_DATA_PPP_DEFAULT_PASSWD
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "SN PPP Password not provisioned, using default");

    /*-----------------------------------------------------------------------
      Set PPP config with default value
    -----------------------------------------------------------------------*/
    if( PPP_DEFAULT_SN_PASSWORD_LEN <= PPP_MAX_PASSWD_LEN )
    {
      ppp_auth_info_ptr->passwd_len = PPP_DEFAULT_SN_PASSWORD_LEN;
      memscpy( ppp_auth_info_ptr->passwd_info,
               PPP_MAX_PASSWD_LEN,
               PPP_DEFAULT_SN_PASSWORD,
               PPP_DEFAULT_SN_PASSWORD_LEN );
    }
    else
    {
      ppp_auth_info_ptr->passwd_len = 0;
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "Default PPP password too long to fit in PPP config. "
                       "SN PPP password not configured");
    }
#else
    /*-----------------------------------------------------------------------
      Set the user_id length as 0.
    -----------------------------------------------------------------------*/
    ppp_auth_info_ptr->passwd_len = 0;
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "PPP password never written");
#endif /* FEATURE_DATA_PPP_DEFAULT_PASSWD */
  }

} /* is707_get_ppp_auth_info_from_nv() */
/*lint -restore*/

/*===========================================================================
FUNCTION DSSNET4_SET_IN_QNC_CALL()

DESCRIPTION
  This function notifies dssnet if the mobile is in a QNC call. Called by
  ds707_pkt_mgr.

DEPENDENCIES
  None.

PARAMETERS
  dssnet4 instance handle
  boolean     in_qnc_call

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_set_in_qnc_call
(
  dssnet4_sm_cb_type *instance_ptr,           
  boolean           in_qnc_call
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( instance_ptr != NULL )
  {
    instance_ptr->qnc_call = in_qnc_call;
  }
} /* dssnet_set_in_qnc_call() */

/*===========================================================================
FUNCTION DSSNET4_OVERRIDE_QCMIP()

DESCRIPTION
  Override dsat707_qcmip value with 2.

  RMSM calls this to force dssnet to close in the qcmip=1 case, 
  so it can bring up netmodel SimpleIP TE2 calls on its own.

  RMSM calls this to force_sip_fallback_to_fail_so_rmsm_can_do_it()

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_override_qcmip
( 
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  instance_ptr->te2_call_override_qcmip = TRUE;
} /* dssnet4_override_qcmip() */

/*===========================================================================
FUNCTION DSSNET4_SET_TE_CLIENT()

DESCRIPTION
  This is a flag set by RMSM to allow dssnet to check if the 
  call is a laptop call or a sockets call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_set_te_client
( 
  dssnet4_sm_cb_type *instance_ptr           /* dssnet4 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  instance_ptr->is_te2_client = TRUE;
} /* dssnet4_set_te_client() */


/*===========================================================================
FUNCTION DSSNET4_REG_PHYS_LINK_EVENTS()

DESCRIPTION
  The common dssnet_start() invokes this function to register
  for the phys link events.
  
DEPENDENCIES
  None.

RETURN VALUE
  0: on success
 -1: on failure
 
SIDE EFFECTS
  None.
===========================================================================*/
int dssnet4_reg_phys_link_events
( 
  dssnet4_sm_cb_type *instance_ptr
)
{
  int result1,result2, result3;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(instance_ptr);

  /*-------------------------------------------------------------------------
    Register for PHYS LINK UP event.
  -------------------------------------------------------------------------*/
  result1 =  ps_iface_event_cback_reg( instance_ptr->ps_iface_ptr,
                                       IFACE_PHYS_LINK_UP_EV,
                                       instance_ptr->phys_link_up_cbuf);

  /*-------------------------------------------------------------------------
    Register for PHYS LINK GONE event.
  -------------------------------------------------------------------------*/
  result2 =  ps_iface_event_cback_reg( instance_ptr->ps_iface_ptr,
                                       IFACE_PHYS_LINK_GONE_EV,
                                       instance_ptr->phys_link_gone_cbuf);

  /*-------------------------------------------------------------------------
    Register for PHYS LINK DOWN event.
  -------------------------------------------------------------------------*/
  result3 =  ps_iface_event_cback_reg( instance_ptr->ps_iface_ptr,
                                       IFACE_PHYS_LINK_DOWN_EV,
                                       instance_ptr->phys_link_down_cbuf);

  if ((-1 == result1) || (-1 == result2) || (-1 == result3))
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR,  "ps_iface_event_cback_reg for "
                     "IFACE_PHYS_LINK_UP/GONE/DOWN event failed");
    return -1;
  }
  
  return result3;
} /* dssnet4_reg_phys_link_events() */

/*===========================================================================
FUNCTION DSSNET4_DEREG_PHYS_LINK_EVENTS()

DESCRIPTION
  When we transition to CLOSED state we invoke this command to deregister the
  phys link event cbacks.
DEPENDENCIES
  None.

RETURN VALUE
  0: on success
 -1: on failure
 
SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_dereg_phys_link_events
( 
  dssnet4_sm_cb_type *instance_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(instance_ptr);
  /*-------------------------------------------------------------------------
    De-Register for PHYS LINK UP event.
  -------------------------------------------------------------------------*/
  ps_iface_event_cback_dereg( instance_ptr->ps_iface_ptr,
                              IFACE_PHYS_LINK_UP_EV,
                              instance_ptr->phys_link_up_cbuf);
  
  /*-------------------------------------------------------------------------
    De-Register for PHYS LINK GONE event.
  -------------------------------------------------------------------------*/
  ps_iface_event_cback_dereg( instance_ptr->ps_iface_ptr,
                              IFACE_PHYS_LINK_GONE_EV,
                              instance_ptr->phys_link_gone_cbuf);

 /*-------------------------------------------------------------------------
    De-Register for PHYS LINK DOWN event.
  -------------------------------------------------------------------------*/
  ps_iface_event_cback_dereg( instance_ptr->ps_iface_ptr,
                              IFACE_PHYS_LINK_DOWN_EV,
                              instance_ptr->phys_link_down_cbuf);

} /* dssnet4_dereg_phys_link_events() */

/*===========================================================================
FUNCTION DSSNET4_REG_PPP_RESYNC_DONE_CB()

DESCRIPTION
  This function registers the call back function to notify the completion of
  a PPP resync (it could be a successful or a failure resync)

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the dssnet instance and pointer to the call back function
  to be registered

RETURN VALUE
  TRUE if registration succeeded else FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssnet4_reg_ppp_resync_done_cb
(
  dssnet4_sm_cb_type *instance_ptr,
  void (* ppp_resync_done_cb)(void *)
)
{
  boolean reg_status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!(DSSNET4I_IS_VALID(instance_ptr)))
  {
    return reg_status;
  }
  if (instance_ptr->state != DSSNET4I_CLOSED_STATE)
  {
    instance_ptr->ppp_resync_done_cb = ppp_resync_done_cb;
    reg_status = TRUE;
  }
  return reg_status;
}

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSNET4I_PROCESS_PHY_LINK_UP_EV()

DESCRIPTION
  This function processes the DSSNET4_PHY_LINK_UP_EV event. 

  State will be changed to DSSNET4I_WAITING_FOR_NET_UP_STATE 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_phy_link_up_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
#ifdef FEATURE_DS_MOBILE_IP
  uint8  qcmip = 0;              /* effective qcmip value to use for this call */
  int8 mip_index = -1;
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
#endif /* FEATURE_DS_MOBILE_IP */
  boolean  handover_attach = FALSE;
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
  ps_iface_type* v6_iface_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */
  ps_iface_net_down_reason_type call_end_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
  ds707_data_session_profile_info_type 
                       *data_sess_profile_info_ptr = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "PHY_LINK_UP_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);

#ifdef FEATURE_DS_MOBILE_IP
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
#endif /* FEATURE_DS_MOBILE_IP */

  switch(instance_ptr->state)
  {
    case DSSNET4I_CLOSED_STATE:
      /*---------------------------------------------------------------
        Once the phys-link is up, perform the check, if the 
        requested profile is allowed to go on the current system.
        If its a dual IP call, it may happen that the other IP type
        call is already up, and the requesting profile is already
        cleared - in this case get the current profile.
      ---------------------------------------------------------------*/
      if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                  instance_ptr->ps_iface_ptr,
                                                  &data_sess_profile_info_ptr) )
      {
        ds707_data_session_get_iface_current_profile(instance_ptr->ps_iface_ptr,
                                                     &data_sess_profile_info_ptr);
      }

      pdncntx_cb_ptr = 
        ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(instance_ptr->ps_iface_ptr);

      if ( DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) && 
           (data_sess_profile_info_ptr) != NULL )
      {
#ifdef FEATURE_EPC_HANDOFF
        handover_attach = pdncntx_cb_ptr->handover_attach;
#endif
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
        if ( (ds707_pkt_is_ehrpd_mode_of_operation() == FALSE) &&
             (data_sess_profile_info_ptr->pdn_type == DS707_DATA_SESS_PDN_TYPE_V4_V6) )
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
              "dual IP request in hrpd mode. set v4 and v6 iface to passive");
          v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);

          PS_IFACE_SET_IS_ACTIVE(instance_ptr->ps_iface_ptr, FALSE);
          PS_IFACE_SET_IS_ACTIVE(v6_iface_ptr, FALSE);

          instance_ptr->is_ehrpd_call = FALSE;

          if( (!PS_IFACE_IS_IN_USE(v6_iface_ptr)) &&
              (ps_iface_state(v6_iface_ptr) == IFACE_DOWN) )
          {
            DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                             "v6 iface 0x%x:%d not in use, in down state - free v6 iface",
                             v6_iface_ptr->name, v6_iface_ptr->instance );
            ds707_pdncntx_free_v6_iface(pdncntx_cb_ptr);
          }
          if(!PS_IFACE_IS_IN_USE(instance_ptr->ps_iface_ptr))
          {
            DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                             "v4 iface 0x%x:%d not in use, transition dssnet4 to closed state",
                             instance_ptr->ps_iface_ptr->name, 
                             instance_ptr->ps_iface_ptr->instance );
            dssnet4i_transition_state(instance_ptr,DSSNET4I_CLOSED_STATE);
            return;
          }
        }
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */
      }
      else
      {
        DATA_IS707_MSG2( MSG_LEGACY_ERROR, 
                         "PDN cb invalid for iface 0x%x:%d",
                         instance_ptr->ps_iface_ptr->name,
                         instance_ptr->ps_iface_ptr->instance );
        dssnet4i_transition_state(instance_ptr,DSSNET4I_CLOSED_STATE);
        return;
      }
      
      /*-----------------------------------------------------------------------
        Check if PDN CB orig sys is compatible with the current pref sys & 
        check if packet call is allowed with the current configuration
      -----------------------------------------------------------------------*/
      if ((FALSE == ds707_pdncntx_is_call_allowed(
                      pdncntx_cb_ptr, &call_end_reason)) ||
          (FALSE == dssnet_call_allowed ( data_sess_profile_info_ptr, 
                                          handover_attach,
                                          &call_end_reason )) )
      {
        /*---------------------------------------------------------------
          Since the Application is not allowed to bring up the call, 
          dont allow it.
        ---------------------------------------------------------------*/
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Call not allowed in current config. "
                        "Moving dssnet4 0x%x to closed state ",
                         instance_ptr);
        dssnet4_set_ps_call_end_reason(instance_ptr, 
                                       call_end_reason);
        dssnet4i_transition_state(instance_ptr,DSSNET4I_CLOSED_STATE);
      }
      else
      {
#ifdef FEATURE_DS_MOBILE_IP
        /*---------------------------------------------------------------------
          Extra logic for SIP fallback
        ---------------------------------------------------------------------*/
        /* if DUN call && QC-MIP set to MIP to SIP Fallback */
        if( (1 == PS_IFACE_GET_PROC_ID( instance_ptr->ps_iface_ptr )) && 
            (1 == ds_1x_runtime_profile_ptr->mip.ds_qcmip_val) )
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "Over-riding MIP value for fall back case");
          qcmip = 2; 
        }
        else
        {
          qcmip = ds_1x_runtime_profile_ptr->mip.ds_qcmip_val;  
        }
#ifdef FEATURE_DATA_PS_MIP_CCOA
        if (dshdr_get_mobile_ip_uses_ccoa()) 
        {
          qcmip = 0;
        }
#endif /* FEATURE_DATA_PS_MIP_CCOA */

        /*---------------------------------------------------------------------
          Set Um PPP device MIP settings according to QCMIP value
          QCMIP = 0  -> SIP only
          QCMIP = 1  -> MIP wanted, SIP if MIP fails for some reason
          QCMIP = 2  -> MIP or nothing
        ---------------------------------------------------------------------*/
        instance_ptr->will_sip = (qcmip == 2) ? FALSE : TRUE;

        DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                         "MIP value now: %d, will_sip: %d",
                         qcmip,
                         instance_ptr->will_sip);

        /*-------------------------------------------------------------------
          If the network doesn't support MIP and the mobile is configured 
          to do PPP with mobile IP only, do not continue.

          For HDR targets, it is possible that the call could get connected 
          on an HDR system that supports MIP even if the 1X system does not
          support MIP, since in hybrid mode operation, we monitor both 
          systems. So, do this check only for non-HDR targets. Do a similar
          check for HDR targets AFTER the physical link has come up.
        -------------------------------------------------------------------*/

        /*-------------------------------------------------------------------
          If mobile is configured to attempt mobile IP, configure the MIP 
          session settings before starting PPP.
        -------------------------------------------------------------------*/

        /*-------------------------------------------------------------------
          If profile id specified for call bring up is MIP profile, get the 
          index to be used to read MIP parameters from NV. If the profile id 
          specified is not a MIP profile, then read the MIP profile index from 
          NV item NV_DS_MIP_ACTIVE_PROF_I.
        -------------------------------------------------------------------*/
        
        if( PROFILE_ID_IS_MIP(data_sess_profile_info_ptr->data_session_profile_id) )
        {
          mip_index = ds707_get_mip_index_from_profile_id(
                        data_sess_profile_info_ptr->data_session_profile_id);
        }
        else
        {
          mip_index = mip_get_active_profile_index_from_nvruim();
          ds707_set_mip_profile_index( mip_index );
        }


#ifdef FEATURE_EHRPD
        /*----------------------------------------------------------------- 
          If serving system is hdr, and mode of operation is ehrpd, do not
          allow MIP call. Bring up SIP call
        -------------------------------------------------------------------*/
        if ( ds707_pkt_is_ehrpd_mode_of_operation())
        {
          /* QCMIP = 1 OR 2 - Bring up SIP call */
          DATA_IS707_MSG0(MSG_LEGACY_MED, "MIP not allowed in eHRPD, trying SimpleIP");
          instance_ptr->want_mip = FALSE;
          dssnet4i_transition_state(instance_ptr, 
                                    DSSNET4I_LINK_OPENING_WITH_SIP_STATE);
        }
        else        
#endif /* FEATURE_EHRPD */
        {
          if ( qcmip > 0 && 
               ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA) && dssnet4_1x_mip_support())
                 || (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR)) 
               && (mip_config_valid() || mip_config_session( mip_index )) )
          {
          
            instance_ptr->want_mip = TRUE;
            /*-----------------------------------------------------------------
              Attempt MIP once physical link is up. This will be set to FALSE 
              by dssnet if mip orig fails 
            -----------------------------------------------------------------*/
            dssnet4i_transition_state(instance_ptr,
                                    DSSNET4I_LINK_OPENING_WITH_MIP_STATE);
          }
          
          else if(instance_ptr->will_sip)
          {
            DATA_IS707_MSG0(MSG_LEGACY_MED, "Trying SimpleIP");
            instance_ptr->want_mip = FALSE;
            dssnet4i_transition_state(instance_ptr,
                                      DSSNET4I_LINK_OPENING_WITH_SIP_STATE);
          }
          else
          {
            /*---------------------------------------------------------------
              If SimpleIP is not an option, abort now
            ---------------------------------------------------------------*/
            DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Can't try SIP, ending call");
            dssnet4_set_ps_call_end_reason(instance_ptr, 
                                           PS_NET_DOWN_REASON_MIP_CONFIG_FAILURE);
            dssnet4i_transition_state(instance_ptr,DSSNET4I_CLOSED_STATE);
            break;
          }
        }
#else
        DATA_IS707_MSG0(MSG_LEGACY_MED, "Trying SIP call");
        dssnet4i_transition_state(instance_ptr, 
                                     DSSNET4I_LINK_OPENING_WITH_SIP_STATE);

#endif /* FEATURE_DS_MOBILE_IP */
      }

      /*---------------------------------------------------------------------
        Deregister for the phys link up event in all cases.  
      ---------------------------------------------------------------------*/
      ps_iface_event_cback_dereg(instance_ptr->ps_iface_ptr,
                                 IFACE_PHYS_LINK_UP_EV,
                                 instance_ptr->phys_link_up_cbuf);
      
      break;

    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_NET_UP_STATE:
    case DSSNET4I_LINK_RESYNC_STATE:
    case DSSNET4I_LINK_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG2( MSG_LEGACY_MED, 
                       "dssnet4 0x%x ignoring PHY_LINK_UP_EV in %d state",
                       instance_ptr,
                       instance_ptr->state);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* switch */

} /* dssnet4i_process_phy_link_up_ev() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_PHY_LINK_GONE_EV()

DESCRIPTION
  This function processes the DSSNET4_PHY_LINK_GONE_EV. It causes a 
  transition to the CLOSED STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_phy_link_gone_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
  rex_timer_type *conn_delay_timer;
  ds3g_pkt_call_info_type *pkt_call_info_ptr = NULL;
  int16                     p_errno;
  int                       ret_val;
  ps_flow_type             *ps_flow_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "PHY_LINK_GONE_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);

  /*---------------------------------------------------------------------------
    Try to bringup the phys link again if this flag is set. This will be set
    when there is a call bringup attempt when phys link is still being cleaned
    up. After the phys link is torn down completely an attempt to bringup the
    phys link again needs to be tried.
  ---------------------------------------------------------------------------*/
  if (dssnet4_get_is_bringup_phys_link(instance_ptr))
  {
    dssnet4_set_is_bringup_phys_link(instance_ptr, FALSE);

    /*-------------------------------------------------------------------------
      All the flows are unbinded when phys link is cleaned up, so bind the flow
      again. Without this another dssnet handling gone event will clean up the
      phys link again. Activate the flow else the data transfer will fail
    -------------------------------------------------------------------------*/
    if ((NULL != instance_ptr->ps_iface_ptr) && 
        (NULL != instance_ptr->ps_iface_ptr->iface_private.phys_link.array))
    {
      ps_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(instance_ptr->ps_iface_ptr);
      (void) ps_flow_bind_phys_link(ps_flow_ptr,
                        PS_IFACEI_GET_PHYS_LINK(instance_ptr->ps_iface_ptr));
      ps_flow_activate_ind(ps_flow_ptr, PS_EIC_NOT_SPECIFIED);
    }

    else
    {
      /*-----------------------------------------------------------------------
        This should never happen
      -----------------------------------------------------------------------*/
      ASSERT(0);
      return;
    }

    /*------------------------------------------------------------------------
      pass valid value as user info to phys_link_up_cmd. Create a struct of
      type ds3g_pkt_call_info_type and pass it as user_info. this  function
      populates pkt_call_info and returns void*.
    ------------------------------------------------------------------------*/
    pkt_call_info_ptr = ds707_pkt_mgr_setup_pkt_call_info(
                                                   instance_ptr->ps_iface_ptr);

    ret_val = ps_iface_phys_link_up_cmd ( instance_ptr->ps_iface_ptr,
                                          &p_errno,
                                          pkt_call_info_ptr );
    if (!((ret_val < 0) && (p_errno != DS_EWOULDBLOCK)))
    {
      /*---------------------------------------------------------------------
        Phys link up cmd successful. No other dssnet state transition needs
        to be done after this
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_MED, "phy link up cmd successful");
      return;
    }
  }

  /*lint -save -e825 suppressing error 825, fall thru intentional */
  switch(instance_ptr->state)
  {
    case DSSNET4I_CLOSED_STATE:

       /* Stop the DHCP instance before giving down indication
        */
      (void) dshdr_stop_dhcp(instance_ptr->ps_iface_ptr);

#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
      /*---------------------------------------------------------------------
        Decrement the number of IWLAN+3GPP2 interface used count.
      ---------------------------------------------------------------------*/
      if (num_used_3gpp2_v4_ifaces > 0)
      {
        num_used_3gpp2_v4_ifaces--;
      }
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

      dssnet4i_transition_state(instance_ptr, DSSNET4I_CLOSED_STATE);

      break;

    case DSSNET4I_NET_UP_STATE:
    case DSSNET4I_LINK_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
      /*---------------------------------------------------------------------
        Post an exit event to the MIP meta sm if not in CCoA mode
      ---------------------------------------------------------------------*/
      if (!dshdr_get_mobile_ip_uses_ccoa())
      {      
        mip_meta_sm_post_event( MSM_EXIT_MIP_EV, instance_ptr->ps_iface_ptr);
      }
#endif /* FEATURE_DS_MOBILE_IP */

      /* Fall through */

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
    case DSSNET4I_LINK_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Call ppp_abort().
      ---------------------------------------------------------------------*/
      /*lint -save -e534 return value not required*/
      (void)ppp_abort(PPP_UM_SN_DEV);
      /*lint -restore*/
      if(PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) == 0)
      {
        /*---------------------------------------------------------------------
          If conn delay timer is running, ignore this event. When the timer 
          expires, the dssnet state will be transitioned
        ---------------------------------------------------------------------*/
        conn_delay_timer = ds707_pkt_get_conn_delay_timer_from_iface_ptr(
                                                instance_ptr->ps_iface_ptr);
        if(rex_get_timer(conn_delay_timer) == 0)
        {
          dssnet4i_transition_state(instance_ptr, DSSNET4I_CLOSED_STATE);
        }
      }
      else
      {
        (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                       IFACE_FLOW_DELETED_EV,
                                       instance_ptr->iface_flow_deleted_cbuf);

#ifdef FEATURE_HDR_QOS
        /* silently cleanup all secondary flows */
        ds707_sec_pkt_mgr_cleanup_all_secondary_flows(
                                       instance_ptr->ps_iface_ptr, FALSE);
#endif
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* end switch */
  /*lint -restore Restore lint error 825*/

} /* dssnet4i_process_phy_link_gone_ev() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_PHY_LINK_DOWN_EV()

DESCRIPTION
  This function processes the DSSNET4_PHY_LINK_DOWN_EV. It causes a 
  transition to the CLOSED STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_phy_link_down_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
  ds3g_pkt_call_info_type  *pkt_call_info_ptr = NULL;
  int16                     p_errno;
  int                       ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "PHY_LINK_DOWN_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);

  /*---------------------------------------------------------------------------
    Try to bringup the phys link again if this flag is set. This will be set
    when there is a call bringup attempt when phys link is in GOING_DOWN state.
    After the phys link goes down completely an attempt to bringup the
    phys link again needs to be tried.
  ---------------------------------------------------------------------------*/
  if (dssnet4_get_is_bringup_phys_link(instance_ptr))
  {
    dssnet4_set_is_bringup_phys_link(instance_ptr, FALSE);

    /*------------------------------------------------------------------------
      pass valid value as user info to phys_link_up_cmd. Create a struct of
      type ds3g_pkt_call_info_type and pass it as user_info. this  function
      populates pkt_call_info and returns void*.
    ------------------------------------------------------------------------*/
    pkt_call_info_ptr = ds707_pkt_mgr_setup_pkt_call_info(
                                                   instance_ptr->ps_iface_ptr);

    ret_val = ps_iface_phys_link_up_cmd ( instance_ptr->ps_iface_ptr,
                                          &p_errno,
                                          pkt_call_info_ptr );
    if (!((ret_val < 0) && (p_errno != DS_EWOULDBLOCK)))
    {
      /*---------------------------------------------------------------------
        Phys link up cmd successful. No other dssnet state transition needs
        to be done after this
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_MED, "phy link up cmd successful");
      return;
    }
  }

  /*lint -save -e825 suppressing error 825, fall thru intentional */
  switch(instance_ptr->state)
  {
    case DSSNET4I_CLOSED_STATE:

       /* Stop the DHCP instance before giving down indication
        */
      (void) dshdr_stop_dhcp(instance_ptr->ps_iface_ptr);

#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
      /*---------------------------------------------------------------------
        Decrement the number of IWLAN+3GPP2 interface used count.
      ---------------------------------------------------------------------*/
      if (num_used_3gpp2_v4_ifaces > 0)
      {
        num_used_3gpp2_v4_ifaces--;
      }
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

      dssnet4i_transition_state(instance_ptr, DSSNET4I_CLOSED_STATE);

      break;

    case DSSNET4I_NET_UP_STATE:
    case DSSNET4I_LINK_CLOSING_STATE:
    case DSSNET4I_LINK_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:

      /*----------------------------------------------------------------------- 
       Once DSSNET4 moves to LINK OPENING WITH MIP/SIP state, we must have
       deregistered the PHY_LINK_DOWN_EV.
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG2( MSG_LEGACY_ERROR, 
                "Got unexpected PHY_LINK_DOWN_EV for DSSNet4 0x%x in %d state",
                instance_ptr,
                instance_ptr->state);
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* end switch */
  /*lint -restore Restore lint error 825*/

} /* dssnet4i_process_phy_link_down_ev() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_LINK_UP_WITH_SIP_EV()

DESCRIPTION
  This function processes the DSSNET4_LINK_UP_WITH_SIP_EV. Changes state to 
  DSSNET4I_NET_UP_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_link_up_with_sip_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif /* FEATURE_CALL_THROTTLE */
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_CALL_THROTTLE
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
#endif /* FEATURE_CALL_THROTTLE */
  ds_fallback_enum_type ds_fallback;
  ds_fallback = DS_MIP_TO_SIP_FALLBACK;
#endif /* FEATURE_DS_MOBILE_IP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "LINK_UP_WITH_SIP_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_CALL_THROTTLE
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
#endif /* FEATURE_CALL_THROTTLE */
#endif /* FEATURE_DS_MOBILE_IP */

  /*lint -save -e825 suppressing error 825, fall thru intentional */
  switch(instance_ptr->state)
  {
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:

      /*---------------------------------------------------------------------
        Fall back to SIP if supported, else tear down the call.  
      ---------------------------------------------------------------------*/
      if (instance_ptr->will_sip)
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "Fall back to Simple IP");
    		event_report_payload ( EVENT_DS_FALLBACK, 
    							             sizeof(ds_fallback_enum_type), 
                               (void*)&ds_fallback);
        /* Fall through to transition to NET_UP state */
      }
      else if (instance_ptr->want_mip)
      {
        /*-------------------------------------------------------------------
          Transition to CLOSING state.  
        -------------------------------------------------------------------*/
#ifdef FEATURE_CALL_THROTTLE
        /* --------------------------------------------------------------- 
          This case will be executed if this is laptop call and if this is
          a mip only sockets calls.  This only posts failure if this is a 
          mip only sockets and network model calls
        ----------------------------------------------------------------*/
        if ( ( instance_ptr->is_te2_client == FALSE) ||
             ( ( instance_ptr->is_te2_client == TRUE) && 
               ( ds_1x_runtime_profile_ptr->mip.ds_qcmip_val == 2)
             )
           )
        {
          pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
          if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
          {
            dctm_post_mip_call_status( MIP_FAILURE_EV,
                                       MIP_FAIL_REASON_PDSN_FAILURE);
            ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
          }
        }
#endif /* FEATURE_CALL_THROTTLE */
        dssnet4i_transition_state( instance_ptr,
                                   DSSNET4I_LINK_CLOSING_STATE);      
        break;
      }
      else
      {
        /*-------------------------------------------------------------------
          qcmip is QCMIP_SIP_ONLY. We should never run into this case. 
        -------------------------------------------------------------------*/
        DATA_ERR_FATAL("Up for SimpleIP, but prov'd for MIP only!");
        break;
      }
#endif /* FEATURE_DS_MOBILE_IP */

    case DSSNET4I_LINK_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
      if ( ds707_drs_is_in_mip_call() == TRUE && 
           dshdr_get_mobile_ip_uses_ccoa() == FALSE )  
      {
        mip_meta_sm_post_event( MSM_EXIT_MIP_EV, instance_ptr->ps_iface_ptr );
      }
      /*---------------------------------------------------------------------
         Fallthrough intentional to allow the connection to be up for 
         MIP -> SIP - FallBack scenario 
      ---------------------------------------------------------------------*/
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_CLOSED_STATE:
      /*---------------------------------------------------------------------
         Due to race condition, there is a possiblity that Phys link gone ev
         and PPP link up ev can be posted in this particular order. When
         DSSNET processes phys link gone ev first, it transitions the SM to
         closed state and posts iface down and cleans up the associated PDN
         When DSSNET processes PPP link up ev here in closed state,
         it ends up moving a stale dssnet SM to Net up state and iface to up
         state which is incorrect. Hence check for iface state here before
         proceeding further.
      ---------------------------------------------------------------------*/
      if ( instance_ptr->state == DSSNET4I_CLOSED_STATE &&
           (ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_DOWN || 
            ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_GOING_DOWN)
          )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Link up with Sip ev recd when DSSNET SM in closed "
                        "state and iface is in down/going down state - "
                        "Ignore");
        break;
      }
      /* Else - Fall through */

    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
      if ( instance_ptr->is_ehrpd_call == TRUE )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Link up with Sip ev recd when DSSNET SM in OPENNING state ");

      }
      else
      {   
#ifdef FEATURE_CALL_THROTTLE 
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        {
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "Posting PPP_SUCCESS_EV with protocol IPCP to DCTM");
          dctm_post_ppp_call_status( PPP_SUCCESS_EV, 
                                     DSDCTM_PPP_PROTOCOL_IPCP, 
                                     PPP_FAIL_REASON_NONE );
          ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
        }
#endif /* FEATURE_CALL_THROTTLE */
      }
      dssnet4i_transition_state( instance_ptr,
                                 DSSNET4I_NET_UP_STATE);      
      break;

    /*---------------------------------------------------------------------
      We cld receive LINK_UP_WITH_MIP if PS task receives IPCP C-ack, before
      processing ppp_stop that was sent on transition to LINK_CLOSING.
      In this case we ignore LINK_UP - eventually the cleanup should happen
      on transition to CLOSED state on phys_link_gone/down bcz of ppp_stop.
    ---------------------------------------------------------------------*/
    case DSSNET4I_LINK_CLOSING_STATE:
      /* In LINK_CLOSING state, Ignore */
      break;

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_NET_UP_STATE:
      /* Fall through */

     default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,"dssnet4 state machine in" 
                       "invalid state: %d", instance_ptr->state);
      ASSERT(0);
  } /* end switch */
  /*lint -restore Restore error 825 */

} /* dssnet4i_process_link_up_with_sip_ev() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_AUTH_STARTING_EV()

DESCRIPTION
  This function processes the DSSNET4_PPP_AUTH_STARTED_EV. The only action taken
  by this method is to post IFACE_AUTHENTICATING_EV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_auth_starting_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    DATA_MDM_ARG_NOT_CONST(instance_ptr);

    if (NULL == instance_ptr) 
    {
      ASSERT(0);
      return;
    }

    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "Received DSSNET4_PPP_AUTH_STARTED_EV");
    /*posting the start of authentication to ps_iface*/
    if(instance_ptr->ps_iface_ptr != NULL)
    {
      ps_iface_generic_ind( instance_ptr->ps_iface_ptr, 
                            IFACE_AUTHENTICATING_EV, 
                            NULL);
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "Cannot post ps_iface_generic_ind because iface "
                       "pointer is NULL");
    }
    return;
}/*dssnet4i_process_auth_starting_ev()*/

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_LINK_DOWN_EV()

DESCRIPTION
  This function processes the DSSNET4_LINK_DOWN_EV. Changes state to 
  DSSNET4I_CLOSED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_link_down_ev
(
  dssnet4_sm_cb_type *instance_ptr,           /* dssnet4 instance ptr      */
  ppp_protocol_e_type protocol,
  ppp_fail_reason_e_type  reason
)
{
  rex_timer_type *conn_delay_timer;
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif /* FEATURE_CALL_THROTTLE*/
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_CALL_THROTTLE
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
#endif /* FEATURE_CALL_THROTTLE*/
  ds_fallback_enum_type ds_fallback;
  ds_fallback = DS_MIP_TO_SIP_FALLBACK;
#endif /* FEATURE_DS_MOBILE_IP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG4(MSG_LEGACY_HIGH, 
     "LINK_DOWN_EV recd for dssnet4 0x%x in state %d. protocol %d, reason %d",
     instance_ptr, instance_ptr->state, protocol, reason);

#ifdef FEATURE_CALL_THROTTLE
#ifdef FEATURE_DS_MOBILE_IP
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr(); 
#endif /* FEATURE_CALL_THROTTLE*/
#endif /* FEATURE_DS_MOBILE_IP */

  /*lint -save -e825 suppressing error 825, fall thru intentional */
  switch(instance_ptr->state)
  {
    case DSSNET4I_CLOSED_STATE:
      /*---------------------------------------------------------------------
        Ignore LINK_DOWN_EV
      ---------------------------------------------------------------------*/
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
      /*---------------------------------------------------------------------
        Fallback to SIP if possible.  
      ---------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Check to see the value of QCMIP. It qcmip==QCMIP_MIP_PREFERRED, 
        transition to DSSNET4I_LINK_OPENING_WITH_SIP_STATE. If 
        qcmip==QCMIP_MIP_ONLY, transition to DSSNET4I_CLOSED_STATE.
      ---------------------------------------------------------------------*/
      if (instance_ptr->will_sip)
      {
        /*---------------------------------------------------------------------
          Post an exit event to the MIP meta sm.  
        ---------------------------------------------------------------------*/
        mip_meta_sm_post_event( MSM_EXIT_MIP_EV, 
                                instance_ptr->ps_iface_ptr);
        event_report_payload ( EVENT_DS_FALLBACK, 
						                   sizeof(ds_fallback_enum_type), 
                               (void*)&ds_fallback);
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "Fall back event DS_MIP_TO_SIP_FALLBACK");
        dssnet4i_transition_state( instance_ptr,
                                   DSSNET4I_LINK_OPENING_WITH_SIP_STATE);
        break;
      }

      /* Fall through */  

#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
#ifdef FEATURE_CALL_THROTTLE
      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
      if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
      {
#ifdef FEATURE_DS_MOBILE_IP
        if ( ( instance_ptr->is_te2_client == FALSE ) ||
             ( ( instance_ptr->is_te2_client == TRUE ) && 
               ( ds_1x_runtime_profile_ptr->mip.ds_qcmip_val == 2 ) ) )
 #endif /*Feature_DS_MOBILE_IP */
        {
          /*-------------------------------------------------------------------
            DCTM for AUTH/LCP is handled in device manger, ignore here
          -------------------------------------------------------------------*/ 
          if(instance_ptr->is_ehrpd_call == FALSE)
          {
            if( (reason == PPP_FAIL_REASON_TIMEOUT) || 
                (reason == PPP_FAIL_REASON_OPTION_MISMATCH) )
            {
              dctm_post_ppp_call_status( PPP_FAILURE_EV,
                                         (dsdctm_ppp_protocol_e_type)protocol,
                                         reason );
              ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
            }
          
            else if ( ( reason == PPP_FAIL_REASON_AUTH_FAILURE ) || 
                      ( reason == PPP_FAIL_REASON_AUTH_PAP_FAILURE ) || 
                      ( reason == PPP_FAIL_REASON_AUTH_CHAP_FAILURE )
                    )
            {
              dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                         DSDCTM_PPP_PROTOCOL_AUTH, 
                                         reason );
              ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
            }
          }
        } 
      }
#endif /* FEATURE_CALL_THROTTLE */

    case DSSNET4I_LINK_RESYNC_STATE:
    case DSSNET4I_LINK_CLOSING_STATE:
    case DSSNET4I_NET_UP_STATE:
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
    
      if(PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) == 0)
      {
        /*-------------------------------------------------------------------
          If conn delay timer is running, ignore this event. When the timer 
          expires, the dssnet state will be transitioned
        -------------------------------------------------------------------*/
        conn_delay_timer = ds707_pkt_get_conn_delay_timer_from_iface_ptr(
                                                 instance_ptr->ps_iface_ptr);
        if(rex_get_timer(conn_delay_timer) == 0)
        {
          dssnet4i_transition_state(instance_ptr, DSSNET4I_CLOSED_STATE);
        }
      }
      else
      {
        (void)ps_iface_event_cback_reg( 
                                      instance_ptr->ps_iface_ptr,
                                      IFACE_FLOW_DELETED_EV,
                                      instance_ptr->iface_flow_deleted_cbuf);
        
        #ifdef FEATURE_HDR_QOS
          
        /* post a command to sec pkt mgr informing of PPP down event */
        ds707_sec_pkt_network_initiated_ppp_down_cmd(
                                        instance_ptr->ps_iface_ptr);
        #endif /* FEATURE_HDR_QOS */
                
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* end switch */
  /*lint -restore error 825 */
} /* dssnet4i_process_link_down_ev() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_LINK_RESYNC_EV()

DESCRIPTION
  This function processes the DSSNET_LINK_RESYNC_EV event. Changes State
  to DSSNET4I_LINK_RESYNC_STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_link_resync_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "LINK_RESYNC_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);

  switch(instance_ptr->state)
  {
    case DSSNET4I_CLOSED_STATE:
    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
    case DSSNET4I_LINK_RESYNC_STATE:
    case DSSNET4I_LINK_CLOSING_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
#endif /* FEATURE_DS_MOBILE_IP */
      /*---------------------------------------------------------------------
        Ignore event.
      ---------------------------------------------------------------------*/
      break;
    
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_NET_UP_STATE:
      dssnet4i_transition_state( instance_ptr, 
                                 DSSNET4I_LINK_RESYNC_STATE);
      break;
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
      dssnet4i_transition_state( instance_ptr, 
                                 DSSNET4I_LINK_CLOSING_STATE);
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      return;
      ASSERT(0);
  } /* end switch */

} /* dssnet_process_link_resync_ev() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_PPP_CLOSE_EV()

DESCRIPTION
  This function processes the DSSNET4_PPP_CLOSE_CMD_EV. Changes state to 
  DSSNETI_WAITING_FOR_LINK_DOWN_STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_ppp_close_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "PPP_CLOSE_CMD_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);

  switch(instance_ptr->state)
  {
    
    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_LINK_RESYNC_STATE:
      if(PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) != 0)
      {
        (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                       IFACE_FLOW_DELETED_EV,
                                       instance_ptr->iface_flow_deleted_cbuf);
      }      
      
      dssnet4i_transition_state( instance_ptr,
                                 DSSNET4I_LINK_CLOSING_STATE);      
      break;

    case DSSNET4I_NET_UP_STATE:
#ifdef FEATURE_DS_MOBILE_IP      
#ifdef FEATURE_DS_MOBILE_IP_DEREG
      if ((ds707_drs_is_in_mip_call() == TRUE) &&
          (ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_NON_EPC_SYSTEM))
      {
        dssnet4i_transition_state( instance_ptr,
                                   DSSNET4I_WAITING_FOR_MIP_DEREG_STATE);
      } 
      else
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
      {
        if(PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) != 0)
        {
          (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                         IFACE_FLOW_DELETED_EV,
                                         instance_ptr->iface_flow_deleted_cbuf);
        }
        dssnet4i_transition_state( instance_ptr,
                                   DSSNET4I_LINK_CLOSING_STATE);
      }
      break;

    case DSSNET4I_LINK_CLOSING_STATE:
      /* ignoring PPP_CLOSE_CMD_EV */
      break;

    case DSSNET4I_CLOSED_STATE:
      dssnet4i_transition_state( instance_ptr,
                                 DSSNET4I_CLOSED_STATE); 
      break;

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
      if ( ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_EPC_SYSTEM )
      {
        if(PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) != 0)
        {
          (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                         IFACE_FLOW_DELETED_EV,
                                         instance_ptr->iface_flow_deleted_cbuf);
        }
        dssnet4i_transition_state( instance_ptr,
                                   DSSNET4I_LINK_CLOSING_STATE);
      } 
      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* end switch */

} /* dssnet4i_process_ppp_close_cmd_ev() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_FLOW_DELETED_EV()

DESCRIPTION
  This function processes the DSSNET4_FLOW_DELETED_CMD_EV. Changes state to 
  DSSNET4I_CLOSED_STATE if number of secondary flows are 0. Only occurs if
  ppp is already trying to bring down the interface. (CLOSING_STATE)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_flow_deleted_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(instance_ptr);

  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

/*---------------------------------------------------------------------------
  State doesn't matter in this case. The interface can be torn down in any
  of the cases (link_down and phys_link_gone) and the flow_deleted event will
  only be registered in those cases where dssnet is trying to close the state
  machine anyway.
---------------------------------------------------------------------------*/
  if(PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) == 0)
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED, 
                     "IFACE_FLOW_DELETED_EV: Flows on iface 0x%x cleaned up.",
                     instance_ptr->ps_iface_ptr);
    /*lint -save -e792 suppressing error 792 */
    (void)ps_iface_event_cback_dereg( instance_ptr->ps_iface_ptr,
                                      IFACE_FLOW_DELETED_EV,
                                      instance_ptr->iface_flow_deleted_cbuf);

    ds707_pkt_mgr_iface_start_conn_delay_timer(instance_ptr->ps_iface_ptr);
    /*lint -restore e792 */
  }
  else
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED, 
                     "IFACE_FLOW_DELETED_EV: Flows still exist on iface 0x%x",
                     instance_ptr->ps_iface_ptr);
  }
} /* dssnet4i_process_flow_deleted_cmd_ev() */

#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION DSSNET4I_PROCESS_LINK_UP_WITH_MIP_EV()

DESCRIPTION
  This function processes the DSSNET4_LINK_UP_WITH_MIP_EV. Changes state to 
  DSSNET4I_NET_UP_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_link_up_with_mip_ev
(
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
)
{
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "LINK_UP_WITH_MIP_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);

  /*lint -save -e825 suppressing error 825, fall thru intentional */
  switch(instance_ptr->state)
  {
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
    case DSSNET4I_CLOSED_STATE:
      /*---------------------------------------------------------------------
         Due to race condition, there is a possiblity that Phys link gone ev
         and PPP link up ev can be posted in this particular order. When
         DSSNET processes phys link gone ev first, it transitions the SM to
         closed state and posts iface down and cleans up the associated PDN
         When DSSNET processes PPP link up ev here in closed state,
         it ends up moving a stale dssnet SM to waiting for mip reg state
         which is incorrect. Hence check for iface state here before
         proceeding further.
      ---------------------------------------------------------------------*/
      if ( instance_ptr->state == DSSNET4I_CLOSED_STATE &&
           (ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_DOWN || 
            ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_GOING_DOWN)
          )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Link up with MIP ev recd when DSSNET SM in closed "
                        "state and iface is in down/going down state - "
                        "Ignore");
        break;
      }
      /* Else - Fall through */

    case DSSNET4I_LINK_RESYNC_STATE:
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
      dssnet4_reg_mip_event(instance_ptr);
      dssnet4i_transition_state(instance_ptr,
                                DSSNET4I_WAITING_FOR_MIP_REG_STATE);      
      break;

    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "dssnet4 0x%x, PDSN likely short on IPv4 addresses.",
                       instance_ptr);
#ifdef FEATURE_CALL_THROTTLE
      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
      if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
      {    
        if ( instance_ptr->is_ehrpd_call == TRUE )
        {    
          dctm_post_ppp_call_status( PPP_FAILURE_EV,
                                     DSDCTM_PPP_PROTOCOL_VSNCP,
                                     PPP_FAIL_REASON_NONE );
        }
        else
        {    
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "Posting PPP_FAILURE_EV with protocol IPCP to DCTM");
          dctm_post_ppp_call_status( PPP_FAILURE_EV,
                                     DSDCTM_PPP_PROTOCOL_IPCP,
                                     PPP_FAIL_REASON_NONE );
        }
        ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
      }
#endif /* FEATURE_CALL_THROTTLE */
      dssnet4i_transition_state( instance_ptr, 
                                 DSSNET4I_LINK_CLOSING_STATE);     
      break;

     /*----------------------------------------------------------------------
       We cld receive LINK_UP_WITH_MIP if PS task receives IPCP C-ack, before
       processing ppp_stop that was sent on transition to LINK_CLOSING.
       In this case we ignore LINK_UP - eventually the cleanup should happen
       on transition to CLOSED state on phys_link_gone/down bcz of ppp_stop.
     ----------------------------------------------------------------------*/
    case DSSNET4I_LINK_CLOSING_STATE:
      /* Ignore */
      break;

    case DSSNET4I_NET_UP_STATE:
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
      /* Fall through */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* end switch */
  /*lint -restore error 825, fall thru intentional */
} /* dssnet4i_process_link_up_with_mip_ev() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_MIP_UP_SUCCESS_EV()

DESCRIPTION
  This function processes the DSSNET4_MIP_UP_SUCCESS_EV. Changes state to 
  DSSNET4I_NET_UP_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_mip_up_success_ev
(
  dssnet4_sm_cb_type *instance_ptr           /* dssnet4 instance ptr      */
)
{
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "MIP_UP_SUCCESS_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);
  /*lint -save -e825 suppressing error 825, fall thru intentional */
  switch(instance_ptr->state)
  {
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
#ifdef FEATURE_CALL_THROTTLE
      /* --------------------------------------------------------------------
         for Laptop calls MIP sucess will be reported from RMSM on receiving 
         UM iFace up.
      ---------------------------------------------------------------------*/
      if (instance_ptr->is_te2_client == FALSE)
      {
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        {
          dctm_post_mip_call_status( MIP_SUCCESS_EV,
                                     MIP_FAIL_REASON_NONE);
          ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
        }
      }
#endif /* FEATURE_CALL_THROTTLE */
      dssnet4i_transition_state( instance_ptr, 
                                 DSSNET4I_NET_UP_STATE);      
      break;
 
    case DSSNET4I_NET_UP_STATE:  // this happens on regular re-reg
    case DSSNET4I_LINK_RESYNC_STATE:
    case DSSNET4I_LINK_CLOSING_STATE:
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_CLOSED_STATE:
      /* Ignore */
      break;

    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
      /* Fall through */

     default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* end switch */
  /*lint -restore error 825, fall thru intentional */
} /* dssnet4i_process_mip_up_success_ev() */

/*===========================================================================
FUNCTION DS707_COPY_RM_NAI()

DESCRIPTION
  This function is used to copy RM NAI into PPP auth info

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_copy_rm_nai
(
  ppp_auth_info_type *ppp_auth_info_ptr,
  nv_stat_enum_type (* nv_get)(nv_items_enum_type, nv_item_type *, sys_modem_as_id_e_type)
)
{
  nv_item_type         nv_item;
  nv_stat_enum_type nv_status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ppp_auth_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("Invalid ppp_auth_info_ptr pointer");
    return;
  }

  nv_status = nv_get( NV_DS_SIP_RM_NAI_I, &nv_item, ds707_curr_subs_id());
  if( nv_status != NV_DONE_S )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                "Laptop NAI cannot be retrieved. Standard NAI will be used");
    return;
  }

  /*----------------------------------------------------------------------- 
   Only copy NAI if its length is valid.  If NAI length is > max allowed
   assume a memory/flash scribble and set length to 0. 
  -----------------------------------------------------------------------*/
  if((nv_item.ds_sip_rm_nai.nai_length <= NV_MAX_NAI_LENGTH) && 
     (nv_item.ds_sip_rm_nai.nai_length > 0))
  {
    ppp_auth_info_ptr->user_id_len = nv_item.ds_sip_rm_nai.nai_length;
    memscpy(ppp_auth_info_ptr->user_id_info,
            PPP_MAX_USER_ID_LEN,
            (char*) nv_item.ds_sip_rm_nai.nai,
            nv_item.ds_sip_rm_nai.nai_length);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                 "Laptop NAI has incorrect length. Standard NAI will be used");
  }
} /* ds707_copy_rm_nai() */

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_MIP_FAILURE_EV()

DESCRIPTION
  This function processes the DSSNET4_MIP_FAILURE_EV. Changes state to 
  DSSNET4I_LINK_CLOSING_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_mip_failure_ev
(
  dssnet4_sm_cb_type *instance_ptr,           /* dssnet4 instance ptr      */
  mip_fail_reason_e_type  mip_status,
  byte               mip_rrp_code
)
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#endif 

  ppp_dev_opts_type *ppp_config = NULL;
  ds_fallback_enum_type ds_fallback;
  ds707_data_session_profile_id  data_sess_profile_id = 
                                     DATA_SESSION_PROFILE_ID_INVALID;
  uint8  sip_profile_index = 0;
  ds_fallback = DS_MIP_TO_SIP_FALLBACK;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG3(MSG_LEGACY_MED, 
          "MIP_FAILURE_EV recd for dssnet4 0x%x in state %d with mip rrp %d",
          instance_ptr, instance_ptr->state, mip_rrp_code );

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  switch(instance_ptr->state)
  {
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
      /*---------------------------------------------------------------------
        Fallback to SIP if possible.  
      ---------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Check to see the value of QCMIP. It qcmip==QCMIP_MIP_PREFERRED & 
        mip rrp code is not 129 (HA_ADMIN_PROHIBIT), transition to 
        DSSNET4I_LINK_OPENING_WITH_SIP_STATE . If qcmip==QCMIP_MIP_ONLY, 
        transition to DSSNET4I_CLOSED_STATE.
      ---------------------------------------------------------------------*/
      if (instance_ptr->will_sip &&
          ds707_pkt_mgr_is_allow_fallback_to_sip(mip_status, mip_rrp_code)
          )
      {
        /*-----------------------------------------------------------------
          Allocate memory for ppp_config
        -----------------------------------------------------------------*/
        ppp_config = (ppp_dev_opts_type*)
                     modem_mem_alloc(sizeof(ppp_dev_opts_type),
                                     MODEM_MEM_CLIENT_DATA_CRIT);
        if(ppp_config == NULL)
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "dssnet4i_process_mip_failure_ev():"
                          "memory allocation failed for ppp_config");
          dssnet4i_transition_state( instance_ptr, DSSNET4I_CLOSED_STATE);
          break;
        }
        memset((void *)ppp_config, 0, sizeof(ppp_dev_opts_type));

        DATA_IS707_MSG0( MSG_LEGACY_MED, "Fallback to SIP");
        ds707_pkt_mgr_incr_num_mip_to_sip_transitions();
        event_report_payload( EVENT_DS_FALLBACK, 
                              sizeof(ds_fallback_enum_type), 
                              (void*)&ds_fallback);

        /*-------------------------------------------------------------------
         Setup PPP configuration options. 
        -------------------------------------------------------------------*/
        ppp_get_default_opts( ppp_config );

        /*lint -save -e534 return value not required*/
        ppp_get_dev_setup(PPP_UM_SN_DEV, ppp_config);
        /*lint -restore*/

        /*-------------------------------------------------------------------
          Retrieve auth info from NV/Card  
        -------------------------------------------------------------------*/
        if(ds707_data_session_get_num_valid_profiles_in_nvruim() > 0 &&
           ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR) &&
               (ds3gdsdif_ehprd_is_pref_system(SYS_MODEM_AS_ID_1) == 
                                                                    FALSE) ) ||
             ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA ))
        {
          sip_profile_index = 
                           ds707_data_session_get_sip_active_profile_index();
          data_sess_profile_id = sip_profile_index + 
                           DATA_SESSION_NVRUIM_PROFILE_MIN;
          ds707_data_session_set_iface_requesting_profile(
                                                   instance_ptr->ps_iface_ptr, 
                                                   data_sess_profile_id,
                                                   ACTIVE_SUBSCRIPTION_1);
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Reading from card." );
          ds707_data_session_get_ppp_auth_info_for_nvruim_profile(
                           &(ppp_config->auth_info) );
        }
        else
        {
          is707_get_ppp_auth_info_from_nv( &(ppp_config->auth_info), 
                                         ds3gcfgmgr_read_legacy_nv_ex );
        }
#ifdef FEATURE_DS_MULTIPLE_NAI
        /*---------------------------------------------------------------------
          Laptop call: Multiple NAIs are applicable only for NV mode, so check 
          if phone is in NV mode before copying RM NAI.
        ---------------------------------------------------------------------*/
        if(((uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV) || 
           (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV)) && 
           (PS_IFACE_IS_CALL_FROM_TETHERED_HOST (instance_ptr->ps_iface_ptr)))
        {        
          DATA_IS707_MSG0(MSG_LEGACY_MED, "This is a laptop call (NV mode), "
                          "NV_DS_SIP_RM_NAI_I may be used if configured");
          ds707_copy_rm_nai(&(ppp_config->auth_info), 
                            ds3gcfgmgr_read_legacy_nv_ex);
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_MED, "NV_PPP_USER_ID_I is used.");
        }
#endif /* FEATURE_DS_MULTIPLE_NAI */

        /*---------------------------------------------------------------------
          Set the flags for username and password from client to FALSE 
          by default.
        ---------------------------------------------------------------------*/
        ppp_config->auth_info.is_userid_set_by_client = FALSE;
        ppp_config->auth_info.is_pwd_set_by_client = FALSE;
        if (FALSE == dsgen_iface_hdlr_use_internal_auth())
        {
          /*-----------------------------------------------------------------------
           check if username, passwd are valid in ps_iface, if so use them
          -----------------------------------------------------------------------*/
          if(instance_ptr->ps_iface_ptr->net_cfg_params.valid_fields &
                                         (uint32) NET_CFG_PRM_AUTH_USERNAME_MASK)
          {
            DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Use User id from PS iface");
            ppp_config->auth_info.user_id_len =
               instance_ptr->ps_iface_ptr->net_cfg_params.auth_info.params.user_id_len;
            memset(ppp_config->auth_info.user_id_info, 0, PPP_MAX_USER_ID_LEN);
            memscpy(ppp_config->auth_info.user_id_info,
                    PPP_MAX_USER_ID_LEN,
                    instance_ptr->ps_iface_ptr->net_cfg_params.auth_info.params.user_id_info,
                    ppp_config->auth_info.user_id_len);
            ppp_config->auth_info.is_userid_set_by_client = TRUE;
          }

          if(instance_ptr->ps_iface_ptr->net_cfg_params.valid_fields &
                                         (uint32) NET_CFG_PRM_AUTH_PASSWORD_MASK)
          {
            DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Use Password from PS iface");
            ppp_config->auth_info.passwd_len =
                instance_ptr->ps_iface_ptr->net_cfg_params.auth_info.params.passwd_len;
            memset( ppp_config->auth_info.passwd_info, 0, PPP_MAX_PASSWD_LEN);
            memscpy( ppp_config->auth_info.passwd_info,
                     PPP_MAX_PASSWD_LEN,
                     instance_ptr->ps_iface_ptr->net_cfg_params.auth_info.params.passwd_info,
                     ppp_config->auth_info.passwd_len);
            ppp_config->auth_info.is_pwd_set_by_client = TRUE;
          }
        }

        if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
        {
          ds707_configure_jcdma_params_for_ppp(ppp_config);
        }

        /*-------------------------------------------------------------------
          Start PPP on Um and transition to resyncing state
        -------------------------------------------------------------------*/
        if (ppp_resync( PPP_UM_SN_DEV, 
                        PPP_PROTOCOL_LCP, 
                        ppp_config) == -1)
        {
          /*-----------------------------------------------------------------
            If Um resync fails for some reason, kill the call
          -----------------------------------------------------------------*/
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "Couldn't resync Um, kill call");
          dssnet4i_transition_state( instance_ptr,
                                     DSSNET4I_LINK_CLOSING_STATE);
        }
        else
        {
          dssnet4i_transition_state( instance_ptr,
                                     DSSNET4I_LINK_RESYNC_STATE);
        }
        modem_mem_free(ppp_config, MODEM_MEM_CLIENT_DATA_CRIT);
        break;
      }

      /*--------------------------------------------------------------------
        Set back the fail reason to previous value
      ---------------------------------------------------------------------*/
      if (MIP_FAIL_REASON_HA_ADMIN_PROHIBIT == mip_status)
      {
        mip_status = MIP_FAIL_REASON_HA_FAILURE;
      }

      /* Fall through */
#ifdef FEATURE_CALL_THROTTLE
      /*--------------------------------------------------------------------
        This case will be executed if this is laptop call and if this is
        a mip only sockets calls.  This only posts failure if this is a 
        mip only sockets or network model call
      ---------------------------------------------------------------------*/
      if ( ( instance_ptr->is_te2_client == FALSE ) ||
           ( ( instance_ptr->is_te2_client == TRUE ) && 
             (ds_1x_runtime_profile_ptr->mip.ds_qcmip_val == 2 )
           )
         )
      {
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        {
          dctm_post_mip_call_status( MIP_FAILURE_EV,
                                     mip_status);
          ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
        }
      }
#endif /* FEATURE_CALL_THROTTLE */

    /* Fall through */
    /*--------------------------------------------------------------------
      No fall back to SIP when in NET UP/Resync state
    ---------------------------------------------------------------------*/
    case DSSNET4I_NET_UP_STATE:
    case DSSNET4I_LINK_RESYNC_STATE:
#ifdef FEATURE_DS_MOBILE_IP
      dssnet4i_set_mip_rrp_code(instance_ptr, mip_rrp_code);
#ifdef FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS
      if(ds707_pkt_mgr_mip_can_do_sess_close(mip_rrp_code))
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, 
                   "dssnet4i_process_mip_failure_ev: err 67, close session");
        ds707_close_hdr_session(FALSE);
      }
#endif /*FEATURE_DATA_MIP_AUTH_FAIL_CLOSE_SESS*/
#endif /* FEATURE_DS_MOBILE_IP */
      dssnet4i_transition_state( instance_ptr,
                                 DSSNET4I_LINK_CLOSING_STATE);      
      break;

    case DSSNET4I_CLOSED_STATE:
    case DSSNET4I_LINK_CLOSING_STATE:
      /* ignoring unexpected MIP_FAILURE_EV */
      break;

    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE:
#ifdef FEATURE_DS_MOBILE_IP 
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE: 
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
      /* ignoring unexpected MIP_FAILURE_EV */
      break;

  default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* end switch */

} /* dssnet4i_process_mip_failure_ev() */

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG

/*===========================================================================
FUNCTION DSSNET4I_PROCESS_MIP_DEREGED_EV()

DESCRIPTION
  This function processes the DSSNET4_MIP_DEREGED_EV. Changes state to 
  DSSNET4I_LINK_CLOSING_STATE if in DSSNET4I_WAITING_FOR_MIP_DEREG_STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_process_mip_dereged_ev
(
  dssnet4_sm_cb_type *instance_ptr           /* dssnet4 instance ptr      */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "MIP_DEREGED_EV recd for dssnet4 0x%x in state %d",
                   instance_ptr,
                   instance_ptr->state);

  switch(instance_ptr->state)
  {
    /* DSSNET4I_NET_UP_STATE needs to be handled, since RMSM side may 
     * call MIP deregistration if UM/RM option mismatched
     */
    case DSSNET4I_NET_UP_STATE:
    case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE:
      if(PS_IFACE_GET_NUM_SEC_FLOWS(instance_ptr->ps_iface_ptr) != 0)
      {
        (void)ps_iface_event_cback_reg(instance_ptr->ps_iface_ptr,
                                       IFACE_FLOW_DELETED_EV,
                                       instance_ptr->iface_flow_deleted_cbuf);
      }
      dssnet4i_transition_state( instance_ptr,  
                                 DSSNET4I_LINK_CLOSING_STATE);
      break;

    case DSSNET4I_LINK_CLOSING_STATE:
    case DSSNET4I_CLOSED_STATE:
      /* ignoring MIP_DEREG in CLOSING states */
      break;

    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE: 
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
    case DSSNET4I_LINK_RESYNC_STATE:
      /* Fall through */

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* end switch */

} /* dssnet4i_process_mip_dereged_ev() */
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION DSSNET4I_HANDLE_MIP_EV_CBACK()

DESCRIPTION
  Called when the MIP call come sup successfully or fails.
  This posts an appropriate event to the dssnet4 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_handle_mip_ev_cback
(
  mip_event_e_type        mip_event,             /* Down/Up/Resync         */
  mip_fail_reason_e_type  mip_fail_reason,
  byte                    mip_rrp_code,
  void                    *user_data
)
{
  dssnet4_sm_cb_type *instance_ptr = NULL;   /* dssnet4 instance ptr      */
  ds_mip_ppp_status  mip_status;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == user_data) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG3(MSG_LEGACY_MED, 
                  "Rcvd MIP event %d with reason %d rrp_code %d",
                  mip_event, mip_fail_reason, mip_rrp_code);

  instance_ptr = (dssnet4_sm_cb_type*)(user_data);

  switch(mip_event)
  {
    case MIP_SUCCESS_EV:
    {
      /*lint -save -e534 return value not required */
      (void)dssnet4_sm_post_event( instance_ptr, 
                                   DSSNET4_MIP_UP_SUCCESS_EV,NULL);
      /*lint -restore*/
      break;
    }
#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
    case MIP_DEREGED_EV:
    {
      /*lint -save -e534 return value not required */
      (void)dssnet4_sm_post_event( instance_ptr, 
                                   DSSNET4_MIP_DEREGED_EV, NULL);
      /*lint -restore*/
      break;
    }
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
    case MIP_FAILURE_EV:
    {
      mip_status.mip_fail_reason = mip_fail_reason;
      mip_status.mip_rrp_code = mip_rrp_code;
      mip_status.ppp_fail_reason = PPP_FAIL_REASON_NONE;
      mip_status.dssnet_instance = instance_ptr;
      /*lint -save -e534 return value not required */
      (void)dssnet4_sm_post_event( instance_ptr,
                                   DSSNET4_MIP_FAILURE_EV,
                                   &mip_status);
      /*lint -restore*/
      break;
    }
    default:
    {
      ASSERT(0);
      return;
    }
  }
} /* dssnet4i_handle_mip_ev_cback() */
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION DSSNET4I_HANDLE_PPP_EV_CBACK()

DESCRIPTION
  Called when the PPP comes up/down/resync.  This posts an appropriate event
  to the dssnet4 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_handle_ppp_ev_cback
(
  ppp_dev_enum_type   ppp_dev,                   /* PPP Device             */
  ppp_protocol_e_type protocol,                  /* PPP Protocol           */
  ppp_event_e_type    ppp_event,                 /* Down/Up/Resync         */
  void                *user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
)
{
  dssnet4_sm_cb_type *instance_ptr = NULL;      /* dssnet4 instance ptr      */
  ds_mip_ppp_status  ppp_status;
  ps_ip_addr_type    ip_addr;
  ppp_fail_reason_e_type  fail_reason = PPP_FAIL_REASON_NONE;
  
#ifdef FEATURE_DS_MOBILE_IP
  ppp_dev_opts_type *dev_opts = NULL;
#endif /* FEATURE_DS_MOBILE_IP */
  
#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
  ds707_pdn_context_cb_type *pdncntx_cb_ptr = NULL;
  ds707_pdncntx_ppp_event_action event_action = DS707_PDNCNTX_INVALID_ACTION;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
  ds707_pdn_throttle_info_type* throttle_info_ptr = NULL;
  ds707_data_session_profile_info_type *pdn_prof_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
  ds707_data_session_profile_info_type *profile_info_ptr = NULL;
#endif /* FEATURE_EHRPD */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == user_data) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG5( MSG_LEGACY_HIGH, 
         "Recd ppp dev %d event %d for dssnet4 0x%x. protocol %d, reason %d",
         ppp_dev, ppp_event, user_data, protocol, ppp_fail_reason);

  instance_ptr = (dssnet4_sm_cb_type*)(user_data);
  ASSERT(ppp_dev == PPP_UM_SN_DEV);
  
  if( NULL == instance_ptr )
  {
    return;
  }
  
  switch(ppp_event)
  {
    case PPP_UP_EV:
#ifdef FEATURE_DS_MOBILE_IP
      /*---------------------------------------------------------------------
        Allocate memory for dev_opts
      ---------------------------------------------------------------------*/
      dev_opts = (ppp_dev_opts_type*)
                 modem_mem_alloc(sizeof(ppp_dev_opts_type),
                                 MODEM_MEM_CLIENT_DATA_CRIT);      								  
      if(dev_opts == NULL)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "dssnet4i_handle_ppp_ev_cback():"
                        "memory allocation failed for dev_opts");
        dssnet4i_transition_state( instance_ptr, DSSNET4I_CLOSED_STATE);
        break;   
      }
      memset((void *)dev_opts, 0, sizeof(ppp_dev_opts_type));

      /*---------------------------------------------------------------------
        Determine if PPP has come up with SIP/MIP. If the interface has not
        been assigned an IP address, then PPP has come up MIP, else with SIP. 
      ---------------------------------------------------------------------*/
      (void) ppp_get_protocol_opts ( ppp_dev, 
                                     dev_opts );

      /*---------------------------------------------------------------------
        get_protocol copies the current WORK values in to the WANT field 
        (in anticipation of starting another PPP), so we example the WANT 
        field to see what was just negotiated.
      ---------------------------------------------------------------------*/
      if ( instance_ptr->want_mip == TRUE && 
           !(dev_opts->ipcp_info.want_mask[FSM_LOCAL] & IPCP_N_1ADDRESS) )
      {
        /*lint -save -e534 return value not required*/
        (void)dssnet4_sm_post_event( instance_ptr,
                                     DSSNET4_LINK_UP_WITH_MIP_EV,NULL);
        /*lint -restore*/
      }
      else
#endif /* FEATURE_DS_MOBILE_IP */
      {
        /*--------------------------------------------------------------------
        Verify IP address before posting event. If IP addr is 0.0.0.0 for IPV4 or 
        any invalid address, post DSSNET4_PPP_CLOSE_EV. Otherwise, it posts 
        DSSNET4_LINK_UP_WITH_SIP_EV.
        ----------------------------------------------------------------------*/
        ip_addr.type = IP_ANY_ADDR;
        ps_iface_get_cached_addr(instance_ptr->ps_iface_ptr, &ip_addr);
        if ( ( ip_addr.type == IP_ADDR_INVALID ) || 
             ( ( ip_addr.type == IPV4_ADDR ) && 
               ( ip_addr.addr.v4.ps_s_addr == 0)
             )
           )
        {
           DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                            "Invaid IP address type %d or invalid IP address",
                            ip_addr.type);
         /*lint -save -e534 return value not required*/
          (void)dssnet4_sm_post_event( instance_ptr,
                                       DSSNET4_PPP_CLOSE_EV,NULL);
          /*lint -restore*/

        }
        else
        {
          if ( instance_ptr->state == DSSNET4I_CLOSED_STATE &&
               (ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_DOWN || 
                ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_GOING_DOWN)
             )
          {
            DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                            "Link up when DSSNET SM in closed "
                            "state and iface is in down/going down state - "
                            "Ignore");
            
          }
          else
          {
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
            if ( (instance_ptr->is_ehrpd_call == TRUE) &&
                 ( (instance_ptr->state == DSSNET4I_LINK_OPENING_WITH_SIP_STATE) ||
                   (instance_ptr->state == DSSNET4I_LINK_RESYNC_STATE)
                 )
               )
            {
              /*-------------------------------------------------------------------
              Clear the v4_count since there is a successful v4 data connection. 
              in PS task context. 
              This is avoid the race condition between PS and ds TASK 
              If at the same time DSSNET6 will allocates the pdn throttle info ptr 
              to throttle v6 ip type for this PDN as part of handling
              PPP_SUBS_LIMITED_TO_V4_EV  and later DS task clears the pdn throttle info
              as part of handling LINK UP WITH SIP Event
              -------------------------------------------------------------------*/

              if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                          instance_ptr->ps_iface_ptr,
                                                          &pdn_prof_ptr) )
              {
                ds707_data_session_get_iface_current_profile(
                                                       instance_ptr->ps_iface_ptr,
                                                       &pdn_prof_ptr);
              }

              if (pdn_prof_ptr != NULL)
              {
                throttle_info_ptr = ds707_pdncntx_get_throttle_info_ptr(pdn_prof_ptr);
                if ( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
                {
                  ds707_pdncntx_clear_throttle_count( throttle_info_ptr,
                                                      DS707_PDNCNTX_IP_TYPE_V4 );
                }
              }
            }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
           /*lint -save -e534 return value not required*/
          (void)dssnet4_sm_post_event( instance_ptr, 
                                       DSSNET4_LINK_UP_WITH_SIP_EV,
                                       NULL);
          /*lint -restore*/
          }
        }
      }

#ifdef FEATURE_DS_MOBILE_IP
      modem_mem_free(dev_opts, MODEM_MEM_CLIENT_DATA_CRIT);
#endif /* FEATURE_DS_MOBILE_IP */  
      break;

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
    case PPP_SUBS_LIMITED_TO_V4_EV:
      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
      if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
      {
        DATA_ERR_FATAL("Cannot get PDN Control Block, do not proceed");
        return;
      }

      event_action = ds707_pdncntx_handle_ppp_ev( pdncntx_cb_ptr, 
                                                  instance_ptr->ps_iface_ptr,
                                                  ppp_event );
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "Event action: %d for dssnet4 0x%x", 
                       event_action, instance_ptr );

      if ( event_action == DS707_PDNCNTX_LINK_UP )
      {
        /*--------------------------------------------------------------------
        Verify IP address before posting event. If IP addr is 0.0.0.0 for IPV4 or 
        any invalid address, post DSSNET4_PPP_CLOSE_EV. Otherwise, it posts 
        DSSNET4_LINK_UP_WITH_SIP_EV.
        ----------------------------------------------------------------------*/
        ip_addr.type = IP_ANY_ADDR;
        ps_iface_get_cached_addr(instance_ptr->ps_iface_ptr, &ip_addr);
        if ( ( ip_addr.type == IP_ADDR_INVALID ) || 
             ( ( ip_addr.type == IPV4_ADDR ) && 
               ( ip_addr.addr.v4.ps_s_addr == 0) ) )
        {
           DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                            "Invaid IP address type %d or invalid IP address",
                            ip_addr.type);
         /*lint -save -e534 return value not required*/
          (void)dssnet4_sm_post_event( instance_ptr,
                                       DSSNET4_PPP_CLOSE_EV,NULL);
          /*lint -restore*/

        }
        else
        {
          if ( instance_ptr->state == DSSNET4I_CLOSED_STATE &&
               (ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_DOWN || 
                ps_iface_state(instance_ptr->ps_iface_ptr) == IFACE_GOING_DOWN)
             )
          {
            DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                            "Link up with event recd when DSSNET SM in closed "
                            "state and iface is in down/going down state - "
                            "Ignore");
            
          }
          else
          {
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
            if ( (instance_ptr->is_ehrpd_call == TRUE) &&
                 ( (instance_ptr->state == DSSNET4I_LINK_OPENING_WITH_SIP_STATE) ||
                   (instance_ptr->state == DSSNET4I_LINK_RESYNC_STATE)
                 )
               )
            {
              /*-------------------------------------------------------------------
              Clear the v4_count since there is a successful v4 data connection. 
              in PS task context. 
              This is avoid the race condition between PS and ds TASK 
              If at the same time DSSNET6 will allocates the pdn throttle info ptr 
              to throttle v6 ip type for this PDN as part of handling
              PPP_SUBS_LIMITED_TO_V4_EV  and later DS task clears the pdn throttle info
              as part of handling LINK UP WITH SIP Event
              -------------------------------------------------------------------*/

              if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                          instance_ptr->ps_iface_ptr,
                                                          &pdn_prof_ptr) )
              {
                ds707_data_session_get_iface_current_profile(
                                                       instance_ptr->ps_iface_ptr,
                                                       &pdn_prof_ptr);
              }

              if (pdn_prof_ptr != NULL)
              {
                throttle_info_ptr = ds707_pdncntx_get_throttle_info_ptr(pdn_prof_ptr);
                if ( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
                {
                  ds707_pdncntx_clear_throttle_count( throttle_info_ptr,
                                                      DS707_PDNCNTX_IP_TYPE_V4 );
                }
              }
            }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
           /*lint -save -e534 return value not required*/
          (void)dssnet4_sm_post_event( instance_ptr, 
                                       DSSNET4_LINK_UP_WITH_SIP_EV,
                                       NULL);
          /*lint -restore*/
        }
        }
      }/* event_action == DS707_PDNCNTX_LINK_UP */
      else if ( event_action == DS707_PDNCNTX_PPP_CLOSE )
      {
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
        /*-------------------------------------------------------------------
          Even if we are tearing down the PDN, clear the v4_count since
          there is a successful v4 data connection.
        -------------------------------------------------------------------*/
        if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                  instance_ptr->ps_iface_ptr,
                                                  &profile_info_ptr))
        {
          ds707_data_session_get_iface_current_profile(
                                                  instance_ptr->ps_iface_ptr,
                                                  &profile_info_ptr);
        }

        if ( profile_info_ptr != NULL )
        {
          throttle_info_ptr = ds707_pdncntx_get_throttle_info_ptr(profile_info_ptr);
          if ( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
          {
            ds707_pdncntx_clear_throttle_count( throttle_info_ptr,
                                                DS707_PDNCNTX_IP_TYPE_V4 );
          }
        }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
        /*lint -save -e534 return value not required*/
        (void)dssnet4_sm_post_event( instance_ptr,
                                     DSSNET4_PPP_CLOSE_EV,NULL);
        /*lint -restore*/
      } /* event_action == DS707_PDNCNTX_PPP_CLOSE */
      else 
      {
        ASSERT(0);        
        return;
      }
      break;

    case PPP_SUBS_LIMITED_TO_V6_EV:
      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
      if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
      {
          DATA_ERR_FATAL("Cannot get PDN Control Block, do not proceed");
          return;
      }

#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
      /*-----------------------------------------------------------------------
        Get the requesting profile id and call function to throttle ipv4
      -----------------------------------------------------------------------*/
      if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                    instance_ptr->ps_iface_ptr,
                                                    &profile_info_ptr) )
      {
        ds707_data_session_get_iface_current_profile(instance_ptr->ps_iface_ptr,
                                                     &profile_info_ptr);
      }
      if ( profile_info_ptr != NULL )
      {
        ds707_pdncntx_throttle_pdn_iptype( profile_info_ptr->data_session_profile_id, 
                                           DS707_PDNCNTX_IP_TYPE_V4,
                                           DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW,
                                           PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V6);      
      }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */

      event_action = ds707_pdncntx_handle_ppp_ev( pdncntx_cb_ptr, 
                                                  instance_ptr->ps_iface_ptr,
                                                  ppp_event ); 
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "Event action: %d for dssnet4 0x%x", 
                       event_action, instance_ptr );

      dssnet4_set_ps_call_end_reason(instance_ptr, 
                                     PS_NET_DOWN_REASON_EHRPD_ERR_SUBS_LIMITED_TO_V6);
      if ( event_action == DS707_PDNCNTX_PPP_CLOSE )
      {
        /*lint -save -e534 return value not required*/
        (void)dssnet4_sm_post_event( instance_ptr, 
                                     DSSNET4_PPP_CLOSE_EV,
                                     NULL );
        /*lint -restore*/
      }
      else if ( event_action == DS707_PDNCNTX_LINK_DOWN )
      {
        ppp_status.ppp_fail_reason = ppp_fail_reason;
#ifdef FEATURE_DS_MOBILE_IP
        ppp_status.mip_fail_reason = MIP_FAIL_REASON_NONE;
#endif  /* FEATURE_DS_MOBILE_IP */
        ppp_status.ppp_protocol = protocol;
        ppp_status.dssnet_instance = instance_ptr;

        /*lint -save -e534 return value not required*/
        (void)dssnet4_sm_post_event( instance_ptr, 
                                     DSSNET4_LINK_DOWN_EV,
                                     &ppp_status);
        /*lint -restore*/
      }
      else
      {
        ASSERT(0);
        return;
      }
      break;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
#endif /* FEATURE_EHRPD */

    /* case PPP_FAILURE_EV: */
    case PPP_DOWN_EV:
#ifdef FEATURE_EHRPD
      /*---------------------------------------------------------------------
        Store the PPP fail reason if it already not set in case of AUTH 
        failure on eHRPD. No other processing is required for AUTH case
      ----------------------------------------------------------------------*/ 
      if ((TRUE == instance_ptr->is_ehrpd_call) 
          && (PPP_PROTOCOL_AUTH == protocol))
      {
        fail_reason = dssnet4i_get_ppp_failure_code(instance_ptr);
        if(PPP_FAIL_REASON_NONE == fail_reason)
        {
          dssnet4i_set_ppp_failure_code(instance_ptr, ppp_fail_reason);
        }
        else
        {
          DATA_IS707_MSG1(MSG_LEGACY_MED, 
                          "PPP fail reason is already set to %d. "
                          "Hence ignore new PPP fail reason.", 
                          fail_reason);
        }
        return;
      }
#endif
    /* fall through */

#ifdef FEATURE_EHRPD
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
      if ( (instance_ptr->is_ehrpd_call == TRUE) && 
           (instance_ptr->state == DSSNET4I_LINK_OPENING_WITH_SIP_STATE) )
      {
        dssnet4_set_ps_call_end_reason( instance_ptr, 
                                 dssnet4i_convert_ppp_failure_code(ppp_fail_reason) );

        if( (ppp_fail_reason == PPP_FAIL_REASON_VSNCP_TIMEOUT) || 
            (ppp_fail_reason == PPP_FAIL_REASON_VSNCP_FAILURE) ||
            ( (ppp_fail_reason >= PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MIN) &&
              (ppp_fail_reason < PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MAX) &&
              (ppp_fail_reason != PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_ID_IN_USE) &&
              (ppp_fail_reason != PPP_FAIL_REASON_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED) &&
              (!((ppp_fail_reason == PPP_FAIL_REASON_VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN) &&
                (FALSE == ds707_pdncntx_is_default_apn(profile_info_ptr))) )
            )
          )
        {
          if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                    instance_ptr->ps_iface_ptr,
                                                    &profile_info_ptr) )
          {
            ds707_data_session_get_iface_current_profile(instance_ptr->ps_iface_ptr,
                                                         &profile_info_ptr);
          }
          if ( profile_info_ptr != NULL )
          {
            /*-----------------------------------------------------------------------
              Trigger throttling
            -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_FALLBACK_LIST
              /*-----------------------------------------------------------------------
                Throttle only if we get PPP_DOWN in link_opening state and no PDN fallback
                is proposed.
                Get the requesting profile id and call function to throttle ipv4
              -----------------------------------------------------------------------*/
              if(!ds707_pkt_mgr_fb_list_fallback(
                           instance_ptr->ps_iface_ptr,
                           profile_info_ptr->apn_string, 
                           profile_info_ptr->apn_string_len)
                 )
#endif
              {
                  ds707_pdncntx_throttle_pdn_iptype( profile_info_ptr->data_session_profile_id, 
                                                     DS707_PDNCNTX_IP_TYPE_V4,
                                                     DS707_PDNCNTX_THROTTLE_TYPE_FAILURE,
                                                     dssnet4i_convert_ppp_failure_code(ppp_fail_reason));
                }
              }
            }        
          }
      /*-----------------------------------------------------------------------
        reconnection not allowed error: Trigger infinite throttling regardless
        of the link state
      -----------------------------------------------------------------------*/
      if (ppp_fail_reason == PPP_FAIL_REASON_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED)
      {
        if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                    instance_ptr->ps_iface_ptr,
                                                    &profile_info_ptr) )
        {
          ds707_data_session_get_iface_current_profile(instance_ptr->ps_iface_ptr,
                                                       &profile_info_ptr);
        }
        if (profile_info_ptr != NULL)
        {
            DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                             "iface4 PPP DOWN with reconnect not allowed!");
            ds707_pdncntx_throttle_pdn_iptype( profile_info_ptr->data_session_profile_id, 
                                               DS707_PDNCNTX_IP_TYPE_V4,
                                               DS707_PDNCNTX_THROTTLE_TYPE_INFINITE,
                                               dssnet4i_convert_ppp_failure_code(ppp_fail_reason));
        }
      }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
      ppp_status.ppp_fail_reason = ppp_fail_reason;

      /*------------------------------------------------------------------- 
        If PPP fail reason is already set to value other than NONE, then 
        ignore setting fail reason again.
      --------------------------------------------------------------------*/ 
      fail_reason = dssnet4i_get_ppp_failure_code(instance_ptr);
      if( fail_reason == PPP_FAIL_REASON_NONE )
      {
        dssnet4i_set_ppp_failure_code(instance_ptr, ppp_fail_reason);
      }
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_MED, 
                        "PPP fail reason is already set to %d. "
                        "Hence ignore new PPP fail reason.", 
                        fail_reason);
      }

#ifdef FEATURE_DS_MOBILE_IP
      ppp_status.mip_fail_reason = MIP_FAIL_REASON_NONE;
#endif  /* FEATURE_DS_MOBILE_IP */
      ppp_status.ppp_protocol = protocol;
      ppp_status.dssnet_instance = instance_ptr;

      /*------------------------------------------------------------------- 
        Required for ehrpd-1x/hrpd handoff. If all ifaces are cleaned,
        move sys_info state to NO_SYSTEM. Next time call is brought up,
        it will be changed to EPC or NON_EPC depending on the system.
        Removed as part of CR 734709
      --------------------------------------------------------------------*/ 
      #if 0
      if ( protocol == PPP_PROTOCOL_LCP )
      {
        ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "LCP down move sys_info to NO_SYSTEM" );
      }
      #endif
      /*lint -save -e534 return value not required*/
      (void)dssnet4_sm_post_event( instance_ptr, 
                                   DSSNET4_LINK_DOWN_EV,
                                   &ppp_status);
      /*lint -restore*/
      break;

    case PPP_RESYNC_EV:
      /*lint -save -e534 return value not required*/
      (void)dssnet4_sm_post_event( instance_ptr,
                                   DSSNET4_LINK_RESYNC_EV,
                                   NULL);
      /*lint -restore*/
      break;

    default:
      ASSERT(0);
      return;
  }
} /* dssnet4i_handle_ppp_ev_cback() */

/*===========================================================================
FUNCTION DSSNET4I_HANDLE_PHYS_LINK_EV_CBACK()

DESCRIPTION
  Called when the PHYS LINK goes up/down/gone.  This posts an appropriate
  event to the dssnet state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_handle_phys_link_ev_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
)
{

  dssnet4_sm_cb_type *instance_ptr = NULL;      /* dssnet4 instance ptr      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == user_info_ptr) 
  {
    ASSERT(0);
    return;
  }

  instance_ptr = (dssnet4_sm_cb_type*)(user_info_ptr);
  DATA_MDM_ARG_NOT_CONST(iface_ptr);

  DATA_IS707_MSG3( MSG_LEGACY_MED, 
                   "Recd ps_iface event %d for dssnet4 0x%x in state %d",
                   event, user_info_ptr, instance_ptr->state);

  switch(event)
  {
    case IFACE_PHYS_LINK_UP_EV: 
      /*lint -save -e534 return value not required*/
      (void)dssnet4_sm_post_event( instance_ptr, 
                                   DSSNET4_PHY_LINK_UP_EV,NULL);
      /*lint -restore*/
      break;

    case IFACE_PHYS_LINK_GONE_EV: 
      /*lint -save -e534 return value not required*/
      (void)dssnet4_sm_post_event( instance_ptr, 
                                   DSSNET4_PHY_LINK_GONE_EV,NULL);
      /*lint -restore*/
      break;

    case IFACE_PHYS_LINK_DOWN_EV: 
      /*lint -save -e534 return value not required*/
      (void)dssnet4_sm_post_event( instance_ptr, 
                                   DSSNET4_PHY_LINK_DOWN_EV,NULL);
      /*lint -restore*/
      break;
    
    case IFACE_FLOW_DELETED_EV:
      /*lint -save -e534 return value not required*/
      (void)dssnet4_sm_post_event( instance_ptr, 
                                   DSSNET4_FLOW_DELETED_EV,NULL);
      /*lint -restore*/
      break;
      
    case IFACE_FLOW_ADDED_EV:
    case IFACE_ENABLED_EV: 
    case IFACE_DISABLED_EV: 
    case IFACE_DOWN_EV: 
    case IFACE_ROUTEABLE_EV: 
    case IFACE_UP_EV: 
    case IFACE_FLOW_ENABLED_EV: 
    case IFACE_FLOW_DISABLED_EV: 
    case IFACE_ADDR_CHANGED_EV: 
    case IFACE_DELETE_EV: 
    default:
      ASSERT(0);
      return;
  }
} /* dssnet4i_handle_phys_link_ev_cback() */

#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION DSSNET4I_1X_MIP_SUPPORT()

DESCRIPTION
  Check if mobile can establish mobile IP call. 

  To do MIP in an IS-835 network,
    - p_rev must be greater than or equal to 6, 
    - qcmdr must be equal to 3,
    - MIP meta state machine must be initialized

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - mobile IP is supported
  FALSE - mobile IP is not supported

SIDE EFFECTS
  None
===========================================================================*/
boolean dssnet4_1x_mip_support
(
  void
)
{
  word   p_rev_index;          /* holds current sys's p_rev index */
  uint8  qcmdr_val = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_MDR_VAL,(void *)&qcmdr_val,
                                          DS707_NON_ARRAY);

  /*-------------------------------------------------------------------------
    Retrieve the currently value of the P_REV being used
  -------------------------------------------------------------------------*/
  p_rev_index = ds707_p_rev_get_index();

  /*-------------------------------------------------------------------------
    Determine whether we can do mip.  
  -------------------------------------------------------------------------*/
  if( (p_rev_index >= DS707_P_REV_6) &&
      (qcmdr_val == DS_MDR_MODE_SO33_PREF) &&
      (mip_meta_sm_is_initialized() == TRUE)  
                                        // can't call directly if in DS task
    )
  {
    DATA_IS707_MSG2( MSG_LEGACY_MED, 
                     "MIP supported. PREV = %d, QCMDR=%d", 
                     p_rev_index,
                     qcmdr_val);
    return TRUE;
  }
  else
  {
    DATA_IS707_MSG2( MSG_LEGACY_MED, 
                     "MIP NOT supported. PREV = %d, QCMDR=%d",
                     p_rev_index, 
                     qcmdr_val);
    return FALSE;
  }

} /* dssnet4_1x_mip_support() */
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION DSSNET4I_TRANSITION_STATE()

DESCRIPTION
  This function is called whenever the state changes. The variable is set 
  and housekeeping done for each state.
  
  Any change to state should be through this function only.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the state.
===========================================================================*/
static void dssnet4i_transition_state
(
  dssnet4_sm_cb_type  *instance_ptr,          /* dssnet4 instance ptr      */
  dssnet4i_state_type  new_state         /* new state to change to */
)
{
  dssnet4i_state_type  old_state;  /* old state              */
  ppp_dev_opts_type    *ppp_config = NULL;          
  boolean              mip_call = FALSE;
  int                  ret_val;
  int16                err_num;
  ds707_data_session_profile_info_type 
	                   *data_sess_profile_info_ptr=NULL;
  
#ifdef FEATURE_HDR_QOS
  ps_flow_type        *ps_flow_ptr_arr[DS707_MAX_DS_FLOWS];
  uint8                num_ps_flows = 0;
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_EHRPD
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
  ppp_ncp_config_union_type  ncp_config;
  ppp_event_alloc_info_type  ppp_event_alloc_info;
  ppp_ncp_handle_type        ncp_handle;
  ds707_sys_chg_sm_e         sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
  ds707_pdn_throttle_info_type* throttle_info_ptr = NULL;
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
#endif /* FEATURE_EHRPD */
#ifdef FEATURE_DS_MOBILE_IP_PERF
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
#endif
  uint32   qcdns_val[MAX_QCDNS_VAL] = {0};
  ps_ip_addr_type pri_dns_addr;
  ps_ip_addr_type sec_dns_addr;
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
  int16 ps_errno;
#endif
#if defined(FEATURE_DATA_EHRPD_DUAL_IP) || defined(FEATURE_DATA_PS_IPV6)
  ps_iface_type *pdn_v6_iface_ptr = NULL;
#endif
  uint32 phys_flow_mask = 0;
  ps_phys_link_type *phys_link_ptr=NULL;
  ds707_data_session_profile_id data_sess_profile_id = 
                                    DATA_SESSION_PROFILE_ID_INVALID;
  uint8    sip_profile_index = 0;
  mc_queue_cmd_status_type       queue_status;
  ps_iface_type*                 iface6_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr) 
  {
    ASSERT(0);
    return;
  }

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_QCDNS_VAL,
                                          (void *)qcdns_val,MAX_QCDNS_VAL);
  /*-------------------------------------------------------------------------
    Changing the state variable.
  -------------------------------------------------------------------------*/
  old_state = instance_ptr->state;
  DATA_IS707_MSG3( MSG_LEGACY_MED, 
                  "dssnet4 0x%x transition state %d to %d",
                  instance_ptr, old_state, new_state);

  instance_ptr->state = new_state;

  /*-------------------------------------------------------------------------
    Executing state based functionality.
  -------------------------------------------------------------------------*/
  /*lint -save -e825 suppressing error 825, fall thru intentional */
  switch (instance_ptr->state)
  {
    case DSSNET4I_CLOSED_STATE:
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "dssnet4 0x%x IP4 network is down", instance_ptr);

      /*---------------------------------------------------------------------
        Enable flow to apps - needed to allow MIP to do ICMP writes.
        Hopefully, iface in configuring state will prevent user-level 
        applications from writing.  Check this with Anup when he gets back.
      ---------------------------------------------------------------------*/
      ps_iface_enable_flow( instance_ptr->ps_iface_ptr,
                            DS_FLOW_CTRL_DSSNET4_MASK);
    
      /*---------------------------------------------------------------------
        Set lcp_auth_disabled, te2_call_override_qcmip, is_te2_client and 
        disable_ipcp_dns_opt to FALSE.
      ---------------------------------------------------------------------*/
      instance_ptr->qnc_call = FALSE;
      instance_ptr->te2_call_override_qcmip = FALSE;
      instance_ptr->is_te2_client = FALSE;
      instance_ptr->disable_ipcp_dns_opt = FALSE;

#ifdef FEATURE_DS_MOBILE_IP
       /* It's possible to exit a MIP session even if this dssnet instance
          didn't start it. To avoid such situation, ensure that this dssnet
          instance started the mip session */
       if (dshdr_get_mobile_ip_uses_ccoa() == FALSE && 
           instance_ptr->mip_call_started == TRUE)
       {
         /*---------------------------------------------------------------------
           Tell MIP to close (will be ignored if wasn't open)
         ---------------------------------------------------------------------*/
         mip_meta_sm_post_event( MSM_EXIT_MIP_EV, 
                                 instance_ptr->ps_iface_ptr );
         mip_reset_config();
         if ( instance_ptr->mip_event_handle )
         {
           (void) mip_event_deregister_all( instance_ptr->mip_event_handle );
         }
       }
#endif /* FEATURE_DS_MOBILE_IP */
       /* Setting to false outside of the above if() check so we ensure the
          flag is set to false when the next call comes in */
       instance_ptr->mip_call_started = FALSE;

      /*---------------------------------------------------------------------
        Take the default route through this interface out
      ---------------------------------------------------------------------*/
      /*lint -save -e534 return value not required*/
      (void)route_flush( instance_ptr->ps_iface_ptr);
      /*lint -restore*/

#ifdef FEATURE_HDR_QOS
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Release all the secondary flows");
      num_ps_flows =0;
      if(ds707_sec_pkt_mgr_get_all_secondary_flows(instance_ptr->ps_iface_ptr,
                                                   ps_flow_ptr_arr,
                                                   DS707_MAX_DS_FLOWS,
                                                   &num_ps_flows) == TRUE)
      {
        ds707_sec_pkt_mgr_release_qos_resources(
                                      ps_flow_ptr_arr,
                                      num_ps_flows,
                                      DS707_SEC_PKT_MGR_AQOS_S_DQOS_S,
                                      DS707_SEC_PKT_FORCEFULLY_CLEAN_DS_FLOW,
                                      PS_EIC_NOT_SPECIFIED
                                    );
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Couldn't get sec flows");
      }
        
#endif /* FEATURE_HDR_QOS */

      ps_flow_go_null_ind(
              PS_IFACE_GET_DEFAULT_FLOW(instance_ptr->ps_iface_ptr),
              PS_EIC_NOT_SPECIFIED);


#ifdef FEATURE_EHRPD
      sys_chg_sm_state = ds707_query_sys_chg_sm_state();

      /*-------------------------------------------------------------------
        If sys chg sm is performing special cleanup, do not interrupt by
        posting phys link go null cmd here. When the cleanup procedure is
        complete, sys chg sm will issue phys link abort that'll end up
        triggering phys link gone ev to DSSNETs. DSSNET SM will then
        transition from closed->closed state and post iface down ind
       ------------------------------------------------------------------*/
      if (DS707_SYS_CHG_SM_WAIT_FOR_DORM == sys_chg_sm_state ||
          DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG == sys_chg_sm_state)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sys Chg SM performing special " 
                        "cleanup. Do not trigger ps_phys_link_go_null_cmd"); 
        ret_val = -1;
        err_num = DS_EWOULDBLOCK;
      }
      else
#endif /* FEATURE_EHRPD */
      {
        ret_val = ps_phys_link_go_null_cmd (
                  PS_IFACE_GET_PHYS_LINK(instance_ptr->ps_iface_ptr),
                  &err_num, NULL);
      }

      if(ret_val == 0)
      {
      /* Stop the DHCP instance before giving down indication
       */
      (void) dshdr_stop_dhcp(instance_ptr->ps_iface_ptr);

#ifdef FEATURE_DATA_PS_IWLAN_3GPP2
      /*---------------------------------------------------------------------
        Decrement the number of IWLAN+3GPP2 interface used count.
      ---------------------------------------------------------------------*/
        if (num_used_3gpp2_v4_ifaces > 0 )
        {
          num_used_3gpp2_v4_ifaces--;
        }
#endif /* FEATURE_DATA_PS_IWLAN_3GPP2 */

        /* Get PS call end reason before sending to PS */
        dssnet4i_get_ps_call_end_reason(instance_ptr);

      /*---------------------------------------------------------------------
          Deregister the phys_link events
      ---------------------------------------------------------------------*/
        dssnet4_dereg_phys_link_events(instance_ptr);

        /*-------------------------------------------------------------------
          Enter crit sect to prevent any post proc on this iface 
          while down ind is being posted here
        ------------------------------------------------------------------*/
        PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
        if(instance_ptr->iface_down_ind_cb != NULL)
        {
          DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                  "V4 iface 0x%x:%d (0x%x) is down. Calling iface_down_ind_cb",
                  instance_ptr->ps_iface_ptr->name, 
                  instance_ptr->ps_iface_ptr->instance,
                  instance_ptr->ps_iface_ptr);

          instance_ptr->iface_down_ind_cb(instance_ptr->ps_iface_ptr,
                                          instance_ptr->call_end_reason);
        }
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);


        /*---------------------------------------------------------------------
          If all ifaces are down & the current system is not EPC system, 
          change sys_info to NO_SYSTEM. This change is also done when MH gets 
          PPP down ev with LCP protocol. Below change is required when ppp_abort
          is called, and MH deregisters from PPP events. MH will not get PPP 
          down ev, and sys info will not be cleared. In case of EPC this will be 
          handled by device manager
        ---------------------------------------------------------------------*/
        if((ds707_pkt_mgr_all_ifaces_down() == TRUE)
            && (DS707_SYS_INFO_EPC_SYSTEM 
                != ds707_sys_chg_hdlr_get_current_system()))
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "all ifaces are in down state, "
                                           "clear sys_info to NO_SYSTEM");
          ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);
        }
        else
        {
          DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
          "all ifaces are not down or in EPC system, no change to sys_info %d",
           ds707_sys_chg_hdlr_get_current_system() );
        } 
      }

      if ((ret_val < 0) && ( err_num != DS_EWOULDBLOCK))
      {
        DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                        "phy link down fail. err_num %d, ret_val %d",
                        err_num, ret_val);
      }

      /*---------------------------------------------------------------------
        Invoke the ppp_resync_done_cb to notify the completion of the PPP
        resync (either successfully or failed resync). Also unregister the
        call back right after invoking it.
      ---------------------------------------------------------------------*/
      if (instance_ptr->ppp_resync_done_cb != NULL)
      {
        instance_ptr->ppp_resync_done_cb(instance_ptr);
        instance_ptr->ppp_resync_done_cb = NULL;
      }

      /* If NCP resync was in progress, reset ncp_resync_started flag */
      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
      if ( DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) &&
           ds707_pdncntx_get_ncp_resync_started(pdncntx_cb_ptr) == TRUE
         )
      {
        ds707_pdncntx_set_ncp_resync_started(pdncntx_cb_ptr, FALSE);
      }

#ifdef FEATURE_DS_MOBILE_IP_PERF
      if ( instance_ptr->dssnet_started_mip_call )
      {
        instance_ptr->dssnet_started_mip_call = FALSE;

        /*-------------------------------------------------------------------
          M.IP call setup is complete for socket calls only.
        -------------------------------------------------------------------*/
        mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_CALL_SETUP_DELAY);
        mip_perf_delay( mip_perf_info_ptr,
                        PERF_TOTAL_CALL_SETUP_DELAY,
                        PERF_CANCEL_TS );
      }
#endif /* FEATURE_DS_MOBILE_IP_PERF */

      if ( instance_ptr->ppp_event_handle != NULL )
      {
        if (FALSE == instance_ptr->is_ehrpd_call)
        {
          (void) ppp_event_deregister( 
                               instance_ptr->ppp_event_handle,
                               PPP_PROTOCOL_LCP,
                               (ppp_event_mask_e_type) ( PPP_DOWN_EV_MASK | 
                                                         PPP_RESYNC_EV_MASK) );

          (void) ppp_event_deregister( 
                                instance_ptr->ppp_event_handle,
                                PPP_PROTOCOL_AUTH,
                                (ppp_event_mask_e_type) (PPP_FAILURE_EV_MASK |
                                                         PPP_STARTING_EV_MASK));

          (void) ppp_event_deregister(
                                 instance_ptr->ppp_event_handle,
                                 PPP_PROTOCOL_IPCP,
                                 (ppp_event_mask_e_type) ( PPP_UP_EV_MASK | 
                                                           PPP_RESYNC_EV_MASK | 
                                                           PPP_DOWN_EV_MASK) );
        }  

        else
        {
          (void) ppp_event_deregister(instance_ptr->ppp_event_handle,
                                      PPP_PROTOCOL_AUTH,
                                      PPP_FAILURE_EV_MASK);

#ifdef FEATURE_EHRPD
          (void) ppp_event_deregister( 
                      instance_ptr->ppp_event_handle,
                      PPP_PROTOCOL_VSNCP,
                      (ppp_event_mask_e_type)( PPP_UP_EV_MASK | 
                                               PPP_RESYNC_EV_MASK | 
                                               PPP_DOWN_EV_MASK 
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
                                               | PPP_SUBS_LIMITED_TO_V4_EV_MASK 
                                               | PPP_SUBS_LIMITED_TO_V6_EV_MASK 
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
                                              ) );
#endif /* FEATURE_EHRPD */
        }

        if ( ppp_event_dealloc( PPP_UM_SN_DEV,
                                &(instance_ptr->ppp_event_handle ) ) == -1 )
        {
          ERR_FATAL( "Unable to dealloc PPP event handle", 0, 0, 0 );
        }

        instance_ptr->ppp_event_handle = NULL;
      }      
#ifdef FEATURE_EHRPD
      if(instance_ptr->is_ehrpd_call == TRUE)
      {
        instance_ptr->is_ehrpd_call = FALSE;
      }
#endif /*FEATURE_EHRPD */

      dssnet4_set_is_bringup_phys_link(instance_ptr, FALSE);
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_LINK_OPENING_WITH_MIP_STATE:
      mip_call = TRUE;
#endif /* FEATURE_DS_MOBILE_IP */
    /* Fall through */

    case DSSNET4I_LINK_OPENING_WITH_SIP_STATE:
      /*-----------------------------------------------------------------
        Deregister Phys link down event, since we no longer need
        it after Link Opening state.
      -----------------------------------------------------------------*/
      DATA_IS707_MSG2( MSG_LEGACY_MED, 
                       "Link Opening State: dereg IFACE_PHYS_LINK_DOWN_EV. "
                       "re-setting graceful_ppp_close for IFACE: 0x%x:%d",
                       instance_ptr->ps_iface_ptr->name,
                       instance_ptr->ps_iface_ptr->instance);
      ps_iface_event_cback_dereg( instance_ptr->ps_iface_ptr,
                                  IFACE_PHYS_LINK_DOWN_EV,
                                  instance_ptr->phys_link_down_cbuf);

       /*---------------------------------------------------------------------
        Set PPP termination policy.
      ---------------------------------------------------------------------*/
      if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
      {
        PS_IFACE_SET_GRACEFUL_CLOSE(instance_ptr->ps_iface_ptr, FALSE);
      }
      else
      {
        PS_IFACE_SET_GRACEFUL_CLOSE(instance_ptr->ps_iface_ptr, TRUE);
      }
      /*-----------------------------------------------------------------------
        Get the requesting profile id from the iface pointer and then get the
        corresponding profile for that profile id
      -----------------------------------------------------------------------*/
      if ( mip_call == FALSE &&
           ds_qcmip_get_runtime_val() == 1 &&
           ds707_data_session_get_num_valid_profiles_in_nvruim() > 0 &&
           ( ( (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR) &&
               (ds3gdsdif_ehprd_is_pref_system(SYS_MODEM_AS_ID_1) == 
                                                                    FALSE) ) ||
             ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA ) )
      {
        sip_profile_index = ds707_data_session_get_sip_active_profile_index();
        data_sess_profile_id = sip_profile_index + 
                             DATA_SESSION_NVRUIM_PROFILE_MIN;
        ds707_data_session_set_iface_requesting_profile(
                                                   instance_ptr->ps_iface_ptr, 
                                                   data_sess_profile_id,
                                                   ACTIVE_SUBSCRIPTION_1);
      }

      if ( FALSE == ds707_data_session_get_iface_requesting_profile(
                                                instance_ptr->ps_iface_ptr,
                                                &data_sess_profile_info_ptr) )
      {
        ds707_data_session_get_iface_current_profile(
                                    instance_ptr->ps_iface_ptr,
                                    &data_sess_profile_info_ptr);
      }

      if ( data_sess_profile_info_ptr == NULL )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                         "Invalid profile in PDN, posting DSSNET4_LINK_DOWN_EV");

        (void)dssnet4_sm_post_event( instance_ptr, 
                                     DSSNET4_LINK_DOWN_EV,NULL);
        return; 
      }

#ifdef FEATURE_EHRPD
      /*---------------------------------------------------------------------
	    In run time, if the traffic channel is 1x, bring up IPCP call else, 
	    try VSNCP call. 
      ---------------------------------------------------------------------*/
      if(ds707_pkt_is_ehrpd_mode_of_operation())
      {
#ifdef FEATURE_DATA_EHRPD_PDN_THROTTLE
        /*---------------------------------------------------------------------
          if PDN is throttled, and v4 throttle timer is running, do not
          allow this call.
	---------------------------------------------------------------------*/
        throttle_info_ptr = ds707_pdncntx_get_throttle_info_ptr(data_sess_profile_info_ptr);
        if ( DS707_THROTTLE_INFO_PTR_IS_VALID(throttle_info_ptr) )
        {
          if( (PS_IFACE_IS_IN_USE(instance_ptr->ps_iface_ptr)) &&
              ((rex_get_timer(&(throttle_info_ptr->ipv4_throttle_info.throttle_timer)) != 0) ||
               (DS707_PDNCNTX_THROTTLE_TYPE_INFINITE == 
                    throttle_info_ptr->ipv4_throttle_info.throttle_type)
              ) 
            )
          {
            /* Set PDN IPV4 Throttled info to PS end reason */
            if (throttle_info_ptr->ipv4_throttle_info.throttle_type 
                  == DS707_PDNCNTX_THROTTLE_TYPE_DISALLOW)
            {
              dssnet4_set_ps_call_end_reason(
                instance_ptr, 
                PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED );
            }
            else if(throttle_info_ptr->ipv4_throttle_info.throttle_type 
                     == DS707_PDNCNTX_THROTTLE_TYPE_FAILURE )
            {
              dssnet4_set_ps_call_end_reason(
                instance_ptr, 
                PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED );
            }
            else if(throttle_info_ptr->ipv4_throttle_info.throttle_type 
                  == DS707_PDNCNTX_THROTTLE_TYPE_INFINITE )
            {
              DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                               "V4: PDN infinitely throttled because of reconnection disallowed");
              dssnet4_set_ps_call_end_reason(
                instance_ptr, 
                PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED );
            }

            DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                             "PDN throttled for V4, posting DSSNET4_LINK_DOWN_EV");
           
            (void)dssnet4_sm_post_event( instance_ptr, 
                                         DSSNET4_LINK_DOWN_EV,NULL);
            return; 
          }
        }
#endif /* FEATURE_DATA_EHRPD_PDN_THROTTLE */
        /*---------------------------------------------------------------------
          When system changes from 1x<->DO, and DS hasnt received IDM change
          from CM, if a call is attempted on EPC, we should not allow the
          call till the non-EPC PPP is torn down. Hence set system to EPC
          only if there is no other call on non-EPC.
        ---------------------------------------------------------------------*/
        if( ds707_sys_chg_hdlr_get_current_system() 
                                              == DS707_SYS_INFO_NON_EPC_SYSTEM)
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
               "EPC call requested when system is non-EPC, posting link_down");
          (void)dssnet4_sm_post_event( instance_ptr, DSSNET4_LINK_DOWN_EV,NULL);
          return;
        }

        instance_ptr->is_ehrpd_call = TRUE;

        /*---------------------------------------------------------------------
          If the Mode-Handler has allocated atleast one PDN it means that the 
          PPP device is configured and need not be configured again. Device 
          manager is used for configuring PPP device & registering LCP/AUTH
        ---------------------------------------------------------------------*/
        if (0 != ds707_devmgr_start(PPP_UM_SN_DEV, data_sess_profile_info_ptr))
        {
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "PPP Configure Device Failed, do not proceed");
          (void)dssnet4_sm_post_event( instance_ptr,
                                       DSSNET4_LINK_DOWN_EV,NULL);
          return;
        }

        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
        if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
        {
            DATA_IS707_MSG0( MSG_LEGACY_MED, 
                             "Cannot get PDN Control Block, do not proceed");
            (void)dssnet4_sm_post_event( instance_ptr,
                                         DSSNET4_LINK_DOWN_EV,NULL);
            return; 
        }
        /*--------------------------------------------------------------------- 
          Configure NCP only if it is the first DSSNET that is coming up
        ---------------------------------------------------------------------*/   
        if( ! ( ds707_pdncntx_get_ncp_started( pdncntx_cb_ptr ) ) )
        {
          DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                           "ds707_pdncntx_configure_vsncp: dssnet4 0x%x, pdn context cb 0x%x",
                           instance_ptr, pdncntx_cb_ptr );
          if ( ds707_pdncntx_configure_vsncp( pdncntx_cb_ptr,
                                              &(ncp_config.vsncp_3gpp2),
                                              data_sess_profile_info_ptr ) 
                                              == FALSE )
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                             "PPP Configure VSNCP Failed, do not proceed");
            dssnet4_set_ps_call_end_reason(instance_ptr, 
                                    PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_CONFIG_FAILURE);
            (void)dssnet4_sm_post_event( instance_ptr,
                                         DSSNET4_LINK_DOWN_EV,NULL);
            return; 
          }
             
          /*------------------------------------------------------------------- 
            Since we are in the v4 Statemachine, make sure that the profile does 
            configure the VSNCP with the correct req_addr_type
          -------------------------------------------------------------------*/
          if ( ( ncp_config.vsncp_3gpp2.option_vals.req_addr_type != 
                 VSNCP_3GPP2_PDN_TYPE_IPV4 ) && 
               ( ncp_config.vsncp_3gpp2.option_vals.req_addr_type != 
                 VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ) )
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                             "Req Addr type Mismatch in the Profile");
            (void)dssnet4_sm_post_event( instance_ptr,
                                         DSSNET4_LINK_DOWN_EV,NULL);
            return; 
          }                    
       
          ncp_handle = ppp_ncp_alloc( PPP_UM_SN_DEV, 
                                      PPP_NCP_VSNCP_3GPP2,
                                      &ncp_config );
                     
          if ( ncp_handle == NULL )
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                             "Cannot Allocate NCP, do not proceed");
            (void)dssnet4_sm_post_event( instance_ptr,
                                         DSSNET4_LINK_DOWN_EV,NULL);
            return; 
          } 
          ds707_pdncntx_set_ncp_handle(pdncntx_cb_ptr, ncp_handle);       
        }/* Check if ds707_pdncntx_set_ncp_started */

        /*-------------------------------------------------------------------
          Register for PPP UP/DOWN/RESYNC Events.
        -------------------------------------------------------------------*/      
        if( (old_state == DSSNET4I_LINK_OPENING_WITH_MIP_STATE ) &&
            (instance_ptr->ppp_event_handle != NULL) )
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "MIP to SIP fallback - deregister all PPP events" );
          (void) ppp_event_deregister( instance_ptr->ppp_event_handle, 
                                       PPP_PROTOCOL_LCP, 
                                       (ppp_event_mask_e_type) ( PPP_DOWN_EV_MASK | 
                                                                 PPP_RESYNC_EV_MASK) );

          (void) ppp_event_deregister( instance_ptr->ppp_event_handle, 
                                       PPP_PROTOCOL_IPCP, 
                                       (ppp_event_mask_e_type) ( PPP_UP_EV_MASK | 
                                                                 PPP_RESYNC_EV_MASK | 
                                                                 PPP_DOWN_EV_MASK) );

          (void) ppp_event_deregister( instance_ptr->ppp_event_handle,
                                       PPP_PROTOCOL_AUTH,
                                       PPP_FAILURE_EV_MASK);

          (void) ppp_event_deregister( instance_ptr->ppp_event_handle,
                                       PPP_PROTOCOL_AUTH,
                                       PPP_STARTING_EV_MASK );

          if ( ppp_event_dealloc( PPP_UM_SN_DEV,
                                  &(instance_ptr->ppp_event_handle ) ) == -1 )
          {
            DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                             "Unable to dealloc PPP event handle" );
          }
          instance_ptr->ppp_event_handle = NULL;
        }

        if( instance_ptr->ppp_event_handle == NULL )
        {
          ppp_event_alloc_info.ncp = PPP_NCP_VSNCP_3GPP2;
          ppp_event_alloc_info.ncp_cb_f = dssnet4i_handle_ppp_ev_cback;
          ppp_event_alloc_info.ncp_user_data = instance_ptr;
          ppp_event_alloc_info.ncp_handle_ptr = 
            ds707_pdncntx_get_ncp_handle(pdncntx_cb_ptr);

          ppp_event_alloc_info.auth_cb_f = dssnet4i_handle_ppp_ev_cback;
          ppp_event_alloc_info.auth_user_data = instance_ptr;

          instance_ptr->ppp_event_handle = ppp_event_alloc_ex( PPP_UM_SN_DEV,
                                                 &ppp_event_alloc_info );
          if( NULL == instance_ptr->ppp_event_handle )
          {
            ERR_FATAL( "Unable to acquire a PPP event handle", 0, 0, 0 );
          }
 
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "Register for VSNCP events" );
          (void) ppp_event_register( instance_ptr->ppp_event_handle,
                                     PPP_PROTOCOL_VSNCP,
                                     (ppp_event_mask_e_type)( 
                                            PPP_UP_EV_MASK | 
                                            PPP_RESYNC_EV_MASK | 
                                            PPP_DOWN_EV_MASK 
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
                                            | PPP_SUBS_LIMITED_TO_V4_EV_MASK 
                                            | PPP_SUBS_LIMITED_TO_V6_EV_MASK 
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
                                            ) );

          /*------------------------------------------------------------------- 
            Register for AUTH failure event so that dssnet4 can store the call 
            end reason
          -------------------------------------------------------------------*/ 
          (void) ppp_event_register(instance_ptr->ppp_event_handle,
                                    PPP_PROTOCOL_AUTH,
                                    PPP_FAILURE_EV_MASK);

        }
        /*--------------------------------------------------------------------- 
          Start NCP only if it is the first DSSNET that is coming up
        ---------------------------------------------------------------------*/ 
        if ( !( ds707_pdncntx_get_ncp_started( pdncntx_cb_ptr ) ) )        
        {
          if ( ppp_ncp_start( ds707_pdncntx_get_ncp_handle(pdncntx_cb_ptr) ) < 0 )
          {
            DATA_IS707_MSG0(MSG_LEGACY_ERROR, "call to ppp_start failed!");
            (void)dssnet4_sm_post_event( instance_ptr,
                                         DSSNET4_LINK_DOWN_EV,NULL);
            return;
          }
          /*-------------------------------------------------------------------
            Call process VSNCP started, here the ncp_started flag is set.
          -------------------------------------------------------------------*/
          ds707_pdncntx_process_vsncp_started( pdncntx_cb_ptr );

          /*-------------------------------------------------------------------
            Set the call bringup mode flag
          -------------------------------------------------------------------*/
          ds707_pdncntx_set_call_bringup_mode(pdncntx_cb_ptr);
          
        }
      } /* if ehrpd */
      else
#endif /*FEATURE_EHRPD */
      {
        /*---------------------------------------------------------------------
          When system changes from 1x<->DO, and DS hasnt received IDM change
          from CM, if a call is attempted on non-EPC, we should not allow the
          call till the EPC PPP is torn down. Hence set system to non-EPC
          only if there is no other call on EPC.
        ---------------------------------------------------------------------*/
        if (ds707_sys_chg_hdlr_get_current_system() 
                                                  == DS707_SYS_INFO_EPC_SYSTEM)
        {
          DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
               "NON-EPC call requested when system is EPC, posting link_down");
          (void)dssnet4_sm_post_event( instance_ptr, DSSNET4_LINK_DOWN_EV,NULL);
          return;
        }
        instance_ptr->is_ehrpd_call = FALSE;
        /*------------------------------------------------------------------- 
          Allocate memory for ppp_config
        -------------------------------------------------------------------*/
        ppp_config = (ppp_dev_opts_type*)
                     modem_mem_alloc(sizeof(ppp_dev_opts_type),
                                     MODEM_MEM_CLIENT_DATA_CRIT);
	if(ppp_config == NULL)
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "dssnet4i_transition_state():"
                          "memory allocation failed for ppp_config");
          dssnet4i_transition_state( instance_ptr, DSSNET4I_CLOSED_STATE);
          break;    
        }
        memset((void *)ppp_config, 0, sizeof(ppp_dev_opts_type));

        ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NON_EPC_SYSTEM);
        dssnet4i_configure_ppp( instance_ptr, 
                                data_sess_profile_info_ptr, 
                                mip_call, 
                                ppp_config );
           
        /*-------------------------------------------------------------------
          Setup IPCP configuration
        -------------------------------------------------------------------*/
        /*lint -save -e534 return value not required*/
        (void)dssnet_ppp_setup_ncp( ppp_config,
                                    instance_ptr->ps_iface_ptr,
                                    mip_call,
                                    instance_ptr->disable_ipcp_dns_opt,
                                    data_sess_profile_info_ptr );
        /*lint -restore*/
        /*-------------------------------------------------------------------
          Setup LCP configuration
        -------------------------------------------------------------------*/
        dssnet_ppp_setup_lcp( ppp_config,
                              data_sess_profile_info_ptr );

        /*-------------------------------------------------------------------
          Register for PPP UP/DOWN/RESYNC Events.
        -------------------------------------------------------------------*/      
        if( instance_ptr->ppp_event_handle == NULL )
        {
          instance_ptr->ppp_event_handle = 
                            ppp_event_alloc( PPP_UM_SN_DEV,
                                             dssnet4i_handle_ppp_ev_cback,
                                             instance_ptr,
                                             dssnet4i_handle_ppp_auth_ev_cback,
                                             instance_ptr,
                                             dssnet4i_handle_ppp_ev_cback,
                                             instance_ptr,
                                             NULL,
                                             NULL );
          if( NULL == instance_ptr->ppp_event_handle )
          {
            ERR_FATAL( "Unable to acquire a PPP event handle", 0, 0, 0 );
          }
 
          (void) ppp_event_register( 
                           instance_ptr->ppp_event_handle,
                           PPP_PROTOCOL_IPCP,
                           (ppp_event_mask_e_type) (PPP_UP_EV_MASK |
                                                    PPP_RESYNC_EV_MASK |
                                                    PPP_DOWN_EV_MASK) );
          (void) ppp_event_register( 
                           instance_ptr->ppp_event_handle,
                           PPP_PROTOCOL_LCP,
                           (ppp_event_mask_e_type)( PPP_DOWN_EV_MASK |
                                                    PPP_RESYNC_EV_MASK) );

          (void) ppp_event_register( 
                           instance_ptr->ppp_event_handle,
                           PPP_PROTOCOL_AUTH,
                           (ppp_event_mask_e_type)( PPP_FAILURE_EV_MASK |
                                                    PPP_STARTING_EV_MASK) );
        }

        if( ppp_start( PPP_UM_SN_DEV, 
                       ppp_config) < 0)
        {
          /*-------------------------------------------------------------------
            PPP configuration failed.  Post the appropriate event to cleanup
            call since PPP will not give any other indications. We don't want
            to fallback to SIP in this case, set the will_sip to false.
          -------------------------------------------------------------------*/
          #ifdef FEATURE_DS_MOBILE_IP
          instance_ptr->will_sip = FALSE;
          #endif /* FEATURE_DS_MOBILE_IP */

          DATA_IS707_MSG0( MSG_LEGACY_ERROR, "call to ppp_start failed!");
          /*lint -save -e534 return value not required*/
          (void)dssnet4_sm_post_event( instance_ptr,
                                       DSSNET4_LINK_DOWN_EV,NULL);
          /*lint -restore*/
        }

        #ifdef FEATURE_DS_MOBILE_IP
        else if( instance_ptr->state == DSSNET4I_LINK_OPENING_WITH_MIP_STATE)
        {
          mip_meta_sm_post_event( MSM_PHYS_IFACE_UP_EV, 
                                  instance_ptr->ps_iface_ptr );
          instance_ptr->mip_call_started = TRUE;
        }
        #endif
        modem_mem_free(ppp_config, MODEM_MEM_CLIENT_DATA_CRIT);
      } /* if not ehrpd */
      break;

#ifdef FEATURE_DS_MOBILE_IP
    case DSSNET4I_WAITING_FOR_MIP_REG_STATE:
      /*---------------------------------------------------------------------
        Set iface to routeable.  This would prevent non-system sockets from
        being able to use the interface till MIP registration is complete.
        MIP will be able to use the interface to register as it uses system
        sockets.
      ---------------------------------------------------------------------*/
      ps_iface_configuring_ind(instance_ptr->ps_iface_ptr);

      /*---------------------------------------------------------------------
        Enable flow to apps - needed to allow MIP to do ICMP writes.
        Hopefully, iface in configuring state will prevent user-level 
        applications from writing.  Check this with Anup when he gets back.
      ---------------------------------------------------------------------*/
      ps_iface_enable_flow( instance_ptr->ps_iface_ptr,
                            DS_FLOW_CTRL_DSSNET4_MASK);
    
      /*---------------------------------------------------------------------
        Notify MIP meta_sm that iface is up with MIP
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Iface up, Waiting on MIP registration");
      mip_meta_sm_post_event( MSM_CFG_IFACE_EV, 
                              instance_ptr->ps_iface_ptr );

      /* There is a possibility that we transition from closed state to
         waiting for mip reg state directly */
      if (!instance_ptr->mip_call_started)
      {
        instance_ptr->mip_call_started = TRUE;
      }

      break;
#endif /* FEATURE_DS_MOBILE_IP */
    
    case DSSNET4I_NET_UP_STATE:
      /*---------------------------------------------------------------------
        Since state is up now, set DNS addresses to default if they were not
        negotiated via PPP.
      ---------------------------------------------------------------------*/
      if( ( instance_ptr->ps_iface_ptr->v4_net_info.primary_dns.ps_s_addr == 0 ) &&
          ( instance_ptr->ps_iface_ptr->v4_net_info.secondary_dns.ps_s_addr == 0 ) )
      {
#if 0
        instance_ptr->ps_iface_ptr->v4_net_info.primary_dns.ps_s_addr =
                                                       ps_htonl(qcdns_val[0]);
        instance_ptr->ps_iface_ptr->v4_net_info.secondary_dns.ps_s_addr =
                                                       ps_htonl(qcdns_val[1]);
#endif

        pri_dns_addr.type = IPV4_ADDR;
        sec_dns_addr.type = IPV4_ADDR;
        
        pri_dns_addr.addr.v4.ps_s_addr = ps_htonl(qcdns_val[0]);
        sec_dns_addr.addr.v4.ps_s_addr = ps_htonl(qcdns_val[1]);
        
        ps_iface_set_v4_dns_addrs( instance_ptr->ps_iface_ptr,
                                     &pri_dns_addr, &sec_dns_addr);
        
        IPV4_ADDR_MSG(instance_ptr->ps_iface_ptr->v4_net_info.primary_dns.ps_s_addr);
        IPV4_ADDR_MSG(instance_ptr->ps_iface_ptr->v4_net_info.secondary_dns.ps_s_addr);
      }
      
      if (instance_ptr->is_ehrpd_call)
      {
#ifdef FEATURE_NIQ_EHRPD
      /*---------------------------------------------------------------------
        Handle S-BCM for n/w init QOS
      ---------------------------------------------------------------------*/
        dsgen_iface_handle_niq_sbcm(instance_ptr->ps_iface_ptr);
#endif /* FEATURE_NIQ_EHRPD */

#ifdef FEATURE_EHRPD
        /*---------------------------------------------------------------------
          Check default APN indication in VSNCP
        ---------------------------------------------------------------------*/
        dsgen_iface_handle_vsncp_default_apn_ind(instance_ptr->ps_iface_ptr);
#endif /* FEATURE_EHRPD */
      }
      /*---------------------------------------------------------------------
        Enable flow to apps - needed to allow MIP to do ICMP writes.
        Hopefully, iface in configuring state will prevent user-level 
        applications from writing.  Check this with Anup when he gets back.
      ---------------------------------------------------------------------*/
      ps_iface_enable_flow( instance_ptr->ps_iface_ptr,
                            DS_FLOW_CTRL_DSSNET4_MASK);
      /*---------------------------------------------------------------------
        Check if ready for 1x tuneaway and update MMODE
      ---------------------------------------------------------------------*/
      pdncntx_cb_ptr = 
        ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(instance_ptr->ps_iface_ptr);
      iface6_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);
      if((ps_iface_state(iface6_ptr)  != IFACE_COMING_UP) &&
         (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_CDMA)) 
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                        "Posting MC_TUNE_AWAY_CNTL_F - Ready for TuneAway");
        queue_status = mc_queue_cmd(MC_TUNE_AWAY_CNTL_F); 
        if (queue_status == MC_FREE_QUEUE_EMPTY) 
        {
          ERR_FATAL( "No MC command links available", 0, 0, 0);
        }
        else if (queue_status != MC_CMD_QUEUED) 
        {
          ERR_FATAL( "Unknown status return from mc_queue_cmd()", 0, 0, 0);
        }
      }
      /*---------------------------------------------------------------------
        Send iface up indication.
      ---------------------------------------------------------------------*/
      ps_iface_up_ind( instance_ptr->ps_iface_ptr);


      /*---------------------------------------------------------------------
        If this is non-DDS, disable the phys link if there is COEX issue 
      ---------------------------------------------------------------------*/
      ds707_pkt_mgr_check_coex_data_status( instance_ptr->ps_iface_ptr);
      
#if 0  
      /*------------------------------------------------------------------
        Starting DHCP
      ------------------------------------------------------------------*/
      dshdr_dhcp_start(instance_ptr->ps_iface_ptr);
#endif /* #if 0 */
      /*---------------------------------------------------------------------
        Invoke the ppp_resync_done_cb to notify the completion of the PPP
        resync (either successfully or failed resync). Also unregister the
        call back right after invoking it.
      ---------------------------------------------------------------------*/
      if (instance_ptr->ppp_resync_done_cb != NULL)
      {
        instance_ptr->ppp_resync_done_cb(instance_ptr);
        instance_ptr->ppp_resync_done_cb = NULL;
      }

      /* If NCP resync was in progress, reset ncp_resync_started flag */
      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
      if ( DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) &&
           ds707_pdncntx_get_ncp_resync_started(pdncntx_cb_ptr) == TRUE
         )
      {
          ds707_pdncntx_set_ncp_resync_started(pdncntx_cb_ptr, FALSE);
      }

      dshdr_dhcp_trigger_negotiation(instance_ptr->ps_iface_ptr);      

#if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MOBILE_IP_PERF)
      if ( instance_ptr->dssnet_started_mip_call )
      {
        instance_ptr->dssnet_started_mip_call = FALSE;

        /*-----------------------------------------------------------------
          M.IP call setup is complete for socket calls only.
        -----------------------------------------------------------------*/
        mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_CALL_SETUP_DELAY);
        mip_perf_delay( mip_perf_info_ptr,
                        PERF_TOTAL_CALL_SETUP_DELAY,
                        PERF_END_TS );
      }
#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DS_MOBILE_IP_PERF */
      break;

    case DSSNET4I_LINK_RESYNC_STATE:

      /*---------------------------------------------------------------------
        Disable flow to apps
      ---------------------------------------------------------------------*/
      ps_iface_disable_flow( instance_ptr->ps_iface_ptr, 
                             DS_FLOW_CTRL_DSSNET4_MASK);
      ps_iface_configuring_ind(instance_ptr->ps_iface_ptr);

#ifdef FEATURE_EHRPD
      if(instance_ptr->is_ehrpd_call == TRUE)
      {
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                      instance_ptr->ps_iface_ptr);
        if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
        {
          DATA_ERR_FATAL("Cannot get PDN Instance to set call bringup mode");
          return; 
        }
        /*-------------------------------------------------------------------
            Set the call bringup mode flag again since PPP resync happened
        -------------------------------------------------------------------*/
        ds707_pdncntx_set_call_bringup_mode(pdncntx_cb_ptr);

        /*-------------------------------------------------------------------
            Set NCP resync started flag to TRUE
        -------------------------------------------------------------------*/
        /* Companion iface will also go into resync and might have already
           set this flag */
        if (ds707_pdncntx_get_ncp_resync_started(pdncntx_cb_ptr) == FALSE)
        {
          ds707_pdncntx_set_ncp_resync_started(pdncntx_cb_ptr, TRUE);
        }
      }
#endif /*FEATURE_EHRPD */
      break;

#ifdef FEATURE_DS_MOBILE_IP
#ifdef FEATURE_DS_MOBILE_IP_DEREG
   case DSSNET4I_WAITING_FOR_MIP_DEREG_STATE:
      /* send bring down mip event to meta sm*/
      mip_meta_sm_post_event( MSM_BRING_DOWN_MIP_EV, 
                              instance_ptr->ps_iface_ptr );

      break;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
    case DSSNET4I_LINK_CLOSING_STATE:
      /*---------------------------------------------------------------------
        Enable flow to apps - needed to allow MIP to do ICMP writes.
        Hopefully, iface in configuring state will prevent user-level 
        applications from writing.  Check this with Anup when he gets back.
      ---------------------------------------------------------------------*/
      ps_iface_enable_flow( instance_ptr->ps_iface_ptr,
                            DS_FLOW_CTRL_DSSNET4_MASK);

      /*---------------------------------------------------------------------
        Close PPP.
      ---------------------------------------------------------------------*/
      if(instance_ptr->ps_iface_ptr != NULL)
      {
      DATA_IS707_MSG3( MSG_LEGACY_MED, 
                       "dssnet4 0x%x Closing PPP on Iface 0x%x:%x" ,
                       instance_ptr,
                       instance_ptr->ps_iface_ptr->name,
                       instance_ptr->ps_iface_ptr->instance);
      }

      pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(
                                                  instance_ptr->ps_iface_ptr);
      if ( !DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr) )
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                         "Cannot get PDN Instance, do not proceed");
        return; 
      }

#ifdef FEATURE_EHRPD
      if(instance_ptr->is_ehrpd_call == TRUE)
      {
        if( ds707_pdncntx_get_ncp_started( pdncntx_cb_ptr ) )
        {
          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "NCP still up, terminate NCP");
          if ((ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_EPC_SYSTEM)
              && (ds707_pkt_is_ehrpd_mode_of_operation() == TRUE))
          {
            DATA_IS707_MSG0(MSG_LEGACY_MED, 
                            "Current system EPC, graceful teardown");

            /*------------------------------------------------------------- 
              Terminate NCP based on graseful termination flag.
            -------------------------------------------------------------*/
            if (
                  (instance_ptr->local_abort != TRUE) 
                  && 
                  (
                    (PHYS_LINK_UP == 
                         ps_iface_phys_link_state(instance_ptr->ps_iface_ptr) )
                    ||
                    (TRUE == 
                    PS_IFACE_GET_GRACEFUL_CLOSE( instance_ptr->ps_iface_ptr) )
                  )  
                )  
            {
              if (ppp_ncp_stop(
                      ds707_pdncntx_get_ncp_handle( pdncntx_cb_ptr )) < 0)
              {
                /*-----------------------------------------------------------
                  Since ppp_ncp_stop failed, we wont get link down from PPP.
                  Post link_down to ourselves to cleanup the DSSNET
                -----------------------------------------------------------*/ 
                DATA_IS707_MSG0(MSG_LEGACY_ERROR, "call to ncp_stop failed! "
                                "Post link_down to cleanup dssnet4");
                (void)dssnet4_sm_post_event(instance_ptr,
                                            DSSNET4_LINK_DOWN_EV, NULL);
                /*----------------------------------------------------------- 
                 Below we set ncp_started to false. So we need to return
                 here itself. Reason: In a dual IP scenario if for some
                 reason the PPP device is already cleaned up and if we attempt
                 to close both dssnet4 & 6, the first dssnet that processes 
                 ppp_close_cmd comes here and attempts to stop/abort ncp 
                 and gets a failure. If it doesn't return here and sets the 
                 ncp_started to false, then the next dssnet will eternally 
                 wait to get link_down from PPP. Return here so the other 
                 dssnet will also attempt stop/abort ncp and cleanup by itself
                 Same logic applies to below calls to ncp abort
                -----------------------------------------------------------*/
                return;
              }
            }
            else
            {
              if (ppp_ncp_abort(
                      ds707_pdncntx_get_ncp_handle( pdncntx_cb_ptr )) < 0)
              {
                DATA_IS707_MSG0(MSG_LEGACY_ERROR, "call to ncp_abort failed!"
                                " Post link_down to cleanup dssnet4");
                (void)dssnet4_sm_post_event( instance_ptr,
                                       DSSNET4_LINK_DOWN_EV,NULL);
                return;
              }
            }
          } /*if EPC system*/
          else /* NON-EPC or OOS */
          {
            /*------------------------------------------------------------- 
              Check if phone is in OOS. If it is & v6 iface is in use, tear
              down v4 only
            -------------------------------------------------------------*/
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
            if (FALSE == ds707_get_has_phone_acquired_signal())
            {
              pdn_v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);

              if( (! PS_IFACEI_IS_IN_ARBITRATION(instance_ptr->ps_iface_ptr))&&
                  (PS_IFACE_IS_VALID(pdn_v6_iface_ptr)) &&
                  (PS_IFACE_IS_IN_USE(pdn_v6_iface_ptr)) )
              {
                DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                                "V6 iface in use, tear down only v4");
                (void) ps_iface_go_null_cmd( instance_ptr->ps_iface_ptr,
                                             &ps_errno, 
                                             NULL );
                dssnet4i_transition_state(instance_ptr, 
                                          DSSNET4I_CLOSED_STATE);
                break;
              }
            }
#endif/*FEATURE_DATA_EHRPD_DUAL_IP*/
            
            /*------------------------------------------------------------- 
              Since system is not eHRPD anymore, call ncp_abort. Don't
              call ppp_abort / ppp_device_reset since this will bring
              down entire PPP i.e. other NCPs as well. Tearing down other
              NCPs is a problem in case of eHRPD to LTE HO, since one PDN
              is transferred independent of other PDNs. So when one PDN
              is successfully handedover and we want to abort it on eHRPD
              side, other PDNs may still be in the process of HO. That
              time we dont want to abort PPP (LCP). Instead it is better
              to abort only this NCP. PPP module will take care of aborting
              LCP when the last NCP is aborted by MH. 
            -------------------------------------------------------------*/
            DATA_IS707_MSG0( MSG_LEGACY_MED, 
                             "Current system NON-EPC, silent teardown");
            if (ppp_ncp_abort(
                    ds707_pdncntx_get_ncp_handle( pdncntx_cb_ptr )) < 0)
            {
                DATA_IS707_MSG0(MSG_LEGACY_ERROR, "call to ncp_abort failed!"
                                " Post link_down to cleanup dssnet4");
                (void)dssnet4_sm_post_event( instance_ptr,
                                       DSSNET4_LINK_DOWN_EV,NULL);
                return;
            }
          }

          ds707_pdncntx_set_ncp_started(pdncntx_cb_ptr, FALSE);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "NCP already down");
        }
      }
      else
#endif /*FEATURE_EHRPD */
      {
        phys_link_ptr =  PS_IFACE_GET_PHYS_LINK(instance_ptr->ps_iface_ptr);
        if(phys_link_ptr != NULL )
        {
          phys_flow_mask = ps_phys_link_get_flow_mask(phys_link_ptr);
        }
        /*lint -save -e534 return value not required*/
        if ((ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_NON_EPC_SYSTEM)
             && ((DS_FLOW_CTRL_IS_BIT_SET(phys_flow_mask,DS_FLOW_NO_SERVICE_MASK)) != TRUE ))
        {
           /*if local abort flag is set to TRUE, perform local abort*/ 
          if (instance_ptr->local_abort == TRUE) 
          {
             DATA_IS707_MSG0(MSG_LEGACY_MED, "Local abort flag is set");
            (void)ppp_abort(PPP_UM_SN_DEV);
          }
          else
          {
            DATA_IS707_MSG0(MSG_LEGACY_MED, "Current system NON-EPC & "
                            "flow mask not set to no service, graceful teardown");

            (void)ppp_stop( PPP_UM_SN_DEV, PPP_PROTOCOL_IPCP);
          }
        }
        else
        {
          /*------------------------------------------------------------- 
            Check if phone is in OOS. If it is & v6 iface is in use, tear
            down v4 only
          -------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
          if (FALSE == ds707_get_has_phone_acquired_signal())
          {
            pdn_v6_iface_ptr = ds707_pdncntx_get_v6_iface(pdncntx_cb_ptr);

            if( (! PS_IFACEI_IS_IN_ARBITRATION(instance_ptr->ps_iface_ptr))&&
                (PS_IFACE_IS_VALID(pdn_v6_iface_ptr)) &&
                (PS_IFACE_IS_IN_USE(pdn_v6_iface_ptr)) )
            {
              DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                              "V6 iface in use, tear down only v4");
              (void)ppp_stop( PPP_UM_SN_DEV, PPP_PROTOCOL_IPCP);
              break;
            }
          }
#endif /* FEATURE_DATA_PS_IPV6 */

          DATA_IS707_MSG0(MSG_LEGACY_MED, "Current system EPC or "
                          "flow mask set to no service, silent teardown");
          (void)ppp_abort(PPP_UM_SN_DEV);
        }
        /*lint -restore*/
      }  
      break;

    default:
      /*---------------------------------------------------------------------
        Sanity Check
      ---------------------------------------------------------------------*/
      ASSERT(0);
      return;
  } /* switch(state) */
  /*lint -restore error 825, fall thru intentional */
} /* dssnet4i_transition_state() */

/*===========================================================================
FUNCTION DSSNET4_SET_IPCP_DNS_OPT()

DESCRIPTION
  This function enables/disables the dns negotiation option for ipcp.

DEPENDENCIES
  None.

PARAMETERS
  dssnet4 instance handle - dssnet instance corresponding to the  ppp instance
                            for which this dns option holds good
  boolean     dns_option  - value of DNS option for IPCP

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_set_ipcp_dns_opt
(
  dssnet4_sm_cb_type *instance_ptr,           
  boolean dns_option
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( instance_ptr != NULL );
  instance_ptr->disable_ipcp_dns_opt = dns_option;

} /* dssnet4_set_ipcp_dns_opt() */

#if defined(FEATURE_DS_MOBILE_IP) && defined(FEATURE_DS_MOBILE_IP_PERF)

/*===========================================================================
FUNCTION DSSNET4_START_MIP_PERF_STATS()

DESCRIPTION
  This function starts collecting call-setup stats for MIP calls

DEPENDENCIES
  None.

PARAMETERS
  dssnet4 instance handle - dssnet instance corresponding to the ppp instance
                            for which the stats are being collected 

RETURN VALUE
  None 

SIDE EFFECTS
  None.
===========================================================================*/

void dssnet4_start_mip_perf_stats
(
  dssnet4_sm_cb_type *instance_ptr           
)
{
  qword ts_null;
  mip_perf_ts_info_type *mip_perf_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( instance_ptr != NULL );

  /*------------------------------------------------------------------------ 
    Initialize ts_null
  ------------------------------------------------------------------------*/
  qw_set(ts_null, 0, 0);

  mip_perf_info_ptr = mip_get_perf_info_ptr(PERF_TOTAL_CALL_SETUP_DELAY);
  if ( qw_cmp( mip_perf_info_ptr->ts_start, 
               ts_null ) == 0)
  {
    /*---------------------------------------------------------------
      Start M.IP call setup if it hasn't already been started by
      RMSM (i.e. this is a sockets call)
    ---------------------------------------------------------------*/
    mip_perf_delay( mip_perf_info_ptr,
                    PERF_TOTAL_CALL_SETUP_DELAY,
                    PERF_START_TS );
    instance_ptr->dssnet_started_mip_call = TRUE;
  }
}

#endif /* FEATURE_DS_MOBILE_IP && FEATURE_DS_MOBILE_IP_PERF */


/*===========================================================================
FUNCTION DSSNET4I_HANDLE_PPP_AUTH_EV_CBACK()

DESCRIPTION
  Called when the PPP comes up/down/resync.  This posts an appropriate event
  to the dssnet4 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dssnet4i_handle_ppp_auth_ev_cback
(
  ppp_dev_enum_type   ppp_dev,                   /* PPP Device             */
  ppp_protocol_e_type protocol,                  /* PPP Protocol           */
  ppp_event_e_type    ppp_event,                 /* SUCCESS/FAIL         */
  void                *user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  ppp_fail_reason
)
{
  dssnet4_sm_cb_type *instance_ptr;           /* dssnet4 instance ptr      */
#ifdef FEATURE_CALL_THROTTLE
  ds707_pdn_context_cb_type* pdncntx_cb_ptr = NULL;
#ifdef FEATURE_DS_MOBILE_IP
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
#endif /*FEATURE_DS_MOBILE_IP*/ 
#endif /*FEATURE_CALL_THROTTLE*/
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == user_data) 
  {
    ASSERT(0);
    return;
  }

  DATA_IS707_MSG4(MSG_LEGACY_MED, 
                  "Recd ppp dev %d event %d for dssnet4 0x%x. Fail reason %d",
                  ppp_dev, ppp_event, user_data, ppp_fail_reason);

  instance_ptr = (dssnet4_sm_cb_type*)(user_data);
  ASSERT(ppp_dev == PPP_UM_SN_DEV);

#ifdef FEATURE_CALL_THROTTLE
#ifdef FEATURE_DS_MOBILE_IP
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
#endif /*FEATURE_DS_MOBILE_IP*/ 
#endif /*FEATURE_CALL_THROTTLE*/

  switch(ppp_event)
  {
    case PPP_FAILURE_EV:
      /*-------------------------------------------------------------------
        Transition to CLOSING state.
      -------------------------------------------------------------------*/
#ifdef FEATURE_CALL_THROTTLE
#ifdef FEATURE_DS_MOBILE_IP
      if( (instance_ptr->is_te2_client == FALSE) || 
          ( (instance_ptr->is_te2_client == TRUE) &&  
            (ds_1x_runtime_profile_ptr->mip.ds_qcmip_val == 2) ) )
#endif /*Feature_DS_MOBILE_IP */
      {
        pdncntx_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr( instance_ptr->ps_iface_ptr );
        if ( ds707_pdncntx_get_dctm_ind_posted(pdncntx_cb_ptr) == FALSE )
        {    
          if( instance_ptr->is_ehrpd_call == FALSE )
          {
            dctm_post_ppp_call_status( PPP_FAILURE_EV, 
                                       DSDCTM_PPP_PROTOCOL_AUTH, 
                                       PPP_FAIL_REASON_AUTH_FAILURE);
            ds707_pdncntx_set_dctm_ind_posted(pdncntx_cb_ptr, TRUE);
          }
        }
      }
#endif /* FEATURE_CALL_THROTTLE */

      dssnet4i_set_ppp_failure_code(instance_ptr, ppp_fail_reason);

      (void)dssnet4_sm_post_event( instance_ptr,
                                   DSSNET4_PPP_CLOSE_EV, NULL);
      break;

    case PPP_SUCCESS_EV:
      break;

    case PPP_STARTING_EV:
      (void)dssnet4_sm_post_event( instance_ptr,
                                    DSSNET4_PPP_AUTH_STARTED_EV,NULL);
      break;

    default:
      /* Ignore */
      break;
  }
} /* dssnet4i_handle_ppp_auth_ev_cback() */


#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DSSNET4I_STORE_ISN()

DESCRIPTION   Utility function to write the EFS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void dssnet4i_store_isn
(
 uint64 *aka_algo_seqnum_arr,
 uint8 aka_algo_seqnum_arr_num_elements
)
{
  int fd; 
  uint8 i;
  char *temp_64_ptr = NULL;
  uint32 seqnum_arr_total_size;
  char seperator = ';';
  uint32 cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "EFS write initial sequence number");
  if(aka_algo_seqnum_arr_num_elements != 32)
  {
    DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                     "Invalid size %d", 
                     aka_algo_seqnum_arr_num_elements);
    return;
  }

  (void)mcfg_fs_truncate(EHRPD_EFS_FILE_NAME, 
                    0,
                    MCFG_FS_TYPE_EFS,
                    (mcfg_fs_sub_id_e_type) ds707_curr_subs_id());
  fd = mcfg_fopen( EHRPD_EFS_FILE_NAME,
                   MCFG_FS_O_WRONLY,
                   MCFG_FS_DEFFILEMODE,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type) ds707_curr_subs_id());
  if(fd != -1) 
  {
    seqnum_arr_total_size = aka_algo_seqnum_arr_num_elements 
                            * AKA_ALGO_SEQ_NO_ITEM_SIZE * sizeof(*temp_64_ptr);
    temp_64_ptr = (char*)ds_system_heap_mem_alloc(seqnum_arr_total_size);
    if(NULL == temp_64_ptr)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Failed to allocate memory");
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS);
      return;
    }

    /*-------------------------------------------------------------------
      Copy the sequence no. into the allocated buffer & then write it
    -------------------------------------------------------------------*/
    memset(temp_64_ptr, 0, seqnum_arr_total_size);
    for(i = 0; i< aka_algo_seqnum_arr_num_elements; i++)
    {
      (void)snprintf ((char*)&temp_64_ptr[cnt], AKA_ALGO_SEQ_NO_ITEM_SIZE,  
                            "%016llx",aka_algo_seqnum_arr[i]);
      cnt += AKA_ALGO_SEQ_NO_ITEM_SIZE;
      temp_64_ptr[cnt-1] = seperator;
    }

    (void) mcfg_fwrite(fd, (char *)temp_64_ptr, cnt,MCFG_FS_TYPE_EFS);
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS);

    ds_system_heap_mem_free(temp_64_ptr);
  }
  else 
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Cannot open file.");
    return;
  }
  return;
}

/*===========================================================================
FUNCTION      DSSNET4I_HANDLE_EAP_AUTH_EV_CBACK()

DESCRIPTION   This is callback funcation registerd with PPP module. When the
              EAP succeeds, this callback is called 

              When the funcation is called, the passed cookie is checked for
              caller's integrity.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void dssnet4i_handle_eap_auth_ev_cback
(
 uint8 *master_session_key,
 uint16 master_session_key_len,
 ps_ppp_eap_result_enum_type ps_eap_result,
 void *eap_user_data,
 uint64 *aka_algo_seqnum_arr,
 uint8 aka_algo_seqnum_arr_num_elements
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( *((uint8 *)eap_user_data) != ds707_eap_user_data)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid cookie 0x%x", 
                    (*(uint8 *)eap_user_data));
    return;
  }
  if(ps_eap_result == PS_PPP_EAP_RESULT_FAILURE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "EAP result failed");
    return;
  }

#if 0
  /*---------------------------------------------------------------------- 
    These values are currently not being used in any module. Commenting
    the code.
  -----------------------------------------------------------------------*/
  /*----------------------------------------------------------------------
   Store the EAP result atomically as these values might be used by other
   clients and in different task context.
  -----------------------------------------------------------------------*/
  ds707_master_session_key = *master_session_key;
  ds707_master_session_key_len = master_session_key_len;

  /*----------------------------------------------------------------------
   Not sure if we should print these parameter. Verify with other team
   members and remove the comment. If not needed, remove this F3 message
  -----------------------------------------------------------------------*/
  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "EAP: Master session key %x, key len = %d", 
                   ds707_master_session_key, 
                   ds707_master_session_key_len);
#endif   

  /*----------------------------------------------------------------------
  have not found the use case where this master session key is 
  being used in Data code. 
  -----------------------------------------------------------------------*/

  /*----------------------------------------------------------------------
  Read the sequence numbers and store it on EFS
  -----------------------------------------------------------------------*/
  dssnet4i_store_isn( aka_algo_seqnum_arr, 
                      aka_algo_seqnum_arr_num_elements);

  return;
}/* dssnet4i_handle_eap_auth_ev_cback ()*/

#if 0
/*===========================================================================
FUNCTION      DSSNET4I_GET_EAP_MASTER_SESSION_KEY()

DESCRIPTION   This is utility function to provide the stored master session
              key information

DEPENDENCIES  The caller should provide the return memory

RETURN VALUE  TRUE  - if successfully copied
              FALSE - otherwise

SIDE EFFECTS  The output paramter is populated with master session key 
              information
===========================================================================*/
boolean dssnet4i_get_eap_master_session_key
(
  uint8 *master_session_key, 
  uint16 *master_session_key_len
)
{
  boolean ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((master_session_key != NULL) && (master_session_key_len != NULL)) 
  {
    *master_session_key = ds707_master_session_key;
    *master_session_key_len = ds707_master_session_key_len;
    ret_val = TRUE;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid parameter passed");
    ret_val = FALSE;
  }
  return ret_val;
}/* dssnet4i_get_eap_master_session_key() */
#endif
#endif /* FEATURE_EHRPD*/

/*===========================================================================
FUNCTION       DSSNET4I_CONFIGURE_PPP

DESCRIPTION    Configure the PPP Parameters
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
static void dssnet4i_configure_ppp
(
  dssnet4_sm_cb_type *dssnet4_instance_ptr,
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr,
  boolean mip_call,
  ppp_dev_opts_type *ppp_config_ptr                   
)  
{
  ds707_data_session_profile_id profile_id = DATA_SESSION_PROFILE_ID_INVALID;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -----------*/

  /*---------------------------------------------------------------------
    Setup PPP configuration 
  ---------------------------------------------------------------------*/
  ppp_get_default_opts(ppp_config_ptr);

#if defined(FEATURE_HDR_RM_QOS)
  ppp_config_ptr->hdlc_mode = HDLC_HW_PREF;
#else
  ppp_config_ptr->hdlc_mode = HDLC_SW;
#endif

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    ppp_config_ptr->setup_timeout = JCDMA_MAX_PPP_SETUP_TIME;
  }

  else
  {
    /*-----------------------------------------------------------------------
      Setting max ppp setup time to 40 sec. This is needed in case there is
      no response from the network during auth phase then this timer will
      termiate the PPP session on the AT side.
    -----------------------------------------------------------------------*/
    ppp_config_ptr->setup_timeout = 40;
  }
  // As of PS12, rx_f_ptr is no longer used, as it is PPP specific.
  // Instead, we always use rx_sig_f_ptr, even for PPP flows.
  ppp_config_ptr->rx_f_ptr       = NULL;
  ppp_config_ptr->rx_sig_f_ptr   = ds707_rx_UM_SN_sig_hdlr;
  ppp_config_ptr->tx_f_ptr       = ds707_pkt_tx_um_SN_data;
  /*---------------------------------------------------------------------
    Use RM as the bridge device. This will never be used if we are in a 
    sockets call, so it is safe to set it here for both MT2 and MT0 calls
  ---------------------------------------------------------------------*/
  ppp_config_ptr->bridge_dev = PPP_RM_DEV;

 /*----------------------------------------------------------------------
   Setup LCP configuration options.
  ---------------------------------------------------------------------*/
  ppp_config_ptr->lcp_info.mode = PPP_CP_MODE_ACTIVE;

  /*----------------------------------------------------------------------- 
    If QNC then disable Auth. If V6 feature is OFF or V6 NV is zero then 
    disable Auth if MIP4 call. 
    In all other cases, enable Auth
  -----------------------------------------------------------------------*/
  if ((dssnet4_instance_ptr->qnc_call == TRUE)
  #if defined(FEATURE_DATA_PS_IPV6)
        || ((!ds707_is_hrpd_ipv6_enabled() ) && (mip_call))
  #else
        || (mip_call)
  #endif

  #ifdef FEATURE_DATA_PKT_CLASSIFIER
        || (ds707_is_pkt_classifier_enabled())
  #endif/* FEATURE_DATA_PKT_CLASSIFIER */
     )

  {
    /*-------------------------------------------------------------------
      Disable PPP auth.
    -------------------------------------------------------------------*/
    ppp_config_ptr->lcp_info.will_mask[FSM_REMOTE] &= ~LCP_N_AP;
    ppp_config_ptr->lcp_info.want_mask[FSM_REMOTE] &= ~LCP_N_AP;
    ppp_config_ptr->lcp_info.will_mask[FSM_LOCAL] &= ~LCP_N_AP;
    ppp_config_ptr->lcp_info.want_mask[FSM_LOCAL] &= ~LCP_N_AP;
  }
  else
  {
    profile_id = data_sess_profile_info_ptr->data_session_profile_id;

    if ((profile_id != DATA_SESSION_DEFAULT_PROFILE) &&
       ((data_sess_profile_info_ptr->profile_type & PROFILE_EHRPD) == FALSE))
    {
      if( ds707_data_session_is_nvruim_profile(profile_id) )
      {
        /*-------------------------------------------------------------------
          Retrieve auth info from NV/RUIM for NVRUIM profiles
        -------------------------------------------------------------------*/
        ds707_data_session_get_ppp_auth_info_for_nvruim_profile(
                               &(ppp_config_ptr->auth_info) );
      }
      else
      {
        /*-------------------------------------------------------------------
          Retrieve auth info from data session profile structure  
        -------------------------------------------------------------------*/
        DATA_IS707_MSG1( MSG_LEGACY_MED, 
                         "Retrieve auth info from data session profile %d",
                         profile_id );
        ds707_assign_ppp_auth_info( &(ppp_config_ptr->auth_info),
                                    data_sess_profile_info_ptr);
      }
    }		
    else
    {
      /*-------------------------------------------------------------------
       For default profile or ehrpd profile, if mode of operation is
       ehrpd then read eap paramters else read from NV.
      -------------------------------------------------------------------*/
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "Retrieving auth info for profile id %d",
                       profile_id );
      is707_get_ppp_auth_info_from_nv( &(ppp_config_ptr->auth_info), 
                                       ds3gcfgmgr_read_legacy_nv_ex );
    }
  }

#ifdef FEATURE_DS_MULTIPLE_NAI
  /*---------------------------------------------------------------------------
    SIP Laptop call: Multiple NAIs are applicable only for NV mode, so check 
    if phone is in NV mode before copying RM NAI.
  ---------------------------------------------------------------------------*/
  if(((uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV) || 
     (uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV)) && 
     (PS_IFACE_IS_CALL_FROM_TETHERED_HOST(dssnet4_instance_ptr->ps_iface_ptr)) &&
     (FALSE == mip_call))
  {        
    DATA_IS707_MSG0(MSG_LEGACY_MED, "This is a laptop call (NV mode), "
                    "NV_DS_SIP_RM_NAI_I may be used if configured");
    ds707_copy_rm_nai(&(ppp_config_ptr->auth_info), ds3gcfgmgr_read_legacy_nv_ex);
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, "NV_PPP_USER_ID_I is used");
  }
#endif /* FEATURE_DS_MULTIPLE_NAI */

  if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    ds707_configure_VJ_compression_for_PPP(ppp_config_ptr);
    ds707_configure_jcdma_params_for_ppp(ppp_config_ptr);
  }
  /*Simplified JCDMA: 
    5.1.26 Enable VJ compression   [D1-No.78]*/
  else if(ds707_roaming_is_simplified_jcdma_mode())
  {
    ds707_configure_VJ_compression_for_PPP(ppp_config_ptr);
  }  

} /* dssnet4i_configure_ppp() */


#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_IFACE4_HDLR_PRASE_DATA

DESCRIPTION   Reads and parses a record from the passed EFS file. The 
              input is considerd to be 64 bit and in hex format

DEPENDENCIES  

RETURN VALUE  


SIDE EFFECTS  
===========================================================================*/
int ds707_iface4_hdlr_prase_data
(
  char *from,
  char *to, 
  uint64 *data_read
)
{
 
  int count =0; /* to make sure that it is 64 bit maximum */
  uint8 value = 0;
  uint64 temp = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *data_read = 0;
  memset(&temp,0,8);

  while(from != to)
  {
    if(count >=16) /* can not exceed 8 bytes (16 nibesls */
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, "Junk value %d. Abort", count);
      return -1;
    }
    
    switch(*from)
    {
      case '0':
        value = 0;
        break;
      case '1':
        value = 1;
        break;
      case '2':
        value = 2;
        break;
      case '3':
        value = 3;
        break;
      case '4':
        value = 4;
        break;
      case '5':
        value = 5;
        break;
      case '6':
        value = 6;
        break;
      case '7':
        value = 7;
        break;
      case '8':
        value = 8;
        break;
      case '9':
        value = 9;
        break;
      case 'A':
      case 'a':
        value = 10;
        break;
      case 'B':
      case 'b':
        value = 11;
        break;
      case 'C':
      case 'c':
        value = 12;
        break;
      case 'D':
      case 'd':
        value = 13;
        break;
      case 'E':
      case 'e':
        value = 14;
        break;
      case 'F':
      case 'f':
        value = 15;
        break;
      default:
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Junk value. Abort");
        return -1;
    }
    temp = temp * 16;
    temp = temp + value;

    count++;   
    from++;
  }
  *data_read = temp;
  return 0; /* success */
}/* ds707_iface4_hdlr_prase_data()*/

/*===========================================================================
FUNCTION      DS707_IFACE4_HDLR_READ_RECORD_FROM_EFS

DESCRIPTION   Reads and parses a record from the passed EFS file.

DEPENDENCIES  file_handle must point to a valid file in the EFS that has 
              been opened for READ access by the calling function.

RETURN VALUE  If successfully able to read the record from EFS and populate
              the database structure, returns 0 and *err_num is set to 
              E_SUCCESS.

              If an error is encountered while reading from the EFS or
              while parsing, returns -1 and *err_num is set to one of

              E_EOF           End of file reached
              E_INVALID_ARG   I/O parameter is invalid
              E_BAD_DATA      Parse error in the EFS text record
              E_OVERFLOW      Parsing exceed the max record size


SIDE EFFECTS  If successfully able to read and parse the text record from
              the EFS file, the file position in the EFS is set to the
              beginning of the next record. If a parse error is encountered,
              the file position is undefined.
===========================================================================*/
LOCAL sint15 ds707_iface4_hdlr_read_record_from_efs
(
  ds3gcfgmgr_efs_token_type  *efs_db_ptr,
  sint15 *err_num,
  ppp_auth_info_type *ppp_auth_info_ptr
)
{
  uint64 data_read; /* 64 bit value stored */
  char *from, *to;
  ds3gcfgmgr_efs_token_parse_status_type  ret_val 
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity checks on input parameters
  -------------------------------------------------------------------------*/
  if(err_num == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "err_num is NULL!");
    return -1;
  }

  if(efs_db_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "database/record is NULL!");
    *err_num = (sint15) E_INVALID_ARG;
    return -1;
  }

  *err_num = (sint15) E_FAILURE;
  ppp_auth_info_ptr->aka_algo_seqnum_arr_num_elements = 0;

  /*-------------------------------------------------------------------------
    Parse the record tokens from the EFS file and assign them to the 
    appropriate record field. If there is a parse error, this will set the
    err_num and return. If the record is parsed successfully, set the database
    entry valid flag to true. The tokens in a record may be separated either
    by '\n' or by ';'
  -------------------------------------------------------------------------*/  
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL 
          != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db_ptr, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and 
      to point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }     
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      if(ds707_iface4_hdlr_prase_data(from, to, &data_read) == -1)
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Error in reading. Abort");
        *err_num = (sint15) E_FAILURE;
        return -1;
      }
      else
      {
        if(ppp_auth_info_ptr->aka_algo_seqnum_arr_num_elements >= EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS)
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                          "Too many elements to parse. Array overflow");
          *err_num = (sint15) E_FAILURE;
          return -1;
        }

        ppp_auth_info_ptr->aka_algo_seqnum_arr[ppp_auth_info_ptr->aka_algo_seqnum_arr_num_elements] = data_read;
        ppp_auth_info_ptr->aka_algo_seqnum_arr_num_elements++;
      }
    }
    else
    {
      ASSERT(DS3GCFGMGR_EFS_TOKEN_PARSE_FAILURE == ret_val);
      *err_num = (sint15) E_FAILURE;
      return -1;
    }
  } /* Big while Parsed the file*/

  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Done Parsing record EFS: Arry size %d",
                   ppp_auth_info_ptr->aka_algo_seqnum_arr_num_elements);
  *err_num = (sint15) E_EOF;
  return -1;
}
/*===========================================================================
FUNCTION       DS707_IFACE4_HDLR_READ_ISN_FROM_EFS

DESCRIPTION    This funcation reads the EFS data base
               
DEPENDENCIES    1. The input parameter of the function
                2. NV read

RETURN VALUE   

SIDE EFFECTS   The PPP configuration parameters are populated with USER ID and 
               SHARED SECRET. 
===========================================================================*/
sint15 ds707_iface4_hdlr_read_isn_from_efs
(
  ppp_auth_info_type *ppp_auth_info_ptr
)
{
  ds3gcfgmgr_efs_token_type  efs_db;
  sint15  err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds3gcfgmgr_efs_file_init(EHRPD_EFS_FILE_NAME, &efs_db) == -1)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Error opening file ISN from EFS");
    return (sint15) E_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Read each record from EFS. 
    If the EFS file is too small, this will result in returning E_SUCCESS
    but only some of the entries in the database will be valid.
    If there is a parse error this will return E_FAILURE.
  -------------------------------------------------------------------------*/
  if(ds707_iface4_hdlr_read_record_from_efs( &efs_db, 
                                             &err_num, 
                                             ppp_auth_info_ptr ) == -1)
  {
    ds3gcfgmgr_efs_file_close(&efs_db);
    if(err_num != (sint15) E_EOF)
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                      "Error %d parsing record from EFS.Aborting ",
                      err_num);
      return (sint15) E_FAILURE;
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "EOF reached reading record from EFS.");
      return (sint15) E_SUCCESS;
    }
  }
  ds3gcfgmgr_efs_file_close(&efs_db);
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "EFS file read successfully");
  return (sint15) E_SUCCESS;
}

/*===========================================================================
FUNCTION       DS707_IFACE4_HDLR_FORM_EAP_NAI

DESCRIPTION    This function forms the eap nai based on the IMSI value 
               
DEPENDENCIES   NONE

RETURN VALUE   boolean

SIDE EFFECTS   NONE
===========================================================================*/
static boolean ds707_iface4_hdlr_form_eap_nai
(   
  nv_ehrpd_imsi_type   *ehrpd_imsi,
  ppp_auth_info_type *ppp_auth_info_ptr
)
{

  char                 user_id_buff[PPP_EAP_MAX_USER_ID_LEN];               
  char                 buff[20]; /* temporary buffer for itoa func */
  uint8                i = 0;
  uint8                n = 0;
  uint8                index = 0;

  if( ( ppp_auth_info_ptr == NULL ) || ( ehrpd_imsi == NULL ) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid parameter");
    return FALSE;
  }

  /*-------------------------------------------------------------------
   Check if number of digits for MCC, MNC and MSIN in the IMSI 
   are valid 
   Number of MCC digits has to be 3 (defined as MAX_MCC_DIGITS)
   Number of MNC digits can be 2 or 3 (3 defined as MAX_MNC_DIGITS)
   Number of MSIN digits can be upto 10, since total IMSI digits are
   not more than 15 (10 defined as MAX_MSIN_DIGITS)
   Only if IMSI read is successfull and IMSI is valid then form NAI 
  ------------------------------------------------------------------*/

  if ((ehrpd_imsi->num_of_mcc_digits != MAX_MCC_DIGITS) ||
      (ehrpd_imsi->num_of_mnc_digits < (MAX_MNC_DIGITS - 1)) ||
      (ehrpd_imsi->num_of_mnc_digits > MAX_MNC_DIGITS) || 
      (ehrpd_imsi->num_of_msin_digits > MAX_MSIN_DIGITS))
  {
    DATA_IS707_MSG3(MSG_LEGACY_ERROR, "EAP: IMSI invalid. "
                    "number of MCC digits %d, MNC digits %d, MSIN digits %d",
                    ehrpd_imsi->num_of_mcc_digits,
                    ehrpd_imsi->num_of_mnc_digits,
                    ehrpd_imsi->num_of_msin_digits); 
    return FALSE;
  }

  /*---------------------------------------------------------------
    Form the NAI from the IMSI
    "6<IMSI>@nai.epc.mnc<MNC>.mcc<MCC>.3gppnetwork.org" for 
    EAP AKA' authentication
  ----------------------------------------------------------------*/ 
  memset( user_id_buff, 0, sizeof(user_id_buff) );
  index = 0;

  /* prefix digit 6 */
  (void)dsatutil_itoa(6, (unsigned char *)buff, 10);
  n = 0;
  while ( (user_id_buff[index++] = buff[n++]) != '\0');
  /* decrement index to start from null position for next char */
  index--;

  /* MCC digits from IMSI */
  for (i = 0; i < ehrpd_imsi->num_of_mcc_digits; i++)
  {
    (void)dsatutil_itoa( ehrpd_imsi->ehrpd_mcc[i], (unsigned char *)buff, 10);
    n = 0;
    while ( (user_id_buff[index++] = buff[n++]) != '\0'); 
    /* decrement index to start from null position for next char */
    index--;
  }

  /* MNC digits from IMSI */
  for (i = 0; i < ehrpd_imsi->num_of_mnc_digits; i++)
  {
    (void)dsatutil_itoa(ehrpd_imsi->ehrpd_mnc[i], (unsigned char *)buff, 10);
    n = 0;
    while ( (user_id_buff[index++] = buff[n++]) != '\0');  
    /* decrement index to start from null position for next char */
    index--;
  }

  /* MSIN digits from IMSI */
  for (i = 0; i < ehrpd_imsi->num_of_msin_digits; i++)
  {
    (void)dsatutil_itoa(ehrpd_imsi->ehrpd_msin[i], (unsigned char *)buff, 10);
    n = 0;
    while ( (user_id_buff[index++] = buff[n++]) != '\0');                                                 
    /* decrement index to start from null position for next char */
    index--;
  }

  (void)strlcpy(buff, "@nai.epc.mnc", 13);
  n = 0;
  while ( (user_id_buff[index++] = buff[n++]) != '\0'); 
  /* decrement index to start from null position for next char */
  index--;

  /* if there are 2 MNC digits, prefix it with 0 */
  if ( ehrpd_imsi->num_of_mnc_digits < MAX_MNC_DIGITS )
  {
    (void)dsatutil_itoa(0, (unsigned char *)buff, 10);
    n = 0;
    while ( (user_id_buff[index++] = buff[n++]) != '\0'); 
    /* decrement index to start from null position for next char */
    index--;
  }
  
  /* add remaining MNC digits */
  for (i = 0; i < ehrpd_imsi->num_of_mnc_digits; i++)
  {
    (void)dsatutil_itoa( ehrpd_imsi->ehrpd_mnc[i], (unsigned char *)buff, 10);
    n = 0;
    while ( (user_id_buff[index++] = buff[n++]) != '\0');
    /* decrement index to start from null position for next char */
    index--;
  }

  (void)strlcpy(buff, ".mcc", 5);
  n = 0;
  while ( (user_id_buff[index++] = buff[n++]) != '\0');
  /* decrement index to start from null position for next char */
  index--;

  /* add MCC digits */
  for (i = 0; i < ehrpd_imsi->num_of_mcc_digits; i++)
  {
    (void)dsatutil_itoa(ehrpd_imsi->ehrpd_mcc[i], (unsigned char *)buff, 10);
    n = 0;
    while ( (user_id_buff[index++] = buff[n++]) != '\0');                                                  
    /* decrement index to start from null position for next char */
    index--;
  }

  (void)strlcpy(buff, ".3gppnetwork.org", 17);
  n = 0;
  while ( (user_id_buff[index++] = buff[n++]) != '\0');
  index--;

  DATA_IS707_MSG2(MSG_LEGACY_MED, "EAP: user_id copied %s, user_id_len %d",
                  user_id_buff, index);

  ppp_auth_info_ptr->eap_user_id_len = index;
  /*lint -save -e545 Passing array and &array is the same as long as the cast is correct */
  memscpy( &(ppp_auth_info_ptr->eap_user_id),
           PPP_EAP_MAX_USER_ID_LEN,
           user_id_buff, 
           ppp_auth_info_ptr->eap_user_id_len);
  /*lint -restore */
  return TRUE;

} /*ds707_iface4_hdlr_form_eap_nai*/

/*===========================================================================
FUNCTION       DS707_IFACE4_HDLR_GET_EAP_PARAMETER

DESCRIPTION    Function gets the EAP parameter from NV item and sets them 
               to the PPP config.

               Also, a call back funcation is registered with PPP which 
               will be envoked when EAP succeeds. A cookie is provided when 
               registerd.
               
DEPENDENCIES    1. The input parameter of the function
                2. NV read

RETURN VALUE   

SIDE EFFECTS   The PPP configuration parameters are populated with USER ID  
               and SHARED SECRET. 
===========================================================================*/
void ds707_iface4_hdlr_get_eap_parameter
(
  ppp_auth_info_type *ppp_auth_info_ptr
)
{
  boolean              setup_fail = FALSE;
  nv_stat_enum_type    nv_status; /* status from NV call     */
  nv_item_type         nv_item;
  nv_ehrpd_imsi_type   ehrpd_imsi;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  if(ppp_auth_info_ptr == NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid parameter");
    return;
  }

  ppp_auth_info_ptr->eap_user_id_len = 0;
  ppp_auth_info_ptr->passwd_len = 0;

  /*----------------------------------------------------------------------
   set EAP algorithm to EAP_AKA_ALGO_MILENAGE
  -----------------------------------------------------------------------*/
  ppp_auth_info_ptr->aka_algo_type = EAP_AKA_ALGO_MILENAGE;
  ppp_auth_info_ptr->ehrpd_usim_present = FALSE;

  /*----------------------------------------------------------------------
    Read the NV to decide if the EAP parameters are in NV or on the card.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
  DATA_IS707_MSG0( MSG_LEGACY_MED, "EAP: NV_EHRPD_AUTH_IN_USIM_I reading");
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_EHRPD_AUTH_IN_USIM_I, &nv_item, ds707_curr_subs_id() );
  if( nv_status == NV_DONE_S)
  {
    ppp_auth_info_ptr->ehrpd_usim_present = nv_item.ehrpd_auth_in_usim ;
    DATA_IS707_MSG0( MSG_LEGACY_MED, "Read EAP parameters from card");
  }
  else 
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
            "NV_EHRPD_AUTH_IN_USIM_I read failed. Using NV mode as default");
    ppp_auth_info_ptr->ehrpd_usim_present = FALSE;
  }

  if( ( setup_fail == FALSE ) && ( ppp_auth_info_ptr->ehrpd_usim_present ) )
  {
    /*----------------------------------------------------------------------
      Read IMSI from the card. 
      If read succeeds form NAI and update accordingly
    -----------------------------------------------------------------------*/
    if(FALSE != ( ds3g_mmgsdi_get_imsi_data (&ehrpd_imsi) ) )
    {
      setup_fail = !( ds707_iface4_hdlr_form_eap_nai( &ehrpd_imsi,
                                                      ppp_auth_info_ptr ) );
    }
    else
    {
      setup_fail = TRUE;
    }
  }
  else if ( ( setup_fail == FALSE ) && 
            ( ppp_auth_info_ptr->ehrpd_usim_present == FALSE ) )
#endif /*FEATURE_MMGSDI_SESSION_LIB*/
  {
    /*----------------------------------------------------------------------
      Read IMSI NV. If read succeeds form NAI and write to eap_user_id and 
      length to eap_user_id_len
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "Reading EAP parameters from NV : NV_EHRPD_IMSI_I");

    nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_EHRPD_IMSI_I, &nv_item, ds707_curr_subs_id() );
    if( nv_status == NV_DONE_S)
    {
      memscpy( (void*)&ehrpd_imsi, 
               sizeof(nv_ehrpd_imsi_type),
               (void*)&(nv_item.ehrpd_imsi), 
               sizeof(nv_ehrpd_imsi_type) );
      
      setup_fail = !( ds707_iface4_hdlr_form_eap_nai( &ehrpd_imsi, 
                                                      ppp_auth_info_ptr ) );
    } /* if nv_status == NV_DONE_S */
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "EAP: NV_EHRPD_IMSI_I read FAILED");
      setup_fail = TRUE;    
    }
  
    if(setup_fail == FALSE )
    {
      /*----------------------------------------------------------------------      
        When IMSI is read from NV and only if the USER ID read succeds, 
        read the shared secret from NV.  If the NV read succeeds, 
        load the shared secret to the ppp config. 
    
        If the NV read fails, mark to unset the user_id_len and user_id set 
        before.
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "EAP: NV_PPP_EAP_SHARED_SECRET_I reading:");
      nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_PPP_EAP_SHARED_SECRET_I, 
                                   &nv_item, ds707_curr_subs_id() );
    
      if( nv_status == NV_DONE_S)
      {
        /*---------------------------------------------------------------------
        The standard defined SHARED SECRET LENGTH is 16 bytes. Valide this NV
        item. If the len is not 16 byte (128 bits), fail the procedure. Makr to 
        unset user_id_len and user_id set before. 
        ----------------------------------------------------------------------*/
        if(nv_item.ppp_eap_shared_secret.eap_shared_secret_len != 
                                                     PPP_EAP_SHARED_SECRET_LEN)
        {
          DATA_IS707_MSG1( MSG_LEGACY_MED, 
                           "EAP: SHARED_SECRET_LEN FAILED, read len = %d:", 
                            nv_item.ppp_eap_shared_secret.eap_shared_secret_len);
          setup_fail = TRUE;
        }
        else
        {
          /*lint -save -e545 Passing array and &array is the same as long as the cast is correct */
          memscpy( &(ppp_auth_info_ptr->eap_shared_secret),
                   PPP_EAP_SHARED_SECRET_LEN, 
                   (char *)nv_item.ppp_eap_shared_secret.eap_shared_secret,
                   nv_item.ppp_eap_shared_secret.eap_shared_secret_len ); 
          /*lint -restore */
          setup_fail = FALSE;
        }
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "EAP: NV_PPP_EAP_SHARED_SECRET_I read FAILED:");
        setup_fail = TRUE;
      }
    }
  
    if( setup_fail == FALSE )
    {
      /*----------------------------------------------------------------------
        read OP field NV and populate aka_algo_milenage_op_data and 
        aka_algo_milenage_op_data_len
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "EAP: NV_EHRPD_MILENAGE_OP_I reading:");
      nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_EHRPD_MILENAGE_OP_I, &nv_item, ds707_curr_subs_id() );
      if( nv_status == NV_DONE_S)
      {
        if(nv_item.ehrpd_milenage_op.ehrpd_milenage_op_length != 
           EAP_AKA_ALGO_MILENAGE_OP_LEN)
        {
          DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                           "EAP: ehrpd_milenage_op_length FAILED, read len = %d:", 
                            nv_item.ehrpd_milenage_op.ehrpd_milenage_op_length);
          setup_fail = TRUE;
        }
        else
        {
          ppp_auth_info_ptr->aka_algo_milenage_op_data_len = 
                      nv_item.ehrpd_milenage_op.ehrpd_milenage_op_length;
          /*lint -save -e545 Passing array and &array is the same as long as the cast is correct */
          memscpy( &(ppp_auth_info_ptr->aka_algo_milenage_op_data),
                   EAP_AKA_ALGO_MILENAGE_OP_LEN,
                   (uint8 *)nv_item.ehrpd_milenage_op.ehrpd_milenage_op,
                   ppp_auth_info_ptr->aka_algo_milenage_op_data_len);
          /*lint -restore */
          setup_fail = FALSE;
        }
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "EAP: NV_EHRPD_MILENAGE_OP_I read FAILED:");
        setup_fail = TRUE;
      }
    }
  
    if(setup_fail == FALSE)
    {
      /*-----------------------------------------------------------------------
        If all above are true read the initial sequence number from EFS. 
    
        If the NV read fails, mark to unset the user_id_len and user_id set 
        before.
      -----------------------------------------------------------------------*/
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "eHRPD: Reading initial sequence number from EFS");  
      if(ds707_iface4_hdlr_read_isn_from_efs(ppp_auth_info_ptr) == (sint15) E_SUCCESS)
      {
        if(ppp_auth_info_ptr->aka_algo_seqnum_arr_num_elements != 32)
        {
          DATA_IS707_MSG1(MSG_LEGACY_MED, "Invalid array size %d, Fail", 
                        ppp_auth_info_ptr->aka_algo_seqnum_arr_num_elements);
          setup_fail = TRUE;
        }
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "Initial sequence number from EFS failed");
        setup_fail = TRUE;
      } 
    }
  }/*ppp_auth_info_ptr->ehrpd_usim_present is FALSE*/
 
  /*-------------------------------------------------------------------------
    In case of failure, set the EAP parameter to invalid. 
  -------------------------------------------------------------------------*/
  if(setup_fail == TRUE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "EAP: ds707_iface4_hdlr_get_eap_parameter FAILED:");
    ppp_auth_info_ptr->eap_user_id_len = 0;
    ppp_auth_info_ptr->get_config_data_after_auth_ptr = NULL;
    ppp_auth_info_ptr->eap_user_data = NULL;
  }
  else
  {
    /*-----------------------------------------------------------------------
    Register the call back funcation and provide the cookie to the PPP.
    -----------------------------------------------------------------------*/
    ppp_auth_info_ptr->get_config_data_after_auth_ptr = 
                                      dssnet4i_handle_eap_auth_ev_cback;
    ppp_auth_info_ptr->eap_user_data = (void *) &ds707_eap_user_data;         
  }
}/* ds707_iface4_hdlr_get_eap_parameter()*/

#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION       DS707_IFACE4_HDLR_PROCESS_CONN_DELAY_TIMER_EV

DESCRIPTION    This function handles the connection release delay timer expiry
               event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_iface4_hdlr_process_conn_delay_timer_ev(
  dssnet4_sm_cb_type  *instance_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "CB: Conncetion Delay timer expired");
  
  /*-------------------------------------------------------------------
  Transition the state to CLOSED_STATE
  -------------------------------------------------------------------*/
  if ( instance_ptr != NULL )
  {
    dssnet4i_transition_state(instance_ptr, DSSNET4I_CLOSED_STATE);
  }
} /* ds707_iface4_hdlr_process_conn_delay_timer_ev() */
#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================
FUNCTION DSSNET4I_CONVERT_MIP_RRP_CODE

DESCRIPTION    This function converts MIP RRP failure code into 
               ps_iface_net_down_reason_type 

DEPENDENCIES   None.

RETURN VALUE   ps_iface_net_down_reason_type

SIDE EFFECTS   None

===========================================================================*/

ps_iface_net_down_reason_type dssnet4i_convert_mip_rrp_code
(
  byte mip_rrp_code
)
{
  int32 end_call_reason = DSSNET4_MIP_ERR_CODE_MASK | ( int32 ) mip_rrp_code;
  DATA_IS707_MSG2(MSG_LEGACY_MED,
                  "Received MIP RRP code as %d and set PS net down reason to 0x%x ", 
                  mip_rrp_code, 
                  end_call_reason);

  return (ps_iface_net_down_reason_type) end_call_reason;
}/* dssnet4i_convert_mip_rrp_code */
/*===========================================================================
FUNCTION DSSNET4I_SET_MIP_RRP_CODE

DESCRIPTION    This function sets the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_set_mip_rrp_code
(
  dssnet4_sm_cb_type *instance_ptr,
  byte mip_rrp_code
)
{
  instance_ptr->dssnet4_mip_ppp_info.mip_rrp_code = mip_rrp_code;
  DATA_IS707_MSG1(MSG_LEGACY_MED, "Set MIP RRP code to %d ", mip_rrp_code);

}/* dssnet4i_set_mip_rrp_code */
/*===========================================================================
FUNCTION DSSNET4I_GET_MIP_RRP_CODE

DESCRIPTION    This function gets the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
byte dssnet4i_get_mip_rrp_code
(
  dssnet4_sm_cb_type *instance_ptr
)
{
  return instance_ptr->dssnet4_mip_ppp_info.mip_rrp_code;
} /* dssnet4i_get_mip_rrp_code */
/*===========================================================================
FUNCTION DSSNET4I_CLEAR_MIP_RRP_CODE

DESCRIPTION    This function clears the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_clear_mip_rrp_code
(
  dssnet4_sm_cb_type *instance_ptr
)
{
  instance_ptr->dssnet4_mip_ppp_info.mip_rrp_code = MIP_RRP_CODE_SUCCESS;
  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "Clear MIP RRP code to %d ", 
                  instance_ptr->dssnet4_mip_ppp_info.mip_rrp_code);
} /* dssnet4i_clear_mip_rrp_code */
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION DSSNET4I_CONVERT_PPP_FAILURE_CODE

DESCRIPTION    This function converts PPP failure code into 
               ps_iface_net_down_reason_type 

DEPENDENCIES   None.

RETURN VALUE   ps_iface_net_down_reason_type

SIDE EFFECTS   None

===========================================================================*/
ps_iface_net_down_reason_type dssnet4i_convert_ppp_failure_code
(
  ppp_fail_reason_e_type reason
)
{
  ps_iface_net_down_reason_type end_call_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;

  if ( (reason >= PPP_FAIL_REASON_VSNCP_TIMEOUT) &&
       (reason < PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MAX) )
  {
    end_call_reason = (ps_iface_net_down_reason_type)(DSSNET4_VSNCP_ERR_CODE_MASK | (int32)reason );
  }
  else
  {
    if ( ((reason > PPP_FAIL_REASON_NONE) &&
         (reason <= PPP_FAIL_REASON_OPTION_MISMATCH)) || 
        ((reason >= PPP_FAIL_REASON_AUTH_PAP_FAILURE) &&
         (reason < PPP_FAIL_REASON_MAX))
      )
    {
      end_call_reason = (ps_iface_net_down_reason_type)(DSSNET4_PPP_ERR_CODE_MASK | (int32)reason );
    }
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
        "dssnet4: PPP error code: %d, corresponding PS net down reason: 0x%x ",
        reason, end_call_reason );

  return end_call_reason;

}/* dssnet4i_convert_ppp_failure_code */

/*===========================================================================
FUNCTION DSSNET4I_SET_PPP_FAILURE_CODE

DESCRIPTION    This function sets the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_set_ppp_failure_code
(
  dssnet4_sm_cb_type *instance_ptr,
   ppp_fail_reason_e_type reason
)
{
  instance_ptr->dssnet4_mip_ppp_info.reason = reason;
  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "Set MIP PPP code to %d ", 
                  instance_ptr->dssnet4_mip_ppp_info.reason);
}/* dssnet4i_set_ppp_failure_code */

/*===========================================================================
FUNCTION DSSNET4I_GET_PPP_FAILURE_CODE

DESCRIPTION    This function gets the PPP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
ppp_fail_reason_e_type dssnet4i_get_ppp_failure_code
(
  dssnet4_sm_cb_type *instance_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!(DSSNET4I_IS_VALID(instance_ptr)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid dssnet4 instance_ptr");
    return PPP_FAIL_REASON_MAX;
  }

  return (instance_ptr->dssnet4_mip_ppp_info.reason);
} /* dssnet4i_get_ppp_failure_code */

/*===========================================================================
FUNCTION DSSNET4I_CLEAR_PPP_FAILURE_CODE

DESCRIPTION    This function clears the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_clear_ppp_failure_code
(
  dssnet4_sm_cb_type *instance_ptr
)
{
  instance_ptr->dssnet4_mip_ppp_info.reason = PPP_FAIL_REASON_NONE;
  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "Clear PPP failure code to %d ", 
                  instance_ptr->dssnet4_mip_ppp_info.reason);
} /* dssnet4i_clear_ppp_failure_code */

/*===========================================================================
FUNCTION DSSNET4_SET_PS_CALL_END_REASON

DESCRIPTION    This function sets end_call_reason to ps. 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4_set_ps_call_end_reason
(
  dssnet4_sm_cb_type *instance_ptr,           /* dssnet4 instance ptr      */
  ps_iface_net_down_reason_type call_end_reason
)
{
  if ( (instance_ptr != NULL) &&
       (instance_ptr->call_end_reason == PS_NET_DOWN_REASON_NOT_SPECIFIED) )
  {
    instance_ptr->call_end_reason = call_end_reason;
    DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                     "call end reason for dssnet4 0x%x set to 0x%x", 
                     instance_ptr, call_end_reason );
  }
} /* dssnet4_set_ps_call_end_reason() */

/*===========================================================================
FUNCTION DSSNET4_SET_PS_LOCAL_ABORT

DESCRIPTION    This function sets local_abort to ps. 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4_set_ps_local_abort
(
  dssnet4_sm_cb_type *instance_ptr,           /* dssnet4 instance ptr      */
  boolean             local_abort             /* local abort               */
)
{
  if (instance_ptr == NULL)
  {
    DATA_ERR_FATAL("Instance pointer for dssnet4_set_ps_local_abort is NULL");
    return;
  }

  else
  {
    instance_ptr->local_abort = local_abort;
    DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                     "local abort for dssnet4 0x%x, set to %d", 
                     instance_ptr,local_abort );
  }
} /* dssnet4_set_ps_local_abort() */

/*===========================================================================
FUNCTION DSSNET4I_GET_PS_CALL_END_REASON

DESCRIPTION    This function gets end_call_reason to ps. If mip_rrp_code is
               set, mip_rrp_code shall be converted to PS reason

DEPENDENCIES   None.

RETURN VALUE   ps_iface_net_down_reason_type

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_get_ps_call_end_reason
(
  dssnet4_sm_cb_type *instance_ptr
)
{
#ifdef FEATURE_DS_MOBILE_IP
  byte mip_rrp_code = MIP_RRP_CODE_SUCCESS;
#endif
  ppp_fail_reason_e_type reason = PPP_FAIL_REASON_NONE;

  if (instance_ptr->call_end_reason == PS_NET_DOWN_REASON_NOT_SPECIFIED)
  {
#ifdef FEATURE_DS_MOBILE_IP
    mip_rrp_code = dssnet4i_get_mip_rrp_code(instance_ptr);
   /* check mip_rrp_code */
    if ( mip_rrp_code )
    {
      /* if mip_rrp_code is set, we need to pass this info to PS*/
      dssnet4_set_ps_call_end_reason(instance_ptr, 
                                      dssnet4i_convert_mip_rrp_code(mip_rrp_code) );
      DATA_IS707_MSG1(MSG_LEGACY_MED,
                      "Convert MIP rrp error code to PS call end reason 0x%x ",  
                       instance_ptr->call_end_reason);
      /* clean up mip rrp code */
      dssnet4i_clear_mip_rrp_code(instance_ptr);
    }
    else
#endif
    {
      reason = dssnet4i_get_ppp_failure_code(instance_ptr);
      if( reason )
      {
        dssnet4_set_ps_call_end_reason(instance_ptr, 
                                        dssnet4i_convert_ppp_failure_code(reason) );
        DATA_IS707_MSG1(MSG_LEGACY_MED,
                        "Convert PPP error code to PS call end reason 0x%x ",  
                        instance_ptr->call_end_reason);
        dssnet4i_clear_ppp_failure_code(instance_ptr);
      }
     
      /*-------------------------------------------------------------
        if mip_rrp_code is not set, we just map CM call end reason to
        PS call end reason and pass it to ps
      -------------------------------------------------------------*/
      else
      {
        dssnet4_set_ps_call_end_reason(instance_ptr, 
                                        ds707_pkt_mgr_get_last_call_end_reason());
        DATA_IS707_MSG1(MSG_LEGACY_MED,
                        "Convert CM call end reason to PS call end reason 0x%x ", 
                        instance_ptr->call_end_reason);
      }
    }
  }
} /* dssnet4i_get_ps_call_end_reason  */


/*=========================================================================
FUNCTION       DSSNET4_IFACE_DOWN_IND_CALLBACK_REG

DESCRIPTION    This function registers for v4 iface down ind event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
int dssnet4_iface_down_ind_callback_reg
(
  dssnet4_sm_cb_type             *instance_ptr,
  dssnet4_iface_down_ind_cb_type  cb_fn
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == instance_ptr)
  {
    return -1;
  }
  instance_ptr->iface_down_ind_cb = cb_fn;

  return 0;
} /* dssnet4_iface_down_ind_callback_reg() */

/*=========================================================================
FUNCTION       DSSNET4_GET_INSTANCE_STATE

DESCRIPTION    Returns dssnet4 instance state
               
DEPENDENCIES   NONE

RETURN VALUE   Failure - DSSNET4I_MAX_STATE
               Success - dssnet4 instance state

SIDE EFFECTS   NONE
=========================================================================*/
dssnet4i_state_type dssnet4_get_instance_state
(
  dssnet4_sm_cb_type *dssnet4_sm_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == dssnet4_sm_cb)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "dssnet4_sm_cb is NULL");
    return DSSNET4I_MAX_STATE;
  }

  return (dssnet4_sm_cb->state);

} /* dssnet4_get_instance_state() */

/*=========================================================================
FUNCTION       DSSNET4_GET_INSTANCE_CALL_END_REASON

DESCRIPTION    Returns dssnet4 instance call end reason
               
DEPENDENCIES   NONE

RETURN VALUE   Failure - PS_NET_DOWN_REASON_MAX
               Success - dssnet4 instance call end reason

SIDE EFFECTS   NONE
=========================================================================*/
ps_iface_net_down_reason_type dssnet4_get_instance_call_end_reason
(
  dssnet4_sm_cb_type *dssnet4_sm_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == dssnet4_sm_cb)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "dssnet4_sm_cb is NULL");
    return PS_NET_DOWN_REASON_MAX;
  }

  return (dssnet4_sm_cb->call_end_reason);

} /* dssnet4_get_instance_call_end_reason() */

/*===========================================================================
FUNCTION DSSNET4_RESET_CALL_END_REASON

DESCRIPTION    This function resets end_call_reason to 
               PS_NET_DOWN_REASON_NOT_SPECIFIED 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4_reset_call_end_reason
(
  dssnet4_sm_cb_type *dssnet4_sm_cb           /* dssnet4 instance ptr      */
)
{
  if (NULL == dssnet4_sm_cb)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "dssnet4_sm_cb is NULL. Can't reset call end reason");
    return;
  }

  dssnet4_sm_cb->call_end_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  return;

} /* dssnet4_reset_ps_call_end_reason */

/*===========================================================================
FUNCTION DSSNET4_RESET_LOCAL_ABORT

DESCRIPTION    This function resets local_abort to 
               FALSE 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4_reset_local_abort
(
  dssnet4_sm_cb_type *dssnet4_sm_cb           /* dssnet4 instance ptr      */
)
{
  if (NULL == dssnet4_sm_cb)
  {
    DATA_ERR_FATAL("dssnet4_sm_cb is NULL. Can't reset call end reason");
  }

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                     "local abort for dssnet4 0x%x,reset to FALSE", 
                     dssnet4_sm_cb);
  dssnet4_sm_cb->local_abort = FALSE;
  return;

} /* dssnet4_reset_ps_local_abort */

/*=========================================================================
FUNCTION       DSSNET4_SET_IS_BRINGUP_PHYS_LINK

DESCRIPTION    Set is_bringup_phys_link flag which will be used to remember 
               that phys link needs to be brought up again after it is
               cleaned up. This is set to TRUE when phys link bringup fails
               due to phys link being in going_null state
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet4_set_is_bringup_phys_link
(
  dssnet4_sm_cb_type            *instance_ptr,
  boolean                        is_bringup_phys_link
)
{
  if (!(DSSNET4I_IS_VALID(instance_ptr)))
  {
    DATA_ERR_FATAL("dssnet4_sm_cb is not valid");
    return;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "dssnet4 0x%x, set is_bringup_phys_link = %d",
                  instance_ptr, is_bringup_phys_link);
  instance_ptr->is_bringup_phys_link = is_bringup_phys_link;
  return;
} /* dssnet4_set_is_bringup_phys_link() */

/*=========================================================================
FUNCTION       DSSNET4_GET_IS_BRINGUP_PHYS_LINK

DESCRIPTION    Get is_bringup_phys_link flag, which will be used to remember 
               that phys link needs to be brought up again after it is
               cleaned up.
               
DEPENDENCIES   NONE

RETURN VALUE   is_bringup_phys_link value

SIDE EFFECTS   NONE
=========================================================================*/
boolean dssnet4_get_is_bringup_phys_link
(
  dssnet4_sm_cb_type            *instance_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!(DSSNET4I_IS_VALID(instance_ptr)))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "instance_ptr is not valid");
    return FALSE;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "dssnet4 0x%x, get is_bringup_phys_link = %d",
                  instance_ptr, instance_ptr->is_bringup_phys_link);
  return (instance_ptr->is_bringup_phys_link);
} /* dssnet4_get_is_bringup_phys_link() */

/*lint -restore Restore lint error 641*/
/*lint -restore Restore lint error 715*/
/*lint -restore Restore lint error 655*/
#endif /* FEATURE_DATA_IS707 */
