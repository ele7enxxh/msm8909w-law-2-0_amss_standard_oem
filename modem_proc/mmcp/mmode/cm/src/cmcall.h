#ifndef CMCALL_H
#define CMCALL_H
/*===========================================================================

         C A L L   M A N A G E R   C A L L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMCALL.C


Copyright (c) 1991 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmcall.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/14   xs      Reset ssac timer upon cell change
04/22/14   xs      hVOLTE SSAC support
03/13/14   xs      SSAC Rel12 requirement
06/12/13   xs      VT answered as voice
05/03/13   fj      Reject mode change request if there's PS data call and SMS in progress.
02/14/13   th      Change existing function to identify if call obj exists or not.
                   This is for EXT IMS purpose.
12/03/12   xs      Fix sRVCC issue for MT volte call
10/18/12   xs      Don't transfer call not in active state after SRVCC
09/26/12   skk     Changes of ESM SM backoff timer requirements
09/21/12   vk      FIX to unreserve connection id in CSFB mode.
09/18/12   qf      Reset SSAC backoff timer if G/W full service is acquired
08/28/12   ak      Increase the SR hold period for HDR sess_neg failure.
07/06/12   qf      FR1730 - service specif access control
05/14/12   ab      LTE call control specific changes CR 360560
04/15/12   ab      FR1681: Location Information support in Call Control envelope command
04/04/12   xs      Codec info propagation
03/27/12   xs      VoLTE Remove of user from Conf-call
03/08/12   xs      Added codec info support for VOLTE
02/13/12   xs      VT upgrade/downgrade support
02/07/12   xs      VOLTE silent redial support
01/24/12   vk      RLF handling during 1XCSFB call.
01/20/12   gm      Data synchronization for CM Call objects
01/17/12   aj/cl   SC SVLTE SO 33 suspend changes
01/06/12   gm      Data synchronization changes for freefloating platform
01/04/12   xs      Add new IMS call end error codes
11/30/11   ab      Support for alerting pattern IE in the MT CC SETUP
08/05/11   xs      VOLTE phase 1 support
07/26/11   jh      Marking GW connection IDs on LTE Activate bearer request
                   Adding additional logs for connection_ids
07/19/11   jh      Revert previous change: move 2 macros back to cmcall.c
06/30/11   fj      Move 2 macros from cmcall.c.
06/27/11   fj      For oprt_mode LPM request, don't end LTE calls.
06/15/11   rk      End the GW PS calls when mode is changed to LTE only and
                   PDP activate request is not yet send to NAS.
06/10/11   mj      Renamed existing & added new function to manage SM
                   pdp_context_trans_id
05/10/11   sv      In case of 1XCSFB call silent redial fix to hold the call
                   when SRV is not full service
05/03/11   sv      Add runtime check for JCDMA in orig plan
04/19/11   rk      Changing the declaration to extern for pdp_context_trans_id
03/22/11   rk      Support for handling negative scenarios
03/11/11   rm      GSDI clean-up
02/28/11   gm      Changes for thermal emergency feature
02/23/11   mh      Removed FEATURE_JCDMA_CM
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/07/11   xs      Remove FEATURE_T53 for hold answer
02/03/11   fj      Add cm_find_call_id_gwl().
12/23/10   sv      Integrate DSDS feature
12/17/10   gm      Changed user_data parameter to const type in CC callback.
12/10/10   rk      Adding new orig mode for CSFB call
12/10/10   rk      Sucessful MO call initial support
11/16/10   xs      Added con_ref notification support
09/22/10   rn      Modified parameter for cmcall_remove_mode_and_force().
07/22/10   fj      Remove duplicate fields in gsm_wcdma_cmcall_ps_type.
07/12/10   am      Added pack and unpack subaddress prototype.
06/29/10   gm      Moved e911 fields to VoIP SR datastructure.
06/21/10   fj      Add support for LTE call control.
05/26/10   jqi     EHRPD VOIP silent redial support.
05/17/10   fj      Added support for GW to LTE call transfer.
05/07/10   fj      Added cmcall_is_no_call_in_lte_ps().
04/16/10   fj      Added support for LTE to GW call object transfer.
02/24/10   aj      Add support for PS optimized silent redial
02/16/10   sg      CM to send handover complete info to CM client (eCall feature requirement)
01/19/10   aj      Added VOIP support for emergency call
08/31/09   fj      Added cmcall_event_2().
04/06/09   fj      Fix compile warning.
12/22/08   fj      Enabled some APIs for LTE.
                   Added lte_info in cmcall_info_s_type.
09/09/09   am      Adding is_acm_read_success and is_acm_max_read_success
                   in cm_ph_charge_data_type
08/19/09   ks      CM is not allowing three primary pdp call at a time
07/26/09   rn      Centralised call control changes
07/16/09   mh      Added cm_call_ptr_is_allocated()
05/27/09   aj      Adding support for passing otasp status to clients
03/09/09   rm      Made cmcall_misc_get_common_mode_pref non-static
02/25/09   sv      Correct Lint errors
02/12/09   mh      Added support for no SR after MAP for IS-707B
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
11/13/08   aj      Added API cmcall_end_each_call_with_type
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
10/15/08   rn      Corrected max line chars
08/05/08   aj      Added api cmcall_check_for_call_state_on_ss()
06/06/08   rn      Added cmcall_overall_ps_call_end()
06/06/08   sv      Removing unneccesary snd.h
05/28/08   ks      Updating the associated primary profile number when
                   secondary PDP is promoted to Primary
04/29/08   ks      Reverting the changes of Updating the associated primary
                   profile number when secondary PDP is promoted to Primary
03/31/08   ks      Updating the associated primary profile number
                   when secondary PDP is promoted to Primary
03/19/08   pk/vs   Corrected CMCALL_IS_HDR_ORIG_ALLOWED, lint suppression
03/05/08   vk      corrected code for HYBR_GW configuration
12/13/07   sk/ak   Added DCM.
11/26/07   sv      Removing Feature FEATURE_GW_CNAP_CODING_SCHEME
11/22/07   sg      Adding is_network_initiated field to gsm_wcdma_cmcall_type.
11/05/07   sg/vs   Renaming FEATURE_CM_TEMP_NAS_DEP_TBD To
                   FEATURE_GW_CNAP_CODING_SCHEME
09/10/07   rn      Added enc_alpha to struct cmcall_s_type to send
                   coding scheme along with string
06/22/07   sk      Added macros for easily looping through call objects.
06/21/07  pk/cl    Added support for power suspend
04/17/07   rp      Making cmcall_get_favored_srv_domain_pref() as extern
03/09/07   sk      RAM usage reduction.
02/02/07   pk      Featurizing cmcall_get_voip_call_id under FEATURE_IP_CALL
01/24/07   pk      Fixed compiler warning
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/21/06   ka      Adding rab_id_present field to call object.
11/16/06   pk      Lint Fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/03/06   ka      Adding support for IP call on WCDMA
09/14/06   ka      Adding support for call control on secondary ps calls
08/28/06   ka      Adding pdp header comp and data comp enums.
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
08/16/06   ka/rp   Adding aoc_ready to Phone charge struct.
08/11/06   pk      Added support for UOOS
08/11/06   ka      Modifying cmcall_check_if_gw_cs_call_active() to check
                   for calls in any state.
08/08/06   sk      Added CMCALL_ACT_FORCE_NON_IP.
                   Added cmcall_remove_mode_and_force().
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
07/07/06   pk      Removed cm_hold_cmd_handle and cmcall_add_cmd_to_hold_q
07/04/06   ka      Changes to support call control on PS calls.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
06/05/06   ka/ic   Externalizing cmcall functions.
04/14/06   pk      Added support for CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT
04/03/06   pk      If HDR starts throttling during a Silent Redial, give it
                   one more chance
                   Once the system is throttled, don't send call end
                   to clients immediately, buffer the call end for 1 second
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
01/13/06   ka      Bringing in CMCALL_CALL_ID_HOLD_FATAL_ERR_TIME.
12/09/05   ic      Lint cleanup
11/17/05   pk      Added cmcall_sms_status_e_type and sms_status in the call
                   object
11/08/05   sk      Separated CM reports and commands.
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
                   moved cm_ss_e_type to cmi.h
09/12/05   pk      Added cmcall_pd_status_e_type, and GPS session related
                   changes
09/10/05   pk      Added FEATURE_HYBR_HDR_REDIAL_WAIT_ACQ_FAIL to decide
                   CMCALL_SR_HYBR_MAX_HDR_DIALS
08/15/05   pk      Added support for priority Queues for origination
                   parameters
08/16/05   ka      Exporting cmcall_is_voice_over_hdr_poss()
08/12/05   ka      Functions to check voice/emerg in origination and decide
                   on call types.
07/30/05   ka      Removed declaration of cmcall_set_call_params()
                   Added is_ccbs_recall_resp field to gsm_wcdma_cmcall_type
07/29/05   dk      Added CMCALL_ACT_WAIT_OTHER_END action to wait for some
                   other activity to end during silent redial.
07/21/05   pk      Modified the printing of Silent redial count and the
                   loging of the redial event. Moved the Silent redial
                   related functions to cmcall.c
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
07/14/05   ic      Removed is_user_originated_call from cdma_cmcall_type as
                   it is aready present in cmcall_s_type
07/08/05   pk      Changed redial action table for pkt data calls
                   modified CMCALL_SR_HYBR_MAX_HDR_DIALS CMCALL_SR_MAX_DIALS
                   for JCDMA builds. These changes are part of the silent
                   redial enhancements
07/08/05   ic      Report highest layer call origination failure as part of
                   call events.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/21/05   dk      Added cmcall_is_active_on_mode() to find if there is any
                   call on a given mode.
06/09/05   ic      Removed all varieties of EJECT comment
                   Cleaned up cmcall_cmd_copy_fields()
06/06/05   pk      Added CMCALL_ACT_REDIAL_HDR_THR, redial action for call
                   throttle
05/19/05   ka      Merging CIPHER_IND feature.
05/04/05   jqi      Fixed for hybr data switch over.
03/14/05   ka      Adding ipapp_id and ipapp_data_block to call obj
03/07/05   ic      Fixed ARM ADS compiler warnings
03/02/05   ic      Removed code under FEATURE_6500_4X (obsolete)
02/24/05   dk      Added cmcall_count_on_ss() function to address CR 57350.
02/18/05   sj      Adding new redial action - REDIAL_SHORT_SAME_SR.
02/14/05   pk      Added fields to store the line control information
02/01/05   dk      Added WLAN Support.
12/19/04   ic      Added cmcall_info_get()
12/02/04   dk      Moved Call Substate Enums to cm.h
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP
10/21/04   ka      Adding CMCALL_PS_DATA_SR_MAX_TIME to increase silent
                   redial time for ps calls and incr MAX_DIALS to 100
09/14/04   ic      Call direction clean up - removed direction field from
                   gsm_wcdma_cmcall_type
09/07/04   ka      Removed mode_in_use, is_orig_sent and added orig_sent_on_mode
08/30/04   sj      Added orig_mode in call object.
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/19/04   dk      Added call_ptr as parameter to
                   cmcall_determine_ss_for_origination().
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/04/04   dk      Added support to hold Packet Data Call origination in HDR
                   if SMS/DBM is in progress.
07/28/04   ka      Added support for secondary PDP context
07/21/04   sj      Added support for umts to 1x handover.
07/12/04   ic      Added support for FRATURE_ALS
07/06/04   dk      Modified signature of cm_find_connection_id().
07/06/04   ws      Add field to call object to indicate waiting for hdr call end.
07/05/04   dk      Changed signature of cmcall_update_gsdi_and_log_call_state().
07/01/04   dk      Added Call Object Iterator.
06/30/04   ic      Fixed obvious RVCT 2.1 compiler warnings (Jaguar build)
06/25/04   dk      Modified call object so that call_obj_array just contains
                   the pointer to the call object. Moved all call relevent
                   information inside the call object.
                   Removed explicit call object allocation/de-allocation.
                   Now it is done as part of call_id allocate/deallocate.
                   Modified in general to support this new behavior.
                   Moved certain generic call obj array walk through functions
                   from cmxcall.c
06/22/04   ic      Comments clean up
06/16/04   ic      Restored cmcall_event() to take const pointer to call object.
                   Added prototype of cmcall_update_call_end_info()
06/15/04   dk      Removed overall_state from cm_overall_calls_s_type. Removed
                   cmcall_update_overall_state() and added
                   cmcall_update_gsdi_and_log_call_state().
06/10/04   dk      Added cmcall_check_each_call_in_call_state function.
06/03/04   ws      Add support for redirecting number.
06/02/04   ka      Added cmcall_check_call_type_in_call_state ()
06/01/04   npr     Added IP Header and Data compression requirements to GW PS
                   call origination and information structures for GPRS SNDCP.
05/27/04   ic      Fixed featurization errors so it compiles without
                   FEATURE_HDR defined.
05/20/04   ic      Put CMCALL_SR_MAX_DIALS = 4 under FEATURE_6500_4X
05/19/04   ic      Put #ifdef around CMCALL_SR_MAX_DIALS definition
04/08/04   ws      Merge from MSMSHARED_CM.01.00.32.00.03
04/01/04   ws      Added cmcall_ptr_callobject_allocate function.
03/31/04   ic      Mainlined FEATURE_REX_DYNA_MEM in CM and unit test.
                   Removed all code related to static allocation of call
                   objects.
03/26/04   ka      Added a new function for processing CMCALL_ACT_PROC_END_REASON
03/15/04   ka      Merge changes from 6250 branch
                   Reject PS data call origination when MB is in GSM voice call.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/06/04   ws      Initial jaguar/mainline merge.
01/29/04   ws      Added function for handling gsdi events in both modes.
01/19/04   ka      Call id and profile no support in packet call structure. New
                   fields to account for call conenct time, call end time and data traffic
                   in cmcall_s_type. Headers for call info list and data session reg.
01/13/04   ka      Merged changes from MSM6200
01/09/03   ws      Take a slot number when sending gstk voice envelopes.
12/11/03   ic      Replaced is_hybr_redialled with is_hybr_redialed
12/08/03   ws      Mainlined umts concurrency services feature.
11/14/03   ws      Added support for is_jcdma_emergency
10/20/03   ws      Dual Slot Support.
10/01/03   ws      Merged from GW Targets
09/23/03   ws      Merged fix for SIM fails during emergency call
                   (do not end the call).
08/26/03   ws      Removed RelA featurization for flash type.
08/21/03   ws      Added changes for automatic mode and silent redial merge.
08/13/03   prk     Added minimum_qos field to gsm_wcdma_cmcall_ps_type.
07/08/03   vt      Added sys mode to call object structure.
04/29/03   sun     Added new cc_reject in gsm_wcdma_cmcall_type
04/11/03   prk     Removed references to FEATURE_SUPS and FEATURE_AOC.
02/27/02   cs      Added MT/MO indication and transaction ID in CS GSM/WCDMA
02/25/03   RI      Added cmcall_check_if_cs_voice_calls_active() and
                   cmcall_check_if_ps_call_active() prototypes.
02/24/03   vt      Modified cm_ss enum type to use SD enums.
02/18/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/03/03   ri      Added support for concurrent services.
           cs      Updated silent redial for WCDMA/GSM.
01/21/03   ws      Updated copyright information for 2003
12/17/02   vt      Defined fixed numbers to an enums for event logging.
                   Added system selection structure.
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Merged replaced is_orig_fail_l2 with orig_fail_layer in sr ^M
                   structure.
                   [CS]Added cmcall_obj_s_type definition.
                   Added support for UMTS silent redial for emergency call for GSM/WCDMA.
                   [prk]Added send_call_event_end field to cmcall_s_type.
                   [RI]Added ss_info field in gsm_wcdma_cmcall_type.
10/28/02   vt      Removed handling of NDSS origination failure.
10/22/02   ic      Under FEATURE_JCDMA_CM set CMCALL_SR_MAX_TIME to 40 sec.
10/22/02   vt      Created a function to return call_id from call object ptr.
                   Removed is_allocated and call id from call object for
                   dynamic memory allocation.
10/22/02   vt      Removed cmd check function that is not used.
10/09/02   vt      Added mode param to cmcall_callobject_allocate().
09/30/02   RI      Added ss_info field in gsm_wcdma_cmcall_type.
09/11/02   RI      Added redirect_party_number field in gsm_wcdma_cmcall_type.
08/30/02   vt      Added calling number to call object.
08/12/02   RI      Added ect_info field in gsm_wcdma_cmcall_type.
08/02/02   ATM     Fixed enum so it matches for logging no matter what FEATURE
                   is on/off.
07/29/02   RI      Added cause_of_no_cli to gsm_wcdma_cmcall_type.
07/10/02   PRK     Removed prototype for cm_call_id_allocate and
                   cm_call_id_deallocate.  They are now defined in cmll.h.
06/14/02   AT      Removed state added for silent redial.
06/11/02   RI      Added cmcall_object_deallocate() to deallocate a
                   call object.
                   Renamed cm_new_call_id() to cm_new_connection_id().
                   Deleted call object pointer parameter in
                   cmcall_client_cmd_check().
05/05/02   vt      Fixed compile problem with new mm cm.h
04/09/02   sh      Added is_hdr_not_avail and removed cmcall_is_in_hdr_call
03/13/02   sh      Renamed and modified some HDR related macros
03/10/02   sh      Added macros for checking HDR and NonHDR call orig/call status
02/06/02   sh      Added hybrid 3.0 support
01/17/02   ic      Added to the comment below a note about the fix for CR#17975
01/12/02   az      Added 2 variables that keep track of Roam preference(for SD2.0)
                   and Roam indicator(for SD1.0) when the call was originated.
                   Fixed CR#17975
12/08/01   sh      Added preliminary hybrid operation support
05/05/02   vt      Fixed compile problem with new mm cm.h
02/06/02   PRK     Added function cmcall_overall_call_state.
02/04/02   RI      Added new states in overall_call_state_e_type
02/13/02   ic      Changed the type of call_id variable from byte/uint8 etc
                   to cm_call_id_type as it should have been to begin with.
01/29/02   ic      Removed the restriction of one voice + one data call
                   (under FEATURE_SVD_CM_CP_INTEGRATION)
01/23/02   ic      Fixes related to running with FEATURE_GPSONE_* enabled
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
01/11/02   ic      Made is_voice() function global (i.e. independed of any
                   feature)
01/03/02   ic      Added ACCT (Access Control based on Call Type) support
                   Added silent redial support for
                   CM_CALL_END_NO_RESPONSE_FROM_BS call end reason.
12/20/01   ic      Under FEATURE_SVD_CM_CP_INTEGRATION, added support
                   for restricting the CM to one voice and one data call
                   at a time as this is what CP code currently supports.
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/30/01   sh      Added access network authentication support
11/17/01   sh      Added 1xhdr data session handoff support
11/01/01   sh      Fixed various bug/deficiencies found from code review
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
08/21/01   ic      Main-lined FEATURE_NEWCM
05/25/01   CK      Changed FEATURE_RUIM to FEATURE_UIM_RUIM
04/13/01   SH      Added Preliminary HDR support
03/08/01   RC      Added CMCALL_ACT_REDIAL_SHORT to cmcall_act_e_type.
03/06/01   HQ      Added support for FEATURE_IS2000_REL_A.
02/07/01   RC      Added support for System Determination 2.0.
01/23/01   ych     Merged T53 and JCDMA features.
12/28/00   fc      Added support for retry order.
12/15/00   HQ      Added privacy_pref field.
10/18/00   HQ      Added ruim_last_poll_time in cmcall_s_type.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
08/23/00   vt      Added new CM interface changes.
07/18/00   JQ      Added support for end_reason
03/20/00   RY      Removed cm_flush_hold_cmd_q()
03/13/00   RY      Added OTAPA support
12/13/99   IT      Added handling for next data call as data.
12/03/99   VT      Merged TriMode Code.
11/04/99   SH      CR11880 - Fixed a problem with NDSS.
10/19/99   SH      Fixed CR11844 - to NDSS connect or not to UI
09/01/99   SH      Featurized OTASP
08/31/99   SH      Added cmcall_end
08/25/99   SH      Added cmcall_offline
08/19/98   SH      Increased act_tbl size
08/09/99   SH      Added alert answer bypass feature
07/07/99   SH      Added CNAP feature
06/30/99   SH      Changed type of otasp_act_code to cm_activate_code_type
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmmmgsdi.h"
#include "lte_rrc_ext_msg.h"
                       /* Definition of LTE RRC SRV indication */

#if defined(FEATURE_CM_LTE)
#include "cm_esm.h"
#endif

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cmxll.h"      /* MC to CM interface*/
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#ifdef CM_GW_SUPPORTED
#include "cmss.h"
#endif
#include "sys_v.h"

#include <stdarg.h>
#include "gstk_exp.h"

#define CALL_MODE_INFO_PTR( xx_call_ptr ) ( &xx_call_ptr->cmcall_mode_info.info.cdma_info )

#define CMCALL_GREP_ALL  1
#define CMCALL_GREP_STOP (end_grep = TRUE)

#define CMCALL_FOR_EACH_CALL_DO                           \
/*lint -e{717} */                                         \
do                                                        \
{                                                         \
  cmcall_s_type    *this_call;                            \
  cm_iterator_type  grep_call_itr;                        \
  boolean           end_grep = FALSE;                     \
                                                          \
  cmcall_obj_iterator_init(&grep_call_itr);               \
                                                          \
  this_call = cmcall_obj_get_next(&grep_call_itr);        \
                                                          \
  while(this_call != NULL && !end_grep)                   \
  {


#define CMCALL_END_FOR                                    \
    /*lint -e{725} Ignore indentation */                \
    this_call = cmcall_obj_get_next(&grep_call_itr);      \
  }                                                       \
}while(0);

/* This macro executes a set of statements for each call object when the
** condition 'cond' is true. The call object can be referenced as 'each_call'.
*/
#define CMCALL_GREP_AND_DO(cond,statements)                       \
          CMCALL_FOR_EACH_CALL_DO                                 \
            if(cond)                                              \
            {                                                     \
              statements;                                         \
            }                                                     \
          CMCALL_END_FOR


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define CM_DUMMY_CALL_ID  0xFF

/* unknown connection ID */
#define CM_UNKNOWN_CONN_ID 0xFF

/* profile number used for ps calls */
#define CMCALL_PROFILE_NUM_INVALID 0xFF

//#define CMCALL_MAX_CALL_OBJ      CM_CALL_ID_MAX

/* This time is the wait time for CM when a 1x voice origination happens
   when an HDR call is in progress.  CM should end the HDR call, wait 2 seconds
   for everything to clean up before sending a origination down to MC.
*/
#define CMCALL_HDR_WAIT_ORIG_UPTIME           2   /* seconds */

#define CMCALL_UPTIME_NONE   (dword)(0)

#define CMCALL_CALL_ID_HOLD_FATAL_ERR_TIME        5    /* seconds */
    /* Max time to hold from error fataling if CM can no longer allocate
    ** anymore call ids.
    */

/* Reserved value; to indicate timer not running.
*/
#define CMCALL_SSAC_UPTIME_NONE             (dword)(-1)
#define CMCALL_SSAC_MAX_BACKOFF_TIME        512

#if defined FEATURE_MMODE_TRIPLE_SIM
#define MAX_SIMS                                       3 /* No.of sims*/
#define MAX_SUB_SESSION_TYPES               4
#elif defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_DUAL_SIM)
#define MAX_SIMS                                       2 /* No.of sims*/
#define MAX_SUB_SESSION_TYPES               3
#else
#define MAX_SIMS                                       1 /* No.of sims*/
#define MAX_SUB_SESSION_TYPES               2
#endif


/*---------------------------------------------------------------------------
                     TUNABLE SILENT REDIAL PARAMETERS
---------------------------------------------------------------------------*/


/* Silent redial feature to silently redial failed calls or to ride through
** limited periods of no service.
**
** In addition, silent redial can be configured to forced the mode
** preference over to analog if such service available and if calls fail
** under poor CDMA RSSI conditions. This force-over to analog is temporary,
** ending at call teardown.
**
** NOTE! generally the following relation should be maintained
** for the silent redial parameters (assuming MAX_DIALS >= 1):
**
** ((MAX_DIALS-1) * PERIOD_TIME) <= MAX_TIME
*/

#define CMCALL_SR_MAX_DIALS                  100
    /* Max number of dials per explicit origination attempt.
    **
    ** NOTE! This number includes the first explicit dial attempt
    ** and therefore should always be > 0.
    ** Changing it to large value since silent redial
    ** can happen for 75 secs on packet side
    */


#define CMCALL_SR_PERIOD_TIME                4  /* seconds */
    /* Time from origination failure to next silent redial attempt */

#define CMCALL_SR_HDR_SESSION_NEGO           4 /* seconds */
    /* Time from origination failure to next silent redial attempt when HDRCP
    ** is still performing session negotiation.
    */

#define CMCALL_SR_MAX_TIME                   30  /* seconds */
    /* Maximum silent redial sequence time per explicit origination attempt.
    **
    ** Note that this time designate a limit for the last silent redial
    ** attempt to be launched. This last origination attempt might fail or
    ** be connected after this max time is expired */

#define CMCALL_SR_EMERGENCY_MAX_TIME_MSIM      30  /* seconds */
    /* Maximum silent redial sequence time per explicit origination attempt.*/

#define CMCALL_PS_DATA_SR_MAX_TIME           75  /* seconds */
    /* GPRS attach procedure is governed by T3310 and T3311 timers.
    ** At the expiry of T3310 (15secs) after a PS attach request is
    ** sent 4 more retries are made at an interval of 15 secs. In
    ** total when PS attach fails after 75 secs T3311 is started.
    ** Comparitive study finds PS attach should be tried for atleast
    ** 75 secs.
    **/


#define CMCALL_SR_FORCE_AMPS_RSSI_THR        100  /* -dB */
    /* During silent redial origination force over to AMPS service
    ** if CDMA RSSI is below this threshold */

#define CMCALL_ORIG_THR_HOLD_UPTIME          1 /* Second */
    /* Time to hold the call end to clients, this is used to flow control
    ** the clients so that they don't queue too many call originations to CM
    */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Max number of HDR origination attempts allowed in hybrid mode.
**
** NOTE! This number includes the first explicit dial attempt
** and therefore should always be > 0.
*/

#if ( defined (FEATURE_JCDMA) || defined (FEATURE_HYBR_HDR_REDIAL_WAIT_ACQ_FAIL) )
#define CMCALL_SR_HYBR_MAX_HDR_DIALS CMCALL_SR_MAX_DIALS
#else
#define CMCALL_SR_HYBR_MAX_HDR_DIALS         2
#endif


#ifdef FEATURE_EXTENDED_TI

#define CM_MAX_PDP_NUM        255
#define CM_MIN_MO_TI            0
#define CM_MAX_MO_TI          127
#define CM_MIN_MT_TI          128
#define CM_MAX_MT_TI          254

#else

#define CM_MAX_PDP_NUM         16 // Still, TIs are available only through 0~6 and 8~14
#define CM_MIN_MO_TI            0
#define CM_MAX_MO_TI            6
#define CM_MIN_MT_TI            8
#define CM_MAX_MT_TI           14

#endif

#define UNKNOWN_CONNECTION_ID          0x07  /* UNKNOWN mobile originated connection ID */
#define MAX_MO_CONNECTION_ID           0x06
#define MO                             0x00

/* Enumeration of call origination action.
**
** When call origination fails (due to no-service, fade, reorder, intercept
** release, etc.) the call origination specifies which one of the following
** action should be invoked.
*/
/* For compatibility with QXDM database, please do not change the following
** fix assigned values. If new values are added please also update the QXDM
** database
*/
typedef enum cmcall_act_e {

    CMCALL_ORIG_ACT_NONE                   = 0,  /* No action should be taken */
    CMCALL_ACT_END_ORIG                    = 1,  /* End the origination process */
    CMCALL_ACT_FORCE_AMPS                  = 2,  /* Force AMPS */
    CMCALL_ACT_FORCE_AMPS_RSSI             = 3,  /* Force AMPS if RSSI is low */
    CMCALL_ACT_REDIAL                      = 4,  /* Schedule next SR */
    CMCALL_ACT_FORCE_AUTO                  = 5,  /* Optimized SR */
    CMCALL_ACT_FORCE_AUTO_RSSI             = 6,  /* Optimized SR, based on RSSI*/
    CMCALL_ACT_REDIAL_SHORT                = 7,  /* Schedule next SR, shorten
                                                 ** if OK to orig
                                                 */
    CMCALL_ACT_WAIT_OK_TO_ORIG             = 8,  /* Wait for ok-to-orig
                                                 ** indication (SD 2.0)
                                                 */
    CMCALL_ACT_HYBR_REDIAL                 = 9,  /* Hybrid redial on either
                                                 ** CDMA or HDR
                                                 */
    CMCALL_ACT_PROC_END_REASON             = 10, /* Process END reason for WCDMA/GSM */
    CMCALL_ACT_FORCE_REDIAL                = 11, /* redial without checking for SRLeft */
    CMCALL_ACT_SHORT_SAME_SR_AUTO          = 13, /* Optimized Short redial without checking
                                                 ** for SRLeft no, but not
                                                 ** decrmenting SRLeft.
                                                 */
    CMCALL_ACT_REDIAL_HDR_THR              = 14, /* Redial on CDMA, due to
                                                 ** throttling on HDR
                                                 */

    CMCALL_ACT_WAIT_OTHER_END              = 15,  /* Wait for other activity to end */


    CMCALL_ACT_WAIT_OK_TO_ORIG_REDIAL      = 16, /* Wait for ok-to-orig
                                                 ** indication (SD 2.0)
                                                 ** the wait time is 4sec
                                                 */

    CMCALL_ACT_FORCE_NON_IP                = 17, /* Force to Non-IP (1x/GW)*/

    CMCALL_ACT_FORCE_TO_IP                 = 18, /* Force to IP */

    CMCALL_ACT_FORCE_TO_CDMA               = 19, /* Force CSFB call to Legacy CDMA */

    CMCALL_ACT_HARD_FAILURE                = 20, /* continue scanning E911 list for leftover LTE/CS systems */

    CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT       = 21, /* Wait for ok_to_orig for 2 seconds. If NO ok_to_orig
                                                 ** is received within 2 secs, Redial the call */

    CMCALL_ORIG_ACT_MAX

} cmcall_act_e_type;


/* Enum of PD status
*/

typedef enum
{

  CMCALL_PD_STATUS_NONE = -1, /* FOR INTERNAL USE OF CM ONLY! */

  CMCALL_PD_STATUS_HOLD_ORIG, /* Hold the origination, GPS needs to clean up*/

  CMCALL_PD_STATUS_END_ORIG,  /* End the origination */

  CMCALL_PD_STATUS_MAX        /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
   , CMCALL_PD_STATUS_BIG = 0x10000000 /* To force enum to 32 bit for windows NT */
  #endif
}cmcall_pd_status_e_type;


/* Enumeration of SMS status for the call.
*/

typedef enum
{

  CMCALL_SMS_STATUS_NONE = -1, /* FOR INTERNAL USE OF CM ONLY! */

  CMCALL_SMS_STATUS_HOLD_ORIG, /* Hold the origination, SMS is waiting for confirmation */

  CMCALL_SMS_STATUS_MAX        /* FOR INTERNAL USE OF CM ONLY! */

  #ifdef FEATURE_RPC
   , CMCALL_SMS_STATUS_BIG = 0x10000000 /* To force enum to 32 bit for windows NT */
  #endif
}cmcall_sms_status_e_type;


/* Enumeration of Silent redial count actions.
*/
typedef enum {

  CM_SR_ACT_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */

  CM_SR_ACT_RESTART,    /* Restart silent redial left count */
  CM_SR_ACT_DEC,        /* Decrement silent redial left count */
  CM_SR_ACT_NO_CHANGE,  /* don't change silent redial left */

  CM_SR_ACT_MAX         /* FOR INTERNAL USE OF CM ONLY! */

} cm_sr_act_e_type;

/* Enumeration of VOIP redial states (used in silent redial
** over voip)
*/
typedef enum cmcall_voip_redial_e {

  CMCALL_VOIP_REDIAL_NONE = -1,    /* FOR INTERNAL USE OF CM ONLY! */
  CMCALL_VOIP_REDIAL_PENDING,    /* Attempt to connect the call over VOIP */
  CMCALL_VOIP_REDIAL_COMPLETE ,  /* All VOIP redial attempts complete */
  CMCALL_VOIP_REDIAL_MAX       /* FOR INTERNAL USE OF CM ONLY! */

} cmcall_voip_redial_e_type;

typedef enum cm_emerg_call_attempt_state_e {
  CMCALL_ATTEMPT_STATE_NONE = -1,
  /* Internal Use Only */
  CMCALL_ATTEMPT_STATE_WWAN_ONLY,
  /* state when call cannot fallback on WLAN 
  ** only WWAN attempts can be made */
  CMCALL_ATTEMPT_STATE_WWAN_WLAN,
  /* state when call can fallback on WLAN 
  ** WWAN scans are going on for E911 call*/
  CMCALL_ATTEMPT_STATE_WLAN_ECBM,
  /* When UE is in WLAN ECBM and Emerg call is originated 
  **First attempt will be over WLAN, and on failure of WLAN attempt
  ** call may go to WWAN_ONLY or WWAN_WLAN depending upon 
  ** UE service status 
  */
  CMCALL_ATTEMPT_STATE_MAX
} cm_emerg_call_attempt_state_e_type;

/* Enumeration of various call debug info
*/
typedef enum cmcall_debug_info_e {

  CMCALL_DEBUG_INFO_NONE = -1, /* for internal use */

  CMCALL_DEBUG_INFO_ACT_BEARER_CALL_ID_FOUND,
   /* check if call has an existing bearer id or not
   */

  CMCALL_DEBUG_INFO_CALL_END_EVENT_SENT,
  /* check if the debug events in cm_qsh_event_e have been sent
  */

  CMCALL_DEBUG_INFO_ABNORMAL_FAILURE,
  /* Set if the call has experienced an abnormal failure
  */

  CMCALL_DEBUG_INFO_NORMAL_CALL_END,
  /* Set if the call is ended normally but experienced a failure while 
  ** processing the call end. The call end status may reflect an abnormal
  ** cause code such as LL_CAUSE but the call itself would have been
  ** successful.
  */

  CMCALL_DEBUG_INFO_MAX       /* for internal use */

} cmcall_debug_info_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Structure to store the custom Silent Redial
** timer info 
*/ 
typedef struct cm_voice_call_sr_timer_s 
{
  boolean          custom_sr_timer_enabled;
         /* Boolean flag tells if custom sr timer
         ** is enabled */

  dword            sr_max_time;
         /* SR timer tells the maximum duration for
         ** which silent redial will be attempted. 
         ** This value is currently applicable only 
         ** for voice calls 
         */ 
} cm_voice_call_sr_timer_s_type;

/* Type for call origination plan.
** Holds all the information that is associated with a particular
** origination plan.
*/

typedef struct cmcall_action_table_entry_s {

    /* orig fail status */               /* orig action to take */
    cm_call_end_e_type end_status;
            /* Origination end status - no-service, fade, reorder, etc */

    cmcall_act_e_type  action;
            /* Action to take - redial, force-AMPS, etc */

} cmcall_action_table_entry_s_type;

typedef struct cmcall_orig_plan_action_table_s {

   unsigned int                        action_table_entry_count;

   cmcall_action_table_entry_s_type   *action_table_entry;

} cmcall_orig_plan_action_table_s_type;

typedef struct cmcall_orig_plan_s {
    /*                                    */
    /* condition for the origination plan */
    /*                                    */
    const cm_feature_e_type    sr_with_jcdma;
        /* Is this origination plan is for when SR with JCDMA feature
        ** is turned ON, OFF, or don't care (NONE=don't care) */

    const cm_call_type_e_type  call_type;
        /* What call type is this origination plan for (NONE=don't care) */

    const byte                 *dial_str;
        /* What dial string is this origination plan for (NULL=don't care) */

    const boolean              is_hybr_allowed;
        /* whether or not packet data call is allowed to originate on
           either HDR or CDMA when the hybrid operation is enabled */

    const cm_call_mode_info_e_type call_mode_info;
        /* Used to differentiate betweeb mode info, required to GW_PS calls
        */

    boolean              is_emerg_on_other_sub;
        
    /*                                    */
    /*    Origination plan                */
    /*                                    */


    const boolean              is_honor_mc_hold_orig;
        /* Indicate whether we should honor MC hold-orig during the
        ** origination process */

    const dword                pwrup_hold_orig_time;
        /* Power-up hold orig uptime - i.e. at power-up how long we should
        ** hold the origination, giving service chance to appear */

    const dword                sr_period_time;
        /* Silent redial off period - i.e. time from origination failure
        ** to next redial attempt */

    const unsigned int         sr_max_dial;
        /* Max numbers of dial attempts */

    dword                sr_max_time;
        /* Max time for silent redial sequence */


    /*                                                                */
    /* Action table indicating action to take when call origination   */
    /* fails due to no-service, fade, reorder, intercept, release     */
    /* or calls being selectively blocked by the base station         */
    /*                                                                */
    const cmcall_orig_plan_action_table_s_type *action_table;

} cmcall_orig_plan_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef enum cmcall_voip_sr_state_e {

  CMCALL_VOIP_SR_STATE_NONE,

  CMCALL_VOIP_SR_STATE_IP_RETRY,

  CMCALL_VOIP_SR_STATE_FALLBACK_CS,

  CMCALL_VOIP_SR_STATE_IP_END,

  CMCALL_VOIP_SR_STATE_MAX

} cmcall_voip_sr_state_e_type;

/* VOIP for none E911 voice Silent Redial data structure
*/
typedef struct cmcall_voip_sr_voice_s {

  dword                         cs_fallback_uptime;
      /* Time period to use to set up the voip_to_cs_fallback_uptime timer.
      ** This timer is maintained in the call object. When a voice call is
      ** originated and voip is available , this timer is started. When the
      ** timer expires and there is no data call connection, CM will abort
      ** the origination over the call is  voip and fall back to CS network.
      ** Initialized to CM_DEFAULT_VOIP_FALLBACK_TO_CS_TIME
      */

  boolean                       is_abort;
      /* Indicate if sending abort IP call.
      */

} cmcall_voip_sr_voice_s_type;

/* Enums for PS call failures
*/
typedef enum cmcall_ps_failure_e {

  CMCALL_PS_FAILURE_HDR_CONNECTION_FAILURE,

  CMCALL_PS_FAILURE_ORIG,

  CMCALL_PS_FAILURE_MAX

} cmcall_ps_failure_e_type;


/* VOIP Silent Redial data structure
*/
typedef struct cmcall_voip_sr_s {

  boolean                        is_valid;
  /* Indicate if the content is valid.
  */

  cmcall_voip_sr_state_e_type    state;
  /* Indicate the state of the VOIP SR.
  */

  /* The call type dicatates the element of union.
  */
  union {

     cmcall_voip_sr_voice_s_type voice;

  } type;

  cmcall_ps_failure_e_type  ps_failure;

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

  /* The pscall failure dicatates the element of union.
  */
  union {

    cm_hdr_connection_deny_e_type conn_deny;
    /* HDR connection deny failures.
    */
    cm_call_orig_fail_e_type      ps_orig;
    /* PS call origination failure.
    */

  } failure;

  #endif

  dword                          e911_voip_fallback_uptime;
  /* also referred to as 1x completion timer. It indicates when
  ** the call attempt over 1x should be stopped and  the SR attempted
  ** over voip for a 911 call
  */
  cmcall_voip_redial_e_type     e911_voip_redial;
  /* This is used to decide whether to originate an e911 call over VOIP
  */

  byte                          lte_soft_failure_count;
  /* counter to keep track of how many soft error happends
  ** during the VOLTE call setup
  */
  boolean                       is_lte_hard_failure;

} cmcall_voip_sr_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Silent redial type - holds all the variables that are
** associated with the silent redial mechanism.
*/
typedef struct cm_sr_s {

    const cmcall_orig_plan_s_type  *orig_plan_ptr;
        /* Pointer to call origination plan
        */

    dword                          sr_start_uptime;
        /* Silent redial sequence start uptime
        */

    dword                          sr_max_uptime;
        /* Silent redial sequence max uptime
        */

    boolean                        is_ok_to_orig;
        /* Indicate whether CM_OK_TO_ORIG_F was received
        */

    boolean                        hdr_is_ok_to_orig;
        /* Indicate whether CM_OK_TO_ORIG_F was received
        ** for the HDR system when the hybrid operation
        ** is enabled
        */
    boolean                        is_hdr_not_avail;
        /*  Indicate hdr can not be used for
        **  next silent redial because there is no
        **  HDR system
        */

    sys_sys_mode_e_type            orig_sent_on_mode;
        /* Records the system mode on which origination
        ** gets sent down. If origination is rejected
        ** and redial needs to be performed it is reset
        ** to None
        */

    dword                          hold_orig_uptime;
        /* Hold origination until this uptime
        */

    dword                          next_orig_uptime;
        /* Try the next origination at this uptime
        */

    dword                          sr_left;
        /* Indicate how many silent redial left
        */

    #if ( defined(FEATURE_MMODE_CDMA_800) && defined(FEATURE_MMODE_CDMA_1900) )
    boolean                        sr_tried;
        /* Indicates whether silent redial was tried even once
        */
    #endif

    #ifdef FEATURE_IS2000_REL_A
    boolean                        is_re_orig;
        /* whether the current origination is a re-origination
        */

    byte                           re_orig_count;
        /* number of re-originations due to layer 2 failure
        */

    boolean                        re_orig_reason;
        /* indicates if re-origination is due to layer 2 failure
        */

    cm_call_orig_fail_layer_e_type orig_fail_layer;
        /* Layer where the origination failed
        */
    #endif /* FEATURE_IS2000_REL_A */

    boolean                        is_hybr_redialed;
        /* Indicates if the call was hybrid redialled, i.e attempted
        ** first on hybrid HDR and then redialled on 1x.
        */

    #ifdef FEATURE_CALL_THROTTLE
    boolean                        is_call_throttled;
        /* indicates if the call is throttled
        */
    #endif

    cmcall_voip_sr_s_type         voip_sr;
       /* data for VOIP silent redial.
       */
    #if defined (FEATURE_LTE_TO_1X)
    byte                              esr_retry_counter;
              /* counter to keep track of how many soft error happends
              ** during the CSFB call setup
              */
    #endif

    #ifdef FEATURE_IP_CALL
    dword                       fall_back_to_wifi_from_cs;
        /* Fall back to Wifi from CS if ok_to_orig is not received in this time
           Time to wait is 5sec for TMO.(NV configurable)
        */
    boolean         ignore_sr_max_uptime;
    /* In case of VoLTE Call, this variable decides whether SR max time would be used to end the call or not
       Basically, for the case of CSFB, IMS requirement is to not being SR Max time in picture*/
    dword                       wlan_fallback_timer;
    /* After this timer expiry if no WWAN system was found, then attempt 
        call over WLAN, if WLAN was available */
    cm_emerg_call_attempt_state_e_type emerg_call_attempt_state;
    /* signifies the state for emergency call attempts , which RATs emerg call can be attempted upon*/
    #endif

	uint8                       e911_cs_retry_counter;
    /* Specifies the Maximum attempt to be made on CS for e911 call
	   before fallback to PS. Set to CMPH_MAX_CS_REDIAL_COUNT_FOR_EMERG
		  if cs_favoured_retry_counter is not set from NV.
    */

} cm_sr_s_type;


/* Handover redial type - holds all the variables that are
** associated with the Handover redial mechanism.
*/
typedef struct
{


  dword                          hr_start_uptime;
      /* Handover redial sequence start uptime */

  dword                          hr_max_uptime;
      /* handover redial sequence max uptime */


  boolean                        is_ho_req_sent;
     /* Indicate whether Handover request command was sent to MC */


} cm_handover_retry_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Forward declarations to pacify the compiler.
*/
typedef struct cmph_s   *cmph_p_type;     /*lint -esym( 761, cmph_s_type ) */
typedef struct cmcall_s *cmcall_p_type; /*lint -esym( 761, cmcall_s_type ) */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* OTASP call types
*/
typedef enum cmcall_otasp_call_e
{
    CMCALL_OTASP_NONE          /* Regular call other than OTASP */
  , CMCALL_OTASP               /* User initiated service provisioning */
  , CMCALL_PC_OTAPA            /* Network initiated service provisioning
                               ** by paging the mobile station with SO 18/19
                               */
  , CMCALL_BUSY_OTAPA          /* Network initiated service provisioning
                               ** while the mobile station is already in
                               ** CDMA conversation state
                               */
} cmcall_otasp_call_type;


/* Type for OTASP/OTAPA Information.
*/
typedef struct cmcall_otasp_info_s {

    cmcall_otasp_call_type     otasp_call;
        /* What type of OTASP/OTAPA call */

    cm_activate_code_e_type    otasp_act_code;
        /* OTASP activation code (for OTASP calls) */

    cm_otasp_status_e_type     otasp_status;
        /* Indicate status of OTASP/OTAPA call */

    boolean                    is_commit_successful;
        /* Indicate whether OTASP/OTAPA activation is completed */

    boolean                    is_wait_for_commit_reply;
        /* Indicate whether we need to wait for OTASP-STATUS from MC
        ** at end of call */

    cm_reply_s_type            commit_reply;
        /* CM reply object to handles the details that are associated
        ** with waiting for an otasp commit status reply from MC */

} cmcall_otasp_info_type;


/* Enumeration of overall call states. At all times a CM call must be in one
** of the states below. This enum is used by logging!! Please notify
** cfa.team of any changes made.
*/
typedef enum cm_overall_call_state_e {

    CM_OVERALL_CALL_STATE_NONE=-1,      /* FOR INTERNAL USE OF CM ONLY! */

    CM_OVERALL_CALL_STATE_IDLE=0,       /* Call is in idle state - i.e. no call */
    CM_OVERALL_CALL_STATE_ORIG=1,       /* Call is in origination state */
    CM_OVERALL_CALL_STATE_INCOM=3,      /* Call is in alerting state */
    CM_OVERALL_CALL_STATE_ORIG_INCOM=4, /* There is an originating call as well
                                           as incoming
                                        */
    CM_OVERALL_CALL_STATE_CONV=5,       /* Call is in conversation state */
    CM_OVERALL_CALL_STATE_RELEASE=6,    /* Call is being released*/
    CM_OVERALL_CALL_STATE_RECALL_RSP_PEND=7, /* Recall rsp is pend on call */

    CM_OVERALL_CALL_STATE_MAX           /* FOR INTERNAL USE OF CM ONLY! */

} cm_overall_call_state_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef CM_GW_SUPPORTED


/* The first structure is used to store the charge data received from
 * the network.  The data contains seven number describing different
 * charges.  The comments below describe each field.
 */
typedef struct cm_ph_e_values
{
  unsigned long e1;     // Units charged per time interval (resolution = 0.1)
  unsigned long e2;     // Seconds/time interval (resolution = 0.1)
  unsigned long e3;     // Scaling factor. (resolution = 0.01)
  unsigned long e4;     // Initial charge unit increment. (resolution = 0.1)
  unsigned long e5;     // Units per data interval. (resolution = 0.1)
  unsigned long e6;     // Segments/data interval (resolution 1)
  unsigned long e7;     // Initial seconds/time interval (resolution = 0.1)
} cm_e_values_type;

/* The following structure becomes part of the call structure.  This structure
 * will have all the charge related data for that particular call.  It also
 * contains a pointer to the next call which has charge information.  The
 * intent of the pointer is to create a linked list of calls which have charge
 * information.  The list will be sorted in order of the ccm_timeout values,
 * with calls having charges next being placed first on the list.  When the
 * CCM_TIMER expires, the first item in the list will be processed, and that
 * call will be put back on the list in its new position.  The head of the
 * list will then be checked if its also timed out, and if so, that element
 * will be processed.  This continues until the head of the list is waiting
 * for a timeout, and then the CCM timer will be started to that timeout.
 * Elements with no time related charges will always be kept at the end of the
 * list, but the entire list must be traversed when computing the timeout in
 * 30 seconds.
 */
typedef struct
{
  boolean aoc_initialized;      // Have we seen any AOC data yet?
  cm_e_values_type e;           // Current E values.
  unsigned long int AOC;                        // Charge info for this call
  unsigned long int  last_AOC_to_update_CCM; // Last AOC put into CCM
  boolean new_e_values_charged; // Have we charged the new E4 yet?
  cm_e_values_type new_e;               // New E values pending to be counted.
  unsigned long segments_per_sec; // The approximation from DS of segs per second
  unsigned long segment_count;    // Total segments for this call.
  unsigned long segment_count_in_AOC; // Segments accounted for in AOC
  boolean use_ccm_timeout;      // Should ccm_timeout be used?
  rex_timer_cnt_type  ccm_timeout; // Timer for time related charges
} cm_wcall_aoc_data_type;


/* The following structure is stored in the phone object.  This
 * structure keeps track of the phone related information.  The
 * CCM is kept for the duration of a call until the next call begins.
 * The ccm_timer and acm_timer are the two timers required for maintaining
 * the AOC information.  The sent_warning variable will ensure that the
 * warning is only sent once.  Drop calls will be set to true when the ACM
 * exceeds ACMmax and charging calls will be dropped.
 */
typedef struct
{
  unsigned long CCM;                    // Current Call Meter (resolution 0.001)
  unsigned long last_CCM_to_update_ACM; // Current Call Meter in ACM
  unsigned long ACM;                    // Accumulated Call Meter.
  unsigned long ACMmax;         // Max allowed Accumulated Call Meter.
  rex_timer_type ccm_timer;     // Timer used for counting of AOC's
  boolean ccm_timer_running;    // State of above timer.
  rex_timer_type acm_timer;     // 5 second timer for ACM updates
  boolean acm_timer_running;    // State of above timer.
  boolean sent_warning;         // Has a warning been sent for ACM/ACMmax
  boolean drop_calls;           // Are we out of funds?
  boolean aoc_ready;            // Is the AOC subsystem ready?
  boolean is_acm_read_success;  //is ACM file read sucessfully?
  boolean is_acm_max_read_success;   //is ACM-MAX file read sucessfully?
} cm_ph_charge_data_type;


typedef struct
{

    uint8                               transaction_id;

    boolean                             accept;

    boolean                             success;

    byte                                call_progress;

    boolean                             inband_rcvd;

    cm_repeat_indicator_s_type          bc_repeat_indicator;

    cm_bearer_capability_s_type         bearer_capability_1;

    cm_bearer_capability_s_type         bearer_capability_2;

    cm_repeat_indicator_s_type          llc_repeat_indicator;

    cm_low_layer_compatibility_s_type   low_layer_compatibility_1;

    cm_low_layer_compatibility_s_type   low_layer_compatibility_2;

    cm_repeat_indicator_s_type          hlc_repeat_indicator;

    cm_high_layer_compatibility_s_type  high_layer_compatibility_1;

    cm_high_layer_compatibility_s_type  high_layer_compatibility_2;

    cm_called_party_bcd_no_s_type       called_party_bcd_number;

    cm_called_party_subaddress_s_type   called_party_subaddress;

    cm_calling_party_bcd_no_s_type      calling_party_bcd_number;

    cm_calling_party_subaddress_s_type  calling_party_subaddress;

    cm_num_s_type                       redirect_party_number;

    cm_forward_cug_info_s_type          forward_cug_info;

    uint32                              rab_id;

    byte                                cm_clir;

    ie_cm_cc_cause_s_type               cc_cause;

    ie_cm_mo_call_details_s_type        call_details;

    ie_cm_ss_error_s_type               ss_error;

    ie_cm_cc_reject_s_type              cc_reject;

    cm_connected_no_s_type              connected_num;

    cm_connected_subaddress_s_type      connected_subaddress;

    cm_call_sups_type_e_type            sups_type;
      /* Type of supplementary service */

    cm_call_sups_params_s_type          sups_params;
      /* Supplementary service specific data */

    cm_cause_of_no_cli_s_type           cause_of_no_cli;
      /* cause for no CLI in mobile terminated call */

    cm_ect_indicator_s_type             ect_info;
      /* explicit call transfer information */

    cm_call_ss_info_s_type              ss_info;
      /* call related SS information */

    cm_wcall_aoc_data_type              wcall_charge_data;

    #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif

    #ifdef FEATURE_UUS
    cm_user_data_s_type              mo_mt_user_data;
      /* user to user data request structure
      */
      #endif

    boolean                             is_receiver_busy;
      /* Used to report user data congestion
      ** False - Receiver not ready
      ** True  - Receiver ready
      */

    ie_cm_cc_cause_s_type               cm_cc_cause;
      /* Used to report user data congestion
      */

    boolean                             inband_tone_present;
      /* device does not show the busy screen immediately when user calls to a busy phone
      */

    cm_ccbs_ret_res_s_type              ccbs_ret_res;
      /* Stores relevant fields of 5_send_conf and recall indication
      ** received from Network.
      */

    cm_ccbs_recall_rsp_e_type           ccbs_recall_rsp;
      /* Stores recall response sent by client
      ** for call state CM_CALL_STATE_RECALL_RSP_PEND
      */

    boolean                             is_ccbs_recall_resp;
      /*
      ** CM decides if its an origination request or recall response
      ** based on this flag.
      ** TRUE   - Send a recall response
      ** FALSE  - Treat as call origination
      */

    cm_call_type_e_type                 new_call_type_req;
      /*
      ** New call type requested. Can be due to local
      ** MO in call modif or remote MT in call modif. call_type carries
      ** the current type of call and new_call_type_req is the requested
      ** new type for the call.
      */


    boolean                             incall_modif_err_status;
      /* Carries the result of in-call modification request
      **
      **  TRUE - Call modif req did not succeed, carries err code.
      **  FALSE - Call modif req is successfull.
      */


    boolean                             gw_cs_modify_res;
      /* Client response to modification request
      ** TRUE   - Modification is accepted by client
      ** FALSE  - Modification gets rejected by client
      */


    boolean                             is_call_mod_possible;
      /*
      ** Used by UI clients to display icon that can be chosen by user to
      ** perform call modifications.
      **
      ** TRUE   - Call can be modified from Voice to VT OR VT to Voice.
      ** FALSE  - Call cannot be modified.
      */

    boolean                             rab_id_present;
      /* TRUE   - rab_id carries valid value
      ** FALSE  - rab_id does not carry valid value
      */

    boolean                             is_network_initiated;
      /* TRUE   - Network initiated event
      ** FALSE  - Remote-user initiated event
      */

     sync_enum_T            handover_cause;
      /* Used to report handover indication from NAS
      */

     cm_alerting_pattern_s_type   alerting_pattern;
     /*  Alerting pattern IE
     */

} gsm_wcdma_cmcall_type;


typedef struct
{
    boolean                         primary_pdp;   /* True=Primary False=Secondary */
    qos_T                           qos;
    qos_T                           minimum_qos;
    pdp_address_T                   pdp_addr;
    apn_T                           apn_name;
    protocol_cfg_options_T          config_options;
    sys_pdp_header_comp_e_type      h_comp;
    sys_pdp_data_comp_e_type        d_comp;
    tft_type_T                      tft;
    nsapi_T                         nsapi;
    ti_T                            tear_down_ind;
    cause_enum_type_T               cause_type;
    pdp_cause_T                     cause;
    rabm_rabid_T                    rabid;
    sys_radio_access_tech_e_type    sys_rat;
    rabmcm_cause_T                  rab_cause;

    byte                            pdp_group_id;
    uint16                          assoc_pri_profile_num;
    boolean                         is_net_alloc_qos_diff_from_req;

    pdp_request_type_T              request_type;
    /** < indicates PDP Activate request type like Handover,Emergency
     should be read only if valid field is set.  
    */
    #ifdef FEATURE_LTE
    byte                            esm_conn_id;
    #endif

    uint16                          profile_number;

    #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif


    cm_sm_prot_descr_type        sm_pd;
      /* Currently used by call control interface only, not
      ** intended for call origination interface.
      **
      ** PDP Protocol descriptor. Defaults to SM_PD
      */

    cm_sm_trans_id_type       trans_id;
      /* Currently used by call control interface only, not
      ** intended for call origination interface.
      **
      ** Transaction id used to identify this pdp origination with the
      ** network.
      */

    cm_sm_msg_type            msg_type;
      /* Currently used by call control interface only, not
      ** intended for call origination interface.
      **
      ** mesg_type defaults to PDP ACTIVATION REQUEST.
      */


    llc_sapi_s_type             llc_sapi;
      /* Currently used by call control interface only, not
      ** intended for call origination interface.
      **
      ** Logical link controller's service access point requried for this
      ** origination. Defaults to LLC_SAPI_3
      */

    timer_3gpp_type3                backoff_timer;
    /** 3gpp type3 timer **/

    pdp_reject_ind_params_s_type    pdp_reject_ind;
    /**< pdp reject parameters, apn name & apn type */

} gsm_wcdma_cmcall_ps_type;
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */



#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

typedef struct
{
    word                       srv_opt;
        /* service option */

    cm_privacy_pref_e_type     privacy_pref;
        /* Indicate the user preference of privacy mode */

    boolean                    is_privacy;
        /* Indicate whether privacy mode is on */

    cmcall_otasp_info_type     otasp_info;
          /* Contains the necessary information for OTASP/OTAPA calls */

    boolean                    is_last_cdma_info_rec;
        /* this field indicates whether this is the last
           CDMA info record or not */

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    byte                        ext_disp_type;
        /* display type for extended disaply info record */
    #endif

   #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    boolean                    alert_ans_bypass;
        /* Indicate whether the incoming call need to be answered by the clients */
    #endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))S  */

    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    boolean                     ndss_in_progress;
    /* MC is perfoming NDSS redirection */
    boolean                     ndss_connect;
    /* send NDSS connect instaead of connect */
    #endif

    #ifdef FEATURE_IS2000
    byte end_reason;
      /* the reason for ending a call, it's different from end_status to keep
       * the old interface unchanged while adding more IS2000 specific reasons
       * */
    boolean                     drs_bit;
      /* to set the DRS bit or not */

    cm_retry_order_s_type         retry_order;
      /* retry order info */

    cm_call_id_type             emergency_flash_call_id;
      /* The call used in sending emergency flash */

    boolean                     emergency_flash_over_incoming_call;
      /* whether a previous emergency request was flashed on this
      incoming call or not */
    #endif /* FEATURE_IS2000 */

    cm_call_sups_type_e_type      flash_type;
      /* flash type */

    cm_ext_brst_intl_s_type       ext_brst_intl_msg;
    /* Contains charge rate information */
    cm_nss_clir_rec_s_type        nss_clir_rec;
    /* National Supplementary services - CLIR */
    cm_nss_aud_ctrl_s_type        nss_aud_ctrl_rec;
    /* National Supplementary services - Audio Control */

    #ifdef FEATURE_IS2000_REL_A
    boolean               qos_parms_incl;
    byte                  qos_parms_len;
    byte                  qos_parms[CAI_ORIG_QOS_LEN_MAX];
    boolean               sr_id_included;
    byte                  sr_id;
    #endif /* FEATURE_IS2000_REL_A */

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
    #error code not present
#endif

    /* Line control parameters
    */
    boolean                              line_ctrl_polarity_included;
      /* TRUE if polarity info included
      */

    boolean                              line_ctrl_toggle;
      /* TRUE = toggle polarity
      */

    boolean                              line_ctrl_reverse;
      /* TRUE = reverse polarity
      ** FALSE = normal polarity
      */

    byte                                 line_ctrl_power_denial;
      /* Power denial time
      */

    cm_call_orig_fail_layer_e_type       orig_fail_layer;
      /* Highest layer where the call origination setup failed.
      ** This field is adjusted after every silent redial failure.
      */

     cm_con_ref_type                      con_ref;
     /* con_ref of the call
     */

    #if defined (FEATURE_LTE_TO_1X)
    ie_cm_cc_reject_T                    cc_reject;
      /* Reject cause info
      */
    #endif

    boolean                              is_special_srv_data_call;
     /* Special data call to clean up existing data session
     */

     boolean is_so_to_call_type_req;

} cdma_cmcall_type;

#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */


#if (defined(FEATURE_IP_CALL))
typedef struct
{
  cm_call_prog_info_e_type              call_prog_info;
  cm_ip_call_attrib_info_s_type         ipcall_attrib_info;
    /* Current call attribute info
    */
  cm_ip_caller_info_s_type *caller_info;
   /* Gives Caller info */

  cm_ip_call_cap_attrib_info_s_type         ipcall_local_cap_attrib_info;
    /* Current local call capability attribute info
    */

  cm_ip_call_cap_attrib_info_s_type         ipcall_peer_cap_attrib_info;
    /* Current peer call capability attribute info
    */

  cm_ip_call_attrib_info_s_type         req_modify_call_attrib;
    /* requested call attrib info to be modified. Modification only takes effect when
    ** got modify success conf.
    */
  cm_call_type_e_type                   req_modify_call_type;
    /* requested call type to be modified. Modification only takes effect when
    ** got modify success conf
    */
  boolean                               call_modif_err_status;
    /**< Error status returned by the IMS as a result of call modification
         request:
         - TRUE -- Error in processing the in-call modification request.
         - FALSE -- No error in processing the in-call modification request. \n

    */
  boolean                               call_modify_response;
    /**< call modify response sent by user
    */
  cm_num_s_type                         num;
    /**< call num from user to remove last party from conference
    */

  cm_call_end_e_type               modify_error_code;
    /* mofify error code
    */

  cm_call_modify_cause_e_type      modify_cause_code;
    /* mofify cause code
    */

  uint32                           retry_timer_val;
    /*retry timer indicates in case of upgrade scenario collision or reject request
    ** so that user can try after such time
    */

  cm_ip_ext_prim_sups_s_type   ip_sups_param;
      /* IP extended parameters */

	cm_ip_call_end_reason_s_type  *ims_end_reason;

   boolean                      switch_codec_cmd_status;
	  /* returns status of switch codec command from IMS to QMI-V */

  cm_call_attrib_e_type       held_call_audio_attrib;
   /* This field tells if Audio RX /TX is available for 
    * Held call. Valid only for REMOTE_PARTY_HOLD Procedure
   */


  boolean                        connected_num_is_ect_trig;
  /* Connected number event is triggered by ECT procedure */

} ip_cmcall_type;
#endif



#if defined (FEATURE_CM_LTE)
typedef cm_lte_call_info_s_type lte_cmcall_type;
#endif /* defined (FEATURE_CM_LTE) */
typedef struct cmcall_info_s {
    #ifdef CM_GW_SUPPORTED
    /* in_CS for all tasks. */
    gsm_wcdma_cmcall_type        gsm_wcdma_info;
    gsm_wcdma_cmcall_ps_type     gsm_wcdma_ps_info;
    #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    cdma_cmcall_type             cdma_info;
    #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

    #if defined (FEATURE_IP_CALL)
    ip_cmcall_type               ip_info;
    #endif /* defined (FEATURE_IP_CALL) */

    #if defined (FEATURE_CM_LTE)
    lte_cmcall_type              lte_info;
    #endif /* defined (FEATURE_CM_LTE) */

} cmcall_info_s_type;


typedef struct
{
    cm_call_mode_info_e_type     info_type;
    cmcall_info_s_type           info;

} cmcall_mode_info_type;

/* The following ENUM is used to determine if call control is required for
** a given technology.  Call control and the SIM from which the subscription
** is obtained are now linked.
*/
typedef enum cmcc_card_status_e {

  CMCC_CARD_STATUS_CC_NONE = -1,
    /**< @internal */

  /* 0 */
  CMCC_CARD_STATUS_CC_NOT_REQUIRED,
    /**< Either not a SIM subscription or the SIM
    ** doesn't require Call Control
    */

  /* 1 */
  CMCC_CARD_STATUS_CC_REQUIRED,
    /**< Call control is required for this subscription */

  /* 2 */
  CMCC_CARD_STATUS_CC_COMPLETED,
    /**< Call control was required, and has been completed. */

  CMCC_CARD_STATUS_CC_MAX
    /**< @internal */

} cmcc_card_status_e_type;


/* The following structure is used when issuing a call control command
 * to keep track of what is required, what has been completed, and the
 * results.
 */
typedef struct cmcc_object_s {

  cmcc_card_status_e_type sim_cc_required;
     /* If SIM CC is required */
  cm_nc_call_control_complete_cb_f_type *cc_complete_cb_func;
     /* Callback to use */
  void  *user_data;                             /* User data supplied. */
  cm_call_id_type call_id;                      /* Original Call id */
  cm_cc_status_e_type status;                   /* Determined status */
  cm_call_type_e_type call_type;                /* Determined call type */
  cm_orig_call_control_s_type orig_request;     /* Initial origination */
  cm_orig_call_control_s_type cc_result_buf;    /* Buffer for CC Result */
  cm_orig_call_control_s_type *cdma_result_ptr; /* Pointer to above. */
  cm_orig_call_control_s_type *gw_result_ptr; /* Pointer to above. */

  mmgsdi_session_app_capabilities_cnf_type sim_capabilities;
  boolean            sim_capabilities_valid;
  boolean            srv_available;
  boolean            srv_available_valid;

  /* The following are params from CM that we can't fully use immediately */
  cm_cc_subscription_source_e_type      cdma_subscription_source;
  cm_cc_subscription_source_e_type      gw_subscription_source;
  boolean            is_envelope_command_sent;
  boolean            is_cc_delayed;
  cm_ss_e_type       ss;


} cmcc_object_s_type;

/* A type for parameters that holds ssac infomation.
*/
typedef struct {

  lte_rrc_ssac_params_s               ssac_params;
  /* LTE RRC SSAC parameters */

  dword                               ssac_voice_uptime;
  /* SSAC uptime for MMTEL-Voice call */

  dword                               ssac_video_uptime;
  /* SSAC uptime for MMTEL-Video call */

  lte_rrc_plmn_s                      plmn;
  /* Current selected plmn */

  uint8                               r12_ssac[MAX_SIMS];
  /*value 1 - Rel-12 spec cr ON
  * value - 0  Rel-12 spec cr OFF
  * With rel-12 spec cr SSAC need to be checked every time even in LTE-connected mode
  * Without rel-12 spec cr SSAC check is applicable only in LTE-IDLE state
  */
  lte_rrc_global_cell_id_t            cell_id;

  rex_timer_type                      ssac_delay_timer;  /*SSAC delayed success handling timer*/
  uint32                              backoff_timer_accumulate;
  cm_call_cmd_s_type                  *call_cmd_ptr;
  sys_plmn_id_s_type                   ssac_associated_plmn;

} cmcall_ssac_s_type;

typedef enum
{
  SSAC_CHK_PASS,
  SSAC_CHK_FAIL,
  SSAC_CHK_WAIT
}cmcall_ssac_chk_e_type;
/* A type for parameters that holds ssac barring check result.
*/
typedef struct {

  cmcall_ssac_chk_e_type  is_ssac_check_pass;
  /* Indicate if it passes SSAC barring check */

  uint32                ssac_backoff_time;
  /* SSAC backoff time */

} cmcall_ssac_check_result_s_type;

/* Enumeration of MMTEL type
*/
typedef enum {
  SSAC_MMTEL_TYPE_NONE,
  SSAC_MMTEL_TYPE_VOICE,
  SSAC_MMTEL_TYPE_VIDEO,
  SSAC_MMTEL_TYPE_VOICE_VIDEO,
  SSAC_MMTEL_TYPE_MAX
} cmcall_ssac_mmtel_e_type;

/* Enumeration of ssac timer type
*/
typedef enum {
  SSAC_TIMER_ACTION_NONE,
  SSAC_TIMER_ACTION_START,
  SSAC_TIMER_ACTION_STOP,
  SSAC_TIMER_ACTION_CANCEL,
  SSAC_TIMER_ACTION_MAX
} cmcall_ssac_timer_action_e_type;

/* Call Type - holds all the information that is associated with
** a specific call.
*/
typedef struct cmcall_s {

    cm_init_mask                       init_mask;
      /* Indicate whether this object was properly initialized
      */

    cm_call_id_type                    call_id;
      /* call ID of this call Object
      */

    sys_modem_as_id_e_type             asubs_id;
      /* Active subscription id
      */

    sys_modem_as_id_e_type             client_asubs_id;
      /* Client asubs_id is to store original asubs_id as requested by client.
      ** This should always be same as asubs_id except for some cases
      ** of Emergency call.
      */

    cm_call_state_e_type               call_state;
      /* Call state - idle, orig, etc.
      */

    cm_call_substate_type              call_subst;
      /* Call substate.
      */

    cm_client_s_type                   *call_client_ptr;
      /* Call client - i.e. originating/answering client
      */

    cm_call_type_e_type                call_type;
      /* Call type - VOICE, SMS, E911, etc
      */

    cm_srv_type_e_type                 srv_type;
      /* service type - AMPS, CDMA, automatic, etc.
      */

    sys_ho_type_e_type                 ho_type;
      /* Flag to indicate if it is umts to 1x handover.
      */

    cm_num_s_type                      num;
      /* Number buffer:
      ** CM_CALL_EVENT_ORIG           - contains dialed digits string.
      ** CM_CALL_EVENT_FLASH          - contains dialed digits string.
      ** CM_CALL_EVENT_INCOM          - contains caller ID string.
      ** CM_CALL_EVENT_CALLER_ID      - contains caller ID string.
      ** CM_CALL_EVENT_CONNECTED_NUM  - contains connected number string.
      ** CM_CALL_EVENT_CALLED_PARTY   - contains called party string.
      **                                Note: presentation and screening indicator
      **                                      fields for called party info are not
      **                                      used
      */

    cm_num_s_type                      calling_num;
      /* Mobile number information
      */
    #ifdef FEATURE_IP_CALL
    cm_num_s_type                      waiting_num;
      /* Mobile number information
      */

    cm_num_s_type                       child_num;
   /* When MT Invite made to a child number */
    #endif

    cm_redirecting_num_s_type          redirecting_number;
      /* Number buffer:
      ** CM_CALL_EVENT_REDIRECTING_NUM          - contains redirecting num.
      */

    cm_alpha_s_type                    alpha;
      /* Alpha tag to be displayed along with dialed address
      ** of an originated call.
      **
      ** NOTE! this is not a phone book name match, but rather
      ** an indication for the UI to display some text for an
      ** outgoing call
      **
      ** We probably need to change this one to a UI text component to
      ** support I18N
      Alpha buffer:
      CM_CALL_EVENT_ORIG
      CM_CALL_EVENT_DISPLAY
      CM_CALL_EVENT_EXT_DISP
      */

    cm_enc_alpha_s_type                    enc_alpha;
      /* Alpha tag to be displayed along with dialed address
      ** of an originated call. This has coding information that
      ** may be later used by UI
      */

    cm_signal_s_type                   signal;
      /* Alert signal information to be played for an incoming call
      */

    cm_call_end_e_type                 end_status;
      /* Call end status (i.e. reason for ending a call in origination,
      ** incoming or conversation state)
      */
    cm_call_end_e_type                 prot_end_status;
      /* Call end status (i.e. reason for ending a call in origination,
      ** incoming or conversation state)
      */
    void (*orig_func)                  ( cmcall_p_type call_ptr );
      /* Pointer to origination processing function. Different call types
      ** might have different origination processing functions associated
      ** with them
      */

    cm_sr_s_type                       sr;
      /* Silent redial component. Holds all the information that is
      ** associated with the silent redial mechanism
      */


    #if (defined (FEATURE_UMTS_1X_HANDOVER_1XMSM))
    #error code not present
#endif

    sys_umts_cdma_handover_e_type      umts_cdma_ho_status;

    sys_ho_status_e_type               ho_status;

    cm_reply_s_type                    reply;
      /* CM reply object to handles the details that are associated
      ** with waiting for a reply from MC
      */

    cmph_p_type                        ph_ptr;
      /* Pointer to phone object. Holds all the information that is
      ** associated with the phone and its settings
      */

    boolean                            is_user_originated_call;
      /* is this a call that we are originating?
      */

    #ifdef FEATURE_UIM_RUIM
    dword                              ruim_last_poll_time;
    #endif /* FEATURE_UIM_RUIM */

    cmcall_mode_info_type              cmcall_mode_info;
    #ifdef CM_GW_SUPPORTED
    struct cmcall_s                    *next_charged_call;
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #ifdef FEATURE_HDR_AN_AUTH
    boolean                            an_auth_status;
      /* AT is authenticated with AN or not */
    #endif
    #ifdef FEATURE_HDR_HANDOFF
    sys_sys_mode_e_type                last_act_data_net;
      /* Last active data network */
    #endif /* FEATURE_HDR_HANDOFF */

    cm_mode_pref_e_type                favored_mode_pref;
      /* Favored mode preference to originate the call
      */

    cm_prl_pref_e_type                 favored_prl_pref;
      /* Favored prl preference to originate the call
      */

    cm_roam_pref_e_type                favored_roam_pref;
      /* Favored roam preference to originate the call
      */

    cm_hybr_pref_e_type                favored_hybr_pref;
      /* Favored hybrid preference to originate the call
      */
    cm_ss_e_type                       ss;
      /* indicate which SD's system slection info should be
      **used for the call
      */

    #if (defined( FEATURE_HDR_HYBRID ))

    boolean                            is_hybr_allowed;
      /* whether or not packet data call is allowed to originate on
      ** either HDR or CDMA when the hybrid operation is enabled
      */
    #endif

    #ifdef FEATURE_HDR_HYBRID

    boolean                            ignore_hdr_call_conn_rpt;
      /* whether or not the received CM_HDR_CONNECTED_F
      ** report should be discard or not
      */

    boolean                            waiting_for_hdr_call_end;
      /* whether an hdr call was just ended for a 1x origination,
      ** in which case the origination may have to be delayed for
      ** hdr to gracefully close its connection.
      */

    #endif /* FEATURE_HDR_HYBRID */

    boolean                            send_call_event_end;
      /* indicate whether the call event end should be sent to the
      ** clients.
      */

    cm_end_params_s_type               end_params;
      /* end parameter information
      */

    sys_sys_mode_e_type                sys_mode;
      /* Indicates the system on which the call is made
      */

    cm_orig_call_control_s_type        cdma_cc_modified_info;
      /* Stores call control modified cdma information
      */

    dword                              call_connect_time;
      /* The time the call was connected.  The units is seconds since
      ** the phone has powered on (clock uptime).
      **/

    dword                              call_end_time;
      /* The time the call was ended.  The units is seconds since
      ** the phone has powered on (clock uptime).
      **/

    sd_ss_orig_mode_e_type             orig_mode;
      /* Indicates the current call orig. mode.
      */

    #ifdef CM_GW_SUPPORTED
    cm_orig_call_control_s_type        gw_cc_modified_info;
      /* Stores call control modified gw information
      */
    #endif

    uint32                             bytes_received;
      /* The number of bytes received during the call (PS data call only).
      ** This field is populated after the call has ended.
      */

    uint32                             bytes_transmitted;
      /* The number of bytes transmitted during the call (PS data call only).
      ** This field is populated after the call has ended.
      */

    byte                               gw_connection_id;
      /*
      ** This holds the connection Id field for the GSM/WCDMA calls.
      */

    cm_conn_id_src_e_type              gw_conn_id_src;
      /* This holds the source of the connection Id, i.e. SM or MN.
      */

    cm_als_line_e_type                 line;
      /* what line is being used for this call
      */

    cm_call_direction_e_type           direction;
      /* Holds direction of the current call.
      ** It can be Mobile originated or Mobile
      ** Terminated
      */

    cm_cc_mode_pref_e_type      main_ss_mode;

    dword                       end_req_wait_uptime;

    #if defined(FEATURE_IP_CALL)
    cmipapp_id_type             ipapp_id;
      /* Holds the IP app id the call is associated with
      */

    cm_call_end_e_type          end_cause_to_client;
      /* VOLTE call SIP layer end cause sent to client
      */

    #endif

    boolean                     is_volte;
      /* Indicate if VOLTE call possible
      */

    boolean                     is_vowlan;
      /* Indicate if VOWLAN call possible
      */

    boolean                     is_ip_call;
      /* Indicate if this is IP call, applicable to voice/VS/VT etc. The flag will be set
      ** when found ipapp register with CM and the sys mode has no conflict with fav_mode*/

    uint32                      ipapp_data_block;
      /* Token created and used by registered IP apps.
      ** App gives CM this value whenever cmipapp_rpt_ind () is called.
      ** CM passes the most recent value given by app while calling
      ** app registered call back functions
      */
    sys_srv_status_e_type        prev_srv_status[CM_SS_MAX];
      /* Holds the previous service status information
      */
    boolean                      force_top_pref;
      /* Force the preferences to the top of the origination parameter Queue
      ** even if the top ptr didn't change
      */

    cmcall_pd_status_e_type      pd_status;
      /* Whether a PD session was ended, and we need to wait for some
      ** time before sending the origination down
      */

    cmcall_sms_status_e_type     sms_status;
      /* Do we need to hold orig because SMS is waiting for confirmation
      */

    boolean                      is_hdr_acq_fail;
      /* Is HDR acq failed received before for this call
      */

    boolean                      waiting_for_hdr_session_close;
      /**<
      ** Any active HDR session needs to be closed before originating this
      ** call.
      */

    boolean                      is_707b_ps_data;
      /*
      ** Is IS-707B Addendum PS Data Call
      */

    boolean                     is_modem_cc_api;
      /* Indicates whether new API for Centralized call control is called */
    cm_call_csfb_e_type        csfb_type;

    sys_cm_domain_sel_domain_e_type    dom_sel;
      /* domain to be used for the call*/

    cm_voice_to_ss_param_s_type        voice_to_ss;
      /* SS params related to callc ontrol*/

    #if defined (FEATURE_LTE_TO_1X)
    boolean is_rlf_in_1xcsfb_call;
      /* Is RLF in progress for 1XCSFB call */
    #endif

    sys_speech_info_s_type             speech_codec_info;

    cmcc_object_s_type                *cc_object_ptr;

    cm_call_hold_state_e_type          call_hold_state;
     /* Indicates call hold state  if active or local hold*/

    sys_voice_handover_e_type          voice_ho_type;
    /* Indicates the type of Handover from LTE to Legacy RAT's*/


  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

       /* Mid Call SRVCC related fields */

    boolean               is_srvcc_call;
    /*is an srvcc call in HO
     */

    boolean               is_int_ims;
    /* identifies IMS type enabled at SRVCC start */

    byte                ims_connection_id;
    /* connection id saved to retrieve during SRVCC fail
      */

    boolean               is_mpty;
    /* Indicates mpty call
      */

    boolean               is_conf_call;
     /*Used to identify IMS conf call before SRVCC */

    cm_call_id_type                    parent_call_id;
      /* call ID of this IMS call Object
      */

    boolean                mid_call_supported;
     /* mid call support flag for IMS call events */

    cm_srvcc_alert_media_type  alert_media_type;
     /* used to identify type of early media request
        sent by IMS before SRVCC.
        Will be used only for Alerting call types */

    cm_call_type_e_type               ims_call_type;
      /* temporarily store VT call type during SRVCC
         which will be used during FAIL/CANCEL */

    cm_srvcc_prog_state                 srvcc_prog_state;
     /* State of SRVCC currently in progress,
        Used currently for 1xSRVCC to handle user call
        operations */

	cm_srvcc_caller_name_s_type    srvcc_caller_info;
	  /* caller name passed on from AP during HO response */

  #endif

  #ifdef FEATURE_MMODE_DUAL_SIM
  boolean                            is_emerg_on_other_sub;
  #endif
    unsigned long                      call_owner;

  /* Below will be only used in SRLTE mode in cmcall_orig_proc ,
    ** to determine whether we are waiting for the rsp of NAS_ESM_1XCSFB_CALL_REQ for MO calls only */
  boolean                            is_waiting_on_esr_rsp;

  /* Below will be only used in SRLTE mode after call ends,
    ** to determine whether we we need to switch to SRLTE or not */
  boolean                            switch_to_srlte;

  cm_orig_client_categ_e_type        client_categ;
  /**< Valid for MO call only, holds the client info, which originated the call */

  boolean    is_em_vt;

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  boolean orm_sent_status; /* whether 1XORM was sent succesfully by 1XCP */
  cm_call_id_type drvcc_parent_call_id; 
  /* used to maintain mapping between switch in and switch out call id */
  #endif 

  boolean    is_emerg_ps_redial;

  boolean                           is_special_pref_volte_redial;
  /* redial over LTE if no CS coverage during CSFB */

  cm_call_state_e_type               prev_call_state;
    /* Call state before call ends. only valid after call ends*/
 
  boolean                            is_call_end_info_sent;
    /* flag to mark that call end info has been sent */

  uint8                              call_debug_info;
    /* used to keep track of various call debug info */
} cmcall_s_type;


/* Overall Calls information - holds the information
** associated with all call objects
*/
typedef struct cm_overall_calls_s
{
    byte                  num_of_call_ids_allocated;
          /* number of call IDs allocated */

    byte                  num_of_calls_active;
          /* number of calls that are active */

    //cm_overall_call_state_e_type  overall_state;
          /* overall state of the call objects */

} cm_overall_calls_s_type;

/* Macros to check the call state and substate.
*/
#define CMCALL_IS_INCOM_OTAPA(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_INCOM) && \
   ((call_ptr)->call_subst.incom == CM_CALL_INCOM_SUBST_OTAPA))

#define CMCALL_IS_INCOM_REGULAR(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_INCOM) && \
   ((call_ptr)->call_subst.incom == CM_CALL_INCOM_SUBST_REGULAR))

#define CMCALL_IS_CONV_REGULAR(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_CONV) && \
   ((call_ptr)->call_subst.conv == CM_CALL_CONV_SUBST_REGULAR))

#define CMCALL_IS_CONV_COMMIT(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_CONV) && \
   ((call_ptr)->call_subst.conv == CM_CALL_CONV_SUBST_COMMIT))

#define CMCALL_IS_CONV_PC_OTAPA_REGULAR(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_CONV) && \
   ((call_ptr)->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_REGULAR))

#define CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr) \
  (((call_ptr)->call_state == CM_CALL_STATE_CONV) && \
   ((call_ptr)->call_subst.conv == CM_CALL_CONV_SUBST_PC_OTAPA_COMMIT))

#define CMCALL_IS_PC_OTAPA(call_ptr) \
  (CMCALL_IS_INCOM_OTAPA(call_ptr) || \
         CMCALL_IS_CONV_PC_OTAPA_REGULAR(call_ptr) || \
         CMCALL_IS_CONV_PC_OTAPA_COMMIT(call_ptr) )

#define CMCALL_IS_HDR_SRV_TYPE(call_ptr) \
  (((call_ptr)->srv_type == CM_SRV_TYPE_HDR) || \
  ((call_ptr)->srv_type == CM_SRV_TYPE_HDR_MORE_PREF))

#ifdef FEATURE_HDR_HYBRID

#define CMCALL_IS_HDR_ORIG_ALLOWED(call_ptr) \
          ( call_ptr->ss  == CM_SS_HDR ||   \
            (call_ptr->ss  == CM_SS_MAIN && \
             call_ptr->favored_mode_pref == CM_MODE_PREF_HDR_ONLY) || \
            (call_ptr->ss  == CM_SS_MAIN &&                           \
             !cmph_get_config_hybr_status(call_ptr->ss) &&                         \
              cmcall_misc_is_mode_pref( call_ptr->favored_mode_pref, \
               CM_MODE_PREF_HDR_ONLY ) && \
             ( call_ptr->call_type == CM_CALL_TYPE_PKT_DATA ) && \
             ( (CMCALL_IS_HDR_SRV_TYPE(call_ptr) )  || \
                call_ptr->srv_type == CM_SRV_TYPE_AUTOMATIC || \
                call_ptr->srv_type == CM_SRV_TYPE_CDMA_HDR ) && \
              ( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR || \
                call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NO_SRV )  ) \
          ) \


#define CMCALL_IS_HDR_CALL_ORIG(call_ptr) \
        ( call_ptr->call_state == CM_CALL_STATE_ORIG   && \
          CMCALL_IS_HDR_ORIG_ALLOWED(call_ptr) \
        ) \

#define CMCALL_IS_NON_HDR_CALL_ORIG(call_ptr) \
        ( call_ptr->call_state == CM_CALL_STATE_ORIG   && \
          CMCALL_IS_HDR_ORIG_ALLOWED(call_ptr) == FALSE ) \

#define CMCALL_IS_HDR_CALL(call_ptr) \
      ( ( call_ptr->call_state == CM_CALL_STATE_INCOM || \
          call_ptr->call_state == CM_CALL_STATE_CONV ) &&  \
         call_ptr->cmcall_mode_info.info.cdma_info.srv_opt == CAI_SO_HDR_PKT_DATA )  \

#define CMCALL_IS_NON_HDR_CALL(call_ptr) \
      ( ( call_ptr->call_state == CM_CALL_STATE_INCOM || \
          call_ptr->call_state == CM_CALL_STATE_CONV ) &&  \
          call_ptr->cmcall_mode_info.info.cdma_info.srv_opt != CAI_SO_HDR_PKT_DATA)  \


#define  CMCALL_IS_HDR_CALL_ORIG_OR_HDR_CALL(call_ptr) \
     (  CMCALL_IS_HDR_CALL_ORIG(call_ptr) || \
        CMCALL_IS_HDR_CALL(call_ptr) ) \


#define  CMCALL_IS_NON_HDR_CALL_ORIG_OR_NON_HDR_CALL(call_ptr) \
     (  CMCALL_IS_NON_HDR_CALL_ORIG(call_ptr) || \
        CMCALL_IS_NON_HDR_CALL(call_ptr) ) \

#endif /* FEATURE_HDR_HYBRID */



/*===========================================================================
FUNCTION cmcc_send_envelope_cmd

DESCRIPTION
 Sends the GSTK envelope command

DEPENDENCIES
  This function is called after CM gets ok_to_orig

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmcc_send_envelope_cmd(cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmcc_object_deallocate

DESCRIPTION
  Deallocates a CM CC object

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcc_object_deallocate
(
    cmcc_object_s_type *cmcc_object_ptr
      /* CM CC object */
);

/*===========================================================================

FUNCTION cmcall_mc_rpt_proc

DESCRIPTION
  Process MC reports


DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);



/*===========================================================================

FUNCTION cmcall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);



/*===========================================================================

FUNCTION cmcall_sd_rpt_proc

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);



#if (defined (FEATURE_UMTS_1X_HANDOVER_1XMSM))
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */


/*===========================================================================

FUNCTION cmcall_init

DESCRIPTION
  Initializing the call object.

  This function must be called before the call object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

extern void cmcall_init( void );


/*===========================================================================

FUNCTION cmcall_info_alloc

DESCRIPTION
  Allocate a call info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated call info struct.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_mm_call_info_s_type  *cmcall_info_alloc( void );


 #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmcall_srvcc_info_alloc

DESCRIPTION
  Allocate a call info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated srvcc call info struct.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_mm_srvcc_call_info_s_type  *cmcall_srvcc_info_alloc( void );

/*===========================================================================

FUNCTION cmcall_end_non_srvcc_call_ho_complete

DESCRIPTION
  1)Finds and ends calls not in SRVCC HO Procedure. This is done
  after sending ho_complete list of calls to clients.
  2)Calls which got originated after SRVCC HO Start and yet to
    identify a mode are ended.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_end_non_srvcc_call_ho_complete(void);


#endif



/*===========================================================================

FUNCTION cmcall_ptr

DESCRIPTION
  Return a pointer to the one and only call object.

  CM call object.
  The call object is responsible for:
  1. Processing clients' call commands.
  2. Processing MC replies for clients' call commands.
  3. Processing MC call related notifications.
  4. Notifying the client list of call events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cmcall_s_type  *cmcall_ptr(

         byte     call_index
             /* index into the call object array */
);



/*===========================================================================

FUNCTION cmcall_call_id_get

DESCRIPTION
  Get the call id corresponding to a call object pointer.

DEPENDENCIES
  None

RETURN VALUE
  call_id

SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_id_type cmcall_call_id_get( const cmcall_s_type *call_ptr );

/*===========================================================================

FUNCTION cmcall_get_asubs_id

DESCRIPTION
  Get asubs_id for the call obj.

DEPENDENCIES
  None.

RETURN VALUE
  sys_modem_as_id_e_type.

SIDE EFFECTS
  None.
===========================================================================*/
sys_modem_as_id_e_type cmcall_get_asubs_id (
  const  cmcall_s_type  *call_ptr
);

/*===========================================================================

FUNCTION cmcall_assert

DESCRIPTION
  Check a series of assertions over the call object to make sure that
  none of its fields are corrupted and to validate its state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_assert(

    const cmcall_s_type    *call_ptr
        /* pointer to a call object */
);


/*===========================================================================

FUNCTION cmcall_event

DESCRIPTION
  Notify the client list of a specified call event.
  This function is for events that apply to a particular call
  (call with a specific call id)

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_event(

    cmcall_s_type    *call_ptr,
        /* pointer to a call object. */

    cm_call_event_e_type    call_event
        /* notify client list of this call event */
);

/*===========================================================================

FUNCTION cmcall_process_dom_sel_params

DESCRIPTION
  Decide if the call can be originated as a CSFB call depending on
  the current voice domain settings.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  the csfb type that can be used for teh call.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_csfb_e_type cmcall_process_dom_sel_params (

  const cmcall_s_type    *call_ptr
    /* pointer to a call object */
);
/*===========================================================================

FUNCTION sr_reset

DESCRIPTION
  Reset a silent redial component, so it is ready for next call.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void sr_reset(

    cm_sr_s_type  *sr_ptr
        /* pointer to silent redial object to reset */
);
/*===========================================================================

FUNCTION cmcall_process_ipcall_params

DESCRIPTION
  Remove HDR component from favored mode pref of a voice call if ss
  instance of voice call is different from that of HDR service that has
  VoIP.

  Setting force_top to TRUE for voice calls that can be possibly sent
  over IP. This takes care of multi call scenarios (MPTY, ECT on voip)
  where more than one voice call needs to be connected.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  Given call_ptr is modified.

===========================================================================*/
extern void cmcall_process_ipcall_params (

  cmcall_s_type    *call_ptr
    /* pointer to a call object */
);
/*===========================================================================

FUNCTION cmcall_event_2

DESCRIPTION
  Notify the client list of a specified call event.
  This function is for events that apply to a particular call
  (call with a specific call id)

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmcall_event_2(

    cmcall_s_type    *call_ptr,
        /* pointer to a call object. */

    cm_call_event_e_type    call_event,
        /* notify client list of this call event */

    const void   *cm_rpt_ptr
        /* dummy report pointer */
);



/*===========================================================================

FUNCTION cmcall_end

DESCRIPTION
  Does call end necessary activities.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmcall_emergency_call_active

DESCRIPTION
  Check all call id's to see if there is an emergency call active

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean  cmcall_emergency_call_active ( void );



/*===========================================================================

FUNCTION cmcall_overall_end_calls

DESCRIPTION
  Ends all calls, except emergency calls when card is removed/in error.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmcall_overall_end_calls ( void );

/*===========================================================================

FUNCTION cmcall_overall_end_calls_per_subs

DESCRIPTION
  Ends all calls for given ss, except emergency calls when card is removed/in error.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_overall_end_calls_per_subs
(
    cm_ss_e_type ss
);

#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)

/*===========================================================================

FUNCTION cm_new_incom_call_id

DESCRIPTION
  Allocate a new call id for an incoming call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cm_new_incom_call_id
(
  cm_call_type_e_type       call_type,
  cm_call_id_type          *call_id_ptr,
  byte                      connection_id
);


/*===========================================================================

FUNCTION cm_find_call_id_per_subs

DESCRIPTION
  Find the call ID using the connection ID & asubs id.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cm_find_call_id_per_subs
(
  cm_conn_id_src_e_type   ll_src,
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr,
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION cm_find_call_id

DESCRIPTION
  Find the call ID using the connection ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cm_find_call_id
(
  cm_conn_id_src_e_type   ll_src,
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr
);


/*===========================================================================

FUNCTION cm_find_call_id_gwl

DESCRIPTION
  Find the call ID during LTE to GW IRAT.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_gwl
(
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr,
  sys_modem_as_id_e_type  asubs_id
);


/*===========================================================================

FUNCTION cm_find_call_id_nsapi

DESCRIPTION
  Find the call ID for non-transferrable dedicated bearer based on nsapi/eps_bearer_id.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_nsapi
(
  byte                    nsapi,
  cm_call_id_type         *call_id_ptr,
  sys_modem_as_id_e_type  asubs_id
);


/*===========================================================================

FUNCTION cm_find_call_id_gw

DESCRIPTION
  Find the call ID with gw(SM) connection_id
  This will search for if there is any call_ptr which has gw connection id

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_gw
(
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr
);

/*===========================================================================

FUNCTION cm_find_connection_id

DESCRIPTION
  Find the call type and connection ID using the call ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cm_find_connection_id
(
  cm_call_id_type         call_id,
  byte                    *connection_id_ptr
);
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_CM_LTE) */

#ifdef CM_GW_SUPPORTED
/*===========================================================================

FUNCTION cm_set_connection_id

DESCRIPTION
  Set connection ID using the call ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern void cm_set_connection_id
(
  cmcall_s_type           *call_ptr,
  byte                    connection_id
);


/*===========================================================================

FUNCTION cm_set_connection_id_src

DESCRIPTION
  Set connection ID src using the call ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern void cm_set_connection_id_src
(
  cmcall_s_type           *call_ptr,
  cm_conn_id_src_e_type   conn_id_src
);

#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */



#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmcall_proc_srv_status

DESCRIPTION
  Process service status change from no service to limited service.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_proc_srv_status(

  const cmss_s_type *ss_ptr,

  cm_ss_e_type      ss
    /* SS whose service status has changed */
);

#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */

/*===========================================================================
FUNCTION CMCALL_SUPSCMD_UPDATE_END_STATUS

DESCRIPTION
  This function sets the end_status of particular calls based on SUPS commands
  received when call is in different conversation states.

DEPENDENCIES
  Call object must be initialized
  cmcall_init().

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/


extern void cmcall_supscmd_update_end_status(cm_call_sups_type_e_type sups_type,cm_call_id_type call_id);


/*===========================================================================
FUNCTION CMCALL_CALL_IDS_ALLOCATED

DESCRIPTION
  This function returns the number of call IDs allocated.

DEPENDENCIES
  Call object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
extern byte cmcall_call_ids_allocated( void );


/*===========================================================================
FUNCTION CMCALL_CALL_IDS_ALLOCATED_PER_SS

DESCRIPTION
  This function returns the number of call IDs allocated.

DEPENDENCIES
  Call object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
extern byte cmcall_call_ids_allocated_per_ss(

       cm_ss_e_type   ss

);

/*===========================================================================
FUNCTION cmcall_call_ids_allocated_per_as_id

DESCRIPTION
  This function returns the number of call IDs allocated per asubs_id.

DEPENDENCIES
  Call object must be initialized

RETURNS
  num of call ids allocated

SIDE EFFECTS
  none

===========================================================================*/
byte cmcall_call_ids_allocated_per_as_id
(
  sys_modem_as_id_e_type   asubs_id
);

/*===========================================================================

FUNCTION cmcall_overall_call_state_per_subs

DESCRIPTION
  Returns overall call state of the subscription that ss is mappped to.

DEPENDENCIES
  none

RETURN VALUE
  overall call state

SIDE EFFECTS
  none

===========================================================================*/
extern cm_overall_call_state_e_type  cmcall_overall_call_state_per_subs
(
  cm_ss_e_type      ss
);

/*===========================================================================

FUNCTION cmcall_overall_call_state_per_ss

DESCRIPTION
  Returns overall call state for the specified ss

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_overall_call_state_e_type  cmcall_overall_call_state_per_ss
(
  cm_ss_e_type      ss
);

/*===========================================================================

FUNCTION cmcall_overall_call_state

DESCRIPTION
  This function returns the overall call state.

DEPENDENCIES
  none

RETURN VALUE
  overall call state

SIDE EFFECTS
  none

===========================================================================*/
extern cm_overall_call_state_e_type  cmcall_overall_call_state
(
    void
);


/*===========================================================================

FUNCTION cmcall_update_gsdi_and_log_call_state

DESCRIPTION
  This function updates the GSDI status about traffic/non-traffic
  status.

  This function also logs the call state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_update_gsdi_and_log_call_state(

  cm_ss_e_type      ss

);


/*===========================================================================

FUNCTION cm_number_of_allocated_calls

DESCRIPTION

  Returns number of allocated calls.

DEPENDENCIES


RETURN VALUE
  Number of allocated calls

SIDE EFFECTS
  none

===========================================================================*/
extern int cm_number_of_allocated_calls( void );


/*===========================================================================

FUNCTION cm_number_of_allocated_calls_with_type

DESCRIPTION

  Returns number of allocated calls with the given call_type.

DEPENDENCIES


RETURN VALUE
  Number of allocated calls

SIDE EFFECTS
  none

===========================================================================*/
extern int cm_number_of_allocated_calls_with_type( cm_call_type_e_type call_type );

/*===========================================================================

FUNCTION cm_number_of_allocated_calls_with_type_substate

DESCRIPTION

  Returns number of allocated calls with the given call_type and substate.

DEPENDENCIES


RETURN VALUE
  Number of allocated calls

SIDE EFFECTS
  none

===========================================================================*/
extern int cm_number_of_allocated_calls_with_type_substate( cm_call_type_e_type call_type, cm_call_incom_subst_e_type substate);

/*===========================================================================

FUNCTION cm_call_id_is_allocated

DESCRIPTION

  Returns TRUE if given call id is allocated and FALSE otherwise.

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cm_call_id_is_allocated( cm_call_id_type call_id );


/*===========================================================================

FUNCTION cmcall_ptr_is_allocated

DESCRIPTION

  Returns TRUE if given call_ptr is allocated and FALSE otherwise.

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_ptr_is_allocated
(
  const cmcall_s_type   *call_ptr
);


/*===========================================================================

FUNCTION cmcall_call_id_mode

DESCRIPTION

  Returns call mode if given call id is allocated.

DEPENDENCIES
  none

RETURN VALUE
  cm_call_mode_info_e_type

SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_mode_info_e_type cmcall_call_id_mode
(
  cm_call_id_type call_id
);



/*===========================================================================

FUNCTION cmcall_is_call_type_valid

DESCRIPTION
  Check whether a call type is valid

DEPENDENCIES
  none

RETURN VALUE
  TRUE if call type is valid, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_call_type_valid
(
  cm_call_type_e_type    call_type
    /* call type */
);



/*===========================================================================

FUNCTION cmcall_is_mobile_in_hdr_call()

DESCRIPTION
  Checks to see if any call is in HDR.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_mobile_in_hdr_call
(
  void
);

/*===========================================================================

FUNCTION cmcall_is_mobile_in_call_on_ss()

DESCRIPTION
  Checks to see if any call is in HDR.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_mobile_in_call_on_ss
(
  cm_ss_e_type      ss
);

/*===========================================================================

FUNCTION  cmcall_call_cmd_answer_para_check

DESCRIPTION
  Check parameter errors for call answer command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_cmd_err_e_type  cmcall_call_cmd_answer_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
);


/*===========================================================================

FUNCTION  cmcall_call_cmd_get_call_info_para_check

DESCRIPTION
  Check parameter errors for get call info command

DEPENDENCIES
  none

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_cmd_err_e_type  cmcall_call_cmd_get_call_info_para_check(
  const cm_call_cmd_s_type       *call_cmd_ptr
);

/*===========================================================================

FUNCTION cmcall_get_favored_tds_band_pref

DESCRIPTION
  Return the favored band preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored band preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_band_pref_e_type  cmcall_get_favored_tds_band_pref(

    cm_call_type_e_type    call_type,
        /* Call type */

    cm_band_pref_e_type    tds_band_pref
        /* The current LTE band preference */
);

/*===========================================================================

FUNCTION cmcall_get_favored_lte_band_pref

DESCRIPTION
  Return the favored band preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored band preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
extern sys_lte_band_mask_e_type  cmcall_get_favored_lte_band_pref(

    cm_call_type_e_type    call_type,
        /* Call type */

    sys_lte_band_mask_e_type     lte_band_pref
        /* The current LTE band preference */
);

/*===========================================================================

FUNCTION cmcall_get_favored_band_pref

DESCRIPTION
  Return the favored band preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored band preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_band_pref_e_type  cmcall_get_favored_band_pref(

    cm_call_type_e_type    call_type,
        /* Call type */

    cm_srv_type_e_type     srv_type,
        /* Service type */

    cm_band_pref_e_type    band_pref
        /* The current band preference */
);

/*===========================================================================

FUNCTION cmcall_get_favored_roam_pref

DESCRIPTION
  Return the favored roam preference to be used for a specified call-type,
  service-state, roam-status and a roam-preference.

DEPENDENCIES
  None.

RETURN VALUE
  The favored roam preference to be used with a specified call-type,
  service-state, roam-status and a roam-preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern cm_roam_pref_e_type  cmcall_get_favored_roam_pref(

    cm_call_type_e_type    call_type,
        /* Call which we try to originate */

    sys_sys_mode_e_type    srv_mode,
        /* The current service mode */

    sys_roam_status_e_type roam_status,
        /* The current roaming status */

    cm_roam_pref_e_type    roam_pref
        /* The current roaming preference */

);



/*===========================================================================

FUNCTION cmcall_get_favored_prl_pref

DESCRIPTION
  Return the favored prl preference to be used for a specified call-type.


DEPENDENCIES
  None.

RETURN VALUE
  The favored prl preference to be used with a specified call-type.


SIDE EFFECTS
  None.

===========================================================================*/
extern cm_prl_pref_e_type         cmcall_get_favored_prl_pref(

    cm_call_type_e_type    call_type
        /* Call which we try to originate */

);



/*===========================================================================

FUNCTION cmcall_force_pref_on_the_fly

DESCRIPTION
  Force the system selection preferences in accordance with the call-type
  and its favored mode preferences.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service.

SIDE EFFECTS
  None.

===========================================================================*/
extern dword  cmcall_force_pref_on_the_fly(

    cmcall_s_type             *call_ptr,
        /* Pointer to a call object */

    sd_ss_pref_reas_e_type    force_reason
        /* Reason for forcing the SS-preference.
        */
);



/*===========================================================================

FUNCTION cmcall_client_cmd_err

DESCRIPTION
  Notify clients of a specified call command error.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_client_cmd_err(

    const cm_call_cmd_s_type       *call_cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type    cmd_err
        /* Indicate the call command error */
);



/*===========================================================================

FUNCTION cmcall_orig_proc

DESCRIPTION
  Perform origination processing in accordance with the loaded
  call origination plan.

  During the origination process this function is being called as follows:
  1. When the call is first originated.
  2. Periodically while the origination is in process.
  3. When receiving an origination failure indication from MC, such as
     a FADE or a REORDER.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_orig_proc(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmcall_orig_start

DESCRIPTION
  Kick-start the origination process.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_orig_start(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmcall_timer_proc

DESCRIPTION
  Process call timer events

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_timer_proc(

    cm_timer_event_type    timer_event
        /* Indicate specific timer event */
);


/*===========================================================================
FUNCTION CMCALL_OFFLINE

DESCRIPTION
  This function performs offline processing for the call object

DEPENDENCIES
  Call object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_offline (
  sys_oprt_mode_e_type              oprt_mode
      /* Indicate the requested operating mode */
);


/*===========================================================================

FUNCTION cmcall_overall_call_end

DESCRIPTION
  Ends all calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmcall_overall_call_end
(
    void
);

/*===========================================================================

FUNCTION cmcall_endall_non_emerg_cs_calls

DESCRIPTION
  Ends all non-emergency calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmcall_endall_non_emerg_cs_calls
(
  cm_call_end_e_type       end_reason,

  uint32                   asubs_id_mask
);

/*===========================================================================

FUNCTION cmcall_endall_non_emerg_calls

DESCRIPTION
  Ends all non-emergency calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_endall_non_emerg_calls
(
  cm_call_end_e_type end_reason,

  boolean local_hold_calls_only
  /*Indictes if only data call needs to be ended*/
);


/*===========================================================================

FUNCTION cmcall_endall_non_emerg_calls_policy_chg

DESCRIPTION
  Ends all non-emergency calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_clean_voice_call_from_q( void );

/*===========================================================================

FUNCTION cmcall_end_all_non_emerg_non_data_calls

DESCRIPTION
  Ends all non-emergency and non-data calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void                                  cmcall_end_all_non_emerg_non_data_calls(

       uint8                          subs_id_bitmask,

       cm_call_end_e_type             end_status
);

/*===========================================================================

FUNCTION cmcall_clean_sms_call_from_q

DESCRIPTION
  Clear priority queue of SMS Calls if no SMS call in progress

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_clean_sms_call_from_q
(
  void
);

/*===========================================================================

FUNCTION cmcall_clean_cs_data_call_from_q

DESCRIPTION
  Clear priority queue of CS Data Calls if no CS Data call in progress

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_clean_cs_data_call_from_q
(
  void
);

/*===========================================================================

FUNCTION cmcall_end_each_call_with_mode

DESCRIPTION
  Ends specified calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmcall_end_each_call_with_mode(

    cm_call_mode_info_e_type call_mode_info,
    /* Given call mode info */

    cm_call_end_e_type call_end_reason
    /* Given reason for ending the call */
);

/*===========================================================================

FUNCTION cmcall_end_each_call_with_mode_per_sub

DESCRIPTION
  Ends specified calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmcall_end_each_call_with_mode_per_sub(

    cm_call_mode_info_e_type call_mode_info,
    /* Given call mode info */

    cm_call_end_e_type call_end_reason,
    /* Given reason for ending the call */

    sys_modem_as_id_e_type asubs_id
  /*SUB ID on which calls need to be ended
  ** None is wild-card, ends all*/
);


/*===========================================================================

FUNCTION cmcall_end_each_call_with_type

DESCRIPTION
  Ends calls based on call type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_end_each_call_with_type
(
  cm_call_type_e_type call_type,
  /* Given call mode info */

  cm_call_end_e_type  call_end_reason
  /* Given reason for ending the call */
);


/*===========================================================================

FUNCTION cmcall_end_each_call_with_type_per_subs

DESCRIPTION
  Ends calls based on call type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_end_each_call_with_type_per_sub
(
  cm_call_type_e_type call_type,
  /* Given call mode info */

  cm_call_end_e_type  call_end_reason,
  /* Given reason for ending the call */

  sys_modem_as_id_e_type asubs_id
  /*SUB ID on which calls need to be ended
  ** None is wild-card, ends all*/
);

/*===========================================================================

FUNCTION cmcall_end_gw_ps_calls_on_subsc_not_avail
DESCRIPTION
  Ends GW PS calls based on call type and call state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_end_gw_ps_calls_on_subsc_not_avail
(
   cm_ss_e_type ss
   /* ss on which calls needs to be ended */
);

/*===========================================================================

FUNCTION cmcall_end_ps_call_in_hold_orig_or_sr
DESCRIPTION
  Ends 1x or GW PS call which are in hold or silent redial state

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
 extern void  cmcall_end_ps_call_in_hold_orig_or_sr
 (
  cm_mode_pref_e_type mode_pref,

  cm_ss_e_type                ss

 );


/*===========================================================================

FUNCTION cmcall_act_get

DESCRIPTION
  Indicating the action to take when call origination fails (due to
  no-service, fade, reorder, intercept, or release) in accordance with
  the currently loaded call origination plan.

DEPENDENCIES
  Origination plan must have already been loaded with
  cmcall_load_orig_plan()

RETURN VALUE
  Origination action to take, such as REDIAL, FORCE_AMPS, etc.

SIDE EFFECTS
  none

===========================================================================*/
extern cmcall_act_e_type  cmcall_act_get(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
);
/*===========================================================================

FUNCTION cmcall_is_lte_only_mode
DESCRIPTION
  Process call command checking mode and band pref support

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CMD ERR

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_lte_only_mode
(
  cm_mode_pref_e_type    mode_pref,

  cm_band_pref_e_type band_pref,

  sys_lte_band_mask_e_type lte_band_pref,

  cm_band_pref_e_type tds_band_pref

);


/*===========================================================================

FUNCTION CMCALL_CLIENT_CMD_PROC

DESCRIPTION
  Invoke appropriate mode routine to process clients' call commands.

DEPENDENCIES
  Call object must have already been initialized with cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_client_cmd_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */
);


/*===========================================================================

FUNCTION cmcall_check_if_cs_voice_calls_active

DESCRIPTION
  Checks if any CS voice calls are active

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if CS voice calls active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cmcall_check_if_cs_voice_calls_active( void );


/*===========================================================================

FUNCTION cmcall_check_if_ps_call_active

DESCRIPTION
  Checks if any PS calls are active

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if PS call is active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cmcall_check_if_ps_call_active(

  cm_ss_e_type              ss

);

/*===========================================================================

FUNCTION cmcall_check_if_ps_call_active_as_id

DESCRIPTION
  Checks if any PS call is active on the specified asubs_id

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if PS call active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cmcall_check_if_ps_call_active_as_id (
        sys_modem_as_id_e_type  asubs_id
        /* sub on which to check if ps call is active */
);

/*===========================================================================

FUNCTION cmcall_get_call_info_list

DESCRIPTION
  Get a sanpshot of the state of all call objects maintained by CM.  This
  is a synchronous function.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  This function returns the state of the call objects in the info parameter.

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_get_call_info_list(

  cm_call_state_info_list_s_type  *info_list
    /* The state of the call objects maintained by CM. */

);



/*===========================================================================

FUNCTION cmcall_data_session_reg

DESCRIPTION
  Register data session callback functions with CM

  When call manager ends a data call, it consults the data session callback
  functions for the following information:

     1. Total number of bytes received and transmitted during the data call.

DEPENDENCIES
  CM must have already been initialized with cm_init_before_task_start().

RETURN VALUE
  Status of registration

SIDE EFFECTS
  none

===========================================================================*/
extern cm_data_session_status_e_type cmcall_data_session_reg(

  cm_get_data_session_total_byte_ctrs_cb_f_type
                              *data_session_total_byte_ctrs_cb_func_ptr
    /* Pointer to data session total byte counters callback function */

);

/*===========================================================================

FUNCTION cmcall_determine_ss_for_origination

DESCRIPTION

  Based on mode and hybrid preference, determine ss for call object /
  cmpref_proc_cmd_pref_sys_chgd() function.

  The assumption is that for call originations sys pref change will be sent
  to either 1X or HDR (depending on where the origination will go) but not both.
  It goes to both only when hybr_pref is toggled from on to off or vice versa.

DEPENDENCIES
  None

RETURN VALUE
  ss

SIDE EFFECTS
  None

===========================================================================*/
extern cm_ss_e_type cmcall_determine_ss_for_origination(
  cm_mode_pref_e_type    mode_pref,

  cm_hybr_pref_e_type    hybr_pref,

  cm_cc_mode_pref_e_type mode_usage,

  const cmcall_s_type   *call_ptr
);

/*===========================================================================

FUNCTION cmcall_determine_orig_srv_opt

DESCRIPTION
  Determine the best service option to be used for call originations.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE - SO found FALSE - SO not found

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_determine_orig_srv_opt(

    cmcall_s_type  *call_ptr
);

/*===========================================================================
FUNCTION cmcall_remove_mode_pref_components

DESCRIPTION

  Remove given SD mode preference component(s) from given CM mode preference.

DEPENDENCIES
  sd_misc_remove_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_mode_pref_e_type cmcall_remove_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                        int number_of_components_to_remove,
                                                        ... );

/*===========================================================================
FUNCTION cmcall_add_mode_pref_components

DESCRIPTION

  Add given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will add a given mode pref component into existing mode pref.

DEPENDENCIES
  sd_misc_add_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cmcall_add_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                    int number_of_components_to_add,
                                                    ... );


/*===========================================================================

FUNCTION cmcall_call_control_complete_cb

DESCRIPTION
  Callback function provided to a client that is performing call control for CM.
  Is called when call control is complete.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_call_control_complete_phase2_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type       *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  const void                              *user_data
    /* data user wanted passed back */

);


/*===========================================================================

FUNCTION cmcall_call_control_complete_cb

DESCRIPTION
  Callback function provided to a client that is performing call control for CM.
  Is called when call control is complete.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_call_control_complete_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type       *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  const void                              *user_data
    /* data user wanted passed back */

);

/*===========================================================================

FUNCTION cmcall_is_cc_required

DESCRIPTION
  Check whether call control is required.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_cc_required
(
  cm_cc_subscription_source_e_type          subscription_source
);

/*===========================================================================

FUNCTION cmcall_send_gstk_voice_envelope

DESCRIPTION
  Perform call control and send a command to gstk.
  GSTK will call a callback when complete.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_send_gstk_voice_envelope
(
  cm_call_id_type                              call_id,
    /* call id */

  const cm_num_s_type                         *orig_num_ptr,
   /* original calling number */

  cm_cmd_type                               *new_cmd_ptr,
   /* cmd ptr for re-queueing of command back to CM */

  cm_nc_call_control_complete_cb_f_type       *cm_call_control_complete_cb_func,

  cm_cc_subscription_source_e_type             subscription_source
);


/*===========================================================================

FUNCTION cmcall_get_ps_call_obj_in_orig_mode

DESCRIPTION
  Get the call object corresponding to the PS call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Pointer to the call object if there is a PS call in origination state.
  NULL otherwise.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern cmcall_s_type *cmcall_get_ps_call_obj_in_orig_mode( void );

#ifdef CM_GW_SUPPORTED

/*==========================================================================

FUNCTION CMCALL_MMGSDI_CARD_STATUS_IND_PROC

DESCRIPTION
  Process MMGSDI Card Status Indication, this may involve
  bringing down calls

DEPENDENCIES
  none.

RETURN VALUE
  none

SIDE EFFECTS
  none

==========================================================================*/
void cmcall_mmgsdi_card_status_ind_proc
(
  mmgsdi_events_enum_type           card_status_event,
    /* MMGSDI card status indication */

  mmgsdi_slot_id_enum_type          slot
    /* MMGSDI slot ID */

);

#endif // #ifdef CM_GW_SUPPORTED

/*===========================================================================

FUNCTION cmcall_set_favored_mode_pref_for_origination

DESCRIPTION

  Given call type, srv_type, DRS bit, compute favored mode preference and
  favored hybrid preference.

DEPENDENCIES

  None

RETURN VALUE

  TRUE if favored mode and hybrid preference were computed successfully,
  in which case they are written into favored_mode_pref_ptr and
  favored_hybr_pref_ptr, FALSE otherwise

SIDE EFFECTS

  None

===========================================================================*/
extern boolean  cmcall_set_favored_mode_pref_for_origination(
                                cm_ss_e_type           ss,
                                cm_call_type_e_type    call_type,
                                cm_srv_type_e_type     srv_type,
                                boolean                drs_bit,
                                word                   srv_opt,
                                /* In */

                                cm_mode_pref_e_type   *favored_mode_pref_ptr,
                                cm_hybr_pref_e_type   *favored_hybr_pref_ptr
                                /* Out */

);



/*===========================================================================

FUNCTION cmcall_is_there_a_call_type_ss

DESCRIPTION
  Checks is there is a requested call type in the particular SS.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of a requested call type, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_id_type cmcall_is_there_a_call_type_ss
(
  cm_ss_e_type        ss,

  cm_call_type_e_type call_type,

  cm_call_id_type     ignore_call_id

);

/*===========================================================================

FUNCTION cmcall_is_there_a_call_type_per_sub

DESCRIPTION
  Checks is there is a requested call type.on the specified sub and with
  requested call state

DEPENDENCIES
  None

RETURN VALUE
  valid call id, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_call_type_per_sub_with_state
(
  sys_modem_as_id_e_type       asubs_id,
  cm_call_type_e_type          call_type,
  cm_call_state_e_type         call_state
 );


/*===========================================================================

FUNCTION cmcall_is_there_a_call_activity_per_sub

DESCRIPTION
  Checks is there is a requested call type on the specified sub

DEPENDENCIES
  None

RETURN VALUE
  active calls bit-mask

SIDE EFFECTS
  none

===========================================================================*/
  uint64 cmcall_is_there_a_call_activity_per_sub
  (
    sys_modem_as_id_e_type       asubs_id,
    uint64                       call_type_mask
  );


/*===========================================================================

FUNCTION cmcall_is_there_cellular_call_per_sub

DESCRIPTION
  Checks is there is an active call of passed call type except VoWLAN call
  All calls on any RAT except WLAN is considered Cellular Call 

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_cellular_call_per_sub
(
  sys_modem_as_id_e_type      as_id,
  uint64                      call_type_mask
  /* Call types to search */
);


/*===========================================================================

FUNCTION cmcall_is_there_a_call_type

DESCRIPTION
  Checks is there is a requested call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_id_type cmcall_is_there_a_call_type
(

  cm_ss_e_type        ss,

  cm_call_type_e_type call_type,

  cm_call_id_type     ignore_call_id

);

/*===========================================================================
FUNCTION cmcall_is_there_a_call_activity

DESCRIPTION
  Checks if there is a any call activity (voice/emergency/CS_DATA/PS_DATA/SMS call)
  in UE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if any call is active

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_there_a_call_activity
(
  cm_ss_e_type        ss
);


/*===========================================================================

FUNCTION cmcall_is_there_a_call_type_per_sub

DESCRIPTION
  Checks is there is a requested call type.on the specified sub

DEPENDENCIES
  None

RETURN VALUE
  valid call idl, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_call_type_per_sub
(
  sys_modem_as_id_e_type       asubs_id,
  cm_call_type_e_type          call_type,
  cm_ss_e_type                *cm_ss
);

 /*===========================================================================

 FUNCTION cmcall_is_active_call_other_than_ss

 DESCRIPTION
   Checks is there is a requested call type. active on ss other than passed

 DEPENDENCIES
   None

 RETURN VALUE
   valid call id of requested call, CM_CALL_ID_INVALID otherwise

 SIDE EFFECTS
   none

 ===========================================================================*/
 cm_call_id_type cmcall_is_active_call_other_than_ss
 (
   cm_ss_e_type        ss,

   cm_call_type_e_type call_type

 );

/*===========================================================================

FUNCTION cmcall_find_active_cs_call_per_subs

DESCRIPTION
  Checks is there is active voice/emergency/vt call per sub

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_id_type cmcall_find_active_cs_call_per_subs(

   sys_modem_as_id_e_type         asubs_id
   /* subs on which to find active CS call */

 );

 #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
/*===========================================================================

FUNCTION cmcall_is_there_csfb_call

DESCRIPTION
  Checks is there is a  requested CSFB call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_csfb_call
(
  cm_ss_e_type        ss,

  cm_call_csfb_e_type csfb_type,

  cm_call_id_type     ignore_call_id

);

/*===========================================================================

FUNCTION cmcall_are_there_csfb_call_preferences

DESCRIPTION
  Checks whether CM priority quieue contains any CSFB call preferences.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If CSFB call exist in Priority queue.
  FALSE  If there is no CSFB call.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_are_there_csfb_call_preferences
(
  cm_ss_e_type           ss
);

#endif /* (FEATURE_3GPP_CSFB) || (FEATURE_LTE_TO_1X)*/
/*===========================================================================

FUNCTION  cmcall_check_call_type_in_call_state

DESCRIPTION
  Checks if there is a call of the given type and in the given state.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a call can be found with the requested parameters.

SIDE EFFECTS
  none

===========================================================================*/

extern boolean cmcall_check_call_type_in_call_state (

                  cm_call_type_e_type   call_type,
                  /* Given call type */

                  cm_call_state_e_type   call_state
                  /* Given call state */
);

/*===========================================================================

FUNCTION  cmcall_check_call_type_in_call_state_ss

DESCRIPTION
  Checks if there is a call of the given type and in the given state
  per stack.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a call can be found with the requested parameters.

SIDE EFFECTS
  none

===========================================================================*/

extern boolean cmcall_check_call_type_in_call_state_ss (

                  cm_call_type_e_type   call_type,
                  /* Given call type */

                  cm_call_state_e_type   call_state,
                  /* Given call state */

                  cm_ss_e_type    ss
                  /* Given call on stack */
);



/*===========================================================================

FUNCTION  cmcall_check_each_call_in_call_state

DESCRIPTION
  Checks if each call is in the given state.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if each call can be found in the requested state.

SIDE EFFECTS
  none

===========================================================================*/

extern boolean cmcall_check_each_call_in_call_state (

                  cm_call_state_e_type   call_state
                  /* Given call state */
);

/*===========================================================================

FUNCTION  cmcall_check_each_call_in_call_state_ss

DESCRIPTION
  Checks if each call is in the given state for the specified asubs_id.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if each call can be found in the requested state.

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_each_call_in_call_state_ss (

        cm_call_state_e_type   call_state,
        /* Given call state */

        sys_modem_as_id_e_type asubs_id
);

/*===========================================================================

FUNCTION cmcall_update_call_end_info

DESCRIPTION
  This function updates the call object information related to the
  CM_CALL_EVENT_END_REQ and CM_CALL_EVENT_END call events.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_update_call_end_info(

  cmcall_s_type           *call_ptr
      /* pointer to a call object. */
);

/*===========================================================================

FUNCTION  cmcall_check_for_call_state_on_ss

DESCRIPTION
  Checks if there is a call in a particular state on a given ss.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if there is a call in a particular state on a specified Serving system

SIDE EFFECTS
  none

===========================================================================*/

extern boolean cmcall_check_for_call_state_on_ss (
                  cm_ss_e_type    ss,
                  /* Given call type */

                  cm_call_state_e_type   call_state
                  /* Given call state  */
);

/*===========================================================================

FUNCTION cmcall_reset

DESCRIPTION
  Reset common call fields to default values reflecting a call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmcall_is_call_id_in_range
DESCRIPTION
  Checks if a given Call Id is in a valid range from 0 to CALL_ID_MAX.
DEPENDENCIES
  Call Id must have already been alloacated.
RETURN VALUE
  TRUE if valid.
  FALSE if not valid.
SIDE EFFECTS
  None
===========================================================================*/
extern boolean cmcall_is_call_id_in_range(

                    cm_call_id_type call_id
);

/*===========================================================================

FUNCTION cmcall_set_call_object

DESCRIPTION

  Sets the call type.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern void cmcall_set_type
(
  cmcall_s_type             *call_ptr,
  cm_call_type_e_type       call_type
);


/*===========================================================================

FUNCTION cmcall_is_a_hdr_call

DESCRIPTION
  Checks whether there is an HDR call up.

DEPENDENCIES
  None

RETURN VALUE
  Valid call id of a HDR call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_id_type cmcall_is_hdr_call
(
  void
);


/*===========================================================================
FUNCTION CMCALL_IS_HYBR_HDR_CALL

DESCRIPTION
  Check whether this is a hybrid HDR call

DEPENDENCIES
  Call object must be initialized

RETURNS
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_hybr_hdr_call (cm_call_id_type *call_id_ptr);


/*===========================================================================
FUNCTION CMCALL_IS_HYBR_HDR_ORIG

DESCRIPTION
  Check whether there is a hybrid HDR call origination

DEPENDENCIES
  Call object must be initialized

RETURNS
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
extern cmcall_s_type* cmcall_is_hybr_hdr_orig (void);

/*===========================================================================

FUNCTION cmcall_is_there_a_cdma_call

DESCRIPTION
  Checks is there is a call object allocated for CDMA.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_id_type cmcall_is_there_a_cdma_call (void);


/*===========================================================================

FUNCTION cmcall_is_there_a_cdma_call

DESCRIPTION
  Checks is there is a call object allocated for HVOLTE
  call , which is waiting for ESR response from NAS

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_id_type cmcall_is_there_hvolte_call_waiting_for_esr_rsp (void);

/*===========================================================================

FUNCTION cmcall_is_there_a_cdma_call_in_conv

DESCRIPTION
  Checks is there is a call object allocated for CDMA.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_id_type cmcall_is_there_a_cdma_call_in_conv (void);


/*===========================================================================

FUNCTION cmcall_generate_connection_id

DESCRIPTION
  This function generates a Connection Id and sets the connection Id and its
  Source in the call Objet.

  Please note that Connection Id makes sense only for the GSM/WCDMA modes.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern boolean cmcall_generate_connection_id
(
  cmcall_s_type             *call_ptr
);


/*===========================================================================

FUNCTION cmcall_set_cdma_cc_result

DESCRIPTION
  This function sets the Call Control (CC) result to the specifid value for a
  given call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern void cmcall_set_cdma_cc_result
(
  cmcall_s_type             *call_ptr,
  cm_cc_result_e_type       cc_result
);


/*===========================================================================

FUNCTION cmcall_set_gw_cc_result

DESCRIPTION
  This function sets the Call Control (CC) result to the specifid value for a
  given call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern void cmcall_set_gw_cc_result
(
  cmcall_s_type             *call_ptr,
  cm_cc_result_e_type       cc_result
);


/*===========================================================================

FUNCTION cmcall_obj_iterator_init

DESCRIPTION
  This function initializes the Call Object Iterator.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object Iterator Current Index will be modified.

===========================================================================*/
extern void cmcall_obj_iterator_init
(
  cm_iterator_type *iterator_ptr
);


/*===========================================================================

FUNCTION cmcall_obj_get_next

DESCRIPTION
  This function gets the next call object in the collection of call objects.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Pointer to Call Object structure.

SIDE EFFECTS
  Call Object Iterator current index will be modified.

===========================================================================*/
extern cmcall_s_type* cmcall_obj_get_next
(
  cm_iterator_type *iterator_ptr
);


/*===========================================================================

FUNCTION cmcall_obj_get_next_voip_silent_redial_call

DESCRIPTION
  This function gets the next call object in the collection of call objects
  which is VOIP and in silent redial state.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Pointer to Call Object structure.

SIDE EFFECTS
  Call Object Iterator current index will be modified.

===========================================================================*/
extern cmcall_s_type*     cmcall_obj_get_next_voip_silent_redial_call(

    cm_iterator_type      *iterator_ptr
);

/*===========================================================================

FUNCTION cmcall_set_call_direction

DESCRIPTION

  Sets the call direction.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern void cmcall_set_call_direction
(
  cmcall_s_type             *call_ptr,
    /* Pointer to a call object */

  cm_call_direction_e_type   call_direction
    /* Direction of the call MO or MT*/
);

/*===========================================================================

FUNCTION cmcall_count_calls_with_call_type

DESCRIPTION

  Counts total number of calls of a particular call type


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern int cmcall_count_calls_with_call_type (

  cm_call_type_e_type call_type
    /* call type */
);

/*===========================================================================
FUNCTION cmcall_count_calls_with_call_type_per_ss

DESCRIPTION
  Counts total number of calls of a particular call type

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern int cmcall_count_calls_with_call_type_per_ss (

    cm_call_type_e_type call_type,
      /* call type */

    cm_ss_e_type   ss
     /* ss for which calls are to be counted */
);

/*===========================================================================
FUNCTION cmcall_count_calls_with_call_type_per_as_id

DESCRIPTION
  Counts total number of calls of a particular call type

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
int cmcall_count_calls_with_call_type_per_as_id (

    cm_call_type_e_type call_type,
      /* call type */

    sys_modem_as_id_e_type   asubs_id
     /* sub for which calls are to be counted */
);

/*===========================================================================
FUNCTION cmcall_count_primary_pdp_calls

DESCRIPTION

  Counts total number of primary pdp calls


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of primary pdp calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern int cmcall_count_primary_pdp_calls (

  cm_ss_e_type      ss
    /* stack whose calls need to be counted */

);


/*===========================================================================

FUNCTION cmcall_get_call_obj_mo_in_orig

DESCRIPTION

  Returns call ptr of first call of a particular type in
  origination state.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  call pointer for the call type in origination state or NULL if none

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
extern cmcall_s_type *cmcall_get_call_obj_mo_in_orig (

  cm_call_type_e_type call_type
    /* call type */
);

/*===========================================================================

FUNCTION cmcall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.
  Copy fields that are common accross modes of operation.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_info_get(

    const cmcall_s_type          *call_ptr,
        /* Point at call object */

    cm_mm_call_info_s_type    *call_info_ptr
        /* Copy call state info into this buffer */
);


#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmcall_srvcc_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer
  during SRVCC_HO

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_srvcc_info_get(

    const cmcall_s_type          *call_ptr,
        /* Point at call object */

    cm_mm_srvcc_call_info_s_type    *srvcc_call_info_ptr
        /* Copy call state info into this buffer */
);

#endif

/*===========================================================================

FUNCTION cmcall_remove_mode_and_force

DESCRIPTION
  Select non-HDR for the next origination

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, the no servie timer is set to
  0.

  Return TRUE, the new mode preference is changed.
  Otherwise, return FALSE.

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmcall_remove_mode_and_force(

  cmcall_s_type          *call_ptr,
        /* pointer to a call object */

  sd_ss_mode_pref_e_type mode_pref_to_remove,
        /* CM mode pref to be removed */

  dword                  *no_srv_time_ptr
        /* Pointer to no service time */

);

/*===========================================================================

FUNCTION cmcall_redial_after_hard_failure

DESCRIPTION


DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, the no servie timer is set to
  0.

  Return TRUE, the new mode preference is changed.
  Otherwise, return FALSE.

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmcall_redial_after_hard_failure(

  cmcall_s_type          *call_ptr,

  dword                  *no_srv_time_ptr
        /* Pointer to no service time */

);



/*===========================================================================

FUNCTION  cmcall_is_active_ip_app

DESCRIPTION
  Determines if there is a active VOIP app for a given System.

DEPENDENCIES
  none
RETURN VALUE
  BOOLEAN
  TRUE  - If VOIP app is active.
  FALSE - Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_active_ip_app
(
  sys_sys_mode_e_type  mode,
    /* system mode than can be serviced by app */

  cm_call_type_e_type  call_type
    /* call type that can be serviced by app */
);

/*===========================================================================

FUNCTION  cmcall_cmd_orig_para_check

DESCRIPTION
  Check parameter errors for call origination command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmcall_cmd_orig_para_check
(
  cm_call_cmd_s_type       *call_cmd_ptr


);


/*===========================================================================

FUNCTION cmcall_process_orig_mode

DESCRIPTION
  Process call command pertaining to CDMA or GW Orig Mode.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CMD ERR

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmcall_process_orig_mode
(
  cm_cc_mode_pref_e_type    mode_usage,
  cmcall_s_type            *call_ptr

);

/*===========================================================================

FUNCTION cmcall_cmd_copy_fields

DESCRIPTION

  Copy the active fields of a call command into a call object.
  Copy common fields and then call cmxcall_cmd_copy_fields() and
  cmwcall_cmd_copy_fields() to copy mode specific fields

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

);


/*===========================================================================

FUNCTION cmcall_is_hdr_waiting_for_end_req

DESCRIPTION

  Checks if any call is waiting to received END_CNF from HDR stack.
DEPENDENCIES

RETURN VALUE
  TRUE: If there exist a call for which we are waiting for END_CNF from
        HDR stack.
  FALSE: Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_hdr_waiting_for_end_req
(
  void
);

/*===========================================================================

FUNCTION cmcall_count_on_ss

DESCRIPTION

  Returns number of calls on a given SS.

DEPENDENCIES


RETURN VALUE
  Number of calls on a given SS.

SIDE EFFECTS
  none

===========================================================================*/
int cmcall_count_on_ss
(
  cm_ss_e_type    ss
);

/*===========================================================================

FUNCTION cmcall_update_orig_mode

DESCRIPTION

  Updates the origmode based on CSFB type

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/

void cmcall_update_orig_mode (cmcall_s_type  *call_ptr);


/*===========================================================================

FUNCTION
void cmcall_update_ok_to_orig


DESCRIPTION

  Updates the ok to orig value

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/

void cmcall_update_ok_to_orig
(
   cmcall_s_type   *call_ptr,
   /* Call pointer */

   boolean ok_to_orig_value

);



/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_cs

DESCRIPTION
  Checks if there are no calls in gw cs domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in cs domain
  FALSE - There is/are calls in cs domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_cs(

  cm_ss_e_type      ss

);


/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_cs_per_subs

DESCRIPTION
  Checks if there are no calls in gw cs domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in cs domain
  FALSE - There is/are calls in cs domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_cs_per_subs(

  cm_ss_e_type      ss,

  sys_modem_as_id_e_type asubs_id

);

/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_ps

DESCRIPTION
  Checks if there are no calls in gw ps domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in ps domain
  FALSE - There is/are calls in ps domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_ps (

  cm_ss_e_type    ss

);

/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_ps

DESCRIPTION
  Checks if there are no calls in gw ps domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in ps domain
  FALSE - There is/are calls in ps domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_ps_per_subs (

  cm_ss_e_type    ss,

  sys_modem_as_id_e_type asubs_id

);

/*===========================================================================

FUNCTION cmcall_get_num_call_in_call_mode

DESCRIPTION
  Get number of calls in the specified call_mode

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  uint8 - num of calls

SIDE EFFECTS
  none

===========================================================================*/
uint8 cmcall_get_num_call_in_call_mode (
      cm_call_mode_info_e_type  call_mode
);


/*===========================================================================

FUNCTION cmcall_is_no_call_in_lte_ps

DESCRIPTION
  Checks if there are no calls in LTE ps domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in ps domain
  FALSE - There is/are calls in ps domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_lte_ps (void);



/*===========================================================================

FUNCTION cmcall_is_active_on_mode

DESCRIPTION
  Checks if there is a call on a given mode.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  FASLE  - No calls
  TRUE   - Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_active_on_mode
(
  sys_sys_mode_e_type  sys_mode
);

/*===========================================================================

FUNCTION cmcall_check_voice_emerg_in_orig

DESCRIPTION
  Checks if current call is voice or emergency in origination

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE : if voice or emergency call is in origination
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmcall_check_voice_emerg_in_orig(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
);


/*===========================================================================

FUNCTION cmcall_check_if_csdata_and_voice

DESCRIPTION
  Checks if one of the call type is CM_CALL_TYPE_VOICE and the other
  CM_CALL_TYPE_CS_DATA

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE : one is voice and the other is CS_DATA
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmcall_check_if_csdata_and_voice(

    cm_call_type_e_type  call_type_1,
        /* call type */

    cm_call_type_e_type  call_type_2
        /* call type */
);

/*===========================================================================
FUNCTION cmcall_hybr_pref_was_toggled_for_origination

DESCRIPTION

  Determine whether hybrid preference was toggled for origination.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybrid preference was toggled for origination,
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern  boolean cmcall_hybr_pref_was_toggled_for_origination(

     cm_hybr_pref_e_type     requested_hybr_pref,
     /* Hybrid preference requested for the next call */

     sys_modem_as_id_e_type  asubs_id
      /* as id */
);


/*===========================================================================
FUNCTION cmcall_misc_is_mode_pref

DESCRIPTION

Check whether specified fav_mode_pref_1 complies with fav_mode_pref_2

DEPENDENCIES

   None

RETURN VALUE

  TRUE if so,
  FALSE otherwise and when any of mapping of CM enum to SD enum failed

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_misc_is_mode_pref(

   cm_mode_pref_e_type fav_mode_pref_1,

   cm_mode_pref_e_type fav_mode_pref_2

);


/*===========================================================================

FUNCTION cmcall_check_if_no_srv
DESCRIPTION
  Function to check if we both the HDR and the main instance have no service

DEPENDENCIES

RETURN VALUE
  True  If both HDR and 1X do not have service
  False Otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_check_if_no_srv(boolean check_hdr);

/*===========================================================================

FUNCTION  cmcall_get_call_obj_in_call_state

DESCRIPTION
  Returns the call obj in call_state.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj in call_state if present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/

extern cmcall_s_type* cmcall_get_call_obj_in_call_state (

                  cm_call_state_e_type   call_state
                  /* Given call state */
);

#ifdef FEATURE_OTASP
/*===========================================================================

FUNCTION  cmcall_get_otasp_call_obj

DESCRIPTION
  Returns the OTASP/OTAPA call obj. There can be only one call obj of this type

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj of type OTASP/OTAPA if present
  NULL otherwise

SIDE EFFECTS
  none
===========================================================================*/

extern cmcall_s_type* cmcall_get_otasp_call_obj(void);

#endif // FEATURE_OTASP
/*===========================================================================

FUNCTION cmcall_check_if_gw_cs_call_active

DESCRIPTION
  Checks if there is a cs call in given call state. CM_CALL_STATE_NONE
  checks for call in any state.

  NOTE:- Typically there will not be a call object with call type SUPS.
  cmcall_is_gw_cs_call_type() checks SUPS call type for completeness.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if CS voice calls active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
extern boolean cmcall_check_if_gw_cs_call_active (

  cm_call_state_e_type      call_state,
    /* Call state for the queried call */

  cm_ss_e_type              ss

);

/*===========================================================================

FUNCTION cmcall_check_if_end_status_to_propagate

DESCRIPTION
  Checks if the current end_status is the one sent to inform FALLBACK_TO_CS to Clients.

DEPENDENCIES

RETURN VALUE
  TRUE: if event has to be sent
  FALSE: otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_check_if_end_status_to_propagate(

       cm_call_end_e_type      end_status

);


/*===========================================================================

FUNCTION cmcall_is_gw_cs_call_type

DESCRIPTION
  Checks if the call type is used on cs domain

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Call type used on gw cs domain
  FALSE - Call type not being used on gw cs domain

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_gw_cs_call_type
(
  cm_call_type_e_type call_type
);

/*===========================================================================

FUNCTION cmcall_get_sec_pdp_callid

DESCRIPTION
  Gives the call id of a secondary PDP call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  call id of secondary pdp call.

SIDE EFFECTS
  none

===========================================================================*/

extern cm_call_id_type cmcall_get_sec_pdp_callid (void);

/*===========================================================================

FUNCTION cmcall_is_ipcall_over_mode_poss

DESCRIPTION

  Returns TRUE if given call can be placed on the mode as a IP call.

DEPENDENCIES

   None

RETURN VALUE

  TRUE  -  Call can be placed as a IP call on given mode
  FALSE -  Call cannot be placed as a IP call.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_ipcall_over_mode_poss(

  sys_sys_mode_e_type              sys_mode,
     /* Specific technology - WCDMA */

  const cmcall_s_type              *call_ptr
    /* Call object */
);


/*===========================================================================

FUNCTION cmcall_det_ss_from_sys_mode

DESCRIPTION
  Determines ss instance on which the given sys_mode is present

DEPENDENCIES
  None.

RETURN VALUE
  SS instance

SIDE EFFECTS
  none

===========================================================================*/
extern cm_ss_e_type  cmcall_det_ss_from_sys_mode(

    cmcall_s_type             *call_ptr
        /* sys mode. WLAN, CDMA ...*/
);

/*===========================================================================
FUNCTION cmcall_get_prim_with_groupid

DESCRIPTION
  Gives the conn id of primary pdp with given group id.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Connection id of primary pdp. Unknown id if no primary pdp is found.

SIDE EFFECTS
  none

===========================================================================*/
extern byte cmcall_get_prim_with_groupid  (

  byte pdp_group_id
    /* Group id of primary pdp */
);

/*===========================================================================

FUNCTION cmcall_det_sys_mode_from_ss

DESCRIPTION
  Returns sys mode that is currently present in the given ss

DEPENDENCIES
  None.

RETURN VALUE
  SS instance

SIDE EFFECTS
  none

===========================================================================*/
extern sys_sys_mode_e_type cmcall_det_sys_mode_from_ss(

    cm_ss_e_type   ss
      /* system instance, MAIN, HYBR_HDR ... */
);


/*===========================================================================
FUNCTION cmcall_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern boolean  cmcall_dem_is_apps_wakeup_from_suspend_event(

    cm_call_event_e_type   call_event
        /* Call event */
);


/*===========================================================================

FUNCTION cmcall_get_voip_call_id

DESCRIPTION
  Return the call id of the existing VoIP call, if present
  Else, return invalid call id

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Return the call id of the existing VoIP call, if present
  Else, return invalid call id


SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_id_type cmcall_get_voip_call_id( void );

/*===========================================================================

FUNCTION cmcall_end_active_1x_datacalls_no_force

DESCRIPTION
 Ends all active data calls on 1X but does not force the prefs to SD

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj if CDMA data call present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_end_active_1x_datacalls_no_force();

/*===========================================================================

FUNCTION  cmcall_end_active_1x_datacalls

DESCRIPTION
 Ends all active data calls on 1X

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj if CDMA data call present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/

extern boolean cmcall_end_active_1x_datacalls (boolean is_force);

/*===========================================================================

FUNCTION  cmcall_end_active_HDR_datacalls

DESCRIPTION
 Ends all active data calls on HDr

DEPENDENCIES
  None

RETURN VALUE
  Returns true if hdr data call present
  False otherwise

SIDE EFFECTS
  none

===========================================================================*/

extern boolean cmcall_end_active_HDR_datacalls (void);

/*===========================================================================

FUNCTION cmcall_get_subs_id_with_call_type

DESCRIPTION
  Return the subscription-id of the existing call of particular call_type
  Else, return subscription-id none

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Return the subscription-id of the existing call of particular call_type
  Else, return subscription-id none

SIDE EFFECTS
  None

===========================================================================*/
extern sys_modem_as_id_e_type cmcall_get_subs_id_with_call_type (

       cm_call_type_e_type   call_type
         /* Given call type */
);
/*===========================================================================

FUNCTION cmcall_get_favored_srv_domain_pref

DESCRIPTION
  Return the favored service domain preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored service domain preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_srv_domain_pref_e_type        cmcall_get_favored_srv_domain_pref(

  const cmcall_s_type            *call_ptr
        /* Pointer to the  call object*/
);


/*===========================================================================
FUNCTION cmcall_misc_get_common_mode_pref

DESCRIPTION

  Gets the common SD mode preference component(s) from given CM mode preference.

  Example:

  The below will add all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_misc_get_common_mode_pref(
                                           CM_MODE_PREF_AUTOMATIC,
                                           3,// because 6 components follow
                                           CM_MODE_PREF_CDMA_ONLY,
                                           CM_MODE_PREF_CDMA_HDR_ONLY,
                                           CM_MODE_PREF_DIGITAL_ONLY,
                                           );

DEPENDENCIES
  sd_misc_get_common_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_mode_pref_e_type cmcall_misc_get_common_mode_pref
(
  cm_mode_pref_e_type cm_mode_pref,
    /* CM mode pref
    */

  int number_of_components,
  ...
);

/*===========================================================================

FUNCTION cmcall_unpack_subaddress

DESCRIPTION
  This function takes, subaddress in the format defined in 3GPP 24.008 spec
  in the section 10.5.4.8, as input and unpack it in the cm_subaddress_s_type
  strcuture format.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_unpack_subaddress(

    cm_subaddress_s_type        *cm_subaddress_ptr,
        /* called party subaddress in struct format */

    const gstk_subaddress_type *gstk_subaddress_ptr
        /* gstk subaddress in stream format */

);

/*===========================================================================

FUNCTION cmcall_pack_subaddress

DESCRIPTION
  This function takes, called party subaddress in the cm_subaddress_s_type
  struct format, as input and packs it in the format defined in 3GPP 24.008
  spec in the section 10.5.4.8.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_pack_subaddress(

    gstk_subaddress_type *gstk_subaddress_ptr,
        /* gstk subaddress in stream format */

    const cm_subaddress_s_type    *cm_subaddress_struct_ptr
        /* called party subaddress in struture format */

);

/*===========================================================================
FUNCTION CMCALL_RESERVE_SM_CONNECTION_ID

DESCRIPTION
  This function is called by the Call Manager in order to reserve a
  connection ID to be passed to SM for a Packet Data call. Each PDP context
  is identified by this connection Id during Primitive exchange.

DEPENDENCIES
  cmcall_init() must have been called before using this function.

  The transmission of primitives between the CM and SM depend on this
  connection ID.

RETURN VALUE

  Returns TRUE if a connection Id is reserved, otherwise returns FALSE.

  connection_id

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmcall_reserve_sm_connection_id(byte* connection_id,
                                               sys_modem_as_id_e_type as_id);

/*===========================================================================
FUNCTION CMCALL_UNRESERVE_SM_CONNECTION_ID

DESCRIPTION
  This function is called by Call Manager in order to unreserve a
  previously reserved connection ID. This will allow CM only to unreserve
  connection for MO Inactive call.

DEPENDENCIES
  cmcall_init() must have been previously called.

  Call Manager uses this function to unreserve a previously
  reserved connection_id.

RETURN VALUE

  Returns TRUE if a connection ID is unreserved, otherwise returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmcall_unreserve_sm_connection_id(byte connection_id,
                                                 sys_modem_as_id_e_type as_id);

/*===========================================================================
FUNCTION CMCALL_IS_SM_CONNECTION_ID_IN_USE

DESCRIPTION
  Accessor function to determine if a connection ID is currently in use.
  Connection ID represents a PDP context and is used between CM and SM.

DEPENDENCIES
  cmcall_init() must have previously been called.

RETURN VALUE

  Returns TRUE if a connection ID is in use, otherwise returns FALSE.
  Returns FALSE for invalid/unknown connection IDs.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmcall_is_sm_connection_id_in_use(byte connection_id,
                                                 sys_modem_as_id_e_type as_id);


#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
/*===========================================================================
FUNCTION CMCALL_MARK_SM_CONNECTION_ID_RESERVED

DESCRIPTION
  This function is called by the Call Manager in order to mark a SM connection
  Id for a Packet Data call when LTE call is originated. When the PS data call
  originated while active rat is LTE, we receives connection_id from NW and we
  should use this connection id instead of generating our own.

DEPENDENCIES

  The transmission of primitives between the CM and SM depend on this
  connection id.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmcall_mark_sm_connection_id_reserved(byte connection_id,
                                                  sys_modem_as_id_e_type as_id);

/*===========================================================================
FUNCTION CM_PRINT_CONNECTION_ID

DESCRIPTION
  This function is utility funtion to print out all GW connection_ids

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void cm_print_connection_ids(void);
#endif


/*===========================================================================

FUNCTION cmcall_is_present_ip_call

DESCRIPTION
  Check whether an VOLTE/IP call present without
  check for call state or substate

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_present_ip_call(void);
/*===========================================================================

/*===========================================================================

FUNCTION cmcall_vowlan_call_not_in_online_oprt_mode()

DESCRIPTION
  Checks if call is over vowlan and oprt_mode is not online

DEPENDENCIES
  None

RETURN VALUE
  TRUE if conditions are met

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_vowlan_call_not_in_online_oprt_mode(cmcall_s_type   *call_ptr);


/*===========================================================================

FUNCTION cmcall_is_there_volte_call_sub

DESCRIPTION
  Checks is there is a  requested VOLTE call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_volte_call_sub
(
   cm_call_type_e_type         call_type,
        /* Call type to search */
   sys_modem_as_id_e_type      as_id
);

/*===========================================================================
FUNCTION cmcall_get_active_srvcc_volte_call

DESCRIPTION
  Find VOLTE  call which is in active CONV state for legacy SRVCC
  Otherwise CONV/INCOMING/Outgoing ORIG

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_get_active_srvcc_volte_call
(
   cm_call_type_mask_type         call_type_mask,
        /* Call type to search */
   sys_modem_as_id_e_type         asubs_id
);

/*===========================================================================

FUNCTION cmcall_is_there_volte_call_obj

DESCRIPTION
  Checks if there is a call object

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_volte_call_obj
(
   cm_call_type_mask_type         call_type_mask
        /* Call type to search */
);

/*===========================================================================

FUNCTION cmcall_get_active_volte_call

DESCRIPTION
  Find VOLTe call which is in active state

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_get_active_volte_call
(
   cm_call_type_mask_type         call_type_mask
        /* Call type to search */
);
/*===========================================================================
FUNCTION cm_reserve_connection_id

DESCRIPTION
  This function is called by the Call Manager in order to get a connection
  Id for a CS call.

DEPENDENCIES

  The transmission of primitives between the CM and MN depend on this
  connection id.

RETURN VALUE

  Returns connection id if a connection Id is found, otherwise returns
  UNKNOWN_CONNECTION_ID.

SIDE EFFECTS
  None.

===========================================================================*/
extern byte cm_reserve_connection_id(sys_modem_as_id_e_type);
/*===========================================================================
FUNCTION cm_unreserve_connection_id

DESCRIPTION
  This function is called by the Call Manager in order to unreserve a
  previously secured MN connection ID. This will allow CM only to unreserve
  connection for MO Inactive call.

DEPENDENCIES

  Call Manager must use this function only to unserve a previously
  reserved connection_id from MN.

RETURN VALUE

  Returns TRUE if a connection Id is unreserved, otherwise returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cm_unreserve_connection_id(byte, sys_modem_as_id_e_type);

/*===========================================================================
FUNCTION cm_is_connection_id_in_use

DESCRIPTION
  Accessor function to determine if a connection ID is currently in use.

DEPENDENCIES

  cmcall_init() must have previously been called.

RETURN VALUE

  Returns TRUE if a connection Id is in use, otherwise returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cm_is_connection_id_in_use
(
  byte connection_id,

  sys_modem_as_id_e_type asubs_id
    /**< Indicates the asubs_id */
);


/*===========================================================================

FUNCTION cmcall_is_volte_restricted_call

DESCRIPTION
  Checks if the call is VOLTE restricted.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_volte_restricted_call
(
   const cmcall_s_type   *call_ptr
        /* Call object pointer */
);

/*===========================================================================

FUNCTION cmvoltecall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. replies to  call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#if defined (FEATURE_CM_LTE) && defined(FEATURE_IP_CALL)
void cmvoltecall_rpt_proc(

  const cm_esm_cmd_u_type   *cm_rpt_ptr
    /* Pointer to LL report */
);
#endif


/*===========================================================================

FUNCTION cmcall_find_active_voice_call

DESCRIPTION
  Checks is there is active voice call

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_voice_call(void);

/*===========================================================================

FUNCTION cmcall_find_active_sms_call

DESCRIPTION
  Checks is there is active sms call

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_sms_call(void);

/*===========================================================================

FUNCTION cmcall_find_active_cs_data_call

DESCRIPTION
  Checks is there is active CS Data call

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_cs_data_call(void);

/*===========================================================================

FUNCTION cmcall_find_active_voice_call

DESCRIPTION
  Checks is there is active voice call

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_voice_call_1(

    sys_modem_as_id_e_type                      as_id
    /* Sub id value */
);

/*===========================================================================

FUNCTION cmcall_unreserve_connection_id

DESCRIPTION
   Unreserve SM/MN connection ids

DEPENDENCIES
  Call object must have already been initialized with cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(715, call_ptr) */
extern void cmcall_unreserve_connection_id(

    const cmcall_s_type                  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmcall_ssac_process_lte_ind

DESCRIPTION
  Process the cell info related to SSAC parameters from LTE RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmcall_ssac_process_lte_ind(
  const lte_rrc_service_ind_s   *rrc_srv_ind_ptr
  /* ptr to message received */
);

/*===========================================================================

FUNCTION cmcall_ssac_process_lte_ind

DESCRIPTION
  Process SSAC parameters update from LTE RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmcall_read_barring_params
(
  lte_rrc_barring_update_ind_s  *rrc_barring_params
);


/*===========================================================================

FUNCTION cmcall_process_ssac_timer

DESCRIPTION
  This function should be called to process ssac timers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmcall_process_ssac_timer(

  cm_timer_event_type    timer_event
  /* Indicate specific timer event.
   ** Note that currently timer event is not being used
   */

);

/*===========================================================================

  FUNCTION cmcall_ssac_process_srv_update

  DESCRIPTION
    Reset ssac timers if LTE has no srv but GWT is in service.

  DEPENDENCIES

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
void cmcall_ssac_process_srv_update(void);

cm_call_cmd_err_e_type    cmcall_sglte_determine_ss_per_call_type(
    sys_modem_as_id_e_type as_id,
    cm_call_type_e_type   call_type,
    cm_ss_e_type         *ss
);


sd_ss_srv_domain_pref_e_type cmcall_get_domain_for_call_type(
    cm_call_type_e_type      call_type
);

boolean cmcall_is_allowed_per_cap(
    cm_ss_e_type          ss,
    cm_mode_pref_e_type  *mode_pref_ptr
);

void cmcall_transfer_cs_calls(
   cm_ss_e_type            source_ss,
   cm_ss_e_type            target_ss,
   boolean                 is_act_merge
);


/*===========================================================================

FUNCTION cmcall_overall_non_ps_call_state_per_subs

DESCRIPTION
  This function returns the overall call state except ps call.

DEPENDENCIES
  none

RETURN VALUE
  overall call state

SIDE EFFECTS
  none

===========================================================================*/
extern cm_overall_call_state_e_type  cmcall_overall_non_ps_call_state_per_subs
(
  cm_ss_e_type      ss
);

/*===========================================================================

FUNCTION cmcall_end_conucrrent_volte_call

DESCRIPTION

  Handles concurrent volte call when e911 call is originated,
  based on the NV, VOLTE call is either terminated or will be on hold.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#if defined (FEATURE_CM_LTE) && defined(FEATURE_IP_CALL)
void cmcall_end_concurrent_volte_call(
  sys_modem_as_id_e_type asubs_id
);
#endif

/*===========================================================================

FUNCTION cmcall_force_mode_and_send_event

DESCRIPTION
  Check if the call is being dialed on 1X , though LTE is active currently

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_force_mode_and_send_event(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
);


/*===========================================================================

FUNCTION cmcall_sent_on_1x_while_lte_active_in_single_mode

DESCRIPTION
  Check if the call is being dialed on 1X , though LTE is active currently

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_sent_on_1x_while_lte_active_in_single_mode(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmcall_sent_mo_call_on_1x_currently_in_single_mode

DESCRIPTION
  Check if the call is being dialed on 1X , And currently in single stack 
  SRLTE mode

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_sent_mo_call_on_1x_currently_in_single_mode(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
);
/*===========================================================================

FUNCTION cmcall_remove_mode_and_force_and_sched_sr

DESCRIPTION
  Check if the call is being dialed on 1X , though LTE is active currently

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_remove_mode_and_force_and_sched_sr(

   cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmcall_redial_after_hard_failure_and_sched_sr

DESCRIPTION
  Redial the call failed due to hard failure 
  And schedules the next silent redial

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_redial_after_hard_failure_and_sched_sr(

   cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmcall_is_ssac_p00

DESCRIPTION
  Check if the SSAC barring factor for MMTEL voice is p00

DEPENDENCIES
  None

RETURN VALUE
  TRUE - mmtel voice barring factor is p00
  FALSE - mmtel voice barring factor is not p00

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_ssac_p00(void);

/*===========================================================================

FUNCTION cmcall_is_esr_allowed_in_hvolte

DESCRIPTION
  This function tells whether ESR is allowed in hvolte

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_esr_allowed_in_hvolte( void );


/*===========================================================================

FUNCTION cmcall_is_there_a_cdma_call_except

DESCRIPTION
  Checks is there is a call object allocated for CDMA,
  other than the passed call-id

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_cdma_call_except (cm_call_id_type call_id);
/*===========================================================================

FUNCTION cmcall_nv_refresh

DESCRIPTION
  Refresh CMcall NVs


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmcall_nv_refresh(

   uint8             refresh_subs_mask
    /* Subs mask whose NVs need to be refreshed
    */
 );
/*===========================================================================

FUNCTION cm_find_call_id_gw_per_sub

DESCRIPTION
  Find the call ID with gw(SM) connection_id per sub
  This will search for if there is any call_ptr which has gw connection id
  including LTE or GW call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_gw_per_sub
(
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr,
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION cmcall_ssac_delay_timer_cb
  callback function invoked when ssac_delay_timer expire

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmcall_ssac_delay_timer_cb(unsigned long dummy);

/*===========================================================================
FUNCTION cmcall_ssac_ptr

DESCRIPTION
  Return a pointer to the SSAC object


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

extern cmcall_ssac_s_type  *cmcall_ssac_ptr( void );

/*===========================================================================

FUNCTION cmcall_check_and_send_mmtel_status

DESCRIPTION
  This function checks VOLTE call in progress and provides
  updated mmtel status to REG
  1)If VOLTE started, update voice status =1
  2)if VOLTE ended, update voice status =0
  3)if VT started, update video status =1
  4)if VT ended, updated video status =0
  5)During SRVCC, update voice/video =0
  6)During CSFB, update voice =0

  call_ptr = call which gets started or ended

  call_start = call ending or starting

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_check_and_send_mmtel_status(  cmcall_s_type  *call_ptr, boolean call_start );

/*===========================================================================

FUNCTION cmcall_shorten_sr_duration

DESCRIPTION
 If UE is moving from no service to service on CAMPED_IND, shorten the silent
 redial timer on pending calls so that NAS does not start T3402

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
 void                            cmcall_shorten_sr_duration(

       cm_ss_e_type                    ss,

       sys_sys_mode_e_type             mode
);

/*===========================================================================

FUNCTION cmcall_end_volte


DESCRIPTION
  Send VOLTE call end request to IMS

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_end_volte
(

  sd_ss_e_type                          end_ss

);
/*===========================================================================

DESCRIPTION
  This function stores the custom voice SR timer info read from
  NV to the static structure and alos updates the static orig
  plan table with the custom sr_timer value
  
DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_voice_sr_max_timer_set( boolean is_enabled,
                                    dword   custom_voice_sr_timer,
                                    sys_modem_as_id_e_type asubs_id);

/*===========================================================================
FUNCTION cmcall_voice_sr_max_time

DESCRIPTION
  This function returns the Silent redial max time value for voice calls
  
DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
dword cmcall_voice_sr_max_timer_get(sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION cmcall_recalculate_csfb_type

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_recalculate_csfb_type(cm_ss_e_type  ss, sys_modem_as_id_e_type asub_id);

/*===========================================================================

FUNCTION cmcall_convert_call_to_ppcsfb_call

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_convert_call_to_ppcsfb_call(cmcall_s_type   *call_ptr);


/*===========================================================================

FUNCTION cmcall_det_ss_for_volte_call

DESCRIPTION

 Returns SS based on MSIM/Single SIM configuration

DEPENDENCIES

   None

RETURN VALUE

  Instance.

SIDE EFFECTS
  none

===========================================================================*/
cm_ss_e_type cmcall_det_ss_for_volte_call(

   const cmcall_s_type        *call_ptr
     /* Call object */
);

#ifdef CM_GW_SUPPORTED	

/*===========================================================================

FUNCTION cmcall_invalidate_all_nsapi

DESCRIPTION
  Invalidates all the nsapis locally in CM

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_invalidate_all_nsapi(

  cm_ss_e_type    ss
    /* Stack on which the abort request has been sent  and Nsapi has to be invalidate*/

);
#endif

/*===========================================================================

FUNCTION cmcall_send_ext_volte_state

DESCRIPTION
  Updates external VOLTE call status to Lower layers

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_send_ext_volte_state
(
  const  cm_ext_volte_call_state_ind_s_type      *volte_state
);

/*===========================================================================

FUNCTION cmcall_process_custom_wifi_settings

DESCRIPTION
  When emergency call on wifi is supported, this function will set
  some wifi call configurations.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  Given call_ptr is modified.

===========================================================================*/
void cmcall_process_custom_wifi_settings (

  cmcall_s_type    *call_ptr
    /* pointer to a call object */
);

/*===========================================================================
FUNCTION cmcall_unforce_call_end_standby_chgd

DESCRIPTION
  unforce orig end before processing standby changed

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_unforce_call_end_standby_chgd(

  cm_ss_e_type cm_ss

);
 /*===========================================================================

FUNCTION cmcall_e911_orig_fail_ind

DESCRIPTION
  Indicate origination failure reason to clients for e911 call

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_e911_orig_fail_ind(

 cmcall_s_type  *call_ptr
);

/*===========================================================================

FUNCTION cmcall_check_and_update_wlan_e911_settings

DESCRIPTION
  Checks call end reason
  and update the Twwan timer and e911 call attempt state accordingly.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  Given call_ptr is modified.

===========================================================================*/
void cmcall_check_and_update_wlan_e911_settings ( cmcall_s_type    *call_ptr);

/*===========================================================================

FUNCTION cmcall_copy_ss_info

DESCRIPTION
  Copies the current serving system info to the call object

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_copy_ss_info(

   cmcall_s_type  *call_ptr
        /* pointer to a call object */
);

/*===========================================================================

FUNCTION cmcall_check_and_send_ps_redial

DESCRIPTION
  Indicate if PS redial is needed for e911 call.
  Need to add CS redial when Reject cause codes for CS e911
  redial are identified

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_check_and_send_ps_redial(

 cmcall_s_type  *call_ptr
);
/*===========================================================================

FUNCTION cmcall_check_and_send_ps_redial

DESCRIPTION
  Indicate if PS redial is needed for e911 call.
  Need to add CS redial when Reject cause codes for CS e911
  redial are identified

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_process_cs_to_ps_redial(

 cmcall_s_type  *call_ptr
);
/*===========================================================================

FUNCTION cmcall_check_and_decrement_cs_redial_count

DESCRIPTION
  Check the current CS redial count and decrements if necessary

DEPENDENCIES
  None

RETURN VALUE
 

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_check_and_decrement_cs_redial_count(
cmcall_s_type     *call_ptr
);

/*===========================================================================
  FUNCTION cmcall_end_stale_gw_ps_calls

  DESCRIPTION
    This function ends the PS calls which are in ORIG state and which weren't sent to NAS.

  DEPENDENCIES

  RETURN VALUE
    none

  SIDE EFFECTS
    none
===========================================================================*/
void cmcall_end_stale_gw_ps_calls(sys_modem_as_id_e_type asubs_id);

#endif /* CMCALL_H */
