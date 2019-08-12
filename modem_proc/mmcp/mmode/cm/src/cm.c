
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E

GENERAL DESCRIPTION
  This module makes up the Call Manager of the subscriber station software.

  The Call Manager provides out of bound call control and event notification
  services to its clients.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1998 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/14   jvo     Fix compilation warning in cm_nc_call_control_processing()
11/07/13   jvo     Verify the length of alpha_buf doesn't overflow in
                   cm_mm_call_cmd_orig_internal()
10/02/13   kc       handle CM_MODE_PREF_NO_CHANGE in CM API07/25/13
09/04/13   mh/jvo  Mainline the modem statistics code
08/07/13   skk     FR3663:Support to limit the bands searched during a manual search
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/15/13   ss      FR1860 - T/G on DSDS
06/25/13   dk      CR470025 stack reduction effort
06/24/13   xs      VT answered as voice
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
05/24/13   ss      Sups LCS_MOLR memory fix
05/02/13   ss      New API for converting ASCII to BCD
04/11/13   xs      FRAT support
01/22/13   xs      Added video share support
01/07/13   qf      FR 2564 - World mode support for DSDA
01/07/12   xs      Added PS supplemetary service support
11/18/12   aj      Pass emerg bearer info from DS to NAS
10/26/12   mg      Set ctm bit from thin UI if auto answer is enabled.
10/24/12   vm      Old cc api use new centralized call control
09/26/12   skk     Changes of ESM SM backoff timer requirements
08/08/12   ak      Update call type from IS-707B to PS_DATA before queuing
                   the call command.
07/20/12   fj      Added support for sending NAS request to set IRAT timer for
                   TDS to LTE HPLMN search.
03/01/12   fj      Added support for DRX.
05/01/12   gm      Cleanup of API cm_get_current_service_status
04/18/12   gm      Simplified API to retrieve SS info for all tech.
03/29/12   gm      Deprecate/Clean cm_get_pref_srv_type_for_packet_orig
03/26/12   gm      Cleanup deprecated APIs/Data-structures
03/09/12   fj      Initialize the hybrid_gw_xx values for SVLTE mode.
03/02/12   vk      Added feature to get DCH cell info.
02/25/12   jqi     Free MO SMS data buffer correctly.
02/23/12   fj      Add SVLTE support in API cm_get_current_service_status().
11/30/11   jh      Added feature for dynamic rat acq order change
02/13/12   xs      VT upgrade/downgrade support
01/20/12   gm      Data synchronization for CM Call objects
01/17/12   aj/cl   SC SVLTE SO 33 suspend changes
01/10/12   gm      Mainlining HDR API change integration
01/06/12   gm      Data synchronization changes for freefloating platform
11/02/11   gm      Changes for HDR API cleanup
09/19/11   xs      Added LTE to valid digital sys checking
05/13/11   skp     validating req. params in cm_ph_cmd_dual_standby_pref
04/12/11   xs      Added band and channel info in cm_get_current_service_status
04/12/11   fj      Added new API cm_ph_cmd_detach_and_plmn_blocking().
04/11/11   am      Cleaning stubs and runnning qtf SUTE
04/08/11   sv      Use DRS bit value from EFS for Voice calls
03/23/11   rk      Allow client to choose the srv opt for 1XCSFB call
02/27/11   jqi     Fix the link error for 8K FUSION TYPE 2 ES2 release.
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/07/11   xs      Remove FEATURE_T53 for hold answer
02/01/11   skp     Fix: CR-272848: Corrected subscription source param in GW
                   mode in cm_mmgsdi_ssn_tbl_get_ssn_info_for_gw_slot().
01/04/11   rm      CMI-4: Moving global data to private files
12/23/10   sv      Integrate DSDS feature
12/20/10   xs      Added debug message in subscription command
11/30/10   gm      Enabled cm_ph_cmd_nam_sel for FEATURE_JCDMA
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/20/10   xs      Added new API cm_ph_cmd_signal_strength_delta_lte
10/20/10   xs      Fix cm_num_copy issue when input len is 0
10/05/10   am      Temporary fix for Call Control rejects 1x voice call
                   origination using SIM toolkit
10/01/10   ak      Merged the Fusion code from Dev branch.
10/01/10   am      Temp. fix so that 1x call may go through.
09/23/10   am      Adding check for app type before providing session info
09/15/10   rm      Release-7 changes
07/16/10   xs      Added new API cm_get_current_sys_mode
07/14/10   mh      Fixed compile warnings
07/11/10   am      Merging call control cb function registration APIs changes
06/21/10   jqi     Remove cm_mm_fusion.h.
06/15/10   aj      support for IRAT measurement support
05/27/10   fj      Added esm_local_cause in cm_res_mod_s_type.
04/16/10   fj      Added support for LTE to GW call object transfer.
02/19/10   fj      Removed act_bearer_rsp from cm_lte_call_info_s_type.
01/20/10   fj      Merged changelist 987678.
12/08/09   fj      Added lte_band_pref in necessary APIs.
11/03/09   np      Added esm local cause for PDN Disconnect Request
11/12/09   fj      Fixes for CFG_LTE.
11/09/09   sg      Add non-demand paging macro to function that are not to be
                   demand-paged.
11/09/09   rn      reset cmd_cc_cb_func
10/23/09   sv      Updating cm_rab_reestab_params_s_type
10/22/09   rm      Adding Modem Statistics changes
08/31/09   fj      Added support for LTE UE initiated call funtionality.
07/27/09   fj      Added esm_cause, esm_local_cause, and protocol_config_options
                   copy in cm_mm_call_cmd_act_bearer_rsp();
                   included cmltecall.h.
09/25/09   ks      Adding changes for Nw Initiated QOS
03/07/09   np      LTE network initiated dormancy
12/22/09   fj      Added new API cm_mm_call_cmd_act_bearer_rsp().
09/15/09   rm      Adding wrapper functions for cmph_get_gw_subscription_source
                   and cm_ph_get_cdma_subscription_source
09/01/09   sv      Removing un-necessary clk API
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
07/26/09   rn      Centralised call control changes
06/09/09   ks      Provide new API to get timing_advance and current
                   CDMA position info through CM
06/04/09   aj      Add check to reject call if srv type is invalid
04/06/09   mj      Update BOOLEAN macro w/ namespace prefix to CM_BOOLEAN
03/24/09   sv      Added cm_ph_get_oprt_mode
03/24/09   sv      updated to cmtaski.h
03/11/09   rk      Fixing cm_get_pref_srv_type_for_packet_orig to return WLAN
                   when phone mode preference is set to WLAN_ONLY
02/27/09   rm      Adding header files of other MOBs
02/23/09   rm      Adding /inc files
02/20/09   rm      Lint clean post CMI changes
02/17/09   mh      Obsoleted avoid_time in cm_ph_cmd_avoid_sys()
02/12/09   mh      Added support for no SR after MAP for IS-707B
12/08/08   fj      Removed function cm_get_mode_band_capability()
12/21/08   rn      FDN checking option in call control
12/03/08   ak      Corrected Feature name to FEATURE_HDR_BCMCS_DYNAMIC_MODEL.
12/03/08   ak      Featurized REG_HANDOFF and BOM_CACHE for QCHAT
11/26/08   jd      Fixing Lint Errors
11/14/08   jd      Fixing Lint Errors
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
10/15/08   rn      Corrected max line chars
09/15/08   np      Added HPCD Rev B.
09/15/08   cl      Add initialization before subscription commands are sent.
                   This allows the service domain to be sent to UI correctly.
09/04/08   dm/sv   Added WPS API for WLAN
09/05/08   sv      Changed Featurization to remove compiler warnings.
08/04/08   rk      Fixed the compilation issue when FEATURE_GPSONE not defined.
07/28/08   rm      Fixing compiler warnings
07/01/08   rm      Mainlining of MBMS changes
06/11/08   fj      Added remoted API cm_get_mode_band_capability
06/12/08   sv      Resolve Klocwork defects
05/28/08   ks      Updating the associated primary profile number when
                   secondary PDP is promoted to Primary
05/06/08   ak      BCMCS 2.0 changes
04/29/08   ks      Reverting the changes of Updating the associated primary
                   profile number when secondary PDP is promoted to Primary
04/07/08   cl      Fix cm_get_pref_srv_type_for_packet_orig() to return
                   correct mode
03/31/08   ks      Updating the associated primary profile number
                   when secondary PDP is promoted to Primary
03/22/08   ic      Replace custmmode.h with modem_mmode_cust.h
                   Replace FEATURE_MMODE_CUSTMMODE_H with FEATURE_CUSTMMODE_H
03/08/08   ic      Corrected Lint comments
03/08/08   ic      Replace FEATURE_MM_CUSTMM_H with FEATURE_MMODE_CUSTMMODE_H
02/29/08   aj      Removed memory leak from cm_ph_cmd_ddtm_pref()
02/28/07   jqi/aj  Added FEATURE_FULL_TIME_SHDR_3 support.
01/03/08   ka      Adding handset based plus code dialing feature.
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/08/07   jqi     CM shall pass the dail string when user decide to forward
                   the incoming Call.
12/07/07   ra      Added support for IMS VCC in HOMER files
11/20/07   vs      Lint Fix for CR#130667 - length of the dailed digitsis
                   wrong in cmdiag
10/03/07   rn      Added cm_mm_call_cmd_ps_sig_release
09/25/07   dm      Added support for FEATURE_MM_SUPERSET
08/17/07   ic      Added compile time checks for custmmode.h and obsoleted
                   custcm.h
08/17/07   ka/dm   Lint fix
08/07/07   ic      Lint clean up
07/09/07   cl      Move cm_ext_disp_itr_* from cm.c to cmutil.c to resolve
                   link errors. Merge 482706
06/27/07   cl      Add new CM API to expose cmwcall_get_user_data() to RPC
04/23/07   jqi     DOS feature rework for dual processor arch.
04/15/07   rp      Removing FEATURE_MMGPS
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      MSG_HIGH for oprt_mode request.
02/23/07   ic      Zero out memory before initializing fields of wlan_pref_ptr
                   in cm_wlan_pref_init()
02/18/07   rp      Replaced FEATURE_APIONE with FEATURE_WMS_CM_NAS_INTERFACES
02/22/07   pk      C-GPS changes
12/05/06   ic      Added check for FEATURE_CM_CUSTCM_H to make sure the right
                   custcm.h file is pulled in by target builds
11/16/06   pk      Lint Fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/04/06   ka      Removing garbage characters that were causing compilation.
                   failure.
09/29/06   rp/ka   Added code to initialize all the fields in sups object.
09/14/06   ka      Adding call control for secondary PS calls.
08/29/06   sk      Added WLAN Adhoc support.
08/28/06   ka      Adding pdp header comp and data comp enums.
07/19/06   ic      Lint fixes
07/07/06   pk      Added cmtask_add_cmd_to_hold_q, and supporting data
                   structures, fields and functions
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Adding Call control Support for PS calls.
07/03/06   ic      Cleaned up leftovers from mainlining FEATURE_SUPS
06/08/06   ic      Fixed RPC featurization problem.
                   Moved featurization into inside of cm_is_valid_mode_pref()
                   to avoid having to featurize cm.h
06/05/06   ka/ic   Moving buffer length to copy size check, before memcpy
                   in cm_num_get.
05/03/06   pk      Lint Fixes
04/13/06   ka      Initializing gw ps info in call cmd structure.
04/01/06   ka      Changes include emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/24/06   ka      Adding VideoTelephony fallback to voice feature for UMTS.
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/26/06   ka      Adding NULL check to dialed_digits argument passed to
                   sups cmd.
01/18/06   ic      In cm_num_fill() initialize number even if number lenght
                   is 0
01/12/06   ic      Trimmed to minimum the list of lint errors that were
                   suppressed for va_list, va_start(), va_arg(), va_end()
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Undid CM API changes from lint clean up for cm_num_init()
                   and cm_num_copy()
01/06/06   ic      Lint cleanup
12/13/05   ic      Corrected comments in cm_bcmcs_cmd_flow_request()
12/13/05   ic      Fixed compiler warnings "different const qualifiers"
12/11/05   ic      Corrected Lint suppression comment so it does not turn on
                   613 warning
12/07/05   ka      Adding minimum_qos to cm_mm_call_cmd_pdp_modify()
11/23/05   ka      Modification to allow hold/active for ip calls.
10/21/05   sk      Removed all unnecessary checks on and modification of
                   digit_mode.
09/26/05   sk      Changed back cm_num_fill() to consider a, b and c as ASCII
                   characters.
09/12/05   pk      Added GPS session related API's
09/01/05   sk      Modified cm_num_fill to accept a, b and c as DTMF chars.
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/19/05   sk      Completely removed is_jcdma_emergency flag.
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
07/15/05   ic      Fixed lint warning 'symbol cm_cmd_alloc_pd_init() not used'
07/11/05   pk      Modified cm_ss_cmd_orig_thr_update_tbl() API, added
                   packet_zone as one more parameter.
07/06/05   ka      Initializing subscription related params of cmd ptr.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/10/05   ic      Lint clean up
06/09/05   ic      In cm_mm_call_cmd_sups() added copying of call_id into
                   common cmd_info_ptr (missed in previous check in)
06/09/05   ic      Changes to cm_mm_call_cmd_sups()
                   - added checks for SUPS params
                   - clean up to make it more readable
                   Removed all varieties of EJECT comments
06/09/05   dk      Fixed cm_get_pref_srv_type_for_packet_orig to return
                   correct SYS_MODE.
06/06/05   pk      Added cm_ss_cmd_orig_thr_update_tbl(). This API queues a
                   command to CM to update the origination throttle table
05/11/05   ic      Externalized cm_ph_net_list_copy()
04/08/05   pk      Changed cmph_get_cur_hybr_status(void), to
                   cmpref_is_hybr_status_on(). Included header file cmpref.h
04/07/05   ic      In cm_mm_call_cmd_orig() test is_jcdma_emergency field and
                   use default if not set correctly
03/14/05   ka      Adding IP call support for answer and set up response
                   api's.
02/17/05   pk      Added cm_is_sys_mode_allowed() to verify the
                   compatiblity of the mode_pref, band_pref and sys_mode
02/17/05   ic      Print parameters to cm_ph_cmd_change_rtre_config()
02/15/05   pk      Corrected alignment
02/09/05   ic      Lint cleanup
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/07/04   ws      Moved cm_num_init () forward in file.
12/03/04   ws      Add API for remote WMS.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP feature
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/27/04   jqi     Added ps dormant optimization.
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/17/04   ic      Undoing P4 change list #121638
08/12/04   ws      Remove cm20 support.
08/02/04   ic      Lint clean up - cm.c now free of Lint issues in all CUTE
                   configurations
07/28/04   ka      Added support for secondary PDP context
07/27/04   ws      Call Control support for SUPS.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Fixed ADS 1.2 compiler warning 'unsigned comparison with 0
                   is always true' by not using INRANGE() macro (casting its
                   parameters to int32 does the trick on RVCT 2.1 compiler but
                   not on ADS 1.2)
07/21/04   sj      Added support for umts to 1x handover.
07/14/04   ws      Fixed GSTK dependencies.
07/12/04   ic      Added FEATURE_ALS support.
                   Removed assertion from cm_cmd_alloc_ph_init() to be able to
                   test in CUTE that CM returns appropriate error when memory
                   allocation fails.
                   In cm_cmd_s_type replaced client field with client_id to make
                   it consistent with the rest of CM objects.
                   Removed code placed under FEATURE_HDR_TEMP.
                   In cm_m51_mode_reg() corrected formatting of if statements.
07/02/04   sj      Fixed lint errors.
06/30/04   ws      Updated cm_pd_cmd_reva_mo_msg to take cmd_cb_func
06/22/04   ic      Fixed obvious RVCT 2.1 compiler warnings
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/04/04   ka      Fixed Lint errors
06/01/04   ws      Fix num type and num plan validation routines.
05/10/04   ka      Fixed Lint errors.
04/30/04   ws      num type and num plan modifications.
04/29/04   ic      Removing outdated comments.
04/28/04   ka      Fixed Lint errors.
04/13/04   ic      Featurization changes related to 6500 5.0 CUST files being
                   used
04/01/04   ws      Updated with favored_mode mode independent.
03/23/04   ws      Merged from 6300.
03/15/04   ka      Fixed Lint errors
03/10/04   ic      FEATURE_BCMCS - removing throwing of an error when
                   CM_BCMCS_CMD_FLOW_REQUEST_F is queued with port number
                   equal to 0
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial Jaguar/Mainline merge.
01/20/04   ka      Added support for Multiple PDP and get call info api
01/19/04   ws      Modified handling of '+' character in cm_num_fill
01/15/04   ws      Fixed problem with cmd err in handling voice as data cmds.
01/13/04   ka      Merged changes from MSM6200
01/07/03   ws      Added handling for second slot for dual slot operations.
12/15/03   tml     Added revision history comment
12/15/03   tml     Fixed call control
12/12/03   jqi     CR38261-China Unicom test.
11/19/03   sbs     Added LCS MOLR Completed API function.
11/14/03   ws      Added support for is_jcdma_emergency.
11/07/03   ws      Fix GW/CDMA featurization problem with otasp_act code.
11/03/03   ws      Fix LTK compile errors.
10/31/03   ws      Include otasp act code/call type in cc processing.
10/27/03   ws      Removed uim pref slot selection support.
10/20/03   ws      Dual Slot Support.
                   Fixed misspelling with unavailable in several enums
                   Added F3 msgs to print out what ps orig acq mode is returned.
10/08/03   ws      Added support for checking NV for ps data calls.
10/07/03   vt      Prohibited more mode_pref's on sys_sel_pref command.
10/06/03   vt      Modified to support common cm.h across all targets.
10/01/03   ws      Mainlined DS_VOICE_AS_DATA
09/29/03   ws      Removed the workaround for Diag call end.
09/23/03   ws      Added support for getting currently acquired system.
09/16/03   vt      Added preferred slot handling.
08/29/03   ws      Added support for FEATURE_CM_PACKET_ORIG_CDMA_ONLY
08/26/03   ws      Removed RelA featurization for flash type.
08/22/03   vt      Mainlined packet state handling.
08/13/03   prk     Added processing of minimum_qos in PS data call origination.
07/08/03   sbs/lt  Added support for Location Services related SS messages
06/16/03   ws      Added check in memcpy qos_parms for overflow
05/28/03   vt      Added checks for new service types for origination.
05/20/03   ws      Modified cm_mm_call_cmd_end to return TRUE at the end.
05/15/03   vt      Added a function to return current service for PS call.
                   Fixed some compiler warnings.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/25/03   ws      JCDMA support for call hold
04/28/03   AT      Lint cleanup.
04/14/03   prk     Mainlined FEATURE_SUPS.
04/14/03   ws      Added BSID changes
03/21/03   hxw     Changed FEATURE_GPSONE_OOS to FEATURE_GPSONE_MSBASED.
03/20/03   ws      Modified call_end routine so that if the client is diag & Rel0,
                   the call id will be modified to the first active call found.
03/11/03   ATT     Added support for call control performed by clients.
02/27/03   cs      Added support for PS data continuity.
02/27/03   ws      Added check for calling number being null in cmd orig
                   Removed reference to end_reason_included.
02/18/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/11/03   prk     Modified cm_mm_call_cmd_sups() to assign the mode info type
                   and call id to the command info structure.  Modified
                   cm_num_init() to correctly initialize the number plan
                   and type for certain targets.
02/07/03   prk     Modified cm_mm_call_cmd_sups() to set cmd info type to
                   CM_CALL_MODE_INFO_GW_CS.
02/04/03   ri      Added support for concurrent services.
                   - modified cm_mm_call_cmd_end().
01/21/03   ic      In cm_cmd_alloc_ph_init() initialized
                   cmd_info_ptr->answer_voice to CM_ANSWER_VOICE_NONE
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Added support for accessing user preferred PLMN list.
11/20/02   vt      Fixed a bug where call id can't be allocated in orig call.
11/19/02   vt      Added changing RTRE configuration.
                   Fixed a compiler warning.
11/15/02   vt      Fixed a compile problem for release A.
                   Properly featurized the packet state.
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Continued merge in position determination changes
                   [ws]Merged in position determination changes
                   [RI}Modified cm_mm_call_cmd_end() for GSM/WCDMA modes.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/31/02   vt      Initialized the number fields to 0.
                   Mainlined feature SD20.
10/30/02   vt      Fixed a compile error.
10/22/02   ic      Added M51 mode object.
                   Fixed compile errors (a field changed from client to
                   client_id).
10/22/02   vt      Fixed a bug in determining minimum of alpha length.
10/22/02   vt      Fixed problems with sups_cmd for 1x.
10/05/02   vt      Removed setting of mode_info for call orig command.
08/30/02   vt      Made several functions return error status if proper
                   features are not defined. Moved cm_aoc_init function
                   prototype to cmwaoc.h
08/12/02   RI      Support for ECT and CD features (GSM/WCDMA).
08/06/02   HQ      Added FEATURE_CDSMS.
07/24/02   vt      Added handling for wakeup from deep sleep(standby)
07/23/02   PRK     Added cm_ph_cmd_get_networks and cm_ph_cmd_terminate_get_
                   networks.
07/11/02   PRK     Included cmll.h.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/03/02   ws      Added a check for NULL data_block_ptr in cm_call_cmd_orig
06/13/02   ws      Changed reference of cmmc.h to cmxll.h
06/11/02   RI      Merged the allocation of call IDs in different
                   modes in cm_mm_call_cmd_orig().
05/17/02   RI      Modified cm_sups_cmd_process_uss to initialze ss_operation
                   field.  Modified processing of called_number and calling_
                   number parameters for call orig command.
04/05/02   hxw     Featurize cmpd_init() with FEATURE_GPSONE_DBM.
04/02/02   hxw     Modified FEATURE_GPSONE_MSBASED.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
03/05/02   az      Changed the signal name for NV writes.
02/06/02   sh      Added hybrid 3.0 support
01/25/02   sj      Added support for tiered services - user zone support.
05/07/02   PRK     Included cmxpd.h instead of cmpd.h.
                   Included cmxsms.h instead of cmsms.h
05/05/02   vt      Fixed compile problem with new mm cm.h
03/11/02   RI      Added CM_CALL_CMD_ERR_NULL_PARAM_P,
                   CM_CALL_CMD_ERR_NO_CALL_ID_S
                   CM_SUPS_CMD_ERR_NULL_PARAM_P
                   Added ss_success field in cm_sups_info_s_type.
                   Deleted cm_sups_cmd_abort().
02/13/02   ic      Changed the type of call_id variable from byte/uint8 etc
                   to cm_call_id_type as it should have been to begin with.
                   Compile errors fixes related to the fact that call_id field
                   was removed from cm_inband_cmd_s_type (not needed, we use
                   call_id field in cm_inband_cmd__info_s_type instead)
02/07/02   ic      Fixes related to the fact that some structures' fields
                   are now compiled under FEATURE_IS2000_REL_A.
                   cm_nc_is_flash_emergency_f_type *cm_nc_is_flash_emergency_func
                   field was removed from cm_nc_s_type structure.
02/06/02   PRK     Added CM_OPRT_MODE_TEST to cm_oprt_mode_e_type.
02/04/02   RI      Modifications based on mn_cm_exp.h for Supplementary
                   Services.
                   - removed mn_cm_exp_t.h
                   - modified SUPS cmd err definitions
                   - modified cm_sups_con_s_type.
01/23/02   YR      Added CM_SIGNAL_NETWORK_ALERT in cm_signal_e_type.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the
                   changes between those two were merged with CCS CM
01/14/02   ic      Fixes related to running with FEATURE_IS2000_REL_A
                   undefined
01/14/02   PRK     Updated PLMN selection/reselection types/enums due to
                   lower layer software interface changes.
                   Removed featurization of the service_domain_pref parameter
                   in function ui_set_ph_mode_pref.
                   Featurized future PLMN selection/reselection code with
                   FEATURE_NEW_PLMN.
                   Added service_status field to cm_selected_network_-
                   attributes_s_type.
                   Added CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S to
                   cm_ph_cmd_err_e_type.
                   Changed stop/start full service to subscription not/available.
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32
                   (CM VU in N*2165)
                   that we needed to merge with and changes between those two
                   VUs were merged onto CCS CM tip.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
11/21/01   RI      Modified the function decaration of cm_find_call_id().
                   Deleted pdp_cause_T defintion.
11/17/01   sh      Added 1xhdr data session handoff support
11/06/01   RI      Modified cm_sups_common_conf_s_type
11/05/01   sh      Used new CM_CALL_TYPE_PKT_DATA for pkt data call orig
11/01/01   sh      Removed is_force_colloc from the call cmd info
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Added support for PLMN selection/reselection
                   (FEATURE_PLMN).
                   Re-added Serving System group and moved srv_state,
                   roam_status, rssi from phone group to serving system group
                   (FEATURE_NEWSS).
10/17/01   AT      Added new events for AOC.
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
10/03/01   VT      Added determining whether flash number is emergency or not.
09/14/01   HD      Reversed the order of get_call_id,cleanup in ph_info_s.
08/23/01   HD      NAS header file reorg changes+ Inband and RSSI reporting
                   for WCDMA.
08/21/01   ic      Main-lined FEATURE_NEWCM
08/01/01   pj      Added call to cmpd_timer_proc.
07/17/01   HD      Added support for PS Data calls.
06/06/01   dna     copy SO for HDR call_cmd_orig_with_drs, too.
04/27/01   HD      Added changes for WCDMA/GSM mode.
04/13/01   SH      Added Preliminary HDR support
03/27/01   RC      Fixed cm_num related compiler errors when CM_ASSERT is
                   turned on.
03/20/01   HQ      Allow NULL pointer for number and alpha buffers in API
                   calls.
02/26/01   sko     Change CM monitor implementation from event to polling.
02/06/01   RC      Added support for System Determination 2.0.
01/29/01   HQ      Properly feature-ized NEWCM, UASMS and GPSONE_DBM.
01/23/01   ych     Merged T53 and JCDMA features.
12/28/00   fc      Added support for retry order.
12/15/00   HQ      Added CM_CALL_CMD_PRIVACY_PREF, CM_CALL_EVENT_PRIVACY_PREF
                   and CM_CALL_CMD_ERR_BAD_PRIVACY_PREF.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
09/15/00   PJ      Added UAPDMS task. Added support for PD commands.
                   (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
07/17/00   jq      Added cm_call_cmd_end_with_reason()
03/20/00   RY      Removed CM_CMD_TYPE_PH from cm_hold_cmd_q_check()
03/16/00   VT      Added BROADCAST SMS support
03/13/00   RY      Added OTAPA support
01/11/00   VT      Added cm_is_valid_mode_pref().
12/18/99   CS      Added cm_kick_dog function.
09/14/99   SH      Removed cm_clk_init
09/07/99   SH      Added uasms_init_cm
08/31/99   SH      Modified codes to featurize OTASP
08/31/99   SH      Modified codes to featurize UASMS
08/25/99   SH      Added cmcall_offline
08/17/99   SH      Used new sound API when new sound feature is defined
08/09/99   SH      Added SMS object
07/07/99   SH      Added CNAP feature
06/30/99   SH      Added activate_code for cm_call_cmd_orig
06/28/99   SH      Modified cm_nc_init
06/21/99   SH      Added processing for TASK_STOP_SIG and TASK_OFFLINE_SIG
05/31/99   SH      Added CM 2.0 features
04/09/99   jkl     Modified for new Sound API
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Customer configuration file
*/
#include "mmcp_variation.h"
#include "customer.h"

/* This is to make sure all targets pull in custmmode.h
*/
#ifndef FEATURE_CUSTMMODE_H
#error  Need to pull in custmmode.h!!!
#endif

/* This is to make sure nobody uses obsoleted custcm.h
*/
#if (defined CUSTCM_H) || (defined FEATURE_CM_CUSTCM_H)
#error custcm.h is obsolete, use custmmode.h instead !!!
#endif

#include "sys.h"       /* External interface to cm.c */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "sys.h"
#include "sys_v.h"
#include "sys_gw.h"
#include "sys_gw_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "queue.h"     /* Interface to queue services */
#include <string.h>    /* Interface to string library */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */
#include "rex.h"       /* Interface to rex services */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmautoreg.h" /* Interface to CM Auto registration */
//#include "cmll.h"      /* Interface to cm_call_id_allocate/deallocate */
#include "cmll_v.h"
#include "cmrpm.h"
#include "cmrpm_lte.h"

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cai.h"       /* Over the air messages definitions */
#include "cai_v.h"
#include "mc.h"        /* Interface to MC Task */
#include "mc_v.h"
#include "cmxcall.h"   /* Interface to CM CDMA call object */
#include "cmxll.h"      /* MC to CM interface*/
#include "db.h"        /* Interface to Database services */
#include "nv.h"        /* Interface to NV services */
#include "task.h"      /* Interface to task services */
#include "cmnv.h"      /* Interface to CM NV services */
#include "cmss.h"     /* Interface to CM serving system object */
#include "cminband.h"  /* Interface to CM inband object */
#include "mccdma.h"    /* Interface to current base station */
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#include "cmsms.h"     /* Interface to cmsms.c*/

#ifdef CM_GW_SUPPORTED
#include "cmutil.h"
#include "cmwaoc.h"
#include "cmwcall.h"
#endif /* CM_GW_SUPPORTED */

#include "cmxdbm.h"     /* Interface to Data Burst Messages */

#ifdef FEATURE_GPSONE
#include "cmxpd.h"     /* Position determination code */
#endif  /* FEATURE_GPSONE */

#include "cmtaski.h"

#ifdef FEATURE_HDR_HYBRID
#include "hdrcp_msg.h"
#endif /* FEATURE_HDR_HYBRID */

#if (defined(FEATURE_GSM) && defined(FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE) )
#include "geran_eng_mode_read_api.h"
#endif  /* defined(FEATURE_GSM) && defined(FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE) */

#include "cmcc.h"      /* CM Centralized call control */

#ifdef FEATURE_CM_LTE
#include "cmltecall.h"
#endif /* FEATURE_CM_LTE */

#include "cmmmgsdi.h"
#include "cmpmprx.h"

#ifdef FEATURE_QSH_EVENT_METRIC
#include "cmdbg_qsh.h"
#endif

#ifdef CM_DEBUG
#error code not present
#endif

#include "cmdrvcc.h"

/**
** Type definition of a buffer that contains a number
*/
#ifdef CM_GW_SUPPORTED
typedef uint8 cm_num_buf_type[CM_MAX_NUMBER_CHARS];
#endif

/*---------------------------------------------------------------------------
      Client registered callbacks for retrieving bearer capability,
      High level compatibility and low level compatibility for specific
      call types.
---------------------------------------------------------------------------*/

#ifdef CM_GW_SUPPORTED
/* Stores callback for specific call types
*/
static cm_call_comp_cap_cb_s_type call_comp_cap_cb_store[] = {

  { CM_CALL_TYPE_VOICE,   NULL },
  { CM_CALL_TYPE_VT,      NULL }

};
#endif /* CM_GW_SUPPORTED */

/* Auto Register */
cm_dev_model_info_status_e_type cm_model_initialized = DEV_MODEL_INIT_NULL;

/* Stores local device model info for cm
*/
cm_device_model_info_efs cm_model_info;

#if (defined CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK) || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
static boolean cm_mm_call_cmd_modify_req_internal (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  cm_call_type_e_type           new_call_type_req,
    /* New call type requested for call modification */

  const cm_ip_call_attrib_info_s_type  *new_call_attrib
   /* New call attribute info for audio and video */
);

static boolean cm_mm_call_cmd_modify_res_internal (
  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  boolean                       modify_res,
    /* Client response to modification request
    ** TRUE   - Modification is accepted by client
    ** FALSE  - Modification gets rejected by client
    */

  ie_cm_cc_cause_s_type         *cc_cause_ptr,
    /* Specific cause value when gw_cs_modify_res is FALSE
    ** Ignored when gw_cs_modify_res is TRUE.
    */

  cm_call_type_e_type           call_type_rsp,
    /* New call type respond for call modification */

  const cm_ip_call_attrib_info_s_type  *call_attrib
   /* New call attribute info for audio and video */

);
#endif

static boolean cm_is_valid_mode_pref_per_subs(
  cm_mode_pref_e_type     mode,
  sys_modem_as_id_e_type  asubs_id
);

/*==========================================================================
============================================================================
============== UTIL functions required to get max values ===================
===========================================================================
==========================================================================*/

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*==========================================================================

FUNCTION: cm_get_inband_max_rev_dialed_digits

DESCRIPTION
  This function returns the max value of
  max_called_info_chars depending on FEATURE_JCDMA
  is defined or not

DEPENDENCIES
  FEATURE_GSM ||FEATURE_WCDMA

RETURN VALUE
 maximum no. of emergency count based on definition of feature

SIDE EFFECTS
  None
==========================================================================*/
static int cm_get_inband_max_rev_dialed_digits( void )
{
  #ifdef FEATURE_JCDMA
  return CM_INBAND_MAX_REV_DIALED_DIGITS_JCDMA;
  #else
  return CM_INBAND_MAX_REV_DIALED_DIGITS;
  #endif
}
#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

/*===========================================================================
=============================================================================
============================ CM COMMAND FUNCTIONS ===========================
=============================================================================
===========================================================================*/

#if (defined (FEATURE_BCMCS) || defined (FEATURE_MBMS))
/*===========================================================================
FUNCTION cm_cmd_alloc_bcmcs_init

DESCRIPTION
  Allocate a BCMCS command buffer and initialize its BCMCS command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  None

===========================================================================*/
static cm_bcmcs_cmd_s_type *cm_cmd_alloc_bcmcs_init( void )
{
  cm_bcmcs_cmd_s_type      *bcmcs_cmd_ptr = (cm_bcmcs_cmd_s_type*)cm_cmd_alloc(sizeof(cm_bcmcs_cmd_s_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** If command allocation is successful, initialize call command fields.
  */

    /*
    ** 1. Initialize BCMCS command fields
    */
    bcmcs_cmd_ptr->client_id                 = CM_CLIENT_ID_ANONYMOUS;
    bcmcs_cmd_ptr->client_ptr                = CM_CLIENT_PTR_NONE;
    bcmcs_cmd_ptr->cmd                       = CM_BCMCS_CMD_NONE;
  bcmcs_cmd_ptr->cmd_type                  = CM_CMD_TYPE_BCMCS;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return bcmcs_cmd_ptr;
} /* cm_cmd_alloc_bcmcs_init() */
#endif /* (FEATURE_BCMCS || FEATURE_MBMS) */


#if defined (FEATURE_MBMS)
#error code not present
#endif /* #if defined (FEATURE_MBMS) */

#if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
/*===========================================================================

FUNCTION cm_cmd_alloc_pd_init

DESCRIPTION
  Obtains a CM command buffer from the free queue and initializes it for
  a PD command.

DEPENDENCIES
  CM must be initialized.

RETURN VALUE
  Pointer to the buffer, NULL otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*lint --e{528} suppress cm_cmd_alloc_pd_init not referenced
** warning
*/
static cm_pd_cmd_s_type* cm_cmd_alloc_pd_init( void )
{
  cm_pd_cmd_s_type            *pd_cmd_ptr        = (cm_pd_cmd_s_type*)cm_cmd_alloc(sizeof(cm_pd_cmd_s_type));
  cm_pd_cmd_info_s_type    *cmd_info_ptr   = NULL;

  cmd_info_ptr   = CMD_INFO_PTR( pd_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the sms command fields.
  */

    /*---------------------------------
      Initialize command type
     *------------------------------ */

  pd_cmd_ptr->cmd_type = CM_CMD_TYPE_PD;

    /*---------------------------------
      Now the header.
     *------------------------------ */

    pd_cmd_ptr->client_ptr     = CM_CLIENT_PTR_NONE;
    pd_cmd_ptr->client_id      = CM_CLIENT_ID_ANONYMOUS;
    pd_cmd_ptr->cmd            = CM_PD_CMD_NONE;

    cmd_info_ptr->cmd_err      = CM_PD_CMD_ERR_NOERR;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return pd_cmd_ptr;

}
#endif /* FEATURE_GPSONE_MSBASED || FEATURE_GPSONE */

/*===========================================================================

FUNCTION cm_pd_cmd_fix_start

DESCRIPTION
  Send a start session command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cm_pd_cmd_fix_start) */
/*lint -esym(765, cm_pd_cmd_fix_start)
** cm_pd_cmd_fix_start() not referenced
** It's not called in some configurations and can't turn it back on at the
** end of the function as Lint will then continue complaining about it
*/
boolean cm_pd_cmd_fix_start(

  cm_pd_cmd_cb_f_type   cmd_cb_func,
      /* client callback function */

  void                  *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type     client_id
      /* requesting client ID */
)
/*lint -esym(715,client_id)*/
{
    #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE)) && defined (FEATURE_HDR_HYBRID)
    cm_pd_cmd_s_type           *pd_cmd_ptr;

    pd_cmd_ptr = cm_cmd_alloc_pd_init();

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      pd_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      pd_cmd_ptr->data_block_ptr  = data_block_ptr;
      pd_cmd_ptr->client_id       = client_id;
      pd_cmd_ptr->cmd             = CM_PD_CMD_FIX_START;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)pd_cmd_ptr );

      return TRUE;
    #else
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PD_CMD_FIX_START,
        CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
    #endif /* ((FEATURE_GPSONE_MSBASED || FEATURE_GPSONE) && FEATURE_HDR_HYBRID)*/
}
/*lint +esym(715,client_id)*/

/*===========================================================================

FUNCTION cm_pd_cmd_fix_end

DESCRIPTION
  Send a end session command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cm_pd_cmd_fix_end) */
/*lint -esym(765, cm_pd_cmd_fix_end)
** cm_pd_cmd_fix_end() not referenced
** It's not called in some configurations and can't turn it back on at the
** end of the function as Lint will then continue complaining about it
*/
boolean cm_pd_cmd_fix_end(

  cm_pd_cmd_cb_f_type   cmd_cb_func,
      /* client callback function */

  void                  *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type     client_id
      /* requesting client ID */

)
/*lint -esym(715,client_id)*/
{
    #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE)) && defined (FEATURE_HDR_HYBRID)

    cm_pd_cmd_s_type           *pd_cmd_ptr;

    pd_cmd_ptr = cm_cmd_alloc_pd_init();

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      pd_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      pd_cmd_ptr->data_block_ptr  = data_block_ptr;
      pd_cmd_ptr->client_id       = client_id;
      pd_cmd_ptr->cmd             = CM_PD_CMD_FIX_END;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)pd_cmd_ptr );

      return TRUE;
    #else
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PD_CMD_FIX_END,
        CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
    #endif /*((FEATURE_GPSONE_MSBASED || FEATURE_GPSONE) && FEATURE_HDR_HYBRID)*/
}
/*lint +esym(715,client_id)*/


/*===========================================================================

FUNCTION cm_num_init

DESCRIPTION
  Initialize a CM number object.

  This function must be called on a CM number object before it being used,
  in any way, place, or form.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void cm_num_init(

    cm_num_s_type    *num_ptr
        /* Pointer to a CM number to be initialized */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(num_ptr == NULL) /*lint !e774 */
  {
    CM_ERR_0("num_ptr is NULL");
    return;
  }

  /*
  ** Set all fields to 0
  */
  num_ptr->len           = 0;
  memset(num_ptr->buf, 0, CM_MAX_NUMBER_CHARS);
  num_ptr->number_type   = (uint8) CM_NUM_TYPE_UNKNOWN;
  num_ptr->number_plan   = (uint8) CM_NUM_PLAN_UNKNOWN;
  num_ptr->pi            = 0;
  num_ptr->si            = 0;
  num_ptr->digit_mode    = CM_DIGIT_MODE_4BIT_DTMF;

  /*
  ** Subaddress fields: num_char 0 indicates that subaddress is not present.
  */
  num_ptr->subaddr.num_char      = 0;
  memset(num_ptr->subaddr.chari, 0, CM_MAX_SUBADDRESS_DIGITS);
  num_ptr->subaddr.extension_bit = 0;
  num_ptr->subaddr.odd_even_ind  = CM_SUBADDRESS_EO_IND_EVEN;
  num_ptr->subaddr.subadd_type   = CM_SUBADDRESS_TYPE_NSAP;


  num_ptr->emerg_srv_categ       = CM_EMERG_NUM_TYPE_NONE;

  return;

} /* cm_num_init() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
#ifdef FEATURE_UUS
/*===========================================================================

FUNCTION cm_user_data_init

DESCRIPTION
  Initialize a CM user data object.

  This function must be called on a CM user data object (user user signaling)
  before it being used, in any way, place, or form.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static void cm_user_data_init (

    cm_user_data_s_type *user_data
        /* Pointer to a CM user data to be initialized */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( user_data != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Setting fields to 0 (UUS_DATA is enum 0)
  */
  user_data->present                 = FALSE;

  user_data->user_user_type          = UUS_DATA;

  user_data->user_user_data_length   = 0;

  user_data->user_user_protocol_disc = 0;

  user_data->more_data_indicator     = FALSE;

} /* cm_user_data_init() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
#endif /* FEATURE_UUS */

/*===========================================================================

FUNCTION cm_cmd_alloc_call_init

DESCRIPTION
  Allocate a CM command buffer and initialize its call command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static cm_call_cmd_s_type *cm_cmd_alloc_call_init( void )
{
  cm_call_cmd_s_type      *call_cmd_ptr       = (cm_call_cmd_s_type*)cm_cmd_alloc(sizeof(cm_call_cmd_s_type));
  cm_call_cmd_info_s_type *cmd_info_ptr;

  #ifdef CM_GW_SUPPORTED
  cm_call_cmd_info_gw_cs_s_type       *cmd_info_gw_cs_ptr = NULL;

    cm_call_cmd_info_gw_ps_s_type     *cmd_info_gw_ps_ptr = NULL;
  #endif /* CM_GW_SUPPORTED */

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cm_call_cmd_info_cdma_s_type      *cmd_info_cdma_ptr = NULL;
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

  int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
    ** 1. Initialize to call command fields
  */
    call_cmd_ptr->client_id                 = CM_CLIENT_ID_ANONYMOUS;
    call_cmd_ptr->client_ptr                = CM_CLIENT_PTR_NONE;
    call_cmd_ptr->cmd                       = CM_CALL_CMD_NONE;
    call_cmd_ptr->client_categ              = CM_ORIG_CLIENT_CATEG_USER;
    call_cmd_ptr->is_modem_cc_api           = FALSE;
    call_cmd_ptr->cmd_subs                  = SYS_MODEM_AS_ID_NONE;
    call_cmd_ptr->direction                 = CM_CALL_DIRECTION_NONE;
    call_cmd_ptr->cmd_type                  = CM_CMD_TYPE_CALL;

    /*
    ** 2. Initialize call command information fields.
    */
    cmd_info_ptr      = CALL_CMD_INFO_PTR( call_cmd_ptr );
    cmd_info_ptr->cmd_err                   = CM_CALL_CMD_ERR_NOERR;
    cmd_info_ptr->call_id                   = CM_CALL_ID_INVALID;
    cmd_info_ptr->call_type                 = CM_CALL_TYPE_NONE;
    cmd_info_ptr->srv_type                  = CM_SRV_TYPE_NONE;
    cm_num_init( &cmd_info_ptr->num );
    cmd_info_ptr->alpha.len                 = CM_ALPHA_LEN_NONE;
    cmd_info_ptr->num_end_call_ids          = 0;
    for ( i = 0; i < CMCALL_MAX_CALL_OBJ; i++ )
    {
      memset( &cmd_info_ptr->end_params[i], 0,  sizeof( cm_end_params_s_type) );
      cmd_info_ptr->end_params[i].call_id   = CM_CALL_ID_INVALID;
    }
    cmd_info_ptr->end_status                = CM_CALL_END_NONE;
    cmd_info_ptr->info_type                 = CM_CALL_MODE_INFO_NONE;
    cmd_info_ptr->favored_mode_pref         = CM_MODE_PREF_NONE;
    cmd_info_ptr->favored_hybr_pref         = CM_HYBR_PREF_NONE;

    cmd_info_ptr->is_cdma_info_present      = FALSE;

    cmd_info_ptr->is_gsm_wcdma_cs_info_present = FALSE;
    cmd_info_ptr->is_gsm_wcdma_ps_info_present = FALSE;
    cmd_info_ptr->sups_cmd_type                = CM_CALL_SUPS_TYPE_NONE;
    cmd_info_ptr->cc_generic_params.is_fdn_to_check = TRUE;

  /* Initializing drvcc setup info related variables */
    cmd_info_ptr->drvcc_setup_info.as_id = SYS_MODEM_AS_ID_NONE;
    cmd_info_ptr->drvcc_setup_info.num_of_calls = 0;
    memset(cmd_info_ptr->drvcc_setup_info.stn_nr, 0, NV_MAX_DIAL_DIGITS);
    memset(cmd_info_ptr->drvcc_setup_info.call_context, 0, sizeof(cmd_info_ptr->drvcc_setup_info.call_context));

    /*
    ** 4. Initialize call command mode-specific information fields.
    */
    #ifdef CM_GW_SUPPORTED
    cmd_info_gw_cs_ptr = WCALL_CMD_INFO_PTR( call_cmd_ptr );
    cmd_info_gw_cs_ptr->accept               = FALSE;
    cmd_info_gw_cs_ptr->cm_clir              = CM_CLIR_DEFAULT;
    cmd_info_gw_cs_ptr->sups_cmd_params.call_id  = CM_CALL_ID_INVALID;
    cmd_info_gw_cs_ptr->new_call_type_req        = CM_CALL_TYPE_NONE;

    #ifdef FEATURE_UUS
    /* Initializing data for user to user signaling */
    cm_user_data_init (&cmd_info_gw_cs_ptr->mo_user_data);
    #endif

    cm_num_init( &cmd_info_gw_cs_ptr->sups_cmd_params.sups_number);

        /* Clear gw ps structure */


    cmd_info_gw_ps_ptr = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr );

    /* used for accepting/rejecting MT PS call */
    cmd_info_gw_ps_ptr->accept            = FALSE;

    /* PS call is primary/secondary */
    cmd_info_gw_ps_ptr->primary_pdp       = FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*****************************************
    ** Default PDP Activation Parameters
    ** 24.008 section 9.5.1
    *****************************************/

    /* Protocol descriptor */
    cmd_info_gw_ps_ptr->sm_pd          = CM_SM_PD;

    /* Transaction id*/
    cmd_info_gw_ps_ptr->trans_id       = CM_UNKNOWN_CONN_ID;

    /* Message type */
    cmd_info_gw_ps_ptr->msg_type       = CM_ACTIVATE_PDP_REQ;

    /* LLC SAPI */
    cmd_info_gw_ps_ptr->llc_sapi.valid = TRUE;
    cmd_info_gw_ps_ptr->llc_sapi.sapi  = CM_LLC_SAPI_3;

    /* NSAPI */
    cmd_info_gw_ps_ptr->nsapi.valid    = FALSE;

    /* qos for secon pdp */
    cmd_info_gw_ps_ptr->qos.valid         = FALSE;

    /* min qos for secon pdp */
    cmd_info_gw_ps_ptr->minimum_qos.valid = FALSE;

    /* Mostly IP address of connected call */
    cmd_info_gw_ps_ptr->pdp_addr.valid    = FALSE;

    /* Access point name */
    cmd_info_gw_ps_ptr->apn_name.valid    = FALSE;

    /* Config options for Rel 5 */
    cmd_info_gw_ps_ptr->config_options.valid = FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmd_info_gw_ps_ptr->h_comp            = SYS_PDP_HEADER_COMP_OFF;

    cmd_info_gw_ps_ptr->d_comp            = SYS_PDP_DATA_COMP_OFF;

    /* Traffic flow template */
    cmd_info_gw_ps_ptr->tft.valid         = FALSE;

    /* When set to TRUE bring down all calls in PDP group */
    cmd_info_gw_ps_ptr->tear_down_ind.valid = FALSE;

    /* cause is used during call end time, assigning it std
    ** deactiv cause
    */
    cmd_info_gw_ps_ptr->cause             = SM_REGULAR_DEACTIVATION;

    cmd_info_gw_ps_ptr->rabid             = 0;


    /* Group identity of pdp bunch */
    cmd_info_gw_ps_ptr->pdp_group_id      = 0;

    /* Profile number of primary pdp */
    cmd_info_gw_ps_ptr->assoc_pri_profile_num = 0;


    /* Profile num of primary pdp  */
    cmd_info_gw_ps_ptr->profile_number    = 0;

    cmd_info_gw_ps_ptr->request_type.valid  = FALSE;
    
    #endif /* CM_GW_SUPPORTED */



    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    cmd_info_cdma_ptr = CALL_CMD_MODE_INFO_PTR( call_cmd_ptr );
    cmd_info_cdma_ptr->srv_opt              = CM_SRV_OPT_NONE;
    cmd_info_cdma_ptr->otasp_act_code       = CM_OTASP_ACT_CODE_NONE;
    cmd_info_cdma_ptr->is_special_srv_data_call = FALSE;
    #ifdef FEATURE_IS2000
    cmd_info_cdma_ptr->drs_bit              = TRUE;
    #endif /* FEATURE_IS2000 */
    cmd_info_cdma_ptr->privacy_pref         = CM_PRIVACY_PREF_NONE;
    #ifdef FEATURE_HDR_HANDOFF
    cmd_info_cdma_ptr->last_act_data_net    = SYS_SYS_MODE_NO_SRV;
    #endif /* FEATURE_HDR_HANDOFF */
    #ifdef FEATURE_IS2000_REL_A
    cmd_info_cdma_ptr->sr_id_included       = FALSE;
    cmd_info_cdma_ptr->qos_parms_incl       = FALSE;
    cmd_info_cdma_ptr->qos_parms_len        = 0;
    #endif /* FEATURE_IS2000_REL_A */
    cmd_info_cdma_ptr->flash_type           = CM_CALL_SUPS_TYPE_NONE;
    #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
    #error code not present
#endif


  return call_cmd_ptr;

} /* cm_cmd_alloc_call_init() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif




/*===========================================================================

FUNCTION cm_cmd_alloc_ph_init

DESCRIPTION
  Allocate a CM command buffer and initialize its phone command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_ph_cmd_s_type *cm_cmd_alloc_ph_init( void )
{
  cm_ph_cmd_s_type       *ph_cmd_ptr       = (cm_ph_cmd_s_type*)cm_cmd_alloc(sizeof(cm_ph_cmd_s_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the phone command fields.
  */
  cm_cmd_ph_init(ph_cmd_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ph_cmd_ptr;

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_cmd_ph_init

DESCRIPTION
  Initialize the command buffer with its phone command fields

DEPENDENCIES
  The command buffer has been allocated.

RETURN VALUE
  .

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void cm_cmd_ph_init( cm_ph_cmd_s_type * ph_cmd_ptr)
{
  cm_ph_cmd_info_s_type  *cmd_info_ptr  = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initialize command type field.
    */
    ph_cmd_ptr->cmd_type                       = CM_CMD_TYPE_PH;


    /* Initialize command header fields.
    */
    ph_cmd_ptr->client_id                   = CM_CLIENT_ID_ANONYMOUS;
    ph_cmd_ptr->client_ptr                  = CM_CLIENT_PTR_NONE;
    ph_cmd_ptr->cmd                         = CM_PH_CMD_NONE;
    ph_cmd_ptr->info.csg_id                 = SYS_CSG_ID_INVALID;
    ph_cmd_ptr->info.csg_rat                = SYS_RAT_NONE;

    /* Initialize the hold command field
    */

    cmd_info_ptr->is_command_on_hold        = FALSE;
    cmd_info_ptr->hold_cmd_reason           = CM_HOLD_CMD_REAS_NONE;

    /* Initialize command information fields.
    */
    cmd_info_ptr->cmd_err                   = CM_PH_CMD_ERR_NOERR;
    cmd_info_ptr->mode_pref                 = CM_MODE_PREF_NONE;
    cmd_info_ptr->pref_term                 = CM_PREF_TERM_NONE;
    cmd_info_ptr->srv_domain_pref           = CM_SRV_DOMAIN_PREF_NONE;
    cmd_info_ptr->network_sel_mode_pref     = CM_NETWORK_SEL_MODE_PREF_NONE;
    cmd_info_ptr->acq_order_pref            = CM_GW_ACQ_ORDER_PREF_NONE;

    sys_plmn_undefine_plmn_id( &cmd_info_ptr->plmn );

    cmd_info_ptr->band_pref                 = CM_BAND_PREF_NONE;
    SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&cmd_info_ptr->lte_band_pref);
    cmd_info_ptr->tds_band_pref             = CM_BAND_PREF_NONE;
    cmd_info_ptr->prl_pref                  = CM_PRL_PREF_NONE;
    cmd_info_ptr->roam_pref                 = CM_ROAM_PREF_NONE;

    cmd_info_ptr->oprt_mode                 = SYS_OPRT_MODE_NONE;
    cmd_info_ptr->nam_sel                   = CM_NAM_NONE;
    cmd_info_ptr->packet_state              = CM_PACKET_STATE_NONE;

    #ifdef FEATURE_HDR_HYBRID
    cmd_info_ptr->hybr_pref                 = CM_HYBR_PREF_NO_CHANGE;
      /* hybrid preference */
    #else
    cmd_info_ptr->hybr_pref                 = CM_HYBR_PREF_NONE;
    #endif /* FEATURE_HDR_HYBRID */

    #ifdef CM_GW_SUPPORTED
    cmd_info_ptr->network_list_type         = CM_NETWORK_LIST_NONE;
    #endif /* CM_GW_SUPPORTED */

    /* Initialize command information fields.
    */

    /* Initialize the signal deltas
    */
    cmd_info_ptr->rssi_delta                = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->ecio_delta                = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->io_delta                  = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->sir_delta                 = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->pathloss_delta            = CM_SIG_STRENGTH_DELTA_NO_CHANGE;
    cmd_info_ptr->cqi_delta                 = CM_SIG_STRENGTH_DELTA_NO_CHANGE;

    cmd_info_ptr->answer_voice              = CM_ANSWER_VOICE_NONE;

    /* RTRE fields
    */
    cmd_info_ptr->rtre_config               = CM_RTRE_CONFIG_NONE;
    cmd_info_ptr->rtre_control              = CM_RTRE_CONTROL_NONE;

    #ifdef FEATURE_DDTM_CNTL
    cmd_info_ptr->ddtm_pref                 = CM_DDTM_PREF_NONE;
    cmd_info_ptr->ddtm_act_mask             = SYS_DDTM_ACT_MASK_EMPTY;
    cmd_info_ptr->ddtm_so_list_act          = CM_DDTM_SO_LIST_ACT_NONE;
    #endif

    cmd_info_ptr->cphs_allowed     = CM_CPHS_PROC_NONE;
    cmd_info_ptr->als_allowed      = CM_ALS_PROC_NONE;
    cmd_info_ptr->line             = CM_ALS_LINE_NONE;
    cmd_info_ptr->line_switching   = CM_ALS_LINE_SWITCHING_NONE;

    /* Pref slot fields
    */
    cmd_info_ptr->uim_status = FALSE;

    #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
        defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
    #error code not present
#endif

    cmd_info_ptr->network_type = CM_MODE_PREF_NONE;

    cmd_info_ptr->wakeup_service = CM_WAKEUP_NONE;

    /*
    ** cm_ph_cmd_subscription parameters
    */
    cmd_info_ptr->cdma_sub_avail_status     = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->cdma_sub_changed_status   = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->cdma_sub_not_avail_status = CM_SUBSCRIPTION_STATUS_NONE;


    cmd_info_ptr->gwl_sub_avail_status       = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->gwl_sub_changed_status     = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->gwl_sub_not_avail_status   = CM_SUBSCRIPTION_STATUS_NONE;


    cmd_info_ptr->gw2_sub_avail_status       = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->gw2_sub_changed_status     = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->gw2_sub_not_avail_status   = CM_SUBSCRIPTION_STATUS_NONE;


    cmd_info_ptr->gw3_sub_avail_status       = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->gw3_sub_changed_status     = CM_SUBSCRIPTION_STATUS_NONE;

    cmd_info_ptr->gw3_sub_not_avail_status   = CM_SUBSCRIPTION_STATUS_NONE;

    /* Initialize Daul Standby cmd Info
    */
    cmd_info_ptr->standby_pref        = SYS_MODEM_DS_PREF_NONE;
    cmd_info_ptr->active_subs         = SYS_MODEM_AS_ID_NO_CHANGE_MASK;
    cmd_info_ptr->cmd_subs_mask       = SYS_MODEM_AS_ID_NO_ACTIVE_MASK;
    cmd_info_ptr->default_voice_subs  = SYS_MODEM_AS_ID_NONE;
    cmd_info_ptr->default_data_subs   = SYS_MODEM_AS_ID_NONE;
    cmd_info_ptr->priority_subs       = SYS_MODEM_AS_ID_NONE;
    cmd_info_ptr->cmd_subs            = SYS_MODEM_AS_ID_NONE;
    cmd_info_ptr->ue_usage_setting    = SYS_UE_USAGE_SETTING_NONE;
    cmd_info_ptr->voice_domain_pref   = SYS_VOICE_DOMAIN_PREF_NONE;
    cmd_info_ptr->dds_switch_type     = DDS_CAUSE_NONE;
    /* Initialize rat_acq_order
    */
    cmd_info_ptr->rat_acq_order_pref.type = CM_ACQ_ORDER_TYPE_NONE;

    /* Initialize CSG Data */
    cmd_info_ptr->csg_id                  = SYS_CSG_ID_INVALID;
    cmd_info_ptr->csg_rat                 = SYS_RAT_NONE;
    cmd_info_ptr->wd_switch_on            = FALSE;


    cmd_info_ptr->lte_disable_cause       = CM_LTE_DISABLE_CAUSE_NONE;

    /*Initialize Block PLMN Data*/
    cmd_info_ptr->request_pref            = CM_PLMN_BLOCKING_PREF_NONE;
    cmd_info_ptr->plmn_block_cause        = SYS_BLOCK_PLMN_CAUSE_NONE;
    cmd_info_ptr->sub_not_avail_cause     = CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_NONE;
    #ifdef FEATURE_DDTM_CNTL
    /* Initialize blocked service option info*/
    cmd_info_ptr->ddtm_num_blocked_srv_opt = CM_INVALID_DDTM_NUM_SRV_OPT;
    memset(cmd_info_ptr->ddtm_blocked_srv_opt_list, 0, sizeof(cmd_info_ptr->ddtm_blocked_srv_opt_list));
    #endif
  }
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_cmd_alloc_ss_init

DESCRIPTION
  Allocate a CM command buffer and initialize its serving system command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_ss_cmd_s_type *cm_cmd_alloc_ss_init( void )
{
  cm_ss_cmd_s_type    *ss_cmd_ptr        = (cm_ss_cmd_s_type*)cm_cmd_alloc(sizeof(cm_ss_cmd_s_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the phone command fields.
  */

    /* Initialize command type field.
    */
  ss_cmd_ptr->cmd_type = CM_CMD_TYPE_SS;

    /* Initialize command header fields.
    */
    ss_cmd_ptr->client_id       = CM_CLIENT_ID_ANONYMOUS;
    ss_cmd_ptr->client_ptr      = CM_CLIENT_PTR_NONE;
    ss_cmd_ptr->cmd             = CM_SS_CMD_NONE;
    ss_cmd_ptr->cmd_subs        = SYS_MODEM_AS_ID_1 ;


  return ss_cmd_ptr;

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION cm_cmd_alloc_inband_init

DESCRIPTION
  Allocate a CM command buffer and initialize its inbnad command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static cm_inband_cmd_s_type *cm_cmd_alloc_inband_init( void )
{
  cm_inband_cmd_s_type      *inband_cmd_ptr        = (cm_inband_cmd_s_type*)cm_cmd_alloc(sizeof(cm_inband_cmd_s_type));
  cm_inband_cmd_info_s_type *cmd_info_ptr  = CMD_INFO_PTR( inband_cmd_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the inband command fields.
  */

    /* Initialize command type field.
    */
    inband_cmd_ptr->cmd_type = CM_CMD_TYPE_INBAND;


    /* Initialize command header fields.
    */
    inband_cmd_ptr->client_id       = CM_CLIENT_ID_ANONYMOUS;
    inband_cmd_ptr->client_ptr      = CM_CLIENT_PTR_NONE;
    inband_cmd_ptr->cmd             = CM_INBAND_CMD_NONE;

    /* Initialize command information fields.
    */
    cmd_info_ptr->cmd_err           = CM_INBAND_CMD_ERR_NOERR;

  return inband_cmd_ptr;

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_cmd_alloc_sms_init

DESCRIPTION
  Allocate a CM command buffer and initialize its SMS command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_sms_cmd_s_type *cm_cmd_alloc_sms_init( void )
{
  cm_sms_cmd_s_type  *sms_cmd_ptr        =
                          (cm_sms_cmd_s_type*)cm_cmd_alloc(sizeof(cm_sms_cmd_s_type));
  cm_sms_cmd_info_s_type    *cmd_info_ptr   = CMD_INFO_PTR( sms_cmd_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initialize command type field.
    */
  sms_cmd_ptr->cmd_type = CM_CMD_TYPE_SMS;


    /* Initialize command header fields.
    */
    sms_cmd_ptr->client_id       = CM_CLIENT_ID_ANONYMOUS;
    sms_cmd_ptr->client_ptr      = CM_CLIENT_PTR_NONE;
    sms_cmd_ptr->cmd             = CM_SMS_CMD_NONE;

    /* Initialize command information fields.
    */
    cmd_info_ptr->cmd_err        = CM_SMS_CMD_ERR_NOERR;


  return sms_cmd_ptr;

}


/*===========================================================================

FUNCTION cm_cmd_alloc_dbm_init

DESCRIPTION
  Obtains a CM command buffer from the free queue and initializes it for
  a DBM command.

DEPENDENCIES
  CM must be initialized.

RETURN VALUE
  Pointer to the buffer, NULL otherwise.

SIDE EFFECTS
  None

===========================================================================*/

static cm_dbm_cmd_s_type* cm_cmd_alloc_dbm_init( void )
{
  cm_dbm_cmd_s_type     *dbm_cmd_ptr        =
                          (cm_dbm_cmd_s_type*)cm_cmd_alloc(sizeof(cm_dbm_cmd_s_type));
  cm_dbm_cmd_info_s_type    *cmd_info_ptr  = CMD_INFO_PTR( dbm_cmd_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /*---------------------------------
      Initialize command type
     *------------------------------ */

  dbm_cmd_ptr->cmd_type = CM_CMD_TYPE_DBM;

    /*---------------------------------
      Now the header.
     *------------------------------ */

    dbm_cmd_ptr->client_id        = CM_CLIENT_ID_ANONYMOUS;
    dbm_cmd_ptr->client_ptr       = CM_CLIENT_PTR_NONE;
    dbm_cmd_ptr->cmd              = CM_DBM_CMD_NONE;
    dbm_cmd_ptr->cmd_err          = CM_DBM_CMD_ERR_NOERR;

    cmd_info_ptr->trans_pref      = CM_DBM_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return dbm_cmd_ptr;

}

#ifdef CM_GW_SUPPORTED


/*===========================================================================

FUNCTION cm_cmd_alloc_sups_init

DESCRIPTION
  Allocate a CM command buffer and initialize its sups command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_sups_cmd_s_type *cm_cmd_alloc_sups_init( void )
{
  cm_sups_cmd_s_type            *sups_cmd_ptr       =
                         (cm_sups_cmd_s_type*)cm_cmd_alloc(sizeof(cm_sups_cmd_s_type));
  cm_sups_cmd_info_s_type  *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the call command fields.
  */
    cmd_info_ptr = SUPS_CMD_INFO_PTR( sups_cmd_ptr );


    /* first initialize the entire buffer */
    memset(sups_cmd_ptr, CM_CALL_CMD_PARAM_DEFAULT_VALUE, sizeof( *sups_cmd_ptr ));

    /* Initialize command type field.
    */
  sups_cmd_ptr->cmd_type = CM_CMD_TYPE_SUPS;


    /* Initialize command header fields.
    */
    sups_cmd_ptr->client_id        = CM_CLIENT_ID_ANONYMOUS;
    sups_cmd_ptr->client_ptr       = CM_CLIENT_PTR_NONE;
    sups_cmd_ptr->cmd              = CM_SUPS_CMD_NONE;
    sups_cmd_ptr->is_parsing_required = FALSE;
    sups_cmd_ptr->is_modem_cc_api  = FALSE;

    cmd_info_ptr->cmd_err          = CM_SUPS_CMD_ERR_NOERR;
    cmd_info_ptr->cmd_err_cause    = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;
    cmd_info_ptr->ss_ref           = CM_SS_REF_UNASSIGNED;
    cmd_info_ptr->ss_code          = CM_SS_CODE_UNASSIGNED;
    cmd_info_ptr->ss_operation     = CM_SS_OPERATION_UNASSIGNED;
    cmd_info_ptr->invoke_id        = CM_SS_INVOKE_ID_UNASSIGNED;
    cmd_info_ptr->ccbs_indx        = CM_CCBS_INDX_INVALID;
    cmd_info_ptr->cmd_subs_id      = SYS_MODEM_AS_ID_NONE;

  return sups_cmd_ptr;

}

#endif /* CM_GW_SUPPORTED */

/*===========================================================================

FUNCTION cm_cmd_alloc_stats_init

DESCRIPTION
  Allocate a CM command buffer and initialize its stats command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_stats_cmd_s_type* cm_cmd_alloc_stats_init( void )
{
  cm_stats_cmd_s_type            *stats_cmd_ptr       =
     (cm_stats_cmd_s_type*)cm_cmd_alloc(sizeof(cm_stats_cmd_s_type));
  cm_stats_cmd_info_s_type *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command allocation is successful, initialize
  ** the stats command fields.
  */

    /* Fill the default values */
    stats_cmd_ptr->client_id      = CM_CLIENT_ID_ANONYMOUS;
    stats_cmd_ptr->client_ptr     = CM_CLIENT_PTR_NONE;
    stats_cmd_ptr->cmd            = CM_STATS_CMD_NONE;
  stats_cmd_ptr->cmd_type       = CM_CMD_TYPE_STATS;

    cmd_info_ptr = CMD_INFO_PTR (stats_cmd_ptr);
    cmd_info_ptr->bit_mask = 0x00;
    cmd_info_ptr->cmd_err  = CM_STATS_CMD_ERR_NONE;
    cmd_info_ptr->module   = SYS_MODEM_MODULE_NONE;

  return stats_cmd_ptr;

}

/*===========================================================================

FUNCTION cm_dbm_cmd_mo_msg

DESCRIPTION
  Send a data burst message. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_dbm_cmd_mo_msg (
    uint8                              burst_type,
        /* Burst type whether a PD ro SDB e.g. CAI_POSITION_DET etc */

    cm_dbm_cmd_cb_f_type               cmd_cb_func,
        /* client callback function */

    void                               *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type                  client_id,
        /* requesting client ID */

    void                              *payload_ptr,
        /* pointer to start of message */

    uint16                             num_bytes,
        /* Number of bytes in message */

    cm_dbm_payload_copy_cb_f_type      *payload_copy_func,
        /* Pointer to callback function to copy payload from client's
         * memory space to CM's memory space
        */

    uint8                              l2_ack_wait_time,
        /* Maximum number of seconds to wait for receiving L2 ack */

    uint8                              l3_ack_wait_time,
        /* Maximum number of seconds to wait for receiving L3 ack
         * before sending the pref sys change command
        */

    cm_dbm_trans_pref_e_type           trans_pref
        /* Transmission preference like REACH, ACH etc.. */

)
/*lint -esym(818, payload_ptr)
** Can't be const, would have to change cm_dbm_payload_copy_cb_f_type
** and that causes compiler errors in other subsystems
*/
/*lint -esym(715,trans_pref) */
/*lint -esym(715,payload_ptr) */
/*lint -esym(715,payload_copy_func) */
/*lint -esym(715,client_id) */
/*lint -esym(715,burst_type) */
/*lint -esym(715,l2_ack_wait_time) */
/*lint -esym(715,l3_ack_wait_time) */
/*lint -esym(715,num_bytes) */
{
  cm_dbm_cmd_s_type          *dbm_cmd_ptr;
  cm_dbm_cmd_info_s_type     *cmd_info_ptr;

  uint16 counter;
  int temp_num_bytes = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dbm_cmd_ptr = cm_cmd_alloc_dbm_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr    = CMD_INFO_PTR( dbm_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */

  dbm_cmd_ptr->burst_type         = burst_type;
  dbm_cmd_ptr->cmd_cb_func        = cmd_cb_func;
  dbm_cmd_ptr->data_block_ptr     = data_block_ptr;
  dbm_cmd_ptr->client_id          = client_id;
  dbm_cmd_ptr->cmd                = CM_DBM_CMD_SEND_MSG;
  dbm_cmd_ptr->l2_ack_wait_time   = l2_ack_wait_time;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->trans_pref     = trans_pref;
  cmd_info_ptr->num_bytes      = num_bytes;

  /* Check if the num_bytes is <= CM_DBM_MAX_PAYLOAD_SIZE
  ** If no, than a failure will eventually be reported back
  ** to the interested clients. Hence no need to copy the payload.
  ** Reject the command is num_bytes > 0 but payload is NULL
  */

  if(!INRANGE(num_bytes, 1, CM_DBM_MAX_PAYLOAD_SIZE ))
  {
    dbm_cmd_ptr->cmd_err = CM_DBM_CMD_ERR_PAYLOAD_SIZE_P;
  }
  else
  if(payload_ptr == NULL)
  {
    dbm_cmd_ptr->cmd_err = CM_DBM_CMD_ERR_PAYLOAD_NULL_PTR_P;
  }
  else
  {
    /*  The supplied payload is less than or equal to maximum
    **  size supported
    */
    temp_num_bytes = MIN(num_bytes, sizeof(cmd_info_ptr->payload));
    if (payload_copy_func != NULL)
    {
      /*  Use callback function provided to get the payload
      */
      if (payload_copy_func(&payload_ptr,
                            cmd_info_ptr->payload,
                            &temp_num_bytes))
      {
        /*  Some error occured during copying
        */
        dbm_cmd_ptr->cmd_err = CM_DBM_CMD_ERR_OTHER;
      }

    }
    else
    {
      for (counter = 0; counter < temp_num_bytes; counter++ )
      {
        cmd_info_ptr->payload[counter] = *((uint8 *)payload_ptr + counter);
      }
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)dbm_cmd_ptr );

  return TRUE;

}
/*lint +esym(715,trans_pref) */
/*lint +esym(715,payload_ptr) */
/*lint +esym(715,payload_copy_func) */
/*lint +esym(715,client_id) */
/*lint +esym(715,burst_type) */
/*lint +esym(715,l2_ack_wait_time) */
/*lint +esym(715,l3_ack_wait_time) */
/*lint +esym(715,num_bytes) */
/*lint +esym(818, payload_ptr) */



/*===========================================================================

FUNCTION cm_dbm_cmd_mo_msg2

DESCRIPTION
  Send a data burst message. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_dbm_cmd_mo_msg2 (
    uint8                              burst_type,
        /* Burst type whether a PD ro SDB e.g. CAI_POSITION_DET etc */

    cm_dbm_cmd_cb_f_type               cmd_cb_func,
        /* client callback function */

    void                              *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type                  client_id,
        /* requesting client ID */

    const cm_dbm_cmd_info_s_type       *dbm_cmd_info_ptr,
        /* DBM information pointer */

    uint8                              l2_ack_wait_time,
        /* Maximum number of seconds to wait for receiving L2 ack */

    uint8                              l3_ack_wait_time
        /* Maximum number of seconds to wait for receiving L3 ack
         * before sending the pref sys change command
        */
)
/*lint -esym(715,burst_type) */
/*lint -esym(715,data_block_ptr) */
/*lint -esym(715,client_id) */
/*lint -esym(715,dbm_cmd_info_ptr */
/*lint -esym(715,payload_copy_func) */
/*lint -esym(715,l2_ack_wait_time) */
/*lint -esym(715,l3_ack_wait_time) */
{
  cm_dbm_cmd_s_type          *dbm_cmd_ptr;
  cm_dbm_cmd_info_s_type     *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dbm_cmd_ptr = cm_cmd_alloc_dbm_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( dbm_cmd_info_ptr == NULL )
  {

    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_DBM_CMD_SEND_MSG, CM_DBM_CMD_ERR_PAYLOAD_NULL_PTR_P );
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the num_bytes is <= CM_DBM_MAX_PAYLOAD_SIZE
  ** If no, than a failure will eventually be reported back
  ** to the interested clients. Hence no need to copy the payload.
  ** Reject the command is num_bytes > 0 but payload is NULL
  */

  if(!INRANGE(dbm_cmd_info_ptr->num_bytes, 1, CM_DBM_MAX_PAYLOAD_SIZE ))
  {
     if ( cmd_cb_func != NULL )
     {
       cmd_cb_func( data_block_ptr,
                    CM_DBM_CMD_SEND_MSG, CM_DBM_CMD_ERR_PAYLOAD_SIZE_P);
     }

     return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* command buffer allocation is successful
  ** copy command information into command structure.
  */

  /* Get a reference to command information.
  */
  cmd_info_ptr    = CMD_INFO_PTR( dbm_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */

  dbm_cmd_ptr->burst_type         = burst_type;
  dbm_cmd_ptr->cmd_cb_func        = cmd_cb_func;
  dbm_cmd_ptr->data_block_ptr     = data_block_ptr;
  dbm_cmd_ptr->client_id          = client_id;
  dbm_cmd_ptr->cmd                = CM_DBM_CMD_SEND_MSG;
  dbm_cmd_ptr->l2_ack_wait_time   = l2_ack_wait_time;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  *cmd_info_ptr = *dbm_cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)dbm_cmd_ptr );

  return TRUE;

}
/*lint +esym(715,burst_type) */
/*lint +esym(715,data_block_ptr) */
/*lint +esym(715,client_id) */
/*lint +esym(715,dbm_cmd_info_ptr */
/*lint +esym(715,payload_copy_func) */
/*lint +esym(715,l2_ack_wait_time) */
/*lint +esym(715,l3_ack_wait_time) */



/*===========================================================================

FUNCTION cm_pd_cmd_session_start

DESCRIPTION
  Send a start session command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_pd_cmd_session_start(

  cm_pd_cmd_cb_f_type        cmd_cb_func,
      /* client callback function */

  void                      *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type          client,
      /* requesting client ID */

  cm_pd_session_type_e_type  session_type
      /* Type of session */

    )
/*lint -esym(715, client) */
/*lint -esym(715, session_type) */
{
  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE) )
  cm_pd_cmd_s_type           *pd_cmd_ptr;
  cm_pd_cmd_info_s_type      *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pd_cmd_ptr = cm_cmd_alloc_pd_init();

    /* Get a reference to command header
    ** and a reference to command information.
    */
    cmd_info_ptr   = CMD_INFO_PTR( pd_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    pd_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    pd_cmd_ptr->data_block_ptr  = data_block_ptr;
    pd_cmd_ptr->client_id       = client;
    pd_cmd_ptr->cmd             = CM_PD_CMD_SESSION_START;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */

    cmd_info_ptr->session_type      = session_type;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)pd_cmd_ptr );

  return TRUE;
  #else /* FEATURE_GPSONE_MSBASED */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PD_CMD_SESSION_START,
      CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_GPSONE_MSBASED */
}
/*lint +esym(715, client) */
/*lint +esym(715, session_type) */

/*===========================================================================

FUNCTION cm_pd_cmd_session_end

DESCRIPTION
  Send a end session command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_pd_cmd_session_end(

  cm_pd_cmd_cb_f_type        cmd_cb_func,
      /* client callback function */

  void                      *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type          client,
      /* requesting client ID */

  cm_pd_session_type_e_type  session_type
      /* type of session */

)
/*lint -esym(715, client) */
{
  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
  cm_pd_cmd_s_type           *pd_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(client);
  SYS_ARG_NOT_USED(session_type);

  pd_cmd_ptr = cm_cmd_alloc_pd_init();

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */

    pd_cmd_ptr->cmd_cb_func          = cmd_cb_func;
    pd_cmd_ptr->data_block_ptr       = data_block_ptr;
    pd_cmd_ptr->client_id            = client;
    pd_cmd_ptr->cmd                  = CM_PD_CMD_SESSION_END;
    pd_cmd_ptr->info.session_type    = session_type;
    pd_cmd_ptr->info.session_dormant = CM_PD_SESSION_DORMANT_NONE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)pd_cmd_ptr );

  return TRUE;
  #else /* FEATURE_GPSONE_MSBASED */

  SYS_ARG_NOT_USED(client);
  SYS_ARG_NOT_USED(session_type);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PD_CMD_SESSION_END,
      CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_GPSONE_MSBASED */
}
/*lint +esym(715, client) */

/*===========================================================================

FUNCTION cm_pd_cmd_session_dorm_update

DESCRIPTION
  Send a session dormancy command to CM. Places a request on the CM queue.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_pd_cmd_session_dorm_update(

  cm_pd_cmd_cb_f_type          cmd_cb_func,
      /* client callback function */

  void                        *data_block_ptr,
      /* pointer to client callback data block */

  cm_client_id_type            client,
      /* requesting client ID */

  cm_pd_session_type_e_type    session_type,
      /* type of session */

  cm_pd_session_dormant_e_type session_dormant
      /* The dormant status of the session */

)
{
  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
  cm_pd_cmd_s_type           *pd_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_1( "cm_pd_cmd_session_dorm_update() - client = %d", client);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(client);
  SYS_ARG_NOT_USED(session_type);
  SYS_ARG_NOT_USED(session_dormant);
  pd_cmd_ptr = cm_cmd_alloc_pd_init();
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    pd_cmd_ptr->cmd_cb_func          = cmd_cb_func;
    pd_cmd_ptr->data_block_ptr       = data_block_ptr;
    pd_cmd_ptr->client_id            = client;
    pd_cmd_ptr->cmd                  = CM_PD_CMD_SESSION_DORMANT;
    pd_cmd_ptr->info.session_type    = session_type;
    pd_cmd_ptr->info.session_dormant = session_dormant;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)pd_cmd_ptr );

  return TRUE;
  #else /* FEATURE_GPSONE_MSBASED */
  SYS_ARG_NOT_USED(client);
  SYS_ARG_NOT_USED(session_type);
  SYS_ARG_NOT_USED(session_dormant);
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PD_CMD_SESSION_DORMANT,
      CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_GPSONE_MSBASED */
}

#ifdef FEATURE_GPSONE
/*===========================================================================

FUNCTION cm_pd_reva_cmd_mo_msg

DESCRIPTION
  send the payload as a reva message. This function should be called by the
  client (in this case uapdms) to put the message on the cm command q.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_pd_cmd_reva_mo_msg(

    cm_pd_cmd_cb_f_type                 cmd_cb_func,
        /* client callback function */

    void                               *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type                   client,
        /* requesting client ID */

    const word                         *payload_ptr,
        /* pointer to start of message */

    uint16                              payload_size
        /* Number of bytes in message */
)
/*lint -esym(715,data_block_ptr)*/
/*lint -esym(715,client)*/
/*lint -esym(715,payload_ptr)*/
/*lint -esym(715,payload_size)*/
/*lint -esym(715,cmd_cb_func)*/
{
  #ifdef FEATURE_IS2000_REL_A

  cm_pd_cmd_s_type            *reva_cmd_ptr;
  cm_pd_cmd_reva_info_s_type  *cmd_info_ptr;
  int counter;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*at this point the cmd_ptr has be created but not initialized*/
  reva_cmd_ptr = cm_cmd_alloc_pd_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Since the command allocation was successful init the cmd_type       */
  reva_cmd_ptr->cmd_type = CM_CMD_TYPE_PD;

  CM_MSG_HIGH_1("cm_pd_cmd_reva_mo_msg() - reva_cmd set to %d", CM_PD_CMD_SEND_REVA);

  /* command buffer allocation is successful
  ** copy command information into command structure.
  */

  /* Get a reference to command information.
  */
  cmd_info_ptr     = &reva_cmd_ptr->revAinfo;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */

  reva_cmd_ptr->cmd                = CM_PD_CMD_SEND_REVA;
  reva_cmd_ptr->data_block_ptr     = data_block_ptr;
  reva_cmd_ptr->client_id          = client;
  reva_cmd_ptr->cmd_cb_func        = cmd_cb_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((payload_size == 0) || (payload_size > CM_BSSR_MAX_PILOTS))
  {
    reva_cmd_ptr->info.cmd_err = CM_PD_CMD_ERR_PAYLOAD_SIZE_P;
  }
  else
  if(payload_ptr == NULL)
  {
    reva_cmd_ptr->info.cmd_err = CM_PD_CMD_ERR_PAYLOAD_NULL_PTR_P;
  }
  else
  {
    /* Copy command information fields.
    */
    cmd_info_ptr->num_pilots = MIN(CM_BSSR_MAX_PILOTS, payload_size);
    for( counter=0; counter < cmd_info_ptr->num_pilots; counter++ )
    {
      CM_MSG_LOW_1("PN_offset=%d", payload_ptr[counter]);
      cmd_info_ptr->pilot_pn[counter] = payload_ptr[counter];
    }
  }

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)reva_cmd_ptr );
  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PD_CMD_SEND_REVA,
                 CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  #endif /* FEATURE_IS2000_REL_A*/
}
/*lint +esym(715,data_block_ptr)*/
/*lint +esym(715,client)*/
/*lint +esym(715,payload_ptr)*/
/*lint +esym(715,payload_size)*/
/*lint +esym(715,cmd_cb_func)*/

#endif /* FEATURE_GPSONE */

/**==========================================================================

@FUNCTION cm_mm_call_cmd_set_707b_flag

@DESCRIPTION
  Set the is_707b_ps_data flag for the call corresponding to call_id.

  As the access is restricted to single boolean, it is safe. In future if
  any other fields in the call object need to be modified, critical section
  protection might be needed.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  None

@SIDE_EFFECT
  None
@x
===========================================================================*/
static void                         cm_mm_call_cmd_set_707b_flag(

       cm_call_id_type              call_id,

       boolean                      flag_val
)
{
  cmcall_s_type               *call_ptr = cmcall_ptr(call_id);

  if(call_ptr != NULL)
  {
    /* Set the is_707b_ps_data flag */
    call_ptr->is_707b_ps_data = flag_val;
  }
}

/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_internal

@DESCRIPTION
  Internal function to command CM to originate a call - can be called by
  either of the 2 functions - cm_mm_call_cmd_orig_exten2() and
  cm_mm_call_cmd_orig_exten3

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Switch_in Call_id will be returned in switch_in_call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
static boolean cm_mm_call_cmd_orig_internal(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< Client callback function */

  cm_call_cmd_exten_cb_f_type   exten_cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  const cm_ip_orig_params_s_type  *ip_orig_params_ptr,
    /* pointer to IP mode specific data */

  cm_call_id_type                switch_out_call_id,
    /**< Switch_out_call_id */

  const cm_cc_generic_params_s_type   *cc_generic_params_ptr,
    /**< Call-control generic parameters */

  cm_orig_client_categ_e_type    orig_client_categ,
    /**< Initiating client category */

  boolean                     is_modem_cc_api,
   /* Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type       asubs_id,
  /* subs on which call needs to be originated*/

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
/*lint -esym(715, cdma_orig_params_ptr) */
/*lint -esym(715, gw_cs_orig_params_ptr) */
/*lint -esym(715, gw_ps_orig_params_ptr) */
/*lint -esym(715, ip_orig_params_ptr) */
/*lint -esym(715, asubs_id) */
{

  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_num_err_e_type             num_err;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_err_e_type        cmd_err = CM_CALL_CMD_ERR_NOERR;

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cmph_s_type                  *ph_ptr  = cmph_ptr ();
  #endif

  #ifdef CM_GW_SUPPORTED
  cm_num_buf_type                asciibuf;
  byte                           bcdbuf[CM_CALLED_PARTY_BCD_WITH_BCD_LEN];
  cm_call_cmd_info_gw_cs_s_type  *cs_cmd_mode_info_ptr;
  cm_call_cmd_info_gw_ps_s_type  *ps_cmd_mode_info_ptr;
  #endif /* CM_GW_SUPPORTED */

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cm_call_cmd_info_cdma_s_type   *cmd_info_cdma_ptr;
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("cm_mm_call_cmd_orig client %d, call type %d, as_id %d",
                orig_client_categ,call_type, asubs_id);

  /** Allocate and initialize call command buffer
  */
  call_cmd_ptr = cm_cmd_alloc_call_init();

  /** Command buffer allocation is successful
  */

  /**
  ** Copy command header fields.
  */
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->cmd_cc_cb_func  = exten_cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->client_categ    = orig_client_categ;
  call_cmd_ptr->cmd             = CM_CALL_CMD_ORIG;
  call_cmd_ptr->is_modem_cc_api = is_modem_cc_api ;
  call_cmd_ptr->direction       = CM_CALL_DIRECTION_MO;

  /**
  ** Check for any function parameter errors
  */
  num_err = cm_num_validate( called_number);
  if ( num_err != CM_NUM_ERR_NOERR )
  {
    switch ( num_err )
    {
      case CM_NUM_ERR_NULL_PTR :
        cmd_err = CM_CALL_CMD_ERR_NULL_NUM_P;
        break;

      case CM_NUM_ERR_INVALID_NUM_LENGTH :
        cmd_err = CM_CALL_CMD_ERR_NUM_LEN_P;
        break;

      default :
        cmd_err = CM_CALL_CMD_ERR_OTHER;
        break;
    } /**< switch ( num_err ) */
  }
  else if( return_call_id_ptr== NULL)
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_PARAM_P;
  }
  else if( alpha_buf == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_ALPHA_P;
  }
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  #ifdef FEATURE_IS2000_REL_A
  else if ( cdma_orig_params_ptr != NULL &&
            cdma_orig_params_ptr->qos_parms_incl &&
            cdma_orig_params_ptr->qos_parms_len > CM_ORIG_QOS_LEN_MAX )
  {

    cmd_err = CM_CALL_CMD_ERR_QOS_LEN_P;
  }
  #endif /* FEATURE_IS2000_REL_A */
  else if ( !BETWEEN(srv_type, CM_SRV_TYPE_NONE, CM_SRV_TYPE_MAX))
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_TYPE_P;
  }
  else if (srv_type == CM_SRV_TYPE_CDMA_HDR && cdma_orig_params_ptr == NULL)
  {
    cmd_err = CM_CALL_CMD_ERR_MISMATCH_P;
  }
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

  #ifdef CM_GW_SUPPORTED
  else if (srv_type == CM_SRV_TYPE_GSM_WCDMA &&
           gw_cs_orig_params_ptr == NULL && gw_ps_orig_params_ptr == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_MISMATCH_P;
  }
  else if ( srv_type == CM_SRV_TYPE_GSM || srv_type == CM_SRV_TYPE_WCDMA )
  {
    /**< The support for GSM and WCDMA seperate service types is not
    ** included in this build.
    ** This is **temporary** and it will be included later.
    */
    cmd_err = CM_CALL_CMD_ERR_SRV_TYPE_S;
  }
  #endif /* if CM_GW_SUPPORTED */

  /**
  ** If no function parameter errors, allocate call id.
  */
  else
  {
    if ((*return_call_id_ptr = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
    {
      cmd_err = CM_CALL_CMD_ERR_NO_CALL_ID_S;
    }
  }

  if(cc_generic_params_ptr == NULL)
  {
     CM_MSG_HIGH_0("CC Gen params ptr NULL");
     cmd_err = CM_CALL_CMD_ERR_MISMATCH_P;
  }

  /**
  ** If no parameter errors found, pack cmd info and queue it for CM,
  ** otherwise just queue it with command error
  */
  cmd_info_ptr          = CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr->cmd_err = cmd_err;

  if ( cmd_err == CM_CALL_CMD_ERR_NOERR )
  {
    /* Set the flag for IS-707B call and change the call type back to
    ** CM_CALL_TYPE_PS_DATA for normal operation
    */
    if( call_type == CM_CALL_TYPE_PS_DATA_IS707B )
    {
      call_type = CM_CALL_TYPE_PS_DATA;
      cm_mm_call_cmd_set_707b_flag(*return_call_id_ptr,TRUE);
    }

    /**<
    ** Copy command information fields.
    */
    cmd_info_ptr->call_id                   = *return_call_id_ptr;
    cmd_info_ptr->switch_out_call_id        = switch_out_call_id;

    cmd_info_ptr->call_type                 = call_type;
    cmd_info_ptr->srv_type                  = srv_type;
    call_cmd_ptr->cmd_subs                  = asubs_id;

    cmd_info_ptr->num                       = *called_number;
    if ( calling_number == NULL )
    {
       cmd_info_ptr->calling_num.len        = 0;
    }
    else
    {
       cmd_info_ptr->calling_num            = *calling_number;
    }

    CM_MSG_HIGH_1 ("PI value received in calling num  = %d",
                  cmd_info_ptr->calling_num.pi);

    /*lint -save -e668 */
    /* Copy the user-provided alpha_buf->buf to cmd_info_ptr->alpha.buf.
    ** Use the length passed in alpha_buf->length, but first check to be
    ** sure the passed length isn't longer than the size of the source
    ** buffer, to avoid a read overflow.
    */
    cmd_info_ptr->alpha.len = memscpy( cmd_info_ptr->alpha.buf,
                                       sizeof(cmd_info_ptr->alpha.buf),
                                       &(alpha_buf->buf),
                                       MIN(sizeof(alpha_buf->buf),
                                           alpha_buf->len) );
    /*lint -restore */

    /* Alpha Identifier is not really a gstk_string_type so we need
            to default to 8 bit DCS  
    */
  if(call_cmd_ptr->client_categ == CM_ORIG_CLIENT_CATEG_CARD)
  {
    cmd_info_ptr->alpha.dcs = CM_USSD_DCS_8_BIT;
    
    /* update dcs for unicode if required */
    if(cmd_info_ptr->alpha.len > 0)
    {
      if((cmd_info_ptr->alpha.buf[0] == 0x80) ||
         (cmd_info_ptr->alpha.buf[0] == 0x81) ||
         (cmd_info_ptr->alpha.buf[0] == 0x82)) 
      {
        /* unicode */
        cmd_info_ptr->alpha.dcs = CM_USSD_DCS_UCS2;
      }
    }

    CM_MSG_HIGH_2 ("alpha given, len = %d, dcs = %d",
                  cmd_info_ptr->alpha.len, cmd_info_ptr->alpha.dcs);\
  }
  
    CM_MSG_HIGH_1 ("Emerg serv categ given = %d",
                  called_number->emerg_srv_categ);

    if( cc_generic_params_ptr != NULL )
    {
      CM_MSG_HIGH_1 ("FDN check to do = %d",
                   cc_generic_params_ptr->is_fdn_to_check);
      cmd_info_ptr->cc_generic_params.is_fdn_to_check =
                                      cc_generic_params_ptr->is_fdn_to_check;
    }

    /**
    ** Copy mode-specific command information fields.
    */
    #ifdef CM_GW_SUPPORTED
    if( gw_ps_orig_params_ptr != NULL )
    {

      cmd_info_ptr->is_gsm_wcdma_ps_info_present = TRUE;

      ps_cmd_mode_info_ptr           = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr);
      ps_cmd_mode_info_ptr->primary_pdp= gw_ps_orig_params_ptr->primary_pdp;
      ps_cmd_mode_info_ptr->qos        = gw_ps_orig_params_ptr->qos;
      ps_cmd_mode_info_ptr->minimum_qos= gw_ps_orig_params_ptr->minimum_qos;
      ps_cmd_mode_info_ptr->pdp_addr   = gw_ps_orig_params_ptr->pdp_addr;


      /** From 9.5.1 AND 9.5.4  spec-24.008 qos field is mandatory
      ** and pdp_addr is mandatory for primary pdp
      */
      if ( !ps_cmd_mode_info_ptr->qos.valid )
      {
        CM_MSG_MED_0 ("Qos not valid, mandatory for pdp orig");
      }

      /** pdp_addr should be valid for primary pdp
      */
      if ( (ps_cmd_mode_info_ptr->primary_pdp)     &&
           (!ps_cmd_mode_info_ptr->pdp_addr.valid)
         )
      {
        CM_MSG_MED_0 ("pdp_addr not valid, mandatory for pdp orig");
      }

      ps_cmd_mode_info_ptr->apn_name           = gw_ps_orig_params_ptr->apn_name;
      ps_cmd_mode_info_ptr->config_options     = gw_ps_orig_params_ptr->config_options;
      ps_cmd_mode_info_ptr->h_comp             = gw_ps_orig_params_ptr->h_comp;
      ps_cmd_mode_info_ptr->d_comp             = gw_ps_orig_params_ptr->d_comp;
      ps_cmd_mode_info_ptr->tft                = gw_ps_orig_params_ptr->tft;

      ps_cmd_mode_info_ptr->pdp_group_id          =
                                gw_ps_orig_params_ptr->pdp_group_id;
      ps_cmd_mode_info_ptr->assoc_pri_profile_num =
                                gw_ps_orig_params_ptr->assoc_pri_profile_num;

      CM_MSG_HIGH_3 ("Primary pdp flag = %d, group id = %d, profile_num = %d",
                        gw_ps_orig_params_ptr->primary_pdp,
                        gw_ps_orig_params_ptr->pdp_group_id,
                        gw_ps_orig_params_ptr->assoc_pri_profile_num);


      ps_cmd_mode_info_ptr->profile_number     = gw_ps_orig_params_ptr->profile_number;

      ps_cmd_mode_info_ptr->request_type = gw_ps_orig_params_ptr->request_type;
    }

    if( gw_cs_orig_params_ptr != NULL )
    {
      CM_MSG_HIGH_1(" MO CTM : %d", gw_cs_orig_params_ptr->bearer_capability_1.ctm);

      cmd_info_ptr->is_gsm_wcdma_cs_info_present      = TRUE;

      cs_cmd_mode_info_ptr                            = WCALL_CMD_INFO_PTR( call_cmd_ptr );
      cs_cmd_mode_info_ptr->bc_repeat_indicator       = gw_cs_orig_params_ptr->bc_repeat_indicator;
      cs_cmd_mode_info_ptr->bearer_capability_1       = gw_cs_orig_params_ptr->bearer_capability_1;
      cs_cmd_mode_info_ptr->bearer_capability_2       = gw_cs_orig_params_ptr->bearer_capability_2;
      cs_cmd_mode_info_ptr->llc_repeat_indicator      = gw_cs_orig_params_ptr->llc_repeat_indicator;
      cs_cmd_mode_info_ptr->low_layer_compatibility_1 = gw_cs_orig_params_ptr->low_layer_compatibility_1;
      cs_cmd_mode_info_ptr->low_layer_compatibility_2 = gw_cs_orig_params_ptr->low_layer_compatibility_2;
      cs_cmd_mode_info_ptr->hlc_repeat_indicator      = gw_cs_orig_params_ptr->hlc_repeat_indicator;
      cs_cmd_mode_info_ptr->high_layer_compatibility_1= gw_cs_orig_params_ptr->high_layer_compatibility_1;
      cs_cmd_mode_info_ptr->high_layer_compatibility_2= gw_cs_orig_params_ptr->high_layer_compatibility_2;
      cs_cmd_mode_info_ptr->called_party_subaddress   = gw_cs_orig_params_ptr->called_party_subaddress;
      cs_cmd_mode_info_ptr->calling_party_subaddress  = gw_cs_orig_params_ptr->calling_party_subaddress;
      cs_cmd_mode_info_ptr->forward_cug_info          = gw_cs_orig_params_ptr->forward_cug_info;
      cs_cmd_mode_info_ptr->cm_clir                   = gw_cs_orig_params_ptr->cm_clir;
      if (gw_cs_orig_params_ptr->bearer_capability_2.present && !gw_cs_orig_params_ptr->bc_repeat_indicator.present)
      {
        CM_MSG_HIGH_1("bearer_capability_2 is present but repeat indicator is FALSE, setting it to TRUE, repeat_indi %d",
          gw_cs_orig_params_ptr->bc_repeat_indicator.repeat_indication);
        cs_cmd_mode_info_ptr->bc_repeat_indicator.present           = TRUE;
        cs_cmd_mode_info_ptr->bc_repeat_indicator.repeat_indication = REPEAT_SERVICE_CHANGE_FALLBACK;
      }

      #ifdef FEATURE_GSTK
      cs_cmd_mode_info_ptr->call_control_completed    = gw_cs_orig_params_ptr->call_control_completed;
      call_cmd_ptr->info.call_control_processing_complete = gw_cs_orig_params_ptr->call_control_completed;
      #endif

      #ifdef FEATURE_UUS
      cs_cmd_mode_info_ptr->mo_user_data              =
                                      gw_cs_orig_params_ptr->mo_user_data;
      #endif

      memset(cs_cmd_mode_info_ptr->called_party_bcd_number.data,
             0xff,
             sizeof(cs_cmd_mode_info_ptr->called_party_bcd_number.data));

      memset(asciibuf,0,sizeof(asciibuf));

      cm_util_ascii_num_to_bcd_num( called_number, bcdbuf, sizeof(bcdbuf) );

      if ( bcdbuf[0] != 0 )
      {
        cs_cmd_mode_info_ptr->called_party_bcd_number.length = bcdbuf[0];
        /**< length is in first byte
        */
        memscpy(cs_cmd_mode_info_ptr->called_party_bcd_number.data, sizeof(cs_cmd_mode_info_ptr->called_party_bcd_number.data), &bcdbuf[1], bcdbuf[0]);
        cs_cmd_mode_info_ptr->called_party_bcd_number.present = TRUE;
      }
      else
      {
        cs_cmd_mode_info_ptr->called_party_bcd_number.present = FALSE;
      }
    } /**< if( gw_cs_orig_params_ptr != NULL ) */
    #endif /* CM_GW_SUPPORTED */

    #if ( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) )
    if ( cdma_orig_params_ptr != NULL )
    {
      cmd_info_ptr->is_cdma_info_present = TRUE;

      cmd_info_cdma_ptr = CALL_CMD_MODE_INFO_PTR( call_cmd_ptr );

      #ifdef FEATURE_HDR_HANDOFF

      /** Catch invalid LADN parameters.
      ** Copy them only for data calls
      */
      if(( call_type == CM_CALL_TYPE_PS_DATA ) ||
         ( call_type == CM_CALL_TYPE_CS_DATA ))
      {
        cmd_info_cdma_ptr->last_act_data_net = cdma_orig_params_ptr->last_act_data_net;
      }
      else
      {
        /**< For all other call types LADN defaults to
        **  SYS_SYS_MODE_NO_SRV. For now, whine about it, fix it, let
        **  origination through
        */
        if( !BETWEEN(cdma_orig_params_ptr->last_act_data_net,
                    SYS_SYS_MODE_NONE,
                    SYS_SYS_MODE_MAX ))
        {
          CM_MSG_MED_2("Ignore LADN value %d for call type %d",
                       cdma_orig_params_ptr->last_act_data_net,
                       call_type);
        }
        cmd_info_cdma_ptr->last_act_data_net = SYS_SYS_MODE_NO_SRV;
      }
      #endif   /* FEATURE_HDR_HANDOFF */

      /** Only copy the service option parameter,
      ** if service type is CDMA specific or CDMA_HDR.
      */
      if( srv_type == CM_SRV_TYPE_CDMA_SPECIFIC ||
          srv_type == CM_SRV_TYPE_CDMA_HDR )
      {
        cmd_info_cdma_ptr->srv_opt = cdma_orig_params_ptr->srv_opt;
      } /**< if( srv_type == CM_SRV_TYPE_CDMA_SPECIFIC ) */

      #ifdef FEATURE_HDR
      else if ( (call_type == CM_CALL_TYPE_PS_DATA) &&
               (srv_type == CM_SRV_TYPE_AUTOMATIC )    )
      {
        cmd_info_cdma_ptr->srv_opt = cdma_orig_params_ptr->srv_opt;
      }
      #endif  /* FEATURE_HDR */

      #ifdef FEATURE_LTE_TO_1X
      else if ( (call_type == CM_CALL_TYPE_VOICE ||
                  call_type == CM_CALL_TYPE_EMERGENCY) &&
                 (srv_type == CM_SRV_TYPE_AUTOMATIC )
               )
      {
        cmd_info_cdma_ptr->srv_opt = cdma_orig_params_ptr->srv_opt;
      }
      #endif

      cmd_info_ptr->favored_hybr_pref = ph_ptr->main_stack_info.pref_info.hybr_pref;
      cmd_info_cdma_ptr->otasp_act_code = cdma_orig_params_ptr->activate_code;

      /* If the MO call is ps data over cdma, set the is_special_srv_data_call
      ** flag to indicate to 1x whether this is a special origination to
      ** clean up data session
      */
      if( call_type == CM_CALL_TYPE_PS_DATA  &&
          srv_type  == CM_SRV_TYPE_CDMA_SPECIFIC )
      {
        cmd_info_cdma_ptr->is_special_srv_data_call =
                              cdma_orig_params_ptr->is_special_srv_data_call;
        CM_MSG_HIGH_1("DS:special_srv %d",cdma_orig_params_ptr->is_special_srv_data_call);
      }
      else
      {
        cmd_info_cdma_ptr->is_special_srv_data_call = FALSE;
      }
      #ifdef FEATURE_IS2000

      /** Only data calls are allowed to set DRS bit in the
      ** origination command
      */
      if(( call_type == CM_CALL_TYPE_PS_DATA ) ||
         ( call_type == CM_CALL_TYPE_CS_DATA ))
      {
        cmd_info_cdma_ptr->drs_bit = cdma_orig_params_ptr->drs_bit;
      }
      else
      {
        /**< For all other call types DRS bit defaults to TRUE.
        */
        cmd_info_cdma_ptr->drs_bit = ph_ptr->voice_drs_bit;
      }
      #endif /* FEATURE_IS2000 */

      #ifdef FEATURE_IS2000_REL_A
      cmd_info_cdma_ptr->sr_id_included = cdma_orig_params_ptr->sr_id_included;
      cmd_info_cdma_ptr->sr_id          = cdma_orig_params_ptr->sr_id;
      cmd_info_cdma_ptr->qos_parms_incl = cdma_orig_params_ptr->qos_parms_incl;

      if ( cmd_info_cdma_ptr->qos_parms_incl )
      {
        cmd_info_cdma_ptr->qos_parms_len = memscpy( cmd_info_cdma_ptr->qos_parms,
                                                    sizeof( cmd_info_cdma_ptr->qos_parms ),
                                                    cdma_orig_params_ptr->qos_parms,
                                                    cdma_orig_params_ptr->qos_parms_len
                                                  );
      } /**< if ( cmd_info_ptr->qos_params_incl ) */

      #endif /* FEATURE_IS2000_REL_A */

    } /* if ( cdma_orig_params_ptr != NULL ) */
    #endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */
  } /* if (cmd_err == CM_CALL_CMD_ERR_NOERR) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IP_CALL
  if(ip_orig_params_ptr != NULL)
  {
    cmd_info_ptr->is_ip_call_info_present = TRUE;

    cmd_info_ptr->ip_call.ipcall_attrib_info = ip_orig_params_ptr->call_attrib;

    CM_MSG_HIGH_3("em_type %d, Audio %x, Video %d", ip_orig_params_ptr->call_attrib.em_type,
        ip_orig_params_ptr->call_attrib.audio_attrib,
                  ip_orig_params_ptr->call_attrib.video_attrib);

    CM_MSG_HIGH_1("is_secure_call %d", ip_orig_params_ptr->call_attrib.is_secure_call);

    if(cmd_info_ptr->ip_call.ipcall_attrib_info.ip_gen_param != NULL)
    {
      ref_cnt_obj_add_ref(cmd_info_ptr->ip_call.ipcall_attrib_info.ip_gen_param);
    }
  }
  #endif

  /** Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
  return TRUE;

} /* cm_mm_call_cmd_orig_internal() */
/*lint +esym(715, cdma_orig_params_ptr) */
/*lint +esym(715, gw_cs_orig_params_ptr) */
/*lint +esym(715, gw_ps_orig_params_ptr) */
/*lint +esym(715, ip_orig_params_ptr) */

/*===========================================================================
FUNCTION cm_mm_call_cmd_orig_per_subs

DESCRIPTION
  Command CM to originate a call.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_orig_per_subs
(

  cm_call_cmd_cb_f_type              cmd_cb_func,
    /* client callback function */

  void                              *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                  client_id,
    /* originating client */

  cm_call_type_e_type                call_type,
    /* call type - voice, data, etc. */

  cm_srv_type_e_type                 srv_type,
    /* service type - automatic, etc. */

  const sys_modem_as_id_e_type       asubs_id,
    /* Subscription-id on which the call is to be originated */

  const cm_num_s_type               *calling_number_ptr,
    /* Number of mobile that is originating the call  */

  const cm_num_s_type               *called_number_ptr,
    /* Number dialed - phone that is being called */

  const cm_orig_alpha_s_type        *alpha_buf,
    /* alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /* pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /* pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /* pointer to GW PS mode specific data */

  cm_call_id_type                   *return_call_id_ptr
    /* call ID will be allocated and returned */

)
/*lint -esym(818, return_call_id_ptr) */
{

  cm_cc_generic_params_s_type cc_generic_params;
  cc_generic_params.is_fdn_to_check = TRUE;

  return cm_mm_call_cmd_orig_internal(cmd_cb_func,
                                      NULL, /* New callback fn ptr NULL */
                                      data_block_ptr,
                                      client_id,
                                      call_type,
                                      srv_type,
                                      calling_number_ptr,
                                      called_number_ptr,
                                      alpha_buf,
                                      cdma_orig_params_ptr,
                                      gw_cs_orig_params_ptr,
                                      gw_ps_orig_params_ptr,
                                      NULL,
                                      CM_CALL_ID_INVALID,
                                      &cc_generic_params,
                                      CM_ORIG_CLIENT_CATEG_USER,
                                      FALSE, /* New CM API */
                                      asubs_id,
                                      return_call_id_ptr
                                     );

} /* cm_mm_call_cmd_orig_per_subs */
/*lint +esym(818, return_call_id_ptr) */

/*===========================================================================

FUNCTION cm_mm_call_cmd_orig

DESCRIPTION
  Command CM to originate a call.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_orig
(

  cm_call_cmd_cb_f_type              cmd_cb_func,
    /* client callback function */

  void                              *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                  client_id,
    /* originating client */

  cm_call_type_e_type                call_type,
    /* call type - voice, data, etc. */

  cm_srv_type_e_type                 srv_type,
    /* service type - automatic, etc. */

  const cm_num_s_type               *calling_number_ptr,
    /* Number of mobile that is originating the call  */

  const cm_num_s_type               *called_number_ptr,
    /* Number dialed - phone that is being called */

  const cm_orig_alpha_s_type        *alpha_buf,
    /* alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /* pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /* pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /* pointer to GW PS mode specific data */

  cm_call_id_type                   *return_call_id_ptr
    /* call ID will be allocated and returned */

)
{
    return (cm_mm_call_cmd_orig_exten(
                                    cmd_cb_func,
                                    /* client callback function */

                                        data_block_ptr,
                                    /* pointer to client callback data block */

                                        client_id,
                                    /* originating client */

                                        call_type,
                                    /* call type - voice, data, etc. */

                                        srv_type,
                                    /* service type - automatic, etc. */

                                        calling_number_ptr,
                                    /* Number of mobile that is originating
                                     * the call
                                     */

                                        called_number_ptr,
                                    /* Number dialed - phone that is being
                                    * called
                                    */

                                        alpha_buf,
                                    /* alpha tag to be displayed for the
                                    * origination
                                    */

                                        cdma_orig_params_ptr,
                                    /* pointer to cdma mode specific data */

                                        gw_cs_orig_params_ptr,
                                    /* pointer to GW CS mode specific data */

                                        gw_ps_orig_params_ptr,
                                    /* pointer to GW PS mode specific data */

                                        CM_CALL_ID_INVALID,
                                    /* Switch out call id*/

                                        return_call_id_ptr
                                    /* call ID will be allocated and returned */

                                    ));

 }


/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_exten

@DESCRIPTION
  Command CM to originate a handover call.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Switch_in Call_id will be returned in switch_in_call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_orig_exten(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  cm_call_id_type             switch_out_call_id,
    /**< Switch_out_call_id */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
{
    cm_cc_generic_params_s_type   cc_generic_params;
    cc_generic_params.is_fdn_to_check = TRUE;

    return (cm_mm_call_cmd_orig_exten2(
                                    cmd_cb_func,
                                    /* client callback function */

                                    data_block_ptr,
                                    /* pointer to client callback data block */

                                    client_id,
                                    /* originating client */

                                    call_type,
                                    /* call type - voice, data, etc. */

                                    srv_type,
                                    /* service type - automatic, etc. */

                                    calling_number,
                                    /* Number of mobile that is originating
                                     * the call
                                     */

                                    called_number,
                                    /* Number dialed - phone that is being
                                    * called
                                    */

                                    alpha_buf,
                                    /* alpha tag to be displayed for the
                                    * origination
                                    */

                                    cdma_orig_params_ptr,
                                    /* pointer to cdma mode specific data */

                                    gw_cs_orig_params_ptr,
                                    /* pointer to GW CS mode specific data */

                                    gw_ps_orig_params_ptr,
                                    /* pointer to GW PS mode specific data */

                                    switch_out_call_id,
                                    /* Switch out call id*/

                                    &cc_generic_params,

                                    return_call_id_ptr
                                    /* call ID will be allocated and returned */

                                    ));

 }

/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_exten2

@DESCRIPTION
  Command CM to originate a call with call-control generic parameters.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Switch_in Call_id will be returned in switch_in_call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_orig_exten2(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  cm_call_id_type             switch_out_call_id,
    /**< Switch_out_call_id */

  const cm_cc_generic_params_s_type   *cc_generic_params_ptr,
    /**< Call-control generic parameters */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
{

   return cm_mm_call_cmd_orig_internal(cmd_cb_func,
                                       NULL, /* New callback fn ptr NULL */
                                       data_block_ptr,
                                       client_id,
                                       call_type,
                                       srv_type,
                                       calling_number,
                                       called_number,
                                       alpha_buf,
                                       cdma_orig_params_ptr,
                                       gw_cs_orig_params_ptr,
                                       gw_ps_orig_params_ptr,
                                       NULL,
                                       switch_out_call_id,
                                       cc_generic_params_ptr,
                                       CM_ORIG_CLIENT_CATEG_USER,
                                       TRUE, /* New CM API */
                                       SYS_MODEM_AS_ID_NONE,
                                       return_call_id_ptr
                                      );

} /* cm_mm_call_cmd_orig_exten2() */


/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_exten3

@DESCRIPTION
  Command CM to originate a call with call-control parameters.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Switch_in Call_id will be returned in switch_in_call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_orig_exten3(

  cm_call_cmd_exten_cb_f_type   cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  cm_call_id_type             switch_out_call_id,
    /**< Switch_out_call_id */

  cm_orig_client_categ_e_type    orig_client_categ,
    /**< Initiating client category */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
{
   cm_cc_generic_params_s_type   cc_generic_params;
   cc_generic_params.is_fdn_to_check = TRUE;

   return cm_mm_call_cmd_orig_internal(NULL, /* Old callback fn ptr NULL */
                                       cmd_cb_func,
                                       data_block_ptr,
                                       client_id,
                                       call_type,
                                       srv_type,
                                       calling_number,
                                       called_number,
                                       alpha_buf,
                                       cdma_orig_params_ptr,
                                       gw_cs_orig_params_ptr,
                                       gw_ps_orig_params_ptr,
                                       NULL,
                                       switch_out_call_id,
                                       &cc_generic_params,
                                       orig_client_categ,
                                       TRUE, /* New CM API */
                                       SYS_MODEM_AS_ID_NONE,
                                       return_call_id_ptr
                                      );

} /* cm_mm_call_cmd_orig_exten3() */
/*===========================================================================

FUNCTION cm_mm_call_cmd_orig_exten4

DESCRIPTION
  Command CM to originate a call.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_orig_exten4
(

  cm_call_cmd_exten_cb_f_type        cmd_cb_func,
    /* client callback function */

  void                              *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                  client_id,
    /* originating client */

  cm_call_type_e_type                call_type,
    /* call type - voice, data, etc. */

  cm_srv_type_e_type                 srv_type,
    /* service type - automatic, etc. */

  const cm_num_s_type               *calling_number_ptr,
    /* Number of mobile that is originating the call  */

  const cm_num_s_type               *called_number_ptr,
    /* Number dialed - phone that is being called */

  const cm_orig_alpha_s_type        *alpha_buf,
    /* alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /* pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /* pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /* pointer to GW PS mode specific data */

  const cm_ip_orig_params_s_type  *ip_orig_params_ptr,
    /* pointer to IP mode specific data */

  cm_call_id_type                   *return_call_id_ptr
    /* call ID will be allocated and returned */

)
{
    cm_cc_generic_params_s_type   cc_generic_params;
    cc_generic_params.is_fdn_to_check = TRUE;

    return cm_mm_call_cmd_orig_internal(NULL,
                                       cmd_cb_func, /* New callback fn ptr NULL */
                                       data_block_ptr,
                                       client_id,
                                       call_type,
                                       srv_type,
                                       calling_number_ptr,
                                       called_number_ptr,
                                       alpha_buf,
                                       cdma_orig_params_ptr,
                                       gw_cs_orig_params_ptr,
                                       gw_ps_orig_params_ptr,
                                       ip_orig_params_ptr,
                                       CM_CALL_ID_INVALID,
                                       &cc_generic_params,
                                       CM_ORIG_CLIENT_CATEG_USER,
                                       TRUE, /* New CM API */
                                       SYS_MODEM_AS_ID_NONE,
                                       return_call_id_ptr
                                      );


 }

/*===========================================================================

FUNCTION cm_mm_call_cmd_orig_exten5_per_subs

DESCRIPTION
  Command CM to originate a call.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_orig_exten5_per_subs
(


  cm_call_cmd_exten_cb_f_type   exten_cmd_cb_func,
    /**< Client callback function */

  void                                              *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type                  client_id,
    /**< Originating client */

  cm_call_type_e_type               call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type                    srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type               *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type               *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type    *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  const cm_ip_orig_params_s_type    *ip_orig_params_ptr,
    /* pointer to IP mode specific data */

  cm_call_id_type                            switch_out_call_id,
    /**< Switch_out_call_id */

  const cm_cc_generic_params_s_type     *cc_generic_params_ptr,
    /**< Call-control generic parameters */

  cm_orig_client_categ_e_type    orig_client_categ,
    /**< Initiating client category */

  boolean                                       is_modem_cc_api,
   /* Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type             asubs_id,
  /* subs on which call needs to be originated*/

  cm_call_id_type                       *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
{

    return cm_mm_call_cmd_orig_internal(NULL,
                                       exten_cmd_cb_func, /* New callback fn ptr NULL */
                                       data_block_ptr,
                                       client_id,
                                       call_type,
                                       srv_type,
                                       calling_number,
                                       called_number,
                                       alpha_buf,
                                       cdma_orig_params_ptr,
                                       gw_cs_orig_params_ptr,
                                       gw_ps_orig_params_ptr,
                                       ip_orig_params_ptr,
                                       switch_out_call_id,
                                       cc_generic_params_ptr,
                                       orig_client_categ,
                                       TRUE, /* New CM API */
                                       asubs_id,
                                       return_call_id_ptr
                                      );


 }



/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_cc_per_subs_exten

@DESCRIPTION
  Command CM to originate a call with call-control parameters and subs-id
  on which call has to originate

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Switch_in Call_id will be returned in switch_in_call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_orig_cc_per_subs_exten(

  cm_call_cmd_exten_cb_f_type   cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  const cm_ip_orig_params_s_type  *ip_orig_params_ptr,
    /* pointer to IP mode specific data */

  cm_call_id_type             switch_out_call_id,
    /**< Switch_out_call_id */

  cm_orig_client_categ_e_type    orig_client_categ,
    /**< Initiating client category */

  const sys_modem_as_id_e_type       asubs_id,
    /**< Subscription-id on which the call is to be originated */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
/*lint -esym(715,asubs_id) */
{
   cm_cc_generic_params_s_type   cc_generic_params;
   cc_generic_params.is_fdn_to_check = TRUE;

   return cm_mm_call_cmd_orig_internal(NULL, /* Old callback fn ptr NULL */
                                       cmd_cb_func,
                                       data_block_ptr,
                                       client_id,
                                       call_type,
                                       srv_type,
                                       calling_number,
                                       called_number,
                                       alpha_buf,
                                       cdma_orig_params_ptr,
                                       gw_cs_orig_params_ptr,
                                       gw_ps_orig_params_ptr,
                                       ip_orig_params_ptr,
                                       switch_out_call_id,
                                       &cc_generic_params,
                                       orig_client_categ,
                                       TRUE, /* New CM API */
                                       asubs_id,
                                       return_call_id_ptr
                                      );

} /* cm_mm_call_cmd_orig_cc_per_subs_exten() */


/**==========================================================================

@FUNCTION cm_mm_call_cmd_orig_cc_per_subs

@DESCRIPTION
  Command CM to originate a call with call-control parameters and subs-id
  on which call has to originate

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request
  Switch_in Call_id will be returned in switch_in_call_id_ptr.

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_mm_call_cmd_orig_cc_per_subs(

  cm_call_cmd_exten_cb_f_type   cmd_cb_func,
    /**< Client callback function */

  void                        *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type           client_id,
    /**< Originating client */

  cm_call_type_e_type         call_type,
    /**< Call type - voice, data, etc. */

  cm_srv_type_e_type          srv_type,
    /**< Service type - automatic, etc. */

  const cm_num_s_type         *calling_number,
    /**< Number of mobile that is originating the call */

  const cm_num_s_type         *called_number,
    /**< Number dialed - phone that is being called */

  const cm_orig_alpha_s_type  *alpha_buf,
    /**< Alpha tag to be displayed for the origination */

  const cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
    /**< Pointer to cdma mode specific data */

  const cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
    /**< Pointer to GW CS mode specific data */

  const cm_gw_ps_orig_params_s_type *gw_ps_orig_params_ptr,
    /**< Pointer to GW PS mode specific data */

  cm_call_id_type             switch_out_call_id,
    /**< Switch_out_call_id */

  cm_orig_client_categ_e_type    orig_client_categ,
    /**< Initiating client category */

  const sys_modem_as_id_e_type       asubs_id,
    /**< Subscription-id on which the call is to be originated */

  cm_call_id_type             *return_call_id_ptr
    /**< Call ID will be allocated and returned */

)
/*lint -esym(715,asubs_id) */
{
   cm_cc_generic_params_s_type   cc_generic_params;
   cc_generic_params.is_fdn_to_check = TRUE;

   return cm_mm_call_cmd_orig_internal(NULL, /* Old callback fn ptr NULL */
                                       cmd_cb_func,
                                       data_block_ptr,
                                       client_id,
                                       call_type,
                                       srv_type,
                                       calling_number,
                                       called_number,
                                       alpha_buf,
                                       cdma_orig_params_ptr,
                                       gw_cs_orig_params_ptr,
                                       gw_ps_orig_params_ptr,
                                       NULL,
                                       switch_out_call_id,
                                       &cc_generic_params,
                                       orig_client_categ,
                                       TRUE, /* New CM API */
                                       asubs_id,
                                       return_call_id_ptr
                                      );

} /* cm_mm_call_cmd_orig_cc_per_subs() */
/*lint +esym(715,asubs_id) */

/*===========================================================================

FUNCTION cm_mm_call_cmd_answer

DESCRIPTION
  Command CM to answer an incoming call.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_answer(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* answering client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  const cm_ans_params_s_type   *ans_params_ptr
    /* pointer to mode specific data */

)
{

  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type *cmd_info_ptr;

  #ifdef CM_GW_SUPPORTED
  cm_call_cmd_info_gw_cs_s_type     *cmd_mode_info_ptr;
  cm_call_cmd_info_gw_ps_s_type     *ps_cmd_mode_info_ptr;
  #endif /* if CM_GW_SUPPORTED */


  if ( ans_params_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL ans_params ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_ANSWER, CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( ans_params_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Copy command header fields. */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_ANSWER;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );
    cmd_info_ptr->call_id         = call_id;
    cmd_info_ptr->info_type       = ans_params_ptr->info_type;

    switch( ans_params_ptr->info_type )
    {
    #ifdef CM_GW_SUPPORTED
    case CM_CALL_MODE_INFO_GW_CS:
    {
      cmd_mode_info_ptr = WCALL_CMD_INFO_PTR( call_cmd_ptr );
      cmd_mode_info_ptr->accept  =
        ans_params_ptr->ans_params.gw_cs_ans.accept;

      if (cmph_ptr()->qmss_enabled == TRUE)
      {
        cmd_mode_info_ptr->cc_cause.present = ans_params_ptr->ans_params.gw_cs_ans.reject_cause_present;
        cmd_mode_info_ptr->cc_cause.cause_value = ans_params_ptr->ans_params.gw_cs_ans.reject_cause;
      }

      cmd_info_ptr->call_type    =
        ans_params_ptr->ans_params.gw_cs_ans.call_type;
      if (ans_params_ptr->ans_params.gw_cs_ans.accept == FALSE)
      {
        cmd_info_ptr->end_status   = CM_CALL_END_INCOM_REJ;
      }
      break;
    }
    case CM_CALL_MODE_INFO_GW_PS:
      ps_cmd_mode_info_ptr = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr );

      ps_cmd_mode_info_ptr->accept  =
        ans_params_ptr->ans_params.gw_ps_ans.accept;

      if (ans_params_ptr->ans_params.gw_ps_ans.accept == FALSE)
      {
        cmd_info_ptr->end_status     = CM_CALL_END_INCOM_REJ;
      }

      cmd_info_ptr->call_type                  = CM_CALL_TYPE_PS_DATA;

      ps_cmd_mode_info_ptr->primary_pdp        =
                    ans_params_ptr->ans_params.gw_ps_ans.primary_pdp;
      ps_cmd_mode_info_ptr->qos                =
                    ans_params_ptr->ans_params.gw_ps_ans.qos;
      /* From 10.5.6.5 24.008 qos field is mandatory */
      ps_cmd_mode_info_ptr->qos.valid          = TRUE;

      ps_cmd_mode_info_ptr->minimum_qos        =
                    ans_params_ptr->ans_params.gw_ps_ans.minimum_qos;

      ps_cmd_mode_info_ptr->pdp_addr           =
                    ans_params_ptr->ans_params.gw_ps_ans.pdp_addr;
      /* From 10.5.6.5 24.008 pdp addr field is mandatory */
      ps_cmd_mode_info_ptr->pdp_addr.valid     = TRUE;

      ps_cmd_mode_info_ptr->apn_name           =
                    ans_params_ptr->ans_params.gw_ps_ans.apn_name;
      ps_cmd_mode_info_ptr->config_options     =
                    ans_params_ptr->ans_params.gw_ps_ans.config_options;
      ps_cmd_mode_info_ptr->h_comp             =
                    ans_params_ptr->ans_params.gw_ps_ans.h_comp;
      ps_cmd_mode_info_ptr->d_comp             =
                    ans_params_ptr->ans_params.gw_ps_ans.d_comp;
      ps_cmd_mode_info_ptr->tft                =
                    ans_params_ptr->ans_params.gw_ps_ans.tft;


      ps_cmd_mode_info_ptr->pdp_group_id          =
               ans_params_ptr->ans_params.gw_ps_ans.pdp_group_id;

      ps_cmd_mode_info_ptr->assoc_pri_profile_num =
               ans_params_ptr->ans_params.gw_ps_ans.assoc_pri_profile_num;

      CM_MSG_HIGH_3 ("CALL_CMD_ANSWER pdp flag %d, group id %d, prof_num %d",
                 ans_params_ptr->ans_params.gw_ps_ans.primary_pdp,
                 ans_params_ptr->ans_params.gw_ps_ans.pdp_group_id,
                 ans_params_ptr->ans_params.gw_ps_ans.assoc_pri_profile_num);


      ps_cmd_mode_info_ptr->profile_number           =
               ans_params_ptr->ans_params.gw_ps_ans.profile_number;

      break;

    #endif /* if CM_GW_SUPPORTED */

    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    case CM_CALL_MODE_INFO_CDMA:
      cmd_info_ptr->call_type = ans_params_ptr->ans_params.cdma_ans.call_type;
      break;
    #endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

    case CM_CALL_MODE_INFO_IP:
      cmd_info_ptr->call_type      = ans_params_ptr->ans_params.ip_ans.call_type;
      cmd_info_ptr->ip_call.accept = ans_params_ptr->ans_params.ip_ans.accept;
      cmd_info_ptr->ip_call.call_reject_reason = ans_params_ptr->ans_params.ip_ans.call_reject_reason;
      cmd_info_ptr->ip_call.sip_reject_code = ans_params_ptr->ans_params.ip_ans.sip_reject_code;
      cmd_info_ptr->ip_call.ipcall_attrib_info = ans_params_ptr->ans_params.ip_ans.call_attrib;
      if(cmd_info_ptr->ip_call.ipcall_attrib_info.ip_gen_param != NULL)
      {
        ref_cnt_obj_add_ref(cmd_info_ptr->ip_call.ipcall_attrib_info.ip_gen_param);
      }
      cmd_info_ptr->ip_call.pi = ans_params_ptr->ans_params.ip_ans.pi;
      CM_MSG_HIGH_1 ("CALL_CMD_ANSWER pi value %d", ans_params_ptr->ans_params.ip_ans.pi);
      break;

    default:
      CM_ERR_1( "Invalid info type %d",ans_params_ptr->info_type);
      cmd_info_ptr->cmd_err = CM_CALL_CMD_ERR_OTHER;
      break;
    } /* switch( ans_params_ptr->info_type ) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

} /* cm_mm_call_cmd_answer() */



/**===========================================================================

FUNCTION cm_mm_call_cmd_act_bearer_rsp

DESCRIPTION
  Command CM to respond the ACT_BEARER_CONTEXT_REQUEST. The call id passed
  has range from 0 to CM_CALL_ID_MAX. The client ID should be from a
  registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
   without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_act_bearer_rsp(

  cm_call_cmd_cb_f_type            cmd_cb_func,
    /**< client callback function */

  void                             *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                client_id,
    /**< answering client */

  cm_call_id_type                  call_id,
    /**< call ID of the call */

  const cm_act_bearer_rsp_s_type   *act_bearer_rsp_ptr
    /**< pointer to the response specific data */

)
{

  #ifdef FEATURE_CM_LTE
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;

  if ( act_bearer_rsp_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL act_bearer_rsp_ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_ACT_BEARER_RSP,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( ans_params_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

    /* Copy command header fields. */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_ACT_BEARER_RSP;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );
    cmd_info_ptr->call_id         = call_id;
    cmd_info_ptr->lte_info.is_accept = act_bearer_rsp_ptr->is_accept;
    cmd_info_ptr->lte_info.is_emergency_bearer = act_bearer_rsp_ptr->is_emergency_bearer;
    cmd_info_ptr->lte_info.as_id     = act_bearer_rsp_ptr->asubs_id;

    cmd_info_ptr->lte_info.esm_cause.valid =
      act_bearer_rsp_ptr->esm_cause.valid;
    if (act_bearer_rsp_ptr->esm_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_cause.esm_cause =
        act_bearer_rsp_ptr->esm_cause.esm_cause;
    }

    cmd_info_ptr->lte_info.esm_local_cause.valid =
      act_bearer_rsp_ptr->esm_local_cause.valid;
    if (act_bearer_rsp_ptr->esm_local_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_local_cause.local_cause =
        act_bearer_rsp_ptr->esm_local_cause.local_cause;
    }

    cmd_info_ptr->lte_info.config_options.valid =
      act_bearer_rsp_ptr->protocol_config_options.valid;
    cmltecall_copy_protocol_config_options( &(cmd_info_ptr->lte_info.config_options),
      &(act_bearer_rsp_ptr->protocol_config_options) );

    /* If accept the request, DS will fill in h_comp/d_comp.
    ** And in this case, need to pass to ESM.
    */
    if (act_bearer_rsp_ptr->is_accept)
    {
      cmd_info_ptr->lte_info.h_comp = act_bearer_rsp_ptr->h_comp;
      cmd_info_ptr->lte_info.d_comp = act_bearer_rsp_ptr->d_comp;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

  #else

  SYS_ARG_NOT_USED( act_bearer_rsp_ptr );
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( call_id );
  SYS_ARG_NOT_USED( client_id );

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_ACT_BEARER_RSP,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;
  #endif

} /* cm_mm_call_cmd_act_bearer_rsp() */



/**===========================================================================

FUNCTION cm_mm_call_cmd_pdn_connectivity_req

DESCRIPTION
  Command CM to request PDN connectivity. The client ID should be from a
  registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
   without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_pdn_connectivity_req(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< answering client */

  const cm_pdn_connectivity_s_type   *pdn_connectivity_ptr
    /**< pointer to the request specific data */
)
{
  #ifdef FEATURE_CM_LTE
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( pdn_connectivity_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL pdn_connectivity_ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDN_CONNECTIVITY_REQ,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( pdn_connectivity_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

    /* Copy command header fields. */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_PDN_CONNECTIVITY_REQ;
    call_cmd_ptr->cmd_subs        = pdn_connectivity_ptr->asubs_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );

    cmd_info_ptr->lte_info.sdf_id = pdn_connectivity_ptr->sdf_id;
    cmd_info_ptr->lte_info.request_type = pdn_connectivity_ptr->request_type;
    cmd_info_ptr->lte_info.as_id  = pdn_connectivity_ptr->asubs_id;

    *(&(cmd_info_ptr->lte_info.pdn_data)) = *(&(pdn_connectivity_ptr->pdn_data));


    CM_MSG_MED_1("cm_mm_call_cmd_pdn_connectivity_req(), lps = %d",\
                         pdn_connectivity_ptr->pdn_data.low_priority_signaling);
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

  #else
  SYS_ARG_NOT_USED( pdn_connectivity_ptr );
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDN_CONNECTIVITY_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;
  #endif

} /* cm_mm_call_cmd_pdn_connectivity_req() */



/**===========================================================================

FUNCTION cm_mm_call_cmd_pdn_connectivity_abort_req

DESCRIPTION
  Command CM to send PDN_CONNECTIVITY_ABORT_REQ to ESM. The client ID should
  be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
   without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_pdn_connectivity_abort_req(
  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< answering client */

  const cm_pdn_connectivity_abort_s_type   *pdn_connectivity_abort_ptr
    /**< pointer to the abort specific data */
)
{
  #ifdef FEATURE_CM_LTE
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( pdn_connectivity_abort_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL pdn_connectivity_abort_ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDN_CONNECTIVITY_ABORT_REQ,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( pdn_connectivity_abort_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();
    /* Copy command header fields. */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_PDN_CONNECTIVITY_ABORT_REQ;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );

    cmd_info_ptr->lte_info.sdf_id = pdn_connectivity_abort_ptr->sdf_id;
    cmd_info_ptr->lte_info.as_id  = pdn_connectivity_abort_ptr->asubs_id;

    cmd_info_ptr->lte_info.esm_cause.valid =
      pdn_connectivity_abort_ptr->esm_cause.valid;
    if (pdn_connectivity_abort_ptr->esm_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_cause.esm_cause =
        pdn_connectivity_abort_ptr->esm_cause.esm_cause;
    }

    cmd_info_ptr->lte_info.esm_local_cause.valid =
      pdn_connectivity_abort_ptr->esm_local_cause.valid;
    if (pdn_connectivity_abort_ptr->esm_local_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_local_cause.local_cause =
        pdn_connectivity_abort_ptr->esm_local_cause.local_cause;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

  #else
  SYS_ARG_NOT_USED( pdn_connectivity_abort_ptr );
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDN_CONNECTIVITY_ABORT_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;
  #endif

} /* cm_mm_call_cmd_pdn_connectivity_abort_req() */



/**===========================================================================

FUNCTION cm_mm_call_cmd_pdn_disconnect_req

DESCRIPTION
  Command CM to send PDN_DISCONNECT_REQ to ESM. The client ID should be
  from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
   without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_pdn_disconnect_req(
  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< answering client */

  const cm_pdn_disconnect_s_type   *pdn_disconnect_ptr
    /**< pointer to the disconnect specific data */
)
{
  #ifdef FEATURE_CM_LTE

  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( pdn_disconnect_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL pdn_disconnect_ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDN_DISCONNECT_REQ,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( pdn_disconnect_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();
    /* Copy command header fields. */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_PDN_DISCONNECT_REQ;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );

    cmd_info_ptr->lte_info.sdf_id = pdn_disconnect_ptr->sdf_id;
    cmd_info_ptr->lte_info.eps_bearer_id = pdn_disconnect_ptr->eps_bearer_id;
    cmd_info_ptr->lte_info.as_id  = pdn_disconnect_ptr->asubs_id;

    cmd_info_ptr->lte_info.config_options.valid =
      pdn_disconnect_ptr->protocol_config_options.valid;
    cmltecall_copy_protocol_config_options( &(cmd_info_ptr->lte_info.config_options),
      &(pdn_disconnect_ptr->protocol_config_options) );

    cmd_info_ptr->lte_info.esm_local_cause.valid =
      pdn_disconnect_ptr->esm_local_cause.valid;
    if (pdn_disconnect_ptr->esm_local_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_local_cause.local_cause =
        pdn_disconnect_ptr->esm_local_cause.local_cause;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

  #else
  SYS_ARG_NOT_USED( pdn_disconnect_ptr );
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDN_DISCONNECT_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;
  #endif

} /* cm_mm_call_cmd_pdn_disconnect_req() */



/**===========================================================================

FUNCTION cm_mm_call_cmd_res_alloc_req

DESCRIPTION
  Command CM to send RES_ALLOC_REQ to ESM. The client ID should be
  from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
   without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_res_alloc_req(
  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< answering client */

  const cm_res_alloc_s_type   *res_alloc_ptr
    /**< pointer to the request specific data */
)
{
  #ifdef FEATURE_CM_LTE

  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( res_alloc_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL res_alloc_ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_RES_ALLOC_REQ,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( res_alloc_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_RES_ALLOC_REQ;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );

    cmd_info_ptr->lte_info.sdf_id = res_alloc_ptr->sdf_id;
    cmd_info_ptr->lte_info.as_id  = res_alloc_ptr->asubs_id;

    cmd_info_ptr->lte_info.sdf_qos.valid = res_alloc_ptr->sdf_qos.valid;
    if (res_alloc_ptr->sdf_qos.valid)
    {
      *(&(cmd_info_ptr->lte_info.sdf_qos)) = *(&(res_alloc_ptr->sdf_qos));
    }

    cmd_info_ptr->lte_info.ul_dl_tft.valid = res_alloc_ptr->ul_dl_tft.valid;
    cmltecall_copy_tft_type( &(cmd_info_ptr->lte_info.ul_dl_tft),
                             &(res_alloc_ptr->ul_dl_tft));

    cmd_info_ptr->lte_info.config_options.valid =
      res_alloc_ptr->protocol_config_options.valid;
    cmltecall_copy_protocol_config_options( &(cmd_info_ptr->lte_info.config_options),
      &(res_alloc_ptr->protocol_config_options) );

    cmd_info_ptr->lte_info.lbi = res_alloc_ptr->lbi;

    cmd_info_ptr->lte_info.pdn_data.low_priority_signaling =
                              res_alloc_ptr->low_priority_signaling;

    CM_MSG_MED_1("cm_mm_call_cmd_res_alloc_req(), lps = %d", res_alloc_ptr->low_priority_signaling);
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

  #else
  SYS_ARG_NOT_USED( res_alloc_ptr );
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_RES_ALLOC_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;
  #endif

} /* cm_mm_call_cmd_res_alloc_req() */



/**===========================================================================

FUNCTION cm_mm_call_cmd_res_alloc_abort_req

DESCRIPTION
  Command CM to send RES_ALLOC_ABORT_REQ to ESM. The client ID should be
  from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
   without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_res_alloc_abort_req(
  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< answering client */

  const cm_res_alloc_abort_s_type   *res_alloc_abort_ptr
    /**< pointer to the abort specific data */
)
{
  #ifdef FEATURE_CM_LTE

  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( res_alloc_abort_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL res_alloc_abort_ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_RES_ALLOC_ABORT_REQ,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( res_alloc_abort_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Copy command header fields. */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_RES_ALLOC_ABORT_REQ;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );

    cmd_info_ptr->lte_info.sdf_id = res_alloc_abort_ptr->sdf_id;
    cmd_info_ptr->lte_info.as_id  = res_alloc_abort_ptr->asubs_id;

    cmd_info_ptr->lte_info.esm_cause.valid =
      res_alloc_abort_ptr->esm_cause.valid;
    if (res_alloc_abort_ptr->esm_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_cause.esm_cause =
        res_alloc_abort_ptr->esm_cause.esm_cause;
    }

    cmd_info_ptr->lte_info.esm_local_cause.valid =
      res_alloc_abort_ptr->esm_local_cause.valid;
    if (res_alloc_abort_ptr->esm_local_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_local_cause.local_cause =
        res_alloc_abort_ptr->esm_local_cause.local_cause;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

  #else
  SYS_ARG_NOT_USED( res_alloc_abort_ptr );
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_RES_ALLOC_ABORT_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;
  #endif

} /* cm_mm_call_cmd_res_alloc_abort_req() */



/**===========================================================================

FUNCTION cm_mm_call_cmd_bearer_res_mod_req

DESCRIPTION
  Command CM to send RES_MOD_REQ to ESM. The client ID should be
  from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
   without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_bearer_res_mod_req(
  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< answering client */

  const cm_res_mod_s_type   *res_mod_ptr
    /**< pointer to the modification specific data */
)
{
  #ifdef FEATURE_CM_LTE

  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( res_mod_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL res_mod_ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_BEARER_RES_MOD_REQ,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( res_mod_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Copy command header fields. */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_BEARER_RES_MOD_REQ;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );

    cmd_info_ptr->lte_info.sdf_id = res_mod_ptr->sdf_id;
    cmd_info_ptr->lte_info.lbi = res_mod_ptr->lbi;
    cmd_info_ptr->lte_info.as_id  = res_mod_ptr->asubs_id;

    cmd_info_ptr->lte_info.sdf_qos.valid = res_mod_ptr->sdf_qos.valid;
    if (res_mod_ptr->sdf_qos.valid)
    {
      *(&(cmd_info_ptr->lte_info.sdf_qos)) = *(&(res_mod_ptr->sdf_qos));
    }

    cmd_info_ptr->lte_info.ul_dl_tft.valid = res_mod_ptr->ul_dl_tft.valid;
    cmltecall_copy_tft_type( &(cmd_info_ptr->lte_info.ul_dl_tft),
                             &(res_mod_ptr->ul_dl_tft));

    cmd_info_ptr->lte_info.esm_cause.valid =
      res_mod_ptr->esm_cause.valid;
    if (res_mod_ptr->esm_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_cause.esm_cause =
        res_mod_ptr->esm_cause.esm_cause;
    }

    cmd_info_ptr->lte_info.esm_local_cause.valid =
      res_mod_ptr->esm_local_cause.valid;
    if (res_mod_ptr->esm_local_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_local_cause.local_cause =
        res_mod_ptr->esm_local_cause.local_cause;
    }

    cmd_info_ptr->lte_info.config_options.valid =
      res_mod_ptr->protocol_config_options.valid;
    cmltecall_copy_protocol_config_options( &(cmd_info_ptr->lte_info.config_options),
      &(res_mod_ptr->protocol_config_options) );

    cmd_info_ptr->lte_info.pdn_data.low_priority_signaling =
                                res_mod_ptr->low_priority_signaling;

    CM_MSG_MED_1("cm_mm_call_cmd_bearer_res_mod_req(), lps = %d", res_mod_ptr->low_priority_signaling);
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

  #else
  SYS_ARG_NOT_USED( res_mod_ptr );
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_BEARER_RES_MOD_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;
  #endif

} /* cm_mm_call_cmd_bearer_res_mod_req() */



/**===========================================================================

FUNCTION cm_mm_call_cmd_mod_bearer_rsp

DESCRIPTION
  Command CM to send MOD_BEARER_ACCEPT or MOD_BEARER_REJ to ESM. The client
  ID should be from a registered CM client.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
  The error code is passed back to the client through the client-provided,
  command-callback function, cmd_cb_func. If the command request is processed
   without errors, CM_CALL_CMD_ERR_NOERR is passed back to the client.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_mod_bearer_rsp(
  cm_call_cmd_cb_f_type  cmd_cb_func,
    /**< client callback function */

  void                   *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type      client_id,
    /**< answering client */

  cm_call_id_type        call_id,
    /**< call ID of the call */

  const cm_mod_bearer_rsp_s_type   *mod_bearer_rsp_ptr
    /**< pointer to the response specific data */
)
{
  #ifdef FEATURE_CM_LTE

  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( mod_bearer_rsp_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      CM_ERR_0("ERROR: NULL mod_bearer_rsp_ptr");
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_MOD_BEARER_RSP,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  } /* if( mod_bearer_rsp_ptr == NULL ) */

  call_cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

    /* Copy command header fields. */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_MOD_BEARER_RSP;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */
    cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );

    cmd_info_ptr->call_id                = call_id;
    cmd_info_ptr->lte_info.eps_bearer_id = mod_bearer_rsp_ptr->eps_bearer_id;
    cmd_info_ptr->lte_info.as_id         = mod_bearer_rsp_ptr->asubs_id;

    cmd_info_ptr->lte_info.esm_cause.valid =
      mod_bearer_rsp_ptr->esm_cause.valid;
    if (mod_bearer_rsp_ptr->esm_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_cause.esm_cause =
        mod_bearer_rsp_ptr->esm_cause.esm_cause;
    }

    cmd_info_ptr->lte_info.esm_local_cause.valid =
      mod_bearer_rsp_ptr->esm_local_cause.valid;
    if (mod_bearer_rsp_ptr->esm_local_cause.valid)
    {
      cmd_info_ptr->lte_info.esm_local_cause.local_cause =
        mod_bearer_rsp_ptr->esm_local_cause.local_cause;
    }

    cmd_info_ptr->lte_info.config_options.valid =
      mod_bearer_rsp_ptr->protocol_config_options.valid;
    cmltecall_copy_protocol_config_options( &(cmd_info_ptr->lte_info.config_options),
      &(mod_bearer_rsp_ptr->protocol_config_options) );

    cmd_info_ptr->lte_info.is_accept = mod_bearer_rsp_ptr->is_accept;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;

  #else
  SYS_ARG_NOT_USED( mod_bearer_rsp_ptr );
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( call_id );

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MOD_BEARER_RSP,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;
  #endif

} /* cm_mm_call_cmd_mod_bearer_rsp() */



/*===========================================================================

FUNCTION cm_mm_call_cmd_setup_res

DESCRIPTION
  Command CM to send a setup message response.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_setup_res(

  cm_call_cmd_cb_f_type               cmd_cb_func,
    /* client callback function */

  void                               *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                   client_id,
    /* answering client */

  cm_call_id_type                     call_id,
    /* call ID of the call */

  const cm_setup_res_params_s_type   *setup_params_ptr
    /* pointer to mode specific data */

)
/*lint -esym(715,setup_params_ptr) */
/*lint -esym(715,call_id) */
/*lint -esym(715,client_id) */
{

  cm_call_cmd_s_type                *call_cmd_ptr;
  cm_call_cmd_info_s_type           *cmd_info_ptr;
  #ifdef CM_GW_SUPPORTED
  cm_call_cmd_info_gw_cs_s_type     *cmd_mode_info_ptr;
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( setup_params_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_SETUP_RES, CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }

  CM_MSG_HIGH_1("Proc setup response CTM: %d ",setup_params_ptr->bearer_capability_1.ctm);

  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Get a reference to command header
    ** and a reference to command information.
    */
    cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );



    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef CM_GW_SUPPORTED
    cmd_mode_info_ptr                = WCALL_CMD_INFO_PTR( call_cmd_ptr );

    cmd_info_ptr->info_type                = CM_CALL_MODE_INFO_GW_CS;

    cmd_mode_info_ptr->accept              = setup_params_ptr->accept;
    cmd_mode_info_ptr->cc_cause            = setup_params_ptr->cc_cause;
    cmd_mode_info_ptr->bc_repeat_indicator = setup_params_ptr->bc_repeat_indicator;
    cmd_mode_info_ptr->bearer_capability_1 = setup_params_ptr->bearer_capability_1;
    cmd_mode_info_ptr->bearer_capability_2 = setup_params_ptr->bearer_capability_2;

    /* Set up response to MN requires just BC  but CM also copies LLC and
    ** HLC so that this information can be used for in-call modification
    ** after call gets connected. Example - For VT setup DS sends setup res
    ** as voice. LLC for VT needs to be stored in llc2 so that it can be
    ** used later for in-call modification to VT.
    */

    /* Copy BC */
    cmd_mode_info_ptr->bc_repeat_indicator = setup_params_ptr->bc_repeat_indicator;
    cmd_mode_info_ptr->bearer_capability_1 = setup_params_ptr->bearer_capability_1;
    cmd_mode_info_ptr->bearer_capability_2 = setup_params_ptr->bearer_capability_2;

    /* Copy LLC */
    cmd_mode_info_ptr->llc_repeat_indicator = setup_params_ptr->llc_repeat_indicator;
    cmd_mode_info_ptr->low_layer_compatibility_1 = setup_params_ptr->low_layer_compatibility_1;
    cmd_mode_info_ptr->low_layer_compatibility_2 = setup_params_ptr->low_layer_compatibility_2;

    /* Copy HLC */
    cmd_mode_info_ptr->hlc_repeat_indicator = setup_params_ptr->hlc_repeat_indicator;
    cmd_mode_info_ptr->high_layer_compatibility_1 = setup_params_ptr->high_layer_compatibility_1;
    cmd_mode_info_ptr->high_layer_compatibility_2 = setup_params_ptr->high_layer_compatibility_2;
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_SETUP_RES;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->call_id                  = call_id;


    if (setup_params_ptr->accept == FALSE)
    {
      cmd_info_ptr->end_status             = CM_CALL_END_SETUP_REJ;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined(FEATURE_IP_CALL)
    /* Copy accept to ip accept flag
    */
    cmd_info_ptr->ip_call.accept           = setup_params_ptr->accept;
    #endif


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
    return TRUE;

  }
/*lint +esym(715,setup_params_ptr) */
/*lint +esym(715,call_id) */
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_pdp_act_res

DESCRIPTION
  Response to an Incoming PDP activation request.(GSM/WCDMA PS calls)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_pdp_act_res(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id,
    /* answering client */

  cm_call_id_type                call_id,
    /* call ID of the call */

  const cm_pdp_act_res_params_s_type   *act_res_params_ptr
    /* pointer to mode specific data */

)
/*lint -esym(715,act_res_params_ptr) */
/*lint -esym(715,call_id) */
/*lint -esym(715,client_id) */
{
  #ifdef CM_GW_SUPPORTED
  cm_call_cmd_s_type                   *call_cmd_ptr;
  cm_call_cmd_info_s_type         *cmd_info_ptr;
  cm_call_cmd_info_gw_ps_s_type          *cmd_mode_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( act_res_params_ptr == NULL)
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDP_ACT_RES,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }

  /* Only care if it was rejected */
  if (act_res_params_ptr->accept == FALSE)
  {
    call_cmd_ptr = cm_cmd_alloc_call_init();

      /* Get a reference to command header
      ** and a reference to command information.
      */
      cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );
      cmd_mode_info_ptr  = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr );


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields. */
      call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      call_cmd_ptr->data_block_ptr  = data_block_ptr;
      call_cmd_ptr->client_id       = client_id;
      call_cmd_ptr->cmd             = CM_CALL_CMD_PDP_ACT_RES;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */
      cmd_info_ptr->call_id         = call_id;
      cmd_info_ptr->info_type       = CM_CALL_MODE_INFO_GW_PS;
      cmd_mode_info_ptr->accept     = act_res_params_ptr->accept;
      cmd_mode_info_ptr->cause      = act_res_params_ptr->cause;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the commands to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;
  } /* if (act_res_params_ptr->accept == FALSE) */

  else
  {
    return TRUE;
  }
  #else /* CM_GW_SUPPORTED */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDP_ACT_RES,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of CM_GW_SUPPORTED */
}
/*lint +esym(715,act_res_params_ptr) */
/*lint +esym(715,call_id) */
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_rab_reestablish

DESCRIPTION
  Command CM to send a RAB reestablish request.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_call_cmd_rab_reestablish(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* answering client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  const cm_rab_reestab_params_s_type   *reestab_params_ptr
    /* pointer to mode specific data */

)
/*lint -esym(715,call_id) */
/*lint -esym(715,client_id) */
/*lint -esym(715,reestab_params_ptr) */
{
  #if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
  cm_call_cmd_s_type                   *call_cmd_ptr;
  cm_call_cmd_info_s_type              *cmd_info_ptr;
  cm_call_cmd_info_gw_ps_s_type        *cmd_mode_info_ptr;



  if( reestab_params_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_RAB_REESTAB,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Get a reference to command header
    ** and a reference to command information.
    */
    cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_RAB_REESTAB;

    cmd_info_ptr->call_id         = call_id;
    cmd_info_ptr->call_type       = CM_CALL_TYPE_PS_DATA;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (reestab_params_ptr->sys_mode != SYS_SYS_MODE_LTE)
    {

      /* Get a reference to command header
      ** and a reference to command information.
      */
      cmd_mode_info_ptr = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Copy command information fields.   */

      cmd_info_ptr->info_type       = CM_CALL_MODE_INFO_GW_PS;
      cmd_mode_info_ptr->rabid      =
      reestab_params_ptr->rab_reestab_params.cm_wcdma_rab_reestab_params.rabid;

    }
    else
    {
      #ifdef FEATURE_CM_LTE
      /* System mode is LTE  */
      cmd_info_ptr->info_type       = CM_CALL_MODE_INFO_LTE;
      cmd_info_ptr->lte_info.eps_bearer_id =
      reestab_params_ptr->rab_reestab_params.cm_lte_rab_reestab_params.eps_bearer_id;
      cmd_info_ptr->lte_info.sys_lte_reestab_calltype =
              reestab_params_ptr->sys_lte_reestab_calltype;
      CM_MSG_HIGH_1( "sys_lte_reestab_calltype = %d", reestab_params_ptr->sys_lte_reestab_calltype);
      #endif

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );


  return TRUE;
  #else /* (CM_GW_SUPPORTED  || defined(FEATURE_CM_LTE)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_RAB_REESTAB,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (CM_GW_SUPPORTED  || defined(FEATURE_CM_LTE)) */
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*lint +esym(715,call_id) */
/*lint +esym(715,client_id) */
/*lint +esym(715,reestab_params_ptr) */



/*===========================================================================

FUNCTION cm_mm_call_cmd_pdp_modify

DESCRIPTION
  Command CM to request a PDP modify command.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_pdp_modify(

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* answering client */

  cm_call_id_type               call_id,
    /* call ID of the call */

  const cm_pdp_modify_params_s_type   *modify_params_ptr
    /* pointer to command data */

)
/*lint -esym(715,modify_params_ptr) */
/*lint -esym(715,client_id) */
/*lint -esym(715,call_id) */
{
  #ifdef CM_GW_SUPPORTED
  cm_call_cmd_s_type                  *call_cmd_ptr;
  cm_call_cmd_info_s_type        *cmd_info_ptr;
  cm_call_cmd_info_gw_ps_s_type         *cmd_mode_info_ptr;


  if( modify_params_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDP_MODIFY, CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Get a reference to command header
    ** and a reference to command information.
    */
    cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );
    cmd_mode_info_ptr = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_PDP_MODIFY;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->call_id         = call_id;
    cmd_info_ptr->call_type       = CM_CALL_TYPE_PS_DATA;
    cmd_info_ptr->info_type       = CM_CALL_MODE_INFO_GW_PS;
    cmd_mode_info_ptr->qos        = modify_params_ptr->qos;
    cmd_mode_info_ptr->tft        = modify_params_ptr->tft;
    cmd_mode_info_ptr->minimum_qos = modify_params_ptr->minimum_qos;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

  return TRUE;
  #else /* CM_GW_SUPPORTED */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PDP_MODIFY,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of CM_GW_SUPPORTED */
}
/*lint +esym(715,modify_params_ptr) */
/*lint +esym(715,client_id) */
/*lint +esym(715,call_id) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_pdp_nw_modify_res

DESCRIPTION
  Command CM to response to a network inititated PDP modify request

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_pdp_nw_modify_res(

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* answering client */

  cm_call_id_type               call_id,
    /* call ID of the call */

  const cm_pdp_act_res_params_s_type   *act_res_params_ptr
    /* pointer to command data */

)
/*lint -esym(715,act_res_params_ptr) */
/*lint -esym(715,client_id) */
/*lint -esym(715,call_id) */
{
  #if ( defined CM_GW_SUPPORTED )
  cm_call_cmd_s_type                  *call_cmd_ptr;
  cm_call_cmd_info_s_type             *cmd_info_ptr;
  cm_call_cmd_info_gw_ps_s_type       *cmd_mode_info_ptr;

  if( act_res_params_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                   CM_CALL_CMD_PDP_NW_MODIFY_RSP,
                   CM_CALL_CMD_ERR_NULL_PARAM_P);
    }
    return FALSE;
  }


  call_cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
    /* Get a reference to command header
    ** and a reference to command information.
    */
    cmd_info_ptr           = CMD_INFO_PTR( call_cmd_ptr );
    cmd_mode_info_ptr      = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_PDP_NW_MODIFY_RSP;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->call_id      = call_id;
    cmd_info_ptr->call_type    = CM_CALL_TYPE_PS_DATA;
    cmd_info_ptr->info_type    = CM_CALL_MODE_INFO_GW_PS;
    cmd_mode_info_ptr->accept  = act_res_params_ptr->accept;

    if( !act_res_params_ptr->accept )
    {
      cmd_mode_info_ptr->cause  = act_res_params_ptr->cause;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
  return TRUE;
  #else /* #if(CM_GW_SUPPORTED */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_CALL_CMD_PDP_NW_MODIFY_RSP,
                 CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* end of #if(CM_GW_SUPPORTED */
}
/*lint +esym(715,act_res_params_ptr) */
/*lint +esym(715,client_id) */
/*lint +esym(715,call_id) */


/*===========================================================================

FUNCTION cm_mm_ss_cmd_ps_data_available_per_subs

DESCRIPTION
  Inform CM that data is available for transmission.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_ss_cmd_ps_data_available_per_subs(

  cm_ss_cmd_cb_f_type            cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id,
    /* requesting client ID */

  sys_modem_as_id_e_type         asubs_id

)
/*lint -esym(715,client_id) */
{
  #if ( defined CM_GW_SUPPORTED && (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)))
  cm_ss_cmd_s_type                *ss_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("CM_SS_CMD_PS_DATA_AVAIL client %d, as_id %d",
                client_id, asubs_id);

  ss_cmd_ptr = cm_cmd_alloc_ss_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */


    /* Copy command header fields. */

    ss_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ss_cmd_ptr->data_block_ptr  = data_block_ptr;
    ss_cmd_ptr->client_id       = client_id;
    ss_cmd_ptr->cmd             = CM_SS_CMD_PS_DATA_AVAIL;
    ss_cmd_ptr->cmd_subs        = asubs_id;

    /* Queue the commands to CM.
    */
    cm_cmd_queue( (cm_cmd_type*)ss_cmd_ptr );


  return TRUE;
  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_SS_CMD_PS_DATA_AVAIL,
                 CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    /* The function is unavailable due to relavent feature being undefined */
  }
  SYS_ARG_NOT_USED(asubs_id);
  return FALSE;
  #endif /* else of (CM_GW_SUPPORTED &&  defined(FEATURE_PS_DORMANT_PWR_SAVE)) */
}
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_mm_ss_cmd_ps_data_available

DESCRIPTION
  Inform CM that data is available for transmission.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_ss_cmd_ps_data_available(

  cm_ss_cmd_cb_f_type            cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id
    /* requesting client ID */

)
/*lint -esym(715,client_id) */
{
  #if (defined (CM_GW_SUPPORTED) && (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)))
  cm_ss_cmd_s_type                *ss_cmd_ptr;

  ss_cmd_ptr = cm_cmd_alloc_ss_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

    /* Copy command header fields. */

    ss_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ss_cmd_ptr->data_block_ptr  = data_block_ptr;
    ss_cmd_ptr->client_id       = client_id;
    ss_cmd_ptr->cmd             = CM_SS_CMD_PS_DATA_AVAIL;


    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ss_cmd_ptr );


  return TRUE;
  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_SS_CMD_PS_DATA_AVAIL,
                 CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    /* The function is unavailable due to relavent feature being undefined */
  }
  return FALSE;
  #endif /* else of (CM_GW_SUPPORTED && defined(FEATURE_PS_DORMANT_PWR_SAVE)) */
}
/*lint +esym(715,client_id) */

/*===========================================================================

FUNCTION cm_mm_call_cmd_ps_data_available

DESCRIPTION
  Inform CM that data is available for transmission.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_ps_data_available(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id
    /* requesting client ID */

)
/*lint -esym(715,client_id) */
{
  #if (defined (CM_GW_SUPPORTED) && !(defined(FEATURE_GSM) || defined(FEATURE_WCDMA)))
  cm_call_cmd_s_type              *call_cmd_ptr;


  call_cmd_ptr = cm_cmd_alloc_call_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */
    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_PS_DATA_AVAILABLE;

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );


  return TRUE;
  #else /* CM_GW_SUPPORTED */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PS_DATA_AVAILABLE,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (CM_GW_SUPPORTED &&  !defined(FEATURE_PS_DORMANT_PWR_SAVE)) */
}
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_ps_sig_release

DESCRIPTION
  Request CM to release PS signaling.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  FEATURE_GSM || FEATURE_WCDMA must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_ps_sig_release(

  cm_call_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client_id
    /* requesting client ID */

)
/*lint -esym(715,client_id) */
{
  #if( defined CM_GW_SUPPORTED && \
       defined(FEATURE_UE_INITIATED_DORMANCY))

  cm_call_cmd_s_type              *call_cmd_ptr;

  call_cmd_ptr = cm_cmd_alloc_call_init();


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_PS_SIG_REL;

    /* Queue the commands to CM.
    */
    cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

  return TRUE;

  #else /* #if(CM_GW_SUPPORTED && \
        ** defined(FEATURE_UE_INITIATED_DORMANCY))
        */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_CALL_CMD_PS_SIG_REL,
                 CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    /* The function is unavailable due to relavent feature being undefined */
  }
  return FALSE;

  #endif /* end of (#if(CM_GW_SUPPORTED && \
         ** defined(FEATURE_UE_INITIATED_DORMANCY))
         */
}
/*lint +esym(715,client_id) */

/*===========================================================================

FUNCTION cm_mm_call_cmd_incoming_page_resp

DESCRIPTION
  Commands CM to proceed with incoming 1x page.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_call_cmd_incoming_page_resp(

  cm_call_cmd_cb_f_type               cmd_cb_func,
    /* client callback function */

  void                                *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                   client_id,
    /* client responding to incoming page - should be DS */

  boolean                             page_response
    /* response to incoming page: TRUE - to proceed, FALSE - otherwise */

)
{
  #ifdef FEATURE_MMODE_SC_SVLTE
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Get a reference to command information.
    */
  cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
  call_cmd_ptr->cmd_cb_func      = cmd_cb_func;
  call_cmd_ptr->data_block_ptr   = data_block_ptr;
  call_cmd_ptr->client_id        = client_id;
  call_cmd_ptr->cmd              = CM_CALL_CMD_INCOMING_PAGE_RSP;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
  cmd_info_ptr->cdma_info.is_page_proceed       = page_response;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM. */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
  return TRUE;
  #else //FEATURE_MMODE_SC_SVLTE
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_INCOMING_PAGE_RSP,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(page_response);
  return FALSE;
  #endif //FEATURE_MMODE_SC_SVLTE

} /* cm_mm_call_cmd_incoming_page_resp*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_call_cmd_blocked_apn_list

DESCRIPTION
  Command CM to send blocked APN list to NAS.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_call_cmd_blocked_apn_list(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* ending client */

  cm_blocked_apn_list_s_type     *blocked_apn_list_ptr
    /* blocked apn list */

)
{
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;

  if( blocked_apn_list_ptr == NULL )
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_BLOCKED_APN_LIST, CM_CALL_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr = cm_cmd_alloc_call_init();


  /* Get a reference to command header
  ** and a reference to command information.
  */
  cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  call_cmd_ptr->cmd_cb_func      = cmd_cb_func;
  call_cmd_ptr->data_block_ptr   = data_block_ptr;
  call_cmd_ptr->client_id        = client_id;
  call_cmd_ptr->cmd              = CM_CALL_CMD_BLOCKED_APN_LIST;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  *(&(cmd_info_ptr->blocked_apn_list)) = *blocked_apn_list_ptr;

  cmd_info_ptr->blocked_apn_list.as_id  = blocked_apn_list_ptr->as_id;

  memscpy( &cmd_info_ptr->blocked_apn_list.apn_name,
           sizeof(cmd_info_ptr->blocked_apn_list.apn_name),
           &blocked_apn_list_ptr->apn_name ,
           sizeof(blocked_apn_list_ptr->apn_name) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM. */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
  return TRUE;


} /* cm_mm_call_cmd_end */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_mm_call_cmd_end

DESCRIPTION
  Command CM to end a call - in origination, incoming or conversation state.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_call_cmd_end(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* ending client */

  uint8                  num_call_ids,
    /* number of call IDs sent */

  const cm_end_params_s_type   *end_params_ptr
    /* pointer to mode specific data */

)
{
  int                          i;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    call_cmd_ptr->cmd_cb_func      = cmd_cb_func;
    call_cmd_ptr->data_block_ptr   = data_block_ptr;
    call_cmd_ptr->client_id        = client_id;
    call_cmd_ptr->cmd              = CM_CALL_CMD_END;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->end_status       = CM_CALL_END_CLIENT_END;

    if ( num_call_ids > CMCALL_MAX_CALL_OBJ )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NUM_CALL_IDS_P;
      cmd_info_ptr->num_end_call_ids = CMCALL_MAX_CALL_OBJ;
      CM_ERR_1("Invalid num_call_ids: %d",num_call_ids);
    }
    else if ( num_call_ids == 0 )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NUM_CALL_IDS_P;
      cmd_info_ptr->num_end_call_ids = 0;
      CM_ERR_1("Invalid num_call_ids: %d",num_call_ids);
    }
    else
    {
      cmd_info_ptr->num_end_call_ids = num_call_ids;
    }

    if ( end_params_ptr == NULL )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NULL_PARAM_P;
      CM_ERR_0("Null end params ptr");
    }

    if ( cmd_info_ptr->cmd_err == CM_CALL_CMD_ERR_NOERR && end_params_ptr != NULL )
    {
      for ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ )
      {
        *(&(cmd_info_ptr->end_params[i])) = end_params_ptr[i];
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM. */
    cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
    return TRUE;


} /* cm_mm_call_cmd_end */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


#if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
     defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
#error code not present
#endif

/*===========================================================================

FUNCTION cm_mm_call_cmd_exten_end

DESCRIPTION
  Command CM to end a call - in origination, incoming or conversation state.

  Note that the client supplied callback function, cmd_callid_cb_func,
  will be called to notify the client of the command status.
  This call back posts call id also to clients

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_exten_end(

  cm_call_cmd_call_id_cb_f_type cmd_call_id_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* ending client */

  uint8                  num_call_ids,
    /* number of call IDs sent */

  cm_end_params_s_type   *end_params_ptr
    /* pointer to mode specific data */

)
/*lint -esym(715, msm_id) */
/*lint -esym(818, end_params_ptr) */
{
  int                          i;
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  call_cmd_ptr = cm_cmd_alloc_call_init();


    /* Get a reference to command information.
    */
    cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    call_cmd_ptr->cmd_call_id_cb_func = cmd_call_id_cb_func;
    call_cmd_ptr->data_block_ptr   = data_block_ptr;
    call_cmd_ptr->client_id        = client_id;
    call_cmd_ptr->cmd              = CM_CALL_CMD_END;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->end_status       = CM_CALL_END_CLIENT_END;

    if ( num_call_ids > CMCALL_MAX_CALL_OBJ )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NUM_CALL_IDS_P;
      cmd_info_ptr->num_end_call_ids = CMCALL_MAX_CALL_OBJ;
      CM_ERR_1("Invalid num_call_ids: %d",num_call_ids);
    }
    else if ( num_call_ids < 1 )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NUM_CALL_IDS_P;
      cmd_info_ptr->num_end_call_ids = 0;
      CM_ERR_1("Invalid num_call_ids: %d",num_call_ids);
    }
    else
    {
      cmd_info_ptr->num_end_call_ids = num_call_ids;
    }

    if ( end_params_ptr == NULL )
    {
      cmd_info_ptr->cmd_err          = CM_CALL_CMD_ERR_NULL_PARAM_P;
      CM_ERR_0("Null end params ptr");
    }

    if ( cmd_info_ptr->cmd_err == CM_CALL_CMD_ERR_NOERR )
    {
      for ( i = 0; i < cmd_info_ptr->num_end_call_ids; i++ )
      {
        if ( end_params_ptr != NULL )
        {
          *(&(cmd_info_ptr->end_params[i])) = end_params_ptr[i];
        }
      }

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM. */
    cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
    return TRUE;

} /* cm_mm_call_cmd_exten_end */
/*lint +esym(715, msm_id) */
/*lint +esym(818, end_params_ptr) */

/*===========================================================================

FUNCTION cm_mm_call_cmd_privacy_pref

DESCRIPTION
  Command CM to change the privacy preference.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_privacy_pref
(
  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* ending client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  cm_privacy_pref_e_type  privacy_pref
    /* privacy pref setting */
)
/*lint -esym(715, privacy_pref) */
/*lint -esym(715, call_id) */
/*lint -esym(715, client_id) */
{
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_info_cdma_s_type *cmd_mode_info_ptr;


  /* allocate and initialize a Call command buffer */

  call_cmd_ptr = cm_cmd_alloc_call_init();


    /*
    ** and a reference to command information.
    */
    cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );
    cmd_mode_info_ptr  = CALL_CMD_MODE_INFO_PTR( call_cmd_ptr );
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    call_cmd_ptr->cmd_cb_func       = cmd_cb_func;
    call_cmd_ptr->data_block_ptr    = data_block_ptr;
    call_cmd_ptr->client_id         = client_id;
    call_cmd_ptr->cmd               = CM_CALL_CMD_PRIVACY_PREF;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->call_id           = call_id;
    cmd_mode_info_ptr->privacy_pref = privacy_pref;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
    return TRUE;
  #else /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_PRIVACY_PREF,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

} /* cm_mm_call_cmd_privacy_pref() */
/*lint +esym(715, privacy_pref) */
/*lint +esym(715, call_id) */
/*lint +esym(715, client_id) */


/*===========================================================================
FUNCTION cm_mm_call_cmd_sups_per_subs

DESCRIPTION
  Command CM to send a call related supplementary service.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_call_cmd_sups_per_subs(

  cm_call_cmd_cb_f_type             cmd_cb_func,
    /* client callback function */

  void                              *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* sending client */

  cm_call_sups_type_e_type          sups_type,
    /* Type of supplementary service */

  const cm_call_sups_params_s_type  *sups_params_ptr,
    /* Pointer to supplementary service specific data */

  sys_modem_as_id_e_type            asubs_id
    /**< Active subscription id for origination */

)
{

  cm_call_cmd_s_type             *call_cmd_ptr;
  cm_call_cmd_info_s_type        *cmd_info_ptr;

  #ifdef CM_GW_SUPPORTED
  cm_call_cmd_info_gw_cs_s_type       *gw_cs_cmd_mode_info_ptr;
  byte                           asciibuf[CM_MAX_NUMBER_CHARS];
  byte                           bcdbuf[CM_CALLED_PARTY_BCD_WITH_BCD_LEN];
  #endif /* CM_GW_SUPPORTED */

  cm_call_cmd_err_e_type         cmd_err = CM_CALL_CMD_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("CM_CALL_CMD_SUPS client %d, sups_type %d, as_id %d",
              client_id, sups_type, asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* All SUPS commands need to provide parameters
  */
  if( sups_params_ptr == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_NUM_P;
  }
  else
  {
    /* SUPS number needs to be at least initialized
    */
    if(cm_num_validate(&sups_params_ptr->sups_number) != CM_NUM_ERR_NOERR)
    {
      cmd_err = CM_CALL_CMD_ERR_NO_NUM_P;
    }

    /* SUPS call commands need to provide valid call id,
    ** ie. call id of an existing call
    */
    if(cmcall_ptr(sups_params_ptr->call_id) == NULL)
    {
      cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
    }
  }

  /* Reject the command right here if errors found
  */
  if(cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    if(cmd_cb_func != NULL)
    {
      cmd_cb_func( data_block_ptr,CM_CALL_CMD_SUPS,cmd_err);
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command is not rejected, try allocating a CM command buffer.
  */
  call_cmd_ptr = cm_cmd_alloc_call_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, command buffer is allocated successfully.
  */

  /* Copy command header fields.
  */
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_SUPS;
  call_cmd_ptr->cmd_subs        = asubs_id;
  /*
  ** Copy command information fields.
  */
  cmd_info_ptr                  = CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr->call_id         = sups_params_ptr->call_id;

  /* Copy sups type
  */
  cmd_info_ptr->sups_cmd_type   = sups_type;

  /* info_type for processing this command is learnt from the call object
  ** pertaining to this call_id
  */

  #ifdef CM_GW_SUPPORTED

  /*
  ** Copy command information fields - GW specific
  */
  gw_cs_cmd_mode_info_ptr                  = WCALL_CMD_INFO_PTR( call_cmd_ptr );

  /*
  ** Copy call id - all GW SUPS come with valid call id
  */
  gw_cs_cmd_mode_info_ptr->sups_cmd_params.call_id  = sups_params_ptr->call_id;

  if (( sups_type == CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL) ||
      ( sups_type == CM_CALL_SUPS_TYPE_SELECT_CALL_X )    ||
      ( sups_type == CM_CALL_SUPS_TYPE_CALL_DEFLECTION))
  {
    gw_cs_cmd_mode_info_ptr->sups_cmd_params.cause    = sups_params_ptr->cause;

    memset(gw_cs_cmd_mode_info_ptr->called_party_bcd_number.data,
           0xff,
           CM_CALLED_PARTY_BCD_NO_LENGTH);

    memset(asciibuf,0,sizeof(asciibuf));

    gw_cs_cmd_mode_info_ptr->called_party_bcd_number.present = FALSE;
    if ( sups_params_ptr->sups_number.len != 0 )
    {

      /* Convert the number to bcd format
      */
      cm_util_ascii_num_to_bcd_num( &sups_params_ptr->sups_number, bcdbuf, sizeof( bcdbuf ) );

      if (bcdbuf[0] != 0)
      {
        gw_cs_cmd_mode_info_ptr->called_party_bcd_number.present = TRUE;

        /* Length is in first byte */
        gw_cs_cmd_mode_info_ptr->called_party_bcd_number.length = bcdbuf[0];
        memscpy( gw_cs_cmd_mode_info_ptr->called_party_bcd_number.data, sizeof(gw_cs_cmd_mode_info_ptr->called_party_bcd_number.data), &bcdbuf[1], bcdbuf[0] );
      }
      *(&cmd_info_ptr->num) = *(&sups_params_ptr->sups_number);
    }

    gw_cs_cmd_mode_info_ptr->called_party_subaddress.present = FALSE;
    if ( sups_params_ptr->sups_number.subaddr.chari[0] != 0 )
    {
      gw_cs_cmd_mode_info_ptr->called_party_subaddress.present = TRUE;
      memset(gw_cs_cmd_mode_info_ptr->called_party_subaddress.data, 0,
             CM_CALLED_PARTY_SUBADDRESS_LENGTH );
      memscpy( gw_cs_cmd_mode_info_ptr->called_party_subaddress.data,
               sizeof(gw_cs_cmd_mode_info_ptr->called_party_subaddress.data),
               sups_params_ptr->sups_number.subaddr.chari,
               CM_MAX_SUBADDRESS_DIGITS );                      /*Parameter 2 may be replaced with --gw_cs_cmd_mode_info_ptr->called_party_subaddress.length--*/
    }
  }

  if (( sups_type == CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB )  ||
           ( sups_type == CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL )        ||
           ( sups_type == CM_CALL_SUPS_TYPE_HOLD_CALL )              ||
           ( sups_type == CM_CALL_SUPS_TYPE_MULTIPARTY_CALL )        ||
           ( sups_type == CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER ) ||
           ( sups_type == CM_CALL_SUPS_TYPE_ACT_CCBS_REQ ))
  {
    /* if statement preserved since it gives good idea of GW sups cmds
    ** that need to be processed.
    */
  }
  #endif /* CM_GW_SUPPORTED */

  #if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900))
  /*
  ** Copy command information fields - 1X specific
  */
  if (( sups_type == CM_CALL_SUPS_TYPE_EMERGENCY_CALL) ||
      ( sups_type == CM_CALL_SUPS_TYPE_SIMPLE_FLASH) ||
      ( sups_type == CM_CALL_SUPS_TYPE_CALL_FORWARD ) ||
      ( sups_type == CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD ) ||
      ( sups_type == CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD)
  )
  {
      /*
      ** Copy command information fields.
      */
      cmd_info_ptr->cdma_info.flash_type = sups_type;
      cm_num_fill(&cmd_info_ptr->num,
                  sups_params_ptr->sups_number.buf,
                  sups_params_ptr->sups_number.len,
                  sups_params_ptr->sups_number.digit_mode);

  } /* if (( sups_type == CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL) || */
  #endif /* (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900)) */

  #ifdef FEATURE_IP_CALL
  if(sups_type == CM_CALL_SUPS_TYPE_RELEASE_CALL_X_FROM_CONFERENCE
    || sups_type == CM_CALL_SUPS_TYPE_ADD_PARTICIPANT)
  {
    *(&cmd_info_ptr->num) = *(&sups_params_ptr->sups_number);
  }

  /** VT call accepted as VOLTE call */
  if(sups_type == CM_CALL_SUPS_TYPE_HOLD_CALL || sups_type == CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB || sups_type == CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL)
  {
    if(sups_params_ptr != NULL)
    {
      cmd_info_ptr->is_ip_call_info_present = TRUE;
      cmd_info_ptr->ip_call.ip_sups_param = sups_params_ptr->ip_params;
    }
  }

    /** Rejecting a waiting call */
  if(sups_type == CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB)
  {
    if(sups_params_ptr != NULL)
    {
      cmd_info_ptr->ip_call.call_reject_reason = sups_params_ptr->ip_params.rej_cause;
      cmd_info_ptr->ip_call.sip_reject_code = sups_params_ptr->ip_params.sip_reject_code;
    }
  }
  if(sups_type == CM_CALL_SUPS_TYPE_SWITCH_SPEECH_CODEC)
  {
    cmd_info_ptr->ip_call.ip_sups_param.switch_to_codec = sups_params_ptr->ip_params.switch_to_codec;
  }

  /* Fill in the IP ECT parameters to cmd ptr if SUPS type is EXPLICIT_CALL_TRANSFER*/
  if(sups_type == CM_CALL_SUPS_TYPE_IP_EXPLICIT_CALL_TRANSFER)
  {
    cmd_info_ptr->ip_call.ip_sups_ect_params = sups_params_ptr->ip_ect_params;
  }
  
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -restore */

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

  return TRUE;
} /* cm_mm_call_cmd_sups_per_subs() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_mm_call_cmd_sups

DESCRIPTION
  Command CM to send a call related supplementary service.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_call_cmd_sups(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* sending client */

  cm_call_sups_type_e_type    sups_type,
    /* Type of supplementary service */

  const cm_call_sups_params_s_type  *sups_params_ptr
    /* Pointer to supplementary service specific data */

)
{
  return cm_mm_call_cmd_sups_per_subs( cmd_cb_func,
                                       data_block_ptr,
                                       client_id,
                                       sups_type,
                                       sups_params_ptr,
                                       SYS_MODEM_AS_ID_NONE
                                     );

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_mm_call_cmd_get_call_info

DESCRIPTION
  Get a sanpshot of the current call information in the CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request


SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_call_cmd_get_call_info(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* requesting client ID */

  cm_call_id_type        call_id
    /* call ID of the call */

)
{

    cm_call_cmd_s_type             *call_cmd_ptr;
    cm_call_cmd_info_s_type   *cmd_info_ptr;

    call_cmd_ptr = cm_cmd_alloc_call_init();

      /* Get a reference to command header
      */
      cmd_info_ptr = CMD_INFO_PTR( call_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      call_cmd_ptr->data_block_ptr  = data_block_ptr;
      call_cmd_ptr->client_id       = client_id;
      call_cmd_ptr->cmd             = CM_CALL_CMD_INFO_GET;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */
      cmd_info_ptr->call_id         = call_id;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

      return TRUE;
    }
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_mm_call_cmd_get_call_info_list_per_subs

DESCRIPTION
  Get a sanpshot of the state of all call objects maintained by CM.  This
  is a synchronous function.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_call_cmd_get_call_info_list_per_subs(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* requesting client ID */

  sys_modem_as_id_e_type  asubs_id
   /* Subs on which call info needed*/
)
{

  cm_call_cmd_s_type             *call_cmd_ptr;

  call_cmd_ptr = cm_cmd_alloc_call_init();

  /* Copy command header fields.
  */
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_INFO_LIST_GET;
  call_cmd_ptr->cmd_subs        = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

  return TRUE;

} /* cm_mm_call_cmd_get_call_info_list */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_mm_call_cmd_get_call_info_list

DESCRIPTION
  Get a sanpshot of the state of all call objects maintained by CM.  This
  is a synchronous function.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_call_cmd_get_call_info_list(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id
    /* requesting client ID */

)
{
  return  cm_mm_call_cmd_get_call_info_list_per_subs(
                                            cmd_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            SYS_MODEM_AS_ID_MAX
                                            );

} /* cm_mm_call_cmd_get_call_info_list */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif




/*===========================================================================

FUNCTION cm_mm_call_cmd_umts_cdma_handover_req

DESCRIPTION
  Command CM to complete the handoff from UMTS -> 1x.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_umts_cdma_handover_req
(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* originating client */

  cm_call_type_e_type    call_type,
    /* call type - voice, data, etc. */

  cm_call_state_e_type   call_state,
    /* Call state - orig, incoming or conv */

  const sys_is2000_msg_s_type  *ho_msg_ptr,
    /* Pointer to CDMA message from UTRAN */

  cm_call_id_type        *return_call_id_ptr
    /* call ID will be allocated and returned */

)
/*lint -esym(715, ho_msg_ptr) */
/*lint -esym(715, call_state) */
/*lint -esym(715, call_type) */
{
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_err_e_type       cmd_err = CM_CALL_CMD_ERR_NOERR;

  /* allocate and initialize a Call command buffer */
  call_cmd_ptr = cm_cmd_alloc_call_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information into command structure.
  */
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_UMTS_CDMA_HANDOVER_REQ;
  call_cmd_ptr->direction       = CM_CALL_DIRECTION_MO;


  /* Allocate call id.
  */
  if(return_call_id_ptr == NULL)
  {
    cmd_err = CM_CALL_CMD_ERR_NULL_PARAM_P;
  }
  else
  if ((*return_call_id_ptr = cm_call_id_allocate()) == CM_CALL_ID_INVALID )
  {
    cmd_err = CM_CALL_CMD_ERR_NO_CALL_ID_S;
  }

  /* No parameter errors found. Now pack cmd info and queue it for CM
  */
  cmd_info_ptr = CMD_INFO_PTR( call_cmd_ptr );
  if (cmd_err == CM_CALL_CMD_ERR_NOERR)
  {
    /*
    ** Copy command information fields.
    */
    cmd_info_ptr->call_id                   = *return_call_id_ptr;
    cmd_info_ptr->call_type                 = call_type;
    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
    #error code not present
#endif

  } /* if (cmd_err == CM_CALL_CMD_ERR_NOERR) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
  return TRUE;

} /* cm_mm_call_cmd_umts_cdma_handover_req() */
/*lint +esym(715, ho_msg_ptr) */
/*lint +esym(715, call_state) */
/*lint +esym(715, call_type) */


/*===========================================================================

FUNCTION cm_mm_call_cmd_umts_cdma_handover_cnf

DESCRIPTION
  Command to CM to inform about the handover status.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
  Call_id will be returned in call_id_ptr.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_call_cmd_umts_cdma_handover_cnf(

  cm_call_cmd_cb_f_type       cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* originating client */

  cm_call_id_type             call_id,
    /* call ID of the call */

  sys_umts_cdma_handover_e_type  status
    /* Handover status */

)
/*lint -esym(715,status)*/
{

  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;


  /* Allocate call cmd info.
  */
  call_cmd_ptr = cm_cmd_alloc_call_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information into command structure.
  */
  call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  call_cmd_ptr->data_block_ptr  = data_block_ptr;
  call_cmd_ptr->client_id       = client_id;
  call_cmd_ptr->cmd             = CM_CALL_CMD_UMTS_CDMA_HANDOVER_CNF;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */
  cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr->call_id         = call_id;
  #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
  #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the commands to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

  return TRUE;

} /* cm_mm_call_cmd_umts_cdma_handover_cnf() */
/*lint +esym(715,status)*/


/*===========================================================================

FUNCTION cm_data_session_reg

DESCRIPTION
  Register data session callback functions with CM

  When call manager ends a data call, it consults the data session callback
  functions for the following information:

     1. Total number of bytes received and transmitted during the data call.

DEPENDENCIES
  CM must have already been initialized with cm_init_before_task_start().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_data_session_status_e_type cm_data_session_reg(

  cm_get_data_session_total_byte_ctrs_cb_f_type
                              *data_session_total_byte_ctrs_cb_func_ptr
    /* Pointer to data session total byte counters callback function */

)
{
  return cmcall_data_session_reg( data_session_total_byte_ctrs_cb_func_ptr );

}  /* cm_data_session_reg */



/*===========================================================================

FUNCTION cm_ph_cmd_oprt_mode

DESCRIPTION
  Command CM to put phone in a specified operating mode, such as offline,
  online, LPM, etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_oprt_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  sys_oprt_mode_e_type    oprt_mode
    /* Switch to this operating mode */

)
{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_1( "PH CMD - cm_ph_cmd_oprt_mode(), mode=%d",oprt_mode);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();


    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_OPRT_MODE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->oprt_mode  = oprt_mode;
    cmd_info_ptr->cmd_subs   = SYS_MODEM_AS_ID_NONE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  }

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
sys_oprt_mode_e_type cm_ph_get_oprt_mode( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the Current Operating Mode
  */
  return cmph_ptr()->oprt_mode;

}


/*===========================================================================

FUNCTION cm_ph_get_voice_domain_pref

DESCRIPTION
  Command CM to get the current voice domain pref.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

  *** THIS API IS TO REMOVE CMI DEPENDENCY ON WCDMA AND IS TEMPORARY ***

RETURN VALUE
SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY
SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY
SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED
SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED

SIDE EFFECTS
  None

===========================================================================*/
sys_voice_domain_pref_e_type cm_ph_get_voice_domain_pref( void )
{

  /* Return the Current voice domain pref
  */
  return cmph_ptr()->main_stack_info.pref_info.voice_domain_pref;

}

/*===========================================================================

FUNCTION cm_ph_get_voice_domain_pref_per_sub

DESCRIPTION
  Command CM to get the current voice domain pref for the given subscription

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

  *** THIS API IS TO REMOVE CMI DEPENDENCY ON WCDMA AND IS TEMPORARY ***

RETURN VALUE
SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY
SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY
SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED
SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED

SIDE EFFECTS
  None

===========================================================================*/
sys_voice_domain_pref_e_type cm_ph_get_voice_domain_pref_per_sub(
  sys_modem_as_id_e_type asubs_id
    /* subscription to get the voice_domain_pref for */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_0("--------------------PH CMD--------------------");
  CM_MSG_HIGH_1("cm_ph_get_voice_domain_pref_per_sub(), sub %d", asubs_id);
  CM_MSG_LOW_0("----------------------------------------------");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the Current voice domain pref
  */
  return CMPH_SS_VOICE_DOMAIN_PREF(asubs_id, cmph_ptr());
}

#if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
     defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
#error code not present
#endif


/*===========================================================================
FUNCTION cm_ph_cmd_meas_mode

DESCRIPTION
  Command CM to put phone in or out of measurement mode. Currently valid only for
  HDR .Measurement mode puts the modem into a special mode that allows monitoring
  HDR air interface without creating a HDR session or using the existing HDR session

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURNS
  TRUE - request sent, FALSE - No buffer to send request

SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event - CM_PH_EVENT_MEAS_MODE.

===========================================================================*/
boolean cm_ph_cmd_meas_mode(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  sys_meas_mode_e_type   meas_mode
    /* Switch to this operating mode */

)
{
  #ifdef FEATURE_CM_SS_MEAS
  #error code not present
#else /* FEATURE_CM_SS_MEAS */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_MEAS_MODE,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( meas_mode);
  return FALSE;
  #endif /* FEATURE_CM_SS_MEAS */
}



/*===========================================================================

FUNCTION cm_ph_cmd_avoid_sys

DESCRIPTION
  Command CM to avoid the system defined in "avoid_type".
  "avoid_time" is no longer used.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_avoid_sys(

        cm_ph_cmd_cb_f_type    cmd_cb_func,
            /* client callback function */

        void                   *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type      client,
            /* Requesting client */

        cm_ss_avoid_sys_e_type avoid_type,
            /* type of the system to avoid */

        dword                  avoid_time
            /* OBSOLETE - avoid time in seconds */

)
{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1( "CM_PH_CMD_AVOID_SYS, avoid_type=%d", avoid_type);

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_AVOID_SYS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->avoid_type  = avoid_type;
    cmd_info_ptr->avoid_time  = avoid_time;
    cmd_info_ptr->cmd_subs    = SYS_MODEM_AS_ID_1;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  }

/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference */

  cm_pref_term_e_type               term_pref,
    /* Switch the preference for this term */

  dword                             pref_duration,
    /* Specify the duration in seconds for temporary term */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /* gw acquisition order preference */

  cm_band_pref_e_type               band_pref,
    /* Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /* Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /* Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /* Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /* Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type          *plmn_ptr,
    /* If network selection is set to manual, this specifies plmn id */

  const cm_wlan_pref_s_type         *wlan_pref_ptr
    /* If WLAN network selection is set to manual, this specifies SSID,
    ** channle, BSSId etc.
    */

)
/*lint -esym(715, wlan_pref_ptr) */
{
  return (cm_ph_cmd_sys_sel_pref_new( cmd_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           mode_pref,
                                           term_pref,
                                           pref_duration,
                                           acq_order_pref,
                                           band_pref,
                                           prl_pref,
                                           roam_pref,
                                           hybr_pref,
                                           srv_domain_pref,
                                           network_sel_mode_pref,
                                           plmn_ptr));

}
/*lint +esym(715,wlan_pref_ptr) */


/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_new

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref_new(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference */

  cm_pref_term_e_type               term_pref,
    /* Switch the preference for this term */

  dword                             pref_duration,
    /* Specify the duration in seconds for temporary term */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /* gw acquisition order preference */

  cm_band_pref_e_type               band_pref,
    /* Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /* Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /* Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /* Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /* Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type          *plmn_ptr
    /* If network selection is set to manual, this specifies plmn id */

)
{
  return (cm_ph_cmd_sys_sel_pref_with_lte_new( cmd_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           mode_pref,
                                           term_pref,
                                           pref_duration,
                                           acq_order_pref,
                                           band_pref,
                                           CM_BAND_PREF_NO_CHANGE,
                                           prl_pref,
                                           roam_pref,
                                           hybr_pref,
                                           srv_domain_pref,
                                           network_sel_mode_pref,
                                           plmn_ptr));

}



/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_per_subs

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref_per_subs(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  sys_modem_as_id_e_type            asubs_id,
    /* Subscription ID to which these preferences should apply to */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference
    ** CM_MODE_PREF_GSM_ONLY
    ** CM_MODE_PREF_WCDMA_ONLY
    ** CM_MODE_PREF_GSM_WCDMA_ONLY
    ** CM_MODE_PREF_CDMA_ONLY
    ** CM_MODE_PREF_PERSISTENT
    ** CM_MODE_PREF_NO_CHANGE
    ** CM_MODE_PREF_TDS_ONLY( see cm_ph_cmd_sys_sel_pref_per_subs_2 )
    ** CM_MODE_PREF_TDS_GSM ( see cm_ph_cmd_sys_sel_pref_per_subs_2 )
    ** Only the above modes are allowed.
    */

  cm_pref_term_e_type               term_pref,
    /* Switch the preference for this term */

  dword                             pref_duration,
    /* Specify the duration in seconds for temporary term */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /* gw acquisition order preference */

  cm_band_pref_e_type               band_pref,
    /* Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /* Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /* Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /* Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /* Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type          *plmn_ptr
    /* If network selection is set to manual, this specifies plmn id */

)
{
  return cm_ph_cmd_sys_sel_pref_per_subs_1(cmd_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           asubs_id,
                                           mode_pref,
                                           term_pref,
                                           pref_duration,
                                           acq_order_pref,
                                           band_pref,
                                           prl_pref,
                                           roam_pref,
                                           hybr_pref,
                                           srv_domain_pref,
                                           network_sel_mode_pref,
                                           plmn_ptr,
                                           0);
}

/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_per_subs_1

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref_per_subs_1(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function. */

  void                              *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                 client_id,
    /**< Requesting client. */

  sys_modem_as_id_e_type            asubs_id,
    /**< Subscription ID to which these preferences should apply. */

  cm_mode_pref_e_type               mode_pref,
    /**< Switch to this mode preferences. */

  cm_pref_term_e_type               term_pref,
    /**< Switch the preference for this term. */

  dword                             pref_duration,
    /**< Specify the duration in seconds for temporary term. */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /**< GW acquisition order preference. */

  cm_band_pref_e_type               band_pref,
    /**< Switch to this band preference. */

  cm_prl_pref_e_type                prl_pref,
    /**< Switch to this PRL preference. */

  cm_roam_pref_e_type               roam_pref,
    /**< Switch to this roam preference. */

  cm_hybr_pref_e_type               hybr_pref,
    /**< Switch to this hybrid preference. */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /**< Switch to this service domain (e.g., Circuit Switch).
         Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
         domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /**< Defines if the network should be selected automatically
         or manually. */

  const sys_plmn_id_s_type          *plmn_ptr,
    /**< If the network selection is set to manual, this specifies
         the PLMN ID. */

  uint16                             req_id
    /**< Request Id to uniquely identify request - response transactions
         to be supplied by client. */

)
{
    return cm_ph_cmd_sys_sel_pref_4(cmd_cb_func,
                                  data_block_ptr,
                                  client_id,
                                  asubs_id,
                                  mode_pref,
                                  term_pref,
                                  pref_duration,
                                  acq_order_pref,
                                  band_pref,
                                  CM_BAND_PREF_NO_CHANGE,
                                  CM_BAND_PREF_NO_CHANGE,
                                  prl_pref,
                                  roam_pref,
                                  hybr_pref,
                                  srv_domain_pref,
                                  network_sel_mode_pref,
                                  plmn_ptr,
                                  SYS_CSG_ID_INVALID,
                                  SYS_RAT_NONE,
                                  req_id);
}/* cm_ph_cmd_gw_sys_sel_pref_per_subs_1 */

/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_per_subs_2

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  Function for DSDS and DSDA. Added tds_band_pref and lte_band_pref
  to support TDS and LTE(currently not supported) mode prefs.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref_per_subs_2(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function. */

  void                              *data_block_ptr,
    /**< Pointer to the client callback data block. */

  cm_client_id_type                 client_id,
    /**< Requesting client. */

  sys_modem_as_id_e_type            asubs_id,
    /**< Subscription ID to which these preferences should apply. */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference
    ** CM_MODE_PREF_GSM_ONLY
    ** CM_MODE_PREF_WCDMA_ONLY
    ** CM_MODE_PREF_GSM_WCDMA_ONLY
    ** CM_MODE_PREF_CDMA_ONLY
    ** CM_MODE_PREF_PERSISTENT
    ** CM_MODE_PREF_NO_CHANGE
    ** CM_MODE_PREF_TDS_ONLY
    ** CM_MODE_PREF_TDS_GSM
    ** Only the above modes are allowed.
    */

  cm_pref_term_e_type               term_pref,
    /**< Switch the preference for this term. */

  dword                             pref_duration,
    /**< Specify the duration in seconds for temporary term. */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /**< GW acquisition order preference. */

  cm_band_pref_e_type               band_pref,
    /**< Switch to this band preference. */

  cm_band_pref_e_type               lte_band_pref,
    /**< Switch to this LTE Band preference. */

  cm_band_pref_e_type               tds_band_pref,
    /**< Switch to this TDS Band preference. */

  cm_prl_pref_e_type                prl_pref,
    /**< Switch to this PRL preference. */

  cm_roam_pref_e_type               roam_pref,
    /**< Switch to this roam preference. */

  cm_hybr_pref_e_type               hybr_pref,
    /**< Switch to this hybrid preference. */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /**< Switch to this service domain (e.g., Circuit Switch).
         Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
         domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /**< Defines if the network should be selected automatically
         or manually. */

  const sys_plmn_id_s_type          *plmn_ptr,
    /**< If the network selection is set to manual, this specifies
         the PLMN ID. */

  uint16                             req_id
    /**< Request Id to uniquely identify request - response transactions
         to be supplied by client. */

)
{
    return cm_ph_cmd_sys_sel_pref_4(cmd_cb_func,
                                  data_block_ptr,
                                  client_id,
                                  asubs_id,
                                  mode_pref,
                                  term_pref,
                                  pref_duration,
                                  acq_order_pref,
                                  band_pref,
                                  lte_band_pref,
                                  tds_band_pref,
                                  prl_pref,
                                  roam_pref,
                                  hybr_pref,
                                  srv_domain_pref,
                                  network_sel_mode_pref,
                                  plmn_ptr,
                                  SYS_CSG_ID_INVALID,
                                  SYS_RAT_NONE,
                                  req_id);
}

/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_with_lte

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref_with_lte(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference */

  cm_pref_term_e_type               term_pref,
    /* Switch the preference for this term */

  dword                             pref_duration,
    /* Specify the duration in seconds for temporary term */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /* gw acquisition order preference */

  cm_band_pref_e_type               band_pref,
    /* Switch to this band preference */

  cm_band_pref_e_type               lte_band_pref,
    /**< Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /* Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /* Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /* Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /* Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type          *plmn_ptr,
    /* If network selection is set to manual, this specifies plmn id */

  const cm_wlan_pref_s_type         *wlan_pref_ptr
    /* If WLAN network selection is set to manual, this specifies SSID,
    ** channle, BSSId etc.
    */

)
/*lint -esym(715, wlan_pref_ptr) */
{
  return (cm_ph_cmd_sys_sel_pref_with_lte_new( cmd_cb_func,
                                   data_block_ptr,
                                   client_id,
                                   mode_pref,
                                   term_pref,
                                   pref_duration,
                                   acq_order_pref,
                                   band_pref,
                                   lte_band_pref,
                                   prl_pref,
                                   roam_pref,
                                   hybr_pref,
                                   srv_domain_pref,
                                   network_sel_mode_pref,
                                   plmn_ptr));
}
/*lint +esym(715,wlan_pref_ptr) */


/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_with_lte_new

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference).

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref_with_lte_new(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference */

  cm_pref_term_e_type               term_pref,
    /* Switch the preference for this term */

  dword                             pref_duration,
    /* Specify the duration in seconds for temporary term */

  cm_gw_acq_order_pref_e_type       acq_order_pref,
    /* gw acquisition order preference */

  cm_band_pref_e_type               band_pref,
    /* Switch to this band preference */

  cm_band_pref_e_type               lte_band_pref,
    /**< Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /* Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /* Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /* Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /* Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type          *plmn_ptr
    /* If network selection is set to manual, this specifies plmn id */

)
{
  return (cm_ph_cmd_sys_sel_pref2_new( cmd_cb_func,
                                   data_block_ptr,
                                   client_id,
                                   mode_pref,
                                   term_pref,
                                   pref_duration,
                                   acq_order_pref,
                                   band_pref,
                                   lte_band_pref,
                                   CM_BAND_PREF_NO_CHANGE,
                                   prl_pref,
                                   roam_pref,
                                   hybr_pref,
                                   srv_domain_pref,
                                   network_sel_mode_pref,
                                   plmn_ptr));
}

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref2

===========================================================================*/
/**
  Commands the CM to change the phone's system selection preferences (i.e., the
  mode, band, LTE band, TD-SCDMA band, and roaming preferences). This function
  supports 2G, 3G, and 4G.

  The preferred modes that can be used for this function are defined in
  cm_mode_pref_e_type(). The preference terms that can be used in this function
  are defined in cm_pref_term_e_type(). The values for the other preferences are
  defined in cm_hybr_pref_e_type() and cm_srv_domain_pref_e_type(). If a
  particular system selection preference is not to be changed, the appropriate
  NO_CHANGE enumerated value should be passed to this function.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Requesting client.
@param[in] mode_pref             Switch to this mode preference.
@param[in] pref_term             Switch the preference for this term.
@param[in] pref_duration         Specify the duration in seconds for temporary
                                 term.
@param[in] acq_order_pref        GW acquisition order preference.
@param[in] band_pref             Switch to this band preference.
@param[in] lte_band_pref         Switch to this LTE band preference.
@param[in] tds_band_pref         Switch to this TD-SCDMA band preference.
@param[in] prl_pref              Switch to this PRL preference.
@param[in] roam_pref             Switch to this roaming preference.
@param[in] hybr_pref             Switch to this hybrid preference.
@param[in] srv_domain_pref       Switch to this service domain (e.g., Circuit
                                 Switched). Set to CM_SRV_DOMAIN_PREF_NO_CHANGE
                                 if no service domain is being changed.
@param[in] network_sel_mode_pref Defines whether the network should be selected
                                 automatically or manually.
@param[in] plmn_ptr              If network selection is set to manual, this
                                 specifies the PLMN ID.
@param[in] wlan_pref_ptr         WLAN preferences. Clients must check
                                 CM_API_WLAN for WLAN support before using
                                 this field.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
EXTERN boolean cm_ph_cmd_sys_sel_pref2(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_mode_pref_e_type               mode_pref,
  cm_pref_term_e_type               pref_term,
  dword                             pref_duration,
  cm_gw_acq_order_pref_e_type       acq_order_pref,
  cm_band_pref_e_type               band_pref,
  cm_band_pref_e_type               lte_band_pref,
  cm_band_pref_e_type               tds_band_pref,
  cm_prl_pref_e_type                prl_pref,
  cm_roam_pref_e_type               roam_pref,
  cm_hybr_pref_e_type               hybr_pref,
  cm_srv_domain_pref_e_type         srv_domain_pref,
  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
  const sys_plmn_id_s_type          *plmn_ptr,
  const cm_wlan_pref_s_type         *wlan_pref_ptr

)
/*lint -esym(715, wlan_pref_ptr) */
{
    return (cm_ph_cmd_sys_sel_pref2_new( cmd_cb_func,
                                   data_block_ptr,
                                   client_id,
                                   mode_pref,
                                   pref_term,
                                   pref_duration,
                                   acq_order_pref,
                                   band_pref,
                                   lte_band_pref,
                                   tds_band_pref,
                                   prl_pref,
                                   roam_pref,
                                   hybr_pref,
                                   srv_domain_pref,
                                   network_sel_mode_pref,
                                   plmn_ptr));
}
/*lint +esym(715,wlan_pref_ptr) */

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref2_new

===========================================================================*/
/**
  Commands the CM to change the phone's system selection preferences (i.e., the
  mode, band, LTE band, TD-SCDMA band, and roaming preferences). This function
  supports 2G, 3G, and 4G.

  The preferred modes that can be used for this function are defined in
  cm_mode_pref_e_type(). The preference terms that can be used in this function
  are defined in cm_pref_term_e_type(). The values for the other preferences are
  defined in cm_hybr_pref_e_type() and cm_srv_domain_pref_e_type(). If a
  particular system selection preference is not to be changed, the appropriate
  NO_CHANGE enumerated value should be passed to this function.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Requesting client.
@param[in] mode_pref             Switch to this mode preference.
@param[in] pref_term             Switch the preference for this term.
@param[in] pref_duration         Specify the duration in seconds for temporary
                                 term.
@param[in] acq_order_pref        GW acquisition order preference.
@param[in] band_pref             Switch to this band preference.
@param[in] lte_band_pref         Switch to this LTE band preference.
@param[in] tds_band_pref         Switch to this TD-SCDMA band preference.
@param[in] prl_pref              Switch to this PRL preference.
@param[in] roam_pref             Switch to this roaming preference.
@param[in] hybr_pref             Switch to this hybrid preference.
@param[in] srv_domain_pref       Switch to this service domain (e.g., Circuit
                                 Switched). Set to CM_SRV_DOMAIN_PREF_NO_CHANGE
                                 if no service domain is being changed.
@param[in] network_sel_mode_pref Defines whether the network should be selected
                                 automatically or manually.
@param[in] plmn_ptr              If network selection is set to manual, this
                                 specifies the PLMN ID.

@return
  TRUE -- Request sent. \n
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
EXTERN boolean cm_ph_cmd_sys_sel_pref2_new(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_mode_pref_e_type               mode_pref,
  cm_pref_term_e_type               pref_term,
  dword                             pref_duration,
  cm_gw_acq_order_pref_e_type       acq_order_pref,
  cm_band_pref_e_type               band_pref,
  cm_band_pref_e_type               lte_band_pref,
  cm_band_pref_e_type               tds_band_pref,
  cm_prl_pref_e_type                prl_pref,
  cm_roam_pref_e_type               roam_pref,
  cm_hybr_pref_e_type               hybr_pref,
  cm_srv_domain_pref_e_type         srv_domain_pref,
  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
  const sys_plmn_id_s_type          *plmn_ptr

)
{
  return cm_ph_cmd_sys_sel_pref_3(cmd_cb_func,
                                  data_block_ptr,
                                  client_id,
                                  mode_pref,
                                  pref_term,
                                  pref_duration,
                                  acq_order_pref,
                                  band_pref,
                                  lte_band_pref,
                                  tds_band_pref,
                                  prl_pref,
                                  roam_pref,
                                  hybr_pref,
                                  srv_domain_pref,
                                  network_sel_mode_pref,
                                  plmn_ptr,
                                  0);
}

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref_3

===========================================================================*/
/**
  Commands the CM to change the phone's system selection preferences (i.e., the
  mode, band, LTE band, TD-SCDMA band, and roaming preferences). This function
  supports 2G, 3G, and 4G.

  The preferred modes that can be used for this function are defined in
  cm_mode_pref_e_type(). The preference terms that can be used in this function
  are defined in cm_pref_term_e_type(). The values for the other preferences are
  defined in cm_hybr_pref_e_type() and cm_srv_domain_pref_e_type(). If a
  particular system selection preference is not to be changed, the appropriate
  NO_CHANGE enumerated value should be passed to this function.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Requesting client.
@param[in] mode_pref             Switch to this mode preference.
@param[in] pref_term             Switch the preference for this term.
@param[in] pref_duration         Specify the duration in seconds for temporary
                                 term.
@param[in] acq_order_pref        GW acquisition order preference.
@param[in] band_pref             Switch to this 2G/3G band preference.
@param[in] lte_band_pref         Switch to this LTE band preference.
@param[in] tds_band_pref         Switch to this TD-SCDMA band preference.
@param[in] prl_pref              Switch to this PRL preference.
@param[in] roam_pref             Switch to this roaming preference.
@param[in] hybr_pref             Switch to this hybrid preference.
@param[in] srv_domain_pref       Switch to this service domain (e.g., Circuit
                                 Switched). Set to CM_SRV_DOMAIN_PREF_NO_CHANGE
                                 if no service domain is being changed.
@param[in] network_sel_mode_pref Defines whether the network should be selected
                                 automatically or manually.
@param[in] plmn_ptr              If network selection is set to manual, this
                                 specifies the PLMN ID.
@param[in] req_id                Request ID to identify unique request - response
                                 transactions.

@return
  TRUE -- Request sent.
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
EXTERN boolean cm_ph_cmd_sys_sel_pref_3(
  cm_ph_cmd_cb_f_type               cmd_cb_func,
  void                              *data_block_ptr,
  cm_client_id_type                 client_id,
  cm_mode_pref_e_type               mode_pref,
  cm_pref_term_e_type               pref_term,
  dword                             pref_duration,
  cm_gw_acq_order_pref_e_type       acq_order_pref,
  cm_band_pref_e_type               band_pref,
  cm_band_pref_e_type               lte_band_pref,
  cm_band_pref_e_type               tds_band_pref,
  cm_prl_pref_e_type                prl_pref,
  cm_roam_pref_e_type               roam_pref,
  cm_hybr_pref_e_type               hybr_pref,
  cm_srv_domain_pref_e_type         srv_domain_pref,
  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
  const sys_plmn_id_s_type          *plmn_ptr,
  uint16                            req_id
  )
{
  sys_modem_as_id_e_type            asubs_id;

#ifdef CFG_HYBR_GW
  #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
  asubs_id = SYS_MODEM_AS_ID_3;
#else
  asubs_id = SYS_MODEM_AS_ID_1;
#endif

  return cm_ph_cmd_sys_sel_pref_4(cmd_cb_func,
                                  data_block_ptr,
                                  client_id,
                                  asubs_id,
                                  mode_pref,
                                  pref_term,
                                  pref_duration,
                                  acq_order_pref,
                                  band_pref,
                                  lte_band_pref,
                                  tds_band_pref,
                                  prl_pref,
                                  roam_pref,
                                  hybr_pref,
                                  srv_domain_pref,
                                  network_sel_mode_pref,
                                  plmn_ptr,
                                  SYS_CSG_ID_INVALID,
                                  SYS_RAT_NONE,
                                  req_id);
}

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref_4

===========================================================================*/
/**
  Commands the CM to change the phone's system selection preferences (i.e., the
  mode, band, LTE band, TD-SCDMA band, and roaming preferences). This function
  supports 2G, 3G, and 4G.

  The preferred modes that can be used for this function are defined in
  cm_mode_pref_e_type(). The preference terms that can be used in this function
  are defined in cm_pref_term_e_type(). The values for the other preferences are
  defined in cm_hybr_pref_e_type() and cm_srv_domain_pref_e_type(). If a
  particular system selection preference is not to be changed, the appropriate
  NO_CHANGE enumerated value should be passed to this function.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Requesting client.
@param[in] asubs_id              Subscription ID to which these preferences
                                 should apply.
@param[in] mode_pref             Switch to this mode preference.
@param[in] pref_term             Switch the preference for this term.
@param[in] pref_duration         Specify the duration in seconds for temporary
                                 term.
@param[in] acq_order_pref        GW acquisition order preference.
@param[in] band_pref             Switch to this 2G/3G band preference.
@param[in] lte_band_pref         Switch to this LTE band preference.
@param[in] tds_band_pref         Switch to this TD-SCDMA band preference.
@param[in] prl_pref              Switch to this PRL preference.
@param[in] roam_pref             Switch to this roaming preference.
@param[in] hybr_pref             Switch to this hybrid preference.
@param[in] srv_domain_pref       Switch to this service domain (e.g., Circuit
                                 Switched). Set to CM_SRV_DOMAIN_PREF_NO_CHANGE
                                 if no service domain is being changed.
@param[in] network_sel_mode_pref Defines whether the network should be selected
                                 automatically or manually.
@param[in] plmn_ptr              If network selection is set to manual, this
                                 specifies the PLMN ID.
@param[in] csg_id                If network selection is set to be manual, this
                                 specifies the CSG identifier to be used for selection.
@param[in] csg_rat               RAT for CSG selection.
@param[in] req_id                Request ID to identify unique request - response
                                 transactions.

@return
  TRUE -- Request sent.
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_ph_cmd_sys_sel_pref_4(
         cm_ph_cmd_cb_f_type               cmd_cb_func,
         /**< Client callback function. */

         void                             *data_block_ptr,
         /**< Pointer to the client callback data block. */

         cm_client_id_type                 client_id,
         /**< Requesting client. */

         sys_modem_as_id_e_type            asubs_id,
         /**< Subscription ID to which these preferences should apply. */

         cm_mode_pref_e_type               mode_pref,
         /**< Switch to this mode preferences. */

         cm_pref_term_e_type               pref_term,
         /**< Switch the preference for this term. */

         dword                             pref_duration,
         /**< Specify the duration in seconds for temporary term.*/

         cm_gw_acq_order_pref_e_type       acq_order_pref,
         /**< GW acquisition order preference. */

         cm_band_pref_e_type               band_pref,
         /**< Switch to this band preference. */

         cm_band_pref_e_type               lte_band_pref,
         /**< Switch to this LTE Band preference. */

         cm_band_pref_e_type               tds_band_pref,
         /**< Switch to this TDS Band preference. */

         cm_prl_pref_e_type                prl_pref,
         /**< Switch to this PRL preference. */

         cm_roam_pref_e_type               roam_pref,
         /**< Switch to this roam preference. */

         cm_hybr_pref_e_type               hybr_pref,
         /**< Switch to this hybrid preference. */

         cm_srv_domain_pref_e_type         srv_domain_pref,
         /**< Switch to this service domain (e.g., Circuit Switch). Set to
         CM_SRV_DOMAIN_PREF_NO_CHANGE if a service domain is not being changed. */

         cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
         /**< Defines if the network should be selected
              Automatically or manually. */

         const sys_plmn_id_s_type          *plmn_ptr,
         /**< If the network selection is set to manual,
              this specifies the PLMN ID. */

         sys_csg_id_type                    csg_id,
         /**< CSG identifier */

         sys_radio_access_tech_e_type       csg_rat,
         /**< RAT specified for CSG */

         uint16                             req_id
         /**< Request Id to uniquely identify request - response
              transactions to be supplied by client. */
)
{
  cm_sys_sel_pref_params_s_type sys_sel_pref_params;
  cm_rat_acq_order_pref_s_type rat_acq_order_pref;

  sys_sel_pref_params.mode_pref = mode_pref;
  sys_sel_pref_params.term_pref = pref_term;
  sys_sel_pref_params.pref_duration = pref_duration;
  sys_sel_pref_params.band_pref = band_pref;
  sys_sel_pref_params.lte_band_pref = sys_map_band_mask_to_lte_band_mask(lte_band_pref);
  sys_sel_pref_params.tds_band_pref = tds_band_pref;
  sys_sel_pref_params.prl_pref = prl_pref;
  sys_sel_pref_params.roam_pref = roam_pref;
  sys_sel_pref_params.hybr_pref = hybr_pref;
  sys_sel_pref_params.srv_domain_pref = srv_domain_pref;
  sys_sel_pref_params.network_sel_mode_pref = network_sel_mode_pref;
  sys_sel_pref_params.plmn_ptr = plmn_ptr;
  sys_sel_pref_params.csg_id = csg_id;
  sys_sel_pref_params.csg_rat = csg_rat;
  sys_sel_pref_params.ue_usage_setting  = SYS_UE_USAGE_SETTING_NO_CHANGE;
  sys_sel_pref_params.voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NO_CHANGE;
  sys_sel_pref_params.lte_disable_cause = CM_LTE_DISABLE_CAUSE_NO_CHANGE;


  rat_acq_order_pref.type = CM_ACQ_ORDER_TYPE_GW;
  rat_acq_order_pref.acq_order.gw_acq_order = acq_order_pref;
  sys_sel_pref_params.rat_acq_order_pref_ptr = &rat_acq_order_pref;

  return cm_ph_cmd_sys_sel_pref_5(cmd_cb_func,
                                  data_block_ptr,
                                  client_id,
                                  asubs_id,
                                  req_id,
                                  &sys_sel_pref_params);

  }

/*===========================================================================

FUNCTION: cm_ph_cmd_sys_sel_pref_5

===========================================================================*/
/**
  Commands the CM to change the phone's system selection preferences (i.e., the
  mode, band, LTE band, TD-SCDMA band, and roaming preferences). This function
  supports 2G, 3G, and 4G.

  The preferred modes that can be used for this function are defined in
  cm_mode_pref_e_type(). The preference terms that can be used in this function
  are defined in cm_pref_term_e_type(). The values for the other preferences are
  defined in cm_hybr_pref_e_type() and cm_srv_domain_pref_e_type(). If a
  particular system selection preference is not to be changed, the appropriate
  NO_CHANGE enumerated value should be passed to this function.

  The client-supplied callback function cmd_cb_func will be called to notify
  the client of the command status.

@param[in] cmd_cb_func           Client callback function.
@param[in] data_block_ptr        Pointer to the client callback data block.
@param[in] client_id             Requesting client.
@param[in] asubs_id              Subscription ID to which these preferences
                                 should apply.

@param[in] req_id                Request Id to uniquely identify request - response
                                 transactions to be supplied by client.
@param[in] sys_sel_pref_params_ptr Pointer to system selection preferences

@return
  TRUE -- Request sent.
  FALSE -- No buffer to send request.

@dependencies
  cm_init_before_task_start() must have already been called.

@sideeffects
  If the command is successful, CM clients are notified of the corresponding
  CM event.
*/
boolean cm_ph_cmd_sys_sel_pref_5(
    cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function. */

    void                             *data_block_ptr,
    /**< Pointer to the client callback data block. */

    cm_client_id_type                 client_id,
    /**< Requesting client. */

    sys_modem_as_id_e_type            asubs_id,
    /**< Subscription ID to which these preferences should apply. */


    uint16                             req_id,
    /**< Request Id to uniquely identify request - response
              transactions to be supplied by client. */

    cm_sys_sel_pref_params_s_type *sys_sel_pref_params_ptr
)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;

  if(sys_sel_pref_params_ptr == NULL)
  {
    return FALSE;
  }

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Variables for printing PLMN.
    */

  /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr          = data_block_ptr;
    ph_cmd_ptr->client_id               = client_id;
    ph_cmd_ptr->cmd                     = CM_PH_CMD_SYS_SEL_PREF;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->mode_pref             = sys_sel_pref_params_ptr->mode_pref;
    cmd_info_ptr->pref_term             = sys_sel_pref_params_ptr->term_pref;
    cmd_info_ptr->pref_duration         = sys_sel_pref_params_ptr->pref_duration;
    cmd_info_ptr->band_pref             = sys_sel_pref_params_ptr->band_pref;
    cmd_info_ptr->lte_band_pref         = sys_sel_pref_params_ptr->lte_band_pref;
    cmd_info_ptr->tds_band_pref         = sys_sel_pref_params_ptr->tds_band_pref;
    cmd_info_ptr->prl_pref              = sys_sel_pref_params_ptr->prl_pref;
    cmd_info_ptr->roam_pref             = sys_sel_pref_params_ptr->roam_pref;
    cmd_info_ptr->hybr_pref             = sys_sel_pref_params_ptr->hybr_pref;
    cmd_info_ptr->srv_domain_pref       = sys_sel_pref_params_ptr->srv_domain_pref;
    cmd_info_ptr->network_sel_mode_pref = sys_sel_pref_params_ptr->network_sel_mode_pref;
    cmd_info_ptr->sys_sel_pref_req_id   = req_id;
    cmd_info_ptr->ue_usage_setting      = sys_sel_pref_params_ptr->ue_usage_setting;
    cmd_info_ptr->csg_id                = sys_sel_pref_params_ptr->csg_id;
    cmd_info_ptr->csg_rat               = sys_sel_pref_params_ptr->csg_rat;
    cmd_info_ptr->voice_domain_pref     = sys_sel_pref_params_ptr->voice_domain_pref;
    cmd_info_ptr->lte_disable_cause     = sys_sel_pref_params_ptr->lte_disable_cause;

    if ( sys_sel_pref_params_ptr->plmn_ptr != NULL )
    {
      cmd_info_ptr->plmn                = *sys_sel_pref_params_ptr->plmn_ptr;
    }
    else
    {
      sys_plmn_undefine_plmn_id( &cmd_info_ptr->plmn );
    }

    cmd_info_ptr->cmd_subs              = asubs_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if( sys_sel_pref_params_ptr->rat_acq_order_pref_ptr != NULL )
    {
      cmd_info_ptr->rat_acq_order_pref = *sys_sel_pref_params_ptr->rat_acq_order_pref_ptr;

      if(sys_sel_pref_params_ptr->rat_acq_order_pref_ptr->type == CM_ACQ_ORDER_TYPE_GW)
      {
        /* If it's GW type, fill the gw_acq_order
        */
        cmd_info_ptr->acq_order_pref = sys_sel_pref_params_ptr->rat_acq_order_pref_ptr->acq_order.gw_acq_order;
        cmd_info_ptr->rat_acq_order_pref.type = CM_ACQ_ORDER_TYPE_NONE;
      }
      else if(sys_sel_pref_params_ptr->rat_acq_order_pref_ptr->type == CM_ACQ_ORDER_TYPE_RAT_PRI)
      {
        /* If it's RAT_PRI type, fill the gw_acq_order as no change
        */
        cmd_info_ptr->acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
      }
    }
    else
    {
      cmd_info_ptr->acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
    }

    /* Print debug messages for params.
    */
    CM_MSG_HIGH_6("sys_sel_pref(), asubs_id %d, mode_pref %d, pref_term %d, acq_order_pref %d, domain_pref %d, net_sel_mode_pref %d",
                  asubs_id,
                cmd_info_ptr->mode_pref, cmd_info_ptr->pref_term, cmd_info_ptr->acq_order_pref,  
                cmd_info_ptr->srv_domain_pref, cmd_info_ptr->network_sel_mode_pref);
	
    CM_MSG_HIGH_6("sys_sel_pref(), roam_pref %d, hybr_pref %d, prl_pref %d, req_id %u, CSG ID %d, CSG RAT %d",
                  cmd_info_ptr->roam_pref, cmd_info_ptr->hybr_pref, cmd_info_ptr->prl_pref,
                 req_id, cmd_info_ptr->csg_id, cmd_info_ptr->csg_rat);
	
    CM_MSG_HIGH_3("sys_sel_pref(), ue_usage_setting %d, voice_domain_pref %d, lte_disbale_cause %d",
                 cmd_info_ptr->ue_usage_setting,cmd_info_ptr->voice_domain_pref,cmd_info_ptr->lte_disable_cause);

    CM_MSG_HIGH_4("sys_sel_pref(),bands: cgw 0x%08x %08x tds 0x%08x %08x",
                                    QWORD_HIGH(cmd_info_ptr->band_pref),
                                    QWORD_LOW(cmd_info_ptr->band_pref),
                                    QWORD_HIGH(cmd_info_ptr->tds_band_pref),
                                    QWORD_LOW(cmd_info_ptr->tds_band_pref));
    cm_print_lte_band_mask(cmd_info_ptr->lte_band_pref);

    sys_plmn_get_mcc_mnc
    (
      cmd_info_ptr->plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );

    CM_MSG_HIGH_4("PLMN - undefined %d pcs_digit %d, PLMN(%d-%d)",
                plmn_id_is_undefined,
                mnc_includes_pcs_digit,mcc, mnc);


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  }

/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_para_init

DESCRIPTION
  This function initializes payload used to make user pref change.

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE - If initialized, FALSE - if an invalid payload pointer is passed.

SIDE EFFECTS
  NONE.

===========================================================================*/

boolean cm_ph_cmd_sys_sel_pref_para_init(

  cm_sys_sel_pref_params_s_type *sys_sel_pref_params_ptr
    /* Has an update for user preferences requested. This is pointer to a
    ** dynamic memory allocated for the purpose passing data to CM. */

)
{
  if(sys_sel_pref_params_ptr == NULL)
  {
    CM_MSG_HIGH_0("sys_sel_pref_params_ptr is NULL, can't initialize");
    return FALSE;
  }

  /* Reset all fields to zero */
  memset(sys_sel_pref_params_ptr, 0, sizeof(cm_cmd_user_pref_update_type));

  /* Now set fields for which 0 is not invalid */
  sys_sel_pref_params_ptr->mode_pref = CM_MODE_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->term_pref = CM_PREF_TERM_NONE;
  sys_sel_pref_params_ptr->band_pref = CM_BAND_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  sys_sel_pref_params_ptr->tds_band_pref = CM_BAND_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->prl_pref = CM_PRL_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->roam_pref = CM_ROAM_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->csg_id = SYS_CSG_ID_INVALID;
  sys_sel_pref_params_ptr->csg_rat = SYS_RAT_NONE;
  sys_sel_pref_params_ptr->ue_usage_setting = SYS_UE_USAGE_SETTING_NO_CHANGE;
  sys_sel_pref_params_ptr->voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NO_CHANGE;
  sys_sel_pref_params_ptr->lte_disable_cause = CM_LTE_DISABLE_CAUSE_NO_CHANGE;

  return TRUE;
}


/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_with_lte2

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference). This function supports rat acquisition
  preference order including LTE.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref_with_lte2(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference */

  cm_pref_term_e_type               term_pref,
    /* Switch the preference for this term */

  dword                             pref_duration,
    /* Specify the duration in seconds for temporary term */

  cm_band_pref_e_type               band_pref,
    /* Switch to this band preference */

  cm_band_pref_e_type               lte_band_pref,
    /**< Switch to this band preference */

  cm_band_pref_e_type               tds_band_pref,
    /**< Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /* Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /* Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /* Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /* Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type          *plmn_ptr,
    /* If network selection is set to manual, this specifies plmn id */

  cm_rat_acq_order_pref_s_type    *rat_acq_order_pref_ptr
    /* rat acquisition order preference */

)
{
  return cm_ph_cmd_sys_sel_pref_with_lte3(cmd_cb_func,
                                   data_block_ptr,
                                   client_id,
                                   mode_pref,
                                   term_pref,
                                   pref_duration,
                                   band_pref,
                                   lte_band_pref,
                                   tds_band_pref,
                                   prl_pref,
                                   roam_pref,
                                   hybr_pref,
                                   srv_domain_pref,
                                   network_sel_mode_pref,
                                   plmn_ptr,
                                   rat_acq_order_pref_ptr,
                                   0,
                                   SYS_UE_USAGE_SETTING_NO_CHANGE);
}

/*===========================================================================

FUNCTION cm_ph_cmd_sys_sel_pref_with_lte3

DESCRIPTION
  Command CM to change the phone's system selection preference (i.e., the
  mode, band and roam preference). This function supports rat acquisition
  preference order including LTE.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_sys_sel_pref_with_lte3(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  cm_mode_pref_e_type               mode_pref,
    /* Switch to this mode preference */

  cm_pref_term_e_type               term_pref,
    /* Switch the preference for this term */

  dword                             pref_duration,
    /* Specify the duration in seconds for temporary term */

  cm_band_pref_e_type               band_pref,
    /* Switch to this band preference */

  cm_band_pref_e_type               lte_band_pref,
    /**< Switch to this band preference */

  cm_band_pref_e_type               tds_band_pref,
    /**< Switch to this band preference */

  cm_prl_pref_e_type                prl_pref,
    /* Switch to this prl preference */

  cm_roam_pref_e_type               roam_pref,
    /* Switch to this roam preference */

  cm_hybr_pref_e_type               hybr_pref,
    /* Switch to this hybrid preference */

  cm_srv_domain_pref_e_type         srv_domain_pref,
    /* Switch to this service domain (e.g. Circuit Switch)
    ** Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
    ** domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref,
    /* Defines whether the network should be selected automatically
    ** or manually */

  const sys_plmn_id_s_type          *plmn_ptr,
    /* If network selection is set to manual, this specifies plmn id */

  cm_rat_acq_order_pref_s_type    *rat_acq_order_pref_ptr,
    /* rat acquisition order preference */

  uint16                              req_id,
    /* Reqeust ID */

  sys_ue_usage_setting_e_type         ue_usage_setting
    /* UE Usage setting */
)
{
   cm_sys_sel_pref_params_s_type sys_sel_pref_params;
   sys_modem_as_id_e_type asubs_id;

    #ifdef CFG_HYBR_GW
   #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
     asubs_id = SYS_MODEM_AS_ID_3;
    #else
   asubs_id = SYS_MODEM_AS_ID_1;
    #endif

   sys_sel_pref_params.mode_pref = mode_pref;
   sys_sel_pref_params.term_pref = term_pref;
   sys_sel_pref_params.pref_duration = pref_duration;
   sys_sel_pref_params.band_pref = band_pref;
   sys_sel_pref_params.lte_band_pref = sys_map_band_mask_to_lte_band_mask(lte_band_pref);
   sys_sel_pref_params.tds_band_pref = tds_band_pref;
   sys_sel_pref_params.prl_pref = prl_pref;
   sys_sel_pref_params.roam_pref = roam_pref;
   sys_sel_pref_params.hybr_pref = hybr_pref;
   sys_sel_pref_params.srv_domain_pref = srv_domain_pref;
   sys_sel_pref_params.network_sel_mode_pref = network_sel_mode_pref;
   sys_sel_pref_params.plmn_ptr = plmn_ptr;
   sys_sel_pref_params.rat_acq_order_pref_ptr = rat_acq_order_pref_ptr;
   sys_sel_pref_params.csg_id = SYS_CSG_ID_INVALID;
   sys_sel_pref_params.csg_rat = SYS_RAT_NONE;
   sys_sel_pref_params.ue_usage_setting  = ue_usage_setting;
   sys_sel_pref_params.voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NO_CHANGE;
   sys_sel_pref_params.lte_disable_cause = CM_LTE_DISABLE_CAUSE_NO_CHANGE;

   return cm_ph_cmd_sys_sel_pref_5(cmd_cb_func,
                                   data_block_ptr,
                                   client_id,asubs_id,
                                   req_id,
                                   &sys_sel_pref_params);

  }

/**===========================================================================

@FUNCTION cm_ph_cmd_set_user_net_sel_mode_pref

@DESCRIPTION
  Command CM to change the user_sel_net_sel_mode_pref of phone

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@x
===========================================================================*/
boolean cm_ph_cmd_set_user_net_sel_mode_pref(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function */

  void                              *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /**< Requesting client */

  cm_user_net_sel_mode_pref_e_type  user_net_sel_mode
    /**< Defines whether the network should be selected automatically,
    ** manually or default, by user*/

)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_SET_USER_NET_SEL_MODE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->user_net_sel_mode    =  user_net_sel_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug messages for params.
  */
  CM_MSG_HIGH_1 ("set_user_net_sel_mode_pref() - mode_pref:%d",
                user_net_sel_mode);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

}


/*===========================================================================

FUNCTION cm_ph_cmd_ddtm_pref

DESCRIPTION
  Command CM to change the phone's data dedicated transmission mode preference
  to a specified selection, such as ON or OFF etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_ddtm_pref(

  cm_ph_cmd_cb_f_type        cmd_cb_func,
    /* client callback function.
    */

  void                       *data_block_ptr,
    /* pointer to client callback data block.
    */

  cm_client_id_type           client_id,
    /* Requesting client.
    */


  cm_ddtm_pref_e_type        ddtm_pref,
    /* Switch to this DDTM preference.
    */

  sys_ddtm_act_mask_e_type   ddtm_act,
    /* When DDTM is enabled, this field specifies what combined
    ** DDTM actions should takes place to enhance HDR data thru'put.
    ** This is bit-mapped enum, with each bit specifying action.
    */

  cm_ddtm_so_list_act_e_type so_list_act,
    /* Specifies what to do ( add, replace, delete ) with the user
    ** specified SO list in the following parameters.
    */

  uint16                     num_srv_opt,
    /* Number of SOs specified in list,
    ** MAX = SYS_DDTM_MAX_SO_LIST_SIZE
    */

  const sys_srv_opt_type     srv_opt_list[]
    /* List of SOs for which 1x pages have to ignored while in
    ** HDR connected state. if srv_opt_list[] contains CAI_SO_NULL
    ** & ddtm_pref is ON & ddtm_act includes ignore_so_pages,
    ** ignore all pages.
    */

)
/*lint -esym(715, ddtm_pref) */
/*lint -esym(715, ddtm_act) */
/*lint -esym(715, so_list_act) */
/*lint -esym(715, num_srv_opt) */
/*lint -esym(715, client_id) */
/*lint -esym(715, srv_opt_list) */
{
  #ifdef FEATURE_DDTM_CNTL
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;
  int i;
  cm_ph_cmd_err_e_type     cmd_err = CM_PH_CMD_ERR_NOERR;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

  if(srv_opt_list == NULL)
  {
    cmd_err = CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P;
  }

  if(cmd_err != CM_PH_CMD_ERR_NOERR)
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_DDTM_PREF, cmd_err);
    }
    /* Free the memory */
    if ( ph_cmd_ptr != NULL )
    {
      cm_cmd_dealloc(ph_cmd_ptr);
    }
    return FALSE;

  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr  = data_block_ptr;
  ph_cmd_ptr->client_id       = client_id;
  ph_cmd_ptr->cmd             = CM_PH_CMD_DDTM_PREF;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->ddtm_pref  = ddtm_pref;
  cmd_info_ptr->ddtm_act_mask = ddtm_act;
  cmd_info_ptr->ddtm_so_list_act = so_list_act;
  cmd_info_ptr->ddtm_num_srv_opt = num_srv_opt;

  CM_MSG_HIGH_4("DDTM: ddtm_pref %d, ddtm_act_mask %d, ddtm_so_list_act %d, ddtm_num_srv_opt %d",
                ddtm_pref, ddtm_act, so_list_act, num_srv_opt);

  for ( i = 0; i < num_srv_opt && i < SYS_DDTM_MAX_SO_LIST_SIZE; i++  )
  {
    cmd_info_ptr->ddtm_srv_opt_list[i] = srv_opt_list[i];
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

  #else /* FEATURE_DDTM_CNTL */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_DDTM_PREF,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* FEATURE_DDTM_CNTL */

} /* cm_ph_cmd_ddtm_pref() */
/*lint +esym(715, ddtm_pref) */
/*lint +esym(715, ddtm_act) */
/*lint +esym(715, so_list_act) */
/*lint +esym(715, num_srv_opt) */
/*lint +esym(715, client_id) */
/*lint +esym(715, srv_opt_list) */

/*===========================================================================

FUNCTION cm_ph_cmd_block_srv_opt

DESCRIPTION
  Commands CM to change the phone's Data Dedicated Transmission Mode (DDTM) preference to block service options
  Clients responsibility to unblock the service options which were blocked by calling this API.
  
  The client-supplied callback function cmd_cb_func will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
  boolean cm_ph_cmd_block_srv_opt(
    cm_ph_cmd_cb_f_type         cmd_cb_func,
    /*Client callback function. */

    void                       *data_block_ptr,
    /*Pointer to the client callback data block. */

    cm_client_id_type           client_id,
    /*Requesting client*/

    uint16                      num_srv_opt,
    /*number of SOs provided in the list*/

    const sys_srv_opt_type     *srv_opt_list_ptr
    /*List of SOs for which 1X pages have to be ignored*/
  
  )
{
  #ifdef FEATURE_DDTM_CNTL
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;
  int i;
  cm_ph_cmd_err_e_type     cmd_err = CM_PH_CMD_ERR_NOERR;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

 
  /* When num service option is more than 0 make sure list of srv opt is not NULL 
   */
  if(num_srv_opt > 0 && srv_opt_list_ptr == NULL)
  {
    cmd_err = CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P;
  }
  /* Ensure we receive at most SYS_DDTM_MAX_SO_LIST_SIZE entries
   */
  if(num_srv_opt > SYS_DDTM_MAX_SO_LIST_SIZE)
  {
    cmd_err = CM_PH_CMD_ERR_DDTM_NUM_SRV_OPTIONS_P;
  }
  /* If command buffer allocation failed,
   ** call the command callback and return FALSE
   */

  if(cmd_err != CM_PH_CMD_ERR_NOERR)
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_CHANGE_SRV_OPT, cmd_err);
    }
    /* Free the memory */
    if ( ph_cmd_ptr != NULL )
    {
      cm_cmd_dealloc(ph_cmd_ptr);
    }
    return FALSE;

  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr  = data_block_ptr;
  ph_cmd_ptr->client_id       = client_id;
  ph_cmd_ptr->cmd             = CM_PH_CMD_CHANGE_SRV_OPT;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->ddtm_num_blocked_srv_opt  = num_srv_opt;

  CM_MSG_HIGH_1 ("Block SO : ddtm_num_srv_opt=%d",num_srv_opt);

  memscpy(cmd_info_ptr->ddtm_blocked_srv_opt_list, num_srv_opt * sizeof(sys_srv_opt_type),
              srv_opt_list_ptr, num_srv_opt * sizeof(sys_srv_opt_type) );

  for ( i = 0; i < num_srv_opt && i < SYS_DDTM_MAX_SO_LIST_SIZE; i++  )
  {
    CM_MSG_HIGH_2("blocked srv opt [%d] - %d", i, cmd_info_ptr->ddtm_blocked_srv_opt_list[i]);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

  #else /* FEATURE_DDTM_CNTL */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_CHANGE_SRV_OPT,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* FEATURE_DDTM_CNTL */

} /* cm_ph_cmd_block_srv_opt() */


/*===========================================================================

FUNCTION cm_ph_cmd_answer_voice

DESCRIPTION
  Command CM to change the phone's answer-voice-as setting to a specified
  selection, such as answer-voice-as-data, answer-voice-as-voice, etc.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_answer_voice(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type          client,
            /* Requesting client */

        cm_answer_voice_e_type     answer_voice,
            /* Indicate whether phone should answer incoming
            ** voice calls as voice, fax or modem */

        dword                      answer_duration
            /* Indicate seconds duration for temp. setting, such as
            ** CM_ANSWER_VOICE_AS_FAX_ONCE or DB_VOICE_AS_MODEM_ONCE */
)
/*lint -esym(715, client) */
/*lint -esym(715, answer_voice) */
/*lint -esym(715, answer_duration) */
{
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

   /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_ANSWER_VOICE;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */
      cmd_info_ptr->answer_voice     = answer_voice;
      cmd_info_ptr->answer_duration  = answer_duration;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  #else /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_ANSWER_VOICE,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

}
/*lint +esym(715, client) */
/*lint +esym(715, answer_voice) */
/*lint +esym(715, answer_duration) */



/*===========================================================================

FUNCTION cm_ph_cmd_nam_sel

DESCRIPTION
  Command CM to change the phone's NAM selection to NAM-1, NAM-2, etc.

  NOTE! This command fails when trying to change the NAM to illegal
  selection. E.g. changing to NAM-2 for a 1 NAM target.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_nam_sel(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* Requesting client */

        cm_nam_e_type               nam_sel
            /* Change NAM selection to this NAM */

)
/*lint -esym(715, client) */
/*lint -esym(715, nam_sel) */
{
  #if ((defined(FEATURE_MMODE_CDMA_800) && defined(FEATURE_MMODE_CDMA_1900)) || defined (FEATURE_JCDMA))
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;



  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_NAM_SEL;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->nam_sel       = nam_sel;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );
    return TRUE;

  #else /* ((defined(FEATURE_MMODE_CDMA_800) && defined(FEATURE_MMODE_CDMA_1900)) || defined (FEATURE_JCDMA)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_NAM_SEL,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of ((defined(FEATURE_MMODE_CDMA_800) && defined(FEATURE_MMODE_CDMA_1900)) || defined (FEATURE_JCDMA)) */
} /* cm_ph_cmd_nam_sel() */
/*lint +esym(715, client) */
/*lint +esym(715, nam_sel) */



/*===========================================================================

FUNCTION cm_ph_cmd_get_subs_pref_info

DESCRIPTION
  Get a sanpshot of the current phone information in the CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_get_subs_pref_info(

  cm_ph_cmd_cb_f_type     cmd_cb_func,
        /* client callback function */

  void                    *data_block_ptr,
        /* pointer to client callback data block */

  cm_client_id_type       client,
        /* requesting client ID */

  sys_modem_as_id_e_type  asubs_id

)
{
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;

  CM_MSG_MED_1("CM_PH_CMD_SUBSCRIPTION_INFO_GET as_id %d", asubs_id);

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* If command buffer allocation failed,
  ** call the command callback and return FALSE
  */

    /* Get a reference to command header
    */
    cmd_info_ptr = CMD_INFO_PTR(ph_cmd_ptr);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_SUBSCRIPTION_INFO_GET;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmd_info_ptr->cmd_subs      = asubs_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;

} /*cm_ph_cmd_get_subs_pref_info*/


/*===========================================================================

FUNCTION cm_ph_cmd_get_ph_info_per_subs

DESCRIPTION
  Get a sanpshot of the current phone information in the CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_ph_cmd_get_ph_info_per_subs(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

  void                   *data_block_ptr,
        /* pointer to client callback data block */

  cm_client_id_type      client,
        /* requesting client ID */

  sys_modem_as_id_e_type asubs_id

)
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;

  CM_MSG_MED_1("CM_PH_CMD_INFO_GET as_id %d", asubs_id);

  ph_cmd_ptr = cm_cmd_alloc_ph_init();


    /* Get a reference to command header
    */
    cmd_info_ptr = CMD_INFO_PTR(ph_cmd_ptr);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_INFO_GET;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmd_info_ptr->cmd_subs      = asubs_id;


    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;

} /* cm_ph_cmd_get_ph_info_per_subs */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_ph_cmd_get_ph_info

DESCRIPTION
  Get a sanpshot of the current phone information in the CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_ph_cmd_get_ph_info(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

  void                   *data_block_ptr,
        /* pointer to client callback data block */

  cm_client_id_type      client
        /* requesting client ID */

)
{
  return  cm_ph_cmd_get_ph_info_per_subs( cmd_cb_func,
                                          data_block_ptr,
                                          client,
                                          #ifdef CFG_HYBR_GW
                                          #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                          SYS_MODEM_AS_ID_3
                                          #else
                                          SYS_MODEM_AS_ID_1
                                          #endif
                                        );
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_ph_cmd_rssi_delta_threshold

DESCRIPTION
  Set the RSSI delta value.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_rssi_delta_threshold(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  delta
        /* RSSI change delta threshold for RSSI reporting */
)
{

  /* Call the new api and pass on the delta for rssi
  */
  return( cm_ph_cmd_signal_strength_delta
          (
            cmd_cb_func,
            data_block_ptr,
            client,
            delta,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE
           )
        );
}


/*===========================================================================

FUNCTION cm_ph_cmd_signal_strength_delta

DESCRIPTION
  Set the RSSI/ECIO/IO delta value.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_signal_strength_delta(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  rssi_delta,
        /* RSSI change delta threshold for Signal Strength reporting */

    uint8                  ecio_delta,
        /* ECIO change delta threshold for Signal Strength  reporting */

    uint8                  io_delta
        /* IO change delta threshold for Signal Strength  reporting */
)
{
  /* Call the new api and pass on the delta for rssi,ecio,io
  */
  return( cm_ph_cmd_signal_strength_delta2
          (
            cmd_cb_func,
            data_block_ptr,
            client,
            rssi_delta,
            ecio_delta,
            io_delta,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE
           )
        );

} /* cm_ph_cmd_signal_strength_delta */



/*===========================================================================

FUNCTION cm_ph_cmd_signal_strength_delta2

DESCRIPTION
  Set the RSSI/ECIO/IO/SIR/PATHLOSS delta value.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_signal_strength_delta2(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  rssi_delta,
        /* RSSI change delta threshold for Signal Strength reporting */

    uint8                  ecio_delta,
        /* ECIO change delta threshold for Signal Strength  reporting */

    uint8                  io_delta,
        /* IO change delta threshold for Signal Strength  reporting */

    uint8                  sir_delta,
        /* SIR change delta threshold for Signal Strength  reporting */

    uint8                  pathloss_delta
        /* PATHLOSS change delta threshold for Signal Strength  reporting */
)
{
  /* Call the new api and pass on the delta for rssi,ecio,io,sir,pathloss
  */
  return( cm_ph_cmd_signal_strength_delta3
          (
            cmd_cb_func,
            data_block_ptr,
            client,
            rssi_delta,
            ecio_delta,
            io_delta,
            sir_delta,
            pathloss_delta,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE
           )
        );

} /* cm_ph_cmd_signal_strength_delta2 */


/*===========================================================================

FUNCTION cm_ph_cmd_signal_strength_delta3

DESCRIPTION
  Set the RSSI/ECIO/IO/SIR/PATHLOSS/CQI delta value.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_signal_strength_delta3(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  rssi_delta,
        /* RSSI change delta threshold for Signal Strength reporting */

    uint8                  ecio_delta,
        /* ECIO change delta threshold for Signal Strength  reporting */

    uint8                  io_delta,
        /* IO change delta threshold for Signal Strength  reporting */

    uint8                  sir_delta,
        /* SIR change delta threshold for Signal Strength  reporting */

    uint8                  pathloss_delta,
        /* PATHLOSS change delta threshold for Signal Strength  reporting */

    uint8                  cqi_delta
        /* CQI change delta threshold for Signal Strength  reporting */
)
{
    return( cm_ph_cmd_signal_strength_delta_lte
          (
            cmd_cb_func,
            data_block_ptr,
            client,
            rssi_delta,
            ecio_delta,
            io_delta,
            sir_delta,
            pathloss_delta,
            cqi_delta,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE,
            CM_SIG_STRENGTH_DELTA_NO_CHANGE
           )
        );


} /* cm_ph_cmd_signal_strength_delta3 */



/*===========================================================================

FUNCTION cm_ph_cmd_signal_strength_delta_lte

DESCRIPTION
  Set the RSSI/ECIO/IO/SIR/PATHLOSS/CQI/RSRQ/RSRP delta value.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
   TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_cmd_signal_strength_delta_lte(

    cm_ph_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                   *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type      client,
        /* requesting client ID */

    uint8                  rssi_delta,
        /* RSSI change delta threshold for Signal Strength reporting */

    uint8                  ecio_delta,
        /* ECIO change delta threshold for Signal Strength  reporting */

    uint8                  io_delta,
        /* IO change delta threshold for Signal Strength  reporting */

    uint8                  sir_delta,
        /* SIR change delta threshold for Signal Strength  reporting */

    uint8                  pathloss_delta,
        /* PATHLOSS change delta threshold for Signal Strength  reporting */

    uint8                  cqi_delta,
        /* CQI change delta threshold for Signal Strength  reporting */

    uint8                   rsrp_delta,
        /* LTE RSRP change delta threshold for Signal Strength  reporting */

    uint8                   rsrq_delta
        /* LTE RSRQ change delta threshold for Signal Strength  reporting */
)
{
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;

  CM_MSG_HIGH_3("CM_PH_CMD_SIGNAL_STRENGTH_DELTA, rssi_delta=%d, ecio_delta=%d, io_delta=%d",
               rssi_delta, ecio_delta, io_delta );
  CM_MSG_HIGH_5("sir_delta=%d, pathloss_delta=%d, cqi_delta=%d, rsrp_delta=%d, rsrq_delta=%d",
               sir_delta, pathloss_delta, cqi_delta,rsrp_delta, rsrq_delta);
 
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func      = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr   = data_block_ptr;
  ph_cmd_ptr->client_id        = client;
  ph_cmd_ptr->cmd              = CM_PH_CMD_SIGNAL_STRENGTH_DELTA;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->rssi_delta     = rssi_delta;
  cmd_info_ptr->ecio_delta     = ecio_delta;
  cmd_info_ptr->io_delta       = io_delta;
  cmd_info_ptr->sir_delta      = sir_delta;
  cmd_info_ptr->pathloss_delta  = pathloss_delta;
  cmd_info_ptr->cqi_delta      = cqi_delta;
  cmd_info_ptr->rsrp_delta     = rsrp_delta;
  cmd_info_ptr->rsrq_delta     = rsrq_delta;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

} /* cm_ph_cmd_signal_strength_delta_lte */

/*===========================================================================

FUNCTION cm_ph_cmd_packet_state

DESCRIPTION
  Command CM to change Rm packet state in packet mode.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_packet_state(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client_id,
            /* Requesting client */

        cm_packet_state_e_type      packet_state
            /* Change Rm packet state to this state */

)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;



  ph_cmd_ptr = cm_cmd_alloc_ph_init();


    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */
    /* Get a reference to command header
    ** and a reference to command information.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_PACKET_STATE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->packet_state  = packet_state;
    cmd_info_ptr->cmd_subs      = SYS_MODEM_AS_ID_1;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );
  return TRUE;

  }



/*===========================================================================

FUNCTION cm_ph_cmd_voc_activity

DESCRIPTION
  Command CM to activate voice activity test for current or next voice call.
  On call termination, the voice activity test will be disabled.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_voc_activity(

        cm_ph_cmd_cb_f_type    cmd_cb_func,
            /* client callback function */

        void                   *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type      client,
            /* Requesting client */

        boolean                enable,
            /* enable/disable voice activity test */

        word                   voc_1,
            /* # of full rate */

        word                   voc_2,
            /* # of 1/2 rate */

        word                   voc_8
            /* # of 1/8 rate */

)
/*lint -esym(715, client) */
/*lint -esym(715, enable) */
/*lint -esym(715, voc_1) */
/*lint -esym(715, voc_2) */
/*lint -esym(715, voc_8) */
{
  #ifdef FEATURE_JCDMA
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;



  ph_cmd_ptr = cm_cmd_alloc_ph_init();


    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id          = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_VOC_ACTIVITY;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->voc_act_enable  = enable;
    cmd_info_ptr->voc_act_voc_1   = voc_1;
    cmd_info_ptr->voc_act_voc_2   = voc_2;
    cmd_info_ptr->voc_act_voc_8   = voc_8;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  #else /* FEATURE_JCDMA */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_VOC_ACTIVITY,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of FEATURE_JCDMA */
}
/*lint +esym(715, client) */
/*lint +esym(715, enable) */
/*lint +esym(715, voc_1) */
/*lint +esym(715, voc_2) */
/*lint +esym(715, voc_8) */

/*===========================================================================

FUNCTION cm_ph_cmd_perso_info_available

DESCRIPTION
  This function informs CM that the personalization information is
  available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_perso_info_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw2_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw3_status
    /**< Subscription status of G sub in SXLTE configuration */

)
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  CM_MSG_HIGH_4( "CM_PH_CMD_PERSO_INFO_AVAILABLE, cdma=%d, gwl=%d, gwl2=%d gwl3=%d",
                cdma_status,gwl_status, gw2_status, gw3_status);


  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func        = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr     = data_block_ptr;
    ph_cmd_ptr->client_id          = client_id;
    ph_cmd_ptr->cmd                = CM_PH_CMD_PERSO_INFO_AVAILABLE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cdma_sub_avail_status = cdma_status;
    cmd_info_ptr->gwl_sub_avail_status  = gwl_status;
    cmd_info_ptr->gw2_sub_avail_status  = gw2_status;
    cmd_info_ptr->gw3_sub_avail_status  = gw3_status;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  status = TRUE;

  return status;

} /* cm_ph_cmd_perso_info_available */

/*===========================================================================

FUNCTION cm_ph_cmd_subscription_available2

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  has been verified and available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_available2(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw2_status
    /**< Subscription status of GSM/WCDMA system */
)
{

  return cm_ph_cmd_subscription_available3( cmd_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            cdma_status,
                                            gwl_status,
                                            gw2_status,
                                            CM_SUBSCRIPTION_STATUS_NO_CHANGE
                                           );
} /* cm_ph_cmd_subscription_available2 */

/*===========================================================================

FUNCTION cm_ph_cmd_subscription_available3

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  has been verified and available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_available3(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw2_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw3_status
    /**< Subscription status of GSM/WCDMA system */

)
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

 CM_MSG_HIGH_4( "cm_ph_cmd_subscription_available3, cdma=%d, gwl=%d gw2=%d, gw3=%d",
                cdma_status,gwl_status, gw2_status,gw3_status);

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func        = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr     = data_block_ptr;
    ph_cmd_ptr->client_id          = client_id;
    ph_cmd_ptr->cmd                = CM_PH_CMD_SUBSCRIPTION_AVAILABLE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cdma_sub_avail_status = cdma_status;
    cmd_info_ptr->gwl_sub_avail_status  = gwl_status;
    cmd_info_ptr->gw2_sub_avail_status  = gw2_status;
    cmd_info_ptr->gw3_sub_avail_status  = gw3_status;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    status = TRUE;

  return status;

} /* cm_ph_cmd_subscription_available2 */

/*===========================================================================

FUNCTION cm_ph_cmd_subscription_available

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  has been verified and available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA/LTE system */

  cm_subscription_status_e_type   wlan_status
    /**< Subscription status of WLAN system */

)
/*lint -esym(715, wlan_status) */
{
  return cm_ph_cmd_subscription_available_new( cmd_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            cdma_status,
                                            gwl_status
                                           );
}
/*lint +esym(715,wlan_status) */
/*===========================================================================

FUNCTION cm_ph_cmd_subscription_available_new

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  has been verified and available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_available_new(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status
    /**< Subscription status of GSM/WCDMA/LTE system */

)
{
  return cm_ph_cmd_subscription_available3( cmd_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            cdma_status,
                                            gwl_status,
                                            #ifdef CFG_HYBR_GW
                                            #error code not present
#else
                                            CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                            #endif
                                            #ifdef CFG_HYBR_GW
                                            #error code not present
#else
                                            CM_SUBSCRIPTION_STATUS_NO_CHANGE
                                            #endif
                                           );
}

/*===========================================================================

FUNCTION cm_ph_cmd_subscription_not_available

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is not available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_not_available(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   wlan_status
    /**< Subscription status of WLAN system */

)
/*lint -esym(715, wlan_status) */
{
  return cm_ph_cmd_subscription_not_available_new(cmd_cb_func,
                                               data_block_ptr,
                                               client_id,
                                               cdma_status,
                                               gwl_status
                                               );

} /* cm_ph_cmd_subscription_not_available */
/*lint +esym(715,wlan_status) */


/*===========================================================================

FUNCTION cm_ph_cmd_subscription_not_available2

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is not available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_not_available2(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw2_status
    /**< Subscription status of GSM/WCDMA system */
)
{
  return cm_ph_cmd_subscription_not_available3( cmd_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            cdma_status,
                                            gwl_status,
                                            gw2_status,
                                            CM_SUBSCRIPTION_STATUS_NO_CHANGE
                                           );


} /* cm_ph_cmd_subscription_not_available2 */

/*===========================================================================

FUNCTION cm_ph_cmd_subscription_not_available3

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is not available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_not_available3(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw2_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw3_status
    /**< Subscription status of GSM/WCDMA system */


)
{

  return cm_ph_cmd_subscription_not_available4(cmd_cb_func,
                                               data_block_ptr,
                                               client_id,
                                               cdma_status,
                                               gwl_status,
                                               gw2_status,
                                               gw3_status,
                                               CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_OTHERS
                                               );


} /* cm_ph_cmd_subscription_not_available3 */
/*===========================================================================

FUNCTION cm_ph_cmd_subscription_not_available_new

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is not available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_not_available_new(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status
    /**< Subscription status of GSM/WCDMA system */

)
{
  return cm_ph_cmd_subscription_not_available3(cmd_cb_func,
                                               data_block_ptr,
                                               client_id,
                                               cdma_status,
                                               gwl_status,
                                               #ifdef CFG_HYBR_GW
                                               #error code not present
#else
                                               CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                               #endif
                                               #ifdef CFG_HYBR_GW
                                               #error code not present
#else
                                               CM_SUBSCRIPTION_STATUS_NO_CHANGE
                                               #endif
                                               );

} /* cm_ph_cmd_subscription_not_available_new */


/*===========================================================================

FUNCTION cm_ph_cmd_subscription_not_available4

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is not available for consumption.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_not_available4(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw2_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw3_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_not_avail_cause_e_type cause
    /**< Cause for subscription not available */


)
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  CM_MSG_HIGH_5( "CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE, cdma=%d, gwl=%d, gw2=%d, gw3=%d cause=%d",
                cdma_status,gwl_status,gw2_status,gw3_status,cause);


  ph_cmd_ptr = cm_cmd_alloc_ph_init();


    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func        = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr     = data_block_ptr;
    ph_cmd_ptr->client_id          = client_id;
    ph_cmd_ptr->cmd                = CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cdma_sub_not_avail_status = cdma_status;
    cmd_info_ptr->gwl_sub_not_avail_status  = gwl_status;
    cmd_info_ptr->gw2_sub_not_avail_status  = gw2_status;
    cmd_info_ptr->gw3_sub_not_avail_status  = gw3_status;
    cmd_info_ptr->sub_not_avail_cause = cause;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    status = TRUE;

  return status;

} /* cm_ph_cmd_subscription_not_available4 */


/*lint -esym(765, cm_ph_cmd_subscription_changed3) */
/*===========================================================================

FUNCTION cm_ph_cmd_subscription_changed3

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is changed.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  If the subscription is currently not available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
static boolean cm_ph_cmd_subscription_changed3(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw2_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw3_status
    /**< Subscription status of GSM/WCDMA system */

)
{
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  CM_MSG_HIGH_4( "CM_PH_CMD_SUBSCRIPTION_CHANGED, cdma=%d, gwl=%d gw2_status=%d, gw3_status=%d",
                cdma_status,gwl_status, gw2_status,gw3_status);


  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func        = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr     = data_block_ptr;
    ph_cmd_ptr->client_id          = client_id;
    ph_cmd_ptr->cmd                = CM_PH_CMD_SUBSCRIPTION_CHANGED;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cdma_sub_changed_status = cdma_status;
    cmd_info_ptr->gwl_sub_changed_status  = gwl_status;
    cmd_info_ptr->gw2_sub_changed_status  = gw2_status;
    cmd_info_ptr->gw3_sub_changed_status  = gw3_status;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    status = TRUE;

  return status;

} /* cm_ph_cmd_subscription_changed3 */
/*lint +esym(765, cm_ph_cmd_subscription_changed3) */
/*===========================================================================

FUNCTION cm_ph_cmd_subscription_changed2

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is changed.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  If the subscription is currently not available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
static boolean cm_ph_cmd_subscription_changed2(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA system */

  cm_subscription_status_e_type   gw2_status
    /**< Subscription status of GSM/WCDMA system */

)
{


  return cm_ph_cmd_subscription_changed3(cmd_cb_func,
                                         data_block_ptr,
                                         client_id,
                                         cdma_status,
                                         gwl_status,
                                         gw2_status,
                                         CM_SUBSCRIPTION_STATUS_NO_CHANGE
                                         );

} /* cm_ph_cmd_subscription_changed2 */
/*lint +esym(765, cm_ph_cmd_subscription_changed2) */

/*===========================================================================

FUNCTION cm_ph_cmd_subscription_changed

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is changed.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  If the subscription is currently not available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_changed(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status,
    /**< Subscription status of GSM/WCDMA/LTE system */

  cm_subscription_status_e_type   wlan_status
    /**< Subscription status of WLAN system */
)
/*lint -esym(715, wlan_status) */
{
  return cm_ph_cmd_subscription_changed_new(cmd_cb_func,
                                         data_block_ptr,
                                         client_id,
                                         cdma_status,
                                         gwl_status
                                         );
} /* cm_ph_cmd_subscription_changed */
/*lint +esym(715,wlan_status) */

/*===========================================================================

FUNCTION cm_ph_cmd_subscription_changed_new

DESCRIPTION
  This function informs CM that the subscription/provisioning information
  is changed.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  If the subscription is currently not available, the request will be
  ignored and a CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S is returned.

  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_subscription_changed_new(

  cm_ph_cmd_cb_f_type             cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< Requesting client */

  cm_subscription_status_e_type   cdma_status,
    /**< Subscription status of CDMA system */

  cm_subscription_status_e_type   gwl_status
    /**< Subscription status of GSM/WCDMA/LTE system */

)
{
  return cm_ph_cmd_subscription_changed2(cmd_cb_func,
                                         data_block_ptr,
                                         client_id,
                                         cdma_status,
                                         gwl_status,
                                         CM_SUBSCRIPTION_STATUS_NO_CHANGE
                                         );
} /* cm_ph_cmd_subscription_changed_new */


/*===========================================================================

FUNCTION cm_ph_cmd_get_networks_per_subs

DESCRIPTION
  Command CM to retrieve either a list of preferred networks or a list of
  all available networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  If command is successful, CM clients will receive the list of networks
  via the CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or CM_PH_EVENT_PREFERRED_-
  NETWORKS_CONF event.
===========================================================================*/
boolean cm_ph_cmd_get_networks_per_subs (

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /* client callback function                                 */

        void                        *data_block_ptr,
            /* pointer to client callback data block                    */

        cm_client_id_type            client_id,
            /* Requesting client                                        */

        cm_network_list_type_e_type  list_type,
            /* Indicates whether to retrieve the preferred or available */
            /* networks                                                 */

        cm_mode_pref_e_type          network_type,
            /* Indicates the desired network such as GW/LTE.
            */

        sys_modem_as_id_e_type       asubs_id
          /* Subscription ID to use for retrieve the GW network list
          ** Only Valid when the network_type is GW and in DualStandby
          ** In Single standby mode search can be performed only on active subs
          */
)
/*lint -esym(715,list_type) */
/*lint -esym(715,client_id) */
{

    return cm_ph_cmd_get_networks_per_subs_extn ( cmd_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           list_type,
                                           network_type,
                                           asubs_id,
                                           CM_BAND_PREF_ANY,
                                           CM_BAND_PREF_LTE_ANY,
                                           CM_BAND_PREF_TDS_ANY
                                          );
} /* cm_ph_cmd_get_networks_per_subs */

/*===========================================================================

FUNCTION cm_ph_cmd_get_networks_per_subs_extn

DESCRIPTION
  Command CM to retrieve either a list of preferred networks or a list of
  all available networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  If command is successful, CM clients will receive the list of networks
  via the CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or CM_PH_EVENT_PREFERRED_-
  NETWORKS_CONF event.
===========================================================================*/
boolean cm_ph_cmd_get_networks_per_subs_extn (

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /* client callback function                                 */

        void                        *data_block_ptr,
            /* pointer to client callback data block                    */

        cm_client_id_type            client_id,
            /* Requesting client                                        */

        cm_network_list_type_e_type  list_type,
            /* Indicates whether to retrieve the preferred or available */
            /* networks                                                 */

        cm_mode_pref_e_type          network_type,
            /* Indicates the desired network such as GW/LTE.
            */

        sys_modem_as_id_e_type       asubs_id,
          /* Subscription ID to use for retrieve the GW network list
          ** Only Valid when the network_type is GW and in DualStandby
          ** In Single standby mode search can be performed only on active subs
          */

        cm_band_pref_e_type          band_pref_lmt,

        cm_band_pref_e_type          lte_band_pref_lmt,

        cm_band_pref_e_type          tds_band_pref_lmt

)
/*lint -esym(715,list_type) */
/*lint -esym(715,client_id) */
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func         = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr      = data_block_ptr;
    ph_cmd_ptr->client_id           = client_id;
    ph_cmd_ptr->cmd                 = CM_PH_CMD_GET_NETWORKS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->network_list_type = list_type;
    cmd_info_ptr->network_type      = network_type;
    cmd_info_ptr->cmd_subs          = asubs_id;
    cmd_info_ptr->band_pref_lmt     = band_pref_lmt;
    cmd_info_ptr->lte_band_pref_lmt = sys_map_band_mask_to_lte_band_mask(lte_band_pref_lmt);
    cmd_info_ptr->tds_band_pref_lmt     = tds_band_pref_lmt;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* print the cmd details
    */
  CM_MSG_HIGH_5("CM_PH_CMD_GET_NETWORKS: network_type %d, list_type %d, subs_id %d. GW  BAND PREF 0x%08x %08x ",
                network_type, list_type, asubs_id,QWORD_HIGH(band_pref_lmt), QWORD_LOW(band_pref_lmt));


  CM_MSG_HIGH_4("User LTE BAND PREF 0x%08x %08x ,User TDS BAND PREF 0x%08x %08x ",
                 QWORD_HIGH(lte_band_pref_lmt), QWORD_LOW(lte_band_pref_lmt),
  		 QWORD_HIGH(tds_band_pref_lmt), QWORD_LOW(tds_band_pref_lmt));
  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  status = TRUE;
  return status;

} /* cm_ph_cmd_get_networks_per_subs_extn */
/*lint +esym(715,list_type) */
/*lint +esym(715,client_id) */

/*===========================================================================

FUNCTION cm_ph_cmd_get_networks_extn

DESCRIPTION
  Command CM to retrieve either a list of preferred networks or a list of
  all available networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  If command is successful, CM clients will receive the list of networks
  via the CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or CM_PH_EVENT_PREFERRED_-
  NETWORKS_CONF event.
===========================================================================*/
boolean cm_ph_cmd_get_networks_extn(

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /* client callback function                                 */

        void                        *data_block_ptr,
            /* pointer to client callback data block                    */

        cm_client_id_type            client_id,
            /* Requesting client                                        */

        cm_network_list_type_e_type  list_type,
            /* Indicates whether to retrieve the preferred or available */
            /* networks                                                 */

        cm_mode_pref_e_type          network_type,
            /* Indicates the desired network such GW.
            ** Only GW is supported*/

        cm_band_pref_e_type          band_pref_lmt,

        cm_band_pref_e_type          lte_band_pref_lmt,

        cm_band_pref_e_type          tds_band_pref_lmt


)
{
  return cm_ph_cmd_get_networks_per_subs_extn ( cmd_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           list_type,
                                           network_type,
                                           #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                           SYS_MODEM_AS_ID_3,
                                           #elif defined CFG_HYBR_GW
                                           #error code not present
#else
                                           SYS_MODEM_AS_ID_1,
                                           #endif
                                           band_pref_lmt,
                                           lte_band_pref_lmt,
                                           tds_band_pref_lmt
                                          );

} /* cm_ph_cmd_get_networks_extn */

/*===========================================================================

FUNCTION cm_ph_cmd_get_networks

DESCRIPTION
  Command CM to retrieve either a list of preferred networks or a list of
  all available networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  If command is successful, CM clients will receive the list of networks
  via the CM_PH_EVENT_AVAILABLE_NETWORKS_CONF or CM_PH_EVENT_PREFERRED_-
  NETWORKS_CONF event.
===========================================================================*/
boolean cm_ph_cmd_get_networks(

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /* client callback function                                 */

        void                        *data_block_ptr,
            /* pointer to client callback data block                    */

        cm_client_id_type            client_id,
            /* Requesting client                                        */

        cm_network_list_type_e_type  list_type,
            /* Indicates whether to retrieve the preferred or available */
            /* networks                                                 */

        cm_mode_pref_e_type          network_type
            /* Indicates the desired network such GW.
            ** Only GW is supported*/

)
{
  return cm_ph_cmd_get_networks_extn ( cmd_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           list_type,
                                           network_type,
                                       CM_BAND_PREF_ANY,
                                       CM_BAND_PREF_LTE_ANY,
                                       CM_BAND_PREF_TDS_ANY
                                          );

} /* cm_ph_cmd_get_networks */

/*===========================================================================

FUNCTION cm_ph_cmd_set_hplmn_timer

DESCRIPTION
  Set HPLMN timer request - please note timer value is in minutes

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
===========================================================================*/
boolean cm_ph_cmd_set_hplmn_timer(

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /* client callback function                                 */

        void                        *data_block_ptr,
            /* pointer to client callback data block                    */

        cm_client_id_type            client_id,
            /* Requesting client                                        */

        uint32                       timer_to_set,
           /* HPLMN timer needs to be set at NAS */

        sys_modem_as_id_e_type       asubs_id
)
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func         = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr      = data_block_ptr;
  ph_cmd_ptr->client_id           = client_id;
  ph_cmd_ptr->cmd                 = CM_PH_CMD_SET_HPLMN_TIMER;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->cmd_subs          = asubs_id;
  cmd_info_ptr->hplmn_timer       = timer_to_set;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  /* print the cmd details
  */
  CM_MSG_MED_2("CM_PH_CMD_SET_HPLMN_TIMER: timer %d, as_id=%d",
              timer_to_set, asubs_id);

  return TRUE;
} /* cm_ph_cmd_set_hplmn_timer  */


/*===========================================================================

FUNCTION cm_ph_cmd_get_hplmn_timer

DESCRIPTION
  Get HPLMN timer from NAS. CM will send request to NAS to get HPLMN timer and will send back to client
  through event.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
===========================================================================*/
boolean cm_ph_cmd_get_hplmn_timer(

        cm_ph_cmd_cb_f_type          cmd_cb_func,
            /* client callback function                                 */

        void                        *data_block_ptr,
            /* pointer to client callback data block                    */

        cm_client_id_type            client_id,
            /* Requesting client                                        */

        sys_modem_as_id_e_type       asubs_id
)
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_info_ptr->cmd_subs          = asubs_id;
  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func         = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr      = data_block_ptr;
  ph_cmd_ptr->client_id           = client_id;
  ph_cmd_ptr->cmd                 = CM_PH_CMD_GET_HPLMN_TIMER;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* print the cmd details
  */
  CM_MSG_HIGH_1("cm_ph_cmd_get_hplmn_timer: as_id=%d",
                asubs_id);

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;
} /* cm_ph_cmd_get_hplmn_timer  */
/*===========================================================================

FUNCTION cm_ph_cmd_terminate_get_networks_per_subs

DESCRIPTION
  Command CM to terminate retrieval operation of list of all available
  networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  If command is successful, the previous command to retrieve the list will be
  terminated.

===========================================================================*/
boolean cm_ph_cmd_terminate_get_networks_per_subs(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type            client_id,
    /* Requesting client */

  sys_modem_as_id_e_type       asubs_id
    /* Subscription ID to use for termination the GW network list request
    ***/
)
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  boolean                    status;

  CM_MSG_HIGH_1("CM_PH_CMD_TERMINATE_GET_NETWORKS as_id %d", asubs_id);

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command header
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func         = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr      = data_block_ptr;
    ph_cmd_ptr->client_id           = client_id;
    ph_cmd_ptr->cmd                 = CM_PH_CMD_TERMINATE_GET_NETWORKS;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cmd_subs          = asubs_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    status = TRUE;
  return status;

} /* cm_ph_cmd_terminate_get_networks_per_subs */


/*===========================================================================

FUNCTION cm_ph_cmd_terminate_get_networks

DESCRIPTION
  Command CM to terminate retrieval operation of list of all available
  networks.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  If command is successful, the previous command to retrieve the list will be
  terminated.

===========================================================================*/
boolean cm_ph_cmd_terminate_get_networks(

  cm_ph_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type            client_id
    /* Requesting client */
)
{
  return cm_ph_cmd_terminate_get_networks_per_subs( cmd_cb_func,
                                                    data_block_ptr,
                                                    client_id,
                                                    #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                                    SYS_MODEM_AS_ID_3
                                                    #elif defined CFG_HYBR_GW
                                                    #error code not present
#else
                                                    SYS_MODEM_AS_ID_1
                                                    #endif
                                                  );
} /* cm_ph_cmd_terminate_get_networks */


/*===========================================================================

FUNCTION cm_ph_cmd_set_preferred_networks_per_subs

DESCRIPTION
  Command CM to save the preferred networks to the SIM/USIM.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_set_preferred_networks_per_subs(

        cm_ph_cmd_cb_f_type                   cmd_cb_func,
            /* client callback function              */

        void                                  *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                     client_id,
            /* Requesting client                     */

        const sys_user_pref_plmn_list_s_type  *preferred_networks_ptr,
            /* The preferred networks to be saved    */

        sys_modem_as_id_e_type                asubs_id
          /* Subscription ID to use for retrieve the GW network list
          ** Only Valid when the network_type is GW and in DualStandby
          ** In Single standby mode search can be performed only on active subs
          */
)
{
  #ifdef CM_GW_SUPPORTED
  cm_ph_cmd_s_type          *ph_cmd_ptr;
  cm_ph_cmd_info_s_type     *cmd_info_ptr;
  int                        i;
  boolean                    status;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  CM_MSG_HIGH_1("CM_PH_CMD_SET_PREFERRED_NETWORKS as_id %d", asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func          = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr       = data_block_ptr;
  ph_cmd_ptr->client_id            = client_id;
  ph_cmd_ptr->cmd                  = CM_PH_CMD_SET_PREFERRED_NETWORKS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->cmd_subs = asubs_id;
  cmd_info_ptr->user_pref_networks.length = preferred_networks_ptr->length;

  if(cmd_info_ptr->user_pref_networks.length <= SYS_USER_PPLMN_LIST_MAX_LENGTH)
  {
    for ( i = 0; i < preferred_networks_ptr->length; i++ )
    {
       cmd_info_ptr->user_pref_networks.info[ i ] = preferred_networks_ptr->info[i];
    }
  }

  else
  {
    if( cmd_cb_func != NULL )
    {
     cmd_cb_func( data_block_ptr, CM_PH_CMD_SET_PREFERRED_NETWORKS,
     CM_PH_CMD_ERR_USER_PREFERRED_NETWORK_LIST_ACCESS);
    }
   status = FALSE;
   return status;

  }/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  status = TRUE;

  return status;

  #else /* CM_GW_SUPPORTED */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_TERMINATE_GET_NETWORKS,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  SYS_ARG_NOT_USED(asubs_id);
  SYS_ARG_NOT_USED( client_id);
  SYS_ARG_NOT_USED( preferred_networks_ptr );

  return FALSE;
  #endif /* CM_GW_SUPPORTED */

} /* cm_ph_cmd_save_preferred_networks_per_subs */


/*===========================================================================

FUNCTION cm_ph_cmd_set_preferred_networks

DESCRIPTION
  Command CM to save the preferred networks to the SIM/USIM.

  Note that the client supplied callback function, cmd_cb_func, will be
  called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  = command was successfully sent to CM
  FALSE = command was not successfully sent to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_set_preferred_networks(

        cm_ph_cmd_cb_f_type             cmd_cb_func,
            /* client callback function              */

        void                            *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type               client_id,
            /* Requesting client                     */

        const sys_user_pref_plmn_list_s_type  *preferred_networks_ptr
            /* The preferred networks to be saved    */
)
{
  return cm_ph_cmd_set_preferred_networks_per_subs( cmd_cb_func,
                                                    data_block_ptr,
                                                    client_id,
                                                    preferred_networks_ptr,
                                                    #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                                    SYS_MODEM_AS_ID_3
                                                    #elif defined CFG_HYBR_GW
                                                    #error code not present
#else
                                                    SYS_MODEM_AS_ID_1
                                                    #endif
                                                   );
} /* cm_ph_cmd_save_preferred_networks */


/*===========================================================================

FUNCTION cm_ph_cmd_wakeup_from_standby

DESCRIPTION
  Command CM to inform lower layers to wake up from deep sleep (standby mode)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_wakeup_from_standby(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id
    /* Requesting client */
)
{
  return cm_ph_cmd_wakeup_from_standby2(cmd_cb_func,
                                        data_block_ptr,
                                        client_id,
                                        CM_WAKEUP_ALL);
}

/*===========================================================================

FUNCTION cm_ph_cmd_wakeup_from_standby_per_subs

DESCRIPTION
  Command CM to inform lower layers to wake up from deep sleep (standby mode)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_wakeup_from_standby_per_subs(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  sys_modem_as_id_e_type  asubs_id
    /**< Subscription ID of the stack to wake-up
    */

)
{
  cm_wakeup_e_type wakeup_ss = CM_WAKEUP_NONE;
  cm_ss_e_type     ss        = CM_SS_MAIN;

  ss = cmph_map_subs_to_ss(asubs_id);

  CM_MSG_HIGH_2( "cm_ph_cmd_wakeup_from_standby_per_subs as_id %d ss %d",
                 asubs_id, ss );

  if( ss == CM_SS_MAIN )
  {
    wakeup_ss = CM_WAKEUP_MAIN;
  }
  else if( ss == CM_SS_HYBR_2 )
  {
    wakeup_ss = CM_WAKEUP_HYBR_GW;
  }
    else if( ss == CM_SS_HYBR_3 )
  {
    wakeup_ss = CM_WAKEUP_HYBR_GW3;
  }

  return cm_ph_cmd_wakeup_from_standby2(cmd_cb_func,
                                        data_block_ptr,
                                        client_id,
                                        wakeup_ss);
}


/*===========================================================================

FUNCTION cm_ph_cmd_wakeup_from_standby2

DESCRIPTION
  Command CM to inform lower layers to wake up from deep sleep (standby mode)

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_wakeup_from_standby2(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  cm_wakeup_e_type       service
    /* Service to be waken up
    ** In dual standby operation when service is MAIN both GW stacks will be
    ** woken-up
    */
)
{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;


  ph_cmd_ptr = cm_cmd_alloc_ph_init();


    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_WAKEUP_FROM_STANDBY;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */

    cmd_info_ptr->wakeup_service = service;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  }


#ifdef FEATURE_MRU_UPDATE

/*===========================================================================

FUNCTION cm_ph_cmd_mru_update

DESCRIPTION
  Command CM to update the indexth record of MRU buffer.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_mru_update(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  cm_mmode_mru_table_entry_u_type *mru_table,
   /* Pointer to mru_table_entry object*/

  uint8                    index,
   /* Need to be update the indexth record of MRU buffer with system record   */

  sys_sys_mode_e_type       mode,
    /*System Mode      */
  cm_mru_update_e_type     mru_update_e_type
   /*Type of enum which tells either update the MRU buffer or clear the MRU buffer */

)

/*lint -esym(818,mru_table) */


{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_2( "PHCMD - CM_PH_CMD_MRU_UPDATE client_id   - %d sys_sys_mode_e_type - %d" , client_id, mode );
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();


    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_MRU_UPDATE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    *(&cmd_info_ptr->mru_table) = *mru_table;
  cmd_info_ptr->mode = mode;
  cmd_info_ptr->index = index;
  cmd_info_ptr->mru_update_e_type = mru_update_e_type;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;

} /* cm_ph_cmd_change_rtre_config() */

/*lint +esym(818,mru_table) */

/*===========================================================================

FUNCTION cm_ph_cmd_read_mru

DESCRIPTION
  Command CM to Read the indexth record from MRU buffer update data block pointer with Data.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_read_mru(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  uint8                    index
    /* index will give that  which record needs to be read from the MRU buffer .
       */

)
/*lint -esym(715, client_id) */
/*lint -esym(715, rtre_config) */
{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_2( " CM_PH_CMD_READ_MRU client_id   - %d index =%d", client_id,index );
 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_READ_MRU;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
  cmd_info_ptr->index = index;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;


} /* cm_ph_cmd_change_rtre_config() */

#endif

/*===========================================================================

FUNCTION cm_ph_cmd_change_rtre_config

DESCRIPTION
  Command CM to change RTRE configuration

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_change_rtre_config(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  cm_rtre_config_e_type  rtre_config

)
/*lint -esym(715, client_id) */
/*lint -esym(715, rtre_config) */
{
  #if ( defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) )
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_2( "CM_PH_CMD_CHANGE_RTRE_CONFIG client_id - %d rtre_config = %d",client_id, rtre_config);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_CHANGE_RTRE_CONFIG;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->rtre_config   = rtre_config;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;

  #else /* defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_CHANGE_RTRE_CONFIG,
      CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE) */

} /* cm_ph_cmd_change_rtre_config() */
/*lint +esym(715, client_id) */
/*lint +esym(715, rtre_config) */


/*===========================================================================

FUNCTION cm_ph_cmd_dual_standby_pref

DESCRIPTION
  Command CM to avoid the system defined in "avoid_type".
  "avoid_time" is no longer used.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_dual_standby_pref(

        cm_ph_cmd_cb_f_type                 cmd_cb_func,
            /* client callback function */

        void                                *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                    client_id,
            /* Requesting client */

        sys_modem_dual_standby_pref_e_type   standby_pref,
            /* Standby preference of the Phone */

        sys_modem_as_id_e_type               active_subs,
            /* The active subscription in Single Standby mode
            ** only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY
            */

        sys_modem_as_id_e_type               default_voice_subs,
            /* type of the system to avoid */

        sys_modem_as_id_e_type               default_data_subs,
            /* type of the system to avoid */

        sys_modem_as_id_e_type               priority_subs,
            /* type of the system to avoid */
       cm_dds_cause_e_type                  dds_switch_type
            /* type of DDS Switch */  
)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;
  uint8                   active_subs_mask;
  int invalid_range_check_cnt = -1;



  if(active_subs == SYS_MODEM_AS_ID_NO_CHANGE)
  {
    active_subs_mask = SYS_MODEM_AS_ID_NO_CHANGE_MASK;
  }
  else if(active_subs == SYS_MODEM_AS_ID_1)
  {
    active_subs_mask = SYS_MODEM_AS_ID_1_MASK;
  }
  else
  {
    active_subs_mask = SYS_MODEM_AS_ID_2_MASK;
  }

  /* This API shouldnt be called for standby pref change
  ** This is defensive change to take care in case called*/

  if(standby_pref == SYS_MODEM_DS_PREF_DUAL_STANDBY ||
    standby_pref == SYS_MODEM_DS_PREF_DUAL_STANDBY_NO_TUNEAWAY)
  {
    active_subs_mask = SYS_MODEM_AS_ID_1_MASK;
    active_subs_mask |= SYS_MODEM_AS_ID_2_MASK;
  }

  if(standby_pref == SYS_MODEM_DS_PREF_TRIPLE_STANDBY ||
      standby_pref == SYS_MODEM_DS_PREF_TRIPLE_STANDBY_NO_TUNEAWAY )
  {
    active_subs_mask = SYS_MODEM_AS_ID_1_MASK;
    active_subs_mask |=  SYS_MODEM_AS_ID_2_MASK;
    active_subs_mask |=  SYS_MODEM_AS_ID_3_MASK;
  }

  if(standby_pref == SYS_MODEM_DS_PREF_NO_STANDBY_PREF)
  {
    active_subs_mask = SYS_MODEM_AS_ID_NO_ACTIVE_MASK;
  }

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /* Check the validity of new preferences.
    */
    if(!(BETWEEN(standby_pref,      invalid_range_check_cnt,  SYS_MODEM_DS_PREF_MAX) &&
         BETWEEN(priority_subs,     SYS_MODEM_AS_ID_NONE,SYS_MODEM_AS_ID_MAX)   &&
         BETWEEN(default_voice_subs,SYS_MODEM_AS_ID_NONE,SYS_MODEM_AS_ID_MAX)   &&
         BETWEEN(dds_switch_type,   DDS_CAUSE_NONE,DDS_CAUSE_MAX)   &&
         BETWEEN(default_data_subs, SYS_MODEM_AS_ID_NONE,SYS_MODEM_AS_ID_MAX)))
    {
      
      CM_MSG_HIGH_6("Invalid dual standby pref vals, Reject cmd, standby pref %d, active subs %d, priority subs %d, dflt voice %d, dflt data %d, dds type %d",
                    standby_pref, active_subs, priority_subs,
                  default_voice_subs, default_data_subs, dds_switch_type);
      
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_DUAL_STANDBY_PREF;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->active_subs        = active_subs_mask;
    cmd_info_ptr->standby_pref       = standby_pref;
    cmd_info_ptr->default_voice_subs = default_voice_subs;
    cmd_info_ptr->default_data_subs  = default_data_subs;
    cmd_info_ptr->priority_subs      = priority_subs;
    cmd_info_ptr->dds_switch_type    = dds_switch_type;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_HIGH_5("Current DS preferences: standby pref %d, active subs %d, priority subs %d, default voice %d, default data %d",
                 cmph_ptr()->user_standby_pref, cmph_ptr()->active_subs,
                  cmph_ptr()->priority_subs, cmph_ptr()->default_voice_subs,
                  cmph_ptr()->default_data_subs);

    CM_MSG_HIGH_6("New DS preferences: standby pref %d, active subs_mask %d, priority subs %d, default voice %d, default data %d, dds_cause %d",
                 standby_pref, active_subs_mask, priority_subs,
                 default_voice_subs, default_data_subs, dds_switch_type);

    /* Queue the command to CM.
       */
    cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  }

/*===========================================================================

FUNCTION cm_ph_cmd_dual_standby_pref_1

DESCRIPTION
  Command CM to avoid the system defined in "avoid_type".
  "avoid_time" is no longer used.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  CDMA Only.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_dual_standby_pref_1(

        cm_ph_cmd_cb_f_type                 cmd_cb_func,
            /* client callback function */

        void                                *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                    client_id,
            /* Requesting client */

        sys_modem_dual_standby_pref_e_type   standby_pref,
            /* Standby preference of the Phone */

        uint8                                 active_subs,
            /* The active subscription in Single Standby mode
            ** only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY
            */

        sys_modem_as_id_e_type               default_voice_subs,
            /* type of the system to avoid */

        sys_modem_as_id_e_type               default_data_subs,
            /* type of the system to avoid */

        sys_modem_as_id_e_type               priority_subs,
            /* type of the system to avoid */
        cm_dds_cause_e_type                  dds_switch_type
            /* type of DDS Switch */   
)
{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  int invalid_range_check_cnt = -1;


  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /* Check the validity of new preferences.
    */
    if(!(BETWEEN(standby_pref,      invalid_range_check_cnt,  SYS_MODEM_DS_PREF_MAX) &&
         BETWEEN(priority_subs,     SYS_MODEM_AS_ID_NONE,SYS_MODEM_AS_ID_MAX)   &&
         BETWEEN(default_voice_subs,SYS_MODEM_AS_ID_NONE,SYS_MODEM_AS_ID_MAX)   &&
         BETWEEN(default_data_subs, SYS_MODEM_AS_ID_NONE,SYS_MODEM_AS_ID_MAX)))
    {
      CM_MSG_HIGH_6("Invalid dual standby pref vals, Reject cmd, standby pref %d, active subs %d, priority subs %d, default voice %d, default data %d, dds_cause %d",
                  standby_pref, active_subs, priority_subs, 
                  default_voice_subs, default_data_subs, dds_switch_type);
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_DUAL_STANDBY_PREF;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->active_subs        = active_subs;
    cmd_info_ptr->standby_pref       = standby_pref;
    cmd_info_ptr->default_voice_subs = default_voice_subs;
    cmd_info_ptr->default_data_subs  = default_data_subs;
    cmd_info_ptr->priority_subs      = priority_subs;
    cmd_info_ptr->dds_switch_type    = dds_switch_type;

    if(standby_pref == SYS_MODEM_DS_PREF_NO_STANDBY_PREF)
    {
      active_subs = SYS_MODEM_AS_ID_NO_ACTIVE_MASK;
    }

    if(standby_pref == SYS_MODEM_DS_PREF_DUAL_STANDBY ||
      standby_pref == SYS_MODEM_DS_PREF_DUAL_STANDBY_NO_TUNEAWAY)
    {
      active_subs = SYS_MODEM_AS_ID_1_MASK;
      active_subs |= SYS_MODEM_AS_ID_2_MASK;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_HIGH_5( "Current DS preferences: standby pref: %d, active subs: %d, priority subs %d, default voice: %d, default data: %d",
                 cmph_ptr()->user_standby_pref, cmph_ptr()->active_subs,
                	cmph_ptr()->priority_subs, cmph_ptr()->default_voice_subs, 
                        cmph_ptr()->default_data_subs );

    CM_MSG_HIGH_6( "New DS preferences: standby pref: %d, active subs: %d, priority subs %d,default voice: %d, default data: %d,dds_cause %d",
                 standby_pref, active_subs, priority_subs,
                 default_voice_subs, default_data_subs,dds_switch_type);

    /* Queue the command to CM.
    */
    cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  }

/*===========================================================================

FUNCTION cm_ph_cmd_activate_subs

DESCRIPTION
  Command CM to activate the subscriptions indicated by the given subs mask

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_activate_subs(

        cm_ph_cmd_cb_f_type                 cmd_cb_func,
            /* client callback function */

        void                                *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                    client_id,
            /* Requesting client */

        uint8                                subs_mask
            /* Subs mask required to be activated */

)
{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  /* Check the validity of new preferences.
  */
  if(!(INRANGE(subs_mask,SYS_MODEM_AS_ID_1_MASK, SYS_MODEM_AS_ID_ALL_ACTIVE_MASK)))
  {
    CM_MSG_HIGH_1("Invalid active subs mask, rejecting cmd, subs_mask %d", subs_mask);
    if( cmd_cb_func != NULL )
    {
        cmd_cb_func( data_block_ptr,
                     CM_PH_CMD_ACTIVATE_SUBS,
                     CM_PH_CMD_ERR_OTHER);
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr  = data_block_ptr;
  ph_cmd_ptr->client_id       = client_id;
  ph_cmd_ptr->cmd             = CM_PH_CMD_ACTIVATE_SUBS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->cmd_subs_mask        = subs_mask;


 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("Current preferences: active subs %d, New preferences: active subs %d",
                cmph_ptr()->active_subs, subs_mask);

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;
} //cm_ph_cmd_activate_subs

/*===========================================================================

FUNCTION cm_ph_cmd_deactivate_subs

DESCRIPTION
  Command CM to activate the subscriptions indicated by the given subs mask

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_deactivate_subs(

        cm_ph_cmd_cb_f_type                 cmd_cb_func,
            /* client callback function */

        void                                *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                    client_id,
            /* Requesting client */

        uint8                                subs_mask
            /* Subs mask required to be deactivated */

)
{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check the validity of new preferences.
  */
  if(!(INRANGE(subs_mask,SYS_MODEM_AS_ID_1_MASK, SYS_MODEM_AS_ID_ALL_ACTIVE_MASK)))
  {
    CM_MSG_HIGH_1("Invalid active subs mask, rejecting cmd, subs_mask %d", subs_mask);
    if( cmd_cb_func != NULL )
    {
        cmd_cb_func( data_block_ptr,
                     CM_PH_CMD_DEACTIVATE_SUBS,
                     CM_PH_CMD_ERR_OTHER);
    }
    return FALSE;
  }

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr  = data_block_ptr;
  ph_cmd_ptr->client_id       = client_id;
  ph_cmd_ptr->cmd             = CM_PH_CMD_DEACTIVATE_SUBS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->cmd_subs_mask        = subs_mask;


 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  CM_MSG_HIGH_2( "Current preferences: active subs: %d. New preferences: deactivated subs: %d", cmph_ptr()->active_subs, subs_mask); 

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;
}//cm_ph_cmd_deactivate_subs



/*===========================================================================

FUNCTION cm_ph_cmd_client_act

DESCRIPTION
  Inform CM that a client was activated so that CM can inform the client
  when the phone information become available.

  This is mostly important during powerup, where a client needs to get
  the phone information in order to make powerup decisions.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void cm_ph_cmd_client_act(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

  cm_client_id_type           client
            /* Requesting client */

)
{

  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_err_e_type    cmd_err = CM_PH_CMD_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command is not rejected, try allocating a CM command buffer.
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client;
    ph_cmd_ptr->cmd             = CM_PH_CMD_CLIENT_ACT;

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether any errors were found.
  */
  if( cmd_err != CM_PH_CMD_ERR_NOERR )
  {
    CM_ERR_1( "=CM= CM_PH_CMD_CLIENT_ACT, cm_err=%d", cmd_err);

    /* If client passed us a callback function, call on it now,
    ** informing the client of the command error.
    **
    ** NOTE that in case of NO errors we are not notifying the client
    ** now since errors might still occur once we actually try
    ** to process this command later.
    */
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_PH_CMD_CLIENT_ACT, cmd_err);
    }
  }

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION cm_ph_cmd_aoc_reset_acm_per_subs

DESCRIPTION
  This function sends a command to the CM task to reset the ACM to 0.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

boolean cm_ph_cmd_aoc_reset_acm_per_subs(

  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  sys_modem_as_id_e_type asubs_id
    /* Subscription ID of the ACM to reset.
    ** Only Valid in DualStandby
    ** In Single standby defaulted to active subs
    */
)
/*lint -esym(715,client_id) */
/*lint -esym(715,asubs_id) */
{
  #ifdef CM_GW_SUPPORTED
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;



  ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_RESET_ACM;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    cmd_info_ptr->cmd_subs      = asubs_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_RESET_ACM,
        CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif
}
/*lint +esym(715,client_id) */
/*lint +esym(715,asubs_id) */


/*===========================================================================

FUNCTION cm_ph_cmd_aoc_reset_acm

DESCRIPTION
  This function sends a command to the CM task to reset the ACM to 0.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

boolean cm_ph_cmd_aoc_reset_acm(
  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id
    /* Requesting client */
)
{
  return cm_ph_cmd_aoc_reset_acm_per_subs(cmd_cb_func,
                                          data_block_ptr,
                                          client_id,
                                          #ifdef CFG_HYBR_GW
                                          #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                          SYS_MODEM_AS_ID_3
                                          #else
                                          SYS_MODEM_AS_ID_1
                                          #endif
                                         );
}


/*===========================================================================
FUNCTION cm_ph_cmd_aoc_set_acmmax_per_subs

DESCRIPTION
  This function sends a message to CM to set the value of the maximum
  value the Accumulated Call Meter can achieve before calls are ended.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_ph_cmd_aoc_set_acmmax_per_subs(

  cm_ph_cmd_cb_f_type       cmd_cb_func,
    /* client callback function */

  void                      *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type         client_id,
    /* Requesting client */

  sys_modem_as_id_e_type    subs_id,
    /* Subscription ID of the ACM to set.
    ** Only Valid in DualStandby
    ** In Single standby defaulted to active subs
    */

  uint32                    new_max
)
/*lint -esym(715,client_id) */
/*lint -esym(715,new_max) */
{
  #ifdef CM_GW_SUPPORTED
  cm_ph_cmd_s_type      *ph_cmd_ptr;
  cm_ph_cmd_info_s_type *cmd_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1( "PH CMD - cm_ph_cmd_aoc_set_acmmax_per_subs(), newmax = %d",new_max);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    ph_cmd_ptr->data_block_ptr  = data_block_ptr;
    ph_cmd_ptr->client_id       = client_id;
    ph_cmd_ptr->cmd             = CM_PH_CMD_SET_ACMMAX;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->new_acmmax = new_max;
    cmd_info_ptr->cmd_subs   = subs_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

    return TRUE;
  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_PH_CMD_SET_ACMMAX,
          CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED(subs_id);
  return FALSE;
  #endif
}
/*lint +esym(715,client_id) */
/*lint +esym(715,new_max) */


/*===========================================================================
FUNCTION cm_ph_cmd_aoc_set_acmmax

DESCRIPTION
  This function sends a message to CM to set the value of the maximum
  value the Accumulated Call Meter can achieve before calls are ended.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_ph_cmd_aoc_set_acmmax(
  cm_ph_cmd_cb_f_type    cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* Requesting client */

  uint32                 new_max
)
{
  return cm_ph_cmd_aoc_set_acmmax_per_subs(cmd_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           #ifdef CFG_HYBR_GW
                                           #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                           SYS_MODEM_AS_ID_3,
                                           #else
                                           SYS_MODEM_AS_ID_1,
                                           #endif
                                           new_max
                                          );
}


/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_ccm_per_subs

DESCRIPTION
  This function returns the current value of the Current Call Meter.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The CCM value from the current or last call.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_ccm_per_subs(

  sys_modem_as_id_e_type            asubs_id
  /* Subscription ID to use for retrieving current max acm
  */
)
{
  #ifdef CM_GW_SUPPORTED
  cm_ss_e_type    ss =  CM_SS_MAIN;

  ss = cmph_map_subs_to_ss(asubs_id);

  if( CM_SS_NONE == ss )
  {
    return 0;
  }

  return cmwaoc_get_ccm(ss);
  #else
  SYS_ARG_NOT_USED(asubs_id);
  return 0;
  #endif
}

/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_ccm

DESCRIPTION
  This function returns the current value of the Current Call Meter.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The CCM value from the current or last call.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_ccm(void)
{
  return cm_ph_cmd_aoc_get_ccm_per_subs(
                                          #ifdef CFG_HYBR_GW
                                          #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                          SYS_MODEM_AS_ID_3
                                          #else
                                          SYS_MODEM_AS_ID_1
                                          #endif
                      );
}

/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_acm_per_subs

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The ACM value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_acm_per_subs(

  sys_modem_as_id_e_type                asubs_id
    /* Subscription ID to use for retrieving current acm
    */
)
{
  #ifdef CM_GW_SUPPORTED

  /* return the acm corresponding to the stack
  */
  return cmwaoc_get_acm(asubs_id);
  #else
  SYS_ARG_NOT_USED(asubs_id);
  return 0;
  #endif
}


/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_acm

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The ACM value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_acm(void)
{
  return cm_ph_cmd_aoc_get_acm_per_subs(
                                          #ifdef CFG_HYBR_GW
                                          #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                          SYS_MODEM_AS_ID_3
                                          #else
                                          SYS_MODEM_AS_ID_1
                                          #endif
                    );
}

/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_acmmax_per_subs

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter Max.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The ACMMax value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_acmmax_per_subs(

  sys_modem_as_id_e_type                asubs_id
  /* Subscription ID to use for retrieving current max acm
  */
)
{
  #ifdef CM_GW_SUPPORTED
  return cmwaoc_get_acmmax(asubs_id);
  #else
  SYS_ARG_NOT_USED(asubs_id);
  return 0;
  #endif
}

/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_acmmax

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter Max.

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The ACMMax value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_acmmax(void)
{
  return cm_ph_cmd_aoc_get_acmmax_per_subs(
                                            #ifdef CFG_HYBR_GW
                                            #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                            SYS_MODEM_AS_ID_3
                                            #else
                                            SYS_MODEM_AS_ID_1
                                            #endif
                    );
}

/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_puct_per_subs

DESCRIPTION
  This function returns the current value of the Price Unit Currency Conversion

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The puct value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_puct_per_subs(

  sys_modem_as_id_e_type    asubs_id
    /* Active subscription-id */

)
{
  SYS_ARG_NOT_USED(asubs_id);

  #ifdef CM_GW_SUPPORTED
  return cmwaoc_get_puct();
  #else
  return 0;
  #endif
}


/*===========================================================================

FUNCTION cm_ph_cmd_aoc_get_puct

DESCRIPTION
  This function returns the current value of the Price Unit Currency Conversion

DEPENDENCIES
  FEATURE_GSM || FEATURE_WCDMA Must be defined.

RETURN VALUE
  The puct value from the SIM.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cm_ph_cmd_aoc_get_puct(void)
{
  return cm_ph_cmd_aoc_get_puct_per_subs(
                                          #ifdef CFG_HYBR_GW
                                          #error code not present
#elif defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                          SYS_MODEM_AS_ID_3
                                          #else
                                          SYS_MODEM_AS_ID_1
                                          #endif
                    );
}


/*===========================================================================
FUNCTION cm_get_device_info

DESCRIPTION
  This function returns the current value of model info

DEPENDENCIES


RETURN VALUE
  model_info

SIDE EFFECTS
  None
===========================================================================*/
cm_device_model_info_efs *cm_get_device_info(void)
{
  return &cm_model_info;
}

/*===========================================================================
FUNCTION cm_set_apss_sw_version

DESCRIPTION
  This function updates the software version of model info

DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void cm_set_apss_sw_version
(
  sw_version_s_type *sw_version_info
)
{
  cm_device_model_info_efs *model_ptr = NULL;

  model_ptr  = cm_get_device_info();

  if ((sw_version_info == NULL) || (model_ptr == NULL))
  {
    CM_ERR_0("SW Version Info NULL or Feature not valid ");
    return;
  }

  /* memset the entire structure to NULL as this is the entry point */
  memset(model_ptr->sw_version,0x0,sizeof(model_ptr->sw_version));

  /*  sw version */
    cmutil_copy_char_array_to_str(model_ptr->sw_version, sizeof(model_ptr->sw_version), sw_version_info->sw_version, sw_version_info->length);


  CM_MSG_HIGH_3("Value of sw_version, 0x%x,0x%x,0x%x",
        model_ptr->sw_version[0],
        model_ptr->sw_version[1],
        model_ptr->sw_version[2]);

  if(cm_model_initialized == DEV_MODEL_INIT_PARTIAL)
  {
    cm_model_initialized = DEV_MODEL_INIT_FULL;
  }

  /*cmph_device_model_init(); will be called at cmph_init() */

  cmph_set_apss_sw_version_efs();

}

/*===========================================================================

FUNCTION cm_ph_cmd_update_cdma_capability

DESCRIPTION
  Command CM to update the CDMA2000 capability information in form of
  IS2000 Status message.This is used by the UE to advertise the CDMA
  capability to UTRAN using RRC connection setup complete or UE
  Capability Information messages.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_ph_cmd_update_cdma_capability(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                              *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  const sys_is2000_msg_s_type             *is2000_msg_ptr
    /* Pointer to the IS2000 message */

)
/*lint -esym(715, is2000_msg_ptr)*/
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func         = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr      = data_block_ptr;
  ph_cmd_ptr->client_id           = client_id;
  ph_cmd_ptr->cmd                 = CM_PH_CMD_UPDATE_CDMA_CAPABILITY;


  #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
      defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
  #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

} /* cm_ph_cmd_update_cdma_capability() */
/*lint +esym(715, is2000_msg_ptr)*/



/*===========================================================================

FUNCTION cm_ss_cmd_get_ss_info

DESCRIPTION
  Get a sanpshot of the current serving system information in the CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request


SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_ss_cmd_get_ss_info(

    cm_ss_cmd_cb_f_type     cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client
        /* requesting client ID */
)
{

  cm_ss_cmd_s_type          *ss_cmd_ptr;




  ss_cmd_ptr = cm_cmd_alloc_ss_init();


      /* Copy command header fields.
      */
   ss_cmd_ptr->cmd_cb_func     = cmd_cb_func;
   ss_cmd_ptr->data_block_ptr  = data_block_ptr;
   ss_cmd_ptr->client_id       = client;
   ss_cmd_ptr->cmd             = CM_SS_CMD_INFO_GET;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)ss_cmd_ptr );

    return TRUE;
}


boolean cm_ss_cmd_get_ss_info_msim(

    cm_ss_cmd_cb_f_type     cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    sys_modem_as_id_e_type  as_id
)
{

  cm_ss_cmd_s_type          *ss_cmd_ptr;




  ss_cmd_ptr = cm_cmd_alloc_ss_init();


      /* Copy command header fields.
      */
   ss_cmd_ptr->cmd_cb_func     = cmd_cb_func;
   ss_cmd_ptr->data_block_ptr  = data_block_ptr;
   ss_cmd_ptr->client_id       = client;
   if(as_id != SYS_MODEM_AS_ID_MAX)
   {
     ss_cmd_ptr->cmd_subs        = as_id;
   }
   ss_cmd_ptr->cmd             = CM_SS_CMD_INFO_GET;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)ss_cmd_ptr );

    return TRUE;
}


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_ss_cmd_get_dch_cell_info

DESCRIPTION
  Gets the CELL_INFO(Cell_ID, PLMN ID) when the UE is in DCH state.
  The client must register for CM_SS_EVENT_SRV_CHANGED in order to receive the
  response to this command. This event is sent to all clients that register
  for this event.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ss_cmd_get_dch_cell_info
(
    cm_ss_cmd_cb_f_type     cmd_cb_func,
    /* client callback function */

    void                    *data_block_ptr,
    /* pointer to client callback data block */

    cm_client_id_type       client_id
    /* Requesting client ID */

)
/*lint -esym(715,client_id)*/
{
  #if (defined(FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH) && defined (FEATURE_WCDMA))

  cm_ss_cmd_s_type          *ss_cmd_ptr = NULL;




  ss_cmd_ptr = cm_cmd_alloc_ss_init();


    /* Copy command header fields. */

  ss_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  ss_cmd_ptr->data_block_ptr  = data_block_ptr;
  ss_cmd_ptr->client_id       = client_id;
  ss_cmd_ptr->cmd             = CM_SS_CMD_GET_DCH_CELL_ID;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Queue the command to CM. */
  cm_cmd_queue( (cm_cmd_type*)ss_cmd_ptr );

  return TRUE;

  #else
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_SS_CMD_GET_DCH_CELL_ID,
      CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  CM_MSG_HIGH_0( "FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH: Feature not defined");
  return FALSE;
  #endif /* FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH */

}
/*lint +esym(715,client_id)*/


/*===========================================================================

FUNCTION cm_ss_cmd_get_measurement

DESCRIPTION
  Get measurements results for systems specified in the payload of this
  command. The client needs to register for CM_SS_EVENT_MEAS_RESPONSE in
  order to receive the response to this command. This event is sent to all
  clients that register for this event.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ss_cmd_get_measurement(

    cm_ss_cmd_cb_f_type     cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    cm_ss_meas_req_s_type    meas_req
        /* meas request information */

)
/*lint -esym(715,meas_req)*/
{
  #ifdef FEATURE_CM_SS_MEAS
#error code not present
#else /* FEATURE_CM_SS_MEAS */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_SS_CMD_GET_MEASUREMENT,
      CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  SYS_ARG_NOT_USED( data_block_ptr );
  SYS_ARG_NOT_USED( client );

  return FALSE;
  #endif /* FEATURE_CM_SS_MEAS */
}
/*lint +esym(715,meas_req)*/

/*===========================================================================

FUNCTION cm_ss_cmd_orig_thr_update_tbl

DESCRIPTION
  This API queues a command to CM to update the origination throttle table

DEPENDENCIES
  cmss_init() should have been called previously.

RETURN VALUE
  True if a command was queued
  False otherwise

SIDE EFFECTS
  None.
===========================================================================*/

boolean cm_ss_cmd_orig_thr_update_tbl
(

  cm_ss_cmd_cb_f_type               *ss_cmd_cb_func,
    /* Pointer to the call back function */

  void                              *data_block_ptr,
    /* Pointer to user data */

  cm_client_id_type                  client,
    /* requesting client ID */

  cm_ss_orig_thr_act_e_type          orig_thr_act,
    /* The action to be taken */

  cm_ss_orig_thr_reasons_e_type      call_status,
    /* The call end reason */

  cm_call_type_e_type                call_type,
    /* Type of the call, e.g PS_DATA */

  sys_sys_mode_e_type                mode,
    /* The mode on which the call was connected
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_CURR_SS
    */

  const sys_sys_id_s_type                 *sys_id,
    /* The sys id of the system, SID,NID
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_SS
    */

  sys_band_class_e_type              band_class,
    /* cellular or PCS
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_SS
    */

  uint16                             packet_zone
    /* The packet zone ID
    ** This field is only used if
    ** orig_thr_act = CM_SS_ORIG_THR_UPDATE_SS
    */

)
{
  SYS_ARG_NOT_USED( orig_thr_act );
  SYS_ARG_NOT_USED( call_status );
  SYS_ARG_NOT_USED( call_type );
  SYS_ARG_NOT_USED( mode );
  SYS_ARG_NOT_USED( band_class );
  SYS_ARG_NOT_USED( client );
  SYS_ARG_NOT_USED( sys_id );
  SYS_ARG_NOT_USED( packet_zone );

  #ifdef FEATURE_CALL_THROTTLE
  {

    cm_ss_cmd_s_type          *ss_cmd_ptr;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    if( orig_thr_act == CM_SS_ORIG_THR_UPDATE_SS )
    {
      CM_ASSERT( sys_id != NULL );
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_HIGH_3("cm_ss_cmd_orig_thr_update_tbl Thr: act %d, call status %d, mode %d",
                  orig_thr_act,
                                                       call_status,
                                                       mode );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    ss_cmd_ptr = cm_cmd_alloc_ss_init();

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initialize the command header fields
    */

    ss_cmd_ptr->info.call_info.sys_id.id_type = SYS_SYS_ID_TYPE_UNDEFINED;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */

    ss_cmd_ptr->cmd_cb_func                = ss_cmd_cb_func;
    ss_cmd_ptr->data_block_ptr             = data_block_ptr;
    ss_cmd_ptr->client_id                  = client;
    ss_cmd_ptr->info.orig_thr_act          = orig_thr_act;
    ss_cmd_ptr->cmd                        = CM_SS_CMD_ORIG_THR_UPDATE_TBL;
    ss_cmd_ptr->info.call_info.call_status = call_status;
    ss_cmd_ptr->info.call_info.call_type   = call_type;
    ss_cmd_ptr->info.mode                  = mode;
    ss_cmd_ptr->info.call_info.mode_info.cdma_info.band_class  = band_class;
    ss_cmd_ptr->info.call_info.mode_info.cdma_info.packet_zone = packet_zone;

    if( sys_id != NULL )
    {
      ss_cmd_ptr->info.call_info.sys_id    = *sys_id;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */

    cm_cmd_queue( (cm_cmd_type*)ss_cmd_ptr );
    return TRUE;

  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #else
  if( ss_cmd_cb_func != NULL )
  {
    ss_cmd_cb_func( data_block_ptr, CM_SS_CMD_ORIG_THR_UPDATE_TBL,
                CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif

}/* cm_ss_cmd_orig_thr_update_tbl */


/*===========================================================================

FUNCTION cm_mm_inband_cmd_burst_dtmf

DESCRIPTION
  Command CM to send a burst DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_inband_cmd_burst_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* Requesting client */

  cm_call_id_type             call_id,
    /* call ID of the call */

  uint8                       on_length,
    /* DTMF pulse width */

  uint8                       off_length,
    /* DTMF inter-digit interval */

  uint8                       cnt,
    /* DTMF digit count */

  const uint8                 *dtmf_digit_ptr
    /* Pointer to the DTMF digit buffer */

)
/*lint -esym(715, cnt) */
/*lint -esym(715, on_length) */
/*lint -esym(715, off_length) */
/*lint -esym(715, call_id) */
/*lint -esym(715, dtmf_digit_ptr) */
/*lint -esym(715, client_id) */
{
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cm_inband_cmd_s_type          *inband_cmd_ptr;
  cm_inband_cmd_info_s_type     *cmd_info_ptr;
  cm_inband_cmd_err_e_type      cmd_err=CM_INBAND_CMD_ERR_NOERR;




  inband_cmd_ptr = cm_cmd_alloc_inband_init();
      /* Get a reference to command information.
      */
  cmd_info_ptr    = CMD_INFO_PTR( inband_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
  inband_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  inband_cmd_ptr->data_block_ptr  = data_block_ptr;
  inband_cmd_ptr->client_id       = client_id;
  inband_cmd_ptr->cmd             = CM_INBAND_CMD_BURST_DTMF;
  cmd_info_ptr->call_id           = call_id;

   /* check null buffer pointer and maximum length related errors */

   /*lint -save -e685 Relational operator '>' always evaluates to 'false'
      ** because cnt is uint8 and CM_INBAND_MAX_REV_DIALED_DIGITS is 255 for JCDMA
      ** so cnt can't be bigger than that on JCDMA builds but it can on others
      */
   if( cnt > cm_get_inband_max_rev_dialed_digits() )
      /*lint -restore */
   {
     cmd_err = CM_INBAND_CMD_ERR_CNT_P;
   }
   else if( dtmf_digit_ptr == NULL )
   {
     cmd_err = CM_INBAND_CMD_ERR_NULL_BUF_P;
   }


   if (cmd_err == CM_INBAND_CMD_ERR_NOERR)
   {

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /*
        ** 668: possibly passing null ptr to function
        ** 669: possible data overrun
        ** Here we know dtmf_digit_ptr is not NULL (see check for
        ** CM_INBAND_CMD_ERR_NULL_BUF_P above) so turn off Lint warning 668
        ** Also we know here that cnt <= CM_INBAND_MAX_REV_DIALED_DIGITS
        ** so turn off Lint warning 669
        */
        /*lint -save -e668 -e669 )*/

        /* Copy command information fields.
        */
        cmd_info_ptr->on_length     = on_length;
        cmd_info_ptr->off_length    = off_length;
        cmd_info_ptr->cnt = memscpy( cmd_info_ptr->digits, cm_get_inband_max_rev_dialed_digits(), dtmf_digit_ptr, cnt );
        /*lint -restore */
   }
   else
   {
     cmd_info_ptr->cmd_err       = cmd_err;
   }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
  cm_cmd_queue( (cm_cmd_type*)inband_cmd_ptr );

  return TRUE;
  #else /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_INBAND_CMD_BURST_DTMF,
      CM_INBAND_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
}
/*lint +esym(715, cnt) */
/*lint +esym(715, on_length) */
/*lint +esym(715, off_length) */
/*lint +esym(715, call_id) */
/*lint +esym(715, dtmf_digit_ptr) */
/*lint +esym(715, client_id) */


/*===========================================================================

FUNCTION cm_mm_inband_cmd_start_cont_dtmf

DESCRIPTION
  Command CM to start a contonuous DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_inband_cmd_start_cont_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* Requesting client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  uint8                       digit
    /* DTMF digit */

)
{

  cm_inband_cmd_s_type          *inband_cmd_ptr;
  cm_inband_cmd_info_s_type     *cmd_info_ptr;


  inband_cmd_ptr = cm_cmd_alloc_inband_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr    = CMD_INFO_PTR( inband_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    inband_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    inband_cmd_ptr->data_block_ptr  = data_block_ptr;
    inband_cmd_ptr->client_id       = client_id;
    inband_cmd_ptr->cmd             = CM_INBAND_CMD_START_CONT_DTMF;
    cmd_info_ptr->call_id           = call_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->cnt           = 1;
    cmd_info_ptr->digits[0]     = digit;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)inband_cmd_ptr );

  return TRUE;

}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_mm_inband_cmd_stop_cont_dtmf

DESCRIPTION
  Command CM to stop a continuous DTMF

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_mm_inband_cmd_stop_cont_dtmf(

  cm_inband_cmd_cb_f_type     cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client_id,
    /* Requesting client */

  cm_call_id_type        call_id,
    /* call ID of the call */

  uint16                      duration
    /* DTMF duration */

)
{

  cm_inband_cmd_s_type          *inband_cmd_ptr;
  cm_inband_cmd_info_s_type     *cmd_info_ptr;


  inband_cmd_ptr = cm_cmd_alloc_inband_init();


  /* Get a reference to command information.
    */
    cmd_info_ptr    = CMD_INFO_PTR( inband_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    inband_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    inband_cmd_ptr->data_block_ptr  = data_block_ptr;
    inband_cmd_ptr->client_id       = client_id;
    inband_cmd_ptr->cmd             = CM_INBAND_CMD_STOP_CONT_DTMF;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */
    cmd_info_ptr->call_id         = call_id;
    cmd_info_ptr->duration        = duration;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)inband_cmd_ptr );

    return TRUE;
  }
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION cm_sms_cmd_mo_msg2

DESCRIPTION

     Send a mobile originated SMS message

DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE

  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  none

===========================================================================*/
boolean cm_sms_cmd_mo_msg2(

    cm_sms_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    boolean                 l2_ack_requested,
        /* l2 ack is required or not */

    const cm_sms_msg_type   *data,
        /* pointer to SMS buffer */

    cm_mode_pref_e_type     mode_pref
        /* The mode over which the SMS is sent */

)
{

  cm_sms_cmd_s_type          *sms_cmd_ptr;
  cm_sms_cmd_info_s_type     *cmd_info_ptr;


  sms_cmd_ptr = cm_cmd_alloc_sms_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr    = CMD_INFO_PTR( sms_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    sms_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    sms_cmd_ptr->data_block_ptr  = data_block_ptr;
    sms_cmd_ptr->client_id       = client;
    sms_cmd_ptr->cmd             = CM_SMS_CMD_MO_MSG;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */

    cmd_info_ptr->l2_ack_requested  = l2_ack_requested;

    /* Set the pointer to a static buffer to holding the MO SMS.
    */
    cmd_info_ptr->data = cmsms_sms_data_buffer_allocate();
    if( cmd_info_ptr->data == NULL )
    {
      if( cmd_cb_func != NULL )
      {
        cmd_cb_func( data_block_ptr, CM_SMS_CMD_MO_MSG, CM_SMS_CMD_ERR_NO_BUF_L);
      }

    cm_mem_free(sms_cmd_ptr);
      return FALSE;
    }


    /* Copy the SMS data buffer to the field.
    */
    cmsms_sms_data_buffer_fill( cmd_info_ptr->data, data );

    /* The mode pref over which the SMS is sent.
    */
    cmd_info_ptr->mode_pref = mode_pref;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Queue the command to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)sms_cmd_ptr );

  return TRUE;
}

/*===========================================================================

FUNCTION cm_sms_cmd_mo_msg

DESCRIPTION

     Send a mobile originated SMS message

DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE

  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS

  none

===========================================================================*/
boolean cm_sms_cmd_mo_msg(

    cm_sms_cmd_cb_f_type    cmd_cb_func,
        /* client callback function */

    void                    *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    boolean                 l2_ack_requested,
        /* l2 ack is required or not */

    const uint8             *data
        /* pointer to SMS buffer */


)
{
  /*lint -save -e826 */
  return( cm_sms_cmd_mo_msg2( cmd_cb_func,
                              data_block_ptr,
                              client,
                              l2_ack_requested,
                              (cm_sms_msg_type *)data,
                              CM_MODE_PREF_CDMA_ONLY) );
  /*lint -restore */
} /*cm_sms_cmd_mo_msg*/




/*===========================================================================

FUNCTION cm_sms_cmd_awi_ack

DESCRIPTION

  Send analog alert with info ack message to base station

DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request


SIDE EFFECTS
  none

===========================================================================*/
boolean cm_sms_cmd_awi_ack(

    cm_sms_cmd_cb_f_type  cmd_cb_func,
        /* client callback function */

    void                 *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type       client,
        /* requesting client ID */

    byte                   seq_no,
        /* sequence number */

    boolean                bf,
        /* begin/final flag */

    byte                   error_class,
        /* error class */

    byte                   status
        /* ack status */

)
/*lint -esym(715, client) */
/*lint -esym(715, seq_no) */
/*lint -esym(715, bf) */
/*lint -esym(715, error_class) */
/*lint -esym(715, status) */
{
    #ifdef FEATURE_CDSMS
    cm_sms_cmd_s_type          *sms_cmd_ptr;
    cm_sms_cmd_info_s_type     *cmd_info_ptr;



    sms_cmd_ptr = cm_cmd_alloc_sms_init();

      /* Get a reference to command information.
      */
      cmd_info_ptr    = CMD_INFO_PTR( sms_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      sms_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      sms_cmd_ptr->data_block_ptr  = data_block_ptr;
      sms_cmd_ptr->client_id       = client;
      sms_cmd_ptr->cmd             = CM_SMS_CMD_AWI_ACK;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */

      cmd_info_ptr->awi_ack.seq_no      = seq_no;
      cmd_info_ptr->awi_ack.bf          = bf;
      cmd_info_ptr->awi_ack.status      = status;
      cmd_info_ptr->awi_ack.error_class = error_class;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)sms_cmd_ptr );

    return TRUE;

    #else /* FEATURE_CDSMS */
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_SMS_CMD_MO_MSG,
        CM_SMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
    #endif /* FEATURE_CDSMS */
}
/*lint +esym(715, client) */
/*lint +esym(715, seq_no) */
/*lint +esym(715, bf) */
/*lint +esym(715, error_class) */
/*lint +esym(715, status) */


/*===========================================================================

FUNCTION cm_sms_cmd_bc_enable

DESCRIPTION

  Enable/disbale broadcast SMS

DEPENDENCIES

  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request


SIDE EFFECTS
  none

===========================================================================*/
boolean cm_sms_cmd_bc_enable(

    cm_sms_cmd_cb_f_type  cmd_cb_func,
        /* client callback function */

    void                  *data_block_ptr,
        /* pointer to client callback data block */

    cm_client_id_type     client,
        /* requesting client ID */

    boolean               bc_enable
        /* enable broadcast message */

)
/*lint -esym(715, client) */
/*lint -esym(715, bc_enable) */
{
    #if defined(FEATURE_CDSMS)
    cm_sms_cmd_s_type          *sms_cmd_ptr;
    cm_sms_cmd_info_s_type     *cmd_info_ptr;



    sms_cmd_ptr = cm_cmd_alloc_sms_init();

      /* Get a reference to command information.
      */
      cmd_info_ptr    = CMD_INFO_PTR( sms_cmd_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command header fields.
      */
      sms_cmd_ptr->cmd_cb_func     = cmd_cb_func;
      sms_cmd_ptr->data_block_ptr  = data_block_ptr;
      sms_cmd_ptr->client_id       = client;
      sms_cmd_ptr->cmd             = CM_SMS_CMD_BC_ENABLE;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Copy command information fields.
      */

      cmd_info_ptr->bc_enable = bc_enable;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Queue the command to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)sms_cmd_ptr );

    return TRUE;

    #else /* ( defined(FEATURE_CDSMS) ) */
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_SMS_CMD_MO_MSG,
        CM_SMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    return FALSE;
    #endif /* else of ( defined(FEATURE_CDSMS) ) */
}
/*lint +esym(715, client) */
/*lint +esym(715, bc_enable) */

#ifdef CM_GW_SUPPORTED
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif
#endif   /* FEATURE_GSM || FEATURE_WCDMA */

/*===========================================================================

FUNCTION cm_nas_cmd_req

DESCRIPTION
  Send a command to NAS.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_nas_cmd_req(

  cm_nas_cmd_cb_f_type                      cmd_cb_func,
    /* client callback function */

  void                                     *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                         client_id,
    /* requesting client ID */

  cm_nas_req_type                           nas_req

)
/*lint -esym(715,nas_req) */
/*lint -esym(715,client_id) */
{
  #if (defined (FEATURE_WMS_CM_NAS_INTERFACES) && defined CM_GW_SUPPORTED)
#error code not present
#else /* (FEATURE_WCDMA || FEATURE_GSM) */
  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_NAS_CMD_REQ,
      CM_NAS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /*(FEATURE_WCDMA || FEATURE_GSM) */
}
/*lint +esym(715,nas_req) */
/*lint +esym(715,client_id) */


/*===========================================================================

FUNCTION cm_is_valid_mode_band_pref

DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target and the PRL that is associated with the
  currently selected NAM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_is_valid_mode_band_pref(

    cm_mode_pref_e_type    mode_pref,
        /* Mode preference to check */

    cm_band_pref_e_type    band_pref
        /* Band preference to check */
)
{
  if(mode_pref == CM_MODE_PREF_NO_CHANGE)
  {
    mode_pref = cmph_ptr()->main_stack_info.pref_info.mode_pref;
  }
  if(band_pref == CM_BAND_PREF_NO_CHANGE)
  {
    band_pref = cmph_ptr()->main_stack_info.pref_info.band_pref;
  }
  return cmph_is_valid_mode_band_pref( mode_pref, band_pref );
}



/*===========================================================================

FUNCTION cm_is_valid_mode_band_pref_with_lte

DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target and the PRL that is associated with the
  currently selected NAM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_is_valid_mode_band_pref_with_lte(

    cm_mode_pref_e_type    mode_pref,
        /* Mode preference to check */

    cm_band_pref_e_type    band_pref,
        /* Band preference to check */

    sys_lte_band_mask_e_type    lte_band_pref
        /* LTE band preference to check */
)
{
  return (cmph_is_valid_mode_band_pref( mode_pref, band_pref ) ||
          cmph_is_valid_lte_mode_band_pref(mode_pref, lte_band_pref ));
}


/*===========================================================================

FUNCTION: cm_is_valid_mode_band_pref2

===========================================================================*/
/**
  Checks whether a specified combination of mode and band preference is
  supported by the current target and the PRL that is associated with the
  currently selected NAM.

@param[in] mode_pref     Mode preference to check.
@param[in] band_pref     2G/3G band preference to check.
@param[in] lte_band_pref LTE band preference to check.
@param[in] tds_band_pref TD-SCDMA band preference to check.

@return
  TRUE -- Specified combination of mode and band preference is supported. \n
  FALSE -- Specified combination of mode and band preference is not supported.

@dependencies
  None.
*/boolean cm_is_valid_mode_band_pref2(

    cm_mode_pref_e_type    mode_pref,
    cm_band_pref_e_type    band_pref,
    sys_lte_band_mask_e_type    lte_band_pref,
    cm_band_pref_e_type    tds_band_pref
)
/*~ FUNCTION cm_is_valid_mode_band_pref2 */
{

  return (cmph_is_valid_mode_band_pref( mode_pref, band_pref ) ||
          cmph_is_valid_lte_mode_band_pref(mode_pref, lte_band_pref ) ||
          cmph_is_valid_tds_mode_band_pref(mode_pref, tds_band_pref ));

}

/*===========================================================================
FUNCTION  cm_is_valid_mode_pref

DESCRIPTION
  Check whether a given mode preference is valid.

DEPENDENCIES
  None

RETURNS
  boolean: TRUE if mode is valid, FALSE if not.

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_is_valid_mode_pref(
  cm_mode_pref_e_type mode
)
/*lint -esym(715,mode) */
{
  return cm_is_valid_mode_pref_per_subs(mode, SYS_MODEM_AS_ID_1);
}
/*lint +esym(715,mode) */

/*===========================================================================
FUNCTION  cm_is_valid_mode_pref_per_subs

DESCRIPTION
  Check whether a given mode preference per subs is valid.

DEPENDENCIES
  None

RETURNS
  boolean: TRUE if mode is valid, FALSE if not.

SIDE EFFECTS
  None
===========================================================================*/
static boolean cm_is_valid_mode_pref_per_subs(
  cm_mode_pref_e_type     mode,
  sys_modem_as_id_e_type  asubs_id
)
/*lint -esym(715,mode) */
{
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  if(mode == CM_MODE_PREF_NO_CHANGE)
  {
    mode = CMPH_SS_MODE_PREF(asubs_id, cmph_ptr());
  }
  return cmph_is_valid_mode_pref( mode, asubs_id);
  #else
  return FALSE;
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
}
/*lint +esym(715,mode) */


/*===========================================================================

FUNCTION CM_GET_CUR_HYBR_STATUS

DESCRIPTION
  This function gets the current hybrid status

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE - hybrid operation is enabled, FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_get_cur_hybr_status(void)
{
  #ifdef FEATURE_HDR_HYBRID
  return cmss_is_hybr_status_on();
  #else
  return FALSE;
  #endif
}


/*===========================================================================

FUNCTION cm_is_sys_mode_digital

DESCRIPTION
  This function determines whether the given system mode is a digital\
  mode.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if the system mode is digital, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_sys_mode_digital(

  sys_sys_mode_e_type    sys_mode
    /* system mode to check */

)
{
  return (( sys_mode == SYS_SYS_MODE_CDMA  ) ||
          ( sys_mode == SYS_SYS_MODE_GSM   ) ||
          ( sys_mode == SYS_SYS_MODE_HDR   ) ||
          ( sys_mode == SYS_SYS_MODE_WCDMA ) ||
          ( sys_mode == SYS_SYS_MODE_GPS   ) ||
          ( sys_mode == SYS_SYS_MODE_LTE   ) ||
          ( sys_mode == SYS_SYS_MODE_TDS   ));

} /* cm_is_sys_mode_digital */


/**==========================================================================

@FUNCTION cm_sups_cmd_register_internal

@DESCRIPTION
 This API is used to send Register Supplementary Service request

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
boolean cm_sups_cmd_register_internal (

  cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
    /**< client callback function */

  cm_sups_cmd_exten_cb_f_type    sups_cmd_exten_cb_func,
    /**< client callback function */

  void                           *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type              client_id,
    /**< originating client */

  const cm_reg_ss_params_s_type  *reg_cmd_params,
    /**< pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ,
   /**< Initiating client category */

  boolean                        is_call_control_processing_complete,
   /* Indicates whether call control has already been completed */

  boolean                        is_modem_cc_api,
   /* Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type         asubs_id,
    /* Indicate which subscription this SUPS command is for */

  const cm_alpha_s_type          *alpha
)
{

  #if defined CM_GW_SUPPORTED || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
  cm_sups_cmd_s_type         *sups_cmd_ptr;
  cm_sups_cmd_info_s_type    *sups_cmd_info_ptr;
  cm_sups_cmd_err_cause_e_type  sups_cmd_err_cause;
  cm_alpha_s_type             cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;


  if( reg_cmd_params == NULL )
  {
    CM_MSG_HIGH_2( "reg params = %x", reg_cmd_params, dialed_digits);

    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_REGISTER,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }
    else if( sups_cmd_exten_cb_func != NULL )
    {
      sups_cmd_exten_cb_func( data_block_ptr,
                              CM_SUPS_CMD_REGISTER,
                              CM_SUPS_CMD_ERR_NULL_PARAM_P,
                              sups_cmd_err_cause,
                              cc_alpha);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->cmd_cc_cb_func  = sups_cmd_exten_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client_id;
  sups_cmd_ptr->client_categ    = orig_client_categ;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_REGISTER;
  sups_cmd_ptr->is_parsing_required = FALSE;
  sups_cmd_ptr->is_call_control_processing_complete
                                = is_call_control_processing_complete;
  sups_cmd_ptr->is_modem_cc_api = is_modem_cc_api;


  if( dialed_digits != NULL )
  {
    /* Copy dialled digits */
    *(&sups_cmd_ptr->dialed_digits) = *dialed_digits;
  }
  else
  {
    sups_cmd_ptr->dialed_digits.len = 0;
  }


  if( alpha != NULL )
  {
     *(&sups_cmd_ptr->cc_result_alpha) = *alpha;
     CM_MSG_HIGH_2 ("Alpha recevd: len=%d, dcs=%d", alpha->len, alpha->dcs);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */
  sups_cmd_info_ptr->ss_code             = reg_cmd_params->ss_code;
  sups_cmd_info_ptr->ss_ref              = reg_cmd_params->ss_ref;
  sups_cmd_info_ptr->basic_service       = reg_cmd_params->basic_service;
  sups_cmd_info_ptr->forwarded_to_number = reg_cmd_params->fwd_to_num;
  sups_cmd_info_ptr->forwarded_to_subaddress
                                         = reg_cmd_params->fwd_to_subaddr;
  sups_cmd_info_ptr->nr_timer            = reg_cmd_params->nr_timer;
  sups_cmd_info_ptr->cmd_subs_id         = asubs_id;
  sups_cmd_info_ptr->sups_time_info      = reg_cmd_params->sups_time_info;

  if (reg_cmd_params->ss_code == (byte)bil && reg_cmd_params->call_barring_num_list != NULL)
  {
    ref_cnt_obj_add_ref(reg_cmd_params->call_barring_num_list);
        CM_MSG_HIGH_0 ("Register: Added ref for call_barring_num_list");
    sups_cmd_info_ptr->call_barring_num_list = reg_cmd_params->call_barring_num_list;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;
  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  cm_sups_cmd_err_cause_e_type    sups_cmd_err_cause;
  cm_alpha_s_type                 cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;

  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( reg_cmd_params );
  SYS_ARG_NOT_USED( dialed_digits );
  SYS_ARG_NOT_USED( orig_client_categ );
  SYS_ARG_NOT_USED( is_call_control_processing_complete );
  SYS_ARG_NOT_USED( is_modem_cc_api);
  SYS_ARG_NOT_USED( asubs_id);

  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr,
              CM_SUPS_CMD_REGISTER,
              CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  else if( sups_cmd_exten_cb_func != NULL )
  {
    sups_cmd_exten_cb_func( data_block_ptr,
               CM_SUPS_CMD_REGISTER,
               CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
               sups_cmd_err_cause,
               cc_alpha);

  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */

}


/**==========================================================================

@FUNCTION cm_sups_cmd_activate_internal

@DESCRIPTION
  Internal function to send an Activate Supplementary Service request.

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
static boolean cm_sups_cmd_activate_internal(

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
    /* client callback function */

  cm_sups_cmd_exten_cb_f_type     sups_cmd_exten_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< originating client */

  const cm_act_ss_params_s_type   *act_cmd_params,
    /**< pointer to activate SS command data */

  const cm_num_s_type             *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type     orig_client_categ,
    /**< Initiating client category */

  boolean                         is_call_control_processing_complete,
    /**< Indicates whether call control has already been completed */

  boolean                         is_modem_cc_api,
    /**< Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type          asubs_id
    /**< Indicates the subscription on which this cmd has to be processed */

)
{

  #if defined CM_GW_SUPPORTED || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
  cm_sups_cmd_s_type              *sups_cmd_ptr;
  cm_sups_cmd_info_s_type         *sups_cmd_info_ptr;
  cm_sups_cmd_err_cause_e_type     sups_cmd_err_cause;
  cm_alpha_s_type                  cc_alpha;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;

  if( act_cmd_params == NULL )
  {
    CM_MSG_HIGH_1( "act params = %x", act_cmd_params);

    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_ACTIVATE,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }
    else if( sups_cmd_exten_cb_func != NULL )
    {
      sups_cmd_exten_cb_func( data_block_ptr,
                        CM_SUPS_CMD_ACTIVATE,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P,
                        sups_cmd_err_cause,
                        cc_alpha);

    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */


  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->cmd_cc_cb_func  = sups_cmd_exten_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client_id;
  sups_cmd_ptr->client_categ    = orig_client_categ;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_ACTIVATE;
  sups_cmd_ptr->is_parsing_required = FALSE;
  /*lint -e{418} keep it off for general / specific walks as well */
  sups_cmd_ptr->is_call_control_processing_complete
                                = is_call_control_processing_complete;
  sups_cmd_ptr->is_modem_cc_api = is_modem_cc_api;

  if (act_cmd_params->ss_code == (byte)bil && act_cmd_params->call_barring_num_list != NULL)
  {
    ref_cnt_obj_add_ref(act_cmd_params->call_barring_num_list);
        CM_MSG_HIGH_0 ("Activate: Added ref for call_barring_num_list");
    sups_cmd_info_ptr->call_barring_num_list = act_cmd_params->call_barring_num_list;
  }

  if(dialed_digits != NULL)
  {
    /* Copy dialed digits */
    *(&sups_cmd_ptr->dialed_digits) = *dialed_digits;
    sups_cmd_info_ptr->pi = dialed_digits->pi;
  }
  else
  {
    sups_cmd_ptr->dialed_digits.len = 0;
    sups_cmd_info_ptr->pi = CM_PRESENTATION_ALLOWED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */
  sups_cmd_info_ptr->ss_code        = act_cmd_params->ss_code;
  sups_cmd_info_ptr->ss_ref         = act_cmd_params->ss_ref;
  sups_cmd_info_ptr->basic_service  = act_cmd_params->basic_service;
  sups_cmd_info_ptr->ss_password    = act_cmd_params->ss_pwd;
  sups_cmd_info_ptr->cmd_subs_id    = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  cm_sups_cmd_err_cause_e_type    sups_cmd_err_cause;
  cm_alpha_s_type                 cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;
  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( act_cmd_params );
  SYS_ARG_NOT_USED( dialed_digits );
  SYS_ARG_NOT_USED( orig_client_categ );
  SYS_ARG_NOT_USED( is_call_control_processing_complete );
  SYS_ARG_NOT_USED( is_modem_cc_api);
  SYS_ARG_NOT_USED( asubs_id );

  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_ACTIVATE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  else if( sups_cmd_exten_cb_func != NULL )
  {
    sups_cmd_exten_cb_func( data_block_ptr,
               CM_SUPS_CMD_ACTIVATE,
               CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
               sups_cmd_err_cause,
               cc_alpha);

  }
  return FALSE;
#endif /* else of defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */

}


/**==========================================================================

@FUNCTION cm_sups_cmd_reg_password_internal

@DESCRIPTION
 This is internal function to to send Register Password Supplementary
 Service request.

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
static boolean cm_sups_cmd_reg_password_internal (

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
    /**< client callback function */

  cm_sups_cmd_exten_cb_f_type       sups_cmd_exten_cb_func,
    /**< client callback function */

  void                              *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                 client_id,
    /**< originating client */

  const cm_reg_pwd_params_s_type    *reg_pwd_cmd_params,
    /**< pointer to register password SS command data */

  const cm_num_s_type               *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type       orig_client_categ,
   /**< Initiating client category */

  boolean                           is_call_control_processing_complete,
    /**< Indicates whether call control has already been completed */

  boolean                           is_modem_cc_api,
    /**< Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type            asubs_id
    /**< Indicates the subscription on which the cmd is to be handled */

)
{
   #ifdef CM_GW_SUPPORTED
   cm_sups_cmd_s_type         *sups_cmd_ptr;
   cm_sups_cmd_info_s_type    *sups_cmd_info_ptr;
   cm_sups_cmd_err_cause_e_type sups_cmd_err_cause;
   cm_alpha_s_type             cc_alpha;
   cc_alpha.len = 0;
   sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;


   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   if( reg_pwd_cmd_params == NULL )
   {
     CM_MSG_HIGH_1( "reg pwd params = %x", reg_pwd_cmd_params);

     if( sups_cmd_cb_func != NULL )
     {
       sups_cmd_cb_func( data_block_ptr,
                         CM_SUPS_CMD_REG_PASSWORD,
                         CM_SUPS_CMD_ERR_NULL_PARAM_P);
     }
     else if( sups_cmd_exten_cb_func != NULL )
     {
       sups_cmd_exten_cb_func( data_block_ptr,
                         CM_SUPS_CMD_REG_PASSWORD,
                         CM_SUPS_CMD_ERR_NULL_PARAM_P,
                         sups_cmd_err_cause,
                         cc_alpha);
     }

     return FALSE;
   }
   /* Allocate a command buffer for the Supplementary Service Request */

   sups_cmd_ptr = cm_cmd_alloc_sups_init();

   /* Get a reference
      to command information. */

   sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Copy command header fields. */

   sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
   sups_cmd_ptr->cmd_cc_cb_func  = sups_cmd_exten_cb_func;
   sups_cmd_ptr->data_block_ptr  = data_block_ptr;
   sups_cmd_ptr->client_id       = client_id;
   sups_cmd_ptr->client_categ    = orig_client_categ;
   sups_cmd_ptr->cmd             = CM_SUPS_CMD_REG_PASSWORD;
   sups_cmd_ptr->is_parsing_required = FALSE;
   sups_cmd_ptr->is_call_control_processing_complete
                                       = is_call_control_processing_complete;
   sups_cmd_ptr->is_modem_cc_api = is_modem_cc_api;

   if( dialed_digits != NULL )
   {
     /* Copy dialled digits */
     *(&sups_cmd_ptr->dialed_digits) = *dialed_digits;
   }
   else
   {
     sups_cmd_ptr->dialed_digits.len = 0;
   }

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */
   sups_cmd_info_ptr->code            = reg_pwd_cmd_params->ss_code;
   sups_cmd_info_ptr->ss_ref          = reg_pwd_cmd_params->ss_ref;
   sups_cmd_info_ptr->ss_password     = reg_pwd_cmd_params->ss_old_pwd;
   sups_cmd_info_ptr->ss_new_password = reg_pwd_cmd_params->ss_new_pwd;
   sups_cmd_info_ptr->ss_new_password_again
                                      = reg_pwd_cmd_params->ss_new_pwd_again;
   sups_cmd_info_ptr->cmd_subs_id     = asubs_id;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Queue the command to CM.*/
   cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

   return TRUE;

   #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
   cm_sups_cmd_err_cause_e_type    sups_cmd_err_cause;
   cm_alpha_s_type                 cc_alpha;
   cc_alpha.len = 0;
   sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;

   SYS_ARG_NOT_USED( client_id );
   SYS_ARG_NOT_USED( reg_pwd_cmd_params );
   SYS_ARG_NOT_USED( dialed_digits );
   SYS_ARG_NOT_USED( orig_client_categ );
   SYS_ARG_NOT_USED( is_call_control_processing_complete );
   SYS_ARG_NOT_USED( is_modem_cc_api);
   SYS_ARG_NOT_USED( asubs_id );

   if( sups_cmd_cb_func != NULL )
   {
     sups_cmd_cb_func( data_block_ptr,
                       CM_SUPS_CMD_REG_PASSWORD,
              CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
   }
   else if( sups_cmd_exten_cb_func != NULL )
   {
     sups_cmd_exten_cb_func( data_block_ptr,
               CM_SUPS_CMD_REG_PASSWORD,
               CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
               sups_cmd_err_cause,
               cc_alpha);
   }
   return FALSE;
   #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */

}



/**==========================================================================

@FUNCTION cm_sups_cmd_deactivate_internal

@DESCRIPTION
  This is internal function to send Deactivate Supplementary Service request.

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

static boolean cm_sups_cmd_deactivate_internal (

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
    /**< client callback function */

  cm_sups_cmd_exten_cb_f_type         sups_cmd_exten_cb_func,
    /**< client callback function */

  void                                *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                   client_id,
    /**< originating client */

  const cm_deact_ss_params_s_type     *deact_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type                 *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type         orig_client_categ,
    /**< Initiating client category */

  boolean                             is_call_control_processing_complete,
    /**< Indicates whether call control has already been completed */

  boolean                             is_modem_cc_api,
    /**< Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type              asubs_id
    /**< Indicates the subscription on which the cmd is to be handled */

)
{
  #if defined CM_GW_SUPPORTED || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
  cm_sups_cmd_s_type         *sups_cmd_ptr;
  cm_sups_cmd_info_s_type    *sups_cmd_info_ptr;
  cm_sups_cmd_err_cause_e_type  sups_cmd_err_cause;
  cm_alpha_s_type             cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( deact_cmd_params == NULL )
  {
    CM_MSG_HIGH_1( "deact params = %x", deact_cmd_params);

    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_DEACTIVATE,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }
    else if( sups_cmd_exten_cb_func != NULL )
    {
      sups_cmd_exten_cb_func( data_block_ptr,
                        CM_SUPS_CMD_DEACTIVATE,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P,
                        sups_cmd_err_cause,
                        cc_alpha);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->cmd_cc_cb_func  = sups_cmd_exten_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client_id;
  sups_cmd_ptr->client_categ    = orig_client_categ;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_DEACTIVATE;
  sups_cmd_ptr->is_parsing_required = FALSE;
  sups_cmd_ptr->is_call_control_processing_complete
                                       = is_call_control_processing_complete;
  sups_cmd_ptr->is_modem_cc_api = is_modem_cc_api;

  if (deact_cmd_params->ss_code == (byte)bil && deact_cmd_params->call_barring_num_list != NULL)
  {
    ref_cnt_obj_add_ref(deact_cmd_params->call_barring_num_list);
        CM_MSG_HIGH_0 ("Deactivate: Added ref for call_barring_num_list");
    sups_cmd_info_ptr->call_barring_num_list = deact_cmd_params->call_barring_num_list;
  }

  if( dialed_digits != NULL )
  {
    /* Copy dialled digits */
    *(&sups_cmd_ptr->dialed_digits) = *dialed_digits;
    sups_cmd_info_ptr->pi = dialed_digits->pi;
  }
  else
  {
    sups_cmd_ptr->dialed_digits.len = 0;
    sups_cmd_info_ptr->pi = CM_PRESENTATION_ALLOWED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */
  sups_cmd_info_ptr->ss_code        = deact_cmd_params->ss_code;
  sups_cmd_info_ptr->ss_ref         = deact_cmd_params->ss_ref;
  sups_cmd_info_ptr->basic_service  = deact_cmd_params->basic_service;
  sups_cmd_info_ptr->ss_password    = deact_cmd_params->ss_pwd;
  sups_cmd_info_ptr->cmd_subs_id    = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if ss_code is ccbs then deactivation gets converted into EraseCCEntry
  ** request 24.093 section 4.4
  */
  if (sups_cmd_info_ptr->ss_code == ccbs) /*lint !e641 */
  {
    sups_cmd_ptr->cmd                 = CM_SUPS_CMD_ERASE_CC_ENTRY;
    sups_cmd_ptr->cmd_info.ccbs_indx  = deact_cmd_params->ccbs_indx;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  cm_sups_cmd_err_cause_e_type    sups_cmd_err_cause;
  cm_alpha_s_type                 cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;

  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( deact_cmd_params );
  SYS_ARG_NOT_USED( dialed_digits );
  SYS_ARG_NOT_USED( orig_client_categ );
  SYS_ARG_NOT_USED( is_call_control_processing_complete );
  SYS_ARG_NOT_USED( is_modem_cc_api );
  SYS_ARG_NOT_USED( asubs_id );

  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_DEACTIVATE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  else if( sups_cmd_exten_cb_func != NULL )
  {
    sups_cmd_exten_cb_func( data_block_ptr, CM_SUPS_CMD_DEACTIVATE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
      sups_cmd_err_cause,
      cc_alpha);
  }
  return FALSE;
#endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */


}



/**==========================================================================

@FUNCTION cm_sups_cmd_erase_internal

@DESCRIPTION
  This is internal function to send Erase Supplementary Service request.

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
static boolean cm_sups_cmd_erase_internal (

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
    /**< client callback function */

  cm_sups_cmd_exten_cb_f_type       sups_cmd_exten_cb_func,
    /**< client callback function */

  void                              *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                 client_id,
    /**< originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type       orig_client_categ,
   /**< Initiating client category */

  boolean                           is_call_control_processing_complete,
    /**< Indicates whether call control has already been completed */

  boolean                           is_modem_cc_api,
    /**< Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type            asubs_id
    /**< Indicates the subscription on which this command is issued */

)
{

  #if defined CM_GW_SUPPORTED || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
  cm_sups_cmd_s_type         *sups_cmd_ptr;
  cm_sups_cmd_info_s_type    *sups_cmd_info_ptr;
  cm_sups_cmd_err_cause_e_type  sups_cmd_err_cause;
  cm_alpha_s_type             cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( erase_cmd_params == NULL )
  {
    CM_MSG_HIGH_1( "erase params = %x", erase_cmd_params);

    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_ERASE,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }
    else if( sups_cmd_exten_cb_func != NULL )
    {
      sups_cmd_exten_cb_func( data_block_ptr,
                        CM_SUPS_CMD_ERASE,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P,
                        sups_cmd_err_cause,
                        cc_alpha);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->cmd_cc_cb_func  = sups_cmd_exten_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client_id;
  sups_cmd_ptr->client_categ    = orig_client_categ;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_ERASE;
  sups_cmd_ptr->is_parsing_required = FALSE;
  /*lint -e{418} keep it suppressed for general / specific walks as well */
  sups_cmd_ptr->is_call_control_processing_complete
                                       = is_call_control_processing_complete;
  sups_cmd_ptr->is_modem_cc_api = is_modem_cc_api;

  if( dialed_digits != NULL )
  {
    /* Copy dialled digits */
    *(&sups_cmd_ptr->dialed_digits) = *dialed_digits;
  }
  else
  {
    sups_cmd_ptr->dialed_digits.len = 0;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */
  /*lint -save -e668 */
  sups_cmd_info_ptr->ss_code       = erase_cmd_params->ss_code;
  sups_cmd_info_ptr->ss_ref        = erase_cmd_params->ss_ref;
  sups_cmd_info_ptr->basic_service = erase_cmd_params->basic_service;
  sups_cmd_info_ptr->cmd_subs_id   = asubs_id;
  /*lint -restore */

  if (erase_cmd_params->ss_code == (byte)bil && erase_cmd_params->call_barring_num_list != NULL)
  {
    ref_cnt_obj_add_ref(erase_cmd_params->call_barring_num_list);
        CM_MSG_HIGH_0 ("Erase: Added ref for call_barring_num_list");
    sups_cmd_info_ptr->call_barring_num_list = erase_cmd_params->call_barring_num_list;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  cm_sups_cmd_err_cause_e_type    sups_cmd_err_cause;
  cm_alpha_s_type                 cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;

  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( erase_cmd_params );
  SYS_ARG_NOT_USED( dialed_digits );
  SYS_ARG_NOT_USED( orig_client_categ );
  SYS_ARG_NOT_USED( is_call_control_processing_complete );
  SYS_ARG_NOT_USED( is_modem_cc_api);
  SYS_ARG_NOT_USED( asubs_id);

  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_ERASE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  else if( sups_cmd_exten_cb_func != NULL )
  {
    sups_cmd_exten_cb_func( data_block_ptr, CM_SUPS_CMD_ERASE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
      sups_cmd_err_cause,
      cc_alpha);
  }
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */

}



/**==========================================================================

@FUNCTION cm_sups_cmd_interrogate_internal

@DESCRIPTION
  This is internal function to send Interrogate Supplementary Service request

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
static boolean cm_sups_cmd_interrogate_internal (

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
    /**< client callback function */

  cm_sups_cmd_exten_cb_f_type         sups_cmd_exten_cb_func,
    /**< client callback function */

  void                                *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                   client_id,
    /**< originating client */

  const cm_interr_ss_params_s_type    *interr_cmd_params,
    /**< pointer to interrogate SS command data */

  const cm_num_s_type                 *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type         orig_client_categ,
    /**< Initiating client category */

  boolean                             is_call_control_processing_complete,
    /**< Indicates whether call control has already been completed */

  boolean                             is_modem_cc_api,
    /**< Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type              asubs_id
    /**< Indicates the subscription on which this command is to be processed */

)
{

  #if defined CM_GW_SUPPORTED || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
  cm_sups_cmd_s_type         *sups_cmd_ptr;
  cm_sups_cmd_info_s_type    *sups_cmd_info_ptr;
  cm_sups_cmd_err_cause_e_type  sups_cmd_err_cause;
  cm_alpha_s_type             cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( interr_cmd_params == NULL )
  {
    CM_MSG_HIGH_1( "inter params = %x", interr_cmd_params);

    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                       CM_SUPS_CMD_INTERROGATE,
                       CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }
    else if( sups_cmd_exten_cb_func != NULL )
    {
      sups_cmd_exten_cb_func( data_block_ptr,
                              CM_SUPS_CMD_INTERROGATE,
                              CM_SUPS_CMD_ERR_NULL_PARAM_P,
                              sups_cmd_err_cause,
                              cc_alpha);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
    to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->cmd_cc_cb_func  = sups_cmd_exten_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client_id;
  sups_cmd_ptr->client_categ    = orig_client_categ;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_INTERROGATE;
  sups_cmd_ptr->is_parsing_required = FALSE;
  sups_cmd_ptr->is_call_control_processing_complete
                                     = is_call_control_processing_complete;
  sups_cmd_ptr->is_modem_cc_api = is_modem_cc_api;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( dialed_digits != NULL )
  {
   /* Copy dialled digits */
   *(&sups_cmd_ptr->dialed_digits) = *dialed_digits;
  }
  else
  {
   sups_cmd_ptr->dialed_digits.len = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */
  sups_cmd_info_ptr->ss_code        = interr_cmd_params->ss_code;
  sups_cmd_info_ptr->ss_ref         = interr_cmd_params->ss_ref;
  sups_cmd_info_ptr->basic_service  = interr_cmd_params->basic_service;
  sups_cmd_info_ptr->cmd_subs_id    = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  cm_sups_cmd_err_cause_e_type    sups_cmd_err_cause;
  cm_alpha_s_type                 cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;

  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( interr_cmd_params );
  SYS_ARG_NOT_USED( dialed_digits );
  SYS_ARG_NOT_USED( orig_client_categ );
  SYS_ARG_NOT_USED( is_call_control_processing_complete );
  SYS_ARG_NOT_USED( is_modem_cc_api);
  SYS_ARG_NOT_USED( asubs_id );

  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_INTERROGATE,
     CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  else if( sups_cmd_exten_cb_func != NULL )
  {
    sups_cmd_exten_cb_func( data_block_ptr, CM_SUPS_CMD_INTERROGATE,
     CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
     sups_cmd_err_cause,
     cc_alpha);
  }

  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
}



/**==========================================================================

@FUNCTION cm_sups_cmd_lcs_molr_internal

@DESCRIPTION
  This is internal function to send Mobile Originated Location Request
  Supplementary Service request

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
static boolean cm_sups_cmd_lcs_molr_internal (

  cm_sups_cmd_cb_f_type                 sups_cmd_cb_func,
    /**< client callback function */

  cm_sups_cmd_exten_cb_f_type           sups_cmd_exten_cb_func,
    /**< client callback function */

  void                                  *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                     client_id,
    /**< originating client */

  const cm_lcs_molr_ss_params_s_type    *lcs_molr_cmd_params,
    /**< pointer to lcs_molr SS command data */

  const cm_num_s_type                   *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type           orig_client_categ,
    /**< Initiating client category */

  boolean                               is_call_control_processing_complete,
    /**< Indicates whether call control has already been completed */

  boolean                               is_modem_cc_api,
    /**< Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type                asubs_id
    /**< Indicates the subscription on which this command is to be processed */

)
{

  #if defined CM_GW_SUPPORTED || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
  cm_sups_cmd_s_type            *sups_cmd_ptr;
  cm_sups_cmd_info_s_type       *sups_cmd_info_ptr;
  cm_sups_cmd_err_cause_e_type  sups_cmd_err_cause;
  cm_alpha_s_type               cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( lcs_molr_cmd_params == NULL )
  {
    CM_MSG_HIGH_1( "lcs molr params = %x", lcs_molr_cmd_params);

    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                       CM_SUPS_CMD_LCS_MOLR,
                       CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }
    else if( sups_cmd_exten_cb_func != NULL )
    {
      sups_cmd_exten_cb_func( data_block_ptr,
                              CM_SUPS_CMD_LCS_MOLR,
                              CM_SUPS_CMD_ERR_NULL_PARAM_P,
                              sups_cmd_err_cause,
                              cc_alpha);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
    to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->cmd_cc_cb_func  = sups_cmd_exten_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client_id;
  sups_cmd_ptr->client_categ    = orig_client_categ;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_LCS_MOLR;
  sups_cmd_ptr->is_parsing_required = FALSE;
  sups_cmd_ptr->is_call_control_processing_complete
                                     = is_call_control_processing_complete;
  sups_cmd_ptr->is_modem_cc_api = is_modem_cc_api;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( dialed_digits != NULL )
  {
   /* Copy dialled digits */
   *(&(sups_cmd_ptr->dialed_digits)) = *dialed_digits;
  }
  else
  {
   sups_cmd_ptr->dialed_digits.len = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */
  sups_cmd_info_ptr->ss_ref         = lcs_molr_cmd_params->ss_ref;
  sups_cmd_info_ptr->basic_service  = lcs_molr_cmd_params->basic_service;
  sups_cmd_info_ptr->molr_type      = lcs_molr_cmd_params->molr_type;
  sups_cmd_info_ptr->lcs_qos        = lcs_molr_cmd_params->lcs_qos;
  sups_cmd_info_ptr->cmd_subs_id    = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  cm_sups_cmd_err_cause_e_type    sups_cmd_err_cause;
  cm_alpha_s_type                 cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;

  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( lcs_molr_cmd_params );
  SYS_ARG_NOT_USED( dialed_digits );
  SYS_ARG_NOT_USED( orig_client_categ );
  SYS_ARG_NOT_USED( is_call_control_processing_complete );
  SYS_ARG_NOT_USED( is_modem_cc_api);
  SYS_ARG_NOT_USED( asubs_id );

  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_LCS_MOLR,
     CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  else if( sups_cmd_exten_cb_func != NULL )
  {
    sups_cmd_exten_cb_func( data_block_ptr, CM_SUPS_CMD_LCS_MOLR,
     CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
     sups_cmd_err_cause,
     cc_alpha);
  }

  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
}


/**===========================================================================

@FUNCTION cm_sups_cmd_process_uss_internal

@DESCRIPTION
  This is internal function to send Process Unstructured Supplementary
  Service request

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

 boolean cm_sups_cmd_process_uss_internal (

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
    /**< client callback function */

  cm_sups_cmd_exten_cb_f_type     sups_cmd_exten_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client_id,
    /**< originating client */

  const cm_uss_params_s_type      *process_uss_cmd_params,
    /**< pointer to Process Unstructured SS command data */

  const cm_num_s_type             *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type     orig_client_categ,
    /**< Initiating client category */

  boolean                         is_call_control_processing_complete,
    /**< Indicates whether call control has already been completed */

  boolean                         is_modem_cc_api,
    /**< Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type          asubs_id,
    /**< Indicates the subscription on which this command is to be processed */

  const cm_alpha_s_type           *alpha

)
{
  #ifdef CM_GW_SUPPORTED
  cm_sups_cmd_s_type           *sups_cmd_ptr;
  cm_sups_cmd_info_s_type      *sups_cmd_info_ptr;
  cm_sups_cmd_err_cause_e_type  sups_cmd_err_cause;
  cm_alpha_s_type               cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( process_uss_cmd_params == NULL )
  {
    CM_MSG_HIGH_1( "process uss params = %x", process_uss_cmd_params);

    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_PROCESS_USS,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }
    else if( sups_cmd_exten_cb_func != NULL )
    {
      sups_cmd_exten_cb_func( data_block_ptr,
                        CM_SUPS_CMD_PROCESS_USS,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P,
                        sups_cmd_err_cause,
                        cc_alpha);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->cmd_cc_cb_func     = sups_cmd_exten_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client_id;
  sups_cmd_ptr->client_categ    = orig_client_categ;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_PROCESS_USS;
  sups_cmd_ptr->is_parsing_required = FALSE;
  /*lint -e{418} keep it off for general / specific walks as well */
  sups_cmd_ptr->is_call_control_processing_complete
                                       = is_call_control_processing_complete;
  sups_cmd_ptr->is_modem_cc_api = is_modem_cc_api;

  if( dialed_digits != NULL )
  {
    /* Copy dialled digits */
    *(&sups_cmd_ptr->dialed_digits) = *dialed_digits;
  }
  else
  {
    sups_cmd_ptr->dialed_digits.len = 0;
  }


  if( alpha != NULL )
  {
     *(&sups_cmd_ptr->cc_result_alpha) = *alpha;
     CM_MSG_HIGH_2 ("Alpha recevd:, len=%d,dcs=%d",alpha->len,alpha->dcs);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */

  sups_cmd_info_ptr->ss_ref       = process_uss_cmd_params->ss_ref;
  sups_cmd_info_ptr->uss_data     = process_uss_cmd_params->uss_data;
  sups_cmd_info_ptr->ss_operation = processUnstructuredSS_Request;
  sups_cmd_info_ptr->cmd_subs_id  = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  cm_sups_cmd_err_cause_e_type    sups_cmd_err_cause;
  cm_alpha_s_type                 cc_alpha;
  cc_alpha.len = 0;
  sups_cmd_err_cause = CM_SUPS_CMD_ERR_CAUSE_NO_ERROR;

  SYS_ARG_NOT_USED( client_id );
  SYS_ARG_NOT_USED( process_uss_cmd_params );
  SYS_ARG_NOT_USED( dialed_digits );
  SYS_ARG_NOT_USED( orig_client_categ );
  SYS_ARG_NOT_USED( is_call_control_processing_complete );
  SYS_ARG_NOT_USED( is_modem_cc_api);
  SYS_ARG_NOT_USED( asubs_id );

  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_PROCESS_USS,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  else if( sups_cmd_exten_cb_func != NULL )
  {
    sups_cmd_exten_cb_func( data_block_ptr, CM_SUPS_CMD_PROCESS_USS,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE,
      sups_cmd_err_cause,
      cc_alpha);
  }

  return FALSE;
#endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */

}


/*===========================================================================

FUNCTION cm_sups_cmd_register

DESCRIPTION
  RegisterSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_register(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type          client,
            /* originating client */

        const cm_reg_ss_params_s_type    *reg_cmd_params
            /* pointer to register SS command data */

)
{
  return cm_sups_cmd_register_cc( sups_cmd_cb_func,
                                  data_block_ptr,
                                  client,
                                  reg_cmd_params,
                                  NULL,
                                  TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_register_cc

DESCRIPTION
  RegisterSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_register_cc(

  cm_sups_cmd_cb_f_type     sups_cmd_cb_func,
    /* client callback function */

  void                      *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type         client,
    /* originating client */

  const cm_reg_ss_params_s_type   *reg_cmd_params,
    /* pointer to register SS command data */

  const cm_num_s_type             *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                   is_call_control_processing_complete
    /* Indicates whether call control has already been coompleted */

)
{

  return cm_sups_cmd_register_internal ( sups_cmd_cb_func,
                                         NULL, /* New fn ptr NULL */
                                         data_block_ptr,
                                         client,
                                         reg_cmd_params,
                                         dialed_digits,
                                         CM_ORIG_CLIENT_CATEG_USER,
                                         is_call_control_processing_complete,
                                         FALSE, /* Old CM API */
                                         #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                         SYS_MODEM_AS_ID_3,
                                         #elif defined(CFG_HYBR_GW) || \
                                             defined(CFG_CG__HYBR_GW)
                                         SYS_MODEM_AS_ID_2,
                                         #else
                                         SYS_MODEM_AS_ID_1,
                                         #endif
                                         NULL
                                       );

}  /* cm_sups_cmd_register_cc */


/*===========================================================================
FUNCTION cm_sups_cmd_register_cc_per_subs

DESCRIPTION
  RegisterSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_register_cc_per_subs(

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
    /* client callback function */

  void                            *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client,
    /* originating client */

  const cm_reg_ss_params_s_type   *reg_cmd_params,
    /* pointer to register SS command data */

  const cm_num_s_type             *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                         is_call_control_processing_complete,
    /* Indicates whether call control has already been coompleted */

  sys_modem_as_id_e_type          asubs_id

)
{
  return cm_sups_cmd_register_internal ( sups_cmd_cb_func,
                                         NULL, /* New fn ptr NULL */
                                         data_block_ptr,
                                         client,
                                         reg_cmd_params,
                                         dialed_digits,
                                         CM_ORIG_CLIENT_CATEG_USER,
                                         is_call_control_processing_complete,
                                         FALSE, /* Old CM API */
                                         asubs_id,
                                         NULL
                                       );
}


/*===========================================================================

FUNCTION cm_sups_cmd_erase

DESCRIPTION
  EraseSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_erase(

        cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
            /* client callback function */

        void                        *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* originating client */

        const cm_erase_ss_params_s_type   *erase_cmd_params
            /* pointer to erase SS command data */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( erase_cmd_params != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -save -e668 */
  return cm_sups_cmd_erase_cc( sups_cmd_cb_func,
                               data_block_ptr,
                               client,
                               erase_cmd_params,
                               NULL,
                               TRUE );
  /*lint -restore */

}

/*===========================================================================

FUNCTION cm_sups_cmd_erase_cc

DESCRIPTION
  EraseSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_erase_cc(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client,
    /* originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params,
    /* pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                     is_call_control_processing_complete
   /* Indicates whether call control has already been completed */

)
{

  return cm_sups_cmd_erase_internal ( sups_cmd_cb_func,
                                      NULL,
                                      data_block_ptr,
                                      client,
                                      erase_cmd_params,
                                      dialed_digits,
                                      CM_ORIG_CLIENT_CATEG_USER,
                                      is_call_control_processing_complete,
                                      FALSE, /* Old CM API */
                                      #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                      SYS_MODEM_AS_ID_3
                                      #elif defined(CFG_HYBR_GW) || \
                                         defined(CFG_CG__HYBR_GW)
                                      SYS_MODEM_AS_ID_2
                                      #else
                                      SYS_MODEM_AS_ID_1
                                      #endif
                                    );

}  /* cm_sups_cmd_erase_cc */


/*===========================================================================

FUNCTION cm_sups_cmd_erase_cc_per_subs

DESCRIPTION
  EraseSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_erase_cc_per_subs(

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
    /* client callback function */

  void                              *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                 client,
    /* originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params,
    /* pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                           is_call_control_processing_complete,
   /* Indicates whether call control has already been completed */

  sys_modem_as_id_e_type            asubs_id

)
{

  return cm_sups_cmd_erase_internal ( sups_cmd_cb_func,
                                      NULL,
                                      data_block_ptr,
                                      client,
                                      erase_cmd_params,
                                      dialed_digits,
                                      CM_ORIG_CLIENT_CATEG_USER,
                                      is_call_control_processing_complete,
                                      FALSE, /* Old CM API */
                                      asubs_id
                                    );

}


/*===========================================================================

FUNCTION cm_sups_cmd_activate

DESCRIPTION
  EraseSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_activate(

        cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
            /* client callback function */

        void                        *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* originating client */

        const cm_act_ss_params_s_type     *act_cmd_params
            /* pointer to activate SS command data */

)
{
  return cm_sups_cmd_activate_cc( sups_cmd_cb_func,
                                  data_block_ptr,
                                  client,
                                  act_cmd_params,
                                  NULL,
                                  TRUE );

}

/*===========================================================================

FUNCTION cm_sups_cmd_activate_cc

DESCRIPTION
  EraseSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_activate_cc(

  cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
    /* client callback function */

  void                     *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type        client,
    /* originating client */

  const cm_act_ss_params_s_type  *act_cmd_params,
    /* pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /* pointer to a cm_num_s_type of the digitis dialed */

  boolean                  is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
{
  return ( cm_sups_cmd_activate_internal(sups_cmd_cb_func,
                                         NULL, /* New fn ptr NULL */
                                         data_block_ptr,
                                         client,
                                         act_cmd_params,
                                         dialed_digits,
                                         CM_ORIG_CLIENT_CATEG_USER,
                                         is_call_control_processing_complete,
                                         FALSE, /* Old CM API */
                                         #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                         SYS_MODEM_AS_ID_3
                                         #elif defined(CFG_HYBR_GW) || \
                                             defined(CFG_CG__HYBR_GW)
                                         SYS_MODEM_AS_ID_2
                                         #else
                                         SYS_MODEM_AS_ID_1
                                         #endif
                                        )
         );

} /* cm_sups_cmd_activate_cc */


/*===========================================================================

FUNCTION cm_sups_cmd_activate_cc_per_subs

DESCRIPTION
  EraseSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_activate_cc_per_subs(

  cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type              client,
    /* originating client */

  const cm_act_ss_params_s_type  *act_cmd_params,
    /* pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /* pointer to a cm_num_s_type of the digitis dialed */

  boolean                        is_call_control_processing_complete,
    /* Indicates whether call control has already been completed */

  sys_modem_as_id_e_type         asubs_id

)
{
  return ( cm_sups_cmd_activate_internal(sups_cmd_cb_func,
                                         NULL, /* New fn ptr NULL */
                                         data_block_ptr,
                                         client,
                                         act_cmd_params,
                                         dialed_digits,
                                         CM_ORIG_CLIENT_CATEG_USER,
                                         is_call_control_processing_complete,
                                         FALSE, /* Old CM API */
                                         asubs_id
                                        )
         );

}


/*===========================================================================

FUNCTION cm_sups_cmd_deactivate

DESCRIPTION
  DeactivateSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_deactivate(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* originating client */

        const cm_deact_ss_params_s_type   *deact_cmd_params
            /* pointer to erase SS command data */

)
{
  return cm_sups_cmd_deactivate_cc( sups_cmd_cb_func,
                                    data_block_ptr,
                                    client,
                                    deact_cmd_params,
                                    NULL,
                                    TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_deactivate_cc

DESCRIPTION
  DeactivateSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_deactivate_cc(

  cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
    /* client callback function */

  void                        *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type           client,
    /* originating client */

  const cm_deact_ss_params_s_type   *deact_cmd_params,
    /* pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                     is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
{

  return cm_sups_cmd_deactivate_internal ( sups_cmd_cb_func,
                                           NULL, /* New fn ptr NULL */
                                           data_block_ptr,
                                           client,
                                           deact_cmd_params,
                                           dialed_digits,
                                           CM_ORIG_CLIENT_CATEG_USER,
                                           is_call_control_processing_complete,
                                           FALSE, /* Old CM API */
                                           #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                           SYS_MODEM_AS_ID_3
                                           #elif defined(CFG_HYBR_GW) || \
                                               defined(CFG_CG__HYBR_GW)
                                           SYS_MODEM_AS_ID_2
                                           #else
                                           SYS_MODEM_AS_ID_1
                                           #endif
                                           );

}  /* cm_sups_cmd_deactivate */


/*===========================================================================

FUNCTION cm_sups_cmd_deactivate_cc_per_subs

DESCRIPTION
  DeactivateSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_deactivate_cc_per_subs(

  cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
    /* client callback function */

  void                              *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                 client,
    /* originating client */

  const cm_deact_ss_params_s_type   *deact_cmd_params,
    /* pointer to erase SS command data */

  const cm_num_s_type               *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                           is_call_control_processing_complete,
    /* Indicates whether call control has already been completed */

  sys_modem_as_id_e_type            asubs_id

)
{
  return cm_sups_cmd_deactivate_internal ( sups_cmd_cb_func,
                                           NULL, /* New fn ptr NULL */
                                           data_block_ptr,
                                           client,
                                           deact_cmd_params,
                                           dialed_digits,
                                           CM_ORIG_CLIENT_CATEG_USER,
                                           is_call_control_processing_complete,
                                           FALSE, /* Old CM API */
                                           asubs_id
                                        );

}


/*===========================================================================

FUNCTION cm_sups_cmd_interrogate

DESCRIPTION
  InterrogateSS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_interrogate(

        cm_sups_cmd_cb_f_type       sups_cmd_cb_func,
            /* client callback function */

        void                        *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type            client,
            /* originating client */

        const cm_interr_ss_params_s_type   *interr_cmd_params
            /* pointer to interrogate SS command data */

)
{
  return cm_sups_cmd_interrogate_cc( sups_cmd_cb_func,
                                     data_block_ptr,
                                     client,
                                     interr_cmd_params,
                                     NULL,
                                     TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_interrogate_cc

DESCRIPTION
  InterrogateSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_interrogate_cc(

  cm_sups_cmd_cb_f_type        sups_cmd_cb_func,
    /* client callback function */

  void                         *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type            client,
    /* originating client */

  const cm_interr_ss_params_s_type   *interr_cmd_params,
    /* pointer to interrogate SS command data */

  const cm_num_s_type                *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                      is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
{

  return cm_sups_cmd_interrogate_internal ( sups_cmd_cb_func,
                                            NULL, /* New fn ptr NULL */
                                            data_block_ptr,
                                            client,
                                            interr_cmd_params,
                                            dialed_digits,
                                            CM_ORIG_CLIENT_CATEG_USER,
                                            is_call_control_processing_complete,
                                            FALSE, /* Old CM API */
                                            #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                            SYS_MODEM_AS_ID_3
                                            #elif defined(CFG_HYBR_GW) || \
                                               defined(CFG_CG__HYBR_GW)
                                            SYS_MODEM_AS_ID_2
                                            #else
                                            SYS_MODEM_AS_ID_1
                                            #endif
                                        );


}  /* cm_sups_cmd_interrogate_cc */


/*===========================================================================
FUNCTION cm_sups_cmd_interrogate_cc_per_subs

DESCRIPTION
  InterrogateSS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_interrogate_cc_per_subs(

  cm_sups_cmd_cb_f_type              sups_cmd_cb_func,
    /* client callback function */

  void                               *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                  client,
    /* originating client */

  const cm_interr_ss_params_s_type   *interr_cmd_params,
    /* pointer to interrogate SS command data */

  const cm_num_s_type                *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                            is_call_control_processing_complete,
    /* Indicates whether call control has already been completed */

  sys_modem_as_id_e_type             asubs_id

)
{
  return cm_sups_cmd_interrogate_internal ( sups_cmd_cb_func,
                                            NULL, /* New fn ptr NULL */
                                            data_block_ptr,
                                            client,
                                            interr_cmd_params,
                                            dialed_digits,
                                            CM_ORIG_CLIENT_CATEG_USER,
                                            is_call_control_processing_complete,
                                            FALSE, /* Old CM API */
                                            asubs_id
                                            );

}


/*===========================================================================

FUNCTION cm_sups_cmd_reg_password

DESCRIPTION
  Register Password SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_reg_password(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type           client,
            /* originating client */

        const cm_reg_pwd_params_s_type    *reg_pwd_cmd_params
            /* pointer to register password SS command data */

)
{
  return cm_sups_cmd_reg_password_cc( sups_cmd_cb_func,
                                      data_block_ptr,
                                      client,
                                      reg_pwd_cmd_params,
                                      NULL,
                                      TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_reg_password_cc

DESCRIPTION
  Register Password SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_reg_password_cc(

  cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
    /* client callback function */

  void                       *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type          client,
    /* originating client */

  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params,
    /* pointer to register password SS command data */

  const cm_num_s_type              *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                    is_call_control_processing_complete
    /* Indicates whether call control has already been completed */

)
{
  return cm_sups_cmd_reg_password_internal ( sups_cmd_cb_func,
                                             NULL, /* New fn ptr NULL */
                                             data_block_ptr,
                                             client,
                                             reg_pwd_cmd_params,
                                             dialed_digits,
                                             CM_ORIG_CLIENT_CATEG_USER,
                                             is_call_control_processing_complete,
                                             FALSE, /* Old CM API */
                                             #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                             SYS_MODEM_AS_ID_3
                                             #elif defined(CFG_HYBR_GW) || \
                                                 defined(CFG_CG__HYBR_GW)
                                             SYS_MODEM_AS_ID_2
                                             #else
                                             SYS_MODEM_AS_ID_1
                                             #endif
                                        );

}  /* cm_sups_cmd_reg_password_cc */


/*===========================================================================
FUNCTION cm_sups_cmd_reg_password_cc_per_subs

DESCRIPTION
  Register Password SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_reg_password_cc_per_subs(

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
    /* client callback function */

  void                            *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client,
    /* originating client */

  const cm_reg_pwd_params_s_type  *reg_pwd_cmd_params,
    /* pointer to register password SS command data */

  const cm_num_s_type             *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                         is_call_control_processing_complete,
    /* Indicates whether call control has already been completed */

  sys_modem_as_id_e_type          asubs_id

)
{
  return cm_sups_cmd_reg_password_internal ( sups_cmd_cb_func,
                                             NULL, /* New fn ptr NULL */
                                             data_block_ptr,
                                             client,
                                             reg_pwd_cmd_params,
                                             dialed_digits,
                                             CM_ORIG_CLIENT_CATEG_USER,
                                             is_call_control_processing_complete,
                                             FALSE, /* Old CM API */
                                             asubs_id
                                        );

}


/*===========================================================================

FUNCTION cm_sups_cmd_process_uss

DESCRIPTION
  Process Unstructured SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_process_uss(

        cm_sups_cmd_cb_f_type    sups_cmd_cb_func,
            /* client callback function */

        void                     *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type        client,
            /* originating client */

        const cm_uss_params_s_type    *process_uss_cmd_params
            /* pointer to Process Unstructured SS command data */

)
{
  return cm_sups_cmd_process_uss_cc( sups_cmd_cb_func,
                                     data_block_ptr,
                                     client,
                                     process_uss_cmd_params,
                                     NULL,
                                     TRUE );
}

/*===========================================================================

FUNCTION cm_sups_cmd_process_uss_cc

DESCRIPTION
  Process Unstructured SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_process_uss_cc(

  cm_sups_cmd_cb_f_type           sups_cmd_cb_func,
    /**< client callback function */

  void                            *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type               client,
    /**< originating client */

  const cm_uss_params_s_type     *process_uss_cmd_params,
    /**< pointer to Process Unstructured SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                         is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

)
{
  return cm_sups_cmd_process_uss_internal ( sups_cmd_cb_func,
                                            NULL, /* New fn ptr NULL */
                                            data_block_ptr,
                                            client,
                                            process_uss_cmd_params,
                                            dialed_digits,
                                            CM_ORIG_CLIENT_CATEG_USER,
                                            is_call_control_processing_complete,
                                            FALSE, /* Old CM API */
                                            #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                            SYS_MODEM_AS_ID_3,
                                            #elif defined(CFG_HYBR_GW) || \
                                               defined(CFG_CG__HYBR_GW)
                                            SYS_MODEM_AS_ID_2,
                                            #else
                                            SYS_MODEM_AS_ID_1,
                                            #endif
              NULL
                                        );

}  /* cm_sups_cmd_process_uss_cc */


/*===========================================================================

FUNCTION cm_sups_cmd_process_uss_cc_per_subs

DESCRIPTION
  Process Unstructured SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_process_uss_cc_per_subs(

  cm_sups_cmd_cb_f_type         sups_cmd_cb_func,
    /**< client callback function */

  void                          *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type             client,
    /**< originating client */

  const cm_uss_params_s_type    *process_uss_cmd_params,
    /**< pointer to Process Unstructured SS command data */

  const cm_num_s_type           *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                       is_call_control_processing_complete,
    /**< Indicates whether call control has already been completed */

  sys_modem_as_id_e_type        asubs_id

)
{
  return cm_sups_cmd_process_uss_internal ( sups_cmd_cb_func,
                                            NULL, /* New fn ptr NULL */
                                            data_block_ptr,
                                            client,
                                            process_uss_cmd_params,
                                            dialed_digits,
                                            CM_ORIG_CLIENT_CATEG_USER,
                                            is_call_control_processing_complete,
                                            FALSE, /* Old CM API */
                                            asubs_id,
              NULL
                                        );

}


/*===========================================================================

FUNCTION cm_sups_cmd_release

DESCRIPTION
  Release SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_release(

        cm_sups_cmd_cb_f_type         sups_cmd_cb_func,
            /* client callback function */

        void                          *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type             client,
            /* originating client */

        const cm_release_ss_params_s_type    *release_cmd_params
            /* pointer to release SS command data */

)
{
  return cm_sups_cmd_release_cc( sups_cmd_cb_func,
                                 data_block_ptr,
                                 client,
                                 release_cmd_params,
                                 NULL,
                                 TRUE );
}


/*===========================================================================
FUNCTION cm_sups_cmd_release_cc_per_subs

DESCRIPTION
  Release SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_release_cc_per_subs(

  cm_sups_cmd_cb_f_type               sups_cmd_cb_func,
    /* client callback function */

  void                                *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                   client,
    /* originating client */

  const cm_release_ss_params_s_type   *release_cmd_params,
    /* pointer to release SS command data */

  const cm_num_s_type                 *dialed_digits,
    /* pointer to a cm_num_s_type of the digits dialed */

  boolean                             is_call_control_processing_complete,
    /* Indicates whether call control has already been completed */

  sys_modem_as_id_e_type              asubs_id

)
/*lint -esym(715,dialed_digits)*/
/*lint -esym(715,is_call_control_processing_complete)*/
/*lint -esym(715,release_cmd_params)*/
/*lint -esym(715,client)*/
{
  #ifdef CM_GW_SUPPORTED
  cm_sups_cmd_s_type           *sups_cmd_ptr;
  cm_sups_cmd_info_s_type      *sups_cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( release_cmd_params == NULL )
  {
    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_RELEASE,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }

  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_RELEASE;
  sups_cmd_ptr->is_call_control_processing_complete
                                = is_call_control_processing_complete;
  /*lint -e{418} keep it off for general / specific walks as well */
  if (dialed_digits != NULL)
  {
    *(&sups_cmd_ptr->dialed_digits) = *dialed_digits;
  }
  else
  {
    sups_cmd_ptr->dialed_digits.len = 0;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */

  sups_cmd_info_ptr->ss_operation = release_cmd_params->ss_oper;
  sups_cmd_info_ptr->ss_ref       = release_cmd_params->ss_ref;
  sups_cmd_info_ptr->invoke_id = release_cmd_params->ss_ref;
  sups_cmd_info_ptr->cmd_subs_id  = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */

  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_RELEASE,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  SYS_ARG_NOT_USED(asubs_id);
  SYS_ARG_NOT_USED(is_call_control_processing_complete);
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */

}  /* cm_sups_cmd_release_cc_per_subs */
/*lint +esym(715,dialed_digits)*/
/*lint +esym(715,is_call_control_processing_complete)*/
/*lint +esym(715,release_cmd_params)*/
/*lint +esym(715,client)*/


/*===========================================================================

FUNCTION cm_sups_cmd_release_cc

DESCRIPTION
  Release SS command to CM with call control.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_release_cc(

  cm_sups_cmd_cb_f_type                 sups_cmd_cb_func,
    /**< client callback function */

  void                                  *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                     client,
    /**< originating client */

  const cm_release_ss_params_s_type     *release_cmd_params,
    /**< pointer to release SS command data */

  const cm_num_s_type                   *dialed_digits,
    /**< pointer to a cm_num_s_type of the digits dialed */

  boolean                               is_call_control_processing_complete
    /**< Indicates whether call control has already been completed */

)
{
  return cm_sups_cmd_release_cc_per_subs( sups_cmd_cb_func,
                                          data_block_ptr,
                                          client,
                                          release_cmd_params,
                                          dialed_digits,
                                          is_call_control_processing_complete,
                                          #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                          SYS_MODEM_AS_ID_3
                                          #elif defined CFG_HYBR_GW
                                          #error code not present
#else
                                          SYS_MODEM_AS_ID_1
                                          #endif
                                        );
}


/*===========================================================================
FUNCTION cm_sups_cmd_uss_res_per_subs

DESCRIPTION
  Unstructured SS response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_uss_res_per_subs(

        cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
            /* client callback function */

        void                              *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                 client,
            /* originating client */

        const cm_uss_res_params_s_type    *uss_res_params,
            /* pointer to Unstructured SS response data */

        sys_modem_as_id_e_type            asubs_id
)
/*lint -esym(715,uss_res_params)*/
/*lint -esym(715,client)*/
{
  #ifdef CM_GW_SUPPORTED
  cm_sups_cmd_s_type           *sups_cmd_ptr;
  cm_sups_cmd_info_s_type      *sups_cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( uss_res_params == NULL )
  {
    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_USS_RES,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }

  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();
  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_USS_RES;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */

  sups_cmd_info_ptr->invoke_id = uss_res_params->invoke_id;

  sups_cmd_info_ptr->uss_res_type = (cm_uss_res_e_type) uss_res_params->uss_res_data.data_type;

  if( (cm_uss_res_e_type)(uss_res_params->uss_res_data.data_type) == CM_USS_RES_SS_DATA )
  {
    sups_cmd_info_ptr->uss_data = uss_res_params->uss_res_data.data.uss_data;
    sups_cmd_info_ptr->uss_res_type = CM_USS_RES_SS_DATA;
  }
  else if( (cm_uss_res_e_type)(uss_res_params->uss_res_data.data_type) == CM_USS_RES_SS_ERROR )
  {
    sups_cmd_info_ptr->ss_error = uss_res_params->uss_res_data.data.cm_ss_error;
    sups_cmd_info_ptr->uss_res_type = CM_USS_RES_SS_ERROR;
  }

  sups_cmd_info_ptr->cmd_subs_id  = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_USS_RES,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED(asubs_id);
  SYS_ARG_NOT_USED(client);
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
} /* cm_sups_cmd_uss_res_per_subs */
/*lint +esym(715,uss_res_params)*/
/*lint +esym(715,client)*/


/*===========================================================================
FUNCTION cm_sups_cmd_uss_res

DESCRIPTION
  Unstructured SS response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_uss_res(

        cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
            /* client callback function */

        void                           *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type              client,
            /* originating client */

        const cm_uss_res_params_s_type       *uss_res_params
            /* pointer to Unstructured SS response data */
)
{
  return cm_sups_cmd_uss_res_per_subs( sups_cmd_cb_func,
                                       data_block_ptr,
                                       client,
                                       uss_res_params,
                                       #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                       SYS_MODEM_AS_ID_3
                                       #elif defined CFG_HYBR_GW
                                       #error code not present
#else
                                       SYS_MODEM_AS_ID_1
                                       #endif
                                     );
} /* cm_sups_cmd_uss_res */



/*===========================================================================

@FUNCTION cm_stats_set_modem_info_mask_per_subs

@DESCRIPTION
  Command CM to change the Modem Statistics info bit mask

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

boolean cm_stats_set_modem_info_mask_per_subs (

   cm_stats_cmd_cb_f_type             cmd_cb_func,
     /**< Client callback function */

   void                              *data_block_ptr,
    /**< Pointer to client callback data block */

   cm_client_id_type                 client_id,
    /**< Requesting client */

   sys_modem_module_e_type           module_type,
    /**< Module for which mask is being set */

   uint64                            bit_mask,
    /**< Bit mask value */

   sys_modem_as_id_e_type            subs_id
    /**< Subscription ID to which this mask should apply */

)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(715,data_block_ptr)*/
/*lint -esym(818,data_block_ptr)*/
/*lint -esym(715,client_id)*/
/*lint -esym(715,module_type)*/
/*lint -esym(715,bit_mask)*/
/*lint -esym(715,subs_id)*/
{

  cm_stats_cmd_s_type       *stats_cmd_ptr;
  cm_stats_cmd_info_s_type  *cmd_info_ptr;

  /* Allocate buffer to post stats command */
  stats_cmd_ptr = cm_cmd_alloc_stats_init();

  /* Command buffer allocation is successful,
  ** copy command information into command structure.
  */
  cmd_info_ptr  = CMD_INFO_PTR (stats_cmd_ptr);

  stats_cmd_ptr->cmd_cb_func    = cmd_cb_func;
  stats_cmd_ptr->data_block_ptr = data_block_ptr;
  stats_cmd_ptr->client_id      = client_id;
  stats_cmd_ptr->cmd            = CM_STATS_CMD_SET_MODEM_INFO;
  stats_cmd_ptr->asubs_id       = subs_id;

  cmd_info_ptr->module   = module_type;
  cmd_info_ptr->bit_mask = bit_mask;

  CM_MSG_HIGH_3("FEATURE_MODEM_STATISTICS is ON: asubs_id %d, module %d, bit_mask %d",
                 subs_id, cmd_info_ptr->module, cmd_info_ptr->bit_mask );

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)stats_cmd_ptr );

  return TRUE;

}
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(715,data_block_ptr)*/
/*lint +esym(818,data_block_ptr)*/
/*lint +esym(715,client_id)*/
/*lint +esym(715,module_type)*/
/*lint +esym(715,bit_mask)*/
/*lint +esym(715,subs_id)*/



/*===========================================================================

@FUNCTION cm_stats_set_modem_info_mask

@DESCRIPTION
  Command CM to change the Modem Statistics info bit mask

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

boolean cm_stats_set_modem_info_mask (

   cm_stats_cmd_cb_f_type             cmd_cb_func,
     /**< Client callback function */

   void                              *data_block_ptr,
    /**< Pointer to client callback data block */

   cm_client_id_type                 client_id,
    /**< Requesting client */

   sys_modem_module_e_type           module_type,
    /**< Module for which mask is being set */

   uint64                            bit_mask
    /**< Bit mask value */

)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(715,data_block_ptr)*/
/*lint -esym(818,data_block_ptr)*/
/*lint -esym(715,client_id)*/
/*lint -esym(715,module_type)*/
/*lint -esym(715,bit_mask)*/
{

  return cm_stats_set_modem_info_mask_per_subs (
                                                 cmd_cb_func,
                                                 data_block_ptr,
                                                 client_id,
                                                 module_type,
                                                 bit_mask,
                                                 SYS_MODEM_AS_ID_1
                                                );

}
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(715,data_block_ptr)*/
/*lint +esym(818,data_block_ptr)*/
/*lint +esym(715,client_id)*/
/*lint +esym(715,module_type)*/
/*lint +esym(715,bit_mask)*/


/*===========================================================================
FUNCTION cm_sups_cmd_uss_notify_res_per_subs

DESCRIPTION
  Unstructured SS Notify response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_uss_notify_res_per_subs(

        cm_sups_cmd_cb_f_type                  sups_cmd_cb_func,
            /* client callback function */

        void                                   *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                      client,
            /* originating client */

        const cm_uss_notify_res_params_s_type  *uss_notify_res_params,
            /* pointer to Unstructured SS Notify response data */

        sys_modem_as_id_e_type                 asubs_id

)
/*lint -esym(715,uss_notify_res_params)*/
/*lint -esym(715,client)*/
{
  #ifdef CM_GW_SUPPORTED
  cm_sups_cmd_s_type           *sups_cmd_ptr;
  cm_sups_cmd_info_s_type      *sups_cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( uss_notify_res_params == NULL )
  {
    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_USS_NOTIFY_RES,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_USS_NOTIFY_RES;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */

  sups_cmd_info_ptr->invoke_id = uss_notify_res_params->invoke_id;

  if( uss_notify_res_params->ss_error.present )
  {
    sups_cmd_info_ptr->ss_error = uss_notify_res_params->ss_error;
    sups_cmd_info_ptr->uss_res_type = CM_USS_RES_SS_ERROR;
  }

  sups_cmd_info_ptr->cmd_subs_id  = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_USS_NOTIFY_RES,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED(asubs_id);
  SYS_ARG_NOT_USED(client);
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
}
/*lint +esym(715,uss_notify_res_params)*/
/*lint +esym(715,client)*/


/*===========================================================================

FUNCTION cm_sups_cmd_uss_notify_res

DESCRIPTION
  Unstructured SS Notify response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_uss_notify_res(

        cm_sups_cmd_cb_f_type                  sups_cmd_cb_func,
            /* client callback function */

        void                                   *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                      client,
            /* originating client */

        const cm_uss_notify_res_params_s_type        *uss_notify_res_params
            /* pointer to Unstructured SS Notify response data */
)
{
  return cm_sups_cmd_uss_notify_res_per_subs( sups_cmd_cb_func,
                                              data_block_ptr,
                                              client,
                                              uss_notify_res_params,
                                              #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                              SYS_MODEM_AS_ID_3
                                              #elif defined CFG_HYBR_GW
                                              #error code not present
#else
                                              SYS_MODEM_AS_ID_1
                                              #endif
                                            );
}


/*===========================================================================

FUNCTION cm_sups_cmd_get_password_res_per_subs

DESCRIPTION
  Get Password response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_get_password_res_per_subs(

        cm_sups_cmd_cb_f_type                 sups_cmd_cb_func,
            /* client callback function */

        void                                  *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                     client,
            /* originating client */

        const cm_get_pwd_res_params_s_type    *get_pwd_res_params,
            /* pointer to Get Password response data */

        sys_modem_as_id_e_type                asubs_id

)
/*lint -esym(715,get_pwd_res_params)*/
/*lint -esym(715,client)*/
{
  #ifdef CM_GW_SUPPORTED
  cm_sups_cmd_s_type           *sups_cmd_ptr;
  cm_sups_cmd_info_s_type      *sups_cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( get_pwd_res_params == NULL )
  {
    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_GET_PASSWORD_RES,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_GET_PASSWORD_RES;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields. */

  sups_cmd_info_ptr->invoke_id = get_pwd_res_params->invoke_id;
  sups_cmd_info_ptr->cmd_subs_id  = asubs_id;

  *(&sups_cmd_info_ptr->ss_password) = *(&get_pwd_res_params->ss_pwd);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_GET_PASSWORD_RES,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED(asubs_id);
  SYS_ARG_NOT_USED(client);
  return FALSE;
  #endif /* else of ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) ) */
}
/*lint +esym(715,get_pwd_res_params)*/
/*lint +esym(715,client)*/


/*===========================================================================

FUNCTION cm_sups_cmd_get_password_res

DESCRIPTION
  Get Password response command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_get_password_res(

        cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
            /* client callback function */

        void                               *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                  client,
            /* originating client */

        const cm_get_pwd_res_params_s_type       *get_pwd_res_params
            /* pointer to Get Password response data */

)
/*lint -esym(715,get_pwd_res_params)*/
/*lint -esym(715,client)*/
{
  return cm_sups_cmd_get_password_res_per_subs( sups_cmd_cb_func,
                                                data_block_ptr,
                                                client,
                                                get_pwd_res_params,
                                                #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                                SYS_MODEM_AS_ID_3
                                                #elif defined CFG_HYBR_GW
                                                #error code not present
#else
                                                SYS_MODEM_AS_ID_1
                                                #endif
                                              );
}


/*===========================================================================
FUNCTION cm_sups_cmd_lcs_molr_per_subs

DESCRIPTION
  LCS MOLR SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_molr_per_subs(

        cm_sups_cmd_cb_f_type            sups_cmd_cb_func,
            /* client callback function */

        void                            *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                client,
            /* originating client */

        const cm_lcs_molr_params_s_type *lcs_molr_cmd_params,
            /* pointer to lcs-MOLR SS command data */

        sys_modem_as_id_e_type          asubs_id
)
{
  /*lint -esym(715, client)*/
  /*lint -esym(715, lcs_molr_cmd_params)*/
  #if ( (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM)) )
  cm_sups_cmd_s_type         *sups_cmd_ptr;
  cm_sups_cmd_info_s_type    *sups_cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( lcs_molr_cmd_params == NULL )
  {
    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_LCS_MOLR,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_LCS_MOLR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


 /* Copy command information fields. */
  sups_cmd_info_ptr->ss_ref = lcs_molr_cmd_params->ss_ref;
  sups_cmd_info_ptr->molr_type = lcs_molr_cmd_params->molr_type;
  sups_cmd_info_ptr->location_method = lcs_molr_cmd_params->location_method;
  sups_cmd_info_ptr->lcs_qos = lcs_molr_cmd_params->lcs_qos;
  sups_cmd_info_ptr->external_id = lcs_molr_cmd_params->external_id;
  sups_cmd_info_ptr->mlc_number = lcs_molr_cmd_params->mlc_number;
  sups_cmd_info_ptr->gps_assistance_data = lcs_molr_cmd_params->gps_assistance_data;
  sups_cmd_info_ptr->supported_gad_shapes = lcs_molr_cmd_params->supported_gad_shapes;
  sups_cmd_info_ptr->cmd_subs_id          = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_LCS_MOLR,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED(asubs_id);
  return FALSE;
  #endif /* else of ((defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM))) */
}
 /*lint +esym(715, client)*/
 /*lint +esym(715, lcs_molr_cmd_params)*/


/*===========================================================================
FUNCTION cm_sups_cmd_lcs_molr

DESCRIPTION
  LCS MOLR SS command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_molr(

        cm_sups_cmd_cb_f_type            sups_cmd_cb_func,
            /* client callback function */

        void                            *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                client,
            /* originating client */

        const cm_lcs_molr_params_s_type *lcs_molr_cmd_params
            /* pointer to lcs-MOLR SS command data */

)
{
  return cm_sups_cmd_lcs_molr_per_subs( sups_cmd_cb_func,
                                        data_block_ptr,
                                        client,
                                        lcs_molr_cmd_params,
                                        SYS_MODEM_AS_ID_NO_CHANGE
                                      );
}


/*===========================================================================

FUNCTION cm_sups_cmd_lcs_molr_completed_per_subs

DESCRIPTION
  LCS MOLR SS Completed command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_molr_completed_per_subs(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
           /* pointer to client callback data block */

        cm_client_id_type           client,
           /* originating client */

        const cm_lcs_molr_completed_params_s_type  *lcs_molr_completed_cmd_params,
          /* pointer to lcs-MOLR Completed SS command data */

        sys_modem_as_id_e_type      asubs_id

)
{
  /*lint -esym(715, client)*/
  /*lint -esym(715, lcs_molr_completed_cmd_params)*/

  #if ( (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM)) )
  cm_sups_cmd_s_type         *sups_cmd_ptr;
  cm_sups_cmd_info_s_type    *sups_cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( lcs_molr_completed_cmd_params == NULL )
  {
    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_LCS_MOLR_COMPLETED,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_LCS_MOLR_COMPLETED;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


 /* Copy command information fields. */
  sups_cmd_info_ptr->ss_ref = lcs_molr_completed_cmd_params->ss_ref;
  sups_cmd_info_ptr->cmd_subs_id  = asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ((defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM)) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_LCS_MOLR_COMPLETED,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED(asubs_id);
  return FALSE;
  #endif /* else of (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM)) */
}
  /*lint +esym(715, client)*/
  /*lint +esym(715, lcs_molr_completed_cmd_params)*/


/*===========================================================================

FUNCTION cm_sups_cmd_lcs_molr_completed

DESCRIPTION
  LCS MOLR SS Completed command to CM.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_molr_completed(

        cm_sups_cmd_cb_f_type      sups_cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
           /* pointer to client callback data block */

        cm_client_id_type           client,
           /* originating client */

        const cm_lcs_molr_completed_params_s_type  *lcs_molr_completed_cmd_params
          /* pointer to lcs-MOLR Completed SS command data */

)
{
  return  cm_sups_cmd_lcs_molr_completed_per_subs( sups_cmd_cb_func,
                                                   data_block_ptr,
                                                   client,
                                                   lcs_molr_completed_cmd_params,
                                                   SYS_MODEM_AS_ID_NO_CHANGE
                                                 );
}


/*===========================================================================
FUNCTION cm_sups_cmd_lcs_location_notification_res_per_subs

DESCRIPTION
  Send an LCS location notification response to the network.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_location_notification_res_per_subs(

        cm_sups_cmd_cb_f_type          sups_cmd_cb_func,
            /* client callback function */

        void                           *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type              client,
            /* originating client */

        const cm_lcs_location_notification_res_params_s_type
                                       *lcs_location_notification_res_params,
            /* pointer to location notification response data */

        sys_modem_as_id_e_type         asubs_id

)
{

  /*lint -esym(715, client)*/
  /*lint -esym(715, lcs_location_notification_res_params)*/

  #if ( (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM)) )
  cm_sups_cmd_s_type           *sups_cmd_ptr;
  cm_sups_cmd_info_s_type      *sups_cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( lcs_location_notification_res_params == NULL )
  {
    if( sups_cmd_cb_func != NULL )
    {
      sups_cmd_cb_func( data_block_ptr,
                        CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES,
                        CM_SUPS_CMD_ERR_NULL_PARAM_P);
    }

    return FALSE;
  }
  /* Allocate a command buffer for the Supplementary Service Request */

  sups_cmd_ptr = cm_cmd_alloc_sups_init();

  /* Get a reference
     to command information. */

  sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields. */

  sups_cmd_ptr->cmd_cb_func     = sups_cmd_cb_func;
  sups_cmd_ptr->data_block_ptr  = data_block_ptr;
  sups_cmd_ptr->client_id       = client;
  sups_cmd_ptr->cmd             = CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */

  sups_cmd_info_ptr->invoke_id = lcs_location_notification_res_params->invoke_id;
  sups_cmd_info_ptr->cmd_subs_id = asubs_id;

  *(&sups_cmd_info_ptr->location_notification_res) = *(&lcs_location_notification_res_params->location_notification_res);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.*/
  cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

  return TRUE;

  #else /* ((defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM)) */
  if( sups_cmd_cb_func != NULL )
  {
    sups_cmd_cb_func( data_block_ptr, CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES,
      CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  SYS_ARG_NOT_USED(asubs_id);
  return FALSE;
  #endif /* else of ((defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || defined(FEATURE_GSM)) */
}
/*lint +esym(715, client)*/
/*lint +esym(715, lcs_location_notification_res_params)*/


/*===========================================================================
FUNCTION cm_sups_cmd_lcs_location_notification_res

DESCRIPTION
  Send an LCS location notification response to the network.

  Note that the client's supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_sups_cmd_lcs_location_notification_res(

        cm_sups_cmd_cb_f_type             sups_cmd_cb_func,
            /* client callback function */

        void                               *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type                  client,
            /* originating client */

        const cm_lcs_location_notification_res_params_s_type
                                        *lcs_location_notification_res_params
            /* pointer to location notification response data */

)
{
  return cm_sups_cmd_lcs_location_notification_res_per_subs(
                             sups_cmd_cb_func,
                             data_block_ptr,
                             client,
                             lcs_location_notification_res_params,
                             SYS_MODEM_AS_ID_NO_CHANGE
                            );
}


/*==========================================================================

@FUNCTION cm_sups_cmd_activate_exten

@DESCRIPTION
  Send an Activate Supplementary Service request.
  This function enables the client to send an Activate Supplementary
  Service request to the CM. This is a common function that can be used
  to activate different Supplementary Services. The client ID should be
  from a registered CM client.

  Note that the client's supplied callback function, sups_cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

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
boolean cm_sups_cmd_activate_exten(

  cm_sups_cmd_exten_cb_f_type sups_cmd_exten_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< originating client */

  const cm_act_ss_params_s_type  *act_cmd_params,
    /**< pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ
   /**< Initiating client category */

)
{

   return ( cm_sups_cmd_activate_internal(NULL, /* Old fn ptr NULL */
                                          sups_cmd_exten_cb_func,
                                          data_block_ptr,
                                          client_id,
                                          act_cmd_params,
                                          dialed_digits,
                                          orig_client_categ,
                                          FALSE, /* CC not complete */
                                          TRUE, /* New CM API */
                                          #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                          SYS_MODEM_AS_ID_3
                                          #elif defined(CFG_HYBR_GW) || \
                                             defined(CFG_CG__HYBR_GW)
                                          SYS_MODEM_AS_ID_2
                                          #else
                                          SYS_MODEM_AS_ID_1
                                          #endif
                                         )
          );

}

/**==========================================================================

@FUNCTION cm_sups_cmd_register_exten

@DESCRIPTION
 This API is used to send Register Supplementary Service request


 Note that the client's supplied callback function, sups_cmd_cb_func,
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
boolean cm_sups_cmd_register_exten (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                         *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type             client_id,
    /**< originating client */

  const cm_reg_ss_params_s_type  *reg_cmd_params,
    /**< pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ
   /**< Initiating client category */

)
{

   return cm_sups_cmd_register_internal( NULL, /* Old fn ptr NULL */
                                         sups_cmd_exten_cb_func,
                                         data_block_ptr,
                                         client_id,
                                         reg_cmd_params,
                                         dialed_digits,
                                         orig_client_categ,
                                         FALSE, /* CC not complete */
                                         TRUE, /* New CM API */
                                         #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                         SYS_MODEM_AS_ID_3,
                                         #elif defined(CFG_HYBR_GW) || \
                                             defined(CFG_CG__HYBR_GW)
                                         SYS_MODEM_AS_ID_2,
                                         #else
                                         SYS_MODEM_AS_ID_1,
                                         #endif
                                         NULL
                                       );

}


/**==========================================================================

@FUNCTION cm_sups_cmd_reg_password_exten

@DESCRIPTION
 This API is used to send Register Password Supplementary Service request.
 Note that the client's supplied callback function, sups_cmd_cb_func,
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
boolean cm_sups_cmd_reg_password_exten (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params,
    /**< pointer to register password SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ
   /**< Initiating client category */

)
{
   return cm_sups_cmd_reg_password_internal ( NULL, /* Old fn ptr NULL */
                                              sups_cmd_exten_cb_func,
                                              data_block_ptr,
                                              client_id,
                                              reg_pwd_cmd_params,
                                              dialed_digits,
                                              orig_client_categ,
                                              FALSE, /* CC not complete */
                                              TRUE, /* New CM API */
                                              #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                              SYS_MODEM_AS_ID_3
                                              #elif defined(CFG_HYBR_GW) || \
                                                 defined(CFG_CG__HYBR_GW)
                                              SYS_MODEM_AS_ID_2
                                              #else
                                              SYS_MODEM_AS_ID_1
                                              #endif
                                            );

}


/**==========================================================================

@FUNCTION cm_sups_cmd_process_uss_exten

@DESCRIPTION
  This API is used to send Process Unstructured Supplementary Service
  request

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

boolean cm_sups_cmd_process_uss_exten (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_uss_params_s_type     *process_uss_cmd_params,
    /**< pointer to Process Unstructured SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ
   /**< Initiating client category */

)
{
   return cm_sups_cmd_process_uss_internal( NULL, /* Old fn ptr NULL */
                                            sups_cmd_exten_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            process_uss_cmd_params,
                                            dialed_digits,
                                            orig_client_categ,
                                            FALSE, /* CC not complete */
                                            TRUE, /* New CM API */
                                            #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                            SYS_MODEM_AS_ID_3,
                                            #elif defined(CFG_HYBR_GW) || \
                                               defined(CFG_CG__HYBR_GW)
                                            SYS_MODEM_AS_ID_2,
                                            #else
                                            SYS_MODEM_AS_ID_1,
                                            #endif
              NULL
                                          );

}


/**==========================================================================

@FUNCTION cm_sups_cmd_deactivate_exten

@DESCRIPTION
  This API is used to send Deactivate Supplementary Service request.

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

boolean cm_sups_cmd_deactivate_exten (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_deact_ss_params_s_type   *deact_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ
   /**< Initiating client category */

)
{
   return cm_sups_cmd_deactivate_internal( NULL, /* Old fn ptr NULL */
                                           sups_cmd_exten_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           deact_cmd_params,
                                           dialed_digits,
                                           orig_client_categ,
                                           FALSE, /* CC not complete */
                                           TRUE, /* New CM API */
                                           #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                           SYS_MODEM_AS_ID_3
                                           #elif defined(CFG_HYBR_GW) || \
                                               defined(CFG_CG__HYBR_GW)
                                           SYS_MODEM_AS_ID_2
                                           #else
                                           SYS_MODEM_AS_ID_1
                                           #endif
                                         );

}


/**==========================================================================

@FUNCTION cm_sups_cmd_interrogate_exten

@DESCRIPTION
  This API is used to send Interrogate Supplementary Service request

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
boolean cm_sups_cmd_interrogate_exten (

  cm_sups_cmd_exten_cb_f_type         sups_cmd_exten_cb_func,
    /**< client callback function */

  void                                *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                   client_id,
    /**< originating client */

  const cm_interr_ss_params_s_type    *interr_cmd_params,
    /**< pointer to interrogate SS command data */

  const cm_num_s_type                 *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type         orig_client_categ
   /**< Initiating client category */

)
{

   return cm_sups_cmd_interrogate_internal( NULL, /* Old fn ptr NULL */
                                            sups_cmd_exten_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            interr_cmd_params,
                                            dialed_digits,
                                            orig_client_categ,
                                            FALSE, /* CC not complete */
                                            TRUE, /* New CM API */
                                            #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                            SYS_MODEM_AS_ID_3
                                            #elif defined(CFG_HYBR_GW) || \
                                               defined(CFG_CG__HYBR_GW)
                                            SYS_MODEM_AS_ID_2
                                            #else
                                            SYS_MODEM_AS_ID_1
                                            #endif
                                          );

}

/**==========================================================================

@FUNCTION cm_sups_cmd_lcs_molr_exten

@DESCRIPTION
  This API is used to send Mobile Originated Location Request
  Supplementary Service request

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
boolean cm_sups_cmd_lcs_molr_exten (

  cm_sups_cmd_exten_cb_f_type           sups_cmd_exten_cb_func,
    /**< client callback function */

  void                                  *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                     client_id,
    /**< originating client */

  const cm_lcs_molr_ss_params_s_type    *lcs_molr_cmd_params,
    /**< pointer to interrogate SS command data */

  const cm_num_s_type                   *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type           orig_client_categ
   /**< Initiating client category */

)
{

   return cm_sups_cmd_lcs_molr_internal( NULL, /* Old fn ptr NULL */
                                            sups_cmd_exten_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            lcs_molr_cmd_params,
                                            dialed_digits,
                                            orig_client_categ,
                                            FALSE, /* CC not complete */
                                            FALSE, /* New CM API */
                                            #if defined(CFG_HYBR_GW) || \
                                               defined(CFG_CDMA_HDR__HYBR_GW)
                                            #error code not present
#else
                                            SYS_MODEM_AS_ID_1
                                            #endif
                                          );

}

/*==========================================================================

@FUNCTION cm_sups_cmd_erase_exten

@DESCRIPTION
  This API is used to send Erase Supplementary Service request.

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
boolean cm_sups_cmd_erase_exten (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ
   /**< Initiating client category */

)
{

   return cm_sups_cmd_erase_internal( NULL, /* Old fn ptr NULL */
                                      sups_cmd_exten_cb_func,
                                      data_block_ptr,
                                      client_id,
                                      erase_cmd_params,
                                      dialed_digits,
                                      orig_client_categ,
                                      FALSE, /* CC not complete */
                                      TRUE, /* New CM API */
                                      #if defined(CFG_HYBR_GW_3) || defined(CFG_GW_G_G_TSTS)
                                      SYS_MODEM_AS_ID_3
                                      #elif defined(CFG_HYBR_GW) || \
                                         defined(CFG_CG__HYBR_GW)
                                      SYS_MODEM_AS_ID_2
                                      #else
                                      SYS_MODEM_AS_ID_1
                                      #endif
                                    );

}

/*==========================================================================

@FUNCTION cm_sups_cmd_activate_cc_exten_per_subs

@DESCRIPTION
  Send an Activate Supplementary Service request.
  This function enables the client to send an Activate Supplementary
  Service request to the CM. This is a common function that can be used
  to activate different Supplementary Services. The client ID should be
  from a registered CM client.

  Note that the client's supplied callback function, sups_cmd_cb_func,
  will be called to notify the client of the command status.
  If the command request is processed without errors, CM_SUPS_CMD_ERR_NOERR
  is passed back to the client.

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
boolean cm_sups_cmd_activate_cc_exten_per_subs(

  cm_sups_cmd_exten_cb_f_type sups_cmd_exten_cb_func,
    /**< client callback function */

  void                     *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type        client_id,
    /**< originating client */

  const cm_act_ss_params_s_type  *act_cmd_params,
    /**< pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ,
   /**< Initiating client category */

  boolean                        is_call_control_processing_complete,
   /**< Indicate if call control is done or not */

  sys_modem_as_id_e_type         subscription_id
   /**< Subscription to originate the call on */

)
{

   return ( cm_sups_cmd_activate_internal(NULL, /* Old fn ptr NULL */
                                          sups_cmd_exten_cb_func,
                                          data_block_ptr,
                                          client_id,
                                          act_cmd_params,
                                          dialed_digits,
                                          orig_client_categ,
                                          is_call_control_processing_complete,
                                          TRUE, /* New CM API */
                                          subscription_id
                                         )
          );

}

/**==========================================================================

@FUNCTION cm_sups_cmd_register_cc_exten_per_subs

@DESCRIPTION
 This API is used to send Register Supplementary Service request


 Note that the client's supplied callback function, sups_cmd_cb_func,
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
boolean cm_sups_cmd_register_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                         *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type             client_id,
    /**< originating client */

  const cm_reg_ss_params_s_type  *reg_cmd_params,
    /**< pointer to activate SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ,
   /**< Initiating client category */

  boolean                        is_call_control_processing_complete,
   /**< Indicate if call control is done or not */

  sys_modem_as_id_e_type         subscription_id
   /**< Subscription to originate the call on */


)
{

   return cm_sups_cmd_register_internal( NULL, /* Old fn ptr NULL */
                                         sups_cmd_exten_cb_func,
                                         data_block_ptr,
                                         client_id,
                                         reg_cmd_params,
                                         dialed_digits,
                                         orig_client_categ,
                                         is_call_control_processing_complete,
                                         TRUE, /* New CM API */
                                         subscription_id,
                                         NULL
                                       );

}


/**==========================================================================

@FUNCTION cm_sups_cmd_reg_password_cc_exten_per_subs

@DESCRIPTION
 This API is used to send Register Password Supplementary Service request.
 Note that the client's supplied callback function, sups_cmd_cb_func,
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
boolean cm_sups_cmd_reg_password_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_reg_pwd_params_s_type   *reg_pwd_cmd_params,
    /**< pointer to register password SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ,
   /**< Initiating client category */

  boolean                        is_call_control_processing_complete,
   /**< Indicate if call control is done or not */

  sys_modem_as_id_e_type         subscription_id
   /**< Subscription to originate the call on */

)
{
   return cm_sups_cmd_reg_password_internal ( NULL, /* Old fn ptr NULL */
                                              sups_cmd_exten_cb_func,
                                              data_block_ptr,
                                              client_id,
                                              reg_pwd_cmd_params,
                                              dialed_digits,
                                              orig_client_categ,
                                              is_call_control_processing_complete,
                                              TRUE, /* New CM API */
                                              subscription_id
                        );

}


/**==========================================================================

@FUNCTION cm_sups_cmd_process_uss_exten_per_subs

@DESCRIPTION
  This API is used to send Process Unstructured Supplementary Service
  request

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

boolean cm_sups_cmd_process_uss_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_uss_params_s_type     *process_uss_cmd_params,
    /**< pointer to Process Unstructured SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ,
   /**< Initiating client category */

  boolean                        is_call_control_processing_complete,
   /**< Indicate if call control is done or not */

  sys_modem_as_id_e_type         subscription_id
   /**< Subscription to originate the call on */

)
{
   return cm_sups_cmd_process_uss_internal( NULL, /* Old fn ptr NULL */
                                            sups_cmd_exten_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            process_uss_cmd_params,
                                            dialed_digits,
                                            orig_client_categ,
                                            is_call_control_processing_complete,
                                            TRUE, /* New CM API */
                                            subscription_id,
              NULL
                      );

}


/**==========================================================================

@FUNCTION cm_sups_cmd_deactivate_cc_exten_per_subs

@DESCRIPTION
  This API is used to send Deactivate Supplementary Service request.

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

boolean cm_sups_cmd_deactivate_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_deact_ss_params_s_type   *deact_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ,
   /**< Initiating client category */

  boolean                        is_call_control_processing_complete,
   /**< Indicate if call control is done or not */

  sys_modem_as_id_e_type         subscription_id
   /**< Subscription to originate the call on */

)
{
   return cm_sups_cmd_deactivate_internal( NULL, /* Old fn ptr NULL */
                                           sups_cmd_exten_cb_func,
                                           data_block_ptr,
                                           client_id,
                                           deact_cmd_params,
                                           dialed_digits,
                                           orig_client_categ,
                                           is_call_control_processing_complete,
                                           TRUE, /* New CM API */
                                           subscription_id
                       );

}


/**==========================================================================

@FUNCTION cm_sups_cmd_interrogate_cc_exten_per_subs

@DESCRIPTION
  This API is used to send Interrogate Supplementary Service request

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
boolean cm_sups_cmd_interrogate_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type         sups_cmd_exten_cb_func,
    /**< client callback function */

  void                                *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type                   client_id,
    /**< originating client */

  const cm_interr_ss_params_s_type    *interr_cmd_params,
    /**< pointer to interrogate SS command data */

  const cm_num_s_type                 *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type         orig_client_categ,
   /**< Initiating client category */

  boolean                        is_call_control_processing_complete,
   /**< Indicate if call control is done or not */

  sys_modem_as_id_e_type         subscription_id
   /**< Subscription to originate the call on */

)
{

   return cm_sups_cmd_interrogate_internal( NULL, /* Old fn ptr NULL */
                                            sups_cmd_exten_cb_func,
                                            data_block_ptr,
                                            client_id,
                                            interr_cmd_params,
                                            dialed_digits,
                                            orig_client_categ,
                                            is_call_control_processing_complete,
                                            TRUE, /* New CM API */
                                            subscription_id
                      );

}


/*==========================================================================

@FUNCTION cm_sups_cmd_erase_cc_exten_per_subs

@DESCRIPTION
  This API is used to send Erase Supplementary Service request.

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
boolean cm_sups_cmd_erase_cc_exten_per_subs (

  cm_sups_cmd_exten_cb_f_type   sups_cmd_exten_cb_func,
    /**< client callback function */

  void                      *data_block_ptr,
    /**< pointer to client callback data block */

  cm_client_id_type         client_id,
    /**< originating client */

  const cm_erase_ss_params_s_type   *erase_cmd_params,
    /**< pointer to erase SS command data */

  const cm_num_s_type            *dialed_digits,
    /**< pointer to a cm_num_s_type of the digitis dialed */

  cm_orig_client_categ_e_type    orig_client_categ,
   /**< Initiating client category */

  boolean                        is_call_control_processing_complete,
   /**< Indicate if call control is done or not */

  sys_modem_as_id_e_type         subscription_id
   /**< Subscription to originate the call on */

)
{

   return cm_sups_cmd_erase_internal( NULL, /* Old fn ptr NULL */
                                      sups_cmd_exten_cb_func,
                                      data_block_ptr,
                                      client_id,
                                      erase_cmd_params,
                                      dialed_digits,
                                      orig_client_categ,
                                      is_call_control_processing_complete,
                                      TRUE, /* New CM API */
                                      subscription_id
                                    );

}

/*===========================================================================
FUNCTION cm_bcmcs_cmd_flow_request

DESCRIPTION

  Queue CM_BCMCS_CMD_FLOW_REQUEST_F command to CM.
  This command is used to register BCMCS data flows with
  lower layer.

  If num_of_flows > 0 and flow_ids_array_ptr is not NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "update flow registration" command sent to the lower layer.

  If num_of_flows = 0 and flow_ids_array_ptr is NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "discontinue broadcast" command sent to the lower layer.


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_bcmcs_cmd_flow_request(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /* array of IP:port number addresses for which flows should be requested */
)
{
  static sys_bcmcs_reg_setup_e_type
            flow_reg_setup_array_ptr[SYS_BCMCS_MAX_FLOWS];


  memset(flow_reg_setup_array_ptr , 0 ,
         (uint32)cmutil_get_sys_bcmcs_max_flows() * sizeof(sys_bcmcs_reg_setup_e_type));

  if(num_of_flows > 0)
  {
    return(cm_bcmcs_cmd_flow_request2(cmd_cb_func,
                                      data_block_ptr,
                                      client_id,
                                      num_of_flows,
                                      flow_ids_array_ptr,
                                      flow_reg_setup_array_ptr));
  }
  else
  {
    return(cm_bcmcs_cmd_flow_request2(cmd_cb_func,
                                      data_block_ptr,
                                      client_id,
                                      num_of_flows,
                                      flow_ids_array_ptr,
                                      NULL));
  }
}

/*===========================================================================
FUNCTION cm_bcmcs_cmd_flow_request2

DESCRIPTION

  Queue CM_BCMCS_CMD_FLOW_REQUEST_F command to CM.
  This command is used to register BCMCS data flows with
  lower layer.

  If num_of_flows > 0 and flow_ids_array_ptr is not NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "update flow registration" command sent to the lower layer.

  If num_of_flows = 0 and flow_ids_array_ptr is NULL,
  queueing CM_BCMCS_CMD_FLOW_REQUEST_F will result in
  "discontinue broadcast" command sent to the lower layer.


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_bcmcs_cmd_flow_request2(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr,
    /* array of IP:port number addresses for which flows should be requested */

  const sys_bcmcs_reg_setup_e_type *flow_reg_setup_array_ptr
    /* whether non-required registration should be allowed for each flow */

)
/*lint -esym(715, num_of_flows)*/
/*lint -esym(715, flow_ids_array_ptr)*/
/*lint -esym(715, flow_reg_setup_array_ptr)*/
/*lint -esym(715, client_id)*/
{

  #if (defined (FEATURE_BCMCS) || defined (FEATURE_MBMS))

  cm_bcmcs_cmd_s_type             *bcmcs_cmd_ptr;
  cm_bcmcs_cmd_info_u_type        *cmd_info_ptr  = NULL;

  CM_ASSERT( BETWEEN(client_id,CM_CLIENT_TYPE_NONE,CM_CLIENT_TYPE_MAX));

  /*
  ** Check for any function parameter errors
  */

  /* Check for valid number of flow ids */
  if( !(num_of_flows <= cmutil_get_sys_bcmcs_max_flows()) ||
      ((num_of_flows == 0) && (flow_ids_array_ptr != NULL) )||
      ((num_of_flows == 0) && (flow_reg_setup_array_ptr != NULL) ))
  {
    if( cmd_cb_func != NULL )
    {
        cmd_cb_func( data_block_ptr,
                     CM_BCMCS_CMD_FLOW_REQUEST_F,
                     CM_BCMCS_CMD_ERR_INVALID_NUMBER_OF_FLOW_IDS_P);
    }
    return FALSE;
  }

  /* Now that num_of_flows parameter is OK, check pointer
     to flow id list */
  if(((flow_ids_array_ptr == NULL) && ( num_of_flows != 0 ))||
     ((flow_reg_setup_array_ptr == NULL) && ( num_of_flows != 0 )))
  {
    if( cmd_cb_func != NULL )
    {

      cmd_cb_func( data_block_ptr,
                   CM_BCMCS_CMD_FLOW_REQUEST_F,
                   CM_BCMCS_CMD_ERR_FLOW_IDS_NULL_PTR_P);
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Now that all function parameters are correct,
     get command buffer to queue CM_BCMCS_CMD_FLOW_REQUEST_F */
  bcmcs_cmd_ptr = cm_cmd_alloc_bcmcs_init();

  /* Copy command header fields. */
  bcmcs_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  bcmcs_cmd_ptr->data_block_ptr  = data_block_ptr;
  bcmcs_cmd_ptr->client_id       = client_id;
  bcmcs_cmd_ptr->cmd             = CM_BCMCS_CMD_FLOW_REQUEST_F;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */
  cmd_info_ptr              = CMD_INFO_PTR( bcmcs_cmd_ptr );

  cmd_info_ptr->flow_req.num_of_flows = num_of_flows;

  if (num_of_flows > ARR_SIZE(cmd_info_ptr->flow_req.flow_addr_array))
  {
    CM_ERR_0 ("num of flows exceeds flow_addr_array size");

    /* Limiting num_of_flows to what array allows
    */
    num_of_flows = ARR_SIZE(cmd_info_ptr->flow_req.flow_addr_array);
  }

  CM_MSG_HIGH_1 ("Flow service requested = %d",
        cmd_info_ptr->flow_req.flow_addr_array[0].flow_service);

  memscpy(cmd_info_ptr->flow_req.flow_addr_array,
          cmd_info_ptr->flow_req.num_of_flows * sizeof(sys_bcmcs_flow_addr_s_type),
          flow_ids_array_ptr,
          num_of_flows * sizeof(sys_bcmcs_flow_addr_s_type) );

  #ifdef FEATURE_BCMCS_20
  #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type *)bcmcs_cmd_ptr );

  return TRUE;

  #else /* FEATURE_BCMCS || MBMS not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_BCMCS_CMD_FLOW_REQUEST_F,
                 CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_BCMCS */

}
/*lint +esym(715, num_of_flows)*/
/*lint +esym(715, flow_ids_array_ptr)*/
/*lint +esym(715, flow_reg_setup_array_ptr)*/
/*lint +esym(715, client_id)*/

/*===========================================================================
FUNCTION cm_bcmcs_cmd_registration_request

DESCRIPTION

  Queue CM_BCMCS_CMD_REG_REQUEST_F command to CM.
  This command is used to register BCMCS data flows with
  lower layer.


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/

boolean cm_bcmcs_cmd_registration_request(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /* array of IP:port number addresses for which flows should be registered */
)
/*lint -esym(715, num_of_flows)*/
/*lint -esym(715, flow_ids_array_ptr)*/
/*lint -esym(715, client_id)*/
{

  #ifdef FEATURE_BCMCS_20
#error code not present
#else /* FEATURE_BCMCS_20 not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_BCMCS_CMD_REG_REQUEST_F,
                 CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;

  #endif /* else of defined FEATURE_BCMCS_20 */

}
/*lint +esym(715, num_of_flows)*/
/*lint +esym(715, flow_ids_array_ptr)*/
/*lint +esym(715, client_id)*/


/*===========================================================================
FUNCTION cm_bcmcs_cmd_registration_handoff

DESCRIPTION

  Queue CM_BCMCS_CMD_REG_HANDOFF_F command to CM.
  This command is used to enable event driven registrations on handoff


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or invalid parameters
  passed to the function.

SIDE EFFECTS
  None

===========================================================================*/

boolean cm_bcmcs_cmd_registration_handoff(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /* array of IP:port number addresses for which flows should be registered */
)
/*lint -esym(715, num_of_flows)*/
/*lint -esym(715, flow_ids_array_ptr)*/
/*lint -esym(715, client_id)*/
{

  #if defined( FEATURE_BCMCS_20 ) && defined( FEATURE_HDR_BCMCS_DYNAMIC_MODEL )
#error code not present
#else /* FEATURE_BCMCS_20 not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_BCMCS_CMD_REG_HANDOFF_F,
                 CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;

  #endif /* else of defined FEATURE_BCMCS_20 */

}
/*lint +esym(715, num_of_flows)*/
/*lint +esym(715, flow_ids_array_ptr)*/
/*lint +esym(715, client_id)*/

/*===========================================================================
FUNCTION cm_bcmcs_cmd_bom_cache_setup

DESCRIPTION

  Queue CM_BCMCS_CMD_BOM_CACHING_SETUP_F command to CM.
  This command is used to enable / disable strict BOM caching

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_bcmcs_cmd_bom_cache_setup(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           stale_BOM_factor,
    /* multiplier for staleness of BOM entry */

  sys_bcmcs_bom_caching_e_type    enable_strict_caching
    /* enable / disable strict BOM caching */
)
/*lint -esym(715, client_id)*/
/*lint -esym(715, stale_BOM_factor)*/
/*lint -esym(715, enable_strict_caching)*/
{

  #if defined( FEATURE_BCMCS_20 ) && defined( FEATURE_HDR_BCMCS_DYNAMIC_MODEL )
#error code not present
#else /* FEATURE_BCMCS_20 not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_BCMCS_CMD_FLOW_REQUEST_F,
                 CM_BCMCS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_BCMCS_20 */

}
/*lint +esym(715, client_id)*/
/*lint +esym(715, stale_BOM_factor)*/
/*lint +esym(715, enable_strict_caching)*/

/*===========================================================================
FUNCTION cm_mbms_cmd_flow_deact_request

DESCRIPTION
  This command is used to deact MBMS data flow requests with
  lower layers.


DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mbms_cmd_flow_deact_request(

  cm_bcmcs_cmd_cb_f_type          cmd_cb_func,
    /* client callback function */

  void                           *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type               client_id,
    /* originating client */

  uint8                           num_of_flows,
    /* number of entries in the array below */

  const sys_bcmcs_flow_addr_s_type *flow_ids_array_ptr
    /* array of IP:port number addresses for which flows should be requested */

)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(715,data_block_ptr)*/
/*lint -esym(715,client_id)*/
/*lint -esym(715,num_of_flows)*/
/*lint -esym(715,flow_ids_array_ptr)*/
/*lint -esym(818, data_block_ptr)*/
{
  #if defined (FEATURE_MBMS)
  #error code not present
#else
  CM_ERR_0( "FEATURE_MBMS is not defined ");
  return FALSE;
  #endif

} /* cm_mbms_cmd_flow_deact_request () */
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(715,data_block_ptr)*/
/*lint +esym(715,client_id)*/
/*lint +esym(715,num_of_flows)*/
/*lint +esym(715,flow_ids_array_ptr)*/
/*lint +esym(818, data_block_ptr)*/

/*===========================================================================
FUNCTION cm_ph_cmd_line_switching

DESCRIPTION

  Queue CM_PH_CMD_LINE_SWITCHING command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_line_switching(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  cm_als_line_switching_e_type         line_switching
    /* whether the user is allowed to switch between lines */

)
/*lint -esym(715, line_switching)*/
/*lint -esym(715, client_id)*/
{

  #if (defined FEATURE_ALS) && (defined FEATURE_GSM)

  cm_client_s_type              *client_ptr    = NULL;
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Check for any function parameter errors
  */

  /*
  ** Verify client id
  */
  client_ptr = cmclient_get_client_ptr( client_id );

  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
    if( cmd_cb_func != NULL )
    {
        cmd_cb_func( data_block_ptr,
                     CM_PH_CMD_LINE_SWITCHING,
                     CM_PH_CMD_ERR_CLIENT_ID_P);
    }
    return FALSE;
  }

  /*
  ** Verify line selection status
  */
  if ( !BETWEEN(line_switching,
                CM_ALS_LINE_SWITCHING_NONE,
                CM_ALS_LINE_SWITCHING_MAX) )
  {
    if( cmd_cb_func != NULL ){
        cmd_cb_func( data_block_ptr,
                     CM_PH_CMD_LINE_SWITCHING,
                     CM_PH_CMD_ERR_LINE_SWITCHING_P);
    }
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_LINE_SWITCHING
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func      = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr   = data_block_ptr;
  ph_cmd_ptr->client_id        = client_id;
  ph_cmd_ptr->cmd              = CM_PH_CMD_LINE_SWITCHING;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr                  = CMD_INFO_PTR( ph_cmd_ptr );
  cmd_info_ptr->cmd_err         = CM_PH_CMD_ERR_NOERR;
  cmd_info_ptr->line_switching  = line_switching;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

  #else /* FEATURE_ALS not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_LINE_SWITCHING,
                 CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_ALS */

}
/*lint +esym(715, line_switching)*/
/*lint +esym(715, client_id)*/

/*===========================================================================
FUNCTION cm_ph_cmd_selected_line

DESCRIPTION

  Queue CM_PH_CMD_SELECTED_LINE command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_selected_line(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  cm_als_line_e_type                   selected_line
    /* selected line */

)
/*lint -esym(715,client_id)*/
/*lint -esym(715,selected_line)*/
{

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)

  cm_client_s_type              *client_ptr    = NULL;
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Check for any function parameter errors
  */

  /*
  ** Verify client id
  */
  client_ptr = cmclient_get_client_ptr( client_id );

  if ( (client_ptr == NULL) || (client_ptr->init_mask != CM_INIT_MASK) )
  {
    if( cmd_cb_func != NULL )
    {
        cmd_cb_func( data_block_ptr,
                     CM_PH_CMD_SELECTED_LINE,
                     CM_PH_CMD_ERR_CLIENT_ID_P);
    }
    return FALSE;
  }

  /*
  ** Verify line selection status
  */
  if ( !BETWEEN(selected_line,
                CM_ALS_LINE_NONE,
                CM_ALS_LINE_MAX) )
  {
    if( cmd_cb_func != NULL )
    {
        cmd_cb_func( data_block_ptr,
                     CM_PH_CMD_SELECTED_LINE,
                     CM_PH_CMD_ERR_SELECTED_LINE_P);
    }
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_LINE_SWITCHING
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_SELECTED_LINE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr                         = CMD_INFO_PTR( ph_cmd_ptr );
  cmd_info_ptr->cmd_err                = CM_PH_CMD_ERR_NOERR;
  cmd_info_ptr->line                   = selected_line;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

  #else /* FEATURE_ALS not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_SELECTED_LINE,
                 CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_ALS */

}
/*lint +esym(715,client_id)*/
/*lint +esym(715,selected_line)*/


/*===========================================================================
FUNCTION cm_ph_cmd_detach_and_plmn_blocking

DESCRIPTION

  Queue CM_PH_CMD_PLMN_BLOCKING command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_detach_and_plmn_blocking(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  cm_plmn_blocking_info_s_type         plmn_blocking_info
    /* LTE detach and/or PLMN blocking info. */

)
/*lint -esym(715,client_id)*/
/*lint -esym(715,plmn_blocking_info)*/
{

  #ifdef FEATURE_CM_LTE
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Check for any function parameter errors
  */

  /*
  ** Verify request_pref range
  */
  if ( !BETWEEN(plmn_blocking_info.request_pref,
                CM_PLMN_BLOCKING_PREF_NONE,
                CM_PLMN_BLOCKING_PREF_MAX) )
  {
    if( cmd_cb_func != NULL )
    {
        cmd_cb_func( data_block_ptr,
                     CM_PH_CMD_PLMN_BLOCKING,
                     CM_PH_CMD_ERR_PLMN_BLOCKING_P);
    }
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_PLMN_BLOCKING
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_PLMN_BLOCKING;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr                         = CMD_INFO_PTR( ph_cmd_ptr );
  cmd_info_ptr->cmd_err                = CM_PH_CMD_ERR_NOERR;
  cmd_info_ptr->request_pref           = plmn_blocking_info.request_pref;
  if((plmn_blocking_info.request_pref == \
          CM_PLMN_BLOCKING_PREF_PLMN_BLOCKING_ONLY) ||
     (plmn_blocking_info.request_pref == \
          CM_PLMN_BLOCKING_PREF_LTE_DETACH_PLMN_BLOCKING))
  {
    cmd_info_ptr->plmn              = plmn_blocking_info.plmn;
    cmd_info_ptr->blocking_interval = plmn_blocking_info.blocking_interval;
  }
  else if(plmn_blocking_info.request_pref == \
            CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY ||
          plmn_blocking_info.request_pref == \
            CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_FORCE_PREF)
  {
    cmd_info_ptr->plmn              = plmn_blocking_info.plmn;
  }

  cmd_info_ptr->cmd_subs            = plmn_blocking_info.asubs_id;
  /* Determine cause for block/unblock command based on client_id
  ** SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE - if client is CM
  ** SYS_BLOCK_PLMN_CAUSE_OTHERS - if client is some other , like DS
  */
  if(client_id == CM_CLIENT_ID_ANONYMOUS)
  {
    cmd_info_ptr->plmn_block_cause = SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE;
  }
  else if (plmn_blocking_info.cause == SYS_BLOCK_PLMN_CAUSE_NONE)
  {
    cmd_info_ptr->plmn_block_cause = SYS_BLOCK_PLMN_CAUSE_OTHERS;
  }
  else
  {
    cmd_info_ptr->plmn_block_cause = plmn_blocking_info.cause;
  }


  CM_MSG_HIGH_3("PLMN blocking info: request_type %d, blocking_interval %d, as_id %d",
             cmd_info_ptr->request_pref,
             cmd_info_ptr->blocking_interval,
             cmd_info_ptr->cmd_subs);
  
  CM_MSG_HIGH_5("requesting client %d, blocking_cause %d, plmn %d %d %d",
  			  client_id, 
  	          cmd_info_ptr->plmn_block_cause,
              cmd_info_ptr->plmn.identity[0],
              cmd_info_ptr->plmn.identity[1],
              cmd_info_ptr->plmn.identity[2]);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

  #else /* FEATURE_CM_LTE not defined */

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_PLMN_BLOCKING,
                 CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of defined FEATURE_CM_LTE */

}
/*lint +esym(715,client_id)*/
/*lint +esym(715,plmn_blocking_info)*/


/*===========================================================================
FUNCTION cm_ph_cmd_get_drx_req

DESCRIPTION

  Queue CM_PH_CMD_GET_DRX_REQ command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_get_drx_req(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id
    /* commanding client's id */
)
{
  cm_get_drx_info_s_type               get_drx_info;

 

  get_drx_info.asubs_id = SYS_MODEM_AS_ID_1;

  return(cm_ph_cmd_get_drx_req_per_subs(cmd_cb_func,
                                 data_block_ptr,
                                 client_id,
                                 &get_drx_info));

}


/*===========================================================================
FUNCTION cm_ph_cmd_get_drx_req_per_subs

DESCRIPTION

  Queue CM_PH_CMD_GET_DRX_REQ command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_get_drx_req_per_subs(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  cm_get_drx_info_s_type              *get_drx_info_ptr
)
/*lint -esym(715,client_id)*/
{
  #if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_GET_DRX_REQ
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_GET_DRX_REQ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error checking.
  */
  if(get_drx_info_ptr == NULL)
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_GET_DRX_REQ,
                 CM_PH_CMD_ERR_INPUT_NULL_POINTER);
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields.
  */
  cmd_info_ptr                         = CMD_INFO_PTR( ph_cmd_ptr );
  cmd_info_ptr->cmd_err                = CM_PH_CMD_ERR_NOERR;
  cmd_info_ptr->cmd_subs               = get_drx_info_ptr->asubs_id;

  CM_MSG_MED_1("as_id=%d",cmd_info_ptr->cmd_subs);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

  #else

  CM_MSG_HIGH_0("cm_ph_cmd_get_drx_req() is not supported");

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_GET_DRX_REQ,
                 CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;

  #endif
}
/*lint +esym(715,client_id)*/



/*===========================================================================
FUNCTION cm_ph_cmd_set_drx_req

DESCRIPTION

  Queue CM_PH_CMD_SET_DRX_REQ command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_set_drx_req(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  sys_drx_cn_coefficient_s1_e_type     drx_coefficient
    /* DRX value to be set to */

)
{
  cm_set_drx_info_s_type                 set_drx_info;

  

  set_drx_info.asubs_id        = SYS_MODEM_AS_ID_1;
  set_drx_info.drx_coefficient = drx_coefficient;

  return(cm_ph_cmd_set_drx_req_per_subs(cmd_cb_func,
                                 data_block_ptr,
                                 client_id,
                                 &set_drx_info));

}



/*===========================================================================
FUNCTION cm_ph_cmd_set_drx_req_per_subs

DESCRIPTION

  Queue CM_PH_CMD_SET_DRX_REQ command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_set_drx_req_per_subs(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  cm_set_drx_info_s_type              *set_drx_info_ptr

)
/*lint -esym(715,client_id)*/
/*lint -esym(715,drx_coefficient)*/
{
  #if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_SET_DRX_REQ
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_SET_DRX_REQ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error checking.
  */
  if(set_drx_info_ptr == NULL)
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_SET_DRX_REQ,
                 CM_PH_CMD_ERR_INPUT_NULL_POINTER);
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr                         = CMD_INFO_PTR( ph_cmd_ptr );
  cmd_info_ptr->cmd_err                = CM_PH_CMD_ERR_NOERR;
  cmd_info_ptr->cmd_subs               = set_drx_info_ptr->asubs_id;
  cmd_info_ptr->drx_coefficient        = set_drx_info_ptr->drx_coefficient;

  CM_MSG_MED_2("as_id=%d, drx_coefficient=%d",
             cmd_info_ptr->cmd_subs,(int)set_drx_info_ptr->drx_coefficient);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

  #else

  CM_MSG_HIGH_0("cm_ph_cmd_set_drx_req() is not supported");

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_SET_DRX_REQ,
                 CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;

  #endif
}
/*lint +esym(715,client_id)*/
/*lint +esym(715,drx_coefficient)*/


/*===========================================================================
FUNCTION cm_ph_cmd_set_tl_irat_search_timer_req

DESCRIPTION

  Queue CM_PH_CMD_SET_TL_IRAT_SEARCH_TIMER_REQ command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_set_tl_irat_search_timer_req(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  uint32                               timer_value
    /* The timer value for TDSCDMA to LTE IRAT search */

)
{
  cm_set_tl_irat_search_timer_info_s_type  set_timer_info;

  set_timer_info.timer_value = timer_value;
  set_timer_info.asubs_id    = SYS_MODEM_AS_ID_1;

  return(cm_ph_cmd_set_tl_irat_search_timer_req_per_subs(cmd_cb_func,
    data_block_ptr,
    client_id,
    &set_timer_info));

}



/*===========================================================================
FUNCTION cm_ph_cmd_set_tl_irat_search_timer_req_per_subs

DESCRIPTION

  Queue CM_PH_CMD_SET_TL_IRAT_SEARCH_TIMER_REQ command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

  FALSE
  No CM buffer available to queue the command or incorrect parameters
  passed into the function

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_set_tl_irat_search_timer_req_per_subs(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  cm_set_tl_irat_search_timer_info_s_type  *set_timer_info_ptr

)
/*lint -esym(715,client_id)*/
/*lint -esym(715,timer_value)*/
{
  #if defined(FEATURE_TDSCDMA)
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_SET_TL_IRAT_SEARCH_TIMER_REQ
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func           = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr        = data_block_ptr;
  ph_cmd_ptr->client_id             = client_id;
  ph_cmd_ptr->cmd                   = CM_PH_CMD_SET_TL_IRAT_SEARCH_TIMER_REQ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error checking.
  */
  if(set_timer_info_ptr == NULL)
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_SET_TL_IRAT_SEARCH_TIMER_REQ,
                 CM_PH_CMD_ERR_INPUT_NULL_POINTER);
    }

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr                               = CMD_INFO_PTR( ph_cmd_ptr );
  cmd_info_ptr->cmd_err                      = CM_PH_CMD_ERR_NOERR;
  cmd_info_ptr->tl_irat_search_timer_value   = set_timer_info_ptr->timer_value;
  cmd_info_ptr->cmd_subs                     = set_timer_info_ptr->asubs_id;

  CM_MSG_MED_2("TDS to LTE IRAT search timer_value=%d,as_id=%d",
              set_timer_info_ptr->timer_value,set_timer_info_ptr->asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

  #else

  CM_MSG_HIGH_0("cm_ph_cmd_set_tl_irat_search_timer_req() is not supported");

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr,
                 CM_PH_CMD_SET_TL_IRAT_SEARCH_TIMER_REQ,
                 CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }

  return FALSE;

  #endif
}
/*lint -restore*/
/*lint -restore*/


/*===========================================================================

FUNCTION cm_mm_call_cmd_recall_rsp

DESCRIPTION
  This function carries response from client for a recall indication from
  network.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  (FEATURE_WCDMA || FEATURE_GSM) must be defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_cmd_recall_rsp (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_call_id_type               call_id,
    /* call ID of the call */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_ccbs_recall_rsp_e_type     ccbs_recall_rsp
    /* client response to recall indication */
)
{

  #ifdef CM_GW_SUPPORTED
  cm_call_cmd_s_type                *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_info_gw_cs_s_type          *cmd_mode_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2( " CM_CALL_CMD_RECALL_RSP client = %d, ccbs_recall_rsp = %d",
                client_id, ccbs_recall_rsp);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Get a reference to command information.
    */
    cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );
    cmd_mode_info_ptr  = WCALL_CMD_INFO_PTR( call_cmd_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_RECALL_RSP;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields.
    */

    /* This helps resolve info_type to route command to cmwcall
    */
    cmd_info_ptr->call_id                       = call_id;

    /* This helps cmwcall to find connection id for sending
    ** request down to MN
    */
    cmd_mode_info_ptr->sups_cmd_params.call_id  = call_id;

    cmd_mode_info_ptr->ccbs_recall_rsp          = ccbs_recall_rsp;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
  return TRUE;

  #else /* CM_GW_SUPPORTED */

  /* Lint suppressions
  */
  SYS_ARG_NOT_USED (ccbs_recall_rsp);
  SYS_ARG_NOT_USED (client_id);
  SYS_ARG_NOT_USED (call_id);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_RECALL_RSP,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of CM_GW_SUPPORTED */

} /* cm_mm_call_cmd_recall_rsp () */

#if (defined CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK) || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
/*===========================================================================

FUNCTION cm_mm_call_cmd_modify_req_internal

DESCRIPTION
  Sends a request for in-call (connected state) modification of the call type.
  Call needs to be connected for this request to be processed.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

RETURN VALUE
  TRUE   - Cmd has been queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
static boolean cm_mm_call_cmd_modify_req_internal (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  cm_call_type_e_type           new_call_type_req,
    /* New call type requested for call modification */

  const cm_ip_call_attrib_info_s_type  *new_call_attrib
   /* New call attribute info for audio and video */
)
{
  #if (defined CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK) || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
  cm_call_cmd_s_type                *call_cmd_ptr;
  cm_call_cmd_info_s_type           *cmd_info_ptr;
  cm_call_cmd_info_gw_cs_s_type     *cmd_mode_info_ptr;
  cm_call_mode_info_e_type          call_mode;
  cm_call_cmd_info_ip_s_type        *ip_cmd_mode_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2( " CM_CALL_CMD_MO_MODIFY_REQ client = %d, new_call_type_req = %d",
                client_id, new_call_type_req);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr = cm_cmd_alloc_call_init();
    /* Get a reference to command information.
    */
    cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );

    call_mode = cmcall_call_id_mode( call_id );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_MO_MODIFY_REQ;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (!BETWEEN(new_call_type_req, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX))
    {
      CM_ERR_1 ("Call type for modification not valid %d", new_call_type_req);
    }

    /* Copy command information fields.
    */

    cmd_info_ptr->call_id                       = call_id;

    if(call_mode == CM_CALL_MODE_INFO_IP)
    {
      ip_cmd_mode_info_ptr = &(call_cmd_ptr->info.ip_call);
      ip_cmd_mode_info_ptr->ipcall_attrib_info = *new_call_attrib;
      ip_cmd_mode_info_ptr->new_call_type_req = new_call_type_req;
    }
    else
    {
      cmd_mode_info_ptr  = WCALL_CMD_INFO_PTR( call_cmd_ptr );
      cmd_mode_info_ptr->new_call_type_req        = new_call_type_req;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
    return TRUE;

  #else /* (CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(new_call_type_req);
  SYS_ARG_NOT_USED(new_call_attrib);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MO_MODIFY_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

} /* cm_mm_call_cmd_modify_req () */
#endif // CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK || defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

/*===========================================================================

FUNCTION cm_mm_call_cmd_modify_req

DESCRIPTION
  Sends a request for in-call (connected state) modification of the call type.
  Call needs to be connected for this request to be processed.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

RETURN VALUE
  TRUE   - Cmd has been queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_cmd_modify_req (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  cm_call_type_e_type           new_call_type_req
    /* New call type requested for call modification */
)
{
  #if (defined CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
#else /* (CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(new_call_type_req);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MO_MODIFY_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

} /* cm_mm_call_cmd_modify_req () */


/*===========================================================================

FUNCTION cm_mm_call_cmd_modify_req_ext

DESCRIPTION
  Sends a request for in-call (connected state) modification of the call type.
  Call needs to be connected for this request to be processed. Added call attribute info for multimedia
  call.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

RETURN VALUE
  TRUE   - Cmd has been queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_cmd_modify_req_ext (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  cm_call_type_e_type           new_call_type_req,
    /* New call type requested for call modification */

  const cm_ip_call_attrib_info_s_type  *call_attrib
   /* New call attribute info for audio and video */
)
{
  #if (defined FEATURE_IP_CALL && defined FEATURE_CM_LTE)

  return cm_mm_call_cmd_modify_req_internal(cmd_cb_func,
                                       data_block_ptr,
                                       client_id,
                                       call_id,
                                       new_call_type_req,
                                       call_attrib);

  #else

  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(new_call_type_req);
  SYS_ARG_NOT_USED(call_attrib);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MO_MODIFY_REQ,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif
} /* cm_mm_call_cmd_modify_req_ext () */

#if (defined CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK) || (defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE))
/*===========================================================================

FUNCTION cm_mm_call_cmd_modify_res_internal

DESCRIPTION
  Response to network initiated in-call modification request.
  Call needs to be connected for this request to be processed.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE   - Cmd got queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
static boolean cm_mm_call_cmd_modify_res_internal (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  boolean                       modify_res,
    /* Client response to modification request
    ** TRUE   - Modification is accepted by client
    ** FALSE  - Modification gets rejected by client
    */

  ie_cm_cc_cause_s_type         *cc_cause_ptr,
    /* Specific cause value when gw_cs_modify_res is FALSE
    ** Ignored when gw_cs_modify_res is TRUE.
    */

  cm_call_type_e_type           call_type_rsp,
    /* New call type respond for call modification */

  const cm_ip_call_attrib_info_s_type  *call_attrib
   /* New call attribute info for audio and video */

)
/*lint -esym(818, cc_cause_ptr)*/
{
  #if ((defined CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK) || \
       (defined FEATURE_IP_CALL && defined FEATURE_CM_LTE))
  cm_call_cmd_s_type                *call_cmd_ptr;
  cm_call_cmd_info_s_type           *cmd_info_ptr;
  cm_call_cmd_info_gw_cs_s_type     *cmd_mode_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2( "CM_CALL_CMD_MODIFY_RES client = %d, gw_cs_modify_res = %d",
                client_id, modify_res);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_cmd_ptr = cm_cmd_alloc_call_init();


    /* Get a reference to command information.
    */
    call_cmd_ptr->cmd_cb_func     = cmd_cb_func;
    call_cmd_ptr->data_block_ptr  = data_block_ptr;
    call_cmd_ptr->client_id       = client_id;
    call_cmd_ptr->cmd             = CM_CALL_CMD_MODIFY_RES;

    cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );
    cmd_info_ptr->call_id                       = call_id;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command information fields. */

    if(cmcall_call_id_mode( call_id ) == CM_CALL_MODE_INFO_IP)
    {
      call_cmd_ptr->info.ip_call.modify_response = modify_res;
      call_cmd_ptr->info.ip_call.ipcall_attrib_info = *call_attrib;
      call_cmd_ptr->info.ip_call.new_call_type_req = call_type_rsp;
    }
    else
    {
      cmd_mode_info_ptr  = WCALL_CMD_INFO_PTR( call_cmd_ptr );
      cmd_mode_info_ptr->gw_cs_modify_res         = modify_res;
      if (cc_cause_ptr)
      {
        cmd_mode_info_ptr->cc_cause                 = *cc_cause_ptr;
      }
      else
      {
        cmd_mode_info_ptr->cc_cause.present         = FALSE;
      }
    }



    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
    return TRUE;

  #else /* (CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(modify_res);
  SYS_ARG_NOT_USED(cc_cause_ptr);
  SYS_ARG_NOT_USED(call_attrib);
  SYS_ARG_NOT_USED(call_type_rsp);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MODIFY_RES,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

} /* cm_mm_call_cmd_modify_res */

#endif //CM_GW_SUPPORTED && defined FEATURE_MULTIMEDIA_FALLBACK || defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

/*===========================================================================

FUNCTION cm_mm_call_cmd_modify_res

DESCRIPTION
  Response to network initiated in-call modification request.
  Call needs to be connected for this request to be processed.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

RETURN VALUE
  TRUE   - Cmd got queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_cmd_modify_res (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  boolean                       modify_res,
    /* Client response to modification request
    ** TRUE   - Modification is accepted by client
    ** FALSE  - Modification gets rejected by client
    */

  ie_cm_cc_cause_s_type         *cc_cause_ptr
    /* Specific cause value when gw_cs_modify_res is FALSE
    ** Ignored when gw_cs_modify_res is TRUE.
    */

)
/*lint -esym(818, cc_cause_ptr)*/
{
  #if (defined CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK))
  #error code not present
#else /* (CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(modify_res);
  SYS_ARG_NOT_USED(cc_cause_ptr);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MODIFY_RES,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif /* else of (CM_GW_SUPPORTED && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

} /* cm_mm_call_cmd_modify_res */
/*lint +esym(818, cc_cause_ptr)*/


/*===========================================================================

FUNCTION cm_mm_call_cmd_modify_res_ext

DESCRIPTION
  Response to network initiated in-call modification request.
  Call needs to be connected for this request to be processed. _ext API takes call attribute parameter


DEPENDENCIES
  cm_init_before_task_start() must have already been called.
  ((FEATURE_WCDMA || FEATURE_GSM) && CM_API_VT_FALLBACK_TO_VOICE) must be
  defined.

RETURN VALUE
  TRUE   - Cmd got queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
/*lint -esym(714, cm_mm_call_cmd_modify_res_ext) */
/*lint -esym(765, cm_mm_call_cmd_modify_res_ext) */
boolean cm_mm_call_cmd_modify_res_ext (

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */

  boolean                       modify_res,
    /* Client response to modification request
    ** TRUE   - Modification is accepted by client
    ** FALSE  - Modification gets rejected by client
    */

  ie_cm_cc_cause_s_type         *cc_cause_ptr,
    /* Specific cause value when gw_cs_modify_res is FALSE
    ** Ignored when gw_cs_modify_res is TRUE.
    */
  cm_call_type_e_type           call_type_rsp,
    /* New call type respond for call modification */

  const cm_ip_call_attrib_info_s_type  *call_attrib
   /* New call attribute info for audio and video */

)
/*lint -esym(818, cc_cause_ptr)*/
{
  #if (defined FEATURE_IP_CALL && defined FEATURE_CM_LTE)

  return cm_mm_call_cmd_modify_res_internal (cmd_cb_func,
                                        data_block_ptr,
                                        client_id,
                                        call_id,
                                        modify_res,
                                        cc_cause_ptr,
                                        call_type_rsp,
                                        call_attrib);
  #else

  SYS_ARG_NOT_USED(call_id);
  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(modify_res);
  SYS_ARG_NOT_USED(cc_cause_ptr);
  SYS_ARG_NOT_USED(call_attrib);
  SYS_ARG_NOT_USED(call_type_rsp);

  if( cmd_cb_func != NULL )
  {
    cmd_cb_func( data_block_ptr, CM_CALL_CMD_MODIFY_RES,
      CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
  }
  return FALSE;
  #endif

} /* cm_mm_call_cmd_modify_res_ext */


/*===========================================================================

FUNCTION cmrpm_check_reset_allowed

DESCRIPTION
  Check whether reset is allowed or not when RPM is on

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_init().

RETURN VALUE
  TRUE if reset is allowed
  FALSE if reset is blocked

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_rpm_check_reset_allowed( void
)
{

  cm_ph_cmd_s_type          *ph_cmd_ptr;
  boolean                   ret;

  ret = cmrpm_check_reset_allowed();

  CM_MSG_HIGH_1("cm rpm ret %d",ret);

  if(ret == TRUE)
  {
    ret = cmrpm_lte_check_reset_allowed();
    CM_MSG_HIGH_1("cm rpm lte ret %d",ret);
  }

  /* If CMRPM has rejected reset queue command to CM to indicate
   ** application initiated reset was rejected */

  if(ret == FALSE)
  {

    ph_cmd_ptr = cm_cmd_alloc_ph_init();

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy command header fields.
    */
    ph_cmd_ptr->cmd                = CM_PH_CMD_RPM_APP_RESET_REJECTED;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Queue the command to CM.
    */
    cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );
  }

  return ret;
}

/*===========================================================================

FUNCTION cm_mm_call_comp_cap_cb_reg

DESCRIPTION
  Clients call this function to register call backs that can be used to
  receive capability and compatibility information for a call type.

  BC, LLC and HLC values are differnet for  call types. For mixed mode call
  origination, a voice call needs to carry BC, LLC, HLC details for a VT
  call. CM provides a storage for these values so that clients can query
  for other call type's details. Example - UI can query bearer details for
  VT call in order to originate a mixed mode call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE   - Callback could be registered successfully
  FALSE  - Callback could not be registered successfully.

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mm_call_comp_cap_cb_reg(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* requesting client ID */

  cm_call_type_e_type    call_type,
    /* Call type for which call back has to be set */

  cm_call_comp_cap_cb_f_type *comp_cap_cb_func_ptr
    /* Callback that retrieves compatibility and capability for the
    ** call type.
    */
)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(818,data_block_ptr)*/
{
  #ifdef CM_GW_SUPPORTED

  unsigned int index=0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  SYS_ARG_NOT_USED(data_block_ptr);
  SYS_ARG_NOT_USED(client_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2( "cm_mm_call_comp_cap_cb_reg() client = %d, call_type = %d", client_id, call_type);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ((call_type != CM_CALL_TYPE_VOICE) &&
        (call_type != CM_CALL_TYPE_VT)
       )
       ||
       (!comp_cap_cb_func_ptr)
     )
  {
    CM_ERR_0 ("Incorrect parameters to cm_mm_call_comp_cap_cb_reg");

    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_MODIFY_RES,
        CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }

    return FALSE;
  }

  /* Find the entry in callback table to store call back func ptr
  */
  for ( index = 0; index < ARR_SIZE( call_comp_cap_cb_store ); index++ )
  {
    if ( call_type == call_comp_cap_cb_store[index].call_type )
    {

      call_comp_cap_cb_store[index].call_comp_cap_func_ptr =
                                            comp_cap_cb_func_ptr;
      return TRUE;
    }
  }


  #else /* CM_GW_SUPPORTED */

  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(call_type);
  SYS_ARG_NOT_USED(comp_cap_cb_func_ptr);
  SYS_ARG_NOT_USED(data_block_ptr);

  #endif /* else of CM_GW_SUPPORTED */

  return FALSE;

} /* cm_mm_call_comp_cap_cb_reg */
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(818,data_block_ptr)*/

/*===========================================================================

FUNCTION cm_mm_get_call_comp_cap

DESCRIPTION
  Return the call compatibility and capability for given call type.


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  TRUE   - Call compatibiliy and capability being returned.
  FALSE  - Call compatibiliy and capability could not be returned.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_mm_get_call_comp_cap(

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* requesting client ID */

  cm_call_type_e_type    call_type,
    /* Call type */

  cm_call_comp_cap_info_s_type   *call_comp_cap_info_ptr
    /* Pointer used to return compatibility and capability for call type */

)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(818,data_block_ptr)*/
/*lint -esym(818,call_comp_cap_info_ptr)*/
{
  #ifdef CM_GW_SUPPORTED
  unsigned int index=0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(data_block_ptr);
  SYS_ARG_NOT_USED(client_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2( "cm_mm_get_call_comp_cap() client = %d, call_type = %d", client_id, call_type);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( (call_type != CM_CALL_TYPE_VOICE) &&
         (call_type != CM_CALL_TYPE_VT)
       )
       ||
       (call_comp_cap_info_ptr == NULL)
     )
  {
    CM_ERR_0 ("Incorrect parameters to cm_mm_call_comp_cap_cb_reg");

    return FALSE;
  }

  /* Clear call_comp_cap_info argument
  */
  memset (call_comp_cap_info_ptr, 0, sizeof(cm_call_comp_cap_info_s_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find callback pertaining to the call type and retrieve
  ** call compatibility and capability
  */
  for ( index = 0; index < ARR_SIZE( call_comp_cap_cb_store ); index++ )
  {
    if ( (call_type == call_comp_cap_cb_store[index].call_type ) &&
         (call_comp_cap_cb_store[index].call_comp_cap_func_ptr)
     )
    {

      call_comp_cap_cb_store[index].call_comp_cap_func_ptr
                                               (call_comp_cap_info_ptr);
      return TRUE;
    }
  }

  #else /* CM_GW_SUPPORTED */

  SYS_ARG_NOT_USED(client_id);
  SYS_ARG_NOT_USED(call_type);
  SYS_ARG_NOT_USED(call_comp_cap_info_ptr);
  SYS_ARG_NOT_USED(data_block_ptr);

  #endif /* else of CM_GW_SUPPORTED */

  return FALSE;

} /* cm_mm_get_call_comp_cap () */
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(818,data_block_ptr)*/
/*lint +esym(818,call_comp_cap_info_ptr)*/



/*===========================================================================
=============================================================================
==================== DATA BURST MESSAGE INTERFACE BLOCK ======================
=============================================================================
===========================================================================*/

/* This block group all the functionality that is associated with
** data burst message interfacing.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/*===========================================================================

FUNCTION cmdbm_ptr

DESCRIPTION
  Return a pointer to the one and only dbm object.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to dbm object

SIDE EFFECTS
  none

===========================================================================*/
cm_dbm_s_type  *cmdbm_ptr( void )
{

  static cm_dbm_s_type   cm_dbm; /* CM data burst message interface object.*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cm_dbm;

} /* cmdbm_ptr() */




/*===========================================================================

FUNCTION cm_dbm_init

DESCRIPTION
  Initialize the CM Data Burst Message interface object.

  This function must be called before the CM data burst message interface object
  is being used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cm_dbm_init) cm_dbm_init() not referenced
** It's not called in some configurations and can't turn it back on at the
** end of the function as Lint will then continue complaining about it
*/
void cm_dbm_init( void )
{

  cm_dbm_s_type    *dbm_ptr = cmdbm_ptr();
  /* point at position determination interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dbm_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize callback function pointers to NULL
  */
  dbm_ptr->cm_pd_resp_is_within_t2m_func     = NULL;
}



/*===========================================================================

FUNCTION cm_pd_reg

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
cm_pd_status_e_type cm_pd_reg(

    cm_pd_resp_is_within_t2m_f_type     *cm_pd_resp_is_within_t2m_func
        /* pointer position determination function  types */
)
/*lint -esym(715, cm_pd_resp_is_within_t2m_func)*/
{
  cm_dbm_s_type    *dbm_ptr = cmdbm_ptr();
  /* point at position determination interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( dbm_ptr != NULL );


  if(cm_pd_resp_is_within_t2m_func == NULL)
  {
    return  CM_PD_ERR_NULL_CHECK_REQ_FUNC;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dbm_ptr->cm_pd_resp_is_within_t2m_func      = cm_pd_resp_is_within_t2m_func;

  return  CM_PD_OK;
}
/*lint +esym(715, cm_pd_resp_is_within_t2m_func)*/


/*===========================================================================
=============================================================================
================ NUMBER-CLASSFICATION INTERFACE BLOCK =======================
=============================================================================
===========================================================================*/


/* This block group all the functionality that is associated with
** number-classfication interfacing.
**
** When call manager is commanded to originate a call, it consults the
** number-classfication of the following information:
**
** 1. Whether the dialed number matches one of the special
**    numbers that are stored in the number-classfication (i.e. E911, OTASP, etc.)
**    If a match is found, CM modify the originating call type accordingly.
**
** 2. After deciding on the originating call type, CM consults the
**    number-classfication whether the dialed number is allowed by the
**    origination restriction currently in effect. If the dialed number
**    is NOT allowed, CM blocks the outgoing call and notifies clients of
**    origination failure.
**
** Note that CM expects a UI like client to register a number-classfication callback
** functions with CM during initialization time.
**
** Until such callback functions are registered with CM, CM uses a default
** hard-coded number-classfication to decide on the call type of special numbers.
*/


/* Type to hold number classification interface related information
*/
typedef struct cm_nc_s {

    #ifdef FEATURE_MMGSDI_SESSION_LIB
    cm_nc_call_control_processing_extn_f_type
                                   *cm_nc_call_control_processing_func_extn;
        /* pointer to a call control processing function */


    cm_nc_call_control_processing_mod_extn_f_type
                                *cm_nc_call_control_processing_func_mod_extn;
        /* pointer to a call control processing function with addnl params */

    #endif  /* FEATURE_MMGSDI_SESSION_LIB */

    cm_nc_resolve_call_type_f_type     *resolve_call_type_func;
        /* pointer to a number-classfication function resolving call types */

    cm_nc_is_call_allowed_f_type       *cm_nc_is_call_allowed_func;
        /* pointer to a number-classfication function deciding whether a
           specific call type is allowed for origination */

    #if defined(FEATURE_MMODE_DUAL_SIM)
    cm_nc_resolve_call_type_per_subs_f_type
                                       *resolve_call_type_func_per_subs;
        /* pointer to a number-classfication function resolving call types */

    cm_nc_is_call_allowed_per_subs_f_type
                                       *cm_nc_is_call_allowed_func_per_subs;
        /* pointer to a number-classfication function deciding whether a
           specific call type is allowed for origination */
    #endif

    #ifdef FEATURE_JCDMA
    cm_nc_pre_pending_dial_f_type      *pre_pending_dial_func;
        /* pointer pre-pending function resolving origination # */
    #endif /* FEATURE_JCDMA */

    cm_nc_call_control_processing_f_type
                                        *cm_nc_call_control_processing_func;
        /* pointer to a call control processing function */

    cm_nc_call_control_preferences_f_type
                                        *cm_nc_call_control_preferences_func;
        /* pointer to a call control preferences function */

    cm_nc_call_control_processing_mod_f_type
                                    *cm_nc_call_control_processing_func_mod;
        /* pointer to a call control processing function with addnl params */

} cm_nc_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* CM number-classfication interface object.
*/
static cm_nc_s_type    cm_nc;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to return a pointer to CM interface object
**
** NOTE! This macro should only be used within the
** number-classfication object scope.
*/
#define CM_NC_PTR() ( &cm_nc )


/*===========================================================================

FUNCTION cm_nc_init

DESCRIPTION
  Initialize the CM number-classficationinterface object.

  This function must be called before the CM number-classfication interface object
  is being used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_nc_init( void )
{

  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at phone-book interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize callback function pointers to NULL
  */
  nc_ptr->resolve_call_type_func              = NULL;
  nc_ptr->cm_nc_is_call_allowed_func          = NULL;
  nc_ptr->cm_nc_call_control_processing_func  = NULL;
  nc_ptr->cm_nc_call_control_preferences_func = NULL;
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  nc_ptr->cm_nc_call_control_processing_func_extn= NULL;
  nc_ptr->cm_nc_call_control_processing_func_mod_extn = NULL;
  #endif
  #ifdef FEATURE_JCDMA
  nc_ptr->pre_pending_dial_func               = NULL;
  #endif /* FEATURE_JCDMA */
  nc_ptr->cm_nc_call_control_processing_func_mod  = NULL;
}


/*===========================================================================

FUNCTION cm_nc_reg

DESCRIPTION
  Register number-classfication callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classfication callback functions of the following information:

  1. Whether the dialed number matches one of the special
     numbers that are stored in the number-classfication (i.e. E911, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. After deciding on the originating call type, CM consults the
     number-classfication whether the dialed number is allowed by the
     origination restriction currently in effect. If dialed number
     is not allowed, CM blocks the outgoing call and notifies clients of
     origination failure.

DEPENDENCIES
  number-classfication interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_nc_status_e_type cm_nc_reg(

    cm_nc_resolve_call_type_f_type     *resolve_call_type_func,
        /* pointer phone-book function resolving call types */

    cm_nc_is_call_allowed_f_type        *nc_is_call_allowed_func,
        /* pointer to a phone-book deciding whether a specific call type
        ** is allowed for origination */

    cm_nc_pre_pending_dial_f_type      *pre_pending_dial_func
        /* pointer pre-pending function resolving origination # */
)
/*lint -esym(715, pre_pending_dial_func)*/
{

  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at number-classfication interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(resolve_call_type_func == NULL)
  {
    return  CM_NC_ERR_NULL_CALL_TYPE;
  }
  else if (nc_is_call_allowed_func == NULL)
  {
    return  CM_NC_ERR_NULL_CALL_ALLOWED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nc_ptr->resolve_call_type_func      = resolve_call_type_func;
  nc_ptr->cm_nc_is_call_allowed_func  = nc_is_call_allowed_func;
  #ifdef FEATURE_JCDMA
  nc_ptr->pre_pending_dial_func       = pre_pending_dial_func;
  #endif /* FEATURE_JCDMA */

  return  CM_NC_OK;
}
/*lint +esym(715, pre_pending_dial_func)*/


/*===========================================================================

FUNCTION cm_nc_reg_with_call_control

DESCRIPTION
  Register number-classification callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classification callback function for the following information:

  1. Whether the originating number matches one of the special
     numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. Whether the originating number is allowed on the sims present.

  3. And if the calls are allowed and call control is required, perform any necessary
     modifications.


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_nc_status_e_type cm_nc_reg_with_call_control(

  cm_nc_call_control_processing_f_type    *cm_nc_call_control_processing_func,
    /* ptr to function that will resolve call type, check whether the call is
       allowed, and do any call control modifications if necessary and for
       JCDMA any pre pending dial information.
    */

  cm_nc_call_control_preferences_f_type   *cm_nc_call_control_preferences_func
    /* ptr to function that will resolve which mode to originate on when the
       call is allowed on both slots and/or the modifications are conflicting.
    */

)
{

  cm_nc_s_type                       *nc_ptr = CM_NC_PTR();
    /* point at number-classification interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cm_nc_call_control_processing_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PROCESSING_FUNC;
  }

  if (cm_nc_call_control_preferences_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PREFERENCES_FUNC;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nc_ptr->cm_nc_call_control_processing_func = cm_nc_call_control_processing_func;
  nc_ptr->cm_nc_call_control_preferences_func = cm_nc_call_control_preferences_func;
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  nc_ptr->cm_nc_call_control_processing_func_extn= NULL;
  #endif

  return CM_NC_OK;
}

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================

@FUNCTION cm_nc_reg_with_call_control_extn

@DESCRIPTION
   Registers number classification callback functions with the
   CM.

   When the CM is commanded to originate a call, it consults
   the number classification callback function for the following
   information:

   1. Whether the originating number matches one of the special
      numbers that are stored in the number classification
      (i.e., Emergency, OTASP, etc.) If a match is found, the
      CM modifies the originating call type accordingly.

   2. Whether the originating number is allowed on the sims
      present.

   3. If the calls are allowed and call control is required,
      perform any necessary modifications.


@DEPENDENCIES
   The CM must have already been initialized with
   cm_init_before_task_start().

@RETURNS
   Request status.

@x
===========================================================================*/
cm_nc_status_e_type cm_nc_reg_with_call_control_extn(

  cm_nc_call_control_processing_extn_f_type    *cm_nc_call_control_processing_func,
    /**< ptr to function that will resolve call type, check whether the call is
    ** allowed, and do any call control modifications if necessary and for
    ** JCDMA any pre pending dial information.
    */

  cm_nc_call_control_preferences_f_type *cm_nc_call_control_preferences_func
    /**< ptr to function that will resolve which mode to originate on when the
    ** call is allowed on both slots and/or the modifications are conflicting.
    */

)

{

  cm_nc_s_type                       *nc_ptr = CM_NC_PTR();
    /* point at number-classification interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cm_nc_call_control_processing_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PROCESSING_FUNC;
  }

  if (cm_nc_call_control_preferences_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PREFERENCES_FUNC;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nc_ptr->cm_nc_call_control_processing_func_extn = cm_nc_call_control_processing_func;
  nc_ptr->cm_nc_call_control_preferences_func = cm_nc_call_control_preferences_func;


  /* If new Call control callbacks are registered, old should be automatically
    be set to NULL */
  nc_ptr->cm_nc_call_control_processing_func = NULL;

  return CM_NC_OK;
}
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================

FUNCTION cm_nc_reg_with_call_control_mod

DESCRIPTION
  Register number-classification callback functions with CM

  When call manager is commanded to originate a call, it consults the
  number-classification callback function for the following information:

  1. Whether the originating number matches one of the special
     numbers that are stored in the number-classification (i.e. EMERGENCY, OTASP, etc.)
     If a match is found, CM modify the originating call type accordingly.

  2. Whether the originating number is allowed on the sims present.

  3. And if the calls are allowed and call control is required, perform any necessary
     modifications.


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
cm_nc_status_e_type cm_nc_reg_with_call_control_mod(

  cm_nc_call_control_processing_mod_f_type
                                    *cm_nc_call_control_processing_func_mod,
    /* ptr to function that will resolve call type, check whether the call is
       allowed, and do any call control modifications if necessary and for
       JCDMA any pre pending dial information.
    */

  cm_nc_call_control_preferences_f_type
                                        *cm_nc_call_control_preferences_func
    /* ptr to function that will resolve which mode to originate on when the
       call is allowed on both slots and/or the modifications are conflicting.
    */

)
{

  cm_nc_s_type                       *nc_ptr = CM_NC_PTR();
    /* point at number-classification interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cm_nc_call_control_processing_func_mod == NULL)
  {
    return CM_NC_ERR_NULL_CC_PROCESSING_FUNC;
  }

  if (cm_nc_call_control_preferences_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PREFERENCES_FUNC;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nc_ptr->cm_nc_call_control_processing_func_mod = cm_nc_call_control_processing_func_mod;
  nc_ptr->cm_nc_call_control_preferences_func = cm_nc_call_control_preferences_func;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  /* If old Call control callbacks are registered and feature DUAL_SIM is defined then
  ** new should be automatically be set to NULL */

  nc_ptr->cm_nc_call_control_processing_func_mod_extn = NULL;
  #endif

  return CM_NC_OK;
}

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*=====================================================

FUNCTION: cm_nc_reg_with_call_control_mod_extn
  Registers number classification callback functions with the
  CM.

  When the CM is commanded to originate a call, it consults
  the number classification callback function for the following
  information:

  1.  Whether the originating number matches one of the special
      numbers that are stored in the number classification
      (i.e., Emergency, OTASP, etc.) If a match is found, the
      CM modifies the originating call type accordingly.</li>

  2.  Whether the originating number is allowed on the sims
      present.

  3.  and if the calls are allowed and call control is required,
      perform any necessary modifications.


@DEPENDENCIES
  The CM must have already been initialized with
  cm_init_before_task_start().

@RETURNS
  Request status.

@SIDE_EFFECT
  None.

@x
===========================================================================*/
cm_nc_status_e_type cm_nc_reg_with_call_control_mod_extn(

  cm_nc_call_control_processing_mod_extn_f_type
                                    *cm_nc_call_control_processing_func_mod,
    /* ptr to function that will resolve call type, check whether the call is
       allowed, and do any call control modifications if necessary and for
       JCDMA any pre pending dial information.
    */

  cm_nc_call_control_preferences_f_type
                                        *cm_nc_call_control_preferences_func
    /* ptr to function that will resolve which mode to originate on when the
       call is allowed on both slots and/or the modifications are conflicting.
    */

)

{

  cm_nc_s_type                       *nc_ptr = CM_NC_PTR();
    /* point at number-classification interface object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cm_nc_call_control_processing_func_mod == NULL)
  {
    return CM_NC_ERR_NULL_CC_PROCESSING_FUNC;
  }

  if (cm_nc_call_control_preferences_func == NULL)
  {
    return CM_NC_ERR_NULL_CC_PREFERENCES_FUNC;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nc_ptr->cm_nc_call_control_processing_func_mod_extn = cm_nc_call_control_processing_func_mod;
  nc_ptr->cm_nc_call_control_preferences_func = cm_nc_call_control_preferences_func;

    /* If new Call control callbacks are registered, old should be automatically be set to NULL */
  nc_ptr->cm_nc_call_control_processing_func_mod = NULL;

  return CM_NC_OK;
}
#endif /* FEATURE_MMGSDI_SESSION_LIB */


/* This is used only by cmwcall/cmxcall, must be moved */


/*===========================================================================

FUNCTION cm_nc_resolve_call_type_per_subs

DESCRIPTION
  Resolve originating call type.

  This functions consults the phone-book to whether the dialed number
  matches one of the special numbers that are stored in the phone-book
  (i.e. E911, OTASP, etc.). If a match is found, it modifies the originating
  call type accordingly.

DEPENDENCIES
  Phone-book interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  none

SIDE EFFECTS
  Modifies the originating call type (pointed by activate_code_ptr) and or
  the activation code (pointed by activate_code_ptr).

===========================================================================*/
static void cm_nc_resolve_call_type_per_subs(

    const cm_num_s_type            *orig_num_ptr,
        /* dialed number */

    cm_call_type_e_type      *call_type_ptr,
        /* Associated call type */

    cm_activate_code_e_type  *activate_code_ptr,
        /* Associated OTASP activation code (for OTASP call types) */

    sys_modem_as_id_e_type   asubs_id
)
{
  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at number-classfication interface object */

  #ifdef FEATURE_MMODE_DUAL_SIM
  cm_nc_resolve_call_type_per_subs_f_type
                                     *resolve_call_type_func_per_subs;
        /* pointer to a number-classfication function resolving call types */
  #endif
  cm_nc_resolve_call_type_f_type     *resolve_call_type_func =
                                              nc_ptr->resolve_call_type_func;
      /* pointer to a number-classfication function resolving call types */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(orig_num_ptr      != NULL);
  CM_ASSERT(call_type_ptr     != NULL);
  CM_ASSERT(activate_code_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is not a voice call, return now.
  */
  if( *call_type_ptr != CM_CALL_TYPE_VOICE )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Consult the phone-book callback function of the type
  ** of the dialed number.
  */
  #ifdef FEATURE_MMODE_DUAL_SIM
  /* Making a copy of the callback, so that callback pointer is not changed
  ** Just before calling the callback */
  resolve_call_type_func_per_subs = nc_ptr->resolve_call_type_func_per_subs;
  if(resolve_call_type_func_per_subs != NULL)
  {
    resolve_call_type_func_per_subs( orig_num_ptr,
                                  call_type_ptr,
                                  activate_code_ptr,
                                  asubs_id
                                );
  }
  else
  #else
  SYS_ARG_NOT_USED(asubs_id);
  #endif
  if(resolve_call_type_func != NULL)
  {
    resolve_call_type_func( orig_num_ptr, call_type_ptr, activate_code_ptr);
  }
  else
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If FEATURE_OTASP is NOT defined, make sure that the callback
  ** did not set the call type to OTASP. If it did, default the call
  ** type back to VOICE.
  */
  #ifndef FEATURE_OTASP

  if( CM_CALL_TYPE_IS_OTASP(*call_type_ptr) )
  {
    CM_ERR_0( "FEATURE_OTASP is NOT defined");
    *call_type_ptr = CM_CALL_TYPE_VOICE;
  }

  #endif /* FEATURE_OTASP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If FEATURE_E911 is NOT defined, make sure that the phone-book
  ** did not set the call type to E911. If it did, default the call
  ** type back to VOICE.
  */
  #if !defined(FEATURE_E911) && !(defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)) && !defined(FEATURE_GSM)
  if( *call_type_ptr == CM_CALL_TYPE_EMERGENCY )
  {
    CM_ERR_0( "FEATURE_E911 is NOT defined");
    *call_type_ptr = CM_CALL_TYPE_VOICE;
  }
  #endif  /* ! FEATURE_E911 && ! FEATURE_WCDMA && ! FEATURE_GSM */
} /* cm_nc_resolve_call_type_per_subs() */


/*===========================================================================

FUNCTION cm_nc_resolve_call_type

DESCRIPTION
  Resolve originating call type.

  This functions consults the phone-book to whether the dialed number
  matches one of the special numbers that are stored in the phone-book
  (i.e. E911, OTASP, etc.). If a match is found, it modifies the originating
  call type accordingly.

DEPENDENCIES
  Phone-book interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  none

SIDE EFFECTS
  Modifies the originating call type (pointed by activate_code_ptr) and or
  the activation code (pointed by activate_code_ptr).

===========================================================================*/
void cm_nc_resolve_call_type(

    const cm_num_s_type      *orig_num_ptr,
        /* dialed number */

    cm_call_type_e_type      *call_type_ptr,
        /* Associated call type */

    cm_activate_code_e_type  *activate_code_ptr
        /* Associated OTASP activation code (for OTASP call types) */
)
{
  cm_nc_resolve_call_type_per_subs( orig_num_ptr,
                                    call_type_ptr,
                                    activate_code_ptr,
                                    SYS_MODEM_AS_ID_1
                                  );
}


/* This is used only by cmxcall/cmwcall (future, when SUPS will be merged) */

/*===========================================================================

FUNCTION cm_nc_is_call_allowed_func_per_subs

DESCRIPTION
  Decide whether a dialed number is allowed.

  This functions consults the number-classfication to whether the dialed number
  is allowed by the origination restriction currently in effect.

DEPENDENCIES
  number-classfication interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  TRUE of dialed number is allowed for origination, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cm_nc_is_call_allowed_func_per_subs(

    const cm_num_s_type            *orig_num_ptr,
        /* pointer to dialed number */

    sys_modem_as_id_e_type         asubs_id
)
{

  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at phone-book interface object */

  cm_nc_is_call_allowed_f_type *call_cb = nc_ptr->cm_nc_is_call_allowed_func;
      /* temporary function pointer */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Consult the number-classfication callback function to whether the originating
  ** number is allowed.
  */
  #ifdef FEATURE_MMODE_DUAL_SIM
  /* Buffer the function pointer so that, it doesn't change when callback is
  ** invoked */
  cm_nc_is_call_allowed_per_subs_f_type *call_cb_per_subs =
                                  nc_ptr->cm_nc_is_call_allowed_func_per_subs;
  if(call_cb_per_subs != NULL )
  {
    return call_cb_per_subs( orig_num_ptr, asubs_id);
  }
  else
  #else
  SYS_ARG_NOT_USED(asubs_id);
  #endif

  if(call_cb != NULL )
  {
    return call_cb( orig_num_ptr);
  }
  else
  {
    /* all call allowed */
    return TRUE;
  }

}


/*===========================================================================

FUNCTION cm_nc_is_call_allowed_func

DESCRIPTION
  Decide whether a dialed number is allowed.

  This functions consults the number-classfication to whether the dialed number
  is allowed by the origination restriction currently in effect.

DEPENDENCIES
  number-classfication interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  TRUE of dialed number is allowed for origination, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_nc_is_call_allowed_func(

    const cm_num_s_type            *orig_num_ptr
        /* pointer to dialed number */
)
{
  return cm_nc_is_call_allowed_func_per_subs( orig_num_ptr,
                                              SYS_MODEM_AS_ID_1
                                            );
}

/*===========================================================================

FUNCTION cm_nc_call_control_processing

DESCRIPTION
  This function will perform call control processing on an originating number.

DEPENDENCIES
  none

RETURN VALUE
  FALSE if CM cmd buffer allocation fails, TRUE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_nc_call_control_processing(

  cm_call_id_type                       call_id,
    /* call id */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_call_type_e_type                   call_type,
    /* call type */

  cm_num_s_type                         *orig_num_ptr,
    /* orginating number */

  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params_ptr,
    /**< Clients need to check for CM_API_GW_PS_CC before
    ** using this structure.
    **
    ** Call control parameters for Packet data call
    */

  const cm_cc_generic_params_s_type     *cc_generic_params_ptr
    /* Call-control generic parameters */

)
/*lint -esym(818, orig_num_ptr)*/
/*lint -esym(818, gw_ps_cc_params_ptr)*/
/* Can't be const, would have to change
** cm_nc_call_control_processing_f_type
** and that causes compiler errors in other subsystems
*/
{

  cm_cmd_type                     *cmd_ptr = (cm_cmd_type*) user_data;
  boolean is_modem_cc_api = FALSE;

  cm_nc_call_control_processing_mod_f_type *cc_proc_mod_func;
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_nc_call_control_processing_mod_extn_f_type *cc_proc_mod_extn_func;
  #endif

  CM_ASSERT(cmd_ptr != NULL);

  #ifdef CM_GW_SUPPORTED
  if(call_type == CM_CALL_TYPE_SUPS)
  {
    CM_ASSERT(CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SUPS);
    is_modem_cc_api = SUPS_CMD_PTR(cmd_ptr)->is_modem_cc_api;
  }
  else
  #endif
  {
    is_modem_cc_api = CALL_CMD_PTR(cmd_ptr)->is_modem_cc_api;
  }

  if( is_modem_cc_api == TRUE )
  {
   

    /* Mark the command buffer for reuse since it will be requeued by the
    ** registered number classification functions.
    */
    cmd_ptr->is_reused = TRUE;
    CM_MSG_HIGH_3("New Modem API: cmd_ptr->is_reused = %d, call_id = %d, cmd_ptr = 0x%x",
                  cmd_ptr->is_reused, call_id, cmd_ptr);

    CM_ASSERT(orig_num_ptr                     != NULL);
    CM_ASSERT(user_data                        != NULL);
    CM_ASSERT(cm_call_control_complete_cb_func != NULL);

    SYS_ARG_NOT_USED(cc_generic_params_ptr);

    /* Do Call control processing in CM */
    return cmcc_call_control_processing
    (
      call_id,
      cdma_subscription_source,
      gw_subscription_source,
      call_type,
      orig_num_ptr,
      user_data,
      cm_call_control_complete_cb_func,
      gw_ps_cc_params_ptr
    );
  }
  else
  {
    cm_nc_s_type                      *nc_ptr = CM_NC_PTR();
    cm_orig_call_control_s_type       *cdma_call_control_info
                                  = (cm_orig_call_control_s_type*)
                                  cm_mem_malloc(sizeof(cm_orig_call_control_s_type));

    cm_orig_call_control_s_type       *gw_call_control_info
                                  = (cm_orig_call_control_s_type*)
                                  cm_mem_malloc(sizeof(cm_orig_call_control_s_type));

    cm_call_type_e_type               new_call_type = call_type;
    cm_activate_code_e_type           otasp_act_code;
    #ifdef FEATURE_MMGSDI_SESSION_LIB
    cm_mmgsdi_session_info_s_type     sess_info =
                        { MMGSDI_GW_PROV_PRI_SESSION, 0};
    cm_mmgsdi_error_e_type            mmgsdi_result = CM_MMGSDI_NOTFOUND;
    #endif

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

    CM_MSG_HIGH_0 (" Old Modem API ");
    CM_ASSERT(cdma_call_control_info  != NULL);
    CM_ASSERT(gw_call_control_info  != NULL);
    CM_ASSERT(orig_num_ptr                     != NULL);
    CM_ASSERT(user_data                        != NULL);
    CM_ASSERT(cm_call_control_complete_cb_func != NULL);

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    memset(cdma_call_control_info,0,sizeof(cm_orig_call_control_s_type));
    memset(gw_call_control_info,0,sizeof(cm_orig_call_control_s_type));

    /* Mark the command buffer for reuse since it will be requeued by the
    ** registered number classification functions.
    */
    cmd_ptr->is_reused = TRUE;

    #if defined(FEATURE_MMGSDI_SESSION_LIB)
    if(gw_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_gw_slot(
        CM_GET_SLOT_FROM_XXX_SUBS_SOURCE(gw_subscription_source),
        &sess_info);
    }
    else if(cdma_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      if((cdma_subscription_source ==CM_CC_SUBSCRIPTION_SOURCE_SIM_1)||
         (cdma_subscription_source==CM_CC_SUBSCRIPTION_SOURCE_SIM_2) ||
         (cdma_subscription_source==CM_CC_SUBSCRIPTION_SOURCE_SIM_3))
      {
        mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_cdma_slot(
          CM_GET_SLOT_FROM_XXX_SUBS_SOURCE(cdma_subscription_source),
           &sess_info);
      }
      else
      {
         /* Note : this is temporary fix to allow call control on GW SIM card
         ** when cdma subscription is from NV */

        mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_slot(MMGSDI_SLOT_1,
                                                                &sess_info);
      }
    }

    if(mmgsdi_result != CM_MMGSDI_SUCCESS)
    {
      CM_MSG_HIGH_3("MMGSDI session info read failure %d, gw subs src %d, cdma subs src %d",
                    mmgsdi_result,
                     gw_subscription_source, cdma_subscription_source);
    }
    #endif /* FEATURE_MMGSDI_SESSION_LIB */

    /*----------------------------------------------------------------------*/


    cc_proc_mod_func = nc_ptr->cm_nc_call_control_processing_func_mod;

    /* If there is a registered number classification function, go ahead and
    ** just use the registered one.
    */
    #if defined(FEATURE_MMGSDI_SESSION_LIB)
    /* Check if DUAL SIM Call Control callback is registered */
    cc_proc_mod_extn_func = nc_ptr->cm_nc_call_control_processing_func_mod_extn;
    if ( cc_proc_mod_extn_func != NULL )
    {
      CM_MSG_HIGH_0 ( "Calling regd CC extn fn" );
      /* Free the allocated memory to heap before return*/
      cm_mem_free(cdma_call_control_info);
      cm_mem_free(gw_call_control_info);

      return cc_proc_mod_extn_func
      (
        call_id,
        cdma_subscription_source,
        gw_subscription_source,
        call_type,
        orig_num_ptr,
        cmd_ptr,
        cm_call_control_complete_cb_func,
        gw_ps_cc_params_ptr,
        cc_generic_params_ptr,
        sess_info
      );
    }
    else
    #endif
    if ( cc_proc_mod_func != NULL )
    {
      CM_MSG_HIGH_0 ( "Calling regd CC fn" );
      /* Free the allocated memory to heap before return*/
      cm_mem_free(cdma_call_control_info);
      cm_mem_free(gw_call_control_info);

      return cc_proc_mod_func
      (
        call_id,
        cdma_subscription_source,
        gw_subscription_source,
        call_type,
        orig_num_ptr,
        cmd_ptr,
        cm_call_control_complete_cb_func,
        gw_ps_cc_params_ptr,
        cc_generic_params_ptr
      );

    }


    CM_MSG_HIGH_0 ("No regd CC fn");

    /* If the call control is for a call type of SS.
    ** There are no default operations for this call type.
    ** Just pass it through as unmodified.
    */
    if (call_type == CM_CALL_TYPE_SUPS)
    {
      gw_call_control_info->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
      cm_call_control_complete_cb_func(
        CM_CALL_ID_UNASSIGNED,
        CM_CC_STATUS_SUCCESS,
        CM_CALL_TYPE_SUPS,
        NULL,
        gw_call_control_info,
        cmd_ptr
      );
      /* Free the allocated memory to heap before return*/
      cm_mem_free(cdma_call_control_info);
      cm_mem_free(gw_call_control_info);

      return TRUE;
    }

    /* Since there is no functions registered ....
    */

    /* First, resolve the call type */
    cm_kick_dog ();

    CM_ASSERT(CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL);
    cm_nc_resolve_call_type_per_subs(orig_num_ptr, &new_call_type,
                                     &otasp_act_code,
                                     CALL_CMD_PTR(cmd_ptr)->cmd_subs);
    cm_kick_dog ();

    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    if ((new_call_type == CM_CALL_TYPE_OTAPA) ||
        (new_call_type == CM_CALL_TYPE_STD_OTASP) ||
        (new_call_type == CM_CALL_TYPE_NON_STD_OTASP))
    {
      cdma_call_control_info->activate_code = otasp_act_code;
      CALL_CMD_PTR(cmd_ptr)->info.cdma_info.otasp_act_code = otasp_act_code;
    }
    #endif  /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

    CALL_CMD_PTR(cmd_ptr)->info.call_type = new_call_type;

    /*
    ** If he call is of type emergency, the call is allowed
    ** and there is no need to continue any further.  Just return.
    */
    if (new_call_type == CM_CALL_TYPE_EMERGENCY)
    {
      cdma_call_control_info->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
      gw_call_control_info->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

      #ifdef CM_GW_SUPPORTED
      /* new_call_type can be EMERGENCY in two ways
      **
      ** 1> call_type passed to this function is emergency. emerg service
      **    categ needs to be copied from cmd
      **
      ** 2> resolve call type changed call to emergency. Defaulting emerg srv
      **    type since resolve call type interf does not have way to give
      **    back emerg serv type
      */

      if (call_type == CM_CALL_TYPE_EMERGENCY)
      {
        gw_call_control_info->num.emerg_srv_categ =
                                    CALL_CMD_PTR(cmd_ptr)->info.num.emerg_srv_categ;
      }
      else
      {
        gw_call_control_info->num.emerg_srv_categ = CM_EMERG_NUM_TYPE_NONE;
      }
      #endif /* CM_GW_SUPPORTED */

      cm_call_control_complete_cb_func(
        call_id,
        CM_CC_STATUS_SUCCESS,
        new_call_type,
        cdma_call_control_info,
        gw_call_control_info,
        cmd_ptr
      );
      /* Free the allocated memory to heap before return*/
      cm_mem_free(cdma_call_control_info);
      cm_mem_free(gw_call_control_info);

      return TRUE;
    }
    else if (new_call_type == CM_CALL_TYPE_PS_DATA)
    {
      /* Handles when there is no call control function registered with CM
      */
      CM_MSG_HIGH_0 ("NO CC Registered, allowing PS as is");

      if (call_type != CM_CALL_TYPE_PS_DATA)
      {
        CM_ASSERT (call_type == CM_CALL_TYPE_PS_DATA);
        CM_ERR_0 ("Unexpected modification to PS call");
      }

      cdma_call_control_info->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
      gw_call_control_info->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

      /* Copy packet call control parameters as given by cmd */
      gw_call_control_info->gw_ps_cc_params = *gw_ps_cc_params_ptr;
    }

    /* Now check whether the call is allowed */
    if (!cm_nc_is_call_allowed_func_per_subs(orig_num_ptr,
                                           CALL_CMD_PTR(cmd_ptr)->cmd_subs)
        )
    {
      cdma_call_control_info->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
      gw_call_control_info->call_control_result = CM_CC_RESULT_NOT_ALLOWED;

      cm_call_control_complete_cb_func(
        call_id,
        CM_CC_STATUS_SUCCESS,
        new_call_type,
        cdma_call_control_info,
        gw_call_control_info,
        cmd_ptr
      );
      /* Free the allocated memory to heap before return*/
      cm_mem_free(cdma_call_control_info);
      cm_mem_free(gw_call_control_info);

      return TRUE;
    }

    #if defined( FEATURE_GSTK ) && (defined CM_GW_SUPPORTED)
    /* If call control is required for the GSM/WCDMA subscription source,
    ** than, perform the call control on that particular card only.
    **
    **
    ** NOTE on call control for packet calls. GSTK interface for performing
    ** call control was introduced to take care of situation when UI does
    ** not register for CC. In current builds UI registers for call control
    ** all the time and default behavior for packet calls has not been added
    ** cmcall_gstk_evt_cb();
    */
    if (gw_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      if ( (new_call_type != CM_CALL_TYPE_EMERGENCY) &&
            cmcall_is_cc_required (gw_subscription_source))
      {
        (void) cmcall_send_gstk_voice_envelope
        (
          call_id,
          orig_num_ptr,
          cmd_ptr,
          cm_call_control_complete_cb_func,
          gw_subscription_source
        );
        /* Free the allocated memory to heap before return*/
        cm_mem_free(cdma_call_control_info);
        cm_mem_free(gw_call_control_info);

        return TRUE;
      }
    }
    #endif   /* FEATURE_GST && ( FEATURE_GSM || FEATURE_WCDMA ) */
    if (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      cdma_call_control_info->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    }
    else
    {
      cdma_call_control_info->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    }
    if (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      gw_call_control_info->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    }
    else
    {
      gw_call_control_info->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    }

    cm_call_control_complete_cb_func(
      call_id,
      CM_CC_STATUS_SUCCESS,
      new_call_type,
      cdma_call_control_info,
      gw_call_control_info,
      cmd_ptr
    );
    /* Free the allocated memory to heap before return*/
    cm_mem_free(cdma_call_control_info);
    cm_mem_free(gw_call_control_info);

    return TRUE;
  } /* !is_modem_cc_api */
}
/*lint +esym(818, orig_num_ptr)*/
/*lint +esym(818, gw_ps_cc_params_ptr)*/


/*===========================================================================

FUNCTION cm_nc_proc_call_control_result

DESCRIPTION
  This function will decide which mode to originate on after call control
  is complete.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_mode_pref_e_type cm_nc_proc_call_control_result(

  cm_orig_call_control_s_type            *cdma_call_control_info_ptr,
    /* cdma information for the call */

  cm_orig_call_control_s_type            *gw_call_control_info_ptr,
    /* gw information for the call */

  cm_num_s_type                          *orig_num_ptr,
    /* Original number information before call control */

  cm_call_type_e_type                    call_type,
    /* The type of call returned by call control */

  sys_sys_mode_e_type                    current_mode
    /* mode, if any, that is acquired right now */
)
{

  /* Provide some sort of default behavior
  */

  if ((cdma_call_control_info_ptr == NULL) &&
        (gw_call_control_info_ptr == NULL))
  {
      return CM_CC_MODE_PREF_NONE;
  }
  if (cdma_call_control_info_ptr == NULL)
  {
      return CM_CC_MODE_PREF_GW_ONLY;
  }
  if (gw_call_control_info_ptr == NULL)
  {
      return CM_CC_MODE_PREF_CDMA_ONLY;
  }

  if ((cdma_call_control_info_ptr->call_control_result == CM_CC_RESULT_NOT_ALLOWED) &&
        (gw_call_control_info_ptr->call_control_result == CM_CC_RESULT_NOT_ALLOWED))
  {
      return CM_CC_MODE_PREF_NONE;
  }

  if (cdma_call_control_info_ptr->call_control_result == CM_CC_RESULT_NOT_ALLOWED)
  {
      return CM_CC_MODE_PREF_GW_ONLY;
  }
  if (gw_call_control_info_ptr->call_control_result == CM_CC_RESULT_NOT_ALLOWED)
  {
      return CM_CC_MODE_PREF_CDMA_ONLY;
  }

  return CM_CC_MODE_PREF_ANY;


} /* cm_nc_call_control_preferences */


#ifdef FEATURE_JCDMA
/*===========================================================================

FUNCTION cm_nc_pre_pending_dial_func

DESCRIPTION
  Resolve originating dial numbers for pre-pending.

  This functions insert or delete the dialed number.
  (i.e. 184, 186, etc.)

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  Modifies the originating dial numbers.

===========================================================================*/
void cm_nc_pre_pending_dial_func(

    cm_num_s_type                  *orig_num_ptr,
        /* dialed number */

    const cm_call_type_e_type      *call_type_ptr
        /* Associated call type */

)
{

  cm_nc_s_type    *nc_ptr = CM_NC_PTR();
      /* point at number-classfication interface object */

  cm_nc_pre_pending_dial_f_type      *pre_pending_dial_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( nc_ptr != NULL );

  pre_pending_dial_func = nc_ptr->pre_pending_dial_func;
  if ( pre_pending_dial_func == NULL )
  {
     return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Consult the pre-pending dial callback function of
  ** the dialed number.
  */
  pre_pending_dial_func( orig_num_ptr, call_type_ptr );

}
#endif /* FEATURE_JCDMA */



/*===========================================================================
=============================================================================
================ M51 MODE SETTINGS INTERFACE BLOCK ==========================
=============================================================================
===========================================================================*/

/* Type to hold information related to M51 mode settings interface
*/
typedef struct cm_m51_mode_settings_s {

    cm_m511_mode_settings_f_type     *change_m511_mode_func;
        /* pointer to a DS function that changes M511 mode */

    cm_m512_mode_settings_f_type     *change_m512_mode_func;
        /* pointer to a DS function that changes M512 mode */

    cm_m513_mode_settings_f_type     *change_m513_mode_func;
      /* pointer to a DS function that changes M513 mode */


} cm_m51_mode_settings_s_type;

/* CM M51 mode settings object
*/
static cm_m51_mode_settings_s_type cm_m51_mode;

/* Macro to return a pointer to M51 interface object
**
** NOTE! This macro should only be used within the
** M51 object scope.
*/
#define CM_M51_MODE_PTR() ( &cm_m51_mode )

/*===========================================================================
FUNCTION default_change_511_mode_function

DESCRIPTION

  Default handler for m511 mode change request.
  It is supposed to be replaced by DS with appropriate handler functions
  via a call to cm_m51_mode_reg()

DEPENDENCIES

RETURN VALUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean default_change_511_mode_function(sys_jcdma_m511_e_type new_mode)
/*lint -esym(715, new_mode) */
{
  
  return FALSE;
}
/*lint +esym(715, new_mode) */

/*===========================================================================
FUNCTION default_change_512_mode_function

DESCRIPTION

  Default handler for m512 mode change request.
  It is supposed to be replaced by DS with appropriate handler functions
  via a call to cm_m51_mode_reg()

DEPENDENCIES

RETURN VALUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean default_change_512_mode_function(sys_jcdma_m512_e_type new_mode)
/*lint -esym(715, new_mode) */
{
  
  return FALSE;
}
/*lint +esym(715, new_mode) */

/*===========================================================================
FUNCTION default_change_513_mode_function

DESCRIPTION

  Default handler for m513 mode change request.
  It is supposed to be replaced by DS with appropriate handler functions
  via a call to cm_m51_mode_reg()

DEPENDENCIES

RETURN VALUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean default_change_513_mode_function(sys_jcdma_m513_e_type new_mode)
/*lint -esym(715, new_mode)*/
{
  
  return FALSE;
}
/*lint +esym(715, new_mode)*/

/*===========================================================================
FUNCTION cm_m51_mode_init

DESCRIPTION
  Initialize M51 mode settings object.

  Use default handlers provided by CM for mode changes handlers.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_m51_mode_init( void )
{

  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize callback function pointers to default CM handlers
  */
  m51_mode_ptr->change_m511_mode_func = default_change_511_mode_function;
  m51_mode_ptr->change_m512_mode_func = default_change_512_mode_function;
  m51_mode_ptr->change_m513_mode_func = default_change_513_mode_function;

}

/*===========================================================================
FUNCTION cm_m51_mode_reg

DESCRIPTION
  Register or de-register with CM functions used to change M51 mode.

  To de-register, provide NULL pointer as argument.
  In this case CM will substitute a default handler for M51 mode change.

DEPENDENCIES

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_m51_mode_reg(

  cm_m511_mode_settings_f_type     *change_m511_mode_func,
      /* pointer to a DS function that changes M511 mode */

  cm_m512_mode_settings_f_type     *change_m512_mode_func,
      /* pointer to a DS function that changes M512 mode */

  cm_m513_mode_settings_f_type     *change_m513_mode_func
    /* pointer to a DS function that changes M513 mode */
)
/*lint -esym(715, change_m511_mode_func ) */
/*lint -esym(715, change_m512_mode_func ) */
/*lint -esym(715, change_m513_mode_func ) */
{
#ifdef FEATURE_JCDMA_1X
  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( m51_mode_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the functions that change M51 mode.
     Use defaults if NULL value provided.
  */
  if(change_m511_mode_func != NULL)
  {
    m51_mode_ptr->change_m511_mode_func = change_m511_mode_func;
  }
  else
  {
    m51_mode_ptr->change_m511_mode_func = default_change_511_mode_function;
  }

  if(change_m512_mode_func != NULL)
  {
    m51_mode_ptr->change_m512_mode_func = change_m512_mode_func;
  }
  else
  {
    m51_mode_ptr->change_m512_mode_func = default_change_512_mode_function;
  }

  if(change_m513_mode_func != NULL)
  {
    m51_mode_ptr->change_m513_mode_func = change_m513_mode_func;
  }
  else
  {
    m51_mode_ptr->change_m513_mode_func = default_change_513_mode_function;
  }

  return TRUE;
#else
  /* Feature FEATURE_JCDMA_1X is undefined so we return FALSE */
  return FALSE;
#endif
}
/*lint +esym(715, change_m511_mode_func ) */
/*lint +esym(715, change_m512_mode_func ) */
/*lint +esym(715, change_m513_mode_func ) */

/*===========================================================================

FUNCTION cm_change_m511_mode

DESCRIPTION
  Request DS to change M511 mode.

  This functions checks with M51 mode object and calls handler function
  that was registered for handling this mode change.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE depending on whether DS managed to change the mode

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_change_m511_mode(sys_jcdma_m511_e_type new_mode) {

  boolean result = FALSE;

  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  cm_m511_mode_settings_f_type *cb_func_ptr =
                                        m51_mode_ptr->change_m511_mode_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cb_func_ptr != NULL)
  {
    result = cb_func_ptr(new_mode);
  }

  return result;
}

/*===========================================================================

FUNCTION cm_change_m512_mode

DESCRIPTION
  Request DS to change M512 mode.

  This functions checks with M51 mode object and calls handler function
  that was registered for handling this mode change.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE depending on whether DS managed to change the mode

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_change_m512_mode(sys_jcdma_m512_e_type new_mode) {

  boolean result = FALSE;
  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  cm_m512_mode_settings_f_type *cb_func_ptr =
                                        m51_mode_ptr->change_m512_mode_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cb_func_ptr != NULL)
  {
    result = cb_func_ptr(new_mode);
  }

  return result;
}

/*===========================================================================

FUNCTION cm_change_m513_mode

DESCRIPTION
  Request DS to change M513 mode.

  This functions checks with M51 mode object and calls handler function
  that was registered for handling this mode change.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE depending on whether DS managed to change the mode

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_change_m513_mode(sys_jcdma_m513_e_type new_mode) {

  boolean result = FALSE;
  cm_m51_mode_settings_s_type    *m51_mode_ptr = CM_M51_MODE_PTR();
      /* point at M51 mode settings */

  cm_m513_mode_settings_f_type *cb_func_ptr =
                                        m51_mode_ptr->change_m513_mode_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cb_func_ptr != NULL)
  {
    result = cb_func_ptr(new_mode);
  }

  return result;
}


/*===========================================================================
=============================================================================
============================ UI INTERFACE BLOCK =============================
=============================================================================
===========================================================================*/


/* This interface block is needed until the new MC interface
** is in place, where CM becomes its own task and MC commands
** are sent directly to CM.
*/

/*===========================================================================

FUNCTION cm_get_current_acq_sys_mode

DESCRIPTION
  This function returns the current system mode that is acquired.

DEPENDENCIES
  none

RETURN VALUE
  system mode.

SIDE EFFECTS
  none

===========================================================================*/
sys_sys_mode_e_type cm_get_current_acq_sys_mode( void )
{
  return sd_misc_get_curr_acq_sys_mode();
}

/*===========================================================================

FUNCTION cm_get_services_count

DESCRIPTION
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
int cm_get_services_count ( void )
{
  return (int) CM_SS_MAX;
}

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
)
{
  if(srv_count < (int)CM_SS_MAX || srv_list == NULL )
  {
    CM_MSG_HIGH_0("CM: Not able to retrieve service status");
    return FALSE;
  }


  cmss_get_service_status_per_stack(CM_SS_MAIN, &srv_list[(int)CM_SS_MAIN]);

  cmss_get_service_status_per_stack(CM_SS_HYBR_1, &srv_list[(int)CM_SS_HYBR_1]);

  cmss_get_service_status_per_stack(CM_SS_HYBR_2, &srv_list[(int)CM_SS_HYBR_2]);

  cmss_get_service_status_per_stack(CM_SS_HYBR_3, &srv_list[(int)CM_SS_HYBR_3]);

  return TRUE;
}

/*===========================================================================

FUNCTION cm_get_cur_nam

DESCRIPTION
  Returns the current NAM.

DEPENDENCIES
  Phone object must be initialized.

RETURNS
  The current NAM. If phone was not yet initialized, return CM_NAM_NONE.

SIDE EFFECTS
  None

===========================================================================*/
cm_nam_e_type  cm_get_curr_nam( void )
{
  return cmph_get_curr_nam();
}



/*===========================================================================

FUNCTION cm_num_fill

DESCRIPTION
  Initialize a CM number object and fill it with a number.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_fill(

    cm_num_s_type    *num_ptr,
        /* Pointer to a CM number to be initialized */

    const byte       *num_buf,
        /* Dialed address buffer.
        **
        ** NOTE! In the case where the digit_mode parameter is set to
        ** CM_DIGIT_MODE_4BIT_DTMF the number buffer may only contain
        ** 0-9, "#", or "*" characters.
        **
        ** In the case where the digit_mode parameter is set to
        ** CM_DIGIT_MODE_8BIT_ASCII the number buffer may contain any
        ** 8-bit ASCII characters */

    byte                   num_len,
        /* dialed address length */

    cm_digit_mode_e_type   digit_mode
        /* digit mode - see J-STD-008 2.7.1.3.2.4 */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr != NULL );
  CM_ASSERT( num_len <= ARR_SIZE(num_ptr->buf));
  CM_ASSERT( BETWEEN(digit_mode,CM_DIGIT_MODE_NONE,CM_DIGIT_MODE_MAX ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -e{774} always false only for when CM_DEBUG defined */
  if(num_ptr == NULL)
  {
    CM_ERR_0("num_ptr is NULL");
    return;
  }

  cm_num_init(num_ptr);

  if(num_len <= ARR_SIZE(num_ptr->buf))
  {
    /*lint -e{670} num_len is always less than num_buf */
    num_ptr->len = memscpy( num_ptr->buf, sizeof(num_ptr->buf), num_buf, num_len );
  }
  else
  {
    CM_ERR_1("Bad num_len = %d",num_len);
    num_ptr->len = 0;
  }

  num_ptr->digit_mode = digit_mode;
} /* cm_num_fill */



/*===========================================================================

FUNCTION cm_num_get

DESCRIPTION
  Copy a CM number into a specified byte buffer.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  The numbers digits actually copied.

SIDE EFFECTS
  none

===========================================================================*/
byte cm_num_get(

    const cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number to copy from */

    byte*                  num_buf,
        /* Pointer to a byte buffer to copy CM number into */

    byte                   max_len
        /* Size of byte buffer */

)
{

  byte cpy_len = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || max_len == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If both pointers are not NULL, copy the number over.
  ** Make sure we do NOT copy more that the size of the buffer.
  */
  if( cm_num_ptr != NULL && num_buf != NULL ) /*lint !e774 */
  {
    cpy_len = memscpy( num_buf, max_len, cm_num_ptr->buf, cm_num_ptr->len );
  }

  return cpy_len;

} /* cm_num_get */


/*===========================================================================

FUNCTION cm_num_cmp

DESCRIPTION
  Compare a CM number byte with a byte buffer number over a specified
  length.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  TRUE if both numbers match over the specified compare length,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_num_cmp(

    const cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*             num_buf,
        /* Pointer to a byte buffer number */

    unsigned int           cmp_len
        /* Length to perform comparison over */
)
{

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || cmp_len == 0 );
  CM_ASSERT( cmp_len <= ARR_SIZE(cm_num_ptr->buf) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If both pointers are not NULL, compare the numbers.
  */
  if( cm_num_ptr != NULL && num_buf != NULL ) /*lint !e774 */
  {
    /* Only compare if CM number is long enough.
    */
    if( cmp_len <= cm_num_ptr->len )
    {
      if( memcmp(cm_num_ptr->buf, num_buf, cmp_len) == 0 )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
} /* cm_num_cmp */



/*===========================================================================

FUNCTION cm_num_cmp_exact

DESCRIPTION
  Compare a CM number byte with a byte buffer for an exact match -
  i.e. length and pattern.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  TRUE if both numbers match exactly, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_num_cmp_exact(

    const cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*            num_buf,
        /* Pointer to a byte buffer number */

    unsigned int           num_len
        /* Length off byte buffer number */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || num_len == 0 );
  CM_ASSERT( num_len <= ARR_SIZE(cm_num_ptr->buf) );

  /* CM_ASSERT will not be turned on target */
  if( num_len > ARR_SIZE(cm_num_ptr->buf) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return CM_BOOLEAN( (num_len == cm_num_ptr->len) &&
                      cm_num_cmp(cm_num_ptr, num_buf, num_len) );
} /* cm_num_cmp_exact */



/*===========================================================================

FUNCTION cm_num_copy

DESCRIPTION
  Copy a byte buffer number into a specified CM number.

DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_num_copy
(

    cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*      num_buf,
        /* Pointer to a number to be copied into CM number */

    byte             num_len
        /* Length of number to be copied */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -save -e774 */
  if((num_buf != NULL) && (num_len > 0))
  /*lint -restore */
  {

    cm_num_init( cm_num_ptr );

    /* Make sure we do NOT copy more that the max length.
    */
    cm_num_ptr->len = memscpy( cm_num_ptr->buf, sizeof(cm_num_ptr->buf), num_buf, num_len );
  }
  else
  {
    /* clear num buffer */
    cm_num_init( cm_num_ptr );
    CM_MSG_HIGH_0("Bad params to cm_num_copy()");
  }
} /* cm_num_copy */



/*===========================================================================

FUNCTION cm_num_ep_msg_copy

DESCRIPTION
  Copy an IS91 Extended Protocol Short Message into CM number.

DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_ep_msg_copy(

    cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*       num_buf,
        /* Pointer to a number to be copied into CM number */

    byte             num_len
        /* Length of number to be copied */
)
{

  unsigned int i, j;
      /* loop counters */

  unsigned int max_len = ARR_SIZE( cm_num_ptr->buf );
      /* In any case do not copy more than the CM buffer size */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || num_len == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #define NAMPS_STX '\\'   /* Start-of-TX character */
  #define NAMPS_ETX 3      /* End-of_TX character */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( num_len > max_len )
  {
    CM_MSG_HIGH_0( "cm_num_ep_msg_copy - num_len > max_len");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM number buffer.
  */
  cm_num_init( cm_num_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the IS91 Extended Protocol Short Message into
  ** the CM number as follows:
  ** 1. Strip any Start-of-TX characters.
  ** 2. Stop copying on End-of_TX character.
  ** 3. Make sure we do NOT copy more than the CM number buffer size.
  */
  for( i=0, j=0; i < MIN( num_len, max_len); i++ )
  {
    /* Stop if end of End-of_TX character.
    */
    if( num_buf[i] == NAMPS_ETX )
    {
      break;
    }

    /* Copy over if not Start-of-TX character.
    */
    if( num_buf[i] != NAMPS_STX )
    {
      cm_num_ptr->buf[j++] = num_buf[i];
    }
  }

  /* Set the CM number length appropriately.
  */
  cm_num_ptr->len = (byte) j;

} /* cm_num_ep_msg_copy */



/*===========================================================================

FUNCTION cm_num_ep_cli_copy

DESCRIPTION
  Copy IS91 Extended Protocol Caller Line ID into a CM number.

DEPENDENCIES
  Specified CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_ep_cli_copy(

    cm_num_s_type    *cm_num_ptr,
        /* Pointer to a CM number */

    const byte*      num_buf,
        /* Pointer to a number to be copied into CM number */

    byte             num_len
        /* Length of number to be copied */
)
{

  unsigned int i;
      /* loop counter */

  unsigned int max_len = ARR_SIZE( cm_num_ptr->buf );
      /* In any case do not copy more than the CM buffer size */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_num_ptr != NULL );
  CM_ASSERT( num_buf != NULL || num_len == 0 );
  CM_ASSERT( num_len <= max_len );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM number buffer.
  */
  cm_num_init( cm_num_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the IS91 Extended Protocol Short Message into
  ** the CM number as follows:
  ** 1. Replace none printable characters with space.
  ** 2. Stop copying on '\0' character.
  ** 3. Make sure we do NOT copy more than the CM number buffer size.
  */
  for( i=0; i < MIN( num_len, max_len); i++ )

  {
    /* Stop if end of message.
    */
    if( num_buf[i] == '\0' )
    {
      break;
    }

    /* If this is a non-printable control character,
    ** replace it with space, else, just copy the character.
    */
    if( BETWEEN(num_buf[i], 0, 0x20))
    {
      cm_num_ptr->buf[i] = ' ';
    }
    else
    {
      cm_num_ptr->buf[i] = num_buf[i];
    }
  }

  /* Set the CM number length appropriately.
  */
  cm_num_ptr->len = (byte) i;
} /* cm_num_ep_cli_copy */

#if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/*===========================================================================

FUNCTION cm_num_attrib_copy

DESCRIPTION
  Set the attributes of a CM number to a specified set of attributes.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_attrib_copy(

    cm_num_s_type           *num_ptr,
        /* Pointer to a CM number */

    byte                    number_type,
        /* Number type - see J-STD-008 2.7.1.3.2.4-2
        ** NOTE! ONLY VALID FOR CDMA CALLS */

    byte                    number_plan,
        /* Numbering Plan - see J-STD-008 2.7.1.3.2.4-3
        ** NOTE! ONLY VALID FOR CDMA CALLS */

    byte                    pi,
        /* Presentation Indicator - see J-STD-008  2.7.4.4-1 */

    byte                    si,
        /* Screening Indicator - see J-STD-008 2.7.4.4-2 */

    cm_digit_mode_e_type    digit_mode
        /* Origination digit mode - see J-STD-008 2.7.1.3.2.4 */
)
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr != NULL );

  CM_ASSERT( number_type <= CAI_ABB_NUM );

  CM_ASSERT( number_plan <= CAI_PRIV_PLAN );

  CM_ASSERT( pi <= CAI_NOT_AVAIL_PI );

  CM_ASSERT( si <= CAI_NET_SI );

  CM_ASSERT( digit_mode <= (CM_DIGIT_MODE_MAX-1) ); /*lint !e641 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  num_ptr->number_type   = number_type;
  num_ptr->number_plan   = number_plan;
  num_ptr->pi            = pi;
  num_ptr->si            = si;
  num_ptr->digit_mode    = digit_mode;

  #else
#error code not present
#endif
} /* cm_num_attrib_copy */
#endif // #if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))



/*===========================================================================

FUNCTION cm_num_validate

DESCRIPTION
  This function validates a number.

DEPENDENCIES
  The number must have already been initialized with cm_num_init().

RETURN VALUE
  CM_NUM_ERR_NOERR is returned if the number is valid.  Otherwise, an
  appropriate error code is returned.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_num_err_e_type cm_num_validate(

    const cm_num_s_type    *num_ptr
        /* Pointer to a CM number to be asserted */
)
{
  cm_num_err_e_type return_code = CM_NUM_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -save -e774 */
  if ( num_ptr == NULL )
  /*lint -restore */
  {
    return_code = CM_NUM_ERR_NULL_PTR;
  }
  else
  if ( num_ptr->len > ARR_SIZE(num_ptr->buf) )
  {
    return_code = CM_NUM_ERR_INVALID_NUM_LENGTH;
  }

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  #if ( !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA) && \
        !defined(FEATURE_TDSCDMA) && !defined(FEATURE_MMODE_REMOVE_1X ))
  else
  if ( num_ptr->number_type > (uint8)CM_NUM_TYPE_MAX )
  {
    return_code = CM_NUM_ERR_INVALID_NUM_TYPE;
  }
  else
  if ( num_ptr->number_plan > (uint8)CM_NUM_PLAN_MAX )
  {
    return_code = CM_NUM_ERR_INVALID_NUM_PLAN;
  }
  else
  if ( num_ptr->pi > CAI_NOT_AVAIL_PI )
  {
    return_code = CM_NUM_ERR_INVALID_PI;
  }
  else
  if ( num_ptr->si > CAI_NET_SI )
  {
    return_code = CM_NUM_ERR_INVALID_SI;
  }
  #endif /* ( !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA) && !defined(FEATURE_TDSCDMA) ) */
  #endif /* FEATURE_MMODE_CDMA_800) || FEATURE_MMODE_CDMA_1900 */

  return return_code;
} /* cm_num_validate() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))


/*===========================================================================

FUNCTION cm_num_assert

DESCRIPTION
  Check a series of assertions over a CM number to validate its state.

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_num_assert(

    const cm_num_s_type    *num_ptr
        /* Pointer to a CM number to be asserted */
)
/*lint -esym(715, num_ptr) */
{
  #ifndef FEATURE_MMODE_REMOVE_1X

  CM_ASSERT( num_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( num_ptr->len <= ARR_SIZE(num_ptr->buf) );

  CM_ASSERT( num_ptr->number_type <= CAI_ABB_NUM );

  CM_ASSERT( num_ptr->number_plan <= CAI_PRIV_PLAN );

  CM_ASSERT( num_ptr->pi <= CAI_NOT_AVAIL_PI );

  CM_ASSERT( num_ptr->si <= CAI_NET_SI );
  #else
  #error code not present
#endif
}
#endif // #if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/*===========================================================================

FUNCTION cm_is_sys_mode_allowed

DESCRIPTION

  The function determines if the given sys_mode is included / conforms to
  given band and mode pref.

   For example, if the system mode is  SYS_SYS_MODE_GSM and  mode band
  preference is CM_MODE_PREF_GSM_WCDMA_ONLY / CM_BAND_PREF_GSM_850  the
  function returns TRUE but given SYS_SYS_MODE_GW it returns FALSE because
  CM_BAND_PREF_GSM_850 does not have WCDMA component.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If Valid
  FALSE - Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_sys_mode_allowed(

    cm_mode_pref_e_type   mode_pref,
      /* The mode preference requested
      */

    cm_band_pref_e_type   band_pref,
      /* The band preference requested
      */

    sys_sys_mode_e_type   sys_mode
      /* The sys mode requested
      */
 )
{
  sd_ss_mode_pref_e_type sd_mode_pref  = SD_SS_MODE_PREF_NONE;

  
  CM_ASSERT( BETWEEN(mode_pref, CM_MODE_PREF_NONE, CM_MODE_PREF_MAX ));
  CM_ASSERT( BETWEEN(band_pref, CM_BAND_PREF_NONE, CM_BAND_PREF_MAX ));
  CM_ASSERT( BETWEEN(sys_mode , SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ----- - - - - -*/

  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref);

  if( sd_mode_pref == SD_SS_MODE_PREF_NONE )
  {
    CM_ERR_1("Mapping failed for %d! mode pref!", mode_pref );
    return FALSE;
  }
  if(!(sd_misc_is_mode_pref((sd_ss_mode_pref_e_type)BM(sys_mode),sd_mode_pref)))
  {
    return FALSE;
  }

  return(sd_misc_is_supp_pref((sd_ss_mode_pref_e_type)BM(sys_mode),
         cmph_map_cm_band_pref_to_sd_band_pref(band_pref)));

}/* End function cm_is_sys_mode_allowed
 */

/*===========================================================================
FUNCTION cm_remove_mode_pref_components

DESCRIPTION

  Remove given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will remove all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_remove_mode_pref_components(
                                           CM_MODE_PREF_AUTOMATIC,
                                           6,// because 6 components follow
                                           SD_SS_MODE_PREF_WCDMA,
                                           SD_SS_MODE_PREF_GSM,
                                           SD_SS_MODE_PREF_GPS,
                                           SD_SS_MODE_PREF_HDR,
                                           SD_SS_MODE_PREF_CDMA,
                                           SD_SS_MODE_PREF_AMPS );

DEPENDENCIES
  sd_misc_remove_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cm_remove_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                        int number_of_components_to_remove,
                                                        ... )
{

  va_list arg_list;
  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_ss_mode_pref_e_type sd_mode_pref_to_remove = SD_SS_MODE_PREF_NONE;
  cm_mode_pref_e_type old_cm_mode_pref = cm_mode_pref;

  /*
  ** First map given CM mode pref to SD mode pref
  */
  if(cm_mode_pref == CM_MODE_PREF_WLAN_ONLY)
  {
    return CM_MODE_PREF_WLAN_ONLY;
  }
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);
  if( sd_mode_pref == SD_SS_MODE_PREF_NONE) {
    CM_MSG_HIGH_1( "Mapping #1 failed - mode_pref=%d!!!", cm_mode_pref);
    return CM_MODE_PREF_NONE;
  }

  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{64} -e{718} -e{746} -e{530} -e{516} */
  va_start(arg_list, number_of_components_to_remove);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be removed from given
  ** CM mode pref
  */

  while(number_of_components_to_remove--)
  {

    /* Point at next function argument */
    /*lint -e{662} -e{826} -e{10} -e{718} -e{746} -e{718} -e{124}*/
    sd_mode_pref_to_remove = (sd_ss_mode_pref_e_type) va_arg(arg_list, int);

    if(!BETWEEN( sd_mode_pref_to_remove,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR_1( "Bad sd_mode_pref_to_remove = %d",sd_mode_pref_to_remove);
      CM_ASSERT(FALSE);
    }

    /* Remove it */
    sd_mode_pref = sd_misc_remove_mode_pref( sd_mode_pref,
                                             sd_mode_pref_to_remove );
  }

  /* Clean up */
  va_end(arg_list);

  /* Now map result back to CM mode pref enum value */
  cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref);
  if(cm_mode_pref == CM_MODE_PREF_MAX) {
    CM_ERR_1( "Mapping #2 failed - mode_pref=%d !!!", cm_mode_pref);
    return old_cm_mode_pref;
  }

  return cm_mode_pref;
}

/*===========================================================================
FUNCTION cm_add_mode_pref_components

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
cm_mode_pref_e_type cm_add_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                int number_of_components_to_add,
                                                ... )
{

  va_list arg_list;
  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_ss_mode_pref_e_type sd_mode_pref_to_add = SD_SS_MODE_PREF_NONE;

  /*
  ** First map given CM mode pref to SD mode pref
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);
  if( sd_mode_pref == SD_SS_MODE_PREF_NONE)
  {
    CM_MSG_HIGH_1("adding modes to target mode_pref %d",cm_mode_pref);
  }

  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{64} -e{530} -e{516} */
  va_start(arg_list, number_of_components_to_add);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be added to given
  ** CM mode pref
  */

  while(number_of_components_to_add--)
  {

    /* Point at next function argument */
    /*lint -e{10} -e{662} -e{826} -e{124}*/
    sd_mode_pref_to_add = (sd_ss_mode_pref_e_type) va_arg(arg_list, int);

    if(!BETWEEN( sd_mode_pref_to_add,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR_1( "Bad sd_mode_pref_to_add = %d",sd_mode_pref_to_add);
      CM_ASSERT(FALSE);
    }

    /* Remove it */
    sd_mode_pref = sd_misc_add_mode_pref( sd_mode_pref,
                                          sd_mode_pref_to_add );
  }

  /* Clean up */
  va_end(arg_list);

  /* If sd_mode_pref has gwl bit set, but not gsm and wcdma, then we need to remove gwl bit.
  */
  if (!(sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_GSM )  &&
      sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_WCDMA )) &&
      sd_misc_is_mode_pref(sd_mode_pref,
                    (sd_ss_mode_pref_e_type)(BM( SYS_SYS_MODE_GWL) ) )
     )
  {
    sd_mode_pref = sd_misc_remove_mode_pref(sd_mode_pref,
                    (sd_ss_mode_pref_e_type)(BM( SYS_SYS_MODE_GWL)));
  }

  /* Now map result back to CM mode pref enum value */
  cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref);
  if(cm_mode_pref == CM_MODE_PREF_MAX) {
    CM_ERR_0( "Mapping #2 failed !!!");
  }

  return cm_mode_pref;

}

/*===========================================================================

FUNCTION cm_ph_net_list_copy_per_subs

DESCRIPTION
  This function copies networks lists from PH event info to the client when
  requested bv client.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void cm_ph_net_list_copy_per_subs
(
  cm_ph_net_lists_info_s_type   *info_ptr,

  sys_modem_as_id_e_type        asubs_id
)
{
  cmph_s_type                *ph_ptr  = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( info_ptr != NULL);
  CM_ASSERT( ph_ptr   != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(info_ptr == NULL)  /*lint !e774 */
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize lists.
  */
  info_ptr->gw_available_networks.length = 0;
  info_ptr->gw_user_pref_networks.length = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* More PH info is copied only when posting CMPH_EVENT.
  ** If a client attempts to access more PH info in any other
  ** circumtances, then no information will be copied.
  */
  if(! ph_ptr->is_event_process)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Network lists PH info is copied only when CM indicates that networks
  ** lists info is available.
  */
  if(! ph_ptr->is_net_lists_present)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef CM_GW_SUPPORTED
  if( asubs_id == ph_ptr->hybr_2_stack_info.asubs_id
      && asubs_id != SYS_MODEM_AS_ID_NONE)
  {
    /* Copy detailed information now.
    */
    if(ph_ptr->hybr_2_stack_info.plmn_list.plmn_list.length > 0   )
    {
      info_ptr->gw_available_networks =
                                ph_ptr->hybr_2_stack_info.plmn_list.plmn_list;
    }


    info_ptr->pci_plmn_list =
                                ph_ptr->hybr_2_stack_info.plmn_list.pci_plmn_list;
    
     
    if (ph_ptr->hybr_2_stack_info.user_pref_networks.length > 0 )
    {
      info_ptr->gw_user_pref_networks =
                                ph_ptr->hybr_2_stack_info.user_pref_networks;
    }
  }
  else
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( asubs_id == ph_ptr->hybr_3_stack_info.asubs_id
    )
  {
    /* Copy detailed information now.
    */

     if(ph_ptr->hybr_3_stack_info.plmn_list.plmn_list.length > 0   )
    {
      info_ptr->gw_available_networks =
                                ph_ptr->hybr_3_stack_info.plmn_list.plmn_list;
    }

 
    info_ptr->pci_plmn_list = ph_ptr->hybr_3_stack_info.plmn_list.pci_plmn_list;
                                
  
    if (ph_ptr->hybr_3_stack_info.user_pref_networks.length > 0 )
    {
      info_ptr->gw_user_pref_networks =
                                ph_ptr->hybr_3_stack_info.user_pref_networks;
    }
  }
  else
  #endif
  {
    /* Copy detailed information now.
    */
    if(ph_ptr->main_stack_info.plmn_list.plmn_list.length > 0   )
    {
      info_ptr->gw_available_networks =
                                ph_ptr->main_stack_info.plmn_list.plmn_list;
    }


    info_ptr->pci_plmn_list = ph_ptr->main_stack_info.plmn_list.pci_plmn_list;
                                
    
    if (ph_ptr->main_stack_info.user_pref_networks.length > 0 )
    {
      info_ptr->gw_user_pref_networks = ph_ptr->main_stack_info.user_pref_networks;
    }
  }
  #endif


  SYS_ARG_NOT_USED(asubs_id);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /* cm_ph_net_list_copy_per_subs() */


/*===========================================================================

FUNCTION cm_ph_net_list_copy

DESCRIPTION
  This function copies networks lists from PH event info to the client when
  requested bv client.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void cm_ph_net_list_copy
(
  cm_ph_net_lists_info_s_type   *info_ptr
)
{
  cm_ph_net_list_copy_per_subs(info_ptr,
                               SYS_MODEM_AS_ID_1
                              );
}


/*===========================================================================

FUNCTION cm_ph_get_gw_subscription_source

DESCRIPTION
  Wrapper function for cmph_get_gw_subscription_source.
  Used to call by non-multimode modules of Modem.

DEPENDENCIES
  None.

RETURN VALUE
  This function will return where gw is currently getting it's
  subscription or none if there  is no subscription available.

SIDE EFFECTS
  None.

===========================================================================*/
cm_cc_subscription_source_e_type cm_ph_get_gw_subscription_source
(
  void
)
{
  return cmph_get_gw_subscription_source(CM_SS_MAIN);
}

/*===========================================================================

FUNCTION cm_ph_get_cdma_subscription_source

DESCRIPTION
  Wrapper function for cmph_get_cdma_subscription_source.
  Used to call by non-multimode modules of Modem.

DEPENDENCIES
  None.

RETURN VALUE
  This function will return where CDMA is currently getting it's
  subscription or none if there is no subscription available.

SIDE EFFECTS
  None.

===========================================================================*/
cm_cc_subscription_source_e_type cm_ph_get_cdma_subscription_source
(
  void
)
{
  return cmph_get_cdma_subscription_source();
}
/*===========================================================================

FUNCTION cm_mbms_cmd_generic

DESCRIPTION
  API to be used by MBMS Apps to send commands to MBMS Service Manager.
  Clients should fill in the appropriate MBMS request type and also
  provide the corresponding parameters in the mbms_generic_req struct.
  Clients should check for CM_API_MBMS before using this function

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE   - Cmd has been queued to CM
  FALSE  - Cmd did not get queued to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_mbms_cmd_generic (

  cm_mbms_cmd_cb_f_type                cmd_cb_func,
    /* client callback function */

  void                                 *data_block_ptr,
    /* pointer to client callback data block */

  const cm_client_id_type              client_id,
    /* originating client */

  const cm_mbms_generic_cmd_s_type    *mbms_generic_cmd
    /* Structure to hold MBMS commands and their parameters */

)
/*lint -esym(715,cmd_cb_func)*/
/*lint -esym(715,data_block_ptr)*/
/*lint -esym(715,client_id)*/
/*lint -esym(715,mbms_generic_cmd)*/
/*lint -esym(818, data_block_ptr) */

{
  #if defined (FEATURE_MBMS)
#error code not present
#else
  CM_ERR_0( "FEATURE_MBMS is not defined ");
  return FALSE;
  #endif /* #if defined (FEATURE_MBMS) */
}
/*lint +esym(715,cmd_cb_func)*/
/*lint +esym(715,data_block_ptr)*/
/*lint +esym(715,client_id)*/
/*lint +esym(715,mbms_generic_cmd)*/
/*lint +esym(818, data_block_ptr) */
/*===========================================================================

FUNCTION cm_mm_call_cmd_data_suspend

DESCRIPTION
  This function posts cmd to suspend data on requested subs

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_mm_call_cmd_data_suspend
(
    cm_call_cmd_cb_f_type  cmd_cb_func,
      /**< Client callback function. */

    void                   *data_block_ptr,
      /**< Pointer to the client callback data block. */

    cm_client_id_type              client_id,
    /* requesting client ID */

    boolean                data_suspend,
      /**< indicates if data needs to be suspended or resumed*/

    sys_modem_as_id_e_type asubs_id
      /**< Active subscription ID. */

)
{
  cm_call_cmd_s_type           *call_cmd_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("CM_CALL_CMD_DATA_SUSPEND with data_suspend=%d, asubs_id=%d",
                data_suspend, asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func         = cmd_cb_func;
    call_cmd_ptr->data_block_ptr      = data_block_ptr;
    call_cmd_ptr->client_id           = client_id;
    call_cmd_ptr->cmd                 = CM_CALL_CMD_DATA_SUSPEND;
    call_cmd_ptr->cmd_subs            = asubs_id;
    call_cmd_ptr->info.data_suspend   = data_suspend;

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );


  return TRUE;
}


/*===========================================================================

FUNCTION cm_mm_call_cmd_inform_secure_context_info

DESCRIPTION
  This function posts CM_CALL_CMD_INFORM_SECURE_CONTEXT_INFO
  to inform secure context info for an ongoing call

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_mm_call_cmd_inform_secure_context_info(

  cm_call_cmd_cb_f_type         cmd_cb_func,
    /* client callback function */

  void                          *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type             client_id,
    /* commanding client's id */

  cm_call_id_type               call_id,
    /* call ID of the call */
  cm_call_secure_context_e_type secure_context_info
    /* Secure Context info  */
)
{
#if defined(FEATURE_IP_CALL)
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("cm_mm_call_cmd_inform_secure_context_info call_id %d, secure_context_info %d",
                call_id, secure_context_info);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func         = cmd_cb_func;
    call_cmd_ptr->data_block_ptr      = data_block_ptr;
    call_cmd_ptr->client_id           = client_id;
    call_cmd_ptr->cmd                 = CM_CALL_CMD_INFORM_SECURE_CONTEXT_INFO;

    cmd_info_ptr       = CMD_INFO_PTR( call_cmd_ptr );
    cmd_info_ptr->call_id                       = call_id;
    cmd_info_ptr->secure_context_info  = secure_context_info;

    /* Queue the commands to CM.
    */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
  return TRUE;

#else
  return FALSE;
#endif // FEATURE_IP_CALL
}

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
void cm_unlock_hdr_resources(void)
{

  #if defined(FEATURE_FULL_TIME_SHDR_3) && defined(FEATURE_HDR_HYBRID)

  cm_send_hdr_unlock_rf_cmd();

  #else

  CM_MSG_HIGH_0("FEATURE_FULL_TIME_SHDR_3 & FEATURE_HDR_HYBRID is undefined");

  #endif

}

/*===========================================================================
@FUNCTION: cm_get_timing_advance

@DESCRIPTION
  To get timing_advance information client has to allocate
  memory before calling this API

@DEPENDENCIES
  None.

@RETURN VALUE
   TRUE :  Success
   FALSE:  Failure

@SIDE EFFECTS
  None
===========================================================================*/
boolean cm_get_timing_advance(

  uint32 *timing_advance_ptr
    /* Timing_advance information */
)
{
  SYS_ARG_NOT_CONST(timing_advance_ptr);

  #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
  /* Sanity check on timing_advance */
  if ( timing_advance_ptr == NULL)
  {
    CM_ERR_0 ("timing_advance_ptr is NULL");
    return FALSE;
  }
  CM_MSG_HIGH_0 ( "CM returning timing advance information to client" );
  geran_eng_mode_data_read(ENG_MODE_TIMING_ADVANCE , timing_advance_ptr );
  return TRUE;
  #else
  SYS_ARG_NOT_USED(timing_advance_ptr);
  CM_MSG_HIGH_0("FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE is undefined");
  return FALSE;
  #endif  /* FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */
}

/*===========================================================================
@FUNCTION: cm_get_cdma_position_info

@DESCRIPTION
  To get UE current CDMA Cell position information
  Client has to allocate memory before calling this API
@DEPENDENCIES
  None.

@RETURN VALUE
   TRUE : Success
   FALSE: Failure

@SIDE EFFECTS
  None
===========================================================================*/
boolean cm_get_cdma_position_info(

  cm_cdma_position_info_s_type   *position_info_ptr
   /* pointer to cdma position info sent to client */
)
{
  byte     base_class=0;       /* Base Station Class */
  byte     packet_zone_id=0;   /* Packet Zone Identification */
  uint16   mcc=0;              /* Mobile Country Code */
  SYS_ARG_NOT_CONST(position_info_ptr);

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

  /* Sanity check on position_info_ptr */
  if ( position_info_ptr == NULL)
  {
    CM_ERR_0 ("position_info_ptr is NULL");
    return FALSE;
  }
  CM_MSG_HIGH_0 ( "CM returning CDMA postion information to client" );
  mccdma_get_cur_bs_info( &position_info_ptr->sid,
                          &position_info_ptr->nid,
                          &position_info_ptr->base_id,
                          &base_class,
                          &position_info_ptr->refpn,
                          &packet_zone_id,
                          &mcc,
                          &position_info_ptr->base_lat,
                          &position_info_ptr->base_long);
  return TRUE;
  #else
  SYS_ARG_NOT_USED(position_info_ptr);
  SYS_ARG_NOT_USED(base_class);
  SYS_ARG_NOT_USED(packet_zone_id);
  SYS_ARG_NOT_USED(mcc);
  CM_MSG_HIGH_0("FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 is undefined");
  return FALSE;
  #endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */
}

/*===========================================================================

@FUNCTION cm_is_multimode_subs

@DESCRIPTION
   Returns if requested subs is multimode in case of DSDA

@DEPENDENCIES
  None

@RETURN VALUE
  Returns if requested subs is multimode

@SIDE EFFECTS
  None

===========================================================================*/

boolean cm_is_multimode_subs (

  sys_modem_as_id_e_type asubs_id
)
{
  if (cmph_is_lte_capable_on_sub(asubs_id))
  {
    return TRUE;
  }
  else if (cmph_is_lte_capable_on_any_sub())
  {
    return FALSE;
  }
  else if (cmph_is_gsm_only_sub(asubs_id))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*===========================================================================

@FUNCTION cm_get_device_mode

@DESCRIPTION
   Returns device mode of operation

@DEPENDENCIES
  None

@RETURN VALUE
  Returns if requested subs is multimode

@SIDE EFFECTS
  None

===========================================================================*/

sys_modem_device_mode_e_type cm_get_device_mode (

  void
)
{
  return cmpmprx_get_device_mode();
}

/*===========================================================================

@FUNCTION cm_mm_call_cmd_local_hold

@DESCRIPTION
   Queues command to put all calls on a subs to local hold

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

@SIDE EFFECTS
  None

===========================================================================*/

boolean cm_mm_call_cmd_local_hold (

  cm_call_cmd_cb_f_type  cmd_cb_func,
    /* client callback function */

  void                   *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type      client_id,
    /* answering client */

  sys_modem_as_id_e_type asubs_id,
    /* call ID of the call */

  const cm_call_hold_state_e_type  call_hold_state
    /* Call hold state for the call */
)
{
  if (cmph_is_dsda())
  {
    cm_call_cmd_s_type           *call_cmd_ptr;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_HIGH_2( "CM_CALL_CMD_LOCAL_HOLD with call_hold_state as %d subs_id %d",
                   call_hold_state, asubs_id );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    call_cmd_ptr = cm_cmd_alloc_call_init();

    /* Copy command header fields. */

    call_cmd_ptr->cmd_cb_func           = cmd_cb_func;
    call_cmd_ptr->data_block_ptr        = data_block_ptr;
    call_cmd_ptr->client_id             = client_id;
    call_cmd_ptr->cmd                   = CM_CALL_CMD_LOCAL_HOLD;
    call_cmd_ptr->cmd_subs              = asubs_id;
    call_cmd_ptr->info.call_hold_state  = call_hold_state;

    /* Queue the commands to CM.
      */
    cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );

    return TRUE;
  }
  else
  {
    if( cmd_cb_func != NULL )
    {
      cmd_cb_func( data_block_ptr, CM_CALL_CMD_LOCAL_HOLD,
        CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE);
    }
    SYS_ARG_NOT_USED(asubs_id);
    SYS_ARG_NOT_USED(call_hold_state);
    SYS_ARG_NOT_USED(client_id);
    CM_ERR_0( "Invalid cmd for non dsda");
    return FALSE;
  }
}

/*===========================================================================

@FUNCTION cm_get_operator_info

@DESCRIPTION
   Queries Operator Type/Name

@DEPENDENCIES
  None

@RETURN VALUE
  cm_operator_e_type - Operator Name

@SIDE EFFECTS
  None

===========================================================================*/
cm_operator_e_type cm_get_operator_info
(
   sys_modem_as_id_e_type asubs_id
)
{
  cmph_s_type                  *ph_ptr  = cmph_ptr ();
  cmph_sub_config_s_type       *ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",asubs_id);
    return OPERATOR_NULL;
  }

  if( ( asubs_id > SYS_MODEM_AS_ID_NONE) && (asubs_id < SYS_MODEM_AS_ID_NO_CHANGE) )
  {
    /* Generic - The Subscriptions are added between SYS_MODEM_AS_ID_NONE(-1)
       and SYS_MODEM_AS_ID_NO_CHANGE
    */
    #if defined(FEATURE_MMODE_TRIPLE_SIM)
    if(asubs_id == SYS_MODEM_AS_ID_3)
    {
      return ph_sub_config_ptr->operator_name;
    }
    #endif
    if( (asubs_id == SYS_MODEM_AS_ID_1) || (asubs_id == SYS_MODEM_AS_ID_2) )
    {
      return ph_sub_config_ptr->operator_name;
    }
    else
    {

      CM_ERR_0("cm_get_operator_info(), Invalid asubs_id as_id_3 without triple sim feature ");
      return OPERATOR_NULL;
    }
  }
  else
  {
    CM_ERR_1("cm_get_operator_info(), Invalid asubs_id = ",asubs_id);
    return OPERATOR_NULL;
  }
}


/*===========================================================================

@FUNCTION cm_is_operator_ct

@DESCRIPTION
   Checks if asubs_id is configured for CT

@DEPENDENCIES
  None

@RETURN VALUE
  boolean  - TRUE/FALSE

@SIDE EFFECTS
  None

===========================================================================*/
boolean cm_is_operator_ct
(
   sys_modem_as_id_e_type asubs_id
)
{
  cmph_s_type                  *ph_ptr  = cmph_ptr ();

  #ifdef FEATURE_MMODE_DUAL_SIM
  if (asubs_id == SYS_MODEM_AS_ID_2)
  {
    return FALSE;
  }
  else
  #endif
  if( asubs_id == SYS_MODEM_AS_ID_1 &&
      ph_ptr->is_cdma_subscription_available &&
      cmmmgsdi_get_imsi_mcc() == IMSI_MCC_CHINATEL_DEFAULT)
  {
    return TRUE;
  }
  return FALSE;
}


/*===========================================================================

FUNCTION cm_cmd_alloc_ac_init

DESCRIPTION
  This function allocates a new command buffer with initialized values.

DEPENDENCIES
  NONE

RETURN VALUE
  NULL - No buffer. Non-NULL - Allocated buffer

SIDE EFFECTS
  NONE.

===========================================================================*/

static cm_ac_cmd_s_type *cm_cmd_alloc_ac_init( void )
{
  cm_ac_cmd_info_s_type  *cmd_info_ptr;

  cm_ac_cmd_s_type *ac_cmd_ptr = (cm_ac_cmd_s_type*)cm_cmd_alloc(
                                                    sizeof(cm_ac_cmd_s_type));

  if ( ac_cmd_ptr != NULL )
  {
    cmd_info_ptr  = CMD_INFO_PTR( ac_cmd_ptr );
    ac_cmd_ptr->cmd_type = CM_CMD_TYPE_AC;

    cmd_info_ptr->reason = CM_AC_REASON_NONE;
  }

  return ac_cmd_ptr;
} /* cm_cmd_alloc_ac_init() */

/*===========================================================================

FUNCTION cm_ac_cmd_PPP_nw_cleanup

DESCRIPTION
  This function informs AC that clients want to start PPP cleaning up process.

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  NONE.

===========================================================================*/

boolean cm_ac_cmd_PPP_nw_cleanup(

  cm_ac_cmd_cb_f_type              cmd_cb_func,
    /* client callback function */

  cm_ac_state_cb_f_type            state_cb_func,
    /* client callback function for ac state */

  void                             *data_block_ptr,
    /* pointer to client callback data block */

  cm_client_id_type                client_id
    /* originating client */
)
{
  cm_ac_cmd_s_type        *ac_cmd_ptr;
  cm_ac_cmd_info_s_type   *cmd_info_ptr;
  cm_ac_cmd_err_e_type    cmd_err = CM_AC_CMD_ERR_NOERR;
  boolean                 result = TRUE;;

 

  if (cmd_cb_func == NULL || state_cb_func == NULL)
  {
    CM_MSG_HIGH_0( "cm_ac_cmd_PPP_nw_cleanup(): NULL call back function ");
    return FALSE;
  }

  /* Allocate a new command buffer with initialized values */
  ac_cmd_ptr = cm_cmd_alloc_ac_init();

  /* No buffer is allocated */
  if ( ac_cmd_ptr == NULL )
  {
    cmd_err = CM_AC_CMD_ERR_NO_BUF_L;
    result = FALSE;
  }

  cmd_info_ptr = CMD_INFO_PTR( ac_cmd_ptr );

  /* Fill out command information */

  /* This API is definitely used for PPP cleaning. So command reason may not need
   * to be checked here.
   */
  ac_cmd_ptr->cmd = CM_AC_CMD_CLEAR_1XPPP;
  cmd_info_ptr->reason = CM_AC_REASON_UNUSED_PPP;
  cmd_info_ptr->asubs_id = SYS_MODEM_AS_ID_NONE;

  ac_cmd_ptr->cmd_cb_func           = cmd_cb_func;
  ac_cmd_ptr->state_cb_func         = state_cb_func;
  ac_cmd_ptr->data_block_ptr        = data_block_ptr;
  ac_cmd_ptr->client_id             = client_id;
  ac_cmd_ptr->cmd_err               = cmd_err;

  cm_cmd_queue( (cm_cmd_type*)ac_cmd_ptr );

  return result;
} /* cm_ac_cmd_PPP_nw_cleanup() */

/*===========================================================================

FUNCTION cm_user_pref_init

DESCRIPTION
  This function initializes payload used to make user pref change.

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE - If initialized, FALSE - if an invalid payload pointer is passed.

SIDE EFFECTS
  NONE.

===========================================================================*/

boolean cm_user_pref_init(

  cm_cmd_user_pref_update_type *user_pref
    /* Has an update for user preferences requested. This is pointer to a
    ** dynamic memory allocated for the purpose passing data to CM. */

)
{
  if(user_pref == NULL)
  {
    CM_MSG_HIGH_0("user_pref is NULL, can't initialize");
    return FALSE;
  }

  /* Reset all fields to zero */
  memset(user_pref, 0, sizeof(cm_cmd_user_pref_update_type));

  /* Now set fields for which 0 is not invalid */
  user_pref->asubs_id = SYS_MODEM_AS_ID_NO_CHANGE;
  user_pref->mode_pref = CM_MODE_PREF_NO_CHANGE;
  user_pref->pref_term = CM_PREF_TERM_NONE;
  user_pref->acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  user_pref->band_pref = CM_BAND_PREF_NO_CHANGE;
  user_pref->lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  user_pref->tds_band_pref = CM_BAND_PREF_NO_CHANGE;
  user_pref->prl_pref = CM_PRL_PREF_NO_CHANGE;
  user_pref->roam_pref = CM_ROAM_PREF_NO_CHANGE;
  user_pref->hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  user_pref->srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
  user_pref->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
  user_pref->csg_id = SYS_CSG_ID_INVALID;
  user_pref->manual_rat = SYS_RAT_NONE;
  user_pref->ue_usage_setting = SYS_UE_USAGE_SETTING_NO_CHANGE;
  user_pref->voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NO_CHANGE;
  user_pref->lte_disable_cause = CM_LTE_DISABLE_CAUSE_NO_CHANGE;
  user_pref->req_id = 0;

  return TRUE;
}

/*===========================================================================

FUNCTION cm_user_pref_update_req

DESCRIPTION
  API is used to queue a cmd to CM for processing the user preference change

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE - success, FALSE - on failure to queue cmd.

SIDE EFFECTS
  NONE.

===========================================================================*/

boolean cm_user_pref_update_req(

  cm_cmd_user_pref_update_type *user_pref,
  /**< Has an update for user preferences requested. This is pointer to a
     memory allocated by caller for the purpose passing data to CM. */

  cm_ph_cmd_cb_f_type               cmd_cb_func,
  /**< Client callback function. */

  void                             *data_block_ptr
  /**< Pointer to the client callback data block. */
)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;

  ph_cmd_ptr = cm_cmd_alloc_ph_init();
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = user_pref->client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_SYS_SEL_PREF;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr->mode_pref             = user_pref->mode_pref;
  cmd_info_ptr->pref_term             = user_pref->pref_term;
  cmd_info_ptr->pref_duration         = user_pref->pref_duration;
  cmd_info_ptr->acq_order_pref        = user_pref->acq_order_pref;
  cmd_info_ptr->band_pref             = user_pref->band_pref;
  cmd_info_ptr->lte_band_pref         = user_pref->lte_band_pref;
  cmd_info_ptr->tds_band_pref         = user_pref->tds_band_pref;
  cmd_info_ptr->prl_pref              = user_pref->prl_pref;
  cmd_info_ptr->roam_pref             = user_pref->roam_pref;
  cmd_info_ptr->hybr_pref             = user_pref->hybr_pref;
  cmd_info_ptr->srv_domain_pref       = user_pref->srv_domain_pref;
  cmd_info_ptr->network_sel_mode_pref = user_pref->network_sel_mode_pref;
  cmd_info_ptr->sys_sel_pref_req_id   = user_pref->req_id;
  cmd_info_ptr->csg_id                = user_pref->csg_id;
  cmd_info_ptr->csg_rat               = user_pref->manual_rat;
  cmd_info_ptr->ue_usage_setting      = user_pref->ue_usage_setting;
  cmd_info_ptr->voice_domain_pref     = user_pref->voice_domain_pref;
  cmd_info_ptr->lte_disable_cause     = user_pref->lte_disable_cause;

  if ( user_pref->plmn_ptr != NULL )
  {
    cmd_info_ptr->plmn                = *(user_pref->plmn_ptr);
  }
  else
  {
    sys_plmn_undefine_plmn_id( &cmd_info_ptr->plmn );
  }

  cmd_info_ptr->cmd_subs              = user_pref->asubs_id;

  if( user_pref->rat_acq_order_pref_ptr != NULL )
  {
    cmd_info_ptr->rat_acq_order_pref = *(user_pref->rat_acq_order_pref_ptr);

    if(user_pref->rat_acq_order_pref_ptr->type == CM_ACQ_ORDER_TYPE_GW)
    {
      /* If it's GW type, fill the gw_acq_order
      */
      cmd_info_ptr->acq_order_pref =
                  user_pref->rat_acq_order_pref_ptr->acq_order.gw_acq_order;
    }
    else if(user_pref->rat_acq_order_pref_ptr->type ==
                                                    CM_ACQ_ORDER_TYPE_RAT_PRI)
    {
      /* If it's RAT_PRI type, fill the gw_acq_order as no change
      */
      cmd_info_ptr->acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
    }
  }
  else
  {
    cmd_info_ptr->acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug messages for params.
  */
  CM_MSG_HIGH_6("sys_sel_pref(), mode_pref %d, pref_term %d, acq_order %d, sub %d, roam_pref %d, hybr_pref %d",
              cmd_info_ptr->mode_pref, cmd_info_ptr->pref_term, cmd_info_ptr->acq_order_pref,
              cmd_info_ptr->cmd_subs, cmd_info_ptr->roam_pref, cmd_info_ptr->hybr_pref);

  CM_MSG_HIGH_6("sys_sel_pref(),domain_pref %d,net_sel_mode_pref %d ,prl_pref %d,req_id %u, CSG ID %d, CSG RAT %d",
              cmd_info_ptr->srv_domain_pref, cmd_info_ptr->network_sel_mode_pref, cmd_info_ptr->prl_pref,
               cmd_info_ptr->sys_sel_pref_req_id, cmd_info_ptr->csg_id, cmd_info_ptr->csg_rat);

  CM_MSG_HIGH_1("sys_sel_pref(),ue_usage_setting %d",cmd_info_ptr->ue_usage_setting);

  CM_MSG_HIGH_4("sys_sel_pref(),bands(MSB)(LSB): cgw 0x%08x 0x%08x, tds 0x%08x 0x%08x",
                                  QWORD_HIGH(cmd_info_ptr->band_pref),
                                    QWORD_LOW(cmd_info_ptr->band_pref),
                                  QWORD_HIGH(cmd_info_ptr->tds_band_pref),
                                    QWORD_LOW(cmd_info_ptr->tds_band_pref));

  cm_print_lte_band_mask(cmd_info_ptr->lte_band_pref);

  sys_plmn_get_mcc_mnc
  (
    cmd_info_ptr->plmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );

  CM_MSG_HIGH_4("PLMN - undefined %d pcs_digit %d PLMN(%d-%d)",
                plmn_id_is_undefined,
                mnc_includes_pcs_digit,
                mcc, mnc);
 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;
}
/*===========================================================================

FUNCTION cm_call_is_there_a_call_type_per_subs

DESCRIPTION
  Checks is there is a requested call type.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if there a call with call_type
  FALSE else cases

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_call_is_there_call_on_other_sub
(

  sys_modem_as_id_e_type       asubs_id,

  cm_call_type_mask_type         call_type_mask

)
{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( (call_ptr->asubs_id != asubs_id
         ) &&
         (SYS_BM_64BIT(call_ptr->call_type) & call_type_mask)
       )
    {
        return TRUE;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return FALSE;

}


/*===========================================================================
FUNCTION cm_ac_cmd_control_nw_access

DESCRIPTION
  This API enables CM-clients to place modem in certain special state.
  Same API is used put the modem back in normal operation. Restriction is
  passed as part of cm_ac_cmd_e_type. And reason for placing in special state
  is passed as part of cm_ac_reason_e_type.

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  NONE.

===========================================================================*/

extern boolean cm_ac_cmd_control_nw_access(

  cm_ac_cmd_cb_f_type              cmd_cb_func,
  /* client callback function */

  void                             *data_block_ptr,
  /* pointer to client callback data block */

  cm_client_id_type                client_id,
  /* originating client */

  cm_ac_cmd_e_type                 ac_cmd,
  /* access restriction cmd */

  cm_ac_reason_e_type              ac_reason,
  /* Reason for requesting to control access */

  sys_modem_as_id_e_type           asubs_id
  /* subs on which call needs to be originated*/

)
{
  cm_ac_cmd_s_type        *ac_cmd_ptr;
  cm_ac_cmd_info_s_type   *cmd_info_ptr;

  CM_MSG_HIGH_3("control_nw_access cmd %d reason %d as_id %d",
             ac_cmd, ac_reason, asubs_id);

  /* Allocate a new command buffer with initialized values */
  ac_cmd_ptr = cm_cmd_alloc_ac_init();

  /* No buffer is allocated */
  if ( ac_cmd_ptr == NULL )
  {
    return FALSE;
  }

  cmd_info_ptr = CMD_INFO_PTR( ac_cmd_ptr );

  ac_cmd_ptr->cmd = ac_cmd;
  cmd_info_ptr->reason = ac_reason;
  cmd_info_ptr->asubs_id = asubs_id;

  ac_cmd_ptr->cmd_cb_func           = cmd_cb_func;
  ac_cmd_ptr->data_block_ptr        = data_block_ptr;
  ac_cmd_ptr->client_id             = client_id;

  cm_cmd_queue( (cm_cmd_type*)ac_cmd_ptr );

  return TRUE;
}
/*===========================================================================

FUNCTION cm_ph_set_trm_priority

DESCRIPTION
  AP request to block/unblock  tuneaway on other sub when AP IMS 
  call ongoing

DEPENDENCIES
  None

RETURN VALUE
  TRUE if request success
  False otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_ph_set_trm_priority
(

   cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  sys_modem_as_id_e_type      asubs_id,

  boolean					tune_away_req

  )
{
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_SET_LTE_DSDS_TRM_PRIOIRTY
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->info.cmd_subs           = asubs_id;
  ph_cmd_ptr->info.tune_away_req      = tune_away_req;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_SET_TRM_PRIORITY;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr                         = CMD_INFO_PTR( ph_cmd_ptr );
  cmd_info_ptr->cmd_err                = CM_PH_CMD_ERR_NOERR;


  CM_MSG_HIGH_2("cm_ph_set_trm_priority: requested tuneaway %d, subs %d",
                tune_away_req, ph_cmd_ptr->info.cmd_subs);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

return TRUE;
}

/*===========================================================================
FUNCTION cm_ph_cmd_set_wd_switch

DESCRIPTION

  Queue CM_PH_CMD_SET_WD_SWITCH command to CM.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_set_wd_switch(

  cm_ph_cmd_cb_f_type                 cmd_cb_func,
    /* client cmd callback function */

  void                                *data_block_ptr,
    /* pointer to client cmd callback data block */

  cm_client_id_type                    client_id,
    /* commanding client's id */

  boolean                              wd_switch_status
    /* WD switch status to be set to */

)
/*lint -esym(715,client_id)*/
/*lint -esym(715,drx_coefficient)*/
{
  cm_ph_cmd_s_type              *ph_cmd_ptr;
  cm_ph_cmd_info_s_type         *cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_PLMN_BLOCKING
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_SET_WD_SWITCH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */
  cmd_info_ptr                         = CMD_INFO_PTR( ph_cmd_ptr );
  cmd_info_ptr->cmd_err                = CM_PH_CMD_ERR_NOERR;
  cmd_info_ptr->wd_switch_on           = wd_switch_status;

  CM_MSG_HIGH_1("wd_switch_status=%d",wd_switch_status);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

}
/**===========================================================================

@FUNCTION cm_ph_cmd_set_data_priority_per_subs

@DESCRIPTION
  Command CM to change the data priority of sub

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@x
===========================================================================*/
boolean cm_ph_cmd_set_data_priority_per_subs(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /**< Client callback function */

  void                              *data_block_ptr,
    /**< Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /**< Requesting client */

  sys_data_priority_e_type  priority_type,
    /* specifies the prirotiy to be set for corresponding subid*/
            
  sys_modem_as_id_e_type       asubs_id
   /* SUB id for which priority has to be updated */
)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_DATA_PRIORITY;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */

  cmd_info_ptr->cmd_subs             = asubs_id;
  cmd_info_ptr->priority_type        = priority_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug messages for params.
  */
  CM_MSG_HIGH_2("cm_ph_cmd_set_data_priority_per_subs(): priority_type %d, sub_id %d, enqueuing SET_DATA_PRIRORITY_CMD",
                priority_type,asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

}

/**===========================================================================

@FUNCTION cm_ph_cmd_data_roaming_per_subs

@DESCRIPTION
  Command CM to send data roaming setting to NAS.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@x
===========================================================================*/
boolean cm_ph_cmd_data_roaming_per_subs(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                             *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  sys_data_roaming_e_type           roam_type,
    /* New nternational roaming setting */
            
  sys_modem_as_id_e_type            asubs_id
   /* Sub to which new roam setting pertains */
)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_DATA_ROAMING;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */

  cmd_info_ptr->cmd_subs             = asubs_id;
  cmd_info_ptr->roam_type            = roam_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug messages for params.
  */
  CM_MSG_HIGH_2 ("enqueuing DATA_ROAMING_CMD - roam_type=%d,asubs_id=%d" ,
                roam_type,asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

}


/**===========================================================================

@FUNCTION cm_ph_cmd_inform_secure_call_capability

@DESCRIPTION
  Command CM to send CM_PH_CMD_INFORM_SECURE_CALL_CAPABILITY to IMS.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@x
===========================================================================*/
boolean cm_ph_cmd_inform_secure_call_capability(

  cm_ph_cmd_cb_f_type                         cmd_cb_func,
  
  void                                       *data_block_ptr,
  
  cm_client_id_type                           client_id,

  cm_ph_secure_call_capability_e_type         secure_call_capability,
  
  sys_modem_as_id_e_type                      asubs_id
    /* Subscription ID to which these preferences should apply. */
)
{
#if defined(FEATURE_IP_CALL)
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func                     = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr                  = data_block_ptr;
  ph_cmd_ptr->client_id                       = client_id;
  ph_cmd_ptr->cmd                             = CM_PH_CMD_INFORM_SECURE_CALL_CAPABILITY;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy command information fields.
  */

  cmd_info_ptr->cmd_subs                     = asubs_id;
  cmd_info_ptr->secure_call_capability       = secure_call_capability;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Print debug messages for params.
  */
  CM_MSG_HIGH_2 ("cm_ph_cmd_inform_secure_call_capability - cap %d, asubs_id %d" ,
                secure_call_capability, asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;
#else
  return FALSE;
#endif // FEATURE_IP_CALL
}


/**===========================================================================

@FUNCTION cm_ph_cmd_set_roaming_pref_per_subs

@DESCRIPTION
  Command CM to send roaming settings to NAS.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@x
===========================================================================*/
boolean cm_ph_cmd_set_roaming_pref_per_subs(

  cm_ph_cmd_cb_f_type               cmd_cb_func,
    /* Client callback function */

  void                             *data_block_ptr,
    /* Pointer to client callback data block */

  cm_client_id_type                 client_id,
    /* Requesting client */

  cm_set_roaming_pref_param_s_type  roaming_pref,
    /* New international roaming setting */

  sys_modem_as_id_e_type            asubs_id
   /* Sub to which new roam setting pertains */
)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;

  if ( CM_ROAMING_PREF_CMD_NONE >= roaming_pref.cmd_type ||
       CM_ROAMING_PREF_CMD_MAX <= roaming_pref.cmd_type )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_SET_ROAMING_PREF;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */

  cmd_info_ptr->cmd_subs             = asubs_id;
  cmd_info_ptr->roaming_pref         = roaming_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug messages for params.
  */
  CM_MSG_HIGH_3 ("PH_CMD_SET_ROAMING_PREF - roaming_type: %d, roaming_allowed: %d,asubs_id: %d",
    roaming_pref.cmd_type, roaming_pref.roaming_allowed, asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

} /* cm_ph_cmd_set_roaming_pref_per_subs () */

/**===========================================================================

@FUNCTION cm_msim_set_data_priority

@DESCRIPTION
  Command CM to change the data priority of sub

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  TRUE - request sent, FALSE - No buffer to send request

@x
===========================================================================*/
boolean cm_msim_set_data_priority(

  
  cm_client_id_type                 client_id,
    /**< Requesting client */
            
  sys_modem_as_id_e_type       asubs_id,
  /* SUB id for which priority has to be updated */

  sys_data_priority_e_type  priority_type
   /* specifies the prirotiy to be set for corresponding subid*/
   
)
{

	cmph_s_type                  *ph_ptr  = cmph_ptr ();

	#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)
    reg_cmd_type                *reg_ptr;
    #endif

	if(cmpmprx_get_num_of_active_data()!=2) 
	{
	  CM_MSG_HIGH_0( "CM_PS_PS Dual Data Feature not supported");
	  return FALSE;
	}

    #if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)

	ph_ptr->data_priority_per_sub[asubs_id].priority_type = priority_type;

	reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_DATA_PRIORITY_IND );

    /* Fill in parameters. */
    reg_ptr->cmd.cm_data_prio_ind.as_id = asubs_id;
	reg_ptr->cmd.cm_data_prio_ind.priority = priority_type;

    /* print the cmd details
      */
    CM_MSG_HIGH_2("Sending CM_DATA_PRIORITY_IND as_id=%d, priority=%d",
                  reg_ptr->cmd.cm_data_prio_ind.as_id,
				  reg_ptr->cmd.cm_data_prio_ind.priority);

      /* Send the request to NAS */
    cm_reg_send_cmd( reg_ptr );

    
    #endif 

   return TRUE;

}

/*===========================================================================
FUNCTION cm_ph_cmd_reset_clnup_st_mc

DESCRIPTION

  Queue CM_PH_CMD_RESET_CLNUP_ST_MC command to CM.
  This is an internal command as yet not intended to be utilized by any
  other client.
  This will simply reset the Cleanup state machine.

  This is to mark the position in the CM command queue after all
  the commands are transferred from Buffer queue to command queue.
  The implication is that all the cleanup for the preceding commands has been
  completed and now the susequent commands will be evaluated for cleanup.

DEPENDENCIES
  None

RETURN VALUE

  TRUE
  Command was queued succesfully to CM

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_ph_cmd_reset_clnup_st_mc( )
{
  cm_ph_cmd_s_type              *ph_cmd_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Now that all function parameters are correct,
  ** get command buffer to queue CM_PH_CMD_PLMN_BLOCKING
  */
  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /*
  ** Command buffer allocation is successful,
  ** copy command information into command structure.
  */

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = NULL;
  ph_cmd_ptr->data_block_ptr          = NULL;
  ph_cmd_ptr->client_id               = CM_CLIENT_ID_ANONYMOUS;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_RESET_CLNUP_ST_MC;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;

}

/*===========================================================================

FUNCTION cm_set_srvcc_capability

DESCRIPTION
  UE can send SRVCC capability to NW with this API

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UE wants to enable SRVCC
  FALSE SRVCC capability disabled on UE

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_set_srvcc_capability
(

  cm_ph_cmd_cb_f_type              cmd_cb_func,
  /* client callback function */

  void                             *data_block_ptr,
  /* pointer to client callback data block */

  cm_client_id_type                client_id,
  /* originating client */

  sys_modem_as_id_e_type      asubs_id,

  boolean                     srvcc_capability

 
)
{
  cm_ph_cmd_s_type        *ph_cmd_ptr;
  cm_ph_cmd_info_s_type   *cmd_info_ptr;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ph_cmd_ptr = cm_cmd_alloc_ph_init();

  /* Get a reference to command information.
  */
  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command header fields.
  */
  ph_cmd_ptr->cmd_cb_func             = cmd_cb_func;
  ph_cmd_ptr->data_block_ptr          = data_block_ptr;
  ph_cmd_ptr->client_id               = client_id;
  ph_cmd_ptr->cmd                     = CM_PH_CMD_SET_SRVCC_CAPABILITY;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields.
  */

  cmd_info_ptr->cmd_subs             = asubs_id;
  cmd_info_ptr->srvcc_capability     = srvcc_capability;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug messages for params.
  */
  CM_MSG_HIGH_2 ("cm_set_srvcc_capability() - srvcc_capability=%d,sub_id=%d" ,
                srvcc_capability,asubs_id);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)ph_cmd_ptr );

  return TRUE;
}
/*======================================================================================

FUNCTION cm_get_rat_expansion_status

DESCRIPTION
 returns the staus of the rat expansion 
 
A-> any mode pref apart from H & L

old mode	oldRDM	new mode	newRDM	result
AHL	         0	      AHL	      HL	CM_RAT_EXPANSION_STATUS_DEACTIVATED
AHL	         HL	      AHL	      HL	CM_RAT_EXPANSION_STATUS_NO_CHANGE
AHL	         0	       A	       0	CM_RAT_EXPANSION_STATUS_DEACTIVATED
AHL	         HL	       A	       0	CM_RAT_EXPANSION_STATUS_NO_CHANGE
H	         H	       AHL	       0	CM_RAT_EXPANSION_STATUS_ACTIVATED
H	         H	       H	       0	CM_RAT_EXPANSION_STATUS_ACTIVATED
H	         H	       AHL	      HL	CM_RAT_EXPANSION_STATUS_NO_CHANGE
H	         H	       AHL	       H	CM_RAT_EXPANSION_STATUS_ACTIVATED
H	         H	       AHL	       L	CM_RAT_EXPANSION_STATUS_ACTIVATED
H	         H	       H	      H	    CM_RAT_EXPANSION_STATUS_NO_CHANGE
AHL	         H	       H	      H	    CM_RAT_EXPANSION_STATUS_DEACTIVATED
AHL	         H	       H	      0	    CM_RAT_EXPANSION_STATUS_NO_CHANGE
AHL	         H	       A	      0	    CM_RAT_EXPANSION_STATUS_DEACTIVATED
AHL	         0	       AHL	      0	    CM_RAT_EXPANSION_STATUS_NO_CHANGE


DEPENDENCIES
  None

RETURN VALUE
  TRUE if request success
  False otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_rat_expansion_status_e_type cm_get_rat_expansion_status
(

  cm_mode_pref_e_type               old_mode_pref,
    /* old mode pref */

  cm_mode_pref_e_type               new_mode_pref,
    /* new mode pref */

  uint32      old_rat_disabled_mask,
 
    /* old rat disabled mask */

  uint32      new_rat_disabled_mask

    /* new rat disabled mask */

)
{

 uint32 is_new_ps_rat_enabled = 0, is_old_ps_rat_enabled = 0;

 sd_ss_mode_pref_e_type sd_old_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(old_mode_pref);
 sd_ss_mode_pref_e_type sd_new_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(new_mode_pref);


 sd_ss_mode_pref_e_type update_sd_old_mode_pref = sd_old_mode_pref & (SD_SS_MODE_PREF_LTE | SD_SS_MODE_PREF_HDR );

 sd_ss_mode_pref_e_type update_sd_new_mode_pref = sd_new_mode_pref & (SD_SS_MODE_PREF_LTE | SD_SS_MODE_PREF_HDR );

 CM_ASSERT( BETWEEN(old_mode_pref, CM_MODE_PREF_NONE, CM_MODE_PREF_MAX ));
 CM_ASSERT( BETWEEN(new_mode_pref, CM_MODE_PREF_NONE, CM_MODE_PREF_MAX ));

 is_old_ps_rat_enabled = update_sd_old_mode_pref ^ old_rat_disabled_mask;

 is_new_ps_rat_enabled = update_sd_new_mode_pref ^ new_rat_disabled_mask; 

 if(!is_old_ps_rat_enabled && is_new_ps_rat_enabled)
 {
   CM_MSG_HIGH_0("CM_RAT_EXPANSION_STATUS: ACTIVATED");
   return CM_RAT_EXPANSION_STATUS_ACTIVATED;
 }
 else if(!is_new_ps_rat_enabled && is_old_ps_rat_enabled)
 {
   CM_MSG_HIGH_0("CM_RAT_EXPANSION_STATUS: DEACTIVATED");
   return CM_RAT_EXPANSION_STATUS_DEACTIVATED;
 }
 else 
 {
   return CM_RAT_EXPANSION_STATUS_NO_CHANGE;
 }

}

/**==========================================================================

@FUNCTION cm_mm_call_cmd_drvcc_setup_per_subs 

@DESCRIPTION
  Command CM to do DRVCC for calls that are send as part of 
  parameters, to be used by IMS in particular.
  Success would be notified to IMS via DRVCC_HO_COMPLETE_NOTIFY
  Failure would be reported via DRVCC_HO_FAIL_NOTIFY

@DEPENDENCIES
  cm_init_before_task_start() must have already been called.

@RETURNS
  - TRUE - request sent
  - FALSE - No buffer to send request, or if there is limited service, and DRVCC can not proceed.

@SIDE_EFFECT

@x
===========================================================================*/
extern boolean cm_mm_call_cmd_drvcc_setup_per_subs(
  cm_drvcc_setup_req_s_type          *drvcc_call_info
  /* info of calls that are to be drvcc'ed */
)
{
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  cmss_s_type *ss_ptr = cmss_ptr();
  cm_call_cmd_s_type           *call_cmd_ptr;
  cm_call_cmd_info_s_type      *cmd_info_ptr;
  cm_call_cmd_err_e_type        cmd_err = CM_CALL_CMD_ERR_NOERR;
  cm_ss_e_type cm_ss = CM_SS_NONE;
  sys_srv_status_e_type srv_status = SYS_SRV_STATUS_NONE;
  sys_roam_status_e_type roam_status = SYS_ROAM_STATUS_NONE;
  boolean is_cdma_custom_home = TRUE;

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cmph_s_type                  *ph_ptr  = cmph_ptr ();
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("DRVCC: setup command, drvcc_type %d, as_id %d, num_of_calls %d",
                drvcc_call_info->drvcc_type, drvcc_call_info->as_id, drvcc_call_info->num_of_calls);
  CM_MSG_HIGH_1("DRVCC: roaming_allowed %d",drvcc_call_info->drvcc_allowed_in_roaming);

  /* check if valid drvcc_type */
  if(!cmcall_drvcc_is_valid_drvcc_type(drvcc_call_info->drvcc_type))
  {
    CM_MSG_HIGH_0("DRVCC: invalid drvcc type");
    return FALSE;
  }
  /* Sanity check on cache from IMS */
  if( (drvcc_call_info->num_of_calls > MAX_CALL_NUM_DRVCC) ||
    (drvcc_call_info->num_of_calls == 0) )
  {
    CM_MSG_HIGH_1("DRVCC: num_of_calls in info from IMS %d",drvcc_call_info->num_of_calls);
    return FALSE;
  }
  
  cm_ss = cmph_map_as_id_to_cm_ss(drvcc_call_info->as_id);

  if(cm_ss == CM_SS_MAIN)
  {
    srv_status = ss_ptr->info.srv_status;
    roam_status = ss_ptr->info.roam_status;
    if(ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA)
    {
      is_cdma_custom_home = ss_ptr->info.is_cdma_custom_home;
    }
  }
  else if(cm_ss == CM_SS_HYBR_2)
  {
    srv_status = ss_ptr->info.gw_srv_status;
    roam_status = ss_ptr->info.gw_roam_status;
  }
  else if(cm_ss == CM_SS_HYBR_3)
  {
    srv_status = ss_ptr->info.gw3_srv_status;
    roam_status = ss_ptr->info.gw3_roam_status;
  }
  else
  {
    CM_MSG_HIGH_1("DRVCC: Invalid stack returned for DRVCC setup %d",cm_ss);
    return FALSE;
  }

  /* if UE is in limited service then reject DRVCC */
  if(sys_srv_status_is_lmtd_or_lmtd_reg_srv(srv_status))
  {
    CM_MSG_HIGH_0("DRVCC: Current srv status is lmtd or lmtd regional so cancel DRVCC");
    return FALSE;
  }

  if( (drvcc_call_info->drvcc_allowed_in_roaming ==  FALSE) &&
    (cmcall_drvcc_is_drvcc_allowed_per_roam_status(srv_status, roam_status, is_cdma_custom_home) == FALSE) 
    )
  {
    CM_MSG_HIGH_0("DRVCC: Current srv status is roaming so cancel DRVCC");
    return FALSE;
  }
    /**
  ** Check if we need to validate STN number
  */
  
  /** Allocate and initialize call command buffer
  */
  call_cmd_ptr = cm_cmd_alloc_call_init();

  /**
  ** Copy command header fields.
  */
  call_cmd_ptr->client_categ    = CM_ORIG_CLIENT_CATEG_USER;
  call_cmd_ptr->cmd             = CM_CALL_CMD_DRVCC_SETUP_REQ;
  call_cmd_ptr->direction       = CM_CALL_DIRECTION_MO;

  /**
  ** If no parameter errors found, pack cmd info and queue it for CM,
  ** otherwise just queue it with command error
  */
  cmd_info_ptr          = CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr->cmd_err = cmd_err;  

    /**<
    ** Copy command information fields.
    */
    /* this call id is to be used as switched in call id */
  cmd_info_ptr->call_id                   = cm_call_id_allocate();
  if(cmd_info_ptr->call_id == CM_CALL_ID_INVALID)
  {
    CM_MSG_HIGH_0("DRVCC: CM could not allocate call id for DRVCC setup");
    cm_cmd_dealloc(call_cmd_ptr);
    return FALSE;
  }  

  /* param: if IMS sends drvcc_type as CSFB, but CM is OOS at that time
  then CM will map drvcc_type no either NATIVE 1X or NATIVE 3GPP 
  Ideally chance for this should be less as IMS would call 
  SDS interface before sending the drvcc setup to CM
  and SDS uses intl srv info ptr to know if CSFB is supported or not*/

  call_cmd_ptr->cmd_subs  = drvcc_call_info->as_id;
  /* Deep copy DRVCC setup related params */
  memscpy(&cmd_info_ptr->drvcc_setup_info,
          sizeof(cmd_info_ptr->drvcc_setup_info),
          drvcc_call_info,
          sizeof(cm_drvcc_setup_req_s_type));

  /** Queue the command to CM.
  */
  cm_cmd_queue( (cm_cmd_type*)call_cmd_ptr );
  return TRUE;

  #else
  
  return FALSE;
  
  #endif

} /* cm_mm_call_cmd_drvcc_setup_per_subs() */
/*===========================================================================

FUNCTION cm_notify_ext_volte_state

DESCRIPTION
 notify ext VoLTE state to CM when a call starts or ends.
 Ex: if a VoLTE call ends, the function is called like this:
   cm_notify_ext_volte_state(SYS_MODEM_AS_ID_1, CM_CALL_TYPE_VOICE_MASK, CM_CALL_STATUS_END);

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cm_notify_ext_volte_state
(

  sys_modem_as_id_e_type        asubs_id,
    /* subs ID of the call */

  cm_call_type_mask_type        call_type, 
    /* bit mask for call type*/

  cm_volte_call_state_e_type    call_state
    /* state of call (either started or ended) */

)
{
  #if defined(FEATURE_QSH_EVENT_METRIC)

  /* for voice/VT/emerg call end, send metric/event to QSH */
  if( call_state == CM_CALL_STATUS_END && 
      (call_type & (CM_CALL_TYPE_VOICE_MASK | 
                    CM_CALL_TYPE_VT_MASK |
                    CM_CALL_TYPE_EMERG_MASK)) 
    )
  {
    /* allocate memory for call_ptr */
    cmcall_s_type *call_ptr;
    call_ptr = (cmcall_s_type *)cm_mem_malloc(sizeof(cmcall_s_type));

    if(call_ptr == NULL)
    {
      //CM_MSG_HIGH_0("couldn't allocate memory for call_ptr, cm_mem_malloc() returned NULL");
      return;
    }

	memset(call_ptr,0,sizeof(cmcall_s_type));

    /* fill in call_ptr fields needed to send to QSH */
    call_ptr->asubs_id = asubs_id;
    call_ptr->call_type = CM_CALL_TYPE_EXT;
    call_ptr->call_state = CM_CALL_STATE_IDLE;
    call_ptr->prev_call_state = CM_CALL_STATE_IDLE;

    /* update call end metrics and send metric/event to QSH */
    cmdbg_chk_and_update_call_end_info_metrics(CM_CALL_EVENT_END, call_ptr);

    /* free call_ptr */
    cm_mem_free(call_ptr);
  }
  
  #endif
}
