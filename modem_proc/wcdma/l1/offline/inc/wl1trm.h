#ifndef WL1TRM_H
#define WL1TRM_H
/*===========================================================================

                    L 1   T R M   A N D   T C X O   M A N A G E R

DESCRIPTION
  This file contains global declarations and external references
  for the L1 TRM and TCXO Manager.

EXTERNALIZED FUNCTIONS

  wl1_trm_request_primary_antenna
    This function requests TRM for the primary antenna before WCDMA can 
    attempt to acquire.

  wl1_trm_debug_delay
    This function generates a delay in msec for debugging purposes

  wl1_tcxo_set_restriction
    Invoked due to either a restriction change from TCXO Mgr, or when error becomes
    large and UMTS is breaking the TCXO Mgr restriction or we are freezing the trk_lo
    for debugging purposes. This simply posts a local command which does all the work.
    Necessary because we cant call tcxomgr_report() from interrupt context.

  wl1_tcxo_set_restriction_cmd
    Generally invoked by the local command TCXO_SET_RESTRICTION_CMD to actually 
    perform the freezing or unfreezing of the TCXO. This function needs send 
    acks (when TCXO Mgr gives us a new restriction) and reports (when breaking 
    a restriction) to TCXO Mgr when necessary. Can also be invoked directly,
    if we cannot post a local command and wait for it at the moment.

  wl1_tcxo_resume_restriction
    Resumes current restriction after WL1 diverges from the restriction due to error
    in the VCO.

  wl1_tcxo_release
    Releases control of the TCXO.

  wl1_tcxo_get_vco
    This function returns the vco value from the mdsp.

  wl1_tcxo_frozen
    Returns if the tcxo is currently frozen or not.

  wl1_tcxo_rpush
    Performs the actual rotator push when using TCXO Mgr 3.0

  wl1_tcxo_request
    This function requests TCXO Mgr for control of the VCTCXO.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2012 by Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1trm.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   gm      Removed unnecessary error prints
09/29/15   bj      Increase the split acq step1+NASTT step lock duration from  80 to 85 ms 
                   and boost the clock level to NOM for NASTT search in ACQ state to avoid 1x page miss.
07/02/15   gsk     Removing unused macro
09/15/15   bj      Split ACQ changes for W+1x.
06/13/15   ar      Changes to not set RRC to IN_LOCK status when we issued a fake grant.
02/23/15   gm      To avoid the race condition between wrm_release() and prio_inv_timer expiry.
02/06/15   ar      Changes to avoid posting fake drx cmd during cphy operations
01/27/15   qj      Extern request resource API to fix compilation warning
01/28/15   gm      Mods to check if client's activity is valid or not.
01/05/15   abs     DSDS: Send BAND_TUNE_CONFIRM to TRM in response to all
                   TRM_MODIFY_BAND requests upon receiving a TRM_GRANT
01/04/15   gm      0x41c1 log packet changes
12/12/14   skk     Change TRM priority if BCCH wakesup before sleep after PICH wakeup. 
12/01/14   gm      Add wrapper wl1idletamgr lock req api wrapper for RRC to call
11/28/14   gm      Add new macros for checking if any cmd is pending and a macro 
                   for checking if an activity needs TX.
11/25/14   sad     Change to handle modify activity failure
11/19/14   rs      Code changes for requesting lock with new proc during BCH decode
11/7/14    sks     Call exit mode on PRx based on whether HW supports DR operation, not on the instantaneous mode.
11/19/14   gm      To implement changes to inform TRM if div is needed for measurement.
11/04/14   sad     Cleanup of lock req for freq scan
11/07/14   sks     Call exit mode on PRx based on whether HW supports DR operation, not on the instantaneous mode.
11/06/14   ar      Add new macro to get the RF device ID for a resource
10/14/14   sd      new APIs for DR to SR fallbacks
10/10/14   sd      Remove l1m_wait_for_trm_stop and related code
10/07/14   rsr     Added Change to Extern Func. WRM Request Resources for Sec.
10/07/14   sd      Fixed compilation warnings
09/24/14   gm      Mods to wait for TRM grant to update freq.
09/26/14   dm      DSDS: Indicate the minimum time needed for unlock when changing the activity.
09/23/14   pv      Support for TRM swapping of chains for call setup in DR mode.
                   Added MACRO to check if CMD PENDING for call setup.
09/04/14   abs     Feature cleanup for Bolt
08/24/14   rsr     Added Changes to Ensure that Sec. Chain is not granted to RxD
                   erroneously, and ensure that WRM Pending request is invalidated
                   once granted, also ensured RxD Grant callback is called once 
                   Pending Request is handled.
08/18/14   skk     Request for lock for CELL_SELECTION_REQ in G2W TA mode as lock can be released as a part of SIB sleep.
08/12/14   sd      Fixed DSDS Medium and Low compiler warnings in WCDMA.
08/04/14   rs      Fixed compiler warnings
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
07/25/14   as      Adding secondary antenna activities.
07/08/14   dm      Fixes related to DR DSDS bring up from multisim side.
07/02/14   gm      Fixes related to DR DSDS bring up from multisim side.
06/19/14   dm      DR-DSDS first cut.
06/17/14   jd      Fix to return floored rxagc when trm is not reserved.
05/06/14   as      Moving WRM to WL1TRM.
04/28/14   sm      Internal Unlock API for Secondary
04/24/14   as      Moving wrm code to wl1trm.
04/18/14   as      DSDS Code Reorganization.
04/16/14   bj      Split ACQ code cleanup
04/03/14   jkb     Use WRM state instead of procedure mask to check lock status
04/02/14   dm      DSDS: New API to make tuneaway duration read from RRC available to 
                   all L1 modules.
02/12/14   sad     DB-3C API check in
04/01/14   rs      Externed a function to request lock for secondary chain
03/27/14   pg      To avoid back to back reservation with FRM in case tuneaway timer 
                   expires in parallel in case FACH QTA
02/14/14   qj      Changes for W diversity antenna sharing during PCH_SLEEP
03/27/14   sm      OMRD changes to make RxD trm compliant
03/24/14   as      Delaying call to dl_cell_check_tddet_status to after closing the QTA gap.
03/24/14   as      Changing wrm_unlock_trm_cb api to get additional data from trm.
01/27/14   as      Implemented Version 2 of the W2G tuneaway log packet WL1TRM_TUNEAWAY_LOG_PKT.
01/22/14   pkg     EUL optimization to avoid HICH result falling into QTA gap and improved RSN management for QTA.
01/22/14   gm      post a local command to handle unlock cancel event. 
01/21/14   abs     Cleaning up DSDA~CXM(FR:2334) changes
01/20/14   ar      Add TRM request support for W->L searches
01/17/14   jkb     Adding lock check to WL1_IN_DS_WITH_ANY_LOCK and 
                   WL1_IN_DS_WITH_NO_LOCK 
01/17/14   raj     Added changes for raising W UL Priority to CXM for SRB Data.
01/13/14   jkb     Changing WL1_IN_DS_WITH_ANY_LOCK and WL1_IN_DS_WITH_NO_LOCK
01/07/14   as      Added data structure and external funtions to support
                   WL1TRM_TUNEAWAY_LOG_PKT log packet.
12/16/13   as      Porting G2W TA from Triton to Dime.
12/06/13   ar      Modified definition wl1_ds_pre_meas_start_ind
11/15/13   mk      DSDS: Handling CPHY_DRX_REQ during sleep.
11/14/13   as      W2G TA: Adding code for RRC dependency.
10/29/13   dm      DSDS: Use WRM state to check if L1 has lock.
09/29/13   as      Continuing port of Dual Sim from Triton to Dime.
09/23/13   as      Porting Dual Sim from Triton to Dime.
09/12/13   jd      Searcher redesign
08/28/13   bs      Resolved KW issues
08/12/13   cc      DBDC CM W2W feature check in
07/24/13   jd      Changes for Dual band Dual carrier
01/11/13   scm     More partial FEATURE_DUAL_SIM support for DSDA.
12/10/12   scm     Partial FEATURE_DUAL_SIM support for DSDA.
12/07/12   scm     Pass trm_get_rf_device() return value into RFM APIs.
08/13/12   jd      Trm changes on dime.
01/30/12   vs      Feature cleanup.
06/30/10   scm     Reserve TCXO control after WFW download.
05/13/09   scm     Implement wl1_trm_set_starting_trk_lo().
10/03/08   hk      Bringing in 7k Mailine fixes
04/17/07   kps     Add wl1_tcxo_resume_restriction()
04/10/07   kps     Fully featurize file under the TRM/TCXOMGR30 features
03/09/07   kps     change the return type for wl1_tcxo_rpush()
01/19/06   kps     Added support for Rx Diversity
12/15/06   kps     Delete wl1_tcxo_freeze_request() and wl1_controls_tcxo().
11/21/06   kps     Add wl1_tcxo_freeze_request() and wl1_controls_tcxo()
11/13/06   kps     TCXO Rotator push support
                   Add wl1_tcxo_rpush()
11/06/06   kps     Add wl1_tcxo_release().
10/27/06   kps     Added support for setting restrictions in TCXOMGR 3.0
10/13/06   kps     Added basic support for TCXOMGR3.0, under FEATURE_WCDMA_TCXOMGR30
10/02/06   kps     Added declaration for wl1_trm_stop_cmd.
09/11/06   kps     Created file. Declaration of wl1_trm_request_primary_antenna.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"

#include "rfm.h"
#include "trm.h"
#include "l1def.h"
#include "l1rrcif.h"

#define WRM_MAX_CLIENTS 3
#define WRM_MAX_BANDS MAX_BAND_INFO 
/*Lock time for regular acquisition in ms*/
#define WL1_TRM_ACQ_LOCK_TIME_MSEC 100

/*Lock time for split acquisition in ms*/
 
#define WL1_TRM_SPLIT_ACQ_LOCK_TIME_MSEC 85 /*step1(30) + NASTT(40) +SW delay(15) ms with NOM clock level*/

/*Lock time for Raw Scan in ms*/
#define WL1_TRM_RAW_SCAN_LOCK_TIME_MSEC 160

/*Lock time for Fine Scan in ms*/
#define WL1_TRM_FINE_SCAN_LOCK_TIME_MSEC 65

/*Lock time for Additional Channel Scan in ms*/
#define WL1_TRM_ADD_CHNL_SCAN_LOCK_TIME_MSEC 100

/* Initial duration of trm request for primary antenna */
#define WL1_TRM_RXTX_MINIMUM_DURATION  TIMETICK_SCLK_FROM_MS(4000)

/* mutex for WL1 resources */
#define WL1_RES_MUTEXLOCK(i)   REX_ISR_LOCK(&(wl1_res_crit_sect[i]))
#define WL1_RES_MUTEXFREE(i)   REX_ISR_UNLOCK(&(wl1_res_crit_sect[i]))

typedef trm_duration_t wrm_duration_type;
typedef trm_time_t wrm_time_type;
typedef trm_frequency_info wrm_frequency_info;
typedef trm_channel_t wrm_channel_type;
typedef trm_unlock_event_enum_t wrm_unlock_event_enum_type;
typedef trm_irat_identifier wrm_irat_identifier;
typedef trm_client_enum_t wrm_client_enum_type;

/*============================================================================

CALLBACK WRM_GRANT_CALLBACK_TYPE

DESCRIPTION
  The prototype for event callback functions, used by WRM 
  to inform the clients of a grant.
  
DEPENDENCIES
  The callback will be called by WRM. It will called from L1 task context.

RETURN VALUE
  True - If grant is available.
  False - This is not possible at this point. 

SIDE EFFECTS
  None

============================================================================*/

typedef void (*wrm_grant_callback_type)(
  boolean grant
);

/*   WRM Resources */
typedef enum
{
  WCDMA_TRM,
  WCDMA_TRM_SECONDARY,
  WCDMA_TRM_CA,
  WCDMA_TRM_SECONDARY_CA,
  WCDMA_TRM_NUM_RESOURCES
} wrm_resource_enum_type;

typedef enum
{
  WRM_PRIMARY_CLIENT_L1,
  WRM_PRIMARY_CLIENT_RRC,
  WRM_PRIMARY_CLIENT_MAX
} wrm_primary_client_enum_type;

typedef enum
{
  WRM_SECONDARY_CLIENT_RXD,
  WRM_SECONDARY_CLIENT_MEAS,
  WRM_SECONDARY_CLIENT_NBR,
  WRM_SECONDARY_CLIENT_MAX
} wrm_secondary_client_enum_type;

/* enum for all the activities */
typedef enum
{
  /* WL1's activity's list */
  WL1_ACTIVITY_START = 0,
  WL1_ACTIVITY_SUSPEND = 1,
  WL1_ACTIVITY_RESUME = 2,
  WL1_ACTIVITY_IDLE = 3,
  WL1_ACTIVITY_STOP = 4,
  WL1_ACTIVITY_DEACTIVATE = 5,
  WL1_ACTIVITY_FS = 6,
  WL1_ACTIVITY_ACQ = 7,
  WL1_ACTIVITY_SETUP_SBCH = 8,
  WL1_ACTIVITY_SETUP_NBCH = 9,
  WL1_ACTIVITY_SETUP_PCH = 10,
  WL1_ACTIVITY_SETUP_CTCH_FACH = 11,
  WL1_ACTIVITY_SETUP_DTCH_FACH = 12,
  WL1_ACTIVITY_SETUP_DCH = 13,
  WL1_ACTIVITY_PAGING = 14,
  WL1_ACTIVITY_CELL_RESEL_START = 15,
  WL1_ACTIVITY_CELL_TRANS_REQ = 16,
  WL1_ACTIVITY_CHANNEL_ACTIVE = 17,
  WL1_ACTIVITY_DROP_CTCH = 18,
  WL1_ACTIVITY_BPLMN = 19,
  WL1_ACTIVITY_BPLMN_BCCH_DROP = 20,
  WL1_ACTIVITY_GEN_CPHY_SETUP_REQ = 21,
  WL1_ACTIVITY_G2W_TA_IDLE = 22,
  WL1_ACTIVITY_G2W_TA_CELL_SELECTION_REQ = 23,
  WL1_ACTIVITY_DRX = 24,
  WL1_ACTIVITY_REQUEST_PRIMARY = 25,
  WL1_ACTIVITY_DATA_CALL = 26,
  WL1_ACTIVITY_ACQ_LOW = 27,
  WL1_ACTIVITY_IDLE_NGBR_MEAS = 28,
  WL1_ACTIVITY_RX_DIVERSITY = 29,
  WL1_ACTIVITY_REQUEST_PRIMARY_CA = 30,
  WL1_ACTIVITY_REQUEST_SECONDARY_CA = 31,
  WL1_ACTIVITY_BCCH_DECODE = 32,

  /* RRC's activity's list */
  RRC_CELL_SELECTION = 33,
  RRC_VOICE_CALL = 34,
  RRC_REGISTRATION = 35,
  RRC_DATA_CALL = 36,

  WRM_MAX_ACTIVITIES
}wrm_activity_enum_type;

/* Structure for each activity's resource and reason */
typedef struct
{
  trm_resource_enum_t resource;
  trm_reason_enum_t reason;
}wrm_res_rsn_for_activity_type;

typedef struct
{
 l1_local_cmd_hdr_type local_cmd_hdr;
}wrm_ext_cmd_proc_resume_type;

/* mutex for WL1 resources */
extern rex_crit_sect_type wl1_res_crit_sect[WCDMA_TRM_NUM_RESOURCES];

/*Enum to identify WRM state*/
typedef enum {
WRM_NO_LOCK_STATE,
WRM_LOCK_REQ_STATE,
WRM_LOCK_RES_STATE,
WRM_IN_LOCK_STATE,
WRM_UNLOCK_IN_PROGRESS_STATE,
WRM_STATE_MAX
} wrm_state_enum_type;

/*Enum to identify client state in WRM*/
typedef enum {
WRM_CLI_NO_LOCK_STATE,
WRM_CLI_LOCK_REQ_STATE,
WRM_CLI_LOCK_RES_STATE,
WRM_CLI_IN_LOCK_STATE,
WRM_CLI_UNLOCK_IN_PROGRESS_STATE,
WRM_CLI_STATE_MAX
} wrm_client_state_enum_type;

/**Commands posted by WRM for which TRM will respond 
 **through an async callback**/
typedef enum {
  /*WRM posted request_and_notify command to TRM and waits for grant*/
  WRM_REQUEST_AND_NOTIFY,
  /*WRM posted modify chain state with param duration. Should get immediate grant or denial.*/
  WRM_MODIFY_DURATION,
  /*WRM posted modify chain state with param reason. Can get immediate or delayed grant.*/
  WRM_MODIFY_REASON,
  /*WRM got grant pending indicator. So it would expect a grant later.*/
  WRM_MODIFY_REASON_PENDING,
  /*WRM posted modify chain state with param band. Can get immediate or delayed grant.*/
  WRM_MODIFY_BAND,
  /*WRM got grant pending indicator. So it would expect a grant later.*/
  WRM_MODIFY_BAND_PENDING,
  /*WRM posted TRM exchange command and waits for a grant.*/
  WRM_EXCHANGE,
  WRM_MAX_COMMANDS
} wrm_command_enum_type;

/* WL1_WRM_GRANT_CALLBACK_CMD structure. */
typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  l1_local_cmd_hdr_type   hdr;

  wrm_resource_enum_type resource;

  /*Info about the command posted to TRM to
  **which the current async callback corresponds to*/
  wrm_command_enum_type posted_cmd;

  /*Callback type received for the command posted to TRM*/
  trm_async_callback_type_enum_t cb_type;

  trm_async_callback_data cb_data;

} wl1_wrm_async_callback_cmd_type;

typedef struct
{
  uint32             num_bands;
  wrm_frequency_info    bands[MAX_BAND_INFO];
} wrm_freq_input_type;


/*Structure to hold L1 and RRC specific information in WRM*/
typedef struct {
wrm_client_state_enum_type cli_state;

trm_duration_t req_dur;

trm_time_t res_starts_at;

trm_duration_t res_dur;

wrm_activity_enum_type  req_activity;

wrm_freq_input_type band_info;

wrm_grant_callback_type grant_cb;

} wrm_client_info_struct_type;

typedef struct {

  boolean valid;
  
  wrm_resource_enum_type resource;
  /**ID of the client requesting resource**/
  uint8 client_id;

  /**Activity for which resource is requested**/
  wrm_activity_enum_type activity;

  /**Bands for which resource is requested**/
  wrm_freq_input_type band_info;

  /**Duration for which resource is requested**/
  wrm_duration_type duration;

  /**Grant callback of the client**/
  wrm_grant_callback_type grant_callback;
  
  /**Flag to indicate if RF device swap is to be performed**/
  boolean wl1_device_swap_pending;
  
  /*Flag to indicate fake drx cmd is pending to be posted as CPHY was in progress during swap attempt.*/
  boolean wl1_fake_drx_pending;
  
  /*Flag to indicate if we faked RRC grant for a call setup*/
  boolean wl1_fake_grant_issued;
  
} wrm_cmd_struct_type;

typedef struct {
  trm_async_callback_type_enum_t cb_type;

  trm_async_callback_data data;

} wrm_rsp_struct_type;

/*Structure to hold WRM information*/
typedef struct {

  /**Current WRM state**/
wrm_state_enum_type wrm_state;

  /**Bitmask of active clients**/
  uint8 wrm_active_client_bmsk;

  /** Info about each client's state and request parameters**/
  wrm_client_info_struct_type wrm_client_info[WRM_MAX_CLIENTS];

  /**RFM Device ID allocated for this resource**/
  rfm_device_enum_type device_id;

  /**Bands currently allocated for this resource.**/
  wrm_freq_input_type band_info;

  /**Details of a WRM request and notify kept pending by WRM state machine.**/
  wrm_cmd_struct_type wrm_pending_cmd;

  /**Command posted by WRM to TRM for which WRM is waiting for a response.**/
  wrm_command_enum_type trm_cmd_posted;

  wrm_rsp_struct_type trm_cmd_posted_rsp;

} wrm_resource_info_struct_type ;

typedef struct {
  /**Number of active measurements**/
  uint8 num_active_measurements;
} wrm_meas_info_struct_type;

typedef struct {
  wrm_resource_info_struct_type wrm_resource_info[WCDMA_TRM_NUM_RESOURCES];
  wrm_meas_info_struct_type meas_db;
} wrm_db_struct_type;


typedef struct {
  uint8 unlock_time_ms;
  boolean (*unlock_cb)(wrm_unlock_event_enum_type);
}wrm_unlock_cb_type;

extern wrm_db_struct_type wrm_db;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Used when waiting for confirmations from mDSP and TRM */
extern rex_timer_type  wl1_timeout_timer;

/*Initialization macros*/
#define WRM_INVALID_TIME_DURATION 0
#define WRM_INVALID_RES_AT_TIME 0
#define WRM_INVALID_TAG 0xFF

/*Macros to check validity of WRM and WRM_TRM client identities*/
#define WRM_IS_TRM_CLIENT_ID_VALID(trm_client_id) (trm_client_id == TRM_UMTS)

/*Macros to check the state of WRM*/
#define WRM_STATE_IS_IN_LOCK(resource) (wrm_db.wrm_resource_info[resource].wrm_state == WRM_IN_LOCK_STATE)
#define WRM_STATE_IS_LOCK_REQ(resource) (wrm_db.wrm_resource_info[resource].wrm_state == WRM_LOCK_REQ_STATE)
#define WRM_STATE_IS_LOCK_RES(resource) (wrm_db.wrm_resource_info[resource].wrm_state == WRM_LOCK_RES_STATE)
#define WRM_STATE_IS_NO_LOCK(resource) (wrm_db.wrm_resource_info[resource].wrm_state == WRM_NO_LOCK_STATE)
#define WRM_STATE_IS_UNLOCK_IN_PROGRESS(resource) (wrm_db.wrm_resource_info[resource].wrm_state == WRM_UNLOCK_IN_PROGRESS_STATE)

/*Macros to check the state of WRM clients*/
#define WRM_CLI_STATE_IS_IN_LOCK(resource, wrm_client_id) (wrm_db.wrm_resource_info[resource].wrm_client_info[wrm_client_id].cli_state == WRM_CLI_IN_LOCK_STATE)
#define WRM_CLI_STATE_IS_LOCK_REQ(resource, wrm_client_id) (wrm_db.wrm_resource_info[resource].wrm_client_info[wrm_client_id].cli_state == WRM_CLI_LOCK_REQ_STATE)
#define WRM_CLI_STATE_IS_LOCK_RES(resource, wrm_client_id) (wrm_db.wrm_resource_info[resource].wrm_client_info[wrm_client_id].cli_state == WRM_CLI_LOCK_RES_STATE)
#define WRM_CLI_STATE_IS_NO_LOCK(resource, wrm_client_id) (wrm_db.wrm_resource_info[resource].wrm_client_info[wrm_client_id].cli_state == WRM_CLI_NO_LOCK_STATE)
#define WRM_CLI_STATE_IS_UNLOCK_IN_PROGRESS(resource, wrm_client_id) (wrm_db.wrm_resource_info[resource].wrm_client_info[wrm_client_id].cli_state == WRM_CLI_UNLOCK_IN_PROGRESS_STATE)

/*Macros to get and set the state of WRM*/
#define WRM_SET_WRM_STATE(resource, state_of_wrm) (wrm_db.wrm_resource_info[resource].wrm_state = state_of_wrm)
#define WRM_GET_WRM_STATE(resource) (wrm_db.wrm_resource_info[resource].wrm_state)

/*Macros to get and set the state of WRM client*/
#define WRM_SET_WRM_CLI_STATE(resource, wrm_client_id, wrm_cli_state) (wrm_db.wrm_resource_info[resource].wrm_client_info[wrm_client_id].cli_state = wrm_cli_state)
#define WRM_GET_WRM_CLI_STATE(resource, wrm_client_id) (wrm_db.wrm_resource_info[resource].wrm_client_info[wrm_client_id].cli_state)

/*Macros to get alternate WRM client id*/
#define WRM_GET_ALTERNATE_CLIENT_ID(wrm_client_id) (wrm_primary_client_enum_type)((wrm_client_id + (wrm_primary_client_enum_type)1) % WRM_PRIMARY_CLIENT_MAX)

#define WRM_IS_RESOURCE_VALID(resource) (resource < WCDMA_TRM_NUM_RESOURCES)

#define WRM_IS_CLIENT_ID_VALID(resource, wrm_client_id) \
         (((resource == WCDMA_TRM) && (wrm_client_id < WRM_PRIMARY_CLIENT_MAX)) || \
          ((resource == WCDMA_TRM_SECONDARY) && (wrm_client_id < WRM_SECONDARY_CLIENT_MAX)) || \
          ((resource == WCDMA_TRM_CA) && (wrm_client_id < WRM_PRIMARY_CLIENT_MAX)) || \
          ((resource == WCDMA_TRM_SECONDARY_CA) && (wrm_client_id < WRM_SECONDARY_CLIENT_MAX)) \
          )

/* Macro to find the activity for given proc */
#define WL1_DS_GET_ACTIVITY_FOR_PROC(ds_proc) (wl1_ds_proc_info[ds_proc].activity)

#define WRM_GET_REASON_FROM_ACTIVITY(activity)   wrm_res_rsn_for_activity[activity].reason  
#define WRM_GET_RESOURCE_FROM_ACTIVITY(activity) wrm_res_rsn_for_activity[activity].resource

#define WRM_GET_ACTIVE_CLIENT_BMSK(resource) \
             wrm_db.wrm_resource_info[resource].wrm_active_client_bmsk


#define WRM_SET_DEVICE_ID(resource, device) \
             (wrm_db.wrm_resource_info[resource].device_id = device)

#define WRM_GET_DEVICE_ID(resource) \
             (wrm_db.wrm_resource_info[resource].device_id)

#define WRM_INVALIDATE_BAND_INFO(resource) \
             (wrm_db.wrm_resource_info[resource].band_info.num_bands = 0)

#define WRM_GET_TRM_CMD_POSTED(resource) \
             (wrm_db.wrm_resource_info[resource].trm_cmd_posted)

#define WRM_SET_TRM_CMD_POSTED(resource, cmd) \
             (wrm_db.wrm_resource_info[resource].trm_cmd_posted = cmd)

#define WRM_INVALIDATE_TRM_CMD_POSTED(resource) \
             (wrm_db.wrm_resource_info[resource].trm_cmd_posted = WRM_MAX_COMMANDS)

#define WRM_ADD_CLIENT_TO_ACTIVE_CLI_BMSK(resource, client_id) \
             (wrm_db.wrm_resource_info[resource].wrm_active_client_bmsk |= (1 << client_id))

#define WRM_REMOVE_CLIENT_FROM_ACTIVE_CLI_BMSK(resource, client_id) \
             (wrm_db.wrm_resource_info[resource].wrm_active_client_bmsk = \
               wrm_db.wrm_resource_info[resource].wrm_active_client_bmsk & (~(1 << client_id)))

#define WRM_GET_COMMAND_PENDING(resource) \
             (wrm_db.wrm_resource_info[resource].wrm_pending_cmd)
             
#define WRM_SET_COMMAND_PENDING(resource, pending_cmd) \
             (wrm_db.wrm_resource_info[resource].wrm_pending_cmd = *pending_cmd)

#define WRM_INVALIDATE_PENDING_CMD(resource) \
  do {\
   wrm_db.wrm_resource_info[resource].wrm_pending_cmd.valid = FALSE; \
   wrm_db.wrm_resource_info[resource].wrm_pending_cmd.wl1_device_swap_pending = FALSE; \
  }while(0)

#define WRM_IS_CMD_PENDING_FOR_CLIENT(resource, wrm_client_id) \
          ((wrm_db.wrm_resource_info[resource].wrm_pending_cmd.valid == TRUE) &&   \
            (wrm_db.wrm_resource_info[resource].wrm_pending_cmd.client_id == wrm_client_id))

#define WRM_RRC_CMD_PENDING() (WRM_IS_CMD_PENDING_FOR_CLIENT(WCDMA_TRM, WRM_PRIMARY_CLIENT_RRC))

#define WRM_IS_ANY_CMD_PENDING() \
  (WRM_IS_CMD_PENDING_FOR_CLIENT(WCDMA_TRM,WRM_PRIMARY_CLIENT_RRC)||  \
  WRM_IS_CMD_PENDING_FOR_CLIENT(WCDMA_TRM,WRM_PRIMARY_CLIENT_L1))
  

#define WRM_IS_ANY_CLI_IN_LOCK_REQ_STATE(resource) 0

#define WRM_DOES_ACTIVITY_NEED_TX(activity) \
  ((activity == RRC_VOICE_CALL) || \
   (activity == RRC_REGISTRATION) || \
   (activity == RRC_DATA_CALL) || \
   (activity == WL1_ACTIVITY_SETUP_DTCH_FACH))

#define WRM_IS_SWAP_PENDING_FOR_CLIENT(resource) \
  (wrm_db.wrm_resource_info[resource].wrm_pending_cmd.wl1_device_swap_pending == TRUE)

#define WRM_SET_SWAP_PENDING_FOR_CLIENT(resource, value) \
  (wrm_db.wrm_resource_info[resource].wrm_pending_cmd.wl1_device_swap_pending = value)

#define WRM_GET_CURRENT_ACTIVITY_FOR_CLIENT(resource,client) \
  (wrm_db.wrm_resource_info[resource].wrm_client_info[client].req_activity)

#define WRM_IS_ACTIVITY_VALID(activity) \
  (activity < WRM_MAX_ACTIVITIES)

#define WRM_SET_CLIENT_ACTIVITY(resource, client, activity)\
  (wrm_db.wrm_resource_info[resource].wrm_client_info[client].req_activity = activity)

#define WRM_SET_FAKE_DRX_PENDING_FOR_CLIENT(value) \
  (wrm_db.wrm_resource_info[WCDMA_TRM].wrm_pending_cmd.wl1_fake_drx_pending = value)

#define WRM_IS_FAKE_DRX_PENDING_FOR_CLIENT() \
  (wrm_db.wrm_resource_info[WCDMA_TRM].wrm_pending_cmd.wl1_fake_drx_pending == TRUE)

#define WRM_SET_FAKE_GRANT_PENDING_FOR_CLIENT(value) \
  (wrm_db.wrm_resource_info[WCDMA_TRM].wrm_pending_cmd.wl1_fake_grant_issued = value)

#define WRM_IS_FAKE_GRANT_PENDING_FOR_CLIENT() \
  (wrm_db.wrm_resource_info[WCDMA_TRM].wrm_pending_cmd.wl1_fake_grant_issued == TRUE)

#define WL1_IS_QTA_CONFIGURED() (wl1_preparing_for_qta() || wl1_is_wfw_in_qta_gap())

/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/


#ifdef FEATURE_WCDMA_TRM

/*** maximum channels in a pirticular band ***/
#define MAX_CHANNELS_IN_BAND 352

/* Array to store the channels information  */
extern uint32 *channels_list;

/*structure for holding Band information ***/
typedef struct
{
  sys_band_class_e_type band;
  uint16 min_channel_number;
  uint16 max_channel_number;
} trm_band_map_struct_type;

/* Maps l1_freq_scan_band_enum_type to sys_band_class_e_type for TRM request*/
extern trm_band_map_struct_type trm_map_l1_band_type[L1_FREQ_WCDMA_BAND_NONE];

#ifdef FEATURE_WCDMA_DUAL_ACTIVE
/* Run-time debug flag.  TRUE means call trm_get_rf_device() for RF device.  FALSE
** means old way, hard-code to RFCOM_TRANSCEIVER_0. */
extern boolean wl1_trm_use_rf_device_from_trm;
#endif

/*===========================================================================
FUNCTION     WL1_TRM_GET_PRIMARY_RF_DEVICE

DESCRIPTION
  This function is invoked everytime we make a call into RFM.  All RFM APIs
  take RF device as a parameter.

  When we got TRM lock for WCDMA primary operation earlier, we called
  trm_get_rf_device().  We stored that value away to be returned by this
  function now.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  rfm_device_enum_type - RF device in use by primary WCDMA operation.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
rfm_device_enum_type wl1_trm_get_primary_rf_device(void)
{
#ifdef FEATURE_WCDMA_DUAL_ACTIVE
  rfm_device_enum_type  rfm_device = RFCOM_TRANSCEIVER_0;


  if (wl1_trm_use_rf_device_from_trm)
  {
    rfm_device = wrm_db.wrm_resource_info[WCDMA_TRM].device_id;
  }

  return rfm_device;
#else
  rfm_device_enum_type  rfm_device = wrm_db.wrm_resource_info[WCDMA_TRM].device_id;

  return rfm_device;


#endif  /* #ifdef FEATURE_WCDMA_DUAL_ACTIVE */
}


/*===========================================================================
FUNCTION     WL1_TRM_GET_PRIMARY_RF_DEVICE_FOR_CA

DESCRIPTION
  This function provides primary RF device ID for Carrier Aggregation (DBDC)

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  rfm_device_enum_type - RF device in use by primary WCDMA operation.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
rfm_device_enum_type  wl1_trm_get_primary_rf_device_for_ca(void)
{
  rfm_device_enum_type  rfm_device = wrm_db.wrm_resource_info[WCDMA_TRM_CA].device_id;

    if (rfm_device >= RFM_INVALID_DEVICE)
    {
      /* This should, in theory, never happen.  For some reason we are making
      ** a call to RFM, but we don't know the RF device in use for W primary
      ** operation.  This could happen if we end up calling RFM API before we
      ** have primary TRM lock.  Anyway, for now hard-code default. */
    WL1_MSG0(LOW, "WL1_TRM: Primary RF device for CA invalid!");
  }

  return rfm_device;
}


/*===========================================================================
FUNCTION     WL1_TRM_GET_SECONDARY_RF_DEVICE

DESCRIPTION
  This function is invoked everytime we make a call into RFM.  All RFM APIs
  take RF device as a parameter.

  When we got TRM lock for WCDMA secondary operation earlier, we called
  trm_get_rf_device().  We stored that value away to be returned by this
  function now.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  rfm_device_enum_type - RF device in use by secondary WCDMA operation.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
rfm_device_enum_type wl1_trm_get_secondary_rf_device(void)
{
#ifdef FEATURE_WCDMA_DUAL_ACTIVE
  rfm_device_enum_type  rfm_device = RFCOM_RECEIVER_DIV;


  if (wl1_trm_use_rf_device_from_trm)
  {
    rfm_device = wrm_db.wrm_resource_info[WCDMA_TRM_SECONDARY].device_id;
  }

  return rfm_device;
#else
  rfm_device_enum_type	rfm_device = wrm_db.wrm_resource_info[WCDMA_TRM_SECONDARY].device_id;
  
  return rfm_device;

#endif  /* #ifdef FEATURE_WCDMA_DUAL_ACTIVE */
}

/*===========================================================================
FUNCTION     WL1_TRM_GET_SECONDARY_RF_DEVICE_FOR_CA

DESCRIPTION
  This function return secondary rf device for carrier aggregation.
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  rfm_device_enum_type - RF device in use by secondary WCDMA operation.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
rfm_device_enum_type wl1_trm_get_secondary_rf_device_for_ca(void)
{
  rfm_device_enum_type  rfm_device = wrm_db.wrm_resource_info[WCDMA_TRM_SECONDARY_CA].device_id;
    
  return rfm_device;
}

/*===========================================================================
FUNCTION     WL1_TRM_GET_RF_DEVICE_FOR_TARGET

DESCRIPTION
  This function return rf device for target.
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  rfm_device_enum_type - RF device in use for target.

SIDE EFFECTS
  None.
===========================================================================*/
extern rfm_device_enum_type wl1_trm_get_rf_device_for_target(uint16 freq);

/*===========================================================================
FUNCTION     WL1_TRM_GET_BAND

DESCRIPTION
  This function returns the band channel belongs to 

DEPENDENCIES
  None.

PARAMETERS
  uint16 channel - The channel we are tuning to

RETURN VALUE
  l1_freq_scan_band_enum_type - Enum type of the band.

SIDE EFFECTS
  None.
===========================================================================*/

extern l1_freq_scan_band_enum_type wl1_trm_get_band(uint16 channel);


/*===========================================================================
FUNCTION     WL1_TRM_CARRIER_AGGREGATION_ON

DESCRIPTION
  This function checks if it is DBDC call or normal DC call

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
boolean to indicate if DBDC call or not

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean
wl1_trm_carrier_aggregation_on
(void);

/*===========================================================================
FUNCTION     WL1_TRM_RESERVE_CARRIER_AGGREGATION_RESOURCES

DESCRIPTION
  This function reserves TRM for Carrier Aggregation

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
boolean to indicate if DBDC call or not

SIDE EFFECTS
  None.
===========================================================================*/
extern void 
wl1_trm_reserve_carrier_aggregation_resources
(uint16 uarfcn);

/*===========================================================================
FUNCTION     WL1_TRM_RELEASE_CARRIER_AGGREGATION_RESOURCES

DESCRIPTION
  This function reserves TRM for Carrier Aggregation

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
boolean to indicate if DBDC call or not

SIDE EFFECTS
  None.
===========================================================================*/
extern void 
wl1_trm_release_carrier_aggregation_resources
(void);

#endif /* FEATURE_WCDMA_TRM */

/*===========================================================================
FUNCTION     WL1_TRM_GET_CURRENT_ALLOCATED_BAND

DESCRIPTION
  The Function 

DEPENDENCIES
  None.

PARAMETERS
  trm_client_enum_t      client - The client which is being informed of an event

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern rfcom_wcdma_band_type wl1_trm_get_current_allocated_band(void);


/*===========================================================================
FUNCTION     WL1_TRM_INIT

DESCRIPTION
  This function initializes TRM database
  
DEPENDENCIES
  None.

PARAMETERS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_trm_init( void );


/*===========================================================================
FUNCTION     WRM_UNLOCK_TRM_CB

DESCRIPTION
  This function is called by TRM when WCDMA needs to release the lock for
  the sake of higher priority activity in other clients.

DEPENDENCIES
  WCDMA must be holding an RF resource lock

PARAMETERS
  unlock_data : Data for unlock callback advanced.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wrm_unlock_trm_cb(
  trm_unlock_callback_data* unlock_data
);

/*===========================================================================
FUNCTION     wrm_handle_pending_request

DESCRIPTION
  This function handles any pending request_and_notify command.
  In some scenarios like say measurements is in progress and RxD asks for lock,
  we would have kept RxD's request pending till meas releases the lock.
    
DEPENDENCIES
  None

PARAMETERS
  resource: The resource for which request_and_notify is pending

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wrm_handle_pending_request(
  wrm_resource_enum_type resource
);

/*===========================================================================
FUNCTION     wrm_post_local_cmd_for_pending_wrm_cmd

DESCRIPTION
  This function posts a local command for handling a pending request_and_notify.
  In some scenarios like say measurements is in progress and RxD asks for lock,
  we would have kept RxD's request pending till meas releases the lock.
    
DEPENDENCIES
  None

PARAMETERS
  resource: The resource for which request_and_notify is pending

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wrm_post_local_cmd_for_pending_wrm_cmd(
  wrm_resource_enum_type resource_id
  );


/*============================================================================
FUNCTION WRM_REQUEST_PRIMARY_ANTENNA

DESCRIPTION
  Specifies the given client needs primary antenna, for the
  given duration, for the supplied activity. The resource request is immediately
  evaluated, and the result returned.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested

RETURN VALUE
  True - Resource is granted.
  False - Resource is denied.

SIDE EFFECTS
  If the client currently holds an RF chain, that chain is released before
  the request is evaluated.
============================================================================*/

extern boolean wrm_request_primary_antenna
(
  /* The client which needs the RF resource */
  wrm_primary_client_enum_type wrm_client_id,

  /* Activity for which RF resource which is being requested */
  wrm_activity_enum_type              activity,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  wrm_freq_input_type band_info
);

/*============================================================================
FUNCTION WRM_REQUEST_PRIMARY_ANTENNA_FOR_CA

DESCRIPTION
  Specifies the given client needs primary antenna for DBDC, for the
  given duration, for the supplied activity. The resource request is immediately
  evaluated, and the result returned.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested

RETURN VALUE
  True - Resource is granted.
  False - Resource is denied.

SIDE EFFECTS
  If the client currently holds an RF chain, that chain is released before
  the request is evaluated.
============================================================================*/
extern boolean wrm_request_primary_antenna_for_ca
(
  /* The client which needs the RF resource */
  wrm_primary_client_enum_type wrm_client_id,

  /* Activity for which RF resource which is being requested */
  wrm_activity_enum_type  activity,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type  duration,

  /*Bands for which RF resource which is being requested */
  wrm_freq_input_type band_info
);

/*===========================================================================
FUNCTION     WRM_REQUEST_AND_NOTIFY_PRIMARY_ANTENNA

DESCRIPTION
  This function makes an asynchronous lock request to TRM for primary antenna.If WRM 
  already has TRM lock, it is provided immediately to the caller. WRM never times out 
  in waiting for the TRM lock. The caller of wrm_request_and_notify() should should 
  implement any timer for the call.If the caller decides to timeout the request, the 
  caller should call wrm_release_primary_antenna to cancel the pending request.

DEPENDENCIES
  None.

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested
  grant_callback: Callback to notify client when resource is granted by TRM at 
                  a later point.
  wrm_prio_inv_req_n_not: to indicate that the req_n_not is getting called 
                          due to priority_inversion_timer expiry

RETURN VALUE
  True  - Resource is granted. The grant callback will NOT be called in this case.
  False - Resource is pending grant. The grant callback will be called when MCS gives
          the grant.

SIDE EFFECTS
  None.
===========================================================================*/

extern boolean wrm_request_and_notify_primary_antenna
(
  /* The client which needs the RF resource */
  wrm_primary_client_enum_type wrm_client_id,

  /* Activity for which RF resource which is being requested */
  wrm_activity_enum_type              activity,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  wrm_freq_input_type band_info,

  /* Callback to notify client when resource is granted */
  wrm_grant_callback_type         grant_callback,

 /* Used to indicate that the req_n_not is getting called due to priority_inversion_timer expiry */
  boolean                         wrm_prio_inv_req_n_not
);

/*============================================================================
FUNCTION WRM_RELEASE_PRIMARY_ANTENNA

DESCRIPTION
  This function releases the lock for primary antenna previously acquired.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs to cancel

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/

extern void wrm_release_primary_antenna (
  wrm_primary_client_enum_type wrm_client_id
);

/*============================================================================
FUNCTION WRM_RELEASE_PRIMARY_ANTENNA_FOR_CA

DESCRIPTION
  This function releases the lock for primary antenna previously acquired for DBDC.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs to cancel

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
extern void wrm_release_primary_antenna_for_ca (
  /* The client releasing RF resource  */
  wrm_primary_client_enum_type wrm_client_id
);

/*===========================================================================
FUNCTION     WRM_RESERVE_PRIMARY_ANTENNA

DESCRIPTION
  Specifies the given client needs the primary antenna at the given
  time, for the given duration, for the supplied activity.

    
DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  when: When the resource will be needed (sclks timestamp)
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested

RETURN VALUE
  None

SIDE EFFECTS
  If the client currently holds an RF chain, the chain is released.
===========================================================================*/
extern void wrm_reserve_primary_antenna(
  /* The client which needs the RF resource */
  wrm_primary_client_enum_type               wrm_client_id,

  wrm_activity_enum_type activity,

  /* When the resource will be needed (sclks timestamp) */
  wrm_time_type                      when,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  wrm_freq_input_type band_info
);

/*============================================================================
FUNCTION WRM_RETAIN_LOCK_FOR_PRIMARY

DESCRIPTION
  Informs the Transceiver Resource Manager that the calling client wants to hold
  primary resource indefinitely.  TRM may inform the WRM that it must
  give up the lock through the registered async callback.
  
DEPENDENCIES
  The calling client must be holding a transceiver resource lock

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
extern void wrm_retain_lock_for_primary(void);

/*============================================================================
FUNCTION WRM_RETAIN_LOCK_FOR_SECONDARY

DESCRIPTION
  Informs the Transceiver Resource Manager that the calling client wants to hold
  primary resource indefinitely.  TRM may inform the WRM that it must
  give up the lock through the registered async callback.
  
DEPENDENCIES
  The client must be holding a transceiver resource lock

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/

extern void wrm_retain_lock_for_secondary(void);

/*============================================================================
FUNCTION WRM_MODIFY_DURATION_FOR_PRIMARY_ANTENNA

DESCRIPTION
 Attempts to extend the duration an RF resource lock is held for. If the entire
 extension can be granted, it will be granted. If the entire extension cannot be 
 granted, the lock duration remains unchanged, and the client should release the 
 lock at the original lock expiry point. If the client had originally locked the 
 resource for longer than the required extension, the lock will remain the original
 length and the extension will be INDICATED AS BEING GRANTED. The extension is all 
 or nothing.  
 
DEPENDENCIES
  The client must be holding an RF resource lock

PARAMETERS
  wrm_client_id: The client which needs to cancel
  new_duration: The required extension, in sclks, from "now"

RETURN VALUE
  TRUE if the lock duration extends from "now" to "now + maximum".
  FALSE if the lock duration is unchanged.

SIDE EFFECTS
  None
============================================================================*/

extern boolean wrm_modify_duration_for_primary_antenna(
  wrm_primary_client_enum_type wrm_client_id,
  wrm_duration_type new_duration
);

/*============================================================================
FUNCTION WRM_MODIFY_FREQ

DESCRIPTION
 This requests for grant for a new frequency. If the grant is given immediately,
 return value will be TRUE. If TRM sends a pending status, return value will be 
 FALSE and the grant callback will be called when TRM gives the grant. The caller 
 is responsible for starting a lock wait timer  if necessary. 
 
DEPENDENCIES
  The client must be holding an RF resource lock.

PARAMETERS
  channel: The channel to which the client needs to tune.
  grant_callback: Callback to notify client when resource is granted by TRM at 
                  a later point.

RETURN VALUE
  TRUE if the lock duration extends from "now" to "now + maximum".
  FALSE if the lock duration is unchanged.

SIDE EFFECTS
  None
============================================================================*/
extern boolean wrm_modify_freq(
  l1_freq_scan_band_enum_type band,
  wrm_resource_enum_type resource,
  uint8 wrm_client_id,
  wrm_grant_callback_type grant_callback
);

/*===========================================================================
FUNCTION     WRM_MODIFY_ACTIVITY

DESCRIPTION
  This requests for grant for a new activity. If the grant is given immediately, return value will be TRUE. 
  If there is a DR<->hybrid transition, return value will be FALSE and the grant callback will be called when
  TRM gives the grant.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The new activity for which lock is being requested
  min_duration : The duration of the activity
  grant_callback : The callback to be called for the grant
  cmd_params : params to be saved in case the cmd is pended

RETURN VALUE
  True - If lock is granted immediately
  False - If lock is not available immediately. In that case, grant callback will be called when lock
            is granted.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wrm_modify_activity(
  wrm_primary_client_enum_type wrm_client_id,
  wrm_activity_enum_type activity,
  /*Minimum time needed to release the lock if there is an unlock by request from TRM*/
  wrm_duration_type      min_duration, 
  wrm_grant_callback_type grant_callback,
  wrm_cmd_struct_type* cmd_params_ptr
  );
/*============================================================================
FUNCTION WRM_REQUEST_SECONDARY_ANTENNA

DESCRIPTION
  Specifies the given client needs secondary chain, for the given duration, for
  the supplied reason. The resource request is immediately evaluated, and the 
  result returned.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested

RETURN VALUE
  True - Resource is granted.
  False - Resource is denied.

SIDE EFFECTS
  If the client currently holds an RF chain, that chain is released before
  the request is evaluated.
============================================================================*/
extern boolean wrm_request_secondary_antenna
(
  /* The client which needs the RF resource */
  wrm_secondary_client_enum_type wrm_client_id,

  /* Activity for which RF resource which is being requested */
  wrm_activity_enum_type              activity,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  wrm_freq_input_type band_info
);

/*============================================================================
FUNCTION WRM_REQUEST_SECONDARY_ANTENNA

DESCRIPTION
  Specifies the given client needs secondary antenna for DBDC, for the
  given duration, for the supplied activity. The resource request is immediately
  evaluated, and the result returned.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested

RETURN VALUE
  True - Resource is granted.
  False - Resource is denied.

SIDE EFFECTS
  If the client currently holds an RF chain, that chain is released before
  the request is evaluated.
============================================================================*/

extern boolean wrm_request_secondary_antenna_for_ca
(
  /* The client which needs the RF resource */
  wrm_secondary_client_enum_type wrm_client_id,

  /* Activity for which RF resource which is being requested */
  wrm_activity_enum_type              activity,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  /*Bands for which RF resource which is being requested */
  wrm_freq_input_type band_info
);

/*===========================================================================
FUNCTION     WRM_REQUEST_AND_NOTIFY_SECONDARY_ANTENNA

DESCRIPTION
  This function makes an asynchronous lock request to TRM for secondary antenna.If WRM 
  already has TRM lock, it is provided immediately to the caller. WRM never times out 
  in waiting for the TRM lock. The caller of wrm_request_and_notify() should should 
  implement any timer for the call.If the caller decides to timeout the request, the 
  caller should call wrm_release_primary_antenna to cancel the pending request.

DEPENDENCIES
  None.

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested
  grant_callback: Callback to notify client when resource is granted by TRM at 
                  a later point.

RETURN VALUE
  True  - Resource is granted. The grant callback will NOT be called in this case.
  False - Resource is pending grant. The grant callback will be called when MCS gives
          the grant.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wrm_request_and_notify_secondary_antenna
(
  /* The client which needs the RF resource */
  wrm_secondary_client_enum_type wrm_client_id,

  /* Activity for which RF resource which is being requested */
  wrm_activity_enum_type              activity,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  wrm_freq_input_type band_info,

  /* Callback to notify client when resource is granted */
  wrm_grant_callback_type         grant_callback
);

/*============================================================================
FUNCTION WRM_RELEASE_SECONDARY_ANTENNA

DESCRIPTION
  This function releases the lock for primary antenna previously acquired.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs to cancel

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/

extern void wrm_release_secondary_antenna (
  wrm_secondary_client_enum_type wrm_client_id
);

/*============================================================================
FUNCTION WRM_RELEASE_SECONDARY_ANTENNA_FOR_CA

DESCRIPTION
  This function releases the lock for secondary antenna previously acquired for DBDC.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs to cancel

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/

extern void wrm_release_secondary_antenna_for_ca (
  /* The client releasing RF resource */
  wrm_secondary_client_enum_type wrm_client_id
);

/*===========================================================================
FUNCTION     WRM_RESERVE_SECONDARY_ANTENNA

DESCRIPTION
  Specifies the given client needs the primary antenna at the given
  time, for the given duration, for the supplied activity.

    
DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  when: When the resource will be needed (sclks timestamp)
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested

RETURN VALUE
  None

SIDE EFFECTS
  If the client currently holds an RF chain, the chain is released.
===========================================================================*/
extern void wrm_reserve_secondary_antenna(
  /* The client which needs the RF resource */
  wrm_secondary_client_enum_type               wrm_client_id,

  wrm_activity_enum_type activity,

  /* When the resource will be needed (sclks timestamp) */
  wrm_time_type                      when,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  wrm_freq_input_type band_info
);

/*===========================================================================
FUNCTION     wrm_request_resources_for_secondary

DESCRIPTION
Allocates buffers and enables clocks for an RF call to entermode
it then calls entermode for the rf chain

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wrm_request_resources_for_secondary(void);

/*===========================================================================
FUNCTION     wrm_release_resources_for_secondary

DESCRIPTION
Drops RxD clocks and places RF call to sleep

DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wrm_release_resources_for_secondary(void);

/*============================================================================
FUNCTION WRM_REQUEST_PRIMARY_ANTENNA_FOR_CA

DESCRIPTION
  Specifies the given client needs primary CA antenna, for the
  given duration, for the supplied activity. The resource request is immediately
  evaluated, and the result returned.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested

RETURN VALUE
  True - Resource is granted.
  False - Resource is denied.

SIDE EFFECTS
  If the client currently holds an RF chain, that chain is released before
  the request is evaluated.
============================================================================*/

extern boolean wrm_request_primary_antenna_for_ca
(
  /* The client which needs the RF resource */
  wrm_primary_client_enum_type wrm_client_id,

  /* Activity for which RF resource which is being requested */
  wrm_activity_enum_type              activity,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  wrm_freq_input_type band_info
);

/*============================================================================
FUNCTION WRM_RELEASE_PRIMARY_ANTENNA_FOR_CA

DESCRIPTION
  This function releases the lock for primary CA antenna previously acquired.

DEPENDENCIES
  None	

PARAMETERS
  wrm_client_id: The client which needs to cancel

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/

extern void wrm_release_primary_antenna_for_ca (
  wrm_primary_client_enum_type wrm_client_id
);

/*============================================================================
FUNCTION WRM_REQUEST_SECONDARY_ANTENNA_FOR_CA

DESCRIPTION
  Specifies the given client needs secondary CA chain, for the given duration, for
  the supplied reason. The resource request is immediately evaluated, and the 
  result returned.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs the RF resource
  activity: The activity for which lock is being requested
  duration: How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested

RETURN VALUE
  True - Resource is granted.
  False - Resource is denied.

SIDE EFFECTS
  If the client currently holds an RF chain, that chain is released before
  the request is evaluated.
============================================================================*/
extern boolean wrm_request_secondary_antenna_for_ca
(
  /* The client which needs the RF resource */
  wrm_secondary_client_enum_type wrm_client_id,

  /* Activity for which RF resource which is being requested */
  wrm_activity_enum_type              activity,

  /* How long the resource will be needed for (in sclks) */
  wrm_duration_type                  duration,

  wrm_freq_input_type band_info
);

/*============================================================================
FUNCTION WRM_RELEASE_SECONDARY_ANTENNA_FOR_CA

DESCRIPTION
  This function releases the lock for primary antenna previously acquired.

DEPENDENCIES
  None

PARAMETERS
  wrm_client_id: The client which needs to cancel

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/

extern void wrm_release_secondary_antenna_for_ca (
  wrm_secondary_client_enum_type wrm_client_id
);
/*===========================================================================
FUNCTION     WRM_START_MEASUREMENT

DESCRIPTION
  This function calls trn_set_client_state API to request grant for performing the given type of
  measurement for the requested duration on the given bands. Irat_id assigned by TRM will be filled by
  this API.

DEPENDENCIES
  None

PARAMETERS
  target_rat: The RAT on which measurement is to be performed.
  duration  : How long the resource will be needed for (in sclks) 
  band_info: Band for which resource is being requested
  irat_id    : ID given by TRM for this measurement. 
  div_needed : if diversity is needed for IRAT or not
  diversity_granted: whether div is granted by TRM or not
  meas_primary_device_id: Primary device id.
  meas_div_device_id: diversity device id.
  
RETURN VALUE
  True - If there is grant for the measurement
  False - If lock is denied.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wrm_start_measurement(
  wrm_client_enum_type target_rat,
  wrm_duration_type duration,
  wrm_freq_input_type band_info,
  wrm_irat_identifier *irat_id,
  boolean is_diversity_needed,
  boolean *diversity_granted,
  rfm_device_enum_type *meas_primary_device_id,
  rfm_device_enum_type *meas_div_device_id
);

/*===========================================================================
FUNCTION     WRM_COMPLETE_MEASUREMENT

DESCRIPTION
This function calls trm_set_client state API for releasing the grant taken with the given irat id for the
given type of measurement on the given bands.

DEPENDENCIES
  None

PARAMETERS
  irat_id    : ID given by TRM for this measurement.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wrm_complete_measurement(
  wrm_irat_identifier irat_id
);


/*============================================================================
FUNCTION WRM_PRIMARY_RF_DEVICE_SUPPORTS_TX

DESCRIPTION
  API to indicate if the primary RF device currently held supports Tx.

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
  True - If primary RF device supports Tx.
  False -Else

SIDE EFFECTS
  None
============================================================================*/
extern boolean wrm_primary_rf_device_supports_tx(void);

/*===========================================================================
FUNCTION     WRM_INIT

DESCRIPTION
  This function must be called during UE power-up to initialize the WRM globals.
  the sake of higher priority activity in other clients.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wrm_init(void);

/*===========================================================================
FUNCTION     wrm_trm_get_band

DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern wrm_freq_input_type wrm_trm_get_band(uint16 channel);

/*===========================================================================
FUNCTION     WL1TRM_IS_UE_IN_DR_MODE

DESCRIPTION
  This function returns whether UE is in DR mode or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
============================================================================*/

extern boolean wl1trm_is_ue_in_dr_mode(void);



/*===========================================================================
FUNCTION     wl1trm_is_ue_dr_capable

DESCRIPTION
  This function returns whether the HW is capable of DR operation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
============================================================================*/

extern boolean wl1trm_is_ue_dr_capable(void);


/*===========================================================================
FUNCTION wrm_register_callback

DESCRIPTION


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void wrm_register_callback(void);

/*===========================================================================
FUNCTION wrm_deregister_callback

DESCRIPTION


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void wrm_deregister_callback(void);
/*===========================================================================
FUNCTION     wl1_ds_release_resources_for_secondary

DESCRIPTION
Deallocates rxlm buffers an RF call to sleep

DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wl1_ds_release_rf_resources_for_secondary(void);
/*===========================================================================
FUNCTION wrm_update_QTA_state

DESCRIPTION


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern boolean wrm_update_qta_state(trm_state_oper_enum_type state_operation, trm_client_enum_t tuning_in_tech);
/*===========================================================================
FUNCTION wrm_tune_done_callback

DESCRIPTION


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void wrm_tune_done_callback(wrm_client_enum_type client);

/*===========================================================================
FUNCTION wrm_compare_priorities

DESCRIPTION


DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern trm_priority_comparison_enum_type wrm_compare_priorities
(
  /* Cleint Id */
  trm_client_enum_t       client,

  /* First activity to compare */
  wrm_activity_enum_type       activity1,

  /* Second activity to compare */
  wrm_activity_enum_type       activity2
);


/*===========================================================================
FUNCTION     WRM_GRANT_CALLBACK_CMD_HANDLER

DESCRIPTION
  Pending

DEPENDENCIES
  None

PARAMETERS
  Pending

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wrm_async_callback_cmd_handler(wl1_wrm_async_callback_cmd_type *async_cb);

/*===========================================================================
FUNCTION wrm_modify_freq_default_callback

DESCRIPTION
Default callback function for wrm_modify_freq 

DEPENDENCIES
None

PARAMETERS
boolean grant, Not used

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
extern void wrm_modify_freq_default_grant_callback(boolean grant);

/*===========================================================================
FUNCTION wrm_modify_freq_wait_for_grant

This function is called to wait on TRM grant  when immediate grant for modify_freq is not given. 

DEPENDENCIES
None

RETURN VALUE
Boolean: True if grant is given

SIDE EFFECTS
None
===========================================================================*/
extern boolean wrm_modify_freq_wait_for_grant(void);

/*===========================================================================
FUNCTION WRM_TRIGGER_DUAL_TO_SINGLE_RECEIVE

DESCRIPTION
  This function is called to trigger from DR-DSDS to SR-DSDS mode. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
===========================================================================*/
extern void wrm_trigger_dual_to_single_receive(void);

/*===========================================================================
FUNCTION WRM_TRIGGER_SINGLE_TO_DUAL_RECEIVE

DESCRIPTION
  This function is called to trigger from SR-DSDS to DR-DSDS mode. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
extern void wrm_trigger_single_to_dual_receive(void);

/*===========================================================================
FUNCTION     wrm_req_trm_lock_for_freq_scan

DESCRIPTION
  This function request TRM lock for frequency scan or raw scan tune

DEPENDENCIES
  None.

PARAMETERS
  Current band to tune to

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wrm_req_trm_lock_for_freq_scan
(
  l1_freq_scan_band_enum_type band
);

/*===========================================================================
FUNCTION    WRM_RRC_REQ_AND_NOTIFY_LOCK

DESCRIPTION
  Function is as a wrapper to wl1idletamgr lock req api to handle ACQ priority inversion in L1. 

DEPENDENCIES
  None.

PARAMETERS
  activity - Type of activity for which lock is needed
  duration - How long the resource will be needed for (in sclks)
  grant_callback - Callback to notify client when resource is granted
  band_info -  Band info

RETURN VALUE
  Boolean. FALSE -> Lock failure , TRUE -> Lock successful
===========================================================================*/
extern boolean wrm_rrc_req_and_notify_lock (
  /* Type of activity for which lock is needed */
  wrm_activity_enum_type activity,

  /* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration,

  /* Callback to notify client when resource is granted */
  wrm_grant_callback_type            grant_callback,

  /* Band info */
  wrm_freq_input_type band_info
);

#endif /* WL1TRM_H */
