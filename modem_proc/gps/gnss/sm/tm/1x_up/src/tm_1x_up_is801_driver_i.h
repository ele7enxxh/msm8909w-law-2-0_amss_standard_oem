#ifndef TM_1X_UP_TRUSTED_H
#define TM_1X_UP_TRUSTED_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM_1X_UP Trusted Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for TM 1X UP Trusted  module

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/src/tm_1x_up_is801_driver_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/14   sj      Fix Memory Leak when Is801 aborts.
01/03/11   gk      Eph throttling rework
08/14/07   ank     Inform MGP about nonexistent SVs.
06/15/07    ank    Added tm_1x_up_msb_op_e_type.
05/27/07   rw      Add new wait state and extern function for JGPS (KDDI) module
12/20/06   ank     Added wait_ref_loc state.
11/15/06   ank     Removed tm_1x_up_read_msid_fields.
10/25/06   ank     Initial Release
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "gps_common.h"
#include "sm_api.h"
#include "aries_os_globals.h"
#include "pdapi.h"
#include "tm_is801.h"
#include "tm_is801b.h"
#include "tm_1x_up.h"
#include "tm_cm_iface.h"

/*=============================================================================

                                Definitions

=============================================================================*/

#define TM_1X_UP_SESS_TIMER_ID               (TM_1X_UP_IS801_DRV_TIMER_ID_BASE+1)
#define TM_1X_UP_IS801_TIMER_ID              (TM_1X_UP_IS801_DRV_TIMER_ID_BASE+2)
#define TM_1X_UP_CONNECT_TIMER_ID            (TM_1X_UP_IS801_DRV_TIMER_ID_BASE+3)
#define TM_1X_UP_DISCONNECT_TIMER_ID         (TM_1X_UP_IS801_DRV_TIMER_ID_BASE+4)
#define TM_1X_UP_MGP_TIMER_ID                (TM_1X_UP_IS801_DRV_TIMER_ID_BASE+5)
#define TM_1X_UP_TRUSTED_TIMER_DISPATCH_MASK ((TM_1X_UP_SESS_TIMER_ID)|(TM_1X_UP_IS801_TIMER_ID)|(TM_1X_UP_CONNECT_TIMER_ID)|(TM_1X_UP_DISCONNECT_TIMER_ID)|(TM_1X_UP_MGP_TIMER_ID)) 

#define TM_1X_UP_MAX_FIX_ERROR       1
#define TM_1X_UP_ERROR_THRESHOLD_MSA 1

#define TM_1X_UP_PR_TIMEOUT_MAX_MSA         30

#define TM_1X_UP_MGP_RESP_TIMEOUT           3*1000   /* 3secs in ms units. */
#define TM_1X_UP_CONNECT_TIMEOUT            60*1000   /* 60 secs in ms units */
#define TM_1X_UP_DISCONNECT_TIMEOUT         60*1000   /* 60 secs in ms units */
#define TM_1X_UP_FORCE_DORMANCY_RECONNECT_TIMEOUT (60*1000)   /* 60 secs in ms units */

#define TM_1X_UP_OPTIMAL_MODE_FIX_TIMEOUT      12  /* 12 secs */
#define TM_1X_UP_ACCURACY_THRESHOLD            50  /* 50 meters */

#define TM_1X_UP_DORMANCY_EVENT_MASK_NONE                   0x0000
#define TM_1X_UP_DORMANCY_EVENT_MASK_RECVD_TC_EXIT          0x0001
#define TM_1X_UP_DORMANCY_EVENT_MASK_RECVD_PHY_LINK_DOWN    0x0002
typedef char  tm_1x_up_dormancy_event_mask_type;

typedef enum 
{
  TM_1X_UP_DORMANCY_FAILURE_TYPE_ABORT = 0,
  TM_1X_UP_DORMANCY_FAILURE_TYPE_OPN_COMPLETE
} tm_1x_up_dormancy_failure_e_type;

/* MSBased aiding throttling feature. This feature throttles MS-PDE 
** connections to download Aiding data, AFLT seed, Eph & Almanac.
** MS starts with a min back-off and goes up to max back-off, increasing
** the interval by back off factor. If the requests shows up on time 
** after the safe duration, then the back off time is reset back to min.
*/
#define GPSONE_MSB_THROTTLE_ENABLE TRUE
#define GPSONE_MSB_THROTTLE_DISABLE FALSE
#define BACK_OFF_FACTOR 4             /* Exp back-off by a factor of 4 */
#define BACK_OFF_MIN_DURATION 60      /* Min duration is 1 min */
#define BACK_OFF_MAX_DURATION 900     /* Max is 15 min's */
#define BACK_OFF_RESET_DURATION 900   /* Safe value is 15 min's */
  
/*=============================================================================

                                Macros

=============================================================================*/

/*
 * The following bit masks represent the generic assistance data that 1x UP 
 * desires to get from the PDE. This bit mask is set only once at the start of
 * each session.
 *
 * Depending on NV configuration, and Base Station Capabilities, some of the
 * requests may not be sent at all.
 */
typedef uint32  tm_1x_up_rlink_req_type;
#define TM_1X_UP_RLINK_REQ_NONE       0x0000
#define TM_1X_UP_RLINK_REQ_GPS_EPH    0x0001
#define TM_1X_UP_RLINK_REQ_GPS_ALM    0x0002
#define TM_1X_UP_RLINK_REQ_GPS_SA     0x0004
#define TM_1X_UP_RLINK_REQ_GLO_EPH    0x0008
#define TM_1X_UP_RLINK_REQ_GLO_ALM    0x0010
#define TM_1X_UP_RLINK_REQ_AA         0x0020
#define TM_1X_UP_RLINK_REQ_LR         0x0040
#define TM_1X_UP_RLINK_REQ_ADV_BSC    0x0080  /* BS Capabilities   */
#define TM_1X_UP_RLINK_REQ_GGTO       0x0100  /* GPS-Glo time bias */
#define TM_1X_UP_RLINK_REQ_ADV_SVHI   0x0200  /* GPS Satellite Health Information */
#define TM_1X_UP_RLINK_REQ_DGNSS_ASST 0x0400  /* DGNSS assistance for Glonass */

/*=============================================================================

                                Variables

=============================================================================*/
/* Typedef enum TM_1X_UP_MODE_E_TYPE indicates which mode is active for TM_1X_UP.
*/
typedef enum {
    TM_1X_UP_MODE_NONE = 0,
    TM_1X_UP_MODE_MSA,                    /* Ms-Assisted Fix */
    TM_1X_UP_MODE_MSB,                     /* Ms-Based Fix */
    TM_1X_UP_MODE_DLOAD,                  /* Navigation Data Download via PDE */
    TM_1X_UP_MODE_MAX
} tm_1x_up_mode_e_type;

typedef enum
{
  EPHEMERIS_DL = 0x01,
  ALMANAC_DL = 0x02,
  LOCATION_DL = 0x04
} event_gps_data_download_payload_e_type;

typedef enum
{
  TM_1X_UP_OP_NONE = 0,
  /* Send 801-1 reverse link responses, MSI, PRDM, PPM, Ref loc, TOM, PRM to PDE */
  RL_RESP_MSI,        
  RL_RESP_PRDM,            /* Prop Data msg, aka System Parameter Information (SPI) */
  RL_RESP_PPM,             /* pilot phase measurement (PPM) */
  RL_RESP_REF_LOC,         /* send seed/reference location stored in msm to pde */
  RL_RESP_TOM,             /* time offset measurement (from pilot phase meas) */
  RL_RESP_PRM_FRM_BUF,     /* Send rev link prefix PRM response previously stored in buffer */

  /* Send 801-B reverse link responses, MSI, SPI, PPM, Ref Loc, TOM, PPM, PRM to PDE */
  RL_RESP_B_MSI,
  RL_RESP_B_SPI,
  RL_RESP_B_PPM,
  RL_RESP_B_REF_LOC_FRM_BUF,
  RL_RESP_B_TOM,
  RL_RESP_B_PPM_FRM_BUF,     /* Send rev link prefix PPM response previously stored in buffer */
  RL_RESP_B_PRM_FRM_BUF,

  RL_REQ_ASST_DATA_1,      /* Send an 801-1 rev link request required assistance data (eph/alm/LR/AA/) */
  RL_REQ_ASST_DATA_B,      /* Send an 801-B rev link request required assistance data (eph/alm/LR/AA/) */
  RL_REQ_B_BSC,            /* Send an 801-B rev link request advanced base station capabilities        */

  /* request PPM/PRM from MGP */
  MGP_REQ_LR,              /* req LR from MGP PE */
  MGP_REQ_PPM,             /* req PPM measurements start */
  MGP_GET_PPM,             /* get PPM measurements */
  MGP_STOP_PPM,            /* stop PPM measurements */

  MGP_START_PRM,           /* start PRM measurements from MGP (engine on) */
  MGP_GET_PRM,             /* get PRM measurements from MGP */
  MGP_STOP_PRM,            /* stop PRM measurements */

  MGP_GET_REF_LOC,         /* get seed/reference location from MGP to send to PDE */

  /* send information to MGP */
  MGP_POST_PREFIX_PPM_BUF, /* send prefix ppm measurements to mgp (sent along with ref loc from PDE) */
  MGP_POST_SEED,           /* post ref location rcvd from PDE to MGP */
  MGP_POST_AA_AND_SV_DIR,  /* post acquisition assistance and sv directions */
  MGP_POST_SA_PTR,         /* post sensitivity assistance */
  MGP_POST_EPH_IONO,       /* post ephemeris and ionosphere */
  MGP_POST_ALM,            /* post almanac to MGP */
  MGP_POST_GLO_ALM,        /* post Glonass Alm to MGP */
  MGP_POST_GLO_EPH,        /* post Glonass Eph to MGP */
  MGP_POST_GGTO,           /* post GPS-Glonass time offset to MGP */
  MGP_POST_SVHI,           /* post SV Health Info to MGP */

  /* IS801 state machine operations (change state of state machine in some way) */
  SAVE_PPM_TO_BUF,         /* save prefix ppm to local buffer (later sent to mgp when ref locn is rcvd from pde */
  SAVE_PRM_TO_BUF,
  SAVE_SEED,               /* save ref locn from pde in case fix fails and session initiator will accept ref locn */
  SAVE_REF_LOC,            /* save ref locn from mgp, to send to PDE as 801-B ref locn, for MSB type E callflow */
  CLR_REQ_MASK,            /* clear appropriate bit from mask in state representing request elts from pde. If all 
                              responses rcvd, stop IS801 timer */
  /* protocol operations */
  CR_PDDM,                 /* create PDDM */
  TX_PDDM,                 /* Transmit PDDM to pde, start IS801 response timer if required */
  OPS_END
} is801_operation;

#define MAX_IS801_STATE_MACHINE_OPERATIONS  25

typedef uint8 pddm_op_mask;
#define TM_1X_UP_SESS_START_MASK  TM_1X_IS801B_SESS_START_MASK
#define TM_1X_UP_SESS_END_MASK    TM_1X_IS801B_SESS_END_MASK
#define TM_1X_UP_SESS_SRC_MASK    TM_1X_IS801B_SESS_SRC_MASK
#define TM_1X_UP_IS801_TIMER_MASK 0x10
#define TM_1X_UP_IS801_1_PDDM     0x08
#define TM_1X_UP_IS801_B_PDDM     0x04

typedef enum
{
  MSB_START = 0,
  DLOAD_START = 1
} tm_1x_up_msb_op_e_type;

typedef struct
{
  tm_1x_up_mode_e_type               mode;               /* whether state machine is in MSA session, DATA DLOAD session or MSB session. Whats 
                                                            known as MSB in the external world is actually two back to back 1xup sessions, 
                                                            DATA_DLOAD followed by a 'stand alone' (called MSB internally in 1xup)
							 */

  uint32                             req_to_send;        /* req/resp_to_send are set in drv_set_mode at start of a session and cleared 1 by 1 */
  uint32                             resp_to_send;       /* as response/rej response come in from the PDE, in handle_assistance_data          */
  uint32                             req_to_send_801b;
  uint32                             resp_to_send_801b;
  byte                               err_count_fix;      /* cumulative err count fix                                                          */
  byte                               err_count_fix_msa;  /* when this exceeds threshold and we are in optimal modes, we switch modes to MSB   */
  byte                               err_count_fix_msb;  /* when this exceeds threshold and we are in optimal modes, we switch modes to MSA   */
  tm_1x_up_mode_e_type               last_error;         /* last_error was MSA/MSB/DATA_DLOAD                                                 */
  boolean                            waiting_msb_lr;     /* whether we are waiting for LR from PE or not                                      */
  boolean                            pending_fallback_msa_fix;
} tm_1x_up_mode_info_s_type;

typedef struct 
{
  boolean                      bs_info_update_reqd;
  qword                        ts;
} tm_1x_up_bs_info_s_type;

typedef enum tm_1x_up_session_type_e 
{
    TM_1X_UP_SESSION_TYPE_NONE = -1,       /* For internal use */
    TM_1X_UP_SESSION_TYPE_AFLT,            /* AFLT only session type */
    TM_1X_UP_SESSION_TYPE_HYBRID,          /* Hybrid Session going on */
    TM_1X_UP_SESSION_TYPE_GPS,             /* Session type GPS only */
    TM_1X_UP_SESSION_TYPE_MAX,             /* Internal use only */
    TM_1X_UP_SESSION_TYPE_E_SIZE = 0x10000000/* Fix enum size as inty */
} tm_1x_up_session_type_e_type;

/* This structre saves all the session information 
*/
typedef struct 
{
  prtl_start_sess_req_param_s_type pd_api_info; 
  tm_1x_up_session_type_e_type     session_type;   /* PD Session type */
} tm_1x_up_session_api_param_s_type;

/*
** TM_1X_UP Session information.
*/
typedef enum tm_1x_up_session_state_e 
{
    TM_1X_UP_SESSION_STATE_NONE,
    TM_1X_UP_SESSION_STATE_INIT,                    /* Init state */
    TM_1X_UP_SESSION_STATE_WAIT_FD,                 /* Waiting for Force Dormancy state */
    TM_1X_UP_SESSION_STATE_ENTRY,                   /* TM_1X_UP entry state, Connection estb, PPM started */
    TM_1X_UP_SESSION_STATE_WAIT_AA,                 /* Waiting for AA from PDE */
    TM_1X_UP_SESSION_STATE_WAIT_SA,                 /* Waiting for SA from PDE */
    TM_1X_UP_SESSION_STATE_WAIT_PR,                 /* Waiting for PR from Searcher */
    TM_1X_UP_SESSION_STATE_PR,                      /* IN PR state waiting for post-fix PPM  */
    TM_1X_UP_SESSION_STATE_WAIT_PD,                 /* Waiting for Position Location response from PDE */
    TM_1X_UP_SESSION_STATE_WAIT_NEXT_EVENT,         /* Wait for next event */
    TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP,         /* Waiting for data download operation to complete */
    TM_1X_UP_SESSION_STATE_WAIT_REF_LOC,            /* Waiting for ref location from PE */
    TM_1X_UP_SESSION_STATE_MAX,                     /* For internal use */
    TM_1X_UP_SESSION_STATE_E_SIZE = 0x10000000      /* Fix enum size as int */
} tm_1x_up_session_state_e_type;

typedef struct
{
  boolean                      gps_eph_validity;
  boolean                      gps_alm_validity;
  float                        pos_unc;
  float                        time_unc;   
  uint32                       gps_alm_delivered_sv_list; /* bit map of alm. delivered; SV1 on Bit 0 */
  uint32                       gps_eph_delivered_sv_list; /* bit map of eph. delivered; SV1 on bit 0 */
  uint32                       gps_eph_requested_sv_list; /* bit map of EPH requested from the server. SV1 on bit 0 */

  uint32                       glo_alm_delivered_sv_list; /* bit map of alm. delivered; SV1 on Bit 0 */
  uint32                       glo_eph_delivered_sv_list; /* bit map of eph. delivered; SV1 on bit 0 */
  uint32                       glo_eph_requested_sv_list; /* bit map of EPH requested from the server. SV1 on bit 0 */
  boolean                      glo_eph_validity;
  boolean                      glo_alm_validity;
} tm_1x_up_aiding_data_status_s_type;

/* typedef struct pdsm_mgp_req_state_type: Used to indicate the current status
** of MGP regarding PPM and PR.
*/
typedef struct {
  boolean ppm_on;    /* Indicate if PPM is going on or not */
  boolean prm_on;     /* Indicate if PR is going on or not */
} tm_1x_up_mgp_req_state_type;

typedef struct 
{
    tm_sess_handle_type                sess_handle;
    boolean                            tm_core_initiate_session;
    tm_1x_up_session_state_e_type      session_state;  /* Session State */
    tm_1x_up_mode_info_s_type          mode_info;
    tm_1x_up_mgp_req_state_type        mgp_info;
    tm_1x_up_bs_info_s_type            bs_info;
    tm_sess_stop_reason_e_type         stop_reason;
    qword                              lr_timestamp;

    tm_1x_up_session_api_param_s_type  api_param;

    boolean                            dload_activated;
    boolean                            is801_eph_reject;
    tm_1x_up_aiding_data_status_s_type aiding_data_status;
    uint32                             is801_req_wait_bmap;

    os_TimerMsgType*                   tm_1x_up_sess_timer;      /* MGP timeout */
    os_TimerMsgType*                   tm_1x_up_mgp_timer;       /* PPM Timer */
    os_TimerMsgType*                   tm_1x_up_is801_timer;     /* IS-801 response timeout */
    os_TimerMsgType*                   tm_1x_up_connect_timer;     /*  Connection timeout */
    os_TimerMsgType*                   tm_1x_up_disconnect_timer;     /*  Disconnection timeout */

    tm_cm_register_events_bmap_e_type  tm_cm_register_bmap;
    tm_1x_up_dormancy_event_mask_type  dormancy_event_mask;
    is801_fwd_link_pddm_info_s_type    fwd_link_pddm;

    boolean                            bs_capabilities_valid;
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#endif
    boolean                            sess_end_sent;

} tm_1x_up_session_info_s_type;

/* structure storing pointers to 801-1 or 801-B GPS ephemeris response */
typedef struct 
{
  uint8 prot;
  union
  {
    is801_eph_resp_type     *eph_801_1;
    is801b_flink_ie_gps_eph *eph_801_B;
  } u;
}is801_unified_gps_eph_resp_type;

/* structure storing pointers to 801-1 or 801-B GPS almanac response */
typedef struct 
{
  uint8 prot;
  union
  {
    is801_alm_resp_type     *alm_801_1;
    is801b_flink_ie_gps_alm *alm_801_B;
  } u;
}is801_unified_gps_alm_resp_type;

/* structure storing pointers to 801-1 or 801-B GPS AA response */
typedef struct 
{
  uint8 prot;
  union
  {
    is801_aa_resp_type      *aa_801_1;
    is801b_flink_ie_gnss_aa *aa_801_B;
  } u;
}is801_unified_aa_resp_type;

/* structure storing pointers to 801-1 or 801-B GPS SA response */
typedef struct 
{
  uint8 prot;
  union
  {
    is801_sa_resp_type      *sa_801_1;
    is801b_flink_ie_gps_sa  *sa_801_B;
  } u;
}is801_unified_gps_sa_resp_type;

/* structure storing pointers to 801-1 or 801-B GPS AA response */
typedef struct 
{
  uint8 prot;
  union
  {
    is801_lr_resp_type      *lr_801_1;
    is801b_flink_ie_adv_lr  *lr_801_B;
  } u;
}is801_unified_loc_resp_type;

extern tm_1x_up_session_info_s_type  tm_1x_up_session_info;  /* 1X UP session information */
extern tm_1x_up_config_s_type        tm_1x_up_config;     /* Config info from NV */
extern is801_lr_resp_type            *ref_position_ptr;

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_session_active

DESCRIPTION
  This function is used to indicate if session is active or not.

RETURN VALUE
  TRUE if Active, else FALSE.

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_session_active( void );

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_start_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_up_is801_drv_start_ppm(void);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_end_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_up_is801_drv_end_ppm(void);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_start_fallback_msa_fix

DESCRIPTION

RETURN VALUE
  pdsm_pd_end_e_type

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_start_fallback_msa_fix(void);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_grant_session

DESCRIPTION
  This function is used to handle the state when we are connected
  with PDE.

   For Call flow 1, 2, 3: Connected to PDE is trigger to REQ_PPM to TM-Core 
   For Call flow 7: Conencted to PDE is trigger to REQ_LR to TM-Core
   
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_handle_grant_session
(
  boolean sess_start
);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_prm

DESCRIPTION
  This function is used to handle GPS results from Searcher. Session Manager post-
  processes the results and creates a IS-801 format message. The packet is send
  as IS-801 to PDE and Session Manager waits for Position Location response.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

boolean tm_1x_up_is801_drv_handle_prm( sm_GpsMeasRptStructType  *info_ptr);


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_initiate_session

DESCRIPTION
  This function is used to initiate a session or continue with a tracking session.
  If MS is already connected with PDE then PPM results are requested in MSA mode
  or MGP is started in MSB mode.

RETURN VALUE
  Boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_initiate_session(void);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_end_session

DESCRIPTION   This function tells TM-Core that session is ended.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_end_session(tm_sess_stop_reason_e_type stop_reason);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_send_and_log_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
extern boolean tm_1x_up_is801_drv_send_and_log_rlink_pddm(void);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_dormancy_success

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_handle_dormancy_success(void);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_dormancy_failure

DESCRIPTION
  Force dormancy attempt is failed, usually due to QNC or voice call.
  Remember the force dormancy is failed and therefore BS Info is
  stale. Fall back to 1X UP state machine to select next possible mode
  or bail out.

RETURN VALUE
  None

DEPENDENCIES
  None.

===========================================================================*/
extern void tm_1x_up_is801_drv_handle_dormancy_failure(tm_1x_up_dormancy_failure_e_type failure_type);

#if defined(FEATURE_CALL_THROTTLE)
/*===========================================================================

FUNCTION tm_1x_up_set_throttling_backoff_time

DESCRIPTION
  This function is used to set pdsm throttling backoff time

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_set_throttling_backoff_time(void);
#endif

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_cleanup

DESCRIPTION   

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_cleanup(boolean abort_connection);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_comm_error

DESCRIPTION
  This function is called when there is an Error during communcation w/ PDE. 
  
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_1x_up_is801_drv_handle_comm_error(void);

boolean tm_1x_up_is801_drv_create_rlink_req(is801_rev_link_req_mask_type req, boolean is_801_1);
void    tm_1x_up_is801_drv_end_session(tm_sess_stop_reason_e_type stop_reason);
boolean tm_1x_up_is801_drv_get_ppm(void);
int     tm_1x_up_is801_drv_create_rlink_pddm(byte sess_start, byte sess_end);
boolean tm_1x_up_is801_1_drv_send_rlink_pddm(byte sess_start, byte sess_end,  uint8 num_pddm);
boolean tm_1x_up_is801_drv_continue_msb_fix(tm_1x_up_msb_op_e_type);
boolean tm_1x_up_is801_drv_start_prm(void);
void tm_1x_up_is801_drv_update_session_state(tm_1x_up_session_state_e_type new_state);

boolean tm_1x_up_execute_fns(const is801_operation ops[], void *params[]);
boolean tm_state_transition(const is801_operation ops[], void *params[], tm_1x_up_session_state_e_type next_state);
void    tm_state_machine_execution_clean_up(void);

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_send_rlink_pddm

DESCRIPTION: This function sends all pending requests and responses. First
             a pddm is created, and then its handed over to 1x up comm for
             sending it to the PDE.

             In case the session start flag is set, all 801-1 PDDMs and 1 
             801-B PDDM is sent. Subsequent calls to this function create
             PDDMs one by one, first 801-1 and then 801-B, and sends the 
             PDDMs one by one.

ARGUMENTS
  sess_flags : pointer to session flags
  tx_is801_b : whether to send is801-b pddms.
  tx_is801_1 : whether to send is801-1 pddms.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_send_rlink_pddm
(
  const byte*    sess_flags,
  boolean        tx_is801_b,
  boolean        tx_is801_1
);
/*===========================================================================

FUNCTION tm_1x_up_is801_b_drv_create_rlink_pddm

DESCRIPTION
This function will create IS801B reverse link pddm(s) from the reqs and resp 
messages passed to the encoder decoder layer earlier. If more than one pddms
are created, the first pddm to be created will have the flags set in sess_flags
parameter, others wont. 

RETURN VALUE
  int

DEPENDENCIES

===========================================================================*/
int tm_1x_up_is801_b_drv_create_rlink_pddm(const byte *sess_flags);

/*===========================================================================

FUNCTION tm_1x_up_is801_b_drv_send_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES
===========================================================================*/
boolean tm_1x_up_is801_b_drv_send_rlink_pddm(byte sess_flags,  uint8 num_pddm);

/*===========================================================================

FUNCTION    : i_1x_up_is801b_drv_send_pddm

DESCRIPTION : Internal Function to get ONE is801b pddm from the encoder layer
              and send it to tm_1x_up_comm, to be sent to the PDE.

DEPENDENCIES

ARGUMENTS
RETURN VALUE
SIDE EFFECTS
Gets one is801b pddm from encoder.
Frees the is801b pddm that it got from the encoder
===========================================================================*/
boolean i_1x_up_is801b_drv_send_pddm(void);

/*===========================================================================

FUNCTION    : i_1x_up_is801_1_drv_send_pddm

DESCRIPTION : Internal Function to get ONE is8011 pddm from the encoder layer
              and send it to tm_1x_up_comm, to be sent to the PDE.

DEPENDENCIES

ARGUMENTS
RETURN VALUE
SIDE EFFECTS
Gets one is801b pddm from encoder.
Frees the is801b pddm that it got from the encoder
===========================================================================*/
boolean i_1x_up_is801_1_drv_send_pddm(void);

#ifdef __cplusplus
}
#endif

#endif /* TM_1X_UP_TRUSTED_H */

