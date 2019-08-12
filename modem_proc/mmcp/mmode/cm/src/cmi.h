#ifndef CMI_H
#define CMI_H
/*===========================================================================

              C A L L   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the Call Manager.
  It provides internal interface to the CM.C file.
  It should NOT be included by any source file outside the Call Manager


Copyright (c) 1991 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/13   mh/jvo  Mainline the modem statistics code
08/13/13   sm      MidCall SRVCC feature
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/19/13   jvo     Remove all MDM/MSM Fusion features
06/24/13   xs      VT answered as voice
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
04/25/13   xs      FRAT support
03/06/13   qf      FR3255 - Get T3402 value through message router
01/07/12   xs      Added PS supplemetary service support
09/26/12   skk     Changes of ESM SM backoff timer requirements
08/07/12   gm      RF init status synch and handling oprt modes
07/20/12   fj      Added support for sending NAS request to set IRAT timer for
                   TDS to LTE HPLMN search.
03/01/12   fj      Added support for DRX.
05/01/12   gm      New offline notify processing is added - TMC deprecation.
03/06/12   jh      Added eMBMS coverage status indication handling
02/23/12   vk      Provided config request support to CMSDS
02/15/12   vk      Replace usage of AEE library function with corresponding
                   CoreBSP library
11/30/11   jh      Added feature for dynamic rat acq order change
01/04/11   vk      Changes for CSFB CS DATA call feature
02/13/12   xs      VT upgrade/downgrade support
02/07/12   xs      VOLTE silent redial support
01/17/12   aj      SC SVLTE SO 33 suspend changes
01/06/12   gm      Data synchronization changes for freefloating platform
01/03/12   vk      Removal of feature flag for CSFB CS DATA calls
12/22/11   xs      Subscription buffer handling fix
12/20/11   vk      Changes for CSFB CS DATA call feature
12/21/11   xs      L->W SRVCC support
12/01/11   vk      PS voice domain selection migration from CNE to CM
06/16/11   ak      MDM to notify MSM LTE activity status for MMSS arbitration
06/14/11   xs      HDR optmize handoff: activate HDR tunnel when attach complete
06/14/11   gm      Thermal emergency fixes - resuming HDR, LPM->Online
04/21/11   sv      Correct Lint Errors
04/12/11   fj      Add new fields in cm_ph_cmd_info_s_type for PLMN blocking.
03/22/11   gm      Fix to waiting 4s after call end to force THERM EMERG
03/22/11   rk      Support for handling negative scenarios
03/15/11   gm      Separate features for CFM interface & therm emerg
02/28/11   gm      Thermal emergency feature support
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
12/23/10   sv      Integrate DSDS feature
12/01/10   sv      Move stubs to test headers. Fix lint errors on MOB
11/24/10   xs      Fusion featurization clean up
11/15/10   rm      Memory optimization changes
11/05/10   aj      do -> lte reselection
10/28/10   gm      Fix to compilation warning.
10/20/10   xs      Added new fields rsrp_delta and rsrq_delta to cm_ph_cmd_info_s
10/01/10   ak      Merged the Fusion code from Dev branch.
09/15/10   rm      Release-7 changes
08/30/10   fj      Add support for LTE call control.
08/30/10   xs      Added new function cm_hold_cmd_q_add_uncond().
08/18/10   fj      Handle LTE RRC service indication to update cell_id.
08/16/10   aj      support VOIP deregistration on ps detach
04/09/09   aj      Added support for IRAT measurements
                   and hold cmd reason for hold cmd Q
03/16/10   jqi     EHRPD VOIP silent redial support.
05/24/10   rm      Correcting featurisation of FEATURE_MODEM_STATISTICS
03/17/10   fj      Added support for LTE RSSI indication handling.
01/19/10   aj      Added VOIP support for emergency call
12/08/09   fj      Added lte_band_pref.
12/03/09   ak      Enabling FEATURE_HICPS_STACK_SYNC_UP on target by default.
10/22/09   rm      Adding Modem Statistics changes
12/22/08   fj      Added lte_info in cm_call_cmd_info_s_type;
                   Added cm_call_cmd_info_lte_s_type;
                   Added cm_msgr_cmd_s_type.
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
07/26/09   rn      Centralised call control changes
04/17/09   ks      Added cm_disable_call_type typedef for uint64 bit mask
04/14/09   sv      Corrected MBMS event buffers featurization
04/06/09   mj      Update BOOLEAN macro w/ namespace prefix to CM_BOOLEAN
04/02/09   sv      Added event buffers for MBMS during power collapse
02/25/09   sv      Correct Lint errors
12/28/08   rn      FDN checking option in call control
11/28/08   rm      CMI defeaturisation
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
09/15/08   cl      Add initialization before subscription commands are sent.
                   This allows the service domain to be sent to UI correctly.
09/04/08   dm/sv   Update Ph cmd info to accept WPS start parameter
07/01/08   rm      Mainlining of MBMS changes
05/28/08   ks      Updating the associated primary profile number when
                   secondary PDP is promoted to Primary
05/27/08   sv      Add new power collapse bitmask, to buffer any events
                   during Power Collapse State
05/06/08   ak      BCMCS 2.0 changes
04/29/08   ks      Reverting the changes of Updating the associated primary
                   profile number when secondary PDP is promoted to Primary
03/31/08   ks      Updating the associated primary profile number
                   when secondary PDP is promoted to Primary
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
01/03/08   ka      Adding handset based plus code dialing feature.
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/07/07   ra      Added support for IMS VCC in HOMER files
11/26/07   sv      Added CM_ALPHA_CODING_SCHEME_NONE to initialize
                   enc_alpha.coding_scheme
06/14/07   pk/cl   Added support for power suspend
04/15/07   rp      Removing FEATURE_MMGPS
04/03/07   sk      Added unused_client_ptr field to phone info object to use
                   along with CM_PH_CMD_CLIENT_RELEASE command.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/18/07   rp      Added support for WMS<->CM<->NAS interface.
11/17/06   jqi     Added support for HDR active protocol revision information.
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/07/06   sk      Memory reduction effort.
09/14/06   ka      Adding support for call control on secondary ps calls
08/28/06   ka      Adding pdp header comp and data comp enums.
07/07/06   pk      Added cmtask_add_cmd_to_hold_q, and supporting data
                   structures, fields and functions
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Changes to support call control on PS calls.
06/14/06   pk      Added support for cmtask_orig_para_count_2 and related
                   enums
06/05/06   ka/ic   Adding CM_ASCII_MAX_NUMBER_CHARS.
03/27/06   pk      Added support for cmtask_orig_para_change_act_priority
                   Added helper functions for the support
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
03/09/06   pk      Added support for POWER COLLAPSE
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/19/06   pk      Externed cm_dip_switch_set_ddtm_on and
                   cm_dip_switch_set_ddtm_off
01/06/06   ic      Lint cleanup
12/09/05   ic      Lint cleanup
11/23/05   ka      Modification to allow hold/active for ip calls.
11/08/05   sk      Separated CM reports and commands.
10/23/05   jqi     Hold ps data call orig or data avail request if there is a
                   pending get network list request.
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
                   Moved cm_ss_e_type from cmcall.h
09/28/05   pk      Added CM_ACT_TYPE_NON_STD_OTASP
09/12/05   pk      Added FEATURE_GPSONE to cm_pd_cmd_info_s_type added
                   cm_pd_session_dormant_e_type session_dormant to info type
08/31/05   pk      Added support for priority Queue for the following call
                   types CM_CALL_TYPE_TEST, CM_CALL_TYPE_PD,
                   CM_CALL_TYPE_VT, CM_CALL_TYPE_VT_LOOPBACK,
                   CM_CALL_TYPE_SUPS
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/15/05   pk      Added support for priority Queues for origination
                   parameters
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/06/05   pk      Added cm_ss_cmd_info_s_type to hold SS command information
03/14/05   ka      IP rpt struct added to cm_cmd_u_type
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/03/04   ws      Add API for Remote WMS.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/27/04   jqi     Added ps dormant optimization.
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/17/04   ic      Undoing P4 change list #121638
08/12/04   ws      Remove cm20 support.
08/02/04   ic      Added Lint error supression to INRANGE()
07/28/04   ka      Added support for secondary PDP context
07/27/04   ws      Call Control support for SUPS.
07/26/04   ic      Modified INRANGE() macro to cast its parameters to int32
                   to get rid of 'pointless comparison of unsigned integer
                   with zero' RVCT 2.1 compiler warning
07/21/04   sj      Added support for umts to 1x handover.
07/12/04   ic      Added support for FEATURE_ALS
07/05/04   dk      Added Iterator Type.
06/22/04   ic      Comments clean up
06/09/04   dk      Moved cm_cmd_dealloc to cmtask.h.
06/01/04   npr     Added IP Header and Data compression requirements to GW PS
                   call origination and information structures for GPRS SNDCP.
04/28/04   ka      Fixed Lint errors.
04/13/04   ic      Featurization changes
03/30/04   ws      Moved favored_mode_pref to mode independent structs.
03/24/04   ic      Fixed compiler warning
03/17/04   sj      Added new DDTM parameters in the command info struct.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial jaguar/mainline merge.
01/19/04   ka      Added support for Multiple PDP and get call info api.
11/14/03   ws      Added support for is_jcdma_emergency
11/07/03   ws      Fixed GPS featurization compile errors
10/28/03   ws      Removed uim pref slot support.
10/20/03   ws      Dual Slot Support.
10/08/03   ws      Added ps data orig pref enums.
09/30/03   vt      Modified to support common cm.h across all targets.
09/18/03   ws      Added subscription source enum.
09/16/03   vt      Added preferred slot handling.
08/22/03   vt      Mainlined packet state handling.
08/13/03   prk     Added mimum_qos field to cm_call_cmd_info_gw_ps_s_type.
07/08/03   sbs/lt  Added support for Location Services related SS messages
05/22/03   sun     Made gw_cs and gw_ps a union in call_cmd_info_s_type
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/25/03   ws      JCDMA support for call hold
04/28/03   AT      Lint cleanup.
04/14/03   prk     Removed references to FEATURE_SUPS.
04/14/03   ws      Added BSID changes
03/11/03   AT      Added support for call control performed by clients.
02/28/03   AT      Added support for call control.
02/27/03   ws      Removed redundant end_reason and end_reason_included definitions
02/24/03   vt      Undid the changes for HDR.
02/18/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/06/03   RI      Added support for concurrent services.
                     - added end_params field in cm_call_cmd_mode_info_s_type.
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Added support for accessing user preferred PLMN list.
11/19/02   vt      Added changing RTRE configuration.
11/15/02   vt      Properly featurized the packet state.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/31/02   vt      Mainlined feature SD20.
10/22/02   ic      Added cm_m51_mode_init() prototype
09/20/02   az      Changed the type of variable l2_ack_wait_time from uint8 to dword
                   in cm_dbm_cmd_s_type
09/10/02   ws      Added initial cm20 backward compatibility.
08/30/02   vt      Added calling number to call cmd structure.
08/06/02   HQ      Added FEATURE_CDSMS.
07/22/02   PRK     Added network_list_type field to cm_ph_cmd_info_s_type.
04/02/02   hxw     Modified FEATURE_GPSONE_MSBASED.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/06/02   sh      Added hybrid 3.0 support
01/25/02   sj      Added support for tiered services - user zone support.
05/07/02   PRK     Fixed indentation.
05/05/02   vt      Fixed compile problem with new mm cm.h
02/13/02   ic      Changed the type of call_id field from byte/uint8 etc
                   to cm_call_id_type as it should have been to begin with.
02/07/02   ic      Under FEATURE_IS2000_REL_A, removed cm_nc_is_flash_emergency_func
02/04/02   RI      Added a new field ie_ss_code_s_type in
                   cm_sups_cmd_info_s_type.
01/23/02   PRK     Updated CM_PH_CMD_SELECT_NETWORK command parameters.
                   Featurized future PLMN selection/reselection code with
                   FEATURE_NEW_PLMN.
                   Updated type definition for PLMN id.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
01/14/02   ic      Fixes related to running with FEATURE_IS2000_REL_A undefined
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/21/01   RI      Modified the function declaration for cm_find_call_id.
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Added support for PLMN selection/reselection (FEATURE_PLMN).
                     Moved srv_state, roam_status, rssi and rssi_delta from
                     phone group to serving system group (FEATURE_NEWSS).
10/17/01   AT      Added the signals for AOC timers and events.
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
09/14/01   HD      Added support for interface to TMC.
08/21/01   ic      Main-lined FEATURE_NEWCM
08/23/01   HD      NAS header file reorg changes.
04/27/01   HD      Added changes for WCDMA/GSM mode.
11/17/01   sh      Added 1xhdr data session handoff support
11/01/01   sh      Removed is_force_colloc from the call cmd info
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/03/01   VT      Added determining whether flash number is emergency or not.
08/21/01   ic      Main-lined FEATURE_NEWCM
03/01/01   sko     Added CM monitor data into cm_client_s_type for NEWCM.
02/28/01   HQ      Fixed compiler warnings.
02/23/01   HQ      Added CM_RUIM_STATUS_SIG.
02/07/01   RC      Added support for System Determination 2.0.
01/29/01   HQ      Added GPSONE_DBM support when NEWCM is defined.
01/23/01   ych     Merged JCDMA features.
12/15/00   HQ      Added privacy_pref field.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
07/27/00   PJ      Added support for PD commands. (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
07/18/00   JQ      Added support for end_reason
03/20/00   RY      Featurized all the cm_commit_reply_* functions with
                   FEATURE_OTASP
03/16/00   VT      Added BROADCAST SMS support
03/13/00   RY      Added OTAPA support
12/18/99   CS      Added cm_kick_dog function.
08/31/99   SH      Modified definitions to featurize UASMS
08/09/99   SH      Added SMS object
07/07/99   SH      Added CNAP feature
06/30/99   SH      Changed type of otasp_act_code to cm_activate_code_type
06/21/99   SH      added the cmd_err field for all cmd_info structures
05/31/99   SH      added CM 2.0 features
02/04/99   RC      Changes the definition of CM_OTASP_ACT_CODE_NONE and
                   CM_SRV_OPT_NONE to avoid ARM compiler confusion.
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */
#include "sys.h"       /* System/Global definition file */
#include "sys_v.h"
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "mmoc_msgr.h"
#include "rf_task_ind_umid.h"
#ifdef FEATURE_CM_LTE
#include "cm_esm.h"    /* data structures between CM and ESM */
#include "lte_cphy_rssi_msg.h"
                       /* Definition of LTE RSSI indication */
#include "lte_rrc_ext_msg.h"
                       /* Definition of LTE RRC SRV indication */

#include "lte_rrc_irat_msg.h"
                       /* Definition of LTE irat resel indication */

#endif // FEATURE_CM_LTE

#ifdef FEATURE_CM_CFM_AVAILABLE
#include "cfcm.h"
#endif

#ifdef FEATURE_TDSCDMA
#include "tds_rrc_msg.h"
#endif

#ifdef CM_GW_SUPPORTED
#include "cmwll.h"     /* Interface to Lower Layer task */
#endif /* (defined CM_GW_SUPPORTED) */

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cmxll.h"     /* Interface to UI task */
#include "db.h"        /* Interface to Database services */
#include "mc.h"        /* Interface to MC Task */
#include "mc_v.h"
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#include "cmipapp.h"   /* Interface to IP report structure */
#include "rex.h"

#include "cm_dualsim.h"
#include <string.h>
#include <stringl/stringl.h>

#if (defined(FEATURE_LTE_TO_HDR_OH) || defined(FEATURE_DOMAIN_SELECTION))
#include "emm_ext_msg.h"
#endif

#include "ui_base_msgr_msg.h"

#include <cm_msgr_msg.h>

#include <msgr.h>


/*===========================================================================
=============================================================================
========================  DEFINES ===========================================
=============================================================================
===========================================================================*/

#define CM_IS_MODE_3GPP(mode) \
   ((mode == SYS_SYS_MODE_GSM) ||(mode == SYS_SYS_MODE_WCDMA) ||(mode == SYS_SYS_MODE_LTE)||(mode == SYS_SYS_MODE_TDS))


#define CM_DEFAULT_VALUE   0

/*
** Put EXTERNAL in front of the definition of every function that is supposed
** to be externalized.
** This way each function in a .c file will end up having EXTERNAL or static
** in front of it.
** This makes it easy to verify whether we meant for this
** function to be externalized or whether we forgot to make it static
**
*/
#define EXTERNAL


#define CM_ACTIVATE_PDP_REQ  0x41
  /* SM message to indicate pdp activation request
  ** Message Types for sm to sm peer messages,
  ** defined in Table 10.4a of TS 24.008.
  */


#define CM_SM_PD             0x0A
  /* Protocol descriminator to identify GPRS session management message.
  ** Table 11.2 of spec 24.007.
  */

#define CM_LLC_SAPI_3        3
  /* Section 10.5.6.9 of spec 24.008.
  ** Identifies the service access point that is used for GPRS
  ** data transfer at LLC layer.
  */

#define CM_DEFAULT_CS_TO_VOIP_FALLBACK_TIME          10  /* seconds */
#define CM_MAX_CS_TO_VOIP_FALLBACK_TIME              120  /* seconds */
  /* At the end of this time if an e911 call is not successfully connected
  ** over a circuit switched network, the call should be attempted over VOIP
  */

#define CM_DEFAULT_VOIP_CANCEL_RETRY_TIME            10  /* seconds */
#define CM_MAX_VOIP_CANCEL_RETRY_TIME                120  /* seconds */
  /* At the end of this time if an e911 call is attempted, if this timer
  ** is not running the call is attempted on 1x first
  */

#define CM_DEFAULT_VOIP_TO_CS_FALLBACK_TIME          20  /* seconds */
  /* At the end of this time if an voice call is attempted over HDR, if this
  ** timer expires and there is no PS call in conversation state, the call
  ** shall be attempted over 1x.
  */
#define CM_INVALID_VOIP_TO_CS_FALLBACK_TIME          0xffffffff
/* in case of alternate_redial_algo voip_to_cs_fallback is not to be used in CM context */

#define CM_INVALID_WWAN_TO_WLAN_FALLBACK_TIME          0xffffffff
/* When call cannot fall on WLAN, invalid fallback timer is set. */


#define CM_LTE_SOFT_FAIL_RETRY_COUNT                 2
   /* If this count is exceeded, the VOLTE call will be attempted over CS
   ** This count keeps track of the number of LTE soft failures during a
   ** VOLTE call
   */


#define CM_LTE_EMERG_SOFT_FAIL_RETRY_COUNT           1
   /* If this count is exceeded, the emergency VOLTE call will be attempted over CS
   ** This count keeps track of the number of LTE soft failures during an
   ** emergency call over LTE IMS
   */


/* Type for init mask variable.
*/
typedef byte    cm_init_mask;
typedef word    cm_timer_event_type;
typedef uint32  cm_act_id_type;
typedef uint64  cm_disable_call_type;
typedef uint64  cm_srvcc_call_type_mask;

/* CM uses temp ascii buffers to convert dialed digits from
** bcd. Extra byte required to store '\0'
*/
#define CM_ASCII_MAX_NUMBER_CHARS   (CM_MAX_NUMBER_CHARS + 1)

typedef int  cm_sm_prot_descr_type;
  /*
  ** Describes protocol used for originating PDP context.
  ** Set to SM_PD by default.
  */

typedef byte cm_sm_trans_id_type;
  /*
  ** Type for transaction id used for originating PDP context.
  */

typedef  uint8     cm_sm_msg_type;
  /* Defines the type of sm layer message */

/*---------------------------------------------------------------------------
                        BASIC TYPES, MACROS & FUNCTIONS
---------------------------------------------------------------------------*/

/* Init mask to identify whether a specific object was ever initialized.
** During object initialization the object's init_mask parameter is set
** to a unique mask of alternating 1's and 0's (i.e. 1010...).
** Later the init_mask parameter can be checked against this value to
** confirm that the object was properly initialized.
*/

/* Init mask value.
*/
#define CM_INIT_MASK    0x66


/* Macro to mark an object as properly initialized.
*/
#define CM_INIT_MARK( mask )   ( (mask) = CM_INIT_MASK )


/* Macro to check whether an object was properly initialized.
*/
#define CM_INIT_CHECK( mask )  ( (mask) == CM_INIT_MASK )


#define CM_GET_SLOT_FROM_XXX_SUBS_SOURCE(xxx_subs_source)\
  ((xxx_subs_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)?MMGSDI_SLOT_3:\
                  ((xxx_subs_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)?MMGSDI_SLOT_2\
           :MMGSDI_SLOT_1))



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** feature mode mask
*/
typedef enum {

  FEATURE_MODE_MASK_EMPTY             = 0,

  FEATURE_MODE_MASK_NORMAL            = BM(SYS_OVERALL_FEATURE_MODE_NORMAL),

  FEATURE_MODE_MASK_SVLTE             = BM(SYS_OVERALL_FEATURE_MODE_SVLTE),

  FEATURE_MODE_MASK_MSIM              = BM(SYS_OVERALL_FEATURE_MODE_MULTISIM),

  FEATURE_MODE_MASK_SGLTE             = BM(SYS_OVERALL_FEATURE_MODE_SGLTE),

  FEATURE_MODE_MASK_SRLTE             = BM(SYS_OVERALL_FEATURE_MODE_SRLTE),

  FEATURE_MODE_MASK_AUTO              = BM(SYS_OVERALL_FEATURE_MODE_SVLTE) |
                                        BM(SYS_OVERALL_FEATURE_MODE_SRLTE) |
                                        BM(SYS_OVERALL_FEATURE_MODE_NORMAL),

  FEATURE_MODE_MASK_SGLTE_MSIM        = BM(SYS_OVERALL_FEATURE_MODE_SGLTE) |
                                        BM(SYS_OVERALL_FEATURE_MODE_MULTISIM),

  FEATURE_MODE_MASK_MSTACK_MNAS       = BM(SYS_OVERALL_FEATURE_MODE_SGLTE) |
                                        BM(SYS_OVERALL_FEATURE_MODE_MULTISIM),

  FEATURE_MODE_MASK_MSTACK_SSIM       = BM(SYS_OVERALL_FEATURE_MODE_SVLTE) |
                                        BM(SYS_OVERALL_FEATURE_MODE_SRLTE) |
                                        BM(SYS_OVERALL_FEATURE_MODE_SGLTE),

  FEATURE_MODE_MASK_MSTACK            = BM(SYS_OVERALL_FEATURE_MODE_SVLTE) |
                                        BM(SYS_OVERALL_FEATURE_MODE_SRLTE) |
                                        BM(SYS_OVERALL_FEATURE_MODE_MULTISIM) |
                                        BM(SYS_OVERALL_FEATURE_MODE_SGLTE),

  FEATURE_MODE_MASK_ALL        = 0x3fffffff,

  /** @cond
  */
  FEATURE_MODE_MASK_MAX   /* For internal use. */
  /** @endcond
  */

} cm_feature_mode_mask_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of CM feature states.
*/
typedef enum cm_feature_e {

    CM_FEATURE_NONE=-1,      /* Don't care if feature is turned ON/OFF */

    CM_FEATURE_OFF = FALSE,  /* Feature is turned ON */
    CM_FEATURE_ON  = TRUE,   /* Feature us turned OFF */

    CM_FEATURE_MAX

} cm_feature_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration to keep track of preferences for packet switched
   data call originations.
*/
typedef enum cm_ps_data_orig_prefs_e {

  CM_PS_DATA_ORIG_PREF_NONE              = -1,

  CM_PS_DATA_ORIG_PREF_CDMA_ONLY         = 0,
    /* Orig ps data calls on cdma only */

  CM_PS_DATA_ORIG_PREF_CDMA_HDR          = 1,
    /* Orig ps data calls on cdma or hdr only */

  CM_PS_DATA_ORIG_PREF_CDMA_HDR_IF_CHINA = 2,
    /* Orig ps data calls on cdma or hdr if china */

  CM_PS_DATA_ORIG_PREF_GW_ONLY           = 3,
    /* Orig ps data calls on GSM/WCDMA only */

  CM_PS_DATA_ORIG_PREF_ANY               = 4,
    /* Orig ps data calls on any preferences */

  CM_PS_DATA_ORIG_PREF_MAX

} cm_ps_data_orig_prefs_e_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of ps data state associated with data available request */
typedef enum cm_ps_data_stt_e {

  CM_PS_DATA_STT_NONE = -1,           /* For internal use */

  CM_PS_DATA_STT_PENDING,             /* Pending state */
  CM_PS_DATA_STT_SRV_ACQ,             /* Looking for full service */
  CM_PS_DATA_STT_RESUME,              /* Waiting for data resume */

  CM_PS_DATA_STT_MAX                  /* For internal use  */

} cm_ps_data_stt_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration to keep track of hold command reasons
*/
typedef enum cm_hold_cmd_reas_e {

  CM_HOLD_CMD_REAS_NONE = -1,

  CM_HOLD_CMD_REAS_GPS,
    /* Hold command till gps session end */

  CM_HOLD_CMD_REAS_MEAS,
    /* Hold command till measurement mode on response arrives from HDRCP */

  CM_HOLD_CMD_REAS_EMG,
    /* Hold command till emergency call end */

  CM_HOLD_CMD_REAS_MAX

} cm_hold_cmd_reas_e_type;


typedef enum
{
  CMMSIMC_EVENT_NONE = -1,
  CMMSIMC_EVENT_ONLINE,
  CMMSIMC_EVENT_OFFLINE,
  CMMSIMC_EVENT_SUB,
  CMMSIMC_EVENT_STANDBY_PREF_CHGD,

  CMMSIMC_EVENT_PREF_SYS_SIM1,
  CMMSIMC_EVENT_PREF_SYS_SIM2,
  CMMSIMC_EVENT_PREF_SYS_SIM3,
  CMMSIMC_EVENT_SRV_SIM1,
  CMMSIMC_EVENT_SRV_SIM2,
  CMMSIMC_EVENT_SRV_SIM3,
  CMMSIMC_EVENT_CALL_SIM1,
  CMMSIMC_EVENT_CALL_SIM2,
  CMMSIMC_EVENT_CALL_SIM3,
  CMMSIMC_EVENT_SUSPEND_RESUME,

  CMMSIMC_EVENT_ANY,
  CMMSIMC_EVENT_MAX
}cmmsimc_event_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether val is between min and max
** (not include min and max).
*/
#define BETWEEN( val, min, max ) \
/*lint -save -e641 -e568 */ ((val) > (min) && (val) < (max)) \
/*lint -restore */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether val is in the range of min to max
** (including min and max).
*/
#define INRANGE( val, min, max )  \
/*lint -save -e641 -e568 -e685 */ (((int32)val) >= ((int32)min) && ((int32)val) <= ((int32)max)) \
/*lint -restore */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* Macro to check whether a character is an ASCII alpha.
*/
#define IS_ALPHA( c ) ( ((c) >= 'a' && (c) <= 'z') || \
                        ((c) >= 'A' && (c) <= 'Z') )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to safely decrement to 0.
*/
#define CM_DEC( num )  ( (num) > 0 ? (--(num)) : (num) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to safely assign a value to a boolean variable.
*/
#define CM_BOOLEAN( val )  ( (boolean) ((val) ? TRUE : FALSE) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Return TRUE if p1 contains p2 otherwise return FALSE.
*/
#define CM_CONTAIN( p1, p2 )  \
        ( CM_BOOLEAN( (byte)p1 & (byte)p2 ) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*---------------------------------------------------------------------------
                          CIRCULAR QUEUE MACROS
---------------------------------------------------------------------------*/


/* Macros to operate on a circular queue that is implemented as a
** contiguous array of elements in memory.
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to define a circular queue of a specified type, name and size.
*/

#define CIRC_Q_DEF( type, name, size ) struct { unsigned int  head;      \
                                                unsigned int  tail;      \
                                                unsigned int  cnt;       \
                                                type *arr[size];          \
                                              } name



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to initialize a circular queue.
*/
#define CIRC_Q_INIT( q )  q.tail = q.head = q.cnt = 0;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the size of a circular queue.
*/
#define CIRC_Q_SIZE( q ) ARR_SIZE( q.arr )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the count (i.e. the number of elements) of a circular
** queue.
*/
#define CIRC_Q_CNT( q ) ( q.cnt )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the next index of a circular queue relatively
** to a given index.
*/
#define CIRC_Q_NEXT( q_size, q_pos ) ((q_pos)+1 >= (q_size) ? 0: ((q_pos)+1))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to increment an index (i.e. head or tail) of a circular queue.
*/
#define CIRC_Q_INC( q_size, q_pos )  ( q_pos = CIRC_Q_NEXT( q_size, q_pos ) )



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to enqueue an item onto a circular queue.
** Return TRUE if item is successfully enqueued, else return FALSE.
*/
#define CIRC_Q_PUT( q, item )                                               \
 ( q.cnt >= CIRC_Q_SIZE(q) ? FALSE : ( q.arr[q.tail] = item,                \
                                       CIRC_Q_INC( CIRC_Q_SIZE(q), q.tail), \
                                       q.cnt++,                             \
                                       TRUE ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to dequeue an item from a circular queue.
** Return TRUE if item is successfully dequeued, else return FALSE.
*/
#define CIRC_Q_GET( q, item )                                               \
    ( q.cnt == 0 ? FALSE : ( item = q.arr[q.head],                          \
                             CIRC_Q_INC( CIRC_Q_SIZE(q), q.head ),          \
                             q.cnt--,                                       \
                             TRUE ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to the item at the head of a circular
** queue. If queue is empty a NULL is returned.
*/

#define CIRC_Q_CHECK( q ) ( q.cnt == 0 ? NULL : q.arr[q.head] )

/*---------------------------------------------------------------------------
                       PHONE-BOOK INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/



/*===========================================================================

FUNCTION cm_nc_resolve_call_type

DESCRIPTION
  Resolve originating number type.

  This functions consults the number-classification to whether the originating number
  matches one of the special numbers that are stored in the phone-book
  (i.e. E911, OTASP, etc.). If a match is found, it modifies the originating
  call type accordingly.

DEPENDENCIES
  number-classification interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  none

SIDE EFFECTS
  Modifies the originating call type (pointed by activate_code_ptr) and or
  the activation code (pointed by activate_code_ptr).

===========================================================================*/
extern void cm_nc_resolve_call_type(

    const cm_num_s_type            *orig_num_ptr,
        /* pointer to originating number */

    cm_call_type_e_type      *call_type_ptr,
        /* pointer to associated call type */

    cm_activate_code_e_type    *activate_code_ptr
        /* pointer to associated OTASP activation code
        ** (for OTASP call types only) */
);


/*===========================================================================

FUNCTION cm_nc_is_call_allowed_func

DESCRIPTION
  Decide whether an originating number is allowed.

  This functions consults number-classification to whether the originating number
  is allowed by the origination restriction currently in effect.

DEPENDENCIES
  number-classification interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  TRUE of originating number is allowed for origination, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_nc_is_call_allowed_func(

    const cm_num_s_type            *orig_num_ptr
        /* pointer to originating number */
);



/*===========================================================================

FUNCTION cm_nc_call_control_processing

DESCRIPTION
  Perform call type resolution and consults number-classification to whether
  the originating number is allowed and do any required call control.

DEPENDENCIES
  number-classification interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  FALSE if CM cmd buffer allocation fails, TRUE otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_nc_call_control_processing(

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
    /* Call Control parameters for GSM/WCDMA packet switched call */

  const cm_cc_generic_params_s_type     *cc_generic_params_ptr
    /* Call-control generic parameters */

);


/*===========================================================================

FUNCTION cm_nc_proc_call_control_result

DESCRIPTION
  Perform a resolution of which mode is preferred after call control.

DEPENDENCIES
  number-classification interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  TRUE of originating number is allowed for origination, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_cc_mode_pref_e_type cm_nc_proc_call_control_result(

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
);


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
extern void cm_nc_pre_pending_dial_func(

    cm_num_s_type                  *orig_num_ptr,
        /* dialed number */

    const cm_call_type_e_type      *call_type_ptr
        /* Associated call type */

);
#endif /* FEATURE_JCDMA */


extern boolean cm_sups_cmd_process_uss_internal (

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

);



extern  boolean cm_sups_cmd_register_internal (

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
);
/*===========================================================================
=============================================================================
=========================== CLIENT INTERFACE BLOCK ==========================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                                  CM COMMAND
---------------------------------------------------------------------------*/


/* Enum of CM command types.

   Whenever you modify this enumeration,
   make sure that you modify cm_cmd_type_e_type_string_map
   in cmdbg.c accordingly.

*/
typedef enum cm_cmd_type_e {

    CM_CMD_TYPE_NONE = -1,

    CM_CMD_TYPE_CALL,         /* Client call command */
    CM_CMD_TYPE_PH,           /* Client phone command */
    CM_CMD_TYPE_CLIENT,       /* Client command */
    CM_CMD_TYPE_SS,           /* Client serving system command */
    CM_CMD_TYPE_INBAND,       /* Client inband command */
    CM_CMD_TYPE_SMS,          /* Client SMS command */
    CM_CMD_TYPE_MC_RPT,       /* MC report */
    CM_CMD_TYPE_RPT,          /* Lower Layer report */

    CM_CMD_TYPE_SD_RPT,       /* SD report */

    #ifdef FEATURE_GPSONE
    CM_CMD_TYPE_PD,           /* Position determination command */
    #endif /* FEATURE_GPSONE */

    CM_CMD_TYPE_DBM,           /* Data Burst Message command */

    #if defined (CM_GW_SUPPORTED) || defined (FEATURE_IP_CALL)
    CM_CMD_TYPE_SUPS,          /* Client sups command */
    #endif

    #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
    CM_CMD_TYPE_BCMCS,        /* Client BCMCS command */
    #endif /* FEATURE_BCMCS || MBMS */

    #ifdef CM_GW_SUPPORTED
    CM_CMD_TYPE_NAS,          /* Client NAS command */
    #endif   /* FEATURE_WCDMA || FEATURE_GSM */

    CM_CMD_TYPE_IP_RPT,       /* Report queued from IP Apps */

    CM_CMD_TYPE_MBMS,         /* Client MBMS Command*/

    CM_CMD_TYPE_ESM_RPT,      /* Report sent from ESM through
                              ** Message Router
                              */

    CM_CMD_TYPE_STATS,        /* Modem Statistics command */

    CM_CMD_TYPE_AC,           /* Commands for access control */

    CM_CMD_TYPE_MAX

} cm_cmd_type_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


typedef enum cm_apps_pwr_status_e{

  CM_APPS_PWR_STATUS_NONE = -1,     /* Internal for CM */

  CM_APPS_PWR_STATUS_PWR_COLLAPSE,  /* The apps processor is power collapsed */

  CM_APPS_PWR_STATUS_PWR_RESTORE,   /* The apps processor is NOT power collapsed */

  CM_APPS_PWR_STATUS_PWR_SUSPEND,   /* The apps processor is in standby mode */

  CM_APPS_PWR_STATUS_MAX            /* Internal for CM */

}cm_apps_pwr_status_e_type;

/** Enumeration of the reason for loss of subscription.
*/
typedef enum cm_subscription_not_avail_cause_e {

/** @cond
*/
  CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_ILLEGAL_SUB,
    /**< Subscription loss is due to illegal sim. */

  CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_OTHERS,
    /**< Subscription loss is due to other reasons. */

/** @cond
*/
  CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_subscription_not_avail_cause_e_type;
/*~ SENTINEL cm_subscription_not_avail_cause_e_type.CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_MAX */

/* Type for Dynamic Environment Manager object
*/
typedef struct cm_dem_s{

  cm_apps_pwr_status_e_type      apps_pwr_status;

}cm_dem_s_type;

/* Define values indicating empty (or inactive) command fields,
*/
#define CM_SRV_OPT_NONE              ( (word) (0xFFFF) )
#define CM_NUMBER_LEN_NONE           ( 0 )
#define CM_ALPHA_LEN_NONE            ( 0 )
#define CM_ALPHA_CODING_SCHEME_NONE  ( 0 )
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
/* display type for CDMA extended display info record */
#define CM_EXT_DISP_TYPE_NONE  ( (byte) 0xFF )
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
#define CM_CLIENT_PTR_NONE           ( 0 )


/** Maximum number of PDP contexts
*/
#ifdef FEATURE_MULTIPLE_PRIMARY_PDP
#define TOTAL_PRIMARY_PDP_CALL_OBJ            SYS_MAX_PRIMARY_PDP_CONTEXTS
#else
#define TOTAL_PRIMARY_PDP_CALL_OBJ            SYS_MIN_PRIMARY_PDP_CONTEXTS
#endif

#define TOTAL_SEC_PDP_CALL_OBJ         SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY

#define TOTAL_CALL_ID(x,y)    (6 +((x)+((x) * (y))))

/* Maximum number of Call objects allowed
*/
#define CMCALL_MAX_CALL_OBJ  TOTAL_CALL_ID \
       ( TOTAL_PRIMARY_PDP_CALL_OBJ , TOTAL_SEC_PDP_CALL_OBJ )

/*   CM_MT_UNALLOCATED_CALL_ID will be used for MT calls in 1xsrlte-svlte mode  
  ** will be used to determine if the call was MT in cmcsfbcall_rpt_proc 
  ** we needed this since we didnt have call obj when  1x sends CM_INCOMING_PAGE_F */
#define CM_MT_UNALLOCATED_CALL_ID (CMCALL_MAX_CALL_OBJ + 1)

/*   below call-id's 
  ** will be used by cmcsfbcall_rpt_proc  ,to determine if the ESR request was due to 3rd party E911 call .
  ** we needed this since we didnt have call obj when  3rd party E911 call is originated */
#define CM_UNALLOCATED_CALL_ID_CMEMG_SUB1 (CMCALL_MAX_CALL_OBJ + 2)
#define CM_UNALLOCATED_CALL_ID_CMEMG_SUB2 (CMCALL_MAX_CALL_OBJ + 3)


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for bit array of arbitrary size.
** Being used to indicate for which call and phone events
** a client is registered.
*/
typedef struct bit_arr_s {

    byte    *bit_buf;
        /* Pointer to bit buffer */

    int     bit_size;
        /* Bit size of the buffer pointed by bit_buf */

} bit_arr_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for CM client.
**
** Client structures keep a record of all the CM events for which their
** respective client is registered and thus call on their client callback
** functions as needed.
**
** Each CM client needs to allocate such a CM client structure in its own
** program space.
**
** Clients register and/or de-register for CM events dynamically via methods
** that operate on the CM client object that is allocated in their own
** program space.
**
** NOTE THAT NONE OF THE CM CLIENT STRUCTURE FIELDS SHOULD BE MODIFIED OR
** ACCESSED DIRECTLY BY THE ITS CLIENT. ANY INITIALIZATION OR MANIPULATION
** OF THE CM CLIENT STRUCTURE IS TO BE DONE VIA THE CM CLIENT METHODS
** SUPPLIED IN THIS MODULE.
*/
typedef struct cm_client_s {


    byte                      init_mask;
        /* Indicate whether this object was properly initialized */

    cm_client_id_type         client_id;
        /* Client ID - each initialized client structure has
        ** a unique client ID */

    cm_processor_e_type       client_processor;
        /* The processor on which the client is executing
        */

    byte                      call_bit_buf[1 + (int)CM_CALL_EVENT_MAX/8];
    bit_arr_s_type            call_bit_arr;
        /* Bit array indicating to which CALL events this
        ** client is registered */

    byte                      ph_bit_buf[1 + (int)CM_PH_EVENT_MAX/8];
    bit_arr_s_type            ph_bit_arr;
        /* Bit array indicating to which PHONE events this
        ** client is registered */

    uint8                     asubs_mask ;

    byte                      ss_bit_buf[1 + (int)CM_SS_EVENT_MAX/8];
    bit_arr_s_type            ss_bit_arr;
        /* Bit array indicating to which SERVING SYSTEM events this
        ** client is registered */

    byte                      inband_bit_buf[1 + (int)CM_INBAND_EVENT_MAX/8];
    bit_arr_s_type            inband_bit_arr;
        /* Bit array indicating to which inband events this
        ** client is registered */

    byte                      sms_bit_buf[1 + (int)CM_SMS_EVENT_MAX/8];
    bit_arr_s_type            sms_bit_arr;
       /* Bit array indicating to which SMS events this
       ** client is registered */

    #ifdef FEATURE_GPSONE
    byte                      pd_bit_buf[1 + (int)CM_PD_EVENT_MAX/8];
    bit_arr_s_type            pd_bit_arr;
        /* Bit array indicating to which PD events this
        ** client is registered */

    #endif /* FEATURE_GPSONE */

    byte                      dbm_event_bit_buf[1 + (int)CM_DBM_EVENT_MAX/8];
    bit_arr_s_type            dbm_event_bit_arr;
        /* Bit array indicating to which DBM events this
        ** client is registered */

    byte                      dbm_class_bit_buf[1 + (int)CM_DBM_CLASS_MAX/8];
    bit_arr_s_type            dbm_class_bit_arr;
        /* Bit array indicating to which DBM class this
        ** client is registered */

    #if defined CM_GW_SUPPORTED || defined FEATURE_IP_CALL
    byte                      sups_bit_buf[1 + (int)CM_SUPS_EVENT_MAX/8];
    bit_arr_s_type            sups_bit_arr;
       /* Bit array indicating to which SUPS events this
       ** client is registered */
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    byte                      ac_bit_buf[1 + (int)CM_AC_EVENT_MAX/8];
    bit_arr_s_type            ac_bit_arr;

    #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
    byte                      bcmcs_bit_buf[1 + (int)CM_BCMCS_EVENT_MAX/8];
    bit_arr_s_type            bcmcs_bit_arr;
        /* Bit array indicating to which BCMCS events this
        ** client is registered */

    byte                      mbms_bit_buf[1 + (int)CM_MBMS_EVENT_MAX/8];
    bit_arr_s_type            mbms_bit_arr;
        /* Bit array indicating to which MBMS events this
        ** client is registered */

    #endif /* FEATURE_BCMCS || MBMS */

    #ifdef CM_GW_SUPPORTED
    byte                      nas_bit_buf[1 + (int)CM_NAS_EVENT_MAX/8];
    bit_arr_s_type            nas_bit_arr;
       /* Bit array indicating to which NAS events this
       ** client is registered */
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #ifdef FEATURE_APPS_POWER_COLLAPSE
    cm_pwr_collapse_notify_e_type call_power_collpase_notify_pref;
    byte                      call_power_collapse_bit_buf[1 + (int)CM_CALL_EVENT_MAX/8];
    bit_arr_s_type            call_power_collapse_bit_arr;
        /* Bit array indicating which CALL events are to be ignored while
        ** in power collapse state
        */
    byte                      call_power_collapse_buf_bit_buf[1 + (int)CM_CALL_EVENT_MAX/8];
    bit_arr_s_type            call_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored CALL events are to be buffered
        ** while in power collapse state
        */

    cm_pwr_collapse_notify_e_type ph_power_collpase_notify_pref;
    byte                      ph_power_collapse_bit_buf[1 + (int)CM_PH_EVENT_MAX/8];
    bit_arr_s_type            ph_power_collapse_bit_arr;
        /* Bit array indicating which PHONE events are to be ignored while in
        ** power collapse state
        */
    byte                      ph_power_collapse_buf_bit_buf[1 + (int)CM_PH_EVENT_MAX/8];
    bit_arr_s_type            ph_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored PHONE events are to be buffered
        ** while in power collapse state
        */

    cm_pwr_collapse_notify_e_type ss_power_collpase_notify_pref;
    byte                      ss_power_collapse_bit_buf[1 + (int)CM_SS_EVENT_MAX/8];
    bit_arr_s_type            ss_power_collapse_bit_arr;
        /* Bit array indicating which SERVING SYSTEM events are to be ignored
        ** while in power collapse
        */
    byte                      ss_power_collapse_buf_bit_buf[1 + (int)CM_SS_EVENT_MAX/8];
    bit_arr_s_type            ss_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored SERVING SYSTEM events are to be buffered
        ** while in power collapse state
        */

    cm_pwr_collapse_notify_e_type inband_power_collpase_notify_pref;
    byte                      inband_power_collapse_bit_buf[1 + (int)CM_INBAND_EVENT_MAX/8];
    bit_arr_s_type            inband_power_collapse_bit_arr;
        /* Bit array indicating which inband events are to be ignored while
        ** in power collapse
        */
    byte                      inband_power_collapse_buf_bit_buf[1 + (int)CM_INBAND_EVENT_MAX/8];
    bit_arr_s_type            inband_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored INBAND events are to be buffered
        ** while in power collapse state
        */

    cm_pwr_collapse_notify_e_type sms_power_collpase_notify_pref;
    byte                      sms_power_collapse_bit_buf[1 + (int)CM_SMS_EVENT_MAX/8];
    bit_arr_s_type            sms_power_collapse_bit_arr;
       /* Bit array indicating which SMS events are to be ignored while in
       ** power collapse state
       */
    byte                      sms_power_collapse_buf_bit_buf[1 + (int)CM_SMS_EVENT_MAX/8];
    bit_arr_s_type            sms_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored SMS events are to be buffered
        ** while in power collapse state
        */

    #ifdef FEATURE_GPSONE
    cm_pwr_collapse_notify_e_type pd_power_collpase_notify_pref;
    byte                      pd_power_collapse_bit_buf[1 + (int)CM_PD_EVENT_MAX/8];
    bit_arr_s_type            pd_power_collapse_bit_arr;
        /* Bit array indicating which PD events are to be ignored while in
        ** power collapse state
        */
    byte                      pd_power_collapse_buf_bit_buf[1 + (int)CM_PD_EVENT_MAX/8];
    bit_arr_s_type            pd_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored PD events are to be buffered
        ** while in power collapse state
        */
    #endif /* FEATURE_GPSONE */

    cm_pwr_collapse_notify_e_type dbm_power_collpase_notify_pref;
    byte                      dbm_power_collapse_bit_buf[1 + (int)CM_DBM_EVENT_MAX/8];
    bit_arr_s_type            dbm_power_collapse_bit_arr;
        /* Bit array indicating which DBM events are to be ignored while in
        ** power collapse state
        */
    byte                      dbm_power_collapse_buf_bit_buf[1 + (int)CM_DBM_EVENT_MAX/8];
    bit_arr_s_type            dbm_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored DBM events are to be buffered
        ** while in power collapse state
        */

    #ifdef CM_GW_SUPPORTED
    cm_pwr_collapse_notify_e_type sups_power_collpase_notify_pref;
    byte                      sups_power_collapse_bit_buf[1 + (int)CM_SUPS_EVENT_MAX/8];
    bit_arr_s_type            sups_power_collapse_bit_arr;
       /* Bit array indicating which SUPS events are to be ignored while in
       ** power collapse state
       */
    byte                      sups_power_collapse_buf_bit_buf[1 + (int)CM_SUPS_EVENT_MAX/8];
    bit_arr_s_type            sups_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored SUPS events are to be buffered
        ** while in power collapse state
        */
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
    cm_pwr_collapse_notify_e_type bcmcs_power_collpase_notify_pref;
    byte                      bcmcs_power_collapse_bit_buf[1 + (int)CM_BCMCS_EVENT_MAX/8];
    bit_arr_s_type            bcmcs_power_collapse_bit_arr;
        /* Bit array indicating which BCMCS events are to be ignored while in
        ** power collapse
        */
    byte                      bcmcs_power_collapse_buf_bit_buf[1 + (int)CM_BCMCS_EVENT_MAX/8];
    bit_arr_s_type            bcmcs_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored BCMCS events are to be buffered
        ** while in power collapse state
        */

    cm_pwr_collapse_notify_e_type mbms_power_collpase_notify_pref;
    byte                      mbms_power_collapse_bit_buf[1 + (int)CM_MBMS_EVENT_MAX/8];
    bit_arr_s_type            mbms_power_collapse_bit_arr;
        /* Bit array indicating which MBMS events are to be ignored while in
        ** power collapse
        */
    byte                      mbms_power_collapse_buf_bit_buf[1 + (int)CM_MBMS_EVENT_MAX/8];
    bit_arr_s_type            mbms_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored MBMS events are to be buffered
        ** while in power collapse state
        */
    #endif /* FEATURE_BCMCS || MBMS */

    #ifdef CM_GW_SUPPORTED
    cm_pwr_collapse_notify_e_type nas_power_collpase_notify_pref;
    byte                      nas_power_collapse_bit_buf[1 + (int)CM_NAS_EVENT_MAX/8];
    bit_arr_s_type            nas_power_collapse_bit_arr;
       /* Bit array indicating which NAS events are to be ignored while in
       ** power collapse
       */
    byte                      nas_power_collapse_buf_bit_buf[1 + (int)CM_NAS_EVENT_MAX/8];
    bit_arr_s_type            nas_power_collapse_buf_bit_arr;
        /* Bit array indicating which ignored NAS events are to be buffered
        ** while in power collapse state
        */
    #endif /* FEATURE_WCDMA || FEATURE_GSM */
    #endif /* FEATURE_APPS_POWER_COLLAPSE */

    byte                      stats_bit_buf[1 + (int)CM_STATS_EVENT_MAX/8];
    bit_arr_s_type            stats_bit_arr;
        /* Bit array indicating to which STATISTICS events this
        ** client is registered */

    cm_mm_call_event_f_type   *call_event_func;
        /* Function pointer to notify client of call events */

    cm_ph_event_f_type        *ph_event_func;
        /* Function pointer to notify client of phone events */

    cm_subs_event_f_type      *subs_event_func;
        /* Function pointer to notify client of phone events */

    cm_mm_msim_ss_event_f_type *msim_ss_event_func;
        /* Function pointer to notify client of serving system events */

    cm_mm_ss_event_f_type     *ss_event_func;
        /* Function pointer to notify client of serving system events */

    cm_inband_event_f_type    *inband_event_func;
        /* Function pointer to notify client of inband events */

    cm_sms_event_f_type       *sms_event_func;
        /* Function pointer to notify client of SMS events */

    cm_call_info_list_f_type  *call_info_list_func;
        /* Function pointer to notify client of call information list */

    #ifdef FEATURE_GPSONE
    cm_pd_event_f_type        *pd_event_func;
        /* Function pointer to notify client of PD events */

    #endif /* FEATURE_GPSONE */

    cm_dbm_event_f_type       *dbm_event_func;
        /* Function pointer to notify client of DBM events */

    #if defined CM_GW_SUPPORTED || defined FEATURE_IP_CALL
    cm_sups_event_f_type      *sups_event_func;
        /* Function pointer to notify client of SUPS events */

    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    cm_ac_event_f_type      *ac_event_func;

    #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
    cm_bcmcs_event_f_type     *bcmcs_event_func;
        /* Function pointer to notify client of BCMCS events */

    #endif /* FEATURE_BCMCS || MBMS */

    #ifdef CM_GW_SUPPORTED
    cm_nas_event_f_type      *nas_event_func;
        /* Function pointer to notify client of NAS events */

    cm_mbms_event_f_type     *mbms_event_func;
        /* Function pointer to notify client of BCMCS events */

    cm_mbms_event_f_type     *power_collapse_mbms_event_func;
        /* Function pointer to notify client of BCMCS events */

    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #ifdef FEATURE_APPS_POWER_COLLAPSE
    cm_mm_call_event_f_type   *power_collapse_call_event_func;
        /* Function pointer to notify client of call events */

    cm_ph_event_f_type        *power_collapse_ph_event_func;
        /* Function pointer to notify client of phone events */

    cm_mm_ss_event_f_type     *power_collapse_ss_event_func;
        /* Function pointer to notify client of serving system events */

    cm_mm_msim_ss_event_f_type     *power_collapse_ss_event_func_msim;
        /* Function pointer to notify client of serving system events */

    cm_inband_event_f_type    *power_collapse_inband_event_func;
        /* Function pointer to notify client of inband events */

    cm_sms_event_f_type       *power_collapse_sms_event_func;
        /* Function pointer to notify client of SMS events */

    cm_nas_event_f_type       *power_collapse_nas_event_func;
        /* Function pointer to notify client of nas events */

    #ifdef FEATURE_GPSONE
    cm_pd_event_f_type        *power_collapse_pd_event_func;
        /* Function pointer to notify client of PD events */

    #endif /* FEATURE_GPSONE */

    cm_dbm_event_f_type       *power_collapse_dbm_event_func;
        /* Function pointer to notify client of DBM events */

    #ifdef CM_GW_SUPPORTED
    cm_sups_event_f_type      *power_collapse_sups_event_func;
        /* Function pointer to notify client of SUPS events */

    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
    cm_bcmcs_event_f_type     *power_collapse_bcmcs_event_func;
        /* Function pointer to notify client of BCMCS events */
    #endif /* FEATURE_BCMCS || MBMS */
    #endif /* FEATURE_APPS_POWER_COLLAPSE */

    cm_stats_event_f_type     *stats_event_func;
        /* Function pointer to notify client of Statistics events */

    cm_mm_call_cmd_err_f_type *call_cmd_err_func;
        /* Function to notify client of call command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients - e.g. data services failed to
        ** originate because phone is offline */

    cm_ph_cmd_err_f_type      *ph_cmd_err_func;
        /* Function to notify the client of phone command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_ss_cmd_err_f_type      *ss_cmd_err_func;
        /* Function to notify the client of serving command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_mm_call_cmd_err_f_type      *call_info_list_cmd_err_func;
        /* Function to notify the client of serving command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_mm_inband_cmd_err_f_type  *inband_cmd_err_func;
        /* Function to notify the client of inband command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_sms_cmd_err_f_type     *sms_cmd_err_func;
        /* Function to notify the client of SMS command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    #ifdef FEATURE_GPSONE
    cm_pd_cmd_err_f_type      *pd_cmd_err_func;
        /* Function to notify the client of PD command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */
    #endif /* FEATURE_GPSONE */

    cm_dbm_cmd_err_f_type     *dbm_cmd_err_func;
        /* Function to notify the client of PD command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    #ifdef CM_GW_SUPPORTED
    cm_sups_cmd_err_f_type    *sups_cmd_err_func;
        /* Function to notify the client of SUPS command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    cm_ac_cmd_err_f_type    *ac_cmd_err_func;

    #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
    cm_bcmcs_cmd_err_f_type    *bcmcs_cmd_err_func;
        /* Function to notify the client of BCMCS command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI-like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */
    #endif /* FEATURE_BCMCS || MBMS */

    #ifdef CM_GW_SUPPORTED
    cm_nas_cmd_err_f_type    *nas_cmd_err_func;
        /* Function to notify the client of NAS command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_mbms_cmd_err_f_type   *mbms_cmd_err_func;
      /* Function to notify the client of MBMS command errors of OTHER
      ** clients.
      ** NOTE that typically only a UI like client registers for such a
      ** service since it needs to display messages for command errors
      ** on behalf of other clients */

    #endif /* FEATURE_WCDMA || FEATURE_GSM */

   cm_stats_cmd_err_f_type   *stats_cmd_err_func;
      /* Function to notify the client of STATISTICS command errors of OTHER
      ** clients.
      ** NOTE that typically only a UI like client registers for such a
      ** service since it needs to display messages for command errors
      ** on behalf of other clients */


} cm_client_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold call commands information.
*/

/* lower layer task that provides the connection ID
*/
typedef enum cm_conn_id_src_e
{
  CONN_ID_UNKNOWN_SRC = -1,
  CONN_ID_MN,
  CONN_ID_SM
  #ifdef FEATURE_CM_LTE
  ,
  CONN_ID_ESM
  #endif
}cm_conn_id_src_e_type;

#if (defined CM_GW_SUPPORTED ||defined(FEATURE_CM_LTE))
typedef struct
{
  boolean                 in_use;
  cm_call_type_e_type     call_type;
  byte                    connection_id;
  cm_conn_id_src_e_type   conn_id_src;
} call_id_info;

typedef struct cm_call_cmd_info_gw_cs_s
{
  boolean                             accept;

  boolean                             success;

  boolean                             complete;

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

  cm_forward_cug_info_s_type          forward_cug_info;

  byte                                cm_clir;

  ie_cm_cc_cause_s_type               cc_cause;

  cm_call_sups_params_s_type          sups_cmd_params;

#ifdef FEATURE_GSTK
  boolean                             call_control_completed;
#endif

  /* UMTS -> 1x handoff status.
  */
  sys_umts_cdma_handover_e_type       umts_cdma_handover_status;

  #ifdef FEATURE_UUS
  cm_user_data_s_type                 mo_user_data;
    /* user to user data request structure
    */
  #endif

  cm_ccbs_recall_rsp_e_type           ccbs_recall_rsp;
    /*
    ** Client's response to recall indication from network
    */

  cm_call_type_e_type                 new_call_type_req;
    /*
    ** New call type requested. Can be due to local
    ** MO in call modif or remote MT in call modif. call_type carries
    ** the current type of call and new_call_type_req is the requested
    ** new type for the call.
    */

  boolean                             gw_cs_modify_res;
    /* Client response to in-call modification request
    ** TRUE   - Modification got accepted by client
    ** FALSE  - Modification got rejected by client
    */


} cm_call_cmd_info_gw_cs_s_type;


typedef struct cm_call_cmd_info_gw_ps_s
{
  boolean                             accept;
  boolean                             primary_pdp;
     /* True=Secondary False=Primary */
  qos_T                               qos;
  qos_T                               minimum_qos;
  pdp_address_T                       pdp_addr;
  apn_T                               apn_name;
  protocol_cfg_options_T              config_options;
  sys_pdp_header_comp_e_type          h_comp;
  sys_pdp_data_comp_e_type            d_comp;
  tft_type_T                          tft;
  ti_T                                tear_down_ind;
  sm_status_T                         cause;
  rabm_rabid_T                        rabid;

  byte                                pdp_group_id;
    /*
    ** Group id of the primary context
    ** that is associated with the
    ** secondary PDP context origination
    */

  uint16                                assoc_pri_profile_num;
   /*
   ** Valid for only Sec PDP context
   ** Sec PDP origination contains requested qos and
   ** minimum_qos.
   ** FALSE - Nw has allocated qos in between requested and minimum_qos
   ** TRUE  - Nw has allocated different qos
   */


  uint16                              profile_number;

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

  nsapi_s_type                nsapi;
    /* Currently used by call control interface only, not
    ** intended for call origination interface.
    **
    ** Logical link controller's service access point requried for this
    ** origination. Netowrk service access point.
    */

  pdp_request_type_T         request_type;
  /** indicates PDP  Activate request type like Handover,Emergency
      should be read only if valid field is set.  
  */
} cm_call_cmd_info_gw_ps_s_type;
#endif /* defined CM_GW_SUPPORTED  ||defined(FEATURE_CM_LTE)*/

#ifdef FEATURE_CM_LTE
typedef cm_lte_call_info_s_type cm_call_cmd_info_lte_s_type;
#endif

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
typedef struct cm_call_cmd_info_cdma_s
{

  word                       srv_opt;
    /* service option */

  cm_activate_code_e_type    otasp_act_code;
    /* OTASP call activation code */

  #ifdef FEATURE_IS2000

  boolean                    drs_bit;
    /* set the DRS bit or not */
  #endif /* FEATURE_IS2000 */

  cm_privacy_pref_e_type     privacy_pref;

  #ifdef FEATURE_HDR_HANDOFF
  sys_sys_mode_e_type         last_act_data_net;
    /* last active data network */
  #endif /* FEATURE_HDR_HANDOFF */

  #ifdef FEATURE_IS2000_REL_A
  boolean                    sr_id_included;
    /* sr_id indicator */

  byte                       sr_id;
    /* sr_id */

  boolean                    qos_parms_incl;
    /* Qos Parameters indicator */

  byte                       qos_parms_len;
    /* Length of QOS parameters */

  byte                       qos_parms[CM_ORIG_QOS_LEN_MAX];
    /* QOS parameters */

  #endif /* FEATURE_IS2000_REL_A */

  cm_call_sups_type_e_type   flash_type;
    /* type of flash */

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
  #error code not present
#endif

  boolean                     is_page_proceed;
    /* DS will indicate whether to proceed with incoming 1x page
    */

  boolean                     is_special_srv_data_call;
  /* Special data call to clean up existing data session
  */

} cm_call_cmd_info_cdma_s_type;
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

typedef struct cm_call_cmd_info_ip_s
{
    boolean                   accept;
      /* Used for sending setup response for MT calls
      */

    cm_ip_call_attrib_info_s_type  ipcall_attrib_info;
      /* Audio/Video call attribute info */

    cm_call_type_e_type            new_call_type_req;
      /*
      ** New call type requested to be modified. Can be due to local
      ** MO in call modif or remote MT in call modif. call_type carries
      ** the current type of call and new_call_type_req is the requested
      ** new type for the call.
      */
     boolean                       modify_response;

     cm_npi_type                   pi;
     /* Presentation indicator
     */

     cm_ip_ext_prim_sups_s_type   ip_sups_param;
      /* IP extended parameters for sups command */

     cm_call_ip_ect_params_s_type ip_sups_ect_params;
       /* IP extended parameters for IP ECT sups command */

     cm_call_end_e_type            call_reject_reason;
      /* ip call answer rejected by client, checked if accept is false */

     cm_ip_call_sip_reject_code_s_type       sip_reject_code;
      /*  SIP code sent for rejecting incoming call */      

}cm_call_cmd_info_ip_s_type;



typedef struct cm_call_cmd_info_s
{
  /*
  ** Mode independent information.
  */
  cm_call_cmd_err_e_type         cmd_err;
      /* error in command request */

  cm_call_id_type                call_id;
      /* call ID of the call */

  cm_call_id_type                switch_out_call_id;
      /* call ID of the call */

  cm_call_type_e_type            call_type;
      /* Call type */

  cm_srv_type_e_type             srv_type;
      /* Service type - AMPS, CDMA, etc. */


  cm_num_s_type                  num;
      /* Number buffer:
      ** Originated calls - contains dialed address.
      ** Flash with info  - contains dialed address. */

  cm_num_s_type                  calling_num;
      /* Mobile number information */

  cm_alpha_s_type                alpha;
      /* Client specified alpha tag to be displayed along
      ** with dialed address of an originated call.
      ** NOTE! THIS IS NOT A PHONE BOOK NAME MATCH */

  uint8                          num_end_call_ids;
      /* Number of calls to be ended */

  cm_end_params_s_type           end_params[CM_CALL_ID_MAX];
      /* Call id's and end parameters of all calls to be ended */

  cm_call_end_e_type             end_status;
      /* Call end status (i.e. reason for ending a call in origination,
      ** incoming or conversation state) */

  cm_mode_pref_e_type        favored_mode_pref;
    /* Call mode preference */

  cm_hybr_pref_e_type       favored_hybr_pref;
    /* favored hybrid preference */

  /*
  ** Mode dependent information.
  */
  cm_call_mode_info_e_type       info_type;
    /* Current mode */

  boolean   is_gsm_wcdma_cs_info_present;
    /* TRUE if GW cs info is present */

  boolean   is_gsm_wcdma_ps_info_present;
    /* TRUE if GW ps info is present */

  #if (defined CM_GW_SUPPORTED || defined(FEATURE_CM_LTE))
  union
  {
    cm_call_cmd_info_gw_cs_s_type  gsm_wcdma_info;
    /* GW mode specific circuit-switched information */

    cm_call_cmd_info_gw_ps_s_type  gsm_wcdma_ps_info;
    /* GW mode specific packet-switched information */
  } cm_gw_mode_info_u;

  #endif /* (defined CM_GW_SUPPORTED || defined(FEATURE_CM_LTE)) */

  boolean                  call_control_processing_complete;
  cm_call_cmd_err_e_type   call_control_cmd_err;
    /* Variables to communicate results of call control. */

  boolean   is_cdma_info_present;
    /* TRUE if CDMA info is present */

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cm_call_cmd_info_cdma_s_type   cdma_info;
    /* CDMA mode specific information */
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

  boolean                          is_ip_call_info_present;
    /* Not being set anywhere right now
    */
  boolean                          data_suspend;
   /* If data needs to be suspended or resumed
   */
  cm_call_cmd_info_ip_s_type  ip_call;
    /* Structure carries specific parameters for ipcall cmd.
    */

  cm_call_sups_type_e_type            sups_cmd_type;
    /* Sups command type */

  cm_cc_generic_params_s_type         cc_generic_params;
    /* Call-control generic parameters */

  #ifdef FEATURE_CM_LTE
  cm_call_cmd_info_lte_s_type         lte_info;
    /* LTE call command info */
  #endif
  cm_call_cmd_err_cause_e_type        error_cause;
    /* Cause of call cmd error */

  cm_blocked_apn_list_s_type          blocked_apn_list;
    /* apn blocked list */

  cm_call_hold_state_e_type           call_hold_state;
  /* Local call hold information */
  cm_drvcc_setup_req_s_type           drvcc_setup_info;
  /* DRVCC setup request parameters */  

  cm_call_secure_context_e_type       secure_context_info;
  /* Secure context info for ongoing call */  

} cm_call_cmd_info_s_type;

/* CM Command common fields - all cmds to CM must start with this
*/
#define CM_CMD_COMMON_FIELDS            \
  q_link_type           link;  /* Q linking element */         \
  cm_cmd_type_e_type    cmd_type;  /* Type of CM command */     \
  boolean               is_reused ;  /* Command buffers that will be requeued can use this flag to prevent
      ** the buffer from being freed when the command is processed for the
      ** first time. This flag should be check by all the callers of
      ** cm_cmd_dealloc().
      */                                                           \
  cm_client_s_type      *client_ptr; /* Client sending this command */   \
  cm_client_id_type     client_id;    /* Client sending this command */   \
  uint32                alloc_time;   /* Uptime when this buffer was allocated */  \
  char                  task[10]     /* Name of task context which requested this buffer */

/*
** Generic command header type .
*/

typedef struct
{
  CM_CMD_COMMON_FIELDS;
} cm_cmd_type;



/* Type for voice/data call commands.
*/
typedef struct cm_call_cmd_s
{
  CM_CMD_COMMON_FIELDS;
    /* Command header */

  cm_call_cmd_exten_cb_f_type      *cmd_cc_cb_func;
    /* pointer to a client provided callback function
       when CM does call control */

  cm_call_cmd_cb_f_type          *cmd_cb_func;
    /* pointer to a client provided callback function */

  cm_call_cmd_call_id_cb_f_type  *cmd_call_id_cb_func;
    /* pointer to a client provided callback function where CM provides call ID also*/

  void                           *data_block_ptr;
    /* pointer to a client provided callback data block */

  cm_call_cmd_e_type             cmd;
    /* CM command */

  cm_call_direction_e_type       direction;
    /* Direction of the call MO/MT */

  cm_call_cmd_info_s_type        info;
    /* CM command information */

  cm_orig_client_categ_e_type    client_categ;
    /* category of initialting client User or Card */

  boolean                        is_modem_cc_api;
    /* Indicates whether new API for Centralized call control is called */

  sys_modem_as_id_e_type         cmd_subs;
    /* Subscription-id on which command is issued */

} cm_call_cmd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold phone command information.
*/
typedef struct cm_ph_cmd_info_s {

    cm_ph_cmd_err_e_type              cmd_err;
        /* error in command request */

    /*
    ** cm_ph_cmd_oprt_mode parameters
    */
    sys_oprt_mode_e_type              oprt_mode;
      /* Indicate current operating mode */

    cm_answer_voice_e_type            answer_voice;
        /* indicate whether incoming calls answered as voice,
        ** modem, fax, etc */

    dword                             answer_duration;
        /* indicate seconds duration for temp. answer-voice settings,
        such as CM_ANSWER_VOICE_AS_FAX_ONCE or DB_VOICE_AS_MODEM_ONCE */

    cm_mode_pref_e_type               mode_pref;
      /* mode preference */

    cm_pref_term_e_type               pref_term;
      /* indicate the mode term preference */

    dword                             pref_duration;
      /* indicate seconds duration for temp. mode term */

    cm_gw_acq_order_pref_e_type       acq_order_pref;
      /* gw acquisition order preference */

    cm_rat_acq_order_pref_s_type      rat_acq_order_pref;
      /* rat acuisition priority order preference */

    cm_band_pref_e_type               band_pref;
      /* band preference */

    sys_lte_band_mask_e_type          lte_band_pref;
      /* LTE band preference */

    cm_band_pref_e_type               tds_band_pref;
      /* TDS band preference */

    cm_prl_pref_e_type                prl_pref;
      /* prl preference */

    cm_roam_pref_e_type               roam_pref;
      /* roam preference */

    cm_hybr_pref_e_type               hybr_pref;
      /* hybrid preference */

    cm_srv_domain_pref_e_type         srv_domain_pref;
      /* The requested/preferred service domain.               */

    cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
      /* The network selection mode selected by the client.    */

    boolean                           net_sel_mode_pref_chgd;
      /* The network selection mode has been changed.
      ** used in sys sel pref cmd. Needs to be stored in cases where
      ** sys sel pref is processed after a wait.
      ** = TRUE if client changed the network sel mode pref */

    sys_plmn_id_s_type                plmn;
      /* The id of the system to be selected */

    /*
    ** cm_ph_cmd_nam_sel parameters
    */
    cm_nam_e_type                     nam_sel;
      /* indicate current NAM selection */

    /*
    ** cm_ph_cmd_signal_strength_delta parameters
    */
    uint8                             rssi_delta;
      /*indicate current RSSI Delta Threshold  */

    uint8                             ecio_delta;
      /*indicate current ECIO Delta Threshold  */

    uint8                             io_delta;
      /*indicate current ECIO Delta Threshold  */

    uint8                             sir_delta;
      /*indicate current SIR Delta Threshold  */

    uint8                             pathloss_delta;
      /*indicate current PATHLOSS Delta Threshold  */

    uint8                            rsrp_delta;
      /*indicate current RSRP Delta Threshold  */

    uint8                            rsrq_delta;
      /*indicate current RSRQ Delta Threshold  */


    /*
    ** cm_ph_cmd_subscription_available_new parameters
    */
    cm_subscription_status_e_type     cdma_sub_avail_status;
      /* Subscription status of CDMA system */

    cm_subscription_status_e_type     gwl_sub_avail_status;
      /* Subscription status of GSM/WCDMA/LTE system */

    cm_subscription_status_e_type     gw2_sub_avail_status;
      /* Subscription status of GSM/WCDMA system */

    cm_subscription_status_e_type     gw3_sub_avail_status;
      /* Subscription status of GSM/WCDMA system */

    /*
    ** cm_ph_cmd_subscription_not_available_new parameters
    */
    cm_subscription_status_e_type     cdma_sub_not_avail_status;
      /* Subscription status of CDMA system */

    cm_subscription_status_e_type     gwl_sub_not_avail_status;
      /* Subscription status of GSM/WCDMA/LTE system */

    cm_subscription_status_e_type     gw2_sub_not_avail_status;
      /* Subscription status of GSM/WCDMA system */

    cm_subscription_status_e_type     gw3_sub_not_avail_status;
      /* Subscription status of GSM/WCDMA system */

    /*
    ** cm_ph_cmd_subscription_changed_new parameters
    */
    cm_subscription_status_e_type     cdma_sub_changed_status;
      /* Subscription status of CDMA system */

    cm_subscription_status_e_type     gwl_sub_changed_status;
      /* Subscription status of GSM/WCDMA/LTE system */

    cm_subscription_status_e_type     gw2_sub_changed_status;
      /* Subscription status of GSM/WCDMA system */

    cm_subscription_status_e_type     gw3_sub_changed_status;
      /* Subscription status of GSM/WCDMA system */

    #ifdef CM_GW_SUPPORTED

    /*
    ** cm_ph_cmd_get_networks parameters
    **/
    //cm_network_list_type_e_type       network_list_type;
      /* The type of network list to be obtained.              */

    sys_sys_mode_e_type               sys_mode;
    /* Indicates the desired system such as GW */

    /*
    ** cm_ph_cmd_set_preferred_networks parameters
    **/
    sys_user_pref_plmn_list_s_type    user_pref_networks;
        /* The preferred network list to be saved.               */

    #endif /* FEATURE_WCMDA, FEATURE_GSM */


    /*
    ** cm_ph_cmd_packet_state parameters
    **/
    cm_packet_state_e_type            packet_state;
      /* packet state */

    #ifdef FEATURE_JCDMA
    /*
    ** cm_ph_cmd_voc_activity parameters
    **/
    boolean                           voc_act_enable;
      /* Voice activity enable */

    word                              voc_act_voc_1;
      /* # of full rate */

    word                              voc_act_voc_2;
      /* # of 1/2 rate */
    word                              voc_act_voc_8;
      /* # of 1/8 rate */
    #endif /* FEATURE_JCDMA */

    cm_ss_avoid_sys_e_type      avoid_type;
      /* type of the system to be avoided */

    dword                       avoid_time;
      /* avoid time in seconds */

    cm_rtre_config_e_type       rtre_config;
      /* new RTRE configuration */

    cm_rtre_control_e_type      rtre_control;
      /* Current RTRE control - RUIM or NV */

    uint32                      new_acmmax;
    /* New value to be set to the ACMmax. */

    boolean                     uim_status;
      /* Indicates the success/failure of UIM command to set slot pref */

    #ifdef FEATURE_DDTM_CNTL
    cm_ddtm_pref_e_type         ddtm_pref;
      /* ddtm preference.
      */

    sys_ddtm_act_mask_e_type    ddtm_act_mask;
      /* DDTM actions mask.
      */

    cm_ddtm_so_list_act_e_type  ddtm_so_list_act;
      /* Specifies what to do ( add, replace, delete ) with the user
      ** specified SO list in the following parameters.
      */

    uint16                      ddtm_num_srv_opt;
      /* Number of SOs specified in list;
      */

    sys_srv_opt_type            ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
      /* List of SOs for which 1x pages have to ignored while in
      ** HDR connected state. if srv_opt_list[] contains CAI_SO_NULL
      ** & ddtm_pref is ON, ignore all pages.
      */

    uint16                      ddtm_num_blocked_srv_opt;
      /* Number service options blocked by CM_PH_CMD_CHANGE_SRV_OPT 
      */
    sys_srv_opt_type            ddtm_blocked_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
      /* List of service options being blocked by CM_PH_CMD_CHANGE_SRV_OPT 
      */

    #endif

    cm_cphs_proc_e_type         cphs_allowed;
      /*
      ** FEATURE_ALS
      ** Whether Common PCN Handset Specifications procedures are allowed
      */

    cm_als_proc_e_type          als_allowed;
      /*
      ** FEATURE_ALS
      ** Whether ALS procedures are allowed.
      ** cphs_allowed must be CM_ALS_PROC_ALLOWED for
      ** als_allowed to be CM_ALS_PROC_ALLOWED
      */

    cm_als_line_switching_e_type line_switching;
      /*
      ** FEATURE_ALS
      ** Whether the user is allowed to switch between lines
      */

    cm_als_line_e_type          line;
      /*
      ** FEATURE_ALS
      ** Currently selected line (all MO voice calls will use it)
      */

    #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
        defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
    #error code not present
#endif

    cm_wakeup_e_type            wakeup_service;


    cm_network_list_type_e_type network_list_type;
      /* The type of network list to be obtained. */

    cm_mode_pref_e_type         network_type;
      /* Network Type requested in cm_ph_cmd_get_networks() API */

    boolean                     is_command_on_hold;
      /* Is the command on Hold */

    dword                       hold_cmd_uptime;
      /* The uptime until which to hold the command  */

    cm_hold_cmd_reas_e_type     hold_cmd_reason;
      /* The reason for  which to hold the command  */

    cm_client_s_type            *unused_client_ptr;
      /**< The client object that should be freed when processing the
      ** CM_PH_CMD_CLIENT_RELASE command. */

    byte                       cqi_delta;

    sys_meas_mode_e_type        meas_mode;
    /* Mode to enable/disable HDR access */

    cm_user_net_sel_mode_pref_e_type     user_net_sel_mode;
      /* User selected net_sel_mode */

    /*---------------------- Daul Standby cmd Info -----------------------*/

    sys_modem_dual_standby_pref_e_type   standby_pref;
        /* Standby preference of the Phone */

    uint8                                active_subs;
        /* The active subscription in Single Standby mode
        ** only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY
        ** or dual_standby
        */

    uint8                                cmd_subs_mask;
        /* The subs_mask associated with the issued command. used in CM_PH_CMD_ACTIVATE_SUBS
        and CM_PH_CMD_DEACTIVATE_SUBS
        */

    sys_modem_as_id_e_type                 default_voice_subs;
        /* type of the system to avoid */

    sys_modem_as_id_e_type                 default_data_subs;
        /* type of the system to avoid */

    sys_modem_as_id_e_type                 priority_subs;
        /* type of the system to avoid */

    sys_modem_as_id_e_type                 cmd_subs;
        /* The subscription on which the command is being issued.
        */

    cm_plmn_blocking_pref_e_type           request_pref;
        /* Preference to LTE detach and/or PLMN blocking.
        */

    dword                                  blocking_interval;
        /* The interval to block a particular PLMN. (ms)
        */

    sys_drx_cn_coefficient_s1_e_type       drx_coefficient;
        /* DRX value.
        */

    uint16                       sys_sel_pref_req_id;
        /* System Selection preference request identifer.
           This is to be supplied by client.
        */

    uint32                       tl_irat_search_timer_value;
        /*  The timer value for TDSCDMA to LTE IRAT search.
            In milliseconds.
        */

    cm_mmode_mru_table_entry_u_type        mru_table;

    uint8                                  index;
   /*Index used to update the index record in MRU buffer or
      ** Read the Indexth record from the MRU buffer
   */

    sys_sys_mode_e_type                    mode;
     /* System Mode*/

    cm_mru_update_e_type                   mru_update_e_type;
     /* Defines the MRU UPDATE or CLEAN MRU */
    sys_ue_usage_setting_e_type            ue_usage_setting;
    /**< UE Usage setting*/

    sys_csg_id_type                    csg_id;
         /**< CSG identifier */

    sys_radio_access_tech_e_type       csg_rat;
         /**< RAT specified for CSG or
              Pref PLMN+RAT req ( which not related to CSG )*/
  uint32                             hplmn_timer;
      /* timer value needs to be set for HPLMN. This value will be passed to NAS
      */

  cm_band_pref_e_type               band_pref_lmt;
    /* search restricted/limited to GW band preference  */

  sys_lte_band_mask_e_type          lte_band_pref_lmt;
    /* search restricted/limited to LTE band preference */

  cm_band_pref_e_type               tds_band_pref_lmt;
    /* search restricted/limited to TDS band preference */

  sys_voice_domain_pref_e_type           voice_domain_pref;
    /**< Voice domain pref */

  cm_lte_disable_cause_e_type         lte_disable_cause;
   /**<Reason for LTE disable procedure */

    boolean                            wd_switch_on;
      /* Wireless disable switch status on Gobi modem
      */
  sys_data_priority_e_type  priority_type;
            /* specifies the prirotiy to be set for corresponding subid*/

	boolean                            srvcc_capability;
	 /* Used to send SRVCC ON/OFF from UE to NW*/

  sys_block_plmn_cause_e_type    plmn_block_cause;
    /* Cause due to which PLMN block command has been given */	
	
  boolean                         tune_away_req;
   /* Tuneaway req*/		

  sys_data_roaming_e_type       roam_type;
    /* International data roaming setting */

  cm_dds_cause_e_type dds_switch_type;
   /*DDS Switch Type*/
  cm_subscription_not_avail_cause_e_type sub_not_avail_cause;
    /* Cause for subscription not available */	

  cm_ph_secure_call_capability_e_type  secure_call_capability;
    /* Secure call capability/ TF card status */

  cm_set_roaming_pref_param_s_type  roaming_pref;
  /*Send Roaming Pref to NAS*/

} cm_ph_cmd_info_s_type;



/* Type for phone commands.
*/
typedef struct cm_ph_cmd_s {

    CM_CMD_COMMON_FIELDS;
        /* Command header */

    cm_ph_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                        *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_ph_cmd_e_type            cmd;
        /* command */

    cm_ph_cmd_info_s_type       info;
        /* command information */

} cm_ph_cmd_s_type;

/* Payload for client management commands
*/

typedef struct cm_client_gen_reg_data_s
{
  cm_client_event_reg_e_type event_reg_type;
    /* Decides if client is for reg or dereg */

  int from_event;
    /* register from this event (including) */

  int to_event;
      /* to this event (including) */

  void *event_func;
    /* callback function to notify the client for interested events */

  void *cmd_err_func;
    /* callback function to notify errors in commands sent from the client */

  uint8 asubs_mask;
    /* The bit mask to represent the asubs_id(s) to send the SS events */

}cm_client_gen_reg_data_s_type;


/* Type to hold client management command
*/
typedef struct cm_client_cmd_s
{

  CM_CMD_COMMON_FIELDS;
    /* Command header */

  cm_client_cmd_e_type             cmd;
    /* CM command */

  cm_client_gen_reg_data_s_type    info;
    /* CM command information */

} cm_client_cmd_s_type;

/* Type to hold serving system command information.
*/

typedef struct cm_ss_cmd_info_s
{
  cm_ss_orig_thr_act_e_type         orig_thr_act;
        /* Origination throttle action */

  cm_ss_orig_thr_call_status_s_type call_info;
        /* Command information */

  sys_sys_mode_e_type               mode;
        /* The mode on which the call was connected */

  cm_country_code_type              selected_country;
        /* Stores country selected by the user
        */

  cm_sid_type                       sid;
       /* Stores sid selected by user for selected_country
       */

  cm_ss_meas_req_s_type             meas_req;
       /* Provides info about system for which measurement is
       ** required
       */

} cm_ss_cmd_info_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for serving system commands.
*/
typedef struct cm_ss_cmd_s {

    CM_CMD_COMMON_FIELDS;
        /* Command header */

    cm_ss_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                        *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_ss_cmd_e_type            cmd;
        /* command */

    sys_modem_as_id_e_type      cmd_subs;

    cm_ss_cmd_info_s_type       info;
        /* Command information */

} cm_ss_cmd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold inbnad commands information.
*/
typedef struct cm_inband_cmd_info_s {

  cm_inband_cmd_err_e_type    cmd_err;
         /* error in command request */
 cm_call_id_type              call_id;
     /* Call ID */
 uint8                        on_length;
     /* DTMF pulse width */
 uint8                        off_length;
     /* DTMF inter-digit interval */
 uint8                        cnt;
     /* digit count */
 #ifdef FEATURE_JCDMA
 uint8                        digits[CM_INBAND_MAX_REV_DIALED_DIGITS_JCDMA];
 #else
 uint8                        digits[CM_INBAND_MAX_REV_DIALED_DIGITS];
 #endif
     /* DTMF digit buffer */
 uint16                       duration;
    /* Continuous DTMF duration; in milliseconds */

} cm_inband_cmd_info_s_type;



/* Type for inbnad commands.
*/
typedef struct cm_inband_cmd_s {

    CM_CMD_COMMON_FIELDS;
        /* Command header */

    cm_inband_cmd_cb_f_type     *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                        *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_inband_cmd_e_type        cmd;
        /* CM command */

    cm_inband_cmd_info_s_type   info;
        /* CM command information */

} cm_inband_cmd_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type to hold SMS commands information.
*/
typedef struct cm_sms_cmd_info_s {

 cm_sms_cmd_err_e_type     cmd_err;
     /* error in command request */

 cm_sms_awi_ack_s_type     awi_ack;
     /* field for awi ack */

 cm_sms_msg_type           *data;
     /* MO SMS data */

 boolean                   l2_ack_requested;
    /* l2 ack required or not - used by MO SMS*/

 boolean                   bc_enable;

 cm_sms_mo_status_e_type   mo_status;
     /* Status for MO SMS */

 cm_mode_pref_e_type       mode_pref;
     /* mode preference over which SMS is sent */

 #ifdef FEATURE_IP_CALL
 cmipapp_id_type           app_id;
    /* App ID */
 #endif

 sys_sys_mode_e_type       sys_mode;
    /* The sys mode over which the SMS is sent or received. */

} cm_sms_cmd_info_s_type;



/* Type for SMS commands.
*/
typedef struct cm_sms_cmd_s {

    CM_CMD_COMMON_FIELDS;
        /* Command header */

    cm_sms_cmd_cb_f_type    *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                    *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_sms_cmd_e_type       cmd;
        /* CM command */

    cm_sms_cmd_info_s_type  info;
        /* CM command information */

    sys_modem_as_id_e_type  cmd_subs;
        /* Subscription on which this command was issued */

} cm_sms_cmd_s_type;


#ifdef CM_GW_SUPPORTED
/* Type to hold NAS commands information.
*/
typedef struct cm_nas_cmd_info_s {

 cm_nas_cmd_err_e_type     cmd_err;
     /* error in command request */

 cm_nas_req_type           nas_req;
     /* nas req type */

} cm_nas_cmd_info_s_type;

/* Type for NAS commands.
*/
typedef struct cm_nas_cmd_s {

    CM_CMD_COMMON_FIELDS;
        /* Command header */

    cm_nas_cmd_cb_f_type    *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                    *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_nas_cmd_e_type       cmd;
        /* CM command */

    cm_nas_cmd_info_s_type  info;
        /* CM command information */

} cm_nas_cmd_s_type;

#endif  /* FEATURE_WCDMA || FEATURE_GSM */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_GPSONE

typedef struct
{
  cm_pd_cmd_err_e_type    cmd_err;

  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE ))
  cm_pd_session_type_e_type    session_type;
  cm_pd_session_dormant_e_type session_dormant;
  #endif /* FEATURE_GPSONE_MSBASED */
} cm_pd_cmd_info_s_type;

#ifdef FEATURE_IS2000_REL_A
typedef struct
{
  word pilot_pn[CM_BSSR_MAX_PILOTS];
  uint16 num_pilots;

} cm_pd_cmd_reva_info_s_type;
#endif /*FEATURE_IS2000_REL_A*/

typedef struct
{
  CM_CMD_COMMON_FIELDS;
    /* Command header */

  cm_pd_cmd_cb_f_type     *cmd_cb_func;
    /* pointer to a client provided callback function */

  void                    *data_block_ptr;
      /* pointer to a client provided callback data block */

  cm_pd_cmd_e_type        cmd;
      /* CM command */

  cm_pd_cmd_info_s_type   info;
      /* The data itself */

  #ifdef FEATURE_IS2000_REL_A
  cm_pd_cmd_reva_info_s_type   revAinfo;
      /* The data itself */
  #endif /*FEATURE_IS2000_REL_A*/

} cm_pd_cmd_s_type;
#endif /* FEATURE_GPSONE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type for Data Burst Message commands
*/

typedef struct
{
  CM_CMD_COMMON_FIELDS;
    /* Command header */

  uint8 burst_type;
    /* Burst type whether a PD or SDB e.g. CAI_POSITION_DET etc. */

  cm_dbm_cmd_cb_f_type    *cmd_cb_func;
    /* pointer to a client provided callback function */

  void                    *data_block_ptr;
      /* pointer to a client provided callback data block */

  cm_dbm_cmd_e_type        cmd;
      /* CM command */

  cm_dbm_cmd_info_s_type   info;
      /* The data itself */

  cm_dbm_cmd_err_e_type cmd_err;

  dword l2_ack_wait_time;
      /* Max num of seconds to wait for receiving L2 ack */

} cm_dbm_cmd_s_type;



/* Type for BCMCS commands.
*/
typedef struct
{

  CM_CMD_COMMON_FIELDS;
      /* Command header */

  cm_bcmcs_cmd_cb_f_type    *cmd_cb_func;
    /* pointer to a client provided callback function */

  void                   *data_block_ptr;
      /* pointer to a client provided callback data block */

  cm_bcmcs_cmd_e_type        cmd;
      /* CM command */

  cm_bcmcs_cmd_info_u_type   info;
      /* The data itself */

  cm_bcmcs_cmd_err_e_type    cmd_err;


} cm_bcmcs_cmd_s_type;

/* Type to hold MBMS command information.
*/

typedef struct {

  cm_mbms_cmd_err_e_type                          cmd_err;
  sys_gw_mbms_req_e_type                          cmd_type;
    /* Command Type */

  union {
    sys_gw_mbms_activate_req_s_type               activate_req;
    sys_gw_mbms_deactivate_req_s_type             deactivate_req;
    sys_gw_mbms_session_complete_ind_s_type       session_complete_ind;
    sys_gw_mbms_abort_req_s_type                  abort_req;
    sys_gw_mbms_suspend_req_s_type                suspend_req;
    sys_gw_mbms_resume_req_s_type                 resume_req;
    sys_gw_mbms_avail_srv_list_req_s_type         avail_srv_list_req;
    sys_gw_mbms_capability_req_s_type             capability_req;
    sys_gw_mbms_set_attr_req_s_type               set_attr;

  }cmd;

} cm_mbms_cmd_info_s_type;

/* Type for MBMS commands.
*/
typedef struct
{

  CM_CMD_COMMON_FIELDS;
     /* Command header */

  cm_mbms_cmd_cb_f_type    *cmd_cb_func;
    /* pointer to a client provided callback function */

  void                   *data_block_ptr;
      /* pointer to a client provided callback data block */

  cm_mbms_cmd_e_type        cmd;
      /* CM command */

  cm_mbms_cmd_info_s_type   info;
      /* The data itself */

  cm_mbms_cmd_err_e_type    cmd_err;


} cm_mbms_cmd_s_type;

#if defined CM_GW_SUPPORTED || defined FEATURE_IP_CALL
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type to hold sups commands information.
*/
typedef struct cm_sups_cmd_info_s {

   cm_sups_cmd_err_e_type               cmd_err;
      /* error in command request */

   byte                                 ss_ref;
   byte                                 ss_code;
   byte                                 ss_operation;
   byte                                 invoke_id;
   basic_service_s_type                 basic_service;
   cm_called_party_bcd_no_s_type        forwarded_to_number;
   cm_called_party_subaddress_s_type    forwarded_to_subaddress;
   ie_no_reply_condition_timer_s_type   nr_timer;
   ie_ss_password_s_type                ss_password;
   ie_ss_password_s_type                ss_new_password;
   ie_ss_password_s_type                ss_new_password_again;
   ie_ss_code_s_type                    code;
   cm_uss_res_e_type                    uss_res_type;
   uss_data_s_type                      uss_data;
   ss_User_Data_s_type                  ussd_data;
   cm_npi_type 							pi;
   cm_call_barring_list_s_type		    *call_barring_num_list;

   ie_ss_location_notification_res_s_type       location_notification_res;
   cm_molr_type_e_type                             molr_type;
   location_method_s_type                       location_method;
   lcs_qos_s_type                               lcs_qos;
   mlc_number_s_type                            mlc_number;
   lcs_client_external_id_s_type                external_id;
   gps_assistance_data_s_type                   gps_assistance_data;
   supported_gad_shapes_s_type                  supported_gad_shapes;


   ie_cm_ss_error_s_type                        ss_error;

   cm_ccbs_indx_type                            ccbs_indx;
     /*
     ** ccbs_indx = CM_CCBS_INDX_INVALID results
     **   deact of all pending ccbs requests.
     **
     ** ccbs_indx = valid results in one particular
     **   ccbs getting deactivated.
     */

   cm_sups_cmd_err_cause_e_type                      cmd_err_cause;
     /* Cause of Sups cmd error */

   sys_modem_as_id_e_type                       cmd_subs_id;
     /* Subscription ID used for the command */

   cm_ip_sups_time_info_s_type  sups_time_info;

} cm_sups_cmd_info_s_type;


/* Type for supplementary services commands.
*/
typedef struct cm_sups_cmd_s {
    CM_CMD_COMMON_FIELDS;
      /* Command header */

    cm_sups_cmd_exten_cb_f_type   *cmd_cc_cb_func;
    /* pointer to a client provided callback function
       when CM does call control */

    cm_sups_cmd_cb_f_type       *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                        *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_sups_cmd_e_type          cmd;
        /* command */

    cm_sups_cmd_info_s_type     cmd_info;
      /* command information */

    boolean                     is_call_control_processing_complete;
      /* Indicates whether to perform call control */

    cm_num_s_type               dialed_digits;
      /* Digits dialed */

    boolean                     is_parsing_required;
      /* Indicates whether to perform parsing on dialled string or not  */

    cm_orig_client_categ_e_type client_categ;
      /* category of initialting client User or Card */

    cm_alpha_s_type             cc_result_alpha;
      /* Alpha sent by SIM toolkit application after call control
      ** This is to be used if cc_result_alpha.len > 0
      */

    boolean                     is_modem_cc_api;
      /* Indicates whether new API for Centralized call control is called */

    cm_cc_result_e_type         cc_result;
      /* Call control result */

    boolean                     is_sim_cc_performed;
      /* Call control peformed by SIM */

} cm_sups_cmd_s_type;

#endif /* FEATURE_WCDMA || FEATURE_GSM */

/* Type for statistics commands.
*/

typedef struct cm_stats_cmd_info_s {

   cm_stats_cmd_err_e_type          cmd_err;
      /* error in command request */

   sys_modem_module_e_type          module;
     /* Module (NAS/GSM/WCDMA) for which bit mask
     ** is to be se
     */

   uint64                          bit_mask;
     /* The bit mask value to be set */

}cm_stats_cmd_info_s_type;

typedef struct cm_stats_cmd_s {

    CM_CMD_COMMON_FIELDS;
        /* Command header */

    cm_stats_cmd_cb_f_type      *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                        *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_stats_cmd_e_type         cmd;
        /* command */

    cm_stats_cmd_info_s_type    info;
      /* command information */

    sys_modem_as_id_e_type   asubs_id;

} cm_stats_cmd_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct cm_ac_cmd_info_s {

  cm_ac_reason_e_type reason;

  sys_modem_as_id_e_type asubs_id;

}cm_ac_cmd_info_s_type;

/* Type for Access Control commands.
*/
typedef struct cm_ac_cmd_s {

  CM_CMD_COMMON_FIELDS;

  cm_ac_cmd_cb_f_type         *cmd_cb_func;
  /* pointer to a client provided callback function */

  cm_ac_state_cb_f_type       *state_cb_func;
  /* client callback function for ac state */

  void                        *data_block_ptr;
  /* pointer to a client provided callback data block */

  cm_ac_cmd_e_type            cmd;
  /* command */

  cm_ac_cmd_info_s_type       info;
  /* command information */

  cm_ac_cmd_err_e_type        cmd_err;
  /* command error code */

} cm_ac_cmd_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/* Type for MC reports */
typedef struct cm_mc_rpt_s {

    cm_mc_rpt_type          *cm_mc_rpt_ptr;
        /* pointer to MC report */

} cm_mc_rpt_s_type;

#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */


/* mapping table for HO indications from RRC/WRRC */
typedef struct cm_ho_type_mapping {

  sys_ho_type_e_type                    rrc_ho_type;
  sys_voice_handover_e_type             client_ho_type;

}cm_ho_type_mapping_table;


#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

/* Enumeration of QC/3rdparty IMS cache state during SRVCC
**/
typedef enum
{
  CMWCALL_CACHE_NOT_EXPECTED  = 0,
  /**< IMS cache not expected. */

  CMWCALL_CACHE_EXPECTED   =1,
  /**< IMS expected to send cache after SRVCC START. */

  CMWCALL_CACHE_RECIEVED = 2,
  /**< IMS sent cache response  */

  CMWCALL_CACHE_EXPIRED =3,
   /**when wait time expired */

  CMWCALL_CACHE_MAX,

} cm_ims_cache_state_e_type;

typedef struct
{


  cm_ims_cache_state_e_type   state;
  /* State of cache response from IMS */

  dword                           cache_expiry_timer;
  /* Maximum time to wait for IMS cache */

  cm_name_type            cmd_waiting;

  sys_radio_access_tech_e_type        rat ;

  cm_srvcc_call_context_T * call_context_t;
  /* Call context buffer to transfer to NAS  */

  sys_ho_type_e_type       ho_type;

  boolean               is_complete_with_fail;
  /* Used to differentiate NAS fail and IMS fail reason */

  sys_modem_as_id_e_type asubs_id;
    /* Indicates the asubs_id */

} cm_ims_cache;


/* type for TID assignment for SRVCC call combinations */

typedef enum cm_srvcc_call_mask_s_type {

  /** @cond
  */
  CM_SRVCC_CALL_MASK_NONE=0,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */
  CM_SRVCC_CALL_MASK_CONV= 1,
  /**< Single Active Call. */

  CM_SRVCC_CALL_MASK_HELD= 2,
  /**< Single Held Call. */

  CM_SRVCC_CALL_MASK_INCOM=3,
   /**< Single Incoming Call. */

  CM_SRVCC_CALL_MASK_OUTGOING=4,
    /**< Single Outgoing Call. */

  CM_SRVCC_CALL_MASK_ACTIVE_CONFERENCE =5,
    /**< Active CONF Call */

  CM_SRVCC_CALL_MASK_HELD_CONFERENCE =6,
   /**< Held CONF Call */

  CM_SRVCC_CALL_MASK_HELD_REQUESTED_CONFERENCE =7,
   /**< Held requested CONF Call */

  CM_SRVCC_CALL_MASK_RETRIEVE_REQUESTED_CONFERENCE =8
   /**< Retrieve requested CONF Call */

}cm_srvcc_call_mask_e_type;

/** Enumeration of CM srvcc call types.
*/
typedef enum cm_srvcc_call_s_type {

  /** @cond
  */
  CM_SRVCC_CALL_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_SRVCC_CALL_CONV=2,
  /**< Single Active Call. */

  CM_SRVCC_CALL_HELD=4,
  /**< Single Held Call. */

  CM_SRVCC_CALL_ACTIVE_AND_HELD =6 ,
   /**< Active and Held Call */

  CM_SRVCC_CALL_INCOM=8,
   /**< Single Incoming Call. */

  CM_SRVCC_CALL_INCOMING_AND_ACTIVE =10,
    /**< Active and Alerting incoming  Call */

  CM_SRVCC_CALL_INCOMING_AND_HELD =12,
   /**< Held and Alerting Incoming Call */

  CM_SRVCC_CALL_OUTGOING=16,
    /**< Single Outgoing Call. */

  CM_SRVCC_CALL_OUTGOING_AND_HELD =20,
   /**< Held and Alerting outgoing Call */

  CM_SRVCC_CALL_ACTIVE_CONFERENCE=32,
    /**< Single Active Conference Call. */

  CM_SRVCC_CALL_HELD_AND_ACTIVE_CONFERENCE = 36,
   /**< Active and Conference Held Call */

  CM_SRVCC_CALL_INCOMING_AND_ACTIVE_CONFERENCE = 40,
   /**< Conference Active  and Incoming Call */

  CM_SRVCC_CALL_OUTGOING_AND_ACTIVE_CONFERENCE = 48,
   /**<Conference Held  and outgoing Call  */

  CM_SRVCC_CALL_HELD_CONFERENCE=64,
   /**< Single Held Conference Call. */

  CM_SRVCC_CALL_ACTIVE_AND_HELD_CONFERENCE = 66,
  /**< Active and Conference Held Call */

  CM_SRVCC_CALL_INCOMING_AND_HELD_CONFERENCE = 72,
  /**< Conference Held  and incoming Call  */

  CM_SRVCC_CALL_OUTGOING_AND_HELD_CONFERENCE = 80,
  /**< Conference Held and ougoing Call  */

  CM_SRVCC_CALL_CONFERENCE_HELD_REQ = 128,
   /**< Conf Held Req   */

  CM_SRVCC_CALL_HELD_AND_CONFERENCE_HELD_REQ = 132,
   /**< Held Call and Conf Held Request   */

  CM_SRVCC_CALL_INCOMING_AND_CONFERENCE_HELD_REQ = 136,
   /**< Incoming Call and Conf Held Request   */

  CM_SRVCC_CALL_OUTGOING_AND_CONFERENCE_HELD_REQ = 144,
   /**< Conf Held Request and Outgoing Call  */

  CM_SRVCC_CALL_CONFERENCE_RET_REQ = 256,
   /**< Conf Retrieve Request   */

  CM_SRVCC_CALL_ACTIVE_AND_CONFERENCE_RET_REQ = 258,
   /**< Active Call and Conf Ret Request   */

  CM_SRVCC_CALL_INCOMING_AND_CONFERENCE_RET_REQ = 264,
   /**< Conf Retrieve Request and Incoming Call  */

  CM_SRVCC_CALL_OUTGOING_AND_CONFERENCE_RET_REQ = 272,
   /**< Conf Retrieve Request and Outgoing Call  */

   /** @cond
  */
  CM_SRVCC_CALL_TYPE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_srvcc_call_type;

/** Enumeration of srvcc progress state 
*/
typedef enum cm_srvcc_prog_state_e {

  /** @cond
  */
  CM_SRVCC_STATE_NONE =-1,   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

  CM_SRVCC_STATE_HO_EUTRA_REQ_RCVD, /* HO EUTRA REQ received from 1x 
									   and waiting for IMS response */

  CM_SRVCC_STATE_HO_EUTRA_RSP_SENT, /* HO EUTRA RSP sent to 1x */


   /** @cond
  */
  CM_SRVCC_STATE_MAX   /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
 */

} cm_srvcc_prog_state;


/* type for SRVCC TID assignment table */
typedef struct cm_srvcc_tid_list_s_type {

  cm_srvcc_call_type                 srvcc_call_combination;
  byte                               conv_call_tid;
  byte                               held_call_tid;
  byte                               incoming_call_tid;
  byte                               outgoing_call_tid;
  byte                               transient_call_tid;
  byte                               conf_participant_tid_list[MAX_CONF_CALL_NUM_SRVCC];
}cm_srvcc_tid_list;


/* type for NAS call state assignment for SRVCC call combinations */
typedef struct cm_srvcc_nas_call_mapping_table_s_type {

  cm_call_conv_subst_e_type              conv_subst;
  cm_call_incom_subst_e_type             incom_subst;
  cm_call_orig_subst_e_type				 orig_subst;
  boolean                                is_mpty;
  byte                                   cc_state;
  byte                                   nas_hod_aux_state;
  byte                                   nas_mpty_aux_state;
  byte                                   mpty_state;

}cm_srvcc_nas_call_mapping_table;


/** Enumeration of SRVCC ERR codes
*/
typedef enum cm_srvcc_ho_err_type_s {

/** @cond
*/
  CM_SRVCC_HO_ERR_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/
  /* 0 */
  CM_SRVCC_HO_ERR_UNMANAGED_CALLS,
    /**< Unmanaged calls recvd from ims cache */

  CM_SRVCC_HO_UNMANAGED_CALL_COMBINATION,
    /**< unsupported call combination revd from ims cahce */


/** @cond
*/
  CM_SRVCC_HO_ERR_MAX  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_srvcc_ho_err_type;

/* Enum for SRVCC IND */
typedef enum cm_srvcc_ho_ind_s {

/** @cond
*/
  CM_SRVCC_HO_IND_NONE = -1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  CM_SRVCC_HO_IND_COMPLETE,
    /**< SRVCC complete from NAS  */

  CM_SRVCC_HO_IND_FAILURE,
    /**< SRVCC complete from NAS  */

  CM_SRVCC_HO_IND_CANCEL,
   /**< SRVCC Cancel  from LTE RRC  */


/** @cond
*/
  CM_SRVCC_HO_IND_MAX  /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} cm_srvcc_ho_ind_type;



#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_CM_LTE
/* union of indications sent by LTE to CM
*/
typedef union cm_lte_sys_u {

  lte_cphy_rssi_ind_s           lte_rssi_ind;
    /* rssi indication
    */

  lte_rrc_service_ind_s         lte_cell_info;
    /* cell information through RRC srv ind
    */

  lte_rrc_to_CM_act_ind_s       lte_actd_ind;
    /* IRAT reselection to LTE indication (can be initiated by HDRCP)
    */

  lte_rrc_conn_rel_ind_s        lte_conn_rel_ind;
    /* LTE RRC connection released ind information.
    */

  lte_rrc_embms_coverage_state_ind_s    lte_embms_coverage_state_ind;
    /* LTE EMBMS coverate state indication.
    */

} cm_lte_sys_u_type;

typedef union cm_ac_u {

  msgr_hdr_s        msg_hdr; /*!< Message router header */

  cm_ac_emergency_enter_req_s_type                     cm_ac_emergency_enter_request;
  cm_ac_info_ind_s_type                                cm_ac_emergency_enter_ind;
  cm_ac_info_ind_s_type                                cm_ac_emergency_ready_ind;
  cm_ac_emergency_exit_req_s_type                      cm_ac_emergency_exit_request;
  cm_ac_info_ind_s_type                                cm_ac_emergency_exit_ind;

} cm_ac_u_type;

#endif

#ifdef FEATURE_TDSCDMA
/* union of indications sent by TD-SCDMA to CM
*/
typedef union cm_tds_sys_u {

    tds_rrc_scell_signal_status_ind_type tds_rrc_scell_sig_ind;
    /* tds cell signal indication
    */

} cm_tds_sys_u_type;
#endif

typedef struct
{
  /* Message router header */
  msgr_hdr_s          msg_hdr;

}cm_policyman_cfg_s;

typedef union cm_msgr_cmd_u {

  #ifdef FEATURE_CM_LTE

  cm_esm_cmd_u_type       esm;
  cm_lte_sys_u_type       lte_sys;
  lte_rrc_barring_update_ind_s  barring_update_ind;
  cm_emg_u_type           emg_cmd;

  emm_attach_complete_ind_type  emm_attach_complete_ind;

  emm_tau_complete_ind_type emm_tau_complete_ind;

  #ifdef FEATURE_DOMAIN_SELECTION
  cm_sds_cmd_u_type       client_cmd;
  emm_t3402_changed_ind_s_type  emm_t3402_changed_ind;
  #endif

  emm_tai_list_ind_type         emm_tai_list_ind;

  #endif /* FEATURE_CM_LTE */
  mmoc_offline_ind_s_type offline_ind;
    /* Notify sent from MMOC when it goes to OFFLINE */

  rf_init_comp_msg_type   rf_init_status;
    /* RF init result sent from RFM */

  msgr_hdr_struct_type    gps_init_complete;
  /* GPS init completed regardless success or failed */

  #ifdef FEATURE_CM_CFM_AVAILABLE
  cfcm_cmd_msg_type_s           cfcm_ind;
  #endif

  #ifdef FEATURE_TDSCDMA
  cm_tds_sys_u_type       tds_sys;
  #endif

  cm_policyman_cfg_s  policyman_cfg;

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  cm_srvcc_call_context_rsp_s_type  srvcc_rsp;
  #endif

  cm_subsc_chgnd_rsp_s_type  subsc_chgnd_rsp;
  ui_base_msgr_msg_csg_select_config_cmd_msg_type  csg_select_cfg;
  

  cm_volte_state_notification_cmd_s_type volte_status_cmd;

  cm_ext_volte_call_state_ind_s_type   ext_volte_state;

  cm_psm_u_type         cm_psm_cmd;
  cm_mm_set_t_wwan_911_req_s_type set_t_wwan_911_req;
  cm_mm_get_t_wwan_911_req_s_type get_t_wwan_911_req; 
  cm_audio_session_rel_msg_type ipapp_audio_rel_ind;

  cm_mm_set_rpm_parameters_req_s_type set_rpm_parameters_req;
  cm_mm_get_rpm_parameters_req_s_type get_rpm_parameters_req;
  cm_mm_set_rpm_config_req_s_type set_rpm_config_req;

} cm_msgr_cmd_u_type;


/* Type to hold all CM commands from Message Router.
*/
typedef struct cm_msgr_cmd_s {

  q_link_type           link;
      /* Q linking element

         This element has to be the first
         element in this data structure
         because of removal of self_ptr
         in q_link_type
       */

  byte                            reserved[4];
  cm_msgr_cmd_u_type              cmd;

} cm_msgr_cmd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of system selections.
*/
typedef enum {

  CM_SS_NONE    = -1,  /* FOR INTERNAL USE OF CM ONLY! */

  CM_SS_MAIN    = SD_SS_MAIN,       /* Main system selection */

  CM_SS_HYBR_1  = SD_SS_HYBR_1,     /* HDR system selection (applicable only when doing
                                    ** CDMA/HDR hybrid operation)
                                    */

  CM_SS_HYBR_2  = SD_SS_HYBR_2,     /* UMTS system selection (applicable only during
                                    ** DualMode hybrid operation, 1x/GW on MAIN)
                                    */

   CM_SS_HYBR_3  = SD_SS_HYBR_3,     /* UMTS system selection (applicable only during
                                    ** DualMode hybrid operation, 1x/GW on MAIN)
                                    */

  CM_SS_MAX     = SD_SS_MAX         /* Notify MAIN, HYBR_1
                                    ** Only for Common Preferences
                                    */

} cm_ss_e_type;


#define CM_SS_HDR CM_SS_HYBR_1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef enum cm_cmp_criteria_e {

  CM_CMP_CRITERIA_NONE = -1,

  CM_CMP_CRITERIA_LESS_THAN,

  CM_CMP_CRITERIA_EQUAL_TO,

  CM_CMP_CRITERIA_NOT_EQUAL_TO,

  CM_CMP_CRITERIA_LESS_THAN_EQUAL_TO,

  CM_CMP_CRITERIA_GREATER_THAN,

  CM_CMP_CRITERIA_GREATER_THAN_EQUAL_TO,

  CM_CMP_CRITERIA_MAX

  #ifdef FEATURE_RPC
   ,CM_CMP_CRITERIA_BIG=0x10000000 /* To force enum to 32 bit for windows NT */
  #endif /* FEATURE_RPC */
} cm_cmp_criteria_e_type;


#define CM_ACT_ID_NONE ((cm_act_id_type) (-1))
/* Enum of CM activity
*/

typedef enum cm_act_type_e {

  CM_ACT_TYPE_NONE = -1,

  CM_ACT_TYPE_EMERG_CALL,    /* Emergency Call           */

  CM_ACT_TYPE_EMERG_CB_MODE, /* Emergency Call back mode */

  CM_ACT_TYPE_AC_EMERG_MODE, /* Phone in Access Contrl   */

  CM_ACT_TYPE_POLICYMAN_RESTRICT, /* Phone in Restricted mode by policy */

  CM_ACT_TYPE_TEST_CALL,     /* Test call                */

  CM_ACT_TYPE_VOICE_CALL,    /* Voice Call               */

  CM_ACT_TYPE_VT_LOOPBACK,   /* VT loopback calls        */

  CM_ACT_TYPE_VT_CALL,       /* Video Telephony call     */

  CM_ACT_TYPE_PD_CALL,       /* PD call                  */

  CM_ACT_TYPE_SMS_CALL,      /* SMS Call                 */

  CM_ACT_TYPE_DATA_CALL,     /* PS Data Call             */

  #if defined (FEATURE_3GPP_CSFB)
  CM_ACT_TYPE_CS_DATA_CALL,  /* CS DATA Call */
  #endif

  CM_ACT_TYPE_OTAPA_CALL,    /* OTAPA Call               */

  CM_ACT_TYPE_STD_OTASP,     /* STD OTASP Call           */

  CM_ACT_TYPE_NON_STD_OTASP, /* NON_STD OTASP Call       */

  CM_ACT_TYPE_DBM,           /* Data Burst               */

  CM_ACT_TYPE_GPS,           /* GPS activity             */

  CM_ACT_TYPE_DEFAULT_CALL,  /* The type for the default call */

  CM_ACT_TYPE_PH_OBJ,        /* The default activity for the phone object */

  CM_ACT_TYPE_MAX            /* For internal CM use      */

}cm_act_type_e_type;


/* Enum of activity priorities
*/

typedef enum cm_act_priority_e {

  CM_ACT_PRIORITY_NONE = -1, /* NULL priority, Internal USE      */

  CM_ACT_PRIORITY_0    = 0,  /* Priority for Emergency calls     */

  CM_ACT_PRIORITY_10   = 10, /* Priority for the call back mode  */

  CM_ACT_PRIORITY_11   = 11, /* Priority for the thermal emergency state */

  CM_ACT_PRIORITY_19   = 19, /* Priority for the thermal emergency state */

  CM_ACT_PRIORITY_20   = 20, /* Priority for Test calls          */

  CM_ACT_PRIORITY_30   = 30, /* Priority for Voice calls         */

  CM_ACT_PRIORITY_40   = 40, /* Priority for VT loopback calls   */

  CM_ACT_PRIORITY_50   = 50, /* Priority for VT calls            */

  CM_ACT_PRIORITY_60   = 60, /* Priority for PD calls            */

  CM_ACT_PRIORITY_70   = 70, /* Priority for SMS calls           */

  CM_ACT_PRIORITY_80   = 80, /* Priority for Data calls          */

  CM_ACT_PRIORITY_90   = 90, /* Priority for OTAPA calls         */

  CM_ACT_PRIORITY_100  = 100, /* Priority for OTASP calls         */

  CM_ACT_PRIORITY_110  = 110, /* Priority for Data Burst Messages */

  CM_ACT_PRIORITY_120  = 120, /* Priority for GPS activity        */

  CM_ACT_PRIORITY_200  = 200, /* Default priority for all other calls */

  CM_ACT_PRIORITY_END  = 210,/* The priority when the call ends and it is
                             ** still in the queue, this should be
                             ** numerically higher (conceptually the lowest)
                             ** of all priorities                */
  CM_ACT_PRIORITY_PH   = 230,/* The priority for the PH entry, this is the
                             ** lowest of all the activities     */
  CM_ACT_PRIORITY_BELOW_PH,  /* The priority after the phone object */

  CM_ACT_PRIORITY_MAX        /* For internal CM use              */

}cm_act_priority_e_type;

/* Enum of activity update reason
*/

typedef enum cm_act_update_reas_e {

  CM_ACT_UPDATE_REAS_NONE = -1, /* No Reason given, Internal use */

  CM_ACT_UPDATE_REAS_ACT_START, /* Activity started              */

  CM_ACT_UPDATE_REAS_ACT_CONT,  /* Continue with the activity    */

  CM_ACT_UPDATE_REAS_ACT_END,   /* Activity ended                */

  CM_ACT_UPDATE_REAS_USER,      /* User is forcing the pref      */

  CM_ACT_UPDATE_REAS_ACT_PWR_SAVE, /* Shut down the stack, used for handoffs */

  CM_ACT_UPDATE_REAS_EMERG_ENTER,
                                   /* Phone is preparing to enter emerg */

  CM_ACT_UPDATE_REAS_MAX        /* For internal CM use           */

} cm_act_update_reas_e_type;


/* Structure to hold the origination parameters
*/

typedef struct cm_act_orig_s{

  cm_act_priority_e_type           act_priority;
      /* The priority of the activity                                   */

  cm_act_id_type                   act_id;
      /* Unique ID of the activity                                      */

  cm_act_update_reas_e_type        act_update_reas;
      /* The reason for this update                                     */

  sd_ss_orig_mode_e_type           orig_mode;
      /* Last origination mode sent to SD's SS_MAIN and SS_HDR.
      ** Needed for unforcing the origination mode for SS_MAIN and SS_HDR.
      */

  cm_pref_term_e_type              orig_pref_term;
      /* Indicate the current origination mode term                     */

  dword                            orig_mode_uptime;
      /* Indicate uptime for CM_MODE_TERM_CM_1_CALL_PLUS orig_mode_term */

  cm_mode_pref_e_type              orig_mode_pref;
      /* Indicate the mode preference used during call origination      */

  cm_band_pref_e_type              orig_band_pref;
      /* Indicate the band preference used during call origination      */

  sys_lte_band_mask_e_type         orig_lte_band_pref;
      /* Indicate the LTE band preference used during call origination      */

  cm_band_pref_e_type              orig_tds_band_pref;
      /* Indicate the TD-SCDMA band preference used during call origination */
      // TODO TDS handle

  cm_prl_pref_e_type               orig_prl_pref;
      /* Indicate the PRL preference used during call origination       */

  cm_roam_pref_e_type              orig_roam_pref;
      /* Indicate the roam preference used during call origination      */

  cm_hybr_pref_e_type              orig_hybr_pref;
      /* Indicate the hybr preference used during call origination      */

  sd_ss_hybr_pref_e_type           orig_int_hybr_pref;
      /* Indicate the mode preference used during call origination      */

  cm_srv_domain_pref_e_type        orig_srv_domain_pref;
      /* Indicate the service domain preference used during call
      ** origination                                                    */

  cm_mode_pref_e_type              user_mode_pref;
      /* Indicate the UE capability and it is derived from phone
      ** mode preference                                                */

  uint16                                      sys_sel_pref_req_id;
      /* Req. Id
      */

  sys_csg_id_type                    csg_id;
      /**< CSG identifier */

  sys_radio_access_tech_e_type       csg_rat;
      /**< RAT specified for CSG */

  /** stores the pref_update_reason */
  sd_ss_pref_update_reason_e_type  pref_update_reason;
} cm_act_orig_s_type;


/* Type to hold all orig param.
*/
typedef struct cm_orig_q_s {

    q_link_type           link;
        /* Q linking element

           This element has to be the first
           element in this data structure
           because of removal of self_ptr
           in q_link_type
         */


    cm_act_type_e_type    act_type;
        /* Type of CM activity */

    cm_act_orig_s_type    *orig;
        /* origination parameters */

} cm_orig_q_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the type of a CM command.
*/
#define CMD_TYPE( cmd_ptr )             (((cm_cmd_type*)cmd_ptr)->cmd_type )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to the call command part of a CM command.
*/
#define CALL_CMD_PTR( cmd_ptr )         ( (cm_call_cmd_s_type*)cmd_ptr )

#if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
/* Macro to return a pointer to the BCMCS command part of a CM command.
*/
#define BCMCS_CMD_PTR( cmd_ptr )        ( (cm_bcmcs_cmd_s_type*)cmd_ptr )
#endif /* FEATURE_BCMCS || MBMS */

/* Macro to return a pointer to the MBMS command part of a CM command.
*/
#define MBMS_CMD_PTR( cmd_ptr )        ( (cm_mbms_cmd_s_type*)cmd_ptr )

/* Macro to return a pointer to the phone command part of a CM command.
*/
#define PH_CMD_PTR( cmd_ptr )           ( (cm_ph_cmd_s_type*)cmd_ptr )

/* Macro to return a pointer to the client command part of a CM command.
*/
#define CLIENT_CMD_PTR( cmd_ptr )       ( (cm_client_cmd_s_type*)cmd_ptr)

/* Macro to return a pointer to the serving system command part of a CM command.
*/
#define SS_CMD_PTR( cmd_ptr )           ( (cm_ss_cmd_s_type*)cmd_ptr )


/* Macro to return a pointer to the inband command part of a CM command.
*/
#define INBAND_CMD_PTR( cmd_ptr )       ( (cm_inband_cmd_s_type*)cmd_ptr )


/* Macro to return a pointer to the SMS command part of a CM command.
*/
#define SMS_CMD_PTR( cmd_ptr )          ( (cm_sms_cmd_s_type*)cmd_ptr )

#ifdef FEATURE_GPSONE
/* Macro to return a pointer to the PD command part of a CM command.
*/
#define PD_CMD_PTR( cmd_ptr )          ( (cm_pd_cmd_s_type*)cmd_ptr)
#endif /* FEATURE_GPSONE */

/* Macro to return a pointer to the DBM command part of a CM command.
*/
#define DBM_CMD_PTR( cmd_ptr )          ( (cm_dbm_cmd_s_type*)cmd_ptr )

#if defined CM_GW_SUPPORTED || defined FEATURE_IP_CALL

/* Macro to return a pointer to the SUPS command part of a CM command.
*/
#define SUPS_CMD_PTR( cmd_ptr )                  ( (cm_sups_cmd_s_type*)cmd_ptr )

#define NAS_CMD_PTR( cmd_ptr )                   ( (cm_nas_cmd_s_type*)cmd_ptr )

#endif /* FEATURE_WCDMA || FEATURE_GSM */

#define STATS_CMD_PTR( cmd_ptr )                  ( (cm_stats_cmd_s_type*)cmd_ptr)

#define AC_CMD_PTR(cmd_ptr) ((cm_ac_cmd_s_type*)cmd_ptr)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* Macro to return a pointer to the information part of a CM command.
*/
#define CMD_INFO_PTR( xx_cmd_ptr )          ( &xx_cmd_ptr->info )
#define CALL_CMD_INFO_PTR( xx_cmd_ptr )     ( &xx_cmd_ptr->info )
#define DRVCC_SETUP_INFO_PTR( xx_cmd_ptr ) ( &xx_cmd_ptr->info.drvcc_setup_info)

#if (defined CM_GW_SUPPORTED || defined(FEATURE_CM_LTE) || defined FEATURE_IP_CALL)
#define WCALL_CMD_INFO_PTR( xx_cmd_ptr )    ( &xx_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info )
#define WCALL_PS_CMD_INFO_PTR( xx_cmd_ptr ) ( &xx_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_ps_info )
#define SUPS_CMD_INFO_PTR( sups_cmd_ptr)        ( &sups_cmd_ptr->cmd_info )
#endif /* (defined CM_GW_SUPPORTED || defined(FEATURE_CM_LTE)) */

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#define CALL_CMD_MODE_INFO_PTR( xx_cmd_ptr )    ( &xx_cmd_ptr->info.cdma_info )
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#define IPCALL_CMD_INFO_PTR( xx_cmd_ptr ) ( &xx_cmd_ptr->info.ip_call )

#define CM_HOLD_CMD_TIME  2
/* Time to hold the command on the command queue
** default it to 2 seconds
*/


#if defined(FEATURE_CM_LTE)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to check MSG ID from ESM
*/
#define CM_MSGR_MSG_ESM( msg_id ) \
    ( msg_id >= MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND && \
      msg_id <= MM_CM_ESM_NOTIFICATION_IND)

/* Macro to check MSG ID for access control
*/
#define CM_MSGR_MSG_AC( msg_id ) \
    (( msg_id >= MM_CM_AC_1XPPP_CLEAR_START_IND && msg_id <= MM_CM_AC_EMERGENCY_EXIT_IND) || \
      ( msg_id >= MM_CM_AC_EMERGENCY_ENTER_REQ && msg_id <= MM_CM_AC_EMERGENCY_EXIT_REQ))

/* Macro to check MSG ID for CSFB call
*/
#define CM_MSGR_MSG_CSFB( msg_id ) \
    ( msg_id >= MM_CM_1XCSFB_ABORT_RSP && \
      msg_id <= MM_CM_1XCSFB_CALL_RSP )

/*Macro to check MSG ID for CMSDS call */
#define CM_MSGR_MSG_CMSDS( msg_id ) \
    ( msg_id >= MM_DOM_SEL_DOMAIN_SELECTED_GET_REQ   && \
      msg_id <= MM_DOM_SEL_CONFIG_LTE_SMS_DOMAIN_PREF_CHANGED_IND)

/*Macro to check MSG ID for CMSDS CONFIG commands */
#define CM_MSGR_MSG_CMSDS_CONFIG( msg_id ) \
    ( msg_id >= MM_DOM_SEL_CONFIG_LTE_VOICE_DOMAIN_PREF_GET_REQ   && \
      msg_id <= MM_DOM_SEL_CONFIG_UPDATE_LTE_SMS_DOMAIN_PREF_REQ)

/*Macro to check MSG ID for IMS domain selection commands*/
#define CM_MSGR_MSG_CMSDS_IMS( msg_id ) \
    ( msg_id >= MM_DOM_SEL_DOMAIN_SELECTED_GET_REQ   && \
      msg_id <= MM_DOM_SEL_UPDATE_IMS_REG_STATUS_IND)


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to check MSG ID for RSSI
*/
#define CM_MSGR_MSG_CPHY_RSSI( msg_id ) ( msg_id == LTE_CPHY_RSSI_IND )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to check MSG ID for LTE RRC
*/
#define CM_MSGR_MSG_LTE_RRC( msg_id ) ( msg_id == LTE_RRC_SERVICE_IND )

#define CM_MSGR_MSG_LTE_RRC_REL( msg_id ) ( msg_id == LTE_RRC_CONN_REL_IND )

#define CM_MSGR_MSG_LTE_RRC_EMBMS( msg_id ) ( msg_id == LTE_RRC_EMBMS_COVERAGE_STATE_IND )




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to check MSG ID for LTE RRC
*/
#define CM_MSGR_LTE_IRAT_RESEL( msg_id ) ( msg_id == LTE_RRC_TO_CM_ACTIVATION_IND )

/* Macro to check MSG ID for EMM ATTACH COMPLETE
*/
#define CM_MSGR_EMM_ATTACH_COMPLETE( msg_id ) ( msg_id == NAS_EMM_ATTACH_COMPLETE_IND )

/* Macro to check MSG ID for EMM_TAU_COMPLETE
*/
#define CM_MSGR_EMM_TAU_COMPLETE( msg_id ) ( msg_id == NAS_EMM_TAU_COMPLETE_IND )

/* Macro to check MSG ID for NAS_EMM_T3402_CHANGED_IND
*/
#define CM_MSGR_EMM_T3402_CHANGED( msg_id ) ( msg_id == NAS_EMM_T3402_CHANGED_IND )

/* Macro to check MSG ID for NAS_EMM_TAI_LIST_IND
*/
#define CM_MSGR_EMM_TAI_LIST_IND( msg_id ) ( msg_id == NAS_EMM_TAI_LIST_IND )

#endif //defined(FEATURE_CM_LTE)

/* Macro to check MSG ID for SRVCC
*/
#if defined (FEATURE_IP_CALL)
#define CM_MSGR_MSG_SRVCC( msg_id ) \
    ( msg_id == MM_CM_SRVCC_CONTEXT_RSP  )
#endif

#define CM_MSGR_EXT_VOLTE_STATE( msg_id ) \
    ( msg_id == MM_CM_EXT_VOLTE_CALL_STATE_IND  )

#ifdef FEATURE_TDSCDMA

/* Macro to check cell signal status ID for TDS RRC
*/
#define CM_MSGR_MSG_TDS_RRC_SCELL_SIGNAL_STATUS( msg_id ) ( msg_id == TDSCDMA_RRC_SCELL_SIGNAL_STATUS_IND )


#endif //defined(FEATURE_TDSCDMA)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to check whether call events are LTE MO call events
*/
#define CMCALL_IS_LTE_MO_CALL_EVT( call_event ) \
/*lint -save -e831 -e641 -e656 */ ( (call_event >= CM_CALL_EVENT_GET_PDN_CONN_IND) && \
                                  (call_event <= CM_CALL_EVENT_BEARER_MOD_REJ_IND) )
/*lint -restore */

#ifdef FEATURE_CM_CFM_AVAILABLE
/* Macro to check if event is thermal emergency type
*/
#define CM_MSGR_CFCM_THERMAL_EMERG_EVT( msg_id ) \
                            ( msg_id == MM_CM_CFCM_MONITOR_THERMAL_PA_EM_IND )
#endif /* FEATURE_CM_CFM_AVAILABLE */

/* Macro to check if event is OFFLINE from MMOC
*/
#define CM_MSGR_IS_MMOC_OFFLINE_IND_EVT( msg_id ) \
                            ( msg_id == MM_MMOC_OFFLINE_IND )

/* Macro to check if msg is for RFM init
*/
#define CM_MSGR_IS_RF_INIT_COMPLETE_IND( msg_id ) \
                            (msg_id == RFA_RF_CONTROL_INIT_COMPLETE_RSP)

/* Macro to check if msg is for GPS RF init
*/
#define CM_MSGR_IS_GPS_PGI_INIT_COMPLETE_IND( msg_id ) \
                            (msg_id == GPS_PGI_INIT_COMPLETE_RSP)

#define CM_MSGR_IS_POLICYMAN_CFG_UPDATE_IND( msg_id ) \
                            (msg_id == POLICYMAN_CFG_UPDATE_MSIM_IND)

#define CM_MSGR_IS_MM_TUI_CSG_SELECTION_CMD( msg_id ) \
                            (msg_id == MM_TUI_CSG_SELECTION_CMD)

#define CM_MSGR_IS_MM_CM_VOLTE_STATE_NOTIFICATION_CMD( msg_id )\
	                        (msg_id == MM_CM_VOLTE_STATE_NOTIFICATION_CMD)


#define CM_MSGR_MSG_PSM( msg_id )\
	                       ( (msg_id == MM_CM_PSM_ENTER_REQ) || (msg_id ==  MM_CM_PSM_GET_CFG_PARAMS_REQ))

#define CM_MSGR_IS_MM_CM_SET_T_WWAN_911_REQ( msg_id ) \
                            (msg_id == MM_CM_SET_T_WWAN_911_REQ)

#define CM_MSGR_IS_MM_CM_GET_T_WWAN_911_REQ( msg_id ) \
                            (msg_id == MM_CM_GET_T_WWAN_911_REQ)

#define CM_MSGR_IS_MM_SET_RPM_PARAMETERS_REQ( msg_id ) \
                            (msg_id == MM_CM_SET_RPM_PARAMETERS_REQ)

#define CM_MSGR_IS_MM_GET_RPM_PARAMETERS_REQ( msg_id ) \
                            (msg_id == MM_CM_GET_RPM_PARAMETERS_REQ)

#define CM_MSGR_IS_MM_CM_SET_RPM_CONFIG_REQ( msg_id ) \
                            (msg_id == MM_CM_SET_RPM_CONFIG_REQ)

#ifdef FEATURE_IP_CALL
/* Bit masks of srvcc call types. Combination of these can be generated
** by using ADD_SRVCC_CALL_TYPE_MASK.
*/

#define CM_SRVCC_CALL_NONE         ((cm_srvcc_call_type_mask)0)
  /**< Bitmask for call type None. */

#define CM_SRVCC_SINGLE_CALL_ACTIVE_MASK   (cm_srvcc_call_type_mask)SYS_BM_64BIT(CM_SRVCC_CALL_MASK_CONV)
  /**< Bitmask for single active call type  */

#define CM_SRVCC_SINGLE_CALL_HELD_MASK   (cm_srvcc_call_type_mask) SYS_BM_64BIT(CM_SRVCC_CALL_MASK_HELD)
  /**< Bitmask for single held  call type. */

#define CM_SRVCC_SINGLE_CALL_INCOMING_MASK (cm_srvcc_call_type_mask) SYS_BM_64BIT(CM_SRVCC_CALL_MASK_INCOM)
  /**< Bitmask for single incoming  call type. */

#define CM_SRVCC_SINGLE_CALL_OUTGOING_MASK   (cm_srvcc_call_type_mask) SYS_BM_64BIT(CM_SRVCC_CALL_MASK_OUTGOING)
  /**< Bitmask for single outgoing  call type. */

#define CM_SRVCC_CONFERENCE_ACTIVE_MASK      (cm_srvcc_call_type_mask) SYS_BM_64BIT(CM_SRVCC_CALL_MASK_ACTIVE_CONFERENCE)
  /**< Bitmask for conference active call type. */

#define CM_SRVCC_CONFERENCE_HELD_MASK    (cm_srvcc_call_type_mask) SYS_BM_64BIT(CM_SRVCC_CALL_MASK_HELD_CONFERENCE)
  /**< Bitmask for conference held call type. */

#define  CM_SRVCC_HELD_REQUESTED_CONFERENCE_MASK    (cm_srvcc_call_type_mask) SYS_BM_64BIT(CM_SRVCC_CALL_MASK_HELD_REQUESTED_CONFERENCE)
  /**< Bitmask for conference held call type. */

#define  CM_SRVCC_RETRIEVE_REQUESTED_CONFERENCE_MASK    (cm_srvcc_call_type_mask) SYS_BM_64BIT(CM_SRVCC_CALL_MASK_RETRIEVE_REQUESTED_CONFERENCE)
  /**< Bitmask for conference held call type. */


/* TID types for mapping to NAS during SRVCC */

#define TID_INVALID  16

/* MT TID for Conv/incom/Transient calls*/
#define MT_TID_8 8

#define MT_TID_9  9

#define MT_TID_10 10

#define MT_TID_11 11

#define MT_TID_12 12

#define MT_TID_13 13

#define MT_TID_14 14

#define MT_TID_15 15

#endif


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
void cm_ph_cmd_client_act(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type          client_id
            /* Requesting client */

);


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

);

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

);


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

);


/*===========================================================================

FUNCTION cm_ph_cmd_perso_info_available

DESCRIPTION
  This function informs CM that the personalization information
  is availlable for consumption.

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

);

/*---------------------------------------------------------------------------
                                 REPLY OBJECT
---------------------------------------------------------------------------*/


/* Type for CM reply.
** Handles the details that are associated with setting up a CM command
** to wait for a reply from MC.
*/
typedef struct cm_reply_s {

    cm_cmd_type              *cmd_ptr;
        /* Pointer to a CM command waiting for a reply from MC */

    cm_name_type               cm_mc_rpt;
        /* The mc reply  for which we are waiting */
    dword                      uptime;
        /* An uptime upper bound for how long we should wait
        ** for the reply to come */

} cm_reply_s_type;



/* Type to hold data burst messaging interface related information
*/
typedef struct cm_dbm_s {

    cm_pd_resp_is_within_t2m_f_type       *cm_pd_resp_is_within_t2m_func;
    /* pointer to a PD function checking if response is right away */

    /* Add SMS call back function here */
    /* pointer to a SMS function checking if response is right away */

} cm_dbm_s_type;

/*
** Type to hold the Iterator information.
*/
typedef struct cm_iterator_s {
  unsigned int next;
}cm_iterator_type;



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
cm_dbm_s_type  *cmdbm_ptr( void );


/*===========================================================================
=============================================================================
============================= MC INTERFACE BLOCK ============================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cm_hold_cmd_free_q_init

DESCRIPTION
  Initialize the hold command free queue.

  This function must be called before the hold command free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_free_q_init( void );


/*===========================================================================

FUNCTION cm_hold_cmd_q_init

DESCRIPTION
  Initialize the hold command queue.

  This function must be called before the hold command free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_q_init( void );




/*===========================================================================

FUNCTION cm_hold_cmd_q_get

DESCRIPTION
  Dequeue a command from the head of the CM command queue.

DEPENDENCIES
  cm_free_cmd_q_init() must have already been called.

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_cmd_type *cm_hold_cmd_q_get( void );




/*===========================================================================

FUNCTION cm_hold_cmd_q_put

DESCRIPTION
  Enqueue a command buffer onto the CM hold command queue.

DEPENDENCIES
  cm_free_cmd_q_init() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_q_put(

    cm_cmd_type *cmd_ptr
        /* pointer to a command buffer to be enqueued */
);



/*===========================================================================

FUNCTION cm_hold_cmd_q_check

DESCRIPTION
  Check if the passed in command matches with the items in hold command q.
  It only checks the cmd type and cmd. It does not check for the contents
  of command, for example, the dialed digits for origination cmd.

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd are same.
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_hold_cmd_q_check(

  const cm_cmd_type *cmd_ptr

);

/*===========================================================================

FUNCTION cm_hold_cmd_q_add

DESCRIPTION
  Adds a command to a hold queue

DEPENDENCIES
  None

RETURN VALUE
None


SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_q_add
(
   const cm_cmd_type *cmd_ptr,

   size_t      cmd_size
);

/*===========================================================================

FUNCTION cm_hold_cmd_q_same_cmd

DESCRIPTION
  Check if the command type and the command are already in the hold q.
  If the NULL is passed as the command, then just the type is checked

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd exists
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_hold_cmd_q_same_cmd(

  const void *cmd
    /* The command
    */
);


/*===========================================================================

FUNCTION cm_hold_cmd_q_count

DESCRIPTION
  Return the number of command in hold command queue.

DEPENDENCIES
  None

RETURN VALUE
Number of command in hold command queue.


SIDE EFFECTS
  none

===========================================================================*/
extern int cm_hold_cmd_q_count( void );

/*===========================================================================

FUNCTION cm_hold_cmd_handle

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_handle(

  boolean chk_uptime,

  cm_hold_cmd_reas_e_type hold_reason

);


/*===========================================================================

FUNCTION cm_hold_cmd_q_exists_cmd

DESCRIPTION
  Check if the command type and the command are already in the hold q.
  If the NULL is passed as the command, then just the type is checked

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd exists
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_hold_cmd_q_exists_cmd(

  cm_cmd_type_e_type cmd_type,
    /* The type of the command
    */

  const void *cmd
    /* The command
    */
);

/*===========================================================================
FUNCTION CM_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void cm_kick_dog(void);



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
cm_ph_cmd_s_type   *cm_cmd_alloc_ph_init( void );



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
void cm_cmd_ph_init( cm_ph_cmd_s_type * ph_cmd_ptr);



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
extern cm_ss_cmd_s_type *cm_cmd_alloc_ss_init( void );



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
void cm_nc_init( void );

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
void cm_m51_mode_init( void );


/*===========================================================================

FUNCTION cm_si_init

DESCRIPTION
  Initialize the CM system indicators object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_init( void );

#if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))


/*===========================================================================

FUNCTION cm_num_assert

DESCRIPTION
  Check a series of assertions over a CM number to validate its state

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_num_assert(

    const cm_num_s_type    *num_ptr
        /* Pointer to a CM number to be verified */
);
#endif // #if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

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
cm_mode_pref_e_type cm_remove_mode_pref_components
(
  cm_mode_pref_e_type cm_mode_pref,
  int number_of_components_to_remove,
  ...
);

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
cm_mode_pref_e_type cm_add_mode_pref_components
(
  cm_mode_pref_e_type cm_mode_pref,
  int number_of_components_to_add,
  ...
);

/*===========================================================================

FUNCTION cmtask_orig_para_insert

DESCRIPTION
  Queues the origination parameters in the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean cmtask_orig_para_insert(
  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_orig_q_s_type *ph_orig_para_ptr
    /* The buffer to add to the queue
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_delete

DESCRIPTION
  Deletes the ph_orig_ptr from the origination queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean cmtask_orig_para_delete(

  cm_ss_e_type               ss,
     /* The ss, used to select a queue
     */

   cm_orig_q_s_type          *ph_orig_ptr,
     /* The buffer to delete from the queue
     */

   boolean                    free_buf
     /* Return the buffer to the free Q
     */

);

/*===========================================================================

FUNCTION cmtask_change_act_priority

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmtask_orig_para_change_act_priority(

  cm_ss_e_type              ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type    act_type,
    /* Type of CM activity
    */

  cm_act_priority_e_type  curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type  new_priority,
    /* The new priority of the activity
    */

  boolean  global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */

);


/*===========================================================================

FUNCTION cmtask_orig_para_change_act_priority

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmtask_orig_para_change_act_priority_3(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_id_type             act_id,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority
    /* The new priority of the activity
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_change_orig_mode

DESCRIPTION
  Changes the mode_pref of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmtask_orig_para_change_orig_mode(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  sd_ss_orig_mode_e_type     orig_mode,
    /* orig mode for the activity
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id

DESCRIPTION
  Searches the orig queue for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_orig_q_s_type* cmtask_orig_para_search_act_id(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_act_id_type act_id
    /* The buffer to add to the queue
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type

DESCRIPTION
  Searches the orig queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_orig_q_s_type* cmtask_orig_para_search_act_type(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_act_type_e_type act_type
    /* The buffer to add to the queue
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_act_priority

DESCRIPTION
  Searches the orig queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern cm_orig_q_s_type* cmtask_orig_para_search_act_type_act_priority(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */

  cm_act_type_e_type act_type,
    /* The act_type to search in the queue
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_get_act_update_reas_per_pref_reason

DESCRIPTION
  Returns the act update reason to be used while forcing the ph object
  preferences.

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
cm_act_update_reas_e_type cmtask_orig_para_get_act_update_reas_per_pref_reason(
  sd_ss_pref_reas_e_type reason, 
  cm_ss_e_type ss,
  cm_act_update_reas_e_type act_update_reas);


/*===========================================================================

FUNCTION cmtask_orig_para_search_orig_mode

DESCRIPTION
  Searches the orig queue for the orig_mode

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
cm_orig_q_s_type* cmtask_orig_para_search_orig_mode(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */

  sd_ss_orig_mode_e_type  orig_mode
    /* The act_type to search in the queue
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_get_top

DESCRIPTION
  Returns the pointer to the top element of the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the top element if present
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_orig_q_s_type* cmtask_orig_para_get_top(
  cm_ss_e_type ss
    /* The ss whose top element is required
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_get_first

DESCRIPTION
  Return a pointer to first orig_para in the cm_ph_orig_xxx_q.

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().

RETURN VALUE
  Return a pointer to first buffer in the cm_ph_orig_xxx_q list.
  If cm_ph_orig_xxx_q list is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_orig_q_s_type* cmtask_orig_para_get_first(

  cm_ss_e_type ss
    /* The ss for which the first element is needed
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_get_next

DESCRIPTION
  Return a pointer to next orig_param in the cm_ph_orig_xxx_q.

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().


RETURN VALUE
  Return a pointer to next buffer in the cm_ph_orig_xxx_q list.
  If cm_ph_orig_xxx_q list is empty a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
extern cm_orig_q_s_type* cmtask_orig_para_get_next(

  cm_orig_q_s_type* q_ptr,
    /* Pointer to the current q element
    */

  cm_ss_e_type ss
    /* The ss for which the next element is needed
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_alloc

DESCRIPTION
  Allocate a CM command buffer from the CM free hold command queue.

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_orig_q_s_type *cmtask_orig_para_alloc( cm_ss_e_type ss );


/*===========================================================================

FUNCTION cmtask_orig_para_delloc

DESCRIPTION
  Dellocate a CM orig buffer.

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmtask_orig_para_delloc (
  cm_ss_e_type ss,
    /* The queue to deque from
    */
  cm_orig_q_s_type *q_ptr
    /* The pointer to the buffer
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_reset

DESCRIPTION
  Reset the origination parameters queue

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

extern void cmtask_orig_para_reset(
  cm_ss_e_type ss
    /* The ss for which the Q is to be reset
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_count

DESCRIPTION
  Returns the number of elements in the queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/

extern int cmtask_orig_para_count(

  cm_ss_e_type ss
    /* The ss, used to select a queue
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_count_2

DESCRIPTION
  Returns the number of elements in the queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/

extern int cmtask_orig_para_count_2(

  cm_ss_e_type           ss,
    /* The ss for which the count is required
    */

  cm_act_priority_e_type act_priority,
    /* The priority of the activity
    */

  cm_cmp_criteria_e_type cmp_criteria
    /* Comparision criteria
    */
);


/*===========================================================================

FUNCTION cmtask_compute_srv_domain_pref

DESCRIPTION
  Gets the union of all the orig_srv_domain_pref

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The union of all the orig_srv_domain_pref

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_srv_domain_pref_e_type cmtask_compute_srv_domain_pref(

  cm_ss_e_type ss
    /* The ss, used to select a queue
    */
);




/*===========================================================================

FUNCTION cmtask_orig_para_init_2

DESCRIPTION
  Initialize the buffer to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmtask_orig_para_init_2(
   cm_act_orig_s_type *orig_ptr
    /* The buffer to initialize
    */
);


/*===========================================================================
FUNCTION cm_dip_switch_set_ddtm_on

DESCRIPTION
  Registered by CM with HDR log object to be called
  when DDTM dip switch is set

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cm_dip_switch_set_ddtm_on(void);


/*===========================================================================
FUNCTION cm_dip_switch_set_ddtm_off

DESCRIPTION
  Registered by CM with HDR log object to be called
  when DDTM dip switch is set

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cm_dip_switch_set_ddtm_off(void);

/*===========================================================================
FUNCTION emerg_callbck_mode_cb

DESCRIPTION
  Registered by CM with SD. To be called by SD whenever 1x cp confirms
  that an emergency call has been connected. Also called by CM whenever a
  VOIP emergency call connected report comes in.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void emerg_callbck_mode_cb( sd_emerg_cb_mode_e_type );


/*===========================================================================

FUNCTION  cm_msgr_send

DESCRIPTION
  This is a utility function that initializes message header and sends the
  message through MSGR primitives.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern errno_enum_type cm_msgr_send
(
  msgr_umid_type             umid,
  msgr_tech_module_type      from,
  msgr_hdr_struct_type*      msg_ptr,
  uint32                     msg_len
);


/*===========================================================================

FUNCTION cmtask_orig_para_compare_func_req_id_reset

DESCRIPTION
  Utility function to compare for the request id and reset it.

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: Insert position found
  FALSE: Insert position not found
  The Q function requires a return type of int so the return type is NOT
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

int cmtask_orig_para_compare_func_req_id_reset(
  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
   uint16 req_id
);

/*===========================================================================

FUNCTION cmtask_orig_para_is_obj_on_any_q

DESCRIPTION
  Returns is the object is present on any of the queues.

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Presence of the element

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmtask_orig_para_is_obj_on_any_q(

  cm_act_type_e_type         obj_act_type
);

/*===========================================================================

FUNCTION cmtask_is_stop_sig_rcvd

DESCRIPTION
  returns if task stop sig has been received.

DEPENDENCIES
  

RETURN VALUE
  

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean cmtask_is_stop_sig_rcvd(void);

/*===========================================================================

FUNCTION cmtask_set_stop_sig_rcvd

DESCRIPTION
  returns if task stop sig has been received.

DEPENDENCIES
  

RETURN VALUE
  

SIDE EFFECTS
  None.
===========================================================================*/
void cmtask_set_stop_sig_rcvd(boolean val);



 #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

/*===========================================================================

FUNCTION cm_set_srvcc_ho_error_state

DESCRIPTION
  Return SRVCC error state

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cm_set_srvcc_ho_error_state(cm_srvcc_ho_err_type err);

/*===========================================================================

FUNCTION cm_get_srvcc_ho_error_state

DESCRIPTION
  Return SRVCC error state

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cm_srvcc_ho_err_type cm_get_srvcc_ho_error_state(void);

/*===========================================================================

FUNCTION cm_get_cache_ind

DESCRIPTION
  Returns HO_TYPE of SRVCC in progress


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern cm_ims_cache cm_get_cache_ind( void );


/*===========================================================================

FUNCTION cm_set_ims_cache_error

DESCRIPTION
  Sets the HO FAIL reason due to IMS cache context error

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_set_ims_cache_error(void);

/*===========================================================================

FUNCTION cm_set_nas_cache_req

DESCRIPTION
  Sets the NAS request and RAT info for GW SRVCC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_set_nas_cache_req( cm_name_type cmd, sys_radio_access_tech_e_type  rat_mode);

/*===========================================================================

FUNCTION cm_set_ims_cache_state

DESCRIPTION
  Sets the IMS cache state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_set_ims_cache_state(cm_ims_cache_state_e_type val);

/*===========================================================================

FUNCTION cm_set_ims_cache_ho_type

DESCRIPTION
  Sets the IMS cache state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_set_ims_cache_ho_type(
  sys_ho_type_e_type val,
  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
);



/*===========================================================================

FUNCTION cm_process_cmd_on_ims_cache

DESCRIPTION
 Process cache cmd sent during timer expriy/Cache sent from IMS

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_process_cmd_on_ims_cache
(
  cm_ims_cache_state_e_type  cache_state
);


/*===========================================================================

FUNCTION cm_ims_cache_update

DESCRIPTION
  IMS cache ind initialize

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_ims_cache_update(
  sys_ho_type_e_type ho_type,
  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
);

/*===========================================================================

FUNCTION cm_ims_cache_reset

DESCRIPTION
  1. IMS SRVCC cache is reset to defaults
  2. If call_context cache is allocated, free it as well

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_ims_cache_reset( void );


/*===========================================================================

FUNCTION cm_ims_cache_init

DESCRIPTION
  IMS cache ind initialize

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_ims_cache_init( void );


/*===========================================================================

FUNCTION cm_free_ims_cache_call_context

DESCRIPTION
  Free call context pointer if allocated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_free_ims_cache_call_context(void);



/*===========================================================================

FUNCTION cmtask_get_cache_ind

DESCRIPTION
  Returns HO_TYPE of SRVCC in progress


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern cm_ims_cache cm_get_cache_ind( void );

/*===========================================================================

FUNCTION cmset_wait_for_ims_audio_rel

DESCRIPTION
IMS cache ind initialize

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
extern void cm_set_wait_for_ims_audio_rel(
  boolean                  value,
  sys_modem_as_id_e_type   asubs_id
);

/*===========================================================================

FUNCTION cm_is_wait_for_ims_audio_rel

DESCRIPTION
IMS cache ind initialize

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
extern boolean cm_is_wait_for_ims_audio_rel(
  sys_modem_as_id_e_type   asubs_id
);


/*===========================================================================

FUNCTION cm_process_answer_cmd_on_ims_cache

DESCRIPTION
 Process Answer cmd and update Cache sent from IMS

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_process_answer_cmd_on_ims_cache(void);




#endif

/*===========================================================================

FUNCTION cm_print_lte_band_mask

DESCRIPTION
Print the LTE band mask.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
extern void cm_print_lte_band_mask(sys_lte_band_mask_e_type lte_band_pref);

/*===========================================================================

FUNCTION cmtask_orig_para_search_orig_mode_on_any_q

DESCRIPTION
  Returns if the spefied orig mode is any of the priority queue.

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Boolen

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmtask_orig_para_search_orig_mode_on_any_q(

  sd_ss_orig_mode_e_type  orig_mode
);

#endif /* CMI_H */
