/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               3 G   D A T A   S E R V I C E S   M A N A G E R

       C A L L   M G R   I N T E R F A C E   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Call Manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds3gi_cm_init() must be called once at startup, to register as a Call
  Manager client.


  Copyright (c) 2001-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gcmif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/15   sd      Fixed throttling info updation on query through QMI WDS. 
02/23/15   sd      Added call_id as a parameter in the new call end command 
                   callback from CM.
02/20/15   sd      Rectified throttle info update handling in CM SS event.
04/03/14   pg      Fixed subs feature mode updation.
04/15/14   sc      Fixed +COPS DSDS issue.
10/23/13   ssb     Added code to handle card refresh cb based on subs id.
09/13/13   tk      ATCoP changes for LTE and DSDS concurrency solution.
08/06/13   sd      Replaced memcpy with memscpy.
06/03/13   sd      ATCoP decoupling changes
05/24/13   sd      TSTS changes for DS3G.
03/27/13   tk      Fixed issues in dual stack commands.
01/11/13   ssb     Supporting DSDx Co-existence with TDS
12/21/12   ssb     Supporting Centralized call control Feature
11/21/12   sd      Feature Dual SIM related changes.
09/25/12   dvk     Merged fix to add support to tear down dormant C2K data
                   call when DDS switch happens.
07/13/12   jz      Support for emergency only mode when mandatory APN is disabled
07/13/12   msh     Remove check for pref mode in ds3gi_cm_call_event_cb
06/19/12   jee     To do graceful teardown when card REFRESH event happens
07/03/12   nd      Removing Incorrect and redundant checks from ds3gi_end_call().
03/27/12   ss      Fixed issue of RAB_REESTAB_IND received before resume ind
                   resulting in error in data path setup.
03/12/12   ss      RPM: Fixed issue of oprt_mode and regd_serv_domain not
                   being set after UE power-up.
03/09/12   msh     Removed FEATURE_DATA_NIKEL
02/01/12   ss      RPM: Fixed of sending ATTACH request again due to PS session
                   initiation after PS ATTACH is ignored by network.
01/17/12   nd      CM global data reduction effort, removing cm_get_user_data().
01/02/12   msh     Coan: Feature cleanup
01/13/12   sa      Added support for CSVT for CSFB targets.
12/26/11   msh     Set   new_sys, new_srv_status to old values in ds3gi_process_cm_ss_srv_chg_cmd
11/24/11   nd      Handling of RAB re-establishment request command error.
11/28/11   vpk     Send DS_707_PKT_SYS_CHG_CMD event only if HDR/CDMA is lost
11/01/11   msh     Corrected Roaming Indication check when eHRPD to HRPD fallback
09/19/11   msh     Fixed compiler warnings
09/14/11   dvk     Global variable clean up.
09/05/11   ss      Changes to support RPM.
09/08/11   ash     Added support for eHRPD to HRPD fallback.
08/18/11   dvk     Fixed Compilation errors
08/17/11   ttv     Added changes to map CM_CALL_END_OFFLINE to
                   PS_NET_DOWN_REASON_OFFLINE.
08/09/11   sha     Global variable clean up.
06/13/11   ttv     Made changes to explicitly distinguish HDR and CDMA modes.
06/13/11   dvk     Merged accessor function to get CM client ID.
03/30/11   sa      Added fix to update the ds_3gpp_sys_mode to be inline
                   with the ds3gi_mode.
03/23/11   sn      Globals cleanup for free floating of tasks.
03/02/11   op      Compiler warning fix
02/28/11   sa      Added support to update DSDS state from PH event info.
02/24/11   ttv     Fixed compilation warnings.
02/16/11   ttv     Added DSDS changes for consolidated profile family.
02/04/11   mg      Process pwroff/lpm mode notification cmd
01/25/11   ss      Fixed the issue of service status variable which is used
                   only in L, W and G is updated while in C.
01/17/11   sa      Add support for CSFB call in service change SS evt from LTE to 1x.
11/22/10   ls      Removed assert() when Origniating the call in CDMA mode
                   but ending call mode is not in CDMA mode
11/22/10   ttv     Added support for DSDS.
10/14/10   sa      Adding debug messages before ASSERT.
09/23/10   ttv     Merged the fix of CSD call termination during LTE attach.
09/09/10   op      Migrated to MSG 2.0 macros
09/03/10   ttv     Merged the fixes of lint critical errors.
07/02/10   nc      Added LTE specific Mode types for sys_mode initialisation.
06/17/10   as      API to access ds3gi_mode for long term and Fusion purposes
06/10/10   bs      Reading the NV for initialising ds3gi_mode instead of the CM API.
06/07/10   ss      Compiler warning fix.
04/14/10   sn      Mainlined GOBI changes.
02/18/10   ps      Added code changes to use modem memory heap for allocation
                   of call control variable.
08/22/08   ua      Added support for N/w initated Qos.
09/22/09   vrk      Merged LTE changes.
08/18/09   ls      When hdr_hybrid is TRUE and hdr_srv_status is
                   SYS_SRV_STATUS_SRV, we still need to determine
                   sys_mode to ensure it is HDR mode
07/27/09   spn     Removed active_hdr_prot from CM_SS_EVENT, and removed
                   registration for CM_SS_EVENT_INFO
06/19/09   ls      Support operation mode change
06/05/09   sa      Fixed SCUDIF issue of not sending SET-UP response
                   for Voice Preferred MT call.
05/12/09   ls      In ds3gi_cm_ss_event_cb(), we shall post IDM change command
                   whenever we get IDM change event
05/05/09   ss      Fixed Compiler Warnings.
04/30/09   ls      Support LPM mode change
03/04/09   sa      AU level CMI modifications.
02/25/09   sn      Fixed off-target lint errors.
12/18/08   ss      Lint Fixes
04/04/09   ms      Added code to handle the pref_mode_change_event from CM
                   and to tear down the call (dormant and active both).
12/12/08   sn      Fixed off-target lint errors.
10/14/08   sn      Fixed lint critical errors.
08/27/08   nc      Fixing compilation issues.
07/09/08   psng    Fixed the issue of Colocated info  posted wrongly
                   when CAM timer is expired
06/12/08   sn      Fixed featurization issue.
04/02/08   sn      Fixed Klockwork errors.
03/26/08   hs      Fixed CR 136980 Problem was because AT was not able to switch calls.
                   ds3gi_find_first_call_in_progress was returning invalid instance. added break statement.
03/14/08   vk      Changes to support GPRS/EGPRS bearer tech reporting and
                   query
01/25/08   vk      Initializing system mode accurately.
01/11/08   vk      Changes to propagate HSPA call status indication
12/18/07   sn      Fixed lint errors.
12/12/07   as      Added support hybrid mode hdr serving system
10/18/07   vrk     Added support for UE initiated dormancy by registering for
                   call event CM_CALL_EVENT_PS_SIG_REL_CNF
08/16/07    as     Posting a SYS_CHANGE_CMD to DS707 upon service change.
06/27/07   ar      Replace UUS callback with remoted cm_get_user_data interface.
06/19/07  ak/squ   Added a new member named call_inst to the call_info structure.
                   The functions DS3GI_PROCESS_CM_CALL_END_CMD and DS3GI_END_CALL
                   check if the Call_id is valid. If not, they use the Call_inst
                   value to end the call
06/06/07   ar      Include CM call ID when posting USER_USER_DATA events.
05/29/07  squ/mga  Added CM_SS_EVENT_HDR_LN_STATUS processing in ds3gi_cm_ss_event_cb
                   Added call back function registration for CM_SS_EVENT_HDR_LN_STATUS
03/28/07   ssr     Fix mis-featurization under FEATURE_DS_CAM
03/28/07   ac      Remove the temporary debug message.
03/20/07   ac      Fix the compiling error when HDR feature is turned off
02/25/07   vd      Added missing return.
02/16/07   ar      Remove need for ds3g_siolib_incoming_cs_call().
02/14/07   vd      Lint Error fixes.
01/10/07   gr      Changes to populate the call end reason related parameters
                   in the event_info correctly while publishing PHYS_LINK_DOWN_EV
01/03/07   vp/sq   Before call ds3g_siolib_incoming_cs_call, check if incoming
                   call a CS call.
11/16/06   sq      Added a cb function to process cm_ph_sys_sel_pref event.
11/01/06   ar      Add inc call check against type limits for concurrent case.
09/27/06   as/gr   Enable flow after receiving CM_CALL_EVENT_END to resume
                   data call upon a failure to start a voice call.
08/28/06   vd      Memset of parameters before invoking mode specific handlers.
06/01/06   ar      Adjust messaging when CM event filtered.
05/22/06   ar      Allocate call instance at SETUP event for ETSI CSD calls.
04/16/06   gr/squ  Added call orig fail reason to the call_end event. The err code
                   will be used to disable phys link flow (if a voice call is up).
2/27/06    sq      Added code to make sure in function ds3gi_manage_terminating_calls
                   the cc_info_ptr->accept_inc_call and cc_info_ptr->action are consistent.
01/19/06   sl      Serializer related changes.
01/17/06   ar      Add check for no service on setup and incoming events.
11/30/05   ar      Perform call end on setup response error from CM.
07/11/05   vrk     Fixed coverity warning
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ack
09/02/05   rc      Added support for Mobile Initiated QOS Modification.
08/26/05   ar      Added support for CS Data incall modification & fallback.
06/30/05   ar      Added support for asynchronous call setup confirmation.
                   Allocate call instance during setup for ETSI CSData calls.
05/20/05   ar      Added support for DS_CM_CALL_PROGRESS_CMD event. Lint
                   corrections.
02/28/04   rr      Added end_type to DS command to distinguish between CM's
                   and Internal call end commands
11/20/04   kvd     Post coloc info changed event on change of SYS_ID in
                   ss_info.  - used for storing coloc info by CAM module.
11/18/04   dwp     Add support for User-User Signalling. ds3gi_cm_uus_event_cb
09/20/04   rc      Feature wrapped power save mode code around
                   FEATURE_PS_DORMANT_PWR_SAVE.
09/17/04   rc      Added function ds3gi_process_cm_ss_ps_data_fail_cmd()
                   to support power save mode operation.
09/10/04   sb      S-PDP changes
05/07/04   vr      Changed ds3gi_chk_incom_call_against_cur_calls to check
                   for the current calls in progress instead of max calls to
                   determine if it is a concurrent call attempt.
04/29/04   gr      Freed the call instance for the Rejected call. This is to
                   ensure that the CALL_END notification from CM gets ignored.
02/13/04   vas     Fixed typo in handling incoming/outgoing call to force out
                   call to end.
12/05/03   vr      When ending a call due to on ORIG failure (via the call-
                   back), make sure to set end_status
10/28/03   ak      Fixed compiler warning.
10/21/03   vr      Changed paramter in call to ds707_pkt_post_idm_change_cmd
                   from sys_mode to cur_idle_digital_mode
10/07/03   vsk     baseline code to support multiple data calls
09/16/03   ak      When ending a call due to an ORIG failure (via the call-
                   back), make sure to set the call mode.
08/27/03   vr      Added support for Idle Digital Mode changed event from CM
                   to support data session handoff between 1X and HDR
08/15/03   ak      For CALL_CONF and CALL_SETUP, do not check to see if
                   CM's call sys_mode is correct.  Need update from CM for
                   this.
08/07/03   ak      On call_end, remove ASSERT when CM says NO_SRV on call
                   mode, as there are multiple conditions when this happens.
08/04/03   ak      Fix for when orig fails - CM says sys_mode = NO_SRV.
07/25/03   ak      When accessing the mode hdlr tbl, use the stored call mode
                   instead of the ds3gi_mode.
05/21/03   vas     Added case from processing a return value of Ignore from
                   an incoming call handler (needed for HDR)
05/21/03   atp     Added fix to protect DS from CP trying to set up a 2nd
                   data call when one data call already active in SVD.
04/18/03   ak      Better msgs on incoming call.  Added param to switch_calls
                   interface
04/09/03   rsl     Changes relating to CM ret. val check.
03/18/03   ak      On incoming call, fixed bug where call_type was not pulled
                   from cmd_ptr, but instead referenced from un-inited array.
02/26/03   rc      Added code to process the ps_data_suspend flag received in
                   the CM_SS_EVENT_SRV_CHANGED Event from Call Manager.
02/12/03   wx      Accommodate cm_mm_call_cmd_end api change and
                   cm_end_params_s_type struct change
02/10/02   ak      Handle all potential sys_mode changes correctly.
02/07/02   ak      Quick hack to handle transitions to AMPS mode.
11/04/02   tmr     Added call_was_connected paramter to call_ended_handler.
                   Also added setting/clearing of call_was_connected field
                   of call info structure.
10/14/02   ak      Added client_id to stored information on CM event cback.
10/14/02   vsk     Store the system mode & call type for an incoming call
                   to be used after the call has ended to get stats
08/19/02   ak      Updated for multiple calls.
07/24/02   atp     Made ALERT_USER a valid return value from mode-specific
                   handler in ds3gi_process_cm_call_incom_cmd().
07/22/02   ak      on incoming call, DO_NOTHING bumps up the call state to
                   WAITING_FOR_CONNECT
07/16/02   ak      Incoming call handler also takes cmd_ptr.
07/01/02   ak      When originating a call, need mode-spec hdlr to fill in
                   the cm_srv_type.
06/05/02   ar      Removed online-command mode SIO watermark preservation
                   variables.
06/05/02   smp     Update ATCOP's service mode whenever the system mode changes.
05/21/02   smp     Added functions to register for and process the system
                   mode change event from CM.
05/16/02   smp     Updates for Multimode CM.
05/10/02   ak      Added ds3g_initiate_call and ds3g_hangup_call.
05/03/02   smp     When a call is ended, CD winking is done only if the call
                   state was ACTIVE, and no result code is returned if the
                   call state was RINGING. Also fixed a merge issue.
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Fixed bug in ds3gi_cm_call_event_cb() where the cmd buffer
                   was not freed in case of invalid call events being recvd.
                   Added checks for call stack type.
03/15/02   smp     Reset the end_result state variable to NO CARRIER in
                   ds3gi_process_cm_call_end_cmd().
03/14/02   smp     The result code returned to the TE when a Data call is
                   ended is now based on a state variable (was previously
                   hard-coded to NO CARRIER).
03/05/02   smp     Added support for CD winking (&C2).
02/26/02   tmr     Added code to handle delay_connect and fixed bug of checking
                   wrong handler function pointer in
                   ds3gi_process_cm_call_connected_cmd()
02/22/02   smp     Save mode-sp. handler sio_rx_func_ptr (to allow for
                   switching between online cmd/data).
02/19/02   rlp     Code review changes.
02/05/02   rlp     Added online-command mode support.
01/11/02   smp     Merged from packet data branch. CM API updates.
12/18/01   smp     Send NO CARRIER result code to the TE when a Data call is
                   ended.
12/14/01   smp     Fix to stop alerting the user when an incoming call is
                   released. Renamed ds3g_ring_serial_port().
11/06/01   smp     Tx watermark was not being registered when switching back
                   to Autodetect upon ending a call.
08/31/01   smp     Updates due to CM name changes.
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/05/01   smp     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

#include "msg.h"
#include "amssassert.h"
#include "err.h"
#include "cm.h"
#include "cm_v.h"
#include "cm_msgr_msg.h"
#include "sio.h"
#include "dsm.h"
#include "ds3gsiolib.h"
#include "ds3gmshif.h"
#include "dsat_v.h"
#include "dsati.h"
#include "ds3gmgr.h"
#include "ds3gcmif.h"
#include "data_err_suppr.h"
#include "data_msg.h"
#include "ds_dsd_apm_ext_i.h"
#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */

#ifdef FEATURE_DATA_WCDMA_CS
#include "dsucsdhdlr.h"
#endif /* FEATURE_DATA_WCDMA_CS */

#ifdef FEATURE_DATA_IS707
#include "ds707_pkt_mgri.h"
#include "ds707_pkt_mgr.h"
#include "ds707_p_rev.h"
#include "ds707_pkt_mgr_hdlr.h"

#ifdef FEATURE_HDR
#include "ds707_pkt_mgr_hdr.h"
#endif /* FEATURE_HDR */
#endif /* FEATURE_DATA_IS707 */

#include "data_err_suppr.h"
#include "modem_mem.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gpp_bearer_context.h"
#include "cm_dualsim.h"
#include "dsumts_subs_mgr.h"
#include "ds3gmmgsdiif.h"
#include "ds3gstkproc.h"
#include "ds3gdsdif.h"
#include "ds3gsubsmgr.h"
#include "ds3gflowmgr.h"
#include "ds3geventmgr.h"
#include "dsmsgr.h"
#include "mc.h"
#include "ps_iface_defs.h"

#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

#ifdef FEATURE_DATA_ANDSF
#include "ds_andsf_api.h"
#endif
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Table listing the number of calls allowed in each mode. The table is
  dependent on the ordering in sys.h which defines sys_sys_mode_e_type
  enumerating the different sys modes.
  Note:
  WCDMA AND GPRS allows concurrent CS + PS calls. One CS & multiple PS calls.
  All other modes, allow only one CS or one PS call.
  NOTE: AMPS,NO_SRV,GPS,GW,WLAN modes have been filled just for completeness
  here.
---------------------------------------------------------------------------*/
#if !( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
#define SYS_MAX_UCSD_CALLS 0
#endif /* (FEATURE_DATA_GCSD) || (FEATURE_DATA_WCDMA_CS) */

#if !((defined(FEATURE_DATA_WCDMA_CS)) || (defined(FEATURE_DATA_GCSD)))
#define SYS_MAX_UCSD_CALLS   0
#endif /* !((defined(FEATURE_DATA_WCDMA_CS)) || (defined(FEATURE_DATA_GCSD))) */

#define UPDATE_DS3G_SUSPEND_FLAG(subs_id,flag) \
  if (ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id)){\
    ds3g_ps_suspend_flag[subs_id] = flag;}

 /*---------------------------------------------------------------------------
  Local typedef for 3G Dsmgr call control related information
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                    accept_inc_call;    /* Accept inc call?       */
  boolean                    switch_inc_call;    /* Switch inc call?       */
  boolean                    force_cur_call_down;/* force cur_call down?   */
  cm_call_id_type            inc_call_id;        /* incoming CM call id    */
  ds3g_incom_e_type          action;             /* action to be taken     */
  uint32                     cur_cs_calls;       /* # CS calls in cur mode */
  uint32                     cur_ps_calls;       /* # ps calls in cur mode */
  uint32                     switch_call_inst;   /* switched call instance */
  cm_ans_params_s_type       ans_params;         /* CM call answer params  */
  cm_end_params_s_type       end_params_inc;     /* CM call end params     */
} ds3gi_call_control_type;

static ds3g_call_limit_e_type ds3gi_call_limit_info[SYS_SYS_MODE_MAX] =
{
  /************ Max Num CS calls    :  Max Num PS calls  : CS+PS allowed? */
  /*0-NO_SRV */
  {1,                   1,                              FALSE},
  /*1-AMPS   */
  {1,                   1,                              FALSE},
  /*2-cdma   */
  {1,                   1,                              FALSE},
  /*3-gsm    */
  {1,                   DS_3GPP_MAX_BEARER_CONTEXT,     TRUE},
  /*4-hdr    */
  {1,                   1,                              FALSE},
  /*5-wcdma  */
  {SYS_MAX_UCSD_CALLS,  DS_3GPP_MAX_BEARER_CONTEXT,     TRUE},
  /*6-gps    */
  {1,                   1,                              FALSE},
  /*7-gw     */
  {SYS_MAX_UCSD_CALLS,  DS_3GPP_MAX_BEARER_CONTEXT,     TRUE},
  /*8-WLAN   */
  {1,                   1,                              FALSE},
#ifdef FEATURE_3GPP_CSFB
  /*9-LTE   */
  {SYS_MAX_UCSD_CALLS,  DS_3GPP_MAX_BEARER_CONTEXT,     TRUE},
#else
  /*9-LTE   */
  {0,                   DS_3GPP_MAX_BEARER_CONTEXT,     FALSE},
#endif /* FEATURE_3GPP_CSFB */
  /*10-GWL   */
  {0,                   0,                              FALSE},
  /*11-tdscdma*/
  {SYS_MAX_UCSD_CALLS,  DS_3GPP_MAX_BEARER_CONTEXT,     TRUE}
};

/*--------------------------------------------------------------------------
  Function ptr to be invoked when LTE enable or disable call to CM fails
--------------------------------------------------------------------------*/
static cm_ph_cmd_cb_f_type *ds3g_cm_ph_cmd_cb_f_ptr = NULL;

/*---------------------------------------------------------------------------
  Flag to cache Emergency callback mode
---------------------------------------------------------------------------*/
static boolean ds3g_cm_ph_state_emerg = FALSE;

/*--------------------------------------------------------------------------- 
  Cache Oprt mode of the phone as advertized by CM
---------------------------------------------------------------------------*/
static sys_oprt_mode_e_type ds_3gpp_cm_ph_info_oprt_mode = {NULL};

/*---------------------------------------------------------------------------
  Variable to store the current plmn_id
---------------------------------------------------------------------------*/
static sys_plmn_id_s_type      ds3gi_plmn_id[DS3G_MAX_SUBS] ;

/*---------------------------------------------------------------------------
  Variable to keep track of roam status from CM at all times.
  Default is 0, which means Roaming is OFF
---------------------------------------------------------------------------*/
static sys_roam_status_e_type  ds3g_3gpp_roam_status[DS3GSUBSMGR_SUBS_ID_MAX] = {0};
#ifdef FEATURE_DATA_IS707
static sys_roam_status_e_type  ds3g_hdr_roam_status[DS3GSUBSMGR_SUBS_ID_MAX] = {0};
static sys_roam_status_e_type  ds3g_cdma_roam_status[DS3GSUBSMGR_SUBS_ID_MAX] = {0};
#endif /* FEATURE_DATA_IS707 */

static boolean ds3g_ps_suspend_flag[DS3G_MAX_SUBS] = {NULL};

/*---------------------------------------------------------------------------
  Variable to hold the subs id mask from CM in case of SUB_NOT_AVAIL.
  This vaiable will always keep track of the latest value from CM. As per CM,
  they will pass only incrememted bit masks (i.e. 01 followed by 11) and
  expect only the most recent one to be returned. Overwritting the previous
  value is ok.
---------------------------------------------------------------------------*/
uint8 ds3gi_sub_not_avail_subs_id_mask = 0xFF;

/*===========================================================================
                    Forward declarations/prototypes
===========================================================================*/
/*===========================================================================
FUNCTION      ds3g_cm_ph_sys_sel_cb

DESCRIPTION   Callback function to be called by CM when system selection
              enable/disable is performed by DS.
              Post a cmd only if an error value is received.

DEPENDENCIES  None

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds3g_cm_ph_sys_sel_cb
(
  void                   *data_block_ptr,
  cm_ph_cmd_e_type        ph_cmd,
  cm_ph_cmd_err_e_type    ph_cmd_err
);

/*===========================================================================
FUNCTION      ds3g_check_if_3gpp_roam_status_changed

DESCRIPTION   This functions checks whether 3gpp Roaming Status has changed

DEPENDENCIES  None

RETURN VALUE  TRUE, if there is a change. FALSE, Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_check_if_3gpp_roam_status_changed
(
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr,
  uint8                            *output_stack_id_p
);

#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION      ds3g_check_if_3gpp2_roam_status_changed

DESCRIPTION   This functions checks whether 3gpp2 Roaming Status has changed

DEPENDENCIES  None

RETURN VALUE  TRUE, if there is a change. FALSE, Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_check_if_3gpp2_roam_status_changed
(
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr,
  uint8                             stack_idx
);
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GI_GET_CALL_MODE

DESCRIPTION   Returns the call mode of the call passed to DS by CM.  Does
              special processing to treat HDR = CDMA.

              If the call mode is irrelevant (GPS, AMPS, etc), then still
              returns that call mode.  The ds3gi_hdlr_tbl should have
              NULL func ptrs for those modes.

DEPENDENCIES  None

RETURN VALUE  The mode of the call.

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3gi_get_call_mode
(
  const ds_cmd_type  *cmd_ptr
)
{
  sys_sys_mode_e_type       ret_mode = SYS_SYS_MODE_NONE;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return ret_mode;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  if( call_info_cmd_ptr->call_mode == SYS_SYS_MODE_HDR )
  {
    ret_mode = SYS_SYS_MODE_CDMA;
  }
  else
  {
    ret_mode = call_info_cmd_ptr->call_mode;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "CM sez call mode is %d", ret_mode);
  return ret_mode;
} /* ds3gi_get_call_mode() */

/*===========================================================================
FUNCTION      DS3GI_UPDATE_3GPP_NETWORK_MODE

DESCRIPTION   This function updates the network mode maintained inside the
              3GPP mode handler.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_update_3gpp_network_mode
(
  sys_sys_mode_e_type     network_mode,
  sys_modem_as_id_e_type  ps_subs_id
)
{
/*-  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined(FEATURE_DATA_LTE) || defined(FEATURE_DS_UMTS_REARCH)
  if ( DS_3GPP_IS_3GPP_CURRENT_MODE(network_mode))
  {
    ds3gpp_update_network_mode(network_mode, ps_subs_id);
  }
#endif /* FEATURE_DATA_LTE || FEATURE_DG_UMTS_REARCH */
   return;
} /* ds3gi_update_3gpp_network_mode() */

/*===========================================================================
FUNCTION      DS3GI_GET_CURR_ACTIVE_CALLS_INFO_IN_MODE

DESCRIPTION   Returns the current number of ps & cs calls in progress/active
              for the mode specified

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_get_curr_active_calls_info_in_mode
(
  uint32             *num_cs_calls,
  uint32             *num_ps_calls,
  sys_sys_mode_e_type call_mode
)
{
  int instance;
  uint32 cs_calls,ps_calls;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cs_calls = 0;
  ps_calls = 0;

  /*----------------------------------------------------------------------
    NOTE: All non-idle calls are assumed to be in ONLY in the CURRENT MODE.
    If any calls in other modes are active, through an error. We cannot
    support multiple modes at the same time. CM should have transitioned all
    calls to the proper state before any mode change
  ----------------------------------------------------------------------*/
  for (instance = 0; instance < DS3GI_MAX_NUM_CALLS; instance++)
  {
    if (ds3gi_call_info[instance].call_state != DS3G_IDLE_CALL_STATE)
    {
      if( call_mode != ds3gi_call_info[instance].call_mode )
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                        "Active calls in other modes: req mode:%d found :%d",
                        call_mode, ds3gi_call_info[instance].call_mode);
        continue;
      }
      switch(ds3gi_call_info[instance].call_type)
      {
      case DS_CALL_TYPE_CKT :
          cs_calls++;
          break;
      case DS_CALL_TYPE_PKT :
          ps_calls++;
          break;
      default:
          DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "invalid call type :%d",
                          ds3gi_call_info[instance].call_type);
          break;
      }
    }
  }

  *num_cs_calls = cs_calls;
  *num_ps_calls = ps_calls;

  return;
}  /* ds3gi_get_curr_active_calls_info() */

/*===========================================================================
FUNCTION      DS3GI_GET_CURR_ACTIVE_CALLS_INFO_NOT_IN_MODE

DESCRIPTION   Returns the current number of ps & cs calls in progress/active
              for all modes other than the one specified

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_get_curr_active_calls_info_not_in_mode
(
  uint32             *num_cs_calls,
  uint32             *num_ps_calls,
  sys_sys_mode_e_type call_mode
)
{
  int instance;
  uint32 cs_calls,ps_calls;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cs_calls = 0;
  ps_calls = 0;

  /*----------------------------------------------------------------------
    NOTE: All non-idle calls are tagged as active
  ----------------------------------------------------------------------*/
  for (instance = 0; instance < DS3GI_MAX_NUM_CALLS; instance++)
  {
    if (ds3gi_call_info[instance].call_state != DS3G_IDLE_CALL_STATE)
    {
      if( call_mode == ds3gi_call_info[instance].call_mode )
      {
        /*----------------------------------------------------------------
        Call of the specified mode is in progress .. skip this
        ----------------------------------------------------------------*/
        continue;
      }
      switch(ds3gi_call_info[instance].call_type)
      {
      case DS_CALL_TYPE_CKT :
          cs_calls++;
          break;
      case DS_CALL_TYPE_PKT :
          ps_calls++;
          break;
      default:
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "invalid call type :%d",
                        ds3gi_call_info[instance].call_type);
        break;
      }
    }
  }

  *num_cs_calls = cs_calls;
  *num_ps_calls = ps_calls;

  return;
}  /* ds3gi_get_curr_active_calls_info_not_in_mode() */

/*===========================================================================
FUNCTION      DS3GI_FILTER_CM_EVENT

DESCRIPTION   Returns whether the Call Manager event should be processed by
              Data Services based on the call configuration.  Multiple configs
              are supported:

              - Call type is CM_CALL_TYPE_CS_DATA or CM_CALL_TYPE_PS_DATA.
                Applies for all modes.

              - Call type is CM_CALL_TYPE_VOICE and either:
                -- Bearer capabiliy 2 and Repeat Indicator is present
                -- DSMgr Call instance was previously allocated.
                Applies for WCDMA mode only.

DEPENDENCIES  None

RETURN VALUE  Boolean indicating event of interest

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_filter_cm_event
(
  const cm_call_event_e_type     call_event,           /* Call event       */
  const cm_mm_call_info_s_type  *call_info_ptr,        /* Call information */
  cm_call_type_e_type           *ds_call_type_ptr      /* Local call type  */
)
{
  boolean result = FALSE;              /* Assume event not to be processed */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (call_info_ptr == NULL) || (ds_call_type_ptr == NULL) )
  {
    DATA_ERR_FATAL("Null pointer for cm_mm_call_info_s_type");
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    Data call and emergency call events are always processed
  -------------------------------------------------------------------------*/
  if( (call_info_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
      (call_info_ptr->call_type == CM_CALL_TYPE_PS_DATA) ||
      (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY) ||
      (call_info_ptr->mode_info.info_type == CM_CALL_MODE_INFO_GW_PS))
  {
    *ds_call_type_ptr = call_info_ptr->call_type;
    result = TRUE;
  }

#if defined(FEATURE_DATA_WCDMA_CS) && defined(FEATURE_MULTIMEDIA_FALLBACK)
  #error code not present
#endif /* FEATURE_DATA_WCDMA_CS && FEATURE_MULTIMEDIA_FALLBACK */

  if( FALSE == result )
  {
    *ds_call_type_ptr = CM_CALL_TYPE_NONE;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,"Filter CM event: result=%d, call_type=%d",
                  result, *ds_call_type_ptr);
  return(result);
} /* ds3gi_filter_cm_event() */

/*===========================================================================
FUNCTION      DS3GI_FIND_FIRST_CALL_IN_PROGRESS

DESCRIPTION   Given a call type, return the index of the first call of that
              type currently in progress ( not in IDLE state).
              If none are in progress,returns DS3GI_INVALID_CALL_INSTANCE

DEPENDENCIES  None

RETURN VALUE  If found, returns the call instance.
              If not found, then returns DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS None
===========================================================================*/
uint32 ds3gi_find_first_call_in_progress
(
  ds_call_type_e_type  call_type,
  sys_sys_mode_e_type  call_mode
)
{
  uint32 instance = DS3GI_INVALID_CALL_INSTANCE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Search for the first call of the specified type & mode in progress
    Call should not be idle state
  -------------------------------------------------------------------------*/
  for (instance = 0; instance < DS3GI_MAX_NUM_CALLS; instance++)
  {
    if ( (ds3gi_call_info[instance].call_state != DS3G_IDLE_CALL_STATE ) &&
         (ds3gi_call_info[instance].call_type == call_type ) &&
         (ds3gi_call_info[instance].call_mode == call_mode)  )
    {
      if(ds3gi_call_info[instance].call_id == CM_CALL_ID_INVALID)
      {
        DATA_ERR_FATAL("Call_id is invalid");
        return DS3GI_INVALID_CALL_INSTANCE;
      }
      break;
    }
  }

  if( instance < DS3GI_MAX_NUM_CALLS )
  {
    return instance;
  }
  else
  {
    return DS3GI_INVALID_CALL_INSTANCE;
  }
} /* ds3gi_find_first_call_in_progress() */

/*===========================================================================
FUNCTION      DS3GI_END_CALL

DESCRIPTION   This function does all the work necessary to end a call. It
              calls the mode-specific call_ended_handler, cleans up the
              Autodetect RX data watermark prior to changing the serial mod
              to Autodetect and sets the state variables to the appropriate
              values.

DEPENDENCIES  This function can be called in any call state, except IDLE.

RETURN VALUE  None

SIDE EFFECTS  Updates the call_state, serial_state and at_state, and
              changes the serial mode to Autodetect.
===========================================================================*/
void ds3gi_end_call
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  sys_sys_mode_e_type       cm_call_mode = SYS_SYS_MODE_NONE;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_ERR_FATAL("ds3gi_end_call has NULL cmd ptr");
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_FATAL, "ds3gi_end_call has unknown call id",
                    call_info_cmd_ptr->call_id);
    ASSERT(0);
    return;
  }

  cm_call_mode = ds3gi_get_call_mode(cmd_ptr);
  if (cm_call_mode == SYS_SYS_MODE_NO_SRV)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3gi_end_call failed - CM says NO SRV mode");
  }
  else if(call_info_cmd_ptr->end_type == DS_CM_CALL_END_EXTERNAL)
  {
    /*
      For GWL, its possible that SysMode is L but call_object is in GW
      and vice-versa. So checking the mode_info.info_type

      In the scenario where
      - DS sends call end request to CM in LTE
      - DS gets Context tranfser ind to W
      - DS receives response to call end in LTE
      It is possible that there is a mismatch in call mode from
      CM and DS perspective. In such a case, we just process the end call
      in whatever mode we are in. No need to perform below checks.
    */
    if(cm_call_mode != ds3gi_call_info[call_inst].call_mode &&
       ds3gi_call_info[call_inst].call_state != DS3G_END_CALL_STATE )
    {

      DATA_3GMGR_MSG3(MSG_LEGACY_ERROR,
                      "CM sys_mode:%d not equal to DSMGR call_mode:%d."
                      "CM mode_info_type:%d",
                      cm_call_mode,
                      ds3gi_call_info[call_inst].call_mode,
                      call_info_cmd_ptr->mode_info.info_type);
      switch(ds3gi_call_info[call_inst].call_mode)
      {
        case SYS_SYS_MODE_GSM:
        case SYS_SYS_MODE_WCDMA:
        case SYS_SYS_MODE_TDS:
          if((call_info_cmd_ptr->mode_info.info_type != CM_CALL_MODE_INFO_GW_PS)&&
             (call_info_cmd_ptr->mode_info.info_type != CM_CALL_MODE_INFO_GW_CS))
          {
            DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                            "DSMGR call_mode:%d CM mode info_type:%d",
                            ds3gi_call_info[call_inst].call_mode,
                            call_info_cmd_ptr->mode_info.info_type);
          }
          break;

        case SYS_SYS_MODE_LTE:
          if(call_info_cmd_ptr->mode_info.info_type != CM_CALL_MODE_INFO_LTE)
          {
            DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                            "DSMGR call_mode:%d CM mode info_type:%d",
                            SYS_SYS_MODE_LTE,
                            call_info_cmd_ptr->mode_info.info_type);
          }
          break;

        default:
          /* If the mode is different, it prints out the error info and cleans up
           * the call after that
           */
          DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                          "ds3gi_call_info.call_mode:%d ds3gi_get_call_mode:%d",
                          ds3gi_call_info[call_inst].call_mode,
                          cm_call_mode);
          break;
      }
    }//call_mode mismatch
    else if(cm_call_mode != ds3gi_call_info[call_inst].call_mode &&
       ds3gi_call_info[call_inst].call_state == DS3G_END_CALL_STATE)
    {
      DATA_3GMGR_MSG2(
         MSG_LEGACY_HIGH,
         "Call mode mismatch while ending call. DS3G call mode:%d CM:%d",
         ds3gi_call_info[call_inst].call_mode,
         cm_call_mode);
    }
  }
  else if(call_info_cmd_ptr->end_type == DS_CM_CALL_END_INTERNAL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "CALL END COMMAND POSTED INTERNALLY BY DSMGR");
  }
  else
  {
    ASSERT(0);
    return;
  }

  /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
  ----------------------------------------------------------------------*/
  hdlr_tbl_ptr =
      ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

  if (hdlr_tbl_ptr == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    call_ended_handler.
  -------------------------------------------------------------------------*/
  if( hdlr_tbl_ptr->call_ended_handler != NULL )
  {
     /*---------------------------------------------------------------------
      Invoke the mode-specific handler function.
      ---------------------------------------------------------------------*/
      hdlr_tbl_ptr->call_ended_handler(
                        cmd_ptr,
                        ds3gi_call_info[call_inst].call_was_connected,
                        call_info_cmd_ptr->subs_id);

  } /* if call_ended_handler != NULL */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Call ended handler not registered");
  }

  /*-------------------------------------------------------------------------
    Free the DS call instance
  -------------------------------------------------------------------------*/
  ds3gi_free_call_instance_record(call_inst);


  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Data call ended for call_id %d",
                  call_info_cmd_ptr->call_id);

  return;
} /* ds3gi_end_call() */

/*===========================================================================
FUNCTION      DS3G_PRINT_PLMN_ID

DESCRIPTION   This function prints the plmn_id in MCC-MNC format.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_print_plmn_id
(
  void  *current_plmn
)
{
  uint8                mcc[3] = {0};
  uint8                mnc[3] = {0};
  sys_plmn_id_s_type  *current_plmn_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(current_plmn == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "PLMN ID pointer is NULL, returning");
    return;
  }

  current_plmn_id = (sys_plmn_id_s_type *)current_plmn;

  mcc[0] = (current_plmn_id->identity[0] & 0x0F);
  mcc[1] = ((current_plmn_id->identity[0] & 0xF0) >> 4);
  mcc[2] = (current_plmn_id->identity[1] & 0x0F);

  mnc[0] = (current_plmn_id->identity[2] & 0x0F);
  mnc[1] = ((current_plmn_id->identity[2] & 0xF0) >> 4);
  mnc[2] = ((current_plmn_id->identity[1] & 0xF0) >> 4);

  DATA_MSG6(MSG_LEGACY_HIGH,"PLMN ID: MCC: %d%d%d MNC: %d%d%d",
            mcc[0], mcc[1], mcc[2],
            mnc[0], mnc[1], mnc[2]);

  return;
} /* ds3g_print_plmn_id() */

/*===========================================================================
FUNCTION      DS3GI_CM_PH_EVENT_CB

DESCRIPTION   CM PH event callback function

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ph_event_cb
(
  cm_ph_event_e_type         ph_event,  /* Event ID              */
  const cm_ph_info_s_type   *event_ptr  /* Pointer to Event info */
)
{
  boolean                   notify = FALSE;
  ds_cmd_type              *cmd_ptr = NULL;
  ds3g_cm_ph_ev_info_type  *ph_ev_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (event_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "NULL event_ptr. return.");
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3gi_cm_ph_event_cb() for event: %d",
                  ph_event);

  switch (ph_event)
  {
    case CM_PH_EVENT_OPRT_MODE:
    case CM_PH_EVENT_SYS_SEL_PREF:
    case CM_PH_EVENT_INFO:
    case CM_PH_EVENT_DUAL_STANDBY_PREF:
    case CM_PH_EVENT_DATA_PRIORITY:
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_CHANGED:
      notify = TRUE;
      break;

    default:
      notify = FALSE;
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Unhandled Event %d", ph_event);
      break;
  }

  if( notify == TRUE )
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_cm_ph_ev_info_type));
    if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
    cmd_ptr->hdr.cmd_id = DS_CMD_CM_PH_EV_INFO;
    ph_ev_info_ptr = (ds3g_cm_ph_ev_info_type*)cmd_ptr->cmd_payload_ptr;

    ph_ev_info_ptr->ph_event           = ph_event;
    ph_ev_info_ptr->as_id              = event_ptr->asubs_id;
    ph_ev_info_ptr->srv_domain_pref    = event_ptr->srv_domain_pref;
    ph_ev_info_ptr->oprt_mode          = event_ptr->oprt_mode;
    ph_ev_info_ptr->mode_pref          = event_ptr->mode_pref;
    ph_ev_info_ptr->ph_state           = event_ptr->ph_state;
    ph_ev_info_ptr->device_mode        = event_ptr->device_mode;
    ph_ev_info_ptr->standby_pref       = event_ptr->standby_pref;
    ph_ev_info_ptr->active_subs        = event_ptr->active_subs;
    ph_ev_info_ptr->default_voice_subs = event_ptr->default_voice_subs;
    ph_ev_info_ptr->default_data_subs  = event_ptr->default_data_subs;
    ph_ev_info_ptr->hybr_pref          = event_ptr->hybr_pref;
    ph_ev_info_ptr->priority_type      = event_ptr->priority_type;

    ds_put_cmd( cmd_ptr );
  }

  return;
}/* ds3gi_cm_ph_event_cb() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION      DS3GI_CM_PH_EVENT_SUB_PREF_CB

DESCRIPTION   A callback function for CM_PH_EVENT_SUBSCRIPTION_PREF_INFO

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ph_event_sub_pref_cb
(
  cm_ph_event_e_type          subs_event,
  const cm_subs_info_s_type  *subs_info_ptr
)
{
  ds_cmd_type            *cmd_ptr = NULL;
  ds3g_cm_sub_info_type  *sub_info_ptr = NULL;
  boolean                 notify_subs_info  = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( subs_info_ptr == NULL )
  {
    DATA_ERR_FATAL("subs_info_ptr = NULL");
  }

  switch ( subs_event )
  {
    case CM_PH_EVENT_SUBSCRIPTION_PREF_INFO:
      DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                      "Received CM_PH_EVENT_SUBSCRIPTION_PREF_INFO. "
                      "Sub [%d], is_default_data[%d], is_default_voice[%d]",
                      subs_info_ptr->subscription_id,
                      subs_info_ptr->is_default_data,
                      subs_info_ptr->is_default_voice);

      notify_subs_info = TRUE;
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Received other cm phone sub events %d",
                      subs_event);
      notify_subs_info = FALSE;
  }

  if ( notify_subs_info == TRUE )
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_cm_sub_info_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
    cmd_ptr->hdr.cmd_id = DS_CMD_CM_PH_EV_SUB_INFO;
    sub_info_ptr = (ds3g_cm_sub_info_type*)cmd_ptr->cmd_payload_ptr;

    /* Copy multi sim subscription info */
    sub_info_ptr->subscription_id  = subs_info_ptr->subscription_id;
    sub_info_ptr->app_id_1x        = subs_info_ptr->app_id_1x;
    sub_info_ptr->session_type_1x  = subs_info_ptr->session_type_1x;
    sub_info_ptr->app_id_gw        = subs_info_ptr->app_id_gw;
    sub_info_ptr->session_type_gw  = subs_info_ptr->session_type_gw;
    sub_info_ptr->is_operational   = subs_info_ptr->is_operational;
    sub_info_ptr->is_priority_subs = subs_info_ptr->is_priority_subs;
    sub_info_ptr->is_default_voice = subs_info_ptr->is_default_voice;
    sub_info_ptr->is_default_data  = subs_info_ptr->is_default_data;
    /* Copy multi sim info required by ATCoP module */
    sub_info_ptr->pref_info        = subs_info_ptr->pref_info;
    sub_info_ptr->is_available_gw  = subs_info_ptr->is_available_gw;
    sub_info_ptr->sub_feature_mode = subs_info_ptr->sub_feature_mode;
    sub_info_ptr->is_subphone_active = subs_info_ptr->is_subphone_active;
    sub_info_ptr->subs_capability    = subs_info_ptr->subs_capability;    
    ds_put_cmd( cmd_ptr );
  }
  return;
}/* ds3gi_cm_ph_event_sub_pref_cb*/

/*===========================================================================
FUNCTION      DS3GI_CM_CALL_EVENT_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr
              whenever a call event occurs that 3G Dsmgr registered for.

              This function sends a command to the DS task, depending on the
              type of call event that occurred.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_call_event_cb
(
  cm_call_event_e_type         call_event,  /* The call event that occurred*/
  const cm_mm_call_info_s_type *call_info_ptr          /* Call information */
)
{
  ds_cmd_type                    *cmd_ptr = NULL;
  ds3g_call_info_cmd_type        *call_info_cmd_ptr = NULL;
  ds_cmd_enum_type                cmd_id = DS_MIN_CMD;
  boolean                         process_event = FALSE;
  cm_call_type_e_type             ds_call_type;
  ds3gdsdif_call_info_cmd_type   *data_block_ptr = NULL;
  sys_modem_as_id_e_type          subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(call_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "call_info_ptr is NULL, returning");
    return;
  }

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3gi_cm_call_event_cb: call_event:%d call_id:%d, call_type:%d",
                  call_event, call_info_ptr->call_id, call_info_ptr->call_type);

  /*-------------------------------------------------------------------------
    DSD to handle only Voice calls, CS Data calls, emergency calls
    and VT calls
  -------------------------------------------------------------------------*/
  if((call_info_ptr->call_type == CM_CALL_TYPE_VOICE)   ||
     (call_info_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
     (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY) ||
     (call_info_ptr->call_type == CM_CALL_TYPE_VT))
  {
    switch(call_event)
    {
      case CM_CALL_EVENT_ORIG:
      case CM_CALL_EVENT_INCOM:
      case CM_CALL_EVENT_CONNECT:
      case CM_CALL_EVENT_HO_COMPLETE:
      case CM_CALL_EVENT_END:
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                        "Voice/CS Data/Emergency call.call_type: %d, "
                        "call_event: %d",
                         call_info_ptr->call_type,call_event);

        /* Get DS command buffer */
        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3gdsdif_call_info_cmd_type));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }

        data_block_ptr = (ds3gdsdif_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

        /*---------------------------------------------------------------------
          Fill in the command parameters and send the command.
        ---------------------------------------------------------------------*/
        data_block_ptr->call_event          = call_event;
        data_block_ptr->call_type          = call_info_ptr->call_type;
        data_block_ptr->call_id            = call_info_ptr->call_id;
        data_block_ptr->call_sys_mode      = call_info_ptr->sys_mode;
        data_block_ptr->call_voice_ho_type = call_info_ptr->voice_ho_type;
        data_block_ptr->subs_id            = call_info_ptr->asubs_id;

        cmd_ptr->hdr.cmd_id        = DS_CMD_DS3GDSDIF_VOICE_CALL_EVENT;
        ds_put_cmd(cmd_ptr);
        break;

      default:
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "Not Voice/CS Data/Emergency call originated or "
                        "connected or incoming or ended. Ignore");
        break;
    }
  }

#ifdef FEATURE_DATA_IS707
  if ((call_event == CM_CALL_EVENT_EXIT_TC) ||
   /*------------------------------------------------------------------
     If the lower layers were not able to successfully establish TC
     for the other type of Service (other than PS/CS Data Call), EXIT_TC
     would not be generated. The flow for the application needs to be
     enabled whenever a CM_CALL_EVENT_END is received.

     Both the CM_CALL_EVENT_END and EXIT_TC event are funneled through
     the same handler (EXIT_TC Handler)
   -----------------------------------------------------------------*/
       ((call_info_ptr->call_type != CM_CALL_TYPE_CS_DATA) &&
        (call_info_ptr->call_type != CM_CALL_TYPE_PS_DATA) &&
        (call_event == CM_CALL_EVENT_END))
     )
  {
   /*------------------------------------------------------------------
    This event is generated only for CDMA TC exit. But still this
    event is handled in ds3g handler because the TC_EXIT event is
    a call event and there can be only one handler registered
    for call events with CM.

    This is called in the CM task context. DS is interested in
    this event. So this function posts a command to the DS command
    buffer.

    Note: The command will be retrived later on from the DS command
    buffer and processing of the command will be done in the DS task
    context.
 -----------------------------------------------------------------*/
    cmd_id = DS_CMD_707_EXIT_TC;
  }

  /* If the call is not a data call (circuit/packet) or emergency call
     then just return */
  else if(
          (call_info_ptr->call_type != CM_CALL_TYPE_CS_DATA) &&
          (call_info_ptr->call_type != CM_CALL_TYPE_PS_DATA) &&
          (call_info_ptr->call_type != CM_CALL_TYPE_EMERGENCY)
         )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Unhandled call_type. call_event %d call_type:%d",
                    call_event,call_info_ptr->call_type);
    return;
  }
#endif /*FEATURE_DATA_IS707 */

  /*-------------------------------------------------------------------------
    We only pay attention to call events related to Data calls
    Circuit Switched (CS) or Packet Switched (PS)
    and ignore all others.
  -------------------------------------------------------------------------*/
  process_event = ds3gi_filter_cm_event( call_event,
                                         call_info_ptr,
                                         &ds_call_type );
  if( TRUE == process_event )
  {
    /*---------------------------------------------------------------------
      Send the appropriate command to the DS Task based on the type of
      event received.
    ---------------------------------------------------------------------*/
    switch( call_event )
    {
      case CM_CALL_EVENT_ORIG:
        cmd_id = DS_CMD_CM_CALL_EV_ORIG;
        break;

      case CM_CALL_EVENT_END:
        cmd_id = DS_CMD_CM_CALL_EV_END;
        break;

      case CM_CALL_EVENT_INCOM:
        cmd_id = DS_CMD_CM_CALL_EV_INCOM;
        break;

#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
      case CM_CALL_EVENT_INCOMING_PAGE:
        cmd_id = DS_CMD_CM_CALL_EV_INCOMING_PAGE;
        break;
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

      case CM_CALL_EVENT_CONNECT:
        cmd_id = DS_CMD_CM_CALL_EV_CONNECT;
        break;

      case CM_CALL_EVENT_SETUP_IND:
        cmd_id = DS_CMD_CM_CALL_EV_SETUP_IND;
        break;

      case CM_CALL_EVENT_PROGRESS_INFO_IND:
        cmd_id = DS_CMD_CM_CALL_EV_PROGRESS_INFO_IND;
        break;

      case CM_CALL_EVENT_CALL_CONF:
        cmd_id = DS_CMD_CM_CALL_EV_CALL_CONF;
        break;

      case CM_CALL_EVENT_PDP_NW_MODIFY_REQ:
        cmd_id = DS_CMD_CM_CALL_EV_PDP_NW_MODIFY_REQ;
        break;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
      #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

      case CM_CALL_EVENT_RAB_REL_IND:
        cmd_id = DS_CMD_CM_CALL_EV_RAB_REL_IND;
        break;

      case CM_CALL_EVENT_RAB_REESTAB_IND:
        cmd_id = DS_CMD_CM_CALL_EV_RAB_REESTAB_IND;
        break;

      case CM_CALL_EVENT_RAB_REESTAB_REJ:
        cmd_id = DS_CMD_CM_CALL_EV_RAB_REESTAB_REJ;
        break;

      case CM_CALL_EVENT_RAB_REESTAB_FAIL :
        cmd_id = DS_CMD_CM_CALL_EV_RAB_REESTAB_FAIL;
        break;

#ifdef FEATURE_UE_INITIATED_DORMANCY
      /*---------------------------------------------------------------------
        Process the incoming notification from CM and pass back the error
        code from RRC. This provides applications with the ability to
        determine if they should attempt forcing dormancy again or not.
      ---------------------------------------------------------------------*/
      case CM_CALL_EVENT_PS_SIG_REL_CNF:
        DATA_MSG2(MSG_LEGACY_LOW,
                  "Rx'ed CM_CALL_EVENT_PS_SIG_REL_CNF call event: %d, "
                  "with error code %d",
                  call_event,
                  call_info_ptr->mode_info.info.gw_ps_call.ps_sig_rel_status);
        cmd_id = DS_CMD_CM_CALL_EV_PS_SIG_REL_CNF;
        break;
#endif /* FEATURE_UE_INITIATED_DORMANCY */

      case CM_CALL_EVENT_PDP_MODIFY_IND :
        cmd_id = DS_CMD_CM_CALL_EV_PDP_MODIFY_IND;
        break;

      case CM_CALL_EVENT_PDP_PROMOTE_IND :
        cmd_id = DS_CMD_CM_CALL_EV_PDP_PROMOTE_IND;
        break;

      case CM_CALL_EVENT_PDP_MODIFY_CONF:
        cmd_id = DS_CMD_CM_CALL_EV_PDP_MODIFY_CONF;
        break;

      case CM_CALL_EVENT_PDP_MODIFY_REJ:
        cmd_id = DS_CMD_CM_CALL_EV_PDP_MODIFY_REJ;
        break;

#ifdef FEATURE_DATA_LTE
      case CM_CALL_EVENT_GET_PDN_CONN_IND:
        cmd_id = DS_CMD_CM_CALL_EV_GET_PDN_CONN_IND;
        break;

      case CM_CALL_EVENT_PDN_CONN_REJ_IND:
        cmd_id = DS_CMD_CM_CALL_EV_PDN_CONN_REJ_IND;
        break;

      case CM_CALL_EVENT_PDN_CONN_FAIL_IND:
        cmd_id = DS_CMD_CM_CALL_EV_PDN_CONN_FAIL_IND;
        break;

      case CM_CALL_EVENT_ACT_BEARER_IND :
        cmd_id = DS_CMD_CM_CALL_EV_ACT_BEARER_IND;
        break;

      case CM_CALL_EVENT_RES_ALLOC_REJ_IND:
        cmd_id = DS_CMD_CM_CALL_EV_BEARER_ALLOC_REJ_IND;
        break;

      case CM_CALL_EVENT_RES_ALLOC_FAIL_IND:
        cmd_id = DS_CMD_CM_CALL_EV_BEARER_ALLOC_FAIL_IND;
        break;

      case CM_CALL_EVENT_BEARER_MOD_REJ_IND:
        cmd_id = DS_CMD_CM_CALL_EV_BEARER_MOD_REJ_IND;
        break;

      case CM_CALL_EVENT_MOD_BEARER_IND:
        cmd_id = DS_CMD_CM_CALL_EV_MOD_BEARER_IND;
        break;
#endif /* FEATURE_DATA_LTE*/

      case CM_CALL_EVENT_USER_DATA_IND :
        DATA_MSG1(MSG_LEGACY_LOW,"Rx'ed CM_CALL_EVENT_USER_DATA_IND call event: %d ",
                  call_event);
        cmd_id = DS_CMD_CM_CALL_EV_USER_DATA_IND;
        break;

      case CM_CALL_EVENT_USER_DATA_CONG_IND :
        DATA_MSG1(MSG_LEGACY_LOW,"Rx'ed CM_CALL_EVENT_USER_DATA_CONG_IND call event: %d ",
                  call_event);
        cmd_id = DS_CMD_CM_CALL_EV_USER_DATA_CONG_IND;
        break;

      case CM_CALL_EVENT_UNBLOCK_ALL_APN:
        cmd_id = DS_CMD_3GPP_PDN_THROT_UNBLOCK_ALL_APN;
        break;

      default:
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "Rx'ed unknown call event: %d", call_event);
        return;
    }
  } /* ( TRUE == process_event ) */
  else
  {
    /* Ignoring event for non-data mode call. */
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Call Event received for a non-data call: %d", call_event);
  }

  if(cmd_id  != DS_MIN_CMD)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_call_info_cmd_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }

    /*-----------------------------------------------------------------------
      Fill in the command parameters and send the command.
    -----------------------------------------------------------------------*/
    cmd_ptr->hdr.cmd_id = cmd_id;
    call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

    call_info_cmd_ptr->call_event    = call_event;
    call_info_cmd_ptr->call_type     = ds_call_type;
    call_info_cmd_ptr->cm_call_type  = call_info_ptr->call_type;
    call_info_cmd_ptr->call_id       = call_info_ptr->call_id;
    call_info_cmd_ptr->client_id     = call_info_ptr->call_client_id;
    call_info_cmd_ptr->call_mode     = call_info_ptr->sys_mode;
    call_info_cmd_ptr->end_status    = call_info_ptr->end_status;
    call_info_cmd_ptr->call_cmd_err  = CM_CALL_CMD_ERR_NOERR;
    call_info_cmd_ptr->subs_id       = call_info_ptr->asubs_id;

    memscpy(&(call_info_cmd_ptr->mode_info),
            sizeof(cm_call_mode_info_s_type),
            &(call_info_ptr->mode_info),
            sizeof(cm_call_mode_info_s_type));

   /*------------------------------------------------------------------------
      If this is a call end command, set the call end type to external
    -----------------------------------------------------------------------*/
    if(cmd_id  == DS_CMD_CM_CALL_EV_END)
    {
      call_info_cmd_ptr->end_type    = DS_CM_CALL_END_EXTERNAL;
      call_info_cmd_ptr->failed_orig = FALSE;
      call_info_cmd_ptr->call_inst   = DS3GI_INVALID_CALL_INSTANCE;
    }

#if (defined(FEATURE_DATA_WCDMA_CS) && defined(FEATURE_UUS))
    if(cmd_id == DS_CMD_CM_CALL_EV_USER_DATA_IND ||
       cmd_id == DS_CMD_CM_CALL_EV_USER_DATA_CONG_IND)
    {
      memscpy(&(call_info_cmd_ptr->uus_info),
              sizeof(cm_call_event_user_data_s_type),
              &(call_info_ptr->mode_info.info.gw_cs_call.end_user_data),
              sizeof(cm_call_event_user_data_s_type));
    }
#endif /* FEATURE_DATA_WCDMA_CS && FEATURE_UUS */

    /*----------------------------------------------------------------------
       For GET_PDN_CONN_REQ, populate the the sequence number
      ----------------------------------------------------------------------*/
    if(cmd_id == DS_CMD_CM_CALL_EV_GET_PDN_CONN_IND)
    {
      call_info_cmd_ptr->seq_num
        = call_info_ptr->mode_info.info.lte_call.seq_num;
      call_info_cmd_ptr->responding_to_page 
        = call_info_ptr->mode_info.info.lte_call.responding_to_page;
    }

    /*-----------------------------------------------------------------------
      Hack !!! for NAS issue. Remove when NAS passes the correct subs_id
      In case of invalid subs_id passed
        - If Single sim mode, set subs_id to DDS
        - Else if mult-sim, return. NAS/CM needs to pass correct subs_id
     ----------------------------------------------------------------------*/
    subs_id = call_info_ptr->asubs_id;
    if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      if (ds3gsubsmgr_get_device_mode() == SYS_MODEM_DEVICE_MODE_SINGLE_SIM)
      {
        subs_id = ds3gsubsmgr_get_def_data_subs_id();
        call_info_cmd_ptr->subs_id  = subs_id;
        DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "CALL INFO SINGLE SIM SUBS OVERRIDE: %d with %d",
                     call_info_ptr->asubs_id, subs_id);
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                        "Rx'ed call event on wrong subs %d", subs_id);
        ds_release_cmd_buf(&cmd_ptr);
        return;
      }
    }

    ds_put_cmd( cmd_ptr );
  } /* (cmd_id  != DS_MIN_CMD)  */
#ifdef FEATURE_DATA_IS707
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,"Invalid DS CMD %d: call_event %d",
                    cmd_id, call_event);
  }
#endif /* FEATURE_DATA_IS707 */

  return;
} /* ds3gi_cm_call_event_cb() */

/*===========================================================================
FUNCTION      DS3GI_CM_SS_EVENT_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr
              whenever a serving system related event occurs that 3G Dsmgr
              registered for.

              This function sends a command to the DS task, depending on the
              type of serving system event that occurred.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ss_event_cb
(
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr
)
{
  uint8                        stack_idx;
  ds_cmd_type                 *cmd_ptr = NULL;
  ds3g_ss_event_info_type     *event_info_ptr = NULL;
  sys_modem_as_id_e_type      *subs_id_ptr = NULL;

#if  defined(FEATURE_DATA_LTE)
  ds3g_ss_irat_cntxt_tx_type  *irat_cntxt_tx_ptr = NULL;
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_IS707
  ds707_call_throttle_info       thr_info;
  ds707_call_throttle_info      *call_thr_info_ptr = NULL;
  boolean                        post_throttle_info_update_cmd = FALSE;
  ds707_roaming_ind_type        *ds707_roaming_ind_ptr = NULL;
#endif /* FEATURE_DATA_IS707 */
  boolean                        post_roam_chg_cmd = FALSE;
  ds3g_roaming_ind_type         *roaming_ind_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ss_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Null ss_info_ptr");
    return;
  }

  DATA_3GMGR_MSG5(MSG_LEGACY_HIGH,
                  "ss event cb: Event:%d asubs_id:%d num_stacks:%d "
                  "changed_fields_subs:0x%x is_stack2_operational:%d",
                  ss_event, ss_info_ptr->asubs_id,
                  ss_info_ptr->number_of_stacks,
                  ss_info_ptr->changed_fields_subs,
                  ss_info_ptr->is_stack2_operational);

  DATA_3GMGR_MSG5(MSG_LEGACY_HIGH,
                  "ss event cb: bsr_in_progress:%d ps_data_suspend:%d "
                  "target_rat:%d div_duplex:%d is_csfb_call_active:%d",
                  ss_info_ptr->bsr_in_progress,
                  ss_info_ptr->ps_data_suspend,
                  ss_info_ptr->target_rat,
                  ss_info_ptr->div_duplex,
                  ss_info_ptr->is_csfb_call_active);


  for(stack_idx=0; stack_idx < ss_info_ptr->number_of_stacks; stack_idx++)
  {
    if(stack_idx > DS3G_SS_STACK_MAX)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "stack_idx:%d > maximum number of stacks", stack_idx);
      break;
    }

    DATA_3GMGR_MSG4(MSG_LEGACY_HIGH,
                    "ss event cb: stack[%d] is_operational:%d "
                    "changed_fields:0x%x changed_fields2:0x%x",
                    stack_idx,
                    ss_info_ptr->stack_info[stack_idx].is_operational,
                    ss_info_ptr->stack_info[stack_idx].changed_fields,
                    ss_info_ptr->stack_info[stack_idx].changed_fields2);

    DATA_3GMGR_MSG6(MSG_LEGACY_HIGH,
                    "ss event cb: stack[%d] sys_mode:%d srv_status:%d "
                    "srv_domain:%d roam_status: %d effective_srv_domain:%d",
                    stack_idx,
                    ss_info_ptr->stack_info[stack_idx].sys_mode,
                    ss_info_ptr->stack_info[stack_idx].srv_status,
                    ss_info_ptr->stack_info[stack_idx].srv_domain,
                    ss_info_ptr->stack_info[stack_idx].roam_status,
                    ss_info_ptr->stack_info[stack_idx].effective_srv_domain);

    DATA_3GMGR_MSG5(MSG_LEGACY_HIGH,
                    "ss event cb: stack[%d] hs_ind:%d hs_call_status:%d "
                    "dtm_supp:%d egprs_supp:%d",
                    stack_idx,
                    ss_info_ptr->stack_info[stack_idx].cell_srv_ind.hs_ind,
                    ss_info_ptr->stack_info[stack_idx].cell_srv_ind.hs_call_status,
                    ss_info_ptr->stack_info[stack_idx].cell_srv_ind.dtm_supp,
                    ss_info_ptr->stack_info[stack_idx].cell_srv_ind.egprs_supp);
  }

  /*-------------------------------------------------------------------------
    Handling only in case of optimized handoff
  -------------------------------------------------------------------------*/
  if (ss_event == CM_SS_EVENT_SRV_CHANGED)
  {
    ds3g_cache_radio_info(ss_info_ptr);

#ifdef FEATURE_DATA_OPTHO
    /* Note: This block should be only after the cache is updated above */
    if (ds707_s101_is_hdr_actv_ho_in_prog() == TRUE &&
        ds3g_is_hdr_in_srvc() == TRUE
       )
    {
      /*----------------------------------------------------------------------
         CM reports HDR is in service. If the Actv ho flag is set, DSD
         will skip parsing info about LTE and HDR in the SS change event.
         So we need to reset Actv ho flag in the cb itself here so DSD will
         process this ss event completely.
         Note: The actvo flag also needs to be cleared here before
         DS_707_PKT_SYS_CHG_CMD is posted below
      ----------------------------------------------------------------------*/
      ds707_s101_set_hdr_actv_ho_complete();
    }
#endif /* FEATURE_DATA_OPTHO */
  }

#ifdef FEATURE_DATA_IS707
  /*----------------------------------------------------------------------
    Checking to see if CDMA HDR service is lost to flow control
    applications to allow system determination to choose best
    system.
  ----------------------------------------------------------------------*/
    if(ss_event == CM_SS_EVENT_SRV_CHANGED)
    {
      for(stack_idx = 0; stack_idx <ss_info_ptr->number_of_stacks; stack_idx++)
      {
        if(stack_idx > DS3G_SS_STACK_MAX)
        {
          DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                        "stack_idx:%d > maximum number of stacks", stack_idx);
          break;
        }

        if((ss_info_ptr->stack_info[stack_idx].is_operational ) &&
           ((ss_info_ptr->stack_info[stack_idx].changed_fields & CM_SS_EVT_SRV_STATUS_MASK) != 0) &&
           ((ss_info_ptr->stack_info[stack_idx].sys_mode == SYS_SYS_MODE_CDMA) ||
            (ss_info_ptr->stack_info[stack_idx].sys_mode == SYS_SYS_MODE_HDR)))
        {
          /*-----------------------------------------------------------------------
            Get a command buffer from the DS Task.
          -----------------------------------------------------------------------*/
          cmd_ptr = ds_allocate_cmd_buf(sizeof(boolean));
          if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
          {
            ASSERT(0);
            return;
          }

          /*---------------------------------------------------------------------
            Fill in the command parameters and send the command to indicate that
            the system mode has changed and/or the suspend state has changed.
          ---------------------------------------------------------------------*/
          cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_SYS_CHG;
          *((boolean *)(cmd_ptr->cmd_payload_ptr)) =
            (ss_info_ptr->stack_info[stack_idx].srv_status != SYS_SRV_STATUS_SRV);
          ds_put_cmd( cmd_ptr );
        }
      }
    }
#endif /* FEATURE_DATA_IS707 */


  /*---------------------------------------------------------------------
   FR 18742 System mode updated to either 3GPP2 or 3GPP or No service at all .
   Hence reset the is_bsr_completed flag, so that new originations
   are unblocked further. This we are doing in CM task context.
  ---------------------------------------------------------------------*/
  if( ss_event == CM_SS_EVENT_SRV_CHANGED )
  {
#ifdef FEATURE_DATA_IS707
    ds707_pkti_set_is_bsr_completed (FALSE);
#endif /* FEATURE_DATA_IS707 */
  }

  /*-------------------------------------------------------------------------
    SS event stack info changed - posting command to process
  -------------------------------------------------------------------------*/
  if(ss_event == CM_SS_EVENT_SRV_CHANGED)
  {
    /*-----------------------------------------------------------------------
     Get a command buffer from the DS Task.
    -----------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_ss_event_info_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }

    event_info_ptr = (ds3g_ss_event_info_type*)cmd_ptr->cmd_payload_ptr;

    /*---------------------------------------------------------------------
      Fill in the command parameters and send the command to indicate that
      the system mode has changed and/or the suspend state has changed.
    ---------------------------------------------------------------------*/
    memset(event_info_ptr, 0, sizeof(ds3g_ss_event_info_type));
    event_info_ptr->asubs_id            = ss_info_ptr->asubs_id;
    event_info_ptr->number_of_stacks    = ss_info_ptr->number_of_stacks;
    event_info_ptr->changed_fields_subs = ss_info_ptr->changed_fields_subs;
    event_info_ptr->ps_data_suspend     = ss_info_ptr->ps_data_suspend;
    event_info_ptr->is_csfb_call_active = ss_info_ptr->is_csfb_call_active;
    event_info_ptr->bsr_in_progress     = ss_info_ptr->bsr_in_progress;
    event_info_ptr->target_rat          = ss_info_ptr->target_rat;
    event_info_ptr->div_duplex          = ss_info_ptr->div_duplex;
    event_info_ptr->is_stack2_operational = ss_info_ptr->is_stack2_operational;

    for(stack_idx=0; stack_idx < ss_info_ptr->number_of_stacks; stack_idx++)
    {
      if(stack_idx > DS3G_SS_STACK_MAX)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "stack_idx:%d > maximum number of stacks", stack_idx);
        break;
      }

      event_info_ptr->stack_info[stack_idx].changed_fields
        = ss_info_ptr->stack_info[stack_idx].changed_fields;
      event_info_ptr->stack_info[stack_idx].changed_fields2
        = ss_info_ptr->stack_info[stack_idx].changed_fields2;
      event_info_ptr->stack_info[stack_idx].is_operational
        = ss_info_ptr->stack_info[stack_idx].is_operational;
      event_info_ptr->stack_info[stack_idx].srv_status
        = ss_info_ptr->stack_info[stack_idx].srv_status;
      event_info_ptr->stack_info[stack_idx].srv_domain
        = ss_info_ptr->stack_info[stack_idx].srv_domain;
      event_info_ptr->stack_info[stack_idx].effective_srv_domain
        = ss_info_ptr->stack_info[stack_idx].effective_srv_domain;
      event_info_ptr->stack_info[stack_idx].sys_mode
        = ss_info_ptr->stack_info[stack_idx].sys_mode;

      memscpy(&(event_info_ptr->stack_info[stack_idx].sys_id),
              sizeof(sys_sys_id_s_type),
              &(ss_info_ptr->stack_info[stack_idx].sys_id),
              sizeof(sys_sys_id_s_type));

      memscpy(&(event_info_ptr->stack_info[stack_idx].cell_srv_ind),
              sizeof(cm_cell_srv_ind_s_type),
              &(ss_info_ptr->stack_info[stack_idx].cell_srv_ind),
              sizeof(cm_cell_srv_ind_s_type));
    }

#ifdef FEATURE_DATA_OPTHO
    /*-----------------------------------------------------------------------
       hdr_actv_ho flag maintained by s101 might have been
       cleared above if this event contains HDR IN SRVC notification.
       Call ds707_s101_is_hdr_actv_ho_in_prog() to get the latest value
    -----------------------------------------------------------------------*/
    event_info_ptr->hdr_actv_ho_in_prog = ds707_s101_is_hdr_actv_ho_in_prog();
#endif /* FEATURE_DATA_OPTHO */

    cmd_ptr->hdr.cmd_id = DS_CMD_CM_SS_EV_SRV_CHG;
    ds_put_cmd(cmd_ptr);


    /*------------------------------------------------------------------------
      Update the current plmn_id if the current sys_mode is L/W/G/T.
      PLMN ID is only updated if srv_status is FULL_SERV.
      Need to post this command after SRV_CHG_CMD otherwise while
      processing PLMN_ID CHG, sys_mode would be incorrect.
    -------------------------------------------------------------------------*/
    for(stack_idx=0; stack_idx < ss_info_ptr->number_of_stacks; stack_idx++)
    {
      if(stack_idx > DS3G_SS_STACK_MAX)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "stack_idx:%d > maximum number of stacks", stack_idx);
        break;
      }
      if((ss_info_ptr->stack_info[stack_idx].is_operational == TRUE) &&
         (DS_3GPP_IS_3GPP_CURRENT_MODE(ss_info_ptr->stack_info[stack_idx].sys_mode)))
      {
        if(ss_info_ptr->stack_info[stack_idx].srv_status == SYS_SRV_STATUS_SRV)
        {
          ds3g_set_current_plmn_id(&(event_info_ptr->stack_info[stack_idx].sys_id.id.plmn),
                                   ss_info_ptr->stack_info[stack_idx].sys_mode,
                                   ss_info_ptr->asubs_id);
        }
      }
    }
  }

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
    If system mode and/or suspend state has not changed, we need to call
    PREV change handler for 1X-only targets. However, for HDR targets, we
    could also get this command from CM if "Idle Digital Mode" changed, ie.:
    if we moved from 1X to HDR or vice-versa. For HDR targets, call PREV
    changed handler only if the Idle Digital Mode changed bit is NOT set.
  -------------------------------------------------------------------------*/
  if (((ss_info_ptr->stack_info[DS3G_SS_STACK_ID_1].is_operational == TRUE) &&
       ((ss_info_ptr->stack_info[DS3G_SS_STACK_ID_1].changed_fields &
        CM_SS_EVT_IDLE_DIGITAL_MODE_MASK) == 0)) )
#endif
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "ds3gi_cm_ss_event_cb: ds707_p_rev_change_handler()");
    ds707_p_rev_change_handler(ss_event, ss_info_ptr);
  }
#endif /*FEATURE_DATA_IS707 */


#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR
#ifdef FEATURE_DS_LN_UPDATE
  /*---------------------------------------------------------------------
    If event posted is CM_SS_EVENT_HDR_LN_STATUS then call the function
    to update the LN status.
  ---------------------------------------------------------------------*/
  if(ss_event == CM_SS_EVENT_HDR_LN_STATUS)
  {
    for(stack_idx = 0; stack_idx < ss_info_ptr->number_of_stacks; stack_idx++)
    {
      if(stack_idx > DS3G_SS_STACK_MAX)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "stack_idx:%d > maximum number of stacks", stack_idx);
        break;
      }

      if((ss_info_ptr->stack_info[stack_idx].is_operational == TRUE)      &&
         (ss_info_ptr->stack_info[stack_idx].sys_mode == SYS_SYS_MODE_HDR))
      {
        ds707_ln_update_status_cmd(
           ss_info_ptr->stack_info[stack_idx].rat_mode_info.hdr_mode_info.hdr_ln_status);
      }
    }
  }
#endif /*FEATURE_DS_LN_UPDATE*/
#endif /* FEATURE_HDR */
#endif /*FEATURE_DATA_IS707 */

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR
#ifdef FEATURE_DS_CAM
  /*-------------------------------------------------------------------------
   SYS_ID change indicates change in colocation status of the system.
   This is needed by the CAM module to switch call to Evdo if available.
   -------------------------------------------------------------------------*/
  if( (ss_event == CM_SS_EVENT_SRV_CHANGED) &&
      (ss_info_ptr->stack_info[DS3G_SS_STACK_ID_1].is_operational) &&
      ((ss_info_ptr->stack_info[DS3G_SS_STACK_ID_1].changed_fields & CM_SS_EVT_SYS_ID_MASK) != 0) )
  {
    ds707_pkt_post_coloc_info_changed_cmd(
       ss_info_ptr->stack_info[DS3G_SS_STACK_ID_1].rat_mode_info.cdma_mode_info.is_colloc);
  }

  if( (ss_event == CM_SS_EVENT_SRV_CHANGED) &&
      (ss_info_ptr->stack_info[DS3G_SS_STACK_ID_2].is_operational) &&
      ((ss_info_ptr->stack_info[DS3G_SS_STACK_ID_2].changed_fields & CM_SS_EVT_SYS_ID_MASK) != 0) )
  {
    ds707_pkt_post_coloc_info_changed_cmd(
       ss_info_ptr->stack_info[DS3G_SS_STACK_ID_2].rat_mode_info.hdr_mode_info.is_colloc);
  }
#endif /* FEATURE_DS_CAM */
#endif /* FEATURE_HDR */
#endif /*FEATURE_DATA_IS707 */

#ifdef FEATURE_DATA_IS707
  if((ss_event == CM_SS_EVENT_INFO) || (ss_event == CM_SS_EVENT_SRV_CHANGED))
  {
    memset((void*)&thr_info, 0, sizeof(ds707_call_throttle_info));

    for(stack_idx = 0; stack_idx < ss_info_ptr->number_of_stacks; stack_idx++)
    {
      if(stack_idx > DS3G_SS_STACK_MAX)
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "stack_idx:%d > maximum number of stacks", stack_idx);
        break;
      }

  /*-------------------------------------------------------------------------
    ds707_throttle_info_requested_flag was added for the case where QMI 
    queries 3GPP2 MH about the throttling status, for which 
    CM_SS_EVT_ORIG_THR_STATUS_MASK will not be set.
  -------------------------------------------------------------------------*/
      if( ((ss_info_ptr->stack_info[stack_idx].changed_fields &
            CM_SS_EVT_ORIG_THR_STATUS_MASK) != 0) ||
       	   (ds707_pkt_mgr_util_get_throttle_info_requested_flag() == TRUE) )
      {
        post_throttle_info_update_cmd = TRUE;
        ds707_pkt_mgr_util_set_throttle_info_requested_flag(FALSE);
        
        if((ss_info_ptr->stack_info[stack_idx].sys_mode == SYS_SYS_MODE_CDMA) &&
           (ss_info_ptr->stack_info[stack_idx].srv_status == SYS_SRV_STATUS_SRV) &&
           (ss_info_ptr->stack_info[stack_idx].thr_status.ss_thr_status) )
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                          "sys mode CDMA, thr uptime: %ld secs, curr uptime: %ld secs",
                          ss_info_ptr->stack_info[stack_idx].thr_status.thr_uptime,
                          ss_info_ptr->curr_uptime);
          thr_info.cdma_remaining_thr_time =
            (ss_info_ptr->stack_info[stack_idx].thr_status.thr_uptime - ss_info_ptr->curr_uptime);
        }
        if((ss_info_ptr->stack_info[stack_idx].sys_mode == SYS_SYS_MODE_HDR) &&
           (ss_info_ptr->stack_info[stack_idx].srv_status == SYS_SRV_STATUS_SRV) &&
           (ss_info_ptr->stack_info[stack_idx].thr_status.ss_thr_status) )
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                          "sys mode HDR, thr uptime: %ld secs, curr uptime: %ld secs",
                          ss_info_ptr->stack_info[stack_idx].thr_status.thr_uptime,
                        ss_info_ptr->curr_uptime);
          thr_info.hdr_remaining_thr_time =
          (ss_info_ptr->stack_info[stack_idx].thr_status.thr_uptime - ss_info_ptr->curr_uptime);
        }
      }
    }

    if ( post_throttle_info_update_cmd == TRUE )
    {
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_call_throttle_info));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return;
      }
      else
      {
       /* Inform call throttle info to 707 which will then pass on the required
          throttle info to client (QMI) who registered with C2K MH. */
        DATA_3GMGR_MSG0(MSG_LEGACY_MED,
                        "Post cmd to update 707 throttle info");
        cmd_ptr->hdr.cmd_id = DS_CMD_707_THROTTLE_INFO_UPDATE;
        call_thr_info_ptr = 
                     (ds707_call_throttle_info *)cmd_ptr->cmd_payload_ptr;
        call_thr_info_ptr->hdr_remaining_thr_time 
                                          = thr_info.hdr_remaining_thr_time;
        call_thr_info_ptr->cdma_remaining_thr_time 
                                          = thr_info.cdma_remaining_thr_time;
        ds_put_cmd( cmd_ptr );
      }
    }
  }
  /*----------------------------------------------------------------------
    Set the Roam status for HDR/CDMA.
  ----------------------------------------------------------------------*/
  if (ss_event == CM_SS_EVENT_INFO || ss_event == CM_SS_EVENT_SRV_CHANGED)
  {
    for(stack_idx = 0; stack_idx < ss_info_ptr->number_of_stacks; stack_idx++)
    {
      if(stack_idx > DS3G_SS_STACK_MAX)
      {
        break;
      }

      if(ds3g_check_if_3gpp2_roam_status_changed(ss_info_ptr, stack_idx))
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                        "3GPP2 Roam change indication recvd by DS task "
                        "stack[%d] roam_status: %d",
                        stack_idx, 
                        ss_info_ptr->stack_info[stack_idx].roam_status);

        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_roaming_ind_type));
     /** Get command buffer to do task switch */
    if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
    {
      ASSERT(0);
      return;
    }
    else
    {
      /*---------------------------------------------------------------------
            Fill in the command parameters and send the command to indicate 3GPP2
            roam status.
      ---------------------------------------------------------------------*/
      cmd_ptr->hdr.cmd_id = DS_CMD_707_ROAM_CHG;
          ds707_roaming_ind_ptr = (ds707_roaming_ind_type*)cmd_ptr->cmd_payload_ptr;
          ds707_roaming_ind_ptr->subscription_id = ss_info_ptr->asubs_id;
          ds707_roaming_ind_ptr->roam_status = ss_info_ptr->stack_info[stack_idx].roam_status;
          ds707_roaming_ind_ptr->sys_mode = ss_info_ptr->stack_info[stack_idx].sys_mode;   
          ds_put_cmd( cmd_ptr );
      }
      }
    }
  }
#endif /* FEATURE_DATA_IS707 */

  if (ss_event == CM_SS_EVENT_INFO || ss_event == CM_SS_EVENT_SRV_CHANGED)
  {
    post_roam_chg_cmd =
      ds3g_check_if_3gpp_roam_status_changed(ss_info_ptr, &stack_idx);
  }
  /*------------------------------------------------------------------------------ 
    ROAMING IND FOR GWT and L
  ------------------------------------------------------------------------------*/
  if(post_roam_chg_cmd)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "3GPP Roam change indication recvd by DS task "
                    "stack[%d] roam_status: %d",
                    stack_idx, 
                    ss_info_ptr->stack_info[stack_idx].roam_status);

    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_roaming_ind_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
    else
    {
      /*---------------------------------------------------------------------
        Fill in the command parameters and send the command to indicate 3GPP
        roam status.
      ---------------------------------------------------------------------*/
      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_ROAMING_STATUS_CHG;
      roaming_ind_ptr = (ds3g_roaming_ind_type*)cmd_ptr->cmd_payload_ptr;
      roaming_ind_ptr->gw_roam_status  = ss_info_ptr->stack_info[stack_idx].roam_status;
      roaming_ind_ptr->subscription_id = ss_info_ptr->asubs_id;

      ds_put_cmd( cmd_ptr );
    }
  }

#ifdef FEATURE_PS_DORMANT_PWR_SAVE
  if (ss_event == CM_SS_EVENT_PS_DATA_FAIL)
  {
    /*-----------------------------------------------------------------------
      Get a command buffer from the DS Task.
    -----------------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
    else
    {
      /*---------------------------------------------------------------------
        Fill in the command parameters and send the command to indicate that
        service acquisition failed.
      ---------------------------------------------------------------------*/
      cmd_ptr->hdr.cmd_id = DS_CMD_CM_SS_EV_PS_DATA_FAIL;
      subs_id_ptr = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
      *subs_id_ptr = ss_info_ptr->asubs_id;
      ds_put_cmd( cmd_ptr );
    }
  }
#endif /* FEATURE_PS_DORMANT_PWR_SAVE */

#if  defined(FEATURE_DATA_LTE)
  if (ss_event == CM_SS_EVENT_IRAT_CTXT_TRANSFER)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_ss_irat_cntxt_tx_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
    else
    {
      irat_cntxt_tx_ptr = (ds3g_ss_irat_cntxt_tx_type*)cmd_ptr->cmd_payload_ptr;

      /* Copy IRAT context type */
      irat_cntxt_tx_ptr->irat_asubs_id = ss_info_ptr->gwl_irat_asubs_id;
      irat_cntxt_tx_ptr->irat_ctxt_target_rat
        = ss_info_ptr->irat_ctxt_target_rat;

      cmd_ptr->hdr.cmd_id = DS_CMD_CM_SS_EV_CNTXT_TRANSFER;
      ds_put_cmd(cmd_ptr);
    }
  }
#endif /* FEATURE_DATA_LTE */


#ifdef FEATURE_DATA_ANDSF
  if (ss_event == CM_SS_EVENT_SRV_CHANGED || ss_event == CM_SS_EVENT_RSSI)
  {
    ds_andsf_notify_wwan_ss_info(ss_event, ss_info_ptr);
  }

#endif /*FEATURE_DATA_ANDSF*/

  return;
}/* ds3gi_cm_ss_event_cb() */

/*===========================================================================
FUNCTION      DS3GI_CM_CALL_CMD_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr
              of the completion status of the call command previously issued
              by 3G Dsmgr.

              This callback function may be invoked in relation to the
              following call commands: cm_call_cmd_answer(),
              cm_call_cmd_orig(), cm_call_cmd_end(), cm_call_cmd_setup_res().

              In each case, if the command was successful, no action is
              taken. If the cm_call_cmd_orig() was unsuccessful, a
              DS_CM_CALL_ENDED_CMD is sent to the DS task. Errors in all
              other call commands do not require any action.

DEPENDENCIES  Assumes a one-call system.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_call_cmd_cb
(
  void                   *data_block_ptr, /* Data blk provided in call cmd */
  cm_call_cmd_e_type      call_cmd,       /* Call command id               */
  cm_call_cmd_err_e_type  call_cmd_err    /* Call command error code       */
)
{
  ds_cmd_type              *cmd_ptr = NULL;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  uint32                    call_inst;
  sys_sys_mode_e_type       sys_mode = SYS_SYS_MODE_NONE;
  sys_modem_as_id_e_type   *subs_id_ptr = NULL;
#ifdef FEATURE_DATA_LTE
  uint32                    sdf_id;
#endif /* FEATURE_DATA_LTE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    /*-----------------------------------------------------------------------
      Some type of error occurred, take action based on the the type of
      command.
    -----------------------------------------------------------------------*/
    switch( call_cmd )
    {
      case CM_CALL_CMD_ORIG:
      case CM_CALL_CMD_SETUP_RES:
      case CM_CALL_CMD_ANSWER:
        /*-------------------------------------------------------------------
          Send a DS_CM_CALL_END_CMD to the DS task. In origination &
          setup response, call id is passed to CM in the data block
        -------------------------------------------------------------------*/
        call_inst = (uint32) data_block_ptr;
        if(call_inst >= DS3GI_MAX_NUM_CALLS)
        {
          DATA_ERR_FATAL("call_inst is greater than maximum calls");
          return;
        }

        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_call_info_cmd_type));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }
        else
        {
          cmd_ptr->hdr.cmd_id = DS_CMD_CM_CALL_EV_END;
          call_info_cmd_ptr
            = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

          call_info_cmd_ptr->call_id = ds3gi_call_info[call_inst].call_id;
          call_info_cmd_ptr->call_inst = call_inst;
          call_info_cmd_ptr->call_mode = ds3gi_call_info[call_inst].call_mode;
          call_info_cmd_ptr->subs_id = ds3gi_call_info[call_inst].subs_id;

          if( call_cmd_err == CM_CALL_CMD_ERR_OFFLINE_S )
          {
            call_info_cmd_ptr->end_status = CM_CALL_END_OFFLINE;
          }
          else if (call_cmd_err == CM_CALL_CMD_ERR_SRV_STATE_S)
          {
            call_info_cmd_ptr->end_status = CM_CALL_END_NO_SRV;
          }
          else
          {
            call_info_cmd_ptr->end_status = CM_CALL_END_INTERNAL_ERROR;
          }

          call_info_cmd_ptr->end_type = DS_CM_CALL_END_INTERNAL;
          call_info_cmd_ptr->failed_orig = TRUE;
          call_info_cmd_ptr->call_cmd_err = call_cmd_err;
          call_info_cmd_ptr->call_type = CM_CALL_TYPE_PS_DATA;

          DATA_3GMGR_MSG3(MSG_LEGACY_LOW,
                          "DS posting DS_CM_CALL_END_CMD, "
                          "call_id:%d, call_cmd_err:%d call_type:%d",
                          ds3gi_call_info[call_inst].call_id,
                          call_cmd_err,
                          call_info_cmd_ptr->call_type);
          ds_put_cmd( cmd_ptr );
        }
        break;

       case CM_CALL_CMD_RAB_REESTAB:
        if(ds3gsubsmgr_is_device_mode_single_sim() == FALSE)
        {
          /*-------------------------------------------------------------------
            CM returned error for RAB_REESTAB_REQ
          -------------------------------------------------------------------*/
          call_inst = (uint32) data_block_ptr;
          if(call_inst >= DS3GI_MAX_NUM_CALLS)
          {
            DATA_ERR_FATAL("call_inst is greater than maximum calls");
            return;
          }

          cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_call_info_cmd_type));
          if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
          {
            ASSERT(0);
            return;
          }
          else
          {
            cmd_ptr->hdr.cmd_id = DS_CMD_CM_CALL_EV_RAB_REESTAB_REJ;
            call_info_cmd_ptr
              = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

            call_info_cmd_ptr->call_id = ds3gi_call_info[call_inst].call_id;
            call_info_cmd_ptr->call_inst = call_inst;
            call_info_cmd_ptr->call_mode
              = ds3gi_call_info[call_inst].call_mode;

            sys_mode
              = ds3g_get_current_network_mode_ex(ds3g_get_ps_subs_id());

            call_info_cmd_ptr->mode_info.info_type = CM_CALL_MODE_INFO_NONE;

            /*-----------------------------------------------------------------
            CM suspends DS when there is a voice call on the non PS subscription.
            If CM responds to our RAB_REESTAB_REQ with an error, then we need to
            reset the rab_reestab_in_progress_flag. This happens only in case of
            WCDMA.
            -----------------------------------------------------------------*/
            if( (sys_mode == SYS_SYS_MODE_WCDMA) ||
                 (sys_mode == SYS_SYS_MODE_GSM)
  #ifdef FEATURE_DATA_LTE
                 || (sys_mode == SYS_SYS_MODE_LTE)
  #endif /* FEATURE DATA LTE */
                 || (sys_mode == SYS_SYS_MODE_TDS) )
            {
              call_info_cmd_ptr->mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
              call_info_cmd_ptr->mode_info.info.gw_ps_call.rab_cause
                = CONGESTTION;
            }

            DATA_3GMGR_MSG3(MSG_LEGACY_LOW,
                            "Posting command DS_CM_RAB_REESTAB_REJ_CMD"
                            "call_inst:%d call_id:%d error:%d",
                            call_inst,
                            ds3gi_call_info[call_inst].call_id,
                            call_cmd_err);
            ds_put_cmd( cmd_ptr );
          }
        }
        break;

      case CM_CALL_CMD_PS_DATA_AVAILABLE:
        /*-------------------------------------------------------------
        CM returned error for Data available
        --------------------------------------------------------------*/
        DATA_3GMGR_MSG1(MSG_LEGACY_MED,
                        "CM returned error:%d for data_avail. "
                        "Stop sending data avail to CM",
                        call_cmd_err);

        cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_CALL_STOP_DATA_AVAIL_IND;
        subs_id_ptr = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
        *subs_id_ptr = (sys_modem_as_id_e_type)data_block_ptr;
        ds_put_cmd( cmd_ptr );
        break;

#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
      case CM_CALL_CMD_INCOMING_PAGE_RSP:
        break;
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

#ifdef FEATURE_DATA_LTE
      case CM_CALL_CMD_PDN_CONNECTIVITY_REQ:
         sdf_id = (uint32) data_block_ptr;

        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_call_info_cmd_type));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }
        else
        {
          cmd_ptr->hdr.cmd_id = DS_CMD_CM_PDN_CONNECTIVITY_REQ_ERR;
          call_info_cmd_ptr
            = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;
          memset(call_info_cmd_ptr, 0, sizeof(ds3g_call_info_cmd_type));
          call_info_cmd_ptr->sdf_id = sdf_id;
          call_info_cmd_ptr->call_id = CM_CALL_ID_INVALID;
          call_info_cmd_ptr->call_mode = SYS_SYS_MODE_LTE;

          call_info_cmd_ptr->end_status = CM_CALL_END_INTERNAL_ERROR;

          call_info_cmd_ptr->end_type = DS_CM_CALL_END_INTERNAL;
          call_info_cmd_ptr->failed_orig = TRUE;
          call_info_cmd_ptr->call_cmd_err = call_cmd_err;
          call_info_cmd_ptr->call_type = CM_CALL_TYPE_PS_DATA;
          call_info_cmd_ptr->subs_id   = ds3g_get_ps_subs_id();

          DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                          "DS posting DS_CMD_CM_PDN_CONNECTIVITY_REQ_ERR, "
                          "call_type:%d error: %d",
                          call_info_cmd_ptr->call_type, call_cmd_err);
          ds_put_cmd( cmd_ptr );
        }
        break;

      case CM_CALL_CMD_PDN_CONNECTIVITY_ABORT_REQ:
      case CM_CALL_CMD_PDN_DISCONNECT_REQ:
      case CM_CALL_CMD_RES_ALLOC_REQ:
      case CM_CALL_CMD_RES_ALLOC_ABORT_REQ:
      case CM_CALL_CMD_BEARER_RES_MOD_REQ:
#endif /* FEATURE_DATA_LTE */

      default:
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "CM call cmd error occurred: %d", call_cmd_err);
        /* No action taken */
        break;

    } /* switch( call_cmd ) */
  } /* if cmd error */
  return;
} /* ds3gi_cm_call_cmd_cb() */

/*===========================================================================
FUNCTION      DS3GI_CM_CALL_CMD_EXT_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of
              the completion status of the call command previously issued by
              3G Dsmgr.

              This callback function may be invoked in relation to
              cm_mm_call_cmd_orig_exten3()

              In each case, if the command was successful, no action is taken.
              If the cm_mm_call_cmd_orig_exten3() was unsuccessful, a
              DS_CM_CALL_ENDED_CMD is sent to the DS task. Errors in all other
              call commands do not require any action.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_call_cmd_ext_cb
(
  void                          *data_block_ptr,     /* Data block */
  cm_call_cmd_e_type             call_cmd,           /* Call command id */
  cm_call_cmd_err_e_type         call_cmd_err,       /* Call command error code */
  cm_call_cmd_err_cause_e_type   call_cmd_err_cause, /* Cause of the error */
  cm_alpha_s_type                alpha               /* Alpha value */
)
{
  ds_cmd_type              *cmd_ptr = NULL;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  uint32                    call_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1( MSG_LEGACY_HIGH,
                   "ds3gi_cm_call_cmd_ext_cb err %d", call_cmd_err);
  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    /*-----------------------------------------------------------------------
      Some type of error occurred, take action based on the the type of
      command.
    -----------------------------------------------------------------------*/
    switch( call_cmd )
    {
      case CM_CALL_CMD_ORIG:
      case CM_CALL_CMD_ANSWER:
        /*-------------------------------------------------------------------
          Send a DS_CM_CALL_END_CMD to the DS task. In origination &
          setup response, call id is passed to CM in the data block
        -------------------------------------------------------------------*/
        call_inst = (uint32) data_block_ptr;
        if(call_inst >= DS3GI_MAX_NUM_CALLS)
        {
          DATA_ERR_FATAL("call_inst is greater than maximum calls");
          return;
        }

        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_call_info_cmd_type));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }
        else
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                          "cm_mm_call_cmd() callid :%d error: %d",
                          ds3gi_call_info[call_inst].call_id,
                          call_cmd_err);

          cmd_ptr->hdr.cmd_id = DS_CMD_CM_CALL_EV_END;
          call_info_cmd_ptr
            = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

          call_info_cmd_ptr->call_id = ds3gi_call_info[call_inst].call_id;
          call_info_cmd_ptr->call_inst = call_inst;
          call_info_cmd_ptr->call_mode = ds3gi_call_info[call_inst].call_mode;
          call_info_cmd_ptr->subs_id = ds3gi_call_info[call_inst].subs_id;

          if( call_cmd_err == CM_CALL_CMD_ERR_OFFLINE_S )
          {
            call_info_cmd_ptr->end_status = CM_CALL_END_OFFLINE;
          }
          else if (call_cmd_err == CM_CALL_CMD_ERR_SRV_STATE_S)
          {
            call_info_cmd_ptr->end_status = CM_CALL_END_NO_SRV;
          }
          else
          {
            call_info_cmd_ptr->end_status = CM_CALL_END_INTERNAL_ERROR;
          }

          call_info_cmd_ptr->end_type = DS_CM_CALL_END_INTERNAL;
          call_info_cmd_ptr->failed_orig = TRUE;
          call_info_cmd_ptr->call_cmd_err = call_cmd_err;
          ds_put_cmd( cmd_ptr );
        }
        break;

      default:
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "CM call cmd error occurred: %d", call_cmd_err);
        /* No action taken */
        break;

    } /* switch( call_cmd ) */
  } /* if cmd error */
  return;
} /* ds3gi_cm_call_cmd_ext_cb() */

/*===========================================================================
FUNCTION      DS3GI_CM_CALL_CMD_END_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of
              the completion status of the call command previously issued by
              3G Dsmgr.

              This callback function may be invoked in relation to
              cm_mm_call_cmd_exten_end(). Added call_id as one of the
              parameters.

              In case, if the command was successful, no action is taken.
              If the cm_mm_call_cmd_exten_end() was unsuccessful, a
              DS_CM_CALL_ENDED_CMD is sent to the DS task.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_call_cmd_end_cb
(
  void                    *data_block_ptr,     /* Data block */
  cm_call_cmd_e_type       call_cmd,           /* Call command id */
  cm_call_cmd_err_e_type   call_cmd_err,       /* Call command error code */
  cm_call_id_type          call_id             /* Call ID passed with the call command */
)
{
  ds_cmd_type              *cmd_ptr;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  uint32                    call_inst;

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    /*-----------------------------------------------------------------------
      Some type of error occurred, take action based on the the type of
      command.
    -----------------------------------------------------------------------*/
    if( call_cmd == CM_CALL_CMD_END)
    {
      /*-------------------------------------------------------------------
             Send a DS_CM_CALL_END_CMD to the DS task.
          -------------------------------------------------------------------*/
      call_inst = (uint32) data_block_ptr;
      if(call_inst >= DS3GI_MAX_NUM_CALLS)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                        "call_inst is greater than maximum calls");
        return;
      }

      if(call_id != ds3gi_call_info[call_inst].call_id)
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                        "Call_id %d received from CM while ds3gi_call_info"
                        "has call_id %d. Skipping call_end",
                        call_id, ds3gi_call_info[call_inst].call_id);
        return;
      }

      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_call_info_cmd_type));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }
      else
      {
        DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                        "cm_mm_call_cmd() callid :%d error: %d call instance %d",
                        ds3gi_call_info[call_inst].call_id,
                        call_cmd_err,
                        call_inst);

        call_info_cmd_ptr = (ds3g_call_info_cmd_type*)
                                     (cmd_ptr->cmd_payload_ptr);

        if (CM_CALL_CMD_ERR_MODE_INFO_CHG_LTE == call_cmd_err)
        {
          cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_CALL_END_MODE_CHG_LTE;
          call_info_cmd_ptr->call_id = ds3gi_call_info[call_inst].call_id;
        }
        else
        {
          cmd_ptr->hdr.cmd_id = DS_CMD_CM_CALL_EV_END;
          call_info_cmd_ptr->call_id = ds3gi_call_info[call_inst].call_id;
          call_info_cmd_ptr->call_inst = call_inst;
          call_info_cmd_ptr->call_mode = ds3gi_call_info[call_inst].call_mode;
          call_info_cmd_ptr->end_status = CM_CALL_END_INTERNAL_ERROR;
          call_info_cmd_ptr->end_type = DS_CM_CALL_END_INTERNAL;
          call_info_cmd_ptr->failed_orig = TRUE;
          call_info_cmd_ptr->call_cmd_err = call_cmd_err;
          call_info_cmd_ptr->subs_id = (sys_modem_as_id_e_type)data_block_ptr;
        }
        ds_put_cmd( cmd_ptr );
      }
    }
  }
}/* ds3gi_cm_call_cmd_end_cb() */

/*===========================================================================
FUNCTION      DS3GI_CM_SS_CMD_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr
              of the completion status of the ss command previously issued
              by 3G Dsmgr.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ss_cmd_cb
(
  void                 *data_block_ptr,
  cm_ss_cmd_e_type      ss_cmd,
  cm_ss_cmd_err_e_type  ss_cmd_err
)
{
  ds_cmd_type            *cmd_ptr;
  sys_modem_as_id_e_type *subs_id_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                  "ds3gi_cm_ss_cmd_cb subs_id:%d ss_cmd:%d ss_cmd_err:%d",
                  (sys_modem_as_id_e_type)data_block_ptr, ss_cmd, ss_cmd_err);

  switch( ss_cmd )
  {
    case CM_SS_CMD_PS_DATA_AVAIL:
      if (ss_cmd_err != CM_SS_CMD_ERR_NOERR )
      {
        /*-------------------------------------------------------------
        CM returned error for Data available
        --------------------------------------------------------------*/
        cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }

        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "Posting cmd DS_CMD_3GPP_CALL_STOP_DATA_AVAIL_IND");
        cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_CALL_STOP_DATA_AVAIL_IND;
        subs_id_ptr = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
        *subs_id_ptr = (sys_modem_as_id_e_type)data_block_ptr;
        ds_put_cmd( cmd_ptr );
      }
      break;
    default:
      /* No action taken */
      break;
  }
  return;
} /* ds3gi_cm_ss_cmd_cb() */

/*===========================================================================
FUNCTION      DS3GI_CM_PWROFF_LPM_MODE_CB

DESCRIPTION   This callback function is registered with CM to notify DS of
              PWROFF / LPM mode. It posts cmd to DS.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_cm_pwroff_lpm_mode_cb( void )
{
  boolean                   ret_val = TRUE;
  ds3geventmgr_filter_type  filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Mode LPM indicates that all the contexts for every subscription needs
    to be cleaned up. subs_id in the event_info.mode_info does not need to
    be populated.
  -------------------------------------------------------------------------*/
  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

  filter_info.ps_subs_id = PS_SYS_PRIMARY_SUBS;
  filter_info.tech = PS_SYS_TECH_ALL;

  if(ds3geventmgr_set_event_info(DS3GEVENTMGR_MODE_LPM_EV, NULL, &filter_info)
     == FALSE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "Unable to provide Mode LPM event trigger information");
    ret_val = FALSE;
  }

  return ret_val;
} /* ds3gi_cm_pwroff_lpm_mode_cb() */

/*===========================================================================
FUNCTION      DS3GI_CM_SUB_NOT_AVAIL_NOTIFY_CB

DESCRIPTION   This callback function is registered with CM to notify DS of
              Subscription Not Available during card refresh/ remove event etc.

DEPENDENCIES  None

RETURN VALUE  TRUE  - Success
              FALSE - Failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_cm_sub_not_avail_notify_cb
(
  uint8   subs_id_mask,
  uint8  *sub_clnup_reason_ptr
)
{
  boolean                   ret_val = TRUE;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
  ds3geventmgr_filter_type  filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (subs_id_mask <= SYS_MODEM_AS_ID_MASK_NONE ||
      subs_id_mask > SYS_MODEM_AS_ID_MASK_ANY)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "Invalid subs_id_mask %d passed. Return FALSE");
    ds3g_cm_data_packet_state_notify(subs_id_mask);
    return FALSE;
  }

  if( sub_clnup_reason_ptr == NULL )
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------------
    This global will always keep track of the latest value from CM. As per CM,
    they will pass only incrememted bit masks (i.e. 01 followed by 11) and
    expect only the most recent one to be returned. Overwritting the previous
    value is ok.
  ---------------------------------------------------------------------------*/
  ds3gi_sub_not_avail_subs_id_mask = subs_id_mask;

  /*---------------------------------------------------------------------------
    Extract the subs id from the mask and process each subscription separately.
    Notify all clients of this event on the desired subscription.
  ---------------------------------------------------------------------------*/
  if ((subs_id_mask & SYS_MODEM_AS_ID_MASK_1) == SYS_MODEM_AS_ID_MASK_1)
  {
    subs_id = SYS_MODEM_AS_ID_1;
  }
  else if ((subs_id_mask & SYS_MODEM_AS_ID_MASK_2) == SYS_MODEM_AS_ID_MASK_2)
  {
    subs_id = SYS_MODEM_AS_ID_2;
  }
  else if ((subs_id_mask & SYS_MODEM_AS_ID_MASK_3) == SYS_MODEM_AS_ID_MASK_3)
  {
    subs_id = SYS_MODEM_AS_ID_3;
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid Subs ID Mask: %d", subs_id_mask);
    ret_val = FALSE;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "ds3gi_cm_sub_not_avail_notify_cb subs_id_mask:%d "
                  "clnup_reason:%d",
                  subs_id_mask, sub_clnup_reason_ptr[subs_id]);

  ds3g_mmgsdi_clear_subs_id_ready_mask(subs_id);

  if(ret_val == TRUE)
  {
    memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                              ds3gsubsmgr_subs_id_cm_to_ds(subs_id);
    filter_info.tech = PS_SYS_TECH_ALL;

    if(ds3geventmgr_set_event_info(DS3GEVENTMGR_SUB_NOT_AVAIL_EV,
                                   NULL,
                                   &filter_info) == FALSE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Unable to send SUB_NOT_AVAIL EV");
      ret_val = FALSE;
    }
  }

  return ret_val;
} /* ds3gi_cm_sub_not_avail_notify_cb() */

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/*===========================================================================
FUNCTION      DS3GI_SS_CMD_INFO_CB

DESCRIPTION   Callback function registered with CM to get status of SS info
              get request.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_ss_cmd_info_cb
(
  void                 *data_block_ptr,
  cm_ss_cmd_e_type      ss_cmd,
  cm_ss_cmd_err_e_type  ss_cmd_err
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( ss_cmd )
  {
    case CM_SS_CMD_INFO_GET:
      if( ss_cmd_err == CM_SS_CMD_ERR_NO_BUF_L )
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                        "Buffer could not be allocated by CM.");
      }
      break;

    default:
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                      "Callback called for unexpected SS command.");
      break;
  }

  return;
} /* ds3gi_ss_cmd_info_cb() */
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION      DS3GI_CM_PLMN_LIST_CHANGE_IND_MSG_HDLR

DESCRIPTION   This function is used to handle the CM_PLMN_LIST_CHANGE_IND
              message from CM. This message indicates that a plmn change has
              occured.

DEPENDENCIES  None

RETURN VALUE  TRUE is processing was successful
              FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_cm_plmn_list_change_ind_msg_hdlr
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  cm_plmn_list_chngd_ind_s_type  *msg_ptr = NULL;
  sys_modem_as_id_e_type          subs_id = SYS_MODEM_AS_ID_NONE;
  ds3geventmgr_filter_type        filter_info = {0};
  ds3geventmgr_event_info_type    event_info = {{0}};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((msgrtype == MM_CM_PLMN_LIST_CHANGE_IND) && (dsmsg_ptr != NULL))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Processing MM_CM_PLMN_LIST_CHANGE_IND");

    msg_ptr = (cm_plmn_list_chngd_ind_s_type*)dsmsg_ptr;

    subs_id = msg_ptr->as_id;

    if (ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE)
    {
      return FALSE;
    }

    memset(&filter_info,0,sizeof(ds3geventmgr_filter_type));
    memset(&event_info,0,sizeof(ds3geventmgr_event_info_type));

    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
                                        ds3gsubsmgr_subs_id_cm_to_ds(subs_id);
    filter_info.tech = PS_SYS_TECH_ALL;

    event_info.plmn_list_chg_info.plmn_type = msg_ptr->changed_list_type;

    ds3geventmgr_set_event_info(DS3GEVENTMGR_PLMN_LIST_CHG_EV,
                                (void*)&event_info,
                                &filter_info);
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "MM_CM_PLMN_LIST_CHANGE_IND msg hdlr. Invalid inputs.");
    return FALSE;
  }

  return TRUE;
} /* ds3gi_cm_plmn_list_change_ind_msg_hdlr */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GI_CM_INIT

DESCRIPTION   This function initializes the Data Services Call Manager
              client, and registers for call events of interest, namely:
                CM_CALL_EVENT_END
                CM_CALL_EVENT_INCOM
                CM_CALL_EVENT_CONNECT

              The following are only applicable to GSM/WCDMA:
                CM_CALL_EVENT_ORIG
                CM_CALL EVENT_SETUP_IND
                CM_CALL_EVENT_PROGRESS_INFO_IND
                CM_CALL_EVENT_CALL_CONF
                CM_CALL_EVENT_RAB_REL_IND
                CM_CALL_EVENT_RAB_ESTAB_IND
                CM_CALL_EVENT_RAB_REESTAB_REJ
                CM_CALL_EVENT_PDP_MODIFY_IND,
                CM_CALL_EVENT_PDP_MODIFY_CONF,
                CM_CALL_EVENT_PDP_MODIFY_REJ,
                CM_CALL_EVENT_MODIFY_IND
                CM_CALL_EVENT_MODIFY_RES
                CM_CALL_EVENT_MODIFY_COMPLETE_CONF

              It then calls the individual mode handlers to allow them to
              register for mode-specific events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_init(void)
{
  cm_client_status_e_type  status;
  cm_client_status_e_type  ret_val;
  cm_client_id_type        ds3gi_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Register as the Data Services client with Call Manager.
  -------------------------------------------------------------------------*/
  if((status = cm_client_init( CM_CLIENT_TYPE_DS, &ds3gi_cm_client_id )) !=
                                                            CM_CLIENT_OK )
  {
    ASSERT(0);
  }
  else
  {
    ds3g_set_cm_client_id(ds3gi_cm_client_id);
    /*-----------------------------------------------------------------------
      Register for call related events of interest. Multiple registrations
      are needed since only one range of events may be registered at a time.
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_END,
                                     CM_CALL_EVENT_END,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_INCOM,
                                     CM_CALL_EVENT_CONNECT,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_HO_COMPLETE,
                                     CM_CALL_EVENT_HO_COMPLETE,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_INCOMING_PAGE,
                                     CM_CALL_EVENT_INCOMING_PAGE,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_SETUP_IND,
                                     CM_CALL_EVENT_SETUP_IND,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_CALL_CONF,
                                     CM_CALL_EVENT_CALL_CONF,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_PROGRESS_INFO_IND,
                                     CM_CALL_EVENT_PROGRESS_INFO_IND,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    /*-----------------------------------------------------------------------
      Register for PDP context preservation events :
      a) RAB is released b) RAB is re-established c) re-establishment fails
      d) re-establishment rejected
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_RAB_REL_IND,
                                     CM_CALL_EVENT_RAB_REL_IND,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_RAB_REESTAB_IND,
                                     CM_CALL_EVENT_RAB_REESTAB_IND,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_RAB_REESTAB_REJ,
                                     CM_CALL_EVENT_RAB_REESTAB_REJ,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_RAB_REESTAB_FAIL,
                                     CM_CALL_EVENT_RAB_REESTAB_FAIL,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

#ifdef FEATURE_UE_INITIATED_DORMANCY
    /*-----------------------------------------------------------------------
      Register for PS_SIG_REL_CNF - response for UE initiated dormancy req
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_PS_SIG_REL_CNF,
                                     CM_CALL_EVENT_PS_SIG_REL_CNF,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }
#endif //FEATURE_UE_INITIATED_DORMANCY

    /*-----------------------------------------------------------------------
      Register for PDP context modification event
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_PDP_MODIFY_IND,
                                     CM_CALL_EVENT_PDP_MODIFY_IND,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    /*-----------------------------------------------------------------------
      Register for NW Init PDP context modification event
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_PDP_NW_MODIFY_REQ,
                                     CM_CALL_EVENT_PDP_NW_MODIFY_REQ,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }
    /*-----------------------------------------------------------------------
      Register for PDP context modification Success event
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_PDP_MODIFY_CONF,
                                     CM_CALL_EVENT_PDP_MODIFY_CONF,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    /*-----------------------------------------------------------------------
      Register for PDP context modification  Failure event
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_PDP_MODIFY_REJ,
                                     CM_CALL_EVENT_PDP_MODIFY_REJ,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    /*-----------------------------------------------------------------------
      Register for PDP context promorion event
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_PDP_PROMOTE_IND,
                                     CM_CALL_EVENT_PDP_PROMOTE_IND,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

#ifdef FEATURE_UUS
    /*-----------------------------------------------------------------------
      User to User Signaling (UUS) Data Indication
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_USER_DATA_IND,
                                     CM_CALL_EVENT_USER_DATA_IND,
                                     NULL );

    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    /*-----------------------------------------------------------------------
      UUS Data Congestion
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_USER_DATA_CONG_IND,
                                     CM_CALL_EVENT_USER_DATA_CONG_IND,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

#endif /* FEATURE_UUS */

#ifdef FEATURE_MULTIMEDIA_FALLBACK
    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                                     ds3gi_cm_call_event_cb,
                                     CM_CLIENT_EVENT_REG,
                                     CM_CALL_EVENT_ORIG,
                                     CM_CALL_EVENT_ORIG,
                                     NULL );
    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_call_reg error %d",(int)(ret_val));
    }

    /*-----------------------------------------------------------------------
      Register to be notified whenever the serving system changes.
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_ss_reg_msim( ds3gi_cm_client_id,
                                        ds3gi_cm_ss_event_cb,
                                        CM_CLIENT_EVENT_REG,
                                        CM_SS_EVENT_SRV_CHANGED,
                                        CM_SS_EVENT_SRV_CHANGED,
                                        NULL,
                                        SYS_MODEM_AS_ID_MASK_ANY );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_ss_reg_msim error %d",(int)(ret_val));
    }

    /*-----------------------------------------------------------------------
      Register to be notified the event, CM_SS_EVENT_INFO.
      The event is triggered by calling cm_ss_cmd_get_ss_info()
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_ss_reg_msim( ds3gi_cm_client_id,
                                        ds3gi_cm_ss_event_cb,
                                        CM_CLIENT_EVENT_REG,
                                        CM_SS_EVENT_INFO,
                                        CM_SS_EVENT_INFO,
                                        NULL,
                                        SYS_MODEM_AS_ID_MASK_ANY );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_mm_client_ss_reg_msim error %d",(int)(ret_val));
    }

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
    /*-----------------------------------------------------------------------
      Get Current Roam status from CM.
    -----------------------------------------------------------------------*/
    if ( FALSE == cm_ss_cmd_get_ss_info_msim(ds3gi_ss_cmd_info_cb,
                                             NULL,
                                             ds3gi_cm_client_id,
                                             SYS_MODEM_AS_ID_1) )
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_ss_cmd_get_ss_info_msim error %d",(int)(ret_val));
    }
    if ( FALSE == cm_ss_cmd_get_ss_info_msim(ds3gi_ss_cmd_info_cb,
                                             NULL,
                                             ds3gi_cm_client_id,
                                             SYS_MODEM_AS_ID_2) )
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_ss_cmd_get_ss_info_msim error %d",(int)(ret_val));
    }

#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
    if(!ds3gi_reg_ac_event())
    {
      DATA_ERR_FATAL("ds3gi_reg_ac_event error!");
    }
#endif
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_PS_DORMANT_PWR_SAVE
    /*-----------------------------------------------------------------------
      Register to be notified whenever service acquisition fails.
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_ss_reg_msim( ds3gi_cm_client_id,
                                        ds3gi_cm_ss_event_cb,
                                        CM_CLIENT_EVENT_REG,
                                        CM_SS_EVENT_PS_DATA_FAIL,
                                        CM_SS_EVENT_PS_DATA_FAIL,
                                        NULL,
                                        SYS_MODEM_AS_ID_MASK_ANY );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_ss_reg_msim error %d",(int)(ret_val));
    }

#endif /* FEATURE_PS_DORMANT_PWR_SAVE */



#ifdef FEATURE_DATA_ANDSF_RULE_MGR
    /*-----------------------------------------------------------------------
      Register to be notified whenever RSSI changes.
    -----------------------------------------------------------------------*/
    ret_val = cm_mm_client_ss_reg_msim( ds3gi_cm_client_id,
                                        ds3gi_cm_ss_event_cb,
                                        CM_CLIENT_EVENT_REG,
                                        CM_SS_EVENT_RSSI,
                                        CM_SS_EVENT_RSSI,
                                        NULL,
                                        SYS_MODEM_AS_ID_MASK_ANY );

    if(ret_val != CM_CLIENT_OK)
    {
       DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "cm_mm_client_ss_reg_msim error %d",(int)(ret_val));
    }

#endif /*FEATURE_DATA_ANDSF_RULE_MGR*/

    ret_val = cm_client_ph_reg( ds3gi_cm_client_id,
                                ds3gi_cm_ph_event_cb,
                                CM_CLIENT_EVENT_REG,
                                CM_PH_EVENT_DUAL_STANDBY_PREF,
                                CM_PH_EVENT_DUAL_STANDBY_PREF,
                                NULL );

    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_client_ph_reg error %d",(int)(ret_val));
    }

    ret_val = cm_client_ph_reg( ds3gi_cm_client_id,
                                ds3gi_cm_ph_event_cb,
                                CM_CLIENT_EVENT_REG,
                                CM_PH_EVENT_DATA_PRIORITY,
                                CM_PH_EVENT_DATA_PRIORITY,
                                NULL );

    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_client_ph_reg error %d",(int)(ret_val));
    }

    /*-----------------------------------------------------------------------
      CM function definition cm_client_subs_reg is featurized
    -----------------------------------------------------------------------*/
    ret_val = cm_client_subs_reg( ds3gi_cm_client_id,
                                  ds3gi_cm_ph_event_sub_pref_cb );

    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_client_subs_reg error %d",(int)(ret_val));
    }

#ifdef FEATURE_DATA_LTE
    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_ACT_BEARER_IND,
                           CM_CALL_EVENT_ACT_BEARER_IND,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_ACT_BEARER_IND, "
                      "error %d", (int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_GET_PDN_CONN_IND,
                           CM_CALL_EVENT_GET_PDN_CONN_IND,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_GET_PDN_CONN_IND, "
                      "error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_PDN_CONN_FAIL_IND,
                           CM_CALL_EVENT_PDN_CONN_FAIL_IND,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_PDN_CONN_FAIL_IND, "
                      "error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_PDN_CONN_REJ_IND,
                           CM_CALL_EVENT_PDN_CONN_REJ_IND,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_PDN_CONN_REJ_IND, "
                      "error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_MOD_BEARER_IND,
                           CM_CALL_EVENT_MOD_BEARER_IND,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_MOD_BEARER_IND, "
                      "error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_RES_ALLOC_REJ_IND,
                           CM_CALL_EVENT_RES_ALLOC_REJ_IND,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_RES_ALLOC_REJ, "
                      "error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_RES_ALLOC_FAIL_IND,
                           CM_CALL_EVENT_RES_ALLOC_FAIL_IND,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_RES_ALLOC_FAIL, "
                      "error %d",(int)(ret_val));
    }

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_BEARER_MOD_REJ_IND,
                           CM_CALL_EVENT_BEARER_MOD_REJ_IND,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_BEARER_MOD_REJ, "
                      "error %d",(int)(ret_val));
    }

#endif /* FEATURE_DATA_LTE */

#if  defined(FEATURE_DATA_LTE)

    ret_val = cm_mm_client_ss_reg_msim( ds3gi_cm_client_id,
                                        ds3gi_cm_ss_event_cb,
                                        CM_CLIENT_EVENT_REG,
                                        CM_SS_EVENT_IRAT_CTXT_TRANSFER,
                                        CM_SS_EVENT_IRAT_CTXT_TRANSFER,
                                        NULL,
                                        SYS_MODEM_AS_ID_MASK_ANY );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                       "DS unable to register CM_SS_EVENT_IRAT_CTXT_TRANSFER, "
                      "error %d",(int)(ret_val));
    }

#endif /* FEATURE_DATA_LTE && FEATURE_DS_UMTS_REARCH */

#if defined (FEATURE_DATA_IS707) || defined (FEATURE_DATA_RPM)
    ret_val = cm_client_ph_reg( ds3gi_cm_client_id,
                                ds3gi_cm_ph_event_cb,
                                CM_CLIENT_EVENT_REG,
                                CM_PH_EVENT_OPRT_MODE,
                                CM_PH_EVENT_OPRT_MODE,
                                NULL);

    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_client_ph_reg error %d",(int)(ret_val));
    }
#endif /* FEATURE_DATA_IS707 || FEATURE_DATA_RPM */

    ret_val = cm_mm_client_call_reg( ds3gi_cm_client_id,
                           ds3gi_cm_call_event_cb,
                           CM_CLIENT_EVENT_REG,
                           CM_CALL_EVENT_UNBLOCK_ALL_APN,
                           CM_CALL_EVENT_UNBLOCK_ALL_APN,
                           NULL
                         );
    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "DS enable to register CM_CALL_EVENT_UNBLOCK_ALL_APN, "
                      "error %d",(int)(ret_val));
    }

#ifdef FEATURE_DATA_IS707
    /*---------------------------------------------------------------------
     * register call back for event CM_CALL_EVENT_EXIT_TC. Since this event
     * CM_CALL_EVENT_EXIT_TC is a "call" event we are using the CM utility
     * "cm_mm_client_call_reg" to register with CM for this event.
     * CM_CALL_EVENT_EXIT_T event is
     * generated when the CDMA traffic channel has been torn down
     * Interested clients include ds707 EPZID Module.
     * ------------------------------------------------------------------*/
    if( cm_mm_client_call_reg(   ds3gi_cm_client_id,
                                 ds3gi_cm_call_event_cb,
                                 CM_CLIENT_EVENT_REG,
                                 CM_CALL_EVENT_EXIT_TC,
                                 CM_CALL_EVENT_EXIT_TC,
                                 NULL
                               ) != CM_CLIENT_OK)
    {
      DATA_ERR_FATAL("DS unable to register CM_CALL_EVENT_EXIT_TC with CM");
      return;
    }
#endif /* FEATURE_DATA_IS707 */

    ret_val = cm_client_ph_reg( ds3gi_cm_client_id,
                                ds3gi_cm_ph_event_cb,
                                CM_CLIENT_EVENT_REG,
                                CM_PH_EVENT_SYS_SEL_PREF,
                                CM_PH_EVENT_SYS_SEL_PREF,
                                NULL);

    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_client_ph_reg error %d",(int)(ret_val));
    }

    ret_val = cm_client_ph_reg( ds3gi_cm_client_id,
                                ds3gi_cm_ph_event_cb,
                                CM_CLIENT_EVENT_REG,
                                CM_PH_EVENT_SUBSCRIPTION_AVAILABLE,
                                CM_PH_EVENT_SUBSCRIPTION_CHANGED,
                                NULL);

    if(ret_val != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_client_ph_reg error %d",(int)(ret_val));
    }

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DS_LN_UPDATE
    /* Call Back function registration for CM_SS_EVENT_HDR_LN_STATUS */
    if( cm_mm_client_ss_reg_msim( ds3gi_cm_client_id,
                                  ds3gi_cm_ss_event_cb,
                                  CM_CLIENT_EVENT_REG,
                                  CM_SS_EVENT_HDR_LN_STATUS,
                                  CM_SS_EVENT_HDR_LN_STATUS,
                                  NULL,
                                  SYS_MODEM_AS_ID_MASK_ANY ) != CM_CLIENT_OK)
    {
      DATA_MSG0(MSG_LEGACY_FATAL,
                "DS Unable to register with CM_SS_EVENT_HDR_LN_STATUS");
    }
#endif /*FEATURE_DS_LN_UPDATE*/

    if(cm_data_oprt_mode_reg(ds3gi_cm_pwroff_lpm_mode_cb) != TRUE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Unable to register pwroff_lpm mode notification callback");
    }

    if(cm_data_subsc_cmd_reg_ext(ds3gi_cm_sub_not_avail_notify_cb) != TRUE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Unable to register CM SUB Not Avail notification cb");
    }

    ds707_pkt_cm_init(ds3gi_cm_client_id);
#endif /* FEATURE_DATA_IS707 */

    cm_client_ph_reg(ds3gi_cm_client_id,
                      ds3gi_cm_ph_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_PH_EVENT_INFO,
                      CM_PH_EVENT_INFO,
                      NULL);

    /*-----------------------------------------------------------------------
      Activate the registered callback functions.
    -----------------------------------------------------------------------*/
    status = cm_client_act( ds3gi_cm_client_id );
    if (status != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "cm_client_act failed, unable to activate client: %d",
                      status);
    }

#if defined (FEATURE_DEDICATED_C2K_MODE_DATA) || defined (FEATURE_DATA_RPM)
    /*-----------------------------------------------------------------------
      Call this function to get ph_info. This needs to be
      called every time the phone is reset, offline to online.
    -------------------------------------------------------------------------*/
    if(cm_ph_cmd_get_ph_info(NULL, NULL, ds3gi_cm_client_id) != TRUE)
    {
      DATA_MSG0(MSG_LEGACY_LOW, "Problem requesting initial CM PH info ");
    }
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA || FEATURE_DATA_RPM */

    /* Register for plmn change indication from CM */
    dsmsgrrcv_reg_hdlr(MM_CM_PLMN_LIST_CHANGE_IND,
                  (dsmsgrcv_msg_hdlr_f)ds3gi_cm_plmn_list_change_ind_msg_hdlr);
  }

 /*-----------------------------------------------------------------------
   Initialize the function ptr, This will be passed to CM when
   enabling/disabling LTE.
  -----------------------------------------------------------------------*/
  ds3g_cm_ph_cmd_cb_f_ptr = ds3g_cm_ph_sys_sel_cb;
  return;
} /* ds3gi_cm_init() */

/*===========================================================================
FUNCTION      DS3GI_GET_HDLR_TBL_PTR_FOR_CALL_INST

DESCRIPTION   Function that returns a pointer to the appropriate handler
              function table based on the current mode and call type.

DEPENDENCIES  None

RETURN VALUE  Handler function table pointer

SIDE EFFECTS  None
===========================================================================*/
ds3g_hdlr_func_tbl_type* ds3gi_get_hdlr_tbl_ptr_for_call_inst
(
  ds3gi_call_info_type  *call_inst_ptr
)
{
  uint32 call_mode = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(call_inst_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "call_inst_ptr is Null");
    return NULL;
  }

  if( (call_inst_ptr->call_mode  < SYS_SYS_MODE_NO_SRV) ||
      (call_inst_ptr->call_mode >= SYS_SYS_MODE_MAX) ||
      (call_inst_ptr->call_type >= DS_NUM_CALL_TYPES) ||
      (call_inst_ptr->call_type == DS_CALL_TYPE_UNSPECIFIED))
  {
     DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                     "Invalid System Mode %d Or Invalid Call Type %d",
                      call_inst_ptr->call_mode,
                      call_inst_ptr->call_type);
     return NULL;
  }
  else
  {
    call_mode = (uint32)call_inst_ptr->call_mode;
    return (ds3gi_hdlr_tbl[call_mode][call_inst_ptr->call_type]);
  }
} /*ds3gi_get_hdlr_tbl_ptr_for_call_inst() */

/*===========================================================================
FUNCTION      DS3GI_ALLOC_CALL_INSTANCE_RECORD

DESCRIPTION   Allocates a call state element

DEPENDENCIES  None

RETURN VALUE  If found, returns the call instance index.
              If not found, then returns DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3gi_alloc_call_instance_record
(
  ds_call_type_e_type  call_type,
  sys_sys_mode_e_type       call_mode,
   sys_modem_as_id_e_type   subs_id
)
{
  uint32 instance;
  uint32 cs_calls,ps_calls;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (call_mode == SYS_SYS_MODE_NONE) ||
      (call_mode >= SYS_SYS_MODE_MAX) )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid System Mode %d", call_mode);
    return DS3GI_INVALID_CALL_INSTANCE;
  }
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return DS3GI_INVALID_CALL_INSTANCE;
  }

  /*-------------------------------------------------------------------------
    First find the number of currently active calls
  -------------------------------------------------------------------------*/
  ds3gi_get_curr_active_calls_info_in_mode(&cs_calls,&ps_calls,call_mode);

  /*-------------------------------------------------------------------------
    Search for a free call instance
  -------------------------------------------------------------------------*/
  for (instance = 0; instance < DS3GI_MAX_NUM_CALLS; instance++)
  {
        if (ds3gi_call_info[instance].call_id == CM_CALL_ID_INVALID)
        {
          if(ds3gi_call_info[instance].call_state != DS3G_IDLE_CALL_STATE)
          {
            DATA_ERR_FATAL("Call is invalid but call state is not idle");
            return DS3GI_INVALID_CALL_INSTANCE;
          }
          break;
        }
  }
  /*-------------------------------------------------------------------------
    We have the total no. of calls for each type & possible a free call
    instance. See if we can allow any further calls of this type. If so
    return the instance, else return an error
  -------------------------------------------------------------------------*/
  if( instance < DS3GI_MAX_NUM_CALLS )
  {
    if( ( (cs_calls >= ds3gi_call_limit_info[call_mode].num_cs_calls)
            && (call_type == DS_CALL_TYPE_CKT) ) ||
        ( (ps_calls >= ds3gi_call_limit_info[call_mode].num_ps_calls)
            && (call_type == DS_CALL_TYPE_PKT) ) )
    {
      return DS3GI_INVALID_CALL_INSTANCE;
    }

    /*----------------------------------------------------------------------
      The total num of cs / ps calls has not reached max.
      check if we need to deny concurrent cs + PS calls
    ----------------------------------------------------------------------*/
    if(
        ( (cs_calls > 0) && (call_type == DS_CALL_TYPE_PKT ) )  ||
        ( (ps_calls > 0) && (call_type == DS_CALL_TYPE_CKT ) )
      )
    {
      if( ! ds3gi_call_limit_info[call_mode].cs_and_ps_allowed_flag  )
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                        " Concurrent PS + CS calls denied CS:%D PS:%d",
                        cs_calls,ps_calls);
        return DS3GI_INVALID_CALL_INSTANCE;
      }
    }

    /*----------------------------------------------------------------------
      return the instance allocated. Make a note of the call type, mode
      & change state for the instance allocated.
    ----------------------------------------------------------------------*/

    ds3gi_call_info[instance].call_type  = call_type;
    ds3gi_call_info[instance].call_mode  = call_mode;
    ds3gi_call_info[instance].subs_id    = subs_id;
    return instance;

  }
  return(DS3GI_INVALID_CALL_INSTANCE);
} /* ds3gi_alloc_call_instance_record() */

/*===========================================================================
FUNCTION      DS3GI_FREE_CALL_INSTANCE_RECORD

DESCRIPTION   Frees a call state element

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Sets the call state to DS3G_IDLE_CALL_STATE
===========================================================================*/
void ds3gi_free_call_instance_record
(
  uint32  instance
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(instance >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_ERR_FATAL("call instance greater than maximum allowed");
    return;
  }
  ds3gi_call_info[instance].call_state = DS3G_IDLE_CALL_STATE;
  ds3gi_call_info[instance].call_id    = CM_CALL_ID_INVALID;
  ds3gi_call_info[instance].call_was_connected = FALSE;
  ds3gi_call_info[instance].call_mode = SYS_SYS_MODE_NONE;
  ds3gi_call_info[instance].subs_id = SYS_MODEM_AS_ID_NONE;

  return;
} /* ds3gi_free_call_instance_record() */

/*===========================================================================
FUNCTION      DS3GI_FIND_CALL_INSTANCE

DESCRIPTION   Given a call_id, will return the index into the call_state array.
              If no instance is found, then returns DS3GI_INVALID_CALL_INSTANCE

DEPENDENCIES  None

RETURN VALUE  If found, returns the call instance.
              If not found, then returns DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3gi_find_call_instance
(
  cm_call_id_type  cm_call_id
)
{
  uint32  instance = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (instance = 0; instance < DS3GI_MAX_NUM_CALLS; instance++)
  {
    if (ds3gi_call_info[instance].call_id == cm_call_id)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Call_id %d found",instance);
      break;
    }
  }

  if (instance == DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Call_id is not found %d", cm_call_id);
    instance = DS3GI_INVALID_CALL_INSTANCE;
  }
  return(instance);
}  /* ds3gi_find_call_instance() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_END_CMD

DESCRIPTION   This function processes the DS_CM_CALL_END_CMD from Call
              Manager. This command is either a confirmation of a release
              initiated by 3G Dsmgr, or a notification of call release by the
              network. Note that if a data call is released from the UI, this
              will appear as a network call release.

DEPENDENCIES  The DS_CM_CALL_END_CMD command is valid in any call state,
              except IDLE. It is ignored if the call state is IDLE, or if
              the specified call id is not recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_end_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_END_CMD");

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  if (call_info_cmd_ptr->call_id == CM_CALL_ID_INVALID)
  {
    if (call_info_cmd_ptr->call_inst != DS3GI_INVALID_CALL_INSTANCE)
    {
      call_inst = call_info_cmd_ptr->call_inst;
      call_info_cmd_ptr->call_id = ds3gi_call_info[call_inst].call_id;
      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                      "CALL_END_CMD call_inst %d. Call Id %d ",
                      call_info_cmd_ptr->call_inst,
                      ds3gi_call_info[call_inst].call_id);
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                      "CM_CALL_END_CMD with bad call_id:%d & bad call_inst:%d",
                      call_info_cmd_ptr->call_id,
                      call_info_cmd_ptr->call_inst);
    }
  }
  else
  {
    call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  }

  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "CM_CALL_END_CMD with bad call_id %d",
                    call_info_cmd_ptr->call_id);
  }
  else if ( ds3gi_call_info[call_inst].call_state == DS3G_IDLE_CALL_STATE )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "CM_CALL_END_CMD rx'ed in Idle call state");
    ds3gi_call_info[call_inst].call_id = CM_CALL_ID_INVALID;
  }
  else
  {
    /*---------------------------------------------------------------------
      Invoke a function to perform the actual work to end the call.
    ---------------------------------------------------------------------*/
    ds3gi_end_call(cmd_ptr);
  }
  return;
} /* ds3gi_process_cm_call_end_cmd() */

/*===========================================================================
FUNCTION      DS3GI_CHK_INCOM_CALL_AGAINST_TYPE_LIMITS

DESCRIPTION   This function checks the incoming call type against the
              call limit table.  If the number of allowed calls is
              exceeded, the incoming call will be flagged for switch.

DEPENDENCIES  This is an internal function to be used by the incoming command
              processing function

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_chk_incom_call_against_type_limits
(
  sys_sys_mode_e_type        inc_call_mode,       /* inc call's mode       */
  ds_call_type_e_type        inc_call_type,       /* incoming call type    */
  ds3gi_call_control_type   *cc_info_ptr          /* call control info     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cc_info_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL passed for cc_info_ptr");
    return;
  }

  if(inc_call_mode == SYS_SYS_MODE_NONE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"System Mode NONE; Returning");
    return;
  }

  /*----------------------------------------------------------------------
    Check to see if we allow multiple calls of the same type.
  -----------------------------------------------------------------------*/
  if( ( ( inc_call_type == DS_CALL_TYPE_CKT) &&
        ( ds3gi_call_limit_info[inc_call_mode].num_cs_calls > 1 ) ) ||
      ( ( inc_call_type == DS_CALL_TYPE_PKT) &&
        ( ds3gi_call_limit_info[inc_call_mode].num_ps_calls > 1 ) ) )
  {
    /*----------------------------------------------------------------------
      Multiple calls of same type are allowed. Allow incoming to go thru
      based on resource availablity.
    -----------------------------------------------------------------------*/
    cc_info_ptr->switch_inc_call = FALSE;
    cc_info_ptr->accept_inc_call = TRUE;
    cc_info_ptr->action = DS3G_ANSWER_CALL;
  }
  else
  {
    /*---------------------------------------------------------------------
      Multiple calls of same type NOT allowed. Switch and handle incoming
      call only if it is of same system mode, else terminate outgoing call.
    ----------------------------------------------------------------------*/
    uint32 cur_calls = (inc_call_type == DS_CALL_TYPE_CKT)?
                       cc_info_ptr->cur_cs_calls :
                       cc_info_ptr->cur_ps_calls;

    cc_info_ptr->switch_inc_call = (cur_calls > 0)? TRUE : FALSE;
    cc_info_ptr->accept_inc_call = TRUE;
    cc_info_ptr->action = DS3G_ANSWER_CALL;
  }
  return;
} /* ds3gi_chk_incom_call_against_type_limits() */

/*===========================================================================
FUNCTION      DS3GI_CHK_INCOM_CALL_AGAINST_CUR_CALLS

DESCRIPTION   This function checks to see how the incoming call should be
              handled. Either it is accepted,
              existing outgoing call switched & incoming call accepted,
              existing outgoing call forced down & inc call accepted

              It sets the appropriate output params to the right values
              accept_inc_call : TRUE indicates accept the inc call
              switch_inc_call : TRUE indicates switch the calls
              force_cur_call_down : TRUE indicates force the current call down

              The valid combinations of the output parameters are
              accept_inc_call is  TRUE
              accept_inc_call is TRUE && switch_inc_call is TRUE
              accept_inc_call is TRUE && force_cur_call_down is TRUE

DEPENDENCIES  None :
              This is an internal function to be used by the incoming command
              processing function

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_chk_incom_call_against_cur_calls
(
  sys_sys_mode_e_type        inc_call_mode,       /* inc call's mode        */
  ds_call_type_e_type        inc_call_type,       /* incoming call type     */
  ds3gi_call_control_type   *cc_info_ptr          /* call control info      */
)
{
                                              /* ps &cs calls in other modes*/
  uint32    cur_cs_calls_in_other_modes;      /* Act cs calls               */
  uint32    cur_ps_calls_in_other_modes;      /* ACt ps calls               */
  boolean   conc_call_attempt=FALSE;          /* is CS+PS attempted         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cc_info_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL passed for cc_info_ptr");
    return;
  }
  /*-------------------------------------------------------------------------
    Initialize the outputs first to known states
  -------------------------------------------------------------------------*/
  cc_info_ptr->cur_cs_calls    = 0;
  cc_info_ptr->cur_ps_calls    = 0;
  cc_info_ptr->accept_inc_call = FALSE;
  cc_info_ptr->switch_inc_call = FALSE;
  cc_info_ptr->force_cur_call_down = FALSE;
  cc_info_ptr->action = DS3G_REJECT_CALL;

  if( (inc_call_mode == SYS_SYS_MODE_NONE) ||
      (inc_call_mode >= SYS_SYS_MODE_MAX) )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Invalid System Mode %d", inc_call_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    First see if there calls in other modes active at this time..
  -------------------------------------------------------------------------*/
  ds3gi_get_curr_active_calls_info_not_in_mode(&cur_cs_calls_in_other_modes,
                                               &cur_ps_calls_in_other_modes,
                                               inc_call_mode);

  /*-------------------------------------------------------------------------
    If calls are active in other modes, ther is a mismatch between CM & DS.
    CM should have performed handover of all calls  to a single system. We
    cannot handle multiple active calls in multiple modes.
  -------------------------------------------------------------------------*/
  if((cur_cs_calls_in_other_modes != 0) &&
          (cur_ps_calls_in_other_modes != 0))
  {
    DATA_ERR_FATAL("Calls active in other modes");
    return;
  }

  /*-------------------------------------------------------------------------
   For the mode specif. see if there is any outgoing calls active at this time
  -------------------------------------------------------------------------*/
  ds3gi_get_curr_active_calls_info_in_mode(&cc_info_ptr->cur_cs_calls,
                                           &cc_info_ptr->cur_ps_calls,
                                           inc_call_mode);

  /*-------------------------------------------------------------------------
    We use the INCOMING MODE to base all our decisions below. CM has to ensure
    that calls are not proceeding across multiple MODES at this time!!.

    If no calls are in progress/origination,
    accept the incoming call
    If we have any calls in progress, Depending on the system mode,
    we need to do one of the foll
    a) Multiple calls not allowed
       term outgoing call & accept incom if only ONE data call is allowed
       for that system ( no concurrent cs + ps call support ) and outgoing
       call is in orig state or else ignore incoming call
    b) Multiple calls are allowed, but concurent calls are not allowed.
       In that case,
       deny incoming call and leave existing calls as is
    c) Multiple calls are allowed  for concurrent call types
      - allow calls to go through based on resource availability
  -------------------------------------------------------------------------*/

  if( ( (inc_call_type == DS_CALL_TYPE_CKT) &&
        (cc_info_ptr->cur_ps_calls > 0) ) ||
      ( (inc_call_type == DS_CALL_TYPE_PKT) &&
        (cc_info_ptr->cur_cs_calls > 0) ) )
  {
    conc_call_attempt = TRUE;
  }

  if( cc_info_ptr->cur_cs_calls == 0 && cc_info_ptr->cur_ps_calls == 0 )
  {
    /*------------------------------------------------------------------------
      No calls in progress.Set the flag to accept the inc call & fall through
    ------------------------------------------------------------------------*/
    cc_info_ptr->accept_inc_call = TRUE;
    cc_info_ptr->action = DS3G_ANSWER_CALL;
  }
  else if(!conc_call_attempt)
  {
    /*----------------------------------------------------------------------
      Check to see if we allow multiple calls of the same type
    -----------------------------------------------------------------------*/
    ds3gi_chk_incom_call_against_type_limits( inc_call_mode,
                                              inc_call_type,
                                              cc_info_ptr );

  }  /* if(!conc_call_attempt ) */
  else if( !ds3gi_call_limit_info[inc_call_mode].cs_and_ps_allowed_flag )
  {
    /*-------------------------------------------------------------------------
      It is a concurrent call & we do not allow concurrent calls
      Terminate incoming/outgoing the call depending on the system.
      On a single call system, terminate any outgoing call and accept inc call
      On multi call system, reject the incoming call
    -------------------------------------------------------------------------*/
    if( ( ds3gi_call_limit_info[inc_call_mode].num_cs_calls == 1 ) &&
        ( ds3gi_call_limit_info[inc_call_mode].num_ps_calls == 1 ) )
    {
      cc_info_ptr->force_cur_call_down = TRUE;
      cc_info_ptr->accept_inc_call = TRUE;
      cc_info_ptr->action = DS3G_ANSWER_CALL;
    }
    else
    {
      /*-----------------------------------------------------------------------
       We would not know which outgoing call to terminate if we have a conc.
       call where we allow multiple PS or CS calls. Just let the inc call
       terminate.All flags would be false.So no special processing required here
      -----------------------------------------------------------------------*/
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "M-call capable:Rej inc call of diff type:%d",
                      inc_call_type);
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "DS already in data call, reject inc call");
      cc_info_ptr->accept_inc_call = FALSE;
      cc_info_ptr->action = DS3G_REJECT_CALL;
    }
  } /* if( conc_call_attempt && !ds3gi_call_limit_info.....) */
  else
  {
    /*----------------------------------------------------------------------
      Check to see if we allow multiple calls of the same type
    -----------------------------------------------------------------------*/
    ds3gi_chk_incom_call_against_type_limits( inc_call_mode,
                                              inc_call_type,
                                              cc_info_ptr );

  } /* if (conc_call_attempt && ds3gi_call_limit_info[inc_cal....) */

  return;
} /* ds3gi_chk_incom_call_against_cur_calls() */

/*===========================================================================
FUNCTION      DS3GI_MANAGE_TERMINATING_CALLS

DESCRIPTION   This function manages multiple call scenarios. For the mode
              specified, it checks to see if multiple calls can be allowed
              and based on the policy either allows,denies/switches calls.

              This function is invoked from multiple handlers:
                SETUP handler    - for GSM/WCDMA calls
                INCOMING handler - for other modes

DEPENDENCIES  Assumes that simultaneous orig + inc call are always same
              technology.

RETURN VALUE  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS  Updates the call_state, serial_state, at_state
===========================================================================*/
boolean ds3gi_manage_terminating_calls
(
  ds_cmd_type               *cmd_ptr,            /* Pointer to the command */
  sys_sys_mode_e_type        inc_call_mode,      /* inc call's mode        */
  ds_call_type_e_type        inc_call_type,      /* incoming call type     */
  ds3gi_call_control_type   *cc_info_ptr         /* call control info      */
)
{
  ds3g_call_info_cmd_type    *call_info_cmd_ptr = NULL;
  boolean                     ret_val = DS3G_SUCCESS;
  ds3g_hdlr_func_tbl_type    *hdlr_tbl_ptr = NULL;   /* Ptr to mode-sp. hdlr table */
  cm_call_id_type             exist_call_id;  /* existing call's call id    */
  uint32                      force_down_call_inst= DS3GI_INVALID_CALL_INSTANCE;
  boolean                     cm_ret_val = FALSE;

  /* End params for the current originating calls.
     The param is an array.  Each element contains the parameter of individual
     originating calls to be ended.
     We only support one call to be ended if requrired for now.
  */
  cm_end_params_s_type        end_params_orig;
  cm_client_id_type           ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    ASSERT(0);
    return DS3G_FAILURE;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /*-------------------------------------------------------------------------
    Find out how the incoimg call is to be handled ?
  -------------------------------------------------------------------------*/
  ds3gi_chk_incom_call_against_cur_calls(inc_call_mode,
                                         inc_call_type,
                                         cc_info_ptr );

  ds3g_cm_client_id = ds3g_get_cm_client_id();

  /*-------------------------------------------------------------------------
    Now based on the flags set by ds3gi_chk_incom_call_against_cur_calls
    process the incoming call.
    First: check to see if we need to force existing call or switch calls
    One of the below combinations would be true
    1) accept_inc_call & force_cur_call_down: one call system & inc call
    is of different type than that of existing call
    2) switch_inc_call & accept_inc_call: one call system & inc call is of
    same type as existing call
    3) accept_inc_call  only : when this is the first call or multiple calls
    can be allowed based on resource availability
  -------------------------------------------------------------------------*/
  if( cc_info_ptr->force_cur_call_down && cc_info_ptr->accept_inc_call )
  {
    /*-----------------------------------------------------------------------
      Override to reject the incoming call unless we are suceesfull in
      forcing the current call down.
    -----------------------------------------------------------------------*/
    cc_info_ptr->accept_inc_call = FALSE;
    cc_info_ptr->action = DS3G_REJECT_CALL;

    /*-----------------------------------------------------------------------
      If more than one call is in progress, we cannot force the calls down
      ignore incoming call.
    -----------------------------------------------------------------------*/
    if( cc_info_ptr->cur_ps_calls > 1 || cc_info_ptr->cur_cs_calls > 1 )
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                      "Err forcing call down.Mul calls act:C:%d P:%d",
                      cc_info_ptr->cur_cs_calls,cc_info_ptr->cur_ps_calls);
    }
    else
    {
      /*----------------------------------------------------------------------
        Get the first call on the other call type in progress
        You can force a call down only if it is in ORIG state.
      ----------------------------------------------------------------------*/
      force_down_call_inst =
        ds3gi_find_first_call_in_progress(
         (inc_call_type == DS_CALL_TYPE_CKT) ? DS_CALL_TYPE_PKT:DS_CALL_TYPE_CKT,
         inc_call_mode );

      if(force_down_call_inst < DS3GI_MAX_NUM_CALLS )
      {
        if(ds3gi_call_info[force_down_call_inst].call_state == DS3G_ORIG_CALL_STATE)
        {
          /*---------------------------------------------------------------------
           Get a pointer to the appropriate handler function table based on the
           current mode and call type.
          ----------------------------------------------------------------------*/
          hdlr_tbl_ptr =
                ds3gi_get_hdlr_tbl_ptr_for_call_inst(
                   &(ds3gi_call_info[force_down_call_inst]));

          if (hdlr_tbl_ptr == NULL)
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"hdlr_table_ptr is Null");
            return DS3G_FAILURE;
          }

          exist_call_id = ds3gi_call_info[force_down_call_inst].call_id;

          if((hdlr_tbl_ptr->force_call_down_handler != NULL) )
          {
            if( TRUE == hdlr_tbl_ptr->force_call_down_handler(
                                          exist_call_id,
                                          &end_params_orig,
                                          call_info_cmd_ptr->subs_id))
            {
              ds3gi_call_info[force_down_call_inst].call_state = DS3G_END_CALL_STATE;
              cm_ret_val = cm_mm_call_cmd_exten_end(
                                           ds3gi_cm_call_cmd_end_cb,
                                           NULL,
                                           ds3g_cm_client_id,
                                           1,
                                           &end_params_orig
                                         );
              if (cm_ret_val == FALSE)
              {
                DATA_ERR_FATAL("CM call cmd end failed");
                return DS3G_FAILURE;
              }
              else
              {
                /*-------------------------------------------------------------
                  Existing call is ended.  Free up the call instance now
                  If the # call_instances is set to 1, then we have only one
                  call instance to work with. Free it & reuse it. When CM call
                  end event comes through, we will ignore it.
                --------------------------------------------------------------*/
                ds3gi_free_call_instance_record( force_down_call_inst );
                cc_info_ptr->accept_inc_call = TRUE;
                cc_info_ptr->action = DS3G_ANSWER_CALL;
              }
            } /* if (hdlr_tbl_ptr->force_call_down_handler(exist_call_id,...) */
          }
          else
          {
           DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                           "force down handler not reg. Reject inc call");
          }
        } /* if(ds3gi_call_info[force_down_call_inst].call_state .......) */
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                          "Can force cur call down-not in orig. Rej inc call");
        }
      } /* if(force_down_call_inst != DS3GI_INVALID_CALL_INSTANCE ) */
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Cannot locate call to be forced down: Rej inc call");
      }
    } /* else((cur_ps_calls > 1 || cur_cs_calls > 1 ) */
  } /* if(force_cur_call_down && accept_inc_call ) */

  if( cc_info_ptr->switch_inc_call && cc_info_ptr->accept_inc_call )
  {
    /*------------------------------------------------------------------------
      Switch the calls in progress.

      If there is a Mhandler function to switch the call,the incoming call
      gets processed & call ids are swapped.
      Based on the result (action), further processing is done.
    ------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------
      If more than one call is in progress, we cannot switch the calls
      reject incoming call.
    ------------------------------------------------------------------------*/
    if( cc_info_ptr->cur_ps_calls > 1 || cc_info_ptr->cur_cs_calls > 1 )
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                      "Not Switching calls.Mul calls act:C:%d P:%d",
                      cc_info_ptr->cur_cs_calls,cc_info_ptr->cur_ps_calls);
    }
    else
    {
      /*------------------------------------------------------------------------
        Get the call instance of the call already in progress (same call type)
        You can force a switch only if the current call is in ORIG state.
      ------------------------------------------------------------------------*/
      cc_info_ptr->switch_call_inst =
        ds3gi_find_first_call_in_progress( inc_call_type,
                                           inc_call_mode );

      if( cc_info_ptr->switch_call_inst < DS3GI_MAX_NUM_CALLS )
      {
        if( ds3gi_call_info[cc_info_ptr->switch_call_inst].call_state ==
            DS3G_ORIG_CALL_STATE)
        {
          ds3gi_call_info_type * call_info_ptr =
            &ds3gi_call_info[cc_info_ptr->switch_call_inst];

          if( (call_info_ptr->call_mode == SYS_SYS_MODE_NONE) ||
              (call_info_ptr->call_type == DS_NUM_CALL_TYPES) )
          {
            DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                            "Invalid System Mode %d Or Invalid Call Type %d",
                            call_info_ptr->call_mode,call_info_ptr->call_type);
            return DS3G_FAILURE;
          }

          hdlr_tbl_ptr = ds3gi_hdlr_tbl[ call_info_ptr->call_mode ]
                                       [ call_info_ptr->call_type ];
          exist_call_id = call_info_ptr->call_id;

          if ((hdlr_tbl_ptr != NULL) &&
            (hdlr_tbl_ptr->switch_call_handler != NULL))
          {
            DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                            "Switch calls hdlr. inc_call_id:%d exist_call_id:%d",
                            cc_info_ptr->inc_call_id, exist_call_id);

            cc_info_ptr->action =
              hdlr_tbl_ptr->switch_call_handler( cmd_ptr,
                                                 cc_info_ptr->inc_call_id,
                                                 exist_call_id,
                                                 &cc_info_ptr->ans_params,
                                                 &end_params_orig,
                                                 &cc_info_ptr->end_params_inc,
                                                 call_info_cmd_ptr->subs_id);

            if (cc_info_ptr->action == DS3G_REJECT_CALL )
            {
              cc_info_ptr->accept_inc_call = FALSE;
            }

            /*----------------------------------------------------------------
            End the existing call with CM.  Incoming call is accepted/rejected
            further down. When CM call_end is received for the existing call,
            we would ignore it.
            ------------------------------------------------------------------*/
            cm_ret_val = cm_mm_call_cmd_exten_end(
                                        ds3gi_cm_call_cmd_end_cb,
                                        NULL,
                                        ds3g_cm_client_id,
                                        1,
                                        &end_params_orig
                                      );
            if (cm_ret_val == FALSE)
            {
              DATA_ERR_FATAL("CM call cmd end failed");
              return DS3G_FAILURE;
            }

            /*--------------------------------------------------------------
              Note the call id of the inc call in the call instance
              that we have used to switch to: reuse the call instance.
              Call type of the instance does not change
            --------------------------------------------------------------*/
            ds3gi_call_info[cc_info_ptr->switch_call_inst].call_id =
              call_info_cmd_ptr->call_id;
          } /* if ((hdlr_tbl_ptr != NULL) && .... */
          else
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                            "MH switch hdlr not registered.ignoring inc call");
            cc_info_ptr->action = DS3G_REJECT_CALL;
            cc_info_ptr->accept_inc_call = FALSE;
          }
        } /* if( ds3gi_call_info[switch_call_inst].call_state .....) */
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                          "Cannot switch call:cur call is not in ORIG st");
          cc_info_ptr->action = DS3G_REJECT_CALL;
          cc_info_ptr->accept_inc_call = FALSE;
        }
      } /* if(switch_call_inst != DS3GI_INVALID_CALL_INSTANCE ) */
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Cannot locate existing call to switch call");
        cc_info_ptr->action = DS3G_REJECT_CALL;
        cc_info_ptr->accept_inc_call = FALSE;
      }
    } /* else (cur_ps_calls > 1 || cur_cs_calls > 1 ) */
  } /* if( switch_inc_call && accept_inc_call) */

  return ret_val;
} /* ds3gi_manage_terminating_calls() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_INCOM_CMD

DESCRIPTION   This function processes the DS_CM_CALL_INCOM_CMD from Call
              Manager. It calls the mode-specific incoming_call_handler, and
              then either answers the incoming call, rings the serial port or
              rejects the incoming call (i.e. does nothing), depending on the
              return value from the handler function.

DEPENDENCIES  Assumes that simultaneous orig + inc call are always same
              technology.


RETURN VALUE  None

SIDE EFFECTS  Updates the call_state, serial_state, at_state
              and may change the serial mode to Rawdata or Packet.
===========================================================================*/
void ds3gi_process_cm_call_incom_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3gi_call_control_type  *cc_info;        /* call control information   */
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;   /* Ptr to mode-sp. hdlr table */
  uint32                    call_inst = DS3GI_DEFAULT_CALL_INSTANCE;
  boolean                   cm_ret_val = FALSE;
  ds_call_type_e_type       inc_call_type;  /* incoming call type         */
  uint32                    inc_call_inst= DS3GI_INVALID_CALL_INSTANCE;
  sys_sys_mode_e_type       inc_call_mode;
  cm_client_id_type         ds3g_cm_client_id;
  void                      *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "Rx'ed CM_CALL_INCOM_CMD, call id: %d",
                  call_info_cmd_ptr->call_id);

  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);
  if( (inc_call_mode == SYS_SYS_MODE_NO_SRV) ||
      (inc_call_mode == SYS_SYS_MODE_NONE) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Call incoming failed - CM says NO SRV mode");
    return;
  }

  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);

  if( (cc_info = (ds3gi_call_control_type*) modem_mem_alloc
                 (sizeof(ds3gi_call_control_type),
                  MODEM_MEM_CLIENT_DATA_CRIT)) == NULL )
  {
    DATA_MSG0(MSG_LEGACY_FATAL,"Heap Memory Allocation failed");
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize call control state information
  -------------------------------------------------------------------------*/
  cc_info->accept_inc_call = FALSE;
  cc_info->switch_inc_call = FALSE;
  cc_info->force_cur_call_down = FALSE;
  cc_info->inc_call_id   = call_info_cmd_ptr->call_id;
  cc_info->action = DS3G_REJECT_CALL;
  cc_info->switch_call_inst = DS3GI_INVALID_CALL_INSTANCE;

  cc_info->end_params_inc.call_id = call_info_cmd_ptr->call_id;
  cc_info->end_params_inc.info_type = call_info_cmd_ptr->mode_info.info_type;
  if ( cc_info->end_params_inc.info_type == CM_CALL_MODE_INFO_CDMA )
  {
    cc_info->end_params_inc.end_params.cdma_end.end_reason_included = FALSE;
  }

  /*-------------------------------------------------------------------------
    Manage the terminating call based on the existing call allocations.
    Note ETSI CS data calls are managed in the call setup handler so are
    exempt here.
  -------------------------------------------------------------------------*/
  if(!( (DS_CALL_TYPE_CKT == inc_call_type) &&
        DS3G_CALL_IS_ETSI_MODE( inc_call_mode ) ))
  {
    /*-------------------------------------------------------------------------
      General data calls.
    -------------------------------------------------------------------------*/
    if( DS3G_SUCCESS != ds3gi_manage_terminating_calls( cmd_ptr,
                                                        inc_call_mode,
                                                        inc_call_type,
                                                        cc_info ))
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Cannot manage terminating call");
      modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );

      return;
    }
  }
  else
  {
    /*-------------------------------------------------------------------------
      ETSI CS data calls.  Automaticaly accept the incoming call.
    -------------------------------------------------------------------------*/
    cc_info->accept_inc_call = TRUE;
    cc_info->switch_inc_call = FALSE;
  }

  if( cc_info->accept_inc_call )
  {
    /*------------------------------------------------------------------------
      At this point, we are either in one of the following states
      a) accepting the call
      b) forced the current call down and ready to accept inc call
      c) switched the call id of curr call & ready to accept inc call
    ------------------------------------------------------------------------*/
    if( !cc_info->switch_inc_call )
    {
      /*-------------------------------------------------------------------
        Point to the appropriate handler function table based on the current
        mode and call type.
      --------------------------------------------------------------------*/
      hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

      /*--------------------------------------------------------------------
        Ensure that a handler function table is registered, with a non-null
        incoming_call_handler.
      --------------------------------------------------------------------*/
      if( hdlr_tbl_ptr != NULL )
      {
        if( hdlr_tbl_ptr->incoming_call_handler != NULL )
        {
          /*------------------------------------------------------------------
            ETSI circuit-switched data calls are allocated in call setup
            state.  All other calls are allocated here in incoming state.
          ------------------------------------------------------------------*/
          if( (DS_CALL_TYPE_CKT == inc_call_type) &&
              DS3G_CALL_IS_ETSI_MODE( inc_call_mode ) )
          {
            inc_call_inst =
              ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
          }
          else
          {
            /*---------------------------------------------------------------
            Need to allocate a call instance for the incoming call
            ---------------------------------------------------------------*/
            inc_call_inst = ds3gi_alloc_call_instance_record(inc_call_type,
                                                             inc_call_mode,
                                               call_info_cmd_ptr->subs_id);
          }

          if( inc_call_inst >= DS3GI_MAX_NUM_CALLS )
          {
            DATA_3GMGR_MSG3(
               MSG_LEGACY_HIGH,
               "Bad call id:%d Or call inst alloc failed:inst:%d type:%d",
               call_info_cmd_ptr->call_id,
               inc_call_inst,
               inc_call_type );

            cc_info->action = DS3G_REJECT_CALL;
          }
          else
          {
            ds3gi_call_info[inc_call_inst].call_id = call_info_cmd_ptr->call_id;
            ds3gi_call_info[inc_call_inst].call_type = inc_call_type;

            /*--------------------------------------------------------------
             Intialize the parameters before invoking mode specific handler.
           --------------------------------------------------------------*/
            memset((void*)&(cc_info->ans_params),
                   0,
                   sizeof(cm_ans_params_s_type));
            memset((void*)&(cc_info->end_params_inc),
                   0,
                   sizeof(cm_end_params_s_type));

            /*--------------------------------------------------------------
              Invoke the mode-specific handler function.
            --------------------------------------------------------------*/
            cc_info->action = hdlr_tbl_ptr->incoming_call_handler
                                              (
                                                cmd_ptr,
                                                cc_info->inc_call_id,
                                                &cc_info->ans_params,
                                                &cc_info->end_params_inc,
                                                call_info_cmd_ptr->subs_id
                                              );
          }
        } /* if( hdlr_tbl_ptr->incoming_call_handler != NULL ) */
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                          "Incoming call handler not registered");
          cc_info->action = DS3G_REJECT_CALL;
        }
      } /* if hdlr_tbl_ptr != NULL */

    } /* if(!cc_info->switch_inc_call) */
    else
    {
      /*----------------------------------------------------------------------
        We have switched calls. Just note the call inst. of the switched call
      ----------------------------------------------------------------------*/
      inc_call_inst = cc_info->switch_call_inst ;
    }
  } /* if( cc_info->accept_inc_call ) */

  /*-------------------------------------------------------------------------
    At this point, the incoming call has been processed by the mode-
    specific handler.  Do next action based on results of mode-specific
    handler return value.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    If the mode specific handler returns ignore, then completely ignore the
    incoming call. Do not save any information regarding the call either.
  -------------------------------------------------------------------------*/
  if( cc_info->action == DS3G_IGNORE )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode_spec hdlr sez ignore");

    /*-----------------------------------------------------------------------
      If we have allocated a call instance, Free the call record only if
      we have'nt switched the call instance. For switched calls, the inc
      call is just ignored & no resource clean up is required
    -----------------------------------------------------------------------*/
    if( (inc_call_inst < DS3GI_MAX_NUM_CALLS) && (!cc_info->switch_inc_call) )
    {
      ds3gi_free_call_instance_record( inc_call_inst );
    }
    modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );

    return;
  }

  if ( cc_info->action != DS3G_REJECT_CALL )
  {
    if(inc_call_inst >= DS3GI_MAX_NUM_CALLS)
    {
      modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );
      DATA_ERR_FATAL("call instance value greater than Maximum calls");
      return;
    }
    ds3gi_call_info[inc_call_inst].call_id = call_info_cmd_ptr->call_id;
    ds3gi_call_info[inc_call_inst].call_type = inc_call_type;

    /*-------------------------------------------------------------------------
      Keep a note of the call type and the mode for the incoming call
      this will be used to find the mode handler when the call has ended
      to retrieve statistics.
      HACK : REMOVE THIS WHEN ACTUAL CM support for multipe calls are added
    -------------------------------------------------------------------------*/
    ds3g_set_last_call_mode(ds3gi_call_info[call_inst].call_mode);
    ds3g_set_last_call_type(ds3gi_call_info[call_inst].call_type);
  }

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  if( cc_info->action == DS3G_ANSWER_CALL )
  {
    /*-----------------------------------------------------------------------
      Set the call state to indicate that we are waiting for the call
      to be connected.
    -----------------------------------------------------------------------*/
    if(inc_call_inst >= DS3GI_MAX_NUM_CALLS)
    {
      modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );
      DATA_ERR_FATAL("call instance value greater than Maximum calls");
      return;
    }
    ds3gi_call_info[inc_call_inst].call_state = DS3G_WAIT_FOR_CONN_CALL_STATE;

    data_block_ptr = (void *)(inc_call_inst);

    /*-----------------------------------------------------------------------
      Now send a command to Call Manager to answer the call.
    -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode-Sp. Hdlr answers call");
    cm_ret_val = cm_mm_call_cmd_answer(
                                        ds3gi_cm_call_cmd_cb,
                                        data_block_ptr,
                                        ds3g_cm_client_id,
                                        ds3gi_call_info[inc_call_inst].call_id,
                                        &cc_info->ans_params
                                      );
    if (cm_ret_val == FALSE)
    {
      modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );
      DATA_ERR_FATAL("CM call cmd answer failed");
      return;
    }
  }
  else if( cc_info->action == DS3G_ALERT_USER )
  {
    /*-----------------------------------------------------------------------
      Set the call state to indicate that we are alerting the user,
      and call a function to perform the alerting.
    -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode-Sp. Hdlr says alert user");
    if (inc_call_inst >= DS3GI_MAX_NUM_CALLS)
    {
       modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );
       DATA_ERR_FATAL("call_instance greater than Maximum calls");
       return;
    }
    ds3gi_call_info[inc_call_inst].call_state = DS3G_RINGING_CALL_STATE;
  }
  else if( cc_info->action == DS3G_REJECT_CALL )
  {
    /*-----------------------------------------------------------------------
      For some reason, the mode-specific handler rejected the incoming
      call / we failes some cases above which leads to the inc call being
      rejected. Do not free the call instance record. We will free it when
      CM call end event is received
    -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode-Sp. Hdlr rejected incoming call");

    cm_ret_val = cm_mm_call_cmd_exten_end(
                                     ds3gi_cm_call_cmd_end_cb,
                                     NULL,
                                     ds3g_cm_client_id,
                                     1,
                                     &cc_info->end_params_inc
                                   );
    if (cm_ret_val == FALSE)
    {
      modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );
      DATA_ERR_FATAL("CM call cmd end failed");
      return;
    }
    /*-----------------------------------------------------------------------
      If we have allocated a call instance, mark the call state and free
      the call record. This was not done earlier. The call instance should
      be freed to ensure that the CALL_END notification from CM gets ignored
    -----------------------------------------------------------------------*/
    if( inc_call_inst < DS3GI_MAX_NUM_CALLS )
    {
      ds3gi_call_info[inc_call_inst].call_state = DS3G_END_CALL_STATE;
      ds3gi_free_call_instance_record( inc_call_inst );
    }
  }
  else if( cc_info->action == DS3G_DO_NOTHING )
  {
    if(inc_call_inst >= DS3GI_MAX_NUM_CALLS)
    {
      modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );
      DATA_ERR_FATAL("call instance is greater than maximum calls");
      return;
    }
    /*-----------------------------------------------------------------
      Set the call state to indicate that we are waiting for the call
      to be connected, as we could receive a CONNECT still (example
      is bypass_alert).
    -----------------------------------------------------------------*/
    ds3gi_call_info[inc_call_inst].call_state = DS3G_WAIT_FOR_CONN_CALL_STATE;
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Mode_spec hdlr sez do nothing");
  }

  modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );

  return;
} /* ds3gi_process_cm_call_incom_cmd() */

#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_INCOMING_PAGE_CMD

DESCRIPTION   This function processes the DS_CM_CALL_INCOMING_PAGE_CMD from
              Call Manager.
              It checks whether the incoming page can be answered or should
              be rejected.
              And returns the same to CM.
              This is a new event from CM, for SVDO/SVLTE, where if DS is going
              to accept the incoming page, the DO/LTE stack will be shut down.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_incoming_page_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  boolean                   cm_ret_val = FALSE;
  ds_call_type_e_type       inc_call_type;
  sys_sys_mode_e_type       inc_call_mode;
  ds3g_incom_e_type         page_response = DS3G_REJECT_CALL;
  cm_client_id_type         ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0( MSG_LEGACY_HIGH, "Rx'ed CM_CALL_INCOMING_PAGE_CMD" );

  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);
  if( (inc_call_mode == SYS_SYS_MODE_NO_SRV) ||
      (inc_call_mode == SYS_SYS_MODE_NONE) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Call incoming failed - CM says NO SRV mode");
    return;
  }

  ds3g_cm_client_id = ds3g_get_cm_client_id();
  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);

  /*-------------------------------------------------------------------
    Point to the appropriate handler function table based on the current
    mode and call type.
  --------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  /*--------------------------------------------------------------------
   Ensure that a handler function table is registered, with a non-null
   incoming_call_handler.
  --------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->incoming_page_handler != NULL )
    {
      /*--------------------------------------------------------------
        Invoke the mode-specific handler function.
      --------------------------------------------------------------*/
      page_response = hdlr_tbl_ptr->incoming_page_handler(
                                            cmd_ptr,
                                            call_info_cmd_ptr->subs_id);
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Incoming call handler not registered");
    page_response = DS3G_REJECT_CALL;
  }

  /*-------------------------------------------------------------------------
    The mode specific handler has processed the incoming page, send page
    response to CM.
  -------------------------------------------------------------------------*/
  if ( page_response == DS3G_ANSWER_CALL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode handler accepts incoming page");
    cm_ret_val = cm_mm_call_cmd_incoming_page_resp( ds3gi_cm_call_cmd_cb,
                                                    NULL,
                                                    ds3g_cm_client_id,
                                                    TRUE );

    if (cm_ret_val == FALSE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                      "CM incoming page resp processing returned FALSE");
      return;
    }
  }
  else if( page_response == DS3G_REJECT_CALL )
  {
    /*-----------------------------------------------------------------------
      The mode specific handler rejected the incoming page, send response to
      CM
    -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode handler rejects incoming page");
    cm_ret_val = cm_mm_call_cmd_incoming_page_resp( ds3gi_cm_call_cmd_cb,
                                                    NULL,
                                                    ds3g_cm_client_id,
                                                    FALSE );
    if (cm_ret_val == FALSE)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                      "CM incoming page resp processing returned FALSE");
      return;
    }
  }

  return;
} /* ds3gi_process_cm_call_incoming_page_cmd() */
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_CONNECTED_CMD

DESCRIPTION   This function processes the DS_CM_CALL_CONNECTED_CMD from Call
              Manager.  This command signifies that either an outgoing or an
              incoming call is connected, and end-to-end data transfer may
              proceed.

              This function invokes the mode-specific call_connected_handler,
              and changes the serial mode to Rawdata or Packet, if directed to
              do so by the mode-specific handler. Note that the serial mode
              may have been changed earlier, either before the origination or
              when the call was answered.

DEPENDENCIES  The DS_CM_CALL_CONNECTED_CMD command is valid only in the ORIG
              and WAIT_FOR_CONNECT call states, and is ignored in all other
              call states. This command is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS  Updates the call_state. May also update the serial_state, and
              change the serial mode to Rawdata or Packet.
===========================================================================*/
void ds3gi_process_cm_call_connected_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;    /* Ptr to mode-sp. hdlr table */
  boolean                   hdlr_result;
  cm_end_params_s_type      end_params;
  boolean                   delay_connect;   /* Should connect be delayed? */
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_CONNECTED_CMD");

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "CM_CALL_CONNECTED_CMD bad call id %d",
                    call_info_cmd_ptr->call_id);
    return;
  }

  memset((void*)&end_params,
         0,
         sizeof(cm_end_params_s_type));
  ds3g_cm_client_id = ds3g_get_cm_client_id();

  if (ds3gi_get_call_mode(cmd_ptr) != ds3gi_call_info[call_inst].call_mode)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,"CM_CALL_CONN with bad mode %d wanted %d",
                    ds3gi_get_call_mode(cmd_ptr),
                    ds3gi_call_info[call_inst].call_mode);
    ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;
    end_params.call_id = call_info_cmd_ptr->call_id;
    end_params.info_type = call_info_cmd_ptr->mode_info.info_type;

    cm_ret_val = cm_mm_call_cmd_exten_end(
                                  ds3gi_cm_call_cmd_end_cb,
                                  NULL,
                                  ds3g_cm_client_id,
                                  1,
                                  &end_params
                                  );
    if (cm_ret_val == FALSE)
    {
      DATA_ERR_FATAL("CM call cmd end failed");
    }
    return;
  }

  if ((ds3gi_call_info[call_inst].call_state == DS3G_ORIG_CALL_STATE) ||
      (ds3gi_call_info[call_inst].call_state == DS3G_WAIT_FOR_CONN_CALL_STATE)
     )
  {

    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr =
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return;
    }
#if defined (FEATURE_DATA_WCDMA_CS) && defined (FEATURE_TDSCDMA)
    /*---------------------------------------------------------------------
      If it is UCSD call, update the ucsd network mode.
    ---------------------------------------------------------------------*/
    if( ds3gi_call_info[call_inst].call_type == DS_CALL_TYPE_CKT )
    {
      ds_ucsd_update_network_mode(ds3gi_get_call_mode(cmd_ptr));
    }
#endif /* defined (FEATURE_DATA_WCDMA_CS) && defined (FEATURE_TDSCDMA) */

    /*---------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      call_connected_handler.
    ---------------------------------------------------------------------*/
    if( hdlr_tbl_ptr->call_connected_handler != NULL )
    {
      /*-----------------------------------------------------------------
        Invoke the mode-specific handler function.
        -----------------------------------------------------------------*/
      /*----------------------------------------------------------------
        Intialize end_params before invoking mode specific handler.
        ---------------------------------------------------------------*/

      hdlr_result = hdlr_tbl_ptr->call_connected_handler(
                      call_info_cmd_ptr->call_id,
                      &call_info_cmd_ptr->mode_info,
                      &delay_connect,
                      &end_params,
                      call_info_cmd_ptr->subs_id);

      /*-------------------------------------------------------------------
        Proceed only if the mode-specific handler was successful in
        connecting the call.
      -------------------------------------------------------------------*/

      if( hdlr_result == DS3G_SUCCESS )
      {
        if (delay_connect == TRUE)
        {
          ds3gi_call_info[call_inst].call_state =
            DS3G_WAIT_FOR_LL_CONNECT_STATE;
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Data call connect delayed");
        }
        else /* delay_connect is FALSE */
        {
          /*---------------------------------------------------------------
            Set Call state to CALL_ACTIVE.
            ---------------------------------------------------------------*/
          ds3gi_call_info[call_inst].call_state =DS3G_CALL_ACTIVE_CALL_STATE;
          ds3gi_call_info[call_inst].call_was_connected = TRUE;
          ds3gi_call_info[call_inst].subs_id = call_info_cmd_ptr->subs_id;

          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Data call connected");
        } /* delay_connect == FALSE */
      } /* if hdlr_result == DS3G_SUCCESS */
      else
      {
        /*---------------------------------------------------------------
          The mode-specific handler failed to connect the lower layers,
          so end the call.
          If dedicated bearer estabishment is rejected then fill the
          casue code and nsapi in the end parameters structure
          ---------------------------------------------------------------*/
        if(ds_bearer_cntxt_is_default_call_id(call_info_cmd_ptr->call_id) ==
           FALSE)
        {
            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                            "dedicated bearer establishment not accepted, "
                            "doing regular deactivation ");
           end_params.end_params.gw_ps_end.cause = SM_REGULAR_DEACTIVATION ;
           end_params.end_params.gw_ps_end.nsapi =
             call_info_cmd_ptr->mode_info.info.gw_ps_call.nsapi;

        }
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Handler failed to connect, ending call");

        ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

        cm_ret_val = cm_mm_call_cmd_exten_end(ds3gi_cm_call_cmd_end_cb,
                                        NULL,
                                        ds3g_cm_client_id,
                                        1,
                                        &end_params);

        if (cm_ret_val == FALSE)
        {
          DATA_ERR_FATAL("CM call cmd end failed");
          return;
        }
      }
    } /* if call_connected_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Call connected handler not registered");
    }
  }/* if call_state == ORIG_CALL or WAIT_FOR_CONNECT */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Unexpected CM_CALL_CONNECTED_CMD rx'ed");
  }
  return;
} /* ds3gi_process_cm_call_connected_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_SETUP_CMD

DESCRIPTION   This function processes the DS_CM_CALL_SETUP_CMD from Call
              Manager. It simply calls the mode-specific setup_handler, which
              validates the setup parameters, and either accepts or rejects the
              call.

              If the call is accepted, the mode-specific handler may specify
              new setup parameters which are sent to Call Manager in the setup
              response.
              The setup response may be sent immediately or be delayed at the
              direction of the mode-specific handler.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_SETUP_CMD is valid only in the IDLE or ORIG call
              states, and is ignored in all other call states.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_setup_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type    *call_info_cmd_ptr = NULL;
  ds3gi_call_control_type    *cc_info;
  ds3g_hdlr_func_tbl_type    *hdlr_tbl_ptr;
  boolean                     hdlr_result;
  cm_setup_res_params_s_type  setup_params;
  sys_sys_mode_e_type         inc_call_mode;
  ds_call_type_e_type         inc_call_type;
  uint32                      inc_call_inst=DS3GI_INVALID_CALL_INSTANCE;
  boolean                     delay_rsp = FALSE;
  uint16                      call_inst_itr=DS3GI_INVALID_CALL_INSTANCE;
#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_SETUP_CMD, call id: %d",
                  call_info_cmd_ptr->call_id);

  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);
  if (inc_call_mode == SYS_SYS_MODE_NO_SRV)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "Call setup failed - CM says NO SRV mode");
    return;
  }

  /*-------------------------------------------------------------------------
    Ensure only ETSI mode calls are processed for call setup event.
  -------------------------------------------------------------------------*/
  if(!(DS3G_CALL_IS_ETSI_MODE(inc_call_mode)))
  {
    DATA_ERR_FATAL(" call mode is not in ETSI mode");
    return;
  }

  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);

  if( (cc_info = (ds3gi_call_control_type*)modem_mem_alloc
                  (sizeof(ds3gi_call_control_type),
                  MODEM_MEM_CLIENT_DATA_CRIT)) == NULL )
  {
    DATA_MSG0(MSG_LEGACY_FATAL,"Heap Memory Allocation failed");
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize call control state information
  -------------------------------------------------------------------------*/
  cc_info->accept_inc_call = FALSE;
  cc_info->switch_inc_call = FALSE;
  cc_info->force_cur_call_down = FALSE;
  cc_info->inc_call_id   = call_info_cmd_ptr->call_id;
  cc_info->action = DS3G_REJECT_CALL;
  cc_info->switch_call_inst = DS3GI_INVALID_CALL_INSTANCE;

  cc_info->end_params_inc.call_id = call_info_cmd_ptr->call_id;
  cc_info->end_params_inc.info_type = call_info_cmd_ptr->mode_info.info_type;

  /*-------------------------------------------------------------------------
    Manage the terminating call based on the existing call allocations.
  -------------------------------------------------------------------------*/
  if( DS3G_SUCCESS != ds3gi_manage_terminating_calls( cmd_ptr,
                                                      inc_call_mode,
                                                      inc_call_type,
                                                      cc_info ))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"Cannot manage terminating call");
    modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );

    return;
  }

  if( cc_info->accept_inc_call )
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the incoming
      call mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      setup_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->setup_handler != NULL )
      {
        /*--------------------------------------------------------------------
          At this point, we are either in one of the following states
          a) accepting the call
          b) forced the current call down and ready to accept inc call
          c) switched the call id of curr call & ready to accept inc call
        --------------------------------------------------------------------*/
        if( !cc_info->switch_inc_call )
        {
          /*------------------------------------------------------------------
            ETSI mode CS Data calls utilize the call setup event to negotiate
            and validate the call bearer configuration.  If call setup fails
            in the mode-specific handler, the call is rejected.  However, the
            CSD API client must receive the call end event, which requires
            DSMgr to allocate the call instance during call setup to pass the
            event.  All other call types are allocated in the incoming handler.
          -------------------------------------------------------------------*/
          if( (DS_CALL_TYPE_CKT == inc_call_type) &&
              DS3G_CALL_IS_ETSI_MODE( inc_call_mode ) )
          {
            /*----------------------------------------------------------------
              Need to allocate a call instance for the incoming call
            ----------------------------------------------------------------*/
            inc_call_inst = ds3gi_alloc_call_instance_record(inc_call_type,
                                                             inc_call_mode,
                                                call_info_cmd_ptr->subs_id);
            if( inc_call_inst == DS3GI_INVALID_CALL_INSTANCE )
            {
              /*--------------------------------------------------------------
                Cannot allocate call instance. Ignore incoming call
              --------------------------------------------------------------*/
              DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                              "Cannot allocate call instance:type:%d",
                              inc_call_type);
              cc_info->action = DS3G_REJECT_CALL;
            }
          }
        } /* if(!cc_info->switch_inc_call) */
        else
        {
          /*------------------------------------------------------------------
            We have switched calls. Just note the call inst. of the
            switched call.
          ------------------------------------------------------------------*/
          inc_call_inst = cc_info->switch_call_inst;
        } /* if(!cc_info->switch_inc_call) */


        if( DS3G_REJECT_CALL != cc_info->action )
        {
          /*----------------------------------------------------------------
            Intialize setup_params before invoking mode specific handler.
          ---------------------------------------------------------------*/
          memset((void*)&setup_params,
                 0,
                 sizeof(cm_setup_res_params_s_type));

          /*-----------------------------------------------------------------
            Invoke the mode-specific handler function.
          -----------------------------------------------------------------*/
          hdlr_result = hdlr_tbl_ptr->setup_handler
            (
              call_info_cmd_ptr->call_id,
              &call_info_cmd_ptr->mode_info,
              &setup_params,
              call_info_cmd_ptr->call_mode,
              &delay_rsp,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
              #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
              call_info_cmd_ptr->subs_id
            );
          if( hdlr_result == DS3G_FAILURE )
          {
            DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                            "Mode-Sp. Hdlr rejected the setup");
          }
        }
      } /* if setup_handler != NULL */
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Setup call handler not registered");
      }
    } /* if hdlr_tbl_ptr != NULL */

  } /* if( cc_info->accept_inc_call ) */

  /*-------------------------------------------------------------------------
    At this point, the incoming call has been processed by the mode-
    specific handler.  Do next action based on results of mode-specific
    handler return value.
  -------------------------------------------------------------------------*/

  if ( cc_info->action != DS3G_REJECT_CALL )
  {
    /*-----------------------------------------------------------------------
      Update call instance state only if it is valid and it was not
      cleared by the mode-specific handler.
    -----------------------------------------------------------------------*/
    if( DS3GI_INVALID_CALL_INSTANCE != inc_call_inst )
    {
      for(call_inst_itr=0;call_inst_itr<DS3GI_MAX_NUM_CALLS;call_inst_itr++)
      {
        if(ds3gi_call_info[call_inst_itr].call_id == call_info_cmd_ptr->call_id)
        {
          DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                          "call id already present in the table wrong "
                          "call_id:%d recieved from CM in call_setup",
                          ds3gi_call_info[call_inst_itr].call_id);
          ASSERT(0);
        }
      }
      ds3gi_call_info[inc_call_inst].call_id = call_info_cmd_ptr->call_id;
      ds3gi_call_info[inc_call_inst].call_type = inc_call_type;
      ds3gi_call_info[inc_call_inst].call_state = DS3G_WAIT_FOR_CONN_CALL_STATE;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
      #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
      {
        /*---------------------------------------------------------------------
          Keep a note of the call type and the mode for the incoming call
          this will be used to find the mode handler when the call has ended
          to retrieve statistics.
          HACK : REMOVE THIS WHEN ACTUAL CM support for multipe calls are added
        ----------------------------------------------------------------------*/
        ds3g_set_last_call_mode( ds3gi_call_info[inc_call_inst].call_mode);
        ds3g_set_last_call_type( ds3gi_call_info[inc_call_inst].call_type);
      }
    }

    /*-------------------------------------------------------------------
      Mode-specific handler control whether setup response message is
      sent immediately or if it is delayed.  If delayed, setup message
      must be sent seperately by calling ds3g_msh_confirm_call().
    -------------------------------------------------------------------*/
    if( FALSE == delay_rsp )
    {
      (void)ds3g_msh_confirm_call( call_info_cmd_ptr->call_id,
                                   &setup_params );
    }
#ifdef FEATURE_MULTIMEDIA_FALLBACK
    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "DSMgr rejected the setup");

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
    /*-------------------------------------------------------------------
      Call instance could not be allocated so reject the call.
      See 3GPP TS24.008 section 5.2.2.3.1.
    -------------------------------------------------------------------*/
    setup_params.accept = FALSE;
    setup_params.bc_repeat_indicator.present = FALSE;
    setup_params.bearer_capability_1.present = FALSE;
    setup_params.bearer_capability_2.present = FALSE;

    setup_params.cc_cause.present = TRUE;
    setup_params.cc_cause.coding_standard = STD_CODESTANDARD;
    setup_params.cc_cause.location = STD_LOCATION;
    setup_params.cc_cause.recommendation_present = FALSE;
    setup_params.cc_cause.diagnostic_length = 0;
    setup_params.cc_cause.cause_value = CALL_REJECTED;

    (void)ds3g_msh_confirm_call( call_info_cmd_ptr->call_id,
                                 &setup_params );
#endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
  }

  modem_mem_free( cc_info, MODEM_MEM_CLIENT_DATA_CRIT );

  return;
} /* ds3gi_process_cm_call_setup_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_CONF_CMD

DESCRIPTION   This function processes the DS_CM_CALL_CONF_CMD received from
              Call Manager. It simply calls the mode-specific call_conf_handler

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_CONF_CMD is valid only in the ORIG call state,
              and is ignored in all other call states. Its is also ignored if
              the call id is not recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_conf_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  boolean                   hdlr_result;
  cm_end_params_s_type      end_params;
  uint32                    call_inst;
  boolean                   cm_ret_val = FALSE;
  sys_sys_mode_e_type       call_mode;
  cm_call_id_type           call_id;
  ds_call_type_e_type       call_type;
  ds3gi_call_state_e_type   call_state;
#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
  cm_client_id_type         ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_CONF_CMD");

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
#ifdef FEATURE_MULTIMEDIA_FALLBACK
    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "CM_CALL_CONF_CMD has unknown call id");
      return;
    }
  }
  else
  {
    call_mode  = ds3gi_call_info[call_inst].call_mode;
    call_type  = ds3gi_call_info[call_inst].call_type;
    call_id    = ds3gi_call_info[call_inst].call_id;
    call_state = ds3gi_call_info[call_inst].call_state;
  }

  /*-------------------------------------------------------------------------
    Call confirm is valid only in the Orig call state.
  -------------------------------------------------------------------------*/
  if (call_state == DS3G_ORIG_CALL_STATE)
  {
    if ( (call_mode == SYS_SYS_MODE_NONE) ||
         ( call_mode >= SYS_SYS_MODE_MAX ))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid System Mode %d", call_mode);
      return;
    }

    if ((call_type >= DS_NUM_CALL_TYPES) ||
        (call_type == DS_CALL_TYPE_UNSPECIFIED))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid Call Type %d", call_type);
      return;
    }

    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[call_mode][call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      call_conf_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->call_conf_handler != NULL )
      {
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_result = hdlr_tbl_ptr->call_conf_handler
                                     (
                                       call_id,
                                       &call_info_cmd_ptr->mode_info,
                                       &end_params,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
                                       #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                                       call_info_cmd_ptr->subs_id
                                     );

        if( hdlr_result == DS3G_FAILURE )
        {
          /*-----------------------------------------------------------------
            The parameters proposed by the network in the Call Conf were
            not acceptable to the mode-specific handler, so end the call.
          -----------------------------------------------------------------*/
          DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                          "Handler rejected CALL_CONF, ending call");
          if( call_inst < DS3GI_MAX_NUM_CALLS )
          {
            ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;
          }

          ds3g_cm_client_id = ds3g_get_cm_client_id();

          cm_ret_val = cm_mm_call_cmd_exten_end(ds3gi_cm_call_cmd_end_cb,
                                          NULL,
                                          ds3g_cm_client_id,
                                          1,
                                          &end_params
                                         );
          if (cm_ret_val == FALSE)
          {
            DATA_ERR_FATAL("CM call cmd end failed");
            return;
          }
        }

#ifdef FEATURE_MULTIMEDIA_FALLBACK
        #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

      } /* if call_conf_handler != NULL */
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "Call confirm handler not registered");
      }
    } /* if hdlr_tbl_ptr != NULL */
  } /* if call_state == ORIG_CALL */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "CM_CALL_CONF_CMD rx'ed in wrong call state or mode");
  }

  return;
} /* ds3gi_process_cm_call_conf_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_NW_MODIFY_REQ_CMD

DESCRIPTION   This function processes the DS_CM_CALL_NW_MODIFY_REQ received
              from Call Manager. It simply calls the mode-specific
              call_nw_modify_handler.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_nw_modify_req_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type      *call_info_cmd_ptr = NULL;
  cm_pdp_act_res_params_s_type  call_modify_info;
  ds3g_hdlr_func_tbl_type      *hdlr_tbl_ptr;
  uint32                        call_inst = DS3GI_DEFAULT_CALL_INSTANCE;
  boolean                       cm_ret_val = FALSE;
  sys_sys_mode_e_type           inc_call_mode;
  ds3g_incom_e_type             action = DS3G_ANSWER_CALL;
  cm_client_id_type             ds3g_cm_client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "Rx'ed CM_CALL_NW_MODIFY_REQ_CMD, call id: %d",
                  call_info_cmd_ptr->call_id);

  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);

  if( (inc_call_mode == SYS_SYS_MODE_NO_SRV) ||
      (inc_call_mode == SYS_SYS_MODE_NONE) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "NW MODIFY Req failed - CM says NO SRV mode");
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize call modify state information
  -------------------------------------------------------------------------*/
  call_modify_info.accept = FALSE;

  if( CM_CALL_TYPE_PS_DATA == call_info_cmd_ptr->call_type )
  {
      call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
      if ( call_inst >= DS3GI_MAX_NUM_CALLS )
      {
          DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid call instance %d",
                          call_inst);
          return;
      }
  /*-------------------------------------------------------------------------
    context modification req is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if ((ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE) &&
      (ds3gi_call_info[call_inst].call_mode  == ds3gi_get_call_mode(cmd_ptr)))
    {
      /*-------------------------------------------------------------------
        Point to the appropriate handler function table based on the current
        mode and call type.
      --------------------------------------------------------------------*/
      hdlr_tbl_ptr = 
         ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

      if (hdlr_tbl_ptr == NULL)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
        return;
      }

      /*--------------------------------------------------------------------
        Ensure that a handler function table is registered, with a non-null
        incoming_call_handler.
      --------------------------------------------------------------------*/

        if( hdlr_tbl_ptr->nw_modify_req_handler != NULL )
        {
          /*--------------------------------------------------------------
            Invoke the mode-specific handler function.
          --------------------------------------------------------------*/
            hdlr_tbl_ptr->nw_modify_req_handler(
                                        ds3gi_call_info[call_inst].call_id,
                                        cmd_ptr,
                                        &call_modify_info,
                                        call_info_cmd_ptr->subs_id
                                       );
        } /* if( hdlr_tbl_ptr->nw_modify_req_handler != NULL ) */
        else
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "nw modify req hdlr not registered");
          action = DS3G_REJECT_CALL;
        }
    }
    else
    {
      action = DS3G_REJECT_CALL;
    }
  }

  /*-------------------------------------------------------------------------
    At this point, the incoming call has been processed by the mode-
    specific handler.  Do next action based on results of mode-specific
    handler return value.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    If the mode specific handler returns ignore, then completely ignore the
    incoming call. Do not save any information regarding the call either.
  -------------------------------------------------------------------------*/

  if ( action != DS3G_REJECT_CALL )
  {
    ds3g_cm_client_id = ds3g_get_cm_client_id();
    /*-----------------------------------------------------------------------
      Now send a command to Call Manager to answer the call.
    -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode-Sp. Hdlr answers call");

    cm_ret_val = cm_mm_call_cmd_pdp_nw_modify_res (
                                        ds3gi_cm_call_cmd_cb,
                                        NULL,
                                        ds3g_cm_client_id,
                                        ds3gi_call_info[call_inst].call_id,
                                        &call_modify_info
                                      );
    if (cm_ret_val == FALSE)
    {
      DATA_ERR_FATAL("CM call cmd modify resp failed");
      return;
    }
  }

  return;
}/* ds3gi_process_cm_call_nw_modify_req_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_PROGRESS_CMD

DESCRIPTION   This function processes the DS_CM_CALL_PROGRESS_CMD from Call
              Manager. It simply calls the mode-specific setup_handler which
              may pass notification to higher-layer clients.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_PROGRESS_CMD is ignored if the call id is not
              recognized and call is not dual-mode.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_progress_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type    *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type    *hdlr_tbl_ptr;
  boolean                     hdlr_result;
  uint32                      call_inst;
  sys_sys_mode_e_type         call_mode;
  cm_call_id_type             call_id;
  ds_call_type_e_type         call_type;
  ds3gi_call_state_e_type     call_state;
#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_PROGRESS_CMD, call id: %d",
                  call_info_cmd_ptr->call_id);

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);

  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
#ifdef FEATURE_MULTIMEDIA_FALLBACK
    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "CM_CALL_PROGRESS_CMD has unknown call id");
      return;
    }
  }
  else
  {
    call_mode  = ds3gi_call_info[call_inst].call_mode;
    call_type  = ds3gi_call_info[call_inst].call_type;
    call_id    = ds3gi_call_info[call_inst].call_id;
    call_state = ds3gi_call_info[call_inst].call_state;
  }

  /*-------------------------------------------------------------------------
    Call progress is valid only in the Orig or Active call state.
  -------------------------------------------------------------------------*/
  if( (call_state == DS3G_ORIG_CALL_STATE) ||
      (call_state == DS3G_CALL_ACTIVE_CALL_STATE) )
  {
    if ( (call_mode == SYS_SYS_MODE_NONE) ||
         (call_mode >= SYS_SYS_MODE_MAX ))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid System Mode %d", call_mode);
      return;
    }

    if ((call_type >= DS_NUM_CALL_TYPES) ||
        (call_type == DS_CALL_TYPE_UNSPECIFIED))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid Call Type %d", call_type);
      return;
    }

    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the incoming
      call mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[call_mode][call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      setup_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->call_progress_handler != NULL )
      {
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_result = hdlr_tbl_ptr->call_progress_handler
                                     (
                                       call_id,
                                       &call_info_cmd_ptr->mode_info,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
                                       #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                                       call_info_cmd_ptr->subs_id
                                     );

        if( hdlr_result == DS3G_FAILURE )
        {
          DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Mode-Sp. Hdlr reported problem");
        }

#ifdef FEATURE_MULTIMEDIA_FALLBACK
        #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

      } /* if progress_handler != NULL */
      else
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                        "Progress call handler not registered");
      }
    } /* if hdlr_tbl_ptr != NULL */
  } /* if call_state == ORIG_CALL */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "CM_CALL_PROGRESS_CMD rx'ed in wrong call state or mode");
  }

  return;
} /* ds3gi_process_cm_call_progress_cmd() */

#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_GET_PDN_CONN_REJ_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the
              indication CM_CALL_EVENT_PDN_CONN_REJ_IND from CM. This function
              invokes the pdn conn rej handler in the LTE mode handler.
              Note that this indication and function have no meaning in a
              mode other than LTE

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_pdn_conn_rej_ind_cmd
(
  ds_cmd_type *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  sys_sys_mode_e_type       inc_call_mode;
  ds_call_type_e_type       inc_call_type;  /* incoming call type CS/PS   */
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;  /* Ptr to mode-sp. hdlr table */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_PDN_CONN_REJ_IND");

  /*-------------------------------------------------------------------------
    Ensure that the call was received in the correct mode
  -------------------------------------------------------------------------*/
  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);

  if(inc_call_mode != SYS_SYS_MODE_LTE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Handling PDN Conn Rej Ind in wrong mode %d",
                    inc_call_mode);
    inc_call_mode = SYS_SYS_MODE_LTE;
  }

  /*-------------------------------------------------------------------------
    Get the table of handler function corresponding to the mode and type of
    call. Here the mode is LTE and the type of call is Packet switched
  -------------------------------------------------------------------------*/
  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  /*--------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_pdn_conn_ind_hdlr. Then invoke the handler to dispatch the app
    mode specific function.
  --------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->pdn_conn_rej_ind_hdlr != NULL )
    {
      hdlr_tbl_ptr->pdn_conn_rej_ind_hdlr(call_info_cmd_ptr,
                                          call_info_cmd_ptr->subs_id);
    }
  }

  return;
} /* ds3gi_process_cm_call_pdn_conn_rej_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_GET_PDN_CONN_FAIL_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the
              indication CM_CALL_EVENT_PDN_CONN_FAIL_IND from CM. This
              function invokes the pdn conn FAIL handler in the LTE mode
              handler. Note that this indication and function have no meaning
              in a mode other than LTE.

              This indication is given whenever a PDN connection request was
              aborted. This is the way we clean up the resources that have
              beeen allocated for that call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_pdn_conn_fail_ind_cmd
(
  ds_cmd_type *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  sys_sys_mode_e_type       inc_call_mode;
  ds_call_type_e_type       inc_call_type;  /* incoming call type CS/PS   */
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;  /* Ptr to mode-sp. hdlr table */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_PDN_CONN_FAIL_IND");

  /*-------------------------------------------------------------------------
    Ensure that the call was received in the correct mode
  -------------------------------------------------------------------------*/
  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);

  if(inc_call_mode != SYS_SYS_MODE_LTE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Received PDN Conn Rej Ind in wrong mode %d",
                    inc_call_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the table of handler function corresponding to the mode and type of
    call. Here the mode is LTE and the type of call is Packet switched
  -------------------------------------------------------------------------*/
  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  /*--------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_pdn_conn_ind_hdlr. Then invoke the handler to dispatch the app
    mode specific function.
  --------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->pdn_conn_fail_ind_hdlr != NULL )
    {
      hdlr_tbl_ptr->pdn_conn_fail_ind_hdlr(call_info_cmd_ptr,
                                           call_info_cmd_ptr->subs_id);
    }
  }

  return;
} /* ds3gi_process_cm_call_pdn_conn_fail_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_GET_PDN_CONN_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the
              indication CM_CALL_EVENT_GET_PDN_CONN_IND from CM. This function
              invokes the attach handler in the LTE mode handler. Note that
              this indication and function have no meaning in a mode other
              than LTE

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_get_pdn_conn_ind_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  sys_sys_mode_e_type       inc_call_mode;
  ds_call_type_e_type       inc_call_type;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
  ds3g_eps_attach_sm_info_type  attach_info_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "Attach Request - Rx'ed CM_CALL_EVENT_GET_PDN_CONN_IND");

  /*-------------------------------------------------------------------------
    Ensure that the call was received in the correct mode
  -------------------------------------------------------------------------*/
  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);

  if(inc_call_mode != SYS_SYS_MODE_LTE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Handling Attach request Ind in wrong mode, %d",
                    inc_call_mode);
    inc_call_mode = SYS_SYS_MODE_LTE;
  }

  attach_info_s.seq_num             = call_info_cmd_ptr->seq_num;
  subs_id = call_info_cmd_ptr->subs_id;
  attach_info_s.responding_to_page  = call_info_cmd_ptr->responding_to_page;

  if(attach_info_s.seq_num == 0)
  {
    DATA_ERR_FATAL("Attach request sequence number is 0,ASSERT !");
  }
  /*-------------------------------------------------------------------------
    Get the table of handler function corresponding to the mode and type of
    call. Here the mode is LTE and the type of call is Packet switched
  -------------------------------------------------------------------------*/
  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  /*--------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_pdn_conn_ind_hdlr. Then invoke the handler to dispatch the app
    mode specific function.
  --------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->get_pdn_conn_ind_hdlr != NULL )
    {
      hdlr_tbl_ptr->get_pdn_conn_ind_hdlr(&attach_info_s,subs_id);
    }
  }

} /* ds3gi_process_cm_call_get_pdn_conn_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_ACT_BEARER_IND_CMD

DESCRIPTION   This function processes the DS_CM_CALL_EVENT_ACT_BEARER_IND
              from Call Manager. This command is notification of bearer setup
              by network.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_act_bearer_ind_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  boolean                   cm_ret_val = FALSE;
  boolean                   action = DS3G_FAILURE;
  ds_call_type_e_type       inc_call_type;
  uint32                    inc_call_inst=DS3GI_INVALID_CALL_INSTANCE;
  sys_sys_mode_e_type       inc_call_mode;
  cm_act_bearer_rsp_s_type *call_activate_response = NULL;
  cm_lte_call_info_s_type  *lte_info_ptr = NULL;
  cm_client_id_type          ds3g_cm_client_id;
  uint16                    call_inst_itr=DS3GI_INVALID_CALL_INSTANCE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "Rx'ed CM_CALL_EVENT_ACT_BEARER_IND, call id: %d",
                  call_info_cmd_ptr->call_id);

  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);
  if( (inc_call_mode == SYS_SYS_MODE_NO_SRV) ||
      (inc_call_mode == SYS_SYS_MODE_NONE) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Call incoming failed - CM says NO SRV mode");
    return;
  }

  if(inc_call_mode != SYS_SYS_MODE_LTE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Received ActivateBearerIndication in wrong mode %d",
                    inc_call_mode);
    return;
  }

  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  if (hdlr_tbl_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_tbl_ptr == NULL");
    return;
  }

  if( hdlr_tbl_ptr->bearer_activate_handler == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "activate bearer ind handler not registered");
    return;
  }

   /*---------------------------------------------------------------
    Need to allocate a call instance for the incoming call
  ---------------------------------------------------------------*/
  inc_call_inst = ds3gi_alloc_call_instance_record(inc_call_type,
                                                   inc_call_mode,
                                                   call_info_cmd_ptr->subs_id);
  if( inc_call_inst >= DS3GI_MAX_NUM_CALLS )
  {
    DATA_3GMGR_MSG3(MSG_LEGACY_ERROR,
                    "Bad call_id:%d or call inst alloc failed inst:%d type:%d "
                    "Returning without processing",
                    call_info_cmd_ptr->call_id,
                    inc_call_inst,
                    inc_call_type);
    return;
  }

  action = DS3G_FAILURE;

  call_activate_response =
    (cm_act_bearer_rsp_s_type *)modem_mem_alloc(sizeof(cm_act_bearer_rsp_s_type),
                                                MODEM_MEM_CLIENT_DATA);
  if (call_activate_response == NULL)
  {
    DATA_ERR_FATAL("Memory Allocation Failed.");
    return;
  }

  memset(call_activate_response, 0, sizeof(cm_act_bearer_rsp_s_type));

  for(call_inst_itr=0;call_inst_itr<DS3GI_MAX_NUM_CALLS;call_inst_itr++)
  {
    if(ds3gi_call_info[call_inst_itr].call_id == call_info_cmd_ptr->call_id)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "call id already present in the table wrong call_id:%d "
                      "recieved from CM in call_setup",
                      ds3gi_call_info[call_inst_itr].call_id);
      ASSERT(0);
    }
  }


  ds3gi_call_info[inc_call_inst].call_id = call_info_cmd_ptr->call_id;
  ds3gi_call_info[inc_call_inst].call_type = inc_call_type;
  action = hdlr_tbl_ptr->bearer_activate_handler(
        ds3gi_call_info[inc_call_inst].call_id,
        &(call_info_cmd_ptr->mode_info),
        call_activate_response,
        call_info_cmd_ptr->subs_id);

  ds3g_cm_client_id = ds3g_get_cm_client_id();

  call_activate_response->asubs_id = call_info_cmd_ptr->subs_id;
  /* using cmd_ptr->cmd.call_info.subs_id which might be filled by CM, have to confirm that */

  /*-------------------------------------------------------------------------
    At this point, the activate bearer ind has been processed by the mode-
    specific handler.  Do next action based on results of mode-specific
    handler return value.
  -------------------------------------------------------------------------*/
  if(action == DS3G_SUCCESS)
  {
    if( inc_call_inst < DS3GI_MAX_NUM_CALLS )
    {
      /*---------------------------------------------------------------
                Set Call state to CALL_ACTIVE.
      ---------------------------------------------------------------*/
      ds3gi_call_info[inc_call_inst].call_state =
                                         DS3G_CALL_ACTIVE_CALL_STATE;
      ds3gi_call_info[inc_call_inst].call_was_connected = TRUE;
      call_activate_response->is_accept = TRUE;

      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Data call connected");
      cm_ret_val = cm_mm_call_cmd_act_bearer_rsp(
                                      ds3gi_cm_call_cmd_cb,
                                      NULL,
                                      ds3g_cm_client_id,
                                      ds3gi_call_info[inc_call_inst].call_id,
                                      call_activate_response
                                    );
    }
    if (cm_ret_val == FALSE)
    {
      modem_mem_free((void*)call_activate_response, MODEM_MEM_CLIENT_DATA);
      DATA_ERR_FATAL("CM call cmd act bearer rsp failed");
      return;
    }
  }

  if(action == DS3G_FAILURE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode Hdlr rejected the call");
    call_activate_response->is_accept = FALSE;
    lte_info_ptr =(cm_lte_call_info_s_type *)
                   &(call_info_cmd_ptr->mode_info.info.lte_call);
    if(lte_info_ptr->esm_cause.valid)
    {
      call_activate_response->esm_cause.valid = TRUE;
      call_activate_response->esm_cause.esm_cause =
                      lte_info_ptr->esm_cause.esm_cause;
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"ESM Cause code is not valid");
    }
    if( inc_call_inst < DS3GI_MAX_NUM_CALLS )
    {
      cm_ret_val = cm_mm_call_cmd_act_bearer_rsp(
                                      ds3gi_cm_call_cmd_cb,
                                      NULL,
                                      ds3g_cm_client_id,
                                      ds3gi_call_info[inc_call_inst].call_id,
                                      call_activate_response
                                    );
    }
    if (cm_ret_val == FALSE)
    {
      modem_mem_free((void*)call_activate_response, MODEM_MEM_CLIENT_DATA);
      DATA_ERR_FATAL("CM call cmd act bearer rsp failed");
      return;
    }
    /*-----------------------------------------------------------------------
      If we have allocated a call instance, mark the call state and free
      the call record. This was not done earlier. The call instance should
      be freed to ensure that the CALL_END notification from CM gets ignored
    -----------------------------------------------------------------------*/
    if( inc_call_inst < DS3GI_MAX_NUM_CALLS )
    {
      ds3gi_call_info[inc_call_inst].call_state = DS3G_END_CALL_STATE;
    }
  }

  modem_mem_free((void*)call_activate_response, MODEM_MEM_CLIENT_DATA);

  return;
}/* ds3gi_process_cm_call_act_bearer_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_BEARER_ALLOC_REJ_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the
              indication CM_CALL_EVENT_RES_ALLOC_REJ from CM. This indication
              and function are applicable only in the LTE mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_bearer_alloc_rej_ind_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  sys_sys_mode_e_type       inc_call_mode;
  ds_call_type_e_type       inc_call_type;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_RES_ALLOC_REJ_IND");

  /*-------------------------------------------------------------------------
    Ensure that the call was received in the correct mode
  -------------------------------------------------------------------------*/
  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);

  if(inc_call_mode != SYS_SYS_MODE_LTE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Received RES ALLOC Rej Ind in wrong mode:%d",
                    inc_call_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the table of handler function corresponding to the mode and type of
    call. Here the mode is LTE and the type of call is Packet switched
  -------------------------------------------------------------------------*/
  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  /*--------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_pdn_conn_ind_hdlr. Then invoke the handler to dispatch the app
    mode specific function.
  --------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if(hdlr_tbl_ptr->bearer_alloc_rej_ind_hdlr != NULL )
    {
      hdlr_tbl_ptr->bearer_alloc_rej_ind_hdlr(&(call_info_cmd_ptr->mode_info),
                                              call_info_cmd_ptr->subs_id);
    }
  }

  return;
} /* ds3gi_process_cm_call_bearer_alloc_rej_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_BEARER_ALLOC_FAIL_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the
              indication CM_CALL_EVENT_RES_ALLOC_FAIL from CM. This indication
              and function are applicable only in the LTE mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_bearer_alloc_fail_ind_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  sys_sys_mode_e_type       inc_call_mode;
  ds_call_type_e_type       inc_call_type;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_RES_ALLOC_FAIL_IND");

  /*-------------------------------------------------------------------------
    Ensure that the call was received in the correct mode
  -------------------------------------------------------------------------*/
  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);

  if(inc_call_mode != SYS_SYS_MODE_LTE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Received RES ALLOC Fail Ind in wrong mode%d",
                    inc_call_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the table of handler function corresponding to the mode and type of
    call. Here the mode is LTE and the type of call is Packet switched
  -------------------------------------------------------------------------*/
  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  /*--------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_pdn_conn_ind_hdlr. Then invoke the handler to dispatch the app
    mode specific function.
  --------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if(hdlr_tbl_ptr->bearer_alloc_fail_ind_hdlr != NULL )
    {
      hdlr_tbl_ptr->bearer_alloc_fail_ind_hdlr(&(call_info_cmd_ptr->mode_info),
                                               call_info_cmd_ptr->subs_id);
    }
  }

  return;
} /* ds3gi_process_cm_call_bearer_alloc_fail_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_BEARER_MOD_REJ_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the
              indication CM_CALL_EVENT_BEARER_MOD_REJ from CM. This indication
              and function are applicable only in the LTE mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_bearer_mod_rej_ind_cmd
(
  ds_cmd_type *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  sys_sys_mode_e_type       inc_call_mode;
  ds_call_type_e_type       inc_call_type;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_BEARER_MOD_REJ_IND");

  /*-------------------------------------------------------------------------
    Ensure that the call was received in the correct mode
  -------------------------------------------------------------------------*/
  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);

  if(inc_call_mode != SYS_SYS_MODE_LTE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Received Bearer Mod Rej Ind in wrong mode%d",
                    inc_call_mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the table of handler function corresponding to the mode and type of
    call. Here the mode is LTE and the type of call is Packet switched
  -------------------------------------------------------------------------*/
  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  /*--------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_pdn_conn_ind_hdlr. Then invoke the handler to dispatch the app
    mode specific function.
  --------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if(hdlr_tbl_ptr->bearer_mod_rej_ind_hdlr != NULL )
    {
      hdlr_tbl_ptr->bearer_mod_rej_ind_hdlr(&(call_info_cmd_ptr->mode_info),
                                            call_info_cmd_ptr->subs_id);
    }
  }

  return;
} /* ds3gi_process_cm_call_bearer_mod_rej_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_BEARER_MOD_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the
              indication CM_CALL_EVENT_BEARER_MOD_IND from CM. This indication
              and function are applicable only in the LTE mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_bearer_mod_ind_cmd
(
  ds_cmd_type *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  sys_sys_mode_e_type       inc_call_mode;
  ds_call_type_e_type       inc_call_type;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  boolean                   action = DS3G_FAILURE;
  boolean                   cm_ret_val = FALSE;
  cm_mod_bearer_rsp_s_type *call_mod_resp = NULL;
  cm_lte_call_info_s_type  *lte_info_ptr;
  cm_client_id_type         ds3g_cm_client_id;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_BEARER_MOD_IND");

  /*-------------------------------------------------------------------------
    Ensure that the call was received in the correct mode
  -------------------------------------------------------------------------*/
  inc_call_mode = ds3gi_get_call_mode (cmd_ptr);
  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);

  if(inc_call_mode != SYS_SYS_MODE_LTE)
  {
    return;
  }

  if(call_inst == DS3GI_INVALID_CALL_INSTANCE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Invalid call instance");
    return;
  }

  /*-------------------------------------------------------------------------
    Get the table of handler function corresponding to the mode and type of
    call. Here the mode is LTE and the type of call is Packet switched
  -------------------------------------------------------------------------*/
  inc_call_type = DS3G_GET_DS_CALL_TYPE(call_info_cmd_ptr->call_type);
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[inc_call_mode][inc_call_type];

  /*--------------------------------------------------------------------
    Ensure that a handler function table is registered, with a non-null
    get_pdn_conn_ind_hdlr. Then invoke the handler to dispatch the app
    mode specific function.
  --------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if(hdlr_tbl_ptr->bearer_mod_ind_hdlr != NULL )
    {
      action = hdlr_tbl_ptr->bearer_mod_ind_hdlr(
                                      &(call_info_cmd_ptr->mode_info),
                                      call_info_cmd_ptr->call_id,
                                      call_info_cmd_ptr->subs_id);
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Call back not registered for bearer_mod_ind_hdlr");
      return;
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "handler table not registered for bearer_mod_ind_hdlr");
    return;
  }/* hdlr_tbl_ptr == NULL */
  /*--------------------------------------------------------------------
    configure the response to CM/ESM.
  --------------------------------------------------------------------*/
  call_mod_resp = modem_mem_alloc(sizeof(cm_mod_bearer_rsp_s_type),
                                               MODEM_MEM_CLIENT_DATA_CRIT);

  if(call_mod_resp == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "memory allocation failed for modify resp");
    return;
  }

  memset(call_mod_resp, 0 , sizeof(cm_mod_bearer_rsp_s_type));
  lte_info_ptr=(cm_lte_call_info_s_type *)
           &(call_info_cmd_ptr->mode_info.info.lte_call);
  call_mod_resp->eps_bearer_id = lte_info_ptr->eps_bearer_id;
  call_mod_resp->protocol_config_options.valid = FALSE;
  call_mod_resp->asubs_id = call_info_cmd_ptr->subs_id;
  ds3g_cm_client_id = ds3g_get_cm_client_id();

  /*--------------------------------------------------------------------
    If the request is being accepted.
  --------------------------------------------------------------------*/
  if(action == DS3G_SUCCESS)
  {
    call_mod_resp->esm_cause.valid = FALSE;
    call_mod_resp->esm_local_cause.valid = FALSE;
    call_mod_resp->is_accept = TRUE;
    cm_ret_val = cm_mm_call_cmd_mod_bearer_rsp(
                                ds3gi_cm_call_cmd_cb,
                                NULL,
                                ds3g_cm_client_id,
                                call_info_cmd_ptr->call_id,
                                call_mod_resp
                              );

    if (cm_ret_val == FALSE)
    {
      DATA_ERR_FATAL("CM call cmd bearer mod accept rsp failed");
      modem_mem_free((void *)call_mod_resp, MODEM_MEM_CLIENT_DATA_CRIT );
      return;
    }
  }/* action == DS3G_SUCCESS */

  if(action == DS3G_FAILURE)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Mode Hdlr rejected the call");
    call_mod_resp->esm_local_cause.valid = FALSE;
    call_mod_resp->is_accept = FALSE;
    if(lte_info_ptr->esm_cause.valid)
    {
      call_mod_resp->esm_cause.valid = TRUE;
      call_mod_resp->esm_cause.esm_cause =
                      lte_info_ptr->esm_cause.esm_cause;
    }
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,"ESM Cause code is not valid");
    }

    cm_ret_val = cm_mm_call_cmd_mod_bearer_rsp(
                                ds3gi_cm_call_cmd_cb,
                                NULL,
                                ds3g_cm_client_id,
                                call_info_cmd_ptr->call_id,
                                call_mod_resp
                              );

    if (cm_ret_val == FALSE)
    {
      DATA_ERR_FATAL("CM call cmd bearer mod reject rsp failed");
      modem_mem_free((void *)call_mod_resp, MODEM_MEM_CLIENT_DATA_CRIT );
      return;
    }
  }/* action == DS3G_FAILURE */
  modem_mem_free((void *)call_mod_resp, MODEM_MEM_CLIENT_DATA_CRIT );
  return;
} /* ds3gi_process_cm_call_bearer_mod_ind_cmd() */
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_SS_3GPP_CNTXT_TRANSFER_CMD

DESCRIPTION   This function processes the DS_CM_SS_3GPP_CNTXT_TRANSFER_CMD
              from Call Manager.  This command signifies that L<->GW inter
              RAT transition is in progress.

              This function invokes mode-specific cntxt_transfer_ind_handler.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_ss_3gpp_cntxt_transfer_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_hdlr_func_tbl_type     *hdlr_tbl_ptr;
  sys_sys_mode_e_type          target_rat;
  uint8                        call_type;
  sys_modem_as_id_e_type       subs_id = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type       dds_id = SYS_MODEM_AS_ID_NONE;
  ds3g_ss_irat_cntxt_tx_type  *ss_irat_cntxt_tx_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    ASSERT(0);
    return;
  }
  ss_irat_cntxt_tx_ptr
    = (ds3g_ss_irat_cntxt_tx_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_SS_3GPP_CNTXT_TRANSFER_CMD");

  /*-------------------------------------------------------------------------
    Context can be transferred only to W/G/L RATs
  -------------------------------------------------------------------------*/
  target_rat = ss_irat_cntxt_tx_ptr->irat_ctxt_target_rat;
  subs_id    = ss_irat_cntxt_tx_ptr->irat_asubs_id;

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    /*-----------------------------------------------------------------------
      Hack !!! for NAS issue.
      To be removed when NAS passes the correct subs_id
    ----------------------------------------------------------------------*/
    if (ds3gsubsmgr_get_device_mode() == SYS_MODEM_DEVICE_MODE_SINGLE_SIM)
    {
      dds_id = ds3gsubsmgr_get_def_data_subs_id();
      subs_id = dds_id;
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "SINGLE SIM SUBS OVERRIDE: %d with %d", subs_id, dds_id);
    }
    else
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Rx'ed irat context target rat:%d on wrong subs %d",
                      target_rat, subs_id);
      return;
    }
  }

  if(!DS_3GPP_IS_3GPP_CURRENT_MODE(target_rat))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                    "Rx'ed wrong irat context target rat:%d on subs %d",
                    target_rat, subs_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Loop through all the registered handler function tables for the
    current mode.
  -------------------------------------------------------------------------*/
  for(call_type = (uint8)DS_CALL_TYPE_CKT;
      call_type < (uint8)DS_NUM_CALL_TYPES;
      call_type++)
  {
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[target_rat][call_type];
    /*--------------------------------------------------------------------
     Ensure that a handler function table is registered, with a non-null
     get_pdn_conn_ind_hdlr. Then invoke the handler to dispatch the app
      mode specific function.
    --------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->cntxt_transfer_ind_hdlr != NULL )
      {
        hdlr_tbl_ptr->cntxt_transfer_ind_hdlr(target_rat, subs_id);
      }
    }
  }

  return;
} /* ds3gi_process_cm_ss_3gpp_cntxt_transfer_cmd() */
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RAB_REL_CMD

DESCRIPTION   This function processes the DS_CM_CALL_RAB_REL_CMD received from
              Call Manager.It simply calls the mode-specific rab_rel_handler.

              RAB release indicates that the Radio link has been released, but
              the call stays up. This is done when there is no activity on the
              RAB to conserve radio resources.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_RAB_REL_CMD is valid only in the
              DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all
              other call states.
              It is also ignored if the call id is not recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_cm_call_rab_rel_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  boolean                   hdlr_result;
  cm_end_params_s_type      end_params;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
  sys_sys_mode_e_type       cm_sys_mode=SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_RAB_REL_CMD");

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "CM_CALL_RAB_REL_CMD has unknown call id");
    return;
  }

  /*-------------------------------------------------------------------------
    Rab Reestablish is valid only in GWL mode.
  -------------------------------------------------------------------------*/
  cm_sys_mode = ds3gi_get_call_mode(cmd_ptr);
  if( !DS_3GPP_IS_3GPP_CURRENT_MODE(cm_sys_mode))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                      "ignoring RAB_REESTAB_REL_CMD rxd in non GWTL mode:%d",
                      cm_sys_mode);
      return;
    }


  /*-------------------------------------------------------------------------
    Rab Release is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if (ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE)
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr =
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return;
    }

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      rab_rel_handler.
    -----------------------------------------------------------------------*/

    if( hdlr_tbl_ptr->rab_rel_handler != NULL )
    {
      /*-------------------------------------------------------------------
        Invoke the mode-specific handler function.
      -------------------------------------------------------------------*/
      /*----------------------------------------------------------------
        Intialize end_params before invoking mode specific handler.
      ---------------------------------------------------------------*/
      memset((void*)&end_params,0,sizeof(cm_end_params_s_type));

      hdlr_result =
        hdlr_tbl_ptr->rab_rel_handler(ds3gi_call_info[call_inst].call_id,
                                      &end_params,
                                      call_info_cmd_ptr->subs_id);

      if( hdlr_result == DS3G_FAILURE )
      {
        ds3g_cm_client_id = ds3g_get_cm_client_id();
        /*-----------------------------------------------------------------
          The Mode specific handler does not handle RAB
          release - go ahead and release the call
        -----------------------------------------------------------------*/
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Handler rejected RAB REL, ending call");
        ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

        cm_ret_val = cm_mm_call_cmd_exten_end(ds3gi_cm_call_cmd_end_cb,
                                        NULL,
                                        ds3g_cm_client_id,
                                        1,
                                        &end_params);
        if (cm_ret_val == FALSE)
        {
          DATA_ERR_FATAL("CM call cmd end failed");
          return;
        }
      }
    } /* if rab_rel_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "RAB release handler not registered");
    }
  } /* if call_state == DS3G_CALL_ACTIVE_CALL_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "RAB_REL_CMD rx'ed in wrong call state");
  }

  return;
} /* ds3gi_process_cm_call_rab_rel_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RAB_REESTAB_IND_CMD

DESCRIPTION   This function processes the DS_CM_CALL_RAB_REESTAB_IND_CMD
              received from Call Manager.It simply calls the mode-specific
              rab_re_estab_ind_handler.

              RAB Restablishment indication means that the RAB has been
              re-established successfully for the call. The protocol layers
              should now bind to the new RAB that has been established. This
              could be in response to a RAB re-establishment request sent by
              the mode specific handler ( Mobile originated) or just the case
              where the network initiates the RAB re-establishment procedure.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_RAB_REESTAB_IND_CMD is valid only in the
              DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all
              other call states. It is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS None
===========================================================================*/
void ds3gi_process_cm_call_rab_reestab_ind_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  boolean                   hdlr_result;
  cm_end_params_s_type      end_params;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
  sys_sys_mode_e_type       cm_sys_mode=SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_RAB_REESTAB_IND_CMD");

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "CM_CALL_RAB_REESTAB_IND_CMD has unknown call id %d",
                    call_info_cmd_ptr->call_id);
    return;
  }

  cm_sys_mode = ds3gi_get_call_mode(cmd_ptr);
  /*-------------------------------------------------------------------------
    Rab Reestablish is valid only in GWL mode.
  -------------------------------------------------------------------------*/
  if( !DS_3GPP_IS_3GPP_CURRENT_MODE(cm_sys_mode))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                      "ignoring RAB_REESTAB_IND_CMD rxd in non GWTL mode:%d",
                      cm_sys_mode);
      return;
    }

  /*-------------------------------------------------------------------------
    Rab Reestablish is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if (ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE)
  {
    if(cm_sys_mode != ds3gi_call_info[call_inst].call_mode)
    {

      DATA_3GMGR_MSG3(MSG_LEGACY_ERROR,
                      "CM sys_mode:%d not equal to DSMGR call_mode:%d."
                      " CM mode_info_type:%d",
                      cm_sys_mode,
                      ds3gi_call_info[call_inst].call_mode,
                      call_info_cmd_ptr->mode_info.info_type);

      switch(ds3gi_call_info[call_inst].call_mode)
      {
        case SYS_SYS_MODE_GSM:
          if((cm_sys_mode != SYS_SYS_MODE_LTE) ||
             (call_info_cmd_ptr->mode_info.info_type != CM_CALL_MODE_INFO_GW_PS))
          {
            return;
          }
          break;

        case SYS_SYS_MODE_WCDMA:
          if((cm_sys_mode != SYS_SYS_MODE_LTE) ||
             (call_info_cmd_ptr->mode_info.info_type != CM_CALL_MODE_INFO_GW_PS))
          {
            return;
          }
          break;

        case SYS_SYS_MODE_LTE:
          if(call_info_cmd_ptr->mode_info.info_type != CM_CALL_MODE_INFO_LTE)
          {
            return;
          }
          break;

        case SYS_SYS_MODE_TDS:
          if((cm_sys_mode != SYS_SYS_MODE_LTE) ||
             (call_info_cmd_ptr->mode_info.info_type != CM_CALL_MODE_INFO_GW_PS))
          {
            return;
          }
          break;

        default:
          break;
      }
    }//call_mode mismatch

   /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ----------------------------------------------------------------------*/
    hdlr_tbl_ptr =
      ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));


    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return;
    }

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      rab_re_estab_ind_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr->rab_re_estab_ind_handler != NULL )
    {
      /*-------------------------------------------------------------------
        Invoke the mode-specific handler function.
      -------------------------------------------------------------------*/
      /*----------------------------------------------------------------
        Intialize end_params before invoking mode specific handler.
      ---------------------------------------------------------------*/
      memset((void*)&end_params,
             0,
             sizeof(cm_end_params_s_type));
      hdlr_result = hdlr_tbl_ptr->rab_re_estab_ind_handler
                                   (
                                     ds3gi_call_info[call_inst].call_id,
                                     &(call_info_cmd_ptr->mode_info),
                                     &end_params,
                                     call_info_cmd_ptr->subs_id
                                   );

      if( hdlr_result == DS3G_FAILURE )
      {
        ds3g_cm_client_id = ds3g_get_cm_client_id();
        /*-----------------------------------------------------------------
          The Mode specific handler did not handle RAB
          re-establishment confirm - go ahead and release the call
        -----------------------------------------------------------------*/
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Handler rejected RAB REESTAB, ending call");
        ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

        cm_ret_val = cm_mm_call_cmd_exten_end(ds3gi_cm_call_cmd_end_cb,
                                        NULL,
                                        ds3g_cm_client_id,
                                        1,
                                        &end_params
                                       );
        if (cm_ret_val == FALSE)
        {
          DATA_ERR_FATAL("CM call cmd end failed");
          return;
        }
      }
    } /* if rab_re_estab_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "RAB_REESTAB_IND_CMD hdlr not registered");
    }
  } /* if call_state == DS3G_CALL_ACTIVE_CALL_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "RAB_REESTAB_IND_CMD rx'ed in wrong call state");
  }
  return;
} /* ds3gi_process_cm_call_rab_reestab_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RAB_REESTAB_REJ_CMD

DESCRIPTION   This function processes the DS_CM_CALL_EVENT_RAB_REESTAB_REJ_CMD
              received from Call Manager.It simply calls the mode-specific
              call_conf_handler.

              RAB re-establishment reject is sent by the SM layers when the SM
              signalling layer procedures to re-establish the RAB have failed.
              This could be due to network rejecting the request due to loading
              conditions or other factors.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_EVENT_RAB_REESTAB_REJ_CMD is valid only in the
              DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all
              other call states. Its is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_rab_reestab_rej_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type   *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type   *hdlr_tbl_ptr = NULL;
  boolean                    hdlr_result;
  cm_end_params_s_type       end_params;
  dsgwl_ps_call_info_u_type *call_info;
  uint32                     call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                    cm_ret_val = FALSE;
  cm_client_id_type          ds3g_cm_client_id;
  sys_sys_mode_e_type        cm_call_mode = SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_RAB_REESTAB_REJ_CMD");

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "CM_CALL_EVENT_RAB_REESTAB_REJ_CMD has unknown call id");
    return;
  }

  /*-------------------------------------------------------------------------
    Validate the call mode.
  -------------------------------------------------------------------------*/
  cm_call_mode = ds3gi_get_call_mode(cmd_ptr);

  if(ds3gi_call_info[call_inst].call_mode  != cm_call_mode)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "RAB_REESTAB_REJ_CMD: CM sys mode %d is not equal to DSMGR mode %d",
                    cm_call_mode,
                    ds3gi_call_info[call_inst].call_mode);

    if( !DS_3GPP_IS_3GPP_CURRENT_MODE(cm_call_mode))
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                  "ignoring RAB_REESTAB_REJ_CMD rxd in non GWTL mode");
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Rab Reestablish reject is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if (ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE)
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr =
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));


    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,"hdlr_table_ptr is Null");
      return;
    }

    /*---------------------------------------------------------------------
      Allocated memory from the heap for  call info
    ---------------------------------------------------------------------*/
    call_info = (dsgwl_ps_call_info_u_type *)modem_mem_alloc(
                                     sizeof(dsgwl_ps_call_info_u_type),
                                  MODEM_MEM_CLIENT_DATA_CRIT);
    if(call_info == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Heap Memory Allocation failed");
      return;
    }
    /*-------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      rab_re_estab_rej_handler.
    -------------------------------------------------------------------*/
    memset((void*)call_info, 0, sizeof(dsgwl_ps_call_info_u_type));

    if( hdlr_tbl_ptr->rab_re_estab_rej_handler != NULL )
    {
      /*---------------------------------------------------------------
        Invoke the mode-specific handler function.
      ---------------------------------------------------------------*/
      switch (call_info_cmd_ptr->mode_info.info_type)
      {
#ifdef FEATURE_DATA_LTE
        case CM_CALL_MODE_INFO_LTE:
          memscpy((void*)&(call_info->lte_call),
                  sizeof(cm_lte_call_info_s_type),
                  (void*)&(call_info_cmd_ptr->mode_info.info.lte_call),
                  sizeof(cm_lte_call_info_s_type));
          break;
#endif /* FEATURE_DATA_LTE */
        case CM_CALL_MODE_INFO_GW_PS:
          memscpy((void*)&(call_info->gw_ps_call),
                  sizeof(cm_gw_ps_call_info_s_type),
                  (void*)&(call_info_cmd_ptr->mode_info.info.gw_ps_call),
                  sizeof(cm_gw_ps_call_info_s_type));
          break;
        default:
        DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "CM_CALL_EVENT_RAB_REESTAB_REJ_CMD "
                        "with wrong mode_info %d",
                        call_info_cmd_ptr->mode_info.info_type);
        modem_mem_free(call_info, MODEM_MEM_CLIENT_DATA_CRIT);
        return;
      }

      hdlr_result = hdlr_tbl_ptr->rab_re_estab_rej_handler
                              (
                                ds3gi_call_info[call_inst].call_id,
                                call_info
                              );
      modem_mem_free(call_info, MODEM_MEM_CLIENT_DATA_CRIT);

      if( hdlr_result == DS3G_FAILURE )
      {
        ds3g_cm_client_id = ds3g_get_cm_client_id();
        /*-------------------------------------------------------------
          The Mode specific handler did not handle RAB
          re-establishment reject - go ahead and release the call
         --------------------------------------------------------------*/
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Handler rejected RAB_REESTAB_REJ, ending call");
        ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;
        memset((void*)&end_params, 0, sizeof(cm_end_params_s_type));

        end_params.call_id = ds3gi_call_info[call_inst].call_id;
        end_params.info_type = call_info_cmd_ptr->mode_info.info_type;

        cm_ret_val = cm_mm_call_cmd_exten_end(ds3gi_cm_call_cmd_end_cb,
                                        NULL,
                                        ds3g_cm_client_id,
                                        1,
                                        &end_params
                                       );
        if (cm_ret_val == FALSE)
        {
          DATA_ERR_FATAL("CM call cmd end failed");
          return;
        }
      }
    } /* if rab_re_estab_rej_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "RAB re_estab_rej handler not registered");
    }
  } /* if call_state == DS3G_CALL_ACTIVE_CALL_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "RAB_REESTAB_REJ_CMD rx'ed in wrong call state");
  }

  return;
} /* ds3gi_process_cm_call_rab_reestab_rej_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RAB_REESTAB_FAIL_CMD

DESCRIPTION   This function processes the DS_CM_CALL_EVENT_RAB_REESTAB_FAIL_CMD
              received from Call Manager.It simply calls the mode-specific
              call_conf_handler.

              RAB re-establihment fail indication is sent when SM signalling
              layer proceeds successfully but the RRC layer could not complete
              the RAB setup.
              SM timesout on the RLC setup procedures and sends this event.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_EVENT_RAB_REESTAB_FAIL_CMD is valid only in the
              DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all
              other call states. Its is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_rab_reestab_fail_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  boolean                   hdlr_result;
  cm_end_params_s_type      end_params;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
  boolean                   cm_ret_val = FALSE;
  cm_client_id_type         ds3g_cm_client_id;
  sys_sys_mode_e_type       cm_sys_mode=SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_RAB_REESTAB_FAIL_CMD");

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "CM_CALL_EVENT_RAB_REESTAB_FAIL_CMD has unknown call id");
    return;
  }
  /*-------------------------------------------------------------------------
    Rab Reestablish is valid only in GWL mode.
  -------------------------------------------------------------------------*/
  cm_sys_mode = ds3gi_get_call_mode(cmd_ptr);
  if( !DS_3GPP_IS_3GPP_CURRENT_MODE(cm_sys_mode))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                      "ignoring RAB_REESTAB_FAIL_CMD rxd in non GWTL mode:%d",
                      cm_sys_mode);
      return;
    }
  /*-------------------------------------------------------------------------
    Rab Reestablish fail is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if (ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE)
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr =
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return ;
    }

    /*-------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      rab_re_estab_fail_handler.
    -------------------------------------------------------------------*/

    if( hdlr_tbl_ptr->rab_re_estab_fail_handler != NULL )
    {
      /*---------------------------------------------------------------
        Invoke the mode-specific handler function.
      ---------------------------------------------------------------*/
      /*----------------------------------------------------------------
        Intialize end_params before invoking mode specific handler.
      ---------------------------------------------------------------*/
      memset((void*)&end_params,
             0,
             sizeof(cm_end_params_s_type));

      hdlr_result = hdlr_tbl_ptr->rab_re_estab_fail_handler
                                (
                                  ds3gi_call_info[call_inst].call_id,
                                  &end_params,
                                  call_info_cmd_ptr->subs_id
                                );

      if( hdlr_result == DS3G_FAILURE )
      {
        ds3g_cm_client_id = ds3g_get_cm_client_id();
        /*-------------------------------------------------------------
          The Mode specific handler did not handle RAB
          re-establishment failure - go ahead and release the call
       --------------------------------------------------------------*/
        DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                        "Handler rejected RAB_REESTAB_FAIL, ending call");
        ds3gi_call_info[call_inst].call_state = DS3G_END_CALL_STATE;

        cm_ret_val = cm_mm_call_cmd_exten_end(ds3gi_cm_call_cmd_end_cb,
                                        NULL,
                                        ds3g_cm_client_id,
                                        1,
                                        &end_params
                                       );
        if (cm_ret_val == FALSE)
        {
          DATA_ERR_FATAL("CM call cmd end failed");
          return;
        }
      }
    } /* if rab_re_estab_fail_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "RAB re_estab_fail handler not registered");
    }
  } /* if call_state == DS3G_CALL_ACTIVE_CALL_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "RAB_REESTAB_FAIL_CMD rx'ed in wrong call state");
  }
  return;
} /* ds3gi_process_cm_call_rab_reestab_fail_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_SS_SRV_CHG_CMD

DESCRIPTION   This function processes the DS_CM_SS_SRV_CHG_CMD from
              Call Manager.

              This function checks the suspend state (ps_data_suspend)from CM
              and does the following:
               TRUE  : Calls the mode specific data suspend handler.
               FALSE : Calls the mode specific data suspend handler.

               This function also calls a function to set the new mode.

               Pseudo code for HDR mode
               [ HDR service availability]
               1. if ( (sys_mode == HDR) &&
                      (srv_status == service) )
                  HDR is available on main stack
                  In this case, we do not need to change anything, since
                  sys_mode is already set to HDR

              2. if ( ( (sys_mode == CDMA) || (sys_mode == No Service) ) &&
                      (hdr_hybrid == TRUE) &&
                      (hdr_srv_status == service) )
                 HDR is available on hybrid stack
                 In this case, we do need to set sys_mode to HDR mode

              Special case for LTE to HDR system change indication from CM

              When system changes from LTE to HDR, CM keeps sys_mode as LTE
              i.e. the main stack still says LTE but the hybrid stack indicates
              HDR. So when DS gets LTE on main stack, it should check if hybrid
              stack is active and if it is then DS should assume that the new
              system is HDR. It is the responsibility of CM to make sure that
              when system moves to LTE, hybrid stack is cleaned up.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_ss_srv_chg_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  uint8                     stack_idx;
  ds3g_ss_event_info_type  *ss_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  ss_event_info_ptr = (ds3g_ss_event_info_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3gi_process_cm_ss_srv_chg_cmd: asubs_id:%d num_stacks:%d "
                  "changed_fields_subs:%d",
                  ss_event_info_ptr->asubs_id,
                  ss_event_info_ptr->number_of_stacks,
                  ss_event_info_ptr->changed_fields_subs);

  for(stack_idx=0; stack_idx<ss_event_info_ptr->number_of_stacks; stack_idx++)
  {
    if(stack_idx > DS3G_SS_STACK_MAX)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "stack_idx:%d > maximum number of stacks", stack_idx);
      break;
    }

    DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                  "ds3gi_process_cm_ss_srv_chg_cmd: stack:[%d] "
                  "changed_fields:%d changed_fields2:%d",
                  stack_idx,
                  ss_event_info_ptr->stack_info[stack_idx].changed_fields,
                  ss_event_info_ptr->stack_info[stack_idx].changed_fields2);
  }

  ds3gstkproc_process_cm_ss_srv_chg_cmd(ss_event_info_ptr);

  return;
} /* ds3gi_process_cm_ss_srv_chg_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_PH_EVENT_CMD

DESCRIPTION   This function processes the DS_CMD_CM_PH_EV_INFO for
              Call Manager PH events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_cm_ph_event_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_cm_ph_ev_info_type  *ph_info_ptr = NULL;
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  sys_sys_mode_e_type       curr_mode = SYS_SYS_MODE_NONE;
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  ph_info_ptr = (ds3g_cm_ph_ev_info_type*)cmd_ptr->cmd_payload_ptr;

  /*-------------------------------------------------------------------------
    Get the CDMA subscription mode. Always SYS_MODEM_AS_ID_1
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  if(ds3gsubsmgr_is_device_mode_multi_sim_standby() == TRUE)
  {
    curr_mode = ds3g_get_current_network_mode_ex(SYS_MODEM_AS_ID_1);
  }
  else
  {
    curr_mode = ds3g_get_current_network_mode();
  }
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

  /*-------------------------------------------------------------------------
      Handling of CM PH events
  -------------------------------------------------------------------------*/
  switch( ph_info_ptr->ph_event )
  {
    case CM_PH_EVENT_OPRT_MODE:
    {
      /*---------------------------------------------------------------------
        Set service domain preference
      ---------------------------------------------------------------------*/
      ds3g_set_current_oprt_mode_srv_domain_ex(ph_info_ptr->srv_domain_pref,
                                               ph_info_ptr->as_id);

      /*---------------------------------------------------------------------
        Notify if operating mode has changed
      ---------------------------------------------------------------------*/
      if(ph_info_ptr->oprt_mode != ds_3gpp_cm_ph_info_oprt_mode)
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                        "oprt_mode change: Old_mode:%d, new_mode:%d",
                        ds_3gpp_cm_ph_info_oprt_mode,
                        ph_info_ptr->oprt_mode);

        ds_3gpp_cm_ph_info_oprt_mode = ph_info_ptr->oprt_mode;
        ds_3gpp_process_oprt_mode_chg_cmd(ph_info_ptr->oprt_mode);
      }

      /*---------------------------------------------------------------------
        Notify subsmgr if mode preference has changed
      ---------------------------------------------------------------------*/
      if(ph_info_ptr->mode_pref !=
         ds3gsubsmgr_get_mode_pref_for_subs_id(ph_info_ptr->as_id))
      {
        ds3gsubsmgr_set_mode_pref(ph_info_ptr->as_id, ph_info_ptr->mode_pref);
      }

      /*---------------------------------------------------------------------
        Process the command
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707      
      ds707_pkt_mgr_process_ph_event_oprt_mode_cmd(cmd_ptr);
#endif /* FEATURE_DATA_IS707 */
    }
    break;

    case CM_PH_EVENT_SYS_SEL_PREF:
    {
      if( (ds3g_cm_ph_state_emerg == TRUE) &&
          (ph_info_ptr->ph_state == CM_PH_STATE_NORMAL) )
      {
        ph_info_ptr->emerg_to_normal_mode = TRUE;
        ds3g_cm_ph_state_emerg = FALSE;
        ds3gdsdif_notify_emergergency_cb_mode(ph_info_ptr->as_id, FALSE);

#ifdef FEATURE_DATA_IS707
        ds707_pkt_mgr_process_ph_sys_sel_pref_normal_cmd(cmd_ptr);
#endif /* FEATURE_DATA_IS707 */
      }
      else if( (ds3g_cm_ph_state_emerg == FALSE) &&
               (ph_info_ptr->ph_state == CM_PH_STATE_EMERG_CB) )
      {
        ph_info_ptr->emerg_to_normal_mode = FALSE;
        ds3g_cm_ph_state_emerg = TRUE;
        ds3gdsdif_notify_emergergency_cb_mode(ph_info_ptr->as_id, TRUE);

#ifdef FEATURE_DATA_IS707
        ds707_pkt_mgr_process_ph_sys_sel_pref_normal_cmd(cmd_ptr);
#endif /* FEATURE_DATA_IS707 */
      }
      else
      {
        /*-------------------------------------------------------------------
          Notify mode pref info to subsmgr
        -------------------------------------------------------------------*/
        DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                        "Notify mode pref info. subs_id:%d, mode_pref: %d",
                        ph_info_ptr->as_id, ph_info_ptr->mode_pref);
        ds3gsubsmgr_set_mode_pref(ph_info_ptr->as_id, ph_info_ptr->mode_pref);
      }
    }
    break;

    case CM_PH_EVENT_INFO:
    {
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
      if(((curr_mode == SYS_SYS_MODE_CDMA) ||
          (curr_mode == SYS_SYS_MODE_HDR))    &&
         ((ph_info_ptr->device_mode == SYS_MODEM_DEVICE_MODE_SINGLE_SIM) ||
           (ph_info_ptr->as_id == SYS_MODEM_AS_ID_1)))
      {
        ph_info_ptr->emerg_to_normal_mode = FALSE;
      }
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

      /*---------------------------------------------------------------------
        Process standby info
      ---------------------------------------------------------------------*/
      ds3gi_process_cm_standby_info_cmd(cmd_ptr);

      /*---------------------------------------------------------------------
        Set service domain pref
      ---------------------------------------------------------------------*/
      ds3g_set_current_oprt_mode_srv_domain_ex(ph_info_ptr->srv_domain_pref,
                                               ph_info_ptr->as_id);
    }
    break;

    case CM_PH_EVENT_DUAL_STANDBY_PREF:
    {
      /*---------------------------------------------------------------------
        Process standby info
      ---------------------------------------------------------------------*/
      ds3gi_process_cm_standby_info_cmd(cmd_ptr);

      /*---------------------------------------------------------------------
        Notify if mode pref info has changed to subsmgr
      ---------------------------------------------------------------------*/
      if(ph_info_ptr->mode_pref !=
         ds3gsubsmgr_get_mode_pref_for_subs_id(ph_info_ptr->as_id))
      {
        ds3gsubsmgr_set_mode_pref(ph_info_ptr->as_id, ph_info_ptr->mode_pref);
      }
    }
    break;

    case CM_PH_EVENT_DATA_PRIORITY:
    {
      if( ph_info_ptr->priority_type == SYS_DATA_PRIORITY_LOW )
      {
        ds3gflowmgr_set_data_hi_prio( ph_info_ptr->as_id, FALSE );
      }
      else if( ph_info_ptr->priority_type == SYS_DATA_PRIORITY_HIGH )
      {
        ds3gflowmgr_set_data_hi_prio( ph_info_ptr->as_id, TRUE );
      }
      else
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid priority_type:%d",
                        ph_info_ptr->priority_type);
      }
    }
    break;

    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_CHANGED:
    {
      /*---------------------------------------------------------------------
        Notify if mode pref info has changed to subsmgr
      ---------------------------------------------------------------------*/
      if(ph_info_ptr->mode_pref !=
         ds3gsubsmgr_get_mode_pref_for_subs_id(ph_info_ptr->as_id))
      {
        ds3gsubsmgr_set_mode_pref(ph_info_ptr->as_id, ph_info_ptr->mode_pref);
      }
    }
    break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Unhandled Event %d",
                      ph_info_ptr->ph_event);
      break;
  }

  return;
} /* ds3gi_process_cm_ph_event_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CONTEXT_MODIFY_IND_CMD

DESCRIPTION   This function processes the DS_CM_PDP_MODIFY_IND_CMD from Call
              Manager.
              Used only in UMTS PS call handler.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_context_modify_ind_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_PDP_MODIFY_IND");

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "CM_CALL_EVENT_PDP_MODIFY_IND has unknown call id %d",
                    call_info_cmd_ptr->call_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Context modification is valid only for GSM/WCDMA PKT calls
  -------------------------------------------------------------------------*/
  if ( ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_GSM ) ||
       ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_WCDMA ) ||
       (ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_TDS)
     )
  {
    if ( ds3gi_call_info[call_inst].call_type != DS_CALL_TYPE_PKT )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                      "PDP_MODIFY_IND is valid only for PKT calls");
      return;
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "PDP_MODIFY_IND is valid only in GSM and WCDMA modes");
    return;
  }

  /*-------------------------------------------------------------------------
    context modification indication is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if ((ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE) &&
      (ds3gi_call_info[call_inst].call_mode  == ds3gi_get_call_mode(cmd_ptr))
     )
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr = 
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return;
    }

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      context_mod_ind_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr->context_mod_ind_handler != NULL )
    {
      /*-------------------------------------------------------------------
        Invoke the mode-specific handler function.
      -------------------------------------------------------------------*/
      hdlr_tbl_ptr->context_mod_ind_handler
                                   (
                                     ds3gi_call_info[call_inst].call_id,
                                     &(call_info_cmd_ptr->mode_info),
                                     call_info_cmd_ptr->subs_id
                                   );
    } /* if context_mod_ind_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "CM_CALL_EVENT_PDP_MODIFY_IND hdlr not registered");
    }
  } /* if call_state == DS3G_CALL_ACTIVE_CALL_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "CM_CALL_EVENT_PDP_MODIFY_IND rx'ed in wrong state or mode");
  }

  return;
} /* ds3gi_process_cm_context_modify_ind_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CONTEXT_MODIFY_CNF_CMD

DESCRIPTION   This function processes the DS_CM_PDP_MODIFY_CONF_CMD from Call
              Manager.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_context_modify_cnf_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;  /* Ptr to mode-sp. hdlr table */
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "CM_CALL_EVENT_PDP_MODIFY_CONF has unknown call id %d",
                    call_info_cmd_ptr->call_id);
    return;
  }

  /*-----------------------------------------------------------------------
    Context modification is valid only for GSM/WCDMA PKT calls
  -----------------------------------------------------------------------*/
  if ( ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_GSM ) ||
       ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_WCDMA ) ||
       ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_TDS )
     )
  {
    if ( ds3gi_call_info[call_inst].call_type != DS_CALL_TYPE_PKT )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                      "PDP_MODIFY_CONF is valid only for PKT calls");
      return;
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "PDP_MODIFY_CONF is valid only in GSM and WCDMA modes");
    return;
  }
  /*-------------------------------------------------------------------------
    Context modification Confirmation is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if ((ds3gi_call_info[call_inst].call_state ==
                                    DS3G_CALL_ACTIVE_CALL_STATE) &&
      (ds3gi_call_info[call_inst].call_mode  ==
                                      ds3gi_get_call_mode(cmd_ptr))
     )
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr =
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return;
    }

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      context_modify_cnf_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr->context_modify_cnf_handler != NULL )
    {
      /*-------------------------------------------------------------------
        Invoke the mode-specific handler function.
      -------------------------------------------------------------------*/
      hdlr_tbl_ptr->context_modify_cnf_handler
                                   (
                                     ds3gi_call_info[call_inst].call_id,
                                     &(call_info_cmd_ptr->mode_info),
                                     call_info_cmd_ptr->subs_id
                                   );
    } /* if context_modify_conf_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "CM_CALL_EVENT_PDP_MODIFY_CONF hdlr not registered");
    }
  } /* if call_state == DS3G_CALL_ACTIVE_CALL_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "CM_CALL_EVENT_PDP_MODIFY_CONF rx'ed in wrong state or mode");
  }

  return;
} /* ds3gi_process_cm_context_modify_cnf_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CONTEXT_MODIFY_REJ_CMD

DESCRIPTION   This function processes the DS_CM_PDP_MODIFY_REJ_CMD from
              Call Manager.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_context_modify_rej_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "CM_CALL_EVENT_PDP_MODIFY_REJ has unknown call id %d",
                    call_info_cmd_ptr->call_id);
    return;
  }
  /*-----------------------------------------------------------------------
    Context modification is valid only for GSM/WCDMA PKT calls
  -----------------------------------------------------------------------*/
  if ( ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_GSM ) ||
       ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_WCDMA ) ||
       ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_TDS )
     )
  {
    if ( ds3gi_call_info[call_inst].call_type != DS_CALL_TYPE_PKT )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                      "PDP_MODIFY_REJ is valid only for PKT calls");
      return;
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "PDP_MODIFY_REJ is valid only in GSM and WCDMA modes");
    return;
  }
  /*-------------------------------------------------------------------------
    Context modification REJ is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if ((ds3gi_call_info[call_inst].call_state ==
                                    DS3G_CALL_ACTIVE_CALL_STATE) &&
      (ds3gi_call_info[call_inst].call_mode  ==
                                      ds3gi_get_call_mode(cmd_ptr))
     )
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr =
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return;
    }

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      context_modify_rej_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr->context_modify_rej_handler != NULL )
    {
      /*-------------------------------------------------------------------
        Invoke the mode-specific handler function.
      -------------------------------------------------------------------*/
      hdlr_tbl_ptr->context_modify_rej_handler
                                   (
                                     ds3gi_call_info[call_inst].call_id,
                                     &(call_info_cmd_ptr->mode_info),
                                     call_info_cmd_ptr->subs_id
                                   );
    } /* if context_modify_rej_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "CM_CALL_EVENT_PDP_MODIFY_REJ hdlr not registered");
    }
  } /* if call_state == DS3G_CALL_ACTIVE_CALL_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "CM_CALL_EVENT_PDP_MODIFY_REJ rx'ed in wrong state or mode");
  }
  return;
} /* ds3gi_process_cm_context_modify_rej_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CONTEXT_PROMOTE_CMD

DESCRIPTION   This function processes the DS_CM_PDP_PROMOTE_IND_CMD from
              Call Manager.
              Used only in UMTS PS call handler.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_context_promote_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  uint32                    call_inst = DS3GI_INVALID_CALL_INSTANCE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed CM_CALL_EVENT_PDP_PROMOTE_IND");

  call_inst = ds3gi_find_call_instance(call_info_cmd_ptr->call_id);
  if (call_inst >= DS3GI_MAX_NUM_CALLS)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "CM_CALL_EVENT_PDP_PROMOTE_IND has unknown call id %d",
                    call_info_cmd_ptr->call_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Context promotion is valid only for GSM/WCDMA PKT calls
  -------------------------------------------------------------------------*/
  if ( ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_GSM ) ||
       ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_WCDMA ) ||
       ( ds3gi_call_info[call_inst].call_mode  == SYS_SYS_MODE_TDS )
     )
  {
    if ( ds3gi_call_info[call_inst].call_type != DS_CALL_TYPE_PKT )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                      "PDP_PROMOTE_IND is valid only for PKT calls");
      return;
    }
  }
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "PDP_PROMOTE_IND is valid only in GSM and WCDMA modes");
    return;
  }

  /*-------------------------------------------------------------------------
    Context promotion is valid only in the Active call state.
  -------------------------------------------------------------------------*/
  if ( ds3gi_call_info[call_inst].call_state == DS3G_CALL_ACTIVE_CALL_STATE )
  {
    /*---------------------------------------------------------------------
      Get a pointer to the appropriate handler function table based
      on the current mode and call type.
    ---------------------------------------------------------------------*/
    hdlr_tbl_ptr =
        ds3gi_get_hdlr_tbl_ptr_for_call_inst(&(ds3gi_call_info[call_inst]));

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return;
    }

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      rab_re_estab_ind_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr->context_prom_ind_handler != NULL )
    {
      /*-------------------------------------------------------------------
        Invoke the mode-specific handler function.
      -------------------------------------------------------------------*/
      hdlr_tbl_ptr->context_prom_ind_handler
                                   (
                                     ds3gi_call_info[call_inst].call_id,
                                     &(call_info_cmd_ptr->mode_info),
                                     call_info_cmd_ptr->subs_id
                                   );
    } /* if context_prom_ind_handler != NULL */
    else
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "CM_CALL_EVENT_PDP_PROMOTE_IND hdlr not registered");
    }
  } /* if call_state == DS3G_CALL_ACTIVE_CALL_STATE */
  else
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "CM_CALL_EVENT_PDP_PROMOTE_IND rx'ed in wrong state or mode");
  }
  return;
} /* ds3gi_process_cm_context_promote_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_SS_PS_DATA_FAIL_CMD

DESCRIPTION   This function processes the DS_CM_SS_PS_DATA_FAIL_CMD from
              Call Manager.
              This event is posted only when PS is in dormant mode & also in
              power save mode and CM could not acquire service to restore
              ps service.
              This function loops through all the registered handler function
              tables and calls the ps_data_fail_ind_handler() if it is
              registered.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_cm_ss_ps_data_fail_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  sys_sys_mode_e_type       mode;
  sys_modem_as_id_e_type   *subs_id_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  subs_id_ptr = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;

  mode = ds3g_get_current_network_mode();
  if (mode == SYS_SYS_MODE_NONE)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Invalid System Mode %d", mode);
    return;
  }

  /*-------------------------------------------------------------------------
    Point to the appropriate Packet handler function table based on the
    current mode.
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[mode][DS_CALL_TYPE_PKT];

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered.
  -------------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->ps_data_fail_ind_handler != NULL )
    {
      /*--------------------------------------------------------------------
        Invoke the mode-specific handler function.
      --------------------------------------------------------------------*/
      hdlr_tbl_ptr->ps_data_fail_ind_handler(*subs_id_ptr);
    }
  }

  return;
} /* ds3gi_process_cm_ss_ps_data_fail_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_STANDBY_INFO_CMD

DESCRIPTION   This function processes the DS_CM_STANDBY_INFO_CMD from
              Call Manager.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_standby_info_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_cm_ph_ev_info_type            *ph_info_ptr = NULL;
  sys_modem_dual_standby_pref_e_type  standby_pref;
  sys_modem_as_id_e_type              ps_subs_id = SYS_MODEM_AS_ID_1;
  sys_modem_as_id_e_type              new_subs_id;
  ds3gsubsmgr_subs_id_e_type          subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  ph_info_ptr = (ds3g_cm_ph_ev_info_type*)cmd_ptr->cmd_payload_ptr;

  /* first update the device mode */
  ds3gsubsmgr_set_device_mode(ph_info_ptr->device_mode);
  if (ph_info_ptr->device_mode == SYS_MODEM_DEVICE_MODE_SINGLE_SIM)
  {
    return;
  }

  /* Get a standby pref */
  standby_pref = ph_info_ptr->standby_pref;

  switch ( standby_pref )
  {
    case SYS_MODEM_DS_PREF_SINGLE_STANDBY:
      DATA_3GMGR_MSG3(MSG_LEGACY_MED,
                      "Received Single Standby Pref, active sub id [%d] "
                      "default data sub id [%d], default voice sub id [%d]",
                      ph_info_ptr->active_subs,
                      ph_info_ptr->default_data_subs,
                      ph_info_ptr->default_voice_subs);
      new_subs_id = ph_info_ptr->active_subs;

      if(!ds3gsubsmgr_is_subs_id_valid(new_subs_id))
      {
        break;
      }

      ds3gsubsmgr_set_single_standby_active_subs(new_subs_id);
      ds3gsubsmgr_set_standby_pref(standby_pref);

      ps_subs_id = ds3gsubsmgr_get_def_data_subs_id();
      DATA_MSG1(MSG_LEGACY_HIGH, "Current ps sub id is %d", ps_subs_id);

      if(ds3gsubsmgr_is_device_mode_multi_sim_standby() == TRUE)
      {

        if(!ds3gsubsmgr_is_subs_id_valid(ps_subs_id))
        {
          break;
        }

        subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( ps_subs_id );
        if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index))
        {
          break;
        }

        /* Update the network mode - subs id is in range*/
        /* Todo: check if this can come from sys change hdlr instead */
        ds3gi_update_3gpp_network_mode(
           ds3g_get_current_network_mode_ex(ps_subs_id),
           ps_subs_id);

        /* Update the suspend /resume flag based on the new subs_id */
        if(ds3g_ps_suspend_flag[subs_index] == FALSE)
        {
          ds3g_process_data_resume_ex(ps_subs_id);
        }
        else
        {
          ds3g_process_data_suspend_ex(ps_subs_id);
        }
      }
      break;

    case SYS_MODEM_DS_PREF_DUAL_STANDBY:
#ifdef FEATURE_TRIPLE_SIM
    case SYS_MODEM_DS_PREF_TRIPLE_STANDBY:
#endif /* FEATURE_TRIPLE_SIM */
      DATA_3GMGR_MSG3(
         MSG_LEGACY_MED,
         "Received Standby Pref %d  voice sub id[%d], data sub id[%d]",
         standby_pref,
         ph_info_ptr->default_voice_subs,
         ph_info_ptr->default_data_subs);

      new_subs_id = ph_info_ptr->default_data_subs;
      if(!ds3gsubsmgr_is_subs_id_valid(new_subs_id))
      {
        break;
      }

      ps_subs_id = ds3gsubsmgr_get_def_data_subs_id();
      DATA_MSG2(MSG_LEGACY_LOW, "ps sub id prev:%d, new:%d",
                ps_subs_id, new_subs_id);

      /* SET PS SUBS id */
      ds3gsubsmgr_set_def_data_subs_id( new_subs_id );
      /* SET Voice SUBS id */
      ds3gsubsmgr_set_def_voice_subs_id(ph_info_ptr->default_voice_subs);
      ds3gsubsmgr_set_standby_pref(standby_pref);

      if(ds3gsubsmgr_is_device_mode_multi_sim_standby() == TRUE)
      {
        if( ps_subs_id != new_subs_id )
        {
          DATA_MSG2(MSG_LEGACY_LOW, "Data Sub id is changed from [%d] to [%d]",
                    ps_subs_id,
                    new_subs_id);

  #ifdef FEATURE_DATA_IS707
          /*
             Call 3GPP2 DDS change handler function to tear down any dormant
             data call. CM will not send call end for dormant data call when
             DDS changes.
          */
          ds707_dds_change_hdlr( new_subs_id );
  #endif /* FEATURE_DATA_IS707 */
        }

        subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( new_subs_id );
        if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index))
        {
            break;
        }

        /* Update Network mode */
        ds3gi_update_3gpp_network_mode(
           ds3g_get_current_network_mode_ex(new_subs_id),
           new_subs_id);

        /* Update the suspend /resume flag based on the new subs_id */
        if(ds3g_ps_suspend_flag[subs_index] == FALSE)
        {
          ds3g_process_data_resume_ex(new_subs_id);
        }
        else
        {
          ds3g_process_data_suspend_ex(new_subs_id);
        }
      }
      break;

    case SYS_MODEM_DS_PREF_NO_CHANGE:
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Received the other Standby Pref [%d], we ignore ",
                      standby_pref);
      break;

  } /* Switch */

  return;
} /* ds3gi_process_cm_standby_info_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_STANDBY_SUB_INFO_CMD

DESCRIPTION   This function processes the DS_CM_STANDBY_SUB_INFO_CMD from
              Call Manager.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_standby_sub_info_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  sys_modem_as_id_e_type         sub_id        = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type         ps_subs_id    = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type         voice_subs_id = SYS_MODEM_AS_ID_NONE;
  boolean                        is_operational = FALSE;
  boolean                        is_default_data  = FALSE;
  boolean                        is_default_voice = FALSE;
  mmgsdi_session_type_enum_type  session_type_1x = 0;
  mmgsdi_session_type_enum_type  session_type_gw = 0;
  ds3gsubsmgr_subs_id_e_type     subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
  ds3g_cm_sub_info_type         *subs_info_ptr = NULL;
  sys_sys_mode_mask_e_type       old_subs_capability;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT ( cmd_ptr != NULL );
  ASSERT ( cmd_ptr->cmd_payload_ptr != NULL );

  subs_info_ptr = (ds3g_cm_sub_info_type*)cmd_ptr->cmd_payload_ptr;

  /* Get a subscription id */
  sub_id = subs_info_ptr->subscription_id;

  if(!ds3gsubsmgr_is_subs_id_valid(sub_id))
  {
    return;
  }

  /* Subscription info change event */
  is_operational = subs_info_ptr->is_operational;
  is_default_data  = subs_info_ptr->is_default_data;
  is_default_voice = subs_info_ptr->is_default_voice;
  session_type_1x  = subs_info_ptr->session_type_1x;
  session_type_gw  = subs_info_ptr->session_type_gw;

  ps_subs_id = ds3gsubsmgr_get_def_data_subs_id();
  voice_subs_id = ds3gsubsmgr_get_def_voice_subs_id();

  DATA_3GMGR_MSG9(MSG_LEGACY_MED,
                  "Received Sub_ID [%d], is_operational [%d], "
                  "is_default_data[%d], is_default_voice[%d] "
                  "is_subphone_active [%d] "
                  "session_type_1x [%d] session_type_gw [%d] "
                  "Current default data subs [%d], default voice subs [%d]",
                  sub_id, is_operational, is_default_data, is_default_voice,
                  subs_info_ptr->is_subphone_active,
                  session_type_1x, session_type_gw,
                  ps_subs_id, voice_subs_id);

  /*--------------------------------------------------------------------- 
    Notify subsmgr if subs capability has changed
  ---------------------------------------------------------------------*/
  old_subs_capability = ds3gsubsmgr_get_subs_capability_for_subs_id(sub_id);
  if (subs_info_ptr->subs_capability != old_subs_capability)
  {
    ds3gsubsmgr_set_subs_capability(sub_id, subs_info_ptr->subs_capability);
  }
  
  /* Update sub id with session types to mmgsdi table */
  if ( ds3g_mmgsdi_update_sub_id( session_type_1x, sub_id ) == FALSE )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "Did not update sub_id into mmgsdi table - "
                    "session_type[%d], sub_id[%d] ",
                    session_type_1x, sub_id);
  }

  if ( ds3g_mmgsdi_update_sub_id( session_type_gw, sub_id ) == FALSE )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "Did not update sub_id into mmgsdi table - "
                    "session_type[%d], sub_id[%d] ",
                    session_type_gw, sub_id);
  }

  /*
     If the is_operational flag is FALSE, then DS3G should not update the
     subs information as modem is offline. However, the session to subs id
     mapping is still valid in the event so DS3G is using that information
     to do the mapping in ds3gmmgsdiif.
  */
  if (is_operational == TRUE)
  {
    if (ds3gsubsmgr_is_device_mode_multi_sim_standby() == TRUE)
    {
      /* if it is for data sub */
      if ( is_default_data )
      {
        /* double check ps_sub_id with sub_id*/
        if ( ps_subs_id != sub_id )
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                          "Data Sub id is changed from [%d] to [%d] ",
                          ps_subs_id,sub_id);

    #ifdef FEATURE_DATA_IS707
          /*
             Call 3GPP2 DDS change handler function to tear down any dormant
             data call. CM will not send call end for dormant data call when
             DDS changes. DDS change handler function of C2K MH is called here
             as part of this dual standby sub info command because
             SYS_MODEM_DS_PREF_DUAL_STANDBY seems not guaranteed to be received
             first always when DDS switch happens.
          */
          ds707_dds_change_hdlr( sub_id );
    #endif /* FEATURE_DATA_IS707 */

          /* update ps_sub_id */
          ds3gsubsmgr_set_def_data_subs_id ( sub_id );

          subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( sub_id );

          /* Update the subs id */
          ds3gi_update_3gpp_network_mode(
             ds3g_get_current_network_mode_ex(sub_id),
             sub_id);

          if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index))
          {
            return;
          }

          /* Update the suspend /resume flag based on the new subs_id */
          if(ds3g_ps_suspend_flag[subs_index] == FALSE)
          {
            ds3g_process_data_resume_ex(sub_id);
          }
          else
          {
            ds3g_process_data_suspend_ex(sub_id);
          }
        }
      }
    }

    /* if it is for voice sub */
    if ( is_default_voice )
    {
      /* double check voice_sub_id with sub_id*/
      if ( voice_subs_id != sub_id )
      {
        DATA_3GMGR_MSG2(MSG_LEGACY_MED,
                        "Voice Sub id is changed from [%d] to [%d]  ",
                        sub_id, voice_subs_id);
        /* update voice_sub_id */
        ds3gsubsmgr_set_def_voice_subs_id ( sub_id );
      }
    }

    /*Call ATCoP sub info command handler */
    dsat_cm_ph_subs_pref_handler((void *)cmd_ptr);
  }
  else
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                    "sub [%d] is_operational [%d]. Not updating subs info",
                    sub_id, is_operational);
  }

  /* Send SUB_AVAIL_EV via mmgsdi if is_subphone_active is TRUE */
  if( subs_info_ptr->is_subphone_active )
  {

    if( subs_info_ptr->session_type_1x != MMGSDI_MAX_SESSION_TYPE_ENUM )
    {
      ds3g_mmgsdi_notify_event(DS3GEVENTMGR_SUB_AVAIL_EV,
                               subs_info_ptr->session_type_1x,
                               NULL);
    }

    if( subs_info_ptr->session_type_gw != MMGSDI_MAX_SESSION_TYPE_ENUM )
    {
      ds3g_mmgsdi_notify_event(DS3GEVENTMGR_SUB_AVAIL_EV,
                               subs_info_ptr->session_type_gw,
                               NULL);
    }
  }

  return;
} /* ds3gi_process_cm_standby_sub_info_cmd() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_PS_SIG_REL_CNF_CMD

DESCRIPTION   This function handles the command posted as a result of the
              dormancy indication CM_CALL_EVENT_PS_SIG_REL_CNF from CM.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_ps_sig_rel_cnf_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr;
  sys_sys_mode_e_type       mode;
  sys_modem_as_id_e_type    subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cmd_ptr != NULL);
  ASSERT(cmd_ptr->cmd_payload_ptr != NULL);

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;
  subs_id = call_info_cmd_ptr->subs_id;
  mode = ds3g_get_current_network_mode_ex(subs_id);

  if( mode == SYS_SYS_MODE_NONE )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                    "Invalid System Mode %d for subs_id:%d",
                    mode, subs_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Point to the appropriate Packet handler function table based on the
    current mode.
  -------------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3gi_hdlr_tbl[mode][DS_CALL_TYPE_PKT];

  /*-------------------------------------------------------------------------
    Ensure that a handler function table is registered.
  -------------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->ps_data_generic_ind_handler != NULL )
    {
      /*--------------------------------------------------------------------
        Invoke the mode-specific handler function.
      --------------------------------------------------------------------*/
      hdlr_tbl_ptr->ps_data_generic_ind_handler(call_info_cmd_ptr,
                                                subs_id);
    }
  }

  return;
} /* ds3gi_process_cm_ps_sig_rel_cnf_cmd() */

/*===========================================================================
FUNCTION      DS3G_GET_STANDBY_PREF

DESCRIPTION   Returns dual standby preference value stored in DS3G.

DEPENDENCIES  None

RETURN VALUE  Dual standby preference.

SIDE EFFECTS  None
===========================================================================*/
sys_modem_dual_standby_pref_e_type ds3g_get_standby_pref(void)
{
  return ds3gsubsmgr_get_standby_pref();
}/* ds3g_get_standby_pref() */

/*===========================================================================
FUNCTION      DS3G_CM_DATA_PACKET_STATE_NOTIFY

DESCRIPTION   When DS receives card REFRESH/PWROFF/LPM mode notification from
              CM, data session is torn down. This function notifies CM that
              data session is torn down.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_cm_data_packet_state_notify
(
  uint8 subs_id_msk
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "cm_data_packet_state_notify() called");
  cm_data_packet_state_notify(subs_id_msk);
  return;
} /* ds3g_cm_data_packet_state_notify() */

/*===========================================================================
FUNCTION      DS3G_DISPATCH_MODE_PREF_CHANGE_HANDLER

DESCRIPTION   This function only dispatches the mode handlers sys change
              due to mode pref chnage handlers.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dispatch_mode_pref_change_handler
(
  sys_sys_mode_e_type    old_mode,
  sys_sys_mode_e_type    new_mode,
  sys_modem_as_id_e_type subs_id
)
{
  uint8               call_type;
  uint8               sys_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    for( sys_mode = 0; sys_mode < (uint8)SYS_SYS_MODE_MAX; sys_mode++ )
    {
      for( call_type = 0; call_type < (uint8)DS_NUM_CALL_TYPES; call_type++ )
      {
        if ((ds3gi_hdlr_tbl[sys_mode][call_type] != NULL) &&
            (ds3gi_hdlr_tbl[sys_mode][call_type]->mode_pref_change_handler != NULL))
        {
          ds3gi_hdlr_tbl[sys_mode][call_type]->mode_pref_change_handler(
                                                    subs_id,
                                                    new_mode,
                                                    old_mode);
        } /* hdlr func not NULL */
      } /* call type loop */
    } /* sys mode loop */
  }
}/* ds3g_dispatch_mode_pref_change_handler */

/*===========================================================================
FUNCTION      DS3G_DISPATCH_SYS_CHANGE_HANDLER_EX

DESCRIPTION   This function only dispatches the mode handlers sys change
              handlers.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dispatch_sys_change_handler_ex
(
  sys_sys_mode_e_type     old_mode,
  sys_sys_mode_e_type     new_mode,
  sys_modem_as_id_e_type  subs_id
)
{
  uint8               call_type;
  uint8               sys_mode;
  sys_srv_domain_e_type   srv_domain = SYS_SRV_DOMAIN_NO_SRV;
  sys_srv_status_e_type   curr_srv_status = SYS_SRV_STATUS_NO_SRV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  curr_srv_status = ds3g_get_current_srv_status_ex(subs_id);
  srv_domain = ds3g_get_current_regd_srv_domain_ex(subs_id);

  DATA_3GMGR_MSG5(MSG_LEGACY_MED,
                  "sys_change_handler. subs_id:%d old_mode:%d, new_mode:%d "
                  "curr srv status:%d srv domain %d",
                  subs_id, old_mode, new_mode, curr_srv_status, srv_domain);


  for( sys_mode = 0; sys_mode < (uint8)SYS_SYS_MODE_MAX; sys_mode++ )
  {
    for( call_type = 0; call_type < (uint8)DS_NUM_CALL_TYPES; call_type++ )
    {
      if ((ds3gi_hdlr_tbl[sys_mode][call_type] != NULL) &&
          (ds3gi_hdlr_tbl[sys_mode][call_type]->sys_change_handler != NULL))
      {
        ds3gi_hdlr_tbl[sys_mode][call_type]->sys_change_handler( subs_id,
                                                                 new_mode,
                                                                 old_mode);
      } /* hdlr func not NULL */
    } /* call type loop */
  } /* sys mode loop */

  return;
}

/*===========================================================================
FUNCTION      DS3G_DISPATCH_SYS_CHANGE_HANDLER

DESCRIPTION   This function only dispatches the mode handlers sys change
              handlers.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dispatch_sys_change_handler
(
  sys_sys_mode_e_type old_mode,
  sys_sys_mode_e_type new_mode
)
{
   sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   subs_id = ds3g_get_ps_subs_id();
   ds3g_dispatch_sys_change_handler_ex(old_mode, new_mode,subs_id);
   return;
} /* ds3g_dispatch_sys_change_handler() */

/*===========================================================================
FUNCTION      DS3G_DISPATCH_BSR_RESET_HANDLER

DESCRIPTION   This function only dispatches the mode handlers BSR reset
              handler.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dispatch_bsr_reset_handler
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint8               call_type;
  uint8               sys_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3g_dispatch_bsr_reset_handler");

  for( sys_mode = 0; sys_mode < (uint8)SYS_SYS_MODE_MAX; sys_mode++ )
  {
    for( call_type = 0; call_type < (uint8)DS_NUM_CALL_TYPES; call_type++ )
    {
      if ((ds3gi_hdlr_tbl[sys_mode][call_type] != NULL) &&
          (ds3gi_hdlr_tbl[sys_mode][call_type]->bsr_reset_hdlr != NULL))
      {
        ds3gi_hdlr_tbl[sys_mode][call_type]->bsr_reset_hdlr(subs_id);
      } /* hdlr func not NULL */
    } /* call type loop */
  } /* sys mode loop */
  return;
}/* ds3g_dispatch_bsr_reset_handler() */

/*===========================================================================
FUNCTION      DS3G_PLMN_CHANGE_HDLR

DESCRIPTION   This function  is used to notify PLMN change so that network
              override table can be reset for cause code 50 and 51 throttling

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_plmn_change_hdlr
(
  ds_cmd_type  *cmd_ptr
)
{
  uint8                      call_type;
  ds_3gpp_plmn_info_type     plmn_info;
  sys_sys_mode_e_type        sys_mode;
  sys_plmn_id_s_type         old_plmn_id;
  sys_plmn_id_s_type         new_plmn_id;
  sys_modem_as_id_e_type     subs_id;
  uint32                     plmn_id_changed = 0;
  ds3gsubsmgr_subs_id_e_type ds_subs_id;
  ds_dsd_apm_ind_pay_load     apm_ind_payload = {0};  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }

  memset(&plmn_info, 0, sizeof(ds_3gpp_plmn_info_type));

  memset(&apm_ind_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));

  memscpy(&plmn_info, sizeof(ds_3gpp_plmn_info_type),
          cmd_ptr->cmd_payload_ptr, sizeof(ds_3gpp_plmn_info_type));

  sys_mode = plmn_info.sys_mode;

  if(sys_mode < SYS_SYS_MODE_NO_SRV || sys_mode >= SYS_SYS_MODE_MAX )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "ds3g_plmn_change_hdlr: invalid sys_mode: %d", sys_mode);
    return;
  }

  memset(&old_plmn_id, 0, sizeof(old_plmn_id));
  subs_id = plmn_info.subs_id;

  ds_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if (DS3GSUBSMGR_SUBS_ID_INVALID != ds_subs_id)
  {
    memscpy(&old_plmn_id, sizeof(&old_plmn_id),
            &ds3gi_plmn_id[ds_subs_id], sizeof(sys_plmn_id_s_type));

    memscpy(&new_plmn_id, sizeof(sys_plmn_id_s_type),
            &(plmn_info.plmn_id), sizeof(sys_plmn_id_s_type));

    /*-------------------------------------------------------------------------
      Invoke the handler for PLMN change
    --------------------------------------------------------------------------*/
    plmn_id_changed = memcmp(&old_plmn_id,
                             &new_plmn_id,
                             sizeof(sys_plmn_id_s_type));

    if (plmn_id_changed != 0)
    {
      DATA_MSG2(MSG_LEGACY_HIGH,
                "PLMN changed on cm subs_id: %d, sys_mode: %d.New PLMN ID is ",
                subs_id, sys_mode);
      ds3g_print_plmn_id(&new_plmn_id);

      /*---------------------------------------------------------------------
        Notify APM module that PLMN ID has been changed.
        - We are not passing any information with PLMN ID.
      ----------------------------------------------------------------------*/
      apm_ind_payload.subs_id = subs_id;
      ds_dsd_apm_ind(DS_DSD_APM_PLMN_CHANGED_IND, &apm_ind_payload);
      memscpy(&ds3gi_plmn_id[ds_subs_id], sizeof(sys_plmn_id_s_type),
              &new_plmn_id, sizeof(sys_plmn_id_s_type));

      for( call_type = 0; call_type < (uint8)DS_NUM_CALL_TYPES; call_type++ )
      {
        if ((ds3gi_hdlr_tbl[sys_mode][call_type] != NULL) &&
            (ds3gi_hdlr_tbl[sys_mode][call_type]->plmn_change_hdlr != NULL))
        {
          ds3gi_hdlr_tbl[sys_mode][call_type]->plmn_change_hdlr(old_plmn_id,
                                                                subs_id);
        } /* hdlr func not NULL */
      } /* call type loop */
    }
  }
  return;
}/* ds3g_plmn_change_hdlr() */

/*===========================================================================
FUNCTION      DS3G_SET_CURRENT_PLMN_ID

DESCRIPTION   This function sets the new_plmn_id.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_current_plmn_id
(
  sys_plmn_id_s_type     *new_plmn_id,
  sys_sys_mode_e_type    call_mode,
  sys_modem_as_id_e_type subs_id
)
{
  ds_cmd_type             *cmd_buf = NULL;
  ds_3gpp_plmn_info_type   plmn_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(new_plmn_id == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "New plmn id pointer is NULL, returning");
    return;
  }

  /*-----------------------------------------------------------------------
    Get a command buffer from the DS Task.
  -----------------------------------------------------------------------*/
  cmd_buf = ds_allocate_cmd_buf(sizeof(ds_3gpp_plmn_info_type));
  if((NULL == cmd_buf) || (NULL == cmd_buf->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_buf->hdr.cmd_id = DS_CMD_CM_PLMN_ID_CHG;
    memset(&(plmn_info), 0, sizeof(ds_3gpp_plmn_info_type));

    memscpy(&(plmn_info.plmn_id), sizeof(sys_plmn_id_s_type),
            new_plmn_id, sizeof(sys_plmn_id_s_type));

    plmn_info.subs_id = subs_id;
    plmn_info.sys_mode = call_mode;

    memscpy(cmd_buf->cmd_payload_ptr, sizeof(ds_3gpp_plmn_info_type),
            &plmn_info, sizeof(ds_3gpp_plmn_info_type));
    ds_put_cmd(cmd_buf);
  }

  return;
} /* ds3g_set_current_plmn_id() */

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_PLMN_ID

DESCRIPTION   This function gets the current_plmn_id.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_get_current_plmn_id
(
  sys_plmn_id_s_type     *current_plmn_id,
  sys_modem_as_id_e_type subs_id
)
{
  ds3gsubsmgr_subs_id_e_type ds_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  if( DS3GSUBSMGR_SUBS_ID_INVALID == ds_subs_id )
  {
    return;
  }

  if (NULL != current_plmn_id)
  {
    memscpy(current_plmn_id,
            sizeof(sys_plmn_id_s_type),
            &(ds3gi_plmn_id[ds_subs_id]),
            sizeof(sys_plmn_id_s_type));
    ds3g_print_plmn_id(&(ds3gi_plmn_id[ds_subs_id]));
  }
  return;
} /* ds3g_get_current_plmn_id() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_DATA_SUSPEND_EX

DESCRIPTION   If the suspend flag is set to TRUE, the mode specific Suspend Ind
              handler is called.This function does not check if the current
              suspend state is different than the previous suspend state.
              It is the responsiblility of the mode specific handler to check
              the suspend state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_data_suspend_ex
(
  sys_modem_as_id_e_type subs_id
)
{
  ds3g_hdlr_func_tbl_type    *hdlr_tbl_ptr;
  uint8                       call_type;
  sys_sys_mode_e_type         mode;
  ds3gsubsmgr_subs_id_e_type  subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  mode = ds3g_get_current_network_mode_ex(subs_id);

  if( mode <= SYS_SYS_MODE_NONE || mode >= SYS_SYS_MODE_MAX )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid System Mode %d", mode);
    return;
  }

  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  /* Update the Suspend flag as TRUE */
  UPDATE_DS3G_SUSPEND_FLAG(subs_index, TRUE)

  /*-------------------------------------------------------------------------
    Loop through all the registered handler function tables for the
    current mode.
  -------------------------------------------------------------------------*/
  for(call_type = (uint8)DS_CALL_TYPE_CKT; call_type < (uint8)DS_NUM_CALL_TYPES;
                                                       call_type++)
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[mode][call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      rab_re_estab_fail_handler.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->data_suspend_ind_handler != NULL )
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Suspend Data Flow on sub: %d",
                        subs_id);
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_tbl_ptr->data_suspend_ind_handler(subs_id);
      }
    }
  }
  return;
} /* ds3g_process_data_suspend_ex() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_DATA_SUSPEND

DESCRIPTION   This is a wrapper function for the ds3gi_process_data_suspend.
              This function will be called from DSD IF depending on the
              suspend flag. This is called only for FEATURE DUAL SIM not
              defined. IF the feature is defined, ds3gi_process_data_suspend is
              called from SS event processing. If the suspend flag is set to
              TRUE, the mode specific Suspend Ind handler is called.

              This function does not check if the current suspend state is
              different than the previous suspend state. It is the
              responsiblility of the mode specific handler to check the suspend
              state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_data_suspend
(
  sys_sys_mode_e_type  mode
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  ds3g_process_data_suspend_ex(subs_id);
  return;
}/* ds3g_process_data_suspend() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_DATA_RESUME_EX

DESCRIPTION   If the suspend flag is set to FALSE, the mode specific Resume
              handler is called.
              This function does not check if the current suspend state is
              different than the previous suspend state. It is the
              responsiblility of the mode specific handler to check the suspend
              state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_data_resume_ex
(
  sys_modem_as_id_e_type subs_id
)
{
  ds3g_hdlr_func_tbl_type    *hdlr_tbl_ptr;
  uint8                       call_type;
  sys_sys_mode_e_type         mode;
  ds3gsubsmgr_subs_id_e_type  subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  mode = ds3g_get_current_network_mode_ex(subs_id);

  if (mode <= SYS_SYS_MODE_NONE || mode >= SYS_SYS_MODE_MAX )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid System Mode %d", mode);
    return;
  }

  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );

  /* Update the Suspend flag as FALSE */
  UPDATE_DS3G_SUSPEND_FLAG(subs_index,FALSE)

  /*-------------------------------------------------------------------------
    Loop through all the registered handler function tables for the
    current mode.
  -------------------------------------------------------------------------*/
  for(call_type = (uint8)DS_CALL_TYPE_CKT; call_type < (uint8)DS_NUM_CALL_TYPES;
                                                       call_type++)
  {
    /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
    -----------------------------------------------------------------------*/
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[mode][call_type];

    /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered.
    -----------------------------------------------------------------------*/
    if( hdlr_tbl_ptr != NULL )
    {
      if( hdlr_tbl_ptr->data_resume_ind_handler != NULL )
      {
        DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Resume Data Flow on sub: %d",
                        subs_id);
        /*-------------------------------------------------------------------
          Invoke the mode-specific handler function.
        -------------------------------------------------------------------*/
        hdlr_tbl_ptr->data_resume_ind_handler(subs_id);
      }
    }/* hdlr_tbl_ptr != NULL */
  }/* for ... */

  return;
} /* ds3g_process_data_resume_ex() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_DATA_RESUME

DESCRIPTION   This is a wrapper function for the ds3gi_process_data_resume.
              This function will be called from DSD IF depending on the
              suspend flag. This is called only for FEATURE DUAL SIM not
              defined. IF the feature is defined, ds3gi_process_data_resume is
              called from SS event processing.

              If the suspend flag is set to FALSE, the mode specific
              Resume handler is called.
              This function does not check if the current suspend state is
              different than the previous suspend state. It is the
              responsiblility of the mode specific handler to check the suspend
              state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_data_resume
(
  sys_sys_mode_e_type  mode
)
{
  sys_modem_as_id_e_type subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  ds3g_process_data_resume_ex(subs_id);
  return;
} /* ds3g_process_data_resume() */

#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
/*===========================================================================
FUNCTION      DS3GI_CM_AC_EVENT_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of
              a certain access control event.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ac_event_cb
(
  cm_ac_event_e_type        event,
  const cm_ac_info_s_type  *info_ptr
)
{
  ds_cmd_type                *cmd_ptr = NULL;
  ds3g_cm_ac_event_info_type *ac_event_info_ptr = NULL;
  boolean                     notify = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL == info_ptr)
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW, "ds3gi_cm_ac_event_cb,invalid info_ptr");
    return;
  }

  DATA_3GMGR_MSG3( MSG_LEGACY_HIGH,
                   "ds3gi_cm_ac_event_cb event:%d, client:%d, reason:%d",
                   event,
                   info_ptr->ac_client_id,
                   info_ptr->reason);
  switch(event)
  {
    case CM_AC_EVENT_EMERGENCY_ENTER:
      notify = TRUE;
      break;

    case CM_AC_EVENT_EMERGENCY_EXIT:
      notify = TRUE;
      break;

    default:
      notify = FALSE;
      break;
  }

  if(notify == TRUE)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_cm_ac_event_info_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_CM_AC_EVENT;
    ac_event_info_ptr = (ds3g_cm_ac_event_info_type*)cmd_ptr->cmd_payload_ptr;
    ac_event_info_ptr->event = event;
    ds_put_cmd( cmd_ptr );
  }

  return;
} /* ds3gi_cm_ac_event_cb() */
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/

/*===========================================================================
FUNCTION      DS3GI_CM_AC_CMD_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of
              the completion status of the Acess Control command previously
              issued by 3G Dsmgr.
              This callback function may be invoked in relation to the
              following call commands: cm_ac_cmd_1xPPP_nw_cleanup().
              In each case, if the command was successful, no action is taken.
              If the cm_ac_cmd_1xPPP_nw_cleanup() was unsuccessful we will
              ASSERT.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ac_cmd_cb
(
  void                   *data_block_ptr,        /* Data blk provided in call cmd */
  cm_ac_cmd_e_type        ac_cmd,                /* Access Control command id */
  cm_ac_cmd_err_e_type    ac_cmd_err             /* Access Control command error code */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG2( MSG_LEGACY_HIGH,
                   "ds3gi_cm_ac_cmd_cb Cmd: %d, err %d", ac_cmd, ac_cmd_err);

  if( CM_AC_CMD_ERR_NOERR != ac_cmd_err)
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,"ds3gi_cm_ac_cmd_cb: ac_cmd:%d err: %d",
                    ac_cmd, ac_cmd_err);
    /*-----------------------------------------------------------------------
      Some type of error occurred, take action based on the the type of
      command.
    -----------------------------------------------------------------------*/
    switch( ac_cmd )
    {
      case CM_AC_CMD_CLEAR_1XPPP:
      {
        ASSERT(0);
        break;
      }
#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
      case CM_AC_CMD_EMERGENCY_ONLY:
      {
        /*-------------------------------------------------------------------
          Assert as the errors reported by CM are non-recovable
        -------------------------------------------------------------------*/
        ASSERT(0);
        break;
      }
      case CM_AC_CMD_NORMAL_OPERATION:
      {
        /*-------------------------------------------------------------------
          Assert as the errors reported by CM are non-recovable
        -------------------------------------------------------------------*/
        ASSERT(0);
        break;
      }
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/
      default:
        break;
    } /* switch( ac_cmd ) */
  }
  return;
} /* ds3gi_cm_ac_cmd_cb() */

/*===========================================================================
FUNCTION      DS3GI_CM_AC_STATE_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of
              the current status of the Acess Control command previously issued
              by 3G Dsmgr.
              This callback function may be invoked in relation to the
              following call commands: cm_ac_cmd_1xPPP_nw_cleanup()

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ac_state_cb
(
  void                    *data_block_ptr,/* Data blk provided in call cmd */
  cm_ac_cmd_e_type         ac_cmd,        /* Access Control command id */
  cm_ac_state_e_type       state,         /* Access Control command state*/
  cm_ac_end_reason_e_type  end_reason     /* Access Control state end reason */
)
{
  ds_cmd_enum_type             cmd_id = DS_MIN_CMD;
  ds_cmd_type                 *cmd_ptr = NULL;
  ds3g_cm_ac_state_info_type  *ac_state_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG3( MSG_LEGACY_HIGH,
                   "ds3gi_cm_ac_state_cb Cmd: %d, state: %d, err %d",
                   ac_cmd, state, end_reason);

  switch( ac_cmd )
  {
    case CM_AC_CMD_CLEAR_1XPPP:
      cmd_id = DS_CMD_CM_AC_STATE;
      break;
    default:
      break;
  } /* switch( ac_cmd ) */

  if(cmd_id  != DS_MIN_CMD)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_cm_ac_state_info_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
    else
    {
      /*---------------------------------------------------------------------
        Fill in the command parameters and send the command.
      ---------------------------------------------------------------------*/
      cmd_ptr->hdr.cmd_id = cmd_id;
      ac_state_info_ptr
        = (ds3g_cm_ac_state_info_type*)cmd_ptr->cmd_payload_ptr;

      ac_state_info_ptr->state      = state;
      ac_state_info_ptr->end_reason = end_reason;

      ds_put_cmd( cmd_ptr );
    }
  }
} /* ds3gi_cm_ac_state_cb() */

#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
/*===========================================================================
FUNCTION      DS3GI_SET_CM_EMERGENCY_ONLY_MODE_EX

DESCRIPTION   This function calls cm_ac_cmd_control_nw_access to issue
              whether enter or exit emergency only mode.

DEPENDENCIES  None

RETURN VALUE  TRUE: SUCC, FALSE: FAIL

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_set_cm_emergency_only_mode_ex
(
  boolean                 is_emergency_only,
  sys_modem_as_id_e_type  subs_id
)
{
  cm_ac_cmd_e_type ac_cmd = CM_AC_CMD_NORMAL_OPERATION;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(is_emergency_only)
  {
    ac_cmd = CM_AC_CMD_EMERGENCY_ONLY;
  }
  else
  {
    ac_cmd = CM_AC_CMD_NORMAL_OPERATION;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3gi_set_cm_emergency_only_mode:%d",
                  is_emergency_only);
  if(!cm_ac_cmd_control_nw_access(
                          ds3gi_cm_ac_cmd_cb,
                          0,
                          ds3g_get_cm_client_id(),
                          ac_cmd,
                          CM_AC_REASON_MAND_PDN_DISABLED,
                          subs_id
                               ))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                    "cm_ac_cmd_control_nw_access return false!");
    return FALSE;
  }
  return TRUE;
} /* ds3gi_set_cm_emergency_only_mode_ex() */

/*===========================================================================
FUNCTION      DS3GI_SET_CM_EMERGENCY_ONLY_MODE

DESCRIPTION   This function calls cm_ac_cmd_control_nw_access to issue whether
              enter or exit emergency only mode.

DEPENDENCIES  None

RETURN VALUE  TRUE: SUCC, FALSE: FAIL

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_set_cm_emergency_only_mode
(
  boolean  is_emergency_only
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id = ds3g_get_ps_subs_id();
  return ds3gi_set_cm_emergency_only_mode_ex(is_emergency_only, subs_id);
} /* ds3gi_set_cm_emergency_only_mode() */

/*===========================================================================
FUNCTION      DS3GI_REG_AC_EVENT

DESCRIPTION   This function register the callback functions for CM access
              control events.

DEPENDENCIES  None

RETURN VALUE  TRUE: SUCC, FALSE: FAIL

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_reg_ac_event(void)
{
  cm_client_status_e_type  ac_event_reg_status = CM_CLIENT_OK;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ac_event_reg_status = cm_client_ac_reg(
                       ds3g_get_cm_client_id(),
                       ds3gi_cm_ac_event_cb,
                       CM_CLIENT_EVENT_REG,
                       CM_AC_EVENT_EMERGENCY_ENTER,
                       CM_AC_EVENT_EMERGENCY_EXIT,
                       NULL
                        );
    if(ac_event_reg_status != CM_CLIENT_OK)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "reg cm ac event fails: %d", ac_event_reg_status);
      return FALSE;
    }
  return TRUE;
} /* ds3gi_reg_ac_event() */
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/

/*===========================================================================
FUNCTION      ds3g_is_ph_state_emergency_cb_mode

DESCRIPTION   Function returns if UE is in emergency callback mode or not.

DEPENDENCIES  None

RETURN VALUE  TRUE:  UE is in emergency callback mode
              FALSE: UE is not in emergency callback mode

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_ph_state_emergency_cb_mode(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1( MSG_LEGACY_HIGH,
                   "ds3g_is_ph_state_emergency_cb_mode cm_ph_state_emerg:%d",
                   ds3g_cm_ph_state_emerg);
  return ds3g_cm_ph_state_emerg;
}/* ds3g_is_ph_state_emergency_cb_mode() */

/*===========================================================================
FUNCTION      ds3g_cm_ph_sys_sel_cb

DESCRIPTION   Callback function to be called by CM when system selection
              enable/disable is performed by DS.
              Post a cmd only if an error value is received.

DEPENDENCIES  None

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds3g_cm_ph_sys_sel_cb
(
  void                   *data_block_ptr,
  cm_ph_cmd_e_type        ph_cmd,
  cm_ph_cmd_err_e_type    ph_cmd_err
)
{
  ds_cmd_type      *cmd_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------
  If ph_cmd is not SYS_SEL_PREF return, Function ptr called incorrectly
  -------------------------------------------------------------------------*/

  if(ph_cmd != CM_PH_CMD_SYS_SEL_PREF)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"CM_PH_CMD_ERR:%d",ph_cmd);
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "SYS_SEL_PREF_ERR:%d", ph_cmd_err);

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(cmd_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  if(ph_cmd_err == CM_PH_CMD_ERR_NOERR)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_SYS_SEL_PREF_NO_ERR;
    cmd_ptr->cmd_payload_ptr = data_block_ptr;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_SYS_SEL_PREF_ERR;
  }

  ds_put_cmd(cmd_ptr);
  return;
} /* ds3g_cm_ph_sys_sel_cb() */

/*===========================================================================
FUNCTION      ds3g_get_sys_sel_cb_fn_ptr

DESCRIPTION   Returns the sys_sel_cb_fn_ptr

DEPENDENCIES  None

RETURN VALUE  void *

SIDE EFFECTS  None
===========================================================================*/
void *ds3g_get_sys_sel_cb_fn_ptr
(
  void
)
{
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - -*/
  return (void *)ds3g_cm_ph_cmd_cb_f_ptr;
} /* ds3g_get_sys_sel_cb_fn_ptr() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_FLOW_DISABLE_EX

DESCRIPTION   Mode specific flow disable handler is called. This function
              does not check if the current flow state is different than the
              previous flow state.
              It is the responsiblility of the mode specific handler to check
              the flow state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_flow_disable_ex
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8               call_type;
  uint8               sys_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3g_process_flow_disable_ex. subs_id:%d",
                  subs_id);

  for( sys_mode = 0; sys_mode < (uint8)SYS_SYS_MODE_MAX; sys_mode++ )
  {
    for( call_type = 0; call_type < (uint8)DS_NUM_CALL_TYPES; call_type++ )
    {
      if ((ds3gi_hdlr_tbl[sys_mode][call_type] != NULL) &&
          (ds3gi_hdlr_tbl[sys_mode][call_type]->coex_flow_disable_hdlr != NULL))
      {
        ds3gi_hdlr_tbl[sys_mode][call_type]->coex_flow_disable_hdlr( subs_id );
      } /* hdlr func not NULL */
    } /* call type loop */
  } /* sys mode loop */
  return;
} /* ds3g_process_flow_disable_ex() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_FLOW_ENABLE_EX

DESCRIPTION   Mode specific flow disable handler is called. This function
              does not check if the current flow state is different than the
              previous flow state.
              It is the responsiblility of the mode specific handler to check
              the flow state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_flow_enable_ex
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8               call_type;
  uint8               sys_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3g_process_flow_enable_ex. subs_id:%d",
                  subs_id);

  for( sys_mode = 0; sys_mode < (uint8)SYS_SYS_MODE_MAX; sys_mode++ )
  {
    for( call_type = 0; call_type < (uint8)DS_NUM_CALL_TYPES; call_type++ )
    {
      if ((ds3gi_hdlr_tbl[sys_mode][call_type] != NULL) &&
          (ds3gi_hdlr_tbl[sys_mode][call_type]->coex_flow_enable_hdlr != NULL))
      {
        ds3gi_hdlr_tbl[sys_mode][call_type]->coex_flow_enable_hdlr( subs_id );
      } /* hdlr func not NULL */
    } /* call type loop */
  } /* sys mode loop */
  return;
} /* ds3g_process_flow_enable_ex() */

/*===========================================================================
FUNCTION      DS3G_GET_TOTAL_ULDL_WM_CNT_EX

DESCRIPTION   Mode specific get total UL & DL wm cnt handler is called.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint64 ds3g_get_total_uldl_wm_cnt_ex
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8                call_type;
  sys_sys_mode_e_type  mode;
  uint64              total_uldl_wm_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return total_uldl_wm_cnt;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3g_get_total_uldl_wm_cnt_ex. subs_id:%d",
                  subs_id);

  mode = ds3g_get_current_network_mode_ex(subs_id);

  if( mode <= SYS_SYS_MODE_NONE || mode >= SYS_SYS_MODE_MAX )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid System Mode %d", mode);
    return total_uldl_wm_cnt;
  }

  for( call_type = 0; call_type < (uint8)DS_NUM_CALL_TYPES; call_type++ )
  {
    if ((ds3gi_hdlr_tbl[mode][call_type] != NULL) &&
        (ds3gi_hdlr_tbl[mode][call_type]->get_total_uldl_wm_cnt_hdlr != NULL))
    {
      total_uldl_wm_cnt += 
        ds3gi_hdlr_tbl[mode][call_type]->get_total_uldl_wm_cnt_hdlr( subs_id );
    } /* hdlr func not NULL */
  } /* call type loop */

  return total_uldl_wm_cnt;
} /* ds3g_get_total_uldl_wm_cnt_ex() */

/*===========================================================================
FUNCTION      DS3G_PROCESS_BRINGDOWN_PHYS_LINK_EX

DESCRIPTION   Mode specific get total UL & DL wm cnt handler is called.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_bringdown_phys_link_ex
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8                call_type;
  sys_sys_mode_e_type  mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "ds3g_get_total_uldl_wm_cnt_ex. subs_id:%d",
                  subs_id);

  mode = ds3g_get_current_network_mode_ex(subs_id);

  if( mode <= SYS_SYS_MODE_NONE || mode >= SYS_SYS_MODE_MAX )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid System Mode %d", mode);
    return;
  }

  for( call_type = 0; call_type < (uint8)DS_NUM_CALL_TYPES; call_type++ )
  {
    if ((ds3gi_hdlr_tbl[mode][call_type] != NULL) &&
        (ds3gi_hdlr_tbl[mode][call_type]->bringdown_phys_link_hdlr != NULL))
    {
      ds3gi_hdlr_tbl[mode][call_type]->bringdown_phys_link_hdlr( subs_id );
    } /* hdlr func not NULL */
  } /* call type loop */

  return;
} /* ds3g_process_bringdown_phys_link_ex() */

/*===========================================================================
FUNCTION      ds3g_check_if_3gpp_roam_status_changed

DESCRIPTION   This functions checks whether 3gpp Roaming Status has changed

DEPENDENCIES  None

RETURN VALUE  TRUE, if there is a change. FALSE, Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_check_if_3gpp_roam_status_changed
(
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr,
  uint8                            *output_stack_id_p
)
{
  boolean                    post_roam_chg_cmd = FALSE;
  uint8                      stack_idx;
  /* ---------------------------------------------------------------------*/

  if( (ss_info_ptr == NULL) || (output_stack_id_p == NULL) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Null ss_info_ptr or output_stack_id_p");
    return FALSE;
  }

  for(stack_idx = 0; stack_idx < ss_info_ptr->number_of_stacks; stack_idx++)
  {
    if(stack_idx > DS3G_SS_STACK_MAX)
    {
      break;
    }

    if((ss_info_ptr->stack_info[stack_idx].is_operational == TRUE) && 
        DS_3GPP_IS_3GPP_CURRENT_MODE(ss_info_ptr->stack_info[stack_idx].sys_mode))
    {
      switch (ss_info_ptr->stack_info[stack_idx].roam_status)
      {
        /* Ignore if Roam Status is None, Unknown or MAX */
        case SYS_ROAM_STATUS_NONE:
        case SYS_ROAM_STATUS_UNKNOWN:
        case SYS_ROAM_STATUS_MAX:
        {
          post_roam_chg_cmd = FALSE;
          break;
        }

        default:
        {
          if (ds3gsubsmgr_is_subs_id_valid(ss_info_ptr->asubs_id) &&
              (ds3g_3gpp_roam_status[ss_info_ptr->asubs_id] != 
                ss_info_ptr->stack_info[stack_idx].roam_status))
          {
            DATA_3GMGR_MSG2(MSG_LEGACY_MED,"3GPP Roam status old: %d, new: %d", 
              ds3g_3gpp_roam_status[ss_info_ptr->asubs_id],
                ss_info_ptr->stack_info[stack_idx].roam_status);

            ds3g_3gpp_roam_status[ss_info_ptr->asubs_id] = 
              ss_info_ptr->stack_info[stack_idx].roam_status;   
            *output_stack_id_p = stack_idx;
            post_roam_chg_cmd = TRUE;
          }
          break;
        }
      } /* end switch */
    }
  }

  return post_roam_chg_cmd;
} /* ds3g_check_if_3gpp_roam_status_changed */

#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION      ds3g_check_if_3gpp2_roam_status_changed

DESCRIPTION   This functions checks whether 3gpp2 Roaming Status has changed

DEPENDENCIES  None

RETURN VALUE  TRUE, if there is a change. FALSE, Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_check_if_3gpp2_roam_status_changed
(
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr,
  uint8                             stack_idx
)
{
  boolean                           post_roam_chg_cmd = FALSE;
  /* ---------------------------------------------------------------------*/

  if(ss_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Null ss_info_ptr");
    return FALSE;
  }

  if(stack_idx > DS3G_SS_STACK_MAX)
  {
    return FALSE;
  }

  if((ss_info_ptr->stack_info[stack_idx].is_operational == TRUE) &&
      ds3gsubsmgr_is_subs_id_valid(ss_info_ptr->asubs_id))
  {
    switch (ss_info_ptr->stack_info[stack_idx].roam_status)
    {
      /* Ignore if Roam Status is None or MAX */
      case SYS_ROAM_STATUS_NONE: 
      case SYS_ROAM_STATUS_MAX:
      {
        post_roam_chg_cmd = FALSE;
        break;
      }

      default:
      {
        if ((ss_info_ptr->stack_info[stack_idx].sys_mode == SYS_SYS_MODE_HDR) &&
            (ds3g_hdr_roam_status[ss_info_ptr->asubs_id] != 
             ss_info_ptr->stack_info[stack_idx].roam_status))
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_MED,"HDR Roam status old: %d, new: %d", 
            ds3g_hdr_roam_status[ss_info_ptr->asubs_id],
              ss_info_ptr->stack_info[stack_idx].roam_status);

          ds3g_hdr_roam_status[ss_info_ptr->asubs_id] = 
            ss_info_ptr->stack_info[stack_idx].roam_status;   
          post_roam_chg_cmd = TRUE;
        }
           
        if ((ss_info_ptr->stack_info[stack_idx].sys_mode == SYS_SYS_MODE_CDMA) &&
            (ds3g_cdma_roam_status[ss_info_ptr->asubs_id] != 
             ss_info_ptr->stack_info[stack_idx].roam_status))
        {
          DATA_3GMGR_MSG2(MSG_LEGACY_MED,"CDMA Roam status old: %d, new: %d", 
            ds3g_cdma_roam_status[ss_info_ptr->asubs_id],
              ss_info_ptr->stack_info[stack_idx].roam_status);

          ds3g_cdma_roam_status[ss_info_ptr->asubs_id] = 
            ss_info_ptr->stack_info[stack_idx].roam_status;   
          post_roam_chg_cmd = TRUE;
        }
        break;
      }
    }/* end switch */
  }
  return post_roam_chg_cmd;
} /* ds3g_check_if_3gpp2_roam_status_changed */

#endif /* FEATURE_DATA_IS707 */
/*===========================================================================
FUNCTION      ds3g_process_pdn_conn_req_err_cmd

DESCRIPTION   This functions processes PDN Connection request error
               command from CM and does mode specific handling

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_pdn_conn_req_err_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;
  ds3g_hdlr_func_tbl_type  *hdlr_tbl_ptr = NULL;
  sys_sys_mode_e_type       cm_call_mode = SYS_SYS_MODE_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Rx'ed DS_CMD_CM_PDN_CONNECTIVITY_REQ_ERR");

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  cm_call_mode = ds3gi_get_call_mode(cmd_ptr);
  if (cm_call_mode == SYS_SYS_MODE_NO_SRV)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,
                    "ds3g_process_pdn_conn_req_err_cmd failed - CM says NO SRV mode");
  }
  else
  {
    hdlr_tbl_ptr = ds3gi_hdlr_tbl[SYS_SYS_MODE_LTE][DS_CALL_TYPE_PKT];

    if (hdlr_tbl_ptr == NULL)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "hdlr_table_ptr is Null");
      return;
    }
    
#ifdef FEATURE_DATA_LTE    
    if( hdlr_tbl_ptr->pdn_conn_req_err_hdlr != NULL )
    {
      /*---------------------------------------------------------------------
       Invoke the mode-specific handler function.
      ---------------------------------------------------------------------*/
      hdlr_tbl_ptr->pdn_conn_req_err_hdlr(cmd_ptr,
                                          call_info_cmd_ptr->subs_id);
    } /* if call_ended_handler != NULL */
    else
#endif /* FEATURE_DATA_LTE */    	
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "pdn_conn_req_err handler not registered");
    }

    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Data call ended for sdf_id 0x%x",
                    call_info_cmd_ptr->sdf_id);
  }
  return;
} /* ds3g_process_pdn_conn_req_err_cmd() */

