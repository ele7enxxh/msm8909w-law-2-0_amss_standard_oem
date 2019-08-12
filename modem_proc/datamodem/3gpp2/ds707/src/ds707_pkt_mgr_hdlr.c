/*===========================================================================

                   D S 7 0 7 _ P K T _ M G R _ H D L R. C
                   
GENERAL DESCRIPTION
  This file contains the handlers for DS3GMGR functionality.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds707_pkt_mgr_init() must be called at startup.

  Copyright (c) 2002 - 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_mgr_hdlr.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
09/11/14    sd     Fix to disable the flow and block reorigination when 1X 
                   is not in service.
08/05/14    ms     DSDA Phase 2 changes.
07/28/14    hr     Sim swap feature
07/02/14    sd     Interfacing with CFCM for CPU and Thermal based flow
                   control events.
06/26/14    ms     Data+MMS - DSDA changes Phase 1.
06/04/14    sd     Fix to utilize DSM memory event callbacks to enable and
                   disable flow control.
04/04/14    sd     Fix to notify bearer tech change when 1x/HDR is acquired.
01/08/14    ms     Originate Data call with 1x system if there is no Data call
                   on eHRPD and no HDR / eHRPD system avaialble.
10/09/13    sc     Removed featurization of FEATURE_DUAL_SIM
09/25/13    sd     Made changes to not honor call connected event if primary 
                   phys link state is in going null or going down state.
06/03/13    sd     ATCoP decoupling changes
01/08/13    sd     Disable the physical link flow when RETRY DELAY is triggered
12/03/12    sd     DS sets the srv type to HYBRID in eHRPD mode during origination
10/17/12    dvk     Merged fix to tear down 3gpp2 call when moving from 1x/HDR 
                   to 3gpp system if handoff is not allowed. 
06/19/12    msh    Call MDR rlp_reset on call end for SO 15
04/18/12    jz     Seperate old JCDMA and Simplified JCDMA 
04/12/12    jee    To propagate correct call end reason from MH
03/09/12    msh    Removed FEATURE_DATA_NIKEL 
01/02/12    msh    Coan: Feature cleanup 
04/01/12    jee    Fix to set PCMT value to iRAT when LTE attach happens
12/23/11    sb     Fixed compiler warnings 
12/21/11    msh    In cnedsd_pref_sys_chg_hdlr set the current sys_mode to the new_mode 
12/14/11    jee    Fix to stay in partial context after handoff to LTE
11/18/11    vpk    HDR SU API cleanup
10/28/11    msh    Fixed compiler warnings 
09/26/11    msh    Fixed compiler warnings 
09/19/11    msh    Fixed compiler warnings
09/08/11    ash    Added support for eHRPD to HRPD fallback.
08/09/11    sha    Global variable clean up.
04/15/11    ls     Get call allowed hdlr and register a function for EPC to 
                   call 
03/15/11    mg     Global variable cleanup
02/11/11    ack    Global Variable Cleanup
12/10/10    ls     Pass cm call cmd err info to 
                   ds707_pkt_mgr_set_last_call_end_reason()
11/22/10    sn     Removed REX premption macros as it is not required.
11/22/10    ttv    Added support for DSDS.
11/08/10    mg     KW fix
11/03/10    ls     Remove iface state check when receiving call end from CM
10/14/10    ss     Adding debug messages before ASSERT.
10/13/10    op     Migrated to MSG 2.0 macros
08/03/10    rp     Added null check for flow_ptr. 
08/02/10    rp     Added Null check for tc_state_ptr in function 
                   ds707_pkti_get_data_count_hdlr.
06/17/10    as     Fusion related additions
05/17/10    ls     Modify CCM featurization
05/29/10    ls     Support refresh voting
04/26/10    gc     Lint Fixes
03/24/10    gc     Added code to return previous call bytes in get_last_sess_count_hdlr
03/15/10    ls     Silent Redial Feature
12/10/09    ps     Mainlining OMH feature based on new profile management
                   design.
11/10/09    as     Cleared Windows Compiler warnings for MOB builds
04/29/09    spn    React to Call End command from CM based on state of all 
                   ifaces. 
02/15/09    ls     Print epzid info before sending origination to CM
12/31/08    psng   Fixed Lint error.
12/07/08    ms     Fixed Offtarget Lint errors and Compiler warnings
11/18/08    sn     DCM: GO NULL timer value will be changed to Voice MOSMS 
                   timer value only if Voice MOSMS timer value is greater 
                   than zero.
11/10/08    yz     Fixed TX/RX byte count issue with wmk approach, 
                   reverted previous changes in rlp
10/07/08    cs     Compiler warning fix.
10/07/08    psng   Replaced legacy F3 messages with 707 messages.
10/06/08    sn     Fix to make sure HDR SMP deactivate is not sent when 1x 
                   data call ends.
07/07/08    ms     Klocwork Fixes
06/17/08    sn     Support for multiple app profiles for OMH.
06/12/08    sn     Fixed featurization issue.
06/06/08    spn    Updated ds707_pkti_get_data_count_hdlr() to return values 
                   in case of MRLP/EMPA
04/14/08    sn     Fixed featurization issue.
04/02/08    hs     Made changes in places where CDSN was updated directly.
03/13/08    sc     Fix to enable phys link flow only after re-orig hdlr reg
02/12/08    sc     Added support to propagate CM call end reason to IFACE.
10/11/07    ac     Propagate the default CTA timer value to RLP to take care
                   of multi-mode targets
08/30/07    sk     Enable/Disable secondary physlink bound flows when primay 
                   physlink bound flows are enabled/disabled
08/20/07    sc     Added debug messages for Application Profiles support
08/16/07    spn    Lint changes.
08/01/07    sk     Fixed featurization error
07/22/07    sk     If NV item NV_HDR_EMPA_SUPPORTED_I is set, the HDR session
                   is not tied to PPP.
07/02/07    sc     Added support for Application Profiles in 
                   ds707_pkti_orig_hdlr
05/25/07  squ/mga  Added code to support FEATURE_DS_LN_UPDATE
04/04/07   as/sq   Fixed Lint error and banned apis
03/08/07    ara    Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X 
01/10/07    gr     Changes to populate the call end reason related parameters
                   in the event_info correctly while publishing 
                   PHYS_LINK_DOWN_EV 
12/11/06    rsj    Added JCDMA 2 JATE changes.
                   Modified  ds707_pkti_call_ended_hdlr to set 
                   global structure to flag CM throttled call.
                   Modified ds707_pkti_orig_hdlr to reset global structure
                   during origination to clear any pending previous values.
10/19/06    rsj    Added JCDMA 2 low battery event processing. Modified 
                   ds707_pkti_call_ended_hdlr to look at primary tc state
                   and send ORDQ value to CM.
10/24/06    rsj	   Add extra featurizing for FEATURE_JCDMA_2 so that 
                   appropriate network mode can be selected based on the 
                   value of force_cdma_only flag. 
07/26/06    as     Changed function call from ds707_wmk_cleanup_fwd_queue() 
                   to ds707_wmk_cleanup_single_wmk in ds707_pkti_cleanup_wmks
07/18/06    vrk    featurize tasklock to prevent RPC call in tasklock. 
04/16/06   squ/gr  added call end info/err field to the call_end_info event.
04/12/06   gr/squ  Transitioned the phys link to DOWN state without cleaning
                   up phys link for the race condition of simultanneous 
		               outgoing and incoming SO33 calls
03/13/06    az     Whenever End key is pressed, added a call to silently 
                   cleanup all secondary flows.
2/17/06     sq     In ds707_pkti_switch_calls_hdlr function, fill out the 
                   orig call info when exist call id can not be found.
2/14/06     sq     In ds707_pkti_switch_calls_hdlr function, added checking 
                   PHYS_LINK_RESUMING state in multiple places.
12/23/05    ksu    EMPA RSP: two fwd rx queues
12/23/05    ksu    EMPA RSP: two rev watermarks per ip flow
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
11/21/05    as/kvd included ds707_pkt_mgr_hdr.h to fix compile warnings
11/11/05    vrk/gr Avoiding ppp resync on failed ALN on MIP
11/07/05    vrk    Check for the LDSN before doing handdown ppp resync
10/27/05    ksu    EMPA: octet stream, octet sequencing
09/10/05    gr     Fixed the problem of ppp_resync_flag reset while going
                   dormant. Now the flag is reset only after the resync
                   procedure completes (successful/failure)
09/09/05    vrk/gr EPZID enhancements releated to L2/L3 ack
06/21/05    gr     Fixed issues with incoming call handler introduced while
                   making changes to support PHYS_LINK_NULL state
06/15/05    spn/vr Added condition CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS  
                   before RLP cleanup in ds707_pkti_call_ended_hdlr() 
06/03/05    ar     Added NULL call progress handler.
05/31/05    gr     Update the last data sess network to what the network 
                   was just prior to cleaning up the phys link.
04/26/05    gr     Added inc_call flag to the ds3g_pkt_call_info_type
04/05/05    gr     Integrated EPZID functionality onto the main line
02/10/05    gr     Moved publishing the CALL_ENDED event in the 
                   call_ended_hdlr after intializing the tc_state_ptr 
02/07/05    rsl    Cleaned up inc_call_hdlr.
02/02/05    gr     Fixed the issue of trying to cleanup 1x RLP for HDR calls
11/09/04    rsl    Added new events for orig, call conn, call ended, inc call
10/21/04    kvd    cleaned up DS707 macros to be called with correct pkt_inst
09/10/04    vas    QOS related changes
07/30/04    gr     Added functionality to reject CDMA data pages when there 
                   is a dormant packet data session on HDR 
05/03/04    gr     Eliminated the HDR RLP init and clean up from ds task. 
                   This has been moved into HDRMC task context.
04/13/04    gr     Modified orig hdlr to set the service type based on
                   force_cdma_only parameter. 
04/11/04    rsl    Added code under FEATURE_HDR to get correct data in 
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

#ifdef FEATURE_DATA_IS707
  #define FEATURE_DSM_WM_CB

  #include "amssassert.h"
  #include "cm.h"
  #include "ds3gmgr.h"
  #include "ds707.h"
  #include "ds707_cback.h"
  #include "ds707_drs.h"
  #include "ds707_pkt_mgr.h"
  #ifdef FEATURE_HDR
    #include "ds707_pkt_mgr_hdr.h"
  #endif /* FEATURE_HDR */
  #include "ds707_pkt_mgr_hdlr.h"
  #include "ds707_sec_pkt_mgr_util.h"
  #include "ds707_pkt_mgri.h"
  #include "ds707_wmk.h"
  #include "ds707_so33.h"
  #include "ds707_so_retrydel.h"
  #include "ds707_epzid.h"
  #include "dscam.h"
  #include "ds_flow_control.h"
  #include "dsat707extctab.h"
  #include "dsat707vendctab.h"
  #include "dsm.h"
  #include "dsrlp_v.h"
  #include "dstaski.h"
  #include "err.h"
  #include "mccsrid.h"
  #include "msg.h"
  #include "data_msg.h"
  #include "queue.h"
  #include "ps_sys_conf.h"
  #include "ps_sys_ioctl.h"

  #ifdef FEATURE_IS95B_MDR
    #include "mdrrlp.h"
  #else
    #include "rlp.h"
  #endif

  #ifdef FEATURE_HDR
    #include "hdrrlp.h"
    #include "hdrpacdsif.h"
    #include "hdrpac.h"
    #include "hdrpac_mdm.h"
    #include "hdrcom_api.h"
    #include "hdrds.h"
    #include "hdrcp_api.h"
	#include "hdrl1_api.h"
    #ifdef FEATURE_HDR_QOS
      #include "ds707_sec_pkt_mgr_handoff.h"
    #endif
    #include "hdrmrlp.h"
  #endif /* FEATURE_HDR */


    #include "ds707_so_jcdma.h"
    #include "ds3gmgrint.h"
    #include "ds707_gonulltmr.h"
    #include "hdrpacdsif.h"
  
  #include "ds707_event_defs.h"

  #include "ds_1x_profile.h"

#include "ds707_sec_pkt_mgr.h"
#include "ds_1x_stats.h"
#include "ds707_gonulltmr.h"


#include <stringl/stringl.h>
#include "ds707_data_session_profile.h"
#include "ds707_wmk.h"
#include "nv.h"
#include "ds707_roaming.h"
#include "ds707_pdn_context.h"
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
#include "ds707_pkt_auto_redial.h"
#endif
#include "ds707_extif.h"
#include "data_err_suppr.h"
#ifdef FEATURE_EHRPD
#include "ds707_sys_chg_hdlr.h"
#include "ds707_devmgr.h"
#endif

#if defined(FEATURE_DATA_FL_FLOW_CONTROL)
#include "ds707_ccm.h"
#endif
#ifdef FEATURE_EPC_HANDOFF
#include "ds_epc_hdlr.h"
#endif

#include "ds3gdsdif.h"
#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
  #include "ds707_s101_vctm.h"
#endif /* FEATURE_DATA_OPTHO */

#include "ds_dsd_ext_i.h"
#include "ds3gtimer.h"
#include "ps_pkt_info_utils.h"
/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/
#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
LOCAL ds3g_incom_e_type ds707_pkti_incoming_page_cmd_hdlr
(
  ds_cmd_type             *cmd_ptr,
  sys_modem_as_id_e_type   subs_id
);
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

LOCAL void ds707_pkti_end_call_hdlr
(
cm_call_id_type        call_id,
cm_end_params_s_type  *end_params,
boolean               *delay_disconnect,
void                  *mode_spec_info_ptr,
sys_modem_as_id_e_type subs_id
);
/*===========================================================================
                            TYPEDEFS
===========================================================================*/

  #define CAN_GO_DORMANT       TRUE
  #define CANNOT_GO_DORMANT    FALSE


/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
   Structure to register with 3G DSMGR.  Has all the callbacks for this
   mode-specific handler. The callback functions pointers are initialized
   at powerup init.
---------------------------------------------------------------------------*/
ds3g_hdlr_func_tbl_type ds707_pkti_mgr_hdlr_func_tbl;

/*--------------------------------------------------------------------------
 Global variables to save the current CTA timer value and force the call
 to go dormant right away. The original CTA timer value is restored after
 the phys link is cleaned up. Set the default value of this parameter to 
 1 second. This parameter contains a value other than 1 only for ALN handup
 scenario (contains the CTA timer value provisioned on the phone). 
---------------------------------------------------------------------------*/
static dword  ds707_pkti_failed_aln_cta_restore_val = 1;

#define                     DS707_NO_OF_SIM    1
/*---------------------------------------------------------------------------
  Structure to generate the throughput Info Indication
---------------------------------------------------------------------------*/
typedef struct
{
  ps_sys_throughput_info_list_type  *report_p;
  /*! Pointer to store the Info list size*/
  uint16                             max_report_size;
  /*! Maximum size of the info list*/
}ds707_report_throughput_info_s;

/*--------------------------------------------------------------------------
   Variable to Cache the throughput info report generated/updated 
   periodically
  --------------------------------------------------------------------------*/
static ds707_report_throughput_info_s ds_707_throughput_info_ind[DS707_NO_OF_SIM];

/*--------------------------------------------------------------------------
 Global variables to save the throughput and rx bytes to be used for 
 calculating and rate events for QMI rate set events
---------------------------------------------------------------------------*/
static uint32               ds707_avg_throughput=0;  /*In Kbps*/
static uint32               ds707_allowed_throughput=0; /*In Kbps*/
static uint64               ds707_total_rx_bytes=0;  /*In Bytes*/


/*Data structure for rate info*/
typedef hdrl1_rmac_rl_rate_info_struct_type ds707_rate_info;
typedef txc_power_level ds707_modem_power_1x;   
typedef hdrl1_lmac_modem_api_data_type ds707_modem_power_hdr;



#ifdef FEATURE_HDR
/*---------------------------------------------------------------------------
  Flag which indicates if  ALN command, (indication sent to DS  from HDRMC to
  DS to do ppp_resync in teh absence of LocationNotification) is pending.
  This flag is set if we are not abe to  do ppp_resync because TC is in
  PHYS_LINK_COMIGN_UP state when we tried to bring up TC to do ALN 
  ppp_resync.
  It is  cleared when the current call is ended / gets connected,
  at which point we bring up TC ( if call is ended) and do a ppp_resync. 
---------------------------------------------------------------------------*/
static boolean aln_cmd_pending = FALSE;
#endif

#ifdef FEATURE_EPC_HANDOFF
/*---------------------------------------------------------------------------
  Function Handler to check if a call is allowed with 3GPP2 MH
---------------------------------------------------------------------------*/
static ds707_call_allowed_hdlr_f_type ds707_call_allowed_hdlr_f = NULL;
#endif /* FEATURE_EPC_HANDOFF */

/*---------------------------------------------------------------------------
  Store the last known good sytem
---------------------------------------------------------------------------*/
static ds707_sys_info_enum_type ds707_last_good_sys = DS707_SYS_INFO_NON_EPC_SYSTEM;

/*---------------------------------------------------------------------------
 FR 18742 : This flag is set in CALL ended handler in DS task context.
This flag is reset to FALSE in SS Event handler in CM task context 
This flag is also read in call orig handler in DS task context 
---------------------------------------------------------------------------*/
static boolean  ds707_is_bsr_completed  = FALSE;

/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_PKTI_CLEANUP_RLP

DESCRIPTION   Called when phys link goes down.  Cleans up RLP instances.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_cleanup_rlp
(
  void
)
{
  ds707_tc_state_type       *tc_state_ptr;
  byte                       sr_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(IS_IN_DS_TASK());

  tc_state_ptr    = ds707_get_pri_tc_ptr();

#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
   Clean up the HDR RLP. Note that usually, the RLP would already have been
   cleaned up through HDRPAC when the traffic channel is brought down. This
   case is a protection against the race condition where the traffic channel
   comes up and goes down immediately, resulting in the HDR RLp being 
   initialized and cleaned up through HDR PAC. But DS gets the Call connected
   from CM only after the traffic channel is already torn down. so for this 
   case, we need to make sure that when Ds gets the call ended command, it
   will clean up HDR RLP.
  -------------------------------------------------------------------------*/
  if ( DS_ISDATA_HDR(tc_state_ptr->so) )
  {
    /*  hdrpac_mdm_sn_cleanup();  */
  }
  else
#endif /* FEATURE_HDR */
  {
    /*-----------------------------------------------------------------------
     Cleanup 1X RLP.
    -----------------------------------------------------------------------*/
    sr_id = dsrlp_get_sr_id( DSRLP_FIXED_SR_ID_INDEX);
    dsrlp_cleanup(sr_id, DSRLP_VER_1X_3); 
  
    /*-----------------------------------------------------------------------
     Need to call rlp reset specific for MDR RLP for SO 15 
     ------------------------------------------------------------------------*/
    if ( DS_ISDATA_SO15(tc_state_ptr->so) )
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Calling RLP reset for SO 15" );
      rlp_reset();
    } 
  }
} /* ds707_pkti_cleanup_rlp() */


/*===========================================================================
FUNCTION      DS707_PKTI_CLEANUP_WMKS

DESCRIPTION   Called when phys link goes down.  Cleans up watermarks and
              queues.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkti_cleanup_wmks
(
  void
)
{
  ds707_tc_state_type       *tc_state_ptr;
  ds707_flow_type           *flow_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(IS_IN_DS_TASK());

  tc_state_ptr    = ds707_get_pri_tc_ptr();
  /*-------------------------------------------------------------------------
    The Macro returns first of the multiple Default DS flows. Using it is 
    fine here as all the tx_wm_list of such DS flows points to the same 
    Watermark. 
  -------------------------------------------------------------------------*/
  flow_ptr        = DS707_PRI_DS_FLOW_PTR(tc_state_ptr);

  /*lint -save -e774 Reason for suppressing error 774*/
  if (tc_state_ptr != NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Flushing primary RX watermark list");
    /* before flushing, update the rx count info */
    ds707_wmk_update_rx_count(tc_state_ptr->fwd_rlp.post_rx_wm_list);
    ds707_wmk_cleanup_single_wmk(tc_state_ptr->fwd_rlp.post_rx_wm_list);
    ds707_clear_1x_rateset();
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "tc_state_ptr is NULL in ds707_pkti_cleanup_wmks");
  }
  /*lint -restore Restore lint error 774*/

  if (flow_ptr != NULL)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Flushing all TX watermark lists");
    /* before flushing, update the tx count info */
    ds707_wmk_update_tx_count(flow_ptr->tx_wm_list);
    ds707_wmk_cleanup_single_wmk(flow_ptr->tx_wm_list);
    ds707_wmk_cleanup_sec_wmks();
    ds707_clear_1x_rateset();
  }
} /* d7s07_pkti_cleanup_wmks() */

/*===========================================================================
FUNCTION      DS707_PKTI_PHYS_LINK_CLEANUP

DESCRIPTION   Called when the IFACE phys_link goes down.  This cleans up
              RLP, empties the watermarks, sets the callbacks for dormancy.

              Third parameter can_go_dormant indicates only if it is okay
              for call to go dormant.  Call may still not go dormant, if
              QNC or IFACE not up.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_phys_link_cleanup
(
  uint32                     pkt_inst,
  boolean                    can_go_dormant,
  ds_cmd_type               *cmd_ptr
)
{
  ds707_tc_state_type         *tc_state_ptr;
  ps_phys_link_type           *phys_link_ptr;
  boolean                     *ppp_resync_ptr;
  ds707_cback_event_info_type  event_info;
  ds707_cm_call_end_info_type  call_end_info;
  uint32                       cta_val = 0;
  ds3g_call_info_cmd_type     *call_info_cmd_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,
                                          DS707_NON_ARRAY);
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  ASSERT(IS_IN_DS_TASK());

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "DS707 PKT phys link down cleanup");

  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  tc_state_ptr   = ds707_get_pri_tc_ptr();

  /*-----------------------------------------------------------------------
    Update the last data sess network to what the network was just prior
    to cleaning up the phys link. It will be updated to NO_SRV while the
    call goes NULL. The update to NULL happens while processing the 
    PHYS_LINK_DOWN or ABORT_SESSION Event
  -----------------------------------------------------------------------*/
  ds707_pkt_set_last_data_sess_network(ds707_pkt_get_current_data_sess_network());

  event_info.so           = tc_state_ptr->so;
  event_info.pkt_inst     = pkt_inst;
  event_info.curr_network = ds707_pkt_get_current_data_sess_network();

  if (cmd_ptr != NULL)
  {
    if( cmd_ptr->cmd_payload_ptr == NULL )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "NULL cmd payload ptr" );
      return;
    }

    call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

    /*-----------------------------------------------------------------------
      Convert the CM end_status value to PS net down reason and assign it to
      the global end reason. 
    -----------------------------------------------------------------------*/
    ds707_pkt_mgr_set_last_call_end_reason(call_info_cmd_ptr->end_status,
                                           call_info_cmd_ptr->call_cmd_err);

    /*-----------------------------------------------------------------------
     Populate the call_end_info with the Highest Failure Reason and the 
     call end status obtained from CM. This info is used by EPZID module
    -----------------------------------------------------------------------*/
    call_end_info.highest_call_fail_reason = 
            call_info_cmd_ptr->mode_info.info.cdma_call.orig_fail_layer;
    call_end_info.end_status = call_info_cmd_ptr->end_status;

    event_info.call_end_info = call_end_info;

    /* populate the call orig failure reason for use by phys_link_down
       hdlr to disable flow only for voice call in progress */
    event_info.failed_orig  = call_info_cmd_ptr->failed_orig;
    event_info.call_cmd_err = call_info_cmd_ptr->call_cmd_err;
  }
  else
  {
    event_info.failed_orig = FALSE;
    event_info.call_cmd_err = CM_CALL_CMD_ERR_NOERR;
  }
  /*-------------------------------------------------------------------------
    Disable flow, as below a lot of watermarks are cleaned up, and various
    re-orig signals may be set.  Since dispersed over a bunch of functions,
    do the overall flow control here.  Re-enable below.
  -------------------------------------------------------------------------*/
  ds707_phys_link_disable_flow(DS_FLOW_707_LOCK_MASK);

  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  tc_state_ptr   = ds707_get_pri_tc_ptr();
  ds707_pkti_cleanup_rlp();
  ds707_pkti_cleanup_wmks();

  /*-------------------------------------------------------------------------
    If call is going dormant, set watermark callback to initiate leaving
    dormancy.  See if holddown timer is required.
  -------------------------------------------------------------------------*/
  if ((ds707_pkti_can_go_dormant(tc_state_ptr,phys_link_ptr) == TRUE) &&
      (can_go_dormant == TRUE)
     )
  {
    event_info.going_dormant = TRUE;
  }
  else
  {
    event_info.going_dormant = FALSE;
  }

  /*-------------------------------------------------------------------------
    Do not enable the phys link flow right away. Do this only after re-
    registering for re-orig handler. This done in the processing of 
    DS707_CBACK_PHYS_LINK_DOWN_EV.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Perform a little housekeeping.
  -------------------------------------------------------------------------*/
  ds707_cback_post_event(DS707_CBACK_PHYS_LINK_DOWN_EV,
                        &event_info);

  tc_state_ptr->call_id = CM_CALL_ID_INVALID;
  tc_state_ptr->so      = DS707_INVALID_SO;

  ppp_resync_ptr  = ds707_get_ppp_resync_flag_ptr();
/*  *ppp_resync_ptr = FALSE;
*/
  DATA_IS707_MSG1( MSG_LEGACY_HIGH, "current PPP resync flag is %d", 
                   *ppp_resync_ptr);

  /*-------------------------------------------------------------------------
    Restore the original CTA timer value, if the aln failed restore CTA 
    timer value is not equal to 1 second (default value). Set the aln failed
    restore CTA timer value to 1 second
  -------------------------------------------------------------------------*/
  if (ds707_pkti_failed_aln_cta_restore_val != 1 )
  {
    cta_val = ds707_pkti_failed_aln_cta_restore_val;
    (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CTA_VAL,(void *)&cta_val,0);
    
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, "Restored CTA timer after ALN to %d", 
                     cta_val);
    ds707_pkti_failed_aln_cta_restore_val = 1;
  }

} /* ds707_pkti_phys_link_down_cleanup() */



/*===========================================================================
                           3G DSMGR HANDLERS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKTI_CALL_ID_HDLR

DESCRIPTION   Called after call originates.  Gives the CM call id to this
              mode specific handler.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_pkti_call_id_hdlr
(
  cm_call_id_type          cm_call_id,
  void                    *mode_spec_info_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  ds707_tc_state_type      *tc_state_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(mode_spec_info_ptr);

  tc_state_ptr = ds707_get_pri_tc_ptr();
  tc_state_ptr->call_id = cm_call_id;
} /* ds707_pkti_call_id_hdlr() */

/*===========================================================================
FUNCTION      DS707_CONFIGURE_JCDMA_CALL_ORIG_PARAMS

DESCRIPTION   Called to request JCDMA call orig params prior to originating
              the data call

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_configure_jcdma_call_orig_params
(
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr
)
{
  /*----------------------------------------------------------------------
    Set the APP rate for JCDMA
  ----------------------------------------------------------------------*/
  if(!ds707_roaming_is_simplified_jcdma_mode())
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, "Setting JCDMA apprate as %d",
                    data_sess_profile_info_ptr->data_rate);
    if (data_sess_profile_info_ptr->data_rate == DS707_DATARATE_HIGH) 
    {
      ds707_jcdma_apprate_set_rate(DS707_JCDMA_APPRATE_HIGH);
    }

    if (data_sess_profile_info_ptr->data_rate == DS707_DATARATE_MED) 
    {
      ds707_jcdma_apprate_set_rate(DS707_JCDMA_APPRATE_MED);
    }

    if (data_sess_profile_info_ptr->data_rate == DS707_DATARATE_LOW) 
    {
      ds707_jcdma_apprate_set_rate(DS707_JCDMA_APPRATE_LOW);
    }
  }

  /*----------------------------------------------------------------------
    Set PPP session close timer value
  ----------------------------------------------------------------------*/
  if ( 
       ds707_gonulltmr_set_timer(
         DS707_GONULLTMRI_DO,
         (int16)(data_sess_profile_info_ptr->ppp_session_close_timer_DO))
       < 0
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not set PPP session close timer DO value.");
  }

  if (
       ds707_gonulltmr_set_timer(
         DS707_GONULLTMRI_1X,
         (int16)(data_sess_profile_info_ptr->ppp_session_close_timer_1X))
      < 0
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not set PPP session close timer 1X value.");
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "PPP session close timer: "
                  "DO value set to %d, 1X value set to %d",
                  data_sess_profile_info_ptr->ppp_session_close_timer_DO,
                  data_sess_profile_info_ptr->ppp_session_close_timer_1X);

}/*ds707_configure_jcdma_call_orig_params*/

/*===========================================================================
FUNCTION     DS707PKTI__SET_IS_BSR_COMPLETED

DESCRIPTION
  Sets the bsr completed flag to TRUE, it blocks all further originations to CM.

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
void ds707_pkti_set_is_bsr_completed
(
  boolean value
)
{
  ds707_is_bsr_completed = value;
} /* ds707_pkti_set_is_bsr_completed() */

/*===========================================================================
FUNCTION     DS707_PKTI_GET_IS_BSR_COMPLETED

DESCRIPTION
  Gets the bsr completed flag value, it blocks all further originations to CM.

PARAMETERS
  None

RETURN VALUE
  bsr_completed flag status
===========================================================================*/
boolean ds707_pkti_get_is_bsr_completed
(
  void
)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED, "get bsr completed = %d", ds707_is_bsr_completed);
  return ds707_is_bsr_completed;
} /* ds707_pkti_get_is_bsr_completed() */

/*===========================================================================
FUNCTION      DS707_PKTI_ORIG_HDLR

DESCRIPTION   Called from 3G DSMGR when originating a packet data call.

DEPENDENCIES  Watermarks to RLP already set up correctly.  Passed in ptrs are
              not NULL.

RETURN VALUE  DS3G_SUCCESS - call can be made.
              DS3G_FAILURE - call can't be made.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkti_orig_hdlr
(
  const byte                     *dial_string,
  uint8                          dial_string_len,
  cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
  boolean                        *cdma_params_changed,
  cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
  boolean                        *gw_cs_params_changed,
  cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
  boolean                        *gw_ps_params_changed,
  cm_srv_type_e_type             *cm_srv_type,
  void                           *mode_spec_info_ptr,
  sys_modem_as_id_e_type          subs_id
)
{
  ds707_so_type                        orig_so;        /* SO to orig with  */
  ds3g_pkt_call_info_type             *orig_info_ptr; /* ptr to orig info */
  ds707_tc_state_type                  *tc_state_ptr;
  ps_phys_link_type                    *phys_link_ptr;
  ds707_call_event_payload_type        ds707_event_payload;
  /* data session profile to be used to originate the packet call */
  ds707_data_session_profile_id        orig_data_sess_profile_id;
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr;
  ds707_mip_profile_type               *ds_1x_runtime_profile_ptr = NULL;
  ps_iface_type *oldest_active_iface_ptr = NULL; 
  uint8                                 qcmdr_val = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* macros to suppress lint warnings */
  DATA_MDM_ARG_NOT_CONST(gw_cs_orig_params_ptr);
  DATA_MDM_ARG_NOT_CONST(gw_cs_params_changed);
  DATA_MDM_ARG_NOT_CONST(gw_ps_orig_params_ptr);
  DATA_MDM_ARG_NOT_CONST(gw_ps_params_changed);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
  FR 18742 : Here we check for bsr completed flag in DS task context. 
  There are two possibilities of flag being modified just before we check here
  1. when the flag is set : as part of CM call end event reason in DS task context,
   it might be outdated already, but we go ahead and originate the phys link. If this
   ever happens, we need to move the logic of setting the flag from call ended handler
   in DS task context to CM task context in ds3gi_cm_call_cmd_cb
  2. When the flag is reset , as part of ss event callback function in CM task context.
     It should be okay, because we might allow the call origination in 3GPP2 system
     but CM would deny it eventually due to wrong preferred RAT.
  --------------------------------------------------------------------------*/
  if ( ds707_pkti_get_is_bsr_completed() )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                    "Deny phys link bring up , wait for SS event..");          
    return DS3G_FAILURE;
  }

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_MDR_VAL,(void *)&qcmdr_val,
                                          DS707_NON_ARRAY);
  DATA_IS707_MSG0( MSG_LEGACY_HIGH, "DS707 PKT ORIG CALL HDLR");

  orig_info_ptr = (ds3g_pkt_call_info_type *)mode_spec_info_ptr;

  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  tc_state_ptr   = ds707_get_pri_tc_ptr();

  /*------------------------------------------------------------------------
    Get the oldest_active_iface_ptr and use it to get the current or 
    requesting data session profile id
  ------------------------------------------------------------------------*/
  oldest_active_iface_ptr = ds707_data_session_get_oldest_active_iface_ptr();
  if (oldest_active_iface_ptr != NULL) 
  {
  /*----------------------------------------------------------------------
    If current data session profile id is valid then use that, otherwise 
    use the requesting data session profile id. When coming out of 
    Dormancy we want to use current data session profile id. Thus this
    check is added.
  ----------------------------------------------------------------------*/
    if( FALSE == ds707_data_session_get_iface_current_profile(
                                                oldest_active_iface_ptr,
                                                &data_sess_profile_info_ptr) )
    {
      ds707_data_session_get_iface_requesting_profile(
                                                oldest_active_iface_ptr,
                                                &data_sess_profile_info_ptr);
    }

    if( data_sess_profile_info_ptr == NULL )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,
                       "Cant orig - Invalid Orig_data_ses_prof_id");
      return(DS3G_FAILURE);
    }

    orig_data_sess_profile_id =
      data_sess_profile_info_ptr->data_session_profile_id;
  }/* if (oldest_active_iface_ptr != NULL) */
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Invalid oldest_active_iface_ptr while originating"
                     "packet call. Proceed using DEFAULT data session "
                     "profile.");
    orig_data_sess_profile_id = DATA_SESSION_DEFAULT_PROFILE;

    /*----------------------------------------------------------------------
      call get profile
    -----------------------------------------------------------------------*/
    ds707_data_session_get_profile(
      orig_data_sess_profile_id,
      (ds_profile_subs_id_enum_type)ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
      &data_sess_profile_info_ptr);
  }

  /*simplified JCDMA: 
    5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
  if(ds707_roaming_is_curr_mode_jcdma()||
     ds707_roaming_is_simplified_jcdma_mode())
  {
    ds707_configure_jcdma_call_orig_params(data_sess_profile_info_ptr);    
  }

 orig_so = ds707_pkti_get_orig_so();

  if (orig_so == DS707_INVALID_SO)
  {
    DATA_ERR_FATAL("Bad mobile settings - no SO to orig with");
    return(DS3G_FAILURE);
  }

  cdma_orig_params_ptr->srv_opt           = (uint16)orig_so;
  cdma_orig_params_ptr->activate_code     = CM_OTASP_ACT_CODE_NONE;
  cdma_orig_params_ptr->drs_bit           = orig_info_ptr->drs_bit;
  cdma_orig_params_ptr->sr_id_included    = FALSE;
  cdma_orig_params_ptr->qos_parms_incl    = FALSE;
  cdma_orig_params_ptr->last_act_data_net = ds707_pkt_get_current_data_sess_network();
  cdma_orig_params_ptr->is_special_srv_data_call = FALSE;

  /*-----------------------------------------------------------------------
    Updating the ds707_current_data_sess_network only if the new network is  
    CDMA. This covers the scenario of handoff from DO-1x with no PPP ReSync. 
    These handoffs typically have an orig with DRS=0
  -----------------------------------------------------------------------*/
  if (orig_info_ptr->new_network == SYS_SYS_MODE_CDMA)
  {
    ds707_pkt_mgr_update_cdsn(orig_info_ptr->new_network);
    ds707_extif_inform_hdr_sess_status();
    DATA_IS707_MSG1( MSG_LEGACY_MED, "Updated CDSN to %d",
                     ds707_pkt_get_current_data_sess_network());
  }
#ifndef FEATURE_DS_LN_UPDATE
  else if (orig_info_ptr->new_network == SYS_SYS_MODE_HDR)
  {
    /*-----------------------------------------------------------------------
    Update ds707_current_data_sess_network. If call is originated because
    of Idle Digital Mode change, the new_network in the mode_spec_info_ptr 
    will be set to the new system (1X or HDR). In all other cases, it MUST
    be set to ds707_current_data_sess_network at the time the phys_link_up
    command is posted to DS so that ds707_current_data_sess_network is left 
    unchanged after executing the statement below.
    If feature FEATURE_DS_LN_UPDATE is defined, then the 
    ds707_current_data_sess_network is updated after receiving a 
    confirmation from the AN.
    -----------------------------------------------------------------------*/
    ds707_pkt_mgr_update_cdsn(orig_info_ptr->new_network);
    ds707_extif_inform_hdr_sess_status();
    DATA_IS707_MSG1( MSG_LEGACY_MED, "Updated CDSN to %d",
                     ds707_pkt_get_current_data_sess_network() );
  }
#endif /* !FEATURE_DS_LN_UPDATE*/
  /*-----------------------------------------------------------------------
    Originate with service type set to Automatic. This means that if it is
    possible to originate the Packet Data call on an HDR system, that will
    be attempted. In 1X only systems, the CM will originate the call on a
    CDMA system.
  -----------------------------------------------------------------------*/

#ifdef FEATURE_HDR
  /*------------------------------------------------------------------------
    If the data session profile that is trying to bring up the phys link is
    the DEFAULT profile or if it is JCDMA INET profile then originate with
    service type set to Automatic. Otherwise use the service option  (data
    mode option) provided in the apps profile structure.
  ------------------------------------------------------------------------*/
#ifdef FEATURE_EHRPD
  /*-----------------------------------------------------------------------
    Override cm_srv_type if this is a 1X special service call
  -----------------------------------------------------------------------*/
  if (DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG == ds707_query_sys_chg_sm_state())
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "1X Special Call In progress."
                       "Forcing CDMA Specific special srv data call ");
    *cm_srv_type = CM_SRV_TYPE_CDMA_SPECIFIC;
    cdma_orig_params_ptr->is_special_srv_data_call = TRUE;
  }
  else if ( ((ds707_sys_chg_is_in_ehsr_state() == TRUE) && 
             (ds707_pkt_is_ehrpd_mode_of_operation() == TRUE)) ||
             (ds707_pkt_mgr_is_data_on_ehrpd()) )  
  {
    /* Even if ehrpd session is lost example as part of e->L, and we are doing IRAT,
       for an active data call existing on eHRPD, we should orig the data call with HDR
       more preferred if coming out of dormany, othrwise if we set to automatic and 
       1x call connection could lead to non-epc event posting which inturn tears down 
       data call on eHRPD and thus  it causes IRAT failure.
       In all other cases, originate with 1x system.
    */       
    /*------------------------------------------------------------------------
      originate with a special service type HDR_PREFERRED if DS is in
      EHSR state
    ------------------------------------------------------------------------*/
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "Originate with service type set to HDR Preferred");
    *cm_srv_type = CM_SRV_TYPE_HDR_MORE_PREF;
    
    /*------------------------------------------------------------------------
      stop the new call attempt timer if it is running.
    ------------------------------------------------------------------------*/
    ds707_sys_chg_sr_process_call_orig();
  }
  else
#endif /* FEATURE_EHRPD */

  if ( (orig_data_sess_profile_id == DATA_SESSION_DEFAULT_PROFILE) ||
       (orig_data_sess_profile_id == DATA_SESSION_JCDMA_INET_PROFILE) )
  {
    if ( (ds707_roaming_is_curr_mode_jcdma() == TRUE) && 
         (orig_info_ptr->force_cdma_only == TRUE))
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Forcing CDMA Specific data call");
      *cm_srv_type = CM_SRV_TYPE_CDMA_SPECIFIC;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Originate with service type set to Automatic");
      *cm_srv_type = CM_SRV_TYPE_CDMA_HDR;
    }
  } /* DEFAULT PROFILE OR JCDMA INET PROFILE */
  else 
  {
    DATA_IS707_MSG1( MSG_LEGACY_MED, 
                     "Originate with service type set to %d",
                     data_sess_profile_info_ptr->data_mode);

    if (data_sess_profile_info_ptr->data_mode == DS707_DATAMODE_CDMA_ONLY) 
    {
      *cm_srv_type = CM_SRV_TYPE_CDMA_SPECIFIC;
    }
    else if (data_sess_profile_info_ptr->data_mode == DS707_DATAMODE_HDR_ONLY)
    {
      *cm_srv_type = CM_SRV_TYPE_HDR;
    }
    else if (data_sess_profile_info_ptr->data_mode == DS707_DATAMODE_CDMA_HDR) 
    {
      *cm_srv_type = CM_SRV_TYPE_CDMA_HDR;
    }
    else
    {
      /* service type set to Automatic */
      *cm_srv_type = CM_SRV_TYPE_CDMA_HDR;
    }
  }
#else
  *cm_srv_type                         = CM_SRV_TYPE_CDMA_SPECIFIC;
#endif /* FEATURE_HDR */

#ifdef FEATURE_IS2000_REL_A_SVD
  if (tc_state_ptr->alloc_sr_id != MCCSRID_INVALID_SRID)
  {
    /*-----------------------------------------------------------------------
      SR_ID was reserved at power up time. So make the SR_ID flag TRUE so
      that MC will use the allocated SR ID.
    -----------------------------------------------------------------------*/
    cdma_orig_params_ptr->sr_id_included = TRUE;
    cdma_orig_params_ptr->sr_id          = tc_state_ptr->alloc_sr_id;
  }
#endif /* FEATURE_IS2000_REL_A_SVD */


  if (orig_so == CAI_SO_PPP_PKT_DATA_3G)
  {
#ifdef FEATURE_IS2000_REL_A
    ds707_so33_orig_qos(cdma_orig_params_ptr);
#endif /* FEATURE_IS2000_REL_A */
  }

  if (ds707_so_retrydel_is_delayed() == TRUE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Pkt data calls are delayed");
    if(ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "SO retry delay set, current mode = HDR, "
                       "orig with srv type set to HDR Preferred");
      *cm_srv_type = CM_SRV_TYPE_HDR_MORE_PREF;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "SO retry delay set - do not send orig to CM");
      return(DS3G_FAILURE);
    }
  }
  if(((DS_FLOW_CTRL_IS_BIT_SET(ps_phys_link_get_flow_mask(phys_link_ptr),
                                     DS_HOLDDOWN_TIMER_MASK)) == TRUE)    &&
     (orig_info_ptr->use_hdown_timer == TRUE))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Cant orig - pkt HOLDDOWN TIMER");
    return(DS3G_FAILURE);
  }
  else
  {
    *cdma_params_changed = TRUE;

    /*-------------------------------------------------------------------------
    Send origination event to DIAG. Call id is unassigned until call
    gets connected. Event is sent only when CM is notified of cdma params 
    changes.
    -------------------------------------------------------------------------*/
    ds707_event_payload.ds707_event  = EVENT_DS707_PKT_ORIG;
    ds707_event_payload.so_type = orig_so;
    ds707_event_payload.call_id = CM_CALL_ID_UNASSIGNED;
    event_report_payload( EVENT_PKT_DS707,
                          sizeof (ds707_event_payload),
                          &ds707_event_payload );

    ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

    DATA_IS707_MSG6( MSG_LEGACY_HIGH, 
               "Orig PKT call: SO %d, MDR %d, QNC %d, QCSO %d, DRS %d, CRM %d",
               orig_so,
               qcmdr_val,
               ds_1x_runtime_profile_ptr->ds707.ds707_qcqnc_val,
               ds_1x_runtime_profile_ptr->ds707.ds707_qcso_val,
               orig_info_ptr->drs_bit,
               ds_1x_runtime_profile_ptr->ds707.ds707_crm_val);

    DATA_IS707_MSG4( MSG_LEGACY_HIGH, 
                     "LADN %d, LDSN %d, CDSN %d, EPZID %d",
                     cdma_orig_params_ptr->last_act_data_net,
                     ds707_pkt_get_last_data_sess_network(),
                     ds707_pkt_get_current_data_sess_network(),
                     orig_info_ptr->epzid);

    if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
    {
      /*---------------------------------------------------------------------
        Clear ds707_mode_handler_info global structure because there should 
        be no pending flag set for current origination attempt from a 
        previous failed originitation.
  
        This currently applies only for laptop calls as part of JCDMA JATE
        requirements.
      ---------------------------------------------------------------------*/
      ds707_pkt_mgr_clear_last_orig_fail_reason();
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Setting Last orig fail reason to None (Reset)"
                       " - PKT_MGR_ORIG_HDLR");
    }
    ds707_pkt_mgr_clear_last_call_end_reason();
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "Setting Last end call reason to Client End (Reset)"
                     " - PKT_MGR_ORIG_HDLR");
    return(DS3G_SUCCESS);
  }
} /* ds707_pkti_orig_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_INC_CALL_HDLR

DESCRIPTION   Called when CM receives a page from BS.  Only time we accept
              a call is when we are dormant.

DEPENDENCIES  None.

RETURN VALUE  Will either REJECT the call or tell CM to answer the call.

SIDE EFFECTS  None
===========================================================================*/
LOCAL ds3g_incom_e_type ds707_pkti_inc_call_hdlr
(
  ds_cmd_type            *cmd_ptr,
  cm_call_id_type         cm_call_id,
  cm_ans_params_s_type   *ans_params,  /* Answer params sent to Call Mgr    */
  cm_end_params_s_type   *end_params,  /* end params if call rejected       */
  sys_modem_as_id_e_type  subs_id
)
{
  ds707_tc_state_type           *tc_state_ptr;
  ps_phys_link_type             *phys_link_ptr;
  ds3g_incom_e_type              ret_val = DS3G_ANSWER_CALL;
  uint16                         inc_srv_opt;
  boolean                        accept_call = TRUE;  /*JCDMA may change this*/
  ds707_call_event_payload_type  ds707_event_payload;
  ds707_cback_event_info_type    event_info;
  boolean                        reject_call = FALSE;
  ds3g_call_info_cmd_type       *call_info_cmd_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "NULL pointer passed" );
    return DS3G_IGNORE;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  tc_state_ptr  = ds707_get_pri_tc_ptr();
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  inc_srv_opt = call_info_cmd_ptr->mode_info.info.cdma_call.srv_opt;

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "DS707 Pkt inc call hdlr with call id %d so %d",
                   cm_call_id, inc_srv_opt);

  /*-------------------------------------------------------------------------
  Send incoming call event to DIAG.
  -------------------------------------------------------------------------*/
  ds707_event_payload.ds707_event  = EVENT_DS707_PKT_INCOMING_CALL;
  ds707_event_payload.so_type = inc_srv_opt;
  ds707_event_payload.call_id = cm_call_id;
  event_report_payload( EVENT_PKT_DS707,
                        sizeof (ds707_event_payload),
                        &ds707_event_payload );
#ifdef FEATURE_HDR
  /*-----------------------------------------------------------------------
    If the packet data session is dormant AND 
      1. The incoming page service option is not HDR  AND
      2. current data session is on HDR
    Reject the incoming page 
  -----------------------------------------------------------------------*/
  if ((ds707_pkt_is_dormant(phys_link_ptr) == TRUE) && 
      (!(DS_ISDATA_HDR(inc_srv_opt))))
  {
    if ( ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR )
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                      "Reject incoming so %d page, pkt session dormant on HDR",
                      inc_srv_opt);
      end_params->call_id = cm_call_id;
      end_params->info_type = CM_CALL_MODE_INFO_CDMA;
      end_params->end_params.cdma_end.end_reason_included = TRUE;
      end_params->end_params.cdma_end.end_reason = CAI_REL_NORMAL;

      ret_val = DS3G_REJECT_CALL;
      return ret_val;
    }
  }
#endif /* HDR */


  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {

#ifdef FEATURE_HDR
    if ( DS_ISDATA_HDR(inc_srv_opt) )
    {
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "no check for jcdma inc pkt so %d", inc_srv_opt);
    }
    else
#endif /* HDR */
    {
      accept_call = ds707_so_jcdma_verify_inc_pkt_so(inc_srv_opt);
    }
  }

  if (ds707_pkt_is_dormant(phys_link_ptr))
  {
    if (accept_call == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "pkt call is dormant, answer incoming page");
      if (call_info_cmd_ptr->mode_info.info.cdma_call.alert_ans_bypass
          == TRUE)
      {
        /*-------------------------------------------------------------------
          Alert_ans_bypass is set, which means we (DS) cannot answer the
          call.  Tell 3G DSMGR to do nothing.
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "alert ans bypass - do nothing");
        ret_val = DS3G_DO_NOTHING;
      }
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "answer incoming call");
        ans_params->info_type = CM_CALL_MODE_INFO_CDMA;
        ans_params->ans_params.cdma_ans.call_type = CM_CALL_TYPE_PS_DATA;
        ret_val = DS3G_ANSWER_CALL;
      }
      tc_state_ptr->call_id = cm_call_id;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "accept_call=FALSE, reject incoming page");
      reject_call = TRUE;
    }
  }
#ifdef FEATURE_HDR
  else if ( DS_ISDATA_HDR(inc_srv_opt))
  {
    ret_val = DS3G_IGNORE;
  }
#endif /* FEATURE_HDR */
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "pkt call not Dormant, reject incoming page");
    reject_call = TRUE;
  }

  if (reject_call == TRUE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "reject incoming call");
    end_params->call_id = cm_call_id;
    end_params->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params->end_params.cdma_end.end_reason_included = TRUE;

    if((ds707_roaming_is_curr_mode_jcdma() == TRUE) &&
       (ds707_pkt_is_dormant(phys_link_ptr) == TRUE)
      )
    {
      /*---------------------------------------------------------------------
        JCDMA may have said reject dormant call.  In this case, still say
        dormant in ORDQ val
      ---------------------------------------------------------------------*/
      end_params->end_params.cdma_end.end_reason = CAI_REL_NORMAL;
    }
    else
    {
      end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
    }
    ret_val = DS3G_REJECT_CALL;
  }

  /*lint -e644 ret_val is initialized previously */
  if ((ret_val == DS3G_ANSWER_CALL) ||            /* answer inc call */
      (ret_val == DS3G_DO_NOTHING)                /* bypass alert    */
     )
  /*lint -restore*/
  {
    DATA_IS707_MSG0( MSG_LEGACY_MED, 
                     "POST INC_CALL_EV");
    event_info.so       = inc_srv_opt;
    ds707_cback_post_event(DS707_CBACK_INC_CALL_EV,
                          &event_info);
                          
  }
  return(ret_val);
} /* ds707_pkti_inc_call_hdlr() */


#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
/*===========================================================================
FUNCTION      DS707_PKTI_INCOMING_PAGE_CMD_HDLR

DESCRIPTION   Called when CM receives a page from BS. Without updating 
              any call params, this function just returns boolean
              whether DS can accept or will reject the incoming call.

DEPENDENCIES  None.

RETURN VALUE  Will either REJECT the call or tell CM to answer the call.

SIDE EFFECTS  None
===========================================================================*/
LOCAL ds3g_incom_e_type ds707_pkti_incoming_page_cmd_hdlr
(
  ds_cmd_type             *cmd_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  ds707_tc_state_type      *tc_state_ptr;
  ps_phys_link_type        *phys_link_ptr;
  ds3g_incom_e_type         ret_val = DS3G_ANSWER_CALL;
  uint16                    inc_srv_opt;
  boolean                   accept_call = TRUE; /* JCDMA may change this val */
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL pointer passed");
    return DS3G_IGNORE;
  }
  
  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  tc_state_ptr  = ds707_get_pri_tc_ptr();
  if (NULL == tc_state_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "tc_state_ptr is NULL");
    ret_val = DS3G_REJECT_CALL;
    return ret_val;
  }

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  inc_srv_opt = call_info_cmd_ptr->mode_info.info.cdma_call.srv_opt;

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "Primary tc state ptr: %d. "
                  "DS707 Pkt inc page cmd hdlr with so %d",
                  tc_state_ptr, inc_srv_opt);

#ifdef FEATURE_HDR
  /*-----------------------------------------------------------------------
    If the packet data session is dormant AND 
      1. The incoming page service option is not HDR  AND
      2. current data session is on HDR
    Reject the incoming page 
  -----------------------------------------------------------------------*/
  if ((ds707_pkt_is_dormant(phys_link_ptr) == TRUE) && 
      (!(DS_ISDATA_HDR(inc_srv_opt))))
  {
    if ( ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR )
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                      "Reject incoming so %d page, pkt session dormant on HDR",
                      inc_srv_opt);
      ret_val = DS3G_REJECT_CALL;
      return ret_val;
    }
  }
#endif /* HDR */

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
#ifdef FEATURE_HDR
    if ( DS_ISDATA_HDR(inc_srv_opt) )
    {
      DATA_IS707_MSG1( MSG_LEGACY_MED, 
                       "no check for jcdma inc pkt so %d", inc_srv_opt);
    }
    else
#endif /* HDR */
    {
      accept_call = ds707_so_jcdma_verify_inc_pkt_so(inc_srv_opt);
    }
  }

  if (ds707_pkt_is_dormant(phys_link_ptr))
  {
    if (accept_call == TRUE)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "pkt call is dormant, answer incoming page");
      ret_val = DS3G_ANSWER_CALL;
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "accept_call=FALSE, reject incoming page" );
      ret_val = DS3G_REJECT_CALL;
    }
  }
#ifdef FEATURE_HDR
  else if ( DS_ISDATA_HDR(inc_srv_opt))
  {
    ret_val = DS3G_REJECT_CALL;
  }
#endif /* FEATURE_HDR */
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "pkt call not Dormant, reject incoming page");
    ret_val = DS3G_REJECT_CALL;
  }
  return(ret_val);  
}
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

/*===========================================================================
FUNCTION      DS707_PKTI_SWITCH_CALLS_HDLR

DESCRIPTION   Called when CM receives an incoming page for the same type of
              call as what we are originating.

              In this case, we want to end the origination, but accept the
              incoming page.  We must then switch our call_id to the
              incoming call id.

DEPENDENCIES  None.

RETURN VALUE  Always tell 3G DSMGR to accept incoming call.

SIDE EFFECTS  None
===========================================================================*/
LOCAL ds3g_incom_e_type ds707_pkti_switch_calls_hdlr
(
  ds_cmd_type            *cmd_ptr,
  cm_call_id_type         inc_call_id,
  cm_call_id_type         exist_call_id,
  cm_ans_params_s_type   *ans_params,      /* ans parms for inc call        */
  cm_end_params_s_type   *end_params_orig, /* end parms for orig call       */
  cm_end_params_s_type   *end_params_inc,   /* end parms for inc call        */
  sys_modem_as_id_e_type  subs_id
)
{
  ps_phys_link_type       *phys_link_ptr = NULL;
  ds707_tc_state_type     *tc_state_ptr  = NULL;
  ds3g_incom_e_type        ret_val;
  uint32                   pkt_inst;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "NULL pointer passed" );
    return DS3G_IGNORE;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "DS707 switch call hdlr inc cid %d exist cid %d",
                   inc_call_id, 
                   exist_call_id);

  pkt_inst = ds707_pkti_get_pkt_state_index_cid(exist_call_id);

  /* we will never get pkt_inst above or equal to MAX_PDN_INSTANCES, but 
     just this check to return do nothing if we get MAX_PDN_INSTANCES 
     as pkt_inst */
     
  if( pkt_inst >= DS707_MAX_PDN_INSTANCES )
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                     "invalid pkt_call_instance :%d, shouldn't "
                     "access ds707_pkt_state array with this index",
                     pkt_inst);
    return(DS3G_IGNORE);
  }

  tc_state_ptr  = ds707_get_pri_tc_ptr();
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

#ifdef FEATURE_HDR
  if ( DS_ISDATA_HDR( call_info_cmd_ptr->mode_info.info.cdma_call.srv_opt ))
  {
    /*---------------------------------------------------------------------
      We have a call in "coming-up state", which means origination.  Not
      necessary to be dormant.  We are already accepting incoming call,
      so need to end the orig'ing call.  Set end_params so that old call is
      not kept dormant in BS.

      Also, switch state_ptr to use the call_id of the incoming call.
      
      Note that we set the end reason to be CAI_REL_ABORT_CALL. This is 
      handled by the  HDR protocols such that it will get rid of the outgoing
      call id but will not bring down the traffic channel if the incoming 
      call is a  HDR call - whereas any other reason would have resulted in
      both incoming and otugoing HDR calls being ended and traffic channel 
      brought down.
      
      The 1X call processing handles the abort call reason as a actual
      end call with reason CAI_REL_SRV_INACT.
     ---------------------------------------------------------------------*/

    /*lint -save -e774 Reason for suppressing error 774*/
    if ((tc_state_ptr != NULL)                                &&
        (phys_link_ptr != NULL)                               &&
        ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP) ||
         (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING) 
        )
       )
    {
      if (call_info_cmd_ptr->mode_info.info.cdma_call.alert_ans_bypass
          == TRUE)
      {
        /*-------------------------------------------------------------------
          Alert_ans_bypass is set, which means we (DS) cannot answer the
          call.
        -------------------------------------------------------------------*/
        ret_val = DS3G_DO_NOTHING;
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "ans_bypass Do nothing inc call, end orig");
      }
      else
      {
        /*-------------------------------------------------------------------
          No bypass alert, so okay to accept incoming call.
        -------------------------------------------------------------------*/
        ans_params->ans_params.cdma_ans.call_type = CM_CALL_TYPE_PS_DATA;
        ans_params->info_type = CM_CALL_MODE_INFO_CDMA;
        ret_val = DS3G_ANSWER_CALL;
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "ans inc call, end orig call");
      }
      tc_state_ptr->call_id = inc_call_id;
    }
    else
    {
      /*---------------------------------------------------------------------
        There is no dormant data session. So ds3gmgr should just ignore the
        incoming Page. It may be for other HDR modules.
      ---------------------------------------------------------------------*/
      ret_val = DS3G_IGNORE;
    }
    /*lint -restore Restore lint error 774*/


    end_params_orig->call_id   = exist_call_id;
    end_params_orig->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params_orig->end_params.cdma_end.end_reason_included = TRUE;
    end_params_orig->end_params.cdma_end.end_reason = CAI_REL_ABORT_CALL;
    return(ret_val);
  }
  else
#endif /* FEATURE_HDR */
  {
    /*lint -save -e774 Reason for suppressing error 774*/

    if ((tc_state_ptr != NULL)                                &&
        (phys_link_ptr != NULL)                               &&
        ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP)||
         (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING) 
        )
       )
    {
      if (call_info_cmd_ptr->mode_info.info.cdma_call.alert_ans_bypass
          == TRUE)
      {
        /*-------------------------------------------------------------------
          Alert_ans_bypass is set, which means we (DS) cannot answer the
          call.
        -------------------------------------------------------------------*/
        ret_val = DS3G_DO_NOTHING;
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "Do nothing inc call, but end orig call");
      }
      else
      {
        /*-------------------------------------------------------------------
          No bypass alert, so okay to accept incoming call.
        -------------------------------------------------------------------*/
        ans_params->ans_params.cdma_ans.call_type = CM_CALL_TYPE_PS_DATA;
        ans_params->info_type = CM_CALL_MODE_INFO_CDMA;
        ret_val = DS3G_ANSWER_CALL;
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                         "ans inc call, end orig call");
      }
      /*---------------------------------------------------------------------
        We have a call in "coming-up state", which means origination.  Not
        necessary to be dormant.  We are already accepting incoming call,
        so need to end the orig'ing call.  Set end_params so that old call is
        not kept dormant in BS.

        Also, switch state_ptr to use the call_id of the incoming call.
      ---------------------------------------------------------------------*/
      tc_state_ptr->call_id = inc_call_id;

      end_params_orig->call_id   = exist_call_id;
      end_params_orig->info_type = CM_CALL_MODE_INFO_CDMA;
      end_params_orig->end_params.cdma_end.end_reason_included = TRUE;
      end_params_orig->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
      return(ret_val);
    }
    else
    {
      /*---------------------------------------------------------------------
        We have no old call with the call id shown.  Get rid of both calls.
      ---------------------------------------------------------------------*/
      end_params_orig->call_id   = exist_call_id;
      end_params_orig->info_type = CM_CALL_MODE_INFO_CDMA;
      end_params_orig->end_params.cdma_end.end_reason_included = TRUE;
      end_params_orig->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;

      end_params_inc->call_id   = inc_call_id;
      end_params_inc->info_type = CM_CALL_MODE_INFO_CDMA;
      end_params_inc->end_params.cdma_end.end_reason_included = TRUE;
      end_params_inc->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "reject inc call");
      return(DS3G_REJECT_CALL);
    }
    /*lint -restore Restore lint error 774*/

  }
} /* ds707_pkti_switch_calls_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_FORCE_CALL_DOWN_HDLR

DESCRIPTION   Called by 3G DSMGR when a call must be forced down, but need
              end_params filled out.

DEPENDENCIES  Called in DS TASK context.  Is called autonomously.

RETURN VALUE  TRUE. there was a call to tear down.  FALSE - no call to tear
              down.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkti_force_call_down_hdlr
(
  cm_call_id_type         call_id,
  cm_end_params_s_type   *end_params_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  uint32                       pkt_inst;
  ps_phys_link_type           *phys_link_ptr;
  ds707_tc_state_type         *tc_state_ptr;
  ds3g_pkt_call_info_type     mode_spec_info;
  boolean                      delay_disconnect;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "ds707 pkt FORCE CALL Down call id %d",
                   call_id);

  pkt_inst = ds707_pkti_get_pkt_state_index_cid(call_id);

  if (pkt_inst < DS707_MAX_PDN_INSTANCES)
  {
    phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
    tc_state_ptr  = ds707_get_pri_tc_ptr();

    mode_spec_info.pkt_instance = pkt_inst;
    ds707_pkti_end_call_hdlr( call_id,
                              end_params_ptr,
                              &delay_disconnect,
                              &mode_spec_info,
                              subs_id);

    if (ds707_pkti_can_go_dormant( tc_state_ptr, 
                                   phys_link_ptr) == FALSE)
    {
      ds707_pkti_phys_link_cleanup( pkt_inst,
                                    CANNOT_GO_DORMANT,
                                    NULL );
    }
    else
    {
      ds707_pkti_phys_link_cleanup( pkt_inst,
                                   CAN_GO_DORMANT,
                                   NULL );
    }
    return(DS3G_SUCCESS);
  }
  else
  {
    return(DS3G_FAILURE);
  }
} /* ds707_pkti_force_call_down_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_ENABLE_HDR_FLOW

DESCRIPTION   Util function to enable HDR flow based on the type of flow

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
STATIC void ds707_pkti_enable_hdr_flow
(
  void
)
{
  if ( hdrcp_stream_stream_is_assigned( HDRHAI_DEF_PACKET_APP_SN ) &&
       (hdrpac_app_flow_is_enabled(HDRHAI_DEF_PACKET_APP_SN) == FALSE)
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Start flow for SN");
    hdrpac_flow_enable( HDRHAI_DEF_PACKET_APP_SN );
  }
#ifdef FEATURE_HDR_QOS
  if ( (hdrcp_stream_stream_is_assigned( HDRHAI_MULTIFLOW_PACKET_APP_SN ) &&
       (hdrpac_app_flow_is_enabled(
                              HDRHAI_MULTIFLOW_PACKET_APP_SN) == FALSE)))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Start flow for MRLP SN");
    hdrpac_flow_enable( HDRHAI_MULTIFLOW_PACKET_APP_SN );
  }
  if ( hdrcp_stream_stream_is_assigned( 
                             HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN ) &&
       (hdrpac_app_flow_is_enabled(
                      HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) == FALSE))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Start flow for ERLP SN");
    hdrpac_flow_enable( HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN );
  }
#ifdef FEATURE_HDR_REVB
  if ( hdrcp_stream_stream_is_assigned( 
                           HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN ) &&
       (hdrpac_app_flow_is_enabled(
                    HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) == FALSE))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Start flow for Multilink MultiflowRLP SN");
    hdrpac_flow_enable( HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN );
  }
#endif /* FEATURE_HDR_REVB */
#endif /* FEATURE_HDR_QOS */
} /* ds707_pkti_enable_hdr_flow */


/*===========================================================================
FUNCTION      DS707_PKTI_CALL_CONN_HDLR

DESCRIPTION   Called by 3G DSMGR when traffic channel connects.

DEPENDENCIES  None.

RETURN VALUE  DS3G_SUCCESS - call connected okay
              DS3G_FAILURE - call failed

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_pkti_call_conn_hdlr
(
  cm_call_id_type            call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  boolean                   *delay_connect,
  cm_end_params_s_type      *end_params,
  sys_modem_as_id_e_type     subs_id
)
{
  boolean                       ret_val = DS3G_FAILURE;
  ps_phys_link_type            *phys_link_ptr = NULL;
  ds707_tc_state_type          *tc_state_ptr  = NULL;
  boolean                      *ppp_resync_ptr;
  boolean                       sanityFlag = FALSE;
  uint32                        pkt_inst, i;
  ds707_cback_event_info_type   event_info;
  ds707_call_event_payload_type ds707_event_payload;
  ds_dsd_event_info_type        dsd_event_info = {0};
  uint32                        cta_val = 0;
  uint32                        pri_phys_link_state = PHYS_LINK_STATE_INVALID;

#ifdef FEATURE_EHRPD
  ds707_sys_chg_sm_e            sys_chg_sm_state = DS707_SYS_CHG_SM_NULL;
#endif /* FEATURE_EHRPD */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "DS707 PKT CALL CONNECTED HDLR call id %d",
                   call_id);

  *delay_connect = FALSE;   /* always FALSE.  For wcdma/gsm only           */  

  pkt_inst = ds707_pkti_get_pkt_state_index_cid(call_id);
  if(pkt_inst >= DS707_MAX_PDN_INSTANCES)
  {
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "invalid pkt_call_instance :%d, shouldn't "
                     "access ds707_pkt_state array with this index ",
                     pkt_inst);
    return(DS3G_FAILURE);  	
  }
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  ds707_gonulltmr_restore_timer_values();
#endif

  tc_state_ptr = ds707_get_pri_tc_ptr();
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  pri_phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Primary phys link state is %d",
                  pri_phys_link_state);

  if ((pri_phys_link_state == PHYS_LINK_COMING_UP)||
      (pri_phys_link_state == PHYS_LINK_RESUMING))
  {
    sanityFlag = TRUE;
  }

#ifdef FEATURE_HDR_QOS
  if( (pri_phys_link_state != PHYS_LINK_GOING_NULL) && 
      (pri_phys_link_state != PHYS_LINK_GOING_DOWN) )
  {
    for (i = 1; i < DS707_NUM_PHYS_LINKS; ++i)
    {
      tc_state_ptr = ds707_get_sec_tc_ptr(i);
      phys_link_ptr = ds707_get_phys_link_ptr(i);
      if (ds707_sec_pkt_is_tc_state_allocated(tc_state_ptr) == FALSE)
      {
        continue;
      }
      if ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP)||
          (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING))
      {
        DATA_IS707_MSG2(MSG_LEGACY_HIGH, "Secondary phys link %d is in state %d",
                        i, PS_PHYS_LINK_GET_STATE(phys_link_ptr));
        sanityFlag = TRUE;
        break;
      }
    }
  }
#endif /* FEATURE_HDR_QOS */
/*lint -save -e774 Reason for suppressing error 774*/
  if (pkt_inst < DS707_MAX_PDN_INSTANCES)
  {
    tc_state_ptr  = ds707_get_pri_tc_ptr();
    phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  }

  if ((tc_state_ptr == NULL) || (phys_link_ptr == NULL))
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "conn call with unknown id %d", call_id);
  }
  else if (sanityFlag == FALSE)
  {
    /*-----------------------------------------------------------------------
      Phone was not dormant (covers case where we get an incoming page) &&
      phone was not orig'ing (covers case where we initiated the call).
      Can't connect.
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "incorrect phys link state:%d for phys_link_ptr:%s",
                    PS_PHYS_LINK_GET_STATE(phys_link_ptr),
                    phys_link_ptr);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Call came up.  RLP is configured either on incoming page or on
      mobile origination.
    -----------------------------------------------------------------------*/
    tc_state_ptr->so = mode_info_ptr->info.cdma_call.srv_opt;
    ds707_pkt_set_prev_call_so(tc_state_ptr->so);

    /*-----------------------------------------------------------------------
    Send call connected event to DIAG. 
    -----------------------------------------------------------------------*/
    ds707_event_payload.ds707_event  = EVENT_DS707_PKT_CALL_CONNECTED;
    ds707_event_payload.so_type = tc_state_ptr->so;
    ds707_event_payload.call_id = call_id;
    event_report_payload( EVENT_PKT_DS707,
                          sizeof (ds707_event_payload),
                          &ds707_event_payload );
    /*-----------------------------------------------------------------------
      Note that the ds707_pkt_prev_sr_id is used only in 1X calls while
      coming out of dormancy to verify that the sr id is the same as what we
      had before we went dormant. For HDR the previous sr id does not make
      any sense. If a HDR call gets connected, then the ds707_pkt_prev_sr_id
      value does not have a valid value.
    -----------------------------------------------------------------------*/
    ds707_pkt_set_prev_sr_id(dsrlp_get_sr_id(DSRLP_FIXED_SR_ID_INDEX)); 
    ret_val = DS3G_SUCCESS;
  }
  /*lint -restore Restore lint error 774*/
  if (ret_val == DS3G_FAILURE)
  {
    /*-----------------------------------------------------------------------
      Hardcode params to kill this call (no dormancy).  Do not have to
      do a phys_link_down_ind() yet, as CM will send a call_cmd_end() when
      TC is gone.
    -----------------------------------------------------------------------*/
    end_params->call_id = call_id;
    end_params->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params->end_params.cdma_end.end_reason_included = TRUE;
    end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
  }
  else
  {
#ifdef FEATURE_IS2000_REL_A
    ds707_so33_process_qos ( mode_info_ptr );
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_EHRPD
    sys_chg_sm_state = ds707_query_sys_chg_sm_state();
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_HDR
    /*-----------------------------------------------------------------------
      If the call connected is an HDR call then start the HDR RLP.
    -----------------------------------------------------------------------*/
    if ( DS_ISDATA_HDR( tc_state_ptr->so ) )
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Call connected on HDR");
      /*-----------------------------------------------------------------------
      Notify DSD
      -----------------------------------------------------------------------*/
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_CALLCONNECTED_EV;
      ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, &dsd_event_info);

      ds707_pkt_mgr_update_cdsn(SYS_SYS_MODE_HDR);
      ds707_pkt_mgr_set_current_sys_mode(SYS_SYS_MODE_HDR);
      ds707_extif_inform_hdr_sess_status();
      /*---------------------------------------------------------------------
        Initialize the HDR SN RLP. Note that if the traffic channel is
        brought up after the call is connected, then the RLP would already
        have been initialized through HDRPAC when the call is connected.
        This case is so that we initialize the HDR RLP if the data call is
        originated while the HDR traffic channel is already connected.
      ---------------------------------------------------------------------*/
      hdrpac_mdm_sn_init();
      /*---------------------------------------------------------------
        Enable data flow for HDR.
      ---------------------------------------------------------------*/
      ds707_pkti_enable_hdr_flow();    

      /*---------------------------------------------------------------------
        When the last traffic channel was up, the phone may have flow 
        controlled the HDR base station since it was running low on memory.
        However, the flow control information at the MAC layer is lost if 
        the traffic channel is lost. So, we need to check here if we are 
        running low on memory and flow control the base station again if 
        needed.

        Critical section not required. This functionality is already being 
        done from hdrfmac_mem_low_event_cb().
      ---------------------------------------------------------------------*/
      if (DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL) <= 
          DSM_POOL_MEM_EVENT_LEVEL(DSM_DS_SMALL_ITEM_POOL,
                                   DSM_MEM_LEVEL_HDR_FEW)
         )
      {
#if defined(FEATURE_DATA_FL_FLOW_CONTROL)
        ds707_ccm_mem_send_flow_control_cmd( DS707_CCM_MEM_DSM, DS707_CCM_FLOW_DOWN );
#else
        hdrl1_fmac_enable_flow_control((uint32) HDRFMAC_DRC_REQ_DSMWM );
#endif
      }
    }
    else  /* Call connected on 1X */
    {
#ifdef FEATURE_EHRPD
      /*-----------------------------------------------------------------------
        If this is a special service call connect, don't do any normal
        call connect processing like notifying DSD, setting current sys mode
        etc.
      -----------------------------------------------------------------------*/
      if (DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG != sys_chg_sm_state && 
          DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP != sys_chg_sm_state)
#endif /* FEATURE_EHRPD */
      {
        /* For HDR targets, we may need to do the following independent of 
           what ds707_current_data_sess_network is:

           If the SO for the call is QNC SO, terminate dormant data session 
           if any - this should be done here if it is done for 1X targets in
           ds707_pkti_get_orig_so.
        */

        DATA_IS707_MSG0( MSG_LEGACY_MED, 
                         "Call connected on 1X");
      /*-----------------------------------------------------------------------
      Notify DSD
      -----------------------------------------------------------------------*/
        dsd_event_info.event = DS_DSD_CLIENT_RADIO_CALLCONNECTED_EV;
        ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
       

        /*---------------------------------------------------------------------
          If last active network is HDR and we got a connect on 1X, 
          check if we need to do ppp_resync.
        ---------------------------------------------------------------------*/
        if (ds707_pkt_get_last_data_sess_network() == SYS_SYS_MODE_HDR)
        {
          ppp_resync_ptr = ds707_get_ppp_resync_flag_ptr();
          ds707_drs_moved_to_1X();
          *ppp_resync_ptr = TRUE;
        }

        /*---------------------------------------------------------------------
          Since the current data session network is not HDR,and the ALN restore
          CTA value holds the original CTA timer value, this is a failed ALN
          scenario. Set the CTA timer to 1 second and force the AT to go 
          dormant right away. Also Reset the aln_pending_cmd
       ----------------------------------------------------------------------*/
        if (ds707_pkti_failed_aln_cta_restore_val != 1 )
        {
          cta_val = 1;
          (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CTA_VAL,
          	                                       (void*)&cta_val,0);
          hdrrlp_copy_default_cta_val(cta_val);
          aln_cmd_pending = FALSE;
          ds707_drs_reset_aln_ppp_resync_flag();

          DATA_IS707_MSG0( MSG_LEGACY_MED, 
                           "Failed ALN handup, forcing CTA timer to 1 sec");
        }

        if ( (ds707_roaming_is_curr_mode_jcdma() == TRUE) && 
             (ds707_drs_is_ppp_resync_on_handdown_set()))
        {
          hdrpacdsif_handdown_to_1x();
        }
        ds707_pkt_mgr_update_cdsn(SYS_SYS_MODE_CDMA);
        ds707_pkt_mgr_set_current_sys_mode(SYS_SYS_MODE_CDMA);
        ds707_extif_inform_hdr_sess_status();
      }
#ifdef FEATURE_EHRPD
      else
      {
        DATA_IS707_MSG0( MSG_LEGACY_MED, "Special srv Call connected on 1X");
      }
#endif /* FEATURE_EHRPD */
    }

#else
    /*-----------------------------------------------------------------------
      Update ds707_current_data_sess_network to keep it accurate for non-HDR
      targets
    -----------------------------------------------------------------------*/
    ds707_pkt_mgr_update_cdsn(SYS_SYS_MODE_CDMA);
    ds707_extif_inform_hdr_sess_status();
#endif /* FEATURE_HDR */

    /*-----------------------------------------------------------------------
      Register for DSM memory events if back pressure removal is enabled.
    -----------------------------------------------------------------------*/
    if ( ps_pkt_info_get_tcp_ack_prio_enabled() &&
         ds707_is_bp_removal_enabled() == TRUE )
    {
      ds707_reg_unreg_dsm_mem_event(TRUE);
    }

    /*-----------------------------------------------------------------------
      Moved the Posting of DS707_PHYS_LINK_UP_EV right after updating the
      current_data_sess_network. This was earlier done before updating the
      the current data session network. Hopefully this does not introduce any 
      timing issues with functionalities on other modules - gr
    -----------------------------------------------------------------------*/
    event_info.so       = tc_state_ptr->so;
    event_info.pkt_inst = pkt_inst;
    event_info.curr_network = ds707_pkt_get_current_data_sess_network();

    /*-----------------------------------------------------------------------
     * CTA timer is started here as a result of posting phys link up 
     * event. Earlier it was done immediately after validating
     * the CALL_CONNECTED event notification from CM. The change is
     * to help the phone go dormant right away after failed ALN
    -----------------------------------------------------------------------*/
    ds707_cback_post_event( DS707_CBACK_PHYS_LINK_UP_EV, 
                            &event_info);

    /*-----------------------------------------------------------------------
      If we came out of dormancy due to SID/NID/PZID change, we might have
      to intiate a PPP resync.
    -----------------------------------------------------------------------*/
    ppp_resync_ptr = ds707_get_ppp_resync_flag_ptr();

    if (*ppp_resync_ptr )
    {
#ifdef FEATURE_HDR
      if (aln_cmd_pending == TRUE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "ALN command pending on call_connect : do ppp_resync");
        (void) ds707_drs_get_drs_bit_on_ALN();        
        aln_cmd_pending = FALSE;
      }
#endif /* FEATURE_HDR */
      ds707_drs_handle_drs1_recon();
      *ppp_resync_ptr = FALSE;
    }
  }
  ds707_drs_reset_ppp_resync_flags();

#ifdef FEATURE_DATA_OPTHO
  ds707_s101_call_connected_via_cm();
#endif /* FEATURE_DATA_OPTHO */

  return(ret_val);
} /* ds707_pkti_call_conn_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_END_CALL_HDLR

DESCRIPTION   Called by 3G DSMGR before telling CM to end a call.  Only
              thing done here is setting the ORDQ value correctly and
              letting CM know that this was a packet call.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_end_call_hdlr
(
cm_call_id_type        call_id,
cm_end_params_s_type  *end_params,
boolean               *delay_disconnect,
void                  *mode_spec_info_ptr,
sys_modem_as_id_e_type subs_id

)
{
  ds707_tc_state_type   *tc_state_ptr = NULL;
  ps_phys_link_type     *phys_link_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(mode_spec_info_ptr);

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "DS707 PKT END CALL HDLR call id : %d", call_id);

  tc_state_ptr  = ds707_get_pri_tc_ptr();
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if (tc_state_ptr->call_id != call_id)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "CALL IDs DO NOT MATCH. STILL CONTINUE");
  }

  *delay_disconnect     = FALSE;
  end_params->call_id   = call_id;
  end_params->info_type = CM_CALL_MODE_INFO_CDMA;

  end_params->end_params.cdma_end.end_reason_included = TRUE;
  /*-------------------------------------------------------------------------
    Assumption is that if DTR is de-asserted, then the IFACE state is not
    up nor is it routeable.  These values are set before we get here.
  -------------------------------------------------------------------------*/
  if( dscam_is_forced_callend()) /* If D2L redirection then set HDR end reason */
  {
    end_params->end_params.cdma_end.end_reason = HDRCP_CAI_IRAT_MEAS;
  }
  else if (ds707_pkti_can_go_dormant( tc_state_ptr,
                                 phys_link_ptr) == TRUE)
  {
    end_params->end_params.cdma_end.end_reason = CAI_REL_NORMAL;
  }
  else
  {
    end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "ds707_pkti_end_call_hdlr: End Reason: %d",
                  end_params->end_params.cdma_end.end_reason);

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    /*-----------------------------------------------------------------------
      Look at Primary tc state ordq value. This was set by
      ds707_pkt_mgr_iface_ioctl_go_null. We can get the  ORDQ reason passed  
      to go_null ioctl through primary tc state global variable.
     ----------------------------------------------------------------------*/
    if (tc_state_ptr->ordq_value == DS707_ORDQ_LOW_BATTERY)
    {
      end_params->end_params.cdma_end.end_reason = CAI_REL_PWDN;
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Sending POWER DOWN Release Reason to CM");
      /* Reset Primary Traffic channel state ORDQ to NULL */
      tc_state_ptr->ordq_value = DS707_ORDQ_NULL;
    }
    else if (tc_state_ptr->ordq_value == DS707_ORDQ_NONE)
    {
      /* We set the end_reason to SRV_INACT because if ps_iface_tear_down is
         called then the call cannot go dormant. Based on above the
         end_reason is set to CAI_REL_SRV_INACT. */
      end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "Sending NORMAL Release Reason to CM");
      /* Reset Primary Traffic channel state ORDQ to NULL */
      tc_state_ptr->ordq_value = DS707_ORDQ_NULL;
    }
  }
} /* ds707_pkti_end_call_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_IS_SN_IFACES_ROUTABLE_OR_UP

DESCRIPTION   Util function to check if atleast one of the ifaces is up or 
              routeable 

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkti_is_sn_ifaces_routable_or_up
(
  void
)
{
  uint8                        curr_iface_index;
  ps_iface_type                *curr_iface_ptr;
  ps_iface_state_enum_type      curr_iface_state;
  boolean                       iface_routeable_or_up = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index ++ )
  {        
    curr_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(
                                                       curr_iface_index);
    ASSERT(curr_iface_ptr != NULL);
    curr_iface_state = ps_iface_state(curr_iface_ptr);
    /*---------------------------------------------------------------
      If any one of the IFACES are UP or ROUTEABLE, set the flag.
      This flag will be used to force bring down of an IFACE.
    -------------------------------------------------------------------*/

    if (   ( ( curr_iface_state == IFACE_UP ) ||
             ( curr_iface_state == IFACE_ROUTEABLE ) ) )
    {
      iface_routeable_or_up = TRUE;
      break;
    }
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, "ds707_pkti_is_sn_ifaces_routable_or_up "
  	               "returned : %d", iface_routeable_or_up);
  return iface_routeable_or_up;
 
} /* ds707_pkti_is_sn_ifaces_routable_or_up */

/*===========================================================================
FUNCTION      DS707_PKTI_CALL_ENDED_HDLR

DESCRIPTION   Called by 3G DSMGR when a call release has been received from
              CM.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_call_ended_hdlr
(
  ds_cmd_type            *cmd_ptr,
  boolean                 call_was_connected,
  sys_modem_as_id_e_type  subs_id
)
{
  uint32                        pkt_instance;
  ds707_tc_state_type          *tc_state_ptr  = NULL;
  ps_phys_link_type            *phys_link_ptr = NULL;
  boolean                       ui_end_key_forces_null = FALSE;
  ds707_call_event_payload_type ds707_event_payload;
  boolean                       sn_ifaces_routeable_or_up = FALSE;
  boolean                       go_null    = FALSE;
  cm_client_id_type             ds3g_cm_client_id;
  ds_dsd_event_info_type        dsd_event_info = {0};
  ds3g_call_info_cmd_type      *call_info_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  DATA_MDM_ARG_NOT_USED(call_was_connected);

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "NULL pointer passed" );
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                   "DS707 PKT CALL ENDED HDLR call id %d",
                   call_info_cmd_ptr->call_id);

  pkt_instance = ds707_pkti_get_pkt_state_index_cid( 
                                             call_info_cmd_ptr->call_id);
  if (pkt_instance < DS707_MAX_PDN_INSTANCES)
  {
    tc_state_ptr  = ds707_get_pri_tc_ptr();
    phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Could not end call with id %d", 
                    call_info_cmd_ptr->call_id);
    return;
  }

  /*-----------------------------------------------------------------------
  Notify DSD client 
  -----------------------------------------------------------------------*/
  if (DS707_INVALID_SO != tc_state_ptr->so)
  {
    if ( DS_ISDATA_HDR( tc_state_ptr->so ))
    { 
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_CALLENDED_EV;
      ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, &dsd_event_info);
    }
    else
    {
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_CALLENDED_EV;
      ds_dsd_ext_event_notify(DS_DSD_RADIO_1X, &dsd_event_info);
    }
  }

  /*-------------------------------------------------------------------------
  Send call ended event to DIAG.
  -------------------------------------------------------------------------*/
  ds707_event_payload.ds707_event  = EVENT_DS707_PKT_CALL_ENDED;
  ds707_event_payload.so_type = tc_state_ptr->so;
  ds707_event_payload.call_id = call_info_cmd_ptr->call_id;
  event_report_payload( EVENT_PKT_DS707,
                        sizeof (ds707_event_payload),
                        &ds707_event_payload );

  /*------------------------------------------------------------------------
    FR 18742, Set the flag to true so that all further Data call originations
    are blocked at DS itself. This helps in CM completing its BSR and acquire
    a better system. This is done in DS task context.
   --------------------------------------------------------------------------*/
  if( call_info_cmd_ptr->end_status == CM_CALL_END_BSR_COMPLETE_GSM || 
      call_info_cmd_ptr->end_status == CM_CALL_END_BSR_COMPLETE_WCDMA  )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "CM end call with BSR completed cause code"
                                    "block further originations until new SS event");
    ds707_pkti_set_is_bsr_completed(TRUE); 
  }

  /*-------------------------------------------------------------------------
     At this point, we know traffic channel is gone.  Have to know if the
     IFACE should go dormant or not.  Then we need to clean up the pkt_state
     info for the next call.

     QNC_HO case
     Register some JCDMA stuff
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  if( DS707_GONULLTIMER_VALUE_FOR_VOICE_MOSMS_INTERRUPTION > 0 )
  {
    if (call_info_cmd_ptr->end_status == CM_CALL_END_HDR_RELEASED_BY_CM)
    {
      ds707_gonulltmr_set_voice_mosms_interruption_timer_values();
    }
    else
    {
      ds707_gonulltmr_restore_timer_values();
    }
  }
#endif

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    ds3g_cm_client_id = ds3g_get_cm_client_id();

    if ((call_info_cmd_ptr->end_status == CM_CALL_END_CLIENT_END) &&
        (call_info_cmd_ptr->client_id != ds3g_cm_client_id)
       )
    {
      DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                       "UI %d ended pkt call - go NULL",
                       call_info_cmd_ptr->client_id);
      ui_end_key_forces_null = TRUE;

#ifdef FEATURE_HDR_QOS
      ds707_sec_pkt_mgr_cleanup_all_secondary_flows(NULL, TRUE);
#endif /* FEATURE_HDR_QOS */
    }
  }

  DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                   "CM Call end_status: %d",
                   call_info_cmd_ptr->end_status);
  /*---------------------------------------------------------------------
    If any one of the IFACES are UP or ROUTEABLE, this func returns TRUE
      This flag will be used to force bring down of an IFACE.
  ---------------------------------------------------------------------*/
  sn_ifaces_routeable_or_up = ds707_pkti_is_sn_ifaces_routable_or_up();


  #ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  if((call_info_cmd_ptr->end_status == CM_CALL_END_HDR_RELEASED_BY_CM) &&
     (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_COMING_UP))
  {
      if ( ds707_pkt_auto_redial_timer_start() == FALSE)
      {

        /* ---------------------------------------------------------------
           If the timer cannot get started because of retry attempts are
            exhausted, tear down the phys link
        -------------------------------------------------------------------*/
        go_null = TRUE;
      }
      
  }
  else
#endif /* FEATURE_DEDICATED_C2K_MODE*/

  /*-------------------------------------------------------------------------
    If call cannot go dormant, then send a message to DSSNET, so that it
    ends any PPP issues.  Also cleanup the phys_link state, indicating that
    the call cannot go dormant.
  -------------------------------------------------------------------------*/
  if ( (call_info_cmd_ptr->end_status == CM_CALL_END_OFFLINE) ||
       (call_info_cmd_ptr->end_status == CM_CALL_END_REL_SO_REJ)
     )
  { 
    DATA_IS707_MSG1( MSG_LEGACY_HIGH,
                     "Call end, Call_end_status: %d , go null", 
                     call_info_cmd_ptr->end_status);
    go_null = TRUE;   
  }
  else if (((ds707_pkti_can_go_dormant(tc_state_ptr, 
                                       phys_link_ptr) == FALSE)  ||
           (ui_end_key_forces_null == TRUE)) &&  
           (sn_ifaces_routeable_or_up == FALSE ) )
  {
    DATA_IS707_MSG3( MSG_LEGACY_HIGH, 
                     "ui_end_key_forces_null %d call_end_status %d sn_iface_state %d",
                     ui_end_key_forces_null, 
                     call_info_cmd_ptr->end_status,
                     sn_ifaces_routeable_or_up );
    go_null = TRUE;
   
  }
   /* going dormant below */
  else if (((call_info_cmd_ptr->end_status == CM_CALL_END_INCOM_CALL) ||
            (call_info_cmd_ptr->end_status == CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS)) &&
           (dsrlp_is_inited(DSRLP_FIXED_SR_ID_INDEX) == TRUE)
       )
  {
    /*---------------------------------------------------------------------
       This covers case where MS is orig'ing same time as incoming data
       call.  In this case, RLP is init'd in MC task, before inc call msg
       is received by DS.  Don't clean up link (meaning RLP), as that is 
       needed when DS finally answers incoming call.
    ---------------------------------------------------------------------*/
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
               "Simultaneous incoming and outgoing call. "
               "Expecting incoming data call now.  Outgoing call ended here");
    /*---------------------------------------------------------------------
       For Simultanneous incoming and outgoing calls, the PHYS LINK state
       should be restored to DOWN state even though RLP and other resources
       are not cleaned up. Otherwise this would result in failure to accept
       incoming calls. Also if the MC does not put through an incoming call
       after rejecting an outgoing call, the PHYS_LINK state would be 
       stuck in RESUMING state and would cause dormancy re-orig failures
      ---------------------------------------------------------------------*/

    /* Reset bearer_id */
    phys_link_ptr->bearer_id = DSRLP_INVALID_HDR_MRLP_FLOW;

    ps_phys_link_down_ind(phys_link_ptr);
  }
  else
  {
    ds707_pkti_phys_link_cleanup( pkt_instance, 
                                  CAN_GO_DORMANT, 
                                  cmd_ptr);
  }

  if (go_null == TRUE)
  {
 /*-------------------------------------------------------------------------
   Set the global end reason to CM end_status value. 
   If the call has been throttled then this is used so that the caller is 
   returned with a DSAT_DELAYED response instead of DSAT_NO_CARRIER for JCDMA
 
   Do not set the global flag to Throttle if call is Dormant. AT should only
   respond with AT commands in response to an AT command. When Call is 
   dormant, AT does not accept any AT commands and thus should not send 
   any.
 -------------------------------------------------------------------------*/
    if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
    {
      if (ds707_pkt_is_dormant(phys_link_ptr) != TRUE)
      {
        (void)ds707_pkt_mgr_set_last_orig_fail_reason(
                                         call_info_cmd_ptr->end_status);
      }
    }

    ds707_pkti_phys_link_cleanup( pkt_instance, 
                                  CANNOT_GO_DORMANT, 
                                  cmd_ptr);
  }

#ifdef FEATURE_HDR
  if (aln_cmd_pending == TRUE)
  {
    aln_cmd_pending = FALSE;
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                 "ALN cmd pending on call_end: do DRS1 orig for ppp_resync");
    ds707_aln_cmd_hdlr();
  }
#endif /* FEATURE_HDR */

#ifdef FEATURE_DATA_OPTHO
  ds707_s101_call_ended_via_cm();
#endif /* FEATURE_DATA_OPTHO */

} /* ds707_pkti_call_ended_hdlr() */
/*lint -restore*/

/*===========================================================================
FUNCTION      DS707_PKTI_GET_DATA_COUNT_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes
              ds_get_current_session_data_counters() to get total count of
              bytes transmitted and received by RLP for the current call.

DEPENDENCIES  None.

RETURN VALUE  boolean:  indicates if the count is valid.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean  ds707_pkti_get_data_count_hdlr
(
cm_call_id_type    call_id,
uint32            *rx_from_network_byte_cnt,
uint32            *tx_to_network_byte_cnt
)
{
  boolean                    ret_val         = TRUE;
  ds707_tc_state_type       *tc_state_ptr    = NULL;
  uint32                     pkt_inst;
  ds707_flow_type           *flow_ptr;

  pkt_inst = ds707_pkti_get_pkt_state_index_cid(call_id);

  if (pkt_inst < DS707_MAX_PDN_INSTANCES)
  {
    tc_state_ptr  = ds707_get_pri_tc_ptr();
  }

  if (tc_state_ptr == NULL)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "get data count unknown id %d",
                    call_id);
    ret_val = FALSE;
  }

  else
  {
    tc_state_ptr = ds707_get_pri_tc_ptr() ;
    flow_ptr     = DS707_PRI_DS_FLOW_PTR(tc_state_ptr);

    if (flow_ptr == NULL)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid flow ptr");
      ret_val = FALSE;
    }
    else
    {
      ds707_wmk_get_byte_counts(tx_to_network_byte_cnt,
                              rx_from_network_byte_cnt,
                              flow_ptr->tx_wm_list,
                              tc_state_ptr->fwd_rlp.post_rx_wm_list);
    }
  }
  return ret_val;
} /* ds707_pkti_get_data_count_hdlr() */

/*===========================================================================
FUNCTION      DS707_PKTI_GET_LAST_SESS_COUNT_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes
              ds_get_last_session_data_counters() to get total count of
              bytes transmitted and received by RLP for the last call which
              is being released or current call.

DEPENDENCIES  None.

RETURN VALUE  boolean:  indicates if the count is valid.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean  ds707_pkti_get_last_sess_count_hdlr
(
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
)
{
  boolean              ret_val         = TRUE;
  ds707_tc_state_type *tc_state_ptr;
  ds707_flow_type     *flow_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*------------------------------------------------------------------------
   * If we are not in a call then return the byte counts from previous call
   * ----------------------------------------------------------------------*/
  if(ds707_get_pkt_data_sess_state() == DS707_PKT_DATA_SESS_NONE )
  {
    ds707_wmk_get_prev_call_byte_counts(tx_to_network_byte_cnt, rx_from_network_byte_cnt);
    return ret_val;
  }  

  tc_state_ptr = ds707_get_pri_tc_ptr();
  flow_ptr     = DS707_PRI_DS_FLOW_PTR(tc_state_ptr);

  if (flow_ptr == NULL)
  {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid flow ptr");
      ret_val = FALSE;
  }
  else
  {
    ds707_wmk_get_byte_counts(tx_to_network_byte_cnt,
                              rx_from_network_byte_cnt,
                              flow_ptr->tx_wm_list,
                              tc_state_ptr->fwd_rlp.post_rx_wm_list);
  }

  return ret_val;
} /* ds707_pkti_get_last_sess_count_hdlr() */


/*===========================================================================
FUNCTION      DS707_PKTI_SYS_CHNG_HDLR

DESCRIPTION   Called by 3G DSMGR when the serving system changes.  At this
              point, it is necessary to ask CM what the preferred pkt mode
              is.  
              
              NOTE: From ds3gmgr perspective, 1X and HDR are the same system.
              Hence this function treats 1X and HDR the same. Read 1X as
              1X or HDR in the statements below in this function.

              If the new mode == 1X, then there is nothing to do:
                prev mode = 1x, new mode = 1x
                  - do nothing
                prev mode != 1x, new mode = 1x
                  - still do nothing.  The ACL's automatically update, so
                    that 1X is now the IFACE to use for pkt calls.  The
                    iface should always be enabled.  The only time the
                    IFACE is disabled is during async calls.

              If the new mode != 1X, then:
                prev mode != 1x, new mode != 1x
                  - do nothing
                  
                prev mode = 1x, new mode != 1x
                  - if call is dormant, then PPP_ABORT to cause a cleanup.
                  - in all other cases, wait for call end to arrive.  On
                    the call down indication, we'll see if call should go
                    dormant or NULL.
                  - There is no need to enable/disable the iface, nor 
                    change the ACL's.  The ACL's automatically update to 
                    not use 1X when 1X is not the preferred pkt mode.

DEPENDENCIES  None.

RETURN VALUE  boolean: indicates if the call connection info is valid.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_pkti_sys_chng_hdlr
(
  sys_modem_as_id_e_type        subs_id,
  sys_sys_mode_e_type           new_mode,
  sys_sys_mode_e_type           old_mode
)
{
  ds707_cback_event_info_type     event_info;
  sys_sys_mode_e_type             ldsn = SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type             cdsn = SYS_SYS_MODE_NO_SRV;
  boolean                         notify_dsd = FALSE;
  boolean                         delay_dsd_notification = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&event_info,0,sizeof(event_info));
  DATA_IS707_MSG3(MSG_LEGACY_HIGH,
                  "Sys Changed Event on Subs Id:%d new mode:%d old mode:%d",
                   subs_id, new_mode, old_mode);
  if ( subs_id != ds3g_get_ps_subs_id() )
  {
    return;
  }

  if (new_mode == SYS_SYS_MODE_NO_SRV)
  {
    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "Setting flag to FALSE. old mode %d new mode %d",
                     old_mode, new_mode);
    ds707_set_has_phone_acquired_signal(FALSE); 

#ifdef FEATURE_DATA_OPTHO
    /* Phone acq signal flag needs to be updated before any processing can be done.
       Also, S101 SM needs to process the sys chg before bearer tech chngd notification
       sent below
     Reason: Bearer tech depends on the S101 SM state to be None to process
     it. When the System moves to anything other than LTE, then S101 transitions
     to None state and bearer tech will process the change as usual. So we need
       to ensure that S101 changes its state before bearer tech is invoked
    */
  ds707_s101_sys_chg_info(new_mode,old_mode);
#endif /* FEATURE_DATA_OPTHO */

    ds707_pkt_bearer_tech_changed_notification();

#ifdef FEATURE_HDR_QOS
    ds707_sec_pkt_mgr_handoff_signal_change_hdlr(new_mode);
#endif

#ifdef FEATURE_DATA_OPTHO
    /*---------------------------------------------------------------------- 
     While HDR Actv HO is in progress, we ignore this command generated
     due to CM SS event.
    -----------------------------------------------------------------------*/ 
    if (!ds707_s101_is_hdr_actv_ho_in_prog())
#endif /* FEATURE_DATA_OPTHO */
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "No service from CM, disable flow");
      ds707_phys_link_disable_flow(DS_FLOW_NO_SERVICE_MASK );
    }
    return;
  }
  else
  {
    /* Phone has acquired signal: either 1x or DO */
    DATA_IS707_MSG2( MSG_LEGACY_HIGH,
                     "Setting flag to TRUE. old mode %d new mode %d",
                     old_mode, new_mode );
    ds707_set_has_phone_acquired_signal(TRUE);

    if ((new_mode == SYS_SYS_MODE_CDMA) || (new_mode == SYS_SYS_MODE_HDR))
    {
      /* check if ehrpd/hrpd/1x*/
      if ((new_mode == SYS_SYS_MODE_HDR) && 
          (TRUE == ds3gdsdif_ehprd_is_pref_system(subs_id)))
      {
        /* eHRPD - store last known good system as EPC */
        ds707_set_last_good_sys(DS707_SYS_INFO_EPC_SYSTEM);
      }
      
      else
      {
        /* HRPD or 1X - store last known good system as NON_EPC */
        ds707_set_last_good_sys(DS707_SYS_INFO_NON_EPC_SYSTEM);
      }
      
      ds707_sys_chg_set_notify_dsd();
      notify_dsd = TRUE;
      ds707_pkt_bearer_tech_changed_notification();
    }
    
    if ( subs_id == SYS_MODEM_AS_ID_1 )
    {      
      DATA_3GMGR_MSG1( MSG_LEGACY_HIGH, "Update current sys mode to %d", 
                       new_mode );
      ds707_pkt_mgr_set_current_sys_mode(new_mode);
    }

#ifdef FEATURE_DATA_OPTHO
    /* S101 SM needs to process this before bearer tech chngd notification
       sent below. 
       Reason: Same as mentioned above
    */
    ds707_s101_sys_chg_info(new_mode,old_mode);
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_HDR_QOS
    /*-------------------------------------------------------
    Inform secondary packet manager when signal is 
     -------------------------------------------------------*/
    ds707_sec_pkt_mgr_handoff_signal_change_hdlr(new_mode);
#endif

#ifdef FEATURE_EHRPD
    /*-------------------------------------------------------
    If Sys Chg Hdlr SM is in !NULL state, it means it's 
    already in some cleanup process. Skip the handoff allowed 
    check and IDM check 
    -------------------------------------------------------*/
    if (DS707_SYS_CHG_SM_NULL == ds707_query_sys_chg_sm_state())
#endif /* FEATURE_EHRPD */
    {
      ldsn = ds707_pkt_get_last_data_sess_network();
      cdsn = ds707_pkt_get_current_data_sess_network();
      DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                       "ds707_pkti_sys_chng_hdlr: LDSN %d, CDSN %d ",
                       ldsn, cdsn);

      /*--------------------------------------------------------------- 
        Check if handoff is allowed betn old and new pref system.
        If handoff is not allowed, for non-fusion targets, if the new
        pref system is 1x or HDR, do not tear down C2K ifaces.
        For fusion type devices we might still need to tear down,
        since 1x and HDR maybe on different modem.
      ---------------------------------------------------------------*/
      if( (!(ds707_pkt_mgr_handoff_allowed(old_mode, new_mode))) 
#ifndef FEATURE_DATA_FUSION
        && ((new_mode != SYS_SYS_MODE_CDMA) && (new_mode != SYS_SYS_MODE_HDR)) 
#endif
      )
      {
        DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                         "handoff not allowed between old sys %d and new sys %d",
                         old_mode, new_mode );
#ifdef FEATURE_DATA_WLAN_MAPCON 
        /* If data_fusion is enabled (Fusion devices with two modems),
           we need to abort calls on 1x<->do transition as well even
           if EPC is defined since 1X and DO reside on two different modems.
           Hence the below if def also checks for !Fusion */
#if defined (FEATURE_EPC_HANDOFF) && !defined (FEATURE_DATA_FUSION)
        if (ds707_pkt_mgr_is_data_on_ehrpd())
        {
          /* Note that we are checking is_data_on_ehrpd() above. If devmgr was
             maintaining partial context, then is_data_on_ehrpd will return false
             and we trigger abort session.
             If there are some ifaces that are up, EPC will decide whether to HO
             to iwlan or tear it down. Trigger nonepc_to_epc event so MH will
             clean up any coming up ifaces/do the necessary housekeeping
             (e.g call ds707_sys_chg_sr_process_epc_handoff) */

          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sys moved to some NON EPC RAT but "
                        "HO to iWLAN may happen. Trigger nonepc_to_epc event");

          //Set the system to no_system since the WWAN rat is neither eHRPD nor 
          //1x. This will ensure that dev mgr doesn't maintain partial context
          //even if EPC hands off a eHRPD call to iwlan.
          ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);
          event_info.curr_network = new_mode;
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "call end reason is set to "
                          "PS_NET_DOWN_REASON_UNPREFERRED_RAT");
          event_info.tear_down_info.validity_mask |=
                                       PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
          event_info.tear_down_info.data_info.call_end_reason = 
                                              PS_NET_DOWN_REASON_UNPREFERRED_RAT;
          ds707_cback_post_event(DS707_CBACK_HANDOFF_EPC_TO_NONEPC, &event_info);
        }
        else
#endif /* EPC defined && data_fusion not defined*/
#endif /* FEATURE_DATA_WLAN_MAPCON */
        if (old_mode == SYS_SYS_MODE_CDMA && new_mode == SYS_SYS_MODE_LTE &&
            ((cdsn == SYS_SYS_MODE_CDMA)|| (ldsn == SYS_SYS_MODE_CDMA))
           )
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, "1X->LTE transition. Trigger "
                                            "NONEPC TO EPC Event");

          /*--------------------------------------------------------------- 
            post DS707_CBACK_HANDOFF_NONEPC_TO_EPC.
            Set the system to no_system so PPP gets aborted locally when
            ifaces are torn down as part of nonepc_to_epc processing
          ---------------------------------------------------------------*/
          ds707_sys_chg_hdlr_chg_sys_info(DS707_SYS_INFO_NO_SYSTEM);
          event_info.curr_network = ds707_pkt_get_current_data_sess_network();
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "call end reason is set to "
                          "PS_NET_DOWN_REASON_UNPREFERRED_RAT");
         event_info.tear_down_info.validity_mask |=
                                       PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
          event_info.tear_down_info.data_info.call_end_reason = 
                                       PS_NET_DOWN_REASON_UNPREFERRED_RAT;
          ds707_cback_post_event(DS707_CBACK_HANDOFF_NONEPC_TO_EPC, &event_info);
        }
        else
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, "TEAR DOWN ds707 iface");

          /*--------------------------------------------------------------- 
            post abort to tear down
          ---------------------------------------------------------------*/
          event_info.curr_network = ds707_pkt_get_current_data_sess_network();
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "call end reason is set to "
                          "PS_NET_DOWN_REASON_UNPREFERRED_RAT");
          event_info.tear_down_info.validity_mask |= 
                                         PS_CLIENT_DATA_INFO_CALL_END_REASON_MASK;
          event_info.tear_down_info.data_info.call_end_reason = 
                                         PS_NET_DOWN_REASON_UNPREFERRED_RAT;
          ds707_cback_post_event(DS707_CBACK_ABORT_SESSION_EV, &event_info);
        }
      }

      /*----------------------------------------------------------------------
        If handoff is allowed and old sys and new sys is 1x <-> HDR,
        call IDM change handler. IDM change handler will post bearer tech
        changed event.
        In case call connected is received before sys change handler is called
        current data session network would have already been updated to the
        new mode. So check for either current data session network, or last
        data session network.
      -----------------------------------------------------------------------*/
      if( ( ((cdsn == SYS_SYS_MODE_CDMA)|| (ldsn == SYS_SYS_MODE_CDMA)) && 
            (new_mode == SYS_SYS_MODE_HDR) ) 
          ||
          ( ((cdsn == SYS_SYS_MODE_HDR)|| (ldsn == SYS_SYS_MODE_HDR)) && 
            (new_mode == SYS_SYS_MODE_CDMA) ) )
      {
        DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Posting IDM change cmd. Delaying"
                                          "Notification to DSD");
        delay_dsd_notification = TRUE;
        ds707_pkt_post_idm_change_cmd(new_mode);
      }
    }
#ifdef FEATURE_EHRPD
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Sys Chg SM in !NULL state. Skip "
                       "HO Allowed/IDM Check and delay DSD notification");
      /*---------------------------------------------------------------------
      If the new system is either CDMA/HDR, we already set the notify_dsd 
      flag. Since the Sys chg SM is in some cleanup state, don't notify dsd
      now. When the sys chg SM moves to NULL, it'll notify DSD 
      ---------------------------------------------------------------------*/
      delay_dsd_notification = TRUE;
    }
#endif /* FEATURE_EHRPD */
  }

 /*---------------------------------------------------------------------
  If the new mode is not OOS or CDMA, else old mode was not cdma and new
  mode is OOS then we need to clear, the CDMA rate set variables
  ---------------------------------------------------------------------*/
  if ( ((new_mode != SYS_SYS_MODE_NO_SRV) || (new_mode != SYS_SYS_MODE_CDMA)) 
       ||
       ((new_mode == SYS_SYS_MODE_NO_SRV) && (old_mode != SYS_SYS_MODE_CDMA)) )
    ds707_clear_1x_rateset();


  /*---------------------------------------------------------------------
  If the system is moving to other RAT other than CDMA, we shall enable 
  the flow if the delay retry timer is running.
  In case, the system is moving back to 1x, the flow control will be
  triggered in ds707_pkti_phys_link_bring_up_cmd()
  ---------------------------------------------------------------------*/
  if((old_mode != new_mode) &&
     (old_mode == SYS_SYS_MODE_CDMA))
  {
    if(ds3gi_is_timer_running(DS3G_TIMER_RETRY_DELAY_ORIG))
    {
      ds707_phys_link_enable_flow(DS_FLOW_707_LOCK_MASK);
    }
  }

  if (notify_dsd)
  {
    if (delay_dsd_notification)
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_pkti_sys_chng_hdlr: Delaying DSD notification");
    }
    else
    {
      DATA_IS707_MSG0( MSG_LEGACY_MED, 
                       "ds707_pkti_sys_chng_hdlr: Sending DSD notification");
      ds707_sys_chg_send_dsd_notification();
    }
  }

} /* ds707_pkti_sys_chng_hdlr() */

#ifdef FEATURE_DATA_PKT_CLASSIFIER
void ds707_pkti_sys_chng_hdlr_ex
(
  sys_sys_mode_e_type sys_mode
)
{
  ds707_pkti_sys_chng_hdlr(SYS_MODEM_AS_ID_1, 
                           sys_mode, 
                           SYS_SYS_MODE_NO_SRV);
}
#endif/* FEATURE_DATA_PKT_CLASSIFIER */

/*===========================================================================
FUNCTION      DS707_ISIM_REFRESH_VOTING_HDLR

DESCRIPTION   
  This function checks the state of all PDNs. If one of the PDN iface is in UP
  state and phy link is not in dormant. It votes NO/FALSE. Otherwise, It votes
  for YES/TRUE

PARAMETERS   
  None.
  
DEPENDENCIES  
  None.

RETURN VALUE  
  Boolean - whether ISIM Refresh can proceed or not from C2K perspective.
  TRUE - Vote for YES
  FALSE - Vote for NO

SIDE EFFECTS  
  None.
===========================================================================*/
LOCAL boolean ds707_isim_refresh_voting_hdlr
(
  sys_modem_as_id_e_type      subs_id
)
{
  uint8                         curr_iface_index;
  ps_iface_type                *curr_iface_ptr = NULL;
  ps_iface_state_enum_type      curr_iface_state;
  ps_phys_link_type            *phys_link_ptr = NULL;

  DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                   "DS707 ISIM refresh handler invoked");

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {
    curr_iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(
                                                       curr_iface_index);
    ASSERT(curr_iface_ptr != NULL);
    curr_iface_state = ps_iface_state(curr_iface_ptr);
    DATA_IS707_MSG3( MSG_LEGACY_HIGH,
                     "curr_iface_instance: 0x%x:%d curr_iface_state: %d",
                     curr_iface_ptr->name,
                     curr_iface_ptr->instance,
                     (int)curr_iface_state);
    /*---------------------------------------------------------------
      If any one of the IFACES are UP/ROUTEABLE/COMINGUP/LINGERING,
      and the phylink is not dormant, DS returns FALSE
    -------------------------------------------------------------------*/

    if (((PS_IFACE_IS_UP((ps_iface_type*) curr_iface_ptr) == TRUE) ||
         (curr_iface_state == IFACE_LINGERING )) &&
         (ds707_pkt_is_dormant(phys_link_ptr) == FALSE ))
    {
      return FALSE;
    }
  }  
  return TRUE;
} /* ds707_isim_refresh_voting_hdlr */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_HDLR_INIT

DESCRIPTION   Called at initialization.  Registers callbacks with 3G DSMGR.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_hdlr_init(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds707_pkti_mgr_hdlr_func_tbl.originate_call_handler = ds707_pkti_orig_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.call_id_handler = ds707_pkti_call_id_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.call_conf_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.call_progress_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.call_connected_handler = ds707_pkti_call_conn_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.rab_rel_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.rab_re_estab_ind_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.rab_re_estab_rej_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.rab_re_estab_fail_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.complete_ll_connect_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.setup_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.incoming_call_handler = ds707_pkti_inc_call_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.user_answer_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.user_end_handler = ds707_pkti_end_call_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.complete_ll_disconnect_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.call_ended_handler = ds707_pkti_call_ended_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.switch_call_handler = ds707_pkti_switch_calls_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.force_call_down_handler = ds707_pkti_force_call_down_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.get_data_counters_handler = ds707_pkti_get_data_count_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.data_suspend_ind_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.data_resume_ind_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.sys_change_handler = ds707_pkti_sys_chng_hdlr;
  ds707_pkti_mgr_hdlr_func_tbl.context_mod_ind_handler = NULL;
#ifdef FEATURE_NW_INITIATED_QOS
  ds707_pkti_mgr_hdlr_func_tbl.nw_modify_req_handler = NULL;
#endif /* FEATURE_NW_INITIATED_QOS */
  ds707_pkti_mgr_hdlr_func_tbl.context_modify_cnf_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.context_modify_rej_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.context_prom_ind_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.ps_data_fail_ind_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.ps_data_generic_ind_handler = NULL;
#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
  ds707_pkti_mgr_hdlr_func_tbl.isim_refresh_voting_handler = ds707_isim_refresh_voting_hdlr;
#ifdef FEATURE_DATA_LTE
  ds707_pkti_mgr_hdlr_func_tbl.bearer_activate_handler = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.get_pdn_conn_ind_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.pdn_conn_rej_ind_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.pdn_conn_fail_ind_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.bearer_alloc_rej_ind_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.bearer_alloc_fail_ind_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.bearer_mod_rej_ind_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.bearer_mod_ind_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.originate_call_handler_ext = NULL;
#endif /* FEATURE_DATA_LTE */
#if defined(FEATURE_DATA_LTE)
  ds707_pkti_mgr_hdlr_func_tbl.cntxt_transfer_ind_hdlr = NULL;
#endif /* defined(FEATURE_DATA_LTE) && defined(FEATURE_DS_UMTS_REARCH) */
#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
  ds707_pkti_mgr_hdlr_func_tbl.incoming_page_handler = ds707_pkti_incoming_page_cmd_hdlr;
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */
  ds707_pkti_mgr_hdlr_func_tbl.bsr_reset_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.report_throughput_info_hdlr = ds707_get_throughput_info;
  ds707_pkti_mgr_hdlr_func_tbl.refresh_actual_throughput_hdlr = ds707_calculate_actual_throughput;
  ds707_pkti_mgr_hdlr_func_tbl.send_downlink_throughput_estimation_indicaiton = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.prepare_downlink_estimated_throughput_report = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.get_downlink_throughput_t_accumulate = NULL;

  ds707_pkti_mgr_hdlr_func_tbl.coex_flow_enable_hdlr = ds707_coex_data_flow_enable_hdlr; 
  ds707_pkti_mgr_hdlr_func_tbl.coex_flow_disable_hdlr = ds707_coex_data_flow_disable_hdlr; 

  /*-----------------------------------------------------------------
     For modem power statistics callbacks
    -------------------------------------------------------------------*/
   ds707_pkti_mgr_hdlr_func_tbl.config_modem_power_stats = ds707_config_modem_power_stats_hdlr;
   ds707_pkti_mgr_hdlr_func_tbl.get_modem_power_stats = ds707_get_modem_power_stats_hdlr;

  ds707_pkti_mgr_hdlr_func_tbl.get_total_uldl_wm_cnt_hdlr = NULL;
  ds707_pkti_mgr_hdlr_func_tbl.bringdown_phys_link_hdlr = NULL;

  /*---------------------------------------------------------------------
     register callbacks with DS3G
  ---------------------------------------------------------------------*/
  ds3g_register_mode_sp_handler(SYS_SYS_MODE_CDMA,
                                DS_CALL_TYPE_PKT,
                                &ds707_pkti_mgr_hdlr_func_tbl);

} /* ds707_pkt_mgr_hdlr_init() */

/*===========================================================================
FUNCTION      DS707_PKT_GET_DATA_COUNT

DESCRIPTION   Returns the count of data sent and received in the current data
              call


DEPENDENCIES  None.

RETURN VALUE  TRUE: If the values are correct
              FALSE: If the values are incorrect

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_get_data_count
(
  uint32                    *rxed_bytes,
  uint32                    *txed_bytes
)
{
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ret_val = ds707_pkti_get_last_sess_count_hdlr( rxed_bytes, 
                                                 txed_bytes );

  return ret_val;
} /* ds707_pkt_get_data_count() */

#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================
FUNCTION DS707_REG_CALL_ALLOWED_HDLR

DESCRIPTION
  This function is used to register the call allowed handler with the 3GPP2
  Mode Handler. This API is called by EPC Mode Handler. 

PARAMETERS
  call_allowed_hdlr_f - Pointer to the call allowed handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_reg_call_allowed_hdlr
(
  ds707_call_allowed_hdlr_f_type call_allowed_hdlr_f
)
{
  /*------------------------------------------------------------------------
    Store the pointer to the call allowed handler provided by the EPC mode
    handler
  ------------------------------------------------------------------------*/
  ds707_call_allowed_hdlr_f = call_allowed_hdlr_f;

} /* ds707_reg_call_allowed_hdlr */

/*===========================================================================
FUNCTION DS707_GET_CALL_ALLOWED_HDLR

DESCRIPTION
  This function is used to get the call allowed handler provided by EPC
  mode handler

PARAMETERS
  call_allowed_hdlr_f - Pointer to the call allowed handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ds707_call_allowed_hdlr_f_type ds707_get_call_allowed_hdlr
(
  void
)
{
  /*------------------------------------------------------------------------
    Return the pointer to the call allowed handler provided by the EPC mode
    handler
  ------------------------------------------------------------------------*/
  return ds707_call_allowed_hdlr_f;
} /* ds707_get_call_allowed_hdlr */

#endif /* FEATURE_EPC_HANDOFF */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_FAILED_ALN_CTA_RESTORE_VAL

DESCRIPTION   
  This function sets the value of failed_aln_cta_restore_val to the passed
  value

PARAMETERS   
  dword  failed_aln_cta_restore_val
  
RETURN VALUE  
  None

SIDE EFFECTS  
  None.
===========================================================================*/
void ds707_pkt_mgr_set_failed_aln_cta_restore_val(
  dword  failed_aln_cta_restore_val
)
{
  DATA_IS707_MSG1( MSG_LEGACY_MED, 
                   "Setting failed aln cta restore val to %d", 
                   failed_aln_cta_restore_val );
  ds707_pkti_failed_aln_cta_restore_val = failed_aln_cta_restore_val;
}

#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION     DS707_SET_ALN_CMD_PENDING

DESCRIPTION  This function sets the value of aln_cmd_pending flag.

PARAMETERS   boolean
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_set_aln_cmd_pending(
  boolean val
)
{
  aln_cmd_pending = val;
}
#endif /* FEATURE_HDR */

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
FUNCTION     DS707_PKTI_TUNNEL_PHYS_LINK_CLEANUP

DESCRIPTION  This handler is called by hdrpac when a 
             HDRIND_IDLE_CONNECTION_OPENED is received while in tunnel mode
             operation

PARAMETERS   None.
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_pkti_tunnel_phys_link_cleanup
(
  uint32 end_reason,
  ds707_tc_state_type *tc_state_ptr,
  ps_phys_link_type   *phys_link_ptr
)
{
  ds707_cback_event_info_type  event_info;
  boolean                      can_go_dormant = FALSE;

  DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                   "DS707 PKT Tun phys link down cleanup");
  
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if (ds707_pkti_is_dorm_action_allowed(
                                        phys_link_ptr,
                                        DS707_PKT_DORM_ACTION_OK_TO_GO_DORM,
                                        DS707_PKT_DORM_REASON_MIN 
                                        ) == TRUE)
  {
    can_go_dormant = TRUE;
  }
  else
  {
    can_go_dormant = FALSE;
  }

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Tunnel Calls can_go_dormant = %d", 
                  can_go_dormant);

  ds707_pkt_mgr_set_last_tun_call_end_reason(end_reason);

  /* OPTHO_ToDo: If HDR provides a cause code that indicates orig failure,
     then call ended handler should pass that detail to this phys link cleanup
     function. Based on the passed value, we can set failed_orig.*/

  //event_info.failed_orig = FALSE;

  /* Ensure that pkt mgr enables flow for both dormant and non-dormant case in
     tunnel mode (when it receives this DS707_CBACK_TUN_PHYS_LINK_DOWN_EV
     posted here). This is slightly different from the direct mode call. 
     In case of direct mode, if we can go dormant, depending on the CM error
     code (e.g phone in use), we decide to disable flow so Apps dont retry.
     CM then notifies us that it is good to go at which time we enable flow
  */
  ds707_phys_link_disable_flow(DS_FLOW_707_LOCK_MASK);

  ds707_pkti_cleanup_wmks();

  /* Set Going Dormant flag*/
  event_info.going_dormant = can_go_dormant;

  /* Post Tunnel Phys Link Down EV*/
  ds707_cback_post_event(DS707_CBACK_TUN_PHYS_LINK_DOWN_EV, &event_info);
  
  /* Invalidate SO*/
  tc_state_ptr->so = DS707_INVALID_SO;
   
  return;
} /* ds707_pkti_tunnel_phys_link_cleanup */

/*===========================================================================
FUNCTION     DS707_PKTI_TUNNEL_CALL_CONN_HDLR

DESCRIPTION  This handler is called by hdrpac when a 
             HDRIND_IDLE_CONNECTION_OPENED is received while in tunnel mode
             operation

PARAMETERS   None.
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_pkti_tunnel_call_conn_hdlr
(
  void
)
{
  ps_phys_link_type            *phys_link_ptr     = NULL;
  ds707_tc_state_type          *tc_state_ptr      = NULL;
  phys_link_state_type          phys_link_state   = PHYS_LINK_STATE_INVALID;
  ds707_cback_event_info_type   event_info;
  ds707_tun_phys_link_state     tun_phys_link_state = DS707_TUN_PHYS_LINK_NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (hdrcp_stream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY) == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "HDR session not open yet. Ignore tun connection open ind");
    return;
  }

  tc_state_ptr = ds707_get_pri_tc_ptr();
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if ((NULL == tc_state_ptr) || 
      (NULL == phys_link_ptr)
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "tc_state_ptr/phys_link_ptr NULL. Return");
    return;
  }

 
  /* Just printing the PS phys link state for debug purpose in case
     PS phys link state and tun phys link state go out of sync*/
  phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                    "pri phys_link_state: %d", phys_link_state);

  tun_phys_link_state = ds707_get_tun_phys_link_state(tc_state_ptr);
  
  /* OPTHO_ToDo: Sec Phys link states not checked. Add the check when QOS
     is needed for OptHo*/

  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                  "Rcvd Call Connected in tun_phys_link_state %d",
                  tun_phys_link_state);

  switch (tun_phys_link_state)
  {
    case DS707_TUN_PHYS_LINK_DOWN:
    {
      /* Post virtual connection success to VCTM. This is needed to unthrottle
         and move to tunnel allowed state. If s101 SM is not chnaged to
         allowed state, phys link will not move to up state for an incoming
         call, so it is needed for both MO and MT calls*/
      ds707_s101_vctm_process_call_success();

      if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_query_sm_state())
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                        "Posting Tun Incoming Call Ev");
      /* Dummy event_info passed in just to satisfy the cback_post_ev func*/
      ds707_cback_post_event(DS707_CBACK_TUN_INC_CALL_EV,
                             &event_info);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                        "Not posting Tun Incoming Call Ev");
      }
      break;
    }

    case DS707_TUN_PHYS_LINK_COMINGUP:
    {
      /* Post virtual connection success to VCTM. This is needed to unthrottle
         and move to tunnel allowed state. If s101 SM is not chnaged to
         allowed state, phys link will not move to up state for an incoming
         call, so it is needed for both MO and MT calls*/
      ds707_s101_vctm_process_call_success();

      /* tc_state_ptr->so is checked in multiple places. Hence
         hardcode it to be HDR
         e.g: DS_ISDATA_HDR(SO) macro is checked in ds707_pkti_cleanup_rlp
              to confirm TC is in HDR. Else it cleans up 1X RLP
              Also checked in tear down cmd hdlr to send xoff to HDR
      */
      tc_state_ptr->so = CAI_SO_HDR_PKT_DATA;

      /*---------------------------------------------------------------
        Enable data flow for HDR.
      ---------------------------------------------------------------*/
      ds707_pkti_enable_hdr_flow();    

      /* Dummy event_info passed in just to satisfy the cback_post_ev func*/
      ds707_cback_post_event(DS707_CBACK_TUN_PHYS_LINK_UP_EV,
                             &event_info);

      break;
    }

    default:
      /* Call Connected received in undesired tun phys link state */
      break;
  }

} /* ds707_pkti_tunnel_call_conn_hdlr */


/*===========================================================================
FUNCTION     DS707_PKTI_TUNNEL_CALL_ENDED_HDLR

DESCRIPTION  This handler is called by hdrpac when
             HDRIND_CONNECTED_CONNECTION_CLOSED
             is received while in tunnel mode operation

PARAMETERS   None.
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_pkti_tunnel_call_ended_hdlr
(
  uint32 end_reason
)
{
  ds707_tc_state_type          *tc_state_ptr  = NULL;
  ps_phys_link_type            *phys_link_ptr = NULL;
  phys_link_state_type          phys_link_state = PHYS_LINK_STATE_INVALID;
  ds707_tun_phys_link_state     tun_phys_link_state = DS707_TUN_PHYS_LINK_NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  tc_state_ptr = ds707_get_pri_tc_ptr();
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if ((NULL == tc_state_ptr) || 
      (NULL == phys_link_ptr)
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "tc_state_ptr/phys_link_ptr NULL. Return");
    return;
  }

  /* Just printing the PS phys link state for debug purpose in case
     PS phys link state and tun phys link state go out of sync*/
  phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                    "pri phys_link_state: %d", phys_link_state);

  tun_phys_link_state = ds707_get_tun_phys_link_state(tc_state_ptr);
  
  /* OPTHO_ToDo: Sec Phys link states not checked. Add the check when QOS
     is needed for OptHo*/

  switch (tun_phys_link_state)
  {
    case DS707_TUN_PHYS_LINK_COMINGUP:
    {
      /* Since HDR might give connection close while in coming up state
         with no end reason, treat this as a recoverable reason so
         EPC will retry call bringup */
      end_reason = HDRHAI_CONN_FAIL_IDLE_CONN_DENY;
    } /* Fall through */
    case DS707_TUN_PHYS_LINK_UP:
    {
      ds707_pkti_tunnel_phys_link_cleanup
                  (end_reason, tc_state_ptr, phys_link_ptr);
      break;
    }
    default:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "Ignoring call ended ind in current tun phys link state");
      break;
    }
  }
  return;
} /* ds707_pkti_tunnel_call_ended_hdlr */

/*===========================================================================
FUNCTION     DS707_PKTI_TUNNEL_CALL_FAILED_HDLR

DESCRIPTION  This handler is called by hdrpac when
             HDRIND_IDLE_CONNECTION_FAILED
             is received while in tunnel mode operation

PARAMETERS   None.
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_pkti_tunnel_call_failed_hdlr
(
  uint32 end_reason,
  uint64 backoff_time
)
{
  ds707_tc_state_type          *tc_state_ptr  = NULL;
  ps_phys_link_type            *phys_link_ptr = NULL;
  phys_link_state_type          phys_link_state = PHYS_LINK_STATE_INVALID;
  ds707_tun_phys_link_state     tun_phys_link_state = DS707_TUN_PHYS_LINK_NULL;

  tc_state_ptr = ds707_get_pri_tc_ptr();
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if ((NULL == tc_state_ptr) || 
      (NULL == phys_link_ptr)
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "tc_state_ptr/phys_link_ptr NULL. Return");
    return;
  }

  /* Just printing the PS phys link state for debug purpose in case
     PS phys link state and tun phys link state go out of sync*/
  phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                    "pri phys_link_state: %d", phys_link_state);

  tun_phys_link_state = ds707_get_tun_phys_link_state(tc_state_ptr);

  /* OPTHO_ToDo: Sec Phys link states not checked. Add the check when QOS
     is needed for OptHo*/

  switch (tun_phys_link_state)
  {
    case DS707_TUN_PHYS_LINK_COMINGUP:
    {
      /* Send backoff_time value to ds707_s101.c
         to trigger virt_conn_throttling*/
      ds707_s101_vctm_process_call_failure();
      ds707_pkti_tunnel_phys_link_cleanup
                     (end_reason, tc_state_ptr, phys_link_ptr);
      break;
    }
    default:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "Ignoring call failed ind in current phys link state");
      break;
    }
  }
  return;
} /* ds707_pkti_tunnel_call_failed_hdlr */

#endif /* FEATURE_DATA_OPTHO */

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
)
{
  ps_phys_link_type            *phys_link_ptr = NULL;
  ds707_cback_event_info_type  event_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "DDS Switch Handler. New subs ID is %d.", 
                  new_subs_id);

  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  /*---------------------------------------------------------------------
    Abort dormant data call as DDS changed.
  ---------------------------------------------------------------------*/
  if(ds707_pkt_is_dormant(phys_link_ptr) == TRUE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "DDS Changed. Abort dormant packet call." );

    event_info.curr_network = ds707_current_data_sess_network;

    /*---------------------------------------------------------------------
      Update last data sess network to what the network was when aborting 
      the call. It will be updated to NO_SRV while the call goes NULL. The 
      update to NO_SRV happens while processing the ABORT_SESSION Event.
    ---------------------------------------------------------------------*/
    ds707_pkt_set_last_data_sess_network(ds707_current_data_sess_network);
    ds707_cback_post_event(DS707_CBACK_ABORT_SESSION_EV, &event_info);
  }
  else
  {
    /*---------------------------------------------------------------------
      CM will send call end for non-dormant active data call when DDS 
      changes and so existing call will be cleaned up as part of call end. 
      Hence we need not have to take any action here for non-dormant data 
      call.
    ---------------------------------------------------------------------*/
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "No dormant data call." );
  }
}

/*===========================================================================
FUNCTION     DS707_SET_LAST_GOOD_SYS

DESCRIPTION
  Sets the last known good sytem

PARAMETERS
  ds707_sys_info_enum_type - last known good system

RETURN VALUE
  None
===========================================================================*/
void ds707_set_last_good_sys
(
  ds707_sys_info_enum_type last_good_sys
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG1(MSG_LEGACY_MED, "set last_good_sys = %d", last_good_sys);
  ds707_last_good_sys = last_good_sys;
} /* ds707_set_last_good_sys() */

/*===========================================================================
FUNCTION     DS707_GET_LAST_GOOD_SYS

DESCRIPTION
  Gets the last known good sytem

PARAMETERS
  None

RETURN VALUE
  ds707_sys_info_enum_type - last known good system
===========================================================================*/
ds707_sys_info_enum_type ds707_get_last_good_sys
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_IS707_MSG1(MSG_LEGACY_MED, "get last_good_sys = %d", ds707_last_good_sys);
  return ds707_last_good_sys;
} /* ds707_get_last_good_sys() */

/*===========================================================================
FUNCTION     ds707_clear_1x_rateset

DESCRIPTION
  Gets the last known good sytem

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
void ds707_clear_1x_rateset
(
  void
)
{
  ds707_avg_throughput=0;  
  ds707_allowed_throughput=0; 
  ds707_total_rx_bytes=0;  
  
} /* ds707_clear_1x_rateset() */

/*===========================================================================
FUNCTION      DS707_FREE_THROUGHPUT_REPORT

DESCRIPTION   This function frees the memory  allocated for the global 
              throughput Information list if already allocated 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_free_throughput_report
(
   void
)
{
  ps_sys_throughput_info_list_type   *report_p   = NULL;
  /*- - - - - - - -  - - - - - - - - - - - - -  - - - - - - - - - - - - - - */

  report_p = ds_707_throughput_info_ind[0].report_p;


  /*------------------------------------------------------------------------- 
    No need to free if the report is already freed
    -------------------------------------------------------------------------*/
  if (report_p == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Ptr to tput info report already NULL");
    return;
  }
  /*------------------------------------------------------------------------- 
    Now free the memory for the report
    -------------------------------------------------------------------------*/
  modem_mem_free((void *)report_p, MODEM_MEM_CLIENT_DATA);
  ds_707_throughput_info_ind[0].report_p            = NULL;
  ds_707_throughput_info_ind[0].max_report_size     = 0;

  return;
} /*ds707_free_throughput_report*/

/*===========================================================================
FUNCTION      DS707_ALLOCATE_THROUGHPUT_INFO

DESCRIPTION   This function invokes the mode handlers to Fill the throughput 
              Info and also sends 'Throughput Info Indication' to PS
              
DEPENDENCIES  None

RETURN VALUE  Error on return

SIDE EFFECTS  None
===========================================================================*/
int16 ds707_allocate_throughput_info
(
   uint16 num_ifaces_in_use
)
{
  uint16                            num_bytes                = 0;
  int16                             dss_errno                = 0;
  ps_sys_throughput_info_list_type  *report_p             = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  /*------------------------------------------------------------------------- 
    Check if memory was already allocated
    -------------------------------------------------------------------------*/
  report_p = ds_707_throughput_info_ind[0].report_p;
  
  if (report_p != NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Already allocated. Check again");
    dss_errno = -1;
    return dss_errno;
  }

  /*------------------------------------------------------------------------- 
    calculate the number of bytes required
    -------------------------------------------------------------------------*/
  num_bytes = sizeof(ps_sys_throughput_info_list_type)            + 
             (num_ifaces_in_use * sizeof(ps_sys_throughput_info_type));

  DATA_IS707_MSG2(MSG_LEGACY_ERROR, "Allocating %d bytes for max %d "
                                      "iface/blocks",num_bytes,
                                       num_ifaces_in_use);

  report_p = (ps_sys_throughput_info_list_type *)
  modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_DATA);
  
  /*------------------------------------------------------------------------- 
    return if the memory allocation failed
    -------------------------------------------------------------------------*/
  if(report_p == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Mem alloc failure for Iface Info list."
                                      "Cannot send tput Info Ind");
    dss_errno = -1;
    return dss_errno;
  }

   /*------------------------------------------------------------------------- 
    Initialize the memory again before filling in the report. Check this again
    -------------------------------------------------------------------------*/
  memset(report_p, 0, num_bytes); 

  DATA_IS707_MSG1(MSG_LEGACY_MED, "Memory available for max"
                                    " %d ifaces",num_ifaces_in_use);
  ds_707_throughput_info_ind[0].report_p = report_p;
  ds_707_throughput_info_ind[0].max_report_size = num_ifaces_in_use;

  dss_errno = 1;
  return dss_errno;
} /* ds707_allocate_throughput_info*/

/*===========================================================================
FUNCTION  DS707_REPORT_IFACE_THROUGHPUT_INFO_FOR_IFACE

DESCRIPTION 
 For the given iface, this utility function reports the throughput stats

PARAMETERS 
  
  iface_p         - Input PS iface pointer
 
  iface_info_list - Location to fill in iface throughput related stats
 
  ds707_rate_info - Holds the rate information
DEPENDENCIES 
  This utility function will be used only when
  * Iface is active
        &
  * Throughput Info Indication timer is running
RETURN VALUE
  TRUE   - If the throughput Info for the iface is reported successfully
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds707_report_throughput_info_for_iface
(
   ps_iface_type                 *ps_iface_p,
   ps_sys_throughput_info_type   *iface_info_list_p,
   ds707_rate_info               hdr_rl_rate
)
{
  boolean                                ret_val               = FALSE;
  ps_sys_ip_addr_enum_type               iface_ip_addr         = DS_SYS_IP_TYPE_V4;
  boolean                                is_v4                 = FALSE;
  ds3g_apn_name_type                     apn_string = {0};
  ps_sys_throughput_quality_enum_type    throughput_quality    = 
    DS_SYS_HIGHER_THROUGHPUT_UNKNOWN; 
  ds707_data_session_profile_info_type   *data_sess_profile_info_ptr = NULL;
  sys_sys_mode_e_type                    current_mode          = SYS_SYS_MODE_NONE;
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*------------------------------------------------------------------------- 
    Validate Input arguments
    -------------------------------------------------------------------------*/
  if (ps_iface_p == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Not a valid iface ptr. Cannot fill throughput report");
    return ret_val;
  }

  if (iface_info_list_p == NULL)
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,"NULL Iface Info List ptr. Cannot fill throughput report for "
                     "iface 0x%x",ps_iface_p);
    return ret_val;
  }

  
  /*------------------------------------------------------------------------- 
    Get the current mode, if it is HDR, then fill the throughput quality
    returned by HDR. For CDMA, throughput quality is UNKNOWN
    -------------------------------------------------------------------------*/
  current_mode = ds3g_get_current_preferred_mode();
  
  if (current_mode == SYS_SYS_MODE_HDR)
  {
    if(hdr_rl_rate.plus_flag == TRUE)
    {
      throughput_quality = DS_SYS_HIGHER_THROUGHPUT_POSSIBLE;
    }
    else
    {
      throughput_quality = DS_SYS_HIGHER_THROUGHPUT_NOT_POSSIBLE;
    }
  }

  else if (current_mode == SYS_SYS_MODE_CDMA)
  {
    throughput_quality = DS_SYS_HIGHER_THROUGHPUT_UNKNOWN;
  }

  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Not a valid SYS MODE. Cannot fill throughput report");
    return ret_val;
  }
 
  /*------------------------------------------------------------------------- 
    Fill APN name only if it exists. In error case, we will have memset 0 
   -------------------------------------------------------------------------*/
  if (FALSE == ds707_data_session_get_iface_current_profile(
                                                   ps_iface_p,
                                                   &data_sess_profile_info_ptr))
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "failed to get data profile ");
    
  }

  if( ds707_data_session_get_profile_apn_name(data_sess_profile_info_ptr, &apn_string) == FALSE )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                     "failed to get apn name for "
                     "profile");
  }
  else
  {
    (void)strlcpy(iface_info_list_p->apn_string,
                    apn_string.apn_string, 
                    DS_SYS_MAX_APN_LEN);
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, "Iface 0x%x APN name:%s",
                            iface_info_list_p->iface_ptr,
                            iface_info_list_p->apn_string
                            );
   }

  /*------------------------------------------------------------------------- 
    Fill Iface and address family
    -------------------------------------------------------------------------*/
  is_v4 = ps_iface_addr_family_is_v4(ps_iface_p);
  iface_ip_addr = (is_v4 == TRUE) ?DS_SYS_IP_TYPE_V4:DS_SYS_IP_TYPE_V6;

  iface_info_list_p->iface_ptr  =  ps_iface_p;
  iface_info_list_p->ip_type    =  iface_ip_addr;
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "Iface 0x%x IP Type:%d",iface_info_list_p->iface_ptr,
                     iface_info_list_p->ip_type);
  /*------------------------------------------------------------------------- 
    TECH TYPE
    -------------------------------------------------------------------------*/
  iface_info_list_p->tech_type  =  DS_SYS_TECH_3GPP2;

  /*------------------------------------------------------------------------- 
    SUBSCRIPTION ID Harcoded to 0 as that is the only valid id for 3GPP2
    -------------------------------------------------------------------------*/
  iface_info_list_p->subscription = DS_SYS_PRIMARY_SUBS;
  
  /*------------------------------------------------------------------------- 
    BEARER INFO LIST is none at this point
    -------------------------------------------------------------------------*/
  iface_info_list_p->bearer_throughput_info_list.num_bearers = 1;
  iface_info_list_p->bearer_throughput_info_list.bearer_trpt_info[0].
    is_default_bearer = 1;
  iface_info_list_p->bearer_throughput_info_list.bearer_trpt_info[0].
    bearer_rlp_mac_id = 0;
  iface_info_list_p->bearer_throughput_info_list.bearer_trpt_info[0].
    bearer_uplink_actual_rate = hdr_rl_rate.curr_rl_tput;
  iface_info_list_p->bearer_throughput_info_list.bearer_trpt_info[0].
    bearer_uplink_queue_size = hdr_rl_rate.q_len;
  
  /*------------------------------------------------------------------------- 
    AGGREGATE ACTUAL THROUGHPUT/QUEUE SIZE
    -------------------------------------------------------------------------*/
  iface_info_list_p->uplink_actual_rate = hdr_rl_rate.curr_rl_tput;
  iface_info_list_p->uplink_allowed_rate  = hdr_rl_rate.est_rl_tput;
  iface_info_list_p->uplink_queue_size  = hdr_rl_rate.q_len;

   
  /*------------------------------------------------------------------------- 
    THROUGHPUT QUALITY
    -------------------------------------------------------------------------*/
  iface_info_list_p->throughput_quality = throughput_quality;
  
  
  ret_val = TRUE;

  return ret_val;
} /*ds707_report_throughput_info_for_iface*/


/*===========================================================================
FUNCTION  DS_707_REPORT_ALL_IFACE_THROUGHPUT_INFO

DESCRIPTION 
 This Utility function reports the the throughput Information for all
 active DS 3GPP2 ifaces

PARAMETERS 
  iface_info_list - Location to fill in iface throughput related stats
  num_max_ifaces  - The throughput info for Max num of ifaces,
                    the info list can hold
                    
DEPENDENCIES 
  This utility function will be used only when
  * there is atleast one active DS 3GPP2 Iface
        &
  * Throughput Info Indication timer is running
RETURN VALUE
  TRUE   - If the throughput Info for the iface is reported successfully
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds707_report_all_iface_throughput_info
(
   ps_sys_throughput_info_list_type *report_p,
   uint16 num_max_ifaces
)
{
  uint8                             index                           = 0;
  boolean                           ret_val                         = FALSE;
  ps_iface_type                     *ds_iface_p = NULL;
  sys_sys_mode_e_type               current_mode = SYS_SYS_MODE_NONE;
  ds707_rate_info                   hdr_rl_rate={0};
  uint8                             num_trpt_info=0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate Arguments
    -------------------------------------------------------------------------*/
  if ((report_p == NULL)|| (num_max_ifaces ==0))
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"No memory available to report throughput list for active"
                       "DS 3GPP2 ifaces");
    return ret_val;
  }
  current_mode = ds3g_get_current_preferred_mode();

  if(current_mode == SYS_SYS_MODE_HDR)
  {
    if(!(E_SUCCESS == hdrl1_rmac_get_rl_rate_info(&hdr_rl_rate)))
    { 
      DATA_IS707_MSG0(MSG_LEGACY_MED, 
                      "Error return from HDR layer. Cannot fill throughput report");
      
      return ret_val;
    }
  }
  else if (current_mode == SYS_SYS_MODE_CDMA)
  {
    hdr_rl_rate.curr_rl_tput = ds707_avg_throughput;
    hdr_rl_rate.est_rl_tput = ds707_allowed_throughput;
    hdr_rl_rate.q_len = (uint32)ds707_get_current_tx_wmk_cnt();
    
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, 
                      "Non-C2K mode. Do not report");
    return ret_val;
  }

  DATA_IS707_MSG3( MSG_LEGACY_HIGH,
		"Filling curr_rl_tput %d est_rl_tput: %d tot_rx_bytes: %d",
                     hdr_rl_rate.curr_rl_tput,
                     hdr_rl_rate.est_rl_tput,
                     hdr_rl_rate.q_len);

  /*--------------------------------------------------------------------------- 
      Initialize the number of elements in the List to 0
   ---------------------------------------------------------------------------*/
  report_p->num_trpt_info = 0;
  
  for (index = 0; index < DS707_MAX_IFACES;index++)
  {
    if ((report_p->num_trpt_info >= num_max_ifaces))
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Memory filled for %d iface's "
                                      "ready to report",index);
      break;
    }
 
    ds_iface_p = NULL;
    /* Get the iface pointer for the corresponding index*/
    ds_iface_p = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index((uint8)index);

    /*----------------------------------------------------------------------- 
      Consider Iface as valid if there is a PDN context associated with the
      DS iface
      -----------------------------------------------------------------------*/
    if ((ds_iface_p == NULL)||
        (ds707_pkt_is_um_iface_up(ds_iface_p) == FALSE) )

    {
      continue;
    }
    num_trpt_info = report_p->num_trpt_info;
    /*----------------------------------------------------------------------- 
      Valid DS iface. Report Throughput Info for this iface
      -----------------------------------------------------------------------*/
    if (FALSE == ds707_report_throughput_info_for_iface(ds_iface_p,
                                                 &report_p->trpt_info[num_trpt_info], hdr_rl_rate))
    {
      continue;
    }

      /*--------------------------------------------------------------------- 
        Report generated for the given iface. Move the pointer to fill in
        the throughput stats for the next active DS iface
        ---------------------------------------------------------------------*/
  
    
    report_p->num_trpt_info++;
    ret_val = TRUE;
  }

   /*Do the check for ifaces and return error if */
  return ret_val;
} /* ds707_report_all_iface_throughput_info*/

/*===========================================================================
FUNCTION ds707_get_throughput_from_rpc

DESCRIPTION
  Depending on RPC rating, this function returns the throughput in Kbps

PARAMETERS
  scrm_sch_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  UINT32 
===========================================================================*/
uint32 ds707_get_throughput_from_rpc
(
  enc_sch_rate_type ds707_rpc_rating
)
{
  uint32 rate=0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  switch (ds707_rpc_rating)
  {
     case ENC_SCH_8TH_RATE:
       rate = 2;
       break;

     case ENC_SCH_QTR_RATE:
       rate = 3;
       break;

     case ENC_SCH_HALF_RATE:
       rate = 5;
       break;

     case ENC_SCH_FULL_RATE:
       rate = 10;
       break;

       case ENC_SCH_2X_RATE:
       rate = 19;
       break;

     case ENC_SCH_4X_RATE:
       rate = 38;
       break;

     case ENC_SCH_8X_RATE:
       rate = 77;
       break;

     case ENC_SCH_16X_RATE:
       rate = 154;
       break;

     default:
       rate = 0;
       //MSG PRINT
       break;
  }

  return rate;
} /* ds707_get_throughput_from_rpc */


/*===========================================================================
FUNCTION DS707_GET_THROUGHPUT_INFO

DESCRIPTION
  Depending on the current network mode, this function gets throughput
  information from the lower layers and generates THROUGHPUT_INFO_EVENT to
  PS framework

PARAMETERS
  Subscriber id.

DEPENDENCIES
  Call should be up.

RETURN VALUE
  TRUE - Success
  FALSE - Failure
===========================================================================*/
void ds707_get_throughput_info
(
  ds3gsubsmgr_subs_id_e_type subs_id /*Use ds3gsubs_type*/  
)
{
  uint8                       num_ifaces_in_use         = 0;
  int16                        dss_errno                 = 0;
  sys_sys_mode_e_type          current_mode   = SYS_SYS_MODE_NONE;
  ps_sys_throughput_info_list_type *report_p           = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  current_mode = ds3g_get_current_network_mode();
  /*------------------------------------------------------------------------- 
    Check the Sys Mode For Non-C2K Sys mode do not fill and return error
   -------------------------------------------------------------------------*/
  if (! ((current_mode == SYS_SYS_MODE_CDMA) || 
         (current_mode == SYS_SYS_MODE_HDR)) )
  {
    return;
  }

  /*------------------------------------------------------------------------- 
    Check the passed subs_id. For C2K if subs_id is not 0, then it is invalid.
    Do not calculate throughput for invalid subs_id
   -------------------------------------------------------------------------*/
  if ((subs_id != DS3GSUBSMGR_SUBS_ID_MIN)) /*use ds3gsubs enum*/
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "No active C2K subs id. Ignore generating"
                                    "tput Info Ind");
  
    return;
  }
  /*------------------------------------------------------------------------- 
    Determine the number of ifaces being used depending on the current
    system mode and if there are no active ifaces, then do not calculate
    the throughput info. 
  -------------------------------------------------------------------------*/
  num_ifaces_in_use = ds707_get_num_active_ds_ifaces();
  if (num_ifaces_in_use == 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "No active DS ifaces. Ignore generating"
                                    "tput Info Ind");
    return;
  }
  /*------------------------------------------------------------------------- 
    Check for the allocated memory being sufficient for filling throughput info
    for the active ifaces. If not then allocate memory
   -------------------------------------------------------------------------*/
  if(ds707_allocate_throughput_info(num_ifaces_in_use) < 0)
  {

     DATA_IS707_MSG0(MSG_LEGACY_MED, "Memory allocation failures. "
                                     "Ignore generating put Info Ind");
     ds707_free_throughput_report();
     return;
  }

  report_p = ds_707_throughput_info_ind[0].report_p;

  if (report_p == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Ptr NULL: Ignore generating put Info Ind");
    return;
  } 
  /*------------------------------------------------------------------------- 
    Atleast one DS iface is active/in use. Memory allocation is successful.
    Report throughput info by filling it first.
    -------------------------------------------------------------------------*/
  if(TRUE == 
     ds707_report_all_iface_throughput_info(report_p, num_ifaces_in_use))
  {
    /*---------------------------------------------------------------------
      Provide throughput info to PS
    ---------------------------------------------------------------------*/
    /*------------------------------------------------------------------------- 
      Evaluate if the indication can be sent
     -------------------------------------------------------------------------*/
    if(report_p->num_trpt_info == 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Info List generated is empty");
      return;
    }
    /*------------------------------------------------------------------------- 
    Send Indication to PS
    -------------------------------------------------------------------------*/
    if (0 > ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                             PS_SYS_CONF_THROUGHPUT_INFO,
            (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_ds3g_to_ds(subs_id),
                             report_p,
                            &dss_errno))
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sending Throughput Info Ind failed");
    }
  }
  /*---------------------------------------------------------------------
      Free the memory List, as the event is sent or failed
    ---------------------------------------------------------------------*/
  ds707_free_throughput_report();
  
  
  return;
} /* ds707_get_throughput_info */


/*===========================================================================
FUNCTION ds707_calculate_actual_throughput

DESCRIPTION
  Depending on the current network mode, this function gets throughput
  information from the lower layers and generates THROUGHPUT_INFO_EVENT to
  PS framework

PARAMETERS
  Sampling Interval - Unit in ms.

DEPENDENCIES
  Call should be up.

RETURN VALUE
  TRUE - Success
  FALSE - Failure
===========================================================================*/
void ds707_calculate_actual_throughput
(
  ds3gsubsmgr_subs_id_e_type subs_id, 
  uint16 timer_expiry_count
)
{

  sys_sys_mode_e_type          current_mode   = SYS_SYS_MODE_NONE;
  /*Defaulting to ds707_rpc_rating as ENC_SCH_16X_RATE as suggested by 1x Team*/
  enc_sch_rate_type    ds707_rpc_rating   = ENC_SCH_16X_RATE; 
  uint32               duration           = 0;
  uint32               new_avg_throughput = 0; /*In kbps*/
  uint64               cur_rx_bytes       = 0;
  uint32               sample_interval    = 0;
  const uint8          MOVING_AVG_COEFF   = 8;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
 
    /*------------------------------------------------------------------------- 
    Check the passed subs_id. For C2K if subs_id is not 0, then it is invalid.
    Do not calculate throughput for invalid subs_id
   -------------------------------------------------------------------------*/

 if ((subs_id != DS3GSUBSMGR_SUBS_ID_MIN)) /*use ds3gsubs enum*/
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "No active C2K subs id. Ignore generating"
                                    "tput Info Ind");
  
    return;
  }
 
   /*------------------------------------------------------------------------- 
    Get the periodic refresh interval
    -------------------------------------------------------------------------*/
  duration     = ds3gi_get_actual_throughput_refresh_interval(subs_id);

  if (duration == 0)
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "act t'put: DS3G subs id %d timer "
                                      "exp interval 0.",subs_id);
    return;
  }

   /*------------------------------------------------------------------------- 
    calculate sampling interval .Skip refreshing if the value is 0
    -------------------------------------------------------------------------*/
  sample_interval = (duration * timer_expiry_count);

  DATA_IS707_MSG1(MSG_LEGACY_MED,"act t'put: periodicity %d msec",
                  sample_interval);
   

  current_mode = ds3g_get_current_preferred_mode();

  if(current_mode == SYS_SYS_MODE_CDMA)
  {
    
    cur_rx_bytes = ds707_get_total_tx_wmk_cnt();
   
    new_avg_throughput = ds_util_calculate_throughput_moving_avg
                                              (
                                                 ds707_avg_throughput,
                                                 ds707_total_rx_bytes,
                                                 cur_rx_bytes,
                                                 (uint64)sample_interval,
                                                 MOVING_AVG_COEFF
                                              );
    DATA_IS707_MSG2(MSG_LEGACY_MED," Prev thrput %d  New thrput %d",
                  ds707_avg_throughput, new_avg_throughput);
    
    ds707_avg_throughput = new_avg_throughput;
    
    /*Update the global variables with latest values*/
    ds707_total_rx_bytes = cur_rx_bytes;
    /*Call 1x API for updating the scrm rating*/
    ds707_rpc_rating = rpc_get_sch_scrm_rate(ds707_rpc_rating);
    ds707_allowed_throughput = ds707_get_throughput_from_rpc(ds707_rpc_rating);

   }

  return;
} /* ds707_calculate_actual_throughput */


/*===========================================================================
FUNCTION ds707_cost_of_modem_power

DESCRIPTION
  Depending on the current network mode, this function gets throughput
  information from the lower layers and generates THROUGHPUT_INFO_EVENT to
  PS framework

PARAMETERS
  Sampling Interval - Unit in ms.

DEPENDENCIES
  Call should be up.

RETURN VALUE
  TRUE - Success
  FALSE - Failure
===========================================================================*/

int16 ds707_cost_of_modem_power
(
  void               *argval_ptr,
  int16              *ps_errno
)
{

  
  sys_sys_mode_e_type                  current_mode =      SYS_SYS_MODE_NONE;
  ds707_modem_power_1x                 tx_power_1x =       TXC_POWER_LOW;
  ds707_modem_power_hdr                tx_power_hdr;
  ps_sys_modem_power_cost_enum_type    *cost_modem_pwr_p = NULL;
  int16 ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  if (ps_errno == NULL) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Invalid ps_errno ptr, fail ioctl");
    return ret_val;
  }

  if ( argval_ptr == NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                       "ds707_cost_of_modem_power(): "
                       "argval_ptr passed in is NULL");
    *ps_errno = DS_EFAULT;
    return ret_val;
  }

  cost_modem_pwr_p = (ps_sys_modem_power_cost_enum_type *)argval_ptr;
  current_mode = ds3g_get_current_preferred_mode();

  if(current_mode == SYS_SYS_MODE_CDMA)
  {
    tx_power_1x = txc_get_cost_of_modem_power(); 
    switch (tx_power_1x)
    {
       case TXC_POWER_LOW:
         *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_LOW;
         break;

       case TXC_POWER_MED:
         *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_MEDIUM;
         break;

       case TXC_POWER_HIGH:
         *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_HIGH;
         break;

       default:
         *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_UNKNOWN;
         break;
    }
  }
  else if (current_mode == SYS_SYS_MODE_HDR)
  {
    hdrl1_lmac_get_modem_api_data(&tx_power_hdr); 
   
    switch (tx_power_hdr.cost_of_modem_power)
    {
       case HDRLMAC_EST_COST_OF_POWER_IS_LOW:
         *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_LOW;
         break;

       case HDRLMAC_EST_COST_OF_POWER_IS_MEDIUM:
         *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_MEDIUM;
         break;

       case HDRLMAC_EST_COST_OF_POWER_IS_HIGH:
         *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_HIGH;
         break;

       default:
         *cost_modem_pwr_p = DS_SYS_MODEM_POWER_COST_UNKNOWN;
         break;
    }
    
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Sys mode not correct to return Modem "
                                    "Power");
    *ps_errno = DS_EFAULT;
    return ret_val;
  }

  *ps_errno = DS_ENOERR;
  ret_val = 0;
 
  return ret_val;
} /* ds707_cost_of_modem_power */

/*===========================================================================
FUNCTION  DS707_COEX_DATA_FLOW_ENABLE_HDLR

DESCRIPTION
  This function enable PS flows for a subscription with COEX mask.

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_coex_data_flow_enable_hdlr
(
  sys_modem_as_id_e_type  subs_id
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Send PS phys link enable for all bearers for the corresponding subs id
  -----------------------------------------------------------------------*/
  if(subs_id == SYS_MODEM_AS_ID_1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Enable Phys link with COEX MASK");
    ds707_phys_link_enable_flow( DS_FLOW_COEX_MASK );
  }
  return;
} /* ds707_coex_data_flow_enable_hdlr() */


/*===========================================================================
FUNCTION  DS707_COEX_DATA_FLOW_DISABLE_HDLR

DESCRIPTION
  This function disable PS flows for a subscription with COEX mask.

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_coex_data_flow_disable_hdlr
(
  sys_modem_as_id_e_type  subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Send PS phys link disable for all bearers for the corresponding subs id
  -----------------------------------------------------------------------*/
  if(subs_id == SYS_MODEM_AS_ID_1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Disable Phys link with COEX MASK");
    ds707_phys_link_disable_flow( DS_FLOW_COEX_MASK );
  }
  return;
} /* ds707_coex_data_flow_enable_hdlr() */


/*===========================================================================
FUNCTION  DS707_CONFIG_MODEM_POWER_STATS_HDLR

DESCRIPTION
  This function disable or enabled MODEM power stats by calling lower layer level APIs

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_config_modem_power_stats_hdlr
(
  sys_modem_as_id_e_type  subs_id,
  boolean enable_flag
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Send PS phys link disable for all bearers for the corresponding subs id
  -----------------------------------------------------------------------*/
  if(subs_id == SYS_MODEM_AS_ID_1)
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED," config modem power stats %d",enable_flag);
    hdrl1_config_modem_power_stats( enable_flag );
    txc_config_modem_power_stats(enable_flag);
    //call 1x API to enable/disable
  }
  return;
} /* ds707_coex_data_flow_enable_hdlr() */

/*===========================================================================
FUNCTION  DS707_GET_MODEM_POWER_STATS_HDLR

DESCRIPTION
  This function queries the lower layers and fills in modem power stats back to DS3G framework

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_get_modem_power_stats_hdlr
(
  sys_modem_as_id_e_type  subs_id,
  uint32 tx_power_bin[]
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  uint32 do_tx_power_bin[DS3G_TX_POWER_BIN_MAX]={0};
  uint32 cdma_tx_power_bin[DS3G_TX_POWER_BIN_MAX]={0};
  uint8 i=0;
  /*-----------------------------------------------------------------------
    Send PS phys link disable for all bearers for the corresponding subs id
  -----------------------------------------------------------------------*/
  if(subs_id == SYS_MODEM_AS_ID_1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED," Get modem power stats ");
    hdrl1_get_modem_power_stats( do_tx_power_bin );
    txc_get_modem_power_stats (cdma_tx_power_bin);

    for(i = 0; i < DS3G_TX_POWER_BIN_MAX ; i++)
    {
      tx_power_bin[i] = cdma_tx_power_bin[i] + do_tx_power_bin[i];
      DATA_IS707_MSG2(MSG_LEGACY_MED," Tx modem power stats 1x "
                      "bin %d hdr bin %d",cdma_tx_power_bin[i],
                      do_tx_power_bin[i]);
    }
  }
  return;
} /* ds707_coex_data_flow_enable_hdlr() */


#endif /* FEATURE_DATA_IS707 */

