#ifndef CMXLL_V_H
#define CMXLL_V_H
/*===========================================================================

              C A L L   M A N A G E R   L O W E R   L A Y E R

                         H E A D E R   F I L E

DESCRIPTION
  This header file contains all the message definitions necessary for
  MC to interface with the Call Manager module.

Copyright (c) 1999 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cmxll.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/12   xs      Codec info propagation
01/17/12   cl      Support SO-33 CALL_END_CNF_F from 1X-CP
01/13/12   aj      SC SVLTE SO 33 suspend changes
01/10/12   gm      Mainlining HDR API change integration
11/02/11   gm      Changes for HDR API cleanup
02/28/11   aj      Updates to DO->LTE reselection
02/01/11   rm      Removing cm_wms_bc_partial_addr_match
01/21/11   rm      CMI-4: Removing #ifdef's
11/16/10   xs      Added con_ref notification support
09/23/10   rk      1XCSFB MO/MT call support
07/31/10   rk      1XCSFB pre-registation initial changes.
06/15/10   aj      support for IRAT measurement support
05/26/10   jqi     EHRPD VOIP silent redial support.
04/01/10   mj      Propagate refresh event session ID
03/15/10   sv      Removing snd.h 
10/15/09   ks      Adding support for FER and PER reporting in 1X and HDR
05/27/09   aj      Adding support for passing otasp status to clients
02/27/09   rm      Adding header files of other MOBs
02/12/09   mh      Added support for no SR after MAP for IS-707B
12/08/08   sv      Adding support for OTASP SPC Error
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
06/06/08   sv      Featurizing snd.h under FEATURE_8K_TO_BE_PURGED
05/06/08   ak      BCMCS 2.0 changes
01/28/07   jqi/aj  Added FEATURE_FULL_TIME_SHDR_3 support.
06/06/07   pk/cl   Added support for LN status notification from HDR
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/27/07   ic      In cm_dbm_mt_type replaced link_flow_number with rlp_flow
                   to make FEATURE_HDR_DOS / EMPA backward compatible
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/17/06   jqi     Added CDMA connect Oder Ack support.
11/16/06   ic      Added support for FEATURE_HDR_EMPA
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/10/06   pk      Modified VoIP - 1X handoff structures
05/17/06   pk      Added support for DDTM during BCMCS flows
03/10/06   pk      Added support for high PSIST
03/02/06   pk      Added VoIP - 1X handoff structures
01/06/06   ic      Lint cleanup  
11/18/05   sk      Added CM API cm_mc_rpt_free_buf() to free CM MC reports.
10/17/05   pk      Added support for CM_CALL_ORIG_ERR_ALL_ACCT_BLOCK
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/22/05   ic      Obsoleted CM_REORDER_F.
                   Use CM_CALL_ORIG_FAIL_F instead with orig_fail.code set to 
                   CM_CALL_ORIG_ERR_REORDER and call_ptr->end_status set to 
                   CM_CALL_END_REORDER
08/16/05   ic      Externalized cm_mc_unlock_resources() 
07/08/05   ic      Report highest layer call origination failure as part of 
                   call events.
                   Moved cm_call_orig_fail_layer_e_type to cm.h  
04/19/05   ic      DOS changes - provided more comments for cm_dbm_mt_type
04/18/05   ic      Merged in third installment of DOS changes
03/04/05   ic      Merged in DataOverSignaling (DOS) support
02/14/05   pk      Removed feature flag from around CM_LINE_CTRL_F 
02/01/05   dk      Added WLAN Support.
01/10/05   sj      Adding call failure handling for no hdr service & no hdr
                   rf lock.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
01/04/05   sj      Corrected base lat/long info in BS status response.
12/04/04   ws      Support for Remote WMS.
11/11/04   sj      Added support for call_end_cnf from HDR.
11/17/04   sj      Added base station info in the BS status rsp msg from 1x.
09/09/04   ws      Added cm_mc_rpt_get_buf_else_err_fatal()
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/02/04   ws      Added support for redirecting record.
05/13/04   ic      Removed signal field from cm_abrv_alert_type (CP layer never
                   never uses it)
02/06/04   ws      Initial jaguar/mainline merge.
11/14/03   ic      Added cm_nss_rel_rec_type for FEATURE_T53.
10/31/03   sj      Removed call_id field in otasp_commit_status.
10/02/03   ic      In cm_hdr_connection_deny_e_type() replaced CM_CD_NETORK_BUSY
                   with CM_CD_NETWORK_BUSY
09/08/03   vt      Made several HDR reports take multiple call ids.
08/27/03   ic      Changes to make hybrid operation work
08/14/03   az      Added support to return overflow size for MO SDB.
04/25/03   ws      JCDMA support for call hold.
04/14/03   ws      Added BSID changes
02/18/03   vt      Fixed problems to build for HDR.
01/21/03   ws      Updated copyright information for 2003
11/15/02   vt      Removed extra message defined for CM_CALL_FAIL_RETRY_ORDER_F
10/31/02   vt      Removed definition of NDSS origination failure.
08/06/02   HQ      Added FEATURE_CDSMS.
07/17/02   vt      Updated it for multimode.
07/08/02   vt      Included cmll.h.
04/09/02   sh      Added the code field and its assocaited enum type for HDR
                   redirect and prl not preferred reports
04/02/02   hxw     Changed CM_GPS_MODE_ABORT_F to CM_GPS_MODE_EXIT_F.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
03/10/02   sh      Added CM_HDR prefix for HDR related reports
02/06/02   sh      Added hybrid 3.0 support
12/11/01   sh      Added CM_CALL_ORIG_NO_SESSION
03/07/02   ic      From cm_call_emergency_flashed_type removed call_id field.
                   Added call_emergency_flashed field to cm_mc_rpt_type
                   structure.
03/06/02   ic      Removed call_id field from cm_ext_brst_intl_type as it was
                   determined it is not needed.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the
                   changes
                   between those two were merged with CCS CM
01/03/02   ic      Added ACCT (Access Control based on Call Type) support
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in
                   N*2165) that we needed to merge with and changes between
                   those two VUs were merged onto CCS CM tip.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
11/30/01   sh      Added access network authentication support
10/23/01   VT/IC   Baseline for CCS
10/09/01   VT      SVD interface changes
08/21/01   ic      Main-lined FEATURE_NEWCM
04/13/01   SH      Added Preliminary HDR support
02/13/01   SH      Added CM_END_ACK_F
02/07/01   RC      Added support for System Determination 2.0.
01/29/01   HQ      Properly feature-ize NEWCM, UASMS and GPSONE.
01/23/01   ych     Merged T53 and JCDMA features.
12/28/00   fc      Added support for retry order.
09/15/00   PJ      Added support for DBM status types.
08/23/00   vt      Added new CM interface changes.
03/13/00   RY      Added OTAPA support
09/09/99   SH      Modified serving system info interface
08/19/98   SH      Added call orig abort and access failure for orig failure
08/17/99   SH      Removed CM_CALL_ORIG_ERR_ABORT
11/08/99   SH      Added NDSS related command IDs
08/09/99   SH      Added alert_ans_bypass for incoming call msg
08/02/99   SH      Added mo_status and orig_fail interfaces
07/15/99   SH      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "cm.h"                          /* Call Manager common structures */
#include "cmll.h"
#include "cmll_v.h"                          /* Common lower layer interface */
#include "cmxll.h"
#include "mc.h"                                  /* Main Control interface */
#include "mc_v.h"                                  /* Main Control interface */

#include "caii.h"                                    /* caii_ext_disp_type */
#include "caii_v.h"                                    /* caii_ext_disp_type */

#include "nv.h"                                            /* NV interface */

#include "hdrcp_msg.h"

#include "homer.h"

#include "mmgsdilib.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

#define CM_MAX_DATA_DIGITS          64  /* Maximum size of a data burst
                                        ** - defined in the CAI */

#define CM_EXTDISP_MAX_RECORD_LEN   64  /* Only for FEATURE_IS95B_EXT_DISP */


/*
** Type for CM_CALL_ORIG_FAIL_F or CM_HDR_CALL_ORIG_FAIL_F
*/
typedef enum cm_call_orig_fail_e {

  CM_CALL_ORIG_ERR_HOLD_ORIG,
    /* Hold orig is TRUE.
    */

  CM_CALL_ORIG_ERR_NO_SRV,
    /* No service.
    */

  CM_CALL_ORIG_ERR_ACC_FAIL,
    /* Fails due to system access failure.
    */

  CM_CALL_ORIG_ERR_ABORT,
    /* Fails due to MC being in a state where it cannot process the
    ** origination command.
    */

  CM_CALL_ORIG_ERR_REDIR,
    /* Fails due to MS receiving a redirection.
    */

  CM_CALL_ORIG_ERR_HANDOFF,
    /*Fails due to MS being handed-off to other mode.
    */

  CM_CALL_ORIG_ACC_IN_PROG,
    /* Fails due to access being in progress.
    */

  CM_CALL_ORIG_ERR_CDMA_LOCK,
    /* Fails due to MS receiving CDMA lock until power cycle order.
    */

  CM_CALL_ORIG_ERR_CDMA_PSIST_NG,
    /* CDMA access failure by persistence test.
    */

  CM_CALL_ORIG_ERR_TIMEOUT,
    /* HDR Connection setup timeout - FEATURE_HDR.
    ** It can be TCA or RTCACK timeout
    */

  CM_CALL_ORIG_NO_SESSION,
    /* HDR session is not open yet - FEATURE_HDR.
    */

  CM_CALL_ORIG_ERR_INCOMPATIBLE,
    /* Concurrent services requested are not compatible.
    */

  CM_CALL_ORIG_ERR_CCS_NOT_SUPPORTED,
    /* Concurrent services are not supported by the MS or the BS.
    */

  CM_CALL_ORIG_ERR_NO_RESPONSE_FROM_BS,
    /* 2nd call orig is not responded to by the BS in 12 seconds.
    */

  CM_CALL_ORIG_ERR_REJECTED_BY_BS,
    /* BS rejects MS's 2nd call orig for other reason.
    */

  CM_CALL_ORIG_ERR_ACCT_BLOCK,
    /* Origination blocked by the BS - FEATURE_IS2000_REL_A.
    */

  CM_CALL_ORIG_ERR_ALREADY_IN_TC,
    /* There is already a traffic channel.
    */

  CM_CALL_ORIG_ERR_HDR_NO_LOCK_GRANTED,
    /* Origination failed because HDR didn't get RF Lock.
    */

  CM_CALL_ORIG_ERR_REORDER,
    /* Origination failed due to reorder 
    */ 

  CM_CALL_ORIG_ERR_ALL_ACCT_BLOCK,
    /* Origination blocked by the BS  for all Mobiles - FEATURE_IS2000_REL_A.
    */

  CM_CALL_ORIG_ERR_MAX_ACC,
    /* Origination failed due to max access probes.
    */

  CM_CALL_ORIG_ERR_TCA_TIMEOUT,
    /* Origination failure due to TCA timeout.
    */

  CM_CALL_ORIG_ERR_RTCACK_TIMEOUT,
    /* Origination failure due to RTC ACK timeout.
    */

  CM_CALL_ORIG_ERR_ACC_FAIL_REJ_ORD,
  /*BS reject order
  */
  CM_CALL_ORIG_ERR_ACC_FAIL_RETRY_ORD,
  /*BS retry order
  */
  CM_CALL_ORIG_ERR_TIMEOUT_T42,
  /* T42 expiry.
  */
  CM_CALL_ORIG_ERR_TIMEOUT_T40,
  /*T40 expiry
  */
  CM_CALL_ORIG_ERR_TRM_REQ_FAIL,
  /*T40 TRM request fail
     */
  CM_CALL_ORIG_ERR_TIMEOUT_T41,
   /*T41 timeout
     */  
  CM_CALL_ORIG_ERR_SIM_BUSY,
   /*SIM not responsive
     */

  CM_CALL_ORIG_ERR_CALL_COLLISION,
   /* Prior to the change, CP will send the flash emergency indication
   ** to CM to end the MO E911 call and send the E911 flash to CLIENT. 
   ** With this change, CP will send the new failure code to CM to redial                                          .
   ** E911 call in 2s and CM will need to end the MT call.
   */

  CM_CALL_ORIG_ERR_MAX
    /* Maximum origination errors.
    */

} cm_call_orig_fail_e_type;


/*
** Release reason
*/
typedef enum {
  CM_CALL_REL_NORMAL,       /* BS released the call and no reason is given */
  CM_CALL_REL_SO_REJECT, /*BS released the call because SO is not supported */
  CM_CALL_REL_OTASP_SPC_ERR /* BS released the call because of SPC error in OTASP */
} cm_rel_reason_e_type;
/*
** Fade reason
*/
typedef enum {
  CM_FADE_GENERIC,              /* to fil the generic fade error cause*/
  CM_FADE_SRV_INIT_FAIL,       /*Fade the call Service Initialization failure */
  CM_FADE_T50_EXP,             /*Fade the call T50m expiry */
  CM_FADE_T51_EXP,             /*Fade the call T51m expiry */
  CM_FADE_RL_ACK_TIMEOUT,      /* Fade the call Ack timeout due to 12 retransmission*/
  CM_FADE_BAD_FL,               /* Fade the call Bad FWD link or T5M expiry*/
  CM_FADE_ERR_MAX
} cm_call_end_fade_e_type ;

/*
** type for CM_HDR_CONNECTION_DENY_F - FEATURE_HDR_HYBRID
*/
typedef enum {
  CM_CD_GENERAL,
  CM_CD_NETWORK_BUSY,
  CM_CD_AUTH_BILLING_FAIL,           /* Authentication or Billing failure */
  CM_CD_PREF_CH_NOT_AVAIL,           /* Preferred channel is not available */
  CM_CD_MAX                          /* Internal use */     
} cm_hdr_connection_deny_e_type;


/*
** type for CM_HDR_REDIRECT_F - FEATURE_HDR_HYBRID
*/
typedef enum {
  CM_REDIRECT_CHG_HDR,
  CM_REDIRECT_EXIT_HDR
} cm_hdr_redirect_e_type;


typedef enum {
  CM_PRL_NOT_PREF_CHG_HDR,
  CM_PRL_NOT_PREF_EXIT_HDR
} cm_hdr_prl_not_preferred_e_type;


/* 1XCSFB status */
typedef enum  {

  CM_1XCSFB_NONE = -1,                  /* Internal use */ 
    
  /* 0 */
  CM_1XCSFB_NOT_AVAIL = 0,              /* 1xCSFB Not available */

  /* 1 */
  CM_1XCSFB_AVAIL,                      /* 1xCSFB available */
    
  /* 2 */
  CM_1XCSFB_PREREG_FAIL,                /* 1xCSFB Pre-registration failure */

  /* 3 */
  CM_1XCSFB_PREREG_SUCCESS,             /* 1xCSFB Pre-registration success */

  CM_1XCSFB_MAX                         /* Internal use */

} cm_1xcsfb_result_e_type;

/* Event that caused the de-register response from 1x */
typedef enum {

  CM_DEREG_FAIL_TIMER = 0,

  CM_DEREG_FAIL_E911_CALL = 1,

  CM_DEREG_FAIL_1X_INTERNAL = 2,

  CM_DEREG_SUCCESS = 3,

}cm_dereg_rsp_cause_e_type;

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Type for CM_OTASP_STATUS_F
*/
typedef struct {
  cm_hdr_type            hdr;                                    /* header */
  cm_otasp_status_e_type otasp_code ;    /* indicates status of OTASP call */                                  
} cm_otasp_status_type;

/* Type for CM_OTASP_COMMIT_STATUS_F
*/
typedef struct {
  cm_hdr_type          hdr;                                      /* header */
  boolean commit_in_progress;    /* TRUE if an OTASP commit is in progress */
                                   /* FALSE if an OTASP commit is finished */
} cm_otasp_commit_status_type;


/* Type for CM_ABRV_ALERT_F - CDMA & AMPS
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
} cm_abrv_alert_type;


/* Type for CM_ABRV_REORDER_F -  AMPS ONLY
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type call_id;                                      /* call id */
} cm_abrv_reorder_type;


/* Type for CM_ABRV_INTERCEPT_F -  AMPS ONLY
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type call_id;                                      /* call id */
} cm_abrv_intercept_type;


/* Type for CM_ALERT_F - Analog only
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  int  signal;                     /* Pitch and cadence of user alert tone */
  byte cnt;   /* Number of chars in first instance of Calling Party Number */
  byte pi;                                       /* Presentation indicator */
  byte si;                                          /* Screening indicator */
  byte cpn[CM_MAX_DATA_DIGITS];                    /* calling party number */
} cm_alert_type;


/* Type for CM_INCOMING_CALL_F or CM_HDR_INCOMING_CALL_F - CDMA only
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  uint32             subs_capability;
  cm_call_id_type    call_id;                                   /* call_id */
  cm_con_ref_type    con_ref;                                   /* con_ref */
  word so;                                               /* service option */
  boolean alert_ans_bypass;/* Call should be automatically answered or not */
} cm_incoming_call_type;

/* Type for CM_INCOMING_PAGE_F - 1x only
*/
typedef struct {
  cm_hdr_type           hdr;             /* header */
  uint32                subs_capability;
  word                  so;              /* service option */
} cm_incoming_page_type;


/* Type for 1X_CALL_END_CNF_F - 1x only
*/
typedef struct {
  cm_hdr_type           hdr;             /* header */
  word                  so;              /* service option */
} cm_1x_call_end_cnf_type;

/* Type for STOP_ALERT_F
*/
typedef union {
  cm_call_end_fade_e_type fade_error_code; /* fade end reason  */
  cm_rel_reason_e_type rel_error_code;								 /* release codes  indicating why the call is released	*/
}cm_stop_alert_error_cause_codes_type;

/* Type for CM_STOP_ALERT_F
** When this command is received, it signals the UI to end the incoming
** call alert.  Additionally, if we're not currently alerting in AMPS
** mode, this signals the UI that the pending Alert failed.  In this
** case, we need to clear out any Caller Id information we have stored.
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  cm_name_type       rpt_type;                                 /*report type */
  word srv_opt;                                                      /* service option from MC */
  cm_stop_alert_error_cause_codes_type error_code;           /* error code */
} cm_stop_alert_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type for CM_INTERCEPT_F */

typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  word srv_opt;                                                      /* service option from MC */
} cm_intercept_type;


/* Type for CM_FLASH_F - Analog only
*/

typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte cnt;   /* Number of chars in first instance of Calling Party Number */
  byte pi;                                       /* Presentation indicator */
  byte si;                                          /* Screening indicator */
  byte cpn[CM_MAX_DATA_DIGITS];                    /* calling party number */
} cm_flash_type;


/* Type for CM_MSG_WAITING_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                          /* header */
  byte msgs;                       /* Indicates number of messages waiting */
} cm_msg_waiting_type;


/* Type for CM_DTMF_BURST_F
*/
typedef struct
{
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte dtmf_on;                              /* dtmf on  CAI 6.7.2.3.2.7-1 */
  byte dtmf_off;                            /* dtmf off  CAI 6.7.2.3.2.7-2 */
  byte cnt;                                                /* digits count */
  byte digits[CM_MAX_DATA_DIGITS];                    /* Data burst digits */
} cm_dtmf_burst_type;


/* Type for CM_DTMF_CONT_START_F
*/
typedef struct
{
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte dtmf_code;                           /* dtmf code CAI 6.7.1.3.2.4-4 */
} cm_dtmf_cont_start_type;


/* Type for CM_DTMF_CONT_STOP_F
*/
typedef struct
{
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
} cm_dtmf_cont_stop_type;

/* Type for CM_CALL_RELEASE_F or CM_HDR_CALL_RELEASE_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  uint8                num_call_ids;     /* Number of calls to be released */
  cm_call_id_type      call_ids[ CM_CALL_ID_MAX ];             /* call_ids */
  cm_rel_reason_e_type rel_codes[ CM_CALL_ID_MAX ];    /* the release code **
                                          - indicates why call is released */
  word srv_opt[ CM_CALL_ID_MAX ]; 													 /* service option from MC */
} cm_call_release_type;


/* Type for CM_MT_CALL_END_IN_ACCESS_STATE
*/
typedef union {
  cm_call_orig_fail_e_type mt_orig_fail_code;         /*error cause for orig fail  */
  cm_rel_reason_e_type rel_code;								 /* release codes  indicating why the call is released	*/
} cm_call_end_error_cause_codes_type;

typedef struct {
  cm_hdr_type             hdr;                                     /* header  */
  cm_name_type            rpt_type;                                        /* report type  */
  cm_call_end_error_cause_codes_type  mt_fail_code;                           /* error cause code */
  word srv_opt;                                                      /* service option from MC */
} cm_call_access_state_mt_fail_type;

/* Type for CM_CALL_FADE_F or CM_HDR_CALL_FADE_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  byte             num_call_ids;       /* Number of calls that were faded  */
  cm_call_id_type  call_ids[CM_CALL_ID_MAX];                   /* call ids */
  cm_call_end_fade_e_type call_end_fade_reason[CM_CALL_ID_MAX]; /* end status for call ids */
  word srv_opt[ CM_CALL_ID_MAX ];                                                      /* service option from MC */
} cm_call_fade_type;

/* Type for CM_LOCK_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  word srv_opt;                                                 /* service option from MC */
  cm_call_id_type call_id;                                 /* callid of active call for which lock report has been sent from MC */
} cm_lock_type;

/* Type for CM_MARKOV_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
} cm_markov_type;


/* Type for CM_CALL_CONNECTED_F or CM_HDR_CALL_CONNECTED_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  cm_con_ref_type    con_ref;                                   /* con_ref */
  word  so;                                              /* service option */
  /* If Base Station had assigned a QoS, that needsto conveyed to CM/UI
  */
  boolean               qos_parms_incl;        /* Qos Parameters indicator */
  byte                  qos_parms_len;
  byte                  qos_parms[CM_ORIG_QOS_LEN_MAX];
  boolean  an_auth_status;/*AT is authenticated with Access network or not */
  
} cm_call_connected_type;

/* Type for the following LL reports to CM:
   CM_HDR_BCMCS_FLOW_STATUS_F
   CM_HDR_BCMCS_BROADCAST_RESET_F
   CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F
   CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F
   CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F
*/
typedef struct {

  cm_hdr_type                          hdr;
    /* Header */

  byte                                 transaction_id;
    /* Transaction id that had been provided by CM
    ** with HDRMC_BCMCS_UPDATE_FLOW_REQ_F command.
    ** Used by the following reports:
    ** - CM_HDR_BCMCS_FLOW_STATUS_F
    ** - CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F
    ** - CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F
    */

  sys_sys_mode_e_type                  protocol;
    /* Indicate what protocol this report comes from
    */

  uint8                                num_of_flows;
    /* Number of entries in flow_status array below
    */

  sys_bcmcs_flow_status_element_s_type      
                                flow_status_array[ SYS_BCMCS_MAX_FLOWS ];

} cm_bcmcs_ll_rpt_type;

typedef struct {

  cm_hdr_type                          hdr;
    /* Header */

}  cm_bcmcs_flow_monitoring_rpt_type;

typedef struct{

  cm_hdr_type                          hdr;
    /* Header */

  byte                                 transaction_id;
  /* 
  ** This field may be used in future. As of now 
  ******* THIS FIELD IS NOT TO BE USED *********
  */

  sys_sys_mode_e_type                  protocol;
    /* Indicate what protocol this report comes from
    */

  uint8                                num_of_flows;
    /* Number of entries in flow_status array below
    */

  sys_bcmcs_reg_status_element_s_type  
                                  reg_status_array[ SYS_BCMCS_MAX_FLOWS ];

} cm_bcmcs_ll_reg_rpt_type;


/* Type for CM_DISPLAY_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                     /* info header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte num_chars;                   /* number of characters in chars array */
  byte chars[ CM_MAX_DATA_DIGITS];                /* Characters to display */
} cm_display_type;


/* Extended Display packed info record
*/
typedef struct {
  byte display;                                  /* display type indicator */
  byte datalen;                              /* length of the packed array */
  byte data[CM_EXTDISP_MAX_RECORD_LEN];                    /* packed array */
} cm_ext_disp_info;

/* Type for CM_EXT_DISPLAY_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                          /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  cm_ext_disp_info disp_info;                    /* struct for packed info */
} cm_ext_disp_type;


/* Type for CM_NDSS_REDIRECTION_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                          /* header */
  cm_call_id_type    call_id;                                   /* call_id */
} cm_ndss_redirection_type;


/* Type for CM_CALLED_PARTY_F
*/
typedef struct {
  cm_info_hdr_type hdr;
  cm_call_id_type    call_id;                                   /* call_id */
  byte number_type;            /* Type of number - see CAI section 7.7.5.2 */
  byte number_plan;            /* Numbering plan - see CAI section 7.7.5.2 */
  byte num_chars;                   /* number of characters in chars array */
  byte chars[ CM_MAX_DATA_DIGITS];                /* Characters to display */
} cm_called_party_type;


/* Type for CM_CALLING_PARTY_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                     /* info header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte number_type;             /* Type of number - see CAI section7.7.5.3 */
  byte number_plan;            /* Numbering plan - see CAI section 7.7.5.3 */
  byte pi;             /* Presentation indicator - see CAI section 7.7.5.3 */
  byte si;                /* Screening indicator - see CAI section 7.7.5.3 */
  byte num_chars;                   /* number of characters in chars array */
  byte chars[ CM_MAX_DATA_DIGITS];                /* Characters to display */
} cm_calling_party_type;


/* Type for CM_CONNECTED_NUM_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                     /* info header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte number_type;            /* Type of number - see CAI section 7.7.5.4 */
  byte number_plan;            /* Numbering plan - see CAI section 7.7.5.4 */
  byte pi;            /* Presentation indicator - see CAI section 7.7.5.3  */
  byte si;                /* Screening indicator - see CAI section 7.7.5.3 */
  byte num_chars;                   /* number of characters in chars array */
  byte chars[ CM_MAX_DATA_DIGITS];                /* Characters to display */
} cm_connected_num_type;


/* Type for CM_SIGNAL_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                     /* info header */
  cm_call_id_type    call_id;                                   /* call_id */
  int  signal_type;  /* Type of signal to generate see CAI section 7.7.5.5 */
  int alert_pitch;     /* Pitch of alerting signal see CAI section 7.7.5.5 */
  int signal;                  /* desired signal - see CAI section 7.7.5.5 */
} cm_signal_type;


/* Type for CM_UPCALL_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  word so;                                               /* service option */
} cm_upcall_type;


/* Type for CM_N_VOICE_F - Analog only
*/
/* This is the IS91 Extended Protocol Voice Mail Notification */

typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  byte  data[14];                    /* 6-bit chunks from message as bytes */
} cm_n_voice_type;


/* Type for CM_N_MSG_F or CM_N_VC_MSG_F - Analog only
*/
/* This is the IS91 Extended Protocol Short Message */
/* The VC flavor is sent if the message is received during  */
/* Voice Channel.  While on this channel, this message is   */
/* interpreted to be CLI info, otherwise it's considered    */
/* SMS info.                                                */
/* The data is ASCII and is ETX terminated.                 */

typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte  data[15];                         /* ASCII data, ETX=03 terminated */
} cm_n_msg_type;


/* Type for CM_N_CLI_F or CM_N_VC_CLI_F - Analog only
*/
/* This is the IS91 Extended Protocol Caller Line ID.  */
/* The VC flavor is sent if the message is received during  */
/* Voice Channel.  While on this channel, this message is   */
/* interpreted to be CLI info, otherwise it's considered    */
/* SMS info.                                                */
/* The data is ASCII and is NULL terminated.                */

typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte  data[32];                    /* 4-bit chunks from message as ASCII */
} cm_n_cli_type;



/* Type for CM_LINE_CTRL_F
*/
typedef struct {
  cm_info_hdr_type  hdr;                 /* info header */
  cm_call_id_type   call_id;             /* call_id */
  boolean           polarity_included;   /* TRUE if polarity info included */
  boolean           toggle;              /* TRUE = toggle polarity         */
  boolean           reverse;             /* TRUE = reverse polarity        */
                                         /* FALSE = normal polarity        */
  byte              power_denial;        /* Power denial time              */
} cm_line_ctrl_type;



typedef struct {
  cm_hdr_type             hdr;                                   /* header */
  byte  band_class;  /* band class see cai.h 0-Cellular 1-PCS 4-Korean PCS */
  nv_block_or_sys_enum_type     block_or_system; /* PCSc block A to F or   */
                                                 /* cellular system A or B */
  word rcv_fields;      /* bit mask to indicate the corresponding field is */
                                    /* rcv'd or not; 1- rcv'd 0- not Rcv'd */
  word  sid;                                           /* 15 bit System ID */
  word  nid;                                          /* 16 bit Network ID */
  word  reg_zone;                              /* 12 bit registration zone */
  byte  packet_zone;                                 /* 8  bit packet zone */
  byte  bs_p_rev;                            /* 8  bit  base station p_rev */
  byte  p_rev_in_use; /* 8  bit  lessor value of BS p_rev and mobile p_rev */
  boolean ccs_supported;              /* concurrent service supported flag */
} cm_srv_sys_info_type;


typedef struct {
  cm_hdr_type hdr;
  uint8    msg_type;
    /* type of message for which status is being reported */
  cm_dbm_mo_status_e_type status;
    /* status of the message */
  word burst_overflow_size;
    /* Contains number of bytes Data Burst has exceeded its limit
       of successful transmission.
    */
} cm_dbm_mo_status_type;

typedef struct {

  cm_hdr_type          hdr;
    /* Header */

  uint8                burst_type;
    /* Type of data burst */

  cm_dbm_source_e_type burst_source;
    /* Indicates what system the DBM was received on */

  uint8                num_bytes;
    /* Number of bytes in the payload array below (CDMA only) */

  uint8                payload[ CM_DBM_MAX_PAYLOAD_SIZE ];
    /* Payload of the mobile terminated dbm (CDMA only) */

  void                *dos_payload;
    /* Data-Over-Signaling payload from HDR passed as a DSM item (HDR only) 
    ** Note - currently no restriction on size 
    */

  uint8                rlp_flow;   
    /* Corresponding Link Flow number for the DoS packet 
    ** (FEATURE_HDR_EMPA)
    */

  byte                 route;
    /* Route on which the DoS packet should go
    ** (FEATURE_HDR_EMPA) 
    */

} cm_dbm_mt_type;


/* Types for the layer where the call origination failed
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  cm_call_orig_fail_e_type code;             /* call orig fail reason call */
  cm_call_orig_fail_layer_e_type layer; /* Layer where the failure occured */
  word srv_opt;                                                      /* service option from MC */
} cm_call_orig_fail_type;


/* type for CM_HDR_CONNECTION_DENY_F */

typedef struct {
  cm_hdr_type hdr;                  /* header */
  byte             num_call_ids;       /* Number of calls that were faded  */
  cm_call_id_type  call_ids[CM_CALL_ID_MAX];                   /* call ids */
  cm_hdr_connection_deny_e_type code;   /* HDR connection denny reason  */
} cm_hdr_connection_deny_type;


/* type for CM_HDR_REDIRECT_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  byte             num_call_ids;       /* Number of calls that were faded  */
  cm_call_id_type  call_ids[CM_CALL_ID_MAX];                   /* call ids */
  cm_hdr_redirect_e_type code;                 /* SD action when receiving */
                                              /* a redirect message in HDR */
} cm_hdr_redirect_type;


/* type for CM_HDR_PRL_NOT_PREFERRED_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  byte             num_call_ids;       /* Number of calls that were faded  */
  cm_call_id_type  call_ids[CM_CALL_ID_MAX];                   /* call ids */
  cm_hdr_prl_not_preferred_e_type code;               /* SD action if on a */
                                           /* system not preferred by PRL  */
} cm_hdr_prl_not_preferred_type;


/* type for CM_IDLE_PGSLOT_CHANGED_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
} cm_idle_pgslot_changed_type;



/* Type for CM_CALL_FAIL_RETRY_ORDER_F
*/
typedef struct {
  cm_hdr_type                   hdr;                             /* header */
  cm_call_id_type    call_id;                                   /* call_id */
  cm_retry_order_s_type retry_info;                    /* retry order info */
} cm_retry_order_report_type;

typedef struct
{
  word           pilot_pn;
  word           base_id;
  word           sid;
  word           nid;
  boolean        base_lat_long_incl;
  sys_base_lat_type
                 base_lat;
  sys_base_long_type
                 base_long;
} cm_pi_rec_type;

typedef struct {
  cm_hdr_type    hdr;                                   /* header */
  boolean        status;    /*Indicates if the send was successful*/
  uint16         num_pilot;
  cm_pi_rec_type pi_rec[CAI_BSS_NUM_PILOTS]; /*record per pilot_pn*/
} cm_bs_status_rsp_type;

/* Type for CM_EXT_BRST_INTL_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                /* Info rec. header */
  word mcc;                                         /* Mobile Country Code */
  byte db_subtype;                                   /* Data Burst subtype */
  byte chg_ind;                                       /* Charge indication */
  byte sub_unit;                           /* Unit call time (1/10 second) */
  byte unit;                                    /* Unit call time (second) */
} cm_ext_brst_intl_type;


/* Type for CM_NSS_CLIR_F
*/
typedef struct
{
  cm_info_hdr_type hdr;                                /* Info rec. header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte  cause;                                              /* Reason code */
} cm_nss_clir_rec_type;


/* Type for CM_NSS_AUD_CTRL_F
*/
typedef struct
{
  cm_info_hdr_type hdr;                                /* Info rec. header */
  cm_call_id_type    call_id;                                   /* call_id */
  byte up_link;                        /* Audio path from the MS to the BS */
  byte down_link;                      /* Audio path from the BS to the MS */
} cm_nss_aud_ctrl_type;

/* Type for CM_NSS_REL_F
*/
typedef struct
{
  cm_info_hdr_type         hdr;                    /* Info rec. header */
  cm_call_id_type          call_id;                         /* call id */
} cm_nss_rel_rec_type;

/* Type for CM_CALL_EMERGENCY_FLASHED_F
*/
/* This message is received when an emergency origination gets
   converted by MC into an emergency flash over existing voice call */

typedef struct
{
  cm_info_hdr_type hdr;                                /* Info rec. header */
  cm_call_id_type original_call_id;                    /* release this one */
  cm_call_id_type emergency_flash_call_id;       /* this one needs to have */
                                             /* call type set to emergency */
} cm_call_emergency_flashed_type;

/* Type for CM_CALL_HOLD_L2ACK_F */
typedef struct {
  cm_hdr_type           hdr;
  cm_call_id_type       call_id;
} cm_call_hold_l2ack_type;

/* Type for CM_SND_BRST_DTMF_ACK_F */
typedef struct {
  cm_hdr_type           hdr;
  cm_call_id_type       call_id;
} cm_snd_brst_dtmf_ack_type;

/* Type for CM_REDIRECTING_NUMBER_F
*/
typedef struct {
  cm_info_hdr_type hdr;                                          /* info header */
  cm_call_id_type    call_id;                                        /* call_id */
  byte number_type;                  /* Type of number - see CAI section7.7.5.3 */
  byte number_plan;                 /* Numbering plan - see CAI section 7.7.5.3 */
  boolean pi_si_included;           /* True if pi/si is included in this record */
  byte pi;                  /* Presentation indicator - see CAI section 7.7.5.3 */
  byte si;                     /* Screening indicator - see CAI section 7.7.5.3 */
  boolean redirecting_reason_included;   /* True if redirection reason included */
  byte redirecting_reason;              /* Redirection reason - See CAI section */
  byte num_chars;                        /* number of characters in chars array */
  byte chars[ CM_MAX_DATA_DIGITS];                     /* Characters to display */
} cm_redirecting_number_type;


/*-------------------------------------------------------------------------*/


/* Type for CM_CDMA_CAPABILITY_UPDATE_F  */
typedef struct
{
  /* Report header.
  */
  cm_hdr_type           hdr;

  /* Capability Information in form of IS2000 Status Message.
  */
  sys_is2000_msg_s_type cdma_cap;

} cm_cdma_cap_upd_rpt_type;

/* Type for CM_UMTS_CDMA_HANDOVER_STATUS_F */
typedef struct
{

  /* Report header.
  */
  cm_hdr_type                     hdr;

  /* Call id
  */
  cm_call_id_type                 call_id;

  /* Handover status
  */
  sys_umts_cdma_handover_e_type  status;

} cm_umts_cdma_handover_status_s_type;


/* Type for CM_HDR_CALL_END_CNF_F
*/
typedef struct {
  cm_hdr_type          hdr;                                     /* header */
  uint8                num_call_ids;     /* Number of calls to be released */
  cm_call_id_type      call_ids[ CM_CALL_ID_MAX ];             /* call_ids */
} cm_call_end_cnf_type;


/* Type for CM_PRIVACY_SETTING_F
*/
typedef struct {
  cm_hdr_type          hdr;                                     /* header */

  mc_long_code_e_type  long_code;                   /* The privacy setting */
 
} cm_privacy_setting_type;

/* Type for CM_PSIST_F
*/
typedef struct {
  cm_hdr_type          hdr;                                     /* header */

  cm_psist_e_type      psist;                        /* The psist setting */
 
} cm_psist_type;

/* Type for CM_HDR_LOCATION_NOTIFICATION_STATUS_F
*/
typedef struct {

  cm_hdr_type          hdr;                                     /* header */

  sys_hdr_location_notification_status_e_type ln_status; /* The LN status */

} cm_hdr_location_notification_status_type;

/* Type for CM_MMGSDI_REFRESH_FCN_IND_F
*/
typedef struct {

  cm_hdr_type                             hdr;         /* header */

  mmgsdi_refresh_evt_info_type            refresh;     /* refresh type */

  mmgsdi_session_id_type                  session_id;  /* session ID for the refresh event */

} cm_hdr_mmgsdi_refresh_ind_type;


/*-------------------------------------------------------------------------*/


/* Type for tunneled message CM_TUNNEL_MSG_F
*/
typedef struct {

  cm_hdr_type            hdr;

  homer_tunnel_msg_s_type   payload;

} cm_tunnel_msg_s_type;


/* Type for tunneled message CM_HDR_TUNNEL_MSG_F
*/
typedef struct {

  cm_hdr_type            hdr;

  homer_hdr_cm_tunnel_msg_s_type hdr_msg;

} cm_hdr_to_cm_tunnel_msg_s_type;

/* Type for tunneled message CM_TUNNEL_MSG_STATUS_F
*/
typedef struct {

  cm_hdr_type                      hdr;

  homer_tunnel_msg_status_e_type   status;

} cm_tunnel_msg_status_s_type;

/* Type for CM_GET_CSNA_MSG_CONFIG_F */
typedef struct
{
  /* Report header.
  */
  cm_hdr_type                     hdr;

} cm_get_csna_msg_config_type;


/* Type for CM_UPDATE_CSNA_MSG_CONFIG_F */
typedef struct
{
  /* Report header.
  */
  cm_hdr_type                     hdr;

  /* Are forward link messages updated
  */
  boolean                         fwd_link_msg_update_present;

  /* Number of forward link messages updated
  */
  int                             num_valid_cch_fwd_link_msg;

  /* The negiotated forward link messages
  */
  homer_csna_msg_config_s_type    cch_fwd_link_msg[HOMER_MAX_CSNA_MSG];

  /* Number of forward link messages updated
  */
  int                             num_valid_ded_fwd_link_msg;

  /* The negiotated forward link messages
  */
  homer_csna_msg_config_s_type    ded_fwd_link_msg[HOMER_MAX_CSNA_MSG];

  /* Are reverse link messages updated
  */
  boolean                         rev_link_msg_update_present;

  /* Number of reverse link messages
  */
  int                             num_valid_cch_rev_link_msg;

  /*The negiotated reverse link messages
  */
  homer_csna_msg_config_s_type    cch_rev_link_msg[HOMER_MAX_CSNA_MSG];

  /* Number of reverse link messages
  */
  int                             num_valid_ded_rev_link_msg;

  /*The negiotated reverse link messages
  */
  homer_csna_msg_config_s_type    ded_rev_link_msg[HOMER_MAX_CSNA_MSG];

} cm_update_csna_msg_config_type;
 

/* Type for HDR meas mode message CM_HDR_MEAS_MODE_F 
*/
typedef struct {

  cm_hdr_type            hdr;                           /* header */
  sys_meas_mode_e_type   meas_mode;                     /* New meas mode  */

} cm_hdr_meas_mode_s_type;

/* Type for HDR measurement response  message CM_HDR_MEAS_RESP_F 
*/
typedef struct {

  cm_hdr_type            hdr;                           /* header */
  cm_ss_hdr_resp_s_type  meas_resp;                     /*HDR meas resp  */

} cm_hdr_meas_resp_s_type;


/* Type for CM_CDMA_CONNECT_ORDER_ACK_F */
typedef struct
{

  /* Report header.
  */
  cm_hdr_type                     hdr;

  /* Call id
  */
  cm_call_id_type                 call_id;

} cm_cdma_connect_order_ack_type;

/* Type for CM_END_VOIP_CALL_F */
typedef struct
{

  /* Report header.
  */
  cm_hdr_type                     hdr;

  /* Call id
  */
  cm_call_id_type                 call_id;

} cm_end_voip_call_type;


/* Type for HDR packet error rate message CM_HDR_PER_INFO_IND_F
*/
typedef struct 
{

  /* Report Header
  */
  cm_hdr_type              hdr;     

  /* HDR packet error rate info from HDR CP
  */
  uint16                   hdr_per_info;

} cm_hdr_per_info_ind_s_type;


/* Type for 1x frame error rate message CM_FER_INFO_IND_F
*/
typedef struct 
{

  /* Report Header
  */
  cm_hdr_type              hdr;     

  /* Frame error rate info from  1X CP
  */
  uint16                   fer_info;

}cm_fer_info_ind_s_type;


/* Type for CM_1XCSFB_STATUS_F
*/
typedef struct {

  /* Report Header
  */
  cm_hdr_type               hdr;                              

  /* 1XCSFB availability/registration status */
  cm_1xcsfb_result_e_type   lte_1xcsfb_result;                 

  /* Is ESR support available in the SIB8 fields */
  boolean     is_esr_supported;
} cm_1xcsfb_status_s_type;


/* Type for CM_1XCSFB_DEACT_CNF_F
*/
typedef struct {

  /* Report Header
  */
  cm_hdr_type               hdr;                              

} cm_1xcsfb_deact_cnf_s_type;

/* Type for CM_1XCSFB_ORIG_RES_F
** This is sent after 1XCP performs persistence test and determines that call 
** could get through.
*/

typedef struct {

  /* Report Header
  */
  cm_hdr_type                       hdr;
                       
  /* Call identifier */
  cm_call_id_type                   call_id;
                   
  /* Call allowed or not.TRUE if allowed, FALSE otherwise 
  */
  boolean                           is_allowed;                

  /* Following fields to be used if is_allowed = FALSE. 
  ** Call orig fail reason call 
  */ 
  cm_call_orig_fail_e_type          code;                      

  /* Layer where the call is failed 
  */
  cm_call_orig_fail_layer_e_type    layer;                     

} cm_1xcsfb_orig_res_type;


/* Type for CM_1XCSFB_CALL_END_CNF_F
** CP will clear the previously stored call object
*/
typedef struct {

  /* Report Header
  */
  cm_hdr_type                       hdr;                       

  /* Call identifier 
  */
  cm_call_id_type                   call_id;                   

} cm_1xcsfb_call_end_cnf_type;



/* Type for  CM_1XCSFB_INCOMING_CALL_F 
** This is for letting CM know that there is an incoming page
*/
typedef struct {

  /* Report Header
  */
  cm_hdr_type                       hdr;                       

  uint32                            subs_capability;

  /* Call identifier.CM_CALL_ID_UNASSIGNED or 0XFE has 
  ** to be used instead of allocating the call ID
  */
  cm_call_id_type                   call_id;                   

  cm_call_type_e_type               call_type;

} cm_1xcsfb_incoming_call_type;

/* Type for  CM_MT_SMS_CALL_RSP 
** This is for letting CM know tabout MT S076 SMS call status
*/
typedef struct {

  /* Report Header
  */
  cm_hdr_type                       hdr;                       

  /* Call identifier.CM_CALL_ID_UNASSIGNED or 0XFE has 
  ** to be used instead of allocating the call ID
  */
  cm_call_id_type                   call_id;

  reject_cause_T                    rej_value;

} cm_1xcsfb_mt_sms_call_rsp_type;

/* Type for CM_CODEC_INFO_F
*/
typedef struct {
  cm_hdr_type        hdr;           /* header */
  cm_call_id_type    call_id;       /* call_id */
  sys_codec_ho_info  codec_info;    /* codec info */
} cm_call_mc_codec_info_type;

/* Type for receiving de-register confirmation from 1x CP
*/
typedef struct {
  cm_hdr_type        hdr;           /* header */
  cm_dereg_rsp_cause_e_type cause;
} cm_dereg_mc_rsp_info_type;


/* Type for receiving responce for the PSM enter request
*/
typedef struct {
  cm_hdr_type        hdr;           /* header */
  sys_psm_status_e_type status;
} cm_psm_rsp_info_type;

/* Type for receiving indication for the PSM enter request
*/
typedef struct {
  cm_hdr_type        hdr;           /* header */
} cm_psm_ind_info_type;

/* 1Xdrvcc ORM status from 1x-cp
*/
typedef struct {
  cm_hdr_type        hdr;           /* header */
  cm_call_id_type    call_id; /* call id for which ORM success status is being reported */
} cm_1xdrvcc_orm_success_ind_s_type;

/* Union of all MC report types.  The header is always present and it
** specifies the command type and attributes. If the command has
** arguments they follow in the union.
*/

typedef union cm_mc_rpt_u {
  cm_hdr_type                   hdr;
  cm_info_hdr_type              info_hdr;
  cm_call_hold_l2ack_type       call_hold_l2ack;
  cm_snd_brst_dtmf_ack_type     snd_brst_dtmf_ack;
  /* CDMA Connect Order ack.
  ** Need to know the featurization: FEATURE_IS2000 or release A???
  */
  cm_cdma_connect_order_ack_type   cdma_connect_order_ack;


  cm_abrv_alert_type            abrv;
  cm_abrv_reorder_type          abrv_reorder;
  cm_abrv_intercept_type        abrv_intercept;
  cm_alert_type                 alert;
  cm_incoming_call_type         incoming_call;
  /*  Incoming page from 1xCP
  */
  cm_incoming_page_type         incoming_page;
  cm_1x_call_end_cnf_type       mc_call_end_cnf;

  cm_stop_alert_type            stop;
  cm_intercept_type             inter;
  cm_flash_type                 flash;


  cm_dtmf_burst_type            dtmf;
  cm_dtmf_cont_start_type       dtmf_cont_start;
  cm_dtmf_cont_stop_type        dtmf_cont_stop;

  cm_call_release_type          release;
  cm_call_fade_type             fade;
  cm_lock_type                  lock;
  cm_call_access_state_mt_fail_type  mt_call_drop_info;
  cm_markov_type                markov;
  cm_call_connected_type        connect;
  cm_display_type               display;

  cm_ext_disp_type              ext_disp;

  cm_called_party_type          called_party;
  cm_calling_party_type         calling_party;
  cm_connected_num_type         connected_num;
  cm_signal_type                signal;

  cm_upcall_type                upcall;

  cm_otasp_status_type          otasp_status;
  cm_otasp_commit_status_type   otasp_commit_status;

  cm_line_ctrl_type             line_ctrl;

  cm_srv_sys_info_type          srv_sys_info;
  
  cm_1xcsfb_status_s_type       cdma_csfb_status;
  cm_1xcsfb_deact_cnf_s_type    cdma_csfb_deact_cnf;

  /* 1XCSFB MO call response */
  cm_1xcsfb_orig_res_type         csfb_org_res;

  /* 1XCSFB call end confirm */
  cm_1xcsfb_call_end_cnf_type     csfb_call_end_cnf;

  /* MT call indication when in 1XCSFB mode */
  cm_1xcsfb_incoming_call_type    csfb_mt_call;

  /* MT SO76 call response */
  cm_1xcsfb_mt_sms_call_rsp_type  csfb_mt_sms_rsp;
  
  cm_dbm_mo_status_type         mo_dbm_status;
  cm_dbm_mt_type                mt_dbm_data;

  cm_msg_waiting_type           msgs;
  cm_n_voice_type               n_voice;
  cm_n_msg_type                 n_msg;
  cm_n_cli_type                 n_cli;

  cm_call_orig_fail_type        orig_fail;

  cm_hdr_connection_deny_type   conn_deny;
  cm_hdr_redirect_type          redirect;
  cm_hdr_prl_not_preferred_type prl_not_preferred;
  cm_idle_pgslot_changed_type   idle_pgslot;

  cm_retry_order_report_type    retry_order;

  cm_bs_status_rsp_type         bss_status_rsp;

  cm_ext_brst_intl_type         ext_brst_intl_rec;
  cm_nss_clir_rec_type          nss_clir_rec;
  cm_nss_aud_ctrl_type          nss_aud_ctrl_rec;
  cm_nss_rel_rec_type           nss_rel_rec;

  cm_ndss_redirection_type      ndss_redirection;

  cm_call_emergency_flashed_type call_emergency_flashed;

  cm_bcmcs_ll_rpt_type                  bcmcs_rpt;
  cm_bcmcs_flow_monitoring_rpt_type     bcmcs_flow_monitoring_rpt;
  cm_bcmcs_ll_reg_rpt_type              bcmcs_reg_rpt;

  cm_redirecting_number_type    redirecting_number;

  /* CDMA capability update report.
  */
  cm_cdma_cap_upd_rpt_type       cdma_cap_rpt;

  /* UMTS -> 1X handover status report.
  */
  cm_umts_cdma_handover_status_s_type  umts_cdma_handover_cnf;

  /* Call end cnf from protocol stack.
  */
  cm_call_end_cnf_type           call_end_cnf;

  /* The voice privacy setting
  */
  cm_privacy_setting_type       privacy_setting;

  /* Psist setting
  */
  cm_psist_type                 psist;

  /* Tunneled message
  */
  cm_tunnel_msg_s_type          tunnel_msg;

  /* Handoff from HDR VoIP to 1X
  */
  cm_hdr_to_cm_tunnel_msg_s_type hdr_tunnel_msg;

  /* Status for the tunneled message
  */
  cm_tunnel_msg_status_s_type    tunnel_msg_status;

  /* Command from HDR requesting the CSNA configuration
  */
  cm_get_csna_msg_config_type    get_csna_msg_config;

  /* Command from HDR requesting the update to CSNA configuration
  */
  cm_update_csna_msg_config_type update_csna_msg_config;

  /* Command from 1x CP to end the voip call
  */
  cm_end_voip_call_type         end_voip_call;

  /* LN status
  */
  cm_hdr_location_notification_status_type ln_status_info;

  cm_hdr_mmgsdi_refresh_ind_type  voting_mmgsdi;  

  /* Packet error rate report from HDR CP
  */
  cm_hdr_per_info_ind_s_type    hdr_per;

  /* Frame error rate report from 1xCP
  */
  cm_fer_info_ind_s_type        fer;

  /* Measurement mode status from HDRCP
  */
  cm_hdr_meas_mode_s_type  meas_mode_status; 

  /*  Measurement response from HDRCP
  */
  cm_hdr_meas_resp_s_type  hdr_meas_resp;

  /*  codec info from 1xcp
  */
  cm_call_mc_codec_info_type  mc_codec_info;

  /* NW deregister report from 1xCP
  */
  cm_dereg_mc_rsp_info_type   mc_dereg_info;

  /* Responce for the PSM enter request 
  */
  cm_psm_rsp_info_type           psm_resp_info;
  
  /* Indication for the PSM enter request 
  */
  cm_psm_ind_info_type           psm_ind_info;
  
  /* 1xdrvcc ORM success status for newly originated call */
  cm_1xdrvcc_orm_success_ind_s_type onexdrvcc_orm_success;

} cm_mc_rpt_type;


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

/****************************************************************************
              FUNCTIONS CALLED BY MC TO COMMUNICATE WITH CM
                              FOR MC'S USE
****************************************************************************/


/*===========================================================================

FUNCTION CM_MC_RPT

DESCRIPTION
  Allows MC to queue up messages to the CM.  Grab a buffer from the
  cm_mc_rpt_free_q, fill it in, and call this function with it. All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cm_mc_rpt
(
    cm_mc_rpt_type *cmd_ptr               /* Pointer to a CM report buffer */
);



/*===========================================================================

FUNCTION CM_AUTONAM_IS_ENABLED

DESCRIPTION
  This function returns whether the auto NAM feature is enabled.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURNS
  boolean - TRUE:  auto NAM is enabled
            FALSE: auto NAM is disabled.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_autonam_is_enabled
(
  void
);


/*===========================================================================

FUNCTION CM_EXT_DISP_BLD_INFO

DESCRIPTION
  This function translates a CAII-format extended display information
  record into a structure that can be iterated to get at the information.

DEPENDENCIES
  none.

RETURN VALUE
  number of bytes of the caii_ext_disp_type structure's var field that
  were used to build the packed structure

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern word cm_ext_disp_bld_info
(
  caii_ext_disp_type* disp_ptr,                /* ptr to caii-format info  */
  cm_ext_disp_info*   info_ptr                /* ptr to packed info struct */
);

/*===========================================================================

FUNCTION CM_MT_DBM_MSG

DESCRIPTION
  This function should be used as the **one** function MC should call when
  a mobile-terminated data burst message is received. This function will
  dispatch either the SMS callback or PD callback depending on the
  type of message received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cm_mt_dbm_msg
(
  uint8   dbm_msg_type,                             /* The data burst type */
  uint8   format,  /* Format of the data burst type. Relevant only for SMS */
  const uint8*  data,                                  /* Pointer to the paylaod */
  uint16  data_len             /* The number of bytes in the payload */
);

/*===========================================================================

FUNCTION CM_MT_SMS_MSG

DESCRIPTION
 CM provides this function for MC to deliver mobile terminated sms message

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_mt_sms_msg
(
  uint8         format,                          /* MT SMS message format */
  uint16        len,                                        /* msg length */
  const uint8   *data                                /* msg buffer pointer */
);

/*===========================================================================

FUNCTION CM_MT_DBM_REPLY_IS_WITHIN_T2M

DESCRIPTION
 CM provides this function for MC to decide if the response to an incoming
 PD DBM is timing-constrainted.

DEPENDENCIES
  none

RETURN VALUE
  True if there is a PD response to be sent out right away for a given request.
  Otherwise False.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cm_mt_dbm_reply_is_within_t2m
(
  uint8 dbm_msg_type,                               /* The data burst type */
  uint8 format,/* The format of the data burst type. Relevant only for SMS */
  uint8      *payload,                               /* msg buffer pointer */
  uint32     numBytes                                        /* msg length */
);


/****************************************************************************
           FUNCTIONS CALLED BY CM TO COMMUNICATE WITH LOWER LAYERS
                          ** FOR INTERNAL USE OF CM **
****************************************************************************/


/*===========================================================================

FUNCTION CM_MC_GET_BUF_ELSE_ERR_FATAL

DESCRIPTION
  Allocate a MC Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MC command buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern mc_msg_type *cm_mc_get_buf_else_err_fatal
(
  void
);


/*===========================================================================

FUNCTION CM_MC_RPT_GET_BUF_ELSE_ERR_FATAL

DESCRIPTION
  Allocate a MC rpt buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MC rpt buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_mc_rpt_type *cm_mc_rpt_get_buf_else_err_fatal
(
  void
);


/*===========================================================================

FUNCTION cm_mc_rpt_free_buf

DESCRIPTION
  Free a MC report buffer allocated from CM. Do NOT use to function to
  indicate the completion of processing of the report. Use cm_ret_rpt_buf for
  that.

DEPENDENCIES
  none

RETURN VALUE
  none.

SIDE EFFECTS
  none

===========================================================================*/
void cm_mc_rpt_free_buf( 

  cm_mc_rpt_type *mc_rpt_ptr

);


/*===========================================================================

FUNCTION CM_MC_SEND_CMD

DESCRIPTION
  Send a command to the MC task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_mc_send_cmd
(
  mc_msg_type  *mc_cmd_ptr                        /* Pointer to mc command */
);

/*========================================================================

FUNCTION CM_SEND_HDR_UNLOCK_RF_CMD

DESCRIPTION
  This function sends a unlock RF command to HDR.

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
extern void cm_send_hdr_unlock_rf_cmd(void);
/*========================================================================

FUNCTION CM_SEND_HDR_ABORT_RESEL_CMD

DESCRIPTION
  This function sends an abort reselection command to HDR.

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
extern void cm_send_hdr_abort_resel_cmd(sys_modem_as_id_e_type        asubs_id);

/*========================================================================

FUNCTION 1.	MC_ABORT_1X_TO_LTE_RESEL_F 

DESCRIPTION
  This function sends an ABORT_RESEL command to 1X.

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
extern void cm_send_1x_abort_resel_cmd(sys_modem_as_id_e_type        asubs_id);

/*========================================================================

FUNCTION CM_MC_UNLOCK_RESOURCES

DESCRIPTION

  This function is called by mccdma.c from mcc_wait() when MC task got a 
  command queue signal, just after srch_rude_wakeup() was called. 

  Send HDRMC_UNLOCK_HDR_RF_F if we have a call on 1X of type 
  voice, sms, pd, otasp, std_otasp, e911 in origination state or
  when sms / dbm is in progress

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
extern void cm_mc_unlock_resources(void);

/*=========================================================================
FUNCTION
  cm_wms_bc_addr_match

DESCRIPTION
  This function checks the BC_ADDR of a received broadcast page or
  broadcast message to see if it passes the filter.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - the mobile should process the received message
  FALSE - the mobile should discard the received message

SIDE EFFECTS
  None

=========================================================================*/
extern boolean cm_wms_bc_addr_match
(
  byte *bc_addr_ptr
);

#endif /*CMXLL_V_H*/
