#ifndef CM_V_H
#define CM_V_H

/**==========================================================================
@FILE_DOC

              C A L L   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Call Manager module.

Copyright (c) 2002 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cm_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/14   xs      hVOLTE SSAC support
05/02/13   ss      New API for converting ASCII to BCD
04/24/13   ss      New API for converting BCD to ASCII
03/05/13   ss      Added new NV for mode CDMA_HDR_GSM_TDS_LTE
07/10/12   xs      Hold subscription unavailable and ok_to_proceed to GSDI until get DS
                   session termination confirmation
07/02/12   sg      Updated file to support C+H+G+W+L mode
05/01/12   gm      Cleanup for API cm_get_current_service_status
04/18/12   gm      Simplified API to retrieve SS info for all tech.
04/04/12   sg      Adding new NV for mode pref TDS_LTE
03/05/12   xs      Added support of avoid unnecessary attach
02/19/12   ak      Move cm_gwl_avoidance_timers to CMPH
01/20/12   gm      Data synchronization for CM Call objects
12/21/11   sg      Move nv items from cmph.h file to this file
09/14/11   vk      New MMOC Transaction "MMOC_TRANS_DEACT_1XCSFB_CMD" related changes.
04/12/11   xs      Added band and channel info in cm_get_current_service_status
02/25/11   cl      GW Service Indication Propagation MSM->MDM
02/02/11   xs      Wait for DS notification for PWROFF/LPM oprt mode change
01/25/11   ak      Added support for periodic GWL scans in DO only coverage.
01/04/11   rm      CMI-4: Moving global data to private files
01/04/11   rm      CMI-4: Removing #ifdef's from /api
12/23/10   sv      Integrate DSDS feature
12/14/10   xs      Added support for call throttle parameter configurable in EFS for JCDMA
11/24/10   xs      Fusion featurization clean up
07/16/10   xs      Added new structure cm_curr_sys_mode_s_type
05/20/10   rm      Moving few defintions from cm_v.h to cm.h
10/05/09   rm      Moving CM_CALL_TYPE_* from cm_v.h to cm.h
09/25/09   ks      Adding changes for Nw Initiated QOS
08/24/09   rm      Moving CM_BAND_PREF_* enums from cm_v.h to cm.h
07/14/09   rm      Moving few defintions to cm.h from here
06/30/09   sv      Moving CM_CALLED_PARTY_BCD_WITH_BCD_LEN def to cm_gw.h
04/04/09   ks      Added support for blocking MO/MT voice call on Data Card
03/24/09   sv      Added cm_ph_get_oprt_mode
03/20/09   rm      Renaming cmph_otasp_update_mode to cm_otasp_update_mode
03/13/09   rm      Adding cmph_otasp_update_mode
02/23/09   rm      Removed cmwmsg.h
09/04/08   sv      Added WPS API
07/30/08   rm      CMI defeaturisation
07/24/08   rn      Send regn reject ind to clients
07/28/08   rk      Added error code to inform CM blocked mode pref change.
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
06/26/08   ak      Fix for compiler warnings
06/16/08   rk      Adding FEATURE_MM_SUPERSET to remaining feature flags
06/11/08   fj      Added remoted API cm_get_mode_band_capability
05/23/08   aj      Adding metacomments for cm_mm_call_cmd_orig_exten
                   cm_mm_call_cmd_handover_req,cm_ph_cmd_wakeup_from_standby2,
                   cm_ph_cmd_update_cdma_capability,cm_ss_cmd_country_selected
05/22/08   rk      Added CM_CALL_EVENT_MPTY_CALL_IND in cm_call_event_e enum.
05/22/08   vk/sv   added roam only preference
05/06/08   ak      BCMCS 2.0 changes
04/10/08   rn      Added metacomment for CM_SS_MOBILITY_MGMT_MASK and others
03/13/08   ks      Define CM_API_SIGNAL_STRENGTH_INDICATION
03/10/08   rn      Added support for indicating Limited_regional to clients
01/28/07   jqi/aj  Added FEATURE_FULL_TIME_SHDR_3 support.
02/15/08   ks      Adding SIR, Pathloss Implementation
02/11/08   rn      Alpha buffer Size being increased to support SIM related
                   GCF test cases.
02/06/08   rn      Reverted the changes done for remoting cm_nc_reg_with_call_control
02/04/08   rn      Remoted CM API cm_nc_reg_with_call_control
01/25/08   rn      Added metacomments to cm_call_comp_cap_cb_f_type etc
01/24/08   jqi     Removed the metacomments for cm_mm_get_call_comp_cap.
01/24/08   ks      Display of UE signal strength indication
01/21/08   rn      Added metacomments for cm_mm_get_call_comp_cap
01/03/08   ka      Adding handset based plus code dialing feature
12/20/07   ic      Added missing meta comment to cm_m511_mode_settings_f_type
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/07/07   ra      Added support for IMS VCC in HOMER files
12/04/07   ic      Corrected typo in comment for CM_API_CLIENT_TYPE_PBM
11/26/07   sv      Removing Feature FEATURE_GW_CNAP_CODING_SCHEME
11/22/07   sg      Adding is_network_initiated field to cm_gw_cs_call_info_s_type.
10/03/07   rn      Added support for UE initiated dormancy
11/05/07   sg/sv   Renaming FEATURE_CM_TEMP_NAS_DEP_TBD to
                   FEATURE_GW_CNAP_CODING_SCHEME
10/29/07   rn      Corrected meta comment for cm_enc_alpha_s_type
10/15/07   sk      Fixed meta comments for cm_call_substate_type
09/25/07   dm      Added support for FEATURE_MM_SUPERSET
09/17/07   ic      Fixed location of meta comments for cm_alpha_s_type
                   Corrected format of comments for cm_enc_alpha_s_type
09/17/07   rn      Added structure cm_enc_alpha_s_type to include coding_scheme
                   and number of bytes info for CNAP string.
                   Added CM_MAX_ALPHA_TAG_BYTES for CNAP
                   Added field enc_alpha in struct cm_mm_call_info_s
09/14/07   dm      Added support for FEATURE_MM_SUPERSET
08/27/07   rm      Added new error type for WMS-CM-NAS flow. Fix for CR 121142
08/17/07   ka/dm   New field to report HS call in progress
08/08/07   cl      Add OTASP Support for ASW Band
07/25/07   sk      Added lots of Doxygen comments.
07/16/07   sk      Featurized CM_API_WLAN_ADHOC under FEATURE_WLAN_ADHOC.
06/27/07   cl      Add new CM API to expose cmwcall_get_user_data() to RPC
06/07/07   rp      Adding support for EGPRS indication
06/06/07   pk/cl   Added support for LN status notification from HDR
05/15/07   rp      Adding gw_ecio
04/30/07   jqi     Added EMPA support for MT DBM over HDR.
04/23/07   jqi     DOS feature rework for dual processor arch.
04/16/07   rp      Removing FEATURE_MMGPS
04/10/07   rp      Adding fields for SMS domain selection
04/03/07   sk      Added CM_PH_CMD_CLIENT_RELEASE for internal use.
03/29/07   rp      Adding RPC metacomments for 3rd part MOLR
03/27/07   rp      Adding  CM_NAS_SMS_NO_RESPONSE_FROM_NETWORK
03/22/07   ic      Added several missing ) in #define CM_BAND_PREF_*
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
                   Fixed the meta comments.
02/23/07   ic      In cm_call_substate_u added to meta comments handling for
                   CM_CALL_STATE_ORIG
02/17/07   rp      Added FEATURE_WMS_CM_NAS_INTERFACES and related datastructures
02/09/07   pk      Added checks for CDMA subscriptions for GSM builds, for PD
                   sessions
02/02/07   pk      Added CM_IP_CALL_END_EMERG_ORIG
01/22/07   rp      Support for copying result from cc to call_info_ptr
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
01/08/07   ic      Removed FEATURE_GSTK from cm_gw_cs_orig_params_s_type
12/28/06   ka      Adding persistent enum to acq order, band, roam, hyb pref
                   and srv domain enums.
12/19/06   ka      Adding new enum for UCS2 Data coding scheme.
12/14/06   ic      FEATURE_HDR_EMPA tune up, restoring original fields in
                   cm_dbm_info_s_type and cm_dbm_data_block_s_type
11/21/06   ka      Adding rab_id_present field to cm_gw_cs_call_info_s_type.
11/20/06   ic      Mainlined FEATURE_HDR_EMPA
11/17/06   jqi     Added support for HDR active protocol revision information.
                   Added CDMA connect Oder Ack support.
11/16/06   ic      Added support for FEATURE_HDR_EMPA
                   Clients to check for CM_API_FEATURE_HDR_EMPA compile flag
                   before using this functionality
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/07/06   sk      Memory reduction effort.
11/06/06   hn/ic   Under FEATURE_HTORPC_METACOMMENTS added meta comments for
                   cm_ph_cmd_subscription_changed_new & cm_wlan_pref_init
10/05/06   ic      Under FEATURE_HTORPC_METACOMMENTS added meta comments for
                   cm_m51_mode_reg()
10/05/06   ic      Added CM_CLIENT_TYPE_OEMNET
09/29/06   rp/ka   Added code to initialize all the fields in sups object.
09/22/06   rp      Added support for BC15, BC16
09/13/06   rp      Added extra field to supps_info_s_type to pass unparsed L3
                   data from MN.
09/06/06   ic      Unnecessary comments clean up
08/29/06   sk      Added WLAN Adhoc support.
08/28/06   ka      Adding pdp header comp and data comp enums.
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
08/16/06   ka/rp   Adding advice of charge info (aoc_info) to PH event struc.
08/11/06   ka      Adding metacomments for cm_mm_client_call_info_list_reg()
08/08/06   sk      Added CM_MODE_PREF_ANY_BUT_HDR_WLAN.
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
07/07/06   pk      Added cmtask_add_cmd_to_hold_q, and supporting data
                   structures, fields and functions. Added
                   CM_PD_SESSION_STATUS_DENY_CMD_ON_HOLD
07/07/06   pk      Added CM_SS_ORIG_THR_DS_SIP_FAILURE to support DCTM 2.0
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Adding Call control Support for PS calls.
06/08/06   ic      Modified meta-comments for cm_bcmcs_cmd_flow_request()
06/05/06   ka/ic   CM clients expect number buffer to hold "+" along with
                   maximum dialed digits.
                   Increased CM_MAX_CALLED_INFO_CHARS by 1 to achieve this.
05/02/06   sk      Added RPC metacomments for cm_mm_call_cmd_modify_res().
04/17/06   ka      Adding support for HSDPA and DTM indications.
                   Metacomments for cm_change_xxx functions in cm.h#228
04/19/06   pk      Added meta comments for 7200
04/06/06   sk      Included CM_SRV_DOMAIN_PREF_NONE as a valid service domain
04/06/06   sk      Added the following fields to cm_mm_ss_info_s_type:
                   - hybr_hdr_bcmcs_srv_status
                   - ss_main_bcmcs_srv_status
04/01/06   ka      Changes include emerg serv category during orig, aborting
                   manual search, returning current band with srv ind and
                   increasing dialed string to 80 chars.
03/24/06   ka      Adding VideoTelephony fallback to voice feature for UMTS.
03/19/06   jqi     Added CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE .
03/14/06   pk      Added support for high PSIST
03/13/06   ka      Adding metacomments to cm_end_params_u.
03/13/06   sk      Added two new client types:
                   - CM_CLIENT_TYPE_GSDI
                   - CM_CLIENT_TYPE_BREW_APPS
03/10/06   pk      Added CM_SS_MCC_MASK and CM_SS_IMSI_11_12_MASK
03/09/06   pk      Added support for Power Collapse
03/05/06   jqi     Merged the WCDMA BC3,4,8,9 support.
02/17/06   ka      Adding support for videoshare
02/10/05   ka      Adding new substates to call incoming state.
02/02/06   pk      Added CM_API_CLIENT_TYPE_PBM
01/09/06   ic      Undid CM API changes from lint clean up for cm_num_init()
                   and cm_num_copy()
01/09/06   ic      Under FEATURE_HTORPC_METACOMMENTS, merged in meta comments
                   from //depot/asic/msm7500/orphans/00.04/services/cm/cm.h#7
                   (delta from 6 to 7)
                   Changed declaration of cm_dbm_payload_copy_cb_f_type so it
                   can be remoted for RPC
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
01/06/06   ic      Lint cleanup.
                   Under FEATURE_HTORPC_METACOMMENTS, added meta comments for
                   cm_end_params_u_type
                   CASE CM_CALL_MODE_INFO_IP cm_end_params_u.ip_cause
01/05/06   pk      Added numbering to all the enums
12/13/05   ic      Fixed compiler error "..\..\services\cm\cmcall.c",
                   Error: C3028E: <arg 7 to 'cm_nc_call_control_processing'>:
                   implicit cast of pointer to non-equal pointer
11/30/05   ka      Adding minimum_qos to pdp modify request.
12/03/05   ic      Added CM_CALL_INCOM_SUBST_ANSWERED
                   to cm_call_incom_subst_e_type
11/30/05   ka      Adding new network selection mode pref for HPLMN search.
11/23/05   ka      Modification to allow hold/active for ip calls.
11/18/05   pk      Added support for BC 5 and BC 11 sub blocks I,J,K
11/18/05   ic      Added CM_CLIENT_TYPE_OEMTELEPHONE, clients to check for
                   compile flag CM_API_CLIENT_TYPE_OEMTELEPHONE
11/07/05   ic      Added CM_CLIENT_TYPE_MFLO, clients to check for compile
                   flag CM_API_CLIENT_TYPE_MFLO
10/27/05   pk      Removed featurization from cm_call_evetn_e_type and
                   cm_ss_event_e_type
                   Added CM_SS_EVENT_ORIG_THR_TBL_UPDATE and
                   cm_ss_orig_thr_info_s_type.
                   Added throttle table information and uptime to
                   cm_mm_ss_info_s_type
10/20/05   pk      Added CM_MODE_PREF_INTERSECT_OR_FORCE and
                   CM_SRV_TYPE_NON_AUTOMATIC
10/20/05   pk      Added support for CM_CALL_END_ACCESS_BLOCK_ALL
10/18/05   pk      Added CM_PD_SESSION_STATUS_HOLD_FORCE_MODE to
                   cm_pd_session_status_e_type
10/14/05   ic      Removed extra comma from cm_nas_error_e_type
09/15/05   pk      Added flags CM_API_PD_SESSION, CM_API_CLIENT_TYPE_DS_BCMCS
09/13/05   pk      Added CM_CLIENT_TYPE_DS_BCMCS client type
09/12/05   pk      Added support for GPS session, added session type
                   CM_PD_SESSION_TYPE_WITH_GPS_NORMAL
                   CM_PD_SESSION_TYPE_WITH_GPS_DMOD
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/23/05   ic      FEATURE_HTORPC_METACOMMENTS changes - removed declaration
                   of cm_call_id_is_allocated() as BREW / BT no longer use it
                   (CRs 71154 and 63846)
08/19/05   sk      Completely removed is_jcdma_emergency flag.
08/15/05   pk      Added cm_ph_state_e_type, this indicates the current state
                   of the phone. Currently we support Normal and emergency
                   call back mode. Added ph_state field to
                   cm_ph_info_s_type
07/29/05   dk      Added a call failure reason when call is on hold because
                   of other activity in progress.
07/25/05   sk      Added function prototypes for:
                   - cm_util_ussd_pack()
                   - cm_util_ussd_unpack()
                   - cm_util_ascii_to_gsm_alphabet()
                   - cm_util_gsm_alphabet_to_ascii()
                   Added comments to old API indicating that the new one
                   should be used instead.
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
07/15/05   pk      Added CM_API_ORIG_THR_PKT_ZONE feature, to differentiate
                   between the old call throttle API without packet_zone
                   support and the new call throttle API with packet_zone
                   support
07/11/05   ic      Externalized cm_util_pack() / cm_util_unpack()
07/11/05   pk      Modified cm_ss_cmd_orig_thr_update_tbl() API, added
                   packet_zone as one more parameter.
07/08/05   ic      Report highest layer call origination failure as part of
                   call events
07/08/05   ic      Added misc utility conversion functions (moved from
                   cmutil.h)
06/30/05   pk      Removed feature flags from
                   CM_PH_EVENT_TERMINATE_GET_NETWORKS
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/23/05   ic      Added ULL suffix to CM_BAND_PREF_ANY (compiler warning fix)
06/07/05   ic      Added meta comments for cm_get_cur_hybr_status()
06/06/05   pk      Added CM_CALL_END_ORIG_THR to call end reason enum
                   Added CM_SS_ORIG_THR_STATUS_MASK
                   Added CM_SS_HDR_SYS_ID_MASK
                   Added CM_SS_ORIG_THR_CMD_UPDATE_TABLE and
                   CM_SS_ORIG_THR_CMD_CALL_STATUS to cm_ss_cmd_e_type
                   Added cm_ss_orig_thr_reasons_e_type
                   Added ss_thr_status and hdr_sys_id to cm_mm_ss_info_s_type
                   The above changes are required to implement the Data
                   Throttle Manager API's
06/02/05   ic      Mainlined FEATURE_36_DIGIT_DIAL_STRING
05/25/05   ic      Formatting and comment changes to cm_ph_cmd_err_e_type
                   Removed references to FEATURE_GPSONE_DBM as it was already
                   mainlined
05/19/05   ka      Adding support to inform clients about call ciphering.
05/12/05   ic      Added meta comments under FEATURE_HTORPC_METACOMMENTS
                   Removed declaration of cmph_is_ph_in_emerg_cb_mode()
05/11/05   ic      Externalized cm_ph_net_list_copy()
04/13/05   ic      Put RPC comments under FEATURE_HTORPC_METACOMMENTS
04/11/05   ka      Restoring #define CM_API_WLAN and differentiating between
                   CM_API_IP_CALL and CM_API_WLAN
04/07/05   ic      Added #define CM_API_IS_JCDMA_EMERGENCY
04/07/05   ic      Added is_jcdma_emergency to cm_cdma_call_info_s_type so
                   it's sent to clients with call events
04/06/05   ka      Change in #define name
03/31/05   ic      Corrected comment for CM_CALL_EVENT_END_REQ
03/24/05   ic      Removed unnecessary featurization from cm_call_cmd_e_type
03/21/05   dk      Added CM_API_BAND_PREF_64_BIT define type.
03/17/05   ic      Fixed typo in cm_ussd_dcs_e_type, added CM_WAKEUP_MAX,
                   fixed typo in cm_nas_error_e_type
03/16/05   ic      Include cmwmsg.h in cm.h directly instead of indirectly
                   (through sys_gw.h) as cm.h uses MAXNO_FORWARD_TO_NUMBER
                   that is defined in cmwmsg.h
03/14/05   ka      Adding call end status enums and new answer struct for ip
                   call.
03/09/05   pk      Added CM_PH_EVENT_DDTM_STATUS event to indicate the change
                   of DDTM status.
03/09/05   dk      Added comments for band_pref.
03/07/05   ic      Removed unnecessary comas
03/04/05   ic      Merged in DataOverSignaling (DOS) support
02/17/05   pk      Added cm_is_sys_mode_allowed() to verify the compatiblity
                   of the mode_pref, band_pref and sys_mode
02/17/05   ic      Removed extra , after CM_ALS_LINE_SWITCHING_MAX
02/17/05   ic      Added new set of Meta-comments
02/14/05   pk      Added CM_CALL_EVENT_LINE_CTRL
02/09/10   dk      Expanded cm_band_pref to 64 bit.
02/03/05   ic      Added support for FEATURE_UMTS_BMC
02/02/05   ka      commented CM_BAND_PREF_WLAN_2400_ETS
02/01/05   dk      Added WLAN Support.
01/17/05   dk      Added support to read unpacked User PLMN Lists.
01/13/05   ka      Correcting signature of  cm_call_event_user_data_f_type
01/12/05   ka      Adding macros and enums for ussd data coding scheme
01/10/05   sj      Added new call end reasons.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
01/04/05   sj      Corrected base lat/long info in BS status response.
12/29/04   dk      Added VideoTelephony call interface.
12/19/04   ic      Replaced CM_CALL_CONV_SUBSTATE_MAX by CM_CALL_CONV_SUBST_MAX
12/05/04   ic      Added cur_ddtm_status to cm_ph_info_s_type
12/03/04   ws      Support for Remote WMS.
12/02/04   dk      Added call_subst in cm_call_state_info_s_type.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
                   Added CM_CALL_CMD_ERR_USER_DATA_LENGTH_P error code to
                   cm_call_cmd_err_e_type
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP feature
11/16/04   sj      Changed comments for avoid type names.
10/20/04   sj      Include is_colloc in CM SS event.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/14/04   ic      Call direction clean up - removed direction field from
                   cm_gw_cs_call_info_s_type
09/13/04   ka      Added new sups cmd error for call control reject.
09/10/04   jqi     Any systerm preference change or operation mode change
                   should terminate a pending get network request.
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/30/04   ic      Fixed ADS 1.2 compiler error "line 4146: Error: C2839E: junk
                   at end of #endif line - ignored" (digit 2 following comment)
08/27/04   jqi     Added ps dormant optimization.
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/23/04   ws      Updated RPC Meta Comments.
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/19/04   dk      Changed CM_CALL_END_HDR_HOLD_DBM_IN_PROG to
                   CM_CALL_END_HOLD_DBM_IN_PROG.
08/18/04   ka      Adding direction as a parameter to call object
08/17/04   ic      Undoing P4 change list #121638
08/12/04   ws      Remove cm20 support.
07/28/04   ka      Added support for secondary PDPcontext
07/27/04   ws      Call Control support for SUPS.
07/21/04   sj      Added support for umts to 1x handover.
07/15/04   sj      Added new end_status = no_full_srv.
07/14/04   ic      Added extern declaration to cm_ph_cmd_line_switching() and
                   cm_ph_cmd_selected_line()
07/14/04   ws      Add requesting_client_id to event info structs.
07/12/04   ic      Added support for FEATURE_ALS
07/08/04   ws      Add RPC meta comments.
07/01/04   sj      Added new call end status for redirection or handoff.
06/30/04   ws      Include callback for cm_pd_cmd_reva_mo_msg()
06/25/04   dk      Added CM_CALL_CMD_ERR_CONNECTION_ID_OUT_L error code.
06/21/04   ic      Corrected comment in cm_ss_event_e_type
06/21/04   ank     Mainlined FEATURE_GPSONE_DBM
06/16/04   ic      Added CM_CALL_END_HDR_RELEASED_BY_CM used when HDR call is
                   released by CM to let voice call through.
                   Removed CM_SMS_CMD_ERR_NOT_ALLOWED_IN_HDR_S as it became
                   obsolete
06/03/04   ws      Add support for redirecting number.
06/02/04   ws      Added camera as a client type.
06/01/04   npr     Added IP Header and Data compression requirements to GW PS
                   call origination and information structures for GPRS SNDCP.
05/28/04   ic      Defined CM_SS_P_REV_IN_USE_SVD_SUPPORTED and
                   CM_SS_P_REV_IN_USE_SVD_NOT_SUPPORTED
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/30/04   ws      Added enums for num type and num plan.
04/29/04   ic      cm_mode_pref_e_type clean up.
                   Removed CM_MODE_PREF_ANY_BUT_HDR_AND_AMPS as it's identical
                   to CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY.
                   Added CM_MODE_PREF_CURRENT_LESS_AMPS
04/19/04   ic      Added support for HDR Ec/Io & Io reporting.
                   HDR Ec/Io & Io will be reported with CM_SS_EVENT_RSSI &
                   CM_SS_EVENT_HDR_RSSI events.
04/15/04   ws      Added CM_MODE_PREF_CDMA_AMPS_HDR_ONLY.
04/13/04   ic      Featurization changes related to 6500 5.0 CUST files being
                   used
04/08/04   ws      Added CM_MODE_PREF_CDMA_HDR_ONLY.
03/29/04   ic      Added support for FEATURE_36_DIGIT_DIAL_STRING
03/17/04   ic      Fixes to comment indentation in file header.
03/17/04   sj      Updated the DDTM api with new parameters.
03/10/04   ic      FEATURE_BCMCS - removed parameter check error
                   CM_BCMCS_CMD_ERR_INVALID_PORT_NUMBER_P
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/06/04   ws      Initial Jaguar/Mainline merge.
01/20/04   ka      Added support for Muliple PDP and call info list api
01/13/04   ws      Added new end status when data calls are ended for e911 calls.
01/08/04   sbs     Added missing semicolon after cm_sups_cmd_lcs_molr_completed
                   function prototype
11/20/03   ic      Added support for:
                   CM Commands:
                   - CM_PD_CMD_FIX_START
                   - CM_PD_CMD_FIX_END
                   CM Events:
                   - CM_PD_EVENT_FIX_START
                   - CM_PD_EVENT_FIX_END
                   Log events:
                   - EVENT_CM_BLOCK_HDR_ORIG_DURING_GPS
                   - EVENT_CM_ALLOW_HDR_ORIG_DURING_GPS
11/19/03   sbs     Added LCS MOLR Completed API function.
11/14/03   ws      Added support for is_jcdma_emergency
10/27/03   ws      Removed uim_pref_slot_selection support.
10/23/03   ws      jqi's changes for band preference enum.
10/20/03   ws      Dual Slot Support.
                   Fixed misspelled available with several enums.
                   Added CM_SS_EVENT_SRV_NEW for FEATURE_GPSONE_BS_INFO.
10/07/03   ic      Added CM_CALL_CMD_ERR_MODE_PREF_P to cm_call_cmd_err_e_type.
                   Added CM_CALL_END_INTERNAL_ERROR to cm_call_end_e_type.
                   Added CM_MODE_PREF_ANY_BUT_HDR_AND_AMPS and
                   CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS to cm_mode_pref_e_type.
09/30/03   vt      Modified to support common cm.h across all targets.
09/22/03   AT      Moved AOC functionality to this file.
09/12/03   prk     Added CM_PH_EVENT_PS_ATTACH_FAILED event to cm_ph_event_e_type.
09/23/03   ws      Added cm_get_current_acq_sys_mode
09/22/03   vt      Added bluetooth client id.
09/12/03   vt      Moved hybrid instance parametrs from cdma structure to
                   common for serving system.
09/05/03   ic      Added CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION to
                   cm_call_cmd_err_e_type enumeration.
                   Changes to cm_mode_pref_e_type enumeration - un-did changes
                   from rev#7 so that new enums are added at the end to not
                   break compatibility with test tools.
09/16/03   vt      Added preferred slot handling.
09/16/03   ws      Add support for uz_id and uz_name in ss
08/27/03   ic      Added CM_SRV_TYPE_WCDMA and CM_SRV_TYPE_GSM to
                   cm_srv_type_e_type.
                   Put drs_bit field in cm_cdma_orig_params_s_type under
                   FEATURE_IS2000 to keep consistent with other structures.
                   Added CM_MODE_PREF_GSM_WCDMA_ONLY,
                   CM_MODE_PREF_ANY_BUT_HDR_AND_AMPS and CM_MODE_PREF_ALL_BUT_HDR
                   to cm_mode_pref_e_type.
08/22/03   vt      Mainlined packet state handling.
08/21/03   ws      Support for MM silent redial and automatic mode.
08/14/03   vt      Added DDTM.
08/13/03   prk     Added minimum_qos field to PS call info structures.
08/01/03   az      Added support to return overflow size for MO SDB.
07/17/03   jqi     Added more band pref enum types.
07/16/03   ic      Added CM_CLIENT_TYPE_BREW to cm_client_type_e_type
07/09/03   prk     Added CM_CALL_CMD_ERR_INVALID_SIM_STATE_S to
                   cm_call_cmd_err_e_type.
07/08/03   sbs/lt  Added support for Location Services related SS messages
07/08/03   vt      Added sys mode to call info structure.
07/02/03   vt      Updated HDR related service indicators.
06/30/03   vt      Added a new call end status.
06/19/03   ws      Changed band pref bitmask to 16 bit enum
06/04/03   wli     Added RTRE control item for SIM access.
05/28/03   vt      Added more service types. Defined roam & band as bitmasks.
05/15/03   vt      Added a function to return current service for PS call.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
05/02/03   prk     Changed CM_MAX_SUBADDRESS_DIGITS from 20 to 21 to be
                   compliant with GSM specs.
04/29/03   sun     Added cc_reject to cm_gw_cs_call_info_s_type
04/28/03   AT      Lint cleanup.
04/21/03   vt      Added handling of SIM access for RTRE.
04/14/03   prk     Removed references to FEATURE_SUPS and FEATURE_PLMN.
04/14/03   ws      Added BSID changes.
04/05/03   ws      Modified #def for flash type emergency to sups type emergency
03/11/03   ATT     Added support for call control via clients.
03/11/03   ws      Changed CALL_ID_MAX to 8 except when CM20 Interface is on
02/28/03   vt      Added the interface to block/unblock HDR data call
                   origination.
02/28/03   AT      Added to error reason for call control rejecting call.
02/27/03   prk     Added support for CM_CLIENT_TYPE_GSTK.
02/27/02   cs      Added MT/MO indication and transaction ID in CS GSM/WCDMA
                   call info for USAT support.
                   Added cm_mm_call_cmd_ps_data_available API function.
                   Added ps_data_suspend field in cm_mm_ss_info_s_type to indicate
                   that the service has been suspended. Added CM_SS_PS_DATA_SUSPEND_MASK.
02/25/03   RI      Added CM_SRV_DOMAIN_PREF_PS_ATTACH and
                   CM_SRV_DOMAIN_PREF_PS_DETACH in cm_srv_domain_pref_e_type.
02/25/03   vt      Set some enums to fixed values for event logging.
02/19/03   ATM     Added DIAG client
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/10/03   at      Added support for CAT_APP
02/06/03   vt      Removed answer_duration field from cm_ph_info_s_type.
02/06/03   ri      Added support for concurrent services.
                   Modified cm_mm_call_cmd_end().
02/03/03   jqi     Added support for band class 6.
01/21/03   ic      Added answer_duration field to cm_ph_info_s_type structure
01/21/03   ws      Updated copyright information for 2003
01/21/03   ws      Added #define for mapping PKT_DATA to PS_DATA for call types.
12/17/02   vt      Defined fixed numbers to several enums for event logging.
12/11/02   vt      Added more comments.
11/27/02   vt      Added comments.
11/22/02   PRK     Added support to process user preferred PLMN list.
11/19/02   vt      Added changing RTRE configuration.
11/14/02   vt      Merged changes from GW mainline.
                   [AT]Added additional support for No funds in AOC.
                   [YR]Added test control type for Test Control test.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/31/02   vt      Removed handling of NDSS origination failure.
                   Mainlined feature SD20.
10/30/02   vt      Undid - NDSS origination failure change.
10/28/02   vt      Removed handling of NDSS origination failure.
10/23/02   vt      Moved M* menu functions into utility section.
10/22/02   ic      Added M51 mode settings object
10/05/02   vt      Added forbidden system to ss mask.
09/30/02   RI      Added CM_CALL_EVENT_ORIG_FWD_STATUS,
                   CM_CALL_EVENT_CALL_FORWARDED,
                   CM_CALL_EVENT_CALL_BEING_FORWARDED,
                   CM_CALL_EVENT_INCOM_FWD_CALL for Call Forwarding SS.
                   Added ss_info field in cm_gw_cs_call_info_s_type.
09/11/02   ws      Added initial cm20 backward compatibility.
09/11/02   RI      Added redirect_party_number field in cm_gw_cs_call_info_s_type
09/05/02   vt      Added comments to indicate what FEATURE's must be
                   defined to access certain functions.
08/30/02   vt      Removed many ifdef FEATURE's & replaced byte with uint8
08/15/02   RI      Modified the max. forwarded to number size.
08/12/02   RI      Support for ECT and CD features ( GSM/WCDMA).
08/08/02   AT      Adding in support for CUG and CNAP events.
08/07/02   vt      Set call objects to 1 if SVD is undefined.
08/06/02   HQ      Added FEATURE_CDSMS.
07/29/02   RI      Added call_progress,cause_of_no_cli fields to
07/24/02   vt      Added handling for wakeup from deep sleep(standby)
07/22/02   PRK     Added cm_ph_cmd_get_networks and cm_ph_cmd_terminate_get_
                   networks.  Added CM_PH_CMD_ERR_GET_NETWORKS_S to
                   cm_ph_cmd_err_e_type.
06/28/02   vt      Added dummy structures for cm_rab_reestab_params_s_type and
                   cm_setup_res_params_s_type.
06/11/02   RI      Added CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S in
                   cm_call_cmd_err_e_type.
05/31/02   RI      Added sups_type and sups_params fields in
                   cm_gw_cs_call_info_s_type.
05/04/02   vt      Updated to make it backward compatible with CM2.0
04/25/02   RI      Added uss_data_type field to cm_sups_info_s_type.
04/24/02   RI      Added CM_CALL_EVENT_RAB_REESTAB_FAIL and
                   renamed CM_CALL_EVENT_RAB_ESTAB_IND to CM_CALL_EVENT_RAB_REESTAB_IND
                   in cm_call_event_e_type.
04/22/02   RI      Added ATCOP as CM client in cm_client_type_e_type.
03/11/02   RI      Added CM_CALL_CMD_ERR_NULL_PARAM_P,
                   CM_CALL_CMD_ERR_NO_CALL_ID_S
                   CM_SUPS_CMD_ERR_NULL_PARAM_P
                   Added ss_success field in cm_sups_info_s_type.
                   Deleted cm_sups_cmd_abort().
02/15/02   VT      Initial multimode version
===========================================================================*/


/****************************************************************************

  Organization of the file:
  -------------------------
    The file is divided into multiple sections. Each section describes a
    call manager object. You can jump from one sections to the other by
    searching for / followed by 3 *'s. The order of objects defined is as
    follows:

        Common section
        Call Object
        Phone Object
        Serving System Object
        Inband Object
        SMS Object
        Position Determination Object
        Supplementary Services Object (GSM/WCDMA)
        DBM Object
        BCMCS Object
        MBMS object
        Client Object
        Utility functions

    Each section is further subdivided to represent different types of info.
    You can jump from one sub-sections to the other by searching for /
    followed by 2 *'s. The data in each section is organized in the
    following order.

        #defines
        enums
        macros
        datatypes
        functions

  Abbreviations Used:
  -------------------
    GW  - GSM/WCDMA
    DBM - Data Burst Message

****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cm.h"
#include "nv.h"

/****************************************************************************

Common to all Call Manager Objects

****************************************************************************/

/** @defgroup COMMON Common definitions.
** @{
*/

/**------------------------------------------------------------------------
** Defines that can be checked by CM clients to detect support for a feature.
** Added for client code to co-exist between different versions of CM.
**---------------------------------------------------------------------------
*/

/** A new error type for WMS-CM-NAS interaction.
** Again CM acts like a pipe only.
*/
#define CM_API_NAS_NO_ERROR_CP_ACK

/** Support for FEATURE_HDR_EMPA
*/
#define CM_API_FEATURE_HDR_EMPA

/** Define the CM api flag for new service indicators
*/
#define CM_TECH_SPEC_SRV_IND

/** Client type GSDI. */
#define CM_API_CLIENT_TYPE_GSDI

/** Client type BREW_APPS. */
#define CM_API_CLIENT_TYPE_BREW_APPS

/** Cleint type PBM
*/
#define CM_API_CLIENT_TYPE_PBM

/** Added several new enums
*/
#define CM_API_CM_DBM_CMD_ERR_PAYLOAD_NULL_PTR_P
#define CM_API_CM_DBM_CMD_ERR_PAYLOAD_SIZE_P
#define CM_API_CM_PD_CMD_ERR_PAYLOAD_SIZE_P
#define CM_API_CM_PD_CMD_ERR_PAYLOAD_NULL_PTR_P
#define CM_API_CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P

/** Client type CM_CLIENT_TYPE_OEMTELEPHONE
*/
#define CM_API_CLIENT_TYPE_OEMTELEPHONE

/** Client type CM_CLIENT_TYPE_MFLO
*/
#define CM_API_CLIENT_TYPE_MFLO

/** Client type DS_BCMCS
*/
#define CM_API_CLIENT_TYPE_DS_BCMCS

/** API change for the new PD session functionality
*/

#define CM_API_PD_SESSION

/** Integration of the BCMCS DS to CM to HDRMC path
*/
#define CM_API_BCMCS_DS_CM_HDRMC_PATH_INTEGRATION

/** New USSD pack/unpack functions.
*/
#define CM_API_UTIL_USSD_FUNCTIONS

/** To differentiate between the old call throttle API without packet_zone
** support and the new call throttle API with packet_zone support
*/
#define CM_API_ORIG_THR_PKT_ZONE


/** Report highest layer call origination failure as part of call events
*/
#define CM_API_HIGHEST_LAYER_CALL_ORIG_FAIL

/** Externalized misc utility functions from cmutil.c
*/
#define CM_API_MISC_UTIL_FUNCTIONS

/** Origination throttle support
*/
#define CM_API_ORIG_THR

/**
** Indicates that client needs to call on cm_ph_net_list_copy()
** to get more detailed CMPH information
** If this flag is not defined, client needs to call on net_list_copy_func
** provided as part of CMPH event info structure
*/
#define CM_API_STATIC_FUNC_FOR_EXTRA_EVENT_INFO

/** Support for USSD data coding scheme enums and macros
*/
#define CM_API_USSD_DCS

/** Support for IP call cause values
*/
#define CM_API_IP_CAUSE

/** Support for WCDMA900 band class (BC3,4,8,9)
*/
#define CM_API_WCDMA900

/** Support for change CM_BAND_PREF_NO_CHANGE to 0x40000000
**                    CM_BAND_PREF_ANY to 0xFFFFFFFFBFFFFFFFULL
**                    CM_BAND_PREF_MAX tp CM_BAND_PREF_ANY + 1
*/
#define CM_API_BAND_PREF_NO_CHANGE_ANY_MAX_UPDATE

/** Support for new incoming call substate when
** call is waiting on timer before
** sending setup response
*/

/** Support for BC 12, 14,15 and 16
*/
#define CM_API_BAND_CLASS_12_14_15_16


/** Support for the new SUPS default values for ss_code, ss_reference, etc
*/
#define CM_API_SUPS_DEFAULT_VALUES


/** Check for this compile flag before trying to use CM_CLIENT_TYPE_OEMNET
*/
#define CM_API_CM_CLIENT_TYPE_OEMNET

/** Check for this compile flag before trying to use
** CM_CALL_EVENT_CONNECT_ORDER_ACK
*/
#define CM_API_CM_CALL_EVENT_CONNECT_ORDER_ACK

/** Addition of enumeration for UCS2
*/
#define CM_API_USSD_UCS2_DCS

/** Adding new enum for NAS-WMS interaction
*/
#define CM_API_NAS_CELL_CHANGE_TYPE

/* Adding new field rssi2, rscp, and ecio_for_display
** to report signal strength indication
*/
#define CM_API_SIGNAL_STRENGTH_INDICATION

/* define newly added NV mode pref values here to remove dependency on nv_items.h
** due to NV team stops suppot on adding new values to enums
*/

#define  NV_MODE_TDS_ONLY                       ((nv_mode_enum_type)53)
/* Service is restricted to TD-SCDMA only */

#define  NV_MODE_TDS_GSM                        ((nv_mode_enum_type)54)
/* Service is restricted to TD-SCDMA or GSM */

#define  NV_MODE_TDS_GSM_LTE                    ((nv_mode_enum_type)55)
/* Service is restricted to TD-SCDMA, GSM or LTE */

#define  NV_MODE_CDMA_HDR_GSM                   ((nv_mode_enum_type)56)
/* Service is restricted to CDMA or GSM or HDR */

#define  NV_MODE_CDMA_GSM                       ((nv_mode_enum_type)57)
/* Service is restricted to CDMA or GSM */

#define  NV_MODE_TDS_GSM_WCDMA_LTE              ((nv_mode_enum_type)58)
/* Service is restricted to TD-SCDMA or GSM or WCDMA or LTE */

#define  NV_MODE_TDS_GSM_WCDMA                  ((nv_mode_enum_type)59)
/* Service is restricted to TD-SCDMA, GSM or WCDMA */

#define  NV_MODE_TDS_LTE                  ((nv_mode_enum_type)60)
/* Service is restricted to TD-SCDMA or LTE */

#define  NV_MODE_CDMA_HDR_GSM_WCDMA_LTE   ((nv_mode_enum_type)61)
/* Service is restricted to CDMA, HDR, GSM, WCDMA or LTE */

#define  NV_MODE_CDMA_GSM_WCDMA_LTE   ((nv_mode_enum_type)62)
/* Service is restricted to CDMA, GSM, WCDMA or LTE */

#define  NV_MODE_TDS_WCDMA   ((nv_mode_enum_type)63)
/* Service is restricted to TDS or WCDMA */

#define  NV_MODE_TDS_WCDMA_LTE                  ((nv_mode_enum_type)64)
/* Service is restricted to TDS or WCDMA */

#define  NV_MODE_CDMA_HDR_GSM_TDS_LTE     ((nv_mode_enum_type)65)
/* Service is restricted to CDMA, GSM, TDS, HDR or LTE */

#define  NV_MODE_PREF_CDMA_GW_TDS     ((nv_mode_enum_type)66)
/* Service is restricted to CDMA, GSM, WCDMA or TDS */

#define  NV_MODE_PREF_CDMA_HDR_GW_TDS     ((nv_mode_enum_type)67)
/* Service is restricted to CDMA, HDR, GSM, WCDMA or TDS */

#define  NV_MODE_PREF_CDMA_HDR_GSM_LTE    ((nv_mode_enum_type)68)
/* Service is restricted to CDMA, GSM, HDR or LTE */

#define  NV_MODE_HDR_TDS_WCDMA     ((nv_mode_enum_type)69)
/* Service is restricted to HDR, TDS or WCDMA */

#define  NV_MODE_HDR_TDS_WCDMA_LTE    ((nv_mode_enum_type)70)
/* Service is restricted to HDR, TDS, WCDMA or LTE */

#define NV_MODE_PREF_ANY_BUT_GSM     ((nv_mode_enum_type)71)

#define NV_MODE_PREF_ANY_BUT_GSM_GPS  ((nv_mode_enum_type)72)


#define NV_MODE_PREF_AMPS_CDMA_HDR_WCDMA_TDS  ((nv_mode_enum_type)73)


#define NV_MODE_PREF_AMPS_CDMA_HDR_GSM_TDS ((nv_mode_enum_type)74)

#include "customer.h"   /* Customer configuration file */
#include "comdef.h"     /* Definition for basic types and macros */
#include "qw.h"         /* FEATURE_JCDMA - Quad word service header file */
#include "sys.h"        /* system wide common types */
#include "sys_v.h"      /* system wide common types */
#include "sys_gw.h"     /* GSM/WCDMA specific types */
#include "cm_gw.h"      /* Defintions from NAS required by clients */
#include "prot.h"       /* Common protocol definitions */


   /**
  ** Alternate Line Service feature default settings (FEATURE_ALS)
  */
#define CM_ALS_DEFAULT_CPHS_SETTING           CM_CPHS_PROC_NOT_ALLOWED

   /**
  ** Alternate Line Service feature default settings (FEATURE_ALS)
  */
#define CM_ALS_DEFAULT_ALS_SETTING            CM_ALS_PROC_NOT_ALLOWED

   /**
  ** Alternate Line Service feature default settings (FEATURE_ALS)
  */
#define CM_ALS_DEFAULT_LINE_SWITCHING         CM_ALS_LINE_SWITCHING_ALLOWED

   /**
  ** Alternate Line Service feature default settings (FEATURE_ALS)
  */
#define CM_ALS_DEFAULT_LINE                   CM_ALS_LINE_01

   /**
  ** No change constant for ph cmd signal delta API
  */
#define CM_SIG_STRENGTH_DELTA_NO_CHANGE        ((uint8)-1)

/*---------------------------------------------------------------------------
** MDM Enums
**---------------------------------------------------------------------------
*/

/* Bit masks indicating which fields are changed
* in cm_mm_msm_cdma_srv_status_s_type structure.
*/
#define CM_MM_CDMA_SRV_SYS_NONE                      0

#define CM_MM_CDMA_SRV_STATUS_MASK                   ( SYS_BM_64BIT(0) )
  /* CDMA Serving Status chagned */

#define CM_MM_CDMA_ROAM_STATUS_MASK                  ( SYS_BM_64BIT(1) )
  /* CDMA Roaming status changed */

#define CM_MM_CDMA_PREF_STATUS_MASK                  ( SYS_BM_64BIT(2) )
  /* CDMA Roaming status changed */

/* Bit masks indicating which fields are changed
* in cm_mm_msm_gw_srv_status_s_type structure.
*/
#define CM_MM_GW_SRV_SYS_NONE                        0

#define CM_MM_GW_SRV_STATUS_MASK                     ( SYS_BM_64BIT(0) )
  /* GW Serving Status Changed */

#define CM_MM_GW_SYS_MODE_MASK                       ( SYS_BM_64BIT(1) )
  /* GW Sys mode, to differentiate between G/W */

#define CM_MM_GW_SRV_DOMAIN_MASK                     ( SYS_BM_64BIT(2) )
  /* GW Serving Domain */

#define CM_MAX_NUM_OF_TAC   16
  /* Max number TAC to be saved from LTE EMM reject */

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** @} */

/****************************************************************************

Call Object

****************************************************************************/

/** @defgroup CALL Call related definitions.
**  @{
*/


#define CM_CALL_ID_ALL_CALLS        ((cm_call_id_type) 0xFD)
  /**< Applies to all calls. Call id is not necessary */

#define CM_SUBADDRESS_TYPE_NSAP     0
  /**< Subaddress type - NSAP (CCITT X.213 / ISO 8348 AD2) */

#define CM_SUBADDRESS_TYPE_USER     1
  /**< Subaddress type - User specified */

#define CM_SUBADDRESS_EO_IND_EVEN   0
  /**< Even number of address signals */

#define CM_SUBADDRESS_EO_IND_ODD    1
  /**< Odd number of address signals  */

#define CM_MAX_CONNECTED_NUM_CHARS  64
/* The following constants are used for GSM/WCDMA.
*/

#define CM_EXTENSION_BIT           0x80
  /**< Indicates octet 3a of the BCD number IE
      (calling party/called party/redirecting party information element)
  */

#define CM_USER_PROVIDED_VER_PASSED 0x01
  /**< User-provided, verified and passed */

#define CM_USER_PROVIDED_VER_FAILED 0x02
  /**< User-provided, verified and failed */

#define CM_NETWORK_PROVIDED         0x03
  /**< Network provided */
#define CM_TON_DEDICATED_ACCESS     (0x04 << 4)
#define CM_TON_RESERVED_6           (0x06 << 4)
  /**< Reserved */

#define CM_TON_RESERVED_EXTENSION   (0x07 << 4)
  /**< Reserved for extension */

/* Numbering Plan Identification
*/
#define CM_NPI_NATIONAL             0x08
#define CM_NPI_RESERVED_CTS         0x0b
#define CM_MAX_SEARCH_LIST_LENGTH   10
  /**< Max length of search list */

#define CM_RP_USER_DATA_TPDU_LENGTH 232
  /**< Max lenght of RP user data */

#define CM_NAS_MN_TPDU_SIZE         164
  /**< Max size of MN TPDU size */

#define CM_NAS_CB_DATA_PAGE_SIZE    88

#define CM_USSD_ALPHABET_MASK       0x0F

#define JCDMA_CALL_THROT_NUM_DEFAULT 4
 /**< default value for JCDMA call throttle number of timer */

#define JCDMA_CALL_THROT_TIMER_DEFAULT 180
 /**< default value for JCDMA call throttle timer */

#define CMSS_RAW_ECIO_VALUE_NO_SIGNAL             63
/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** signal/tone  information from the network (for GSM/WCDMA)
*/
typedef enum cm_gw_tone_e {

  CM_GW_TONE_NONE = -1,
    /**< @internal */

  /* 0 */
  CM_GW_DIAL_TONE_ON,
    /**< dial tone */

  CM_GW_RING_BACK_TONE_ON,
    /**< ring back tone */

  CM_GW_INTERCEPT_TONE_ON,
    /**< intercept tone */

  CM_GW_NW_CONGESTION_TONE_ON,
    /**< network congestion */

  /* 4 */
  CM_GW_BUSY_TONE_ON,
    /**< busy tone */

  CM_GW_CONFIRM_TONE_ON,
    /**< call confirmed tone */

  CM_GW_ANSWER_TONE_ON,
    /**< answer tone */

  CM_GW_CALL_WAITING_TONE_ON,
    /**< call waiting tone */

  /* 8 */
  CM_GW_OFF_HOOK_WARNING_TONE_ON,
    /**< off hook warning tone */

  CM_GW_TONES_OFF = 0x3F,
    /**< no tones */

  CM_GW_ALERTING_TONE_OFF = 0x4F,
    /**< alerting tone off */

  CM_GW_TONE_MAX
    /**< @internal */


} cm_gw_tone_e_type;


/**
** Enumeration of error codes used by cm_num_validate.
*/
typedef enum cm_num_err_e {

  CM_NUM_ERR_NONE = -1,
    /**< @internal */

  CM_NUM_ERR_NOERR,
    /**< No errors found */

  CM_NUM_ERR_NULL_PTR,
    /**< Null pointer */

  CM_NUM_ERR_INVALID_NUM_LENGTH,
    /**< Invalid number length */

  CM_NUM_ERR_INVALID_NUM_TYPE,
    /**< Invalid number type */

  CM_NUM_ERR_INVALID_NUM_PLAN,
    /**< Invalid number plan */

  CM_NUM_ERR_INVALID_PI,
    /**< Invalid presentation indicator */

  CM_NUM_ERR_INVALID_SI,
    /**< Invalid screening indicator */

  CM_NUM_ERR_MAX
    /**< @internal */


} cm_num_err_e_type;

/* Enumeration of events between 2 Modems.
*/
typedef enum cm_mm_event_e {

  CM_MM_EVENT_NONE=-1,
    /* Internal */

  CM_MM_EVENT_CDMA_SRV_STATUS,
    /* A snapshot of CDMA Serving system information */

  CM_MM_EVENT_GW_SRV_STATUS,
    /* A snapshot of GW Serving system information */

  CM_MM_EVENT_MAX
    /* Internal */


} cm_mm_event_e_type;

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/* The following data structures are primarily for passing information
** through the functions. They are not used anywhere else.
*/


/** Type to hold client callbacks for retrieving call compatibilty and
** capability information
*/
typedef struct cm_call_comp_cap_cb_s {

  cm_call_type_e_type           call_type;
    /**< Call type for cb function */

  cm_call_comp_cap_cb_f_type    *call_comp_cap_func_ptr;
    /**< Call compatibility and capability info callback */

} cm_call_comp_cap_cb_s_type;

/* This is the type to provide quick serving system information to those
** clients who can't register as CM-clients. For example libraries.
** This gives service information for any given technology:
**   MAIN - 1x/GW/GWL
**   HYBR_1 - HDR
**   HYBR_2 - GWL
*/
typedef struct cm_service_status_s {

  sys_srv_status_e_type srv_status;
    /* Service status on the given service stack */

  sys_sys_mode_e_type   sys_mode;
    /* System mode for service indicated in srv_status */

  sys_band_class_e_type band;
    /* Active band class info for the service */

  sys_channel_num_type  chan;
    /* Active chan info for the service */

}cm_service_status_s_type;

typedef struct cm_curr_sys_mode_s  {

  boolean                    is_hybrid;
    /**< system is hybrid or not.
    */

  sys_modem_as_id_e_type     main_as_id;
    /**< The subscription-id of Main service info.
    */

  sys_srv_status_e_type      main_srv_status;
    /**< The service status of the system on main stack
    */

  sys_sys_mode_e_type        main_sys_mode;
    /**< The system's mode on main stack.
     ** if main_srv_status is SYS_SRV_STATUS_SRV,it is the current serving system mode.
     ** else if main_srv_status is SYS_SRV_STATUS_NO_SRV, it is the mode under acquiring.
     ** else it is SYS_SYS_MODE_NO_SRV
    */

  sys_roam_status_e_type     main_roam_status;
    /**< Indicate current roaming status of main stack
    */

  sys_band_class_e_type      band;
    /**< active band of current Main serving system */

  sys_channel_num_type       channel;
    /**< active channel of current Main serving system */

  sys_srv_status_e_type      hybrid_srv_status;
    /**< The service status of the system on hybrid stack
    */

  sys_sys_mode_e_type        hybrid_sys_mode;
    /**< The system's mode on hybrid stack.
     ** if hybrid_srv_status is SYS_SRV_STATUS_SRV,it is the current serving system mode.
     ** else if hybrid_srv_status is SYS_SRV_STATUS_NO_SRV, it is the mode under acqing.
     ** else it is SYS_SYS_MODE_NO_SRV
    */

  sys_roam_status_e_type     hybrid_roam_status;
    /**< Indicate current roaming status of hybrid stack
    */

  sys_band_class_e_type      hybrid_band;
   /**< active band of current Hybrid serving system */

  sys_channel_num_type       hybrid_channel;
   /**< active band of current Hybrid serving system */

  sys_modem_as_id_e_type     hybrid_gw_as_id;
    /**< The subscription-id of Hybr GW service info.
    */

  sys_srv_status_e_type      hybrid_gw_srv_status;
    /**< The service status of the system on hybrid stack
    */

  sys_sys_mode_e_type        hybrid_gw_sys_mode;
    /**< The system's mode on hybrid stack.
     ** if hybrid_srv_status is SYS_SRV_STATUS_SRV,it is the current serving system mode.
     ** else if hybrid_srv_status is SYS_SRV_STATUS_NO_SRV, it is the mode under acqing.
     ** else it is SYS_SYS_MODE_NO_SRV
    */

  sys_band_class_e_type      hybrid_gw_band;
   /**< active band of current Hybrid GW serving system */

  sys_channel_num_type       hybrid_gw_channel;
   /**< active band of current Hybrid GW serving system */

}cm_curr_service_status_s_type;

/**---------------------------------------------------------------------------
** MDM Data structures
**---------------------------------------------------------------------------
*/
/*
** Type to hold the CDMA serving system information of the other modem.
*/
typedef struct cm_mm_cdma_srv_status_s {

  uint64                          changed_fields;
    /* Bit mask indicating which of the following serving system fields
    * were changed since last notification.  Refer to the #defines
    * (CM_MM_*_MASK) above.
    */

  sys_srv_status_e_type           srv_status;
    /* The service status of the system.
    */

  sys_roam_status_e_type          roam_status;
    /* Roaming status
    */

  boolean                         is_most_pref;
    /* Indicates if the CDMA service status is more preferred than the
    ** MDM service status
    */

} cm_mm_cdma_srv_status_s_type;

/*
** Type to hold the GW serving system information of the other modem.
*/
typedef struct cm_mm_gw_srv_status_s {

  uint64                          changed_fields;
    /* Bit mask indicating which of the following serving system fields
    * were changed since last notification.  Refer to the #defines
    * (CM_MM_*_MASK) above.
    */

  sys_srv_status_e_type           srv_status;
    /* The service status of the system.
    */
  sys_sys_mode_e_type             sys_mode;
      /* The current sys mode acquired
      */
  sys_srv_domain_e_type           srv_domain;
      /* The current srv domain of the system
      */

} cm_mm_gw_srv_status_s_type;


/*
** Union to hold all the information of the other modem.
** When notified of event, clients are given a reference to such a structure.
*/
typedef  struct cm_mm_info_s {

  cm_client_id_type             requesting_client_id;
    /**< Requesting client */

  cm_mm_cdma_srv_status_s_type  cdma_srv_status;
    /* CDMA Serving system
    */

  cm_mm_gw_srv_status_s_type    gw_srv_status;
    /* GW Serving system
    */

} cm_mm_info_s_type;

/** Type for event callback function.
**
** A client that want to be notified of the other modem information needs to
** register with such a function with CM.
*/
typedef void (cm_mm_event_f_type) (

  cm_mm_event_e_type              event,
    /* Indicate the type of modem to modem event */

  const cm_mm_info_s_type         *info_ptr
   /* A pointer to a modem to modem information structure
   */

);

typedef struct
{
  boolean        use_ue_based_cw;
  /* indicate if UE is configured as UE based call waiting. UE based CW setting will overide network based CW*/
  boolean        is_enabled;
  /* indicate if UE based call waiting is enabled or not */
}cm_ue_based_cw_s_type;

typedef PACKED struct PACKED_POST
{
  word                   tac;
  /* Each TAC takes two octets */

  sys_plmn_id_s_type     plmn;
  /* PLMN ID */
}cmrpm_lte_tai_lst_s_type;

typedef PACKED struct PACKED_POST{
  
  uint8                     counter;
  /* EMM Reject 15 per-TAC counter */

  cmrpm_lte_tai_lst_s_type  lte_tai;
  /* LTE TAI list */

}cmrpm_lte_emm_15_s_type;


/* This holds RPM parameter information stored in CMNV_EFS_ID_LTE_RPM_INFO_2
** Need to declare as PACKED since this structure is also stored in CM-EFS.
*/
typedef PACKED struct PACKED_POST{

  uint8                    version;
  /* Version Number */

  boolean                  is_rpm_enabled;
  /* Indicate whether RPM functionality should be enabled or not */

  uint8                    max_event_counter;
  /* Maximum allowable number for LTE RPM EMM reject event */

  uint8                    last_emm_reject_cause;
  /* Stores the last EMM reject cause */

  uint8                    emm_gen_counter;
  /* Counter for received EMM reject cause 3/6/7/8 from NAS; 
   * EMM reject cause 15 will be handled in emm_15_counter
   */

  uint16                   max_event_timer;
  /* Maximum allowable event tracking time for LTE RPM */

  uint32                   event_timer; 
  /* Timer for tracking RPM EMM reject event. Will store time in secs */

  cmrpm_lte_emm_15_s_type  emm_15_reject[CM_MAX_NUM_OF_TAC];
  /* Per-TAC counter and [tac, plmn] pair for received 
   * EMM reject cause 15 from NAS 
   */

  /* Version 0. First Version must be 0 */
  uint8 reserved[16];
  
}cmrpm_lte_efs_rpm_info_s_type;

typedef enum cm_sub_clnup_reason_e {

  CM_SUB_CLNUP_REASON_NONE=-1 ,    /* Internal bound checking. */

  CM_SUB_CLNUP_REASON_SUBS_NOT_AVAIL,    /* 0 subscription is unavailable(UIM triggered) */

  CM_SUB_CLNUP_REASON_SUB_DEACTIVATED,    /* 1 Subscription is being deactivated(MCFG deactivate) */

  CM_SUB_CLNUP_REASON_STANDBY_PREF_CHANGE,  /* 2- Cleanup needed as standby preference
                                              changed removing a particular subcription*/

  CM_SUB_CLNUP_REASON_MAX  /* Internal bound checking. */

} cm_sub_clnup_reason_e_type;

/* Type for DS mode oprt change notification callback function. CM will notify DS
** oprt mode change when PWROFF/LPM. And wait for DS tear down data traffic
** to continue command processing.
** Return value:
**   TRUE -CM will hold oprt mode command until DS's notification or timeout(5s)
**   FALSE - CM will continue processing the oprt mode command without waiting
*/
typedef boolean (cm_ds_sys_pwroff_lpm_notif_f_type)( void );

typedef boolean (cm_ds_cmd_notif_ext_f_type)( uint8,uint8[]);

/*---------------------------------------------------------------------------
** Macros
**-------------------------------------------------------------------------*/

/** Macro to decide whether a given call type is an OTASP
** (standard or none standard ).
*/
#define CM_CALL_TYPE_IS_OTASP( call_type )  \
              ((call_type) == CM_CALL_TYPE_STD_OTASP    || \
                                (call_type) == CM_CALL_TYPE_NON_STD_OTASP   )

#define CM_CALL_TYPE_IS_OTAPA( call_type )  \
              ((call_type) == CM_CALL_TYPE_OTAPA)

/** Macro to return the length of a CM number.
*/
#define CM_NUM_LEN( num_ptr )  ( (num_ptr)->len )


/** Macro to return the Number type of a call manager number
*/
#define CM_NUM_NUMBER_TYPE( num_ptr )  ( (num_ptr)->number_type )


/** Macro to return the Numbering Plan of a CM number
*/
#define CM_NUM_NUMBER_PLAN( num_ptr )  ( (num_ptr)->number_plan )


/** Macro to return the Presentation Indicator of a CM number
*/
#define CM_NUM_PI( num_ptr )  ( (num_ptr)->pi )


/** Macro to return the Screening Indicator of a CM number
*/
#define CM_NUM_SI( num_ptr )  ( (num_ptr)->si )


/** Macro to return the Origination digit mode of a CM number
*/
#define CM_NUM_DIGIT_MODE( num_ptr )  ( (num_ptr)->digit_mode )


/*---------------------------------------------------------------------------
** Functions
**-------------------------------------------------------------------------*/

/* CM command functions
**
** The CM command functions defines the Client->CM interface.
**
** Each command function takes a pointer to a client provided callback
** function, a client provided void pointer and the command's required set
** of fields.
**
** If the command function find one or more of the parameters to contain
** invalid values ( e.g. not one of the enum values for an enum parameter),
** it calls on the client's callback function IMMEDIATELY with the
** appropriate command error code. NOTE that in this case the callback
** function is actually being called from the client context - NOT CM.
**
** If all parameters found to be valid, the command function queues a
** command to CM. When CM gets the command it checks whether the command
** is valid relative to the current state of the phone/call.
**
** In case the command is not valid (e.g. ending a call that was already
** ended), CM calls on the client callback function with the appropriate
** command error code. NOTE that in this case the callback function is
** indeed being called from the CM context.
**
** If the command is successful, CM calls on the client callback function
** with an error code that indicates successful command. In addition CM
** notified registered clients of the corresponding CM event (e.g. if a
** client successfully ends a call, CM notify clients that are registered
** for the call-end-event of this event).
*/


/**===========================================================================

@FUNCTION cm_mm_call_cmd_end2

@DESCRIPTION
  Command CM to end a call - in origination, incoming or conversation state.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_end2(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< ending client */

  uint8                  num_call_ids,
    /**< number of call IDs sent */

  cm_end_params_s_type   *end_params_ptr,
    /**< pointer to mode specific data */

  sys_msm_id_e_type      msm_id
    /**< MSM id to which this command is directed.
    */

);


/**===========================================================================

@FUNCTION cm_mm_call_cmd_pdp_act_res

@DESCRIPTION
  Response to an Incoming PDP activation request.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_pdp_act_res(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id,
    /**< answering client */

  cm_call_id_type                call_id,
    /**< call ID of the call */

  const cm_pdp_act_res_params_s_type   *act_res_params_ptr
    /**< pointer to mode specific data */

);


/*===========================================================================

@FUNCTION cm_mm_call_cmd_umts_cdma_handover_req

@DESCRIPTION
  Command CM to complete the handoff from UMTS -> 1x.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_umts_cdma_handover_req
(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< originating client */

  cm_call_type_e_type    call_type,
    /**< call type - voice, data, etc. */

  cm_call_state_e_type   call_state,
    /**< Call state - orig, incoming or conv */

  const sys_is2000_msg_s_type  *ho_msg_ptr,
    /**< Pointer to CDMA message from UTRAN */

  cm_call_id_type        *return_call_id_ptr
    /**< call ID will be allocated and returned */

);


/**===========================================================================

@FUNCTION cm_mm_call_cmd_umts_cdma_handover_cnf

@DESCRIPTION
  Command to CM to inform about the handover status.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request
  Call_id will be returned in call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_umts_cdma_handover_cnf(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< originating client */

  cm_call_id_type             call_id,
    /**< call ID of the call */

  sys_umts_cdma_handover_e_type  status
    /**< Handover status */

);

/** @}
** endgroup of CALL group.
*/

/****************************************************************************

Number Classification Types & Functions

****************************************************************************/

/** @defgroup NCTF Number Classification Types & Functions
**                NUMBER CLASSIFICATION TYPES & FUNCTIONS
** This block group all the functionality that is associated with
** number-classification interfacing.
**
** When call manager is commanded to originate a call, it consults the
** number classification of the following information:
**
** 1. Whether the originating number matches one of the special
**    numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
**    If a match is found, CM modify the originating call type accordingly.
**
** 2. After deciding on the originating call type, CM consults the
**    number-classification whether the originating number is allowed by the
**    origination restriction currently in effect. If originating number
**    is not allowed, CM blocks the outgoing call and notifies clients of
**    origination failure.
**
** Note that CM expects a UI like client to register a number-classification callback
** functions with CM during initialization time.
**
** Until such callback functions are registered with CM, CM uses a default
* hard-coded number-classification to decide on the call type of special numbers.


@{
*/

/** Type for number-classification callback function to determine whether an originating
** number matches one of the special number-classification number, such as EMERGENCY or
** OTASP. If a match is found, this function should indicate the originating
** call type via the call_type_ptr and if applicable the OTASP activation
** code via the activate_code_ptr.
*/
typedef void ( cm_nc_resolve_call_type_f_type ) (

  const cm_num_s_type      *orig_num_ptr,
    /**< originating number */

  cm_call_type_e_type      *call_type_ptr,
    /**< Associated call type */

  cm_activate_code_e_type  *activate_code_ptr
    /* Associated OTASP activation code (for OTASP call types) */

);


/** Type for number-classification callback function to determine whether an originating
** number is allowed by the origination restriction currently in effect. If
** originating number is allowed this function should return TRUE, otherwise
** this function should return FALSE.
*/
typedef boolean ( cm_nc_is_call_allowed_f_type ) (

  const cm_num_s_type      *orig_num_ptr
    /**< originating number */

);


/** Type for number-classification callback function to insert or delete 184 or 186
** number to original dial number strings as pre-pending dialing.
*/
typedef void ( cm_nc_pre_pending_dial_f_type ) (

  cm_num_s_type                  *orig_num_ptr,
    /**< originating number */

  const cm_call_type_e_type      *call_type_ptr
    /**< Associated call type */

);



/** Type for number classification callback function to
    determine whether an originating number matches one of the
    special number classification numbers, such as Emergency or
    OTASP. If a match is found, this function indicates the
    originating call type via the call_type_ptr and, if
    applicable, the OTASP activation code via the
    activate_code_ptr.
*/
typedef void ( cm_nc_resolve_call_type_per_subs_f_type ) (

  const cm_num_s_type      *orig_num_ptr,
    /**< Originating number. */

  cm_call_type_e_type      *call_type_ptr,
    /**< Associated call type. */

  cm_activate_code_e_type  *activate_code_ptr,
    /* Associated OTASP activation code (for OTASP call types). */

  sys_modem_as_id_e_type   asubs_id
    /* Active subscription ID. */
);


/** Type for a number classification callback function to
    determine whether an originating number is allowed by the
    origination restriction currently in effect. If the
    originating number is allowed, this function returns
    TRUE, otherwise it returns FALSE.
*/
typedef boolean ( cm_nc_is_call_allowed_per_subs_f_type ) (

  const cm_num_s_type      *orig_num_ptr,
    /**< Originating number. */

  sys_modem_as_id_e_type   asubs_id
    /**< Active subscription ID. */
);


/**===========================================================================

@FUNCTION cm_nc_reg

@DESCRIPTION
  Register number-classification callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classification callback functions of the following information:

  1. Whether the originating number matches one of the special
     numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. After deciding on the originating call type, CM consults the
     number-classification whether the originating number is allowed by the
     origination restriction currently in effect. If originating number
     is not allowed, CM blocks the outgoing call and notifies clients of
     origination failure.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_nc_status_e_type cm_nc_reg(

  cm_nc_resolve_call_type_f_type     *resolve_call_type_func,
    /**< pointer number-classification function resolving call types */

  cm_nc_is_call_allowed_f_type       *cm_nc_is_call_allowed_func,
    /**< pointer to a number-classification deciding whether a specific call type
    ** is allowed for origination */

  cm_nc_pre_pending_dial_f_type      *pre_pending_dial_func
    /**< pointer pre-pending function resolving origination # */

);



/** @} */

/****************************************************************************

Phone Object

****************************************************************************/

/** @defgroup PHONE Phone related definitions.
** @{
*/


/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/* Default avoid time for avoid system command from UI == 100 hours
*/
#define CM_DFLT_AVOID_SYS_TIME  (360000)

#define CM_MMGSDI_MAX_UICC_LEN    50

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/**===========================================================================

@FUNCTION cm_ph_cmd_oprt_mode2

@DESCRIPTION
  Command CM to put phone in a specified operating mode, such as offline,
  online, LPM, etc with msm_id information.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_oprt_mode2(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< Requesting client */

  sys_oprt_mode_e_type   oprt_mode,
    /**< Switch to this operating mode */

  sys_msm_id_e_type      msm_id
    /**< MSM to which this command has to be sent.
    */
);


/*===========================================================================

FUNCTION cm_ph_get_oprt_mode

DESCRIPTION
  Command CM to get the specified operating mode of the phone, such as offline,
  online, LPM, etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

  *** THIS API IS TO REMOVE CMI DEPENDENCY ON WCDMA AND IS TEMPORARY ***

RETURN VALUE
  SYS_OPRT_MODE_PWROFF
  SYS_OPRT_MODE_FTM
  SYS_OPRT_MODE_OFFLINE
  SYS_OPRT_MODE_OFFLINE_AMPS
  SYS_OPRT_MODE_OFFLINE_CDMA
  SYS_OPRT_MODE_ONLINE
  SYS_OPRT_MODE_LPM
  SYS_OPRT_MODE_RESET
  SYS_OPRT_MODE_NET_TEST_GW
  SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM
  SYS_OPRT_MODE_PSEUDO_ONLINE
  SYS_OPRT_MODE_RESET_MODEM

SIDE EFFECTS
  None

===========================================================================*/
extern sys_oprt_mode_e_type cm_ph_get_oprt_mode( void );


/**===========================================================================

@FUNCTION cm_ph_cmd_voc_activity

@DESCRIPTION
  Command CM to activate voice activity test for current or next voice call.
  On call termination, the voice activity test will be disabled.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_JCDMA must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_ph_cmd_voc_activity(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< Requesting client */

  boolean                enable,
    /**< enable/disable voice activity test */

  uint16                 voc_1,
    /**< # of full rate */

  uint16                 voc_2,
    /**< # of 1/2 rate */

  uint16                 voc_8
    /**< # of 1/8 rate */

);



/** @}
** endgroup of PHONE group.
*/

/****************************************************************************

Serving System Object

****************************************************************************/

/** @defgroup SSO Serving System Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/
/* apitrim -include */
/* apitrim -end-include */
#define CM_MAX_MOBILE_COUNTRY_CODE_VAL                1000
   /* Mobile country code cannot exceed this value.
   ** Mirrors SD_MCC_MAX value
   */


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/** Enumeration of avoid system type.
*/
typedef enum cm_avoid_sys_e_type {

  CM_AVOID_SYS_SRV_IF_UZ, /* =  SD_SS_AVOID_SYS_SRV_IF_UZ,*/
    /**< Avoid serving system only if MS has currently selected
    ** user zone */

  CM_AVOID_SYS_SRV,       /* =  SD_SS_AVOID_SYS_SRV, */
    /**< Avoid serving system */

  CM_AVOID_SYS_MAX       /* =  SD_SS_AVOID_SYS_MAX */
    /**< @internal */



} cm_avoid_sys_e_type;

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** Orig throttle SS information
*/
typedef union cm_ss_orig_thr_ss_info_u {

  cm_ss_cdma_info_s_type   cdma_info;
    /**< CDMA SS information */

} cm_ss_orig_thr_ss_info_u_type;


/** Type to hold the origination call status information
*/
typedef struct cm_ss_orig_throttle_call_status_s
{
  cm_call_type_e_type                 call_type;
    /**< Type of the call, e.g PS_DATA
    */

  sys_sys_id_s_type                   sys_id;
    /**< The sys id of the system, SID,NID
    */

  cm_ss_orig_thr_ss_info_u_type       mode_info;
    /**< Mode specific serving system information
    */

  cm_ss_orig_thr_reasons_e_type       call_status;
    /**< The call end reason
    */
} cm_ss_orig_thr_call_status_s_type;


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/


/** @}
*  endgroup of Serving System Object
*/


/****************************************************************************

Inband Object

****************************************************************************/

/** @defgroup IO Inband Object
** @{
*/

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/


/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/** @}
*  endgroup of Inband Object
*/


/****************************************************************************

Short Messaging Service Object
** For internal use (uasms) only **

****************************************************************************/

/** @defgroup SMSO Short Messaging Service Object
** @{
*/


/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/
#define CM_RP_USER_DATA_TPDU_LENGTH       232

#define CM_NAS_MN_TPDU_SIZE               164

/** Enumeration of type of NAS command
** Clients should check for CM_API_REMOTE_WMS
** before using this enum.
*/
typedef enum cm_nas_cmd_e {

  CM_NAS_CMD_NONE=-1,
    /**< @internal */

  CM_NAS_CMD_REQ,
    /**< FEATURE_GSM || FEATURE_WCDMA */

  CM_NAS_CMD_MAX
    /**< @internal */


} cm_nas_cmd_e_type;


/** Enumeration of NAS command errors returned by NAS command
** callback functions.
** Clients need to check for CM_API_REMOTE_WMS before using
** this enum.
*/
typedef enum cm_nas_cmd_err_e {

  CM_NAS_CMD_ERR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_CMD_ERR_NOERR,

  /* Error codes resulting from clients feeding a serving system command
  ** function with bad or inappropriate parameters.
  */

  CM_NAS_CMD_ERR_CLIENT_ID_P,
    /**< FEATURE_CDSMS - Bad client parameter */

  CM_NAS_CMD_ERR_NULL_PARAM_P,
    /**< Null pointer parameter */

  /* Error codes resulting from operational limitation of CM.
  */

  CM_NAS_CMD_ERR_NO_BUF_L,

  /* 4 */
  CM_NAS_CMD_ERR_OTHER,

  CM_NAS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_NAS_CMD_ERR_MAX
    /**< @internal */


} cm_nas_cmd_err_e_type;



/** Enum of cell change types from NAS to WMS
** Clients should check for CM_API_NAS_CELL_CHANGE_TYPE
** before using this enum.
*/
typedef enum cm_nas_cell_change_type_e
{
  CM_NAS_CELL_CHANGE_NONE=-1,
    /**< FOR INTERNAL CM USE ONLY! */

  CM_NAS_CELL_CHANGE_NO_SERVICE,
   /**< No service */

  CM_NAS_CELL_CHANGE_SAME_CELL,
    /**< Same cell. No change */

  CM_NAS_CELL_CHANGE_CELL_CHANGE,
    /**< Cell has changed */

  CM_NAS_CELL_CHANGE_LAC_CHANGE,
    /**< LAC has changed */

  CM_NAS_CELL_CHANGE_PLMN_CHANGE,
    /**< PLMN has changed */

  CM_NAS_CELL_CHANGE_MAX
    /**< FOR INTERNAL CM USE ONLY! */

} cm_nas_cell_change_type_e_type;

/** Enumeration of NAS events that a client can register for.
** Clients need to check for CM_API_REMOTE_WMS before using
** this enum.
*/
typedef enum cm_nas_event_e
{

  CM_NAS_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_EVENT_SM_DATA_IND,
    /**< Data Indication from SM */

  CM_NAS_EVENT_SM_REPORT_IND,
    /**< Report Indication from SM */

  CM_NAS_EVENT_CB_DATA_IND,
    /**< Data Indication from CB */

  CM_NAS_EVENT_CB_CELL_CHANGE_IND,
    /**< Cell Change Indication from CB */

  /* 4 */
  CM_NAS_EVENT_CB_DUPLICATION_DETECTION_IND,
    /**< Duplicate Detection Indication from CB */

  CM_NAS_EVENT_SM_LINK_CONTROL_IND,
    /**< Link Control Indication from SM */

  CM_NAS_EVENT_MAX
   /**< @internal */


} cm_nas_event_e_type;

/** Enumeration of NAS MN Link Control modes
*/
typedef enum cm_nas_mn_link_control_mode_e
{

  CM_NAS_MN_LINK_CONTROL_MODE_NONE=-1,
    /**< @internal */

  CM_NAS_MN_LINK_CONTROL_MODE_DISABLED,
    /**< MN Link control disabled */

  CM_NAS_MN_LINK_CONTROL_MODE_ENABLED_ONE,
    /**< MN Link control one enabled */

  CM_NAS_MN_LINK_CONTROL_MODE_ENABLED_ALL,
    /**< MN Link control all enabled */

  CM_NAS_MN_LINK_CONTROL_MODE_MAX
    /**< @internal */


}  cm_nas_mn_link_control_mode_e_type;

/** Enumeration of NAS Errors
*/
typedef enum cm_nas_error_e
{

  CM_NAS_ERROR_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_NO_ERROR_RP_ACK,
    /**< No Error RP Ack */

  CM_NAS_NO_ERROR_ABORT_SM_COMPLETE,
    /**< No Error on Abort */

  CM_NAS_NO_ERROR_SMMA_ABORT_SENT,
    /**< No Error Abort Sent */

  CM_NAS_SMR_ERROR_NO_SM_RESOURCES,
    /**< Error No SM Resourcees */

  /* 4 */
  CM_NAS_SMR_ERROR_TR1M_TIMEOUT,
    /**< Error TR1M timeout */

  CM_NAS_SMR_ERROR_TR2M_TIMEOUT,
    /**< Error TR2M timeout */

  CM_NAS_SMS_RP_ERROR,
    /**< Error SMS RP */

  CM_NAS_SMS_CP_ERROR,
    /**< Error SMS CP */

  /* 8 */
  CM_NAS_SMS_LL_ERROR,
    /**< Error SMS LL */

  CM_NAS_SMS_NO_RESPONSE_FROM_NETWORK,
    /**< No response from Network */

  CM_NAS_NO_ERROR_CP_ACK,
    /**< ACK from NAS to WMS */

  CM_NAS_SMS_LL_MAX
    /**< @internal */


} cm_nas_error_e_type;

/** Enumeration of NAS MN Link Control Status
*/
typedef enum cm_nas_mn_link_control_status_e
{

  CM_NAS_MN_LINK_CONTROL_STATUS_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_MN_LINK_CONTROL_STATUS_INACTIVE,
    /**< MN Link Control Inactive */

  CM_NAS_MN_LINK_CONTROL_STATUS_ACTIVE,
    /**< MN Link Control Active */

  CM_NAS_MN_LINK_CONTROL_STATUS_TIMEOUT,
    /**< MN Link Control Timed Out */

  CM_NAS_MN_LINK_CONTROL_STATUS_CONNECTION_RELEASED,
    /**< MN Link Control Released */

  /* 4 */
  CM_NAS_MN_LINK_CONTROL_STATUS_MAX
    /**< @internal */

} cm_nas_mn_link_control_status_e_type;


/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/** Structure of SM Data Indication Information returned from a
*  NAS event. Clients need to check for CM_API_REMOTE_WMS before
*  using this struct.
*/
typedef struct cm_nas_sm_rl_data_ind_s
{

  uint8                               connection_id;
    /**< Connection Id */

  cm_called_party_bcd_no_s_type       RP_OA;
    /**< RP OA */

  uint8                               tpdu_length;
    /**< Length of tpdu buffer */

  uint8                               tpdu[ CM_RP_USER_DATA_TPDU_LENGTH ];
    /**< tpdu */

} cm_nas_sm_rl_data_ind_s_type;

/** Structure of SM RL report indication returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_sm_rl_report_ind_s
{

  uint16                               short_message_identifier;
    /**< Short Message Identifier */

  cm_nas_error_e_type                  error_status;
    /**< Error Status */

  uint8                                cause_value;
    /**< Cause Value */

  boolean                              diagnostics_present;
    /**< Diagnostics present */

  uint8                                diagnostics;
    /**< Diagnostics */

  uint8                                tpdu_length;
    /**< Length of tpdu buffer */

  uint8                                tpdu[ CM_RP_USER_DATA_TPDU_LENGTH ];
    /**< tpdu */

  boolean                              permanent_error;
    /**< Indicates any access class restriction or sim invalidated information*/

  sys_srv_domain_e_type                sms_srv_domain;
    /**< Domain in which SMS is tried */


} cm_nas_sm_rl_report_ind_s_type;

/** Structure of SM RL Link Control report returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_sm_rl_link_control_report_s
{

  cm_nas_mn_link_control_mode_e_type                  mode;
    /**< Link Control Mode */

  cm_nas_mn_link_control_status_e_type                status;
    /**< Link Control Status */

} cm_nas_sm_rl_link_control_report_s_type;

/** Structure of CB data indication returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_data_ind_s
{

  uint8                               page_length;
    /**< Page Length */

  uint8                               cb_page[ CM_NAS_CB_DATA_PAGE_SIZE ];
    /**< CB page */

} cm_nas_cb_data_ind_s_type;

/** Structure of CB duplication detection returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_duplication_detection_ind_s
{

  uint16                            message_id;
    /**< Message Identifier */

  uint16                            message_code;
    /**< Message Code */

  uint8                             gs_indicator;
    /**< Geographic scope */

  uint8                             update_number;
    /**< Update Number */

  uint8                             page_number;
    /**< Page Number */

  uint8                             number_of_pages;
    /**< Total number of pages */

  uint8                             data_coding_scheme;
    /**< Data coding Scheme */


} cm_nas_cb_duplication_detection_ind_s_type;

/** Structure of information returned from a NAS event.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_info_s {

  cm_client_id_type                          nas_client_id;
    /**< Client Id */

  cm_nas_sm_rl_data_ind_s_type               rl_data_ind;
    /**< SM Data Indication Information */

  cm_nas_sm_rl_report_ind_s_type             rl_report_ind;
    /**< SM Report Indication Information */

  cm_nas_sm_rl_link_control_report_s_type    rl_link_control_report_ind;
    /**< SM Link Control Report Information */

  cm_nas_cb_data_ind_s_type                  cb_data_ind;
    /**< CB Data Indication Information */

  cm_nas_cb_duplication_detection_ind_s_type cb_duplicate_ind;
    /**< CB Duplication Detection Information */

  cm_client_id_type                          requesting_client_id;
    /**< Requesting client */

  cm_nas_cell_change_type_e_type             cell_change_type;
  /**< Cell Change Type
  ** Clients should check for CM_API_NAS_CELL_CHANGE_TYPE before using this
  ** field.
  */

} cm_nas_info_s_type;


/** Type to describe a CB message id range.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_message_id_value {

  uint16                          from;
    /**< from message id */

  uint16                          to;
    /**< to message id */

} cm_nas_cb_message_id_value_type;

/** Type to hold a CB search list request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_search_list_req {

  byte                             search_list_length;
    /**< length of the search list */

  cm_nas_cb_message_id_value_type  search_list[ CM_MAX_SEARCH_LIST_LENGTH ];
    /**< pointer to the search list */

  boolean reading_advised ;
    /**<
    ** Boolean flag indicating whether BMC task is going to
    ** process "Reading Advised" SFNs of the BMC Scheduling Message
    */

  boolean reading_optional ;
    /**<
    ** Boolean flag indicating whether BMC task is going to
    ** process "Reading Optional" SFNs of the BMC Scheduling Message
    */

} cm_nas_cb_search_list_req_type;

/** Type to hold a CB delete message request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_cb_delete_msg_req {

  uint16              message_id;
    /**< message id */

  uint16              message_code;
    /**< message code */

  uint8               page_number;
    /**< page number */

} cm_nas_cb_delete_msg_req_type;


/** Enumeration of MM Bearer Selections.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef enum cm_nas_mm_bearer_selection_e
{
  CM_NAS_MM_BEARER_SELECTION_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_MM_BEARER_SELECTION_CS_PREFERRED,
    /**< Circuit Switch Preferred */

  CM_NAS_MM_BEARER_SELECTION_PS_PREFERRED,
    /**< Packete Switch Preferred */

  CM_NAS_MM_BEARER_SELECTION_CS_ONLY,
    /**< Circuit Switch Only */

  CM_NAS_MM_BEARER_SELECTION_PS_ONLY,
    /**< Packet Switch Only */

  /* 4 */
  CM_NAS_MM_BEARER_SELECTION_MAX
    /**< @internal */


} cm_nas_mm_bearer_selection_e_type;

/** Type to hold a MN RL Memory Available Request
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_mn_rl_memory_available_req
{

  uint16       short_message_identifier;
    /**< Short Message identifier */

  boolean      abort;
    /**< Whether to abort or not */

} cm_nas_mn_rl_memory_available_req_type;

/** Type to hold a MN RL Report Request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_mn_rl_report_req
{

  uint8         connection_id;
    /**< Connection Id */

  boolean       sm_rl_success;
    /**< Status */

  uint8         cause_value;
    /**< Cause Value */

  boolean       diagnostics_present;
    /**< Diagnostics Present */

  uint8         diagnostics;
    /**< Diagnostics */

  uint8         tpdu_length;
    /**< Length of TPDU */

  uint8         tpdu[ CM_NAS_MN_TPDU_SIZE ];
    /**< TPDU */

} cm_nas_mn_rl_report_req_type;

/** Type to hold a MN RL Data Request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_mn_rl_data_req
{

  uint8                            short_message_identifier;
    /**< Short Message Identifier */

  cm_called_party_bcd_no_s_type    RP_DA;
    /**< RP DA */

  uint8                            tpdu_length;
    /**< Length of TPDU */

  uint8                            tpdu[ CM_NAS_MN_TPDU_SIZE ];
    /**< TPDU */

} cm_nas_mn_rl_data_req_type;

/** Type to hold a MN RL Link Control Request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_mn_rl_link_control_req
{

  uint8                               timeout_in_seconds;
    /**< Timeout */

  cm_nas_mn_link_control_mode_e_type  mode;
    /**< Mode of Link Control */

} cm_nas_mn_rl_link_control_req_type;

/** Enumeration of NAS Request Types.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef enum cm_nas_req_e
{

  CM_NAS_REQ_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_NAS_REQ_MN_RL_REPORT,
    /**< MN RL Report */

  CM_NAS_REQ_MN_RL_DATA,
    /**< MN RL Data */

  CM_NAS_REQ_MN_RL_MEMORY_AVAILABLE,
    /**< MN Memory Available Req */

  CM_NAS_REQ_MN_RL_LINK_CONTROL,
    /**< MN Link Control Req */

  /* 4 */
  CM_NAS_REQ_MM_BEARER_SELECTION,
    /**< MM Bearer Selection Req */

  CM_NAS_REQ_CB_SEARCH_LIST,
    /**< CB Search List Req */

  CM_NAS_REQ_CB_DELETE_MESSAGE,
    /**< CB Delete message Req */

  CM_NAS_REQ_CB_DELETE_ALL_MESSAGES,
    /* CB Delete all messages Req */

  CM_NAS_REQ_MAX
    /**< @internal */


} cm_nas_req_e_type;

/** Type to hold the NAS request parameters.
** Clients need to check for CM_API_REMOTE_WMS before using this
** union.
*/
typedef union cm_nas_req_param_u
{
  cm_nas_mn_rl_report_req_type               rl_report_req;
    /**< MN Report Req */

  cm_nas_mn_rl_data_req_type                 rl_data_req;
    /**< MN Data Req */

  cm_nas_mn_rl_memory_available_req_type     memory_available_req;
    /**< MN Memory Available Req */

  cm_nas_mn_rl_link_control_req_type         rl_link_control_req;
    /**< MN Link Control Req */

  cm_nas_mm_bearer_selection_e_type          bearer_selection_req;
    /**< MM Bearer Selection Req */

  cm_nas_cb_search_list_req_type             search_list_req;
    /**< CB Search List Req */

  cm_nas_cb_delete_msg_req_type              delete_msg_req;
    /**< CB Delete Msg Req */

} cm_nas_req_param_u_type;





/** Type to hold a NAS Request.
** Clients need to check for CM_API_REMOTE_WMS before using this
** struct.
*/
typedef struct cm_nas_req
{

  cm_nas_req_e_type                   req;
    /**< Type of Req */

  cm_nas_req_param_u_type             param;
    /**< Parameters according to type of req */

} cm_nas_req_type;

/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/


/** Type for NAS command callback functions.
**
** A client that wants to be notified of the result of a
** nas-command-function needs to supply a pointer to a function
** of this type as the first parameter to a inband-command-function.
*/
typedef void (cm_nas_cmd_cb_f_type) (

  void                        *data_block_ptr,
    /**< Pointer to client provided data block */

  cm_nas_cmd_e_type           nas_cmd,
    /**< Indicate which sms command is this error status for */

  cm_nas_cmd_err_e_type       nas_cmd_err
    /* NAS command error code, indicating whether command
    ** is rejected and the reason */
);


/**===========================================================================

@FUNCTION cm_nas_cmd_req

@DESCRIPTION
  Send a command to NAS.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_nas_cmd_req(

  cm_nas_cmd_cb_f_type                      cmd_cb_func,
    /**< client callback function */

  void                                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                         client_id,
    /**< requesting client ID */

  cm_nas_req_type                           nas_req
    /**< request to send to NAS and parameters */

);


/** @}
*  endgroup Short Messaging Service Object
*/


/****************************************************************************

Position Determination Object
** For internal use (uapdms) only **

****************************************************************************/


/** @defgroup PDO Position Determination Object
** @{
*/


/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/


/** Possible PD events
*/
typedef enum cm_pd_event_e {

  CM_PD_EVENT_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_EVENT_SESSION_START,
    /**< A PD session start event */

  CM_PD_EVENT_SESSION_GRANTED,
    /**< A PD session granted event */

  CM_PD_EVENT_REVA_MSG,
    /**<A PD event with RevA message*/

  CM_PD_EVENT_SESSION_END,
    /**< A PD session end event */

  /* 4 */
  CM_PD_EVENT_FIX_START,
    /**< Blocked HDR data call origination to start GPS search */

  CM_PD_EVENT_FIX_END,
    /**< Unblocked HDR data call origination */

  CM_PD_EVENT_SESSION_DENIED,
    /**< PD Session denied */

  CM_PD_EVENT_SESSION_DORMANT_CHGD,
    /**< The session dormancy changed */

  /* 8 */
  CM_PD_EVENT_MAX
    /**< @internal */


} cm_pd_event_e_type;


/** status related typedefs. Since PD uses DBM, these statuses
*  must map to the DBM statuses.
*/
typedef enum cm_pd_mo_status_e {

  CM_PD_MO_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_MO_OK=CM_DBM_MO_OK,
    /**< PD msg has been completed successfully */

  CM_PD_MO_HOLD_ORIG_RETRY_TIMEOUT=CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT,
    /**< HOLD ORIG retry timeout*/

  CM_PD_MO_HOLD_ORIG=CM_DBM_MO_HOLD_ORIG,
    /**< MC can not process MO MS because HOLD ORIG is TRUE*/

  CM_PD_MO_NO_SRV=CM_DBM_MO_NO_SRV,
    /**< can not send msg  - no service */

  /* 4 */
  CM_PD_MO_ABORT=CM_DBM_MO_ABORT,
    /**< MC or CM aborts the request */

  CM_PD_MO_NOT_ALLOWED_IN_AMPS=CM_DBM_MO_NOT_ALLOWED_IN_AMPS,
    /**< can not send msg in analog mode */

  CM_PD_MO_NOT_ALLOWED_IN_HDR=CM_DBM_MO_NOT_ALLOWED_IN_HDR,
    /**< can not send data burst msg while HDR call is up */

  CM_PD_MO_L2_ACK_FAILURE=CM_DBM_MO_L2_ACK_FAILURE,
    /**< not receiving L2 ack from base station */

  /* 8 */
  CM_PD_MO_OUT_OF_RESOURCES=CM_DBM_MO_OUT_OF_RESOURCES,
    /**< e.g. out of memory buffer */

  CM_PD_MO_ACCESS_TOO_LARGE=CM_DBM_MO_ACCESS_TOO_LARGE,
    /**< message is too large to be sent over access */

  CM_PD_MO_DTC_TOO_LARGE=CM_DBM_MO_DTC_TOO_LARGE,
    /**< message is too large to be sent over DTC */

  CM_PD_MO_ACCT_BLOCK,
    /**< Access Control based on Call Type(service option)  is blocked */

  CM_PD_MO_MAX
  /**< @internal */


} cm_pd_mo_status_e_type;


/** PD session status typedef
** FEATURE_GPSONE_MSBASED
*/
typedef enum cm_pd_session_status_e {

  CM_PD_SESSION_STATUS_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_SESSION_STATUS_END,
    /**< PD session is ended normally*/

  CM_PD_SESSION_STATUS_ABORT_PHONE_BUSY,
    /**< PD session abort because Phone is not in Idle:
    **  +. Incoming call
    **  +. Outgoing call
    **  +. etc.
    */

  CM_PD_SESSION_STATUS_ABORT_ACQ_FAIL,
    /**< PD session abort because Acquisition is failed */

  CM_PD_SESSION_STATUS_ABORT_SYSTEM_LOST,
    /**< PD session abort because system is lost */

  /* 4 */
  CM_PD_SESSION_STATUS_ABORT_GPS_MODE_EXIT,
    /**< PD session abort because of GPS mode exit */

  CM_PD_SESSION_STATUS_DENY_GSM_CALL,
    /**< PD session deny because Phone is in GSM call
    */

  CM_PD_SESSION_STATUS_DENY_HDR_CALL,
    /**< PD session deny because Phone is in HDR call
    */

  CM_PD_SESSION_STATUS_DENY_GSM_ORIG,
    /**< PD session deny because Phone is in GSM call Orig
    */

  /* 8 */
  CM_PD_SESSION_STATUS_GRANTED,
    /**< Session can be granted */

  CM_PD_SESSION_STATUS_HOLD_FORCE_MODE,
    /**< Wait 1 seconds for the force mode to work */

  CM_PD_SESSION_STATUS_DENY_CMD_ON_HOLD,
    /**< There is a pending command */

  CM_PD_SESSION_STATUS_DENY_NO_CDMA_SUBSC,
    /**< PD session deny because Phone doesn't have CDMA subscription */


  CM_PD_SESSION_STATUS_MAX
    /**< @internal */

} cm_pd_session_status_e_type;


/** PD sessions
** FEATURE_GPSONE_MSBASED
*/
typedef enum cm_pd_session_type_e {

  CM_PD_SESSION_NONE=-1,
    /**< @internal */

  /* 0 */
  CM_PD_SESSION_TYPE_WITH_SYS_TIME,

  CM_PD_SESSION_TYPE_WITH_GPS_MODE,

  CM_PD_SESSION_TYPE_WITH_GPS_NORMAL,
    /**< Normal GPS session, typically
    ** this is of short duration
    */

  CM_PD_SESSION_TYPE_WITH_GPS_DMOD,
    /**< GPS DMOD session, this might
    ** Take upto 6 minutes
    */

  /* 4 */
  CM_PD_SESSION_TYPE_SIMULTANEOUS_DMOD_NORMAL,
    /**< Simultaneous DMOD and Normal sessions
    */

  CM_PD_SESSION_MAX
    /**< @internal */

} cm_pd_session_type_e_type;

/** PD session dormant type
*/

typedef enum cm_pd_session_dormant_e {

  CM_PD_SESSION_DORMANT_NONE=-1,
    /**< @internal */

  CM_PD_SESSION_ACTIVE_TO_DORMANT,
    /**< Session going from active to dormant
    */

  CM_PD_SESSION_DORMANT_TO_ACTIVE,
    /**< Session going from Dormant to Active
    */

  CM_PD_SESSION_DORMANT_MAX
    /**< @internal */

}cm_pd_session_dormant_e_type;


typedef enum cm_pd_status_e {

  CM_PD_ERR_NONE=-1,
    /**< @internal */

  CM_PD_OK,
    /**< No error, request was completed successfully */

  CM_PD_ERR_NULL_CHECK_REQ_FUNC,
    /**< Null function provided for checking if a request is time-constraint */

  CM_PD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
    /**< The function is unavailable due to relavent feature being undefined */

  CM_PD_ERR_MAX
    /**< @internal */


} cm_pd_status_e_type;



/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

#define CM_BSSR_MAX_PILOTS          0xF /* 0xF pilots per Rec*/

typedef struct
{
  word    pilot_pn;
  word    base_id;
  word    sid;
  word    nid;
  boolean           base_lat_long_incl;
  sys_base_lat_type
                    base_lat;
  sys_base_long_type
                    base_long;
} cm_pd_pi_rec_type;

/**for now the assumption is that num_records will be 1*/
typedef struct {
    uint16            num_pilots;
    cm_pd_pi_rec_type pi_rec[CM_BSSR_MAX_PILOTS]; /*record per pilot_pn*/
} cm_pd_bss_rsp_info_s_type;


typedef struct cm_pd_info_s {

  cm_pd_session_status_e_type   end_status;
    /**< Session status. FEATURE_GPSONE_MSBASED only */

  cm_pd_session_type_e_type     session_type;
    /**< Session type. FEATURE_GPSONE_MSBASED only */

  boolean                       gps_fix_status;
  /**<---------------------------------
    Following field used for receiving
    Pilot info in BSStatus Request message.
  *------------------------------ */
  cm_pd_bss_rsp_info_s_type pilot_info;
  /**< this contains pilot info received in BSStatusRsp msg*/
    /**< HDR data call origination status. TRUE - blocked; FALSE - Unblocked */

  cm_client_id_type             requesting_client_id;
    /**< Requesting client */

  cm_pd_session_dormant_e_type  dormant_status;
    /**< The dormancy status */

} cm_pd_info_s_type;

/* NV control for SSAC hvolte functionality
*/
typedef PACKED struct PACKED_POST {

  boolean       is_hvolte;                //to control generic hVOLTE feature
  boolean       volte_ssac_sr_control;    // to control SSAC redial
  boolean       volte_acb_sr_control;     //to control ACB redial
  boolean       is_ssac_hvolte;           //to control ssac domain selection report
  uint16        t_ssac;                   // SSAC hysteresis timer
  uint16        ssac_reserved[2];
}cm_ssac_hvolte_s_type;




/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/**===========================================================================

@FUNCTION cm_pd_cmd_session_start

@DESCRIPTION
  Send a start session command to CM. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()
  FEATURE_GPSONE_MSBASED must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_pd_cmd_session_start(

  cm_pd_cmd_cb_f_type         cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< requesting client ID */

  cm_pd_session_type_e_type   session_type

);

/**===========================================================================

@FUNCTION cm_pd_cmd_session_dorm_update

@DESCRIPTION
  Send a session dormancy command to CM. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
boolean cm_pd_cmd_session_dorm_update(

  cm_pd_cmd_cb_f_type          cmd_cb_func,
      /**< client callback function */

  void                        *data_block_ptr,
      /**< pointer to client callback data block */

  cm_client_id_type            client,
      /**< requesting client ID */

  cm_pd_session_type_e_type    session_type,
      /**< type of session */

  cm_pd_session_dormant_e_type session_dormant
      /**< The dormant status of the session */

);

/**===========================================================================

@FUNCTION cm_pd_cmd_session_end

@DESCRIPTION
  Send a end session command to CM. Places a request on the CM queue.

@DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()
  FEATURE_GPSONE_MSBASED must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_pd_cmd_session_end
(
  cm_pd_cmd_cb_f_type       cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< requesting client ID */

  cm_pd_session_type_e_type session_type
    /**< type of session */

);

/**===========================================================================

@FUNCTION cm_pd_reva_cmd_mo_msg

@DESCRIPTION
  Abort mobile originated data burst message if it is pending

@DEPENDENCIES
  None

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern void cm_pd_cmd_reva_mo_msg(

    cm_pd_cmd_cb_f_type                cmd_cb_func,
        /**< client callback function */

    void                               * data_block_ptr,
        /**< pointer to client callback data block */

    cm_client_id_type                  client,
        /**< requesting client ID */

    const word                        *payload,
        /**< pointer to start of message */

    uint16                             num_bytes
        /**< Number of bytes in message */
    );


/** For FEATURE_GPSONE_NEW_L2_ACK only
*/
typedef boolean ( cm_pd_resp_is_within_t2m_f_type ) (

  uint8      *payload,
    /**< msg buffer pointer */

  uint32     numBytes
    /**< msg length */

);



/**===========================================================================

@FUNCTION cm_pd_reg

@DESCRIPTION

@DEPENDENCIES
  FEATURE_GPSONE_NEW_L2_ACK must be defined.

@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern cm_pd_status_e_type cm_pd_reg (

  cm_pd_resp_is_within_t2m_f_type   *cm_pd_resp_is_within_t2m_func
    /**< pointer position determination function  types */

);

/** @}
*  endgroup Position Determination Object
*/

/****************************************************************************

Supplementary Services Object

****************************************************************************/

/** @defgroup SSO Supplementary Services Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/**
** Client should check for CM_API_SUPS_DEFAULT_VALUES before using this macro
*/
#define CM_SS_REF_UNASSIGNED  ((byte) 0x00)
  /* Default value of ss_ref in cm_sups_info_s */

/**
** Client should check for CM_API_SUPS_DEFAULT_VALUES before using this macro
*/
#define CM_SS_CODE_UNASSIGNED  ((byte) 0x00)
  /**< Default value of ss_code in cm_sups_info_s */

/**
** Client should check for CM_API_SUPS_DEFAULT_VALUES before using this macro
*/
#define CM_SS_OPERATION_UNASSIGNED  ((byte) 0x00)
  /**< Default value of ss_operation in cm_sups_info_s
  ** Set equal to NULL_SS_OPERATION in MN
  */

/**
** Client should check for CM_API_SUPS_DEFAULT_VALUES before using this macro
*/
#define CM_SS_INVOKE_ID_UNASSIGNED  ((byte) 0x00)
  /**< Default value of invoke_id in cm_sups_info_s */


#define CM_USS_DATA_MAX CM_USS_PHASE2_TYPE_MAX
  /**< Enum cm_uss_data_e */

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/**===========================================================================

@FUNCTION cm_sups_cmd_register_cc

@DESCRIPTION
  Register SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_register_cc(

  cm_sups_cmd_cb_f_type     sups_cmd_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_reg_ss_params_s_type   *reg_cmd_params,
    /**< pointer to register SS command data */

  const cm_num_s_type             *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                   is_call_control_processing_complete
    /**< Indicates whether call control has already been coompleted */

);




/**===========================================================================

@FUNCTION cm_sups_cmd_erase_cc

@DESCRIPTION
  EraseSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_erase_cc(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                     is_call_control_processing_complete
   /**< Indicates whether call control has already been completed */

);




/**===========================================================================

@FUNCTION cm_sups_cmd_activate_cc

@DESCRIPTION
  EraseSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_activate_cc(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< originating client */

  const cm_act_ss_params_s_type  *act_cmd_params,
    /**< pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  boolean                  is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);




/**===========================================================================

@FUNCTION cm_sups_cmd_deactivate_cc

@DESCRIPTION
  DeactivateSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_deactivate_cc(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /**< client callback function */

  void                        *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< originating client */

  const cm_deact_ss_params_s_type   *deact_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                     is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_interrogate_cc

@DESCRIPTION
  InterrogateSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_interrogate_cc(

  cm_sups_cmd_cb_f_type        sups_cmd_cb_func,
    /**< client callback function */

  void                         *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type            client_id,
    /**< originating client */

  const cm_interr_ss_params_s_type   *interr_cmd_params,
    /**< pointer to interrogate SS command data */

  const cm_num_s_type                *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                      is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);



/**===========================================================================

@FUNCTION cm_sups_cmd_process_uss_cc

@DESCRIPTION
  Process Unstructured SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_process_uss_cc(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< originating client */

  const cm_uss_params_s_type     *process_uss_cmd_params,
    /**< pointer to Process Unstructured SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                  is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);


/**===========================================================================

@FUNCTION cm_sups_cmd_reg_password_cc

@DESCRIPTION
  Register Password SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_reg_password_cc(

  cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
    /**< client callback function */

  void                       *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type          client_id,
    /**< originating client */

  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params,
    /**< pointer to register password SS command data */

  const cm_num_s_type              *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                    is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);


/**===========================================================================

@FUNCTION cm_sups_cmd_release_cc

@DESCRIPTION
  Release SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_sups_cmd_release_cc(

  cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id,
    /**< originating client */

  const cm_release_ss_params_s_type    *release_cmd_params,
    /**< pointer to release SS command data */

  const cm_num_s_type                  *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                        is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

);


/** @}
*  endgroup Supplementary Services Object
*/


/****************************************************************************

BCMCS Object

This object implements CM part of "High Rate BroadCast-MultiCast packet
data air interface Specification C.S0054" and "BCMCS Framework Draft Document
0.1.3 XP0019"

****************************************************************************/

/** @defgroup BO BCMCS Object
** @{
*/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/





/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

typedef struct {

  sys_bcmcs_bom_caching_e_type    enable_strict_caching;
      /* enable / disable strict BOM caching */

  uint8                           stale_BOM_factor;
      /* multiplier used to compute the staleness of BOM entry based
         on the BOP.Only valid if strict caching is enabled. */
}cm_bcmcs_cmd_info_bom_cache_s_type;

typedef struct {

  uint8                           num_of_flows;
      /* number of entries in flow_addr_array[] */

  sys_bcmcs_flow_addr_s_type      flow_addr_array[ SYS_BCMCS_MAX_FLOWS ];
      /* array of flow addresses */

} cm_bcmcs_cmd_info_reg_s_type;

typedef struct {

  cm_bcmcs_cmd_err_e_type         cmd_err;
      /* error in command request */

  uint8                           num_of_flows;
      /* number of entries in flow_addr_array[] */

  sys_bcmcs_flow_addr_s_type      flow_addr_array[ SYS_BCMCS_MAX_FLOWS ];
      /* array of flow addresses */

  sys_bcmcs_reg_setup_e_type      flow_reg_setup_array[ SYS_BCMCS_MAX_FLOWS ];

} cm_bcmcs_cmd_info_flow_s_type;

typedef union
{
  cm_bcmcs_cmd_info_reg_s_type  reg_req;

  cm_bcmcs_cmd_info_flow_s_type flow_req;

  cm_bcmcs_cmd_info_bom_cache_s_type bom_cache_setup;
}cm_bcmcs_cmd_info_u_type;


/*---------------------------------------------------------------------------
** Macros
**-------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
** Functions
**-------------------------------------------------------------------------*/

/* CM command functions
**
** The CM command functions defines the Client->CM interface.
**
** Each command function takes a pointer to a client provided callback
** function, a client provided void pointer and the command's required set
** of fields.
**
** If the command function find one or more of the parameters to contain
** invalid values ( e.g. not one of the enum values for an enum parameter),
** it calls on the client's callback function IMMEDIATELY with the
** appropriate command error code. NOTE that in this case the callback
** function is actually being called from the client context - NOT CM.
**
** If all parameters found to be valid, the command function queues a
** command to CM. When CM gets the command it checks whether the command
** is valid relative to the current state of the system.
**
** In case the command is not valid, CM calls on the client callback
** function with the appropriate command error code. NOTE that in this
** case the callback function is indeed being called from the CM context.
**
** If the command is successful, CM calls on the client callback function
** with an error code that indicates successful command. In addition CM
** notifies registered clients of the corresponding CM event.
*/

boolean cm_bcmcs_cmd_bom_cache_setup(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< originating client */

  uint8                           stale_BOM_factor,
    /**< multiplier for staleness of BOM entry */

  sys_bcmcs_bom_caching_e_type    enable_strict_caching
    /**< enable / disable strict BOM caching */
);


/****************************************************************************

MBMS Object

This object implements CM part of "Multicast Broadcast Multimedia Services"


****************************************************************************/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/* Type to hold MBMS information.
** When notified of MBMS events, clients are given
** a reference to such a structure.
** Clients should check for CM_API_MBMS before using this struct
*/



/**--------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------*/

/* CM command functions
**
** The CM command functions defines the Client->CM interface.
**
** Each command function takes a pointer to a client provided callback
** function, a client provided void pointer and the command's required set
** of fields.
**
** If the command function find one or more of the parameters to contain
** invalid values ( e.g. not one of the enum values for an enum parameter),
** it calls on the client's callback function IMMEDIATELY with the
** appropriate command error code. NOTE that in this case the callback
** function is actually being called from the client context - NOT CM.
**
** If all parameters found to be valid, the command function queues a
** command to CM. When CM gets the command it checks whether the command
** is valid relative to the current state of the system.
**
** In case the command is not valid, CM calls on the client callback
** function with the appropriate command error code. NOTE that in this
** case the callback function is indeed being called from the CM context.
**
** If the command is successful, CM calls on the client callback function
** with an error code that indicates successful command. In addition CM
** notifies registered clients of the corresponding CM event.
*/

/****************************************************************************

Client Object

****************************************************************************/

/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/




/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/* Types for CM event callback functions.
**
** A client that want to be notified of CM events needs to register
** with the appropriate type of callback function with CM.
**
** CM calls on the client's callback functions when events to which the
** client is registered for occur. Each client is responsible for the
** implementation of its callback functions. Note that since callback
** functions are being called from CM context they are expected to
** only queue commands to their corresponding client and return. No extra
** processing is allowed and in particular no rex_wait() and alike.
**
** NOTE! callback functions are not allowed to change the content
** of the info structure which they are given a pointer.
**
** NOTE! clients are not allowed to access the info structure after
** the their callback functions return.
*/

/** Type for NAS event callback function.
**
** A client that want to be notified of NAS events needs to
** register with such a function with CM.
*/
typedef void (cm_nas_event_f_type) (

  cm_nas_event_e_type           nas_event,
    /**< Indicate nas event */

  const cm_nas_info_s_type      *nas_info_ptr
    /**< A pointer to a nas information structure */

);



/** Type for PD event callback function.
**
** A client that want to be notified of PD events needs to
** register with such a function with CM.
*/
typedef void (cm_pd_event_f_type) (

  cm_pd_event_e_type           pd_event,
    /**< Indicate pd event */

  const cm_pd_info_s_type      *pd_info_ptr
    /**< A pointer to a pd information structure */

);


/*/8 Type for PD command error callback function.
**
** A client that want to be notified of OTHER clients pd command errors
** needs to register with such a function with CM.
*/
typedef void (cm_pd_cmd_err_f_type) (

  cm_client_id_type          client_id,
    /**< Indicate which client caused the error */

  cm_pd_cmd_e_type           cmd,
    /**< Indicate for which sms command this error is reported */

  cm_pd_cmd_err_e_type       cmd_err
    /**< Indicate nature of error */

);


typedef void (cm_nas_cmd_err_f_type) (

  cm_client_id_type            client_id,
    /**< Indicate which client caused the error */

  cm_nas_cmd_e_type           cmd,
    /**< Indicate for which sups command this error is reported */

  cm_nas_cmd_err_e_type       cmd_err
    /**< Indicate nature of error */

);


/**===========================================================================

@FUNCTION cm_client_pd_reg

@DESCRIPTION
  This function is to be used by CM clients interested in registering for
  Position Determination call manager events.

@DEPENDENCIES
  cm_client_init must have been called previously.

@RETURNS
  results of the registration.

@SIDE_EFFECT
  None

@x
===========================================================================*/

cm_client_status_e_type cm_client_pd_reg(

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_pd_event_f_type             *pd_event_func,
    /**< Pointer to a callback function to notify the client of sms
    ** events */

  cm_client_event_reg_e_type      event_reg_type,
    /**< Is this a registration or de-reg */

  cm_pd_event_e_type              from_pd_event,
    /**< register from this event (including) */

  cm_pd_event_e_type              to_pd_event,
    /**< to this event (including) */


  cm_pd_cmd_err_f_type           *pd_cmd_err_func
    /**< pointer to a callback function to notify the client of sms
    ** command errors of OTHER clients */

);


/**===========================================================================

@FUNCTION cm_mm_client_pd_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_pd_event and to_pd_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of pd events, a client needs to call on this API twice- once for each
  range of ph events. In case of conflicting pwr_collapse_type, the last
  one will be honored

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_pd_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_pd_event_f_type           *pd_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Is this a registration or de-reg */

    cm_pd_event_e_type            from_pd_event,
        /**< register from this event (including) */

    cm_pd_event_e_type            to_pd_event
          /* to this event (including) */
);


/*===========================================================================
@FUNCTION cm_client_nas_reg

@DESCRIPTION

  This function allows a client to register the following NAS
  callbacks and events with CM:

   1. Callback function to receive registered NAS events
   2. Register for and de-register from receiving NAS events
   3. Callback function to receive other clients' NAS command errors.

   If a client registers a callback function to receive NAS event
   notification, it also needs to register one or more NAS events
   it is interested in receiving. This API has a cumulative effect for event
   registration. For example, in order to register for two different ranges
   of NAS events, a client needs to call on this API twice -
   once for each range of BCMCS events.

   If a client registers a callback function to receive other clients'
   NAS command errors, it will receive notifications on all possible
   NAS command errors caused by other clients. A client cannot
   register to receive only a subset of NAS command errors.
   The NULL function pointer can also be used for the callback function
   if a client is not interested in receiving that callback.
   For example, if a client is not interested in
   receiving other clients' NAS command errors, it can set
   nas_cmd_err_func = NULL. However, if the nas_event_func is set to NULL,
   all registered NAS events are also de-registered and the
   event_reg_type, from_nas_event, and to_nas_event parameters are ignored.

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().

@RETURNS
  Request status

@SIDE_EFFECT
  None

@x
===========================================================================*/
cm_client_status_e_type cm_client_nas_reg(

    cm_client_id_type             client,
        /**< client id
        */

    cm_nas_event_f_type          *nas_event_func,
        /**< Pointer to a callback function to notify the client
        ** of NAS events
        */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Registering or deregistering?
        */

    cm_nas_event_e_type           from_nas_event,
        /**< register from this event (including)
        */

    cm_nas_event_e_type           to_nas_event,
          /**< to this event (including)
          */

    cm_nas_cmd_err_f_type        *nas_cmd_err_func
      /**< pointer to a callback function to notify the client of NAS
      ** command errors of OTHER clients
      */
);



/**===========================================================================

@FUNCTION cm_mm_client_nas_pwr_collapse_reg

@DESCRIPTION
  This function allows a client to register/de-register the events it should
  be notified during power collapse. If the client wants to be notified of
  all the events that it is registered for in the normal (power restored
  state), it should use CM_PWR_COLLAPSE_NOTIFY_NORMAL as the
  pwr_collapse_type. If the client wants CM to decide on "important events"
  to wake up the processor from the power collapse state, it should use
  CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT. If the client wants to override
  CM's default, it should use CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM.

  The from_nas_event and to_nas_event, and event_reg_type are used only
  if the pwr_collapse_type is CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_CUSTOM. If
  event_reg_type is CM_CLIENT_EVENT_REG, the event will be notified while in
  power collapse. If the event_reg_type is CM_CLIENT_EVENT_DEREG, the event
  will not be reported while in power collapse.

  This API has a cumulative effect for event registration.
  For example, in order to regieter/de-register for two different ranges
  of nas events, a client needs to call on this API twice- once for each
  range of nas events. In case of conflicting pwr_collapse_type, the last
  one will be honored

@DEPENDENCIES
  Client must have already been initialized with cm_client_init().


@RETURNS
  request status

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_client_status_e_type cm_mm_client_nas_pwr_collapse_reg(

    cm_client_id_type             client,
        /**< client id */

    cm_nas_event_f_type          *nas_event_func,
        /**< Pointer to a callback function to notify the client of call
        ** events */

    cm_processor_e_type           proc_id,
        /**< The processor on which the client is executing */

    cm_pwr_collapse_notify_e_type pwr_collapse_type,
        /**< The power collapse type, None, Full, default, custom */

    cm_client_event_reg_e_type    event_reg_type,
        /**< Is this a registration or de-reg */

    cm_nas_event_e_type           from_nas_event,
        /**< register from this event (including) */

    cm_nas_event_e_type           to_nas_event
          /* to this event (including) */
);




/**===========================================================================

@FUNCTION cmclient_get_client_type

@DESCRIPTION
  Returns client id that corresponds to a client object

@DEPENDENCIES
  None

@RETURNS
  client id

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern cm_client_type_e_type cmclient_get_client_type(

    cm_client_id_type client_id
    /**< Client object pointer */
);


/** @}
*  endgroup
*/

/****************************************************************************

Utility Functions

****************************************************************************/


/** @defgroup UF Utility Functions
** @{
*/


/*---------------------------------------------------------------------------
** Defines
**---------------------------------------------------------------------------
*/

#define CM_EXTDISP_MAX_SUBREC_LEN  20
  /* from ANSI T1.610 Annex D */


/*---------------------------------------------------------------------------
** Enums
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Datatypes
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Macros
**---------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------
** Functions
**---------------------------------------------------------------------------
*/

/*                 MISC CONVERSION FUNCTIONS
*/

/**===========================================================================

@FUNCTION: cm_util_def_alphabet_to_ascii

@DESCRIPTION
  USE cm_util_gsm_alphabet_to_ascii INSTEAD. This function has bugs and
  exists for backward compatibility only.

  Convert the default alphabet to ASCII.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_def_alphabet_to_ascii(

    const byte          *default_alphabet_string,

    byte          num_chars,

    byte          *ascii_string
);

/*===========================================================================

@FUNCTION: cm_util_gsm_alphabet_to_ascii_internal

@DESCRIPTION
  THIS FUNCTION IS DEPRECATED - USE cm_util_gsm7_to_iso8859() INSTEAD

  Unpacks bytes of data into 7-bit GSM default alphabet string and then
  converts it to an ASCII string.

@DEPENDENCIES
  None.

@RETURNS
  Number of characters written into the output buffer.

@SIDE_EFFECT
  None

===========================================================================*/
extern byte cm_util_gsm_alphabet_to_ascii_internal(
    byte        *ascii_string,
    size_t       ascii_string_len,
    const byte  *gsm_alphabet_string,
    byte        num_bytes
);



/**===========================================================================

@FUNCTION: cm_util_number_to_bcd

@DESCRIPTION
  Convert the phone number from ASCII to BCD

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_number_to_bcd(
    const cm_num_s_type *number,

    byte                *bcd_number
);


/**===========================================================================

@FUNCTION: cm_util_ascii_num_to_bcd_num

@DESCRIPTION
  Convert the phone number from ASCII to BCD

@DEPENDENCIES
  None.

@RETURNS
  TRUE - If string is succesfully converted
  FALSE - Otherwise

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern boolean cm_util_ascii_num_to_bcd_num(
    const cm_num_s_type   *number,

    byte                  *bcd_number,

    size_t                bcd_size
);


/**===========================================================================

@FUNCTION: cm_util_bcd_to_ascii

@DESCRIPTION
  Convert the phone number from BCD to ASCII.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_bcd_to_ascii(
    const byte    *bcd_number,

    byte          *ascii_number
);

/**===========================================================================

@FUNCTION: cm_util_bcd_num_to_ascii_num

@DESCRIPTION
   Converts a phone number from BCD to ASCII.

   This function ensures that the destination buffer will always
   be NULL terminated and that there will not be a copy beyond
   the size of the destination buffer.

   @param[in] bcd_number    BCD number to convert.
   @param[in] ascii_number  Resulting ASCII number.
   @param[in] ascii_size    Size of the ASCII number destination buffer

@DEPENDENCIES
  None.

@RETURNS
  Length of unpacked BCD string

@SIDE_EFFECT
  None

@x

===========================================================================*/

extern size_t cm_util_bcd_num_to_ascii_num(

    const byte    *bcd_number,
    byte          *ascii_number,
    size_t        ascii_size

);

/**===========================================================================

@FUNCTION: cm_util_ascii_to_def_alphabet

@DESCRIPTION
  USE cm_util_ascii_to_gsm_alphabet INSTEAD. This function has bugs and
  exists for backward compatibility only.

  Convert the ASCII to default alphabet.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_ascii_to_def_alphabet(
    byte          *default_alphabet_string,

    byte          num_chars,

    const byte   *ascii_string
);


/**===========================================================================

@FUNCTION: cm_util_pack

@DESCRIPTION
  USE cm_util_ussd_pack INSTEAD. This function has bugs and exists for
  backward compatibility only.

  Pack the bits in a byte.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_pack(
  byte         *coded_data,

  byte         num_chars,

  const byte  *string_to_convert
);


/**===========================================================================

@FUNCTION: cm_util_unpack

@DESCRIPTION
  USE cm_util_ussd_unpack INSTEAD. This function has bugs and exists for
  backward compatibility only.

  Unpack the bits into bytes.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern void cm_util_unpack(const byte *user_data, byte num_chars, byte *message_string);


/**===========================================================================

@FUNCTION: cm_util_ussd_pack

@DESCRIPTION
  Pack 7-bit GSM characters into bytes (8-bits)

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern byte cm_util_ussd_pack(

    byte *packed_data,

    const byte *str,

    byte num_chars
);

/**===========================================================================

@FUNCTION: cm_util_ussd_unpack

@DESCRIPTION
  Unpack the bytes (8-bit) into 7-bit GSM characters

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern byte cm_util_ussd_unpack(

    byte *str,

    const byte *packed_data,

    byte num_bytes
);

/**===========================================================================

@FUNCTION: cm_util_ascii_to_gsm_alphabet

@DESCRIPTION
  Convert the ASCII string to GSM default alphabets string and packs it
  into bytes.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern byte cm_util_ascii_to_gsm_alphabet(

    byte          *gsm_alphabet_string,

    const byte          *ascii_string,

    byte          num_chars
);

/**===========================================================================

@FUNCTION: cm_util_gsm_alphabet_to_ascii

@DESCRIPTION
  Unpacks bytes of data into 7-bit GSM default alphabet string and then
  converts it to an ASCII string.

@DEPENDENCIES
  None.

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern byte cm_util_gsm_alphabet_to_ascii(

    byte    *ascii_string,

    const byte    *gsm_alphabet_string,

    byte    num_bytes
);



/**                M51 MODE SETTING TYPES & FUNCTIONS
** This block group all the functionality that is associated with
** M51 UI / DS interfacing.
**
** It is not desired that clients communicate with one another.
** Therefore CM provides API interface to both UI and DS to allow the user
** to change M51 settings.
**
** When the user selects M51 mode from UI menu, UI calls the following
** functions in CM:
** boolean cm_change_m511_mode(sys_jcdma_m511_type new_mode);
** boolean cm_change_m512_mode(sys_jcdma_m512_type new_mode);
** boolean cm_change_m513_mode(sys_jcdma_m513_type new_mode);
**
** In turn, CM looks at the M51 mode object and calls appropriate handler
** function that had been registered with it by DS.
**
** Note that CM expects DS to register M51 mode setting functions
** with CM during initialization time and those are expected to be
** synchronous functions.
**
** Until such callback functions are registered with CM, CM uses a default
** handlers that return FALSE to all M51 mode change requests.
**
*/





















/**===========================================================================

@FUNCTION cm_get_cur_nam

@DESCRIPTION
  Returns the current NAM.

@DEPENDENCIES
  Phone object must be initialized.

@RETURNS
  The current NAM. If phone was not yet initialized, return CM_NAM_NONE.

@SIDE_EFFECT
  None

@x
===========================================================================*/
extern cm_nam_e_type  cm_get_curr_nam(

  void

);









/**===========================================================================

@FUNCTION cm_num_get

@DESCRIPTION
  Copy a CM number into a specified byte buffer.

@DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

@RETURNS
  The numbers digits actually copied.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern uint8 cm_num_get(

  const cm_num_s_type    *cm_num_ptr,
    /**< Pointer to a CM number to copy from */

  uint8*                  num_buf,
    /**< Pointer to a byte buffer to copy CM number into */

  uint8                   max_len
    /**< Size of byte buffer */

);



/**===========================================================================

@FUNCTION cm_num_cmp

@DESCRIPTION
  Compare a CM number byte with a byte buffer number over a specified
  length.

@DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

@RETURNS
  TRUE if both numbers match over the specified compare length,
  FALSE otherwise.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_num_cmp(

  const cm_num_s_type    *cm_num_ptr,
    /**< Pointer to a CM number */

  const uint8*           num_buf,
    /**< Pointer to a byte buffer number */

  unsigned int           cmp_len
    /**< Length to perform comparison over */

);



/**===========================================================================

@FUNCTION cm_num_cmp_exact

@DESCRIPTION
  Compare a CM number byte with a byte buffer for an exact match -
  i.e. length and pattern.

@DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

@RETURNS
  TRUE if both numbers match exactly, FALSE otherwise.

@SIDE_EFFECT
  none

@x
===========================================================================*/
extern boolean cm_num_cmp_exact(

  const cm_num_s_type    *cm_num_ptr,
    /**< Pointer to a CM number */

  const uint8*                  num_buf,
    /**< Pointer to a byte buffer number */

  unsigned int           num_len
    /**< Length off byte buffer number */

);



/**===========================================================================

@FUNCTION cm_num_copy

@DESCRIPTION
  Copy a byte buffer number into a specified CM number.

@DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
void cm_num_copy(

    cm_num_s_type    *cm_num_ptr,
        /**< Pointer to a CM number */

    const byte*      num_buf,
        /**< Pointer to a number to be copied into CM number */

    uint8            num_len
        /**< Length of number to be copied */
);



/**===========================================================================

@FUNCTION cm_num_ep_msg_copy

@DESCRIPTION
  Copy an IS91 Extended Protocol Short Message into CM number.

@DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
void cm_num_ep_msg_copy(

    cm_num_s_type    *cm_num_ptr,
        /**< Pointer to a CM number */

    const byte*      num_buf,
        /**< Pointer to a number to be copied into CM number */

    uint8            num_len
        /**< Length of number to be copied */
);



/**===========================================================================

@FUNCTION cm_num_ep_cli_copy

@DESCRIPTION
  Copy IS91 Extended Protocol Caller Line ID into a CM number.

@DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
void cm_num_ep_cli_copy(

    cm_num_s_type    *cm_num_ptr,
        /**< Pointer to a CM number */

    const byte*      num_buf,
        /**< Pointer to a number to be copied into CM number */

    uint8            num_len
        /**< Length of number to be copied */
);




/**===========================================================================

@FUNCTION cm_num_attrib_copy

@DESCRIPTION
  Set the attributes of a CM number to a specified set of attributes.

@DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

@RETURNS
  none

@SIDE_EFFECT
  none

@x
===========================================================================*/
void cm_num_attrib_copy(

    cm_num_s_type           *num_ptr,
        /**< Pointer to a CM number */

    uint8                   number_type,
        /**< Number type */

    uint8                   number_plan,
        /**< Numbering Plan */

    uint8                   pi,
        /**< Presentation Indicator */

    uint8                   si,
        /**< Screening Indicator */

    cm_digit_mode_e_type    digit_mode
        /**< Origination digit mode */
);



/**===========================================================================

@FUNCTION cm_num_validate

@DESCRIPTION
  This function validates a number.

@DEPENDENCIES
  The number must have already been initialized with cm_num_init().

@RETURNS
  CM_NUM_ERR_NOERR is returned if the number is valid.  Otherwise, an
  appropriate error code is returned.

@SIDE_EFFECT
  none

@x
===========================================================================*/
cm_num_err_e_type cm_num_validate(

    const cm_num_s_type    *num_ptr
        /**< Pointer to a CM number to be asserted */
);



/**=========================================================================*/

#define cm_flash_type_e_type     cm_call_sups_type_e_type
#define CM_FLASH_TYPE_EMERGENCY  CM_CALL_SUPS_TYPE_EMERGENCY_CALL
#define CM_CALL_TYPE_DATA        CM_CALL_TYPE_PS_DATA
#define CM_CALL_TYPE_PKT_DATA    CM_CALL_TYPE_PS_DATA

/**=========================================================================*/





/**===========================================================================

@FUNCTION cm_is_sys_mode_allowed

@DESCRIPTION

  The function determines if the given sys_mode is included / conforms to
  given band and mode pref.

  For example, if the system mode is  SYS_SYS_MODE_GSM and  mode band
  preference is CM_MODE_PREF_GSM_WCDMA_ONLY / CM_BAND_PREF_GSM_850  the
  function returns TRUE but given SYS_SYS_MODE_GW it returns FALSE because
  CM_BAND_PREF_GSM_850 does not have WCDMA component.

@DEPENDENCIES
  None

@RETURNS
  TRUE  - If Valid
  FALSE - Otherwise

@SIDE_EFFECT
  none

@x
===========================================================================*/
boolean cm_is_sys_mode_allowed(

    cm_mode_pref_e_type   mode_pref,
      /**< The mode preference requested
      */

    cm_band_pref_e_type   band_pref,
      /**< The band preference requested
      */

    sys_sys_mode_e_type   sys_mode
      /**< The sys mode requested
      */
  );

/*===========================================================================

FUNCTION cm_otasp_update_mode

DESCRIPTION
  Check the mode in NV and update the mode

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

  CM will not write this mode to NV.  It is expected this value has already
  been written to NV.

RETURN VALUE
  none

SIDE EFFECTS
  Phone mode may be changed to reflect the NV item

===========================================================================*/
void cm_otasp_update_mode(
   cm_nam_e_type        nam,
   nv_mode_enum_type    nv_mode
);


/*========================================================================

FUNCTION CM_UNLOCK_HDR_RESOURCES

DESCRIPTION

  This function unconditionally sends DO an advise unlock notification

Limitation:

  The API is restricted to be used by 1XCP module when
  FEATURE_FULL_TIME_SHDR_3 & FEATURE_HDR_HYBRID are defined

  It is not recommended to be used by any other clients.

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
extern void cm_unlock_hdr_resources(void);

/*===========================================================================

FUNCTION cm_get_services_count

  Gives number of simultaneous services that MMODE can provide. While this is
  the maximum number of services that MMODE may support, the actual number
  present may support be less than this.

DEPENDENCIES
  none

RETURN VALUE
  Max. services that MMODE can provide at any instant.

SIDE EFFECTS
  none

===========================================================================*/
int cm_get_services_count( void );

/*===========================================================================

FUNCTION cm_get_services_status

DESCRIPTION
  Gives service status for simultaneous services that MMODE can provide.
  Client using this API need to get max simultaneous count from
  cm_get_services_count(). Then it should pass pointer to an array of
  type cm_service_status_s_type with size equal to max simultaneous services.

  Index 0 = service on MAIN stack - CDMA/GW/GWL
  Index 1 = service on HYBR_1 stack - HDR
  Index 2 = service on HYBR_2 stack - GWL

DEPENDENCIES
  cm_get_services_count should be called before this API.

RETURN VALUE
  TRUE - Service status for all services supported by MMODE.
  FALSE - If not able to retrieve service status.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_get_services_status(

    cm_service_status_s_type *srv_list,
      /* Array to hold service status for each technology supported */

    int srv_count
      /* Size of the array srv_list, equals to MAX simultaneous tech. */
);

/*===========================================================================

FUNCTION cm_data_packet_state_notify

DESCRIPTION
  Function for DS to notify CM data traffic status. The bitmask input is 
  based on SYS_MODEM_AS_ID_MASK_* Macro defined in sys.h

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cm_data_packet_state_notify(

   uint8          inactive_subs_id_bitmask
);

/*===========================================================================

FUNCTION cm_data_oprt_mode_reg

DESCRIPTION
  registration function for DS to register callback. When PWROFF/LPM received, CM will call DS callback
  To notify DS

DEPENDENCIES
  none

RETURN VALUE
  results of the registration

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_data_oprt_mode_reg(cm_ds_sys_pwroff_lpm_notif_f_type * ds_ntfy_cb_func);

/*===========================================================================

FUNCTION cm_data_subsc_cmd_reg_ext

DESCRIPTION
  registration function for DS to register callback. When PWROFF/LPM received, CM will call DS callback
  To notify DS

DEPENDENCIES
  none

RETURN VALUE
  results of the registration

SIDE EFFECTS
  none

===========================================================================*/

boolean cm_data_subsc_cmd_reg_ext(

cm_ds_cmd_notif_ext_f_type  * ds_ntfy_cb_func_ext

    /*  pointer to DS callback function */
);


/*===========================================================================

FUNCTION cm_get_hdr_intl_srv_status

DESCRIPTION
  Returns the internal servcie status of HDR mode

DEPENDENCIES
  none

RETURN VALUE
  results of the registration

SIDE EFFECTS
  none

===========================================================================*/
sys_srv_status_e_type cm_get_hdr_intl_srv_status (void);




/*===========================================================================

FUNCTION cmsds_deactivate_1xcsfb_protocol

DESCRIPTION
  This function handles the deregistation of 1XCSFB

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void  cmsds_deactivate_1xcsfb_protocol
(
  prot_trans_type     mmoc_tans_id,

  prot_deact_e_type   deact_reason
);

/*===========================================================================

FUNCTION cmsds_is_1xcsfb_activated

DESCRIPTION
  This function returns whether 1x protocol is in 1xcsfb mode or not.

DEPENDENCIES
  none

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmsds_is_1xcsfb_activated
(
  void
);



/*===========================================================================

FUNCTION NAME cmmmgsdi_read_cdma_imsi

DESCRIPTION

  Function that reads the IMSI from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
extern boolean cmmmgsdi_read_cdma_imsi
(
  void
);
/*===========================================================================

FUNCTION NAME cmmmgsdi_get_imsi_mcc

DESCRIPTION

  Function that reads the sends the mcc value read from the SIM

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
extern word cmmmgsdi_get_imsi_mcc
(
  void
);
/*===========================================================================

FUNCTION NAME cmmmgsdi_get_imsi_mnc

DESCRIPTION

  Function that reads the sends the mnc value read from the SIM

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
extern byte cmmmgsdi_get_imsi_mnc
(
  void
);

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

@FUNCTION cmsds_activate_hdr_tunnel_req

@DESCRIPTION
  Send activate tunnel request to HDR.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
extern void cmsds_activate_hdr_tunnel_req (void);

/*===========================================================================

@FUNCTION cmsds_deactivate_hdr_tunnel_req

@DESCRIPTION
  Send deactivate tunnel request to HDR.

@DEPENDENCIES

None

@RETURNS
  None

@SIDE_EFFECT

None

===========================================================================*/
extern void cmsds_deactivate_hdr_tunnel_req (void);
#endif

extern boolean cm_is_plmn_rat_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat
         /* Mode of the PLMN under query
         */
);

/*===========================================================================

FUNCTION cmcall_is_there_volte_call

DESCRIPTION
  Checks is there is a  requested VOLTE call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_id_type cmcall_is_there_volte_call
(  
   cm_call_type_e_type         call_type
        /* Call type to search */
);

/*===========================================================================

FUNCTION cmcall_is_there_vowlan_call

DESCRIPTION
  Checks is there is a  requested VOWLAN call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_vowlan_call();


/*===========================================================================

@FUNCTION cm_per_subs_is_plmn_rat_allowed

@DESCRIPTION
  Check if the plmn is allowed for registration.  For now, only as_id_1 is valid input
  parameter since svlte/mmss is not supported on as_id_2.

@DEPENDENCIES

None

@RETURNS
  TRUE if allowed.
  FALSE otherwise.

@SIDE_EFFECT

None

===========================================================================*/
extern boolean  cm_per_subs_is_plmn_rat_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat,
         /* Mode of the PLMN under query
         */
        sys_modem_as_id_e_type         as_id,
        /* subscription id
        */
       sys_modem_stack_id_e_type         stack_id
);

/*===========================================================================

@FUNCTION cm_per_subs_is_reg_allowed

@DESCRIPTION
  Check if the plmn is allowed for registration.  For now, only as_id_1 is valid input
  parameter.

@DEPENDENCIES

None

@RETURNS
  TRUE if allowed.
  FALSE otherwise.

@SIDE_EFFECT

None

===========================================================================*/
extern boolean  cm_per_subs_is_reg_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat,
         /* Mode of the PLMN under query
         */
        sys_modem_as_id_e_type         as_id,
        /* subscription id
        */
       sys_modem_stack_id_e_type         stack_id
);


/*===========================================================================

FUNCTION cmregprx_check_lte_priority_scan

DESCRIPTION
  Check if CS to PS redial is enabled
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmregprx_check_lte_priority_scan(sd_ss_e_type ss);

/*===========================================================================

FUNCTION cmregprx_check_lte_priority_scan_voice

DESCRIPTION
 This function checks if LTE priority scan is
 required when the CS to PS special VOLTE redial is enabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern boolean  cmregprx_check_lte_priority_scan_voice
(
  sd_ss_e_type                ss
);

/*===========================================================================

FUNCTION cmsds_is_gwt_scan_allowed

DESCRIPTION
  This function returns whether GWT scan allowed during RLF based 
  on VOLTE call and NV enabled

DEPENDENCIES
  none

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmsds_is_gwt_scan_allowed
(
  sd_ss_e_type ss
);

/*===========================================================================

FUNCTION cmrpm_lte_update_sim_state

DESCRIPTION
  This function will check whether LTE RPM is enabled to determine which
  SIM state to use based on emm_gen_counter and last_emm_reject_cause.

DEPENDENCIES

RETURN VALUE
  The updated SIM state for SD to use.

SIDE EFFECTS
  none

===========================================================================*/
extern sys_sim_state_e_type cmrpm_lte_update_sim_state( void );

/*===========================================================================

FUNCTION cmrpm_lte_get_srv_domain_pref

DESCRIPTION
  This function will check whether LTE RPM is enabled to determine which
  srv_domain_pref to use based on emm_gen_counter and last_emm_reject_cause.

DEPENDENCIES

RETURN VALUE
  The updated srv_domain_pref for SD to use.

SIDE EFFECTS
  none

===========================================================================*/
extern sd_ss_srv_domain_pref_e_type cmrpm_lte_get_srv_domain_pref( void );


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*CM_V_H*/
